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

/*机柜类型定义*/
#define HUAWEI						1		
#define LTKJ						2

/*http 请求类型*/
#define HTTPGET						1		
#define HTTPPOST					2

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

/*电源控制器地址*/
#define POWER_CTRL_ADDR			71		/*电源控制器地址*/

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
#define ENVI_REG_MAX			29		/*环境数据寄存器数量，暂定为11*/

/*装置信息寄存器*/
#define DEVICEINFO_START_ADDR			900
#define DEVICEINFO_REG_MAX				23

/*装置参数寄存器*/
#define PARAMS_START_ADDR		1200		/*设备参数寄存器开始地址*/
#define PARAMS_REG_MAX			5			/*本版本所支持的最大寄存器数*/ 

/*空调参数寄存器*/
#define AIRCOND_START_ADDR		1220		/*设备参数寄存器开始地址*/
#define AIRCOND_REG_MAX			5			/*本版本所支持的最大寄存器数*/ 

/*遥控寄存器*/
#define DO_START_ADDR					1500
#define DO_REG_MAX						49
#define REMOTE_RESET_REG					1548		/*遥控复位*/

//预留12路车牌识别
#define VEHPLATE_NUM 12
//预留2路RSU控制器
#define RSUCTL_NUM 2
//预留12路天线
#define ANTENNA_NUM 12
//预留2台交换机
#define IPSWITCH_NUM 2
//预留2台防火墙
#define FIREWARE_NUM 2
//预留4路CAM
#define CAM_NUM 4
//预留2路ATLAS
#define ATLAS_NUM 2

// 最大支持6个伏安表, 每个伏安表为6组电流电压值
#define VA_METER_BD_NUM		6
#define VA_PHASE_NUM 6
//最大支持4路电子门锁
#define LOCK_NUM			4
//最大支持3层电源板
#define POWER_BD_NUM			3
//最大支持36路开关数量
#define SWITCH_COUNT	36			

/*功能码*/
#define	READ_REGS				0x03           //读寄存器
#define	PRESET_REGS			0x10                   //写寄存器
#define	FORCE_COIL				0x05           //设置继电器输出状态

//遥控寄存器定义 老控制板
#define RSU1_REG			1500		//RSU天线1 0xFF00: 遥合;0xFF01: 遥分
#define DOOR_DO_REG			1501 		//电子门锁 0xFF00: 关锁;0xFF01: 开锁
#define AUTORECLOSURE_REG	1502		//自动重合闸0xFF00: 遥合;0xFF01: 遥分
//#define VPLATE1_REG			1503		//车牌识别1 0xFF00: 遥合;0xFF01: 遥分

//遥控寄存器定义 新12DO控制板
#define VPLATE1_REG			1500		//车牌识别1 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE2_REG			1501		//车牌识别2 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE3_REG			1502		//车牌识别3 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE4_REG			1503		//车牌识别4 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE5_REG			1504		//车牌识别5 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE6_REG			1505		//车牌识别6 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE7_REG			1506		//车牌识别7 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE8_REG			1507		//车牌识别8 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE9_REG			1508		//车牌识别9 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE10_REG			1509		//车牌识别10 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE11_REG			1510		//车牌识别11 0xFF00: 遥合; 0xFF01: 遥分
#define VPLATE12_REG			1511		//车牌识别12 0xFF00: 遥合; 0xFF01: 遥分

#define AIRCONDSET_REG			1220		//空调开关机寄存器1220
#define AIRCOLDSTARTPOINT_REG 	1221		//空调制冷点//1221 			
#define AIRCOLDLOOP_REG			1222		//空调制冷回差//1222		
#define AIRHOTSTARTPOINT_REG 	1223		//空调制热点//1223 
#define AIRHOTLOOP_REG			1224		//空调制热回差//1224	

#define SYSRESET		1		//系统重启

//遥控开关定义
#define	ACT_HOLD		0           //保持状态
#define	ACT_CLOSE		1           //分闸
#define	ACT_OPEN		2           //合闸
#define	ACT_SOFTWARERST 3           //软重启
#define	ACT_HARDWARERST 4           //断电重启
#define	ACT_HOLD_FF		255           //保持状态

/*使能定义*/
#define	SWITCH_ON		0xFF00           //合闸
#define	SWITCH_OFF		0xFF01           //分闸

//门锁开关定义
#define	ACT_LOCK		1           //关锁
#define	ACT_UNLOCK		2           //开锁
#pragma pack(push, 1)


typedef enum BAUDRATE
{
    BAUDRATE_1200      = 0,			//1200
    BAUDRATE_2400		=1,
	BAUDRATE_4800		=2,
	BAUDRATE_9600		=3,
	BAUDRATE_19200		=4,
	BAUDRATE_115200		=5
}BAUDRATE;

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
	INT16 temp;	// 当前环境温度值300 ×10
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
	UINT16 air_cond_fan_in_status;			//空调内风机状态311
	UINT16 air_cond_fan_out_status;			//空调外风机状态312
	UINT16 air_cond_comp_status;			//空调压缩机状态313
	UINT16 air_cond_heater_status;			//空调电加热状态314
	UINT16 air_cond_fan_emgy_status;		//空调应急风机状态315
	UINT16 air_cond_temp_out;		//当前空调室外温度值316 ×10
	UINT16 air_cond_temp_in;		//当前空调室内温度值317 ×10
	UINT16 air_cond_amp;					//当前空调电流值318 ×1000
	UINT16 air_cond_volt;					//当前空调电压值319 ×1
	
	UINT16 air_cond_hightemp_alarm;			//空调高温告警320
	UINT16 air_cond_lowtemp_alarm;			//空调低温告警321
	UINT16 air_cond_highmoist_alarm;		//空调高湿告警322
	UINT16 air_cond_lowmoist_alarm;			//空调低湿告警323
	UINT16 air_cond_infan_alarm;			//空调内风机故障324
	UINT16 air_cond_outfan_alarm;			//空调外风机故障325
	UINT16 air_cond_comp_alarm;				//空调压缩机故障326
	UINT16 air_cond_heater_alarm;			//空调电加热故障327
	UINT16 air_cond_emgyfan_alarm;			//空调应急风机故障328
}ENVI_PARAMS;


//每路RSU天线数据结构体
typedef struct va_phase_struct
{
	UINT16 vln;		//RSU天线电压
	UINT16 amp;		//RSU天线电流
	UINT16 enable;	// 投切标志
}VA_PHASE_PARAMS;


//RSU天线数据结构体
typedef struct va_meter_struct
{
	UINT16 address;
	VA_PHASE_PARAMS phase[VA_PHASE_NUM];		// 每个伏安表6路电流电压值
}VA_METER_PARAMS;


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
	UINT16 fact_Ain;		// 功率因素
	UINT16 fact_Bin;		// 功率因素
	UINT16 fact_Cin;		// 功率因素

	UINT16 bypass_voltA;	// 旁路电压A
	UINT16 bypass_voltB;	// 旁路电压B
	UINT16 bypass_voltC;	// 旁路电压C
	UINT16 bypass_freq;	// 旁路频率
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
	UINT16 fact_Cout;*/
	UINT16 kw_Aout;		// 有功
	UINT16 kw_Bout;		// 有功
	UINT16 kw_Cout;		// 有功
	UINT16 kva_Aout;		// 视在
	UINT16 kva_Bout;		// 视在
	UINT16 kva_Cout;		// 视在

	UINT16 load_Aout;		// 负载
	UINT16 load_Bout;		// 负载
	UINT16 load_Cout;		// 负载
}UPS_OUT_PARAMS;

// USP子结构体--电池
typedef struct ups_bat_struct
{
	UINT16 running_day;			// UPS运行时间 56
	UINT16 battery_volt;		//UPS电池电压	57
	UINT16 amp_charge;			//UPS充电电流 58
	UINT16 amp_discharge;		//UPS放电电流
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

	UINT16 fact_Ain;		// 功率因素
	UINT16 fact_Bin;		// 功率因素
	UINT16 fact_Cin;		// 功率因素

	UINT16 bypass_voltA;	// 旁路电压A
	UINT16 bypass_voltB;	// 旁路电压B
	UINT16 bypass_voltC;	// 旁路电压C
	UINT16 bypass_freq;	// 旁路频率
	//输出数据
	UINT16 out_phase_num;		// 输出相数 48
	UINT16 out_freq;			//UPS交流输出频率 49      ×100
	UINT16 volt_Aout;		//交流输出相电压A 50 ×10
	UINT16 volt_Bout;		//交流输出相电压B 51 ×10
	UINT16 volt_Cout;		//交流输出相电压C 52      ×10
	UINT16 amp_Aout;		//交流输出相电流A 53 ×10
	UINT16 amp_Bout;		//交流输出相电流B 54 ×10
	UINT16 amp_Cout;		//交流输出相电流C 55 ×10

/*	UINT16 fact_Aout;
	UINT16 fact_Bout;
	UINT16 fact_Cout;*/
	UINT16 kw_Aout; 	// 有功
	UINT16 kw_Bout; 	// 有功
	UINT16 kw_Cout; 	// 有功
	UINT16 kva_Aout;		// 视在
	UINT16 kva_Bout;		// 视在
	UINT16 kva_Cout;		// 视在

	UINT16 load_Aout;		// 负载
	UINT16 load_Bout;		// 负载
	UINT16 load_Cout;		// 负载
	
	//电池参数
	UINT16 running_day; 		// UPS运行时间 56 天
	UINT16 battery_volt;		//UPS电池电压	57 ×10
	UINT16 amp_charge;			//UPS充电电流 58 ×100
	UINT16 amp_discharge;		//UPS放电电流
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
}DEVICE_PARAMS;


//控制器参数结构体
typedef struct vmctl_params_struct
{
    char CabinetType[6];            //机柜类型  1:华为；2:利通
    //门架信息
    char FlagNetRoadID[20];    //ETC 门架路网编号
    char FlagRoadID [20];      //ETC 门架路段编号
    char FlagID[30];            //ETC 门架编号
    char PosId[10];             //ETC 门架序号
    char Direction[10];         //行车方向
    char DirDescription[50];    //行车方向说明

    //参数设置
    char HWServer[20];         //华为服务器IP地址
    char HWGetPasswd[20];      //SNMP GET 密码
    char HWSetPasswd[20];      //SNMP SET 密码
    char ServerURL1[128];      //服务器1推送地址
    char ServerURL2[128];      //服务器2推送地址
    char ServerURL3[128];      //服务器3推送地址
    char ServerURL4[128];      //门锁4推送地址
    char StationURL[128];      //控制器接收地址
    
    char RSUCount[5];            //RSU数量
    char RSUIP[RSUCTL_NUM][20];            //RSU控制器IP地址
    char RSUPort[RSUCTL_NUM][6];          //RSU控制器端口
    char VehPlateCount[5];            //识别仪数量
    char VehPlateIP[VEHPLATE_NUM][30];      //识别仪IP地址(预留12路)
    char VehPlatePort[VEHPLATE_NUM][10];    //识别仪端口(预留12路)
    char VehPlateKey[VEHPLATE_NUM][20];    //识别仪用户名密码(预留12路)
    char CAMCount[5];            //RSU数量
    char CAMIP[CAM_NUM][30];            //监控摄像头IP地址
    char CAMPort[CAM_NUM][10];          //监控摄像头端口(预留4路CAM)
    char CAMKey[CAM_NUM][20];            //监控摄像头用户名密码

    char FireWareCount[5];            //防火墙数量
    char FireWareIP[FIREWARE_NUM][20];       //防火墙地址(预留2台)
    char FireWareGetPasswd[FIREWARE_NUM][20];   //防火墙get密码
    char FireWareSetPasswd[FIREWARE_NUM][20];   //防火墙set密码
    char SwitchCount[5];            //交换机数量
    char SwitchIP[IPSWITCH_NUM][20];         //交换机地址(预留2台)
    char SwitchGetPasswd[IPSWITCH_NUM][20];     //交换机get密码
    char SwitchSetPasswd[IPSWITCH_NUM][20];     //交换机set密码 
    
    char LockAddr[LOCK_NUM][4];         //门锁地址	最多4把锁
    char VameterAddr[VA_METER_BD_NUM][4];     //电能表地址 最多6个表 每层2个
    char PowerAddr[POWER_BD_NUM][4];       //电源板地址 最多3层
    char DoCount[5];						//DO数量
    char DeviceNameSeq[SWITCH_COUNT][20];	//设备名称的配置 最多36个设备
    char DoSeq[SWITCH_COUNT][4];	//设备名称与 DO映射 最多36路开关，与设备名称对应

    //硬件信息
    char deviceType[20];		//设备型号900~919
    char hardwareid[50];		//硬件ID
    char softVersion[20]; 		//主程序版本号920
    char softDate[20]; 			//版本日期
    char secSoftVersion[3][10]; 		//副版本号，支持最多3个电源板
}VMCONTROL_PARAM;

//电源控制设备的配置
typedef struct Control_CONFIG_struct	//
{
	UINT16 vehplate;	// 设备名称
	UINT16 do_seq;		// 对应的DO
}CONTROL_CONFIG;

#define SINGLE_REMOTE_NUM	12	//单板上DO数量
//遥控寄存器
typedef struct Remote_Control_struct	//
{
	UINT16 rsu1;				//1500 RSU天线1 0xFF00: 遥合;0xFF01: 遥分
	UINT16 door_do;				//1501 电子门锁 0xFF00: 关锁;0xFF01: 开锁
	UINT16 autoreclosure;		//1502 自动重合闸0xFF00: 遥合;0xFF01: 遥分
	
	UINT16 vehplate[VEHPLATE_NUM];			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
	UINT16 rsucontrlor[RSUCTL_NUM];			//rsu 0xFF00: 遥合;0xFF01: 遥分
	UINT16 antenna[ANTENNA_NUM];			//天线 0xFF00: 遥合;0xFF01: 遥分
	UINT16 fireware[FIREWARE_NUM];			//防火墙 0xFF00: 遥合;0xFF01: 遥分
	UINT16 ipswitch[IPSWITCH_NUM];			//交换机 0xFF00: 遥合;0xFF01: 遥分
	
	UINT16 doseq[SWITCH_COUNT];			//备用do 0xFF00: 遥合;0xFF01: 遥分

	UINT16 SysReset;			//系统重启 1548
	UINT16 FrontDoorCtrl;			//前门电子门锁 0：保持 1：关锁：2：开锁
	UINT16 BackDoorCtrl;			//后门电子门锁0：保持 1：关锁：2：开锁
	UINT16 SideDoorCtrl;			//侧门电子门锁0：保持 1：关锁：2：开锁
	UINT16 AutoReclosure_Close;		//自动重合闸-合闸
	UINT16 AutoReclosure_Open;		//自动重合闸-分闸
	char systemtime[50];		//设置控制器时间
	
	UINT16 aircondset;		//空调关机//1220					1
	UINT16 aircoldstartpoint;		//空调制冷点//1221				50
	UINT16 aircoldloop; 	//空调制冷回差//1222					10
	UINT16 airhotstartpoint;		//空调制热点//1223				15
	UINT16 airhotloop;		//空调制热回差//1224					10

	INT16 hwctrlmonequipreset;		//控制单板复位 0：保持；1：热复位；
	INT16 hwsetacsuppervoltlimit;	//AC过压点设置	0:保持；50-600（有效）；280（缺省值）
	INT16 hwsetacslowervoltlimit;	//AC欠压点设置	0:保持；50-600（有效）；180（缺省值）
	INT16 hwsetdcsuppervoltlimit;	//设置DC过压点	0:保持；53-600（有效）；58（缺省值）
	INT16 hwsetdcslowervoltlimit;	//设置DC欠压点	0:保持；35 - 57（有效）；45（缺省值）
	INT16 hwsetenvtempupperlimit[2];	//环境温度告警上限 0:保持；25-80（有效）；55（缺省值）
	INT16 hwsetenvtemplowerlimit[2];	//环境温度告警下限255:保持；-20-20（有效）；-20（缺省值）
	INT16 hwsetenvhumidityupperlimit[2];	//环境湿度告警上限 255:保持；0-100（有效）；95（缺省值）
	INT16 hwsetenvhumiditylowerlimit[2];	//环境湿度告警下限 255:保持；0-100（有效）；5（缺省值）
	INT16 hwcoolingdevicesmode;		//温控模式				0：保持；1：纯风扇模式；2：纯空调模式；3：智能模式；
	INT16 hwdcairpowerontemppoint[2];		//空调开机温度点 255:保持； -20-80（有效）；45(缺省值)
	INT16 hwdcairpowerofftemppoint[2];		//空调关机温度点  		  255:保持； -20-80（有效）；37(缺省值)
	INT16 hwdcairctrlmode[2];			//空调控制模式 0：保持；1：自动；2：手动
	INT16 hwctrlsmokereset[2];			//控制烟感复位 0：保持；1：不需复位；2：复位
}REMOTE_CONTROL;

//空调参数结构体
typedef struct Air_Cond_struct	//
{
	UINT16 aircondset;		//空调关机//1220					1
	UINT16 aircoldstartpoint;		//空调制冷点//1221 			50
	UINT16 aircoldloop; 	//空调制冷回差//1222					10
	UINT16 airhotstartpoint;		//空调制热点//1223 			15
	UINT16 airhotloop;		//空调制热回差//1224					10
}AIRCOND_PARAM;

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

	INT16 cpu_occupancy;	//CPU占用率212
}FLAGRUNSTATUS;

#pragma pack(pop)

/*寄存器操作*/
UINT8 Read_Register(UINT16 nStartRegNo, UINT16 nRegNum, UINT8 *pdatabuf, UINT8 *perr);
UINT8 Write_Register(UINT16 nStartRegNo, INT16 nRegNum, const UINT8 *pdatabuf, UINT8 datalen, UINT8 *perr);

extern UINT16  System_Reset;

#endif






