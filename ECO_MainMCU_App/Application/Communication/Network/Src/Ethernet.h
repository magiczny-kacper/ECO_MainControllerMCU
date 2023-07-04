#ifndef ETHERNET_H_
#define ETHERNET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void EthernetTask_Init(void);

void EthernetTask_IRQ_Callback(void);

void EthernetTask_Tick(void);

#endif /* ETHERNET_H_ */
