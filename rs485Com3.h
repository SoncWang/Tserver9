#ifndef __RS485COM3_H__
#define __RS485COM3_H__

#include "modbus.h"
#include "registers.h"
#include "./src/jsonPackage.h"
#include "HttpPost.h"

void Rs485Com3Init(void);
UINT16 SendCom3RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code);
void lockerPollingInit(void);
int SendCom3ReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT);
void *ComPort3Thread(void *param);
UINT16 SendCom3Test(char *buf,int len);
int SendCom3WriteReg(UINT16 Addr, UINT8 Func);

#endif


