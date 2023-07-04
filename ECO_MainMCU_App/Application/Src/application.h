#ifndef APPLICATION_H_
#define APPLICATION_H_

extern unsigned long ulHighFreqTimerTicks;

void Application_Init(void);

void Application_Run(void);

void Application_SysTick_Callback(void);

void Application_HighFreqTick_Callback(void);

unsigned long Application_GetHighFreqTicks(void);

#endif /* APPLICATION_H_ */
