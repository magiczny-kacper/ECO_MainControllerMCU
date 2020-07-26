/*
 * drv_EEPROM.h
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#ifndef SRC_CONFIGEEPROM_DRV_EEPROM_H_
#define SRC_CONFIGEEPROM_DRV_EEPROM_H_

#include "stm32f4xx_hal.h"

#define EE_ADDR 			0xA0
#define EE_PAGE_SIZE		16
#define EE_BYTE_WRITE_TIME	4
#define EE_PAGE_WRITE_TIME	(EE_PAGE_SIZE * EE_BYTE_WRITE_TIME)
#define EE_BLOCK_SIZE		256
#define EE_BLOCKS			4
#define EE_SIZE				1024

typedef enum{
	EE_ERR_BUSY,
	EE_ERR_NULL,
	EE_ERR_TIMEOUT,
	EE_OK = 0,

} EE_StatusTypeDef;

EE_StatusTypeDef EE_Init (I2C_HandleTypeDef* hi2c);

EE_StatusTypeDef EE_Read (void* data, uint8_t addr, uint32_t len);

EE_StatusTypeDef EE_Write (void* data, uint32_t addr, uint32_t len);

EE_StatusTypeDef EE_Erease (void);

#endif /* SRC_CONFIGEEPROM_DRV_EEPROM_H_ */
