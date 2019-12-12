#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "registers.h"
#include "HttpPost.h"
#include "zte.h"
#include "base_64.h"
#include "tea.h"


using namespace std;

//中兴机柜ID
string zteTempDevID[2] ;
string zteLockDevID[4] ;
string zteTemperature[2];
string zteHumidity[2];

extern bool jsonzteDevReader(char* jsonstr, int len);//读取中兴机柜信息
extern bool jsonzteTempReader(char* jsonstr, int len,int Index);//读取中兴温湿度


UINT8 locker_cmd_pack(UINT8 msg_type,UINT8 door_pos,UINT16 door_cmd,UINT8 *buf)
{
	/*取得目标串口对应的发送缓存*/
	UINT8 *pbuf = buf;	//buf->pTxBuf;
	UINT8 len = 0;
	UINT8 crc_cal[2];
	//case DOOR_POLL_CMD:
		pbuf[len++] = 0x7E;
		pbuf[len++] = door_pos;
		pbuf[len++] = 1;
		pbuf[len++] = 0;
		pbuf[len++] = 0;
		pbuf[len++] = 0x00;
		pbuf[len++] = 0x02;
		pbuf[len++] = (door_cmd>>8)&0xFF;
		pbuf[len++] = door_cmd&0xFF;
		CalulateCRCbySoft(&buf[1],len-1,crc_cal);
		pbuf[len++] = crc_cal[1];	// 高位要在前
		pbuf[len++] = crc_cal[0];
		pbuf[len++] = 0x7E;
		pbuf[len++] = 0x7E;
		 printf("Locker cmd begins\r\n ");
		for(int j=0;j<len;j++) printf("0x%02x ",pbuf[j]);printf("\r\n");
	return len;
}


void locker_process_zte(UINT8 )
{

}


void *zte_HTTP_thread(void *param)
{
    zteTempDevID[0] == "";
    zteTempDevID[1] == "";
    zteLockDevID[0] = zteLockDevID[1] = zteLockDevID[2] = zteLockDevID[3] = "";

    string mStrdata = "";
    string mStrUser = "admin";
    string mStrkey = "admin";
    sleep(5);
    HttpPostParm("http://128.8.82.246/jscmd/objs",mStrdata,"",HTTPGET,mStrUser,mStrkey);
    jsonzteDevReader((char *)(mStrdata.c_str()),mStrdata.size());
	Base64 Base64Cal;
	UINT8 byteSend[30]={0x00,};
	UINT8 lock_len = 0;

    while(1)
    {
        sleep(5);
        //设备列表若未获取到，则再获取
        if(zteTempDevID[0] == "")
        {
            mStrdata = "";
            HttpPostParm("http://128.8.82.246/jscmd/objs",mStrdata,"",HTTPGET,mStrUser,mStrkey);
           if(mStrdata != "")
              jsonzteDevReader((char *)(mStrdata.c_str()),mStrdata.size());
        }
        else
        {
            mStrdata = "";
            string tempid1,tempid2;
            if(atoi(zteTempDevID[0].c_str()) > atoi(zteTempDevID[1].c_str()))
            {
                tempid1 =  zteTempDevID[1];
                tempid2 =  zteTempDevID[0];
            }
            else
            {
                tempid2 =  zteTempDevID[1];
                tempid1 =  zteTempDevID[0];
            }

           //获取温湿度1
            tempid1 = "http://128.8.82.246/jscmd/objdataquery?objId="+tempid1+"&mId=0";
            HttpPostParm(tempid1,mStrdata,"",HTTPGET,mStrUser,mStrkey);
            if(mStrdata != "")
              jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),0);
            //获取温湿度2
            tempid2 = "http://128.8.82.246/jscmd/objdataquery?objId="+tempid2+"&mId=0" ;
            HttpPostParm(tempid2,mStrdata,"",HTTPGET,mStrUser,mStrkey);
            if(mStrdata != "")
              jsonzteTempReader((char *)(mStrdata.c_str()),mStrdata.size(),1);

		   //locker_process_zte();
           //获取锁信息
           mStrdata = "";
		   string lockerid[4];
		   string locker_data[4];	// data部分
		   //lock_len = locker_cmd_pack(DOOR_OPEN_CMD,FRONT_DOOR,DOOR_OPEN,byteSend);
		   lock_len = locker_cmd_pack(DOOR_POLL_CMD,FRONT_DOOR,DOOR_POLL_CMD,byteSend);

			locker_data[0]= Base64Cal.Encode(byteSend, lock_len);
			printf("lockerSecret = %s\r\n",locker_data[0].c_str());

			lockerid[0] = "http://128.8.82.246/jscmd/serialsend";
			//mStrdata = "{\"jsonrpc\":\"2.0\",\"method\": \"POST_METHOD\",\"id\": \"3\",\"params\": \"POST_METHOD\",}";
				//"{"jsonrpc":"2.0","method":"POST_METHOD","id":"3","params":{"objid":zteLockDevID[0],"data":locker_data[0],"endchar":""}}";
			//HttpPostParm(lockerid[0],mStrdata,"",HTTPPOST,mStrUser,mStrkey);
		   //lockerid[0] = "http://128.8.82.246/jscmd/serialsend;
		}
	}
	return 0;
}

int zteinit(void)
{
    pthread_t m_zte_HTTP_Get ;
    pthread_create(&m_zte_HTTP_Get,NULL,zte_HTTP_thread,NULL);

    return 0 ;
}

