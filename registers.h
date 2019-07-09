/**************************Copyright (c)****************************************************
 * 								
 * 						
 * 	
 * ------------------------文件信息---------------------------------------------------
 * 文件名:
 * 版	本:
 * 描	述: modbus协议的寄存器处理宏定义，用户接口
 * 
 * --------------------------------------------------------------------------------------- 
 * 作	者: 
 * 日	期: 
 * 版	本:
 * 描	述:
 ****************************************************************************************/

#ifndef	__REGISTERS_H
#define	__REGISTERS_H

typedef signed char       	INT8;
typedef unsigned char       UINT8;
typedef signed short        INT16;
typedef unsigned short      UINT16;
typedef signed int      	INT32;
typedef unsigned int      	UINT32;

/*寄存器属性定义*/
#define READONLY						0		/*只读寄存器*/
#define WRITEONLY					1		/*只写寄存器*/
#define WRITEREAD					2		/*R/W寄存器*/

/*操作寄存器的错误码*/
#define REG_OK						0x00		/*正确*/
#define FUNCCODE_ERR					0x01		/*不支持的功能码*/
#define REGADDR_ERR					0x02		/*寄存器地址非法*/
#define DATA_ERR						0x03		/*数据错误*/
#define OPERATION_ERR				0x04		/*无效操作*/


/*SPARK01装置寄存器的范围*/
/*实时数据寄存器*/
#define DATA_START_ADDR			0		/*数据寄存器起始地址*/
#define DATA_REG_MAX			68		/*本版本所支持的最大寄存器数*/

/*RSU天线数据寄存器*/
#define RSU_START_ADDR			0		/*RSU天线数据寄存器起始地址*/
#define RSU_REG_MAX			12		/*RSU天线数据寄存器数*/

/*UPS数据寄存器*/
#define UPS_START_ADDR			40		/*UPS数据寄存器起始地址*/
#define UPS_REG_MAX			39		/*UPS数据寄存器数*/

/*防雷器数据寄存器*/
#define SPD_START_ADDR			90		/*数据寄存器起始地址*/
#define SPD_REG_MAX			3		/*最大寄存器数*/

/*环境数据寄存器*/
#define ENVI_START_ADDR			300		/*环境数据寄存器起始地址*/
#define ENVI_REG_MAX			15		/*环境数据寄存器数量，暂定为11*/

/*装置信息寄存器*/
#define DEVICEINFO_START_ADDR			900
#define DEVICEINFO_REG_MAX				23

/*装置参数寄存器*/
#define PARAMS_START_ADDR		1200		/*设备参数寄存器开始地址*/
#define PARAMS_REG_MAX			25			/*本版本所支持的最大寄存器数*/ 

/*遥控寄存器*/
#define DO_START_ADDR					1500
#define DO_REG_MAX						49
#define REMOTE_RESET_REG					1548		/*遥控复位*/

// 暂时为6路RSU
#define RSU_NUM 6

/*功能码*/
#define	READ_REGS				0x03           //读寄存器
#define	PRESET_REGS			0x10                   //写寄存器
#define	FORCE_COIL				0x05           //设置继电器输出状态

//遥控寄存器定义
#define RSU1_PRECLOSE	1500		//RSU天线1遥合预置
#define RSU1_CLOSE		1501		//RSU天线1遥合执行
#define RSU1_PREOPEN	1502		//RSU天线1遥分预置
#define RSU1_OPEN		1503		//RSU天线1遥分执行
#define RSU2_PRECLOSE	1504		//RSU天线2遥合预置
#define RSU2_CLOSE		1505		//RSU天线2遥合执行
#define RSU2_PREOPEN	1506		//RSU天线2遥分预置
#define RSU2_OPEN		1507		//RSU天线2遥分执行
#define RSU3_PRECLOSE	1508		//RSU天线3遥合预置
#define RSU3_CLOSE		1509		//RSU天线3遥合执行
#define RSU3_PREOPEN	1510		//RSU天线3遥分预置
#define RSU3_OPEN		1511		//RSU天线3遥分执行
#define RSU4_PRECLOSE	1512		//RSU天线4遥合预置
#define RSU4_CLOSE		1513		//RSU天线4遥合执行
#define RSU4_PREOPEN	1514		//RSU天线4遥分预置
#define RSU4_OPEN		1515		//RSU天线4遥分执行
#define RSU5_PRECLOSE	1516		//RSU天线5遥合预置
#define RSU5_CLOSE		1517		//RSU天线5遥合执行
#define RSU5_PREOPEN	1518		//RSU天线5遥分预置
#define RSU5_OPEN		1519		//RSU天线5遥分执行
#define RSU6_PRECLOSE	1520		//RSU天线6遥合预置
#define RSU6_CLOSE		1521		//RSU天线6遥合执行
#define RSU6_PREOPEN	1522		//RSU天线6遥分预置
#define RSU6_OPEN		1523		//RSU天线6遥分执行

#define SYSRESET		1548		//系统重启
#define DOOR_UNLOCK		1549 		//电子门锁 开
#define DOOR_LOCK		1550		//电子门锁 锁

typedef enum BAUDRATE
{
    BAUDRATE_1200      = 0,			//1200
    BAUDRATE_2400		=1,
	BAUDRATE_4800		=2,
	BAUDRATE_9600		=3,
	BAUDRATE_19200		=4,
	BAUDRATE_115200		=5
}BAUDRATE;

/*使能定义*/
#define	WRITE_ENABLE		0xFF00           //使能
#define	WRITE_DISABLE		0x0000           //不使能

/*寄存器对象定义*/
typedef struct __Register__
{
	UINT16 *pAddr;				/*寄存器地址*/
	UINT8 bAtrrib;				/*寄存器属性定义RO,WO,RW*/
	UINT16 nLenth;				/*寄存器长度*/
}Reg_Object;

/*modubs寄存器列表列表定义*/
typedef struct __Modbus_Map_Reg
{
	UINT16 nRegNo;				/*寄存器号*/
	Reg_Object reg;				/*寄存器号所对应的寄存器*/
} Map_Reg_Table;


// 环境数据结构体
typedef struct envi_struct
{
	UINT16 temp;	// 当前环境温度值300 ×10
	UINT16 moist;	// 当前环境湿度值301 ×10
	UINT16 water_flag;	// 漏水302
	UINT16 front_door_flag;			//前柜门开关状态303
	UINT16 back_door_flag;		//后柜门开关状态304
	UINT16 door_overtime;		//柜门开启超时305
	UINT16 smoke_event_flag;	//烟雾报警306
	UINT16 Reserve1;			//保留1 307
	UINT16 Reserve2; 			//保留2 308
	UINT16 Reserve3;			//保留1 309
	UINT16 air_cond_status;		//空调状态310
	UINT16 air_cond_temp_out;		//当前空调室外温度值311 ×10
	UINT16 air_cond_temp_in;		//当前空调室内温度值312 ×10
	UINT16 air_cond_hightemp_alarm;			//空调高温告警313
	UINT16 air_cond_lowtemp_alarm;			//空调低温告警314
}ENVI_PARAMS;


//每路RSU天线数据结构体
typedef struct phase_struct
{
	UINT16 vln;		//RSU天线电压
	UINT16 amp;		//RSU天线电流
	UINT16 enable;	// 投切标志
}RSU_PHASE_PARAMS;


//RSU天线数据结构体
typedef struct rsu_struct
{
	RSU_PHASE_PARAMS phase[RSU_NUM];		// 暂时为6路RSU
}RSU_PARAMS;


// USP子结构体--输入
typedef struct ups_in_struct
{
	UINT16 phase_num;		// 相数 40
	UINT16 freq;			// 交流输入频率	41
	UINT16 volt_Ain;		//交流输入相电压A 42
	UINT16 volt_Bin;		//交流输入相电压B 43
	UINT16 volt_Cin;		//交流输入相电压C 44

	UINT16 amp_Ain;			//交流输入相电流A      45
	UINT16 amp_Bin;			//交流输入相电流B      46
	UINT16 amp_Cin;			//交流输入相电流C      47
/*	UINT16 fact_Ain;		// 功率因素
	UINT16 fact_Bin;		// 功率因素
	UINT16 fact_Cin;		// 功率因素

	UINT16 bypass_voltA;	// 旁路电压A
	UINT16 bypass_voltB;	// 旁路电压B
	UINT16 bypass_voltC;	// 旁路电压C
	UINT16 bypass_freq;	// 旁路频率*/
}UPS_IN_PARAMS;


// USP子结构体--输出
typedef struct ups_out_struct
{
	UINT16 phase_num;		// 输出相数 48
	UINT16 freq;			//UPS交流输出频率 49
	UINT16 volt_Aout;		//交流输出相电压A 50
	UINT16 volt_Bout;		//交流输出相电压B 51
	UINT16 volt_Cout;		//交流输出相电压C 52
	UINT16 amp_Aout;		//交流输出相电流A 53
	UINT16 amp_Bout;		//交流输出相电流B 54
	UINT16 amp_Cout;		//交流输出相电流C 55

/*	UINT16 fact_Aout;
	UINT16 fact_Bout;
	UINT16 fact_Cout;
	UINT16 kw_Aout;		// 有功
	UINT16 kw_Bout;		// 有功
	UINT16 kw_Cout;		// 有功
	UINT16 kva_Aout;		// 视在
	UINT16 kva_Bout;
	UINT16 kva_Cout;	

	UINT16 load_Aout;		// 负载
	UINT16 load_Bout;		// 负载
	UINT16 load_Cout;		// 负载*/
}UPS_OUT_PARAMS;

// USP子结构体--电池
typedef struct ups_bat_struct
{
	UINT16 running_day;			// UPS运行时间 56
	UINT16 battery_volt;		//UPS电池电压	57
	UINT16 amp_charge;			//UPS充电电流 58
//	UINT16 amp_discharge;		//UPS放电电流
	UINT16 battery_left;		//UPS电池后备时间	59
	UINT16 battery_tmp;			// 环境温度 60
	UINT16 battery_capacity;	//电池当前容量 61
	UINT16 battery_dischg_times;//电池放电次数 62
}UPS_BAT_PARAMS;


// USP子结构体--状态
typedef struct ups_status_struct
{
	// 功能码43	
	UINT16 supply_out_status;		// 输出供电状态 63
	UINT16 supply_in_status;		// 输入供电状态 64
	UINT16 battery_status;			// 电池状态 65
}UPS_STATUS_PARAMS;

// USP子结构体--告警
typedef struct ups_alarm_struct
{
	// 功能码44	
	UINT16 main_abnormal;			// 主路异常 66
	UINT16 system_overtemp;			// 系统过温, 67
	UINT16 sysbat_low_prealarm;		// 系统电池电量低预告警,68
	UINT16 rectifier_overload;		// 整流器过载,69
	UINT16 inverter_overload;		// 逆变器过载,70
	UINT16 bypass_abnomal;			// 旁路异常,71
	UINT16 battery_low_prealarm;	// 电池电压低,72
	UINT16 battery_abnomal;			// 电池电压异常,73
	UINT16 battery_overtemp;		// 电池过温,74
	UINT16 fan_abnormal;			// 风扇故障,75
	UINT16 shutdown_alarm;			//紧急关机告警,76
	UINT16 maintain_status;			//维修模式,77
	UINT16 inverter_supply;			//电池逆变供电,78
	UINT16 bypass_supply;			//旁路供电,79
}UPS_ALARM_PARAMS;


// USP结构体
typedef struct ups_struct
{
	// 输入数据
	UINT16 in_phase_num;		// 相数 40
	UINT16 in_freq;			// 交流输入频率	41 ×100
	UINT16 volt_Ain;		//交流输入相电压A 42 ×10
	UINT16 volt_Bin;		//交流输入相电压B 43 ×10
	UINT16 volt_Cin;		//交流输入相电压C 44 ×10

	UINT16 amp_Ain;			//交流输入相电流A      45 ×10
	UINT16 amp_Bin;			//交流输入相电流B      46 ×10
	UINT16 amp_Cin;			//交流输入相电流C      47 ×10

	//输出数据
	UINT16 out_phase_num;		// 输出相数 48
	UINT16 out_freq;			//UPS交流输出频率 49      ×100
	UINT16 volt_Aout;		//交流输出相电压A 50 ×10
	UINT16 volt_Bout;		//交流输出相电压B 51 ×10
	UINT16 volt_Cout;		//交流输出相电压C 52      ×10
	UINT16 amp_Aout;		//交流输出相电流A 53 ×10
	UINT16 amp_Bout;		//交流输出相电流B 54 ×10
	UINT16 amp_Cout;		//交流输出相电流C 55 ×10

	//电池参数
	UINT16 running_day; 		// UPS运行时间 56 天
	UINT16 battery_volt;		//UPS电池电压	57 ×10
	UINT16 amp_charge;			//UPS充电电流 58 ×100
	UINT16 battery_left;		//UPS电池后备时间 59 ×10，分钟
	UINT16 battery_tmp; 		// 环境温度 60 ×10
	UINT16 battery_capacity;	//电池当前容量 61 ×100%
	UINT16 battery_dischg_times;//电池放电次数 62

	//USP状态参数
	UINT16 supply_out_status;		// 输出供电状态 63
	UINT16 supply_in_status;		// 输入供电状态 64
	UINT16 battery_status;			// 电池状态 65

	//USP告警
	UINT16 main_abnormal;			// 主路异常 66 0x00：正常 0xF0：异常
	UINT16 system_overtemp;			// 系统过温, 67 0x00：正常 0xF0：异常
	UINT16 sysbat_low_prealarm;		// 系统电池电量低预告警,68 0x00：正常 0xF0：异常
	UINT16 rectifier_overload;		// 整流器过载,69 0x00：正常 0xF0：异常
	UINT16 inverter_overload;		// 逆变器过载,70 0x00：正常 0xF0：异常
	UINT16 bypass_abnomal;			// 旁路异常,71 0x00：正常 0xF0：异常
	UINT16 battery_low_prealarm;	// 电池电压低,72 0x00：正常 0xF0：异常
	UINT16 battery_abnomal;			// 电池电压异常,73 0x00：正常 0xF0：异常
	UINT16 battery_overtemp;		// 电池过温,74 0x00：正常 0xF0：异常
	UINT16 fan_abnormal;			// 风扇故障,75 0x00：正常 0xF0：异常
	UINT16 shutdown_alarm;			//紧急关机告警,76 0x00：正常 0xF0：异常
	UINT16 maintain_status;			//维修模式,77 0x00：正常 0xF0：异常
	UINT16 inverter_supply;			//电池逆变供电,78 0x00：正常 0xF0：异常
	UINT16 bypass_supply;			//旁路供电,79 0x00：正常 0xF0：异常
}UPS_PARAMS;

// 防雷器结构体
typedef struct spd_struct
{
	UINT16 status;	// 防雷器在线状态	90     0：正常 1：异常
	UINT16 DI_status;	// 防雷器DI输入状态	91
	UINT16 struck_times;	// 雷击次数	92 0~65535
}SPD_PARAMS;

//装置参数寄存器
typedef struct device_params_struct	/*共384个字节*/
{
	UINT16 Address;		// 终端设备地址设置 1200
	UINT16 BaudRate_1;	// 串口1波特率 1201
	UINT16 BaudRate_2;	// 串口2波特率 1202
	UINT16 BaudRate_3;	// 串口3波特率 1203
	UINT16 BaudRate_4;	// 串口4波特率 1204

	UINT16 Pre_Remote;		/*RSU天线遥控预置，0：退出，1：投入*/// 1205
	UINT16 AutoSend;		/*自动上传是否投入*///1206
	UINT16 Reserve[13];		//保留 1207~1219

	UINT16 AirCondSet;		/*空调开机关机*///1220					1
	UINT16 AirColdStartPoint;		/*空调制冷点*///1221				50
	UINT16 AirColdLoop;		/*空调制冷回差*///1222					10
	UINT16 AirHotStartPoint;		/*空调制热点*///1223				15
	UINT16 AirHotLoop;		/*空调制热回差*///1224					10
}DEVICE_PARAMS;


//采集器设备信息结构体
typedef  struct  _DeviceInfo_Struct      /*用于整块写命令时边界判断，例如防止32BIT数只写了16BIT*/
{
   UINT16 deviceType[20];		//装置型号900~919
   UINT16 softVersion;			//主程序版本号920
   UINT16 protocolVersion;		//规约版本921
   UINT16 softYear;				//年922
   UINT16 softMonth;			//月923
   UINT16 softDate;				//日924
}DeviceInfoParams;

//遥控寄存器
typedef struct Remote_Control_struct	//
{
	UINT16 RSU1_PreClose;		//RSU天线1遥合预置 1500
	UINT16 RSU1_Close;			//RSU天线1遥合执行 1501
	UINT16 RSU1_PreOpen;		//RSU天线1遥分预置 1502
	UINT16 RSU1_Open;			//RSU天线1遥分执行 1503
	UINT16 RSU2_PreClose;		//RSU天线2遥合预置 1504
	UINT16 RSU2_Close;			//RSU天线2遥合执行 1505
	UINT16 RSU2_PreOpen;		//RSU天线2遥分预置 1506
	UINT16 RSU2_Open;			//RSU天线2遥分执行 1507
	UINT16 RSU3_PreClose;		//RSU天线3遥合预置 1508
	UINT16 RSU3_Close;			//RSU天线3遥合执行 1509
	UINT16 RSU3_PreOpen;		//RSU天线3遥分预置 1510
	UINT16 RSU3_Open;			//RSU天线3遥分执行 1511
	UINT16 RSU4_PreClose;		//RSU天线4遥合预置 1512
	UINT16 RSU4_Close;			//RSU天线4遥合执行 1513
	UINT16 RSU4_PreOpen;		//RSU天线4遥分预置 1514
	UINT16 RSU4_Open;			//RSU天线4遥分执行 1515
	UINT16 RSU5_PreClose;		//RSU天线5遥合预置 1516
	UINT16 RSU5_Close;			//RSU天线5遥合执行 1517
	UINT16 RSU5_PreOpen;		//RSU天线5遥分预置 1518
	UINT16 RSU5_Open;			//RSU天线5遥分执行 1519
	UINT16 RSU6_PreClose;		//RSU天线6遥合预置 1520
	UINT16 RSU6_Close;			//RSU天线6遥合执行 1521
	UINT16 RSU6_PreOpen;		//RSU天线6遥分预置 1522
	UINT16 RSU6_Open;			//RSU天线6遥分执行 1523

	UINT16 SysReset;			//系统重启 1548
	UINT16 Door_UnLock;			//电子门锁开
	UINT16 Door_Lock;			//电子门锁关
}REMOTE_CONTROL;


// ETC 门架自由流运行状态结构体
typedef struct FlagRunStatus_struct
{
	INT16 RecordNo;		//1 记录号
	INT16 FlagNetRoadID;	//2 ETC 门架路网编号
	INT16 FlagRoadID;		//3 ETC 门架路段编号
	INT32 FlagID;			//4 ETC 门架编号
	INT16 PosId;			//5 ETC 门架序号
	INT8 Direction;		//6 行车方向
	char DirDescription[50];//7 行车方向说明
	char CatchTime[50];		//8 状态时间
	INT8 Computer;			//9 工控机状态
	INT32 DiskCapacity;	//10 硬盘容量
	INT32 DiskUsage;		//11 硬盘使用率
	INT8 PowerType;		//12 供电类型
	INT8 BackUpPower;		//13 备用电源状态
	INT16 BatteryRemain;	//14 蓄电池电量
	INT16 DBState;			//15 数据库状态
	INT8 CableNetState;	//16 有线网络状态
	INT8 WireLessState;	//17 无线网络状态
	INT8 SoftWare;			//18 ETC 门架软件状态
	char SoftVersion[30];	//19 软件版本
	
	INT16 CamerCount;		//20 车牌识别设备数量
	INT8 Vehplate1; 		//21 车牌设别1
	INT8 Vehplate2; 		//22 车牌设别2
	INT8 Vehplate3; 		//23 车牌设别3
	INT8 Vehplate4; 		//24 车牌设别4
	INT8 Vehplate5; 		//25 车牌设别5
	INT8 Vehplate6; 		//26 车牌设别6
	INT8 Vehplate7; 		//27 车牌设别7
	INT8 Vehplate8; 		//28 车牌设别8
	INT8 Vehplate9; 		//29 车牌设别9
	INT8 Vehplate10; 		//30 车牌设别10
	INT8 Vehplate11; 		//31 车牌设别11
	INT8 Vehplate12; 		//32 车牌设别12
	INT8 Vehplate13; 		//33 车牌设别13
	INT8 Vehplate14; 		//34 车牌设别14
	INT8 Vehplate15; 		//35 车牌设别15
	INT8 Vehplate16;  		//36 车牌设别16
	
	INT16 RSUCount;		//37 RSU数量 
	
	INT8 RSU1; 			//38 天线1 
	INT16 RSU1_Power; 		//39 天线1功率
	INT8 RSU1_Channel; 	//40 天线1信道号
	INT8 RSU1_Switch; 		//41 天线1开关状态
	INT32 RSU1_ErrInfo; 	//42 天线1异常信息
	INT8 RSU2; 			//43 天线2 
	INT16 RSU2_Power; 		//44 天线2功率
	INT8 RSU2_Channel; 	//45 天线2信道号
	INT8 RSU2_Switch; 		//46 天线2开关状态
	INT32 RSU2_ErrInfo; 	//47 天线2异常信息
	INT8 RSU3; 			//48 天线3
	INT16 RSU3_Power; 		//49 天线3功率
	INT8 RSU3_Channel;		//50 天线3信道号
	INT8 RSU3_Switch; 		//51 天线3开关状态
	INT32 RSU3_ErrInfo; 	//52 天线3异常信息
	INT8 RSU4; 			//53 天线4
	INT16 RSU4_Power; 		//54 天线4天线功率
	INT8 RSU4_Channel; 	//55 天线4信道号
	INT8 RSU4_Switch; 		//56 天线4开关状态
	INT32 RSU4_ErrInfo; 	//57 天线4异常信息
	INT8 RSU5; 			//58 天线5
	INT16 RSU5_Power; 		//59
	INT8 RSU5_Channel; 	//60 
	INT8 RSU5_Switch; 		//61 
	INT32 RSU5_ErrInfo; 	//62 
	INT8 RSU6; 			//63 天线6
	INT16 RSU6_Power; 		//64 
	INT8 RSU6_Channel; 	//65 
	INT8 RSU6_Switch; 		//66
	INT32 RSU6_ErrInfo; 	//67
	INT8 RSU7; 			//68 天线7
	INT16 RSU7_Power; 		//69 
	INT8 RSU7_Channel; 	//70 
	INT8 RSU7_Switch; 		//71
	INT32 RSU7_ErrInfo; 	//72 
	INT8 RSU8; 			//73 天线8
	INT16 RSU8_Power; 		//74 
	INT8 RSU8_Channel; 	//75 
	INT8 RSU8_Switch; 		//76 
	INT32 RSU8_ErrInfo; 	//77 
	INT8 RSU9; 			//78 天线9
	INT16 RSU9_Power; 		//79 
	INT8 RSU9_Channel; 	//80 
	INT8 RSU9_Switch; 		//81
	INT32 RSU9_ErrInfo; 	//82 
	INT8 RSU10; 			//83 天线10
	INT16 RSU10_Power; 	//84 
	INT8 RSU10_Channel; 	//85 
	INT8 RSU10_Switch; 	//86 
	INT32 RSU10_ErrInfo; 	//87 
	INT8 RSU11; 			//88 天线11
	INT16 RSU11_Power; 	//89 
	INT8 RSU11_Channel; 	//90 
	INT8 RSU11_Switch; 	//91 
	INT32 RSU11_ErrInfo; 	//92 
	INT8 RSU12; 			//93 天线12
	INT16 RSU12_Power; 	//94 
	INT8 RSU12_Channel; 	//95 
	INT8 RSU12_Switch; 	//96 
	INT32 RSU12_ErrInfo;	//97 
	INT8 RSU13; 			//98 天线13
	INT16 RSU13_Power; 	//99 
	INT8 RSU13_Channel; 	//100 
	INT8 RSU13_Switch; 	//101 
	INT32 RSU13_ErrInfo; 	//102 
	INT8 RSU14; 			//103 天线14
	INT16 RSU14_Power; 	//104 
	INT8 RSU14_Channel; 	//105 
	INT8 RSU14_Switch; 	//106 
	INT32 RSU14_ErrInfo;	//107 
	INT8 RSU15; 			//108 天线15
	INT16 RSU15_Power ;	//109 
	INT8 RSU15_Channel; 	//110
	INT8 RSU15_Switch; 	//111
	INT32 RSU15_ErrInfo; 	//112 
	INT8 RSU16; 			//113 天线16
	INT16 RSU16_Power; 	//114 
	INT8 RSU16_Channel; 	//115 
	INT8 RSU16_Switch; 	//116 
	INT32 RSU16_ErrInfo; 	//117 
	
	char BackUp1[50]; 		//118 备用1
	char BackUp2[50]; 		//119 备用2
	char BackUp3[50]; 		//120 备用3
	char BackUp4[50]; 		//121 备用4
	char TimeFlag[50];  	//122 时间戳标识

	//状态信息
	INT16 temp;	// 当前环境温度值123
	INT16 moist;	// 当前环境湿度值124
	INT16 online_flag;		//温湿度是否在线125
	char BackUp5[50]; 		// 备用 126
	char BackUp6[50]; 		// 备用 127
	char BackUp7[50]; 		// 备用 128
	INT16 air_cond_status;	//空调整机状态129
	INT16 air_cond_fan_in_status;	//空调内风机状态130
	INT16 air_cond_fan_out_status;	//空调外风机状态131
	INT16 air_cond_comp_status;	//空调压缩机状态132
	INT16 air_cond_heater_status;	//空调电加热状态133
	INT16 air_cond_fan_emgy_status;	//空调应急风机状态134
	INT16 air_cond_temp_in;	//当前空调室内温度值135
	INT16 air_cond_temp_out;	//当前空调室外温度值	136
	INT16 air_cond_amp;	//当前空调室电流值137
	INT16 air_cond_volt;	//当前空调室电压值138
	INT16 in_phase_num;	//UPS输入相数139
	INT16 in_freq;	//UPS交流输入频率140
	INT16 volt_Ain;	//交流输入相电压A`141
	INT16 volt_Bin;	//交流输入相电压B 142
	INT16 volt_Cin;	//交流输入相电压C143
	INT16 amp_Ain;	//交流输入相电流A144
	INT16 amp_Bin;	//交流输入相电流B145
	INT16 amp_Cin;	//交流输入相电流C146
	INT16 fact_Ain;	//功率因素A147
	INT16 fact_Bin;	//功率因素B148
	INT16 fact_Cin;	//功率因素C149
	INT16 bypass_voltA;	//旁路电压A150
	INT16 bypass_voltB;	//旁路电压B151
	INT16 bypass_voltC;	//旁路电压C152
	INT16 bypass_freq;	//旁路频率153
	INT16 out_phase_num;	//UPS输出相数154
	INT16 out_freq;	//UPS交流输出频率155
	INT16 volt_Aout;	//交流输出相电压A156
	INT16 volt_Bout;	//交流输出相电压B157
	INT16 volt_Cout;	//交流输出相电压C158
	INT16 amp_Aout;	//交流输出相电流A159
	INT16 amp_Bout;	//交流输出相电流B169
	INT16 amp_Cout;	//交流输出相电流C161
	INT16 kw_Aout;	//UPS A相输出有功功率162
	INT16 kw_Bout;	//UPS B相输出有功功率	163
	INT16 kw_Cout;	//UPS C相输出有功功率164
	INT16 kva_Aout;	//UPS A相输出视在功率165
	INT16 kva_Bout;	//UPS B相输出视在功率166
	INT16 kva_Cout;	//UPS C相输出视在功率167
	INT16 load_Aout;	//UPS A相输出负载率168
	INT16 load_Bout;	//UPS B相输出负载率169
	INT16 load_Cout;	//UPS C相输出负载率170
	INT16 running_day;	//UPS运行时间171
	INT16 battery_volt;	//UPS电池电压172
	INT16 amp_charge;	//UPS充电电流173
	INT16 amp_discharge;	//UPS放电电流174
	INT16 battery_left;	//UPS电池后备时间175
	INT16 battery_tmp;	//环境温度176
	INT16 battery_capacity;	//电池当前容量177
	INT16 battery_dischg_times;	//电池放电次数178
	INT16 status;	//防雷器在线状态179
	INT16 struck_times;	//雷击次数180

	//告警信息
	INT16 water_flag;	//	漏水181
	INT16 front_door_flag;	//	前柜门开关状态182
	INT16 back_door_flag;	//	后柜门开关状态183
	INT16 door_overtime;	//	柜门开启超时184
	INT16 smoke_event_flag;	//	烟雾报警185
	INT16 air_cond_hightemp_alarm;	//	空调高温告警186
	INT16 air_cond_lowtemp_alarm;	//	空调低温告警187
	INT16 air_cond_highmoist_alarm;	//	空调高湿告警188
	INT16 air_cond_lowmoist_alarm;	//	空调低湿告警189
	INT16 air_cond_infan_alarm;	//	空调内风机故障190
	INT16 air_cond_outfan_alarm;	//	空调外风机故障191
	INT16 air_cond_comp_alarm;	//	空调压缩机故障192
	INT16 air_cond_heater_alarm;	//	空调电加热故障193
	INT16 air_cond_emgyfan_alarm;	//	空调应急风机故障194
	INT16 supply_out_status;	//	输出供电状态195
	INT16 supply_in_status;	//	输入供电状态196
	INT16 battery_status;	//	电池状态197
	INT16 main_abnormal;	//	主路异常198
	INT16 system_overtemp;	//	系统过温199
	INT16 sysbat_low_prealarm;	//	系统电池电量低预告警200
	INT16 rectifier_overload;	//	整流器过载201
	INT16 inverter_overload;	//	逆变器过载202
	INT16 bypass_abnomal;	//	旁路异常203
	INT16 battery_low_prealarm	;	//电池电压低204
	INT16 battery_abnomal;	//	电池电压异常205
	INT16 battery_overtemp;	//	电池过温206
	INT16 fan_abnormal;	//	风扇故障207
	INT16 shutdown_alarm;	//	紧急关机告警208
	INT16 maintain_status;	//	维修模式209
	INT16 inverter_supply;	//	电池逆变供电210
	INT16 bypass_supply;	//	旁路供电211
}FLAGRUNSTATUS;


/*寄存器操作*/
UINT8 Read_Register(UINT16 nStartRegNo, UINT16 nRegNum, UINT8 *pdatabuf, UINT8 *perr);
UINT8 Write_Register(UINT16 nStartRegNo, INT16 nRegNum, const UINT8 *pdatabuf, UINT8 datalen, UINT8 *perr);

extern UINT16  System_Reset;

#endif



