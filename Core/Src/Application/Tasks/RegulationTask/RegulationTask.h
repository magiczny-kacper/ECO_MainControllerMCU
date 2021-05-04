/*
 * RegulationTask.h
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#ifndef SRC_TASKS_REGULATIONTASK_REGULATIONTASK_H_
#define SRC_TASKS_REGULATIONTASK_REGULATIONTASK_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../Utils/Config/config.h"
#include "../Utils/Modbus/ModbusRTUMaster.h"
#include "../DeviceDrivers/EnergyMeter/EnergyMeter.h"
#include "../DeviceDrivers/TemperatureSensors/TemperatureSensors.h"

#define COUNTER_ADDRESS 			1
#define TEMPERATURE_ADDRESS 		2
#define SENSOR_NUMBER 				10

typedef union{
	struct{
		uint8_t input10	:1;
		uint8_t input9 	:1;
		uint8_t output6 :1;
		uint8_t output5 :1;
		uint8_t output4 :1;
		uint8_t output3 :1;
		uint8_t output2 :1;
		uint8_t output1 :1;

		uint8_t input1 	:1;
		uint8_t input2 	:1;
		uint8_t input3 	:1;
		uint8_t input4 	:1;
		uint8_t input5 	:1;
		uint8_t input6 	:1;
		uint8_t input7 	:1;
		uint8_t input8 	:1;
	}signals;

	struct{
		uint8_t portA;
		uint8_t portB;
	}ports;

	uint8_t bytes[2];
	uint16_t word;
} Expander_signals_t;

typedef union{
	uint8_t MainSwitch_State 	:1;
	uint8_t CWUHeaterStateOut 	:1;
	uint8_t COHeaterStateOut 	:1;
	uint8_t CWUHeaterError 		:1;
	uint8_t COHeaterError 		:1;
} ControlWord_t;

typedef struct {
	uint8_t counter_present;
	uint8_t last_counter_present;
	uint8_t temperatures_present;
	uint8_t last_temperatures_pesent;
	uint16_t CWU_heater_PWM[3];
	uint16_t CO_heater_PWM[3];
	float CO_heater_max_power[3];
	float CWU_heater_max_power[3];
	float voltages[3];
	float currents[3];
	float powers[3];
	float power_in_buffor;
	float CWU_temps[2];
	float CO_temps[4];
} counter_data;

typedef struct{
	counter_data counter;
	ModbusRTUMaster_t mbPort;
	EnergyMeter_t meter;
	TemperatureSensors_t temp;
	ControlWord_t ControlWord;
	Expander_signals_t IOsignals;
	ConfigStatus_t configStatus;
	RegulationConfig_t parameters;
} RegTaskData_t;

void RegulationTask(void* argument);

RegTaskData_t* RegulationTask_GetData (void);

#endif /* SRC_TASKS_REGULATIONTASK_REGULATIONTASK_H_ */
