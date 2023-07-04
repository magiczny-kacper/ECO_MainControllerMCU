#ifndef RESOURCEGUARD_H_
#define RESOURCEGUARD_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "spi.h"
#include "i2c.h"

void RG_Init(void);
void RG_SPI_ChipSelect(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void RG_SPI_ChipDeselect(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void RG_I2C_Acquire(void);
void RG_I2C_Release(void);

#endif /* RESOURCEGUARD_H_ */
