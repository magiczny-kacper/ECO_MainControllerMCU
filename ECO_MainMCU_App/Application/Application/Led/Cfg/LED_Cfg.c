#include "Led_Cfg.h"

static const LED_PatternStep_t SOS_Steps[] = 
{
    {
        .on_time = 100,
        .off_time = 200,
        .repeats = 3
    },
    {
        .on_time = 500,
        .off_time = 200,
        .repeats = 3
    },
    {
        .on_time = 100,
        .off_time = 200,
        .repeats = 3
    },
    {
        .on_time = 0,
        .off_time = 2000,
        .repeats = 1
    },
};

static const LED_PatternStep_t FastBlink_Steps[] = 
{
    {
        .on_time = 200,
        .off_time = 300,
        .repeats = 1
    }
};

static const LED_PatternStep_t SlowBlink_Steps[] = 
{
    {
        .on_time = 500,
        .off_time = 1000,
        .repeats = 1
    }
};

const LED_Pattern_t Patterns[PATTERN_CNT] = 
{
    {
        .step_count = 4,
        .steps = SOS_Steps
    },
    {
        .step_count = 1,
        .steps = FastBlink_Steps
    },
    {
        .step_count = 1,
        .steps = SlowBlink_Steps
    }
};