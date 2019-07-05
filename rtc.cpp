#include <stdio.h>  
#include <stdlib.h>  
#include <linux/rtc.h>  
#include <sys/ioctl.h>  
#include <sys/time.h>  
#include <sys/types.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <errno.h>  
#include <time.h> 
#include <string.h>
#include "rtc.h"


/************************************************
设置操作系统时间
参数:*dt数据格式为"2006-4-20 20:30:30"
调用方法:
char *pt="2006-4-20 20:30:30";
SetSystemTime(pt);
**************************************************/
int SetSystemTime(char *dt)
{
   struct rtc_time tm;
   struct tm _tm;
   struct timeval tv;
   time_t timep;
   sscanf(dt, "%d-%d-%d %d:%d:%d", &tm.tm_year,
          &tm.tm_mon, &tm.tm_mday,&tm.tm_hour,
          &tm.tm_min, &tm.tm_sec);
   _tm.tm_sec = tm.tm_sec;
   _tm.tm_min = tm.tm_min;
   _tm.tm_hour = tm.tm_hour;
   _tm.tm_mday = tm.tm_mday;
   _tm.tm_mon = tm.tm_mon - 1;
   _tm.tm_year = tm.tm_year - 1900;

   timep = mktime(&_tm);
   tv.tv_sec = timep;
   tv.tv_usec = 0;
   if(settimeofday (&tv, (struct timezone *) 0) < 0)
   {
     printf("Set system datatime error!\n");
     return -1;
   }
   system("hwclock  -w");  //写到硬时钟
   return 0;
}

string GetSystemTime(void)   
{   
    char gettime[20] ;
    memset(gettime,0x00,20) ;

    time_t timep;  
    struct tm *p;  
    time(&timep);
    p = localtime(&timep);  

    sprintf(gettime, "%04d%02d%02d%02d%02d%02d", p->tm_year + 1900,p->tm_mon + 1, p->tm_mday,p->tm_hour,p->tm_min, p->tm_sec);
    string strtime = gettime ;
    return strtime ;
}   







