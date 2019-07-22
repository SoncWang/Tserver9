
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
#include "rs485server.h"
#include "MyCritical.h"
#include <string>
#include <semaphore.h>
#include "tea.h"
#include "rtc.h"
#include "server.h"

/*Global definition*/
using namespace std;
CComPort *mComPort4 = NULL;
CMyCritical Com4Cri;
CMyCritical Com4SendCri;

UINT32  comm_flag=0;	// 轮询标志
UINT32  ctrl_flag=0;	// 写标志
UINT8  WAIT_response_flag=0;
UINT8 Recive_Flag = 0;			/* 接收标志*/


extern LOCKER_HW_PARAMS *lockerHw_Param[LOCK_NUM];
extern RSU_PARAMS *stuRSU_Param[VA_METER_BD_NUM];

const UINT32 locker_id[CARD_NUM] =
{
	2035400,
	13838538,
	1547012,
	10863352,
	2857740885u
};

/*5次锁的轮询后轮询其它事项*/
const UINT16 polling_cnt[] =
{
#if (LOCK_NUM >= 1)
	LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (LOCK_NUM >= 1)
	,LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (LOCK_NUM >= 1)
	,LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (LOCK_NUM >= 1)
	,LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (LOCK_NUM >= 1)
	,LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (VA_METER_BD_NUM >=1)
	,VOLT_AMP_GET_FLAG_1
#endif
#if (LOCK_NUM >= 1)
	,LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (LOCK_NUM >= 1)
	,LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (LOCK_NUM >= 1)
	,LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (LOCK_NUM >= 1)
	,LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (LOCK_NUM >= 1)
	,LOCKER_1_STATUS
#endif
#if (LOCK_NUM >= 2)
	,LOCKER_2_STATUS
#endif
#if (VA_METER_BD_NUM >=2)
	,VOLT_AMP_GET_FLAG_2
#elif (VA_METER_BD_NUM >=1)
	,VOLT_AMP_GET_FLAG_1
#endif
};


int SendCom4ReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
    Com4SendCri.Lock();
    UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};

    int len=8;
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

	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort4->SendBuf(bytSend,len);

    Com4SendCri.UnLock();
	usleep(5000);	//delay 5ms
	return 0 ;
}



void *Locker_DataPollingthread(void *param)
{
	param = NULL;
	int i;
	static UINT16 polling_counter = 0;

	while(1)
	{
		if (ctrl_flag > 0)
		{
			comm_flag = 0;
		}
		else
		{
			comm_flag |= LBIT(polling_cnt[polling_counter]);
			polling_counter++;
			if (polling_counter >= (sizeof(polling_cnt)/sizeof(UINT16)))
			{
				printf("\r\npoling over");
				printf("0x%02x" ,polling_counter);printf("\r\n");
				polling_counter = 0;
			}
		}

		// 如果没有收到回复数据,这里会一直进不来,要用带timeout的信号量
		//if (!WAIT_response_flag)
		{
			// 控制,优先处理DO
			if (ctrl_flag & (LBIT(POWER_1_CTRL_CLOSE) |LBIT(POWER_1_CTRL_OPEN) |LBIT(POWER_2_CTRL_CLOSE) |LBIT(POWER_2_CTRL_OPEN)
						|LBIT(POWER_3_CTRL_CLOSE) |LBIT(POWER_3_CTRL_OPEN) |LBIT(POWER_4_CTRL_CLOSE) |LBIT(POWER_4_CTRL_OPEN)
						|LBIT(POWER_5_CTRL_CLOSE) |LBIT(POWER_5_CTRL_OPEN) |LBIT(POWER_6_CTRL_CLOSE) |LBIT(POWER_6_CTRL_OPEN)
						|LBIT(POWER_7_CTRL_CLOSE) |LBIT(POWER_7_CTRL_OPEN) |LBIT(POWER_8_CTRL_CLOSE) |LBIT(POWER_8_CTRL_OPEN)
						|LBIT(POWER_9_CTRL_CLOSE) |LBIT(POWER_9_CTRL_OPEN) |LBIT(POWER_10_CTRL_CLOSE) |LBIT(POWER_10_CTRL_OPEN)
						|LBIT(POWER_11_CTRL_CLOSE) |LBIT(POWER_11_CTRL_OPEN) |LBIT(POWER_12_CTRL_CLOSE) |LBIT(POWER_12_CTRL_OPEN))
				)
			{
				for(i=0; i<12; i++)
				{
					if (ctrl_flag & (LBIT(POWER_1_CTRL_CLOSE+2*i)))
					{
						ctrl_flag &= ~(LBIT(POWER_1_CTRL_CLOSE+2*i));
						WAIT_response_flag = WAIT_POWER_1_CTRL_CLOSE_RES+2*i;
						SendCom4RCtlReg(POWER_CTRL_ADDR,FORCE_COIL,VPLATE1_REG+i,SWITCH_OFF);
						break;	// 跳出循环,等待下一个450ms后大循环再来控制，怕485冲突
					}
					else if (ctrl_flag & (LBIT(POWER_1_CTRL_OPEN+2*i)))
					{
						ctrl_flag &= ~(LBIT(POWER_1_CTRL_OPEN+2*i));
						WAIT_response_flag = WAIT_POWER_1_CTRL_OPEN_RES+2*i;
						SendCom4RCtlReg(POWER_CTRL_ADDR,FORCE_COIL,VPLATE1_REG+i,SWITCH_ON);
						break;	// 跳出循环,等待下一个450ms后大循环再来控制，怕485冲突
					}
				}
			}
			else if (ctrl_flag & LBIT(LOCKER_1_CTRL_UNLOCK))
			{
				ctrl_flag &= ~(LBIT(LOCKER_1_CTRL_UNLOCK));
				WAIT_response_flag = WAIT_LOCKER_1_UNLOCK_RES;
				SendCom4RCtlReg(lockerHw_Param[0]->address,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
			}
			else if (ctrl_flag & LBIT(LOCKER_1_CTRL_LOCK))
			{
				ctrl_flag &= ~(LBIT(LOCKER_1_CTRL_LOCK));
				WAIT_response_flag = WAIT_LOCKER_1_LOCK_RES;
		 		SendCom4RCtlReg(lockerHw_Param[0]->address,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
			}
			#if (LOCK_NUM >= 2)
			else if (ctrl_flag & LBIT(LOCKER_2_CTRL_UNLOCK))
			{
				ctrl_flag &= ~(LBIT(LOCKER_2_CTRL_UNLOCK));
				WAIT_response_flag = WAIT_LOCKER_2_UNLOCK_RES;
		 		SendCom4RCtlReg(lockerHw_Param[1]->address,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
			}
			else if (ctrl_flag & LBIT(LOCKER_2_CTRL_LOCK))
			{
				ctrl_flag &= ~(LBIT(LOCKER_2_CTRL_LOCK));
				WAIT_response_flag = WAIT_LOCKER_2_LOCK_RES;
		 		SendCom4RCtlReg(lockerHw_Param[1]->address,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
			}
			#endif

			// 轮询
			else if (comm_flag &LBIT(LOCKER_1_STATUS))
			{
				comm_flag &= ~(LBIT(LOCKER_1_STATUS));
				SendCom4ReadReg(lockerHw_Param[0]->address,READ_REGS,DOOR_STATUS_REG,LOCKER_REG_NUM);
				WAIT_response_flag = WAIT_LOCKER_1_MSG;
			}
			#if (LOCK_NUM >= 2)
			else if (comm_flag &LBIT(LOCKER_2_STATUS))
			{
				comm_flag &= ~(LBIT(LOCKER_2_STATUS));
				SendCom4ReadReg(lockerHw_Param[1]->address,READ_REGS,DOOR_STATUS_REG,LOCKER_REG_NUM);
				WAIT_response_flag = WAIT_LOCKER_2_MSG;
			}
			#endif
			else if (comm_flag &LBIT(VOLT_AMP_GET_FLAG_1))
			{
				comm_flag &= ~(LBIT(VOLT_AMP_GET_FLAG_1));
				SendCom4ReadReg(stuRSU_Param[0]->address, READ_REGS, VA_REG, VA_DATA_NUM);
				WAIT_response_flag = WAIT_VA_DATA_1_MSG;
			}
			#if (VA_METER_BD_NUM >=2)
			else if (comm_flag &LBIT(VOLT_AMP_GET_FLAG_2))
			{
				comm_flag &= ~(LBIT(VOLT_AMP_GET_FLAG_2));
				SendCom4ReadReg(stuRSU_Param[1]->address, READ_REGS, VA_REG, VA_DATA_NUM);
				WAIT_response_flag = WAIT_VA_DATA_2_MSG;
			}
			#endif
		}
		usleep(INTERVAL_TIME);		// every 700ms sending
	}
	return 0 ;
}


/*A fixed timer to polling the LOCKER */
void lockerPollingInit()
{
	pthread_t m_locker_polling_thread ;
	pthread_create(&m_locker_polling_thread,NULL,Locker_DataPollingthread,NULL);
}


/******************************************************************************
*  函数名: void char_to_long(INT8U* buffer,LONG32U* value)
*
*  描述: 字符转化为整型
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
void char_to_int(UINT8* buffer,UINT16* value)
{
	INTEGER_UNION int_value;

	int_value.b[1] = *(buffer);
	int_value.b[0] = *(buffer + 1);
	*value = int_value.i;
}


/******************************************************************************
*  函数名: void char_to_long(INT8U* buffer,LONG32U* value)
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
void char_to_long(UINT8* buffer,UINT32* value)
{
	LONG_UNION long_value;
	UINT8 i;

	for(i=0;i<4;i++)
	{
		long_value.b[3 - i] = *(buffer + i);
	}
	*value = long_value.i;
}


/*Unpack the data from RS485*/
#define FIXED_NUM		4	// The former num of the locker's status
static UINT8 locker_opened[LOCK_NUM] = {0,};
static UINT16 last_cnt[LOCK_NUM] = {0,};
static UINT32 last_card = 0;

int DealLockerMsg(unsigned char *buf,unsigned short int len)
{
	UINT8 i=0;
	UINT8 addr = 1;
	UINT8 addr_base = 0;
	UINT16* pointer = &(lockerHw_Param[0]->status);
	UINT8* pid = &(lockerHw_Param[0]->id[0]);
	UINT32 card_read = 0;

	if((len == (LOCKER_REG_NUM*2+5)))
	{
		addr = *(buf+0);		// the first byte is the addr.
		if(addr == lockerHw_Param[0]->address)
		{
			addr_base = 0;
		}
		#if (LOCK_NUM >= 2)
		else
		{
			addr_base = 1;
		}
		#endif

		pointer = &(lockerHw_Param[addr_base]->status);
		pid = &(lockerHw_Param[addr_base]->id[0]);

		for (i=0;i<FIXED_NUM;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
		if(last_cnt[addr_base] >= lockerHw_Param[addr_base]->report_cnt)
		{
			locker_opened[addr_base] = 0;
		}

		for (i=0;i< (LOCKER_REG_NUM-FIXED_NUM)*2;i++)
		{
			*(pid+i) = *(buf + FRAME_HEAD_NUM + FIXED_NUM*2 + i);
			//printf("0x%02x ",*(pid+i));
			#if 0
			if ((i>0)&&(i<5))
			{
				card_read <<=8;
				card_read |= (UINT8)(*(pid+i));
			}
			#endif
		}

	}

	char_to_long(&(lockerHw_Param[addr_base]->id[1]),&card_read);

	printf("lock begain\r\n");
	//printf("%5hd ",lockerHw_Param[addr_base]->status);printf("\r\n");
	//printf("%5hd ",lockerHw_Param[addr_base]->open_reason);printf("\r\n");
	//printf("%5hd ",lockerHw_Param[addr_base]->report_cnt);printf("\r\n");
	//printf("%5hd ",lockerHw_Param[addr_base]->id_length);printf("\r\n");
	int j ;//for(j=0;j<(LOCKER_REG_NUM-FIXED_NUM)*2;j++) printf("0x%02x ",lockerHw_Param->id[j]);
	printf("0x%08x" ,card_read);printf("\r\n");


	for (j=0;j<CARD_NUM;j++)
	{
		if (locker_opened[addr_base] == 0)
		{
			if (card_read == locker_id[j])
			{
				ctrl_flag |= LBIT(LOCKER_1_CTRL_UNLOCK+addr_base*2);
				SendCom4RCtlReg(addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
				locker_opened[addr_base] =1;
				last_card = card_read;
				break;
			}
		}
	}
	last_cnt[addr_base] = lockerHw_Param[addr_base]->report_cnt;
	return 0 ;
}

/******************************************************************************
 * 函数名:	comm_VAData_analyse
 * 描述: 		电压电流采集传感器的解析
 *            -
 * 输入参数:
 * 输出参数:
 * 返回值:
 *
 * 作者:Jerry
 * 创建日期:2019.7.12
 ******************************************************************************/
void comm_VAData_analyse(unsigned char *buf,unsigned short int len,unsigned char seq)
{
	UINT8 i;
	UINT16 *pointer = &stuRSU_Param[seq]->phase[0].vln;	/*第0相*/

	if(len == (REAL_DATA_NUM*2+5))
	{
		printf("va begain\r\n");
		for (i=0;i<2;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
		printf("%5hd ",stuRSU_Param[seq]->phase[0].vln);printf("\r\n");
		printf("%5hd ",stuRSU_Param[seq]->phase[0].amp);printf("\r\n");

		/*第1相*/
		pointer = &stuRSU_Param[seq]->phase[1].vln;
		for(i = 7;i <= 8;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-7));
		}
		printf("%5hd ",stuRSU_Param[seq]->phase[1].vln);printf("\r\n");
		printf("%5hd ",stuRSU_Param[seq]->phase[1].amp);printf("\r\n");

		/*第2相*/
		pointer = &stuRSU_Param[seq]->phase[2].vln;
		for(i = 14;i <= 15;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-14));
		}
		printf("%5hd ",stuRSU_Param[seq]->phase[2].vln);printf("\r\n");
		printf("%5hd ",stuRSU_Param[seq]->phase[2].amp);printf("\r\n");

		/*第3相*/
		pointer = &stuRSU_Param[seq]->phase[3].vln;
		for(i = 21;i <= 22;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-21));
		}
		printf("%5hd ",stuRSU_Param[seq]->phase[3].vln);printf("\r\n");
		printf("%5hd ",stuRSU_Param[seq]->phase[3].amp);printf("\r\n");

		/*第4相*/
		pointer = &stuRSU_Param[seq]->phase[4].vln;
		for(i = 28;i <= 29;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-27));
		}
		printf("%5hd ",stuRSU_Param[seq]->phase[4].vln);printf("\r\n");
		printf("%5hd ",stuRSU_Param[seq]->phase[4].amp);printf("\r\n");

		/*第5相*/
		pointer = &stuRSU_Param[seq]->phase[5].vln;
		for(i = 35;i <= 36;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-35));
		}
		printf("%5hd ",stuRSU_Param[seq]->phase[5].vln);printf("\r\n");
		printf("%5hd ",stuRSU_Param[seq]->phase[5].amp);printf("\r\n");
	}
}



int DealComm485(unsigned char *buf,unsigned short int len)
{
	switch(Recive_Flag)
	{
		case WAIT_LOCKER_1_MSG:					/*MSG from the locker*/
		case WAIT_LOCKER_2_MSG:
			DealLockerMsg(buf, len);
		break;

		case WAIT_VA_DATA_1_MSG:				/*MSG from the Volt-amp detector*/
			comm_VAData_analyse(buf, len,0);
		break;

	#if (VA_METER_BD_NUM >= 2)
		case WAIT_VA_DATA_2_MSG:
			comm_VAData_analyse(buf, len,1);
		break;
	#endif

		default:
		break;
	}

	Recive_Flag = 0;
	WAIT_response_flag = 0;
	return 0;
}




void *ComPort4Thread(void *param)
{
   param = NULL;
   int buffPos=0;
   int len ;
   unsigned char buf[256] ;
   while(1)
   {
      	len = read(mComPort4->fd, buf+buffPos, 256);
	  	buffPos = buffPos+len;
	  	if(buffPos<5) continue;

	  	//CRC
	  	unsigned short int CRC = CRC16(buf,buffPos-2) ;
	  	if((((CRC&0xFF00) >> 8)!= buf[buffPos-2]) || ((CRC&0x00FF) != buf[buffPos-1]))
		{
		  printf("CRC error\r\n");
 		  if(buffPos>=256) buffPos=0;

		  continue ;
	  	}

      	printf("com1 len=%d\r\n",buffPos) ;
		Recive_Flag = WAIT_response_flag;	// already received the data.

	  	/*debug the information*/
	  	int j ;for(j=0;j<buffPos;j++)printf("0x%02x ",buf[j]);printf("\r\n");

	  	DealComm485(buf , buffPos);
	  	buffPos=0;

      	usleep(5000); //delay 5ms
   }

   return NULL ;
}

void rs485init(void)
{
   mComPort4 = new CComPort();

   /*there is only ttysp1 for RS485 now in A287*/
   mComPort4->fd = mComPort4->openSerial((char *)"/dev/ttymxc4",9600);
   printf("rs485 status");
   printf("0x%02x \r\n",mComPort4->fd);

   pthread_t m_ComPort4Thread ;
   pthread_create(&m_ComPort4Thread,NULL,ComPort4Thread,NULL);
}


//鍙戦�佸啓閬ユ帶瀵勫瓨鍣?顔怉DDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + MBUS_OPT_CODE_H + MBUS_OPT_CODE_L + CRC(2)
UINT16 SendCom4RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code)
{
    Com4SendCri.Lock();
    UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};

    int len=8;

    bytSend[MBUS_ADDR]        = Addr;
    bytSend[MBUS_FUNC]        = Func;       //鍐欏瘎瀛樺櫒
    bytSend[MBUS_REFS_ADDR_H] = (REFS_ADDR&0xFF00) >> 8;     //瀵勫瓨鍣ㄨ捣濮嬪湴鍧�楂樹綅
    bytSend[MBUS_REFS_ADDR_L] =  REFS_ADDR&0x00FF;           //瀵勫瓨鍣ㄨ捣濮嬪湴鍧�浣庝綅
    bytSend[MBUS_OPT_CODE_H] = (code&0xFF00) >> 8;     //鎿嶄綔鐮侀珮浣?    bytSend[MBUS_OPT_CODE_L] =  code&0x00FF;           //鎿嶄綔鐮佷綆浣?
	bytSend[MBUS_OPT_CODE_L] =  code&0xFF;
	//CRC鏍￠獙
    unsigned short int CRC = CRC16(bytSend,len-2) ;
    bytSend[len-2] = (CRC&0xFF00) >> 8;     //CRC楂樹綅
    bytSend[len-1] =  CRC&0x00FF;           //CRC浣庝綅

	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort4->SendBuf(bytSend,len);

	//memcpy(LastSendBuf,bytSend,len);
	//LastSendLen=len;
	//free(bytSend);

    Com4SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0;
}



