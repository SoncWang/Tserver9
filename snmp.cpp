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
THUAWEIGantry HUAWEIDevValue;
//extern THUAWEIGantry *stuHUAWEIDevValue;//华为机柜状态

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
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatVolt = mbuf ;
       printf("电池电压:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatVolt).c_str());
       break;
   case hwAcbGroupBatCurr:            //电池电流
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatCurr = mbuf ;
       printf("电池电流:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatCurr).c_str());
       break;
   case hwAcbGroupTotalCapacity:                //电池总容量
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTotalCapacity = mbuf ;
       printf("电池总容量:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTotalCapacity).c_str());
       break ;
   case hwAcbGroupTotalRemainCapacity:               //电池剩余容量
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity = mbuf ;
       printf("电池剩余容量:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity).c_str());
       break;
   case hwAcbGroupBackupTime:              //电池备电时长
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBackupTime = mbuf ;
       printf("电池备电时长:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBackupTime).c_str());
       break;
   case hwAcbGroupBatSoh:             //电池 SOH
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatSoh = mbuf ;
       printf("电池 SOH:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatSoh).c_str());
       break;
   //开关电源
   case hwApOrAblVoltage:                //A/AB 电压
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwApOrAblVoltage = mbuf ;
       printf("A/AB 电压:%s\r\n",(HUAWEIDevValue.strhwApOrAblVoltage).c_str());
        break;
   case hwBpOrBclVoltage:                //B/BC 电压
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwBpOrBclVoltage = mbuf ;
       printf("B/BC 电压:%s\r\n",(HUAWEIDevValue.strhwBpOrBclVoltage).c_str());
       break;
   case hwCpOrCalVoltage:                //C/CA 电压
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwCpOrCalVoltage = mbuf ;
       printf("C/CA 电压:%s\r\n",(HUAWEIDevValue.strhwCpOrCalVoltage).c_str());
       break;
   case hwAphaseCurrent:               //A 相电流
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
	       sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAphaseCurrent = mbuf ;
       printf("A 相电流:%s\r\n",(HUAWEIDevValue.strhwAphaseCurrent).c_str());
       break;
   case hwBphaseCurrent:              //B 相电流
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwBphaseCurrent = mbuf ;
       printf("B 相电流:%s\r\n",(HUAWEIDevValue.strhwBphaseCurrent).c_str());
       break;
   case hwCphaseCurrent:             //C 相电流
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwCphaseCurrent = mbuf ;
       printf("C 相电流:%s\r\n",(HUAWEIDevValue.strhwCphaseCurrent).c_str());
       break;
   case hwDcOutputVoltage:             //DC 输出电压
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwDcOutputVoltage = mbuf ;
       printf("DC 输出电压:%s\r\n",(HUAWEIDevValue.strhwDcOutputVoltage).c_str());
       break;
   case hwDcOutputCurrent:               //DC 输出电流
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
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
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirCtrlMode = mbuf ;
       printf("空调控制模式:%s\r\n",(HUAWEIDevValue.strhwDcAirCtrlMode).c_str());
       break;
   case hwDcAirRunStatus:             //空调运行状态
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirRunStatus = mbuf ;
       printf("空调运行状态:%s\r\n",(HUAWEIDevValue.strhwDcAirRunStatus).c_str());
       break;
   case hwDcAirCompressorRunStatus:             //空调压缩机运行状态
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
//      cout<<strsnmpget<<endl;
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
       session.community_len = strcommunity.size();//strlen(strcommunity.c_str());

       SOCK_STARTUP;
       ss = snmp_open(&session);                     /* establish the session */
       if (!ss) {
          snmp_sess_perror("ack", &session);
          SOCK_CLEANUP;
          //exit(1);
          continue ;
       }
    
       /*
         * Create the PDU for the data for our request.
         *   1) We're going to GET the system.sysDescr.0 node.
       */
       pdu = snmp_pdu_create(SNMP_MSG_GET);
       anOID_len = MAX_OID_LEN;

       if (!snmp_parse_oid(stroid.c_str(), anOID, &anOID_len)) {
          snmp_perror(stroid.c_str());
          snmp_close(ss);
          SOCK_CLEANUP;
          continue ;
       }

       snmp_add_null_var(pdu, anOID, anOID_len);
  
       /*
         * Send the Request out.
       */
       status = snmp_synch_response(ss, pdu, &response);
       /*
         * Process the response.
       */
       if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
          /*
           * SUCCESS: Print the result variables
          */
/*         for(vars = response->variables; vars; vars = vars->next_variable)
         {
           print_variable(vars->name, vars->name_length, vars);     
         }*/
         /* manipuate the information ourselves */
         for(vars = response->variables; vars; vars = vars->next_variable) {
            if (vars->type == ASN_OCTET_STR) {
	       char *sp = (char *)malloc(1 + vars->val_len);
	       memcpy(sp, vars->val.string, vars->val_len);
	       sp[vars->val_len] = '/0';
//               printf("value #%d is a string: %s/n", count++, sp);
	       free(sp);
	    }
            else
            {
              unsigned int IntegerValue = *(vars->val.integer) ;
//              printf("value #%d is a integer: %d\n", count++, IntegerValue);
              //printf("value #%d is NOT a string! Ack!/n", count++);
              UpdataHUAWEIGantry(IntegerValue,(EM_HUAWEIGantry)IntHUAWEIGantry);
              
            }
         }
       } 
       else {
        /*
          * FAILURE: print what went wrong!
        */
        if (status == STAT_SUCCESS)
            fprintf(stderr, "Error in packet/nReason: %s/n",
                snmp_errstring(response->errstat));
        else if (status == STAT_TIMEOUT)
            fprintf(stderr, "Timeout: No response from %s./n",
                session.peername);
        else
            snmp_sess_perror("snmpdemoapp", ss);
      }

      if (response)
        snmp_free_pdu(response);
      snmp_close(ss);
      SOCK_CLEANUP;

      usleep(20000) ;

    

  }

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
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.5.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupBatCurr:            //电池电流
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.6.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupTotalCapacity:                //电池总容量
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.7.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break ;
   case hwAcbGroupTotalRemainCapacity:               //电池剩余容量
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.8.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupBackupTime:              //电池备电时长
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.11.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupBatSoh:             //电池 SOH
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.13.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   //开关电源
   case hwApOrAblVoltage:                //A/AB 电压
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
        break;
   case hwBpOrBclVoltage:                //B/BC 电压
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.5.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwCpOrCalVoltage:                //C/CA 电压
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.6.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAphaseCurrent:               //A 相电流
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.7.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwBphaseCurrent:              //B 相电流
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.8.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwCphaseCurrent:             //C 相电流
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.9.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcOutputVoltage:             //DC 输出电压
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.6.2.1.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcOutputCurrent:               //DC 输出电流
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.6.2.1.1.5.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   //环境传感器
   case hwEnvTemperature:              //环境温度值
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.8.2.1.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwEnvHumidity:             //环境湿度值
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.8.2.2.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   //直流空调
   case hwDcAirCtrlMode:             //空调控制模式
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirRunStatus:             //空调运行状态
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.6.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirCompressorRunStatus:             //空调压缩机运行状态
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.7.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirInnrFanSpeed:             //空调内机转速
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.8.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirOuterFanSpeed:             //空调外风机转速
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.9.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirCompressorRunTime:             //空调压缩机运行时间
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.10.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirEnterChannelTemp:             //空调回风口温度
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.13.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirPowerOnTempPoint:             //空调开机温度点
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.14.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirPowerOffTempPoint:             //空调关机温度点
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.15.1;" + strsend + ";";
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
	   sleep(60);
     for(int i=1;i<=HWGANTRY_COUNT;i++)
     SendHUAWEIsnmp((EM_HUAWEIGantry)i);
   }
   return 0 ;
}

void initHUAWEIGantry()
{
   //锂电
   HUAWEIDevValue.strhwAcbGroupBatVolt="0";                //电池电压 "51.1"
   HUAWEIDevValue.strhwAcbGroupBatCurr="0";            //电池电流
   HUAWEIDevValue.strhwAcbGroupTotalCapacity="0";                //电池总容量
   HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity="0";               //电池剩余容量
   HUAWEIDevValue.strhwAcbGroupBackupTime="0";              //电池备电时长
   HUAWEIDevValue.strhwAcbGroupBatSoh="0";             //电池 SOH
   //开关电源
   HUAWEIDevValue.strhwApOrAblVoltage="0";                //A/AB 电压
   HUAWEIDevValue.strhwBpOrBclVoltage="0";                //B/BC 电压
   HUAWEIDevValue.strhwCpOrCalVoltage="0";                //C/CA 电压
   HUAWEIDevValue.strhwAphaseCurrent="0";               //A 相电流
   HUAWEIDevValue.strhwBphaseCurrent="0";              //B 相电流
   HUAWEIDevValue.strhwCphaseCurrent="0";             //C 相电流
   HUAWEIDevValue.strhwDcOutputVoltage="0";             //DC 输出电压
   HUAWEIDevValue.strhwDcOutputCurrent="0";               //DC 输出电流
   //环境传感器
   HUAWEIDevValue.strhwEnvTemperature="0";              //环境温度值
   HUAWEIDevValue.strhwEnvHumidity="0";            //环境湿度值
   //直流空调
	HUAWEIDevValue.strhwDcAirCtrlMode="0";			//空调控制模式
	HUAWEIDevValue.strhwDcAirRunStatus="0";			//空调运行状态
	HUAWEIDevValue.strhwDcAirCompressorRunStatus="0";		//空调压缩机运行状态
	HUAWEIDevValue.strhwDcAirInnrFanSpeed="0";			//空调内机转速
	HUAWEIDevValue.strhwDcAirOuterFanSpeed="0";			//空调外风机转速
	HUAWEIDevValue.strhwDcAirCompressorRunTime="0";		//空调压缩机运行时间
	HUAWEIDevValue.strhwDcAirEnterChannelTemp="0";		//空调回风口温度
	HUAWEIDevValue.strhwDcAirPowerOnTempPoint="0";		//空调开机温度点
	HUAWEIDevValue.strhwDcAirPowerOffTempPoint="0";		//空调关机温度点
}

int snmpInit(void)
{
   initHUAWEIGantry();
   
   pthread_mutex_init(&snmpoidMutex,NULL);
   pthread_t m_snmpthread ;
   pthread_create(&m_snmpthread,NULL,snmpthread,NULL);

   pthread_t m_Sendsnmpthread ;
   pthread_create(&m_Sendsnmpthread,NULL,Sendsnmpthread,NULL);

   snmptrapInit();
   return 0 ;

}








