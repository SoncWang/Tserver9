#ifndef __AtlasServer_H__
#define __AtlasServer_H__

#include "registers.h"
#include <string>

using namespace std;//引入整个名空间

typedef struct atlas_state_struct
{
	unsigned long TimeStamp; 		//状态获取时间戳
	bool Linked;					//连接状态 0:断开 1：连接

	string stratlasdata;
}ATLAS_STATE;


int GetAtlasData(void);
int AtlasInit(void);
void init_atlas_struct(int atlasno);


#endif

