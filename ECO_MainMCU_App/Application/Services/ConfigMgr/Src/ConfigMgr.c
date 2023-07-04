/**
 * @addtogroup config
 */
#include <string.h>
#include "ConfigMgr.h"
#include "EventLog.h"
#include "EEPROM.h"

#define CCITT_POLY 0x1081
#define CONFIG_REVISION 1

#define member_size(type, member) sizeof(((type *)0)->member)

CRC_HandleTypeDef* crc;

static ConfigStruct_t configuration;
static ConfigLoadStatus_t configuartionStatus;

static const ConfigStruct_t defaultConfig = {
    .config_revision = CONFIG_REVISION,
    .EthernetConfig.use_dhcp = 0,
    .EthernetConfig.ipAddress = {192,168,1,192},
    .EthernetConfig.subnetMask = {255,255,255,0},
    .EthernetConfig.gatewayAddress = {192,168,1,1},
    .EthernetConfig.macAddress = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
    .RegulationConfig.CWU_heater_power = 0.0f,
    .RegulationConfig.CO_heater_power = 0.0f,
    .RegulationConfig.net_max_power = 0.0f,
    .RegulationConfig.counter_address = 1,
    .RegulationConfig.temperature_address = 2,
    .RegulationConfig.acumulated_power_coeff = 0,
    .RegulationConfig.heater_power_coeff = 0,
    .RegulationConfig.CO_hi_temp = 0,
    .RegulationConfig.CO_lo_temp = 0,
    .RegulationConfig.CWU_hi_temp = 0,
    .RegulationConfig.CWU_lo_temp = 0,
    .RegulationConfig.exported_power = 0.0f,
    .RegulationConfig.imported_power = 0.0f,
    .RegulationConfig.acumulated_power = 0.0f,
    .dummy = MAGIC_WORD
};

static uint32_t ConfigMgr_CalculateCRC(void);

static ConfigStatus_t ConfigMgr_CheckData(void);

ConfigStatus_t ConfigMgr_Init(I2C_HandleTypeDef* i2c_handle, CRC_HandleTypeDef* crc_handle){
    int32_t retval = 0;

    configuartionStatus = CONF_STAT_LOADED;

    crc = crc_handle;

    retval = EE_Init(i2c_handle);
    retval |= EE_Read(&configuration, 0, CONFIG_BYTES_LEN);

    if(retval == EE_OK){
        retval = ConfigMgr_CheckData();
    }

    if(retval == CONF_OK){
        configuartionStatus = CONF_STAT_LOADED;
    }else{
        ConfigMgr_Copy(&configuration, &defaultConfig);
        configuration.crc = ConfigMgr_CalculateCRC();
        if((retval == CONF_ERR_NOCONF) || (retval == CONF_ERR_CRC)){
            retval = ConfigMgr_Save();
            configuartionStatus = CONF_STAT_LOADED;
        }else{
            configuartionStatus = CONF_STAT_ERROR;
        }
    }

    return retval;
}

void ConfigMgr_ChangeValue(ConfigParams_t param, void* value){
    if(value != NULL && param < PARAM_NUMS){
        switch(param){
            case CWUHEATERPOWER:
                memcpy(&configuration.RegulationConfig.CWU_heater_power, value, member_size(RegulationConfig_t, CWU_heater_power));
                break;

            case COHEATERPOWER:
                memcpy(&configuration.RegulationConfig.CO_heater_power, value, member_size(RegulationConfig_t, CO_heater_power));
                break;

            case NETMAXPOWER:
                memcpy(&configuration.RegulationConfig.net_max_power, value, member_size(RegulationConfig_t, net_max_power));
                break;

            case COUNTERADDRESS:
                memcpy(&configuration.RegulationConfig.counter_address, value, member_size(RegulationConfig_t, counter_address));
                break;

            case TEMPADDRESS:
                memcpy(&configuration.RegulationConfig.temperature_address, value, member_size(RegulationConfig_t, temperature_address));
                break;

            case ACCPOWERCOEFF:
                memcpy(&configuration.RegulationConfig.acumulated_power_coeff, value, member_size(RegulationConfig_t, acumulated_power_coeff));
                break;

            case HEATERPOWERCOEFF:
                memcpy(&configuration.RegulationConfig.heater_power_coeff, value, member_size(RegulationConfig_t, heater_power_coeff));
                break;

            case COHITEMP:
                memcpy(&configuration.RegulationConfig.CO_hi_temp, value, member_size(RegulationConfig_t, CO_hi_temp));
                break;

            case COLOTEMP:
                memcpy(&configuration.RegulationConfig.CO_lo_temp, value, member_size(RegulationConfig_t, CO_lo_temp));
                break;

            case CEUHITEMP:
                memcpy(&configuration.RegulationConfig.CWU_hi_temp, value, member_size(RegulationConfig_t, CWU_hi_temp));
                break;

            case CWULOTEMP:
                memcpy(&configuration.RegulationConfig.CWU_lo_temp, value, member_size(RegulationConfig_t, CWU_lo_temp));
                break;

            case EXPORTEDPOWER:
                memcpy(&configuration.RegulationConfig.exported_power, value, member_size(RegulationConfig_t, exported_power));
                break;

            case IMPORTEDPOWER:
                memcpy(&configuration.RegulationConfig.imported_power, value, member_size(RegulationConfig_t, imported_power));
                break;

            case ACCUMULATEDPOWER:
                memcpy(&configuration.RegulationConfig.acumulated_power, value, member_size(RegulationConfig_t, acumulated_power));
                break;

            case IPADDRESS:
                memcpy(configuration.EthernetConfig.ipAddress, value, member_size(EthernetConfig_t, ipAddress));
                break;

            case SUBNETMASK:
                memcpy(configuration.EthernetConfig.subnetMask, value, member_size(EthernetConfig_t, subnetMask));
                break;

            case GATEWAY:
                memcpy(configuration.EthernetConfig.gatewayAddress, value, member_size(EthernetConfig_t, gatewayAddress));
                break;

            case MAC:
                memcpy(configuration.EthernetConfig.macAddress, value, member_size(EthernetConfig_t, macAddress));
                break;

            default:
                break;
        }
        configuration.dummy = MAGIC_WORD;
        configuration.crc = ConfigMgr_CalculateCRC();
    }
}

static uint32_t ConfigMgr_CalculateCRC (void){
    HAL_CRCEx_Polynomial_Set(crc, CCITT_POLY, CRC_POLYLENGTH_16B);
    return HAL_CRC_Calculate(crc, (uint32_t*)&configuration, (CONFIG_BYTES_LEN - 2)/4);
}

static ConfigStatus_t ConfigMgr_CheckData (void){
    ConfigStatus_t retval;
    uint32_t readCRC, actualCRC;

    readCRC = configuration.crc;
    actualCRC = ConfigMgr_CalculateCRC();

    if(readCRC != actualCRC){
        if(configuration.dummy != MAGIC_WORD){
            retval = CONF_ERR_NOCONF;
        }
        retval = CONF_ERR_CRC;
    }else{
        retval = CONF_OK;
    }
    return retval;
}

ConfigStatus_t ConfigMgr_Save (void){
    ConfigStatus_t retval;
    DataLog_LogEvent(EV_CFG_CHANGE, NULL, 0);
    retval = (ConfigStatus_t)EE_Write(&configuration, 0, CONFIG_BYTES_LEN);
    return retval;
}

ConfigLoadStatus_t ConfigMgr_GetConfigStatus(void){
    return configuartionStatus;
}

ConfigStatus_t ConfigMgr_GetConfig (ConfigStruct_t* config){
    if(config == NULL) return CONF_ERR_NULL;

    if(configuartionStatus == CONF_STAT_LOADED){
        memcpy(config, &configuration, CONFIG_BYTES_LEN);
        return CONF_OK;
    }
    return CONF_ERR_NOCONF;
}

EthernetConfig_t* ConfigMgr_GetEthernetConfig (void)
{
    if(configuartionStatus == CONF_STAT_LOADED){
        return &configuration.EthernetConfig;
    }
    return NULL;
}

ConfigStatus_t ConfigMgr_GetRegConfig (RegulationConfig_t* config){
    if(config == NULL) return CONF_ERR_NULL;

    if(configuartionStatus == CONF_STAT_LOADED){
        memcpy(config, &configuration.RegulationConfig, REG_CONFIG_BYTES_LEN);
        return CONF_OK;
    }
    return CONF_ERR_NOCONF;
}

ConfigStatus_t ConfigMgr_Copy (ConfigStruct_t* dest, const ConfigStruct_t* src){
    if((dest == NULL) || (src == NULL)) return CONF_ERR_NULL;
    memcpy(dest, src, CONFIG_BYTES_LEN);
    return CONF_OK;
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

/**
 * @}
 */
