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

/*
 * TODO: Define values.
 */

#define DEC_PLACES 					10.0
#define ACUMULATED_POWER_FACTOR 	0
#define NET_POWER 					0
#define PHASE_POWER 				0
#define PHASE_POWER_COEFF 			0
#define CO_HEATER_PHASE_POWER 		1
#define CWU_HEATER_PHASE_POWER 		1

#define BOJLER_HIGH 				0
#define BOJLER_LOW 					0

#define CO_HIGH						0
#define CO_LOW 						0

#define MAIN_SWITCH 				0
#define MAIN_SWITCH_SET 			0
#define MAIN_SWITCH_RESET 			0

#define CO_FUSE 					0
#define	CO_FUSE_SET					0
#define CO_FUSE_RESET				0

#define CWU_FUSE 					0
#define CWU_FUSE_SET				0
#define CWU_FUSE_RESET				0

#define CO_HEATER					0
#define CO_HEATER_SET				0
#define CO_HEATER_RESET				0

#define CWU_HEATER					0
#define CWU_HEATER_SET				0
#define CWU_HEATER_RESET			0

#define CO_CONTACTOR_ERROR			0
#define CO_CONTACTOR_ERROR_SET		0
#define CO_CONTACTOR_ERROR_RESET	0

#define CWU_CONTACTOR_ERROR			0
#define CWU_CONTACTOR_ERROR_SET		0
#define CWU_CONTACTOR_ERROR_RESET	0

#define EXP_STATE 					0

#define MAIN_SW_EXPPIN 				0
#define CWU_F_EXPPIN 				1
#define CO_F_EXPPIN 				2
#define CWU_C_EXPPIN 				3
#define CO_C_EXPPIN 				4

#define MAIN_SW_STATE 				((EXP_STATE >> MAIN_SW_EXPPIN) & 1)
#define CWU_F_STATE 				((EXP_STATE >> CWU_F_EXPPIN) & 1)
#define CO_F_STATE 					((EXP_STATE >> CO_F_EXPPIN) & 1)
#define CWU_C_STATE 				((EXP_STATE >> CWU_C_EXPPIN) & 1)
#define CO_C_STATE 					((EXP_STATE >> CO_C_EXPPIN) & 1)


union float_bytes {
	float value;
	uint8_t bytes[4];
};

struct counter_data{
	uint8_t counter_present;
	uint8_t last_counter_present;
	uint8_t temperatures_present;
	uint8_t last_temperatures_pesent;

	uint8_t control_word;

	uint8_t main_switch;
	uint8_t CO_fuse;
	uint8_t CWU_fuse;

	uint8_t CWU_heating;
	uint8_t CO_heating;

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
};

void RegulationTask(void const * argument);

#endif /* SRC_TASKS_REGULATIONTASK_REGULATIONTASK_H_ */
