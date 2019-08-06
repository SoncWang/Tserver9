
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
    int i;
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
	}
	StrCAMIP = "";			//监控摄像头IP地址
	StrCAMPort = "";		//监控摄像头端口

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

    Strkey = "RSUIP=";
    StrRSUIP[0] = getstring(StrConfig,Strkey) ;//RSU IP 地址
    sprintf(gsRSUIP,StrRSUIP[0].c_str());//RSUIP地址
    
    Strkey = "RSUPort=";
    StrRSUPort[0] = getstring(StrConfig,Strkey) ;//RSU端口
    sprintf(gsRSUPort,StrRSUPort[0].c_str());//RSU端口
    
    Strkey = "VehPlateCount=";
    StrVehPlateCount = getstring(StrConfig,Strkey) ;//识别仪数量

    Strkey = "VehPlate1IP=";
    StrVehPlateIP[0] = getstring(StrConfig,Strkey) ;//识别仪1IP地址
    Strkey = "VehPlate1Port=";
    StrVehPlatePort[0] = getstring(StrConfig,Strkey) ;//识别仪1端口
    
    Strkey = "VehPlate2IP=";
    StrVehPlateIP[1] = getstring(StrConfig,Strkey) ;//识别仪2IP地址
    Strkey = "VehPlate2Port=";
    StrVehPlatePort[1] = getstring(StrConfig,Strkey) ;//识别仪2端口
    
    Strkey = "VehPlate3IP=";
    StrVehPlateIP[2] = getstring(StrConfig,Strkey) ;//识别仪3IP地址
    Strkey = "VehPlate3Port=";
    StrVehPlatePort[2] = getstring(StrConfig,Strkey) ;//识别仪3端口
    
    Strkey = "VehPlate4IP=";
    StrVehPlateIP[3] = getstring(StrConfig,Strkey) ;//识别仪4IP地址
    Strkey = "VehPlate4Port=";
    StrVehPlatePort[3] = getstring(StrConfig,Strkey) ;//识别仪4端口
    
    Strkey = "CAMIP=";
    StrCAMIP = getstring(StrConfig,Strkey) ;//监控摄像头IP地址
    
    Strkey = "CAMPort=";
    StrCAMPort = getstring(StrConfig,Strkey) ;//监控摄像头端口
    
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

	for (i = 0; i < SWITCH_COUNT; i++)
	{
		switch (i)
		{
		case 0:
			Strkey = "VEHPLATE1=";
			break;
		case 1:
			Strkey = "VEHPLATE2=";
			break;
		case 2:
			Strkey = "VEHPLATE3=";
			break;
		case 3:
			Strkey = "VEHPLATE4=";
			break;
		case 4:
			Strkey = "VEHPLATE5=";
			break;
		case 5:
			Strkey = "VEHPLATE6=";
			break;
		case 6:
			Strkey = "VEHPLATE7=";
			break;
		case 7:
			Strkey = "VEHPLATE8=";
			break;
		case 8:
			Strkey = "VEHPLATE9=";
			break;
		case 9:
			Strkey = "VEHPLATE10=";
			break;
		case 10:
			Strkey = "VEHPLATE11=";
			break;
		case 11:
			Strkey = "VEHPLATE12=";
			break;
		default:
			break;
		}
		StrDoSeq[i] = getstring(StrConfig,Strkey);
	}
	
/*	printf("%d %s\n",StrID.length(), StrID.c_str());			//硬件ID
	printf("%d %s\n",StrstationID.length(), StrstationID.c_str());	//虚拟站编号
	printf("%d %s\n",StrstationName.length(), StrstationName.c_str());	//虚拟站名
	printf("%d %s\n",StrNET.length(), StrNET.c_str());			//网络方式
	printf("%d %s\n",StrDHCP.length(), StrDHCP.c_str()); 		//是否DHCP
	printf("%d %s\n",StrIP.length(), StrIP.c_str());			//IP地址
	printf("%d %s\n",StrMask.length(), StrMask.c_str()); 		//子网掩码
	printf("%d %s\n",StrGateway.length(), StrGateway.c_str());		//网关
	printf("%d %s\n",StrDNS.length(), StrDNS.c_str());			//DNS地址
	
	printf("%d %s\n",StrHWServer.length(), StrHWServer.c_str()); 	//华为服务器地址
	printf("%d %s\n",StrHWGetPasswd.length(), StrHWGetPasswd.c_str());	//SNMP GET 密码
	printf("%d %s\n",StrHWSetPasswd.length(), StrHWSetPasswd.c_str());	//SNMP SET 密码
	printf("%d %s\n",StrServerURL1.length(), StrServerURL1.c_str());	//服务端URL1
	printf("%d %s\n",StrServerURL2.length(), StrServerURL2.c_str());	//服务端URL2
	printf("%d %s\n",StrServerURL3.length(), StrServerURL3.c_str());	//服务端URL3
	printf("%d %s\n",StrStationURL.length(), StrStationURL.c_str());	//虚拟站端URL
	printf("%d %s\n",StrRSUIP.length(), StrRSUIP.c_str());	//RSUIP地址
	printf("%d %s\n",StrRSUPort.length(), StrRSUPort.c_str());	//RSU端口
	printf("%d %s\n",StrVehPlateIP[0].length(), StrVehPlateIP[0].c_str());	//识别仪1IP地址
	printf("%d %s\n",StrVehPlatePort[0].length(), StrVehPlatePort[0].c_str());	//识别仪1端口
	printf("%d %s\n",StrVehPlateIP[1].length(), StrVehPlateIP[1].c_str());	//识别仪2IP地址
	printf("%d %s\n",StrVehPlatePort[1].length(), StrVehPlatePort[1].c_str());	//识别仪2端口
	printf("%d %s\n",StrVehPlateIP[2].length(), StrVehPlateIP[2].c_str());	//识别仪3IP地址
	printf("%d %s\n",StrVehPlatePort[2].length(), StrVehPlatePort[2].c_str());	//识别仪3端口
	printf("%d %s\n",StrVehPlateIP[3].length(), StrVehPlateIP[3].c_str());	//识别仪4IP地址
	printf("%d %s\n",StrVehPlatePort[3].length(), StrVehPlatePort[3].c_str());	//识别仪4端口
	printf("%d %s\n",StrCAMIP.length(), StrCAMIP.c_str());	//监控摄像头IP地址
	printf("%d %s\n",StrCAMPort.length(), StrCAMPort.c_str());	//监控摄像头端口
	
	printf("%d %s\n",StrdeviceType.length(), StrdeviceType.c_str()); //设备型号
	printf("%d %s\n",StrID.length(), StrID.c_str()); //设备ID号
	printf("%d %s\n",StrVersionNo.length(), StrVersionNo.c_str());	//主程序版本号
	printf("%d %s\n",StrSoftDate.length(), StrSoftDate.c_str());	//版本日期
	
	printf("%d %s\n",StrCabinetType.length(), StrCabinetType.c_str());		//机柜类型
	printf("%d %s\n",StrFlagNetRoadID.length(), StrFlagNetRoadID.c_str());	//ETC 门架路网编号
	printf("%d %s\n",StrFlagRoadID.length(), StrFlagRoadID.c_str());		//ETC 门架路段编号
	printf("%d %s\n",StrFlagID.length(), StrFlagID.c_str());			//ETC 门架编号
	printf("%d %s\n",StrPosId.length(), StrPosId.c_str());			//ETC 门架序号
	printf("%d %s\n",StrDirection.length(), StrDirection.c_str());		//行车方向
	printf("%d %s\n",StrDirDescription.length(), StrDirDescription.c_str());	//行车方向说明
	
	
	printf("%d %s\n",StrWIFIUSER.length(), StrWIFIUSER.c_str()); 	//WIIFI用户名
	printf("%d %s\n",StrWIFIKEY.length(), StrWIFIKEY.c_str());		//WIIFI密码
	
	printf("%d %s\n",StrFireWareIP.length(), StrFireWareIP.c_str());		  //防火墙IP
	printf("%d %s\n",StrFireWareGetPasswd.length(), StrFireWareGetPasswd.c_str());  //防火墙get密码
	printf("%d %s\n",StrFireWareSetPasswd.length(), StrFireWareSetPasswd.c_str());  //防火墙set密码
	printf("%d %s\n",StrSwitchIP.length(), StrSwitchIP .c_str());//交换机IP
	printf("%d %s\n",StrSwitchGetPasswd.length(), StrSwitchGetPasswd .c_str());//交换机get密码
	printf("%d %s\n",StrSwitchSetPasswd.length(), StrSwitchSetPasswd .c_str());//交换机set密码*/

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








