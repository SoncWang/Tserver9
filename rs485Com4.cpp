
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
#include "rs485Com4.h"
#include "rs485server.h"
#include "MyCritical.h"
#include <string>
#include <semaphore.h>
#include "tea.h"
#include "rtc.h"
#include "server.h"
#include "config.h"

/*Global definition*/
using namespace std;
CComPort *mComPort4 = NULL ;   	//RS485对应串口4,轮询电流电压传感器和电源板
CMyCritical Com4Cri;
CMyCritical Com4SendCri;


/*A fixed timer to polling the LOCKER */
void devPollingInit(void)
{
	pthread_t m_dev_polling_thread ;
	pthread_create(&m_dev_polling_thread,NULL,Dev_DataPollingthread,NULL);
}

void Rs485Com4Init(void)
{
	int temp = 0;

	mComPort4 = new CComPort();
	/*there is only ttysp for RS485 now in 9100 is ttymxc*/
	mComPort4->fd = mComPort4->openSerial((char *)"/dev/ttymxc4",9600);
	printf("ttymxc4 status = 0x%02x \r\n",mComPort4->fd);

	pthread_t m_ComPort4Thread ;
	pthread_create(&m_ComPort4Thread,NULL,ComPort4Thread,NULL);

	devPollingInit();
}


/*串口4的接收数据处理*/
void *ComPort4Thread(void *param)
{
   param = NULL;
   int buffPos=0;
   int len ;
   unsigned char buf[256] ;
   while(1)
   {
      	len = read(mComPort4->fd, buf+buffPos, 256);
	  	buffPos = buffPos+len;
	  	if(buffPos<5) continue;

	  	//CRC
	  	unsigned short int CRC = CRC16(buf,buffPos-2) ;
	  	if((((CRC&0xFF00) >> 8)!= buf[buffPos-2]) || ((CRC&0x00FF) != buf[buffPos-1]))
		{
		  //printf("CRC error\r\n");
 		  if(buffPos>=256) buffPos=0;

		  continue ;
	  	}

      	//printf("com4 len=%d\r\n",buffPos) ;
	  	/*debug the information*/
//	  	int j ;for(j=0;j<buffPos;j++)printf("0x%02x ",buf[j]);printf("\r\n");

	  	DealComm485(buf, buffPos, RS485_2);
	  	buffPos=0;

      	usleep(5000); //delay 5ms
   }

   return NULL ;
}

/* Com4发送数据--控制命令 */
// DDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + MBUS_OPT_CODE_H + MBUS_OPT_CODE_L + CRC(2)
UINT16 SendCom4RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code)
{
    Com4SendCri.Lock();
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
    //printf("Com4 Cdata:");
//	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort4->SendBuf(bytSend,len);
    Com4SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0;
}


/* Com3发送数据--读取外设数据 */
int SendCom4ReadReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 REFS_COUNT)
{
    Com4SendCri.Lock();
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

	//printf("Com4 Rdata:");
//	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort4->SendBuf(bytSend,len);
    Com4SendCri.UnLock();
	usleep(5000);	//delay 5ms
	return 0 ;
}



