/*
 * ModbusTCPServer.c
 *
 *  Created on: Oct 24, 2020
 *      Author: kacper
 */

#include "ModbusTCPServer.h"
#include "ModbusRTUMaster.h"

#include "../RuntimeStats/RuntimeStats.h"

int32_t ModbusTCPServer_Init (void){
	int32_t retval = 0;

	return retval;
}

int32_t ModbusTCPServer_Proc(ModbusRTUMaster_t* modbus, uint8_t* inBuf, uint16_t len, uint8_t* outBuf){
	int32_t retval = 0;
	uint8_t* rdPtr;
	uint8_t* wrPtr;

	int16_t dataLen = 0;
	uint16_t transId = 0, protId = 0;
	uint8_t slaveId, fcCode;
	uint16_t lenCpy = len - 6;

	if((inBuf == NULL) || (outBuf == NULL) || (len == 0)){
		retval = -1;
	}

	if(retval > -1){
		wrPtr = outBuf;
		rdPtr = inBuf;

		transId = ((uint16_t)(*rdPtr) << 8) + (*(rdPtr + 1));
		rdPtr += 2;
		protId = ((uint16_t)(*rdPtr) << 8) + (*(rdPtr + 1));
		rdPtr += 2;
		dataLen = ((uint16_t)(*rdPtr) << 8) + (*(rdPtr + 1));
		rdPtr += 2;

		if(protId == 0){
			if(dataLen  == lenCpy){
				memcpy(wrPtr, (uint8_t*)&transId,2);
				*wrPtr = (uint8_t)((transId & 0xFF00) >> 8);
				wrPtr ++;
				*wrPtr = (uint8_t)(transId & 0xFF);
				wrPtr ++;

				*wrPtr = (uint8_t)((protId & 0xFF00) >> 8);
				wrPtr ++;
				*wrPtr = (uint8_t)(protId & 0xFF);
				wrPtr ++;

				slaveId = *rdPtr;
				fcCode = *(rdPtr + 1);

				if((slaveId > 0) && (slaveId < 255)){
					dataLen = ModbusRTUMaster_TCPForward(modbus, rdPtr, dataLen, wrPtr + 2);
					if(dataLen > 3){
						*wrPtr = (uint8_t)((dataLen & 0xFF00) >> 8);
						wrPtr ++;
						*wrPtr = (uint8_t)(dataLen & 0xFF);
						wrPtr ++;

						retval = (int32_t)(dataLen + 6);
					}
				}else{
					switch(fcCode){
						default:
							*wrPtr = 0;
							wrPtr ++;
							*wrPtr = 5;
							wrPtr ++;
							*wrPtr = slaveId;
							wrPtr ++;
							*wrPtr = fcCode | 128;
							wrPtr ++;
							*wrPtr = 1;
							wrPtr ++;

							retval = 11;
							break;
					}
				}


			}else{
				retval = -1;
			}
		}else{
			retval = -1;
		}
	}

	return retval;
}
