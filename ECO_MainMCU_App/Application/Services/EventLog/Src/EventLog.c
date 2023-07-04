/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>
#include <string.h>

#include "DateTimeMgr.h"
#include "EventLog.h"

#include "FreeRTOS.h"
#include "portmacro.h"
#include "projdefs.h"
#include "task.h"
#include "queue.h"

#include "crc.h"
#include "printf.h"

#include "SPI_Flash.h"

/******************************************************************************/
/*  Local defines                                                             */
/******************************************************************************/
#define EVENT_Q_ELEMENT_COUNT 10
/******************************************************************************/
/*  Local types                                                               */
/******************************************************************************/
typedef struct {
    DataLogEvent_t event;
    uint32_t index;
    uint16_t crc;
} DataLogEntry_t;

/******************************************************************************/
/*  Local variables                                                           */
/******************************************************************************/
static uint32_t next_log_index = 0;
static DataLogEvent_t xLastEvent;
static QueueHandle_t xEventQueue;
static StaticQueue_t xStaticEventQueue;
static DataLogEvent_t eventQueueBuffer[EVENT_Q_ELEMENT_COUNT];
static TaskHandle_t xDataLogThread;
extern RTC_HandleTypeDef hrtc;

#define EVENT_FORMAT_STRING "%02d.%02d.%02d %02d:%02d:%02d %s\r\n"

#ifdef SYS_EVENT
#undef SYS_EVENT
#endif

#define SYS_EVENT(name, text) text,

static const char* eventMessages[EVENT_CNT] = {
	SYS_EVENTS
};

static const DataLogEvent_t corrupted_event = {
    .timestamp.timestamp = 0,
    .eventCode = EV_CORRUPTED,
    .additionalData = {0}
};

/******************************************************************************/
/*  Static function prototypes                                                */
/******************************************************************************/
static void DataLog_RestoreIndex(void);
static void DataLog_SaveEvent(DataLogEvent_t* xEvent);
static void DataLog_GetTimestampToEvent(DataLogEvent_t* event);
static bool DataLog_CheckEvent(DataLogEntry_t* event);
static uint16_t DataLog_CalculateCRC(DataLogEntry_t* event);
static void DataLog_MemWrite(void* data, uint32_t size, uint32_t address);
static void DataLog_MemRead(void* data, uint32_t size, uint32_t address);
static void DataLogTask(void* argument);

/******************************************************************************/
/*  Global function declarations                                              */
/******************************************************************************/
void DataLog_Init (void){
    xEventQueue = xQueueCreateStatic(
        EVENT_Q_ELEMENT_COUNT, 
        sizeof(DataLogEvent_t), 
        (uint8_t*)eventQueueBuffer, 
        &xStaticEventQueue
    );
    (void)xTaskCreate(
        DataLogTask, "DataLog", 512 / 4,
        (void*) NULL, 2, &xDataLogThread
    );
}

void DataLog_LogEvent (DataLogEventCode_t event, uint8_t* addData, uint8_t addDataLen){
	DataLogEvent_t xEventTemp;
    memset(&xEventTemp, 0xFF, sizeof(DataLogEvent_t));
    DataLog_GetTimestampToEvent(&xEventTemp);
	xEventTemp.eventCode = event;
	if((addData != NULL) && (addDataLen != 0)){
		memcpy(xEventTemp.additionalData, addData, addDataLen);
	}
	if(0 == xQueueSend(xEventQueue, &xEventTemp, 20))
    {
        __NOP();
    }
}

void DataLog_GetLastEvent (DataLogEvent_t* event)
{
    DataLogEntry_t entry;
    if(NULL == event)
    {
        return;
    }
    DataLog_MemRead((void*)&entry, sizeof(DataLogEntry_t), sizeof(DataLogEntry_t) * (next_log_index - 1));
    if(DataLog_CheckEvent(&entry))
    {
        memcpy(event, &entry.event, sizeof(DataLogEvent_t));
    }
    else
    {
        memcpy(event, &corrupted_event, sizeof(DataLogEvent_t));
    }
}

DataLogStatus_t DataLog_GetLastEventByIndex(DataLogEvent_t* event, uint32_t index)
{
    uint32_t read_address;
    DataLogEntry_t entry;
    if((NULL == event) || (index >= next_log_index))
    {
        return DL_NO_EVENTS;
    }
    read_address = sizeof(DataLogEntry_t) * ((next_log_index - 1) - index);
    DataLog_MemRead((void*)&entry, sizeof(DataLogEntry_t), read_address);
    if(DataLog_CheckEvent(&entry))
    {
        memcpy(event, &entry.event, sizeof(DataLogEvent_t));
    }
    else
    {
        memcpy(event, &corrupted_event, sizeof(DataLogEvent_t));
    }
    return DL_OK;
}

uint32_t DataLog_TranslateEvent (DataLogEvent_t* event, char* output){
	uint32_t len = 0;
	if(event->eventCode < EVENT_CNT){
		len = sprintf(output, EVENT_FORMAT_STRING,
				event->timestamp.day, event->timestamp.month,
				event->timestamp.year, event->timestamp.hours,
				event->timestamp.minutes, event->timestamp.seconds,
				eventMessages[event->eventCode]
			);
	}
	return len;
}

uint32_t DataLog_GetSavedEventsCount(void)
{
    return next_log_index - 1;
}

void DataLog_ClearAll(void)
{
    SPI_Flash_ErasePartition(DATA_LOG_AREA);
}

/******************************************************************************/
/*  Static function declarations                                              */
/******************************************************************************/
static void DataLog_RestoreIndex(void)
{
    DataLogEntry_t current_entry, next_entry;
    uint32_t read_address = 0;
    
    DataLog_MemRead((void*)&current_entry, sizeof(DataLogEntry_t), read_address);
    read_address += sizeof(DataLogEntry_t);
    
    if(!DataLog_CheckEvent(&current_entry)) return;
    
    
    while(1)
    {
        DataLog_MemRead((void*)&next_entry, sizeof(DataLogEntry_t), read_address);
        read_address += sizeof(DataLogEntry_t);
        if(!DataLog_CheckEvent(&current_entry) || 
            (next_entry.index - current_entry.index != 1))
        {
            break;
        }
        memcpy(&next_entry, &current_entry, sizeof(DataLogEntry_t));
    }
    next_log_index = current_entry.index + 1;
}

static void DataLog_SaveEvent(DataLogEvent_t* xEvent)
{
    DataLogEntry_t xEventEntry;
    uint32_t save_address;

    if(NULL == xEvent)
    {
        return;
    }

    memset(&xEventEntry, 0xFF, sizeof(DataLogEntry_t));
    memcpy(&xEventEntry.event, xEvent, sizeof(DataLogEvent_t));

    xEventEntry.index = next_log_index;
    xEventEntry.crc = DataLog_CalculateCRC(&xEventEntry);
    save_address = next_log_index * sizeof(DataLogEntry_t);

    // DataLog_MemWrite((void*)&xEventEntry, sizeof(DataLogEntry_t), save_address);
    next_log_index++;
}

static void DataLog_GetTimestampToEvent(DataLogEvent_t* event)
{
    DateTimeMgr_GetTimestamp(&event->timestamp);
}

static bool DataLog_CheckEvent(DataLogEntry_t* event)
{
    uint16_t calculated_crc;
    if(NULL == event)
    {
        return false;
    }

    calculated_crc = DataLog_CalculateCRC(event);
    return (event -> crc == calculated_crc);
}

static uint16_t DataLog_CalculateCRC(DataLogEntry_t* event)
{
    (void)event;
    return 0;
}

static void DataLog_MemWrite(void* data, uint32_t size, uint32_t address)
{
    uint32_t partition_size;
    if(NULL == data)
    {
        return;
    }
    partition_size = SPI_Flash_GetPartitionSize(DATA_LOG_AREA);
    if(address >= partition_size)
    {
        address = address % partition_size;
    }
    SPI_Flash_Write(DATA_LOG_AREA, data, address, size);
}

static void DataLog_MemRead(void* data, uint32_t size, uint32_t address)
{
    uint32_t partition_size;
    if(NULL == data)
    {
        return;
    }
    partition_size = SPI_Flash_GetPartitionSize(DATA_LOG_AREA);
    if(address >= partition_size)
    {
        address = address % partition_size;
    }
    SPI_Flash_Read(DATA_LOG_AREA, data, address, size);
}

static void DataLogTask(void* argument)
{
	(void) argument;
	DataLogEvent_t event;
    
    memcpy(&xLastEvent, &corrupted_event, sizeof(DataLogEvent_t));
    // DataLog_RestoreIndex();

	for(;;)
	{
		if(pdPASS == xQueueReceive(xEventQueue, &event, portMAX_DELAY))
		{
			DataLog_SaveEvent(&event);
		}
	}
}
