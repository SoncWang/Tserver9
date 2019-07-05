#include "jsonPackage.h"

extern ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
extern UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体
extern DEVICE_PARAMS *stuDev_Param;	//装置参数寄存器
extern DeviceInfoParams *stuDev_Info;	//采集器设备信息结构体
extern RSU_PARAMS *stuRSU_Param;		//RSU天线信息结构体
extern REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
extern FLAGRUNSTATUS *stuFlagRunStatus;//门架自由流运行状态结构体
//extern THUAWEIGantry *stuHUAWEIDevValue;//华为机柜状态
extern RSUCONTROLER *stuRsuControl;		//RSU控制器状态
extern THUAWEIGantry HUAWEIDevValue;	//华为机柜状态
extern THUAWEIALARM HUAWEIDevAlarm;			//华为机柜告警

extern string StrFlagNetRoadID;	//ETC 门架路网编号
extern string StrFlagRoadID;		//ETC 门架路段编号
extern string StrFlagID;			//ETC 门架编号
extern string StrPosId;			//ETC 门架序号
extern string StrDirection;		//行车方向
extern string StrDirDescription;	//行车方向说明

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


bool jsonstrDevReader(char* jsonstr, int len, char *pstrDevPam, UINT8 *pBuf)
{
	std::string json = jsonstr;
	std::map<std::string, std::string> out;
	jsonReader(json, out);
	
	DEVICE_PARAMS *pParm=(DEVICE_PARAMS *)pstrDevPam;
	char key[50];
	int value;
	
	std::map<std::string, std::string>::iterator it;
	it = out.begin();
	while (it != out.end())
	{
		sprintf(key,"%s",it->first.c_str());value=atoi(it->second.c_str()) ;
		printf("%s %d\n",key,value);
		if(it->first=="Pre_Remote")	pParm->Pre_Remote=value;//RSU天线遥控预置，0：退出，1：投入// 1205
		else if(it->first=="AutoSend")	pParm->AutoSend=value;//自动上传是否投入//1206
		else if(it->first=="AirCondSet")	pParm->AirCondSet=value;//空调开机关机//1220
		else if(it->first=="AirColdStartPoint")	pParm->AirColdStartPoint=value;//空调制冷点///1221
		else if(it->first=="AirColdLoop")	pParm->AirColdLoop=value;//空调制冷回差///1222
		else if(it->first=="AirHotStartPoint")	pParm->AirHotStartPoint=value;//空调制冷点///1223
		else if(it->first=="AirHotLoop")	pParm->AirHotLoop=value;//空调制冷回差///1224
		it++;
	}
	printf("\n");
	
/*	pParm->AirCondSet=1; 
	pParm->AirColdStartPoint=50;
	pParm->AirColdLoop=10; 
	pParm->AirHotStartPoint=15;
	pParm->AirHotLoop=10; */

	int bp=0;
	pBuf[bp]=PARAMS_REG_MAX*2;bp++;			//字节数
	// 装置参数寄存器
	pBuf[bp]=(pParm->Address&0xFF00) >> 8;bp++;	//终端设备地址高位;
	pBuf[bp]=pParm->Address&0x00FF;bp++;	//终端设备地址低位;
	pBuf[bp]=(pParm->BaudRate_1&0xFF00) >> 8;bp++;	//串口1波特率高位;
	pBuf[bp]=pParm->BaudRate_1&0x00FF;bp++;	//串口1波特率低位;
	pBuf[bp]=(pParm->BaudRate_2&0xFF00) >> 8;bp++;	//串口2波特率高位;
	pBuf[bp]=pParm->BaudRate_2&0x00FF;bp++;	//串口2波特率低位;
	pBuf[bp]=(pParm->BaudRate_3&0xFF00) >> 8;bp++;	//串口3波特率高位;
	pBuf[bp]=pParm->BaudRate_3&0x00FF;bp++;	//串口3波特率低位;
	pBuf[bp]=(pParm->BaudRate_4&0xFF00) >> 8;bp++;	//串口41波特率高位;
	pBuf[bp]=pParm->BaudRate_4&0x00FF;bp++;	//串口4波特率低位;
	pBuf[bp]=(pParm->Pre_Remote&0xFF00) >> 8;bp++;	//RSU天线遥控预置高位;
	pBuf[bp]=pParm->Pre_Remote&0x00FF;bp++;	//RSU天线遥控预置低位;
	pBuf[bp]=(pParm->AutoSend&0xFF00) >> 8;bp++;	//自动上传是否投入高位;
	pBuf[bp]=pParm->AutoSend&0x00FF;bp++;	//自动上传是否投入低位;
	for(int i=0;i<13;i++)
		{pBuf[bp]=0;bp++;pBuf[bp+1]=0;bp++;}		//保留
	pBuf[bp]=(pParm->AirCondSet&0xFF00) >> 8;bp++;	//空调开机关机高位;
	pBuf[bp]=pParm->AirCondSet&0x00FF;bp++;	//空调开机关机低位;
	pBuf[bp]=(pParm->AirColdStartPoint&0xFF00) >> 8;bp++;	//空调制冷点高位;
	pBuf[bp]=pParm->AirColdStartPoint&0x00FF;bp++;	//空调制冷点低位;
	pBuf[bp]=(pParm->AirColdLoop&0xFF00) >> 8;bp++;	//空调制冷回差高位;
	pBuf[bp]=pParm->AirColdLoop&0x00FF;bp++;	//空调制冷回差低位;
	pBuf[bp]=(pParm->AirHotStartPoint&0xFF00) >> 8;bp++;	//空调制热回差高位;
	pBuf[bp]=pParm->AirHotStartPoint&0x00FF;bp++;	//空调制热回差低位;
	pBuf[bp]=(pParm->AirHotLoop&0xFF00) >> 8;bp++;	//空调制热回差高位;
	pBuf[bp]=pParm->AirHotLoop&0x00FF;bp++;	//空调制热回差低位;
	
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
		
		if(it->first=="RSU1_PreClose")	pRCtrl->RSU1_PreClose=value;		//RSU天线1遥合预置
		else if(it->first=="RSU1_Close")	pRCtrl->RSU1_Close=value;	//RSU天线1遥合执行
		else if(it->first=="RSU1_PreOpen")	pRCtrl->RSU1_PreOpen=value;	//RSU天线1遥分预置
		else if(it->first=="RSU1_Open")	pRCtrl->RSU1_Open=value;			//RSU天线1遥分执行
		else if(it->first=="RSU2_PreClose")	pRCtrl->RSU2_PreClose=value;	//RSU天线2遥合预置
		else if(it->first=="RSU2_Close")	pRCtrl->RSU2_Close=value;	//RSU天线2遥合执行
		else if(it->first=="RSU2_PreOpen")	pRCtrl->RSU2_PreOpen=value;	//RSU天线2遥分预置
		else if(it->first=="RSU2_Open")	pRCtrl->RSU2_Open=value;			//RSU天线2遥分执行
		else if(it->first=="RSU3_PreClose")	pRCtrl->RSU3_PreClose=value;	//RSU天线3遥合预置
		else if(it->first=="RSU3_Close")	pRCtrl->RSU3_Close=value;	//RSU天线3遥合执行
		else if(it->first=="RSU3_PreOpen")	pRCtrl->RSU3_PreOpen=value;	//RSU天线3遥分预置
		else if(it->first=="RSU3_Open")	pRCtrl->RSU3_Open=value;			//RSU天线3遥分执行
		else if(it->first=="RSU4_PreClose")	pRCtrl->RSU4_PreClose=value;	//RSU天线4遥合预置
		else if(it->first=="RSU4_Close")	pRCtrl->RSU4_Close=value;	//RSU天线4遥合执行
		else if(it->first=="RSU4_PreOpen")	pRCtrl->RSU4_PreOpen=value;	//RSU天线4遥分预置
		else if(it->first=="RSU4_Open")	pRCtrl->RSU4_Open=value;			//RSU天线4遥分执行
		else if(it->first=="RSU5_PreClose")	pRCtrl->RSU5_PreClose=value;	//RSU天线5遥合预置
		else if(it->first=="RSU5_Close")	pRCtrl->RSU5_Close=value;	//RSU天线5遥合执行
		else if(it->first=="RSU5_PreOpen")	pRCtrl->RSU5_PreOpen=value;	//RSU天线5遥分预置
		else if(it->first=="RSU5_Open")	pRCtrl->RSU5_Open=value;			//RSU天线5遥分执行
		else if(it->first=="RSU6_PreClose")	pRCtrl->RSU6_PreClose=value;	//RSU天线6遥合预置
		else if(it->first=="RSU6_Close")	pRCtrl->RSU6_Close=value;	//RSU天线6遥合执行
		else if(it->first=="RSU6_PreOpen")	pRCtrl->RSU6_PreOpen=value;	//RSU天线6遥分预置
		else if(it->first=="RSU6_Open")	pRCtrl->RSU6_Open=value;			//RSU天线6遥分执行
		
		else if(it->first=="SysReset")	pRCtrl->SysReset=value;			//系统重启
		it++;
	}
	printf("\n");
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
	
	std::map<std::string, std::string> item;

	char str[20];
	// 环境数据结构体
	sprintf(str,"%.1f",pParm->temp/10.0);item["temp"]=str;// 当前环境温度值300 ×10
	sprintf(str,"%.1f",pParm->moist/10.0);item["moist"]=str;// 当前环境湿度值301 ×10
	sprintf(str,"%d",pParm->water_flag);item["water_flag"]=str;// 漏水302
	sprintf(str,"%d",pParm->front_door_flag);item["front_door_flag"]=str;//前柜门开关状态303
	sprintf(str,"%d",pParm->back_door_flag);item["back_door_flag"]=str;//后柜门开关状态304
	sprintf(str,"%d",pParm->door_overtime);item["door_overtime"]=str;//柜门开启超时305
	sprintf(str,"%d",pParm->smoke_event_flag);item["smoke_event_flag"]=str;//烟雾报警306
/*	sprintf(str,"%d",pParm->Reserve1);item["Reserve1"]=str;				//保留1 307
	sprintf(str,"%d",pParm->Reserve2);item["Reserve2"]=str; 			//保留2 308
	sprintf(str,"%d",pParm->Reserve3);item["Reserve3"]=str; 			//保留3 309*/
	sprintf(str,"%d",pParm->air_cond_status);item["air_cond_status"]=str;//空调状态310
	sprintf(str,"%.1f",pParm->air_cond_temp_out/10.0);item["air_cond_temp_out"]=str;//当前空调室外温度值311 ×10
	sprintf(str,"%.1f",pParm->air_cond_temp_in/10.0);item["air_cond_temp_in"]=str;//当前空调室内温度值312 ×10
	sprintf(str,"%d",pParm->air_cond_hightemp_alarm);item["air_cond_hightemp_alarm"]=str;//空调高温告警313
	sprintf(str,"%d",pParm->air_cond_lowtemp_alarm);item["air_cond_lowtemp_alarm"]=str;//空调低温告警314

	std::string json_out;
	jsonWriter(item, json_out);
	printf("the json len= %d out = %s\n",json_out.length(), json_out.c_str());
	*len=json_out.length();
	memcpy(json,(char*)json_out.c_str(),json_out.length());
	return true;
}


bool jsonStrSpdWriter(char *pstrSpdPam, char *json, int *len)
{
	SPD_PARAMS *pParm=(SPD_PARAMS *)pstrSpdPam;
	
	std::map<std::string, std::string> item;

	char str[20];
	// 防雷器结构体
	sprintf(str,"%d",pParm->status);item["status"]=str;//  防雷器在线状态
	sprintf(str,"%d",pParm->DI_status);item["DI_status"]=str;// 防雷器DI输入状态	91
	sprintf(str,"%d",pParm->struck_times);item["struck_times"]=str;// 雷击次数

	std::string json_out;
	jsonWriter(item, json_out);
	printf("the json len= %d out = %s\n",json_out.length(), json_out.c_str());
	*len=json_out.length();
	memcpy(json,(char*)json_out.c_str(),json_out.length());
	return true;
}


bool jsonStrDevWriter(char *pstrDevPam, char *json, int *len)
{
	DEVICE_PARAMS *pParm=(DEVICE_PARAMS *)pstrDevPam;
	
	std::map<std::string, std::string> item;

	char str[100];
	// 装置参数寄存器
	sprintf(str,"%d",pParm->Address);item["Address"]=str;//终端设备地址设置  
	sprintf(str,"%d",pParm->BaudRate_1);item["BaudRate_1"]=str;// 串口1波特率
	sprintf(str,"%d",pParm->BaudRate_2);item["BaudRate_2"]=str;// 串口2波特率
	sprintf(str,"%d",pParm->BaudRate_3);item["BaudRate_3"]=str;//  串口3波特率
	sprintf(str,"%d",pParm->BaudRate_4);item["BaudRate_4"]=str;//串口4波特率
	sprintf(str,"%d",pParm->Pre_Remote);item["Pre_Remote"]=str;// RSU天线遥控预置，0：退出，1：投入
	sprintf(str,"%d",pParm->AutoSend);item["AutoSend"]=str;//  自动上传是否投入
	sprintf(str,"%d",pParm->AirCondSet);item["AirCondSet"]=str;//空调开机关机
	sprintf(str,"%d",pParm->AirColdStartPoint);item["AirColdStartPoint"]=str;//空调制冷点
	sprintf(str,"%d",pParm->AirColdLoop);item["AirColdLoop"]=str;// 空调制冷回差
	sprintf(str,"%d",pParm->AirHotStartPoint);item["AirHotStartPoint"]=str;// 空调制冷点 
	sprintf(str,"%d",pParm->AirHotLoop);item["AirHotLoop"]=str;//空调制冷回差
	
	std::string json_out;
	jsonWriter(item, json_out);
	printf("the json len= %d out = %s\n",json_out.length(), json_out.c_str());
	*len=json_out.length();
	memcpy(json,(char*)json_out.c_str(),json_out.length());
	return true;
}

bool jsonStrDevInfoWriter(char *pstrDevInfo, char *json, int *len)
{
	DeviceInfoParams *pParm=(DeviceInfoParams *)pstrDevInfo;
	
	std::map<std::string, std::string> item;
	char str[100];
	// 装置信息寄存器
//	memcpy(str,&pstrDevInfo->deviceType[0],40);item["deviceType"]=str;//装置型号
    for(int i=0;i<20;i++)
    {
        str[i*2]=(pParm->deviceType[i]&0xff00)>>8;
        str[i*2+1]=pParm->deviceType[i]&0xff;
    }
	item["deviceType"]=str;//装置型号
	sprintf(str,"%d",pParm->softVersion);item["softVersion"]=str;//主程序版本号 
	sprintf(str,"%d",pParm->protocolVersion);item["protocolVersion"]=str;//规约版本 
	sprintf(str,"%d",pParm->softYear);item["softYear"]=str;//  
	sprintf(str,"%d",pParm->softMonth);item["softMonth"]=str;//
	sprintf(str,"%d",pParm->softDate);item["softDate"]=str;// 
	
	std::string json_out;
	jsonWriter(item, json_out);
	printf("the json len= %d out = %s\n",json_out.length(), json_out.c_str());
	*len=json_out.length();
	memcpy(json,(char*)json_out.c_str(),json_out.length());
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
	REMOTE_CONTROL *pParm=(REMOTE_CONTROL *)pstrRemoteInfo;
	
	std::map<std::string, std::string> item;
	char str[100],itemstr[10];
	
	//遥控寄存器
	sprintf(str,"%d",pParm->RSU1_PreClose);item["RSU1_PreClose"]=str;//RSU天线1遥合预置 
	sprintf(str,"%d",pParm->RSU1_Close);item["RSU1_Close"]=str;//RSU天线1遥合执行 
	sprintf(str,"%d",pParm->RSU1_PreOpen);item["RSU1_PreOpen"]=str;//RSU天线1遥分预置  
	sprintf(str,"%d",pParm->RSU1_Open);item["RSU1_Open"]=str;//RSU天线1遥分执行
	sprintf(str,"%d",pParm->RSU2_PreClose);item["RSU2_PreClose"]=str;//RSU天线2遥合预置 
	sprintf(str,"%d",pParm->RSU2_Close);item["RSU2_Close"]=str;//RSU天线2遥合执行 
	sprintf(str,"%d",pParm->RSU2_PreOpen);item["RSU2_PreOpen"]=str;//RSU天线2遥分预置  
	sprintf(str,"%d",pParm->RSU2_Open);item["RSU2_Open"]=str;//RSU天线2遥分执行
	sprintf(str,"%d",pParm->RSU3_PreClose);item["RSU3_PreClose"]=str;//RSU天线3遥合预置 
	sprintf(str,"%d",pParm->RSU3_Close);item["RSU3_Close"]=str;//RSU天线3遥合执行 
	sprintf(str,"%d",pParm->RSU3_PreOpen);item["RSU3_PreOpen"]=str;//RSU天线3遥分预置  
	sprintf(str,"%d",pParm->RSU3_Open);item["RSU3_Open"]=str;//RSU天线3遥分执行
	sprintf(str,"%d",pParm->RSU4_PreClose);item["RSU4_PreClose"]=str;//RSU天线4遥合预置 
	sprintf(str,"%d",pParm->RSU4_Close);item["RSU4_Close"]=str;//RSU天线4遥合执行 
	sprintf(str,"%d",pParm->RSU4_PreOpen);item["RSU4_PreOpen"]=str;//RSU天线4遥分预置  
	sprintf(str,"%d",pParm->RSU4_Open);item["RSU4_Open"]=str;//RSU天线4遥分执行
	sprintf(str,"%d",pParm->RSU5_PreClose);item["RSU5_PreClose"]=str;//RSU天线5遥合预置 
	sprintf(str,"%d",pParm->RSU5_Close);item["RSU5_Close"]=str;//RSU天线5遥合执行 
	sprintf(str,"%d",pParm->RSU5_PreOpen);item["RSU5_PreOpen"]=str;//RSU天线5遥分预置  
	sprintf(str,"%d",pParm->RSU5_Open);item["RSU5_Open"]=str;//RSU天线5遥分执行
	sprintf(str,"%d",pParm->RSU6_PreClose);item["RSU6_PreClose"]=str;//RSU天线6遥合预置 
	sprintf(str,"%d",pParm->RSU6_Close);item["RSU6_Close"]=str;//RSU天线6遥合执行 
	sprintf(str,"%d",pParm->RSU6_PreOpen);item["RSU6_PreOpen"]=str;//RSU天线6遥分预置  
	sprintf(str,"%d",pParm->RSU6_Open);item["RSU6_Open"]=str;//RSU天线6遥分执行
	
	sprintf(str,"%d",pParm->SysReset);item["SysReset"]=str;//系统重启
//	sprintf(str,"%d",pParm->Door_UnLock);item["Door_UnLock"]=str;//电子门锁开
//	sprintf(str,"%d",pParm->Door_Lock);item["Door_Lock"]=str;//电子门锁关
	
	std::string json_out;
	jsonWriter(item, json_out);
	printf("the json len= %d out = %s\n",json_out.length(), json_out.c_str());
	*len=json_out.length();
	memcpy(json,(char*)json_out.c_str(),json_out.length());
	return true;
}

bool jsonStrUpsWriter(char *pstrUpsPam, char *json, int *len)
{
	UPS_PARAMS *pParm=(UPS_PARAMS *)pstrUpsPam;
	
	std::map<std::string, std::string> item;

	char str[20];
	// UPS数据结构体
	// 输入数据
	sprintf(str,"%d",pParm->in_phase_num);item["in_phase_num"]=str;//UPS输入相数 40
	sprintf(str,"%.2f",pParm->in_freq/100.0);item["in_freq"]=str;//交流输入频率	41 ×100
	sprintf(str,"%.1f",pParm->volt_Ain/10.0);item["volt_Ain"]=str;//UPS交流输入相电压A 42 ×10
	sprintf(str,"%.1f",pParm->volt_Bin/10.0);item["volt_Bin"]=str;//UPS交流输入相电压B       43 ×10
	sprintf(str,"%.1f",pParm->volt_Cin/10.0);item["volt_Cin"]=str;//UPS交流输入相电压C 44 ×10
	sprintf(str,"%.1f",pParm->amp_Ain/10.0);item["amp_Ain"]=str;//UPS交流输入相电流A       45 ×10
	sprintf(str,"%.1f",pParm->amp_Bin/10.0);item["amp_Bin"]=str;//UPS交流输入相电流B 46 ×10
	sprintf(str,"%.1f",pParm->amp_Cin/10.0);item["amp_Cin"]=str;//UPS交流输入相电流C 47 ×10
	
	//输出数据
	sprintf(str,"%d",pParm->out_phase_num);item["out_phase_num"]=str;//UPS输出相数 48
	sprintf(str,"%.2f",pParm->out_freq/100.0);item["out_freq"]=str;//交流输出频率 49      ×100
	sprintf(str,"%.1f",pParm->volt_Aout/10.0);item["volt_Aout"]=str;//UPS交流输出相电压A50 ×10
	sprintf(str,"%.1f",pParm->volt_Bout/10.0);item["volt_Bout"]=str;//UPS交流输出相电压B51 ×10
	sprintf(str,"%.1f",pParm->volt_Cout/10.0);item["volt_Cout"]=str;//UPS交流输出相电压C 52      ×10
	sprintf(str,"%.1f",pParm->amp_Aout/10.0);item["amp_Aout"]=str;//UPS交流输出相电流A 53 ×10
	sprintf(str,"%.1f",pParm->amp_Bout/10.0);item["amp_Bout"]=str;//UPS交流输出相电流B 54 ×10
	sprintf(str,"%.1f",pParm->amp_Cout/10.0);item["amp_Cout"]=str;//UPS交流输出相电流C   55 ×10
	
	//电池参数
	sprintf(str,"%d",pParm->running_day);item["running_day"]=str;//UPS运行时间  56 天
	sprintf(str,"%.1f",pParm->battery_volt/10.0);item["battery_volt"]=str;//UPS电池电压 	57 ×10
	sprintf(str,"%.2f",pParm->amp_charge/100.0);item["amp_charge"]=str;//UPS充电电流 58 ×100
	sprintf(str,"%.1f",pParm->battery_left/10.0);item["battery_left"]=str;//UPS电池后备时间 59 ×10，分钟
	sprintf(str,"%.1f",pParm->battery_tmp/10.0);item["battery_tmp"]=str;//电池温度 60 ×10
	sprintf(str,"%.2f",pParm->battery_capacity/100.0);item["battery_capacity"]=str;//电池当前容量  61 ×100%
	sprintf(str,"%d",pParm->battery_dischg_times);item["battery_dischg_times"]=str;//电池放电次数 62

	//USP状态参数
	sprintf(str,"%d",pParm->supply_out_status);item["supply_out_status"]=str;//输出供电状态 63
	sprintf(str,"%d",pParm->supply_in_status);item["supply_in_status"]=str;//输入供电状态 64
	sprintf(str,"%d",pParm->battery_status);item["battery_status"]=str;//电池状态 65

	//USP告警
	sprintf(str,"%d",pParm->main_abnormal);item["main_abnormal"]=str;// 主路异常 66 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->system_overtemp);item["system_overtemp"]=str;// 系统过温, 67 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->sysbat_low_prealarm);item["sysbat_low_prealarm"]=str;// 系统电池电量低预告警,68 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->rectifier_overload);item["rectifier_overload"]=str;// 整流器过载,69 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->inverter_overload);item["inverter_overload"]=str;// 逆变器过载,70 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->bypass_abnomal);item["bypass_abnomal"]=str;// 旁路异常,71 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->battery_low_prealarm);item["battery_low_prealarm"]=str;// 电池电压低,72 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->battery_abnomal);item["battery_abnomal"]=str;// 电池电压异常,73 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->battery_overtemp);item["battery_overtemp"]=str;// 电池过温,74 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->fan_abnormal);item["fan_abnormal"]=str;// 风扇故障,75 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->shutdown_alarm);item["shutdown_alarm"]=str;//紧急关机告警,76 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->maintain_status);item["maintain_status"]=str;//维修模式,77 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->inverter_supply);item["inverter_supply"]=str;//电池逆变供电,78 0x00：正常 0xF0：异常
	sprintf(str,"%d",pParm->bypass_supply);item["bypass_supply"]=str;//旁路供电,79 0x00：正常 0xF0：异常
	
	std::string json_out;
	jsonWriter(item, json_out);
	printf("the json len= %d out = %s\n",json_out.length(), json_out.c_str());
	*len=json_out.length();
	memcpy(json,(char*)json_out.c_str(),json_out.length());
	return true;
}



bool jsonStrFlagRunStatusWriter(char *pstrInfo, char *json, int *len)
{
	FLAGRUNSTATUS *pParm=(FLAGRUNSTATUS *)pstrInfo;
	
	std::map<std::string, std::string> item;
	char str[100],itemstr[10];
	
	sprintf(str,"%d",pParm->RecordNo);item["RecordNo"]=str;//1 记录号
	sprintf(str,"%d",pParm->FlagNetRoadID);item["FlagNetRoadID"]=str;//2 ETC 门架路网编号
	sprintf(str,"%d",pParm->FlagRoadID);item["FlagRoadID"]=str;//3 ETC 门架路段编号
	sprintf(str,"%d",pParm->FlagID);item["FlagID"]=str;//4 ETC 门架编号
	sprintf(str,"%d",pParm->PosId);item["PosId"]=str;//5 ETC 门架序号
	sprintf(str,"%d",pParm->Direction);item["Direction"]=str;//6 行车方向
	sprintf(str,"%s",pParm->DirDescription);item["DirDescription"]=str;//7 行车方向说明
	sprintf(str,"%s",pParm->CatchTime);item["CatchTime"]=str;//8 状态时间
	sprintf(str,"%d",pParm->Computer);item["Computer"]=str;//9 工控机状态
	sprintf(str,"%d",pParm->DiskCapacity);item["DiskCapacity"]=str;//10 硬盘容量
	sprintf(str,"%d",pParm->DiskUsage);item["DiskUsage"]=str;//11 硬盘使用率
	sprintf(str,"%d",pParm->PowerType);item["PowerType"]=str;//12 供电类型
	sprintf(str,"%d",pParm->BackUpPower);item["BackUpPower"]=str;//13 备用电源状态
	sprintf(str,"%d",pParm->BatteryRemain);item["BatteryRemain"]=str;//14 蓄电池电量
	sprintf(str,"%d",pParm->DBState);item["DBState"]=str;//15 数据库状态
	sprintf(str,"%d",pParm->CableNetState);item["CableNetState"]=str;//16 有线网络状态
	sprintf(str,"%d",pParm->WireLessState);item["WireLessState"]=str;//17 无线网络状态
	sprintf(str,"%d",pParm->SoftWare);item["SoftWare"]=str;//18 ETC 门架软件状态
	sprintf(str,"%s",pParm->SoftVersion);item["SoftVersion"]=str;//19 软件版本
	
	sprintf(str,"%d",pParm->CamerCount);item["CamerCount"]=str;//20 车牌识别设备数量
	sprintf(str,"%d",pParm->Vehplate1);item["Vehplate1"]=str;//21 车牌设别1
	sprintf(str,"%d",pParm->Vehplate2);item["Vehplate2"]=str;//22 车牌设别2
	sprintf(str,"%d",pParm->Vehplate3);item["Vehplate3"]=str;//23 车牌设别3
	sprintf(str,"%d",pParm->Vehplate4);item["Vehplate4"]=str;//24 车牌设别4
	sprintf(str,"%d",pParm->Vehplate5);item["Vehplate5"]=str;//25 车牌设别5
	sprintf(str,"%d",pParm->Vehplate6);item["Vehplate6"]=str;//26 车牌设别6
	sprintf(str,"%d",pParm->Vehplate7);item["Vehplate7"]=str;//27 车牌设别7
	sprintf(str,"%d",pParm->Vehplate8);item["Vehplate8"]=str;//28 车牌设别8
	sprintf(str,"%d",pParm->Vehplate9);item["Vehplate9"]=str;//29 车牌设别9
	sprintf(str,"%d",pParm->Vehplate10);item["Vehplate10"]=str;//30 车牌设别10
	sprintf(str,"%d",pParm->Vehplate11);item["Vehplate11"]=str;//31 车牌设别11
	sprintf(str,"%d",pParm->Vehplate12);item["Vehplate12"]=str;//32 车牌设别12
	sprintf(str,"%d",pParm->Vehplate13);item["Vehplate13"]=str;//33 车牌设别13
	sprintf(str,"%d",pParm->Vehplate14);item["Vehplate14"]=str;//34 车牌设别14
	sprintf(str,"%d",pParm->Vehplate15);item["Vehplate15"]=str;//35 车牌设别15
	sprintf(str,"%d",pParm->Vehplate16);item["Vehplate16"]=str;//36 车牌设别16
	
	sprintf(str,"%d",pParm->RSUCount);item["RSUCount"]=str;//37 RSU数量
	sprintf(str,"%d",pParm->RSU1);item["RSU1"]=str;//38 天线1
	sprintf(str,"%d",pParm->RSU1_Power);item["RSU1_Power"]=str;//39 天线1功率
	sprintf(str,"%d",pParm->RSU1_Channel);item["RSU1_Channel"]=str;//40 天线1信道号
	sprintf(str,"%d",pParm->RSU1_Switch);item["RSU1_Switch"]=str;//41 天线1开关状态
	sprintf(str,"%d",pParm->RSU1_ErrInfo);item["RSU1_ErrInfo"]=str;//42 天线1异常信息
	sprintf(str,"%d",pParm->RSU2);item["RSU2"]=str;//43 天线2 
	sprintf(str,"%d",pParm->RSU2_Power);item["RSU2_Power"]=str;//44 天线2功率
	sprintf(str,"%d",pParm->RSU2_Channel);item["RSU2_Channel"]=str;//45 天线2信道号
	sprintf(str,"%d",pParm->RSU2_Switch);item["RSU2_Switch"]=str;//46 天线2开关状态
	sprintf(str,"%d",pParm->RSU2_ErrInfo);item["RSU2_ErrInfo"]=str;//47 天线2异常信息
	sprintf(str,"%d",pParm->RSU3);item["RSU3"]=str;//48 天线3
	sprintf(str,"%d",pParm->RSU3_Power);item["RSU3_Power"]=str;//49 天线3功率
	sprintf(str,"%d",pParm->RSU3_Channel);item["RSU3_Channel"]=str;//50 天线3信道号
	sprintf(str,"%d",pParm->RSU3_Switch);item["RSU3_Switch"]=str;//51 天线3开关状态
	sprintf(str,"%d",pParm->RSU3_ErrInfo);item["RSU3_ErrInfo"]=str;//52 天线3异常信息
	sprintf(str,"%d",pParm->RSU4);item["RSU4"]=str;//53 天线4
	sprintf(str,"%d",pParm->RSU4_Power);item["RSU4_Power"]=str;//54 天线4天线功率
	sprintf(str,"%d",pParm->RSU4_Channel);item["RSU4_Channel"]=str;//55 天线4信道号
	sprintf(str,"%d",pParm->RSU4_Switch);item["RSU4_Switch"]=str;//56 天线4开关状态
	sprintf(str,"%d",pParm->RSU4_ErrInfo);item["RSU4_ErrInfo"]=str;//57 天线4异常信息
	sprintf(str,"%d",pParm->RSU5);item["RSU5"]=str;//58 天线5
	sprintf(str,"%d",pParm->RSU5_Power);item["RSU5_Power"]=str;//59
	sprintf(str,"%d",pParm->RSU5_Channel);item["RSU5_Channel"]=str;//60
	sprintf(str,"%d",pParm->RSU5_Switch);item["RSU5_Switch"]=str;//61
	sprintf(str,"%d",pParm->RSU5_ErrInfo);item["RSU5_ErrInfo"]=str;//62 
	sprintf(str,"%d",pParm->RSU6);item["RSU6"]=str;//63 天线6
	sprintf(str,"%d",pParm->RSU6_Power);item["RSU6_Power"]=str;//64
	sprintf(str,"%d",pParm->RSU6_Channel);item["RSU6_Channel"]=str;//65
	sprintf(str,"%d",pParm->RSU6_Switch);item["RSU6_Switch"]=str;//66
	sprintf(str,"%d",pParm->RSU6_ErrInfo);item["RSU6_ErrInfo"]=str;//67
	sprintf(str,"%d",pParm->RSU7);item["RSU7"]=str;//68 天线7
	sprintf(str,"%d",pParm->RSU7_Power);item["RSU7_Power"]=str;//69
	sprintf(str,"%d",pParm->RSU7_Channel);item["RSU7_Channel"]=str;//70
	sprintf(str,"%d",pParm->RSU7_Switch);item["RSU7_Switch"]=str;//71
	sprintf(str,"%d",pParm->RSU7_ErrInfo);item["RSU7_ErrInfo"]=str;//72
	sprintf(str,"%d",pParm->RSU8);item["RSU8"]=str;//73 天线8
	sprintf(str,"%d",pParm->RSU8_Power);item["RSU8_Power"]=str;//74
	sprintf(str,"%d",pParm->RSU8_Channel);item["RSU8_Channel"]=str;//75
	sprintf(str,"%d",pParm->RSU8_Switch);item["RSU8_Switch"]=str;//76 
	sprintf(str,"%d",pParm->RSU8_ErrInfo);item["RSU8_ErrInfo"]=str;//77 
	sprintf(str,"%d",pParm->RSU9);item["RSU9"]=str;//78 天线9
	sprintf(str,"%d",pParm->RSU9_Power);item["RSU9_Power"]=str;//79 
	sprintf(str,"%d",pParm->RSU9_Channel);item["RSU9_Channel"]=str;//80
	sprintf(str,"%d",pParm->RSU9_Switch);item["RSU9_Switch"]=str;//81
	sprintf(str,"%d",pParm->RSU9_ErrInfo);item["RSU9_ErrInfo"]=str;//82
	sprintf(str,"%d",pParm->RSU10);item["RSU10"]=str;//83 天线10
	sprintf(str,"%d",pParm->RSU10_Power);item["RSU10_Power"]=str;//84 
	sprintf(str,"%d",pParm->RSU10_Channel);item["RSU10_Channel"]=str;//85 
	sprintf(str,"%d",pParm->RSU10_Switch);item["RSU10_Switch"]=str;//86
	sprintf(str,"%d",pParm->RSU10_ErrInfo);item["RSU10_ErrInfo"]=str;//87
	sprintf(str,"%d",pParm->RSU11);item["RSU11"]=str;//88 天线11
	sprintf(str,"%d",pParm->RSU11_Power);item["RSU11_Power"]=str;//89 
	sprintf(str,"%d",pParm->RSU11_Channel);item["RSU11_Channel"]=str;//90
	sprintf(str,"%d",pParm->RSU11_Switch);item["RSU11_Switch"]=str;//91 
	sprintf(str,"%d",pParm->RSU11_ErrInfo);item["RSU11_ErrInfo"]=str;//92
	sprintf(str,"%d",pParm->RSU12);item["RSU12"]=str;//93 天线12
	sprintf(str,"%d",pParm->RSU12_Power);item["RSU12_Power"]=str;//94 
	sprintf(str,"%d",pParm->RSU12_Channel);item["RSU12_Channel"]=str;//95
	sprintf(str,"%d",pParm->RSU12_Switch);item["RSU12_Switch"]=str;//96 
	sprintf(str,"%d",pParm->RSU12_ErrInfo);item["RSU12_ErrInfo"]=str;//97
	sprintf(str,"%d",pParm->RSU13);item["RSU13"]=str;//98 天线13
	sprintf(str,"%d",pParm->RSU13_Power);item["RSU13_Power"]=str;//99
	sprintf(str,"%d",pParm->RSU13_Channel);item["RSU13_Channel"]=str;//100
	sprintf(str,"%d",pParm->RSU13_Switch);item["RSU13_Switch"]=str;//101 
	sprintf(str,"%d",pParm->RSU13_ErrInfo);item["RSU13_ErrInfo"]=str;//102 
	sprintf(str,"%d",pParm->RSU14);item["RSU14"]=str;//103 天线14
	sprintf(str,"%d",pParm->RSU14_Power);item["RSU14_Power"]=str;//104
	sprintf(str,"%d",pParm->RSU14_Channel);item["RSU14_Channel"]=str;//105
	sprintf(str,"%d",pParm->RSU14_Switch);item["RSU14_Switch"]=str;//106
	sprintf(str,"%d",pParm->RSU14_ErrInfo);item["RSU14_ErrInfo"]=str;//107
	sprintf(str,"%d",pParm->RSU15);item["RSU15"]=str;//108 天线15
	sprintf(str,"%d",pParm->RSU15_Power);item["RSU15_Power"]=str;//109 
	sprintf(str,"%d",pParm->RSU15_Channel);item["RSU15_Channel"]=str;//110
	sprintf(str,"%d",pParm->RSU15_Switch);item["RSU15_Switch"]=str;//111
	sprintf(str,"%d",pParm->RSU15_ErrInfo);item["RSU15_ErrInfo"]=str;//112 
	sprintf(str,"%d",pParm->RSU16);item["RSU16"]=str;//113 天线16
	sprintf(str,"%d",pParm->RSU16_Power);item["RSU16_Power"]=str;//114
	sprintf(str,"%d",pParm->RSU16_Channel);item["RSU16_Channel"]=str;//115 
	sprintf(str,"%d",pParm->RSU16_Switch);item["RSU16_Switch"]=str;//116 
	sprintf(str,"%d",pParm->RSU16_ErrInfo);item["RSU16_ErrInfo"]=str;//117 
	sprintf(str,"%s",pParm->BackUp1);item["BackUp1"]=str;//118 备用1
	sprintf(str,"%s",pParm->BackUp2);item["BackUp2"]=str;//119 备用2
	sprintf(str,"%s",pParm->BackUp3);item["BackUp3"]=str;//120 备用3
	sprintf(str,"%s",pParm->BackUp4);item["BackUp4"]=str;//121 备用4
	sprintf(str,"%s",pParm->TimeFlag);item["TimeFlag"]=str;//122 时间戳标识
	
	std::string json_out;
	jsonWriter(item, json_out);
	printf("the json len= %d out = %s\n",json_out.length(), json_out.c_str());
	*len=json_out.length();
	memcpy(json,(char*)json_out.c_str(),json_out.length());
	return true;
}

void SetjsonFlagRunStatusStr(char *json, int *len)
{
	char str[30],sDateTime[30];
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
    strJson +=  "\"flagrunstatus\": {\n";

	
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
    strJson +=  "\"softversion\": \"string\",\n";	//19 软件版本
	
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
		sprintf(str,"\"rsu%d\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Control_state);strJson += str;		//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Power);strJson += str;	//39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Channel);strJson += str;	//40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Status);strJson += str;		//41 天线i 开关状态
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
    strJson +=  "\"timeflag\": \"2019-06-26T11:11:11.737Z\",\n";	//122 时间戳标识
    
	//状态信息
	if(HUAWEIDevValue.strhwEnvTemperature=="2147483647" && HUAWEIDevValue.strhwEnvHumidity=="2147483647") 
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
	}
//	strJson +=	"\"BackUp5[50]\": \"string\",\n";		// 备用 126
//	strJson +=	"\"BackUp6[50]\": \"string\",\n";		// 备用 127
//	strJson +=	"\"BackUp7[50]\": \"string\",\n";		// 备用 128
	
	//空调信息
	strJson +=  "\"air_cond_status\": 0,\n";	//空调整机状态129
	strJson +=  "\"air_cond_fan_in_status\": 0,\n";	//空调内风机状态130
	strJson +=  "\"air_cond_fan_out_status\": 0,\n";	//空调外风机状态131
	strJson +=  "\"air_cond_comp_status\": 0,\n";	//空调压缩机状态132
	strJson +=  "\"air_cond_heater_status\": 0,\n";	//空调电加热状态133
	strJson +=  "\"air_cond_fan_emgy_status\": 0,\n";	//空调应急风机状态134
	strJson +=  "\"air_cond_temp_in\": 0,\n";	//当前空调室内温度值135
	strJson +=  "\"air_cond_temp_out\": 0,\n";	//当前空调室外温度值	136
	strJson +=  "\"air_cond_amp\": 0,\n";	//当前空调室电流值137
	strJson +=  "\"air_cond_volt\": 0,\n";	//当前空调室电压值138

	//UPS信息
	strJson +=  "\"in_phase_num\": 0,\n";	//UPS输入相数139
	strJson +=  "\"in_freq\": 0,\n";	//UPS交流输入频率140
	strJson = strJson + "\"volt_Ain\": " + HUAWEIDevValue.strhwApOrAblVoltage.c_str() + ",\n";	// 交流输入相电压A`141
	strJson = strJson + "\"volt_Bin\": " + HUAWEIDevValue.strhwBpOrBclVoltage.c_str() + ",\n";	// 交流输入相电压B 142
	strJson = strJson + "\"volt_Cin\": " + HUAWEIDevValue.strhwCpOrCalVoltage.c_str() + ",\n";	// 交流输入相电压C143
	strJson = strJson + "\"amp_Ain\": " + HUAWEIDevValue.strhwAphaseCurrent.c_str() + ",\n";	// 交流输入相电流A144
	strJson = strJson + "\"amp_Bin\": " + HUAWEIDevValue.strhwBphaseCurrent.c_str() + ",\n";	// 交流输入相电流B145
	strJson = strJson + "\"amp_Cin\": " + HUAWEIDevValue.strhwCphaseCurrent.c_str() + ",\n";	// 交流输入相电流C146
	strJson +=  "\"fact_Ain\": 0,\n";	//功率因素A147
	strJson +=  "\"fact_Bin\": 0,\n";	//功率因素B148
	strJson +=  "\"fact_Cin\": 0,\n";	//功率因素C149
	strJson +=  "\"bypass_voltA\": 0,\n";	//旁路电压A150
	strJson +=  "\"bypass_voltB\": 0,\n";	//旁路电压B151
	strJson +=  "\"bypass_voltC\": 0,\n";	//旁路电压C152
	strJson +=  "\"bypass_freq\": 0,\n";	//旁路频率153
	strJson +=  "\"out_phase_num\": 0,\n";	//UPS输出相数154
	strJson +=  "\"out_freq\": 0,\n";	//UPS交流输出频率155
	strJson +=  "\"volt_Aout\": 0,\n";	//交流输出相电压A156
	strJson +=  "\"volt_Bout\": 0,\n";	//交流输出相电压B157
	strJson +=  "\"volt_Cout\": 0,\n";	//交流输出相电压C158
	strJson +=  "\"amp_Aout\": 0,\n";	//交流输出相电流A159
	strJson +=  "\"amp_Bout\": 0,\n";	//交流输出相电流B169
	strJson +=  "\"amp_Cout\": 0,\n";	//交流输出相电流C161
	strJson +=  "\"kw_Aout\": 0,\n";	//UPS A相输出有功功率162
	strJson +=  "\"kw_Bout\": 0,\n";	//UPS B相输出有功功率	163
	strJson +=  "\"kw_Cout\": 0,\n";	//UPS C相输出有功功率164
	strJson +=  "\"kva_Aout\": 0,\n";	//UPS A相输出视在功率165
	strJson +=  "\"kva_Bout\": 0,\n";	//UPS B相输出视在功率166
	strJson +=  "\"kva_Cout\": 0,\n";	//UPS C相输出视在功率167
	strJson +=  "\"load_Aout\": 0,\n";	//UPS A相输出负载率168
	strJson +=  "\"load_Bout\": 0,\n";	//UPS B相输出负载率169
	strJson +=  "\"load_Cout\": 0,\n";	//UPS C相输出负载率170
	strJson +=  "\"running_day\": 0,\n";	//UPS运行时间171
	strJson = strJson + "\"battery_volt\": " + HUAWEIDevValue.strhwAcbGroupBatVolt.c_str() + ",\n"; // UPS电池电压172
	strJson +=	"\"amp_charge\": 0,\n"; //UPS充电电流173
	strJson = strJson + "\"amp_discharge\": " + HUAWEIDevValue.strhwAcbGroupBatCurr.c_str() + ",\n";	// UPS放电电流174
	strJson = strJson + "\"battery_left\": " + HUAWEIDevValue.strhwAcbGroupBackupTime.c_str() + ",\n";	// UPS电池后备时间175
	strJson +=	"\"battery_tmp\": 0,\n";	//环境温度176
	strJson = strJson + "\"battery_capacity\": " + HUAWEIDevValue.strhwAcbGroupTotalCapacity.c_str() + ",\n";	//电池当前容量177
	strJson +=  "\"battery_dischg_times\": 0,\n";	//电池放电次数178
	strJson +=  "\"status\": 0,\n";	//防雷器在线状态179
	strJson +=  "\"struck_times\": 0,\n";	//雷击次数180

	//告警信息
	strJson +=  "\"water_flag\": 0,\n";	//	漏水181
	strJson = strJson + "\"front_door_flag\": " + HUAWEIDevAlarm.hwDoorAlarmTraps.c_str() + ",\n";	//前柜门开关状态182
	strJson +=  "\"back_door_flag\": 0,\n";	//	后柜门开关状态183
	strJson +=  "\"door_overtime\": 0,\n";	//	柜门开启超时184
	strJson +=  "\"smoke_event_flag\": 0,\n";	//	烟雾报警185
	strJson +=  "\"air_cond_hightemp_alarm\": 0,\n";	//	空调高温告警186
	strJson +=  "\"air_cond_lowtemp_alarm\": 0,\n";	//	空调低温告警187
	strJson +=  "\"air_cond_highmoist_alarm\": 0,\n";	//	空调高湿告警188
	strJson +=  "\"air_cond_lowmoist_alarm\": 0,\n";	//	空调低湿告警189
	strJson +=  "\"air_cond_infan_alarm\": 0,\n";	//	空调内风机故障190
	strJson +=  "\"air_cond_outfan_alarm\": 0,\n";	//	空调外风机故障191
	strJson +=  "\"air_cond_comp_alarm\": 0,\n";	//	空调压缩机故障192
	strJson +=  "\"air_cond_heater_alarm\": 0,\n";	//	空调电加热故障193
	strJson +=  "\"air_cond_emgyfan_alarm\": 0,\n";	//	空调应急风机故障194
	strJson +=  "\"supply_out_status\": 0,\n";	//	输出供电状态195
	strJson +=  "\"supply_in_status\": 0,\n";	//	输入供电状态196
	strJson +=  "\"battery_status\": 0,\n";	//	电池状态197
	strJson +=  "\"main_abnormal\": 0,\n";	//	主路异常198
	strJson +=  "\"system_overtemp\": 0,\n";	//	系统过温199
	strJson +=  "\"sysbat_low_prealarm\": 0,\n";	//	系统电池电量低预告警200
	strJson +=  "\"rectifier_overload\": 0,\n";	//	整流器过载201
	strJson +=  "\"inverter_overload\": 0,\n";	//	逆变器过载202
	strJson +=  "\"bypass_abnomal\": 0,\n";	//	旁路异常203
	strJson +=  "\"battery_low_prealarm\": 0,\n";	//电池电压低204
	strJson +=  "\"battery_abnomal\": 0,\n";	//	电池电压异常205
	strJson +=  "\"battery_overtemp\": 0,\n";	//	电池过温206
	strJson +=  "\"fan_abnormal\": 0,\n";	//	风扇故障207
	strJson +=  "\"shutdown_alarm\": 0,\n";	//	紧急关机告警208
	strJson +=  "\"maintain_status\": 0,\n";	//	维修模式209
	strJson +=  "\"inverter_supply\": 0,\n";	//	电池逆变供电210
	strJson +=  "\"bypass_supply\": 0\n";	//	旁路供电211
    strJson +=  " }\n";
    strJson +=  "}\n\0";
	
	*len=strJson.length();
	memcpy(json,(char*)strJson.c_str(),*len);
	
}

void SetjsonTableStr(char* table, char *json, int *len)
{
	char str[30],sDateTime[30];
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
    strJson +=  "\"softversion\": \"string\",\n";	//19 软件版本
	
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
		sprintf(str,"\"rsu%d\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Control_state);strJson += str;		//38 天线i 状态
		sprintf(str,"\"rsu%d_power\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Power);strJson += str;	//39 天线i 功率
		sprintf(str,"\"rsu%d_channel\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Channel);strJson += str;	//40 天线i 信道号
		sprintf(str,"\"rsu%d_switch\": %d,\n", i+1,stuRsuControl->AntennaInfoN[i].Status);strJson += str;		//41 天线i 开关状态
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
    strJson +=  "\"timeflag\": \"2019-06-26T11:11:11.737Z\",\n";	//122 时间戳标识
    
	//状态信息
	if(HUAWEIDevValue.strhwEnvTemperature=="2147483647" && HUAWEIDevValue.strhwEnvHumidity=="2147483647") 
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
	}
//	strJson +=	"\"BackUp5[50]\": \"string\",\n";		// 备用 126
//	strJson +=	"\"BackUp6[50]\": \"string\",\n";		// 备用 127
//	strJson +=	"\"BackUp7[50]\": \"string\",\n";		// 备用 128
	
	//空调信息
	strJson +=  "\"air_cond_status\": 0,\n";	//空调整机状态129
	strJson +=  "\"air_cond_fan_in_status\": 0,\n";	//空调内风机状态130
	strJson +=  "\"air_cond_fan_out_status\": 0,\n";	//空调外风机状态131
	strJson +=  "\"air_cond_comp_status\": 0,\n";	//空调压缩机状态132
	strJson +=  "\"air_cond_heater_status\": 0,\n";	//空调电加热状态133
	strJson +=  "\"air_cond_fan_emgy_status\": 0,\n";	//空调应急风机状态134
	strJson +=  "\"air_cond_temp_in\": 0,\n";	//当前空调室内温度值135
	strJson +=  "\"air_cond_temp_out\": 0,\n";	//当前空调室外温度值	136
	strJson +=  "\"air_cond_amp\": 0,\n";	//当前空调室电流值137
	strJson +=  "\"air_cond_volt\": 0,\n";	//当前空调室电压值138

	//UPS信息
	strJson +=  "\"in_phase_num\": 0,\n";	//UPS输入相数139
	strJson +=  "\"in_freq\": 0,\n";	//UPS交流输入频率140
	strJson = strJson + "\"volt_Ain\": " + HUAWEIDevValue.strhwApOrAblVoltage.c_str() + ",\n";	// 交流输入相电压A`141
	strJson = strJson + "\"volt_Bin\": " + HUAWEIDevValue.strhwBpOrBclVoltage.c_str() + ",\n";	// 交流输入相电压B 142
	strJson = strJson + "\"volt_Cin\": " + HUAWEIDevValue.strhwCpOrCalVoltage.c_str() + ",\n";	// 交流输入相电压C143
	strJson = strJson + "\"amp_Ain\": " + HUAWEIDevValue.strhwAphaseCurrent.c_str() + ",\n";	// 交流输入相电流A144
	strJson = strJson + "\"amp_Bin\": " + HUAWEIDevValue.strhwBphaseCurrent.c_str() + ",\n";	// 交流输入相电流B145
	strJson = strJson + "\"amp_Cin\": " + HUAWEIDevValue.strhwCphaseCurrent.c_str() + ",\n";	// 交流输入相电流C146
	strJson +=  "\"fact_Ain\": 0,\n";	//功率因素A147
	strJson +=  "\"fact_Bin\": 0,\n";	//功率因素B148
	strJson +=  "\"fact_Cin\": 0,\n";	//功率因素C149
	strJson +=  "\"bypass_voltA\": 0,\n";	//旁路电压A150
	strJson +=  "\"bypass_voltB\": 0,\n";	//旁路电压B151
	strJson +=  "\"bypass_voltC\": 0,\n";	//旁路电压C152
	strJson +=  "\"bypass_freq\": 0,\n";	//旁路频率153
	strJson +=  "\"out_phase_num\": 0,\n";	//UPS输出相数154
	strJson +=  "\"out_freq\": 0,\n";	//UPS交流输出频率155
	strJson +=  "\"volt_Aout\": 0,\n";	//交流输出相电压A156
	strJson +=  "\"volt_Bout\": 0,\n";	//交流输出相电压B157
	strJson +=  "\"volt_Cout\": 0,\n";	//交流输出相电压C158
	strJson +=  "\"amp_Aout\": 0,\n";	//交流输出相电流A159
	strJson +=  "\"amp_Bout\": 0,\n";	//交流输出相电流B169
	strJson +=  "\"amp_Cout\": 0,\n";	//交流输出相电流C161
	strJson +=  "\"kw_Aout\": 0,\n";	//UPS A相输出有功功率162
	strJson +=  "\"kw_Bout\": 0,\n";	//UPS B相输出有功功率	163
	strJson +=  "\"kw_Cout\": 0,\n";	//UPS C相输出有功功率164
	strJson +=  "\"kva_Aout\": 0,\n";	//UPS A相输出视在功率165
	strJson +=  "\"kva_Bout\": 0,\n";	//UPS B相输出视在功率166
	strJson +=  "\"kva_Cout\": 0,\n";	//UPS C相输出视在功率167
	strJson +=  "\"load_Aout\": 0,\n";	//UPS A相输出负载率168
	strJson +=  "\"load_Bout\": 0,\n";	//UPS B相输出负载率169
	strJson +=  "\"load_Cout\": 0,\n";	//UPS C相输出负载率170
	strJson +=  "\"running_day\": 0,\n";	//UPS运行时间171
	strJson = strJson + "\"battery_volt\": " + HUAWEIDevValue.strhwAcbGroupBatVolt.c_str() + ",\n"; // UPS电池电压172
	strJson +=	"\"amp_charge\": 0,\n"; //UPS充电电流173
	strJson = strJson + "\"amp_discharge\": " + HUAWEIDevValue.strhwAcbGroupBatCurr.c_str() + ",\n";	// UPS放电电流174
	strJson = strJson + "\"battery_left\": " + HUAWEIDevValue.strhwAcbGroupBackupTime.c_str() + ",\n";	// UPS电池后备时间175
	strJson +=	"\"battery_tmp\": 0,\n";	//环境温度176
	strJson = strJson + "\"battery_capacity\": " + HUAWEIDevValue.strhwAcbGroupTotalCapacity.c_str() + ",\n";	//电池当前容量177
	strJson +=  "\"battery_dischg_times\": 0,\n";	//电池放电次数178
	strJson +=  "\"status\": 0,\n";	//防雷器在线状态179
	strJson +=  "\"struck_times\": 0,\n";	//雷击次数180

	//告警信息
	strJson +=  "\"water_flag\": 0,\n";	//	漏水181
	strJson = strJson + "\"front_door_flag\": " + HUAWEIDevAlarm.hwDoorAlarmTraps.c_str() + ",\n";	//前柜门开关状态182
	strJson +=  "\"back_door_flag\": 0,\n";	//	后柜门开关状态183
	strJson +=  "\"door_overtime\": 0,\n";	//	柜门开启超时184
	strJson +=  "\"smoke_event_flag\": 0,\n";	//	烟雾报警185
	strJson +=  "\"air_cond_hightemp_alarm\": 0,\n";	//	空调高温告警186
	strJson +=  "\"air_cond_lowtemp_alarm\": 0,\n";	//	空调低温告警187
	strJson +=  "\"air_cond_highmoist_alarm\": 0,\n";	//	空调高湿告警188
	strJson +=  "\"air_cond_lowmoist_alarm\": 0,\n";	//	空调低湿告警189
	strJson +=  "\"air_cond_infan_alarm\": 0,\n";	//	空调内风机故障190
	strJson +=  "\"air_cond_outfan_alarm\": 0,\n";	//	空调外风机故障191
	strJson +=  "\"air_cond_comp_alarm\": 0,\n";	//	空调压缩机故障192
	strJson +=  "\"air_cond_heater_alarm\": 0,\n";	//	空调电加热故障193
	strJson +=  "\"air_cond_emgyfan_alarm\": 0,\n";	//	空调应急风机故障194
	strJson +=  "\"supply_out_status\": 0,\n";	//	输出供电状态195
	strJson +=  "\"supply_in_status\": 0,\n";	//	输入供电状态196
	strJson +=  "\"battery_status\": 0,\n";	//	电池状态197
	strJson +=  "\"main_abnormal\": 0,\n";	//	主路异常198
	strJson +=  "\"system_overtemp\": 0,\n";	//	系统过温199
	strJson +=  "\"sysbat_low_prealarm\": 0,\n";	//	系统电池电量低预告警200
	strJson +=  "\"rectifier_overload\": 0,\n";	//	整流器过载201
	strJson +=  "\"inverter_overload\": 0,\n";	//	逆变器过载202
	strJson +=  "\"bypass_abnomal\": 0,\n";	//	旁路异常203
	strJson +=  "\"battery_low_prealarm\": 0,\n";	//电池电压低204
	strJson +=  "\"battery_abnomal\": 0,\n";	//	电池电压异常205
	strJson +=  "\"battery_overtemp\": 0,\n";	//	电池过温206
	strJson +=  "\"fan_abnormal\": 0,\n";	//	风扇故障207
	strJson +=  "\"shutdown_alarm\": 0,\n";	//	紧急关机告警208
	strJson +=  "\"maintain_status\": 0,\n";	//	维修模式209
	strJson +=  "\"inverter_supply\": 0,\n";	//	电池逆变供电210
	strJson +=  "\"bypass_supply\": 0\n";	//	旁路供电211
	
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
