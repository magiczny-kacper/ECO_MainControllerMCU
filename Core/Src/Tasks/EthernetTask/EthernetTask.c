/*
 * EthernetTask.c
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#include "EthernetTask.h"

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "cmsis_os.h"
#include "main.h"

#include "../../CLI/CLI.h"
#include "../../RuntimeStats/RuntimeStats.h"
#include "../ConfigEEPROM/config.h"
#include "../Flash/dataLog.h"
#include "../Modbus/ModbusTCPServer.h"

#include "wizchip_conf.h"
#include "socket.h"
#include "W5500/w5500.h"

#include "../Internet/DNS/dns.h"
#include "../Internet/SNTP/sntp.h"

#define MODBUS_SOCKET 0
#define MODBUS_PORT 502

#define HTTP_SOCKET 1
#define HTTP_PORT 80

#define MQTT_SOCKET 2
#define MQTT_PORT 1883

#define TELNET_SOCKET 3
#define TELNET_PORT 23

#define DNS_SOCKET 6
#define DNS_PORT 53

#define SNTP_SOCKET 7
#define SNTP_PORT 123

#define TELNET_TX_BUF_SIZE 256
#define TELNET_RX_BUF_SIZE 256

#define MB_TX_BUF_SIZE 256
#define MB_RX_BUF_SIZE 256

extern osMutexId SPIMutexHandle;
extern SPI_HandleTypeDef hspi1;
extern osMutexId ModbusMutexHandle;
extern ModbusRTUMaster_t mbPort;
extern RTC_HandleTypeDef hrtc;

static const uint8_t gretMsg[] = 	"ECO_MainMCU Telnet.\r\nSoftware version: 0.0.1\r\n";

static uint8_t rcvBuf[TELNET_RX_BUF_SIZE], txBuf[TELNET_TX_BUF_SIZE];
static uint8_t mbRxBuf[MB_RX_BUF_SIZE], mbTxBuf[MB_TX_BUF_SIZE];

static uint8_t dnsBuf[MAX_DNS_BUF_SIZE];
static const uint8_t dnsIp[4] = {8,8,8,8};

static uint8_t ntpBuf[256];
static const uint8_t* ntpUrl = "ntp1.tp.pl";
static uint8_t ntpIp[4];

uint16_t freesize;
static const uint8_t bufSize[8] = {2, 2, 2, 2, 2, 2, 2, 2};
static wiz_NetInfo netInfo = {	.mac 	= {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},	// Mac address
	                  	 .ip 	= {192, 168, 0, 192},					// IP address
	                     .sn 	= {255, 255, 255, 0},					// Subnet mask
	                     .gw 	= {192, 168, 0, 1}};					// Gateway address


EthernetConfig_t ethConf;
datetime time;

static void cs_sel();

static void cs_desel();

static void cs_sel() {
	xSemaphoreTake(SPIMutexHandle, portMAX_DELAY);
	HAL_GPIO_WritePin(ETH_CS_GPIO_Port, ETH_CS_Pin, GPIO_PIN_RESET); //CS LOW
}

static void cs_desel() {
	HAL_GPIO_WritePin(ETH_CS_GPIO_Port, ETH_CS_Pin, GPIO_PIN_SET); //CS HIGH
	xSemaphoreGive(SPIMutexHandle);
}

static void W5500_ReadBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Receive(&hspi1, buff, len, HAL_MAX_DELAY);
}

static void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Transmit(&hspi1, buff, len, HAL_MAX_DELAY);
}

static uint8_t W5500_ReadByte(void) {
    uint8_t byte;
    W5500_ReadBuff(&byte, sizeof(byte));
    return byte;
}

static void W5500_WriteByte(uint8_t byte) {
    W5500_WriteBuff(&byte, sizeof(byte));
}

void EthernetTask(void* argument)
{
	(void) argument;
  /* USER CODE BEGIN EthernetTask */
	BaseType_t xMoreDataToFollow;
	intr_kind interruptSource;
	uint8_t interrupt;
	uint8_t first_frame = 1;
	uint8_t remoteIP[4][4];
	uint16_t remotePort[4];
	int32_t rcvSize = 0;
	int32_t mbRcvSize = 0;

	reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);

	CLI_Init();

	if(Config_GetEthernetConfig(&ethConf) != CONF_OK){
		vTaskSuspend(NULL);
	}

	memcpy(&netInfo.gw, &ethConf.gatewayAddress, 4);
	memcpy(&netInfo.ip, &ethConf.ipAddress, 4);
	memcpy(&netInfo.mac, &ethConf.macAddress, 6);
	memcpy(&netInfo.sn, &ethConf.subnetMask, 4);

	wizchip_sw_reset();
	wizchip_init(bufSize, bufSize);

	wizchip_setnetinfo(&netInfo);
	wizchip_setinterruptmask(IK_SOCK_0 | IK_SOCK_1 | IK_SOCK_3 | IK_SOCK_7);
	wizchip_getnetinfo(&netInfo);



	// Modbus Port
	if(socket(MODBUS_SOCKET, Sn_MR_TCP, MODBUS_PORT, SF_TCP_NODELAY) == MODBUS_SOCKET) {
		  	  /* Put socket in LISTEN mode. This means we are creating a TCP server */
		if(listen(MODBUS_SOCKET) == SOCK_OK) {
			setSn_IMR(MODBUS_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
		}
	}

	// Telnet Port (CLI, debug)
	if(socket(TELNET_SOCKET, Sn_MR_TCP, TELNET_PORT, SF_TCP_NODELAY) == TELNET_SOCKET){
		if(listen(TELNET_SOCKET) == SOCK_OK) {
			setSn_IMR(TELNET_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
		}
	}

	if(socket(DNS_SOCKET, Sn_MR_UDP, DNS_PORT, 0) == DNS_SOCKET){
		setSn_IMR(DNS_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
		DNS_init(DNS_SOCKET, dnsBuf);
		DNS_run(dnsIp, ntpUrl, ntpIp);
	}

	if(socket(SNTP_SOCKET, Sn_MR_UDP, SNTP_PORT, 0) == SNTP_SOCKET){
		setSn_IMR(SNTP_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
		SNTP_init(SNTP_SOCKET, ntpIp, 25, ntpBuf);
		SNTP_run(&time);
	}

  /* Infinite loop */
	for(;;)
	{
		if(pdTRUE == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, portMAX_DELAY)){
			while(HAL_GPIO_ReadPin(ETH_INT_GPIO_Port, ETH_INT_Pin) == 0){
				interruptSource = wizchip_getinterrupt();

				if(interruptSource & IK_WOL){

				}

				if(interruptSource & IK_PPPOE_TERMINATED){

				}

				if(interruptSource & IK_DEST_UNREACH){

				}

				if(interruptSource & IK_IP_CONFLICT){

				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_0){
					freesize = getSn_TxMAX(MODBUS_SOCKET);
					interrupt = getSn_IR(MODBUS_SOCKET);
					setSn_IR(MODBUS_SOCKET, interrupt);
					if(interrupt & Sn_IR_CON){

					}

					if(interrupt & Sn_IR_RECV){
						int32_t mbTxSize;
						mbRcvSize = recv(MODBUS_SOCKET, mbRxBuf, MB_RX_BUF_SIZE);
						RuntimeStats_ModbusSlaveRqAllInc();
						xSemaphoreTake(ModbusMutexHandle, portMAX_DELAY);
						mbTxSize = (int16_t)ModbusTCPServer_Proc(&mbPort, mbRxBuf, mbRcvSize, mbTxBuf);
						xSemaphoreGive(ModbusMutexHandle);
						if(mbTxSize > MODBUS_SOCKET){
							RuntimeStats_ModbusSlaveRqOkInc();
							freesize = send(MODBUS_SOCKET, mbTxBuf, mbTxSize);
							RuntimeStats_ModbusSlaveRespInc();
						}else{
							RuntimeStats_ModbusSlaveRqErrInc();
						}
					}

					if(interrupt & Sn_IR_SENDOK){
						memset(mbTxBuf, 0, MB_TX_BUF_SIZE);
						ClrSiS(MODBUS_SOCKET);
					}

					if((interrupt & Sn_IR_DISCON) || (interrupt & Sn_IR_TIMEOUT)){
						disconnect(MODBUS_SOCKET);
						if(socket(MODBUS_SOCKET, Sn_MR_TCP, MODBUS_PORT, SF_TCP_NODELAY) == MODBUS_SOCKET){
							if(listen(MODBUS_SOCKET) == SOCK_OK) {
								setSn_IMR(MODBUS_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
							}
						}
					}
				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_1){
					interrupt = getSn_IR(HTTP_SOCKET);
					if(interrupt & Sn_IR_CON){
						getsockopt(HTTP_SOCKET, SO_DESTIP, &remoteIP[1]);
						getsockopt(HTTP_SOCKET, SO_DESTPORT, (uint8_t*)&remotePort[1]);

					}

					if(interrupt & Sn_IR_RECV){
						RuntimeStats_TelnetRxInc();
						rcvSize += recv(HTTP_SOCKET, &rcvBuf[rcvSize], TELNET_RX_BUF_SIZE);
					}
				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_2){
					interrupt = getSn_IR(MQTT_SOCKET);
				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_3){
					freesize = getSn_TxMAX(TELNET_SOCKET);
					interrupt = getSn_IR(TELNET_SOCKET);
					setSn_IR(3, interrupt);
					if(interrupt & Sn_IR_CON){
						getsockopt(TELNET_SOCKET, SO_DESTIP, &remoteIP[TELNET_SOCKET]);
						getsockopt(TELNET_SOCKET, SO_DESTPORT, (uint8_t*)&remotePort[TELNET_SOCKET]);
						RuntimeStats_TelnetCurrIPSet(&remoteIP[TELNET_SOCKET][0]);
						RuntimeStats_TelnetTxInc();
						DataLog_LogEvent(EV_TELNET_CONN, NULL, 0);
						freesize = send(TELNET_SOCKET, (uint8_t*)gretMsg, 47);
						first_frame = 1;
					}

					if(interrupt & Sn_IR_RECV){
						if(first_frame == 0){
							RuntimeStats_TelnetRxInc();
							rcvSize += recv(TELNET_SOCKET, &rcvBuf[rcvSize], TELNET_RX_BUF_SIZE);
							while(((rcvBuf[rcvSize - 1] == '\r') || (rcvBuf[rcvSize - 1] == '\n')) && (rcvSize > 0)){
								rcvBuf[rcvSize - 1] = 0;
								rcvSize--;
							}

							do{
								xMoreDataToFollow = FreeRTOS_CLIProcessCommand(&rcvBuf, &txBuf, TELNET_TX_BUF_SIZE);
								RuntimeStats_TelnetTxInc();
								freesize = send(TELNET_SOCKET, txBuf, strlen((char*)txBuf));
								vTaskDelay(1);
							}while(xMoreDataToFollow != pdFALSE);
							rcvSize = 0;
						}else{
							recv(3, rcvBuf, 128);
							first_frame = 0;
						}
					}

					if(interrupt & Sn_IR_SENDOK){
						memset(txBuf, 0, TELNET_TX_BUF_SIZE);
						ClrSiS(TELNET_SOCKET);
					}

					if((interrupt & Sn_IR_DISCON) || (interrupt & Sn_IR_TIMEOUT)){
						RuntimeStats_TelnetLastIPSet();
						DataLog_LogEvent(EV_TELNET_DISCON, NULL, 0);
						disconnect(TELNET_SOCKET);
						if(socket(TELNET_SOCKET, Sn_MR_TCP, TELNET_PORT, SF_TCP_NODELAY) == 3){
							if(listen(TELNET_SOCKET) == SOCK_OK) {
								setSn_IMR(TELNET_SOCKET, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
							}
						}
					}

				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_4){
					interrupt = getSn_IR(4);
				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_5){
					interrupt = getSn_IR(5);
				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_6){
					interrupt = getSn_IR(6);
				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_7){
					interrupt = getSn_IR(7);
					if(SNTP_run(&time)){
						RTC_TimeTypeDef timeh;
						RTC_DateTypeDef date;
						uint8_t yr = (uint8_t)(time.yy - 2000);

						HAL_RTC_GetTime(&hrtc, &timeh, RTC_FORMAT_BIN);
						HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
						if((timeh.Hours != time.hh) || (timeh.Minutes != time.mm) ||
							(date.Date != time.dd) || (date.Month != time.mo) || (date.Year != yr)){
							date.Date = time.dd;
							date.Month = time.mo;
							date.Year = yr;
							timeh.Hours = time.hh;
							timeh.Minutes = time.mm;

							HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
							HAL_RTC_SetTime(&hrtc, &timeh, RTC_FORMAT_BIN);
							DataLog_LogEvent(EV_RTC_UPDATE, NULL, 0);
						}
					}

				}
			}
		}
	}
  /* USER CODE END EthernetTask */
}

void EthernetTask_Request (uint32_t request){

}
