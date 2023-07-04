#ifndef LED_TYPES_H_
#define LED_TYPES_H_

#include "LED_Cfg.h"

typedef struct Led_Handle
{
    LED_PatternId_t current_pattern;
} LED_Handle_t;

#endif /* LED_TYPES_H_ */
