#ifndef W25QXXCONF_H_
#define W25QXXCONF_H_

#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

extern SPI_HandleTypeDef hspi2;

#define _W25QXX_SPI                   hspi2
#define _W25QXX_CS_GPIO               FLASH_CS_GPIO_Port
#define _W25QXX_CS_PIN                FLASH_CS_Pin
#define _W25QXX_USE_FREERTOS          1
#define _W25QXX_DEBUG                 0

#endif /* W25QXXCONF_H_ */
