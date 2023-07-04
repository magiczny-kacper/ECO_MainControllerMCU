#ifndef HEATER_H_
#define HEATER_H_

#include <stdint.h>
#include <stdbool.h>

#include "tim.h"
#include "Heater_Cfg.h"

typedef uint32_t Heater_Id_t;

void Heater_Init(Heater_Id_t dev);

void Heater_SetDutyCycles(Heater_Id_t dev, uint32_t* duty);

bool Heater_Check(Heater_Id_t dev);

void Heater_Enable(Heater_Id_t dev);

void Heater_Disable(Heater_Id_t dev);

#endif /* HEATER_H_ */
