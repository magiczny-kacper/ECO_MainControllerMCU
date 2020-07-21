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

#include "../../Flash/w25qxx.h"

extern osMutexId SPIMutexHandle;



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
	W25qxx_Init();
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END DataLog */
}
