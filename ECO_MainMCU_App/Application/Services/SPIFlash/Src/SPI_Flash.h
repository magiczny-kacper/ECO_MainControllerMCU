#ifndef SPI_FLASH_H_
#define SPI_FLASH_H_

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "SPI_Flash_Cfg.h"

/******************************************************************************/
/*  Global defines                                                            */
/******************************************************************************/

/******************************************************************************/
/*  Global types                                                              */
/******************************************************************************/
#ifdef DEF_FLASH_PARTITION
#undef DEF_FLASH_PARTITION
#endif
#define DEF_FLASH_PARTITION(partition_name, partition_name_str, partition_address, partition_size) partition_name,

typedef enum {
    SPI_FLASH_PARTITIONS
    PARTITION_CNT,
    PARTITION_INVALID
} SPI_Flash_PartitionId_t;

/******************************************************************************/
/*  Global function prototypes                                                */
/******************************************************************************/

/**
 * @brief Initializes SPI Flash memory
 * 
 */
void SPI_Flash_Init(void);

/**
 * @brief Returns given partition sze
 * 
 * @param partition Partition ID
 * @return uint32_t Partition size in bytes
 */
uint32_t SPI_Flash_GetPartitionSize(SPI_Flash_PartitionId_t partition);

/**
 * @brief Reads data from given partition
 * 
 * @param partition Partitin to read 
 * @param dest Place to read data
 * @param offset Read offset from beginning of partition
 * @param size Size of data to be read
 * @return true Read successfull
 * @return false Read error
 */
bool SPI_Flash_Read(SPI_Flash_PartitionId_t partition, void* dest, uint32_t offset, uint32_t size);

/**
 * @brief rites data to given partition
 * 
 * @param partition Partition to write
 * @param data Data to be written
 * @param offset Write offset from beginning of partition
 * @param size Size of data to be written
 * @return true Write successfull
 * @return false Write error
 */
bool SPI_Flash_Write(SPI_Flash_PartitionId_t partition, void* data, uint32_t offset, uint32_t size);

/**
 * @brief Erases given partition
 * 
 * @param partition Partition to erase
 * @return true Erase successfull
 * @return false Erase error
 */
bool SPI_Flash_ErasePartition(SPI_Flash_PartitionId_t partition);

#endif /* SPI_FLASH_H_ */
