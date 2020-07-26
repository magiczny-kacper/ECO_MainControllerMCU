/*
 * RegulationTask.c
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#include "RegulationTask.h"

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include "../../Modbus/Modbus.h"
#include "../../Expander/mcp23017.h"

extern osThreadId PowerRegulationHandle;

extern TIM_HandleTypeDef htim1, htim2, htim3;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

MCP23017_HandleTypeDef expander1;

RegTaskData_t regulationData;

#define HIGH 						1
#define LOW							0

#define DEC_PLACES 					10.0
#define ACUMULATED_POWER_FACTOR 	0
#define NET_POWER 					0
#define PHASE_POWER 				0
#define PHASE_POWER_COEFF 			0
#define CO_HEATER_PHASE_POWER 		1
#define CWU_HEATER_PHASE_POWER 		1

float fModbusParseFloat (uint8_t* in_data){
	union float_bytes buffor;

	buffor.bytes[3] = *in_data;
	in_data ++;
	buffor.bytes[2] = *in_data;
	in_data ++;
	buffor.bytes[1] = *in_data;
	in_data ++;
	buffor.bytes[0] = *in_data;

	return buffor.value;
}

void RegulationTask(void const * argument)
{
  /* USER CODE BEGIN PowerRegulation */
	ModbusHandler mbPort;
	TickType_t xLastWakeTime;

	const TickType_t xDelay = 1000;

	//Gotowe dane z licznika
	float imported_power = 0;
	float last_imported_power;
	float exported_power = 0;
	float last_exported_power;
	float temperatures[SENSOR_NUMBER];
	float available_power[3];

	//Surowe dane i zmienne pomocnicze
	uint8_t counter_data_raw[36];
	uint8_t imported_exported_power_raw[8];
	uint8_t receieves = 0;
	uint8_t temperatures_raw[SENSOR_NUMBER * 2];

	regulationData.configStatus = Config_GetRegConfig(&regulationData.parameters);

	HAL_TIM_PWM_Init(&htim1);
	HAL_TIM_PWM_Init(&htim2);
	HAL_TIM_PWM_Init(&htim3);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

	TIM1 -> CCR1 = 500;
	TIM1 -> CCR2 = 500;
	TIM1 -> CCR3 = 500;

	TIM2 -> CCR1 = 500;
	TIM2 -> CCR2 = 500;
	TIM2 -> CCR3 = 500;

	TIM3 -> CCR1 = 500;
	TIM3 -> CCR2 = 500;
	TIM3 -> CCR3 = 500;

	//Uchwyty i inne struktury
	mbPort.task =  xTaskGetCurrentTaskHandle();

	mcp23017_init(&expander1, &hi2c1, 0b0100111);
	mcp23017_iodir(&expander1, 1, 0xFF);
	mcp23017_iodir(&expander1, 0, 0x03);
	expander1.gpio[0] = 0b11111100;
	mcp23017_write_gpio(&expander1, 0);

	vTaskDelay(1000);

	expander1.gpio[0] = 0;
	mcp23017_write_gpio(&expander1, 0);
	vModbusInit(&mbPort, &huart2, 100);

	for(;;){

		mcp23017_read_gpio(&expander1, 0);
		mcp23017_read_gpio(&expander1, 1);

		regulationData.IOsignals.bytes[0] = expander1.gpio[0];
		regulationData.IOsignals.bytes[1] = expander1.gpio[1];

		receieves = 0;

		if(regulationData.IOsignals.signals.input2 != regulationData.IOsignals.signals.output1){
			regulationData.ControlWord.CWUHeaterError = HIGH;
		}else{
			regulationData.ControlWord.CWUHeaterError = LOW;
		}

		if(regulationData.IOsignals.signals.input3 != regulationData.IOsignals.signals.output2){
			regulationData.ControlWord.COHeaterError = HIGH;
		}else{
			regulationData.ControlWord.COHeaterError = LOW;
		}

		if(vModbusReadInputRegisters(&mbPort, COUNTER_ADDRESS, L1_VOLTAGE, 18, counter_data_raw) == Modbus_OK){
			receieves += 1;
			for(uint8_t i = 0; i < 3; i++){
				regulationData.counter.voltages[i] = fModbusParseFloat(counter_data_raw + i * 4);
				regulationData.counter.currents[i] = fModbusParseFloat(counter_data_raw + 12 + i * 4);
				regulationData.counter.powers[i] = fModbusParseFloat(counter_data_raw + 24 + 4 * i);
			}
		}

		if(vModbusReadInputRegisters(&mbPort, COUNTER_ADDRESS, IMPORTED_ACTIVE_POWER, 4, imported_exported_power_raw) == Modbus_OK){
			receieves += 2;
			last_exported_power = exported_power;
			last_imported_power = imported_power;
			imported_power = fModbusParseFloat(imported_exported_power_raw);
			exported_power = fModbusParseFloat(imported_exported_power_raw + 4);

			if(regulationData.counter.power_in_buffor > 0){
				regulationData.counter.power_in_buffor -= (imported_power - last_imported_power);
			}else{
				regulationData.counter.power_in_buffor = 0;
			}
			regulationData.counter.power_in_buffor += (exported_power - last_exported_power) * ACUMULATED_POWER_FACTOR;
		}

		if(vModbusReadInputRegisters(&mbPort, TEMPERATURE_ADDRESS, 0, SENSOR_NUMBER, temperatures_raw) == Modbus_OK){
			receieves += 4;
			for(uint8_t i = 0; i < SENSOR_NUMBER; i++){
				int16_t temp = temperatures_raw[i * 2];
				temp = temp << 8;
				temp += temperatures_raw[i*2 + 1];
				temperatures[i] = temp / DEC_PLACES;
			}
			memcpy(&regulationData.counter.CWU_temps[0], &temperatures[0], 24);
		}

		regulationData.counter.counter_present = 1;
		regulationData.counter.temperatures_present = 1;
		if(receieves == 7){
			/*
			 * Obliczenia dostępnej mocy dla grzałek na każdej fazie.
			 */
			for(uint8_t i = 0; i < 3; i ++){
				if(regulationData.counter.powers[i] > 0){
					available_power[i] = ((regulationData.parameters.net_max_power / 3.0f) - regulationData.counter.powers[i]) * regulationData.parameters.heater_power_coeff;
				}else{
					available_power[i]  = -regulationData.counter.powers[i] * regulationData.parameters.heater_power_coeff / 100.0f;
				}
				if(regulationData.parameters.CO_heater_power > 0.0f){
					regulationData.counter.CO_heater_max_power[i] = available_power[i] / regulationData.parameters.CO_heater_power / 3.0f;
					if(regulationData.counter.CO_heater_max_power[i] > 1.0f) regulationData.counter.CO_heater_max_power[i] = 1.0f;
				}else{
					regulationData.counter.CO_heater_max_power[i] = 0.0f;
				}
				if(regulationData.parameters.CWU_heater_power > 0.0f){
					regulationData.counter.CWU_heater_max_power[i] = available_power[i] / regulationData.parameters.CWU_heater_power / 3.0f;
					if(regulationData.counter.CWU_heater_max_power[i] > 1.0f) regulationData.counter.CWU_heater_max_power[i] = 1.0f;
				}else{
					regulationData.counter.CWU_heater_max_power[i] = 0.0f;
				}
				/*
				 * Konwersja z float na wypełnienie PWM, z uwzględnieniam napięcia fazy.
				 */
				if(regulationData.counter.voltages[i] > 200.0){
					regulationData.counter.CO_heater_PWM[i] = (uint32_t)(regulationData.counter.CO_heater_max_power[i] * 1000.0);
					regulationData.counter.CWU_heater_PWM[i] = (uint32_t)(regulationData.counter.CWU_heater_max_power[i] * 1000.0);
				}else{
					regulationData.counter.CO_heater_PWM[i] = 0;
					regulationData.counter.CWU_heater_PWM[i] = 0;
				}
			}
			/*
			 * Sprawdzenie progów temperatur dla obu zbiorników.
			 */
			if(regulationData.counter.CWU_temps[0] < regulationData.parameters.CWU_hi_temp){
				if(regulationData.counter.CWU_temps[1] < regulationData.parameters.CWU_lo_temp){
					regulationData.ControlWord.CWUHeaterStateOut = HIGH;
				}
			}else{
				regulationData.ControlWord.CWUHeaterStateOut = LOW;
			}

			if(regulationData.counter.CO_temps[0] < regulationData.parameters.CO_hi_temp){
				if(regulationData.counter.CO_temps[3] < regulationData.parameters.CO_lo_temp){
					regulationData.ControlWord.COHeaterStateOut = HIGH;
				}
			}else{
				regulationData.ControlWord.COHeaterStateOut = LOW;
			}
			/*
			 * Wyłącz CWU jeśli ma zostać włączone CO.
			 */
			if(regulationData.ControlWord.COHeaterStateOut){
				regulationData.ControlWord.CWUHeaterStateOut = LOW;
			}

			/*
			 * Sprawdzenie stanu włącznika i określenie ostatecznych wartości wypełnień.
			 */
			if(regulationData.IOsignals.signals.input1 == HIGH){
				if(regulationData.IOsignals.signals.input4 == LOW ||
					regulationData.ControlWord.CWUHeaterError == HIGH){
					regulationData.ControlWord.CWUHeaterStateOut = LOW;
				}
				if(regulationData.IOsignals.signals.input5 == LOW &&
					regulationData.ControlWord.COHeaterError == HIGH){
					regulationData.ControlWord.CWUHeaterStateOut = LOW;
				}

				if(regulationData.ControlWord.CWUHeaterStateOut == LOW){
					for(int i = 0; i < 3; i++){
						regulationData.counter.CWU_heater_PWM[i] = 0;
					}
				}

				if(regulationData.ControlWord.COHeaterStateOut == LOW){
					for(int i = 0; i < 3; i++){
						regulationData.counter.CO_heater_PWM[i] = 0;
					}
				}

			}else{
				regulationData.ControlWord.COHeaterStateOut = LOW;
				regulationData.ControlWord.CWUHeaterStateOut = LOW;
				for(int i = 0; i < 3; i++){
					regulationData.counter.CO_heater_PWM[i] = 0;
					regulationData.counter.CWU_heater_PWM[i] = 0;
				}
			}

		}else{

		}

		TIM3 -> CCR1 = regulationData.counter.CWU_heater_PWM[0];
		TIM3 -> CCR2 = regulationData.counter.CWU_heater_PWM[1];
		TIM3 -> CCR3 = regulationData.counter.CWU_heater_PWM[2];

		TIM1 -> CCR1 = regulationData.counter.CO_heater_PWM[0];
		TIM1 -> CCR2 = regulationData.counter.CO_heater_PWM[1];
		TIM1 -> CCR3 = regulationData.counter.CO_heater_PWM[2];

		expander1.gpio[MCP23017_PORTA] = regulationData.IOsignals.ports.portA;
		mcp23017_write_gpio(&expander1, 0);
		vTaskDelayUntil(&xLastWakeTime, xDelay);
	}
  /* USER CODE END PowerRegulation */
}

void RegulationTask_GetData (RegTaskData_t* destination){
	memcpy(destination, &regulationData, sizeof(RegTaskData_t));
}
