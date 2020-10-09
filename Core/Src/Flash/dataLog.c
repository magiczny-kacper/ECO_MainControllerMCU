/*
 * dataLog.c
 *
 *  Created on: 5 wrz 2020
 *      Author: magiczny_kacper
 */

#include "dataLog.h"
#include "w25qxx.h"
#include "queue.h"

#include "cmsis_os.h"

#include "stm32f4xx_hal.h"

#define HEAD_ADDR 0
#define FIRSTLOG_ADDR

typedef union{
	uint32_t nextSavedLog;
	uint8_t bytes[4];
} DataLogMemHead_t;

static uint8_t pageBuf[256];
static DataLogMemHead_t logHead;

extern osMessageQId DataLogQueueHandle;
extern w25qxx_t	w25qxx;
extern RTC_HandleTypeDef hrtc;

static const char* eventsStr[EVENT_CNT] = {
	"%02d.%02d.%02d %02d:%02d:%02d Device start up.\r\n",
	"%02d.%02d.%02d %02d:%02d:%02d RTC Update.\r\n",
	"%02d.%02d.%02d %02d:%02d:%02d Telnet connected.\r\n",
	"%02d.%02d.%02d %02d:%02d:%02d Telnet disconnected.\r\n",
	"%02d.%02d.%02d %02d:%02d:%02d Test event.\r\n",
	"%02d.%02d.%02d %02d:%02d:%02d Flash ereased.\r\n",
	"%02d.%02d.%02d %02d:%02d:%02d Configuration changed.\r\n",
	"%02d.%02d.%02d %02d:%02d:%02d EEPROM ereased.\r\n",
};

static void DataLog_UpdateMemPointer (uint8_t inc);

DataLogStatus_t DataLog_Init (void){
	DataLogStatus_t retval = DL_NO_FLASH;
	DataLogEvent_t lastEvent;
	if(W25qxx_Init() == true){
		retval = DL_OK;
	}

	logHead.nextSavedLog = HAL_RTCEx_BKUPRead(&hrtc, 1);
	if(retval == DL_OK){
		DataLog_LogEvent(EV_STARTUP);

		DataLog_GetLastEvent(&lastEvent);
	}
	return retval;
}

DataLogStatus_t DataLog_LogEvent (DataLogEventCode_t event){
	DataLogEvent_t eventS;
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	eventS.timestamp.fields.seconds = time.Seconds;
	eventS.timestamp.fields.minutes = time.Minutes;
	eventS.timestamp.fields.hours = time.Hours;
	eventS.timestamp.fields.day = date.Date;
	eventS.timestamp.fields.month = date.Month;
	eventS.timestamp.fields.year = date.Year;
	eventS.timestamp.fields.empty = 0;

	eventS.eventCode = event;
	eventS.reserved = 0;
	xQueueSend(DataLogQueueHandle, &eventS, 10);
}

void DataLog_GetLastEvent (DataLogEvent_t* event){
	W25qxx_ReadBytes((uint8_t*)event, logHead.nextSavedLog - 16, 16);
}

DataLogStatus_t DataLog_GetLastEvents(DataLogEvent_t* events, uint32_t eventCount){
	uint32_t eventsSaved;
	eventsSaved = logHead.nextSavedLog / sizeof(DataLogEvent_t);
	if(eventsSaved == 0){
		return DL_NO_EVENTS;
	}
	if(eventsSaved > eventCount){
		eventsSaved = eventCount;
	}
	W25qxx_ReadBytes((uint8_t*)events, logHead.nextSavedLog - eventsSaved, eventsSaved * 16);
	return DL_OK;
}

DataLogStatus_t DataLog_GetLastEventsN(DataLogEvent_t* events, uint32_t event){
	uint32_t eventsSaved;
	eventsSaved = DataLog_GetSavedEventsCnt();
	if((eventsSaved == 0) || (event > eventsSaved)){
		return DL_NO_EVENTS;
	}

	W25qxx_ReadBytes((uint8_t*)events, logHead.nextSavedLog - (event * 16), 16);
	return DL_OK;
}
/**
 *
 * @param event
 */
void DataLog_SaveEvent (DataLogEvent_t* event){
	for(int i = 0; i < 16; i++){
		W25qxx_WriteByte(event->bytes[i], logHead.nextSavedLog + i);
	}
	DataLog_UpdateMemPointer(1);
}

void DataLog_EreaseMemory (void){
	W25qxx_EraseChip();
	logHead.nextSavedLog = 0;
	HAL_RTCEx_BKUPWrite(&hrtc, 1, logHead.nextSavedLog);
	DataLog_LogEvent(EV_FLASH_CLR);
}

uint32_t DataLog_GetCurrentMemPointer (void){
	return logHead.nextSavedLog;
}

uint32_t DataLog_TranslateEvent (DataLogEvent_t* event, char* output){
	uint32_t len = 0;
	if(event->eventCode < EVENT_CNT){
		len = sprintf(output, eventsStr[event->eventCode], event->timestamp.fields.day,
				event->timestamp.fields.month, event->timestamp.fields.year,
				event->timestamp.fields.hours, event->timestamp.fields.minutes,
				event->timestamp.fields.seconds);
	}
	return len;
}

uint32_t DataLog_GetSavedEventsCnt (void){
	return logHead.nextSavedLog / 16;
}
/**
 *
 * @param inc
 */
static void DataLog_UpdateMemPointer (uint8_t inc){
	logHead.nextSavedLog += sizeof(DataLogEvent_t);

	if(logHead.nextSavedLog >= w25qxx.CapacityInKiloByte * 1024){
		logHead.nextSavedLog = 0;
	}

	HAL_RTCEx_BKUPWrite(&hrtc, 1, logHead.nextSavedLog);
}
