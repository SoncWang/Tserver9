#ifndef __RS485SERVER_H__
#define __RS485SERVER_H__

#include "modbus.h"
#include "registers.h"
#include "./src/jsonPackage.h"
#include "HttpPost.h"

#define LOCK_NUM			1
#define VA_METER_BD_NUM		0


#define CARD_NUM		5	// ��ʱΪ5�ſ�

#if (LOCK_NUM >= 1)
#define DOOR_LOCK_ADDR_1		91			// address of the LOCKER
#endif
#if (LOCK_NUM >= 2)
#define DOOR_LOCK_ADDR_2		92			// address of the LOCKER
#endif

/*REMOTE CONTROL definition*/
#define	SINGLE_WRITE_HW		0x06	// function of the LOCKER
#define DOOR_LOCK_REG		0x4002 		// remote control

#define	REMOTE_UNLOCK		0x33		// OPEN CMD of the locker
#define	REMOTE_LOCK			0xCC		// CLOSE CMD of the locker


/*Status reading definition*/
#define DOOR_STATUS_REG			0x1000 		// status reading
#define LOCKER_ID_LENTH			10		// it only use 10 bytes from the ID card.
#define LOCKER_REG_NUM			9		// 4+5(��)
#define FRAME_HEAD_NUM 			3		/*������ʱ����֡��Ч����ǰ���ݸ���*/

#define INTERVAL_TIME		700000



/*�ṹ������������*/
typedef union int_union
{
	UINT16 i;
	UINT8 b[2];
}INTEGER_UNION;

/*�ṹ��������������*/
typedef union long_union
{
	UINT32 i;
	UINT8 b[4];
}LONG_UNION;



/*the struct definition of the LOCKER*/
typedef struct locker_struct
{
	UINT16 status;		// 0:close 1:open
	UINT16 open_reason;	// 0:close 1:cmd 2:reserve 3:key
	UINT16 report_cnt;	// >128 it will be cleared.
	UINT16 id_length;
	UINT8 id[96];
}LOCKER_HW_PARAMS;

//485��ѯ����ö��,����չ
typedef enum
{
	#if (LOCK_NUM >= 1)
	LOCKER_1_STATUS = 0,
	#endif

	#if (LOCK_NUM >= 2)
	LOCKER_2_STATUS,
	#endif

	#if (VA_METER_BD_NUM >= 1)
	VOLT_AMP_GET_FLAG_1,
	#endif

	#if (VA_METER_BD_NUM >= 2)
	VOLT_AMP_GET_FLAG_2,
	#endif

	#if (VA_METER_BD_NUM >= 3)
	VOLT_AMP_GET_FLAG_3,
	#endif

	POLLING_NUM		// 4
}POLLING_LIST;




void rs485init(void) ;
UINT16 SendCom4RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code);
int DealComm485(unsigned char *buf,unsigned short int len);
void lockerPollingInit();


#endif
