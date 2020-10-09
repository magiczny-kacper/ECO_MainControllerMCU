/*
 * dataLog.h
 *
 *  Created on: 5 wrz 2020
 *      Author: magiczny_kacper
 */

#ifndef SRC_FLASH_DATALOG_H_
#define SRC_FLASH_DATALOG_H_

#include <stdint.h>
#include "FreeRTOS.h"

typedef enum{
	DL_NO_EVENTS,
	DL_NO_FLASH,
	DL_OK = 0,
} DataLogStatus_t;

typedef enum {
	EV_STARTUP = 0,
	EV_RTC_UPDATE = 1,
	EV_TELNET_CONN = 2,
	EV_TELNET_DISCON = 3,
	EV_TEST = 4,
	EV_FLASH_CLR = 5,
	EV_CFG_CHANGE = 6,
	EV_EE_CLR = 7,
	EVENT_CNT
} DataLogEventCode_t;




typedef union __attribute__((packed)){
	struct{
		uint8_t seconds;
		uint8_t minutes;
		uint8_t hours;
		uint8_t day;
		uint8_t month;
		uint8_t year;
		uint16_t empty;
	} fields;
	uint64_t timestamp;
} EventTimestamp_t;

typedef union __attribute__((packed)){
	struct{
		EventTimestamp_t timestamp;
		uint32_t eventCode;
		uint32_t reserved;
	};
	uint8_t bytes[16];
} DataLogEvent_t;

DataLogStatus_t DataLog_Init (void);

DataLogStatus_t DataLog_LogEvent (DataLogEventCode_t event);

void DataLog_GetLastEvent (DataLogEvent_t* event);

DataLogStatus_t DataLog_GetLastEvents(DataLogEvent_t* events, uint32_t eventCount);

DataLogStatus_t DataLog_GetLastEventsN(DataLogEvent_t* events, uint32_t event);

void DataLog_SaveEvent (DataLogEvent_t* event);

void DataLog_EreaseMemory (void);

uint32_t DataLog_GetCurrentMemPointer (void);

uint32_t DataLog_TranslateEvent (DataLogEvent_t* event, char* output);

uint32_t DataLog_GetSavedEventsCnt (void);

#endif /* SRC_FLASH_DATALOG_H_ */
