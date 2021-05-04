/*
 * TemperatureSensors.h
 *
 *  Created on: 3 maj 2021
 *      Author: Kacper
 */

#ifndef SRC_DEVICEDRIVERS_TEMPERATURESENSORS_TEMPERATURESENSORS_H_
#define SRC_DEVICEDRIVERS_TEMPERATURESENSORS_TEMPERATURESENSORS_H_

/**
 * @brief
 * @defgroup TemperatureSensor
 * @{
 */

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "../Utils/Modbus/ModbusRTUMaster.h"

#define REG_ADDR 0x01

/**
 * @brief Device handler
 *
 */
typedef struct{
	uint8_t address; /**< Device address */
	uint8_t device_ok;
	uint8_t max_retries;
	uint8_t curr_retries;
	ModbusRTUMaster_t* mb; /**< Modbus master instance */
	float temps[10]; /**< Temperatures */
}TemperatureSensors_t;

/**
 * @brief
 *
 * @param dev
 * @param master
 * @param address
 */
void TemperatureSensor_Init(TemperatureSensors_t* dev, ModbusRTUMaster_t* master, uint8_t address);

int32_t TemperatureSensor_ReadTemps(TemperatureSensors_t* dev);

/**
 * @}
 */
#endif /* SRC_DEVICEDRIVERS_TEMPERATURESENSORS_TEMPERATURESENSORS_H_ */
