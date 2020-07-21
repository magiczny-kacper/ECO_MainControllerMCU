/*
 * config.c
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#include <string.h>
#include "config.h"
#include "drv_EEPROM.h"

extern CRC_HandleTypeDef hcrc;

static ConfigStruct_t configuration;
static ConfigLoadStatus_t configuartionStatus;

static uint32_t Config_CalculateCRC (void);

static CONFStatus_t Config_CheckData (void);

CONFStatus_t Config_Init(I2C_HandleTypeDef* iic){
	CONFStatus_t retval = 0;
	uint32_t crc;

	configuartionStatus = CONF_STAT_INIT;

	retval |= EE_Init(iic);
	retval |= EE_Read(&configuration, 0, CONFIG_BYTES_LEN);

	retval |= Config_CheckData();

	if(retval == CONF_OK) configuartionStatus = CONF_STAT_LOADED;
	else if(retval == CONF_ERR_NOCONF) configuartionStatus = CONF_STAT_NOCONF;
	else configuartionStatus = CONF_STAT_ERROR;
	return retval;
}

void Config_ChangeValue(uint32_t param, void* value){
	if(value != NULL && param < PARAM_NUMS){
		switch(param){
			case CWUHEATERPOWER:
				memcpy(&configuration.RegulationConfig.CWU_heater_power, value, 4);
				break;

			case COHEATERPOWER:
				memcpy(&configuration.RegulationConfig.CO_heater_power, value, 4);
				break;

			case NETMAXPOWER:
				memcpy(&configuration.RegulationConfig.net_max_power, value, 4);
				break;

			case COUNTERADDRESS:
				memcpy(&configuration.RegulationConfig.counter_address, value, 1);
				break;

			case TEMPADDRESS:
				memcpy(&configuration.RegulationConfig.temperature_address, value, 1);
				break;

			case ACCPOWERCOEFF:
				memcpy(&configuration.RegulationConfig.acumulated_power_coeff, value, 1);
				break;

			case HEATERPOWERCOEFF:
				memcpy(&configuration.RegulationConfig.heater_power_coeff, value, 1);
				break;

			case COHITEMP:
				memcpy(&configuration.RegulationConfig.CO_hi_temp, value, 1);
				break;

			case COLOTEMP:
				memcpy(&configuration.RegulationConfig.CO_lo_temp, value, 1);
				break;

			case CEUHITEMP:
				memcpy(&configuration.RegulationConfig.CWU_hi_temp, value, 1);
				break;

			case CWULOTEMP:
				memcpy(&configuration.RegulationConfig.CWU_lo_temp, value, 1);
				break;

			case EXPORTEDPOWER:
				memcpy(&configuration.RegulationConfig.exported_power, value, 4);
				break;

			case IMPORTEDPOWER:
				memcpy(&configuration.RegulationConfig.imported_power, value, 4);
				break;

			case ACCUMULATEDPOWER:
				memcpy(&configuration.RegulationConfig.acumulated_power, value, 4);
				break;

			case IPADDRESS:
				memcpy(&configuration.EthernetConfig.ipAddress[0], value, 4);
				break;

			case SUBNETMASK:
				memcpy(&configuration.EthernetConfig.subnetMask[0], value, 1);
				break;

			case GATEWAY:
				memcpy(&configuration.EthernetConfig.gatewayAddress[0], value, 1);
				break;

			case MAC:
				memcpy(&configuration.EthernetConfig.macAddress[0], value, 1);
				break;
		}
		configuration.dummy = MAGIC_WORD;
		configuration.crc = Config_CalculateCRC();
	}
}

static uint32_t Config_CalculateCRC (void){
	return HAL_CRC_Calculate(&hcrc, (uint32_t*)&configuration, (CONFIG_BYTES_LEN - 2)/4);
}

static CONFStatus_t Config_CheckData (void){
	uint32_t readCRC, actualCRC;

	readCRC = configuration.crc;
	actualCRC = Config_CalculateCRC();

	if(readCRC != actualCRC){
		if(configuration.dummy != MAGIC_WORD){
			return CONF_ERR_NOCONF;
		}
		return CONF_ERR_CRC;
	}else{
		return CONF_OK;
	}
}

CONFStatus_t Config_Save (void){
	CONFStatus_t retval;
	retval = (CONFStatus_t)EE_Write(&configuration, 0, CONFIG_BYTES_LEN);
	return retval;
}

ConfigLoadStatus_t Config_GetConfigStatus(void){
	return configuartionStatus;
}

CONFStatus_t Config_GetConfig (ConfigStruct_t* config){
	if(config == NULL) return CONF_ERR_NULL;

	if(configuartionStatus == CONF_STAT_LOADED){
		memcpy(config, &configuration, CONFIG_BYTES_LEN);
		return CONF_OK;
	}
	return CONF_ERR_NOCONF;
}

CONFStatus_t Config_GetEthernetConfig (EthernetConfig_t* config){
	if(config == NULL) return CONF_ERR_NULL;

	if(configuartionStatus == CONF_STAT_LOADED){
		memcpy(config, &configuration.EthernetConfig, ETH_CONFIG_BYTES_LEN);
		return CONF_OK;
	}
	return CONF_ERR_NOCONF;
}

CONFStatus_t Config_GetRegConfig (RegulationConfig_t* config){
	if(config == NULL) return CONF_ERR_NULL;

	if(configuartionStatus == CONF_STAT_LOADED){
		memcpy(config, &configuration.RegulationConfig, REG_CONFIG_BYTES_LEN);
		return CONF_OK;
	}
	return CONF_ERR_NOCONF;
}

/*void ConfigSaveTmr(void const * argument)
{
	uint8_t* data_pointer;
	uint8_t buffor_to_send_size;
	HAL_StatusTypeDef status = HAL_BUSY;
	char buffor_to_send[40];
	if(eeprom_error == 0){
		data_pointer = (uint8_t*)(&(counter.eeprom_data));

		while(status == HAL_BUSY){
			status = HAL_I2C_Mem_Write(&hi2c1, 0xA0, 0, 1, data_pointer, 16, 10);
		};

		while(status == HAL_BUSY){
			status = HAL_I2C_Mem_Write(&hi2c1, 0xA0, 16, 1, data_pointer + 16, 4, 10);
		};

		if(status == HAL_OK){
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_SETTINGS_SAVED]);
		}else{
			buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_SETTINGS_NOT_SAVED]);
		}

		for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
			buffor_to_send[i] = 0xFF;
		}
		buffor_to_send_size += 3;

		HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
	}*/
  /* USER CODE END EEPROM_Save */
