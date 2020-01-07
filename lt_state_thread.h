#ifndef __LT_STATE_THREAD_h__
#define __LT_STATE_THREAD_h__

#include <string>

using namespace std;//引入整个名空间

//控制器运行状态结构体
typedef struct vmctl_state_struct
{
	unsigned long TimeStamp; 		//状态获取时间戳
	bool Linked;					//连接状态 0:断开 1：连接

	string strhostname;
	string strcpuRate;
	string strcpuTemp;
	string strmenTotal;
	string strmenUsed;
	string strmenRate;
}VMCONTROL_STATE;

void init_lt_status();
void init_lt_state_struct();


#endif

