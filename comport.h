#ifndef __COMPORT_H__
#define __COMPORT_H__

void cominit(void) ;
class CComPort
{
public:
       CComPort(void);
       ~CComPort(void);
      
       int openSerial(char *cSerialName,int Baud);
       int SendBuf(unsigned char *buf,int len) ;

       int fd ;
       unsigned short int comlen ;
       unsigned char *m_combuf ; 
       unsigned char RecvStart ;
       

};



#endif
