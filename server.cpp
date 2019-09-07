#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
//#include "comport.h"
#include "server.h"
#include <unistd.h> // fork, close
#include <arpa/inet.h>  // inet_ntoa
#include <pthread.h>
#include <netinet/tcp.h>
#include <linux/rtc.h>
#include "comserver.h"
#include "config.h"
#include <string>

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
#include "rs485server.h"

using namespace std;//寮??ユ?翠釜??绌洪??

#define PORT 		5000       		// The port which is communicate with server
#define BACKLOG 	10

int fd_A[BACKLOG];    // accepted connection fd
int conn_amount;      // current connection amount
int data_conamount;

void* NetWork_server_thread(void *param);
void Client_CmdProcess(int fd, char *cmdbuffer,void *arg);
int NetSend(int s,char *pbuffer,int nsize);
void AirCondControl(UINT8* pRCtrl);
void RemoteControl(UINT8* pRCtrl);
extern void WriteLog(char* str);
extern void myprintf(char* str);
int RebootIpCam(int Ipcamindex);

extern ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
extern UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体
extern DEVICE_PARAMS *stuDev_Param;		//装置参数寄存器
extern VA_METER_PARAMS stuVA_Meter_Param[VA_METER_BD_NUM];		//伏安表电压电流结构体
extern REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体
extern VMCONTROL_PARAM *stuVMCtl_Param;	//采集器设备信息结构体
extern AIRCOND_PARAM *stuAirCondRead;		//读空调状态结构体
extern AIRCOND_PARAM *stuAirCondWrite;		//写空调状态结构体
extern THUAWEIGantry HUAWEIDevValue;		//华为机柜状态

extern string StrID;			//硬件ID
extern string StrdeviceType;	//设备型号
extern string StrVersionNo;	//主程序版本号
extern string StrSoftDate;	//版本日期

extern string StrIP;			//IP地址
extern string StrMask;			//子网掩码
extern string StrGateway;		//网关
extern string StrDNS;			//DNS地址

extern string StrHWServer;		//华为服务器地址
extern string StrHWGetPasswd;	//SNMP GET 密码
extern string StrHWSetPasswd;	//SNMP SET 密码
extern string StrServerURL1;	//服务端URL1
extern string StrServerURL2;	//服务端URL2
extern string StrServerURL3;	//服务端URL3
extern string StrStationURL;	//虚拟站端URL
extern string StrRSUCount;	//RSU数量
extern string StrRSUIP[RSUCTL_NUM];	//RSUIP地址
extern string StrRSUPort[RSUCTL_NUM];	//RSU端口
extern string StrVehPlateCount;	//识别仪数量
extern string StrVehPlateIP[VEHPLATE_NUM];	//识别仪IP地址
extern string StrVehPlatePort[VEHPLATE_NUM];	//识别仪端口
extern string StrVehPlateKey[VEHPLATE_NUM];	//识别仪用户名密码
extern string StrCAMIP;	//监控摄像头IP地址
extern string StrCAMPort;	//监控摄像头端口
extern string StrCAMKey;	//监控摄像头用户名密码
extern string StrFireWareIP;         //防火墙IP
extern string StrFireWareGetPasswd;  //防火墙get密码
extern string StrFireWareSetPasswd;  //防火墙set密码
extern string StrSwitchIP ;//交换机IP
extern string StrSwitchGetPasswd ;//交换机get密码
extern string StrSwitchSetPasswd ;//交换机set密码
extern string StrDeviceNameSeq[SWITCH_COUNT];	//设备名的配置

extern string StrCabinetType;		//机柜类型
extern string StrFlagNetRoadID;	//ETC 门架路网编号
extern string StrFlagRoadID;		//ETC 门架路段编号
extern string StrFlagID;			//ETC 门架编号
extern string StrPosId;			//ETC 门架序号
extern string StrDirection;		//行车方向
extern string StrDirDescription;	//行车方向说明

extern int Writeconfig(void);
extern int Setconfig(string StrKEY,string StrSetconfig);
extern bool jsonstrRCtrlReader(char* jsonstr, int len, UINT8 *pstuRCtrl);
extern bool jsonComputerReader(char* jsonstr, int len);

pthread_mutex_t PostGetMutex ;


void SetIPinfo(IPInfo *ipInfo)
{
	StrIP=ipInfo->ip;				//IP地址
	StrMask=ipInfo->submask;		//子网掩码
	StrGateway=ipInfo->gateway_addr;//网关
	StrDNS=ipInfo->dns;				//DNS地址
    /*
	Setconfig("IP=",ipInfo->ip);
	Setconfig("Mask=",ipInfo->submask);
	Setconfig("Gateway=",ipInfo->gateway_addr);
	Setconfig("DNS=",ipInfo->dns);

    Writeconfig();
    */

    string strwrconfig = "IP=" + StrIP + "\n" + "Mask=" + StrMask + "\n" + "Gateway=" + StrGateway + "\n" + "DNS=" + StrDNS + "\n" ;
    WriteNetconfig((char *)(strwrconfig.c_str()),strwrconfig.length());


}

void GetIPinfo(IPInfo *ipInfo)
{
	sprintf(ipInfo->ip,StrIP.c_str());
	sprintf(ipInfo->submask ,StrMask.c_str());
	sprintf(ipInfo->gateway_addr,StrGateway.c_str());
	sprintf(ipInfo->dns ,StrDNS.c_str());
}

void SetConfig(VMCONTROL_PARAM *vmctrl_param)
{
	char key[20];
	int i,rsucnt,vehplatecnt;

	StrCabinetType=vmctrl_param->CabinetType;				//机柜类型
	StrHWServer=vmctrl_param->HWServer;				//华为服务器地址
	StrHWGetPasswd=vmctrl_param->HWGetPasswd;				//SNMP GET 密码
	StrHWSetPasswd=vmctrl_param->HWSetPasswd;				//SNMP SET 密码
	StrServerURL1=vmctrl_param->ServerURL1;		//
	StrServerURL2=vmctrl_param->ServerURL2;//
	StrServerURL3=vmctrl_param->ServerURL3;				//
	StrStationURL=vmctrl_param->StationURL;		//
	StrRSUCount=vmctrl_param->RSUCount;		//RSU数量
	rsucnt=atoi(vmctrl_param->RSUCount);
	for(i=0;i<rsucnt;i++)
	{
		StrRSUIP[i]=vmctrl_param->RSUIP[i];		//RSUIP地址
		StrRSUPort[i]=vmctrl_param->RSUPort[i];		//RSU端口
	}
	StrVehPlateCount=vmctrl_param->VehPlateCount;		//识别仪数量
	vehplatecnt=atoi(vmctrl_param->VehPlateCount);
	for(i=0;i<vehplatecnt;i++)
	{
		StrVehPlateIP[i]=vmctrl_param->VehPlateIP[i];		//
		StrVehPlatePort[i]=vmctrl_param->VehPlatePort[i];		//
		StrVehPlateKey[i]=vmctrl_param->VehPlateKey[i];		//用户名密码
	}
	StrCAMIP=vmctrl_param->CAMIP;		//
	StrCAMPort=vmctrl_param->CAMPort;		//
	StrCAMKey=vmctrl_param->CAMKey;		//监控摄像头用户名密码

	StrFireWareIP=vmctrl_param->FireWareIP;		//防火墙地址
	StrFireWareGetPasswd=vmctrl_param->FireWareGetPasswd;		///防火墙get密码
	StrFireWareSetPasswd=vmctrl_param->FireWareSetPasswd;		///防火墙set密码
	StrSwitchIP=vmctrl_param->SwitchIP;		//交换机地址
	StrSwitchGetPasswd=vmctrl_param->SwitchGetPasswd;		//交换机get密码
	StrSwitchSetPasswd=vmctrl_param->SwitchSetPasswd;		//交换机set密码

	for(i=0;i<LOCK_NUM;i++)
		StrAdrrLock[i]=vmctrl_param->LockAddr[i];		//门锁地址
	for(i=0;i<VA_METER_BD_NUM;i++)
		StrAdrrVAMeter[i]=vmctrl_param->VameterAddr[i];		//电能表地址
	for(i=0;i<POWER_BD_NUM;i++)
		StrAdrrPower[i]=vmctrl_param->PowerAddr[i];		//电源板地址
	for(i=0;i<SWITCH_COUNT;i++)
	{
		StrDeviceNameSeq[i]=vmctrl_param->DeviceNameSeq[i];		//设备名称的配置
		StrDoSeq[i]=vmctrl_param->DoSeq[i];		//车牌识别DO映射 最多12路车牌识别
	}
	StrID=vmctrl_param->hardwareid;			//硬件ID

	StrCabinetType=vmctrl_param->CabinetType;		//机柜类型
	StrFlagNetRoadID=vmctrl_param->FlagNetRoadID; //ETC 门架路网编号
	StrFlagRoadID=vmctrl_param->FlagRoadID;		//ETC 门架路段编号
	StrFlagID=vmctrl_param->FlagID;			//ETC 门架编号
	StrPosId=vmctrl_param->PosId; 		//ETC 门架序号
	StrDirection=vmctrl_param->Direction; 	//行车方向
	StrDirDescription=vmctrl_param->DirDescription;	//行车方向说明

	Setconfig("IP=",StrIP.c_str());
	Setconfig("Mask=",StrMask.c_str());
	Setconfig("Gateway=",StrGateway.c_str());
	Setconfig("DNS=",StrDNS.c_str());

	Setconfig("CabinetType=",vmctrl_param->CabinetType);		//机柜类型
	Setconfig("HWServer=",vmctrl_param->HWServer);		//华为服务器地址
	Setconfig("HWGetPasswd=",vmctrl_param->HWGetPasswd);		//SNMP GET 密码
	Setconfig("HWSetPasswd=",vmctrl_param->HWSetPasswd);		//SNMP SET 密码
	Setconfig("ServerURL1=",vmctrl_param->ServerURL1);
	Setconfig("ServerURL2=",vmctrl_param->ServerURL2);
	Setconfig("ServerURL3=",vmctrl_param->ServerURL3);
	Setconfig("StationURL=",vmctrl_param->StationURL);
	Setconfig("RSUCount=",vmctrl_param->RSUCount);
	rsucnt=atoi(vmctrl_param->RSUCount);
	for(i=0;i<rsucnt;i++)
	{
		sprintf(key,"RSU%dIP=",i+1);//RSUIP地址
		Setconfig(key,vmctrl_param->RSUIP[i]);

		sprintf(key,"RSU%dPort=",i+1);//RSU端口
		Setconfig(key,vmctrl_param->RSUPort[i]);
	}
	Setconfig("VehPlateCount=",vmctrl_param->VehPlateCount);
	vehplatecnt=atoi(vmctrl_param->VehPlateCount);
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(key,"VehPlate%dIP=",i+1);//识别仪IP地址
		Setconfig("key=",vmctrl_param->VehPlateIP[i]);

		sprintf(key,"VehPlate%dPort=",i+1);//识别仪端口
		Setconfig(key,vmctrl_param->VehPlatePort[i]);

		sprintf(key,"VehPlate%dKey=",i+1);//识别仪用户名密码
		Setconfig(key,vmctrl_param->VehPlateKey[i]);
	}
	Setconfig("CAMIP=",vmctrl_param->CAMIP);
	Setconfig("CAMPort=",vmctrl_param->CAMPort);
	Setconfig("FireWareIP=",vmctrl_param->FireWareIP);//防火墙地址
	Setconfig("FireWareGetPasswd=",vmctrl_param->FireWareGetPasswd);//防火墙get密码
	Setconfig("FireWareSetPasswd=",vmctrl_param->FireWareSetPasswd);//防火墙set密码
	Setconfig("SwitchIP=",vmctrl_param->SwitchIP);//交换机地址
	Setconfig("SwitchGetPasswd=",vmctrl_param->SwitchGetPasswd);//交换机get密码
	Setconfig("SwitchSetPasswd=",vmctrl_param->SwitchSetPasswd);//交换机set密码

	Setconfig("LOCKADD1=",vmctrl_param->LockAddr[0]);//门锁地址1
	Setconfig("LOCKADD2=",vmctrl_param->LockAddr[1]);//门锁地址2
	Setconfig("LOCKADD3=",vmctrl_param->LockAddr[2]);//门锁地址2
	Setconfig("VAMETERADDR1=",vmctrl_param->VameterAddr[0]);//电能表地址1
	Setconfig("VAMETERADDR2=",vmctrl_param->VameterAddr[1]);//电能表地址2
	Setconfig("VAMETERADDR3=",vmctrl_param->VameterAddr[2]);//电能表地址3
	Setconfig("VAMETERADDR4=",vmctrl_param->VameterAddr[3]);//电能表地址4
	Setconfig("VAMETERADDR5=",vmctrl_param->VameterAddr[4]);//电能表地址5
	Setconfig("VAMETERADDR6=",vmctrl_param->VameterAddr[5]);//电能表地址6
	Setconfig("POWERBDADD1=",vmctrl_param->PowerAddr[0]);//电源板地址1
	Setconfig("POWERBDADD2=",vmctrl_param->PowerAddr[1]);//电源板地址2
	Setconfig("POWERBDADD3=",vmctrl_param->PowerAddr[2]);//电源板地址3

	for(int i=0;i<SWITCH_COUNT;i++)
	{
		sprintf(key,vmctrl_param->DeviceNameSeq[i]);//设备名称的配置
		Setconfig(key,vmctrl_param->DoSeq[i]);//车牌识别DO映射
	}

	Setconfig("ID=",vmctrl_param->hardwareid);//硬件ID
	Setconfig("FlagNetRoadID=",vmctrl_param->FlagNetRoadID);
	Setconfig("FlagRoadID=",vmctrl_param->FlagRoadID);
	Setconfig("FlagID=",vmctrl_param->FlagID);
	Setconfig("PosId=",vmctrl_param->PosId);
	Setconfig("Direction=",vmctrl_param->Direction);
	Setconfig("DirDescription=",vmctrl_param->DirDescription);

	Writeconfig();
}
void GetConfig(VMCONTROL_PARAM *vmctrl_param)
{
	int i,rsucnt,vehplatecnt;

	sprintf(vmctrl_param->HWServer ,StrHWServer.c_str());		//华为服务器地址
	sprintf(vmctrl_param->HWGetPasswd ,StrHWGetPasswd.c_str());		//SNMP GET 密码
	sprintf(vmctrl_param->HWSetPasswd ,StrHWSetPasswd.c_str());		//SNMP SET 密码
	sprintf(vmctrl_param->ServerURL1 ,StrServerURL1.c_str());
	sprintf(vmctrl_param->ServerURL2 ,StrServerURL2.c_str());
	sprintf(vmctrl_param->ServerURL3 ,StrServerURL3.c_str());
	sprintf(vmctrl_param->StationURL ,StrStationURL.c_str());
	sprintf(vmctrl_param->RSUCount ,StrRSUCount.c_str());
	rsucnt=atoi(StrRSUCount.c_str());
	for(i=0;i<rsucnt;i++)
	{
		sprintf(vmctrl_param->RSUIP[i] ,StrRSUIP[i].c_str());
		sprintf(vmctrl_param->RSUPort[i] ,StrRSUPort[i].c_str());
	}
	sprintf(vmctrl_param->VehPlateCount ,StrVehPlateCount.c_str());
	vehplatecnt=atoi(StrVehPlateCount.c_str());
	for(i=0;i<vehplatecnt;i++)
	{
		sprintf(vmctrl_param->VehPlateIP[i] ,StrVehPlateIP[i].c_str());
		sprintf(vmctrl_param->VehPlatePort[i] ,StrVehPlatePort[i].c_str());
		sprintf(vmctrl_param->VehPlateKey[i] ,StrVehPlateKey[i].c_str());
	}
	sprintf(vmctrl_param->CAMIP ,StrCAMIP.c_str());
	sprintf(vmctrl_param->CAMPort ,StrCAMPort.c_str());
	sprintf(vmctrl_param->CAMKey ,StrCAMKey.c_str());//监控摄像头用户名密码
	sprintf(vmctrl_param->FireWareIP ,StrFireWareIP.c_str());	//防火墙地址
	sprintf(vmctrl_param->FireWareGetPasswd ,StrFireWareGetPasswd.c_str());	//防火墙get密码
	sprintf(vmctrl_param->FireWareSetPasswd ,StrFireWareSetPasswd.c_str());	//防火墙set密码
	sprintf(vmctrl_param->SwitchIP ,StrSwitchIP.c_str());	//交换机地址
	sprintf(vmctrl_param->SwitchGetPasswd ,StrSwitchGetPasswd.c_str());	//交换机get密码
	sprintf(vmctrl_param->SwitchSetPasswd ,StrSwitchSetPasswd.c_str());	//交换机set密码

	for(int i=0;i<LOCK_NUM;i++)
		sprintf(vmctrl_param->LockAddr[i] ,StrAdrrLock[i].c_str());	//门锁地址
	for(int i=0;i<VA_METER_BD_NUM;i++)
		sprintf(vmctrl_param->VameterAddr[i] ,StrAdrrVAMeter[i].c_str());	//电能表地址
	for(int i=0;i<POWER_BD_NUM;i++)
		sprintf(vmctrl_param->PowerAddr[i] ,StrAdrrPower[i].c_str());	//电源板地址
	for(int i=0;i<SWITCH_COUNT;i++)
	{
		sprintf(vmctrl_param->DeviceNameSeq[i] ,StrDeviceNameSeq[i].c_str());	//设备名称的配置
		sprintf(vmctrl_param->DoSeq[i] ,StrDoSeq[i].c_str());	//DO映射

	}

	sprintf(vmctrl_param->CabinetType ,StrCabinetType.c_str());//机柜类型
	sprintf(vmctrl_param->FlagNetRoadID ,StrFlagNetRoadID.c_str());
	sprintf(vmctrl_param->FlagRoadID ,StrFlagRoadID.c_str());
	sprintf(vmctrl_param->FlagID ,StrFlagID.c_str());
	sprintf(vmctrl_param->PosId ,StrPosId.c_str());
	sprintf(vmctrl_param->Direction ,StrDirection.c_str());
	sprintf(vmctrl_param->DirDescription ,StrDirDescription.c_str());

	sprintf(vmctrl_param->deviceType,StrdeviceType.c_str());		//设备型号900~919
	sprintf(vmctrl_param->hardwareid,StrID.c_str());		//硬件ID
	sprintf(vmctrl_param->softVersion,StrVersionNo.c_str()); 		//主程序版本号920
	sprintf(vmctrl_param->softDate,StrSoftDate.c_str()); 			//版本日期
}

void initServer()
{
	//获取RTC时钟
//	gRTCfd=rtc_init();

	pthread_t tNetwork_server;
	if (pthread_create(&tNetwork_server, NULL, NetWork_server_thread,NULL))
	{
		printf("NetWork server create failed!\n");
	}
	pthread_detach(tNetwork_server);
}

void* NetWork_server_thread(void *param)
{
	param=NULL;
	int result,nlen;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	int sockfd;                        	// Socket file descriptor
	int newfd;               		// New Socket file descriptor
	int num;
	socklen_t sin_size;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char buf[NETPACKET_MAXLEN];
	int ret;
	int i;
	SocketPara sockpara;
	sockpara.IsQuit = 0;
	char * jsonPack=(char *)malloc(JSON_LEN);
	if(jsonPack==NULL)
	{
		myprintf("NetWork_server_thread jsonPack malloc error!\n");
		return 0;
	}
	int jsonPackLen=0;

	/* Get the Socket file descriptor */
	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
    	printf ("ERROR: Failed to obtain Socket Despcritor.\n");
    	return (0);
	} else {
    	printf ("OK: Obtain Socket Despcritor sucessfully.\n");
	}

	/* Fill the local socket address struct */
	server_addr.sin_family = AF_INET;           		// Protocol Family
	server_addr.sin_port = htons (PORT);         		// Port number
	server_addr.sin_addr.s_addr  = htonl (INADDR_ANY);  	// AutoFill local address
	memset (server_addr.sin_zero,0,8);          		// Flush the rest of struct

	/*  Blind a special Port */
	if( bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1 )
	{
	  	printf ("ERROR: Failed to bind Port %d.\n",PORT);
		close(sockfd);
		return (0);
	} else {
    	printf("OK: Bind the Port %d sucessfully.\n",PORT);
	}

	/*  Listen remote connect/calling */
	if(listen(sockfd,BACKLOG) == -1)
	{
    	printf ("ERROR: Failed to listen Port %d.\n", PORT);
		close(sockfd);
		return (0);
	} else {
    	printf ("OK: Listening the Port %d sucessfully.\n", PORT);
	}

	//hym 2019.05.08
	int nSendBuffLen = 1048576;//1M //1000000;
	nlen = 4;
	result = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &nSendBuffLen, nlen);

	result = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout,	sizeof(struct timeval));

	int keepAlive = 1;//
	int keepIdle = 5;//
	int keepInterval = 5;//
	int keepCount = 3;//
	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*) &keepAlive,
			sizeof(keepAlive)) == -1) {
		printf("SO_KEEPALIVE Error!\n");
	}

	if (setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void *) &keepIdle,
			sizeof(keepIdle)) == -1) {
		printf("TCP_KEEPIDLE Error!\n");
	}

	if (setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *) &keepInterval,
			sizeof(keepInterval)) == -1) {
		printf("TCP_KEEPINTVL Error!\n");
	}

	if (setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *) &keepCount,
			sizeof(keepCount)) == -1) {
		printf("TCP_KEEPCNT Error!\n");
	}
	//hym end

	fd_set fdsr;
	int maxsock;
	struct timeval tv;

	conn_amount = 0;
	sin_size = sizeof(client_addr);
	maxsock = sockfd;
	while (1) {
		FD_ZERO(&fdsr);
		FD_SET(sockfd, &fdsr);

		// timeout setting
		tv.tv_sec = 30;
		tv.tv_usec = 0;

		// add active connection to fd set
		for (i = 0; i < BACKLOG; i++) {
			if (fd_A[i] != 0) {
				FD_SET(fd_A[i], &fdsr);
				if(maxsock< fd_A[i])
					maxsock = fd_A[i];
			}
		}

		ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
		if (ret < 0) {
			perror("select");
		} else if (ret == 0) {

			continue;
		}

		for (i = 0; i < conn_amount; i++)
		{
			if (FD_ISSET(fd_A[i], &fdsr))
			{
				ret = recv(fd_A[i], buf, NETCMD_HEADERLEN, 0);
//				printf("Client_CmdProcessbuff: 000 ret=%d\r\n",ret);
				if (ret <= 0)
				{ // client close
					printf("client[%d] close\n", i);
					close(fd_A[i]);
					FD_CLR(fd_A[i], &fdsr);
					fd_A[i] = 0;
					if(i==conn_amount-1)
						conn_amount--;
				}
				else
				{
					if(ret==NETCMD_HEADERLEN)
					{

						NETCMD_HEADER *pheader = (NETCMD_HEADER *) buf;
						if (pheader->magic == NETCMD_MAGIC)
						{
//							printf("Client_CmdProcessbuff: datalen=%d\r\n",pheader->datalen);
							if (pheader->datalen > 0)
							{
								ret += recv(fd_A[i], pheader->data,	pheader->datalen, 0);
							}

//							printf("Client_CmdProcessbuff: ret=%d\r\n",ret);
							if (ret == NETCMD_HEADERLEN + pheader->datalen)
							{
								//printf("");
								//("Client_CmdProcessbuff:%s\r\n",buf);
								printf("cmd=%d,len=%d",pheader->cmd,pheader->datalen);
								Client_CmdProcess(fd_A[i], buf,	(void *) &sockpara);
//								printf("len=%d,data=%s",pheader->datalen,pheader->data );
								NetSend(fd_A[i], buf,pheader->datalen + NETCMD_HEADERLEN);
							}
						}
						else
						{
							//printf("client[%d] not a current client force closed\n",i);
							close(fd_A[i]);
							FD_CLR(fd_A[i], &fdsr);
							fd_A[i] = 0;
							if (i == conn_amount - 1)
								conn_amount--;

						}
					}

				}
			}
		}
		//printf("sockfd%d1212\r\n",sockfd);

		if (FD_ISSET(sockfd, &fdsr))
		{
			//printf("sockfd*****\r\n");
			newfd = accept(sockfd, (struct sockaddr *) &client_addr,&sin_size);
			//printf("newfd%d\r\n",newfd);
			if (newfd <= 0)
			{
				perror("accept");
				continue;
			}

			// add to fd queue
			if (conn_amount < BACKLOG)
			{
				//find empty
				for (i = 0; i < conn_amount; i++)
				{
					if (fd_A[i] == 0)
					{
						fd_A[i] = newfd;
						break;
					}
				}

				if(i==conn_amount)
				{
					fd_A[i] = newfd;
					conn_amount++;
				}
				printf("new connection client[%d] %s:%d\n", i,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
				if (newfd > maxsock)
					maxsock = newfd;
				//有新客户端连接，主动发门架状态信息
				memset(jsonPack,0,JSON_LEN);
                string mstrdata;
                SetjsonFlagRunStatusStr(NETCMD_FLAGRUNSTATUS,mstrdata);
				//printf("%s",jsonPack);
                NetSendParm(NETCMD_FLAGRUNSTATUS,(char *)(mstrdata.c_str()),mstrdata.size());

			}
		}
	}
	free(jsonPack);
}

void Client_CmdProcess(int fd, char *cmdbuffer,void *arg)
{
	NETCMD_HEADER *pCMD = (NETCMD_HEADER *) cmdbuffer;

	SocketPara *socketpara = (SocketPara *) arg;
	IPInfo ipinfo;
	VMCONTROL_PARAM vmctrl_param;
	//SYSTEMTIME system_time;
	char tmpStringData[100],tmpstr[50];
	struct rtc_time rtc_tm;
	int ret;
	char * jsonPack=(char *)malloc(JSON_LEN);
	if(jsonPack==NULL)
	{
		myprintf("Client_CmdProcess jsonPack malloc error!\n");
		return ;
	}
	int jsonPackLen=0;
	char * pRecvBuf=(char *)malloc(JSON_LEN);
	if(pRecvBuf==NULL)
	{
		myprintf("Client_CmdProcess pRecvBuf malloc error!\n");
		return ;
	}

	unsigned char  regAddr;
	unsigned short regValue;
	int status;
	if(pCMD->cmd!=NETCMD_PING)
	{
		//printf("Client_CmdProcess cmd =%d \r\n",pCMD->cmd);
		sprintf(tmpStringData,"Client_CmdProcess cmd =%d \r\n",pCMD->cmd);
		printf(tmpStringData);
//		WriteLog(tmpStringData);
	}
	switch (pCMD->cmd)
	{
		case NETCMD_CONTROLERID:		//6 保留
			sprintf(tmpStringData,"LTKJ-CONTROLER-V1.0");
			memcpy( (char *) pCMD->data,tmpStringData,strlen(tmpStringData));
			pCMD->datalen = strlen(tmpStringData);

			break;
		case NETCMD_DATETIME: 			//1 设置日期时间
			if(pCMD->status==SFLAG_WRITE)
			{
				memcpy(tmpstr,pCMD->data,pCMD->datalen);
//				sprintf(tmpStringData,"date %s\n",tmpstr);	//A287
				sprintf(tmpStringData,"date -s \"%s\"\n",tmpstr);		//IoT 9100
				printf("setSystemTimer %s",tmpStringData);
				system(tmpStringData);		//设置日期时间
				system("hwclock -w");		//写入硬时钟
				pCMD->datalen =  0;

			}
			break;

		case NETCMD_NETWORK: 			//2 设置网络
			if(pCMD->status==SFLAG_READ)
			{
				printf("Get IP Addr\n");
				GetIPinfo(&ipinfo);
				memcpy((char *) pCMD->data,(char *) &ipinfo,	sizeof(IPInfo));
				pCMD->datalen = sizeof(IPInfo);
			}
			else if(pCMD->status==SFLAG_WRITE)
			{
				printf("Set IP Addr len=%d, %s\n",pCMD->datalen,pCMD->data);
				memcpy((char *) &ipinfo, (char *) pCMD->data, pCMD->datalen);
				SetIPinfo(&ipinfo);
				//重新读设置文件
				GetConfig();
				pCMD->datalen = 0;
                system("reboot") ;
			}
			break;
		case NETCMD_REBOOT: 			//4 重启设备指令
			pCMD->datalen = 0;
			printf("System Reboot\n");
			system("reboot") ;
			break;

		case NETCMD_CONFIG_PARA: 		//7 设置参数
			if(pCMD->status==SFLAG_READ)
			{
				memset(&vmctrl_param,0,sizeof(VMCONTROL_PARAM));
				GetConfig(&vmctrl_param);
				memcpy((char *) pCMD->data,(char *) &vmctrl_param,	sizeof(VMCONTROL_PARAM));
				pCMD->datalen = sizeof(VMCONTROL_PARAM);
				printf("NETCMD_CONFIG_PARA read param = %s length=%d \n",pCMD->data,pCMD->datalen);
			}
			else if(pCMD->status==SFLAG_WRITE)
			{
				printf("NETCMD_CONFIG_PARA write param = %s length=%d \n",pCMD->data,pCMD->datalen);
				memcpy((char *) &vmctrl_param, (char *) pCMD->data, pCMD->datalen);
				SetConfig(&vmctrl_param);
				//重新读设置文件
				//GetConfig();
				pCMD->datalen = 0;
			}
			break;

		case NETCMD_CONFIG_NETWORK:		//5 设置网络
			if(pCMD->status==SFLAG_WRITE)
			{
				printf("NETCMD_CONFIG_NETWORK write param = %s length=%d \n",pCMD->data,pCMD->datalen);
				string iptmp,masktmp,gatewaytmp;
				iptmp=StrIP;masktmp=StrMask;gatewaytmp=StrGateway;//保存原IP设置
				memset(pRecvBuf,0,JSON_LEN);
				memcpy(pRecvBuf,pCMD->data,pCMD->datalen);
				jsonstrIpInfoReader(pRecvBuf,pCMD->datalen,(UINT8*)&ipinfo);//将json字符串转换成结构体
				SetIPinfo(&ipinfo);
				//重新读设置文件
				//GetConfig();
				pCMD->datalen = 0;
				if(iptmp!=StrIP || masktmp!=StrMask || gatewaytmp!=StrGateway)	//IP设置更改
					stuRemote_Ctrl->SysReset=SYSRESET; 	//等待重启
			}
			break;

		case NETCMD_SEND_ENVI_PARAM: 			//9 环境寄存器参数
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				ENVI_PARAMS *pstrEnvPam=stuEnvi_Param;
				jsonStrEvnWriter(pCMD->cmd,(char*)pstrEnvPam,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			break;

		case NETCMD_SEND_UPS_PARAM: 			//10 UPS参数
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				UPS_PARAMS *pstrUpsPam=stuUps_Param;
				jsonStrUpsWriter(pCMD->cmd,(char*)pstrUpsPam,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			break;

		case NETCMD_SEND_SPD_PARAM: 			//11 防雷器寄存器参数
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				SPD_PARAMS *pstrSpdPam=stuSpd_Param;
				jsonStrSpdWriter(pCMD->cmd,(char*)pstrSpdPam,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			break;
		case NETCMD_SEND_DEV_PARAM: 			//12 控制器参数
			if(pCMD->status==SFLAG_READ)
			{
				GetConfig(&vmctrl_param);
				memset(jsonPack,0,JSON_LEN);
				int jsonPackLen=0;
				jsonStrVMCtlParamWriter(pCMD->cmd,(char*)&vmctrl_param,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			else if(pCMD->status==SFLAG_WRITE)
			{
				printf("NETCMD_SEND_DEV_PARAM write param = %s length=%d \n",pCMD->data,pCMD->datalen);
				string iptmp,masktmp,gatewaytmp;
				iptmp=StrIP;masktmp=StrMask;gatewaytmp=StrGateway;//保存原IP设置
				memset(pRecvBuf,0,JSON_LEN);
				memcpy(pRecvBuf,pCMD->data,pCMD->datalen);
				jsonstrVmCtlParamReader(pRecvBuf,pCMD->datalen,(UINT8*)&vmctrl_param,(UINT8*)&ipinfo);//将json字符串转换成结构体
				SetConfig(&vmctrl_param);
				//SetConfig(&vmctrl_param);
				//重新读设置文件
				//GetConfig();
				pCMD->datalen = 0;
				if(iptmp!=StrIP || masktmp!=StrMask || gatewaytmp!=StrGateway)	//IP设置更改
					stuRemote_Ctrl->SysReset=SYSRESET; 	//等待重启
			}
			break;
		case NETCMD_SEND_AIR_PARAM: 			//13 空调参数
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				AIRCOND_PARAM *pstuAirPam=stuAirCondRead;
				jsonStrAirCondWriter(pCMD->cmd,(char*)pstuAirPam,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			else if(pCMD->status==SFLAG_WRITE)
			{
				printf("NETCMD_SEND_AIR_PARAM write param = %s length=%d \n",pCMD->data,pCMD->datalen);
				memset(pRecvBuf,0,JSON_LEN);
				memcpy(pRecvBuf,pCMD->data,pCMD->datalen);
				AIRCOND_PARAM *pstuAirPam=stuAirCondWrite;
				memset(pstuAirPam,0,sizeof(REMOTE_CONTROL));
				jsonstrAirCondReader(pRecvBuf,pCMD->datalen,(UINT8 *)pstuAirPam);//将json字符串转换成结构体
				//AirCondControl((UINT8*)pstuAirPam);
				pCMD->datalen = 0;
			}
			break;
		case NETCMD_SEND_RSU_PARAM: 			//14 RSU天线状态
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				jsonStrRsuWriter(pCMD->cmd,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			break;
		case NETCMD_SEND_CAM_PARAM: 			//15 车牌识别仪状态
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				jsonStrVehPlateWriter(pCMD->cmd,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			break;
		case NETCMD_FLAGRUNSTATUS: 			//17 门架运行状态
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
                string mstrdata;
                SetjsonFlagRunStatusStr(pCMD->cmd,mstrdata);
                memcpy((char *) pCMD->data,(char *)(mstrdata.c_str()),mstrdata.size());
				pCMD->datalen = jsonPackLen;
			}
			break;
		case NETCMD_REMOTE_CONTROL: 			//18 遥控设备
			if(pCMD->status==SFLAG_WRITE)
			{
				memset(pRecvBuf,0,JSON_LEN);
				memcpy(pRecvBuf,pCMD->data,pCMD->datalen);
				REMOTE_CONTROL *pRCtrl=stuRemote_Ctrl;
				memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
				jsonstrRCtrlReader(pRecvBuf,pCMD->datalen,(UINT8 *)pRCtrl);//将json字符串转换成结构体
				RemoteControl((UINT8*)pRCtrl);
				sprintf(pCMD->data,"执行命令->执行成功!\n");
				pCMD->datalen = strlen(pCMD->data);
			}
			break;
		case NETCMD_SWITCH_STATUS: 			//19 回路开关状态
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				jsonStrSwitchStatusWriter(pCMD->cmd,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			break;
		case NETCMD_HWCABINET_STATUS: 			//20  华为机柜状态
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				jsonStrHWCabinetWriter(pCMD->cmd,(char*)&HUAWEIDevValue,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			break;
		case NETCMD_DEAL_LOCKER:			//22  门禁开关锁请求
			if(pCMD->status==SFLAG_WRITE)
			{
				memset(pRecvBuf,0,JSON_LEN);
				memcpy(pRecvBuf,pCMD->data,pCMD->datalen);
				REMOTE_CONTROL *pRCtrl=stuRemote_Ctrl;
				memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
				jsonstrRCtrlReader(pRecvBuf,pCMD->datalen,(UINT8 *)pRCtrl);//将json字符串转换成结构体
				RemoteControl((UINT8*)pRCtrl);
				sprintf(pCMD->data,"执行命令->执行成功!\n");
				pCMD->datalen = strlen(pCMD->data);
			}
			break;
		default:
			break;

	}
	free(jsonPack);
	free(pRecvBuf);
 }

 int NetSend(int s,char *pbuffer,int nsize)
 {
   int nSendLen = 0;
   int nLen = 0;
   while(nSendLen < nsize) {
	 // printf("nsize :%d\r\n",nsize);
	 nLen = send(s, pbuffer+nSendLen, nsize - nSendLen, MSG_NOSIGNAL);
	 //printf("nLen :%d\r\n",nLen);
	 if(nLen<=0)
	   break;
	 nSendLen += nLen;
   }
   return nSendLen;
 }


 int  MySendMessage(char *pBuf)
 {
	 int nsendlen;
	 int i,nlen;
	 char done = 0;

	 NETCMD_HEADER netcmd;
	 netcmd.cmd 	= NETCMD_SEND_DATA;
	 netcmd.magic	= 0x12345678;
	 netcmd.status	= SFLAG_READ;
	 netcmd.datalen = strlen(pBuf);
//	 netcmd.data    = pBuf;


	 for (i = 0; i < BACKLOG; i++) {
		 if (fd_A[i] > 0) {
			 nlen = NetSend(fd_A[i], (char *) &netcmd,NETCMD_HEADERLEN);

		 	 nsendlen=strlen(pBuf);
			 if (nsendlen > 0) {
				 nlen = NetSend(fd_A[i], (char *) pBuf, nsendlen);
				 if (nlen != nsendlen) {
					 printf("send data error, avisock=%d\n", fd_A[i]);
					 return -1;
				 }else
					 done = 1;
			 }
		 }
//		 else
//			 printf("soket %d is unable=%d\n", i,fd_A[i]);

	 }

	 return 0;
 }

  int  NetSendParm(NETCMD_TYPE cmd,char *pBuf,int len)
  {
	  int nsendlen;
	  int i,nlen;
	  char done = 0;

	  NETCMD_HEADER netcmd;
	  netcmd.cmd	 = cmd;
	  netcmd.magic	 = 0x12345678;
	  netcmd.status  = SFLAG_READ;
	  netcmd.datalen = len;


	  for (i = 0; i < BACKLOG; i++) {
		  if (fd_A[i] > 0) {
			  nlen = NetSend(fd_A[i], (char *) &netcmd,NETCMD_HEADERLEN);

			  nsendlen=len;
			  if (nsendlen > 0) {
				  nlen = NetSend(fd_A[i], (char *) pBuf, nsendlen);
				  if (nlen != nsendlen) {
					  printf("send data error, avisock=%d\n", fd_A[i]);
					  return -1;
				  }else
					  done = 1;
			  }
		  }
 // 	  else
 // 		  printf("soket %d is unable=%d\n", i,fd_A[i]);

	  }

	  return 0;
  }

 int Net_close()
 {
	 int i;
	 fd_set fdsr;

	 for (i = 0; i < BACKLOG; i++) {
		 if (fd_A[i] != 0)
		 {
			 close(fd_A[i]);
			 FD_CLR(fd_A[i], &fdsr);
			 fd_A[i] = 0;
		 }
	 }
	 return 0;
 }

void RemoteControl(UINT8* pRCtrl)
{
	 int i,j;
	 REMOTE_CONTROL *pstuRCtrl=(REMOTE_CONTROL *)pRCtrl;
	 int seq = 0;
	 UINT16 seq_temp = 0;
	 char value[10];

	 for (i = 0; i < SWITCH_COUNT; i++)
	 {
//		 printf("do_seqx=0x%02x\r\n",pstuRCtrl->doseq[i]);
	 	 if(pstuRCtrl->doseq[i]==ACT_CLOSE)			 //do分闸
		 {
			 seq_temp = DoSeq[i];
			 printf("DoSeq=0x%02x\r\n",DoSeq[i]);
			 //如果未被配置,则不控制
			 if (seq_temp != NULL_VAR)
			 {
				 seq_temp = i%SINGLE_REMOTE_NUM;
				 seq = i/SINGLE_REMOTE_NUM;
				 printf("seq_temp0x%02x=0x%02x,seq=%d\r\n",i,seq_temp,seq);
				 power_ctrl_flag[seq] |= LBIT(POWER_1_CTRL_CLOSE+2*seq_temp);
				 printf("power_ctrl_flagseq=0x%04x\r\n",power_ctrl_flag[seq]);
			 }
		 }
		 if(pstuRCtrl->doseq[i]==ACT_OPEN)			 //DO合闸
		 {
			 seq_temp = DoSeq[i];
			 printf("DoSeq=0x%02x\r\n",DoSeq[i]);
			 //如果未被配置,则不控制
			 if (seq_temp != NULL_VAR)
			 {
				 seq_temp = i%SINGLE_REMOTE_NUM;
				 seq = i/SINGLE_REMOTE_NUM;
				 printf("seq_temp0x%02x=0x%02x,seq=%d\r\n",i,seq_temp,seq);
				 power_ctrl_flag[seq] |= LBIT(POWER_1_CTRL_OPEN+2*seq_temp);
				 printf("power_ctrl_flagseq=0x%04x\r\n",power_ctrl_flag[seq]);
			 }
		 }
	 }

	 printf("power_ctrl_flag0=0x%04x\r\n",power_ctrl_flag[0]);
//	if(pstuRCtrl->SysReset==SYSRESET)					//系统重启
//		system("reboot") ;

	if(pstuRCtrl->FrontDoorCtrl==ACT_UNLOCK)					//开锁
 	{
printf("FrontDoorCtrl ACT_UNLOCK");
		locker_ctrl_flag |= LBIT(LOCKER_1_CTRL_UNLOCK);
		usleep(2000);
	}
	if(pstuRCtrl->FrontDoorCtrl==ACT_LOCK)					//关锁
 	{
printf("FrontDoorCtrl ACT_LOCK");
 		locker_ctrl_flag |= LBIT(LOCKER_1_CTRL_LOCK);
		usleep(2000);
	}
	if(pstuRCtrl->BackDoorCtrl==ACT_UNLOCK)					//开锁
 	{
printf("BackDoorCtrl ACT_UNLOCK");
 		locker_ctrl_flag |= LBIT(LOCKER_2_CTRL_UNLOCK);
		usleep(2000);
	}
	if(pstuRCtrl->BackDoorCtrl==ACT_LOCK)					//关锁
 	{
printf("BackDoorCtrl ACT_LOCK")	;
 		locker_ctrl_flag |= LBIT(LOCKER_2_CTRL_LOCK);
		usleep(2000);
	}
	if(pstuRCtrl->SideDoorCtrl==ACT_UNLOCK)					//开锁
 	{
printf("SideDoorCtrl ACT_UNLOCK");
 		locker_ctrl_flag |= LBIT(LOCKER_3_CTRL_UNLOCK);
		usleep(2000);
	}
	if(pstuRCtrl->SideDoorCtrl==ACT_LOCK)					//关锁
 	{
printf("SideDoorCtrl ACT_LOCK");
 		locker_ctrl_flag |= LBIT(LOCKER_3_CTRL_LOCK);
		usleep(2000);
	}
	if(pstuRCtrl->hwcoolingdevicesmodectl!=ACT_HOLD)					//温控模式   	0：保持；1：纯风扇模式；2：纯空调模式；3：智能模式；
 	{
 		sprintf(value,"%d",pstuRCtrl->hwcoolingdevicesmodectl);
		printf("RemoteControl 温控模式=%s\n",value);
		SnmpSetOid(hwCoolingDevicesModeCtl,value);
	}
	if(pstuRCtrl->hwdcairpowerontemppointctl!=ACT_HOLD_FF)			//空调开机温度点 		255:保持； -20-80（有效）；45(缺省值)
	{
 		sprintf(value,"%d",pstuRCtrl->hwdcairpowerontemppointctl);
		printf("RemoteControl 空调开机温度点=%s\n",value);
		SnmpSetOid(hwDcAirPowerOnTempPointCtl,value);
	}
	if(pstuRCtrl->hwdcairpowerofftemppointctl!=ACT_HOLD_FF)					//空调关机温度点 		  255:保持； -20-80（有效）；37(缺省值)
 	{
 		sprintf(value,"%d",pstuRCtrl->hwdcairpowerofftemppointctl);
		printf("RemoteControl 空调关机温度点=%s\n",value);
		SnmpSetOid(hwDcAirPowerOffTempPointCtl,value);
	}
}

void DealDoReset(REMOTE_CONTROL *pstuRCtrl)
{
	int i,j;
	string StrDev;
	char dev[30];
	int seq = 0;
	UINT16 seq_temp = 0;

	if(stuRemote_Ctrl->SysReset==SYSRESET)					//系统重启
		system("reboot") ;
	for (i = 0; i < SWITCH_COUNT; i++)
	{
		if(pstuRCtrl->doseq[i]==ACT_HARDWARERST)	//断电重启
		{
			seq_temp = DoSeq[i];
			printf("DoSeq=0x%02x\r\n",DoSeq[i]);
			//如果未被配置,则不控制
			if (seq_temp != NULL_VAR)
			{
				seq_temp = i%SINGLE_REMOTE_NUM;
				seq = i/SINGLE_REMOTE_NUM;
				printf("seq_temp0x%02x=0x%02x,seq=%d\r\n",i,seq_temp,seq);
				power_ctrl_flag[seq] |= LBIT(POWER_1_CTRL_CLOSE+2*seq_temp);
				printf("power_ctrl_flagseq=0x%04x\r\n",power_ctrl_flag[seq]);
				sleep(2);
				printf("seq_temp0x%02x=0x%02x,seq=%d\r\n",i,seq_temp,seq);
				power_ctrl_flag[seq] |= LBIT(POWER_1_CTRL_OPEN+2*seq_temp);
				printf("power_ctrl_flagseq=0x%04x\r\n",power_ctrl_flag[seq]);
			}
			pstuRCtrl->doseq[i]=ACT_HOLD;
		}
		if(pstuRCtrl->doseq[i]==ACT_SOFTWARERST)	//软件重启
		{
			for(j=0;j<atoi(StrVehPlateCount.c_str());j++)
			{
				sprintf(dev,"vehplate%d_do",j+1);
				StrDev=dev;
				if(StrDeviceNameSeq[i]==StrDev)
				{
					printf("do%d  reboot %s\n",i,dev);
					RebootIpCam(j);
				}
			}
			pstuRCtrl->doseq[i]==ACT_HOLD;
		}
	}
}

void *HTTP_DataGetthread(void *param)
{
    string mStrdata = "";
    string mstrkey = ""; //没有用户名和密码：则为“”；
    int start,end;
	while(1)
	{
        HttpPostParm(StrServerURL3,mStrdata,mstrkey,HTTPGET);
        start = mStrdata.find('[');
        end = mStrdata.find(']');
        if((end > start) && (end > 0) && (start >= 0))
        {
           mStrdata = mStrdata.substr(start+1,end-start-1) ;
           jsonComputerReader((char *)(mStrdata.c_str()),mStrdata.size());
        }

        sleep(10);
//		sleep(1);
	}
	return 0 ;
}

void init_HTTP_DataGet()
{
	pthread_mutex_init(&PostGetMutex , NULL);
	pthread_t m_HTTP_DataGetthread ;
    pthread_create(&m_HTTP_DataGetthread,NULL,HTTP_DataGetthread,NULL);
}

void *LTKJ_DataPostthread(void *param)
{
    string mStrdata;
    string mstrkey = ""; //没有用户名和密码：则为“”；
	while(1)
	{

        SetjsonTableStr("flagrunstatus", mStrdata);
        if(StrServerURL1.length()>0)
           HttpPostParm(StrServerURL1,mStrdata,mstrkey,HTTPPOST);

		sleep(300);
//		sleep(1);
	}

	return 0 ;
}

void init_LTKJ_DataPost()
{
	pthread_t m_LTKJ_DataPostthread ;
    pthread_create(&m_LTKJ_DataPostthread,NULL,LTKJ_DataPostthread,NULL);
}

void *XY_DataPostthread(void *param)
{

    string mStrdata;
    string mstrkey = ""; //没有用户名和密码：则为“”；
	while(1)
	{
        SetjsonFlagRunStatusStr(NETCMD_FLAGRUNSTATUS,mStrdata);
        if(StrServerURL2.length()>0)
            HttpPostParm(StrServerURL2,mStrdata,mstrkey,HTTPPOST);

		sleep(300);
//		sleep(1);
	}

	return 0 ;
}

void init_XY_DataPost()
{
	pthread_t m_XY_DataPostthread ;
	pthread_create(&m_XY_DataPostthread,NULL,XY_DataPostthread,NULL);
}

void *SocketNetSendthread(void *param)
{

    string mStrdata;
	while(1)
	{
        //memset(jsonPack,0,JSON_LEN);
        SetjsonFlagRunStatusStr(NETCMD_FLAGRUNSTATUS,mStrdata);
        NetSendParm(NETCMD_FLAGRUNSTATUS,(char *)(mStrdata.c_str()),mStrdata.size());

		sleep(10);
//		sleep(1);
	}

	return 0 ;
}

void init_SocketNetSend()
{
	pthread_t m_SoketNetSendthread ;
	pthread_create(&m_SoketNetSendthread,NULL,SocketNetSendthread,NULL);
}


void *DealDoResetthread(void *param)
{

    string mStrdata;
	while(1)
	{
    	DealDoReset(stuRemote_Ctrl);							//处理DO重启

		sleep(1);
	}

	return 0 ;
}

void init_DealDoReset()
{
	pthread_t m_DealDoResetthread ;
	pthread_create(&m_DealDoResetthread,NULL,DealDoResetthread,NULL);
}



