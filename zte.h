#ifndef __ZTE_H__
#define __ZTE_H__

int zteinit(void);

#pragma pack(push, 1)


// 信息打包类型
#define NOT_USED_CMD			0xFF		// 未使用
#define DOOR_ZTE_POLL_CMD		0x01		// 中兴查询卡
#define DOOR_JSA_POLL_CMD		0x02		// 金晟安查询卡
#define DOOR_ZTE_OPEN_CMD		0x03		// 中兴开锁命令
#define DOOR_JSA_OPEN_CMD		0x04		// 金晟安开锁命令
#define DOOR_ZTE_CLOSE_CMD		0x05		// 中兴关锁
#define DOOR_JSA_CLOSE_CMD		0x06		// JSA关锁
#define DOOR_JSA_AUTHOR_CMD		0x07		// JSA确权
#define DOOR_JSA_STATUS_CMD		0x08		// JSA读锁状态



#define DOOR_POLL_CMD			0x10	// 定义一个统一的接口，下面再区分zte还是JSA
#define DOOR_OPEN_CMD			0x20
#define DOOR_CLOSE_CMD			0x30



#define FRONT_DOOR		0x01
#define BACK_DOOR		0x02

#define ZTE_DOOR_POLL		0x0016		// 锁协议中的控制命令
#define ZTE_DOOR_OPEN		0x0005
#define ZTE_DOOR_CLOSE		0x0006

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


/*结构联合声明长整型*/
typedef union longlong_union
{
	UINT64 i;
	UINT8 b[8];
}LONGLONG_UNION;


bool card_id_parse(char *buf,int len,int seq);
void zte_locker_id_send_hook(int seq,UINT64 card_id);
bool zte_jsa_locker_process(int seq,UINT8 msg_type,UINT8 *pSend,string strDigestUser,string strDigestKey);
int zteinit(void);



#pragma pack(pop)


#endif

