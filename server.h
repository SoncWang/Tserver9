#ifndef __SERVER_H__
#define __SERVER_H__

#define NETCMD_HEADERLEN     13
#define NETPACKET_MAXLEN     1024*10
//#define IPV4_LEN 15

#define JSON_LEN 10*1024

#define NETCMD_MAGIC       0x12345678

typedef enum NETCMD_TYPE
{
    NETCMD_LOGIN      = 0,			//登录设备
    NETCMD_DATETIME   = 1,			//设置日期时间
    NETCMD_NETWORK    = 2,			//设置网络
    NETCMD_PING       = 3,       	//保持连接指令
    NETCMD_REBOOT     = 4,       	//重启设备指令
    NETCMD_CONFIG_NETWORK = 5,		//设置网络
    NETCMD_CONTROLERID= 6,			//获取设备ID
    NETCMD_CONFIG_PARA= 7,			//设置参数
    NETCMD_SEND_DATA  = 8,			//发送数据
    NETCMD_SEND_ENVI_PARAM = 9,		//环境寄存器参数
    NETCMD_SEND_UPS_PARAM = 10,		//UPS参数
    NETCMD_SEND_SPD_PARAM = 11,		//防雷器寄存器参数
    NETCMD_SEND_DEV_PARAM = 12,		//控制器参数
    NETCMD_SEND_AIR_PARAM = 13,		//空调参数
    NETCMD_SEND_RSU_PARAM = 14,		//RSU天线参数
    NETCMD_SEND_CAM_PARAM = 15,		//车牌识别参数
    NETCMD_SEND_SWITCH_INFO = 16,	//交换机状态
    NETCMD_FLAGRUNSTATUS  = 17,		//门架运行状态
    NETCMD_REMOTE_CONTROL = 18,		//遥控设备
    NETCMD_SWITCH_STATUS = 19,		//回路开关状态
	NETCMD_HWCABINET_STATUS = 20	//华为机柜状态
}NETCMD_TYPE;


typedef enum SFLAG_TYPE
{
	SFLAG_CMD    =   0,
	SFLAG_READ   =   1,
	SFLAG_WRITE  =   2,
	SFLAG_ERROR  =   255,
}SFLAG_TYPE;

typedef struct NETCMD_HEADER
{
	int magic;        
	int cmd;          
	int datalen;     
	char status;       
	char data[1];
}NETCMD_HEADER;

typedef struct
{
    char ip[20];
    char submask[20];
    char gateway_addr[20];
    char dns[20];
}IPInfo;

typedef struct _SOCKET_PARA
{
	int  fd;
	int  trigger;
	char IsQuit;
}SocketPara; 


void initServer(void);
int  MySendMessage(char *pBuf);
int  NetSendParm(NETCMD_TYPE cmd,char *pBuf,int len);
int Net_close();

//初始化利通定时推送线程
void init_LTKJ_DataPost();
//初始化新粤定时推送线程
void init_XY_DataPost();
//初始化定时取工控机状态线程
void init_HTTP_DataGet();
//初始化socket定时推送线程(推送给小槟)
void init_SocketNetSend();


#endif



