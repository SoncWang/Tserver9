#include <iostream>
#include <cstring>
#include <map>
#include <json/json.h>
#include <stdio.h>
#include "../registers.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../snmp.h"
#include "../net_rsu.h"

bool jsonReader(std::string json, std::map<std::string, std::string> &out);
bool jsonWriter(std::map<std::string, std::string> in, std::string &json);
bool jsonStrEvnWriter(char *pstrEnvPam, char *json, int *len);//9 环境寄存器参数
bool jsonStrUpsWriter(char *pstrUpsPam, char *json, int *len);//10 UPS参数
bool jsonStrSpdWriter(char *pstrUpsPam, char *json, int *len);//11 防雷器寄存器参数
bool jsonStrVMCtlParamWriter(char *pstrDevInfo, char *json, int *len);//12 控制器参数->JSON字符串
bool jsonstrVmCtlParamReader(char* jsonstr, int len, UINT8 *pstPam, UINT8 *pstIPPam);//12 JSON字符串->控制器参数
bool jsonStrAirCondWriter(char *pstPam, char *json, int *len);//13 空调参数->JSON字符串
bool jsonstrAirCondReader(char* jsonstr, int len, UINT8 *pstPam);//13 JSON字符串->空调参数
bool jsonStrRsuWriter(char *pstrRsuInfo, char *json, int *len);//14 RSU天线参数
bool jsonStrFlagRunStatusWriter(char *pstrInfo, char *json, int *len);//17 门架运行状态
void SetjsonFlagRunStatusStr(char *json, int *lenr);//17 门架运行状态
void SetjsonTableStr(char* table, char *json, int *lenr);//17 门架运行状态
bool jsonStrRemoteCtrlWriter(char *pstrRemoteInfo, char *json, int *len);//18 遥控设备
bool jsonStrHWCabinetWriter(char *pstPam, char *json, int *len);				//20 华为机柜状态
bool jsonStrReader(char* jsonstrin, int lenin, char* jsonstrout, int *lenout);
bool SetjsonReceiveOKStr(int messagetype,char *json, int *len);


