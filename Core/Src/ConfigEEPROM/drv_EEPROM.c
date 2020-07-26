/*
 * drv_EEPROM.c
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#include <string.h>

#include "drv_EEPROM.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal_i2c.h"

I2C_HandleTypeDef* ee_iic;

EE_StatusTypeDef EE_Init (I2C_HandleTypeDef* hi2c){
	if(hi2c != NULL){
		ee_iic = hi2c;
		return EE_OK;
	}
	return EE_ERR_NULL;
}

EE_StatusTypeDef EE_Read (void* data, uint8_t addr, uint32_t len){
	HAL_StatusTypeDef i2cStatus;
	uint8_t* dataBuf;

	if(data == NULL || len <= 0 ) return EE_ERR_NULL;

	dataBuf = (uint8_t*)data;
	i2cStatus= HAL_I2C_Mem_Read(ee_iic, EE_ADDR, addr, 1, dataBuf, len, EE_BYTE_WRITE_TIME * (len + 2));
	if(i2cStatus == HAL_OK){
		return EE_OK;
	}else if(i2cStatus == HAL_ERROR){
		return EE_ERR_TIMEOUT;
	}else if(i2cStatus == HAL_BUSY){
		return EE_ERR_BUSY;
	}else{
		return EE_ERR_TIMEOUT;
	}
}

EE_StatusTypeDef EE_Write (void* data, uint32_t addr, uint32_t len){
	EE_StatusTypeDef retval = EE_ERR_NULL;
	HAL_StatusTypeDef i2cStatus;
	uint8_t* dataBuf;
	uint32_t datalen, i, pages, rest, bytelen;
	uint8_t eeBlock, eeAddr;

	if(data == NULL || len <= 0 || len >= EE_SIZE) return retval;

	eeBlock = addr / EE_BLOCK_SIZE;
	eeAddr = addr % EE_BLOCK_SIZE;

	dataBuf = (uint8_t*)data;
	datalen = len;
	if(len <= EE_PAGE_SIZE){
		i2cStatus= HAL_I2C_Mem_Write(ee_iic, EE_ADDR + eeBlock, eeAddr, 1, dataBuf, len, EE_BYTE_WRITE_TIME * (len + 2));
		if(i2cStatus == HAL_OK){
			vTaskDelay(EE_BYTE_WRITE_TIME * (len + 1));
			retval = EE_OK;
		}
		else if(i2cStatus == HAL_ERROR) retval = EE_ERR_TIMEOUT;
		else if(i2cStatus == HAL_BUSY) retval = EE_ERR_BUSY;
	}else{
		pages = datalen >> 4;
		rest = datalen % 16;
		if(rest) pages ++;
		for(i = 0; i < pages; i++){
			if(i == pages - 1){
				bytelen = rest;
			}else{
				bytelen = 16;
			}
			i2cStatus = HAL_I2C_Mem_Write(ee_iic, EE_ADDR, addr, 1, dataBuf, bytelen, EE_BYTE_WRITE_TIME * bytelen);
			if(i2cStatus == HAL_OK){
				retval = EE_OK;
				vTaskDelay(bytelen * EE_BYTE_WRITE_TIME);
			}else{
				if(i2cStatus == HAL_ERROR) retval = EE_ERR_TIMEOUT;
				else if(i2cStatus == HAL_BUSY) retval = EE_ERR_BUSY;
				break;
			}
		}
	}
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


