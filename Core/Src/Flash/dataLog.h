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
	DL_NO_FLASH,
	DL_OK = 0,
} DataLogStatus_t;

typedef enum {
	EV_STARTUP = 0,
	EV_RTC_UPDATE,
	EV_TELNET_CONN,
	EV_TELNET_DISCON,
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
		DataLogEventCode_t eventCode;
		uint32_t reserved;
	};
	uint8_t bytes[16];
} DataLogEvent_t;

DataLogStatus_t DataLog_Init (void);

DataLogStatus_t DataLog_LogEvent (DataLogEventCode_t event);

void DataLog_GetLastEvent (DataLogEvent_t* event);

DataLogStatus_t DataLog_GetLastEvents(DataLogEvent_t* events, uint32_t eventCount);

void DataLog_SaveEvent (DataLogEvent_t* event);

#endif /* SRC_FLASH_DATALOG_H_ */
