#include <stdio.h>
#include <stdlib.h>
#include <evhttp.h>
#include <event.h>
#include <string.h>
#include "event2/http.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http_struct.h"
#include "event2/http_compat.h"
#include "event2/util.h"
#include "event2/listener.h"
#include <pthread.h>
#include "event2/http.h"
#include "tea.h"
#include <unistd.h> 

#define BUF_MAX 1024*16
#define JSON_LEN 10*1024

extern bool jsonStrReader(char* jsonstrin, int lenin, char* jsonstrout, int *lenout);

int WritepUpdata(unsigned char *pDateBuf,int pDateLen)
{
    FILE* fdd ;
    fdd	= fopen("/opt/tranter", "wb");
    if(fdd == NULL)
    {
       return 1 ;
    }

    fwrite(pDateBuf,pDateLen, 1, fdd);
    fflush(fdd);
    fclose(fdd);
    
    return 0 ;

}


//解析post请求数据
void get_post_message(char *buf, struct evhttp_request *req)
{
    size_t post_size = 0;

    post_size = evbuffer_get_length(req->input_buffer);//获取数据长度
    if (post_size <= 0)
    {
        printf("post msg is empty!\n");
        return;
    }
    else
    {
        size_t copy_len = post_size > BUF_MAX ? BUF_MAX : post_size;
        printf("post len:%d, copy_len:%d\n",post_size,copy_len);
        //updata
        unsigned char *pbuf = (unsigned char *)(evbuffer_pullup(req->input_buffer,-1));
        if((pbuf[0] == 0x12) && (pbuf[1] == 0x34) && (pbuf[2] == 0x56) && (pbuf[3] == 0x78)) 
        {
           unsigned int getlen ;
           memcpy(&getlen,pbuf+4,4);
           if(getlen == post_size)
           {
              unsigned short int IntCRC;
              unsigned short int getCrc = GetCrc(pbuf,getlen-2) ;
              memcpy(&IntCRC,pbuf+getlen-2,2);
              if(IntCRC == getCrc)
              {
                 system("mv tranter tranter1") ;
                 printf("start updata\r\n");
                 WritepUpdata(pbuf+8,getlen-16);

                 struct evbuffer *retbuff = NULL;
                 retbuff = evbuffer_new();
                 if(retbuff == NULL)
                 {
                   printf("retbuff is null.");
                   //system("reboot") ;
                   //return;
                 }
                 else
                 {
                   evbuffer_add_printf(retbuff,"Updata LTKJ Controller device! Don't turn off the power in 10 seconds! device is auto restart! Thank you!");
                   evhttp_send_reply(req,HTTP_OK,"Client",retbuff);
                 }

                 sleep(5);
                 system("chmod 777 tranter") ;
                 printf("chmod 777 tranter\r\n");
                 sleep(1);
                 system("reboot") ;

              }
              else
              {
                  printf("updata CRC erro %d %d\r\n",IntCRC,getCrc) ;
              }

           }
        }

        memcpy(buf, evbuffer_pullup(req->input_buffer,-1), copy_len);
        buf[post_size] = '\0';
//        printf("post msg:%s\n",buf);
    }
}



void http_handler_post_msg(struct evhttp_request *req,void *arg)
{
	char *jsonPack=(char*)malloc(JSON_LEN);
	if(jsonPack==NULL)
	{
		printf("http_handler_post_msg jsonPack malloc error!\n");
		return;
	}
	int jsonPackLen;
	
    if(req == NULL)
    {
        printf("====line:%d,%s\n",__LINE__,"input param req is null.");
        return;
    }

    char buf[BUF_MAX] = {0};
    get_post_message(buf, req);
    if(buf == NULL)
    {
        printf("====line:%d,%s\n",__LINE__,"get_post_message return null.");
        return;
    }
    else
    {
        printf("====line:%d,request data:%s,len: %d\n",__LINE__,buf,strlen(buf));
    }

	//解析发来的JSON请求，打包请求数据
	memset(jsonPack,0,JSON_LEN);
	jsonPackLen=0;
	jsonStrReader(buf,strlen(buf),jsonPack,&jsonPackLen);
	printf("jsonPack len:%d out:%s\n",jsonPackLen,jsonPack);
	
    //回响�?    
    struct evbuffer *retbuff = NULL;
    retbuff = evbuffer_new();
    if(retbuff == NULL)
    {
        printf("retbuff is null.");
        return;
    }
//    evbuffer_add_printf(retbuff,"Receive post request,Thanks for the request!");
    evbuffer_add_printf(retbuff,jsonPack);
    evhttp_add_header(evhttp_request_get_output_headers(req),"Content-Type", "application/json");
	evhttp_send_reply(req,HTTP_OK,"Client",retbuff);
    evbuffer_free(retbuff);
	free(jsonPack);
}

void *HttpServerhread(void *param)
{
    struct evhttp *http_server = NULL;
    short http_port = 8081;
    char *http_addr = "0.0.0.0";

    event_init();
    http_server = evhttp_start(http_addr,http_port);
    if(http_server == NULL)
    {
        printf("http server start failed.");
        return 0;
    }

    //设置请求超时时间(s)
    evhttp_set_timeout(http_server,10);
    evhttp_set_cb(http_server,"/Device/RemoteCtl",http_handler_post_msg,NULL);


    event_dispatch();
    evhttp_free(http_server);

    return 0;
}


int HttpInit(void)
{
   pthread_t m_HttpServerhread ;
   pthread_create(&m_HttpServerhread,NULL,HttpServerhread,NULL);
   return 0 ;
}



