/********************************************************************************
*  版权所有: 广东利通科技
*
*  文件名:    ydn23.c
*
*
*  当前版本: 0.1
*
*  作者:   Jerry
*
*  日期:  20191028
*
*  描述: 电子锁的协议,电总协议格式
*
********************************************************************************/

/*
*********************************************************************************************************
*                                         包含文件
*********************************************************************************************************
*/
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
#include <assert.h>
#include "comport.h"
#include "rs485server.h"
#include "MyCritical.h"
#include <string>
#include <semaphore.h>
#include "tea.h"
#include "rtc.h"
#include "server.h"
#include "config.h"
#include "ydn23.h"


/*Global definition*/
using namespace std;



/*根据lenth计算电总协议的LCHKSUM*/
/*发送的时候计算*/
UINT16 lchkSumCalc(UINT16 len_value)
{
	UINT16 re_value = 0;

	/*先对D0D1D2D3+D4D5D6D7+D8D9D10D11求和*/
	re_value = ((len_value&0x0F) + ((len_value>>4)&0x0F) + ((len_value>>8)&0x0F));
	re_value = (~(re_value&0x0F))+1;	// 再模16余数取反加1
	re_value = re_value&0x0F;

	return re_value;
}


/*根据lenth计算电总协议的CHECKSUM*/
UINT16 checkSumCalc(UINT8 *buffer, UINT8 len)
{
	UINT16 re_value = 0;
	UINT8 i = 0;

	/*先对所有除SIO之外的求和*/
	for (i = 0; i<len; i++)
	{
		re_value = re_value+buffer[i];
	}
	// 模65535的余数+1
	re_value = (~(re_value&0xFFFF))+1;	// 再模65536余数取反加1

	return re_value;
}


 UINT8 char_to_ascii(UINT8 x)
 {
	 UINT8 re_value = 0;

	 assert(((x >= 0) && (x <= 9)) ||((x >= 0x0A) && (x <= 0x0F)));

	 if ((x >= 0) && (x <= 9))
	 {
		 re_value = x+0x30;  // 数字加上0x30，即0的ascii码
	 }
	 else if ((x >= 0x0A) && (x <= 0x0F))
	 {
		 re_value =(x-0x0A)+65;  // A的ASCII码是65
	 }

	 return re_value;
 }

 /*将一个hex转换成2个ascii字符，如0x4B-> 0x34, 0x42*/
  void hex2_to_ascii(UINT8 x, UINT8 * buffer)
 {
	 buffer[1] = (x>>4)&0x0F;
	 buffer[1] = char_to_ascii(buffer[1]);

	 buffer[0] = x&0x0F;
	 buffer[0] = char_to_ascii(buffer[0]);
 }

 /*将一个16位的hex转换成4个ascii字符，如0x4B-> 0x34, 0x42*/
  void hex4_to_ascii(UINT16 x, UINT8 * buffer)
 {
	 buffer[3] = (x>>12)&0x0F;
	 buffer[3] = char_to_ascii(buffer[3]);

	 buffer[2] = (x>>8)&0x0F;
	 buffer[2] = char_to_ascii(buffer[2]);

	 buffer[1] = (x>>4)&0x0F;
	 buffer[1] = char_to_ascii(buffer[1]);

	 buffer[0] = x&0x0F;
	 buffer[0] = char_to_ascii(buffer[0]);
 }

 // 把几个ASCII码合成16进制
 UINT8 ascii_to_char(UINT8 x)
 {
	 UINT8 re_value = 0;

	 assert(((x >= 0x30) && (x <= 0x39)) ||((x >= 65) && (x <= 70)) ||((x >= 97) && (x <= 102)));  // ascii码要在0~9,A~F,a~f之间

	 if ((x >= 0x30) && (x <= 0x39))
	 {
		 re_value = x-0x30;  // 数字加上0x30，即0的ascii码
	 }
	 else if ((x >= 65) && (x <= 70))
	 {
		 re_value =(x-65)+0x0A;  // A的ASCII码是65
	 }
	 else if ((x >= 97) && (x <= 102))
	 {
		 re_value =(x-97)+0x0A;  // a的ASCII码是97
	 }
	 re_value = re_value&0x0F;	 // 高4位清0

	 return re_value;
 }

 /*将2个ascii字符转换成一个8位的hex，如 0x34, 0x42->0x4B*/
 // 传输时 高位在前,从而buffer0应该是高位
  UINT8 ascii_to_hex2(UINT8 * buffer)
 {
	 UINT8 re_value=0;
	 const UINT8 cmp2[2] = {0x20,0x20};

	 if ((memcmp(buffer, cmp2, 2)) == 0)
	 {
		 return 0xFF;		 // 当全部为空格时,返回0xFFFF
	 }

	 re_value = ascii_to_char(buffer[0]);
	 re_value = (re_value<<4) |(ascii_to_char(buffer[1]));

	 return re_value;
 }

 /*将4个ascii字符转换成一个16位的hex，如 0x34, 0x42->0x4B*/
 // 传输时 高位在前,从而buffer0应该是高位
  UINT16 ascii_to_hex4(UINT8 * buffer)
 {
	 UINT16 re_value = 0;
	 const UINT8 cmp2[4] = {0x20,0x20,0x20,0x20};

	 if ((memcmp(buffer, cmp2, 4)) == 0)
	 {
		 return 0xFFFF; 	 // 当全部为空格时,返回0xFFFF
	 }

	 re_value = ascii_to_char(buffer[0]);
	 re_value = (re_value<<4) |(ascii_to_char(buffer[1]));
	 re_value = (re_value<<4) |(ascii_to_char(buffer[2]));
	 re_value = (re_value<<4) |(ascii_to_char(buffer[3]));

	 return re_value;
 }


#if 0
/******************************************************************************
 * 函数名:	comm_UPSParam_ANALYSE
 * 描述: 		UPS概况数据解析
 *            -
 * 输入参数:
 * 输出参数:
 * 返回值:
 *
 * 作者:JERRY
 * 创建日期:
 ******************************************************************************/
 #define UPS_PARAM_RES_LEN		72			 // 0x42返回数据长度72
 #define VOLT_AIN_POS				15			// 15字节开始是A相输入电压数值
 #define VOLT_BIN_POS				(VOLT_AIN_POS+4)			// 19字节开始是B相电压数值
 #define VOLT_CIN_POS				(VOLT_BIN_POS+4)			// 23字节开始是C相电压数值

 #define VOLT_AOUT_POS			(VOLT_CIN_POS+4)			// 27字节开始是A相电压数值
 #define VOLT_BOUT_POS			(VOLT_AOUT_POS+4)		// 31字节开始是B相电压数值
 #define VOLT_COUT_POS			(VOLT_BOUT_POS+4)		// 35字节开始是C相电压数值

 #define AMP_AOUT_POS			(VOLT_COUT_POS+4)			// 27字节开始是A相电压数值
 #define AMP_BOUT_POS			(AMP_AOUT_POS+4)		// 31字节开始是B相电压数值
 #define AMP_COUT_POS			(AMP_BOUT_POS+4)		// 35字节开始是C相电压数值

 #define FREQ_OUT_POS			(AMP_COUT_POS+8)		// 35字节开始是C相电压数值

void comm_UPSParam_ANALYSE(void)
{
	INT16U* pointer = &UPSParams.ups_in.volt_Ain;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_PARAM_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_PARAM_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_PARAM_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_AIN_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_BIN_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_CIN_POS);

		pointer = &UPSParams.ups_out.volt_Aout;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_AOUT_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_BOUT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_COUT_POS);

		pointer = &UPSParams.ups_out.amp_Aout;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_AOUT_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_BOUT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_COUT_POS);

		// 输出频率
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FREQ_OUT_POS);
	}
}
#endif

/***********************************************************************
函数名:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

输入参数:		start_reg:	读取的寄存器开始地址
              reg_num:		读取的寄存器个数

输出参数:		无.

返回值:		无.

功能说明:              读取寄存器处理
***********************************************************************/
UINT16 comm_pack_ydn(UINT16 addr,UINT16 cid2,UINT8 info_len,UINT8 *cmd_buf,UINT8 *send_buf)
{
	UINT8 c_value[4] = {0,0,};
	USP_LENGTH_BITS uspLenthData;
	UINT16 temp = 0;
	UINT16 send_len = 0;
	UINT8 *pbuf = send_buf;	//buf->pTxBuf;
	UINT8 i = 0;

	*pbuf = YDN_SOI;	// 电总协议0x7E开始
	send_len++;

	hex2_to_ascii(YDN_VER, c_value);
	*(pbuf + send_len) = c_value[1]; // 通讯版本号0x10
	send_len++;
	*(pbuf + send_len) = c_value[0];
	send_len++;
	//send_len += 2;

	hex2_to_ascii(addr, c_value);			// 设备地址
	*(pbuf + send_len) = c_value[1];
	send_len++;
	*(pbuf + send_len) = c_value[0];
	send_len++;
	//send_len += 2;

	hex2_to_ascii(YDN_CID1, c_value);			// cid1
	*(pbuf + send_len) = c_value[1];
	send_len++;
	*(pbuf + send_len) = c_value[0];
	send_len++;
	//send_len += 2;

	hex2_to_ascii(cid2, c_value);			// CID2
	*(pbuf + send_len) = c_value[1];
	send_len++;
	*(pbuf + send_len) = c_value[0];
	send_len++;
	//send_len += 2;

	/*LENTH的计算*/
	uspLenthData.lenth_bits.lenid = 2*info_len;
	uspLenthData.lenth_bits.lchksum = lchkSumCalc(uspLenthData.lenth_bits.lenid) &0x0F;
	hex4_to_ascii(uspLenthData.lenth_word, c_value);
	*(pbuf + send_len) = c_value[3];
	send_len++;
	*(pbuf + send_len) = c_value[2];
	send_len++;
	*(pbuf + send_len) = c_value[1];
	send_len++;
	*(pbuf + send_len) = c_value[0];
	send_len++;
	//send_len += 4;

	if (info_len > 0)
	{
		for (i=0; i<info_len; i++)
		{
			temp = cmd_buf[i];
			hex2_to_ascii(temp, c_value);
			*(pbuf + send_len) = c_value[1];
			send_len++;
			*(pbuf + send_len) = c_value[0];
			send_len++;
		}
	}

	/*都已经转成了ASCII码*/
	temp = checkSumCalc(pbuf+1, send_len-1);	//SOI不算在内
	hex4_to_ascii(temp, c_value);
	*(pbuf + send_len) = c_value[3];
	send_len++;
	*(pbuf + send_len) = c_value[2];
	send_len++;
	*(pbuf + send_len) = c_value[1];
	send_len++;
	*(pbuf + send_len) = c_value[0];
	send_len++;
	//send_len += 4;

	*(pbuf + send_len) = YDN_EOI;
	send_len++;

	return send_len;
}



