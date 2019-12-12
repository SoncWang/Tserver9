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

#define WDT "/dev/watchdog"

#define DEFAULT_MIBDIRS "$HOME/.snmp/mibs:/usr/share/snmp/mibs:/opt/xr/9100dll/net-snmp-5.8/install/share/snmp/mibs/"

using namespace std;

ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
SPD_PARAMS *stuSpd_Param;		//防雷器结构体
DEVICE_PARAMS *stuDev_Param[POWER_BD_MAX_NUM];	//装置参数寄存器
VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体
VA_METER_PARAMS *stuVA_Meter_Param[VA_METER_BD_MAX_NUM];		//伏安表电压电流结构体
REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
FLAGRUNSTATUS *stuFlagRunStatus;//门架自由流运行状态结构体
THUAWEIGantry HUAWEIDevValue;//华为机柜状态
THUAWEIALARM HUAWEIDevAlarm;		//华为机柜告警
THUAWEIGantry *stuHUAWEIDevValue;//华为机柜状态
RSUCONTROLER stuRsuControl;	//RSU控制器状态
RSU_DATA_INIT stuRsuData[2];	//RSU设备信息结构体
RSU_RESET stuRsuReset;			//天线软件复位状态结构体
//SPD_AI_PARAMS stuSpd_AI_Param;	//防雷器结构体

AIRCOND_PARAM *stuAirCondRead;		//读空调状态结构体
AIRCOND_PARAM *stuAirCondWrite;		//写空调状态结构体
LOCKER_HW_PARAMS *lockerHw_Param[LOCK_MAX_NUM];	//门锁状态结构体

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
	
	// 环境数据结构体
	stuEnvi_Param = (ENVI_PARAMS*)malloc(sizeof(ENVI_PARAMS));
	InitStuEnvi_Param(stuEnvi_Param);
	//USP结构体 电源数据
	stuUps_Param = (UPS_PARAMS*)malloc(sizeof(UPS_PARAMS));
	InitStuUPS_Param(stuUps_Param);
	//防雷器结构体
	stuSpd_Param = (SPD_PARAMS*)malloc(sizeof(SPD_PARAMS));
	memset(stuSpd_Param,0,sizeof(SPD_PARAMS));

	lockerDataInit(true);
#if 0
	//电子锁参数配置
	for (i = 0; i < LOCK_MAX_NUM; i++)
	{
		lockerHw_Param[i] = (LOCKER_HW_PARAMS*)malloc(sizeof(LOCKER_HW_PARAMS));
		memset(lockerHw_Param[i],0,sizeof(LOCKER_HW_PARAMS));
		/*配置文件中是否有配置*/
		if (StrAdrrLock[i].length() != 0)
		{
			lockerHw_Param[i]->address = atoi(StrAdrrLock[i].c_str());
			//更新配置表
			Rs485_table_set(LOCKER_1+i, ENABLE,pos_cnt++, lockerHw_Param[i]->address);
		}
		else
		{
			//lockerHw_Param[i] = NULL;		// 防止为野指针
			Rs485_table_set(LOCKER_1+i, DISABLE,NULL_VAR, NULL_VAR);
		}
	}
	actual_locker_num = pos_cnt;
	pos_cnt = 0;
//	printf("actual_locker_num 0x%02x ",actual_locker_num);printf("\r\n");

	/*动态开辟一个数组，并存储有效电子锁轮询配置*/
	polling_arr = (int*)malloc(sizeof(int)*actual_locker_num);
	for (i = 0,j=0; i <= LOCKER_4; i++)
	{
		if ((Var_Table[i].enable)&&(j<actual_locker_num))
		{
			polling_arr[j] = Var_Table[i].status;
//			printf("pollingcnt 0x%02x ",polling_arr[j]);printf("\r\n");
			j++;
		}
	}
#endif

	/////////////////  电压电流传感器配置开始  /////////////////////////////////////////////
	for (i = 0; i < VA_METER_BD_MAX_NUM; i++)
	{
		/*配置文件中是否有配置*/
		if (StrAdrrVAMeter[i].length() != 0)
		{
			stuVA_Meter_Param[i] = (VA_METER_PARAMS*)malloc(sizeof(VA_METER_PARAMS));
			memset(stuVA_Meter_Param[i],0,sizeof(VA_METER_PARAMS));
			stuVA_Meter_Param[i]->address = atoi(StrAdrrVAMeter[i].c_str());
			/*这里的Position要算上电子锁的实际数量*/
			Rs485_table_set(VA_METER_1+i, ENABLE,pos_cnt+actual_locker_num, stuVA_Meter_Param[i]->address);
			pos_cnt++;
		}
		else
		{
			stuVA_Meter_Param[i] = NULL;		// 防止为野指针
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


	/*打印485配置表的调试信息
	printf("LOCKER_1=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[LOCKER_1].status, Var_Table[LOCKER_1].enable,Var_Table[LOCKER_1].position,Var_Table[LOCKER_1].addr);
	printf("LOCKER_2=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[LOCKER_2].status, Var_Table[LOCKER_2].enable,Var_Table[LOCKER_2].position,Var_Table[LOCKER_2].addr);
	printf("LOCKER_3=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[LOCKER_3].status, Var_Table[LOCKER_3].enable,Var_Table[LOCKER_3].position,Var_Table[LOCKER_3].addr);
	printf("LOCKER_4=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[LOCKER_4].status, Var_Table[LOCKER_4].enable,Var_Table[LOCKER_4].position,Var_Table[LOCKER_4].addr);
	printf("VA_METER_1=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[VA_METER_1].status, Var_Table[VA_METER_1].enable,Var_Table[VA_METER_1].position,Var_Table[VA_METER_1].addr);
	printf("VA_METER_2=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[VA_METER_2].status, Var_Table[VA_METER_2].enable,Var_Table[VA_METER_2].position,Var_Table[VA_METER_2].addr);
	printf("VA_METER_3=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[VA_METER_3].status, Var_Table[VA_METER_3].enable,Var_Table[VA_METER_3].position,Var_Table[VA_METER_3].addr);
	printf("VA_METER_4=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[VA_METER_4].status, Var_Table[VA_METER_4].enable,Var_Table[VA_METER_4].position,Var_Table[VA_METER_4].addr);
	printf("VA_METER_5=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[VA_METER_5].status, Var_Table[VA_METER_5].enable,Var_Table[VA_METER_5].position,Var_Table[VA_METER_5].addr);
	printf("VA_METER_6=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[VA_METER_6].status, Var_Table[VA_METER_6].enable,Var_Table[VA_METER_6].position,Var_Table[VA_METER_6].addr);
	printf("POWER_BD_1=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[POWER_BD_1].status, Var_Table[POWER_BD_1].enable,Var_Table[POWER_BD_1].position,Var_Table[POWER_BD_1].addr);
	printf("POWER_BD_2=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[POWER_BD_2].status, Var_Table[POWER_BD_2].enable,Var_Table[POWER_BD_2].position,Var_Table[POWER_BD_2].addr);
	printf("POWER_BD_3=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[POWER_BD_3].status, Var_Table[POWER_BD_3].enable,Var_Table[POWER_BD_3].position,Var_Table[POWER_BD_3].addr);
	printf("IO_BD_1=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[IO_BD_1].status, Var_Table[IO_BD_1].enable,Var_Table[IO_BD_1].position,Var_Table[IO_BD_1].addr);
	printf("IO_BD_2=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[IO_BD_2].status, Var_Table[IO_BD_2].enable,Var_Table[IO_BD_2].position,Var_Table[IO_BD_2].addr);
	printf("IO_BD_3=0x%02x=0x%02x=0x%02x=0x%02x\r\n",Var_Table[IO_BD_3].status, Var_Table[IO_BD_3].enable,Var_Table[IO_BD_3].position,Var_Table[IO_BD_3].addr);
	*/

	//控制器参数结构体
	stuVMCtl_Param = (VMCONTROL_PARAM*)malloc(sizeof(VMCONTROL_PARAM));
	memset(stuVMCtl_Param,0,sizeof(VMCONTROL_PARAM));
	//遥控寄存器结构体
	stuRemote_Ctrl = (REMOTE_CONTROL*)malloc(sizeof(REMOTE_CONTROL));
	memset(stuRemote_Ctrl,0,sizeof(REMOTE_CONTROL));

	/////////////////  DO配置表开始/////////////////////////////////////////////
	temp = 0;	//统计到底有几个DO
//printf("main 111\n");
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
//printf("main 111 StrDoSeq[%d]=%s, DoSeq[%d]=%d\n",i,StrDoSeq[i].c_str(),i,DoSeq[i]);
	}
//printf("main 222\n");
	// 如果都没有配置，就按DO顺序进行默认配置
	if (temp == 0)
	{
		//printf("temp=0\r\n");
		for (i = 0; i < SWITCH_COUNT; i++)
		{
			DoSeq[i] = i;
		}
	}
//printf("main 333\n");
/*	for (i = 0; i < SWITCH_COUNT; i++)
	{
		printf("do_seqx=0x%02x\r\n",DoSeq[i]);
	}*/
	/////////////////  DO配置表结束/////////////////////////////////////////////

	//门架自由流运行状态结构体
	stuFlagRunStatus = (FLAGRUNSTATUS*)malloc(sizeof(FLAGRUNSTATUS));
	memset(stuFlagRunStatus,0,sizeof(FLAGRUNSTATUS));
	//华为机柜状态结构体
	stuHUAWEIDevValue = &HUAWEIDevValue;
    initHUAWEIGantry();
	initHUAWEIALARM();
	
	//初始化RSU控制器状态
	memset(&stuRsuControl,0,sizeof(RSUCONTROLER)); 
//	for(j=0;j<8;j++)
//		memset(&stuRsuControl.ControlStatusN,0,sizeof(AntennaInfoN_n));	//rsu 旧版本用
	//rsu 新版本用
	for(j=0;j<12;j++)
	{
		memset(&stuRsuControl.Psam1,0,sizeof(PSAM1_S));//控制器1,PSAM卡状态
		memset(&stuRsuControl.Psam2,0,sizeof(PSAM2_S));//控制器2,PSAM卡状态
	}
	for(j=0;j<8;j++)
		memset(&stuRsuControl.AntennaInfoN,0,sizeof(AntennaInfoN_n));
	//end rsu 新版本用
	//初始化RSU设备信息结构体
	for(i=0;i<RSUCTL_NUM;i++)
	{
		memset(&stuRsuData[i],0,sizeof(RSU_DATA_INIT)); 
		for(j=0;j<8;j++)
			memset(&stuRsuData[i].PSAMInfoN[j],0,sizeof(PSAMInfoN_n));
		//初始化天线软件复位状态结构体
	}
	memset(&stuRsuReset,0,sizeof(RSU_RESET)); 
	//读空调状态结构体
	stuAirCondRead = (AIRCOND_PARAM*)malloc(sizeof(AIRCOND_PARAM));
	memset(stuAirCondRead,0,sizeof(AIRCOND_PARAM));
	//写空调状态结构体
	stuAirCondWrite = (AIRCOND_PARAM*)malloc(sizeof(AIRCOND_PARAM));
	memset(stuAirCondWrite,0,sizeof(AIRCOND_PARAM));
//	//初始化防雷器结构体
//	memset(&stuSpd_AI_Param,0,sizeof(RSU_DATA_INIT)); 

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

	//初始化定时取工控机状态线程
//	init_HTTP_DataGet();
//	usleep(100000); //delay 100ms

	//初始化利通定时推送线程
	init_LTKJ_DataPost();
	usleep(100000); //delay 100ms
	write(WDTfd, "\0", 1);
	//初始化新粤定时推送线程(取消)，新粤改为主动取数据方式
//	init_XY_DataPost();
//	usleep(100000); //delay 100ms

/*	//初始化socket定时推送线程(推送给小槟)
	init_SocketNetSend();
	usleep(100000); //delay 100ms
	write(WDTfd, "\0", 1);*/
	//初始化获取摄像头状态
	IpCamServerInit();
	write(WDTfd, "\0", 1);
	//处理重启
	init_DealDoReset();
    //atlas
    AtlasInit();
    while(1)
    {
        write(WDTfd, "\0", 1);
		sleep(5);
		
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


