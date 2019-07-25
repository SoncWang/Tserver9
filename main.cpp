#include <stdlib.h>
#include <linux/watchdog.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>  
#include <sys/ioctl.h>
#include <string>
#include <sys/time.h>
#include <csignal>
#include <unistd.h>
#include <sys/stat.h>
#include "rs485server.h"

#include "main.h"

#define WDT "/dev/watchdog"

using namespace std;

ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
SPD_PARAMS *stuSpd_Param;		//防雷器结构体
DEVICE_PARAMS *stuDev_Param;	//装置参数寄存器
VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体
RSU_PARAMS *stuRSU_Param[VA_METER_BD_NUM];		//RSU天线信息结构体
REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
FLAGRUNSTATUS *stuFlagRunStatus;//门架自由流运行状态结构体
THUAWEIGantry HUAWEIDevValue;//华为机柜状态
THUAWEIALARM HUAWEIDevAlarm;		//华为机柜告警
THUAWEIGantry *stuHUAWEIDevValue;//华为机柜状态
RSUCONTROLER *stuRsuControl;	//RSU控制器状态
AIRCOND_PARAM *stuAirCondRead;		//读空调状态结构体
AIRCOND_PARAM *stuAirCondWrite;		//写空调状态结构体
LOCKER_HW_PARAMS *lockerHw_Param[LOCK_NUM];	//门锁状态结构体

extern string StrServerURL1;	//服务端URL1
extern string StrServerURL2;	//服务端URL2

extern string StrAdrrLock1;	//门锁1的地址
extern string StrAdrrLock2;	//门锁2的地址
extern string StrAdrrVAMeter1;	//电压电流传感器1的地址
extern string StrAdrrVAMeter2;	//电压电流传感器2的地址

extern void initHUAWEIGantry();
extern void initHUAWEIALARM();

/*char gIpAddr[20];
HANDLE g_hCamera;
int gCamStatus=CAMERA_STOP;
char gPlateNum[16];
ColorType gPlateColor;
void MYCameraJpegCallBackFunc(char * pBuffer,int size,IMAGE_CAPTURE_INFO *pImageInfo,DWORD nContext);
void MYCameraEventCallBackFunc(CAMERA_STATE cEvent,DWORD nContext);*/
void WriteLog(char* str);

int WDTfd = -1;

void WatchDogInit(void)
{
    //int WDTfd = -1;
    int timeout;

    WDTfd = open(WDT, O_WRONLY);
    if (WDTfd == -1)
    {
        printf("fail to open "WDT "!\n");
    }
    printf(WDT " is opened!\n");

    timeout = 60;
    ioctl(WDTfd, WDIOC_SETTIMEOUT, &timeout);

    ioctl(WDTfd, WDIOC_GETTIMEOUT, &timeout);
    printf("The timeout was is %d seconds\n", timeout);
    write(WDTfd, "\0", 1);

}

//定时中断处理
void sig_handler(int signo)
{
    printf("sig_handler\t\n");
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
	//初始化看门狗
    WatchDogInit();

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
	InitStuEnvi_Param(stuEnvi_Param);
	//USP结构体 电源数据
	stuUps_Param = (UPS_PARAMS*)malloc(sizeof(UPS_PARAMS));
	InitStuUPS_Param(stuUps_Param);
	//防雷器结构体
	stuSpd_Param = (SPD_PARAMS*)malloc(sizeof(SPD_PARAMS));
	memset(stuSpd_Param,0,sizeof(SPD_PARAMS));
	//装置参数寄存器
	stuDev_Param = (DEVICE_PARAMS*)malloc(sizeof(DEVICE_PARAMS));
	memset(stuDev_Param,0,sizeof(DEVICE_PARAMS));
	//控制器参数结构体
	stuVMCtl_Param = (VMCONTROL_PARAM*)malloc(sizeof(VMCONTROL_PARAM)); 
	memset(stuVMCtl_Param,0,sizeof(VMCONTROL_PARAM));
	//RSU天线信息结构体
	stuRSU_Param[0] = (RSU_PARAMS*)malloc(sizeof(RSU_PARAMS));
	memset(stuRSU_Param[0],0,sizeof(RSU_PARAMS));
	stuRSU_Param[0]->address = atoi(StrAdrrVAMeter1.c_str());

#if (VA_METER_BD_NUM >= 2)
	stuRSU_Param[1] = (RSU_PARAMS*)malloc(sizeof(RSU_PARAMS));
	memset(stuRSU_Param[1],0,sizeof(RSU_PARAMS));
	stuRSU_Param[1]->address = atoi(StrAdrrVAMeter2.c_str());
#endif
	//遥控寄存器结构体
	stuRemote_Ctrl = (REMOTE_CONTROL*)malloc(sizeof(REMOTE_CONTROL));
	memset(stuRemote_Ctrl,0,sizeof(REMOTE_CONTROL));
	//门架自由流运行状态结构体
	stuFlagRunStatus = (FLAGRUNSTATUS*)malloc(sizeof(FLAGRUNSTATUS));
	memset(stuFlagRunStatus,0,sizeof(FLAGRUNSTATUS));
	//华为机柜状态结构体
	stuHUAWEIDevValue = &HUAWEIDevValue;
	initHUAWEIGantry();
	initHUAWEIALARM();
	
	///RSU控制器状态
	stuRsuControl = (RSUCONTROLER*)malloc(sizeof(RSUCONTROLER));
	memset(stuRsuControl,0,sizeof(RSUCONTROLER));
	//读空调状态结构体
	stuAirCondRead = (AIRCOND_PARAM*)malloc(sizeof(AIRCOND_PARAM));
	memset(stuAirCondRead,0,sizeof(AIRCOND_PARAM));
	//写空调状态结构体
	stuAirCondWrite = (AIRCOND_PARAM*)malloc(sizeof(AIRCOND_PARAM));
	memset(stuAirCondWrite,0,sizeof(AIRCOND_PARAM));

	lockerHw_Param[0] = (LOCKER_HW_PARAMS*)malloc(sizeof(LOCKER_HW_PARAMS));
	memset(lockerHw_Param[0],0,sizeof(LOCKER_HW_PARAMS));
	lockerHw_Param[0]->address = atoi(StrAdrrLock1.c_str());

#if (LOCK_NUM >= 2)
	lockerHw_Param[1] = (LOCKER_HW_PARAMS*)malloc(sizeof(LOCKER_HW_PARAMS));
	memset(lockerHw_Param[1],0,sizeof(LOCKER_HW_PARAMS));
	lockerHw_Param[1]->address = atoi(StrAdrrLock2.c_str());
#endif

	//初始化串口
	cominit();
	rs485init();

	lockerPollingInit();

	//初始化http服务端
	HttpInit();
	usleep(500000); //delay 500ms

	//初始化服务器线程
	initServer();
	usleep(500000); //delay 500ms
	
	//初始化RSU
	init_net_rsu();
	usleep(500000); //delay 500ms
	
	//初始化RSU
	snmpInit();
	usleep(100000); //delay 100ms

	//初始化利通定时推送线程
	init_LTKJ_DataPost();
	usleep(100000); //delay 100ms

	//初始化新粤定时推送线程
	init_XY_DataPost();
	usleep(100000); //delay 100ms

    while(1)
    {
       write(WDTfd, "\0", 1);
       sleep(5);
    }

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

void InitStuEnvi_Param(ENVI_PARAMS *pParam)
{
	pParam->temp=0x7fff;	// 当前环境温度值300 ×10
	pParam->moist=0x7fff;	// 当前环境湿度值301 ×10
	pParam->water_flag=0x7fff;	// 漏水302
	pParam->front_door_flag=0x7fff;			//前柜门开关状态303
	pParam->back_door_flag=0x7fff;		//后柜门开关状态304
	pParam->door_overtime=0x7fff;		//柜门开启超时305
	pParam->smoke_event_flag=0x7fff;	//烟雾报警306
	pParam->Reserve1=0x7fff;			//保留1 307
	pParam->Reserve2=0x7fff; 			//保留2 308
	pParam->Reserve3=0x7fff;			//保留1 309
	pParam->air_cond_status=0x7fff;		//空调状态310
	pParam->air_cond_fan_in_status=0x7fff;			//空调内风机状态311
	pParam->air_cond_fan_out_status=0x7fff;			//空调外风机状态312
	pParam->air_cond_comp_status=0x7fff;			//空调压缩机状态313
	pParam->air_cond_heater_status=0x7fff;			//空调电加热状态314
	pParam->air_cond_fan_emgy_status=0x7fff;		//空调应急风机状态315
	pParam->air_cond_temp_out=0x7fff;		//当前空调室外温度值316 ×10
	pParam->air_cond_temp_in=0x7fff;		//当前空调室内温度值317 ×10
	pParam->air_cond_amp=0x7fff;					//当前空调电流值318 ×1000
	pParam->air_cond_volt=0x7fff;					//当前空调电压值319 ×1
	
	pParam->air_cond_hightemp_alarm=0x7fff;			//空调高温告警320
	pParam->air_cond_lowtemp_alarm=0x7fff;			//空调低温告警321
	pParam->air_cond_highmoist_alarm=0x7fff;		//空调高湿告警322
	pParam->air_cond_lowmoist_alarm=0x7fff;			//空调低湿告警323
	pParam->air_cond_infan_alarm=0x7fff;			//空调内风机故障324
	pParam->air_cond_outfan_alarm=0x7fff;			//空调外风机故障325
	pParam->air_cond_comp_alarm=0x7fff;				//空调压缩机故障326
	pParam->air_cond_heater_alarm=0x7fff;			//空调电加热故障327
	pParam->air_cond_emgyfan_alarm=0x7fff;			//空调应急风机故障328

}


void InitStuUPS_Param(UPS_PARAMS *pParam)
{
	// 输入数据
	pParam->in_phase_num=0x7fff;		// 相数 40
	pParam->in_freq=0x7fff; 		// 交流输入频率	41 ×100
	pParam->volt_Ain=0x7fff;		//交流输入相电压A 42 ×10
	pParam->volt_Bin=0x7fff;		//交流输入相电压B 43 ×10
	pParam->volt_Cin=0x7fff;		//交流输入相电压C 44 ×10

	pParam->amp_Ain=0x7fff; 		//交流输入相电流A	  45 ×10
	pParam->amp_Bin=0x7fff; 		//交流输入相电流B	  46 ×10
	pParam->amp_Cin=0x7fff; 		//交流输入相电流C	  47 ×10

	pParam->fact_Ain=0x7fff;		// 功率因素
	pParam->fact_Bin=0x7fff;		// 功率因素
	pParam->fact_Cin=0x7fff;		// 功率因素

	pParam->bypass_voltA=0x7fff;	// 旁路电压A
	pParam->bypass_voltB=0x7fff;	// 旁路电压B
	pParam->bypass_voltC=0x7fff;	// 旁路电压C
	pParam->bypass_freq=0x7fff; // 旁路频率
	//输出数据
	pParam->out_phase_num=0x7fff;		// 输出相数 48
	pParam->out_freq=0x7fff;			//UPS交流输出频率 49		×100
	pParam->volt_Aout=0x7fff;		//交流输出相电压A 50 ×10
	pParam->volt_Bout=0x7fff;		//交流输出相电压B 51 ×10
	pParam->volt_Cout=0x7fff;		//交流输出相电压C 52 	 ×10
	pParam->amp_Aout=0x7fff;		//交流输出相电流A 53 ×10
	pParam->amp_Bout=0x7fff;		//交流输出相电流B 54 ×10
	pParam->amp_Cout=0x7fff;		//交流输出相电流C 55 ×10

/*	pParam->fact_Aout=0x7fff;
	pParam->fact_Bout=0x7fff;
	pParam->fact_Cout=0x7fff;*/
	pParam->kw_Aout=0x7fff; 	// 有功
	pParam->kw_Bout=0x7fff; 	// 有功
	pParam->kw_Cout=0x7fff; 	// 有功
	pParam->kva_Aout=0x7fff;		// 视在
	pParam->kva_Bout=0x7fff;		// 视在
	pParam->kva_Cout=0x7fff;		// 视在

	pParam->load_Aout=0x7fff;		// 负载
	pParam->load_Bout=0x7fff;		// 负载
	pParam->load_Cout=0x7fff;		// 负载
	
	//电池参数
	pParam->running_day=0x7fff; 		// UPS运行时间 56 天
	pParam->battery_volt=0x7fff;		//UPS电池电压	57 ×10
	pParam->amp_charge=0x7fff;			//UPS充电电流 58 ×100
	pParam->amp_discharge=0x7fff;		//UPS放电电流
	pParam->battery_left=0x7fff;		//UPS电池后备时间 59 ×10，分钟
	pParam->battery_tmp=0x7fff; 		// 环境温度 60 ×10
	pParam->battery_capacity=0x7fff;	//电池当前容量 61 ×100%
	pParam->battery_dischg_times=0x7fff;//电池放电次数 62

	//USP状态参数
	pParam->supply_out_status=0x7fff;		// 输出供电状态 63
	pParam->supply_in_status=0x7fff;		// 输入供电状态 64
	pParam->battery_status=0x7fff;			// 电池状态 65

	//USP告警
	pParam->main_abnormal=0x7fff;			// 主路异常 66 0x00：正常 0xF0：异常
	pParam->system_overtemp=0x7fff; 		// 系统过温, 67 0x00：正常 0xF0：异常
	pParam->sysbat_low_prealarm=0x7fff; 	// 系统电池电量低预告警,68 0x00：正常 0xF0：异常
	pParam->rectifier_overload=0x7fff;		// 整流器过载,69 0x00：正常 0xF0：异常
	pParam->inverter_overload=0x7fff;		// 逆变器过载,70 0x00：正常 0xF0：异常
	pParam->bypass_abnomal=0x7fff;			// 旁路异常,71 0x00：正常 0xF0：异常
	pParam->battery_low_prealarm=0x7fff;	// 电池电压低,72 0x00：正常 0xF0：异常
	pParam->battery_abnomal=0x7fff; 		// 电池电压异常,73 0x00：正常 0xF0：异常
	pParam->battery_overtemp=0x7fff;		// 电池过温,74 0x00：正常 0xF0：异常
	pParam->fan_abnormal=0x7fff;			// 风扇故障,75 0x00：正常 0xF0：异常
	pParam->shutdown_alarm=0x7fff;			//紧急关机告警,76 0x00：正常 0xF0：异常
	pParam->maintain_status=0x7fff; 		//维修模式,77 0x00：正常 0xF0：异常
	pParam->inverter_supply=0x7fff; 		//电池逆变供电,78 0x00：正常 0xF0：异常
	pParam->bypass_supply=0x7fff;			//旁路供电,79 0x00：正常 0xF0：异常
}

void WriteLog(char* str)
 {
	 char sDateTime[30],stmp[10];
	 FILE *fpLog;
	 string exePath,filename;
	 exePath="logs";
	 if(access(exePath.c_str(),0) == -1)
	 	mkdir(exePath.c_str(),0755);
	 
	 time_t nSeconds;
	 struct tm * pTM;
	 
	 time(&nSeconds);
	 pTM = localtime(&nSeconds);
	 
	 //系统日期和时间,格式: yyyymmddHHMMSS 
	 sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
			 pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
			 pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
	 
	 sprintf(stmp,"%d",pTM->tm_mday);	 
	 filename=exePath+"/"+stmp+".txt";
	 fpLog = fopen(filename.c_str(), "a");
	 
	 fseek(fpLog, 0, SEEK_END);
	 fprintf(fpLog, "%s->%s\n", sDateTime,str);
	 printf("%s-->%s",sDateTime,str);	 
	 
	 fclose(fpLog);
 }
 
 void myprintf(char* str)
  {
	  char sDateTime[30],stmp[10];
	  time_t nSeconds;
	  struct tm * pTM;
	  
	  time(&nSeconds);
	  pTM = localtime(&nSeconds);
	  
	  //系统日期和时间,格式: yyyymmddHHMMSS 
	  sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
			  pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
			  pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
	  
	  printf("%s-->%s",sDateTime,str);	  
  }


