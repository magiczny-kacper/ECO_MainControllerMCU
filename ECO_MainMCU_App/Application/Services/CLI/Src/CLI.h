#ifndef CLI_H_
#define CLI_H_

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"

/******************************************************************************/
/*  Global defines                                                            */
/******************************************************************************/

/******************************************************************************/
/*  Global types                                                              */
/******************************************************************************/

/******************************************************************************/
/*  Global function prototypes                                                */
/******************************************************************************/

/**
 * @brief Command Line Input initialization
 *
 */
void CLI_Init (void);

/**
 * @brief EERPOM Erease command callback
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_EreaseEE( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Show current config saved in eeprom memory
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_ShowConfig( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Prints current system status
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_GetSystemStatus( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Prints RTOS tasks stats
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_GetRtosStats( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Prints communication statistics
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_GetCommStats( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Saves test event to memory
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_WriteTestEvent( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Erases external flash memory
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_EreaseFlash( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Resets MCU
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_Reset( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Show I/O status
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_ShowIO( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Prints last event from memory
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_GetLastEvent( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Prints n last events
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_GetEvents( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Prints last n events in byte format
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_GetEventsRaw( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Sets RTC time
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_SetTime( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @brief Sets RTC date
 *
 * @param pcWriteBuffer Buffer to save response
 * @param xWriteBufferLen Max length of response
 * @param pcCommandString Received command string
 * @return pdFALSE if execution ended, pdTRUE otherwise
 */
BaseType_t xCLI_SetDate( char *pcWriteBuffer, size_t xWriteBufferLen, char* pcCommandString );

/**
 * @}
 */
#endif /* CLI_H_ */
