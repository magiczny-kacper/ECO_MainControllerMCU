#include "Modbus.h"
#include "../RuntimeStats/RuntimeStats.h"

void vModbusInit (ModbusHandler *modbus, UART_HandleTypeDef *port, TickType_t timeout){
	modbus -> ModbusSerialPort = port;
	modbus -> timeout_t = timeout / portTICK_PERIOD_MS;
}

uint16_t uModbusCalculateCRC (uint8_t frame_length, uint8_t *frame){
	uint16_t calculatedCRC = 0xFFFF;
	uint16_t CRChigh, CRClow;
	for(int i = 0; i < frame_length; i++){
		calculatedCRC ^= (*frame);
		for(int j = 8; j > 0; j--){
			if(calculatedCRC & 0x0001){
				calculatedCRC >>= 1;
				calculatedCRC ^= 0xA001;
			}else{
				calculatedCRC >>= 1;
			}
		}
		frame++;
	}

	CRChigh = (calculatedCRC & 0x00FF) <<8;
	CRClow = (calculatedCRC & 0xFF00) >>8;
	calculatedCRC = CRChigh + CRClow;

	//calculatedCRC = HAL_CRC_Calculate(&hcrc, frame, frame_length);
	return calculatedCRC;
}

uint8_t bModbusCheckCRC (uint8_t frame_length, uint8_t *frame){
	uint16_t crc = uModbusCalculateCRC(frame_length - 2, frame);
	uint16_t frameCRC = (uint16_t)(*(frame + frame_length - 2));
	frameCRC <<= 8;
	frameCRC += *(frame + frame_length - 1);

	if(crc == frameCRC){
		return 1;
	}

	return 0;
}

ModbusState vReadCoilStatus (ModbusHandler *modbus, uint8_t slave_address, uint16_t first_coil, uint16_t number_of_coils, uint8_t *response_frame){
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
	transmitCRC = uModbusCalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	//Wy�lij dane;
	HAL_GPIO_WritePin(MASTER_TE_GPIO_Port, MASTER_TE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin, GPIO_PIN_RESET);
	RuntimeStats_ModbusMasterRqSendInc();
	HAL_UART_Transmit_DMA(modbus -> ModbusSerialPort, transmit_frame_buffer, 8);
	//W��cz odbieranie
	HAL_UART_Receive_DMA(modbus -> ModbusSerialPort, received_frame_buffer, received_frame_length);
	uint32_t ulNotificationValue = ulTaskNotifyTake( pdTRUE, modbus->timeout_t );
	vTaskDelay(1);

	if(ulNotificationValue){
		if(bModbusCheckCRC(received_frame_length, received_frame_buffer)){
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

ModbusState vReadInputStatus (ModbusHandler *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame){
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
	transmitCRC = uModbusCalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	//Wy�lij dane;
	HAL_GPIO_WritePin(MASTER_TE_GPIO_Port, MASTER_TE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin, GPIO_PIN_RESET);
	RuntimeStats_ModbusMasterRqSendInc();
	HAL_UART_Transmit_DMA(modbus -> ModbusSerialPort, transmit_frame_buffer, 8);
	//W��cz odbieranie
	HAL_UART_Receive_DMA(modbus -> ModbusSerialPort, received_frame_buffer, received_frame_length);
	uint32_t ulNotificationValue = ulTaskNotifyTake( pdTRUE, modbus->timeout_t );
	vTaskDelay(1);
	if(ulNotificationValue){
		if(bModbusCheckCRC(received_frame_length, received_frame_buffer)){
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

ModbusState vModbusReadHoldingRegisters (ModbusHandler *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame){
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
	transmitCRC = uModbusCalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	//Wy�lij dane;
	HAL_GPIO_WritePin(MASTER_TE_GPIO_Port, MASTER_TE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin, GPIO_PIN_RESET);
	RuntimeStats_ModbusMasterRqSendInc();
	HAL_UART_Transmit_DMA(modbus -> ModbusSerialPort, transmit_frame_buffer, 8);
	//W��cz odbieranie
	HAL_UART_Receive_DMA(modbus -> ModbusSerialPort, received_frame_buffer, received_frame_length);
	uint32_t ulNotificationValue = ulTaskNotifyTake( pdTRUE, modbus->timeout_t );
	vTaskDelay(1);
	if(ulNotificationValue){
		if(bModbusCheckCRC(received_frame_length, received_frame_buffer)){
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

ModbusState vModbusReadInputRegisters (ModbusHandler *modbus, uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, uint8_t *response_frame){
	uint8_t transmit_frame_buffer[8];
	uint16_t transmitCRC;
	uint8_t received_frame_length = (registers_count * 2) + 5;
	uint8_t received_frame_buffer[received_frame_length];
	uint32_t ulNotificationValue;
	for(int i = 0; i < received_frame_length; i++){
		received_frame_buffer[i] = 0;

	}
	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x04;
	transmit_frame_buffer[2] = (uint8_t)(first_register_address >> 8);
	transmit_frame_buffer[3] = (uint8_t)first_register_address;
	transmit_frame_buffer[4] = (uint8_t)(registers_count >> 8);
	transmit_frame_buffer[5] = (uint8_t)(registers_count);
	transmitCRC = uModbusCalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	//Wy�lij dane;
	HAL_GPIO_WritePin(MASTER_TE_GPIO_Port, MASTER_TE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin, GPIO_PIN_RESET);
	RuntimeStats_ModbusMasterRqSendInc();
	HAL_UART_Transmit_IT(modbus -> ModbusSerialPort, transmit_frame_buffer, 8);
	ulNotificationValue = ulTaskNotifyTake( pdTRUE, modbus->timeout_t );
	HAL_UART_Receive_DMA(modbus -> ModbusSerialPort, received_frame_buffer, received_frame_length);
	ulNotificationValue = ulTaskNotifyTake( pdTRUE, modbus->timeout_t );
	vTaskDelay(1);
	HAL_UART_AbortTransmit(modbus -> ModbusSerialPort);
	HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin, GPIO_PIN_SET);
	if(ulNotificationValue){
		if(bModbusCheckCRC(received_frame_length, received_frame_buffer)){
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

ModbusState vForceSingleCoil (ModbusHandler *modbus, uint8_t slave_address, uint16_t coil, uint8_t coil_state){
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
	transmitCRC = uModbusCalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	//Wy�lij dane;
	HAL_GPIO_WritePin(MASTER_TE_GPIO_Port, MASTER_TE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin, GPIO_PIN_RESET);
	RuntimeStats_ModbusMasterRqSendInc();
	HAL_UART_Transmit_DMA(modbus -> ModbusSerialPort, transmit_frame_buffer, 8);
	//W��cz odbieranie
	HAL_UART_Receive_DMA(modbus -> ModbusSerialPort, received_frame_buffer, received_frame_length);
	uint32_t ulNotificationValue = ulTaskNotifyTake( pdTRUE, modbus->timeout_t );
	vTaskDelay(1);
	if(ulNotificationValue){
		if(bModbusCheckCRC(received_frame_length, received_frame_buffer)){
			for(int i = 0 ; i < 8; i++){
				if(transmit_frame_buffer[i] != received_frame_buffer[i]){
					RuntimeStats_ModbusMasterNoRespInc();
					return Modbus_TIMEOUT;
				}
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

void vModbusReceieveResponseCallback (ModbusHandler *modbus){
	vTaskNotifyGiveFromISR(modbus -> task, NULL);
	HAL_UART_AbortReceive_IT(modbus -> ModbusSerialPort);
}
