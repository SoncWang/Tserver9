
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "comport.h"
#include "comserver.h"
#include "MyCritical.h"
#include <string>
#include <semaphore.h>
#include "tea.h"
#include "Protocol.h"
#include "rtc.h"
#include "server.h"
#include "rs485server.h"


using namespace std;//寮曞叆鏁翠釜鍚嶇┖闂?
CComPort *mComPort1 = NULL ;   //
CComPort *mComPort2 = NULL ;   //Stm32

CMyCritical Com1Cri;
CMyCritical Com2Cri;

CMyCritical Com2SendCri;

extern sem_t httpsem;
extern string Sendhttp ;
extern CMyCritical HttpCri;

string EmStyle ;

extern int httpsend(string strhttp);
string StrOrderNumber = "";
string StrPayQrCode = "";

unsigned int DeviceId = 1 ;
extern ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
extern UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体
extern DEVICE_PARAMS *stuDev_Param[POWER_BD_MAX_NUM];		//装置参数寄存器
extern VA_METER_PARAMS *stuVA_Meter_Param[VA_METER_BD_MAX_NUM];		//伏安表电压电流结构体
extern AIRCOND_PARAM *stuAirCondRead;	//读空调状态结构体

char LastSendBuf[256];
int LastSendLen=0;


//To Stm32
void *ComPort2Thread(void *param)
{
   param = NULL;
   int buffPos=0;
   int len ;
   unsigned char buf[256] ;
   while(1)
   {
      len = read(mComPort2->fd, buf+buffPos, 256) ;
	  buffPos = buffPos+len;
	  if(buffPos<5) continue;

	  //CRC
	  unsigned short int CRC = CRC16(buf,buffPos-2) ;
	  if((((CRC&0xFF00) >> 8)!= buf[buffPos-2]) || ((CRC&0x00FF) != buf[buffPos-1]))
		{
//		  printf("CRC error\r\n");
 		  if(buffPos>=256) buffPos=0;

		  continue ;
	  	}

//      printf("com1 len=%d\r\n",buffPos) ;
//	  int j ;for(j=0;j<buffPos;j++)printf("0x%02x ",buf[j]);printf("\r\n");

	  DealComm(buf , buffPos);

	  buffPos=0;
      usleep(5000);//delay 5ms
   }

   return NULL ;

}


void *ComPortGetDataThread(void *param)
{
	while(1)
	{
	printf("ComPortGetDataThread\r\n");
		//9 查询环境变量寄存器
		SendCom1ReadReg(POWER_CTRL_ADDR_1,READ_REGS,ENVI_START_ADDR,ENVI_REG_MAX);
		usleep(5000);//delay 5ms
		//10 查询UPS变量寄存器
		SendCom1ReadReg(POWER_CTRL_ADDR_1,READ_REGS,UPS_START_ADDR,UPS_REG_MAX);
		usleep(5000);//delay 5ms
		//11 查询SPD变量寄存器
		SendCom1ReadReg(POWER_CTRL_ADDR_1,READ_REGS,SPD_START_ADDR,SPD_REG_MAX);
		usleep(5000);//delay 5ms
		//13 查询空调参数寄存器
		SendCom1ReadReg(POWER_CTRL_ADDR_1,READ_REGS,AIRCOND_START_ADDR,AIRCOND_REG_MAX);
	  	sleep(10);//delay 5s
	}
	return 0 ;
}


void cominit(void)
{
   //SetSystemTime("2017-08-30 17:03:00");
   mComPort2 = new CComPort();

//   mComPort2->fd = mComPort2->openSerial((char *)"/dev/ttySP2",115200) ;//287 To Stm32
   mComPort2->fd = mComPort2->openSerial((char *)"/dev/ttymxc3",115200) ;//9100 To Stm32
   if(mComPort2->fd>0)
   	printf("ComPort2 open secess! %d\r\n",mComPort2->fd);
   else
	   printf("ComPort2 open fail! %d\r\n",mComPort2->fd);

   pthread_t m_ComPort2Thread ;
   pthread_create(&m_ComPort2Thread,NULL,ComPort2Thread,NULL);

   pthread_t m_ComPortGetDataThread ;
   pthread_create(&m_ComPortGetDataThread,NULL,ComPortGetDataThread,NULL);
}


//发送读数据寄存器 ADDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + REFS_COUNT_H + MBUS_REFS_COUNT_L + CRC(2)
int SendCom1ReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
    Com2SendCri.Lock();
    UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};

    int len=8;
    bytSend[MBUS_ADDR]        = Addr;
    bytSend[MBUS_FUNC]        = Func;       //读寄存器
    bytSend[MBUS_REFS_ADDR_H] = (REFS_ADDR&0xFF00) >> 8;     //寄存器起始地址高位
    bytSend[MBUS_REFS_ADDR_L] =  REFS_ADDR&0x00FF;           //寄存器起始地址低位
    bytSend[MBUS_REFS_COUNT_H] = (REFS_COUNT&0xFF00) >> 8;     //寄存器数量高位
    bytSend[MBUS_REFS_COUNT_L] =  REFS_COUNT&0x00FF;           //寄存器数量低位

    //CRC校验
    unsigned short int CRC = CRC16(bytSend,len-2) ;
    bytSend[len-2] = (CRC&0xFF00) >> 8;     //CRC高位
    bytSend[len-1] =  CRC&0x00FF;           //CRC低位

//	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort2->SendBuf(bytSend,len);

	memcpy(LastSendBuf,bytSend,len);
	LastSendLen=len;

    Com2SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0 ;
}

//发送写数据寄存器 ADDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + REFS_COUNT_H + MBUS_REFS_COUNT_L + DATA + CRC(2)
int SendCom1WriteReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT, UINT8 *pBuf)
{
    Com2SendCri.Lock();
    UINT8 i,j;
	UINT8 *bytSend;

	UINT8 datalen=pBuf[0];
	UINT8 len=6+1+datalen+2;
	bytSend = (UINT8 *)malloc(len);

    bytSend[MBUS_ADDR]        = Addr;
    bytSend[MBUS_FUNC]        = Func;       //写寄存器
    bytSend[MBUS_REFS_ADDR_H] = (REFS_ADDR&0xFF00) >> 8;     //寄存器起始地址高位
    bytSend[MBUS_REFS_ADDR_L] =  REFS_ADDR&0x00FF;           //寄存器起始地址低位
    bytSend[MBUS_REFS_COUNT_H] = (REFS_COUNT&0xFF00) >> 8;     //寄存器数量高位
    bytSend[MBUS_REFS_COUNT_L] =  REFS_COUNT&0x00FF;           //寄存器数量低位
    memcpy(bytSend+6,pBuf,datalen+1);

    //CRC校验
    unsigned short int CRC = CRC16(bytSend,len-2) ;
    bytSend[len-2] = (CRC&0xFF00) >> 8;     //CRC高位
    bytSend[len-1] =  CRC&0x00FF;           //CRC低位

//	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort2->SendBuf(bytSend,len);

	memcpy(LastSendBuf,bytSend,len);
	LastSendLen=len;
	free(bytSend);

    Com2SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0 ;
}

//发送写遥控寄存器 ADDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + MBUS_OPT_CODE_H + MBUS_OPT_CODE_L + CRC(2)
int SendCom1RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code)
{
    Com2SendCri.Lock();
    UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};

    int len=8;

    bytSend[MBUS_ADDR]        = Addr;
    bytSend[MBUS_FUNC]        = Func;       //写寄存器
    bytSend[MBUS_REFS_ADDR_H] = (REFS_ADDR&0xFF00) >> 8;     //寄存器起始地址高位
    bytSend[MBUS_REFS_ADDR_L] =  REFS_ADDR&0x00FF;           //寄存器起始地址低位
    bytSend[MBUS_OPT_CODE_H] = (code&0xFF00) >> 8;     //操作码高位
    bytSend[MBUS_OPT_CODE_L] =  code&0x00FF;           //操作码低位

    //CRC校验
    unsigned short int CRC = CRC16(bytSend,len-2) ;
    bytSend[len-2] = (CRC&0xFF00) >> 8;     //CRC高位
    bytSend[len-1] =  CRC&0x00FF;           //CRC低位

//	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort2->SendBuf(bytSend,len);

	memcpy(LastSendBuf,bytSend,len);
	LastSendLen=len;

    Com2SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0 ;
}

//查询数据寄存器 ADDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + REFS_COUNT_H + MBUS_REFS_COUNT_L + CRC(2)
int DealComm(unsigned char *buf,unsigned short int len)
{
	ENVI_PARAMS *pstuEnvPam=stuEnvi_Param;		// 环境数据结构体
	UPS_PARAMS *pstuUpsPam=stuUps_Param;		//USP结构体 电源数据寄存器
	SPD_PARAMS *pstuSpdPam=stuSpd_Param;		//防雷器结构体
	DEVICE_PARAMS *pstuDevPam=stuDev_Param[0];		//装置参数寄存器
//	RSU_PARAMS *pstuRsuInfo=stuRSU_Param;		//RSU天线信息结构体


	unsigned char *sendbuf;
	unsigned char MBusAddr , MBusFunc;
	MBusAddr = LastSendBuf[0];
	MBusFunc = LastSendBuf[1];
	unsigned short int RegAddr ,RegCount;
	RegAddr = LastSendBuf[2]<<8 | LastSendBuf[3];
	RegCount = LastSendBuf[4]<<8 | LastSendBuf[5];
	char * jsonPack=(char *)malloc(2*1024);
	int jsonPackLen=0;
	if(MBusFunc==READ_REGS)					//读寄存器
	{
		switch(RegAddr)
		{
	      case ENVI_START_ADDR:      //9 环境数据寄存器
	          SetEnviStatusStruct(buf,len);
	          break ;
		  case UPS_START_ADDR: 	  //10 UPS数据寄存器
	          SetUpsStatusStruct(buf,len);
			  break ;
		  case SPD_START_ADDR:	  //11 防雷器数据寄存器
			  SetSpdStatusStruct(buf,len);
			  break ;
		  case AIRCOND_START_ADDR:	   //13 空调参数寄存器
			  SetAirCondStruct(buf,len);
			  break ;
		  case RSU_START_ADDR:	 //14 RSU天线参数寄存器
			  SetRsuStatusStruct(buf,len);
//			jsonStrRsuWriter((char*)pstuRsuInfo,jsonPack,&jsonPackLen);
//			NetSendParm(NETCMD_SEND_RSU_PARAM,jsonPack,jsonPackLen);
			break ;
	      default:
	          return 1 ;
	          break ;
	   }
	}
	else if(MBusFunc==PRESET_REGS)		//写寄存器
	{
		if(buf[1]!=MBusFunc)
		{
			printf("执行命令失败！\r\n");
			MySendMessage("执行命令失败！\r\n");
		}
		else
		{
			printf("执行命令成功！\r\n");
			MySendMessage("执行命令成功！\r\n");
		}
	}
	else if(MBusFunc==FORCE_COIL)		//设置继电器输出状态
	{
		if(buf[1]!=MBusFunc)
		{
			printf("执行命令失败！\r\n");
			MySendMessage("执行命令失败！\r\n");
		}
		else
		{
			printf("执行命令成功！\r\n");
			MySendMessage("执行命令成功！\r\n");
		}
	}
	free(jsonPack);
	return 0 ;
}

int SetEnviStatusStruct(unsigned char *buf,unsigned short int len)
{
   unsigned char MBusAddr , MBusFunc;
   MBusAddr = buf[0];
   MBusFunc = buf[1];
   unsigned char RegCount;
   RegCount = buf[2];

   int bp=3;
   stuEnvi_Param->temp=buf[bp]<<8 | buf[bp+1];bp+=2;// 当前环境温度值300
   stuEnvi_Param->moist=buf[bp]<<8 | buf[bp+1];bp+=2;// 当前环境湿度值301
   stuEnvi_Param->water_flag=buf[bp]<<8 | buf[bp+1];bp+=2;// 漏水302
   stuEnvi_Param->front_door_flag=buf[bp]<<8 | buf[bp+1];bp+=2;//前柜门开关状态303
   stuEnvi_Param->back_door_flag=buf[bp]<<8 | buf[bp+1];bp+=2;//后柜门开关状态304
   stuEnvi_Param->door_overtime=buf[bp]<<8 | buf[bp+1];bp+=2;//柜门开启超时305
   stuEnvi_Param->smoke_event_flag=buf[bp]<<8 | buf[bp+1];bp+=2;//烟雾报警306
   stuEnvi_Param->Reserve1=buf[bp]<<8 | buf[bp+1];bp+=2;		//保留1 307
   stuEnvi_Param->Reserve2=buf[bp]<<8 | buf[bp+1];bp+=2;		//保留2 308
   stuEnvi_Param->Reserve3=buf[bp]<<8 | buf[bp+1];bp+=2;		//保留3 309
   stuEnvi_Param->air_cond_status=buf[bp]<<8 | buf[bp+1];bp+=2;//空调状态310
   stuEnvi_Param->air_cond_fan_in_status=buf[bp]<<8 | buf[bp+1];bp+=2;//空调内风机状态311
   stuEnvi_Param->air_cond_fan_out_status=buf[bp]<<8 | buf[bp+1];bp+=2;//空调外风机状态312
   stuEnvi_Param->air_cond_comp_status=buf[bp]<<8 | buf[bp+1];bp+=2;//空调压缩机状态313
   stuEnvi_Param->air_cond_heater_status=buf[bp]<<8 | buf[bp+1];bp+=2;//空调电加热状态314
   stuEnvi_Param->air_cond_fan_emgy_status=buf[bp]<<8 | buf[bp+1];bp+=2;//空调应急风机状态315
   stuEnvi_Param->air_cond_temp_out=buf[bp]<<8 | buf[bp+1];bp+=2;//当前空调室外温度值316 ×10
   stuEnvi_Param->air_cond_temp_in=buf[bp]<<8 | buf[bp+1];bp+=2;//当前空调室内温度值317 ×10
   stuEnvi_Param->air_cond_amp=buf[bp]<<8 | buf[bp+1];bp+=2;//当前空调电流值318 ×1000
   stuEnvi_Param->air_cond_volt=buf[bp]<<8 | buf[bp+1];bp+=2;//当前空调电压值319 ×1

   //告警
   stuEnvi_Param->air_cond_hightemp_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;//空调高温告警320
   stuEnvi_Param->air_cond_lowtemp_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;//空调低温告警321
   stuEnvi_Param->air_cond_highmoist_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;//空调高湿告警322
   stuEnvi_Param->air_cond_lowmoist_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;//空调低湿告警323
   stuEnvi_Param->air_cond_infan_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;//空调内风机故障324
   stuEnvi_Param->air_cond_outfan_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;//空调外风机故障325
   stuEnvi_Param->air_cond_comp_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;//空调压缩机故障326
   stuEnvi_Param->air_cond_heater_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;//空调电加热故障327
   stuEnvi_Param->air_cond_emgyfan_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;//空调应急风机故障328

   return 0;
}

int SetUpsStatusStruct(unsigned char *buf,unsigned short int len)
{
   unsigned char MBusAddr , MBusFunc;
   MBusAddr = buf[0];
   MBusFunc = buf[1];
   unsigned char RegCount;
   RegCount = buf[2];

   int bp=3;
   // 输入数据
   stuUps_Param->in_phase_num=buf[bp]<<8 | buf[bp+1];bp+=2;// UPS输入相数 40
   stuUps_Param->in_freq=buf[bp]<<8 | buf[bp+1];bp+=2;// 交流输入频率   41 ×100
   stuUps_Param->volt_Ain=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输入相电压A 42 ×10
   stuUps_Param->volt_Bin=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输入相电压B 43 ×10
   stuUps_Param->volt_Cin=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输入相电压C 44 ×10
   stuUps_Param->amp_Ain=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输入相电流A 	 45 ×10
   stuUps_Param->amp_Bin=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输入相电流B 	 46 ×10
   stuUps_Param->amp_Cin=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输入相电流C47 ×10

   //输出数据
   stuUps_Param->out_phase_num=buf[bp]<<8 | buf[bp+1];bp+=2;// 输出相数 48
   stuUps_Param->out_freq=buf[bp]<<8 | buf[bp+1];bp+=2;// UPS交流输出频率 49	   ×100
   stuUps_Param->volt_Aout=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输出相电压A 50 ×10
   stuUps_Param->volt_Bout=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输出相电压B 51 ×10
   stuUps_Param->volt_Cout=buf[bp]<<8 | buf[bp+1];bp+=2; //UPS交流输出相电压C 52		×10
   stuUps_Param->amp_Aout=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输出相电流A 53 ×10
   stuUps_Param->amp_Bout=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输出相电流B 54 ×10
   stuUps_Param->amp_Cout=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS交流输出相电流C 55 ×10

   //电池参数
   stuUps_Param->running_day=buf[bp]<<8 | buf[bp+1];bp+=2; //UPS运行时间 56 天
   stuUps_Param->battery_volt=buf[bp]<<8 | buf[bp+1];bp+=2; //UPS电池电压57 ×10
   stuUps_Param->amp_charge=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS充电电流 58 ×100
   stuUps_Param->battery_left=buf[bp]<<8 | buf[bp+1];bp+=2;//UPS电池后备时间 59 ×10，分钟
   stuUps_Param->battery_tmp=buf[bp]<<8 | buf[bp+1];bp+=2; //电池温度 60 ×10
   stuUps_Param->battery_capacity=buf[bp]<<8 | buf[bp+1];bp+=2;//电池当前容量 61 ×100%
   stuUps_Param->battery_dischg_times=buf[bp]<<8 | buf[bp+1];bp+=2; //电池放电次数 62

   //USP状态参数
   stuUps_Param->supply_out_status=buf[bp]<<8 | buf[bp+1];bp+=2;//输出供电状态 63
   stuUps_Param->supply_in_status=buf[bp]<<8 | buf[bp+1];bp+=2;//输入供电状态 64
   stuUps_Param->battery_status=buf[bp]<<8 | buf[bp+1];bp+=2;//电池状态 65

   //USP告警
   stuUps_Param->main_abnormal=buf[bp]<<8 | buf[bp+1];bp+=2;		   // 主路异常 66 0x00：正常 0xF0：异常
   stuUps_Param->system_overtemp=buf[bp]<<8 | buf[bp+1];bp+=2;		   // 系统过温, 67 0x00：正常 0xF0：异常
   stuUps_Param->sysbat_low_prealarm=buf[bp]<<8 | buf[bp+1];bp+=2;	   // 系统电池电量低预告警,68 0x00：正常 0xF0：异常
   stuUps_Param->rectifier_overload=buf[bp]<<8 | buf[bp+1];bp+=2;	   // 整流器过载,69 0x00：正常 0xF0：异常
   stuUps_Param->inverter_overload=buf[bp]<<8 | buf[bp+1];bp+=2;	   // 逆变器过载,70 0x00：正常 0xF0：异常
   stuUps_Param->bypass_abnomal=buf[bp]<<8 | buf[bp+1];bp+=2;		   // 旁路异常,71 0x00：正常 0xF0：异常
   stuUps_Param->battery_low_prealarm=buf[bp]<<8 | buf[bp+1];bp+=2;    // 电池电压低,72 0x00：正常 0xF0：异常
   stuUps_Param->battery_abnomal=buf[bp]<<8 | buf[bp+1];bp+=2;		   // 电池电压异常,73 0x00：正常 0xF0：异常
   stuUps_Param->battery_overtemp=buf[bp]<<8 | buf[bp+1];bp+=2; 	   // 电池过温,74 0x00：正常 0xF0：异常
   stuUps_Param->fan_abnormal=buf[bp]<<8 | buf[bp+1];bp+=2; 		   // 风扇故障,75 0x00：正常 0xF0：异常
   stuUps_Param->shutdown_alarm=buf[bp]<<8 | buf[bp+1];bp+=2;		   //紧急关机告警,76 0x00：正常 0xF0：异常
   stuUps_Param->maintain_status=buf[bp]<<8 | buf[bp+1];bp+=2;		   //维修模式,77 0x00：正常 0xF0：异常
   stuUps_Param->inverter_supply=buf[bp]<<8 | buf[bp+1];bp+=2;		   //电池逆变供电,78 0x00：正常 0xF0：异常
   stuUps_Param->bypass_supply=buf[bp]<<8 | buf[bp+1];bp+=2;		   //旁路供电,79 0x00：正常 0xF0：异常
   return 0;
}

int SetSpdStatusStruct(unsigned char *buf,unsigned short int len)
{
   unsigned char MBusAddr , MBusFunc;
   MBusAddr = buf[0];
   MBusFunc = buf[1];
   unsigned char RegCount;
   RegCount = buf[2];

   int bp=3;
   stuSpd_Param->status=buf[bp]<<8 | buf[bp+1];bp+=2;// 防雷器在线状态
   stuSpd_Param->struck_times=buf[bp]<<8 | buf[bp+1];bp+=2;//雷击次数

   return 0;
};

int SetRsuStatusStruct(unsigned char *buf,unsigned short int len)
{
   unsigned char MBusAddr , MBusFunc;
   MBusAddr = buf[0];
   MBusFunc = buf[1];
   unsigned char RegCount;
   RegCount = buf[2];

   int bp=3;
/*   for(int i=0;i<RSU_NUM;i++)
   	{
	   stuRSU_Param->phase[i].vln=buf[bp]<<8 | buf[bp+1];bp+=2;//RSU天线电压
	   stuRSU_Param->phase[i].amp=buf[bp]<<8 | buf[bp+1];bp+=2;//RSU天线电流
	   stuRSU_Param->phase[i].enable=1;// 投切标志
   	}*/

   return 0;
}

int SetAirCondStruct(unsigned char *buf,unsigned short int len)
{
	unsigned char MBusAddr , MBusFunc;
	MBusAddr = buf[0];
	MBusFunc = buf[1];
	unsigned char RegCount;
	RegCount = buf[2];

	int bp=3;
	stuAirCondRead->aircondset=buf[bp]<<8 | buf[bp+1];bp+=2;//空调关机//1220
	stuAirCondRead->aircoldstartpoint=buf[bp]<<8 | buf[bp+1];bp+=2;//空调制冷点//1221 			50
	stuAirCondRead->aircoldloop=buf[bp]<<8 | buf[bp+1];bp+=2;//空调制冷回差//1222					10
	stuAirCondRead->airhotstartpoint=buf[bp]<<8 | buf[bp+1];bp+=2;//空调制热点//1223 			15
	stuAirCondRead->airhotloop=buf[bp]<<8 | buf[bp+1];bp+=2;//空调制热回差//1224					10

	return 0;
}


