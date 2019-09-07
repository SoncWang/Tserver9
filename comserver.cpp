
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
#include <sys/time.h>


using namespace std;
CComPort *mComPort1 = NULL ;	// 未用
CComPort *mComPort2 = NULL ;	// 接触摸屏

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
extern string StrFlagID;
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

// 读取门架号
unsigned long long GetFlagID(void)
{
	unsigned long long flagIDSaved;	// 64位长度存ID

	/*不能用atoi和atol，它们最大只能转换长整型,超出的会返回0x7FFFFFFF*/
	flagIDSaved = (unsigned long long)strtod(StrFlagID.c_str(),NULL);
	return flagIDSaved;
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

		printf("com2 len=%d\r\n",buffPos) ;
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
	unsigned long long IDInfo = 0;
	unsigned long long flagIDInfo = 0;

	time_t nSeconds;
	struct tm * pTM;

	switch (msg_type)
	{
	case WRITE_VAR_MSG:
		IDInfo = IDgetFromConfig();
		IPgetFromConfig();
		VergetFromConfig();
		flagIDInfo = GetFlagID();
		printf("flagID=%lld \r\n",flagIDInfo);

		pbuf[len++] = FRAME_HEAD_1;
		pbuf[len++] = FRAME_HEAD_2;		// 帧头为0x5AA5
		pbuf[len++] = 0x05;				// 长度先临时写入5，后面再更新
		pbuf[len++] = CMD_WRITE;
		// 串口屏的地址是以16位为单位的
		pbuf[len++] = ((address>>8)&0xFF);
		pbuf[len++] = address&0xFF;

		// 门架号flagID
		pbuf[len++] = (flagIDInfo>>56)&0xFF;
		pbuf[len++] = (flagIDInfo>>48)&0xFF;
		pbuf[len++] = (flagIDInfo>>40)&0xFF;
		pbuf[len++] = (flagIDInfo>>32)&0xFF;
		pbuf[len++] = (flagIDInfo>>24)&0xFF;
		pbuf[len++] = (flagIDInfo>>16)&0xFF;
		pbuf[len++] = (flagIDInfo>>8)&0xFF;
		pbuf[len++] = flagIDInfo&0xFF;

		// IP地址转换过来顺序是反的，IP高位在s_addr的低位
		// IP地址1
		pbuf[len++] = 0;
		pbuf[len++] = IPaddr.s_addr&0xFF;;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr.s_addr >>8)&0xFF;;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr.s_addr >>16)&0xFF;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr.s_addr >>24) &0xFF;

		// IP地址2 默认192.192.1.136，需要改
		pbuf[len++] = 0;
		pbuf[len++] = 192;
		pbuf[len++] = 0;
		pbuf[len++] = 192;
		pbuf[len++] = 0;
		pbuf[len++] = 2;
		pbuf[len++] = 0;
		pbuf[len++] = 136;

		// ID号
		pbuf[len++] = (IDInfo>>56)&0xFF;
		pbuf[len++] = (IDInfo>>48)&0xFF;
		pbuf[len++] = (IDInfo>>40)&0xFF;
		pbuf[len++] = (IDInfo>>32)&0xFF;
		pbuf[len++] = (IDInfo>>24)&0xFF;
		pbuf[len++] = (IDInfo>>16)&0xFF;
		pbuf[len++] = (IDInfo>>8)&0xFF;
		pbuf[len++] = IDInfo&0xFF;
		// 版本号
		pbuf[len++] = 0;
		pbuf[len++] = SoftVersion[0];
		pbuf[len++] = 0;
		pbuf[len++] = SoftVersion[1];
		pbuf[len++] = 0;
		pbuf[len++] = SoftVersion[2];
		// 0xDFFF保留未用，填0
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		// 0xE000开始
		for (int i = 0; i < VA_METER_BD_MAX_NUM; i++)
		{
			// 判断实际的使用情况，以防溢出
			if (stuVA_Meter_Param[i] != NULL)
			{
				len += VAGetAndSaved(pbuf+len, stuVA_Meter_Param[i]);
			}
		}
		// 实际长度要减去前面3个字节
		pbuf[BUF_LENTH] = len-3;
		break;

		//		/* Used by other time functions.
		//		struct tm
		//		{
		//		  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
		//		  int tm_min;			/* Minutes.	[0-59] */
		//		  int tm_hour;			/* Hours.	[0-23] */
		//		  int tm_mday;			/* Day.		[1-31] */
		//		  int tm_mon;			/* Month.	[0-11] */
		//		  int tm_year;			/* Year	- 1900.  */
		//		  int tm_wday;			/* Day of week.	[0-6] */
		//		  int tm_yday;			/* Days in year.[0-365]	*/
		//		  int tm_isdst;			/* DST.		[-1/0/1]*/
		//
		//		# ifdef	__USE_BSD
		//		  long int tm_gmtoff;		/* Seconds east of UTC.  */
		//		  const char *tm_zone;		/* Timezone abbreviation.  */
		//		# else
		//		  long int __tm_gmtoff;		/* Seconds east of UTC.  */
		//		  const char *__tm_zone;	/* Timezone abbreviation.  */
		//		# endif
		//		};

	case WRITE_TIME_MSG:
		// 取得当前时间
		time(&nSeconds);
		pTM = localtime(&nSeconds);

		pbuf[len++] = FRAME_HEAD_1;
		pbuf[len++] = FRAME_HEAD_2;		// 帧头为0x5AA5
		pbuf[len++] = 0x05;				// 长度先临时写入5，后面再更新
		pbuf[len++] = CMD_WRITE;
		// 时间的地址是固定的0x0010
		pbuf[len++] = ((TIME_REG_ADD>>8)&0xFF);
		pbuf[len++] = TIME_REG_ADD&0xFF;
		// 触摸屏年份从2000年开始, 而这里是从1900年开始
		pbuf[len++] = (UINT8)(pTM->tm_year-100);
		// 月份0~11，要加1
		pbuf[len++] = (UINT8)(pTM->tm_mon+1);
		pbuf[len++] = (UINT8)pTM->tm_mday;
		pbuf[len++] = (UINT8)pTM->tm_wday;	// 星期，未使用
		pbuf[len++] = (UINT8)pTM->tm_hour;
		pbuf[len++] = (UINT8)pTM->tm_min;
		pbuf[len++] = (UINT8)pTM->tm_sec;
		pbuf[len++] = 0;
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
	//static UINT8 cnt = 0;
	//static bool entry = false;	//第一次进来2个命令都要发
	while(1)
	{
		// 6s钟对时一次, 1s后发送显示命令
		SendCom2WriteReg(TIME_REG_ADD,WRITE_TIME_MSG);
		sleep(1);//delay 1seconds
		SendCom2WriteReg(VAR_REG_ADD,WRITE_VAR_MSG);
		sleep(5);//delay 5seconds

	#if 0
		/*半分钟同步一次时间*/
		if (cnt >= 6)
		{
			cnt = 0;
			SendCom2WriteReg(TIME_REG_ADD,WRITE_TIME_MSG);
		}
		else
		{
			SendCom2WriteReg(VAR_REG_ADD,WRITE_VAR_MSG);
		}
		cnt++;
	  	sleep(5);//delay 10seconds
	#endif
	}
	return 0 ;
}

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

