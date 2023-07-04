/*
 * ModbusTCPServer.c
 *
 *  Created on: Oct 24, 2020
 *      Author: kacper
 */

#include "ModbusTCPServer.h"
#include "ModbusRTUMaster.h"

#include "RuntimeStats.h"

int32_t ModbusTCPServer_Init (void){
	int32_t retval = 0;

	return retval;
}

int32_t ModbusTCPServer_Proc(uint8_t* inBuf, uint16_t len, uint8_t* outBuf){
	int32_t retval = -1;

	int16_t dataLen = 0;
	uint16_t transId = 0, protId = 0;
	uint8_t slaveId, fcCode;
	uint16_t lenCpy = len - 6;

	if((inBuf != NULL) && (outBuf != NULL) && (len != 0))
	{
		transId = ((uint16_t)(inBuf[0]) << 8) + ((inBuf[1]));
		protId = ((uint16_t)(inBuf[2]) << 8) + ((inBuf[3]));
		dataLen = ((uint16_t)(inBuf[4]) << 8) + ((inBuf[5]));

		if(protId == 0){
			if(dataLen == lenCpy){
				memcpy(outBuf, (uint8_t*)&transId,2);
				outBuf[2] = (uint8_t)((transId & 0xFF00) >> 8);
				outBuf[3] = (uint8_t)(transId & 0xFF);
				outBuf[4] = (uint8_t)((protId & 0xFF00) >> 8);
				outBuf[5] = (uint8_t)(protId & 0xFF);

				slaveId = inBuf[0];
				fcCode = inBuf[1];

				if((slaveId > 0) && (slaveId < 255))
				{
					dataLen = ModbusRTUMaster_TCPForward(inBuf, dataLen, outBuf + 6);
					if(dataLen > 3){
						outBuf[dataLen + 6] = (uint8_t)((dataLen & 0xFF00) >> 8);
						outBuf[dataLen + 7] = (uint8_t)(dataLen & 0xFF);
						retval = (int32_t)(dataLen + 6);
					}
				}
				else
				{
					switch(fcCode)
					{
						default:
							outBuf[6] = 0;
							outBuf[7] = 5;
							outBuf[8] = slaveId;
							outBuf[9] = fcCode | 128;
							outBuf[10] = 1;
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
