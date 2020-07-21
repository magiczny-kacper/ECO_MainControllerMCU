/*
 * myTasks.h
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#ifndef SRC_TASKS_MYTASKS_H_
#define SRC_TASKS_MYTASKS_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include "wizchip_conf.h"
#include "socket.h"

#include "../Flash/w25qxx.h"
#include "../nRF24/nRF24.h"
#include "../Modbus/Modbus.h"
#include "UART_DMA.h"
#include "../Expander/mcp23017.h"

/*
 * Unions and structures used in tasks
 */

uint8_t nextion1_in[10];
uint8_t nextion2_in[10];









/*
 * Power regulation specific defines
 */




/*
 * Nextion displays defines, enums
 */







void PowerRegulation(void const * argument);
void UpdateLCD(void const * argument);
void TestTask(void const * argument);
void EthernetTask(void const * argument);
void RadioComm(void const * argument);
void RadioComm(void const * argument);


#endif /* SRC_TASKS_MYTASKS_H_ */
