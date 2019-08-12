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

using namespace std;

int mywalkappinit();
oid     objid_mib[] = { 1, 3, 6, 1, 2, 1 } ;  
int     numprinted = 0;  
  
int c;  
int count_print = 0;  

pthread_mutex_t WalksnmpoidMutex ;
pthread_mutex_t SetsnmpoidMutex ;

vector<string> vecWalkSnmp;

extern string StrHWServer;		//华为服务器地址
extern string StrHWGetPasswd ;
extern string StrHWSetPasswd ;

extern THUAWEIGantry HUAWEIDevValue;
extern pthread_mutex_t snmpoidMutex ;   

extern string StrFireWareIP;         //防火墙IP
extern string StrFireWareGetPasswd;  //防火墙get密码
extern string StrFireWareSetPasswd;  //防火墙set密码
extern string StrSwitchIP ;//交换机IP
extern string StrSwitchGetPasswd ;//交换机get密码
extern string StrSwitchSetPasswd ;//交换机set密码



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
       printf("电池电压:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatVolt).c_str());
       break;
   case hwAcbGroupBatCurr:            //电池电流
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatCurr = mbuf ;
       printf("电池电流:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatCurr).c_str());
       break;
   case hwAcbGroupTotalCapacity:                //电池总容量
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTotalCapacity = mbuf ;
       printf("电池总容量:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTotalCapacity).c_str());
       break ;
   case hwAcbGroupTotalRemainCapacity:               //电池剩余容量
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity = mbuf ;
       printf("电池剩余容量:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity).c_str());
       break;
   case hwAcbGroupBackupTime:              //电池备电时长
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBackupTime = mbuf ;
       printf("电池备电时长:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBackupTime).c_str());
       break;
   case hwAcbGroupBatSoh:             //电池 SOH
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatSoh = mbuf ;
       printf("电池 SOH:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatSoh).c_str());
       break;
   //开关电源
   case hwApOrAblVoltage:                //A/AB 电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwApOrAblVoltage = mbuf ;
       printf("A/AB 电压:%s\r\n",(HUAWEIDevValue.strhwApOrAblVoltage).c_str());
        break;
   case hwBpOrBclVoltage:                //B/BC 电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwBpOrBclVoltage = mbuf ;
       printf("B/BC 电压:%s\r\n",(HUAWEIDevValue.strhwBpOrBclVoltage).c_str());
       break;
   case hwCpOrCalVoltage:                //C/CA 电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwCpOrCalVoltage = mbuf ;
       printf("C/CA 电压:%s\r\n",(HUAWEIDevValue.strhwCpOrCalVoltage).c_str());
       break;
   case hwAphaseCurrent:               //A 相电流
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
           sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAphaseCurrent = mbuf ;
       printf("A 相电流:%s\r\n",(HUAWEIDevValue.strhwAphaseCurrent).c_str());
       break;
   case hwBphaseCurrent:              //B 相电流
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwBphaseCurrent = mbuf ;
       printf("B 相电流:%s\r\n",(HUAWEIDevValue.strhwBphaseCurrent).c_str());
       break;
   case hwCphaseCurrent:             //C 相电流
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwCphaseCurrent = mbuf ;
       printf("C 相电流:%s\r\n",(HUAWEIDevValue.strhwCphaseCurrent).c_str());
       break;
   case hwDcOutputVoltage:             //DC 输出电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwDcOutputVoltage = mbuf ;
       printf("DC 输出电压:%s\r\n",(HUAWEIDevValue.strhwDcOutputVoltage).c_str());
       break;
   case hwDcOutputCurrent:               //DC 输出电流
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",mIntegerValue) ;
       else
           sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwDcOutputCurrent = mbuf ;
       printf("DC 输出电流:%s\r\n",(HUAWEIDevValue.strhwDcOutputCurrent).c_str());
       break;
   //环境传感器
   case hwEnvTemperature:              //环境温度值
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwEnvTemperature = mbuf ;
       printf("环境温度值:%s\r\n",(HUAWEIDevValue.strhwEnvTemperature).c_str());
       break;
   case hwEnvHumidity:             //环境湿度值
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwEnvHumidity = mbuf ;
       printf("环境湿度值:%s\r\n",(HUAWEIDevValue.strhwEnvHumidity).c_str());
       break;
   //直流空调
   case hwDcAirCtrlMode:             //空调控制模式
       if(mIntegerValue==0xFF)
           sprintf(mbuf,"%d",0x7FFFFFFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirCtrlMode = mbuf ;
       printf("空调控制模式:%s\r\n",(HUAWEIDevValue.strhwDcAirCtrlMode).c_str());
       break;
   case hwDcAirRunStatus:             //空调运行状态
       if(mIntegerValue==0xFF)
           sprintf(mbuf,"%d",0x7FFFFFFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirRunStatus = mbuf ;
       printf("空调运行状态:%s\r\n",(HUAWEIDevValue.strhwDcAirRunStatus).c_str());
       break;
   case hwDcAirCompressorRunStatus:             //空调压缩机运行状态
       if(mIntegerValue==0xFF)
           sprintf(mbuf,"%d",0x7FFFFFFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirCompressorRunStatus = mbuf ;
       printf("空调压缩机运行状态:%s\r\n",(HUAWEIDevValue.strhwDcAirCompressorRunStatus).c_str());
       break;
   case hwDcAirInnrFanSpeed:             //空调内机转速
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirInnrFanSpeed = mbuf ;
       printf("空调内机转速:%s\r\n",(HUAWEIDevValue.strhwDcAirInnrFanSpeed).c_str());
       break;
   case hwDcAirOuterFanSpeed:             //空调外风机转速
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirOuterFanSpeed = mbuf ;
       printf("空调外风机转速:%s\r\n",(HUAWEIDevValue.strhwDcAirOuterFanSpeed).c_str());
       break;
   case hwDcAirCompressorRunTime:             //空调压缩机运行时间
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirCompressorRunTime = mbuf ;
       printf("空调压缩机运行时间:%s\r\n",(HUAWEIDevValue.strhwDcAirCompressorRunTime).c_str());
       break;
   case hwDcAirEnterChannelTemp:             //空调回风口温度
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirEnterChannelTemp = mbuf ;
       printf("空调回风口温度:%s\r\n",(HUAWEIDevValue.strhwDcAirEnterChannelTemp).c_str());
       break;
   case hwDcAirPowerOnTempPoint:             //空调开机温度点
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirPowerOnTempPoint = mbuf ;
       printf("空调开机温度点:%s\r\n",(HUAWEIDevValue.strhwDcAirPowerOnTempPoint).c_str());
       break;
   case hwDcAirPowerOffTempPoint:             //空调关机温度点
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirPowerOffTempPoint = mbuf ;
       printf("空调关机温度点:%s\r\n",(HUAWEIDevValue.strhwDcAirPowerOffTempPoint).c_str());
       break;
    //新增加
   //锂电(新增加)
   case hwAcbGroupTemperature:			  //电池温度
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTemperature = mbuf ;
       printf("电池温度:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTemperature).c_str());
       break;
   case hwAcbGroupOverCurThr:			  //充电过流告警点
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/100,mIntegerValue%100) ;
       HUAWEIDevValue.strhwAcbGroupOverCurThr = mbuf ;
       printf("充电过流告警点:%s\r\n",(HUAWEIDevValue.strhwAcbGroupOverCurThr).c_str());
       break;
   case hwAcbGroupHighTempThr:			  //高温告警点
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwAcbGroupHighTempThr = mbuf ;
       printf("高温告警点:%s\r\n",(HUAWEIDevValue.strhwAcbGroupHighTempThr).c_str());
       break;
   case hwAcbGroupLowTempTh:			  //低温告警点
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwAcbGroupLowTempTh = mbuf ;
       printf("低温告警点:%s\r\n",(HUAWEIDevValue.strhwAcbGroupLowTempTh).c_str());
       break;
   case hwAcbGroupDodToAcidBattery:			  //锂电放电DOD
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwAcbGroupDodToAcidBattery = mbuf ;
       printf("锂电放电DOD:%s\r\n",(HUAWEIDevValue.strhwAcbGroupDodToAcidBattery).c_str());
       break;
   //开关电源(新增加)
   case hwSetAcsUpperVoltLimit:			  //AC过压点设置
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwSetAcsUpperVoltLimit = mbuf ;
       printf("AC过压点设置:%s\r\n",(HUAWEIDevValue.strhwSetAcsUpperVoltLimit).c_str());
       break;
   case hwSetAcsLowerVoltLimit:			  //AC欠压点设置
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwSetAcsLowerVoltLimit = mbuf ;
       printf("AC欠压点设置:%s\r\n",(HUAWEIDevValue.strhwSetAcsLowerVoltLimit).c_str());
       break;
   case hwSetDcsUpperVoltLimit:			  //设置DC过压点
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwSetDcsUpperVoltLimit = mbuf ;
       printf("设置DC过压点:%s\r\n",(HUAWEIDevValue.strhwSetDcsUpperVoltLimit).c_str());
       break;
   case hwSetDcsLowerVoltLimit: 		  //设置DC欠压点
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwSetDcsLowerVoltLimit = mbuf ;
       printf("设置DC欠压点:%s\r\n",(HUAWEIDevValue.strhwSetDcsLowerVoltLimit).c_str());
       break;
   case hwSetLvdVoltage:			  //设置LVD电压
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwSetLvdVoltage = mbuf ;
       printf("设置LVD电压:%s\r\n",(HUAWEIDevValue.strhwSetLvdVoltage).c_str());
       break;
   //环境传感器(新增加)
   case hwSetEnvTempUpperLimit:			  //环境温度告警上限
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwSetEnvTempUpperLimit = mbuf ;
       printf("环境温度告警上限:%s\r\n",(HUAWEIDevValue.strhwSetEnvTempUpperLimit).c_str());
       break;
   case hwSetEnvTempLowerLimit:			  //环境温度告警下限
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
            sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwSetEnvTempLowerLimit = mbuf ;
       printf("环境温度告警下限:%s\r\n",(HUAWEIDevValue.strhwSetEnvTempLowerLimit).c_str());
       break;
   case hwSetEnvTempUltraHighTempThreshold:			  //环境高高温告警点
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold = mbuf ;
       printf("环境高高温告警点:%s\r\n",(HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold).c_str());
       break;
   case hwSetEnvHumidityUpperLimit:			  //环境湿度告警上限
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwSetEnvHumidityUpperLimit = mbuf ;
       printf("环境湿度告警上限:%s\r\n",(HUAWEIDevValue.strhwSetEnvHumidityUpperLimit).c_str());
       break;
   case hwSetEnvHumidityLowerLimit:			  //环境湿度告警下限
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwSetEnvHumidityLowerLimit = mbuf ;
       printf("环境湿度告警下限:%s\r\n",(HUAWEIDevValue.strhwSetEnvHumidityLowerLimit).c_str());
       break;
   //直流空调(新增加)
   case hwDcAirRunTime:			  //空调运行时间
       if(mIntegerValue==0x7FFFFFFF)
           sprintf(mbuf,"%d",0x7FFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirRunTime = mbuf ;
       printf("空调运行时间:%s\r\n",(HUAWEIDevValue.strhwDcAirRunTime).c_str());
       break;
   case hwCoolingDevicesMode:			  //温控模式
       if(mIntegerValue==0xFF)
           sprintf(mbuf,"%d",0x7FFFFFFF) ;
       else
           sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwCoolingDevicesMode = mbuf ;
       printf("温控模式:%s\r\n",(HUAWEIDevValue.strhwCoolingDevicesMode).c_str());
       break;
   //防火墙
   case hwEntityCpuUsage:               //CPU
       sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwEntityCpuUsage = mbuf ;
       printf("防火墙CPU:%s\r\n",(HUAWEIDevValue.strhwEntityCpuUsage).c_str());
       break;
    //交换机
    case hwswitchEntityCpuUsage:               //CPU
         sprintf(mbuf,"%d",mIntegerValue) ;
         HUAWEIDevValue.strhwswitchEntityCpuUsage = mbuf ;
         printf("交换机CPU:%s\r\n",(HUAWEIDevValue.strhwswitchEntityCpuUsage).c_str());
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
          strsend = StrFireWareIP+ ";" + StrFireWareGetPasswd + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.5;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
          //发送查询内存使用率 mgetindx
          IntEM = (int)hwEntityMemUsage;
          stringstream ssmem;
          ssmem<<IntEM;
          strsend = ssmem.str();

          strsend = StrFireWareIP+ ";" + StrFireWareGetPasswd + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.7;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
          //查询温度
          IntEM = (int)hwEntityTemperature;
          stringstream sstemper;
          sstemper<<IntEM;
          strsend = sstemper.str();

          strsend = StrFireWareIP+ ";" + StrFireWareGetPasswd + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.11;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
        }
        break ;
    case hwEntityMemUsage:
        {
           //获取内存使用率
           //pthread_mutex_lock(&snmpoidMutex);
           HUAWEIDevValue.strhwEntityMemUsage = getsp;
           //pthread_mutex_unlock(&snmpoidMutex);
           printf("防火墙内存使用率:%s\r\n",getsp.c_str());
        }
        break;
    case hwEntityTemperature:
        {
           //获取查询温度
           //pthread_mutex_lock(&snmpoidMutex);
           HUAWEIDevValue.strhwEntityTemperature = getsp;
           //pthread_mutex_unlock(&snmpoidMutex);
           printf("防火墙温度:%s\r\n",getsp.c_str());
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
          strsend = StrSwitchIP+ ";" + StrSwitchGetPasswd + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.5;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
          //发送查询内存使用率 mgetindx
          IntEM = (int)hwswitchEntityMemUsage;
          stringstream ssmem;
          ssmem<<IntEM;
          strsend = ssmem.str();

          strsend = StrSwitchIP+ ";" + StrSwitchGetPasswd + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.7;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
          //查询温度
          IntEM = (int)hwswitchEntityTemperature;
          stringstream sstemper;
          sstemper<<IntEM;
          strsend = sstemper.str();

          strsend = StrSwitchIP+ ";" + StrSwitchGetPasswd + ";" + ".1.3.6.1.4.1.2011.5.25.31.1.1.1.1.11;" + "15;" + strmgetindx + ";" + "RPU;" + strsend+ ";" ;
          SendWalkSnmpOid(strsend);
        }
        break;
    case hwswitchEntityMemUsage:
        {
          //获取内存使用率
          //pthread_mutex_lock(&snmpoidMutex);
          HUAWEIDevValue.strhwswitchEntityMemUsage = getsp;
          //pthread_mutex_unlock(&snmpoidMutex);
          printf("交换机内存使用率:%s\r\n",getsp.c_str());
       }
       break;
    case hwswitchEntityTemperature:
       {
         //获取查询温度
         //pthread_mutex_lock(&snmpoidMutex);
         HUAWEIDevValue.strhwswitchEntityTemperature = getsp;
         //pthread_mutex_unlock(&snmpoidMutex);
         printf("交换机温度:%s\r\n",getsp.c_str());
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
       cout<<strsnmpget<<endl;
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
                        printf("not part of this subtree\r\n");
                        continue;  
                    }  
                    numprinted++;  
                   // print_variable(vars->name, vars->name_length, vars);
                    if (vars->type == ASN_OCTET_STR)
                    {
                        string getsp ;
                        char *sp = (char *)malloc(1 + vars->val_len); 
                        memcpy(sp, vars->val.string, vars->val_len); 
                        sp[vars->val_len] = '\0'; 
                        //printf("string:%s\r\n",sp);
                        getsp = sp ;
                        free(sp);

                        if(Intstroidtype == 0)
                        {
                           if(getsp == strHUAWEIGantry)
                           {
                               int mgetindx = vars->name_loc[Intstroidindx] ;
                               UpdataHUAWEIFirewall(mgetindx,strHUAWEIGantry,Intstrtype);
                           }
                        }


                        //string Stroid = vars->name_loc;

                        //printf("%s\r\n",Stroid.c_str()) ;
                        //for(int n=0;n<128;n++)
                            //printf(".%d",vars->name_loc[n]);
                       // printf("\r\n");


                    }
                    else if((vars->type == ASN_INTEGER) || (vars->type == ASN_GAUGE))
                    {
                        unsigned int IntegerValue = *(vars->val.integer) ;
                        stringstream ssIntegerValue;
                        ssIntegerValue<<IntegerValue;
                        string getsp = ssIntegerValue.str();

                        if(Intstroidtype == 0x7FFFFFFF)
                        {
                           //全部获取
                           UpdataHUAWEIFirewall(Intstroidtype,getsp,Intstrtype);
                        }
                        else if(Intstroidtype == vars->name_loc[Intstroidindx])
                        {
                           UpdataHUAWEIFirewall(Intstroidtype,getsp,Intstrtype);
                           //printf("UpdataHUAWEIFirewall:%d,%d\r\n",Intstroidtype,IntegerValue);
                        }
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
                    printf("End of MIB\n");  
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
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.4;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
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
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.2.1.1.5;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetEnvTempLowerLimit:			 //环境温度告警下限
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.2.1.1.6;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetEnvTempUltraHighTempThreshold:			 //环境高高温告警点
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.2.1.1.10;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetEnvHumidityUpperLimit:			 //环境湿度告警上限
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.2.2.1.5;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
       SendWalkSnmpOid(strsend);
       break;
   case hwSetEnvHumidityLowerLimit:			 //环境湿度告警下限
       strsend = StrHWServer+";"+StrHWGetPasswd + ";" + ".1.3.6.1.4.1.2011.6.164.1.8.2.2.1.6;" + "15;" + "2147483647‬;" + "RPU;" + strsend+ ";" ;
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

   default:
       break;


   }
   return 0 ;
}




void *WalkSendsnmpthread(void *param)
{
    int IntEM;

    string strsend ;
    while(1)
    {
       //查询CPU使用率
       // IntEM = (int)hwEntityCpuUCheck;
        //stringstream sscpu;
       // sscpu<<IntEM;
       // strsend = sscpu.str();

       strsend = StrFireWareIP+";"+StrFireWareGetPasswd + ";" + ".1.3.6.1.2.1.47.1.1.1.1.7;" + "12;" + "0;" + "RPU;" + "10000;";
       SendWalkSnmpOid(strsend);
       sleep(5);

      // IntEM = (int)hwswitchEntityCpuCheck;
      // stringstream sscpu;
       //sscpu<<IntEM;
       //strsend = sscpu.str();

       strsend = StrSwitchIP+";"+StrSwitchGetPasswd + ";" + ".1.3.6.1.2.1.47.1.1.1.1.7;" + "12;" + "0;" + "MPU Board 0;" + "11000;";
       SendWalkSnmpOid(strsend);
       sleep(5);

       for(int i=1;i<=HWGANTRY_COUNT;i++)
       {
//			printf("SendHUAWEIsnmp %d\n",i);
           SendHUAWEIsnmpWalk((EM_HUAWEIGantry)i);
       }

       sleep(25);


    }

    return 0 ;
}

int SnmpSetOid(EM_HUAWEIGantry mEM_HUAWEIGantry,string mIntValue)
{
    string msetoid ;
    pthread_mutex_lock(&SetsnmpoidMutex);
    switch(mEM_HUAWEIGantry)
    {
       //温控模式
       case hwCoolingDevicesModeCtl:
            msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.10.1.3.0 i " + mIntValue ;
            system(msetoid.c_str());
            break;
       //空调开机温度点
       case hwDcAirPowerOnTempPointCtl:
           msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.10.2.2.1.14.1 i " + mIntValue ;
            system(msetoid.c_str());
            break;
       //空调关机温度点
       case hwDcAirPowerOffTempPointCtl:
            msetoid = "./snmpset -v 2c -c " + StrHWSetPasswd + " " + StrHWServer + " .1.3.6.1.4.1.2011.6.164.1.10.2.2.1.15.1 i " + mIntValue ;
            system(msetoid.c_str());
            break;
       default:
            break;

    }
    
    pthread_mutex_unlock(&SetsnmpoidMutex);
    return 0 ;
}

int mywalkappinit(void)
{
    pthread_mutex_init(&SetsnmpoidMutex,NULL);
    pthread_mutex_init(&WalksnmpoidMutex,NULL);
    pthread_t m_Walksnmpthread ;
    pthread_create(&m_Walksnmpthread,NULL,Walksnmpthread,NULL);

    pthread_t m_SendWalksnmpthread ;
    pthread_create(&m_SendWalksnmpthread,NULL,WalkSendsnmpthread,NULL);
    return 0 ;
}

















