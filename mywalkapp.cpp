#include <net-snmp/net-snmp-config.h>  
#include <net-snmp/net-snmp-includes.h>  
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <string>
#include "snmp.h"
#include <pthread.h>
#include <vector>
#include <sstream> 
#include <iostream>
#include "snmp.h"
#include "registers.h"

using namespace std;

int mywalkappinit();
oid     objid_mib[] = { 1, 3, 6, 1, 2, 1 } ;  
int     numprinted = 0;  
  
int c;  
int count_print = 0;  

pthread_mutex_t WalksnmpoidMutex ;
pthread_mutex_t SetsnmpoidMutex ;
pthread_mutex_t snmpdataMutex ;
extern int snmpdataTime ;

vector<string> vecWalkSnmp;

extern string StrHWServer;		//华为服务器地址
extern string StrHWGetPasswd ;
extern string StrHWSetPasswd ;

extern THUAWEIGantry HUAWEIDevValue;
extern THUAWEIALARM HUAWEIDevAlarm;		//华为机柜告警
extern pthread_mutex_t snmpoidMutex ;   

extern string StrFireWareCount;	//防火墙数量
extern string StrFireWareIP[FIREWARE_NUM];         //防火墙IP
extern string StrFireWareGetPasswd[FIREWARE_NUM];  //防火墙get密码
extern string StrFireWareSetPasswd[FIREWARE_NUM];  //防火墙set密码
extern string StrIPSwitchCount;	//交换机数量
extern string StrIPSwitchIP[IPSWITCH_NUM] ;//交换机IP
extern string StrIPSwitchGetPasswd[IPSWITCH_NUM] ;//交换机get密码
extern string StrIPSwitchSetPasswd[IPSWITCH_NUM] ;//交换机set密码
extern string StrDeviceNameSeq[SWITCH_COUNT];	//设备名的配置

//交换机网络数据
TFIRESWITCH mTFIRESWITCH[32];
string strswitchjson = "";
//防火墙网络数据
TFIRESWITCH mTFIREWALL[32];
string strfirewalljson = "";

//空调地址
unsigned int hwAirAddrbuf[2];
//温适度地址
unsigned int hwTemAddrbuf[2];
//电池地址
unsigned int hwAcbAddrbuf[10] = {0,0,0,0,0,0,0,0,0,0};
unsigned char mAcbIndex = 0 ;

int sethwDcAirPowerOffTempPointCtlIndex[2];
int sethwDcAirPowerOnTempPointCtlIndex[2];
int sethwSetEnvTempUpperLimitIndex[2];
int sethwSetEnvTempLowerLimitIndex[2];
int sethwSetEnvHumidityUpperLimitIndex[2];
int sethwSetEnvHumidityLowerLimitIndex[2];
int sethwDcAirCtrlModeIndex[2];
int sethwCoolingDevicesModeIndex;
int sethwCtrlSmokeResetIndex;
int sethwSetAcsUpperVoltLimitIndex;
int sethwSetAcsLowerVoltLimitIndex;
int sethwSetDcsUpperVoltLimitIndex;
int sethwSetDcsLowerVoltLimitIndex;
int sethwCtrlMonEquipResetIndex;


void snmp_get_and_print(netsnmp_session *ss,oid *theoid,size_t theoid_len)
{  
    netsnmp_pdu     *pdu,*response;  
    netsnmp_variable_list   *vars;  
    int     status;  
    //FILE *fp = fopen("./data.txt","a+");  
    pdu = snmp_pdu_create(SNMP_MSG_GET);  
    snmp_add_null_var(pdu,theoid,theoid_len);  
  
    status = snmp_synch_response(ss, pdu, &response);  
    if(status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {  
        for(vars = response->variables; vars; vars = vars->next_variable){  
            numprinted ++;  
            //print_variable(vars->name, vars->name_length, vars);  
            /*if (vars->type == ASN_OCTET_STR) { 
                        char *sp = (char *)malloc(1 + vars->val_len); 
                        memcpy(sp, vars->val.string, vars->val_len); 
                        sp[vars->val_len] = '\0'; 
                        //printf("value #%d is a string: %s\n\n%d\n", count_print ++, sp,sizeof(sp)) 
 
                        while (*sp != '\0'){ 
                            c = *sp ++; 
                            fputc(c,fp); 
                        } 
                        free(sp); 
            }*/  
        }  
    }  
    if(response)  
        snmp_free_pdu(response);  
}  


int UpdataHUAWEIFirewall(int mgetindx,string getsp,int Intstrtype)
{
   //printf("oidindex=%d\r\n",mgetindx);
   pthread_mutex_lock(&snmpoidMutex);

   
   int mIntegerValue = atoi(getsp.c_str());
   
   EM_HUAWEIGantry mIntHUAWEIGantry = (EM_HUAWEIGantry)Intstrtype ;
   char mbuf[50];
   memset(mbuf,0,50);
   switch(mIntHUAWEIGantry)
   {
   //锂电
   case hwAcbGroupBatVolt:               //电池电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatVolt = mbuf ;
       //printf("电池电压:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatVolt).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);

       break;
   case hwAcbGroupBatCurr:            //电池电流
   {
       float fmIntegerValue = (float)mIntegerValue;
       fmIntegerValue = fmIntegerValue/10;
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%f",fmIntegerValue) ;
       else
            sprintf(mbuf,"%f",fmIntegerValue) ;
       HUAWEIDevValue.strhwAcbGroupBatCurr = mbuf ;
       //printf("电池电流:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatCurr).c_str());
   }
       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);

       break;
   case hwAcbGroupTotalCapacity:                //电池总容量
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTotalCapacity = mbuf ;
       //printf("电池总容量:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTotalCapacity).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break ;
   case hwAcbGroupTotalRemainCapacity:               //电池剩余容量
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity = mbuf ;
       //printf("电池剩余容量:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwAcbGroupBackupTime:              //电池备电时长
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBackupTime = mbuf ;
       //printf("电池备电时长:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBackupTime).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwAcbGroupBatSoh:             //电池 SOH
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatSoh = mbuf ;
       //printf("电池 SOH:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatSoh).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   //开关电源
   case hwApOrAblVoltage:                //A/AB 电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwApOrAblVoltage = mbuf ;
       //printf("A/AB 电压:%s\r\n",(HUAWEIDevValue.strhwApOrAblVoltage).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
        break;
   case hwBpOrBclVoltage:                //B/BC 电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwBpOrBclVoltage = mbuf ;
       //printf("B/BC 电压:%s\r\n",(HUAWEIDevValue.strhwBpOrBclVoltage).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwCpOrCalVoltage:                //C/CA 电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwCpOrCalVoltage = mbuf ;
       //printf("C/CA 电压:%s\r\n",(HUAWEIDevValue.strhwCpOrCalVoltage).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwAphaseCurrent:               //A 相电流
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
           sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAphaseCurrent = mbuf ;
       //printf("A 相电流:%s\r\n",(HUAWEIDevValue.strhwAphaseCurrent).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwBphaseCurrent:              //B 相电流
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwBphaseCurrent = mbuf ;
       //printf("B 相电流:%s\r\n",(HUAWEIDevValue.strhwBphaseCurrent).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwCphaseCurrent:             //C 相电流
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwCphaseCurrent = mbuf ;
       //printf("C 相电流:%s\r\n",(HUAWEIDevValue.strhwCphaseCurrent).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwDcOutputVoltage:             //DC 输出电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwDcOutputVoltage = mbuf ;
       //printf("DC 输出电压:%s\r\n",(HUAWEIDevValue.strhwDcOutputVoltage).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwDcOutputCurrent:               //DC 输出电流
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
           sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwDcOutputCurrent = mbuf ;
       //printf("DC 输出电流:%s\r\n",(HUAWEIDevValue.strhwDcOutputCurrent).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   //环境传感器
   case hwEnvTemperature:              //环境温度值
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwTemAddrbuf[0])
       {
           HUAWEIDevValue.strhwEnvTemperature[0] = mbuf ;
           //printf("设备柜环境温度值:%s\r\n",(HUAWEIDevValue.strhwEnvTemperature[0]).c_str());
       }
       else if(mgetindx == hwTemAddrbuf[1])
       {
           HUAWEIDevValue.strhwEnvTemperature[1] = mbuf ;
           //printf("电池柜环境温度值:%s\r\n",(HUAWEIDevValue.strhwEnvTemperature[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwEnvHumidity:             //环境湿度值
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwTemAddrbuf[0])
       {
           HUAWEIDevValue.strhwEnvHumidity[0] = mbuf ;
           //printf("设备柜环境湿度值:%s\r\n",(HUAWEIDevValue.strhwEnvHumidity[0]).c_str());
       }
       else if(mgetindx == hwTemAddrbuf[1])
       {
           HUAWEIDevValue.strhwEnvHumidity[1] = mbuf ;
           //printf("电池柜环境湿度值:%s\r\n",(HUAWEIDevValue.strhwEnvHumidity[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   //直流空调
	   case hwDcAirCtrlMode:			 //空调控制模式 已去掉这个节点
/*       if(mIntegerValue==0xFF)
           sprintf(mbuf,"%d",0x7FFFFFFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;

       if(mgetindx == hwAirAddrbuf[0])
       {
           sethwDcAirCtrlModeIndex[0] = mgetindx;
           HUAWEIDevValue.strhwDcAirCtrlMode[0] = mbuf ;
           printf("空调设备柜控制模式:%s\r\n",(HUAWEIDevValue.strhwDcAirCtrlMode[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           sethwDcAirCtrlModeIndex[1] = mgetindx;
           HUAWEIDevValue.strhwDcAirCtrlMode[1] = mbuf ;
           printf("空调电池柜控制模式:%s\r\n",(HUAWEIDevValue.strhwDcAirCtrlMode[1]).c_str());
       }
*/
       break;
   case hwDcAirRunStatus:             //空调运行状态
       if(mIntegerValue==0xFF)
           sprintf(mbuf,"%d",0x7FFFFFFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwAirAddrbuf[0])
       {
           HUAWEIDevValue.strhwDcAirRunStatus[0] = mbuf ;
           //printf("空调设备柜运行状态:%s\r\n",(HUAWEIDevValue.strhwDcAirRunStatus[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           HUAWEIDevValue.strhwDcAirRunStatus[1] = mbuf ;
           //printf("空调电池柜运行状态:%s\r\n",(HUAWEIDevValue.strhwDcAirRunStatus[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwDcAirCompressorRunStatus:             //空调压缩机运行状态
       if(mIntegerValue==0xFF)
           sprintf(mbuf,"%d",0x7FFFFFFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;

       if(mgetindx == hwAirAddrbuf[0])
       {
           HUAWEIDevValue.strhwDcAirCompressorRunStatus[0] = mbuf ;
           //printf("空调设备柜压缩机运行状态:%s\r\n",(HUAWEIDevValue.strhwDcAirCompressorRunStatus[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           HUAWEIDevValue.strhwDcAirCompressorRunStatus[1] = mbuf ;
           //printf("空调电池柜压缩机运行状态:%s\r\n",(HUAWEIDevValue.strhwDcAirCompressorRunStatus[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwDcAirInnrFanSpeed:             //空调内机转速
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwAirAddrbuf[0])
       {
           HUAWEIDevValue.strhwDcAirInnrFanSpeed[0] = mbuf ;
           //printf("空调设备柜内机转速:%s\r\n",(HUAWEIDevValue.strhwDcAirInnrFanSpeed[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           HUAWEIDevValue.strhwDcAirInnrFanSpeed[1] = mbuf ;
           //printf("空调电池柜内机转速:%s\r\n",(HUAWEIDevValue.strhwDcAirInnrFanSpeed[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwDcAirOuterFanSpeed:             //空调外风机转速
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwAirAddrbuf[0])
       {
           HUAWEIDevValue.strhwDcAirOuterFanSpeed[0] = mbuf ;
           //printf("空调设备柜外风机转速:%s\r\n",(HUAWEIDevValue.strhwDcAirOuterFanSpeed[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           HUAWEIDevValue.strhwDcAirOuterFanSpeed[1] = mbuf ;
           //printf("空调电池柜外风机转速:%s\r\n",(HUAWEIDevValue.strhwDcAirOuterFanSpeed[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwDcAirCompressorRunTime:             //空调压缩机运行时间
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwAirAddrbuf[0])
       {
           HUAWEIDevValue.strhwDcAirCompressorRunTime[0] = mbuf ;
           //printf("空调设备柜压缩机运行时间:%s\r\n",(HUAWEIDevValue.strhwDcAirCompressorRunTime[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           HUAWEIDevValue.strhwDcAirCompressorRunTime[1] = mbuf ;
           //printf("空调电池柜压缩机运行时间:%s\r\n",(HUAWEIDevValue.strhwDcAirCompressorRunTime[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwDcAirEnterChannelTemp:             //空调回风口温度
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwAirAddrbuf[0])
       {
           HUAWEIDevValue.strhwDcAirEnterChannelTemp[0] = mbuf ;
           //printf("空调设备柜回风口温度:%s\r\n",(HUAWEIDevValue.strhwDcAirEnterChannelTemp[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           HUAWEIDevValue.strhwDcAirEnterChannelTemp[1] = mbuf ;
           //printf("空调电池柜回风口温度:%s\r\n",(HUAWEIDevValue.strhwDcAirEnterChannelTemp[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwDcAirPowerOnTempPoint:             //空调开机温度点
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwAirAddrbuf[0])
       {
           sethwDcAirPowerOnTempPointCtlIndex[0] = mgetindx;
           HUAWEIDevValue.strhwDcAirPowerOnTempPoint[0] = mbuf ;
           //printf("空调设备柜开机温度点:%s\r\n",(HUAWEIDevValue.strhwDcAirPowerOnTempPoint[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           sethwDcAirPowerOnTempPointCtlIndex[1] = mgetindx;
           HUAWEIDevValue.strhwDcAirPowerOnTempPoint[1] = mbuf ;
           //printf("空调电池柜开机温度点:%s\r\n",(HUAWEIDevValue.strhwDcAirPowerOnTempPoint[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwDcAirPowerOffTempPoint:             //空调关机温度点
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwAirAddrbuf[0])
       {
           sethwDcAirPowerOffTempPointCtlIndex[0] = mgetindx;
           HUAWEIDevValue.strhwDcAirPowerOffTempPoint[0] = mbuf ;
           //printf("空调设备柜关机温度点:%s\r\n",(HUAWEIDevValue.strhwDcAirPowerOffTempPoint[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           sethwDcAirPowerOffTempPointCtlIndex[1] = mgetindx;
           HUAWEIDevValue.strhwDcAirPowerOffTempPoint[1] = mbuf ;
           //printf("空调电池柜关机温度点:%s\r\n",(HUAWEIDevValue.strhwDcAirPowerOffTempPoint[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
    //新增加
   //锂电(新增加)
   case hwAcbGroupTemperature:			  //电池温度
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTemperature = mbuf ;
       //printf("电池温度:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTemperature).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwAcbGroupOverCurThr:			  //充电过流告警点
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/100,mIntegerValue%100) ;
       HUAWEIDevValue.strhwAcbGroupOverCurThr = mbuf ;
       //printf("充电过流告警点:%s\r\n",(HUAWEIDevValue.strhwAcbGroupOverCurThr).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwAcbGroupHighTempThr:			  //高温告警点
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwAcbGroupHighTempThr = mbuf ;
       //printf("高温告警点:%s\r\n",(HUAWEIDevValue.strhwAcbGroupHighTempThr).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwAcbGroupLowTempTh:			  //低温告警点
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwAcbGroupLowTempTh = mbuf ;
       //printf("低温告警点:%s\r\n",(HUAWEIDevValue.strhwAcbGroupLowTempTh).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwAcbGroupDodToAcidBattery:			  //锂电放电DOD
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwAcbGroupDodToAcidBattery = mbuf ;
       //printf("锂电放电DOD:%s\r\n",(HUAWEIDevValue.strhwAcbGroupDodToAcidBattery).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   //开关电源(新增加)
   case hwSetAcsUpperVoltLimit:			  //AC过压点设置
       sprintf(mbuf,"%d",mIntegerValue) ;
       sethwSetAcsUpperVoltLimitIndex = mgetindx;
       HUAWEIDevValue.strhwSetAcsUpperVoltLimit = mbuf ;
       //printf("AC过压点设置:%s\r\n",(HUAWEIDevValue.strhwSetAcsUpperVoltLimit).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwSetAcsLowerVoltLimit:			  //AC欠压点设置
       sprintf(mbuf,"%d",mIntegerValue) ;
       sethwSetAcsLowerVoltLimitIndex = mgetindx;
       HUAWEIDevValue.strhwSetAcsLowerVoltLimit = mbuf ;
       //printf("AC欠压点设置:%s\r\n",(HUAWEIDevValue.strhwSetAcsLowerVoltLimit).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwSetDcsUpperVoltLimit:			  //设置DC过压点
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       sethwSetDcsUpperVoltLimitIndex = mgetindx;
       HUAWEIDevValue.strhwSetDcsUpperVoltLimit = mbuf ;
       //printf("设置DC过压点:%s\r\n",(HUAWEIDevValue.strhwSetDcsUpperVoltLimit).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwSetDcsLowerVoltLimit: 		  //设置DC欠压点
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       sethwSetDcsLowerVoltLimitIndex = mgetindx;
       HUAWEIDevValue.strhwSetDcsLowerVoltLimit = mbuf ;
       //printf("设置DC欠压点:%s\r\n",(HUAWEIDevValue.strhwSetDcsLowerVoltLimit).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwSetLvdVoltage:			  //设置LVD电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwSetLvdVoltage = mbuf ;
       //printf("设置LVD电压:%s\r\n",(HUAWEIDevValue.strhwSetLvdVoltage).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   //环境传感器(新增加)
   case hwSetEnvTempUpperLimit:			  //环境温度告警上限
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwTemAddrbuf[0])
       {
           sethwSetEnvTempUpperLimitIndex[0] = mgetindx;
           HUAWEIDevValue.strhwSetEnvTempUpperLimit[0] = mbuf ;
           //printf("设备柜环境温度告警上限:%s\r\n",(HUAWEIDevValue.strhwSetEnvTempUpperLimit[0]).c_str());
       }
       else if(mgetindx == hwTemAddrbuf[1])
       {
           sethwSetEnvTempUpperLimitIndex[1] = mgetindx;
           HUAWEIDevValue.strhwSetEnvTempUpperLimit[1] = mbuf ;
           //printf("电池柜环境温度告警上限:%s\r\n",(HUAWEIDevValue.strhwSetEnvTempUpperLimit[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwSetEnvTempLowerLimit:			  //环境温度告警下限
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwTemAddrbuf[0])
       {
           sethwSetEnvTempLowerLimitIndex[0] = mgetindx;
           HUAWEIDevValue.strhwSetEnvTempLowerLimit[0] = mbuf ;
           //printf("设备柜环境温度告警下限:%s\r\n",(HUAWEIDevValue.strhwSetEnvTempLowerLimit[0]).c_str());
       }
       else if(mgetindx == hwTemAddrbuf[1])
       {
           sethwSetEnvTempLowerLimitIndex[1] = mgetindx;
           HUAWEIDevValue.strhwSetEnvTempLowerLimit[1] = mbuf ;
           //printf("电池柜环境温度告警下限:%s\r\n",(HUAWEIDevValue.strhwSetEnvTempLowerLimit[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwSetEnvTempUltraHighTempThreshold:			  //环境高高温告警点
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold = mbuf ;
       //printf("环境高高温告警点:%s\r\n",(HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwSetEnvHumidityUpperLimit:			  //环境湿度告警上限
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwTemAddrbuf[0])
       {
           sethwSetEnvHumidityUpperLimitIndex[0] = mgetindx;
           HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[0] = mbuf ;
           //printf("设备柜环境湿度告警上限:%s\r\n",(HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[0]).c_str());
       }
       else if(mgetindx == hwTemAddrbuf[1])
       {
           sethwSetEnvHumidityUpperLimitIndex[1] = mgetindx;
           HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[1] = mbuf ;
           //printf("电池柜环境湿度告警上限:%s\r\n",(HUAWEIDevValue.strhwSetEnvHumidityUpperLimit[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwSetEnvHumidityLowerLimit:			  //环境湿度告警下限
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwTemAddrbuf[0])
       {
           sethwSetEnvHumidityLowerLimitIndex[0] = mgetindx;
           HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[0] = mbuf ;
           //printf("设备柜环境湿度告警下限:%s\r\n",(HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[0]).c_str());
       }
       else if(mgetindx == hwTemAddrbuf[1])
       {
           sethwSetEnvHumidityLowerLimitIndex[1] = mgetindx;
           HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[1] = mbuf ;
           //printf("电池柜环境湿度告警下限:%s\r\n",(HUAWEIDevValue.strhwSetEnvHumidityLowerLimit[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   //直流空调(新增加)
   case hwDcAirRunTime:			  //空调运行时间
       sprintf(mbuf,"%d",mIntegerValue) ;
       if(mgetindx == hwAirAddrbuf[0])
       {
           HUAWEIDevValue.strhwDcAirRunTime[0] = mbuf ;
           //printf("空调设备柜运行时间:%s\r\n",(HUAWEIDevValue.strhwDcAirRunTime[0]).c_str());
       }
       else if(mgetindx == hwAirAddrbuf[1])
       {
           HUAWEIDevValue.strhwDcAirRunTime[1] = mbuf ;
           //printf("空调电池柜运行时间:%s\r\n",(HUAWEIDevValue.strhwDcAirRunTime[1]).c_str());
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwCoolingDevicesMode:			  //温控模式
       sethwCoolingDevicesModeIndex = mgetindx;
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwCoolingDevicesMode = mbuf ;
       //printf("温控模式:%s\r\n",(HUAWEIDevValue.strhwCoolingDevicesMode).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   //2019-08-20新增
   case hwAcbGroupBatRunningState:			  //电池状态
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevValue.strhwAcbGroupBatRunningState = mbuf ;
	   //printf("电池状态:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatRunningState).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   case hwSmokeSensorStatus:			  //烟雾传感器状态
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevAlarm.hwSmokeAlarmTraps = mbuf ;
	   //printf("烟雾传感器状态:%s\r\n",(HUAWEIDevAlarm.hwSmokeAlarmTraps).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   case hwWaterSensorStatus:			  //水浸传感器状态
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevAlarm.hwWaterAlarmTraps = mbuf ;
	   //printf("水浸传感器状态:%s\r\n",(HUAWEIDevAlarm.hwWaterAlarmTraps).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   case hwDoorSensorStatus:			  //门磁传感器状态
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevAlarm.hwDoorAlarmTraps = mbuf ;
	   //printf("门磁传感器状态:%s\r\n",(HUAWEIDevAlarm.hwDoorAlarmTraps).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   case hwDcAirEquipAddress:			  //空调地址
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevValue.strhwDcAirEquipAddress = mbuf ;
       //21：设备柜（控制器）  22：电池柜
       if(HUAWEIDevValue.strhwDcAirEquipAddress == "21")
       {
           hwAirAddrbuf[0] = mgetindx ;
           //printf("空调设备柜索引:%d\r\n",mgetindx);
       }
       else if(HUAWEIDevValue.strhwDcAirEquipAddress == "22")
       {
           hwAirAddrbuf[1] = mgetindx ;
           //printf("空调电池柜索引:%d\r\n",mgetindx);
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   case hwTemHumEquipAddress:			  //温湿度地址
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevValue.strhwTemHumEquipAddress = mbuf ;
       //51：设备柜（控制器）  52：电池柜
       if(HUAWEIDevValue.strhwTemHumEquipAddress == "51")
       {
           hwTemAddrbuf[0] = mgetindx ;
           //printf("温湿度设备柜索引:%d\r\n",mgetindx);
       }
       else if(HUAWEIDevValue.strhwTemHumEquipAddress == "52")
       {
           hwTemAddrbuf[1] = mgetindx ;
           //printf("温湿度电池柜索引:%d\r\n",mgetindx);
       }

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   //单个锂电池2019-08-20新增
   case hwAcbBatVolt:			  //单个电池电压
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevValue.strhwAcbBatVolt = mbuf ;
	   //printf("单个电池电压:%s\r\n",(HUAWEIDevValue.strhwAcbBatVolt).c_str());
       if(mAcbIndex > 3)
           mAcbIndex = 0 ;
       hwAcbAddrbuf[mAcbIndex] =  mgetindx ;
       mAcbIndex = mAcbIndex + 1 ;
       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   case hwAcbBatCurr:			  //单个电池电流
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevValue.strhwAcbBatCurr = mbuf ;
	   //printf("单个电池电流:%s\r\n",(HUAWEIDevValue.strhwAcbBatCurr).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   case hwAcbBatSoh:			  //单个电池串SOH
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevValue.strhwAcbBatSoh = mbuf ;
	   //printf("单个电池串SOH:%s\r\n",(HUAWEIDevValue.strhwAcbBatSoh).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   case hwAcbBatCapacity:			  //单个电池容量
	   sprintf(mbuf,"%d",mIntegerValue) ;
	   HUAWEIDevValue.strhwAcbBatCapacity = mbuf ;
	   //printf("单个电池容量:%s\r\n",(HUAWEIDevValue.strhwAcbBatCapacity).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
	   break;
   //2019-09-05
   case hwCtrlSmokeReset:			  //控制烟感复位
       sethwCtrlSmokeResetIndex = mgetindx ;
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwCtrlSmokeReset = mbuf ;
       //printf("控制烟感复位:%s\r\n",(HUAWEIDevValue.strhwCtrlSmokeReset).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;
   case hwCtrlMonEquipReset:			  //控制单板复位 1,2,255
       sethwCtrlMonEquipResetIndex = mgetindx ;
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwCtrlMonEquipReset = mbuf ;
       //printf("控制单板复位:%s\r\n",(HUAWEIDevValue.strhwCtrlMonEquipReset).c_str());

       pthread_mutex_lock(&snmpdataMutex);
       snmpdataTime = 0;
       pthread_mutex_unlock(&snmpdataMutex);
       break;

   //防火墙
   case hwEntityCpuUsage:               //CPU
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwEntityCpuUsage = mbuf ;
       //printf("防火墙CPU:%s\r\n",(HUAWEIDevValue.strhwEntityCpuUsage).c_str());
       break;
    //交换机
    case hwswitchEntityCpuUsage:               //CPU
         sprintf(mbuf,"%d",mIntegerValue) ;
         HUAWEIDevValue.strhwswitchEntityCpuUsage = mbuf ;
         //printf("交换机CPU:%s\r\n",(HUAWEIDevValue.strhwswitchEntityCpuUsage).c_str());
         break;
   case hwEntityCpuUCheck:
        {
          stringstream ss;
          ss<<mgetindx;
          string strmgetindx = ss.str();

          string strsend;
          int IntEM = (int)hwEntityCpuUsage;
          stringstream sscpu;
          sscpu<<IntEM;
          strsend = sscpu.str();
          //查询CPU 
          strsend = StrFireWareIP[0]+ ";" + StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.5;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
          //发送查询内存使用率 mgetindx
          IntEM = (int)hwEntityMemUsage;
          stringstream ssmem;
          ssmem<<IntEM;
          strsend = ssmem.str();

          strsend = StrFireWareIP[0]+ ";" + StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.7;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
          //查询温度
          IntEM = (int)hwEntityTemperature;
          stringstream sstemper;
          sstemper<<IntEM;
          strsend = sstemper.str();

          strsend = StrFireWareIP[0]+ ";" + StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.11;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
        }
        break ;
    case hwEntityMemUsage:
        {
           //获取内存使用率
           //pthread_mutex_lock(&snmpoidMutex);
           HUAWEIDevValue.strhwEntityMemUsage = getsp;
           //pthread_mutex_unlock(&snmpoidMutex);
           //printf("防火墙内存使用率:%s\r\n",getsp.c_str());
        }
        break;
    case hwEntityTemperature:
        {
           //获取查询温度
           //pthread_mutex_lock(&snmpoidMutex);
           HUAWEIDevValue.strhwEntityTemperature = getsp;
           //pthread_mutex_unlock(&snmpoidMutex);
           //printf("防火墙温度:%s\r\n",getsp.c_str());
        }
        break;

   //交换机
   case hwswitchEntityCpuCheck:
        {
          stringstream ss;
          ss<<mgetindx;
          string strmgetindx = ss.str();

          string strsend;
          int IntEM = (int)hwswitchEntityCpuUsage;
          stringstream sscpu;
          sscpu<<IntEM;
          strsend = sscpu.str();
          //查询CPU
          strsend = StrIPSwitchIP[0]+ ";" + StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.5;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
          //发送查询内存使用率 mgetindx
          IntEM = (int)hwswitchEntityMemUsage;
          stringstream ssmem;
          ssmem<<IntEM;
          strsend = ssmem.str();

          strsend = StrIPSwitchIP[0]+ ";" + StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.7;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
          //查询温度
          IntEM = (int)hwswitchEntityTemperature;
          stringstream sstemper;
          sstemper<<IntEM;
          strsend = sstemper.str();

          strsend = StrIPSwitchIP[0]+ ";" + StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.11;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
        }
        break;
    case hwswitchEntityMemUsage:
        {
          //获取内存使用率
          //pthread_mutex_lock(&snmpoidMutex);
          HUAWEIDevValue.strhwswitchEntityMemUsage = getsp;
          //pthread_mutex_unlock(&snmpoidMutex);
          //printf("交换机内存使用率:%s\r\n",getsp.c_str());
       }
       break;
    case hwswitchEntityTemperature:
       {
         //获取查询温度
         //pthread_mutex_lock(&snmpoidMutex);
         HUAWEIDevValue.strhwswitchEntityTemperature = getsp;
         //pthread_mutex_unlock(&snmpoidMutex);
         //printf("交换机温度:%s\r\n",getsp.c_str());
       }
       break;

   case hwswitchEntityDescr://接口查询
       //printf("接口:%d,%s\r\n",mgetindx,getsp.c_str());
      // mTFIRESWITCH
   {
       char buf[10];
       string strbuf = "";
       string strbufg = "";
       string strbufx = "";
       for(int i=0;i<32;i++)
       {
           memset(buf,0x00,10);
           sprintf(buf,"%d",i+1);
           strbuf = buf;
           strbufg = "GigabitEthernet0/0/" + strbuf;
           strbufx = "XGigabitEthernet0/0/" + strbuf;
           if(getsp == strbufg)
           {
               mTFIRESWITCH[i].type = "g";
               mTFIRESWITCH[i].Descr = mgetindx;
           }
           else if(getsp == strbufx)
           {
               mTFIRESWITCH[i+25].type = "x";
               mTFIRESWITCH[i+25].Descr = mgetindx;
           }
       }
   }
       break;
   case hwswitchEntityOperStatus://接口状态查询
        //printf("接口状态查询:%d,%s\r\n",mgetindx,getsp.c_str());
        //strJson = strJson + "\"switchip\":\""+ StrIPSwitchIP[0] +"\",\n";	//交换机IP
       for(int i=0;i<32;i++)
       {
           if((mTFIRESWITCH[i].Descr == mgetindx) && ((mTFIRESWITCH[i].type == "g") || (mTFIRESWITCH[i].type == "x")))
           {
               if(getsp == "1")
                   mTFIRESWITCH[i].state = mTFIRESWITCH[i].type + "up";
               else
                   mTFIRESWITCH[i].state = mTFIRESWITCH[i].type + "down";
           }
       }
       break;
   case hwswitchEntityInOctets://总字节数
        //printf("总字节数:%d,%s\r\n",mgetindx,getsp.c_str());
       for(int i=0;i<32;i++)
       {
         if((mTFIRESWITCH[i].Descr == mgetindx) && ((mTFIRESWITCH[i].type == "g") || (mTFIRESWITCH[i].type == "x")))
         {
             mTFIRESWITCH[i].inoctets = getsp;
         }
       }
       break;
   case hwswitchEntityInErrors://出错数
       //printf("出错数:%d,%s\r\n",mgetindx,getsp.c_str());
       for(int i=0;i<32;i++)
       {
         if((mTFIRESWITCH[i].Descr == mgetindx) && ((mTFIRESWITCH[i].type == "g") || (mTFIRESWITCH[i].type == "x")))
         {
             mTFIRESWITCH[i].inerrors = getsp;
         }
       }
       break;
   case hwswitchEntityOutOctets://总字节数
       //printf("总字节数:%d,%s\r\n",mgetindx,getsp.c_str());
       for(int i=0;i<32;i++)
       {
         if((mTFIRESWITCH[i].Descr == mgetindx) && ((mTFIRESWITCH[i].type == "g") || (mTFIRESWITCH[i].type == "x")))
         {
             mTFIRESWITCH[i].outoctets = getsp;
         }
       }
       break;
   case hwswitchEntityOutErrors://出错数
       //printf("出错数:%d,%s\r\n",mgetindx,getsp.c_str());
       for(int i=0;i<32;i++)
       {
         if((mTFIRESWITCH[i].Descr == mgetindx) && ((mTFIRESWITCH[i].type == "g") || (mTFIRESWITCH[i].type == "x")))
         {
             mTFIRESWITCH[i].outerrors = getsp;
         }
       }

       break;

   case hwEntityDescr://接口查询
       //printf("防火墙接口:%d,%s\r\n",mgetindx,getsp.c_str());
      // mTFIRESWITCH
   {
       char buf[10];
       string strbuf = "";
       string strbufg = "";
       string strbufx = "";
       for(int i=0;i<32;i++)
       {
           memset(buf,0x00,10);
           sprintf(buf,"%d",i);
           strbuf = buf;
           strbufg = "GigabitEthernet0/0/" + strbuf;
           strbufx = "XGigabitEthernet0/0/" + strbuf;
           if(getsp == strbufg)
           {
               mTFIREWALL[i].type = "g";
               mTFIREWALL[i].Descr = mgetindx;
           }
           else if(getsp == strbufx)
           {
               mTFIREWALL[i+24].type = "x";
               mTFIREWALL[i+24].Descr = mgetindx;
           }
       }
   }
       break;
   case hwEntityOperStatus://接口状态查询
        //printf("防火墙接口状态查询:%d,%s\r\n",mgetindx,getsp.c_str());
        //strJson = strJson + "\"switchip\":\""+ StrIPSwitchIP[0] +"\",\n";	//交换机IP
       for(int i=0;i<32;i++)
       {
           if((mTFIREWALL[i].Descr == mgetindx) && ((mTFIREWALL[i].type == "g") || (mTFIREWALL[i].type == "x")))
           {
               if(getsp == "1")
                   mTFIREWALL[i].state = mTFIREWALL[i].type + "up";
               else
                   mTFIREWALL[i].state = mTFIREWALL[i].type + "down";
           }
       }
       break;
   case hwEntityInOctets://总字节数
        //printf("防火墙总字节数:%d,%s\r\n",mgetindx,getsp.c_str());
       for(int i=0;i<32;i++)
       {
         if((mTFIREWALL[i].Descr == mgetindx) && ((mTFIREWALL[i].type == "g") || (mTFIREWALL[i].type == "x")))
         {
             mTFIREWALL[i].inoctets = getsp;
         }
       }
       break;
   case hwEntityInErrors://出错数
       //printf("防火墙出错数:%d,%s\r\n",mgetindx,getsp.c_str());
       for(int i=0;i<32;i++)
       {
         if((mTFIREWALL[i].Descr == mgetindx) && ((mTFIREWALL[i].type == "g") || (mTFIREWALL[i].type == "x")))
         {
             mTFIREWALL[i].inerrors = getsp;
         }
       }
       break;
   case hwEntityOutOctets://总字节数
       //printf("防火墙总字节数:%d,%s\r\n",mgetindx,getsp.c_str());
       for(int i=0;i<32;i++)
       {
         if((mTFIREWALL[i].Descr == mgetindx) && ((mTFIREWALL[i].type == "g") || (mTFIREWALL[i].type == "x")))
         {
             mTFIREWALL[i].outoctets = getsp;
         }
       }
       break;
   case hwEntityOutErrors://出错数
       //printf("防火墙出错数:%d,%s\r\n",mgetindx,getsp.c_str());
       for(int i=0;i<32;i++)
       {
         if((mTFIREWALL[i].Descr == mgetindx) && ((mTFIREWALL[i].type == "g") || (mTFIREWALL[i].type == "x")))
         {
             mTFIREWALL[i].outerrors = getsp;
         }
       }

       break;

   default:
       break;


   }

    pthread_mutex_unlock(&snmpoidMutex);
}




//int mywalkappinit()
void *Walksnmpthread(void *param)
{  
    netsnmp_session     session, *ss;  
    netsnmp_pdu     *pdu,*response;  
    netsnmp_variable_list   *vars;  
    oid     name[MAX_OID_LEN];  
    size_t  name_length;  
    oid     root[MAX_OID_LEN];  
    size_t  rootlen;  
    int     count;  
    int     running;  
    int     status;  
    int     exitval = 0;  
  
    init_snmp("mywalkapp");  
  
    snmp_sess_init( &session );  

    //获取Walksnmp的OID
    int pos,vecSnmpSize,IntHUAWEIGantry,Intstroidindx,Intstroidtype,Intstrtype;
    string strsnmpget,strip,strcommunity,stroid,stroidindx,stroidtype,strHUAWEIGantry,strtype;

    while(1)
    {
       //获取数据 ip;community;oid
       pthread_mutex_lock(&WalksnmpoidMutex);
       vecSnmpSize = vecWalkSnmp.size();
       if(vecSnmpSize <= 0)
       {
           pthread_mutex_unlock(&WalksnmpoidMutex);
           usleep(300000) ;
           continue ;
       }

       vector<string>::iterator fistSnmp = vecWalkSnmp.begin();
       strsnmpget = *fistSnmp ;
//       cout<<strsnmpget<<endl;
       vecWalkSnmp.erase(fistSnmp);
       pthread_mutex_unlock(&WalksnmpoidMutex);

       pos = strsnmpget.find(';') ;
       if(pos > 0)
          strip = strsnmpget.substr(0,pos) ;
       else
          continue ;
       strsnmpget = strsnmpget.substr(pos+1) ;

       pos = strsnmpget.find(';') ;
       if(pos > 0)
           strcommunity = strsnmpget.substr(0,pos) ;
       else
           continue ;
       strsnmpget = strsnmpget.substr(pos+1) ;

       pos = strsnmpget.find(';') ;
       if(pos > 0)
          stroid = strsnmpget.substr(0,pos) ;
       else
          continue ;
       strsnmpget = strsnmpget.substr(pos+1) ;

       pos = strsnmpget.find(';') ;
       if(pos > 0)
          stroidindx = strsnmpget.substr(0,pos) ;
       else
          continue ;
       Intstroidindx = atoi(stroidindx.c_str()) ;
       strsnmpget = strsnmpget.substr(pos+1) ;

       if(Intstroidindx > 127)
          continue ;

       pos = strsnmpget.find(';') ;
       if(pos > 0)
          stroidtype = strsnmpget.substr(0,pos) ;
       else
          continue ;
       Intstroidtype= atoi(stroidtype.c_str()) ;
       strsnmpget = strsnmpget.substr(pos+1) ;



       pos = strsnmpget.find(';') ;
       if(pos > 0)
           strHUAWEIGantry = strsnmpget.substr(0,pos) ;
       else
           continue ;
       strsnmpget = strsnmpget.substr(pos+1) ;


       pos = strsnmpget.find(';') ;
       if(pos > 0)
           strtype = strsnmpget.substr(0,pos) ;
       else
           continue ;
       Intstrtype = atoi(strtype.c_str());
 

    session.peername = strdup(strip.c_str());  
    session.version = SNMP_VERSION_2c;  
  
    session.securityName = strdup("motorola");  
    session.securityNameLen = strlen(session.securityName);  
  
    session.community = (unsigned char*)(strcommunity.c_str());
    session.community_len = strcommunity.size();
  
    session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;  
  
    session.securityAuthProto = usmHMACMD5AuthProtocol;  
    session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);  
  
    session.securityAuthKeyLen = USM_AUTH_KU_LEN;  
  
    
    rootlen = MAX_OID_LEN;  
    if (snmp_parse_oid (stroid.c_str(), root, &rootlen) == NULL) {  
        snmp_perror(strip.c_str());  
       // exit(1);
        continue ; 
    }  
  
    SOCK_STARTUP;  
      
    ss = snmp_open(&session);  
    if(ss == NULL){  
        snmp_sess_perror("snmpwalk",&session);  
        SOCK_CLEANUP; 
        continue ; 
        //exit(1);  
    }  
  
    memmove(name,root,rootlen * sizeof(oid));  
    name_length = rootlen;  
      
    snmp_get_and_print(ss,root,rootlen);  
      
    running = 1;  

    mAcbIndex = 0 ;
    int mindex = 0;
    while (running) {
        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);  
        snmp_add_null_var(pdu, name, name_length);  
    
        status = snmp_synch_response(ss, pdu, &response);  
        if (status == STAT_SUCCESS) {  
            if (response->errstat == SNMP_ERR_NOERROR)
            {
                /* 
                 * check resulting variables  
                 */  

                for (vars = response->variables; vars;  
                     vars = vars->next_variable)
                {
                    if ((vars->name_length < rootlen)  
                        || (memcmp(root, vars->name, rootlen * sizeof(oid))  
                            != 0)) {  
                        /* 
                         * not part of this subtree  
                         */  
                        running = 0;  
                        //printf("not part of this subtree\r\n");
                        continue;  
                    }  
                    numprinted++;  
                    //printf("start\r\n");
                    //print_variable(vars->name, vars->name_length, vars);
                    //string Intstrnnn = (char *)vars->buf;
                   // printf("Intstrnnn=%s\r\n",Intstrnnn.c_str());
                    if (vars->type == ASN_OCTET_STR)
                    {
                        string getsp ;
                        char *sp = (char *)malloc(1 + vars->val_len); 
                        memcpy(sp, vars->val.string, vars->val_len); 
                        sp[vars->val_len] = '\0'; 
                       // printf("string:%s\r\n",sp);
						UpdataHUAWEIGantryStr(sp,vars->val_len+1,(EM_HUAWEIGantry)Intstrtype);
                        getsp = sp ;
                        free(sp);

                        //全部获取
                        if(Intstroidtype == 0x7FFFFFFF)
                        {
                              int mgetindx = vars->name_loc[Intstroidindx] ;
                              UpdataHUAWEIFirewall(mgetindx,getsp,Intstrtype);
                        }
                        else if(Intstroidtype == 0) //查找一个
                        {
                            if(getsp == strHUAWEIGantry)
                             {
                                  int mgetindx = vars->name_loc[Intstroidindx] ;
                                  UpdataHUAWEIFirewall(mgetindx,strHUAWEIGantry,Intstrtype);
                             }
                        }



                       // string Stroid = vars->name_loc;

                       // printf("%s\r\n",Stroid.c_str()) ;
                        /*for(int n=0;n<20;n++)
                            printf(".%d",vars->name_loc[n]);
                        printf("\r\n");
                        printf("\r\n");
                        printf("\r\n");
                        */


                    }
                    else if((vars->type == ASN_INTEGER) || (vars->type == ASN_GAUGE) || (vars->type == ASN_COUNTER))
                    {
                       // unsigned int IntegerValue = *(vars->val.integer) ;
                        stringstream ssIntegerValue;
					    UINT32 value=*(vars->val.integer);
//                        ssIntegerValue<<*(vars->val.integer) ;
                        ssIntegerValue<<value;
                        string getsp = ssIntegerValue.str();
                        //全部获取
                        if(Intstroidtype == 0x7FFFFFFF)
                        {
                           //UpdataHUAWEIFirewall(Intstroidtype,getsp,Intstrtype);
                            UpdataHUAWEIFirewall(vars->name_loc[Intstroidindx],getsp,Intstrtype);
                        }
                        else if(Intstroidtype == vars->name_loc[Intstroidindx]) //查找一个
                        {
                           UpdataHUAWEIFirewall(Intstroidtype,getsp,Intstrtype);
                        }

                       /*
                        for(int n=0;n<20;n++)
                            printf(".%d",vars->name_loc[n]);

                        printf("\r\n");
                        printf("\r\n");
                        printf("\r\n");
                        */

                    }
                    /*else if(vars->type == ASN_OBJECT_ID)
                    {


                    }
                    */


                    if ((vars->type != SNMP_ENDOFMIBVIEW) &&  
                        (vars->type != SNMP_NOSUCHOBJECT) &&  
                        (vars->type != SNMP_NOSUCHINSTANCE))
                    {
                        memmove((char *) name, (char *) vars->name,  
                                vars->name_length * sizeof(oid));  
                        name_length = vars->name_length;  
                    }
                    else
                    {
                        running = 0;
                    }


                }  
            }
            else
            {
                running = 0;  
                if (response->errstat == SNMP_ERR_NOSUCHNAME) {  
                    ;//printf("End of MIB\n");  
                } else {  
                    fprintf(stderr, "Error in packet.\nReason: %s\n",  
                            snmp_errstring(response->errstat));  
                    if (response->errindex != 0) {  
                        fprintf(stderr, "Failed object: ");  
                        for (count = 1, vars = response->variables;  
                             vars && count != response->errindex;  
                             vars = vars->next_variable, count++)  
                            /*EMPTY*/;  
                        if (vars)  
                            fprint_objid(stderr, vars->name,  
                                         vars->name_length);  
                        fprintf(stderr, "\n");  
                    }  
                    exitval = 2;  
                }  
            }  
        }
        else if (status == STAT_TIMEOUT)
        {
            fprintf(stderr, "Timeout: No Response from %s\n",  
                    session.peername);  
            running = 0;  
            exitval = 1;  
        }
        else
        {
            snmp_sess_perror("snmpwalk", ss);  
            running = 0;  
            exitval = 1;  
        }  
        if (response)  
            snmp_free_pdu(response);  
    }
    if (numprinted == 0 && status == STAT_SUCCESS)
    {
       snmp_get_and_print(ss, root, rootlen);  
    }
    snmp_close(ss);  
  
    SOCK_CLEANUP;  

    usleep(20000) ;

   }
   return 0;
}  



int SendWalkSnmpOid(string mSnmpOid)
{
    pthread_mutex_lock(&WalksnmpoidMutex);
    vecWalkSnmp.push_back(mSnmpOid.c_str());
    pthread_mutex_unlock(&WalksnmpoidMutex);
    return 0 ;
}


int SendHUAWEIsnmpWalk(EM_HUAWEIGantry mEM_HUAWEIGantry)
{
   int IntEM = (int)mEM_HUAWEIGantry;
   stringstream ss;
   ss<<IntEM;
   string strsend = ss.str();

   switch(mEM_HUAWEIGantry)
   {
   //锂电
   case hwAcbGroupBatVolt:               //电池电压
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.1.1.5;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAcbGroupBatCurr:            //电池电流
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.1.1.6;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAcbGroupTotalCapacity:                //电池总容量
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.1.1.7;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break ;
   case hwAcbGroupTotalRemainCapacity:               //电池剩余容量
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.1.1.8;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAcbGroupBackupTime:              //电池备电时长
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.1.1.11;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAcbGroupBatSoh:             //电池 SOH
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.1.1.13;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   //开关电源
   case hwApOrAblVoltage:                //A/AB 电压
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.5.2.1.1.4;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
        break;
   case hwBpOrBclVoltage:                //B/BC 电压
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.5.2.1.1.5;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwCpOrCalVoltage:                //C/CA 电压
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.5.2.1.1.6;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAphaseCurrent:               //A 相电流
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.5.2.1.1.7;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwBphaseCurrent:              //B 相电流
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.5.2.1.1.8;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwCphaseCurrent:             //C 相电流
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.5.2.1.1.9;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcOutputVoltage:             //DC 输出电压
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.6.2.1.1.4;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcOutputCurrent:               //DC 输出电流
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.6.2.1.1.5;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   //环境传感器
   case hwEnvTemperature:              //环境温度值
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.28.1.1.4;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwEnvHumidity:             //环境湿度值
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.28.1.1.5;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   //直流空调
   case hwDcAirCtrlMode:             //空调控制模式
//       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.4;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
//       SendWalkSnmpOid(strsend);
       break;
   case hwDcAirRunStatus:             //空调运行状态
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.6;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcAirCompressorRunStatus:             //空调压缩机运行状态
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.7;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcAirInnrFanSpeed:             //空调内机转速
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.8;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcAirOuterFanSpeed:             //空调外风机转速
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.9;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcAirCompressorRunTime:             //空调压缩机运行时间
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.10;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcAirEnterChannelTemp:             //空调回风口温度
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.13;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcAirPowerOnTempPoint:             //空调开机温度点
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.14;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcAirPowerOffTempPoint:             //空调关机温度点
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.15;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwMonEquipSoftwareVersion:			  //软件版本
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.2.2.1.1.5;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwMonEquipManufacturer:			  //设备生产商
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.2.2.1.1.7;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAcbGroupTemperature:			  //电池温度
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.1.1.9;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAcbGroupOverCurThr:			  //充电过流告警点
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.2.1.14;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAcbGroupHighTempThr:			  //高温告警点
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.2.1.21;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAcbGroupLowTempTh:			  //低温告警点
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.2.1.22;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwAcbGroupDodToAcidBattery:			  //锂电放电DOD
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.2.1.26;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetAcsUpperVoltLimit:			  //AC过压点设置
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.5.1.5;" + "13;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
    case hwSetAcsLowerVoltLimit:			  //AC欠压点设置
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.5.1.6;" + "13;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetDcsUpperVoltLimit:			  //设置DC过压点
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.6.1.7;" + "13;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetDcsLowerVoltLimit:			 //设置DC欠压点
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.6.1.8;" + "13;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetLvdVoltage:			 //设置LVD电压
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.7.2.1.1.5;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetEnvTempUpperLimit:			 //环境温度告警上限
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.28.1.1.6;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetEnvTempLowerLimit:			 //环境温度告警下限
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.28.1.1.7;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);						  
       break;
   case hwSetEnvTempUltraHighTempThreshold:			 //环境高高温告警点
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.2.1.1.10;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetEnvHumidityUpperLimit:			 //环境湿度告警上限
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.28.1.1.8;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetEnvHumidityLowerLimit:			 //环境湿度告警下限
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.28.1.1.9;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwDcAirRunTime:			 //空调运行时间
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.11;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwCoolingDevicesMode:			 //温控模式
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.1.3;" + "13;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   //2019-08-20新增
   //单个锂电池2019-08-20新增
   case hwAcbGroupBatRunningState:			 //电池状态
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.17.1.1.4;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   case hwSmokeSensorStatus:			 //烟雾传感器状态
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.2.13.1.4;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   case hwWaterSensorStatus:			 //水浸传感器状态
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.2.14.1.4;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   case hwDoorSensorStatus:			 //门磁传感器状态
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.2.15.1.4;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   case hwDcAirEquipAddress:			 //空调地址
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.21;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   case hwTemHumEquipAddress:			 //温湿度地址
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.28.1.1.10;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   case hwAcbBatVolt:			 //单个电池电压
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.18.2.1.1;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   case hwAcbBatCurr:			 //单个电池电流
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.18.2.1.2;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   case hwAcbBatSoh:			 ///单个电池串SOH
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.18.2.1.4;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   case hwAcbBatCapacity:			 //单个电池容量
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.18.2.1.5;" + "14;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
	   SendWalkSnmpOid(strsend);
	   break;
   //2019-09-05
   case hwCtrlSmokeReset:			 //控制烟感复位
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.1.3;" + "13;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwCtrlMonEquipReset:		 //控制单板复位 1,2,255
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.2.2.1.1.12;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;

   case hwswitchEntityDescr:			 //交换机接口查询
       strsend = StrIPSwitchIP[0]+";"+StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.2;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwswitchEntityOperStatus:			 //交换机接口状态查询      
       strsend = StrIPSwitchIP[0]+";"+StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.8;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwswitchEntityInOctets:			 //交换机输入总字节数
       strsend = StrIPSwitchIP[0]+";"+StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.10;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwswitchEntityInErrors:			 //交换机出错数
       strsend = StrIPSwitchIP[0]+";"+StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.14;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwswitchEntityOutOctets:			 //交换机输出总字节数
       strsend = StrIPSwitchIP[0]+";"+StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.16;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwswitchEntityOutErrors:			 //交换机出错数
       strsend = StrIPSwitchIP[0]+";"+StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.20;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwEntityDescr:			 //防火墙接口查询
       strsend = StrFireWareIP[0]+";"+StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.2;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwEntityOperStatus:			 //防火墙接口状态查询
       strsend = StrFireWareIP[0]+";"+StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.8;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwEntityInOctets:			 //防火墙输入总字节数
       strsend = StrFireWareIP[0]+";"+StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.10;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwEntityInErrors:			 //防火墙出错数
       strsend = StrFireWareIP[0]+";"+StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.14;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwEntityOutOctets:			 //防火墙输出总字节数
       strsend = StrFireWareIP[0]+";"+StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.16;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwEntityOutErrors:			 //防火墙出错数
       strsend = StrFireWareIP[0]+";"+StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.2.1.2.2.1.20;" + "10;" + "2147483647‬;" + "Ethernet;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   default:
       break;


   }
   return 0 ;
}


void *WalkSendsnmpthread(void *param)
{
    int j,IntEM;
	int portcnt;
	
    string strsend ;

    for(int i=0;i<32;i++)
    {
        mTFIRESWITCH[i].Descr = 0x7FFFFFFF;
        mTFIRESWITCH[i].type = "";
        mTFIREWALL[i].Descr = 0x7FFFFFFF;
        mTFIREWALL[i].type = "";
    }

    //空调地址
    hwAirAddrbuf[0] = hwAirAddrbuf[1] = 0;
    //温适度地址
    hwTemAddrbuf[0] = hwTemAddrbuf[1] = 0;

	sethwDcAirPowerOffTempPointCtlIndex[0] = sethwDcAirPowerOffTempPointCtlIndex[1]= 0x7FFFFFFF;
	sethwDcAirPowerOnTempPointCtlIndex[0] = sethwDcAirPowerOnTempPointCtlIndex[1]= 0x7FFFFFFF;
	sethwCoolingDevicesModeIndex = 0x7FFFFFFF;
	sethwSetEnvTempUpperLimitIndex[0] = sethwSetEnvTempUpperLimitIndex[1]= 0x7FFFFFFF;
	sethwSetEnvTempLowerLimitIndex[0] = sethwSetEnvTempLowerLimitIndex[1]= 0x7FFFFFFF;
	sethwSetEnvHumidityUpperLimitIndex[0] = sethwSetEnvHumidityUpperLimitIndex[1] = 0x7FFFFFFF;
	sethwSetEnvHumidityLowerLimitIndex[0] = sethwSetEnvHumidityLowerLimitIndex[1] = 0x7FFFFFFF;
	sethwDcAirCtrlModeIndex[0] = sethwDcAirCtrlModeIndex[1] = 0x7FFFFFFF;
	sethwCtrlSmokeResetIndex= 0x7FFFFFFF;
	sethwSetAcsUpperVoltLimitIndex= 0x7FFFFFFF;
	sethwSetAcsLowerVoltLimitIndex= 0x7FFFFFFF;
	sethwSetDcsUpperVoltLimitIndex= 0x7FFFFFFF;
	sethwSetDcsLowerVoltLimitIndex= 0x7FFFFFFF;
	sethwCtrlMonEquipResetIndex = 0x7FFFFFFF;

    sleep(5);
    while(1)
    {
       strsend = StrIPSwitchIP[0]+";"+StrIPSwitchGetPasswd[0] + ";" + ".1.3.6.1.2.1.47.1.1.1.1.7;" + "12;" + "0;" + "MPU Board 0;" + "11000;";
       SendWalkSnmpOid(strsend);
       sleep(1);
       //交换机网络参数
       for(int i=hwswitchEntityDescr;i<=hwswitchEntityDescr+6;i++)
       {
//			printf("SendHUAWEIsnmp %d\n",i);
           SendHUAWEIsnmpWalk((EM_HUAWEIGantry)i);
       }
       sleep(3);


       strsend = StrFireWareIP[0]+";"+StrFireWareGetPasswd[0] + ";" + ".1.3.6.1.2.1.47.1.1.1.1.7;" + "12;" + "0;" + "RPU;" + "10000;";
       SendWalkSnmpOid(strsend);
       sleep(1);
       //防火墙网络参数
       for(int i=hwEntityDescr;i<=hwEntityDescr+6;i++)
       {
//			printf("SendHUAWEIsnmp %d\n",i);
           SendHUAWEIsnmpWalk((EM_HUAWEIGantry)i);
       }
       sleep(3);


       //获取空调地址
       SendHUAWEIsnmpWalk(hwDcAirEquipAddress);
       //获取温适度地址
       SendHUAWEIsnmpWalk(hwTemHumEquipAddress);
       for(int i=1;i<=HWGANTRY_COUNT;i++)
       {
//			printf("SendHUAWEIsnmp %d\n",i);
           SendHUAWEIsnmpWalk((EM_HUAWEIGantry)i);
       }

       sleep(5);

       int portcount = 0;
       for(int i=0;i<32;i++)
       {
          if(mTFIRESWITCH[i].Descr != 0x7FFFFFFF)
              ++ portcount;
       }

       stringstream ss;
       ss<<portcount;
       string strportcount = ss.str();

       string strswitchnet = "" ;
       strswitchnet = "\"ip\":\""+ StrIPSwitchIP[0] +"\",\n";
       strswitchnet = strswitchnet + "\"type\":\"switch\",\n";
       strswitchnet = strswitchnet + "\"portcount\":\""+ strportcount +"\",\n";
       strswitchnet = strswitchnet + "\"portinfolist\":[\n";
	   portcnt=atoi(strportcount.c_str());
	   //printf("ip switch port count=%d\n",portcnt);
       for(int i=0,j=0;i<32;i++)
       {
         if(mTFIRESWITCH[i].Descr != 0x7FFFFFFF)
         {
             stringstream ssport;
             if((mTFIRESWITCH[i].type == "x") && (i>24))
                 ssport<<i-24;
             else
                 ssport<<i+1;

             string strport = ssport.str();
             strswitchnet = strswitchnet + "{\n";
             strswitchnet = strswitchnet + "\"port_id\":\""+ strport +"\",\n";
             strswitchnet = strswitchnet + "\"state\":\""+ mTFIRESWITCH[i].state +"\",\n";
             strswitchnet = strswitchnet + "\"inoctets\":\""+ mTFIRESWITCH[i].inoctets +"\",\n";
             strswitchnet = strswitchnet + "\"inerrors\":\""+ mTFIRESWITCH[i].inerrors +"\",\n";
             strswitchnet = strswitchnet + "\"outoctets\":\""+ mTFIRESWITCH[i].outoctets +"\",\n";
             strswitchnet = strswitchnet + "\"outerrors\":\""+ mTFIRESWITCH[i].outerrors +"\"\n";
			 if(j==portcnt-1)
             	 strswitchnet = strswitchnet + "}\n";
			 else
				 strswitchnet = strswitchnet + "},\n";
			 j++;
         }
       }

       strswitchnet = strswitchnet + "]\n";
       strswitchjson = strswitchnet;
       //printf("%s\r\n",strswitchnet.c_str());
       sleep(5);


       portcount = 0;
       for(int i=0;i<32;i++)
       {
          if(mTFIREWALL[i].Descr != 0x7FFFFFFF)
              ++ portcount;
       }

       stringstream ssfirewall;
       ssfirewall<<portcount;
       strportcount = ssfirewall.str();

       strswitchnet = "" ;
       strswitchnet = "\"ip\":\""+ StrIPSwitchIP[0] +"\",\n";
       strswitchnet = strswitchnet + "\"type\":\"fireware\",\n";
       strswitchnet = strswitchnet + "\"portcount\":\""+ strportcount +"\",\n";
       strswitchnet = strswitchnet + "\"portinfolist\":[\n";
	   portcnt=atoi(strportcount.c_str());
	   //printf("firewall port count=%d\n",portcnt);
       for(int i=0,j=0;i<32;i++)
       {
         if(mTFIREWALL[i].Descr != 0x7FFFFFFF)
         {
             stringstream ssport;
             if((mTFIREWALL[i].type == "x") && (i>23))
                 ssport<<i-23;
             else
                 ssport<<i;

             string strport = ssport.str();
             strswitchnet = strswitchnet + "{\n";
             strswitchnet = strswitchnet + "\"port_id\":\""+ strport +"\",\n";
             strswitchnet = strswitchnet + "\"state\":\""+ mTFIREWALL[i].state +"\",\n";
             strswitchnet = strswitchnet + "\"inoctets\":\""+ mTFIREWALL[i].inoctets +"\",\n";
             strswitchnet = strswitchnet + "\"inerrors\":\""+ mTFIREWALL[i].inerrors +"\",\n";
             strswitchnet = strswitchnet + "\"outoctets\":\""+ mTFIREWALL[i].outoctets +"\",\n";
             strswitchnet = strswitchnet + "\"outerrors\":\""+ mTFIREWALL[i].outerrors +"\"\n";
			 if(j==portcnt-1)
             	strswitchnet = strswitchnet + "}\n";
			 else
				 strswitchnet = strswitchnet + "},\n";
			 j++;
         }
       }

       strswitchnet = strswitchnet + "]\n";
       strfirewalljson = strswitchnet ;
       //printf("%s\r\n",strswitchnet.c_str());
       sleep(60*5);



    }

    return 0 ;
}

//mIndex: 1：设备柜，2：电池柜
int SnmpSetOid(EM_HUAWEIGantry mEM_HUAWEIGantry,string mIntValue,int mIndex)
{
    string msetoid ;
    pthread_mutex_lock(&SetsnmpoidMutex);
    switch(mEM_HUAWEIGantry)
    {
       //控制烟感复位 2:复位
      case hwCtrlSmokeReset:
        if(sethwCtrlSmokeResetIndex != 0x7FFFFFFF)
        {
         stringstream ssIndex;
         ssIndex<<sethwCtrlSmokeResetIndex;
         string strIndex = ssIndex.str();
         msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.8.1.3." + strIndex + " i " + mIntValue ;
         //printf("控制烟感复位:%s\r\n",msetoid.c_str());
         system(msetoid.c_str());
        }
         break;
       //温控模式 2：纯空调模式；3：智能模式；
       case hwCoolingDevicesMode:
        if(sethwCoolingDevicesModeIndex != 0x7FFFFFFF)
        {
            stringstream ssIndex;
            ssIndex<<sethwCoolingDevicesModeIndex;
            string strIndex = ssIndex.str();
            msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.10.1.3." + strIndex + " i " + mIntValue ;
            system(msetoid.c_str());
        }
            break;
       //空调开机温度点  温控模式为纯空调模式时，值范围为：35 - 50 ，温控模式为智能模式时，值范围为：38 - 50
       case hwDcAirPowerOnTempPoint:
       {
           stringstream ssIndex;
           if(mIndex == 1)
           {
               if(sethwDcAirPowerOnTempPointCtlIndex[0] == 0x7FFFFFFF)
                  break;
               ssIndex<<sethwDcAirPowerOnTempPointCtlIndex[0];
           }
           else
           {
               if(sethwDcAirPowerOnTempPointCtlIndex[1] == 0x7FFFFFFF)
                  break;
               ssIndex<<sethwDcAirPowerOnTempPointCtlIndex[1];
           }
           string strIndex = ssIndex.str();
           msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.10.2.2.1.14." + strIndex + " i " + mIntValue ;
           system(msetoid.c_str());
        }
            break;
       //空调关机温度点 温控模式为纯空调模式时，值范围为：27 - 45 ，温控模式为智能模式时，值范围为：33 - 45
       case hwDcAirPowerOffTempPoint:
       {
           stringstream ssIndex;
           if(mIndex == 1)
           {
               if(sethwDcAirPowerOffTempPointCtlIndex[0] == 0x7FFFFFFF)
                  break;
               ssIndex<<sethwDcAirPowerOffTempPointCtlIndex[0];
           }
           else
           {
               if(sethwDcAirPowerOffTempPointCtlIndex[1] == 0x7FFFFFFF)
                  break;
               ssIndex<<sethwDcAirPowerOffTempPointCtlIndex[1];
           }
           string strIndex = ssIndex.str();
           msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.10.2.2.1.15." + strIndex + " i " + mIntValue ;
           system(msetoid.c_str());
        }
            break;
        //环境温度告警上限
        case hwSetEnvTempUpperLimit:
        if(sethwCtrlSmokeResetIndex != 0x7FFFFFFF)
        {
            stringstream ssIndex;
            if(mIndex == 1)
            {
                if(sethwSetEnvTempUpperLimitIndex[0] == 0x7FFFFFFF)
                   break;
                ssIndex<<sethwSetEnvTempUpperLimitIndex[0];
            }
            else
            {
                if(sethwSetEnvTempUpperLimitIndex[1] == 0x7FFFFFFF)
                   break;
                ssIndex<<sethwSetEnvTempUpperLimitIndex[1];
            }
            string strIndex = ssIndex.str();
            msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.28.1.1.6." + strIndex + " i " + mIntValue ;
            system(msetoid.c_str());
         }
             break;
        //环境温度告警下限
        case hwSetEnvTempLowerLimit:
        if(sethwCtrlSmokeResetIndex != 0x7FFFFFFF)
        {
            stringstream ssIndex;
            if(mIndex == 1)
            {
                if(sethwSetEnvTempLowerLimitIndex[0] == 0x7FFFFFFF)
                   break;
                ssIndex<<sethwSetEnvTempLowerLimitIndex[0];
            }
            else
            {
                if(sethwSetEnvTempLowerLimitIndex[1] == 0x7FFFFFFF)
                   break;
                ssIndex<<sethwSetEnvTempLowerLimitIndex[1];
            }
            string strIndex = ssIndex.str();
            msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.28.1.1.7." + strIndex + " i " + mIntValue ;
            system(msetoid.c_str());
         }
             break;
        //环境湿度告警上限
        case hwSetEnvHumidityUpperLimit:
        if(sethwCtrlSmokeResetIndex != 0x7FFFFFFF)
        {
            stringstream ssIndex;
            if(mIndex == 1)
            {
                if(sethwSetEnvHumidityUpperLimitIndex[0] == 0x7FFFFFFF)
                   break;
                ssIndex<<sethwSetEnvHumidityUpperLimitIndex[0];
            }
            else
            {
                if(sethwSetEnvHumidityUpperLimitIndex[1] == 0x7FFFFFFF)
                   break;
                ssIndex<<sethwSetEnvHumidityUpperLimitIndex[1];
            }
            string strIndex = ssIndex.str();
            msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.28.1.1.8." + strIndex + " i " + mIntValue ;
            system(msetoid.c_str());
         }
             break;
        //环境湿度告警下限
        case hwSetEnvHumidityLowerLimit:
        if(sethwCtrlSmokeResetIndex != 0x7FFFFFFF)
        {
            stringstream ssIndex;
            if(mIndex == 1)
            {
                if(sethwSetEnvHumidityLowerLimitIndex[0] == 0x7FFFFFFF)
                   break;
                ssIndex<<sethwSetEnvHumidityLowerLimitIndex[0];
            }
            else
            {
                if(sethwSetEnvHumidityLowerLimitIndex[1] == 0x7FFFFFFF)
                   break;
                ssIndex<<sethwSetEnvHumidityLowerLimitIndex[1];
            }
            string strIndex = ssIndex.str();
            msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.28.1.1.9." + strIndex + " i " + mIntValue ;
            system(msetoid.c_str());
         }
             break;
        //空调控制模式 已去掉这个节点
        /*case hwDcAirCtrlMode:
        {
             stringstream ssIndex;
             if(mIndex == 1)
             {
                 if(sethwDcAirCtrlModeIndex[0] == 0x7FFFFFFF)
                    break;
                ssIndex<<sethwDcAirCtrlModeIndex[0];
             }
             else
             {
                 if(sethwDcAirCtrlModeIndex[1] == 0x7FFFFFFF)
                    break;
                ssIndex<<sethwDcAirCtrlModeIndex[1];
             }
             string strIndex = ssIndex.str();
             msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.10.2.2.1.4." + strIndex + " i " + mIntValue ;
             system(msetoid.c_str());
         }
         break;
        */
        //AC过压点设置
        case hwSetAcsUpperVoltLimit:
        if(sethwSetAcsUpperVoltLimitIndex != 0x7FFFFFFF)
        {
             stringstream ssIndex;
             ssIndex<<sethwSetAcsUpperVoltLimitIndex;
             string strIndex = ssIndex.str();
             msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.5.1.5." + strIndex + " u " + mIntValue ;
             system(msetoid.c_str());
         }
         break;
        //AC欠压点设置
        case hwSetAcsLowerVoltLimit:
        if(sethwSetAcsLowerVoltLimitIndex != 0x7FFFFFFF)
        {
             stringstream ssIndex;
             ssIndex<<sethwSetAcsLowerVoltLimitIndex;
             string strIndex = ssIndex.str();
             msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.5.1.6." + strIndex + " u " + mIntValue ;
             system(msetoid.c_str());
         }
         break;
        //设置DC过压点
        case hwSetDcsUpperVoltLimit:
        if(sethwSetDcsUpperVoltLimitIndex != 0x7FFFFFFF)
        {
             stringstream ssIndex;
             ssIndex<<sethwSetDcsUpperVoltLimitIndex;
             string strIndex = ssIndex.str();
             msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.6.1.7." + strIndex + " u " + mIntValue ;
             system(msetoid.c_str());
         }
         break;
        //设置DC欠压点
        case hwSetDcsLowerVoltLimit:
        if(sethwSetDcsLowerVoltLimitIndex != 0x7FFFFFFF)
        {
             stringstream ssIndex;
             ssIndex<<sethwSetDcsLowerVoltLimitIndex;
             string strIndex = ssIndex.str();
             msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.6.1.8." + strIndex + " u " + mIntValue ;
             system(msetoid.c_str());
            // printf("%s\r\n",msetoid.c_str());
         }
         break;
        //控制单板复位
        case hwCtrlMonEquipReset:
        if(sethwCtrlMonEquipResetIndex != 0x7FFFFFFF)
        {
             stringstream ssIndex;
             ssIndex<<sethwCtrlMonEquipResetIndex;
             string strIndex = ssIndex.str();
             msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.2.2.1.1.12." + strIndex + " i " + mIntValue ;
             system(msetoid.c_str());
         }
         break;

       default:
            break;

    }
    
    pthread_mutex_unlock(&SetsnmpoidMutex);
    return 0 ;
}

int mywalkappinit(void)
{
    memset(hwAcbAddrbuf,0x00,10);
    pthread_mutex_init(&snmpdataMutex,NULL);
    pthread_mutex_init(&SetsnmpoidMutex,NULL);
    pthread_mutex_init(&WalksnmpoidMutex,NULL);
    pthread_t m_Walksnmpthread ;
    pthread_create(&m_Walksnmpthread,NULL,Walksnmpthread,NULL);
    printf("Walksnmpthread\r\n");
    pthread_t m_SendWalksnmpthread ;
    pthread_create(&m_SendWalksnmpthread,NULL,WalkSendsnmpthread,NULL);
    printf("WalkSendsnmpthread\r\n");
    return 0 ;
}

















