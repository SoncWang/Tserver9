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
extern RSUCONTROLER stuRsuControl;	//RSU控制器状态
extern RSU_DATA_INIT stuRsuData[RSUCTL_NUM];	//RSU设备信息结构体
extern RSU_RESET stuRsuReset;			//天线软件复位状态结构体
extern int gRSUCount;	//RSU数量
extern char gsRSUIP[RSUCTL_NUM][20];	//RSUIP地址
extern char gsRSUPort[RSUCTL_NUM][10];	//RSU端口
static int sockfd_rsu; 
static void* NetWork_server_thread_RSU(void *arg);
static void* NetWork_server_thread_RSU2(void *arg);
void myprintf(char* str);
extern void WriteLog(char* str);
//***********************10.15路网中心协议修改后的版本*************************
unsigned short CRC16_pc(unsigned char* pchMsg, unsigned short wDataLen) // 1. MSB first; 2. Initial value is 0000;
{
    unsigned char i, chChar;
    unsigned short wCRC = 0xffff;
    while (wDataLen--)
    {
        chChar = *pchMsg++;      
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
    return wCRC;
	
}

void send_RSU(char command,bool ReSend,char state,int number)
{
	unsigned char send_buff[128];
	int rsctl_all=0;	
	memset(send_buff,0,sizeof(send_buff));
	int buff_len = 0;
	int i;
	unsigned short crc;
	//----处理帧序号-------
	if(!ReSend)
	{
		if(rsctl_all == 0)
		{
			rsctl_all = 0x00;
		}
		else
		{
			rsctl_all += 0x10;
			if(rsctl_all > 0x90)
			{
				rsctl_all = 0x00; 
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
		send_buff[10]=number;		//对哪台天线进行操作
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
		send_buff[9] = number;
		buff_len=10;
		break;
	}
	crc=CRC16_pc(send_buff+2,buff_len-2);
	send_buff[buff_len] = ((crc&0xff00)>>8);
	send_buff[buff_len+1] = (crc&0x00ff);
	buff_len=buff_len+2;
	//sprintf(str,"sent_rsu data:");
	printf("sent_rsu data:");
	for(i=0;i<buff_len;i++)
	{
		printf("%x-",send_buff[i]);
		
	}
	//sprintf(str,"\n");
	printf("\n");
	write(sockfd_rsu,send_buff,buff_len+2);
}

void init_net_rsu()
{
	pthread_t tNetwork_server_RSU;
	if (pthread_create(&tNetwork_server_RSU, NULL, NetWork_server_thread_RSU,NULL)) 
	{
		printf("NetWork_server_thread_RSU create failed!\n");
	}
	pthread_detach(tNetwork_server_RSU);

	if(gRSUCount==2)
	{
		pthread_t tNetwork_server_RSU2;
		if (pthread_create(&tNetwork_server_RSU2, NULL, NetWork_server_thread_RSU2,NULL)) 
		{
			printf("NetWork_server_thread_RSU2 create failed!\n");
		}
		pthread_detach(tNetwork_server_RSU2);
	}
}
int obtain_net(char *sip,char *sport)
{
	const char *IPaddress;
	const char * IPport;
	int port;
	struct sockaddr_in server_addr; 
	if( (sockfd_rsu = socket(AF_INET, SOCK_STREAM, 0)) == -1 )  
	{   
    		printf ("ERROR: Failed to obtain RSU Socket Despcritor.\n");
    		return (0);
	} 
	else 
	{
    		printf ("OK: Obtain RSU Socket Despcritor sucessfully.\n");
	}
//	IPaddress = gsRSUIP[0];//获取配置文件中的IP地址
//	IPport=gsRSUPort[0];
	IPaddress = sip;//获取配置文件中的IP地址
	IPport=sport;
	port=atoi(IPport);
	/* Fill the local socket address struct */
	server_addr.sin_family = AF_INET;           		// Protocol Family
	server_addr.sin_port = htons (port);         		// Port number
	server_addr.sin_addr.s_addr  = inet_addr (IPaddress);  	// AutoFill local address
	memset (server_addr.sin_zero,0,8);          		// Flush the rest of struct
//侯林汝写的侦听服务器-天线的数据代码
	printf("IPaddress=%s,IPport=%s\n",IPaddress,IPport);
	if (connect(sockfd_rsu, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
   	 {
		printf("connect to RSU server error!\n");
		close(sockfd_rsu);
		return (0);
	}
	else
	{
		printf("connect to RSU server sucess!\n");
		return(1);
	}
}
static void* NetWork_server_thread_RSU(void*arg)//接收天线数据线程
{
	char str[1024];
	int i,j,m,n,h,t;
	int nlen,temp;
	unsigned char buf[128];
	memset(buf,0,sizeof(buf));
	unsigned short crc_rsu;
	int bFlag=0;
	temp=0;
	while(temp==0)
	{
		temp=obtain_net(gsRSUIP[0],gsRSUPort[0]);
		printf("temp=%d\n",temp);
		if(temp==0)		
		{printf("connect _rsu error\n");
		sleep(2);}
		else
		{
			send_RSU(0xc0,false,0,1);
		}
	}
	while(1)
		{
			nlen = read(sockfd_rsu, buf, sizeof(buf)-1);
    			if (nlen <=0)
        		{
				while(temp==0)
				{
					printf("nlen is %d connect _rsu error\n",nlen);
					temp=obtain_net(gsRSUIP[0],gsRSUPort[0]);
				}	
			}
			else
			{
				printf("read %d byte data from the server:",nlen);//接收数据，解析第十一字节（天线数量）十二字节*4（天线状态）				
				for(i=0;i<nlen;i++)
				{
					printf("%x-",buf[i]);
				}
				printf("\n");
				crc_rsu=CRC16_pc(buf+2,nlen-4);
				if(buf[8]==0xb1&&buf[nlen-2]==((crc_rsu&0xff00)>>8)&&buf[nlen-1]==(crc_rsu&0x00ff)&&buf[0]==0xff)
				{
					stuRsuControl.ControlStatus_1=buf[9];
					stuRsuControl.ControlStatus_2=buf[10];
					stuRsuControl.ControlStatus_3=buf[11];
					printf("stuRsuControl.ControlStatus_3 is %x\n",stuRsuControl.ControlStatus_3);
					stuRsuControl.Psam_num1=buf[12];
					printf("Psam_num1 is %x\n",stuRsuControl.Psam_num1);
					n=buf[12];
					for(i=0;i<n;i++)
					{
						stuRsuControl.Psam1[i].Psam1_channel=buf[13+i*3];
						stuRsuControl.Psam1[i].Psam1_status=buf[14+i*3];
						stuRsuControl.Psam1[i].Psam1_auth=buf[15+i*3];//增加了授权状态
					}
					stuRsuControl.Psam_num2=buf[13+3*n];
					printf("Psam_num2 is %x\n",stuRsuControl.Psam_num2);
					m=buf[13+3*n];
					for(i=0;i<m;i++)
					{
						stuRsuControl.Psam2[i].Psam2_channel=buf[14+3*n+i*3];
						stuRsuControl.Psam2[i].Psam2_status=buf[15+3*n+i*3];
						stuRsuControl.Psam2[i].Psam2_auth=buf[16+3*n+i*3];
					}
					stuRsuControl.AntennaCount=buf[14+3*n+3*m];		//天线数量
					stuRsuControl.AntennaCount2=buf[15+3*n+3*m];	//正常工作天线数量
					printf("全部 Antenna num is %x\n",stuRsuControl.AntennaCount);
					printf("正常工作 Antenna num is %x\n",stuRsuControl.AntennaCount2);
					h=buf[14+3*n+3*m];
					for(i=0;i<h;i++)
					{
						stuRsuControl.AntennaInfoN[i].Rsu_id=buf[16+3*n+3*m+i*4];
						stuRsuControl.AntennaInfoN[i].Control_state=buf[17+3*n+3*m+i*4];
						stuRsuControl.AntennaInfoN[i].Channel=buf[18+3*n+3*m+i*4];
						stuRsuControl.AntennaInfoN[i].Power=buf[19+3*n+3*m+i*4];
						//stuRsuControl.AntennaInfoN[i].send_status=buf[19+2*n+2*m+i*6];
						//stuRsuControl.AntennaInfoN[i].recv_status=buf[20+2*n+2*m+i*6];
					}
				}
				if(buf[8]==0xb0&&buf[nlen-2]==((crc_rsu&0xff00)>>8)&&buf[nlen-1]==(crc_rsu&0x00ff)&&buf[0]==0xff)
				{
					stuRsuData[0].RSUStatus=buf[9];
					stuRsuData[0].PSAMCount=buf[10];
					printf("B0_psam num is %x\n",stuRsuData[0].PSAMCount);
					t=buf[10];
					for(i=0;i<t;i++)
					{
						stuRsuData[0].PSAMInfoN[i].PSAM_channel=buf[11+9*i];	
						stuRsuData[0].PSAMInfoN[i].PSAM_version=buf[12+9*i];
						stuRsuData[0].PSAMInfoN[i].PSAM_auth=buf[13+9*i];
						printf("psam_ID is:");
						for(j=0;j<6;j++)
						{
							stuRsuData[0].PSAMInfoN[i].Psam_ID[j]=buf[14+9*i+j];
							printf(" %x-",stuRsuData[0].PSAMInfoN[i].Psam_ID[j]);
						}
						printf("\n");
					}
					stuRsuData[0].RSUAlgId=buf[11+9*t];
					stuRsuData[0].RSUManuID=buf[12+9*t];
					printf("RSUManuID is %x\n",stuRsuData[0].RSUManuID);
					for(j=0;j<3;j++)
					{stuRsuData[0].RSUID[j]=buf[13+9*t+j];}
					for(j=0;j<2;j++)
					{stuRsuData[0].RSUVersion[j]=buf[16+9*t+j];}
					stuRsuData[0].Workstatus=buf[18+9*t];
					for(j=0;j<3;j++)
					{stuRsuData[0].FlagID[j]=buf[19+9*t+j];}
					for(j=0;j<4;j++)
					{stuRsuData[0].Reserved[j]=buf[22+9*t+j];}
				}
				if(buf[8]==0xd1&&buf[nlen-2]==((crc_rsu&0xff00)>>8)&&buf[nlen-1]==(crc_rsu&0x00ff)&&buf[0]==0xff)
				{
					stuRsuReset.AntennaCount=buf[9];
					stuRsuReset.RSUState=buf[10];
				}
			}
		}
	sleep(2);
}// 接收天线数据结束

static void* NetWork_server_thread_RSU2(void*arg)//接收天线数据线程
{
	char str[1024];
	int i,j,m,n,h,t;
	int nlen,temp;
	unsigned char buf[128];
	memset(buf,0,sizeof(buf));
	unsigned short crc_rsu;
	int bFlag=0;
	temp=0;
	while(temp==0)
	{
		temp=obtain_net(gsRSUIP[1],gsRSUPort[1]);
		printf("RSU2 temp=%d\n",temp);
		if(temp==0)		
		{printf("RSU2 connect _rsu error\n");
		sleep(2);}
		else
		{
			send_RSU(0xc0,false,0,1);
		}
	}
	while(1)
		{
			nlen = read(sockfd_rsu, buf, sizeof(buf)-1);
    			if (nlen <=0)
        		{
				while(temp==0)
				{
					printf("RSU2 nlen is %d connect _rsu error\n",nlen);
					temp=obtain_net(gsRSUIP[1],gsRSUPort[1]);
				}	
			}
			else
			{
				printf("RSU2 read %d byte data from the server:",nlen);//接收数据，解析第十一字节（天线数量）十二字节*4（天线状态）				
				for(i=0;i<nlen;i++)
				{
					printf("%x-",buf[i]);
				}
				printf("\n");
				crc_rsu=CRC16_pc(buf+2,nlen-4);
				if(buf[8]==0xb1&&buf[nlen-2]==((crc_rsu&0xff00)>>8)&&buf[nlen-1]==(crc_rsu&0x00ff)&&buf[0]==0xff)
				{
					stuRsuControl.ControlStatus_1=buf[9];
					stuRsuControl.ControlStatus_2=buf[10];
					stuRsuControl.ControlStatus_3=buf[11];
					printf("RSU2 stuRsuControl.ControlStatus_3 is %x\n",stuRsuControl.ControlStatus_3);
					stuRsuControl.Psam_num1=buf[12];
					printf("RSU2 Psam_num1 is %x\n",stuRsuControl.Psam_num1);
					n=buf[12];
					for(i=0;i<n;i++)
					{
						stuRsuControl.Psam1[i].Psam1_channel=buf[13+i*2];
						stuRsuControl.Psam1[i].Psam1_status=buf[14+i*2];
					}
					stuRsuControl.Psam_num2=buf[13+2*n];
					printf("RSU2 Psam_num2 is %x\n",stuRsuControl.Psam_num2);
					m=buf[13+2*n];
					for(i=0;i<m;i++)
					{
						stuRsuControl.Psam2[i].Psam2_channel=buf[14+2*n+i*2];
						stuRsuControl.Psam2[i].Psam2_status=buf[15+2*n+i*2];
					}
					stuRsuControl.AntennaCount=buf[14+2*n+2*m];
					printf("RSU2 Antenna num is %x\n",stuRsuControl.AntennaCount);
					h=buf[14+2*n+2*m];
					for(i=0;i<h;i++)
					{
						stuRsuControl.AntennaInfoN[i].Rsu_id=buf[15+2*n+2*m+i*6];
						stuRsuControl.AntennaInfoN[i].Control_state=buf[16+2*n+2*m+i*6];
						stuRsuControl.AntennaInfoN[i].Channel=buf[17+2*n+2*m+i*6];
						stuRsuControl.AntennaInfoN[i].Power=buf[18+2*n+2*m+i*6];
						//stuRsuControl.AntennaInfoN[i].send_status=buf[19+2*n+2*m+i*6];
						//stuRsuControl.AntennaInfoN[i].recv_status=buf[20+2*n+2*m+i*6];
					}
				}
				if(buf[8]==0xb0&&buf[nlen-2]==((crc_rsu&0xff00)>>8)&&buf[nlen-1]==(crc_rsu&0x00ff)&&buf[0]==0xff)
				{
					stuRsuData[1].RSUStatus=buf[9];
					stuRsuData[1].PSAMCount=buf[10];
					printf("RSU2 B0_psam num is %x\n",stuRsuData[1].PSAMCount);
					t=buf[10];
					for(i=0;i<t;i++)
					{
						stuRsuData[1].PSAMInfoN[i].PSAM_channel=buf[11+9*i];	
						stuRsuData[1].PSAMInfoN[i].PSAM_version=buf[12+9*i];
						stuRsuData[1].PSAMInfoN[i].PSAM_auth=buf[13+9*i];
						printf("RSU2 psam_ID is:");
						for(j=0;j<6;j++)
						{
							stuRsuData[1].PSAMInfoN[i].Psam_ID[j]=buf[14+9*i+j];
							printf(" %x-",stuRsuData[1].PSAMInfoN[i].Psam_ID[j]);
						}
						printf("\n");
					}
					stuRsuData[1].RSUAlgId=buf[11+9*t];
					stuRsuData[1].RSUManuID=buf[12+9*t];
					printf("RSU2 RSUManuID is %x\n",stuRsuData[1].RSUManuID);
					for(j=0;j<3;j++)
					{stuRsuData[1].RSUID[j]=buf[13+9*t+j];}
					for(j=0;j<2;j++)
					{stuRsuData[1].RSUVersion[j]=buf[16+9*t+j];}
					stuRsuData[1].Workstatus=buf[18+9*t];
					for(j=0;j<3;j++)
					{stuRsuData[1].FlagID[j]=buf[19+9*t+j];}
					for(j=0;j<4;j++)
					{stuRsuData[1].Reserved[j]=buf[22+9*t+j];}
				}
				if(buf[8]==0xd1&&buf[nlen-2]==((crc_rsu&0xff00)>>8)&&buf[nlen-1]==(crc_rsu&0x00ff)&&buf[0]==0xff)
				{
					stuRsuReset.AntennaCount=buf[9];
					stuRsuReset.RSUState=buf[10];
				}
			}
		}
	sleep(2);
}// 接收天线数据结束

