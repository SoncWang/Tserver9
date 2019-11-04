#ifndef __ERR_COM_h__
#define __ERR_COM_h__



/*****************  通信报警定义          *********************/
#define MAX_ERR_CNT		3
#define MAX_COM_NUM		4

#define ERR_COM1		0
#define ERR_COM2		1
#define ERR_COM3		2
#define ERR_COM4		3

/*the struct definition of the LOCKER*/
typedef struct err_com_struct
{
	UINT16 err_cnt[MAX_COM_NUM];	// 指示通信中断次数
	UINT16 err_flag[MAX_COM_NUM];	// 中断为1,成功为0
}ERR_COM_PARAMS;
/*****************  通信报警结束          *********************/


UINT16 Comm_Err_Flag_Get(UINT16 seq);
UINT16 Comm_Err_Process(UINT16 seq,UINT8 *wait_flag, UINT8 wait_msg);


#endif

