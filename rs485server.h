#ifndef __RS485SERVER_H__
#define __RS485SERVER_H__

#include "modbus.h"
#include "registers.h"
#include "./src/jsonPackage.h"
#include "HttpPost.h"

#define DOOR_LOCK_ADDR		91			// 门锁地址
#define DOOR_LOCK_REG		0x4002 		// 电子门锁 开/关,直接对接锁的协议

#define	REMOTE_UNLOCK		0x33		// 华为协议，遥控解锁
#define	REMOTE_LOCK			0xCC		// 华为协议，遥控锁

#define	SINGLE_WRITE_HW			0x06		// 华为协议，写单个寄存器

void rs485init(void) ;
UINT16 SendCom4RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code);

#endif
