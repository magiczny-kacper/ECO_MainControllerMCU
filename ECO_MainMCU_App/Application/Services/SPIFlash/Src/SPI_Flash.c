/**
 * @addtogroup
 *
 */

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>
#include <string.h>

#include "SPI_Flash_Cfg.h"
#include "SPI_Flash.h"

#include "w25qxx.h"
/******************************************************************************/
/*  Local defines                                                             */
/******************************************************************************/

/******************************************************************************/
/*  Local types                                                               */
/******************************************************************************/

typedef struct {
    uint32_t base_address;
    uint32_t size;
    const char* name;
} SPI_FlashPartition_t;

/******************************************************************************/
/*  Local variables                                                           */
/******************************************************************************/
// static w25qxx_t external_flash;
static bool flash_initialized = false;
extern w25qxx_t	w25qxx;

#ifdef DEF_FLASH_PARTITION
#undef DEF_FLASH_PARTITION
#endif
#define DEF_FLASH_PARTITION(partition_name, partition_name_str, partition_address, partition_size) \
    {                                                                                              \
        .base_address = ALIGN_SIZE(partition_address, 4096),                                       \
        .size = ALIGN_SIZE(partition_size, 4096),                                                  \
        .name = partition_name_str                                                                 \
    },

const SPI_FlashPartition_t spi_flash_partitions[PARTITION_CNT] = 
{
    SPI_FLASH_PARTITIONS
};

/******************************************************************************/
/*  Static function prototypes                                                */
/******************************************************************************/

/******************************************************************************/
/*  Global function declarations                                              */
/******************************************************************************/
void SPI_Flash_Init(void)
{
    flash_initialized = W25qxx_Init();
}

uint32_t SPI_Flash_GetPartitionSize(SPI_Flash_PartitionId_t partition)
{
    uint32_t size = 0;
    if(partition < PARTITION_CNT)
    {
        size = spi_flash_partitions[partition].size;
    }
    return size;
}

bool SPI_Flash_Read(SPI_Flash_PartitionId_t partition, void* dest, uint32_t offset, uint32_t size)
{
    bool ret = false;
    uint32_t read_address;

    if((PARTITION_CNT > partition) && (NULL != dest) && (0 < size))
    {
        if((offset + size) < spi_flash_partitions[partition].size)
        {
            read_address = spi_flash_partitions[partition].base_address + offset;
            while(size)
            {   
                if((size >= 65536) && ((read_address % 65536) == 0)) // Block
                {
                    W25qxx_ReadBlock(
                        (uint8_t*)dest, 
                        read_address / 65536, 
                        0, 
                        65536
                    );
                    dest += 65536;
                    read_address += 65536;
                    size -= 65536;
                }
                else if((size >= 4096) && ((read_address % 4096) == 0)) // Sector
                {
                    W25qxx_ReadSector(
                        (uint8_t*)dest,
                        read_address / 4096,
                        0,
                        4096
                    );
                    dest += 4096;
                    read_address += 4096;
                    size -= 4096;
                }
                else if((size >= 256) && ((read_address % 256) == 0)) // Page
                {
                    W25qxx_ReadPage(
                        (uint8_t*)dest,
                        read_address / 256,
                        0,
                        256
                    );
                    dest += 256;
                    read_address += 256;
                    size -= 256;
                }
                else 
                {                
                    W25qxx_ReadByte(dest, read_address);
                    dest++;
                    read_address++;
                    size--;
                }
            }
            ret = true;
        }
    }

    return ret;
}

bool SPI_Flash_Write(SPI_Flash_PartitionId_t partition, void* data, uint32_t offset, uint32_t size)
{
    bool ret = false;
    uint32_t write_address;

    if((PARTITION_CNT > partition) && (NULL != data) && (0 < size))
    {
        if((offset + size) < spi_flash_partitions[partition].size)
        {
            write_address = spi_flash_partitions[partition].base_address + offset;
            while(size)
            {   
                if((write_address % 4096) == 0)
                {
                    W25qxx_EraseSector(write_address / 4096);
                }

                if((size >= 65536) && ((write_address % 65536) == 0)) // Block
                {
                    W25qxx_WriteBlock(
                        (uint8_t*)data, 
                        write_address / 65536, 
                        0, 
                        65536
                    );
                    data += 65536;
                    write_address += 65536;
                    size -= 65536;
                }
                else if((size >= 4096) && ((write_address % 4096) == 0)) // Sector
                {
                    W25qxx_WriteSector(
                        (uint8_t*)data,
                        write_address / 4096,
                        0,
                        4096
                    );
                    data += 4096;
                    write_address += 4096;
                    size -= 4096;
                }
                else if((size >= 256) && ((write_address % 256) == 0)) // Page
                {
                    W25qxx_WritePage(
                        (uint8_t*)data,
                        write_address / 256,
                        0,
                        256
                    );
                    data += 256;
                    write_address += 256;
                    size -= 256;
                }
                else 
                {      
                    uint8_t byte = *(uint8_t*)data;           
                    W25qxx_WriteByte(byte, write_address);
                    data++;
                    write_address++;
                    size--;
                }
            }
            ret = true;
        }
    }

    return ret;
}

bool SPI_Flash_ErasePartition(SPI_Flash_PartitionId_t partition)
{
    bool ret = false;
    uint32_t start_address;
    uint32_t bytes_left;
    if(partition < PARTITION_CNT)
    {
        start_address = spi_flash_partitions[partition].base_address;
        bytes_left = spi_flash_partitions[partition].size;
        while(bytes_left)
        {
            if((bytes_left >= 65536) && ((start_address % 65536) == 0)) // Block
            {
                W25qxx_EraseBlock(start_address / 65536);
                start_address += 65536;
                bytes_left -= 65536;
            }
            else if((bytes_left >= 4096) && ((start_address % 4096) == 0)) // Sector
            {
                W25qxx_EraseSector(start_address / 4096);
                start_address += 4096;
                bytes_left -= 4096;
            }
        }
    }
    return ret;
}
/******************************************************************************/
/*  Static function declarations                                              */
/******************************************************************************/