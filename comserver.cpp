
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
#include "snmp.h"
#include "err_com.h"

using namespace std;
CComPort *mComPort1 = NULL ;	// 未用
CComPort *mComPort2 = NULL ;	// 接触摸屏

CMyCritical Com1Cri;
CMyCritical Com2Cri;

CMyCritical Com1SendCri;
CMyCritical Com2SendCri;

char LastSendBuf[256];
int LastSendLen=0;


UINT16	screen_poll_flag = 0;


// 等待RS232的回应
UINT8  WAIT_response_com_flag = WAIT_COM_NONE;
UINT8  WAIT_response_com_last = WAIT_COM_NONE;	// 保存上一次


/*因为大小端的原因，IPaddr的整型值和IP地址刚好是反的*/
/*比如128.8.130.8转后变成0x08820880*/
static in_addr IPaddr[2] = {0,0};
UINT8 SoftVersion[3] = {0,0,0};

extern string StrID;			//硬件ID
extern string StrVersionNo;
extern string StrFlagID;
extern void GetIPinfo(IPInfo *ipInfo);
extern void GetIPinfo2(IPInfo *ipInfo);
extern VA_METER_PARAMS *stuVA_Meter_Param[VA_METER_BD_MAX_NUM];
extern THUAWEIGantry HUAWEIDevValue;	//华为机柜状态


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


// 从配置文件中读取IP1地址，放在IPaddr1中
void IP1getFromConfig(void)
{
	IPInfo IPDadaForCom;	// 只用于RS232，传输IP地址

	memset(&IPDadaForCom,0,sizeof(IPInfo));
	GetIPinfo(&IPDadaForCom);
	//printf("IP1s0x%s \r\n",IPDadaForCom.ip);
	ipstring_to_ipint(IPDadaForCom.ip,&IPaddr[0]);
}

// 从配置文件中读取IP2地址，放在IPaddr2中
void IP2getFromConfig(void)
{
	IPInfo IPDadaForCom;	// 只用于RS232，传输IP地址

	memset(&IPDadaForCom,0,sizeof(IPInfo));
	GetIPinfo2(&IPDadaForCom);
	//printf("IP2s0x%s \r\n",IPDadaForCom.ip);
	ipstring_to_ipint(IPDadaForCom.ip,&IPaddr[1]);
}


// 从配置文件中读取温度值，并放大10倍
int tmpGetFromConfig(string *temp)
{
	int tempValue;	// 只用于RS232，传输温度值

	// 先转换成实数,后四舍五入
	tempValue = round(atof((*temp).c_str()));
	return tempValue;
}


// 从配置文件中读取湿度值，并放大10倍
int moistGetFromConfig(string *moist)
{
	int moistValue;	// 只用于RS232，传输湿度值

	// 先转换成实数,后四舍五入
	moistValue = round(atof((*moist).c_str()));
	return moistValue;
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
	IDDataSaved = (unsigned long long)strtoll(StrID.c_str(),NULL,10);
	return IDDataSaved;
}

// 读取门架号,连同前面的字母,字母放在pbuf的第一个字符中
unsigned long long GetFlagID(char *pbuf)
{
	const char *pid = NULL;
	const char *pstr = NULL;
	unsigned long long flagIDSaved;	// 64位长度存ID
	UINT8 len = 0;
	char realstr[50]={0,};	// 实际的截断后的字符串
	int i,j=0;

	pid = StrFlagID.c_str();
	*pbuf = *pid;			// 返回第一个字母地址
	len = strlen(pid);
	//printf("len = 0x%02x \r\n",len);
	// 触摸屏上只能显示17位字符加上前面的G/S一共18位
	for(i =0, j=0;i< len; i++)
    {
		if(isdigit(*(pid+i)))
		{
			realstr[j] = pid[i];
			j++;			// 把数值赋给新的字符串
		}
	}
	realstr[j] = '\0';			// 字符串的长度
	len = j;
	printf("len = 0x%02x \r\n",len);
	if (len >17)		// 已经排除了前面的字母
	{
		pstr = realstr+len-17;
		//memcpy(realstr,pid+len-17,17);
		//realstr = pid+len-17;	// 取后17位
		printf("pstr = %s\r\n",pstr);
		// 这里就舍弃掉前面的字母了, 不用加1
		flagIDSaved = (unsigned long long)strtoll(pstr,NULL,10);
		printf("flagIDSaved = %lld\r\n",flagIDSaved);
	}
	// 刚好18位，就按照原来的来
	else
	{
		/*不能用atoi和atol，它们最大只能转换长整型,超出的会返回0x7FFFFFFF*/
		flagIDSaved = (unsigned long long)strtoll(realstr,NULL,10);
		printf("realstr = %s\r\n",realstr);
	}

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


int DealComm(unsigned char *buf,unsigned short int len)
{
	// 功能暂时保留
	switch(WAIT_response_com_flag)
	{
		case WAIT_TIME_MSG:
		break;

		default:
		break;
	}

	// 写数据的正常回应 0x5a 0xa5 0x03 0x82 0x4f 0x4b
	// 测试发现，如果只拔掉触摸屏而不是断开com1端，则数据又会原原本本返回
	if ((len == 6) && (buf[4] == 0x4F) && (buf[5] == 0x4B))
	{
		WAIT_response_com_flag = WAIT_COM_NONE;
		//printf("WAIT_response_com_flag=NONE");
	}
	return 0;
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
		len = read(mComPort2->fd, buf+buffPos, 256);
		buffPos = buffPos+len;
		if(buffPos<5) continue;

		// 这里有个小bug,当屏幕热插拔后，有回复，当时数据帧可能会不完整，会出现
		// FRAM error, 可能半分钟后才会恢复, 导致的结果就是屏幕进入休眠很慢
		if((buf[BUF_HEAD1] != FRAME_HEAD_1) || (buf[BUF_HEAD2] != FRAME_HEAD_2))
		{
			printf("FRAM error\r\n");
			if(buffPos>=256) buffPos=0;
			continue ;
		  }

		// printf("com2 len=%d\r\n",buffPos) ;
		// int j ;for(j=0;j<buffPos;j++)printf("0x%02x ",buf[j]);printf("\r\n");

		if ((buf[BUF_CMD] == CMD_WRITE) || (buf[BUF_CMD] == CMD_READ))
		{
			DealComm(buf, buffPos);;
		}
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
	UINT16 temp_v[2];
	UINT16 moist_v[2];
	char letter = 0;

	time_t nSeconds;
	struct tm * pTM;

	switch (msg_type)
	{
	case WRITE_VAR_MSG:
		IDInfo = IDgetFromConfig();
		IP1getFromConfig();
		IP2getFromConfig();
		VergetFromConfig();
		flagIDInfo = GetFlagID(&letter);
		temp_v[0] = tmpGetFromConfig(&HUAWEIDevValue.strhwEnvTemperature[0]);
		//printf("temp1=%s ",HUAWEIDevValue.strhwEnvTemperature[0].c_str());
		temp_v[1] = tmpGetFromConfig(&HUAWEIDevValue.strhwEnvTemperature[1]);
		//printf("temp2=%s ",HUAWEIDevValue.strhwEnvTemperature[1].c_str());
		moist_v[0] = moistGetFromConfig(&HUAWEIDevValue.strhwEnvHumidity[0]);
		//printf("moist1=%s ",HUAWEIDevValue.strhwEnvHumidity[0].c_str());
		moist_v[1] = moistGetFromConfig(&HUAWEIDevValue.strhwEnvHumidity[1]);
		//printf("moist2=%s ",HUAWEIDevValue.strhwEnvHumidity[1].c_str());

		pbuf[len++] = FRAME_HEAD_1;
		pbuf[len++] = FRAME_HEAD_2;		// 帧头为0x5AA5
		pbuf[len++] = 0x05;				// 长度先临时写入5，后面再更新
		pbuf[len++] = CMD_WRITE;
		// 串口屏的地址是以16位为单位的
		pbuf[len++] = ((address>>8)&0xFF);
		pbuf[len++] = address&0xFF;

		// 温度
		pbuf[len++] = (temp_v[0]>>8)&0xFF;
		pbuf[len++] = temp_v[0]&0xFF;
		pbuf[len++] = (temp_v[1]>>8)&0xFF;
		pbuf[len++] = temp_v[1]&0xFF;

		// 湿度
		pbuf[len++] = (moist_v[0]>>8)&0xFF;
		pbuf[len++] = moist_v[0]&0xFF;
		pbuf[len++] = (moist_v[1]>>8)&0xFF;
		pbuf[len++] = moist_v[1]&0xFF;

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
		pbuf[len++] = IPaddr[0].s_addr&0xFF;;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr[0].s_addr >>8)&0xFF;;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr[0].s_addr >>16)&0xFF;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr[0].s_addr >>24) &0xFF;

		// IP地址2 默认192.192.1.136，需要改
		pbuf[len++] = 0;
		pbuf[len++] = IPaddr[1].s_addr&0xFF;;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr[1].s_addr >>8)&0xFF;;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr[1].s_addr >>16)&0xFF;
		pbuf[len++] = 0;
		pbuf[len++] = (IPaddr[1].s_addr >>24) &0xFF;

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

	case BACKLIGHT_EN_MSG:
		pbuf[len++] = FRAME_HEAD_1;
		pbuf[len++] = FRAME_HEAD_2;		// 帧头为0x5AA5
		pbuf[len++] = 0x07;				// 长度先临时写入7，后面再更新
		pbuf[len++] = CMD_WRITE;
		// 串口屏的地址是以16位为单位的
		pbuf[len++] = ((address>>8)&0xFF);
		pbuf[len++] = address&0xFF;
		pbuf[len++] = 0x5A;		// 写的时候要是0x5A，配置生效后会清0
		pbuf[len++] = 0x00;		// 不关心
		pbuf[len++] = 0x00;		// 不关心
		pbuf[len++] = 0x3C;		// 开启背光位是bit3,1:开启，0:关闭
		pbuf[BUF_LENTH] = len-3;
		break;

	case BACKLIGHT_CFG_MSG:
		pbuf[len++] = FRAME_HEAD_1;
		pbuf[len++] = FRAME_HEAD_2;		// 帧头为0x5AA5
		pbuf[len++] = 0x07;				// 长度先临时写入7，后面再更新
		pbuf[len++] = CMD_WRITE;
		// 串口屏的地址是以16位为单位的
		pbuf[len++] = ((address>>8)&0xFF);
		pbuf[len++] = address&0xFF;
		pbuf[len++] = LIGHT_100_ON;		// 背光打开时的亮度，100%
		pbuf[len++] = LIGHT_0_OFF;		// 背光关闭时的亮度，0%
		pbuf[len++] = ((SLEEP_ENTER_TIM>>8)&0xFF);
		pbuf[len++] = SLEEP_ENTER_TIM&0xFF;	// 进入休眠的时间
		pbuf[BUF_LENTH] = len-3;
		break;

	case BACKLIGHT_ON_MSG:
		pbuf[len++] = FRAME_HEAD_1;
		pbuf[len++] = FRAME_HEAD_2;		// 帧头为0x5AA5
		pbuf[len++] = 0x05;				// 长度先临时写入5，后面再更新
		pbuf[len++] = CMD_WRITE;
		// 串口屏的地址是以16位为单位的
		pbuf[len++] = ((address>>8)&0xFF);
		pbuf[len++] = address&0xFF;
		pbuf[len++] = LIGHT_100_ON;		// 背光打开时的亮度，100%
		pbuf[len++] = LIGHT_100_ON;		// 背光关闭时的亮度，0%
		pbuf[BUF_LENTH] = len-3;
		break;

	case BACKLIGHT_OFF_MSG:
		pbuf[len++] = FRAME_HEAD_1;
		pbuf[len++] = FRAME_HEAD_2;		// 帧头为0x5AA5
		pbuf[len++] = 0x05;				// 长度先临时写入5，后面再更新
		pbuf[len++] = CMD_WRITE;
		// 串口屏的地址是以16位为单位的
		pbuf[len++] = ((address>>8)&0xFF);
		pbuf[len++] = address&0xFF;
		pbuf[len++] = LIGHT_100_ON;		// 背光打开时的亮度，100%
		pbuf[len++] = LIGHT_0_OFF;		// 背光关闭时的亮度，0%
		pbuf[BUF_LENTH] = len-3;
		break;

	case WRITE_LETTER_MSG:
		GetFlagID(&letter);
		//printf("letter = %c \r\n",letter);
		//printf("test = %d \r\n",'G');
		//printf("test1 = %d \r\n",'高');
		pbuf[len++] = FRAME_HEAD_1;
		pbuf[len++] = FRAME_HEAD_2;		// 帧头为0x5AA5
		pbuf[len++] = 0x05;				// 长度先临时写入5，后面再更新
		pbuf[len++] = CMD_WRITE;
		// 串口屏的地址是以16位为单位的
		pbuf[len++] = ((address>>8)&0xFF);
		pbuf[len++] = address&0xFF;

		// 温度
		pbuf[len++] = letter&0xFF;	// 英文字符GB2312编码只占1个字节
		pbuf[BUF_LENTH] = len-3;
		break;

	case NOT_USED_MSG:
		break;
	default:
		break;
	}
	return len;
}


// 设置标志位的处理函数，外部调用
void ScreenFlagSet(SREEN_SET_LIST sFlag)
{
	screen_poll_flag |= BIT(sFlag);
}


void *ComPort2HandleDataThread(void *param)
{
	param = param;
	static bool entry = false;	// 刚连上屏幕要发背光休眠设置命令
	static UINT8 entry_cnt = 0;
	UINT8 i = 0;
	static UINT16 screen_poll_cnt = 0;
	// 默认是关的
	static UINT16 door_status_last = LOCKER_CLOSED;
	UINT16 door_status_now = LOCKER_CLOSED;		// 现在的门状态

	while(1)
	{
		screen_poll_cnt++;
		//printf("screen_poll_cnt = %d\r\n",screen_poll_cnt);
		if ((screen_poll_cnt % WAIT_SECOND_2) == 0)	// 2s进行一次走时
		{
			screen_poll_flag |= BIT(SCREEN_TIME_SET);
		}
		if (screen_poll_cnt >= WAIT_SECOND_5)	// 50秒进行一次变量同步
		{
			screen_poll_cnt = 0;
			// 断线了，先不发送，等恢复
			//if (Comm_Err_Flag_Get(ERR_COM1) == 0)
			{
				screen_poll_flag |= BIT(VAR_SET);
			}
		}

		/************断线逻辑,这个是屏幕背光休眠需要的，不然换屏后失效********/
		// 处理是否是换屏了,或者是屏幕坏了
		if (Comm_Err_Flag_Get(ERR_COM1))
		{
			printf("com break!\r\n");
			entry = false;
			entry_cnt = 0;
		}
		else		// 否则发送
		{
			// 第一次进来,默认是一个新屏幕
			if (entry == false)
			{
				//printf("WAIT_SECOND_5 = %d\r\n",WAIT_SECOND_5);	// C测试下宏定义
				// 多发一次，防止失败
				entry_cnt++;
				if (entry_cnt >= 2)
				{
					entry_cnt = 2;
					entry = true;
					printf("com recovery!\r\n");
				}
				screen_poll_flag |= BIT(BACKLIGHT_EN_SET);
			}
		}
		/*******************断线逻辑,完*********************/
		/*-----------------------------------------------------------------------------*/


		/********************开关门逻辑************************/
		// 400ms取一次状态，要够快
		door_status_now = DoorStatusFromLocker();
		//printf("door_status = %d\r\n",door_status_now);
		if (door_status_now != door_status_last)
		{
			// 开门
			if ((door_status_now == LOCKER_OPEN))
			{
				screen_poll_flag |= BIT(BACKLIGHT_ON_SET);
			}
			else
			{
				// 关门
				screen_poll_flag |= BIT(BACKLIGHT_OFF_SET);
			}
		}
		door_status_last = door_status_now;
		/********************开关门逻辑，完**********************/

		//printf("screen_poll_flag = 0x%02x\r\n",screen_poll_flag);	// C测试下宏定义

		// 以上是置标志，现在是真正的发送数据
		if (screen_poll_flag&BIT(ERR_CHECK))	// 错误检测不占用串口,可以随时检查,和其它标志不排斥
		{
			screen_poll_flag &= ~(BIT(ERR_CHECK));
			Comm_Err_Process(ERR_COM1,&WAIT_response_com_flag,WAIT_TIME_MSG);
		}

		// 设置时序，门架号前面的字母只设置一次，最高有限级
		if (screen_poll_flag&BIT(LETTER_SET))
		{
			screen_poll_flag &= ~(BIT(LETTER_SET));
			WAIT_response_com_flag = WAIT_LETTER_MSG;
			SendCom2WriteReg(LETTER_REG_ADD,WRITE_LETTER_MSG);
		}
		else if (screen_poll_flag&BIT(BACKLIGHT_EN_SET))
		{
			screen_poll_flag &= ~(BIT(BACKLIGHT_EN_SET));
			// 先使能背光, 再设置时间，直接一起, 不然发2次不好处理
			WAIT_response_com_flag = WAIT_BACKLIGHT_EN_MSG;
			SendCom2WriteReg(SYS_CFG_ADDR,BACKLIGHT_EN_MSG);
			usleep(MSG_SEND_INTERVAL);
			SendCom2WriteReg(LED_CFG_ADDR,BACKLIGHT_CFG_MSG);
		}
		else if (screen_poll_flag&BIT(BACKLIGHT_ON_SET))
		{
			screen_poll_flag &= ~(BIT(BACKLIGHT_ON_SET));
			WAIT_response_com_flag = WAIT_BACKLIGHT_ON_MSG;
			SendCom2WriteReg(LED_CFG_ADDR,BACKLIGHT_ON_MSG);
		}
		else if (screen_poll_flag&BIT(BACKLIGHT_OFF_SET))
		{
			screen_poll_flag &= ~(BIT(BACKLIGHT_OFF_SET));
			WAIT_response_com_flag = WAIT_BACKLIGHT_OFF_MSG;
			SendCom2WriteReg(LED_CFG_ADDR,BACKLIGHT_OFF_MSG);
		}
		else if (screen_poll_flag&BIT(SCREEN_TIME_SET))
		{
			screen_poll_flag &= ~(BIT(SCREEN_TIME_SET));
			WAIT_response_com_flag = WAIT_TIME_MSG;
			SendCom2WriteReg(TIME_REG_ADD,WRITE_TIME_MSG);
			// 每次发送一次时间设置都进行一次错误检测
			screen_poll_flag |= BIT(ERR_CHECK);
		}
		else if (screen_poll_flag&BIT(VAR_SET))
		{
			screen_poll_flag &= ~(BIT(VAR_SET));
			WAIT_response_com_flag = WAIT_VAR_MSG;
			SendCom2WriteReg(VAR_REG_ADD,WRITE_VAR_MSG);
		}
		usleep(MSG_SEND_INTERVAL);	// 周期400ms
	}
	return 0 ;
}

void cominit(void)
{
	//SetSystemTime("2017-08-30 17:03:00");
	mComPort2 = new CComPort();

	/*改用串口1,原因是外壳接错了*/
	mComPort2->fd = mComPort2->openSerial((char *)"/dev/ttymxc1",115200) ;//9100 To TouchScreen
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
	//for(j=0;j<datalen;j++) printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort2->SendBuf(bytSend,datalen);
    Com2SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0 ;
}

