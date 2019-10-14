
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
#include "rs485Com3.h"
#include "rs485Com4.h"
#include "MyCritical.h"
#include <string>
#include <semaphore.h>
#include "tea.h"
#include "rtc.h"
#include "server.h"
#include "config.h"

/*Global definition*/
using namespace std;

UINT32  comm_flag[RS485_NUM] = {0,0};	// 轮询标志
//UINT32  ctrl_flag;
UINT32  locker_ctrl_flag=0;	// 电子锁写标志
UINT32	power_ctrl_flag[POWER_BD_MAX_NUM] = {0,0,0};
UINT32	power_read_flag[POWER_BD_MAX_NUM] = {0,0,0};	// 控制一次电压电流通断后马上读一次数据
UINT16	power_ver_flag = 0;		// 读版本号
UINT8  WAIT_response_flag[RS485_NUM]={0,0};
UINT8 Recive_Flag[RS485_NUM] = {0,0};			/* 接收标志*/
UINT8 actual_locker_num = 0;
UINT8 actual_485dev_num = 0;

/*Unpack the data from RS485*/
#define FIXED_NUM		4	// The former num of the locker's status
static UINT16 *locker_opened;
static UINT16 *last_cnt;
static UINT32 last_card = 0;


//UINT8 actual_locker_num = 0;
extern string StrServerURL1;	//服务端URL1
extern string StrServerURL2;	//服务端URL2
extern string StrServerURL4;	//服务端URL4

extern LOCKER_HW_PARAMS *lockerHw_Param[LOCK_MAX_NUM];	//门锁状态结构体
extern VA_METER_PARAMS *stuVA_Meter_Param[VA_METER_BD_NUM];		//伏安表电压电流结构体
extern REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
extern VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体

string StrAdrrLock[LOCK_MAX_NUM];	//门锁1的地址

string StrAdrrVAMeter[VA_METER_BD_MAX_NUM];	//电压电流传感器1的地址
string StrAdrrPower[POWER_BD_MAX_NUM];	//电源板1的地址
string StrDeviceNameSeq[SWITCH_COUNT];	//设备名的配置
string StrDoSeq[SWITCH_COUNT];	//do和设备映射的配置
UINT16 DoSeq[SWITCH_COUNT]={0,};	// 另外定义一个专门用来存储映射的数组,stuRemote_Ctrl会被清0

extern void RemoteControl(UINT8* pRCtrl);

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
	{LOCKER_4,	 DISABLE,	 NULL_VAR,			DOOR_LOCK_ADDR_1 +3},		// 锁3，地址为94,默认不使能
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

/*和电子锁有关的变量初始化*/
void lockerDataMalloc(void)
{
	/*与电子锁有关的变量进行动态分配*/
	locker_opened = (UINT16*)malloc(sizeof(UINT16)*actual_locker_num);
	memset(locker_opened,0,sizeof(UINT16)*actual_locker_num);
	last_cnt = (UINT16*)malloc(sizeof(UINT16)*actual_locker_num);
	memset(last_cnt,0,sizeof(UINT16)*actual_locker_num);
}

/**********依赖的通用运算函数************************************************/
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

int SendPowerVerReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
	UINT16 reval;
	reval = SendCom4ReadReg(Addr, Func, REFS_ADDR, REFS_COUNT);
	return reval;
}
/**********依赖的通用运算函数完毕************************************************/
/********************************************************************************/


/**********接口函数，调用各串口的读写函数********************************************/

/* 电子锁控制 ,指定使用哪个串口->com3*/
UINT16 SendLockerRCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 Cmd)
{
	UINT16 reval;
	reval = SendCom3RCtlReg(Addr, Func, REFS_ADDR,Cmd);
	return reval;
}

int SendLockerReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
	UINT16 reval;
	reval = SendCom3ReadReg(Addr, Func, REFS_ADDR, REFS_COUNT);
	return reval;
}

/* 电源板DO控制 ,指定使用哪个串口->com4*/
UINT16 SendPowerRCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 Cmd)
{
	UINT16 reval;
	reval = SendCom4RCtlReg(Addr, Func, REFS_ADDR,Cmd);
	return reval;
}

int SendPowerReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
	UINT16 reval;
	reval = SendCom4ReadReg(Addr, Func, REFS_ADDR, REFS_COUNT);
	return reval;
}

/**********接口函数定义完毕****************************************************************/
/******************************************************************************************/


/*模块化电源控制的处理,输出后马上读取一次*/
int Power_ctrl_process(UINT32 *pctrl_flag, UINT32 *pread_flag, RS485_DEV_LIST dev_name)
{
	int i;
	for(i=0; i<12; i++)
	{
		if (*pctrl_flag & (LBIT(POWER_1_CTRL_CLOSE+2*i)))
		{
			*pctrl_flag &= ~(LBIT(POWER_1_CTRL_CLOSE+2*i));
			*pread_flag |= (LBIT(i));	// 读取标志置位
			if (Rs485_table_enable_get(dev_name))		// 配置文件中是否配置使能
			{
				WAIT_response_flag[RS485_2] = WAIT_POWER_1_CTRL_CLOSE_RES+2*i;
				SendPowerRCtlReg(Var_Table[dev_name].addr,FORCE_COIL,VPLATE1_REG+i,SWITCH_OFF);
			}
			return 0;	// 跳出循环,等待下一个450ms后大循环再来控制，怕485冲突
		}
		else if (*pctrl_flag & (LBIT(POWER_1_CTRL_OPEN+2*i)))
		{
			*pctrl_flag &= ~(LBIT(POWER_1_CTRL_OPEN+2*i));
			*pread_flag |= (LBIT(i));
			if (Rs485_table_enable_get(dev_name))
			{
				WAIT_response_flag[RS485_2] = WAIT_POWER_1_CTRL_OPEN_RES+2*i;
				SendPowerRCtlReg(Var_Table[dev_name].addr,FORCE_COIL,VPLATE1_REG+i,SWITCH_ON);
			}
			return 0;	// 跳出循环,等待下一个450ms后大循环再来控制，怕485冲突
		}
	}
	return 1;
}


/*模块化电源控制的处理,输出后马上读取一次的处理*/
int Power_readback_process(UINT32 *pread_flag, UINT32 *pcomm_flag, RS485_DEV_LIST dev_name)
{
	int i;
	RS485_DEV_LIST temp = dev_name;

	// 全部判读后一次性置位
	for(i=0; i<12; i++)
	{
		if (*pread_flag & (LBIT(i)))
		{
			*pread_flag &= ~(LBIT(i));
			if (i < 6)
			{
				temp = dev_name;
			}
			else
			{
				// 是7~12路，下一个电压电流传感器
				temp = (RS485_DEV_LIST)(dev_name +1);
			}
			if (Rs485_table_enable_get((UINT16)temp))
			{
				*pcomm_flag |= LBIT(temp);
			}
			return 0;
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
				WAIT_response_flag[RS485_1] = WAIT_LOCKER_1_UNLOCK_RES+2*i;
				SendLockerRCtlReg(Var_Table[dev_name+i].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
			}
			return 0;
		}
		else if (*pctrl_flag & LBIT(LOCKER_1_CTRL_LOCK+2*i))
		{
			locker_ctrl_flag &= ~(LBIT(LOCKER_1_CTRL_LOCK+2*i));
			if (Rs485_table_enable_get(dev_name+i))
			{
				WAIT_response_flag[RS485_1] = WAIT_LOCKER_1_LOCK_RES+2*i;
		 		SendLockerRCtlReg(Var_Table[dev_name+i].addr,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
			}
			return 0;
		}
	}
	return 1;
}


/*电子锁设备的轮询处理*/
int Locker_polling_process(UINT32 *pcomm_flag)
{
	int i;
	for(i=LOCKER_1; i <= LOCKER_4; i++)
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
				case (LOCKER_4):
					// 门锁统一处理
					WAIT_response_flag[RS485_1] = WAIT_LOCKER_1_MSG+i;
					SendLockerReadReg(Var_Table[i].addr,READ_REGS,DOOR_STATUS_REG,LOCKER_REG_NUM);
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


/*模块化485设备的轮询处理*/
int Dev_polling_process(UINT32 *pcomm_flag)
{
	int i;
	for(i=VA_METER_1; i <= VA_METER_6; i++)
	{
		if (*pcomm_flag & LBIT(i))
		{
			*pcomm_flag &= ~(LBIT(i));
			if (Rs485_table_enable_get(i))
			{
				switch(i)
				{
				case (VA_METER_1):
				case (VA_METER_2):
				case (VA_METER_3):
				case (VA_METER_4):
				case (VA_METER_5):
				case (VA_METER_6):
					// 门锁统一处理, RES标志混合处理，要特别小心
					WAIT_response_flag[RS485_2] = WAIT_VA_DATA_1_MSG+(i-VA_METER_1);
					SendPowerReadReg(Var_Table[i].addr, READ_REGS, VA_REG, VA_DATA_NUM);
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

/*电源控制板的版本号轮询*/
int Ver_polling_process(UINT16 *pver_flag)
{
	int i;
	for(i = 0; i < POWER_BD_RD_NUM; i++)
	{
		if (*pver_flag & LBIT(i))
		{
			*pver_flag &= ~(LBIT(i));
			if (Rs485_table_enable_get(i+POWER_BD_1))
			{
				switch(i)
				{
				case (POWER_BD_RD_1):
			#if (POWER_BD_NUM >=2)
				case (POWER_BD_RD_2):
			#endif
			#if (POWER_BD_NUM >=3)
				case (POWER_BD_RD_3):
			#endif
					// 门锁统一处理, RES标志混合处理，要特别小心
					WAIT_response_flag[RS485_2] = WAIT_POWER_1_VER_RES+i;
					SendPowerVerReg(Var_Table[i+POWER_BD_1].addr, READ_REGS, VER_REG, VER_DATA_NUM);
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


void *Dev_DataPollingthread(void *param)
{
	param = NULL;
	static UINT16 dataget_cnt = 0;
	static UINT16 dev_poll_counter = 0;
	static UINT16 loop_cnt = 0;			// 表明电压电流传感器轮询循环次数,超过阈值轮询其它的设备
	static UINT16 ver_entry = 0;		// 轮询电源板的版本号
	static UINT16 ver_poll_counter = 0;

	while(1)
	{
		if ((power_ctrl_flag[0]&BITS_MSK_GET(0,POWER_CTRL_NUM))|| (power_ctrl_flag[1]&BITS_MSK_GET(0,POWER_CTRL_NUM)) \
			|| (power_ctrl_flag[2]&BITS_MSK_GET(0,POWER_CTRL_NUM)) \
			|| (power_read_flag[0]&BITS_MSK_GET(0,POWER_CTRL_NUM/2))|| (power_read_flag[1]&BITS_MSK_GET(0,POWER_CTRL_NUM/2)) \
			|| (power_read_flag[2]&BITS_MSK_GET(0,POWER_CTRL_NUM/2))|| (power_ver_flag&BITS_MSK_GET(0,POWER_BD_RD_NUM)))
		{
			comm_flag[RS485_2] = 0;
//			printf("power_ctrl_flag0485=0x%04x\r\n",power_ctrl_flag[0]);
		}
		else
		{
			dataget_cnt++;
			// 360*8 = 2.9s，即2.9s轮询一个VA传感器
			if (dataget_cnt >= VA_INTERVAL)
			{
				dataget_cnt = 0;
				if (ver_entry)
				{
					ver_entry = 0;
					// 电源板的版本号轮询
					if (ver_poll_counter < POWER_BD_RD_NUM)
					{
						/*加个判断保险一点*/
						if (Rs485_table_enable_get(ver_poll_counter+POWER_BD_1))
						{
							power_ver_flag |= LBIT(ver_poll_counter);
							//printf("power_ver_flag=0x%04x\r\n",power_ver_flag);
						}
					}
					ver_poll_counter++;
					if (ver_poll_counter >= POWER_BD_RD_NUM)
					{
						//printf("verpoll over0x%02x" ,ver_poll_counter);printf("\r\n");
						ver_poll_counter = 0;
					}
				}
				else
				{
					// 轮询电压电流传感器数据,2.9s一次
					if (dev_poll_counter < actual_485dev_num)
					{
						comm_flag[RS485_2] |= LBIT(polling_subarr[dev_poll_counter]);
						//printf("comm_flag2=0x%04x\r\n",comm_flag[RS485_2]);
					}
					dev_poll_counter++;
					if (dev_poll_counter >= actual_485dev_num)
					{
//						printf("\r\ndevpoling over");
//						printf("0x%02x" ,dev_poll_counter);printf("\r\n");
						dev_poll_counter = 0;
						loop_cnt++;
						// 如果有6个VA传感器，要2.9*6*6s才轮询一次电源板
						if (loop_cnt >= VA_LOOP_NUM)
						{
							loop_cnt = 0;
							ver_entry =1; // 其它设备轮询一次
						}
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
				Power_ctrl_process(&power_ctrl_flag[0],&power_read_flag[0],POWER_BD_1);
			}
			else if (power_ctrl_flag[1]&BITS_MSK_GET(0,POWER_CTRL_NUM))	// 怕高位被意外置位跳不出来
			{
				Power_ctrl_process(&power_ctrl_flag[1],&power_read_flag[1],POWER_BD_2);
			}
			else if (power_ctrl_flag[2]&BITS_MSK_GET(0,POWER_CTRL_NUM))	// 怕高位被意外置位跳不出来
			{
				Power_ctrl_process(&power_ctrl_flag[2],&power_read_flag[2],POWER_BD_3);
			}
			else if (power_read_flag[0]&BITS_MSK_GET(0,(POWER_CTRL_NUM/2)))
			{
				Power_readback_process(&power_read_flag[0],&comm_flag[RS485_2],VA_METER_1);
				Dev_polling_process(&comm_flag[RS485_2]);
			}
			else if (power_read_flag[1]&BITS_MSK_GET(0,(POWER_CTRL_NUM/2)))
			{
				Power_readback_process(&power_read_flag[1],&comm_flag[RS485_2],VA_METER_3);
				Dev_polling_process(&comm_flag[RS485_2]);
			}
			else if (power_read_flag[2]&BITS_MSK_GET(0,(POWER_CTRL_NUM/2)))
			{
				Power_readback_process(&power_read_flag[2],&comm_flag[RS485_2],VA_METER_5);
				Dev_polling_process(&comm_flag[RS485_2]);
			}
			else if (comm_flag[RS485_2] &BITS_MSK_GET(VA_METER_1,VA_METER_6+1))
			{
				Dev_polling_process(&comm_flag[RS485_2]);
			}
			else if (power_ver_flag &BITS_MSK_GET(0,POWER_BD_RD_NUM))
			{
				Ver_polling_process(&power_ver_flag);
			}
		}
		usleep(DEV_INTERVAL_TIME);		// every 3.6s sending
	}
	return 0 ;
}

void *Locker_DataPollingthread(void *param)
{
	param = NULL;
	int i;
	static UINT16 polling_counter = 0;

	while(1)
	{
		if (locker_ctrl_flag&BITS_MSK_GET(0,LOCKER_CTRL_NUM))
		{
			comm_flag[RS485_1] = 0;
			//printf("locker_ctrl_flag=0x%04x\r\n",locker_ctrl_flag);
		}
		else
		{
			// 主轮询, 电子锁
			if (polling_counter < actual_locker_num)
			{
				comm_flag[RS485_1] |= LBIT(polling_arr[polling_counter]);
				//printf("comm_flag=0x%04x\r\n",comm_flag[RS485_1]);
			}
			polling_counter++;
			if (polling_counter >= actual_locker_num)
			{
//				printf("\r\npoling over");
//				printf("0x%02x" ,polling_counter);printf("\r\n");
				polling_counter = 0;
			}
		}

		// 如果没有收到回复数据,这里会一直进不来,要用带timeout的信号量
		//if (!WAIT_response_flag)
		{

			if (locker_ctrl_flag&BITS_MSK_GET(0,LOCKER_CTRL_NUM))			// 怕高位被意外置位跳不出来
			{
				Locker_ctrl_process(&locker_ctrl_flag,LOCKER_1);
			}
			else if (comm_flag[RS485_1] &BITS_MSK_GET(LOCKER_1,LOCKER_4+1))
			{
				Locker_polling_process(&comm_flag[RS485_1]);
			}
		}
		usleep(INTERVAL_TIME);		// every 350ms sending
	}
	return 0 ;
}


/*处理电子锁的轮询信息*/
int DealLockerMsg(unsigned char seq,unsigned char *buf,unsigned short int len)
{
	UINT8 i=0;
	UINT8 addr = 1;
	//UINT8 addr_base = 0;
	UINT16* pointer = &(lockerHw_Param[seq]->status);
	UINT8* pid = &(lockerHw_Param[seq]->id[0]);
	UINT32 card_read = 0;
	string jsonstr;
    string mstrkey = ""; //没有用户名和密码：则为“”；
	REMOTE_CONTROL *pRCtrl;
	int ret;

	if (seq >= LOCK_MAX_NUM)
	{
		return 0;
	}
	if((len == (LOCKER_REG_NUM*2+5)))
	{
		addr = *(buf+0);		// the first byte is the addr.
		//addr_base = addr-lockerHw_Param[0]->address;
		//if (addr_base > 2)		// 防止内存溢出
		//{
		//	return 0;
		//}

		pointer = &(lockerHw_Param[seq]->status);
		pid = &(lockerHw_Param[seq]->id[0]);

		/*读取前面的4个状态量,即status,open reason,...*/
		for (i=0;i<FIXED_NUM;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
		// 如果本次读取的report_cnt比上次还要小,表明128次上报已经结束,锁的状态重置为未开锁
		if(last_cnt[seq] >= lockerHw_Param[seq]->report_cnt)
		{
			locker_opened[seq] = 0;
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
	char_to_long(&(lockerHw_Param[seq]->id[1]),&card_read);

	printf("lock begain\r\n");
	//printf("%5hd ",lockerHw_Param[addr_base]->status);printf("\r\n");
	//printf("%5hd ",lockerHw_Param[addr_base]->open_reason);printf("\r\n");
	//printf("%5hd ",lockerHw_Param[addr_base]->report_cnt);printf("\r\n");
	//printf("%5hd ",lockerHw_Param[addr_base]->id_length);printf("\r\n");
	int j ;//for(j=0;j<(LOCKER_REG_NUM-FIXED_NUM)*2;j++) printf("0x%02x ",lockerHw_Param->id[j]);
	printf("0x%08x" ,card_read);printf("\r\n");

	//上传条件:1.读取到卡号 2.这张卡是第一张卡 report_cnt为1，或者换了一张卡, report_cnt为1
	if ((card_read!=0) && ((last_cnt[seq] >= lockerHw_Param[seq]->report_cnt) \
		||((lockerHw_Param[seq]->report_cnt == 1) && (last_cnt[seq]==0))))
	{
		SetjsonDealLockerStr(NETCMD_DEAL_LOCKER,card_read,addr,jsonstr);
		printf("DealLockerMsg jsonstr=%s\n" ,jsonstr.c_str());
		NetSendParm(NETCMD_DEAL_LOCKER, (char*)jsonstr.c_str(), jsonstr.length());
		if(StrServerURL4.length()>0)			//上报利通后台
		{
		   ret=HttpPostParm(StrServerURL4,jsonstr,mstrkey,HTTPPOST);
		   if(ret)
		   {
			   pRCtrl=stuRemote_Ctrl;
			   memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
			   jsonstrRCtrlReader((char*)jsonstr.c_str(),jsonstr.length(),(UINT8 *)pRCtrl);//将json字符串转换成结构体
			   RemoteControl((UINT8*)pRCtrl);
		   }
		   else
		   		printf("LTKJ HttpPostParm error, ret=%d\n",ret);
		}
		if(StrServerURL2.length()>0)			//上报新粤后台
		{
		   ret=HttpPostParm(StrServerURL2,jsonstr,mstrkey,HTTPPOST);
		   if(ret)
		   {
			   pRCtrl=stuRemote_Ctrl;
			   memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
			   jsonstrRCtrlReader((char*)jsonstr.c_str(),jsonstr.length(),(UINT8 *)pRCtrl);//将json字符串转换成结构体
			   RemoteControl((UINT8*)pRCtrl);
		   }
		   else
				printf("XY HttpPostParm error, ret=%d\n",ret);
		}
	}
/*	for (j=0;j<CARD_NUM;j++)
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
	}*/
	last_cnt[seq] = lockerHw_Param[seq]->report_cnt;
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
		//printf("va begain\r\n");
		/*第5相*/
		pointer = &stuVA_Meter_Param[seq]->phase[0].vln;
		for(i = 35;i <= 36;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-35));
		}
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[0].vln);printf("\r\n");
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[0].amp);printf("\r\n");

		/*第4相*/
		pointer = &stuVA_Meter_Param[seq]->phase[1].vln;
		for(i = 28;i <= 29;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-28));
		}
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[1].vln);printf("\r\n");
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[1].amp);printf("\r\n");

		/*第3相*/
		pointer = &stuVA_Meter_Param[seq]->phase[2].vln;
		for(i = 21;i <= 22;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-21));
		}
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[2].vln);printf("\r\n");
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[2].amp);printf("\r\n");

		/*第2相*/
		pointer = &stuVA_Meter_Param[seq]->phase[3].vln;
		for(i = 14;i <= 15;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-14));
		}
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[3].vln);printf("\r\n");
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[3].amp);printf("\r\n");

		/*第1相*/
		pointer = &stuVA_Meter_Param[seq]->phase[4].vln;
		for(i = 7;i <= 8;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i-7));
		}
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[4].vln);printf("\r\n");
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[4].amp);printf("\r\n");

		/*第0相*/
		pointer = &stuVA_Meter_Param[seq]->phase[5].vln;
		for (i=0;i<2;i++)
		{
			char_to_int(buf + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[5].vln);printf("\r\n");
		//printf("%5hd ",stuVA_Meter_Param[seq]->phase[5].amp);printf("\r\n");
	}
}


/******************************************************************************
 * 函数名:	comm_VerData_analyse
 * 描述: 		装置信息的读取
 *            -
 * 输入参数:
 * 输出参数:
 * 返回值:
 *
 * 作者:Jerry
 * 创建日期:2019.7.12
 ******************************************************************************/

void comm_VerData_analyse(unsigned char *buf,unsigned short int len,unsigned char seq)
{
	UINT8 i;
	UINT8 addr_base = 0;
	UINT16 verCode = 0;
	//UINT16 verProtocol = 0;		/*规约版本号*/
	UINT16 *pointer = &verCode;	/*程序版本号*/
	UINT8 verchar[10];
	UINT16 temp1 = 0;
	UINT16 temp2 = 0;

	if(len == (VER_DATA_NUM*2+5))
	{
		addr_base = *(buf+0)-POWER_CTRL_ADDR_1;		// the first byte is the addr.
		//printf("ver begain\r\n");
		// 暂时只处理程序版本号
		char_to_int(buf + FRAME_HEAD_NUM, pointer);
		temp1 = verCode;
		for(i=0; i<5; i++)
		{
			temp2 = temp1%10;
			temp1 = temp1/10;
			verchar[i] = temp2;
		}
		sprintf(stuVMCtl_Param->secSoftVersion[addr_base],"V%d.%d%d.%d%d\0",verchar[4],verchar[3],verchar[2],verchar[1],verchar[0]);
		//printf("%s",stuVMCtl_Param->secSoftVersion[addr_base]);
	}
}


int DealComm485(unsigned char *buf,unsigned short int len, RS485_COM_LIST seq)
{
	Recive_Flag[seq] = WAIT_response_flag[seq];	// already received the data.
	switch(Recive_Flag[seq])
	{
		case WAIT_LOCKER_1_MSG:					/*MSG from the locker*/
			DealLockerMsg(0,buf, len);
		break;
		case WAIT_LOCKER_2_MSG:
			DealLockerMsg(1,buf, len);
		break;
		case WAIT_LOCKER_3_MSG:
			DealLockerMsg(2,buf, len);
		break;
		case WAIT_LOCKER_4_MSG:
			DealLockerMsg(3,buf, len);
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

		case WAIT_POWER_1_VER_RES:					/*MSG from the POWERBOARD*/
		case WAIT_POWER_2_VER_RES:
		case WAIT_POWER_3_VER_RES:
			comm_VerData_analyse(buf, len,5);
		break;

		default:
		break;
	}

	Recive_Flag[seq] = 0;
	WAIT_response_flag[seq] = 0;
	return 0;
}


/* RS485初始化 */
void rs485init(void)
{
	Rs485Com3Init();
	Rs485Com4Init();
	/*开机读一次版本信息*/
	power_ver_flag |=BITS_MSK_GET(0,POWER_BD_RD_NUM);
}


