#ifndef __NETPSD_H__
#define __NETPSD_H__

#include "registers.h"

#define BIT(i)	(1<<(i))	// 32位平台32位?


#define SPD_ID_ADDR		10		// 改设备id的地址
#define DO_ON_CMD	0xFF00	// 清零
#define DO_OFF_CMD	0x0

#define RES_TEST_ADDR		0x11	// 测试接地电阻值
#define RES_ID_ADDR			0x12		// 改接地电阻设备id的地址
#define RES_ALARM_ADDR		0x13		// 更改报警值的地址
#define RES_TEST_EN			0x1			// 接地电阻测试开始


//读命令码
#define SPD_DO_CMD		0x01
#define SPD_DI_CMD		0x02
#define SPD_READ_CMD		0x03
#define SPD_RES_READ_CMD	0x03

//写命令码
#define SPD_DO_CTRL_CMD		0x05
#define SPD_RES_SET_CMD		0x06
#define SPD_WRITE_CMD		0x10

#define SPD_ADDR			0x0B	// 设备地址
#define SPD_RES_ADDR		0x09	// 接地电阻设备地址

#define SPD_AI_ADDR			0
#define SPD_AI_NUM			58

// 全部读出来
#define SPD_DI_ADDR			0
#define SPD_DI_NUM			8	// DI5未规定

#define SPD_DO_ADDR			0
#define SPD_DO_NUM			6	// DO3未规定

#define SPD_RES_STATUS_ADDR			0x0C
#define SPD_RES_STATUS_NUM			8	// 8个数据,后面3个是可读可写的


#define SPD_INTERVAL_TIME	350000		// 350ms,让控制命令更快下发
#define SPD_POLLING_INTERVAL 4			// 1.2s轮询一次参数


// 4字节表示一个float型
typedef union {
 float f;
 unsigned char c[4];
} FDATA;


// PSD子结构体--AI输入
typedef struct spd_ai_struct
{
	FDATA leak_current;	// 漏电流
	FDATA ref_volt;		// 基准电压
	FDATA real_volt;	// 实时电压
	FDATA spd_temp;		// 防雷器温度
	FDATA envi_temp;	// 环境温度
	FDATA id;			// 设备地址
	FDATA struck_cnt;	// 雷击计数
	FDATA struck_total;	// 合计雷击计数
	FDATA soft_version;		// 软件版本号
	FDATA leak_alarm_threshold;		// 报警阈值
	FDATA day_time;				// 在线天数

	UINT16 systime_year;		// 在线时间_年份
	UINT16 systime_month;		// 在线时间_月份
	UINT16 systime_day;		// 在线时间_日期
	UINT16 systime_hour;		// 在线时间_时
	UINT16 systime_min;		// 在线时间_分
	UINT16 systime_sec;		// 在线时间_秒

	// 最近一次防雷发生时间
	UINT16 last_1_struck_year;
	UINT16 last_1_struck_month;
	UINT16 last_1_struck_day;
	UINT16 last_1_struck_hour;
	UINT16 last_1_struck_min;

	UINT16 life_time;			// 寿命值0%-100%

	// 最近第2次防雷发生时间
	UINT16 last_2_struck_year;
	UINT16 last_2_struck_month;
	UINT16 last_2_struck_day;
	UINT16 last_2_struck_hour;
	UINT16 last_2_struck_min;

	UINT16 reversed0;

	// 最近第3次防雷发生时间
	UINT16 last_3_struck_year;
	UINT16 last_3_struck_month;
	UINT16 last_3_struck_day;
	UINT16 last_3_struck_hour;
	UINT16 last_3_struck_min;

	UINT16 reversed1;

	// 最近第4次防雷发生时间
	UINT16 last_4_struck_year;
	UINT16 last_4_struck_month;
	UINT16 last_4_struck_day;
	UINT16 last_4_struck_hour;
	UINT16 last_4_struck_min;

	UINT16 reversed2;

	// 最近第5次防雷发生时间
	UINT16 last_5_struck_year;
	UINT16 last_5_struck_month;
	UINT16 last_5_struck_day;
	UINT16 last_5_struck_hour;
	UINT16 last_5_struck_min;		// 57长度
}SPD_AI_PARAMS;


typedef struct spd_di_struct
{
	UINT8 SPD_DI;	// DI数据

	UINT8 bit_0;	// 对DI位的解析
	UINT8 bit_1;	// 对DI位的解析
	UINT8 bit_2;	// 对DI位的解析
	UINT8 C1_status;		// bit3,C1遥信
	UINT8 grd_alarm;		// bit4,接地告警
	UINT8 bit_5;	// 对DI位的解析
	UINT8 leak_alarm;		// bit6,漏电流告警
	UINT8 volt_alarm;		// bit7,市电电压告警
}SPD_DI_PARAMS;

typedef struct spd_do_struct
{
	UINT8 SPD_DO;	// 漏电流
}SPD_DO_PARAMS;


typedef struct spd_res_st_struct
{
	UINT16 alarm;			// 接地报警 0x0c
	UINT16 grd_res_value;	// 协议取得的接地电阻值 0x0d
	UINT16 grd_res_dot_num;	// 接地电阻小数位 0x0e
	UINT16 grd_volt;		// 电压值 0x0f

	UINT16 reserved0;		// 0x10
	UINT16 test;			// 测试	// 0x11
	UINT16 id;				// 更改id地址	// 0x12
	UINT16 alarm_value;		// 报警值修改	// 0x13

	float grd_res_real;		// 真实的接地电阻值	//不在协议内计算的值
}SPD_RES_ST_PARAMS;

#if 0
typedef struct spd_res_ctrl_struct
{
	UINT16 test;			// 测试
	UINT16 id;				// 更改id地址
	UINT16 alarm_value;		// 报警值修改
}SPD_RES_CTRL_PARAMS;
#endif

typedef struct spd_modbus_array_struct
{
   //UINT8	addr;	// 设备地址
   UINT8    func_code;
   UINT16 	reg_addr;
   UINT16 	reg_num;
   //void   (*Function)(PDUResponse_Struct *Response);
}SPD_FunctionArray_Struct;


/*防雷读写类型定义*/
typedef enum
{
	SPD_AI_DATA = 0,
	SPD_DI_DATA,
	SPD_DO_DATA,

	SPD_RES_DATA,	// 接地电阻部分

	SPD_DATA_NUM
}SPD_DATA_LIST;

/*电源 控制定义*/
typedef enum
{
	SPD_AI_SET = 0,		// 模拟量设置
	SPD_DO_SET,			// DO控制

	SPD_RES_SET,		// 接地电阻设置

	SPD_CTRL_NUM		//3
}SPD_CTRL_LIST;


// 防雷器结构体
typedef struct spd_struct
{
	SPD_AI_PARAMS dSPD_AIdata;
	SPD_DI_PARAMS dSPD_DI;
	SPD_DO_PARAMS dSPD_DO;
	SPD_RES_ST_PARAMS dSPD_res;
}SPD_PARAMS;


// 防雷器结构体
typedef struct spd_ctrl_value_struct
{
	UINT8 ref_addr;		// 要控制的寄存器地址
	FDATA f_ai_set;		// 要控制的值,根据地址不同而不同
	UINT16 do_set;		//
	UINT16 res_set;		//
}SPD_CTRL_VALUE;


extern SPD_CTRL_VALUE SPD_ctrl_value;
extern UINT8 SPD_Address;
extern UINT8 SPD_Res_Address;


void init_net_spd(void);
//void send_SPD(char command,bool ReSend,char state,int num);
int spd_send_process(UINT16 *pnet_flag, SPD_DATA_LIST SPD_data_event);
int spd_ctrl_process(UINT16 *pctrl_flag, SPD_CTRL_LIST SPD_ctrl_event);
int obtain_net_psd(void);
void DealSPDAiMsg(unsigned char *buf,unsigned short int len);
void DealSPDDiMsg(unsigned char *buf,unsigned short int len);
int DealNetSPD(unsigned char *buf,unsigned short int len);
int Ex_SPD_Set_Process(SPD_CTRL_LIST SPD_ctrl_event, UINT8 set_addr, FDATA ai_data,UINT16 data);


#endif


