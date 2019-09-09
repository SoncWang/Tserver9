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
#include "../snmp.h"
#include "../net_rsu.h"

bool jsonReader(std::string json, std::map<std::string, std::string> &out);
bool jsonWriter(std::map<std::string, std::string> in, std::string &json);
void SetjsonIPStr(int messagetype,string &mstrjson);//5 读取/设置网口1
bool jsonstrIpInfoReader(char* jsonstr, int len, UINT8 *pstIPPam);//8 IP地址
bool jsonStrEvnWriter(int messagetype,char *pstrEnvPam, char *json, int *len);//9 环境寄存器参数
bool jsonStrUpsWriter(int messagetype,char *pstrUpsPam, char *json, int *len);//10 UPS参数
bool jsonStrSpdWriter(int messagetype,char *pstrUpsPam, char *json, int *len);//11 防雷器寄存器参数
bool jsonStrVMCtlParamWriter(int messagetype,char *pstrDevInfo, char *json, int *len);//12 控制器参数->JSON字符串
bool jsonstrVmCtlParamReader(char* jsonstr, int len, UINT8 *pstPam, UINT8 *pstIPPam);//12 JSON字符串->控制器参数
bool jsonStrAirCondWriter(int messagetype,char *pstPam, char *json, int *len);//13 空调参数->JSON字符串
bool jsonstrAirCondReader(char* jsonstr, int len, UINT8 *pstPam);//13 JSON字符串->空调参数
bool jsonStrRsuWriter(int messagetype,char *json, int *len);//14 RSU天线参数
void SetjsonIPSwitchStatusStr(int messagetype,string &mstrjson);//16交换机状态参数
bool jsonStrVehPlateWriter(int messagetype,char *json, int *len);//15 车牌识别仪参数
void SetjsonFlagRunStatusStr(int messagetype,string &mstrjson);//17 门架运行状态
void SetjsonTableStr(char* table, string &mstrjson);//17 门架运行状态
bool jsonstrRCtrlReader(char* jsonstr, int len, UINT8 *pstuRCtrl);//18 json解析到结构体
bool jsonStrSwitchStatusWriter(int messagetype, char *json, int *len);	//19回路电压电流开关状态
bool jsonStrHWCabinetWriter(int messagetype,char *pstPam, char *json, int *len);				//20 华为机柜状态
void SetjsonDealLockerStr(int messagetype,UINT32 cardid,UINT8 lockaddr,string &mstrjson);		//22 门禁开关锁请求					//22 门禁开关锁请求
void SetjsonFireWallStatusStr(int messagetype,string &mstrjson);			//23防火墙状态		
bool jsonStrReader(char* jsonstrin, int lenin, char* jsonstrout, int *lenout);
bool SetjsonReceiveOKStr(int messagetype,char *json, int *len);
bool jsonIPCamReader(char* jsonstr, int len);
void SetjsonIP2Str(int messagetype,string &mstrjson);//25 读取/设置网口2


