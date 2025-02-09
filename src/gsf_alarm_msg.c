/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_alarm_msg.c

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/08/29
  Description   :
  History       :
  1.Date        : 2021/08/29
    Author      :
    Modification: Created file

******************************************************************************/
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <time.h>
#include "devsdk.h"
#include "gsf_base.h"
#include "gsf_alarm_msg.h"

#define GSF_ALARM_MSG_SVR	"/tmp/devsdkalarm"

unsigned int gsf_alarm_get_time(GSF_ALARM_DATA_S *pstAlarm)
{
	if (pstAlarm)
	{
		time_t tTime = 0;
		struct tm tmTime = {0};
		tmTime.tm_year = pstAlarm->u8Year;
		tmTime.tm_mon  = (pstAlarm->u8Month%13) - 1;
		tmTime.tm_mday = pstAlarm->u8Date;
		tmTime.tm_hour = pstAlarm->u8Hour;
		tmTime.tm_min  = pstAlarm->u8Minute;
		tmTime.tm_sec  = pstAlarm->u8Second;
		tTime = mktime(&tmTime);
		return (unsigned int)tTime;
	}
	return 0;
}

#if 1
void *gsf_alarm_msg_task(void *h)
{
	int ret = 0;
	char szData[128] = {0};
	GSF_ALARM_MSG_S *pstAlarmMsg = {0};
	// send request & recv response
	GSF_ALARM_MSG_HDL *pHdl = (GSF_ALARM_MSG_HDL *)h;
	do
	{
		if (pHdl == NULL)
			break;
		
		// create local udp server
		if ((pHdl->nSock =  gsf_create_local_udp_server(GSF_ALARM_MSG_SVR)) < 0)
		{
			printf("create local server err %d(%s)\n", errno, strerror(errno));
			break;
		}
		// wait for alarm msg
		while(pHdl->nExit == 0)
		{
			if ((ret = gsf_select(&pHdl->nSock, 1, 0x1, 1000)) < 0)
			{
				printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
				sleep(1);
				continue;
			}
			else if (ret == 0 || ret != (0x10000 | pHdl->nSock))
			{
				//printf("[%s %d] waiting for alarm msg\n", __func__, __LINE__);
				continue;
			}
			memset(szData, 0, sizeof(szData));
			if ((ret = gsf_local_udp_recv(pHdl->nSock, (unsigned char *)&szData, sizeof(szData))) <= 0)
			{
				printf("[%s %d] local udp recv ret(%d) err %d(%s)\n", __func__, __LINE__, ret, errno, strerror(errno));
				continue;
			}
			pstAlarmMsg = (GSF_ALARM_MSG_S *)szData;
			if (pstAlarmMsg->stHead.u32Size < sizeof(GSF_ALARM_DATA_S))
			{
				printf("[%s %d] alarm size(%d) expected >= %d\n", __func__, __LINE__, pstAlarmMsg->stHead.u32Size, sizeof(GSF_ALARM_DATA_S));
				continue;
			}
			/*printf("[%s %d] ret(%d) size(%d) ch(%hhu) type(%hhu) action(%hhu) jpeg(%hhu %s) time(%04d-%02d-%02d %02d:%02d:%02d:%02d %02d)\n", 
				__func__, __LINE__, ret, sizeof(GSF_ALARM_MSG_S), 
				pstAlarmMsg->stData.u8Channel, pstAlarmMsg->stData.u8Type, pstAlarmMsg->stData.u8Action, 
				pstAlarmMsg->stData.u8Jpeg, pstAlarmMsg->szFileName, 
				pstAlarmMsg->stData.u8Year+1900, pstAlarmMsg->stData.u8Month, 
				pstAlarmMsg->stData.u8Date, pstAlarmMsg->stData.u8Hour, pstAlarmMsg->stData.u8Minute, 
				pstAlarmMsg->stData.u8Second, pstAlarmMsg->stData.u8MSec, pstAlarmMsg->stData.u8Weekday);
			*/
			if (pHdl->fCallback)
			{
				pHdl->fCallback(pstAlarmMsg->stData.u8Channel, pstAlarmMsg->stData.u8Type, \
								pstAlarmMsg->stData.u8Action, gsf_alarm_get_time(&pstAlarmMsg->stData), \
								pstAlarmMsg->szFileName, pHdl->pUser);
			}
		}
	}while(0);
	if (pHdl)
	{
		gsf_close_socket(&pHdl->nSock);
		unlink(GSF_ALARM_MSG_SVR);
	}
	return NULL;
}
#else
void *gsf_alarm_msg_task(void *h)
{
	int ret = 0;
	GSF_ALARM_MSG_S stAlarmMsg = {0};
	//char szData[128] = {0};
	// send request & recv response
	GSF_ALARM_MSG_HDL *pHdl = (GSF_ALARM_MSG_HDL *)h;
	do
	{
		if (pHdl == NULL)
			break;
		
		// create local udp server
		if ((pHdl->nSock =  gsf_create_local_udp_server(GSF_ALARM_MSG_SVR)) < 0)
		{
			printf("create local server err %d(%s)\n", errno, strerror(errno));
			break;
		}
		// wait for alarm msg
		while(pHdl->nExit == 0)
		{
			if ((ret = gsf_select(&pHdl->nSock, 1, 0x1, 1000)) < 0)
			{
				printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
				sleep(1);
				continue;
			}
			else if (ret == 0 || ret != (0x10000 | pHdl->nSock))
			{
				//printf("[%s %d] waiting for alarm msg\n", __func__, __LINE__);
				continue;
			}
			memset(&stAlarmMsg, 0, sizeof(stAlarmMsg));
			if (sizeof(GSF_ALARM_HEAD_S) != (ret = gsf_local_udp_recv(pHdl->nSock, &stAlarmMsg.stHead, sizeof(GSF_ALARM_HEAD_S))))
			{
				printf("[%s %d] local udp recv expected len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(GSF_ALARM_HEAD_S), ret, errno, strerror(errno));
				continue;
			}
			if (stAlarmMsg.stHead.u32Size == 0)
			{
				printf("[%s %d] invalid alarm msg size(%d)\n", __func__, __LINE__, stAlarmMsg.stHead.u32Size);
				continue;
			}
			if (stAlarmMsg.stHead.u32Size >= sizeof(GSF_ALARM_DATA_S))
			{
				if (sizeof(GSF_ALARM_DATA_S) != (ret = gsf_local_udp_recv(pHdl->nSock, &stAlarmMsg.stData, sizeof(GSF_ALARM_DATA_S))))
				{
					printf("[%s %d] local udp recv expected len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(GSF_ALARM_DATA_S), ret, errno, strerror(errno));
					continue;
				}
				printf("[%s %d] ch(%hhu) type(%hhu) action(%hhu) jpeg(%hhu) time(%04d-%02d-%02d %02d:%02d:%02d:%02d %02d)\n", 
					__func__, __LINE__, 
					stAlarmMsg.stData.u8Channel, stAlarmMsg.stData.u8Type, stAlarmMsg.stData.u8Action, 
					stAlarmMsg.stData.u8Jpeg, stAlarmMsg.stData.u8Year+1900, stAlarmMsg.stData.u8Month, 
					stAlarmMsg.stData.u8Date, stAlarmMsg.stData.u8Hour, stAlarmMsg.stData.u8Minute, 
					stAlarmMsg.stData.u8Second, stAlarmMsg.stData.u8MSec, stAlarmMsg.stData.u8Weekday);
				if (stAlarmMsg.stData.u8Jpeg && stAlarmMsg.stHead.u32Size >= (sizeof(GSF_ALARM_DATA_S)+GSF_ALARM_FILE_NAME_LEN))
				{
					if (GSF_ALARM_FILE_NAME_LEN != (ret = gsf_local_udp_recv(pHdl->nSock, &stAlarmMsg.szFileName, GSF_ALARM_FILE_NAME_LEN)))
					{
						printf("[%s %d] local udp recv filename len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, GSF_ALARM_FILE_NAME_LEN, ret, errno, strerror(errno));
						// continue;
					}
				}
			}
			if (pHdl->fCallback)
			{
				pHdl->fCallback(stAlarmMsg.stData.u8Channel, stAlarmMsg.stData.u8Type, \
								stAlarmMsg.stData.u8Action, gsf_alarm_get_time(&stAlarmMsg.stData), \
								stAlarmMsg.szFileName, pHdl->pUser);
			}
		}
	}while(0);
	if (pHdl)
	{
		gsf_close_socket(&pHdl->nSock);
		unlink(GSF_ALARM_MSG_SVR);
	}
	return NULL;
}
#endif

int gsf_alarm_msg_stop(void *hdl)
{
	if (hdl == NULL)
		return 0;
	
	GSF_ALARM_MSG_HDL *pHdl = (GSF_ALARM_MSG_HDL *)hdl;
	pHdl->nExit = 1;
	pthread_join(pHdl->thid, NULL);
	
	return 0;
}