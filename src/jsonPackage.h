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
bool jsonStrEvnWriter(char *pstrEnvPam, char *json, int *len);
bool jsonStrUpsWriter(char *pstrUpsPam, char *json, int *len);
bool jsonStrSpdWriter(char *pstrUpsPam, char *json, int *len);
bool jsonStrDevWriter(char *pstrSpdPam, char *json, int *len);
bool jsonStrDevInfoWriter(char *pstrDevInfo, char *json, int *len);
bool jsonStrRsuWriter(char *pstrRsuInfo, char *json, int *len);
bool jsonstrDevReader(char* jsonstr, int len, char *pstrDevPam, UINT8 *pBuf);
bool jsonStrRemoteCtrlWriter(char *pstrRemoteInfo, char *json, int *len);
bool jsonstrRCtrlReader(char* jsonstr, int len, UINT8 *pstuRCtrl);
bool jsonStrFlagRunStatusWriter(char *pstrInfo, char *json, int *len);
void SetjsonFlagRunStatusStr(char *json, int *lenr);
void SetjsonTableStr(char* table, char *json, int *lenr);


