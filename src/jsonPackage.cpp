#include "jsonPackage.h"
#include "../IpcamServer.h"
#include "../comserver.h"
#include "../rs485server.h"

extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体
extern VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体
extern VA_METER_PARAMS *stuVA_Meter_Param[VA_METER_BD_NUM];		//伏安表电压电流结构体
extern REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
extern RSUCONTROLER stuRsuControl[RSUCTL_NUM];	//RSU控制器状态
extern RSU_DATA_INIT stuRsuData[RSUCTL_NUM];	//RSU设备信息结构体
extern RSU_RESET stuRsuReset;			//天线软件复位状态结构体
extern THUAWEIGantry HUAWEIDevValue;	//华为机柜状态
extern THUAWEIALARM HUAWEIDevAlarm;			//华为机柜告警
extern LOCKER_HW_PARAMS *lockerHw_Param[LOCK_NUM];	//门锁状态结构体
extern VMCONTROL_STATE *stuVMCtl_State;	//控制器运行状态结构体
extern char gsRSUIP[RSUCTL_NUM][20];	//RSUIP地址
extern char gsRSUPort[RSUCTL_NUM][10];	//RSU端口

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
extern string StrHWServer2;		//金晟安服务器地址2
extern string StrHWGetPasswd2;	//金晟安 SNMP GET 密码2
extern string StrHWSetPasswd2;	//金晟安SNMP SET 密码2
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
extern string StrVehPlate900Count;	//900识别仪数量
extern string StrVehPlate900IP[VEHPLATE900_NUM];	//900识别仪IP地址
extern string StrVehPlate900Port[VEHPLATE900_NUM];	//900识别仪端口
extern string StrVehPlate900Key[VEHPLATE900_NUM];	//900识别仪用户名密码
extern string StrCAMCount;	//监控摄像头数量
extern string StrCAMIP[CAM_NUM];	//监控摄像头IP地址
extern string StrCAMPort[CAM_NUM];	//监控摄像头端口
extern string StrCAMKey[CAM_NUM];	//监控摄像头用户名密码

extern string StrFireWareType;//防火墙类型
extern string StrFireWareCount;	//防火墙数量
extern string StrFireWareIP[FIREWARE_NUM];         //防火墙IP
extern string StrFireWareGetPasswd[FIREWARE_NUM];  //防火墙get密码
extern string StrFireWareSetPasswd[FIREWARE_NUM];  //防火墙set密码
extern string StrIPSwitchType;//防火墙类型
extern string StrIPSwitchCount;	//交换机数量
extern string StrIPSwitchIP[IPSWITCH_NUM] ;//交换机IP
extern string StrIPSwitchGetPasswd[IPSWITCH_NUM] ;//交换机get密码
extern string StrIPSwitchSetPasswd[IPSWITCH_NUM] ;//交换机set密码
extern string StrDeviceNameSeq[SWITCH_COUNT];	//设备名的配置

extern int IntFireWareType ; //防火墙类型 1：华为,2：迪普
extern int IntIPSwitchType ; //交换机类型
extern string StrAtlasCount;	//Atlas数量
extern string StrAtlasIP[ATLAS_NUM] ;//AtlasIP
extern string StrAtlasPasswd[ATLAS_NUM] ;//Atlas密码
extern string StrSPDType;	//PSD厂家类型,1:雷迅,2:华咨,3...
extern string StrSPDCount;	//PSD数量
extern string StrSPDIP[SPD_NUM+RES_NUM];	//SPD控制器IP地址
extern string StrSPDPort[SPD_NUM+RES_NUM];	//SPD控制器端口
extern string StrSPDAddr[SPD_NUM+RES_NUM];			//SPD控制器硬件地址
extern char gsSPDIP[SPD_NUM+RES_NUM][20];	//PSD-IP地址
extern char gsSPDPort[SPD_NUM+RES_NUM][10];	//PSD端口
extern UINT8 SPD_Address[SPD_NUM+RES_NUM];

extern string StrAdrrVAMeter[VA_METER_BD_MAX_NUM];	//电压电流传感器1的地址
extern string StrAdrrPower[POWER_BD_MAX_NUM];	//电源板1的地址
extern string StrDoCount;//do数量
extern string StrDeviceNameSeq[SWITCH_COUNT];	//设备名的配置
extern string StrDoSeq[SWITCH_COUNT];	//do和设备映射的配置
extern UINT16 DoSeq[SWITCH_COUNT];	// 另外定义一个专门用来存储映射的数组,stuRemote_Ctrl会被清0
extern string StrUnWireDevName[SWITCH_COUNT];	//没接线设备名的配置
extern string StrUnWireDo[SWITCH_COUNT];	//没接线设备DO配置

//交换机网络数据
extern TFIRESWITCH mTFIRESWITCH[32];
extern string strswitchjson;	//交换机网络数据
extern string strswitchjson1;	//交换机网络数据
//防火墙网络数据
extern TFIRESWITCH mTFIREWALL[32];
extern string strfirewalljson;	//防火墙网络数据
extern string strfirewalljson1;	//防火墙网络数据
extern ATLAS_STATE stuAtlasState[ATLAS_NUM]; //Atlas状态
//空调地址
extern unsigned int hwAirAddrbuf[2];
//温适度地址
extern unsigned int hwTemAddrbuf[2];

extern pthread_mutex_t snmpoidMutex ;

extern int Writeconfig(void);
extern void GetConfig(VMCONTROL_PARAM *vmctrl_param);
extern void RemoteControl(UINT8* pRCtrl);
extern int Setconfig(string StrKEY,string StrSetconfig);
extern void SetIPinfo(IPInfo *ipInfo);
extern void SetIPinfo2(IPInfo *ipInfo);
// 获取电子锁的开关状态信息
UINT16 LockerStatusGet(unsigned char seq);
extern unsigned long GetTickCount(); 

extern TIPcamState mTIPcamState[VEHPLATE_NUM];
extern TIPcamState mTIPcam900State[VEHPLATE900_NUM];

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
/*	std::string json = jsonstrin;
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
	}*/
//	char key[50],value[128],keytmp[50];
	int valueint;
	int opt=SFLAG_READ;
	string mstrdata;

	cJSON *json=0, *jsonkey=0, *jsonvalue=0;
	//解析数据包
	json = cJSON_Parse(jsonstrin);
	if(json==0) return false;

	jsonkey = cJSON_GetObjectItem(json, "messagetype");
	if(jsonkey!=0)				
	{
		messagetype = jsonkey->valueint;
	}
	jsonkey = cJSON_GetObjectItem(json, "opt");
	if(jsonkey!=0)				
	{
		opt = jsonkey->valueint;
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
			break;
		case NETCMD_SEND_UPS_PARAM: 			//10 UPS参数
			break;
/*		case NETCMD_SEND_SPD_PARAM: 			//11 防雷器寄存器参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrSpdWriter(messagetype,(char*)stuSpd_Param,jsonstrout,lenout);
			break;*/
		case NETCMD_SEND_DEV_PARAM: 			//12 控制器参数
			if(opt==SFLAG_READ)
			{
				GetConfig(&vmctrl_param);
				jsonStrVMCtlParamWriterXY(messagetype,(char*)&vmctrl_param,mstrdata);
				*lenout = mstrdata.size();
				memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			}
			else if(opt==SFLAG_WRITE)
			{
				jsonstrVmCtlParamReaderXY(jsonstrin,lenin,(UINT8*)&vmctrl_param);//将json字符串转换成结构体
				SetjsonReceiveOKStr(messagetype,jsonstrout,lenout);
			}
			break;
		case NETCMD_SEND_AIR_PARAM: 			//13 空调参数 
			break;
		case NETCMD_SEND_RSU_PARAM: 			//14 RSU天线参数
			memset(jsonstrout,0,JSON_LEN);
			jsonStrRsuWriterXY(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_SEND_VEHPLATE_PARAM: 			//15 车牌识别参数
			memset(jsonstrout,0,JSON_LEN);
			jsonStrVehPlateWriter(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_SEND_SWITCH_INFO: 			//16 交换机状态
			memset(jsonstrout,0,JSON_LEN);
			SetjsonIPSwitchStatusStr(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_FLAGRUNSTATUS:			//17 门架运行状态
			SetjsonReceiveOKStr(messagetype,jsonstrout,lenout);
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
			jsonStrSwitchStatusWriterXY(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_HWCABINET_STATUS:			//20  华为机柜状态
			memset(jsonstrout,0,JSON_LEN);
			jsonStrHWCabinetWriter(messagetype,(char*)&HUAWEIDevValue,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_SEND_FIREWALL_INFO:			//23 防火墙状态
			memset(jsonstrout,0,JSON_LEN);
			SetjsonFireWallStatusStr(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_SEND_ATLAS_INFO: 		//24 ATLAS状态
			memset(jsonstrout,0,JSON_LEN);
			SetjsonAtlasStatusStr(messagetype,mstrdata);
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
		case NETCMD_SEND_SPD_AI_PARAM:	//27 防雷器参数
			if(opt==SFLAG_READ)
			{
				SetjsonSpdAIStatusStr(messagetype,mstrdata);
				*lenout = mstrdata.size();
				memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			}
			else if(opt==SFLAG_WRITE)
			{
				pRCtrl=stuRemote_Ctrl;
				memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
				jsonstrSPDReader(jsonstrin,lenin,(UINT8 *)pRCtrl);//将json字符串转换成结构体
				RemoteControl((UINT8*)pRCtrl);
				memset(jsonstrout,0,JSON_LEN);
				*lenout=0;
				SetjsonReceiveOKStr(messagetype,jsonstrout,lenout);
			}
			break;
		case NETCMD_SEND_SPD_RES_PARAM:	//28 接地电阻参数
			if(opt==SFLAG_READ)
			{
				SetjsonSpdResStatusStr(messagetype,mstrdata);
				*lenout = mstrdata.size();
				memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			}
			break;
		case NETCMD_SEND_VEHPLATE900_PARAM: 			//29 300万全景车牌识别参数
			memset(jsonstrout,0,JSON_LEN);
			jsonStrVehPlate900Writer(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		case NETCMD_SEND_VMCTRL_STATE: 			//30 控制器运行状态
			memset(jsonstrout,0,JSON_LEN);
			jsonStrVMCtrlStateWriter(messagetype,mstrdata);
			*lenout = mstrdata.size();
			memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
			break;
		default:
			break;
	
	}
	return true;
}


bool jsonIPCamReaderNext(char* jsonstr, int len,int mIndex)
{
    //printf("%s \r\n",jsonstr);


  Json::Reader reader;

  Json::Value json_object;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  //交通部协议的状态内容部分
  string picstateid;   //流水号
  string gantryid;     //门架编号,全网唯一编号
  string statetime;    //状态采集时间
  string overstockImageJourCount ; //积压图片流水数
  string overstockImageCount;      //积压图片数
  string cameranum;    //相机编号（101~299）
  string lanenum;      //车道编号
  string connectstatus; //连接状态
  string workstatus;    //工作状态
  string lightworkstatus; //补光灯的工作状态
  string recognitionrate; //识别成功率
  string hardwareversion; //固件版本
  string softwareversion; //软件版本
  string runningtime;     //设备从开机到现在的运行时间（秒）
  string brand;           //厂商型号
  string devicetype;      //设备型号
  string statuscode;      //状态码,详见附录A3 0-正常；其他由厂商自定义
  string statusmsg;       //状态描述 由厂商自定义,最大长度256 例如：正常

  mTIPcamState[mIndex].TimeStamp=GetTickCount();
  mTIPcamState[mIndex].Linked=true;

  picstateid = json_object["picstateid"].toStyledString() ;
  if(picstateid != "")
  {
    printf("picstateid:%s\r\n",picstateid.c_str()) ;
    mTIPcamState[mIndex].picstateid = picstateid ;
  }

  gantryid = json_object["gantryid"].toStyledString() ;
  if(gantryid != "")
  {
     printf("gantryid:%s\r\n",gantryid.c_str()) ;
     mTIPcamState[mIndex].gantryid = gantryid ;
  }

  statetime = json_object["statetime"].toStyledString() ;
  if(statetime != "")
  {
     printf("statetime:%s\r\n",statetime.c_str()) ;
     mTIPcamState[mIndex].statetime = statetime ;
  }


  overstockImageJourCount = json_object["overstockImageJourCount"].toStyledString() ;
  if(overstockImageJourCount != "")
  {
     printf("overstockImageJourCount:%s\r\n",overstockImageJourCount.c_str()) ;
     mTIPcamState[mIndex].overstockImageJourCount = overstockImageJourCount ;
  }

  overstockImageCount = json_object["overstockImageCount"].toStyledString() ;
  if(overstockImageCount != "")
  {
     printf("overstockImageCount:%s\r\n",overstockImageCount.c_str()) ;
     mTIPcamState[mIndex].overstockImageCount = overstockImageCount ;
  }

  cameranum = json_object["cameranum"].toStyledString() ;
  if(cameranum != "")
  {
     printf("cameranum:%s\r\n",cameranum.c_str()) ;
     mTIPcamState[mIndex].cameranum = cameranum ;
  }


   lanenum = json_object["lanenum"].toStyledString() ;
  if(lanenum != "")
  {
      printf("lanenum:%s\r\n",lanenum.c_str()) ;
      mTIPcamState[mIndex].lanenum = lanenum ;
  }

   connectstatus = json_object["connectstatus"].toStyledString() ;
  if(connectstatus != "")
  {
      printf("connectstatus:%s\r\n",connectstatus.c_str()) ;
      mTIPcamState[mIndex].connectstatus = connectstatus ;
  }


   workstatus = json_object["workstatus"].toStyledString() ;
  if(workstatus != "")
  {
      printf("workstatus:%s\r\n",workstatus.c_str()) ;
      mTIPcamState[mIndex].workstatus = workstatus ;
  }

  lightworkstatus = json_object["lightworkstatus"].toStyledString() ;
 if(lightworkstatus != "")
 {
     printf("lightworkstatus:%s\r\n",lightworkstatus.c_str()) ;
     mTIPcamState[mIndex].lightworkstatus = lightworkstatus ;
 }

  recognitionrate = json_object["recognitionrate"].toStyledString() ;
 if(recognitionrate != "")
 {
     printf("recognitionrate:%s\r\n",recognitionrate.c_str()) ;
     mTIPcamState[mIndex].recognitionrate = recognitionrate ;
 }


  hardwareversion = json_object["hardwareversion"].toStyledString() ;
  if(hardwareversion != "")
  {
     printf("hardwareversion:%s\r\n",hardwareversion.c_str()) ;
     mTIPcamState[mIndex].hardwareversion = hardwareversion ;
  }

  softwareversion = json_object["softwareversion"].toStyledString() ;
  if(softwareversion != "")
  {
    printf("softwareversion:%s\r\n",softwareversion.c_str()) ;
    mTIPcamState[mIndex].softwareversion = softwareversion ;
  }


  runningtime = json_object["runningtime"].toStyledString() ;
  if(runningtime != "")
  {
    printf("runningtime:%s\r\n",runningtime.c_str()) ;
    mTIPcamState[mIndex].runningtime = runningtime ;
  }

  brand = json_object["brand"].toStyledString() ;
  if(brand != "")
  {
    printf("brand:%s\r\n",brand.c_str()) ;
    mTIPcamState[mIndex].brand = brand ;
  }

  devicetype = json_object["devicetype"].toStyledString() ;
  if(devicetype != "")
  {
    printf("devicetype:%s\r\n",devicetype.c_str()) ;
    mTIPcamState[mIndex].devicetype = devicetype ;
  }

  statuscode = json_object["statuscode"].toStyledString() ;
  if(statuscode != "")
  {
    printf("statuscode:%s\r\n",statuscode.c_str()) ;
    mTIPcamState[mIndex].statuscode = statuscode ;
  }

  statusmsg = json_object["statusmsg"].toStyledString() ;
  if(statusmsg != "")
  {
    printf("statusmsg:%s\r\n",statusmsg.c_str()) ;
    mTIPcamState[mIndex].statusmsg = statusmsg ;
  }

  return true;
}


bool jsonIPCam900ReaderNext(char* jsonstr, int len,int mIndex)
{
  Json::Reader reader;

  Json::Value json_object;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  //交通部协议的状态内容部分
  string picstateid;   //流水号
  string gantryid;     //门架编号,全网唯一编号
  string statetime;    //状态采集时间
  string overstockImageJourCount ; //积压图片流水数
  string overstockImageCount;      //积压图片数
  string cameranum;    //相机编号（101~299）
  string lanenum;      //车道编号
  string connectstatus; //连接状态
  string workstatus;    //工作状态
  string lightworkstatus; //补光灯的工作状态
  string recognitionrate; //识别成功率
  string hardwareversion; //固件版本
  string softwareversion; //软件版本
  string runningtime;     //设备从开机到现在的运行时间（秒）
  string brand;           //厂商型号
  string devicetype;      //设备型号
  string statuscode;      //状态码,详见附录A3 0-正常；其他由厂商自定义
  string statusmsg;       //状态描述 由厂商自定义,最大长度256 例如：正常

  mTIPcam900State[mIndex].TimeStamp=GetTickCount();
  mTIPcam900State[mIndex].Linked=true;

  picstateid = json_object["picstateid"].toStyledString() ;
  if(picstateid != "")
  {
    printf("900 picstateid:%s\r\n",picstateid.c_str()) ;
    mTIPcam900State[mIndex].picstateid = picstateid ;
  }

  gantryid = json_object["gantryid"].toStyledString() ;
  if(gantryid != "")
  {
     printf("900 gantryid:%s\r\n",gantryid.c_str()) ;
     mTIPcam900State[mIndex].gantryid = gantryid ;
  }

  statetime = json_object["statetime"].toStyledString() ;
  if(statetime != "")
  {
     printf("900 statetime:%s\r\n",statetime.c_str()) ;
     mTIPcam900State[mIndex].statetime = statetime ;
  }


  overstockImageJourCount = json_object["overstockImageJourCount"].toStyledString() ;
  if(overstockImageJourCount != "")
  {
     printf("900 overstockImageJourCount:%s\r\n",overstockImageJourCount.c_str()) ;
     mTIPcam900State[mIndex].overstockImageJourCount = overstockImageJourCount ;
  }

  overstockImageCount = json_object["overstockImageCount"].toStyledString() ;
  if(overstockImageCount != "")
  {
     printf("900 overstockImageCount:%s\r\n",overstockImageCount.c_str()) ;
     mTIPcam900State[mIndex].overstockImageCount = overstockImageCount ;
  }

  cameranum = json_object["cameranum"].toStyledString() ;
  if(cameranum != "")
  {
     printf("900 cameranum:%s\r\n",cameranum.c_str()) ;
     mTIPcam900State[mIndex].cameranum = cameranum ;
  }


   lanenum = json_object["lanenum"].toStyledString() ;
  if(lanenum != "")
  {
      printf("900 lanenum:%s\r\n",lanenum.c_str()) ;
      mTIPcam900State[mIndex].lanenum = lanenum ;
  }

   connectstatus = json_object["connectstatus"].toStyledString() ;
  if(connectstatus != "")
  {
      printf("900 connectstatus:%s\r\n",connectstatus.c_str()) ;
      mTIPcam900State[mIndex].connectstatus = connectstatus ;
  }


   workstatus = json_object["workstatus"].toStyledString() ;
  if(workstatus != "")
  {
      printf("900 workstatus:%s\r\n",workstatus.c_str()) ;
      mTIPcam900State[mIndex].workstatus = workstatus ;
  }

  lightworkstatus = json_object["lightworkstatus"].toStyledString() ;
 if(lightworkstatus != "")
 {
     printf("900 lightworkstatus:%s\r\n",lightworkstatus.c_str()) ;
     mTIPcam900State[mIndex].lightworkstatus = lightworkstatus ;
 }

  recognitionrate = json_object["recognitionrate"].toStyledString() ;
 if(recognitionrate != "")
 {
     printf("900 recognitionrate:%s\r\n",recognitionrate.c_str()) ;
     mTIPcam900State[mIndex].recognitionrate = recognitionrate ;
 }


  hardwareversion = json_object["hardwareversion"].toStyledString() ;
  if(hardwareversion != "")
  {
     printf("900 hardwareversion:%s\r\n",hardwareversion.c_str()) ;
     mTIPcam900State[mIndex].hardwareversion = hardwareversion ;
  }

  softwareversion = json_object["softwareversion"].toStyledString() ;
  if(softwareversion != "")
  {
    printf("900 softwareversion:%s\r\n",softwareversion.c_str()) ;
    mTIPcam900State[mIndex].softwareversion = softwareversion ;
  }


  runningtime = json_object["runningtime"].toStyledString() ;
  if(runningtime != "")
  {
    printf("900 runningtime:%s\r\n",runningtime.c_str()) ;
    mTIPcam900State[mIndex].runningtime = runningtime ;
  }

  brand = json_object["brand"].toStyledString() ;
  if(brand != "")
  {
    printf("900 brand:%s\r\n",brand.c_str()) ;
    mTIPcam900State[mIndex].brand = brand ;
  }

  devicetype = json_object["devicetype"].toStyledString() ;
  if(devicetype != "")
  {
    printf("900 devicetype:%s\r\n",devicetype.c_str()) ;
    mTIPcam900State[mIndex].devicetype = devicetype ;
  }

  statuscode = json_object["statuscode"].toStyledString() ;
  if(statuscode != "")
  {
    printf("900 statuscode:%s\r\n",statuscode.c_str()) ;
    mTIPcam900State[mIndex].statuscode = statuscode ;
  }

  statusmsg = json_object["statusmsg"].toStyledString() ;
  if(statusmsg != "")
  {
    printf("900 statusmsg:%s\r\n",statusmsg.c_str()) ;
    mTIPcam900State[mIndex].statusmsg = statusmsg ;
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


bool jsonIPCam900Reader(char* jsonstr, int len,int mIndex)
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
     printf("900 statuscode:%s\r\n",strstatuscode.c_str()) ;
  }

  string strmessage = json_object["message"].toStyledString() ;
  if(strmessage != "")
  {
     printf("900 message:%s\r\n",strmessage.c_str()) ;
  }

  string strentity = json_object["entity"].toStyledString() ;
  if(strentity.size() > 3)
  {
     //printf("entity:%s\r\n",strentity.c_str()) ;
     jsonIPCam900ReaderNext((char *)(strentity.c_str()),strentity.size(),mIndex);
  }

  //std::cout << json_object["age"] << std::endl;

return true ;

}




bool jsonstrRCtrlReader(char* jsonstr, int len, UINT8 *pstuRCtrl)
{
	//printf("%s \t\n",jsonstr);
	
	std::string json = jsonstr;
	printf("jsonstrRCtrlReader %s \t\n",json.c_str());
	
	std::map<std::string, std::string> out;
	jsonReader(json, out);
	
	REMOTE_CONTROL *pRCtrl=(REMOTE_CONTROL *)pstuRCtrl;
	THUAWEIGantry *hwDev=&HUAWEIDevValue;	//华为机柜状态
	SPD_PARAMS *spdDev=stuSpd_Param;		//防雷器结构体
	
	char key[50],keytmp[50];
	int i,value;
	int cabineid=0,operate=0;

	memset(pRCtrl,ACT_HOLD,sizeof(REMOTE_CONTROL));
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
		
		if(it->first=="fireware")	pRCtrl->fireware[0]=value;			//防火墙 0xFF00: 遥合;0xFF01: 遥分
		if(it->first=="ipswitch")	pRCtrl->ipswitch[0]=value;			//交换机 0xFF00: 遥合;0xFF01: 遥分

		if(it->first=="sysreset")	pRCtrl->SysReset=value;			//系统重启
		if(it->first=="setsystemtime")	sprintf(pRCtrl->systemtime,"%s",it->second.c_str());			//设置控制器时间

		if(it->first=="hwctrlmonequipreset") pRCtrl->hwctrlmonequipreset=value;	//控制单板复位 0：保持；1：热复位；
		if(it->first=="hwsetacsuppervoltlimit" && hwDev->strhwSetAcsUpperVoltLimit!=it->second) {pRCtrl->hwsetacsuppervoltlimit=value;printf("AC过压点设置 old=%s, new=%d\n",hwDev->strhwSetAcsUpperVoltLimit.c_str(),value);}	//AC过压点设置 0:保持；50-600（有效）；280（缺省值）
		if(it->first=="hwsetacslowervoltlimit" && hwDev->strhwSetAcsLowerVoltLimit!=it->second) {pRCtrl->hwsetacslowervoltlimit=value;printf("AC欠压点设置 old=%s, new=%d\n",hwDev->strhwSetAcsLowerVoltLimit.c_str(),value);}	//AC欠压点设置 0:保持；50-600（有效）；180（缺省值）
		if(it->first=="hwsetdcsuppervoltlimit" && atoi(hwDev->strhwSetDcsUpperVoltLimit.c_str())!=atoi(it->second.c_str())) {pRCtrl->hwsetdcsuppervoltlimit=value;printf("设置DC过压点 old=%s, new=%d\n",hwDev->strhwSetDcsUpperVoltLimit.c_str()	,value);}//设置DC过压点 0:保持；53-600（有效）；58（缺省值）
		if(it->first=="hwsetdcslowervoltlimit" && atoi(hwDev->strhwSetDcsLowerVoltLimit.c_str())!=atoi(it->second.c_str())) {pRCtrl->hwsetdcslowervoltlimit=value;printf("设置DC欠压点 old=%s, new=%d\n",hwDev->strhwSetDcsLowerVoltLimit.c_str(),value);}	//设置DC欠压点 0:保持；35 - 57（有效）；45（缺省值）
		if(it->first=="hwsetenvtempupperlimit" && hwDev->strhwSetEnvTempUpperLimit[0]!=it->second) {pRCtrl->hwsetenvtempupperlimit[0]=value;printf("环境温度告警上限 old=%s, new=%d\n",hwDev->strhwSetEnvTempUpperLimit[0].c_str(),value);}	//环境温度告警上限 0:保持；25-80（有效）；55（缺省值）
		if(it->first=="hwsetenvtempupperlimit2" && hwDev->strhwSetEnvTempUpperLimit[1]!=it->second) {pRCtrl->hwsetenvtempupperlimit[1]=value;printf("环境温度告警上限 old=%s, new=%d\n",hwDev->strhwSetEnvTempUpperLimit[1].c_str(),value);}	//环境温度告警上限 0:保持；25-80（有效）；55（缺省值）
		if(it->first=="hwsetenvtemplowerlimit" && hwDev->strhwSetEnvTempLowerLimit[0]!=it->second) {pRCtrl->hwsetenvtemplowerlimit[0]=value;printf("环境温度告警下限 old=%s, new=%d\n",hwDev->strhwSetEnvTempLowerLimit[0].c_str(),value);}	//环境温度告警下限255:保持；-20-20（有效）；-20（缺省值）
		if(it->first=="hwsetenvtemplowerlimit2" && hwDev->strhwSetEnvTempLowerLimit[1]!=it->second) {pRCtrl->hwsetenvtemplowerlimit[1]=value;printf("环境温度告警下限255 old=%s, new=%d\n",hwDev->strhwSetEnvTempLowerLimit[1].c_str(),value);}//环境温度告警下限255:保持；-20-20（有效）；-20（缺省值）
		if(it->first=="hwsetenvhumidityupperlimit" && hwDev->strhwSetEnvHumidityUpperLimit[0]!=it->second) {pRCtrl->hwsetenvhumidityupperlimit[0]=value;printf("环境湿度告警上限 old=%s, new=%d\n",hwDev->strhwSetEnvHumidityUpperLimit[0].c_str()	,value);}//环境湿度告警上限 255:保持；0-100（有效）；95（缺省值）
		if(it->first=="hwsetenvhumidityupperlimit2" && hwDev->strhwSetEnvHumidityUpperLimit[1]!=it->second) {pRCtrl->hwsetenvhumidityupperlimit[1]=value;printf("环境湿度告警上限 old=%s, new=%d\n",hwDev->strhwSetEnvHumidityUpperLimit[1].c_str(),value);}	//环境湿度告警上限 255:保持；0-100（有效）；95（缺省值）
		if(it->first=="hwsetenvhumiditylowerlimit" && hwDev->strhwSetEnvHumidityLowerLimit[0]!=it->second) {pRCtrl->hwsetenvhumiditylowerlimit[0]=value;printf("环境湿度告警下限 old=%s, new=%d\n",hwDev->strhwSetEnvHumidityLowerLimit[0].c_str()	,value);}//环境湿度告警下限 255:保持；0-100（有效）；5（缺省值）
		if(it->first=="hwsetenvhumiditylowerlimit2" && hwDev->strhwSetEnvHumidityLowerLimit[1]!=it->second) {pRCtrl->hwsetenvhumiditylowerlimit[1]=value;printf("环境湿度告警下限 old=%s, new=%d\n",hwDev->strhwSetEnvHumidityLowerLimit[1].c_str(),value);}	//环境湿度告警下限 255:保持；0-100（有效）；5（缺省值）
		if(it->first=="hwcoolingdevicesmode" && hwDev->strhwCoolingDevicesMode!=it->second) {pRCtrl->hwcoolingdevicesmode=value;printf("温控模式 old=%s, new=%d\n",hwDev->strhwCoolingDevicesMode.c_str(),value);}//温控模式				0：保持；1：纯风扇模式；2：纯空调模式；3：智能模式；
		if(it->first=="hwdcairpowerontemppoint" && hwDev->strhwDcAirPowerOnTempPoint[0]!=it->second) {pRCtrl->hwdcairpowerontemppoint[0]=value;printf("空调开机温度点 old=%s, new=%d\n",hwDev->strhwDcAirPowerOnTempPoint[0].c_str()	,value);}	//空调开机温度点 255:保持； -20-80（有效）；45(缺省值)
		if(it->first=="hwdcairpowerontemppoint2" && hwDev->strhwDcAirPowerOnTempPoint[1]!=it->second) {pRCtrl->hwdcairpowerontemppoint[1]=value;printf("空调开机温度点 old=%s, new=%d\n",hwDev->strhwDcAirPowerOnTempPoint[1].c_str()	,value);}	//空调开机温度点 255:保持； -20-80（有效）；45(缺省值)
		if(it->first=="hwdcairpowerofftemppoint" && hwDev->strhwDcAirPowerOffTempPoint[0]!=it->second) {pRCtrl->hwdcairpowerofftemppoint[0]=value;printf("空调关机温度点 old=%s, new=%d\n",hwDev->strhwDcAirPowerOffTempPoint[0].c_str()	,value);}	//空调关机温度点 		  255:保持； -20-80（有效）；37(缺省值)
		if(it->first=="hwdcairpowerofftemppoint2" && hwDev->strhwDcAirPowerOffTempPoint[1]!=it->second) {pRCtrl->hwdcairpowerofftemppoint[1]=value;printf("空调关机温度点 old=%s, new=%d\n",hwDev->strhwDcAirPowerOffTempPoint[1].c_str()	,value);}	//空调关机温度点 		  255:保持； -20-80（有效）；37(缺省值)
		if(it->first=="hwctrlsmokereset") pRCtrl->hwctrlsmokereset[0]=value;			//控制烟感复位 0：保持；1：不需复位；2：复位
		if(it->first=="hwctrlsmokereset2") pRCtrl->hwctrlsmokereset[1]=value;			//控制烟感复位 0：保持；1：不需复位；2：复位

		if(it->first=="alarm_value" && spdDev->rSPD_res.alarm_value!=value) pRCtrl->spdres_alarm_value=value;	//报警值修改
		if(it->first=="id" && spdDev->rSPD_res.id!=value) {pRCtrl->spdres_id=value; printf("res.id=%d\n",spdDev->rSPD_res.id);}		//修改设备id

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

bool jsonstrVmCtlParamReader(char* jsonstr, int len, UINT8 *pstPam)
{
	//printf("%s \t\n",jsonstr);
	int i,j,k;
	bool locker_changed = false;
	char deal_do[SWITCH_COUNT];//处理do标记;
	FDATA dummy;
	
	std::string json = jsonstr;
	std::map<std::string, std::string> out;
	jsonReader(json, out);
	
	VMCONTROL_PARAM *pRCtrl=(VMCONTROL_PARAM *)pstPam;
	char key[50],value[128],keytmp[50],devicename[50];
//	int value;
	memset(deal_do,0,sizeof(char)*SWITCH_COUNT);

	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());
		sprintf(value,"%s",it->second.c_str());
		printf("%s %s\n",key,value);
		
		//门架信息
		if(it->first=="cabinettype" && StrCabinetType!=value)	//机柜类型	1：华为双机柜双开门；2：华为双机柜单开门；
																//3：华为单机柜双开门；4：华为单机柜单开门
																//5：中兴； 6：金晟安；7：爱特斯
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
			// 更新屏幕上的字母
			ScreenFlagSet(LETTER_SET);
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
			ClearvecWalkSnmp();
		}
		if(it->first=="hwgetpasswd" && StrHWGetPasswd!=value)	//SNMP GET 密码
		{
			StrHWGetPasswd=value;
			sprintf(pRCtrl->HWGetPasswd,"%s",value);	
			Setconfig("HWGetPasswd=",value);		
			ClearvecWalkSnmp();
		}
		if(it->first=="hwsetpasswd" && StrHWSetPasswd!=value)	//SNMP SET 密码
		{
			StrHWSetPasswd=value;
			sprintf(pRCtrl->HWSetPasswd,"%s",value);	
			Setconfig("HWSetPasswd=",value);		
			ClearvecWalkSnmp();
		}
		if(it->first=="hwserver2" && StrHWServer2!=value)	//金晟安服务器地址
		{
			StrHWServer2=value;
			sprintf(pRCtrl->HWServer2,"%s",value);
			Setconfig("HWServer2=",value);	
			ClearvecWalkSnmp();
		}
		if(it->first=="hwgetpasswd2" && StrHWGetPasswd2!=value)	//金晟安 SNMP GET 密码
		{
			StrHWGetPasswd2=value;
			sprintf(pRCtrl->HWGetPasswd2,"%s",value);	
			Setconfig("HWGetPasswd2=",value);		
			ClearvecWalkSnmp();
		}
		if(it->first=="hwsetpasswd2" && StrHWSetPasswd2!=value)	//金晟安 SNMP SET 密码
		{
			StrHWSetPasswd2=value;
			sprintf(pRCtrl->HWSetPasswd2,"%s",value);	
			Setconfig("HWSetPasswd2=",value);		
			ClearvecWalkSnmp();
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
				sprintf(gsRSUIP[i],"%s",value); 
				sprintf(key,"RSU%dIP=",i+1);//RSUIP地址
				Setconfig(key,value);
			}
			sprintf(keytmp,"rsu%dport",i+1);//RSU端口
			if(it->first==keytmp && StrRSUPort[i]!=value)	
			{
				StrRSUPort[i]=value;
				sprintf(pRCtrl->RSUPort[i],"%s",value);  
				sprintf(gsRSUPort[i],"%s",value); 
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
		if(it->first=="vehplate900count" && StrVehPlate900Count!=value && atoi(value)>=0 && atoi(value)<=VEHPLATE900_NUM)	//900识别仪数量
		{
			StrVehPlate900Count=value;	
			sprintf(pRCtrl->VehPlate900Count,"%s",value);	
			Setconfig("VehPlate900Count=",value);
		}
		for(i=0;i<VEHPLATE900_NUM;i++)
		{
			sprintf(keytmp,"vehplate900%dip",i+1);//900识别仪IP地址
			if(it->first==keytmp && StrVehPlate900IP[i]!=value)
			{
				StrVehPlate900IP[i]=value;	
				sprintf(pRCtrl->VehPlate900IP[i],"%s",value);	
				sprintf(key,"VehPlate900%dIP=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"vehplate900%dport",i+1);//900识别仪端口
			if(it->first==keytmp && StrVehPlate900Port[i]!=value)
			{
				StrVehPlate900Port[i]=value;
				sprintf(pRCtrl->VehPlate900Port[i],"%s",value);	
				sprintf(key,"VehPlate900%dPort=",i+1);
				Setconfig(key,value);
			}
			sprintf(keytmp,"vehplate900%dkey",i+1);//900识别仪用户名密码
			if(it->first==keytmp && StrVehPlate900Key[i]!=value)
			{
				StrVehPlate900Key[i]=value;
				sprintf(pRCtrl->VehPlate900Key[i],"%s",value);
				sprintf(key,"VehPlate900%dKey=",i+1);
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
		if(it->first=="ipswitchtype" && StrIPSwitchType!=value)	//交换机类型
		{
			StrIPSwitchType=value;	
			IntIPSwitchType=atoi(value);
			sprintf(pRCtrl->SwitchType,"%s",value);	
			Setconfig("SwitchType=",value);
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
				ClearvecWalkSnmp();
			}
			sprintf(keytmp,"ipswitch%dgetpasswd",i+1);//交换机get密码
			if(it->first==keytmp && StrIPSwitchGetPasswd[i]!=value)
			{
				StrIPSwitchGetPasswd[i]=value;
				sprintf(pRCtrl->SwitchGetPasswd[i],"%s",value);	
				sprintf(key,"Switch%dGetPasswd=",i+1);
				Setconfig(key,value);
				ClearvecWalkSnmp();
			}
			sprintf(keytmp,"ipswitch%dsetpasswd",i+1);//交换机set密码
			if(it->first==keytmp && StrIPSwitchSetPasswd[i]!=value)
			{
				StrIPSwitchSetPasswd[i]=value;
				sprintf(pRCtrl->SwitchSetPasswd[i],"%s",value);
				sprintf(key,"Switch%dSetPasswd=",i+1);
				Setconfig(key,value);
				ClearvecWalkSnmp();
			}
		}
		if(it->first=="firewarecount" && StrFireWareCount!=value && atoi(value)>=0 && atoi(value)<=FIREWARE_NUM)	//防火墙数量
		{
			StrFireWareCount=value;	
			sprintf(pRCtrl->FireWareCount,"%s",value);	
			Setconfig("FireWareCount=",value);
		}
		if(it->first=="firewaretype" && StrFireWareType!=value)	//防火墙类型
		{
			StrFireWareType=value;	
			IntFireWareType=atoi(value);
			sprintf(pRCtrl->FireWareType,"%s",value);	
			Setconfig("FireWareType=",value);
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
				ClearvecWalkSnmp();
			}
			sprintf(keytmp,"fireware%dgetpasswd",i+1);//防火墙get密码
			if(it->first==keytmp && StrFireWareGetPasswd[i]!=value)
			{
				StrFireWareGetPasswd[i]=value;
				sprintf(pRCtrl->FireWareGetPasswd[i],"%s",value);	
				sprintf(key,"FireWare%dGetPasswd=",i+1);
				Setconfig(key,value);
				ClearvecWalkSnmp();
			}
			sprintf(keytmp,"fireware%dsetpasswd",i+1);//防火墙set密码
			if(it->first==keytmp && StrFireWareSetPasswd[i]!=value)
			{
				StrFireWareSetPasswd[i]=value;
				sprintf(pRCtrl->FireWareSetPasswd[i],"%s",value);
				sprintf(key,"FireWare%dSetPasswd=",i+1);
				Setconfig(key,value);
				ClearvecWalkSnmp();
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
		//防雷器SPD
		if(it->first=="spdcount" && StrSPDCount!=value && atoi(value)>=0 && atoi(value)<=SPD_NUM)	//防雷器数量
		{
			StrSPDCount=value;
			SPD_num =atoi(StrSPDCount.c_str());
			printf("spdcount = %d\r\n",SPD_num);
			sprintf(pRCtrl->SPDCount,"%s",value);
			Setconfig("SPDCount=",value);
			// 数量改变，全部初始化
			for(i=0;i<SPD_NUM+RES_NUM;i++)
			{
				HZ_reset_pre[i] = true;
			}
		}
		if(it->first=="spdtype" && StrSPDType!=value)	//防雷器类型
		{
			StrSPDType=value;
			SPD_Type = atoi(StrSPDType.c_str());
			printf("spdtype = %d\r\n",SPD_Type);
			sprintf(pRCtrl->SPDType,"%s",value);
			Setconfig("SPDType=",value);
			// 类型改变，全部初始化
			for(i=0;i<SPD_NUM+RES_NUM;i++)
			{
				HZ_reset_pre[i] = true;
			}
		}
		for(i=0;i<SPD_NUM;i++)
		{
			sprintf(keytmp,"spd%dip",i+1);//SPDIP地址
			if(it->first==keytmp && StrSPDIP[i]!=value)
			{
				StrSPDIP[i]=value;
				sprintf(gsSPDIP[i],StrSPDIP[i].c_str());
				printf("spd%dip = %s\r\n",i+1,gsSPDIP[i]);
				sprintf(pRCtrl->SPDIP[i],"%s",value);
				sprintf(key,"SPD%dIP=",i+1);
				Setconfig(key,value);
				HZ_reset_pre[i] = true;
			}
			sprintf(keytmp,"spd%dport",i+1);//SPD端口
			if(it->first==keytmp && StrSPDPort[i]!=value)
			{
				StrSPDPort[i]=value;
				sprintf(gsSPDPort[i],StrSPDPort[i].c_str());
				printf("spd%dport = %s\r\n",i+1,gsSPDPort[i]);
				sprintf(pRCtrl->SPDPort[i],"%s",value);
				sprintf(key,"SPD%dPort=",i+1);
				Setconfig(key,value);
				HZ_reset_pre[i] = true;
			}
			sprintf(keytmp,"spd%daddr",i+1);//SPD硬件地址
			if(it->first==keytmp && StrSPDAddr[i]!=value)
			{
				StrSPDAddr[i]=value;
				SPD_Address[i] = atoi(StrSPDAddr[i].c_str());
				printf("spd%daddr = %d\r\n",i+1,SPD_Address[i]);
				sprintf(pRCtrl->SPDAddr[i],"%s",value);
				sprintf(key,"SPD%dAddr=",i+1);
				Setconfig(key,value);
				HZ_reset_pre[i] = true;
			}
		}

		if(it->first=="spdresip" && StrSPDIP[SPD_NUM]!=value)	//防雷器接地电阻IP
		{
			StrSPDIP[SPD_NUM]=value;
			sprintf(gsSPDIP[SPD_NUM],StrSPDIP[SPD_NUM].c_str());
			printf("spdresip = %s\r\n",gsSPDIP[SPD_NUM]);
			sprintf(pRCtrl->SPDIP[SPD_NUM],"%s",value);
			sprintf(key,"SPDResIP=");
			Setconfig(key,value);
			HZ_reset_pre[SPD_NUM] = true;
		}
		if(it->first=="spdresport" && StrSPDPort[SPD_NUM]!=value)	//防雷器接地电阻端口
		{
			StrSPDPort[SPD_NUM]=value;
			sprintf(gsSPDPort[SPD_NUM],StrSPDPort[SPD_NUM].c_str());
			printf("spdresport = %s\r\n",gsSPDPort[SPD_NUM]);
			sprintf(pRCtrl->SPDPort[SPD_NUM],"%s",value);
			sprintf(key,"SPDResPort=");
			Setconfig(key,value);
			HZ_reset_pre[SPD_NUM] = true;
		} 
		if(it->first=="spdresid" && StrSPDAddr[SPD_NUM]!=value)	//防雷器接地电阻地址
		{ 
			StrSPDAddr[SPD_NUM]=value;
			SPD_Address[SPD_NUM] = atoi(StrSPDAddr[SPD_NUM].c_str());
			printf("spdresaddr = %d\r\n",SPD_Address[SPD_NUM]);
			sprintf(pRCtrl->SPDAddr[SPD_NUM],"%s",value);
			sprintf(key,"SPDResAddr=");
			Setconfig(key,value);
			HZ_reset_pre[SPD_NUM] = true;
			// 不要去修改具体的硬件设备本身的地址，我们只是对接协议，不改地址
			//Ex_SPD_Set_Process(SPD_RES_SET,RES_ID_ADDR,dummy,SPD_Address[SPD_NUM]);
		}
		if(it->first=="spdres_alarm_value" && stuSpd_Param->rSPD_res.alarm_value!=atoi(value))	//接地电阻报警值
		{
			UINT16 addr_ref = 1;
			stuSpd_Param->rSPD_res.alarm_value=atoi(value);
			printf("spdres报警值修改=%s\n",value);
			if (SPD_Type == TYPE_LEIXUN)
			{
				addr_ref = RES_ALARM_ADDR;
			}
			else if (SPD_Type == TYPE_KY)
			{
				addr_ref = KY_RES_ALARM_ADDR;
			}
			//spdres更改报警值
			Ex_SPD_Set_Process(0,SPD_RES_SET,addr_ref,dummy,atoi(value));
		}

		for(i=0;i<LOCK_MAX_NUM;i++)
		{
			sprintf(keytmp,"adrrlock%d",i+1);//门锁地址
			if(it->first==keytmp && StrAdrrLock[i]!=value)
			{
				StrAdrrLock[i]=value;	
				sprintf(pRCtrl->LockAddr[i],"%s",value);	
				sprintf(key,"LOCKADD%d=",i+1);
				Setconfig(key,value);
				locker_changed = true;	// 锁的配置发生变化
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
		for(i=0;i<RSUCTL_NUM;i++)
		{
			sprintf(devicename,"rsu%d_do",i+1);
			if(it->first==devicename)	
			{
				j=atoi(value)-1;
				deal_do[j]=1;//处理do标记
				if(j>=0 && j<SWITCH_COUNT)
				{
					if(StrDeviceNameSeq[j]!=devicename)
					{
						string strold=StrDeviceNameSeq[j];
						StrDeviceNameSeq[j]=devicename;
						string strnew=StrDeviceNameSeq[j];
						transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
						strnew=strnew+"=";
						Setconfig(strnew.c_str(),value);//DO映射
						
						if(strold=="")		//原来没有设备，添加do映射
						{
							StrDoSeq[j]=value;
							DoSeq[j]=atoi(value);
						}
						transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
						strold=strold+"=";
						Setconfig(strold.c_str(),"");//清除原来DO映射
					}
				}
				else if(j==-1) //do改为无设备
				{
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrDeviceNameSeq[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							sprintf(keytmp,"do%d_do",k+1);
							StrDeviceNameSeq[k]=keytmp; //设备名
							sprintf(keytmp,"%d",k+1);
							StrDoSeq[k] = keytmp;	//对应DO
						}
					}
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrUnWireDevName[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							StrUnWireDevName[k]=""; //设备名
							StrUnWireDo[k] = "";	//对应DO
						}
					}
				}
				else if(j>=100 && j<100+SWITCH_COUNT)
				{
					string strnew=devicename;
					transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
					strnew=strnew+"=";
					Setconfig(strnew.c_str(),value);//DO映射
					
					StrUnWireDevName[j-100]=devicename;
					StrUnWireDo[j-100] = value;	//对应DO
				}
			}
		}
		for(i=0;i<VEHPLATE_NUM;i++)
		{
			sprintf(devicename,"vehplate%d_do",i+1);
			if(it->first==devicename)	
			{
				j=atoi(value)-1;
				deal_do[j]=1;//处理do标记
//printf("value=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",atoi(value),StrDeviceNameSeq[i].c_str(),StrDoSeq[i].c_str(),devicename,value);
				if(j>=0 && j<SWITCH_COUNT)
				{
					if(StrDeviceNameSeq[j]!=devicename)		//do改设备
					{
						string strold=StrDeviceNameSeq[j];
						StrDeviceNameSeq[j]=devicename;
						string strnew=StrDeviceNameSeq[j];
						transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
						strnew=strnew+"=";
						Setconfig(strnew.c_str(),value);//DO映射
						
						if(strold=="")		//原来没有设备，添加do映射
						{
							StrDoSeq[j]=value;
							DoSeq[j]=atoi(value);
						}
						transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
						strold=strold+"=";
						Setconfig(strold.c_str(),"");//清除原来DO映射
					}
				}
				else if(j==-1) //do改为无设备
				{
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrDeviceNameSeq[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							sprintf(keytmp,"do%d_do",k+1);
							StrDeviceNameSeq[k]=keytmp; //设备名
							sprintf(keytmp,"%d",k+1);
							StrDoSeq[k] = keytmp;	//对应DO
						}
					}
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrUnWireDevName[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							StrUnWireDevName[k]=""; //设备名
							StrUnWireDo[k] = "";	//对应DO
						}
					}
				}
				else if(j>=100 && j<100+SWITCH_COUNT)
				{
					string strnew=devicename;
					transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
					strnew=strnew+"=";
					Setconfig(strnew.c_str(),value);//DO映射
					
					StrUnWireDevName[j-100]=devicename;
					StrUnWireDo[j-100] = value;	//对应DO
				}
			}
		}
		for(i=0;i<VEHPLATE900_NUM;i++)
		{
			sprintf(devicename,"vehplate900%d_do",i+1);
			if(it->first==devicename)	
			{
				j=atoi(value)-1;
				deal_do[j]=1;//处理do标记
				if(j>=0 && j<SWITCH_COUNT)
				{
					printf("olddev:%s,newdev:%s\n",StrDeviceNameSeq[j].c_str(),devicename);
					if(StrDeviceNameSeq[j]!=devicename)
					{
						string strold=StrDeviceNameSeq[j];
						StrDeviceNameSeq[j]=devicename;
						string strnew=StrDeviceNameSeq[j];
						transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
						strnew=strnew+"=";
						Setconfig(strnew.c_str(),value);//DO映射
						
						if(strold=="")		//原来没有设备，添加do映射
						{
							StrDoSeq[j]=value;
							DoSeq[j]=atoi(value);
						}
						transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
						strold=strold+"=";
						Setconfig(strold.c_str(),"");//清除原来DO映射
					}
				}
				else if(j==-1) //do改为无设备
				{
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrDeviceNameSeq[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							sprintf(keytmp,"do%d_do",k+1);
							StrDeviceNameSeq[k]=keytmp; //设备名
							sprintf(keytmp,"%d",k+1);
							StrDoSeq[k] = keytmp;	//对应DO
						}
					}
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrUnWireDevName[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							StrUnWireDevName[k]=""; //设备名
							StrUnWireDo[k] = "";	//对应DO
						}
					}
				}
				else if(j>=100 && j<100+SWITCH_COUNT)
				{
					string strnew=devicename;
					transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
					strnew=strnew+"=";
					Setconfig(strnew.c_str(),value);//DO映射
					
					StrUnWireDevName[j-100]=devicename;
					StrUnWireDo[j-100] = value;	//对应DO
				}
			}
		}
		for(i=0;i<CAM_NUM;i++)
		{
			sprintf(devicename,"cam%d_do",i+1);
			if(it->first==devicename)	
			{
				j=atoi(value)-1;
				deal_do[j]=1;//处理do标记
				if(j>=0 && j<SWITCH_COUNT)
				{
					if(StrDeviceNameSeq[j]!=devicename)
					{
						string strold=StrDeviceNameSeq[j];
						StrDeviceNameSeq[j]=devicename;
						string strnew=StrDeviceNameSeq[j];
						transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
						strnew=strnew+"=";
						Setconfig(strnew.c_str(),value);//DO映射
						
						if(strold=="")		//原来没有设备，添加do映射
						{
							StrDoSeq[j]=value;
							DoSeq[j]=atoi(value);
						}
						transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
						strold=strold+"=";
						Setconfig(strold.c_str(),"");//清除原来DO映射
					}
				}
				else if(j==-1) //do改为无设备
				{
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrDeviceNameSeq[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							sprintf(keytmp,"do%d_do",k+1);
							StrDeviceNameSeq[k]=keytmp; //设备名
							sprintf(keytmp,"%d",k+1);
							StrDoSeq[k] = keytmp;	//对应DO
						}
					}
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrUnWireDevName[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							StrUnWireDevName[k]=""; //设备名
							StrUnWireDo[k] = "";	//对应DO
						}
					}
				}
				else if(j>=100 && j<100+SWITCH_COUNT)
				{
					string strnew=devicename;
					transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
					strnew=strnew+"=";
					Setconfig(strnew.c_str(),value);//DO映射
					
					StrUnWireDevName[j-100]=devicename;
					StrUnWireDo[j-100] = value;	//对应DO
				}
			}
		}
		for(i=0;i<FIREWARE_NUM;i++)
		{
			sprintf(devicename,"fireware%d_do",i+1);
			if(it->first==devicename)	
			{
				j=atoi(value)-1;
				deal_do[j]=1;//处理do标记
				if(j>=0 && j<SWITCH_COUNT)
				{
					if(StrDeviceNameSeq[j]!=devicename)
					{
						string strold=StrDeviceNameSeq[j];
						StrDeviceNameSeq[j]=devicename;
						string strnew=StrDeviceNameSeq[j];
						transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
						strnew=strnew+"=";
						Setconfig(strnew.c_str(),value);//DO映射
						
						if(strold=="")		//原来没有设备，添加do映射
						{
							StrDoSeq[j]=value;
							DoSeq[j]=atoi(value);
						}
						transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
						strold=strold+"=";
						Setconfig(strold.c_str(),"");//清除原来DO映射
					}
				}
				else if(j==-1) //do改为无设备
				{
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrDeviceNameSeq[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							sprintf(keytmp,"do%d_do",k+1);
							StrDeviceNameSeq[k]=keytmp; //设备名
							sprintf(keytmp,"%d",k+1);
							StrDoSeq[k] = keytmp;	//对应DO
						}
					}
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrUnWireDevName[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							StrUnWireDevName[k]=""; //设备名
							StrUnWireDo[k] = "";	//对应DO
						}
					}
				}
				else if(j>=100 && j<100+SWITCH_COUNT)
				{
					string strnew=devicename;
					transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
					strnew=strnew+"=";
					Setconfig(strnew.c_str(),value);//DO映射
					
					StrUnWireDevName[j-100]=devicename;
					StrUnWireDo[j-100] = value;	//对应DO
				}
			}
		}
		for(i=0;i<IPSWITCH_NUM;i++)
		{
			sprintf(devicename,"ipswitch%d_do",i+1);
			if(it->first==devicename)	
			{
				j=atoi(value)-1;
				deal_do[j]=1;//处理do标记
				if(j>=0 && j<SWITCH_COUNT)
				{
					if(StrDeviceNameSeq[j]!=devicename)
					{
						string strold=StrDeviceNameSeq[j];
						StrDeviceNameSeq[j]=devicename;
						string strnew=StrDeviceNameSeq[j];
						transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
						strnew=strnew+"=";
						Setconfig(strnew.c_str(),value);//DO映射
						
						if(strold=="")		//原来没有设备，添加do映射
						{
							StrDoSeq[j]=value;
							DoSeq[j]=atoi(value);
						}
						transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
						strold=strold+"=";
						Setconfig(strold.c_str(),"");//清除原来DO映射
					}
				}
				else if(j==-1) //do改为无设备
				{
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrDeviceNameSeq[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							sprintf(keytmp,"do%d_do",k+1);
							StrDeviceNameSeq[k]=keytmp; //设备名
							sprintf(keytmp,"%d",k+1);
							StrDoSeq[k] = keytmp;	//对应DO
						}
					}
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrUnWireDevName[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							StrUnWireDevName[k]=""; //设备名
							StrUnWireDo[k] = "";	//对应DO
						}
					}
				}
				else if(j>=100 && j<100+SWITCH_COUNT)
				{
					string strnew=devicename;
					transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
					strnew=strnew+"=";
					Setconfig(strnew.c_str(),value);//DO映射
					
					StrUnWireDevName[j-100]=devicename;
					StrUnWireDo[j-100] = value;	//对应DO
				}
			}
		}
		for(i=0;i<ATLAS_NUM;i++)
		{
			sprintf(devicename,"atlas%d_do",i+1);
			if(it->first==devicename)	
			{
				j=atoi(value)-1;
				deal_do[j]=1;//处理do标记
				if(j>=0 && j<SWITCH_COUNT)
				{
					if(StrDeviceNameSeq[j]!=devicename)
					{
						string strold=StrDeviceNameSeq[j];
						StrDeviceNameSeq[j]=devicename;
						string strnew=StrDeviceNameSeq[j];
						transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
						strnew=strnew+"=";
						Setconfig(strnew.c_str(),value);//DO映射
						
						if(strold=="")		//原来没有设备，添加do映射
						{
							StrDoSeq[j]=value;
							DoSeq[j]=atoi(value);
						}
						transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
						strold=strold+"=";
						Setconfig(strold.c_str(),"");//清除原来DO映射
					}
				}
				else if(j==-1) //do改为无设备
				{
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrDeviceNameSeq[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							sprintf(keytmp,"do%d_do",k+1);
							StrDeviceNameSeq[k]=keytmp; //设备名
							sprintf(keytmp,"%d",k+1);
							StrDoSeq[k] = keytmp;	//对应DO
						}
					}
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrUnWireDevName[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							StrUnWireDevName[k]=""; //设备名
							StrUnWireDo[k] = "";	//对应DO
						}
					}
				}
				else if(j>=100 && j<100+SWITCH_COUNT)
				{
					string strnew=devicename;
					transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
					strnew=strnew+"=";
					Setconfig(strnew.c_str(),value);//DO映射
					
					StrUnWireDevName[j-100]=devicename;
					StrUnWireDo[j-100] = value;	//对应DO
				}
			}
		}
		for(i=0;i<ANTENNA_NUM;i++)
		{
			sprintf(devicename,"antenna%d_do",i+1);
			if(it->first==devicename)	
			{
				j=atoi(value)-1;
				deal_do[j]=1;//处理do标记
				if(j>=0 && j<SWITCH_COUNT)
				{
					if(StrDeviceNameSeq[j]!=devicename)
					{
						string strold=StrDeviceNameSeq[j];
						StrDeviceNameSeq[j]=devicename;
						string strnew=StrDeviceNameSeq[j];
						transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
						strnew=strnew+"=";
						Setconfig(strnew.c_str(),value);//DO映射
						
						if(strold=="")		//原来没有设备，添加do映射
						{
							StrDoSeq[j]=value;
							DoSeq[j]=atoi(value);
						}
						transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
						strold=strold+"=";
						Setconfig(strold.c_str(),"");//清除原来DO映射
					}
				}
				else if(j==-1) //do改为无设备
				{
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrDeviceNameSeq[k])
						{
	//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							sprintf(keytmp,"do%d_do",k+1);
							StrDeviceNameSeq[k]=keytmp; //设备名
							sprintf(keytmp,"%d",k+1);
							StrDoSeq[k] = keytmp;	//对应DO
						}
					}
					for(k=0;k<SWITCH_COUNT;k++)
					{
						string strnew=devicename;
						if(strnew==StrUnWireDevName[k])
						{
//printf("k=%d,olddev=%s,do=%s,newdev=%s,do=%s\n",k,StrDeviceNameSeq[k].c_str(),StrDoSeq[k].c_str(),strnew.c_str(),value);
							transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
							strnew=strnew+"=";
							Setconfig(strnew.c_str(),value);//DO映射

							StrUnWireDevName[k]=""; //设备名
							StrUnWireDo[k] = "";	//对应DO
						}
					}
				}
				else if(j>=100 && j<100+SWITCH_COUNT)
				{
					string strnew=devicename;
					transform(strnew.begin(), strnew.end(), strnew.begin(), ::toupper);
					strnew=strnew+"=";
					Setconfig(strnew.c_str(),value);//DO映射
					
					StrUnWireDevName[j-100]=devicename;
					StrUnWireDo[j-100] = value;	//对应DO
				}
			}
		}
		it++;
	}

	// 统一处理防雷接地初始化标志
	for(i=0;i<SPD_NUM+RES_NUM;i++)
	{
		if (SPD_Type == TYPE_HUAZI)
		{
			if (HZ_reset_pre[i] == true)
			{
				HZ_reset_pre[i] = false;
				HZ_reset_flag[i] = true;
			}
		}
	}

	//清除无设备do标记
/*	int count=0;
	for(i=0;i<12;i++)
	{
		count+=deal_do[i];//处理do标记
	}
	if(count>0)	//设置do
	{
		for(i=0;i<12;i++)
		{
			if(deal_do[i]==0)//没有设置的do，清空原设备do
			{
				string strold=StrDeviceNameSeq[i];
//printf("aaa change do%d %s\n",i+1,strold.c_str());
				transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
				strold=strold+"=";
				Setconfig(strold.c_str(),"");//清除原来DO映射
				StrDeviceNameSeq[i]="";
				StrDoSeq[i]="";
				DoSeq[i]=i;
			}
		}
	}
	count=0;
	for(i=12;i<24;i++)
	{
		count+=deal_do[i];//处理do标记
	}
	if(count>0) //设置do
	{
		for(i=12;i<24;i++)
		{
			if(deal_do[i]==0)//没有设置的do，清空原设备do
			{
				string strold=StrDeviceNameSeq[i];
//printf("aaa change do%d %s\n",i+1,strold.c_str());
				transform(strold.begin(), strold.end(), strold.begin(), ::toupper);
				strold=strold+"=";
				Setconfig(strold.c_str(),"");//清除原来DO映射
				StrDeviceNameSeq[i]="";
				StrDoSeq[i]="";
				DoSeq[i]=i;
			}
		}
	}*/
	// 没有配置的都置空
	if (SPD_num < SPD_NUM)
	{
		for (i=SPD_num;i<SPD_NUM;i++)
		{
			StrSPDIP[i] =""; ;//防雷器IP
			sprintf(key,"SPD%dIP=",i+1);
			Setconfig(key,StrSPDIP[i]);
			StrSPDPort[i] ="";//防雷器端口
			sprintf(key,"SPD%dPort=",i+1);
			Setconfig(key,StrSPDPort[i]);
			StrSPDAddr[i] ="";//防雷器硬件端口
			sprintf(key,"SPD%dAddr=",i+1);
			Setconfig(key,StrSPDAddr[i]);
		}
		Setconfig("SPDResIP=","");
		Setconfig("SPDResPort=","");
	}
	if (locker_changed)
	{
		locker_changed = false;
		// 锁的参数重新初始化
		lockerDataInit(false);
	}
	printf("\n");
	
	Writeconfig();
	
	return true;
}


bool jsonstrVmCtlParamReaderXY(char* jsonstr, int len, UINT8 *pstPam)
{
//	printf("%s \t\n",jsonstr);
	//json是json对象指针,json_name是 name对象的指针,json_age是age对象的指针
	VMCONTROL_PARAM *pRCtrl=(VMCONTROL_PARAM *)pstPam;
	char key[50],value[128],keytmp[50];
	int valueint,arraysize;
	FDATA dummy;

	cJSON *json=0, *jsonkey=0, *jsonvalue=0, *jsonlist=0, *jsonitem=0;
	int i,vpcount;
	//解析数据包
	json = cJSON_Parse(jsonstr);
	if(json==0) return false;

	//门架信息
	sprintf(key,"cabinettype");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				//机柜类型	1：华为双机柜双开门；2：华为双机柜单开门；3：华为单机柜双开门；4：华为单机柜单开门
	{
		valueint = jsonkey->valueint;
		printf("%s %d\n",key,valueint);
		if(valueint!=atoi(StrCabinetType.c_str()))
		{
			sprintf(value,"%d",valueint);
			StrCabinetType=value;
			sprintf(pRCtrl->CabinetType,"%s",value);
			Setconfig("CabinetType=",value);
		}
	}
	sprintf(key,"flagnetroadid");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				//ETC 门架路网编号
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrFlagNetRoadID)
		{
			StrFlagNetRoadID=value;
			sprintf(pRCtrl->FlagNetRoadID,"%s",value);
			Setconfig("FlagNetRoadID=",value);
		}
	}
	//ETC 门架路段编号
	sprintf(key,"flagroadid");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrFlagRoadID)
		{
			StrFlagRoadID=value;
			sprintf(pRCtrl->FlagRoadID,"%s",value);
			Setconfig("FlagRoadID=",value);
		}
	}
	
	//ETC 门架编号
	sprintf(key,"flagid");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrFlagID)
		{
			StrFlagID=value;
			// 更新屏幕上的字母
			ScreenFlagSet(LETTER_SET);
			sprintf(pRCtrl->FlagID,"%s",value);
			Setconfig("FlagID=",value);
		}
	}

	//ETC 门架序号
	sprintf(key,"posid");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrPosId)
		{
			StrPosId=value;
			sprintf(pRCtrl->PosId,"%s",value);
			Setconfig("PosId=",value);
		}
	}
	
	//行车方向
	sprintf(key,"direction");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrDirection)
		{
			StrDirection=value;
			sprintf(pRCtrl->Direction,"%s",value);
			Setconfig("Direction=",value);
		}
	}
	
	//行车方向说明
	sprintf(key,"dirdescription");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrDirDescription)
		{
			StrDirDescription=value;
			sprintf(pRCtrl->DirDescription,"%s",value);
			Setconfig("DirDescription=",value);
		}
	}
	
	//参数设置
	//华为服务器地址
	sprintf(key,"hwserver");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrHWServer)
		{
			StrHWServer=value;
			sprintf(pRCtrl->HWServer,"%s",value);
			Setconfig("HWServer=",value);
		}
	}
	
	//SNMP GET 密码
	sprintf(key,"hwgetpasswd");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrHWGetPasswd)
		{
			StrHWGetPasswd=value;
			sprintf(pRCtrl->HWGetPasswd,"%s",value);	
			Setconfig("HWGetPasswd=",value);		
		}
	}
	
	//SNMP SET 密码
	sprintf(key,"hwsetpasswd");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrHWSetPasswd)
		{
			StrHWSetPasswd=value;
			sprintf(pRCtrl->HWSetPasswd,"%s",value);	
			Setconfig("HWSetPasswd=",value);		
		}
	}
	
	//服务器推送地址列表
    jsonlist = cJSON_GetObjectItem(json, "serverurllist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
				sprintf(key,"url");
				jsonkey=cJSON_GetObjectItem(jsonitem,key);
				if(jsonkey != NULL)
				{
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s%d=%s\n",key,i+1,value);
					//服务器1推送地址
					if(i==0 && value!=StrServerURL1)
					{
						StrServerURL1=value;
						sprintf(pRCtrl->ServerURL1,"%s",value); 
						sprintf(key,"ServerURL1=",i+1);
						Setconfig(key,value);
					}
					//服务器2推送地址
					if(i==1 && value!=StrServerURL2)
					{
						StrServerURL2=value;
						sprintf(pRCtrl->ServerURL2,"%s",value); 
						sprintf(key,"ServerURL2=",i+1);//RSUIP地址
						Setconfig(key,value);
					}
					//服务器3推送地址
					if(i==2 && value!=StrServerURL3)
					{
						StrServerURL3=value;
						sprintf(pRCtrl->ServerURL3,"%s",value); 
						sprintf(key,"ServerURL3=",i+1);//RSUIP地址
						Setconfig(key,value);
					}
					//门锁4推送地址
					if(i==3 && value!=StrServerURL4)
					{
						StrServerURL4=value;
						sprintf(pRCtrl->ServerURL4,"%s",value); 
						sprintf(key,"ServerURL4=",i+1);//RSUIP地址
						Setconfig(key,value);
					}
				}
            }
        }
    }
	
	//控制器接收地址
	sprintf(key,"stationurl");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrStationURL)
		{
			StrStationURL=value;
			sprintf(pRCtrl->StationURL,"%s",value); 
			Setconfig("StationURL=",value);
		}
	}
	
	//RSU控制器数量
	sprintf(key,"rsucount");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrRSUCount && atoi(value)>=0 && atoi(value)<=RSUCTL_NUM)
		{
			StrRSUCount=value;
			sprintf(pRCtrl->RSUCount,"%s",value); 
			Setconfig("RSUCount=",value);
		}
	}
	
	//RSU 列表
    jsonlist = cJSON_GetObjectItem(json, "rsulist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//RSUIP地址
				sprintf(key,"ip");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrRSUIP[i])
					{
						StrRSUIP[i]=value;
						sprintf(pRCtrl->RSUIP[i],"%s",value); 
						sprintf(key,"RSU%dIP=",i+1);//RSUIP地址
						Setconfig(key,value);
					}
                }
				//RSU端口
				sprintf(key,"port");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrRSUPort[i])
					{
						StrRSUPort[i]=value;
						sprintf(pRCtrl->RSUPort[i],"%s",value);  
						sprintf(key,"RSU%dPort=",i+1);//RSU端口
						Setconfig(key,value);
					}
                }
            }
        }
    }
	
	//识别仪数量
	sprintf(key,"vehplatecount");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrVehPlateCount && atoi(value)>=0 && atoi(value)<=VEHPLATE_NUM)
		{
			StrVehPlateCount=value; 
			sprintf(pRCtrl->VehPlateCount,"%s",value);	
			Setconfig("VehPlateCount=",value);
		}
	}
	
	//识别仪列表
    jsonlist = cJSON_GetObjectItem(json, "vehplatelist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//IP地址
				sprintf(key,"ip");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrVehPlateIP[i])
					{
						StrVehPlateIP[i]=value; 
						sprintf(pRCtrl->VehPlateIP[i],"%s",value);	
						sprintf(key,"VehPlate%dIP=",i+1);
						Setconfig(key,value);
					}
                }
				//端口
				sprintf(key,"port");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrVehPlatePort[i])
					{
						StrVehPlatePort[i]=value;
						sprintf(pRCtrl->VehPlatePort[i],"%s",value);	
						sprintf(key,"VehPlate%dPort=",i+1);
						Setconfig(key,value);
					}
                }
				//识别仪用户名密码
				sprintf(key,"key");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrVehPlateKey[i])
					{
						StrVehPlateKey[i]=value;
						sprintf(pRCtrl->VehPlateKey[i],"%s",value);
						sprintf(key,"VehPlate%dKey=",i+1);
						Setconfig(key,value);
					}
                }
            }
        }
    }
	
	//900识别仪数量
	sprintf(key,"vehplate900count");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrVehPlate900Count && atoi(value)>=0 && atoi(value)<=VEHPLATE900_NUM)
		{
			StrVehPlate900Count=value;	
			sprintf(pRCtrl->VehPlate900Count,"%s",value);	
			Setconfig("VehPlate900Count=",value);
		}
	}
	
	//900识别仪列表
    jsonlist = cJSON_GetObjectItem(json, "vehplate900list");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize && i<VEHPLATE900_NUM;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//IP地址
				sprintf(key,"ip");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrVehPlate900IP[i])
					{
						StrVehPlate900IP[i]=value; 
						sprintf(pRCtrl->VehPlate900IP[i],"%s",value);	
						sprintf(key,"VehPlate900%dIP=",i+1);
						Setconfig(key,value);
					}
                }
				//端口
				sprintf(key,"port");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrVehPlate900Port[i])
					{
						StrVehPlate900Port[i]=value;
						sprintf(pRCtrl->VehPlate900Port[i],"%s",value);	
						sprintf(key,"VehPlate900%dPort=",i+1);
						Setconfig(key,value);
					}
                }
				//识别仪用户名密码
				sprintf(key,"key");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrVehPlate900Key[i])
					{
						StrVehPlate900Key[i]=value;
						sprintf(pRCtrl->VehPlate900Key[i],"%s",value);
						sprintf(key,"VehPlate900%dKey=",i+1);
						Setconfig(key,value);
					}
                }
            }
        }
    }
	
	//监控摄像头数量
	sprintf(key,"camcount");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrCAMCount && atoi(value)>=0 && atoi(value)<=CAM_NUM)
		{
			StrCAMCount=value;	
			sprintf(pRCtrl->CAMCount,"%s",value);	
			Setconfig("CAMCount=",value);
		}
	}
	
	//监控摄像头列表
    jsonlist = cJSON_GetObjectItem(json, "camlist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize && i<CAM_NUM;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//IP地址
				sprintf(key,"ip");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrCAMIP[i])
					{
						StrCAMIP[i]=value;	
						sprintf(pRCtrl->CAMIP[i],"%s",value);	
						sprintf(key,"CAM%dIP=",i+1);
						Setconfig(key,value);
					}
                }
				//端口
				sprintf(key,"port");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrCAMPort[i])
					{
						StrCAMPort[i]=value;
						sprintf(pRCtrl->CAMPort[i],"%s",value); 
						sprintf(key,"CAM%dPort=",i+1);
						Setconfig(key,value);
					}
                }
				//监控摄像头用户名密码
				sprintf(key,"key");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrCAMKey[i])
					{
						StrCAMKey[i]=value;
						sprintf(pRCtrl->CAMKey[i],"%s",value);
						sprintf(key,"CAM%dKey=",i+1);
						Setconfig(key,value);
					}
                }
            }
        }
    }
	
	//交换机数量
	sprintf(key,"ipswitchcount");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrIPSwitchCount && atoi(value)>=0 && atoi(value)<=IPSWITCH_NUM)
		{
			StrIPSwitchCount=value; 
			sprintf(pRCtrl->SwitchCount,"%s",value);	
			Setconfig("SwitchCount=",value);
		}
	}
	
	//交换机列表
    jsonlist = cJSON_GetObjectItem(json, "ipswitchlist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize && i<IPSWITCH_NUM;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//IP地址
				sprintf(key,"ip");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrIPSwitchIP[i])
					{
						StrIPSwitchIP[i]=value; 
						sprintf(pRCtrl->SwitchIP[i],"%s",value);	
						sprintf(key,"Switch%dIP=",i+1);
						Setconfig(key,value);
					}
                }
				//交换机get密码
				sprintf(key,"getpasswd");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrIPSwitchGetPasswd[i])
					{
						StrIPSwitchGetPasswd[i]=value;
						sprintf(pRCtrl->SwitchGetPasswd[i],"%s",value); 
						sprintf(key,"Switch%dGetPasswd=",i+1);
						Setconfig(key,value);
					}
                }
				//交换机set密码
				sprintf(key,"setpasswd");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrIPSwitchSetPasswd[i])
					{
						StrIPSwitchSetPasswd[i]=value;
						sprintf(pRCtrl->SwitchSetPasswd[i],"%s",value);
						sprintf(key,"Switch%dSetPasswd=",i+1);
						Setconfig(key,value);
					}
                }
            }
        }
    }
	
	//防火墙数量
	sprintf(key,"firewarecount");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrFireWareCount && atoi(value)>=0 && atoi(value)<=FIREWARE_NUM)
		{
			StrFireWareCount=value; 
			sprintf(pRCtrl->FireWareCount,"%s",value);	
			Setconfig("FireWareCount=",value);
		}
	}
	
	//防火墙列表
    jsonlist = cJSON_GetObjectItem(json, "firewarelist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize && i<FIREWARE_NUM;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//IP地址
				sprintf(key,"ip");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrFireWareIP[i])
					{
						StrFireWareIP[i]=value; 
						sprintf(pRCtrl->FireWareIP[i],"%s",value);	
						sprintf(key,"FireWare%dIP=",i+1);
						Setconfig(key,value);
					}
                }
				//防火墙get密码
				sprintf(key,"getpasswd");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrFireWareGetPasswd[i])
					{
						StrFireWareGetPasswd[i]=value;
						sprintf(pRCtrl->FireWareGetPasswd[i],"%s",value);	
						sprintf(key,"FireWare%dGetPasswd=",i+1);
						Setconfig(key,value);
					}
                }
				//防火墙set密码
				sprintf(key,"setpasswd");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrFireWareSetPasswd[i])
					{
						StrFireWareSetPasswd[i]=value;
						sprintf(pRCtrl->FireWareSetPasswd[i],"%s",value);
						sprintf(key,"FireWare%dSetPasswd=",i+1);
						Setconfig(key,value);
					}
                }
            }
        }
    }
	
	//Atlas数量
	sprintf(key,"atlascount");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrAtlasCount && atoi(value)>=0 && atoi(value)<=ATLAS_NUM)
		{
			StrAtlasCount=value;	
			sprintf(pRCtrl->AtlasCount,"%s",value); 
			Setconfig("AtlasCount=",value);
		}
	}
	
	//Atlas列表
    jsonlist = cJSON_GetObjectItem(json, "atlaslist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize && i<ATLAS_NUM;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//Atlas地址
				sprintf(key,"ip");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrAtlasIP[i])
					{
						StrAtlasIP[i]=value; 
						sprintf(pRCtrl->AtlasIP[i],"%s",value);	
						sprintf(key,"Atlas%dIP=",i+1);
						Setconfig(key,value);
					}
                }
				//Atlas密码
				sprintf(key,"passwd");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrAtlasPasswd[i])
					{
						StrAtlasPasswd[i]=value;
						sprintf(pRCtrl->AtlasPasswd[i],"%s",value); 
						sprintf(key,"Atlas%dPasswd=",i+1);
						Setconfig(key,value);
					}
                }
            }
        }
    }
	
	//防雷器SPD数量
	sprintf(key,"spdcount");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrSPDCount && atoi(value)>=0 && atoi(value)<=SPD_NUM)
		{
			StrSPDCount=value;	
			SPD_num =atoi(StrSPDCount.c_str());
			printf("spdcount = %d\r\n",SPD_num);
			sprintf(pRCtrl->SPDCount,"%s",value);	
			Setconfig("SPDCount=",value);
			// 数量改变，全部初始化
			for(i=0;i<SPD_NUM+RES_NUM;i++)
			{
				HZ_reset_pre[i] = true;
			}
		}
	}
	
	//防雷器SPD类型
	sprintf(key,"spdtype");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrSPDType)
		{
			StrSPDType=value;	
			SPD_Type = atoi(StrSPDType.c_str());
			printf("spdtype = %d\r\n",SPD_Type);
			sprintf(pRCtrl->SPDType,"%s",value);	
			Setconfig("SPDType=",value);
			// 类型改变，全部初始化
			for(i=0;i<SPD_NUM+RES_NUM;i++)
			{
				HZ_reset_pre[i] = true;
			}
		}
	}
	
	//防雷器列表
    jsonlist = cJSON_GetObjectItem(json, "spdlist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//防雷器ip地址
				sprintf(key,"ip");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrSPDIP[i])
					{
						StrSPDIP[i]=value; 
						sprintf(gsSPDIP[i],StrSPDIP[i].c_str());
						sprintf(pRCtrl->SPDIP[i],"%s",value);
						sprintf(key,"SPD%dIP=",i+1);
						Setconfig(key,value);
						HZ_reset_pre[i] = true;
					}
                }
            	//防雷器端口
				sprintf(key,"port");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrSPDPort[i])
					{
						StrSPDPort[i]=value; 
						sprintf(gsSPDPort[i],StrSPDPort[i].c_str());
						sprintf(pRCtrl->SPDPort[i],"%s",value);	
						sprintf(key,"SPD%dPort=",i+1);
						Setconfig(key,value);
						HZ_reset_pre[i] = true;
					}
                }
            	//防雷器设备地址
				sprintf(key,"addr");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					if(value!=StrSPDAddr[i])
					{
						StrSPDAddr[i]=value; 
						SPD_Address[i] = atoi(StrSPDAddr[i].c_str());
						sprintf(pRCtrl->SPDAddr[i],"%s",value);	
						sprintf(key,"SPD%dAddr=",i+1);
						Setconfig(key,value);
						HZ_reset_pre[i] = true;
					}
                }
            }
        }
    }
	
	//接地检测器地址
	sprintf(key,"spdresip");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrSPDIP[SPD_NUM])
		{
			StrSPDIP[SPD_NUM]=value;	
			sprintf(gsSPDIP[SPD_NUM],StrSPDIP[SPD_NUM].c_str());
			sprintf(pRCtrl->SPDAddr[SPD_NUM],"%s",value); 
			Setconfig("SPDResIP=",value);
			HZ_reset_pre[SPD_NUM] = true;
		}
	}
	//接地检测器端口
	sprintf(key,"spdresport");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrSPDPort[SPD_NUM])
		{
			StrSPDPort[SPD_NUM]=value;	
			sprintf(gsSPDPort[SPD_NUM],StrSPDPort[SPD_NUM].c_str());
			sprintf(pRCtrl->SPDPort[SPD_NUM],"%s",value); 
			Setconfig("SPDResPort=",value);
			HZ_reset_pre[SPD_NUM] = true;
		}
	}
	//接地检测器id
	sprintf(key,"spdresid");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrSPDAddr[SPD_NUM])
		{
			StrSPDAddr[SPD_NUM]=value;	
			SPD_Address[SPD_NUM] = atoi(StrSPDAddr[SPD_NUM].c_str());
			sprintf(pRCtrl->SPDAddr[SPD_NUM],"%s",value); 
			Setconfig("SPDResPort=",value);
			HZ_reset_pre[SPD_NUM] = true;
			//Ex_SPD_Set_Process(SPD_RES_SET,RES_ID_ADDR,dummy,SPD_Address[SPD_NUM]);
		}
	}
	
	//接地电阻报警值
	sprintf(key,"spdres_alarm_value");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		UINT16 addr_ref=1;
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(atoi(value)!=stuSpd_Param->rSPD_res.alarm_value)
		{
			stuSpd_Param->rSPD_res.alarm_value=atoi(value);
			printf("spdres报警值修改=%s\n",value);
			//spdres更改报警值
			if (SPD_Type == TYPE_LEIXUN)
			{
				addr_ref = RES_ALARM_ADDR;
			}
			else if (SPD_Type == TYPE_KY)
			{
				addr_ref = KY_RES_ALARM_ADDR;
			}
			Ex_SPD_Set_Process(0,SPD_RES_SET,addr_ref,dummy,atoi(value));
		}
	}
	
	//do数量
	sprintf(key,"do_count");
	jsonkey = cJSON_GetObjectItem(json, key);
	if(jsonkey!=0)				
	{
		sprintf(value,"%s",jsonkey->valuestring);
		printf("%s %s\n",key,value);
		if(value!=StrDoCount && atoi(value)>=0 && atoi(value)<=SWITCH_COUNT)
		{
			StrDoCount=value;	
			sprintf(pRCtrl->DoCount,"%s",value); 
			Setconfig("DO_Count=",value);
		}
	}
		
	//do列表
    jsonlist = cJSON_GetObjectItem(json, "dolist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//设备名
				sprintf(key,"name");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(keytmp,"%s",jsonkey->valuestring);
                }
            	//do值
				sprintf(key,"value");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
                }
            }
			printf("%s %s\n",keytmp,value);

			bool found=false;
			for(int j=0;j<SWITCH_COUNT;j++)
			{
				if(keytmp==StrDeviceNameSeq[j])
				{   
					found=true;
					if(StrDoSeq[j]!=value)
					{
						StrDoSeq[j]=value;
						DoSeq[j]=atoi(value);
						string stmp=StrDeviceNameSeq[j];
						transform(stmp.begin(), stmp.end(), stmp.begin(), ::toupper);
						stmp=stmp+"=";
						Setconfig(stmp.c_str(),value);//DO映射
					}
				}
			}
			if(found==false)		//匹配不到该设备名，增加一个do
			{
				int pos=atoi(value);
				StrDoSeq[pos-1]=value;
				DoSeq[pos-1]=atoi(value);
				StrDeviceNameSeq[pos-1]=keytmp;
				string stmp=keytmp;
				transform(stmp.begin(), stmp.end(), stmp.begin(), ::toupper);
				stmp=stmp+"=";
				Setconfig(stmp.c_str(),value);//DO映射
			}
				
        }
    }
	
	// 统一处理防雷接地初始化标志
	for(i=0;i<SPD_NUM+RES_NUM;i++)
	{
		if (SPD_Type == TYPE_HUAZI)
		{
			if (HZ_reset_pre[i] == true)
			{
				HZ_reset_pre[i] = false;
				HZ_reset_flag[i] = true;
			}
		}
	}
	
	// 没有配置的都置空
	if (SPD_num == 1)
	{
		for (i=1;i<SPD_NUM;i++)
		{
			StrSPDIP[i] =""; ;//防雷器IP
			sprintf(key,"SPD%dIP=",i+1);
			Setconfig(key,StrSPDIP[i]);
			StrSPDPort[i] ="";//防雷器端口
			sprintf(key,"SPD%dPort=",i+1);
			Setconfig(key,StrSPDPort[i]);
			StrSPDAddr[i] ="";//防雷器硬件端口
			sprintf(key,"SPD%dAddr=",i+1);
			Setconfig(key,StrSPDAddr[i]);
		}
		Setconfig("SPDResIP=","");
		Setconfig("SPDResPort=","");
	}
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



bool jsonStrVMCtlParamWriter(int messagetype,char *pstrVMCtl,string &mstrjson)
{
	VMCONTROL_PARAM *pParm=(VMCONTROL_PARAM *)pstrVMCtl;
	int vehplatecnt,vehplate900cnt,rsucnt;
	
	char str[100],sDateTime[30];
	int i,j,CabinetType; 
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

	CabinetType=atoi(StrCabinetType.c_str());
	if(CabinetType==JINSHENGAN_CAB)
	{
		strJson = strJson + "\"hwserver2\":\""+ StrHWServer2 +"\",\n";	//华为服务器IP地址
		strJson = strJson + "\"hwgetpasswd2\":\""+ StrHWGetPasswd2 +"\",\n";	//SNMP GET 密码
		strJson = strJson + "\"hwsetpasswd2\":\""+ StrHWSetPasswd2 +"\",\n";	//SNMP SET 密码
	}
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
	strJson = strJson + "\"vehplate900count\":\""+ StrVehPlate900Count +"\",\n";	//900识别仪数量
	vehplate900cnt=atoi(StrVehPlate900Count.c_str());
	for(i=0;i<VEHPLATE900_NUM;i++)
	{
		sprintf(str,"\"vehplate900%dip\":\"%s\",\n",i+1,StrVehPlate900IP[i].c_str()); strJson = strJson + str;//900识别仪地址
		sprintf(str,"\"vehplate900%dport\":\"%s\",\n",i+1,StrVehPlate900Port[i].c_str()); strJson = strJson + str;//900识别仪端口
		sprintf(str,"\"vehplate900%dkey\":\"%s\",\n",i+1,StrVehPlate900Key[i].c_str()); strJson = strJson + str;//900识别仪用户名密码
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
	sprintf(str,"\"firewaretype\":\"%d\",\n",IntFireWareType); strJson = strJson + str;//防火墙类型
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
	sprintf(str,"\"ipswitchtype\":\"%d\",\n",IntIPSwitchType); strJson = strJson + str;//交换机类型
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
	//防雷器
	strJson = strJson + "\"spdcount\":\""+ StrSPDCount +"\",\n";	//防雷器数量
	strJson = strJson + "\"spdtype\":\""+ StrSPDType +"\",\n";	//防雷器类型
	if(StrSPDType=="1")
		strJson = strJson + "\"spdfactory\":\"雷迅\",\n";	//防雷器厂商
	else if(StrSPDType=="2")
		strJson = strJson + "\"spdfactory\":\"华咨圣泰\",\n";	//防雷器厂商
	else
		strJson = strJson + "\"spdfactory\":\"\",\n";	//防雷器厂商
	for(i=0;i<SPD_NUM;i++)
	{
		sprintf(str,"\"spd%dip\":\"%s\",\n",i+1,StrSPDIP[i].c_str()); //防雷器IP
		strJson = strJson + str;
		sprintf(str,"\"spd%dport\":\"%s\",\n",i+1,StrSPDPort[i].c_str()); //防雷器端口
		strJson = strJson + str;
		sprintf(str,"\"spd%daddr\":\"%s\",\n",i+1,StrSPDAddr[i].c_str()); //防雷器设备地址
		strJson = strJson + str;
	}
	sprintf(str,"\"spdresip\":\"%s\",\n",StrSPDIP[SPD_NUM].c_str()); //接地电阻IP
	strJson = strJson + str;
	sprintf(str,"\"spdresport\":\"%s\",\n",StrSPDPort[SPD_NUM].c_str()); //接地电阻端口
	strJson = strJson + str;
	sprintf(str,"\"spdresid\":\"%s\",\n",StrSPDAddr[SPD_NUM].c_str()); //接地电阻设备地址
	strJson = strJson + str;
	sprintf(str,"\"spdres_alarm_value\":\"%d\",\n",stuSpd_Param->rSPD_res.alarm_value); //接地电阻报警值
	strJson = strJson + str;
	
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
	for(i=0;i<SWITCH_COUNT;i++)
	{
		if(StrUnWireDevName[i]!="" && StrUnWireDo[i]!="")
		{sprintf(str,"\"%s\":\"%s\",\n",StrUnWireDevName[i].c_str(),StrUnWireDo[i].c_str()); strJson = strJson + str;}//没接线设备映射DO
	}
	
	strJson = strJson + "\"devicetype\":\""+ StrdeviceType +"\",\n";	//设备型号900~919
	strJson = strJson + "\"hardwareid\":\""+ StrID +"\",\n";	//硬件ID
	strJson = strJson + "\"softversion\":\""+ StrVersionNo +"\",\n";	//主程序版本号920
	
	strJson = strJson + "\"secsoftversion1\":\""+ stuVMCtl_Param->secSoftVersion[0] +"\",\n";	//副版本号
	strJson = strJson + "\"secsoftversion2\":\""+ stuVMCtl_Param->secSoftVersion[1] +"\",\n";	//副版本号
	strJson = strJson + "\"secsoftversion3\":\""+ stuVMCtl_Param->secSoftVersion[2] +"\",\n";	//副版本号
	strJson = strJson + "\"softdate\":\""+ StrSoftDate +"\"\n";	//版本日期
	
    strJson +=  "}";

    mstrjson = strJson;
	return true;
}


bool jsonStrVMCtlParamWriterXY(int messagetype,char *pstrVMCtl, string &mstrjson)
{
	VMCONTROL_PARAM *pParm=(VMCONTROL_PARAM *)pstrVMCtl;
	int vehplatecnt,vehplate900cnt,rsucnt;
	
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
	
	strJson = strJson + "\"vehplate900count\":\""+ StrVehPlate900Count +"\",\n";	//900识别仪数量
	vehplate900cnt=atoi(StrVehPlate900Count.c_str());
	strJson = strJson + "\"vehplate900list\": [\n";		//vehplate900列表
	for(i=0;i<VEHPLATE900_NUM;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"vehplate900%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"ip\":\"%s\",\n",StrVehPlate900IP[i].c_str()); strJson = strJson + str;//900识别仪地址
		sprintf(str,"\"port\":\"%s\",\n",StrVehPlate900Port[i].c_str()); strJson = strJson + str;//900识别仪端口
		sprintf(str,"\"key\":\"%s\"\n",StrVehPlate900Key[i].c_str()); strJson = strJson + str;//900识别仪用户名密码
		if(i<VEHPLATE900_NUM-1)
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
	
	strJson = strJson + "\"atlascount\":\""+ StrAtlasCount +"\",\n";	//atlas数量
	strJson = strJson + "\"atlaslist\": [\n";		//atlas列表
	for(i=0;i<ATLAS_NUM;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"atlas%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"ip\":\"%s\",\n",StrAtlasIP[i].c_str()); strJson = strJson + str;//atlasIP
		sprintf(str,"\"passwd\":\"%s\"\n",StrAtlasPasswd[i].c_str()); strJson = strJson + str;//atlas密码
		if(i<ATLAS_NUM-1)
			strJson +=	"},\n";
		else
			strJson +=	"}\n";
	}
	strJson +=	"],\n";
	
	strJson = strJson + "\"spdcount\":\""+ StrSPDCount +"\",\n";	//防雷器数量
	strJson = strJson + "\"spdtype\":\""+ StrSPDType +"\",\n";	//防雷器类型
	if(StrSPDType=="1")
		strJson = strJson + "\"spdfactory\":\"雷迅\",\n";	//防雷器厂商
	else if(StrSPDType=="2")
		strJson = strJson + "\"spdfactory\":\"华咨圣泰\",\n";	//防雷器厂商
	else
		strJson = strJson + "\"spdfactory\":\"\",\n";	//防雷器厂商
	strJson = strJson + "\"spdlist\": [\n";		//防雷器列表
	for(i=0;i<SPD_NUM;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"name\":\"spd%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"ip\":\"%s\",\n",StrSPDIP[i].c_str()); strJson = strJson + str;//防雷器IP
		sprintf(str,"\"port\":\"%s\",\n",StrSPDPort[i].c_str()); strJson = strJson + str;//防雷器端口
		sprintf(str,"\"addr\":\"%s\"\n",StrSPDAddr[i].c_str()); strJson = strJson + str;//设备地址
		if(i<SPD_NUM-1)
			strJson +=	"},\n";
		else
			strJson +=	"}\n";
	}
	strJson +=	"],\n";
	
	sprintf(str,"\"spdresip\":\"%s\",\n",StrSPDIP[SPD_NUM].c_str()); //接地电阻IP
	strJson = strJson + str;
	sprintf(str,"\"spdresport\":\"%s\",\n",StrSPDPort[SPD_NUM].c_str()); //接地电阻端口
	strJson = strJson + str;
	sprintf(str,"\"spdresid\":\"%s\",\n",StrSPDAddr[SPD_NUM].c_str()); //接地电阻设备地址
	strJson = strJson + str;
	sprintf(str,"\"spdres_alarm_value\":\"%d\",\n",stuSpd_Param->rSPD_res.alarm_value); //接地电阻报警值
	strJson = strJson + str;
	
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
	
	strJson +=	"}";

    mstrjson = strJson;
	return true;
}

bool jsonStrRsuWriterXY(int messagetype, string &mstrjson)
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
		if(stuRsuControl[i].Linked && GetTickCount()-stuRsuControl[i].TimeStamp>1*60*1000) //1分钟没更新，默认没连接
			initRSUState(i);
		
		strJson = strJson + "{\n";	
		
		sprintf(str,"\"id\":\"%d\",\n",i+1); strJson = strJson + str;//RSU编号
		sprintf(str,"\"name\":\"rsu%d\",\n",i+1); strJson = strJson + str;//设备名称
		strJson = strJson + "\"ip\":\""+ StrRSUIP[i] +"\",\n";	//RSUIP地址
		sprintf(str,"\"port\":\"%s\",\n",StrRSUPort[i].c_str()); strJson = strJson + str;//RSU端口
		sprintf(str,"\"volt\":%.3f,\n",stuVA_Meter_Param[i]->phase[i].vln/100.0);strJson = strJson + str;	//电压 
		sprintf(str,"\"amp\":%.3f,\n",stuVA_Meter_Param[i]->phase[i].amp/1000.0); strJson = strJson + str;//电流
		sprintf(str,"\"controlstatus\":%d,\n",stuRsuControl[i].ControlStatus_1);strJson = strJson + str;//控制器1状态;00:表示正常 ，否则表示异常

		sprintf(str,"\"algid\":%d,\n",stuRsuData[i].RSUAlgId); strJson = strJson + str;//算法标识，默认填写00H
		sprintf(str,"\"manuid\":\"%02x\",\n",stuRsuData[i].RSUManuID); strJson = strJson + str;//路侧单元厂商代码
		if(stuRsuData[i].RSUManuID==0xa8)
			{sprintf(str,"\"manufactory\":\"爱特斯\",\n"); strJson = strJson + str;}//路侧单元厂商
		else if(stuRsuData[i].RSUManuID==0xa2)
			{sprintf(str,"\"manufactory\":\"万集\",\n"); strJson = strJson + str;}//路侧单元厂商
		sprintf(str,"\"controlid\":\"%02x%02x%02x\",\n",stuRsuData[i].RSUID[0],stuRsuData[i].RSUID[1],stuRsuData[i].RSUID[2]); strJson = strJson + str;//路侧单元编号
		sprintf(str,"\"softwareversion\":\"%02x%02x\",\n",stuRsuData[i].RSUVersion[0],stuRsuData[i].RSUVersion[1]); strJson = strJson + str;//路侧单元软件版本号
		sprintf(str,"\"workstatus\":%d,\n",stuRsuData[i].Workstatus);strJson = strJson + str;	//工作模式返回状态，默认填写00H
		sprintf(str,"\"flagid\":\"%02x%02x%02x\",\n",stuRsuData[i].FlagID[0],stuRsuData[i].FlagID[1],stuRsuData[i].FlagID[2]); strJson = strJson + str;//ETC门架编号（由C0 帧中获取,失败填充00H
		if(stuRsuData[i].PSAMCount>0)
			{sprintf(str,"\"psamcount\":%d,\n",stuRsuData[i].PSAMCount); strJson = strJson + str;}//PSAM数量
		else
			{sprintf(str,"\"psamcount\":%d\n",stuRsuData[i].PSAMCount); strJson = strJson + str;}//PSAM数量
		if(stuRsuData[i].PSAMCount>0)
		{
			strJson = strJson + "\"psamlist\": [\n";		//psam列表
			for(j=0;j<stuRsuData[i].PSAMCount;j++)
			{
				strJson = strJson + "{\n";	
				sprintf(str,"\"id\": \"%d\",\n", stuRsuData[i].PSAMInfoN[j].PSAM_channel);strJson += str;		//PSAM卡插槽号
				sprintf(str,"\"name\": \"psam%d\",\n", j+1);strJson += str;		//名称
				sprintf(str,"\"psamid\":\"%02x%02x%02x%02x%02x%02x\",\n",stuRsuData[i].PSAMInfoN[j].Psam_ID[0],
					stuRsuData[i].PSAMInfoN[j].Psam_ID[1],stuRsuData[i].PSAMInfoN[j].Psam_ID[2],stuRsuData[i].PSAMInfoN[j].Psam_ID[3],
					stuRsuData[i].PSAMInfoN[j].Psam_ID[4],stuRsuData[i].PSAMInfoN[j].Psam_ID[5]);
				strJson = strJson + str; // PSAM信息
				sprintf(str,"\"status\": \"%d\"\n", stuRsuData[i].PSAMInfoN[j].PSAM_auth);strJson += str;		//1字节PSAM授权状态00H已授权01H未授权
				if(j==stuRsuData[i].PSAMCount-1)
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
	strJson = strJson + "],\n";
	if(stuRsuControl[0].AntennaCount>0)
	{
		sprintf(str,"\"antennacount\":%d,\n",stuRsuControl[0].AntennaCount);strJson = strJson + str;	//天线数量
		strJson = strJson + "\"antennalist\": [\n"; 	//天线头列表
		for(j=0;j<stuRsuControl[0].AntennaCount;j++)
		{
			strJson = strJson + "{\n";	
			sprintf(str,"\"id\": \"%d\",\n", j+1);strJson += str;		//天线编号
			sprintf(str,"\"name\": \"antenna%d\",\n", j+1);strJson += str;		//名称
			sprintf(str,"\"status\": %d,\n", stuRsuControl[0].AntennaInfoN[j].Control_state);strJson += str;		//38 天线i 控制状态
			sprintf(str,"\"power\": %d,\n", stuRsuControl[0].AntennaInfoN[j].Power);strJson += str; //39 天线i 功率
			sprintf(str,"\"channel\": %d\n", stuRsuControl[0].AntennaInfoN[j].Channel);strJson += str; //40 天线i 信道号
			//sprintf(str,"\"sendstatus\": %d,\n", stuRsuControl[0].AntennaInfoN[j].send_status);strJson += str; //41 天线i 发射状态,00H正常，01H异常
			//sprintf(str,"\"recvstatus\": %d\n", stuRsuControl[0].AntennaInfoN[j].recv_status);strJson += str;	//41 天线i 接收状态,00H正常，01H异常
			if(j==stuRsuControl[0].AntennaCount-1)
				strJson = strJson + "}\n";
			else
				strJson = strJson + "},\n";
		}
		strJson = strJson + "]\n";
	}
	else
	{
		sprintf(str,"\"antennacount\":%d\n",stuRsuControl[0].AntennaCount);strJson = strJson + str;	//天线数量
	}
	
	strJson = strJson + "}\n";

	mstrjson=strJson;
	return true;
}


bool jsonStrVehPlateWriter(int messagetype, string &mstrjson)
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
		if(mTIPcamState[i].Linked && GetTickCount()-mTIPcamState[i].TimeStamp>5*60*1000)//5分钟没更新，恢复默认
			initIPcamState(i);
		
		strJson = strJson + "{\n";
	
		sprintf(str,"\"name\":\"vehplate%d\",\n",i+1);strJson+=str;	//摄相机名称
		sprintf(str,"\"ip\":\"%s\",\n",StrVehPlateIP[i].c_str());strJson+=str;	//摄相机IP
		sprintf(str,"\"port\":\"%s\",\n",StrVehPlatePort[i].c_str());strJson+=str;	//识别仪端口
		sprintf(str,"\"key\":\"%s\",\n",StrVehPlateKey[i].c_str());strJson+=str;	//用户名密码
		sprintf(str,"\"picstateid\":%s,\n",mTIPcamState[i].picstateid.c_str());strJson+=str;	//流水号
		sprintf(str,"\"gantryid\":%s,\n",mTIPcamState[i].gantryid.c_str());strJson+=str;	//门架编号,全网唯一编号
		sprintf(str,"\"statetime\":%s,\n",mTIPcamState[i].statetime.c_str());strJson+=str	;	//状态采集时间
		sprintf(str,"\"overstockimagejourcount\":%s,\n",mTIPcamState[i].overstockImageJourCount.c_str());strJson+=str;	//积压图片流水数
		sprintf(str,"\"overstockimagecount\":%s,\n",mTIPcamState[i].overstockImageCount.c_str());strJson+=str;	//积压图片数
		sprintf(str,"\"cameranum\":%s,\n",mTIPcamState[i].cameranum.c_str());strJson+=str;	//相机编号（101~299）
		sprintf(str,"\"lanenum\":%s,\n",mTIPcamState[i].lanenum.c_str());strJson+=str;	//车道编号
		sprintf(str,"\"connectstatus\":%s,\n",mTIPcamState[i].connectstatus.c_str());strJson+=str;	//连接状态
		sprintf(str,"\"workstatus\":%s,\n",mTIPcamState[i].workstatus.c_str());strJson+=str;	//工作状态
		sprintf(str,"\"lightworkstatus\":%s,\n",mTIPcamState[i].lightworkstatus.c_str());strJson+=str;	//补光灯的工作状态
		sprintf(str,"\"recognitionrate\":%s,\n",mTIPcamState[i].recognitionrate.c_str());strJson+=str;	//识别成功率
		sprintf(str,"\"hardwareversion\":%s,\n",mTIPcamState[i].hardwareversion.c_str());strJson+=str;//固件版本
		sprintf(str,"\"softwareversion\":%s,\n",mTIPcamState[i].softwareversion.c_str());strJson+=str;	//软件版本
		sprintf(str,"\"runningtime\":%s,\n",mTIPcamState[i].runningtime.c_str());strJson+=str;	//设备从开机到现在的运行时间（秒）
		sprintf(str,"\"brand\":%s,\n",mTIPcamState[i].brand.c_str());strJson+=str;	//厂商型号
		sprintf(str,"\"devicetype\":%s,\n",mTIPcamState[i].devicetype.c_str());strJson+=str;	//设备型号
		sprintf(str,"\"statuscode\":%s,\n",mTIPcamState[i].statuscode.c_str());strJson+=str;	//状态码,详见附录A3 0-正常；其他由厂商自定义
		sprintf(str,"\"statusmsg\":%s\n",mTIPcamState[i].statusmsg.c_str());strJson+=str;	//状态描述 由厂商自定义,最大长度256 例如：正常
			
		if(i==vehplatecnt-1)
			strJson = strJson + "}\n";
		else
			strJson = strJson + "},\n";
	}
	strJson = strJson + "]\n";
	
	strJson +=	"}\n\n\0\0";

	mstrjson=strJson;
	return true;
}

bool jsonStrVehPlate900Writer(int messagetype, string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j; 
	int vehplate900cnt;
	
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
	strJson = strJson + "\"vehplate900cnt\":\""+ StrVehPlate900Count +"\",\n";	//车牌识别数量
	vehplate900cnt=atoi(StrVehPlate900Count.c_str());
	strJson = strJson + "\"vehplate900list\":[\n";
	for(i=0;i<vehplate900cnt;i++)
	{
		if(mTIPcam900State[i].Linked && GetTickCount()-mTIPcam900State[i].TimeStamp>5*60*1000)//5分钟没更新，恢复默认
			initIPcamState(i);
		strJson = strJson + "{\n";
	
		sprintf(str,"\"name\":\"vehplate900%d\",\n",i+1);strJson+=str;	//摄相机名称
		sprintf(str,"\"ip\":\"%s\",\n",StrVehPlate900IP[i].c_str());strJson+=str;	//摄相机IP
		sprintf(str,"\"port\":\"%s\",\n",StrVehPlate900Port[i].c_str());strJson+=str;	//识别仪端口
		sprintf(str,"\"key\":\"%s\",\n",StrVehPlate900Key[i].c_str());strJson+=str;	//用户名密码
		sprintf(str,"\"picstateid\":%s,\n",mTIPcam900State[i].picstateid.c_str());strJson+=str;	//流水号
		sprintf(str,"\"gantryid\":%s,\n",mTIPcam900State[i].gantryid.c_str());strJson+=str;	//门架编号,全网唯一编号
		sprintf(str,"\"statetime\":%s,\n",mTIPcam900State[i].statetime.c_str());strJson+=str	;	//状态采集时间
		sprintf(str,"\"overstockimagejourCount\":%s,\n",mTIPcam900State[i].overstockImageJourCount.c_str());strJson+=str;	//积压图片流水数
		sprintf(str,"\"overstockimagecount\":%s,\n",mTIPcam900State[i].overstockImageCount.c_str());strJson+=str;	//积压图片数
		sprintf(str,"\"cameranum\":%s,\n",mTIPcam900State[i].cameranum.c_str());strJson+=str;	//相机编号（101~299）
		sprintf(str,"\"lanenum\":%s,\n",mTIPcam900State[i].lanenum.c_str());strJson+=str;	//车道编号
		sprintf(str,"\"connectstatus\":%s,\n",mTIPcam900State[i].connectstatus.c_str());strJson+=str;	//连接状态
		sprintf(str,"\"workstatus\":%s,\n",mTIPcam900State[i].workstatus.c_str());strJson+=str;	//工作状态
		sprintf(str,"\"lightworkstatus\":%s,\n",mTIPcam900State[i].lightworkstatus.c_str());strJson+=str;	//补光灯的工作状态
		sprintf(str,"\"recognitionrate\":%s,\n",mTIPcam900State[i].recognitionrate.c_str());strJson+=str;	//识别成功率
		sprintf(str,"\"hardwareversion\":%s,\n",mTIPcam900State[i].hardwareversion.c_str());strJson+=str;//固件版本
		sprintf(str,"\"softwareversion\":%s,\n",mTIPcam900State[i].softwareversion.c_str());strJson+=str;	//软件版本
		sprintf(str,"\"runningtime\":%s,\n",mTIPcam900State[i].runningtime.c_str());strJson+=str;	//设备从开机到现在的运行时间（秒）
		sprintf(str,"\"brand\":%s,\n",mTIPcam900State[i].brand.c_str());strJson+=str;	//厂商型号
		sprintf(str,"\"devicetype\":%s,\n",mTIPcam900State[i].devicetype.c_str());strJson+=str;	//设备型号
		sprintf(str,"\"statuscode\":%s,\n",mTIPcam900State[i].statuscode.c_str());strJson+=str;	//状态码,详见附录A3 0-正常；其他由厂商自定义
		sprintf(str,"\"statusmsg\":%s\n",mTIPcam900State[i].statusmsg.c_str());strJson+=str;	//状态描述 由厂商自定义,最大长度256 例如：正常
			
		if(i==vehplate900cnt-1)
			strJson = strJson + "}\n";
		else
			strJson = strJson + "},\n";
	}
	strJson = strJson + "]\n";
	
	strJson +=	"}\n\n\0\0";

	mstrjson=strJson;
	return true;
}

bool jsonStrVMCtrlStateWriter(int messagetype, string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j; 
	
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

	if(stuVMCtl_State->Linked && GetTickCount()-stuVMCtl_State->TimeStamp>1*60*1000)//如果大于1分钟没更新，认为没连接
		init_lt_state_struct();
	
	strJson = strJson + "\"hostname\":\""+ stuVMCtl_State->strhostname +"\",\n";	//主机名称
	strJson = strJson + "\"cpurate\":\""+ stuVMCtl_State->strcpuRate +"\",\n";	//CPU占用率
	strJson = strJson + "\"cputemp\":\""+ stuVMCtl_State->strcpuTemp +"\",\n";	//CPU温度
	strJson = strJson + "\"mentotal\":\""+ stuVMCtl_State->strmenTotal +"\",\n";	//内存总数
	strJson = strJson + "\"menused\":\""+ stuVMCtl_State->strmenUsed +"\",\n";	//已使用内存
	strJson = strJson + "\"menrate\":\""+ stuVMCtl_State->strmenRate +"\"\n";	//内存使用率

	strJson +=	"}\n\0";
	
	mstrjson=strJson;
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

//19回路电压电流开关状态
bool jsonStrSwitchStatusWriter(int messagetype, string &mstrjson)
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

	docount=atoi(StrDoCount.c_str());
	for(i=0;i<docount;i++) //开关数量
	{
		va_meter_bd=i/VA_METER_BD_MAX_NUM;
		phase=i%VA_METER_BD_MAX_NUM;
		if (StrAdrrVAMeter[va_meter_bd].length() != 0)
		{
			if(stuVA_Meter_Param[va_meter_bd]->phase[phase].vln/100.0<24.0)
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

	mstrjson=strJson;
	return true;
}


//19回路电压电流开关状态-新粤
bool jsonStrSwitchStatusWriterXY(int messagetype, string &mstrjson)
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
			if(stuVA_Meter_Param[va_meter_bd]->phase[phase].vln/100.0<24.0)
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

	mstrjson=strJson;

	return true;
}


bool jsonStrHWCabinetWriter(int messagetype,char *pstPam, string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,status; 
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

    pthread_mutex_lock(&snmpoidMutex);
	if(HUAWEIDevValue.hwLinked && GetTickCount()-HUAWEIDevValue.hwTimeStamp>5*60*1000)//超过5分钟没更新，认为没有连接
	{
		initHUAWEIGantry();
		initHUAWEIALARM();
	}
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
    strJson = strJson + "\"hwair_cond_infan_alarm\": " + HUAWEIDevAlarm.hwair_cond_infan_alarm.c_str() + ",\n";	//空调内风机故障 245
    strJson = strJson + "\"hwair_cond_infan_alarm2\": " + HUAWEIDevAlarm.hwair_cond_infan_alarm2.c_str() + ",\n";	//空调内风机故障 245
    strJson = strJson + "\"hwair_cond_outfan_alarm\": " + HUAWEIDevAlarm.hwair_cond_outfan_alarm.c_str() + ",\n";	//空调外风机故障 246
    strJson = strJson + "\"hwair_cond_outfan_alarm2\": " + HUAWEIDevAlarm.hwair_cond_outfan_alarm2.c_str() + ",\n";	//空调外风机故障 246
    strJson = strJson + "\"hwair_cond_comp_alarm\": " + HUAWEIDevAlarm.hwair_cond_comp_alarm.c_str() + ",\n";	//空调压缩机故障 247
    strJson = strJson + "\"hwair_cond_comp_alarm2\": " + HUAWEIDevAlarm.hwair_cond_comp_alarm2.c_str() + ",\n";	//空调压缩机故障 247
    strJson = strJson + "\"hwair_cond_return_port_sensor_alarm\": " + HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm.c_str() + ",\n";	//空调回风口传感器故障 248
    strJson = strJson + "\"hwair_cond_return_port_sensor_alarm2\": " + HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm2.c_str() + ",\n";	//空调回风口传感器故障 248
    strJson = strJson + "\"hwair_cond_evap_freezing_alarm\": " + HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm.c_str() + ",\n";	//空调蒸发器冻结 249
    strJson = strJson + "\"hwair_cond_evap_freezing_alarm2\": " + HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm2.c_str() + ",\n";	//空调蒸发器冻结 249
    strJson = strJson + "\"hwair_cond_freq_high_press_alarm\": " + HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm.c_str() + ",\n";	//空调频繁高压力 250
    strJson = strJson + "\"hwair_cond_freq_high_press_alarm2\": " + HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm2.c_str() + ",\n";	//空调频繁高压力 250
    strJson = strJson + "\"hwair_cond_comm_fail_alarm\": " + HUAWEIDevAlarm.hwair_cond_comm_fail_alarm.c_str() + ",\n";	//空调通信失败告警 251
    strJson = strJson + "\"hwair_cond_comm_fail_alarm2\": " + HUAWEIDevAlarm.hwair_cond_comm_fail_alarm2.c_str() + ",\n";	//空调通信失败告警 251
	//防雷器告警
    strJson = strJson + "\"hwacspdalarmtraps\": " + HUAWEIDevAlarm.hwACSpdAlarmTraps.c_str() + ",\n";	//交流防雷器故障
    strJson = strJson + "\"hwdcspdalarmtraps\": " + HUAWEIDevAlarm.hwDCSpdAlarmTraps.c_str() + ",\n";	//直流防雷器故障
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
    strJson = strJson + "\"hwsetenvhumidityupperlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[0].c_str() + ",\n";	//环境湿度告警上限
    strJson = strJson + "\"hwsetenvhumidityupperlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[1].c_str() + ",\n";	//环境湿度告警上限
    strJson = strJson + "\"hwsetenvhumiditylowerlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[0].c_str() + ",\n";	//环境湿度告警下限
    strJson = strJson + "\"hwsetenvhumiditylowerlimit2\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[1].c_str() + ",\n";	//环境湿度告警下限
	//直流空调(新增加)
    strJson = strJson + "\"hwdcairruntime\": " + HUAWEIDevValue.strhwDcAirRunTime[0].c_str() + ",\n";	//设备柜空调运行时间
    strJson = strJson + "\"hwdcairruntime2\": " + HUAWEIDevValue.strhwDcAirRunTime[1].c_str() + ",\n";	//电池柜空调运行时间
    strJson = strJson + "\"hwcoolingdevicesmode\": " + HUAWEIDevValue.strhwCoolingDevicesMode.c_str() + ",\n";	//温控模式
	//供配电
	//电源告警
	sprintf(str,"\"hwacinputfailalarm\": %s,\n", HUAWEIDevAlarm.hwAcInputFailAlarm.c_str());strJson += str;		 //交流电源输入停电告警
	sprintf(str,"\"hwacinputl1failalarm\": %s,\n", HUAWEIDevAlarm.hwAcInputL1FailAlarm.c_str());strJson += str;		 //交流电源输入L1	相缺相告警
	sprintf(str,"\"hwacinputl2failalarm\": %s,\n", HUAWEIDevAlarm.hwAcInputL2FailAlarm.c_str());strJson += str;		 //交流电源输入L2	相缺相告警
	sprintf(str,"\"hwacinputl3failalarm\": %s,\n", HUAWEIDevAlarm.hwAcInputL3FailAlarm.c_str());strJson += str;		 //交流电源输入L3	相缺相告警
	sprintf(str,"\"hwdcvoltalarmtraps\": %s,\n", HUAWEIDevAlarm.hwDcVoltAlarmTraps.c_str());strJson += str;		 //直流电源输出告警
	sprintf(str,"\"hwloadlvdalarmtraps\": %s,\n", HUAWEIDevAlarm.hwLoadLvdAlarmTraps.c_str());strJson += str;		 //LLVD1下电告警
	//锂电池告警
	sprintf(str,"\"hwacbgroup_comm_fail_alarm\": %s,\n", HUAWEIDevAlarm.hwAcbGroup_comm_fail_alarm.c_str());strJson += str;		 //所有锂电通信失败
	sprintf(str,"\"hwacbgroup_discharge_alarm\": %s,\n", HUAWEIDevAlarm.hwAcbGroup_discharge_alarm.c_str());strJson += str;		 //电池放电告警
	sprintf(str,"\"hwacbgroup_charge_overcurrent_alarm\": %s,\n", HUAWEIDevAlarm.hwAcbGroup_charge_overcurrent_alarm.c_str());strJson += str;		 //电池充电过流
	sprintf(str,"\"hwacbgroup_temphigh_alarm\": %s,\n", HUAWEIDevAlarm.hwAcbGroup_temphigh_alarm.c_str());strJson += str;		 //电池温度高
	sprintf(str,"\"hwacbgroup_templow_alarm\": %s,\n", HUAWEIDevAlarm.hwAcbGroup_templow_alarm.c_str());strJson += str;		 //电池温度低
	sprintf(str,"\"hwacbgroup_poweroff_alarm\": %s,\n", HUAWEIDevAlarm.hwAcbGroup_poweroff_alarm.c_str());strJson += str;		 //电池下电
	sprintf(str,"\"hwacbgroup_fusebreak_alarm\": %s,\n", HUAWEIDevAlarm.hwAcbGroup_fusebreak_alarm.c_str());strJson += str;		 //电池熔丝断
	sprintf(str,"\"hwacbgroup_moduleloss_alarm\": %s,\n", HUAWEIDevAlarm.hwAcbGroup_moduleloss_alarm.c_str());strJson += str;		 //模块丢失
	//2019-08-20新增
	sprintf(str,"\"hwacbgroupbatrunningstate\": %s,\n", HUAWEIDevValue.strhwAcbGroupBatRunningState.c_str());strJson += str;		 //电池状态
	sprintf(str,"\"hwsmokesensorstatus\": %s,\n", HUAWEIDevAlarm.hwSmokeAlarmTraps.c_str());strJson += str;		 //烟雾传感器状态
	sprintf(str,"\"hwwatersensorstatus\": %s,\n", HUAWEIDevAlarm.hwWaterAlarmTraps.c_str());strJson += str;		 //水浸传感器状态
	sprintf(str,"\"hwdoorsensorstatus\": %s,\n", HUAWEIDevAlarm.hwDoorAlarmTraps.c_str());strJson += str;		 //（电池柜）门磁传感器状态
	sprintf(str,"\"hwdoorsensorstatus2\": %s,\n", HUAWEIDevAlarm.hwDoorAlarmTraps2.c_str());strJson += str;		 //（设备柜）门磁传感器状态
	sprintf(str,"\"hwdcairequipaddress\": %d,\n", hwAirAddrbuf[0]);strJson += str;		 //空调地址（设备柜）
	sprintf(str,"\"hwdcairequipaddress2\": %d,\n", hwAirAddrbuf[1]);strJson += str;		 //空调地址（电池柜）
	sprintf(str,"\"hwtemhumequipaddress\": %d,\n", hwTemAddrbuf[0]);strJson += str;		 //温湿度地址（设备柜）
	sprintf(str,"\"hwtemhumequipaddress2\": %d,\n", hwTemAddrbuf[1]);strJson += str;		 //空调地址（电池柜）
	if(HUAWEIDevValue.strhwAcbBatVolt!="2147483647")
		{sprintf(str,"\"hwacbbatvolt\": %.1f,\n", atoi(HUAWEIDevValue.strhwAcbBatVolt.c_str())/10.0);strJson += str;}		 //单个电池电压
	else	
		{sprintf(str,"\"hwacbbatvolt\": %d,\n", HUAWEIDevValue.strhwAcbBatVolt.c_str());strJson += str;}		 //单个电池电压
	sprintf(str,"\"hwacbbatcurr\": %s,\n", HUAWEIDevValue.strhwAcbBatCurr.c_str());strJson += str;		 //单个电池电流
	sprintf(str,"\"hwacbbatsoh\": %s,\n", HUAWEIDevValue.strhwAcbBatSoh.c_str());strJson += str;		 //单个电池串SOH
	sprintf(str,"\"hwacbbatcapacity\": %s,\n", HUAWEIDevValue.strhwAcbBatCapacity.c_str());strJson += str;		 //单个电池容量
	//2019-11-19新增4个门锁状态
	status=LockerStatusGet(0);	sprintf(str,"\"hwbatcabfrontdoorstatus\": \"%d\",\n", status);strJson += str;		 //电池柜前门锁状态
	status=LockerStatusGet(1);	sprintf(str,"\"hwbatcabbackdoorstatus\": \"%d\",\n", status);strJson += str;		 //电池柜后门锁状态
	status=LockerStatusGet(2);	sprintf(str,"\"hwequcabfrontdoorstatus\": \"%d\",\n", status);strJson += str;		 //设备柜前门锁状态
	status=LockerStatusGet(3);	sprintf(str,"\"hwequcabbackdoorstatus\": \"%d\",\n", status);strJson += str;		 //设备柜后门锁状态
    //CABINETTYPE  1：华为（包括华为单门 双门等） 5：中兴; 6：金晟安; 7：爱特斯 StrVersionNo
	//2019-12-13新增飞达中兴机柜状态
	#if(CABINETTYPE==5)//飞达中兴
		sprintf(str,"\"rectifiermodulevol\": %s,\n", HUAWEIDevValue.RectifierModuleVol.c_str());strJson += str;		 //整流器输出电压
		sprintf(str,"\"rectifiermodulecurr\": %s,\n", HUAWEIDevValue.RectifierModuleCurr.c_str());strJson += str;		 //整流器输出电流
		sprintf(str,"\"rectifiermoduletemp\": %s,\n", HUAWEIDevValue.RectifierModuleTemp.c_str());strJson += str;		 //整流器机内温度
	    //空调
		sprintf(str,"\"in_fanstate\": %s,\n", HUAWEIDevValue.StrIn_FanState.c_str());strJson += str;		 //内风机状态 0代表关闭 1代表开启
		sprintf(str,"\"out_fanstate\": %s,\n", HUAWEIDevValue.StrOut_FanState.c_str());strJson += str;		 //外风机状态 0代表关闭 1代表开启
	#elif(CABINETTYPE==6)//金晟安
		sprintf(str,"\"upscityvol\": %s,\n", HUAWEIDevValue.StrUpsCityVol.c_str());strJson += str;		 //
		sprintf(str,"\"upsovol\": %s,\n", HUAWEIDevValue.StrUpsOVol.c_str());strJson += str;		 //
		sprintf(str,"\"upstemp\": %s,\n", HUAWEIDevValue.StrUpsTemp.c_str());strJson += str;		 //
	    //空调
		sprintf(str,"\"in_fanstate\": %s,\n", HUAWEIDevValue.StrIn_FanState.c_str());strJson += str;		 //内风机状态 0代表关闭 1代表开启
		sprintf(str,"\"out_fanstate\": %s,\n", HUAWEIDevValue.StrOut_FanState.c_str());strJson += str;		 //外风机状态 0代表关闭 1代表开启
	#elif(CABINETTYPE==7)//爱特斯
		sprintf(str,"\"upscityvol\": %s,\n", HUAWEIDevValue.StrUpsCityVol.c_str());strJson += str;		 //
		sprintf(str,"\"upsovol\": %s,\n", HUAWEIDevValue.StrUpsOVol.c_str());strJson += str;		 //
		sprintf(str,"\"upstemp\": %s,\n", HUAWEIDevValue.StrUpsTemp.c_str());strJson += str;		 //
		//空调
		sprintf(str,"\coolerstate\": %s,\n", HUAWEIDevValue.StrCoolerState.c_str());strJson += str;		 //制冷器状态
		sprintf(str,"\"in_fanstate\": %s,\n", HUAWEIDevValue.StrIn_FanState.c_str());strJson += str;		 //内风机状态 0代表关闭 1代表开启
		sprintf(str,"\"out_fanstate\": %s,\n", HUAWEIDevValue.StrOut_FanState.c_str());strJson += str;		 //外风机状态 0代表关闭 1代表开启
		sprintf(str,"\"heaterstate\": %s,\n", HUAWEIDevValue.StrHeaterState.c_str());strJson += str;		 //加热器状态
	#endif
	
    pthread_mutex_unlock(&snmpoidMutex);
	strJson = strJson + "\"opttime\": \"" + sDateTime + "\"\n";	//时间
    strJson +=  "}\n\0";
	
	mstrjson=strJson;
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
		if(StrIPSwitchType == "1" || StrIPSwitchType == "")
			{sprintf(str,"\"factoryname\":\"华为\",\n"); strJson = strJson + str;}//生产商
//		else if(StrIPSwitchType == "2")
//			{sprintf(str,"\"factoryname\":\"迪普\",\n"); strJson = strJson + str;}//生产商
		if(i==0)
		{
			if(HUAWEIDevValue.hwswitchEntityLinked && GetTickCount()-HUAWEIDevValue.hwswitchEntityTimeStamp>5*60*1000) //5分钟没有更新数据，恢复默认
				initHUAWEIswitchEntity();
			sprintf(str,"\"devicemodel\":\"%s\",\n",HUAWEIDevValue.strhwswitchEntityDevModel.c_str()); strJson = strJson + str;//设备型号
			sprintf(str,"\"cpuusage\":\"%s\",\n",HUAWEIDevValue.strhwswitchEntityCpuUsage.c_str()); strJson = strJson + str;//CPU使用率
			sprintf(str,"\"memusage\":\"%s\",\n",HUAWEIDevValue.strhwswitchEntityMemUsage.c_str()); strJson = strJson + str;//内存使用率
			sprintf(str,"\"temperature\":\"%s\",\n",HUAWEIDevValue.strhwswitchEntityTemperature.c_str()); strJson = strJson + str;//温度
			strJson = strJson + strswitchjson;								//交换机网络数据
		}
		else if(i==1)
		{
			if(HUAWEIDevValue.hwswitchEntityLinked1 && GetTickCount()-HUAWEIDevValue.hwswitchEntityTimeStamp1>5*60*1000) //5分钟没有更新数据，恢复默认
				initHUAWEIswitchEntity1();
			sprintf(str,"\"devicemodel\":\"%s\",\n",HUAWEIDevValue.strhwswitchEntityDevModel1.c_str()); strJson = strJson + str;//设备型号
			sprintf(str,"\"cpuusage\":\"%s\",\n",HUAWEIDevValue.strhwswitchEntityCpuUsage1.c_str()); strJson = strJson + str;//CPU使用率
			sprintf(str,"\"memusage\":\"%s\",\n",HUAWEIDevValue.strhwswitchEntityMemUsage1.c_str()); strJson = strJson + str;//内存使用率
			sprintf(str,"\"temperature\":\"%s\",\n",HUAWEIDevValue.strhwswitchEntityTemperature1.c_str()); strJson = strJson + str;//温度
			strJson = strJson + strswitchjson1;								//交换机网络数据
		}
		strJson +=	"}\n";
		if(i!=ipswitchcnt-1)
			strJson = strJson + ",";								
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
		if(StrFireWareType == "1" || StrFireWareType == "")
			{sprintf(str,"\"factoryname\":\"华为\",\n"); strJson = strJson + str;}//生产商
		else if(StrFireWareType == "2")
			{sprintf(str,"\"factoryname\":\"迪普\",\n"); strJson = strJson + str;}//生产商
		if(i==0)
		{
			if(HUAWEIDevValue.hwEntityLinked && GetTickCount()-HUAWEIDevValue.hwEntityTimeStamp>5*60*1000) //5分钟没有更新数据，恢复默认
				initHUAWEIEntity();
			sprintf(str,"\"devicemodel\":\"%s\",\n",HUAWEIDevValue.strhwEntityDevModel.c_str()); strJson = strJson + str;//设备型号
			sprintf(str,"\"cpuusage\":\"%s\",\n",HUAWEIDevValue.strhwEntityCpuUsage.c_str()); strJson = strJson + str;//CPU使用率
			sprintf(str,"\"memusage\":\"%s\",\n",HUAWEIDevValue.strhwEntityMemUsage.c_str()); strJson = strJson + str;//内存使用率
			sprintf(str,"\"temperature\":\"%s\",\n",HUAWEIDevValue.strhwEntityTemperature.c_str()); strJson = strJson + str;//温度
			strJson = strJson + strfirewalljson;								//网络数据
		}
		else if(i==1)
		{
			if(HUAWEIDevValue.hwEntityLinked1 && GetTickCount()-HUAWEIDevValue.hwEntityTimeStamp1>5*60*1000) //5分钟没有更新数据，恢复默认
				initHUAWEIEntity1();
			sprintf(str,"\"devicemodel\":\"%s\",\n",HUAWEIDevValue.strhwEntityDevModel1.c_str()); strJson = strJson + str;//设备型号
			sprintf(str,"\"cpuusage\":\"%s\",\n",HUAWEIDevValue.strhwEntityCpuUsage1.c_str()); strJson = strJson + str;//CPU使用率
			sprintf(str,"\"memusage\":\"%s\",\n",HUAWEIDevValue.strhwEntityMemUsage1.c_str()); strJson = strJson + str;//内存使用率
			sprintf(str,"\"temperature\":\"%s\",\n",HUAWEIDevValue.strhwEntityTemperature1.c_str()); strJson = strJson + str;//温度
			strJson = strJson + strfirewalljson1;								//网络数据
		}
		strJson +=	"}\n";
		if(i!=firewarecnt-1)
			strJson = strJson + ",";								
	}
	strJson +=	"]\n";
	strJson +=	"}\n";
	
	pthread_mutex_unlock(&snmpoidMutex);

	mstrjson = strJson;
	//*len=strJson.length();
	//memcpy(json,(char*)strJson.c_str(),*len);
	
}


void SetjsonAtlasStatusStr(int messagetype,string &mstrjson)
{
	char str[100],sDateTime[30];
	int i,j,atlascnt; 
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
	strJson = strJson + "\"atlascnt\": \"" + StrAtlasCount + "\",\n";	//Atlas数量
	strJson = strJson + "\"atlaslist\": [\n";	//Atlas列表
	atlascnt=atoi(StrAtlasCount.c_str()); 
	for(i=0;i<atlascnt;i++)
	{
		if(stuAtlasState[i].Linked && GetTickCount()-stuAtlasState[i].TimeStamp>5*60*1000) //5分钟不更新，恢复默认
			init_atlas_struct(i);
	}	
	for(i=0;i<atlascnt;i++)
	{
		if(i==0 && stuAtlasState[i].stratlasdata!="")
		{
			strJson = strJson + stuAtlasState[i].stratlasdata;	
			if(i<atlascnt-1 && stuAtlasState[1].stratlasdata!="")
				strJson = strJson + ",";	
		}
		else if(i==1 && stuAtlasState[i].stratlasdata!="")
			strJson = strJson + stuAtlasState[i].stratlasdata;		
	}
	strJson +=	"]\n";
	strJson +=	"}\n";
	
	pthread_mutex_unlock(&snmpoidMutex);

	mstrjson = strJson;
	
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


void SetjsonSpdAIStatusStr(int messagetype,string &mstrjson)
{
	char str[100],sDateTime[30];
	int spdcnt;
	
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
	strJson = strJson + "\"spdcnt\": \"" + StrSPDCount + "\",\n";	//防雷器数量
	strJson = strJson + "\"spdlist\": [\n";	//防雷器列表
	spdcnt=atoi(StrSPDCount.c_str());
	for(int i=0;i<spdcnt;i++)
	{
		strJson +=	"{\n";
		sprintf(str,"\"spdid\":\"%d\",\n",i+1); strJson = strJson + str;//编号
		sprintf(str,"\"name\":\"spd%d\",\n",i+1); strJson = strJson + str;//名称
		sprintf(str,"\"ip\":\"%s\",\n",StrSPDIP[i].c_str()); strJson = strJson + str;//ip地址
		if(StrSPDType=="1")
			strJson = strJson + "\"spdfactory\":\"雷迅\",\n"; //防雷器厂商
		else if(StrSPDType=="2")
			strJson = strJson + "\"spdfactory\":\"华咨圣泰\",\n";	//防雷器厂商
		else
			strJson = strJson + "\"spdfactory\":\"\",\n";	//防雷器厂商
		sprintf(str,"\"leak_current\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].leak_current);strJson += str;//实时漏电流
		sprintf(str,"\"a_leak_current\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].leak_A);strJson += str;//A相漏电流
		sprintf(str,"\"b_leak_current\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].leak_B);strJson += str;//B相漏电流
		sprintf(str,"\"c_leak_current\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].leak_C);strJson += str;//C相漏电流
		sprintf(str,"\"ref_volt\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].ref_volt);strJson += str;		//基准电压
		sprintf(str,"\"real_volt\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].real_volt);strJson += str;		//实时电压
		sprintf(str,"\"a_volt\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].volt_A);strJson += str;//A相电压
		sprintf(str,"\"b_volt\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].volt_B);strJson += str;//B相电压
		sprintf(str,"\"c_volt\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].volt_C);strJson += str;//C相电压
		sprintf(str,"\"spd_temp\": \"%.1f\",\n", stuSpd_Param->rSPD_data[i].spd_temp);strJson += str;		// 防雷器温度
		sprintf(str,"\"envi_temp\": \"%.1f\",\n", stuSpd_Param->rSPD_data[i].envi_temp);strJson += str;		//环境温度
		sprintf(str,"\"addr\": \"%d\",\n", stuSpd_Param->rSPD_data[i].id);strJson += str;		//设备地址
		sprintf(str,"\"struck_cnt\": \"%.0f\",\n", stuSpd_Param->rSPD_data[i].struck_cnt);strJson += str;		//雷击计数
		sprintf(str,"\"struck_total\": \"%.0f\",\n", stuSpd_Param->rSPD_data[i].struck_total);strJson += str;		//合计雷击计数
		sprintf(str,"\"soft_version\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].soft_version);strJson += str;		// 软件版本号
		sprintf(str,"\"leak_alarm_threshold\": \"%.3f\",\n", stuSpd_Param->rSPD_data[i].leak_alarm_threshold);strJson += str;		// 报警阈值
		sprintf(str,"\"day_time\": \"%.0f\",\n", stuSpd_Param->rSPD_data[i].day_time);strJson += str;		//在线天数
		sprintf(str,"\"systime\": \"%d-%d-%d %d:%d:%d\",\n", stuSpd_Param->rSPD_data[i].systime_year,stuSpd_Param->rSPD_data[i].systime_month,
			 stuSpd_Param->rSPD_data[i].systime_day,stuSpd_Param->rSPD_data[i].systime_hour,stuSpd_Param->rSPD_data[i].systime_min,
			 stuSpd_Param->rSPD_data[i].systime_sec);
		strJson += str;		//系统时间
		
		sprintf(str,"\"life_time\": \"%.2f\",\n", stuSpd_Param->rSPD_data[i].life_time);strJson += str;		// 防雷器寿命值
		sprintf(str,"\"remotestatusalarm\": \"%d\",\n", stuSpd_Param->rSPD_data[i].DI_C1_status);strJson += str;		//防雷器脱扣状态报警
		sprintf(str,"\"linegroundstatusalarm\": \"%d\",\n", stuSpd_Param->rSPD_data[i].DI_grd_alarm);strJson += str;	//线路&接地状态告警
		sprintf(str,"\"eakcuralarm\": \"%d\",\n", stuSpd_Param->rSPD_data[i].DI_leak_alarm);strJson += str;		//漏电流告警
		sprintf(str,"\"voltalarm\": \"%d\",\n", stuSpd_Param->rSPD_data[i].DI_volt_alarm);strJson += str;		//市电电压告警
		
		int struckcount=5;
		sprintf(str,"\"struckcount\": \"%d\",\n",5);strJson += str; 	//雷击列表个数
		strJson = strJson + "\"strucklist\": [\n"; //雷击列表
		for(int j=0;j<struckcount;j++)
		{
			strJson +=	"{\n";
			sprintf(str,"\"struckid\":\"%d\",\n",j+1); strJson = strJson + str;//编号
			sprintf(str,"\"name\":\"last_%d\",\n",j+1); strJson = strJson + str;//雷击发生最近第j+1次
			if(j==0)
				sprintf(str,"\"strucktime\": \"%d-%d-%d %d:%d\"\n", stuSpd_Param->rSPD_data[i].last_1_struck_year,stuSpd_Param->rSPD_data[i].last_1_struck_month,
				 stuSpd_Param->rSPD_data[i].last_1_struck_day,stuSpd_Param->rSPD_data[i].last_1_struck_hour,stuSpd_Param->rSPD_data[i].last_1_struck_min);
			else if(j==1)
				sprintf(str,"\"strucktime\": \"%d-%d-%d %d:%d\"\n", stuSpd_Param->rSPD_data[i].last_2_struck_year,stuSpd_Param->rSPD_data[i].last_2_struck_month,
				 stuSpd_Param->rSPD_data[i].last_2_struck_day,stuSpd_Param->rSPD_data[i].last_2_struck_hour,stuSpd_Param->rSPD_data[i].last_2_struck_min);
			else if(j==2)
				sprintf(str,"\"strucktime\": \"%d-%d-%d %d:%d\"\n", stuSpd_Param->rSPD_data[i].last_3_struck_year,stuSpd_Param->rSPD_data[i].last_3_struck_month,
				 stuSpd_Param->rSPD_data[i].last_3_struck_day,stuSpd_Param->rSPD_data[i].last_3_struck_hour,stuSpd_Param->rSPD_data[i].last_3_struck_min);
			else if(j==3)
				sprintf(str,"\"strucktime\": \"%d-%d-%d %d:%d\"\n", stuSpd_Param->rSPD_data[i].last_4_struck_year,stuSpd_Param->rSPD_data[i].last_4_struck_month,
				 stuSpd_Param->rSPD_data[i].last_4_struck_day,stuSpd_Param->rSPD_data[i].last_4_struck_hour,stuSpd_Param->rSPD_data[i].last_4_struck_min);
			else if(j==4)
				sprintf(str,"\"strucktime\": \"%d-%d-%d %d:%d\"\n", stuSpd_Param->rSPD_data[i].last_4_struck_year,stuSpd_Param->rSPD_data[i].last_4_struck_month,
				 stuSpd_Param->rSPD_data[i].last_4_struck_day,stuSpd_Param->rSPD_data[i].last_4_struck_hour,stuSpd_Param->rSPD_data[i].last_4_struck_min);
			strJson += str; 	//雷击发生时间
			
			if(j==struckcount-1)
				strJson = strJson + "}\n";	
			else
				strJson = strJson + "},\n";	
		}
		strJson +=	"]\n";
		if(i==spdcnt-1)
			strJson = strJson + "}\n";	
		else
			strJson = strJson + "},\n"; 
	}
	strJson +=	"]\n";
	strJson +=	"}\n";

	mstrjson = strJson;
}


//28 接地电阻参数
void SetjsonSpdResStatusStr(int messagetype,string &mstrjson)
{
	char str[100],sDateTime[30];
	int spdcnt;
	
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
	
	sprintf(str,"\"alarm\": \"%d\",\n", stuSpd_Param->rSPD_res.alarm);strJson += str;		//接地报警 0x0c
	sprintf(str,"\"spdresip\": \"%s\",\n", StrSPDIP[SPD_NUM].c_str());strJson += str;		// 接地检测器地址
	sprintf(str,"\"spdresport\": \"%s\",\n", StrSPDPort[SPD_NUM].c_str());strJson += str;		// 接地检测器端口
	sprintf(str,"\"grd_res\": \"%.3f\",\n", stuSpd_Param->rSPD_res.grd_res_real);strJson += str;		//接地电阻值
	sprintf(str,"\"grd_volt\": \"%d\",\n", stuSpd_Param->rSPD_res.grd_volt);strJson += str;		// 电压值 0x0f
	sprintf(str,"\"spdresid\": \"%d\",\n", StrSPDAddr[SPD_NUM].c_str());strJson += str;		// 更改id地址
	sprintf(str,"\"spdres_alarm_value\": \"%d\"\n", stuSpd_Param->rSPD_res.alarm_value);strJson += str;		// 报警值修改
	strJson +=	"}\n";

	mstrjson = strJson;
}


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
//	strJson = strJson + "\"dcairctrlmode\": " + HUAWEIDevValue.strhwDcAirCtrlMode[0].c_str() + ",\n"; //空调控制模式 230
//	strJson = strJson + "\"dcairctrlmode2\": " + HUAWEIDevValue.strhwDcAirCtrlMode[1].c_str() + ",\n";	//空调控制模式 230
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

void SetjsonDealLockerStr(int messagetype,UINT32 cardid,UINT16 lockaddr,string &mstrjson)
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

	sprintf(str,"\"cardid\":\"%llu\",\n",cardid); strJson = strJson + str;	//ID卡号
	sprintf(str,"\"operate\":%d\n",ACT_UNLOCK); strJson = strJson + str;	//操作请求
	strJson +=	"}\n\0";

    mstrjson = strJson;
}

// 64位卡号发送函数
void SetjsonDealLockerStr64(int messagetype,UINT64 cardid,UINT16 lockaddr,string &mstrjson)
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

	sprintf(str,"\"cardid\":\"%llu\",\n",cardid); strJson = strJson + str;	//ID卡号
	sprintf(str,"\"operate\":%d\n",ACT_UNLOCK); strJson = strJson + str;	//操作请求
	strJson +=	"}\n\0";

    mstrjson = strJson;
}


bool jsonstrSPDReader(char* jsonstr, int len, UINT8 *pstuRCtrl)
{
//	printf("%s \t\n",jsonstr);
	char key[50],value[128],keytmp[50];
	int valueint,arraysize;

	cJSON *json=0, *jsonkey=0, *jsonvalue=0, *jsonlist=0, *jsonitem=0;
	int i;
	//解析数据包
	json = cJSON_Parse(jsonstr);
	if(json==0) return false;

	REMOTE_CONTROL *pRCtrl=(REMOTE_CONTROL *)pstuRCtrl;
	THUAWEIGantry *hwDev=&HUAWEIDevValue;	//华为机柜状态
	SPD_PARAMS *spdDev=stuSpd_Param;		//防雷器结构体
	
	memset(pRCtrl,ACT_HOLD,sizeof(REMOTE_CONTROL));
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
	
	//SPD 列表
    jsonlist = cJSON_GetObjectItem(json, "spdlist");
    if(jsonlist!=0)
    {
        arraysize=cJSON_GetArraySize(jsonlist);
        for(i=0;i<arraysize;i++)
        {
            jsonitem=cJSON_GetArrayItem(jsonlist,i);
            if(jsonitem != NULL)
            {
            	//雷击计数清零
				sprintf(key,"clearcounter");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					pRCtrl->DO_spdcnt_clear[i]=atoi(value); 
                }
            	//总雷击计数清0
				sprintf(key,"cleartotalcounter");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					pRCtrl->DO_totalspdcnt_clear[i]=atoi(value); 
                }
            	//雷击时间清0
				sprintf(key,"strucktimerecclear");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					pRCtrl->DO_psdtime_clear[i]=atoi(value); 
                }
            	//在线时间清0
				sprintf(key,"onlinetimeclear");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					pRCtrl->DO_daytime_clear[i]=atoi(value); 
                }
            	//漏电流报警阈值
				sprintf(key,"leak_alarm_threshold");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					pRCtrl->spdleak_alarm_threshold[i]=atof(value); 
                }
            	//外接漏电流控制
				sprintf(key,"extleakcurrctrl");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					pRCtrl->DO_leak_type[i]=atoi(value); 
                }
            	//防雷器设备地址
				sprintf(key,"modbus_addr");
                jsonkey=cJSON_GetObjectItem(jsonitem,key);
                if(jsonkey != NULL)
                {
					sprintf(value,"%s",jsonkey->valuestring);
					printf("%s %s\n",key,value);
					pRCtrl->spd_modbus_addr[i]=atoi(value); 
                }
            }
        }
    }
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
