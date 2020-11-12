#ifndef MODBUS_H
#define MODBUS_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"
#include "main.h"

#include "UART_DMA.h"

#define MODBUS_CRC_POLY 0x8005

typedef struct{
	UART_HandleTypeDef *ModbusSerialPort;
	UARTDMA_HandleTypeDef dma;
	TickType_t timeout_t;
	TaskHandle_t task;
}ModbusRTUMaster_t;

typedef enum{
	Modbus_OK = 0,
	Modbus_TIMEOUT,
	Modbus_CRCERR
}ModbusState_t;

void ModbusRTUMaster_Init (ModbusRTUMaster_t *modbus, UART_HandleTypeDef *port, uint32_t timeout);
uint16_t ModbusRTU_CalculateCRC (uint8_t frame_length, uint8_t *frame);
uint8_t ModbusRTU_CheckCRC (uint8_t frame_length, uint8_t *frame);
ModbusState_t ModbusRTUMaster_ReadHoldingRegisters (ModbusRTUMaster_t *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame);
ModbusState_t ModbusRTUMaster_ReadInputRegisters (ModbusRTUMaster_t *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame, uint8_t* out);
void ModbusRTUMaster_ReceieveResponseCallback (ModbusRTUMaster_t *modbus);
int16_t ModbusRTUMaster_TCPForward (ModbusRTUMaster_t* modbus, uint8_t* inData, uint32_t len, uint8_t* outData);

#endif
