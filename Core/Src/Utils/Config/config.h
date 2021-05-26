/*
 * config.h
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#ifndef SRC_CONFIGEEPROM_CONFIG_H_
#define SRC_CONFIGEEPROM_CONFIG_H_
/**
 * @defgroup config
 *
 */
#include "stm32f4xx_hal.h"

#define IP_LEN 4
#define MAC_LEN 6
#define NTP_URL_LEN 30

typedef enum{
	CONF_ERR_NOCONF = -5,
	CONF_ERR_CRC = -4,
	CONF_ERR_BUSY = -3,
	CONF_ERR_NULL = -2,
	CONF_ERR_TIMEOUT = -1,
	CONF_OK = 0,
} ConfigStatus_t;

typedef enum{
	CONF_STAT_ERROR = -3,
	CONF_STAT_NOCONF = -2,
	CONF_STAT_INIT = -1,
	CONF_STAT_LOADED = 0,
} ConfigLoadStatus_t;

enum{
	CWUHEATERPOWER = 0,
	COHEATERPOWER,
	NETMAXPOWER,
	COUNTERADDRESS,
	TEMPADDRESS,
	ACCPOWERCOEFF,
	HEATERPOWERCOEFF,
	COHITEMP,
	COLOTEMP,
	CEUHITEMP,
	CWULOTEMP,
	EXPORTEDPOWER,
	IMPORTEDPOWER,
	ACCUMULATEDPOWER,
	IPADDRESS,
	SUBNETMASK,
	GATEWAY,
	MAC,
	PARAM_NUMS
};

typedef struct __attribute__((packed)){
	float CWU_heater_power;			//0
	float CO_heater_power;			//4
	float net_max_power;			//8

	uint8_t counter_address;		//12
	uint8_t temperature_address;	//13
	int8_t acumulated_power_coeff;	//14
	int8_t heater_power_coeff;		//15
	int8_t CO_hi_temp;				//16
	int8_t CO_lo_temp;				//17
	int8_t CWU_hi_temp;				//18
	int8_t CWU_lo_temp;				//19

	float exported_power;			//20
	float imported_power;			//24
	float acumulated_power;			//28
} RegulationConfig_t;

typedef struct __attribute__((packed)){
	uint8_t ipAddress[IP_LEN];			//32
	uint8_t subnetMask[IP_LEN];			//36
	uint8_t gatewayAddress[IP_LEN];		//40
	uint8_t macAddress[MAC_LEN];		//44
	char ntpUrl[NTP_URL_LEN];			//50
} EthernetConfig_t;

typedef struct  __attribute__((packed)){
	RegulationConfig_t RegulationConfig;
	EthernetConfig_t EthernetConfig;
	uint32_t dummy;					//80
	uint32_t crc;					//84
} ConfigStruct_t;

#define REG_CONFIG_BYTES_LEN sizeof(RegulationConfig_t)
#define ETH_CONFIG_BYTES_LEN sizeof(EthernetConfig_t)
#define CRC_BYTES_LEN 8
#define CONFIG_BYTES_LEN (REG_CONFIG_BYTES_LEN + ETH_CONFIG_BYTES_LEN + CRC_BYTES_LEN)
#define MAGIC_WORD 0xAA55

ConfigStatus_t Config_Init(I2C_HandleTypeDef* iic);

void Config_ChangeValue(uint32_t param, void* value);

ConfigStatus_t Config_Save (void);

ConfigLoadStatus_t Config_GetConfigStatus(void);

ConfigStatus_t Config_GetConfig (ConfigStruct_t* config);

ConfigStatus_t Config_GetEthernetConfig (EthernetConfig_t* config);

ConfigStatus_t Config_GetRegConfig (RegulationConfig_t* config);

ConfigStatus_t Config_Copy (ConfigStruct_t* dest, ConfigStruct_t* src);

/**
 * @}
 */
#endif /* SRC_CONFIGEEPROM_CONFIG_H_ */
