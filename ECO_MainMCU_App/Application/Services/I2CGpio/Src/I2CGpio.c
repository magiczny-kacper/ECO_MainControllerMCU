#include "I2CGpio.h"
#include "i2c.h"
#include "MCP23017.h"

#include "FreeRTOS.h"
#include "task.h"

#define EXPANDER_I2C &hi2c1

#define PIN_TO_EXPANDER(pin) (pin >> 4)
#define PIN_TO_PORT(pin) ((pin >> 3) & 0x01)
#define PIN_TO_GPIO(pin) (pin & 0x07)

// #define IODIRA 0b11111100
// #define IODIRB 0b00000000
#define IODIRA 0x03
#define IODIRB 0xFF

#define IPOL 0x00
#define GGPU 0x00

#define MAX_EXPANDER_COUNT 8
#define EXPANDER_PINS 16
#define MAX_PIN_COUNT (MAX_EXPANDER_COUNT * EXPANDER_PINS)

#define TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define GPIO_REFRESH_PERIOD 10

typedef struct I2CGPIO_Expander_Flags
{
    uint16_t used :1;
    uint16_t initialized :1;
    uint16_t present :1;
} I2CGPIO_Expander_Flags_t;

typedef struct I2CGPIO_Expander
{
    MCP23017_HandleTypeDef expander;
    I2CGPIO_Expander_Flags_t flags;
} I2CGPIO_Expander_t;

static uint8_t expander_count;

static I2CGPIO_Expander_t expanders[MAX_EXPANDER_COUNT];

static TaskHandle_t xI2CGpioThread;
static StaticTask_t xTaskBuffer;
static StackType_t xTaskStack[TASK_STACK_SIZE];

static void I2CGPIO_Task(void* argument);

void I2CGPIO_Init(uint8_t expanders_count)
{
    uint8_t i;
    HAL_StatusTypeDef status;
    MCP23017_HandleTypeDef *exp;

    if (expanders_count > MAX_EXPANDER_COUNT)
    {
        expanders_count = MAX_EXPANDER_COUNT;
    }
    expander_count = expanders_count;

    for(i = 0; i < MAX_EXPANDER_COUNT; i++)
    {
        if (i < expander_count)
        {
            exp = &expanders[i].expander;
            expanders[i].flags.used = 1;

            mcp23017_init(exp, EXPANDER_I2C, MCP23017_ADDRESS_27 + i); 
            status = mcp23017_iodir(exp, MCP23017_PORTA, IODIRA);

            if (HAL_OK == status)
            {
                status = mcp23017_iodir(exp, MCP23017_PORTB, IODIRB);
            }

            if (HAL_OK == status)
            {
                status = mcp23017_ipol(exp, MCP23017_PORTA, IPOL);
            }

            if (HAL_OK == status)
            {
                status = mcp23017_ipol(exp, MCP23017_PORTB, IPOL);
            }

            if (HAL_OK == status)
            {
                status = mcp23017_ggpu(exp, MCP23017_PORTA, GGPU);
            }

            if (HAL_OK == status)
            {
                status = mcp23017_ggpu(exp, MCP23017_PORTB, GGPU);
            }

            if (HAL_OK == status)
            {
                status = mcp23017_read_gpio(exp, MCP23017_PORTA);
            }

            if (HAL_OK == status)
            {
                status = mcp23017_read_gpio(exp, MCP23017_PORTB);
            }

            if (HAL_OK == status)
            {
            }

            if (HAL_OK == status)
            {
                expanders[i].flags.initialized = 1;
                expanders[i].flags.present = 1;
            }
        }
        else
        {
            status = HAL_I2C_IsDeviceReady(EXPANDER_I2C, MCP23017_ADDRESS_27 + i, 5, 5);
            if (HAL_OK == status)
            {
                expanders[i].flags.present = 1;
            }
        }
    }

    xI2CGpioThread = xTaskCreateStatic(
        I2CGPIO_Task, 
        "I2CGpioTask", 
        TASK_STACK_SIZE,
        (void*) NULL, 
        3, 
        xTaskStack, 
        &xTaskBuffer
    );
}

bool I2CGPIO_Read(uint16_t pin)
{
    uint8_t expander_idx;
    uint8_t expander_pin;
    uint8_t expander_port;
    uint8_t state;

    if (pin >= MAX_PIN_COUNT) return false;

    expander_idx = PIN_TO_EXPANDER(pin);
    expander_port = PIN_TO_PORT(pin);
    expander_pin = PIN_TO_GPIO(pin);
    state = expanders[expander_idx].expander.gpio[expander_port] & (1 << expander_pin);
    return (state != 0);
}

void I2CGPIO_Write(uint16_t pin, bool state)
{
    uint8_t expander_idx;
    uint8_t expander_pin;
    uint8_t expander_port;

    if (pin >= MAX_PIN_COUNT) return;

    expander_idx = PIN_TO_EXPANDER(pin);
    expander_port = PIN_TO_PORT(pin);
    expander_pin = PIN_TO_GPIO(pin);

    if (state)
    {
        expanders[expander_idx].expander.gpio[expander_port] |= (1 << expander_pin);
    }
    else
    {
        expanders[expander_idx].expander.gpio[expander_port] &= ~(1 << expander_pin);
    }
}

void I2CGPIO_Fetch(void)
{
    uint8_t i;
    MCP23017_HandleTypeDef *exp;

    for(i = 0; i < MAX_EXPANDER_COUNT; i++)
    {
        exp = &expanders[i].expander;
        mcp23017_read_gpio(exp, MCP23017_PORTA);
        mcp23017_read_gpio(exp, MCP23017_PORTB);
    }
}

void I2CGPIO_Push(void)
{
    uint8_t i;
    MCP23017_HandleTypeDef *exp;

    for(i = 0; i < MAX_EXPANDER_COUNT; i++)
    {
        exp = &expanders[i].expander;
        mcp23017_write_gpio(exp, MCP23017_PORTA);
        // mcp23017_write_gpio(exp, MCP23017_PORTB);q
    }
}

static void I2CGPIO_Task(void* argument)
{
    (void) argument;

    while(1)
    {
        I2CGPIO_Fetch();
        I2CGPIO_Push();
        vTaskDelay(GPIO_REFRESH_PERIOD);
    }
}