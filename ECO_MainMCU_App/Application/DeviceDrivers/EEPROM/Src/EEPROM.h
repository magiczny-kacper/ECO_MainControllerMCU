#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include "i2c.h"

#define EE_ADDR             0xA0
#define EE_PAGE_SIZE        16u
#define EE_BYTE_WRITE_TIME  1u
#define EE_PAGE_WRITE_TIME  (EE_PAGE_SIZE * EE_BYTE_WRITE_TIME)
#define EE_BLOCK_SIZE       256u
#define EE_BLOCKS           4u
#define EE_SIZE             1048u

typedef enum{
    EE_ERR_BUSY = -3,
    EE_ERR_NULL = -2,
    EE_ERR_TIMEOUT = -1,
    EE_OK = 0,

} EE_Status_t;

EE_Status_t EE_Init (I2C_HandleTypeDef* hi2c);

EE_Status_t EE_Read (void* data, uint8_t addr, size_t len);

EE_Status_t EE_Write (void* data, uint32_t addr, size_t len);

EE_Status_t EE_Erase (void);

#endif /* EEPROM_H_ */
