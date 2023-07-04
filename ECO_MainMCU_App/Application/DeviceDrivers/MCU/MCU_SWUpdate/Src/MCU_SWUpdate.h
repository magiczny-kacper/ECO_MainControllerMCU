#ifndef MCU_SWUPDATE_H_
#define MCU_SWUPDATE_H_

#include <stdint.h>
#include <string.h>

void MCU_SWUpdate_SaveChunk(uint32_t offset, uint32_t size, uint8_t* data);

void MCU_SWUpdate_Finalize(void);

#endif /* MCU_SWUPDATE_H_ */
