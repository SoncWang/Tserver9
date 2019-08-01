#include "jsonPackage.h"

#define JSON_LEN 5*1024

extern ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
extern UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体
extern DEVICE_PARAMS *stuDev_Param[6];	//装置参数寄存器
extern VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体
extern RSU_PARAMS stuRSU_Param[2];		//RSU天线信息结构体
extern REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
extern FLAGRUNSTATUS *stuFlagRunStatus;//门架自由流运行状态结构体
//extern THUAWEIGantry *stuHUAWEIDevValue;//华为机柜状态
extern RSUCONTROLER *stuRsuControl;		//RSU控制器状态
extern THUAWEIGantry HUAWEIDevValue;	//华为机柜状态
extern THUAWEIALARM HUAWEIDevAlarm;			//华为机柜告警
extern AIRCOND_PARAM *stuAirCondRead;		//读空调状态结构体
extern AIRCOND_PARAM *stuAirCondWrite;		//写空调状态结构体

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
extern string StrServerURL1;	//服务端URL1
extern string StrServerURL2;	//服务端URL2
extern string StrServerURL3;	//服务端URL3
extern string StrStationURL;	//虚拟站端URL
extern string StrRSUIP;	//RSUIP地址
extern string StrRSUPort;	//RSU端口
extern string StrVehPlate1IP;	//识别仪1IP地址
extern string StrVehPlate1Port;	//识别仪1端口
extern string StrCAMIP;	//监控摄像头IP地址
extern string StrCAMPort;	//监控摄像头端口

extern pthread_mutex_t snmpoidMutex ;

extern void GetConfig(VMCONTROL_PARAM *vmctrl_param);

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
			jsonStrEvnWriter((char*)stuEnvi_Param,jsonstrout,lenout);
			break;
		case NETCMD_SEND_UPS_PARAM: 			//10 UPS参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrUpsWriter((char*)stuUps_Param,jsonstrout,lenout);
			break;
		case NETCMD_SEND_SPD_PARAM: 			//11 防雷器寄存器参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrSpdWriter((char*)stuSpd_Param,jsonstrout,lenout);
			break;
		case NETCMD_SEND_DEV_PARAM: 			//12 控制器参数
			GetConfig(&vmctrl_param);
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrVMCtlParamWriter((char*)&vmctrl_param,jsonstrout,lenout);
			break;
		case NETCMD_SEND_AIR_PARAM: 			//13 空调参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrAirCondWriter((char*)stuAirCondRead,jsonstrout,lenout);
			break;
		case NETCMD_SEND_RSU_PARAM: 			//14 RSU天线参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			SetjsonReceiveOKStr(NETCMD_SEND_RSU_PARAM,jsonstrout,lenout);
			break;
		case NETCMD_SEND_CAM_PARAM: 			//15 车牌识别参数
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			SetjsonReceiveOKStr(NETCMD_SEND_CAM_PARAM,jsonstrout,lenout);
			break;
		case NETCMD_SEND_SWITCH_INFO: 			//16 交换机状态
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			SetjsonReceiveOKStr(NETCMD_SEND_SWITCH_INFO,jsonstrout,lenout);
			break;
		case NETCMD_FLAGRUNSTATUS:			//17 门架运行状态
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			SetjsonFlagRunStatusStr(jsonstrout,lenout);
			break;
		case NETCMD_REMOTE_CONTROL: 			//18 遥控设备
/*			memset(pRecvBuf,0,JSON_LEN);
			memcpy(pRecvBuf,pCMD->data,pCMD->datalen);
			REMOTE_CONTROL *pRCtrl=stuRemote_Ctrl;
			memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
			jsonstrRCtrlReader(pRecvBuf,pCMD->datalen,(UINT8 *)pRCtrl);//将json字符串转换成结构体
			RemoteControl((UINT8*)pRCtrl);*/
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			SetjsonReceiveOKStr(NETCMD_REMOTE_CONTROL,jsonstrout,lenout);
			break;
		case NETCMD_CIRCUIT_STATUS:			//19 回路开关状态
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			SetjsonReceiveOKStr(NETCMD_CIRCUIT_STATUS,jsonstrout,lenout);
			break;
		case NETCMD_HWCABINET_STATUS:			//20  华为机柜状态
			memset(jsonstrout,0,JSON_LEN);
			*lenout=0;
			jsonStrHWCabinetWriter((char*)&HUAWEIDevValue,jsonstrout,lenout);
			break;
		default:
			break;

	}
	return true;
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
		else if(it->first=="serverurl1")	StrServerURL1=value;	//服务端URL1
		else if(it->first=="serverurl2")	StrServerURL2=value;//服务端URL2
		else if(it->first=="serverurl3")	StrServerURL3=value;	//服务端URL3
		else if(it->first=="stationurl")	StrStationURL=value;//虚拟站端URL
		else if(it->first=="rsuip")	StrRSUIP=value;//RSUIP地址
		else if(it->first=="rsuport")	StrRSUPort=value;//RSU端口
		else if(it->first=="vehplate1ip")	StrVehPlate1IP;	//识别仪1IP地址
		else if(it->first=="vehplate1port")	StrVehPlate1Port; //识别仪1端口
		else if(it->first=="camip")	StrCAMIP; //监控摄像头IP地址
		else if(it->first=="camport")	StrCAMPort;	//监控摄像头端口
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
	sprintf(pRCtrl->ServerURL1,"%s",StrServerURL1.c_str());	//服务器1推送地址
	sprintf(pRCtrl->ServerURL2,"%s",StrServerURL2.c_str());	//服务器2推送地址
	sprintf(pRCtrl->ServerURL3,"%s",StrServerURL3.c_str());	//服务器3推送地址
	sprintf(pRCtrl->StationURL,"%s",StrStationURL.c_str());	//控制器接收地址
	sprintf(pRCtrl->RSUIP,"%s",StrRSUIP.c_str());	//RSUIP地址
	sprintf(pRCtrl->RSUPort,"%s",StrRSUPort.c_str());	 //RSU端口
	sprintf(pRCtrl->VehPlate1IP,"%s",StrVehPlate1IP.c_str());	//识别仪1IP地址
	sprintf(pRCtrl->VehPlate1Port,"%s",StrVehPlate1Port.c_str());	//识别仪1端口
	sprintf(pRCtrl->CAMIP,"%s",StrCAMIP.c_str());	//监控摄像头IP地址
	sprintf(pRCtrl->CAMPort,"%s",StrCAMPort.c_str());	//监控摄像头端口

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



bool jsonStrEvnWriter(char *pstrEnvPam, char *json, int *len)
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
    strJson +=  "\"messagetype\": 9,\n";
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


bool jsonStrSpdWriter(char *pstrSpdPam, char *json, int *len)
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
    strJson +=  "\"messagetype\": 11,\n";
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


bool jsonStrVMCtlParamWriter(char *pstrVMCtl, char *json, int *len)
{
	VMCONTROL_PARAM *pParm=(VMCONTROL_PARAM *)pstrVMCtl;

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
    strJson +=  "\"messagetype\": 12,\n";
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
	strJson = strJson + "\"serverurl1\":\""+ StrServerURL1 +"\",\n";	//服务器1推送地址
	strJson = strJson + "\"serverurl2\":\""+ StrServerURL2 +"\",\n";	//服务器2推送地址
	strJson = strJson + "\"serverurl3\":\""+ StrServerURL3 +"\",\n";	//服务器3推送地址
	strJson = strJson + "\"stationurl\":\""+ StrStationURL +"\",\n";	//控制器接收地址
	strJson = strJson + "\"rsuip\":\""+ StrRSUIP +"\",\n";	//RSUIP地址
	strJson = strJson + "\"rsuport\":\""+ StrRSUPort +"\",\n";	//RSU端口
	strJson = strJson + "\"vehplate1ip\":\""+ StrVehPlate1IP +"\",\n";	//识别仪1IP地址
	strJson = strJson + "\"vehplate1port\":\""+ StrVehPlate1Port +"\",\n";	//识别仪1端口
	strJson = strJson + "\"camip\":\""+ StrCAMIP +"\",\n";	//监控摄像头IP地址
	strJson = strJson + "\"camport\":\""+ StrCAMPort +"\",\n";	//监控摄像头端口

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

bool jsonStrRsuWriter(char *pstrRsuInfo, char *json, int *len)
{
	RSU_PARAMS *pParm=(RSU_PARAMS *)pstrRsuInfo;

	std::map<std::string, std::string> item;
	char str[100],itemstr[10];

	// RSU天线数据结构体
    for(int i=0;i<RSU_NUM;i++)
    {
    	sprintf(itemstr,"phase %d vln",i);
		sprintf(str,"%.3f V",pParm->phase[i].vln/100.0);item[itemstr]=str;///RSU天线电压
    	sprintf(itemstr,"phase %d amp",i);
		sprintf(str,"%d mA",pParm->phase[i].amp);item[itemstr]=str;//RSU天线电流
    	sprintf(itemstr,"phase %d enable",i);
		sprintf(str,"%d",pParm->phase[i].enable);item[itemstr]=str;//投切标志
    }

	std::string json_out;
	jsonWriter(item, json_out);
	printf("the json len= %d out = %s\n",json_out.length(), json_out.c_str());
	*len=json_out.length();
	memcpy(json,(char*)json_out.c_str(),json_out.length());
	return true;
}

bool jsonStrRemoteCtrlWriter(char *pstrRemoteInfo, char *json, int *len)
{
/*	REMOTE_CONTROL *pParm=(REMOTE_CONTROL *)pstrRemoteInfo;

	std::map<std::string, std::string> item;
	char str[100],itemstr[10];

	//遥控寄存器
	sprintf(str,"%d",pParm->RSU1_PreClose);item["rsu1_preclose"]=str;//RSU天线1遥合预置
	sprintf(str,"%d",pParm->RSU1_Close);item["rsu1_close"]=str;//RSU天线1遥合执行
	sprintf(str,"%d",pParm->RSU1_PreOpen);item["rsu1_preopen"]=str;//RSU天线1遥分预置
	sprintf(str,"%d",pParm->RSU1_Open);item["rsu1_open"]=str;//RSU天线1遥分执行
	sprintf(str,"%d",pParm->RSU2_PreClose);item["rsu2_preclose"]=str;//RSU天线2遥合预置
	sprintf(str,"%d",pParm->RSU2_Close);item["rsu2_close"]=str;//RSU天线2遥合执行
	sprintf(str,"%d",pParm->RSU2_PreOpen);item["rsu2_PreOpen"]=str;//RSU天线2遥分预置
	sprintf(str,"%d",pParm->RSU2_Open);item["rsu2_open"]=str;//RSU天线2遥分执行
	sprintf(str,"%d",pParm->RSU3_PreClose);item["rsu3_preclose"]=str;//RSU天线3遥合预置
	sprintf(str,"%d",pParm->RSU3_Close);item["rsu3_close"]=str;//RSU天线3遥合执行
	sprintf(str,"%d",pParm->RSU3_PreOpen);item["rsu3_PreOpen"]=str;//RSU天线3遥分预置
	sprintf(str,"%d",pParm->RSU3_Open);item["rsu3_open"]=str;//RSU天线3遥分执行
	sprintf(str,"%d",pParm->RSU4_PreClose);item["rsu4_preclose"]=str;//RSU天线4遥合预置
	sprintf(str,"%d",pParm->RSU4_Close);item["rsu4_close"]=str;//RSU天线4遥合执行
	sprintf(str,"%d",pParm->RSU4_PreOpen);item["rsu4_PreOpen"]=str;//RSU天线4遥分预置
	sprintf(str,"%d",pParm->RSU4_Open);item["rsu4_open"]=str;//RSU天线4遥分执行
	sprintf(str,"%d",pParm->RSU5_PreClose);item["rsu5_preclose"]=str;//RSU天线5遥合预置
	sprintf(str,"%d",pParm->RSU5_Close);item["rsu5_close"]=str;//RSU天线5遥合执行
	sprintf(str,"%d",pParm->RSU5_PreOpen);item["rsu5_PreOpen"]=str;//RSU天线5遥分预置
	sprintf(str,"%d",pParm->RSU5_Open);item["rsu5_open"]=str;//RSU天线5遥分执行
	sprintf(str,"%d",pParm->RSU6_PreClose);item["rsu6_preclose"]=str;//RSU天线6遥合预置
	sprintf(str,"%d",pParm->RSU6_Close);item["rsu6_close"]=str;//RSU天线6遥合执行
	sprintf(str,"%d",pParm->RSU6_PreOpen);item["rsu6_PreOpen"]=str;//RSU天线6遥分预置
	sprintf(str,"%d",pParm->RSU6_Open);item["rsu6_open"]=str;//RSU天线6遥分执行

	sprintf(str,"%d",pParm->SysReset);item["sysreset"]=str;//系统重启
//	sprintf(str,"%d",pParm->Door_UnLock);item["Door_UnLock"]=str;//电子门锁开
//	sprintf(str,"%d",pParm->Door_Lock);item["Door_Lock"]=str;//电子门锁关

	std::string json_out;
	jsonWriter(item, json_out);
	printf("the json len= %d out = %s\n",json_out.length(), json_out.c_str());
	*len=json_out.length();
	memcpy(json,(char*)json_out.c_str(),json_out.length());*/
	return true;
}

bool jsonStrUpsWriter(char *pstrUpsPam, char *json, int *len)
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
    strJson +=  "\"messagetype\": 10,\n";
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

	strJson = strJson + "\"receive\": \"OK\",\n";

	strJson +=	"}\n\n\0\0";

	printf("the json len= %d out = %s\n",strJson.length(), strJson.c_str());
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

	return true;
}

bool jsonStrAirCondWriter(char *pstPam, char *json, int *len)
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
    strJson +=  "\"messagetype\": 13,\n";
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


bool jsonStrHWCabinetWriter(char *pstPam, char *json, int *len)
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
    strJson +=  "\"messagetype\": 20,\n";
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
    strJson = strJson + "\"hwair_cond_comm_fail_alarm\": " + HUAWEIDevAlarm.hwair_cond_comm_fail_alarm.c_str() + "\n";	//空调通信失败告警 251

    pthread_mutex_unlock(&snmpoidMutex);

    strJson +=  "}\n\0";


	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

}


void SetjsonFlagRunStatusStr(char *json, int *len)
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
    strJson +=  "\"messagetype\": 17,\n";
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
	strJson +=	"\"computer\": 0,\n";		//9 工控机状态
	strJson +=	"\"diskcapacity\": 0,\n";	//10 硬盘容量
	strJson +=	"\"diskusage\": 0,\n";		//11 硬盘使用率
	strJson +=	"\"powertype\": 0,\n";		//12 供电类型
	strJson +=	"\"backuppower\": 0,\n";	//13 备用电源状态
	strJson +=	"\"batteryremain\": 0,\n";	//14 蓄电池电量
	strJson +=	"\"dbstate\": 0,\n";		//15 数据库状态
	strJson +=	"\"cablenetstate\": 0,\n";	//16 有线网络状态
	strJson +=	"\"wirelessstate\": 0,\n";	//17 无线网络状态
	strJson +=	"\"software\": 0,\n";		//18 ETC 门架软件状态
	strJson +=	"\"softversion\": \"LT08201906251800000B\",\n";	//19 软件版本

	strJson +=	"\"camercount\": 0,\n"; 	 //20 车牌识别设备数量
	strJson +=	"\"vehplate1\": 0,\n";	 //21 车牌设别1
	strJson +=	"\"vehplate2\": 0,\n";	 //22 车牌设别2
	strJson +=	"\"vehplate3\": 0,\n";//23 车牌设别3
	strJson +=	"\"vehplate4\": 0,\n";//24 车牌设别4
	strJson +=	"\"vehplate5\": 0,\n";//25 车牌设别5
	strJson +=	"\"vehplate6\": 0,\n";//26 车牌设别6
	strJson +=	"\"vehplate7\": 0,\n";//27 车牌设别7
	strJson +=	"\"vehplate8\": 0,\n";//28 车牌设别8
	strJson +=	"\"vehplate9\": 0,\n";//29 车牌设别9
	strJson +=	"\"vehplate10\": 0,\n";//30 车牌设别10
	strJson +=	"\"vehplate11\": 0,\n";//31 车牌设别11
	strJson +=	"\"vehplate12\": 0,\n";//32 车牌设别12
	strJson +=	"\"vehplate13\": 0,\n";//33 车牌设别13
	strJson +=	"\"vehplate14\": 0,\n";//34 车牌设别14
	strJson +=	"\"vehplate15\": 0,\n";//35 车牌设别15
	strJson +=	"\"vehplate16\": 0,\n";//36 车牌设别16

	sprintf(str,"%d", stuRsuControl->AntennaCount);strJson = strJson + "\"rsucount\":"+ str +",\n"; 	//37 天线数量
	for(i=0;i<stuRsuControl->AntennaCount;i++)
	{
		sprintf(str,"\"rsu%d\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Status);strJson += str;		//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Power);strJson += str; //39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Channel);strJson += str; //40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Control_state);strJson += str;		//41 天线i 开关状态
	}
	for(i=stuRsuControl->AntennaCount;i<16;i++)
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
	strJson = strJson + "\"hwair_cond_comm_fail_alarm\": " + HUAWEIDevAlarm.hwair_cond_comm_fail_alarm.c_str() + "\n";	//空调通信失败告警 251

    strJson +=  "}\n\0";

    pthread_mutex_unlock(&snmpoidMutex);

	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

}

void SetjsonTableStr(char* table, char *json, int *len)
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
    strJson +=  "\"computer\": 0,\n";		//9 工控机状态
    strJson +=  "\"diskcapacity\": 0,\n";	//10 硬盘容量
    strJson +=  "\"diskusage\": 0,\n";		//11 硬盘使用率
    strJson +=  "\"powertype\": 0,\n";		//12 供电类型
    strJson +=  "\"backuppower\": 0,\n";	//13 备用电源状态
    strJson +=  "\"batteryremain\": 0,\n";	//14 蓄电池电量
    strJson +=  "\"dbstate\": 0,\n";		//15 数据库状态
    strJson +=  "\"cablenetstate\": 0,\n";	//16 有线网络状态
    strJson +=  "\"wirelessstate\": 0,\n";	//17 无线网络状态
    strJson +=  "\"software\": 0,\n";		//18 ETC 门架软件状态
	strJson +=	"\"softversion\": \"LT08201906251800000B\",\n";	//19 软件版本

	strJson +=	"\"camercount\": 0,\n"; 	 //20 车牌识别设备数量
	strJson +=  "\"vehplate1\": 0,\n"; 	 //21 车牌设别1
	strJson +=  "\"vehplate2\": 0,\n"; 	 //22 车牌设别2
	strJson +=  "\"vehplate3\": 0,\n";//23 车牌设别3
	strJson +=  "\"vehplate4\": 0,\n";//24 车牌设别4
	strJson +=  "\"vehplate5\": 0,\n";//25 车牌设别5
	strJson +=  "\"vehplate6\": 0,\n";//26 车牌设别6
	strJson +=  "\"vehplate7\": 0,\n";//27 车牌设别7
	strJson +=  "\"vehplate8\": 0,\n";//28 车牌设别8
	strJson +=  "\"vehplate9\": 0,\n";//29 车牌设别9
	strJson +=  "\"vehplate10\": 0,\n";//30 车牌设别10
	strJson +=  "\"vehplate11\": 0,\n";//31 车牌设别11
	strJson +=  "\"vehplate12\": 0,\n";//32 车牌设别12
	strJson +=  "\"vehplate13\": 0,\n";//33 车牌设别13
	strJson +=  "\"vehplate14\": 0,\n";//34 车牌设别14
	strJson +=  "\"vehplate15\": 0,\n";//35 车牌设别15
	strJson +=  "\"vehplate16\": 0,\n";//36 车牌设别16

	sprintf(str,"%d", stuRsuControl->AntennaCount);strJson = strJson + "\"rsucount\":"+ str +",\n";		//37 天线数量
	for(i=0;i<stuRsuControl->AntennaCount;i++)
	{
		sprintf(str,"\"rsu%d\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Status);strJson += str;		//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Power);strJson += str;	//39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Channel);strJson += str;	//40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Control_state);strJson += str;		//41 天线i 开关状态
	}
	for(i=stuRsuControl->AntennaCount;i<16;i++)
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
    strJson = strJson + "\"ishandle\": 0\n";	//空调通信失败告警 251

    pthread_mutex_unlock(&snmpoidMutex);

	strJson +=	" }\n";
	strJson +=	" }\n";
	strJson +=	"}\n\0";

	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);

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
