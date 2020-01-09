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
#include "config.h"
#include <pthread.h>
#include "main.h"
#include "AtlasServer.h"
#include "net_spd.h"
#include "ats.h"

#define WDT "/dev/watchdog"

#define DEFAULT_MIBDIRS "$HOME/.snmp/mibs:/usr/share/snmp/mibs:/opt/xr/9100dll/net-snmp-5.8/install/share/snmp/mibs/"

using namespace std;

SPD_PARAMS *stuSpd_Param;		//防雷器结构体
DEVICE_PARAMS *stuDev_Param[POWER_BD_MAX_NUM];	//装置参数寄存器
VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体
VA_METER_PARAMS *stuVA_Meter_Param[VA_METER_BD_MAX_NUM];		//伏安表电压电流结构体
REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
THUAWEIGantry HUAWEIDevValue;//华为机柜状态
THUAWEIALARM HUAWEIDevAlarm;		//华为机柜告警
THUAWEIGantry *stuHUAWEIDevValue;//华为机柜状态
RSUCONTROLER stuRsuControl[RSUCTL_NUM];	//RSU控制器状态
RSU_DATA_INIT stuRsuData[RSUCTL_NUM];	//RSU设备信息结构体
RSU_RESET stuRsuReset;			//天线软件复位状态结构体
ATLAS_STATE stuAtlasState[ATLAS_NUM]; //Atlas状态

LOCKER_HW_PARAMS *lockerHw_Param[LOCK_MAX_NUM];	//门锁状态结构体
VMCONTROL_STATE VMCtl_State;	//控制器运行状态结构体
VMCONTROL_STATE *stuVMCtl_State;	//控制器运行状态结构体

extern void initHUAWEIGantry();
extern void initHUAWEIALARM();

void WriteLog(char* str);

int WDTfd = -1;

int snmpdataTime = 0;
int litdataTime = 0;
extern pthread_mutex_t snmpdataMutex ;
extern pthread_mutex_t litdataMutex ;
pthread_mutex_t updataRebootMutex ;
extern pthread_mutex_t uprebootMutex ;

extern int zteinit(void);

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

    timeout = 120;
     printf("set timeout was is %d seconds\n", timeout);

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
	int i,j;
	unsigned int pos_cnt = 0;
	unsigned int temp = 0;
    sleep(3);
	//初始化看门狗
    WatchDogInit();

	//读设置文件
	GetConfig();

	//防雷器结构体
	stuSpd_Param = (SPD_PARAMS*)malloc(sizeof(SPD_PARAMS));
	memset(stuSpd_Param,0,sizeof(SPD_PARAMS));

	lockerDataInit(true);

	/////////////////  电压电流传感器配置开始  /////////////////////////////////////////////
	for (i = 0; i < VA_METER_BD_MAX_NUM; i++)
	{
		stuVA_Meter_Param[i] = (VA_METER_PARAMS*)malloc(sizeof(VA_METER_PARAMS));
		memset(stuVA_Meter_Param[i],0,sizeof(VA_METER_PARAMS));
		/*配置文件中是否有配置*/
		if (StrAdrrVAMeter[i].length() != 0)
		{
			stuVA_Meter_Param[i]->address = atoi(StrAdrrVAMeter[i].c_str());
			/*这里的Position要算上电子锁的实际数量*/
			Rs485_table_set(VA_METER_1+i, ENABLE,pos_cnt+actual_locker_num, stuVA_Meter_Param[i]->address);
			pos_cnt++;
		}
		else
		{
			//stuVA_Meter_Param[i] = NULL;		// 防止为野指针
			Rs485_table_set(VA_METER_1+i, DISABLE,NULL_VAR, NULL_VAR);
		}
	}
	/*如果配置文件里面没有配置,则默认配置2个电压电流传感器,分别为81,82*/
	if (pos_cnt == 0)
	{
		for (i = 0; i < 2; i++)
		{
			stuVA_Meter_Param[i] = (VA_METER_PARAMS*)malloc(sizeof(VA_METER_PARAMS));
			memset(stuVA_Meter_Param[i],0,sizeof(VA_METER_PARAMS));
			if (i == 0)
			{
				stuVA_Meter_Param[i]->address = VA_STATION_ADDRESS_1;
			}
			else
			{
				stuVA_Meter_Param[i]->address = VA_STATION_ADDRESS_2;
			}
			/*这里的Position要算上电子锁的实际数量*/
			Rs485_table_set(VA_METER_1+i, ENABLE,pos_cnt+actual_locker_num, stuVA_Meter_Param[i]->address);
			pos_cnt++;
		}
	}

	/*上移到电压电流传感器后*/
	/********    得到除电子锁外的所有485设备的轮询信息                         ***********************/
	/*动态开辟一个数组，并存储有效其它485设备的轮询配置*/
	actual_485dev_num = pos_cnt;
	//pos_cnt = 0;// 这里pos_cnt不要清0，后面还要用,要统计全部的485设备数量
//	printf("actual_485dev_num 0x%02x ",actual_485dev_num);printf("\r\n");
	polling_subarr = (int*)malloc(sizeof(int)*actual_485dev_num);
	/*暂时副轮询只针对电压电流传感器*/
	for (i=VA_METER_1,j=0; i < POWER_BD_1; i++)
	{
		if ((Var_Table[i].enable)&&(j<actual_485dev_num))
		{
			/*polling_subarr存储的是被使能的Var_Table的status,即顺序号*/
			polling_subarr[j] = Var_Table[i].status;
//			printf("pollingsubcnt 0x%02x ",polling_subarr[j]);printf("\r\n");
			j++;
		}
	}
	/////////////////  电压电流传感器配置开始完毕/////////////////////////////////////

	/////////////////  电源控制板配置开始	/////////////////////////////////////////////
	//装置参数寄存器,分为电源板和IO板
	temp = 0;	//统计到底有几个电源板
	for (i = 0; i < POWER_BD_MAX_NUM; i++)
	{
		/*配置文件中是否有配置*/
		if (StrAdrrPower[i].length() != 0)
		{
			stuDev_Param[i] = (DEVICE_PARAMS*)malloc(sizeof(DEVICE_PARAMS));
			memset(stuDev_Param[i],0,sizeof(DEVICE_PARAMS));
			stuDev_Param[i]->Address = atoi(StrAdrrPower[i].c_str());
			Rs485_table_set(POWER_BD_1+i, ENABLE,pos_cnt+actual_locker_num, stuDev_Param[i]->Address);
			pos_cnt++;
			temp++;
		}
		else
		{
			stuDev_Param[i] = NULL; 	// 防止为野指针
			Rs485_table_set(POWER_BD_1+i, DISABLE,NULL_VAR, NULL_VAR);
		}
	}
	/*如果配置表中没有配置，则默认配置1块电源板，地址为71*/
	if (temp == 0)
	{
		stuDev_Param[0] = (DEVICE_PARAMS*)malloc(sizeof(DEVICE_PARAMS));
		memset(stuDev_Param[0],0,sizeof(DEVICE_PARAMS));
		stuDev_Param[0]->Address = POWER_CTRL_ADDR_1;
		Rs485_table_set(POWER_BD_1, ENABLE,pos_cnt+actual_locker_num, stuDev_Param[0]->Address);
		pos_cnt++;
		temp++;
	}
	/////////////////  电源控制板配置结束	/////////////////////////////////////////////


	//控制器参数结构体
	stuVMCtl_Param = (VMCONTROL_PARAM*)malloc(sizeof(VMCONTROL_PARAM));
	memset(stuVMCtl_Param,0,sizeof(VMCONTROL_PARAM));
	//遥控寄存器结构体
	stuRemote_Ctrl = (REMOTE_CONTROL*)malloc(sizeof(REMOTE_CONTROL));
	memset(stuRemote_Ctrl,0,sizeof(REMOTE_CONTROL));
	//控制器运行状态结构体
	stuVMCtl_State = &VMCtl_State;

	/////////////////  DO配置表开始/////////////////////////////////////////////
	temp = 0;	//统计到底有几个DO
	for (i = 0; i < SWITCH_COUNT; i++)
	{
		/*配置文件中是否有配置*/
		if (StrDoSeq[i].length() != 0)
		{
			temp++; // 表明配置文件中有配置
			DoSeq[i] = atoi(StrDoSeq[i].c_str());
			if(DoSeq[i] > 0)
			{
				DoSeq[i]--; 		// 配置文件是从1~12, 标号是要减1
			}
		}
		else
		{
			DoSeq[i] = NULL_VAR;	// 未配置,不使用
		}
	}
	// 如果都没有配置，就按DO顺序进行默认配置
	if (temp == 0)
	{
		//printf("temp=0\r\n");
		for (i = 0; i < SWITCH_COUNT; i++)
		{
			DoSeq[i] = i;
		}
	}
	/////////////////  DO配置表结束/////////////////////////////////////////////

	//华为机柜状态结构体
	stuHUAWEIDevValue = &HUAWEIDevValue;
    initHUAWEIGantry();
	initHUAWEIALARM();

	write(WDTfd, "\0", 1);
	//初始化串口232
	cominit();
	//初始化串口485
	rs485init();
	write(WDTfd, "\0", 1);
	//初始化http服务端
	HttpInit();
	usleep(500000); //delay 500ms
	write(WDTfd, "\0", 1);
	//初始化服务器线程
	initServer();
	usleep(500000); //delay 500ms
	write(WDTfd, "\0", 1);
	//初始化RSU
	init_net_rsu();
	usleep(500000); //delay 500ms
	write(WDTfd, "\0", 1);
	//初始化RSU
	snmpInit();
	usleep(100000); //delay 100ms
	write(WDTfd, "\0", 1);

	//初始化防雷检测线程
	init_net_spd();
	usleep(500000); //delay 500ms
	write(WDTfd, "\0", 1);

	//初始化利通定时推送线程
//	init_LTKJ_DataPost();
	usleep(100000); //delay 100ms
	write(WDTfd, "\0", 1);

	//初始化获取摄像头状态
	IpCamServerInit();
	write(WDTfd, "\0", 1);
	//处理重启
	init_DealDoReset();
    //atlas
    AtlasInit();
    //飞达中兴
    zteinit();
    //爱特思
    atsInit();

    //初始化利通控制器状态获取线程
    init_lt_status();

    while(1)
    {
        write(WDTfd, "\0", 1);
		sleep(5);
//system("reboot") ;
/*        //判断snmp 获取机柜是否有数据返回 120分钟
        pthread_mutex_lock(&snmpdataMutex);
        if(++ snmpdataTime > 12*120)
        {
            pthread_mutex_unlock(&snmpdataMutex);
            //判断是否正在远程升级
            pthread_mutex_lock(&uprebootMutex);
            system("reboot") ;
            pthread_mutex_unlock(&uprebootMutex);
        }
        else
        {
            pthread_mutex_unlock(&snmpdataMutex);
        }

        //判断http 推送数据给路段站级服务器是否有返回 120分钟
        pthread_mutex_lock(&litdataMutex);
        if(++ litdataTime > 12*120)
        {
            pthread_mutex_unlock(&litdataMutex);
            //判断是否正在远程升级
            pthread_mutex_lock(&uprebootMutex);
            system("reboot") ;
            pthread_mutex_unlock(&uprebootMutex);
        }
        else
        {
            pthread_mutex_unlock(&litdataMutex);
        }*/
    }

	return 0;
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

	 //判断前一天文件是否存在，存在就先删除
	 if(pTM->tm_mday>1 && pTM->tm_mday<=31)
	 {
		 sprintf(stmp,"%d",pTM->tm_mday-1);
		 filename=exePath+"/"+stmp+".txt";
		 if((access(filename.c_str(),F_OK))!=-1)
		 {
			 printf("%s 存在\n",filename.c_str());
			 remove(filename.c_str());
		 }
	 }
	 else if(pTM->tm_mday==1)
	 {
		 filename=exePath+"/30.txt";
		 if((access(filename.c_str(),F_OK))!=-1)
		 {
			 printf("%s 存在\n",filename.c_str());
			 remove(filename.c_str());
		 }
		 filename=exePath+"/31.txt";
		 if((access(filename.c_str(),F_OK))!=-1)
		 {
			 printf("%s 存在\n",filename.c_str());
			 remove(filename.c_str());
		 }
	 }

	 //系统日期和时间,格式: yyyymmddHHMMSS
	 sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
			 pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
			 pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

/*	 sprintf(stmp,"%d",pTM->tm_mday);
	 filename=exePath+"/"+stmp+".txt";
	 fpLog = fopen(filename.c_str(), "a");

	 fseek(fpLog, 0, SEEK_END);
	 fprintf(fpLog, "%s->%s\n", sDateTime,str);*/
	 printf("%s-->%s",sDateTime,str);

//	 fclose(fpLog);
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

unsigned long GetTickCount() //返回毫秒
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

//  ----------------------------------------------------------------------------
/// 通用函数，对GetTickCount再次封装.
/// \return 32-bits timestamp.
//  ----------------------------------------------------------------------------
UINT32 timestamp_get(void)
{
    return GetTickCount();
}


//  ----------------------------------------------------------------------------
/// \brief  得到时间间隔.
//  ----------------------------------------------------------------------------
UINT32 timestamp_delta(UINT32 const timestamp)
{
    UINT32 now = timestamp_get();
    // The unsigned substraction takes care of one possible wrap-around, but no
    // more.
    return now - timestamp;
}

