#include <stdlib.h>
#include <linux/watchdog.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>
#include <sys/time.h>
#include <csignal>

#include "main.h"
#include "rs485server.h"

using namespace std;

ENVI_PARAMS *stuEnvi_Param;		// �������ݽṹ��
UPS_PARAMS *stuUps_Param;		//USP�ṹ�� ��Դ���ݼĴ���
SPD_PARAMS *stuSpd_Param;		//�������ṹ��
DEVICE_PARAMS *stuDev_Param;	//װ�ò����Ĵ���
DeviceInfoParams *stuDev_Info;	//�ɼ����豸��Ϣ�ṹ��
RSU_PARAMS *stuRSU_Param;		//RSU������Ϣ�ṹ��
REMOTE_CONTROL *stuRemote_Ctrl;	//ң�ؼĴ����ṹ��
FLAGRUNSTATUS *stuFlagRunStatus;//�ż�����������״̬�ṹ��
//THUAWEIGantry *stuHUAWEIDevValue;//��Ϊ����״̬
RSUCONTROLER *stuRsuControl;	//RSU������״̬

extern string StrServerURL1;	//�����URL1
extern string StrServerURL2;	//�����URL2

/*char gIpAddr[20];
HANDLE g_hCamera;
int gCamStatus=CAMERA_STOP;
char gPlateNum[16];
ColorType gPlateColor;
void MYCameraJpegCallBackFunc(char * pBuffer,int size,IMAGE_CAPTURE_INFO *pImageInfo,DWORD nContext);
void MYCameraEventCallBackFunc(CAMERA_STATE cEvent,DWORD nContext);*/
void WriteLog(char* str);

int wdt_fd = -1;

void WatchDogInit(void)
{
   // int wdt_fd = -1;
    int timeout;

    wdt_fd = open("/dev/watchdog", O_WRONLY);
    if (wdt_fd == -1)
    {
        printf("fail to open /dev/watchdog!\n");
    }
    printf("/dev/watchdog is opened!\n");

    timeout = 60;
    ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);

    ioctl(wdt_fd, WDIOC_GETTIMEOUT, &timeout);
    printf("The timeout was is %d seconds\n", timeout);
}

//��ʱ�жϴ���
void sig_handler(int signo)
{
    printf("sig_handler\t\n");

/*	//���ż����в���ת��JSON�ַ���
	char * jsonPack=(char *)malloc(50*1024);
	FLAGRUNSTATUS *pFlagRunStatus;
	int jsonPackLen=0;
	jsonStrFlagRunStatusWriter((char*)pFlagRunStatus,jsonPack,&jsonPackLen);
	HttpPostParm(jsonPack,jsonPackLen);
	free(jsonPack);*/
}

//��ʼ����ʱ��
void InitTimer(void)
{
     struct timeval interval;
     struct itimerval timer;
     //����ʱ����Ϊ10��
     interval.tv_sec = 10;
	 interval.tv_usec =0;

     timer.it_interval = interval;
     timer.it_value = interval;

     setitimer(ITIMER_VIRTUAL, &timer, NULL);//��������SIGVTALRM�ź�

     //ΪSIGVTALRMע���źŴ�����
     signal(SIGALRM, sig_handler);
}

int main(void)
{
	char ch;
    int loop=0;
	char str[100];
	char * jsonPack=(char *)malloc(50*1024);
	REMOTE_CONTROL *pRCtrl;
	int jsonPackLen=0;
	FLAGRUNSTATUS *pFlagRunStatus;

	//�������ļ�
	GetConfig();

	// �������ݽṹ��
	stuEnvi_Param = (ENVI_PARAMS*)malloc(sizeof(ENVI_PARAMS));
	memset(stuEnvi_Param,0,sizeof(ENVI_PARAMS));
	//USP�ṹ�� ��Դ����
	stuUps_Param = (UPS_PARAMS*)malloc(sizeof(UPS_PARAMS));
	memset(stuUps_Param,0,sizeof(UPS_PARAMS));
	//�������ṹ��
	stuSpd_Param = (SPD_PARAMS*)malloc(sizeof(SPD_PARAMS));
	memset(stuSpd_Param,0,sizeof(SPD_PARAMS));
	//װ�ò����Ĵ���
	stuDev_Param = (DEVICE_PARAMS*)malloc(sizeof(DEVICE_PARAMS));
	memset(stuDev_Param,0,sizeof(DEVICE_PARAMS));
	//�ɼ����豸��Ϣ�ṹ��
	stuDev_Info = (DeviceInfoParams*)malloc(sizeof(DeviceInfoParams));
	memset(stuDev_Info,0,sizeof(DeviceInfoParams));
	//RSU������Ϣ�ṹ��
	stuRSU_Param = (RSU_PARAMS*)malloc(sizeof(RSU_PARAMS));
	memset(stuRSU_Param,0,sizeof(RSU_PARAMS));
	//ң�ؼĴ����ṹ��
	stuRemote_Ctrl = (REMOTE_CONTROL*)malloc(sizeof(REMOTE_CONTROL));
	memset(stuRemote_Ctrl,0,sizeof(REMOTE_CONTROL));
	//�ż�����������״̬�ṹ��
	stuFlagRunStatus = (FLAGRUNSTATUS*)malloc(sizeof(FLAGRUNSTATUS));
	memset(stuFlagRunStatus,0,sizeof(FLAGRUNSTATUS));
	//��Ϊ����״̬�ṹ��
//	stuHUAWEIDevValue = (THUAWEIGantry*)malloc(sizeof(THUAWEIGantry));
//	memset(stuHUAWEIDevValue,0,sizeof(THUAWEIGantry));
	///RSU������״̬
	stuRsuControl = (RSUCONTROLER*)malloc(sizeof(RSUCONTROLER));
	memset(stuRsuControl,0,sizeof(RSUCONTROLER));

	//system("hwclock –s");  //写到硬时�?
	//��ʼ������
	cominit();
	rs485init();

	//��ʼ��http�����
	HttpInit();

	//��ʼ���������߳�
	initServer();

//    WatchDogInit();

	//��ʼ�����������
//	IpcamInit();

	//��ʼ����ʱ��
//	InitTimer();

	//��ʼ��RSU
	init_net_rsu();

	//��ʼ��RSU
	snmpInit();

	//��ʼ����ͨ��ʱ�����߳�
	//init_LTKJ_DataPost();

	//��ʼ��������ʱ�����߳�
	//init_XY_DataPost();


/*    while(1)
    {
		usleep(5000);//delay 5ms
	}*/
	printf("������ʾ : \n 'v' ���Ͱ汾��\n 'e' ��ѯ��������\n 'u' ��ѯUPS����\n 's' ��ѯ����������\n ");
	printf("'d' ��ѯװ�ò���\n 'i' ��ѯװ����Ϣ\n 'r' ��ѯRSU���߲���\n 'c' ����������\n 'o' ����������\n ");
	printf("'h' �����ż�����״̬\n 't' RSU�·�����\n 'q' �˳�\n");
    while('q' != (ch = getchar()))
    {
        switch(ch)
        {
//        case 't' :  //����ץ��
//			TrigerImage(g_hCamera);
//			break;
        case 'v' :  //�����ַ���
			sprintf(str,"MySendMessage! �㶫��ͨ�Ƽ�Ͷ�����޹�˾��%d\n",loop/10);
			printf(str);
			MySendMessage(str);

			break;
		case 'e' :	//��ѯ���������Ĵ���
			//SendCom1QueryEvnReg();
			SendCom1ReadReg(0x01,READ_REGS,ENVI_START_ADDR,ENVI_REG_MAX);
			break;
		case 'u' :	//��ѯUPS�����Ĵ���
			SendCom1ReadReg(0x01,READ_REGS,UPS_START_ADDR,UPS_REG_MAX);
			break;
		case 's' :	//��ѯSPD�����Ĵ���
			SendCom1ReadReg(0x01,READ_REGS,SPD_START_ADDR,SPD_REG_MAX);
			break;
		case 'd' :	//��ѯװ�ò����Ĵ���
			SendCom1ReadReg(0x01,READ_REGS,PARAMS_START_ADDR,PARAMS_REG_MAX);
			break;
		case 'i' :	//��ѯװ����Ϣ�Ĵ���
			SendCom1ReadReg(0x01,READ_REGS,DEVICEINFO_START_ADDR,DEVICEINFO_REG_MAX);
			break;
		case 'r' :	//��ѯRSU���߲����Ĵ���
			SendCom1ReadReg(0x01,READ_REGS,RSU_START_ADDR,RSU_REG_MAX);
			break;
		case 'c' :	//ң�ؼĴ��� ����
			memset(jsonPack,0,1024);
			pRCtrl=stuRemote_Ctrl;
//			pRCtrl->Door_Lock=0xFF00;  //����������
//			pRCtrl->Door_UnLock=0x0000;  //����������
			pRCtrl->RSU1_Close=0xFF00;  //RSU����1ң��ִ��
			pRCtrl->RSU1_Open=0x0000;  //RSU����1ң��ִ��
			jsonStrRemoteCtrlWriter((char*)pRCtrl,jsonPack,&jsonPackLen);
			NetSendParm(NETCMD_REMOTE_CONTROL,jsonPack,jsonPackLen);
			break;
		case 'o' :	//ң�ؼĴ��� ����
			memset(jsonPack,0,1024);
			pRCtrl=stuRemote_Ctrl;
//			pRCtrl->Door_Lock=0x0000;  //����������
//			pRCtrl->Door_UnLock=0xFF00;  //����������
			pRCtrl->RSU1_Close=0x0000;  //RSU����1ң��ִ��
			pRCtrl->RSU1_Open=0xFF00;  //RSU����1ң��ִ��
			jsonStrRemoteCtrlWriter((char*)pRCtrl,jsonPack,&jsonPackLen);
			NetSendParm(NETCMD_REMOTE_CONTROL,jsonPack,jsonPackLen);
			break;
		case 'h' :	//�����ż�����״̬
			memset(jsonPack,0,50*1024);
			SetjsonTableStr("flagrunstatus",jsonPack,&jsonPackLen);
//			SetjsonFlagRunStatusStr(jsonPack,&jsonPackLen);
			printf("%s",jsonPack);
			HttpPostParm(StrServerURL1,jsonPack,jsonPackLen);
			NetSendParm(NETCMD_FLAGRUNSTATUS,jsonPack,jsonPackLen);
			break;
		case 't' :	//RSU�·�����C0֡
			send_RSU(0xC4,0 ,1,1);
			break;
		case 'y' :	//���Կ���
			SendCom4RCtlReg(DOOR_LOCK_ADDR,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_UNLOCK);
			break;
		case 'z' :	//������
			SendCom4RCtlReg(DOOR_LOCK_ADDR,SINGLE_WRITE_HW,DOOR_LOCK_REG,REMOTE_LOCK);
			break;
		case 'q' : //�˳�
			break;
		}
	}
//	ReleaseCamera(g_hCamera);
	Net_close();
	free(jsonPack);
	return 0;
}


/*void InitCamera(void)
{
	sprintf(gIpAddr,"192.192.2.139");
	g_hCamera=CreateCamera();
	SetCameraCallBack(g_hCamera,MYCameraJpegCallBackFunc,MYCameraEventCallBackFunc,(DWORD)g_hCamera);
	ConnectCamera(g_hCamera,gIpAddr,DBVT_MYPORT);
	ConnectDataCamera(g_hCamera,gIpAddr);
}

//��Ƶ��ʾ�߳�(JPEG)
void MYCameraJpegCallBackFunc(char * pBuffer,int size,IMAGE_CAPTURE_INFO *pImageInfo,DWORD nContext)
{
	char str[100],strPlatePos[100];
	int nPlatePosStrLen=0;

	//�洢JPEGͼƬ
	memcpy(gPlateNum,pImageInfo->nCarSpeed,sizeof(char)*12);		   //����
	gPlateColor=(ColorType)pImageInfo->nLimitSpeed[0];
	printf("MYCameraJpegCallBackFunc plate=%s, color=%d\n",gPlateNum,gPlateColor);

	FILE *ExprssWayFile;
	ExprssWayFile = fopen("capture.jpg","wb");
	if(ExprssWayFile!=NULL)
	{
		fwrite(pBuffer,1,size,ExprssWayFile);
	}
	fclose(ExprssWayFile);
}

void MYCameraEventCallBackFunc(CAMERA_STATE cEvent,DWORD nContext)
{
	HANDLE hCamera = (HANDLE)nContext;
    if(cEvent==CAMERA_LINKED)
    {
        gCamStatus=CAMERA_LINKED;
		printf("CAMERA_LINKED\n");
    }

    if(cEvent==CAMERA_BREAK)
    {
        gCamStatus=CAMERA_BREAK;
		printf("CAMERA_BREAK\n");
    }
}

void WriteLog(char* str)
{
	printf("%s\n",str);
}*/


