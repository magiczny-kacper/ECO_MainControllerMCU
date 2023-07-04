#include "Heater.h"
#include "Heater_Cfg.h"

typedef struct Heater
{
    Heater_Cfg_t* config;
    uint32_t output_duty[3];
    bool enabled;
    bool enabled_feedback;
    bool fuse;
    bool error;
} Heater_t;

static Heater_t heater[HEATER_COUNT];

void Heater_Init(Heater_Id_t dev)
{
    Heater_Cfg_t* cfg;
    if (dev >= HEATER_COUNT) return;

    cfg = &heaters_cfg[dev];
    heater[dev].config = cfg;

    for (uint32_t i = 0; i < PHASES_3PH; i++)
    {
        HAL_TIM_PWM_Start(cfg -> timer[i], cfg -> timer_channel[i]);
        __HAL_TIM_SET_COMPARE(cfg -> timer[i], cfg -> timer_channel[i], 0);
    }
}

void Heater_SetDutyCycles(Heater_Id_t dev, uint32_t* duty)
{
    if (dev >= HEATER_COUNT) return;
    for(uint32_t i = 0; i < PHASES_3PH; i++)
    {
        heater[dev].output_duty[i] = duty[i];
    }
}

bool Heater_Check(Heater_Id_t dev)
{
    bool fuse;
    bool feedback;
    bool error;
    Heater_Cfg_t* cfg;

    if (dev >= HEATER_COUNT) return false;

    cfg = heater[dev].config;
    fuse = Heater_ReadGPIO(cfg->fuse_input);
    feedback = Heater_ReadGPIO(cfg->enable_input);
    error = !(fuse & (feedback ^ heater[dev].enabled));

    heater[dev].fuse = fuse;
    heater[dev].enabled_feedback = feedback;
    heater[dev].error = error;
    
    return !error;
}

void Heater_Enable(Heater_Id_t dev)
{
    Heater_Cfg_t* cfg;

    if (dev >= HEATER_COUNT) return;

    cfg = heater[dev].config;

    if (true == Heater_Check(dev))
    {
        Heater_SetGPIO(cfg->enable_output, true);

        for (uint32_t i = 0; i < PHASES_3PH; i++)
        {
            __HAL_TIM_SET_COMPARE(cfg -> timer[i], 
                cfg -> timer_channel[i], 
                heater[dev].output_duty[i]);
        }
        heater[dev].enabled = true;
    }
}

void Heater_Disable(Heater_Id_t dev)
{
    Heater_Cfg_t* cfg;
    if (dev >= HEATER_COUNT) return;
    
    cfg = heater[dev].config;
    Heater_SetGPIO(cfg->enable_output, false);
}