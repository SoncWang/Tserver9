#ifndef __SERVER_H__
#define __SERVER_H__

#define NETCMD_HEADERLEN     13
#define NETPACKET_MAXLEN     1024*7
//#define IPV4_LEN 15


#define NETCMD_MAGIC       0x12345678

typedef enum NETCMD_TYPE
{
    NETCMD_LOGIN      = 0,			//鐧诲綍璁惧
    NETCMD_DATETIME   = 1,			//璁剧疆鏃ユ湡鏃堕棿
    NETCMD_NETWORK    = 2,			//璁剧疆缃戠粶
    NETCMD_PING       = 3,       	//淇濇寔杩炴帴鎸囦护
    NETCMD_REBOOT     = 4,       	//閲嶅惎璁惧鎸囦护
    NETCMD_STATE      = 5,		 	//鑾峰彇鐘舵€?
    NETCMD_CONTROLERID= 6,			//鑾峰彇璁惧ID
    NETCMD_CONFIG_PARA= 7,			//璁剧疆鍙傛暟
    NETCMD_SEND_DATA  = 8,			//鍙戦€佹暟鎹?
    NETCMD_SEND_ENVI_PARAM = 9,		//环境寄存器参数
	NETCMD_SEND_UPS_PARAM = 10,		//UPS参数
	NETCMD_SEND_SPD_PARAM = 11,		//防雷器寄存器参数
	NETCMD_SEND_DEV_PARAM = 12,		//装置参数寄存器
	NETCMD_SEND_DEV_INFO  = 13,		//装置信息参数
	NETCMD_SEND_RSU_PARAM = 14,		//装置RSU天线参数寄存器
	NETCMD_SET_DEV_PARAM  =	15,		//设置装置参数寄存器
	NETCMD_REMOTE_CONTROL = 16,		//遥控设备
	NETCMD_FLAGRUNSTATUS  = 17 		//门架运行状态
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

/*typedef struct _UDP_SOCK
{
	char machinecode[2];
	char   mac[6];
	int dhcpstatu;
	unsigned short httpport;
	unsigned int  serverport;
	struct in_addr ip;
	struct in_addr submask;
	struct in_addr gateway_addr;
	struct in_addr dns;	
}IPInfoReport;*/

typedef struct
{
    char ip[20];
    char submask[20];
    char gateway_addr[20];
    char dns[20];
}IPInfo;

typedef struct
{
    char server[20];           //服务器地址
    char ServerURL1[128];      //服务器1推送地址
    char ServerURL2[128];      //服务器2推送地址
    char ServerURL3[128];      //服务器3推送地址
    char StationURL[128];      //控制器接收地址
    char RSUIP[20];            //RSUIP地址
    char RSUPort[20];          //RSU端口
    char VehPlate1IP[20];      //识别仪1IP地址
    char VehPlate1Port[20];    //识别仪1端口
    char CAMIP[20];            //监控摄像头IP地址
    char CAMPort[20];          //监控摄像头端口

}VMCONTROL_PARAM;

typedef struct _SOCKET_PARA
{
	int  fd;
	int  trigger;
	char IsQuit;
}SocketPara;


typedef struct _SYSTEMTIME {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

void initServer(void);
int  MySendMessage(char *pBuf);
int  NetSendParm(NETCMD_TYPE cmd,char *pBuf,int len);
int Net_close();

//初始化利通定时推送线程
void init_LTKJ_DataPost();
//初始化新粤定时推送线程
void init_XY_DataPost();


#endif

