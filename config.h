#ifndef __CONFIG_H__
#define __CONFIG_H__

/*The name of the cabinet types*/
typedef enum
{
	CABIN_NONE = 0,
	CABIN_HUAWEI_2_2,	// 双机柜双开门
	CABIN_HUAWEI_2_1,	// 双机柜单开门
	CABIN_HUAWEI_1_2,	// 单机柜双开门
	CABIN_HUAWEI_1_1,	// 单机柜单开门
	CABIN_ZTE,
	CABIN_JSA,
	CABIN_ATS,

	CABIN_NUM
}CABIN_TYPE_LIST;


int GetConfig(void);
int WriteNetconfig(char *configbuf,int configlen);
int WriteNetconfig2(char *configbuf,int configlen);
CABIN_TYPE_LIST CabinetTypeGet(void);


#endif
