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
BaseType_t xCLI_EreaseEE( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_ShowConfig( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_GetSystemStatus( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_GetRtosStats( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_GetCommStats( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_WriteTestEvent( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_EreaseFlash( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_Reset( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_ShowIO( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_GetLastEvent( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_GetEvents( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_GetEventsRaw( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_SetTime( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
BaseType_t xCLI_SetDate( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );
#endif /* SRC_CLI_CLI_H_ */
