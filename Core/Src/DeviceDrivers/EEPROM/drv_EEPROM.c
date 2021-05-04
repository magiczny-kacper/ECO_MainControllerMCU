/*
 * drv_EEPROM.c
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#include <string.h>

#include "drv_EEPROM.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "cmsis_os.h"
#include "task.h"
#include "stm32f4xx_hal_i2c.h"

extern osMutexId I2CMutexHandle;
I2C_HandleTypeDef* ee_iic;

static HAL_StatusTypeDef EE_I2C_Read (uint8_t addr, void* dataOut, size_t len);

EE_StatusTypeDef EE_Init (I2C_HandleTypeDef* hi2c){
	if(hi2c != NULL){
		ee_iic = hi2c;
		return EE_OK;
	}
	return EE_ERR_NULL;
}

EE_StatusTypeDef EE_Read (void* data, uint8_t addr, size_t len){
	HAL_StatusTypeDef i2cStatus;
	uint8_t* dataBuf;

	if(data == NULL || len <= 0 ) return EE_ERR_NULL;

	dataBuf = (uint8_t*)data;
	i2cStatus = EE_I2C_Read(addr, dataBuf, len);

	switch (i2cStatus){
		case HAL_OK:
			return EE_OK;
			break;

		case HAL_ERROR:
			return EE_ERR_TIMEOUT;
			break;

		case HAL_BUSY:
			return EE_ERR_BUSY;
			break;

		default:
			return EE_ERR_TIMEOUT;
			break;
	}
}

EE_StatusTypeDef EE_Write (void* data, uint32_t addr, size_t len){
	EE_StatusTypeDef retval = EE_ERR_NULL;
	HAL_StatusTypeDef i2cStatus;
	uint8_t* dataBuf;
	uint32_t dataLength, i, pagesToWrite, rest, bytesToWrite, addrCpy;
	uint8_t eeWriteBlock, eeWriteAddr;

	if(data == NULL || len <= 0 || len >= EE_SIZE) return retval;

	eeWriteBlock = addr / EE_BLOCK_SIZE;
	eeWriteAddr = addr % EE_BLOCK_SIZE;

	dataBuf = (uint8_t*)data;
	dataLength = len;
	addrCpy = addr;

	if(pdFALSE == xSemaphoreTake(I2CMutexHandle, 100)){
		return EE_ERR_TIMEOUT;
	}

	if(dataLength <= EE_PAGE_SIZE){
		i2cStatus= HAL_I2C_Mem_Write(ee_iic, EE_ADDR + eeWriteBlock, eeWriteAddr, 1, dataBuf, len, EE_BYTE_WRITE_TIME * (len + 2));
		if(i2cStatus == HAL_OK){
			vTaskDelay(EE_BYTE_WRITE_TIME * (len + 2));
			retval = EE_OK;
		}
		else if(i2cStatus == HAL_ERROR) retval = EE_ERR_TIMEOUT;
		else if(i2cStatus == HAL_BUSY) retval = EE_ERR_BUSY;
	}else{
		pagesToWrite = dataLength >> 4;
		rest = dataLength % 16;
		if(rest) pagesToWrite ++;
		for(i = 0; i < pagesToWrite; i++){
			if(i == pagesToWrite - 1){
				bytesToWrite = rest;
			}else{
				bytesToWrite = 16;
			}
			i2cStatus = HAL_I2C_Mem_Write(ee_iic, EE_ADDR, addrCpy, 1, dataBuf, bytesToWrite, EE_BYTE_WRITE_TIME * (bytesToWrite + 2));
			if(i2cStatus == HAL_OK){
				dataBuf += bytesToWrite;
				addrCpy += bytesToWrite;
				retval = EE_OK;
				while(HAL_I2C_IsDeviceReady(ee_iic, EE_ADDR, 10, 10) != HAL_OK);
				//vTaskDelay((bytesToWrite + 2) * EE_BYTE_WRITE_TIME);
			}else{
				if(i2cStatus == HAL_ERROR) retval = EE_ERR_TIMEOUT;
				else if(i2cStatus == HAL_BUSY) retval = EE_ERR_BUSY;
				break;
			}
		}
	}
	xSemaphoreGive(I2CMutexHandle);
	return retval;
}

EE_StatusTypeDef EE_Erease (void){
	uint32_t i;
	int32_t retval = 0;
	uint8_t blankData[16];
	memset(blankData, 255, 16);

	for(i = 0; i < EE_SIZE; i += 16){
		retval |= EE_Write(blankData, i, 16);
	}
	return retval;
}

static HAL_StatusTypeDef EE_I2C_Read (uint8_t addr, void* dataOut, size_t len){
	HAL_StatusTypeDef retval;
	if(pdTRUE == xSemaphoreTake(I2CMutexHandle, 100)){
		retval = HAL_I2C_Mem_Read(ee_iic, EE_ADDR, addr, 1, dataOut, len, EE_BYTE_WRITE_TIME * (len + 2));
		xSemaphoreGive(I2CMutexHandle);
	}else{
		retval = HAL_BUSY;
	}
	return retval;
}
