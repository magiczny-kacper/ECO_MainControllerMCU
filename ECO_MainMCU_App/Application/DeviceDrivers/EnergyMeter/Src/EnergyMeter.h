#ifndef ENERGYMETER_H_
#define ENERGYMETER_H_

/**
 * @brief
 * @defgroup EnergyMeter
 * @{
 */

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "ModbusRTUMaster.h"

union float_bytes {
	float value;
	uint8_t bytes[4];
};

typedef struct EnergyMeter_Values {
	float voltage[3];
	float inter_phase_voltage[3];
	float average_phase_voltage;
	float current[3];
	float active_power[3];
	float apparent_power[3];
	float reactive_power[3];
	float power_factor[3];
	float imported_active_power;
	float exported_active_power;
} EnergyMeter_Values_t;

typedef struct EnergyMeter {
	uint8_t address;
	uint8_t device_ok;
	uint8_t max_retries;
	uint8_t curr_retries;
	EnergyMeter_Values_t values;
} EnergyMeter_t;

typedef enum EnergyMeter_Regs {
	L1_VOLTAGE = 0x00,
	L2_VOLTAGE = 0x02,
	L3_VOLTAGE	= 0x04,

	L1_CURRENT = 0x06,
	L2_CURRENT = 0x08,
	L3_CURRENT = 0x0A,

	L1_ACTIVE_POWER = 0x0C,
	L2_ACTIVE_POWER = 0x0E,
	L3_ACTIVE_POWER = 0x10,

	L1_APPARENT_POWER = 0x12,
	L2_APPARENT_POWER = 0x14,
	L3_APPARENT_POWER = 0x16,

	L1_REACTIVE_POWER = 0x18,
	L2_REACTIVE_POWER = 0x1A,
	L3_REACTIVE_POWER = 0x1C,

	L1_POWER_FACTOR = 0x1E,
	L2_POWER_FACTOR = 0x20,
	L3_POWER_FACTOR = 0x22,

	AVERAGE_PHASE_VOLTAGE = 0x2A,
	TOTAL_POWER = 0x34,
	NETWORK_FREQUENCY = 0x46,
	IMPORTED_ACTIVE_POWER = 0x48,
	EXPORTED_ACTIVE_POWER = 0x4A,

	L1_L2_VOLTAGE = 0xC8,
	L2_L3_VOLTAGE = 0xCA,
	L3_L1_VOLTAGE = 0xCC
} EnergyMeter_Regs_t;

void EnergyMeter_Init(EnergyMeter_t* meter, uint8_t address, uint8_t max_retries);

float EnergyMeter_ParseFloat (uint8_t* in_data);

int32_t EnergyMeter_Read(EnergyMeter_t* meter);

/**
 * @}
 */
#endif /* ENERGYMETER_H_ */
