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
#include "net_rsu.h"
#define NETWORK_FILE "/opt/config"
static int sockfd_rsu; 
extern RSUCONTROLER *stuRsuControl;	//RSU控制器状态

static void* NetWork_server_thread_RSU(void *arg);
unsigned short const crc_ccitt_table[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
static inline unsigned short  crc_ccitt_byte(unsigned short crc, const unsigned char c)
{
	return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
}
void send_RSU(char command,bool ReSend,char state,int num)
{
	char send_buff[128];
	int rsctl_all=0x10; 
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
		send_buff[7] = 2;
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
		send_buff[7] = 17;
		send_buff[8] = 0xc0;
		for(i=0;i<17;i++)
		{
			send_buff[9+i] = 0x00;
		}
		buff_len=25;
		break;
		case 0x1D:
		send_buff[4] = 0x00;		//数据长度四个字节
		send_buff[5] = 0x00;
		send_buff[6] = 0x00;
		send_buff[7] = 2;
		send_buff[4] = 0x1d;
		send_buff[5] = num;
		buff_len=10;
		break;
	}
	for(i=3;i<buff_len;i++)
	{
		crc=crc_ccitt_byte(crc,send_buff[i]);
	}
	crc ^= 0xffff;
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
    	printf ("ERROR: Failed to obtain Socket Despcritor.\n");
    	return (0);
	} else {
    	printf ("OK: Obtain Socket Despcritor sucessfully.\n");
	}
	IPaddress = getNetworkInfo("address");//获取配置文件中的IP地址
	IPport=getNetworkInfo("port");
	port=atoi(IPport);
	/* Fill the local socket address struct */
	server_addr.sin_family = AF_INET;           		// Protocol Family
	server_addr.sin_port = htons (port);         		// Port number
	server_addr.sin_addr.s_addr  = inet_addr (IPaddress);  	// AutoFill local address
	memset (server_addr.sin_zero,0,8);          		// Flush the rest of struct
//侯林汝写的侦听服务器-天线的数据代码
	if (connect(sockfd_rsu, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
		printf("connect to server error!\n");
	}
	else{
	while(1)
	{
		nlen = read(sockfd_rsu, buf, sizeof(buf)-1);
			if (nlen == -1)
			{
			printf("read message error\n");
		}
		else
		{
/*			printf("read data from the server:");//接收到B9帧数据，解析第十一字节（天线数量）十二字节*4（天线状态）				
			for(i=0;i<nlen;i++)
			{
				printf("%x-",buf[i]);
			}
			printf("\n");*/
			if(buf[8]==0xb9)
			{	
				stuRsuControl->ControlCount=buf[17]; 	//控制器数量
				
				temp=stuRsuControl->ControlCount;
				for(i=0;i<temp;i++)
				{stuRsuControl->ControlStatusN[i]=buf[18+i];}		//控制器状态
				stuRsuControl->AntennaCount=buf[18+temp];	//天线数量
				for(j=0;j<stuRsuControl->AntennaCount;j++)
				{
					stuRsuControl->AntennaInfoN[j].Status=buf[19+temp+j*4];
					stuRsuControl->AntennaInfoN[j].Power=buf[20+temp+j*4];;
					stuRsuControl->AntennaInfoN[j].Channel=buf[21+temp+j*4];;
					stuRsuControl->AntennaInfoN[j].Control_state=buf[22+temp+j*4];
				}
/*				printf("ControlCount=%d\t\n",stuRsuControl->ControlCount);
				for(i=0;i<stuRsuControl->ControlCount;i++)
					printf("Control %d ControlStatusN=%d\t\n",i,stuRsuControl->ControlStatusN[i]);
				for(j=0;j<stuRsuControl->AntennaCount;j++)
					printf("Antenna %d Status=%d  Power=%d  Channel=%d  Control_state=%d\t\n",j,stuRsuControl->AntennaInfoN[j].Status,stuRsuControl->AntennaInfoN[j].Power,stuRsuControl->AntennaInfoN[j].Channel,stuRsuControl->AntennaInfoN[j].Control_state);*/
			}
			else if(buf[8]==0xb0)
			{
				data2.RSUManuID=buf[10];
				for(j=0;j<3;j++)
				{data2.RSUID[i]=buf[11+i];}
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
		sleep(5);
	}
}	
}// 接收天线数据结束

