/*
 * ipscan.c
 *
 *  Created on: 2011-9-27
 *      Author: hl
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include "ipscan.h"
#include <string.h>
#include "server.h"
#include <unistd.h>

extern void GetIPinfo(IPInfo *ipInfo);

void *IpScan_thread (void *param)
{
printf("IpScan_thread 111\n");
	int ipscan_fd;
	struct sockaddr_in ipscan_addr;
	struct sockaddr_in owner_addr;
	int scan_cmd;
	IPInfo ipinfo;
	socklen_t fromlen = sizeof(owner_addr);

	GetIPinfo(&ipinfo);

	if ((ipscan_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("IpScan Create error");
	}

printf("IpScan_thread 222\n");
	int yes = 1;

	if (setsockopt(ipscan_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) 
	{
		perror("IpScan set opt error");
		close(ipscan_fd);
	}

printf("IpScan_thread 333\n");
	int so_broadcast = 1;

	if (setsockopt(ipscan_fd, SOL_SOCKET, SO_BROADCAST, &so_broadcast,
			sizeof(int)) < 0) {
		perror("Broadcast UDP set socket error");
		close(ipscan_fd);
	}

printf("IpScan_thread 444\n");
	memset(&owner_addr, 0, sizeof(owner_addr));
	owner_addr.sin_family = AF_INET;
	owner_addr.sin_port = htons(6800);
	owner_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&owner_addr.sin_zero, '\0', sizeof(owner_addr.sin_zero));
	bind(ipscan_fd, (struct sockaddr *) &owner_addr, sizeof(owner_addr));

	memset(&ipscan_addr, 0, sizeof(ipscan_addr));
	ipscan_addr.sin_family = AF_INET;
	ipscan_addr.sin_port = htons(6801);
	ipscan_addr.sin_addr.s_addr = INADDR_BROADCAST;
	memset(&ipscan_addr.sin_zero, '\0', sizeof(ipscan_addr.sin_zero));

printf("IpScan_thread 555\n");
	if (sendto(ipscan_fd, (void *) &ipinfo, sizeof(ipinfo), 0,
			(struct sockaddr *) &ipscan_addr, sizeof(ipscan_addr)) < 0)
		perror("IpScan sendto error");

printf("IpScan_thread 666\n");
	while (1) {

		if (recvfrom(ipscan_fd, &scan_cmd, sizeof(int), 0,(struct sockaddr *) &owner_addr, &fromlen) == -1) 
		{
printf("IpScan_thread 777\n");
			perror("recvfrom");
			continue;
		}
printf("IpScan_thread 888\n");
		printf("scan_cmd %d\n",scan_cmd);
		if (scan_cmd == 0x9100) {
printf("IpScan_thread 999\n");

			memset(&ipscan_addr, 0, sizeof(ipscan_addr));
			ipscan_addr.sin_family = AF_INET;
			ipscan_addr.sin_port = htons(6801);
			ipscan_addr.sin_addr.s_addr = INADDR_BROADCAST;
//			ipscan_addr.sin_addr.s_addr = inet_addr("192.168.1.255");
			memset(&ipscan_addr.sin_zero, '\0', sizeof(ipscan_addr.sin_zero));

			if (sendto(ipscan_fd, (void *) &ipinfo, sizeof(ipinfo), 0,
					(struct sockaddr *) &ipscan_addr, sizeof(ipscan_addr)) < 0)
				perror("IpScan sendto error");

		} else {

printf("IpScan_thread aaa\n");
			continue;

		}
		sleep(10);
	}

}

void init_IpScan()
{
	pthread_t m_IpScan_thread;
	pthread_create(&m_IpScan_thread,NULL,IpScan_thread,NULL);
}


