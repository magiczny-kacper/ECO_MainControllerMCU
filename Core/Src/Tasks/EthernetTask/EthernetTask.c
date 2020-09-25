/*
 * EthernetTask.c
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#include "EthernetTask.h"

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"

#include "../../CLI/CLI.h"
#include "../../RuntimeStats/RuntimeStats.h"
#include "../ConfigEEPROM/config.h"

#include "wizchip_conf.h"
#include "socket.h"
#include "W5500/w5500.h"

extern osMutexId SPIMutexHandle;
extern SPI_HandleTypeDef hspi1;

static const uint8_t gretMsg[] = 	"ECO_MainMCU Telnet.\r\nSoftware version: 0.0.1\r\n";

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

#define TX_BUF_SIZE 256
#define RX_BUF_SIZE 256

uint16_t freesize;
uint8_t rcvBuf[RX_BUF_SIZE], txBuf[TX_BUF_SIZE], bufSize[] = {2, 2, 2, 2, 2};
static wiz_NetInfo netInfo = {	.mac 	= {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},	// Mac address
	                  	 .ip 	= {192, 168, 0, 192},					// IP address
	                     .sn 	= {255, 255, 255, 0},					// Subnet mask
	                     .gw 	= {192, 168, 0, 1}};					// Gateway address

EthernetConfig_t ethConf;

void EthernetTask(void const * argument)
{
  /* USER CODE BEGIN EthernetTask */
	BaseType_t xMoreDataToFollow;
	intr_kind interruptSource;
	uint8_t interrupt;
	uint8_t first_frame = 1;
	uint8_t remoteIP[4][4];
	uint16_t remotePort[4];
	int32_t rcvSize = 0;

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
	wizchip_init(bufSize, bufSize);

	wizchip_setnetinfo(&netInfo);
	wizchip_setinterruptmask(IK_SOCK_3);
	wizchip_getnetinfo(&netInfo);

	// Modbus Port
	//if(socket(0, Sn_MR_TCP, 502, 0) == 0) {
		  	  /* Put socket in LISTEN mode. This means we are creating a TCP server */
		//if(listen(0) == SOCK_OK) {
		//}
	//}

	// HTTP Port (WebServer)
	/*if(socket(1, Sn_MR_TCP, 80, 0) == 0){
		if(listen(1) == SOCK_OK) {
		}
	}*/

	// MQTT Port
	//if(socket(2, Sn_MR_TCP, 1883, 0) == 0){

	//}

	// Telnet Port (CLI, debug)
	if(socket(3, Sn_MR_TCP, 23, SF_TCP_NODELAY) == 3){
		if(listen(3) == SOCK_OK) {
			setSn_IMR(3, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
		}
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
					interrupt = getSn_IR(0);
					if(interrupt & Sn_IR_CON){
						getsockopt(0, SO_DESTIP, &remoteIP[0]);
						getsockopt(0, SO_DESTPORT, (uint8_t*)&remotePort[0]);
					}
				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_1){
					interrupt = getSn_IR(1);
					if(interrupt & Sn_IR_CON){
						getsockopt(1, SO_DESTIP, &remoteIP[1]);
						getsockopt(1, SO_DESTPORT, (uint8_t*)&remotePort[1]);

					}
				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_2){
					interrupt = getSn_IR(2);
				}
//------------------------------------------------------------------------------------------------------
				if(interruptSource & IK_SOCK_3){
					freesize = getSn_TxMAX(3);
					interrupt = getSn_IR(3);
					setSn_IR(3, interrupt);
					if(interrupt & Sn_IR_CON){
						getsockopt(3, SO_DESTIP, &remoteIP[3]);
						getsockopt(3, SO_DESTPORT, (uint8_t*)&remotePort[3]);
						RuntimeStats_TelnetCurrIPSet(&remoteIP[3][0]);
						RuntimeStats_TelnetTxInc();
						freesize = send(3, (uint8_t*)gretMsg, 47);
						first_frame = 1;
					}

					if(interrupt & Sn_IR_RECV){
						if(first_frame == 0){
							RuntimeStats_TelnetRxInc();
							rcvSize += recv(3, &rcvBuf[rcvSize], RX_BUF_SIZE);
							while(((rcvBuf[rcvSize - 1] == '\r') || (rcvBuf[rcvSize - 1] == '\n')) && (rcvSize > 0)){
								rcvBuf[rcvSize - 1] = 0;
								rcvSize--;
							}

							do{
								xMoreDataToFollow = FreeRTOS_CLIProcessCommand(&rcvBuf, &txBuf, TX_BUF_SIZE);
								RuntimeStats_TelnetTxInc();
								freesize = send(3, txBuf, strlen((char*)txBuf));
								vTaskDelay(1);
							}while(xMoreDataToFollow != pdFALSE);
							rcvSize = 0;
						}else{
							recv(3, rcvBuf, 128);
							first_frame = 0;
						}
					}

					if(interrupt & Sn_IR_SENDOK){
						ClrSiS(3);
					}

					if(interrupt & Sn_IR_DISCON || interrupt & Sn_IR_TIMEOUT){
						RuntimeStats_TelnetLastIPSet();
						disconnect(3);
						if(socket(3, Sn_MR_TCP, 23, SF_TCP_NODELAY) == 3){
							if(listen(3) == SOCK_OK) {
								setSn_IMR(3, (Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON | Sn_IR_TIMEOUT | Sn_IR_SENDOK));
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
				}
			}
		}
	}
  /* USER CODE END EthernetTask */
}

