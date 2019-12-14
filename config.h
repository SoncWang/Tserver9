#ifndef __CONFIG_H__
#define __CONFIG_H__

//CABINETTYPE:作为区分机柜类型，用于编译不同的代码
//CABINETTYPE  1：华为（包括华为单门 双门等） 5：中兴; 6：金晟安; 7：爱特斯 StrVersionNo
#define CABINETTYPE  1

int GetConfig(void);
int WriteNetconfig(char *configbuf,int configlen);
int WriteNetconfig2(char *configbuf,int configlen);

#endif
