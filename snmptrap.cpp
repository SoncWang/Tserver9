#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_api.h>
#include <net-snmp/agent/ds_agent.h>
#include <string> 
#include "snmp.h"

using namespace std; 

int      netsnmp_running = 1;

extern string StrServerURL1;
THUAWEIALARM HUAWEIDevAlarm;		//��Ϊ����澯
extern void SetjsonTableStr(char* table, char *json, int *lenr);
extern int HttpPostParm(string url,char *pParmbuf,int parmlen);

/************************************************************************
** 
** 
************************************************************************/

int snmp_input(int op, netsnmp_session *session, int reqid, netsnmp_pdu *pdu, void *magic)
{
	int count = 0 ;
	int i ;
	int jsonPackLen=0;
	char * jsonPack=(char *)malloc(50*1024);
    printf("We got a trap:\n");
    struct variable_list *vars;
 /*   for(vars = pdu->variables; vars; vars = vars->next_variable)
    {
        print_variable(vars->name, vars->name_length, vars);
    }
*/
         for(vars = pdu->variables; vars; vars = vars->next_variable) {
            if (vars->type == ASN_OCTET_STR) {
/*
	       char *sp = (char *)malloc(1 + vars->val_len);
	       memcpy(sp, vars->val.string, vars->val_len);
	       sp[vars->val_len] = '/0';
               printf("value #%d is a string: %s/n", count++, sp);
	       free(sp);
*/
	    }
            else if(vars->type == ASN_OBJECT_ID)
            {
/*
                 printf("value #%d is a oid: ", count++);
                 for(i=0; i<vars->name_length; i++)
                 {
                     if(*(vars->name_loc+i) == 0)
                         break;
                     printf(".%d", *(vars->name_loc+i));
                 }
                 printf(" and value is ");
                 */
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
//                 printf("%s\r\n",Stroid.c_str()) ;
                 if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.31") == 0)
                 {
 				   	  printf("Door open alarm!\r\n\n\n\n");
                 	  HUAWEIDevAlarm.hwDoorAlarmTraps="1";
                 	  HUAWEIDevAlarm.hwDoorAlarmResume="0";
                 }
                 else if(strcmp(Stroid.c_str(),".1.3.6.1.4.1.2011.6.164.2.1.0.32") == 0)
                 {
                     printf("Door close restore!\r\n\n\n\n");
					 HUAWEIDevAlarm.hwDoorAlarmTraps="0";
					 HUAWEIDevAlarm.hwDoorAlarmResume="0";
                 }
				 
/*				 memset(jsonPack,0,50*1024);
				 SetjsonFlagRunAlarmStr(jsonPack,&jsonPackLen);
				 printf("%s",jsonPack);
				 HttpPostParm(StrAlarmURL,jsonPack,jsonPackLen);*/
				 
				 memset(jsonPack,0,50*1024);
				 SetjsonTableStr("flagrunstatusalarm",jsonPack,&jsonPackLen);
				 printf("%s",jsonPack);
				 HttpPostParm(StrServerURL1,jsonPack,jsonPackLen);
	  		 	 NetSendParm(NETCMD_FLAGRUNSTATUS,jsonPack,jsonPackLen);
            
                 
            }
            else
            {
/*
              unsigned int IntegerValue = *(vars->val.integer) ;
              printf("value #%d is a integer: %d\n", count++, IntegerValue);
  */            
            }
         }

	free(jsonPack);

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
	HUAWEIDevAlarm.hwEnvTempAlarmTraps="0";		//����/���¸澯
	HUAWEIDevAlarm.hwEnvTempAlarmResume="0";	//����/���¸澯�ָ�
	HUAWEIDevAlarm.hwEnvHumiAlarmTraps="0";		//��ʪ/��ʪ�澯
	HUAWEIDevAlarm.hwEnvHumiAlarmResume="0";	//��ʪ/��ʪ�澯�ָ�
	HUAWEIDevAlarm.hwSpareDigitalAlarmTraps="0";	//����ɽӵ�澯
	HUAWEIDevAlarm.hwSpareDigitalAlarmResume="0";	//����ɽӵ�澯�ָ�
	HUAWEIDevAlarm.hwDoorAlarmTraps="0";		//�Ž��澯
	HUAWEIDevAlarm.hwDoorAlarmResume="0";		//�Ž��澯�ָ�
	HUAWEIDevAlarm.hwWaterAlarmTraps="0";		//ˮ���澯
	HUAWEIDevAlarm.hwWaterAlarmResume="0";		//ˮ���澯�ָ�
	HUAWEIDevAlarm.hwSmokeAlarmTraps="0";		//�̸и澯
	HUAWEIDevAlarm.hwSmokeAlarmResume="0";		//�̸и澯�ָ�
}




int snmptrapInit(void)
{
	initHUAWEIALARM();

   pthread_t m_snmptrapthread ;
   pthread_create(&m_snmptrapthread,NULL,snmptrapthread,NULL);

//	HUAWEIDevAlarm.hwDoorAlarmTraps="0";
   return 0 ;

}








