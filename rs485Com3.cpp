
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
#include "comport.h"
#include "rs485Com3.h"
#include "rs485server.h"
#include "MyCritical.h"
#include <string>
#include <semaphore.h>
#include "tea.h"
#include "rtc.h"
#include "server.h"
#include "config.h"
#include "comserver.h"

/*Global definition*/
using namespace std;
CComPort *mComPort3 = NULL ;   	//RS485对应串口3,轮询电子锁
CMyCritical Com3Cri;
CMyCritical Com3SendCri;


/*A fixed timer to polling the LOCKER */
void lockerPollingInit(void)
{
	pthread_t m_locker_polling_thread ;
	pthread_create(&m_locker_polling_thread,NULL,Locker_DataPollingthread,NULL);
}

void Rs485Com3Init(void)
{
	int temp = 0;

	mComPort3 = new CComPort();
	/*there is only ttysp1 for RS485 now in A287，ttymxc3是串口4*/
	mComPort3->fd = mComPort3->openSerial((char *)"/dev/ttymxc3",9600);
	printf("ttymxc3 status= 0x%02x \r\n",mComPort3->fd);

	pthread_t m_ComPort3Thread ;
	pthread_create(&m_ComPort3Thread,NULL,ComPort3Thread,NULL);

	lockerDataMalloc();
	//lockerPollingInit();
}


/*串口3的接收数据处理*/
void *ComPort3Thread(void *param)
{
   param = NULL;
   int buffPos=0;
   int len ;
   unsigned char buf[256] ;
   while(1)
   {
      	len = read(mComPort3->fd, buf+buffPos, 256);
	  	buffPos = buffPos+len;
      	printf("com3 len=%d\r\n",buffPos) ;
	  	/*debug the information*/
	  	int j ;for(j=0;j<buffPos;j++)printf("0x%02x ",buf[j]);printf("\r\n");

		if (wait_msg == RS485_2_WAIT) // 注意是交叉测试
		{
			// 收到正常数据
			if ((buf[BUF_HEAD1] == FRAME_TEST_1) &&(buf[BUF_HEAD2] == FRAME_TEST_2) \
				&&(buf[2] == FRAME_TEST_3) &&(buf[3] == FRAME_TEST_4))
			{
				err_flag = TEST_OK;
			}
		}

	  	//DealComm485(buf, buffPos,RS485_1);
	  	buffPos=0;
      	usleep(5000); //delay 5ms
   }

   return NULL ;
}

/* Com3发送数据--控制命令 */
// DDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + MBUS_OPT_CODE_H + MBUS_OPT_CODE_L + CRC(2)
UINT16 SendCom3RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code)
{
    Com3SendCri.Lock();
    UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};

    int len=8;

    bytSend[MBUS_ADDR]        = Addr;
    bytSend[MBUS_FUNC]        = Func;
    bytSend[MBUS_REFS_ADDR_H] = (REFS_ADDR&0xFF00) >> 8;
    bytSend[MBUS_REFS_ADDR_L] =  REFS_ADDR&0x00FF;
    bytSend[MBUS_OPT_CODE_H] = (code&0xFF00) >> 8;     // bytSend[MBUS_OPT_CODE_L] =  code&0x00FF;
	bytSend[MBUS_OPT_CODE_L] =  code&0xFF;
	//CRC
    unsigned short int CRC = CRC16(bytSend,len-2) ;
    bytSend[len-2] = (CRC&0xFF00) >> 8;     //CRC
    bytSend[len-1] =  CRC&0x00FF;           //CRC
    printf("Com3 Cdata:");
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort3->SendBuf(bytSend,len);
    Com3SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0;
}


/* Com3发送数据--读取外设数据 */
int SendCom3ReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
    Com3SendCri.Lock();
    UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};

    int len=8;
    bytSend[MBUS_ADDR]        = Addr;
    bytSend[MBUS_FUNC]        = Func;	// 0x03
    bytSend[MBUS_REFS_ADDR_H] = (REFS_ADDR&0xFF00) >> 8;     // Register address
    bytSend[MBUS_REFS_ADDR_L] =  REFS_ADDR&0x00FF;
    bytSend[MBUS_REFS_COUNT_H] = (REFS_COUNT&0xFF00) >> 8;  // Register counter
    bytSend[MBUS_REFS_COUNT_L] =  REFS_COUNT&0x00FF;

    // CRC calculation
    unsigned short int CRC = CRC16(bytSend,len-2) ;
    bytSend[len-2] = (CRC&0xFF00) >> 8;     //CRC high
    bytSend[len-1] =  CRC&0x00FF;           //CRC low

	printf("Com3 Rdata:");
	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort3->SendBuf(bytSend,len);
    Com3SendCri.UnLock();
	usleep(5000);	//delay 5ms
	return 0 ;
}

#if 0
UINT16 SendCom3Test(char *buf,int len)
{
    Com3SendCri.Lock();
    UINT8 i,j,bytSend[128];

	memcpy(bytSend,buf,len);
	//CRC
    unsigned short int CRC = CRC16(bytSend,len);
    bytSend[len] = (CRC&0xFF00) >> 8;     //CRC
    bytSend[len+1] =  CRC&0x00FF;           //CRC
    printf("SendCom3Test data:");
	for(j=0;j<len+2;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort3->SendBuf(bytSend,len+2);
    Com3SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0;
}
#endif

//发送写数据寄存器 ,没有CRC
int SendCom3WriteReg(UINT16 Addr, UINT8 Func)
{
    Com3SendCri.Lock();
    UINT8 j;
	UINT8 bytSend[256];
	UINT8 datalen = 0;

	datalen = message_pack(Addr,Func,bytSend);

	printf("Com3 Wdata:");
	// debug测试打印
	for(j=0;j<datalen;j++) printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort3->SendBuf(bytSend,datalen);
    Com3SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0 ;
}



