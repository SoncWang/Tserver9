#include <stdlib.h>
#include <linux/watchdog.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>
#include <sys/time.h>
#include <csignal>

#include "main.h"
#include "rs485server.h"

using namespace std;

ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
SPD_PARAMS *stuSpd_Param;		//防雷器结构体
DEVICE_PARAMS *stuDev_Param;	//装置参数寄存器
DeviceInfoParams *stuDev_Info;	//采集器设备信息结构体
RSU_PARAMS *stuRSU_Param[VA_METER_BD_NUM];		//RSU天线信息结构体
REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
FLAGRUNSTATUS *stuFlagRunStatus;//门架自由流运行状态结构体
//THUAWEIGantry *stuHUAWEIDevValue;//华为机柜状态
RSUCONTROLER *stuRsuControl;	//RSU控制器状态
LOCKER_HW_PARAMS *lockerHw_Param[LOCK_NUM];


extern string StrServerURL1;	//服务端URL1
extern string StrServerURL2;	//服务端URL2

/*char gIpAddr[20];
HANDLE g_hCamera;
int gCamStatus=CAMERA_STOP;
char gPlateNum[16];
ColorType gPlateColor;
void MYCameraJpegCallBackFunc(char * pBuffer,int size,IMAGE_CAPTURE_INFO *pImageInfo,DWORD nContext);
void MYCameraEventCallBackFunc(CAMERA_STATE cEvent,DWORD nContext);*/
void WriteLog(char* str);

int wdt_fd = -1;

void WatchDogInit(void)
{
   // int wdt_fd = -1;
    int timeout;

    wdt_fd = open("/dev/watchdog", O_WRONLY);
    if (wdt_fd == -1)
    {
        printf("fail to open /dev/watchdog!\n");
    }
    printf("/dev/watchdog is opened!\n");

    timeout = 60;
    ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);

    ioctl(wdt_fd, WDIOC_GETTIMEOUT, &timeout);
    printf("The timeout was is %d seconds\n", timeout);
}

//定时中断处理
void sig_handler(int signo)
{
    printf("sig_handler\t\n");

/*	//将门架运行参数转成JSON字符串
	char * jsonPack=(char *)malloc(50*1024);
	FLAGRUNSTATUS *pFlagRunStatus;
	int jsonPackLen=0;
	jsonStrFlagRunStatusWriter((char*)pFlagRunStatus,jsonPack,&jsonPackLen);
	HttpPostParm(jsonPack,jsonPackLen);
	free(jsonPack);*/
}

//初始化定时器
void InitTimer(void)
{
     struct timeval interval;
     struct itimerval timer;
     //设置时间间隔为10秒
     interval.tv_sec = 10;
	 interval.tv_usec =0;

     timer.it_interval = interval;
     timer.it_value = interval;

     setitimer(ITIMER_VIRTUAL, &timer, NULL);//让它产生SIGVTALRM信号

     //为SIGVTALRM注册信号处理函数
     signal(SIGALRM, sig_handler);
}

int main(void)
{
	char ch;
    int loop=0;
	char str[100];
	char * jsonPack=(char *)malloc(50*1024);
	REMOTE_CONTROL *pRCtrl;
	int jsonPackLen=0;
	FLAGRUNSTATUS *pFlagRunStatus;

	//读设置文件
	GetConfig();

	// 环境数据结构体
	stuEnvi_Param = (ENVI_PARAMS*)malloc(sizeof(ENVI_PARAMS));
	memset(stuEnvi_Param,0,sizeof(ENVI_PARAMS));
	//USP结构体 电源数据
	stuUps_Param = (UPS_PARAMS*)malloc(sizeof(UPS_PARAMS));
	memset(stuUps_Param,0,sizeof(UPS_PARAMS));
	//防雷器结构体
	stuSpd_Param = (SPD_PARAMS*)malloc(sizeof(SPD_PARAMS));
	memset(stuSpd_Param,0,sizeof(SPD_PARAMS));
	//装置参数寄存器
	stuDev_Param = (DEVICE_PARAMS*)malloc(sizeof(DEVICE_PARAMS));
	memset(stuDev_Param,0,sizeof(DEVICE_PARAMS));
	//采集器设备信息结构体
	stuDev_Info = (DeviceInfoParams*)malloc(sizeof(DeviceInfoParams));
	memset(stuDev_Info,0,sizeof(DeviceInfoParams));
	//RSU天线信息结构体
	stuRSU_Param[0] = (RSU_PARAMS*)malloc(sizeof(RSU_PARAMS));
	memset(stuRSU_Param[0],0,sizeof(RSU_PARAMS));

	#if (VA_METER_BD_NUM >= 2)
	stuRSU_Param[1] = (RSU_PARAMS*)malloc(sizeof(RSU_PARAMS));
	memset(stuRSU_Param[1],0,sizeof(RSU_PARAMS));
	#endif

	//遥控寄存器结构体
	stuRemote_Ctrl = (REMOTE_CONTROL*)malloc(sizeof(REMOTE_CONTROL));
	memset(stuRemote_Ctrl,0,sizeof(REMOTE_CONTROL));
	//门架自由流运行状态结构体
	stuFlagRunStatus = (FLAGRUNSTATUS*)malloc(sizeof(FLAGRUNSTATUS));
	memset(stuFlagRunStatus,0,sizeof(FLAGRUNSTATUS));
	//华为机柜状态结构体
//	stuHUAWEIDevValue = (THUAWEIGantry*)malloc(sizeof(THUAWEIGantry));
//	memset(stuHUAWEIDevValue,0,sizeof(THUAWEIGantry));
	///RSU控制器状态
	stuRsuControl = (RSUCONTROLER*)malloc(sizeof(RSUCONTROLER));
	memset(stuRsuControl,0,sizeof(RSUCONTROLER));

	lockerHw_Param[0] = (LOCKER_HW_PARAMS*)malloc(sizeof(LOCKER_HW_PARAMS));
	memset(lockerHw_Param[0],0,sizeof(LOCKER_HW_PARAMS));

	#if (LOCK_NUM >= 2)
	lockerHw_Param[1] = (LOCKER_HW_PARAMS*)malloc(sizeof(LOCKER_HW_PARAMS));
	memset(lockerHw_Param[1],0,sizeof(LOCKER_HW_PARAMS));
	#endif

	//system("hwclock 鈥搒");  //鍐欏埌纭椂閽?
	//初始化串口
	cominit();
	rs485init();

	//初始化http服务端
	HttpInit();

	//初始化服务器线程
	initServer();

//    WatchDogInit();

	//初始化宇视摄像机
//	IpcamInit();

	//初始化定时器
//	InitTimer();

	//初始化RSU
	init_net_rsu();

	//初始化RSU
	snmpInit();

	//初始化利通定时推送线程
	//init_LTKJ_DataPost();

	//初始化新粤定时推送线程
	//init_XY_DataPost();

	/*A fixed timer to polling the LOCKER*/
	lockerPollingInit();


/*    while(1)
    {
		usleep(5000);//delay 5ms
	}*/
	printf("输入提示 : \n 'v' 发送版本号\n 'e' 查询环境参数\n 'u' 查询UPS参数\n 's' 查询防雷器参数\n ");
	printf("'d' 查询装置参数\n 'i' 查询装置信息\n 'r' 查询RSU天线参数\n 'c' 电子门锁关\n 'o' 电子门锁开\n ");
	printf("'h' 推送门架运行状态\n 't' RSU下发数据\n 'q' 退出\n");
    while('q' != (ch = getchar()))
    {
        switch(ch)
        {
//        case 't' :  //触发抓拍
//			TrigerImage(g_hCamera);
//			break;
        case 'v' :  //发送字符串
			sprintf(str,"MySendMessage! 广东利通科技投资有限公司：%d\n",loop/10);
			printf(str);
			MySendMessage(str);

			break;
		case 'e' :	//查询环境变量寄存器
			//SendCom1QueryEvnReg();
			SendCom1ReadReg(0x01,READ_REGS,ENVI_START_ADDR,ENVI_REG_MAX);
			break;
		case 'u' :	//查询UPS变量寄存器
			SendCom1ReadReg(0x01,READ_REGS,UPS_START_ADDR,UPS_REG_MAX);
			break;
		case 's' :	//查询SPD变量寄存器
			SendCom1ReadReg(0x01,READ_REGS,SPD_START_ADDR,SPD_REG_MAX);
			break;
		case 'd' :	//查询装置参数寄存器
			SendCom1ReadReg(0x01,READ_REGS,PARAMS_START_ADDR,PARAMS_REG_MAX);
			break;
		case 'i' :	//查询装置信息寄存器
			SendCom1ReadReg(0x01,READ_REGS,DEVICEINFO_START_ADDR,DEVICEINFO_REG_MAX);
			break;
		case 'r' :	//查询RSU天线参数寄存器
			SendCom1ReadReg(0x01,READ_REGS,RSU_START_ADDR,RSU_REG_MAX);
			break;
		case 'c' :	//遥控寄存器 锁门
			memset(jsonPack,0,1024);
			pRCtrl=stuRemote_Ctrl;
//			pRCtrl->Door_Lock=0xFF00;  //电子门锁关
//			pRCtrl->Door_UnLock=0x0000;  //电子门锁开
			pRCtrl->RSU1_Close=0xFF00;  //RSU天线1遥合执行
			pRCtrl->RSU1_Open=0x0000;  //RSU天线1遥分执行
			jsonStrRemoteCtrlWriter((char*)pRCtrl,jsonPack,&jsonPackLen);
			NetSendParm(NETCMD_REMOTE_CONTROL,jsonPack,jsonPackLen);
			break;
		case 'o' :	//遥控寄存器 开门
			memset(jsonPack,0,1024);
			pRCtrl=stuRemote_Ctrl;
//			pRCtrl->Door_Lock=0x0000;  //电子门锁关
//			pRCtrl->Door_UnLock=0xFF00;  //电子门锁开
			pRCtrl->RSU1_Close=0x0000;  //RSU天线1遥合执行
			pRCtrl->RSU1_Open=0xFF00;  //RSU天线1遥分执行
			jsonStrRemoteCtrlWriter((char*)pRCtrl,jsonPack,&jsonPackLen);
			NetSendParm(NETCMD_REMOTE_CONTROL,jsonPack,jsonPackLen);
			break;
		case 'h' :	//推送门架运行状态
			memset(jsonPack,0,50*1024);
			SetjsonTableStr("flagrunstatus",jsonPack,&jsonPackLen);
//			SetjsonFlagRunStatusStr(jsonPack,&jsonPackLen);
			printf("%s",jsonPack);
			HttpPostParm(StrServerURL1,jsonPack,jsonPackLen);
			NetSendParm(NETCMD_FLAGRUNSTATUS,jsonPack,jsonPackLen);
			break;
		case 't' :	//RSU下发数据C0帧
			send_RSU(0xC4,0 ,1,1);
			break;
		case 'y' :	//测试开锁
			SendCom4RCtlReg(DOOR_LOCK_ADDR_1,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
			break;
		case 'z' :	//测试锁
			SendCom4RCtlReg(DOOR_LOCK_ADDR_1,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
			break;
		case 'a' :	//测试开锁
			SendCom4RCtlReg(DOOR_LOCK_ADDR_2,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
			break;
		case 'b' :	//测试锁
			SendCom4RCtlReg(DOOR_LOCK_ADDR_2,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
			break;
		case 'q' : //退出
			break;
		}
	}
//	ReleaseCamera(g_hCamera);
	Net_close();
	free(jsonPack);
	return 0;
}


/*void InitCamera(void)
{
	sprintf(gIpAddr,"192.192.2.139");
	g_hCamera=CreateCamera();
	SetCameraCallBack(g_hCamera,MYCameraJpegCallBackFunc,MYCameraEventCallBackFunc,(DWORD)g_hCamera);
	ConnectCamera(g_hCamera,gIpAddr,DBVT_MYPORT);
	ConnectDataCamera(g_hCamera,gIpAddr);
}

//视频显示线程(JPEG)
void MYCameraJpegCallBackFunc(char * pBuffer,int size,IMAGE_CAPTURE_INFO *pImageInfo,DWORD nContext)
{
	char str[100],strPlatePos[100];
	int nPlatePosStrLen=0;

	//存储JPEG图片
	memcpy(gPlateNum,pImageInfo->nCarSpeed,sizeof(char)*12);		   //车牌
	gPlateColor=(ColorType)pImageInfo->nLimitSpeed[0];
	printf("MYCameraJpegCallBackFunc plate=%s, color=%d\n",gPlateNum,gPlateColor);

	FILE *ExprssWayFile;
	ExprssWayFile = fopen("capture.jpg","wb");
	if(ExprssWayFile!=NULL)
	{
		fwrite(pBuffer,1,size,ExprssWayFile);
	}
	fclose(ExprssWayFile);
}

void MYCameraEventCallBackFunc(CAMERA_STATE cEvent,DWORD nContext)
{
	HANDLE hCamera = (HANDLE)nContext;
    if(cEvent==CAMERA_LINKED)
    {
        gCamStatus=CAMERA_LINKED;
		printf("CAMERA_LINKED\n");
    }

    if(cEvent==CAMERA_BREAK)
    {
        gCamStatus=CAMERA_BREAK;
		printf("CAMERA_BREAK\n");
    }
}

void WriteLog(char* str)
{
	printf("%s\n",str);
}*/


