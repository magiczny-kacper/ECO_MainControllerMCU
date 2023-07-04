#ifndef ETHERNET_CFG_H_
#define ETHERNET_CFG_H_

#include <stdint.h>
#include "Ethernet_Defs.h"

#define MODBUS_SOCKET 0 /** Socket used for modbus */
#define MODBUS_PORT 502 /** Modbus port */
#define MB_TX_BUF_SIZE 256 /** Modbus transmit buffer size */
#define MB_RX_BUF_SIZE 256 /** Modbus receive buffer size */

#define TELNET_SOCKET 1 /** Socket used for telnet */
#define TELNET_PORT 23 /** Telnet port */
#define TELNET_TX_BUF_SIZE 256 /** Telnet trasmit buffer size */
#define TELNET_RX_BUF_SIZE 256 /** Telnet receive buffer size */

#define SWDL_SOCKET 2
#define SWDL_PORT 4300
#define SWDL_RX_BUFFER_SIZE 2048
#define SWDL_TX_BUFFER_SIZE 256

#define MQTT_SOCKET 3
#define MQTT_PORT 1883
#define MQTT_RX_BUFFER_SIZE 2048
#define MQTT_TX_BUFFER_SIZE 2048

#define DNS_SOCKET 3
#define DNS_PORT 53

#define SNTP_SOCKET 4
#define SNTP_PORT 123

#define TASK_STACK_SIZE 150

#define USED_SOCKETS 4

extern const char *prv_pucGreetingMessage;
extern const uint8_t prv_ucDnsIp[4];
extern const char prv_pucNtpUrl[];
extern const uint8_t prv_ucBufferSizes[8];

extern const Eth_Socket_Config_t socket_config[USED_SOCKETS];

#endif /* ETHERNET_CFG_H_ */
