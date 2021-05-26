/*
 * TemperatureSensors.c
 *
 *  Created on: 3 maj 2021
 *      Author: Kacper
 */

/**
 * @addtogroup TemperatureSensor
 * @{
 */

#include "TemperatureSensors.h"

void TemperatureSensor_Init(TemperatureSensors_t* dev, ModbusRTUMaster_t* master, uint8_t address){
	if(dev != NULL){
		if(master != NULL){
			dev -> mb = master;
			dev -> address = address;
		}
	}
}

int32_t TemperatureSensor_ReadTemps(TemperatureSensors_t* dev){
	int16_t temps[10];

	if(Modbus_OK == ModbusRTUMaster_ReadInputRegisters(dev -> mb, dev -> address, REG_ADDR, 10, (uint8_t*)temps, NULL)){
		for(uint8_t i = 0; i < 10; i++){
			dev -> temps[i] = (float)temps[i] / 100.0f;
		}
	}

	return 0;
}

/**
 * @}
 */
