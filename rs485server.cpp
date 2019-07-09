
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
#include "rs485server.h"
#include "MyCritical.h"
#include <string>
#include <semaphore.h>
#include "tea.h"
#include "rtc.h"
#include "server.h"


using namespace std;
CComPort *mComPort4 = NULL;
CMyCritical Com4Cri;
CMyCritical Com4SendCri;

void *ComPort4Thread(void *param)
{
   param = NULL;
   while(1)
   {
      usleep(5000);//delay 10ms
   }

}

void rs485init(void)
{
   mComPort4 = new CComPort();

   mComPort4->fd = mComPort4->openSerial((char *)"/dev/ttymxc4",9600);
   printf("rs485 status");
   printf("0x%02x \r\n",mComPort4->fd);

   pthread_t m_ComPort4Thread ;
   pthread_create(&m_ComPort4Thread,NULL,ComPort4Thread,NULL);
}


//发送写遥控寄存�?ADDR + FUNC + REFS_ADDR_H + REFS_ADDR_L + MBUS_OPT_CODE_H + MBUS_OPT_CODE_L + CRC(2)
UINT16 SendCom4RCtlReg(UINT8 Addr, UINT8 Func, UINT16 REFS_ADDR, UINT16 code)
{
    Com4SendCri.Lock();
    UINT8 i,j,bytSend[8]={0x00,0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00};

    int len=8;

    bytSend[MBUS_ADDR]        = Addr;
    bytSend[MBUS_FUNC]        = Func;       //写寄存器
    bytSend[MBUS_REFS_ADDR_H] = (REFS_ADDR&0xFF00) >> 8;     //寄存器起始地址高位
    bytSend[MBUS_REFS_ADDR_L] =  REFS_ADDR&0x00FF;           //寄存器起始地址低位
    bytSend[MBUS_OPT_CODE_H] = (code&0xFF00) >> 8;     //操作码高�?    bytSend[MBUS_OPT_CODE_L] =  code&0x00FF;           //操作码低�?
	bytSend[MBUS_OPT_CODE_L] =  code&0xFF;
	//CRC校验
    unsigned short int CRC = CRC16(bytSend,len-2) ;
    bytSend[len-2] = (CRC&0xFF00) >> 8;     //CRC高位
    bytSend[len-1] =  CRC&0x00FF;           //CRC低位

	for(j=0;j<len;j++)printf("0x%02x ",bytSend[j]);printf("\r\n");

	mComPort4->SendBuf(bytSend,len);

	//memcpy(LastSendBuf,bytSend,len);
	//LastSendLen=len;
	//free(bytSend);

    Com4SendCri.UnLock();
	usleep(5000);//delay 5ms
	return 0;
}



