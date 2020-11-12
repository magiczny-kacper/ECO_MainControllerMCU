/*
 * UART_DMA.h
 *
 *  Created on: 27.10.2019
 *      Author: Mateusz Salamon
 *		www.msalamon.pl
 *
 *      Website: https://msalamon.pl/odbieranie-uart-po-dma-to-bulka-z-maslem-lekcja-z-kursu-stm32/
 *      GitHub:  https://github.com/lamik/UART_DMA_receiving
 *      Contact: mateusz@msalamon.pl
*/
#ifndef UARTDMA_H
#define UARTDMA_H

#include "stm32f4xx_hal.h"

#define DMA_RX_BUFFER_SIZE          128
#define UART_BUFFER_SIZE           	128

typedef struct __attribute__ ((packed))
{
	UART_HandleTypeDef* huart;					// UART handler

	uint8_t DMA_RX_Buffer[DMA_RX_BUFFER_SIZE];	// DMA direct buffer
	uint8_t UART_Buffer[UART_BUFFER_SIZE];		// UART working circular buffer
	uint8_t length;
	uint16_t UartBufferHead;
	uint16_t UartBufferTail;
	uint8_t  UartBufferLines;
	void (*callback)();
	void* callbackArgs;
}UARTDMA_HandleTypeDef;

uint8_t UARTDMA_UartIrqHandler(UARTDMA_HandleTypeDef *huartdma);
void UARTDMA_DmaIrqHandler(UARTDMA_HandleTypeDef *huartdma);

void UARTDMA_Init(UARTDMA_HandleTypeDef *huartdma, UART_HandleTypeDef *huart);
void UARTDMA_Start(UARTDMA_HandleTypeDef *huartdma);

uint32_t UARTDMA_GetLen (UARTDMA_HandleTypeDef* huartdma);
uint32_t UARTDMA_GetData (UARTDMA_HandleTypeDef *huartdma, uint8_t* outBuf);

#endif
