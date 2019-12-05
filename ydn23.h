/********************************************************************************
*  版权所有: 广东利通科技
*
*  文件名:    ydn23.h
*
*
*  当前版本: 0.1
*
*  作者:   Jerry(蒋建荣)
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

#ifndef _YDN23_H
#define _YDN23_H

/*状态有效为1, 无效为0*/
typedef union{
	struct
	{
		UINT16 lenid:12;
		UINT16 lchksum:4;
	}lenth_bits;
	/* 必须以数组形式定义,否则连续定义2个变量会放在同一个地址*/
	UINT16 lenth_word;
}USP_LENGTH_BITS;


#define   YDN_SOI		0x7E
// ver-0x10
#define   YDN_VER		0x10
// ver-0x80
#define   YDN_CID1		0x80

#define   YDN_CID2_AUTH		0x48	// 确权
#define   YDN_CID2_CTRL		0x49
#define   YDN_CID2_POLL		0xED

#define YDN_CMD_GRP_CTRL	0xF1	// 门锁设置，其COMMAND GROUP = F1H
#define YDN_CMD_TYPE_CTRL	0xED	// 门锁设置，其COMMAND TYPE = EDH
#define YDN_CMD_DOOR_OPEN	0x01	// 单一放行，1: 开门 0：不操作

#define   YDN_EOI			0x0D



/********************************************************************************************************
*                                          ydn23模块接口函数
********************************************************************************************************/
UINT16 lchkSumCalc(UINT16 len_value);
UINT16 checkSumCalc(UINT8 *buffer, UINT8 len);
void hex2_to_ascii(UINT8 x, UINT8 * buffer);
void hex4_to_ascii(UINT16 x, UINT8 * buffer);
UINT8 ascii_to_hex2(UINT8 * buffer);
UINT16 ascii_to_hex4(UINT8 * buffer);
UINT8 char_to_ascii(UINT8 x);
UINT8 ascii_to_char(UINT8 x);
UINT16 comm_pack_ydn(UINT16 addr,UINT16 cid2,UINT8 info_len,UINT8 *cmd_buf,UINT8 *send_buf);


#endif
/*********************************************************************************************************
**                            文件结束
********************************************************************************************************/




