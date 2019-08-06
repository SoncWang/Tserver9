#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

//#define VERSIONNO         "V001.0"   

#define QRSCAN      0x0001         //二维码扫�?
#define QRTOLL      0x0002         //支付扣费
#define QRWASHED    0x0003         //支付冲正

#define QRLED       0x0020        //更新LED屏幕信息
#define QRERRO      0x0030        //ERRO report

/*enum EM_ERROR_BACK
{
   NORMAL=0,                          //正常
//   CRC_ERROR=-1                     //crc错误
   CRC_ERROR=1 					
};

enum EM_STYLE
{
   EMWX=0,                      //微信  
   EMZB=1,                     //支付�?
   EMYB=2                      //粤通宝
};


enum EM_OPER_BACK
{
   NO_ERRORR=0,                     //正常
   MTC_TOLL_TIMEOUT=-1,             //车道支付超时
   MTC_TOLLQUERY_TIMEOUT=-2,        //车道支付查询超时
   MTC_WASHED_TIMEOUT=-3,           //车道退款超�?
   MTC_WASHEDQUERY_TIMEOUT=-4,      //车道退款查询超�?
   MTC_QR_TIMEOUT=-5,               //扫码超时
   MTC_HEART_ERR=-6,                //网络心跳异常
   MTC_PORTOPEN_ERR=-7,            //打开串口失败
   MTC_PORTCLOSE_ERR=-8,           //关闭串口失败
   MTC_PORT_TRANS_ERR=-9,          //串口数据通信异常
   MTC_UPLOAD_ERR=-10,             //流水上传失败
   MTC_WASHED_ERR=-11,             //退款失�?
   MTC_WASHEDQUERY_ERR=-12,        //退款查询失�?
   MTC_CMD_ERR=-13,                //非当前指令或不识�?
   MTC_BOX_ERR=-98,                //盒子返回的错误�?
   MTC_OTHER_ERR=-99               //其他错误
};*/


#pragma pack(push, 1)



#pragma pack(pop)


#endif
