/*
 * EnergyMeter.c
 *
 *  Created on: May 3, 2021
 *      Author: Kacper
 */

/**
 * @addtogroup EnergyMeter
 * @{
 */

#include "EnergyMeter.h"
#include "FreeRTOS.h"
#include "semphr.h"

void EnergyMeter_Init(EnergyMeter_t* meter, uint8_t address, uint8_t max_retries){
    meter->max_retries = max_retries;
	meter->address = address;
}

float EnergyMeter_ParseFloat (uint8_t* in_data){
	union float_bytes buffor;

	buffor.bytes[3] = *in_data;
	in_data ++;
	buffor.bytes[2] = *in_data;
	in_data ++;
	buffor.bytes[1] = *in_data;
	in_data ++;
	buffor.bytes[0] = *in_data;

	return buffor.value;
}

int32_t EnergyMeter_Read(EnergyMeter_t* meter){
	int32_t retval = 0;
	uint8_t counter_data_raw[36];
	if(meter == NULL) return -1;
	ModbusState_t state = ModbusRTUMaster_ReadInputRegisters(meter->address, 
            L1_VOLTAGE, 18, counter_data_raw);

	if(state == Modbus_OK){
		for(uint8_t i = 0; i < 3; i++){
			meter -> values.voltage[i] = EnergyMeter_ParseFloat(
                    counter_data_raw + i * 4);
			meter -> values.current[i] = EnergyMeter_ParseFloat(
                    counter_data_raw + 12 + i * 4);
			meter -> values.active_power[i] = EnergyMeter_ParseFloat(
                    counter_data_raw + 24 + 4 * i);
		}

		state = ModbusRTUMaster_ReadInputRegisters(meter->address,
                IMPORTED_ACTIVE_POWER, 4, counter_data_raw);
		if(state == Modbus_OK){
			meter -> values.imported_active_power = EnergyMeter_ParseFloat(
                    counter_data_raw);
			meter -> values.exported_active_power = EnergyMeter_ParseFloat(
                    counter_data_raw + 4);
		}else{
			retval = -3;
		}
	}else{
		retval = -2;
	}

	return retval;
}

/**
 * @}
 */
