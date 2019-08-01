
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

char gsRSUIP[20];	//RSUIP地址
char gsRSUPort[10];	//RSU端口

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
	int i =0;
    char strbuf[1501];
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
	StrServerURL1 = "";		//服务端URL1
	StrServerURL2 = "";		//服务端URL2
	StrServerURL3 = "";		//服务端URL3
	StrStationURL = "";		//虚拟站端URL
	StrRSUIP = "" ;			//RSU IP 地址
	StrRSUPort = "" ;		//RSU端口
	StrVehPlate1IP = "";	//识别仪1IP地址
	StrVehPlate1Port = "";	//识别仪1端口
	StrCAMIP = "";			//监控摄像头IP地址
	StrCAMPort = "";		//监控摄像头端口

	StrCabinetType="";		//机柜类型
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
		StrAdrrLock[i] = "" ;
	}
	for (i = 0; i < VA_METER_BD_MAX_NUM; i++)
	{
		StrAdrrVAMeter[i] = "" ;
	}
	for (i = 0; i < POWER_BD_MAX_NUM; i++)
	{
		StrAdrrPower[i] = "" ;
	}
	for (i = 0; i < IO_BD_MAX_NUM; i++)
	{
		StrAdrrIO[i] = "" ;
	}


	/*StrAdrrLock1 = "" ;		//门锁1地址
	StrAdrrLock2 = "" ;		//门锁2地址
	StrAdrrLock3 = "" ;		//门锁3地址
	StrAdrrVAMeter1 = "" ;	//电压电流传感器1地址
	StrAdrrVAMeter2 = "" ;	//电压电流传感器2地址
	StrAdrrVAMeter3 = "" ;	//电压电流传感器3地址
	StrAdrrVAMeter4 = "" ;	//电压电流传感器4地址
	StrAdrrVAMeter5 = "" ;	//电压电流传感器5地址
	StrAdrrVAMeter6 = "" ;	//电压电流传感器6地址
	StrAdrrPower1 = "" ;		//电源板1地址
	StrAdrrPower2 = "" ;		//电源板2地址
	StrAdrrPower3 = "" ;		//电源板3地址
	StrAdrrIO1 = "" ;		//IO板1地址
	StrAdrrIO2 = "" ;		//IO板2地址
	StrAdrrIO3 = "" ;		//IO板3地址
	*/

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

    Strkey = "HWServer=";
    StrHWServer = getstring(StrConfig,Strkey) ;

    Strkey = "ServerURL1=";
    StrServerURL1 = getstring(StrConfig,Strkey) ;

    Strkey = "ServerURL2=";
    StrServerURL2 = getstring(StrConfig,Strkey) ;

    Strkey = "ServerURL3=";
    StrServerURL3 = getstring(StrConfig,Strkey) ;

    Strkey = "StationURL=";
    StrStationURL = getstring(StrConfig,Strkey) ;//虚拟站端URL

    Strkey = "RSUIP=";
    StrRSUIP = getstring(StrConfig,Strkey) ;//RSU IP 地址
    sprintf(gsRSUIP,StrRSUIP.c_str());//RSUIP地址

    Strkey = "RSUPort=";
    StrRSUPort = getstring(StrConfig,Strkey) ;//RSU端口
    sprintf(gsRSUPort,StrRSUPort.c_str());//RSU端口

    Strkey = "VehPlate1IP=";
    StrVehPlate1IP = getstring(StrConfig,Strkey) ;//识别仪1IP地址

    Strkey = "VehPlate1Port=";
    StrVehPlate1Port = getstring(StrConfig,Strkey) ;//识别仪1端口

    Strkey = "VehPlate1Port=";
    StrVehPlate1Port = getstring(StrConfig,Strkey) ;//识别仪1端口

    Strkey = "CAMIP=";
    StrCAMIP = getstring(StrConfig,Strkey) ;//监控摄像头IP地址

    Strkey = "CAMPort=";
    StrCAMPort = getstring(StrConfig,Strkey) ;//监控摄像头端口

    Strkey = "CabinetType=";
    StrCabinetType = getstring(StrConfig,Strkey) ;//机柜类型

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
		if (i == 0)
		{
			Strkey = "LOCKADD1=";
		}
		else if (i == 1)
		{
			Strkey = "LOCKADD2=";
		}
		else if (i == 2)
		{
			Strkey = "LOCKADD3=";
		}
		StrAdrrLock[i] = getstring(StrConfig,Strkey) ;
	}

	/*电压电流传感器的地址配置, 最大支持6个*/
	for (i = 0; i < VA_METER_BD_MAX_NUM; i++)
	{
		if (i == 0)
		{
			Strkey = "VAMETERADD1=";
		}
		else if (i == 1)
		{
			Strkey = "VAMETERADD2=";
		}
		else if (i == 2)
		{
			Strkey = "VAMETERADD3=";
		}
		else if (i == 3)
		{
			Strkey = "VAMETERADD4=";
		}
		else if (i == 4)
		{
			Strkey = "VAMETERADD5=";
		}

		else if (i == 5)
		{
			Strkey = "VAMETERADD6=";
		}
		StrAdrrVAMeter[i] = getstring(StrConfig,Strkey);
	}

	/*电源控制板的地址配置, 最大支持3块*/
	for (i = 0; i < POWER_BD_MAX_NUM; i++)
	{
		if (i == 0)
		{
			Strkey = "POWERBDADD1=";
		}
		else if (i == 1)
		{
			Strkey = "POWERBDADD2=";
		}
		else if (i == 2)
		{
			Strkey = "POWERBDADD3=";
		}
		StrAdrrPower[i] = getstring(StrConfig,Strkey);
	}

	/*IO控制板的地址配置, 最大支持3块*/
	for (i = 0; i < IO_BD_MAX_NUM; i++)
	{
		if (i == 0)
		{
			Strkey = "IOBDADD1=";
		}
		else if (i == 1)
		{
			Strkey = "IOBDADD2=";
		}
		else if (i == 2)
		{
			Strkey = "IOBDADD3=";
		}
		StrAdrrIO[i] = getstring(StrConfig,Strkey);
	}

	for (i = 0; i < SWITCH_COUNT; i++)
	{
		switch (i)
		{
		case 0:
			Strkey = "VEPLATE1=";
			break;
		case 1:
			Strkey = "VEPLATE2=";
			break;
		case 2:
			Strkey = "VEPLATE3=";
			break;
		case 3:
			Strkey = "VEPLATE4=";
			break;
		case 4:
			Strkey = "VEPLATE5=";
			break;
		case 5:
			Strkey = "VEPLATE6=";
			break;
		case 6:
			Strkey = "VEPLATE7=";
			break;
		case 7:
			Strkey = "VEPLATE8=";
			break;
		case 8:
			Strkey = "VEPLATE9=";
			break;
		case 9:
			Strkey = "VEPLATE10=";
			break;
		case 10:
			Strkey = "VEPLATE11=";
			break;
		case 11:
			Strkey = "VEPLATE12=";
			break;
		default:
			break;
		}
		StrDoSeq[i] = getstring(StrConfig,Strkey);
	}

	/*
	Strkey = "LOCKADD1=";
	StrAdrrLock1 = getstring(StrConfig,Strkey) ;//电子锁1地址配置
	Strkey = "LOCKADD2=";
	StrAdrrLock2 = getstring(StrConfig,Strkey) ;//电子锁2地址配置
	Strkey = "LOCKADD3=";
	StrAdrrLock3 = getstring(StrConfig,Strkey) ;//电子锁3地址配置

	Strkey = "VAMETERADDR1=";
	StrAdrrVAMeter1 = getstring(StrConfig,Strkey) ;//电压电流传感器1地址配置
	Strkey = "VAMETERADDR2=";
	StrAdrrVAMeter2 = getstring(StrConfig,Strkey) ;//电压电流传感器2地址配置
	Strkey = "VAMETERADDR3=";
	StrAdrrVAMeter3 = getstring(StrConfig,Strkey) ;//电压电流传感器3地址配置
	Strkey = "VAMETERADDR4=";
	StrAdrrVAMeter4 = getstring(StrConfig,Strkey) ;//电压电流传感器4地址配置
	Strkey = "VAMETERADDR5=";
	StrAdrrVAMeter5 = getstring(StrConfig,Strkey) ;//电压电流传感器5地址配置
	Strkey = "VAMETERADDR6=";
	StrAdrrVAMeter6 = getstring(StrConfig,Strkey) ;//电压电流传感器6地址配置

	Strkey = "POWERBDADD1=";
	StrAdrrPower1 = getstring(StrConfig,Strkey) ;//电源控制板1地址配置
	Strkey = "POWERBDADD2=";
	StrAdrrPower2 = getstring(StrConfig,Strkey) ;//电源控制板2地址配置
	Strkey = "POWERBDADD3=";
	StrAdrrPower3 = getstring(StrConfig,Strkey) ;//电源控制板3地址配置

	Strkey = "IOBDADD1=";
	StrAdrrIO1 = getstring(StrConfig,Strkey) ;//IO控制板1地址配置
	Strkey = "IOBDADD2=";
	StrAdrrIO2 = getstring(StrConfig,Strkey) ;//IO控制板2地址配置
	Strkey = "IOBDADD3=";
	StrAdrrIO3 = getstring(StrConfig,Strkey) ;//IO控制板3地址配置
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








