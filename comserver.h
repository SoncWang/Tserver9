#ifndef __COMSERVER_H__
#define __COMSERVER_H__

#include "modbus.h"
#include "registers.h"
#include "./src/jsonPackage.h"
#include "HttpPost.h"

void cominit(void) ;

//向采集器发送查询寄存器指令
int SendCom1ReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT);

//处理COMM口返回数据
int DealComm(unsigned char *buf,unsigned short int len);

//设置结构体参数
int SetUpsStatusStruct(unsigned char *buf,unsigned short int len);
int SetEnviStatusStruct(unsigned char *buf,unsigned short int len);
int SetSpdStatusStruct(unsigned char *buf,unsigned short int len);
int SetRsuStatusStruct(unsigned char *buf,unsigned short int len);
int SetAirCondStruct(unsigned char *buf,unsigned short int len);

//向采集器发送写寄存器指令
int SendCom1WriteReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT, UINT8 *pBuf);
//发送写遥控寄存器 
int SendCom1RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code);

#endif
