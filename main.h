#ifndef __MAIN_H__
#define __MAIN_H__

#include "comport.h"
#include "comserver.h"
//#include "http.h"
#include "tea.h"
#include "server.h"
#include "HttpPost.h"
#include "HttpServer.h"
#include "Ipcam.h"
#include "config.h"
#include "net_rsu.h"
#include "snmp.h"

void InitStuEnvi_Param(ENVI_PARAMS *pParam);
void InitStuUPS_Param(UPS_PARAMS *pParam);

#endif
