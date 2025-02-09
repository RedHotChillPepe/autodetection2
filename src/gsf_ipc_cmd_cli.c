/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_ipc_cmd_cli.c

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/01/16
  Description   :
  History       :
  1.Date        : 2021/01/16
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
#include "gsf_ipc_cmd.h"
#include "gsf_ipc_cmd_cli.h"

#define GSF_IPC_CMD_MAX_RETRY 5
#define GSF_DEFAULT_LOCAL_TIME 2

int gsf_clear_recv_buf(int fd, int maxSize)
{
	int ret = 0;
	int clrSize = 0;
	unsigned char data[32] = {0};
	while(clrSize < maxSize)
	{
		if ((ret = gsf_select_simple(fd, 0)) < 0)
		{
			printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
			if (errno == EAGAIN || errno == EINTR)
			{
				sleep(1);
				continue;
			}
			break;
		}
		else if (ret == 0)
		{
			printf("[%s %d] clear recv buf size(%d)\n", __func__, __LINE__, clrSize);
			break;
		}
		gsf_local_tcp_recv(fd, data, 1);
		clrSize += 1;
	}
	return 0;
}

int ipc_cmd_req(int nSock, GSF_IPC_CMD_MSG_S *pReqMsg, GSF_IPC_CMD_MSG_S *pRspMsg)
{
	int ret = 0;
	int len = sizeof(GSF_IPC_CMD_MSG_S);
	if (len != (ret = gsf_local_tcp_send(nSock, (unsigned char *)pReqMsg, len)))
	{
		printf("[%s %d] tcp send len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, len, ret, errno, strerror(errno));
		return DEVSDK_ERR_SEND;
	}
	// recv response
	len = sizeof(GSF_IPC_CMD_MSG_S);
	if (0 > (ret = gsf_local_tcp_recv(nSock, (unsigned char *)pRspMsg, len)))
	{
		printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, len, ret, errno, strerror(errno));
		return DEVSDK_ERR_RECV;
		//break;
	}
	//printf("[%s %d] magic(%0#x) seq(%d) cmd(%d) dir(%d) type(%d) err(%d)\n", __func__, __LINE__, 
	//	pRspMsg->magic, pRspMsg->seq, pRspMsg->cmd, pRspMsg->dir, 
	//	pRspMsg->type, pRspMsg->err);
	return 0;
}

int gsf_ipc_cmd_cli_release_file(int nSock, const char *pszFileName)
{
	GSF_IPC_CMD_MSG_S stReqMsg = {0};
	GSF_IPC_CMD_MSG_S stRspMsg = {0};
	stReqMsg.magic = GSF_IPC_CMD_MAGIC;
	stReqMsg.seq = 1;
	stReqMsg.cmd = GSF_IPC_CMD_DEL_FILE;
	stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
	strncpy(stReqMsg.param.path, pszFileName, GSF_IPC_CMD_MAX_PATH_LEN-1);
	if (nSock < 0 &&
		(nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH)) < 0)
	{
		printf("[%s %d] connect serv(%s) err(%d %s)!!\n", __func__, __LINE__, GSF_IPC_CMD_SERV_PATH, errno, strerror(errno));
		return -1;
	}
	if (ipc_cmd_req(nSock, &stReqMsg, &stRspMsg) != 0 ||
		stRspMsg.err != GSF_IPC_CMD_ERR_OK)
	{
		printf("[%s %d] del file(%s) err(%d)!!\n", __func__, __LINE__, pszFileName, stRspMsg.err);
		return -1;
	}

	if(nSock >= 0)
		gsf_close_socket(&nSock);
	
	return 0;
}
//抓拍图片
int ipc_cmd_cli_proc_pic( int nChn,const char *pszFileName)
{
	// send request & recv response
	int seq = 0;
	int len = 0;
	int ret = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	// create local socket
	int nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
	//int nSock = ipc_cmd_connect_svr(GSF_IPC_CMD_SVR, GSF_IPC_CMD_PORT);
	if (nSock < 0)
	{
		printf("[%s %d] connect local server(%s) err %d(%s)\n", __func__, __LINE__, GSF_IPC_CMD_SERV_PATH, errno, strerror(errno));
		return nSock;
	}
	while(1)
	{
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = seq++;
		stReqMsg.cmd = GSF_IPC_CMD_PIC;
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_FILE;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		strcpy(stReqMsg.param.path, pszFileName);
		
		if (ipc_cmd_req(nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			break;
		}
		if (stRspMsg.type == GSF_IPC_CMD_TYPE_FILE)
		{
			printf("[%s %d] file(%s)\n", __func__, __LINE__, stRspMsg.param.path);
		}
		else if (stRspMsg.type == GSF_IPC_CMD_TYPE_DATA)
		{
			printf("[%s %d] pic type(%d) unsupport!!\n", __func__, __LINE__, stRspMsg.type);
			/*char *pData = (char *)calloc(1, stRspMsg.param.size);
			if (pData)
			{
				if (stRspMsg.param.size != (ret = gsf_local_tcp_recv(nSock, pData, stRspMsg.param.size)))
				{
					printf("[%s %d] local_tcp_recv ret(%d) err %d(%s)\n", __func__, __LINE__, ret, errno, strerror(errno));
				}
				else
				{
					printf("[%s %d] local_tcp_recv ret(%d) ok\n", __func__, __LINE__, ret);
				}
				free(pData);
			}
			else
			{
				printf("[%s %d] calloc buf size(%d) err %d(%s)\n", __func__, __LINE__, 
					stRspMsg.param.size, errno, strerror(errno));
			}*/
		}
		//break;
		sleep(1);
		// delete after use
		//gsf_ipc_cmd_cli_release_file(nSock, stRspMsg.param.path);
		break;
	}
	close(nSock);
	return 0;
}

int ipc_cmd_cli_proc_pic_v2( int nChn, int nStream, const char *pszFileName)
{
	// send request & recv response
	int seq = 0;
	int len = 0;
	int ret = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	// create local socket
	int nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
	//int nSock = ipc_cmd_connect_svr(GSF_IPC_CMD_SVR, GSF_IPC_CMD_PORT);
	if (nSock < 0)
	{
		printf("[%s %d] connect local server(%s) err %d(%s)\n", __func__, __LINE__, GSF_IPC_CMD_SERV_PATH, errno, strerror(errno));
		return nSock;
	}
	while(1)
	{
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = seq++;
		stReqMsg.cmd = GSF_IPC_CMD_PIC_V2;
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_FILE;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		stReqMsg.param.pic.nCh = nChn;
		stReqMsg.param.pic.nStream = nStream;
		strcpy(stReqMsg.param.path, pszFileName);
		
		if (ipc_cmd_req(nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			break;
		}
		if (stRspMsg.type == GSF_IPC_CMD_TYPE_FILE)
		{
			printf("[%s %d] file(%s)\n", __func__, __LINE__, stRspMsg.param.path);
		}
		else if (stRspMsg.type == GSF_IPC_CMD_TYPE_DATA)
		{
			printf("[%s %d] pic type(%d) unsupport!!\n", __func__, __LINE__, stRspMsg.type);
			/*char *pData = (char *)calloc(1, stRspMsg.param.size);
			if (pData)
			{
				if (stRspMsg.param.size != (ret = gsf_local_tcp_recv(nSock, pData, stRspMsg.param.size)))
				{
					printf("[%s %d] local_tcp_recv ret(%d) err %d(%s)\n", __func__, __LINE__, ret, errno, strerror(errno));
				}
				else
				{
					printf("[%s %d] local_tcp_recv ret(%d) ok\n", __func__, __LINE__, ret);
				}
				free(pData);
			}
			else
			{
				printf("[%s %d] calloc buf size(%d) err %d(%s)\n", __func__, __LINE__, 
					stRspMsg.param.size, errno, strerror(errno));
			}*/
		}
		//break;
		sleep(1);
		// delete after use
		//gsf_ipc_cmd_cli_release_file(nSock, stRspMsg.param.path);
		break;
	}
	close(nSock);
	return 0;
}

//抓拍图片数据
void *gsf_ipc_cmd_cli_capture_task(void *h)
{
	// send request & recv response
	int ret = 0;
	int nRecvSize = 8*1024;
	char szFileName[64] = {0};
	FILE *fp = NULL;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	GSF_IPC_CMD_CAPTURE_HDL *pCapHdl = (GSF_IPC_CMD_CAPTURE_HDL *)h;
	do
	{
		if (pCapHdl == NULL)
			break;
		
		// create local socket
		pCapHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
		if (pCapHdl->nSock < 0)
		{
			printf("connect local server err %d(%s)\n", errno, strerror(errno));
			break;
		}
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pCapHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_PIC;

		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_DATA;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		
		if (ipc_cmd_req(pCapHdl->nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			break;
		}
		if (1)
		{ 
			DEVSDK_FRAME_HEAD_S *pFrameHead = NULL;
			pCapHdl->nBufSize = 2<<20;
			pCapHdl->pBuf = calloc(1, pCapHdl->nBufSize);
			if (pCapHdl->pBuf == NULL)
			{
				printf("[%s %d] calloc(1, %d) err %d(%s)\n", __func__, __LINE__, pCapHdl->nBufSize, errno, strerror(errno));
				pCapHdl->nBufSize = 0;
				break;
			}
 
			if ((ret = gsf_select_simple(pCapHdl->nSock, 1000)) < 0)
			{
				printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
				sleep(1);
				continue;
			}
			else if (ret == 0)
			{
				printf("[%s %d] waiting for data \n", __func__, __LINE__ );
				continue;
			} 
 
				
			if (sizeof(DEVSDK_FRAME_HEAD_S) != (ret = gsf_local_tcp_recv(pCapHdl->nSock, pCapHdl->pBuf, sizeof(DEVSDK_FRAME_HEAD_S))))
			{
				printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(DEVSDK_FRAME_HEAD_S), ret, errno, strerror(errno));
				break;
			}
			pFrameHead = (DEVSDK_FRAME_HEAD_S *)pCapHdl->pBuf;
			
			// 确保接收缓冲足够大
			if (pFrameHead->u32FrameSize > pCapHdl->nBufSize-sizeof(DEVSDK_FRAME_HEAD_S))
			{
				DEVSDK_FRAME_HEAD_S stTempHead = {0};
				// step 1: save FRAME HEAD
				memcpy(&stTempHead, pFrameHead, sizeof(DEVSDK_FRAME_HEAD_S));
				// step 2: realloc buffer
				pCapHdl->pBuf = (unsigned char *)gsf_realloc(pCapHdl->pBuf, pFrameHead->u32FrameSize+sizeof(DEVSDK_FRAME_HEAD_S), &pCapHdl->nBufSize);
			}
			if (pFrameHead->u32FrameSize > pCapHdl->nBufSize-sizeof(DEVSDK_FRAME_HEAD_S))
			{
				printf("realloc buffer size(%d -> %d) err %d(%s)\n", 
					pCapHdl->nBufSize, pFrameHead->u32FrameSize+sizeof(DEVSDK_FRAME_HEAD_S), errno, strerror(errno));
				break;
			}

			int total_len = pFrameHead->u32FrameSize;
			int offset = 0;
			int revret = 0;					
			while(total_len > 0)
			{
				int each = (total_len) >= nRecvSize ? nRecvSize : total_len;
				revret = gsf_local_tcp_recv(pCapHdl->nSock, pCapHdl->pBuf+sizeof(DEVSDK_FRAME_HEAD_S)+offset, each);
								
				if(ret == -1) 
				{
					printf("[%s %d] pic data send err!!!\n",__FUNCTION__,__LINE__);
					return NULL;
				}
				offset += revret;
				total_len -= revret;
			}

			
			sprintf(szFileName,"/mnt/gsf_sensor415_%d_0_1_%d.jpg",pCapHdl->nChn,pFrameHead->u32FrameNo);
			
			fp = fopen(szFileName,"wb+");
			if(fp == NULL)
			{
				printf("fopen err\n");
				return NULL;
			}

			fwrite(pCapHdl->pBuf+sizeof(DEVSDK_FRAME_HEAD_S),1,pFrameHead->u32FrameSize,fp);
			
			if (pCapHdl->fCallback)
				pCapHdl->fCallback( pCapHdl->pBuf+sizeof(DEVSDK_FRAME_HEAD_S), pFrameHead->u32FrameSize, pCapHdl->pUser);

		}
		else
		{
			printf("[%s %d] capture type(%d) unsupport!!\n", __func__, __LINE__, stRspMsg.type);
		}
	}while(0);
	
	if (pCapHdl)
	{
		if (pCapHdl->nSock >= 0)
		{
			close(pCapHdl->nSock);
		}
		if (pCapHdl->pBuf && pCapHdl->nBufSize > 0)
		{
			free(pCapHdl->pBuf);
			pCapHdl->nBufSize = 0;
		}
		free(pCapHdl);
	}

	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	
	pthread_detach(pthread_self());
	return NULL;
}

//录像文件
void *gsf_ipc_cmd_cli_RecFile_task(void *h)
{
	int len = 0;
	int ret = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	GSF_IPC_CMD_RECFILE_HDL *pHdl = (GSF_IPC_CMD_RECFILE_HDL *)h;
	do
	{
		if (pHdl == NULL)
			break;
		
		// create local socket
		pHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
		if (pHdl->nSock < 0)
		{
			printf("connect local server err %d(%s)\n", errno, strerror(errno));
			ret = DEVSDK_ERR_CONN;
			break;
		}
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_MP4_REC;

		stReqMsg.param.rec.type = pHdl->nRecType;  // IO Alarm Record 
		stReqMsg.param.rec.ctrl = GSF_IPC_CMD_VIDEO_CTRL_START;
		stReqMsg.param.rec.audio = 0;  // need audio
		stReqMsg.param.rec.speed = 2;
		stReqMsg.param.rec.start_time = pHdl->nStartTime;
		stReqMsg.param.rec.end_time   = pHdl->nStopTime;
		
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_FILE;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		
		if (ipc_cmd_req(pHdl->nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			ret = DEVSDK_ERR_CONN;
			break;
		}
		printf("[%s %d] file(%s) ctrl(%d) sock(%d)\n", __func__, __LINE__, stRspMsg.param.path, stRspMsg.param.video.ctrl,pHdl->nSock);
		while(pHdl->nExit == 0)
		{
			if ((ret = gsf_select_simple(pHdl->nSock, 1000)) < 0)
			{
				printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
				sleep(1);
			}
			else if (ret == 0)
			{
				printf("[%s %d] waiting for response\n", __func__, __LINE__);
				continue;
			}
			if (0 >= (ret = gsf_local_tcp_recv(pHdl->nSock, (unsigned char *)&stRspMsg, sizeof(stRspMsg))))
			//if (0 >= (ret = gsf_tcp_recv(nSock, (unsigned char *)&stRspMsg, sizeof(stRspMsg))))
			{
				printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(stRspMsg), ret, errno, strerror(errno));
				ret = DEVSDK_ERR_CONN;
				break;
			}
			
			printf("[%s %d] recv: magic(%0#x) seq(%d) cmd(%d) dir(%d) type(%d) ctrl(%d) err(%d) path(%s)\n", __func__, __LINE__, 
				stRspMsg.magic, stRspMsg.seq, stRspMsg.cmd, stRspMsg.dir, 
				stRspMsg.type, stRspMsg.param.video.ctrl, stRspMsg.err, stRspMsg.param.path);
			ret = DEVSDK_OK;
			break;
		}
	}while(0);

	if (pHdl && pHdl->fCallback)
		pHdl->fCallback(ret, stRspMsg.param.path, pHdl->pUser);
	
	if (pHdl)
	{
		if (pHdl->nSock >= 0)
		{
			close(pHdl->nSock);
		}
		if (pHdl->pBuf && pHdl->nBufSize > 0)
		{
			free(pHdl->pBuf);
			pHdl->nBufSize = 0;
		}

		free(pHdl);
	}
	
	pthread_detach(pthread_self());
	
	return NULL;
}

void *gsf_ipc_cmd_cli_AVFile_task_v2(void *h)
{
	int len = 0;
	int ret = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	GSF_IPC_CMD_VIDEO_HDL_V2 *pHdl = (GSF_IPC_CMD_VIDEO_HDL_V2 *)h;
	do
	{
		if (pHdl == NULL)
			break;
		
		// create local socket
		pHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
		if (pHdl->nSock < 0)
		{
			printf("connect local server err %d(%s)\n", errno, strerror(errno));
			ret = DEVSDK_ERR_CONN;
			break;
		}
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_MP4_VIDEO_V2;

		stReqMsg.param.videov2.mask = 0x03;
		stReqMsg.param.videov2.ctrl = GSF_IPC_CMD_VIDEO_CTRL_START;
		stReqMsg.param.videov2.audio = 1;  // need audio
		stReqMsg.param.videov2.stream = pHdl->nStream;  // IO Alarm Record 
		stReqMsg.param.videov2.speed = 2;
		stReqMsg.param.videov2.limit_size = 128<<20; // 128MB
		stReqMsg.param.videov2.StartPts = pHdl->uStartTimeMs;
		stReqMsg.param.videov2.StopPts  = pHdl->uStopTimeMs;
		
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_FILE;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		printf("get sz size(%d),but(%d) (%d)\n",sizeof(GSF_IPC_CMD_MSG_S),sizeof(stReqMsg),sizeof(stReqMsg));
		if (ipc_cmd_req(pHdl->nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			ret = DEVSDK_ERR_CONN;
			break;
		}
		printf("[%s %d] file(%s) ctrl(%d)\n", __func__, __LINE__, stRspMsg.param.path, stRspMsg.param.video.ctrl);
		while(pHdl->nExit == 0)
		{
			if ((ret = gsf_select_simple(pHdl->nSock, 1000)) < 0)
			{
				printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
				sleep(1);
			}
			else if (ret == 0)
			{
				//printf("[%s %d] waiting for response\n", __func__, __LINE__);
				continue;
			}
			if (0 >= (ret = gsf_local_tcp_recv(pHdl->nSock, (unsigned char *)&stRspMsg, sizeof(stRspMsg))))
			{
				printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(stRspMsg), ret, errno, strerror(errno));
				ret = DEVSDK_ERR_CONN;
				
				continue;
			}
			
			ret = DEVSDK_OK;
			
			if (pHdl && pHdl->fCallback)
				pHdl->fCallback(ret, stRspMsg.param.path, pHdl->pUser, stRspMsg.param.videov2.StartPts, stRspMsg.param.videov2.StopPts);
			printf("[%s %d] recv: magic(%0#x) seq(%d) cmd(%d) dir(%d) type(%d) ctrl(%d) err(%d) path(%s)\n", __func__, __LINE__, 
				stRspMsg.magic, stRspMsg.seq, stRspMsg.cmd, stRspMsg.dir, 
				stRspMsg.type, stRspMsg.param.video.ctrl, stRspMsg.err, stRspMsg.param.path);
			continue;
		}
		if(pHdl)
		{	
			stReqMsg.param.videov2.ctrl = GSF_IPC_CMD_VIDEO_CTRL_STOP;			
			if (ipc_cmd_req(pHdl->nSock, &stReqMsg, &stRspMsg) != 0 ||
					stRspMsg.err != GSF_IPC_CMD_ERR_OK)
			{
				printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
				ret = DEVSDK_ERR_CONN;
			}
			while(1)
			{
				if ((ret = gsf_select_simple(pHdl->nSock, 1000)) < 0)
				{
					printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
					break;
				}
				else if (ret == 0)
				{
					//printf("[%s %d] waiting for response\n", __func__, __LINE__);
					continue;
				}			
				if (0 >= (ret = gsf_local_tcp_recv(pHdl->nSock, (unsigned char *)&stRspMsg, sizeof(stRspMsg))))
				{
					printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(stRspMsg), ret, errno, strerror(errno));
					ret = DEVSDK_ERR_CONN;				
				}
				else
				{				
					ret = DEVSDK_OK;				
					if (pHdl->fCallback)
						pHdl->fCallback(ret, stRspMsg.param.path, pHdl->pUser, stRspMsg.param.videov2.StartPts, stRspMsg.param.videov2.StopPts);
					
					printf("[%s %d] recv: magic(%0#x) seq(%d) cmd(%d) dir(%d) type(%d) ctrl(%d) err(%d) path(%s)\n", __func__, __LINE__, 
						stRspMsg.magic, stRspMsg.seq, stRspMsg.cmd, stRspMsg.dir, 
						stRspMsg.type, stRspMsg.param.video.ctrl, stRspMsg.err, stRspMsg.param.path);
				}
				break;
			}				
		}		
	}while(0);
	printf("[%s %d] gsf_ipc_cmd_cli_AVFile_task_v2 exit\n",__func__,__LINE__);
	if (pHdl)
	{		
		if (pHdl->nSock >= 0)
		{
			close(pHdl->nSock);
		}
		if (pHdl->pBuf && pHdl->nBufSize > 0)
		{
			free(pHdl->pBuf);
			pHdl->nBufSize = 0;
		}
	}
	return NULL;
}


//音视频文件
void *gsf_ipc_cmd_cli_AVFile_task(void *h)
{
	int len = 0;
	int ret = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	GSF_IPC_CMD_VIDEO_HDL *pHdl = (GSF_IPC_CMD_VIDEO_HDL *)h;
	do
	{
		if (pHdl == NULL)
			break;
		
		// create local socket
		pHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
		if (pHdl->nSock < 0)
		{
			printf("connect local server err %d(%s)\n", errno, strerror(errno));
			ret = DEVSDK_ERR_CONN;
			break;
		}
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_MP4_VIDEO;

		stReqMsg.param.video.mask = 0x03;
		stReqMsg.param.video.ctrl = GSF_IPC_CMD_VIDEO_CTRL_START;
		stReqMsg.param.video.audio = 0;  // need audio
		stReqMsg.param.video.stream = pHdl->nStream;  // IO Alarm Record 
		stReqMsg.param.video.speed = 2;
		stReqMsg.param.video.limit_size = 128<<20; // 128MB
		stReqMsg.param.video.start_time = pHdl->nStartTime;
		stReqMsg.param.video.end_time   = pHdl->nStopTime;
		
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_FILE;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		
		if (ipc_cmd_req(pHdl->nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			ret = DEVSDK_ERR_CONN;
			break;
		}
		printf("[%s %d] file(%s) ctrl(%d)\n", __func__, __LINE__, stRspMsg.param.path, stRspMsg.param.video.ctrl);
		while(pHdl->nExit == 0)
		{
			if ((ret = gsf_select_simple(pHdl->nSock, 1000)) < 0)
			{
				printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
				sleep(1);
			}
			else if (ret == 0)
			{
				printf("[%s %d] waiting for response\n", __func__, __LINE__);
				continue;
			}
			if (0 >= (ret = gsf_local_tcp_recv(pHdl->nSock, (unsigned char *)&stRspMsg, sizeof(stRspMsg))))
			//if (0 >= (ret = gsf_tcp_recv(nSock, (unsigned char *)&stRspMsg, sizeof(stRspMsg))))
			{
				printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(stRspMsg), ret, errno, strerror(errno));
				ret = DEVSDK_ERR_CONN;
				break;
			}
			
			printf("[%s %d] recv: magic(%0#x) seq(%d) cmd(%d) dir(%d) type(%d) ctrl(%d) err(%d) path(%s)\n", __func__, __LINE__, 
				stRspMsg.magic, stRspMsg.seq, stRspMsg.cmd, stRspMsg.dir, 
				stRspMsg.type, stRspMsg.param.video.ctrl, stRspMsg.err, stRspMsg.param.path);
			ret = DEVSDK_OK;
			break;
		}
	}while(0);

	if (pHdl && pHdl->fCallback)
		pHdl->fCallback(ret, stRspMsg.param.path, pHdl->pUser);
	
	if (pHdl)
	{
		if (pHdl->nSock >= 0)
		{
			close(pHdl->nSock);
		}
		if (pHdl->pBuf && pHdl->nBufSize > 0)
		{
			free(pHdl->pBuf);
			pHdl->nBufSize = 0;
		}
//		if (pHdl->fCallback)
//			pHdl->fCallback(DEVSDK_ERR_CONN, NULL, NULL, NULL);
		free(pHdl);

/*		while(pHdl->nExit == 0) // wait for video file idle and delete
		{
			if (0 == DEVSDK_IsFileLocked(stRspMsg.param.path))
			{
				printf("[%s %d] file(%s) idle, del and exit!\n", __func__, __LINE__, stRspMsg.param.path);
				break;
			}
		}
		gsf_ipc_cmd_cli_release_file(-1, stRspMsg.param.path);
*/
	}
	
	pthread_detach(pthread_self());
	
	return NULL;
}

#ifdef GSF_IPC_CMD_SUPP_ACK
int ipc_cmd_send_ack_msg(int nSock, DEVSDK_FRAME_HEAD_S *pFrameHead)
{
	int ret = 0;
	GSF_IPC_CMD_ACK_MSG_S stAckMsg;
	memset(&stAckMsg, 0, sizeof(stAckMsg));
	stAckMsg.u32Magic = 0x20201215;
	stAckMsg.u32FrameNo = pFrameHead->u32FrameNo;
	stAckMsg.u32FrameSize = pFrameHead->u32FrameSize;
	if (sizeof(stAckMsg) != (ret = gsf_local_tcp_send(nSock, (unsigned char *)&stAckMsg, sizeof(stAckMsg))))
	{
		printf("[%s %d] send ack msg frameNo(%u) frameSize(%u) err %d(%s)!!\n", __func__, __LINE__, 
			stAckMsg.u32FrameNo, stAckMsg.u32FrameSize, errno, strerror(errno));
		return -1;
	}
	//printf("[%s %d] send ack msg frameNo(%u) frameSize(%u) ok\n", __func__, __LINE__, 
	//	stAckMsg.u32FrameNo, stAckMsg.u32FrameSize);
	return 0;
}
#endif
//实时音视频
void *gsf_ipc_cmd_cli_RealAV_task(void *h)
{
	// send request & recv response
	int len = 0;
	int ret = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	GSF_IPC_CMD_REAL_AV_HDL *pHdl = (GSF_IPC_CMD_REAL_AV_HDL *)h;
	do
	{
		if (pHdl == NULL)
			break;
		
		int nWaitForData = 0;
		int nRetry = 0;
		int nRcvFrameCnt = 0;
		DEVSDK_FRAME_HEAD_S *pFrameHead = NULL;
		// create local socket
		pHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
		//int nSock = ipc_cmd_connect_svr(GSF_IPC_CMD_SVR, GSF_IPC_CMD_PORT);
		if (pHdl->nSock < 0)
		{
			printf("[%s %d] connect local err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
			//printf("[%s %d] connect server(%s:%d) err %d(%s)\n", __func__, __LINE__, GSF_IPC_CMD_SVR, GSF_IPC_CMD_PORT, errno, strerror(errno));
			break;
		}
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_REAL_AV_DATA;
		
		stReqMsg.param.video.ctrl = GSF_IPC_CMD_VIDEO_CTRL_START;
		stReqMsg.param.video.mask = 0x02;// 0x01-DataSize 0x02- time 
		stReqMsg.param.video.stream = pHdl->nStream;  // 0:main stream, 1:sub stream
		stReqMsg.param.video.audio = 1;   // audio
		stReqMsg.param.video.speed = 2;
		stReqMsg.param.video.limit_size = (128<<20); // 128MB
		stReqMsg.param.video.start_time = pHdl->nStartTime; // 指定预录时长
		stReqMsg.param.video.end_time   = pHdl->nStopTime;  // 指定后录时长
		
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_DATA;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		
		if (ipc_cmd_req(pHdl->nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			break;
		}
		pHdl->nBufSize = 1<<20;
		pHdl->pBuf = calloc(1, pHdl->nBufSize);
		if (pHdl->pBuf == NULL)
		{
			printf("[%s %d] calloc(1, %d) err %d(%s)\n", __func__, __LINE__, pHdl->nBufSize, errno, strerror(errno));
			pHdl->nBufSize = 0;
			break;
		}
		while(pHdl->nExit == 0)
		{
			if ((ret = gsf_select_simple(pHdl->nSock, 1000)) < 0)
			{
				printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
				sleep(1);
				continue;
			}
			else if (ret == 0)
			{
				printf("[%s %d] waiting for data(%d)\n", __func__, __LINE__, nWaitForData);
				continue;
			}
			nWaitForData = 0;
			if (sizeof(DEVSDK_FRAME_HEAD_S) != (ret = gsf_local_tcp_recv(pHdl->nSock, pHdl->pBuf, sizeof(DEVSDK_FRAME_HEAD_S))))
			//if (0 >= (ret = gsf_tcp_recv(nSock, pHdl->pBuf, sizeof(DEVSDK_FRAME_HEAD_S))))
			{
				printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(DEVSDK_FRAME_HEAD_S), ret, errno, strerror(errno));
				break;
			}
			pFrameHead = (DEVSDK_FRAME_HEAD_S *)pHdl->pBuf;
			// 需要接收的帧大小为0时，表示数据已经传输完成，退出接收数据循环
			if (pFrameHead->u32FrameSize == 0)
			{
				printf("[%s %d] recv raw data cnt(%d) finish!!\n", __func__, __LINE__, nRcvFrameCnt);
				break;
			}
			// 实际应用时需要考虑缓冲不够时，重新申请足够大的缓冲
			if (pFrameHead->u16FrameFlag != 0x3448)
			{
				printf("[%s %d] recv err!! framesize(%d) flag(%0#hhx) err!! retry(%d) ...\n", __func__, __LINE__, 
					pFrameHead->u32FrameSize, pFrameHead->u16FrameFlag, nRetry);
				if (++nRetry < GSF_IPC_CMD_MAX_RETRY)
				{
					gsf_clear_recv_buf(pHdl->nSock, pHdl->nBufSize);
					continue;
				}
				break;
			}
			// 确保接收缓冲足够大
			if (pFrameHead->u32FrameSize > pHdl->nBufSize-sizeof(DEVSDK_FRAME_HEAD_S))
			{
				DEVSDK_FRAME_HEAD_S stTempHead = {0};
				// step 1: save FRAME HEAD
				memcpy(&stTempHead, pFrameHead, sizeof(DEVSDK_FRAME_HEAD_S));
				// step 2: realloc buffer
				pHdl->pBuf = (unsigned char *)gsf_realloc(pHdl->pBuf, pFrameHead->u32FrameSize+sizeof(DEVSDK_FRAME_HEAD_S), &pHdl->nBufSize);
			}
			if (pFrameHead->u32FrameSize > pHdl->nBufSize-sizeof(DEVSDK_FRAME_HEAD_S))
			{
				printf("realloc buffer size(%d -> %d) err %d(%s)\n", 
					pHdl->nBufSize, pFrameHead->u32FrameSize+sizeof(DEVSDK_FRAME_HEAD_S), errno, strerror(errno));
				break;
			}
			
			if (pFrameHead->u32FrameSize != (ret = gsf_local_tcp_recv(pHdl->nSock, pHdl->pBuf+sizeof(DEVSDK_FRAME_HEAD_S), pFrameHead->u32FrameSize)))
			//if (0 >= (ret = gsf_tcp_recv(nSock, pBuf+sizeof(DEVSDK_FRAME_HEAD_S), pFrameHead->u32FrameSize)))
			{
				printf("[%s %d] tcp recv expected len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, pFrameHead->u32FrameSize, ret, errno, strerror(errno));
				break;
			}
			nRetry = 0;
			nRcvFrameCnt++;
			// send ack msg
		#ifdef GSF_IPC_CMD_SUPP_ACK
			ipc_cmd_send_ack_msg(pHdl->nSock, pFrameHead);
		#endif
			if (pHdl->fCallback)
				pHdl->fCallback(DEVSDK_OK, pFrameHead, pHdl->pBuf+sizeof(DEVSDK_FRAME_HEAD_S), pHdl->pUser);
		}
	}while(0);
	
	if (pHdl)
	{
		if (pHdl->nSock >= 0)
		{
			close(pHdl->nSock);
		}
		if (pHdl->pBuf && pHdl->nBufSize > 0)
		{
			free(pHdl->pBuf);
			pHdl->nBufSize = 0;
		}
		if (pHdl->fCallback)
			pHdl->fCallback(DEVSDK_ERR_CONN, NULL, NULL, NULL);
	}
	return 0;
}

//实时音视频编码信息
void *gsf_ipc_cmd_cli_RealAV_enc_info_task(void *h)
{
	// send request & recv response
	int len = 0;
	int ret = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	GSF_IPC_CMD_REALAV_ENC_INFO_HDL *pHdl = (GSF_IPC_CMD_REALAV_ENC_INFO_HDL *)h;
	do
	{
		if (pHdl == NULL)
			break;
		
		int nWaitForData = 0;
		int nRetry = 0;
		DEVSDK_AV_ENC_INFO_S stAvEncInfo = {0};
		// create local socket
		pHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
		//int nSock = ipc_cmd_connect_svr(GSF_IPC_CMD_SVR, GSF_IPC_CMD_PORT);
		if (pHdl->nSock < 0)
		{
			printf("[%s %d] connect local err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
			//printf("[%s %d] connect server(%s:%d) err %d(%s)\n", __func__, __LINE__, GSF_IPC_CMD_SVR, GSF_IPC_CMD_PORT, errno, strerror(errno));
			break;
		}
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_REALAV_ENC_INFO;

		stReqMsg.param.enc.ctrl = GSF_IPC_CMD_CTRL_START;
		stReqMsg.param.enc.stream = pHdl->nStream;
		
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_DATA;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		
		if (ipc_cmd_req(pHdl->nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			break;
		}
		while(pHdl->nExit == 0)
		{
			if ((ret = gsf_select_simple(pHdl->nSock, 1000)) < 0)
			{
				printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
				//usleep(200000);
				sleep(1);
				continue;
			}
			else if (ret == 0)
			{
				printf("[%s %d] waiting for av encode info(%d)\n", __func__, __LINE__, nWaitForData++);
				continue;
			}
			nWaitForData = 0;
			if (sizeof(DEVSDK_AV_ENC_INFO_S) != (ret = gsf_local_tcp_recv(pHdl->nSock, (unsigned char *)&stAvEncInfo, sizeof(DEVSDK_AV_ENC_INFO_S))))
			//if (0 >= (ret = gsf_tcp_recv(nSock, pHdl->pBuf, sizeof(DEVSDK_FRAME_HEAD_S))))
			{
				printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(DEVSDK_AV_ENC_INFO_S), ret, errno, strerror(errno));
				break;
			}
			if (stAvEncInfo.u16FrameFlag != 0x3448)
			{
				printf("[%s %d] recv err!! frameno(%d) flag(%0#hhx) err!! retry(%d) ...\n", __func__, __LINE__, 
					stAvEncInfo.u32FrameNo, stAvEncInfo.u16FrameFlag, nRetry);
				if (++nRetry < GSF_IPC_CMD_MAX_RETRY)
				{
					continue;
				}
				break;
			}
			if (pHdl->fCallback)
				pHdl->fCallback(DEVSDK_OK, &stAvEncInfo, pHdl->pUser);
		}
	}while(0);
	
	if (pHdl)
	{
		if (pHdl->nSock >= 0)
		{
			close(pHdl->nSock);
		}
		if (pHdl->fCallback)
			pHdl->fCallback(DEVSDK_ERR_CONN, NULL, NULL);
	}
	return 0;
}

//录像数据
void *gsf_ipc_cmd_cli_rec_task(void *h)
{
	// send request & recv response
	int len = 0;
	int ret = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	GSF_IPC_CMD_REC_HDL *pRecHdl = (GSF_IPC_CMD_REC_HDL *)h;
	do
	{
		if (pRecHdl == NULL)
			break;
		
		// create local socket
		pRecHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
		if (pRecHdl->nSock < 0)
		{
			printf("connect local server err %d(%s)\n", errno, strerror(errno));
			break;
		}
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pRecHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_REC_DATA;
		
		stReqMsg.param.rec.type = pRecHdl->nRecType;  // IO Alarm Record 
		stReqMsg.param.rec.ctrl = GSF_IPC_CMD_VIDEO_CTRL_START;
		stReqMsg.param.rec.audio = 0;  // need audio
		stReqMsg.param.rec.speed = 2;
		stReqMsg.param.rec.start_time = pRecHdl->nStartTime;
		stReqMsg.param.rec.end_time   = pRecHdl->nStopTime;
		
		//printf("[%s %d] rec type(%0#x) ctrl(%d) speed(%d) time(%u %u)\n", __func__, __LINE__, 
		//	stReqMsg.param.rec.type, stReqMsg.param.rec.ctrl, stReqMsg.param.rec.speed, 
		//	stReqMsg.param.rec.start_time, stReqMsg.param.rec.end_time);
		
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_DATA;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		
		if (ipc_cmd_req(pRecHdl->nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			break;
		}
		if (1)
		{
			int nWaitForData = 0;
			int nRetry = 0;
			int nRcvFrameCnt = 0;
			DEVSDK_FRAME_HEAD_S *pFrameHead = NULL;
			pRecHdl->nBufSize = 512<<10;
			pRecHdl->pBuf = calloc(1, pRecHdl->nBufSize);
			if (pRecHdl->pBuf == NULL)
			{
				printf("[%s %d] calloc(1, %d) err %d(%s)\n", __func__, __LINE__, pRecHdl->nBufSize, errno, strerror(errno));
				pRecHdl->nBufSize = 0;
				break;
			}
			while(pRecHdl->nExit == 0)
			{
				if ((ret = gsf_select_simple(pRecHdl->nSock, 1000)) < 0)
				{
					printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
					sleep(1);
					continue;
				}
				else if (ret == 0)
				{
					printf("[%s %d] waiting for data(%d)\n", __func__, __LINE__, nWaitForData);
					continue;
				}
				nWaitForData = 0;
				if (1)
				{
					if (sizeof(DEVSDK_FRAME_HEAD_S) != (ret = gsf_local_tcp_recv(pRecHdl->nSock, pRecHdl->pBuf, sizeof(DEVSDK_FRAME_HEAD_S))))
					//if (0 >= (ret = gsf_tcp_recv(pRecHdl->nSock, pRecHdl->pBuf, sizeof(DEVSDK_FRAME_HEAD_S))))
					{
						printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(DEVSDK_FRAME_HEAD_S), ret, errno, strerror(errno));
						break;
					}
					pFrameHead = (DEVSDK_FRAME_HEAD_S *)pRecHdl->pBuf;
					// 需要接收的帧大小为0时，表示数据已经传输完成，退出接收数据循环
					if (pFrameHead->u32FrameSize == 0)
					{
						printf("recv rec data reach end!! cnt(%d)\n", __func__, __LINE__, nRcvFrameCnt);
						if (pRecHdl->fCallback)
							pRecHdl->fCallback(DEVSDK_OK, NULL, NULL, NULL);
						break;
					}
					if (pFrameHead->u16FrameFlag != 0x3448)
					{
						//printf("[%s %d] recv err!! framesize(%d) flag(%0#hx) err!! retry(%d) ...\n", __func__, __LINE__, 
						//	pFrameHead->u32FrameSize, pFrameHead->u16FrameFlag, nRetry);
						if (++nRetry < GSF_IPC_CMD_MAX_RETRY)
						{
							gsf_clear_recv_buf(pRecHdl->nSock, pRecHdl->nBufSize);
							continue;
						}
						break;
					}
					// 确保接收缓冲足够大
					if (pFrameHead->u32FrameSize > pRecHdl->nBufSize-sizeof(DEVSDK_FRAME_HEAD_S))
					{
						DEVSDK_FRAME_HEAD_S stTempHead = {0};
						// step 1: save FRAME HEAD
						memcpy(&stTempHead, pFrameHead, sizeof(DEVSDK_FRAME_HEAD_S));
						// step 2: realloc buffer
						pRecHdl->pBuf = (unsigned char *)gsf_realloc(pRecHdl->pBuf, pFrameHead->u32FrameSize+sizeof(DEVSDK_FRAME_HEAD_S), &pRecHdl->nBufSize);
					}
					if (pFrameHead->u32FrameSize > pRecHdl->nBufSize-sizeof(DEVSDK_FRAME_HEAD_S))
					{
						printf("realloc buffer size(%d -> %d) err %d(%s)\n", 
							pRecHdl->nBufSize, pFrameHead->u32FrameSize+sizeof(DEVSDK_FRAME_HEAD_S), errno, strerror(errno));
						break;
					}
					if (pFrameHead->u32FrameSize != (ret = gsf_local_tcp_recv(pRecHdl->nSock, pRecHdl->pBuf+sizeof(DEVSDK_FRAME_HEAD_S), pFrameHead->u32FrameSize)))
					//if (0 >= (ret = gsf_tcp_recv(pRecHdl->nSock, pRecHdl->pBuf+sizeof(DEVSDK_FRAME_HEAD_S), pFrameHead->u32FrameSize)))
					{
						printf("[%s %d] tcp recv expected len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, pFrameHead->u32FrameSize, ret, errno, strerror(errno));
						break;
					}
					nRetry = 0;
					nRcvFrameCnt++;
					/*if (DEVSDK_IS_I_FRAME(pFrameHead->u8FrameType))
					{
						printf("[%s %d] frame: cnt(%d) no(%u) type(%d) size(%u) time(%u) res(%d %d) fr(%d) enc(%d) \n", 
							__func__, __LINE__, nRcvFrameCnt, 
							pFrameHead->u32FrameNo, pFrameHead->u8FrameType, pFrameHead->u32FrameSize, pFrameHead->u32Time, 
							pFrameHead->unVideoInfo.u16ImageWidth, pFrameHead->unVideoInfo.u16ImageHeight, 
							pFrameHead->unVideoInfo.u8FrameRate, pFrameHead->unVideoInfo.u8EncodeType);
					}*/
					// send ack msg
				#ifdef GSF_IPC_CMD_SUPP_ACK
					ipc_cmd_send_ack_msg(pRecHdl->nSock, pFrameHead);
				#endif
					if (pRecHdl->fCallback)
						pRecHdl->fCallback(DEVSDK_OK, pFrameHead, pRecHdl->pBuf+sizeof(DEVSDK_FRAME_HEAD_S), pRecHdl->pUser);
				}
			}
		}
		else
		{
			printf("[%s %d] video type(%d) unsupport!!\n", __func__, __LINE__, stRspMsg.type);
		}
	}while(0);
	
	if (pRecHdl)
	{
		if (pRecHdl->nSock >= 0)
		{
			close(pRecHdl->nSock);
		}
		if (pRecHdl->pBuf && pRecHdl->nBufSize > 0)
		{
			free(pRecHdl->pBuf);
			pRecHdl->nBufSize = 0;
		}
		if (pRecHdl->fCallback)
			pRecHdl->fCallback(DEVSDK_ERR_CONN, NULL, NULL, NULL);
	}
	return NULL;
}

//原始sensor数据
void *gsf_ipc_cmd_cli_RawSensor_task(void *h)
{
	// send request & recv response
	int len = 0;
	int ret = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	GSF_IPC_CMD_RAW_SENSOR_HDL *pHdl = (GSF_IPC_CMD_RAW_SENSOR_HDL *)h;
	do
	{
		if (pHdl == NULL)
			break;
		
		int nWaitForData = 0;
		int nRetry = 0;
		int nRcvFrameCnt = 0;
		int nRecvSize = 160*1024;
		int nRecvLen = 0;
		int nMsgSize= 0;
		
		int nOffsetSize = 0;
		unsigned char *pHeadBuf = calloc(1, 128);
		DEVSDK_FRAME_HEAD_S *pFrameHead = NULL;
		DEVSDK_AV_PIPE_FRAME_INFO *pViPipeFrame = NULL;
		// create local socket
		pHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);

		if (pHdl->nSock < 0)
		{
			printf("[%s %d] connect local err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
			break;
		}
		usleep(10000);
		// send request
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_RAW_SENSOR_DATA;

		stReqMsg.param.vi.ctrl = GSF_IPC_CMD_VIDEO_CTRL_START;
		stReqMsg.param.vi.nViPipe = 0;							//PipeID
		stReqMsg.param.vi.nByteAlign = 1;						//是否字节对齐
		stReqMsg.param.vi.nBitWidth = pHdl->nBitWidth;
		stReqMsg.param.vi.nCompressMode = pHdl->nCompressMode;
		
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_DATA;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;
		
		if (ipc_cmd_req(pHdl->nSock, &stReqMsg, &stRspMsg) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			break;
		}
		pHdl->nBufSize = 5<<20;		//帧数据大小为 sensor输出分辨率 宽*高*2
		pHdl->pBuf = calloc(1, pHdl->nBufSize);
		if (pHdl->pBuf == NULL)
		{
			printf("[%s %d] calloc(1, %d) err %d(%s)\n", __func__, __LINE__, pHdl->nBufSize, errno, strerror(errno));
			pHdl->nBufSize = 0;
			break;
		}
		while(pHdl->nExit == 0)
		{
			nRecvLen = 0;
			nOffsetSize = 0;
			nRecvSize = 160*1024;
			
			if ((ret = gsf_select_simple(pHdl->nSock, 1000)) < 0)
			{
				printf("[%s %d] select err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
				sleep(1);
				continue;
			}
			else if (ret == 0)
			{
				printf("[%s %d] nSock(%d) waiting for data(%d)\n", __func__, __LINE__, pHdl->nSock, nWaitForData);
				continue;
			}
			nWaitForData = 0;
			if (sizeof(DEVSDK_AV_PIPE_FRAME_INFO) != (ret = gsf_local_tcp_recv(pHdl->nSock, pHeadBuf, sizeof(DEVSDK_AV_PIPE_FRAME_INFO))))
			{
				printf("[%s %d] tcp recv len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, sizeof(DEVSDK_AV_PIPE_FRAME_INFO), ret, errno, strerror(errno));
				break;
			}
			pViPipeFrame = (DEVSDK_AV_PIPE_FRAME_INFO *)pHeadBuf;

			// 需要接收的帧大小为0时，表示数据已经传输完成，退出接收数据循环
			if (pViPipeFrame->u32FrameSize == 0)
			{
				printf("[%s %d] recv raw data cnt(%d) finish!!\n", __func__, __LINE__, nRcvFrameCnt);
				break;
			}

			// 确保接收缓冲足够大
			if (pViPipeFrame->u32FrameSize > pHdl->nBufSize)
			{
				DEVSDK_AV_PIPE_FRAME_INFO stTempHead = {0};
				// step 1: save FRAME HEAD
				memcpy(&stTempHead, pViPipeFrame, sizeof(DEVSDK_AV_PIPE_FRAME_INFO));
				// step 2: realloc buffer
				pHdl->pBuf = (unsigned char *)gsf_realloc(pHdl->pBuf, pViPipeFrame->u32FrameSize, &pHdl->nBufSize);
			}

			while( pViPipeFrame->u32FrameSize > nRecvLen)
			{
				if((pViPipeFrame->u32FrameSize - nRecvLen) <= nRecvSize)
					nRecvSize = pViPipeFrame->u32FrameSize - nRecvLen;
				
				ret = gsf_local_tcp_recv(pHdl->nSock, pHdl->pBuf+nOffsetSize, nRecvSize);
				if (ret <= 0)
				{
					printf("[%s %d] tcp recv expected len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, pViPipeFrame->u32FrameSize, nRecvSize, errno, strerror(errno));
					break;
				}
				nRecvLen += ret;
				nOffsetSize += ret;
			}

			nRetry = 0;
			nRcvFrameCnt++;

			if (pHdl->fCallback)
				pHdl->fCallback(DEVSDK_OK, pViPipeFrame, pHdl->pBuf, pHdl->pUser);
		}
	}while(0);
	
	if (pHdl)
	{
		if (pHdl->nSock >= 0)
		{
			close(pHdl->nSock);
		}
		if (pHdl->pBuf && pHdl->nBufSize > 0)
		{
			free(pHdl->pBuf);
			pHdl->nBufSize = 0;
		}
		if (pHdl->fCallback)
			pHdl->fCallback(DEVSDK_ERR_CONN, NULL, NULL, NULL);
	}
	return 0;
}

int gsf_ipc_cmd_cli_stop(void *hdl, int type)
{
	if (hdl == NULL)
		return 0;
	
	switch(type)
	{
		//case GSF_IPC_CMD_MP4_VIDEO:
		case GSF_IPC_CMD_REAL_AV_DATA:
		case GSF_IPC_CMD_REC_DATA:
		{
			GSF_IPC_CMD_HDL *pHdl = (GSF_IPC_CMD_HDL *)hdl;
			pHdl->nExit = 1;
			pthread_join(pHdl->thid, NULL);
		}
			break;
		case GSF_IPC_CMD_RAW_SENSOR_DATA:
		{
			GSF_IPC_CMD_RAW_SENSOR_HDL *pHdl = (GSF_IPC_CMD_RAW_SENSOR_HDL *)hdl;
			pHdl->nExit = 1;
			pthread_join(pHdl->thid, NULL);
		}
			break;
		case GSF_IPC_CMD_MP4_VIDEO_V2:
		{			
			GSF_IPC_CMD_VIDEO_HDL_V2 *pHdl = (GSF_IPC_CMD_VIDEO_HDL_V2 *)hdl;
			pHdl->nExit = 1;
			
			printf("[%s %d] come to stop task\n",__func__,__LINE__);
			pthread_join(pHdl->thid, NULL);
		}
			break;
		default:
			break;
	}
	return 0;
}

#define GSF_IPC_CMD_PD_SERV_PATH "/tmp/ipccmdPD"

void *gsf_ipc_cmd_get_pd_rect_info(void *h)
{
	int ret = 0;
	char szData[1024] = {0};
	GSF_IVP_PD_RECT_CFG *pIvpPDRectCfg = NULL;
	GSF_IPC_CMD_PD_RECT_HDL *pHdl = (GSF_IPC_CMD_PD_RECT_HDL *)h;
	
	if (0 > (pHdl->nSock = gsf_create_local_udp_server(GSF_IPC_CMD_PD_SERV_PATH)))
	{
		printf( "gsf_create_local_tcp_server(%s) err %d(%s)\n", GSF_IPC_CMD_PD_SERV_PATH, errno, strerror(errno));
		return NULL;
	}

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
			printf("[%s %d] select timeout\n", __func__, __LINE__);
			continue;
		}
		memset(&szData, 0, sizeof(szData));
		if ((ret = gsf_local_udp_recv(pHdl->nSock, (unsigned char *)&szData, sizeof(GSF_IVP_PD_RECT_CFG))) <= 0)
		{
			printf("[%s %d] local udp recv ret(%d) err %d(%s)\n", __func__, __LINE__, ret, errno, strerror(errno));
			continue;
		}
		pIvpPDRectCfg = (GSF_IVP_PD_RECT_CFG *)szData;

		for(int i = 0; i < pIvpPDRectCfg->u8ClassNum; i++)
		{
			for(int j = 0; j < pIvpPDRectCfg->u8RectNum[i]; j++)
			{
				if(pIvpPDRectCfg->u8RectNum[i] <= 0)
					continue;
				printf("[%s %d] Class_id(%d) rect_id(%d) quality(%f) rect(%d %d %d %d)\n",__func__,__LINE__,
						i,j,
						pIvpPDRectCfg->rect[i][j].quality,
						pIvpPDRectCfg->rect[i][j].rect.x,
						pIvpPDRectCfg->rect[i][j].rect.y,
						pIvpPDRectCfg->rect[i][j].rect.width,
						pIvpPDRectCfg->rect[i][j].rect.height);
			}
		}
	}

	gsf_close_socket(&pHdl->nSock);
	
	return NULL;
}

int gsf_ipc_cmd_stop_get_pd_rect_info(PDRectHdl hdl)
{
	int ret = -1;
	GSF_IPC_CMD_PD_RECT_HDL *pHdl = (GSF_IPC_CMD_PD_RECT_HDL *)hdl;

	pHdl->nExit = 1;
	ret = gsf_close_socket(&pHdl->nSock);
	if(ret == 0 && (access(GSF_IPC_CMD_PD_SERV_PATH,F_OK) == 0))
	{
		unlink(GSF_IPC_CMD_PD_SERV_PATH);
	}
	pthread_join(pHdl->thid, NULL);
	
	return 0;
}

int gsf_ipc_cmd_cli_open_push_data(void *hdl)
{
	int ret = 0;
	GSF_IPC_CMD_PUSH_DATA_HDL *pHdl = (GSF_IPC_CMD_PUSH_DATA_HDL *)hdl;
	do
	{
		if (pHdl == NULL)
			break;

		if (pHdl->nSock > 0)
			break;
		
		// create local socket
		pHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
		if (pHdl->nSock < 0)
		{
			printf("connect local server err %d(%s)\n", errno, strerror(errno));
			ret = DEVSDK_ERR_CONN;
			break;
		}
		//usleep(1000);
	}while(0);
	return ret;
}

int gsf_ipc_cmd_cli_push_data(void *hdl, DEVSDK_AI_DATA_S *pData)
{
	int ret = 0;
	int len = 0;
	GSF_IPC_CMD_MSG_S stReqMsg;
	GSF_IPC_CMD_MSG_S stRspMsg;
	GSF_IPC_CMD_PUSH_DATA_HDL *pHdl = (GSF_IPC_CMD_PUSH_DATA_HDL *)hdl;
	do
	{
		if (pHdl == NULL)
		{
			ret = DEVSDK_ERR_PARAM;
			printf("param err: hdl == NULL\n");
			break;
		}
		if (pHdl->nSock < 0)
		{
			pHdl->nSock = gsf_connect_local_svr(GSF_IPC_CMD_SERV_PATH);
			if (pHdl->nSock < 0)
			{
				printf("connect local server err %d(%s)\n", errno, strerror(errno));
				ret = DEVSDK_ERR_CONN;
				break;
			}
			usleep(1000);
		}
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_PUSH_DATA;
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_DATA;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;

		stReqMsg.param.size = sizeof(*pData) + (pData->target_num*sizeof(DEVSDK_AI_TARGET_S));

		// send msg
		printf("[%s %d] send msg size(%d) ..\n", __func__, __LINE__, sizeof(stReqMsg));
		if ((ret = ipc_cmd_req(pHdl->nSock, &stReqMsg, &stRspMsg)) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d)!!\n", __func__, __LINE__, stRspMsg.err);
			if (ret == 0)
			{
				ret = (stRspMsg.err == GSF_IPC_CMD_ERR_NOTSUPP)? DEVSDK_ERR_NOTSUPP: DEVSDK_ERR_UNKNOWN;
			}
			break;
		}
		// send data config
		usleep(1000);
		len = sizeof(DEVSDK_AI_DATA_S);
		printf("[%s %d] send data conf size(%d) ..\n", __func__, __LINE__, len);
		if (len != (ret = gsf_local_tcp_send(pHdl->nSock, (unsigned char *)pData, len)))
		{
			printf("[%s %d] tcp send len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, len, ret, errno, strerror(errno));
			ret = DEVSDK_ERR_SEND;
			break;
		}
		// send data
		if (pData->target_num > 0)
		{
			len = pData->target_num*sizeof(DEVSDK_AI_TARGET_S);
			printf("[%s %d] send data conf size(%d) ..\n", __func__, __LINE__, len);
			//debug
			int i = 0;
			for (; i < pData->target_num; i++)
			{
				printf("i(%d) target(%f, (%f, %f), (%f, %f), %s)\n", 
					i, pData->target[i].conf, pData->target[i].p1.x, pData->target[i].p1.y, 
					pData->target[i].p2.x, pData->target[i].p2.y, pData->target[i].text);
			}
			//---
			if (len != (ret = gsf_local_tcp_send(pHdl->nSock, (unsigned char *)pData->target, len)))
			{
				printf("[%s %d] tcp send len(%d) but(%d) err %d(%s)\n", __func__, __LINE__, len, ret, errno, strerror(errno));
				ret = DEVSDK_ERR_SEND;
				break;
			}
		}
		ret = DEVSDK_OK;
		printf("[%s %d] send finish! total size(%d).\n", __func__, __LINE__, sizeof(stReqMsg)+stReqMsg.param.size);
	}while(0);
	if (ret != DEVSDK_OK)
	{
		gsf_close_socket(&pHdl->nSock);
	}
	return ret;
}

int gsf_ipc_cmd_cli_close_push_data(void *hdl)
{
	int ret = 0;
	GSF_IPC_CMD_PUSH_DATA_HDL *pHdl = (GSF_IPC_CMD_PUSH_DATA_HDL *)hdl;
	if (pHdl && pHdl->nSock > 0)
	{
		GSF_IPC_CMD_MSG_S stReqMsg;
		GSF_IPC_CMD_MSG_S stRspMsg;
		
		memset(&stReqMsg, 0, sizeof(stReqMsg));
		stReqMsg.magic = GSF_IPC_CMD_MAGIC;
		stReqMsg.seq = pHdl->nSeq++;
		stReqMsg.cmd = GSF_IPC_CMD_PUSH_DATA;
		stReqMsg.dir = GSF_IPC_CMD_DIR_REQ;
		stReqMsg.type = GSF_IPC_CMD_TYPE_DATA;
		stReqMsg.err = GSF_IPC_CMD_ERR_OK;

		stReqMsg.param.data.ctrl = GSF_IPC_CMD_VIDEO_CTRL_STOP;

		// send msg
		if ((ret = ipc_cmd_req(pHdl->nSock, &stReqMsg, &stRspMsg)) != 0 ||
			stRspMsg.err != GSF_IPC_CMD_ERR_OK)
		{
			printf("[%s %d] something err(%d) ret(%d) !\n", __func__, __LINE__, stRspMsg.err, ret);
			if (ret == 0)
			{
				//ret = (stRspMsg.err == GSF_IPC_CMD_ERR_NOTSUPP)? DEVSDK_ERR_NOTSUPP: DEVSDK_ERR_UNKNOWN;
			}
		}
		usleep(10000);
		ret = gsf_close_socket(&pHdl->nSock);
	}
	return ret;
}
