/*
 * DatalogTask.c
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#include "DataLogTask.h"

#include "cmsis_os.h"
#include "main.h"
#include "stm32f4xx_hal.h"

#include "../Flash/dataLog.h"

extern osMutexId SPIMutexHandle;
extern osMessageQId DataLogQueueHandle;

static DataLogEvent_t event;

void FLASH_ChipSelect (void){
	xSemaphoreTake(SPIMutexHandle, portMAX_DELAY);
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
}

void FLASH_ChipDeselect (void){
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);
	xSemaphoreGive(SPIMutexHandle);
}

void DataLogTask(void const * argument)
{
  /* USER CODE BEGIN DataLog */
	if(DataLog_Init() != DL_OK){
		vTaskSuspend(NULL);
	}
  /* Infinite loop */
	for(;;)
	{
		if(pdPASS == xQueueReceive(DataLogQueueHandle, &event, portMAX_DELAY)){
			DataLog_SaveEvent(&event);
		}
	}
  /* USER CODE END DataLog */
}
