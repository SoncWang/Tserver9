
#include <iostream>
#include <string>
#include <curl/curl.h>
//#include <curl/types.h>
#include <curl/easy.h>
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <pthread.h>

#include "HttpPost.h"
#include "registers.h"
using namespace std;


extern pthread_mutex_t PostGetMutex ;


//Base64 *pBase64 = new Base64();

struct MemoryStruct
{
    char *memory;
    size_t size;
    MemoryStruct()
    {
        memory = (char *)malloc(1);
        size = 0;
    }
    ~MemoryStruct()
    {
        free(memory);
        memory = NULL;
        size = 0;
    }
};

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory)
    {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}



int HttpPostjpeng(unsigned char *pjpengbuf,int jpenglen)
{
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if(CURLE_OK != res)
    {
        cout<<"curl init failed"<<endl;
        return 1;
    }

    CURL *pCurl ;
    pCurl = curl_easy_init();

    if( NULL == pCurl)
    {
        cout<<"Init CURL failed..."<<endl;
        return -1;
    }

    string url = "http://172.17.0.18:290/api/UpLoad/SendFlagCarPassList";
    string filename = "result.json";

    curl_slist *pList = NULL;
    pList = curl_slist_append(pList,"Accept: application/json");
    pList = curl_slist_append(pList,"content-type: application/json-patch+json");
    //pList = curl_slist_append(pList,"Referer: http://172.17.0.18:290/swagger/index.html?tdsourcetag=s_pcqq_aiomsg");
    pList = curl_slist_append(pList,"Accept-Language: zh-Hans-CN,zh-Hans;q=0.5");
    pList = curl_slist_append(pList,"Accept-Encoding: gzip, deflate");
    //pList = curl_slist_append(pList,"User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
    pList = curl_slist_append(pList,"Connection: Keep-Alive");
    //pList = curl_slist_append(pList,"Cache-Control: no-cache");
    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pList);

    curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str() ); //提交表单的URL地址

    curl_easy_setopt(pCurl, CURLOPT_HEADER, 0L);  //启用时会将头文件的信息作为数据流输
    curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);//允许重定向
    curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L);

    //将返回结果通过回调函数写到自定义的对象中
    MemoryStruct oDataChunk;
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &oDataChunk);
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L); //启用时会汇报所有的信息
    //post表单参数
    string strJsonData = "";
    //strJsonData +=  "{\"images\": \"" + pBase64->Encode(pjpengbuf,jpenglen) + "\","; //"{\"images\": \"string\",";
    strJsonData +=  "\"direction\": 0,";
    strJsonData +=  "\"recordno\": 0,";
    strJsonData +=  "\"flagnetroadid\": 0,";
    strJsonData +=  "\"roadid\": 0,";
    strJsonData +=  "\"flagid\": 0,";
    strJsonData +=  "\"posid\": 0,";
    strJsonData +=  "\"laneno\": 0,";
    strJsonData +=  "\"deviceid\": 0,";
    strJsonData +=  "\"vehspeed\": 0,";
    strJsonData +=  "\"vehbodycolorno\": 0,";
    strJsonData +=  "\"vehbodydeepno\": 0,";
    strJsonData +=  "\"vehtypeno\": 0,";
    strJsonData +=  "\"platetypeno\": 0,";
    strJsonData +=  "\"optime\": \"2019-05-29T03:10:17.192Z\",";
    unsigned int Intran = rand();
    stringstream ss;
    ss<<Intran; 
    string s1 = ss.str();
    strJsonData +=  "\"picserialno\": \"" + s1 + "\",";
    //strJsonData +=  "\"picserialno\": \"string\",";
    strJsonData +=  "\"vehplate\": \"string\",";
    strJsonData +=  "\"vehcolor\": \"string\"}";
 
    //printf("%s\r\n",strJsonData.c_str());

    //libcur的相关POST配置项
    curl_easy_setopt(pCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strJsonData.c_str());
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, strJsonData.size());


    res = curl_easy_perform(pCurl);

    long res_code=0;
    res=curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &res_code);

    if(( res == CURLE_OK ) && (res_code == 200 || res_code == 201))
    {
       /*
        FILE* fTmpMem = (FILE*)oDataChunk.memory;
        if (!fTmpMem) {

        }
        FILE *fp=fopen(filename.c_str(),"wb");
        if(!fp)
        {
            cout<<"open file failed";
            return -1;
        }

        fwrite(fTmpMem, 1, oDataChunk.size, fp);
        fclose(fp);
        */
        string strmemory = oDataChunk.memory;
        printf("%s\r\n",strmemory.c_str()) ;
        return true;
    }

    curl_slist_free_all(pList);
    curl_easy_cleanup(pCurl);
    curl_global_cleanup();

    return 0;
}


int HttpPostParm(string url,string &StrParmbuf,string strkey,int flag)
{
    pthread_mutex_lock(&PostGetMutex );

    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if(CURLE_OK != res)
    {
        cout<<"curl init failed"<<endl;
         pthread_mutex_unlock(&PostGetMutex );
        return 1;
    }

    CURL *pCurl ;
    pCurl = curl_easy_init();

    if( NULL == pCurl)
    {
        cout<<"Init CURL failed..."<<endl;
         pthread_mutex_unlock(&PostGetMutex );
        return -1;
    }

//    string url = "http://202.104.33.34:8085/toll/data";
//    string url = "http://172.17.2.32:290/api/UpLoad/SendDeviceStatus";
//	string url = StrServerURL; 
    string filename = "result.json";
 cout<<url.c_str()<<endl;
    curl_slist *pList = NULL;
    pList = curl_slist_append(pList,"Accept: application/json");
    pList = curl_slist_append(pList,"Content-Type:application/json");
    pList = curl_slist_append(pList,"Accept-Language: zh-Hans-CN,zh-Hans;q=0.5");
    pList = curl_slist_append(pList,"Accept-Encoding: gzip, deflate");
    pList = curl_slist_append(pList,"Connection: Keep-Alive");
    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pList);

    curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str() ); //提交表单的URL地址

    curl_easy_setopt(pCurl, CURLOPT_HEADER, 0L);  //启用时会将头文件的信息作为数据流输
    curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);//允许重定向
    curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L);

    //将返回结果通过回调函数写到自定义的对象中
    MemoryStruct oDataChunk;
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &oDataChunk);
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L); //启用时会汇报所有的信息
    //post表单参数
//    string strJsonData = "";
    string strJsonData = StrParmbuf;

	if(flag==HTTPPOST)
	{
	    //libcur的相关POST配置项
	    curl_easy_setopt(pCurl, CURLOPT_POST, 1L);
	    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strJsonData.c_str());
	    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, strJsonData.size());
	}
	else if(flag==HTTPGET)
	{
		//设置Get
	    curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, "GET");
	}
    //是否设置CURLAUTH_BASIC密码
    if(strkey != "")
    {
        curl_easy_setopt(pCurl, CURLOPT_USERPWD, strkey.c_str());
        curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    }
        //是否设置CURLAUTH_BASIC密码
       // curl_easy_setopt(pCurl, CURLOPT_USERPWD, "hdcam:hdcam");
       // curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);


    res = curl_easy_perform(pCurl);

    long res_code=0;
    res=curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &res_code);

    if(( res == CURLE_OK ) && (res_code == 200 || res_code == 201))
    {
       /*
        FILE* fTmpMem = (FILE*)oDataChunk.memory;
        if (!fTmpMem) {

        }
        FILE *fp=fopen(filename.c_str(),"wb");
        if(!fp)
        {
            cout<<"open file failed";
            pthread_mutex_unlock(&PostGetMutex );
            return -1;
        }

        fwrite(fTmpMem, 1, oDataChunk.size, fp);
        fclose(fp);
        */
        string strmemory = oDataChunk.memory;
        printf("HttpPost res: %s\r\n",strmemory.c_str()) ;
        StrParmbuf = strmemory ;
        //memcpy(pParmbuf,strmemory.c_str(),strmemory.length());
        //*parmlen=strmemory.length();
			
        curl_slist_free_all(pList);
        curl_easy_cleanup(pCurl);
       curl_global_cleanup();
    
        pthread_mutex_unlock(&PostGetMutex );
        return true;
    }
    else
    {
       curl_slist_free_all(pList);
       curl_easy_cleanup(pCurl);
       curl_global_cleanup();
       pthread_mutex_unlock(&PostGetMutex );
       return 0;

    }


}



