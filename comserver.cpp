
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
#include "arpa/inet.h"



using namespace std;//寮曞叆鏁翠釜鍚嶇┖闂?
CComPort *mComPort1 = NULL ;   //
CComPort *mComPort2 = NULL ;   //Stm32

CMyCritical Com1Cri;
CMyCritical Com2Cri;

CMyCritical Com1SendCri;
CMyCritical Com2SendCri;

char LastSendBuf[256];
int LastSendLen=0;

/*因为大小端的原因，IPaddr的整型值和IP地址刚好是反的*/
/*比如128.8.130.8转后变成0x08820880*/
in_addr IPaddr = {0};
UINT8 SoftVersion[3] = {0,0,0};

extern string StrID;			//硬件ID
extern string StrVersionNo;
extern void GetIPinfo(IPInfo *ipInfo);
extern VA_METER_PARAMS *stuVA_Meter_Param[VA_METER_BD_MAX_NUM];


// 把如128.8.130.82转成一个4字节整型
void ipstring_to_ipint(char *ip_addr, in_addr *buf)
{
    char *p;
    int i=0;

	#if 0
	// 分隔IP地址字符串,第一次调用,返回第一个.分隔符的字符串
    p = strtok(ip_addr,".");
    while((p)&&(i<4))
    {
    	// 把上一次转换好的字符串转成整数
        buf[i] = atoi(p);
		printf("IP0x%02x \r\n",buf[i]);
		// ""表示以后的每一次调用的地址都是上一次调用的.分隔符的下一个地址
        p = strtok("NULL",".");
		printf("P=%s \r\n",p);
        i++;
    }
	printf("i=0x%02x \r\n",i);
	#endif
	inet_aton(ip_addr,buf);
}


// 从配置文件中读取IP地址，放在IPaddr中
void IPgetFromConfig(void)
{
	IPInfo IPDadaForCom;	// 只用于RS232，传输IP地址

	memset(&IPDadaForCom,0,sizeof(IPInfo));
	GetIPinfo(&IPDadaForCom);
	//printf("IPs0x%s \r\n",IPDadaForCom.ip);
	ipstring_to_ipint(IPDadaForCom.ip,&IPaddr);
}

// 把如V1.00.00转成一个3字节数组，数组里面是整数值
void verstring_to_verint(char *verval, unsigned char *buf)
{
    char *p;
    int i=0;

	// 分隔IP地址字符串,第一次调用,返回第一个.分隔符的字符串
    p = strtok(verval,".");
    while((p)&&(i<3))
    {
    	// 把上一次转换好的字符串转成整数
        buf[i] = atoi(p);
		//printf("ver0x%02x \r\n",buf[i]);
		// ""表示以后的每一次调用的地址都是上一次调用的.分隔符的下一个地址
        p = strtok(NULL,".");
        i++;
    }
}


// 从配置文件中读取IP地址，放在IPaddr中
void VergetFromConfig(void)
{
	char softver[20];	// 只用于RS232，传输版本号

	memset(softver,0,sizeof(softver));
	sprintf(softver,StrVersionNo.c_str());
	verstring_to_verint(&softver[1],SoftVersion);
}

// 从配置文件中读取设备ID
unsigned long long IDgetFromConfig(void)
{
	unsigned long long IDDataSaved;	// 64位长度存ID

	/*不能用atoi和atol，它们最大只能转换长整型,超出的会返回0x7FFFFFFF*/
	IDDataSaved = (unsigned long long)strtod(StrID.c_str(),NULL);
	return IDDataSaved;
}

// 获取电压电流传感器的电压电流值,返回增加的长度
int VAGetAndSaved(UINT8 *pbuf, VA_METER_PARAMS *pVA_Meter)
{
	int lenth = 0;

	pbuf[lenth++] = (pVA_Meter->phase[0].vln>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[0].vln&0xFF;
	pbuf[lenth++] = (pVA_Meter->phase[0].amp>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[0].amp;
	pbuf[lenth++] = (pVA_Meter->phase[1].vln>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[1].vln&0xFF;
	pbuf[lenth++] = (pVA_Meter->phase[1].amp>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[1].amp;
	pbuf[lenth++] = (pVA_Meter->phase[2].vln>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[2].vln&0xFF;
	pbuf[lenth++] = (pVA_Meter->phase[2].amp>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[2].amp;
	pbuf[lenth++] = (pVA_Meter->phase[3].vln>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[3].vln&0xFF;
	pbuf[lenth++] = (pVA_Meter->phase[3].amp>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[3].amp;
	pbuf[lenth++] = (pVA_Meter->phase[4].vln>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[4].vln&0xFF;
	pbuf[lenth++] = (pVA_Meter->phase[4].amp>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[4].amp;
	pbuf[lenth++] = (pVA_Meter->phase[5].vln>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[5].vln&0xFF;
	pbuf[lenth++] = (pVA_Meter->phase[5].amp>>8)&0xFF;
	pbuf[lenth++] = pVA_Meter->phase[5].amp;

	return lenth;
}



//To touchScreen
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

		if((buf[BUF_HEAD1] != FRAME_HEAD_1) || (buf[BUF_HEAD2] != FRAME_HEAD_2))
		{
			printf("FRAM error\r\n");
			if(buffPos>=256) buffPos=0;
			continue ;
	  	}

		printf("com3 len=%d\r\n",buffPos) ;
		int j ;for(j=0;j<buffPos;j++)printf("0x%02x ",buf[j]);printf("\r\n");

		if (buf[BUF_CMD] == CMD_WRITE)
		{
			;
		}
		else if (buf[BUF_CMD] == CMD_READ)
		{
			;
		}
		//DealComm(buf , buffPos);
		buffPos=0;
		usleep(5000);//delay 5ms
   }

   return NULL ;

}


/******************************************************************************
 * 函数名:	message_pack
 * 描述: 信息的打包,形成协议格式数据
 *
 * 输入参数: uart_no, 目标输出串口, buf，数据处理串口
 * 输出参数:
 * 返回值:
 *
 * 作者:Jerry
 * 创建日期:2018.10.18
 *
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
UINT8 message_pack(UINT16 address,UINT8 msg_type,UINT8 *buf)
{
	/*取得目标串口对应的发送缓存*/
	UINT8 *pbuf = buf;	//buf->pTxBuf;
	UINT8 len = 0;
	unsigned long long IDInfo = IDgetFromConfig();
	printf("IDD=%lld \r\n",IDInfo);

	IPgetFromConfig();
	VergetFromConfig();
	switch (msg_type)
	{
	/*专门用来测试*/
	case WRITE_MSG:
		pbuf[len++] = FRAME_HEAD_1;
		pbuf[len++] = FRAME_HEAD_2;		// 帧头为0x5AA5
		pbuf[len++] = 0x05;				// 长度先临时写入5，后面再更新
		pbuf[len++] = CMD_WRITE;
		// 串口屏的地址是以16位为单位的
		pbuf[len++] = ((address>>8)&0xFF);
		pbuf[len++] = address&0xFF;
		// IP地址转换过来顺序是反的，IP高位在s_addr的低位
		pbuf[len++] = 0;
		pbuf[len++] = IPaddr.s_addr&0xFF;;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr.s_addr >>8)&0xFF;;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr.s_addr >>16)&0xFF;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr.s_addr >>24) &0xFF;
		pbuf[len++] = (IDInfo>>56)&0xFF;
		pbuf[len++] = (IDInfo>>48)&0xFF;
		pbuf[len++] = (IDInfo>>40)&0xFF;
		pbuf[len++] = (IDInfo>>32)&0xFF;
		pbuf[len++] = (IDInfo>>24)&0xFF;
		pbuf[len++] = (IDInfo>>16)&0xFF;
		pbuf[len++] = (IDInfo>>8)&0xFF;
		pbuf[len++] = IDInfo&0xFF;
		pbuf[len++] = 0;
		pbuf[len++] = SoftVersion[0];
		pbuf[len++] = 0;
		pbuf[len++] = SoftVersion[1];
		pbuf[len++] = 0;
		pbuf[len++] = SoftVersion[2];
		// 0xDFFB~0xDFFF保留未用，填0
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		// 0xE000开始
		for (int i = 0; i < VA_METER_BD_MAX_NUM; i++)
		{
			if (stuVA_Meter_Param[i] != NULL)
			{
				len += VAGetAndSaved(pbuf+len, stuVA_Meter_Param[i]);
			}
		}

		//len += VAGetAndSaved(pbuf+len, stuVA_Meter_Param[1]);
		//len += VAGetAndSaved(pbuf+len, stuVA_Meter_Param[2]);
		//len += VAGetAndSaved(pbuf+len, stuVA_Meter_Param[3]);
		//len += VAGetAndSaved(pbuf+len, stuVA_Meter_Param[4]);
		//len += VAGetAndSaved(pbuf+len, stuVA_Meter_Param[5]);
		pbuf[BUF_LENTH] = len-3;
		break;

	case NOT_USED_MSG:
		break;
	default:
		break;
	}
	return len;
}


void *ComPort2HandleDataThread(void *param)
{
	param = param;
	while(1)
	{
		//9 查询环境变量寄存器
		//SendCom1ReadReg(POWER_CTRL_ADDR_1,READ_REGS,ENVI_START_ADDR,ENVI_REG_MAX);
		//usleep(5000);//delay 5ms
		SendCom2WriteReg(REG_ADD,WRITE_MSG);
	  	sleep(11);//delay 11	// 错开一个时间，防止负载同一时间太高
	}
	return 0 ;
}

IPInfo ipinfo;

void cominit(void)
{
	//SetSystemTime("2017-08-30 17:03:00");
	mComPort2 = new CComPort();

	/*不用*/
	mComPort2->fd = mComPort2->openSerial((char *)"/dev/ttymxc2",115200) ;//9100 To TouchScreen
	if(mComPort2->fd>0)
		printf("ComPort2 open secess! %d\r\n",mComPort2->fd);
	else
	   printf("ComPort2 open fail! %d\r\n",mComPort2->fd);

	pthread_t m_ComPort2Thread ;
	pthread_create(&m_ComPort2Thread,NULL,ComPort2Thread,NULL);

	pthread_t m_ComPortGetDataThread ;
	pthread_create(&m_ComPortGetDataThread,NULL,ComPort2HandleDataThread,NULL);
}

//发送写数据寄存器 ,没有CRC
int SendCom2WriteReg(UINT16 Addr, UINT8 Func)
{
    Com2SendCri.Lock();
    UINT8 j;
	UINT8 bytSend[256];
	UINT8 datalen = 0;

	datalen = message_pack(Addr,Func,bytSend);

	// debug测试打印
	for(j=0;j<datalen;j++) printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort2->SendBuf(bytSend,datalen);
    Com2SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0 ;
}


//发送写遥控寄存器 ADDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + MBUS_OPT_CODE_H + MBUS_OPT_CODE_L + CRC(2)
int SendCom1RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code)
{
    Com2SendCri.Lock();
    UINT8 bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};

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
#if 0

//发送读数据寄存器 ADDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + REFS_COUNT_H + MBUS_REFS_COUNT_L + CRC(2)
int SendCom2ReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
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
#endif

