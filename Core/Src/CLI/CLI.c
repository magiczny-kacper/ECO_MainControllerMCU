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
#include "../Flash/dataLog.h"
#include "../Tasks/EthernetTask/EthernetTask.h"
#include "../Tasks/RegulationTask/RegulationTask.h"

#include "FreeRTOS.h"

extern RTC_HandleTypeDef hrtc;

static const CLI_Command_Definition_t xEreaseEECmd = {
	"erease_eeprom",
	"erease_eeprom:\r\n    Ereases EEPROM memory.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_EreaseEE,
	0
};

static const CLI_Command_Definition_t xShowConfigCmd = {
	"show_config",
	"show_config:\r\n    Shows configuration.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_ShowConfig,
	0
};

static const CLI_Command_Definition_t xGetSysStatusCmd = {
	"get_system_status",
	"get_system_status:\r\n    Shows general system status, date, time, uptime.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_GetSystemStatus,
	0
};

static const CLI_Command_Definition_t xGetRtosStatsCmd = {
	"get_rtos_stats",
	"get_rtos_stats:\r\n    Shows RTOS runtime statistics.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_GetRtosStats,
	0
};

static const CLI_Command_Definition_t xGetCommStatsCmd = {
	"get_comm_stats",
	"get_comm_stats:\r\n    Shows communication statistics.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_GetCommStats,
	0
};

static const CLI_Command_Definition_t xWriteTestEventCmd = {
	"write_test_event",
	"write_test_event:\r\n    Writes test event into flash memory.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_WriteTestEvent,
	0
};

static const CLI_Command_Definition_t xEreaseFlashCmd = {
	"erease_flash",
	"erease_flash:\r\n    Ereases flash memory.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_EreaseFlash,
	0
};

static const CLI_Command_Definition_t xResetCmd = {
	"reset",
	"reset:\r\n    MCU software reset.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_Reset,
	0
};

static const CLI_Command_Definition_t xGetIOCmd = {
	"get_io",
	"get_io:\r\n    Shows input and outputs state.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_ShowIO,
	0
};

static const CLI_Command_Definition_t xGetLastEventCmd = {
	"get_last_event",
	"get_last_event:\r\n    Reads last saved event from memory.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_GetLastEvent,
	0
};

static const CLI_Command_Definition_t xGetLastEventsCmd = {
	"get_events",
	"get_events x:\r\n    Reads last x saved event from memory.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_GetEvents,
	1
};

static const CLI_Command_Definition_t xGetLastEventsRawCmd = {
	"get_events_raw",
	"get_events_raw x:\r\n    Reads last x saved event from memory.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_GetEventsRaw,
	1
};

static const CLI_Command_Definition_t xSetTimeCmd = {
	"set_time",
	"set_time hh mm ss:\r\n    Sets time.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_SetTime,
	3
};

static const CLI_Command_Definition_t xSetDateCmd = {
	"set_date",
	"set_date dd mm yy:\r\n    Sets date.\r\n",
	(const pdCOMMAND_LINE_CALLBACK) xCLI_SetDate,
	3
};

void CLI_Init (void){
	FreeRTOS_CLIRegisterCommand(&xEreaseEECmd);
	FreeRTOS_CLIRegisterCommand(&xEreaseFlashCmd);
	FreeRTOS_CLIRegisterCommand(&xShowConfigCmd);
	FreeRTOS_CLIRegisterCommand(&xGetSysStatusCmd);
	FreeRTOS_CLIRegisterCommand(&xGetRtosStatsCmd);
	FreeRTOS_CLIRegisterCommand(&xGetCommStatsCmd);
	FreeRTOS_CLIRegisterCommand(&xGetIOCmd);
	FreeRTOS_CLIRegisterCommand(&xGetLastEventCmd);
	FreeRTOS_CLIRegisterCommand(&xGetLastEventsCmd);
	FreeRTOS_CLIRegisterCommand(&xGetLastEventsRawCmd);
	FreeRTOS_CLIRegisterCommand(&xWriteTestEventCmd);
	FreeRTOS_CLIRegisterCommand(&xSetTimeCmd);
	FreeRTOS_CLIRegisterCommand(&xSetDateCmd);
	FreeRTOS_CLIRegisterCommand(&xResetCmd);
}

BaseType_t xCLI_EreaseEE( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;
	EE_StatusTypeDef retval = EE_Erease();
	if(retval == EE_OK){
		DataLog_LogEvent(EV_EE_CLR, NULL, 0);
		sprintf(pcWriteBuffer, "Wyczyszczono.\r\n");
	}else{
		sprintf(pcWriteBuffer, "Nie wyczyszczono.\r\n");
	}
	return pdFALSE;
}

BaseType_t xCLI_ShowConfig( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
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
					"    MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
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
					(int)config.RegulationConfig.CWU_heater_power,
					(int)config.RegulationConfig.CO_heater_power,
					(int)config.RegulationConfig.net_max_power);
			step++;
			break;

		case 3:
			sprintf(pcWriteBuffer, "    Counter address: %d\r\n"
					"    Temperature address: %d\r\n"
					"    Accumulated power coefficient: %d %\r\n"
					"    Heater power coefficient: %d %\r\n",
					(int)config.RegulationConfig.counter_address,
					(int)config.RegulationConfig.temperature_address,
					(int)config.RegulationConfig.acumulated_power_coeff,
					(int)config.RegulationConfig.heater_power_coeff);
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

BaseType_t xCLI_GetSystemStatus( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;

	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	sprintf(pcWriteBuffer, "    %02d.%02d.%02d %02d:%02d\r\n",
			date.Date, date.Month, date.Year, time.Hours, time.Minutes);
	return pdFALSE;
}

BaseType_t xCLI_GetRtosStats( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;
	vTaskGetRunTimeStats(pcWriteBuffer);
	return pdFALSE;
}

BaseType_t xCLI_GetCommStats( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
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
			sprintf(pcWriteBuffer, "Modbus RTU Master statistics:\r\n"
					"    Requests send: %d\r\n"
					"    Responses received: %d\r\n"
					"    Requests not responded: %d\r\n"
					"    Wrong responds: %d\r\n",
					RuntimeStats_ModbusMasterRqSendGet(), RuntimeStats_ModbusMasterRespOkGet(),
					RuntimeStats_ModbusMasterNoRespGet(), RuntimeStats_ModbusMasterWrongRespGet());
			step++;
			break;

		case 4:
			sprintf(pcWriteBuffer, "Modbus TCP Server statistics:\r\n"
					"    Requests received: %d\r\n"
					"    Proper requests received: %d\r\n"
					"    Wrong requests received: %d\r\n"
					"    Responds sent: %d\r\n",
					RuntimeStats_ModbusSlaveRqAllGet(), RuntimeStats_ModbusSlaveRqOkGet(),
					RuntimeStats_ModbusSlaveRqErrGet(), RuntimeStats_ModbusSlaveRespGet());
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

BaseType_t xCLI_WriteTestEvent( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;
	DataLog_LogEvent(EV_TEST, NULL, 0);
	sprintf(pcWriteBuffer, "    Done.\r\n    Current pointer: %d\r\n", DataLog_GetCurrentMemPointer());
	return pdFALSE;
}

BaseType_t xCLI_EreaseFlash( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;

	DataLog_EreaseMemory();
	sprintf(pcWriteBuffer, "    Done.\r\n");

	return pdFALSE;
}

BaseType_t xCLI_Reset( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;
	(void) pcWriteBuffer;
	HAL_NVIC_SystemReset();
	return pdFALSE;
}

BaseType_t xCLI_ShowIO( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;
	RegTaskData_t* ptr;

	ptr = RegulationTask_GetData();

	sprintf(pcWriteBuffer, "Inputs: \r\n"
			"    1:  %d\r\n    2:  %d\r\n"
			"    3:  %d\r\n    4:  %d\r\n"
			"    5:  %d\r\n    6:  %d\r\n"
			"    7:  %d\r\n    8:  %d\r\n"
			"    9:  %d\r\n    10: %d\r\n"
			"Outputs:\r\n"
			"    1:  %d\r\n    2:  %d\r\n"
			"    3:  %d\r\n    4:  %d\r\n"
			"    5:  %d\r\n    6:  %d\r\n",
			ptr->IOsignals.signals.input1, ptr->IOsignals.signals.input2,
			ptr->IOsignals.signals.input3, ptr->IOsignals.signals.input4,
			ptr->IOsignals.signals.input5, ptr->IOsignals.signals.input6,
			ptr->IOsignals.signals.input7, ptr->IOsignals.signals.input8,
			ptr->IOsignals.signals.input9, ptr->IOsignals.signals.input10,
			ptr->IOsignals.signals.output1, ptr->IOsignals.signals.output2,
			ptr->IOsignals.signals.output3, ptr->IOsignals.signals.output4,
			ptr->IOsignals.signals.output5, ptr->IOsignals.signals.output6);

	return pdFALSE;
}

BaseType_t xCLI_GetLastEvent( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;

	DataLogEvent_t lastEvent;

	DataLog_GetLastEvent(&lastEvent);

	DataLog_TranslateEvent(&lastEvent, pcWriteBuffer);

	return pdFALSE;
}

BaseType_t xCLI_GetEvents( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	xWriteBufferLen = xWriteBufferLen;
	char* parameter;
	int8_t len;
	uint32_t mul = 1;
	static uint32_t par = 0;
	static uint8_t step = 0;

	BaseType_t retVal = pdTRUE;
	DataLogEvent_t event;

	switch (step){
		case 0:
			parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 1, (BaseType_t*)&len);
			for(uint8_t i = len; i > 0; i--){
				par += (*(parameter + i - 1) - '0') * mul;
				mul *= 10;
			}
			if(par > DataLog_GetSavedEventsCnt()){
				par = DataLog_GetSavedEventsCnt();
				sprintf(pcWriteBuffer, "Only %d events to read...\r\n", (int)par);
			}else{
				sprintf(pcWriteBuffer, "Reading %d events...\r\n", (int)par);
			}
			step++;
			break;

		case 1:
			if(DL_OK == DataLog_GetLastEventsN(&event, par)){
				DataLog_TranslateEvent(&event, pcWriteBuffer);
				par --;
			}else{
				sprintf(pcWriteBuffer, "No more events.\r\n");
				par = 0;
			}
			if(par == 0){
				step++;
			}

			break;

		default:
			step = 0;
			sprintf(pcWriteBuffer, "\r\n");
			retVal = pdFALSE;
			break;
	}

	return retVal;
}

BaseType_t xCLI_GetEventsRaw( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	xWriteBufferLen = xWriteBufferLen;
	char* parameter;
	int8_t len;
	uint32_t mul = 1;
	static uint32_t par = 0;
	static uint8_t step = 0;

	BaseType_t retVal = pdTRUE;
	DataLogEvent_t event;

	switch (step){
		case 0:
			parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 1, (BaseType_t*)&len);
			for(uint8_t i = len; i > 0; i--){
				par += (*(parameter + i - 1) - '0') * mul;
				mul *= 10;
			}
			if(par > DataLog_GetSavedEventsCnt()){
				par = DataLog_GetSavedEventsCnt();
				sprintf(pcWriteBuffer, "Only %d events to read...\r\n", (int)par);
			}else{
				sprintf(pcWriteBuffer, "Reading %d events...\r\n", (int)par);
			}
			step++;
			break;

		case 1:
			if(DL_OK == DataLog_GetLastEventsN(&event, par)){
				sprintf(pcWriteBuffer, "%02d %02d %02d %02d %02d %02d %d %d %d %d %d %d %d %d %d %d \r\n",
						event.bytes[0], event.bytes[1], event.bytes[2], event.bytes[3],
						event.bytes[4], event.bytes[5], event.bytes[6], event.bytes[7],
						event.bytes[8], event.bytes[9], event.bytes[10], event.bytes[11],
						event.bytes[12], event.bytes[13], event.bytes[14], event.bytes[15]);
				par --;
			}else{
				sprintf(pcWriteBuffer, "No more events.\r\n");
				par = 0;
			}
			if(par == 0){
				step++;
			}

			break;

		default:
			step = 0;
			sprintf(pcWriteBuffer, "\r\n");
			retVal = pdFALSE;
			break;
	}

	return retVal;
}

BaseType_t xCLI_SetTime( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	xWriteBufferLen = xWriteBufferLen;
	char* parameter;
	uint8_t len;
	RTC_TimeTypeDef time;
	uint8_t hour, minute, second;

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 1, (BaseType_t*)&len);
	hour = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 2, (BaseType_t*)&len);
	minute = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 3, (BaseType_t*)&len);
	second = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	time.Hours = hour;
	time.Minutes = minute;
	time.Seconds = second;

	HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
	DataLog_LogEvent(EV_RTC_UPDATE, NULL, 0);
	sprintf(pcWriteBuffer, "    Time changed.\r\n");
	return pdFALSE;
}

BaseType_t xCLI_SetDate( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	xWriteBufferLen = xWriteBufferLen;
	char* parameter;
	uint8_t len;
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	uint8_t day, month, year;

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 1, (BaseType_t*)&len);
	day = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 2, (BaseType_t*)&len);
	month = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 3, (BaseType_t*)&len);
	year = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	date.Date = day;
	date.Month = month;
	date.Year = year;

	HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
	DataLog_LogEvent(EV_RTC_UPDATE, NULL, 0);
	sprintf(pcWriteBuffer, "    Date changed.\r\n");
	return pdFALSE;
}
