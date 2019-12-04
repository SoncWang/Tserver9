#ifndef __NETRSU_H__
#define __NETRSU_H__

#include "registers.h"

#define NETWORK_FILE "/opt/config"

//***B1（心跳包）数据帧解析****
typedef struct PSAM1_S{
		char Psam1_channel;
		char Psam1_status;	
}PSAM1_STU;
typedef struct PSAM2_S{
		char Psam2_channel;
		char Psam2_status;	
}PSAM2_STU;
typedef struct AntennaInfoN_n{
			char Rsu_id;			//天线ID编号
			char Control_state;		//天线控制状态
			char Channel;			//天线信道
			char Power;			//天线功率
			char send_status;		//1字节天线发射状态,00H正常，01H异常
			char recv_status;		//1字节天线接收状态，00H正常,01H异常
}ANTENNAINFON_N;				
typedef struct control_S{
	char ControlStatus_1;	//控制器1状态
	char ControlStatus_2;	//控制器2状态
	char ControlStatus_3;	//控制器之间的网络连接状态
	char Psam_num1;		//路侧单元控制器1PSAM数量,记为 n
	char Psam_num2;		//路侧单元控制器2PSAM数量,记为 m
	PSAM1_S Psam1[12];	//控制器1,PSAM卡状态
	PSAM2_S Psam2[12];	//控制器2,PSAM卡状态
	char AntennaCount;	//天线数量
	AntennaInfoN_n  AntennaInfoN[8];	//天线数量
}RSUCONTROLER;
//************B0（设备状态）数据帧解析******************
typedef struct PSAMInfoN_n{
	char PSAM_channel;	//1字节PSAM通道号
	char PSAM_version;	//1字节PSAM版本号
	char PSAM_auth;		//1字节PSAM授权状态00H已授权01H未授权
	char Psam_ID[6];	//6字节PSAM终端机编号
}PSAMINFON_N;
typedef struct RSU_data_init{
	char RSUStatus;		//路侧单元主状态参数;00:表示正常 ，否则表示异常 
	char PSAMCount;		//PSAM数量
	PSAMInfoN_n PSAMInfoN[8];	//PSAM信息
	char RSUAlgId;		//算法标识，默认填写00H
	char RSUManuID;		//路侧单元厂商代码
	char RSUID[3];		//路侧单元编号
	char RSUVersion[2];	//路侧单元软件版本号
	char Workstatus;	//工作模式返回状态，默认填写00H
	char FlagID[3];		//ETC门架编号（由C0 帧中获取,失败填充00H
	char Reserved[4];
}RSU_DATA_INIT;
//**********************D1（设备重启）数据帧解析***************
typedef struct RSU_reset{
	char AntennaCount;	//重启的天线序号
	char RSUState;		//天线是否重启状态  0：没有重启  1：系统已经重启
}RSU_RESET;

void init_net_rsu(void);
void send_RSU(char command,bool ReSend,char state,int num);


#endif


