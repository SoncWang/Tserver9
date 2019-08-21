#ifndef __COMSERVER_H__
#define __COMSERVER_H__

#include "modbus.h"
#include "registers.h"
#include "./src/jsonPackage.h"
#include "HttpPost.h"

#define FRAME_HEAD_1		0x5A
#define FRAME_HEAD_2		0xA5
#define CMD_WRITE			0x82
#define CMD_READ			0x83
// 数据地址先定义为0xDFF0, 是IP地址
#define REG_ADD				0xDFF0


/*写数据的回复buf的顺序定义*/
// 5A A5 03 82 4F 4B
#define BUF_HEAD1			0
#define BUF_HEAD2			1
#define BUF_LENTH			2
#define BUF_CMD				3


// 信息打包类型
#define NOT_USED_MSG		0xFF		// 未使用
#define TRANS_MSG			0x00		// 透传命令
#define WRITE_MSG			0x01		// 写命令
#define READ_MSG			0x02		// 读命令



void cominit(void) ;

//向串口屏发送数据
int SendCom2WriteReg(UINT16 Addr, UINT8 Func);
UINT8 message_pack(UINT16 address,UINT8 msg_type,UINT8 *buf);



//发送写遥控寄存器
int SendCom1RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code);
#if 0
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
#endif

#endif
