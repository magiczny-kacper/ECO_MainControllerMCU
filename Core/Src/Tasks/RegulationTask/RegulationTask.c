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
#include "../../ConfigEEPROM/config.h"

typedef union{
	struct{
		uint8_t input1 	:1;
		uint8_t input2 	:1;
		uint8_t input3 	:1;
		uint8_t input4 	:1;
		uint8_t input5 	:1;
		uint8_t input6 	:1;
		uint8_t input7 	:1;
		uint8_t input8 	:1;
		uint8_t input10	:1;
		uint8_t input9 	:1;
		uint8_t output1 :1;
		uint8_t output2 :1;
		uint8_t output3 :1;
		uint8_t output4 :1;
		uint8_t output5 :1;
		uint8_t output6 :1;
	}signals;

	struct{
		uint8_t portA;
		uint8_t portB;
	}ports;

	uint8_t bytes[2];
	uint16_t word;
} Expander_signals_t;

extern osThreadId PowerRegulationHandle;

extern TIM_HandleTypeDef htim1, htim2, htim3;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

struct counter_data counter;
MCP23017_HandleTypeDef expander1;
Expander_signals_t IOsignals;

#define DEC_PLACES 					10.0
#define ACUMULATED_POWER_FACTOR 	0
#define NET_POWER 					0
#define PHASE_POWER 				0
#define PHASE_POWER_COEFF 			0
#define CO_HEATER_PHASE_POWER 		1
#define CWU_HEATER_PHASE_POWER 		1

#define BOJLER_HIGH 				0
#define BOJLER_LOW 					0

#define CO_HIGH						0
#define CO_LOW 						0

#define MAIN_SWITCH 				0
#define MAIN_SWITCH_SET 			0
#define MAIN_SWITCH_RESET 			0

#define CO_FUSE 					0
#define	CO_FUSE_SET					0
#define CO_FUSE_RESET				0

#define CWU_FUSE 					0
#define CWU_FUSE_SET				0
#define CWU_FUSE_RESET				0

#define CO_HEATER					0
#define CO_HEATER_SET				0
#define CO_HEATER_RESET				0

#define CWU_HEATER					0
#define CWU_HEATER_SET				0
#define CWU_HEATER_RESET			0

#define CO_CONTACTOR_ERROR			0
#define CO_CONTACTOR_ERROR_SET		0
#define CO_CONTACTOR_ERROR_RESET	0

#define CWU_CONTACTOR_ERROR			0
#define CWU_CONTACTOR_ERROR_SET		0
#define CWU_CONTACTOR_ERROR_RESET	0

#define EXP_STATE 					0

#define MAIN_SW_EXPPIN 				0
#define CWU_F_EXPPIN 				1
#define CO_F_EXPPIN 				2
#define CWU_C_EXPPIN 				3
#define CO_C_EXPPIN 				4

#define MAIN_SW_STATE 				(IOsignals.signals.input1)
#define CWU_F_STATE 				(IOsignals.signals.input2)
#define CO_F_STATE 					(IOsignals.signals.input3)
#define CWU_C_STATE 				(IOsignals.signals.input9)
#define CO_C_STATE 					(IOsignals.signals.input10)

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

	RegulationConfig_t parameters;
	CONFStatus_t configStatus;
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

	configStatus = Config_GetRegConfig(&parameters);

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

	TIM1 -> CCR1 = 0;
	TIM1 -> CCR2 = 0;
	TIM1 -> CCR3 = 0;

	TIM2 -> CCR1 = 0;
	TIM2 -> CCR2 = 0;
	TIM2 -> CCR3 = 0;

	TIM3 -> CCR1 = 0;
	TIM3 -> CCR2 = 0;
	TIM3 -> CCR3 = 0;

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

#ifdef __DEBUG
	printf("%d PowerRegulation Task Loop...\n", HAL_GetTick());
#endif
	for(;;){

		mcp23017_read_gpio(&expander1, 1);
		memcpy(&IOsignals, &expander1.gpio, 2);
		receieves = 0;

		if(MAIN_SW_STATE == GPIO_PIN_SET) MAIN_SWITCH_SET;
		else MAIN_SWITCH_RESET;

		if(CWU_F_STATE == GPIO_PIN_SET) CWU_FUSE_SET;
		else CWU_FUSE_RESET;

		if(CO_F_STATE == GPIO_PIN_SET) CO_FUSE_SET;
		else CO_FUSE_RESET;

		if(CWU_C_STATE != CWU_HEATER) CWU_CONTACTOR_ERROR_SET;
		else CWU_CONTACTOR_ERROR_RESET;

		if(CO_C_STATE != CO_HEATER) CO_CONTACTOR_ERROR_SET;
		else CO_CONTACTOR_ERROR_RESET;

		if(vModbusReadInputRegisters(&mbPort, COUNTER_ADDRESS, L1_VOLTAGE, 18, counter_data_raw) == Modbus_OK){
			receieves += 1;
			for(uint8_t i = 0; i < 3; i++){
				counter.voltages[i] = fModbusParseFloat(counter_data_raw + i * 4);
				counter.currents[i] = fModbusParseFloat(counter_data_raw + 12 + i * 4);
				counter.powers[i] = fModbusParseFloat(counter_data_raw + 24 + 4 * i);
			}
		}

		if(vModbusReadInputRegisters(&mbPort, COUNTER_ADDRESS, IMPORTED_ACTIVE_POWER, 4, imported_exported_power_raw) == Modbus_OK){
			receieves += 2;
			last_exported_power = exported_power;
			last_imported_power = imported_power;
			imported_power = fModbusParseFloat(imported_exported_power_raw);
			exported_power = fModbusParseFloat(imported_exported_power_raw + 4);

			if(counter.power_in_buffor > 0){
				counter.power_in_buffor -= (imported_power - last_imported_power);
			}else{
				counter.power_in_buffor = 0;
			}
			counter.power_in_buffor += (exported_power - last_exported_power) * ACUMULATED_POWER_FACTOR;
		}

		if(vModbusReadInputRegisters(&mbPort, TEMPERATURE_ADDRESS, 0, SENSOR_NUMBER, temperatures_raw) == Modbus_OK){
			receieves += 4;
			for(uint8_t i = 0; i < SENSOR_NUMBER; i++){
				int16_t temp = temperatures_raw[i * 2];
				temp = temp << 8;
				temp += temperatures_raw[i*2 + 1];
				temperatures[i] = temp / DEC_PLACES;
			}
			memcpy(&counter.CWU_temps[0], &temperatures[0], 24);
			/*counter.CWU_temps[0] = temperatures[0];
			counter.CWU_temps[1] = temperatures[1];
			counter.CO_temps[0] = temperatures[2];
			counter.CO_temps[1] = temperatures[3];
			counter.CO_temps[2] = temperatures[4];
			counter.CO_temps[3] = temperatures[5];*/
		}

		counter.counter_present = 1;
		counter.temperatures_present = 1;
		if(receieves == 7){
			for(uint8_t i = 0; i < 3; i ++){
				if(counter.voltages[i] > 200.0){
					if(counter.powers[i] > 0) available_power[i] = (PHASE_POWER - counter.powers[i]) * PHASE_POWER_COEFF;
					else available_power[i] = -counter.powers[i] * PHASE_POWER_COEFF;

					counter.CO_heater_max_power[i] = available_power[i] / CO_HEATER_PHASE_POWER;
					counter.CWU_heater_max_power[i] = available_power[i] / CWU_HEATER_PHASE_POWER;

					if(counter.CO_heater_max_power[i] > 1.0) counter.CO_heater_max_power[i] = 1.0;
					if(counter.CWU_heater_max_power[i] > 1.0) counter.CWU_heater_max_power[i] = 1.0;

					counter.CO_heater_PWM[i] = (uint32_t)(counter.CO_heater_max_power[i] * 1000.0);
					counter.CO_heater_PWM[i] -= (counter.CO_heater_PWM[i] % 10);

					counter.CWU_heater_PWM[i] = (uint32_t)(counter.CWU_heater_max_power[i] * 1000.0);
					counter.CWU_heater_PWM[i] -= (counter.CWU_heater_PWM[i] % 10);
				}else{
					counter.CO_heater_PWM[i] = 0;
					counter.CWU_heater_PWM[i] = 0;
				}
			}
			if(counter.CWU_temps[0] < BOJLER_HIGH){
				if(counter.CWU_temps[1] < BOJLER_LOW){
					CWU_HEATER_SET;
				}
			}else{
				CWU_HEATER_RESET;
			}

			if(counter.CO_temps[0] < CO_HIGH){
				if(counter.CO_temps[3] < CO_LOW){
					CO_HEATER_SET;
				}
			}else{
				CO_HEATER_RESET;
			}

			if(MAIN_SWITCH){
				if(CWU_HEATER){
					if(CO_HEATER){
						for(int i = 0; i < 3; i++){
							float power = available_power[i] - counter.CWU_heater_max_power[i] * CWU_HEATER_PHASE_POWER;
							if(power > 0.0){
								counter.CO_heater_max_power[i] = power / CO_HEATER_PHASE_POWER;
								if(counter.CO_heater_max_power[i] > 1.0) counter.CO_heater_max_power[i] = 1.0;
								counter.CO_heater_PWM[i] = (uint32_t)(counter.CO_heater_max_power[i] * 1000.0);
								counter.CO_heater_PWM[i] -= (counter.CO_heater_PWM[i] % 10);
							}else{
								counter.CO_heater_PWM[i] = 0;
							}
						}
						if(!counter.CO_heater_PWM[0] && !counter.CO_heater_PWM[1] && !counter.CO_heater_PWM[2]){
							CO_HEATER_RESET;
						}
					}else{
						for(int i = 0; i < 3; i++){
							counter.CO_heater_PWM[i] = 0;
						}
					}
				}else if(CO_HEATER){
					for(int i = 0; i < 3; i++){
						counter.CWU_heater_PWM[i] = 0;
					}
				}else{
					for(int i = 0; i < 3; i++){
						counter.CWU_heater_PWM[i] = 0;
						counter.CO_heater_PWM[i] = 0;
					}
				}
			}else{
				CO_HEATER_RESET;
				CWU_HEATER_RESET;
				for(int i = 0; i < 3; i++){
					counter.CO_heater_PWM[i] = 0;
					counter.CWU_heater_PWM[i] = 0;
				}
			}

		}else{
			CO_HEATER_RESET;
			CWU_HEATER_RESET;
			for(int i = 0; i < 3; i++){
				counter.CO_heater_PWM[i] = 0;
				counter.CWU_heater_PWM[i] = 0;
			}

			if(!(receieves & 1) || !(receieves & 2)){
				counter.counter_present = 0;
			}

			if(!(receieves & 4)){
				counter.temperatures_present = 0;
			}
		}

		if(!CWU_FUSE){
			CO_HEATER_RESET;
			for(int i = 0; i < 3; i++){
				counter.CWU_heater_PWM[i] = 0;
			}
		}

		if(!CO_FUSE){
			CWU_HEATER_RESET;
			for(int i = 0; i < 3; i++){
				counter.CO_heater_PWM[i] = 0;
			}
		}

		TIM3 -> CCR1 = counter.CWU_heater_PWM[0];
		TIM3 -> CCR2 = counter.CWU_heater_PWM[1];
		TIM3 -> CCR3 = counter.CWU_heater_PWM[2];

		TIM1 -> CCR1 = counter.CO_heater_PWM[0];
		TIM1 -> CCR2 = counter.CO_heater_PWM[1];
		TIM1 -> CCR3 = counter.CO_heater_PWM[2];

		memcpy(&expander1.gpio, &IOsignals, 2);
		mcp23017_write_gpio(&expander1, MCP23017_PORTA);
		vTaskDelayUntil(&xLastWakeTime, xDelay);
	}
  /* USER CODE END PowerRegulation */
}
