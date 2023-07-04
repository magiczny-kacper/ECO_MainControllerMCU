#ifndef CONFIGMGR_H_
#define CONFIGMGR_H_
/**
 * @defgroup config
 *
 */
#include "i2c.h"

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

typedef enum{
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
} ConfigParams_t;

typedef struct {
	float CWU_heater_power;
	float CO_heater_power;
	float net_max_power;

	float exported_power;
	float imported_power;
	float acumulated_power;

	uint8_t counter_address;
	uint8_t temperature_address;
	int8_t acumulated_power_coeff;
	int8_t heater_power_coeff;
	int8_t CO_hi_temp;
	int8_t CO_lo_temp;
	int8_t CWU_hi_temp;
	int8_t CWU_lo_temp;
} RegulationConfig_t;

typedef struct {
    uint8_t use_dhcp;
	uint8_t ipAddress[IP_LEN];
	uint8_t subnetMask[IP_LEN];
	uint8_t gatewayAddress[IP_LEN];
	uint8_t macAddress[MAC_LEN];
	char ntpUrl[NTP_URL_LEN];
} EthernetConfig_t;

typedef struct {
    uint8_t config_revision;
	RegulationConfig_t RegulationConfig;
	EthernetConfig_t EthernetConfig;
	uint32_t dummy;
	uint32_t crc;
} ConfigStruct_t;

#define REG_CONFIG_BYTES_LEN (sizeof(RegulationConfig_t))

#define ETH_CONFIG_BYTES_LEN (sizeof(EthernetConfig_t))
#define CRC_BYTES_LEN (2 * sizeof(uint32_t))
#define CONFIG_BYTES_LEN sizeof(ConfigStruct_t)
#define MAGIC_WORD 0xAA55

ConfigStatus_t ConfigMgr_Init(I2C_HandleTypeDef* i2c_handle, CRC_HandleTypeDef* crc_handle);

void ConfigMgr_ChangeValue(ConfigParams_t param, void* value);

ConfigStatus_t ConfigMgr_Save (void);

ConfigLoadStatus_t ConfigMgr_GetConfigStatus(void);

ConfigStatus_t ConfigMgr_GetConfig (ConfigStruct_t* config);

EthernetConfig_t* ConfigMgr_GetEthernetConfig (void);

ConfigStatus_t ConfigMgr_GetRegConfig (RegulationConfig_t* config);

ConfigStatus_t ConfigMgr_Copy (ConfigStruct_t* dest, const ConfigStruct_t* src);

/**
 * @}
 */
#endif /* CONFIGMGR_H_ */
