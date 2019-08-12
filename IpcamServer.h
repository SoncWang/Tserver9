#ifndef __IPCAMSERVER_H__
#define __IPCAMSERVER_H__

#include <string>
using namespace std;

int IpCamServerInit(void);
void initIPcamState();


#pragma pack(push, 1)

typedef struct
{
   string ip;            //摄相机IP
   string factoryid;      //摄相机厂商1-宇视2-海康3-大华4-中威5-哈工大6-华为7-北京智通 8-北京信路威
   string statuscode ;  //摄相机状态编码 0-正常 1-异常
   string errcode;      //正常时为0，状态异常时为厂商自定义的错误代码
   string devicemodel;  //摄相机的设备型号
   string softversion;  //摄相机的软件版本号
   string statustime;   //状态时间
   string filllight ;   //闪光灯状态 0:正常，1:异常
   string temperature ; //摄像枪温度（摄氏度）
}TIPcamState;


#pragma pack(pop)



#endif

