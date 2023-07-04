#ifndef SPI_FLASH_CFG_H_
#define SPI_FLASH_CFG_H_

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/*  Global defines                                                            */
/******************************************************************************/
#define ALIGN_SIZE(sizeToAlign, PowerOfTwo) (((sizeToAlign) + (PowerOfTwo) - 1) & ~((PowerOfTwo) - 1))

#ifdef DEF_FLASH_PARTITION
#undef DEF_FLASH_PARTITION
#endif
#define DEF_FLASH_PARTITION(partition_name, partition_name_str, partition_address, partition_size)

#define SPI_FLASH_PARTITIONS                                                    \
    DEF_FLASH_PARTITION(APP_SLOT_2,    "App_Slot_2",    0x00000000, 0x00030000) \
    DEF_FLASH_PARTITION(APP_SWAP,      "App_Swap",      0x00030000, 0x00001000) \
    DEF_FLASH_PARTITION(DATA_LOG_AREA, "Data_Log_Area", 0x00031000, 0x007CF000)

/******************************************************************************/
/*  Global types                                                              */
/******************************************************************************/

/******************************************************************************/
/*  Global function prototypes                                                */
/******************************************************************************/

#endif /* SPI_FLASH_CFG_H_ */
