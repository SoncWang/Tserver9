#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "AtlasServer.h"

using namespace std;

int AtlassockSrv;
struct sockaddr_in AtlasAddr;
pthread_mutex_t atlasdataMutex ;

int AtlassockSrv1;
struct sockaddr_in AtlasAddr1;
pthread_mutex_t atlasdataMutex1 ;

extern ATLAS_STATE stuAtlasState[ATLAS_NUM]; //Atlas状态
extern string StrAtlasCount;	//Atlas数量
extern string StrAtlasIP[ATLAS_NUM] ;//AtlasIP
extern string StrAtlasPasswd[ATLAS_NUM] ;//Atlas密码
extern unsigned long GetTickCount(); //返回毫秒

int GetAtlasData(void)
{
   string mstrsend = "{\"runstate\":\"1\"}";
   pthread_mutex_lock(&atlasdataMutex);             
   sendto(AtlassockSrv,(char *)mstrsend.c_str(),mstrsend.size(),0,(struct sockaddr *)&AtlasAddr,sizeof(struct sockaddr_in));
   pthread_mutex_unlock(&atlasdataMutex);
   return 0 ;
}

int GetAtlasData1(void)
{
   string mstrsend = "{\"runstate\":\"1\"}";
   pthread_mutex_lock(&atlasdataMutex1);
   sendto(AtlassockSrv1,(char *)mstrsend.c_str(),mstrsend.size(),0,(struct sockaddr *)&AtlasAddr1,sizeof(struct sockaddr_in));
   pthread_mutex_unlock(&atlasdataMutex1);
   return 0 ;
}


void *AtlasSendthread(void *param)
{
  sleep(10);
  while(1)
  {
     //发送获取
     GetAtlasData();
     sleep(60*5);

  }
  
}

void *AtlasSendthread1(void *param)
{
  sleep(10);
  while(1)
  {
     //发送获取
     GetAtlasData1();
     sleep(60*5);

  }

}



void *AtlasRecvthread(void *param)
{ 
    int addr_len =sizeof(struct sockaddr_in);
    if((AtlassockSrv = socket(AF_INET,SOCK_DGRAM,0))<0){  
        perror("socket");  
        exit(1);  
    }  

    int nRecvBufLen = 8*1024; 
    setsockopt(AtlassockSrv, SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBufLen, sizeof( int ) );

    int nSendBufLen = 8*1024; 
    setsockopt(AtlassockSrv, SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );
    struct timeval timeout={8,0};//3s
    setsockopt(AtlassockSrv,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
	
	printf("StrAtlasIP[0]:%s\r\n",StrAtlasIP[0].c_str());	
	
    bzero(&AtlasAddr,sizeof(AtlasAddr));  
    AtlasAddr.sin_family = AF_INET;  
    AtlasAddr.sin_addr.s_addr = inet_addr(StrAtlasIP[0].c_str());  
    AtlasAddr.sin_port = htons(9902);  

    struct sockaddr_in Recvaddr = AtlasAddr;

    int DataLen  ;
    char *recvBuf = new char[1024*11];  
    while(1)
    { 
        memset(recvBuf,0,10240);
        DataLen = recvfrom(AtlassockSrv,recvBuf,10240,0,(struct sockaddr *)&Recvaddr,(socklen_t*)&addr_len);
        if(DataLen > 0)
        {   
           pthread_mutex_lock(&atlasdataMutex);             
		   stuAtlasState[0].TimeStamp=GetTickCount();
		   stuAtlasState[0].Linked=true;
		   stuAtlasState[0].stratlasdata = recvBuf;
           printf("atlasdata:%s\r\n",stuAtlasState[0].stratlasdata.c_str());
           pthread_mutex_unlock(&atlasdataMutex);
        }
			  
    }
    close(AtlassockSrv);  

}



void *AtlasRecvthread1(void *param)
{
    int addr_len =sizeof(struct sockaddr_in);
    if((AtlassockSrv1 = socket(AF_INET,SOCK_DGRAM,0))<0){
        perror("socket");
        exit(1);
    }

    int nRecvBufLen = 8*1024;
    setsockopt(AtlassockSrv1, SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBufLen, sizeof( int ) );

    int nSendBufLen = 8*1024;
    setsockopt(AtlassockSrv1, SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );
    struct timeval timeout={8,0};//3s
    setsockopt(AtlassockSrv1,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
	
	printf("StrAtlasIP[1]:%s\r\n",StrAtlasIP[1].c_str());

    bzero(&AtlasAddr1,sizeof(AtlasAddr1));
    AtlasAddr1.sin_family = AF_INET;
    AtlasAddr1.sin_addr.s_addr = inet_addr(StrAtlasIP[1].c_str());
    AtlasAddr1.sin_port = htons(9902);

    struct sockaddr_in Recvaddr = AtlasAddr1;

    int DataLen  ;
    char *recvBuf = new char[1024*11];
    while(1)
    {
        memset(recvBuf,0,10240);
        DataLen = recvfrom(AtlassockSrv1,recvBuf,10240,0,(struct sockaddr *)&Recvaddr,(socklen_t*)&addr_len);
        if(DataLen > 0)
        {
           pthread_mutex_lock(&atlasdataMutex1);
		   stuAtlasState[1].TimeStamp=GetTickCount();
		   stuAtlasState[1].Linked=true;
		   stuAtlasState[1].stratlasdata = recvBuf;
           printf("atlasdata1:%s\r\n",stuAtlasState[1].stratlasdata.c_str());
           pthread_mutex_unlock(&atlasdataMutex1);
        }

    }
    close(AtlassockSrv1);

}


void initTeststr()
{
/*	stratlasdata="{\n";
	stratlasdata+="\"hostname\":\"Euler\",\n";
	stratlasdata+="\"cpurate\":\"26.6%\",\n";
	stratlasdata+="\"cputemp\":\"58\",\n";
	stratlasdata+="\"mentotal\":\"3868M\",\n";
	stratlasdata+="\"menused\":\"299M\",\n";
	stratlasdata+="\"menrate\":\"7.73009%\",\n";
	stratlasdata+="\"mmccount\":\"5\",\n";
	stratlasdata+="\"mmclist\":[\n";
	stratlasdata+="{\n";
	stratlasdata+="\"mmcname\":\"/\",\n";
	stratlasdata+="\"mmcsize\":\"1.9G\",\n";
	stratlasdata+="\"mmcrate\":\"40%\"\n";
	stratlasdata+="},\n";
	stratlasdata+="{\n";
	stratlasdata+="\"mmcname\":\"/home/log\",\n";
	stratlasdata+="\"mmcsize\":\"946M\",\n";
	stratlasdata+="\"mmcrate\":\"1%\"\n";
	stratlasdata+="},\n";
	stratlasdata+="{\n";
	stratlasdata+="\"mmcname\":\"/var/lib/docker\",\n";
	stratlasdata+="\"mmcsize\":\"12G\",\n";
	stratlasdata+="\"mmcrate\":\"1%\"\n";
	stratlasdata+="},\n";
	stratlasdata+="{\n";
	stratlasdata+="\"mmcname\":\"/home/data\",\n";
	stratlasdata+="\"mmcsize\":\"945M\",\n";
	stratlasdata+="\"mmcrate\":\"36%\"\n";
	stratlasdata+="},\n";
	stratlasdata+="{\n";
	stratlasdata+="\"mmcname\":\"/opt\",\n";
	stratlasdata+="\"mmcsize\":\"1.9G\",\n";
	stratlasdata+="\"mmcrate\":\"4%\"\n";
	stratlasdata+="}\n";
	stratlasdata+="],\n";
	stratlasdata+="\"diskcount\":\"0\",\n";
	stratlasdata+="\"disklist\":[\n";
	stratlasdata+="],\n";
	stratlasdata+="\"eth0count\":\"2\",\n";
	stratlasdata+="\"eth0list\":[\n";
	stratlasdata+="{\n";
	stratlasdata+="\"ip\":\"192.168.2.111\",\n";
	stratlasdata+="\"netmask\":\"255.255.255.0\"\n";
	stratlasdata+="},\n";
	stratlasdata+="{\n";
	stratlasdata+="\"ip\":\"192.168.130.4\",\n";
	stratlasdata+="\"netmask\":\"255.255.255.0\"\n";
	stratlasdata+="}\n";
	stratlasdata+="],\n";
	stratlasdata+="\"eth1count\":\"0\",\n";
	stratlasdata+="\"eth1list\":[\n";
	stratlasdata+="],\n";
	stratlasdata+="\"datetime\":\"2019-10-22 19:57:05\"\n";
	stratlasdata+="}\n";

	stratlasdata1=stratlasdata;*/
}

void init_atlas_struct(int atlasno)
{
	//初始化
	stuAtlasState[atlasno].TimeStamp=GetTickCount();
	stuAtlasState[atlasno].Linked=false;
	
	stuAtlasState[atlasno].stratlasdata = "";
}

int AtlasInit(void)
{
	//initTeststr();
	int i;
	for(i=0;i<ATLAS_NUM;i++)
		init_atlas_struct(i);
	
	pthread_mutex_init(&atlasdataMutex,NULL);
	pthread_mutex_init(&atlasdataMutex1,NULL);

	pthread_t m_AtlasRecvthread ;
	pthread_create(&m_AtlasRecvthread,NULL,AtlasRecvthread,NULL);

	pthread_t m_AtlasSendthread ;
	pthread_create(&m_AtlasSendthread,NULL,AtlasSendthread,NULL);

	pthread_t m_AtlasRecvthread1 ;
	pthread_create(&m_AtlasRecvthread1,NULL,AtlasRecvthread1,NULL);

	pthread_t m_AtlasSendthread1 ;
	pthread_create(&m_AtlasSendthread1,NULL,AtlasSendthread1,NULL);


   return 0 ;
}



