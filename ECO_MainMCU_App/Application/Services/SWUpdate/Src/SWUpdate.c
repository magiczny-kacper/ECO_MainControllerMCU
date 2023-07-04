#include <stdbool.h>
#include "SWUpdate.h"
#include "crc.h"

#include "SWUpdate_Types.h"
#include "MCU_SWUpdate.h"
#include "HMI_SWUpdate.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

extern CRC_HandleTypeDef hcrc;

#define KERMIT_CRC_POLY 0x1021

#define SWUPDATE_INFO_FRAME 0xA0
#define SWUPDATE_CHUNK_FRAME 0xB5

#define SWUPDATE_FRAME_CORRUPTED 0x00
#define SWUPDATE_RESPONSE_OK 0xAA
#define SWUPDATE_RESPONSE_NOT_OK 0x55
#define SWUPDATE_RESPONSE_END 0xAC

#define SWUPDATE_BLOCK_APP 0x00
#define SWUPDATE_BLOCK_NEXTION_SMALL 0x01
#define SWUPDATE_BLOCK_NEXTION_BIG 0x02

/**
 * @brief Callback function to start SWUpdate process for given block
 * 
 * @param block Block ID to be updated
 * @param size Size of fiel to be uploaded
 * @return Returns true when successfull
 */
typedef bool (*SWUpdate_Start_Ptr_t)(SWUpdate_Block_t block, uint32_t size);

/**
 * @brief Callback function to save 4kB chunk of data
 * 
 * @param offset Chunke offset in memory
 * @param size Size of chunk
 * @param data Data pointer
 * @return None
 */
typedef void (*SWUpdate_SaveChunk_Ptr_t)(uint32_t offset, uint32_t size, uint8_t* data);

/**
 * @brief Callback function to finalize SWUpdate
 * 
 * @return None
 */
typedef void (*SWUpdate_Finalize_Ptr_t)(void);

static const SWUpdate_Start_Ptr_t start_funcs[SWUPDATE_BLOCK_COUNT] = 
{
    NULL,
    HMI_SWUpdate_Start,
    HMI_SWUpdate_Start
};

static const SWUpdate_SaveChunk_Ptr_t save_funcs[SWUPDATE_BLOCK_COUNT] =
{
    MCU_SWUpdate_SaveChunk,
    HMI_SWUpdate_UpdateBlock,
    HMI_SWUpdate_UpdateBlock
};

static const SWUpdate_Finalize_Ptr_t finalize_funcs[SWUPDATE_BLOCK_COUNT] =
{
    MCU_SWUpdate_Finalize,
    HMI_SWUpdate_Finalize,
    HMI_SWUpdate_Finalize
};

static bool update_ongoing = false;
static uint8_t updated_block = 0;
static uint32_t file_size = 0;
static uint32_t bytes_uploaded = 0;
static uint32_t uploaded_chunks = 0;
static uint16_t saved_chunk_bytes = 0;
static uint8_t chunk_buffer[4096];

static TimerHandle_t update_timeout_timer;
static StaticTimer_t update_timeout_buffer;

static uint16_t SWUpdate_SaveChunk( 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer);
        
static uint16_t SWUpdate_InitUpdate( 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer);

static void SWUpdate_Timeout_Callback(TimerHandle_t timer);

static bool SWUpdate_CheckFrame(uint16_t crc, uint8_t* frame, uint16_t size);

void SWUpdate_Init(void)
{
    HMI_SWUpdate_Init();
    update_timeout_timer = xTimerCreateStatic(
        "UpdateTimeout",
        1000 * 5,
        pdFALSE,
        (void*)0,
        SWUpdate_Timeout_Callback,
        &update_timeout_buffer
    );
}

uint16_t SWUPdate_Network_Callback(uint8_t interrupt, 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer)
{
    uint16_t return_len = 0;
    uint16_t frame_data_len = 0;
    uint16_t receive_index = 0;
    uint16_t frame_crc;
    uint8_t frame_id;

    frame_id = receive_buffer[receive_index];
    receive_index++;
    memcpy(&frame_data_len, &receive_buffer[receive_index], 2);
    receive_index += 2;
    memcpy(&frame_crc, &receive_buffer[receive_index], 2);
    receive_index += 2;

    if (SWUpdate_CheckFrame(frame_crc, &receive_buffer[receive_index], frame_data_len))
    {
        switch (frame_id)
        {
            case SWUPDATE_INFO_FRAME:
                return_len = SWUpdate_InitUpdate(frame_data_len, 
                    &receive_buffer[receive_index], max_transmit_size, transmit_buffer);
                break;

            case SWUPDATE_CHUNK_FRAME:
                return_len = SWUpdate_SaveChunk(frame_data_len, 
                    &receive_buffer[receive_index], max_transmit_size, transmit_buffer);
                break;
            
            default:
                break;
        }
    }
    else
    {
        transmit_buffer[0] = SWUPDATE_FRAME_CORRUPTED;
        return_len = 1;
    }

    return return_len;
}

static uint16_t SWUpdate_InitUpdate( 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer)
{
    uint16_t return_len;
    uint16_t receive_index = 0;
    bool status = true;

    if (false == update_ongoing)
    {
        bytes_uploaded = 0;
        updated_block = receive_buffer[receive_index];
        receive_index++;
        memcpy(&file_size, &receive_buffer[receive_index], 4);
        receive_index += 4;

        
        if (start_funcs[updated_block] != NULL)
        {
            status = start_funcs[updated_block](updated_block, file_size);
        }

        if (true == status)
        {
            transmit_buffer[0] = SWUPDATE_RESPONSE_OK;
            update_ongoing = true;
        }
        else
        {
            transmit_buffer[0] = SWUPDATE_RESPONSE_NOT_OK;
            update_ongoing = false;
        }

        return_len = 1;
        xTimerStart(update_timeout_timer, 10);
    }

    return return_len;
}

static uint16_t SWUpdate_SaveChunk( 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer)
{
    uint16_t return_len = 0;
    uint16_t receive_index = 0;

    if (true == update_ongoing)
    {
        xTimerReset(update_timeout_timer, 10);

        memcpy(&chunk_buffer[saved_chunk_bytes], receive_buffer, receive_size);
        saved_chunk_bytes += receive_size;
        if ((4096 == saved_chunk_bytes) || (0 != (saved_chunk_bytes % 1024)))
        {
            // vTaskDelay(250);
            uploaded_chunks++;
            bytes_uploaded += saved_chunk_bytes;
            /* Save chunk to flash memory here */
            save_funcs[updated_block](bytes_uploaded, saved_chunk_bytes, chunk_buffer);
            saved_chunk_bytes = 0;
            if (bytes_uploaded >= file_size)
            {
                finalize_funcs[updated_block]();
                update_ongoing = false;
            }
        }

        if (true == update_ongoing)
        {
            transmit_buffer[0] = SWUPDATE_RESPONSE_OK;
        }
        else
        {
            transmit_buffer[0] = SWUPDATE_RESPONSE_END;
        }
        return_len = 1;
    }
    return return_len;
}

static bool SWUpdate_CheckFrame(uint16_t crc, uint8_t* frame, uint16_t size)
{
    uint32_t actual_crc;
    hcrc.Instance->INIT = 0;
    hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
    hcrc.Init.GeneratingPolynomial = CRC_POLYLENGTH_16B;
    hcrc.Init.CRCLength = CRC_POLYLENGTH_16B;
    HAL_CRCEx_Polynomial_Set(&hcrc, KERMIT_CRC_POLY, CRC_POLYLENGTH_16B);

    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
    actual_crc = HAL_CRC_Accumulate(&hcrc, frame, size);
    return (crc == (uint16_t)actual_crc);
}

static void SWUpdate_Timeout_Callback(TimerHandle_t timer)
{
    update_ongoing = false;
}