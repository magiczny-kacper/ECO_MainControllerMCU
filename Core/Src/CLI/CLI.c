/*
 * CLI.c
 *
 *  Created on: 23 lip 2020
 *      Author: Kacper
 */


#include "CLI.h"
#include "../ConfigEEPROM/drv_EEPROM.h"

static const CLI_Command_Definition_t xEreaseEECmd = {
	"erease_eeprom",
	"erease_eeprom:\r\n    Ereases EEPROM memory.\r\n",
	xCLI_EreaseEE,
	0
};

void CLI_Init (void){
	FreeRTOS_CLIRegisterCommand(&xEreaseEECmd);
}

BaseType_t xCLI_EreaseEE( char *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;
	EE_StatusTypeDef retval = EE_Erease();
	if(retval == EE_OK){
		sprintf(pcWriteBuffer, "Wyczyszczono.\r\n");
	}else{
		sprintf(pcWriteBuffer, "Nie wyczyszczono.\r\n");
	}
	return pdFALSE;
}
