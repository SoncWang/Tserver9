#ifndef __HTTPPOST_H__
#define __HTTPPOST_H__


int HttpPostjpeng(unsigned char *pjpengbuf,int jpenglen);
int HttpPostParm(std::string url,char *pParmbuf,int *parmlen,int flag);


#endif
