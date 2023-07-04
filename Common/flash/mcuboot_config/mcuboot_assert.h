/*
 * mcuboot_assert.h
 *
 *  Created on: Mar 3, 2023
 *      Author: kapib
 */

#ifndef INC_MCUBOOT_CONFIG_MCUBOOT_ASSERT_H_
#define INC_MCUBOOT_CONFIG_MCUBOOT_ASSERT_H_

//#include "hal/logging.h"

extern void example_assert_handler(const char *file, int line);

#ifdef assert
#undef assert
#endif

#define assert(exp) \
  do {                                                  \
    if (!(exp)) {                                       \
      example_assert_handler(__FILE__, __LINE__);       \
    }                                                   \
  } while (0)

#endif /* INC_MCUBOOT_CONFIG_MCUBOOT_ASSERT_H_ */
