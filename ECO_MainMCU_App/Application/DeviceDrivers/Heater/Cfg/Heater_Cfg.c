#include "main.h"
#include "Heater_Cfg.h"

const Heater_Cfg_t heaters_cfg[HEATER_COUNT] = 
{
    {
        .timer = 
        {
            &htim1, &htim1, &htim1
        },
        .timer_channel = 
        {
            TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3
        },
        .enable_output = 7,
        .enable_input = 9,
        .fuse_input = 10
    },
    {
        .timer = 
        {
            &htim2, &htim2, &htim2
        },
        .timer_channel = 
        {
            TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3
        },
        .enable_output = 6,
        .enable_input = 11,
        .fuse_input = 12
    },
    {
        .timer = 
        {
            &htim3, &htim3, &htim3
        },
        .timer_channel = 
        {
            TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3
        },
        .enable_output = 5,
        .enable_input = 13,
        .fuse_input = 14
    },
};

bool Heater_ReadGPIO(uint16_t pin)
{
    (void)pin;
    return false;
}

void Heater_SetGPIO(uint16_t pin, bool state)
{
    (void)pin;
    (void)state;
}