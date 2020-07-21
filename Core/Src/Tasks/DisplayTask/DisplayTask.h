/*
 * DisplayTask.h
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#ifndef SRC_TASKS_DISPLAYTASK_DISPLAYTASK_H_
#define SRC_TASKS_DISPLAYTASK_DISPLAYTASK_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
	PHASE1 = 1,
	PHASE2,
	PHASE3
} phases;

typedef enum _nextion_command{
	NEX_WAKE = 0,
	NEX_SENDME,
	NEX_YEAR,
	NEX_MONTH,
	NEX_DAY,
	NEX_HOUR,
	NEX_MINUTE,
	NEX_SECONDS,
	NEX_CO_HIGH_TEMP,
	NEX_CO_LOW_TEMP,
	NEX_CO_HEATER_POWER,
	NEX_CWU_HIGH_TEMP,
	NEX_CWU_LOW_TEMP,
	NEX_CWU_HEATER_POWER,
	NEX_ACC_COEFF,
	NEX_HEATER_COEFF,
	NEX_CWORD,
	NEX_SETTINGS_SAVED,
	NEX_SETTINGS_NOT_SAVED,
	NEX_COUNTER_PRESENCE,
	NEX_TSENS_PRESENCE,
	NEX_MAIN_SW,
	NEX_CWU_PIC,
	NEX_CO_PIC,
	NEX_PHASE_VOLTAGE,
	NEX_PHASE_CURRENT,
	NEX_PHASE_POWER,
	NEX_PHASE_PRG_BAR,
	NEX_CO_HEATER_PHASE_DUTY,
	NEX_CWU_HEATER_PHASE_DUTY,
	NEX_CWU_TEMP,
	NEX_CO_TEMP,
	NEX_SETTINGS_UNLOCK,
	NEX_SETTINGS_LOCK,
	NEX_COMMANDS_COUNT
} nextion_command;

const static char* nextion_commands[NEX_COMMANDS_COUNT] = {
		"sleep=0",
		"sendme",
		"rtc0=%d",
		"rtc1=%d",
		"rtc2=%d",
		"rtc3=%d",
		"rtc4=%d",
		"rtc5=%d",
		"sett.COhi.txt=\"%d\"",
		"sett.COlo.txt=\"%d\"",
		"sett.COhp.txt=\"%d\"",
		"sett.CWUhi.txt=\"%d\"",
		"sett.CWUlo.txt=\"%d\"",
		"sett.CWUhp.txt=\"%d\"",
		"sett.apcoeff.txt=\"%d\"",
		"sett.hpcoeff.txt=\"%d\"",
		"sett.cword.val=%d",
		"sett.saved.txt=\"Zapisano\"",
		"sett.saved.txt=\"Blad\"",
		"pow.cntrpres.val=%d",
		"pow.temppres.val=%d",
		"pow.mainsw.val=%d",
		"pow.baniakCWU.pic=%d",
		"pow.baniakCO.pic=%d",
		"pow.L%dV.txt=\"%d.%d\"",
		"pow.L%dC.txt=\"%d.%d\"",
		"pow.L%dP.txt=\"%d.%d\"",
		"pow.L%dPRG.val=%d",
		"pow.COHL%d.val=%d",
		"pow.CWUHL%d.val=%d",
		"pow.CWUt%d.val=%d",
		"pow.COt%d.val=%d",
		"pow.lock.val=0",
		"pow.lock.val=1"
};

void DisplayTask(void const * argument);

#endif /* SRC_TASKS_DISPLAYTASK_DISPLAYTASK_H_ */
