/*
 * ModbusTCPServer.h
 *
 *  Created on: Oct 24, 2020
 *      Author: kacper
 */

#ifndef MODBUSTCPSERVER_H_
#define MODBUSTCPSERVER_H_

#include <stdint.h>
#include <string.h>

#include "ModbusRTUMaster.h"

int32_t ModbusTCPServer_Proc(uint8_t* inBuf, uint16_t len, uint8_t* outBuf);

#endif /* MODBUSTCPSERVER_H_ */
