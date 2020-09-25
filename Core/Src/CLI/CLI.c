/*
 * CLI.c
 *
 *  Created on: 23 lip 2020
 *      Author: Kacper
 */


#include "CLI.h"
#include "printf.h"

#include "../ConfigEEPROM/config.h"
#include "../ConfigEEPROM/drv_EEPROM.h"
#include "../RuntimeStats/RuntimeStats.h"


static const CLI_Command_Definition_t xEreaseEECmd = {
	"erease_eeprom",
	"erease_eeprom:\r\n    Ereases EEPROM memory.\r\n",
	xCLI_EreaseEE,
	0
};

static const CLI_Command_Definition_t xShowConfigCmd = {
	"show_config",
	"show_config:\r\n    Shows configuration.\r\n",
	xCLI_ShowConfig,
	0
};

static const CLI_Command_Definition_t xGetSysStatusCmd = {
	"get_system_status",
	"get_system_status:\r\n    Shows general system status, date, time, uptime.\r\n",
	xCLI_GetSystemStatus,
	0
};

static const CLI_Command_Definition_t xGetRtosStatsCmd = {
	"erease_eeprom",
	"erease_eeprom:\r\n    Ereases EEPROM memory.\r\n",
	xCLI_EreaseEE,
	0
};

static const CLI_Command_Definition_t xGetCommStatsCmd = {
	"get_comm_stats",
	"get_comm_stats:\r\n    Shows communication statistics.\r\n",
	xCLI_GetCommStats,
	0
};

void CLI_Init (void){
	FreeRTOS_CLIRegisterCommand(&xEreaseEECmd);
	FreeRTOS_CLIRegisterCommand(&xShowConfigCmd);
	FreeRTOS_CLIRegisterCommand(&xGetSysStatusCmd);
	FreeRTOS_CLIRegisterCommand(&xGetRtosStatsCmd);
	FreeRTOS_CLIRegisterCommand(&xGetCommStatsCmd);
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

BaseType_t xCLI_ShowConfig( char *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;

	static ConfigStruct_t config;
	static CONFStatus_t configStatus;
	BaseType_t retVal = pdTRUE;
	static uint8_t step = 0;

	switch (step){
		case 0:
			configStatus = Config_GetConfig(&config);
			switch(configStatus){
				case CONF_ERR_NOCONF:
					sprintf(pcWriteBuffer, "No configuration in memory.\r\n");
					break;
				case CONF_ERR_CRC:
					sprintf(pcWriteBuffer, "Configuration CRC error.\r\n");
					break;
				case CONF_ERR_BUSY:
					sprintf(pcWriteBuffer, "Cannot access I2C bus.\r\n");
					break;
				case CONF_ERR_TIMEOUT:
					sprintf(pcWriteBuffer, "Memory access timeout.\r\n");
					break;
				case CONF_OK:
					sprintf(pcWriteBuffer, "Configuration loaded.\r\n");
					break;
				default:
					sprintf(pcWriteBuffer, "Unexpected error.\r\n");
 					break;
 			}
			step++;
			break;

		case 1:
			sprintf(pcWriteBuffer, "Ethernet configuration:\r\n"
					"    IP: %d.%d.%d.%d\r\n"
					"    GW: %d.%d.%d.%d\r\n"
					"    Net Mask: %d.%d.%d.%d\r\n"
					"    MAC: %02d:%02d:%02d:%02d:%02d:%02d\r\n",
					config.EthernetConfig.ipAddress[0], config.EthernetConfig.ipAddress[1],
					config.EthernetConfig.ipAddress[2], config.EthernetConfig.ipAddress[3],
					config.EthernetConfig.gatewayAddress[0], config.EthernetConfig.gatewayAddress[1],
					config.EthernetConfig.gatewayAddress[2], config.EthernetConfig.gatewayAddress[3],
					config.EthernetConfig.subnetMask[0], config.EthernetConfig.subnetMask[1],
					config.EthernetConfig.subnetMask[2], config.EthernetConfig.subnetMask[3],
					config.EthernetConfig.macAddress[0], config.EthernetConfig.macAddress[1],
					config.EthernetConfig.macAddress[2], config.EthernetConfig.macAddress[3],
					config.EthernetConfig.macAddress[4], config.EthernetConfig.macAddress[5]);
			step++;
			break;

		case 2:
			sprintf(pcWriteBuffer, "Regulation configuration:\r\n"
					"    CWU heater power: %d W\r\n"
					"    CO heater power: %d W\r\n"
					"    Network max power: %d W\r\n",
					(uint32_t)config.RegulationConfig.CWU_heater_power,
					(uint32_t)config.RegulationConfig.CO_heater_power,
					(uint32_t)config.RegulationConfig.net_max_power);
			step++;
			break;

		case 3:
			sprintf(pcWriteBuffer, "    Counter address: %d\r\n"
					"    Temperature address: %d\r\n"
					"    Accumulated power coefficient: %d %\r\n"
					"    Heater power coefficient: %d %\r\n",
					config.RegulationConfig.counter_address,
					config.RegulationConfig.temperature_address,
					config.RegulationConfig.acumulated_power_coeff,
					config.RegulationConfig.heater_power_coeff);
			step++;
			break;

		case 4:
			sprintf(pcWriteBuffer, "    CO high temperature: %d C\r\n"
					"    CO low temperature: %d C\r\n"
					"    CWU high temperature: %d C\r\n"
					"    CWU low temperature: %d C\r\n",
					config.RegulationConfig.CO_hi_temp,
					config.RegulationConfig.CO_lo_temp,
					config.RegulationConfig.CWU_hi_temp,
					config.RegulationConfig.CWU_lo_temp);
			step++;
			break;

		case 5:
			sprintf(pcWriteBuffer, "Magic word: 0x%04x\r\n"
					"CRC: 0x%08x\r\n", config.dummy, config.crc);
			step++;
			break;

		default:
			step = 0;
			sprintf(pcWriteBuffer, "\r\n");
			retVal = pdFALSE;
			break;
	}

	return retVal;
}

BaseType_t xCLI_GetSystemStatus( char *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;

	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	return pdFALSE;
}

BaseType_t xCLI_GetCommStats( char *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;

	BaseType_t retVal = pdTRUE;
	static uint8_t step = 0;

	uint8_t ipArr[4];

	switch(step){

		case 0:
			RuntimeStats_TelnetGetCurIP(ipArr);
			sprintf(pcWriteBuffer, "Telnet statistics:\r\n"
					"    Current IP connected: %d.%d.%d.%d\r\n",
					ipArr[0], ipArr[1], ipArr[2], ipArr[3]);
			step++;
 			break;

		case 1:
			RuntimeStats_TelnetGetLastIP(ipArr);
			sprintf(pcWriteBuffer, "    Last IP connected: %d.%d.%d.%d\r\n",
					ipArr[0], ipArr[1], ipArr[2], ipArr[3]);
			step++;
			break;

		case 2:
			sprintf(pcWriteBuffer, "    TX count: %d\r\n    RX count: %d\r\n",
					RuntimeStats_TelnetGetTxCnt(), RuntimeStats_TelnetGetRxCnt());
			step++;
			break;

		case 3:
			sprintf(pcWriteBuffer, "Modbus statistics:\r\n"
					"    Requests send: %d\r\n"
					"    Responses received: %d\r\n"
					"    Requests not responded: %d\r\n"
					"    Wrong responds: %d\r\n",
					RuntimeStats_ModbusMasterRqSendGet(), RuntimeStats_ModbusMasterRespOkGet(),
					RuntimeStats_ModbusMasterNoRespGet(), RuntimeStats_ModbusMasterWrongRespGet());
			step++;
			break;

		default:
			step = 0;
			sprintf(pcWriteBuffer, "\r\n");
			retVal = pdFALSE;
			break;
	}
	return retVal;
}
