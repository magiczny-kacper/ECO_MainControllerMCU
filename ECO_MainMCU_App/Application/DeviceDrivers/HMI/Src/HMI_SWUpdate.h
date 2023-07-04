#ifndef HMI_SWUPDATE_H_
#define HMI_SWUPDATE_H_

#include <stdbool.h>
#include <stdint.h>

void HMI_SWUpdate_Init(void);

bool HMI_SWUpdate_Start(uint8_t display, uint32_t file_size);

bool HMI_SWUpdate_UpdateBlock(uint32_t offset, uint32_t size, uint8_t* data);

void HMI_SWUpdate_Finalize(void);

void HMI_SWUpdate_DataSentCallback(void);

void HMI_SWUpdate_ReponseReceivedCallback(uint16_t size);

#endif /* HMI_SWUPDATE_H_ */
