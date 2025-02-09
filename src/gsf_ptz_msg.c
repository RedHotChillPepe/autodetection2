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
#include "gsf_ptz_msg.h"
#include "gsf_ptz.h"

void *gsf_ptz_msg_task(void *PData)
{
	int ret = 0;
	GSF_PTZ_NOTIFY_MSG_HDL *PHdl = (GSF_PTZ_NOTIFY_MSG_HDL *)PData;
	GSF_PTZ_CMD_S PtzData={0};
	if (0 > (PHdl->nSock = gsf_create_local_udp_server(GSF_PTZ_NOTIFY_MSG_PATH)))
	{
		printf( "gsf_create_local_tcp_server(%s) err %d(%s)\n", GSF_PTZ_NOTIFY_MSG_PATH, errno, strerror(errno));
		return NULL;
	}
	while(PHdl->nExit == 0)
	{
		if ((ret = gsf_select(&PHdl->nSock, 1, 0x1, 1000)) < 0)
		{
			printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
			usleep(10000);
			continue;
		}
		else if (ret == 0 || ret != (0x10000 | PHdl->nSock))
		{
			printf("[%s %d] select timeout\n", __func__, __LINE__);
			continue;
		}
		memset(&PtzData,0,sizeof(PtzData));
		if ((ret = gsf_local_udp_recv(PHdl->nSock, (unsigned char *)&PtzData, sizeof(GSF_PTZ_CMD_S))) <= 0)
		{
			printf("[%s %d] local udp recv ret(%d) err %d(%s)\n", __func__, __LINE__, ret, errno, strerror(errno));
			continue;
		}
		if (PHdl->fCallback)
		{
			PHdl->fCallback(PtzData.cmd, PtzData.addr, \
							PtzData.data1, PtzData.data2, \
							PtzData.funNo);
		}
	}
	gsf_close_socket(&PHdl->nSock);
}

int gsf_ptz_msg_stop(void *hdl)
{
	if (hdl == NULL)
		return 0;
	
	GSF_PTZ_NOTIFY_MSG_HDL *pHdl = (GSF_PTZ_NOTIFY_MSG_HDL *)hdl;
	pHdl->nExit = 1;
	pthread_join(pHdl->thid, NULL);
	
	return 0;
}


