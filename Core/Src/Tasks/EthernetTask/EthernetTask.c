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

#include "wizchip_conf.h"
#include "socket.h"

extern osMutexId SPIMutexHandle;
extern SPI_HandleTypeDef hspi1;

static void cs_sel();

static void cs_desel();

static uint8_t spi_rb(void);

static void spi_wb(uint8_t b);

static void cs_sel() {
	xSemaphoreTake(SPIMutexHandle, portMAX_DELAY);
	HAL_GPIO_WritePin(ETH_CS_GPIO_Port, ETH_CS_Pin, GPIO_PIN_RESET); //CS LOW
}

static void cs_desel() {
	HAL_GPIO_WritePin(ETH_CS_GPIO_Port, ETH_CS_Pin, GPIO_PIN_SET); //CS HIGH
	xSemaphoreGive(SPIMutexHandle);
}

static uint8_t spi_rb(void) {
	uint8_t rbuf;
	HAL_SPI_Receive(&hspi1, &rbuf, 1, 0xFFFFFFFF);
	return rbuf;
}

static void spi_wb(uint8_t b) {
	HAL_SPI_Transmit(&hspi1, &b, 1, 0xFFFFFFFF);
}

void EthernetTask(void const * argument)
{
  /* USER CODE BEGIN EthernetTask */
	uint8_t rcvBuf[20], bufSize[] = {2, 2, 2, 2, 2};

	reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
	reg_wizchip_spi_cbfunc(spi_rb, spi_wb);
	wizchip_init(bufSize, bufSize);
	wiz_NetInfo netInfo = {	.mac 	= {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},	// Mac address
		                  	 .ip 	= {192, 168, 0, 192},					// IP address
		                     .sn 	= {255, 255, 255, 0},					// Subnet mask
		                     .gw 	= {192, 168, 0, 1}};					// Gateway address
	wizchip_setnetinfo(&netInfo);
	wizchip_setinterruptmask(0b0000000111000000);
	wizchip_getnetinfo(&netInfo);

	// Modbus Port
	if(socket(0, Sn_MR_TCP, 502, 0) == 0) {
		  	  /* Put socket in LISTEN mode. This means we are creating a TCP server */
		if(listen(0) == SOCK_OK) {
		}
	}

	// HTTP Port (WebServer)
	if(socket(1, Sn_MR_TCP, 80, 0) == 0){
		if(listen(1) == SOCK_OK) {
		}
	}

	// MQTT Port
	if(socket(2, Sn_MR_TCP, 1883, 0) == 0){

	}

	// Telnet Port (CLI, debug)
	if(socket(3, Sn_MR_TCP, 23, 0) == 0){
		if(listen(3) == SOCK_OK) {
		}
	}

	intr_kind interruptSource;
	uint8_t interrupt;
	uint8_t recv_buffer[256];

	uint8_t remoteIP[4][4];
	uint16_t remotePort[4];
  /* Infinite loop */
	for(;;)
	{
		if(pdTRUE == xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, portMAX_DELAY)){
			interruptSource = wizchip_getinterrupt();

			if(interruptSource & IK_WOL){

			}

			if(interruptSource & IK_PPPOE_TERMINATED){

			}

			if(interruptSource & IK_DEST_UNREACH){

			}

			if(interruptSource & IK_IP_CONFLICT){

			}

			if(interruptSource & IK_SOCK_0){
				interrupt = getSn_IR(0);
				if(interrupt & Sn_IR_CON){
					getsockopt(0, SO_DESTIP, &remoteIP[0]);
					getsockopt(0, SO_DESTPORT, (uint8_t*)&remotePort[0]);
				}
			}

			if(interruptSource & IK_SOCK_1){
				interrupt = getSn_IR(1);
				if(interrupt & Sn_IR_CON){
					getsockopt(1, SO_DESTIP, &remoteIP[1]);
					getsockopt(1, SO_DESTPORT, (uint8_t*)&remotePort[1]);
				}
			}

			if(interruptSource & IK_SOCK_2){
				interrupt = getSn_IR(2);
			}

			if(interruptSource & IK_SOCK_3){
				interrupt = getSn_IR(3);
				if(interrupt & Sn_IR_CON){
					getsockopt(3, SO_DESTIP, &remoteIP[3]);
					getsockopt(3, SO_DESTPORT, (uint8_t*)&remotePort[3]);
				}
			}

			if(interruptSource & IK_SOCK_4){
				interrupt = getSn_IR(4);
			}

			if(interruptSource & IK_SOCK_5){
				interrupt = getSn_IR(5);
			}

			if(interruptSource & IK_SOCK_6){
				interrupt = getSn_IR(6);
			}

			if(interruptSource & IK_SOCK_7){
				interrupt = getSn_IR(7);
			}

			wizchip_clrinterrupt(interruptSource);
		}
	}
  /* USER CODE END EthernetTask */
}

