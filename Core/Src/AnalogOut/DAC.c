/*
 * DAC.c
 *
 *  Created on: 19 lip 2020
 *      Author: Kacper
 */

#include "DAC.h"
#include "main.h"
#include "cmsis_os.h"

extern osMutexId SPIMutexHandle;

SPI_HandleTypeDef* dacSpi;

#define DAC_CONF 0b0111000000000000
#define DAC_MAX_VAL 4095

static void DAC_ChipSelect (void);

static void DAC_ChipDeselect (void);

static void DAC_SpiSend(uint16_t data);

static void DAC_ChipSelect (void){
	xSemaphoreTake(SPIMutexHandle, portMAX_DELAY);
	HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_RESET);
}

static void DAC_ChipDeselect (void){
	HAL_GPIO_WritePin(DAC_CS_GPIO_Port, DAC_CS_Pin, GPIO_PIN_SET);
	xSemaphoreGive(SPIMutexHandle);
}

static void DAC_SpiSend(uint16_t data){
	uint16_t datacpy;
	datacpy = data;
	HAL_SPI_Transmit(dacSpi, (uint8_t*)&datacpy, 2, 10);
}

void DAC_Init(SPI_HandleTypeDef* dac_spi){
	uint16_t initData = DAC_CONF;

	dacSpi = dac_spi;
	DAC_ChipSelect();
	DAC_SpiSend(initData);
	DAC_ChipDeselect();
}

void DAC_SetRawValue(uint16_t value){
	uint16_t dacData;

	if(value > DAC_MAX_VAL) dacData = DAC_MAX_VAL;
	else dacData = value;
	dacData |= DAC_CONF;
	DAC_ChipSelect();
	DAC_SpiSend(dacData);
	DAC_ChipDeselect();
}

void DAC_SetNormValue (float value){
	float value1;
	uint16_t value2;

	if(value > 1.0f){
		value1 = 1.0f;
	}else if(value < 0.0f){
		value1 = 0.0f;
	}else{
		value1 = value;
	}

	value1 *= DAC_MAX_VAL;
	value2 = (uint16_t) value1;
	if(value2 > DAC_MAX_VAL) value2 = DAC_MAX_VAL;

	DAC_SetRawValue(value2);
}
