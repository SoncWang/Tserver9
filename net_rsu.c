#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h> // fork, close
#include <arpa/inet.h>  // inet_ntoa
#include <pthread.h>
#include <netinet/tcp.h>
#include <linux/rtc.h>
#include <ctype.h>
#include <stdbool.h>
#include "net_rsu.h"
#define NETWORK_FILE "/opt/config"
#define CRC_16_POLYNOMIALS 0x8408
int num;//输入的数据
int rsctl_all=0x10;
static int sockfd_rsu; 
//extern std::string StrRSUIP;	//RSUIP地址
//extern std::string StrRSUPort;	//RSU端口
extern RSUCONTROLER *stuRsuControl;	//RSU控制器状态
extern char gsRSUIP[20];	//RSUIP地址
extern char gsRSUPort[10];	//RSU端口

static void* NetWork_server_thread_RSU(void *arg);
void myprintf(char* str);
//***B9（心跳包）数据帧解析****

//************B0（设备状态）数据帧解析******************
//**********************D1（设备重启）数据帧解析***************
unsigned short CRC16_pc(unsigned char* pchMsg, unsigned short wDataLen) // 1. MSB first; 2. Initial value is 0000;
{
    unsigned char i, chChar;
    unsigned short wCRC = 0xffff;
	printf("crc_data is:");
    while (wDataLen--)
    {
        chChar = *pchMsg++;  
	printf("%x-",chChar);      
        wCRC ^= ((unsigned short) chChar);
        for (i = 0; i < 8; i++)
        {
            if (wCRC & 0x01)
                wCRC = (wCRC >>1) ^ CRC_16_POLYNOMIALS;
            else
                wCRC >>= 1;
        }
	

    }
	wCRC = (~wCRC) & 0xFFFF;
    printf("crc is %x\n",wCRC);
    return wCRC;
	
}

void send_RSU(char command,bool ReSend,char state,int num)
{
	unsigned char send_buff[128];
	memset(send_buff,0,sizeof(send_buff));
	char buff_len = 0;
	int i;
	unsigned short crc;
	//----处理帧序号-------
	if(!ReSend)
	{
		if(rsctl_all == 0)
		{
			rsctl_all = 0x10;
		}
		else
		{
			rsctl_all += 0x10;
			if(rsctl_all > 0x90)
			{
				rsctl_all = 0x10; 
			}
		}		
	}
	//--------------------- 
	send_buff[0] = 0xff;
	send_buff[1] = 0xff;
	send_buff[2] = 0x00;
	send_buff[3] = rsctl_all;	
	switch(command)
	{
		case 0xC4:				   //打开，关闭复位天线指令
		send_buff[4] = 0x00;		//数据长度四个字节
		send_buff[5] = 0x00;
		send_buff[6] = 0x00;
		send_buff[7] = 3;
		send_buff[8] = 0xc4;
		if(state==0x01)
		{
			send_buff[9] = 0x01;	//打开天线
		}  
		else if(state==0x00)
		{	
			send_buff[9] = 0x00;	//关闭天线
		}  
		send_buff[10]=num;		//对哪台天线进行操作
		buff_len=11;
		break;
		case 0xC0:		   //获取天线配置指令
		send_buff[4] = 0x00;		//数据长度四个字节
		send_buff[5] = 0x00;
		send_buff[6] = 0x00;
		send_buff[7] = 18;
		send_buff[8] = 0xC0;
		for(i=0;i<17;i++)
		{
			send_buff[9+i] = 0x00;
		}
		buff_len=26;
		break;
		case 0x1D:
		send_buff[4] = 0x00;		//数据长度四个字节
		send_buff[5] = 0x00;
		send_buff[6] = 0x00;
		send_buff[7] = 2;
		send_buff[8] = 0x1D;
		send_buff[9] = num;
		buff_len=10;
		break;
	}
	/*for(i=2;i<buff_len;i++)
	{
		crc=crc_ccitt_byte(crc,send_buff[i]);
	}*/
	crc=CRC16_pc(send_buff+2,buff_len-2);
	//crc ^= 0xffff;
	send_buff[buff_len] = ((crc&0xff00)>>8);
	send_buff[buff_len+1] = (crc&0x00ff);
	buff_len=buff_len+2;
	printf("sent_rsu data:");
	for(i=0;i<buff_len;i++)
	{
		printf("%x-",send_buff[i]);
	}
	printf("\n");
	write(sockfd_rsu,send_buff,buff_len);
}
static char* getNetworkInfo(char *maches)	//读取配置文件的内容
{
	char szBuf[256];
	char *szNetwork=NULL;
	int i = 0;
	FILE *fp = NULL;
	if((fp=fopen(NETWORK_FILE, "r"))==NULL)             //判断文件是否为空
   	{
        	printf( "Can 't   open   file!\n"); 
		return 0;
    	}
	while(fgets(szBuf,128,fp))                         //从文件开关开始向下读，把读到的内容放到szBuf中
    	{                         
		if(strstr(szBuf,maches) != NULL)                 //找到maches在文件中第一次出现的位置。。如address 
        	{
			for(i =0;i < strlen(szBuf);i++)
          		{              
				if(isdigit(*(szBuf+i)))                      //从szBuf字符串中找出数字。
                		{
                    			szNetwork = (char*)malloc(strlen(szBuf));  //为szNetwork分配内存
                    			strcpy(szNetwork,szBuf+i);             
                    			szNetwork[strlen(szNetwork)-1] = '\0';
                    			fclose(fp);
					return szNetwork;
                		}
            		}
        	}
		else
		continue;
    	}
    	fclose(fp);
	return szNetwork;
}
void init_net_rsu()
{
	pthread_t tNetwork_server_RSU;
	if (pthread_create(&tNetwork_server_RSU, NULL, NetWork_server_thread_RSU,NULL)) 
	{
		printf("NetWork server create failed!\n");
	}
	pthread_detach(tNetwork_server_RSU);
}
static void* NetWork_server_thread_RSU(void*arg)//接收天线数据线程
{
	int i,j,temp;
	const char *IPaddress;
	const char * IPport;
	int port,nlen;
	char buf[128];
	struct sockaddr_in server_addr; 
	memset(buf,0,sizeof(buf));
//	struct FlagRunStatus_struct RSU_data;

//	struct control_S data1;
	struct RSU_data_init data2;
	struct RSU_reset data3;
	//struct AntennaInfoN[8] data2[8];
//	memset(&data1,0,sizeof(data1));
	if( (sockfd_rsu = socket(AF_INET, SOCK_STREAM, 0)) == -1 )  
	{   
    	myprintf ("ERROR: Failed to obtain RSU Socket Despcritor.\n");
    	return (0);
	} else {
    	myprintf ("OK: Obtain RSU Socket Despcritor sucessfully.\n");
	}
//	IPaddress = getNetworkInfo("RSUIP");//获取配置文件中的IP地址
//	IPport=getNetworkInfo("RSUPort");
	IPaddress = gsRSUIP;//获取配置文件中的IP地址
	IPport=gsRSUPort;
	port=atoi(IPport);
	/* Fill the local socket address struct */
	server_addr.sin_family = AF_INET;           		// Protocol Family
	server_addr.sin_port = htons (port);         		// Port number
	server_addr.sin_addr.s_addr  = inet_addr (IPaddress);  	// AutoFill local address
	memset (server_addr.sin_zero,0,8);          		// Flush the rest of struct
//侯林汝写的侦听服务器-天线的数据代码
	if (connect(sockfd_rsu, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
		myprintf("connect to RSU server error!\n");
	}
	else
	{
		myprintf("connect to RSU server sucess!\n");
	while(1)
	{
		memset(buf,0,sizeof(buf));
		nlen = read(sockfd_rsu, buf, sizeof(buf)-1);
		if (nlen == -1)
		{
			printf("read message error\n");
		}
		else
		{
			printf("read data from the server:");//接收到B9帧数据，解析第十一字节（天线数量）十二字节*4（天线状态）				
			for(i=0;i<nlen;i++)
			{
				printf("%x-",buf[i]);
			}
			printf("\n");
			if(buf[8]==0xb9)
			{	
				stuRsuControl->ControlCount=buf[17]; 	//控制器数量
				
				temp=stuRsuControl->ControlCount;
				if(temp>8)
				{
					printf("RSU ControlCount = %d error!\n",temp);
					continue;
				}
				for(i=0;i<temp;i++)
				{stuRsuControl->ControlStatusN[i]=buf[18+i];}		//控制器状态
				stuRsuControl->AntennaCount=buf[18+temp];	//天线数量
				if(stuRsuControl->AntennaCount>8)
				{
					printf("RSU AntennaCount = %d error!\n",stuRsuControl->AntennaCount);
					continue;
				}
				for(j=0;j<stuRsuControl->AntennaCount;j++)
				{
					stuRsuControl->AntennaInfoN[j].Status=buf[19+temp+j*4];
					stuRsuControl->AntennaInfoN[j].Power=buf[20+temp+j*4];;
					stuRsuControl->AntennaInfoN[j].Channel=buf[21+temp+j*4];;
					stuRsuControl->AntennaInfoN[j].Control_state=buf[22+temp+j*4];
				}
				printf("ControlCount=%d\t\n",stuRsuControl->ControlCount);
				for(i=0;i<stuRsuControl->ControlCount;i++)
					printf("Control %d ControlStatusN=%d\t\n",i,stuRsuControl->ControlStatusN[i]);
				for(j=0;j<stuRsuControl->AntennaCount;j++)
					printf("Antenna %d Status=%d  Power=%d  Channel=%d  Control_state=%d\t\n",j,stuRsuControl->AntennaInfoN[j].Status,stuRsuControl->AntennaInfoN[j].Power,stuRsuControl->AntennaInfoN[j].Channel,stuRsuControl->AntennaInfoN[j].Control_state);
			}
			else if(buf[8]==0xb0)
			{
				data2.RSUManuID=buf[10];
				for(j=0;j<3;j++)
				{
				data2.RSUID[i]=buf[11+i];}
				data2.RSUVersion[0]=buf[14];
				data2.RSUVersion[1]=buf[15];
				data2.ControlId=buf[16];
				data2.PSAMCount=buf[17];
				for(j=0;j<data2.PSAMCount;j++)
				{
					for(i=0;i<8;i++)
						{data2.PSAMInfoN[j].Psam_ID[i]=buf[18+j+i];}
				}
			}
			else if(buf[8]==0xd1)
			{
				data3.AntennaCount=buf[9];
				data3.RSUState=buf[10];
			}
		}
		sleep(2);
	}
}	
}// 接收天线数据结束



