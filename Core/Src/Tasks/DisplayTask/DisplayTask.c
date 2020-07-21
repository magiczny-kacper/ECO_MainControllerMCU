/*
 * DisplayTask.c
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#include "DisplayTask.h"
#include "../../ConfigEEPROM/config.h"

#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include "../../Nextion/nextion.h"
#include "UART_DMA.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
extern RTC_HandleTypeDef hrtc;

#define NEXTION_SMALL_UART &huart1
#define NEXTION_BIG_UART &huart6

UARTDMA_HandleTypeDef nextion_big_comm;
UARTDMA_HandleTypeDef nextion_small_comm;
struct nextion nextion_small;
struct nextion nextion_big;

uint8_t command_correct[NEX_COMMANDS_COUNT];
uint8_t current_command;

static void nextion_SendData(UART_HandleTypeDef* uart, nextion_command cmd, int32_t value1, int32_t value2, int32_t value3);
void Nextion1_DataRcv (void);
void Nextion2_DataRcv (void);

static void nextion_SendData(UART_HandleTypeDef* uart, nextion_command cmd, int32_t value1, int32_t value2, int32_t value3){
	char buffor_to_send[40];
	uint8_t buffor_to_send_size;

	if(cmd < NEX_COMMANDS_COUNT && cmd > -1){
		if(cmd == NEX_WAKE || cmd == NEX_SENDME || cmd == NEX_SETTINGS_SAVED || cmd == NEX_SETTINGS_NOT_SAVED
			|| cmd == NEX_SETTINGS_LOCK || cmd == NEX_SETTINGS_UNLOCK){
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[cmd]);
		}else if(cmd == NEX_PHASE_VOLTAGE ||
			cmd == NEX_PHASE_CURRENT ||
			cmd == NEX_PHASE_POWER){
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[cmd], value1, value2, value3);
		}else if(cmd == NEX_PHASE_PRG_BAR ||
			cmd == NEX_CO_HEATER_PHASE_DUTY ||
			cmd == NEX_CWU_HEATER_PHASE_DUTY ||
			cmd == NEX_CWU_TEMP || cmd == NEX_CO_TEMP){
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[cmd], value1, value2);
		}else{
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[cmd], value1);
		}
		for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
			buffor_to_send[i] = 0xFF;
		}
		HAL_UART_Transmit(uart, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
	}
}

void DisplayTask(void const * argument)
{
  /* USER CODE BEGIN UpdateLCD */


	char buffor_to_send[40];
	uint8_t buffor_to_send_size;
	uint32_t ulNotificationValue;
	uint8_t licz;

	RTC_TimeTypeDef currTime;
	RTC_DateTypeDef currDate;

	ConfigStruct_t configuration;

	nextion_small.display = 30;
	nextion_small.last_display = 30;
	nextion_big.display = 30;
	nextion_big.last_display = 30;

	UARTDMA_Init(&nextion_small_comm, NEXTION_SMALL_UART);
	UARTDMA_Init(&nextion_big_comm, NEXTION_BIG_UART);

	nextion_big_comm.callback = Nextion1_DataRcv;
	nextion_small_comm.callback = Nextion2_DataRcv;

	Config_GetConfig(&configuration);

	vTaskDelay(1000);

	HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BIN);

	current_command = NEX_WAKE;
	nextion_SendData(NEXTION_SMALL_UART, NEX_WAKE, 0, 0, 0);
	vTaskDelay(1);

	current_command = NEX_SENDME;
	nextion_SendData(NEXTION_SMALL_UART, NEX_SENDME, 0, 0, 0);
	vTaskDelay(1);
	ulNotificationValue = ulTaskNotifyTake(pdTRUE, 1000);

	current_command = NEX_WAKE;
	nextion_SendData(NEXTION_BIG_UART, NEX_WAKE, 0, 0, 0);
	vTaskDelay(1);

	current_command = NEX_SENDME;
	nextion_SendData(NEXTION_BIG_UART, NEX_SENDME, 0, 0, 0);
	vTaskDelay(1);
	ulNotificationValue = ulTaskNotifyTake(pdTRUE, 1000);


	if(nextion_big.display != 30 || nextion_small.display != 30){
		if(nextion_big.display != 30){
			// Rok
			current_command = NEX_YEAR;
			nextion_SendData(NEXTION_BIG_UART, NEX_YEAR, currDate.Year + 2000, 0, 0);

			// Miesiąc
			current_command = NEX_MONTH;
			nextion_SendData(NEXTION_BIG_UART, NEX_MONTH, currDate.Month, 0, 0);

			// Dzień
			current_command = NEX_DAY;
			nextion_SendData(NEXTION_BIG_UART, NEX_DAY, currDate.Date, 0, 0);

			// Godzina
			current_command = NEX_HOUR;
			nextion_SendData(NEXTION_BIG_UART, NEX_HOUR, currTime.Hours, 0, 0);

			// Minuta
			current_command = NEX_MINUTE;
			nextion_SendData(NEXTION_BIG_UART, NEX_MINUTE, currTime.Minutes, 0, 0);

			// Sekundy
			current_command = NEX_SECONDS;
			nextion_SendData(NEXTION_BIG_UART, NEX_SECONDS, currTime.Seconds, 0, 0);
		}

		if(nextion_small.display != 30){
			nextion_SendData(NEXTION_SMALL_UART, NEX_CO_HIGH_TEMP, configuration.RegulationConfig.CO_hi_temp, 0, 0);
			nextion_SendData(NEXTION_SMALL_UART, NEX_CO_LOW_TEMP, configuration.RegulationConfig.CO_lo_temp, 0, 0);
			nextion_SendData(NEXTION_SMALL_UART, NEX_CO_HEATER_POWER, (int32_t)configuration.RegulationConfig.CO_heater_power, 0, 0);
			nextion_SendData(NEXTION_SMALL_UART, NEX_CWU_HIGH_TEMP, configuration.RegulationConfig.CWU_hi_temp, 0, 0);
			nextion_SendData(NEXTION_SMALL_UART, NEX_CWU_LOW_TEMP, configuration.RegulationConfig.CWU_lo_temp, 0, 0);
			nextion_SendData(NEXTION_SMALL_UART, NEX_CWU_HEATER_POWER, (int32_t)configuration.RegulationConfig.CWU_heater_power, 0, 0);
			nextion_SendData(NEXTION_SMALL_UART, NEX_ACC_COEFF, configuration.RegulationConfig.acumulated_power_coeff, 0, 0);
			nextion_SendData(NEXTION_SMALL_UART, NEX_HEATER_COEFF, configuration.RegulationConfig.heater_power_coeff, 0, 0);
		}

		if(nextion_big.display != 30){
			nextion_SendData(NEXTION_BIG_UART, NEX_CO_HIGH_TEMP, configuration.RegulationConfig.CO_hi_temp, 0, 0);
			nextion_SendData(NEXTION_BIG_UART, NEX_CO_LOW_TEMP, configuration.RegulationConfig.CO_lo_temp, 0, 0);
			nextion_SendData(NEXTION_BIG_UART, NEX_CO_HEATER_POWER, (int32_t)configuration.RegulationConfig.CO_heater_power, 0, 0);
			nextion_SendData(NEXTION_BIG_UART, NEX_CWU_HIGH_TEMP, configuration.RegulationConfig.CWU_hi_temp, 0, 0);
			nextion_SendData(NEXTION_BIG_UART, NEX_CWU_LOW_TEMP, configuration.RegulationConfig.CWU_lo_temp, 0, 0);
			nextion_SendData(NEXTION_BIG_UART, NEX_CWU_HEATER_POWER, (int32_t)configuration.RegulationConfig.CWU_heater_power, 0, 0);
			nextion_SendData(NEXTION_BIG_UART, NEX_ACC_COEFF, configuration.RegulationConfig.acumulated_power_coeff, 0, 0);
			nextion_SendData(NEXTION_BIG_UART, NEX_HEATER_COEFF, configuration.RegulationConfig.heater_power_coeff, 0, 0);
		}
	}
	// Pętla nieskończona
#ifdef __DEBUG
	printf("%d UpdateLCD Task Loop...\n", HAL_GetTick());
#endif
	for(;;){

		/*//if(huart1.RxState == HAL_UART_STATE_READY) HAL_UART_Receive_DMA(&huart1, nextion2_in, 20);

		if(nextion_big.display != nextion_big.last_display){
			if(nextion_big.display == 2){
				// Rok
				current_command = NEX_YEAR;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_YEAR], currDate.Year + 2000);
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size += 3;
				HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				// Miesiąc
				current_command = NEX_MONTH;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_MONTH], currDate.Month);
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size += 3;
				HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				// Dzień
				current_command = NEX_DAY;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_DAY], currDate.Date);
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size += 3;
				HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				// Godzina
				current_command = NEX_HOUR;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_HOUR], currTime.Hours);
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size += 3;
				HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				// Minuta
				current_command = NEX_MINUTE;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_MINUTE], currTime.Minutes);
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size += 3;
				HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				// Sekundy
				current_command = NEX_SECONDS;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_SECONDS], currTime.Seconds);
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size += 3;
				HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

			}else if(nextion_big.display == 2){
				if(nextion_small.display != 30){
					//buffor_to_send_size = sprintf(buffor_to_send, "power.lock.val=1");
					//HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
				}
			}
			nextion_big.last_display = nextion_big.display;
		}

		if(nextion_small.display != nextion_small.last_display){
			if(nextion_small.display == 1){
				if(nextion_big.display != 30){
					//buffor_to_send_size = sprintf(buffor_to_send, "power.lock.val=1");
					//HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
				}
			}
			nextion_small.last_display = nextion_small.display;
		}

		current_command = NEX_CWORD;
		if(nextion_small.display == 1 && nextion_small.sleep == 0){
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWORD], counter.control_word);
			for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
				buffor_to_send[i] = 0xFF;
			}
			buffor_to_send_size += 3;
			HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size,20);
			vTaskDelay(1);
		}

		if(nextion_big.display == 1){
			current_command = NEX_COUNTER_PRESENCE;
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_COUNTER_PRESENCE], counter.counter_present);
			for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
				buffor_to_send[i] = 0xFF;
			}
			buffor_to_send_size += 3;
			HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

			// Połączenie z termometrami
			current_command = NEX_TSENS_PRESENCE;
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_TSENS_PRESENCE], counter.temperatures_present);
			for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
				buffor_to_send[i] = 0xFF;
			}
			buffor_to_send_size += 3;
			HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

			// glowny wylacznik
			current_command = NEX_MAIN_SW;
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_MAIN_SW], MAIN_SWITCH);
			for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
				buffor_to_send[i] = 0xFF;
			}
			buffor_to_send_size += 3;
			HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

			// STAN CWU
			current_command = NEX_CWU_PIC;
			if(CWU_CONTACTOR_ERROR || (!(CWU_FUSE))){
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWU_PIC], 3);
			}else{
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWU_PIC], (CWU_HEATER) + 1);
			}
			for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
				buffor_to_send[i] = 0xFF;
			}
			buffor_to_send_size += 3;
			HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

			// STAN CO
			current_command = NEX_CO_PIC;
			if(CO_CONTACTOR_ERROR || (!(CO_FUSE))){
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CO_PIC], 3);
			}else{
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CO_PIC], (CO_HEATER) + 1);
			}
			for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
				buffor_to_send[i] = 0xFF;
			}
			buffor_to_send_size += 3;
			HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

			//PARAMETRY SIECI
			for(licz = 0; licz < 3; licz++){
				current_command = NEX_PHASE_VOLTAGE;
				buffor_to_send_size= sprintf(buffor_to_send, nextion_commands[NEX_PHASE_VOLTAGE], licz + 1, (int)(counter.voltages[licz] * 20),(int)((counter.voltages[licz] - (int)counter.voltages[licz])*10));
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size+=3;
				if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
				//if(nextion_small.display == 0) HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				current_command = NEX_PHASE_CURRENT;
				buffor_to_send_size= sprintf(buffor_to_send, nextion_commands[NEX_PHASE_CURRENT], licz + 1, (int)(counter.currents[licz] * 10),(int)((counter.currents[licz] - (int)counter.currents[licz])*10));
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size+=3;
				if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
				//if(nextion_small.display == 0) HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				current_command = NEX_PHASE_POWER;
				buffor_to_send_size= sprintf(buffor_to_send, nextion_commands[NEX_PHASE_POWER], licz + 1, (int)(counter.powers[licz] * 10),(int)((counter.powers[licz] - (int)counter.powers[licz])*10));
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size+=3;
				if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
				//if(nextion_small.display == 0) HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				current_command = NEX_PHASE_PRG_BAR;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_PHASE_PRG_BAR],licz + 1, (int8_t)(counter.powers[licz]/PHASE_POWER * 100.0));
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size+=3;
				if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				current_command = NEX_CO_HEATER_PHASE_DUTY;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CO_HEATER_PHASE_DUTY], licz + 1, (counter.CO_heater_PWM[licz] / 10));
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size+=3;
				if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);

				current_command = NEX_CWU_HEATER_PHASE_DUTY;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWU_HEATER_PHASE_DUTY], licz + 1, (counter.CWU_heater_PWM[licz] / 10));
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size+=3;
				if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
			}
		}

		if(nextion_big.display == 1 || nextion_small.display == 0){
			// Temperatury
			for(int i = 0; i < 4; i++){
				if(i < 2){
					current_command = NEX_CWU_TEMP;
					buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWU_TEMP], i + 1, (int16_t)(counter.CWU_temps[i] * 10));
					for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
						buffor_to_send[i] = 0xFF;
					}
					buffor_to_send_size += 3;
					if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
					if(nextion_small.display == 0 && nextion_small.sleep == 0) HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
				}
				current_command = NEX_CO_TEMP;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CO_TEMP], i + 1, (int16_t)(counter.CO_temps[i] * 10));
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size += 3;
				if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
				if(nextion_small.display == 0 && nextion_small.sleep == 0) HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
			}
		}

		ulNotificationValue = ulTaskNotifyTake(pdTRUE, 1000);
		if((ulNotificationValue > 0) && (ulNotificationValue < 255)){

			xTimerStart(EEPROMHandle, 10000);
			if(nextion_small.sleep){
				current_command = NEX_WAKE;
				buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_WAKE]);
				for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
					buffor_to_send[i] = 0xFF;
				}
				buffor_to_send_size += 3;
				HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
				vTaskDelay(10);
			}

			switch (ulNotificationValue) {
				case 6:
					current_command = NEX_CO_HIGH_TEMP;
					buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CO_HIGH_TEMP], counter.eeprom_data.CO_hi_temp);
					for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
						buffor_to_send[i] = 0xFF;
					}
					buffor_to_send_size+=3;
					break;

				case 7:
					current_command = NEX_CO_LOW_TEMP;
					buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CO_LOW_TEMP], counter.eeprom_data.CO_lo_temp);
					for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
						buffor_to_send[i] = 0xFF;
					}
					buffor_to_send_size+=3;
					break;

				case 8:
					current_command = NEX_CO_HEATER_POWER;
					buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CO_HEATER_POWER], (int)counter.eeprom_data.CO_heater_power);
					for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
						buffor_to_send[i] = 0xFF;
					}
					buffor_to_send_size+=3;
					break;

				case 9:
					current_command = NEX_CWU_HIGH_TEMP;
					buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWU_HIGH_TEMP], counter.eeprom_data.CWU_hi_temp);
					for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
						buffor_to_send[i] = 0xFF;
					}
					buffor_to_send_size+=3;
					break;

				case 10:
					current_command = NEX_CWU_LOW_TEMP;
					buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWU_LOW_TEMP], counter.eeprom_data.CWU_lo_temp);
					for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
						buffor_to_send[i] = 0xFF;
					}
					buffor_to_send_size+=3;
					break;

				case 11:
					current_command = NEX_CWU_HEATER_POWER;
					buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWU_HEATER_POWER], (int)counter.eeprom_data.CWU_heater_power);
					for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
						buffor_to_send[i] = 0xFF;
					}
					buffor_to_send_size+=3;
					break;

				case 12:
					current_command = NEX_ACC_COEFF;
					buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_ACC_COEFF], counter.eeprom_data.acumulated_power_coeff);
					for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
						buffor_to_send[i] = 0xFF;
					}
					buffor_to_send_size+=3;
					break;

				case 13:
					current_command = NEX_HEATER_COEFF;
					buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_HEATER_COEFF], counter.eeprom_data.heater_power_coeff);
					for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
						buffor_to_send[i] = 0xFF;
					}
					buffor_to_send_size+=3;
					break;
			}
			HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 30);
			HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 30);
		}*/
	}
  /* USER CODE END UpdateLCD */
}

void Nextion1_DataRcv (void){
	uint8_t stopbyte = 0;
	uint8_t i = 0;
	uint8_t y = 0;
	uint8_t n = 0;
	uint8_t len;
	uint16_t start;
	uint32_t number = 0;
	int32_t value = 0;

	while(nextion_big_comm.UartBufferTail != nextion_big_comm.UartBufferHead){
		len = (nextion_big_comm.UartBufferTail - nextion_big_comm.UartBufferHead) & (UART_BUFFER_SIZE - 1);
		start = nextion_big_comm.UartBufferHead;
		if(len > 3){
			if(len > 9){
				if(nextion_big_comm.UART_Buffer[start + 4] == 0x0A && nextion_big_comm.UART_Buffer[start + 4] == 0x0A){
					memcpy(&number, &nextion_big_comm.UART_Buffer[start], 4);
					memcpy(&value, &nextion_big_comm.UART_Buffer[start + 5], 4);
					//ModifyEEPROMStruct(number, value);
					nextion_big_comm.UartBufferHead += 10;
				}else{
					n = 1;
				}
			}else {
				n = 1;
			}
			if(n){
				n = 0;
				for(i = start + 1; i < (start + 5); i++){
					if(nextion_big_comm.UART_Buffer[i & (UART_BUFFER_SIZE - 1)] == 0xFF){
						stopbyte ++;
					}
				}
				if(stopbyte == 3){
					stopbyte = 0;
					switch(nextion_big_comm.UART_Buffer[start]){
						case 0x66:
							nextion_big.display = nextion_big_comm.UART_Buffer[start + 1];
							y = 5;
							break;

						case 0x1a:
							command_correct[current_command] = 1;
							y = 4;
							break;
						default:
							y = 4;
							break;
					}
					nextion_big_comm.UartBufferHead += y;
				}else{
					nextion_big_comm.UartBufferHead ++;
				}
			}

		}else{
			//if(nextion_big_comm.UART_Buffer[start] == 0 || nextion_big_comm.UART_Buffer[start] == 255){
				nextion_big_comm.UartBufferHead++;
			//}
		}
		nextion_big_comm.UartBufferHead &= (UART_BUFFER_SIZE - 1);
	}
}

void Nextion2_DataRcv (void){
	uint8_t stopbyte = 0;
	uint8_t i = 0;
	uint8_t y = 0;
	uint8_t n = 0;
	uint8_t len;
	uint16_t start;
	uint32_t number = 0;
	int32_t value = 0;

	while(nextion_small_comm.UartBufferTail != nextion_small_comm.UartBufferHead){
		len = (nextion_small_comm.UartBufferTail - nextion_small_comm.UartBufferHead) & (UART_BUFFER_SIZE - 1);
		start = nextion_small_comm.UartBufferHead;
		if(len > 3){
			if(len > 9){
				if(nextion_small_comm.UART_Buffer[start + 4] == 0x0A && nextion_small_comm.UART_Buffer[start + 4] == 0x0A){
					memcpy(&number, &nextion_small_comm.UART_Buffer[start], 4);
					memcpy(&value, &nextion_small_comm.UART_Buffer[start + 5], 4);
					//ModifyEEPROMStruct(number, value);
					nextion_small_comm.UartBufferHead += 10;
				}else{
					n = 1;
				}
			}else {
				n = 1;
			}
			if(n){
				n = 0;
				for(i = start + 1; i < (start + 5); i++){
					if(nextion_small_comm.UART_Buffer[i & (UART_BUFFER_SIZE - 1)] == 0xFF){
						stopbyte ++;
					}
				}
				if(stopbyte == 3){
					stopbyte = 0;
					switch(nextion_small_comm.UART_Buffer[start]){
						case 0x66:
							nextion_small.display = nextion_small_comm.UART_Buffer[start + 1];
							y = 5;
							break;

						case 0x1a:
							command_correct[current_command] = 1;
							y = 4;
							break;

						case 0x86:
							nextion_small.sleep = 1;
							y = 4;
							break;

						case 0x87:
							nextion_small.sleep = 0;
							y = 4;
							break;

						case 0x88:
							nextion_small.ready = 1;
							y = 4;
							break;

						default:
							y = 4;
							break;
					}
					nextion_small_comm.UartBufferHead += y;
				}else{
					nextion_small_comm.UartBufferHead ++;
				}
			}

		}else{
			//if(nextion_small_comm.UART_Buffer[start] == 0 || nextion_small_comm.UART_Buffer[start] == 255){
				nextion_small_comm.UartBufferHead++;
			//}
		}
		nextion_small_comm.UartBufferHead &= (UART_BUFFER_SIZE - 1);
	}
}
