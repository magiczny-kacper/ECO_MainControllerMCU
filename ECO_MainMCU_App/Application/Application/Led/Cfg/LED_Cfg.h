#ifndef LED_CFG_H_
#define LED_CFG_H_

#include <stdint.h>

/* Global defines */
#define LED_COUNT 2

/* Type definitions */
typedef struct LED_PatternStep
{
    uint32_t on_time;
    uint32_t off_time;
    uint32_t repeats;
} LED_PatternStep_t;

typedef struct LED_Pattern
{
    uint32_t step_count;
    const LED_PatternStep_t *steps;
} LED_Pattern_t;

typedef enum LED_PatternId
{
    SOS = 0,
    FAST_BLINK,
    SLOW_BLINK,
    PATTERN_CNT,
    ALWAYS_ON,
    ALWAYS_OFF
}   LED_PatternId_t;

/* Exported variables */
extern const LED_Pattern_t Patterns[PATTERN_CNT];

#endif /* LED_CFG_H_ */
