#include <string.h>

#include "EEPROM.h"
#include "ResourceGuard.h"

I2C_HandleTypeDef* ee_iic;

static HAL_StatusTypeDef EE_I2C_Read (uint8_t addr, void* dataOut, size_t len);

EE_Status_t EE_Init (I2C_HandleTypeDef* hi2c)
{
	EE_Status_t ret;
	if(hi2c != NULL)
	{
		ee_iic = hi2c;
		ret = EE_OK;
	}
	else
	{
		ret = EE_ERR_NULL;
	}
	return ret;
}

EE_Status_t EE_Read (void* data, uint8_t addr, size_t len)
{
	EE_Status_t ret = EE_OK;
	HAL_StatusTypeDef i2cStatus;

	if((data == NULL) || (len <= 0))
	{
		ret = EE_ERR_NULL;
	}

	if(ret == EE_OK)
	{
        RG_I2C_Acquire();
        while(len)
        {
            uint8_t read_len = 0;
            const uint32_t end_addr = addr + len;
            const uint8_t page_number = ((addr / EE_BLOCK_SIZE) << 1) & 0b1110;
            if ((len > EE_BLOCK_SIZE) || ((addr / EE_BLOCK_SIZE) != (end_addr / EE_BLOCK_SIZE)))
            {
                if (addr % EE_BLOCK_SIZE != 0)
                {
                    read_len = EE_BLOCK_SIZE - (addr % EE_BLOCK_SIZE);
                }
                else
                {
                    read_len = EE_BLOCK_SIZE;
                }
            }
            else
            {
                read_len = len;
            }
            i2cStatus = HAL_I2C_Mem_Read(ee_iic, EE_ADDR + page_number, addr & 0xFF, 1, data, read_len, 10);
            if (i2cStatus == HAL_OK)
            {
                addr += read_len;
                len -= read_len;
            }
            else
            {
                if(i2cStatus == HAL_ERROR)
                {
                    ret = EE_ERR_TIMEOUT;
                }
                else if(i2cStatus == HAL_BUSY)
                {
                    ret = EE_ERR_BUSY;
                }
                break;
            }
        }
        RG_I2C_Release();
	}

	return ret;
}

EE_Status_t EE_Write (void* data, uint32_t addr, size_t len)
{
	EE_Status_t retval = EE_ERR_NULL;
	HAL_StatusTypeDef i2cStatus;
	uint8_t* dataBuf;
    uint8_t i2cBuff[EE_PAGE_SIZE + 1] = {0};
	uint8_t write_timeout = 0;

	if((data != NULL) && (len > 0) && (len <= EE_SIZE))
	{
		dataBuf = (uint8_t*)data;

		RG_I2C_Acquire();

        while(len)
        {
            uint8_t write_len = 0;
            const uint8_t page_number = ((addr / EE_BLOCK_SIZE) << 1) & 0b1110;
            if ((len > EE_PAGE_SIZE) || (((addr + len) % 16) != 0))
            {
                if (addr % EE_PAGE_SIZE != 0)
                {
                    write_len = EE_PAGE_SIZE - (addr % EE_PAGE_SIZE);
                }
                else
                {
                    write_len = EE_PAGE_SIZE;
                }
            }
            else
            {
                write_len = len;
            }

            i2cBuff[0] = (uint8_t)(addr & 0xFF);
            memcpy(&i2cBuff[1], dataBuf, write_len);
            i2cStatus = HAL_I2C_Master_Transmit(ee_iic, EE_ADDR + page_number, i2cBuff, write_len + 1, 10);
            if(i2cStatus == HAL_OK)
            {
                addr += write_len;
                len -= write_len;
                while(
                    (HAL_I2C_IsDeviceReady(ee_iic, EE_ADDR, 10, 10) != HAL_OK) &&
                    (write_timeout < 10))
                {
                    write_timeout++;
                    vTaskDelay(2);
                }
                if(write_timeout == 10)
                {
                    retval = EE_ERR_TIMEOUT;
                }
            }
            else
            {
                if(i2cStatus == HAL_ERROR)
                {
                    retval = EE_ERR_TIMEOUT;
                }
                else if(i2cStatus == HAL_BUSY)
                {
                    retval = EE_ERR_BUSY;
                }
                break;
            }
            
        }
		RG_I2C_Release();
	}
	else
	{
		retval = EE_ERR_TIMEOUT;
	}

	return retval;
}

EE_Status_t EE_Erase (void){
	uint32_t i;
	int32_t retval = 0;
	uint8_t blankData[16];
	memset(blankData, 255, 16);

	for(i = 0; i < EE_SIZE; i += 16){
		retval |= EE_Write(blankData, i, 16);
	}
	return retval;
}

static HAL_StatusTypeDef EE_I2C_Read (uint8_t addr, void* dataOut, size_t len){
	HAL_StatusTypeDef retval;
	RG_I2C_Acquire();
	retval = HAL_I2C_Mem_Read(ee_iic, EE_ADDR, addr, 1, (uint8_t*)dataOut, len, EE_BYTE_WRITE_TIME * (len + 2));
	RG_I2C_Release();
	return retval;
}
