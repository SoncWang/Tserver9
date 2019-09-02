
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
#include "comport.h"



CComPort::CComPort(void)
{
    fd = 0 ;
    comlen = 0 ;
    RecvStart = 0 ;
    m_combuf = new unsigned char[2048] ;
}


CComPort::~CComPort(void)
{
   if(m_combuf)
   {
     delete []m_combuf ;
   }
}


int CComPort::openSerial(char *cSerialName,int Baud)
{
    int iFd;

    struct termios opt; 

    iFd = open(cSerialName, O_RDWR | O_NOCTTY);                        
    if(iFd < 0) {
        perror(cSerialName);
        return -1;
    }

    tcgetattr(iFd, &opt);      

    //cfsetispeed(&opt, B57600);
    //cfsetospeed(&opt, B57600);
 
    switch(Baud)
    {
       case 9600:
            {
              cfsetispeed(&opt, B9600);
              cfsetospeed(&opt, B9600);
            }
            break ;
       case 38400:
            {
              cfsetispeed(&opt, B38400);
              cfsetospeed(&opt, B38400);
            }
       case 115200:
            {
              cfsetispeed(&opt, B115200);
              cfsetospeed(&opt, B115200);
            }
            break ;
       default:
            {
              cfsetispeed(&opt, B9600);
              cfsetospeed(&opt, B9600);
            }
            break ;
    }

    
    /*
     * raw mode
     */
    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE   |   PARENB);
    opt.c_cflag   |=   CS8;

    /*
     * 'DATA_LEN' bytes can be read by serial
     */
    opt.c_cc[VMIN]   =   1;                                      
    opt.c_cc[VTIME]  =   0;  //0 yi zhi deng

    if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
        return   -1;
    }


    return iFd;
}


int CComPort::SendBuf(unsigned char *buf,int len)
{
   write(fd, buf, len);
   return len ;
}



 

