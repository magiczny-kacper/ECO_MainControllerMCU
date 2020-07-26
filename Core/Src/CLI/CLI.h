/*
 * CLI.h
 *
 *  Created on: 23 lip 2020
 *      Author: Kacper
 */

#ifndef SRC_CLI_CLI_H_
#define SRC_CLI_CLI_H_


#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"

void CLI_Init (void);
BaseType_t xCLI_EreaseEE( char *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

#endif /* SRC_CLI_CLI_H_ */
