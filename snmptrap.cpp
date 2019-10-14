#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_api.h>
#include <net-snmp/agent/ds_agent.h>
#include <string> 
#include "snmp.h"
#include "registers.h"

using namespace std; 

int      netsnmp_running = 1;

extern string StrServerURL1;
extern THUAWEIALARM HUAWEIDevAlarm;		//华为机柜告警
//extern void SetjsonTableStr(char* table, char *json, int *lenr);
extern void SetjsonTableStr(char* table, string &mstrjson);//17 门架运行状态
void SetjsonLTAlarmTableStr(char* table, string &mstrjson);
//extern int HttpPostParm(string url,char *pParmbuf,int *parmlen,int flag);
extern int HttpPostParm(string url,string &StrParmbuf,string strkey,int flag);
extern void myprintf(char* str);
extern void WriteLog(char* str);

//空调地址
extern unsigned int hwAirAddrbuf[2];
//温适度地址
extern unsigned int hwTemAddrbuf[2];
//电池地址
extern unsigned int hwAcbAddrbuf[10] ;

/************************************************************************
** 
** 
************************************************************************/

int snmp_input(int op, netsnmp_session *session, int reqid, netsnmp_pdu *pdu, void *magic)
{
	int count = 0 ;
	int i ;
	int AlarmID;
	char oidbuf[20] ;
	string Stroid,Strgetid,StrName;
    WriteLog("We got a trap:\n");
    struct variable_list *vars;
    for(vars = pdu->variables; vars; vars = vars->next_variable)
    {
        print_variable(vars->name, vars->name_length, vars);
    }

    //告警ID 设备地址
    int getAlarmID = 0 ;
    int getDevAddr = 0 ;
    int getIndex = 0;
    string getStroid = "";
    //string getstradd = "";
    Stroid = "";

    //.1.3.6.1.4.1.2011.6.164.1.1.2.100.1.2.9354
    string StrAlarmID = ".1.3.6.1.4.1.2011.6.164.1.1.2.100.1.2" ;
    string StrAirDevAddr = ".1.3.6.1.4.1.2011.6.164.1.10.2.2.1.2" ;
    string StrTemDevAddr = ".1.3.6.1.4.1.2011.6.164.1.28.1.1.2" ;
    string StrAcbAddr = ".1.3.6.1.4.1.2011.6.164.1.18.1.1.2";//锂电池地址
	for(vars = pdu->variables; vars; vars = vars->next_variable) 
	{
        getStroid = "";
        //printf("oididex: ");
        for(i=0; i<vars->name_length; i++)
        {
            if(*(vars->name_loc+i) == 0)
                break;
            //printf(".%d", *(vars->name_loc+i));


            memset(oidbuf,0,20);
            sprintf(oidbuf,".%d", *(vars->name_loc+i));
            Strgetid = oidbuf;
            if(i+1<vars->name_length)
               getStroid = getStroid + Strgetid;
            //if(i+3 == vars->name_length)
               // getstradd = getStroid ;
            getIndex = *(vars->name_loc+i) ;
        }
        //printf("\r\n");
        //printf("getStroid=%s,getstradd=%s,getIndex=%d\r\n",getStroid.c_str(),getstradd.c_str(),getIndex);
        if(getStroid == StrAlarmID)
        {
            //获取告警ID
            getAlarmID = getIndex ;
            printf("告警ID:%d\r\n",getAlarmID);
        }
        if(getStroid == StrAirDevAddr)
        {
            //空调设备地址
            getDevAddr = getIndex ;
            printf("空调设备地址:%d\r\n",getIndex);
        }
        if(getStroid == StrTemDevAddr)
        {
            //环境温湿度地址
            getDevAddr = getIndex ;
            printf("环境温湿度地址:%d\r\n",getIndex);
        }
        if(getStroid == StrAcbAddr)
        {
            //锂电池地址
            getDevAddr = getIndex ;
            printf("锂电池地址:%d\r\n",getIndex);
        }




        /*if (vars->type == ASN_OCTET_STR)
		{
			char *sp = (char *)malloc(1 + vars->val_len);
			memcpy(sp, vars->val.string, vars->val_len);
			sp[vars->val_len] = '\0';
			printf("value #%d is a string: %s\n", count, sp);

            //AlarmID=GetAlarmID(sp);

			free(sp);
		}
        else
            */
        if(vars->type == ASN_OBJECT_ID)
		{

			printf("value #%d is a oid: ", count);
			for(i=0; i<vars->name_length; i++)
			{
				if(*(vars->name_loc+i) == 0)
					break;
				printf(".%d", *(vars->name_loc+i));
			}
			printf(" and value is ");

			Stroid = "";
			for(i=0; i<(vars->val_len/sizeof(int)); i++)
			{
				memset(oidbuf,0,20);
				sprintf(oidbuf,".%d", *(vars->val.objid+i));
				Strgetid = oidbuf;
				Stroid = Stroid + Strgetid;
			}
			printf("%s\r\n",Stroid.c_str()) ;

		     
		}
        /*else
		{
			unsigned int IntegerValue = *(vars->val.integer) ;
			printf("value #%d is a integer: %d\n", count, IntegerValue);
		}
        */
		count++;
	}
    printf("Stroid=%s,getAlarmID=%d,getDevAddr=%d\r\n",Stroid.c_str(),getAlarmID,getDevAddr);
    DealAlarm(Stroid,getAlarmID,getDevAddr);
	
	string mstrjson;
	string mstrkey = "";
//	SetjsonTableStr("flagrunstatusalarm",mstrjson);
	SetjsonLTAlarmTableStr("cabinetAlarmUpload",mstrjson);
    //printf("SetjsonLTAlarmTableStr \n%s\n",mstrjson.c_str());
	if(StrServerURL1!="")
		HttpPostParm(StrServerURL1,mstrjson,mstrkey,HTTPPOST);
	
	mstrkey = "";
	SetjsonTableStr("flagrunstatusalarm",mstrjson);
	NetSendParm(NETCMD_FLAGRUNSTATUS,(char *)(mstrjson.c_str()),mstrjson.size());
	
    return 1;
}

static int pre_parse(netsnmp_session * session, netsnmp_transport *transport,
          void *transport_data, int transport_data_length)
{
	return 1;
}

static netsnmp_session *snmptrapd_add_session(netsnmp_transport *t)
{
    netsnmp_session sess, *session = &sess, *rc = NULL;

    snmp_sess_init(session);
    session->peername = SNMP_DEFAULT_PEERNAME;  /* Original code had NULL here */
    session->version = SNMP_DEFAULT_VERSION;
    session->community_len = SNMP_DEFAULT_COMMUNITY_LEN;
    session->retries = SNMP_DEFAULT_RETRIES;
    session->timeout = SNMP_DEFAULT_TIMEOUT;
    session->callback = snmp_input;
    session->callback_magic = (void *) t;
    session->authenticator = NULL;
    sess.isAuthoritative = SNMP_SESS_UNKNOWNAUTH;

    rc = snmp_add(session, t, pre_parse, NULL);
    if (rc == NULL)
	{
        snmp_sess_perror("snmptrapd", session);
    }
    return rc;
}

static void snmptrapd_close_sessions(netsnmp_session * sess_list)
{
    netsnmp_session *s = NULL, *next = NULL;

    for (s = sess_list; s != NULL; s = next)
	{
        next = s->next;
        snmp_close(s);
    }
}

static void snmptrapd_main_loop(void)
{
	int		count, numfds, block;
	fd_set	readfds,writefds,exceptfds;
	struct timeval  timeout, *tvp;

	printf("snmptrapd_main_loop\n");

	while (netsnmp_running)
	{		
		numfds = 0;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
		block = 0;
		tvp = &timeout;
		timerclear(tvp);
		tvp->tv_sec = 5;
		snmp_select_info(&numfds, &readfds, tvp, &block);
		if (block == 1)
		{
			tvp = NULL;         /* block without timeout */
		}

//#ifndef NETSNMP_FEATURE_REMOVE_FD_EVENT_MANAGER
//		netsnmp_external_event_info(&numfds, &readfds, &writefds, &exceptfds);
//#endif /* NETSNMP_FEATURE_REMOVE_FD_EVENT_MANAGER */

		count = select(numfds, &readfds, &writefds, &exceptfds, tvp);
//		printf("count = %d\n", count);
		if (count > 0)
		{
//#ifndef NETSNMP_FEATURE_REMOVE_FD_EVENT_MANAGER
//			netsnmp_dispatch_external_events(&count, &readfds, &writefds, &exceptfds);
//#endif /* NETSNMP_FEATURE_REMOVE_FD_EVENT_MANAGER */

			/* If there are any more events after external events, then try SNMP events. */
			if (count > 0)
			{
				snmp_read(&readfds);
			}
		}
		else 
		{
			switch (count)
			{
				case 0:
					snmp_timeout();
					break;
				case -1:
					//if (errno == EINTR)	continue;		// ??
					snmp_log_perror("select");
					netsnmp_running = 0;
					break;
				default:
					fprintf(stderr, "select returned %d\n", count);
					netsnmp_running = 0;
			}
		}
		run_alarms();
	}
}



/************************************************************************
** 
** 
 * Returns:
 *	2	Always succeeds.  (?)
 *
 *
 * Setup and start the trap receiver daemon.
 *
 * Also successfully EXITs with zero for some options.
 *
************************************************************************/
void *snmptrapthread(void *param)
{
    sleep(5);

	netsnmp_session *sess_list = NULL, *ss = NULL;
	netsnmp_transport *transport = NULL;
	int             arg, i = 0;
	int             uid = 0, gid = 0;
	char           *cp, *listen_ports = NULL;
#if defined(USING_AGENTX_SUBAGENT_MODULE) && !defined(NETSNMP_SNMPTRAPD_DISABLE_AGENTX)
	int             agentx_subagent = 1;
#endif
	
	/*
	  * Initialize the world.
	  */
	init_snmp("snmptrapd");

	if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_QUIT_IMMEDIATELY))
	{
		/*
		  * just starting up to process specific configuration and then
		  * shutting down immediately. 
		  */
		netsnmp_running = 0;
	}

	/*
	  * if no logging options on command line or in conf files, use syslog
	  */
	if (0 == snmp_get_do_logging())
	{
#ifndef NETSNMP_FEATURE_REMOVE_LOGGING_SYSLOG

	snmp_enable_syslog_ident("snmptrapd", LOG_DAEMON);

#endif /* NETSNMP_FEATURE_REMOVE_LOGGING_SYSLOG */
	}

	if (listen_ports)
	{
		cp = listen_ports;
	}
	else
	{
		cp =  "udp:162";	/* Default default port */;
	}

	while (cp != NULL) 
	{
		char *sep = strchr(cp, ',');
		if (sep != NULL)
		{
			*sep = 0;
		}
		transport = netsnmp_transport_open_server("snmptrap", cp);	//cp=udp:162
		if (transport == NULL) 
		{
			//snmp_log(LOG_ERR, "couldn't open %s -- errno %d (\"%s\")\n", cp, errno, strerror(errno));
			printf("ERR: couldn't open %s ", cp);
			snmptrapd_close_sessions(sess_list);
			SOCK_CLEANUP;
			return 0;
		} 
		else 
		{
			ss = snmptrapd_add_session(transport);
			if (ss == NULL) 
			{
			/*
			  * Shouldn't happen?  We have already opened the transport
			  * successfully, so what could have gone wrong?  
			  */
				snmptrapd_close_sessions(sess_list);
				netsnmp_transport_free(transport);
				//snmp_log(LOG_ERR, "couldn't open snmp - %s", strerror(errno));
				printf("couldn't open snmp");
				SOCK_CLEANUP;
				return 0;
			} 
			else 
			{
				ss->next = sess_list;
				sess_list = ss;
			}
		}

		/*
		  * Process next listen address, if there is one.  
		  */

		if (sep != NULL)
		{
			*sep = ',';
			cp = sep + 1;
		}
		else
		{
			cp = NULL;
		}
	}

	printf("snmp free listen_ports\n");
	SNMP_FREE(listen_ports); /* done with them */

#ifdef NETSNMP_USE_MYSQL
	if( netsnmp_mysql_init() )
	{
		fprintf(stderr, "MySQL initialization failed\n");
		return 0;
	}
#endif

	snmp_log(LOG_INFO, "NET-SNMP version %s\n", netsnmp_get_version());


	snmptrapd_main_loop();

	if (snmp_get_do_logging())
	{
		struct tm      *tm;
		time_t          timer;
		time(&timer);
		tm = localtime(&timer);
		snmp_log(LOG_INFO,
                "%.4d-%.2d-%.2d %.2d:%.2d:%.2d NET-SNMP version %s Stopped.\n",
                 tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
                 tm->tm_min, tm->tm_sec, netsnmp_get_version());
	}
	snmp_log(LOG_INFO, "Stopping snmptrapd\n");
    
	snmptrapd_close_sessions(sess_list);
	snmp_shutdown("snmptrapd");
	snmp_disable_log();
	SOCK_CLEANUP;
	return 0;

}


void initHUAWEIALARM()
{
	HUAWEIDevAlarm.hwEnvTempAlarmTraps="0";		//设备柜高温/低温告警
	HUAWEIDevAlarm.hwEnvTempAlarmTraps2="0";		//电池柜高温/低温告警
	HUAWEIDevAlarm.hwEnvHumiAlarmTraps="0";		//设备柜高湿/低湿告警
	HUAWEIDevAlarm.hwEnvHumiAlarmTraps2="0";		//电池柜高湿/低湿告警
	HUAWEIDevAlarm.hwSpareDigitalAlarmTraps="0";	//输入干接点告警
	HUAWEIDevAlarm.hwDoorAlarmTraps="0";		//门禁告警
	HUAWEIDevAlarm.hwWaterAlarmTraps="0";		//水浸告警
	HUAWEIDevAlarm.hwSmokeAlarmTraps="0";		//烟感告警
	HUAWEIDevAlarm.hwair_cond_infan_alarm="0";		//空调内风机故障
	HUAWEIDevAlarm.hwair_cond_outfan_alarm="0";		//空调外风机故障
	HUAWEIDevAlarm.hwair_cond_comp_alarm="0";		//空调压缩机故障
	HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm="0";		//空调回风口传感器故障
	HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm="0";		//空调蒸发器冻结
	HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm="0";		//空调频繁高压力
	HUAWEIDevAlarm.hwair_cond_comm_fail_alarm="0";		//空调通信失败告警
	//新增加告警
	HUAWEIDevAlarm.hwACSpdAlarmTraps="0";					//交流防雷器故障
	HUAWEIDevAlarm.hwDCSpdAlarmTraps="0";					//直流防雷器故障
	//电源告警
	HUAWEIDevAlarm.hwAcInputFailAlarm="0";				//交流电源输入停电告警
	HUAWEIDevAlarm.hwAcInputL1FailAlarm="0";				//交流电源输入L1	相缺相告警
	HUAWEIDevAlarm.hwAcInputL2FailAlarm="0";				//交流电源输入L2	相缺相告警
	HUAWEIDevAlarm.hwAcInputL3FailAlarm="0";				//交流电源输入L3	相缺相告警
	HUAWEIDevAlarm.hwDcVoltAlarmTraps="0";				//直流电源输出告警
	HUAWEIDevAlarm.hwLoadLvdAlarmTraps="0";				//LLVD1下电告警
	//锂电池告警
	HUAWEIDevAlarm.hwAcbGroup_comm_fail_alarm="0";		//所有锂电通信失败
	HUAWEIDevAlarm.hwAcbGroup_discharge_alarm="0";		//电池放电告警
	HUAWEIDevAlarm.hwAcbGroup_charge_overcurrent_alarm="0";	//电池充电过流
	HUAWEIDevAlarm.hwAcbGroup_temphigh_alarm="0";		//电池温度高
	HUAWEIDevAlarm.hwAcbGroup_templow_alarm="0";		//电池温度低
	HUAWEIDevAlarm.hwAcbGroup_poweroff_alarm="0";		//电池下电
	HUAWEIDevAlarm.hwAcbGroup_fusebreak_alarm="0";		//电池熔丝断
	HUAWEIDevAlarm.hwAcbGroup_moduleloss_alarm="0";		//模块丢失
}


int snmptrapInit(void)
{
   pthread_t m_snmptrapthread ;
   pthread_create(&m_snmptrapthread,NULL,snmptrapthread,NULL);

   return 0 ;

}



void DealAlarm(string Stroid,int AlarmID,int mgetIndex)
{
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.14.0.1") == 0)//环境传感器
	{
		if(AlarmID==17452)//高温告警
		{printf("高温告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwTemAddrbuf[1])
            {
              HUAWEIDevAlarm.hwEnvTempAlarmTraps2="1";
			  printf("电池柜高温告警\n");
            }
            else
            {
              HUAWEIDevAlarm.hwEnvTempAlarmTraps="1";
			  printf("设备柜高温告警\n");
			}
        }
		if(AlarmID==17453)//低温告警
		{printf("低温告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwTemAddrbuf[1])
			{
                HUAWEIDevAlarm.hwEnvTempAlarmTraps2="2";
			  printf("电池柜低温告警\n");
			}
            else
			{
                HUAWEIDevAlarm.hwEnvTempAlarmTraps="2";
			  printf("设备柜低温告警\n");
			}
        }
		if(AlarmID==17455)//高湿告警
		{printf("高湿告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwTemAddrbuf[1])
			{
                HUAWEIDevAlarm.hwEnvHumiAlarmTraps2="1";
			  printf("电池柜高湿告警\n");
			}
            else
			{
                HUAWEIDevAlarm.hwEnvHumiAlarmTraps="1";
			  printf("设备柜高湿告警\n");
			}
        }
		if(AlarmID==17456)//低湿告警
		{printf("低湿告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwTemAddrbuf[1])
			{
                HUAWEIDevAlarm.hwEnvHumiAlarmTraps2="2";
			  printf("电池柜低湿告警\n");
			}
            else
			{
                HUAWEIDevAlarm.hwEnvHumiAlarmTraps="2";
			  printf("设备柜低湿告警\n");
			}
        }
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.14.0.2") == 0)
	{
		if(AlarmID==17452)//高温告警恢复
		{printf("高温告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwTemAddrbuf[1])
			{
                HUAWEIDevAlarm.hwEnvTempAlarmTraps2="0";
			  printf("电池柜高温告警恢复\n");
			}
            else
			{
                HUAWEIDevAlarm.hwEnvTempAlarmTraps="0";
			  printf("设备柜高温告警恢复\n");
			}
        }
		if(AlarmID==17453)//低温告警恢复
		{printf("低温告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwTemAddrbuf[1])
			{
                HUAWEIDevAlarm.hwEnvTempAlarmTraps2="0";
			  printf("电池柜低温告警恢复\n");
			}
            else
			{
                HUAWEIDevAlarm.hwEnvTempAlarmTraps="0";
			  printf("设备柜低温告警恢复\n");
			}
        }
		if(AlarmID==17455)//高湿告警恢复
		{printf("高湿告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwTemAddrbuf[1])
			{
                HUAWEIDevAlarm.hwEnvHumiAlarmTraps2="0";
			  printf("电池柜高湿告警恢复\n");
			}
            else
			{
                HUAWEIDevAlarm.hwEnvHumiAlarmTraps="0";
			  printf("设备柜高湿告警恢复\n");
			}
        }
		if(AlarmID==17456)//低湿告警恢复
		{printf("低湿告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwTemAddrbuf[1])
			{
                HUAWEIDevAlarm.hwEnvHumiAlarmTraps2="0";
			  printf("电池柜低湿告警恢复\n");
			}
            else
			{
                HUAWEIDevAlarm.hwEnvHumiAlarmTraps="0";
			  printf("设备柜低湿告警恢复\n");
			}
        }
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.31") == 0)//门禁告警
	{
		printf("门禁告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		HUAWEIDevAlarm.hwDoorAlarmTraps="1";
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.32") == 0)//门禁告警恢复
	{
		printf("门禁告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		HUAWEIDevAlarm.hwDoorAlarmTraps="0";
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.33") == 0)//水浸告警
	{
		printf("水浸告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		HUAWEIDevAlarm.hwWaterAlarmTraps="1";
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.34") == 0)//水浸告警恢复
	{
		printf("水浸告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		HUAWEIDevAlarm.hwWaterAlarmTraps="0";
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.35") == 0)//烟雾告警
	{
		printf("烟雾告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		HUAWEIDevAlarm.hwSmokeAlarmTraps="1";
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.36") == 0)//烟雾告警恢复
	{
		printf("烟雾告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		HUAWEIDevAlarm.hwSmokeAlarmTraps="0";
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.57") == 0)//防雷器故障告警
	{
		if(AlarmID==108)//交流防雷器故障
		{
			printf("交流防雷器故障告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
			HUAWEIDevAlarm.hwACSpdAlarmTraps="1";
		}
		if(AlarmID==420)//直流防雷器故障
		{
			printf("直流防雷器故障告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
			HUAWEIDevAlarm.hwDCSpdAlarmTraps="1";
		}
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.58") == 0)//防雷器告警恢复
	{
		if(AlarmID==108)//交流防雷器故障恢复
		{
			printf("交流防雷器故障告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
			HUAWEIDevAlarm.hwACSpdAlarmTraps="0";
		}
		if(AlarmID==420)//直流防雷器故障恢复
		{
			printf("直流防雷器故障告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
			HUAWEIDevAlarm.hwDCSpdAlarmTraps="0";
		}
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.85") == 0)//直流空调告警
	{
		if(AlarmID==9350)//空调内风机故障
        {
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_infan_alarm2="1";
				printf("电池柜空调内风机故障告警\n");
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_infan_alarm="1";
				printf("设备柜空调内风机故障告警\n");
			}
        }
		if(AlarmID==9351)//空调外风机故障
		{printf("空调外风机故障,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
           if(mgetIndex == hwAirAddrbuf[1])
		   {
               HUAWEIDevAlarm.hwair_cond_outfan_alarm2="1";
		   }
           else
		   {
               HUAWEIDevAlarm.hwair_cond_outfan_alarm="1";
		   }
        }
		if(AlarmID==9353)//空调压缩机故障
		{printf("空调压缩机故障,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_comp_alarm2="1";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_comp_alarm="1";
			}
        }
		if(AlarmID==9356)//空调回风口传感器故障
		{printf("空调回风口传感器故障,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm2="1";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm="1";
			}
        }
		if(AlarmID==9354)//空调通信失败告警
		{printf("空调通信失败告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_comm_fail_alarm2="1";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_comm_fail_alarm="1";
			}
        }
		if(AlarmID==9364)//空调蒸发器冻结
		{printf("空调蒸发器冻结,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm2="1";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm="1";
			}
        }
		if(AlarmID==9365)//空调频繁高压力
		{printf("空调频繁高压力,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm2="1";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm="1";
			}
        }
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.86") == 0)//直流空调告警恢复
	{
		if(AlarmID==9350)//空调内风机故障
		{printf("空调内风机故障恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
               HUAWEIDevAlarm.hwair_cond_infan_alarm2="0";
		    }
            else
			{
               HUAWEIDevAlarm.hwair_cond_infan_alarm="0";
		   }
        }
		if(AlarmID==9351)//空调外风机故障
		{printf("空调外风机故障恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_outfan_alarm2="0";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_outfan_alarm="0";
			}
        }
		if(AlarmID==9353)//空调压缩机故障
		{printf("空调压缩机故障恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_comp_alarm2="0";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_comp_alarm="0";
			}
        }
		if(AlarmID==9356)//空调回风口传感器故障
		{printf("空调回风口传感器故障恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm2="0";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm="0";
			}
        }
		if(AlarmID==9354)//空调通信失败告警
		{printf("空调通信失败告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_comm_fail_alarm2="0";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_comm_fail_alarm="0";
			}
        }
		if(AlarmID==9364)//空调蒸发器冻结
		{printf("空调蒸发器冻结恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm2="0";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm="0";
			}
        }
		if(AlarmID==9365)//空调频繁高压力
		{printf("空调频繁高压力恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
            if(mgetIndex == hwAirAddrbuf[1])
			{
                HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm2="0";
			}
            else
			{
                HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm="0";
			}
        }
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.15") == 0)//交流电告警
	{printf("交流电告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		if(AlarmID==1550)//交流停电告警
		{
			HUAWEIDevAlarm.hwAcInputFailAlarm="1";
		   printf("交流停电告警\n");
		}
		if(AlarmID==1567)//交流电源输入L1	相缺相告警
		{
			HUAWEIDevAlarm.hwAcInputL1FailAlarm="1";
		   printf("交流电源输入L1	相缺相告警\n");
		}
		if(AlarmID==1568)//交流电源输入L2	相缺相告警
		{
			HUAWEIDevAlarm.hwAcInputL2FailAlarm="1";
		   printf("交流电源输入L2 相缺相告警\n");
		}
		if(AlarmID==1569)//交流电源输入L3	相缺相告警
		{
			HUAWEIDevAlarm.hwAcInputL3FailAlarm="1";
		   printf("交流电源输入L3 相缺相告警\n");
		}
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.16") == 0)//交流电告警恢复
	{printf("交流电告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		if(AlarmID==1550)//交流停电告警
		{
			HUAWEIDevAlarm.hwAcInputFailAlarm="0";
		   printf("交流电告警恢复\n");
		}
		if(AlarmID==1567)//交流电源输入L1	相缺相告警
		{
			HUAWEIDevAlarm.hwAcInputL1FailAlarm="0";
		   printf("交流电源输入L1	相缺相告警恢复\n");
		}
		if(AlarmID==1568)//交流电源输入L2	相缺相告警
		{
			HUAWEIDevAlarm.hwAcInputL2FailAlarm="0";
		   printf("交流电源输入L2 相缺相告警恢复\n");
		}
		if(AlarmID==1569)//交流电源输入L3	相缺相告警
		{
			HUAWEIDevAlarm.hwAcInputL3FailAlarm="0";
		   printf("交流电源输入L3 相缺相告警恢复\n");
		}
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.19") == 0)//直流电告警
	{printf("直流电告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		if(AlarmID==10152)//直流过压告警
		{
			HUAWEIDevAlarm.hwDcVoltAlarmTraps="1";
		   printf("直流过压告警\n");
		}
		if(AlarmID==10153)//直流欠压告警
		{
			HUAWEIDevAlarm.hwDcVoltAlarmTraps="2";
		   printf("直流欠压告警\n");
		}
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.20") == 0)//直流电告警恢复
	{printf("直流电告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		if(AlarmID==10152)//直流过压
		{
			HUAWEIDevAlarm.hwDcVoltAlarmTraps="0";
		   printf("直流过压告警恢复\n");
		}
		if(AlarmID==10153)//直流欠压
		{
			HUAWEIDevAlarm.hwDcVoltAlarmTraps="0";
		   printf("直流欠压告警恢复\n");
		}
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.21") == 0)//LLVD1下电告警
	{printf("LLVD1下电告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		HUAWEIDevAlarm.hwLoadLvdAlarmTraps="1";
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.22") == 0)//LLVD1下电告警恢复
	{printf("LLVD1下电告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		HUAWEIDevAlarm.hwLoadLvdAlarmTraps="0";
	}
	if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.2.0.99") == 0)//锂电告警
	{printf("锂电告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		if(AlarmID==13800)//所有锂电通信失败
		{
			HUAWEIDevAlarm.hwAcbGroup_comm_fail_alarm="1";
			printf("所有锂电通信失败告警\n");
		}
		if(AlarmID==13801)//电池放电告警
		{
			HUAWEIDevAlarm.hwAcbGroup_discharge_alarm="1";
			printf("电池放电告警\n");
		}
		if(AlarmID==13811)//电池充电过流
		{
			HUAWEIDevAlarm.hwAcbGroup_charge_overcurrent_alarm="1";
			printf("电池充电过流告警\n");
		}
		if(AlarmID==13812)//电池温度高
		{
			HUAWEIDevAlarm.hwAcbGroup_temphigh_alarm="1";
			printf("电池温度高告警\n");
		}
		if(AlarmID==13813)//电池温度低
		{
			HUAWEIDevAlarm.hwAcbGroup_templow_alarm="1";
			printf("电池温度低告警\n");
		}
		if(AlarmID==13814)//电池下电
		{
			HUAWEIDevAlarm.hwAcbGroup_poweroff_alarm="1";
			printf("电池下电告警\n");
		}
		if(AlarmID==13816)//电池熔丝断
		{
			HUAWEIDevAlarm.hwAcbGroup_fusebreak_alarm="1";
			printf("电池熔丝断告警\n");
		}
		if(AlarmID==13821)//模块丢失
		{
			HUAWEIDevAlarm.hwAcbGroup_moduleloss_alarm="1";
			printf("模块丢失告警\n");
		}
	}

    if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.2.0.100") == 0)//锂电告警恢复
	{printf("锂电告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
		if(AlarmID==13800)//所有锂电通信失败
		{
			HUAWEIDevAlarm.hwAcbGroup_comm_fail_alarm="0";
			printf("所有锂电通信失败恢复\n");
		}
		if(AlarmID==13801)//电池放电告警
		{
			HUAWEIDevAlarm.hwAcbGroup_discharge_alarm="0";
			printf("电池放电告警恢复\n");
		}
		if(AlarmID==13811)//电池充电过流
		{
			HUAWEIDevAlarm.hwAcbGroup_charge_overcurrent_alarm="0";
			printf("电池充电过流告警恢复\n");
		}
		if(AlarmID==13812)//电池温度高
		{
			HUAWEIDevAlarm.hwAcbGroup_temphigh_alarm="0";
			printf("电池温度高告警恢复\n");
		}
		if(AlarmID==13813)//电池温度低
		{
			HUAWEIDevAlarm.hwAcbGroup_templow_alarm="0";
			printf("电池温度低告警恢复\n");
		}
		if(AlarmID==13814)//电池下电
		{
			HUAWEIDevAlarm.hwAcbGroup_poweroff_alarm="0";
			printf("电池下电告警恢复\n");
		}
		if(AlarmID==13816)//电池熔丝断
		{
			HUAWEIDevAlarm.hwAcbGroup_fusebreak_alarm="0";
			printf("电池熔丝断告警恢复\n");
		}
		if(AlarmID==13821)//模块丢失
		{
			HUAWEIDevAlarm.hwAcbGroup_moduleloss_alarm="0";
			printf("模块丢失告警恢复\n");
		}
	}

    //单个锂电池告警
    if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.3.0.99") == 0)
    {   printf("单个锂电告警,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
        int mAcdIndex = 0;
        for(int i=0;i<4;i++)
        {
            if(hwAcbAddrbuf[i] == mgetIndex)
            {
               mAcdIndex = i;
               break;
            }
        }

        if(AlarmID==13750)//锂电通信失败
        {
            HUAWEIDevAlarm.hwAcbCom_Failure[mAcdIndex]="1";
            printf("锂电池%d通信失败\r\n",mAcdIndex+1);

        }
        else if(AlarmID==13751)//加热器故障
        {
            HUAWEIDevAlarm.hwAcbHeater_Fault_alarm[mAcdIndex]="1";
            printf("锂电池%d加热器故障\r\n",mAcdIndex+1);

        }
        else if(AlarmID==13752)//单板故障
        {
            HUAWEIDevAlarm.hwAcbBoard_Hardware_Fault_alarm[mAcdIndex]="1";
            printf("锂电池%d单板故障\r\n",mAcdIndex+1);
        }
        else if(AlarmID==13755)//低温保护
        {
            HUAWEIDevAlarm.hwAcbLow_Temp_Protection_alarm[mAcdIndex]="1";
            printf("锂电池%d低温保护\r\n",mAcdIndex+1);
        }

    }

    //单个锂电池告警恢复
    if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.3.0.100") == 0)
    {   printf("单个锂电告警恢复,AlarmID:%d,Index:%d\r\n",AlarmID,mgetIndex);
        int mAcdIndex = 0;
        for(int i=0;i<4;i++)
        {
            if(hwAcbAddrbuf[i] == mgetIndex)
            {
               mAcdIndex = i;
               break;
            }
        }

        if(AlarmID==13750)//锂电通信失败
        {
            HUAWEIDevAlarm.hwAcbCom_Failure[mAcdIndex]="0";
            printf("锂电池%d通信失败恢复\r\n",mAcdIndex+1);

        }
        else if(AlarmID==13751)//加热器故障
        {
            HUAWEIDevAlarm.hwAcbHeater_Fault_alarm[mAcdIndex]="0";
            printf("锂电池%d加热器故障恢复\r\n",mAcdIndex+1);

        }
        else if(AlarmID==13752)//单板故障
        {
            HUAWEIDevAlarm.hwAcbBoard_Hardware_Fault_alarm[mAcdIndex]="0";
            printf("锂电池%d单板故障恢复\r\n",mAcdIndex+1);
        }
        else if(AlarmID==13755)//低温保护
        {
            HUAWEIDevAlarm.hwAcbLow_Temp_Protection_alarm[mAcdIndex]="0";
            printf("锂电池%d低温保护恢复\r\n",mAcdIndex+1);
        }

    }



}

int GetAlarmID(char* sp)
{
	int AlarmID=0;
	
	if(strcmp(sp,"Door Open Alarm") == 0)	//门磁告警
		AlarmID=6090;
	if(strcmp(sp,"Water Alarm") == 0)	//水浸告警
		AlarmID=6070;
	if(strcmp(sp,"Smoke Alarm") == 0)	//烟雾告警告警
		AlarmID=6080;
	if(strcmp(sp,"Ambient High Temperature") == 0)	//环境高温告警
		AlarmID=17452;
	if(strcmp(sp,"Ambient Low Temperature") == 0)	//环境低温告警
		AlarmID=17453;
	if(strcmp(sp,"Ambient Temperature Sensor Missing") == 0)	//环境温度传感器未接告警
		AlarmID=17454;
	if(strcmp(sp,"Ambient High Humidity") == 0) //环境高湿告警
		AlarmID=17455;
	if(strcmp(sp,"Ambient Low Humidity") == 0)	//环境低湿告警
		AlarmID=17456;
	if(strcmp(sp,"Ambient Humidity Sensor Missing") == 0)	//环境湿度传感器未接告警
		AlarmID=17457;
	if(strcmp(sp,"AC Surge Protector Fault") == 0)	//交流防雷器故障告警
		AlarmID=108;
	if(strcmp(sp,"DC Surge Protector Fault") == 0)	//直流防雷器故障告警
		AlarmID=420;
	if(strcmp(sp,"Internal Fan Fault") == 0)	//空调内风机故障告警
		AlarmID=9350;
	if(strcmp(sp,"External Fan Fault") == 0)	//空调外风机故障告警
		AlarmID=9351;
	if(strcmp(sp,"Compressor Fault") == 0)	//空调压缩机故障告警
		AlarmID=9353;
	if(strcmp(sp,"Cabinet Return Air Temperature Sensor Fault") == 0)	//空调回风口传感器故障告警
		AlarmID=9356;
	if(strcmp(sp,"Evaporator Frozen") == 0) //空调蒸发器冻结告警
		AlarmID=9364;
	if(strcmp(sp,"Frequent High Pressure") == 0)	//空调频繁高压力告警
		AlarmID=9365;
	if(strcmp(sp,"Communication Failure") == 0) //空调通信失败告警告警
		AlarmID=9354;
	if(strcmp(sp,"AC Failure") == 0)	//交流停电告警
		AlarmID=1550;
	if(strcmp(sp,"AC Phase L1 Failure") == 0)	//交流L1相缺相告警
		AlarmID=1567;
	if(strcmp(sp,"AC Phase L2 Failure") == 0)	//交流L2相缺相告警
		AlarmID=1568;
	if(strcmp(sp,"AC Phase L3 Failure") == 0)	//交流L3相缺相告警
		AlarmID=1569;
	if(strcmp(sp,"DC Ultra Overvoltage") == 0)	//直流过压告警
		AlarmID=10152;
	if(strcmp(sp,"DC Ultra Undervoltage") == 0) //直流欠压告警
		AlarmID=10153;
	if(strcmp(sp,"LLVD Disconnected") == 0) //LLVD1下电告警
		AlarmID=8822;
	if(strcmp(sp,"All ESMU Communication Failure") == 0)	//所有锂电通信失败
		AlarmID=13800;
	if(strcmp(sp,"Battery Discharge") == 0) //电池放电
		AlarmID=13801;
	if(strcmp(sp,"Battery Charge Overcurrent") == 0)	//电池充电过流
		AlarmID=13811;
	if(strcmp(sp,"High Battery Temperature") == 0)	//电池温度高
		AlarmID=13812;
	if(strcmp(sp,"Low Battery Temperature") == 0)	//电池温度低
		AlarmID=13813;
	if(strcmp(sp,"BLVD Disconnected") == 0) //电池下电
		AlarmID=13814;
	if(strcmp(sp,"Battery Fuse Break") == 0)	//电池熔丝断
		AlarmID=13816;
	if(strcmp(sp,"ESMU Missing") == 0)	//模块丢失
		AlarmID=13821;
/*	if(strcmp(sp,"") == 0)	//
		AlarmID=;
	if(strcmp(sp,"") == 0)	//
		AlarmID=;*/

	return AlarmID;
}


