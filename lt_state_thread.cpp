#include <stdlib.h>
#include <linux/watchdog.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>  
#include <sys/ioctl.h>
#include <string>
#include <sys/time.h>
#include <csignal>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "lt_state_thread.h"

using namespace std;

char *infobuf;
extern VMCONTROL_STATE *stuVMCtl_State;	//控制器运行状态结构体
extern unsigned long GetTickCount();
extern void myprintf(char* str);

int ReadLTInfo(char *pbuf)
{
    FILE* Infofd ;
    if((Infofd=fopen("./ltinfo", "rb"))==NULL)
    {
        printf("read ltinfo erro\r\n");
        return 0;
    }
    else
    {
        fread(pbuf,1,1024*5,Infofd);
        fclose(Infofd);
        return 1;
    }

}

int getstrvaule(string &mstr,int mIndex)
{
   int pos = 0 ;
   string strret = "";
   //mstr = "Cpu(s):  0.2%us,  0.3%sy,  0.0%ni, 99.5%id,  0.0%wa,  0.0%hi,  0.0%si,  0.0%st";
   for(int i=0;i<mIndex;i++)
   {
      pos = mstr.find(" ");
      if(pos < 0)
      {
         //printf("return:%d\r\n",i);
         mstr = "";
         return i ;
      }
      strret = mstr.substr(0,pos) ;
      //printf("getmstr:%s\r\n",strret.c_str());
      mstr = mstr.substr(pos+1);
      
      pos = mstr.find_first_not_of(" ");
      if(pos > 0)
      {
        mstr = mstr.substr(pos) ;
        //printf("nextmstr:%s,pos:%d\r\n",mstr.c_str(),pos);
      }
   }
   mstr = strret ;
   //处理最后回车的值
   if(mstr.find("\r\n") == (mstr.size()-2))
   {
       mstr = mstr.substr(0,(mstr.size()-2)) ;
   }
   else if(mstr.find("\n") == (mstr.size()-1))
   {
       mstr = mstr.substr(0,(mstr.size()-1)) ;
   }
   return mIndex ;
}
int getstrvaule2(string &mstr,int mCols,int mIndex)
{
   int i,pos = 0 ;
   string strret = "";
   //mstr = "Cpu(s):  0.2%us,  0.3%sy,	0.0%ni, 99.5%id,  0.0%wa,  0.0%hi,	0.0%si,  0.0%st";
   for(i=0;i<mCols;i++)
   {
	  pos = mstr.find(" ");
	  if(pos < 0)
	  {
		 //printf("return:%d\r\n",i);
		 mstr = "";
		 return i;
	  }
	  if(i==mIndex)
	  {
		  strret = mstr.substr(0,pos) ;
		  //printf("getmstr:%s\r\n",strret.c_str());
	  }
	  mstr = mstr.substr(pos);
	  
	  pos = mstr.find_first_not_of(" ");
	  if(pos > 0)
	  {
		mstr = mstr.substr(pos) ;
		//printf("nextmstr:%s\r\n",mstr.c_str());
	  }
   }
   //printf("strret:%s\r\n",strret.c_str());
   mstr = strret ;
   //处理最后回车的值
   if(mstr.find("\r\n") == (mstr.size()-2))
   {
	   mstr = mstr.substr(0,(mstr.size()-2)) ;
   }
   else if(mstr.find("\n") == (mstr.size()-1))
   {
	   mstr = mstr.substr(0,(mstr.size()-1)) ;
   }
   return i ;
}



int getallstate(void)
{
    char strmsg[100],value[20];
	string strinfo = "";

	stuVMCtl_State->TimeStamp=GetTickCount();
	stuVMCtl_State->Linked=true;

    //查询系统名称
    system("hostname > ./ltinfo");
    memset(infobuf,0x00,1024*5+1) ;
    ReadLTInfo(infobuf);
    stuVMCtl_State->strhostname = infobuf;
    //处理最后回车的值
    if(stuVMCtl_State->strhostname.find("\r\n") == (stuVMCtl_State->strhostname.size()-2))
    {
        stuVMCtl_State->strhostname = stuVMCtl_State->strhostname.substr(0,(stuVMCtl_State->strhostname.size()-2)) ;
    }
    else if(stuVMCtl_State->strhostname.find("\n") == (stuVMCtl_State->strhostname.size()-1))
    {
        stuVMCtl_State->strhostname = stuVMCtl_State->strhostname.substr(0,(stuVMCtl_State->strhostname.size()-1)) ;
    }
    printf("系统名称:%s\r\n",stuVMCtl_State->strhostname.c_str());

    //查询CPU占用率
    system("top -bn1 | grep Cpu > ./ltinfo");
    memset(infobuf,0x00,1024*5+1) ;
    ReadLTInfo(infobuf);
    strinfo = infobuf;
    strinfo = strinfo + " ";
    if(getstrvaule2(strinfo,8,4) == 8)
       stuVMCtl_State->strcpuRate = strinfo ;
    else
       stuVMCtl_State->strcpuRate = "" ;
    if(stuVMCtl_State->strcpuRate.size() > 4)
    {
       stuVMCtl_State->strcpuRate = stuVMCtl_State->strcpuRate.substr(0,stuVMCtl_State->strcpuRate.size() - 4) ;
    }

    float fcpuRate = atof(stuVMCtl_State->strcpuRate.c_str());
    fcpuRate = 100.0 - fcpuRate;
	sprintf(value,"%.2f", fcpuRate);
    stuVMCtl_State->strcpuRate = value;
    printf("CPU占用率:%s%\r\n",stuVMCtl_State->strcpuRate.c_str());

    //查询CPU温度
//    system("npu-smi info -t temp -i 0 | grep LM75A_TE > ./ltinfo");
    system("cat /sys/class/thermal/thermal_zone0/temp > ./ltinfo");
    memset(infobuf,0x00,1024*5+1) ;
    ReadLTInfo(infobuf);
    strinfo = infobuf;
	sprintf(infobuf,"%.1f\0",atoi(infobuf)/1000.0);
	stuVMCtl_State->strcpuTemp=infobuf;   
    printf("CPU温度:%s\r\n",stuVMCtl_State->strcpuTemp.c_str());


    //查询内存大小
    system("free -m | grep Mem > ./ltinfo");
    memset(infobuf,0x00,1024*5+1) ;
    ReadLTInfo(infobuf);
    strinfo = infobuf;
    strinfo = strinfo + " ";
    if(getstrvaule(strinfo,2) == 2)
       stuVMCtl_State->strmenTotal = strinfo;
    else
       stuVMCtl_State->strmenTotal = "" ;
    printf("内存总大小%sM\r\n",stuVMCtl_State->strmenTotal.c_str());

    strinfo = infobuf;
    strinfo = strinfo + " ";
    if(getstrvaule(strinfo,3) == 3)
       stuVMCtl_State->strmenUsed = strinfo;
    else
       stuVMCtl_State->strmenUsed = "" ;
    printf("内存使用大小%sM\r\n",stuVMCtl_State->strmenUsed.c_str());

    float FmenTotal = (float)atoi(stuVMCtl_State->strmenTotal.c_str());
    float FmenUse = (float)atoi(stuVMCtl_State->strmenUsed.c_str());
    float FmenRate = (FmenUse/FmenTotal)*100 ;

    stringstream ss;
    ss<<FmenRate;
    stuVMCtl_State->strmenRate = ss.str();
    stuVMCtl_State->strmenRate = stuVMCtl_State->strmenRate;
    printf("内存占用率%s%\r\n",stuVMCtl_State->strmenRate.c_str());


	return 0 ;

}


void *LT_Statusthread(void *param)
 {
	infobuf = new char[1024*5+1];

	 string mStrdata;
	 string mstrkey = ""; //没有用户名和密码：则为“”；
	 while(1)
	 {
		 getallstate();
//		 sleep(300);
  	 	 sleep(20);
	 }

	 return 0 ;
 }

void init_lt_state_struct()
{
	//初始化
	stuVMCtl_State->TimeStamp=GetTickCount();
	stuVMCtl_State->Linked=false;
	
	stuVMCtl_State->strhostname = "";
	stuVMCtl_State->strcpuRate = "";
	stuVMCtl_State->strcpuTemp = "";
	stuVMCtl_State->strmenTotal = "";
	stuVMCtl_State->strmenUsed = "";
	stuVMCtl_State->strmenRate = "";
}

void init_lt_status()
{
	init_lt_state_struct();
	pthread_t m_LT_Statusthread ;
	pthread_create(&m_LT_Statusthread,NULL,LT_Statusthread,NULL);
}


/*int main(void)
{
   infobuf = new char[1024*5+1];
   getallstate();

   struct sockaddr_in addr;
   int addr_len =sizeof(struct sockaddr_in);
   int sockSrv;
   if((sockSrv = socket(AF_INET,SOCK_DGRAM,0))<0){
       perror("socket");
       exit(1);
   }

   addr.sin_family = AF_INET;
   addr.sin_port = htons(9902);
   addr.sin_addr.s_addr = htonl(INADDR_ANY);

   int nRecvBufLen = 8*1024;
   setsockopt(sockSrv, SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBufLen, sizeof( int ) );

   int nSendBufLen = 8*1024;
   setsockopt(sockSrv, SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );
   struct timeval timeout={8,0};
   setsockopt(sockSrv,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

   if (bind(sockSrv, (struct sockaddr *)&addr, sizeof(addr)) < 0)
   {
       printf("bind err\r\n");
       exit(1);
   }

   int DataLen = 0 ;
   char *recvBuf = new char[2049];
   string strrecv = "";
   string strJson = "";
   char sDateTime[30];
   string strDate = "";
   while(1)
   {
       DataLen = recvfrom(sockSrv,recvBuf,2048,0,(struct sockaddr *)&addr,(socklen_t*)&addr_len);
       if(DataLen > 0)
       {
           //recvBuf
           recvBuf[DataLen] = 0x00;
           strrecv = recvBuf ;
           if(strrecv == "{\"runstate\":\"1\"}")
           {
               getallstate();

              time_t nSeconds;
              struct tm * pTM;
              time(&nSeconds);
              pTM = localtime(&nSeconds);
              memset(sDateTime,0x00,30);
              sprintf(sDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
                      pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
                      pTM->tm_hour, pTM->tm_min, pTM->tm_sec);	

               strDate = sDateTime;

               strJson = "" ;
               strJson = strJson + "{\n";
               strJson = strJson + "\"hostname\":\""+ strhostname +"\",\n"; //strhostname
               strJson = strJson + "\"cpurate\":\""+ strcpuRate +"\",\n"; //strcpuRate
               strJson = strJson + "\"cputemp\":\""+ strcpuTemp +"\",\n"; //strcpuTemp
               strJson = strJson + "\"mentotal\":\""+ strmenTotal +"\",\n"; //strmenTotal
               strJson = strJson + "\"menused\":\""+ strmenUsed +"\",\n"; //strmenUsed
               strJson = strJson + "\"menrate\":\""+ strmenRate +"\",\n"; //strmenRate
               strJson = strJson + strmmcjsonState + ",\n";
               strJson = strJson + strdiskjsonState + ",\n";
               strJson = strJson + streth0jsonState + ",\n";
               strJson = strJson + streth1jsonState + ",\n";
               strJson = strJson + "\"datetime\":\""+ strDate +"\"\n"; //datetime
               strJson = strJson + "}\n";

               sendto(sockSrv,(char *)strJson.c_str(),strJson.size(),0,(struct sockaddr *)&addr,sizeof(struct sockaddr_in));

             }

          }
          else
          {
           usleep(2000000);
          }

   }


    return 0;
}*/





