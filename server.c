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

#define LENGTH 512              		// Buffer length                                                                                 

int fd_A[BACKLOG];    // accepted connection fd
int conn_amount;      // current connection amount
int data_conamount;

void* NetWork_server_thread(void *param);
void Client_CmdProcess(int fd, char *cmdbuffer,void *arg);
int NetSend(int s,char *pbuffer,int nsize);
void AirCondControl(UINT8* pRCtrl);
void RemoteControl(UINT8* pRCtrl);
extern void WriteLog(char* str);

//extern void SendCom1QueryEvnReg();
extern ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
extern UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体
extern DEVICE_PARAMS *stuDev_Param;		//装置参数寄存器
extern RSU_PARAMS *stuRSU_Param;		//RSU天线信息结构体
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
extern string StrServerURL1;	//服务端URL1
extern string StrServerURL2;	//服务端URL2
extern string StrServerURL3;	//服务端URL3
extern string StrStationURL;	//虚拟站端URL
extern string StrRSUIP;	//RSUIP地址
extern string StrRSUPort;	//RSU端口
extern string StrVehPlate1IP;	//识别仪1IP地址
extern string StrVehPlate1Port;	//识别仪1端口
extern string StrCAMIP;	//监控摄像头IP地址
extern string StrCAMPort;	//监控摄像头端口

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

void SetIPinfo(IPInfo *ipInfo)
{
	StrIP=ipInfo->ip;				//IP地址
	StrMask=ipInfo->submask;		//子网掩码
	StrGateway=ipInfo->gateway_addr;//网关
	StrDNS=ipInfo->dns;				//DNS地址
	
	Setconfig("IP=",ipInfo->ip);
	Setconfig("Mask=",ipInfo->submask);
	Setconfig("Gateway=",ipInfo->gateway_addr);
	Setconfig("DNS=",ipInfo->dns);

	Writeconfig();
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
	StrHWServer=vmctrl_param->HWServer;				//华为服务器地址
	StrServerURL1=vmctrl_param->ServerURL1;		//
	StrServerURL2=vmctrl_param->ServerURL2;//
	StrServerURL3=vmctrl_param->ServerURL3;				//
	StrStationURL=vmctrl_param->StationURL;		//
	StrRSUIP=vmctrl_param->RSUIP;		//
	StrRSUPort=vmctrl_param->RSUPort;		//
	StrVehPlate1IP=vmctrl_param->VehPlate1IP;		//
	StrVehPlate1Port=vmctrl_param->VehPlate1Port;		//
	StrCAMIP=vmctrl_param->CAMIP;		//
	StrCAMPort=vmctrl_param->CAMPort;		//
	
	StrCabinetType=vmctrl_param->CabinetType;		//机柜类型
	StrFlagNetRoadID=vmctrl_param->FlagNetRoadID; //ETC 门架路网编号
	StrFlagRoadID=vmctrl_param->FlagRoadID;		//ETC 门架路段编号
	StrFlagID=vmctrl_param->FlagID;			//ETC 门架编号
	StrPosId=vmctrl_param->PosId; 		//ETC 门架序号
	StrDirection=vmctrl_param->Direction; 	//行车方向
	StrDirDescription=vmctrl_param->DirDescription;	//行车方向说明
	
	Setconfig("HWServer=",vmctrl_param->HWServer);		//华为服务器地址
	Setconfig("ServerURL1=",vmctrl_param->ServerURL1);
	Setconfig("ServerURL2=",vmctrl_param->ServerURL2);
	Setconfig("ServerURL3=",vmctrl_param->ServerURL3);
	Setconfig("StationURL=",vmctrl_param->StationURL);
	Setconfig("RSUIP=",vmctrl_param->RSUIP);
	Setconfig("RSUPort=",vmctrl_param->RSUPort);
	Setconfig("VehPlate1IP=",vmctrl_param->VehPlate1IP);
	Setconfig("VehPlate1Port=",vmctrl_param->VehPlate1Port);
	Setconfig("CAMIP=",vmctrl_param->CAMIP);
	Setconfig("CAMPort=",vmctrl_param->CAMPort);
	
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
	sprintf(vmctrl_param->HWServer ,StrHWServer.c_str());		//华为服务器地址
	sprintf(vmctrl_param->ServerURL1 ,StrServerURL1.c_str());
	sprintf(vmctrl_param->ServerURL2 ,StrServerURL2.c_str());
	sprintf(vmctrl_param->ServerURL3 ,StrServerURL3.c_str());
	sprintf(vmctrl_param->StationURL ,StrStationURL.c_str());
	sprintf(vmctrl_param->RSUIP ,StrRSUIP.c_str());
	sprintf(vmctrl_param->RSUPort ,StrRSUPort.c_str());
	sprintf(vmctrl_param->VehPlate1IP ,StrVehPlate1IP.c_str());
	sprintf(vmctrl_param->VehPlate1Port ,StrVehPlate1Port.c_str());
	sprintf(vmctrl_param->CAMIP ,StrCAMIP.c_str());
	sprintf(vmctrl_param->CAMPort ,StrCAMPort.c_str());
	
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
	char sdbuf[LENGTH];          		// Send buffer
	struct sockaddr_in server_addr; 
	struct sockaddr_in client_addr; 
	char sendstr[16]= {"123456789 abcde"}; 
	char buf[NETPACKET_MAXLEN];
	int ret;
	int i;
	SocketPara sockpara;
	sockpara.IsQuit = 0;
	char * jsonPack=(char *)malloc(50*1024);
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
							//printf("Client_CmdProcessbuff:\r\n");
							if (pheader->datalen > 0)
							{
								ret += recv(fd_A[i], pheader->data,	pheader->datalen, 0);
							}
							
							//printf("Client_CmdProcessbuff:\r\n");
							if (ret == NETCMD_HEADERLEN + pheader->datalen) 
							{
								//printf("");
								//("Client_CmdProcessbuff:%s\r\n",buf);
//								printf("cmd=%d,len=%d",pheader->cmd,pheader->datalen);
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
				memset(jsonPack,0,50*1024);
				SetjsonFlagRunStatusStr(jsonPack,&jsonPackLen);
				printf("%s",jsonPack);
				NetSendParm(NETCMD_FLAGRUNSTATUS,jsonPack,jsonPackLen);
				
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
	int jsonPackLen=0;
	char * pRecvBuf=(char *)malloc(JSON_LEN);
	
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
				GetConfig();
				pCMD->datalen = 0;
			}
			break;

		case NETCMD_SEND_ENVI_PARAM: 			//9 环境寄存器参数
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				ENVI_PARAMS *pstrEnvPam=stuEnvi_Param;
				jsonStrEvnWriter((char*)pstrEnvPam,jsonPack,&jsonPackLen);
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
				jsonStrUpsWriter((char*)pstrUpsPam,jsonPack,&jsonPackLen);
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
				jsonStrSpdWriter((char*)pstrSpdPam,jsonPack,&jsonPackLen);
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
				jsonStrVMCtlParamWriter((char*)&vmctrl_param,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
			}
			else if(pCMD->status==SFLAG_WRITE)
			{
				printf("NETCMD_SEND_DEV_PARAM write param = %s length=%d \n",pCMD->data,pCMD->datalen);
				memset(pRecvBuf,0,JSON_LEN);
				memcpy(pRecvBuf,pCMD->data,pCMD->datalen);
				jsonstrVmCtlParamReader(pRecvBuf,pCMD->datalen,(UINT8*)&vmctrl_param,(UINT8*)&ipinfo);//将json字符串转换成结构体
				SetConfig(&vmctrl_param);
				//重新读设置文件
				GetConfig();
				pCMD->datalen = 0;
			}
			break;
		case NETCMD_SEND_AIR_PARAM: 			//13 空调参数 
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				AIRCOND_PARAM *pstuAirPam=stuAirCondRead;
				jsonStrAirCondWriter((char*)pstuAirPam,jsonPack,&jsonPackLen);
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
				AirCondControl((UINT8*)pstuAirPam);
				pCMD->datalen = 0;
			}
			break;
		case NETCMD_SEND_RSU_PARAM: 			//14 RSU天线参数
/*			if(pCMD->status==SFLAG_READ)
			{
				SendCom1ReadReg(POWER_CTRL_ADDR,READ_REGS,RSU_START_ADDR,RSU_REG_MAX);//查询RSU天线参数寄存器
			}*/
			pCMD->datalen = 0;
			break;
		case NETCMD_FLAGRUNSTATUS: 			//17 门架运行状态
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				SetjsonFlagRunStatusStr(jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
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
				pCMD->datalen = 0;
			}
			break;
		case NETCMD_HWCABINET_STATUS: 			//20  华为机柜状态
			if(pCMD->status==SFLAG_READ)
			{
				memset(jsonPack,0,JSON_LEN);
				jsonPackLen=0;
				jsonStrHWCabinetWriter((char*)&HUAWEIDevValue,jsonPack,&jsonPackLen);
				memcpy((char *) pCMD->data,jsonPack,jsonPackLen);
				pCMD->datalen = jsonPackLen;
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
	
/*	if(pstuRCtrl->rsu1==ACT_CLOSE)			//RSU天线1分闸
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,FORCE_COIL,RSU1_REG,SWITCH_OFF);usleep(2000);}//delay 2ms
	if(pstuRCtrl->rsu1==ACT_OPEN)						//RSU天线1合闸
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,FORCE_COIL,RSU1_REG,SWITCH_ON);usleep(2000);}
	if(pstuRCtrl->door_do==ACT_CLOSE)			// 电子门锁关锁
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,FORCE_COIL,DOOR_DO_REG,SWITCH_OFF);usleep(2000);}//delay 2ms
	if(pstuRCtrl->door_do==ACT_OPEN)						//电子门锁开锁
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,FORCE_COIL,DOOR_DO_REG,SWITCH_ON);usleep(2000);}
	if(pstuRCtrl->autoreclosure==ACT_CLOSE)			//自动重合闸分闸
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,FORCE_COIL,AUTORECLOSURE_REG,SWITCH_OFF);usleep(2000);}//delay 2ms
	if(pstuRCtrl->autoreclosure==ACT_OPEN)						//自动重合闸合闸
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,FORCE_COIL,AUTORECLOSURE_REG,SWITCH_ON);usleep(2000);}*/

	for(i=0;i<12;i++,j++)
	{
		if(pstuRCtrl->vehplate[i]==ACT_CLOSE)			//车牌识别1分闸
		{
			ctrl_flag |= LBIT(POWER_1_CTRL_CLOSE+2*i);
			usleep(2000);
		}
		if(pstuRCtrl->vehplate[i]==ACT_OPEN)						//车牌识别1合闸
		{
			ctrl_flag |= LBIT(POWER_1_CTRL_OPEN+2*i);
			usleep(2000);
		}
	}  
//	if(pstuRCtrl->SysReset==WRITE_ENABLE)					//系统重启 1548
//		system("reboot") ;
	
	if(pstuRCtrl->FrontDoorCtrl==SWITCH_OFF)					//开锁
 	{
		ctrl_flag |= LBIT(LOCKER_1_CTRL_UNLOCK);
		usleep(2000);
	}
	if(pstuRCtrl->FrontDoorCtrl==SWITCH_ON)					//关锁
 	{
 		ctrl_flag |= LBIT(LOCKER_1_CTRL_LOCK);
		usleep(2000);
	}
#if (LOCK_NUM >= 2)
	if(pstuRCtrl->BackDoorCtrl==SWITCH_OFF)					//开锁
 	{
 		ctrl_flag |= LBIT(LOCKER_2_CTRL_UNLOCK);
		usleep(2000);
	}
	if(pstuRCtrl->BackDoorCtrl==SWITCH_ON)					//关锁
 	{
 		ctrl_flag |= LBIT(LOCKER_2_CTRL_LOCK);
		usleep(2000);
	}
#endif
}

void AirCondControl(UINT8* pRCtrl)
{
	AIRCOND_PARAM *pstuRCtrl=(AIRCOND_PARAM *)pRCtrl;
	
	if(pstuRCtrl->aircondset==ACT_CLOSE)			//空调关机//1220	
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,PRESET_REGS,AIRCONDSET_REG,SWITCH_OFF);usleep(2000);}//delay 2ms
	if(pstuRCtrl->aircondset==ACT_OPEN)						//空调开机
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,PRESET_REGS,AIRCONDSET_REG,SWITCH_ON);usleep(2000);}
	if(pstuRCtrl->aircoldstartpoint!=0)			// 空调制冷点//1221
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,PRESET_REGS,AIRCOLDSTARTPOINT_REG,pstuRCtrl->aircoldstartpoint);usleep(2000);}//delay 2ms
	if(pstuRCtrl->aircoldloop!=0)						//空调制冷回差//1222	
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,PRESET_REGS,AIRCOLDLOOP_REG,pstuRCtrl->aircoldloop);usleep(2000);}
	if(pstuRCtrl->airhotstartpoint!=0)						//空调制热点//1223 
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,PRESET_REGS,AIRHOTSTARTPOINT_REG,pstuRCtrl->airhotstartpoint);usleep(2000);}
	if(pstuRCtrl->airhotloop!=0)						//空调制热回差//1224
 		{SendCom1RCtlReg(POWER_CTRL_ADDR,PRESET_REGS,AIRHOTLOOP_REG,pstuRCtrl->airhotloop);usleep(2000);}
	  
}

void *LTKJ_DataPostthread(void *param)
{
	char ch;
	int loop=0;
	char str[100];
	char * jsonPack=(char *)malloc(50*1024);
	REMOTE_CONTROL *pRCtrl;
	int jsonPackLen=0;

	while(1)
	{
		memset(jsonPack,0,50*1024);
		SetjsonTableStr("flagrunstatus",jsonPack,&jsonPackLen);
//		printf("%s",jsonPack);
		HttpPostParm(StrServerURL1,jsonPack,jsonPackLen);
		
		sleep(300);
	}
	free(jsonPack);
	return 0 ;
}

void init_LTKJ_DataPost()
{
	pthread_t m_LTKJ_DataPostthread ;
	pthread_create(&m_LTKJ_DataPostthread,NULL,LTKJ_DataPostthread,NULL);
}

void *XY_DataPostthread(void *param)
{
	char ch;
	int loop=0;
	char str[100];
	char * jsonPack=(char *)malloc(50*1024);
	REMOTE_CONTROL *pRCtrl;
	int jsonPackLen=0;

	while(1)
	{
		memset(jsonPack,0,50*1024);
		SetjsonFlagRunStatusStr(jsonPack,&jsonPackLen);
//		printf("%s",jsonPack);
		HttpPostParm(StrServerURL2,jsonPack,jsonPackLen);
		NetSendParm(NETCMD_FLAGRUNSTATUS,jsonPack,jsonPackLen);
		
		sleep(300);
	}
	free(jsonPack);
	return 0 ;
}

void init_XY_DataPost()
{
	pthread_t m_XY_DataPostthread ;
	pthread_create(&m_XY_DataPostthread,NULL,XY_DataPostthread,NULL);
}



