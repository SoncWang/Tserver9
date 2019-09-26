#ifndef __RS485COM4_H__
#define __RS485COM4_H__

#include "modbus.h"
#include "registers.h"
#include "./src/jsonPackage.h"
#include "HttpPost.h"

void Rs485Com4Init(void);
UINT16 SendCom4RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code);
int SendCom4ReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT);
void *Dev_DataPollingthread(void *param);
void *ComPort4Thread(void *param);
int SendCom4WriteReg(UINT16 Addr, UINT8 Func);

#endif


