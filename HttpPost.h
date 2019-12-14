#ifndef __HTTPPOST_H__
#define __HTTPPOST_H__

#include <stdlib.h>
#include "config.h"

using namespace std;

int HttpPostjpeng(unsigned char *pjpengbuf,int jpenglen);
int HttpPostParm(string url,string &StrParmbuf,string strkey,int flag);

//int HttpPostParmm(string url,char *pParmbuf,int *parmlen,int flag);

#if((CABINETTYPE == 5) || (CABINETTYPE == 6)) //5：中兴; 6：金晟安
int zteHttpPostParm(string url,string &StrParmbuf,string strBasickey,int flag,string strDigestUser,string strDigestKey,int Inttimeout);
#endif



#endif
