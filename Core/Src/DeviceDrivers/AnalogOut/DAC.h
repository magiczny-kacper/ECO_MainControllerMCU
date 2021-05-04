/*
 * DAC.h
 *
 *  Created on: 19 lip 2020
 *      Author: Kacper
 */

#ifndef SRC_ANALOGOUT_DAC_H_
#define SRC_ANALOGOUT_DAC_H_

#include "stm32f4xx_hal.h"

void DAC_Init(SPI_HandleTypeDef* dac_spi);

void DAC_SetRawValue(uint16_t value);

void DAC_SetNormValue(float value);

#endif /* SRC_ANALOGOUT_DAC_H_ */
