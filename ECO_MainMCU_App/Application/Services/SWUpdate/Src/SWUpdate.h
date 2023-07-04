#ifndef SWUPDATE_H_
#define SWUPDATE_H_

#include <stdint.h>

/**
 * @brief Initializes Software Update service
 * 
 */
void SWUpdate_Init(void);

/**
 * @brief Callback for ethernet frames received on update port
 * 
 * @param interrupt Event kind
 * @param receive_size Size of received data
 * @param receive_buffer Receive buffer pointer
 * @param max_transmit_size Size of transmit buffer
 * @param transmit_buffer Transmit buffer pointer
 * @return uint16_t Size of response data
 */
uint16_t SWUPdate_Network_Callback(uint8_t interrupt, 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer);
        
#endif /* SWUPDATE_H_ */
