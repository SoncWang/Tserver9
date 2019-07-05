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
//extern THUAWEIGantry *stuHUAWEIDevValue;//��Ϊ����״̬

void UpdataHUAWEIGantry(unsigned int mIntegerValue,EM_HUAWEIGantry mIntHUAWEIGantry)
{
   pthread_mutex_lock(&snmpoidMutex);

   char mbuf[50];
   memset(mbuf,0,50);
   switch(mIntHUAWEIGantry)
   {
   //﮵�
   case hwAcbGroupBatVolt:               //��ص�ѹ
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatVolt = mbuf ;
       printf("��ص�ѹ:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatVolt).c_str());
       break;
   case hwAcbGroupBatCurr:            //��ص���
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatCurr = mbuf ;
       printf("��ص���:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatCurr).c_str());
       break;
   case hwAcbGroupTotalCapacity:                //���������
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTotalCapacity = mbuf ;
       printf("���������:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTotalCapacity).c_str());
       break ;
   case hwAcbGroupTotalRemainCapacity:               //���ʣ������
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity = mbuf ;
       printf("���ʣ������:%s\r\n",(HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity).c_str());
       break;
   case hwAcbGroupBackupTime:              //��ر���ʱ��
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBackupTime = mbuf ;
       printf("��ر���ʱ��:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBackupTime).c_str());
       break;
   case hwAcbGroupBatSoh:             //��� SOH
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAcbGroupBatSoh = mbuf ;
       printf("��� SOH:%s\r\n",(HUAWEIDevValue.strhwAcbGroupBatSoh).c_str());
       break;
   //���ص�Դ
   case hwApOrAblVoltage:                //A/AB ��ѹ
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwApOrAblVoltage = mbuf ;
       printf("A/AB ��ѹ:%s\r\n",(HUAWEIDevValue.strhwApOrAblVoltage).c_str());
        break;
   case hwBpOrBclVoltage:                //B/BC ��ѹ
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwBpOrBclVoltage = mbuf ;
       printf("B/BC ��ѹ:%s\r\n",(HUAWEIDevValue.strhwBpOrBclVoltage).c_str());
       break;
   case hwCpOrCalVoltage:                //C/CA ��ѹ
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwCpOrCalVoltage = mbuf ;
       printf("C/CA ��ѹ:%s\r\n",(HUAWEIDevValue.strhwCpOrCalVoltage).c_str());
       break;
   case hwAphaseCurrent:               //A �����
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
	       sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwAphaseCurrent = mbuf ;
       printf("A �����:%s\r\n",(HUAWEIDevValue.strhwAphaseCurrent).c_str());
       break;
   case hwBphaseCurrent:              //B �����
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwBphaseCurrent = mbuf ;
       printf("B �����:%s\r\n",(HUAWEIDevValue.strhwBphaseCurrent).c_str());
       break;
   case hwCphaseCurrent:             //C �����
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwCphaseCurrent = mbuf ;
       printf("C �����:%s\r\n",(HUAWEIDevValue.strhwCphaseCurrent).c_str());
       break;
   case hwDcOutputVoltage:             //DC �����ѹ
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       		sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwDcOutputVoltage = mbuf ;
       printf("DC �����ѹ:%s\r\n",(HUAWEIDevValue.strhwDcOutputVoltage).c_str());
       break;
   case hwDcOutputCurrent:               //DC �������
	   if(mIntegerValue==0x7FFFFFFF)
		   sprintf(mbuf,"0") ;
	   else    
       	   sprintf(mbuf,"%d.%d",mIntegerValue/10,mIntegerValue%10) ;
       HUAWEIDevValue.strhwDcOutputCurrent = mbuf ;
       printf("DC �������:%s\r\n",(HUAWEIDevValue.strhwDcOutputCurrent).c_str());
       break;
   //����������
   case hwEnvTemperature:              //�����¶�ֵ
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwEnvTemperature = mbuf ;
       printf("�����¶�ֵ:%s\r\n",(HUAWEIDevValue.strhwEnvTemperature).c_str());
       break;
   case hwEnvHumidity:             //����ʪ��ֵ
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwEnvHumidity = mbuf ;
       printf("����ʪ��ֵ:%s\r\n",(HUAWEIDevValue.strhwEnvHumidity).c_str());
       break;
   //ֱ���յ�
   case hwDcAirCtrlMode:             //�յ�����ģʽ
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirCtrlMode = mbuf ;
       printf("�յ�����ģʽ:%s\r\n",(HUAWEIDevValue.strhwDcAirCtrlMode).c_str());
       break;
   case hwDcAirRunStatus:             //�յ�����״̬
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirRunStatus = mbuf ;
       printf("�յ�����״̬:%s\r\n",(HUAWEIDevValue.strhwDcAirRunStatus).c_str());
       break;
   case hwDcAirCompressorRunStatus:             //�յ�ѹ��������״̬
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirCompressorRunStatus = mbuf ;
       printf("�յ�ѹ��������״̬:%s\r\n",(HUAWEIDevValue.strhwDcAirCompressorRunStatus).c_str());
       break;
   case hwDcAirInnrFanSpeed:             //�յ��ڻ�ת��
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirInnrFanSpeed = mbuf ;
       printf("�յ��ڻ�ת��:%s\r\n",(HUAWEIDevValue.strhwDcAirInnrFanSpeed).c_str());
       break;
   case hwDcAirOuterFanSpeed:             //�յ�����ת��
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirOuterFanSpeed = mbuf ;
       printf("�յ�����ת��:%s\r\n",(HUAWEIDevValue.strhwDcAirOuterFanSpeed).c_str());
       break;
   case hwDcAirCompressorRunTime:             //�յ�ѹ��������ʱ��
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirCompressorRunTime = mbuf ;
       printf("�յ�ѹ��������ʱ��:%s\r\n",(HUAWEIDevValue.strhwDcAirCompressorRunTime).c_str());
       break;
   case hwDcAirEnterChannelTemp:             //�յ��ط���¶�
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirEnterChannelTemp = mbuf ;
       printf("�յ��ط���¶�:%s\r\n",(HUAWEIDevValue.strhwDcAirEnterChannelTemp).c_str());
       break;
   case hwDcAirPowerOnTempPoint:             //�յ������¶ȵ�
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirPowerOnTempPoint = mbuf ;
       printf("�յ������¶ȵ�:%s\r\n",(HUAWEIDevValue.strhwDcAirPowerOnTempPoint).c_str());
       break;
   case hwDcAirPowerOffTempPoint:             //�յ��ػ��¶ȵ�
	   sprintf(mbuf,"%d",mIntegerValue) ;
       HUAWEIDevValue.strhwDcAirPowerOffTempPoint = mbuf ;
       printf("�յ��ػ��¶ȵ�:%s\r\n",(HUAWEIDevValue.strhwDcAirPowerOffTempPoint).c_str());
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
    //��ȡ���� ip;community;oid
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
   //﮵�
   case hwAcbGroupBatVolt:               //��ص�ѹ
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.5.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupBatCurr:            //��ص���
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.6.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupTotalCapacity:                //���������
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.7.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break ;
   case hwAcbGroupTotalRemainCapacity:               //���ʣ������
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.8.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupBackupTime:              //��ر���ʱ��
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.11.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAcbGroupBatSoh:             //��� SOH
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.17.1.1.13.96;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   //���ص�Դ
   case hwApOrAblVoltage:                //A/AB ��ѹ
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
        break;
   case hwBpOrBclVoltage:                //B/BC ��ѹ
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.5.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwCpOrCalVoltage:                //C/CA ��ѹ
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.6.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwAphaseCurrent:               //A �����
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.7.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwBphaseCurrent:              //B �����
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.8.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwCphaseCurrent:             //C �����
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.5.2.1.1.9.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcOutputVoltage:             //DC �����ѹ
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.6.2.1.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcOutputCurrent:               //DC �������
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.6.2.1.1.5.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   //����������
   case hwEnvTemperature:              //�����¶�ֵ
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.8.2.1.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwEnvHumidity:             //����ʪ��ֵ
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.8.2.2.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   //ֱ���յ�
   case hwDcAirCtrlMode:             //�յ�����ģʽ
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.4.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirRunStatus:             //�յ�����״̬
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.6.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirCompressorRunStatus:             //�յ�ѹ��������״̬
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.7.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirInnrFanSpeed:             //�յ��ڻ�ת��
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.8.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirOuterFanSpeed:             //�յ�����ת��
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.9.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirCompressorRunTime:             //�յ�ѹ��������ʱ��
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.10.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirEnterChannelTemp:             //�յ��ط���¶�
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.13.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirPowerOnTempPoint:             //�յ������¶ȵ�
       strsend = "128.8.82.233;a12345;.1.3.6.1.4.1.2011.6.164.1.10.2.2.1.14.1;" + strsend + ";";
       SendSnmpOid(strsend);
       break;
   case hwDcAirPowerOffTempPoint:             //�յ��ػ��¶ȵ�
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
   //﮵�
   HUAWEIDevValue.strhwAcbGroupBatVolt="0";                //��ص�ѹ "51.1"
   HUAWEIDevValue.strhwAcbGroupBatCurr="0";            //��ص���
   HUAWEIDevValue.strhwAcbGroupTotalCapacity="0";                //���������
   HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity="0";               //���ʣ������
   HUAWEIDevValue.strhwAcbGroupBackupTime="0";              //��ر���ʱ��
   HUAWEIDevValue.strhwAcbGroupBatSoh="0";             //��� SOH
   //���ص�Դ
   HUAWEIDevValue.strhwApOrAblVoltage="0";                //A/AB ��ѹ
   HUAWEIDevValue.strhwBpOrBclVoltage="0";                //B/BC ��ѹ
   HUAWEIDevValue.strhwCpOrCalVoltage="0";                //C/CA ��ѹ
   HUAWEIDevValue.strhwAphaseCurrent="0";               //A �����
   HUAWEIDevValue.strhwBphaseCurrent="0";              //B �����
   HUAWEIDevValue.strhwCphaseCurrent="0";             //C �����
   HUAWEIDevValue.strhwDcOutputVoltage="0";             //DC �����ѹ
   HUAWEIDevValue.strhwDcOutputCurrent="0";               //DC �������
   //����������
   HUAWEIDevValue.strhwEnvTemperature="0";              //�����¶�ֵ
   HUAWEIDevValue.strhwEnvHumidity="0";            //����ʪ��ֵ
   //ֱ���յ�
	HUAWEIDevValue.strhwDcAirCtrlMode="0";			//�յ�����ģʽ
	HUAWEIDevValue.strhwDcAirRunStatus="0";			//�յ�����״̬
	HUAWEIDevValue.strhwDcAirCompressorRunStatus="0";		//�յ�ѹ��������״̬
	HUAWEIDevValue.strhwDcAirInnrFanSpeed="0";			//�յ��ڻ�ת��
	HUAWEIDevValue.strhwDcAirOuterFanSpeed="0";			//�յ�����ת��
	HUAWEIDevValue.strhwDcAirCompressorRunTime="0";		//�յ�ѹ��������ʱ��
	HUAWEIDevValue.strhwDcAirEnterChannelTemp="0";		//�յ��ط���¶�
	HUAWEIDevValue.strhwDcAirPowerOnTempPoint="0";		//�յ������¶ȵ�
	HUAWEIDevValue.strhwDcAirPowerOffTempPoint="0";		//�յ��ػ��¶ȵ�
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








