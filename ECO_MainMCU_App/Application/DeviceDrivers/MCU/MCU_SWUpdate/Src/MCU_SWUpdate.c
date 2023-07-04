#include "MCU_SWUpdate.h"

#include "bootutil/bootutil.h"
#include "sysflash/sysflash.h"
#include "flash_map_backend/flash_map_backend.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

static const struct flash_area *fa;
static uint8_t page_buffer[256];
static TimerHandle_t reset_timer;
static StaticTimer_t reset_timer_buffer;

static void MCU_SWUpdate_Reset_Callback(TimerHandle_t timer);

void MCU_SWUpdate_Init(void)
{
    reset_timer = xTimerCreateStatic(
        "ResetTimer",
        1000,
        pdFALSE,
        (void*)0,
        MCU_SWUpdate_Reset_Callback,
        &reset_timer_buffer
    );
}

void MCU_SWUpdate_SaveChunk(uint32_t offset, uint32_t size, uint8_t* data)
{   
    uint32_t bytes_left = size;
    uint32_t copy_size;

    flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0), &fa);

    if (size % 4096)
    {
        size -= size % 4096;
        size += 4096;
    }

    if (0 == flash_area_erase(fa, offset, size))
    {   
        while(size)
        {
            if (bytes_left < 256)
            {
                copy_size = bytes_left;
            }
            else
            {
                copy_size = 256;
            }

            memset(page_buffer, 0x00, 256);
            memcpy(page_buffer, data, copy_size);
            flash_area_write(fa, offset, data, 256);
            data += 256;
            offset += 256;
            size -= copy_size;
        }
    }
}

void MCU_SWUpdate_Finalize(void)
{
    boot_set_pending(0);
    xTimerStart(reset_timer, 10);
}

static void MCU_SWUpdate_Reset_Callback(TimerHandle_t timer)
{
    HAL_NVIC_SystemReset();
}