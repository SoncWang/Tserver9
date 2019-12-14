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
#include "ats.h"
#include "snmp.h"
#include "config.h"



using namespace std;

int atsssockSrv;
struct sockaddr_in atsAddr;
pthread_mutex_t atsdataMutex ;

extern string StrHWServer;		//华为服务器地址

unsigned char atslockstate = 0x30;
unsigned char atsforntdoorstate = 0x30;
unsigned char atsbackdoorstate = 0x30;
unsigned char atsopendoorstate = 0x30;

extern THUAWEIGantry HUAWEIDevValue;
extern THUAWEIALARM HUAWEIDevAlarm;

//int atsInit(void);

//CABINETTYPE:作为区分机柜类型，用于编译不同的代码
//CABINETTYPE  1：华为（包括华为单门 双门等） 5：中兴; 6：金晟安; 7：爱特思 StrVersionNo
#if(CABINETTYPE == 7) //爱特思

unsigned char getbcc(unsigned char *buf,int len)
{
   unsigned char bcc=0x00;
   for(int i=0;i<len;i++)
   {
       bcc^=buf[i];
   }
   return bcc;
}

int SendatsData(unsigned char cam,unsigned char *buf,unsigned char len)
{
   if(len > 45)
      return 0 ;
   unsigned char sendbuf[50];
   unsigned char *pbuf = sendbuf;
   *(pbuf++) = 0x02;
   *(pbuf++) = cam;
   *(pbuf++) = len;
   if(len > 0)
   {
      memcpy(pbuf,buf,len);
      pbuf += len;
   }
   unsigned char mbcc = getbcc(sendbuf+1, pbuf-sendbuf-1);
   *(pbuf++) = mbcc ;
   *(pbuf++) = 0x03 ;

   sendto(atsssockSrv,(char *)sendbuf,pbuf-sendbuf,0,(struct sockaddr *)&atsAddr,sizeof(struct sockaddr_in));
int nnn = pbuf-sendbuf;
for(int n=0;n<nnn;n++)
printf(" %x",sendbuf[n]);
printf("\r\n");
   return 0 ;
}


int openatslock(void)
{
   unsigned char buf[50];
   //远程开锁
   buf[0] = 0x00;
   SendatsData(0x53,buf,0);
   sleep(1);
   return 0 ;
}


void *atsSendthread(void *param)
{
  sleep(3);
  unsigned char buf[50];
  while(1)
  {
     //发送获取
     //SendatsData(unsigned char cam,unsigned char *buf,unsigned char len)
     //查询 8 路输入状态 02 41 00 41 03
     //buf[0] = 0x00;
     //SendatsData(0x41,buf,0);
     //sleep(1);
     //查询门锁状态
     buf[0] = 0x00;
     SendatsData(0x51,buf,0);
     sleep(1);
     //远程开锁
     buf[0] = 0x00;
     SendatsData(0x53,buf,0);
     sleep(1);
     //电池状态
     buf[0] = 0x00;
     SendatsData(0x61,buf,0);
     sleep(1);
     //逆变器状态
     buf[0] = 0x00;
     SendatsData(0x71,buf,0);
     sleep(1);
     //空调状态
     buf[0] = 0x00;
     SendatsData(0x81,buf,0);
     sleep(1);
     //机柜温湿度
     buf[0] = 0x00;
     SendatsData(0x91,buf,0);
     sleep(1);
     //电表状态
     buf[0] = 0x00;
     SendatsData(0xa1,buf,0);
     sleep(1);
     //通信电源状态
     buf[0] = 0x00;
     SendatsData(0xb1,buf,0);
     sleep(1);
     //查询水浸及烟雾状态
     buf[0] = 0x00;
     SendatsData(0xc1,buf,0);

     sleep(5);

  }
  
}


short int  yuanma(unsigned short int buma)
{
   short int  yuanmatem ;
   //负数
   if(buma&0x8000)
   {
     yuanmatem = ~buma;
     yuanmatem = yuanmatem+1;
     yuanmatem |= 0x8000;
   }
   else //正数
     yuanmatem = buma;

   return yuanmatem;

}



void atcDataRecv(unsigned char *pbuf,int len)
{
   for(int n=0;n<len;n++)
     printf(" %.2x",pbuf[n]);
   printf("\r\n");
   switch(pbuf[1])
   {
      case 0x42://查询 8 路输入状态
           break;
      case 0x52: //查询门锁状态
           atslockstate = pbuf[3];
           atsforntdoorstate = pbuf[4];
           atsbackdoorstate = pbuf[5];
           printf("门锁状态:%.2x,%.2x,%.2x",atslockstate,atsforntdoorstate,atsbackdoorstate);
           if((atsforntdoorstate == 0x31) || (atsbackdoorstate == 0x31))
           {
               HUAWEIDevAlarm.hwDoorAlarmTraps = "1";
               printf("门禁告警\r\n");
           }
           else
               HUAWEIDevAlarm.hwDoorAlarmTraps = "0";
           break;
      case 0x54://远程开锁
           atsopendoorstate = pbuf[3];
           printf("开锁状态:%.2x",atsopendoorstate);
           break;
      case 0x62://电池状态
      {
           //x1-x14 为电池 1 的参数，x15-x28 为电池 2 参数，每种参数均由两字节组成，先高位后低位
           unsigned short int mget;
           float fRemainCapacity;
           float fTotalCapacity;
           float fmget ;
           char fbuf[30];
           pbuf += 3;
           //温度
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++); 
           fmget = (float)mget;
           memset(fbuf,0x00,30);
           sprintf(fbuf,"%f",fmget/100) ;
           HUAWEIDevValue.strhwAcbGroupTemperature = fbuf;
           printf("电池1温度:%s\r\n",HUAWEIDevValue.strhwAcbGroupTemperature.c_str());

           //电流
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++); 
           short int myuanma =  yuanma(mget);
           fmget = (float)(myuanma & 0x7FFF);
           memset(fbuf,0x00,30);
           if(myuanma & 0x8000)
             sprintf(fbuf,"-%f",fmget/100) ;
           else
              sprintf(fbuf,"%f",fmget/100) ;
           HUAWEIDevValue.strhwAcbGroupBatCurr = fbuf;
           printf("电池1电流:%s\r\n",HUAWEIDevValue.strhwAcbGroupBatCurr.c_str());

           //电压
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++); 
           fmget = (float)mget;
           memset(fbuf,0x00,30);
           sprintf(fbuf,"%f",fmget/1000) ;
           HUAWEIDevValue.strhwAcbGroupBatVolt = fbuf;
           printf("电池1电压:%s\r\n",HUAWEIDevValue.strhwAcbGroupBatVolt.c_str());

           //剩余容量
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++); 
           fmget = (float)mget;
           fRemainCapacity = fmget;
           //memset(fbuf,0x00,30);
           //sprintf(fbuf,"%f",fmget/100) ;
           //HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity = fbuf;
           //printf("电池1剩余容量:%s\r\n",HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str());


           //充满容量
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++); 
           fmget = (float)mget;
           fTotalCapacity = fmget;
           //memset(fbuf,0x00,30);
           //sprintf(fbuf,"%f",fmget/100) ;
           //HUAWEIDevValue.strhwAcbGroupTotalCapacity = fbuf;
           //printf("电池1充满容量:%s\r\n",HUAWEIDevValue.strhwAcbGroupTotalCapacity.c_str());


           //循环次数
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++); 
           //fmget = (float)mget;
           memset(fbuf,0x00,30);
           sprintf(fbuf,"%d",mget) ;
          // HUAWEIDevValue.strhwAcbGroupTotalCapacity = fbuf;
           printf("电池1循环次数:%s\r\n",fbuf);

           //设计容量
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++); 
           fmget = (float)mget;
           memset(fbuf,0x00,30);
           sprintf(fbuf,"%f",fmget/100) ;
           //HUAWEIDevValue.strhwAcbGroupTotalCapacity = fbuf;
           printf("电池1设计容量:%s\r\n",fbuf);

           //温度
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++);
           fmget = (float)mget;
           memset(fbuf,0x00,30);
           sprintf(fbuf,"%f",fmget/100) ;
           //HUAWEIDevValue.strhwAcbGroupTemperature = fbuf;
           printf("电池2温度:%s\r\n",fbuf);

           //电流
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++);

           myuanma =  yuanma(mget);
          fmget = (float)(myuanma & 0x7FFF);
           memset(fbuf,0x00,30);
           if(myuanma & 0x8000)
             sprintf(fbuf,"-%f",fmget/100) ;
           else
             sprintf(fbuf,"%f",fmget/100) ;
           HUAWEIDevValue.strhwAcbBatCurr = fbuf;
           printf("电池2电流:%s\r\n",HUAWEIDevValue.strhwAcbBatCurr.c_str());

           //电压
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++);
           fmget = (float)mget;
           memset(fbuf,0x00,30);
           sprintf(fbuf,"%f",fmget/1000) ;
           HUAWEIDevValue.strhwAcbBatVolt = fbuf;
           printf("电池2电压:%s\r\n",HUAWEIDevValue.strhwAcbBatVolt.c_str());

           //剩余容量
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++);
           fmget = (float)mget;
           fRemainCapacity += fmget;
           //memset(fbuf,0x00,30);
           //sprintf(fbuf,"%f",fmget/100) ;
           //HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity = fbuf;
           //printf("电池2剩余容量:%s\r\n",fbuf);


           //充满容量
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++);
           fmget = (float)mget;
           fTotalCapacity += fmget;
           //memset(fbuf,0x00,30);
          // sprintf(fbuf,"%f",fmget/100) ;
           //HUAWEIDevValue.strhwAcbBatCapacity = fbuf;
           //printf("电池2充满容量:%s\r\n",HUAWEIDevValue.strhwAcbBatCapacity.c_str());


           //赋值累加容量
           memset(fbuf,0x00,30);
           sprintf(fbuf,"%f",fRemainCapacity/100) ;
           HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity = fbuf;
           printf("电池组剩余容量:%s\r\n",HUAWEIDevValue.strhwAcbGroupTotalRemainCapacity.c_str());

           memset(fbuf,0x00,30);
           sprintf(fbuf,"%f",fTotalCapacity/100) ;
           HUAWEIDevValue.strhwAcbGroupTotalCapacity = fbuf;
           printf("电池组充满容量:%s\r\n",HUAWEIDevValue.strhwAcbGroupTotalCapacity.c_str());




           //循环次数
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++);
           //fmget = (float)mget;
           memset(fbuf,0x00,30);
           sprintf(fbuf,"%d",mget) ;
          // HUAWEIDevValue.strhwAcbGroupTotalCapacity = fbuf;
           printf("电池2循环次数:%s\r\n",fbuf);

           //设计容量
           mget = *(pbuf++);
           mget = (mget<<8) & 0xFF00;
           mget = mget | *(pbuf++);
           fmget = (float)mget;
           //memset(fbuf,0x00,30);
           //sprintf(fbuf,"%f",fmget/100) ;
           //HUAWEIDevValue.strhwAcbGroupTotalCapacity = fbuf;
           //printf("电池2设计容量:%s\r\n",fbuf);

      }
           break;

      case 0x72://逆变器状态
      {
         unsigned short int mget;
         float fmget ;
         char fbuf[30];
         pbuf += 3;
         //市电电压
         mget = *(pbuf++);
         mget = (mget<<8) & 0xFF00;
         mget = mget | *(pbuf++);
         fmget = (float)mget;
         memset(fbuf,0x00,30);
         sprintf(fbuf,"%f",fmget/10) ;
         HUAWEIDevValue.StrUpsCityVol = fbuf;
         printf("逆变器市电电压:%s\r\n",fbuf);

         //输出电压
         mget = *(pbuf++);
         mget = (mget<<8) & 0xFF00;
         mget = mget | *(pbuf++);
         fmget = (float)mget;
         memset(fbuf,0x00,30);
         sprintf(fbuf,"%f",fmget/10) ;
         HUAWEIDevValue.StrUpsOVol = fbuf;
         printf("逆变器输出电压:%s\r\n",fbuf);


         //负载百分比
         mget = *(pbuf++);
         mget = (mget<<8) & 0xFF00;
         mget = mget | *(pbuf++);
         //fmget = (float)mget;
         memset(fbuf,0x00,30);
         sprintf(fbuf,"%d",mget) ;
         //HUAWEIDevValue.strhwAcbGroupTemperature = fbuf;
         //printf("逆变器负载百分比:%s\r\n",fbuf);

         //市电异常状态
         mget = *(pbuf++);
         if(mget == 1)
           printf("逆变器市电异常\r\n");
          //电池电压低状态
         mget = *(pbuf++);
         if(mget == 1)
           printf("电池电压低异常\r\n");
          //旁路工作状态
         mget = *(pbuf++);
         if(mget == 1)
           printf("旁路工作异常\r\n");
          //ups 故障状态
         mget = *(pbuf++);
         if(mget == 1)
         {
           HUAWEIDevAlarm.Ups_alarm = "1";
           printf("逆变器ups异常\r\n");
         }
         else
         {
           HUAWEIDevAlarm.Ups_alarm = "0";
           printf("逆变器ups正常\r\n");
         }
         //ups温度
         mget = *(pbuf++);
         mget = (mget<<8) & 0xFF00;
         mget = mget | *(pbuf++);
         fmget = (float)mget;
         memset(fbuf,0x00,30);
         sprintf(fbuf,"%f",fmget/10) ;
         HUAWEIDevValue.StrUpsTemp = fbuf;
         printf("ups温度:%s\r\n",fbuf);

      }
      break;

      case 0x82://空调状态 固定开机温度40度 关机温度25度
      {

         unsigned short int mget;
         float fmget ;
         char fbuf[30];
         pbuf += 3;

         //制冷器状态
         mget = *(pbuf++);
         if(mget == 0)
         {
             HUAWEIDevValue.StrCoolerState = "0";
             printf("空调制冷器闭合\r\n");
         }
         else if(mget == 1)
         {
             HUAWEIDevValue.StrCoolerState = "1";
             printf("空调制冷器开启\r\n");
         }

         //内风机状态
         mget = *(pbuf++);
         if(mget == 0)
         {
             HUAWEIDevValue.StrIn_FanState = "0";
             printf("空调内风机闭合\r\n");
         }
         else if(mget == 1)
         {
             HUAWEIDevValue.StrIn_FanState = "1";
             printf("空调内风机开启\r\n");
         }

         //外风机状态
         mget = *(pbuf++);
         if(mget == 0)
         {
             HUAWEIDevValue.StrOut_FanState = "0";
             printf("空调外风机闭合\r\n");
         }
         else if(mget == 1)
         {
             HUAWEIDevValue.StrOut_FanState = "1";
             printf("空调外风机开启\r\n");
         }


         //加热器状态
         mget = *(pbuf++);
         if(mget == 0)
         {
             HUAWEIDevValue.StrHeaterState = "0";
             printf("空调加热器闭合\r\n");
         }
         else if(mget == 1)
         {
             HUAWEIDevValue.StrHeaterState = "1";
             printf("空调加热器开启\r\n");
         }


         //制冷器故障告警
         mget = *(pbuf++);
         if(mget == 1)
         {
             HUAWEIDevAlarm.Air_Cooler_alarm = "1";
             printf("空调制冷器故障告警\r\n");
         }
         else if(mget == 0)
         {
             HUAWEIDevAlarm.Air_Cooler_alarm = "0";
             printf("空调制冷器正常\r\n");
         }

         //高温告警
         mget = *(pbuf++);
         if(mget == 1)
         {
             HUAWEIDevAlarm.Air_High_temper_alarm = "1";
             printf("空调高温告警\r\n");
         }
         else if(mget == 0)
         {
             HUAWEIDevAlarm.Air_High_temper_alarm = "0";
             printf("空调高温正常\r\n");
         }

         //低温告警
         mget = *(pbuf++);
         if(mget == 1)
         {
             HUAWEIDevAlarm.Air_Lower_temper_alarm = "1";
             printf("空调低温告警\r\n");
         }
         else if(mget == 0)
         {
             HUAWEIDevAlarm.Air_Lower_temper_alarm = "0";
             printf("空调低温正常\r\n");
         }

         //加热器故障告警
         mget = *(pbuf++);
         if(mget == 1)
         {
             HUAWEIDevAlarm.Air_Heater_alarm = "1";
             printf("空调加热器故障告警\r\n");
         }
         else if(mget == 0)
         {
             HUAWEIDevAlarm.Air_Heater_alarm = "0";
             printf("空调加热器正常\r\n");
         }

         //温度传感器故障
         mget = *(pbuf++);
         if(mget == 1)
         {
             HUAWEIDevAlarm.Air_Temper_Sensor_alarm = "1";
             printf("空调温度传感器故障\r\n");
         }
         else if(mget == 0)
         {
             HUAWEIDevAlarm.Air_Temper_Sensor_alarm = "0";
             printf("空调温度传感器正常\r\n");
         }

         //电压高压告警
         mget = *(pbuf++);
         if(mget == 1)
         {
             HUAWEIDevAlarm.Air_High_Vol_alarm = "1";
             printf("空调电压高压告警\r\n");
         }
         else if(mget == 0)
         {
             HUAWEIDevAlarm.Air_High_Vol_alarm = "0";
             printf("空调电压高压正常\r\n");
         }

         //电压低压告警
         mget = *(pbuf++);
         if(mget == 1)
         {
             HUAWEIDevAlarm.Air_Lower_Vol_alarm = "1";
             printf("电压低压告警\r\n");
         }
         else if(mget == 0)
         {
             HUAWEIDevAlarm.Air_Lower_Vol_alarm = "0";
             printf("电压低压正常\r\n");
         }


      }
      break;

      case 0x92://机柜温湿度
      {
         unsigned short int mget;
         float fmget ;
         char fbuf[30];
         pbuf += 3;
         //机柜温度
         mget = *(pbuf++);
         mget = (mget<<8) & 0xFF00;
         mget = mget | *(pbuf++);
         fmget = (float)mget;
         memset(fbuf,0x00,30);
         sprintf(fbuf,"%f",fmget/10) ;
         HUAWEIDevValue.strhwEnvTemperature[0] = fbuf;
         printf("机柜温度:%s\r\n",fbuf);
         //机柜湿度
         mget = *(pbuf++);
         mget = (mget<<8) & 0xFF00;
         mget = mget | *(pbuf++);
         fmget = (float)mget;
         memset(fbuf,0x00,30);
         sprintf(fbuf,"%f",fmget/10) ;
         HUAWEIDevValue.strhwEnvHumidity[0] = fbuf;
         printf("机柜湿度:%s\r\n",fbuf);

      }
      break;

     case 0xa2://电表状态
     {
       unsigned short int mget;
       float fmget ;
       char fbuf[30];
       pbuf += 3;
       //累计电量
       unsigned int powertall ;
       memcpy(&powertall,pbuf,4);
       powertall = ntohl(powertall);
       fmget = (float)powertall;
       memset(fbuf,0x00,30);
       sprintf(fbuf,"%f",fmget/100) ;
       printf("累计电量:%s\r\n",fbuf);
       pbuf += 4;

       //电压
       powertall ;
       memcpy(&powertall,pbuf,4);
       powertall = ntohl(powertall);
       fmget = (float)powertall;
       memset(fbuf,0x00,30);
       sprintf(fbuf,"%f",fmget/10000) ;
       HUAWEIDevValue.strhwApOrAblVoltage = fbuf;
       printf("A相电压:%s\r\n",fbuf);
       //printf("电表电压:%s\r\n",fbuf);
       pbuf += 4;

       //电流
       powertall ;
       memcpy(&powertall,pbuf,4);
       powertall = ntohl(powertall);
       fmget = (float)powertall;
       memset(fbuf,0x00,30);
       sprintf(fbuf,"%f",fmget/10000) ;
       HUAWEIDevValue.strhwAphaseCurrent = fbuf;
       printf("A相电流:%s\r\n",fbuf);
       //printf("电表电流:%s\r\n",fbuf);
       pbuf += 4;


       //频率
       mget = *(pbuf++);
       mget = (mget<<8) & 0xFF00;
       mget = mget | *(pbuf++);
       fmget = (float)mget;
       memset(fbuf,0x00,30);
       sprintf(fbuf,"%f",fmget/100) ;
       printf("电表频率:%s\r\n",fbuf);



     }
     break;

     case 0xb2://通信电源状态
     {
       unsigned short int mget;
       float fmget ;
       char fbuf[30];
       pbuf += 3;
       //交流电压
       mget = *(pbuf++);
       mget = (mget<<8) & 0xFF00;
       mget = mget | *(pbuf++);
       fmget = (float)mget;
       memset(fbuf,0x00,30);
       sprintf(fbuf,"%f",fmget/10) ;
       //HUAWEIDevValue.strhwApOrAblVoltage = fbuf;
       //printf("A相电压:%s\r\n",fbuf);
       //交流电流
       mget = *(pbuf++);
       mget = (mget<<8) & 0xFF00;
       mget = mget | *(pbuf++);
       mget = mget & 0x7FFF;
       fmget = (float)mget;
       memset(fbuf,0x00,30);
       sprintf(fbuf,"%f",fmget/10) ;
       //HUAWEIDevValue.strhwAphaseCurrent = fbuf;
       //printf("A相电流:%s\r\n",fbuf);
       //交流告警
       unsigned char malarm = *(pbuf++);
       if(malarm & 0x10)
       {
           printf("交流电流过高告警\r\n");
       }
       if(malarm & 0x04)
       {
           printf("A 相电压过高告警\r\n");
       }
       if(malarm & 0x02)
       {
           printf("A 相电压过低告警\r\n");
       }
       if(malarm & 0x01)
       {
           HUAWEIDevAlarm.hwAcInputL1FailAlarm = "1";
           printf("A 相电压缺相告警）\r\n");
       }
       else
           HUAWEIDevAlarm.hwAcInputL1FailAlarm = "0";

     }
     break;

    //查询水浸及烟雾状态
    case 0xc2:
    {
        pbuf += 3;
        unsigned char malarm;
        malarm = *(pbuf++);
        //水浸状态
        if(malarm == 0x31)
        {
          HUAWEIDevAlarm.hwWaterAlarmTraps = "1";
          printf("水浸高警\r\n");
        }
        else
            HUAWEIDevAlarm.hwWaterAlarmTraps = "0";
        //烟雾状态
        malarm = *(pbuf++);
        if(malarm == 0x31)
        {
          HUAWEIDevAlarm.hwSmokeAlarmTraps = "1";
          printf("烟雾高警\r\n");
        }
        else
            HUAWEIDevAlarm.hwSmokeAlarmTraps = "0";
    }
    break;
      default:
           break;
   }

}



void *atsRecvthread(void *param)
{ 
    int addr_len =sizeof(struct sockaddr_in);
    if((atsssockSrv = socket(AF_INET,SOCK_DGRAM,0))<0){  
        perror("socket");  
        exit(1);  
    }  

    int nRecvBufLen = 8*1024; 
    setsockopt(atsssockSrv, SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBufLen, sizeof( int ) );

    int nSendBufLen = 8*1024; 
    setsockopt(atsssockSrv, SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );
    struct timeval timeout={8,0};//3s
    setsockopt(atsssockSrv,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
	
    printf("StratsIP:%s\r\n",StrHWServer.c_str());
	
    bzero(&atsAddr,sizeof(atsAddr));  
    atsAddr.sin_family = AF_INET;  
    atsAddr.sin_addr.s_addr = inet_addr(StrHWServer.c_str());
    atsAddr.sin_port = htons(2929);  

    struct sockaddr_in Recvaddr = atsAddr;

    int DataLen  ;
    char *recvBuf = new char[1024*11];  
    while(1)
    { 
        memset(recvBuf,0,10240);
        DataLen = recvfrom(atsssockSrv,recvBuf,10240,0,(struct sockaddr *)&Recvaddr,(socklen_t*)&addr_len);
        if(DataLen > 0)
        {   
            atcDataRecv((unsigned char *)recvBuf,DataLen);
           //pthread_mutex_lock(&atsdataMutex);             
           //stratlasdata = recvBuf ;
           //printf("atlasdata:%s\r\n",stratlasdata.c_str());
           //pthread_mutex_unlock(&atsdataMutex);
        }

    }
    close(atsssockSrv);  

}


#endif


int atsInit(void)
{
    //CABINETTYPE:作为区分机柜类型，用于编译不同的代码
    //CABINETTYPE  1：华为（包括华为单门 双门等） 5：中兴; 6：金晟安; 7：爱特思 StrVersionNo
    #if(CABINETTYPE == 7) //爱特思

   pthread_mutex_init(&atsdataMutex,NULL);

   pthread_t m_atsRecvthread ;
   pthread_create(&m_atsRecvthread,NULL,atsRecvthread,NULL);

   pthread_t m_atsSendthread ;
   pthread_create(&m_atsSendthread,NULL,atsSendthread,NULL);

   #endif

   return 0 ;
}



