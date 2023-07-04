/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>

#include "DateTimeMgr.h"
#include "EventLog.h"
#include "rtc.h"

/******************************************************************************/
/*  Local defines                                                             */
/******************************************************************************/

/******************************************************************************/
/*  Local types                                                               */
/******************************************************************************/

/******************************************************************************/
/*  Local variables                                                           */
/******************************************************************************/
extern RTC_HandleTypeDef hrtc;

/******************************************************************************/
/*  Static function prototypes                                                */
/******************************************************************************/

/******************************************************************************/
/*  Global function declarations                                              */
/******************************************************************************/
void DateTimeMgr_GetTimestamp(Timestamp_t* timestamp)
{
    RTC_TimeTypeDef xTime;
	RTC_DateTypeDef xDate;

    if(NULL == timestamp)
    {
         return;
    }

	HAL_RTC_GetTime(&hrtc, &xTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &xDate, RTC_FORMAT_BIN);

	timestamp->seconds = xTime.Seconds;
	timestamp->minutes = xTime.Minutes;
	timestamp->hours = xTime.Hours;
	timestamp->day = xDate.Date;
	timestamp->month = xDate.Month;
	timestamp->year = xDate.Year;
}

void DateTimeMgr_SetDateTime(Timestamp_t* timestamp)
{
    RTC_TimeTypeDef xTime;
	RTC_DateTypeDef xDate;
    Timestamp_t current_time;

    if(NULL == timestamp) return;

    DateTimeMgr_GetTimestamp(&current_time);

    xTime.Seconds = timestamp->seconds;
	xTime.Minutes = timestamp->minutes;
	xTime.Hours = timestamp->hours;
	xDate.Date = timestamp->day;
	xDate.Month = timestamp->month;
	xDate.Year = timestamp->year;

    HAL_RTC_SetDate(&hrtc, &xDate, RTC_FORMAT_BIN);
    HAL_RTC_SetTime(&hrtc, &xTime, RTC_FORMAT_BIN);
    DataLog_LogEvent(EV_RTC_UPDATE, (uint8_t*)&current_time, sizeof(Timestamp_t));
}

void DateTimeMgr_SetTime(Timestamp_t* timestamp)
{
    RTC_TimeTypeDef xTime;
    Timestamp_t current_time;
    
    if(timestamp == NULL) return;
    
    DateTimeMgr_GetTimestamp(&current_time);

    xTime.Seconds = timestamp->seconds;
	xTime.Minutes = timestamp->minutes;
	xTime.Hours = timestamp->hours;

    HAL_RTC_SetTime(&hrtc, &xTime, RTC_FORMAT_BIN);
    DataLog_LogEvent(EV_RTC_UPDATE, (uint8_t*)&current_time, sizeof(Timestamp_t));
}

void DateTimeMgr_SetDate(Timestamp_t* timestamp)
{
	RTC_DateTypeDef xDate;
    Timestamp_t current_time;

    if(NULL == timestamp) return;

    DateTimeMgr_GetTimestamp(&current_time);

    xDate.Date = timestamp->day;
	xDate.Month = timestamp->month;
	xDate.Year = timestamp->year;

    HAL_RTC_SetDate(&hrtc, &xDate, RTC_FORMAT_BIN);
    DataLog_LogEvent(EV_RTC_UPDATE, (uint8_t*)&current_time, sizeof(Timestamp_t));
}

/******************************************************************************/
/*  Static function declarations                                              */
/******************************************************************************/
