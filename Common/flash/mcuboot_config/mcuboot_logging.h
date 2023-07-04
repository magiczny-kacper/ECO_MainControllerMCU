/*
 * mcuboot_logging.h
 *
 *  Created on: Mar 3, 2023
 *      Author: kapib
 */

#ifndef INC_MCUBOOT_CONFIG_MCUBOOT_LOGGING_H_
#define INC_MCUBOOT_CONFIG_MCUBOOT_LOGGING_H_

#include "printf.h"

#define EXAMPLE_LOG(...)   \
  do {                     \
	  printf(__VA_ARGS__); \
  } while (0)

#define MCUBOOT_LOG_MODULE_DECLARE(...)

#define MCUBOOT_LOG_ERR(_fmt, ...) \
  EXAMPLE_LOG("[ERR] " _fmt "\r\n", ##__VA_ARGS__);
#define MCUBOOT_LOG_WRN(_fmt, ...) \
  EXAMPLE_LOG("[WRN] " _fmt "\r\n", ##__VA_ARGS__);
#define MCUBOOT_LOG_INF(_fmt, ...) \
  EXAMPLE_LOG("[INF] " _fmt "\r\n", ##__VA_ARGS__);
#define MCUBOOT_LOG_DBG(_fmt, ...) \
  EXAMPLE_LOG("[DBG] " _fmt "\r\n", ##__VA_ARGS__);

#endif /* INC_MCUBOOT_CONFIG_MCUBOOT_LOGGING_H_ */
