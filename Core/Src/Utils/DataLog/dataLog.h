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

/**
 * @defgroup dataLog
 * @{
 */
typedef enum{
	DL_NO_EVENTS,/**< DL_NO_EVENTS */
	DL_NO_FLASH, /**< DL_NO_FLASH */
	DL_OK = 0,   /**< DL_OK */
} DataLogStatus_t;

typedef enum {
	EV_STARTUP = 0, /**< Device powered up event */
	EV_RTC_UPDATE = 1, /**< RTC updated by user */
	EV_TELNET_CONN = 2, /**< Telnet client connected */
	EV_TELNET_DISCON = 3, /**< Telnet client disconnected */
	EV_TEST = 4, /**< Test event */
	EV_FLASH_CLR = 5, /**< Flash memory cleared */
	EV_CFG_CHANGE = 6, /**< Configuration changed */
	EV_EE_CLR = 7, /**< EEPROM cleared */
	EV_RTC_NTP_UPDATE = 8, /**< RTC updated by NTP */
	EVENT_CNT /**< Count of event codes */
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
		uint8_t eventCode;
		uint8_t additionalData[7];
	};
	uint8_t bytes[16];
} DataLogEvent_t;

/**
 * @brief Initializes data logger
 *
 * @return Returns DataLogStatus_t
 */
DataLogStatus_t DataLog_Init (void);

/**
 * @brief Adds event to log queue
 *
 * @param event Event to log, see DataLogEventCode_t
 * @param addData Additional event data pointer, up to 9 bytes
 * @param addDataLen Length of additional data
 * @return Returns DataLogStatus_t
 */
DataLogStatus_t DataLog_LogEvent (DataLogEventCode_t event, uint8_t* addData, uint8_t addDataLen);

/**
 * @brief Returns last event saved in memory
 *
 * @param event Pointer to save event
 */
void DataLog_GetLastEvent (DataLogEvent_t* event);

/**
 * @brief Reads given number of last events from memory
 *
 * @param events Place to save events
 * @param eventCount Number of events to read
 * @return Returns DataLogStatus_t
 */
DataLogStatus_t DataLog_GetLastEvents(DataLogEvent_t* events, uint32_t eventCount);

/**
 * @brief Gets all available events from memory
 *
 * @param events
 * @param event
 * @return
 */
DataLogStatus_t DataLog_GetLastEventsN(DataLogEvent_t* events, uint32_t event);

/**
 * @brief
 *
 * @param event
 */
void DataLog_SaveEvent (DataLogEvent_t* event);

/**
 * @brief
 *
 */
void DataLog_EreaseMemory (void);

/**
 * @brief
 *
 * @return
 */
uint32_t DataLog_GetCurrentMemPointer (void);

/**
 * @brief
 *
 * @param event
 * @param output
 * @return
 */
uint32_t DataLog_TranslateEvent (DataLogEvent_t* event, char* output);

/**
 * @brief
 *
 * @return
 */
uint32_t DataLog_GetSavedEventsCnt (void);

/**
 * @}
 *
 */
#endif /* SRC_FLASH_DATALOG_H_ */
