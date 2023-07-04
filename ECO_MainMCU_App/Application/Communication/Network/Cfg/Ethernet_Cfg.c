#include "Ethernet_Cfg.h"
#include "socket.h"
#include "SWUpdate.h"

const char *prv_pucGreetingMessage = "ECO_MainMCU Telnet.\r\nSoftware version: 0.0.1\r\n";
const uint8_t prv_ucDnsIp[4] = {8,8,8,8};
const char prv_pucNtpUrl[] = "time.coi.pw.edu.pl";
const uint8_t prv_ucBufferSizes[8] = {2, 2, 2, 2, 2, 2, 2, 2};

static uint8_t prv_ucTelnetReceiveBuffer[TELNET_RX_BUF_SIZE];
static uint8_t prv_ucTelnetTransmitBuffer[TELNET_TX_BUF_SIZE];
static uint8_t prv_ucModbusReceiveBuffer[MB_RX_BUF_SIZE];
static uint8_t prv_ucModbusTransmitBuffer[MB_TX_BUF_SIZE];
static uint8_t prv_ucSWDLTransmitBuffer[SWDL_TX_BUFFER_SIZE];
static uint8_t prv_ucSWDLReceiveBuffer[SWDL_RX_BUFFER_SIZE];
static uint8_t prv_ucMQTTTransmitBuffer[MQTT_TX_BUFFER_SIZE];
static uint8_t prv_ucMQTTReceiveBuffer[MQTT_RX_BUFFER_SIZE];

const Eth_Socket_Config_t socket_config[USED_SOCKETS] = 
{
    {
        .callback = NULL, //Ethernet_Telnet_Callback,
        .tx_buffer = prv_ucTelnetTransmitBuffer,
        .rx_buffer = prv_ucTelnetReceiveBuffer,
        .tx_buffer_size = TELNET_TX_BUF_SIZE,
        .rx_buffer_size = TELNET_RX_BUF_SIZE,
        .socket = TELNET_SOCKET,
        .protocol = Sn_MR_TCP,
        .flags = SF_TCP_NODELAY,
        .interrupts = (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK),
        .port = TELNET_PORT
    },
    {
        .callback = NULL, //Ethernet_Modbus_Callback,
        .tx_buffer = prv_ucModbusTransmitBuffer,
        .rx_buffer = prv_ucModbusReceiveBuffer,
        .tx_buffer_size = MB_TX_BUF_SIZE,
        .rx_buffer_size = MB_RX_BUF_SIZE,
        .socket = MODBUS_SOCKET,
        .protocol = Sn_MR_TCP,
        .flags = SF_TCP_NODELAY,
        .interrupts = (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK),
        .port = MODBUS_PORT
    },
    {
        .callback = SWUPdate_Network_Callback,
        .tx_buffer = prv_ucSWDLTransmitBuffer,
        .rx_buffer = prv_ucSWDLReceiveBuffer,
        .tx_buffer_size = SWDL_TX_BUFFER_SIZE,
        .rx_buffer_size = SWDL_RX_BUFFER_SIZE,
        .socket = SWDL_SOCKET,
        .protocol = Sn_MR_UDP,
        .flags = SF_IO_NONBLOCK,
        .interrupts = (Sn_IR_RECV),
        .port = SWDL_PORT
    },
    {
        .callback = NULL,
        .tx_buffer = prv_ucMQTTTransmitBuffer,
        .rx_buffer = prv_ucMQTTReceiveBuffer,
        .tx_buffer_size = MQTT_TX_BUFFER_SIZE,
        .rx_buffer_size = MQTT_RX_BUFFER_SIZE,
        .socket = MQTT_SOCKET,
        .protocol = Sn_MR_TCP,
        .flags = SF_TCP_NODELAY,
        .interrupts = (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK),
        .port = MQTT_PORT
    }
    // {
    //     .callback = NULL,
    //     .tx_buffer = NULL,
    //     .rx_buffer = NULL,
    //     .tx_buffer_size = 0,
    //     .rx_buffer_size = 0,
    //     .socket = DNS_SOCKET,
    //     .protocol = Sn_MR_UDP,
    //     .flags = SF_IO_NONBLOCK,
    //     .interrupts = (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK),
    //     .port = DNS_PORT
    // },
    // {
    //     .callback = NULL,
    //     .tx_buffer = NULL,
    //     .rx_buffer = NULL,
    //     .tx_buffer_size = 0,
    //     .rx_buffer_size = 0,
    //     .socket = SNTP_SOCKET,
    //     .protocol = Sn_MR_UDP,
    //     .flags = SF_IO_NONBLOCK,
    //     .interrupts = (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK),
    //     .port = SNTP_PORT
    // },
};