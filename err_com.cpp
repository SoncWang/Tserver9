
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "comserver.h"
#include "MyCritical.h"
#include <string>
#include <semaphore.h>
#include "rs485server.h"
#include "arpa/inet.h"
#include <sys/time.h>
#include "snmp.h"
#include "err_com.h"

using namespace std;

ERR_COM_PARAMS err_ind = {{0,0,0},{0,0,0}};

/* 处理通信错误的函数，当连续3次读写数据没反应后,判断通信已经中断*/
// seq用来区分是哪个通信口，最多支持4个,2个485,2个232
UINT16 Comm_Err_Process(UINT16 seq,UINT8 *wait_flag, UINT8 wait_msg)
{
	switch(*wait_flag)
	{
	// 0可以兼容WAIT_NONE和WAIT_COM_NONE
	// 只要收到1次回应，就认为是正常了
	case (0):
		err_ind.err_cnt[seq] = 0;
		err_ind.err_flag[seq] = 0;
		break;

	default:
		if (*wait_flag == wait_msg)
		{
			// 错误要判断3次
			err_ind.err_cnt[seq]++;
			if (err_ind.err_cnt[seq] >= MAX_ERR_CNT)
			{
				err_ind.err_flag[seq] = 1;
			}
		}
		break;
	}
	return err_ind.err_flag[seq];
}


UINT16 Comm_Err_Flag_Get(UINT16 seq)
{
	return err_ind.err_flag[seq];
}

/********************** 处理通信错误的函数，完 ***********************/



