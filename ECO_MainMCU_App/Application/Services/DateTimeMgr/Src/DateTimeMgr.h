#ifndef DATETIMEMGR_H_
#define DATETIMEMGR_H_

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <string.h>
#include <stdint.h>

/******************************************************************************/
/*  Global defines                                                            */
/******************************************************************************/

/******************************************************************************/
/*  Global types                                                              */
/******************************************************************************/

/**
 * @brief Global timestamp format
 * 
 */
typedef union
{
	struct
    {
		uint8_t seconds; /**< Seconds */
		uint8_t minutes; /**< Minutes */
		uint8_t hours; /**< Hours */
		uint8_t day; /**< Day */
		uint8_t month; /**< Month */
		uint8_t year; /**< Year */
	};
	uint64_t timestamp;
    uint8_t bytes[8];
} Timestamp_t;

/******************************************************************************/
/*  Global function prototypes                                                */
/******************************************************************************/
void DateTimeMgr_Init(void);

void DateTimeMgr_GetTimestamp(Timestamp_t* timestamp);

void DateTimeMgr_SetDateTime(Timestamp_t* timestamp);

void DateTimeMgr_SetTime(Timestamp_t* timestamp);

void DateTimeMgr_SetDate(Timestamp_t* timestamp);

#endif /* DATETIMEMGR_H_ */
