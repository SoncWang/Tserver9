#ifndef __ZTE_H__
#define __ZTE_H__

int zteinit(void);

#pragma pack(push, 1)


// 信息打包类型
#define NOT_USED_CMD		0xFF		// 未使用
#define DOOR_ZTE_OPEN_CMD		0x01		// 中兴开锁命令
#define DOOR_ZTE_POLL_CMD		0x02		// 中兴查询卡
#define DOOR_JSA_OPEN_CMD		0x03		// 中兴开锁命令
#define DOOR_JSA_POLL_CMD		0x04		// 中兴查询卡



#define FRONT_DOOR		0x01
#define BACK_DOOR		0x02

#define DOOR_POLL		0x0016
#define DOOR_OPEN		0x0005
#define DOOR_CLOSE		0x0006

#define DOOR_STATUS_ADDR	9	// 返回帧锁的状态在第几个字节
#define DOOR_ID_ADDR		10	// 返回帧锁的ID在第几个字节
#define DOOR_SOI_ADDR		0
#define DOOR_EOI_ADDR		25


typedef struct
{
  string TemplateName;//模板名
  string ObjId;//设备 ID

}TzteDev;

/*the struct definition of the LOCKER*/
typedef struct ZTE_locker_struct
{
	UINT16 status;		// 0:close 1:open
	UINT32 id;			// 4个字节卡号
}LOCKER_ZTE_PARAMS;


typedef struct
{
  string lockDevID;//锁ID
  string portID;//锁串口号
}LOCKER_ZTE_ID_PORT;


void zte_locker_id_send_hook(int seq);
void zte_locker_ctrl_process(int seq,UINT8 cmd,UINT8 *pSend,string strDigestUser,string strDigestKey);
int zteinit(void);



#pragma pack(pop)


#endif

