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

#include "../../ConfigEEPROM/config.h"

/*
 * F&F energy meter modbus registers addresses
 */

#define L1_VOLTAGE 				0x00 // V
#define L2_VOLTAGE 				0x02
#define L3_VOLTAGE				0x04

#define L1_CURRENT 				0x06 // A
#define L2_CURRENT 				0x08
#define L3_CURRENT 				0x0A

#define L1_ACTIVE_POWER			0x0C // W
#define L2_ACTIVE_POWER			0x0E
#define L3_ACTIVE_POWER 		0x10

#define L1_APPARENT_POWER		0x12 // VA
#define L2_APPARENT_POWER		0x14
#define L3_APPARENT_POWER		0x16

#define L1_REACTIVE_POWER		0x18 // VAr
#define L2_REACTIVE_POWER		0x1A
#define L3_REACTIVE_POWER		0x1C

#define L1_POWER_FACTOR			0x1E
#define L2_POWER_FACTOR			0x20
#define L3_POWER_FACTOR			0x22

#define AVERAGE_PHASE_VOLTAGE	0x2A // V
#define TOTAL_POWER				0x34 // A
#define NETWORK_FREQUENCY		0x46 // Hz
#define IMPORTED_ACTIVE_POWER	0x48 // kWh
#define EXPORTED_ACTIVE_POWER	0x4A // kWh

#define L1_L2_VOLTAGE 			0xC8 // V
#define L2_L3_VOLTAGE			0xCA
#define L3_L1_VOLTAGE			0xCC

#define COUNTER_ADDRESS 			1
#define TEMPERATURE_ADDRESS 		2
#define SENSOR_NUMBER 				10




union float_bytes {
	float value;
	uint8_t bytes[4];
};

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

		uint8_t input8 	:1;
		uint8_t input7 	:1;
		uint8_t input6 	:1;
		uint8_t input5 	:1;
		uint8_t input4 	:1;
		uint8_t input3 	:1;
		uint8_t input2 	:1;
		uint8_t input1 	:1;
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
	ControlWord_t ControlWord;
	Expander_signals_t IOsignals;
	CONFStatus_t configStatus;
	RegulationConfig_t parameters;
} RegTaskData_t;

void RegulationTask(void const * argument);

void RegulationTask_GetData (RegTaskData_t* destination);

#endif /* SRC_TASKS_REGULATIONTASK_REGULATIONTASK_H_ */
