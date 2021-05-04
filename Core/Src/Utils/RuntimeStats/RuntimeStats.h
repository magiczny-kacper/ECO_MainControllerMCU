/*
 * RuntimeStats.h
 *
 *  Created on: 4 wrz 2020
 *      Author: magiczny_kacper
 */

#ifndef SRC_RUNTIMESTATS_RUNTIMESTATS_H_
#define SRC_RUNTIMESTATS_RUNTIMESTATS_H_

#define STAT_SIZE_B 48

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	uint32_t rxFrames;
	uint32_t txFrames;
	uint8_t currIP[4];
	uint8_t lastIP [4];
} TelnetCommStats_t;

typedef struct {
	uint32_t rqAll;
	uint32_t rqOk;
	uint32_t rqErr;
	uint32_t respSend;
} ModbusSlaveCommStats_t;

typedef struct {
	uint32_t rqSend;
	uint32_t respOk;
	uint32_t noResp;
	uint32_t wrongRq;
} ModbusMasterCommStats_t;

typedef struct {
	TelnetCommStats_t telnetStats;
	ModbusSlaveCommStats_t mbSlaveStats;
	ModbusMasterCommStats_t mdMAsterStats;
} CommStats_t;

void RuntimeStats_Init(void);

void RuntimeStats_TelnetRxInc(void);
void RuntimeStats_TelnetTxInc(void);
void RuntimeStats_TelnetCurrIPSet (uint8_t* ip);
void RuntimeStats_TelnetLastIPSet (void);

uint32_t RuntimeStats_TelnetGetRxCnt (void);
uint32_t RuntimeStats_TelnetGetTxCnt (void);
void RuntimeStats_TelnetGetCurIP (uint8_t* ip);
void RuntimeStats_TelnetGetLastIP (uint8_t* ip);

void RuntimeStats_ModbusMasterRqSendInc (void);
void RuntimeStats_ModbusMasterNoRespInc (void);
void RuntimeStats_ModbusMasterRespOkInc (void);
void RuntimeStats_ModbusMasterWrongRespInc (void);

uint32_t RuntimeStats_ModbusMasterRqSendGet (void);
uint32_t RuntimeStats_ModbusMasterNoRespGet (void);
uint32_t RuntimeStats_ModbusMasterRespOkGet (void);
uint32_t RuntimeStats_ModbusMasterWrongRespGet (void);

void RuntimeStats_ModbusSlaveRqAllInc (void);
void RuntimeStats_ModbusSlaveRqOkInc (void);
void RuntimeStats_ModbusSlaveRqErrInc (void);
void RuntimeStats_ModbusSlaveRespInc (void);

uint32_t RuntimeStats_ModbusSlaveRqAllGet (void);
uint32_t RuntimeStats_ModbusSlaveRqOkGet (void);
uint32_t RuntimeStats_ModbusSlaveRqErrGet (void);
uint32_t RuntimeStats_ModbusSlaveRespGet (void);

#endif /* SRC_RUNTIMESTATS_RUNTIMESTATS_H_ */
