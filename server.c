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
void WriteLog(char* str);
void RemoteControl(UINT8* pRCtrl);

//extern void SendCom1QueryEvnReg();
extern ENVI_PARAMS *stuEnvi_Param;		// 环境数据结构体
extern UPS_PARAMS *stuUps_Param;		//USP结构体 电源数据寄存器
extern SPD_PARAMS *stuSpd_Param;		//防雷器结构体
extern DEVICE_PARAMS *stuDev_Param;		//装置参数寄存器
extern DeviceInfoParams *stuDev_Info;	//采集器设备信息结构体
extern RSU_PARAMS *stuRSU_Param;		//RSU天线信息结构体
extern REMOTE_CONTROL *stuRemote_Ctrl;	//遥控寄存器结构体

extern string StrID;
extern string StrNET;
extern string StrDHCP;
extern string StrIP;
extern string StrMask;
extern string StrGateway;
extern string StrDNS;
extern string StrSERVER;
extern string StrWIFIUSER;
extern string StrWIFIKEY;
extern string StrVersionNo ;
extern string StrServerURL1;
extern string StrServerURL2;
extern string StrAlarmURL ;

int gRTCfd = -1;

extern int Writeconfig(void);
extern int Setconfig(string StrKEY,string StrSetconfig) ;
extern int GetConfig(void);

#define GETRTCTIME  0x90
#define SETRTCTIME  0x91
void setSystemTimer(void * data)
{
//	time_t  _tTime;
//	memcpy((char *) &_tmTime, (char *)data, sizeof(struct tm));
    char str[100];
//	struct rtc_time setrtc_tm;
//	memcpy((char *) &setrtc_tm, (char *) data,	sizeof(setrtc_tm));
	sprintf(str,"date %s\n",data);
	printf("setSystemTimer %s",str);
	system(str);
	
		
/*	struct rtc_time setrtc_tm;
	struct rtc_time getrtc_tm;
	char *rtcbuff;
	time_t  sysTime;
	//{
	struct tm _tmTime;
	time_t  _tTime;
	struct timeval tv;
	int rec;
	int ret;
	memcpy((char *) &_tmTime, (char *)data,	sizeof(struct tm));
	_tTime=mktime(&_tmTime);
	tv.tv_sec = (long)_tTime;
    printf("the second: %ld",tv.tv_sec);
    tv.tv_usec = 0;
	rec = settimeofday(&tv,NULL);
    if(rec <0 )
    {
        printf("settimeofday failed!\n");
    }
    else
    {
        printf("Set system time ok!\n");
    }
	memcpy((char *) &setrtc_tm, (char *) data,	sizeof(setrtc_tm));
	ret = ioctl(gRTCfd, SETRTCTIME, &setrtc_tm);
	if(ret < 0){
		OSA_ERROR("RTC_SET_TIME\n");
	
	}
	//ret = ioctl(gRTCfd, GETRTCTIME, rtcbuff);
	//if(ret < 0){
		//OSA_ERROR("RTC_SET_TIME\n");
	
	//}
	
	//memcpy(&getrtc_tm,rtcbuff,sizeof(getrtc_tm));
	//printf("year :%04d mouth:%02d day:%02d\r\n",getrtc_tm.tm_year,getrtc_tm.tm_mon,getrtc_tm.tm_mday);
	//set_time(gRTCfd,&rtc_tm);
	printf("NETCMD_DATETIME write\r\n");
            //}*/
}

int rtc_init()
{
	gRTCfd = open("/dev/i2c_MCP79401",O_RDWR);//O_WRONLY);
	if(gRTCfd < 0) {
		printf("rtc_init() error!\n");
	}
	return gRTCfd;
}

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

void initServer()
{
	//获取RTC时钟
	gRTCfd=rtc_init();
	
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
		return (0);
	} else {
    	printf("OK: Bind the Port %d sucessfully.\n",PORT);
	}

	/*  Listen remote connect/calling */
	if(listen(sockfd,BACKLOG) == -1)    
	{  
    	printf ("ERROR: Failed to listen Port %d.\n", PORT);
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
	//SYSTEMTIME system_time;
	char tmpStringData[100];
	struct rtc_time rtc_tm;
	int ret;
	
	unsigned char  regAddr;
	unsigned short regValue;
	int status;
	if(pCMD->cmd!=NETCMD_PING)
		printf("Client_CmdProcess cmd =%d \r\n",pCMD->cmd);
	switch (pCMD->cmd)
	{
		case NETCMD_CONTROLERID:
			sprintf(tmpStringData,"LTKJ-CONTROLER-V1.0");
			memcpy( (char *) pCMD->data,tmpStringData,strlen(tmpStringData));
			pCMD->datalen = strlen(tmpStringData);
			
			break;
		case NETCMD_DATETIME: 
			if(pCMD->status==SFLAG_WRITE)
			{
				sprintf(tmpStringData,"date %s\n",pCMD->data);
				printf("setSystemTimer %s",tmpStringData);
				system(tmpStringData);
//  				  	system("reboot") ;
//					setSystemTimer((void *)pCMD->data);
				pCMD->datalen =  0;
			
			}
			else
			{
						pCMD->datalen = 0; 
			}
			break;

		case NETCMD_NETWORK: 
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
//                system("reboot") ;
			}
			break;
		case NETCMD_REBOOT: 
			pCMD->datalen = 0;
			printf("System Reboot\n");
			system("reboot") ;
			break;

		case NETCMD_CONFIG_PARA: 
			printf("NETCMD_CONFIG_PARA param = %s length=%d \n",pCMD->data,pCMD->datalen);
			pCMD->datalen = 0;
			break;

		case NETCMD_SEND_ENVI_PARAM: 
			if(pCMD->status==SFLAG_READ)
			{
				SendCom1ReadReg(0x01,READ_REGS,ENVI_START_ADDR,ENVI_REG_MAX);//查询环境变量寄存器

/*				char * jsonPack=(char *)malloc(1024);
				memset(jsonPack,0,1024);
				int jsonPackLen=0;
				ENVI_PARAMS *pstrEnvPam=stuEnvi_Param;
				pstrEnvPam->temp=256;pstrEnvPam->moist=88;pstrEnvPam->air_cond_status=1;pstrEnvPam->air_cond_temp_out=352;
				jsonStrEvnWriter((char*)pstrEnvPam,jsonPack,&jsonPackLen);
				NetSendParm(NETCMD_SEND_ENVI_PARAM,jsonPack,jsonPackLen);
				free(jsonPack);*/
			}
			pCMD->datalen = 0;
			break;
		
		case NETCMD_SEND_UPS_PARAM: 
			if(pCMD->status==SFLAG_READ)
			{
				SendCom1ReadReg(0x01,READ_REGS,UPS_START_ADDR,UPS_REG_MAX);//查询UPS变量寄存器

/*				char * jsonPack=(char *)malloc(1024);
				memset(jsonPack,0,1024);
				int jsonPackLen=0;
				UPS_PARAMS *pstrUpsPam=stuUps_Param;
				pstrUpsPam->phase_num=1;pstrUpsPam->volt_Ain=220;pstrUpsPam->amp_Ain=2;pstrUpsPam->battery_tmp=25;
				jsonStrUpsWriter((char*)pstrUpsPam,jsonPack,&jsonPackLen);
				NetSendParm(NETCMD_SEND_UPS_PARAM,jsonPack,jsonPackLen);
				free(jsonPack);*/
			}
			pCMD->datalen = 0;
			break;
			
		case NETCMD_SEND_SPD_PARAM: 
			if(pCMD->status==SFLAG_READ)
			{
				SendCom1ReadReg(0x01,READ_REGS,SPD_START_ADDR,SPD_REG_MAX);//查询SPD变量寄存器
				
/*				char * jsonPack=(char *)malloc(1024);
				memset(jsonPack,0,1024);
				int jsonPackLen=0;
				SPD_PARAMS *pstrSpdPam=stuSpd_Param;
				pstrSpdPam->status=1;pstrSpdPam->grd_res=10;pstrSpdPam->struck_times=20;
				jsonStrSpdWriter((char*)pstrSpdPam,jsonPack,&jsonPackLen);
				NetSendParm(NETCMD_SEND_SPD_PARAM,jsonPack,jsonPackLen);
				free(jsonPack);*/
			}
			pCMD->datalen = 0;
			break;
		case NETCMD_SEND_DEV_PARAM: 
			if(pCMD->status==SFLAG_READ)
			{
				SendCom1ReadReg(0x01,READ_REGS,PARAMS_START_ADDR,PARAMS_REG_MAX);//查询装置参数寄存器
				
/*				char * jsonPack=(char *)malloc(1024);
				memset(jsonPack,0,1024);
				int jsonPackLen=0;
				DEVICE_PARAMS *pstrDevPam=stuDev_Param;
				pstrDevPam->Address=1;pstrDevPam->BaudRate_1=3;pstrDevPam->BaudRate_2=2;pstrDevPam->Pre_Remote=1;
				jsonStrDevWriter((char*)pstrDevPam,jsonPack,&jsonPackLen);
				NetSendParm(NETCMD_SEND_SPD_PARAM,jsonPack,jsonPackLen);
				free(jsonPack);*/
			}
			pCMD->datalen = 0;
			break;
		case NETCMD_SEND_DEV_INFO: 
			if(pCMD->status==SFLAG_READ)
			{
//				SendCom1ReadReg(0x01,READ_REGS,DEVICEINFO_START_ADDR,DEVICEINFO_REG_MAX);//查询装置信息寄存器

				char * jsonPack=(char *)malloc(1024);
				memset(jsonPack,0,1024);
				int jsonPackLen=0;
				DeviceInfoParams *pstrDevInfo=stuDev_Info;
				char s[40];sprintf(s,"广东利通科技 LTKJ-CONTR-101\0");
				for(int i=0;i<20;i++) {pstrDevInfo->deviceType[i]=(s[2*i]<<8)|(s[2*i+1]&0xff);}pstrDevInfo->softVersion=1;pstrDevInfo->softVersion=1;pstrDevInfo->protocolVersion=1;
				jsonStrDevInfoWriter((char*)pstrDevInfo,jsonPack,&jsonPackLen);
				NetSendParm(NETCMD_SEND_DEV_INFO,jsonPack,jsonPackLen);
				free(jsonPack);
			}
			pCMD->datalen = 0;
			break;
		case NETCMD_SEND_RSU_PARAM: 
			if(pCMD->status==SFLAG_READ)
			{
				SendCom1ReadReg(0x01,READ_REGS,RSU_START_ADDR,RSU_REG_MAX);//查询RSU天线参数寄存器
			}
			pCMD->datalen = 0;
			break;
		case NETCMD_SET_DEV_PARAM: 
			if(pCMD->status==SFLAG_WRITE)
			{
				DEVICE_PARAMS *pstuDevPam=stuDev_Param;
				UINT8 * pBuf=(UINT8 *)malloc(1024);
				memset(pBuf,0,1024);
				jsonstrDevReader((char *) pCMD->data,pCMD->datalen,(char *)pstuDevPam,pBuf);//将json字符串转换成结构体
				SendCom1WriteReg(0x01,PRESET_REGS,PARAMS_START_ADDR,PARAMS_REG_MAX,pBuf);//写装置参数寄存器
			}
			pCMD->datalen = 0;
			break;
		case NETCMD_REMOTE_CONTROL: 			//遥控设备
			if(pCMD->status==SFLAG_WRITE)
			{
				REMOTE_CONTROL *pRCtrl=stuRemote_Ctrl;
				memset(pRCtrl,0,sizeof(REMOTE_CONTROL));
				UINT8 * pBuf=(UINT8 *)malloc(1024);
				memset(pBuf,0,1024);
				jsonstrRCtrlReader((char *) pCMD->data,pCMD->datalen,(UINT8 *)pRCtrl);//将json字符串转换成结构体
				RemoteControl((UINT8*)pRCtrl);
			}
			pCMD->datalen = 0;
			break;
		default:
			break;

	}
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

 void WriteLog(char* str)
 {
	 printf("%s\n",str);
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
	REMOTE_CONTROL *pstuRCtrl=(REMOTE_CONTROL *)pRCtrl;
	
	if(pstuRCtrl->RSU1_PreClose==WRITE_ENABLE)			//RSU天线1遥合预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU1_PRECLOSE,WRITE_ENABLE);usleep(2000);}//delay 2ms
	if(pstuRCtrl->RSU1_Close==WRITE_ENABLE)						//RSU天线1遥合执行
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU1_CLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU1_PreOpen==WRITE_ENABLE)			//RSU天线1遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU1_PREOPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU1_Open==WRITE_ENABLE)						//RSU天线1遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU1_OPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU2_PreClose==WRITE_ENABLE)			//RSU天线2遥合预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU2_PRECLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU2_Close==WRITE_ENABLE)						//RSU天线2遥合执行
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU2_CLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU2_PreOpen==WRITE_ENABLE)			//RSU天线2遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU2_PREOPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU2_Open==WRITE_ENABLE)						//RSU天线2遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU2_OPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU3_PreClose==WRITE_ENABLE)			//RSU天线3遥合预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU3_PRECLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU3_Close==WRITE_ENABLE)						//RSU天线3遥合执行
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU3_CLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU3_PreOpen==WRITE_ENABLE)			//RSU天线3遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU3_PREOPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU3_Open==WRITE_ENABLE)						//RSU天线3遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU3_OPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU4_PreClose==WRITE_ENABLE)			//RSU天线4遥合预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU4_PRECLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU4_Close==WRITE_ENABLE)						//RSU天线4遥合执行
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU4_CLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU4_PreOpen==WRITE_ENABLE)			//RSU天线4遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU4_PREOPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU4_Open==WRITE_ENABLE)						//RSU天线4遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU4_OPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU5_PreClose==WRITE_ENABLE)			//RSU天线5遥合预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU5_PRECLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU5_Close==WRITE_ENABLE)						//RSU天线5遥合执行
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU5_CLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU5_PreOpen==WRITE_ENABLE)			//RSU天线5遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU5_PREOPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU5_Open==WRITE_ENABLE)						//RSU天线5遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU5_OPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU6_PreClose==WRITE_ENABLE)			//RSU天线6遥合预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU6_PRECLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU6_Close==WRITE_ENABLE)						//RSU天线6遥合执行
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU6_CLOSE,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU6_PreOpen==WRITE_ENABLE)			//RSU天线6遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU6_PREOPEN,WRITE_ENABLE);usleep(2000);}
	if(pstuRCtrl->RSU6_Open==WRITE_ENABLE)						//RSU天线6遥分预置
 		{SendCom1RCtlReg(0x01,FORCE_COIL,RSU6_OPEN,WRITE_ENABLE);usleep(2000);}
	  
	if(pstuRCtrl->SysReset==WRITE_ENABLE)					//系统重启 1548
 		{SendCom1RCtlReg(0x01,FORCE_COIL,SYSRESET,WRITE_ENABLE);usleep(2000);}
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
		printf("%s",jsonPack);
		HttpPostParm(StrServerURL1,jsonPack,jsonPackLen);
		
		sleep(10);
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
		printf("%s",jsonPack);
		HttpPostParm(StrServerURL2,jsonPack,jsonPackLen);
		NetSendParm(NETCMD_REMOTE_CONTROL,jsonPack,jsonPackLen);
		
		sleep(10);
	}
	free(jsonPack);
	return 0 ;
}

void init_XY_DataPost()
{
	pthread_t m_XY_DataPostthread ;
	pthread_create(&m_XY_DataPostthread,NULL,XY_DataPostthread,NULL);
}


