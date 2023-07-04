/*
 * RuntimeStats.h
 *
 *  Created on: 4 wrz 2020
 *      Author: magiczny_kacper
 */

#ifndef RUNTIMESTATS_H_
#define RUNTIMESTATS_H_

#define STAT_SIZE_B 48

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TELNET_STATS \
	TELNET_STAT(RxFrames) \
	TELNET_STAT(TxFrames)


#ifdef TELNET_STAT
#undef TELNET_STAT
#endif

#define TELNET_STAT(param) uint32_t param;

typedef struct {
	TELNET_STATS
	uint8_t currIP[4];
	uint8_t lastIP [4];
} TelnetCommStats_t;

#define MODBUS_SLAVE_STATS 			\
	MODBUS_SLAVE_STAT(AllRequests) 	\
	MODBUS_SLAVE_STAT(OkResponses) 	\
	MODBUS_SLAVE_STAT(ErrResponses) \
	MODBUS_SLAVE_STAT(AllResponses)

#ifdef MODBUS_SLAVE_STAT
#undef MODBUS_SLAVE_STAT
#endif

#define MODBUS_SLAVE_STAT(param) uint32_t param;

typedef struct {
	MODBUS_SLAVE_STATS
} ModbusSlaveCommStats_t;

#define MODBUS_MASTER_STATS 			\
	MODBUS_MASTER_STAT(AllRequests) 	\
	MODBUS_MASTER_STAT(OkResponses) 	\
	MODBUS_MASTER_STAT(ErrResponses)	\
	MODBUS_MASTER_STAT(WrongResponses)	\
	MODBUS_MASTER_STAT(NoResponses)		\
	MODBUS_MASTER_STAT(AllResponses) 	\


#ifdef MODBUS_MASTER_STAT
#undef MODBUS_MASTER_STAT
#endif

#define MODBUS_MASTER_STAT(param) uint32_t param;

typedef struct {
	MODBUS_MASTER_STATS
} ModbusMasterCommStats_t;

typedef struct {
	TelnetCommStats_t Telnet;
	ModbusSlaveCommStats_t ModbusSlave;
	ModbusMasterCommStats_t ModbusMaster;
} CommStats_t;

void RuntimeStats_Init(void);

#ifdef TELNET_STAT
#undef TELNET_STAT
#endif

#define TELNET_STAT(param) 							\
	void RuntimeStats_Telnet_ ## param ## _Inc(void); 		\
	uint32_t RuntimeStats_Telnet_ ## param ## _Get(void);

TELNET_STATS

#ifdef MODBUS_MASTER_STAT
#undef MODBUS_MASTER_STAT
#endif

#define MODBUS_MASTER_STAT(param) 							\
	void RuntimeStats_ModbusMaster_ ## param ## _Inc(void); \
	uint32_t RuntimeStats_ModbusMaster_ ## param ## _Get(void);

MODBUS_MASTER_STATS

#ifdef MODBUS_SLAVE_STAT
#undef MODBUS_SLAVE_STAT
#endif

#define MODBUS_SLAVE_STAT(param) 							\
	void RuntimeStats_ModbusSlave_ ## param ## _Inc(void); \
	uint32_t RuntimeStats_ModbusSlave_ ## param ## _Get(void);

MODBUS_SLAVE_STATS

void RuntimeStats_TelnetCurrIPSet (uint8_t* ip);
void RuntimeStats_TelnetLastIPSet (void);

void RuntimeStats_TelnetGetCurIP (uint8_t* ip);
void RuntimeStats_TelnetGetLastIP (uint8_t* ip);

#endif /* RUNTIMESTATS_H_ */
