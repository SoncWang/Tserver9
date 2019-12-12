#ifndef __ZTE_H__
#define __ZTE_H__


// 信息打包类型
#define NOT_USED_CMD		0xFF		// 未使用
#define DOOR_OPEN_CMD		0x01		// 开锁命令
#define DOOR_POLL_CMD		0x02		// 查询卡



#define FRONT_DOOR		0x01
#define BACK_DOOR		0x02

#define DOOR_POLL		0x0016
#define DOOR_OPEN		0x0005
#define DOOR_CLOSE		0x0006



int zteinit(void);

#pragma pack(push, 1)

typedef struct
{
  string TemplateName;//模板名
  string ObjId;//设备 ID

}TzteDev;





#pragma pack(pop)


#endif

