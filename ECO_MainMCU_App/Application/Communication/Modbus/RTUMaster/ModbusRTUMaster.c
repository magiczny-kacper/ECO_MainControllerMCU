/**
 * @addtogroup ModbusRTUMaster
 * @{
 */

#include "ModbusRTUMaster.h"
#include "RuntimeStats.h"

#include "FreeRTOS.h"
#include "task.h"

typedef struct{
    uint16_t rx_size;
	UART_HandleTypeDef *port; /**< UART for communication */
	TickType_t timeout; /**< Timeout given in ticks */
	TaskHandle_t task; /**< Current task to notify */
	SemaphoreHandle_t mutex; /**< Usage mutes */
}ModbusRTUMaster_t;

typedef enum {
    READ_MULTIPLE_DQ = 0x01,
    READ_MULTIPLE_DI = 0x02,
    READ_HOLDING_REG = 0x03,
    READ_INPUT_REG = 0x04,
    WRITE_SINGLE_DQ = 0x05,
    WRITE_SINGLE_OUTPUT_REG = 0x06,
    WRITE_MULTPLE_DQ = 0x0F,
    WRITE_MULTIPLE_OUTPUT_REG = 0x10
} Commands_t;

static ModbusRTUMaster_t master;

static void send_frame(uint8_t* data, uint16_t len);

static ModbusState_t perform_communication(uint8_t* tx_buf, uint8_t* rx_buf, uint16_t tx_len, uint16_t rx_len);

void ModbusRTUMaster_Init (
	UART_HandleTypeDef *port,
	uint32_t timeout
){
    master.rx_size = 0;
	master.mutex = xSemaphoreCreateMutex();
	master.port = port;
	master.timeout = timeout / portTICK_PERIOD_MS;
}


uint16_t ModbusRTU_CalculateCRC(uint8_t frame_length, uint8_t *frame)
{
	uint16_t calculatedCRC = 0xFFFF;
	uint16_t CRChigh, CRClow;

    if(frame != NULL){
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
    }

	return calculatedCRC;
}


uint8_t ModbusRTU_CheckCRC(uint8_t frame_length, uint8_t *frame)
{
	uint8_t ret = 0;
	uint16_t crc = ModbusRTU_CalculateCRC(frame_length - 2, frame);
	uint16_t frameCRC = (uint16_t)(*(frame + frame_length - 2));
	frameCRC <<= 8;
	frameCRC += *(frame + frame_length - 1);

	if(crc == frameCRC)
	{
		ret = 1;
	}

	return ret;
}

ModbusState_t ModbusRTUMaster_ReadCoilStatus(uint8_t slave_address, uint16_t first_coil, uint16_t number_of_coils, void *response)
{
	ModbusState_t ret;
	uint8_t transmit_frame_buffer[8];
	uint16_t transmitCRC;
	uint8_t coil_regs = number_of_coils / 8;

	if(number_of_coils % 8)
	{
		coil_regs++;
	}

	uint8_t received_frame_length = coil_regs + 5;
	uint8_t received_frame_buffer[received_frame_length];

	memset(transmit_frame_buffer, 0, 8);

	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x01;
	transmit_frame_buffer[2] = (uint8_t)(first_coil >> 8);
	transmit_frame_buffer[3] = (uint8_t)first_coil;
	transmit_frame_buffer[4] = (uint8_t)(number_of_coils >> 8);
	transmit_frame_buffer[5] = (uint8_t)(number_of_coils);
	transmitCRC = ModbusRTU_CalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	ret = perform_communication(transmit_frame_buffer, received_frame_buffer, 8, received_frame_length);
	if(ret == Modbus_OK)
	{
		memcpy(response, &received_frame_buffer[3], received_frame_buffer[2]);
	}
	return ret;
}

ModbusState_t ModbusRTUMaster_ReadInputStatus(uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, void *response)
{
	ModbusState_t ret;

	uint8_t transmit_frame_buffer[8];
	uint16_t transmitCRC;
	uint8_t received_frame_length = (registers_count * 2) + 5;
	uint8_t received_frame_buffer[received_frame_length];

	memset(transmit_frame_buffer, 0, 8);

	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x02;
	transmit_frame_buffer[2] = (uint8_t)(first_register_address >> 8);
	transmit_frame_buffer[3] = (uint8_t)first_register_address;
	transmit_frame_buffer[4] = (uint8_t)(registers_count >> 8);
	transmit_frame_buffer[5] = (uint8_t)(registers_count);
	transmitCRC = ModbusRTU_CalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	ret = perform_communication(transmit_frame_buffer, received_frame_buffer, 8, received_frame_length);
	if(ret == Modbus_OK)
	{
		memcpy(response, &received_frame_buffer[3], received_frame_buffer[2]);
	}
	return ret;
}

ModbusState_t ModbusRTUMaster_ReadHoldingRegisters(uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, void *response)
{
	ModbusState_t ret;

	uint8_t transmit_frame_buffer[8];
	uint16_t transmitCRC;
	uint8_t received_frame_length = (registers_count * 2) + 5;
	uint8_t received_frame_buffer[received_frame_length];

	memset(received_frame_buffer, 0, received_frame_length);

	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x03;
	transmit_frame_buffer[2] = (uint8_t)(first_register_address >> 8);
	transmit_frame_buffer[3] = (uint8_t)first_register_address;
	transmit_frame_buffer[4] = (uint8_t)(registers_count >> 8);
	transmit_frame_buffer[5] = (uint8_t)(registers_count);
	transmitCRC = ModbusRTU_CalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	ret = perform_communication(transmit_frame_buffer, received_frame_buffer, 8, received_frame_length);
	if(ret == Modbus_OK)
	{
		memcpy(response, &received_frame_buffer[3], received_frame_buffer[2]);
	}
	return ret;
}

ModbusState_t ModbusRTUMaster_ReadInputRegisters(uint8_t slave_address, uint16_t first_register_address, uint16_t registers_count, void *response)
{
	ModbusState_t ret;

	uint8_t transmit_frame_buffer[8] = {0};
	uint16_t transmitCRC;
	uint8_t received_frame_length = (registers_count * 2) + 5;
	uint8_t received_frame_buffer[received_frame_length];

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

	ret = perform_communication(transmit_frame_buffer, received_frame_buffer, 8, received_frame_length);
	if(ret == Modbus_OK)
	{
		memcpy(response, &received_frame_buffer[3], received_frame_buffer[2]);
	}
	return ret;
}

ModbusState_t ModbusRTUMaster_ForceSingleCoil(uint8_t slave_address, uint16_t coil, uint8_t coil_state, void *response)
{
	ModbusState_t ret;

	uint8_t transmit_frame_buffer[8];
	uint16_t transmitCRC;
	uint8_t received_frame_length = 0;
	uint8_t received_frame_buffer[received_frame_length];

	memset(received_frame_buffer, 0, received_frame_length);

	transmit_frame_buffer[0] = slave_address;
	transmit_frame_buffer[1] = 0x05;
	transmit_frame_buffer[2] = (uint8_t)(coil >> 8);
	transmit_frame_buffer[3] = (uint8_t)coil;
	if(coil_state)
	{
		transmit_frame_buffer[4] = 0xFF;
		transmit_frame_buffer[5] = 0;
	}
	else
	{
		transmit_frame_buffer[4] = 0xFF;
		transmit_frame_buffer[5] = 0;
	}
	transmitCRC = ModbusRTU_CalculateCRC (6, transmit_frame_buffer);
	transmit_frame_buffer[6] = (transmitCRC >> 8);
	transmit_frame_buffer[7] = (uint8_t)(transmitCRC);

	ret = perform_communication(transmit_frame_buffer, received_frame_buffer, 8, received_frame_length);
	if(ret == Modbus_OK)
	{
		memcpy(response, &received_frame_buffer[3], 2);
	}
	return ret;
}

void ModbusRTUMaster_ReceieveResponseCallback(uint16_t rx_size)
{
    master.rx_size = rx_size;
	vTaskNotifyGiveFromISR(master.task, NULL);
}

int16_t ModbusRTUMaster_TCPForward(uint8_t* inData, uint32_t len, uint8_t* outData)
{
	int16_t retval = -1;
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

	switch(txFrameBuf[1])
	{
		case 1:
		case 2:
			regsCnt = ((uint16_t)txFrameBuf[2] << 8) + txFrameBuf[3];
			rxDataExcLen += regsCnt / 8;
			if(regsCnt % 8)
			{
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

    rxLen = perform_communication(txFrameBuf, rxFrameBuf, len + 2, rxDataExcLen);
	if(rxLen > 2)
	{
        memcpy(outData, &rxFrameBuf[0], rxLen - 2);
        retval = (int16_t)rxLen - 2;
	}
	else
	{
		retval = -1;
	}

	return retval;
}

static void send_frame(uint8_t* data, uint16_t len)
{
//    HAL_GPIO_WritePin(RS485_TE_GPIO_Port, RS485_TE_Pin, GPIO_PIN_SET);
    RuntimeStats_ModbusMaster_AllRequests_Inc();
    master.task = xTaskGetCurrentTaskHandle();
    HAL_UART_Transmit(master.port, data, len, 100);
//    HAL_GPIO_WritePin(RS485_TE_GPIO_Port, RS485_TE_Pin, GPIO_PIN_RESET);
}

static bool receive_frame(uint8_t* data, uint16_t len)
{
    uint32_t ulNotificationValue = 0; 
    HAL_UARTEx_ReceiveToIdle_DMA(master.port, data, len);
    ulNotificationValue = ulTaskNotifyTake(pdTRUE, master.timeout);
    vTaskDelay(1);
//    HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin, GPIO_PIN_SET);
    return (ulNotificationValue != 0);
}

static ModbusState_t perform_communication(uint8_t* tx_buf, uint8_t* rx_buf, uint16_t tx_len, uint16_t rx_len)
{
	ModbusState_t ret;
	bool status;
	uint32_t rx_len_temp;

    xSemaphoreTake(master.mutex, portMAX_DELAY);
	send_frame(tx_buf, tx_len);
    status = receive_frame(rx_buf, rx_len);
    xSemaphoreGive(master.mutex);

    if(true == status) {
	    status = ModbusRTU_CheckCRC(rx_len_temp, rx_buf);
		if(1 == status)
		{
			if(((rx_buf[0] & 0x7F) == tx_buf[0]) &&
				(rx_buf[1] == 0x04))
			{
				if(rx_buf[0] & 0x80)
				{
					ret = rx_buf[2];
				}
				else
				{
					RuntimeStats_ModbusMaster_OkResponses_Inc();
					ret = Modbus_OK;
				}
			}
			else
			{
				RuntimeStats_ModbusMaster_WrongResponses_Inc();
				ret = Modbus_CRC_ERR;
			}
		}
		else
		{
			RuntimeStats_ModbusMaster_WrongResponses_Inc();
			ret = Modbus_CRC_ERR;
		}
	}else{
		RuntimeStats_ModbusMaster_NoResponses_Inc();
		ret = Modbus_TIMEOUT;
	}

	return ret;
}
/**
 * @}
 */
