#ifndef MODBUS_H
#define MODBUS_H
#endif

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"
#include "main.h"

#define MODBUS_CRC_POLY 0x8005

typedef struct{
	UART_HandleTypeDef *ModbusSerialPort;
	TickType_t timeout_t;
	TaskHandle_t task;
}ModbusHandler;

typedef enum{
	Modbus_OK = 0,
	Modbus_TIMEOUT,
	Modbus_CRCERR
}ModbusState_t;

void vModbusInit (ModbusHandler *modbus, UART_HandleTypeDef *port, uint32_t timeout);
uint16_t uModbusCalculateCRC (uint8_t frame_length, uint8_t *frame);
uint8_t bModbusCheckCRC (uint8_t frame_length, uint8_t *frame);
ModbusState_t vModbusReadHoldingRegisters (ModbusHandler *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame);
ModbusState_t vModbusReadInputRegisters (ModbusHandler *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame);
void vModbusReceieveResponseCallback (ModbusHandler *modbus);
