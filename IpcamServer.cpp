
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <string>
#include <pthread.h>
#include <vector>
#include <sstream> 
#include <iostream>
#include "IpcamServer.h"
#include "HttpPost.h"
#include "registers.h"
#include <unistd.h>


using namespace std;

//摄像机的ip地址
extern string StrVehPlateCount;	//识别仪数量
extern string StrVehPlateIP[VEHPLATE_NUM];	//识别仪IP地址
extern string StrVehPlatePort[VEHPLATE_NUM];	//识别仪端口
extern string StrVehPlateKey[VEHPLATE_NUM];	//识别仪用户名密码

string mstrurlget[VEHPLATE_NUM];
string mstrurlctl[VEHPLATE_NUM];
string mstrkey[VEHPLATE_NUM];

extern bool jsonIPCamReader(char* jsonstr, int len,int mIndex);

pthread_mutex_t IpCamStateMutex ;
TIPcamState mTIPcamState[VEHPLATE_NUM];

void *HTTP_IPCamDataGet(void *param)
{
    int i,IntIpcamCount = atoi(StrVehPlateCount.c_str());
    int n = 0;

	for(i=0;i<IntIpcamCount;i++)
	{
    	mstrurlget[i] = StrVehPlateIP[i] + ":" + StrVehPlatePort[i] + "/api/LeaTop/GetDeviceStatus" ;

    	mstrurlctl[i] = StrVehPlateIP[i] + ":" + StrVehPlatePort[i] + "/api/LeaTop/ControlDevice" ;

    	mstrkey[i] = StrVehPlateIP[i] + ":" + StrVehPlatePort[i] ;
	}
    string mStrdata;
	while(1)
	{
        for(n=0;n<IntIpcamCount;n++)
        {
            HttpPostParm(mstrurlget[n],mStrdata,mstrkey[n],HTTPGET);
            pthread_mutex_lock(&IpCamStateMutex);
            jsonIPCamReader((char *)(mStrdata.c_str()),mStrdata.size(),n);
            pthread_mutex_unlock(&IpCamStateMutex);
            sleep(1);
        }
        sleep(300);
	}
	return 0 ;
}


int RebootIpCam(int Ipcamindex)
{
    if((Ipcamindex < 0) || (Ipcamindex > 6))
        return 1 ;

    string mStrdata = "{\"action\":\"camreboot\",\"entity\": null}";
    HttpPostParm(mstrurlctl[Ipcamindex],mStrdata,mstrkey[Ipcamindex],HTTPPOST);
  //  jsonIPCamReader((char *)(mStrdata.c_str()),mStrdata.size());
    return 0 ;

}


int IpCamServerInit(void)
{
	initIPcamState();
	
    pthread_mutex_init(&IpCamStateMutex,NULL);
    pthread_t m_HTTP_IPCamDataGet ;
    pthread_create(&m_HTTP_IPCamDataGet,NULL,HTTP_IPCamDataGet,NULL);
    return 0 ;
}

void initIPcamState()
{
	int i;
	for(i=0;i<VEHPLATE_NUM;i++)
	{
		mTIPcamState[i].ip="";//摄相机IP
		mTIPcamState[i].factoryid="2147483647"; //摄相机厂商1-宇视2-海康3-大华4-中威5-哈工大6-华为7-北京智通 8-北京信路威
		mTIPcamState[i].statuscode="2147483647";//摄相机状态编码 0-正常 1-异常
		mTIPcamState[i].errcode="2147483647";//正常时为0，状态异常时为厂商自定义的错误代码
		mTIPcamState[i].devicemodel="";//摄相机的设备型号
		mTIPcamState[i].softversion="";//摄相机的软件版本号
		mTIPcamState[i].statustime="";//状态时间
		mTIPcamState[i].filllight="2147483647";//闪光灯状态 0:正常，1:异常
		mTIPcamState[i].temperature="2147483647";//摄像枪温度（摄氏度）
	}
}
	
