
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>   
#include <limits.h> 
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "comport.h"
#include "comserver.h"
#include "MyCritical.h"
#include <string>
#include <semaphore.h>  
#include "Protocol.h"


using namespace std;//�??��?�个??空�??
CMyCritical ConfigCri;

string STRCONFIG = "";
string STRWIFIWAP = "";

string StrID;			//Ӳ��ID
string StrstationID;	//����վ���
string StrstationName;	//����վ��
string StrNET;			//���緽ʽ
string StrDHCP;			//�Ƿ�DHCP
string StrIP;			//IP��ַ
string StrMask;			//��������
string StrGateway;		//����
string StrDNS;			//DNS��ַ
string StrSERVER;		//��������ַ
string StrServerURL1;	//�����URL1
string StrServerURL2;	//�����URL2
string StrServerURL3;	//�����URL3
string StrStationURL;	//����վ��URL
string StrRSUIP;	//RSUIP��ַ
string StrRSUPort;	//RSU�˿�
string StrVehPlate1IP;	//ʶ����1IP��ַ
string StrVehPlate1Port;	//ʶ����1�˿�
string StrCAMIP;	//�������ͷIP��ַ
string StrCAMPort;	//�������ͷ�˿�

string StrdeviceType="LTKJ-VMCTRL-101";	//�豸�ͺ�
string StrVersionNo="V1.00.00" ;	//������汾��

string StrFlagNetRoadID="4420";	//ETC �ż�·�����
string StrFlagRoadID="52";		//ETC �ż�·�α��
string StrFlagID="4";			//ETC �żܱ��
string StrPosId="1";			//ETC �ż����
string StrDirection="1";		//�г�����
string StrDirDescription="GuangZhou to ShenZhen";	//�г�����˵��


string StrWIFIUSER;		//WIIFI�û���
string StrWIFIKEY;		//WIIFI����
//deviceType=LTKJ-VMCTL-101

int Writeconfig(void);
int Setconfig(string StrKEY,string StrSetconfig) ;
int WriteWificonfig(void);
int SetWificonfig(string StrKEY,string StrSetconfig);

string getstring(string str,string strkey)
{
  string strget = "";
  int lenpos ;
  lenpos = str.find(strkey) ;
  if(lenpos >= 0)
  {
      str = str.substr(lenpos+strkey.length()) ; 
  }
  else
     return strget ;

  lenpos = str.find('\n') ;
  if(lenpos >= 0)
  {
      str = str.substr(0,lenpos) ; 
      strget = str ;
  }
  else
     return strget ;
  

  return strget ;
}


int GetConfig(void)
{
    char strbuf[1501]; 
    memset(strbuf,0x00,1501) ;

    char strwifibuf[1001]; 
    memset(strwifibuf,0x00,1001) ;

    StrID  = "" ;			//Ӳ��ID
	StrstationID  = "" ;	//����վ���
	StrstationName  = "" ;	//����վ��
    StrNET = "" ;			//���緽ʽ
    StrDHCP = "";			//�Ƿ�DHCP
    StrIP  = "" ;			//IP��ַ
    StrMask = "" ;			//��������
    StrGateway = "";		//����
    StrDNS = "";			//DNS��ַ
    StrSERVER = "";			//��������ַ
	StrServerURL1 = "";		//�����URL1
	StrServerURL2 = "";		//�����URL2
//    StrAlarmURL = "";		//�澯��������ַ
	StrStationURL = "";		//����վ��URL
	StrdeviceType = "";		//�豸�ͺ�
	StrVersionNo = "" ;		//������汾��
    StrWIFIUSER = "";		//WIIFI�û���
    StrWIFIKEY = "" ;		//WIIFI����
	StrRSUIP = "" ;			//RSU IP ��ַ
	StrRSUPort = "" ;		//RSU�˿�

    ConfigCri.Lock();
    //read config
    FILE* fdd ;
    if((fdd=fopen("/opt/config", "rb"))==NULL)
    {
        printf("read config erro\r\n");
        ConfigCri.UnLock();
        return 1;
    }
    fread(strbuf,1,1500,fdd);
    fclose(fdd);


/*    FILE* fdwifi ;
    if((fdwifi=fopen("/opt/wpa_supplicant.conf", "rb"))==NULL)
    {
        printf("read wpa_supplicant.conf erro\r\n");
        ConfigCri.UnLock();
        return 1;
    }
    fread(strwifibuf,1,1000,fdwifi);
    fclose(fdwifi);*/

/*    StrID  = "" ;			//Ӳ��ID
	StrstationID  = "" ;	//����վ���
	StrstationName  = "" ;	//����վ��
    StrNET = "" ;			//���緽ʽ
    StrDHCP = "";			//�Ƿ�DHCP
    StrIP  = "" ;			//IP��ַ
    StrMask = "" ;			//��������
    StrGateway = "";		//����
    StrDNS = "";			//DNS��ַ
    StrSERVER = "";			//��������ַ
	StrServerURL1 = "";		//�����URL1
	StrServerURL2 = "";		//�����URL2
    StrAlarmURL = "";		//�澯��������ַ
	StrStationURL = "";		//����վ��URL
	StrdeviceType = "";		//�豸�ͺ�
	StrVersionNo = "" ;		//������汾��*/
    
    StrVersionNo = VERSIONNO ;
    printf("Version:%s\n",StrVersionNo.c_str()) ;

    printf("-----config----\n%s\n----end config----\n",strbuf) ;
    STRCONFIG = strbuf ;

    printf("-----wpa----\n%s\n----end config----\n",strwifibuf) ;
    STRWIFIWAP = strwifibuf ;
    

    string StrConfig = STRCONFIG ;
    string Strkey ;

    Strkey = "ID=";
    StrID = getstring(StrConfig,Strkey) ;

    Strkey = "NET=";
    StrNET = getstring(StrConfig,Strkey) ;

    Strkey = "DHCP=";
    StrDHCP = getstring(StrConfig,Strkey) ;

    Strkey = "IP=";
    StrIP = getstring(StrConfig,Strkey) ;

    Strkey = "Mask=";
    StrMask = getstring(StrConfig,Strkey) ;

    Strkey = "Gateway=";
    StrGateway = getstring(StrConfig,Strkey) ;

    Strkey = "DNS=";
    StrDNS = getstring(StrConfig,Strkey) ;

    Strkey = "SERVER=";
    StrSERVER = getstring(StrConfig,Strkey) ;

    Strkey = "ServerURL1=";
    StrServerURL1 = getstring(StrConfig,Strkey) ;

    Strkey = "ServerURL2=";
    StrServerURL2 = getstring(StrConfig,Strkey) ;

//    Strkey = "AlarmURL=";
//    StrAlarmURL = getstring(StrConfig,Strkey) ;

    Strkey = "address=";					//RSU IP ��ַ
    StrRSUIP = getstring(StrConfig,Strkey) ;

    Strkey = "port=";						//RSU�˿�
    StrRSUPort = getstring(StrConfig,Strkey) ;

/*
    Strkey = "WIFIUSER=";
    StrWIFIUSER = getstring(StrConfig,Strkey) ;

    Strkey = "WIFIKEY=";
    StrWIFIKEY = getstring(StrConfig,Strkey) ;
*/

    ConfigCri.UnLock();
    return 0 ;
}



int Setconfig(string StrKEY,string StrSetconfig)
{
    string setconfig = ""; 
    string strstart = "";
    string strend = "";
    
    ConfigCri.Lock();
    setconfig =  STRCONFIG ;

    int lenpos ;
    lenpos = setconfig.find(StrKEY) ;
    if(lenpos >= 0)
    {
       strstart = setconfig.substr(0,lenpos+StrKEY.length()) ; 
    }
    else
    {
       ConfigCri.UnLock();
       return 1 ;
    }
 
    lenpos = setconfig.find('\n',lenpos) ;
    if(lenpos >= 0)
    {
        strend = setconfig.substr(lenpos);
    }
    else
    {
        ConfigCri.UnLock();
        return 1 ;
    }

    STRCONFIG = strstart + StrSetconfig + strend ;
    ConfigCri.UnLock();

    return 0 ;
}

int Writeconfig(void)
{
    ConfigCri.Lock();
    string setconfig  = STRCONFIG ;
    FILE* fdd ;
    fdd	= fopen("/opt/config", "wb");
    if(fdd == NULL)
    {
       ConfigCri.UnLock();
       return 1 ;
    }
	
//printf("setconfig=%s\r\n",setconfig.c_str() );
    int len = setconfig.length();
    fwrite(setconfig.c_str(),len, 1, fdd);
    fflush(fdd);
    fclose(fdd);
    ConfigCri.UnLock();
    
    return 0 ;

}

int SetWificonfig(string StrKEY,string StrSetconfig)
{
    string setconfig = ""; 
    string strstart = "";
    string strend = "";
    
    ConfigCri.Lock();
    setconfig =  STRWIFIWAP ;

    int lenpos ;
    lenpos = setconfig.find(StrKEY) ;
    if(lenpos >= 0)
    {
       strstart = setconfig.substr(0,lenpos+StrKEY.length()) ; 
    }
    else
    {
       ConfigCri.UnLock();
       return 1 ;
    }
 
    lenpos = setconfig.find('\n',lenpos) ;
    if(lenpos >= 0)
    {
        strend = setconfig.substr(lenpos);
    }
    else
    {
        ConfigCri.UnLock();
        return 1 ;
    }

    STRWIFIWAP = strstart + StrSetconfig + strend ;
    ConfigCri.UnLock();

    return 0 ;
}

int WriteWificonfig(void)
{
    ConfigCri.Lock();
    string setconfig  = STRWIFIWAP ;
    FILE* fdd ;
    fdd	= fopen("/opt/wpa_supplicant.conf", "wb");
    if(fdd == NULL)
    {
       ConfigCri.UnLock();
       return 1 ;
    }

    int len = setconfig.length();
    fwrite(setconfig.c_str(),len, 1, fdd);
    fflush(fdd);
    fclose(fdd);
    ConfigCri.UnLock();
    
    return 0 ;

}







