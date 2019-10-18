#include "jsonPackage.h"
#include "../IpcamServer.h"
#include "../net_spd.h"

extern ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
extern UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体
extern DEVICE_PARAMS *stuDev_Param[6];	//装置参数寄存器
extern VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体
extern VA_METER_PARAMS *stuVA_Meter_Param[VA_METER_BD_NUM];		//伏安表电压电流结构体
extern REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
extern FLAGRUNSTATUS *stuFlagRunStatus;//门架自由流运行状态结构体
extern RSUCONTROLER stuRsuControl;	//RSU控制器状态
extern RSU_DATA_INIT stuRsuData;	//RSU设备信息结构体
extern RSU_RESET stuRsuReset;			//天线软件复位状态结构体
extern THUAWEIGantry HUAWEIDevValue;	//华为机柜状态
extern THUAWEIALARM HUAWEIDevAlarm;			//华为机柜告警
extern AIRCOND_PARAM *stuAirCondRead;		//读空调状态结构体
extern AIRCOND_PARAM *stuAirCondWrite;		//写空调状态结构体
extern LOCKER_HW_PARAMS *lockerHw_Param[LOCK_NUM];	//门锁状态结构体

extern string StrID;			//硬件ID
extern string StrdeviceType;	//设备型号
extern string StrVersionNo;	//主程序版本号
extern string StrSoftDate;	//版本日期

extern string StrFlagNetRoadID;	//ETC 门架路网编号
extern string StrFlagRoadID;		//ETC 门架路段编号
extern string StrFlagID;			//ETC 门架编号
extern string StrPosId;			//ETC 门架序号
extern string StrDirection;		//行车方向
extern string StrDirDescription;	//行车方向说明
extern string StrCabinetType;		//机柜类型

extern string StrIP;			//IP地址
extern string StrMask;			//子网掩码
extern string StrGateway;		//网关
extern string StrDNS;			//DNS地址

extern string StrIP2;			//IP地址
extern string StrMask2;			//子网掩码
extern string StrGateway2;		//网关
extern string StrDNS2;			//DNS地址

extern string StrHWServer;		//华为服务器地址
extern string StrHWGetPasswd;	//SNMP GET 密码
extern string StrHWSetPasswd;	//SNMP SET 密码
extern string StrServerURL1;	//服务端URL1
extern string StrServerURL2;	//服务端URL2
extern string StrServerURL3;	//服务端URL3
extern string StrServerURL4;	//服务端URL4
extern string StrStationURL;	//虚拟站端URL
extern string StrRSUCount;	//RSU数量
extern string StrRSUIP[RSUCTL_NUM];	//RSUIP地址
extern string StrRSUPort[RSUCTL_NUM];	//RSU端口
extern string StrVehPlateCount;	//识别仪数量
extern string StrVehPlateIP[VEHPLATE_NUM];	//识别仪IP地址
extern string StrVehPlatePort[VEHPLATE_NUM];	//识别仪端口
extern string StrVehPlateKey[VEHPLATE_NUM];	//识别仪用户名密码
extern string StrCAMCount;	//监控摄像头数量
extern string StrCAMIP[CAM_NUM];	//监控摄像头IP地址
extern string StrCAMPort[CAM_NUM];	//监控摄像头端口
extern string StrCAMKey[CAM_NUM];	//监控摄像头用户名密码

extern string StrFireWareCount;	//防火墙数量
extern string StrFireWareIP[FIREWARE_NUM];         //防火墙IP
extern string StrFireWareGetPasswd[FIREWARE_NUM];  //防火墙get密码
extern string StrFireWareSetPasswd[FIREWARE_NUM];  //防火墙set密码
extern string StrIPSwitchCount;	//交换机数量
extern string StrIPSwitchIP[IPSWITCH_NUM] ;//交换机IP
extern string StrIPSwitchGetPasswd[IPSWITCH_NUM] ;//交换机get密码
extern string StrIPSwitchSetPasswd[IPSWITCH_NUM] ;//交换机set密码
extern string StrDeviceNameSeq[SWITCH_COUNT];	//设备名的配置

extern string StrAtlasCount;	//Atlas数量
extern string StrAtlasIP[ATLAS_NUM] ;//AtlasIP
extern string StrAtlasPasswd[ATLAS_NUM] ;//Atlas密码
extern string StrSPDCount;	//防雷器数量
//extern string StrSPDIP[SPD_NUM] ;//防雷器IP
//extern string StrSPDPasswd[SPD_NUM] ;//防雷器密码

extern string StrAdrrVAMeter[VA_METER_BD_MAX_NUM];	//电压电流传感器1的地址
extern string StrAdrrPower[POWER_BD_MAX_NUM];	//电源板1的地址
extern string StrDoCount;//do数量
extern string StrDeviceNameSeq[SWITCH_COUNT];	//设备名的配置
extern string StrDoSeq[SWITCH_COUNT];	//do和设备映射的配置
extern UINT16 DoSeq[SWITCH_COUNT];	// 另外定义一个专门用来存储映射的数组,stuRemote_Ctrl会被清0

//交换机网络数据
extern TFIRESWITCH mTFIRESWITCH[32];
extern string strswitchjson;	//交换机网络数据
//防火墙网络数据
extern TFIRESWITCH mTFIREWALL[32];
extern string strfirewalljson;	//防火墙网络数据

extern pthread_mutex_t snmpoidMutex ;

extern int Writeconfig(void);
extern void GetConfig(VMCONTROL_PARAM *vmctrl_param);
extern void RemoteControl(UINT8* pRCtrl);
extern int Setconfig(string StrKEY,string StrSetconfig);
extern void SetIPinfo(IPInfo *ipInfo);
extern void SetIPinfo2(IPInfo *ipInfo);

extern TIPcamState mTIPcamState[VEHPLATE_NUM];

bool isIPAddressValid(const char* pszIPAddr)
{
    if (!pszIPAddr) return false; //若pszIPAddr为空
    char IP1[100],cIP[4];
    int len = strlen(pszIPAddr);
    int i = 0,j=len-1;
    int k, m = 0,n=0,num=0;
    //去除首尾空格(取出从i-1到j+1之间的字符):
    while (pszIPAddr[i++] == ' ');
    while (pszIPAddr[j--] == ' ');

    for (k = i-1; k <= j+1; k++)
    {
        IP1[m++] = *(pszIPAddr + k);
    }
    IP1[m] = '\0';

    char *p = IP1;

    while (*p!= '\0')
    {
        if (*p == ' ' || *p<'0' || *p>'9') return false;
        cIP[n++] = *p; //保存每个子段的第一个字符，用于之后判断该子段是否为0开头

        int sum = 0;  //sum为每一子段的数值，应在0到255之间
        while (*p != '.'&&*p != '\0')
        {
          if (*p == ' ' || *p<'0' || *p>'9') return false;
          sum = sum * 10 + *p-48;  //每一子段字符串转化为整数
          p++;
        }
        if (*p == '.') {
            if ((*(p - 1) >= '0'&&*(p - 1) <= '9') && (*(p + 1) >= '0'&&*(p + 1) <= '9'))//判断"."前后是否有数字，若无，则为无效IP，如“1.1.127.”
                num++;  //记录“.”出现的次数，不能大于3
            else
                return false;
        };
        if ((sum > 255) || (sum > 0 && cIP[0] =='0')||num>3) return false;//若子段的值>255或为0开头的非0子段或“.”的数目>3，则为无效IP

        if (*p != '\0') p++;
        n = 0;
    }
    if (num != 3) return false;
    return true;
}

bool jsonReader(std::string json, std::map<std::string, std::string> &out)
{
	const char* str = json.c_str();
	Json::CharReaderBuilder b;
	Json::CharReader* reader(b.newCharReader());
	Json::Value root;
	std::string errs;
	bool ok = reader->parse(str, str + strlen(str), &root, &errs);
	if (ok&&errs.size() == 0)
	{
		Json::Value::Members mem = root.getMemberNames();
		for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
		{
			out[*iter] = root[*iter].asString();
		}
	}
	delete reader;
	return true;
}


bool jsonStrReader(char* jsonstrin, int lenin, char* jsonstrout, int *lenout)
{
	VMCONTROL_PARAM vmctrl_param;
	REMOTE_CONTROL *pRCtrl;
	int messagetype=-1;
	std::string json = jsonstrin;
	std::map<std::string, std::string> out;
	jsonReader(jsonstrin, out);
	string mstrdata;
	int opt=SFLAG_READ;

	char key[128],value[128];

	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());
		sprintf(value,"%s",it->second.c_str());
		//printf("%s %s\n",key,value);

		if(it->first=="messagetype")	messagetype=atoi(value);
		if(it->first=="opt") opt=atoi(value);
		it++;
	}
	switch (messagetype)
	{
		case NETCMD_CONFIG_NETWORK:		//5 读取/设置网口1
			if(opt==SFLAG_READ)
			{
				SetjsonIPStr(messagetype,mstrdata);
				*lenout = mstrdata.size();
				memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			}
			else if(opt==SFLAG_WRITE)
			{
				IPInfo ipinfo;
				string iptmp,masktmp,gatewaytmp;
				iptmp=StrIP;masktmp=StrMask;gatewaytmp=StrGateway;//保存原IP设置
				jsonstrIpInfoReader(jsonstrin,lenin,(UINT8*)&ipinfo);//将json字符串转换成结构体
				SetIPinfo(&ipinfo);
				SetjsonReceiveOKStr(messagetype,jsonstrout,lenout);
				if(iptmp!=StrIP || masktmp!=StrMask || gatewaytmp!=StrGateway)	//IP设置更改
					stuRemote_Ctrl->SysReset=SYSRESET; 	//等待重启
			}
			break;
		case NETCMD_SEND_ENVI_PARAM: 			//9 环境寄存器参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrEvnWriter(messagetype,(char*)stuEnvi_Param,jsonstrout,lenout);
			break;
		case NETCMD_SEND_UPS_PARAM: 			//10 UPS参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrUpsWriter(messagetype,(char*)stuUps_Param,jsonstrout,lenout);
			break;
		case NETCMD_SEND_SPD_PARAM: 			//11 防雷器寄存器参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrSpdWriter(messagetype,(char*)stuSpd_Param,jsonstrout,lenout);
			break;
		case NETCMD_SEND_DEV_PARAM: 			//12 控制器参数
			if(opt==SFLAG_READ)
			{
				GetConfig(&vmctrl_param);
				memset(jsonstrout,0,JSON_LEN);
				*lenout=0;
//				jsonStrVMCtlParamWriter(messagetype,(char*)&vmctrl_param,jsonstrout,lenout);
				jsonStrVMCtlParamWriterXY(messagetype,(char*)&vmctrl_param,jsonstrout,lenout);
			}
			else if(opt==SFLAG_WRITE)
			{
				jsonstrVmCtlParamReader(jsonstrin,lenin,(UINT8*)&vmctrl_param);//将json字符串转换成结构体
				SetjsonReceiveOKStr(messagetype,jsonstrout,lenout);
			}
			break;
		case NETCMD_SEND_AIR_PARAM: 			//13 空调参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrAirCondWriter(messagetype,(char*)stuAirCondRead,jsonstrout,lenout);
			break;
		case NETCMD_SEND_RSU_PARAM: 			//14 RSU天线参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
//			jsonStrRsuWriter(messagetype,jsonstrout,lenout);
			jsonStrRsuWriterXY(messagetype,jsonstrout,lenout);
			break;
		case NETCMD_SEND_CAM_PARAM: 			//15 车牌识别参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrVehPlateWriter(messagetype,jsonstrout,lenout);
			break;
		case NETCMD_SEND_SWITCH_INFO: 			//16 交换机状态
			memset(jsonstrout,0,JSON_LEN);
			SetjsonIPSwitchStatusStr(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_FLAGRUNSTATUS:			//17 门架运行状态
			memset(jsonstrout,0,JSON_LEN);
			SetjsonFlagRunStatusStr(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_REMOTE_CONTROL: 			//18 遥控设备
		case NETCMD_HWCABINET_PARMSET: 			//21 华为机柜参数设置
		case NETCMD_DEAL_LOCKER:			//22  门禁开关锁请求
			pRCtrl=stuRemote_Ctrl;
			memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
			jsonstrRCtrlReader(jsonstrin,lenin,(UINT8 *)pRCtrl);//将json字符串转换成结构体
			RemoteControl((UINT8*)pRCtrl);
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			SetjsonReceiveOKStr(messagetype,jsonstrout,lenout);
			break;
		case NETCMD_SWITCH_STATUS:			//19 回路开关状态
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			//jsonStrSwitchStatusWriter(messagetype,jsonstrout,lenout);
			jsonStrSwitchStatusWriterXY(messagetype,jsonstrout,lenout);
			break;
		case NETCMD_HWCABINET_STATUS:			//20  华为机柜状态
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrHWCabinetWriter(messagetype,(char*)&HUAWEIDevValue,jsonstrout,lenout);
			break;
		case NETCMD_SEND_FIREWALL_INFO:			//23 防火墙状态
			memset(jsonstrout,0,JSON_LEN);
			SetjsonFireWallStatusStr(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_CONFIG_NETWORK2: 	//25 读取/设置网口2
			if(opt==SFLAG_READ)
			{
				SetjsonIP2Str(messagetype,mstrdata);
				*lenout = mstrdata.size();
				memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			}
			else if(opt==SFLAG_WRITE)
			{
				IPInfo ipinfo;
				string iptmp,masktmp,gatewaytmp;
				iptmp=StrIP2;masktmp=StrMask2;gatewaytmp=StrGateway2;//保存原IP设置
				jsonstrIpInfoReader(jsonstrin,lenin,(UINT8*)&ipinfo);//将json字符串转换成结构体
				SetIPinfo2(&ipinfo);
				SetjsonReceiveOKStr(messagetype,jsonstrout,lenout);
				if(iptmp!=StrIP2 || masktmp!=StrMask2 || gatewaytmp!=StrGateway2)	//IP设置更改
					stuRemote_Ctrl->SysReset=SYSRESET; 	//等待重启
			}
			break;
		default:
			break;

	}
	return true;
}


bool jsonComputerReader(char* jsonstr, int len)
{
	//printf("%s \t\n",jsonstr);

	std::string json = jsonstr;
	std::map<std::string, std::string> out;
	jsonReader(json, out);

	FLAGRUNSTATUS *pFRS=stuFlagRunStatus;
	char key[50];
	int value;

	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());value=atoi(it->second.c_str()) ;
		//printf("%s %d\n",key,value);

		if(it->first=="computer")	pFRS->Computer=value;		//9 工控机状态
		else if(it->first=="diskcapacity")	pFRS->DiskCapacity=value;	//10 硬盘容量
		else if(it->first=="diskusage")	pFRS->DiskUsage=value;	//11 硬盘使用率
		else if(it->first=="dbstate")	pFRS->DBState=value;	//15 数据库状态
		else if(it->first=="cablenetstate")	pFRS->CableNetState=value;	//16 有线网络状态
		else if(it->first=="wirelessstate")	pFRS->WireLessState=value;	//17 无线网络状态
		else if(it->first=="software")	pFRS->SoftWare=value;	//18 ETC 门架软件状态
		else if(it->first=="softversion")	sprintf(pFRS->SoftVersion,it->second.c_str());	//19 软件版本
		else if(it->first=="cpu_occupancy")	pFRS->cpu_occupancy=value;	//CPU占用率

		it++;
	}
	//printf("\n");
	return true;

/*	printf("%s \t\n",jsonstr);

	FLAGRUNSTATUS *pFRS=stuFlagRunStatus;

	Json::Reader reader;
	Json::Value json_object;

	if (!reader.parse(jsonstr, json_object))
		return 0;

	string computer = json_object["computer"].toStyledString() ;//9 工控机状态
	if(computer != "")	pFRS->Computer=atoi(computer.c_str()) ;
	string diskcapacity = json_object["diskcapacity"].toStyledString() ;//10 硬盘容量
	if(diskcapacity != "")	pFRS->DiskCapacity=atoi(diskcapacity.c_str()) ;
	string diskusage = json_object["diskusage"].toStyledString() ;//11 硬盘使用率
	if(diskusage != "")	pFRS->DiskUsage=atof(diskusage.c_str()) ;
	string dbstate = json_object["dbstate"].toStyledString() ;//15 数据库状态
	if(dbstate != "")	pFRS->DBState=atoi(dbstate.c_str()) ;
	string cablenetstate = json_object["cablenetstate"].toStyledString() ;//16 有线网络状态
	if(cablenetstate != "")	pFRS->CableNetState=atoi(cablenetstate.c_str()) ;
	string wirelessstate = json_object["wirelessstate"].toStyledString() ;//17 无线网络状态
	if(wirelessstate != "")	pFRS->WireLessState=atoi(wirelessstate.c_str()) ;
	string  software= json_object["software"].toStyledString() ;//18 ETC 门架软件状态
	if(software != "")	pFRS->SoftWare=atoi(software.c_str()) ;
	string programversion = json_object["programversion"].toStyledString() ;//19 软件版本
	if(programversion != "")	sprintf(pFRS->SoftVersion,programversion.c_str()) ;
	string cpu_occupancy = json_object["cpu_occupancy"].toStyledString() ;//CPU占用率
	if(cpu_occupancy != "")	pFRS->cpu_occupancy=atoi(cpu_occupancy.c_str()) ;

	return true;*/
}



bool jsonIPCamReaderNext(char* jsonstr, int len,int mIndex)
{
    //printf("%s \r\n",jsonstr);


  Json::Reader reader;

  Json::Value json_object;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  string strdevicemodel = json_object["devicemodel"].toStyledString() ;
  if(strdevicemodel != "")
  {
    //printf("devicemodel:%s\r\n",strdevicemodel.c_str()) ;
    mTIPcamState[mIndex].devicemodel = strdevicemodel ;
  }

  string strerrcode = json_object["errcode"].toStyledString() ;
  if(strerrcode != "")
  {
     //printf("devicemodel:%s\r\n",strerrcode.c_str()) ;
     mTIPcamState[mIndex].errcode = strerrcode ;
  }

  string strfactoryid = json_object["factoryid"].toStyledString() ;
  if(strfactoryid != "")
  {
     //printf("factoryid:%s\r\n",strfactoryid.c_str()) ;
     mTIPcamState[mIndex].factoryid = strfactoryid ;
  }


  string strfilllight = json_object["filllight"].toStyledString() ;
  if(strfilllight != "")
  {
     //printf("filllight:%s\r\n",strfilllight.c_str()) ;
     mTIPcamState[mIndex].filllight = strfilllight ;
  }

  string strip = json_object["ip"].toStyledString() ;
  if(strip != "")
  {
     //printf("ip:%s\r\n",strip.c_str()) ;
     mTIPcamState[mIndex].ip = strip ;
  }

  string strsoftversion = json_object["softversion"].toStyledString() ;
  if(strsoftversion != "")
  {
     //printf("softversion:%s\r\n",strsoftversion.c_str()) ;
     mTIPcamState[mIndex].softversion = strsoftversion ;
  }


  string strstatuscode = json_object["statuscode"].toStyledString() ;
  if(strstatuscode != "")
  {
      //printf("statuscode:%s\r\n",strstatuscode.c_str()) ;
      mTIPcamState[mIndex].statuscode = strstatuscode ;
  }

  string strstatustime = json_object["statustime"].toStyledString() ;
  if(strstatustime != "")
  {
      //printf("statustime:%s\r\n",strstatustime.c_str()) ;
      mTIPcamState[mIndex].statustime = strstatustime ;
  }


  string strtemperature = json_object["temperature"].toStyledString() ;
  if(strtemperature != "")
  {
      //printf("temperature:%s\r\n",strtemperature.c_str()) ;
      mTIPcamState[mIndex].temperature = strtemperature ;
  }

  return true;
}

bool jsonIPCamReader(char* jsonstr, int len,int mIndex)
{
    //printf("%s \r\n",jsonstr);


Json::Reader reader;

  Json::Value json_object;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  string strstatuscode = json_object["statuscode"].toStyledString() ;
  if(strstatuscode != "")
  {
     printf("statuscode:%s\r\n",strstatuscode.c_str()) ;
  }

  string strmessage = json_object["message"].toStyledString() ;
  if(strmessage != "")
  {
     printf("message:%s\r\n",strmessage.c_str()) ;
  }

  string strentity = json_object["entity"].toStyledString() ;
  if(strentity.size() > 3)
  {
     //printf("entity:%s\r\n",strentity.c_str()) ;
     jsonIPCamReaderNext((char *)(strentity.c_str()),strentity.size(),mIndex);
  }

  //std::cout << json_object["age"] << std::endl;

return true ;

}





bool jsonstrRCtrlReader(char* jsonstr, int len, UINT8 *pstuRCtrl)
{
	//printf("%s \t\n",jsonstr);

	std::string json = jsonstr;
	std::map<std::string, std::string> out;
	jsonReader(json, out);

	REMOTE_CONTROL *pRCtrl=(REMOTE_CONTROL *)pstuRCtrl;
	char key[50],keytmp[50];
	int i,value;
	int cabineid=0,operate=0;

	pRCtrl->hwsetenvtemplowerlimit[0]=ACT_HOLD_FF;	//环境温度告警下限255:保持；-20-20（有效）；-20（缺省值）
	pRCtrl->hwsetenvtemplowerlimit[1]=ACT_HOLD_FF;	//环境温度告警下限255:保持；-20-20（有效）；-20（缺省值）
	pRCtrl->hwsetenvhumidityupperlimit[0]=ACT_HOLD_FF;	//环境湿度告警上限 255:保持；0-100（有效）；95（缺省值）
	pRCtrl->hwsetenvhumidityupperlimit[1]=ACT_HOLD_FF;	//环境湿度告警上限 255:保持；0-100（有效）；95（缺省值）
	pRCtrl->hwsetenvhumiditylowerlimit[0]=ACT_HOLD_FF;	//环境湿度告警下限 255:保持；0-100（有效）；5（缺省值）
	pRCtrl->hwsetenvhumiditylowerlimit[1]=ACT_HOLD_FF;	//环境湿度告警下限 255:保持；0-100（有效）；5（缺省值）
	pRCtrl->hwdcairpowerontemppoint[0]=ACT_HOLD_FF;		//空调开机温度点 255:保持； -20-80（有效）；45(缺省值)
	pRCtrl->hwdcairpowerontemppoint[1]=ACT_HOLD_FF;		//空调开机温度点 255:保持； -20-80（有效）；45(缺省值)
	pRCtrl->hwdcairpowerofftemppoint[0]=ACT_HOLD_FF;		//空调关机温度点  		  255:保持； -20-80（有效）；37(缺省值)
	pRCtrl->hwdcairpowerofftemppoint[1]=ACT_HOLD_FF;		//空调关机温度点  		  255:保持； -20-80（有效）；37(缺省值)
	sprintf(pRCtrl->systemtime,"");						//设置控制器时间

	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());value=atoi(it->second.c_str()) ;
		//printf("%s %s\n",key,it->second.c_str());

		if(it->first=="rsu1")	pRCtrl->rsu1=value;		//1500 RSU天线1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="door_do")	pRCtrl->door_do=value;	//1501 电子门锁 0xFF00: 关锁;0xFF01: 开锁
		if(it->first=="autoreclosure")	pRCtrl->autoreclosure=value;	//1502 自动重合闸0xFF00: 遥合;0xFF01: 遥分

		if(it->first=="vehplate1")	pRCtrl->vehplate[0]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate2")	pRCtrl->vehplate[1]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate3")	pRCtrl->vehplate[2]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate4")	pRCtrl->vehplate[3]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate5")	pRCtrl->vehplate[4]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate6")	pRCtrl->vehplate[5]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate7")	pRCtrl->vehplate[6]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate8")	pRCtrl->vehplate[7]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate9")	pRCtrl->vehplate[8]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate10")	pRCtrl->vehplate[9]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate11")	pRCtrl->vehplate[10]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="vehplate12")	pRCtrl->vehplate[11]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分

		if(it->first=="rsucontrlor1")	pRCtrl->rsucontrlor[0]=value;			//rsu 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="rsucontrlor2")	pRCtrl->rsucontrlor[1]=value;			//rsu 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="rsucontrlor3")	pRCtrl->rsucontrlor[2]=value;			//rsu 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="rsucontrlor4")	pRCtrl->rsucontrlor[3]=value;			//rsu 0xFF00: 遥合;0xFF01: 遥分

		if(it->first=="antenna1")	pRCtrl->antenna[0]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna2")	pRCtrl->antenna[1]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna3")	pRCtrl->antenna[2]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna4")	pRCtrl->antenna[3]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna5")	pRCtrl->antenna[4]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna6")	pRCtrl->antenna[5]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna7")	pRCtrl->antenna[6]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna8")	pRCtrl->antenna[7]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna9")	pRCtrl->antenna[8]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna10")	pRCtrl->antenna[9]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna11")	pRCtrl->antenna[10]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna12")	pRCtrl->antenna[11]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分

		if(it->first=="do1")	pRCtrl->antenna[0]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna2")	pRCtrl->antenna[1]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna3")	pRCtrl->antenna[2]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna4")	pRCtrl->antenna[3]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna5")	pRCtrl->antenna[4]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna6")	pRCtrl->antenna[5]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna7")	pRCtrl->antenna[6]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna8")	pRCtrl->antenna[7]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna9")	pRCtrl->antenna[8]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna10")	pRCtrl->antenna[9]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna11")	pRCtrl->antenna[10]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="antenna12")	pRCtrl->antenna[11]=value;			//天线 0xFF00: 遥合;0xFF01: 遥分

		if(it->first=="fireware")	pRCtrl->fireware[0]=value;			//防火墙 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="ipswitch")	pRCtrl->ipswitch[0]=value;			//交换机 0xFF00: 遥合;0xFF01: 遥分

		if(it->first=="sysreset")	pRCtrl->SysReset=value;			//系统重启
		if(it->first=="setsystemtime")	sprintf(pRCtrl->systemtime,"%s",it->second.c_str());			//设置控制器时间

		if(it->first=="hwctrlmonequipreset") pRCtrl->hwctrlmonequipreset=value;	//控制单板复位 0：保持；1：热复位；
		if(it->first=="hwsetacsuppervoltlimit") pRCtrl->hwsetacsuppervoltlimit=value;	//AC过压点设置 0:保持；50-600（有效）；280（缺省值）
		if(it->first=="hwsetacslowervoltlimit") pRCtrl->hwsetacslowervoltlimit=value;	//AC欠压点设置 0:保持；50-600（有效）；180（缺省值）
		if(it->first=="hwsetdcsuppervoltlimit") pRCtrl->hwsetdcsuppervoltlimit=value;	//设置DC过压点 0:保持；53-600（有效）；58（缺省值）
		if(it->first=="hwsetdcslowervoltlimit") pRCtrl->hwsetdcslowervoltlimit=value;	//设置DC欠压点 0:保持；35 - 57（有效）；45（缺省值）
		if(it->first=="hwsetenvtempupperlimit") pRCtrl->hwsetenvtempupperlimit[0]=value;	//环境温度告警上限 0:保持；25-80（有效）；55（缺省值）
		if(it->first=="hwsetenvtempupperlimit2") pRCtrl->hwsetenvtempupperlimit[1]=value;	//环境温度告警上限 0:保持；25-80（有效）；55（缺省值）
		if(it->first=="hwsetenvtemplowerlimit") pRCtrl->hwsetenvtemplowerlimit[0]=value;	//环境温度告警下限255:保持；-20-20（有效）；-20（缺省值）
		if(it->first=="hwsetenvtemplowerlimit2") pRCtrl->hwsetenvtemplowerlimit[1]=value;	//环境温度告警下限255:保持；-20-20（有效）；-20（缺省值）
		if(it->first=="hwsetenvhumidityupperlimit") pRCtrl->hwsetenvhumidityupperlimit[0]=value;	//环境湿度告警上限 255:保持；0-100（有效）；95（缺省值）
		if(it->first=="hwsetenvhumidityupperlimit2") pRCtrl->hwsetenvhumidityupperlimit[1]=value;	//环境湿度告警上限 255:保持；0-100（有效）；95（缺省值）
		if(it->first=="hwsetenvhumiditylowerlimit") pRCtrl->hwsetenvhumiditylowerlimit[0]=value;	//环境湿度告警下限 255:保持；0-100（有效）；5（缺省值）
		if(it->first=="hwsetenvhumiditylowerlimit2") pRCtrl->hwsetenvhumiditylowerlimit[1]=value;	//环境湿度告警下限 255:保持；0-100（有效）；5（缺省值）
		if(it->first=="hwcoolingdevicesmode") pRCtrl->hwcoolingdevicesmode=value;		//温控模式				0：保持；1：纯风扇模式；2：纯空调模式；3：智能模式；
		if(it->first=="hwdcairpowerontemppoint") pRCtrl->hwdcairpowerontemppoint[0]=value;		//空调开机温度点 255:保持； -20-80（有效）；45(缺省值)
		if(it->first=="hwdcairpowerontemppoint2") pRCtrl->hwdcairpowerontemppoint[1]=value;		//空调开机温度点 255:保持； -20-80（有效）；45(缺省值)
		if(it->first=="hwdcairpowerofftemppoint") pRCtrl->hwdcairpowerofftemppoint[0]=value;		//空调关机温度点 		  255:保持； -20-80（有效）；37(缺省值)
		if(it->first=="hwdcairpowerofftemppoint2") pRCtrl->hwdcairpowerofftemppoint[1]=value;		//空调关机温度点 		  255:保持； -20-80（有效）；37(缺省值)
		if(it->first=="hwdcairctrlmode") pRCtrl->hwdcairctrlmode[0]=value;			//空调控制模式 0：保持；1：自动；2：手动
		if(it->first=="hwdcairctrlmode2") pRCtrl->hwdcairctrlmode[1]=value;			//空调控制模式 0：保持；1：自动；2：手动
		if(it->first=="hwctrlsmokereset") pRCtrl->hwctrlsmokereset[0]=value;			//控制烟感复位 0：保持；1：不需复位；2：复位
		if(it->first=="hwctrlsmokereset2") pRCtrl->hwctrlsmokereset[1]=value;			//控制烟感复位 0：保持；1：不需复位；2：复位

		if(it->first=="cabineid")	cabineid=value;			//电子门锁id
		if(it->first=="operate")	operate=value;			//电子门锁操作

		for(i=0;i<SWITCH_COUNT;i++)
		{
			sprintf(keytmp,"do%d",i+1);

			if(it->first==keytmp)
			{
				pRCtrl->doseq[i]=value;
				//printf("%s %d doseq%d=%d\n",keytmp,value,i,pRCtrl->doseq[i]);
			}
		}

		it++;
	}
	//printf("\n");
	if(cabineid==1 && operate==ACT_UNLOCK) pRCtrl->FrontDoorCtrl=ACT_UNLOCK;//前门电子门锁 0：保持 1：关锁：2：开锁；3无权限
	if(cabineid==2 && operate==ACT_UNLOCK) pRCtrl->BackDoorCtrl=ACT_UNLOCK; 	//后门电子门锁 0：保持 1：关锁：2：开锁；3无权限
	if(cabineid==3 && operate==ACT_UNLOCK) pRCtrl->SideDoorCtrl=ACT_UNLOCK; 		//侧门电子门锁 0：保持 1：关锁：2：开锁；3无权限
	if(cabineid==4 && operate==ACT_UNLOCK) pRCtrl->RightSideDoorCtrl=ACT_UNLOCK; 		//侧门电子门锁 0：保持 1：关锁：2：开锁；3无权限
	return true;
}

bool jsonstrAirCondReader(char* jsonstr, int len, UINT8 *pstPam)
{
	//printf("%s \t\n",jsonstr);

	std::string json = jsonstr;
	std::map<std::string, std::string> out;
	jsonReader(json, out);

	AIRCOND_PARAM *pRCtrl=(AIRCOND_PARAM *)pstPam;
	char key[50];
	int value;

	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());value=atoi(it->second.c_str()) ;
		//printf("%s %d\n",key,value);

		if(it->first=="aircondset")	pRCtrl->aircondset=value;		//空调关机//1220   		1
		else if(it->first=="aircoldstartpoint")	pRCtrl->aircoldstartpoint=value;	//空调制冷点//1221 			50
		else if(it->first=="aircoldloop")	pRCtrl->aircoldloop=value;	//空调制冷回差//1222					10
		else if(it->first=="airhotstartpoint")	pRCtrl->airhotstartpoint=value;			//空调制热点//1223 			15
		else if(it->first=="airhotloop")	pRCtrl->airhotloop=value;			//空调制热回差//1224					10
		it++;
	}
	//printf("\n");
	return true;
}

bool jsonstrVmCtlParamReader(char* jsonstr, int len, UINT8 *pstPam)
{
	//printf("%s \t\n",jsonstr);
	int i;

	std::string json = jsonstr;
	std::map<std::string, std::string> out;
	jsonReader(json, out);

	VMCONTROL_PARAM *pRCtrl=(VMCONTROL_PARAM *)pstPam;
	char key[50],value[128],keytmp[50];
//	int value;

	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());
		sprintf(value,"%s",it->second.c_str());
		printf("%s %s\n",key,value);

		//门架信息
		if(it->first=="cabinettype" && StrCabinetType!=value)	//机柜类型	1:华为；2:利通
		{
			StrCabinetType=value;
			sprintf(pRCtrl->CabinetType,"%s",value);
			Setconfig("CabinetType=",value);
		}
		if(it->first=="flagnetroadid" && StrFlagNetRoadID!=value)	//ETC 门架路网编号
		{
			StrFlagNetRoadID=value;
			sprintf(pRCtrl->FlagNetRoadID,"%s",value);
			Setconfig("FlagNetRoadID=",value);
		}
		if(it->first=="flagroadid" && StrFlagRoadID!=value)	//ETC 门架路段编号
		{
			StrFlagRoadID=value;
			sprintf(pRCtrl->FlagRoadID,"%s",value);
			Setconfig("FlagRoadID=",value);
		}
		if(it->first=="flagid" && StrFlagID!=value)	//ETC 门架编号
		{
			StrFlagID=value;
			sprintf(pRCtrl->FlagID,"%s",value);
			Setconfig("FlagID=",value);
		}
		if(it->first=="posid" && StrPosId!=value)	//ETC 门架序号
		{
			StrPosId=value;
			sprintf(pRCtrl->PosId,"%s",value);
			Setconfig("PosId=",value);
		}
		if(it->first=="direction" && StrDirection!=value)	//行车方向
		{
			StrDirection=value;
			sprintf(pRCtrl->Direction,"%s",value);
			Setconfig("Direction=",value);
		}
		if(it->first=="dirdescription" && StrDirDescription!=value)	//行车方向说明
		{
			StrDirDescription=value;
			sprintf(pRCtrl->DirDescription,"%s",value);
			Setconfig("DirDescription=",value);
		}
		//参数设置
		if(it->first=="hwserver" && StrHWServer!=value)	//华为服务器地址
		{
			StrHWServer=value;
			sprintf(pRCtrl->HWServer,"%s",value);
			Setconfig("HWServer=",value);
		}
		if(it->first=="hwgetpasswd" && StrHWGetPasswd!=value)	//SNMP GET 密码
		{
			StrHWGetPasswd=value;
			sprintf(pRCtrl->HWGetPasswd,"%s",value);
			Setconfig("HWGetPasswd=",value);
		}
		if(it->first=="hwsetpasswd" && StrHWSetPasswd!=value)	//SNMP SET 密码
		{
			StrHWSetPasswd=value;
			sprintf(pRCtrl->HWSetPasswd,"%s",value);
			Setconfig("HWSetPasswd=",value);
		}
		if(it->first=="serverurl1" && StrServerURL1!=value)	//服务器1推送地址
		{
			StrServerURL1=value;
			sprintf(pRCtrl->ServerURL1,"%s",value);
			Setconfig("ServerURL1=",value);
		}
		if(it->first=="serverurl2" && StrServerURL2!=value)	//服务器2推送地址
		{
			StrServerURL2=value;
			sprintf(pRCtrl->ServerURL2,"%s",value);
			Setconfig("ServerURL2=",value);
		}
		if(it->first=="serverurl3" && StrServerURL3!=value)	//服务器3推送地址
		{
			StrServerURL3=value;
			sprintf(pRCtrl->ServerURL3,"%s",value);
			Setconfig("ServerURL3=",value);
		}
		if(it->first=="serverurl4" && StrServerURL4!=value)	//门锁4推送地址
		{
			StrServerURL4=value;
			sprintf(pRCtrl->ServerURL4,"%s",value);
			Setconfig("ServerURL4=",value);
		}
		if(it->first=="stationurl" && StrStationURL!=value)	//控制器接收地址
		{
			StrStationURL=value;
			sprintf(pRCtrl->StationURL,"%s",value);
			Setconfig("StationURL=",value);
		}
		if(it->first=="rsucount" && StrRSUCount!=value && atoi(value)>=0 && atoi(value)<=RSUCTL_NUM)	//RSU控制器数量
		{
			StrRSUCount=value;
			sprintf(pRCtrl->RSUCount,"%s",value);
			Setconfig("RSUCount=",value);
		}
		for(i=0;i<RSUCTL_NUM;i++)
		{
			sprintf(keytmp,"rsu%dip",i+1);//RSUIP地址
			if(it->first==keytmp && StrRSUIP[i]!=value)
			{
				StrRSUIP[i]=value;
				sprintf(pRCtrl->RSUIP[i],"%s",value);
				sprintf(key,"RSU%dIP=",i+1);//RSUIP地址
				Setconfig(key,value);
			}
			sprintf(keytmp,"rsu%dport",i+1);//RSU端口
			if(it->first==keytmp && StrRSUPort[i]!=value)
			{
				StrRSUPort[i]=value;
				sprintf(pRCtrl->RSUPort[i],"%s",value);
				sprintf(key,"RSU%dPort=",i+1);//RSU端口
				Setconfig(key,value);
			}
		}
		if(it->first=="vehplatecount" && StrVehPlateCount!=value && atoi(value)>=0 && atoi(value)<=VEHPLATE_NUM)	//识别仪数量
		{
			StrVehPlateCount=value;
			sprintf(pRCtrl->VehPlateCount,"%s",value);
			Setconfig("VehPlateCount=",value);
		}
		for(i=0;i<VEHPLATE_NUM;i++)
		{
			sprintf(keytmp,"vehplate%dip",i+1);//识别仪IP地址
			if(it->first==keytmp && StrVehPlateIP[i]!=value)
			{
				StrVehPlateIP[i]=value;
				sprintf(pRCtrl->VehPlateIP[i],"%s",value);
				sprintf(key,"VehPlate%dIP=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"vehplate%dport",i+1);//识别仪端口
			if(it->first==keytmp && StrVehPlatePort[i]!=value)
			{
				StrVehPlatePort[i]=value;
				sprintf(pRCtrl->VehPlatePort[i],"%s",value);
				sprintf(key,"VehPlate%dPort=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"vehplate%dkey",i+1);//识别仪用户名密码
			if(it->first==keytmp && StrVehPlateKey[i]!=value)
			{
				StrVehPlateKey[i]=value;
				sprintf(pRCtrl->VehPlateKey[i],"%s",value);
				sprintf(key,"VehPlate%dKey=",i+1);
				Setconfig(key,value);
			}
		}
		if(it->first=="camcount" && StrCAMCount!=value && atoi(value)>=0 && atoi(value)<=CAM_NUM)	//监控摄像头数量
		{
			StrCAMCount=value;
			sprintf(pRCtrl->CAMCount,"%s",value);
			Setconfig("CAMCount=",value);
		}
		for(i=0;i<CAM_NUM;i++)
		{
			sprintf(keytmp,"cam%dip",i+1);//监控摄像头IP地址
			if(it->first==keytmp && StrCAMIP[i]!=value)
			{
				StrCAMIP[i]=value;
				sprintf(pRCtrl->CAMIP[i],"%s",value);
				sprintf(key,"CAM%dIP=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"cam%dport",i+1);//监控摄像头端口
			if(it->first==keytmp && StrCAMPort[i]!=value)
			{
				StrCAMPort[i]=value;
				sprintf(pRCtrl->CAMPort[i],"%s",value);
				sprintf(key,"CAM%dPort=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"cam%dkey",i+1);//监控摄像头用户名密码
			if(it->first==keytmp && StrCAMKey[i]!=value)
			{
				StrCAMKey[i]=value;
				sprintf(pRCtrl->CAMKey[i],"%s",value);
				sprintf(key,"CAM%dKey=",i+1);
				Setconfig(key,value);
			}
		}
		if(it->first=="ipswitchcount" && StrIPSwitchCount!=value && atoi(value)>=0 && atoi(value)<=IPSWITCH_NUM)	//交换机数量
		{
			StrIPSwitchCount=value;
			sprintf(pRCtrl->SwitchCount,"%s",value);
			Setconfig("SwitchCount=",value);
		}
		for(i=0;i<IPSWITCH_NUM;i++)
		{
			sprintf(keytmp,"ipswitch%dip",i+1);//交换机IP地址
			if(it->first==keytmp && StrIPSwitchIP[i]!=value)
			{
				StrIPSwitchIP[i]=value;
				sprintf(pRCtrl->SwitchIP[i],"%s",value);
				sprintf(key,"Switch%dIP=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"ipswitch%dgetpasswd",i+1);//交换机get密码
			if(it->first==keytmp && StrIPSwitchGetPasswd[i]!=value)
			{
				StrIPSwitchGetPasswd[i]=value;
				sprintf(pRCtrl->SwitchGetPasswd[i],"%s",value);
				sprintf(key,"Switch%dGetPasswd=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"ipswitch%dsetpasswd",i+1);//交换机set密码
			if(it->first==keytmp && StrIPSwitchSetPasswd[i]!=value)
			{
				StrIPSwitchSetPasswd[i]=value;
				sprintf(pRCtrl->SwitchSetPasswd[i],"%s",value);
				sprintf(key,"Switch%dSetPasswd=",i+1);
				Setconfig(key,value);
			}
		}
		if(it->first=="firewarecount" && StrFireWareCount!=value && atoi(value)>=0 && atoi(value)<=FIREWARE_NUM)	//防火墙数量
		{
			StrFireWareCount=value;
			sprintf(pRCtrl->FireWareCount,"%s",value);
			Setconfig("FireWareCount=",value);
		}
		for(i=0;i<FIREWARE_NUM;i++)
		{
			sprintf(keytmp,"fireware%dip",i+1);//防火墙IP地址
			if(it->first==keytmp && StrFireWareIP[i]!=value)
			{
				StrFireWareIP[i]=value;
				sprintf(pRCtrl->FireWareIP[i],"%s",value);
				sprintf(key,"FireWare%dIP=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"fireware%dgetpasswd",i+1);//防火墙get密码
			if(it->first==keytmp && StrFireWareGetPasswd[i]!=value)
			{
				StrFireWareGetPasswd[i]=value;
				sprintf(pRCtrl->FireWareGetPasswd[i],"%s",value);
				sprintf(key,"FireWare%dGetPasswd=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"fireware%dsetpasswd",i+1);//防火墙set密码
			if(it->first==keytmp && StrFireWareSetPasswd[i]!=value)
			{
				StrFireWareSetPasswd[i]=value;
				sprintf(pRCtrl->FireWareSetPasswd[i],"%s",value);
				sprintf(key,"FireWare%dSetPasswd=",i+1);
				Setconfig(key,value);
			}
		}

		//Atlas
		if(it->first=="atlascount" && StrAtlasCount!=value && atoi(value)>=0 && atoi(value)<=ATLAS_NUM)	//Atlas数量
		{
			StrAtlasCount=value;
			sprintf(pRCtrl->AtlasCount,"%s",value);
			Setconfig("AtlasCount=",value);
		}
		for(i=0;i<ATLAS_NUM;i++)
		{
			sprintf(keytmp,"atlas%dip",i+1);//AtlasIP地址
			if(it->first==keytmp && StrAtlasIP[i]!=value)
			{
				StrAtlasIP[i]=value;
				sprintf(pRCtrl->AtlasIP[i],"%s",value);
				sprintf(key,"Atlas%dIP=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"atlas%dpasswd",i+1);//Atlas密码
			if(it->first==keytmp && StrAtlasPasswd[i]!=value)
			{
				StrAtlasPasswd[i]=value;
				sprintf(pRCtrl->AtlasPasswd[i],"%s",value);
				sprintf(key,"Atlas%dPasswd=",i+1);
				Setconfig(key,value);
			}
		}
		#if 0
		//防雷器SPD
		if(it->first=="spdcount" && StrSPDCount!=value && atoi(value)>=0 && atoi(value)<=SPD_NUM)	//Atlas数量
		{
			StrSPDCount=value;
			sprintf(pRCtrl->SPDCount,"%s",value);
			Setconfig("SPDCount=",value);
		}
		for(i=0;i<SPD_NUM;i++)
		{
			sprintf(keytmp,"spd%dip",i+1);//SPDIP地址
			if(it->first==keytmp && StrSPDIP[i]!=value)
			{
				StrSPDIP[i]=value;
				sprintf(pRCtrl->SPDIP[i],"%s",value);
				sprintf(key,"SPD%dIP=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"spd%dpasswd",i+1);//SPD密码
			if(it->first==keytmp && StrSPDPasswd[i]!=value)
			{
				StrSPDPasswd[i]=value;
				sprintf(pRCtrl->SPDPasswd[i],"%s",value);
				sprintf(key,"SPD%dPasswd=",i+1);
				Setconfig(key,value);
			}
		}
		#endif
		for(i=0;i<LOCK_MAX_NUM;i++)
		{
			sprintf(keytmp,"adrrlock%d",i+1);//门锁地址
			if(it->first==keytmp && StrAdrrLock[i]!=value)
			{
				StrAdrrLock[i]=value;
				sprintf(pRCtrl->LockAddr[i],"%s",value);
				sprintf(key,"LOCKADD%d=",i+1);
				Setconfig(key,value);
			}
		}
		for(i=0;i<POWER_BD_NUM;i++)
		{
			sprintf(keytmp,"poweraddr%d",i+1);//门锁地址
			if(it->first==keytmp && StrAdrrPower[i]!=value)
			{
				StrAdrrPower[i]=value;
				sprintf(pRCtrl->PowerAddr[i],"%s",value);
				sprintf(key,"POWERBDADD%d=",i+1);
				Setconfig(key,value);
			}
		}
		if(it->first=="hardwareid" && StrID!=value)
		{
			StrID=value;	//硬件ID
			sprintf(pRCtrl->hardwareid,"%s",value); //硬件ID
			Setconfig("ID=",value);//硬件ID
		}
		for(i=0;i<VA_METER_BD_NUM;i++)
		{
			sprintf(keytmp,"adrrvameter%d",i+1);//电能表地址
			if(it->first==keytmp && StrAdrrVAMeter[i]!=value)
			{
				StrAdrrVAMeter[i]=value;
				sprintf(pRCtrl->VameterAddr[i],"%s",value);
				sprintf(key,"VAMETERADDR%d=",i+1);
				Setconfig(key,value);
			}
		}

		if(it->first=="do_count" && StrDoCount!=value && atoi(value)>=0 && atoi(value)<=SWITCH_COUNT)	//do数量
		{
			StrDoCount=value;
			sprintf(pRCtrl->DoCount,"%s",value);
			Setconfig("DO_Count=",value);
		}
		for(i=0;i<SWITCH_COUNT;i++)
		{
			if(it->first==StrDeviceNameSeq[i] && StrDoSeq[i]!=value)
			{
				StrDoSeq[i]=value;
				DoSeq[i]=atoi(value);
				sprintf(key,pRCtrl->DeviceNameSeq[i]);//设备名称的配置
				Setconfig(key,value);//车牌识别DO映射
			}
		}
		it++;
	}

	printf("\n");

	Writeconfig();

	return true;
}


bool jsonstrIpInfoReader(char* jsonstr, int len, UINT8 *pstIPPam)
{
	//printf("%s \t\n",jsonstr);

	std::string json = jsonstr;
	std::map<std::string, std::string> out;
	jsonReader(json, out);

	IPInfo *pIPInfo=(IPInfo *)pstIPPam;
	char key[50],value[128];

	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());
		sprintf(value,"%s",it->second.c_str());
		//printf("%s %s\n",key,value);

		//参数设置
		if(it->first=="ipaddr")
		{
			if(isIPAddressValid(value))
			{
				StrIP=value;	//IP地址
				sprintf(pIPInfo->ip,"%s",StrIP.c_str());//IP地址
			}
		}
		if(it->first=="mask")
		{
			if(isIPAddressValid(value))
			{
				StrMask=value;	//子网掩码
				sprintf(pIPInfo->submask,"%s",StrMask.c_str()); //子网掩码
			}
		}
		if(it->first=="gateway")
		{
			if(isIPAddressValid(value))
			{
				StrGateway=value;//网关
				sprintf(pIPInfo->gateway_addr,"%s",StrGateway.c_str()); //网关
			}
		}
		if(it->first=="dns")
		{
			StrDNS=value;//DNS地址
			sprintf(pIPInfo->dns,"%s",StrDNS.c_str());//DNS地址
		}

		if(it->first=="ipaddr2")
		{
			if(isIPAddressValid(value))
			{
				StrIP2=value;	//IP地址
				sprintf(pIPInfo->ip,value);//IP地址
			}
		}
		if(it->first=="mask2")
		{
			if(isIPAddressValid(value))
			{
				StrMask2=value;	//子网掩码
				sprintf(pIPInfo->submask,"%s",value); //子网掩码
			}
		}
		if(it->first=="gateway2")
		{
			StrGateway2=value;//网关
			sprintf(pIPInfo->gateway_addr,"%s",value); //网关
		}
		if(it->first=="dns2")
		{
			StrDNS2=value;//DNS地址
			sprintf(pIPInfo->dns,"%s",value);//DNS地址
		}
		it++;
	}

	printf("\n");
	return true;
}


bool jsonWriter(std::map<std::string, std::string> in, std::string &json)
{
	Json::Value item;
//	Json.parseObject("...", Feature.OrderedField);

	std::map<std::string, std::string>::iterator it;

	it = in.begin();

	while (it != in.end())
	{
		item[it->first] = it->second;
		it++;
	}

	json = item.toStyledString();
	return true;
}



bool jsonStrEvnWriter(int messagetype,char *pstrEnvPam, char *json, int *len)
{
	ENVI_PARAMS *pParm=(ENVI_PARAMS *)pstrEnvPam;
	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
	// 环境数据结构体
	sprintf(str,"%.1f",pParm->temp/10.0);strJson = strJson + "\"temp\":"+ str +",\n";	//当前环境温度值300 ×10
	sprintf(str,"%.1f",pParm->moist/10.0);strJson = strJson + "\"moist\":"+ str +",\n";	//当前环境湿度值301 ×10
	if(pParm->temp==0x7fff && pParm->moist==0x7fff)
		strJson = strJson + "\"online_flag\": 1 ,\n";	//温湿度是否在线
	else
		strJson = strJson + "\"online_flag\": 0 ,\n";	//温湿度是否在线
	sprintf(str,"%d",pParm->water_flag);strJson = strJson + "\"water_flag\":"+ str +",\n";	//漏水302
	sprintf(str,"%d",pParm->front_door_flag);strJson = strJson + "\"front_door_flag\":"+ str +",\n";	//前柜门开关状态303
	sprintf(str,"%d",pParm->back_door_flag);strJson = strJson + "\"back_door_flag\":"+ str +",\n";	//后柜门开关状态304
	sprintf(str,"%d",pParm->door_overtime);strJson = strJson + "\"door_overtime\":"+ str +",\n";	//柜门开启超时305
	sprintf(str,"%d",pParm->smoke_event_flag);strJson = strJson + "\"smoke_event_flag\":"+ str +",\n";	//烟雾报警306

	sprintf(str,"%d",pParm->air_cond_status);strJson = strJson + "\"air_cond_status\":"+ str +",\n";	//空调状态310
	sprintf(str,"%d",pParm->air_cond_fan_in_status);strJson = strJson + "\"air_cond_fan_in_status\":"+ str +",\n";	//空调内风机状态311
	sprintf(str,"%d",pParm->air_cond_fan_out_status);strJson = strJson + "\"air_cond_fan_out_status\":"+ str +",\n";	//空调外风机状态312
	sprintf(str,"%d",pParm->air_cond_comp_status);strJson = strJson + "\"air_cond_comp_status\":"+ str +",\n";	//空调压缩机状态313
	sprintf(str,"%d",pParm->air_cond_heater_status);strJson = strJson + "\"air_cond_heater_status\":"+ str +",\n";	//空调电加热状态314
	sprintf(str,"%d",pParm->air_cond_fan_emgy_status);strJson = strJson + "\"air_cond_fan_emgy_status\":"+ str +",\n";	//空调应急风机状态315
	sprintf(str,"%.1f",pParm->air_cond_temp_out/10.0);strJson = strJson + "\"air_cond_temp_out\":"+ str +",\n";	//当前空调室外温度值316 ×10
	sprintf(str,"%.1f",pParm->air_cond_temp_in/10.0);strJson = strJson + "\"air_cond_temp_in\":"+ str +",\n";	//当前空调室内温度值317 ×10
	sprintf(str,"%.4f",pParm->air_cond_amp/1000.0);strJson = strJson + "\"air_cond_amp\":"+ str +",\n";	//当前空调电流值318 ×1000
	sprintf(str,"%.1f",pParm->air_cond_volt/10.0);strJson = strJson + "\"air_cond_volt\":"+ str +",\n";	//当前空调电压值319 ×1

	sprintf(str,"%d",pParm->air_cond_hightemp_alarm);strJson = strJson + "\"air_cond_hightemp_alarm\":"+ str +",\n";	//空调高温告警320
	sprintf(str,"%d",pParm->air_cond_lowtemp_alarm);strJson = strJson + "\"air_cond_lowtemp_alarm\":"+ str +",\n";	//空调低温告警321
	sprintf(str,"%d",pParm->air_cond_highmoist_alarm);strJson = strJson + "\"air_cond_highmoist_alarm\":"+ str +",\n";	//空调高湿告警322
	sprintf(str,"%d",pParm->air_cond_lowmoist_alarm);strJson = strJson + "\"air_cond_lowmoist_alarm\":"+ str +",\n";	//空调低湿告警323
	sprintf(str,"%d",pParm->air_cond_infan_alarm);strJson = strJson + "\"air_cond_infan_alarm\":"+ str +",\n";	//空调内风机故障324
	sprintf(str,"%d",pParm->air_cond_outfan_alarm);strJson = strJson + "\"air_cond_outfan_alarm\":"+ str +",\n";	//空调外风机故障325
	sprintf(str,"%d",pParm->air_cond_comp_alarm);strJson = strJson + "\"air_cond_comp_alarm\":"+ str +",\n";	//空调压缩机故障326
	sprintf(str,"%d",pParm->air_cond_heater_alarm);strJson = strJson + "\"air_cond_heater_alarm\":"+ str +",\n";	//空调电加热故障327
	sprintf(str,"%d",pParm->air_cond_emgyfan_alarm);strJson = strJson + "\"air_cond_emgyfan_alarm\":"+ str +"\n";	//空调应急风机故障328
    strJson +=  "}\n\n\0\0";


//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);
	return true;
}


bool jsonStrSpdWriter(int messagetype,char *pstrSpdPam, char *json, int *len)
{
	SPD_PARAMS *pParm=(SPD_PARAMS *)pstrSpdPam;
	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
	// 防雷器结构体
	//sprintf(str,"%d",pParm->status);strJson = strJson + "\"status\":"+ str +",\n";	// 防雷器在线状态
	//sprintf(str,"%d",pParm->struck_times);strJson = strJson + "\"struck_times\":"+ str +"\n";	// 雷击次数
    strJson +=  "}\n\n\0\0";

//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);
	return true;
}


bool jsonStrVMCtlParamWriter(int messagetype,char *pstrVMCtl, char *json, int *len)
{
	VMCONTROL_PARAM *pParm=(VMCONTROL_PARAM *)pstrVMCtl;
	int vehplatecnt,rsucnt;

	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间

    //门架信息
	strJson = strJson + "\"flagnetroadid\":\""+ StrFlagNetRoadID +"\",\n";	//ETC 门架路网编号
	strJson = strJson + "\"flagroadid\":\""+ StrFlagRoadID +"\",\n";	//ETC 门架路段编号
	strJson = strJson + "\"flagid\":\""+ StrFlagID +"\",\n";	//ETC 门架编号
	strJson = strJson + "\"posid\":\""+ StrPosId +"\",\n";	//ETC 门架序号
	strJson = strJson + "\"direction\":\""+ StrDirection +"\",\n";	//行车方向
	strJson = strJson + "\"dirdescription\":\""+ StrDirDescription +"\",\n";	//行车方向说明

    //IP 地址
	strJson = strJson + "\"ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"mask\":\""+ StrMask +"\",\n";	//子网掩码
	strJson = strJson + "\"gateway\":\""+ StrGateway +"\",\n";	//网关
	strJson = strJson + "\"dns\":\""+ StrDNS +"\",\n";	//DNS地址

	strJson = strJson + "\"ipaddr2\":\""+ StrIP2 +"\",\n";	//IP地址
	strJson = strJson + "\"mask2\":\""+ StrMask2 +"\",\n";	//子网掩码
	strJson = strJson + "\"gateway2\":\""+ StrGateway2 +"\",\n";	//网关
	strJson = strJson + "\"dns2\":\""+ StrDNS2 +"\",\n";	//DNS地址

    //参数设置;
	strJson = strJson + "\"hwserver\":\""+ StrHWServer +"\",\n";	//华为服务器IP地址
	strJson = strJson + "\"hwgetpasswd\":\""+ StrHWGetPasswd +"\",\n";	//SNMP GET 密码
	strJson = strJson + "\"hwsetpasswd\":\""+ StrHWSetPasswd +"\",\n";	//SNMP SET 密码
	strJson = strJson + "\"serverurl1\":\""+ StrServerURL1 +"\",\n";	//服务器1推送地址
	strJson = strJson + "\"serverurl2\":\""+ StrServerURL2 +"\",\n";	//服务器2推送地址
	strJson = strJson + "\"serverurl3\":\""+ StrServerURL3 +"\",\n";	//服务器3推送地址
	strJson = strJson + "\"serverurl4\":\""+ StrServerURL4 +"\",\n";	//服务器4推送地址
	strJson = strJson + "\"stationurl\":\""+ StrStationURL +"\",\n";	//控制器接收地址
	strJson = strJson + "\"rsucount\":\""+ StrRSUCount +"\",\n";	//RSU数量
	rsucnt=atoi(StrRSUCount.c_str());
	for(i=0;i<RSUCTL_NUM;i++)
	{
		sprintf(str,"\"rsu%dip\":\"%s\",\n",i+1,StrRSUIP[i].c_str()); strJson = strJson + str;//RSUIP地址
		sprintf(str,"\"rsu%dport\":\"%s\",\n",i+1,StrRSUPort[i].c_str()); strJson = strJson + str;//RSU端口
	}
	strJson = strJson + "\"vehplatecount\":\""+ StrVehPlateCount +"\",\n";	//识别仪数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	for(i=0;i<VEHPLATE_NUM;i++)
	{
		sprintf(str,"\"vehplate%dip\":\"%s\",\n",i+1,StrVehPlateIP[i].c_str()); strJson = strJson + str;//识别仪地址
		sprintf(str,"\"vehplate%dport\":\"%s\",\n",i+1,StrVehPlatePort[i].c_str()); strJson = strJson + str;//识别仪端口
		sprintf(str,"\"vehplate%dkey\":\"%s\",\n",i+1,StrVehPlateKey[i].c_str()); strJson = strJson + str;//识别仪用户名密码
	}
	strJson = strJson + "\"camcount\":\""+ StrCAMCount +"\",\n";	//监控摄像头数量
	for(i=0;i<CAM_NUM;i++)
	{
		sprintf(str,"\"cam%dip\":\"%s\",\n",i+1,StrCAMIP[i].c_str());
		strJson = strJson + str;//监控摄像头IP地址
		sprintf(str,"\"cam%dport\":\"%s\",\n",i+1,StrCAMPort[i].c_str()); //监控摄像头端口
		strJson = strJson + str;
		sprintf(str,"\"cam%dkey\":\"%s\",\n",i+1,StrCAMKey[i].c_str()); //监控摄像头用户名密码
		strJson = strJson + str;
	}

	strJson = strJson + "\"firewarecount\":\""+ StrFireWareCount +"\",\n";	//防火墙数量
	for(i=0;i<FIREWARE_NUM;i++)
	{
		sprintf(str,"\"fireware%dip\":\"%s\",\n",i+1,StrFireWareIP[i].c_str()); //防火墙IP
		strJson = strJson + str;
		sprintf(str,"\"fireware%dgetpasswd\":\"%s\",\n",i+1,StrFireWareGetPasswd[i].c_str()); //防火墙get密码
		strJson = strJson + str;
		sprintf(str,"\"fireware%dsetpasswd\":\"%s\",\n",i+1,StrFireWareSetPasswd[i].c_str()); //防火墙set密码
		strJson = strJson + str;
	}
	strJson = strJson + "\"ipswitchcount\":\""+ StrIPSwitchCount +"\",\n";	//交换机数量
	for(i=0;i<IPSWITCH_NUM;i++)
	{
		sprintf(str,"\"ipswitch%dip\":\"%s\",\n",i+1,StrIPSwitchIP[i].c_str()); //交换机IP
		strJson = strJson + str;
		sprintf(str,"\"ipswitch%dgetpasswd\":\"%s\",\n",i+1,StrIPSwitchGetPasswd[i].c_str()); //交换机get密码
		strJson = strJson + str;
		sprintf(str,"\"ipswitch%dsetpasswd\":\"%s\",\n",i+1,StrIPSwitchSetPasswd[i].c_str()); //交换机set密码
		strJson = strJson + str;
	}
	//Atlas
	strJson = strJson + "\"atlascount\":\""+ StrAtlasCount +"\",\n";	//Atlas数量
	for(i=0;i<ATLAS_NUM;i++)
	{
		sprintf(str,"\"atlas%dip\":\"%s\",\n",i+1,StrAtlasIP[i].c_str()); //AtlasIP
		strJson = strJson + str;
		sprintf(str,"\"atlas%dpasswd\":\"%s\",\n",i+1,StrAtlasPasswd[i].c_str()); //Atlas密码
		strJson = strJson + str;
	}
	#if 0
	//防雷器
	strJson = strJson + "\"spdcount\":\""+ StrSPDCount +"\",\n";	//防雷器数量
	for(i=0;i<ATLAS_NUM;i++)
	{
		sprintf(str,"\"spd%dip\":\"%s\",\n",i+1,StrSPDIP[i].c_str()); //防雷器IP
		strJson = strJson + str;
		sprintf(str,"\"spd%dpasswd\":\"%s\",\n",i+1,StrSPDPasswd[i].c_str()); //防雷器密码
		strJson = strJson + str;
	}
	#endif

	for(i=0;i<LOCK_NUM;i++)
	{
		sprintf(str,"\"adrrlock%d\":\"%s\",\n",i+1,StrAdrrLock[i].c_str()); strJson = strJson + str;//门锁的地址
	}
	for(i=0;i<VA_METER_BD_NUM;i++)
	{
		sprintf(str,"\"adrrvameter%d\":\"%s\",\n",i+1,StrAdrrVAMeter[i].c_str()); strJson = strJson + str;//电压电流传感器的地址
	}
	for(i=0;i<POWER_BD_NUM;i++)
	{
		sprintf(str,"\"poweraddr%d\":\"%s\",\n",i+1,StrAdrrPower[i].c_str()); strJson = strJson + str;//电源板1的地址
	}
	strJson = strJson + "\"do_count\":\""+ StrDoCount +"\",\n";	//do数量
	for(i=0;i<SWITCH_COUNT;i++)
	{
		sprintf(str,"\"%s\":\"%s\",\n",StrDeviceNameSeq[i].c_str(),StrDoSeq[i].c_str()); strJson = strJson + str;//设备映射DO
	}

	strJson = strJson + "\"devicetype\":\""+ StrdeviceType +"\",\n";	//设备型号900~919
	strJson = strJson + "\"hardwareid\":\""+ StrID +"\",\n";	//硬件ID
	strJson = strJson + "\"softversion\":\""+ StrVersionNo +"\",\n";	//主程序版本号920

	strJson = strJson + "\"secsoftversion1\":\""+ stuVMCtl_Param->secSoftVersion[0] +"\",\n";	//副版本号
	strJson = strJson + "\"secsoftversion2\":\""+ stuVMCtl_Param->secSoftVersion[1] +"\",\n";	//副版本号
	strJson = strJson + "\"secsoftversion3\":\""+ stuVMCtl_Param->secSoftVersion[2] +"\",\n";	//副版本号
	strJson = strJson + "\"softdate\":\""+ StrSoftDate +"\"\n";	//版本日期

    strJson +=  "}\n\n\0\0";

//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);
	return true;
}


bool jsonStrVMCtlParamWriterXY(int messagetype,char *pstrVMCtl, char *json, int *len)
{
	VMCONTROL_PARAM *pParm=(VMCONTROL_PARAM *)pstrVMCtl;
	int vehplatecnt,rsucnt;

	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间

    //门架信息
	strJson = strJson + "\"flagnetroadid\":\""+ StrFlagNetRoadID +"\",\n";	//ETC 门架路网编号
	strJson = strJson + "\"flagroadid\":\""+ StrFlagRoadID +"\",\n";	//ETC 门架路段编号
	strJson = strJson + "\"flagid\":\""+ StrFlagID +"\",\n";	//ETC 门架编号
	strJson = strJson + "\"posid\":\""+ StrPosId +"\",\n";	//ETC 门架序号
	strJson = strJson + "\"direction\":\""+ StrDirection +"\",\n";	//行车方向
	strJson = strJson + "\"dirdescription\":\""+ StrDirDescription +"\",\n";	//行车方向说明

	strJson = strJson + "\"iplist\": [\n";		//ip列表
	//IP 地址
    strJson +=  "{\n";
	sprintf(str,"\"name\":\"lan1\",\n"); strJson = strJson + str;//名称
	strJson = strJson + "\"ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"mask\":\""+ StrMask +"\",\n";	//子网掩码
	strJson = strJson + "\"gateway\":\""+ StrGateway +"\",\n";	//网关
	strJson = strJson + "\"dns\":\""+ StrDNS +"\"\n";	//DNS地址
    strJson +=  "},\n";

    strJson +=  "{\n";
	sprintf(str,"\"name\":\"lan2\",\n"); strJson = strJson + str;//名称
	strJson = strJson + "\"ipaddr\":\""+ StrIP2 +"\",\n";	//IP地址
	strJson = strJson + "\"mask\":\""+ StrMask2 +"\",\n";	//子网掩码
	strJson = strJson + "\"gateway\":\""+ StrGateway2 +"\",\n";	//网关
	strJson = strJson + "\"dns\":\""+ StrDNS2 +"\"\n";	//DNS地址
    strJson +=  "}\n";
    strJson +=  "],\n";

	//参数设置;
	strJson = strJson + "\"hwserver\":\""+ StrHWServer +"\",\n";	//华为服务器IP地址
	strJson = strJson + "\"hwgetpasswd\":\""+ StrHWGetPasswd +"\",\n";	//SNMP GET 密码
	strJson = strJson + "\"hwsetpasswd\":\""+ StrHWSetPasswd +"\",\n";	//SNMP SET 密码

	strJson = strJson + "\"serverurllist\": [\n";		//url列表
	for(i=0;i<4;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"serverurl%d\",\n",i+1); strJson = strJson + str;//名称
		if(i==0)
			strJson = strJson + "\"url\":\""+ StrServerURL1 +"\"\n";	//服务器1推送地址
		if(i==1)
			strJson = strJson + "\"url\":\""+ StrServerURL2 +"\"\n";	//服务器2推送地址
		if(i==2)
			strJson = strJson + "\"url\":\""+ StrServerURL3 +"\"\n";	//服务器3推送地址
		if(i==3)
			strJson = strJson + "\"url\":\""+ StrServerURL4 +"\"\n";	//服务器4推送地址

		if(i<3)
			strJson +=	"},\n";
		else
			strJson +=	"}\n";
	}
	strJson +=	"],\n";

//	strJson = strJson + "\"stationurl\":\""+ StrStationURL +"\",\n";	//控制器接收地址
	strJson = strJson + "\"rsucount\":\""+ StrRSUCount +"\",\n";	//RSU数量
	rsucnt=atoi(StrRSUCount.c_str());
	strJson = strJson + "\"rsulist\": [\n";		//rsu列表
	for(i=0;i<RSUCTL_NUM;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"rsu%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"ip\":\"%s\",\n",StrRSUIP[i].c_str()); strJson = strJson + str;//RSUIP地址
		sprintf(str,"\"port\":\"%s\"\n",StrRSUPort[i].c_str()); strJson = strJson + str;//RSU端口
		if(i<RSUCTL_NUM-1)
			strJson +=	"},\n";
		else
			strJson +=	"}\n";
	}
	strJson +=	"],\n";

	strJson = strJson + "\"vehplatecount\":\""+ StrVehPlateCount +"\",\n";	//识别仪数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	strJson = strJson + "\"vehplatelist\": [\n";		//vehplate列表
	for(i=0;i<VEHPLATE_NUM;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"vehplate%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"ip\":\"%s\",\n",StrVehPlateIP[i].c_str()); strJson = strJson + str;//识别仪地址
		sprintf(str,"\"port\":\"%s\",\n",StrVehPlatePort[i].c_str()); strJson = strJson + str;//识别仪端口
		sprintf(str,"\"key\":\"%s\"\n",StrVehPlateKey[i].c_str()); strJson = strJson + str;//识别仪用户名密码
		if(i<VEHPLATE_NUM-1)
			strJson +=	"},\n";
		else
			strJson +=	"}\n";
	}
	strJson +=	"],\n";

	strJson = strJson + "\"camcount\":\""+ StrCAMCount +"\",\n";	//监控摄像头数量
	strJson = strJson + "\"camlist\": [\n";		//cam列表
	for(i=0;i<CAM_NUM;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"cam%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"ip\":\"%s\",\n",StrCAMIP[i].c_str());strJson = strJson + str;//监控摄像头IP地址
		sprintf(str,"\"port\":\"%s\",\n",StrCAMPort[i].c_str()); strJson = strJson + str;//监控摄像头端口
		sprintf(str,"\"key\":\"%s\"\n",StrCAMKey[i].c_str()); strJson = strJson + str;//监控摄像头用户名密码
		if(i<CAM_NUM-1)
			strJson +=	"},\n";
		else
			strJson +=	"}\n";
	}
	strJson +=	"],\n";

	strJson = strJson + "\"firewarecount\":\""+ StrFireWareCount +"\",\n";	//防火墙数量
	strJson = strJson + "\"firewarelist\": [\n";		//防火墙列表
	for(i=0;i<FIREWARE_NUM;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"fireware%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"ip\":\"%s\",\n",StrFireWareIP[i].c_str());strJson = strJson + str; //防火墙IP
		sprintf(str,"\"getpasswd\":\"%s\",\n",StrFireWareGetPasswd[i].c_str()); strJson = strJson + str;//防火墙get密码
		sprintf(str,"\"setpasswd\":\"%s\"\n",StrFireWareSetPasswd[i].c_str()); strJson = strJson + str;//防火墙set密码
		if(i<FIREWARE_NUM-1)
			strJson +=	"},\n";
		else
			strJson +=	"}\n";
	}
	strJson +=	"],\n";

	strJson = strJson + "\"ipswitchcount\":\""+ StrIPSwitchCount +"\",\n";	//交换机数量
	strJson = strJson + "\"ipswitchlist\": [\n";		//交换机列表
	for(i=0;i<IPSWITCH_NUM;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"ipswitch%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"ip\":\"%s\",\n",StrIPSwitchIP[i].c_str()); strJson = strJson + str;//交换机IP
		sprintf(str,"\"getpasswd\":\"%s\",\n",StrIPSwitchGetPasswd[i].c_str()); strJson = strJson + str;//交换机get密码
		sprintf(str,"\"setpasswd\":\"%s\"\n",StrIPSwitchSetPasswd[i].c_str()); strJson = strJson + str;//交换机set密码
		if(i<IPSWITCH_NUM-1)
			strJson +=	"},\n";
		else
			strJson +=	"}\n";
	}
	strJson +=	"],\n";

	strJson = strJson + "\"do_count\":\""+ StrDoCount +"\",\n"; //do数量
	strJson = strJson + "\"dolist\": [\n";		//do映射列表
	for(i=0;i<SWITCH_COUNT;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"%s\",\n",StrDeviceNameSeq[i].c_str()); strJson = strJson + str;//名称
		sprintf(str,"\"value\":\"%s\"\n",StrDoSeq[i].c_str()); strJson = strJson + str;//设备映射DO
		if(i<SWITCH_COUNT-1)
			strJson +=	"},\n";
		else
			strJson +=	"}\n";
	}
	strJson +=	"],\n";

	strJson = strJson + "\"devicetype\":\""+ StrdeviceType +"\",\n";	//设备型号900~919
	strJson = strJson + "\"hardwareid\":\""+ StrID +"\",\n";	//硬件ID
	strJson = strJson + "\"softversion\":\""+ StrVersionNo +"\",\n";	//主程序版本号920

	strJson = strJson + "\"secsoftversion1\":\""+ stuVMCtl_Param->secSoftVersion[0] +"\",\n";	//副版本号
	strJson = strJson + "\"secsoftversion2\":\""+ stuVMCtl_Param->secSoftVersion[1] +"\",\n";	//副版本号
	strJson = strJson + "\"secsoftversion3\":\""+ stuVMCtl_Param->secSoftVersion[2] +"\",\n";	//副版本号
	strJson = strJson + "\"softdate\":\""+ StrSoftDate +"\"\n"; //版本日期

	strJson +=	"}\n\n\0\0";

//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);
	return true;
}



bool jsonStrRsuWriter(int messagetype,char *json, int *len)
{
	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;

	time_t nSeconds;
	struct tm * pTM;

	time(&nSeconds);
	pTM = localtime(&nSeconds);

	//系统日期和时间,格式: yyyymmddHHMMSS
	sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
			pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
			pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

	strJson +=	"{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n"; //IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
	//
	strJson = strJson + "\"rsu_ip\":\""+ StrRSUIP[0] +"\",\n";	//RSUIP地址
//	strJson = strJson + "\"rsu_port\":\""+ StrRSUPort[0] +"\",\n";	//RSU端口
	sprintf(str,"\"rsuport\":%s,\n",StrRSUPort[0].c_str()); strJson = strJson + str;//RSU端口
	sprintf(str,"\"rsu_vol\":%.3f,\n",stuVA_Meter_Param[0]->phase[11].vln/100.0);strJson = strJson + str;	//电压
	sprintf(str,"\"rsu_amp\":%.3f,\n",stuVA_Meter_Param[0]->phase[11].amp/1000.0); strJson = strJson + str;//电流
	sprintf(str,"\"rsu_id\":\"%02x%02x%02x\",\n",stuRsuData.RSUID[0],stuRsuData.RSUID[1],stuRsuData.RSUID[2]); strJson = strJson + str;//路侧单元编号
	sprintf(str,"\"rsu_manuid\":\"%02x\",\n",stuRsuData.RSUManuID); strJson = strJson + str;//路侧单元厂商代码
	sprintf(str,"\"rsu_version\":\"%02x%02x\",\n",stuRsuData.RSUVersion[0],stuRsuData.RSUVersion[1]); strJson = strJson + str;//路侧单元软件版本号
	sprintf(str,"\"rsu_controlid\":%d,\n",stuRsuData.ControlId); strJson = strJson + str;//当前工作的天线控制盒Id
	sprintf(str,"\"rsu_psamcount\":%d,\n",stuRsuData.PSAMCount); strJson = strJson + str;//PSAM数量
	for(i=0;i<stuRsuData.PSAMCount;i++)
	{
		sprintf(str,"\"rsu_psamid%d\":\"%02x%02x%02x%02x%02x%02x%02x%02x\",\n",i+1,stuRsuData.PSAMInfoN[i].Psam_ID[0],
			stuRsuData.PSAMInfoN[i].Psam_ID[1],stuRsuData.PSAMInfoN[i].Psam_ID[2],stuRsuData.PSAMInfoN[i].Psam_ID[3],
			stuRsuData.PSAMInfoN[i].Psam_ID[4],stuRsuData.PSAMInfoN[i].Psam_ID[5],stuRsuData.PSAMInfoN[i].Psam_ID[6],
			stuRsuData.PSAMInfoN[i].Psam_ID[7]);
		strJson = strJson + str; // PSAM信息
	}
	sprintf(str,"\"rsu_controlcount\":%d,\n",stuRsuControl.ControlCount);strJson = strJson + str;	//控制器数量
	sprintf(str,"\"rsu_controlstatus\":%d,\n",stuRsuControl.ControlStatusN[0]);strJson = strJson + str; //工作状态
	if(stuRsuControl.AntennaCount>0)
	{
		sprintf(str,"\"rsu_antennacount\":%d,\n",stuRsuControl.AntennaCount);strJson = strJson + str;	//天线数量
		for(i=0;i<stuRsuControl.AntennaCount;i++)
		{
			sprintf(str,"\"antenna%d_status\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Status);strJson += str;		//38 天线i 状态
			sprintf(str,"\"antenna%d_power\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Power);strJson += str; //39 天线i 功率
			sprintf(str,"\"antenna%d_channel\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Channel);strJson += str; //40 天线i 信道号
			if(i<stuRsuControl.AntennaCount-1)
				{sprintf(str,"\"antenna%d_controlstate\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Control_state);strJson += str;} 	//41 天线i 开关状态
			else if(i==stuRsuControl.AntennaCount-1)
				{sprintf(str,"\"antenna%d_controlstate\": %d\n", i+1,stuRsuControl.AntennaInfoN[i].Control_state);strJson += str;}		//41 天线i 开关状态
		}
	}
	else
		{sprintf(str,"\"rsu_antennacount\":%d\n",stuRsuControl.AntennaCount);strJson = strJson + str;}	//天线数量

	strJson +=	"}\n\n\0\0";

//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);
	return true;
}


bool jsonStrRsuWriterXY(int messagetype,char *json, int *len)
{
	char str[100],sDateTime[30];
	int i,j,rsucnt;

	time_t nSeconds;
	struct tm * pTM;

	time(&nSeconds);
	pTM = localtime(&nSeconds);

	//系统日期和时间,格式: yyyymmddHHMMSS
	sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
			pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
			pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

	strJson +=	"{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n"; //IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
	sprintf(str,"\"rsucnt\":%s,\n",StrRSUCount.c_str()); strJson = strJson + str;//RSU数量
	rsucnt=atoi(StrRSUCount.c_str());
	strJson = strJson + "\"rsulist\": [\n"; 		//do列表

	for(i=0;i<rsucnt;i++)
	{
		strJson = strJson + "{\n";

		sprintf(str,"\"name\":\"rsu%d\",\n",i+1); strJson = strJson + str;//设备名称
		strJson = strJson + "\"ip\":\""+ StrRSUIP[i] +"\",\n";	//RSUIP地址
		sprintf(str,"\"port\":%s,\n",StrRSUPort[i].c_str()); strJson = strJson + str;//RSU端口
		sprintf(str,"\"volt\":%.3f,\n",stuVA_Meter_Param[0]->phase[i].vln/100.0);strJson = strJson + str;	//电压
		sprintf(str,"\"amp\":%.3f,\n",stuVA_Meter_Param[0]->phase[i].amp/1000.0); strJson = strJson + str;//电流
		sprintf(str,"\"id\":\"%02x%02x%02x\",\n",stuRsuData.RSUID[0],stuRsuData.RSUID[1],stuRsuData.RSUID[2]); strJson = strJson + str;//路侧单元编号
		sprintf(str,"\"manuid\":\"%02x\",\n",stuRsuData.RSUManuID); strJson = strJson + str;//路侧单元厂商代码
		if(stuRsuData.RSUManuID==0xa8)
			{sprintf(str,"\"manufactory\":\"爱特斯\",\n"); strJson = strJson + str;}//路侧单元厂商
		sprintf(str,"\"version\":\"%02x%02x\",\n",stuRsuData.RSUVersion[0],stuRsuData.RSUVersion[1]); strJson = strJson + str;//路侧单元软件版本号
		sprintf(str,"\"controlid\":%d,\n",stuRsuData.ControlId); strJson = strJson + str;//当前工作的天线控制盒Id
		sprintf(str,"\"controlcount\":%d,\n",stuRsuControl.ControlCount);strJson = strJson + str;	//控制器数量
		sprintf(str,"\"controlstatus\":%d,\n",stuRsuControl.ControlStatusN[0]);strJson = strJson + str; //工作状态
		sprintf(str,"\"psamcount\":%d,\n",stuRsuData.PSAMCount); strJson = strJson + str;//PSAM数量
		sprintf(str,"\"antennacount\":%d,\n",stuRsuControl.AntennaCount);strJson = strJson + str;	//天线数量
		if(stuRsuData.PSAMCount>0)
		{
			strJson = strJson + "\"psamlist\": [\n";		//do列表
			for(j=0;j<stuRsuData.PSAMCount;j++)
			{
				strJson = strJson + "{\n";
				sprintf(str,"\"name\": \"psam%d\",\n", j+1);strJson += str;		//名称
				sprintf(str,"\"psamid\":\"%02x%02x%02x%02x%02x%02x%02x%02x\"\n",stuRsuData.PSAMInfoN[j].Psam_ID[0],
					stuRsuData.PSAMInfoN[j].Psam_ID[1],stuRsuData.PSAMInfoN[j].Psam_ID[2],stuRsuData.PSAMInfoN[j].Psam_ID[3],
					stuRsuData.PSAMInfoN[j].Psam_ID[4],stuRsuData.PSAMInfoN[j].Psam_ID[5],stuRsuData.PSAMInfoN[j].Psam_ID[6],
					stuRsuData.PSAMInfoN[j].Psam_ID[7]);
				strJson = strJson + str; // PSAM信息
				if(j==stuRsuData.PSAMCount-1)
					strJson = strJson + "}\n";
				else
					strJson = strJson + "},\n";
			}
			strJson = strJson + "],\n";
		}
		if(stuRsuControl.AntennaCount>0)
		{
			strJson = strJson + "\"antennalist\": [\n";		//天线头列表
			for(j=0;j<stuRsuControl.AntennaCount;j++)
			{
				strJson = strJson + "{\n";
				sprintf(str,"\"name\": \"antenna%d\",\n", j+1);strJson += str;		//名称
				sprintf(str,"\"status\": %d,\n", stuRsuControl.AntennaInfoN[j].Status);strJson += str;		//38 天线i 状态
				sprintf(str,"\"power\": %d,\n", stuRsuControl.AntennaInfoN[j].Power);strJson += str; //39 天线i 功率
				sprintf(str,"\"channel\": %d,\n", stuRsuControl.AntennaInfoN[j].Channel);strJson += str; //40 天线i 信道号
				sprintf(str,"\"controlstate\": %d\n", stuRsuControl.AntennaInfoN[j].Control_state);strJson += str;	//41 天线i 开关状态
				if(j==stuRsuControl.AntennaCount-1)
					strJson = strJson + "}\n";
				else
					strJson = strJson + "},\n";
			}
			strJson = strJson + "]\n";
		}

		if(i==rsucnt-1)
			strJson = strJson + "}\n";
		else
			strJson = strJson + "},\n";
	}
	strJson = strJson + "]\n";
	strJson = strJson + "}\n";

//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);
	return true;
}

bool jsonStrVehPlateWriter(int messagetype,char *json, int *len)
{
	char str[100],sDateTime[30];
	int i,j;
	int vehplatecnt;

	time_t nSeconds;
	struct tm * pTM;

	time(&nSeconds);
	pTM = localtime(&nSeconds);

	//系统日期和时间,格式: yyyymmddHHMMSS
	sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
			pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
			pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

	strJson +=	"{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n"; //IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
	//
	strJson = strJson + "\"vehplatecnt\":\""+ StrVehPlateCount +"\",\n";	//车牌识别数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	strJson = strJson + "\"vehplatelist\":[\n";
	for(i=0;i<vehplatecnt;i++)
	{
		strJson = strJson + "{\n";

		sprintf(str,"\"name\":\"vehplate%d\",\n",i+1);strJson+=str;	//摄相机名称
		sprintf(str,"\"ip\":\"%s\",\n",StrVehPlateIP[i].c_str());strJson+=str;	//摄相机IP
		sprintf(str,"\"port\":\"%s\",\n",StrVehPlatePort[i].c_str());strJson+=str;	//识别仪端口
		sprintf(str,"\"key\":\"%s\",\n",StrVehPlateKey[i].c_str());strJson+=str;	//用户名密码
		sprintf(str,"\"status\":%s,\n",mTIPcamState[i].statuscode.c_str());strJson+=str;	//摄相机状态编码 0-正常 1-异常
		if(atoi(mTIPcamState[i].statuscode.c_str())==0)
			{sprintf(str,"\"statusdesc\":\"正常\",\n");strJson+=str;	}//工作状态描述
		else
			{sprintf(str,"\"statusdesc\":\"异常\",\n");strJson+=str;	}	//工作状态描述
		sprintf(str,"\"errcode\":%s,\n",mTIPcamState[i].errcode.c_str());strJson+=str;	//正常时为0，状态异常时为厂商自定义的错误代码
		if(mTIPcamState[i].statuscode=="1")
			{sprintf(str,"\"errdesc\":\"网络断开\",\n");strJson+=str	;}	//错误状态描述
		else
			{sprintf(str,"\"errdesc\":\"\",\n");strJson+=str;}	//错误状态描述
		sprintf(str,"\"factoryid\":%s,\n",mTIPcamState[i].factoryid.c_str());strJson+=str	;	//摄相机厂商1-宇视2-海康3-大华4-中威5-哈工大6-华为7-北京智通 8-北京信路威
		if(atoi(mTIPcamState[i].factoryid.c_str())==1)
			{sprintf(str,"\"factoryname\":\"宇视\",\n");strJson+=str;}	//摄相机厂商1-宇视2-海康3-大华4-中威5-哈工大6-华为7-北京智通 8-北京信路威
		else
			{sprintf(str,"\"factoryname\":\"\",\n");strJson+=str;}	//摄相机厂商1-宇视2-海康3-大华4-中威5-哈工大6-华为7-北京智通 8-北京信路威
		sprintf(str,"\"devicemodel\":%s,\n",mTIPcamState[i].devicemodel.c_str());strJson+=str;	//摄相机的设备型号
		sprintf(str,"\"softversion\":%s,\n",mTIPcamState[i].softversion.c_str());strJson+=str;	//摄相机的软件版本号
		sprintf(str,"\"filllight\":%s,\n",mTIPcamState[i].filllight.c_str());strJson+=str;	//闪光灯状态 0:正常，1:异常
		sprintf(str,"\"temperature\":%s\n",mTIPcamState[i].temperature.c_str());strJson+=str;//摄像枪温度（摄氏度）

		if(i==vehplatecnt-1)
			strJson = strJson + "}\n";
		else
			strJson = strJson + "},\n";
	}
	strJson = strJson + "]\n";

	strJson +=	"}\n\n\0\0";

//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);
	return true;
}

bool jsonStrUpsWriter(int messagetype,char *pstrUpsPam, char *json, int *len)
{
	UPS_PARAMS *pParm=(UPS_PARAMS *)pstrUpsPam;
	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
	// UPS数据结构体
	// 输入数据
	sprintf(str,"%d",pParm->in_phase_num);strJson = strJson + "\"in_phase_num\":"+ str +",\n";	//相数 40
	sprintf(str,"%.1f",pParm->in_freq/10.0);strJson = strJson + "\"in_freq\":"+ str +",\n";	//交流输入频率	41 ×100
	sprintf(str,"%.1f",pParm->volt_Ain/10.0);strJson = strJson + "\"volt_ain\":"+ str +",\n";	//交流输入相电压A 42 ×10
	sprintf(str,"%.1f",pParm->volt_Bin/10.0);strJson = strJson + "\"volt_bin\":"+ str +",\n";	//交流输入相电压B 43 ×10
	sprintf(str,"%.1f",pParm->volt_Cin/10.0);strJson = strJson + "\"volt_cin\":"+ str +",\n";	//交流输入相电压C 44 ×10
	sprintf(str,"%.1f",pParm->amp_Ain/10.0);strJson = strJson + "\"amp_ain\":"+ str +",\n";	//交流输入相电流A      45 ×10
	sprintf(str,"%.1f",pParm->amp_Bin/10.0);strJson = strJson + "\"amp_bin\":"+ str +",\n";	//交流输入相电流B      46 ×10
	sprintf(str,"%.1f",pParm->amp_Cin/10.0);strJson = strJson + "\"amp_cin\":"+ str +",\n";	//交流输入相电流C      47 ×10
	sprintf(str,"%.1f",pParm->fact_Ain/10.0);strJson = strJson + "\"fact_ain\":"+ str +",\n";	//功率因素
	sprintf(str,"%.1f",pParm->fact_Bin/10.0);strJson = strJson + "\"fact_bin\":"+ str +",\n";	//功率因素
	sprintf(str,"%.1f",pParm->fact_Cin/10.0);strJson = strJson + "\"fact_cin\":"+ str +",\n";	//功率因素
	sprintf(str,"%.1f",pParm->bypass_voltA/10.0);strJson = strJson + "\"bypass_volta\":"+ str +",\n";	//旁路电压A
	sprintf(str,"%.1f",pParm->bypass_voltB/10.0);strJson = strJson + "\"bypass_voltb\":"+ str +",\n";	//旁路电压B
	sprintf(str,"%.1f",pParm->bypass_voltC/10.0);strJson = strJson + "\"bypass_voltc\":"+ str +",\n";	//旁路电压C
	sprintf(str,"%.1f",pParm->bypass_freq/10.0);strJson = strJson + "\"bypass_freq\":"+ str +",\n";	//旁路频率

	//输出数据
	sprintf(str,"%d",pParm->out_phase_num);strJson = strJson + "\"out_phase_num\":"+ str +",\n";	//输出相数 48
	sprintf(str,"%.1f",pParm->out_freq/10.0);strJson = strJson + "\"out_freq\":"+ str +",\n";	//UPS交流输出频率 49		×100
	sprintf(str,"%.1f",pParm->volt_Aout/10.0);strJson = strJson + "\"volt_aout\":"+ str +",\n";	//交流输出相电压A 50 ×10
	sprintf(str,"%.1f",pParm->volt_Bout/10.0);strJson = strJson + "\"volt_bout\":"+ str +",\n";	//交流输出相电压B 51 ×10
	sprintf(str,"%.1f",pParm->volt_Cout/10.0);strJson = strJson + "\"volt_cout\":"+ str +",\n";	//交流输出相电压C 52 	 ×10
	sprintf(str,"%.1f",pParm->amp_Aout/10.0);strJson = strJson + "\"amp_aout\":"+ str +",\n";	//交流输出相电流A 53 ×10
	sprintf(str,"%.1f",pParm->amp_Bout/10.0);strJson = strJson + "\"amp_bout\":"+ str +",\n";	//交流输出相电流B 54 ×10
	sprintf(str,"%.1f",pParm->amp_Cout/10.0);strJson = strJson + "\"amp_cout\":"+ str +",\n";	//交流输出相电流C 55 ×10
	sprintf(str,"%.1f",pParm->kw_Aout/10.0);strJson = strJson + "\"kw_aout\":"+ str +",\n";	//有功
	sprintf(str,"%.1f",pParm->kw_Bout/10.0);strJson = strJson + "\"kw_bout\":"+ str +",\n";	//有功
	sprintf(str,"%.1f",pParm->kw_Cout/10.0);strJson = strJson + "\"kw_cout\":"+ str +",\n";	//有功
	sprintf(str,"%.1f",pParm->kva_Aout/10.0);strJson = strJson + "\"kva_aout\":"+ str +",\n";	// 视在
	sprintf(str,"%.1f",pParm->kva_Bout/10.0);strJson = strJson + "\"kva_bout\":"+ str +",\n";	//视在
	sprintf(str,"%.1f",pParm->kva_Cout/10.0);strJson = strJson + "\"kva_cout\":"+ str +",\n";	//视在
	sprintf(str,"%.1f",pParm->load_Aout/10.0);strJson = strJson + "\"load_aout\":"+ str +",\n";	//负载
	sprintf(str,"%.1f",pParm->load_Bout/10.0);strJson = strJson + "\"load_bout\":"+ str +",\n";	//负载
	sprintf(str,"%.1f",pParm->load_Cout/10.0);strJson = strJson + "\"load_cout\":"+ str +",\n";	//负载

	//电池参数
	sprintf(str,"%d",pParm->running_day);strJson = strJson + "\"running_day\":"+ str +",\n";	//UPS运行时间 56 天
	sprintf(str,"%.1f",pParm->battery_volt/10.0);strJson = strJson + "\"battery_volt\":"+ str +",\n";	//UPS电池电压	57 ×10
	sprintf(str,"%.3f",pParm->amp_charge/100.0);strJson = strJson + "\"amp_charge\":"+ str +",\n";	//UPS充电电流 58 ×100
	sprintf(str,"%.3f",pParm->amp_discharge/100.0);strJson = strJson + "\"amp_discharge\":"+ str +",\n";	//UPS放电电流
	sprintf(str,"%.1f",pParm->battery_left/10.0);strJson = strJson + "\"battery_left\":"+ str +",\n";	//UPS电池后备时间 59 ×10，分钟
	sprintf(str,"%.1f",pParm->battery_tmp/10.0);strJson = strJson + "\"battery_tmp\":"+ str +",\n";	// 环境温度 60 ×10
	sprintf(str,"%.1f",pParm->battery_capacity/10.0);strJson = strJson + "\"battery_capacity\":"+ str +",\n";	//电池当前容量 61 ×100%
	sprintf(str,"%d",pParm->battery_dischg_times);strJson = strJson + "\"battery_dischg_times\":"+ str +",\n";	//电池放电次数 62

	//USP状态参数
	sprintf(str,"%d",pParm->supply_out_status);strJson = strJson + "\"supply_out_status\":"+ str +",\n";	//输出供电状态 63
	sprintf(str,"%d",pParm->supply_in_status);strJson = strJson + "\"supply_in_status\":"+ str +",\n";	//输入供电状态 64
	sprintf(str,"%d",pParm->battery_status);strJson = strJson + "\"battery_status\":"+ str +",\n";	//电池状态 65

	//USP告警
	sprintf(str,"%d",pParm->main_abnormal);strJson = strJson + "\"main_abnormal\":"+ str +",\n";	//主路异常 66 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->system_overtemp);strJson = strJson + "\"system_overtemp\":"+ str +",\n";	//系统过温, 67 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->sysbat_low_prealarm);strJson = strJson + "\"sysbat_low_prealarm\":"+ str +",\n";	//系统电池电量低预告警,68 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->rectifier_overload);strJson = strJson + "\"rectifier_overload\":"+ str +",\n";	//整流器过载,69 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->inverter_overload);strJson = strJson + "\"inverter_overload\":"+ str +",\n";	//逆变器过载,70 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->bypass_abnomal);strJson = strJson + "\"bypass_abnomal\":"+ str +",\n";	//旁路异常,71 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->battery_low_prealarm);strJson = strJson + "\"battery_low_prealarm\":"+ str +",\n";	//电池电压低,72 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->battery_abnomal);strJson = strJson + "\"battery_abnomal\":"+ str +",\n";	//电池电压异常,73 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->battery_overtemp);strJson = strJson + "\"battery_overtemp\":"+ str +",\n";	//电池过温,74 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->fan_abnormal);strJson = strJson + "\"fan_abnormal\":"+ str +",\n";	//风扇故障,75 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->shutdown_alarm);strJson = strJson + "\"shutdown_alarm\":"+ str +",\n";	//紧急关机告警,76 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->maintain_status);strJson = strJson + "\"maintain_status\":"+ str +",\n";	//维修模式,77 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->inverter_supply);strJson = strJson + "\"inverter_supply\":"+ str +",\n";	//电池逆变供电,78 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->bypass_supply);strJson = strJson + "\"bypass_supply\":"+ str +"\n";	//旁路供电,79 0x00：正常 0xF0：异常
    strJson +=  "}\n\n\0\0";


//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

	return true;
}


bool SetjsonReceiveOKStr(int messagetype,char *json, int *len)
{
	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;
    strJson +=  "{\n";
	sprintf(str,"%d",messagetype);strJson = strJson + "\"messagetype\":"+ str +",\n";	//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间

	strJson = strJson + "\"receive\": \"OK\"\n";

	strJson +=	"}\n\n\0\0";

//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

	return true;
}

bool jsonStrAirCondWriter(int messagetype,char *pstPam, char *json, int *len)
{
	AIRCOND_PARAM *pParm=(AIRCOND_PARAM *)pstPam;
	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间

	sprintf(str,"%d",pParm->aircondset);strJson = strJson + "\"aircondset\":"+ str +",\n";	//空调关机//1220					1
	sprintf(str,"%d",pParm->aircoldstartpoint);strJson = strJson + "\"aircoldstartpoint\":"+ str +",\n";	//空调制冷点//1221 			50
	sprintf(str,"%d",pParm->aircoldloop);strJson = strJson + "\"aircoldloop\":"+ str +",\n";	//空调制冷回差//1222					10
	sprintf(str,"%d",pParm->airhotstartpoint);strJson = strJson + "\"airhotstartpoint\":"+ str +",\n";	//空调制热点//1223 			15
	sprintf(str,"%d",pParm->airhotloop);strJson = strJson + "\"airhotloop\":"+ str +"\n";	//空调制热回差//1224					10

    strJson +=  "}\n\n\0\0";

//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

	return true;
}


//19回路电压电流开关状态
bool jsonStrSwitchStatusWriter(int messagetype, char *json, int *len)
{
	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;
	int va_meter_bd,phase,docount;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间

	strJson = strJson + "\"vehplate_count\": \"" + StrVehPlateCount + "\",\n";	//识别仪数量
	strJson = strJson + "\"rsu_count\": \"" + StrRSUCount + "\",\n";	//RSU数量
//	strJson = strJson + "\"antenna_count\": " + stuRsuControl.AntennaCount + ",\n";	//天线头数量

	docount=atoi(StrDoCount.c_str());
	for(i=0;i<docount;i++) //开关数量
	{
		va_meter_bd=i/VA_METER_BD_MAX_NUM;
		phase=i%VA_METER_BD_MAX_NUM;
		if (StrAdrrVAMeter[va_meter_bd].length() != 0)
		{
			if(stuVA_Meter_Param[va_meter_bd]->phase[phase].vln/100.0<5.0)
				sprintf(str,"\"do%d_status\":0,\n",i+1); //断电
			else
				sprintf(str,"\"do%d_status\":1,\n",i+1); //通电
			strJson = strJson + str;
			sprintf(str,"\"do%d_vol\":%.3f,\n",i+1,stuVA_Meter_Param[va_meter_bd]->phase[phase].vln/100.0); strJson = strJson + str;//电压
			if(i==docount-1)
			{
				sprintf(str,"\"do%d_amp\":%.3f\n",i+1,stuVA_Meter_Param[va_meter_bd]->phase[phase].amp/1000.0); strJson = strJson + str;//电流
			}
			else
			{
				sprintf(str,"\"do%d_amp\":%.3f,\n",i+1,stuVA_Meter_Param[va_meter_bd]->phase[phase].amp/1000.0); strJson = strJson + str;//电流
			}

		}
	}
/*	for(i=0;i<VEHPLATE_NUM;i++)	//前面用作车牌识别
	{
		va_meter_bd=i/VA_METER_BD_MAX_NUM;
		phase=i%VA_METER_BD_MAX_NUM;
		if(stuVA_Meter_Param[va_meter_bd].phase[phase].vln/100.0<5.0)
			sprintf(str,"\"vehplate%d\":0,\n",i+1);	//断电
		else
			sprintf(str,"\"vehplate%d\":1,\n",i+1);	//通电
		strJson = strJson + str;
		sprintf(str,"\"vehplate%d_vol\":%.3f,\n",i+1,stuVA_Meter_Param[va_meter_bd].phase[phase].vln/100.0); strJson = strJson + str;//电压
		sprintf(str,"\"vehplate%d_amp\":%.3f,\n",i+1,stuVA_Meter_Param[va_meter_bd].phase[phase].amp/1000.0); strJson = strJson + str;//电流
	}
	for(i=0;i<RSUCTL_NUM;i++)	//RSU控制器
	{
		if(stuVA_Meter_Param[0].phase[i].vln/100.0<5.0)
			sprintf(str,"\"rsucontrlor%d\":0,\n",i+1-11);	//断电
		else
			sprintf(str,"\"rsucontrlor%d\":1,\n",i+1-11);	//通电
		strJson = strJson + str;
		sprintf(str,"\"rsucontrlor%d_vol\":%.3f,\n",i+1-11,stuVA_Meter_Param[0].phase[i].vln/100.0); strJson = strJson + str;//电压
		sprintf(str,"\"rsucontrlor%d_amp\":%.3f,\n",i+1-11,stuVA_Meter_Param[0].phase[i].amp/1000.0); strJson = strJson + str;//电流
	}*/
//	sprintf(str,"\"frontdoorlock\":%d,\n",lockerHw_Param[0]->status); strJson = strJson + str;//前门锁
//	sprintf(str,"\"backdoorlock\":%d,\n",lockerHw_Param[1]->status); strJson = strJson + str;//后门锁
//	sprintf(str,"\"sidedoorlock\":%d,\n",lockerHw_Param[2]->status); strJson = strJson + str;//侧门锁
//	sprintf(str,"\"rightsidedoorlock\":%d\n",lockerHw_Param[3]->status); strJson = strJson + str;//侧门锁

	strJson +=	"}\n\n\0\0";

//	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

	return true;
}


//19回路电压电流开关状态-新粤
bool jsonStrSwitchStatusWriterXY(int messagetype, char *json, int *len)
{
	char str[100],sDateTime[30];
	int i,j,docount;
	int va_meter_bd,phase;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间

	strJson = strJson + "\"do_count\": \"" + StrDoCount + "\",\n";	//do数量
	strJson = strJson + "\"dolist\": [\n"; 		//do列表

	docount=atoi(StrDoCount.c_str());
	for(i=0;i<docount;i++)
	{
		strJson = strJson + "{\n";
		for(j=0;j<docount;j++)
		{
//printf("StrDoSeq[%d]=%s,%d\n",StrDoSeq[j].c_str(),atoi(StrDoSeq[j].c_str());
			if(atoi(StrDoSeq[j].c_str())==i+1)
				break;
		}
//		if(j!=docount)
			strJson = strJson + "\"name\": \"" + StrDeviceNameSeq[j] + "\",\n";	//设备名称
//		else
//			strJson = strJson + "\"name\": \"\",\n";	//设备名称
		va_meter_bd=i/VA_METER_BD_MAX_NUM;
		phase=i%VA_METER_BD_MAX_NUM;
		if (StrAdrrVAMeter[va_meter_bd].length() != 0)
		{
			if(stuVA_Meter_Param[va_meter_bd]->phase[phase].vln/100.0<5.0)
				sprintf(str,"\"status\":0,\n"); //断电
			else
				sprintf(str,"\"status\":1,\n"); //通电
			strJson = strJson + str;
			sprintf(str,"\"volt\":%.3f,\n",stuVA_Meter_Param[va_meter_bd]->phase[phase].vln/100.0); strJson = strJson + str;//电压
			sprintf(str,"\"amp\":%.3f\n",stuVA_Meter_Param[va_meter_bd]->phase[phase].amp/1000.0); strJson = strJson + str;//电流
		}
		if(i==docount-1)
			strJson = strJson + "}\n";
		else
			strJson = strJson + "},\n";
	}
	strJson = strJson + "]\n";
	strJson = strJson + "}\n";

	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

	return true;
}


bool jsonStrHWCabinetWriter(int messagetype,char *pstPam, char *json, int *len)
{
	char str[100],sDateTime[30];
	int i,j;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间

    pthread_mutex_lock(&snmpoidMutex);

	//华为机柜字段
    strJson = strJson + "\"hwacbgroupbatvolt\": " + HUAWEIDevValue.strhwAcbGroupBatVolt.c_str() + ",\n";	//锂电电池电压 214
    strJson = strJson + "\"hwacbgroupbatcurr\": " + HUAWEIDevValue.strhwAcbGroupBatCurr.c_str() + ",\n";	//锂电电池电流 215
    strJson = strJson + "\"hwacbgrouptotalcapacity\": " + HUAWEIDevValue.strhwAcbGroupTotalCapacity.c_str() + ",\n";	//锂电电池总容量 216
    strJson = strJson + "\"hwacbgrouptotalremaincapacity\": " + HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str() + ",\n";	//锂电电池剩余容量 217
    strJson = strJson + "\"hwacbgroupbackuptime\": " + HUAWEIDevValue.strhwAcbGroupBackupTime.c_str() + ",\n";	//电池备电时长 218
    strJson = strJson + "\"hwacbgroupbatsoh\": " + HUAWEIDevValue.strhwAcbGroupBatSoh.c_str() + ",\n";	//电池SOH 219
    strJson = strJson + "\"hwaporablvoltage\": " + HUAWEIDevValue.strhwApOrAblVoltage.c_str() + ",\n";	//A/AB电压 220
    strJson = strJson + "\"hwbporbclvoltage\": " + HUAWEIDevValue.strhwBpOrBclVoltage.c_str() + ",\n";	//B/BC电压 221
    strJson = strJson + "\"hwcporcalvoltage\": " + HUAWEIDevValue.strhwCpOrCalVoltage.c_str() + ",\n";	//C/CA电压 222
    strJson = strJson + "\"hwaphasecurrent\": " + HUAWEIDevValue.strhwAphaseCurrent.c_str() + ",\n";	//A相电流 223
    strJson = strJson + "\"hwbphasecurrent\": " + HUAWEIDevValue.strhwBphaseCurrent.c_str() + ",\n";	//B相电流 224
    strJson = strJson + "\"hwcphasecurrent\": " + HUAWEIDevValue.strhwCphaseCurrent.c_str() + ",\n";	//C相电流 225
    strJson = strJson + "\"hwdcoutputvoltage\": " + HUAWEIDevValue.strhwDcOutputVoltage.c_str() + ",\n";	//DC输出电压 226
    strJson = strJson + "\"hwdcoutputcurrent\": " + HUAWEIDevValue.strhwDcOutputCurrent.c_str() + ",\n";	//DC输出电流 227
    strJson = strJson + "\"hwenvtemperature\": " + HUAWEIDevValue.strhwEnvTemperature[0].c_str() + ",\n";	//环境温度值 228
    strJson = strJson + "\"hwenvtemperature2\": " + HUAWEIDevValue.strhwEnvTemperature[1].c_str() + ",\n";	//环境温度值 228
    strJson = strJson + "\"hwenvhumidity\": " + HUAWEIDevValue.strhwEnvHumidity[0].c_str() + ",\n";	//环境湿度值 229
    strJson = strJson + "\"hwenvhumidity2\": " + HUAWEIDevValue.strhwEnvHumidity[1].c_str() + ",\n";	//环境湿度值 229
//    strJson = strJson + "\"hwdcairctrlmode\": " + HUAWEIDevValue.strhwDcAirCtrlMode[0].c_str() + ",\n";	//空调控制模式 230
//    strJson = strJson + "\"hwdcairctrlmode2\": " + HUAWEIDevValue.strhwDcAirCtrlMode[1].c_str() + ",\n";	//空调控制模式 230
    strJson = strJson + "\"hwdcairrunstatus\": " + HUAWEIDevValue.strhwDcAirRunStatus[0].c_str() + ",\n";	//空调运行状态 231
    strJson = strJson + "\"hwdcairrunstatus2\": " + HUAWEIDevValue.strhwDcAirRunStatus[1].c_str() + ",\n";	//空调运行状态 231
    strJson = strJson + "\"hwdcaircompressorrunstatus\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus[0].c_str() + ",\n";	//空调压缩机运行状态 232
    strJson = strJson + "\"hwdcaircompressorrunstatus2\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus[1].c_str() + ",\n";	//空调压缩机运行状态 232
    strJson = strJson + "\"hwdcairinnrfanspeed\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed[0].c_str() + ",\n";	//空调内机转速 233
    strJson = strJson + "\"hwdcairinnrfanspeed2\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed[1].c_str() + ",\n";	//空调内机转速 233
    strJson = strJson + "\"hwdcairouterfanspeed\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed[0].c_str() + ",\n";	//空调外风机转速 234
    strJson = strJson + "\"hwdcairouterfanspeed2\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed[1].c_str() + ",\n";	//空调外风机转速 234
    strJson = strJson + "\"hwdcaircompressorruntime\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime[0].c_str() + ",\n";	//空调压缩机运行时间 235
    strJson = strJson + "\"hwdcaircompressorruntime2\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime[1].c_str() + ",\n";	//空调压缩机运行时间 235
    strJson = strJson + "\"hwdcairenterchanneltemp\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp[0].c_str() + ",\n";	//空调回风口温度 236
    strJson = strJson + "\"hwdcairenterchanneltemp2\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp[1].c_str() + ",\n";	//空调回风口温度 236
    strJson = strJson + "\"hwdcairpowerontemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint[0].c_str() + ",\n";	//空调开机温度点 237
    strJson = strJson + "\"hwdcairpowerontemppoint2\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint[1].c_str() + ",\n";	//空调开机温度点 237
    strJson = strJson + "\"hwdcairpowerofftemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint[0].c_str() + ",\n";	//空调关机温度点 238
    strJson = strJson + "\"hwdcairpowerofftemppoint2\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint[1].c_str() + ",\n";	//空调关机温度点 238
    strJson = strJson + "\"hwenvtempalarmtraps\": " + HUAWEIDevAlarm.hwEnvTempAlarmTraps.c_str() + ",\n";	//设备柜环境温度告警 239
    strJson = strJson + "\"hwenvtempalarmtraps2\": " + HUAWEIDevAlarm.hwEnvTempAlarmTraps2.c_str() + ",\n";	//电池柜环境温度告警 239
    strJson = strJson + "\"hwenvhumialarmresumetraps\": " + HUAWEIDevAlarm.hwEnvHumiAlarmTraps.c_str() + ",\n";	//设备柜环境湿度告警 240
    strJson = strJson + "\"hwenvhumialarmresumetraps2\": " + HUAWEIDevAlarm.hwEnvHumiAlarmTraps2.c_str() + ",\n";	//电池柜环境湿度告警 240
    strJson = strJson + "\"hwdooralarmtraps\": " + HUAWEIDevAlarm.hwDoorAlarmTraps.c_str() + ",\n";	//门禁告警 241
    strJson = strJson + "\"hwwateralarmtraps\": " + HUAWEIDevAlarm.hwWaterAlarmTraps.c_str() + ",\n";	//水浸告警 242
    strJson = strJson + "\"hwsmokealarmtraps\": " + HUAWEIDevAlarm.hwSmokeAlarmTraps.c_str() + ",\n";	//烟雾告警 243
    strJson = strJson + "\"hwspdalarmtraps\": " + HUAWEIDevAlarm.hwSpareDigitalAlarmTraps.c_str() + ",\n";	//防雷器故障告警 244
    strJson = strJson + "\"hwair_cond_infan_alarm\": " + HUAWEIDevAlarm.hwair_cond_infan_alarm.c_str() + ",\n";	//空调内风机故障 245
    strJson = strJson + "\"hwair_cond_outfan_alarm\": " + HUAWEIDevAlarm.hwair_cond_outfan_alarm.c_str() + ",\n";	//空调外风机故障 246
    strJson = strJson + "\"hwair_cond_comp_alarm\": " + HUAWEIDevAlarm.hwair_cond_comp_alarm.c_str() + ",\n";	//空调压缩机故障 247
    strJson = strJson + "\"hwair_cond_return_port_sensor_alarm\": " + HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm.c_str() + ",\n";	//空调回风口传感器故障 248
    strJson = strJson + "\"hwair_cond_evap_freezing_alarm\": " + HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm.c_str() + ",\n";	//空调蒸发器冻结 249
    strJson = strJson + "\"hwair_cond_freq_high_press_alarm\": " + HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm.c_str() + ",\n";	//空调频繁高压力 250
    strJson = strJson + "\"hwair_cond_comm_fail_alarm\": " + HUAWEIDevAlarm.hwair_cond_comm_fail_alarm.c_str() + ",\n";	//空调通信失败告警 251
	//新增加的状态
	//设备信息
    strJson = strJson + "\"hwmonequipsoftwareversion\": \"" + HUAWEIDevValue.strhwMonEquipSoftwareVersion.c_str() + "\",\n";	//软件版本
    strJson = strJson + "\"hwmonequipmanufacturer\": \"" + HUAWEIDevValue.strhwMonEquipManufacturer.c_str() + "\",\n";	//设备生产商
	//锂电(新增加)
    strJson = strJson + "\"hwacbgrouptemperature\": " + HUAWEIDevValue.strhwAcbGroupTemperature.c_str() + ",\n";	//电池温度
    strJson = strJson + "\"hwacbgroupovercurthr\": " + HUAWEIDevValue.strhwAcbGroupOverCurThr.c_str() + ",\n";	//充电过流告警点
    strJson = strJson + "\"hwacbgrouphightempthr\": " + HUAWEIDevValue.strhwAcbGroupHighTempThr.c_str() + ",\n";	//高温告警点
    strJson = strJson + "\"hwacbgrouplowtempth\": " + HUAWEIDevValue.strhwAcbGroupLowTempTh.c_str() + ",\n";	//低温告警点
    strJson = strJson + "\"hwacbgroupdodtoacidbattery\": " + HUAWEIDevValue.strhwAcbGroupDodToAcidBattery.c_str() + ",\n";	//锂电放电DOD
	//开关电源(新增加)
    strJson = strJson + "\"hwsetacsuppervoltlimit\": " + HUAWEIDevValue.strhwSetAcsUpperVoltLimit.c_str() + ",\n";	//AC过压点设置
    strJson = strJson + "\"hwsetacslowervoltlimit\": " + HUAWEIDevValue.strhwSetAcsLowerVoltLimit.c_str() + ",\n";	//AC欠压点设置
    strJson = strJson + "\"hwsetdcsuppervoltlimit\": " + HUAWEIDevValue.strhwSetDcsUpperVoltLimit.c_str() + ",\n";	//设置DC过压点
    strJson = strJson + "\"hwsetdcslowervoltlimit\": " + HUAWEIDevValue.strhwSetDcsLowerVoltLimit.c_str() + ",\n";	//设置DC欠压点
    strJson = strJson + "\"hwsetlvdvoltage\": " + HUAWEIDevValue.strhwSetLvdVoltage.c_str() + ",\n";	//设置LVD电压
	//环境传感器(新增加)
    strJson = strJson + "\"hwsetenvtempupperlimit\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit[0].c_str() + ",\n";	//环境温度告警上限
    strJson = strJson + "\"hwsetenvtempupperlimit2\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit[1].c_str() + ",\n";	//环境温度告警上限
    strJson = strJson + "\"hwsetenvtemplowerlimit\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit[0].c_str() + ",\n";	//环境温度告警下限
    strJson = strJson + "\"hwsetenvtemplowerlimit2\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit[1].c_str() + ",\n";	//环境温度告警下限
//    strJson = strJson + "\"hwsetenvtempultrahightempthreshold\": " + HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold.c_str() + ",\n";	//环境高高温告警点
    strJson = strJson + "\"hwsetenvhumidityupperlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[0].c_str() + ",\n";	//环境湿度告警上限
    strJson = strJson + "\"hwsetenvhumidityupperlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[1].c_str() + ",\n";	//环境湿度告警上限
    strJson = strJson + "\"hwsetenvhumiditylowerlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[0].c_str() + ",\n";	//环境湿度告警下限
    strJson = strJson + "\"hwsetenvhumiditylowerlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[1].c_str() + ",\n";	//环境湿度告警下限
	//直流空调(新增加)
    strJson = strJson + "\"hwdcairruntime\": " + HUAWEIDevValue.strhwDcAirRunTime[0].c_str() + ",\n";	//设备柜空调运行时间
    strJson = strJson + "\"hwdcairruntime2\": " + HUAWEIDevValue.strhwDcAirRunTime[1].c_str() + ",\n";	//电池柜空调运行时间
    strJson = strJson + "\"hwcoolingdevicesmode\": " + HUAWEIDevValue.strhwCoolingDevicesMode.c_str() + ",\n";	//温控模式
    //防火墙
    strJson = strJson + "\"hwentitycpuusage\": " + HUAWEIDevValue.strhwEntityCpuUsage.c_str() + ",\n";	//CPU
    strJson = strJson + "\"hwentitymemusage\": " + HUAWEIDevValue.strhwEntityMemUsage.c_str() + ",\n";	//内存使用率
    strJson = strJson + "\"hwentitytemperature\": " + HUAWEIDevValue.strhwEntityTemperature.c_str() + ",\n";	//温度
    //交换机
    strJson = strJson + "\"hwswitchentitycpuusage\": " + HUAWEIDevValue.strhwswitchEntityCpuUsage.c_str() + ",\n";	//CPU
    strJson = strJson + "\"hwswitchentitymemusage\": " + HUAWEIDevValue.strhwswitchEntityMemUsage.c_str() + ",\n";	//内存使用率
    strJson = strJson + "\"hwswitchentitytemperature\": " + HUAWEIDevValue.strhwswitchEntityTemperature.c_str() + "\n";	//温度

    pthread_mutex_unlock(&snmpoidMutex);

    strJson +=  "}\n\0";

	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

}


void SetjsonIPSwitchStatusStr(int messagetype,string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,ipswitchcnt;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
	strJson = strJson + "\"ipswitchcnt\": \"" + StrIPSwitchCount + "\",\n";	//交换机数量
	strJson = strJson + "\"ipswitchlist\": [\n";	//交换机列表
	ipswitchcnt=atoi(StrIPSwitchCount.c_str());
	for(i=0;i<ipswitchcnt;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"ipswitch%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"factoryname\":\"华为\",\n"); strJson = strJson + str;//生产商
		sprintf(str,"\"devicemodel\":\"\",\n"); strJson = strJson + str;//设备型号
		strJson = strJson + strswitchjson;								//交换机网络数据
		strJson +=	"}\n";
	}
    strJson +=  "]\n";
	strJson +=	"}\n";

	pthread_mutex_unlock(&snmpoidMutex);

	mstrjson = strJson;
	//*len=strJson.length();
	//memcpy(json,(char*)strJson.c_str(),*len);

}


void SetjsonFireWallStatusStr(int messagetype,string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,firewarecnt;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
	strJson = strJson + "\"firewarecnt\": \"" + StrFireWareCount + "\",\n";	//防火墙数量
	strJson = strJson + "\"firewarelist\": [\n";	//防火墙列表
	firewarecnt=atoi(StrFireWareCount.c_str());
	for(i=0;i<firewarecnt;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"fireware%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"factoryname\":\"华为\",\n"); strJson = strJson + str;//生产商
		sprintf(str,"\"devicemodel\":\"\",\n"); strJson = strJson + str;//设备型号
		strJson = strJson + strfirewalljson;								//交换机网络数据
		strJson +=	"}\n";
	}
	strJson +=	"]\n";
	strJson +=	"}\n";

	pthread_mutex_unlock(&snmpoidMutex);

	mstrjson = strJson;
	//*len=strJson.length();
	//memcpy(json,(char*)strJson.c_str(),*len);

}


void SetjsonIPStr(int messagetype,string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,vehplatecnt;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
    //IP 地址
	strJson = strJson + "\"ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"mask\":\""+ StrMask +"\",\n";	//子网掩码
	strJson = strJson + "\"gateway\":\""+ StrGateway +"\",\n";	//网关
	strJson = strJson + "\"dns\":\""+ StrDNS +"\"\n";	//DNS地址
	strJson +=	"}\n\0";

	pthread_mutex_unlock(&snmpoidMutex);

	mstrjson = strJson;
	//*len=strJson.length();
	//memcpy(json,(char*)strJson.c_str(),*len);

}

void SetjsonIP2Str(int messagetype,string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,vehplatecnt;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
    //IP 地址
	strJson = strJson + "\"ipaddr2\":\""+ StrIP2 +"\",\n";	//IP地址
	strJson = strJson + "\"mask2\":\""+ StrMask2 +"\",\n";	//子网掩码
	strJson = strJson + "\"gateway2\":\""+ StrGateway2 +"\",\n";	//网关
	strJson = strJson + "\"dns2\":\""+ StrDNS2 +"\"\n";	//DNS地址
	strJson +=	"}\n\0";

	pthread_mutex_unlock(&snmpoidMutex);

	mstrjson = strJson;
	//*len=strJson.length();
	//memcpy(json,(char*)strJson.c_str(),*len);

}



void SetjsonFlagRunStatusStr(int messagetype,string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,vehplatecnt;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间

	sprintf(str,"%d", recordno);
	strJson = strJson + "\"recordno\":"+ str +",\n";		//1 记录号
	recordno++;
	strJson = strJson + "\"flagnetroadid\":\""+ StrFlagNetRoadID +"\",\n";	//2 ETC 门架路网编号
	strJson = strJson + "\"flagroadid\":\""+ StrFlagRoadID +"\",\n";		//3 ETC 门架路段编号
	strJson = strJson + "\"flagid\": \"" + StrFlagID +"\",\n";			//4 ETC 门架编号
	strJson = strJson + "\"posid\": \"" + StrPosId +"\",\n";			//5 ETC 门架序号
	strJson = strJson + "\"direction\": \"" + StrDirection +"\",\n";		//6 行车方向
	strJson = strJson + "\"dirdescription\": \"" + StrDirDescription +"\",\n";		//7 行车方向说明
	strJson = strJson + "\"catchtime\": \"" + sDateTime + "\",\n";	//8 状态时间
	sprintf(str,"\"computer\": \"%d\",\n", stuFlagRunStatus->Computer);strJson += str;		//9 工控机状态
	sprintf(str,"\"diskcapacity\": \"%d\",\n", stuFlagRunStatus->DiskCapacity);strJson += str;		//10 硬盘容量
	sprintf(str,"\"diskusage\": \"%d\",\n", stuFlagRunStatus->DiskUsage);strJson += str;		//11 硬盘使用率
	sprintf(str,"\"powertype\": \"%d\",\n", stuFlagRunStatus->PowerType);strJson += str;		//12 供电类型
	sprintf(str,"\"backuppower\": \"%d\",\n", stuFlagRunStatus->BackUpPower);strJson += str;		//13 备用电源状态
	strJson = strJson +	"\"batteryremain\": \"" + HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str() + "\",\n";	//14 蓄电池电量(华为)
	sprintf(str,"\"dbstate\": %d,\n", stuFlagRunStatus->DBState);strJson += str;		//15 数据库状态
	sprintf(str,"\"cablenetstate\": %d,\n", stuFlagRunStatus->CableNetState);strJson += str;		//16 有线网络状态
	sprintf(str,"\"wirelessstate\": %d,\n", stuFlagRunStatus->WireLessState);strJson += str;		//17 无线网络状态
	sprintf(str,"\"software\": %d,\n", stuFlagRunStatus->SoftWare);strJson += str;		//18 ETC 门架软件状态
//	sprintf(str,"\"softversion\": %s,\n", stuFlagRunStatus->SoftVersion);strJson += str;		//19 软件版本
	sprintf(str,"\"softversion\": \"%s\",\n","LT00201907220006000B");strJson += str;		//19 软件版本
	sprintf(str,"\"cpu_occupancy\": %d,\n", stuFlagRunStatus->cpu_occupancy);strJson += str;		//cpu占用率212

	sprintf(str,"\"camercount\": \"%s\",\n", StrVehPlateCount.c_str());strJson = strJson + str; //20 车牌识别设备数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(str,"\"vehplate%d\": %s,\n", i+1,mTIPcamState[i].statuscode.c_str());strJson = strJson + str; //20 车牌识别设备状态
	}
	for(i=vehplatecnt;i<16;i++)
	{
		sprintf(str,"\"vehplate%d\": 2147483647,\n", i+1);strJson = strJson + str; // 车牌识别设备状态
	}

	sprintf(str,"%d", stuRsuControl.AntennaCount);strJson = strJson + "\"rsucount\":\""+ str +"\",\n"; 	//37 天线数量
	for(i=0;i<stuRsuControl.AntennaCount;i++)
	{
		sprintf(str,"\"rsu%d\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Status);strJson += str;		//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Power);strJson += str; //39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Channel);strJson += str; //40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Control_state);strJson += str;		//41 天线i 开关状态
	}
	for(i=stuRsuControl.AntennaCount;i<16;i++)
	{
		sprintf(str,"\"rsu%d\": 2147483647,\n", i+1);strJson += str; 	//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": 2147483647,\n", i+1);strJson += str;	//39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": 2147483647,\n", i+1);strJson += str; //40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\":	2147483647,\n", i+1);strJson += str; 	//41 天线i 开关状态
	}

	strJson +=	"\"backup1\": \"string\",\n";	//118 备用1
	strJson +=	"\"backup2\": \"string\",\n";	//119 备用2
	strJson +=	"\"backup3\": \"string\",\n";	//120 备用3
	strJson +=	"\"backup4\": \"string\",\n";	//121 备用4
	strJson = strJson + "\"timeflag\": \"" + sDateTime + "\",\n";	//122 时间戳标识

	//状态信息
	sprintf(str,"\"temp\": %d,\n", stuEnvi_Param->temp);strJson += str; 	//当前环境温度值123
	sprintf(str,"\"moist\": %d,\n", stuEnvi_Param->moist);strJson += str;		//当前环境湿度值124
	if(stuEnvi_Param->temp==0x7fff && stuEnvi_Param->moist==0x7fff)
		strJson +=	"\"online_flag\": 1,\n";		//温湿度是否在线125		//离线
	else
		strJson +=	"\"online_flag\": 0,\n";		//温湿度是否在线125		//在线

	//空调信息
	sprintf(str,"\"air_cond_status\": %d,\n", stuEnvi_Param->air_cond_status);strJson += str;		//空调整机状态129
	sprintf(str,"\"air_cond_fan_in_status\": %d,\n", stuEnvi_Param->air_cond_fan_in_status);strJson += str; 	//空调内风机状态130
	sprintf(str,"\"air_cond_fan_out_status\": %d,\n", stuEnvi_Param->air_cond_fan_out_status);strJson += str;		//空调外风机状态131
	sprintf(str,"\"air_cond_comp_status\": %d,\n", stuEnvi_Param->air_cond_comp_status);strJson += str; 	//空调压缩机状态132
	sprintf(str,"\"air_cond_heater_status\": %d,\n", stuEnvi_Param->air_cond_heater_status);strJson += str; 	//空调电加热状态133
	sprintf(str,"\"air_cond_fan_emgy_status\": %d,\n", stuEnvi_Param->air_cond_fan_emgy_status);strJson += str; 	//空调应急风机状态134
	sprintf(str,"\"air_cond_temp_in\": %d,\n", stuEnvi_Param->air_cond_temp_in);strJson += str; 	//当前空调室内温度值135
	sprintf(str,"\"air_cond_temp_out\": %d,\n", stuEnvi_Param->air_cond_temp_out);strJson += str;		//当前空调室外温度值	136
	sprintf(str,"\"air_cond_amp\": %d,\n", stuEnvi_Param->air_cond_amp);strJson += str; 	//当前空调室电流值137
	sprintf(str,"\"air_cond_volt\": %d,\n", stuEnvi_Param->air_cond_volt);strJson += str;		//当前空调室电压值138


	//UPS信息
	sprintf(str,"\"in_phase_num\": %d,\n", stuUps_Param->in_phase_num);strJson += str;		//UPS输入相数139
	sprintf(str,"\"in_freq\": %d,\n", stuUps_Param->in_freq);strJson += str;		//UPS交流输入频率140
	sprintf(str,"\"volt_ain\": %d,\n", stuUps_Param->volt_Ain);strJson += str;		// 交流输入相电压A`141
	sprintf(str,"\"volt_bin\": %d,\n", stuUps_Param->volt_Bin);strJson += str;		// 交流输入相电压B 142
	sprintf(str,"\"volt_cin\": %d,\n", stuUps_Param->volt_Cin);strJson += str;		// 交流输入相电压C143
	sprintf(str,"\"amp_ain\": %d,\n", stuUps_Param->amp_Ain);strJson += str;		// 交流输入相电流A144
	sprintf(str,"\"amp_bin\": %d,\n", stuUps_Param->amp_Bin);strJson += str;		//交流输入相电流B145
	sprintf(str,"\"amp_cin\": %d,\n", stuUps_Param->amp_Cin);strJson += str;		//交流输入相电流C146
	sprintf(str,"\"fact_ain\": %d,\n", stuUps_Param->fact_Ain);strJson += str;		//功率因素A147
	sprintf(str,"\"fact_bin\": %d,\n", stuUps_Param->fact_Bin);strJson += str;		//功率因素B148
	sprintf(str,"\"fact_cin\": %d,\n", stuUps_Param->fact_Cin);strJson += str;		//功率因素C149
	sprintf(str,"\"bypass_volta\": %d,\n", stuUps_Param->bypass_voltA);strJson += str;		//旁路电压A150
	sprintf(str,"\"bypass_voltb\": %d,\n", stuUps_Param->bypass_voltB);strJson += str;		//旁路电压B151
	sprintf(str,"\"bypass_voltc\": %d,\n", stuUps_Param->bypass_voltC);strJson += str;		//旁路电压C152
	sprintf(str,"\"bypass_freq\": %d,\n", stuUps_Param->bypass_freq);strJson += str;		//旁路频率153
	sprintf(str,"\"out_phase_num\": %d,\n", stuUps_Param->out_phase_num);strJson += str;		//UPS输出相数154
	sprintf(str,"\"out_freq\": %d,\n", stuUps_Param->out_freq);strJson += str;		//UPS交流输出频率155
	sprintf(str,"\"volt_aout\": %d,\n", stuUps_Param->volt_Aout);strJson += str;		//交流输出相电压A156
	sprintf(str,"\"volt_bout\": %d,\n", stuUps_Param->volt_Bout);strJson += str;		//交流输出相电压B157
	sprintf(str,"\"volt_cout\": %d,\n", stuUps_Param->volt_Cout);strJson += str;		//交流输出相电压C158
	sprintf(str,"\"amp_aout\": %d,\n", stuUps_Param->amp_Aout);strJson += str;		//交流输出相电流A159
	sprintf(str,"\"amp_bout\": %d,\n", stuUps_Param->amp_Bout);strJson += str;		//交流输出相电流B169
	sprintf(str,"\"amp_cout\": %d,\n", stuUps_Param->amp_Cout);strJson += str;		//交流输出相电流C161
	sprintf(str,"\"kw_aout\": %d,\n", stuUps_Param->kw_Aout);strJson += str;		//UPS A相输出有功功率162
	sprintf(str,"\"kw_bout\": %d,\n", stuUps_Param->kw_Bout);strJson += str;		//UPS B相输出有功功率	163
	sprintf(str,"\"kw_cout\": %d,\n", stuUps_Param->kw_Cout);strJson += str;		//UPS C相输出有功功率164
	sprintf(str,"\"kva_aout\": %d,\n", stuUps_Param->kva_Aout);strJson += str;		//UPS A相输出视在功率165
	sprintf(str,"\"kva_bout\": %d,\n", stuUps_Param->kva_Bout);strJson += str;		//UPS B相输出视在功率166
	sprintf(str,"\"kva_cout\": %d,\n", stuUps_Param->kva_Cout);strJson += str;		//UPS C相输出视在功率167
	sprintf(str,"\"load_aout\": %d,\n", stuUps_Param->load_Aout);strJson += str;		//UPS A相输出负载率168
	sprintf(str,"\"load_bout\": %d,\n", stuUps_Param->load_Bout);strJson += str;		//UPS B相输出负载率169
	sprintf(str,"\"load_cout\": %d,\n", stuUps_Param->load_Cout);strJson += str;		//UPS C相输出负载率170
	sprintf(str,"\"running_day\": %d,\n", stuUps_Param->running_day);strJson += str;		//UPS运行时间171
	sprintf(str,"\"battery_volt\": %d,\n", stuUps_Param->battery_volt);strJson += str;		//UPS电池电压172
	sprintf(str,"\"amp_charge\": %d,\n", stuUps_Param->amp_charge);strJson += str;		//UPS充电电流173
	sprintf(str,"\"amp_discharge\": %d,\n", stuUps_Param->amp_discharge);strJson += str;		//UPS放电电流174
	sprintf(str,"\"battery_left\": %d,\n", stuUps_Param->battery_left);strJson += str;		// UPS电池后备时间175
	sprintf(str,"\"battery_tmp\": %d,\n", stuUps_Param->battery_tmp);strJson += str;		//环境温度176
	sprintf(str,"\"battery_capacity\": %d,\n", stuUps_Param->battery_capacity);strJson += str;		//电池当前容量177
	sprintf(str,"\"battery_dischg_times\": %d,\n", stuUps_Param->battery_dischg_times);strJson += str;		//电池放电次数178

	//防雷器
	//sprintf(str,"\"status\": %d,\n", stuSpd_Param->status);strJson += str;		//防雷器在线状态179
	//sprintf(str,"\"struck_times\": %d,\n", stuSpd_Param->struck_times);strJson += str;		//雷击次数180

	//告警信息
	sprintf(str,"\"water_flag\": %d,\n", stuEnvi_Param->water_flag);strJson += str; 	//漏水181
	sprintf(str,"\"front_door_flag\": %d,\n", stuEnvi_Param->front_door_flag);strJson += str;		//前柜门开关状态182
	sprintf(str,"\"back_door_flag\": %d,\n", stuEnvi_Param->back_door_flag);strJson += str; 	//后柜门开关状态183
	sprintf(str,"\"door_overtime\": %d,\n", stuEnvi_Param->door_overtime);strJson += str;		//柜门开启超时184
	sprintf(str,"\"smoke_event_flag\": %d,\n", stuEnvi_Param->smoke_event_flag);strJson += str; 	//烟雾报警185
	sprintf(str,"\"air_cond_hightemp_alarm\": %d,\n", stuEnvi_Param->air_cond_hightemp_alarm);strJson += str;		//空调高温告警186
	sprintf(str,"\"air_cond_lowtemp_alarm\": %d,\n", stuEnvi_Param->air_cond_lowtemp_alarm);strJson += str; 	//空调低温告警187
	sprintf(str,"\"air_cond_highmoist_alarm\": %d,\n", stuEnvi_Param->air_cond_highmoist_alarm);strJson += str; 	//空调高湿告警188
	sprintf(str,"\"air_cond_lowmoist_alarm\": %d,\n", stuEnvi_Param->air_cond_lowmoist_alarm);strJson += str;		//空调低湿告警189
	sprintf(str,"\"air_cond_infan_alarm\": %d,\n", stuEnvi_Param->air_cond_infan_alarm);strJson += str; 	//空调内风机故障190
	sprintf(str,"\"air_cond_outfan_alarm\": %d,\n", stuEnvi_Param->air_cond_outfan_alarm);strJson += str;		//空调外风机故障191
	sprintf(str,"\"air_cond_comp_alarm\": %d,\n", stuEnvi_Param->air_cond_comp_alarm);strJson += str;		//空调压缩机故障192
	sprintf(str,"\"air_cond_heater_alarm\": %d,\n", stuEnvi_Param->air_cond_heater_alarm);strJson += str;		//空调电加热故障193
	sprintf(str,"\"air_cond_emgyfan_alarm\": %d,\n", stuEnvi_Param->air_cond_emgyfan_alarm);strJson += str; 	//空调应急风机故障194
	sprintf(str,"\"supply_out_status\": %d,\n", stuUps_Param->supply_out_status);strJson += str;		//输出供电状态195
	sprintf(str,"\"supply_in_status\": %d,\n", stuUps_Param->supply_in_status);strJson += str;		//输入供电状态196
	sprintf(str,"\"battery_status\": %d,\n", stuUps_Param->battery_status);strJson += str;		//电池状态197
	sprintf(str,"\"main_abnormal\": %d,\n", stuUps_Param->main_abnormal);strJson += str;		//主路异常198
	sprintf(str,"\"system_overtemp\": %d,\n", stuUps_Param->system_overtemp);strJson += str;		//系统过温199
	sprintf(str,"\"sysbat_low_prealarm\": %d,\n", stuUps_Param->sysbat_low_prealarm);strJson += str;		//系统电池电量低预告警200
	sprintf(str,"\"rectifier_overload\": %d,\n", stuUps_Param->rectifier_overload);strJson += str;		//整流器过载201
	sprintf(str,"\"inverter_overload\": %d,\n", stuUps_Param->inverter_overload);strJson += str;		//逆变器过载202
	sprintf(str,"\"bypass_abnomal\": %d,\n", stuUps_Param->bypass_abnomal);strJson += str;		//旁路异常203
	sprintf(str,"\"battery_low_prealarm\": %d,\n", stuUps_Param->battery_low_prealarm);strJson += str;		//电池电压低204
	sprintf(str,"\"battery_abnomal\": %d,\n", stuUps_Param->battery_abnomal);strJson += str;		//电池电压异常205
	sprintf(str,"\"battery_overtemp\": %d,\n", stuUps_Param->battery_overtemp);strJson += str;		//电池过温206
	sprintf(str,"\"fan_abnormal\": %d,\n", stuUps_Param->fan_abnormal);strJson += str;		//风扇故障207
	sprintf(str,"\"shutdown_alarm\": %d,\n", stuUps_Param->shutdown_alarm);strJson += str;		//紧急关机告警208
	sprintf(str,"\"maintain_status\": %d,\n", stuUps_Param->maintain_status);strJson += str;		//维修模式209
	sprintf(str,"\"inverter_supply\": %d,\n", stuUps_Param->inverter_supply);strJson += str;		//电池逆变供电210
	sprintf(str,"\"bypass_supply\": %d,\n", stuUps_Param->bypass_supply);strJson += str;		//旁路供电211

    pthread_mutex_lock(&snmpoidMutex);

	//华为机柜字段
	strJson = strJson + "\"vmctrl_ipaddr\": \"" + StrIP.c_str() + "\",\n";	//控制器IP地址 212
	strJson = strJson + "\"cabinettype\": " + StrCabinetType.c_str() + ",\n";	//机柜类型 213
	strJson = strJson + "\"hwacbgroupbatvolt\": " + HUAWEIDevValue.strhwAcbGroupBatVolt.c_str() + ",\n";	//锂电电池电压 214
	strJson = strJson + "\"hwacbgroupbatcurr\": " + HUAWEIDevValue.strhwAcbGroupBatCurr.c_str() + ",\n";	//锂电电池电流 215
	strJson = strJson + "\"hwacbgrouptotalcapacity\": " + HUAWEIDevValue.strhwAcbGroupTotalCapacity.c_str() + ",\n";	//锂电电池总容量 216
	strJson = strJson + "\"hwacbgrouptotalremaincapacity\": " + HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str() + ",\n";	//锂电电池剩余容量 217
	strJson = strJson + "\"hwacbgroupbackuptime\": " + HUAWEIDevValue.strhwAcbGroupBackupTime.c_str() + ",\n";	//电池备电时长 218
	strJson = strJson + "\"hwacbgroupbatsoh\": " + HUAWEIDevValue.strhwAcbGroupBatSoh.c_str() + ",\n";	//电池SOH 219
	strJson = strJson + "\"hwaporablvoltage\": " + HUAWEIDevValue.strhwApOrAblVoltage.c_str() + ",\n";	//A/AB电压 220
	strJson = strJson + "\"hwbporbclvoltage\": " + HUAWEIDevValue.strhwBpOrBclVoltage.c_str() + ",\n";	//B/BC电压 221
	strJson = strJson + "\"hwcporcalvoltage\": " + HUAWEIDevValue.strhwCpOrCalVoltage.c_str() + ",\n";	//C/CA电压 222
	strJson = strJson + "\"hwaphasecurrent\": " + HUAWEIDevValue.strhwAphaseCurrent.c_str() + ",\n";	//A相电流 223
	strJson = strJson + "\"hwbphasecurrent\": " + HUAWEIDevValue.strhwBphaseCurrent.c_str() + ",\n";	//B相电流 224
	strJson = strJson + "\"hwcphasecurrent\": " + HUAWEIDevValue.strhwCphaseCurrent.c_str() + ",\n";	//C相电流 225
	strJson = strJson + "\"hwdcoutputvoltage\": " + HUAWEIDevValue.strhwDcOutputVoltage.c_str() + ",\n";	//DC输出电压 226
	strJson = strJson + "\"hwdcoutputcurrent\": " + HUAWEIDevValue.strhwDcOutputCurrent.c_str() + ",\n";	//DC输出电流 227
    strJson = strJson + "\"hwenvtemperature\": " + HUAWEIDevValue.strhwEnvTemperature[0].c_str() + ",\n";	//环境温度值 228
    strJson = strJson + "\"hwenvtemperature2\": " + HUAWEIDevValue.strhwEnvTemperature[1].c_str() + ",\n";	//环境温度值 228
    strJson = strJson + "\"hwenvhumidity\": " + HUAWEIDevValue.strhwEnvHumidity[0].c_str() + ",\n";	//环境湿度值 229
    strJson = strJson + "\"hwenvhumidity2\": " + HUAWEIDevValue.strhwEnvHumidity[1].c_str() + ",\n";	//环境湿度值 229
    strJson = strJson + "\"hwdcairctrlmode\": " + HUAWEIDevValue.strhwDcAirCtrlMode[0].c_str() + ",\n";	//空调控制模式 230
    strJson = strJson + "\"hwdcairctrlmode2\": " + HUAWEIDevValue.strhwDcAirCtrlMode[1].c_str() + ",\n";	//空调控制模式 230
    strJson = strJson + "\"hwdcairrunstatus\": " + HUAWEIDevValue.strhwDcAirRunStatus[0].c_str() + ",\n";	//空调运行状态 231
    strJson = strJson + "\"hwdcairrunstatus2\": " + HUAWEIDevValue.strhwDcAirRunStatus[1].c_str() + ",\n";	//空调运行状态 231
    strJson = strJson + "\"hwdcaircompressorrunstatus\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus[0].c_str() + ",\n";	//空调压缩机运行状态 232
    strJson = strJson + "\"hwdcaircompressorrunstatus2\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus[1].c_str() + ",\n";	//空调压缩机运行状态 232
    strJson = strJson + "\"hwdcairinnrfanspeed\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed[0].c_str() + ",\n";	//空调内机转速 233
        strJson = strJson + "\"hwdcairinnrfanspeed2\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed[1].c_str() + ",\n";	//空调内机转速 233
    strJson = strJson + "\"hwdcairouterfanspeed\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed[0].c_str() + ",\n";	//空调外风机转速 234
        strJson = strJson + "\"hwdcairouterfanspeed2\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed[1].c_str() + ",\n";	//空调外风机转速 234
    strJson = strJson + "\"hwdcaircompressorruntime\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime[0].c_str() + ",\n";	//空调压缩机运行时间 235
        strJson = strJson + "\"hwdcaircompressorruntime2\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime[1].c_str() + ",\n";	//空调压缩机运行时间 235
    strJson = strJson + "\"hwdcairenterchanneltemp\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp[0].c_str() + ",\n";	//空调回风口温度 236
        strJson = strJson + "\"hwdcairenterchanneltemp2\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp[1].c_str() + ",\n";	//空调回风口温度 236
    strJson = strJson + "\"hwdcairpowerontemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint[0].c_str() + ",\n";	//空调开机温度点 237
        strJson = strJson + "\"hwdcairpowerontemppoint2\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint[1].c_str() + ",\n";	//空调开机温度点 237
    strJson = strJson + "\"hwdcairpowerofftemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint[0].c_str() + ",\n";	//空调关机温度点 238
        strJson = strJson + "\"hwdcairpowerofftemppoint2\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint[1].c_str() + ",\n";	//空调关机温度点 238
	strJson = strJson + "\"hwenvtempalarmtraps\": " + HUAWEIDevAlarm.hwEnvTempAlarmTraps.c_str() + ",\n";	//环境温度告警 239
	strJson = strJson + "\"hwenvhumialarmresumetraps\": " + HUAWEIDevAlarm.hwEnvHumiAlarmTraps.c_str() + ",\n"; //环境湿度告警 240
	strJson = strJson + "\"hwdooralarmtraps\": " + HUAWEIDevAlarm.hwDoorAlarmTraps.c_str() + ",\n"; //门禁告警 241
	strJson = strJson + "\"hwwateralarmtraps\": " + HUAWEIDevAlarm.hwWaterAlarmTraps.c_str() + ",\n";	//水浸告警 242
	strJson = strJson + "\"hwsmokealarmtraps\": " + HUAWEIDevAlarm.hwSmokeAlarmTraps.c_str() + ",\n";	//烟雾告警 243
	strJson = strJson + "\"hwspdalarmtraps\": " + HUAWEIDevAlarm.hwSpareDigitalAlarmTraps.c_str() + ",\n";	//防雷器故障告警 244
	strJson = strJson + "\"hwair_cond_infan_alarm\": " + HUAWEIDevAlarm.hwair_cond_infan_alarm.c_str() + ",\n"; //空调内风机故障 245
	strJson = strJson + "\"hwair_cond_outfan_alarm\": " + HUAWEIDevAlarm.hwair_cond_outfan_alarm.c_str() + ",\n";	//空调外风机故障 246
	strJson = strJson + "\"hwair_cond_comp_alarm\": " + HUAWEIDevAlarm.hwair_cond_comp_alarm.c_str() + ",\n";	//空调压缩机故障 247
	strJson = strJson + "\"hwair_cond_return_port_sensor_alarm\": " + HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm.c_str() + ",\n";	//空调回风口传感器故障 248
	strJson = strJson + "\"hwair_cond_evap_freezing_alarm\": " + HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm.c_str() + ",\n"; //空调蒸发器冻结 249
	strJson = strJson + "\"hwair_cond_freq_high_press_alarm\": " + HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm.c_str() + ",\n"; //空调频繁高压力 250
	strJson = strJson + "\"hwair_cond_comm_fail_alarm\": " + HUAWEIDevAlarm.hwair_cond_comm_fail_alarm.c_str() + ",\n";	//空调通信失败告警 251

	//新增加的状态
	//设备信息
    strJson = strJson + "\"hwmonequipsoftwareversion\": \"" + HUAWEIDevValue.strhwMonEquipSoftwareVersion.c_str() + "\",\n";	//软件版本
    strJson = strJson + "\"hwmonequipmanufacturer\": \"" + HUAWEIDevValue.strhwMonEquipManufacturer.c_str() + "\",\n";	//设备生产商
	//锂电(新增加)
    strJson = strJson + "\"hwacbgrouptemperature\": " + HUAWEIDevValue.strhwAcbGroupTemperature.c_str() + ",\n";	//电池温度
    strJson = strJson + "\"hwacbgroupovercurthr\": " + HUAWEIDevValue.strhwAcbGroupOverCurThr.c_str() + ",\n";	//充电过流告警点
    strJson = strJson + "\"hwacbgrouphightempthr\": " + HUAWEIDevValue.strhwAcbGroupHighTempThr.c_str() + ",\n";	//高温告警点
    strJson = strJson + "\"hwacbgrouplowtempth\": " + HUAWEIDevValue.strhwAcbGroupLowTempTh.c_str() + ",\n";	//低温告警点
    strJson = strJson + "\"hwacbgroupdodtoacidbattery\": " + HUAWEIDevValue.strhwAcbGroupDodToAcidBattery.c_str() + ",\n";	//锂电放电DOD
	//开关电源(新增加)
    strJson = strJson + "\"hwsetacsuppervoltlimit\": " + HUAWEIDevValue.strhwSetAcsUpperVoltLimit.c_str() + ",\n";	//AC过压点设置
    strJson = strJson + "\"hwsetacslowervoltlimit\": " + HUAWEIDevValue.strhwSetAcsLowerVoltLimit.c_str() + ",\n";	//AC欠压点设置
    strJson = strJson + "\"hwsetdcsuppervoltlimit\": " + HUAWEIDevValue.strhwSetDcsUpperVoltLimit.c_str() + ",\n";	//设置DC过压点
    strJson = strJson + "\"hwsetdcslowervoltlimit\": " + HUAWEIDevValue.strhwSetDcsLowerVoltLimit.c_str() + ",\n";	//设置DC欠压点
    strJson = strJson + "\"hwsetlvdvoltage\": " + HUAWEIDevValue.strhwSetLvdVoltage.c_str() + ",\n";	//设置LVD电压
	//环境传感器(新增加)
    strJson = strJson + "\"hwsetenvtempupperlimit\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit[0].c_str() + ",\n";	//环境温度告警上限
        strJson = strJson + "\"hwsetenvtempupperlimit2\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit[1].c_str() + ",\n";	//环境温度告警上限
    strJson = strJson + "\"hwsetenvtemplowerlimit\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit[0].c_str() + ",\n";	//环境温度告警下限
        strJson = strJson + "\"hwsetenvtemplowerlimit2\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit[1].c_str() + ",\n";	//环境温度告警下限
//    strJson = strJson + "\"hwsetenvtempultrahightempthreshold\": " + HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold.c_str() + ",\n";	//环境高高温告警点
    strJson = strJson + "\"hwsetenvhumidityupperlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[0].c_str() + ",\n";	//环境湿度告警上限
        strJson = strJson + "\"hwsetenvhumidityupperlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[1].c_str() + ",\n";	//环境湿度告警上限
    strJson = strJson + "\"hwsetenvhumiditylowerlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[0].c_str() + ",\n";	//环境湿度告警下限
        strJson = strJson + "\"hwsetenvhumiditylowerlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[1].c_str() + ",\n";	//环境湿度告警下限
	//直流空调(新增加)
    strJson = strJson + "\"hwdcairruntime\": " + HUAWEIDevValue.strhwDcAirRunTime[0].c_str() + ",\n";	//空调运行时间
        strJson = strJson + "\"hwdcairruntime2\": " + HUAWEIDevValue.strhwDcAirRunTime[1].c_str() + ",\n";	//空调运行时间
    strJson = strJson + "\"hwcoolingdevicesmode\": " + HUAWEIDevValue.strhwCoolingDevicesMode.c_str() + ",\n";	//温控模式
    //防火墙
    strJson = strJson + "\"hwentitycpuusage\": " + HUAWEIDevValue.strhwEntityCpuUsage.c_str() + ",\n";	//CPU
    strJson = strJson + "\"hwentitymemusage\": " + HUAWEIDevValue.strhwEntityMemUsage.c_str() + ",\n";	//内存使用率
    strJson = strJson + "\"hwentitytemperature\": " + HUAWEIDevValue.strhwEntityTemperature.c_str() + ",\n";	//温度
    //交换机
    strJson = strJson + "\"hwswitchentitycpuusage\": " + HUAWEIDevValue.strhwswitchEntityCpuUsage.c_str() + ",\n";	//CPU
    strJson = strJson + "\"hwswitchentitymemusage\": " + HUAWEIDevValue.strhwswitchEntityMemUsage.c_str() + ",\n";	//内存使用率
    strJson = strJson + "\"hwswitchentitytemperature\": " + HUAWEIDevValue.strhwswitchEntityTemperature.c_str() + "\n";	//温度
    strJson +=  "}\n\0";

    pthread_mutex_unlock(&snmpoidMutex);

    mstrjson = strJson;
    //*len=strJson.length();
    //memcpy(json,(char*)strJson.c_str(),*len);

}

//void SetjsonTableStr(char* table, char *json, int *len) mstrjson
void SetjsonTableStr(char* table, string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,vehplatecnt;
	static int recordno=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson,strTable;
	strTable = table;

    strJson +=  "{\n";
    strJson +=  "\"param\": {\n";
	strJson +=	"\"action\":\""+ strTable + "\",\n";
	strJson +=	"\"entity\": {\n";

	sprintf(str,"%d", recordno);
    strJson = strJson + "\"recordno\":"+ str +",\n";		//1 记录号
    recordno++;
	strJson = strJson + "\"flagnetroadid\":\""+ StrFlagNetRoadID +"\",\n";	//2 ETC 门架路网编号
	strJson = strJson + "\"flagroadid\":\""+ StrFlagRoadID +"\",\n";		//3 ETC 门架路段编号
	strJson = strJson + "\"flagid\": \"" + StrFlagID +"\",\n";			//4 ETC 门架编号
	strJson = strJson + "\"posid\": \"" + StrPosId +"\",\n";			//5 ETC 门架序号
	strJson = strJson + "\"direction\": \"" + StrDirection +"\",\n";		//6 行车方向
	strJson = strJson + "\"dirdescription\": \"" + StrDirDescription +"\",\n";		//7 行车方向说明
	strJson = strJson + "\"catchtime\": \"" + sDateTime + "\",\n";	//8 状态时间
	sprintf(str,"\"computer\": \"%d\",\n", stuFlagRunStatus->Computer);strJson += str;		//9 工控机状态
	sprintf(str,"\"diskcapacity\": \"%d\",\n", stuFlagRunStatus->DiskCapacity);strJson += str;		//10 硬盘容量
	sprintf(str,"\"diskusage\": \"%d\",\n", stuFlagRunStatus->DiskUsage);strJson += str;		//11 硬盘使用率
	sprintf(str,"\"powertype\": \"%d\",\n", stuFlagRunStatus->PowerType);strJson += str;		//12 供电类型
	sprintf(str,"\"backuppower\": \"%d\",\n", stuFlagRunStatus->BackUpPower);strJson += str;		//13 备用电源状态
	strJson = strJson + "\"batteryremain\": \"" + HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str() + "\",\n";	//14 蓄电池电量(华为)
	sprintf(str,"\"dbstate\": %d,\n", stuFlagRunStatus->DBState);strJson += str;		//15 数据库状态
	sprintf(str,"\"cablenetstate\": %d,\n", stuFlagRunStatus->CableNetState);strJson += str;		//16 有线网络状态
	sprintf(str,"\"wirelessstate\": %d,\n", stuFlagRunStatus->WireLessState);strJson += str;		//17 无线网络状态
	sprintf(str,"\"software\": %d,\n", stuFlagRunStatus->SoftWare);strJson += str;		//18 ETC 门架软件状态
//	sprintf(str,"\"softversion\": %s,\n", stuFlagRunStatus->SoftVersion);strJson += str;		//19 软件版本
	sprintf(str,"\"softversion\": \"%s\",\n","LT00201907220006000B");strJson += str;		//19 软件版本
	sprintf(str,"\"cpu_occupancy\": %d,\n", stuFlagRunStatus->cpu_occupancy);strJson += str;		//cpu占用率212

	sprintf(str,"\"camercount\": \"%s\",\n", StrVehPlateCount.c_str());strJson = strJson + str; //20 车牌识别设备数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(str,"\"vehplate%d\": %s,\n", i+1,mTIPcamState[i].statuscode.c_str());strJson = strJson + str; //20 车牌识别设备状态
	}
	for(i=vehplatecnt;i<16;i++)
	{
		sprintf(str,"\"vehplate%d\": 2147483647,\n", i+1);strJson = strJson + str; // 车牌识别设备状态
	}

	sprintf(str,"%d", stuRsuControl.AntennaCount);strJson = strJson + "\"rsucount\":\""+ str +"\",\n";	//37 天线数量
	for(i=0;i<stuRsuControl.AntennaCount;i++)
	{
		sprintf(str,"\"rsu%d\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Status);strJson += str;		//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Power);strJson += str; //39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Channel);strJson += str; //40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Control_state);strJson += str; 	//41 天线i 开关状态
	}
	for(i=stuRsuControl.AntennaCount;i<16;i++)
	{
		sprintf(str,"\"rsu%d\": 2147483647,\n", i+1);strJson += str;	//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": 2147483647,\n", i+1);strJson += str;	//39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": 2147483647,\n", i+1);strJson += str; //40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\":	2147483647,\n", i+1);strJson += str;	//41 天线i 开关状态
	}

	strJson +=	"\"backup1\": \"string\",\n";	//118 备用1
	strJson +=	"\"backup2\": \"string\",\n";	//119 备用2
	strJson +=	"\"backup3\": \"string\",\n";	//120 备用3
	strJson +=	"\"backup4\": \"string\",\n";	//121 备用4
	strJson = strJson + "\"timeflag\": \"" + sDateTime + "\",\n";	//122 时间戳标识

	//状态信息
/*	if(HUAWEIDevValue.strhwEnvTemperature=="2147483647" && HUAWEIDevValue.strhwEnvHumidity=="2147483647")
	{
		strJson = strJson + "\"temp\": 0,\n";	// 当前环境温度值123
		strJson = strJson + "\"moist\": 0,\n";	// 当前环境湿度值124
		strJson +=	"\"online_flag\": 1,\n";		//温湿度是否在线125		//离线
	}
	else
	{
		strJson = strJson + "\"temp\": " + HUAWEIDevValue.strhwEnvTemperature.c_str() + ",\n";	// 当前环境温度值123
		strJson = strJson + "\"moist\": " + HUAWEIDevValue.strhwEnvHumidity.c_str() + ",\n";	// 当前环境湿度值124
		strJson +=	"\"online_flag\": 0,\n";		//温湿度是否在线125		//在线
	}*/
	sprintf(str,"\"temp\": %d,\n", stuEnvi_Param->temp);strJson += str;		//当前环境温度值123
	sprintf(str,"\"moist\": %d,\n", stuEnvi_Param->moist);strJson += str;		//当前环境湿度值124
	if(stuEnvi_Param->temp==0x7fff && stuEnvi_Param->moist==0x7fff)
		strJson +=	"\"online_flag\": 1,\n";		//温湿度是否在线125		//离线
	else
		strJson +=	"\"online_flag\": 0,\n";		//温湿度是否在线125		//在线
//	strJson +=	"\"BackUp5[50]\": \"string\",\n";		// 备用 126
//	strJson +=	"\"BackUp6[50]\": \"string\",\n";		// 备用 127
//	strJson +=	"\"BackUp7[50]\": \"string\",\n";		// 备用 128

	//空调信息
	sprintf(str,"\"air_cond_status\": %d,\n", stuEnvi_Param->air_cond_status);strJson += str;		//空调整机状态129
	sprintf(str,"\"air_cond_fan_in_status\": %d,\n", stuEnvi_Param->air_cond_fan_in_status);strJson += str;		//空调内风机状态130
	sprintf(str,"\"air_cond_fan_out_status\": %d,\n", stuEnvi_Param->air_cond_fan_out_status);strJson += str;		//空调外风机状态131
	sprintf(str,"\"air_cond_comp_status\": %d,\n", stuEnvi_Param->air_cond_comp_status);strJson += str;		//空调压缩机状态132
	sprintf(str,"\"air_cond_heater_status\": %d,\n", stuEnvi_Param->air_cond_heater_status);strJson += str;		//空调电加热状态133
	sprintf(str,"\"air_cond_fan_emgy_status\": %d,\n", stuEnvi_Param->air_cond_fan_emgy_status);strJson += str;		//空调应急风机状态134
	sprintf(str,"\"air_cond_temp_in\": %d,\n", stuEnvi_Param->air_cond_temp_in);strJson += str;		//当前空调室内温度值135
	sprintf(str,"\"air_cond_temp_out\": %d,\n", stuEnvi_Param->air_cond_temp_out);strJson += str;		//当前空调室外温度值	136
	sprintf(str,"\"air_cond_amp\": %d,\n", stuEnvi_Param->air_cond_amp);strJson += str;		//当前空调室电流值137
	sprintf(str,"\"air_cond_volt\": %d,\n", stuEnvi_Param->air_cond_volt);strJson += str;		//当前空调室电压值138


	//UPS信息
	sprintf(str,"\"in_phase_num\": %d,\n", stuUps_Param->in_phase_num);strJson += str;		//UPS输入相数139
	sprintf(str,"\"in_freq\": %d,\n", stuUps_Param->in_freq);strJson += str;		//UPS交流输入频率140
	sprintf(str,"\"volt_ain\": %d,\n", stuUps_Param->volt_Ain);strJson += str;		// 交流输入相电压A`141
	sprintf(str,"\"volt_bin\": %d,\n", stuUps_Param->volt_Bin);strJson += str;		// 交流输入相电压B 142
	sprintf(str,"\"volt_cin\": %d,\n", stuUps_Param->volt_Cin);strJson += str;		// 交流输入相电压C143
	sprintf(str,"\"amp_ain\": %d,\n", stuUps_Param->amp_Ain);strJson += str;		// 交流输入相电流A144
	sprintf(str,"\"amp_bin\": %d,\n", stuUps_Param->amp_Bin);strJson += str;		//交流输入相电流B145
	sprintf(str,"\"amp_cin\": %d,\n", stuUps_Param->amp_Cin);strJson += str;		//交流输入相电流C146
	sprintf(str,"\"fact_ain\": %d,\n", stuUps_Param->fact_Ain);strJson += str;		//功率因素A147
	sprintf(str,"\"fact_bin\": %d,\n", stuUps_Param->fact_Bin);strJson += str;		//功率因素B148
	sprintf(str,"\"fact_cin\": %d,\n", stuUps_Param->fact_Cin);strJson += str;		//功率因素C149
	sprintf(str,"\"bypass_volta\": %d,\n", stuUps_Param->bypass_voltA);strJson += str;		//旁路电压A150
	sprintf(str,"\"bypass_voltb\": %d,\n", stuUps_Param->bypass_voltB);strJson += str;		//旁路电压B151
	sprintf(str,"\"bypass_voltc\": %d,\n", stuUps_Param->bypass_voltC);strJson += str;		//旁路电压C152
	sprintf(str,"\"bypass_freq\": %d,\n", stuUps_Param->bypass_freq);strJson += str;		//旁路频率153
	sprintf(str,"\"out_phase_num\": %d,\n", stuUps_Param->out_phase_num);strJson += str;		//UPS输出相数154
	sprintf(str,"\"out_freq\": %d,\n", stuUps_Param->out_freq);strJson += str;		//UPS交流输出频率155
	sprintf(str,"\"volt_aout\": %d,\n", stuUps_Param->volt_Aout);strJson += str;		//交流输出相电压A156
	sprintf(str,"\"volt_bout\": %d,\n", stuUps_Param->volt_Bout);strJson += str;		//交流输出相电压B157
	sprintf(str,"\"volt_cout\": %d,\n", stuUps_Param->volt_Cout);strJson += str;		//交流输出相电压C158
	sprintf(str,"\"amp_aout\": %d,\n", stuUps_Param->amp_Aout);strJson += str;		//交流输出相电流A159
	sprintf(str,"\"amp_bout\": %d,\n", stuUps_Param->amp_Bout);strJson += str;		//交流输出相电流B169
	sprintf(str,"\"amp_cout\": %d,\n", stuUps_Param->amp_Cout);strJson += str;		//交流输出相电流C161
	sprintf(str,"\"kw_aout\": %d,\n", stuUps_Param->kw_Aout);strJson += str;		//UPS A相输出有功功率162
	sprintf(str,"\"kw_bout\": %d,\n", stuUps_Param->kw_Bout);strJson += str;		//UPS B相输出有功功率	163
	sprintf(str,"\"kw_cout\": %d,\n", stuUps_Param->kw_Cout);strJson += str;		//UPS C相输出有功功率164
	sprintf(str,"\"kva_aout\": %d,\n", stuUps_Param->kva_Aout);strJson += str;		//UPS A相输出视在功率165
	sprintf(str,"\"kva_bout\": %d,\n", stuUps_Param->kva_Bout);strJson += str;		//UPS B相输出视在功率166
	sprintf(str,"\"kva_cout\": %d,\n", stuUps_Param->kva_Cout);strJson += str;		//UPS C相输出视在功率167
	sprintf(str,"\"load_aout\": %d,\n", stuUps_Param->load_Aout);strJson += str;		//UPS A相输出负载率168
	sprintf(str,"\"load_bout\": %d,\n", stuUps_Param->load_Bout);strJson += str;		//UPS B相输出负载率169
	sprintf(str,"\"load_cout\": %d,\n", stuUps_Param->load_Cout);strJson += str;		//UPS C相输出负载率170
	sprintf(str,"\"running_day\": %d,\n", stuUps_Param->running_day);strJson += str;		//UPS运行时间171
	sprintf(str,"\"battery_volt\": %d,\n", stuUps_Param->battery_volt);strJson += str;		//UPS电池电压172
	sprintf(str,"\"amp_charge\": %d,\n", stuUps_Param->amp_charge);strJson += str;		//UPS充电电流173
	sprintf(str,"\"amp_discharge\": %d,\n", stuUps_Param->amp_discharge);strJson += str;		//UPS放电电流174
	sprintf(str,"\"battery_left\": %d,\n", stuUps_Param->battery_left);strJson += str;		// UPS电池后备时间175
	sprintf(str,"\"battery_tmp\": %d,\n", stuUps_Param->battery_tmp);strJson += str;		//环境温度176
	sprintf(str,"\"battery_capacity\": %d,\n", stuUps_Param->battery_capacity);strJson += str;		//电池当前容量177
	sprintf(str,"\"battery_dischg_times\": %d,\n", stuUps_Param->battery_dischg_times);strJson += str;		//电池放电次数178

	//防雷器
	//sprintf(str,"\"status\": %d,\n", stuSpd_Param->status);strJson += str;		//防雷器在线状态179
	//sprintf(str,"\"struck_times\": %d,\n", stuSpd_Param->struck_times);strJson += str;		//雷击次数180

	//告警信息
	sprintf(str,"\"water_flag\": %d,\n", stuEnvi_Param->water_flag);strJson += str;		//漏水181
	sprintf(str,"\"front_door_flag\": %d,\n", stuEnvi_Param->front_door_flag);strJson += str;		//前柜门开关状态182
	sprintf(str,"\"back_door_flag\": %d,\n", stuEnvi_Param->back_door_flag);strJson += str;		//后柜门开关状态183
	sprintf(str,"\"door_overtime\": %d,\n", stuEnvi_Param->door_overtime);strJson += str;		//柜门开启超时184
	sprintf(str,"\"smoke_event_flag\": %d,\n", stuEnvi_Param->smoke_event_flag);strJson += str;		//烟雾报警185
	sprintf(str,"\"air_cond_hightemp_alarm\": %d,\n", stuEnvi_Param->air_cond_hightemp_alarm);strJson += str;		//空调高温告警186
	sprintf(str,"\"air_cond_lowtemp_alarm\": %d,\n", stuEnvi_Param->air_cond_lowtemp_alarm);strJson += str;		//空调低温告警187
	sprintf(str,"\"air_cond_highmoist_alarm\": %d,\n", stuEnvi_Param->air_cond_highmoist_alarm);strJson += str;		//空调高湿告警188
	sprintf(str,"\"air_cond_lowmoist_alarm\": %d,\n", stuEnvi_Param->air_cond_lowmoist_alarm);strJson += str;		//空调低湿告警189
	sprintf(str,"\"air_cond_infan_alarm\": %d,\n", stuEnvi_Param->air_cond_infan_alarm);strJson += str;		//空调内风机故障190
	sprintf(str,"\"air_cond_outfan_alarm\": %d,\n", stuEnvi_Param->air_cond_outfan_alarm);strJson += str;		//空调外风机故障191
	sprintf(str,"\"air_cond_comp_alarm\": %d,\n", stuEnvi_Param->air_cond_comp_alarm);strJson += str;		//空调压缩机故障192
	sprintf(str,"\"air_cond_heater_alarm\": %d,\n", stuEnvi_Param->air_cond_heater_alarm);strJson += str;		//空调电加热故障193
	sprintf(str,"\"air_cond_emgyfan_alarm\": %d,\n", stuEnvi_Param->air_cond_emgyfan_alarm);strJson += str;		//空调应急风机故障194
	sprintf(str,"\"supply_out_status\": %d,\n", stuUps_Param->supply_out_status);strJson += str;		//输出供电状态195
	sprintf(str,"\"supply_in_status\": %d,\n", stuUps_Param->supply_in_status);strJson += str;		//输入供电状态196
	sprintf(str,"\"battery_status\": %d,\n", stuUps_Param->battery_status);strJson += str;		//电池状态197
	sprintf(str,"\"main_abnormal\": %d,\n", stuUps_Param->main_abnormal);strJson += str;		//主路异常198
	sprintf(str,"\"system_overtemp\": %d,\n", stuUps_Param->system_overtemp);strJson += str;		//系统过温199
	sprintf(str,"\"sysbat_low_prealarm\": %d,\n", stuUps_Param->sysbat_low_prealarm);strJson += str;		//系统电池电量低预告警200
	sprintf(str,"\"rectifier_overload\": %d,\n", stuUps_Param->rectifier_overload);strJson += str;		//整流器过载201
	sprintf(str,"\"inverter_overload\": %d,\n", stuUps_Param->inverter_overload);strJson += str;		//逆变器过载202
	sprintf(str,"\"bypass_abnomal\": %d,\n", stuUps_Param->bypass_abnomal);strJson += str;		//旁路异常203
	sprintf(str,"\"battery_low_prealarm\": %d,\n", stuUps_Param->battery_low_prealarm);strJson += str;		//电池电压低204
	sprintf(str,"\"battery_abnomal\": %d,\n", stuUps_Param->battery_abnomal);strJson += str;		//电池电压异常205
	sprintf(str,"\"battery_overtemp\": %d,\n", stuUps_Param->battery_overtemp);strJson += str;		//电池过温206
	sprintf(str,"\"fan_abnormal\": %d,\n", stuUps_Param->fan_abnormal);strJson += str;		//风扇故障207
	sprintf(str,"\"shutdown_alarm\": %d,\n", stuUps_Param->shutdown_alarm);strJson += str;		//紧急关机告警208
	sprintf(str,"\"maintain_status\": %d,\n", stuUps_Param->maintain_status);strJson += str;		//维修模式209
	sprintf(str,"\"inverter_supply\": %d,\n", stuUps_Param->inverter_supply);strJson += str;		//电池逆变供电210
	sprintf(str,"\"bypass_supply\": %d,\n", stuUps_Param->bypass_supply);strJson += str;		//旁路供电211

    pthread_mutex_lock(&snmpoidMutex);

	//华为机柜字段
    strJson = strJson + "\"vmctrl_ipaddr\": \"" + StrIP.c_str() + "\",\n";	//控制器IP地址 212
    strJson = strJson + "\"cabinettype\": " + StrCabinetType.c_str() + ",\n";	//机柜类型 213
    strJson = strJson + "\"hwacbgroupbatvolt\": " + HUAWEIDevValue.strhwAcbGroupBatVolt.c_str() + ",\n";	//锂电电池电压 214
    strJson = strJson + "\"hwacbgroupbatcurr\": " + HUAWEIDevValue.strhwAcbGroupBatCurr.c_str() + ",\n";	//锂电电池电流 215
    strJson = strJson + "\"hwacbgrouptotalcapacity\": " + HUAWEIDevValue.strhwAcbGroupTotalCapacity.c_str() + ",\n";	//锂电电池总容量 216
    strJson = strJson + "\"hwacbgrouptotalremaincapacity\": " + HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str() + ",\n";	//锂电电池剩余容量 217
    strJson = strJson + "\"hwacbgroupbackuptime\": " + HUAWEIDevValue.strhwAcbGroupBackupTime.c_str() + ",\n";	//电池备电时长 218
    strJson = strJson + "\"hwacbgroupbatsoh\": " + HUAWEIDevValue.strhwAcbGroupBatSoh.c_str() + ",\n";	//电池SOH 219
    strJson = strJson + "\"hwaporablvoltage\": " + HUAWEIDevValue.strhwApOrAblVoltage.c_str() + ",\n";	//A/AB电压 220
    strJson = strJson + "\"hwbporbclvoltage\": " + HUAWEIDevValue.strhwBpOrBclVoltage.c_str() + ",\n";	//B/BC电压 221
    strJson = strJson + "\"hwcporcalvoltage\": " + HUAWEIDevValue.strhwCpOrCalVoltage.c_str() + ",\n";	//C/CA电压 222
    strJson = strJson + "\"hwaphasecurrent\": " + HUAWEIDevValue.strhwAphaseCurrent.c_str() + ",\n";	//A相电流 223
    strJson = strJson + "\"hwbphasecurrent\": " + HUAWEIDevValue.strhwBphaseCurrent.c_str() + ",\n";	//B相电流 224
    strJson = strJson + "\"hwcphasecurrent\": " + HUAWEIDevValue.strhwCphaseCurrent.c_str() + ",\n";	//C相电流 225
    strJson = strJson + "\"hwdcoutputvoltage\": " + HUAWEIDevValue.strhwDcOutputVoltage.c_str() + ",\n";	//DC输出电压 226
    strJson = strJson + "\"hwdcoutputcurrent\": " + HUAWEIDevValue.strhwDcOutputCurrent.c_str() + ",\n";	//DC输出电流 227
    strJson = strJson + "\"hwenvtemperature\": " + HUAWEIDevValue.strhwEnvTemperature[0].c_str() + ",\n";	//环境温度值 228
    strJson = strJson + "\"hwenvtemperature2\": " + HUAWEIDevValue.strhwEnvTemperature[1].c_str() + ",\n";	//环境温度值 228
    strJson = strJson + "\"hwenvhumidity\": " + HUAWEIDevValue.strhwEnvHumidity[0].c_str() + ",\n";	//环境湿度值 229
    strJson = strJson + "\"hwenvhumidity2\": " + HUAWEIDevValue.strhwEnvHumidity[1].c_str() + ",\n";	//环境湿度值 229
    strJson = strJson + "\"hwdcairctrlmode\": " + HUAWEIDevValue.strhwDcAirCtrlMode[0].c_str() + ",\n";	//空调控制模式 230
    strJson = strJson + "\"hwdcairctrlmode2\": " + HUAWEIDevValue.strhwDcAirCtrlMode[1].c_str() + ",\n";	//空调控制模式 230
    strJson = strJson + "\"hwdcairrunstatus\": " + HUAWEIDevValue.strhwDcAirRunStatus[0].c_str() + ",\n";	//空调运行状态 231
    strJson = strJson + "\"hwdcairrunstatus2\": " + HUAWEIDevValue.strhwDcAirRunStatus[1].c_str() + ",\n";	//空调运行状态 231
    strJson = strJson + "\"hwdcaircompressorrunstatus\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus[0].c_str() + ",\n";	//空调压缩机运行状态 232
    strJson = strJson + "\"hwdcaircompressorrunstatus2\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus[1].c_str() + ",\n";	//空调压缩机运行状态 232
    strJson = strJson + "\"hwdcairinnrfanspeed\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed[0].c_str() + ",\n";	//空调内机转速 233
	strJson = strJson + "\"hwdcairinnrfanspeed2\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed[1].c_str() + ",\n";	//空调内机转速 233
	strJson = strJson + "\"hwdcairouterfanspeed\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed[0].c_str() + ",\n";	//空调外风机转速 234
	strJson = strJson + "\"hwdcairouterfanspeed2\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed[1].c_str() + ",\n";	//空调外风机转速 234
	strJson = strJson + "\"hwdcaircompressorruntime\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime[0].c_str() + ",\n";	//空调压缩机运行时间 235
	strJson = strJson + "\"hwdcaircompressorruntime2\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime[1].c_str() + ",\n";	//空调压缩机运行时间 235
	strJson = strJson + "\"hwdcairenterchanneltemp\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp[0].c_str() + ",\n";	//空调回风口温度 236
	strJson = strJson + "\"hwdcairenterchanneltemp2\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp[1].c_str() + ",\n";	//空调回风口温度 236
	strJson = strJson + "\"hwdcairpowerontemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint[0].c_str() + ",\n";	//空调开机温度点 237
	strJson = strJson + "\"hwdcairpowerontemppoint2\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint[1].c_str() + ",\n";	//空调开机温度点 237
	strJson = strJson + "\"hwdcairpowerofftemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint[0].c_str() + ",\n";	//空调关机温度点 238
	strJson = strJson + "\"hwdcairpowerofftemppoint2\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint[1].c_str() + ",\n";	//空调关机温度点 238
    strJson = strJson + "\"hwenvtempalarmtraps\": " + HUAWEIDevAlarm.hwEnvTempAlarmTraps.c_str() + ",\n";	//环境温度告警 239
    strJson = strJson + "\"hwenvhumialarmresumetraps\": " + HUAWEIDevAlarm.hwEnvHumiAlarmTraps.c_str() + ",\n";	//环境湿度告警 240
    strJson = strJson + "\"hwdooralarmtraps\": " + HUAWEIDevAlarm.hwDoorAlarmTraps.c_str() + ",\n";	//门禁告警 241
    strJson = strJson + "\"hwwateralarmtraps\": " + HUAWEIDevAlarm.hwWaterAlarmTraps.c_str() + ",\n";	//水浸告警 242
    strJson = strJson + "\"hwsmokealarmtraps\": " + HUAWEIDevAlarm.hwSmokeAlarmTraps.c_str() + ",\n";	//烟雾告警 243
    strJson = strJson + "\"hwspdalarmtraps\": " + HUAWEIDevAlarm.hwSpareDigitalAlarmTraps.c_str() + ",\n";	//防雷器故障告警 244
    strJson = strJson + "\"hwair_cond_infan_alarm\": " + HUAWEIDevAlarm.hwair_cond_infan_alarm.c_str() + ",\n";	//空调内风机故障 245
    strJson = strJson + "\"hwair_cond_outfan_alarm\": " + HUAWEIDevAlarm.hwair_cond_outfan_alarm.c_str() + ",\n";	//空调外风机故障 246
    strJson = strJson + "\"hwair_cond_comp_alarm\": " + HUAWEIDevAlarm.hwair_cond_comp_alarm.c_str() + ",\n";	//空调压缩机故障 247
    strJson = strJson + "\"hwair_cond_return_port_sensor_alarm\": " + HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm.c_str() + ",\n";	//空调回风口传感器故障 248
    strJson = strJson + "\"hwair_cond_evap_freezing_alarm\": " + HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm.c_str() + ",\n";	//空调蒸发器冻结 249
    strJson = strJson + "\"hwair_cond_freq_high_press_alarm\": " + HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm.c_str() + ",\n";	//空调频繁高压力 250
    strJson = strJson + "\"hwair_cond_comm_fail_alarm\": " + HUAWEIDevAlarm.hwair_cond_comm_fail_alarm.c_str() + ",\n";	//空调通信失败告警 251
	//新增加的状态
	//设备信息
    strJson = strJson + "\"hwmonequipsoftwareversion\": \"" + HUAWEIDevValue.strhwMonEquipSoftwareVersion.c_str() + "\",\n";	//软件版本
    strJson = strJson + "\"hwmonequipmanufacturer\": \"" + HUAWEIDevValue.strhwMonEquipManufacturer.c_str() + "\",\n";	//设备生产商
	//锂电(新增加)
    strJson = strJson + "\"hwacbgrouptemperature\": " + HUAWEIDevValue.strhwAcbGroupTemperature.c_str() + ",\n";	//电池温度
    strJson = strJson + "\"hwacbgroupovercurthr\": " + HUAWEIDevValue.strhwAcbGroupOverCurThr.c_str() + ",\n";	//充电过流告警点
    strJson = strJson + "\"hwacbgrouphightempthr\": " + HUAWEIDevValue.strhwAcbGroupHighTempThr.c_str() + ",\n";	//高温告警点
    strJson = strJson + "\"hwacbgrouplowtempth\": " + HUAWEIDevValue.strhwAcbGroupLowTempTh.c_str() + ",\n";	//低温告警点
    strJson = strJson + "\"hwacbgroupdodtoacidbattery\": " + HUAWEIDevValue.strhwAcbGroupDodToAcidBattery.c_str() + ",\n";	//锂电放电DOD
	//开关电源(新增加)
    strJson = strJson + "\"hwsetacsuppervoltlimit\": " + HUAWEIDevValue.strhwSetAcsUpperVoltLimit.c_str() + ",\n";	//AC过压点设置
    strJson = strJson + "\"hwsetacslowervoltlimit\": " + HUAWEIDevValue.strhwSetAcsLowerVoltLimit.c_str() + ",\n";	//AC欠压点设置
    strJson = strJson + "\"hwsetdcsuppervoltlimit\": " + HUAWEIDevValue.strhwSetDcsUpperVoltLimit.c_str() + ",\n";	//设置DC过压点
    strJson = strJson + "\"hwsetdcslowervoltlimit\": " + HUAWEIDevValue.strhwSetDcsLowerVoltLimit.c_str() + ",\n";	//设置DC欠压点
    strJson = strJson + "\"hwsetlvdvoltage\": " + HUAWEIDevValue.strhwSetLvdVoltage.c_str() + ",\n";	//设置LVD电压
	//环境传感器(新增加)
    strJson = strJson + "\"hwsetenvtempupperlimit\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit[0].c_str() + ",\n";	//环境温度告警上限
        strJson = strJson + "\"hwsetenvtempupperlimit2\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit[1].c_str() + ",\n";	//环境温度告警上限
    strJson = strJson + "\"hwsetenvtemplowerlimit\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit[0].c_str() + ",\n";	//环境温度告警下限
        strJson = strJson + "\"hwsetenvtemplowerlimit2\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit[1].c_str() + ",\n";	//环境温度告警下限
//    strJson = strJson + "\"hwsetenvtempultrahightempthreshold\": " + HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold.c_str() + ",\n";	//环境高高温告警点
    strJson = strJson + "\"hwsetenvhumidityupperlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[0].c_str() + ",\n";	//环境湿度告警上限
        strJson = strJson + "\"hwsetenvhumidityupperlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[1].c_str() + ",\n";	//环境湿度告警上限
    strJson = strJson + "\"hwsetenvhumiditylowerlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[0].c_str() + ",\n";	//环境湿度告警下限
        strJson = strJson + "\"hwsetenvhumiditylowerlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[1].c_str() + ",\n";	//环境湿度告警下限
	//直流空调(新增加)
    strJson = strJson + "\"hwdcairruntime\": " + HUAWEIDevValue.strhwDcAirRunTime[0].c_str() + ",\n";	//空调运行时间
        strJson = strJson + "\"hwdcairruntime2\": " + HUAWEIDevValue.strhwDcAirRunTime[1].c_str() + ",\n";	//空调运行时间
    strJson = strJson + "\"hwcoolingdevicesmode\": " + HUAWEIDevValue.strhwCoolingDevicesMode.c_str() + ",\n";	//温控模式
    //防火墙
    strJson = strJson + "\"hwentitycpuusage\": " + HUAWEIDevValue.strhwEntityCpuUsage.c_str() + ",\n";	//CPU
    strJson = strJson + "\"hwentitymemusage\": " + HUAWEIDevValue.strhwEntityMemUsage.c_str() + ",\n";	//内存使用率
    strJson = strJson + "\"hwentitytemperature\": " + HUAWEIDevValue.strhwEntityTemperature.c_str() + ",\n";	//温度
    //交换机
    strJson = strJson + "\"hwswitchentitycpuusage\": " + HUAWEIDevValue.strhwswitchEntityCpuUsage.c_str() + ",\n";	//CPU
    strJson = strJson + "\"hwswitchentitymemusage\": " + HUAWEIDevValue.strhwswitchEntityMemUsage.c_str() + ",\n";	//内存使用率
    strJson = strJson + "\"hwswitchentitytemperature\": " + HUAWEIDevValue.strhwswitchEntityTemperature.c_str() + ",\n";	//温度
    strJson = strJson + "\"ishandle\": 0\n";	//告警处理标记

    pthread_mutex_unlock(&snmpoidMutex);

	strJson +=	" }\n";
	strJson +=	" }\n";
	strJson +=	"}\n\0";

    mstrjson = strJson;
    //*len=strJson.length();
    //memcpy(json,(char*)strJson.c_str(),*len);

}

void SetjsonLTAlarmTableStr(char* table, string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,vehplatecnt;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson,strTable;
	strTable = table;

    strJson +=  "{\n";
    strJson +=  "\"param\": {\n";
	strJson +=	"\"action\":\""+ strTable + "\",\n";
	strJson +=	"\"entity\": {\n";

	sprintf(str,"\"id\":\"%s%04d%02d%02d%02d%02d%02d\",\n",StrFlagID.c_str(),pTM->tm_year + 1900,
				pTM->tm_mon + 1, pTM->tm_mday,pTM->tm_hour, pTM->tm_min, pTM->tm_sec); strJson = strJson + str;//id
	strJson = strJson + "\"gantryid\": \"" + StrFlagID +"\",\n";			//门架编号
	strJson = strJson + "\"time\": \"" + sDateTime + "\",\n";	//状态数据生成时间
	strJson = strJson + "\"direction\": \"" + StrDirection +"\",\n";		//6 行车方向
	strJson = strJson + "\"dirdescription\": \"" + StrDirDescription +"\",\n";		//7 行车方向说明
    strJson = strJson + "\"vmctrl_ipaddr\": \"" + StrIP.c_str() + "\",\n";	//控制器IP地址 212

	//ETC门架关键设备运行状态告警
	sprintf(str,"\"frontsoftstatus\": %d,\n", stuFlagRunStatus->SoftWare);strJson += str;		//前端软件运行状态
	sprintf(str,"\"fronthardwarestatus\": 0,\n");strJson += str;		//前端硬件资源可用性
	sprintf(str,"\"frontbizdiskstatus\": %d,\n", stuFlagRunStatus->DiskUsage);strJson += str;		//前端应用盘磁盘使用状态
	sprintf(str,"\"frontdatadiskstatus\": 1,\n");strJson += str;		//前端数据盘使用状态
	sprintf(str,"\"backsoftstatus\": 0,\n");strJson += str;		//后台软件运行状态
	sprintf(str,"\"backhardwarestatus\": 1,\n");strJson += str;		//后台硬件资源可用性
	sprintf(str,"\"backbizdiskstatus\": 1,\n");strJson += str;		//后台应用盘磁盘使用状态
	sprintf(str,"\"backdatadiskstatus\": 1,\n");strJson += str;		//后台数据盘使用状态
	sprintf(str,"\"rsustatus\": 1,\n");strJson += str;		//RSU 状态
	sprintf(str,"\"vplrstatus\": 1,\n");strJson += str;		//车牌识别状态
	sprintf(str,"\"vehdetectorstatus\": 1,\n");strJson += str;		//车检器状态
	sprintf(str,"\"classdetectorstatus\": 1,\n");strJson += str;		//车型检测器状态
	sprintf(str,"\"loaddetectorstatus\": 1,\n");strJson += str;		//载重检测器状态
	sprintf(str,"\"controlstatusrsu\": 1,\n");strJson += str;		//控制器状态
	sprintf(str,"\"controlnetwrok\": 1,\n");strJson += str;		//RSU 主备控制器之间的网络连接状态

    pthread_mutex_lock(&snmpoidMutex);

	//华为机柜字段
	//设备信息
    strJson = strJson + "\"monequipsoftwareversion\": \"" + HUAWEIDevValue.strhwMonEquipSoftwareVersion.c_str() + "\",\n";	//软件版本
//    strJson = strJson + "\"monequipmanufacturer\": \"" + HUAWEIDevValue.strhwMonEquipManufacturer.c_str() + "\",\n";	//设备生产商
    strJson = strJson + "\"cabinettype\": " + StrCabinetType.c_str() + ",\n";	//机柜类型 213
    strJson = strJson + "\"acbgroupbatvolt\": " + HUAWEIDevValue.strhwAcbGroupBatVolt.c_str() + ",\n";	//锂电电池电压 214
    strJson = strJson + "\"envtempalarmtraps\": " + HUAWEIDevAlarm.hwEnvTempAlarmTraps.c_str() + ",\n";	//环境温度告警 239
    strJson = strJson + "\"envhumialarmresumetraps\": " + HUAWEIDevAlarm.hwEnvHumiAlarmTraps.c_str() + ",\n";	//环境湿度告警 240
    strJson = strJson + "\"dooralarmtraps\": " + HUAWEIDevAlarm.hwDoorAlarmTraps.c_str() + ",\n";	//门禁告警 241
    strJson = strJson + "\"wateralarmtraps\": " + HUAWEIDevAlarm.hwWaterAlarmTraps.c_str() + ",\n";	//水浸告警 242
    strJson = strJson + "\"smokealarmtraps\": " + HUAWEIDevAlarm.hwSmokeAlarmTraps.c_str() + ",\n";	//烟雾告警 243
	//供配电
	//电源告警
	sprintf(str,"\"acinputfailalarm\": %d,\n", HUAWEIDevAlarm.hwAcInputFailAlarm.c_str());strJson += str;		 //交流电源输入停电告警
	sprintf(str,"\"acinputl1failalarm\": %d,\n", HUAWEIDevAlarm.hwAcInputL1FailAlarm.c_str());strJson += str;		 //交流电源输入L1	相缺相告警
	sprintf(str,"\"acinputl2failalarm\": %d,\n", HUAWEIDevAlarm.hwAcInputL2FailAlarm.c_str());strJson += str;		 //交流电源输入L2	相缺相告警
	sprintf(str,"\"acinputl3failalarm\": %d,\n", HUAWEIDevAlarm.hwAcInputL3FailAlarm.c_str());strJson += str;		 //交流电源输入L3	相缺相告警
	sprintf(str,"\"dcvoltalarmtraps\": %d,\n", HUAWEIDevAlarm.hwDcVoltAlarmTraps.c_str());strJson += str;		 //直流电源输出告警
	sprintf(str,"\"loadlvdalarmtraps\": %d,\n", HUAWEIDevAlarm.hwLoadLvdAlarmTraps.c_str());strJson += str;		 //LLVD1下电告警
	//锂电池告警
	sprintf(str,"\"acbgroup_comm_fail_alarm\": %d,\n", HUAWEIDevAlarm.hwAcbGroup_comm_fail_alarm.c_str());strJson += str;		 //所有锂电通信失败
	sprintf(str,"\"acbgroup_discharge_alarm\": %d,\n", HUAWEIDevAlarm.hwAcbGroup_discharge_alarm.c_str());strJson += str;		 //电池放电告警
	sprintf(str,"\"acbgroup_charge_overcurrent_alarm\": %d,\n", HUAWEIDevAlarm.hwAcbGroup_charge_overcurrent_alarm.c_str());strJson += str;		 //电池充电过流
	sprintf(str,"\"acbgroup_temphigh_alarm\": %d,\n", HUAWEIDevAlarm.hwAcbGroup_temphigh_alarm.c_str());strJson += str;		 //电池温度高
	sprintf(str,"\"acbgroup_templow_alarm\": %d,\n", HUAWEIDevAlarm.hwAcbGroup_templow_alarm.c_str());strJson += str;		 //电池温度低
	sprintf(str,"\"acbgroup_poweroff_alarm\": %d,\n", HUAWEIDevAlarm.hwAcbGroup_poweroff_alarm.c_str());strJson += str;		 //电池下电
	sprintf(str,"\"acbgroup_fusebreak_alarm\": %d,\n", HUAWEIDevAlarm.hwAcbGroup_fusebreak_alarm.c_str());strJson += str;		 //电池熔丝断
	sprintf(str,"\"acbgroup_moduleloss_alarm\": %d,\n", HUAWEIDevAlarm.hwAcbGroup_moduleloss_alarm.c_str());strJson += str;		 //模块丢失
	//防雷器告警
    strJson = strJson + "\"acspdalarmtraps\": " + HUAWEIDevAlarm.hwACSpdAlarmTraps.c_str() + ",\n";	//交流防雷器故障
    strJson = strJson + "\"dcspdalarmtraps\": " + HUAWEIDevAlarm.hwDCSpdAlarmTraps.c_str() + ",\n";	//直流防雷器故障
	//直流空调告警
    strJson = strJson + "\"air_cond_infan_alarm\": " + HUAWEIDevAlarm.hwair_cond_infan_alarm.c_str() + ",\n";	//空调内风机故障 245
    strJson = strJson + "\"air_cond_outfan_alarm\": " + HUAWEIDevAlarm.hwair_cond_outfan_alarm.c_str() + ",\n";	//空调外风机故障 246
    strJson = strJson + "\"air_cond_comp_alarm\": " + HUAWEIDevAlarm.hwair_cond_comp_alarm.c_str() + ",\n";	//空调压缩机故障 247
    strJson = strJson + "\"air_cond_return_port_sensor_alarm\": " + HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm.c_str() + ",\n";	//空调回风口传感器故障 248
    strJson = strJson + "\"air_cond_evap_freezing_alarm\": " + HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm.c_str() + ",\n";	//空调蒸发器冻结 249
    strJson = strJson + "\"air_cond_freq_high_press_alarm\": " + HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm.c_str() + ",\n";	//空调频繁高压力 250
    strJson = strJson + "\"air_cond_comm_fail_alarm\": " + HUAWEIDevAlarm.hwair_cond_comm_fail_alarm.c_str() + ",\n";	//空调通信失败告警 251
    strJson = strJson + "\"ishandle\": 0\n";	//告警处理标记

    pthread_mutex_unlock(&snmpoidMutex);

	strJson +=	" }\n";
	strJson +=	" }\n";
	strJson +=	"}\n\0";

    mstrjson = strJson;
    //*len=strJson.length();
    //memcpy(json,(char*)strJson.c_str(),*len);

}


//17 门架关键设备状态数据-新部标准
void SetjsongantryRunStatus(char* table, string &mstrjson)
{
    char str[100],sDateTime1[30];
    char sDateTime2[30];
    int i,j,vehplatecnt;
    static int id=0;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    memset(sDateTime1,0x00,30);
    memset(sDateTime2,0x00,30);
    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime1, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

    //YYYY-MM-DDTHH:mm:ss
    sprintf(sDateTime2, "%04d%02d%02d%02d%02d%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

    string strstateNo;
    strstateNo = StrFlagID + sDateTime2;
    std::string strJson,strTable;
    strTable = table;

    strJson +=  "{\n";
    strJson +=  "\"param\": [\n";
    strJson +=	"{\"action\":\"gantryRunStatus\",\n";
    strJson +=	"\"entity\": {\n";

    strJson = strJson + "\"runstateid\":\""+ strstateNo +"\",\n";		//状态流水号
    strJson = strJson + "\"gantryid\": \"" + StrFlagID +"\",\n";			// ETC 门架编号
    strJson = strJson + "\"statetime\": \"" + sDateTime1 + "\",\n";	//状态数据生成时间
//    strJson = strJson + "\"otherversion\": \"\",\n";	//其它参数版本号
    strJson = strJson + "\"frontrateversion\": \"\",\n";	//计费模块和计费参数版本号
    strJson = strJson + "\"frontparamversion\": \"\",\n";	//前端运行参数版本号
    strJson = strJson + "\"frontsysversion\": \"\",\n";	//门架前端控制机操作系统软件版本
    strJson = strJson + "\"frontsoftversion\": \"LT00201907220006000B\",\n";	//前端软件版本号
    memset(str,0x00,100);
    sprintf(str,"\"frontsoftstate\": \"%d\",\n", stuFlagRunStatus->SoftWare);strJson += str;		//前端软件运行状态
    strJson = strJson + "\"frontovertradecount\": -1,\n";	//前端积压通行流水数
    strJson = strJson + "\"fronttranserrtradecount\": -1,\n";	//前端传输异常流水数
    strJson = strJson + "\"frontovervehdatacount\": -1,\n";	//前端积压牌识流水数
    strJson = strJson + "\"fronttranserrvehdatacount\": -1,\n";	    //前端传输异常牌识流水数
    strJson = strJson + "\"frontloadaverage\": \"\",\n";	    //前端硬件资源平均负载（5 分钟）
    strJson = strJson + "\"frontdiskdatatotalsize\": 100,\n";	    //前端数据盘总容量
    strJson = strJson + "\"frontdiskdataleftsize\": 100,\n";	    //前端数据盘剩余容量
    strJson = strJson + "\"frontdiskruntotalsize\": 100,\n";	    //前端运行盘总容量
    strJson = strJson + "\"frontdiskrunleftsize\": 100,\n";	    //前端运行盘剩余容量
    strJson = strJson + "\"frontcpurate\": \"5\",\n";	    //前端 CPU 使用率
    strJson = strJson + "\"frontmemoryrate\": \"5\",\n";	    //前端物理内存使用率
    strJson = strJson + "\"frontbeidoustate\": \"0\",\n";	    //前端北斗授时服务状态
    strJson = strJson + "\"backsysversion\": \"0\",\n";	    //门架后台服务器操作系统软件版本
    strJson = strJson + "\"backdbversion\": \"0\",\n";	    //门架后台服务器数据库系统软件版本
    strJson = strJson + "\"backparamversion\": \"0\",\n";	    //后台运行参数版本号
    strJson = strJson + "\"backsoftversion\": \"\",\n";	       //后台版本号
    strJson = strJson + "\"backsoftstatus\": \"0\",\n";	       //后台软件运行状态
    strJson = strJson + "\"backovertradecount\": -1,\n";	   //后台积压通行流水数
    strJson = strJson + "\"backtranserrtradecount\": -1,\n";	       //后台传输异常流水数
    strJson = strJson + "\"backovervehdatacount\": -1,\n";	   //后台积压牌识流水数
    strJson = strJson + "\"backtranserrvehdatacount\": -1,\n";	   //后台积压牌识流水数
    strJson = strJson + "\"backoverpiccount\": -1,\n";	       //后台积压牌识图片数
    strJson = strJson + "\"backcomputerstate\": \"1\",\n";	    //后台工作主机
    strJson = strJson + "\"backloadaverage\": \"5\",\n";	    //后台硬件资源平均负载（5 分钟）
    strJson = strJson + "\"backdiskdatatotalsize\": 100,\n";	    //后台数据盘总容量
    strJson = strJson + "\"backdiskdataleftsize\": 100,\n";	    //后台数据盘剩余容量
    strJson = strJson + "\"backdiskruntotalsize\": 100,\n";	    //后台运行盘总容量
    strJson = strJson + "\"backdiskrunleftsize\": 100,\n";	    //后台运行盘剩余容量
    strJson = strJson + "\"backcpurate\": \"5\",\n";	    //后台 CPU 使用率
    strJson = strJson + "\"backmemoryrate\": \"5\",\n";	    //后台物理内存使用率
    strJson = strJson + "\"backbeidoustate\": \"0\",\n";	    //后台北斗授时服务

	//状态 1-正常响应  	2-无设备
	sprintf(str,"\"rsustatus\":\"%d\",\n",stuRsuControl.ControlStatusN[0]);strJson = strJson + str; //RSU状态
	sprintf(str,"111");
/*	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(str,"%s%d",str,mTIPcamState[i].statuscode.c_str());	//连接状态 0-正常 1-异常
	}*/
    strJson = strJson + "\"vplrstatus\": \""+str+"\",\n";	           //车牌识别状态
    strJson = strJson + "\"vplrversion\": \"1.0.0.1\",\n";	       //车牌识别软件版本号
    strJson = strJson + "\"vehdetectorstatus\": \"1\",\n";	       //车检器状态
    strJson = strJson + "\"vehdetectorversion\": \"1.0.0.1\",\n";  //车检器软件版本号
    strJson = strJson + "\"weatherdetectorstatus\": \"1\",\n";  //气象检测设备状态
    strJson = strJson + "\"weatherdetectorversion\": \"1.0.0.1\",\n";  //气象检测设备软件版本号

    strJson = strJson + "\"classdetectorstatus\": \"1\",\n";	   //车型检测器状态
    strJson = strJson + "\"classdetectorversio\": \"1.0.0.1\",\n"; //车型检测器软件版本号
    strJson = strJson + "\"loaddetectorstatus\": \"1\",\n";	       //载重检测器状态
    strJson = strJson + "\"loaddetectorversion\": \"1.0.0.1\",\n";	       //载重检测器软件版本号
    strJson = strJson + "\"vehiclecount\": \"1\",\n";	       //过车监测
	sprintf(str,"\"cameracount\":\"%s\",\n",StrVehPlateCount.c_str()); strJson = strJson + str;//车牌图像识别设备数量

	sprintf(str,"\"controlid\":\"%02x%02x%02x\",\n",stuRsuData.RSUID[0],stuRsuData.RSUID[1],stuRsuData.RSUID[2]); strJson = strJson + str;//RSU 控制器编号
	sprintf(str,"\"controlstatus\":\"%d\",\n",stuRsuControl.ControlStatusN[0]);strJson = strJson + str; //RSU 控制器状态
	sprintf(str,"\"psamnum\":\"%d\",\n",stuRsuData.PSAMCount); strJson = strJson + str;//Psam 数量
	sprintf(str,"\"rsumanuid\":\"%02x\",\n",stuRsuData.RSUManuID); strJson = strJson + str;//路侧单元厂商代码
	sprintf(str,"\"rsuid\":\"%d\",\n",stuRsuData.ControlId); strJson = strJson + str;//路侧单元编号
	sprintf(str,"\"rsuupdateversion\":\"%02x%02x\",\n",stuRsuData.RSUVersion[0],stuRsuData.RSUVersion[1]); strJson = strJson + str;//路侧单元更新包版本号
	sprintf(str,"\"rsuhardwareversion\":\"%02x%02x\",\n",stuRsuData.RSUVersion[0],stuRsuData.RSUVersion[1]); strJson = strJson + str;//路侧单元硬件版本
	sprintf(str,"\"rsusoftwareversion\":\"%02x%02x\",\n",stuRsuData.RSUVersion[0],stuRsuData.RSUVersion[1]); strJson = strJson + str;//路侧单元软件版本
    strJson = strJson + "\"rsustorestatus\": \"1\",\n";	       //路侧单元存储使用状态
	strJson = strJson + "\"rsuip\":\""+ StrRSUIP[0] +"\",\n";	//路侧单元网络 IP地址
    strJson = strJson + "\"rsutemperature\": \"1\",\n";	       //路侧单元控制板温度
	sprintf(str,"\"controlnetwrok\":\"%d\",\n",stuRsuControl.ControlStatusN[0]);strJson = strJson + str; //RSU 主备控制器之间的网络连接状态
    strJson = strJson + "\"antennahardwareversion\": \"1\",\n";	       //天线头硬件版本
    strJson = strJson + "\"antennasoftwareversion\": \"1\",\n";	       //天线头软件版本
	sprintf(str,"\"antennanum\":\"%d\",\n",stuRsuControl.AntennaCount);strJson = strJson + str;	//RSU 天线头数量


    strJson = strJson + "\"detectordeviceid\": \"1\",\n";	       //车检器设备编号
    strJson = strJson + "\"detectorstatustime\": \"1\",\n";	       //车检器状态检测时间
    strJson = strJson + "\"detectorstatuscode\": \"0\",\n";	       //车检器状态代码
    strJson = strJson + "\"detectorstatusdesc\": \"1\",\n";	       //车检器状态的文字描述
    strJson = strJson + "\"detectorsoftwareversion\": \"1\"\n";   //车检器软件版本

    strJson +=	" }\n";
    strJson +=	"},\n";

	vehplatecnt=atoi(StrVehPlateCount.c_str());
	for(i=0;i<vehplatecnt;i++)
	{
	    strJson +=	"{\"action\":\"cameraheartbeatlist\",\n";
	    strJson +=	"\"entity\": {\n";
	    strJson = strJson + "\"runstateid\":\""+ strstateNo +"\",\n";		//状态流水号
		sprintf(str,"\"cameranum\":\"%d\",\n",101+i);strJson+=str;	//相机编号（101~299）
	    sprintf(str,"\"lanenum\": \"%d\",\n",51+i);	strJson = strJson + str;	       //车道编号
		sprintf(str,"\"connectstatus\":\"%s\",\n",mTIPcamState[i].statuscode.c_str());strJson+=str;	//连接状态 0-正常 1-异常
		sprintf(str,"\"workstatus\":\"%s\",\n",mTIPcamState[i].statuscode.c_str());strJson+=str;	//工作状态 0-正常 1-异常
		sprintf(str,"\"lightworkstatus\":\"%s\",\n",mTIPcamState[i].filllight.c_str());strJson+=str;	//补光灯的工作状态 0:正常，1:异常
	    strJson = strJson + "\"recognitionrate\": \"90\",\n";	       //识别成功率
		sprintf(str,"\"hardwareversion\":%s,\n",mTIPcamState[i].softversion.c_str());strJson+=str;	//固件版本
		sprintf(str,"\"softwareversion\":%s,\n",mTIPcamState[i].softversion.c_str());strJson+=str;	//软件版本
		sprintf(str,"\"runningtime\":\"0\",\n");strJson+=str;	//设备从开机到现在的运行时间（秒）
		sprintf(str,"\"brand\":\"%s\",\n",mTIPcamState[i].factoryid.c_str());strJson+=str;	//厂商型号
		sprintf(str,"\"devicetype\":%s,\n",mTIPcamState[i].devicemodel.c_str());strJson+=str;	//摄相机的设备型号
		sprintf(str,"\"statuscode\":\"%s\",\n",mTIPcamState[i].errcode.c_str());strJson+=str;	//状态码 正常时为0，状态异常时为厂商自定义的错误代码
	    strJson = strJson + "\"statusmsg\": \"\"\n";	       //状态描述
	    strJson +=	" }\n";
	    strJson +=	"},\n";
	}

	for(i=0;i<stuRsuData.PSAMCount ;i++)
	{
	    strJson +=	"{\"action\":\"psaminfolist\",\n";
	    strJson +=	"\"entity\": {\n";
	    strJson = strJson + "\"runstateid\":\""+ strstateNo +"\",\n";		//状态流水号
		sprintf(str,"\"channelid\": \"%d\",\n", i+1); strJson += str;	//PSAM 卡插槽号或PCI 通道号
	    strJson = strJson + "\"psamstatus\": \"0\"\n";	       //PSAM 状态
	    strJson +=	" }\n";
	    strJson +=	"},\n";
	}

	for(i=0;i<stuRsuControl.AntennaCount;i++)
	{
	    strJson +=	"{\"action\":\"antennalinfolist\",\n";
	    strJson +=	"\"entity\": {\n";
	    strJson = strJson + "\"runstateid\":\""+ strstateNo +"\",\n";		//状态流水号
		sprintf(str,"\"antennaid\": \"%d\",\n", i+1); strJson += str; 	//天线头编号
		sprintf(str,"\"status\": \"%d\"\n", i+1,stuRsuControl.AntennaInfoN[i].Status);strJson += str; 	//RSU 天线头的状态信息
		strJson +=	" }\n";
		if(i==stuRsuControl.AntennaCount-1)
			strJson +=	"}\n";
		else
			strJson +=	"},\n";
	}

    strJson = strJson + "]\n";
    strJson +=	"}\n";

    mstrjson = strJson;
    //*len=strJson.length();
    //memcpy(json,(char*)strJson.c_str(),*len);

}



//20 机柜状态数据-新部标准
void SetjsonCabinetStatus(char* table, string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,vehplatecnt;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson,strTable;
	strTable = table;

    strJson +=  "{\n";
    strJson +=  "\"param\": {\n";
	strJson +=	"\"action\":\""+ strTable + "\",\n";
	strJson +=	"\"entity\": {\n";

    pthread_mutex_lock(&snmpoidMutex);

	sprintf(str,"\"id\":\"%s%04d%02d%02d%02d%02d%02d\",\n",StrFlagID.c_str(),pTM->tm_year + 1900,
				pTM->tm_mon + 1, pTM->tm_mday,pTM->tm_hour, pTM->tm_min, pTM->tm_sec); strJson = strJson + str;//id
	strJson = strJson + "\"gantryid\": \"" + StrFlagID +"\",\n";			// ETC 门架编号
	strJson = strJson + "\"time\": \"" + sDateTime + "\",\n";	//状态数据生成时间
	strJson = strJson + "\"direction\": \"" + StrDirection +"\",\n";		//行驶方向
	strJson = strJson + "\"dirdescription\": \"" + StrDirDescription +"\",\n";		//行车方向说明
	strJson = strJson + "\"vmctrl_ipaddr\": \"" + StrIP.c_str() + "\",\n";	//控制器IP地址 212

	//华为机柜字段
    strJson = strJson + "\"cabinettype\": " + StrCabinetType.c_str() + ",\n";	//机柜类型 213
	strJson = strJson + "\"monequipsoftwareversion\": \"" + HUAWEIDevValue.strhwMonEquipSoftwareVersion.c_str() + "\",\n";	//软件版本
	strJson = strJson + "\"monequipmanufacturer\": \"" + HUAWEIDevValue.strhwMonEquipManufacturer.c_str() + "\",\n";	//设备生产商

	//环境传感器
	strJson = strJson + "\"envtemperature\": " + HUAWEIDevValue.strhwEnvTemperature[0].c_str() + ",\n";	//环境温度值 228
	strJson = strJson + "\"envtemperature2\": " + HUAWEIDevValue.strhwEnvTemperature[1].c_str() + ",\n";	//环境温度值 228
	strJson = strJson + "\"envhumidity\": " + HUAWEIDevValue.strhwEnvHumidity[0].c_str() + ",\n"; //环境湿度值 229
	strJson = strJson + "\"envhumidity2\": " + HUAWEIDevValue.strhwEnvHumidity[1].c_str() + ",\n";	//环境湿度值 229

	strJson = strJson + "\"setenvtempupperlimit\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit[0].c_str() + ",\n";	//环境温度告警上限
	strJson = strJson + "\"setenvtempupperlimit2\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit[1].c_str() + ",\n";	//环境温度告警上限
	strJson = strJson + "\"setenvtemplowerlimit\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit[0].c_str() + ",\n";	//环境温度告警下限
	strJson = strJson + "\"setenvtemplowerlimit2\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit[1].c_str() + ",\n";	//环境温度告警下限
//	strJson = strJson + "\"setenvtempultrahightempthreshold\": " + HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold.c_str() + ",\n";	//环境高高温告警点
	strJson = strJson + "\"setenvhumidityupperlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[0].c_str() + ",\n";	//环境湿度告警上限
	strJson = strJson + "\"setenvhumidityupperlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[1].c_str() + ",\n";	//环境湿度告警上限
	strJson = strJson + "\"setenvhumiditylowerlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[0].c_str() + ",\n";	//环境湿度告警下限
	strJson = strJson + "\"setenvhumiditylowerlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[1].c_str() + ",\n";	//环境湿度告警下限

	//供配电
	strJson = strJson + "\"aporablvoltage\": " + HUAWEIDevValue.strhwApOrAblVoltage.c_str() + ",\n";	//A/AB电压 220
	strJson = strJson + "\"bporbclvoltage\": " + HUAWEIDevValue.strhwBpOrBclVoltage.c_str() + ",\n";	//B/BC电压 221
	strJson = strJson + "\"cporcalvoltage\": " + HUAWEIDevValue.strhwCpOrCalVoltage.c_str() + ",\n";	//C/CA电压 222
	strJson = strJson + "\"aphasecurrent\": " + HUAWEIDevValue.strhwAphaseCurrent.c_str() + ",\n";	//A相电流 223
	strJson = strJson + "\"bphasecurrent\": " + HUAWEIDevValue.strhwBphaseCurrent.c_str() + ",\n";	//B相电流 224
	strJson = strJson + "\"cphasecurrent\": " + HUAWEIDevValue.strhwCphaseCurrent.c_str() + ",\n";	//C相电流 225
	strJson = strJson + "\"dcoutputvoltage\": " + HUAWEIDevValue.strhwDcOutputVoltage.c_str() + ",\n";	//DC输出电压 226
	strJson = strJson + "\"dcoutputcurrent\": " + HUAWEIDevValue.strhwDcOutputCurrent.c_str() + ",\n";	//DC输出电流 227
	strJson = strJson + "\"setacsuppervoltlimit\": " + HUAWEIDevValue.strhwSetAcsUpperVoltLimit.c_str() + ",\n";	//AC过压点设置
	strJson = strJson + "\"setacslowervoltlimit\": " + HUAWEIDevValue.strhwSetAcsLowerVoltLimit.c_str() + ",\n";	//AC欠压点设置
	strJson = strJson + "\"setdcsuppervoltlimit\": " + HUAWEIDevValue.strhwSetDcsUpperVoltLimit.c_str() + ",\n";	//设置DC过压点
	strJson = strJson + "\"setdcslowervoltlimit\": " + HUAWEIDevValue.strhwSetDcsLowerVoltLimit.c_str() + ",\n";	//设置DC欠压点
	strJson = strJson + "\"setlvdvoltage\": " + HUAWEIDevValue.strhwSetLvdVoltage.c_str() + ",\n";	//设置LVD电压

	//电池参数
	strJson = strJson + "\"acbgroupbatvolt\": " + HUAWEIDevValue.strhwAcbGroupBatVolt.c_str() + ",\n";	//锂电电池电压 214
	strJson = strJson + "\"acbgroupbatcurr\": " + HUAWEIDevValue.strhwAcbGroupBatCurr.c_str() + ",\n";	//锂电电池电流 215
	strJson = strJson + "\"acbgrouptotalcapacity\": " + HUAWEIDevValue.strhwAcbGroupTotalCapacity.c_str() + ",\n";	//锂电电池总容量 216
	strJson = strJson + "\"acbgrouptotalremaincapacity\": " + HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str() + ",\n";	//锂电电池剩余容量 217
	strJson = strJson + "\"acbgroupbackuptime\": " + HUAWEIDevValue.strhwAcbGroupBackupTime.c_str() + ",\n";	//电池备电时长 218
	strJson = strJson + "\"acbgroupbatsoh\": " + HUAWEIDevValue.strhwAcbGroupBatSoh.c_str() + ",\n";	//电池SOH 219
	strJson = strJson + "\"acbgrouptemperature\": " + HUAWEIDevValue.strhwAcbGroupTemperature.c_str() + ",\n";	//电池温度
	strJson = strJson + "\"acbgroupovercurthr\": " + HUAWEIDevValue.strhwAcbGroupOverCurThr.c_str() + ",\n";	//充电过流告警点
	strJson = strJson + "\"acbgrouphightempthr\": " + HUAWEIDevValue.strhwAcbGroupHighTempThr.c_str() + ",\n";	//高温告警点
	strJson = strJson + "\"acbgrouplowtempth\": " + HUAWEIDevValue.strhwAcbGroupLowTempTh.c_str() + ",\n";	//低温告警点
	strJson = strJson + "\"acbgroupdodtoacidbattery\": " + HUAWEIDevValue.strhwAcbGroupDodToAcidBattery.c_str() + ",\n";	//锂电放电DOD

	//直流空调状态信息
	strJson = strJson + "\"dcairctrlmode\": " + HUAWEIDevValue.strhwDcAirCtrlMode[0].c_str() + ",\n"; //空调控制模式 230
	strJson = strJson + "\"dcairctrlmode2\": " + HUAWEIDevValue.strhwDcAirCtrlMode[1].c_str() + ",\n";	//空调控制模式 230
	strJson = strJson + "\"dcairrunstatus\": " + HUAWEIDevValue.strhwDcAirRunStatus[0].c_str() + ",\n";	//空调运行状态 231
	strJson = strJson + "\"dcairrunstatus2\": " + HUAWEIDevValue.strhwDcAirRunStatus[1].c_str() + ",\n";	//空调运行状态 231
	strJson = strJson + "\"dcaircompressorrunstatus\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus[0].c_str() + ",\n";	//空调压缩机运行状态 232
	strJson = strJson + "\"dcaircompressorrunstatus2\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus[1].c_str() + ",\n";	//空调压缩机运行状态 232
	strJson = strJson + "\"dcairinnrfanspeed\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed[0].c_str() + ",\n"; //空调内机转速 233
	strJson = strJson + "\"dcairinnrfanspeed2\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed[1].c_str() + ",\n";	//空调内机转速 233
	strJson = strJson + "\"dcairouterfanspeed\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed[0].c_str() + ",\n";	//空调外风机转速 234
	strJson = strJson + "\"dcairouterfanspeed2\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed[1].c_str() + ",\n";	//空调外风机转速 234
	strJson = strJson + "\"dcaircompressorruntime\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime[0].c_str() + ",\n";	//空调压缩机运行时间 235
	strJson = strJson + "\"dcaircompressorruntime2\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime[1].c_str() + ",\n";	//空调压缩机运行时间 235
	strJson = strJson + "\"dcairenterchanneltemp\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp[0].c_str() + ",\n"; //空调回风口温度 236
	strJson = strJson + "\"dcairenterchanneltemp2\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp[1].c_str() + ",\n";	//空调回风口温度 236
	strJson = strJson + "\"dcairpowerontemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint[0].c_str() + ",\n"; //空调开机温度点 237
	strJson = strJson + "\"dcairpowerontemppoint2\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint[1].c_str() + ",\n";	//空调开机温度点 237
	strJson = strJson + "\"dcairpowerofftemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint[0].c_str() + ",\n";	//空调关机温度点 238
	strJson = strJson + "\"dcairpowerofftemppoint2\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint[1].c_str() + ",\n";	//空调关机温度点 238
	strJson = strJson + "\"hwdcairruntime\": " + HUAWEIDevValue.strhwDcAirRunTime[0].c_str() + ",\n";	//空调运行时间
	strJson = strJson + "\"hwdcairruntime2\": " + HUAWEIDevValue.strhwDcAirRunTime[1].c_str() + ",\n";	//空调运行时间
	strJson = strJson + "\"hwcoolingdevicesmode\": " + HUAWEIDevValue.strhwCoolingDevicesMode.c_str() + ",\n";	//温控模式



	//防火墙状态信息
	if(HUAWEIDevValue.strhwEntityCpuUsage=="2147483647")
		strJson = strJson + "\"entitycpuucheck\": 1,\n";	//状态检测 异常
	else
		strJson = strJson + "\"entitycpuucheck\": 0,\n";	//状态检测 正常
	strJson = strJson + "\"entitycpuusage\": " + HUAWEIDevValue.strhwEntityCpuUsage.c_str() + ",\n";	//CPU
	strJson = strJson + "\"entitymemusage\": " + HUAWEIDevValue.strhwEntityMemUsage.c_str() + ",\n";	//内存使用率
	strJson = strJson + "\"entitytemperature\": " + HUAWEIDevValue.strhwEntityTemperature.c_str() + ",\n";	//温度
	//交换机状态信息
	if(HUAWEIDevValue.strhwswitchEntityCpuUsage=="2147483647")
		strJson = strJson + "\"switchentitycpucheck\": 1,\n";	//状态检测 异常
	else
		strJson = strJson + "\"switchentitycpucheck\": 0,\n";	//状态检测 异常
	strJson = strJson + "\"switchentitycpuusage\": " + HUAWEIDevValue.strhwswitchEntityCpuUsage.c_str() + ",\n";	//CPU
	strJson = strJson + "\"switchentitymemusage\": " + HUAWEIDevValue.strhwswitchEntityMemUsage.c_str() + ",\n";	//内存使用率
	strJson = strJson + "\"switchentitytemperature\": " + HUAWEIDevValue.strhwswitchEntityTemperature.c_str() + "\n";	//温度

	pthread_mutex_unlock(&snmpoidMutex);

	strJson +=	" }\n";
	strJson +=	" }\n";
	strJson +=	"}\n\0";

	mstrjson = strJson;
	//*len=strJson.length();
	//memcpy(json,(char*)strJson.c_str(),*len);

}

void SetjsonDealLockerStr(int messagetype,UINT32 cardid,UINT8 lockaddr,string &mstrjson)
{
	char str[100],sDateTime[30];
	int i;

    time_t nSeconds;
    struct tm * pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    //系统日期和时间,格式: yyyymmddHHMMSS
    sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	std::string strJson;

    strJson +=  "{\n";
	sprintf(str,"\"messagetype\":%d,\n",messagetype); strJson = strJson + str;//消息类型
	strJson = strJson + "\"vmctrl_ipaddr\":\""+ StrIP +"\",\n";	//IP地址
	strJson = strJson + "\"flagroadid\":\""+ StrFlagRoadID +"\",\n";		// ETC 门架路段编号
	strJson = strJson + "\"flagid\": \"" + StrFlagID +"\",\n";			// ETC 门架编号
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\",\n";	//时间
	strJson = strJson + "\"cabinettype\":"+ StrCabinetType +",\n";	//机柜类型
//	sprintf(str,"\"cabineid\":%d,\n",lockaddr); strJson = strJson + str;	//机柜ID
	for (i = 0; i < LOCK_MAX_NUM; i++)
	{
		if(lockaddr==atoi(StrAdrrLock[i].c_str()))
		{sprintf(str,"\"cabineid\":%d,\n",i+1); strJson = strJson + str;}	//机柜ID
	}

	sprintf(str,"\"cardid\":\"%u\",\n",cardid); strJson = strJson + str;	//ID卡号
	sprintf(str,"\"operate\":%d\n",ACT_UNLOCK); strJson = strJson + str;	//操作请求
	strJson +=	"}\n\0";

    mstrjson = strJson;
}

/*int main(int argc, char *argv[])
{
	printf("hello main\n");
	std::string json = "{\"temperature\":25,\"humidity\":\"15\"}";
	std::map<std::string, std::string> out;
	jsonReader(json, out);

	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		printf("the key = %s,value = %s\n",it->first.c_str(), it->second.c_str());
		it++;
	}

	std::map<std::string, std::string> in;
	in["temp"] = "21";
	in["humi"] = "50";
	in["V1"] = "12";
	in["A1"] = "0.2";
	in["V2"] = "48";
	in["A2"] = "0.5";
	in["V3"] = "220";
	in["A3"] = "1";
	std::string json_out;
	jsonWriter(in, json_out);

	printf("the json out = %s\n",json_out.c_str());
}*/
