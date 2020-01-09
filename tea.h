#ifndef __TEA_h__
#define __TEA_h__

#include <stdio.h>
#include <stdint.h>

//CRC
unsigned short GetCrc(unsigned char *buf,int len) ;
//加密函数
void encrypt (uint32_t* v) ;
//解密函数
void decrypt (uint32_t* v) ;

unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen);
void CalulateCRCbySoft(unsigned char *pucData,unsigned char wLength,unsigned char *pOutData);
unsigned char CRC_sum(unsigned char *puchMsg , unsigned short usDataLen);


#endif
