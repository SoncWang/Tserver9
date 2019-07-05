#include <stdio.h>
#include <stdlib.h>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <string> 
#include <sstream>
#include <assert.h>
#include <semaphore.h>  
#include "http.h"
#include "MyCritical.h"
#include "Protocol.h"
#include "comserver.h"

using namespace std; 

sem_t httpsem;  
string Sendhttp ;
CMyCritical HttpCri;

int httpsend(string strhttp);

BIO *sbio, *out;

char* join(char *s1, char *s2);

std::string UrlEncode(const std::string& szToEncode)
{
    std::string src = szToEncode;
    char hex[] = "0123456789ABCDEF";
    string dst;
 
    for (size_t i = 0; i < src.size(); ++i)
    {
        unsigned char cc = src[i];
        if (isascii(cc))
        {
            if (cc == ' ')
            {
                dst += "%20";
            }
            else
                dst += cc;
        }
        else
        {
            unsigned char c = static_cast<unsigned char>(src[i]);
            dst += '%';
            dst += hex[c / 16];
            dst += hex[c % 16];
        }
    }
    return dst;
}
 
 
std::string UrlDecode(const std::string& szToDecode)
{
    std::string result;
    int hex = 0;
    for (size_t i = 0; i < szToDecode.length(); ++i)
    {
        switch (szToDecode[i])
        {
        case '+':
            result += ' ';
            break;
        case '%':
            if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
            {
                std::string hexStr = szToDecode.substr(i + 1, 2);
                hex = strtol(hexStr.c_str(), 0, 16);
                //字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
                //可以不经过编码直接用于URL
                if (!((hex >= 48 && hex <= 57) || //0-9
                    (hex >=97 && hex <= 122) ||   //a-z
                    (hex >=65 && hex <= 90) ||    //A-Z
                    //一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]
                    hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
                    || hex == 0x2a || hex == 0x2b|| hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
                    || hex == 0x3A || hex == 0x3B|| hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
                    ))
                {
                    result += char(hex);
                    i += 2;
                }
                else result += '%';
            }else {
                result += '%';
            }
            break;
        default:
            result += szToDecode[i];
            break;
        }
    }
    return result;
}

int inithttpsend(void)
{
    SSL_CTX *ctx;
    SSL *ssl;
 
    //ERR_load_crypto_strings();
    ERR_load_SSL_strings();
    //OpenSSL_add_all_algorithms();
    SSL_library_init();
 
    //如果系统平台不支持自动进行随机数种子的设置，这里应该进行设置(seed PRNG)
    ctx = SSL_CTX_new(SSLv23_client_method());
 
    //ctx = SSL_CTX_new(TLS_client_method());
    //SSL_CTX_set_cipher_list(ctx,"EXP-RC4-MD5");
 
 
    //通常应该在这里设置一些验证路径和模式等，因为这里没有设置，所以该例子可以跟使用任意CA签发证书的任意服务器建立连接
    sbio = BIO_new_ssl_connect(ctx);
 
    BIO_get_ssl(sbio, &ssl);
 
    if(!ssl)
    {
        //fprintf(stderr, "Can't locate SSL pointer/n");
        //ERR_print_errors_fp(stderr);
        BIO_free_all(sbio);
        return -1 ;
    }
 
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
 
    //这里你可以添加对SSL的其它一些设�? 
 //   BIO_set_conn_hostname(sbio, "106.39.167.232:443");
      BIO_set_conn_hostname(sbio, "61.145.127.35:8443");
    //BIO_set_conn_hostname(sbio, "61.145.127.31:8553");
 printf("e:61.145.127.35:8443\r\n") ;
    out = BIO_new_fp(stdout, BIO_NOCLOSE);
    if(BIO_do_connect(sbio) <= 0)
    {
        //fprintf(stderr, "Error connecting to server/n");
        //ERR_print_errors_fp(stderr);
        BIO_free_all(sbio);
        BIO_free(out);
        return -1 ;
    }
    printf("SSL connection using %s\n",SSL_get_cipher(ssl));
    //SSL_CTX_set_cipher_list(ctx,SSL_get_cipher(ssl));
 
    X509 *cert = SSL_get_peer_certificate(ssl);
 
    printf ("Server certificate:\n");
    char *str = X509_NAME_oneline(X509_get_subject_name(cert),0,0);
    printf ("\t subject: %s\n", str);
 
    OPENSSL_free(str);
 
    str = X509_NAME_oneline(X509_get_issuer_name(cert),0,0);
    printf ("\t issuer: %s\n", str);
    OPENSSL_free(str);
 
    X509_free(cert);
 
    if(BIO_do_handshake(sbio) <= 0)
    {
        //fprintf(stderr, "Error establishing SSL connection/n");
        //ERR_print_errors_fp(stderr);
        BIO_free_all(sbio);
        BIO_free(out);
        return -1 ;
    }
   
    return 0 ;
  
}


int httpsend(string strhttp)
{
    int len;
    char tmpbuf[1024];   
    string cstrSendData ; 
     
     int ret = inithttpsend();
     if(ret != 0)
     {
         //SendCom3Buf(QRERRO,(unsigned char *)"erro inithttp",13) ;
         return -1 ;
     }

     strhttp = UrlEncode(strhttp) ;

     cstrSendData = "POST ";
     cstrSendData += strhttp + "\r\n";
     cstrSendData += "Accept: image/gif, image/jpeg, text/json,text/html */*\r\nAccept-Language: zh-cn\r\n"
                "Accept-Encoding: gzip, deflate\r\nHost: 61.145.127.35:8443\r\n"
                "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:54.0) Gecko/20100101 Firefox/54.0\r\nConnection: Keep-Alive\r\n\r\n";

    printf("SendReq:%s",cstrSendData.c_str());
    printf("\r\n");
    //BIO_puts(sbio, header_get);
    BIO_write(sbio,cstrSendData.c_str(),cstrSendData.size());
    for(;;)
    {
        len = BIO_read(sbio, tmpbuf, 1023);

        if(len <= 0)
        {printf("err break\r\n") ;
          break;
        }
        //返回串口数据
        tmpbuf[len] = 0x00 ;
        //SendCom3Buf(QRTOLL,(unsigned char *)tmpbuf,len+1) ;
        BIO_write(out, tmpbuf, len);
        //sleep(1);
        //printf("1\r\n") ;
    }

    BIO_free_all(sbio);
    BIO_free(out);
 
 
    return 0;
}




void *HttpThread(void *param)
{
   string mhttp ;
   while(1)
   {
      sem_wait(&httpsem);  
      printf("get httpsem\r\n") ;
      HttpCri.Lock();
      if(Sendhttp != "")
      {printf("deal httpsem\r\n") ;
         mhttp = Sendhttp ;
         Sendhttp = "" ;
         HttpCri.UnLock();
         httpsend(mhttp);
      }
      else
      {
         HttpCri.UnLock();
      }
      usleep(2000);//delay 2ms  
   }

}

void httpinit(void)
{
   sem_init(&httpsem, 0, 0);  
   Sendhttp = "" ;
   //creat http send thread
   pthread_t m_HttpThread ;
   pthread_create(&m_HttpThread,NULL,HttpThread,NULL);




}





