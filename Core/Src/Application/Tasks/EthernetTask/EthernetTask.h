/*
 * EthernetTask.h
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#ifndef SRC_TASKS_ETHERNETTASK_ETHERNETTASK_H_
#define SRC_TASKS_ETHERNETTASK_ETHERNETTASK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void EthernetTask(void* argument);

void EthernetTask_Request (uint32_t request);

#endif /* SRC_TASKS_ETHERNETTASK_ETHERNETTASK_H_ */
