/*
 * DataLogTask.h
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#ifndef SRC_TASKS_DATALOGTASK_DATALOGTASK_H_
#define SRC_TASKS_DATALOGTASK_DATALOGTASK_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void FLASH_ChipSelect(void);

void FLASH_ChipDeselect(void);

void DataLogTask(void const * argument);

#endif /* SRC_TASKS_DATALOGTASK_DATALOGTASK_H_ */
