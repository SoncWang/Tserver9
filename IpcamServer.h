#ifndef __IPCAMSERVER_H__
#define __IPCAMSERVER_H__

#include <string>
using namespace std;

int IpCamServerInit(void);
void initIPcamState(int camno);
void initIPcam900State(int camno);


#pragma pack(push, 1)

typedef struct
{
	unsigned long TimeStamp; 		//状态获取时间戳
	bool Linked;					//连接状态 0:断开 1：连接
	
	string ip;            //摄相机IP
	string factoryid;      //摄相机厂商1-宇视2-海康3-大华4-中威5-哈工大6-华为7-北京智通 8-北京信路威
	//string statuscode ;  //摄相机状态编码 0-正常 1-异常
	string errcode;      //正常时为0，状态异常时为厂商自定义的错误代码
	string devicemodel;  //摄相机的设备型号
	string softversion;  //摄相机的软件版本号
	string statustime;   //状态时间
	string filllight ;   //闪光灯状态 0:正常，1:异常
	string temperature ; //摄像枪温度（摄氏度）
	//交通部协议的状态内容部分
	string picstateid;   //流水号
	string gantryid;     //门架编号,全网唯一编号
	string statetime;    //状态采集时间
	string overstockImageJourCount ; //积压图片流水数
	string overstockImageCount;      //积压图片数
	string cameranum;    //相机编号（101~299）
	string lanenum;      //车道编号
	string connectstatus; //连接状态
	string workstatus;    //工作状态
	string lightworkstatus; //补光灯的工作状态
	string recognitionrate; //识别成功率
	string hardwareversion; //固件版本
	string softwareversion; //软件版本
	string runningtime;     //设备从开机到现在的运行时间（秒）
	string brand;           //厂商型号
	string devicetype;      //设备型号
	string statuscode;      //状态码,详见附录A3 0-正常；其他由厂商自定义
	string statusmsg;       //状态描述 由厂商自定义,最大长度256 例如：正常
}TIPcamState;


#pragma pack(pop)



#endif

