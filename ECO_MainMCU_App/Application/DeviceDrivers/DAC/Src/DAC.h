/*
 * DAC.h
 *
 *  Created on: 19 lip 2020
 *      Author: Kacper
 */

#ifndef DAC_H_
#define DAC_H_

#include "spi.h"

void DAC_Init(SPI_HandleTypeDef* dac_spi);

void DAC_SetRawValue(uint16_t value);

void DAC_SetNormValue(float value);

#endif /* DAC_H_ */
