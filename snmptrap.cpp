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
//extern int HttpPostParm(string url,char *pParmbuf,int *parmlen,int flag);
extern int HttpPostParm(string url,string &StrParmbuf,string strkey,int flag);
extern void myprintf(char* str);
extern void WriteLog(char* str);

/************************************************************************
**
**
************************************************************************/

int snmp_input(int op, netsnmp_session *session, int reqid, netsnmp_pdu *pdu, void *magic)
{
	int count = 0 ;
	int i ;
	char str[256];
    WriteLog("We got a trap:\n");
    struct variable_list *vars;
    for(vars = pdu->variables; vars; vars = vars->next_variable)
    {
        print_variable(vars->name, vars->name_length, vars);
    }

	for(vars = pdu->variables; vars; vars = vars->next_variable)
	{
		if (vars->type == ASN_OCTET_STR)
		{
			char *sp = (char *)malloc(1 + vars->val_len);
			memcpy(sp, vars->val.string, vars->val_len);
			sp[vars->val_len] = '\0';
			printf("value #%d is a string: %s\n", count, sp);
			free(sp);
		}
		else if(vars->type == ASN_OBJECT_ID)
		{

			printf("value #%d is a oid: ", count);
			for(i=0; i<vars->name_length; i++)
			{
				if(*(vars->name_loc+i) == 0)
					break;
				printf(".%d", *(vars->name_loc+i));
			}
			printf(" and value is ");

			char oidbuf[20] ;
			string Stroid,Strgetid;
			Stroid = "";
			for(i=0; i<(vars->val_len/sizeof(int)); i++)
			{
				memset(oidbuf,0,20);
				sprintf(oidbuf,".%d", *(vars->val.objid+i));
				Strgetid = oidbuf;
				Stroid = Stroid + Strgetid;
			}
			printf("%s\r\n",Stroid.c_str()) ;
			if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.25") == 0)//高温告警
			{
				WriteLog("Env Temp Alarm!\r\n");
				HUAWEIDevAlarm.hwEnvTempAlarmTraps="1";
				HUAWEIDevAlarm.hwEnvTempAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.26") == 0)//高温告警恢复
			{
				WriteLog("Env Temp Alarm restore!\r\n");
				HUAWEIDevAlarm.hwEnvTempAlarmTraps="0";
				HUAWEIDevAlarm.hwEnvTempAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.27") == 0)//高湿告警
			{
				WriteLog("Env Humi alarm!\r\n");
				HUAWEIDevAlarm.hwEnvHumiAlarmTraps="1";
				HUAWEIDevAlarm.hwEnvHumiAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.28") == 0)//高湿告警恢复
			{
				WriteLog("Env Humi restore!\r\n");
				HUAWEIDevAlarm.hwEnvHumiAlarmTraps="0";
				HUAWEIDevAlarm.hwEnvHumiAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.31") == 0)//门禁告警
			{
				WriteLog("Door open alarm!\r\n");
				HUAWEIDevAlarm.hwDoorAlarmTraps="1";
				HUAWEIDevAlarm.hwDoorAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.32") == 0)//门禁告警恢复
			{
				WriteLog("Door close restore!\r\n");
				HUAWEIDevAlarm.hwDoorAlarmTraps="0";
				HUAWEIDevAlarm.hwDoorAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.33") == 0)//水浸告警
			{
				WriteLog("Water Alarm!\r\n");
				HUAWEIDevAlarm.hwWaterAlarmTraps="1";
				HUAWEIDevAlarm.hwWaterAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.34") == 0)//水浸告警恢复
			{
				WriteLog("Water Alarm restore!\r\n");
				HUAWEIDevAlarm.hwWaterAlarmTraps="0";
				HUAWEIDevAlarm.hwWaterAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.35") == 0)//烟雾告警
			{
				WriteLog("Smoke Alarm!\r\n");
				HUAWEIDevAlarm.hwSmokeAlarmTraps="1";
				HUAWEIDevAlarm.hwSmokeAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.36") == 0)//烟雾告警恢复
			{
				WriteLog("Smoke Alarm restore!\r\n");
				HUAWEIDevAlarm.hwSmokeAlarmTraps="0";
				HUAWEIDevAlarm.hwSmokeAlarmResume="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.57") == 0)//防雷器告警
			{
				WriteLog("AC Spd Alarm!\r\n");
				HUAWEIDevAlarm.hwACSpdAlarmTraps="1";
				HUAWEIDevAlarm.hwACSpdAlarmResumeTraps="0";
			}
			else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.58") == 0)//防雷器告警恢复
			{
				WriteLog("AC Spd Alarm restore!\r\n");
				HUAWEIDevAlarm.hwACSpdAlarmTraps="0";
				HUAWEIDevAlarm.hwACSpdAlarmResumeTraps="0";
			}

            string mstrjson;
            string mstrkey = "";
            SetjsonTableStr("flagrunstatusalarm",mstrjson);
            //SetjsonTableStr("flagrunstatusalarm",jsonPack,&jsonPackLen);
            printf("%s",mstrjson.c_str());
            //HttpPostParm(StrServerURL1,jsonPack,&jsonPackLen,HTTPPOST);
            HttpPostParm(StrServerURL1,mstrjson,mstrkey,HTTPPOST);

            NetSendParm(NETCMD_FLAGRUNSTATUS,(char *)(mstrjson.c_str()),mstrjson.size());


		}
		else
		{
			unsigned int IntegerValue = *(vars->val.integer) ;
			printf("value #%d is a integer: %d\n", count, IntegerValue);
		}
		count++;
	}

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
	HUAWEIDevAlarm.hwEnvTempAlarmTraps="0";		//高温/低温告警
	HUAWEIDevAlarm.hwEnvTempAlarmResume="0";	//高温/低温告警恢复
	HUAWEIDevAlarm.hwEnvHumiAlarmTraps="0";		//高湿/低湿告警
	HUAWEIDevAlarm.hwEnvHumiAlarmResume="0";	//高湿/低湿告警恢复
	HUAWEIDevAlarm.hwSpareDigitalAlarmTraps="0";	//输入干接点告警
	HUAWEIDevAlarm.hwSpareDigitalAlarmResume="0";	//输入干接点告警恢复
	HUAWEIDevAlarm.hwDoorAlarmTraps="0";		//门禁告警
	HUAWEIDevAlarm.hwDoorAlarmResume="0";		//门禁告警恢复
	HUAWEIDevAlarm.hwWaterAlarmTraps="0";		//水浸告警
	HUAWEIDevAlarm.hwWaterAlarmResume="0";		//水浸告警恢复
	HUAWEIDevAlarm.hwSmokeAlarmTraps="0";		//烟感告警
	HUAWEIDevAlarm.hwSmokeAlarmResume="0";		//烟感告警恢复
	HUAWEIDevAlarm.hwair_cond_infan_alarm="0";		//空调内风机故障
	HUAWEIDevAlarm.hwair_cond_outfan_alarm="0";		//空调外风机故障
	HUAWEIDevAlarm.hwair_cond_comp_alarm="0";		//空调压缩机故障
	HUAWEIDevAlarm.hwair_cond_return_port_sensor_alarm="0";		//空调回风口传感器故障
	HUAWEIDevAlarm.hwair_cond_evap_freezing_alarm="0";		//空调蒸发器冻结
	HUAWEIDevAlarm.hwair_cond_freq_high_press_alarm="0";		//空调频繁高压力
	HUAWEIDevAlarm.hwair_cond_comm_fail_alarm="0";		//空调通信失败告警
}


int snmptrapInit(void)
{
   pthread_t m_snmptrapthread ;
   pthread_create(&m_snmptrapthread,NULL,snmptrapthread,NULL);

   return 0 ;

}









