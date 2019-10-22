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
static int sockfd_spd;

UINT16  spd_net_flag = 0;	// SPD轮询标志
UINT16  spd_ctl_flag = 0;	// SPD控制标志

UINT8  WAIT_net_res_flag=0;	// 等待信息回应


UINT8 SPD_Address = SPD_ADDR;
UINT8 SPD_Res_Address = SPD_RES_ADDR;
SPD_CTRL_VALUE SPD_ctrl_value;

pthread_mutex_t SPDdataHandleMutex;


//extern RSUCONTROLER stuRsuControl;	//RSU控制器状态
//extern RSU_DATA_INIT stuRsuData;	//RSU设备信息结构体
//extern RSU_RESET stuRsuReset;			//天线软件复位状态结构体
extern char gsSPDIP[20];	//SPD-IP地址
extern char gsSPDPort[10];	//SPD-端口
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体


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
	{SPD_RES_READ_CMD,SPD_RES_STATUS_ADDR,SPD_RES_STATUS_NUM},
};


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
int SPD_Read_Reg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
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

	printf("psd Rdata:");
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	write(sockfd_spd,bytSend,len);
	pthread_mutex_unlock(&SPDdataHandleMutex);

	usleep(5000);	//delay 5ms
	return 0 ;
}


/* psd发送数据--读取模拟,DI，DO数据 */
int SPD_DO_Ctrl_Reg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 cmd)
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

	printf("psd Ddata:");
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");
	write(sockfd_spd,bytSend,len);

	pthread_mutex_unlock(&SPDdataHandleMutex);
	usleep(5000);	//delay 5ms
	return 0 ;
}

#define AI_SET_MIN	2		// AI设置每次至少2个数量
/* psd发送数据--读取模拟,DI，DO数据 ,REFS_ADDR要特别注意一一对应*/
int SPD_AI_Set_Reg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT, FDATA *data)
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

	if (temp2 > 247)	// 字节总共超出了256
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

	printf("spd cdata:");
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");
	write(sockfd_spd,bytSend,len);
	pthread_mutex_unlock(&SPDdataHandleMutex);

	usleep(5000);	//delay 5ms
	return 0 ;
}

/* psd发送数据--设置接地电阻部分数据 */
int SPD_Res_Set_Reg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 cmd)
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
	write(sockfd_spd,bytSend,len);

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

	if (SPD_data_event >= SPD_DATA_NUM)
	{
		return 0;
	}

	if (SPD_data_event < SPD_RES_DATA)
	{
		addr_temp = SPD_Address;
	}
	else
	{
		addr_temp = SPD_Res_Address;	// 接地电阻仪地址不一样
	}

	if (*pnet_flag & (BIT(SPD_data_event)))
	{
		*pnet_flag &= ~(BIT(SPD_data_event));
		//读取防雷器和接地电阻的数据
		SPD_Read_Reg(addr_temp, g_SPD_Function_Array[SPD_data_event].func_code,\
				g_SPD_Function_Array[SPD_data_event].reg_addr,g_SPD_Function_Array[SPD_data_event].reg_num);
	}
	return 1;
}


/*模块化spd数据设置*/
int spd_ctrl_process(UINT16 *pctrl_flag, SPD_CTRL_LIST SPD_ctrl_event)
{
	UINT8 addr_temp;

	if (SPD_ctrl_event >= SPD_CTRL_NUM)
	{
		return 0;
	}

	if (SPD_ctrl_event < SPD_RES_SET)
	{
		addr_temp = SPD_Address;
	}
	else
	{
		addr_temp = SPD_Res_Address;	// 接地电阻仪地址不一样
	}

	if (*pctrl_flag & (BIT(SPD_ctrl_event)))
	{
		*pctrl_flag &= ~(BIT(SPD_ctrl_event));
		if (SPD_ctrl_event == SPD_AI_SET)
		{
			SPD_AI_Set_Reg(addr_temp,SPD_WRITE_CMD,SPD_ctrl_value.ref_addr,AI_SET_MIN,&SPD_ctrl_value.f_ai_set);//设置防雷器的AI数据
			// 改了id后要更新地址
			if (SPD_ctrl_value.ref_addr == SPD_ID_ADDR)
			{
				SPD_Address = (UINT8)SPD_ctrl_value.f_ai_set.f;
			}
		}
		else if (SPD_ctrl_event == SPD_DO_SET)
		{
			// DO每次只写1个寄存器
			SPD_DO_Ctrl_Reg(addr_temp,SPD_DO_CTRL_CMD,SPD_ctrl_value.ref_addr,SPD_ctrl_value.do_set);//设置防雷器的DO数据
		}
		else if (SPD_ctrl_event == SPD_RES_SET)
		{
			// RES接地电阻部分设置
			SPD_Res_Set_Reg(addr_temp,SPD_RES_SET_CMD,SPD_ctrl_value.ref_addr,SPD_ctrl_value.res_set);
			// 改了id后要更新地址
			if (SPD_ctrl_value.ref_addr == RES_ID_ADDR)
			{
				SPD_Res_Address = (UINT8)SPD_ctrl_value.res_set;
			}
		}
	}
	return 1;
}

int obtain_net_psd(void)
{
	char str[10*1024];
	const char *IPaddress;
	const char * IPport;
	int port;
	struct sockaddr_in net_psd_addr;
	if( (sockfd_spd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
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
	IPaddress = gsSPDIP;//获取配置文件中的IP地址
	IPport=gsSPDPort;
	port=atoi(IPport);
	/* Fill the local socket address struct */
	net_psd_addr.sin_family = AF_INET;           		// Protocol Family
	net_psd_addr.sin_port = htons (port);         		// Port number
	net_psd_addr.sin_addr.s_addr  = inet_addr (IPaddress);  	// AutoFill local address
	memset (net_psd_addr.sin_zero,0,8);          		// Flush the rest of struct
	printf("SPD-IPaddress=%s,SPD-IPport=%s\n",IPaddress,IPport);
	sprintf(str,"SPD-IPaddress=%s,SPD-IPport=%s\n",IPaddress,IPport);
	WriteLog(str);

	if (connect(sockfd_spd, (struct sockaddr*)&net_psd_addr, sizeof(net_psd_addr)) == -1)
   	 {
		printf("connect to SPD server refused!\n");
		WriteLog("connect to SPD server refused!\n");
		close(sockfd_spd);
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
		printf("spd_AI begain\r\n");
		// 前面11个字节是实数,4个字节
		for(i = 0;i < INT_REG_POS;i++)
		{
			char_to_float(buf + FRAME_HEAD_NUM + i*4, (fpointer+i));
		}
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


		pointer = &stuSpd_Param->dSPD_AIdata.systime_year;
		// 后面的字节是整数,2个字节,另外最后一个数据没有变量对应,舍弃掉
		for(i = INT_REG_POS;i < (SPD_AI_NUM-INT_REG_POS-1);i++)		// 从11项开始,前面的实数占了2个位置，所以减掉
		{
			char_to_int(buf+(FRAME_HEAD_NUM + INT_REG_POS*4)+(i-INT_REG_POS)*2, (pointer+i-INT_REG_POS));
		}
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

		stuSpd_Param->dSPD_DI.C1_status = (stuSpd_Param->dSPD_DI.SPD_DI & BIT(3))?1:0;
		stuSpd_Param->dSPD_DI.grd_alarm = (stuSpd_Param->dSPD_DI.SPD_DI & BIT(4))?1:0;
		stuSpd_Param->dSPD_DI.leak_alarm  =  (stuSpd_Param->dSPD_DI.SPD_DI & BIT(6))?1:0;
		stuSpd_Param->dSPD_DI.volt_alarm  =  (stuSpd_Param->dSPD_DI.SPD_DI & BIT(7))?1:0;

		printf("SPD_DI = 0x%02x \r\n",stuSpd_Param->dSPD_DI.SPD_DI);
		printf("C1_status = 0x%02x \r\n",stuSpd_Param->dSPD_DI.C1_status);
		printf("grd_alarm = 0x%02x \r\n",stuSpd_Param->dSPD_DI.grd_alarm);
		printf("leak_alarm = 0x%02x \r\n",stuSpd_Param->dSPD_DI.leak_alarm);
		printf("volt_alarm = 0x%02x \r\n",stuSpd_Param->dSPD_DI.volt_alarm);
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
	UINT16 *pointer = &stuSpd_Param->dSPD_res.alarm;
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

		dot_num = stuSpd_Param->dSPD_res.grd_res_dot_num;
		res_temp = (float)stuSpd_Param->dSPD_res.grd_res_value;
		if (dot_num > 0)
		{
			for (i=0;i<dot_num;i++)
			{
				// 实数除法
				res_temp = res_temp/10;
			}
			stuSpd_Param->dSPD_res.grd_res_real = res_temp;
		}
		else
		{
			stuSpd_Param->dSPD_res.grd_res_real = res_temp;
		}
		printf("res_alarm = 0x%02x \r\n",stuSpd_Param->dSPD_res.alarm);
		printf("grd_res_value = 0x%02x \r\n",stuSpd_Param->dSPD_res.grd_res_value);
		printf("grd_res_dot_num = 0x%02x \r\n",stuSpd_Param->dSPD_res.grd_res_dot_num);
		printf("grd_volt = 0x%02x \r\n",stuSpd_Param->dSPD_res.grd_volt);

		printf("test = 0x%02x \r\n",stuSpd_Param->dSPD_res.test);
		printf("id = 0x%02x \r\n",stuSpd_Param->dSPD_res.id);
		printf("alarm_value = 0x%02x \r\n",stuSpd_Param->dSPD_res.alarm_value);

		printf("grd_res_real = %7.3f \r\n",stuSpd_Param->dSPD_res.grd_res_real);
	}
}

int DealNetSPD(unsigned char *buf,unsigned short int len)
{
	if (buf[0] == SPD_Address)
	{
		// 根据命令码来进行信息的区分
		switch(buf[1])
		{
			case SPD_READ_CMD:
				DealSPDAiMsg(buf, len);
			break;
			case SPD_DI_CMD:
				DealSPDDiMsg(buf, len);
			break;
			// DO不处理
			case SPD_DO_CMD:
				DealSPDDoMsg(buf, len);
			break;
			default:
			break;
		}
	}
	else if (buf[0] == SPD_Res_Address)
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
	return 0;
}

void *NetWork_DataGet_thread_SPD(void *param)
{
	param = NULL;
	int buffPos=0;
	int len,temp;
	unsigned char buf[256] ;
	while(1)
	{
      	len = read(sockfd_spd, buf, sizeof(buf)-1);
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
		  	DealNetSPD(buf, buffPos);
		  	buffPos=0;
		}
		// 断线了
		else
		{
			while(temp==0)
			{
				temp=obtain_net();
			}
		}
      	usleep(5000); //delay 5ms
	}
}


void* NetWork_server_thread_SPD(void*arg)//接收PSD数据线程
{
	char str[10*1024];
	int i,j,temp;
	int port,nlen;
	struct sockaddr_in net_spd_addr;
	unsigned short crc_spd;
	//int bFlag=0;
	static UINT16 op_counter = 0;
	static UINT16 poll_cnt = 0;
	FDATA dummy;

	memset (&SPD_ctrl_value,0,sizeof(SPD_CTRL_VALUE));
	dummy.f=NULL_VAR;

	#if 0
	// 测试时定义的数据
	static UINT16 testvalue = 9;
	FDATA test_v;
	static UINT16 entry = 0;
	#endif

	temp=0;
	while(temp==0)
	{
		temp=obtain_net_psd();
		sprintf(str,"temp=%d\n",temp);
		WriteLog(str);
		if(temp==0)
		{
			printf("connect _psd error\n");
			WriteLog("IN NETWORK_Server_thread connect _psd error!\n");
			sleep(2);
		}
	}
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


			// 350*4 = 1.4s，即1.4s轮询一个项目
			if (poll_cnt >= SPD_POLLING_INTERVAL)
			{
				poll_cnt = 0;
				spd_net_flag |= BIT(op_counter);
				op_counter++;
				if (op_counter >= SPD_DATA_NUM)
				{
					op_counter = 0;
				}
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
			printf("SPD_RES_SET_end\n\r");
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

	// 数据接收线程
	pthread_t tNetwork_dataget_SPD;
	if (pthread_create(&tNetwork_dataget_SPD, NULL, NetWork_DataGet_thread_SPD,NULL))
	{
		printf("NetWork SPD data create failed!\n");
		WriteLog("NetWork SPD data create failed!\n");
	}
	pthread_detach(tNetwork_dataget_SPD);

	pthread_mutex_init(&SPDdataHandleMutex,NULL);
}

