#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/socket.h>
#include <unistd.h> 	// fork, close
#include <arpa/inet.h>  // inet_ntoa
#include <pthread.h>
#include <netinet/tcp.h>
#include <linux/rtc.h>
#include <ctype.h>
#include <stdbool.h>
#include "tea.h"
#include "net_spd.h"
#include "modbus.h"
#include "rs485server.h"

/*************************数据定义*****************************/
static int sockfd_spd[SPD_NUM+RES_NUM];	// 还有1个接地电阻
static int udpfd_spd[SPD_NUM+RES_NUM];	// 华咨的是udp协议


UINT16  spd_net_flag[SPD_NUM] = {0,0};	// SPD轮询标志
UINT16  spd_ctl_flag[SPD_NUM] = {0,0};	// SPD控制标志

UINT8  WAIT_net_res_flag[SPD_NUM]={0,0};	// 等待信息回应

UINT16 gsocket = 0;
UINT16 net_Conneted = 0;	// 连接标志1:已经连接
//UINT16 HZ_Conneted[SPD_NUM+RES_NUM] = {0,};	// 连接标志1:已经连接


UINT8 SPD_Type = TYPE_LEIXUN;
UINT8 SPD_num = SPD_NUM;
UINT8 SPD_Address[SPD_NUM+RES_NUM] = {SPD_ADDR,SPD_ADDR,SPD_RES_ADDR};
//UINT8 SPD_Res_Address = SPD_RES_ADDR;
SPD_CTRL_VALUE SPD_ctrl_value[SPD_NUM];

pthread_mutex_t SPDdataHandleMutex;


//extern RSUCONTROLER stuRsuControl;	//RSU控制器状态
//extern RSU_DATA_INIT stuRsuData;	//RSU设备信息结构体
//extern RSU_RESET stuRsuReset;			//天线软件复位状态结构体
extern char gsSPDIP[SPD_NUM+RES_NUM][20];	//SPD-IP地址
extern char gsSPDPort[SPD_NUM+RES_NUM][10];	//SPD-端口
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体

// 华咨的网络有关变量定义
struct sockaddr_in HZSPDAddr[SPD_NUM+RES_NUM];
//string strHZSPDdata[SPD_NUM+RES_NUM] = {"",};

pthread_mutex_t HZSPDMutex[SPD_NUM+RES_NUM];

UINT8 HZ_reset_flag[SPD_NUM+RES_NUM] = {false,};
UINT8 HZ_reset_pre[SPD_NUM+RES_NUM] = {false,};	// 对HZ_reset_flag预先处理


int obtain_net();
extern void char_to_int(UINT8* buffer,UINT16* value);
extern void WriteLog(char* str);
//void myprintf(char* str);


/*功能码及功能码对应的处理函数*/
const  SPD_FunctionArray_Struct g_SPD_Function_Array[SPD_DATA_NUM] =
{
	{SPD_READ_CMD,SPD_AI_ADDR,SPD_AI_NUM},
	{SPD_DI_CMD,SPD_DI_ADDR,SPD_DI_NUM},
	{SPD_DO_CMD,SPD_DO_ADDR,SPD_DO_NUM},
	// 这项对应的是华咨的,但是华咨不是MODBUS格式，所以没用
	{0,0,0},
	// 宽永的先保留
	{0,0,0},
	{0,0,0},
	{0,0,0},
	// 接地电阻值
	{SPD_RES_READ_CMD,SPD_RES_STATUS_ADDR,SPD_RES_STATUS_NUM},
};


/******************************************************************************
*  函数名: void HZ_char_to_int(INT8U* buffer,LONG32U* value)
*
*  描述: 字符转化为整型，低字节在前
*
*
*
*  输入:
*
*  输出:
*
*  返回值:
*
*  其它:
*******************************************************************************/
void HZ_char_to_int(UINT8* buffer,UINT16* value)
{
	INTEGER_UNION int_value;

	int_value.b[0] = *(buffer);
	int_value.b[1] = *(buffer + 1);
	*value = int_value.i;
}


/******************************************************************************
*  函数名: void char_to_float(INT8U* buffer,LONG32U* value)
*
*  描述: 字符转化为长整型
*
*
*
*  输入:
*
*  输出:
*
*  返回值:
*
*  其它:
*******************************************************************************/
void char_to_float(UINT8* buffer,FDATA* value)
{
	FDATA f_value;
	UINT8 i;

	for(i=0;i<4;i++)
	{
		//f_value.c[3-i] = *(buffer + i);
		f_value.c[i] = *(buffer + i);
	}
	value->f = f_value.f;
}

/*接口函数，其它线程调用这个函数,进行参数的设置*/
/*ai_data: ai参数设置，data:其它参数设置*/
int Ex_SPD_Set_Process(int seq,SPD_CTRL_LIST SPD_ctrl_event, UINT8 set_addr, FDATA ai_data,UINT16 data)
{
	// 如果是SPD_NUM表明所有的防雷监测器都要设置
	if (seq < SPD_NUM)
	{
		spd_ctl_flag[seq] |= BIT(SPD_ctrl_event);	//先置标志位

		SPD_ctrl_value[seq].ref_addr = set_addr;
		if (SPD_ctrl_event == SPD_AI_SET)
		{
			SPD_ctrl_value[seq].f_ai_set = ai_data;
		}
		else if (SPD_ctrl_event == SPD_DO_SET)
		{
			SPD_ctrl_value[seq].do_set = data;
		}
		else if (SPD_ctrl_event == SPD_RES_SET)
		{
			SPD_ctrl_value[seq].res_set = data;
		}
	}
	else
	{
		for (int i = 0; i< SPD_num; i++)
		{
			spd_ctl_flag[i] |= BIT(SPD_ctrl_event);	//先置标志位

			SPD_ctrl_value[i].ref_addr = set_addr;
			if (SPD_ctrl_event == SPD_AI_SET)
			{
				SPD_ctrl_value[i].f_ai_set = ai_data;
			}
			else if (SPD_ctrl_event == SPD_DO_SET)
			{
				SPD_ctrl_value[i].do_set = data;
			}
			else if (SPD_ctrl_event == SPD_RES_SET)
			{
				SPD_ctrl_value[i].res_set = data;
			}
		}
	}

	return 0;
}



/* psd发送数据--读取模拟,DI，DO数据 */
int SPD_Read_Reg(int seq,UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
    UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};
    int len=8;

	pthread_mutex_lock(&SPDdataHandleMutex);

    bytSend[MBUS_ADDR]        = Addr;
    bytSend[MBUS_FUNC]        = Func;	// 0x03
    bytSend[MBUS_REFS_ADDR_H] = (REFS_ADDR&0xFF00) >> 8;     // Register address
    bytSend[MBUS_REFS_ADDR_L] =  REFS_ADDR&0x00FF;
    bytSend[MBUS_REFS_COUNT_H] = (REFS_COUNT&0xFF00) >> 8;  // Register counter
    bytSend[MBUS_REFS_COUNT_L] =  REFS_COUNT&0x00FF;

    // CRC calculation
    unsigned short int CRC = CRC16(bytSend,len-2) ;
    bytSend[len-2] = (CRC&0xFF00) >> 8;     //CRC high
    bytSend[len-1] =  CRC&0x00FF;           //CRC low

	printf("psd Rdata->socket_addr=%d:",sockfd_spd[seq]);
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	write(sockfd_spd[seq],bytSend,len);
	pthread_mutex_unlock(&SPDdataHandleMutex);

	usleep(5000);	//delay 5ms
	return 0 ;
}


/* psd发送数据--华咨读取防雷数据,REFS_ADDR未使用*/
int SPD_HZ_Read_Reg(int seq,UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
    UINT8 i,j,bytSend[24]={0x00,};
    int len=24;

	pthread_mutex_lock(&SPDdataHandleMutex);

	// 前导码为100,低位在前
    bytSend[0] = 0x64;
    bytSend[1] = 0;
    bytSend[2] = 0;
    bytSend[3] = 0;

	// 版本为1，低位在前
    bytSend[4] = 0x01;
    bytSend[5] = 0;
    bytSend[6] = 0;
    bytSend[7] = 0;

	// 长度为24 = 4*6，低位在前
    bytSend[8] = REFS_COUNT;
    bytSend[9] = 0;
    bytSend[10] = 0;
    bytSend[11] = 0;

	// 地址为1，低位在前
    bytSend[12] = Addr;
    bytSend[13] = 0;
    bytSend[14] = 0;
    bytSend[15] = 0;

	// 命令码为0x0C，低位在前
    bytSend[16] = Func;
    bytSend[17] = 0;
    bytSend[18] = 0;
    bytSend[19] = 0;

	// 校验码全为0
    bytSend[20] = 0;
    bytSend[21] = 0;
    bytSend[22] = 0;
    bytSend[23] = 0;

	//printf("psd HZRdata->socket_addr=%d:",sockfd_spd[seq]);
	//for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	//write(socket_addr,bytSend,len);
	sendto(udpfd_spd[seq], bytSend, REFS_COUNT, 0, (struct sockaddr *)&HZSPDAddr[seq], sizeof(struct sockaddr_in));
	pthread_mutex_unlock(&SPDdataHandleMutex);

	usleep(5000);	//delay 5ms
	return 0 ;
}


/* psd发送数据--读取模拟,DI，DO数据 */
int SPD_DO_Ctrl_Reg(int socket_addr,UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 cmd)
{
	UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};
	int len=0;

	pthread_mutex_lock(&SPDdataHandleMutex);
	bytSend[len++] = Addr;
	bytSend[len++] = Func;	// 0x05
	bytSend[len++] = (REFS_ADDR&0xFF00) >> 8;	 // Register address
	bytSend[len++] =  REFS_ADDR&0x00FF;
	bytSend[len++] = (cmd&0xFF00) >> 8;	// Register counter
	bytSend[len++] =  cmd&0x00FF;

	// CRC calculation
	unsigned short int CRC = CRC16(bytSend,len);
	bytSend[len++] = (CRC&0xFF00) >> 8; 	//CRC high
	bytSend[len++] =  CRC&0x00FF;			//CRC low

	//printf("psd Ddata->socket_addr=%d:",socket_addr);
	//for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");
	write(socket_addr,bytSend,len);

	pthread_mutex_unlock(&SPDdataHandleMutex);
	usleep(5000);	//delay 5ms
	return 0 ;
}

#define AI_SET_MIN	2		// AI设置每次至少2个数量
/* psd发送数据--读取模拟,DI，DO数据 ,REFS_ADDR要特别注意一一对应*/
int SPD_AI_Set_Reg(int socket_addr,UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT, FDATA *data)
{
    UINT8 i,j,bytSend[20];
    int len=0;
	int temp,temp2=0;
	FDATA *pdata = data;

	pthread_mutex_lock(&SPDdataHandleMutex);

	memset(bytSend,0,20);
    bytSend[len++]        = Addr;
    bytSend[len++]        = Func;	// 0x10
    bytSend[len++] = (REFS_ADDR&0xFF00) >> 8;     // Register address
    bytSend[len++] =  REFS_ADDR&0x00FF;
    bytSend[len++] = (REFS_COUNT&0xFF00) >> 8;  // Register counter
    temp = REFS_COUNT&0x00FF;
    bytSend[len++] =  temp;
	temp2 = temp*2;
	bytSend[len++] =  temp2;

	if (temp2 > 12)	// 字节总共超出了12,防止溢出
	{
		return 0;
	}

	for (i = 0; i <temp2/4;i++)
	{
		bytSend[len++] =  pdata->c[0];
		bytSend[len++] =  pdata->c[1];
		bytSend[len++] =  pdata->c[2];
		bytSend[len++] =  pdata->c[3];
		pdata++;
	}

    // CRC calculation
    unsigned short int CRC = CRC16(bytSend,len);	// 这里是len，不是len-2
    bytSend[len++] = (CRC&0xFF00) >> 8;     //CRC high
    bytSend[len++] =  CRC&0x00FF;           //CRC low

	//printf("psd Cdata->socket_addr=%d:",socket_addr);
	//for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");
	write(socket_addr,bytSend,len);
	pthread_mutex_unlock(&SPDdataHandleMutex);

	usleep(5000);	//delay 5ms
	return 0 ;
}

/* psd发送数据--设置接地电阻部分数据 */
int SPD_Res_Set_Reg(int socket_addr,UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 cmd)
{
	UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};
	int len=0;

	pthread_mutex_lock(&SPDdataHandleMutex);
	bytSend[len++]		  = Addr;
	bytSend[len++]		  = Func;	// 0x06
	bytSend[len++] = (REFS_ADDR&0xFF00) >> 8;	 // Register address
	bytSend[len++] =  REFS_ADDR&0x00FF;
	bytSend[len++] = (cmd&0xFF00) >> 8;	// Register counter
	bytSend[len++] =  cmd&0x00FF;

	// CRC calculation
	unsigned short int CRC = CRC16(bytSend,len);
	bytSend[len++] = (CRC&0xFF00) >> 8; 	//CRC high
	bytSend[len++] =  CRC&0x00FF;			//CRC low

	//printf("psd ResSetdata:");
	//for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");
	write(socket_addr,bytSend,len);

	pthread_mutex_unlock(&SPDdataHandleMutex);
	usleep(5000);	//delay 5ms
	return 0 ;
}

/* psd发送数据--时间同步 */
int SPD_Time_Set_Reg(int socket_addr,UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR,UINT16 REFS_COUNT)
{
	UINT8 i,j,bytSend[30]={0x00,};
	int len=0;
	int temp = 0;
	UINT16 real_year = 2019;

	time_t nSeconds;
	struct tm * pTM;

	pthread_mutex_lock(&SPDdataHandleMutex);
	// 取得当前时间
	time(&nSeconds);
	pTM = localtime(&nSeconds);

	bytSend[len++]        = Addr;
    bytSend[len++]        = Func;	// 0x10
    bytSend[len++] = (REFS_ADDR&0xFF00) >> 8;     // Register address
    bytSend[len++] =  REFS_ADDR&0x00FF;
    bytSend[len++] = (REFS_COUNT&0xFF00) >> 8;  // Register counter
    temp = REFS_COUNT&0x00FF;
    bytSend[len++] =  temp;
	bytSend[len++] =  temp*2;

	// tm_year是从1900开始算的
	real_year = 1900+pTM->tm_year;

	bytSend[len++] = (real_year&0xFF00) >> 8;
	bytSend[len++] = real_year&0x00FF;			// 年

	bytSend[len++] = 0;
	bytSend[len++] = (UINT8)(pTM->tm_mon+1);	// 月

	bytSend[len++] = 0;
	bytSend[len++] = (UINT8)pTM->tm_mday;		// 日

	bytSend[len++] = 0;
	bytSend[len++] = (UINT8)pTM->tm_hour;		// 时

	bytSend[len++] = 0;
	bytSend[len++] = (UINT8)pTM->tm_min;		// 分

	bytSend[len++] = 0;							// 秒
	bytSend[len++] = (UINT8)pTM->tm_sec;

	// CRC calculation
	unsigned short int CRC = CRC16(bytSend,len);
	bytSend[len++] = (CRC&0xFF00) >> 8; 	//CRC high
	bytSend[len++] =  CRC&0x00FF;			//CRC low

	//printf("psd TimeSetdata:");
	//for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");
	write(socket_addr,bytSend,len);

	pthread_mutex_unlock(&SPDdataHandleMutex);
	usleep(5000);	//delay 5ms
	return 0 ;
}


/*模块化spd数据读取*/
int spd_send_process(UINT16 seq,UINT16 *pnet_flag)
{
	UINT8 addr_temp;
	UINT8 func_temp = HZ_SPD_READ;;
	int seq_t = 0;	// 表明这是哪一台装置
	int event_j;

	if (SPD_Type == TYPE_LEIXUN)
	{
		for(event_j=SPD_AI_DATA; event_j<SPD_DATA_NUM; event_j++)
		{
			if (event_j < SPD_HZ_DATA)
			{
				addr_temp = SPD_Address[seq];
			}
			else if (event_j == SPD_RES_DATA)
			{
				addr_temp = SPD_Address[SPD_NUM];	// 接地电阻仪地址
			}
			else
			{
				// 从其它类型切换过来, 清除掉,然后继续
				*pnet_flag &= ~(BIT(event_j));
				continue;
			}

			if (*pnet_flag & (BIT(event_j)))
			{
				*pnet_flag &= ~(BIT(event_j));
				//读取防雷器和接地电阻的数据
				SPD_Read_Reg(0,addr_temp, g_SPD_Function_Array[event_j].func_code,\
						g_SPD_Function_Array[event_j].reg_addr,g_SPD_Function_Array[event_j].reg_num);
				return 0;
			}
		}
	}
	else if (SPD_Type == TYPE_KY)
	{
		;
	}
	else
	{
		for(event_j=SPD_AI_DATA; event_j<SPD_DATA_NUM; event_j++)
		{
			if (event_j == SPD_HZ_DATA)
			{
				seq_t = seq;
				addr_temp = SPD_Address[seq];
				func_temp = HZ_SPD_READ;
			}
			else if (event_j == SPD_RES_DATA)
			{
				seq_t = 2;	// 接地电阻单独有1个网口地址
				addr_temp = SPD_Address[SPD_NUM];	// 接地电阻仪地址
				func_temp = HZ_RES_READ;
			}
			else
			{
				// 从其它类型切换过来, 清除掉,然后继续
				*pnet_flag &= ~(BIT(event_j));
				continue;
			}

			if (*pnet_flag & (BIT(event_j)))
			{
				*pnet_flag &= ~(BIT(event_j));
				//读取防雷器和接地电阻的数据
				SPD_HZ_Read_Reg(seq_t,HZ_ADDR, func_temp,NULL_VAR,24);
				return 0;
			}
		}
	}
	return 1;
}


/*模块化spd数据设置*/
int spd_ctrl_process(UINT16 seq,UINT16 *pctrl_flag)
{
	UINT8 addr_temp;
	int socketq = 0;	// 表明这是哪一台装置
	int event_i;

	if (SPD_Type == TYPE_LEIXUN)
	{
		socketq = sockfd_spd[0];

		for(event_i=SPD_AI_SET; event_i<SPD_CTRL_NUM; event_i++)
		{
			if (event_i < SPD_RES_SET)	// 防雷检测地址
			{
				addr_temp = SPD_Address[seq];
			}
			else
			{
				addr_temp = SPD_Address[SPD_NUM];	// 接地电阻仪地址
			}

			if (*pctrl_flag & (BIT(event_i)))
			{
				*pctrl_flag &= ~(BIT(event_i));
				switch(event_i)
				{
				case SPD_AI_SET:
					// 不要改id,防止误操作
					if (SPD_ctrl_value[seq].ref_addr != AI_SPD_ID_ADDR)
					{
						//SPD_Address[0] = (UINT8)SPD_ctrl_value.f_ai_set.f;
						//设置防雷器的AI数据
						SPD_AI_Set_Reg(socketq,addr_temp,SPD_WRITE_CMD,SPD_ctrl_value[seq].ref_addr,AI_SET_MIN,&SPD_ctrl_value[seq].f_ai_set);
					}
					break;

				case SPD_DO_SET:
					// DO每次只写1个寄存器
					SPD_DO_Ctrl_Reg(socketq,addr_temp,SPD_DO_CTRL_CMD,SPD_ctrl_value[seq].ref_addr,SPD_ctrl_value[seq].do_set);
					break;

				case SPD_RES_SET:
					// 不要改id,防止误操作
					if (SPD_ctrl_value[seq].ref_addr != RES_ID_ADDR)
					{
						//SPD_Address[SPD_NUM] = (UINT8)SPD_ctrl_value.res_set;
						// RES接地电阻部分设置
						SPD_Res_Set_Reg(socketq,addr_temp,SPD_RES_SET_CMD,SPD_ctrl_value[seq].ref_addr,SPD_ctrl_value[seq].res_set);
					}
					break;

				case SPD_TIME_SET:
					// 对时写
					SPD_Time_Set_Reg(socketq,addr_temp,SPD_WRITE_CMD,SPD_ctrl_value[seq].ref_addr,TIME_SET_LEN);
					break;
				default:
					break;
				}
				return 0;	// 如果有事件发生就要直接返回，防止连续写
			}
		}
	}
	return 1;
}

// 连接到的是哪个网络设备?
int obtain_net_psd(UINT16 seq)
{
	char str[10*1024];
	const char *IPaddress;
	const char * IPport;
	int port;
	struct sockaddr_in net_psd_addr;
	if( (sockfd_spd[seq] = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
    	printf ("ERROR: Failed to obtain SPD Socket Despcritor.\n");
		WriteLog("Socket_SPD error!\n");
    	return (0);
	}
	else
	{
    	printf ("OK: Obtain SPD Socket Despcritor sucessfully.\n");
		WriteLog("Socket_SPD sucessfully.\n");
	}
	IPaddress = gsSPDIP[seq];//获取配置文件中的IP地址
	IPport=gsSPDPort[seq];
	port=atoi(IPport);
	/* Fill the local socket address struct */
	net_psd_addr.sin_family = AF_INET;           		// Protocol Family
	net_psd_addr.sin_port = htons (port);         		// Port number
	net_psd_addr.sin_addr.s_addr  = inet_addr (IPaddress);  	// AutoFill local address
	memset (net_psd_addr.sin_zero,0,8);          		// Flush the rest of struct
	printf("SPD-IPaddress=%s,SPD-IPport=%s\n",IPaddress,IPport);
	sprintf(str,"SPD-IPaddress=%s,SPD-IPport=%s\n",IPaddress,IPport);
	WriteLog(str);

	// 经测试，这个函数在连接不同网段的ip时，返回时间长达30s
	// 修改IP后要等下一次connect才能成功连接
	if (connect(sockfd_spd[seq], (struct sockaddr*)&net_psd_addr, sizeof(net_psd_addr)) == -1)
   	 {
		printf("connect to SPD%d server refused!\n",seq);
		WriteLog("connect to SPD server refused!\n");
		close(sockfd_spd[seq]);
		return (0);
	}
	else
	{
		printf("connect to SPD server sucess!\n");
		WriteLog("connect to SPD server sucess!\n");
		return(1);
	}
}

#define INT_REG_POS	11	// 前面11个是实数

void DealSPDAiMsg(int seq,unsigned char *buf,unsigned short int len)
{
	assert(seq < SPD_NUM);
	UINT8 i;

	FDATA *fpointer = &stuSpd_Param->dSPD_AIdata[seq].leak_current;
	UINT16 *pointer = &stuSpd_Param->dSPD_AIdata[seq].systime_year;

	if(len == (SPD_AI_NUM*2+5))
	{
		// 前面11个字节是实数,4个字节
		for(i = 0;i < INT_REG_POS;i++)
		{
			char_to_float(buf + FRAME_HEAD_NUM + i*4, (fpointer+i));
		}
		#if 0
		// 打印出来看调试结果
		printf("leak_current = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.leak_current.f);
		printf("ref_volt = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.ref_volt.f);
		printf("real_volt = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.real_volt.f);
		printf("spd_temp = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.spd_temp.f);
		printf("envi_temp = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.envi_temp.f);
		printf("id = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.id.f);
		printf("struck_cnt = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.struck_cnt.f);
		printf("struck_total = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.struck_total.f);
		printf("soft_version = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.soft_version.f);
		printf("leak_alarm_threshold = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.leak_alarm_threshold.f);
		printf("day_time = %7.3f \r\n",stuSpd_Param->dSPD_AIdata.day_time.f);
		#endif

		pointer = &stuSpd_Param->dSPD_AIdata[seq].systime_year;
		// 后面的字节是整数,2个字节,另外最后一个数据没有变量对应,舍弃掉
		for(i = INT_REG_POS;i < (SPD_AI_NUM-INT_REG_POS-1);i++)		// 从11项开始,前面的实数占了2个位置，所以减掉
		{
			char_to_int(buf+(FRAME_HEAD_NUM + INT_REG_POS*4)+(i-INT_REG_POS)*2, (pointer+i-INT_REG_POS));
		}
		#if 0
		printf("spd_AI begain\r\n");
		printf("systime_year = %5hd \r\n",stuSpd_Param->dSPD_AIdata.systime_year);
		printf("systime_month = %5hd \r\n",stuSpd_Param->dSPD_AIdata.systime_month);
		printf("systime_day = %5hd \r\n",stuSpd_Param->dSPD_AIdata.systime_day);
		printf("systime_hour = %5hd \r\n",stuSpd_Param->dSPD_AIdata.systime_hour);
		printf("systime_min = %5hd \r\n",stuSpd_Param->dSPD_AIdata.systime_min);
		printf("systime_sec = %5hd \r\n",stuSpd_Param->dSPD_AIdata.systime_sec);

		printf("last_1_struck_year = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_1_struck_year);
		printf("last_1_struck_month = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_1_struck_month);
		printf("last_1_struck_day = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_1_struck_day);
		printf("last_1_struck_hour = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_1_struck_hour);
		printf("last_1_struck_min = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_1_struck_min);
		printf("life_time = %5hd \r\n",stuSpd_Param->dSPD_AIdata.life_time);

		printf("last_2_struck_year = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_2_struck_year);
		printf("last_2_struck_month = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_2_struck_month);
		printf("last_2_struck_day = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_2_struck_day);
		printf("last_2_struck_hour = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_2_struck_hour);
		printf("last_2_struck_min = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_2_struck_min);
		printf("reversed0 = %5hd \r\n",stuSpd_Param->dSPD_AIdata.reversed0);

		printf("last_3_struck_year = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_3_struck_year);
		printf("last_3_struck_month = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_3_struck_month);
		printf("last_3_struck_day = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_3_struck_day);
		printf("last_3_struck_hour = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_3_struck_hour);
		printf("last_3_struck_min = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_3_struck_min);
		printf("reversed1 = %5hd \r\n",stuSpd_Param->dSPD_AIdata.reversed1);

		printf("last_4_struck_year = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_4_struck_year);
		printf("last_4_struck_month = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_4_struck_month);
		printf("last_4_struck_day = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_4_struck_day);
		printf("last_4_struck_hour = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_4_struck_hour);
		printf("last_4_struck_min = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_4_struck_min);
		printf("reversed2 = %5hd \r\n",stuSpd_Param->dSPD_AIdata.reversed2);

		printf("last_5_struck_year = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_5_struck_year);
		printf("last_5_struck_month = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_5_struck_month);
		printf("last_5_struck_day = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_5_struck_day);
		printf("last_5_struck_hour = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_5_struck_hour);
		printf("last_5_struck_min = %5hd \r\n",stuSpd_Param->dSPD_AIdata.last_5_struck_min);
		#endif
	}
}

void DealSPDDiMsg(int seq,unsigned char *buf,unsigned short int len)
{
	assert(seq < SPD_NUM);

	UINT8 i;
	UINT8 *pointer = &stuSpd_Param->dSPD_DI[seq].SPD_DI;

	// 只返回1个字节
	if(len == (1+5))
	{
		printf("LXspd_DI begain->%d\r\n",seq);
		*pointer = *(buf+FRAME_HEAD_NUM);

		printf("SPD_DI = 0x%02x \r\n",stuSpd_Param->dSPD_DI[seq].SPD_DI);
	}
}

void DealSPDDoMsg(int seq,unsigned char *buf,unsigned short int len)
{
	UINT8 i;
	UINT8 *pointer = &stuSpd_Param->dSPD_DO[seq].SPD_DO;

	// 只返回6个字节
	if(len == (1+5))
	{
		printf("LXspd_DO begain->%d\r\n",seq);
		*pointer = *(buf+FRAME_HEAD_NUM);
		printf("SPD_DO = 0x%02x \r\n",stuSpd_Param->dSPD_DO[seq].SPD_DO);
	}
}

void DealSPDResStatusMsg(unsigned char *buf,unsigned short int len)
{
	UINT8 i;
	UINT16 *pointer = &stuSpd_Param->rSPD_res.alarm;
	float res_temp = 0;
	UINT16 dot_num = 0;

	// 返回字节数
	if(len == (SPD_RES_STATUS_NUM*2+5))
	{
		printf("spd_res begain\r\n");
		for (i=0;i<SPD_RES_STATUS_NUM;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i));
		}

		dot_num = stuSpd_Param->rSPD_res.grd_res_dot_num;
		res_temp = (float)stuSpd_Param->rSPD_res.grd_res_value;
		if (dot_num > 0)
		{
			for (i=0;i<dot_num;i++)
			{
				// 实数除法
				res_temp = res_temp/10;
			}
			stuSpd_Param->rSPD_res.grd_res_real = res_temp;
		}
		else
		{
			stuSpd_Param->rSPD_res.grd_res_real = res_temp;
		}
		printf("res_alarm = 0x%02x \r\n",stuSpd_Param->rSPD_res.alarm);
		printf("grd_res_value = 0x%02x \r\n",stuSpd_Param->rSPD_res.grd_res_value);
		printf("grd_res_dot_num = 0x%02x \r\n",stuSpd_Param->rSPD_res.grd_res_dot_num);
		printf("grd_volt = 0x%02x \r\n",stuSpd_Param->rSPD_res.grd_volt);

		printf("test = 0x%02x \r\n",stuSpd_Param->rSPD_res.test);
		printf("id = 0x%02x \r\n",stuSpd_Param->rSPD_res.id);
		printf("alarm_value = 0x%02x \r\n",stuSpd_Param->rSPD_res.alarm_value);

		printf("grd_res_real = %7.3f \r\n",stuSpd_Param->rSPD_res.grd_res_real);
	}
}

// 把和下位机的协议数据转换成和后台的协议数据
void RealDataCopy(int seq,SPD_DATA_LIST msg_t)
{
	assert(seq < SPD_NUM);

	UINT16 *pdes = NULL;
	UINT16 *psrc = NULL;
	UINT8 i;

	switch (msg_t)
	{
	case (SPD_AI_DATA):
		// 不能memset清0, 因为会把DI值清掉
		stuSpd_Param->rSPD_data[seq].id = (UINT16)stuSpd_Param->dSPD_AIdata[seq].id.f;
		stuSpd_Param->rSPD_data[seq].ref_volt = stuSpd_Param->dSPD_AIdata[seq].ref_volt.f;
		stuSpd_Param->rSPD_data[seq].real_volt = stuSpd_Param->dSPD_AIdata[seq].real_volt.f;
		stuSpd_Param->rSPD_data[seq].volt_A = NULL_VALUE;	// 雷迅没有
		stuSpd_Param->rSPD_data[seq].volt_B = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].volt_C = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].leak_current = stuSpd_Param->dSPD_AIdata[seq].leak_current.f;
		stuSpd_Param->rSPD_data[seq].leak_A = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].leak_B = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].leak_C = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].struck_cnt = stuSpd_Param->dSPD_AIdata[seq].struck_cnt.f;
		stuSpd_Param->rSPD_data[seq].struck_total = stuSpd_Param->dSPD_AIdata[seq].struck_total.f;
		stuSpd_Param->rSPD_data[seq].spd_temp = stuSpd_Param->dSPD_AIdata[seq].spd_temp.f;
		stuSpd_Param->rSPD_data[seq].envi_temp = stuSpd_Param->dSPD_AIdata[seq].envi_temp.f;
		stuSpd_Param->rSPD_data[seq].life_time = (float)stuSpd_Param->dSPD_AIdata[seq].life_time;
		stuSpd_Param->rSPD_data[seq].soft_version = stuSpd_Param->dSPD_AIdata[seq].soft_version.f;
		stuSpd_Param->rSPD_data[seq].leak_alarm_threshold = stuSpd_Param->dSPD_AIdata[seq].leak_alarm_threshold.f;
		stuSpd_Param->rSPD_data[seq].day_time = stuSpd_Param->dSPD_AIdata[seq].day_time.f;

		pdes = &stuSpd_Param->rSPD_data[seq].systime_year;
		psrc = &stuSpd_Param->dSPD_AIdata[seq].systime_year;

		// 系统时间
		for(i = 0;i < 6;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 最近1次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_1_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata[seq].last_1_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 倒数第2次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_2_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata[seq].last_2_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 倒数第3次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_3_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata[seq].last_3_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 倒数第4次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_4_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata[seq].last_4_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 倒数第5次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_5_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata[seq].last_5_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		printf("LXspd_real begain,%5hd\r\n",seq);
		printf("leak_current = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].leak_current);
		printf("A_leak_current = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].leak_A);
		printf("B_leak_current = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].leak_B);
		printf("C_leak_current = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].leak_C);
		printf("ref_volt = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].ref_volt);
		printf("real_volt = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].real_volt);
		printf("volt_A = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].volt_A);
		printf("volt_B = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].volt_B);
		printf("volt_C = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].volt_C);
		printf("struck_cnt = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].struck_cnt);
		printf("struck_total = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].struck_total);
		printf("spd_temp = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].spd_temp);
		printf("envi_temp = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].envi_temp);
		printf("id =  %5hd \r\n",stuSpd_Param->rSPD_data[seq].id);
		printf("soft_version = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].soft_version);
		printf("leak_alarm_threshold = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].leak_alarm_threshold);
		printf("day_time = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].day_time);
		printf("life_time = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].life_time);

		printf("systime_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_year);
		printf("systime_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_month);
		printf("systime_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_day);
		printf("systime_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_hour);
		printf("systime_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_min);
		printf("systime_sec = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_sec);

		printf("last_1_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_year);
		printf("last_1_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_month);
		printf("last_1_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_day);
		printf("last_1_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_hour);
		printf("last_1_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_min);

		printf("last_2_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_year);
		printf("last_2_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_month);
		printf("last_2_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_day);
		printf("last_2_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_hour);
		printf("last_2_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_min);

		printf("last_3_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_year);
		printf("last_3_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_month);
		printf("last_3_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_day);
		printf("last_3_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_hour);
		printf("last_3_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_min);

		printf("last_4_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_year);
		printf("last_4_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_month);
		printf("last_4_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_day);
		printf("last_4_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_hour);
		printf("last_4_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_min);

		printf("last_5_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_year);
		printf("last_5_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_month);
		printf("last_5_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_day);
		printf("last_5_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_hour);
		printf("last_5_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_min);
		break;

	case (SPD_DI_DATA):
		// 这2个0：告警，1:正常,所以要反一下
		stuSpd_Param->rSPD_data[seq].DI_C1_status = (stuSpd_Param->dSPD_DI[seq].SPD_DI & BIT(3))?0:1;
		stuSpd_Param->rSPD_data[seq].DI_leak_alarm  = (stuSpd_Param->dSPD_DI[seq].SPD_DI & BIT(6))?0:1;
		// 这里0：正常，1:告警
		stuSpd_Param->rSPD_data[seq].DI_grd_alarm = (stuSpd_Param->dSPD_DI[seq].SPD_DI & BIT(4))?1:0;
		stuSpd_Param->rSPD_data[seq].DI_volt_alarm  = (stuSpd_Param->dSPD_DI[seq].SPD_DI & BIT(7))?1:0;


		printf("C1_status%d = 0x%02x \r\n",seq,stuSpd_Param->rSPD_data[0].DI_C1_status);
		printf("grd_alarm%d = 0x%02x \r\n",seq,stuSpd_Param->rSPD_data[0].DI_grd_alarm);
		printf("leak_alarm%d = 0x%02x \r\n",seq,stuSpd_Param->rSPD_data[0].DI_leak_alarm);
		printf("volt_alarm%d = 0x%02x \r\n",seq,stuSpd_Param->rSPD_data[0].DI_volt_alarm);

		break;

	case (SPD_DO_DATA):
		stuSpd_Param->rSPD_data[seq].DO_spdcnt_clear = (stuSpd_Param->dSPD_DO[seq].SPD_DO & BIT(0))?1:0;
		stuSpd_Param->rSPD_data[seq].DO_totalspdcnt_clear = (stuSpd_Param->dSPD_DO[seq].SPD_DO & BIT(1))?1:0;
		stuSpd_Param->rSPD_data[seq].DO_leak_type = (stuSpd_Param->dSPD_DO[seq].SPD_DO & BIT(2))?1:0;
		stuSpd_Param->rSPD_data[seq].DO_psdtime_clear = (stuSpd_Param->dSPD_DO[seq].SPD_DO & BIT(4))?1:0;
		stuSpd_Param->rSPD_data[seq].DO_daytime_clear = (stuSpd_Param->dSPD_DO[seq].SPD_DO & BIT(5))?1:0;

		printf("DO_leak_type%d = 0x%02x \r\n",seq,stuSpd_Param->rSPD_data[0].DO_leak_type);
		break;

	case (SPD_HZ_DATA):
		//seq = msg_t - SPD_HZ_DATA_1;
		stuSpd_Param->rSPD_data[seq].id = 1;	// 地址是1不会变
		stuSpd_Param->rSPD_data[seq].ref_volt = NULL_VALUE;	// 没有的清0
		stuSpd_Param->rSPD_data[seq].real_volt = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].volt_A = (float)stuSpd_Param->dSPD_HZ[seq].volt_A/10;
		stuSpd_Param->rSPD_data[seq].volt_B = (float)stuSpd_Param->dSPD_HZ[seq].volt_B/10;
		stuSpd_Param->rSPD_data[seq].volt_C = (float)stuSpd_Param->dSPD_HZ[seq].volt_C/10;

		stuSpd_Param->rSPD_data[seq].leak_current = NULL_VALUE;
		// 漏电流转为mA单位
		stuSpd_Param->rSPD_data[seq].leak_A = (float)stuSpd_Param->dSPD_HZ[seq].leak_A/10000;
		stuSpd_Param->rSPD_data[seq].leak_B = (float)stuSpd_Param->dSPD_HZ[seq].leak_B/10000;
		stuSpd_Param->rSPD_data[seq].leak_C = (float)stuSpd_Param->dSPD_HZ[seq].leak_C/10000;

		stuSpd_Param->rSPD_data[seq].DI_C1_status = (float)stuSpd_Param->dSPD_HZ[seq].breaker_alarm;
		stuSpd_Param->rSPD_data[seq].DI_grd_alarm = (float)stuSpd_Param->dSPD_HZ[seq].grd_alarm;
		stuSpd_Param->rSPD_data[seq].struck_cnt = (float)stuSpd_Param->dSPD_HZ[seq].struck_cnt;
		stuSpd_Param->rSPD_data[seq].struck_total = (float)stuSpd_Param->dSPD_HZ[seq].struck_cnt;
		stuSpd_Param->rSPD_data[seq].spd_temp = (float)stuSpd_Param->dSPD_HZ[seq].spd_temp/10;
		stuSpd_Param->rSPD_data[seq].envi_temp = (float)stuSpd_Param->dSPD_HZ[seq].envi_temp/10;
		stuSpd_Param->rSPD_data[seq].life_time = (float)stuSpd_Param->dSPD_HZ[seq].life_time/10;
		stuSpd_Param->rSPD_data[seq].soft_version = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].leak_alarm_threshold = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].day_time = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].DI_bit_0 = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].DI_bit_1 = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].DI_bit_2 = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].DI_bit_5 = NULL_VALUE;
		stuSpd_Param->rSPD_data[seq].DI_leak_alarm = 0;
		stuSpd_Param->rSPD_data[seq].DI_volt_alarm = 0;

		pdes = &stuSpd_Param->rSPD_data[seq].systime_year;
		// 系统时间
		for(i = 0;i < 6;i++)
		{
			*pdes = 0;
			pdes++;
		}
		// 最近1次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_1_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = 0;
			pdes++;
		}
		// 倒数第2次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_2_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = 0;
			pdes++;
		}
		// 倒数第3次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_3_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = 0;
			pdes++;
		}
		// 倒数第4次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_4_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = 0;
			pdes++;
		}
		// 倒数第5次雷击
		pdes = &stuSpd_Param->rSPD_data[seq].last_5_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = 0;
			pdes++;
		}
		printf("HZspd_real begain,%5hd\r\n",seq);
		printf("breaker_alarm = %5hd \r\n",stuSpd_Param->rSPD_data[seq].DI_C1_status);
		printf("grd_alarm = %5hd \r\n",stuSpd_Param->rSPD_data[seq].DI_grd_alarm);
		printf("leak_current = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].leak_current);
		printf("A_leak_current = %7.4f \r\n",stuSpd_Param->rSPD_data[seq].leak_A);
		printf("B_leak_current = %7.4f \r\n",stuSpd_Param->rSPD_data[seq].leak_B);
		printf("C_leak_current = %7.4f \r\n",stuSpd_Param->rSPD_data[seq].leak_C);
		printf("ref_volt = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].ref_volt);
		printf("real_volt = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].real_volt);
		printf("volt_A = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].volt_A);
		printf("volt_B = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].volt_B);
		printf("volt_C = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].volt_C);
		printf("struck_cnt = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].struck_cnt);
		printf("struck_total = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].struck_total);
		printf("spd_temp = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].spd_temp);
		printf("envi_temp = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].envi_temp);
		printf("id =  %5hd \r\n",stuSpd_Param->rSPD_data[seq].id);
		printf("soft_version = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].soft_version);
		printf("leak_alarm_threshold = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].leak_alarm_threshold);
		printf("day_time = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].day_time);
		printf("life_time = %7.3f \r\n",stuSpd_Param->rSPD_data[seq].life_time);

		printf("systime_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_year);
		printf("systime_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_month);
		printf("systime_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_day);
		printf("systime_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_hour);
		printf("systime_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_min);
		printf("systime_sec = %5hd \r\n",stuSpd_Param->rSPD_data[seq].systime_sec);

		printf("last_1_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_year);
		printf("last_1_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_month);
		printf("last_1_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_day);
		printf("last_1_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_hour);
		printf("last_1_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_1_struck_min);

		printf("last_2_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_year);
		printf("last_2_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_month);
		printf("last_2_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_day);
		printf("last_2_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_hour);
		printf("last_2_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_2_struck_min);

		printf("last_3_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_year);
		printf("last_3_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_month);
		printf("last_3_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_day);
		printf("last_3_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_hour);
		printf("last_3_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_3_struck_min);

		printf("last_4_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_year);
		printf("last_4_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_month);
		printf("last_4_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_day);
		printf("last_4_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_hour);
		printf("last_4_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_4_struck_min);

		printf("last_5_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_year);
		printf("last_5_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_month);
		printf("last_5_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_day);
		printf("last_5_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_hour);
		printf("last_5_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[seq].last_5_struck_min);
		break;

	case (SPD_RES_DATA):
		break;

	default:
		break;
	}
}

// 华咨防雷数据的解析
void DealHZSPDMsg(int seq,unsigned char *buf,unsigned short int len)
{
	UINT8 i;
	UINT16 *pointer = &stuSpd_Param->dSPD_HZ[seq].breaker_alarm;

	// 返回46个字节
	if(len == HZ_SPD_LEN)
	{
		//printf("hz_spd_data begain\r\n");

		for (i=0;i<HZ_SPD_DATA_NUM;i++)
		{
			HZ_char_to_int(buf + HZ_HEAD_NUM + i*2, (pointer+i));
			//printf("HZitem%5hd = %5hd \r\n",i,*(pointer+i));
		}
		//printf("life_time = %5hd \r\n",stuSpd_Param->dSPD_HZ[seq].life_time);
	}
}

// 华咨地阻数据的解析
void DealHZResMsg(int seq,unsigned char *buf,unsigned short int len)
{
	UINT8 i;
	UINT16 *pointer = &stuSpd_Param->rSPD_res.grd_res_value;

	// 返回26个字节
	if(len == HZ_RES_LEN)
	{
		// 前面2个字节保留
		HZ_char_to_int(buf + HZ_HEAD_NUM +2*2, pointer);
		stuSpd_Param->rSPD_res.grd_res_real = (float)stuSpd_Param->rSPD_res.grd_res_value/100;
		printf("HZ_grd_res_real = %7.3f \r\n",stuSpd_Param->rSPD_res.grd_res_real);
	}
}


int DealNetSPD(int skt,unsigned char *buf,unsigned short int len)
{
	UINT8 seq = 0;
	int i=0;

	// 当为雷迅则外部的skt没有意义，因为是同一个IP地址来的
	if (SPD_Type == TYPE_LEIXUN)
	{
		for (i=0; i<SPD_NUM+RES_NUM; i++)
		{
			if (buf[0] == SPD_Address[i])
			{
				seq = i;	// 找出返回的数据是对应哪个地址?
				break;
			}
		}
		if (i == (SPD_NUM+RES_NUM))
		{
			return 0;	// 没找到对应的地址，无效数据，直接返回
		}

		// 如果是接地电阻的数据
		if (buf[0] == SPD_Address[SPD_NUM])
		{
			// 根据命令码来进行信息的区分
			switch(buf[1])
			{
				case SPD_RES_READ_CMD:
					DealSPDResStatusMsg(buf, len);
				break;
				default:
				break;
			}
		}
		else
		{
			// 根据命令码来进行信息的区分
			switch(buf[1])
			{
				case SPD_READ_CMD:
					DealSPDAiMsg(seq,buf, len);
					RealDataCopy(seq,SPD_AI_DATA);
				break;
				case SPD_DI_CMD:
					DealSPDDiMsg(seq,buf, len);
					RealDataCopy(seq,SPD_DI_DATA);
				break;
				// DO不处理
				case SPD_DO_CMD:
					DealSPDDoMsg(seq,buf, len);
					RealDataCopy(seq,SPD_DO_DATA);
				break;
				default:
				break;
			}
		}
	}
	else if (SPD_Type == TYPE_HUAZI)
	{
		// 0x64000000开头的数据
		if ((buf[0] == 0x64)&&(buf[1] == 0)&&(buf[2] == 0)&&(buf[3] == 0))
		{
			// 根据长度区分是防雷数据还是地阻数据
			switch(buf[8])
			{
			// 防雷数据长度为46
			case (HZ_SPD_LEN):
				DealHZSPDMsg(skt,buf, len);
				RealDataCopy(skt,SPD_HZ_DATA);
			break;
			// 地阻数据长度为26
			case (HZ_RES_LEN):
				DealHZResMsg(seq,buf, len);
			break;
			default:
			break;
			}
		}
	}
	return 0;
}

/*雷迅的接收线程*/
void *NetWork_DataGet_thread_SPD_L(void *param)
{
	param = NULL;
	int buffPos=0;
	int len,temp = 0;
	unsigned char buf[256];
	static UINT8 first_entry = 0;
	static FDATA dummy;
	static UINT16 dummy_u;
	//gsocket = 0;
	while(1)
	{
		if ((SPD_Type == TYPE_LEIXUN) ||(SPD_Type == TYPE_KY))
		{
	      	len = read(sockfd_spd[0], buf, sizeof(buf)-1);
			if (len >= 0)
			{
			  	buffPos = buffPos+len;
			  	if(buffPos<5) continue;

			  	//CRC
			  	unsigned short int CRC = CRC16(buf,buffPos-2) ;
			  	if((((CRC&0xFF00) >> 8)!= buf[buffPos-2]) || ((CRC&0x00FF) != buf[buffPos-1]))
				{
				  printf("psdCRC error\r\n");
		 		  if(buffPos>=256) buffPos=0;
				  continue ;
			  	}

		      	printf("spd len=%d\r\n",buffPos) ;
			  	/*debug the information*/
				int j ;for(j=0;j<buffPos;j++)printf("0x%02x ",buf[j]);printf("\r\n");
			  	DealNetSPD(0,buf, buffPos);
			  	buffPos=0;
				// 第一次连接对时一次
				if (first_entry == 0)
				{
					first_entry = 1;
					Ex_SPD_Set_Process(SPD_NUM,SPD_TIME_SET,TIME_SET_ADDR,dummy,dummy_u);
				}
			}
			// 断线了
			else
			{
				first_entry = 0;		// 断线重连再次对时
				net_Conneted = 0;
				while(net_Conneted==0)
				{
					// SPD断线了
					printf("spd-break%d\n\r",net_Conneted);
					net_Conneted=obtain_net_psd(0);
					usleep(50000); //delay 50ms
				}
			}
		}
      	usleep(5000); //delay 5ms
	}
}


void udp_param_reset(int Pos)
{
	const char *IPaddress;
	const char * IPport;
	int port;

	IPaddress = gsSPDIP[Pos];	//获取配置文件中的IP地址
	IPport=gsSPDPort[Pos];
	port=atoi(IPport);
    bzero(&HZSPDAddr[Pos],sizeof(HZSPDAddr[Pos]));

    HZSPDAddr[Pos].sin_family = AF_INET;
    HZSPDAddr[Pos].sin_addr.s_addr = inet_addr(IPaddress);
    HZSPDAddr[Pos].sin_port = htons(port);	// Port number
	printf("Pos=%d,hzSPD-IPaddress=%s,hzSPD-IPport=%s\n",Pos,IPaddress,IPport);
}


void SPD_HZ_Data_Get_Func(int seq)
{
	int Pos=seq;
	int len,temp;
	int addr_len =sizeof(struct sockaddr_in);

    if((udpfd_spd[Pos] = socket(AF_INET,SOCK_DGRAM,0))<0){
        perror("socket_1");
        exit(1);
    }

    int nRecvBufLen = 512;
    setsockopt(udpfd_spd[Pos], SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBufLen, sizeof( int ));
	int nSendBufLen = 512;
    setsockopt(udpfd_spd[Pos], SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );
    struct timeval timeout={8,0};//3s
    setsockopt(udpfd_spd[Pos],SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	// 获取配置文件中的IP地址
	udp_param_reset(Pos);

	struct sockaddr_in *Recvaddr = &HZSPDAddr[Pos];
    int DataLen  ;
    char *recvBuf = new char[512];
    while(1)
    {
    	if (SPD_Type == TYPE_HUAZI)
    	{
	    	if (HZ_reset_flag[Pos] == true)
	    	{
	    		HZ_reset_flag[Pos] = false;
				udp_param_reset(Pos);
	    	}
	        memset(recvBuf,0,512);
	        DataLen = recvfrom(udpfd_spd[Pos],recvBuf,512,0,(struct sockaddr *)Recvaddr,(socklen_t*)&addr_len);
	        if(DataLen > 0)
	        {
				// 打印出来
				pthread_mutex_lock(&HZSPDMutex[Pos]);
				printf("spd_HZ_len=%d\r\n",DataLen);
				//int j ;for(j=0;j<DataLen;j++) printf("0x%02x ",recvBuf[j]);printf("\r\n");
				DealNetSPD(Pos,(unsigned char *)recvBuf,DataLen);
				pthread_mutex_unlock(&HZSPDMutex[Pos]);
	        }
    	}
		usleep(5000); //delay 5ms
    }
    close(udpfd_spd[Pos]);
}


/*华咨的防雷器1接收线程*/
void *NetWork_DataGet_thread_SPD_HZ1(void *param)
{
	param = NULL;

	SPD_HZ_Data_Get_Func(0);	// 0,1,2 即SPD1，SPD2, 接地电阻
}


void *NetWork_DataGet_thread_SPD_HZ2(void *param)
{
	param = NULL;

	SPD_HZ_Data_Get_Func(1);
}



void *NetWork_DataGet_thread_SPD_HZRes(void *param)
{
	param = NULL;

	SPD_HZ_Data_Get_Func(SPD_NUM);
}


// 统一创建接收线程
// 宽永和雷迅共用1个线程，2者处理方式是一样的
void DataGet_Thread_Create(UINT16 SPD_t)
{
	//if (SPD_t == TYPE_LEIXUN)
	{
		pthread_t tNetwork_dataget_SPD_L;
		if (pthread_create(&tNetwork_dataget_SPD_L, NULL, NetWork_DataGet_thread_SPD_L,NULL))
		{
			printf("NetWork SPD data create failed!\n");
			WriteLog("NetWork SPD data create failed!\n");
		}
		pthread_detach(tNetwork_dataget_SPD_L);
	}

	//else if (SPD_t == TYPE_HUAZI)
	{
		pthread_mutex_init(&HZSPDMutex[0],NULL);
		pthread_mutex_init(&HZSPDMutex[1],NULL);
		pthread_mutex_init(&HZSPDMutex[2],NULL);

		pthread_t tNetwork_dataget_SPD_HZ1;
		if (pthread_create(&tNetwork_dataget_SPD_HZ1, NULL, NetWork_DataGet_thread_SPD_HZ1,NULL))
		{
			printf("NetWork SPD_1 data create failed!\n");
			WriteLog("NetWork SPD_1 data create failed!\n");
		}
		pthread_detach(tNetwork_dataget_SPD_HZ1);

		// 华咨的SPD2
		pthread_t tNetwork_dataget_SPD_HZ2;
		if (pthread_create(&tNetwork_dataget_SPD_HZ2, NULL, NetWork_DataGet_thread_SPD_HZ2,NULL))
		{
			printf("NetWork SPD_2 data create failed!\n");
			WriteLog("NetWork SPD_2 data create failed!\n");
		}
		pthread_detach(tNetwork_dataget_SPD_HZ2);

		pthread_t tNetwork_dataget_SPD_HZres;
		if (pthread_create(&tNetwork_dataget_SPD_HZres, NULL, NetWork_DataGet_thread_SPD_HZRes,NULL))
		{
			printf("NetWork SPD_RES data create failed!\n");
			WriteLog("NetWork SPD_RES data create failed!\n");
		}
		pthread_detach(tNetwork_dataget_SPD_HZres);
	}
}


void* NetWork_server_thread_SPD(void*arg)
{
	char str[10*1024];
	int i,j,temp;
	int port,nlen;
	struct sockaddr_in net_spd_addr;
	unsigned short crc_spd;
	//int bFlag=0;
	static UINT16 op_counter = 0;
	static UINT16 poll_cnt = 0;
	static UINT16 seq_cnt = 0;		// 标志现在轮询到了几个寄存器了
	// 线程开始测试一次电阻，不然要等10分钟, 延时20s,先对时
	static UINT32 ctrl_counter = (SPD_TEST_RES_INTERVAL-10);
	FDATA dummy;
	UINT16 dummy_u;
	static UINT16 try_connect[SPD_NUM+RES_NUM] = {0,};
	static UINT16 anyone_connect = 0;	// 如果任何一个设备已经连接

	for (i = 0; i < SPD_NUM; i++)
	{
		memset (&SPD_ctrl_value[i],0,sizeof(SPD_CTRL_VALUE));
	}
	dummy.f=NULL_VAR;

	#if 0
	// 测试时定义的数据
	static UINT16 testvalue = 9;
	FDATA test_v;
	static UINT16 entry = 0;
	#endif

	temp=0;
	if ((SPD_Type == TYPE_LEIXUN) || (SPD_Type == TYPE_KY))
	{
		net_Conneted=0;		// 开始连接前置0
		while(net_Conneted==0)
		{
			// 雷迅的防雷只有1个IP地址
			net_Conneted=obtain_net_psd(0);
			sprintf(str,"net_Conneted=%d\n",net_Conneted);
			WriteLog(str);
			if(net_Conneted==0)
			{
				printf("connect _psd error\n");
				WriteLog("IN NETWORK_Server_thread connect _psd error!\n");
				sleep(1);
			}
		}
	}
	else
	{
		// 连一次，给sockfd_spd一个值，不然切换的时候会出现异常
		obtain_net_psd(0);
	}
	// 连接成功后再创建接收的线程
	DataGet_Thread_Create(SPD_Type);

	sleep(2);	//连接后等待2s稳定
	while(1)
	{
		#if 0
		if (entry == 0)
		{
			entry = 1;
			Ex_SPD_Set_Process(SPD_RES_SET,RES_ALARM_ADDR,dummy,0);
			printf("SPD_RES_SET\n\r");
			usleep(500000);
		}
		#endif
		if (SPD_Type == TYPE_HUAZI)
		{
			// 如果是华咨,没有设置参数，忽略
			for (i = 0; i < SPD_NUM; i++)
			{
				spd_ctl_flag[i] = 0;
			}
		}
		if ((spd_ctl_flag[0]&BITS_MSK_GET(0,SPD_CTRL_NUM)) ||(spd_ctl_flag[1]&BITS_MSK_GET(0,SPD_CTRL_NUM)))
		{
			spd_net_flag[0] = 0;
			spd_net_flag[1] = 0;
		}
		else
		{
			poll_cnt++;
			#if 0
			// 改id测试
			if (poll_cnt == 1)
			{
				test_v.f =testvalue;
				Ex_SPD_Set_Process(SPD_RES_SET,RES_ID_ADDR,dummy,testvalue);
				testvalue++;
				if (testvalue >=11)
				{
					testvalue = 9;
				}
				usleep(500000);
			}
			#endif
			#if 0
			// DO清0测试
			if (poll_cnt == 2)
			{
				Ex_SPD_Set_Process(SPD_DO_SET,testvalue,dummy,DO_OFF_CMD);
				testvalue++;
				if (testvalue >= 6)
				{
					testvalue = 0;
				}
				usleep(500000);
			}
			#endif


			// 400*3 = 1.2s，即1.2s轮询一个项目
			if (poll_cnt >= SPD_POLLING_INTERVAL)
			{
				poll_cnt = 0;
				if (SPD_Type == TYPE_LEIXUN)
				{
					if (op_counter >= SPD_DATA_NUM)
					{
						op_counter = 0;
						seq_cnt++;
						// 按config设置的数量来
						if (seq_cnt >= SPD_num)
						{
							seq_cnt = 0;
						}
					}
					// 要放在下面，否则不会执行上面的if语句
					else if (op_counter >= SPD_HZ_DATA)
					{
						op_counter = SPD_RES_DATA;	// 跳过第二个防雷器
					}
					if ((ctrl_counter % SPD_TEST_RES_INTERVAL) ==0)
					{
						// 接地电阻10分钟测试一次
						Ex_SPD_Set_Process(SPD_NUM,SPD_RES_SET,RES_TEST_ADDR,dummy,RES_TEST_EN);
					}
					if (ctrl_counter >= SPD_TIME_SYN_INTERVAL)
					{
						ctrl_counter = 0;
						Ex_SPD_Set_Process(SPD_NUM,SPD_TIME_SET,TIME_SET_ADDR,dummy,dummy_u);
					}
					ctrl_counter++;		// 每隔一段时间进行一次接地电阻测试
					//printf("ctrl_counter = 0x%08x \r\n",ctrl_counter);
				}
				else if (SPD_Type == TYPE_HUAZI)
				{
					if (op_counter < SPD_HZ_DATA)
					{
						op_counter = SPD_HZ_DATA;
					}

					if ((op_counter > SPD_RES_DATA) || (op_counter >= SPD_DATA_NUM))
					{
						op_counter = SPD_HZ_DATA;
						seq_cnt++;
						// 按config设置的数量来
						if (seq_cnt >= SPD_num)
						{
							seq_cnt = 0;
						}
					}
					// 要放在下面,否则上面的if不会执行
					else if (op_counter > SPD_HZ_DATA)
					{
						op_counter = SPD_RES_DATA;
					}
				}

				// 宽永的处理
				else if (SPD_Type == TYPE_KY)
				{
					if (op_counter <= SPD_RUN_DATA)
					{
						op_counter = SPD_RUN_DATA;	// 防止异常值
					}
					if (op_counter >= SPD_DATA_NUM)
					{
						op_counter = SPD_RUN_DATA;
						seq_cnt++;
						// 按config设置的数量来
						if (seq_cnt >= SPD_num)
						{
							seq_cnt = 0;
						}
					}
				}

				if (seq_cnt < SPD_NUM)
				{
					spd_net_flag[seq_cnt] |= BIT(op_counter);
					printf("seq_cnt=%d,spd_net_flag=%02x",seq_cnt,spd_net_flag[seq_cnt]);
				}
				op_counter++;// 轮询间隔标志
			}
		}

		// 统一处理的设置函数，这种结构注定上位机只能一次设置一个，且要有个间隔时间,400ms
		for (i=0; i<SPD_NUM; i++)
		{
			if (spd_ctl_flag[i]&BITS_MSK_GET(0,SPD_CTRL_NUM))
			{
				spd_ctrl_process(i,&spd_ctl_flag[i]);
				break;	// 有事件发生直接跳出循环，下次再轮询
			}

			if (spd_net_flag[i]&BITS_MSK_GET(0,SPD_DATA_NUM))
			{
				spd_send_process(i,&spd_net_flag[i]);
				break;	// 有事件发生直接跳出循环，下次再轮询
			}
		}

	#if 0
		if (spd_ctl_flag[0]&BIT(SPD_AI_SET))
		{
			spd_ctrl_process(0,&spd_ctl_flag[0],SPD_AI_SET);
		}
		else if (spd_ctl_flag[0]&BIT(SPD_DO_SET))
		{
			spd_ctrl_process(0,&spd_ctl_flag[0],SPD_DO_SET);
		}
		else if (spd_ctl_flag[0]&BIT(SPD_TIME_SET))
		{
			spd_ctrl_process(0,&spd_ctl_flag[0],SPD_TIME_SET);
		}
		else if (spd_ctl_flag[0]&BIT(SPD_RES_SET))
		{
			spd_ctrl_process(0,&spd_ctl_flag[0],SPD_RES_SET);
		}

		else if (spd_ctl_flag[1]&BIT(SPD_AI_SET))
		{
			spd_ctrl_process(1,&spd_ctl_flag[1],SPD_AI_SET);
		}
		else if (spd_ctl_flag[1]&BIT(SPD_DO_SET))
		{
			spd_ctrl_process(1,&spd_ctl_flag[1],SPD_DO_SET);
		}
		else if (spd_ctl_flag[1]&BIT(SPD_TIME_SET))
		{
			spd_ctrl_process(1,&spd_ctl_flag[1],SPD_TIME_SET);
		}
		// 接地电阻只有1个，这里是多余的
		else if (spd_ctl_flag[1]&BIT(SPD_RES_SET))
		{
			spd_ctl_flag[1] &= ~(BIT(SPD_RES_SET));
			//spd_ctrl_process(1,&spd_ctl_flag,SPD_RES_SET);
		}


		// 设置完毕，开始轮询
		else if (spd_net_flag[0]&BIT(SPD_AI_DATA))
		{
			spd_send_process(0,&spd_net_flag[0],SPD_AI_DATA);
		}
		else if (spd_net_flag[0]&BIT(SPD_DI_DATA))
		{
			spd_send_process(0,&spd_net_flag[0],SPD_DI_DATA);
		}
		else if (spd_net_flag[0]&BIT(SPD_DO_DATA))
		{
			spd_send_process(0,&spd_net_flag[0],SPD_DO_DATA);
		}

		// 宽永的
		else if (spd_net_flag[0]&BIT(SPD_RUN_DATA))
		{
			spd_send_process(0,&spd_net_flag[0],SPD_RUN_DATA);
		}
		else if (spd_net_flag[0]&BIT(SPD_REMOTE_DATA))
		{
			spd_send_process(0,&spd_net_flag[0],SPD_REMOTE_DATA);
		}
		else if (spd_net_flag[0]&BIT(SPD_REMOTE_DATA))
		{
			spd_send_process(0,&spd_net_flag[0],SPD_REMOTE_DATA);
		}

		// 华咨的
		else if (spd_net_flag[0]&BIT(SPD_HZ_DATA))
		{
			spd_send_process(0,&spd_net_flag[0],SPD_HZ_DATA);
		}
		// 接地电阻,共用的
		else if (spd_net_flag[0]g&BIT(SPD_RES_DATA))
		{
			spd_send_process(0,&spd_net_flag,SPD_RES_DATA);
		}


		// 第二套轮询
		else if (spd_net_flag[1]&BIT(SPD_AI_DATA))
		{
			spd_send_process(1,&spd_net_flag[1],SPD_AI_DATA);
		}
		else if (spd_net_flag[1]&BIT(SPD_DI_DATA))
		{
			spd_send_process(1,&spd_net_flag[1],SPD_DI_DATA);
		}
		else if (spd_net_flag[1]&BIT(SPD_DO_DATA))
		{
			spd_send_process(1,&spd_net_flag[1],SPD_DO_DATA);
		}

		else if (spd_net_flag[1]&BIT(SPD_HZ_DATA))
		{
			spd_send_process(1,&spd_net_flag[1],SPD_HZ_DATA);
		}
		else if (spd_net_flag[1]&BIT(SPD_RES_DATA))
		{
			// 只有1套接地
			spd_net_flag[1] &= ~(BIT(SPD_RES_DATA));
			//spd_send_process(0,&spd_net_flag,SPD_RES_DATA);
		}
		#endif
		usleep(SPD_INTERVAL_TIME);	// 参数设置间隔
	}
}


void init_net_spd()
{
	// 数据发送线程
	pthread_t tNetwork_server_SPD;
	if (pthread_create(&tNetwork_server_SPD, NULL, NetWork_server_thread_SPD,NULL))
	{
		printf("NetWork SPD create failed!\n");
		WriteLog("NetWork SPD create failed!\n");
	}
	pthread_detach(tNetwork_server_SPD);

	pthread_mutex_init(&SPDdataHandleMutex,NULL);
}

