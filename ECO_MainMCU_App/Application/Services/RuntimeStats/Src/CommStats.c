/*
 * CommStats.c
 *
 *  Created on: 4 wrz 2020
 *      Author: magiczny_kacper
 */

#include "RuntimeStats.h"
#include "printf.h"

static CommStats_t commStats;

void RuntimeStats_Init(void){
	memset(&commStats, 0, sizeof(CommStats_t));
}

#ifdef TELNET_STAT
#undef TELNET_STAT
#endif

#define TELNET_STAT(param) 							\
	void RuntimeStats_Telnet_ ## param ## _Inc(void){commStats.Telnet.param ++;} \
	uint32_t RuntimeStats_Telnet_ ## param ## _Get(void){return commStats.Telnet.param;}

TELNET_STATS

#ifdef MODBUS_MASTER_STAT
#undef MODBUS_MASTER_STAT
#endif

#define MODBUS_MASTER_STAT(param) 							\
	void RuntimeStats_ModbusMaster_ ## param ## _Inc(void){commStats.ModbusMaster.param ++;} \
	uint32_t RuntimeStats_ModbusMaster_ ## param ## _Get(void){return commStats.ModbusMaster.param;}

MODBUS_MASTER_STATS

#ifdef MODBUS_SLAVE_STAT
#undef MODBUS_SLAVE_STAT
#endif

#define MODBUS_SLAVE_STAT(param) 							\
	void RuntimeStats_ModbusSlave_ ## param ## _Inc(void){commStats.ModbusSlave.param ++;} \
	uint32_t RuntimeStats_ModbusSlave_ ## param ## _Get(void){return commStats.ModbusSlave.param;}

MODBUS_SLAVE_STATS

void RuntimeStats_TelnetCurrIPSet (uint8_t* ip){
	memcpy(&commStats.Telnet.currIP, ip, 4);
}

void RuntimeStats_TelnetLastIPSet (void){
	memcpy(&commStats.Telnet.lastIP, &commStats.Telnet.currIP, 4);
	memset(&commStats.Telnet.currIP, 0, 4);
}

void RuntimeStats_TelnetGetCurIP (uint8_t* ip){
	memcpy(ip, &commStats.Telnet.currIP, 4);
}

void RuntimeStats_TelnetGetLastIP (uint8_t* ip){
	memcpy(ip, &commStats.Telnet.lastIP, 4);
}
