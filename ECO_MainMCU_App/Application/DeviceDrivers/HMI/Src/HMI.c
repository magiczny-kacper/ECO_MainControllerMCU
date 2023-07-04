#include "HMI.h"

typedef enum{
    PHASE1 = 1,
    PHASE2,
    PHASE3
} phases;

typedef enum _nextion_command{
    NEX_WAKE = 0,
    NEX_SENDME,
    NEX_YEAR,
    NEX_MONTH,
    NEX_DAY,
    NEX_HOUR,
    NEX_MINUTE,
    NEX_SECONDS,
    NEX_CO_HIGH_TEMP,
    NEX_CO_LOW_TEMP,
    NEX_CO_HEATER_POWER,
    NEX_CWU_HIGH_TEMP,
    NEX_CWU_LOW_TEMP,
    NEX_CWU_HEATER_POWER,
    NEX_ACC_COEFF,
    NEX_HEATER_COEFF,
    NEX_CWORD,
    NEX_SETTINGS_SAVED,
    NEX_SETTINGS_NOT_SAVED,
    NEX_COUNTER_PRESENCE,
    NEX_TSENS_PRESENCE,
    NEX_MAIN_SW,
    NEX_CWU_PIC,
    NEX_CO_PIC,
    NEX_PHASE_VOLTAGE,
    NEX_PHASE_CURRENT,
    NEX_PHASE_POWER,
    NEX_PHASE_PRG_BAR,
    NEX_CO_HEATER_PHASE_DUTY,
    NEX_CWU_HEATER_PHASE_DUTY,
    NEX_CWU_TEMP,
    NEX_CO_TEMP,
    NEX_SETTINGS_UNLOCK,
    NEX_SETTINGS_LOCK,
    NEX_COMMANDS_COUNT
} nextion_command;

const static char* nextion_commands[NEX_COMMANDS_COUNT] = {
    "sleep=0",
    "sendme",
    "rtc0=%d",
    "rtc1=%d",
    "rtc2=%d",
    "rtc3=%d",
    "rtc4=%d",
    "rtc5=%d",
    "sett.COhi.txt=\"%d\"",
    "sett.COlo.txt=\"%d\"",
    "sett.COhp.txt=\"%d\"",
    "sett.CWUhi.txt=\"%d\"",
    "sett.CWUlo.txt=\"%d\"",
    "sett.CWUhp.txt=\"%d\"",
    "sett.apcoeff.txt=\"%d\"",
    "sett.hpcoeff.txt=\"%d\"",
    "sett.cword.val=%d",
    "sett.saved.txt=\"Zapisano\"",
    "sett.saved.txt=\"Blad\"",
    "pow.cntrpres.val=%d",
    "pow.temppres.val=%d",
    "pow.mainsw.val=%d",
    "pow.baniakCWU.pic=%d",
    "pow.baniakCO.pic=%d",
    "pow.L%dV.txt=\"%d.%d\"",
    "pow.L%dC.txt=\"%d.%d\"",
    "pow.L%dP.txt=\"%d.%d\"",
    "pow.L%dPRG.val=%d",
    "pow.COHL%d.val=%d",
    "pow.CWUHL%d.val=%d",
    "pow.CWUt%d.val=%d",
    "pow.COt%d.val=%d",
    "pow.lock.val=0",
    "pow.lock.val=1"
};

typedef struct{
    uint8_t display;
    uint8_t last_display;
    uint8_t sleep;
    uint8_t ready;
    uint32_t param;
    uint8_t value[4];
}nextion;

extern osThreadId DiplaysHandle;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
extern RTC_HandleTypeDef hrtc;

#define NEXTION_SMALL_UART &huart1
#define NEXTION_BIG_UART &huart6

RTC_TimeTypeDef currTime;
RTC_DateTypeDef currDate;

UARTDMA_HandleTypeDef nextion_big_comm;
UARTDMA_HandleTypeDef nextion_small_comm;

nextion nextion_small;
nextion nextion_big;

RegTaskData_t RegulationTaskData;

uint8_t command_correct[NEX_COMMANDS_COUNT];
uint8_t current_command;

static void nextion_UpdateTime(UART_HandleTypeDef* uart);
static void nextion_SendData(UART_HandleTypeDef* uart, nextion_command cmd, int32_t value1, int32_t value2, int32_t value3);
static void nextion_SendConfig(UART_HandleTypeDef* uart);
void NextionBig_DataRcv (void);
void NextionSmall_DataRcv (void);

static void nextion_SendData(UART_HandleTypeDef* uart, nextion_command cmd, int32_t value1, int32_t value2, int32_t value3){
    char buffor_to_send[40];
    uint8_t buffor_to_send_size;
    current_command = cmd;
    if(cmd < NEX_COMMANDS_COUNT && cmd > -1){
        if(cmd == NEX_WAKE || cmd == NEX_SENDME || cmd == NEX_SETTINGS_SAVED || cmd == NEX_SETTINGS_NOT_SAVED
            || cmd == NEX_SETTINGS_LOCK || cmd == NEX_SETTINGS_UNLOCK){
            buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[cmd]);
        }else if(cmd == NEX_PHASE_VOLTAGE ||
            cmd == NEX_PHASE_CURRENT ||
            cmd == NEX_PHASE_POWER){
            buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[cmd], value1, value2, value3);
        }else if(cmd == NEX_PHASE_PRG_BAR ||
            cmd == NEX_CO_HEATER_PHASE_DUTY ||
            cmd == NEX_CWU_HEATER_PHASE_DUTY ||
            cmd == NEX_CWU_TEMP || cmd == NEX_CO_TEMP){
            buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[cmd], value1, value2);
        }else{
            buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[cmd], value1);
        }
        for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
            buffor_to_send[i] = 0xFF;
        }
        buffor_to_send_size += 3;
        HAL_UART_Transmit(uart, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
        vTaskDelay(5);
    }
}

static void nextion_UpdateTime(UART_HandleTypeDef* uart){
    HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BIN);

    current_command = NEX_YEAR;
    nextion_SendData(uart, NEX_YEAR, currDate.Year + 2000, 0, 0);
    current_command = NEX_MONTH;
    nextion_SendData(uart, NEX_MONTH, currDate.Month, 0, 0);
    current_command = NEX_DAY;
    nextion_SendData(uart, NEX_DAY, currDate.Date, 0, 0);
    current_command = NEX_HOUR;
    nextion_SendData(uart, NEX_HOUR, currTime.Hours, 0, 0);
    current_command = NEX_MINUTE;
    nextion_SendData(uart, NEX_MINUTE, currTime.Minutes, 0, 0);
    current_command = NEX_SECONDS;
    nextion_SendData(uart, NEX_SECONDS, currTime.Seconds, 0, 0);
}

static void nextion_SendConfig(UART_HandleTypeDef* uart){
    nextion_SendData(uart, NEX_CO_HIGH_TEMP, RegulationTaskData.parameters.CO_hi_temp, 0, 0);
    nextion_SendData(uart, NEX_CO_LOW_TEMP, RegulationTaskData.parameters.CO_lo_temp, 0, 0);
    nextion_SendData(uart, NEX_CO_HEATER_POWER, (int32_t)RegulationTaskData.parameters.CO_heater_power, 0, 0);
    nextion_SendData(uart, NEX_CWU_HIGH_TEMP, RegulationTaskData.parameters.CWU_hi_temp, 0, 0);
    nextion_SendData(uart, NEX_CWU_LOW_TEMP, RegulationTaskData.parameters.CWU_lo_temp, 0, 0);
    nextion_SendData(uart, NEX_CWU_HEATER_POWER, (int32_t)RegulationTaskData.parameters.CWU_heater_power, 0, 0);
    nextion_SendData(uart, NEX_ACC_COEFF, RegulationTaskData.parameters.acumulated_power_coeff, 0, 0);
    nextion_SendData(uart, NEX_HEATER_COEFF, RegulationTaskData.parameters.heater_power_coeff, 0, 0);
}

void DisplayTask(void const * argument)
{
  /* USER CODE BEGIN UpdateLCD */
    uint32_t ulNotificationValue;
    uint32_t param = 0;
    uint8_t* valPtr = NULL;
    uint32_t rest;
    //uint8_t licz;

    EthernetConfig_t configuration;
    
    nextion_small.display = 30;
    nextion_small.last_display = 30;
    nextion_big.display = 30;
    nextion_big.last_display = 30;

    UARTDMA_Init(&nextion_small_comm, NEXTION_SMALL_UART);
    UARTDMA_Init(&nextion_big_comm, NEXTION_BIG_UART);

    nextion_big_comm.callback = NextionBig_DataRcv;
    nextion_small_comm.callback = NextionSmall_DataRcv;

    Config_GetEthernetConfig(&configuration);
    RegulationTask_GetData(&RegulationTaskData);

    vTaskDelay(1000);

    HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BIN);

    nextion_SendData(NEXTION_SMALL_UART, NEX_WAKE, 0, 0, 0);
    vTaskDelay(1);

    nextion_SendData(NEXTION_SMALL_UART, NEX_SENDME, 0, 0, 0);
    vTaskDelay(1);
    ulNotificationValue = ulTaskNotifyTake(pdTRUE, 1000);

    nextion_SendData(NEXTION_BIG_UART, NEX_SENDME, 0, 0, 0);
    vTaskDelay(1);
    ulNotificationValue = ulTaskNotifyTake(pdTRUE, 1000);

    if(nextion_big.display != 30){
        nextion_UpdateTime(NEXTION_BIG_UART);
        nextion_SendConfig(NEXTION_BIG_UART);
    }

    if(nextion_small.display != 30){
        nextion_SendConfig(NEXTION_SMALL_UART);
    }

    // Pętla nieskończona
    for(;;){
        RegulationTask_GetData(&RegulationTaskData);
        if(nextion_big.display != nextion_big.last_display){
            if(nextion_big.display == 2){
                nextion_UpdateTime(NEXTION_BIG_UART);
                if(nextion_small.display != 30){
                    nextion_SendData(NEXTION_SMALL_UART, NEX_SETTINGS_LOCK, 0, 0, 0);
                }
            }else{
                if(nextion_small.display != 30){
                    nextion_SendData(NEXTION_SMALL_UART, NEX_SETTINGS_UNLOCK, 0, 0, 0);
                }
            }
            nextion_big.last_display = nextion_big.display;
        }

        if(nextion_small.display != nextion_small.last_display){
            if(nextion_small.display == 1){
                if(nextion_big.display != 30){
                    nextion_SendData(NEXTION_BIG_UART, NEX_SETTINGS_LOCK, 0, 0, 0);
                }
            }else{
                if(nextion_big.display != 30){
                    nextion_SendData(NEXTION_BIG_UART, NEX_SETTINGS_UNLOCK, 0, 0, 0);
                }
            }
        }
        if(nextion_big.display == 1){
            nextion_SendData(NEXTION_BIG_UART, NEX_COUNTER_PRESENCE, RegulationTaskData.counter.counter_present, 0, 0);
            nextion_SendData(NEXTION_BIG_UART, NEX_TSENS_PRESENCE, RegulationTaskData.counter.temperatures_present, 0, 0);
            nextion_SendData(NEXTION_BIG_UART, NEX_MAIN_SW, RegulationTaskData.ControlWord.MainSwitch_State, 0, 0);

            uint8_t pic;
            if(RegulationTaskData.ControlWord.CWUHeaterError || RegulationTaskData.IOsignals.signals.input4){
                pic = 3;
            }else if(RegulationTaskData.ControlWord.CWUHeaterStateOut){
                pic = 2;
            }else{
                pic = 1;
            }
            nextion_SendData(NEXTION_BIG_UART, NEX_CWU_PIC, pic, 0, 0);

            if(RegulationTaskData.ControlWord.COHeaterError || RegulationTaskData.IOsignals.signals.input5){
                pic = 3;
            }else if(RegulationTaskData.ControlWord.COHeaterStateOut){
                pic = 2;
            }else{
                pic = 1;
            }
            nextion_SendData(NEXTION_BIG_UART, NEX_CO_PIC, pic, 0, 0);

            for(uint8_t i = 0; i < 3; i ++){
                rest = (uint32_t)(RegulationTaskData.counter.voltages[i] * 10.0) % 10;
                nextion_SendData(NEXTION_BIG_UART, NEX_PHASE_VOLTAGE, i + 1, (uint32_t)RegulationTaskData.counter.voltages[i], rest);
                rest = (uint32_t)(RegulationTaskData.counter.currents[i] * 100.0) % 100;
                nextion_SendData(NEXTION_BIG_UART, NEX_PHASE_CURRENT, i + 1, (uint32_t)RegulationTaskData.counter.currents[i], rest);
                rest = (uint32_t)(RegulationTaskData.counter.powers[i] * 10.0) % 10;
                nextion_SendData(NEXTION_BIG_UART, NEX_PHASE_POWER, i + 1, (uint32_t)RegulationTaskData.counter.powers[i], rest);
                rest = (uint32_t)(RegulationTaskData.counter.powers[i] / RegulationTaskData.parameters.net_max_power * 100.0);
                nextion_SendData(NEXTION_BIG_UART, NEX_PHASE_PRG_BAR, i + 1, rest, 0);
                nextion_SendData(NEXTION_BIG_UART, NEX_CO_HEATER_PHASE_DUTY, i + 1, RegulationTaskData.counter.CO_heater_PWM[i], 0);
                nextion_SendData(NEXTION_BIG_UART, NEX_CWU_HEATER_PHASE_DUTY, i + 1, RegulationTaskData.counter.CWU_heater_PWM[i], 0);
            }

        }
        vTaskDelay(1000);
        /*
        current_command = NEX_CWORD;
        if(nextion_small.display == 1 && nextion_small.sleep == 0){
            buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWORD], counter.control_word);
            for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
                buffor_to_send[i] = 0xFF;
            }
            buffor_to_send_size += 3;
            HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size,20);
            vTaskDelay(1);
        }
        if(nextion_big.display == 1 || nextion_small.display == 0){
            // Temperatury
            for(int i = 0; i < 4; i++){
                if(i < 2){
                    current_command = NEX_CWU_TEMP;
                    buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CWU_TEMP], i + 1, (int16_t)(counter.CWU_temps[i] * 10));
                    for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
                        buffor_to_send[i] = 0xFF;
                    }
                    buffor_to_send_size += 3;
                    if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
                    if(nextion_small.display == 0 && nextion_small.sleep == 0) HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
                }
                current_command = NEX_CO_TEMP;
                buffor_to_send_size = sprintf(buffor_to_send, nextion_commands[NEX_CO_TEMP], i + 1, (int16_t)(counter.CO_temps[i] * 10));
                for(int i = buffor_to_send_size; i < buffor_to_send_size + 3; i++){
                    buffor_to_send[i] = 0xFF;
                }
                buffor_to_send_size += 3;
                if(nextion_big.display == 1) HAL_UART_Transmit(NEXTION_BIG_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
                if(nextion_small.display == 0 && nextion_small.sleep == 0) HAL_UART_Transmit(NEXTION_SMALL_UART, (uint8_t*)buffor_to_send, buffor_to_send_size, 20);
            }
        }*/

        ulNotificationValue = ulTaskNotifyTake(pdTRUE, 1000);
        if(ulNotificationValue > 0){
            if(ulNotificationValue == 1){
                param = nextion_big.param;
                valPtr = &nextion_big.value[0];
            }else if(ulNotificationValue == 2){
                param = nextion_small.param;
                valPtr = &nextion_small.value[0];
            }
            Config_ChangeValue(param, (void*)valPtr);

        }
    }
  /* USER CODE END UpdateLCD */
}

void NextionBig_DataRcv (void){
    uint8_t stopbyte = 0;
    uint8_t i = 0;
    uint8_t y = 0;
    uint8_t n = 0;
    uint8_t len;
    uint16_t start;
    uint32_t number = 0;
    int32_t value = 0;

    while(nextion_big_comm.UartBufferTail != nextion_big_comm.UartBufferHead){
        len = (nextion_big_comm.UartBufferTail - nextion_big_comm.UartBufferHead) & (UART_BUFFER_SIZE - 1);
        start = nextion_big_comm.UartBufferHead;
        if(len > 3){
            if(len > 9){
                if(nextion_big_comm.UART_Buffer[start + 4] == 0x0A && nextion_big_comm.UART_Buffer[start + 4] == 0x0A){
                    memcpy(&number, &nextion_big_comm.UART_Buffer[start], 4);
                    memcpy(&value, &nextion_big_comm.UART_Buffer[start + 5], 4);
                    nextion_big.param = number;
                    memcpy(&nextion_big.value, &value, 4);
                    Config_ChangeValue(number, value);
                    nextion_big_comm.UartBufferHead += 10;
                    xTaskNotifyFromISR(DiplaysHandle, 1, eSetValueWithOverwrite, NULL);
                }else{
                    n = 1;
                }
            }else {
                n = 1;
            }
            if(n){
                n = 0;
                for(i = start + 1; i < (start + 5); i++){
                    if(nextion_big_comm.UART_Buffer[i & (UART_BUFFER_SIZE - 1)] == 0xFF){
                        stopbyte ++;
                    }
                }
                if(stopbyte == 3){
                    stopbyte = 0;
                    switch(nextion_big_comm.UART_Buffer[start]){
                        case 0x66:
                            nextion_big.display = nextion_big_comm.UART_Buffer[start + 1];
                            y = 5;
                            break;

                        case 0x1a:
                            command_correct[current_command] = 1;
                            y = 4;
                            break;
                        default:
                            y = 4;
                            break;
                    }
                    nextion_big_comm.UartBufferHead += y;
                }else{
                    nextion_big_comm.UartBufferHead ++;
                }
            }

        }else{
            //if(nextion_big_comm.UART_Buffer[start] == 0 || nextion_big_comm.UART_Buffer[start] == 255){
                nextion_big_comm.UartBufferHead++;
            //}
        }
        nextion_big_comm.UartBufferHead &= (UART_BUFFER_SIZE - 1);
    }
}

void NextionSmall_DataRcv (void){
    uint8_t stopbyte = 0;
    uint8_t i = 0;
    uint8_t y = 0;
    uint8_t n = 0;
    uint8_t len;
    uint16_t start;
    uint32_t number = 0;
    int32_t value = 0;

    while(nextion_small_comm.UartBufferTail != nextion_small_comm.UartBufferHead){
        len = (nextion_small_comm.UartBufferTail - nextion_small_comm.UartBufferHead) & (UART_BUFFER_SIZE - 1);
        start = nextion_small_comm.UartBufferHead;
        if(len > 3){
            if(len > 9){
                if(nextion_small_comm.UART_Buffer[start + 4] == 0x0A && nextion_small_comm.UART_Buffer[start + 4] == 0x0A){
                    memcpy(&number, &nextion_small_comm.UART_Buffer[start], 4);
                    memcpy(&value, &nextion_small_comm.UART_Buffer[start + 5], 4);
                    nextion_small.param = number;
                    memcpy(&nextion_small.value, &value, 4);
                    Config_ChangeValue(number, value);
                    nextion_small_comm.UartBufferHead += 10;
                    xTaskNotifyFromISR(DiplaysHandle, 2, eSetValueWithOverwrite, NULL);
                }else{
                    n = 1;
                }
            }else {
                n = 1;
            }
            if(n){
                n = 0;
                for(i = start + 1; i < (start + 5); i++){
                    if(nextion_small_comm.UART_Buffer[i & (UART_BUFFER_SIZE - 1)] == 0xFF){
                        stopbyte ++;
                    }
                }
                if(stopbyte == 3){
                    stopbyte = 0;
                    switch(nextion_small_comm.UART_Buffer[start]){
                        case 0x66:
                            nextion_small.display = nextion_small_comm.UART_Buffer[start + 1];
                            y = 5;
                            break;

                        case 0x1a:
                            command_correct[current_command] = 1;
                            y = 4;
                            break;

                        case 0x86:
                            nextion_small.sleep = 1;
                            y = 4;
                            break;

                        case 0x87:
                            nextion_small.sleep = 0;
                            y = 4;
                            break;

                        case 0x88:
                            nextion_small.ready = 1;
                            y = 4;
                            break;

                        default:
                            y = 4;
                            break;
                    }
                    nextion_small_comm.UartBufferHead += y;
                }else{
                    nextion_small_comm.UartBufferHead ++;
                }
            }

        }else{
            //if(nextion_small_comm.UART_Buffer[start] == 0 || nextion_small_comm.UART_Buffer[start] == 255){
                nextion_small_comm.UartBufferHead++;
            //}
        }
        nextion_small_comm.UartBufferHead &= (UART_BUFFER_SIZE - 1);
    }
}
