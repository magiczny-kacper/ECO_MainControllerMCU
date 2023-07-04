/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include "CLI.h"
#include "DateTimeMgr.h"
#include "printf.h"

#include "ConfigMgr.h"
#include "EEPROM.h"
#include "RuntimeStats.h"
#include "EventLog.h"
// #include "EthernetTask.h"
// #include "RegulationTask.h"

#include "task.h"

/******************************************************************************/
/*  Local defines                                                             */
/******************************************************************************/
#define CLI_COMMAND(name, cnt, cmd, help)

#define CLI_COMMANDS \
	CLI_COMMAND(EreaseEE,         0, "erease_eeprom",     "Erases EEPROM memory.\r\n"                            ) \
	CLI_COMMAND(EreaseFlash,      0, "erease_flash",      "Ereases flash memory.\r\n"                            ) \
	CLI_COMMAND(GetCommStats,     0, "get_comm_stats",    "Shows communication statistics.\r\n"                  ) \
	CLI_COMMAND(GetIO,            0, "get_io",            "Shows input and outputs state.\r\n"                   ) \
	CLI_COMMAND(GetLastEvent,     0, "get_last_event",    "Reads last saved event from memory.\r\n"              ) \
	CLI_COMMAND(GetLastEvents,    1, "get_events",        "Reads last x saved event from memory.\r\n"            ) \
	CLI_COMMAND(GetLastEventsRaw, 1, "get_events_raw",    "Reads last x saved event from memory.\r\n"            ) \
	CLI_COMMAND(GetRtosStats,     0, "get_rtos_stats",    "Shows RTOS runtime statistics.\r\n"                   ) \
	CLI_COMMAND(GetSystemStatus,  0, "get_system_status", "Shows general system status, date, time, uptime.\r\n" ) \
	CLI_COMMAND(Reset,            0, "reset",             "MCU software reset.\r\n"                              ) \
	CLI_COMMAND(ShowConfig,       0, "show_config",       "Shows configuration.\r\n"                             ) \
	CLI_COMMAND(SetDate,          3, "set_date",          "Sets date yy mm dd.\r\n"                              ) \
	CLI_COMMAND(SetTime,          3, "set_time",          "Sets time hh mm ss.\r\n"                              ) \
	CLI_COMMAND(WriteTestEvent,   0, "write_test_event",  "Writes test event into flash memory.\r\n"             ) \

/******************************************************************************/
/*  Local types                                                               */
/******************************************************************************/

/******************************************************************************/
/*  Local variables                                                           */
/******************************************************************************/
#ifdef CLI_COMMAND
#undef CLI_COMMAND
#endif

#define CLI_COMMAND(name, cnt, cmd, help) \
	BaseType_t xCLI_##name(char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString);

CLI_COMMANDS

#ifdef CLI_COMMAND
#undef CLI_COMMAND
#endif

#define CLI_COMMAND(name, cnt, cmd, help) \
	static const CLI_Command_Definition_t xCLI_##name##Cmd = { \
		cmd, cmd":\r\n    "help"\r\n", (const pdCOMMAND_LINE_CALLBACK) xCLI_##name, cnt \
    };

CLI_COMMANDS

/******************************************************************************/
/*  Static function prototypes                                                */
/******************************************************************************/

/******************************************************************************/
/*  Global function declarations                                              */
/******************************************************************************/
#ifdef CLI_COMMAND
#undef CLI_COMMAND
#endif

#define CLI_COMMAND(name, cnt, cmd, help) FreeRTOS_CLIRegisterCommand(&xCLI_##name##Cmd);

void CLI_Init (void){
	CLI_COMMANDS
}

BaseType_t xCLI_EreaseEE( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;
	EE_Status_t retval = EE_Erase();
	if(retval == EE_OK){
		DataLog_LogEvent(EV_EE_CLR, NULL, 0);
		sprintf(pcWriteBuffer, "Erased.\r\n");
	}else{
		sprintf(pcWriteBuffer, "Not erased.\r\n");
	}
	return pdFALSE;
}

BaseType_t xCLI_ShowConfig( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;

	static ConfigStruct_t config;
	static ConfigStatus_t configStatus;
	BaseType_t retVal = pdTRUE;
	static uint8_t step = 0;

	switch (step){
		case 0:
			configStatus = ConfigMgr_GetConfig(&config);
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
					"\tIP: %d.%d.%d.%d\r\n"
					"\tGW: %d.%d.%d.%d\r\n"
					"\tNet Mask: %d.%d.%d.%d\r\n"
					"\tMAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
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
					"\tCWU heater power: %d W\r\n"
					"\tCO heater power: %d W\r\n"
					"\tNetwork max power: %d W\r\n",
					(int)config.RegulationConfig.CWU_heater_power,
					(int)config.RegulationConfig.CO_heater_power,
					(int)config.RegulationConfig.net_max_power);
			step++;
			break;

		case 3:
			sprintf(pcWriteBuffer,
					"\tCounter address: %d\r\n"
					"\tTemperature address: %d\r\n"
					"\tAccumulated power coefficient: %d %%\r\n"
					"\tHeater power coefficient: %d %%\r\n",
					(int)config.RegulationConfig.counter_address,
					(int)config.RegulationConfig.temperature_address,
					(int)config.RegulationConfig.acumulated_power_coeff,
					(int)config.RegulationConfig.heater_power_coeff);
			step++;
			break;

		case 4:
			sprintf(pcWriteBuffer, "\tCO high temperature: %d C\r\n"
					"\tCO low temperature: %d C\r\n"
					"\tCWU high temperature: %d C\r\n"
					"\tCWU low temperature: %d C\r\n",
					config.RegulationConfig.CO_hi_temp,
					config.RegulationConfig.CO_lo_temp,
					config.RegulationConfig.CWU_hi_temp,
					config.RegulationConfig.CWU_lo_temp);
			step++;
			break;

		case 5:
			sprintf(pcWriteBuffer, "Magic word: 0x%04x\r\n"
					"CRC: 0x%08x\r\n", (unsigned int)config.dummy,
					(unsigned int)config.crc);
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

	Timestamp_t timestamp;
    DateTimeMgr_GetTimestamp(&timestamp);

	sprintf(pcWriteBuffer, "\t%02d.%02d.%02d %02d:%02d\r\n",
			timestamp.day, timestamp.month, timestamp.year, timestamp.hours, timestamp.minutes);
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
			sprintf(pcWriteBuffer, "\tTX count: %lu\r\n\tRX count: %lu\r\n",
					RuntimeStats_Telnet_TxFrames_Get(), RuntimeStats_Telnet_RxFrames_Get());
			step++;
			break;

		case 3:
			sprintf(pcWriteBuffer, "Modbus RTU Master statistics:\r\n"
					"    Requests send: %lu\r\n"
					"    Responses received: %lu\r\n"
					"    Requests not responded: %lu\r\n"
					"    Wrong responds: %lu\r\n",
					RuntimeStats_ModbusMaster_AllRequests_Get(), RuntimeStats_ModbusMaster_OkResponses_Get(),
					RuntimeStats_ModbusMaster_NoResponses_Get(), RuntimeStats_ModbusMaster_WrongResponses_Get());
			step++;
			break;

//		case 4:
//			sprintf(pcWriteBuffer, "Modbus TCP Server statistics:\r\n"
//					"    Requests received: %lu\r\n"
//					"    Proper requests received: %lu\r\n"
//					"    Wrong requests received: %lu\r\n"
//					"    Responds sent: %lu\r\n",
//					RuntimeStats_ModbusSlave_AllRequests_Get(), RuntimeStats_);
//					step++;
//					break;

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
	sprintf(pcWriteBuffer, "    Done.\r\n");
	return pdFALSE;
}

BaseType_t xCLI_EreaseFlash( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;
	DataLog_ClearAll();
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

BaseType_t xCLI_GetIO( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void) pcCommandString;
	(void) xWriteBufferLen;
	// RegTaskData_t* ptr;

	// ptr = RegulationTask_GetData();

	// sprintf(pcWriteBuffer, "Inputs: \r\n"
	// 		"    1:  %d    2:  %d\r\n"
	// 		"    3:  %d    4:  %d\r\n"
	// 		"    5:  %d    6:  %d\r\n"
	// 		"    7:  %d    8:  %d\r\n"
	// 		"    9:  %d    10: %d\r\n"
	// 		"Outputs:\r\n"
	// 		"    1:  %d    2:  %d\r\n"
	// 		"    3:  %d    4:  %d\r\n"
	// 		"    5:  %d    6:  %d\r\n",
	// 		ptr->IOsignals.signals.input1, ptr->IOsignals.signals.input2,
	// 		ptr->IOsignals.signals.input3, ptr->IOsignals.signals.input4,
	// 		ptr->IOsignals.signals.input5, ptr->IOsignals.signals.input6,
	// 		ptr->IOsignals.signals.input7, ptr->IOsignals.signals.input8,
	// 		ptr->IOsignals.signals.input9, ptr->IOsignals.signals.input10,
	// 		ptr->IOsignals.signals.output1, ptr->IOsignals.signals.output2,
	// 		ptr->IOsignals.signals.output3, ptr->IOsignals.signals.output4,
	// 		ptr->IOsignals.signals.output5, ptr->IOsignals.signals.output6);

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

BaseType_t xCLI_GetLastEvents( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void)xWriteBufferLen;
    
	char* parameter;
	int8_t len;
	uint32_t mul = 1;
	static uint32_t event_count = 0;
	static uint8_t step = 0;

	BaseType_t retVal = pdTRUE;
	DataLogEvent_t event;

	switch (step){
		case 0:
			parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 1, (BaseType_t*)&len);
			for(uint8_t i = len; i > 0; i--){
				event_count += (*(parameter + i - 1) - '0') * mul;
				mul *= 10;
			}
			if(event_count > DataLog_GetSavedEventsCount()){
				event_count = DataLog_GetSavedEventsCount();
				sprintf(pcWriteBuffer, "Only %lu events to read...\r\n", event_count);
			}else{
				sprintf(pcWriteBuffer, "Reading %lu events...\r\n", event_count);
			}
			step++;
			break;

		case 1:
			if(DL_OK == DataLog_GetLastEventByIndex(&event, event_count)){
				DataLog_TranslateEvent(&event, pcWriteBuffer);
				event_count --;
			}else{
				sprintf(pcWriteBuffer, "No more events.\r\n");
				event_count = 0;
			}
			if(event_count == 0){
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

BaseType_t xCLI_GetLastEventsRaw( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	xWriteBufferLen = xWriteBufferLen;
	char* parameter;
	int8_t len;
	uint32_t mul = 1;
    uint32_t saved_event_count = 0;
	static uint32_t event_count = 0;
	static uint8_t step = 0;

	BaseType_t retVal = pdTRUE;
	DataLogEvent_t event;

	switch (step){
		case 0:
			parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 1, (BaseType_t*)&len);
			for(uint8_t i = len; i > 0; i--){
				event_count += (*(parameter + i - 1) - '0') * mul;
				mul *= 10;
			}
            saved_event_count = DataLog_GetSavedEventsCount();
			if(event_count > saved_event_count){
				event_count = saved_event_count;
				sprintf(pcWriteBuffer, "Only %d events to read...\r\n", (int)event_count);
			}else{
				sprintf(pcWriteBuffer, "Reading %d events...\r\n", (int)event_count);
			}
			step++;
			break;

		case 1:
			if(DL_OK == DataLog_GetLastEventByIndex(&event, event_count)){
				sprintf(pcWriteBuffer, "%02d-%02d-%02d %02d:%02d:%02d %d %d %d %d %d %d %d %d \r\n",
						event.timestamp.year, 
                        event.timestamp.month, 
                        event.timestamp.day, 
                        event.timestamp.hours,
						event.timestamp.minutes, 
                        event.timestamp.seconds, 
                        event.eventCode, 
                        event.additionalData[0], event.additionalData[1],
                        event.additionalData[2], event.additionalData[3],
                        event.additionalData[4], event.additionalData[5],
                        event.additionalData[6]
                );
				event_count --;
			}else{
				sprintf(pcWriteBuffer, "No more events.\r\n");
				event_count = 0;
			}
			if(event_count == 0){
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
	(void)xWriteBufferLen;
	char* parameter;
	uint8_t len;
    Timestamp_t time;
	uint8_t hour, minute, second;

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 1, (BaseType_t*)&len);
	hour = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 2, (BaseType_t*)&len);
	minute = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 3, (BaseType_t*)&len);
	second = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	time.hours = hour;
	time.minutes = minute;
	time.seconds = second;
    DateTimeMgr_SetTime(&time);
	sprintf(pcWriteBuffer, "    Time changed.\r\n");
	return pdFALSE;
}

BaseType_t xCLI_SetDate( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString ){
	(void)xWriteBufferLen;
    
	char* parameter;
	uint8_t len;
	Timestamp_t date;
	uint8_t day, month, year;

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 1, (BaseType_t*)&len);
	day = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 2, (BaseType_t*)&len);
	month = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	parameter = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 3, (BaseType_t*)&len);
	year = (*parameter - '0') * 10 + (*(parameter + 1) - '0');

	date.day = day;
	date.month = month;
	date.year = year;

    DateTimeMgr_SetDate(&date);
	sprintf(pcWriteBuffer, "    Date changed.\r\n");
	return pdFALSE;
}

/**
 * @}
 */
