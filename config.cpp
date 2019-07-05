
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


using namespace std;//寮??ユ?翠釜??绌洪??
CMyCritical ConfigCri;

string STRCONFIG = "";
string STRWIFIWAP = "";

string StrID;			//硬件ID
string StrstationID;	//虚拟站编号
string StrstationName;	//虚拟站名
string StrNET;			//网络方式
string StrDHCP;			//是否DHCP
string StrIP;			//IP地址
string StrMask;			//子网掩码
string StrGateway;		//网关
string StrDNS;			//DNS地址
string StrSERVER;		//服务器地址
string StrServerURL1;	//服务端URL1
string StrServerURL2;	//服务端URL2
string StrServerURL3;	//服务端URL3
string StrStationURL;	//虚拟站端URL
string StrRSUIP;	//RSUIP地址
string StrRSUPort;	//RSU端口
string StrVehPlate1IP;	//识别仪1IP地址
string StrVehPlate1Port;	//识别仪1端口
string StrCAMIP;	//监控摄像头IP地址
string StrCAMPort;	//监控摄像头端口

string StrdeviceType="LTKJ-VMCTRL-101";	//设备型号
string StrVersionNo="V1.00.00" ;	//主程序版本号

string StrFlagNetRoadID="4420";	//ETC 门架路网编号
string StrFlagRoadID="52";		//ETC 门架路段编号
string StrFlagID="4";			//ETC 门架编号
string StrPosId="1";			//ETC 门架序号
string StrDirection="1";		//行车方向
string StrDirDescription="GuangZhou to ShenZhen";	//行车方向说明


string StrWIFIUSER;		//WIIFI用户名
string StrWIFIKEY;		//WIIFI密码
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

    StrID  = "" ;			//硬件ID
	StrstationID  = "" ;	//虚拟站编号
	StrstationName  = "" ;	//虚拟站名
    StrNET = "" ;			//网络方式
    StrDHCP = "";			//是否DHCP
    StrIP  = "" ;			//IP地址
    StrMask = "" ;			//子网掩码
    StrGateway = "";		//网关
    StrDNS = "";			//DNS地址
    StrSERVER = "";			//服务器地址
	StrServerURL1 = "";		//服务端URL1
	StrServerURL2 = "";		//服务端URL2
//    StrAlarmURL = "";		//告警服务器地址
	StrStationURL = "";		//虚拟站端URL
	StrdeviceType = "";		//设备型号
	StrVersionNo = "" ;		//主程序版本号
    StrWIFIUSER = "";		//WIIFI用户名
    StrWIFIKEY = "" ;		//WIIFI密码
	StrRSUIP = "" ;			//RSU IP 地址
	StrRSUPort = "" ;		//RSU端口

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

/*    StrID  = "" ;			//硬件ID
	StrstationID  = "" ;	//虚拟站编号
	StrstationName  = "" ;	//虚拟站名
    StrNET = "" ;			//网络方式
    StrDHCP = "";			//是否DHCP
    StrIP  = "" ;			//IP地址
    StrMask = "" ;			//子网掩码
    StrGateway = "";		//网关
    StrDNS = "";			//DNS地址
    StrSERVER = "";			//服务器地址
	StrServerURL1 = "";		//服务端URL1
	StrServerURL2 = "";		//服务端URL2
    StrAlarmURL = "";		//告警服务器地址
	StrStationURL = "";		//虚拟站端URL
	StrdeviceType = "";		//设备型号
	StrVersionNo = "" ;		//主程序版本号*/
    
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

    Strkey = "address=";					//RSU IP 地址
    StrRSUIP = getstring(StrConfig,Strkey) ;

    Strkey = "port=";						//RSU端口
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







