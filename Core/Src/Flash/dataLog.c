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

#define HEAD_ADDR 0
#define FIRSTLOG_ADDR

typedef union __attribute__((packed)){
	uint32_t lastSavedLog;
	uint8_t bytes[4];
} DataLogMemHead_t;

DataLogMemHead_t logHead;

extern osMessageQId DataLogQueueHandle;
extern w25qxx_t	w25qxx;
extern RTC_HandleTypeDef hrtc;

static void DataLog_UpdateMemPointer (uint8_t inc);

DataLogStatus_t DataLog_Init (void){
	DataLogStatus_t retval = DL_NO_FLASH;

	if(W25qxx_Init() == true){
		retval = DL_OK;
	}

	if(retval == DL_OK){
		DataLog_UpdateMemPointer(0);
		DataLog_LogEvent(EV_STARTUP);
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

	xQueueSend(DataLogQueueHandle, &eventS, 10);
}

void DataLog_GetLastEvent (DataLogEvent_t* event){
	W25qxx_ReadBytes(&logHead.bytes, HEAD_ADDR, sizeof(DataLogMemHead_t));
	W25qxx_ReadBytes((uint8_t*)event, logHead.lastSavedLog, sizeof(DataLogEvent_t));
}

DataLogStatus_t DataLog_GetLastEvents(DataLogEvent_t* events, uint32_t eventCount){

}

void DataLog_SaveEvent (DataLogEvent_t* event){
	W25qxx_ReadBytes(&logHead.bytes, HEAD_ADDR, sizeof(DataLogMemHead_t));

	for(int i = 0; i < 16; i++){
		W25qxx_WriteByte(event->bytes[i], logHead.lastSavedLog + i);
	}

	DataLog_UpdateMemPointer(1);
}

static void DataLog_UpdateMemPointer (uint8_t inc){
	uint8_t pageBuf[256];

	W25qxx_ReadBytes(&logHead.bytes, HEAD_ADDR, sizeof(DataLogMemHead_t));
	logHead.lastSavedLog += sizeof(DataLogEvent_t) * inc;

	if((logHead.lastSavedLog >= w25qxx.CapacityInKiloByte * 1024) || (logHead.lastSavedLog < w25qxx.PageSize)){
		logHead.lastSavedLog = w25qxx.PageSize;
	}
	memcpy(pageBuf, logHead.bytes, 4);
	W25qxx_WritePage(pageBuf, 0, 0, 0);
}
