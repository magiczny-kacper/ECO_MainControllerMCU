#include "ResourceGuard.h"

static StaticSemaphore_t xSpiMutexBuffer;
static SemaphoreHandle_t xSpiMutex;
static StaticSemaphore_t xI2CMutexBuffer;
static SemaphoreHandle_t xI2CMutex;

void RG_Init(void)
{
    xSpiMutex = xSemaphoreCreateMutexStatic(&xSpiMutexBuffer);
	xI2CMutex = xSemaphoreCreateMutexStatic(&xI2CMutexBuffer);
}

void RG_SPI_ChipSelect(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	xSemaphoreTake(xSpiMutex, portMAX_DELAY);
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

void RG_SPI_ChipDeselect(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	xSemaphoreGive(xSpiMutex);
}

void RG_I2C_Acquire(void)
{
	xSemaphoreTake(xI2CMutex, portMAX_DELAY);
}

void RG_I2C_Release(void)
{
	xSemaphoreGive(xI2CMutex);
}