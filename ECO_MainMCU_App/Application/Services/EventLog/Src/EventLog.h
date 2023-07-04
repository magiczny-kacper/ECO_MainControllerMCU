#ifndef EVENTLOG_H_
#define EVENTLOG_H_

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "DateTimeMgr.h"

/******************************************************************************/
/*  Global defines                                                            */
/******************************************************************************/
#define SYS_EVENTS \
	SYS_EVENT(STARTUP, "Device start up.") \
	SYS_EVENT(RTC_UPDATE, "RTC Update.") \
	SYS_EVENT(TELNET_CONN, "Telnet connected.") \
	SYS_EVENT(TELNET_DISCON, "Telnet disconnected.") \
	SYS_EVENT(TEST, "Test event") \
	SYS_EVENT(FLASH_CLR, "Flash ereased.") \
	SYS_EVENT(CFG_CHANGE, "Configuration changed.") \
	SYS_EVENT(EE_CLR, "EEPROM ereased.") \
	SYS_EVENT(RTC_NTP_UPDATE, "RTC updated from NTP.") \
    SYS_EVENT(CORRUPTED, "Event corrupted.")
    
/******************************************************************************/
/*  Global types                                                              */
/******************************************************************************/
typedef enum
{
	DL_QUEUE_ERR,
	DL_NO_EVENTS,/**< DL_NO_EVENTS */
	DL_NO_FLASH, /**< DL_NO_FLASH */
	DL_OK = 0,   /**< DL_OK */
} DataLogStatus_t;

#ifdef SYS_EVENT
#undef SYS_EVENT
#endif

#define SYS_EVENT(name, text) EV_##name,

typedef enum
{
	SYS_EVENTS
	EVENT_CNT
} DataLogEventCode_t;

typedef Timestamp_t EventTimestamp_t;

typedef struct
{
    EventTimestamp_t timestamp;
    DataLogEventCode_t eventCode;
    uint8_t additionalData[12];
} DataLogEvent_t;

/******************************************************************************/
/*  Global function prototypes                                                */
/******************************************************************************/

/**
 * @brief Initializes data logger
 * 
 */
void DataLog_Init (void);

/**
 * @brief Adds event to logging queue
 * 
 * @param event Event code
 * @param addData Additional event data
 * @param addDataLen Length of additional data
 */
void DataLog_LogEvent (DataLogEventCode_t event, uint8_t* addData, uint8_t addDataLen);

/**
 * @brief Gets last logged event
 * 
 * @param event Pointer to save event
 */
void DataLog_GetLastEvent (DataLogEvent_t* event);

/**
 * @brief Gets given coun tof last events
 * 
 * @param events Pointer to save events
 * @param eventCount Count of events to read
 * @return DataLogStatus_t Read status
 */
DataLogStatus_t DataLog_GetLastEvents(DataLogEvent_t* events, uint32_t eventCount);

/**
 * @brief Gets previous event by index (last event index - index)
 * 
 * @param events Pointer to save event
 * @param index Event index to read
 * @return DataLogStatus_t Read status
 */
DataLogStatus_t DataLog_GetLastEventByIndex(DataLogEvent_t* events, uint32_t index);

/**
 * @brief Converts event to string
 * 
 * @param event Event to convert
 * @param output Pointer to save string
 * @return uint32_t Number of bytes written
 */
uint32_t DataLog_TranslateEvent (DataLogEvent_t* event, char* output);

/**
 * @brief Returns saved events count
 * 
 * @return uint32_t Saved events count
 */
uint32_t DataLog_GetSavedEventsCount(void);

/**
 * @brief Erases data logger partition
 * 
 */
void DataLog_ClearAll(void);

#endif /* EVENTLOG_H_ */
