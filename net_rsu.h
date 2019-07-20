#ifndef __NETRSU_H__
#define __NETRSU_H__

#include "registers.h"

#define NETWORK_FILE "/opt/config"

//***B9（心跳包）数据帧解析****
typedef struct AntennaInfoN_n{
			char Status;			//天线状态
			char Power;			//天线功率
			char Channel;			//天线信道
			char Control_state;		//天线控制状态
}ANTENNAINFON_N;				
typedef struct control_S{
	int Control_datetime[7];//日期时间
	char ControlCount;	//控制器数量
	char ControlStatusN[4];	//控制器状态
	char AntennaCount;	//天线数量
	AntennaInfoN_n  AntennaInfoN[8];	//天线数量
}RSUCONTROLER;
//************B0（设备状态）数据帧解析******************
typedef struct PSAMInfoN_n{
 char Psam_ID[8];
}PSAMINFON_N;
typedef struct RSU_data_init{
	char RSUManuID;		//路侧单元厂商代码
	char RSUID[3];		//路侧单元编号
	char RSUVersion[2];	//路侧单元软件版本号
	char ControlId;		//当前工作的天线控制盒Id
	char PSAMCount;		//PSAM数量
	PSAMInfoN_n PSAMInfoN[8];	//PSAM信息
}RSU_DATA_INIT;
//**********************D1（设备重启）数据帧解析***************
typedef struct RSU_reset{
	char AntennaCount;	//重启的天线序号
	char RSUState;		//天线是否重启状态  0：没有重启  1：系统已经重启
}RSU_RESET;

void init_net_rsu(void);
void send_RSU(char command,bool ReSend,char state,int num);


#endif


