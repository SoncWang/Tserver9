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


using namespace std;

extern string StrHWServer;		//华为服务器地址
extern string StrHWGetPasswd ;
extern string StrHWSetPasswd ;

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

//LOCKER_ZTE_PARAMS ZTE_Locker_Param[4];
extern LOCKER_HW_PARAMS *lockerHw_Param[LOCK_MAX_NUM];	//门锁状态结构体
Base64 Base64Cal;

extern pthread_mutex_t snmpoidMutex ;
extern THUAWEIGantry HUAWEIDevValue;
extern THUAWEIALARM HUAWEIDevAlarm;		//华为机柜告警


extern bool jsonzteDevReader(char* jsonstr, int len);//读取中兴机柜信息
extern bool jsonzteTempReader(char* jsonstr, int len,int Index);//读取中兴温湿度
void char_to_long(UINT8* buffer,UINT32* value);


string strzteAcbBatVolt[4];				//电池电压
string strzteAcbTotalCapacity[4];           //电池总容量
string strzteAcbTotalRemainCapacity[4];		//电池剩余容量
string strzteAcbTemperature[4];             //电池温度



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
UINT8 locker_cmd_pack(UINT8 msg_type,UINT8 door_pos,UINT16 door_cmd,UINT8 *buf)
{
	/*取得目标串口对应的发送缓存*/
	UINT8 *pbuf = buf;	//buf->pTxBuf;
	UINT8 len = 0;
	UINT8 crc_cal[2];
	// 注意0x20对应的是空格,即不带管理员信息开锁命令
	UINT8 databuf[8] = {YDN_CMD_GRP_CTRL,YDN_CMD_TYPE_CTRL,YDN_CMD_DOOR_OPEN,' ',' ',' ',' ',' '};
	// 确权对应的comand group,type 分别为0xF0,0xE0，密码5个0
	UINT8 authorbuf[7] = {0xF0,0xE0,0,0,0,0,0};

	switch(msg_type)
	{
	case DOOR_ZTE_OPEN_CMD:
	case DOOR_ZTE_POLL_CMD:
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
		printf("Locker cmd begins\r\n ");
		for(int j=0;j<len;j++) printf("0x%02x ",pbuf[j]);printf("\r\n");
		break;

	case DOOR_JSA_POLL_CMD:
		break;
	case DOOR_JSA_OPEN_CMD:
		len = comm_pack_ydn(1,YDN_CID2_AUTH,7,authorbuf,buf);
		printf("JSA author begins\r\n ");
		for(int j=0;j<len;j++) printf("%02x ",pbuf[j]);printf("\r\n");
		usleep(100000);		// 延时100ms
		len = comm_pack_ydn(1,YDN_CID2_CTRL,8,databuf,buf);
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



// 上传卡号钩子函数, 先保留
void zte_locker_id_send_hook(int seq)
{
	;
}

// 门锁开关命令,seq为锁的序号，pSend为清零的缓存
void zte_locker_ctrl_process(int seq,UINT8 *pSend,string strDigestUser,string strDigestKey)
{
	string lockerurl;
	string locker_data; // data部分
	string mStrdata = "";
	UINT8 lock_len = 0;
	int i = 0;

	// 组包锁的轮询协议, 设备柜前门
	if ((seq%2) == 0)
	{
		// 如果是锁0,2是前门
		lock_len = locker_cmd_pack(DOOR_ZTE_OPEN_CMD,FRONT_DOOR,DOOR_OPEN,pSend);
		//lock_len = locker_cmd_pack(DOOR_JSA_OPEN_CMD,FRONT_DOOR,DOOR_OPEN,pSend);

	}
	else
	{
		// 如果是锁1,3则是后门
		lock_len = locker_cmd_pack(DOOR_ZTE_OPEN_CMD,BACK_DOOR,DOOR_OPEN,pSend);
	}
	locker_data= Base64Cal.Encode(pSend, lock_len); // 加密
	printf("lockerSecretOpen = %s\r\n",locker_data.c_str());
	lockerurl = "http://128.8.82.246/jscmd/serialsend";
	mStrdata = "{\"jsonrpc\":\"2.0\",\"method\":\"POST_METHOD\",\"id\":\"3\",\"params\":{\"objid\":\""+zteLockDevID[seq]+"\",\"data\":\""+locker_data+"\",\"endchar\":\"\"}}";
	printf("lockerOpen = %s\r\n",mStrdata.c_str());
	HttpPostParm(lockerurl,mStrdata,"",HTTPPOST,strDigestUser,strDigestKey,15);	// 要不要再发一次?
}




// 锁状态
bool jsonzteLockerReader(char* jsonstr, int len, int seq)
{
    //printf("%s \r\n",jsonstr);


  Json::Reader reader;

  Json::Value json_object;

  string data_read;
  char card_read[BASE64_HEX_LEN];	// c语言中的字符串
  UINT32 card_id = 0;

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
                    	// 卡的回应:7E 01 01 00 00 00 0F 00     16 00 (状态) 00 00 00 00  （卡号） FF FF FF FF FF FF 00 00 EF 21  7E 7E
                     	if ((card_read[0] == 0x7E) &&(card_read[DOOR_EOI_ADDR] == 0x7E))	// 锁的协议以0x7E开始，0x7E结束
                     	{
                     		//if (seq < actual_locker_num)	// 防止泄露
                     		{
	                     		lockerHw_Param[seq]->status = card_read[DOOR_STATUS_ADDR];
								memcpy(&lockerHw_Param[seq]->id[1],&card_read[DOOR_ID_ADDR],4);
								printf("status = %d\r\n",lockerHw_Param[seq]->status);
								//printf("lockerHw_Param[seq]->id = ");
								//for (int i = 0 ; i< 5; i++)
								//{
								//	printf("%02x  ",lockerHw_Param[seq]->id[i]);
								//}
								char_to_long(&(lockerHw_Param[seq]->id[1]),&card_id);
								printf("  lock_id = %d\r\n",card_id);
								if (card_id != 0)
								{
									zte_locker_id_send_hook(seq);
									return true;
								}
                     		}
                     	}
                    }
                 }
             //}
        // }

  }

  //std::cout << json_object["age"] << std::endl;

  return false;

}


// 中兴锁的轮询函数
bool zte_locker_polling(int seq,UINT8 *pSend,string strDigestUser,string strDigestKey)
{
	string lockerurl;
	string locker_data;	// data部分
	string mStrdata = "";
	UINT8 lock_len = 0;
	int i = 0;
	bool re_val=false;

	// 组包锁的轮询协议, 设备柜前门
	if ((seq%2) == 0)
	{
		// 如果是锁0,2是前门
		lock_len = locker_cmd_pack(DOOR_ZTE_POLL_CMD,FRONT_DOOR,DOOR_POLL,pSend);
	}
	else
	{
		// 如果是锁1,3则是后门
		lock_len = locker_cmd_pack(DOOR_ZTE_POLL_CMD,BACK_DOOR,DOOR_POLL,pSend);
	}
	locker_data= Base64Cal.Encode(pSend, lock_len);	// 加密
	printf("lockerSecret = %s\r\n",locker_data.c_str());
	lockerurl = "http://128.8.82.246/jscmd/serialsend";
	mStrdata = "{\"jsonrpc\":\"2.0\",\"method\":\"POST_METHOD\",\"id\":\"3\",\"params\":{\"objid\":\""+zteLockDevID[seq]+"\",\"data\":\""+locker_data+"\",\"endchar\":\"\"}}";
	printf("lockerPolling = %s\r\n",mStrdata.c_str());
	HttpPostParm(lockerurl,mStrdata,"",HTTPPOST,strDigestUser,strDigestKey,15);
	if(mStrdata != "")
	{
		re_val = jsonzteLockerReader((char *)(mStrdata.c_str()),mStrdata.size(),seq);
	}
	return re_val;
}


void *zte_HTTP_thread(void *param)
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

    string mStrdata = "";

    string mStrUser = StrHWGetPasswd;
    string mStrkey = StrHWSetPasswd;
    sleep(5);
    //StrHWServer
   // HttpPostParm("http://128.8.82.238/jscmd/objs",mStrdata,"",HTTPGET,mStrUser,mStrkey);
    string getStrHWServer = "http://"+StrHWServer+"/jscmd/objs";
    HttpPostParm(getStrHWServer.c_str(),mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
    jsonzteDevReader((char *)(mStrdata.c_str()),mStrdata.size());

    while(1)
    {
         mStrUser = StrHWGetPasswd;
         mStrkey = StrHWSetPasswd;

        //设备列表若未获取到，则再获取
        if(zteTempDevID[0] == "")
        {
            mStrdata = "";
            getStrHWServer = "http://"+StrHWServer+"/jscmd/objs";
            HttpPostParm(getStrHWServer,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
            //HttpPostParm("http://128.8.82.238/jscmd/objs",mStrdata,"",HTTPGET,mStrUser,mStrkey);
           if(mStrdata != "")
              jsonzteDevReader((char *)(mStrdata.c_str()),mStrdata.size());
        }
        else
        {
            mStrdata = "";
            string tempid1;
            //string tempid2;
/*            if(atoi(zteTempDevID[0].c_str()) > atoi(zteTempDevID[1].c_str()))
            {
                tempid1 =  zteTempDevID[1];
                tempid2 =  zteTempDevID[0];
            }
            else
            {
                tempid2 =  zteTempDevID[1];
                tempid1 =  zteTempDevID[0];
            }
            zteTempDevID[0] = tempid1;
            zteTempDevID[1] = tempid2;
*/
            printf("zteTempDevID[0]:%s,zteTempDevID[1]:%s\r\n",zteTempDevID[0].c_str(),zteTempDevID[1].c_str());

           //获取温湿度
          for(int n=0;n<2;n++)
          {
            if(zteTempDevID[n] != "")
            {
                mStrdata = "";
              tempid1 = "http://"+StrHWServer+"/jscmd/objdataquery?objId="+zteTempDevID[n]+"&mId=0";
              //tempid1 = "http://128.8.82.238/jscmd/objdataquery?objId="+zteTempDevID[n]+"&mId=0";
              HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
              if(mStrdata != "")
                jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
              //http://128.8.82.239/jscmd/getAlarmMeteCfg?deviceId=22549&meteId=118202001
              //获取高温阀值
              mStrdata = "";
              tempid1 = "http://"+StrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118202001";
              //tempid1 = "http://128.8.82.238/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118202001";
              HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
              if(mStrdata != "")
                jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118202001);
              //获取低温阀值
              mStrdata = "";
              tempid1 = "http://"+StrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118203001";
              //tempid1 = "http://128.8.82.238/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118203001";
              HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
              if(mStrdata != "")
                jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118203001);
              //获取高湿阀值
              mStrdata = "";
              tempid1 = "http://"+StrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118206001";
              //tempid1 = "http://128.8.82.238/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118206001";
              HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
              if(mStrdata != "")
                jsonzteTempHLReader((char *)(mStrdata.c_str()),mStrdata.size(),n,118206001);
              //获取低湿阀值
              mStrdata = "";
              tempid1 = "http://"+StrHWServer+"/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118205001";
              //tempid1 = "http://128.8.82.238/jscmd/getAlarmMeteCfg?deviceId="+zteTempDevID[n]+"&meteId=118205001";
              HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
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
                tempid1 = "http://"+StrHWServer+"/jscmd/objdataquery?objId="+zteAirDevID[n]+"&mId=0";
                //tempid1 = "http://128.8.82.238/jscmd/objdataquery?objId="+zteAirDevID[n]+"&mId=0";
                HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
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
                tempid1 = "http://"+StrHWServer+"/jscmd/objdataquery?objId="+zteBatDevID[n]+"&mId=0";
                //tempid1 = "http://128.8.82.238/jscmd/objdataquery?objId="+zteBatDevID[n]+"&mId=0";
                HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                if(mStrdata != "")
                  jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
              }

          }


          //获取UPS信息
          for(int n=0;n<2;n++)
          {
              if(zteUPSDevID[n] != "")
              {
                mStrdata = "";
                tempid1 = "http://"+StrHWServer+"/jscmd/objdataquery?objId="+zteUPSDevID[n]+"&mId=0";
                //tempid1 = "http://128.8.82.238/jscmd/objdataquery?objId="+zteUPSDevID[n]+"&mId=0";
                HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
                if(mStrdata != "")
                  jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
              }

          }



          //获取开关电源信息




           //获取告警信息
            {
              mStrdata = "";
              tempid1 = "http://"+StrHWServer+"/jscmd/alarmquery?type=now";
              //tempid1 = "http://128.8.82.238/jscmd/alarmquery?type=now";
              HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey,15);
              if(mStrdata != "")
                jsonzteAlarmReader((char *)(mStrdata.c_str()),mStrdata.size());
            }



        }

        sleep(60);
    }
     return 0 ;
}

int zteinit(void)
{
    pthread_t m_zte_HTTP_Get ;
    pthread_create(&m_zte_HTTP_Get,NULL,zte_HTTP_thread,NULL);

    return 0 ;
}

