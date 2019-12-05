#ifndef __HTTPPOST_H__
#define __HTTPPOST_H__

#include <stdlib.h>
using namespace std;

int HttpPostjpeng(unsigned char *pjpengbuf,int jpenglen);
int HttpPostParm(string url,string &StrParmbuf,string strBasickey,int flag,string strDigestUser,string strDigestKey,int Inttimeout);

//int HttpPostParmm(string url,char *pParmbuf,int *parmlen,int flag);

#endif
