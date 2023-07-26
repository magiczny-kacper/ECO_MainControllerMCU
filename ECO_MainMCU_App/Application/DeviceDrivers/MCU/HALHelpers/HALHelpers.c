#include "usart.h"
#include "ModbusRTUMaster.h"
#include "ResourceGuard.h"
#include "application.h"
#include "Ethernet.h"
#include "HMI_SWUpdate.h"

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(USART1 == huart->Instance) // Nextion big port
    {
        HMI_SWUpdate_ReponseReceivedCallback(Size);
    }
    else if(USART2 == huart->Instance) // Debug port
    {
        
    }
    else if(USART3 == huart->Instance) // Modbus RS485 port
    {
        ModbusRTUMaster_ReceieveResponseCallback(Size);
    }
    else if(USART4 == huart->Instance) // Nextion small port
    {
        
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(USART1 == huart->Instance) // Nextion big port
    {

    }
    else if(USART2 == huart->Instance) // Debug port
    {
    }
    else if(USART3 == huart->Instance) // Modbus RS485 port
    {

    }
    else if(USART4 == huart->Instance) // Nextion small port
    {
        HMI_SWUpdate_DataSentCallback();
    }
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    // if(GPIO_Pin & NRF_INT_Pin){
    // 	vTaskNotifyGiveFromISR(nRF24L01Handle, NULL);
    // }

    if(GPIO_Pin & ETH_INT_Pin){
        EthernetTask_IRQ_Callback();
    }

    if(GPIO_Pin & PG_24V_Pin){
        // DeviceMonitor_PGChangeCallback();
    	// uint32_t value;
    	// value = HAL_RTCEx_BKUPRead(&hrtc, 0);
    	// value ++;
    	// HAL_RTCEx_BKUPWrite(&hrtc, 0, value);
    }
}

void SysTick_Handler(void)
{
    Application_SysTick_Callback();
}

void W25Q64_Set_ChipSelect_Low(void)
{
	RG_SPI_ChipSelect(FLASH_CS_GPIO_Port, FLASH_CS_Pin);
}

void W25Q64_Set_ChipSelect_High(void)
{
	RG_SPI_ChipDeselect(FLASH_CS_GPIO_Port, FLASH_CS_Pin);
}
