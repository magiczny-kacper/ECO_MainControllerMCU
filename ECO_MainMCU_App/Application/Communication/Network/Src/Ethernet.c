#include <stdbool.h>
#include <string.h>

#include "Ethernet.h"
#include "Ethernet_Defs.h"
#include "Ethernet_Cfg.h"
#include "EthernetRequest.h"

#include "spi.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "main.h"

#include "ResourceGuard.h"

#include "CLI.h"
#include "RuntimeStats.h"
#include "ConfigMgr.h"
#include "EventLog.h"
#include "ModbusTCPServer.h"
#include "SWUpdate.h"

#include "wizchip_conf.h"
#include "socket.h"
#include "w5500.h"

#include "dns.h"
#include "sntp.h"

/*------------------------------ Local defines -------------------------------*/


/*------------------------------- Local types --------------------------------*/

static uint16_t Ethernet_Telnet_Callback(uint8_t interrupt, 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer);

static uint16_t Ethernet_Modbus_Callback(uint8_t interrupt, 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer);

/*----------------------------- External variables ---------------------------*/
extern SPI_HandleTypeDef hspi2;
extern RTC_HandleTypeDef hrtc;

/*------------------------------ Local variables -----------------------------*/
/* Buffers */

static uint8_t prv_ucDnsBuffer[MAX_DNS_BUF_SIZE];
static uint8_t prv_ucNtpBuffer[256];

static datetime xTime;

static TaskHandle_t xEthernetThread = NULL;

static StaticTask_t xTaskBuffer;
static StackType_t xTaskStack[TASK_STACK_SIZE];

static EthernetConfig_t *xEthernetConfig = NULL;
static Eth_Socket_State_t socket_state[8] = {0};

/*------------------------ Static functions prototypes -----------------------*/
static void EthernetTask(void* argument);
static void Ethernet_InitSocket(const Eth_Socket_Config_t* config);
static void W5500_Init(uint16_t interrupts);
static void W5500_CS_Select(void);
static void W5500_CS_Deselect(void);
static void W5500_ReadBuff(uint8_t* buff, uint16_t len);
static void W5500_WriteBuff(uint8_t* buff, uint16_t len);
static uint8_t W5500_ReadByte(void);
static void W5500_WriteByte(uint8_t byte);

/*---------------------- Global functions declarations -----------------------*/
void EthernetTask_Init(void)
{
    // xEthernetSemaphore = xSemaphoreCreateBinaryStatic(&xEthernetSemaphoreBuffer);
    /* Create the task, storing the handle. */
    xEthernetThread = xTaskCreateStatic(
        EthernetTask, 
        "EthernetTask", 
        TASK_STACK_SIZE,
        (void*) NULL, 
        3, 
        xTaskStack, 
        &xTaskBuffer
    );
}

void EthernetTask_IRQ_Callback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(xEthernetThread, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    // xSemaphoreGiveFromISR(xEthernetSemaphore, NULL);
}

void EthernetTask_Tick(void)
{
    DNS_time_handler();
}

/*---------------------- Static functions declarations -----------------------*/
static void EthernetTask(void* argument)
{
    (void) argument;
    
    intr_kind eth_int_source;
    uint8_t eth_socket_int_source;

    xEthernetConfig = ConfigMgr_GetEthernetConfig();
    W5500_Init(/*IK_SOCK_0 | IK_SOCK_1 | IK_SOCK_2 | IK_SOCK_3*/IK_SOCK_2);

    for (uint8_t i = 0; i < (sizeof(socket_config) / sizeof(socket_config[0])); i++)
    {
        socket_state[socket_config[i].socket].config = &socket_config[i];
        Ethernet_InitSocket(&socket_config[i]);
    }

    // if(socket(DNS_SOCKET, Sn_MR_UDP, DNS_PORT, SF_IO_NONBLOCK) == DNS_SOCKET)
    // {
    //     setSn_IMR(DNS_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
    //     DNS_init(DNS_SOCKET, prv_ucDnsBuffer);
    //     DNS_run((uint8_t *)prv_ucDnsIp, (uint8_t *)prv_pucNtpUrl, prv_ucNtpIp);
    // }

    // if(socket(SNTP_SOCKET, Sn_MR_UDP, SNTP_PORT, SF_IO_NONBLOCK) == SNTP_SOCKET)
    // {
    //     setSn_IMR(SNTP_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
    //     SNTP_init(SNTP_SOCKET, prv_ucNtpIp, 25, prv_ucNtpBuffer);
    //     SNTP_run(&xTime);
    // }

  /* Infinite loop */
    while(1)
    {
        if(pdTRUE == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, portMAX_DELAY))
        {
            while(HAL_GPIO_ReadPin(ETH_INT_GPIO_Port, ETH_INT_Pin) == 0)
            {
                eth_int_source = wizchip_getinterrupt();
                
                if(eth_int_source & IK_WOL)
                {

                }

                if(eth_int_source & IK_PPPOE_TERMINATED)
                {

                }

                if(eth_int_source & IK_DEST_UNREACH)
                {

                }

                if(eth_int_source & IK_IP_CONFLICT)
                {

                }

                // if(eth_int_source & IK_SOCK_3)
                // {
                //     eth_socket_int_source = getSn_IR(3);
                //     if(SNTP_run(&xTime))
                //     {
                //         RTC_TimeTypeDef timeh;
                //         RTC_DateTypeDef date;
                //         uint8_t yr = (uint8_t)(xTime.yy - 2000);

                //         HAL_RTC_GetTime(&hrtc, &timeh, RTC_FORMAT_BIN);
                //         HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
                //         if((timeh.Hours != xTime.hh) || (timeh.Minutes != xTime.mm) ||
                //             (date.Date != xTime.dd) || (date.Month != xTime.mo) || (date.Year != yr))
                //         {
                //             date.Date = xTime.dd;
                //             date.Month = xTime.mo;
                //             date.Year = yr;
                //             timeh.Hours = xTime.hh;
                //             timeh.Minutes = xTime.mm;

                //             HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
                //             HAL_RTC_SetTime(&hrtc, &timeh, RTC_FORMAT_BIN);
                //             DataLog_LogEvent(EV_RTC_NTP_UPDATE, NULL, 0);
                //         }
                //     }
                // }

                for(uint8_t socket = 0; socket < 8; socket ++)
                {
                    Eth_Socket_Config_t const *config = socket_state[socket].config;
                    if (NULL == config)
                    {
                        continue;
                    }

                    if (eth_int_source & (1 << (socket + 8)))
                    {
                        eth_socket_int_source = getSn_IR(socket);
                        if ((NULL != config->rx_buffer) &&
                            (0 < config->rx_buffer_size))
                        {
                            if (Sn_MR_UDP == config->protocol)
                            {
                                socket_state[socket].receive_size = recvfrom(socket, 
                                    config->rx_buffer, 
                                    config->rx_buffer_size,
                                    socket_state[socket].remote_ip,
                                    &socket_state[socket].remote_port);
                            }
                            else if (Sn_MR_TCP == config->protocol)
                            {
                                if (eth_socket_int_source & Sn_IR_CON){
                                    getsockopt(TELNET_SOCKET, SO_DESTIP, 
                                        socket_state[socket].remote_ip);
                                    getsockopt(TELNET_SOCKET, SO_DESTPORT, 
                                        &socket_state[socket].remote_port);
                                }
                                socket_state[socket].receive_size = recv(socket, 
                                    config->rx_buffer,
                                    config->rx_buffer_size);
                            }
                        }
                        if (NULL != config->callback)
                        {
                            socket_state[socket].transmit_size = config->callback(
                                eth_socket_int_source, 
                                socket_state[socket].receive_size, 
                                config->rx_buffer,
                                config->tx_buffer_size, 
                                config->tx_buffer
                            );
                            if (socket_state[socket].transmit_size > 0)
                            {
                                sendto(socket, 
                                    config->tx_buffer,
                                    socket_state[socket].transmit_size,
                                    socket_state[socket].remote_ip,
                                    socket_state[socket].remote_port);
                            }
                        }
                        setSn_IR(socket, eth_socket_int_source);
                        wizchip_clrinterrupt(1 << (socket + 8));
                    }
                }
            }
        }
    }
}

void EthernetTask_Request (EthernetRequest_t eRequest)
{
    UNUSED(eRequest);
}

static void Ethernet_InitSocket(const Eth_Socket_Config_t const* config)
{
    if (NULL == config) return;

    if (socket(config -> socket, config -> protocol, config -> port, config -> flags) == config -> socket)
    {
        /* Put socket in LISTEN mode. This means we are creating a TCP server */
        if (Sn_MR_TCP == config -> protocol)
        {
            if(listen(config -> socket) == SOCK_OK)
            {
                setSn_IMR(config -> socket, config -> interrupts);
            }
        }
    }
}

static uint16_t Ethernet_Telnet_Callback(uint8_t interrupt, 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer)
{
    (void)interrupt;
    (void)receive_size;
    (void)receive_buffer;
    (void)max_transmit_size;
    (void)transmit_buffer;
    return 0;
    // if (interrupt & Sn_IR_CON){
    //     getsockopt(TELNET_SOCKET, SO_DESTIP, remote_ip[TELNET_SOCKET]);
    //     getsockopt(TELNET_SOCKET, SO_DESTPORT, &remote_port[TELNET_SOCKET]);
    //     RuntimeStats_TelnetCurrIPSet(remote_ip[TELNET_SOCKET]);
    //     RuntimeStats_Telnet_TxFrames_Inc();
    //     DataLog_LogEvent(EV_TELNET_CONN, NULL, 0);
    //     // prv_usFreeSize = send(TELNET_SOCKET, (uint8_t*)prv_pucGreetingMessage, strlen(prv_pucGreetingMessage));
    //     // telnet_config_frame_received = true;

    //     setSn_IR(TELNET_SOCKET, Sn_IR_CON);
    // }

    // if(interrupt & Sn_IR_RECV)
    // {
    //     if(telnet_config_frame_received == true)
    //     {
    //         RuntimeStats_Telnet_RxFrames_Inc();
    //         lReceiveSize += recv(TELNET_SOCKET, &prv_ucTelnetReceiveBuffer[lReceiveSize], TELNET_RX_BUF_SIZE);
    //         while(((prv_ucTelnetReceiveBuffer[lReceiveSize - 1] == '\r') ||
    //             (prv_ucTelnetReceiveBuffer[lReceiveSize - 1] == '\n')) &&
    //             (lReceiveSize > 0))
    //         {
    //             prv_ucTelnetReceiveBuffer[lReceiveSize - 1] = 0;
    //             lReceiveSize--;
    //         }

    //         do
    //         {
    //             cli_data_ready = FreeRTOS_CLIProcessCommand(
    //                     (const char * const)prv_ucTelnetReceiveBuffer,
    //                     (char *)prv_ucTelnetTransmitBuffer,
    //                     TELNET_TX_BUF_SIZE);
    //             RuntimeStats_Telnet_TxFrames_Inc();
    //             prv_usFreeSize = send(TELNET_SOCKET, prv_ucTelnetTransmitBuffer, strlen((char*)prv_ucTelnetTransmitBuffer));
    //             vTaskDelay(1);
    //         }
    //         while(cli_data_ready != pdFALSE);
    //         lReceiveSize = 0;
    //     }
    //     else
    //     {
    //         recv(TELNET_SOCKET, prv_ucTelnetReceiveBuffer, 128);
    //         // prv_usFreeSize = send(TELNET_SOCKET, prv_ucReceiveBuffer, 128);
    //         prv_usFreeSize = send(TELNET_SOCKET, (uint8_t*)prv_pucGreetingMessage, strlen(prv_pucGreetingMessage));
    //         telnet_config_frame_received = true;
    //     }

    //     setSn_IR(TELNET_SOCKET, Sn_IR_RECV);
    // }

    // if(interrupt & Sn_IR_SENDOK)
    // {
    //     memset(prv_ucTelnetTransmitBuffer, 0, TELNET_TX_BUF_SIZE);
    //     ClrSiS(TELNET_SOCKET);
    //     // prv_usFreeSize = send(TELNET_SOCKET, (uint8_t*)prv_pucGreetingMessage, strlen(prv_pucGreetingMessage));
    //     // telnet_config_frame_received = true;

    //     setSn_IR(TELNET_SOCKET, Sn_IR_SENDOK);
    // }

    // if((interrupt & Sn_IR_DISCON) || (interrupt & Sn_IR_TIMEOUT))
    // {
    //     RuntimeStats_TelnetLastIPSet();
    //     DataLog_LogEvent(EV_TELNET_DISCON, NULL, 0);
    //     disconnect(TELNET_SOCKET);
    //     if(socket(TELNET_SOCKET, Sn_MR_TCP, TELNET_PORT, SF_TCP_NODELAY) == TELNET_SOCKET)
    //     {
    //         if(listen(TELNET_SOCKET) == SOCK_OK)
    //         {
    //             setSn_IMR(TELNET_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
    //         }
    //     }
    // }
}

static uint16_t Ethernet_Modbus_Callback(uint8_t interrupt, 
        uint16_t receive_size, uint8_t* receive_buffer, 
        uint16_t max_transmit_size, uint8_t* transmit_buffer)
{
    (void)interrupt;
    (void)receive_size;
    (void)receive_buffer;
    (void)max_transmit_size;
    (void)transmit_buffer;
    return 0;
    // prv_usFreeSize = getSn_TxMAX(MODBUS_SOCKET);
    // eth_socket_int_source = getSn_IR(0);
    // setSn_IR(MODBUS_SOCKET, eth_socket_int_source);
    // if(interrupt & Sn_IR_CON)
    // {

    // }

    // if(interrupt & Sn_IR_RECV)
    // {
    //     int32_t mbTxSize;
    //     lModbusReceiveSize = recv(MODBUS_SOCKET, prv_ucModbusReceiveBuffer, MB_RX_BUF_SIZE);
    //     RuntimeStats_ModbusSlave_AllRequests_Inc();
    //     mbTxSize = (int16_t)ModbusTCPServer_Proc(prv_ucModbusReceiveBuffer, lModbusReceiveSize, prv_ucModbusTransmitBuffer);
    //     if(mbTxSize > MODBUS_SOCKET)
    //     {
    //         RuntimeStats_ModbusSlave_OkResponses_Inc();
    //         prv_usFreeSize = send(MODBUS_SOCKET, prv_ucModbusTransmitBuffer, mbTxSize);
    //     }
    //     else
    //     {
    //         RuntimeStats_ModbusMaster_NoResponses_Inc();
    //     }
    // }

    // if(interrupt & Sn_IR_SENDOK)
    // {
    //     memset(prv_ucModbusTransmitBuffer, 0, MB_TX_BUF_SIZE);
    //     ClrSiS(MODBUS_SOCKET);
    // }

    // if((interrupt & Sn_IR_DISCON) || (interrupt & Sn_IR_TIMEOUT))
    // {
    //     disconnect(MODBUS_SOCKET);
    //     if(socket(MODBUS_SOCKET, Sn_MR_TCP, MODBUS_PORT, SF_TCP_NODELAY) == MODBUS_SOCKET)
    //     {
    //         if(listen(MODBUS_SOCKET) == SOCK_OK)
    //         {
    //             setSn_IMR(MODBUS_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
    //         }
    //     }
    // }
}

static void W5500_Init(uint16_t interrupts)
{
    wiz_NetInfo xNetworkInfo;

    memcpy(&xNetworkInfo.gw, &xEthernetConfig->gatewayAddress, 4);
    memcpy(&xNetworkInfo.ip, &xEthernetConfig->ipAddress, 4);
    memcpy(&xNetworkInfo.mac, &xEthernetConfig->macAddress, 6);
    memcpy(&xNetworkInfo.sn, &xEthernetConfig->subnetMask, 4);

    reg_wizchip_cs_cbfunc(W5500_CS_Select, W5500_CS_Deselect);
    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);

    wizchip_sw_reset();
    wizchip_init((uint8_t*)prv_ucBufferSizes, (uint8_t*)prv_ucBufferSizes);
    wizchip_setnetinfo(&xNetworkInfo);
    wizchip_setinterruptmask(interrupts);
}

static void W5500_CS_Select(void)
{
    RG_SPI_ChipSelect(ETH_CS_GPIO_Port, ETH_CS_Pin);
}

static void W5500_CS_Deselect(void)
{
    RG_SPI_ChipDeselect(ETH_CS_GPIO_Port, ETH_CS_Pin);
}

static void W5500_ReadBuff(uint8_t* buff, uint16_t len)
{
    if (HAL_ERROR == HAL_SPI_Receive(&hspi2, buff, len, 100))
    {
        __NOP();
    }
}

static void W5500_WriteBuff(uint8_t* buff, uint16_t len)
{
    if (HAL_ERROR == HAL_SPI_Transmit(&hspi2, buff, len, 100))
    {
        __NOP();
    }
}

static uint8_t W5500_ReadByte(void)
{
    uint8_t byte;
    W5500_ReadBuff(&byte, sizeof(byte));
    return byte;
}

static void W5500_WriteByte(uint8_t byte)
{
    W5500_WriteBuff(&byte, sizeof(byte));
}