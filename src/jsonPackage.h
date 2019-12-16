#include <iostream>
#include <cstring>
#include <map>
#include <json/json.h>
#include <stdio.h>
#include "../registers.h"
#include "../rs485server.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include "../snmp.h"
#include "../net_rsu.h"
#include "../cJSON.h"
#include "../net_spd.h"

bool jsonReader(std::string json, std::map<std::string, std::string> &out);
bool jsonWriter(std::map<std::string, std::string> in, std::string &json);
void SetjsonIPStr(int messagetype,string &mstrjson);//5 读取/设置网口1
bool jsonstrIpInfoReader(char* jsonstr, int len, UINT8 *pstIPPam);//8 IP地址
bool jsonStrEvnWriter(int messagetype,char *pstrEnvPam, char *json, int *len);//9 环境寄存器参数
bool jsonStrUpsWriter(int messagetype,char *pstrUpsPam, char *json, int *len);//10 UPS参数
//bool jsonStrSpdWriter(int messagetype,char *pstrUpsPam, char *json, int *len);//11 防雷器寄存器参数
bool jsonStrVMCtlParamWriter(int messagetype,char *pstrDevInfo, string &mstrjson);//12 控制器参数->JSON字符串
bool jsonStrVMCtlParamWriterXY(int messagetype,char *pstrVMCtl, string &mstrjson);//12 控制器参数->JSON字符串(新粤)
bool jsonstrVmCtlParamReader(char* jsonstr, int len, UINT8 *pstPam);//12 JSON字符串->控制器参数
bool jsonstrVmCtlParamReaderXY(char* jsonstr, int len, UINT8 *pstPam);//12 JSON字符串->JSON字符串(新粤)
bool jsonStrAirCondWriter(int messagetype,char *pstPam, char *json, int *len);//13 空调参数->JSON字符串
bool jsonstrAirCondReader(char* jsonstr, int len, UINT8 *pstPam);//13 JSON字符串->空调参数
bool jsonStrRsuWriterXY(int messagetype, string &mstrjson);//14 RSU天线参数
void SetjsonIPSwitchStatusStr(int messagetype,string &mstrjson);//16交换机状态参数
bool jsonStrVehPlateWriter(int messagetype, string &mstrjson);//15 车牌识别仪参数
void SetjsongantryRunStatus(char* table, string &mstrjson);//17 门架运行状态-新部标准
bool jsonstrRCtrlReader(char* jsonstr, int len, UINT8 *pstuRCtrl);//18 json解析到结构体
void SetjsonTableStr(char* table, string &mstrjson);
//bool jsonstrRCtrlReader(string jsonstr, UINT8 *pstuRCtrl);//18 json解析到结构体
bool jsonStrSwitchStatusWriter(int messagetype, string &mstrjson);	//19回路电压电流开关状态
bool jsonStrSwitchStatusWriterXY(int messagetype, string &mstrjson);	//19回路电压电流开关状态
bool jsonStrHWCabinetWriter(int messagetype,char *pstPam, string &mstrjson);				//20 华为机柜状态
void SetjsonCabinetStatus(char* table, string &mstrjson);//20 门架运行状态-新部标准
void SetjsonDealLockerStr(int messagetype,UINT32 cardid,UINT8 lockaddr,string &mstrjson);		//22 门禁开关锁请求					//22 门禁开关锁请求
void SetjsonDealLockerStr64(int messagetype,UINT64 cardid,UINT8 lockaddr,string &mstrjson);
void SetjsonFireWallStatusStr(int messagetype,string &mstrjson);			//23防火墙状态
void SetjsonAtlasStatusStr(int messagetype,string &mstrjson);	//24 ATLAS状态
bool jsonStrReader(char* jsonstrin, int lenin, char* jsonstrout, int *lenout);
bool SetjsonReceiveOKStr(int messagetype,char *json, int *len);
bool jsonIPCamReader(char* jsonstr, int len);
bool jsonIPCam900Reader(char* jsonstr, int len,int mIndex);
void SetjsonIP2Str(int messagetype,string &mstrjson);//25 读取/设置网口2
void SetjsonSpdAIStatusStr(int messagetype,string &mstrjson);	//27 防雷器参数
bool jsonstrSPDReader(char* jsonstr, int len, UINT8 *pstuRCtrl);//27防雷器参数json解析到结构体
void SetjsonSpdResStatusStr(int messagetype,string &mstrjson);//28 接地电阻参数
bool jsonStrVehPlate900Writer(int messagetype, string &mstrjson);	//29 300万全景车牌识别参数


