#include "LED.h"

#include "gpio.h"

#include "FreeRTOS.h"
#include "task.h"

// #include "printf.h"

typedef struct LED_Pin
{
    const GPIO_TypeDef *gpio_base;
    const uint16_t gpio_pin;
} LED_Pin_t;

typedef struct LED_State
{
    LED_PatternId_t pattern;
    uint32_t pattern_step;
    uint32_t repeat_counter;
    const LED_Pin_t* pin;
} LED_State_t;

static const char *task_name[LED_COUNT] = 
{
    "LED 1", "LED 2"
};

static TaskHandle_t task_handle[LED_COUNT] = {NULL};

#define TASK_STACK_SIZE 150
static StaticTask_t task_buffer[LED_COUNT] = {0};
static StackType_t task_stack[LED_COUNT][TASK_STACK_SIZE] = {0};
static LED_State_t task_state[2] = {0};

static const LED_Pin_t led_pin[LED_COUNT] = 
{
    {
        .gpio_base = LED1_GPIO_Port,
        .gpio_pin = LED1_Pin
    },
    {
        .gpio_base = LED2_GPIO_Port,
        .gpio_pin = LED2_Pin
    }
};

static void LedTask(void* argument);

void LED_Init(void)
{
    task_state[0].pattern = SOS;
    task_state[1].pattern = FAST_BLINK;
    
    for (uint32_t i = 0; i < LED_COUNT; i++)
    {
        task_state[i].pin = &led_pin[i];
        task_handle[i] = xTaskCreateStatic(
            LedTask, 
            task_name[i], 
            TASK_STACK_SIZE,
            (void*) &task_state[i], 
            3, 
            task_stack[i], 
            &task_buffer[i]
        );
    }
}

static void LedTask(void* argument)
{
    const LED_Pattern_t* current_pattern;
    const LED_PatternStep_t* current_step;

    while(1)
    {
        LED_State_t* state = (LED_State_t*) argument;
        current_pattern = &Patterns[state -> pattern];
        current_step = &(current_pattern -> steps[state -> pattern_step]);

        if (current_step -> on_time > 0)
        {
            HAL_GPIO_WritePin((GPIO_TypeDef*)state -> pin -> gpio_base,
                state -> pin -> gpio_pin, GPIO_PIN_SET);
            vTaskDelay(current_step -> on_time);
        }
        
        if (current_step -> off_time > 0)
        {
            HAL_GPIO_WritePin((GPIO_TypeDef*)state -> pin -> gpio_base,
                state -> pin -> gpio_pin, GPIO_PIN_RESET);
            vTaskDelay(current_step -> off_time);
        }

        state -> repeat_counter++;
        if (state -> repeat_counter >= current_step->repeats)
        {
            state -> repeat_counter = 0;
            state -> pattern_step++;
            if (state -> pattern_step >= current_pattern -> step_count)
            {
                state -> pattern_step = 0;
            }
        }
    }
}