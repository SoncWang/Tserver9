#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>
#include <iostream>

#include "registers.h"
#include "HttpPost.h"
#include "zte.h"
#include "./src/jsonPackage.h"
#include "snmp.h"
#include "base_64.h"
#include "tea.h"
#include "rs485server.h"
#include "ydn23.h"
#include "server.h"
#include "config.h"



using namespace std;

extern string StrHWServer;		//华为服务器地址
extern string StrHWGetPasswd ;
extern string StrHWSetPasswd ;
extern string StrHWServer2;		//华为服务器地址
extern string StrHWGetPasswd2 ;
extern string StrHWSetPasswd2 ;

//中兴机柜ID
string zteTempDevID[2] ;
string zteLockDevID[4] ;
string ztewaterDevID;
string zteDoorDevID[2] ;
string zteSmokeDevID[2] ;
string zteAirDevID[2] ;
string zteBatDevID[4] ;
string zteUPSDevID[2] ;
string zteSwitchPowerDevID;//开关电源
LOCKER_ZTE_ID_PORT zteLockDevParam[4];// 用来对ID号排序，根据PortID排序

//LOCKER_ZTE_PARAMS ZTE_Locker_Param[4];
extern LOCKER_HW_PARAMS *lockerHw_Param[LOCK_MAX_NUM];	//门锁状态结构体
Base64 Base64Cal;

extern pthread_mutex_t snmpoidMutex ;
extern THUAWEIGantry HUAWEIDevValue;
extern THUAWEIALARM HUAWEIDevAlarm;		//华为机柜告警

extern REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
extern VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体

extern string StrServerURL1;	//服务端URL1
extern string StrServerURL2;	//服务端URL2
extern string StrServerURL4;	//服务端URL4


extern bool jsonzteDevReader(char* jsonstr, int len);//读取中兴机柜信息
extern bool jsonzteTempReader(char* jsonstr, int len,int Index);//读取中兴温湿度
//void char_to_long(UINT8* buffer,UINT32* value);
extern void RemoteControl(UINT8* pRCtrl);


string strzteAcbBatVolt[4];				//电池电压
string strzteAcbTotalCapacity[4];           //电池总容量
string strzteAcbTotalRemainCapacity[4];		//电池剩余容量
string strzteAcbTemperature[4];             //电池温度

extern string StrCabinetType;		//机柜类型
extern pthread_mutex_t CabinetTypeMutex ;
pthread_mutex_t lockerwriteMutex;	// 有485，防止冲突


#if((CABINETTYPE == 5) || (CABINETTYPE == 6)) //5：中兴; 6：金晟安


/******************************************************************************
*  函数名: void char_to_long_reverse(INT8U* buffer,LONG32U* value)
*
*  描述: 字符转化为长整型,逆序
*
*
*
*  输入:
*
*  输出:
*
*  返回值:
*
*  其它:
*******************************************************************************/
void char_to_long_reverse(UINT8* buffer,UINT32* value)
{
	LONG_UNION long_value;
	UINT8 i;

	for(i=0;i<4;i++)
	{
		long_value.b[i] = *(buffer + i);
	}
	*value = long_value.i;
}


/******************************************************************************
*  函数名: void char_to_longlong(INT8U* buffer,LONG64U* value)
*
*  描述: 节字符转化为长长整型
*
*
*
*  输入:
*
*  输出:
*
*  返回值:
*
*  其它:
*******************************************************************************/
void char_to_longlong(UINT8* buffer,UINT64* value)
{
	LONGLONG_UNION longlong_value;
	UINT8 i;

	for(i=0;i<8;i++)
	{
		longlong_value.b[7 - i] = *(buffer + i);
	}
	*value = longlong_value.i;
}






//中兴机柜获取

//设备列表获取
bool jsonzteDevReader(char* jsonstr, int len)
{
    //printf("%s \r\n",jsonstr);


  Json::Reader reader;

  Json::Value json_object;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  string strentity = json_object["result"].toStyledString() ;
  if(strentity.size() > 3)
  {
     //printf("result:%s\r\n",strentity.c_str()) ;
     Json::Reader reader_Objs;
     Json::Value json_object_Objs;
     if (!reader_Objs.parse(strentity, json_object_Objs))
       return 0;
      string strObjs = json_object_Objs["Objs"].toStyledString() ;
      if(strObjs.size() > 3)
      {
          int TempTick = 0;
          int AirTick  = 0;
          int BatTick = 0;
          int UPSTick = 0;
          int LockTick = 0;
          int DoorTick = 0;
          int SmokeTick = 0 ;
         //printf("Objs:%s\r\n",strObjs.c_str()) ;
         Json::Value::Members mem = json_object_Objs.getMemberNames();
         for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
         {
             for(int n=0;n<json_object_Objs[*iter].size();n++)
             {
                 string strTemplate = json_object_Objs[*iter][n].toStyledString();


                 //printf("strTemplate:%s\r\n",strTemplate.c_str());
                 //解析设备列表具体协议
                 {
                     Json::Reader reader_devs;
                     Json::Value json_object_devs;
                     if (!reader_devs.parse(strTemplate, json_object_devs))
                       return 0;

                     string strgetTemp = "";
                     string strgetAir = "";
                     string strgetBat = "";
                     string strgetUPS = "";
                     string strgetLock = "";
                     string strgetwater = "";
                     string strgetdoor = "";
                     string strgetSmoke = "";
                     string strgetSwitchPower = "";
                     string strObjId = "";
                     Json::Value::Members memdev = json_object_devs.getMemberNames();

                     for (Json::Value::Members::iterator iterdev = memdev.begin(); iterdev != memdev.end(); iterdev++)
                     {
                         string newstring;
                         stringstream ss;
                         switch (json_object_devs[*iterdev].type())
                         {
                         case Json::nullValue:
                             break;
                         case Json::booleanValue:
                             break;
                         case Json::intValue:
                             ss<<(json_object_devs[*iterdev].asInt());
                             newstring = ss.str();
                             break;
                         case Json::uintValue:
                             ss<<(json_object_devs[*iterdev].asUInt());
                             newstring = ss.str();
                             break;
                         case Json::realValue:
                             ss<<(json_object_devs[*iterdev].asDouble());
                             newstring = ss.str();
                             break;
                         case Json::stringValue:
                             newstring = json_object_devs[*iterdev].asString();
                             break;
                         default:
                             break;
                         }

                        // json_object_devs[*iterdev].toStyledString();
                         //printf("newstring:%s,%s\r\n",newstring.c_str(),(*iterdev).c_str());
                         if(newstring == "数字温湿度")
                            strgetTemp = newstring;
                         if(newstring == "普通空调")
                            strgetAir = newstring;
                         if(newstring == "锂电池")
                            strgetBat = newstring;
                         if(newstring == "UPS")
                            strgetUPS = newstring;
                         if(newstring == "cabinet_lock")
                             strgetLock = newstring;
                         if(newstring == "水浸")
                             strgetwater = newstring;
                         if(newstring == "门磁")
                             strgetdoor = newstring;
                         if(newstring == "烟感")
                             strgetSmoke = newstring;
                         if(newstring == "开关电源")
                             strgetSwitchPower = newstring;

                         if((*iterdev) == "ObjId")
                             strObjId = newstring ;
                    }

                     if(strgetTemp == "数字温湿度")
                     {
                        zteTempDevID[TempTick]  = strObjId;
                        TempTick = (TempTick+1)%2;
                        printf("数字温湿度设备ID:%s\r\n",strObjId.c_str());
                     }
                     else if(strgetAir == "普通空调")
                     {
                        zteAirDevID[AirTick]  = strObjId;
                        AirTick = (AirTick+1)%2;
                        printf("普通空调设备ID:%s\r\n",strObjId.c_str());
                     }
                     else if(strgetBat == "锂电池")  //string zteBatDevID[4] ;
                     {
                        zteBatDevID[BatTick]  = strObjId;
                        BatTick = (BatTick+1)%4;
                        printf("锂电池设备ID:%s\r\n",strObjId.c_str());
                     }
                     else if(strgetUPS == "UPS")
                     {
                        zteUPSDevID[UPSTick]  = strObjId;
                        UPSTick = (UPSTick+1)%2;
                        printf("UPS设备ID:%s\r\n",strObjId.c_str());
                     }
                     else if(strgetLock == "cabinet_lock")
                     {
                        zteLockDevID[LockTick] = strObjId ;
                        LockTick = (LockTick+1)%4;
                        printf("锁设备ID:%s\r\n",strObjId.c_str());
                     }
                     else if(strgetwater == "水浸")
                     {
                        ztewaterDevID = strObjId ;
                        printf("水浸设备ID:%s\r\n",strObjId.c_str());
                     }
                     else if(strgetdoor == "门磁")
                     {
                         zteDoorDevID[DoorTick]  = strObjId;
                         DoorTick = (DoorTick+1)%2;
                         printf("门磁设备ID:%s\r\n",strObjId.c_str());
                     }
                     else if(strgetSmoke == "烟感")
                     {
                         zteSmokeDevID[SmokeTick]  = strObjId;
                         SmokeTick = (SmokeTick+1)%2;
                         printf("烟感设备ID:%s\r\n",strObjId.c_str());
                     }
                     else if(strgetSwitchPower == "开关电源")
                     {
                        zteSwitchPowerDevID = strObjId ;
                        printf("开关电源设备ID:%s\r\n",strObjId.c_str());
                     }




                 }
             }
         }

      }

  }

  //std::cout << json_object["age"] << std::endl;

return true ;

}

//温湿度获取
bool jsonzteTempReader(char* jsonstr, int len,int Index)
{
    //printf("jsonzteTempReader:%s \r\n",jsonstr);


  Json::Reader reader;

  Json::Value json_object;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  string strObjs = json_object["result"].toStyledString() ;
  if(strObjs.size() > 3)
  {
         // int TempTick = 0;
         // int LockTick = 0;
         //printf("Objs:%s\r\n",strObjs.c_str()) ;
         Json::Value::Members mem = json_object.getMemberNames();
         for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
         {
             for(int n=0;n<json_object[*iter].size();n++)
             {
                 string strTemplate = json_object[*iter][n].toStyledString();


                 //printf("strTemp:%s\r\n",strTemplate.c_str());
                 //解析设备列表具体协议
                 {
                     Json::Reader reader_devs;
                     Json::Value json_object_devs;
                     if (!reader_devs.parse(strTemplate, json_object_devs))
                       return 0;

                     string strgetValue = "";
                     string strObjId = "";
                     Json::Value::Members memdev = json_object_devs.getMemberNames();

                     for (Json::Value::Members::iterator iterdev = memdev.begin(); iterdev != memdev.end(); iterdev++)
                     {
                         string newstring;
                         stringstream ss;
                         switch (json_object_devs[*iterdev].type())
                         {
                         case Json::nullValue:
                             break;
                         case Json::booleanValue:
                             break;
                         case Json::intValue:
                             ss<<(json_object_devs[*iterdev].asInt());
                             newstring = ss.str();
                             break;
                         case Json::uintValue:
                             ss<<(json_object_devs[*iterdev].asUInt());
                             newstring = ss.str();
                             break;
                         case Json::realValue:
                             ss<<(json_object_devs[*iterdev].asDouble());
                             newstring = ss.str();
                             break;
                         case Json::stringValue:
                             newstring = json_object_devs[*iterdev].asString();
                             break;
                         default:
                             break;
                         }

                        // json_object_devs[*iterdev].toStyledString();
                         //printf("newstring:%s,%s\r\n",newstring.c_str(),(*iterdev).c_str());
                         if((*iterdev) == "Id")
                             strObjId = newstring ;
                         else if((*iterdev) == "Value")
                         {
                             strgetValue = newstring ;
                             //printf("Value:%s\r\n",strgetValue.c_str());
                         }
                     }

                     if(strObjId == "118201001")
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         HUAWEIDevValue.strhwEnvTemperature[Index] = strgetValue;
                         printf("温度%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);

                     }
                     else if(strObjId == "118204001")
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         HUAWEIDevValue.strhwEnvHumidity[Index] = strgetValue;
                         printf("湿度%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "115276001")//普通空调柜内温度
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         HUAWEIDevValue.strhwDcAirEnterChannelTemp[Index] = strgetValue;
                         printf("普通空调柜内温度%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "115281001")
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         HUAWEIDevValue.strhwDcAirPowerOnTempPoint[Index] = strgetValue;
                         printf("空调开机温度点%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "115282001")
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         HUAWEIDevValue.strhwDcAirPowerOffTempPoint[Index] = strgetValue;
                         printf("空调关机温度点%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "115283001")
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         HUAWEIDevValue.strhwDcAirPowerOffTempPoint[Index] = strgetValue;
                         printf("空调加热开启温度%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "115284001")
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         //HUAWEIDevValue.strhwDcAirPowerOffTempPoint[Index] = strgetValue;
                         printf("空调加热停止温度%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "190004001") //电池组电压# 49.35
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%4;
                         strzteAcbBatVolt[Index] = strgetValue;
                         printf("电池电压%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "190264001") //剩余容量# 101.95
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%4;
                         strzteAcbTotalRemainCapacity[Index] = strgetValue;
                         printf("电池剩余容量%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "190265001") //满充容量# 102.76
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%4;
                         strzteAcbTotalCapacity[Index] = strgetValue;
                         printf("电池总容量%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "190002001") //单体温度# 27.7
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%4;
                         strzteAcbTemperature[Index] = strgetValue;
                         printf("电池温度%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "108035001")  //输入电压值 222.2
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         if(Index == 0)
                           HUAWEIDevValue.strhwApOrAblVoltage = strgetValue;
                         printf("输入电压值%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "108012001")  //电池电压
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         if(Index == 0)
                            HUAWEIDevValue.strhwDcOutputVoltage = strgetValue;
                         printf("输出电压值%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }
                     else if(strObjId == "108301001") //电压下限
                     {
                         pthread_mutex_lock(&snmpoidMutex);
                         Index = Index%2;
                         if(Index == 0)
                         HUAWEIDevValue.strhwSetAcsLowerVoltLimit = strgetValue;
                         printf("电压下限%d:%s\r\n",Index,strgetValue.c_str());
                         pthread_mutex_unlock(&snmpoidMutex);
                     }


                     //zteUPSDevID

                        //115276001 普通空调柜内温度   115281001 制冷开启温度   115282001 制冷停止温度
                       //115283001 加热开启温度  115284001 加热停止温度   115324001


                 }
             }
         }

  }

  //std::cout << json_object["age"] << std::endl;

return true ;

}



//温湿度阀值
bool jsonzteTempHLReader(char* jsonstr, int len,int Index,int Temptype)
{
    //printf("%s \r\n",jsonstr);


  Json::Reader reader;

  Json::Value json_object;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  string strObjs = json_object["result"].toStyledString() ;
  if(strObjs.size() > 3)
  {
          //int TempTick = 0;
          //int LockTick = 0;
         //printf("Objs:%s\r\n",strObjs.c_str()) ;
        // Json::Value::Members mem = json_object.getMemberNames();
         //for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
         {
             //for(int n=0;n<json_object[*iter].size();n++)
             {
                 string strTemplate = strObjs;//json_object[*iter][n].toStyledString();


                 printf("strTemp:%s\r\n",strTemplate.c_str());
                 //解析设备列表具体协议
                 {
                     Json::Reader reader_devs;
                     Json::Value json_object_devs;
                     if (!reader_devs.parse(strTemplate, json_object_devs))
                       return 0;

                     string strgetlowThreshold = "";
                     string strgethighThreshold = "";
                     Json::Value::Members memdev = json_object_devs.getMemberNames();

                     for (Json::Value::Members::iterator iterdev = memdev.begin(); iterdev != memdev.end(); iterdev++)
                     {
                         string newstring;
                         stringstream ss;
                         switch (json_object_devs[*iterdev].type())
                         {
                         case Json::nullValue:
                             break;
                         case Json::booleanValue:
                             break;
                         case Json::intValue:
                             ss<<(json_object_devs[*iterdev].asInt());
                             newstring = ss.str();
                             break;
                         case Json::uintValue:
                             ss<<(json_object_devs[*iterdev].asUInt());
                             newstring = ss.str();
                             break;
                         case Json::realValue:
                             ss<<(json_object_devs[*iterdev].asDouble());
                             newstring = ss.str();
                             break;
                         case Json::stringValue:
                             newstring = json_object_devs[*iterdev].asString();
                             break;
                         default:
                             break;
                         }

                        // json_object_devs[*iterdev].toStyledString();
                         //printf("newstring:%s,%s\r\n",newstring.c_str(),(*iterdev).c_str());
                         if((*iterdev) == "lowThreshold")
                             strgetlowThreshold = newstring ;
                         else if((*iterdev) == "highThreshold")
                             strgethighThreshold = newstring ;
                     }


                     switch(Temptype)
                     {
                         case 118202001: //温度过高
                         {
                            pthread_mutex_lock(&snmpoidMutex);
                            Index = Index%2;
                            HUAWEIDevValue.strhwSetEnvTempUpperLimit[Index] = strgetlowThreshold;
                            printf("温度%d过高上限:%s\r\n",Index,strgetlowThreshold.c_str());
                            pthread_mutex_unlock(&snmpoidMutex);
                          }
                          break;
                         case 118203001: //温度过低
                         {
                            pthread_mutex_lock(&snmpoidMutex);
                            Index = Index%2;
                            HUAWEIDevValue.strhwSetEnvTempLowerLimit[Index] = strgethighThreshold;
                            printf("温度%d过低下限:%s\r\n",Index,strgethighThreshold.c_str());
                            pthread_mutex_unlock(&snmpoidMutex);
                         }
                          break;
                         case 118205001: //湿度过低
                         {
                            pthread_mutex_lock(&snmpoidMutex);
                            Index = Index%2;
                            HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[Index] = strgethighThreshold;
                            printf("湿度%d过低下限:%s\r\n",Index,strgethighThreshold.c_str());
                            pthread_mutex_unlock(&snmpoidMutex);
                         }
                          break;
                         case 118206001: //湿度过高
                         {
                           pthread_mutex_lock(&snmpoidMutex);
                           Index = Index%2;
                           HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[Index] = strgetlowThreshold;
                           printf("湿度%d过高下限:%s\r\n",Index,strgetlowThreshold.c_str());
                           pthread_mutex_unlock(&snmpoidMutex);
                         }
                          break;


                     }

                 }
             }
         }

  }

  //std::cout << json_object["age"] << std::endl;

return true ;

}




//水浸获取
bool jsonzteAlarmReader(char* jsonstr, int len)
{
    //printf("%s \r\n",jsonstr);


  Json::Reader reader;

  Json::Value json_object;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  string strObjs = json_object["result"].toStyledString() ;
  if(strObjs.size() > 3)
  {
          int TempTick = 0;
          int LockTick = 0;
         //printf("Objs:%s\r\n",strObjs.c_str()) ;
         Json::Value::Members mem = json_object.getMemberNames();
         for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
         {
             for(int n=0;n<json_object[*iter].size();n++)
             {
                 string strTemplate = json_object[*iter][n].toStyledString();


                 //printf("strTemp:%s\r\n",strTemplate.c_str());
                 //解析设备列表具体协议
                 {
                     Json::Reader reader_devs;
                     Json::Value json_object_devs;
                     if (!reader_devs.parse(strTemplate, json_object_devs))
                       return 0;

                     string strgetDeviceId = "";
                     string strObjId = "";
                     Json::Value::Members memdev = json_object_devs.getMemberNames();

                     for (Json::Value::Members::iterator iterdev = memdev.begin(); iterdev != memdev.end(); iterdev++)
                     {
                         string newstring;
                         stringstream ss;
                         switch (json_object_devs[*iterdev].type())
                         {
                         case Json::nullValue:
                             break;
                         case Json::booleanValue:
                             break;
                         case Json::intValue:
                             ss<<(json_object_devs[*iterdev].asInt());
                             newstring = ss.str();
                             break;
                         case Json::uintValue:
                             ss<<(json_object_devs[*iterdev].asUInt());
                             newstring = ss.str();
                             break;
                         case Json::realValue:
                             ss<<(json_object_devs[*iterdev].asDouble());
                             newstring = ss.str();
                             break;
                         case Json::stringValue:
                             newstring = json_object_devs[*iterdev].asString();
                             break;
                         default:
                             break;
                         }

                        // json_object_devs[*iterdev].toStyledString();
                         //printf("newstring:%s,%s\r\n",newstring.c_str(),(*iterdev).c_str());
                         if((*iterdev) == "Id")
                             strObjId = newstring ;
                         else if((*iterdev) == "DeviceId")
                             strgetDeviceId = newstring ;
                     }

                     if(strgetDeviceId == zteDoorDevID[0]) //门磁1告警
                     {
                         HUAWEIDevAlarm.hwDoorAlarmTraps = "1";
                         printf("门磁1告警\r\n");
                     }
                     else if(strgetDeviceId == zteDoorDevID[1]) //门磁2告警
                     {
                         HUAWEIDevAlarm.hwDoorAlarmTraps = "1";
                         printf("门磁2告警\r\n");
                     }
                     else if(strgetDeviceId == ztewaterDevID) //水浸告警
                     {
                         HUAWEIDevAlarm.hwWaterAlarmTraps = "1";
                         printf("水浸告警\r\n");
                     }
                     if(strgetDeviceId == zteSmokeDevID[0]) //烟感1告警
                     {
                         HUAWEIDevAlarm.hwSmokeAlarmTraps = "1";
                         printf("烟感1告警\r\n");
                     }
                     else if(strgetDeviceId == zteSmokeDevID[1]) //烟感2告警
                     {
                         HUAWEIDevAlarm.hwSmokeAlarmTraps = "1";
                         printf("烟感2告警\r\n");
                     }
                     else if(strgetDeviceId == zteSwitchPowerDevID) //开关电源告警
                     {
                         if(strObjId == "0")
                         {
                            HUAWEIDevAlarm.SwitchPowerCom_alarm = "1";
                            printf("开关电源断线告警\r\n");
                         }

                         int poslen = strObjId.find("106132"); //106132001 106132002 后3位为序列号
                         if(poslen == 0) //整流模块通讯故障
                         {
                            HUAWEIDevAlarm.RectifierModuleCom_alarm = "1";
                            printf("整流模块通讯故障告警\r\n");
                         }

                     }




                 }
             }
         }

  }

  //std::cout << json_object["age"] << std::endl;

return true ;

}


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////          锁处理开始             ///////////////////////////////////

// 中兴机柜的打包子函数
UINT8 zte_cmd_pack(UINT8 door_pos,UINT16 door_cmd,UINT8 *buf)
{
	UINT8 *pbuf = buf;
	UINT8 len = 0;
	UINT8 crc_cal[2];

	pbuf[len++] = 0x7E;
	pbuf[len++] = door_pos;
	pbuf[len++] = 1;
	pbuf[len++] = 0;
	pbuf[len++] = 0;
	pbuf[len++] = 0x00;
	pbuf[len++] = 0x02;
	pbuf[len++] = (door_cmd>>8)&0xFF;
	pbuf[len++] = door_cmd&0xFF;
	CalulateCRCbySoft(&buf[1],len-1,crc_cal);
	pbuf[len++] = crc_cal[1];	// 高位要在前
	pbuf[len++] = crc_cal[0];
	pbuf[len++] = 0x7E;
	pbuf[len++] = 0x7E;
	printf("zte Locker data begins\r\n ");
	for(int j=0;j<len;j++) printf("0x%02x ",pbuf[j]);printf("\r\n");

	return len;

}

UINT8 locker_cmd_pack(UINT8 msg_type,UINT8 door_pos,UINT8 *buf)
{
	/*取得目标串口对应的发送缓存*/
	UINT8 *pbuf = buf;
	UINT8 len = 0;
	//UINT8 crc_cal[2];
	UINT16 door_cmd = ZTE_DOOR_POLL;
	// 这2个金晟安门锁需要的命令码
	// 注意0x20对应的是空格,即不带管理员信息开锁命令
	UINT8 databuf[8] = {YDN_CMD_GRP_CTRL,YDN_CMD_TYPE_CTRL,YDN_CMD_DOOR_OPEN,' ',' ',' ',' ',' '};
	// 确权对应的comand group,type 分别为0xF0,0xE0，密码5个0
	UINT8 authorbuf[7] = {0xF0,0xE0,0,0,0,0,0};
	// 读取记录对应的comand group,type 分别为0xF2,0xE2
	UINT8 pollbuf[2] = {0xF2,0xE2};

	switch(msg_type)
	{
	case DOOR_ZTE_OPEN_CMD:
		door_cmd = ZTE_DOOR_OPEN;
		len = zte_cmd_pack(door_pos,door_cmd,buf);
		break;

	case DOOR_ZTE_POLL_CMD:
		door_cmd = ZTE_DOOR_POLL;
		len = zte_cmd_pack(door_pos,door_cmd,buf);
		break;

	case DOOR_JSA_POLL_CMD:
		len = comm_pack_ydn(door_pos,YDN_CID2_POLL,2,pollbuf,buf);
		printf("JSA poll begins\r\n ");
		for(int j=0;j<len;j++) printf("%02x ",pbuf[j]);printf("\r\n");
		break;
	case DOOR_JSA_OPEN_CMD:
		// 开锁命令需要先确权
		len = comm_pack_ydn(door_pos,YDN_CID2_AUTH,7,authorbuf,buf);
		printf("JSA author begins\r\n ");
		for(int j=0;j<len;j++) printf("%02x ",pbuf[j]);printf("\r\n");
		usleep(100000);		// 延时100ms
		len = comm_pack_ydn(door_pos,YDN_CID2_CTRL,8,databuf,buf);
		printf("JSA cmd begins\r\n ");
		for(int j=0;j<len;j++) printf("%02x ",pbuf[j]);printf("\r\n");
		break;

	default:
	break;
	}

	return len;
}


// 字符串排序,冒泡法
void StrBubbleSort(string *arr, int size)
{
    int i, j;
	string tmp;
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - i - 1; j++) {
            if (atoi(arr[j].c_str()) > atoi(arr[j+1].c_str())) {
                tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
}



// 上传卡号钩子函数
void zte_locker_id_send_hook(int seq,UINT64 card_id)
{
	string jsonstr;
    string mstrkey = ""; //没有用户名和密码：则为“”；
	REMOTE_CONTROL *pRCtrl;
	int ret;

	// 注意上报上去的ID是1,2,257,258，要区别一下
	// 语句：if(cabineid==1 && operate==ACT_UNLOCK) pRCtrl->FrontDoorCtrl=ACT_UNLOCK;
	SetjsonDealLockerStr64(NETCMD_DEAL_LOCKER,card_id,lockerHw_Param[seq]->address,jsonstr);
	printf("DealLockerMsg jsonstr=%s\n" ,jsonstr.c_str());
	NetSendParm(NETCMD_DEAL_LOCKER, (char*)jsonstr.c_str(), jsonstr.length());
	if(StrServerURL4.length()>0)			//上报利通后台
	{
		ret=zteHttpPostParm(StrServerURL4,jsonstr,mstrkey,HTTPPOST,"","",15);
		if(ret)
		{
			pRCtrl=stuRemote_Ctrl;
			memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
			jsonstrRCtrlReader((char*)jsonstr.c_str(),jsonstr.length(),(UINT8 *)pRCtrl);//将json字符串转换成结构体
			RemoteControl((UINT8*)pRCtrl);
		}
		else
			printf("LTKJ HttpPostParm error, ret=%d\n",ret);
	}
	if(StrServerURL2.length()>0)			//上报新粤后台
	{
		ret=zteHttpPostParm(StrServerURL2,jsonstr,mstrkey,HTTPPOST,"","",15);
		if(ret)
		{
			pRCtrl=stuRemote_Ctrl;
			memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
			jsonstrRCtrlReader((char*)jsonstr.c_str(),jsonstr.length(),(UINT8 *)pRCtrl);//将json字符串转换成结构体
			RemoteControl((UINT8*)pRCtrl);
		}
		else
			printf("XY HttpPostParm error, ret=%d\n",ret);
	}
}

#if 0
// 门锁开关命令,seq为锁的序号，pSend为清零的缓存
void zte_locker_ctrl_process(int seq,UINT8 cmd,UINT8 *pSend,string strDigestUser,string strDigestKey)
{
	string lockerurl;
	string locker_data; // data部分
	string mStrdata = "";
	UINT8 lock_len = 0;
	int i = 0;
	UINT8 msg_t = DOOR_ZTE_OPEN_CMD;
	int door_addr = 1;

	// 因为2个柜子的前后门地址都是1,2，我们实际设置时要设1,2, 257,258,然后屏蔽掉高8位
	door_addr = lockerHw_Param[seq]->address&0x00FF;
	//cout<<"dooraddr"<<seq<<"="<<door_addr<<endl;
	// 组包锁的轮询协议, 如果是锁0,2是前门，1,3是后门

	if (CabinetTypeGet() == CABIN_ZTE)
	{
		lockerurl = "http://"+StrHWServer+"/jscmd/serialsend";
	}
	// 金圣安有2个柜子IP，需要区别对待
	else if (CabinetTypeGet() == CABIN_JSA)
	{
		lockerurl = "http://"+StrHWServer+"/jscmd/serialsend";
	}
	else
	{
		return;
	}

	// 如果是锁0,2是前门
	// 如果是锁1,3则是后门
	msg_t = cmd;
	lock_len = locker_cmd_pack(msg_t,door_addr,DOOR_OPEN,pSend);
	locker_data= Base64Cal.Encode(pSend, lock_len); // 加密
	printf("lockerSecretOpen = %s\r\n",locker_data.c_str());
	mStrdata = "{\"jsonrpc\":\"2.0\",\"method\":\"POST_METHOD\",\"id\":\"3\",\"params\":{\"objid\":\""+zteLockDevID[seq]+"\",\"data\":\""+locker_data+"\",\"endchar\":\"\"}}";
	printf("lockerOpen = %s\r\n",mStrdata.c_str());
	zteHttpPostParm(lockerurl,mStrdata,"",HTTPPOST,strDigestUser,strDigestKey,15);	// 要不要再发一次?
}
#endif


// 根据锁的ID获取其配置的COM口，从而判断ID是设备柜还是电源柜
// 把COM5口对应的ID号放至zteLockDevID[0]/[1]，其它的放至[2]/[3]
bool jsonzteLockerComGet(char* jsonstr, int len)
{
    //printf("%s \r\n",jsonstr);

	UINT8 i=0,j=0,k=0,m=0;
  Json::Reader reader;
  Json::Value json_object;


  //if (!reader.parse(jsonstr, json_object))

  //  return 0;

 // string strentity = json_object["result"].toStyledString() ;
  //if(strentity.size() > 3)
  {
     //printf("result:%s\r\n",strentity.c_str()) ;
     Json::Reader reader_Objs;
     Json::Value json_object_Objs;
     if (!reader_Objs.parse(jsonstr, json_object_Objs))
       return 0;
      string strObjs = json_object_Objs["result"].toStyledString() ;
      if(strObjs.size() > 3)
      {
         //printf("Objs:%s\r\n",strObjs.c_str()) ;
         Json::Value::Members mem = json_object_Objs.getMemberNames();
		 // 这里取成员是result下面有几个大括号成员，这里只有1个
         for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
         {
         	 printf("===========:%d\r\n",m++);	//这里会打印4次，没理解清楚
         	 // 这里取成员是Objs下面有几个中括号，这里只有n个
             for(int n=0;n<json_object_Objs[*iter].size();n++)
             {
                 string strTemplate = json_object_Objs[*iter][n].toStyledString();


                 //printf("strTemplate:%s\r\n",strTemplate.c_str());
                 //解析设备列表具体协议
                 {
                     Json::Reader reader_devs;
                     Json::Value json_object_devs;
                     if (!reader_devs.parse(strTemplate, json_object_devs))
                       return 0;

                     string strObjId = "";
					 string strPortId = "";
                     Json::Value::Members memdev = json_object_devs.getMemberNames();

					 // 这里取中括号下面大括号项目的每1行
                     for (Json::Value::Members::iterator iterdev = memdev.begin(); iterdev != memdev.end(); iterdev++)
                     {
                         string newstring;
                         stringstream ss;
                         switch (json_object_devs[*iterdev].type())
                         {
                         case Json::nullValue:
                             break;
                         case Json::booleanValue:
                             break;
                         case Json::intValue:
                             ss<<(json_object_devs[*iterdev].asInt());
                             newstring = ss.str();
                             break;
                         case Json::uintValue:
                             ss<<(json_object_devs[*iterdev].asUInt());
                             newstring = ss.str();
                             break;
                         case Json::realValue:
                             ss<<(json_object_devs[*iterdev].asDouble());
                             newstring = ss.str();
                             break;
                         case Json::stringValue:
                             newstring = json_object_devs[*iterdev].asString();
                             break;
                         default:
                             break;
                         }

						  if((*iterdev) == "objId")
                             strObjId = newstring ;
                         else if((*iterdev) == "portId")
                             strPortId = newstring ;

                    }
					// 1个成员结束，判断是否为锁
					for (i = 0; i < LOCK_MAX_NUM; i++)
					{
						if (zteLockDevParam[i].lockDevID == strObjId)
						{
							zteLockDevParam[i].portID = strPortId;
							cout<<"lockID"<<i<<"=="<<zteLockDevParam[i].lockDevID<<endl;
							cout<<"portID"<<i<<"=="<<zteLockDevParam[i].portID<<endl;
						}
					}

                 }
             }
         }
		 for (i=0,j=0,k=2; i < LOCK_MAX_NUM; i++)
		 {
		 	// COM5是接设备柜，对应zteLockDevID 0,1号
		 	if (zteLockDevParam[i].portID == "0_COM5")
		 	{
		 		if (j < LOCK_MAX_NUM)
		 		{
					zteLockDevID[j] = zteLockDevParam[i].lockDevID;
					j++;
		 		}
		 	}
			// COM3是接电源柜，对应zteLockDevID 2,3号
			else
			{
				if (k < LOCK_MAX_NUM)
				{
					zteLockDevID[k] = zteLockDevParam[i].lockDevID;
					k++;
				}
			}
		 }
		 for (i=0; i < LOCK_MAX_NUM; i++)
		 {
		 	cout<<"postID"<<i<<"="<<zteLockDevID[i]<<endl;
		 }

      }

  }

  //std::cout << json_object["age"] << std::endl;

return true ;

}


#define CARD_ID_POS 	13		// 定义互信互达卡ID的位置
// 读卡后数据解析
bool card_id_parse(char *buf,int len,int seq)
{
	UINT8 *p_read = (UINT8 *)buf;
	UINT64 card_id64 = 0;
	UINT32 card_id = 0;
	UINT16 buf_len = 0;
	UINT8 *pbuf = (UINT8 *)buf;
	UINT16 temp = 0;
	UINT8 remark = 0;
	UINT8 c_value[4] = {0,0,};

	#if(CABINETTYPE == 5) //中兴
	{
		if ((p_read[0] == 0x7E) &&(p_read[DOOR_EOI_ADDR] == 0x7E))	// 锁的协议以0x7E开始，0x7E结束
		{
			if (seq < LOCK_MAX_NUM)	// 防止泄露
			{
				lockerHw_Param[seq]->status = p_read[DOOR_STATUS_ADDR];
				memcpy(&lockerHw_Param[seq]->id[1],&p_read[DOOR_ID_ADDR],4);
				printf("status = %d\r\n",lockerHw_Param[seq]->status);
				//printf("lockerHw_Param[seq]->id = ");
				//for (int i = 0 ; i< 5; i++)
				//{
				//	printf("%02x  ",card_read[DOOR_STATUS_ADDR+i]);
				//}
				// 这个锁读到的卡号是逆序的，比如0xD6E9C160的卡(3605643616)读出来是60 c1 e9 d6
				char_to_long_reverse(&(lockerHw_Param[seq]->id[1]),&card_id);
				printf("  lock_id = %d\r\n",card_id);
				if (card_id != 0)
				{
					zte_locker_id_send_hook(seq,card_id);
					return true;
				}
			}
		}
	}
	#elif (CABINETTYPE == 6) //金晟安
	{
		buf_len = strlen(buf);
		temp  = checkSumCalc(pbuf+1, buf_len-1);
		// 对收到的数据进行校验，1是长度，1是校验码
		/*
		7E
		31 30 	// VER
		30 31 	// ADR
		38 30 	// CID1
		30 30 	// RTN
		33 30 31 43 // L.TH
		30 30 44 36 45 39 43 31 36 30 	// CARD ID
		32 30 31 39 31 32 30 35 31 35 31 32 33 37 // DATA-TIME
		30 38 	// STATUS
		30 38 	// remark
		46 37 44 36 // SUM
		0D
		*/
		// 锁的状态没法读?
		if ((buf_len == 46) && (temp == ascii_to_hex4(pbuf+buf_len-5)))
		{
			remark = ascii_to_hex2(pbuf+buf_len-7);		// remark字段
			// REMARK字段，8: 无效卡        9:有效期过期       10：当前时间无进入权限            0：刷卡记录开门
			if ((remark == 8) || (remark == 9) ||(remark == 10))
			{
				if (seq < LOCK_MAX_NUM)	// 防止泄露
				{
					lockerHw_Param[seq]->id[1] = 0;
					lockerHw_Param[seq]->id[2] = 0;
					lockerHw_Param[seq]->id[3] = 0;
					lockerHw_Param[seq]->id[4] = ascii_to_hex2(pbuf+CARD_ID_POS);
					lockerHw_Param[seq]->id[5] = ascii_to_hex2(pbuf+CARD_ID_POS+2);
					lockerHw_Param[seq]->id[6] = ascii_to_hex2(pbuf+CARD_ID_POS+4);
					lockerHw_Param[seq]->id[7] = ascii_to_hex2(pbuf+CARD_ID_POS+6);
					lockerHw_Param[seq]->id[8] = ascii_to_hex2(pbuf+CARD_ID_POS+8);
					// ID卡是有5字节的
					char_to_longlong(&(lockerHw_Param[seq]->id[1]),&card_id64);
					if (card_id64 != 0)
					{
						zte_locker_id_send_hook(seq,card_id64);
						return true;
					}
				}
			}
		}
	}
	#endif
	return false;
}


// 锁状态
bool jsonzteLockerReader(char* jsonstr, int len, int seq)
{
    //printf("%s \r\n",jsonstr);


  Json::Reader reader;

  Json::Value json_object;

  string data_read;
  char card_read[BASE64_HEX_LEN];	// c语言中的字符串
  bool reval = false;
  int real_len = 0;

  //const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\"}";

  if (!reader.parse(jsonstr, json_object))

    return 0;

  string strObjs = json_object["result"].toStyledString() ;
  if(strObjs.size() > 3)
  {
          int TempTick = 0;
          int LockTick = 0;
         //printf("Objs:%s\r\n",strObjs.c_str()) ;
        // Json::Value::Members mem = json_object.getMemberNames();
        // for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
        // {
             //for(int n=0;n<json_object[*iter].size();n++)
            // {
                 //string strTemplate = json_object[*iter][n].toStyledString();


                 //printf("strTemp:%s\r\n",strTemplate.c_str());
                 //解析设备列表具体协议
                 {
                     Json::Reader reader_devs;
                     Json::Value json_object_devs;
                     if (!reader_devs.parse(strObjs, json_object_devs))
                       return 0;

                     string strgetDeviceId = "";
                     string strObjId = "";
					 string strData = "";
                     Json::Value::Members memdev = json_object_devs.getMemberNames();

                     for (Json::Value::Members::iterator iterdev = memdev.begin(); iterdev != memdev.end(); iterdev++)
                     {
                         string newstring;
                         stringstream ss;
                         switch (json_object_devs[*iterdev].type())
                         {
                         case Json::nullValue:
                             break;
                         case Json::booleanValue:
                             break;
                         case Json::intValue:
                             ss<<(json_object_devs[*iterdev].asInt());
                             newstring = ss.str();
                             break;
                         case Json::uintValue:
                             ss<<(json_object_devs[*iterdev].asUInt());
                             newstring = ss.str();
                             break;
                         case Json::realValue:
                             ss<<(json_object_devs[*iterdev].asDouble());
                             newstring = ss.str();
                             break;
                         case Json::stringValue:
                             newstring = json_object_devs[*iterdev].asString();
                             break;
                         default:
                             break;
                         }

                        // json_object_devs[*iterdev].toStyledString();
                         //printf("newstring:%s,%s\r\n",newstring.c_str(),(*iterdev).c_str());
                         if((*iterdev) == "objid")
                             strObjId = newstring ;
                         else if((*iterdev) == "data")
                             strData = newstring ;
                     }

					printf("strData = %s\n\r",strData.c_str());

					memset(card_read,0,sizeof(card_read));
					base64_2_Hex_decode(strData.c_str(), strData.size(),(unsigned char *)card_read);
					//printf("card_read = %s\n\r",card_read);	// 这样输出因为有0很快就结束了
					printf(" card_read[i] = ");
					for (int i = 0 ; i< 30; i++)
					{
					 	//card_read[i] = atoi(&card_read[i]);
						printf(" %02x",card_read[i]);
					}

                    if(strObjId == zteLockDevID[seq]) //锁1
                    {
                    	reval = card_id_parse(card_read,BASE64_HEX_LEN,seq);
						return reval;
                    }
                 }
             //}
        // }

  }

  //std::cout << json_object["age"] << std::endl;

  return false;

}

#if 0
// 中兴锁的轮询函数
bool zte_locker_polling(int seq,UINT8 *pSend,string strDigestUser,string strDigestKey)
{
	string lockerurl;
	string locker_data;	// data部分
	string mStrdata = "";
	UINT8 lock_len = 0;
	int i = 0;
	bool re_val=false;
	UINT8 msg_t = DOOR_ZTE_POLL_CMD;
	int door_addr = 1;

	// 因为2个柜子的前后门地址都是1,2，我们实际设置时要设1,2, 257,258,然后屏蔽掉高8位
	door_addr = lockerHw_Param[seq]->address&0x00FF;
	//cout<<"dooraddr"<<seq<<"="<<door_addr<<endl;
	// 组包锁的轮询协议, 如果是锁0,2是前门，1,3是后门

	if (CabinetTypeGet() == CABIN_ZTE)
	{
		msg_t = DOOR_ZTE_POLL_CMD;
		lockerurl = "http://"+StrHWServer+"/jscmd/serialsend";
	}
	// 金圣安有2个柜子IP，需要区别对待
	else if (CabinetTypeGet() == CABIN_JSA)
	{
		msg_t = DOOR_JSA_POLL_CMD;
		lockerurl = "http://"+StrHWServer+"/jscmd/serialsend";
	}
	else
	{
		return false;
	}


	lock_len = locker_cmd_pack(msg_t,door_addr,pSend);
	locker_data= Base64Cal.Encode(pSend, lock_len);	// 加密
	printf("lockerSecret = %s\r\n",locker_data.c_str());
	mStrdata = "{\"jsonrpc\":\"2.0\",\"method\":\"POST_METHOD\",\"id\":\"3\",\"params\":{\"objid\":\""+zteLockDevID[seq]+"\",\"data\":\""+locker_data+"\",\"endchar\":\"\"}}";
	printf("lockerPolling = %s\r\n",mStrdata.c_str());
	zteHttpPostParm(lockerurl,mStrdata,"",HTTPPOST,strDigestUser,strDigestKey,15);
	if(mStrdata != "")
	{
		re_val = jsonzteLockerReader((char *)(mStrdata.c_str()),mStrdata.size(),seq);
	}
	return re_val;
}
#endif


// 中兴金圣安锁的统一处理函数
bool zte_jsa_locker_process(int seq,UINT8 msg_type,UINT8 *pSend,string strDigestUser,string strDigestKey)
{
	string lockerurl;
	string locker_data;			// data部分
	string mStrdata = "";
	UINT8 lock_len = 0;
	int i = 0;
	bool re_val=false;
	UINT8 msg_div = msg_type;	// 把统一的命令分开
	int door_addr = 1;

	pthread_mutex_lock(&lockerwriteMutex);
	// 因为2个柜子的前后门地址都是1,2，我们实际设置时要设1,2, 257,258,然后屏蔽掉高8位
	door_addr = lockerHw_Param[seq]->address&0x00FF;
	//cout<<"dooraddr"<<seq<<"="<<door_addr<<endl;
	// 组包锁的轮询协议, 如果是锁0,2是前门，1,3是后门

	#if(CABINETTYPE == 5) //中兴
	{
		lockerurl = "http://"+StrHWServer+"/jscmd/serialsend";
		if (msg_type == DOOR_OPEN_CMD)
		{
			msg_div = DOOR_ZTE_OPEN_CMD;
		}
		else if (msg_type == DOOR_CLOSE_CMD)
		{
			msg_div = DOOR_ZTE_CLOSE_CMD;
		}
		else if (msg_type == DOOR_POLL_CMD)
		{
			msg_div = DOOR_ZTE_POLL_CMD;
		}
	}
	#elif (CABINETTYPE == 6) //金晟安
	// 金圣安有2个柜子IP，需要区别对待
	{
		lockerurl = "http://"+StrHWServer+"/jscmd/serialsend";
		if (msg_type == DOOR_OPEN_CMD)
		{
			msg_div = DOOR_JSA_OPEN_CMD;
		}
		else if (msg_type == DOOR_CLOSE_CMD)
		{
			msg_div = DOOR_JSA_CLOSE_CMD;
		}
		else if (msg_type == DOOR_POLL_CMD)
		{
			msg_div = DOOR_JSA_POLL_CMD;
		}
	}
	#else
	{
		return false;
	}
	#endif


	lock_len = locker_cmd_pack(msg_div,door_addr,pSend);
	locker_data= Base64Cal.Encode(pSend, lock_len);	// 加密
	printf("lockerSecret = %s\r\n",locker_data.c_str());
	mStrdata = "{\"jsonrpc\":\"2.0\",\"method\":\"POST_METHOD\",\"id\":\"3\",\"params\":{\"objid\":\""+zteLockDevID[seq]+"\",\"data\":\""+locker_data+"\",\"endchar\":\"\"}}";
	printf("lockertype=%d ---> %s\r\n",msg_type,mStrdata.c_str());
	zteHttpPostParm(lockerurl,mStrdata,"",HTTPPOST,strDigestUser,strDigestKey,15);
	if(mStrdata != "")
	{
		re_val = jsonzteLockerReader((char *)(mStrdata.c_str()),mStrdata.size(),seq);
	}
	pthread_mutex_unlock(&lockerwriteMutex);

	return re_val;
}


int zteDevIDInit(void)
{
   zteTempDevID[0] == "";
   zteTempDevID[1] == "";
   zteLockDevID[0] = zteLockDevID[1] = zteLockDevID[2] = zteLockDevID[3] = "";
   ztewaterDevID = "";
   zteDoorDevID[0] = "";
   zteDoorDevID[1] = "";
   zteAirDevID[0] = "";
   zteAirDevID[1] = "";
   zteSwitchPowerDevID = "";
   return 0;
}


#endif



#if(CABINETTYPE == 5) //中兴

void *zte_HTTP_thread(void *param)
{

    string mStrdata = "";
    string mStrHWServer = "";
    string mStrUser = "" ;
    string mStrkey = "" ;
	string getStrHWServer;
	static UINT16 poll_cnt = 10;

    sleep(5);

    int zteCabinetType = 0;

	UINT8 byteSend[BASE64_HEX_LEN]={0x00,};
	UINT8 lock_len = 0;
	//bool isCardExist[4] = {false,false,false,false};

	zteDevIDInit();
	mStrHWServer = StrHWServer;
    mStrUser = StrHWGetPasswd;
    mStrkey = StrHWSetPasswd;

	sleep(2);
	//设备列表获取
	// 2s后获取电子锁对应的端口号，COM5->设备柜,COM3->电源柜
    mStrdata = "";
    getStrHWServer = "http://"+mStrHWServer+"/jscmd/objs";
    zteHttpPostParm(getStrHWServer,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
    if(mStrdata != "")
       jsonzteDevReader((char *)(mStrdata.c_str()),mStrdata.size());

	sleep(2);
	// 获取电子锁对应的端口号，COM5->设备柜,COM3->电源柜
	getStrHWServer = "http://"+StrHWServer+"/jscmd/sensorports";
	cout<<"agin = "<<getStrHWServer<<endl;
	zteHttpPostParm(getStrHWServer,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
	// 先初始化, 初始化不能放在开始那里，否则取不到数据
	for (int i = 0; i < LOCK_MAX_NUM; i++)
	{
		zteLockDevParam[i].lockDevID = zteLockDevID[i];
	}
	jsonzteLockerComGet((char *)(mStrdata.c_str()),mStrdata.size());

    while(1)
    {
    	//设备列表若未获取到，则再获取
        if(zteTempDevID[0] == "")
        {
        	sleep(2);
            mStrdata = "";
		    getStrHWServer = "http://"+mStrHWServer+"/jscmd/objs";
		    zteHttpPostParm(getStrHWServer,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
		    if(mStrdata != "")
		       jsonzteDevReader((char *)(mStrdata.c_str()),mStrdata.size());

			sleep(2);
			// 获取电子锁对应的端口号，COM5->设备柜,COM3->电源柜
			getStrHWServer = "http://"+StrHWServer+"/jscmd/sensorports";
			cout<<"agin = "<<getStrHWServer<<endl;
			zteHttpPostParm(getStrHWServer,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
			// 先初始化, 初始化不能放在开始那里，否则取不到数据
			for (int i = 0; i < LOCK_MAX_NUM; i++)
			{
				zteLockDevParam[i].lockDevID = zteLockDevID[i];
			}
			jsonzteLockerComGet((char *)(mStrdata.c_str()),mStrdata.size());
        }
		else
    	{
    		if (poll_cnt++ >= 10)
    		{
    		  poll_cnt = 0;
        	  mStrdata = "";
			  string tempid1;
			  //printf("zteTempDevID[0]:%s,zteTempDevID[1]:%s\r\n",zteTempDevID[0].c_str(),zteTempDevID[1].c_str());
			  for(int n=0;n<2;n++)
			  {
                 if(zteTempDevID[n] != "")
                 {
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/objdataquery?objId="+zteTempDevID[n]+"&mId=0";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                     jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
                   //获取高温阀值
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118202001";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                      jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118202001);
                   //获取低温阀值
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118203001";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                      jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118203001);
                   //获取高湿阀值
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118206001";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                      jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118206001);
                   //获取低湿阀值
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118205001";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                     jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118205001);
               }
            }

            //获取普通空调信息
            for(int n=0;n<2;n++)
            {
                   if(zteAirDevID[n] != "")
                   {
                      mStrdata = "";
                      tempid1 = "http://"+mStrHWServer+"/jscmd/objdataquery?objId="+zteAirDevID[n]+"&mId=0";
                      zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                      if(mStrdata != "")
                        jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
                   }
             }


                //获取锂电池信息
                for(int n=0;n<4;n++)
                {
                     if(zteBatDevID[n] != "")
                     {
                         mStrdata = "";
                         tempid1 = "http://"+mStrHWServer+"/jscmd/objdataquery?objId="+zteBatDevID[n]+"&mId=0";
                         zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                         if(mStrdata != "")
                           jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
                     }
                }


                //获取开关电源信息


                //获取告警信息
                {
                     mStrdata = "";
                     tempid1 = "http://"+mStrHWServer+"/jscmd/alarmquery?type=now";
                     zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                     if(mStrdata != "")
                        jsonzteAlarmReader((char *)(mStrdata.c_str()),mStrdata.size());
                }
    		  }


			// 获取锁的卡号和状态
			// 先进行排序,数字小的一组22545/22546接设备柜前/后门, 数字大一组23042/23043的接电池柜前/后门
			// 同时数字小的为前门22545、23042，数字大的为后门22546/23043
			//StrBubbleSort(zteLockDevID,4);
			printf("lockerID0 = %s\r\n",zteLockDevID[0].c_str());
			printf("lockerID1 = %s\r\n",zteLockDevID[1].c_str());
			printf("lockerID2 = %s\r\n",zteLockDevID[2].c_str());
			printf("lockerID3 = %s\r\n",zteLockDevID[3].c_str());
			for(int n=0;n<LOCK_MAX_NUM;n++)
			{
				mStrdata = "";
				if (zteLockDevID[n] != "")
				{
					// 组包锁的轮询协议, 设备柜前门/后门
					memset(byteSend,0,BASE64_HEX_LEN);
					// 轮询顺序安装zteLockDevID下标来
					if (zteLockDevID[n] != "" )	// 如果是空的，则会得到无尽的回应，导致崩溃
					{
						if (zte_jsa_locker_process(n,DOOR_POLL_CMD,byteSend,mStrUser,mStrkey))
						{
							//memset(byteSend,0,BASE64_HEX_LEN);
							// 测试用，只要刷卡有卡号，就开锁
							//zte_jsa_locker_process(n,DOOR_OPEN_CMD,byteSend,mStrUser,mStrkey);
						 }
					}
				}
			}
            sleep(1);
         }
    }
    return 0 ;

}

#elif (CABINETTYPE == 6) //金晟安
void *jsa_HTTP_thread(void *param)
{

    string mStrdata = "";
    string mStrHWServer = "";
    string mStrUser = "" ;
    string mStrkey = "" ;
    sleep(5);

    int zteCabinetType = 0;

    UINT8 byteSend[BASE64_HEX_LEN]={0x00,};
    UINT8 lock_len = 0;
    bool isCardExist[4] = {false,false,false,false};
    while(1)
    {

        zteDevIDInit();

        {
           mStrHWServer = StrHWServer;
           mStrUser = StrHWGetPasswd;
           mStrkey = StrHWSetPasswd;

           //设备列表获取
           {
              mStrdata = "";
              string getStrHWServer = "http://"+mStrHWServer+"/jscmd/objs";
              zteHttpPostParm(getStrHWServer,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
              if(mStrdata != "")
                jsonzteDevReader((char *)(mStrdata.c_str()),mStrdata.size());
           }

           {
              mStrdata = "";
              string tempid1;
              //printf("zteTempDevID[0]:%s,zteTempDevID[1]:%s\r\n",zteTempDevID[0].c_str(),zteTempDevID[1].c_str());
              for(int n=0;n<2;n++)
              {
                 if(zteTempDevID[n] != "")
                 {
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/objdataquery?objId="+zteTempDevID[n]+"&mId=0";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                     jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
                   //获取高温阀值
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118202001";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                      jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118202001);
                   //获取低温阀值
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118203001";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                      jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118203001);
                   //获取高湿阀值
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118206001";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                      jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118206001);
                   //获取低湿阀值
                   mStrdata = "";
                   tempid1 = "http://"+mStrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118205001";
                   zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                   if(mStrdata != "")
                     jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118205001);
                  }

               }


                //获取普通空调信息
                for(int n=0;n<2;n++)
                {
                   if(zteAirDevID[n] != "")
                   {
                      mStrdata = "";
                      tempid1 = "http://"+mStrHWServer+"/jscmd/objdataquery?objId="+zteAirDevID[n]+"&mId=0";
                      zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                      if(mStrdata != "")
                        jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
                   }
                }


                 //获取锂电池信息
                 for(int n=0;n<4;n++)
                 {
                     if(zteBatDevID[n] != "")
                     {
                         mStrdata = "";
                         tempid1 = "http://"+mStrHWServer+"/jscmd/objdataquery?objId="+zteBatDevID[n]+"&mId=0";
                         zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                         if(mStrdata != "")
                           jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
                     }
                  }


                  //获取开关电源信息


                  //获取告警信息
                  {
                     mStrdata = "";
                     tempid1 = "http://"+mStrHWServer+"/jscmd/alarmquery?type=now";
                     zteHttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                     if(mStrdata != "")
                        jsonzteAlarmReader((char *)(mStrdata.c_str()),mStrdata.size());
                  }


                  // 获取锁的卡号和状态
            // 先进行排序,数字小的一组22545/22546接设备柜前/后门, 数字大一组23042/23043的接电池柜前/后门
            // 同时数字小的为前门22545、23042，数字大的为后门22546/23043
            //StrBubbleSort(zteLockDevID,4);
                printf("lockerID0 = %s\r\n",zteLockDevID[0].c_str());
                printf("lockerID1 = %s\r\n",zteLockDevID[1].c_str());
                printf("lockerID2 = %s\r\n",zteLockDevID[2].c_str());
                printf("lockerID3 = %s\r\n",zteLockDevID[3].c_str());
                for(int n=0;n<LOCK_MAX_NUM;n++)
                {
                   mStrdata = "";
                   // 组包锁的轮询协议, 设备柜前门/后门
                   memset(byteSend,0,BASE64_HEX_LEN);
                   // 轮询顺序安装zteLockDevID下标来
                   if (zteLockDevID[n] != "" )	// 如果是空的，则会得到无尽的回应，导致崩溃
                   {
                       if (zte_jsa_locker_process(n,DOOR_POLL_CMD,byteSend,mStrUser,mStrkey))
                       {
                            memset(byteSend,0,BASE64_HEX_LEN);
                            // 测试用，只要刷卡有卡号，就开锁
                            zte_jsa_locker_process(n,DOOR_OPEN_CMD,byteSend,mStrUser,mStrkey);
                       }
                   }
                }


             }

             sleep(6);

         }

    }
    return 0 ;

}


#endif




int zteinit(void)
{
    //CABINETTYPE:作为区分机柜类型，用于编译不同的代码
    //CABINETTYPE  1：华为（包括华为单门 双门等） 5：中兴; 6：金晟安; 7：爱特斯 StrVersionNo
    #if(CABINETTYPE == 5) //中兴
       pthread_t m_zte_HTTP_Get ;
       pthread_create(&m_zte_HTTP_Get,NULL,zte_HTTP_thread,NULL);
    #elif (CABINETTYPE == 6) //金晟安
       pthread_t m_zte_HTTP_Get ;
       pthread_create(&m_zte_HTTP_Get,NULL,jsa_HTTP_thread,NULL);
    #endif
	   pthread_mutex_init(&lockerwriteMutex,NULL);


    return 0 ;
}




