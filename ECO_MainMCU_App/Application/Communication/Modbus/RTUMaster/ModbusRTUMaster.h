#ifndef MODBUSRTUMASTER_H_
#define MODBUSRTUMASTER_H_

/**
 * @defgroup ModbusRTUMaster
 * @{
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "main.h"

#include "usart.h"

#define MODBUS_CRC_POLY 0x8005

/**
 * @brief Return values
 *
 */
typedef enum{
	Modbus_TIMEOUT,/**< Modbus_TIMEOUT */
	Modbus_CRC_ERR,  /**< Modbus_CRCERR */
	Modbus_OK = 0, /**< Modbus_OK */
	Modbus_ILLEGAL_FUNC = 1,
	Modbus_ILLEGAL_DATA_ADDR,
	Modbus_ILLEGAL_DATA_VAL,
	Modbus_SLAVE_FAILURE,
	Modbus_ACK,
	Modbus_SLAVE_BUSY,
	Modbus_NACK,
	Modbus_PARITY_ERR
}ModbusState_t;

/**
 * @brief Modbus master initialization
 *
 * @param modbus Pointer for modbus configuration structure
 * @param port Pointer for HAL Uart structure
 * @param timeout Request timeout
 */
void ModbusRTUMaster_Init(UART_HandleTypeDef *port, uint32_t timeout);

/**
 * @brief Calculates CRC of given frame
 *
 * @param frame_length Length of frame
 * @param frame Pointer to frame to calculate CRC.
 * @return Calculated CRC
 */
uint16_t ModbusRTU_CalculateCRC(uint8_t frame_length, uint8_t *frame);

/**
 * @brief Checks if CRC form given frame is correct.
 *
 * @param frame_length Length of frame (with CRC).
 * @param frame Pointer to frame to check.
 * @return Returns 0 if CRC is incorrect, else, 1.
 */
uint8_t ModbusRTU_CheckCRC(uint8_t frame_length, uint8_t *frame);

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
ModbusState_t ModbusRTUMaster_ReadHoldingRegisters(uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, void *response);

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
ModbusState_t ModbusRTUMaster_ReadInputRegisters(uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, void *response);

/**
 * @brief
 *
 * @param modbus
 */
void ModbusRTUMaster_ReceieveResponseCallback(uint16_t rx_size);

/**
 * @brief
 *
 * @param modbus
 * @param inData
 * @param len
 * @param outData
 * @return
 */
int16_t ModbusRTUMaster_TCPForward(uint8_t* inData, uint32_t len, uint8_t* outData);

#endif /* MODBUSRTUMASTER_H_ */

/**
 * @}
 */
