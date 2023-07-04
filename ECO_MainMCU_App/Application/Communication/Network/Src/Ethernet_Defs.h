#ifndef ETHERNET_DEFS_H_
#define ETHERNET_DEFS_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Socket callback function pointer. Called on network event.
 * 
 * @param interrupt Interrupt type
 * @param receive_size Received data size
 * @param receive_buffer Received data pointer
 * @param max_transmit_size Size of reponse buffer
 * @param transmit_buffer Pointer to reponse buffer
 * @return Returns size of reponse
 */
typedef uint16_t (*Socket_Callback_Ptr_t)(uint8_t interrupt,
    uint16_t receive_size, uint8_t* receive_buffer, 
    uint16_t max_transmit_size, uint8_t* transmit_buffer);

/**
 * @brief Ethernet socket config structure
 * 
 */
typedef struct Eth_Socket_Config
{
    Socket_Callback_Ptr_t callback; /** Network event callback function pointer */
    uint8_t* tx_buffer; /** Transmit buffer pointer */
    uint8_t* rx_buffer; /** Receive buffer pointer */
    uint16_t tx_buffer_size; /** Transmit buffer size */
    uint16_t rx_buffer_size; /** Receive buffer size */
    uint8_t socket; /** Socket number. Must be in range 0-7 */
    uint8_t protocol; /** Protocol used by socket */
    uint8_t flags; /** Socket flags */
    uint8_t interrupts; /** Used interrupts mask */
    uint16_t port; /** Socket port */
} Eth_Socket_Config_t;

/**
 * @brief Ethernet socket size
 * 
 */
typedef struct Eth_Socket_State
{
    Eth_Socket_Config_t const* config; /** Socket configuration pointer */
    uint8_t remote_ip[4]; /** Remote IP address */
    uint16_t remote_port; /** Remote port */
    uint16_t receive_size; /** Received data size */
    uint16_t transmit_size; /** Transmit size */
} Eth_Socket_State_t;

#endif /* ETHERNET_DEFS_H_ */
