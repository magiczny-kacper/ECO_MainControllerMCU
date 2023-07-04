#ifndef HEATER_CFG_H_
#define HEATER_CFG_H_

#include "Heater.h"

#define HEATER_COUNT 3

#define PHASES_3PH 3

typedef struct Heater_Cfg
{
    TIM_HandleTypeDef* timer[PHASES_3PH];
    uint16_t timer_channel[PHASES_3PH];
    uint16_t enable_output;
    uint16_t enable_input;
    uint16_t fuse_input;
} Heater_Cfg_t;

extern const Heater_Cfg_t heaters_cfg[HEATER_COUNT];

bool Heater_ReadGPIO(uint16_t pin);

void Heater_SetGPIO(uint16_t pin, bool state);

#endif /* HEATER_CFG_H_ */
