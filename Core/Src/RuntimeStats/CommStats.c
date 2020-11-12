/*
 * CommStats.c
 *
 *  Created on: 4 wrz 2020
 *      Author: magiczny_kacper
 */

#include "RuntimeStats.h"
#include "printf.h"

CommStats_t commStats;

void RuntimeStats_Init(void){
	memset(&commStats, 0, 48);
}

void RuntimeStats_TelnetRxInc (void){
	commStats.telnetStats.rxFrames++;
}

void RuntimeStats_TelnetTxInc (void){
	commStats.telnetStats.txFrames++;
}

void RuntimeStats_TelnetCurrIPSet (uint8_t* ip){
	memcpy(&(commStats.telnetStats.currIP), ip, 4);
}

void RuntimeStats_TelnetLastIPSet (void){
	memcpy(&(commStats.telnetStats.lastIP), &(commStats.telnetStats.currIP), 4);
	memset(&(commStats.telnetStats.currIP), 0, 4);
}

uint32_t RuntimeStats_TelnetGetRxCnt (void){
	return commStats.telnetStats.rxFrames;
}

uint32_t RuntimeStats_TelnetGetTxCnt (void){
	return commStats.telnetStats.txFrames;
}

void RuntimeStats_TelnetGetCurIP (uint8_t* ip){
	memcpy(ip, &(commStats.telnetStats.currIP), 4);
}

void RuntimeStats_TelnetGetLastIP (uint8_t* ip){
	memcpy(ip, &(commStats.telnetStats.lastIP), 4);
}

void RuntimeStats_ModbusMasterRqSendInc (void){
	commStats.mdMAsterStats.rqSend ++;
}

void RuntimeStats_ModbusMasterNoRespInc (void){
	commStats.mdMAsterStats.noResp ++;
}

void RuntimeStats_ModbusMasterRespOkInc (void){
	commStats.mdMAsterStats.respOk ++;
}

void RuntimeStats_ModbusMasterWrongRespInc (void){
	commStats.mdMAsterStats.wrongRq ++;
}

uint32_t RuntimeStats_ModbusMasterRqSendGet (void){
	return commStats.mdMAsterStats.rqSend;
}

uint32_t RuntimeStats_ModbusMasterNoRespGet (void){
	return commStats.mdMAsterStats.noResp;
}

uint32_t RuntimeStats_ModbusMasterRespOkGet (void){
	return commStats.mdMAsterStats.respOk;
}

uint32_t RuntimeStats_ModbusMasterWrongRespGet (void){
	return commStats.mdMAsterStats.wrongRq;
}

void RuntimeStats_ModbusSlaveRqAllInc (void){
	commStats.mbSlaveStats.rqAll++;
}

void RuntimeStats_ModbusSlaveRqOkInc (void){
	commStats.mbSlaveStats.rqOk++;
}

void RuntimeStats_ModbusSlaveRqErrInc (void){
	commStats.mbSlaveStats.rqErr++;
}

void RuntimeStats_ModbusSlaveRespInc (void){
	commStats.mbSlaveStats.respSend++;
}

uint32_t RuntimeStats_ModbusSlaveRqAllGet (void){
	return commStats.mbSlaveStats.rqAll;
}

uint32_t RuntimeStats_ModbusSlaveRqOkGet (void){
	return commStats.mbSlaveStats.rqOk;
}

uint32_t RuntimeStats_ModbusSlaveRqErrGet (void){
	return commStats.mbSlaveStats.rqErr;
}

uint32_t RuntimeStats_ModbusSlaveRespGet (void){
	return commStats.mbSlaveStats.respSend;
}
