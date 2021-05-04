#ifndef MODBUS_H
#define MODBUS_H

/**
 * @defgroup ModbusRTUMaster
 * @{
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include "main.h"

#include "UART_DMA.h"

#define MODBUS_CRC_POLY 0x8005

/**
 * @brief Handler structure
 *
 */
typedef struct{
	UART_HandleTypeDef *ModbusSerialPort; /**< UART for communication */
	UARTDMA_HandleTypeDef dma; /**< DMA for receiving data */
	TickType_t timeout_t; /**< Timeout given in ticks */
	TaskHandle_t task; /**< Current task to notify */
	SemaphoreHandle_t mutex; /**< Usage mutes */
}ModbusRTUMaster_t;

/**
 * @brief Return values
 *
 */
typedef enum{
	Modbus_OK = 0, /**< Modbus_OK */
	Modbus_TIMEOUT,/**< Modbus_TIMEOUT */
	Modbus_CRCERR  /**< Modbus_CRCERR */
}ModbusState_t;

/**
 * @brief Modbus master initialization
 *
 * @param modbus Pointer for modbus configuration structure
 * @param port Pointer for HAL Uart structure
 * @param timeout Request timeout
 */
void ModbusRTUMaster_Init (ModbusRTUMaster_t *modbus, UART_HandleTypeDef *port, SemaphoreHandle_t mutex, uint32_t timeout);

/**
 * @brief Calculates CRC of given frame
 *
 * @param frame_length Length of frame
 * @param frame Pointer to frame to calculate CRC.
 * @return Calculated CRC
 */
uint16_t ModbusRTU_CalculateCRC (uint8_t frame_length, uint8_t *frame);

/**
 * @brief Checks if CRC form given frame is correct.
 *
 * @param frame_length Length of frame (with CRC).
 * @param frame Pointer to frame to check.
 * @return Returns 0 if CRC is incorrect, else, 1.
 */
uint8_t ModbusRTU_CheckCRC (uint8_t frame_length, uint8_t *frame);

/**
 * @brief Modbus Read Coil Status (0x01)
 *
 * @param modbus Pointer for modbus structure
 * @param slave_address Addres of slave to send request
 * @param first_coil First coil address
 * @param number_of_coils Number of coils to read
 * @param response_frame Pointer to write response frame
 * @return Communication result
 */
ModbusState_t ModbusRTUMaster_ReadHoldingRegisters (ModbusRTUMaster_t *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame);

/**
 * @brief
 *
 * @param modbus
 * @param slave_address
 * @param first_register_address
 * @param registers_count
 * @param response_frame
 * @param out
 * @return
 */
ModbusState_t ModbusRTUMaster_ReadInputRegisters (ModbusRTUMaster_t *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame, uint8_t* out);

/**
 * @brief
 *
 * @param modbus
 */
void ModbusRTUMaster_ReceieveResponseCallback (ModbusRTUMaster_t *modbus);

/**
 * @brief
 *
 * @param modbus
 * @param inData
 * @param len
 * @param outData
 * @return
 */
int16_t ModbusRTUMaster_TCPForward (ModbusRTUMaster_t* modbus, uint8_t* inData, uint32_t len, uint8_t* outData);

#endif

/**
 * @}
 */
