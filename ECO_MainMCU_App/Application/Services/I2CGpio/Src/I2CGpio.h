#ifndef I2CGPIO_H_
#define I2CGPIO_H_

#include <stdint.h>
#include <stdbool.h>

void I2CGPIO_Init(uint8_t expanders_count);

bool I2CGPIO_Read(uint16_t pin);

void I2CGPIO_Write(uint16_t pin, bool state);

void I2CGPIO_Fetch(void);

void I2CGPIO_Push(void);

#endif /* I2CGPIO_H_ */
