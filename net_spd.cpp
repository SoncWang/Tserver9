#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
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

UINT16  spd_net_flag = 0;	// SPD轮询标志
UINT16  spd_ctl_flag = 0;	// SPD控制标志

UINT8  WAIT_net_res_flag=0;	// 等待信息回应

UINT16 gsocket = 0;
UINT16 net_Conneted = 0;	// 连接标志1:已经连接
//UINT16 HZ_Conneted[SPD_NUM+RES_NUM] = {0,};	// 连接标志1:已经连接


UINT8 SPD_Type = TYPE_LEIXUN;
UINT8 SPD_num = SPD_NUM;
UINT8 SPD_Address[SPD_NUM+RES_NUM] = {SPD_ADDR,SPD_ADDR,SPD_RES_ADDR};
//UINT8 SPD_Res_Address = SPD_RES_ADDR;
SPD_CTRL_VALUE SPD_ctrl_value;

pthread_mutex_t SPDdataHandleMutex;


//extern RSUCONTROLER stuRsuControl;	//RSU控制器状态
//extern RSU_DATA_INIT stuRsuData;	//RSU设备信息结构体
//extern RSU_RESET stuRsuReset;			//天线软件复位状态结构体
extern char gsSPDIP[SPD_NUM+RES_NUM][20];	//SPD-IP地址
extern char gsSPDPort[SPD_NUM+RES_NUM][10];	//SPD-端口
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体

// 华咨的网络有关变量定义
struct sockaddr_in HZSPDAddr[SPD_NUM+RES_NUM];
string strHZSPDdata[SPD_NUM+RES_NUM] = {"",};

pthread_mutex_t HZSPDMutex1;
pthread_mutex_t HZSPDMutex2;
pthread_mutex_t HZSPDMutexRes;



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
	// 这2项对应的是华咨的,但是华咨不是MODBUS格式，所以没用
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
int Ex_SPD_Set_Process(SPD_CTRL_LIST SPD_ctrl_event, UINT8 set_addr, FDATA ai_data,UINT16 data)
{
	spd_ctl_flag |= BIT(SPD_ctrl_event);	//先置标志位

	SPD_ctrl_value.ref_addr = set_addr;
	if (SPD_ctrl_event == SPD_AI_SET)
	{
		SPD_ctrl_value.f_ai_set = ai_data;
	}
	else if (SPD_ctrl_event == SPD_DO_SET)
	{
		SPD_ctrl_value.do_set = data;
	}
	else if (SPD_ctrl_event == SPD_RES_SET)
	{
		SPD_ctrl_value.res_set = data;
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

	printf("psd HZRdata->socket_addr=%d:",sockfd_spd[seq]);
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	//write(socket_addr,bytSend,len);
	sendto(sockfd_spd[seq], bytSend, REFS_COUNT, 0, (struct sockaddr *)&HZSPDAddr[seq], sizeof(struct sockaddr_in));
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

	printf("psd Ddata->socket_addr=%d:",socket_addr);
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");
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

	printf("psd Cdata->socket_addr=%d:",socket_addr);
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");
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

	printf("psd ResSetdata:");
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");
	write(socket_addr,bytSend,len);

	pthread_mutex_unlock(&SPDdataHandleMutex);
	usleep(5000);	//delay 5ms
	return 0 ;
}


#if 0
void spd_send_process(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
	static UINT16 op_counter = 0;

	switch(op_counter)
	{
		case (0):
			SendPSDReadReg(PSD_ADDR,g_SPD_Function_Array[SPD_AI_DATA].func_code,\
				g_SPD_Function_Array[SPD_AI_DATA].reg_addr,g_SPD_Function_Array[SPD_AI_DATA].reg_num);	//读取AI模拟量
		break;

		case (1):
			SendPSDReadReg(PSD_ADDR,g_SPD_Function_Array[SPD_DI_DATA].func_code,\
				g_SPD_Function_Array[SPD_DI_DATA].reg_addr,g_SPD_Function_Array[SPD_DI_DATA].reg_num);	//读取AI模拟量
		break;

		case (2):
			SendPSDReadReg(PSD_ADDR,g_SPD_Function_Array[SPD_DO_DATA].func_code,\
				g_SPD_Function_Array[SPD_DO_DATA].reg_addr,g_SPD_Function_Array[SPD_DO_DATA].reg_num);	//读取AI模拟量
		break;

		default:
		break;
	}

	op_counter++;
	if (op_counter >= SPD_DATA_NUM)
	{
		op_counter = 0;
	}
}
#endif
#if 0
void send_RSU(char command,bool ReSend,char state,int num)
{
	unsigned char send_buff[128];
	memset(send_buff,0,sizeof(send_buff));
	char buff_len = 0;
	int i;
	unsigned short crc;
	//----处理帧序号-------
	if(!ReSend)
	{
		if(rsctl_all == 0)
		{
			rsctl_all = 0x10;
		}
		else
		{
			rsctl_all += 0x10;
			if(rsctl_all > 0x90)
			{
				rsctl_all = 0x10;
			}
		}
	}
	//---------------------
	send_buff[0] = 0xff;
	send_buff[1] = 0xff;
	send_buff[2] = 0x00;
	send_buff[3] = rsctl_all;
	switch(command)
	{
		case 0xC4:				   //打开，关闭复位天线指令
		send_buff[4] = 0x00;		//数据长度四个字节
		send_buff[5] = 0x00;
		send_buff[6] = 0x00;
		send_buff[7] = 3;
		send_buff[8] = 0xc4;
		if(state==0x01)
		{
			send_buff[9] = 0x01;	//打开天线
		}
		else if(state==0x00)
		{
			send_buff[9] = 0x00;	//关闭天线
		}
		send_buff[10]=num;		//对哪台天线进行操作
		buff_len=11;
		break;
		case 0xC0:		   //获取天线配置指令
		send_buff[4] = 0x00;		//数据长度四个字节
		send_buff[5] = 0x00;
		send_buff[6] = 0x00;
		send_buff[7] = 18;
		send_buff[8] = 0xC0;
		for(i=0;i<17;i++)
		{
			send_buff[9+i] = 0x00;
		}
		buff_len=26;
		break;
		case 0x1D:
		send_buff[4] = 0x00;		//数据长度四个字节
		send_buff[5] = 0x00;
		send_buff[6] = 0x00;
		send_buff[7] = 2;
		send_buff[8] = 0x1D;
		send_buff[9] = num;
		buff_len=10;
		break;
	}
	/*for(i=2;i<buff_len;i++)
	{
		crc=crc_ccitt_byte(crc,send_buff[i]);
	}*/
	crc=CRC16_pc(send_buff+2,buff_len-2);
	//crc ^= 0xffff;
	send_buff[buff_len] = ((crc&0xff00)>>8);
	send_buff[buff_len+1] = (crc&0x00ff);
	buff_len=buff_len+2;
	printf("sent_rsu data:");
	for(i=0;i<buff_len;i++)
	{
		printf("%x-",send_buff[i]);
	}
	printf("\n");
	write(sockfd_rsu,send_buff,buff_len);
}
#endif


/*模块化spd数据读取*/
int spd_send_process(UINT16 *pnet_flag, SPD_DATA_LIST SPD_data_event)
{
	UINT8 addr_temp;
	UINT8 func_temp = HZ_SPD_READ;;
	int seq = 0;	// 表明这是哪一台装置

	if (SPD_data_event >= SPD_DATA_NUM)
	{
		return 0;
	}

	if (SPD_Type == TYPE_LEIXUN)
	{
		seq = 0;
		if (SPD_data_event < SPD_RES_DATA)
		{
			addr_temp = SPD_Address[0];
		}
		else
		{
			addr_temp = SPD_Address[SPD_NUM];	// 接地电阻仪地址
		}
		if (*pnet_flag & (BIT(SPD_data_event)))
		{
			*pnet_flag &= ~(BIT(SPD_data_event));
			//读取防雷器和接地电阻的数据
			SPD_Read_Reg(seq,addr_temp, g_SPD_Function_Array[SPD_data_event].func_code,\
					g_SPD_Function_Array[SPD_data_event].reg_addr,g_SPD_Function_Array[SPD_data_event].reg_num);
		}
	}
	else
	{
		if (SPD_data_event == SPD_HZ_DATA_1)
		{
			seq = 0;
			addr_temp = SPD_Address[0];
			func_temp = HZ_SPD_READ;
		}
		else if (SPD_data_event == SPD_HZ_DATA_2)
		{
			seq = 1;
			addr_temp = SPD_Address[1];
			func_temp = HZ_SPD_READ;
		}
		else
		{
			seq = 2;
			addr_temp = SPD_Address[SPD_NUM];	// 接地电阻仪地址
			func_temp = HZ_RES_READ;
		}

		if (*pnet_flag & (BIT(SPD_data_event)))
		{
			*pnet_flag &= ~(BIT(SPD_data_event));
			//读取防雷器和接地电阻的数据
			SPD_HZ_Read_Reg(seq,HZ_ADDR, func_temp,NULL_VAR,24);
		}
	}
	return 1;
}


/*模块化spd数据设置*/
int spd_ctrl_process(UINT16 *pctrl_flag, SPD_CTRL_LIST SPD_ctrl_event)
{
	UINT8 addr_temp;
	int socketq = 0;	// 表明这是哪一台装置

	if (SPD_ctrl_event >= SPD_CTRL_NUM)
	{
		return 0;
	}

	if (SPD_Type == TYPE_LEIXUN)
	{
		socketq = sockfd_spd[0];
		if (SPD_ctrl_event < SPD_RES_SET)	// 防雷检测地址
		{
			addr_temp = SPD_Address[0];
		}
		else
		{
			addr_temp = SPD_Address[SPD_NUM];	// 接地电阻仪地址
		}
	}

	if (*pctrl_flag & (BIT(SPD_ctrl_event)))
	{
		*pctrl_flag &= ~(BIT(SPD_ctrl_event));
		switch(SPD_ctrl_event)
		{
		case SPD_AI_SET:
			//设置防雷器的AI数据
			SPD_AI_Set_Reg(socketq,addr_temp,SPD_WRITE_CMD,SPD_ctrl_value.ref_addr,AI_SET_MIN,&SPD_ctrl_value.f_ai_set);
			// 改了id后要更新地址
			if (SPD_ctrl_value.ref_addr == SPD_ID_ADDR)
			{
				SPD_Address[0] = (UINT8)SPD_ctrl_value.f_ai_set.f;
			}
			break;

		case SPD_DO_SET:
			// DO每次只写1个寄存器
			SPD_DO_Ctrl_Reg(socketq,addr_temp,SPD_DO_CTRL_CMD,SPD_ctrl_value.ref_addr,SPD_ctrl_value.do_set);
			break;

		case SPD_RES_SET:
			// RES接地电阻部分设置
			SPD_Res_Set_Reg(socketq,addr_temp,SPD_RES_SET_CMD,SPD_ctrl_value.ref_addr,SPD_ctrl_value.res_set);
			// 改了id后要更新地址
			if (SPD_ctrl_value.ref_addr == RES_ID_ADDR)
			{
				SPD_Address[SPD_NUM] = (UINT8)SPD_ctrl_value.res_set;
			}
			break;
		default:
			break;
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

void DealSPDAiMsg(unsigned char *buf,unsigned short int len)
{
	UINT8 i;
	FDATA *fpointer = &stuSpd_Param->dSPD_AIdata.leak_current;
	UINT16 *pointer = &stuSpd_Param->dSPD_AIdata.systime_year;

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

		pointer = &stuSpd_Param->dSPD_AIdata.systime_year;
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

void DealSPDDiMsg(unsigned char *buf,unsigned short int len)
{
	UINT8 i;
	UINT8 *pointer = &stuSpd_Param->dSPD_DI.SPD_DI;

	// 只返回1个字节
	if(len == (1+5))
	{
		printf("spd_DI begain\r\n");
		*pointer = *(buf+FRAME_HEAD_NUM);

		printf("SPD_DI = 0x%02x \r\n",stuSpd_Param->dSPD_DI.SPD_DI);
	}
}

void DealSPDDoMsg(unsigned char *buf,unsigned short int len)
{
	UINT8 i;
	UINT8 *pointer = &stuSpd_Param->dSPD_DO.SPD_DO;

	// 只返回6个字节
	if(len == (1+5))
	{
		printf("spd_DO begain\r\n");
		*pointer = *(buf+FRAME_HEAD_NUM);
		printf("SPD_DO = 0x%02x \r\n",stuSpd_Param->dSPD_DO.SPD_DO);
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
void RealDataCopy(SPD_DATA_LIST msg_t)
{
	UINT16 *pdes = NULL;
	UINT16 *psrc = NULL;
	UINT8 i;
	UINT8 seq = 0;

	switch (msg_t)
	{
	case (SPD_AI_DATA):
		stuSpd_Param->rSPD_data[0].id = (UINT16)stuSpd_Param->dSPD_AIdata.id.f;
		stuSpd_Param->rSPD_data[0].ref_volt = stuSpd_Param->dSPD_AIdata.ref_volt.f;
		stuSpd_Param->rSPD_data[0].real_volt = stuSpd_Param->dSPD_AIdata.real_volt.f;
		stuSpd_Param->rSPD_data[0].volt_A = 0;	// 雷迅没有
		stuSpd_Param->rSPD_data[0].volt_B = 0;
		stuSpd_Param->rSPD_data[0].volt_C = 0;
		stuSpd_Param->rSPD_data[0].leak_current = stuSpd_Param->dSPD_AIdata.leak_current.f;
		stuSpd_Param->rSPD_data[0].leak_A = 0;
		stuSpd_Param->rSPD_data[0].leak_B = 0;
		stuSpd_Param->rSPD_data[0].leak_C = 0;
		stuSpd_Param->rSPD_data[0].struck_cnt = stuSpd_Param->dSPD_AIdata.struck_cnt.f;
		stuSpd_Param->rSPD_data[0].struck_total = stuSpd_Param->dSPD_AIdata.struck_total.f;
		stuSpd_Param->rSPD_data[0].spd_temp = stuSpd_Param->dSPD_AIdata.spd_temp.f;
		stuSpd_Param->rSPD_data[0].envi_temp = stuSpd_Param->dSPD_AIdata.envi_temp.f;
		stuSpd_Param->rSPD_data[0].life_time = (float)stuSpd_Param->dSPD_AIdata.life_time;
		stuSpd_Param->rSPD_data[0].soft_version = stuSpd_Param->dSPD_AIdata.soft_version.f;
		stuSpd_Param->rSPD_data[0].leak_alarm_threshold = stuSpd_Param->dSPD_AIdata.leak_alarm_threshold.f;
		stuSpd_Param->rSPD_data[0].day_time = stuSpd_Param->dSPD_AIdata.day_time.f;

		pdes = &stuSpd_Param->rSPD_data[0].systime_year;
		psrc = &stuSpd_Param->dSPD_AIdata.systime_year;

		// 系统时间
		for(i = 0;i < 6;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 最近1次雷击
		pdes = &stuSpd_Param->rSPD_data[0].last_1_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata.last_1_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 倒数第2次雷击
		pdes = &stuSpd_Param->rSPD_data[0].last_2_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata.last_2_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 倒数第3次雷击
		pdes = &stuSpd_Param->rSPD_data[0].last_3_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata.last_3_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 倒数第4次雷击
		pdes = &stuSpd_Param->rSPD_data[0].last_4_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata.last_4_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		// 倒数第5次雷击
		pdes = &stuSpd_Param->rSPD_data[0].last_5_struck_year;
		psrc = &stuSpd_Param->dSPD_AIdata.last_5_struck_year;
		for(i = 0;i < 5;i++)
		{
			*pdes = *psrc;
			pdes++;
			psrc++;
		}
		printf("spd_real begain\r\n");
		printf("leak_current = %7.3f \r\n",stuSpd_Param->rSPD_data[0].leak_current);
		printf("A_leak_current = %7.3f \r\n",stuSpd_Param->rSPD_data[0].leak_A);
		printf("B_leak_current = %7.3f \r\n",stuSpd_Param->rSPD_data[0].leak_B);
		printf("C_leak_current = %7.3f \r\n",stuSpd_Param->rSPD_data[0].leak_C);
		printf("ref_volt = %7.3f \r\n",stuSpd_Param->rSPD_data[0].ref_volt);
		printf("real_volt = %7.3f \r\n",stuSpd_Param->rSPD_data[0].real_volt);
		printf("volt_A = %7.3f \r\n",stuSpd_Param->rSPD_data[0].volt_A);
		printf("volt_B = %7.3f \r\n",stuSpd_Param->rSPD_data[0].volt_B);
		printf("volt_C = %7.3f \r\n",stuSpd_Param->rSPD_data[0].volt_C);
		printf("struck_cnt = %7.3f \r\n",stuSpd_Param->rSPD_data[0].struck_cnt);
		printf("struck_total = %7.3f \r\n",stuSpd_Param->rSPD_data[0].struck_total);
		printf("spd_temp = %7.3f \r\n",stuSpd_Param->rSPD_data[0].spd_temp);
		printf("envi_temp = %7.3f \r\n",stuSpd_Param->rSPD_data[0].envi_temp);
		printf("id =  %5hd \r\n",stuSpd_Param->rSPD_data[0].id);
		printf("soft_version = %7.3f \r\n",stuSpd_Param->rSPD_data[0].soft_version);
		printf("leak_alarm_threshold = %7.3f \r\n",stuSpd_Param->rSPD_data[0].leak_alarm_threshold);
		printf("day_time = %5hd \r\n",stuSpd_Param->rSPD_data[0].day_time);
		printf("life_time = %5hd \r\n",stuSpd_Param->rSPD_data[0].life_time);

		printf("systime_year = %5hd \r\n",stuSpd_Param->rSPD_data[0].systime_year);
		printf("systime_month = %5hd \r\n",stuSpd_Param->rSPD_data[0].systime_month);
		printf("systime_day = %5hd \r\n",stuSpd_Param->rSPD_data[0].systime_day);
		printf("systime_hour = %5hd \r\n",stuSpd_Param->rSPD_data[0].systime_hour);
		printf("systime_min = %5hd \r\n",stuSpd_Param->rSPD_data[0].systime_min);
		printf("systime_sec = %5hd \r\n",stuSpd_Param->rSPD_data[0].systime_sec);

		printf("last_1_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_1_struck_year);
		printf("last_1_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_1_struck_month);
		printf("last_1_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_1_struck_day);
		printf("last_1_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_1_struck_hour);
		printf("last_1_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_1_struck_min);

		printf("last_2_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_2_struck_year);
		printf("last_2_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_2_struck_month);
		printf("last_2_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_2_struck_day);
		printf("last_2_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_2_struck_hour);
		printf("last_2_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_2_struck_min);

		printf("last_3_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_3_struck_year);
		printf("last_3_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_3_struck_month);
		printf("last_3_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_3_struck_day);
		printf("last_3_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_3_struck_hour);
		printf("last_3_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_3_struck_min);

		printf("last_4_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_4_struck_year);
		printf("last_4_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_4_struck_month);
		printf("last_4_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_4_struck_day);
		printf("last_4_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_4_struck_hour);
		printf("last_4_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_4_struck_min);

		printf("last_5_struck_year = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_5_struck_year);
		printf("last_5_struck_month = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_5_struck_month);
		printf("last_5_struck_day = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_5_struck_day);
		printf("last_5_struck_hour = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_5_struck_hour);
		printf("last_5_struck_min = %5hd \r\n",stuSpd_Param->rSPD_data[0].last_5_struck_min);
		break;

	case (SPD_DI_DATA):
		// 这3个0：告警，1:正常,所以要反一下
		stuSpd_Param->rSPD_data[0].DI_C1_status = (stuSpd_Param->dSPD_DI.SPD_DI & BIT(3))?0:1;
		stuSpd_Param->rSPD_data[0].DI_grd_alarm = (stuSpd_Param->dSPD_DI.SPD_DI & BIT(4))?0:1;
		stuSpd_Param->rSPD_data[0].DI_leak_alarm  =  (stuSpd_Param->dSPD_DI.SPD_DI & BIT(6))?0:1;
		// 这里0：正常，1:告警
		stuSpd_Param->rSPD_data[0].DI_volt_alarm  =  (stuSpd_Param->dSPD_DI.SPD_DI & BIT(7))?1:0;

		printf("C1_status = 0x%02x \r\n",stuSpd_Param->rSPD_data[0].DI_C1_status);
		printf("grd_alarm = 0x%02x \r\n",stuSpd_Param->rSPD_data[0].DI_grd_alarm);
		printf("leak_alarm = 0x%02x \r\n",stuSpd_Param->rSPD_data[0].DI_leak_alarm);
		printf("volt_alarm = 0x%02x \r\n",stuSpd_Param->rSPD_data[0].DI_volt_alarm);
		break;

	case (SPD_DO_DATA):
		stuSpd_Param->rSPD_data[0].DO_spdcnt_clear = (stuSpd_Param->dSPD_DO.SPD_DO & BIT(0))?1:0;
		stuSpd_Param->rSPD_data[0].DO_totalspdcnt_clear = (stuSpd_Param->dSPD_DO.SPD_DO & BIT(1))?1:0;
		stuSpd_Param->rSPD_data[0].DO_leak_type = (stuSpd_Param->dSPD_DO.SPD_DO & BIT(2))?1:0;
		stuSpd_Param->rSPD_data[0].DO_psdtime_clear = (stuSpd_Param->dSPD_DO.SPD_DO & BIT(4))?1:0;
		stuSpd_Param->rSPD_data[0].DO_daytime_clear = (stuSpd_Param->dSPD_DO.SPD_DO & BIT(5))?1:0;

		printf("DO_leak_type = 0x%02x \r\n",stuSpd_Param->rSPD_data[0].DO_leak_type);
		break;

	case (SPD_HZ_DATA_1):
	case (SPD_HZ_DATA_2):
		seq = msg_t - SPD_HZ_DATA_1;
		stuSpd_Param->rSPD_data[seq].id = 1;	// 地址是1不会变
		stuSpd_Param->rSPD_data[seq].volt_A = (float)stuSpd_Param->dSPD_HZ[seq].volt_A/10;
		stuSpd_Param->rSPD_data[seq].volt_B = (float)stuSpd_Param->dSPD_HZ[seq].volt_B/10;
		stuSpd_Param->rSPD_data[seq].volt_C = (float)stuSpd_Param->dSPD_HZ[seq].volt_C/10;

		stuSpd_Param->rSPD_data[seq].leak_A = (float)stuSpd_Param->dSPD_HZ[seq].leak_A/10;
		stuSpd_Param->rSPD_data[seq].leak_B = (float)stuSpd_Param->dSPD_HZ[seq].leak_B/10;
		stuSpd_Param->rSPD_data[seq].leak_C = (float)stuSpd_Param->dSPD_HZ[seq].leak_C/10;

		stuSpd_Param->rSPD_data[seq].DI_C1_status = (float)stuSpd_Param->dSPD_HZ[seq].breaker_alarm;
		stuSpd_Param->rSPD_data[seq].DI_grd_alarm = (float)stuSpd_Param->dSPD_HZ[seq].grd_alarm;
		stuSpd_Param->rSPD_data[seq].struck_cnt = (float)stuSpd_Param->dSPD_HZ[seq].struck_cnt;
		stuSpd_Param->rSPD_data[seq].struck_total = (float)stuSpd_Param->dSPD_HZ[seq].struck_cnt;
		stuSpd_Param->rSPD_data[seq].spd_temp = (float)stuSpd_Param->dSPD_HZ[seq].spd_temp/10;
		stuSpd_Param->rSPD_data[seq].envi_temp = (float)stuSpd_Param->dSPD_HZ[seq].envi_temp/10;
		stuSpd_Param->rSPD_data[seq].life_time = (float)stuSpd_Param->dSPD_HZ[seq].life_time/10;

		printf("HZspd_real begain\r\n");
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
		printf("day_time = %5hd \r\n",stuSpd_Param->rSPD_data[seq].day_time);
		printf("life_time = %5hd \r\n",stuSpd_Param->rSPD_data[seq].life_time);

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
		printf("hz_spd_data begain\r\n");

		for (i=0;i<HZ_SPD_DATA_NUM;i++)
		{
			HZ_char_to_int(buf + HZ_HEAD_NUM + i*2, (pointer+i));
		}
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
		stuSpd_Param->rSPD_res.grd_res_real = (float)stuSpd_Param->rSPD_res.grd_res_real/100;
		printf("HZ_grd_res_real = %7.3f \r\n",stuSpd_Param->rSPD_res.grd_res_real);
	}
}


int DealNetSPD(int seq,unsigned char *buf,unsigned short int len)
{
	if (SPD_Type == TYPE_LEIXUN)
	{
		if (buf[0] == SPD_Address[0])
		{
			// 根据命令码来进行信息的区分
			switch(buf[1])
			{
				case SPD_READ_CMD:
					DealSPDAiMsg(buf, len);
					RealDataCopy(SPD_AI_DATA);
				break;
				case SPD_DI_CMD:
					DealSPDDiMsg(buf, len);
					RealDataCopy(SPD_DI_DATA);
				break;
				// DO不处理
				case SPD_DO_CMD:
					DealSPDDoMsg(buf, len);
					RealDataCopy(SPD_DO_DATA);
				break;
				default:
				break;
			}
		}
		else if (buf[0] == SPD_Address[SPD_NUM])
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
				DealHZSPDMsg(seq,buf, len);
				RealDataCopy((SPD_DATA_LIST)(SPD_HZ_DATA_1+seq));
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

void *NetWork_DataGet_thread_SPD_L(void *param)
{
	param = NULL;
	int buffPos=0;
	int len,temp = 0;
	unsigned char buf[256];
	//gsocket = 0;
	while(1)
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
		}
		// 断线了
		else
		{
			net_Conneted = 0;
			while(net_Conneted==0)
			{
				// SPD断线了
				printf("spd-break%d\n\r",net_Conneted);
				net_Conneted=obtain_net_psd(0);
			}
		}
      	usleep(5000); //delay 5ms
	}
}

void *NetWork_DataGet_thread_SPD_HZ1(void *param)
{
	param = NULL;
	int Pos=0;
	int len,temp;
	const char *IPaddress;
	const char * IPport;
	int port;
	int addr_len =sizeof(struct sockaddr_in);

    if((sockfd_spd[Pos] = socket(AF_INET,SOCK_DGRAM,0))<0){
        perror("socket_1");
        exit(1);
    }

    int nRecvBufLen = 512;
    setsockopt(sockfd_spd[Pos], SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBufLen, sizeof( int ));
	int nSendBufLen = 512;
    setsockopt(sockfd_spd[Pos], SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );
    struct timeval timeout={8,0};//3s
    setsockopt(sockfd_spd[Pos],SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	IPaddress = gsSPDIP[Pos];	//获取配置文件中的IP地址
	IPport=gsSPDPort[Pos];
	port=atoi(IPport);
    bzero(&HZSPDAddr[Pos],sizeof(HZSPDAddr[Pos]));

    HZSPDAddr[Pos].sin_family = AF_INET;
    HZSPDAddr[Pos].sin_addr.s_addr = inet_addr(IPaddress);
    HZSPDAddr[Pos].sin_port = htons(port);	// Port number
	printf("Pos=%d,hzSPD-IPaddress=%s,hzSPD-IPport=%s\n",Pos,IPaddress,IPport);

	struct sockaddr_in Recvaddr = HZSPDAddr[Pos];
    int DataLen  ;
    char *recvBuf = new char[512];
    while(1)
    {
        memset(recvBuf,0,512);
        DataLen = recvfrom(sockfd_spd[Pos],recvBuf,512,0,(struct sockaddr *)&Recvaddr,(socklen_t*)&addr_len);
        if(DataLen > 0)
        {
			// 打印出来
			pthread_mutex_lock(&HZSPDMutex1);
			strHZSPDdata[0] = recvBuf;
			printf("spd_HZ_len=%d\r\n",DataLen);
			//printf("HZSPDdata:%s\r\n",strHZSPDdata[0].c_str());// 这种风格打印遇到0就停止了
			int j ;for(j=0;j<DataLen;j++) printf("0x%02x ",recvBuf[j]);printf("\r\n");
			DealNetSPD(0,(unsigned char *)recvBuf,DataLen);
			pthread_mutex_unlock(&HZSPDMutex1);
        }

    }
    close(sockfd_spd[Pos]);
}


void *NetWork_DataGet_thread_SPD_HZ2(void *param)
{
	param = NULL;
	int Pos=1;
	int len,temp;
	const char *IPaddress;
	const char * IPport;
	int port;
	int addr_len =sizeof(struct sockaddr_in);

	if((sockfd_spd[Pos] = socket(AF_INET,SOCK_DGRAM,0))<0){
		perror("socket_1");
		exit(1);
	}

	int nRecvBufLen = 512;
	setsockopt(sockfd_spd[Pos], SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBufLen, sizeof( int ));
	int nSendBufLen = 512;
	setsockopt(sockfd_spd[Pos], SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );
	struct timeval timeout={8,0};//3s
	setsockopt(sockfd_spd[Pos],SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	IPaddress = gsSPDIP[Pos];	//获取配置文件中的IP地址
	IPport=gsSPDPort[Pos];
	port=atoi(IPport);
	bzero(&HZSPDAddr[Pos],sizeof(HZSPDAddr[Pos]));

	HZSPDAddr[Pos].sin_family = AF_INET;
	HZSPDAddr[Pos].sin_addr.s_addr = inet_addr(IPaddress);
	HZSPDAddr[Pos].sin_port = htons(port);	// Port number
	printf("Pos=%d,hzSPD-IPaddress=%s,hzSPD-IPport=%s\n",Pos,IPaddress,IPport);

	struct sockaddr_in Recvaddr = HZSPDAddr[Pos];
	int DataLen  ;
	char *recvBuf = new char[512];
	while(1)
	{
		memset(recvBuf,0,512);
		DataLen = recvfrom(sockfd_spd[Pos],recvBuf,512,0,(struct sockaddr *)&Recvaddr,(socklen_t*)&addr_len);
		if(DataLen > 0)
		{
			// 打印出来
			pthread_mutex_lock(&HZSPDMutex1);
			strHZSPDdata[1] = recvBuf;
			printf("spd_HZ_len=%d\r\n",DataLen);
			//printf("HZSPDdata:%s\r\n",strHZSPDdata[1].c_str());
			int j ;for(j=0;j<DataLen;j++) printf("0x%02x ",recvBuf[j]);printf("\r\n");
			DealNetSPD(1,(unsigned char *)recvBuf,DataLen);
			pthread_mutex_unlock(&HZSPDMutex1);
		}
	}
	close(sockfd_spd[Pos]);
}



void *NetWork_DataGet_thread_SPD_HZRes(void *param)
{
	param = NULL;
	int Pos=2;
	int len,temp;
	const char *IPaddress;
	const char * IPport;
	int port;
	int addr_len =sizeof(struct sockaddr_in);

	if((sockfd_spd[Pos] = socket(AF_INET,SOCK_DGRAM,0))<0){
		perror("socket_1");
		exit(1);
	}

	int nRecvBufLen = 512;
	setsockopt(sockfd_spd[Pos], SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBufLen, sizeof( int ));
	int nSendBufLen = 512;
	setsockopt(sockfd_spd[Pos], SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );
	struct timeval timeout={8,0};//3s
	setsockopt(sockfd_spd[Pos],SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	IPaddress = gsSPDIP[Pos];	//获取配置文件中的IP地址
	IPport=gsSPDPort[Pos];
	port=atoi(IPport);
	bzero(&HZSPDAddr[Pos],sizeof(HZSPDAddr[Pos]));

	HZSPDAddr[Pos].sin_family = AF_INET;
	HZSPDAddr[Pos].sin_addr.s_addr = inet_addr(IPaddress);
	HZSPDAddr[Pos].sin_port = htons(port);	// Port number
	printf("Pos=%d,hzSPD-IPaddress=%s,hzSPD-IPport=%s\n",Pos,IPaddress,IPport);

	struct sockaddr_in Recvaddr = HZSPDAddr[Pos];
	int DataLen  ;
	char *recvBuf = new char[512];
	while(1)
	{
		memset(recvBuf,0,512);
		DataLen = recvfrom(sockfd_spd[Pos],recvBuf,512,0,(struct sockaddr *)&Recvaddr,(socklen_t*)&addr_len);
		if(DataLen > 0)
		{
			// 打印出来
			pthread_mutex_lock(&HZSPDMutex1);
			strHZSPDdata[2] = recvBuf;
			printf("spd_HZ_len=%d\r\n",DataLen);
			//printf("HZSPDdata:%s\r\n",strHZSPDdata[2].c_str());
			int j ;for(j=0;j<DataLen;j++) printf("0x%02x ",recvBuf[j]);printf("\r\n");
			DealNetSPD(0,(unsigned char *)recvBuf,DataLen);
			pthread_mutex_unlock(&HZSPDMutex1);
		}
	}
	close(sockfd_spd[Pos]);
}




// 统一创建接收线程
void DataGet_Thread_Create(UINT16 SPD_t)
{
	if (SPD_t == TYPE_LEIXUN)
	{
		pthread_t tNetwork_dataget_SPD_L;
		if (pthread_create(&tNetwork_dataget_SPD_L, NULL, NetWork_DataGet_thread_SPD_L,NULL))
		{
			printf("NetWork SPD data create failed!\n");
			WriteLog("NetWork SPD data create failed!\n");
		}
		pthread_detach(tNetwork_dataget_SPD_L);
	}

	else if (SPD_t == TYPE_HUAZI)
	{
		pthread_mutex_init(&HZSPDMutex1,NULL);
		pthread_mutex_init(&HZSPDMutex2,NULL);
		pthread_mutex_init(&HZSPDMutexRes,NULL);

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
	// 线程开始测试一次电阻，不然要等10分钟
	static UINT16 ctrl_counter = (SPD_TEST_RES_INTERVAL-3);
	FDATA dummy;
	static UINT16 try_connect[SPD_NUM+RES_NUM] = {0,};
	static UINT16 anyone_connect = 0;	// 如果任何一个设备已经连接

	memset (&SPD_ctrl_value,0,sizeof(SPD_CTRL_VALUE));
	dummy.f=NULL_VAR;

	#if 0
	// 测试时定义的数据
	static UINT16 testvalue = 9;
	FDATA test_v;
	static UINT16 entry = 0;
	#endif

	temp=0;
	if (SPD_Type == TYPE_LEIXUN)
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
				sleep(2);
			}
		}
	}
	#if 0
	else if (SPD_Type == TYPE_HUAZI)
	{
		for (i = 0;i<(SPD_NUM+RES_NUM);i++)
		{
			HZ_Conneted[i] = 0;	// 开始连接前置0
		}

		// 如果每台设备都连了3次都没连上就继续尝试连接，直到任意一台连上
		while (anyone_connect == 0)
		{
			for (i = 0;i<(SPD_NUM+RES_NUM);i++)
			{
				while(HZ_Conneted[i]==0)
				{
					HZ_Conneted[i]=obtain_net_HZ_psd(i);
					sprintf(str,"HZ_Conneted%d=%d\n",i,HZ_Conneted[i]);
					WriteLog(str);
					if(HZ_Conneted[i]==0)
					{
						try_connect[i]++;
						printf("connect _psd error\n");
						WriteLog("IN NETWORK_Server_thread connect _psd error!\n");
						if (try_connect[i] >= 3)	// 开头尝试连接3次都不成功,就不连了,继续下一个连接
						{
							break;
						}
						sleep(2);
					}
					else
					{
						anyone_connect = 1;
					}
				}
			}
		}
	}
	#endif
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
			spd_ctl_flag = 0;
		}
		if (spd_ctl_flag&BITS_MSK_GET(0,SPD_CTRL_NUM))
		{
			spd_net_flag = 0;
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


			// 350*3 = 1.05s，即1.05s轮询一个项目
			if (poll_cnt >= SPD_POLLING_INTERVAL)
			{
				poll_cnt = 0;
				op_counter++;						// 轮询间隔标志
				if (SPD_num == TYPE_LEIXUN)
				{
					if (op_counter == SPD_HZ_DATA_1)
					{
						op_counter = SPD_RES_DATA;	// 跳过第二个防雷器
					}
					if(ctrl_counter >= SPD_TEST_RES_INTERVAL)
					{
						//spd_ctl_flag |= SPD_RES_SET;
						ctrl_counter = 0;
						// 接地电阻10分钟测试一次
						Ex_SPD_Set_Process(SPD_RES_SET,RES_TEST_ADDR,dummy,RES_TEST_EN);
					}
					if (op_counter >= SPD_DATA_NUM)
					{
						op_counter = 0;
					}
					ctrl_counter++;		// 每隔一段时间进行一次接地电阻测试
				}
				else if (SPD_num == TYPE_HUAZI)
				{
					// 这里的逻辑要再想想
					if (op_counter <= SPD_HZ_DATA_1)
					{
						op_counter = SPD_HZ_DATA_1;	// 跳过雷迅的轮询
					}

					if (op_counter >= SPD_DATA_NUM)
					{
						op_counter = SPD_HZ_DATA_1;
					}
				}
				spd_net_flag |= BIT(op_counter);
			}
		}
		// 统一处理的设置函数，这种结构注定上位机只能一次设置一个，且要有个间隔时间,350ms
		if (spd_ctl_flag&BIT(SPD_AI_SET))
		{
			spd_ctrl_process(&spd_ctl_flag,SPD_AI_SET);
		}
		else if (spd_ctl_flag&BIT(SPD_DO_SET))
		{
			spd_ctrl_process(&spd_ctl_flag,SPD_DO_SET);
		}
		else if (spd_ctl_flag&BIT(SPD_RES_SET))
		{
			spd_ctrl_process(&spd_ctl_flag,SPD_RES_SET);
		}

		// 设置完毕，开始轮询
		else if (spd_net_flag&BIT(SPD_AI_DATA))
		{
			spd_send_process(&spd_net_flag,SPD_AI_DATA);
		}
		else if (spd_net_flag&BIT(SPD_DI_DATA))
		{
			spd_send_process(&spd_net_flag,SPD_DI_DATA);
		}
		else if (spd_net_flag&BIT(SPD_DO_DATA))
		{
			spd_send_process(&spd_net_flag,SPD_DO_DATA);
		}
		else if (spd_net_flag&BIT(SPD_HZ_DATA_1))
		{
			spd_send_process(&spd_net_flag,SPD_HZ_DATA_1);
		}
		else if (spd_net_flag&BIT(SPD_HZ_DATA_2))
		{
			spd_send_process(&spd_net_flag,SPD_HZ_DATA_2);
		}
		else if (spd_net_flag&BIT(SPD_RES_DATA))
		{
			spd_send_process(&spd_net_flag,SPD_RES_DATA);
		}
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

