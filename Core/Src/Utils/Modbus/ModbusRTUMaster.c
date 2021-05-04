/**
 * @addtogroup ModbusRTUMaster
 * @{
 */

#include "ModbusRTUMaster.h"
#include "../RuntimeStats/RuntimeStats.h"

static uint32_t send_frame (ModbusRTUMaster_t* mb, uint8_t* data, uint16_t len);

void ModbusRTUMaster_Init (ModbusRTUMaster_t *modbus, UART_HandleTypeDef *port, SemaphoreHandle_t mutex, TickType_t timeout){
	modbus -> mutex = mutex;
	modbus -> ModbusSerialPort = port;
	UARTDMA_Init(&modbus -> dma, port);
	modbus->dma.callbackArgs = modbus;
	modbus->dma.callback = ModbusRTUMaster_ReceieveResponseCallback;
	//HAL_UART_DMAStop(port);
	modbus -> timeout_t = timeout / portTICK_PERIOD_MS;
}


uint16_t ModbusRTU_CalculateCRC (uint8_t frame_length, uint8_t *frame){
	uint16_t calculatedCRC = 0xFFFF;
	uint16_t CRChigh, CRClow;
	uint8_t* ptr = frame;
	for(int i = 0; i < frame_length; i++){
		calculatedCRC ^= (*ptr);
		for(int j = 8; j > 0; j--){
			if(calculatedCRC & 0x0001){
				calculatedCRC >>= 1;
				calculatedCRC ^= 0xA001;
			}else{
				calculatedCRC >>= 1;
			}
		}
		ptr++;
	}

	CRChigh = (calculatedCRC & 0x00FF) <<8;
	CRClow = (calculatedCRC & 0xFF00) >>8;
	calculatedCRC = CRChigh + CRClow;

	return calculatedCRC;
}


uint8_t ModbusRTU_CheckCRC (uint8_t frame_length, uint8_t *frame){
	uint16_t crc = ModbusRTU_CalculateCRC(frame_length - 2, frame);
	uint16_t frameCRC = (uint16_t)(*(frame + frame_length - 2));
	frameCRC <<= 8;
	frameCRC += *(frame + frame_length - 1);

	if(crc == frameCRC){
		return 1;
	}

	return 0;
}

ModbusState_t ModbusRTUMaster_ReadCoilStatus (ModbusRTUMaster_t *modbus, uint8_t slave_address, uint16_t first_coil, uint16_t number_of_coils, uint8_t *response_frame){
	uint8_t transmit_frame_buffer[8];
	uint16_t transmitCRC;
	uint8_t coil_regs = number_of_coils / 8;
	if(number_of_coils % 8) coil_regs++;
	uint8_t received_frame_length = coil_regs + 5;
	uint8_t received_frame_buffer[received_frame_length];
	for(int i = 0; i < received_frame_length; i++){
		received_frame_buffer[i] = 0;
	}
	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x01;
	transmit_frame_buffer[2] = (uint8_t)(first_coil >> 8);
	transmit_frame_buffer[3] = (uint8_t)first_coil;
	transmit_frame_buffer[4] = (uint8_t)(number_of_coils >> 8);
	transmit_frame_buffer[5] = (uint8_t)(number_of_coils);
	transmitCRC = ModbusRTU_CalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	if(0 != send_frame(modbus, transmit_frame_buffer, 8)){
		if(ModbusRTU_CheckCRC(received_frame_length, received_frame_buffer)){
			if(received_frame_buffer[0] == slave_address && received_frame_buffer[1] == 0x04){
				for(int i = 0; i < received_frame_buffer[2] + 2; i ++){
					*response_frame = received_frame_buffer[i + 3];
					response_frame++;
				}
				RuntimeStats_ModbusMasterRespOkInc();
				return Modbus_OK;
			}
			RuntimeStats_ModbusMasterWrongRespInc();
			return Modbus_CRCERR;
		}
		RuntimeStats_ModbusMasterWrongRespInc();
		return Modbus_CRCERR;
	}else{
		HAL_UART_AbortReceive_IT(modbus -> ModbusSerialPort);
		RuntimeStats_ModbusMasterNoRespInc();
		return Modbus_TIMEOUT;
	}
	RuntimeStats_ModbusMasterWrongRespInc();
	return Modbus_CRCERR;
}

ModbusState_t ModbusRTUMaster_ReadInputStatus (ModbusRTUMaster_t *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame){
	uint8_t transmit_frame_buffer[8];
	uint16_t transmitCRC;
	uint8_t received_frame_length = (registers_count * 2) + 5;
	uint8_t received_frame_buffer[received_frame_length];
	for(int i = 0; i < received_frame_length; i++){
		received_frame_buffer[i] = 0;
	}
	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x02;
	transmit_frame_buffer[2] = (uint8_t)(first_register_address >> 8);
	transmit_frame_buffer[3] = (uint8_t)first_register_address;
	transmit_frame_buffer[4] = (uint8_t)(registers_count >> 8);
	transmit_frame_buffer[5] = (uint8_t)(registers_count);
	transmitCRC = ModbusRTU_CalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	if(0 != send_frame(modbus, transmit_frame_buffer, 8)){
		if(ModbusRTU_CheckCRC(received_frame_length, received_frame_buffer)){
			if(received_frame_buffer[0] == slave_address && received_frame_buffer[1] == 0x04){
				for(int i = 0; i < received_frame_buffer[2] + 2; i ++){
					*response_frame = received_frame_buffer[i + 3];
					response_frame++;
				}
				RuntimeStats_ModbusMasterRespOkInc();
				return Modbus_OK;
			}
			RuntimeStats_ModbusMasterWrongRespInc();
			return Modbus_CRCERR;
		}
		RuntimeStats_ModbusMasterWrongRespInc();
		return Modbus_CRCERR;
	}else{
		HAL_UART_AbortReceive_IT(modbus -> ModbusSerialPort);
		RuntimeStats_ModbusMasterNoRespInc();
		return Modbus_TIMEOUT;
	}
	RuntimeStats_ModbusMasterWrongRespInc();
	return Modbus_CRCERR;
}

ModbusState_t ModbusRTUMaster_ReadHoldingRegisters (ModbusRTUMaster_t *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame){
	uint8_t transmit_frame_buffer[8];
	uint16_t transmitCRC;
	uint8_t received_frame_length = (registers_count * 2) + 5;
	uint8_t received_frame_buffer[received_frame_length];
	for(int i = 0; i < received_frame_length; i++){
		received_frame_buffer[i] = 0;

	}
	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x03;
	transmit_frame_buffer[2] = (uint8_t)(first_register_address >> 8);
	transmit_frame_buffer[3] = (uint8_t)first_register_address;
	transmit_frame_buffer[4] = (uint8_t)(registers_count >> 8);
	transmit_frame_buffer[5] = (uint8_t)(registers_count);
	transmitCRC = ModbusRTU_CalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	if(0 != send_frame(modbus, transmit_frame_buffer, 8)){
		if(ModbusRTU_CheckCRC(received_frame_length, received_frame_buffer)){
			if(received_frame_buffer[0] == slave_address && received_frame_buffer[1] == 0x04){
				for(int i = 0; i < received_frame_buffer[2] + 2; i ++){
					*response_frame = received_frame_buffer[i + 3];
					response_frame++;
				}
				RuntimeStats_ModbusMasterRespOkInc();
				return Modbus_OK;
			}
			RuntimeStats_ModbusMasterWrongRespInc();
			return Modbus_CRCERR;
		}
		RuntimeStats_ModbusMasterWrongRespInc();
		return Modbus_CRCERR;
	}else{
		HAL_UART_AbortReceive_IT(modbus -> ModbusSerialPort);
		RuntimeStats_ModbusMasterNoRespInc();
		return Modbus_TIMEOUT;
	}
	RuntimeStats_ModbusMasterWrongRespInc();
	return Modbus_CRCERR;
}

ModbusState_t ModbusRTUMaster_ReadInputRegisters (ModbusRTUMaster_t *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame, uint8_t* out){
	uint8_t transmit_frame_buffer[8] = {0};
	uint16_t transmitCRC;
	uint8_t received_frame_length = (registers_count * 2) + 5;
	uint8_t received_frame_buffer[received_frame_length];
	uint32_t ulNotificationValue;
	uint32_t rxLen = 0;

	memset(received_frame_buffer, 0, received_frame_length);

	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x04;
	transmit_frame_buffer[2] = (uint8_t)(first_register_address >> 8);
	transmit_frame_buffer[3] = (uint8_t)first_register_address;
	transmit_frame_buffer[4] = (uint8_t)(registers_count >> 8);
	transmit_frame_buffer[5] = (uint8_t)(registers_count);
	transmitCRC = ModbusRTU_CalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	if(0 != send_frame(modbus, transmit_frame_buffer, 8)){
		rxLen = UARTDMA_GetData(&(modbus -> dma), received_frame_buffer);
		if(ModbusRTU_CheckCRC(rxLen, received_frame_buffer)){
			if((received_frame_buffer[0] == slave_address) && ((received_frame_buffer[1] & 127) == 0x04)){
				if((response_frame != NULL) && (!(received_frame_buffer[1] & 128))){
					memcpy(response_frame, received_frame_buffer + 3, received_frame_length - 5);
				}
				if(out != NULL){
					memcpy(out, received_frame_buffer, received_frame_length);
				}
				RuntimeStats_ModbusMasterRespOkInc();
				return Modbus_OK;
			}
			RuntimeStats_ModbusMasterWrongRespInc();
			return Modbus_CRCERR;
		}
		RuntimeStats_ModbusMasterWrongRespInc();
		return Modbus_CRCERR;
	}else{
		RuntimeStats_ModbusMasterNoRespInc();
		return Modbus_TIMEOUT;
	}
	RuntimeStats_ModbusMasterWrongRespInc();
	return Modbus_CRCERR;
}

ModbusState_t ModbusRTUMaster_ForceSingleCoil (ModbusRTUMaster_t *modbus, uint8_t slave_address, uint16_t coil, uint8_t coil_state, uint8_t* out){
	uint8_t transmit_frame_buffer[8];
	uint16_t transmitCRC;
	uint8_t received_frame_length = 0;
	uint8_t received_frame_buffer[received_frame_length];
	for(int i = 0; i < received_frame_length; i++){
		received_frame_buffer[i] = 0;
	}
	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x05;
	transmit_frame_buffer[2] = (uint8_t)(coil >> 8);
	transmit_frame_buffer[3] = (uint8_t)coil;
	if(coil_state){
		transmit_frame_buffer[4] = 0xFF;
		transmit_frame_buffer[5] = 0;
	}else{
		transmit_frame_buffer[4] = 0xFF;
		transmit_frame_buffer[5] = 0;
	}
	transmitCRC = ModbusRTU_CalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	if(0 != send_frame(modbus, transmit_frame_buffer, 8)){
		if(ModbusRTU_CheckCRC(received_frame_length, received_frame_buffer)){
			for(int i = 0 ; i < 8; i++){
				if(transmit_frame_buffer[i] != received_frame_buffer[i]){
					RuntimeStats_ModbusMasterWrongRespInc();
					return Modbus_TIMEOUT;
				}
			}
			if(out != NULL){
				memcpy(out, received_frame_buffer, received_frame_length);
			}
			RuntimeStats_ModbusMasterRespOkInc();
			return Modbus_OK;
		}
		RuntimeStats_ModbusMasterWrongRespInc();
		return Modbus_CRCERR;
	}else{
		HAL_UART_AbortReceive_IT(modbus -> ModbusSerialPort);
		RuntimeStats_ModbusMasterNoRespInc();
		return Modbus_TIMEOUT;
	}
	RuntimeStats_ModbusMasterWrongRespInc();
	return Modbus_CRCERR;
}

void ModbusRTUMaster_ReceieveResponseCallback (ModbusRTUMaster_t *modbus){
	vTaskNotifyGiveFromISR(modbus -> task, NULL);
}

int16_t ModbusRTUMaster_TCPForward (ModbusRTUMaster_t* modbus, uint8_t* inData, uint32_t len, uint8_t* outData){
	int16_t retval = -1;
	uint32_t ulNotificationValue = 0;
	uint32_t rxLen = 0;

	uint16_t calculatedCRC;
	static uint16_t rxDataExcLen = 5;
	uint16_t regsCnt = 0;

	uint8_t txFrameBuf [len + 2];

	rxDataExcLen = 5;

	calculatedCRC = ModbusRTU_CalculateCRC(len, inData);
	memcpy(txFrameBuf, inData, len);
	txFrameBuf[len] = (calculatedCRC >> 8);
	txFrameBuf[len + 1] = (uint8_t)(calculatedCRC);

	switch(txFrameBuf[1]){
		case 1:
		case 2:
			regsCnt = ((uint16_t)txFrameBuf[2] << 8) + txFrameBuf[3];
			rxDataExcLen += regsCnt / 8;
			if(regsCnt % 8){
				rxDataExcLen ++;
			}
			break;
		case 3:
		case 4:
			regsCnt = ((uint16_t)txFrameBuf[4] << 8) + txFrameBuf[5];
			rxDataExcLen += regsCnt * 2;
			break;
		case 5:
		case 6:
		case 15:
		case 16:
			rxDataExcLen = 8;
			break;
		default:
			break;
	}

	uint8_t rxFrameBuf [rxDataExcLen];
	memset(rxFrameBuf, 0, rxDataExcLen);

	if(0 != send_frame(modbus, txFrameBuf, len + 2)){
		rxLen = UARTDMA_GetData(&(modbus -> dma), rxFrameBuf);
		if(ModbusRTU_CheckCRC(rxLen, rxFrameBuf)){
			memcpy(outData, &rxFrameBuf[0], rxLen - 2);
			retval = (int16_t)rxLen - 2;
		}
	}else{
		retval = -1;
	}

	return retval;
}

static uint32_t send_frame (ModbusRTUMaster_t* mb, uint8_t* data, uint16_t len){
	uint32_t ulNotificationValue = 0;
	if(pdTRUE == xSemaphoreTake(mb -> mutex, portMAX_DELAY)){
		HAL_GPIO_WritePin(MASTER_TE_GPIO_Port, MASTER_TE_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin, GPIO_PIN_RESET);
		RuntimeStats_ModbusMasterRqSendInc();
		mb -> task = xTaskGetCurrentTaskHandle();
		HAL_UART_Transmit(mb -> ModbusSerialPort, data, len, 100);
		HAL_GPIO_WritePin(MASTER_TE_GPIO_Port, MASTER_TE_Pin, GPIO_PIN_RESET);
		ulNotificationValue = ulTaskNotifyTake( pdTRUE, mb->timeout_t );
		vTaskDelay(1);
		HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin, GPIO_PIN_SET);
		xSemaphoreGive(mb -> mutex);
	}
	return ulNotificationValue;
}

/**
 * @}
 */
