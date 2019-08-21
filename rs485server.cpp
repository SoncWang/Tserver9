
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
#include "config.h"

/*Global definition*/
using namespace std;
CComPort *mComPort3 = NULL ;   	//RS485对应串口4,轮询计量设备
CComPort *mComPort4 = NULL;		//RS485对应串口5,轮询电子锁,最多4把锁
CMyCritical Com3Cri;
CMyCritical Com4Cri;
CMyCritical Com3SendCri;
CMyCritical Com4SendCri;

UINT32  comm_flag=0;	// 轮询标志
//UINT32  ctrl_flag;
UINT32  locker_ctrl_flag=0;	// 电子锁写标志
UINT32	power_ctrl_flag[POWER_BD_MAX_NUM] = {0,0,0};
UINT8  WAIT_response_flag=0;
UINT8 Recive_Flag = 0;			/* 接收标志*/
UINT8 actual_locker_num = 0;
UINT8 actual_485dev_num = 0;

/*Unpack the data from RS485*/
#define FIXED_NUM		4	// The former num of the locker's status
static UINT16 *locker_opened;
static UINT16 *last_cnt;
static UINT32 last_card = 0;


//UINT8 actual_locker_num = 0;

extern LOCKER_HW_PARAMS *lockerHw_Param[LOCK_MAX_NUM];	//门锁状态结构体
extern VA_METER_PARAMS *stuVA_Meter_Param[VA_METER_BD_NUM];		//伏安表电压电流结构体

string StrAdrrLock[LOCK_MAX_NUM];	//门锁1的地址

string StrAdrrVAMeter[VA_METER_BD_MAX_NUM];	//电压电流传感器1的地址
string StrAdrrPower[POWER_BD_MAX_NUM];	//电源板1的地址
string StrDoSeq[SWITCH_COUNT];	//do和设备映射的配置
UINT16 DoSeq[SWITCH_COUNT]={0,};	// 另外定义一个专门用来存储映射的数组,stuRemote_Ctrl会被清0


int *polling_arr;		// 注意存储的是Var_Table中被使能的status,作为轮询的标志
int *polling_subarr;

const UINT32 locker_id[CARD_NUM] =
{
	2035400,
	13838538,
	1547012,
	10863352,
	2857740885u
};

#if 0
/*5次锁的轮询后轮询其它事项,其它事项再单独定义一个数组*/
UINT16 polling_cnt[] =
{

	VOLT_AMP_GET_FLAG_1

	,LOCKER_1_STATUS

	,LOCKER_2_STATUS

	,LOCKER_1_STATUS

	,LOCKER_2_STATUS

	,LOCKER_1_STATUS

	,LOCKER_2_STATUS

	,LOCKER_1_STATUS

	,LOCKER_2_STATUS

	,LOCKER_1_STATUS

	,LOCKER_2_STATUS
};
// 其它事项轮询顺序
UINT16 dev_rs485_cnt[] =
{

	VOLT_AMP_GET_FLAG_1

	,VOLT_AMP_GET_FLAG_2

	,VOLT_AMP_GET_FLAG_3

	,VOLT_AMP_GET_FLAG_4

	,VOLT_AMP_GET_FLAG_5

	,VOLT_AMP_GET_FLAG_6
};
#endif

/*寄存器索引表, 不是常量，要读配置文件*/
/*默认为1个电源控制板,2个电压电流传感器,2把锁*/
RS485_Reg_Table Var_Table[RS485_DEV_MAX_NUM] =
{
	{LOCKER_1,   ENABLE, 	 NULL_VAR,   		DOOR_LOCK_ADDR_1 +0},		// 锁1，地址为91
	{LOCKER_2,   ENABLE,     NULL_VAR,   		DOOR_LOCK_ADDR_1 +1},		// 锁2，地址为92
	{LOCKER_3,   DISABLE,    NULL_VAR,  		DOOR_LOCK_ADDR_1 +2},		// 锁3，地址为93,默认不使能
	{LOCKER_4,   DISABLE,    NULL_VAR,  		DOOR_LOCK_ADDR_1 +3},		// 锁3，地址为94,默认不使能
	{VA_METER_1, ENABLE,     NULL_VAR,   		VA_STATION_ADDRESS_1 +0},		// 电压电流传感器1，地址为81
	{VA_METER_2, ENABLE,     NULL_VAR,   		VA_STATION_ADDRESS_1 +1},		// 电压电流传感器2，地址为82
	{VA_METER_3, DISABLE, 	 NULL_VAR,   		VA_STATION_ADDRESS_1 +2},	// 电压电流传感器3，地址为83
	{VA_METER_4, DISABLE, 	 NULL_VAR,   		VA_STATION_ADDRESS_1 +3},	// 电压电流传感器4，地址为84
	{VA_METER_5, DISABLE,    NULL_VAR,   		VA_STATION_ADDRESS_1 +4},	// 电压电流传感器5，地址为85
	{VA_METER_6, DISABLE,    NULL_VAR,   		VA_STATION_ADDRESS_1 +5},	// 电压电流传感器6，地址为86
	{POWER_BD_1, ENABLE,     NULL_VAR,   	POWER_CTRL_ADDR_1 +0},		// 电源控制板1，地址为71
	{POWER_BD_2, DISABLE, 	NULL_VAR,   	POWER_CTRL_ADDR_1 +1},		// 电源控制板2，地址为72
	{POWER_BD_3, DISABLE, 	NULL_VAR,   	POWER_CTRL_ADDR_1 +2},		// 电源控制板3，地址为73
	{IO_BD_1, 	 DISABLE, 	NULL_VAR,   	IO_CTRL_ADDR_1 +0},		// IO控制板1，地址为61
	{IO_BD_2, 	 DISABLE, 	NULL_VAR,   	IO_CTRL_ADDR_1 +1},		// IO控制板2，地址为62
	{IO_BD_3, 	 DISABLE, 	NULL_VAR,   	IO_CTRL_ADDR_1 +2},		// IO控制板3，地址为63
};


/*设置485配置表的变量*/
void Rs485_table_set(UINT16 name, UINT16 enable, UINT16 position, UINT16 address)
{
	if(name >= RS485_DEV_MAX_NUM)
	{
		return;
	}

	if (enable != NULL_VAR)
	{
		Var_Table[name].enable = enable;
	}
	if (position != NULL_VAR)
	{
		Var_Table[name].position = position;
	}
	if (address != NULL_VAR)
	{
		Var_Table[name].addr = address;
	}
}


/*获取485配置表某位变量是否使能*/
UINT16 Rs485_table_enable_get(UINT16 name)
{
	if(name >= RS485_DEV_MAX_NUM)
	{
		return 0;
	}

	return Var_Table[name].enable;
}


/*模块化电源控制的处理*/
int Power_ctrl_process(UINT32 *pctrl_flag, RS485_DEV_LIST dev_name)
{
	int i;
	for(i=0; i<12; i++)
	{
		if (*pctrl_flag & (LBIT(POWER_1_CTRL_CLOSE+2*i)))
		{
			*pctrl_flag &= ~(LBIT(POWER_1_CTRL_CLOSE+2*i));
			if (Rs485_table_enable_get(dev_name))		// 配置文件中是否配置使能
			{
				WAIT_response_flag = WAIT_POWER_1_CTRL_CLOSE_RES+2*i;
				SendCom4RCtlReg(Var_Table[dev_name].addr,FORCE_COIL,VPLATE1_REG+i,SWITCH_OFF);
			}
			return 0;	// 跳出循环,等待下一个450ms后大循环再来控制，怕485冲突
		}
		else if (*pctrl_flag & (LBIT(POWER_1_CTRL_OPEN+2*i)))
		{
			*pctrl_flag &= ~(LBIT(POWER_1_CTRL_OPEN+2*i));
			if (Rs485_table_enable_get(dev_name))
			{
				WAIT_response_flag = WAIT_POWER_1_CTRL_OPEN_RES+2*i;
				SendCom4RCtlReg(Var_Table[dev_name].addr,FORCE_COIL,VPLATE1_REG+i,SWITCH_ON);
			}
			return 0;	// 跳出循环,等待下一个450ms后大循环再来控制，怕485冲突
		}
	}
	return 1;
}


/*模块化电子锁控制的处理*/
int Locker_ctrl_process(UINT32 *pctrl_flag, RS485_DEV_LIST dev_name)
{
	int i;
	for(i=0; i<LOCK_MAX_NUM; i++)
	{
		if (*pctrl_flag & LBIT(LOCKER_1_CTRL_UNLOCK+2*i))
		{
			*pctrl_flag &= ~(LBIT(LOCKER_1_CTRL_UNLOCK+2*i));
			if (Rs485_table_enable_get(dev_name+i))
			{
				WAIT_response_flag = WAIT_LOCKER_1_UNLOCK_RES+2*i;
				SendCom4RCtlReg(Var_Table[dev_name+i].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
			}
			return 0;
		}
		else if (*pctrl_flag & LBIT(LOCKER_1_CTRL_LOCK+2*i))
		{
			locker_ctrl_flag &= ~(LBIT(LOCKER_1_CTRL_LOCK+2*i));
			if (Rs485_table_enable_get(dev_name+i))
			{
				WAIT_response_flag = WAIT_LOCKER_1_LOCK_RES+2*i;
		 		SendCom4RCtlReg(Var_Table[dev_name+i].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
			}
			return 0;
		}
	}
	return 1;
}


/*模块化485设备的轮询处理*/
int Dev_polling_process(UINT32 *pcomm_flag)
{
	int i;
	for(i=LOCKER_1; i <= VA_METER_6; i++)
	{
		if (*pcomm_flag & LBIT(i))
		{
			*pcomm_flag &= ~(LBIT(i));
			if (Rs485_table_enable_get(i))
			{
				switch(i)
				{
				case (LOCKER_1):
				case (LOCKER_2):
				case (LOCKER_3):
					// 门锁统一处理
					WAIT_response_flag = WAIT_LOCKER_1_MSG+i;
					SendCom4ReadReg(Var_Table[i].addr,READ_REGS,DOOR_STATUS_REG,LOCKER_REG_NUM);
					break;	//注意这里的break只是跳出switch, 而不是跳出for

				case (VA_METER_1):
				case (VA_METER_2):
				case (VA_METER_3):
				case (VA_METER_4):
				case (VA_METER_5):
				case (VA_METER_6):
					// 门锁统一处理, RES标志混合处理，要特别小心
					WAIT_response_flag = WAIT_VA_DATA_1_MSG+(i-VA_METER_1);
					SendCom4ReadReg(Var_Table[i].addr, READ_REGS, VA_REG, VA_DATA_NUM);
					break;	//注意这里的break只是跳出switch, 而不是跳出for

				default:
					break;
				}
			}
			return 0;		// 处理一个后直接返回，下一个循环再处理另外一个标志位
		}
	}
	return 1;
}



void *Locker_DataPollingthread(void *param)
{
	param = NULL;
	int i;
	static UINT16 polling_counter = 0;
	static UINT16 loop_cnt = 0;			// 表明电子锁的轮询循环次数,超过阈值轮询其它的设备
	static UINT16 sub_poll_counter = 0;
	static UINT16 dev_485_entry = 0;

	while(1)
	{
		if ((locker_ctrl_flag&BITS_MSK_GET(0,LOCKER_CTRL_NUM)) || (power_ctrl_flag[0]&BITS_MSK_GET(0,POWER_CTRL_NUM))
			 || (power_ctrl_flag[1]&BITS_MSK_GET(0,POWER_CTRL_NUM)) || (power_ctrl_flag[2]&BITS_MSK_GET(0,POWER_CTRL_NUM)))
		{
			comm_flag = 0;
			printf("there is no 485msg\r\n");
		}
		else
		{
			if(dev_485_entry)
			{
				dev_485_entry = 0;
				// 其它的485设备
				if (sub_poll_counter < actual_485dev_num)
				{
					/*加个判断保险一点*/
					comm_flag |= LBIT(polling_subarr[sub_poll_counter]);
					printf("comm_flag=0x%04x\r\n",comm_flag);
				}
				sub_poll_counter++;
				if (sub_poll_counter >= actual_485dev_num)
				{
					printf("subroll over0x%02x" ,sub_poll_counter);printf("\r\n");
					sub_poll_counter = 0;
				}
			}
			else
			{
				// 主轮询, 电子锁
				if (polling_counter < actual_locker_num)
				{
					comm_flag |= LBIT(polling_arr[polling_counter]);
					printf("comm_flag=0x%04x\r\n",comm_flag);
				}
				polling_counter++;
				if (polling_counter >= actual_locker_num)
				{
					printf("\r\npoling over");
					printf("0x%02x" ,polling_counter);printf("\r\n");
					polling_counter = 0;
					loop_cnt++;
					if (loop_cnt >= LOCKER_LOOP_NUM)
					{
						loop_cnt = 0;
						dev_485_entry =1; // 其它设备轮询一次
					}
				}
			}
		}

		// 如果没有收到回复数据,这里会一直进不来,要用带timeout的信号量
		//if (!WAIT_response_flag)
		{
			// 控制,优先处理DO
			if (power_ctrl_flag[0]&BITS_MSK_GET(0,POWER_CTRL_NUM))	// 怕高位被意外置位跳不出来
			{
				Power_ctrl_process(&power_ctrl_flag[0],POWER_BD_1);
				#if 0
				for(i=0; i<12; i++)
				{
					if (power_ctrl_flag[0] & (LBIT(POWER_1_CTRL_CLOSE+2*i)))
					{
						power_ctrl_flag[0] &= ~(LBIT(POWER_1_CTRL_CLOSE+2*i));
						if (Rs485_table_enable_get(POWER_BD_1))
						{
							WAIT_response_flag = WAIT_POWER_1_CTRL_CLOSE_RES+2*i;
							SendCom4RCtlReg(Var_Table[POWER_BD_1].addr,FORCE_COIL,VPLATE1_REG+i,SWITCH_OFF);
						}
						break;	// 跳出循环,等待下一个450ms后大循环再来控制，怕485冲突
					}
					else if (power_ctrl_flag[0] & (LBIT(POWER_1_CTRL_OPEN+2*i)))
					{
						power_ctrl_flag[0] &= ~(LBIT(POWER_1_CTRL_OPEN+2*i));
						if (Rs485_table_enable_get(POWER_BD_1))
						{
							WAIT_response_flag = WAIT_POWER_1_CTRL_OPEN_RES+2*i;
							SendCom4RCtlReg(Var_Table[POWER_BD_1].addr,FORCE_COIL,VPLATE1_REG+i,SWITCH_ON);
						}
						break;	// 跳出循环,等待下一个450ms后大循环再来控制，怕485冲突
					}
				}
				#endif
			}
			else if (power_ctrl_flag[1]&BITS_MSK_GET(0,POWER_CTRL_NUM))	// 怕高位被意外置位跳不出来
			{
				Power_ctrl_process(&power_ctrl_flag[1],POWER_BD_2);
			}
			else if (power_ctrl_flag[2]&BITS_MSK_GET(0,POWER_CTRL_NUM))	// 怕高位被意外置位跳不出来
			{
				Power_ctrl_process(&power_ctrl_flag[2],POWER_BD_3);
			}

			else if (locker_ctrl_flag&BITS_MSK_GET(0,LOCKER_CTRL_NUM))			// 怕高位被意外置位跳不出来
			{
				Locker_ctrl_process(&locker_ctrl_flag,LOCKER_1);
			}
			else if (comm_flag &BITS_MSK_GET(LOCKER_1,VA_METER_6+1))
			{
				Dev_polling_process(&comm_flag);
			}
#if 0
			else if (locker_ctrl_flag & LBIT(LOCKER_1_CTRL_UNLOCK))
			{
				locker_ctrl_flag &= ~(LBIT(LOCKER_1_CTRL_UNLOCK));
				if (Rs485_table_enable_get(LOCKER_1))
				{
					WAIT_response_flag = WAIT_LOCKER_1_UNLOCK_RES;
					SendCom4RCtlReg(Var_Table[LOCKER_1].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
				}
			}
			else if (locker_ctrl_flag & LBIT(LOCKER_1_CTRL_LOCK))
			{
				locker_ctrl_flag &= ~(LBIT(LOCKER_1_CTRL_LOCK));
				if (Rs485_table_enable_get(LOCKER_1))
				{
					WAIT_response_flag = WAIT_LOCKER_1_LOCK_RES;
		 			SendCom4RCtlReg(Var_Table[LOCKER_1].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
				}
			}

			else if (locker_ctrl_flag & LBIT(LOCKER_2_CTRL_UNLOCK))
			{
				locker_ctrl_flag &= ~(LBIT(LOCKER_2_CTRL_UNLOCK));
				if (Rs485_table_enable_get(LOCKER_2))
				{
					WAIT_response_flag = WAIT_LOCKER_2_UNLOCK_RES;
		 			SendCom4RCtlReg(Var_Table[LOCKER_2].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
				}
			}
			else if (ctrl_flag & LBIT(LOCKER_2_CTRL_LOCK))
			{
				locker_ctrl_flag &= ~(LBIT(LOCKER_2_CTRL_LOCK));
				if (Rs485_table_enable_get(LOCKER_2))
				{
					WAIT_response_flag = WAIT_LOCKER_2_LOCK_RES;
		 			SendCom4RCtlReg(Var_Table[LOCKER_2].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
				}
			}
			else if (locker_ctrl_flag & LBIT(LOCKER_3_CTRL_UNLOCK))
			{
				locker_ctrl_flag &= ~(LBIT(LOCKER_3_CTRL_UNLOCK));
				if (Rs485_table_enable_get(LOCKER_3))
				{
					WAIT_response_flag = WAIT_LOCKER_3_UNLOCK_RES;
		 			SendCom4RCtlReg(Var_Table[LOCKER_3].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
				}
			}
			else if (ctrl_flag & LBIT(LOCKER_3_CTRL_LOCK))
			{
				locker_ctrl_flag &= ~(LBIT(LOCKER_3_CTRL_LOCK));
				if (Rs485_table_enable_get(LOCKER_3))
				{
					WAIT_response_flag = WAIT_LOCKER_3_LOCK_RES;
		 			SendCom4RCtlReg(Var_Table[LOCKER_3].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
				}
			}

			// 轮询
			else if (comm_flag &LBIT(LOCKER_1_STATUS))
			{
				comm_flag &= ~(LBIT(LOCKER_1_STATUS));
				if (Rs485_table_enable_get(LOCKER_1))
				{
					SendCom4ReadReg(Var_Table[LOCKER_1].addr,READ_REGS,DOOR_STATUS_REG,LOCKER_REG_NUM);
					WAIT_response_flag = WAIT_LOCKER_1_MSG;
				}
			}
			else if (comm_flag &LBIT(LOCKER_2_STATUS))
			{
				comm_flag &= ~(LBIT(LOCKER_2_STATUS));
				if (Rs485_table_enable_get(LOCKER_2))
				{
					SendCom4ReadReg(Var_Table[LOCKER_2].addr,READ_REGS,DOOR_STATUS_REG,LOCKER_REG_NUM);
					WAIT_response_flag = WAIT_LOCKER_2_MSG;
				}
			}
			else if (comm_flag &LBIT(LOCKER_3_STATUS))
			{
				comm_flag &= ~(LBIT(LOCKER_3_STATUS));
				if (Rs485_table_enable_get(LOCKER_3))
				{
					SendCom4ReadReg(Var_Table[LOCKER_3].addr,READ_REGS,DOOR_STATUS_REG,LOCKER_REG_NUM);
					WAIT_response_flag = WAIT_LOCKER_3_MSG;
				}
			}

			else if (comm_flag &LBIT(VOLT_AMP_GET_FLAG_1))
			{
				comm_flag &= ~(LBIT(VOLT_AMP_GET_FLAG_1));
				SendCom4ReadReg(stuVA_Meter_Param[0]->address, READ_REGS, VA_REG, VA_DATA_NUM);
				WAIT_response_flag = WAIT_VA_DATA_1_MSG;
			}

			else if (comm_flag &LBIT(VOLT_AMP_GET_FLAG_2))
			{
				comm_flag &= ~(LBIT(VOLT_AMP_GET_FLAG_2));
				SendCom4ReadReg(stuVA_Meter_Param[1]->address, READ_REGS, VA_REG, VA_DATA_NUM);
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

/*处理电子锁的轮询信息*/
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
		addr_base = addr-lockerHw_Param[0]->address;
		if (addr_base > 2)		// 防止内存溢出
		{
			return 0;
		}

		pointer = &(lockerHw_Param[addr_base]->status);
		pid = &(lockerHw_Param[addr_base]->id[0]);

		/*读取前面的4个状态量,即status,open reason,...*/
		for (i=0;i<FIXED_NUM;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
		// 如果本次读取的report_cnt比上次还要小,表明128次上报已经结束,锁的状态重置为未开锁
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

	/*锁的ID从id1开始,id0是生产厂商编码*/
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
				locker_ctrl_flag |= LBIT(LOCKER_1_CTRL_UNLOCK+addr_base*2);
				//SendCom4RCtlReg(addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
				locker_opened[addr_base] =1;
				last_card = card_read;	// 记录上次开锁的门卡,暂时未使用
				break;
			}
		}
	}
	last_cnt[addr_base] = lockerHw_Param[addr_base]->report_cnt;
	return 1 ;
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
	UINT16 *pointer = &stuVA_Meter_Param[seq]->phase[0].vln;	/*第5相,实际接点路序和传感器是倒序的关系*/

	if(len == (REAL_DATA_NUM*2+5))
	{
		printf("va begain\r\n");
		/*第5相*/
		pointer = &stuVA_Meter_Param[seq]->phase[0].vln;
		for(i = 35;i <= 36;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-35));
		}
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[0].vln);printf("\r\n");
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[0].amp);printf("\r\n");

		/*第4相*/
		pointer = &stuVA_Meter_Param[seq]->phase[1].vln;
		for(i = 28;i <= 29;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-28));
		}
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[1].vln);printf("\r\n");
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[1].amp);printf("\r\n");

		/*第3相*/
		pointer = &stuVA_Meter_Param[seq]->phase[2].vln;
		for(i = 21;i <= 22;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-21));
		}
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[2].vln);printf("\r\n");
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[2].amp);printf("\r\n");

		/*第2相*/
		pointer = &stuVA_Meter_Param[seq]->phase[3].vln;
		for(i = 14;i <= 15;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-14));
		}
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[3].vln);printf("\r\n");
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[3].amp);printf("\r\n");

		/*第1相*/
		pointer = &stuVA_Meter_Param[seq]->phase[4].vln;
		for(i = 7;i <= 8;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-7));
		}
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[4].vln);printf("\r\n");
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[4].amp);printf("\r\n");

		/*第0相*/
		pointer = &stuVA_Meter_Param[seq]->phase[5].vln;
		for (i=0;i<2;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[5].vln);printf("\r\n");
		printf("%5hd ",stuVA_Meter_Param[seq]->phase[5].amp);printf("\r\n");
	}
}



int DealComm485(unsigned char *buf,unsigned short int len)
{
	switch(Recive_Flag)
	{
		case WAIT_LOCKER_1_MSG:					/*MSG from the locker*/
		case WAIT_LOCKER_2_MSG:
		case WAIT_LOCKER_3_MSG:
		case WAIT_LOCKER_4_MSG:
			DealLockerMsg(buf, len);
		break;

		case WAIT_VA_DATA_1_MSG:				/*MSG from the Volt-amp detector*/
			comm_VAData_analyse(buf, len,0);
		break;
		case WAIT_VA_DATA_2_MSG:
			comm_VAData_analyse(buf, len,1);
		break;
		case WAIT_VA_DATA_3_MSG:				/*MSG from the Volt-amp detector*/
			comm_VAData_analyse(buf, len,2);
		break;
		case WAIT_VA_DATA_4_MSG:
			comm_VAData_analyse(buf, len,3);
		break;
		case WAIT_VA_DATA_5_MSG:				/*MSG from the Volt-amp detector*/
			comm_VAData_analyse(buf, len,4);
		break;
		case WAIT_VA_DATA_6_MSG:
			comm_VAData_analyse(buf, len,5);
		break;

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

      	printf("com4 len=%d\r\n",buffPos) ;
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
	int temp = 0;

	mComPort4 = new CComPort();
	/*there is only ttysp1 for RS485 now in A287，ttymxc3是串口4*/
	mComPort4->fd = mComPort4->openSerial((char *)"/dev/ttymxc3",9600);
	printf("ttymxc3 status");
	printf("0x%02x \r\n",mComPort4->fd);
	temp = BITS_MSK_GET(0,LOCKER_CTRL_NUM);
	printf("debug 0x%02x \r\n",temp);
	temp = BITS_MSK_GET(0,POWER_CTRL_NUM);
	printf("debug 0x%02x \r\n",temp);

	pthread_t m_ComPort4Thread ;
	pthread_create(&m_ComPort4Thread,NULL,ComPort4Thread,NULL);

	/*与电子锁有关的变量进行动态分配*/
	locker_opened = (UINT16*)malloc(sizeof(UINT16)*actual_locker_num);
	memset(locker_opened,0,sizeof(UINT16)*actual_locker_num);
	last_cnt = (UINT16*)malloc(sizeof(UINT16)*actual_locker_num);
	memset(last_cnt,0,sizeof(UINT16)*actual_locker_num);
}


// DDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + MBUS_OPT_CODE_H + MBUS_OPT_CODE_L + CRC(2)
UINT16 SendCom4RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code)
{
    Com4SendCri.Lock();
    UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};

    int len=8;

    bytSend[MBUS_ADDR]        = Addr;
    bytSend[MBUS_FUNC]        = Func;
    bytSend[MBUS_REFS_ADDR_H] = (REFS_ADDR&0xFF00) >> 8;
    bytSend[MBUS_REFS_ADDR_L] =  REFS_ADDR&0x00FF;
    bytSend[MBUS_OPT_CODE_H] = (code&0xFF00) >> 8;     // bytSend[MBUS_OPT_CODE_L] =  code&0x00FF;
	bytSend[MBUS_OPT_CODE_L] =  code&0xFF;
	//CRC
    unsigned short int CRC = CRC16(bytSend,len-2) ;
    bytSend[len-2] = (CRC&0xFF00) >> 8;     //CRC
    bytSend[len-1] =  CRC&0x00FF;           //CRC
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort4->SendBuf(bytSend,len);

	//memcpy(LastSendBuf,bytSend,len);
	//LastSendLen=len;
	//free(bytSend);

    Com4SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0;
}

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


