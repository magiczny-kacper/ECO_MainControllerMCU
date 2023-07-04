#include "logging.h"
#include "ring.h"
#include "main.h"
#include "stm32g0xx_hal.h"
#include "printf.h"

#include <stdbool.h>

#define TX_BUFFER_SIZE 4096

static RingBuffer_t tx_ring;
static uint8_t tx_buffer[TX_BUFFER_SIZE];
static volatile bool is_sending = false;

extern UART_HandleTypeDef huart2;

void Logging_Init(void)
{
	RingInit(&tx_ring, tx_buffer, TX_BUFFER_SIZE, 1);
	printf("\r\n\r\n");
}

void Logging_Wait(void)
{
	while(is_sending == true);
}

void Logging_Callback(void)
{
	RingStatus_t status;
	uint8_t data;
	status = RingReadElement(&tx_ring, &data);
	if(status == OK)
	{
		HAL_UART_Transmit_IT(&huart2, &data, 1);
	}
	else
	{
		is_sending = false;
	}
}

void _putchar(char character)
{
	RingWriteElement(&tx_ring, &character);
	if(is_sending == false)
	{
		uint8_t data;
		RingReadElement(&tx_ring, &data);
		if(HAL_OK == HAL_UART_Transmit_IT(&huart2, &data, 1))
		{
			is_sending = true;
		}
	}
}
