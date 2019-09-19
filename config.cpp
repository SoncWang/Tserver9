
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

string StrIP2;			//IP2地址
string StrMask2;			//子网掩码
string StrGateway2;		//网关
string StrDNS2;			//DNS地址

string StrHWServer;		//华为服务器地址
string StrHWGetPasswd;	//SNMP GET 密码
string StrHWSetPasswd;	//SNMP SET 密码
string StrServerURL1;	//服务端URL1
string StrServerURL2;	//服务端URL2
string StrServerURL3;	//服务端URL3
string StrServerURL4;	//服务端URL4
string StrStationURL;	//虚拟站端URL
string StrRSUCount;	//RSU数量
string StrRSUIP[RSUCTL_NUM];	//RSU控制器IP地址
string StrRSUPort[RSUCTL_NUM];	//RSU控制器端口
string StrVehPlateCount;	//识别仪数量
string StrVehPlateIP[VEHPLATE_NUM];	//识别仪IP地址
string StrVehPlatePort[VEHPLATE_NUM];	//识别仪端口
string StrVehPlateKey[VEHPLATE_NUM];	//识别仪用户名密码
string StrCAMCount;	//监控摄像头数量
string StrCAMIP[CAM_NUM];	//监控摄像头IP地址
string StrCAMPort[CAM_NUM];	//监控摄像头端口
string StrCAMKey[CAM_NUM];	//监控摄像头用户名密码

char gsRSUIP[RSUCTL_NUM][20];	//RSUIP地址
char gsRSUPort[RSUCTL_NUM][10];	//RSU端口

string StrdeviceType="LTKJ-VMCTRL-101";	//设备型号
string StrVersionNo="00.14.06" ;	//主程序版本号
string StrSoftDate="2019-09-17" ;	//版本日期

string StrCabinetType;		//机柜类型
string StrFlagNetRoadID;	//ETC 门架路网编号
string StrFlagRoadID;		//ETC 门架路段编号
string StrFlagID;			//ETC 门架编号
string StrPosId;			//ETC 门架序号
string StrDirection;		//行车方向
string StrDirDescription;	//行车方向说明


string StrWIFIUSER;		//WIIFI用户名
string StrWIFIKEY;		//WIIFI密码

string StrFireWareCount;	//防火墙数量
string StrFireWareIP[FIREWARE_NUM];         //防火墙IP
string StrFireWareGetPasswd[FIREWARE_NUM];  //防火墙get密码
string StrFireWareSetPasswd[FIREWARE_NUM];  //防火墙set密码
string StrIPSwitchCount;	//交换机数量
string StrIPSwitchIP[IPSWITCH_NUM] ;//交换机IP
string StrIPSwitchGetPasswd[IPSWITCH_NUM] ;//交换机get密码
string StrIPSwitchSetPasswd[IPSWITCH_NUM] ;//交换机set密码
string StrDoCount;//do数量

extern string StrDeviceNameSeq[SWITCH_COUNT];	//设备名的配置
extern string StrDoSeq[SWITCH_COUNT];	//do和设备映射的配置

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
    int i,j,vehplatecnt,rsucnt;
	char key[128],value[10],devicename[128];
	char *strbuf; 
	string strvalue;
	int isize;
	char stripbuf[1501];
    char stripbuf2[1501];

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
	StrServerURL4 = "";		//服务端URL4
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
	StrCAMCount = "" ;	//监控摄像头数量
	for(i=0;i<CAM_NUM;i++)
	{
		StrCAMIP[i] = "";			//监控摄像头IP地址
		StrCAMPort[i] = "";		//监控摄像头端口
		StrCAMKey[i] = ""; //监控摄像头用户名密码
	}

	StrCabinetType="";		//机柜类型

	StrFireWareCount = "" ;	//防火墙数量
	for(i=0;i<FIREWARE_NUM;i++)
	{
		StrFireWareIP[i]="";		  //防火墙IP
		StrFireWareGetPasswd[i]="";  //防火墙get密码
		StrFireWareSetPasswd[i]="";  //防火墙set密码
	}
	StrIPSwitchCount = "" ;	//交换机数量
	for(i=0;i<IPSWITCH_NUM;i++)
	{
		StrIPSwitchIP[i] ="";//交换机IP
		StrIPSwitchGetPasswd[i] ="";//交换机get密码
		StrIPSwitchSetPasswd[i] ="";//交换机set密码
	}

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
	StrDoCount="";						//DO 数量
	
    ConfigCri.Lock();
    //read config
    FILE* fdd ;
    if((fdd=fopen("/opt/config", "rb"))==NULL)
    {
        printf("read config erro\r\n");
        ConfigCri.UnLock();
        return 1;
    }
	fseek(fdd,0,SEEK_END);
	isize = ftell(fdd);
	strbuf=(char*)malloc(isize);
	fseek(fdd,0,SEEK_SET);
    fread(strbuf,1,isize,fdd);
    fclose(fdd);


    FILE* ipfd ;
    if((ipfd=fopen("/home/root/net/netconfig", "rb"))==NULL)
    {
        printf("read config erro\r\n");
        //ConfigCri.UnLock();
        //return 1;
    }
    else
    {
        fread(stripbuf,1,1500,ipfd);
        fclose(ipfd);
    }


    FILE* ipfd2 ;
    if((ipfd2=fopen("/home/root/net/netconfig2", "rb"))==NULL)
    {
        printf("read config erro\r\n");
        //ConfigCri.UnLock();
       // return 1;
    }
    else
    {
       fread(stripbuf2,1,1500,ipfd2);
       fclose(ipfd2);
    }

//    StrVersionNo = VERSIONNO ;
    printf("Version:%s\n",StrVersionNo.c_str()) ;

    printf("-----config----\n%s\n----end config----\n",strbuf) ;
    STRCONFIG = strbuf ;

    printf("-----netconfig----\n%s\n----end netconfig----\n",stripbuf) ;
    STRWIFIWAP = strwifibuf ;
    
    printf("-----netconfig2----\n%s\n----end netconfig2----\n",stripbuf2) ;
  


    string Strkey ;

//  netconfig 读取
    string StrIpConfig = stripbuf;
    Strkey = "IP=";
    StrIP = getstring(StrIpConfig,Strkey) ;

    Strkey = "Mask=";
    StrMask = getstring(StrIpConfig,Strkey) ;

    Strkey = "Gateway=";
    StrGateway = getstring(StrIpConfig,Strkey) ;

    Strkey = "DNS=";
    StrDNS = getstring(StrIpConfig,Strkey) ;

 //end    netconfig 读取


//  netconfig2 读取
	string StrIpConfig2 = stripbuf2;
	Strkey = "IP=";
	StrIP2 = getstring(StrIpConfig2,Strkey) ;

	Strkey = "Mask=";
	StrMask2 = getstring(StrIpConfig2,Strkey) ;

	Strkey = "Gateway=";
	StrGateway2 = getstring(StrIpConfig2,Strkey) ;

	Strkey = "DNS=";
	StrDNS2 = getstring(StrIpConfig2,Strkey) ;

 //end    netconfig2 读取


    string StrConfig = STRCONFIG ;
    Strkey = "ID=";
    StrID = getstring(StrConfig,Strkey) ;
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

    Strkey = "ServerURL4=";
    StrServerURL4 = getstring(StrConfig,Strkey);

    Strkey = "StationURL=";
    StrStationURL = getstring(StrConfig,Strkey) ;//虚拟站端URL

    Strkey = "RSUCount=";
    StrRSUCount = getstring(StrConfig,Strkey) ;//RSU数量
    if(StrRSUCount=="")
		StrRSUCount="0";
	rsucnt=atoi(StrRSUCount.c_str());
	if(rsucnt>RSUCTL_NUM)
	{
		sprintf(value,"%d", RSUCTL_NUM) ;
		StrRSUCount=value;
		rsucnt=RSUCTL_NUM;
	}
	else if(rsucnt<0)
	{
		StrRSUCount="0";
		rsucnt=0;
	}

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
    if(StrVehPlateCount=="")
		StrVehPlateCount="0";
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	if(vehplatecnt>VEHPLATE_NUM)
	{
		sprintf(value,"%d", VEHPLATE_NUM) ;
		StrVehPlateCount=value;
		vehplatecnt=VEHPLATE_NUM;
	}
	else if(vehplatecnt<0)
	{
		StrVehPlateCount="0";
		vehplatecnt=0;
	}
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(key,"VehPlate%dIP=",i+1);
	    StrVehPlateIP[i] = getstring(StrConfig,key) ;//识别仪IP地址
	    
		sprintf(key,"VehPlate%dPort=",i+1);
	    StrVehPlatePort[i] = getstring(StrConfig,key) ;//识别仪端口
	    
		sprintf(key,"VehPlate%dKey=",i+1);
	    StrVehPlateKey[i] = getstring(StrConfig,key) ;//识别仪用户名密码
	}
    
    Strkey = "CAMCount=";
    StrCAMCount = getstring(StrConfig,Strkey) ;//监控摄像头数量
    if(StrCAMCount=="")
		StrCAMCount="0";
	if(atoi(StrCAMCount.c_str())>CAM_NUM)
	{
		sprintf(value,"%d", CAM_NUM) ;
		StrCAMCount=value;
	}
	else if(atoi(StrCAMCount.c_str())<0)
		StrCAMCount="0";
	for(i=0;i<CAM_NUM;i++)
	{
		sprintf(key,"CAM%dIP=",i+1);
	    StrCAMIP[i] = getstring(StrConfig,key) ;//监控摄像头IP地址
	    
		sprintf(key,"CAM%dPort=",i+1);
	    StrCAMPort[i] = getstring(StrConfig,key) ;//监控摄像头端口
	    
		sprintf(key,"CAM%dKey=",i+1);
	    StrCAMKey[i] = getstring(StrConfig,key) ;//监控摄像头用户名密码
	}
    
    Strkey = "CabinetType=";
    StrCabinetType = getstring(StrConfig,Strkey) ;//机柜类型

    //防火墙配置
    Strkey = "FireWareCount=";
    StrFireWareCount = getstring(StrConfig,Strkey) ;//防火墙数量
    if(StrFireWareCount=="")
		StrFireWareCount="0";
	if(atoi(StrFireWareCount.c_str())>FIREWARE_NUM)
	{
		sprintf(value,"%d", FIREWARE_NUM) ;
		StrFireWareCount=value;
	}
	else if(atoi(StrFireWareCount.c_str())<0)
		StrFireWareCount="0";
	for(i=0;i<FIREWARE_NUM;i++)
	{
		sprintf(key,"FireWare%dIP=",i+1);
	    StrFireWareIP[i] = getstring(StrConfig,key) ;//防火墙IP地址

		sprintf(key,"FireWare%dGetPasswd=",i+1);
	    StrFireWareGetPasswd[i] = getstring(StrConfig,key) ;//防火墙get密码

		sprintf(key,"FireWare%dSetPasswd=",i+1);
	    StrFireWareSetPasswd[i] = getstring(StrConfig,key) ;//防火墙set密码
	}
    //交换机配置
    Strkey = "SwitchCount=";
    StrIPSwitchCount = getstring(StrConfig,Strkey) ;//防火墙数量
    if(StrIPSwitchCount=="")
		StrIPSwitchCount="0";
	if(atoi(StrIPSwitchCount.c_str())>IPSWITCH_NUM)
	{
		sprintf(value,"%d", IPSWITCH_NUM) ;
		StrIPSwitchCount=value;
	}
	else if(atoi(StrIPSwitchCount.c_str())<0)
		StrIPSwitchCount="0";
	for(i=0;i<IPSWITCH_NUM;i++)
	{
		sprintf(key,"Switch%dIP=",i+1);
	    StrIPSwitchIP[i] = getstring(StrConfig,key) ;//交换机IP地址

		sprintf(key,"Switch%dGetPasswd=",i+1);
	    StrIPSwitchGetPasswd[i] = getstring(StrConfig,key) ;//交换机get密码

		sprintf(key,"Switch%dSetPasswd=",i+1);
	    StrIPSwitchSetPasswd[i] = getstring(StrConfig,key) ;//交换机set密码
	}

    
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

    Strkey = "DO_Count=";
    StrDoCount = getstring(StrConfig,Strkey) ;//DO数量
	if(atoi(StrDoCount.c_str())>SWITCH_COUNT)
	{
		sprintf(value,"%d", SWITCH_COUNT) ;
		StrDoCount=value;
	}
	
	//DO映射设备，最大支持36路DO
	//车牌识别映射DO
	j=0;
	//RSU映射DO
	for (i = 0; i < RSUCTL_NUM; i++)
	{
		sprintf(key,"RSU%d_DO=",i+1);
		sprintf(devicename,"rsu%d_do",i+1);
		strvalue=getstring(StrConfig,key);
//			printf("config key=%s,strvalue=%s\n",key,strvalue.c_str());
		if(strvalue!="")
		{
			
			StrDeviceNameSeq[j]=devicename; //设备名
			StrDoSeq[j] = strvalue; 	//对应DO
			j++;
		}
	}
	for (i = 0; i < VEHPLATE_NUM; i++)
	{
		sprintf(key,"VEHPLATE%d_DO=",i+1);
		sprintf(devicename,"vehplate%d_do",i+1);
		strvalue=getstring(StrConfig,key);
//			printf("config key=%s,strvalue=%s\n",key,strvalue.c_str());
		if(strvalue!="")
		{
			
			StrDeviceNameSeq[j]=devicename; //设备名
			StrDoSeq[j] = strvalue; 	//对应DO
			j++;
		}
	}
	for (i = 0; i < CAM_NUM; i++)
	{
		sprintf(key,"CAM%d_DO=",i+1);
		sprintf(devicename,"cam%d_do",i+1);
		strvalue=getstring(StrConfig,key);
//			printf("config key=%s,strvalue=%s\n",key,strvalue.c_str());
		if(strvalue!="")
		{
			
			StrDeviceNameSeq[j]=devicename; //设备名
			StrDoSeq[j] = strvalue; 	//对应DO
			j++;
		}
	}
	//交换机映射DO
	for (i = 0; i < IPSWITCH_NUM; i++)
	{
		sprintf(key,"IPSWITCH%d_DO=",i+1);
		sprintf(devicename,"ipswitch%d_do",i+1);
		strvalue=getstring(StrConfig,key);
//			printf("config key=%s,strvalue=%s\n",key,strvalue.c_str());
		if(strvalue!="")
		{
			
			StrDeviceNameSeq[j]=devicename; //设备名
			StrDoSeq[j] = strvalue; 	//对应DO
			j++;
		}
	}
	//防火墙映射DO
	for (i = 0; i < FIREWARE_NUM; i++)
	{
		sprintf(key,"FIREWARE%d_DO=",i+1);
		sprintf(devicename,"fireware%d_do",i+1);
		strvalue=getstring(StrConfig,key);
//			printf("config key=%s,strvalue=%s\n",key,strvalue.c_str());
		if(strvalue!="")
		{
			
			StrDeviceNameSeq[j]=devicename; //设备名
			StrDoSeq[j] = strvalue; 	//对应DO
			j++;
		}
	}
	//ATLAS映射DO
	for (i = 0; i < ATLAS_NUM; i++)
	{
		sprintf(key,"ATLAS%d_DO=",i+1);
		sprintf(devicename,"atlas%d_do",i+1);
		strvalue=getstring(StrConfig,key);
//			printf("config key=%s,strvalue=%s\n",key,strvalue.c_str());
		if(strvalue!="")
		{
			
			StrDeviceNameSeq[j]=devicename; //设备名
			StrDoSeq[j] = strvalue; 	//对应DO
			j++;
		}
	}
	//天线头映射DO
	for (i = 0; i < ANTENNA_NUM; i++)
	{
		sprintf(key,"ANTENNA%d_DO=",i+1);
		sprintf(devicename,"antenna%d_do",i+1);
		strvalue=getstring(StrConfig,key);
//			printf("config key=%s,strvalue=%s\n",key,strvalue.c_str());
		if(strvalue!="")
		{
			
			StrDeviceNameSeq[j]=devicename; //设备名
			StrDoSeq[j] = strvalue; 	//对应DO
			j++;
		}
	}
	for (i = j; i < SWITCH_COUNT; i++)		//默认DO
	{
		sprintf(key,"do%d_do",i+1);
		StrDeviceNameSeq[i]=key; //设备名
		sprintf(key,"%d",i+1);
		StrDoSeq[i] = key; 	//对应DO
	}
/*for (i = 0; i < SWITCH_COUNT; i++)
{
printf("config StrDeviceNameSeq[%d]=%s,StrDoSeq[%d]=%s\n",i,StrDeviceNameSeq[i].c_str(),i,StrDoSeq[i].c_str());
}
printf("j=%d\n",j);*/
	ConfigCri.UnLock();
	free(strbuf);
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


//单独写网络配置文件
int WriteNetconfig(char *configbuf,int configlen)
{
    int ret = 0;
    ConfigCri.Lock();
    FILE* fdd ;
    fdd	= fopen("/opt/netconfig", "wb");
    if(fdd == NULL)
    {
       ConfigCri.UnLock();
       return 1 ;
    }

    ret = fwrite(configbuf,configlen, 1, fdd);
    fflush(fdd);
    fclose(fdd);
    ConfigCri.UnLock();

    if(ret > 0)
    {
        //覆盖/home/root/net/netconfig
        system("wr cp /opt/netconfig /home/root/net/");
    }


    return 0 ;

}


int WriteNetconfig2(char *configbuf,int configlen)
{
    int ret = 0;
    ConfigCri.Lock();
    FILE* fdd ;
    fdd	= fopen("/opt/netconfig2", "wb");
    if(fdd == NULL)
    {
       ConfigCri.UnLock();
       return 1 ;
    }

    ret = fwrite(configbuf,configlen, 1, fdd);
    fflush(fdd);
    fclose(fdd);
    ConfigCri.UnLock();

    if(ret > 0)
    {
        //覆盖/home/root/net/netconfig2
        system("wr cp /opt/netconfig2 /home/root/net/");
    }


    return 0 ;

}







