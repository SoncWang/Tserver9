
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
#include "config.h"
#include "rs485server.h"


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

string StrHWServer;		//华为服务器地址
string StrHWGetPasswd;	//SNMP GET 密码
string StrHWSetPasswd;	//SNMP SET 密码
string StrServerURL1;	//服务端URL1
string StrServerURL2;	//服务端URL2
string StrServerURL3;	//服务端URL3
string StrStationURL;	//虚拟站端URL
string StrRSUCount;	//RSU数量
string StrRSUIP[RSUCTL_NUM];	//RSU控制器IP地址
string StrRSUPort[RSUCTL_NUM];	//RSU控制器端口
string StrVehPlateCount;	//识别仪数量
string StrVehPlateIP[VEHPLATE_NUM];	//识别仪IP地址
string StrVehPlatePort[VEHPLATE_NUM];	//识别仪端口
string StrVehPlateKey[VEHPLATE_NUM];	//识别仪用户名密码
string StrCAMIP;	//监控摄像头IP地址
string StrCAMPort;	//监控摄像头端口
string StrCAMKey;	//监控摄像头用户名密码

char gsRSUIP[RSUCTL_NUM][20];	//RSUIP地址
char gsRSUPort[RSUCTL_NUM][10];	//RSU端口


string StrdeviceType="LTKJ-VMCTRL-101";	//设备型号
string StrVersionNo="V1.00.00" ;	//主程序版本号
string StrSoftDate="2019-07-01" ;	//版本日期

string StrCabinetType;		//机柜类型
string StrFlagNetRoadID;	//ETC 门架路网编号
string StrFlagRoadID;		//ETC 门架路段编号
string StrFlagID;			//ETC 门架编号
string StrPosId;			//ETC 门架序号
string StrDirection;		//行车方向
string StrDirDescription;	//行车方向说明


string StrWIFIUSER;		//WIIFI用户名
string StrWIFIKEY;		//WIIFI密码

string StrFireWareIP;         //防火墙IP
string StrFireWareGetPasswd;  //防火墙get密码
string StrFireWareSetPasswd;  //防火墙set密码
string StrSwitchIP ;//交换机IP
string StrSwitchGetPasswd ;//交换机get密码
string StrSwitchSetPasswd ;//交换机set密码

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
    int i,vehplatecnt,rsucnt;
	char key[128];
	char strbuf[1501];
	string strvalue;
    memset(strbuf,0x00,1501) ;

    char strwifibuf[1001];
    memset(strwifibuf,0x00,1001) ;

    StrID  = "" ;			//硬件ID
	StrstationID  = "" ;	//虚拟站编号
	StrstationName  = "" ;	//虚拟站名

	//参数设置
    StrNET = "" ;			//网络方式
    StrDHCP = "";			//是否DHCP
    StrIP  = "" ;			//IP地址
    StrMask = "" ;			//子网掩码
    StrGateway = "";		//网关
    StrDNS = "";			//DNS地址
    StrHWServer = "";		//华为服务器地址
	StrHWGetPasswd = "";	//SNMP GET 密码
	StrHWSetPasswd = "";	//SNMP SET 密码
	StrServerURL1 = "";		//服务端URL1
	StrServerURL2 = "";		//服务端URL2
	StrServerURL3 = "";		//服务端URL3
	StrStationURL = "";		//虚拟站端URL
	StrRSUCount = ""; 		//RSU数量
	for(i=0;i<RSUCTL_NUM;i++)
	{
		StrRSUIP[i] = "" ;			//RSU IP 地址
		StrRSUPort[i] = "" ;		//RSU端口
	}
	StrVehPlateCount = "" ;	//识别仪数量
	for(i=0;i<VEHPLATE_NUM;i++)
	{
		StrVehPlateIP[i] = "";	//识别仪1IP地址
		StrVehPlatePort[i] = "";	//识别仪1端口
		StrVehPlateKey[i] = "";	//识别仪用户名密码
	}
	StrCAMIP = "";			//监控摄像头IP地址
	StrCAMPort = "";		//监控摄像头端口
	StrCAMKey = "";	//监控摄像头用户名密码

	StrCabinetType="";		//机柜类型

	StrFireWareIP="";		  //防火墙IP
	StrFireWareGetPasswd="";  //防火墙get密码
	StrFireWareSetPasswd="";  //防火墙set密码
	StrSwitchIP ="";//交换机IP
	StrSwitchGetPasswd ="";//交换机get密码
	StrSwitchSetPasswd ="";//交换机set密码

	//门架信息
	StrFlagNetRoadID = "";	//ETC 门架路网编号
	StrFlagRoadID = "";		//ETC 门架路段编号
	StrFlagID = "";			//ETC 门架编号
	StrPosId = "";			//ETC 门架序号
	StrDirection = "";		//行车方向
	StrDirDescription = "";	//行车方向说明

//	StrdeviceType = "";		//设备型号
//	StrVersionNo = "" ;		//主程序版本号
    StrWIFIUSER = "";		//WIIFI用户名
    StrWIFIKEY = "" ;		//WIIFI密码

	for (i = 0; i < LOCK_MAX_NUM; i++)
	{
		StrAdrrLock[i] = "" ;			//门锁地址
	}
	for (i = 0; i < VA_METER_BD_MAX_NUM; i++)
	{
		StrAdrrVAMeter[i] = "" ;		//电压电流传感器地址
	}
	for (i = 0; i < POWER_BD_MAX_NUM; i++)
	{
		StrAdrrPower[i] = "" ;			//电源板的地址
	}

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


//    StrVersionNo = VERSIONNO ;
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

    Strkey = "HWServer=";
    StrHWServer = getstring(StrConfig,Strkey) ;

    Strkey = "HWGetPasswd=";
    StrHWGetPasswd = getstring(StrConfig,Strkey) ;//SNMP GET 密码

    Strkey = "HWSetPasswd=";
    StrHWSetPasswd = getstring(StrConfig,Strkey) ;//SNMP SET 密码

    Strkey = "ServerURL1=";
    StrServerURL1 = getstring(StrConfig,Strkey) ;

    Strkey = "ServerURL2=";
    StrServerURL2 = getstring(StrConfig,Strkey) ;

    Strkey = "ServerURL3=";
    StrServerURL3 = getstring(StrConfig,Strkey) ;

    Strkey = "StationURL=";
    StrStationURL = getstring(StrConfig,Strkey) ;//虚拟站端URL

    Strkey = "RSUCount=";
    StrRSUCount = getstring(StrConfig,Strkey) ;//RSU数量
    rsucnt=atoi(StrRSUCount.c_str());

	for(i=0;i<rsucnt;i++)
	{
		sprintf(key,"RSU%dIP=",i+1);
	    StrRSUIP[i] = getstring(StrConfig,key) ;//RSU IP 地址
	    sprintf(gsRSUIP[i],StrRSUIP[i].c_str());//RSUIP地址

		sprintf(key,"RSU%dPort=",i+1);
	    StrRSUPort[i] = getstring(StrConfig,key) ;//RSU端口
	    sprintf(gsRSUPort[i],StrRSUPort[i].c_str());//RSU端口

	}

    Strkey = "VehPlateCount=";
    StrVehPlateCount = getstring(StrConfig,Strkey) ;//识别仪数量
	vehplatecnt=atoi(StrVehPlateCount.c_str());

	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(key,"VehPlate%dIP=",i+1);
	    StrVehPlateIP[i] = getstring(StrConfig,key) ;//识别仪IP地址

		sprintf(key,"VehPlate%dPort=",i+1);
	    StrVehPlatePort[i] = getstring(StrConfig,key) ;//识别仪端口

		sprintf(key,"VehPlate%dKey=",i+1);
	    StrVehPlateKey[i] = getstring(StrConfig,key) ;//识别仪用户名密码
	}

    Strkey = "CAMIP=";
    StrCAMIP = getstring(StrConfig,Strkey) ;//监控摄像头IP地址

    Strkey = "CAMPort=";
    StrCAMPort = getstring(StrConfig,Strkey) ;//监控摄像头端口

    Strkey = "StrCAMKey=";
    StrCAMKey = getstring(StrConfig,Strkey) ;//监控摄像头用户名密码

    Strkey = "CabinetType=";
    StrCabinetType = getstring(StrConfig,Strkey) ;//机柜类型

    //防火墙配置
    Strkey = "FireWareIP=";
    StrFireWareIP = getstring(StrConfig,Strkey) ;//防火墙IP

    Strkey = "FireWareGetPasswd=";
    StrFireWareGetPasswd = getstring(StrConfig,Strkey) ;//防火墙get密码

    Strkey = "FireWareSetPasswd=";
    StrFireWareSetPasswd = getstring(StrConfig,Strkey) ;//防火墙set密码

    //交换机配置
    Strkey = "SwitchIP=";
    StrSwitchIP = getstring(StrConfig,Strkey) ;//交换机IP

    Strkey = "SwitchGetPasswd=";
    StrSwitchGetPasswd = getstring(StrConfig,Strkey) ;//交换机get密码

    Strkey = "SwitchSetPasswd=";
    StrSwitchSetPasswd = getstring(StrConfig,Strkey) ;//交换机set密码


	//门架信息
    Strkey = "FlagNetRoadID=";
    StrFlagNetRoadID = getstring(StrConfig,Strkey) ;//ETC 门架路网编号

    Strkey = "FlagRoadID=";
    StrFlagRoadID = getstring(StrConfig,Strkey) ;//ETC 门架路段编号

    Strkey = "FlagID=";
    StrFlagID = getstring(StrConfig,Strkey) ;//ETC 门架编号

    Strkey = "PosId=";
    StrPosId = getstring(StrConfig,Strkey) ;//ETC 门架序号

    Strkey = "Direction=";
    StrDirection = getstring(StrConfig,Strkey) ;//行车方向

    Strkey = "DirDescription=";
    StrDirDescription = getstring(StrConfig,Strkey) ;//行车方向说明

/*
    Strkey = "WIFIUSER=";
    StrWIFIUSER = getstring(StrConfig,Strkey) ;

    Strkey = "WIFIKEY=";
    StrWIFIKEY = getstring(StrConfig,Strkey) ;
*/

	/*电子锁的地址配置, 最大支持3把*/
	for (i = 0; i < LOCK_MAX_NUM; i++)
	{
		sprintf(key,"LOCKADD%d=",i+1);
		StrAdrrLock[i] = getstring(StrConfig,key) ;
	}

	/*电压电流传感器的地址配置, 最大支持6个*/
	for (i = 0; i < VA_METER_BD_MAX_NUM; i++)
	{
		sprintf(key,"VAMETERADD%d=",i+1);
		StrAdrrVAMeter[i] = getstring(StrConfig,key);
	}

	/*电源控制板的地址配置, 最大支持3块*/
	for (i = 0; i < POWER_BD_MAX_NUM; i++)
	{
		sprintf(key,"POWERBDADD%d=",i+1);
		StrAdrrPower[i] = getstring(StrConfig,key);
	}

	for (i = 0; i < VEHPLATE_NUM; i++)
	{
		sprintf(key,"VEHPLATE%d_DO=",i+1);
		strvalue=getstring(StrConfig,key);
		if(strvalue!="")
		{
			StrDoSeq[i] = strvalue;
		}
	}

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








