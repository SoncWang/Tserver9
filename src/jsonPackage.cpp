#include "jsonPackage.h"
#include "../IpcamServer.h"

extern ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
extern UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体
extern DEVICE_PARAMS *stuDev_Param[6];	//装置参数寄存器
extern VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体
extern VA_METER_PARAMS stuVA_Meter_Param[VA_METER_BD_NUM];		//伏安表电压电流结构体
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

extern string StrHWServer;		//华为服务器地址
extern string StrHWGetPasswd;	//SNMP GET 密码
extern string StrHWSetPasswd;	//SNMP SET 密码
extern string StrServerURL1;	//服务端URL1
extern string StrServerURL2;	//服务端URL2
extern string StrServerURL3;	//服务端URL3
extern string StrStationURL;	//虚拟站端URL
extern string StrRSUCount;	//RSU数量
extern string StrRSUIP[RSUCTL_NUM];	//RSUIP地址
extern string StrRSUPort[RSUCTL_NUM];	//RSU端口
extern string StrVehPlateCount;	//识别仪数量
extern string StrVehPlateIP[VEHPLATE_NUM];	//识别仪IP地址
extern string StrVehPlatePort[VEHPLATE_NUM];	//识别仪端口
extern string StrVehPlateKey[VEHPLATE_NUM];	//识别仪用户名密码
extern string StrCAMIP;	//监控摄像头IP地址
extern string StrCAMPort;	//监控摄像头端口
extern string StrCAMKey;	//监控摄像头用户名密码

extern string StrFireWareIP;         //防火墙IP
extern string StrFireWareGetPasswd;  //防火墙get密码
extern string StrFireWareSetPasswd;  //防火墙set密码
extern string StrSwitchIP ;//交换机IP
extern string StrSwitchGetPasswd ;//交换机get密码
extern string StrSwitchSetPasswd ;//交换机set密码

extern string StrAdrrVAMeter[VA_METER_BD_MAX_NUM];	//电压电流传感器1的地址
extern string StrAdrrPower[POWER_BD_MAX_NUM];	//电源板1的地址
extern string StrDoSeq[SWITCH_COUNT];	//do和设备映射的配置
extern UINT16 DoSeq[SWITCH_COUNT];	// 另外定义一个专门用来存储映射的数组,stuRemote_Ctrl会被清0

extern pthread_mutex_t snmpoidMutex ;

extern void GetConfig(VMCONTROL_PARAM *vmctrl_param);
extern void RemoteControl(UINT8* pRCtrl);

extern TIPcamState mTIPcamState[VEHPLATE_NUM];

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
	
	char key[128],value[128];
	
	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());
		sprintf(value,"%s",it->second.c_str());
		printf("%s %s\n",key,value);
		
		if(it->first=="messagetype")	messagetype=atoi(value);
		it++;
	}
	switch (messagetype)
	{
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
			GetConfig(&vmctrl_param);
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrVMCtlParamWriter(messagetype,(char*)&vmctrl_param,jsonstrout,lenout);
			break;
		case NETCMD_SEND_AIR_PARAM: 			//13 空调参数 
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrAirCondWriter(messagetype,(char*)stuAirCondRead,jsonstrout,lenout);
			break;
		case NETCMD_SEND_RSU_PARAM: 			//14 RSU天线参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrRsuWriter(messagetype,jsonstrout,lenout);
			break;
		case NETCMD_SEND_CAM_PARAM: 			//15 车牌识别参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrVehPlateWriter(messagetype,jsonstrout,lenout);
			break;
		case NETCMD_SEND_SWITCH_INFO: 			//16 交换机状态
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			SetjsonReceiveOKStr(messagetype,jsonstrout,lenout);
			break;
		case NETCMD_FLAGRUNSTATUS:			//17 门架运行状态
            {
              memset(jsonstrout,0,JSON_LEN);
              //*lenout=0;
              string mstrdata;
              SetjsonFlagRunStatusStr(messagetype,mstrdata);
              *lenout = mstrdata.size();
              memcpy(jsonstrout,mstrdata.c_str(),mstrdata.size());
              //SetjsonFlagRunStatusStr(messagetype,jsonstrout,lenout);
            }
			break;
		case NETCMD_REMOTE_CONTROL: 			//18 遥控设备
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
			jsonStrSwitchStatusWriter(messagetype,jsonstrout,lenout);
			break;
		case NETCMD_HWCABINET_STATUS:			//20  华为机柜状态
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrHWCabinetWriter(messagetype,(char*)&HUAWEIDevValue,jsonstrout,lenout);
			break;
		default:
			break;
	
	}
	return true;
}


bool jsonComputerReader(char* jsonstr, int len)
{
	printf("%s \t\n",jsonstr);
	
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
		printf("%s %d\n",key,value);
		
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
	printf("\n");
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
    printf("%s \r\n",jsonstr);


  Json::Reader reader;

  Json::Value json_object;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  string strdevicemodel = json_object["devicemodel"].toStyledString() ;
  if(strdevicemodel != "")
  {
    printf("devicemodel:%s\r\n",strdevicemodel.c_str()) ;
    mTIPcamState[mIndex].devicemodel = strdevicemodel ;
  }

  string strerrcode = json_object["errcode"].toStyledString() ;
  if(strerrcode != "")
  {
     printf("devicemodel:%s\r\n",strerrcode.c_str()) ;
     mTIPcamState[mIndex].errcode = strerrcode ;
  }

  string strfactoryid = json_object["factoryid"].toStyledString() ;
  if(strfactoryid != "")
  {
     printf("factoryid:%s\r\n",strfactoryid.c_str()) ;
     mTIPcamState[mIndex].factoryid = strfactoryid ;
  }


  string strfilllight = json_object["filllight"].toStyledString() ;
  if(strfilllight != "")
  {
     printf("filllight:%s\r\n",strfilllight.c_str()) ;
     mTIPcamState[mIndex].filllight = strfilllight ;
  }

  string strip = json_object["ip"].toStyledString() ;
  if(strip != "")
  {
     printf("ip:%s\r\n",strip.c_str()) ;
     mTIPcamState[mIndex].ip = strip ;
  }

  string strsoftversion = json_object["softversion"].toStyledString() ;
  if(strsoftversion != "")
  {
     printf("softversion:%s\r\n",strsoftversion.c_str()) ;
     mTIPcamState[mIndex].softversion = strsoftversion ;
  }


  string strstatuscode = json_object["statuscode"].toStyledString() ;
  if(strstatuscode != "")
  {
      printf("statuscode:%s\r\n",strstatuscode.c_str()) ;
      mTIPcamState[mIndex].statuscode = strstatuscode ;
  }

  string strstatustime = json_object["statustime"].toStyledString() ;
  if(strstatustime != "")
  {
      printf("statustime:%s\r\n",strstatustime.c_str()) ;
      mTIPcamState[mIndex].statustime = strstatustime ;
  }


  string strtemperature = json_object["temperature"].toStyledString() ;
  if(strtemperature != "")
  {
      printf("temperature:%s\r\n",strtemperature.c_str()) ;
      mTIPcamState[mIndex].temperature = strtemperature ;
  }

  return true;
}

bool jsonIPCamReader(char* jsonstr, int len,int mIndex)
{
    printf("%s \r\n",jsonstr);


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
  if(strentity != "")
  {
     printf("entity:%s\r\n",strentity.c_str()) ;
     jsonIPCamReaderNext((char *)(strentity.c_str()),strentity.size(),mIndex);
  }

  //std::cout << json_object["age"] << std::endl;

return true ;

}





bool jsonstrRCtrlReader(char* jsonstr, int len, UINT8 *pstuRCtrl)
{
	printf("%s \t\n",jsonstr);
	
	std::string json = jsonstr;
	std::map<std::string, std::string> out;
	jsonReader(json, out);
	
	REMOTE_CONTROL *pRCtrl=(REMOTE_CONTROL *)pstuRCtrl;
	char key[50];
	int value;
	
	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());value=atoi(it->second.c_str()) ;
		printf("%s %d\n",key,value);
		
		if(it->first=="rsu1")	pRCtrl->rsu1=value;		//1500 RSU天线1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="door_do")	pRCtrl->door_do=value;	//1501 电子门锁 0xFF00: 关锁;0xFF01: 开锁
		else if(it->first=="autoreclosure")	pRCtrl->autoreclosure=value;	//1502 自动重合闸0xFF00: 遥合;0xFF01: 遥分

		else if(it->first=="vehplate1")	pRCtrl->vehplate[0]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate2")	pRCtrl->vehplate[1]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate3")	pRCtrl->vehplate[2]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate4")	pRCtrl->vehplate[3]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate5")	pRCtrl->vehplate[4]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate6")	pRCtrl->vehplate[5]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate7")	pRCtrl->vehplate[6]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate8")	pRCtrl->vehplate[7]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate9")	pRCtrl->vehplate[8]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate10")	pRCtrl->vehplate[9]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate11")	pRCtrl->vehplate[10]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		else if(it->first=="vehplate12")	pRCtrl->vehplate[11]=value;			//车牌识别1 0xFF00: 遥合;0xFF01: 遥分
		
		else if(it->first=="sysreset")	pRCtrl->SysReset=value;			//系统重启
		else if(it->first=="frontdoorctrl")	pRCtrl->FrontDoorCtrl=value;			//前门电子门锁 0：保持 1：关锁：2：开锁
		else if(it->first=="backdoorctrl")	pRCtrl->BackDoorCtrl=value;			//后门电子门锁 0：保持 1：关锁：2：开锁
		else if(it->first=="sidedoorcCtrl")	pRCtrl->SideDoorCtrl=value;			//侧门电子门锁 0：保持 1：关锁：2：开锁
		it++;
	}
	printf("\n");
	return true;
}

bool jsonstrAirCondReader(char* jsonstr, int len, UINT8 *pstPam)
{
	printf("%s \t\n",jsonstr);
	
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
		printf("%s %d\n",key,value);
		
		if(it->first=="aircondset")	pRCtrl->aircondset=value;		//空调关机//1220   		1
		else if(it->first=="aircoldstartpoint")	pRCtrl->aircoldstartpoint=value;	//空调制冷点//1221 			50
		else if(it->first=="aircoldloop")	pRCtrl->aircoldloop=value;	//空调制冷回差//1222					10
		else if(it->first=="airhotstartpoint")	pRCtrl->airhotstartpoint=value;			//空调制热点//1223 			15
		else if(it->first=="airhotloop")	pRCtrl->airhotloop=value;			//空调制热回差//1224					10
		it++;
	}
	printf("\n");
	return true;
}

bool jsonstrVmCtlParamReader(char* jsonstr, int len, UINT8 *pstPam, UINT8 *pstIPPam)
{
	printf("%s \t\n",jsonstr);
	int i;
	
	std::string json = jsonstr;
	std::map<std::string, std::string> out;
	jsonReader(json, out);
	
	VMCONTROL_PARAM *pRCtrl=(VMCONTROL_PARAM *)pstPam;
	IPInfo *pIPInfo=(IPInfo *)pstIPPam;
	char key[50],value[128];
//	int value;
	
	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());
		sprintf(value,"%s",it->second.c_str());
		printf("%s %s\n",key,value);
		
		//门架信息
		if(it->first=="cabinettype")	StrCabinetType=value;		//机柜类型	1:华为；2:利通
		else if(it->first=="flagnetroadid")	StrFlagNetRoadID=value;	//ETC 门架路网编号
		else if(it->first=="flagroadid ")	StrFlagRoadID=value;	//ETC 门架路段编号
		else if(it->first=="flagid")	StrFlagID=value;			//ETC 门架编号
		else if(it->first=="posid ")	StrPosId=value;				//ETC 门架序号
		else if(it->first=="direction ")	StrDirection=value;		//行车方向
		else if(it->first=="dirdescription ")	StrDirDescription=value;	//行车方向说明
		//参数设置
		else if(it->first=="ipaddr")	StrIP=value;	//IP地址
		else if(it->first=="mask")	StrMask=value;	//子网掩码
		else if(it->first=="gateway")	StrGateway=value;//网关
		else if(it->first=="dns")	StrDNS=value;//DNS地址
		
		else if(it->first=="hwserver")	StrHWServer=value;//华为服务器地址
		else if(it->first=="hwgetpasswd")	StrHWGetPasswd=value;//SNMP GET 密码
		else if(it->first=="hwsetpasswd")	StrHWSetPasswd=value;//SNMP SET 密码
		else if(it->first=="serverurl1")	StrServerURL1=value;	//服务端URL1
		else if(it->first=="serverurl2")	StrServerURL2=value;//服务端URL2
		else if(it->first=="serverurl3")	StrServerURL3=value;	//服务端URL3
		else if(it->first=="stationurl")	StrStationURL=value;//虚拟站端URL
		else if(it->first=="rsucnt")	StrRSUCount=value;//RSU数量
		else if(it->first=="rsu1ip")	StrRSUIP[0]=value;//RSUIP地址
		else if(it->first=="rsu1port")	StrRSUPort[0]=value;//RSU端口
		else if(it->first=="rsu2ip")	StrRSUIP[1]=value;//RSUIP地址
		else if(it->first=="rsu2port")	StrRSUPort[1]=value;//RSU端口
		else if(it->first=="rsu3ip")	StrRSUIP[2]=value;//RSUIP地址
		else if(it->first=="rsu3port")	StrRSUPort[2]=value;//RSU端口
		else if(it->first=="rsu4ip")	StrRSUIP[3]=value;//RSUIP地址
		else if(it->first=="rsu4port")	StrRSUPort[3]=value;//RSU端口
		else if(it->first=="vehplatecount")	StrVehPlateCount=value;	//识别仪1IP地址
		else if(it->first=="vehplate1ip")	StrVehPlateIP[0]=value;	//识别仪1IP地址
		else if(it->first=="vehplate1port")	StrVehPlatePort[0]=value; //识别仪1端口
		else if(it->first=="vehplate1key")	StrVehPlateKey[0]=value;	//识别仪1用户名密码
		else if(it->first=="vehplate2ip")	StrVehPlateIP[1]=value;	//识别仪2IP地址
		else if(it->first=="vehplate2port")	StrVehPlatePort[1]=value; //识别仪2端口
		else if(it->first=="vehplate2key")	StrVehPlateKey[1]=value;	//识别仪2用户名密码
		else if(it->first=="vehplate3ip")	StrVehPlateIP[2]=value;	//识别仪3IP地址
		else if(it->first=="vehplate3port")	StrVehPlatePort[2]=value; //识别仪3端口
		else if(it->first=="vehplate3key")	StrVehPlateKey[2]=value;	//识别仪3用户名密码
		else if(it->first=="vehplate4ip")	StrVehPlateIP[3]=value;	//识别仪4IP地址
		else if(it->first=="vehplate4port")	StrVehPlatePort[3]=value; //识别仪4端口
		else if(it->first=="vehplate4key")	StrVehPlateKey[3]=value;	//识别仪4用户名密码
		else if(it->first=="vehplate5ip")	StrVehPlateIP[4]=value;	//识别仪5IP地址
		else if(it->first=="vehplate5port")	StrVehPlatePort[4]=value; //识别仪5端口
		else if(it->first=="vehplate5key")	StrVehPlateKey[4]=value;	//识别仪5用户名密码
		else if(it->first=="vehplate6ip")	StrVehPlateIP[5]=value;	//识别仪6IP地址
		else if(it->first=="vehplate6port")	StrVehPlatePort[5]=value; //识别仪6端口
		else if(it->first=="vehplate6key")	StrVehPlateKey[5]=value;	//识别仪6用户名密码
		else if(it->first=="vehplate7ip")	StrVehPlateIP[6]=value;	//识别仪7IP地址
		else if(it->first=="vehplate7port")	StrVehPlatePort[6]=value; //识别仪7端口
		else if(it->first=="vehplate7key")	StrVehPlateKey[6]=value;	//识别仪7用户名密码
		else if(it->first=="vehplate8ip")	StrVehPlateIP[7]=value;	//识别仪8IP地址
		else if(it->first=="vehplate8port")	StrVehPlatePort[7]=value; //识别仪8端口
		else if(it->first=="vehplate8key")	StrVehPlateKey[7]=value;	//识别仪8用户名密码
		else if(it->first=="vehplate9ip")	StrVehPlateIP[8]=value;	//识别仪9IP地址
		else if(it->first=="vehplate9port")	StrVehPlatePort[8]=value; //识别仪9端口
		else if(it->first=="vehplate9key")	StrVehPlateKey[8]=value;	//识别仪9用户名密码
		else if(it->first=="vehplate10ip")	StrVehPlateIP[9]=value;	//识别仪10IP地址
		else if(it->first=="vehplate10port")	StrVehPlatePort[9]=value; //识别仪10端口
		else if(it->first=="vehplate10key")	StrVehPlateKey[9]=value;	//识别仪10用户名密码
		else if(it->first=="vehplate11ip")	StrVehPlateIP[10]=value;	//识别仪11IP地址
		else if(it->first=="vehplate11port")	StrVehPlatePort[10]=value; //识别仪11端口
		else if(it->first=="vehplate11key")	StrVehPlateKey[10]=value;	//识别仪11用户名密码
		else if(it->first=="vehplate12ip")	StrVehPlateIP[11]=value;	//识别仪12IP地址
		else if(it->first=="vehplate12port")	StrVehPlatePort[11]=value; //识别仪12端口
		else if(it->first=="vehplate12key")	StrVehPlateKey[11]=value;	//识别仪12用户名密码
		else if(it->first=="camip")	StrCAMIP=value; //监控摄像头IP地址
		else if(it->first=="camport")	StrCAMPort=value;	//监控摄像头端口
		else if(it->first=="camkey")	StrCAMKey=value; //监控摄像头用户名密码

		else if(it->first=="firewareip")	StrFireWareIP=value;	//防火墙IP
		else if(it->first=="firewaregetpasswd")	StrFireWareGetPasswd=value;	//防火墙get密码
		else if(it->first=="firewaresetpasswd")	StrFireWareSetPasswd=value;	//防火墙set密码
		else if(it->first=="switchip")	StrSwitchIP=value;	//交换机地址
		else if(it->first=="switchgetpasswd")	StrSwitchGetPasswd=value;	//交换机get密码
		else if(it->first=="switchsetpasswd")	StrSwitchSetPasswd=value;	//交换机set密码
		
		else if(it->first=="adrrlock1")	StrAdrrLock[0]=value;	//门锁地址1
		else if(it->first=="adrrlock2")	StrAdrrLock[1]=value;	//门锁地址2
		else if(it->first=="adrrlock3")	StrAdrrLock[2]=value;	//门锁地址3
		else if(it->first=="adrrvameter1")	StrAdrrVAMeter[0]=value;	//电能表地址1
		else if(it->first=="adrrvameter2")	StrAdrrVAMeter[1]=value;	//电能表地址2
		else if(it->first=="adrrvameter3")	StrAdrrVAMeter[2]=value;	//电能表地址3
		else if(it->first=="adrrvameter4")	StrAdrrVAMeter[3]=value;	//电能表地址4
		else if(it->first=="adrrvameter5")	StrAdrrVAMeter[4]=value;	//电能表地址5
		else if(it->first=="adrrvameter6")	StrAdrrVAMeter[5]=value;	//电能表地址6
		else if(it->first=="poweraddr1")	StrAdrrPower[0]=value;	//电源板地址1
		else if(it->first=="poweraddr2")	StrAdrrPower[1]=value;	//电源板地址2
		else if(it->first=="poweraddr3")	StrAdrrPower[2]=value;	//电源板地址3

		for(i=0;i<VEHPLATE_NUM;i++)
		{
        	sprintf(key,"doseq%d",i+1);   //车牌识别DO映射 最多12路车牌识别
			if(it->first==key)	StrDoSeq[i]=value;	
		}
		it++;
	}
	
	printf("\n");
	sprintf(pRCtrl->CabinetType,"%s",StrCabinetType.c_str());
	sprintf(pRCtrl->FlagNetRoadID,"%s",StrFlagNetRoadID.c_str());
	sprintf(pRCtrl->FlagRoadID,"%s",StrFlagRoadID.c_str());
	sprintf(pRCtrl->FlagID,"%s",StrFlagID.c_str()); //ETC 门架编号
	sprintf(pRCtrl->PosId,"%s",StrPosId.c_str());	//ETC 门架序号
	sprintf(pRCtrl->Direction,"%s",StrDirection.c_str());	//行车方向
	sprintf(pRCtrl->DirDescription,"%s",StrDirDescription.c_str()); //行车方向说明
	
    //IP 地址
	sprintf(pIPInfo->ip,"%s",StrIP.c_str());//IP地址
	sprintf(pIPInfo->submask,"%s",StrMask.c_str());	//子网掩码
	sprintf(pIPInfo->gateway_addr,"%s",StrGateway.c_str());	//网关
	sprintf(pIPInfo->dns,"%s",StrDNS.c_str());//DNS地址
	
	sprintf(pRCtrl->HWServer,"%s",StrHWServer.c_str());	// 华为服务器IP地址
	sprintf(pRCtrl->HWGetPasswd,"%s",StrHWGetPasswd.c_str());	//SNMP GET 密码 
	sprintf(pRCtrl->HWSetPasswd,"%s",StrHWSetPasswd.c_str());	//SNMP SET 密码 
	sprintf(pRCtrl->ServerURL1,"%s",StrServerURL1.c_str());	//服务器1推送地址
	sprintf(pRCtrl->ServerURL2,"%s",StrServerURL2.c_str());	//服务器2推送地址
	sprintf(pRCtrl->ServerURL3,"%s",StrServerURL3.c_str());	//服务器3推送地址
	sprintf(pRCtrl->StationURL,"%s",StrStationURL.c_str());	//控制器接收地址
	sprintf(pRCtrl->RSUCount,"%s",StrRSUCount.c_str());	//RSU控制器数量
	for(i=0;i<RSUCTL_NUM;i++)
	{
		sprintf(pRCtrl->RSUIP[i],"%s",StrRSUIP[i].c_str());	//RSUIP地址
		sprintf(pRCtrl->RSUPort[i],"%s",StrRSUPort[i].c_str());	 //RSU端口
	}
	sprintf(pRCtrl->VehPlateCount,"%s",StrVehPlateCount.c_str());	//识别仪数量
	for(i=0;i<VEHPLATE_NUM;i++)
	{
		sprintf(pRCtrl->VehPlateIP[i],"%s",StrVehPlateIP[i].c_str());	//识别仪1IP地址
		sprintf(pRCtrl->VehPlatePort[i],"%s",StrVehPlatePort[i].c_str());	//识别仪1端口
		sprintf(pRCtrl->VehPlateKey[i],"%s",StrVehPlateKey[i].c_str());	//识别仪用户名密码
	}
	sprintf(pRCtrl->CAMIP,"%s",StrCAMIP.c_str());	//监控摄像头IP地址
	sprintf(pRCtrl->CAMPort,"%s",StrCAMPort.c_str());	//监控摄像头端口
	sprintf(pRCtrl->CAMKey,"%s",StrCAMKey.c_str());	//监控摄像头用户名密码

	sprintf(pRCtrl->FireWareIP,"%s",StrFireWareIP.c_str());	//防火墙地址
	sprintf(pRCtrl->FireWareGetPasswd,"%s",StrFireWareGetPasswd.c_str());	//防火墙get密码
	sprintf(pRCtrl->FireWareSetPasswd,"%s",StrFireWareSetPasswd.c_str());	//防火墙set密码
	sprintf(pRCtrl->SwitchIP,"%s",StrSwitchIP.c_str());	//交换机地址
	sprintf(pRCtrl->SwitchGetPasswd,"%s",StrSwitchGetPasswd.c_str());	//交换机get密码
	sprintf(pRCtrl->SwitchSetPasswd,"%s",StrSwitchSetPasswd.c_str());	//交换机set密码

	for(i=0;i<LOCK_NUM;i++)
		sprintf(pRCtrl->LockAddr[i],"%s",StrAdrrLock[i].c_str());	//门锁地址
	for(i=0;i<VA_METER_BD_NUM;i++)
		sprintf(pRCtrl->VameterAddr[i],"%s",StrAdrrVAMeter[i].c_str());	//电能表地址
	for(i=0;i<POWER_BD_NUM;i++)
		sprintf(pRCtrl->PowerAddr[i],"%s",StrAdrrPower[i].c_str());	//电源板地址
	for(i=0;i<VEHPLATE_NUM;i++)
		sprintf(pRCtrl->DoSeq[i],"%s",StrDoSeq[i].c_str());	//车牌识别DO映射 最多12路车牌识别
	
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
	

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
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
	sprintf(str,"%d",pParm->status);strJson = strJson + "\"status\":"+ str +",\n";	// 防雷器在线状态
	sprintf(str,"%d",pParm->struck_times);strJson = strJson + "\"struck_times\":"+ str +"\n";	// 雷击次数
    strJson +=  "}\n\n\0\0";

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
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
	
    //参数设置;		
	strJson = strJson + "\"hwserver\":\""+ StrHWServer +"\",\n";	//华为服务器IP地址
	strJson = strJson + "\"hwgetpasswd\":\""+ StrHWGetPasswd +"\",\n";	//SNMP GET 密码
	strJson = strJson + "\"hwsetpasswd\":\""+ StrHWSetPasswd +"\",\n";	//SNMP SET 密码
	strJson = strJson + "\"serverurl1\":\""+ StrServerURL1 +"\",\n";	//服务器1推送地址
	strJson = strJson + "\"serverurl2\":\""+ StrServerURL2 +"\",\n";	//服务器2推送地址
	strJson = strJson + "\"serverurl3\":\""+ StrServerURL3 +"\",\n";	//服务器3推送地址
	strJson = strJson + "\"stationurl\":\""+ StrStationURL +"\",\n";	//控制器接收地址
	strJson = strJson + "\"rsucount\":"+ StrRSUCount +",\n";	//RSU数量
	rsucnt=atoi(StrRSUCount.c_str());
	for(i=0;i<rsucnt;i++)
	{
		sprintf(str,"\"rsu%dip\":\"%s\",\n",i+1,StrRSUIP[i].c_str()); strJson = strJson + str;//RSUIP地址
		sprintf(str,"\"rsu%dport\":%s,\n",i+1,StrRSUPort[i].c_str()); strJson = strJson + str;//RSU端口
	}
	strJson = strJson + "\"vehplatecount\":"+ StrVehPlateCount +",\n";	//识别仪数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(str,"\"vehplate%dip\":\"%s\",\n",i+1,StrVehPlateIP[i].c_str()); strJson = strJson + str;//识别仪地址
		sprintf(str,"\"vehplate%dport\":%s,\n",i+1,StrVehPlatePort[i].c_str()); strJson = strJson + str;//识别仪端口
		sprintf(str,"\"vehplate%dkey\":\"%s\",\n",i+1,StrVehPlateKey[i].c_str()); strJson = strJson + str;//识别仪用户名密码
	}
	strJson = strJson + "\"camip\":\""+ StrCAMIP +"\",\n";	//监控摄像头IP地址
	sprintf(str,"\"camport\":%s,\n",StrCAMPort.c_str()); strJson = strJson + str;//监控摄像头端口
	strJson = strJson + "\"camkey\":\""+ StrCAMKey +"\",\n";	//监控摄像头用户名密码
	
	strJson = strJson + "\"firewareip\":\""+ StrFireWareIP +"\",\n";	//防火墙IP
	strJson = strJson + "\"firewaregetpasswd\":\""+ StrFireWareGetPasswd +"\",\n";	//防火墙get密码
	strJson = strJson + "\"firewaresetpasswd\":\""+ StrFireWareSetPasswd +"\",\n";	//防火墙set密码
	strJson = strJson + "\"switchip\":\""+ StrSwitchIP +"\",\n";	//交换机IP
	strJson = strJson + "\"switchgetpasswd\":\""+ StrSwitchGetPasswd +"\",\n";	//交换机get密码
	strJson = strJson + "\"switchsetpasswd\":\""+ StrSwitchSetPasswd +"\",\n";	//交换机set密码
	
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
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(str,"\"doseq%d\":\"%s\",\n",i+1,StrDoSeq[i].c_str()); strJson = strJson + str;//车牌识别映射DO
	}
	
	strJson = strJson + "\"devicetype\":\""+ StrdeviceType +"\",\n";	//设备型号900~919
	strJson = strJson + "\"hardwareid\":\""+ StrID +"\",\n";	//硬件ID
	strJson = strJson + "\"softversion\":\""+ StrVersionNo +"\",\n";	//主程序版本号920
	strJson = strJson + "\"softdate\":\""+ StrSoftDate +"\"\n";	//版本日期
	
    strJson +=  "}\n\n\0\0";

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
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
	sprintf(str,"\"rsu_vol\":%.3f,\n",stuVA_Meter_Param[0].phase[11].vln/100.0);strJson = strJson + str;	//电压 
	sprintf(str,"\"rsu_amp\":%.3f,\n",stuVA_Meter_Param[0].phase[11].amp/1000.0); strJson = strJson + str;//电流
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
	strJson +=	"}\n\n\0\0";

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
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
	strJson = strJson + "\"vehplatecnt\":"+ StrVehPlateCount +",\n";	//车牌识别数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(str,"\"vehplate%d_ip\":\"%s\",\n",i+1,StrVehPlateIP[i].c_str());strJson+=str;	//摄相机IP
		sprintf(str,"\"vehplate%d_port\":%s,\n",i+1,StrVehPlatePort[i].c_str());strJson+=str;	//识别仪端口
		sprintf(str,"\"vehplate%d_key\":\"%s\",\n",i+1,StrVehPlateKey[i].c_str());strJson+=str;	//用户名密码
		sprintf(str,"\"vehplate%d_status\":%s,\n",i+1,mTIPcamState[i].statuscode.c_str());strJson+=str;	//摄相机状态编码 0-正常 1-异常
		sprintf(str,"\"vehplate%d_errcode\":%s,\n",i+1,mTIPcamState[i].errcode.c_str());strJson+=str;	//正常时为0，状态异常时为厂商自定义的错误代码
		sprintf(str,"\"vehplate%d_factoryid\":%s,\n",i+1,mTIPcamState[i].factoryid.c_str());strJson+=str;	//摄相机厂商1-宇视2-海康3-大华4-中威5-哈工大6-华为7-北京智通 8-北京信路威
		sprintf(str,"\"vehplate%d_devicemodel\":\"%s\",\n",i+1,mTIPcamState[i].devicemodel.c_str());strJson+=str;	//摄相机的设备型号
		sprintf(str,"\"vehplate%d_softversion\":\"%s\",\n",i+1,mTIPcamState[i].softversion.c_str());strJson+=str;	//摄相机的软件版本号
		sprintf(str,"\"vehplate%d_filllight\":%s,\n",i+1,mTIPcamState[i].filllight.c_str());strJson+=str;	//闪光灯状态 0:正常，1:异常
		if(i==vehplatecnt-1)
			sprintf(str,"\"vehplate%d_temperature\":%s\n",i+1,mTIPcamState[i].temperature.c_str());//摄像枪温度（摄氏度）
		else
			sprintf(str,"\"vehplate%d_temperature\":%s,\n",i+1,mTIPcamState[i].temperature.c_str());//摄像枪温度（摄氏度）
		strJson+=str;	
	}
	
	strJson +=	"}\n\n\0\0";

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
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
	

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
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

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
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

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
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
	
	strJson = strJson + "\"vehplate_count\": " + StrVehPlateCount + ",\n";	//识别仪数量
	strJson = strJson + "\"rsu_count\": " + StrRSUCount + ",\n";	//RSU数量
//	strJson = strJson + "\"antenna_count\": " + stuRsuControl.AntennaCount + ",\n";	//天线头数量

	for(i=0;i<VEHPLATE_NUM-1;i++)	//前面N-1路用作车牌识别
	{
		if(stuVA_Meter_Param[0].phase[i].vln/100.0<5.0)
			sprintf(str,"\"vehplate%d\":0,\n",i+1);	//断电
		else 
			sprintf(str,"\"vehplate%d\":1,\n",i+1);	//通电
		strJson = strJson + str;
		sprintf(str,"\"vehplate%d_vol\":%.3f,\n",i+1,stuVA_Meter_Param[0].phase[i].vln/100.0); strJson = strJson + str;//电压
		sprintf(str,"\"vehplate%d_amp\":%.3f,\n",i+1,stuVA_Meter_Param[0].phase[i].amp/1000.0); strJson = strJson + str;//电流
//		sprintf(str,"\"vehplate%d_enable:\":%d,\n",i+1,stuVA_Meter_Param[0].phase[i].enable); strJson = strJson + str;//电流
	}
	for(i=VEHPLATE_NUM-1;i<VEHPLATE_NUM;i++)	//最后一路用作RSU控制器
	{
		if(stuVA_Meter_Param[0].phase[i].vln/100.0<5.0)
			sprintf(str,"\"rsucontrlor%d\":0,\n",i+1-11);	//断电
		else 
			sprintf(str,"\"rsucontrlor%d\":1,\n",i+1-11);	//通电
		strJson = strJson + str;
		sprintf(str,"\"rsucontrlor%d_vol\":%.3f,\n",i+1-11,stuVA_Meter_Param[0].phase[i].vln/100.0); strJson = strJson + str;//电压
		sprintf(str,"\"rsucontrlor%d_amp\":%.3f,\n",i+1-11,stuVA_Meter_Param[0].phase[i].amp/1000.0); strJson = strJson + str;//电流
//		sprintf(str,"\"rsucontrlor%d_enable:\":%d,\n",i+1,stuVA_Meter_Param[0].phase[i].enable); strJson = strJson + str;//电流
	}
	sprintf(str,"\"frontdoorlock\":%d,\n",lockerHw_Param[0]->status); strJson = strJson + str;//前门锁
	sprintf(str,"\"backdoorlock\":%d\n",lockerHw_Param[1]->status); strJson = strJson + str;//后门锁
//	sprintf(str,"\"sidedoorlock\":%d,\n",lockerHw_Param[2]->status); strJson = strJson + str;//侧门锁
//	strJson = strJson + "\"autoreclosure\": " +  + ",\n";	//

	strJson +=	"}\n\n\0\0";

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
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
    strJson = strJson + "\"hwenvtemperature\": " + HUAWEIDevValue.strhwEnvTemperature.c_str() + ",\n";	//环境温度值 228
    strJson = strJson + "\"hwenvhumidity\": " + HUAWEIDevValue.strhwEnvHumidity.c_str() + ",\n";	//环境湿度值 229
    strJson = strJson + "\"hwdcairctrlmode\": " + HUAWEIDevValue.strhwDcAirCtrlMode.c_str() + ",\n";	//空调控制模式 230
    strJson = strJson + "\"hwdcairrunstatus\": " + HUAWEIDevValue.strhwDcAirRunStatus.c_str() + ",\n";	//空调运行状态 231
    strJson = strJson + "\"hwdcaircompressorrunstatus\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus.c_str() + ",\n";	//空调压缩机运行状态 232
    strJson = strJson + "\"hwdcairinnrfanspeed\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed.c_str() + ",\n";	//空调内机转速 233
    strJson = strJson + "\"hwdcairouterfanspeed\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed.c_str() + ",\n";	//空调外风机转速 234
    strJson = strJson + "\"hwdcaircompressorruntime\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime.c_str() + ",\n";	//空调压缩机运行时间 235
    strJson = strJson + "\"hwdcairenterchanneltemp\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp.c_str() + ",\n";	//空调回风口温度 236
    strJson = strJson + "\"hwdcairpowerontemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint.c_str() + ",\n";	//空调开机温度点 237
    strJson = strJson + "\"hwdcairpowerofftemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint.c_str() + ",\n";	//空调关机温度点 238
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
    strJson = strJson + "\"hwsetenvtempupperlimit\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit.c_str() + ",\n";	//环境温度告警上限
    strJson = strJson + "\"hwsetenvtemplowerlimit\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit.c_str() + ",\n";	//环境温度告警下限
    strJson = strJson + "\"hwsetenvtempultrahightempthreshold\": " + HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold.c_str() + ",\n";	//环境高高温告警点
    strJson = strJson + "\"hwsetenvhumidityupperlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit.c_str() + ",\n";	//环境湿度告警上限
    strJson = strJson + "\"hwsetenvhumiditylowerlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit.c_str() + ",\n";	//环境湿度告警下限
	//直流空调(新增加)
    strJson = strJson + "\"hwdcairruntime\": " + HUAWEIDevValue.strhwDcAirRunTime.c_str() + ",\n";	//空调运行时间
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
	strJson = strJson + "\"flagnetroadid\":"+ StrFlagNetRoadID +",\n";	//2 ETC 门架路网编号
	strJson = strJson + "\"flagroadid\":"+ StrFlagRoadID +",\n";		//3 ETC 门架路段编号
	strJson = strJson + "\"flagid\": " + StrFlagID +",\n";			//4 ETC 门架编号
	strJson = strJson + "\"posid\": " + StrPosId +",\n";			//5 ETC 门架序号
	strJson = strJson + "\"direction\": " + StrDirection +",\n";		//6 行车方向
	strJson = strJson + "\"dirdescription\": \"" + StrDirDescription +"\",\n";		//7 行车方向说明
	strJson = strJson + "\"catchtime\": \"" + sDateTime + "\",\n";	//8 状态时间
	sprintf(str,"\"computer\": %d,\n", stuFlagRunStatus->Computer);strJson += str;		//9 工控机状态
	sprintf(str,"\"diskcapacity\": %d,\n", stuFlagRunStatus->DiskCapacity);strJson += str;		//10 硬盘容量
	sprintf(str,"\"diskusage\": %d,\n", stuFlagRunStatus->DiskUsage);strJson += str;		//11 硬盘使用率
	sprintf(str,"\"powertype\": %d,\n", stuFlagRunStatus->PowerType);strJson += str;		//12 供电类型
	sprintf(str,"\"backuppower\": %d,\n", stuFlagRunStatus->BackUpPower);strJson += str;		//13 备用电源状态
	strJson = strJson +	"\"batteryremain\": " + HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str() + ",\n";	//14 蓄电池电量(华为)
	sprintf(str,"\"dbstate\": %d,\n", stuFlagRunStatus->DBState);strJson += str;		//15 数据库状态
	sprintf(str,"\"cablenetstate\": %d,\n", stuFlagRunStatus->CableNetState);strJson += str;		//16 有线网络状态
	sprintf(str,"\"wirelessstate\": %d,\n", stuFlagRunStatus->WireLessState);strJson += str;		//17 无线网络状态
	sprintf(str,"\"software\": %d,\n", stuFlagRunStatus->SoftWare);strJson += str;		//18 ETC 门架软件状态
//	sprintf(str,"\"softversion\": %s,\n", stuFlagRunStatus->SoftVersion);strJson += str;		//19 软件版本
	sprintf(str,"\"softversion\": \"%s\",\n","LT00201907220006000B");strJson += str;		//19 软件版本
	sprintf(str,"\"cpu_occupancy\": %d,\n", stuFlagRunStatus->cpu_occupancy);strJson += str;		//cpu占用率212
	
	sprintf(str,"\"camercount\": %s,\n", StrVehPlateCount.c_str());strJson = strJson + str; //20 车牌识别设备数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(str,"\"vehplate%d\": %s,\n", i+1,mTIPcamState[i].statuscode.c_str());strJson = strJson + str; //20 车牌识别设备状态
	}
	for(i=vehplatecnt;i<16;i++)
	{
		sprintf(str,"\"vehplate%d\": 2147483647,\n", i+1);strJson = strJson + str; // 车牌识别设备状态
	}
	
	sprintf(str,"%d", stuRsuControl.AntennaCount);strJson = strJson + "\"rsucount\":"+ str +",\n"; 	//37 天线数量
	for(i=0;i<stuRsuControl.AntennaCount;i++)
	{
		sprintf(str,"\"rsu%d\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Status);strJson += str;		//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Power);strJson += str; //39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Channel);strJson += str; //40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Control_state);strJson += str;		//41 天线i 开关状态
	}
	for(i=stuRsuControl.AntennaCount;i<16;i++)
	{
		sprintf(str,"\"rsu%d\": 0,\n", i+1);strJson += str; 	//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": 0,\n", i+1);strJson += str;	//39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": 0,\n", i+1);strJson += str; //40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\":	0,\n", i+1);strJson += str; 	//41 天线i 开关状态
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
	sprintf(str,"\"status\": %d,\n", stuSpd_Param->status);strJson += str;		//防雷器在线状态179
	sprintf(str,"\"struck_times\": %d,\n", stuSpd_Param->struck_times);strJson += str;		//雷击次数180
	
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
	strJson = strJson + "\"hwenvtemperature\": " + HUAWEIDevValue.strhwEnvTemperature.c_str() + ",\n";	//环境温度值 228
	strJson = strJson + "\"hwenvhumidity\": " + HUAWEIDevValue.strhwEnvHumidity.c_str() + ",\n";	//环境湿度值 229
	strJson = strJson + "\"hwdcairctrlmode\": " + HUAWEIDevValue.strhwDcAirCtrlMode.c_str() + ",\n";	//空调控制模式 230
	strJson = strJson + "\"hwdcairrunstatus\": " + HUAWEIDevValue.strhwDcAirRunStatus.c_str() + ",\n";	//空调运行状态 231
	strJson = strJson + "\"hwdcaircompressorrunstatus\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus.c_str() + ",\n";	//空调压缩机运行状态 232
	strJson = strJson + "\"hwdcairinnrfanspeed\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed.c_str() + ",\n";	//空调内机转速 233
	strJson = strJson + "\"hwdcairouterfanspeed\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed.c_str() + ",\n";	//空调外风机转速 234
	strJson = strJson + "\"hwdcaircompressorruntime\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime.c_str() + ",\n";	//空调压缩机运行时间 235
	strJson = strJson + "\"hwdcairenterchanneltemp\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp.c_str() + ",\n";	//空调回风口温度 236
	strJson = strJson + "\"hwdcairpowerontemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint.c_str() + ",\n";	//空调开机温度点 237
	strJson = strJson + "\"hwdcairpowerofftemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint.c_str() + ",\n";	//空调关机温度点 238
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
    strJson = strJson + "\"hwsetenvtempupperlimit\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit.c_str() + ",\n";	//环境温度告警上限
    strJson = strJson + "\"hwsetenvtemplowerlimit\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit.c_str() + ",\n";	//环境温度告警下限
    strJson = strJson + "\"hwsetenvtempultrahightempthreshold\": " + HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold.c_str() + ",\n";	//环境高高温告警点
    strJson = strJson + "\"hwsetenvhumidityupperlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit.c_str() + ",\n";	//环境湿度告警上限
    strJson = strJson + "\"hwsetenvhumiditylowerlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit.c_str() + ",\n";	//环境湿度告警下限
	//直流空调(新增加)
    strJson = strJson + "\"hwdcairruntime\": " + HUAWEIDevValue.strhwDcAirRunTime.c_str() + ",\n";	//空调运行时间
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
    strJson = strJson + "\"flagnetroadid\":"+ StrFlagNetRoadID +",\n";	//2 ETC 门架路网编号
    strJson = strJson + "\"flagroadid\":"+ StrFlagRoadID +",\n";		//3 ETC 门架路段编号
    strJson = strJson + "\"flagid\": " + StrFlagID +",\n";			//4 ETC 门架编号
    strJson = strJson + "\"posid\": " + StrPosId +",\n";			//5 ETC 门架序号
    strJson = strJson + "\"direction\": " + StrDirection +",\n";		//6 行车方向
    strJson = strJson + "\"dirdescription\": \"" + StrDirDescription +"\",\n";		//7 行车方向说明
    strJson = strJson + "\"catchtime\": \"" + sDateTime + "\",\n";	//8 状态时间
	sprintf(str,"\"computer\": %d,\n", stuFlagRunStatus->Computer);strJson += str;		//9 工控机状态
	sprintf(str,"\"diskcapacity\": %d,\n", stuFlagRunStatus->DiskCapacity);strJson += str;		//10 硬盘容量
	sprintf(str,"\"diskusage\": %d,\n", stuFlagRunStatus->DiskUsage);strJson += str;		//11 硬盘使用率
	sprintf(str,"\"powertype\": %d,\n", stuFlagRunStatus->PowerType);strJson += str;		//12 供电类型
	sprintf(str,"\"backuppower\": %d,\n", stuFlagRunStatus->BackUpPower);strJson += str;		//13 备用电源状态
	strJson = strJson +	"\"batteryremain\": " + HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str() + ",\n";	//14 蓄电池电量(华为)
	sprintf(str,"\"dbstate\": %d,\n", stuFlagRunStatus->DBState);strJson += str;		//15 数据库状态
	sprintf(str,"\"cablenetstate\": %d,\n", stuFlagRunStatus->CableNetState);strJson += str;		//16 有线网络状态
	sprintf(str,"\"wirelessstate\": %d,\n", stuFlagRunStatus->WireLessState);strJson += str;		//17 无线网络状态
	sprintf(str,"\"software\": %d,\n", stuFlagRunStatus->SoftWare);strJson += str;		//18 ETC 门架软件状态
//	sprintf(str,"\"softversion\": %s,\n", stuFlagRunStatus->SoftVersion);strJson += str;		//19 软件版本
	sprintf(str,"\"softversion\": \"%s\",\n","LT00201907220006000B");strJson += str;		//19 软件版本
	sprintf(str,"\"cpu_occupancy\": %d,\n", stuFlagRunStatus->cpu_occupancy);strJson += str;		//cpu占用率212
	
	sprintf(str,"\"camercount\": %s,\n", StrVehPlateCount.c_str());strJson = strJson + str; //20 车牌识别设备数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(str,"\"vehplate%d\": %s,\n", i+1,mTIPcamState[i].statuscode.c_str());strJson = strJson + str; //20 车牌识别设备状态
	}
	for(i=vehplatecnt;i<16;i++)
	{
		sprintf(str,"\"vehplate%d\": 2147483647,\n", i+1);strJson = strJson + str; // 车牌识别设备状态
	}
	
	sprintf(str,"%d", stuRsuControl.AntennaCount);strJson = strJson + "\"rsucount\":"+ str +",\n";		//37 天线数量
	for(i=0;i<stuRsuControl.AntennaCount;i++)
	{
		sprintf(str,"\"rsu%d\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Status);strJson += str;		//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Power);strJson += str;	//39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Channel);strJson += str;	//40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\": %d,\n", i+1,stuRsuControl.AntennaInfoN[i].Control_state);strJson += str;		//41 天线i 开关状态
	}
	for(i=stuRsuControl.AntennaCount;i<16;i++)
	{
		sprintf(str,"\"rsu%d\": 0,\n", i+1);strJson += str;		//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": 0,\n", i+1);strJson += str;	//39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": 0,\n", i+1);strJson += str;	//40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\":  0,\n", i+1);strJson += str;		//41 天线i 开关状态
	}
    
    strJson +=  "\"backup1\": \"string\",\n";	//118 备用1
    strJson +=  "\"backup2\": \"string\",\n";	//119 备用2
    strJson +=  "\"backup3\": \"string\",\n";	//120 备用3
    strJson +=  "\"backup4\": \"string\",\n";	//121 备用4
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
	sprintf(str,"\"status\": %d,\n", stuSpd_Param->status);strJson += str;		//防雷器在线状态179
	sprintf(str,"\"struck_times\": %d,\n", stuSpd_Param->struck_times);strJson += str;		//雷击次数180
	
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
    strJson = strJson + "\"hwenvtemperature\": " + HUAWEIDevValue.strhwEnvTemperature.c_str() + ",\n";	//环境温度值 228
    strJson = strJson + "\"hwenvhumidity\": " + HUAWEIDevValue.strhwEnvHumidity.c_str() + ",\n";	//环境湿度值 229
    strJson = strJson + "\"hwdcairctrlmode\": " + HUAWEIDevValue.strhwDcAirCtrlMode.c_str() + ",\n";	//空调控制模式 230
    strJson = strJson + "\"hwdcairrunstatus\": " + HUAWEIDevValue.strhwDcAirRunStatus.c_str() + ",\n";	//空调运行状态 231
    strJson = strJson + "\"hwdcaircompressorrunstatus\": " + HUAWEIDevValue.strhwDcAirCompressorRunStatus.c_str() + ",\n";	//空调压缩机运行状态 232
    strJson = strJson + "\"hwdcairinnrfanspeed\": " + HUAWEIDevValue.strhwDcAirInnrFanSpeed.c_str() + ",\n";	//空调内机转速 233
    strJson = strJson + "\"hwdcairouterfanspeed\": " + HUAWEIDevValue.strhwDcAirOuterFanSpeed.c_str() + ",\n";	//空调外风机转速 234
    strJson = strJson + "\"hwdcaircompressorruntime\": " + HUAWEIDevValue.strhwDcAirCompressorRunTime.c_str() + ",\n";	//空调压缩机运行时间 235
    strJson = strJson + "\"hwdcairenterchanneltemp\": " + HUAWEIDevValue.strhwDcAirEnterChannelTemp.c_str() + ",\n";	//空调回风口温度 236
    strJson = strJson + "\"hwdcairpowerontemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOnTempPoint.c_str() + ",\n";	//空调开机温度点 237
    strJson = strJson + "\"hwdcairpowerofftemppoint\": " + HUAWEIDevValue.strhwDcAirPowerOffTempPoint.c_str() + ",\n";	//空调关机温度点 238
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
    strJson = strJson + "\"hwsetenvtempupperlimit\": " + HUAWEIDevValue.strhwSetEnvTempUpperLimit.c_str() + ",\n";	//环境温度告警上限
    strJson = strJson + "\"hwsetenvtemplowerlimit\": " + HUAWEIDevValue.strhwSetEnvTempLowerLimit.c_str() + ",\n";	//环境温度告警下限
    strJson = strJson + "\"hwsetenvtempultrahightempthreshold\": " + HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold.c_str() + ",\n";	//环境高高温告警点
    strJson = strJson + "\"hwsetenvhumidityupperlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityUpperLimit.c_str() + ",\n";	//环境湿度告警上限
    strJson = strJson + "\"hwsetenvhumiditylowerlimit\": " + HUAWEIDevValue.strhwSetEnvHumidityLowerLimit.c_str() + ",\n";	//环境湿度告警下限
	//直流空调(新增加)
    strJson = strJson + "\"hwdcairruntime\": " + HUAWEIDevValue.strhwDcAirRunTime.c_str() + ",\n";	//空调运行时间
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
