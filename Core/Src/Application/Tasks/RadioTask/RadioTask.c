/*
 * RadioTask.c
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#include "RadioTask.h"

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

//#include "../DeviceDrivers/NRF24L01/nRF24.h"

extern osMutexId SPIMutexHandle;

extern SPI_HandleTypeDef hspi1;

void NRF_ChipSelect (void);

void NRF_ChipDeselect (void);

void NRF_ChipSelect (void){
	xSemaphoreTake(SPIMutexHandle, portMAX_DELAY);
	HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_RESET);
}

void NRF_ChipDeselect (void){
	HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_SET);
	xSemaphoreGive(SPIMutexHandle);
}

void RadioTask(void* argument)
{
	const uint8_t ecoIOinitMSG[] = "IsAlive";
  /* USER CODE BEGIN RadioComm */
//	nRF24_Init(&hspi1);
//	nRF24_SetRXAddress(0, "_ECOE");
//	nRF24_SetRXAddress(1, "_ECOI");
//	nRF24_SetTXAddress("_ECOM");
//	nRF24_RX_Mode();
	//nRF24_SendData((uint8_t*)ecoIOinitMSG, sizeof(ecoIOinitMSG));

  /* Infinite loop */
  for(;;)
  {
	  vTaskDelay(osWaitForever);
	  if(pdTRUE == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, portMAX_DELAY)){
		  //nRF24_Event();
	  }
  }
  /* USER CODE END RadioComm */
}
