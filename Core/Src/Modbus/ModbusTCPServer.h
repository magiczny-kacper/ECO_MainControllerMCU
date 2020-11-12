/*
 * ModbusTCPServer.h
 *
 *  Created on: Oct 24, 2020
 *      Author: kacper
 */

#ifndef SRC_MODBUS_MODBUSTCPSERVER_H_
#define SRC_MODBUS_MODBUSTCPSERVER_H_

#include <stdint.h>
#include <string.h>

#include "ModbusRTUMaster.h"

int32_t ModbusTCPServer_Proc(ModbusRTUMaster_t* modbus, uint8_t* inBuf, uint16_t len, uint8_t* outBuf);

#endif /* SRC_MODBUS_MODBUSTCPSERVER_H_ */
