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
typedef unsigned long long  UINT64;

/*机柜类型定义*/
#define HUAWEI_DUALCAB_DUALDOOR		1
#define HUAWEI_DUALCAB_SINGDOOR		2
#define HUAWEI_SINGCAB_DUALDOOR		3
#define HUAWEI_SINGCAB_SINGDOOR		4
#define ZHONGXING_CAB				5
#define JINSHENGAN_CAB				6
#define AITESI_CAB					7

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

//预留9路300万车牌识别
#define VEHPLATE_NUM 12
//预留3路900万车牌识别
#define VEHPLATE900_NUM 3
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
//预留4路防雷检测器
#define SPD_NUM 4
// 预留1路网口接地电阻
#define RES_NUM 1

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
	UINT32 TimeStamp; 		//获取时间戳
	bool Linked;
}VA_METER_PARAMS;


// 电源板的数据状态
typedef struct power_stamp_struct
{
	UINT32 TimeStamp; 		//获取时间戳
	bool Linked;
}POWER_STAMP_PARAMS;


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
    char HWServer2[20];         //金晟安服务器IP地址
    char HWGetPasswd2[20];      //金晟安 SNMP GET 密码
    char HWSetPasswd2[20];      //金晟安 SNMP SET 密码
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
    char VehPlate900Count[5];            //900识别仪数量
    char VehPlate900IP[VEHPLATE900_NUM][30];      //900识别仪IP地址(预留12路)
    char VehPlate900Port[VEHPLATE900_NUM][10];    //900识别仪端口(预留12路)
    char VehPlate900Key[VEHPLATE900_NUM][20];    //900识别仪用户名密码(预留12路)
    char CAMCount[5];            //RSU数量
    char CAMIP[CAM_NUM][30];            //监控摄像头IP地址
    char CAMPort[CAM_NUM][10];          //监控摄像头端口(预留4路CAM)
    char CAMKey[CAM_NUM][20];            //监控摄像头用户名密码

    char FireWareType[5];				//防火墙类型
    char FireWareCount[5];            //防火墙数量
    char FireWareIP[FIREWARE_NUM][20];       //防火墙地址(预留2台)
    char FireWareGetPasswd[FIREWARE_NUM][20];   //防火墙get密码
    char FireWareSetPasswd[FIREWARE_NUM][20];   //防火墙set密码
    char SwitchType[5];				//交换机类型
    char SwitchCount[5];            //交换机数量
    char SwitchIP[IPSWITCH_NUM][20];         //交换机地址(预留2台)
    char SwitchGetPasswd[IPSWITCH_NUM][20];     //交换机get密码
    char SwitchSetPasswd[IPSWITCH_NUM][20];     //交换机set密码
    char AtlasCount[5];            //Atlas数量
    char AtlasIP[ATLAS_NUM][20];         //Atlas地址(预留2台)
    char AtlasPasswd[ATLAS_NUM][20];     //Atlas密码
	char SPDType[5];	//PSD厂家类型,1:雷迅,2:华咨,3...
	char SPDCount[5];	//PSD数量
	char SPDIP[SPD_NUM+RES_NUM][20];	//SPD控制器IP地址
	char SPDPort[SPD_NUM+RES_NUM][10];	//SPD控制器端口
	char SPDAddr[SPD_NUM+RES_NUM][10];		//SPD控制器硬件地址

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
	UINT16 RightSideDoorCtrl;			//侧门电子门锁0：保持 1：关锁：2：开锁
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

	float spd_modbus_addr[SPD_NUM];			//防雷器设备地址
	float spdleak_alarm_threshold[SPD_NUM];			//漏电流报警阈值
	// DO报警的值
	UINT8 DO_spdcnt_clear[SPD_NUM];	// 雷击计数清0
	UINT8 DO_totalspdcnt_clear[SPD_NUM];	// 总雷击计数清0
	UINT8 DO_leak_type[SPD_NUM];		// 0:内置漏电流，1：外接漏电流
	UINT8 DO_psdtime_clear[SPD_NUM];	// 雷击时间清0
	UINT8 DO_daytime_clear[SPD_NUM];	// 在线时间清0
	//接地电阻
	UINT16 spdres_id;				// 更改id地址	// 0x12
	UINT16 spdres_alarm_value;		// 报警值修改	// 0x13
}REMOTE_CONTROL;

#pragma pack(pop)

/*寄存器操作*/
UINT8 Read_Register(UINT16 nStartRegNo, UINT16 nRegNum, UINT8 *pdatabuf, UINT8 *perr);
UINT8 Write_Register(UINT16 nStartRegNo, INT16 nRegNum, const UINT8 *pdatabuf, UINT8 datalen, UINT8 *perr);

extern UINT16  System_Reset;

#endif






