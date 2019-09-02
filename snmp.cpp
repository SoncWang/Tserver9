#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <string> 
#include <vector>
#include <sstream> 
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "snmp.h"

using namespace std; 

/* change the word "define" to "undef" to try the (insecure) SNMPv1 version */
#define DEMO_USE_SNMP_VERSION_3
#ifdef DEMO_USE_SNMP_VERSION_3
const char *our_v3_passphrase = "The Net-SNMP Demo Password";
#endif
#define DEFAULT_MIBDIRS "$HOME/.snmp/mibs:/usr/share/snmp/mibs:/opt/xr/net-snmp/share/snmp/mibs"
//int main(int argc, char ** argv)

pthread_mutex_t snmpoidMutex ;   
vector<string> vecSnmp;
extern THUAWEIGantry HUAWEIDevValue;
extern string StrCabinetType;
extern string StrHWServer;		//华为服务器地址
extern void myprintf(char* str);

void UpdataHUAWEIGantryStr(char* mstr,int len,EM_HUAWEIGantry mIntHUAWEIGantry)
{
   pthread_mutex_lock(&snmpoidMutex);

   switch(mIntHUAWEIGantry)
   {
   //锂电
	  case hwMonEquipSoftwareVersion:			 //软件版本
		  HUAWEIDevValue.strhwMonEquipSoftwareVersion = mstr;
		  printf(":%s\r\n",(HUAWEIDevValue.strhwMonEquipSoftwareVersion).c_str());
		  break;
	  case hwMonEquipManufacturer:			 //设备生产商
		  HUAWEIDevValue.strhwMonEquipManufacturer = mstr ;
		  printf(":%s\r\n",(HUAWEIDevValue.strhwMonEquipManufacturer).c_str());
		  break;
	 default:
		 break;


	 }

  pthread_mutex_unlock(&snmpoidMutex);
}

void UpdataHUAWEIGantry(unsigned int mIntegerValue,EM_HUAWEIGantry mIntHUAWEIGantry)
{
   pthread_mutex_lock(&snmpoidMutex);

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

   default:
       break;


   }

    pthread_mutex_unlock(&snmpoidMutex);
}


void *snmpthread(void *param)
{
    netsnmp_session session, *ss;
    netsnmp_pdu *pdu;
    netsnmp_pdu *response;
    oid anOID[MAX_OID_LEN];
    size_t anOID_len;
    netsnmp_variable_list *vars;
    int status;
    int count=1;
	char *sp = (char *)malloc(1024);
	if(sp==NULL)
	{
		printf("snmpthread malloc error!\n");
		return 0;
	}
    init_snmp("snmpdemoapp");
    snmp_sess_init( &session );


	int pos,vecSnmpSize,IntHUAWEIGantry;
	string strsnmpget,strip,strcommunity,stroid,strHUAWEIGantry;
	while(1)
	{
	    //获取数据 ip;community;oid
	    pthread_mutex_lock(&snmpoidMutex);
	    vecSnmpSize = vecSnmp.size();
	    if(vecSnmpSize <= 0)
	    {
			pthread_mutex_unlock(&snmpoidMutex);
			usleep(300000) ;
			continue ;
	    }

		vector<string>::iterator fistSnmp = vecSnmp.begin();
		strsnmpget = *fistSnmp ;
		cout<<strsnmpget<<endl;
		vecSnmp.erase(fistSnmp);
		pthread_mutex_unlock(&snmpoidMutex);

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
			strHUAWEIGantry = strsnmpget.substr(0,pos) ; 
		else
			continue ;

		IntHUAWEIGantry = atoi(strHUAWEIGantry.c_str()) ;

		session.peername = strdup(strip.c_str());
		session.version = SNMP_VERSION_1;
		session.community = (unsigned char*)(strcommunity.c_str());
		session.community_len = strcommunity.size();

		SOCK_STARTUP;
		ss = snmp_open(&session);                     /* establish the session */
		if (!ss) 
		{
			snmp_sess_perror("ack", &session);
			SOCK_CLEANUP;
			//exit(1);
			continue ;
		}
    
		// Create the PDU for the data for our request.
		//  1) We're going to GET the system.sysDescr.0 node.
		pdu = snmp_pdu_create(SNMP_MSG_GET);
		anOID_len = MAX_OID_LEN;

		if (!snmp_parse_oid(stroid.c_str(), anOID, &anOID_len)) 
		{
			snmp_perror(stroid.c_str());
			snmp_close(ss);
			SOCK_CLEANUP;
			continue ;
		}

		snmp_add_null_var(pdu, anOID, anOID_len);

		// Send the Request out.
		status = snmp_synch_response(ss, pdu, &response);
		
		// Process the response.
		if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) 
		{
			// SUCCESS: Print the result variables
			for(vars = response->variables; vars; vars = vars->next_variable)
			{
				print_variable(vars->name, vars->name_length, vars);     
			}
			// manipuate the information ourselves 
			for(vars = response->variables; vars; vars = vars->next_variable) 
			{
				if (vars->type == ASN_OCTET_STR) 
				{
					printf("ASN_OCTET_STR len=%d, str=%s\n",vars->val_len,vars->val.string);
					memcpy(sp, vars->val.string, vars->val_len);
					sp[vars->val_len] = '\0';
					printf("value #%d is a string: %s\n", count++, sp);
					UpdataHUAWEIGantryStr(sp,vars->val_len+1,(EM_HUAWEIGantry)IntHUAWEIGantry);
				}
	            else
	            {
					printf("ASN_OCTET_INT len=%d\n",*(vars->val.integer));
					unsigned int IntegerValue = *(vars->val.integer) ;
					printf("value #%d is a integer: %d\n", count++, IntegerValue);
					//printf("value #%d is NOT a string! Ack!/n", count++);
					UpdataHUAWEIGantry(IntegerValue,(EM_HUAWEIGantry)IntHUAWEIGantry);

				}
			}
		} 
		else 
		{
			// FAILURE: print what went wrong!
			if (status == STAT_SUCCESS)
				fprintf(stderr, "Error in packet\nReason: %s\n",snmp_errstring(response->errstat));
			else if (status == STAT_TIMEOUT)
				fprintf(stderr, "Timeout: No response from %s.\n",session.peername);
			else
				snmp_sess_perror("snmpdemoapp", ss);
		}

		if (response)
		{
			snmp_free_pdu(response);
		}
		snmp_close(ss);
		SOCK_CLEANUP;

		usleep(20000) ;
	}

	free(sp);
	printf("snmpthread exit.\t\r");
	return (0);

} 

int SendSnmpOid(string mSnmpOid)
{
    pthread_mutex_lock(&snmpoidMutex);
    vecSnmp.push_back(mSnmpOid.c_str());
    pthread_mutex_unlock(&snmpoidMutex);
    return 0 ;
}

int SendHUAWEIsnmp(EM_HUAWEIGantry mEM_HUAWEIGantry)
{
   int IntEM = (int)mEM_HUAWEIGantry;
   stringstream ss;
   ss<<IntEM; 
   string strsend = ss.str();

   switch(mEM_HUAWEIGantry)
   {
   //锂电
   case hwAcbGroupBatVolt:               //电池电压
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.5.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupBatCurr:            //电池电流
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.6.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupTotalCapacity:                //电池总容量
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.7.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break ;
   case hwAcbGroupTotalRemainCapacity:               //电池剩余容量
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.8.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupBackupTime:              //电池备电时长
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.11.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupBatSoh:             //电池 SOH
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.13.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   //开关电源
   case hwApOrAblVoltage:                //A/AB 电压
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
        break;
   case hwBpOrBclVoltage:                //B/BC 电压
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.5.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwCpOrCalVoltage:                //C/CA 电压
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.6.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAphaseCurrent:               //A 相电流
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.7.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwBphaseCurrent:              //B 相电流
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.8.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwCphaseCurrent:             //C 相电流
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.9.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcOutputVoltage:             //DC 输出电压
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.6.2.1.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcOutputCurrent:               //DC 输出电流
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.6.2.1.1.5.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   //环境传感器
   case hwEnvTemperature:              //环境温度值
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.28.1.1.4.4101;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwEnvHumidity:             //环境湿度值
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.28.1.1.5.4101;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   //直流空调
   case hwDcAirCtrlMode:             //空调控制模式
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.4.1;" + strsend + ";";
//myprintf(strsend);
       SendSnmpOid(strsend);
       break;
   case hwDcAirRunStatus:             //空调运行状态
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.6.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirCompressorRunStatus:             //空调压缩机运行状态
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.7.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirInnrFanSpeed:             //空调内机转速
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.8.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirOuterFanSpeed:             //空调外风机转速
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.9.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirCompressorRunTime:             //空调压缩机运行时间
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.10.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirEnterChannelTemp:             //空调回风口温度
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.13.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirPowerOnTempPoint:             //空调开机温度点
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.14.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirPowerOffTempPoint:             //空调关机温度点
       strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.15.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwMonEquipSoftwareVersion:			  //软件版本
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.2.2.1.1.5.1;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwMonEquipManufacturer:			  //设备生产商
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.2.2.1.1.7.1;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwAcbGroupTemperature:			  //电池温度
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.9.96;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwAcbGroupOverCurThr:			  //充电过流告警点
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.2.1.14.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwAcbGroupHighTempThr:			  //高温告警点
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.2.1.21.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwAcbGroupLowTempTh:			  //低温告警点
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.2.1.22.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwAcbGroupDodToAcidBattery:			  //锂电放电DOD
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.17.2.1.26.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwSetAcsUpperVoltLimit:			  //AC过压点设置
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.5.1.5.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
	case hwSetAcsLowerVoltLimit:			  //AC欠压点设置
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.5.1.6.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwSetDcsUpperVoltLimit:			  //设置DC过压点
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.6.1.7.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwSetDcsLowerVoltLimit:			 //设置DC欠压点
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.6.1.8.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwSetLvdVoltage:			 //设置LVD电压
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.7.2.1.1.5.1;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwSetEnvTempUpperLimit:			 //环境温度告警上限
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.8.2.1.1.5.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwSetEnvTempLowerLimit:			 //环境温度告警下限
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.8.2.1.1.6.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwSetEnvTempUltraHighTempThreshold:			 //环境高高温告警点
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.8.2.1.1.10.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwSetEnvHumidityUpperLimit:			 //环境湿度告警上限
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.8.2.2.1.5.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwSetEnvHumidityLowerLimit:			 //环境湿度告警下限
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.8.2.2.1.6.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwDcAirRunTime:			 //空调运行时间
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.11.1;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;
   case hwCoolingDevicesMode:			 //温控模式
	   strsend = StrHWServer+";a12345;.1.3.6.1.4.1.2011.6.164.1.10.1.3.0;" + strsend + ";";
	   SendSnmpOid(strsend);
	   break;

   default:
       break;


   }
   return 0 ;
}


void *Sendsnmpthread(void *param)
{

   while(1)
   {
//		initHUAWEIGantry();
		
		for(int i=1;i<=HWGANTRY_COUNT;i++)
		{
			printf("SendHUAWEIsnmp %d\n",i);
			SendHUAWEIsnmp((EM_HUAWEIGantry)i);
		}
		sleep(300);
//		sleep(1);
   }
   printf("Sendsnmpthread exit\t\n");
   return 0 ;
}

void initHUAWEIGantry()
{
   //锂电
   HUAWEIDevValue.strhwAcbGroupBatVolt="2147483647";                //电池电压 "51.1"
   HUAWEIDevValue.strhwAcbGroupBatCurr="2147483647";            //电池电流
   HUAWEIDevValue.strhwAcbGroupTotalCapacity="2147483647";                //电池总容量
   HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity="2147483647";               //电池剩余容量
   HUAWEIDevValue.strhwAcbGroupBackupTime="2147483647";              //电池备电时长
   HUAWEIDevValue.strhwAcbGroupBatSoh="2147483647";             //电池 SOH
   //开关电源
   HUAWEIDevValue.strhwApOrAblVoltage="2147483647";                //A/AB 电压
   HUAWEIDevValue.strhwBpOrBclVoltage="2147483647";                //B/BC 电压
   HUAWEIDevValue.strhwCpOrCalVoltage="2147483647";                //C/CA 电压
   HUAWEIDevValue.strhwAphaseCurrent="2147483647";               //A 相电流
   HUAWEIDevValue.strhwBphaseCurrent="2147483647";              //B 相电流
   HUAWEIDevValue.strhwCphaseCurrent="2147483647";             //C 相电流
   HUAWEIDevValue.strhwDcOutputVoltage="2147483647";             //DC 输出电压
   HUAWEIDevValue.strhwDcOutputCurrent="2147483647";               //DC 输出电流
   //环境传感器
   HUAWEIDevValue.strhwEnvTemperature="2147483647";              //环境温度值
   HUAWEIDevValue.strhwEnvHumidity="2147483647";            //环境湿度值
   //直流空调
	HUAWEIDevValue.strhwDcAirCtrlMode="2147483647";			//空调控制模式
	HUAWEIDevValue.strhwDcAirRunStatus="2147483647";			//空调运行状态
	HUAWEIDevValue.strhwDcAirCompressorRunStatus="2147483647";		//空调压缩机运行状态
	HUAWEIDevValue.strhwDcAirInnrFanSpeed="2147483647";			//空调内机转速
	HUAWEIDevValue.strhwDcAirOuterFanSpeed="2147483647";			//空调外风机转速
	HUAWEIDevValue.strhwDcAirCompressorRunTime="2147483647";		//空调压缩机运行时间
	HUAWEIDevValue.strhwDcAirEnterChannelTemp="2147483647";		//空调回风口温度
	HUAWEIDevValue.strhwDcAirPowerOnTempPoint="2147483647";		//空调开机温度点
	HUAWEIDevValue.strhwDcAirPowerOffTempPoint="2147483647";		//空调关机温度点
	
	//设备信息
	HUAWEIDevValue.strhwMonEquipSoftwareVersion="";	//软件版本
	HUAWEIDevValue.strhwMonEquipManufacturer="";		//设备生产商
	//锂电(新增加)
	HUAWEIDevValue.strhwAcbGroupTemperature="2147483647";			//电池温度
	HUAWEIDevValue.strhwAcbGroupOverCurThr="2147483647";			//充电过流告警点
	HUAWEIDevValue.strhwAcbGroupHighTempThr="2147483647";		//高温告警点
	HUAWEIDevValue.strhwAcbGroupLowTempTh="2147483647";			//低温告警点
	HUAWEIDevValue.strhwAcbGroupDodToAcidBattery="2147483647";	//锂电放电DOD
	//开关电源(新增加)
   	HUAWEIDevValue.strhwSetAcsUpperVoltLimit="2147483647";		//AC过压点设置
   	HUAWEIDevValue.strhwSetAcsLowerVoltLimit="2147483647";		//AC欠压点设置
   	HUAWEIDevValue.strhwSetDcsUpperVoltLimit="2147483647";		//设置DC过压点
   	HUAWEIDevValue.strhwSetDcsLowerVoltLimit="2147483647";		//设置DC欠压点
   	HUAWEIDevValue.strhwSetLvdVoltage="2147483647";				//设置LVD电压
	//环境传感器(新增加)
   	HUAWEIDevValue.strhwSetEnvTempUpperLimit="2147483647";		//环境温度告警上限
   	HUAWEIDevValue.strhwSetEnvTempLowerLimit="2147483647";		//环境温度告警下限
   	HUAWEIDevValue.strhwSetEnvTempUltraHighTempThreshold="2147483647";		//环境高高温告警点
   	HUAWEIDevValue.strhwSetEnvHumidityUpperLimit="2147483647";		//环境湿度告警上限
   	HUAWEIDevValue.strhwSetEnvHumidityLowerLimit="2147483647";		//环境湿度告警下限
	//直流空调(新增加)
	HUAWEIDevValue.strhwDcAirRunTime="2147483647";				//空调运行时间
	HUAWEIDevValue.strhwCoolingDevicesMode="2147483647";			//温控模式
	
	//2019-08-20新增
	HUAWEIDevValue.strhwAcbGroupBatRunningState="2147483647";		//电池状态
	HUAWEIDevValue.strhwDcAirEquipAddress="2147483647";				//空调地址
	HUAWEIDevValue.strhwTemHumEquipAddress="2147483647";			//温湿度地址
	//单个锂电池2019-08-20新增
	HUAWEIDevValue.strhwAcbBatVolt="2147483647";					//单个电池电压
	HUAWEIDevValue.strhwAcbBatCurr="2147483647";					//单个电池电流
	HUAWEIDevValue.strhwAcbBatSoh="2147483647";						//单个电池串SOH
	HUAWEIDevValue.strhwAcbBatCapacity="2147483647";				//单个电池容量
	
    //防火墙
    HUAWEIDevValue.strhwEntityCpuUsage="2147483647";                //CPU 
    HUAWEIDevValue.strhwEntityMemUsage ="2147483647";              //内存使用率
    HUAWEIDevValue.strhwEntityTemperature="2147483647";            //温度
    //交换机
    HUAWEIDevValue.strhwswitchEntityCpuUsage="2147483647";          //CPU 
    HUAWEIDevValue.strhwswitchEntityMemUsage="2147483647";          //内存使用率
    HUAWEIDevValue.strhwswitchEntityTemperature="2147483647";       //温度
}

int snmpInit(void)
{
   if(StrCabinetType=="1")		//机柜类型是华为
   	{
	   pthread_mutex_init(&snmpoidMutex,NULL);
	   
   	   snmptrapInit();
/*	   
	   pthread_t m_Sendsnmpthread ;
	   pthread_create(&m_Sendsnmpthread,NULL,Sendsnmpthread,NULL);

	   pthread_t m_snmpthread ;
	   pthread_create(&m_snmpthread,NULL,snmpthread,NULL);
*/
       mywalkappinit();
   	}
   return 0 ;

}









