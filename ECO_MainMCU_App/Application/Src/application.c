#include "application.h"

#include "ResourceGuard.h"
#include "LED.h"
#include "I2CGpio.h"
#include "EventLog.h"
#include "Ethernet.h"
#include "ConfigMgr.h"
#include "CLI.h"
#include "SWUpdate.h"

#include "crc.h"
#include "i2c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "bootutil/bootutil.h"

unsigned long ulHighFreqTimerTicks;
static TaskHandle_t xAppTaskHandle = NULL;
static TimerHandle_t xImageValidationTimer = NULL;

static void Application_ValidateImageTimerCllback (TimerHandle_t xTimer);

extern void xPortSysTickHandler (void);

static void Application_Task(void* argument)
{
    RG_Init();
    // I2CGPIO_Init(1);
    CLI_Init();
    DataLog_Init();
    ConfigMgr_Init(&hi2c2, &hcrc);
    EthernetTask_Init();
    SWUpdate_Init();
    LED_Init();
    xImageValidationTimer = xTimerCreate (
            "Timer",
            10 * 1000,
            pdFALSE,
            (void*)0,
            Application_ValidateImageTimerCllback
        );
    if (xImageValidationTimer != NULL)
    {
        xTimerStart(xImageValidationTimer, 10);
    }

    vTaskDelete(NULL);
}

void Application_Init(void)
{
    BaseType_t xReturned;
    
    /* Create the task, storing the handle. */
    (void)xTaskCreate
    (
        Application_Task, "Application_Task", 128, NULL, tskIDLE_PRIORITY,
        &xAppTaskHandle
    );
}

void Application_Run(void)
{
    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    while(1)
    {
        
    }
}

void Application_SysTick_Callback(void)
{
#if (INCLUDE_xTaskGetSchedulerState == 1 )
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
#else
    xPortSysTickHandler();
#endif /* INCLUDE_xTaskGetSchedulerState */

    EthernetTask_Tick();
}

void Application_HighFreqTick_Callback(void)
{
    ulHighFreqTimerTicks ++;
}

static void Application_ValidateImageTimerCllback (TimerHandle_t xTimer)
{
    boot_set_confirmed();
}