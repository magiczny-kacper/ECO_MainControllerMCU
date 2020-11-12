/*
 * UART_DMA.c
 *
 *  Created on: 27.10.2019
 *      Author: Mateusz Salamon
 *		www.msalamon.pl
 *
 *      Website: https://msalamon.pl/odbieranie-uart-po-dma-to-bulka-z-maslem-lekcja-z-kursu-stm32/
 *      GitHub:  https://github.com/lamik/UART_DMA_receiving
 *      Contact: mateusz@msalamon.pl
 */

#include "UART_DMA.h"
#include <string.h>

uint8_t UARTDMA_UartIrqHandler(UARTDMA_HandleTypeDef *huartdma)
{
	if(huartdma->huart->Instance->SR & UART_FLAG_IDLE)       // Check if Idle flag is set
	{
		volatile uint32_t tmp;
		tmp = huartdma->huart->Instance->SR;                      // Read status register
		tmp = huartdma->huart->Instance->DR;                      // Read data register
		huartdma->huart->hdmarx->Instance->CR &= ~DMA_SxCR_EN; // Disable DMA - it will force Transfer Complete interrupt if it's enabled
		tmp = tmp; // For unused warning
		return 1;
	}
	else
	{
		return 0;
	}
}

void UARTDMA_DmaIrqHandler(UARTDMA_HandleTypeDef *huartdma){
	__disable_irq();
	uint32_t Length, i;

	typedef struct
	{
		__IO uint32_t ISR;   // DMA interrupt status register
		__IO uint32_t Reserved0;
		__IO uint32_t IFCR;  // DMA interrupt flag clear register
	} DMA_Base_Registers;

	DMA_Base_Registers *DmaRegisters = (DMA_Base_Registers *) huartdma->huart->hdmarx->StreamBaseAddress; // Take registers base address

	if (__HAL_DMA_GET_IT_SOURCE(huartdma->huart->hdmarx, DMA_IT_TC) != RESET) // Check if interrupt source is Transfer Complete
	{
		DmaRegisters->IFCR = DMA_FLAG_TCIF0_4 << huartdma->huart->hdmarx->StreamIndex;	// Clear Transfer Complete flag

		Length = DMA_RX_BUFFER_SIZE - huartdma->huart->hdmarx->Instance->NDTR; // Get the Length of transfered data

		huartdma->length += Length;
		//huartdma->length = Length;

		for(i = 0; i < Length; i++){
			huartdma->UART_Buffer[huartdma->UartBufferTail] = huartdma->DMA_RX_Buffer[i];
			huartdma->UartBufferTail++;
			huartdma->UartBufferTail &= 127;
		}

		if(huartdma->callback != NULL){
			huartdma -> callback(huartdma -> callbackArgs);
		}

		DmaRegisters->IFCR = 0x3FU << huartdma->huart->hdmarx->StreamIndex; 		// Clear all interrupts
		huartdma->huart->hdmarx->Instance->M0AR = (uint32_t) huartdma->DMA_RX_Buffer; // Set memory address for DMA again
		huartdma->huart->hdmarx->Instance->NDTR = DMA_RX_BUFFER_SIZE; // Set number of bytes to receive
		huartdma->huart->hdmarx->Instance->CR |= DMA_SxCR_EN;            	// Start DMA transfer
	}
	__enable_irq();
}

void UARTDMA_Init(UARTDMA_HandleTypeDef *huartdma, UART_HandleTypeDef *huart)
{
	huartdma->huart = huart;
	huartdma->callback = NULL;
	huartdma->callbackArgs = NULL;
	__HAL_UART_ENABLE_IT(huartdma->huart, UART_IT_IDLE);   	// UART Idle Line interrupt
	__HAL_DMA_ENABLE_IT(huartdma->huart->hdmarx, DMA_IT_TC); // UART DMA Transfer Complete interrupt

	HAL_UART_Receive_DMA(huartdma->huart, &huartdma->DMA_RX_Buffer[0], DMA_RX_BUFFER_SIZE); // Run DMA for whole DMA buffer

	huartdma->huart->hdmarx->Instance->CR &= ~DMA_SxCR_HTIE; // Disable DMA Half Complete interrupt
}

void UARTDMA_Start(UARTDMA_HandleTypeDef *huartdma){
	HAL_UART_Receive_DMA(huartdma->huart, huartdma->DMA_RX_Buffer, DMA_RX_BUFFER_SIZE); // Run DMA for whole DMA buffer
	huartdma->huart->hdmarx->Instance->CR &= ~DMA_SxCR_HTIE; // Disable DMA Half Complete interrupt
}

uint32_t UARTDMA_GetLen (UARTDMA_HandleTypeDef* huartdma){
	return huartdma -> length;
}

uint32_t UARTDMA_GetData (UARTDMA_HandleTypeDef *huartdma, uint8_t* outBuf){
	uint32_t retval = 0;
	uint32_t temp1, temp2;
	uint32_t tempTail, tempHead, tempLen;

	tempTail = huartdma -> UartBufferTail;
	tempHead = huartdma -> UartBufferHead;
	tempLen = huartdma -> length;

	if(tempTail != tempHead){
		if(tempHead + tempLen > 128){
			temp1 = 128 - tempTail;
			temp2 = tempLen - temp1;
			memcpy(outBuf, &huartdma -> UART_Buffer[tempHead], temp1);
			memcpy(outBuf + temp1, &huartdma -> UART_Buffer[0], temp2);
		}else{
			memcpy(outBuf, &huartdma -> UART_Buffer[tempHead], tempLen);

		}
		tempHead = tempTail;
		huartdma -> UartBufferHead = 0;
		huartdma -> UartBufferTail = 0;
		huartdma -> length = 0;
		retval = tempLen;
	}
	return retval;
}
