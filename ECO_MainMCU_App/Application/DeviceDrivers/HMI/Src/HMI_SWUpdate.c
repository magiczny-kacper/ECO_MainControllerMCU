#include <stdint.h>

#include "HMI_SWUpdate.h"
#include "usart.h"
#include "printf.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define TEMP_BUFFER_SIZE 1024
#define UPDATE_RESPONSE_TIMEOUT 1000

static SemaphoreHandle_t update_mutex = NULL;
static StaticSemaphore_t update_mutex_buffer;
static volatile uint8_t update_response;
static volatile uint8_t temp_buffer[TEMP_BUFFER_SIZE];
static uint16_t received_size = 0;
static TaskHandle_t task_to_notify;

static const uint8_t connect1[] = 
{
    0x44, 
    0x52, 
    0x41, 
    0x4B, 
    0x4A, 
    0x48, 
    0x53, 
    0x55, 
    0x59, 
    0x44, 
    0x47, 
    0x42,
    0x4E, 
    0x43, 
    0x4A, 
    0x48, 
    0x47, 
    0x4A, 
    0x4B, 
    0x53, 
    0x48, 
    0x42, 
    0x44, 
    0x4E, 
    0xFF, 
    0xFF, 
    0xFF
};

static const uint8_t connect2[] = 
{
    0x63,
    0x6F,
    0x6E,
    0x6E,
    0x65,
    0x63,
    0x74,
    0xFF,
    0xFF,
    0xFF
};

static void notify_task(void);

void HMI_SWUpdate_Init(void)
{
    update_mutex = xSemaphoreCreateMutexStatic(&update_mutex_buffer);
}

bool HMI_SWUpdate_Start(uint8_t display, uint32_t file_size)
{
    bool ret = false;
    uint16_t update_header_size;
    xSemaphoreTake(update_mutex, portMAX_DELAY);
    task_to_notify = xTaskGetCurrentTaskHandle();

    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, temp_buffer, 4);
    HAL_UART_Transmit(&huart1, connect1, sizeof(connect1), 100);
    if (pdPASS == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, UPDATE_RESPONSE_TIMEOUT))
    {
        vTaskDelay(5);
        HAL_UART_Transmit_DMA(&huart1, connect2, sizeof(connect2));
        if (pdPASS == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, portMAX_DELAY))
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&huart1, temp_buffer, TEMP_BUFFER_SIZE);
            if (pdPASS == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, UPDATE_RESPONSE_TIMEOUT))
            {
                ret = true;
            }
        }
    }

    if (ret == true)
    {
        update_header_size = snprintf(temp_buffer, TEMP_BUFFER_SIZE, 
            "whmi-wri %d,115200,r", file_size);

        for(uint8_t i = 0; i < 3; i++)
        {
            temp_buffer[update_header_size++] = 0xFF;
        }
        vTaskDelay(5);
        HAL_UART_Transmit_DMA(&huart1, temp_buffer, update_header_size);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, &update_response, 1);
        if (pdPASS == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, portMAX_DELAY))
        {
            if (pdPASS == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, UPDATE_RESPONSE_TIMEOUT))
            {
                if ((received_size == 1) && (update_response == 0x05))
                {
                    ret = true;
                }
            }
        }
    }
    return ret;
}

bool HMI_SWUpdate_UpdateBlock(uint32_t offset, uint32_t size, uint8_t* data)
{
    (void)offset;
    bool ret = false;
    task_to_notify = xTaskGetCurrentTaskHandle();

    HAL_UART_Transmit_DMA(&huart1, data, size);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, &update_response, 1);
    xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, portMAX_DELAY);
    if (pdPASS == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, UPDATE_RESPONSE_TIMEOUT))
    {
        if ((received_size == 1) && (update_response == 0x05))
        {
            ret = true;
        }
    }
    return ret;
}

void HMI_SWUpdate_Finalize(void)
{
    xSemaphoreGive(update_mutex);
}

void HMI_SWUpdate_DataSentCallback(void)
{
    notify_task();
}

void HMI_SWUpdate_ReponseReceivedCallback(uint16_t size)
{
    received_size = size;
    notify_task();
}

static void notify_task(void)
{
    BaseType_t xHigherPriorityTaskWoken;
    vTaskNotifyGiveFromISR(task_to_notify, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}