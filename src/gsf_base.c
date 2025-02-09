/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_base.c

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/01/16
  Description   :
  History       :
  1.Date        : 2021/01/16
    Author      :
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "gsf_base.h"

#define GSF_MAX_RETRY_TIME      3
#define GSF_MAX_BLOCK_RECV_TIME 3
#define GSF_MAX_BLOCK_SEND_TIME 3
#define GSF_REALLOC_ALIGN_LEN 4096

#define GSF_RETURN_FAIL (-1)
#define GSF_RETURN_OK   (0)

void *gsf_realloc(void *pOldAddr, int nNewSize, int *pNewBufSize)
{
	if (nNewSize <= 0 || pNewBufSize == NULL)
	{
		return pOldAddr;
	}

	int nOldBufSize = *pNewBufSize;
	int nReallocSize = (nNewSize + GSF_REALLOC_ALIGN_LEN - 1) & 0xFFFFF000;
	void *pBakBuff = pOldAddr;
	void *pNewAddr = realloc(pOldAddr, nReallocSize);
	if (pNewAddr)
	{
		printf("realloc buffer size(%d -> %d) new(%d) ok!!\n", nOldBufSize, nReallocSize, nNewSize);
		*pNewBufSize = nReallocSize;
	}
	else
	{
		printf("realloc buffer size(%d) err %d(%s), keep oldsize(%d)\n", 
			nReallocSize, nOldBufSize, errno, strerror(errno));
		pNewAddr = pBakBuff;
	}
	return pNewAddr;
}

/*
 * 
 */
int gsf_pthread_set_name(const char *fmt, ...)
{
    va_list ap;
    char name[128] = {0};
    int rtval = 0;
    
    va_start(ap, fmt);
    vsnprintf(name, sizeof(name), fmt, ap);
    va_end(ap);
    if ((rtval = prctl(PR_SET_NAME, name)) < 0)
    {
        printf("[%s %d] prctl(PR_SET_NAME)[%s] error: %d(%s)\n", __FILE__, __LINE__, name, errno, strerror(errno));
        return -1;
    }
    return 0;
}

/*
 * @stacksize: Byte
 */
int gsf_pthread_create(pthread_t *tid, int stacksize, pthread_task_fun func, void *arg)
{
	int nRet = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, (stacksize >= 0x8000 && stacksize <= 0x800000)? stacksize : 0x200000);
    if((nRet = pthread_create(tid, &attr, func, arg)) < 0)
    {
		printf("[%s %d] pthread_create err %d(%s)\n", __func__, __LINE__, errno, strerror(errno));
    }
	pthread_attr_destroy(&attr);
    return nRet;
}

int gsf_close_socket(int *pSock)
{
	if (pSock != NULL && *pSock > 0)
	{
//		shutdown(*pSock, 2); //ljm close 2010-07-15
		close(*pSock);
		*pSock = -1;
		return GSF_RETURN_OK;
	}
	return GSF_RETURN_FAIL;
}

int gsf_create_sock(GSF_SOCK_TYPE iType)
{
	int domain = AF_INET;
	
	if (iType == GSF_TCP_SOCK_V6 || iType == GSF_UDP_SOCK_V6)
	{
		domain = AF_INET6;
	}
	else if (iType == GSF_LOCAL_TCP_SOCK || iType == GSF_LOCAL_UDP_SOCK)
	{
		domain = AF_UNIX;
	}
	
	return socket(domain, (iType%2) ? (SOCK_STREAM):(SOCK_DGRAM), 0);
}

unsigned long gsf_ip_n2a(unsigned long ip, char *ourIp, int len)
{
	if(!ourIp)
		return GSF_RETURN_FAIL;
	
	memset(ourIp, 0, len);
	ip = htonl(ip);
	inet_ntop(AF_INET, &ip, ourIp, len);
	return GSF_RETURN_OK;
}

unsigned long gsf_ip_a2n(char *pstrIP)
{
	int err;
	unsigned long ret;
    struct sockaddr_in *sinp;
	struct addrinfo	gsfnts, *result, *aip;

	if (!pstrIP)
		return -1;

	if(1 == inet_pton(AF_INET, pstrIP, &ret))
		return htonl(ret);

	memset(&gsfnts, 0, sizeof(gsfnts));
	gsfnts.ai_family = AF_INET;

	if ((err = getaddrinfo(pstrIP, 0, &gsfnts, &result)) != 0)
	{
#ifdef GSF_SPEC_LITEOS_VER
		printf("getaddrinfo error: %s\n", strerror(errno));
#else
		printf("getaddrinfo error: %s\n", gai_strerror(err));
#endif
		return -1;
	}

	ret = 0xffffffff;
	for (aip = result; aip != NULL; aip = aip->ai_next)
	{
		if (aip->ai_family == AF_INET)
		{
			sinp = (struct sockaddr_in *)aip->ai_addr;
			ret = sinp->sin_addr.s_addr;
			break;
		}
	}
	
	freeaddrinfo(result);
	return htonl(ret);
}

int gsf_local_tcp_send(int fd, unsigned char *data, int len)
{
	int ret    = 0;
	int sndCnt = 0;
	int sndSum = 0;
	
	if (fd <= 0 || data == NULL || len == 0)
		return -1;

	while (len > 0 && sndCnt++ < GSF_MAX_RETRY_TIME)
	{
		if ((ret = write(fd, data, len)) < 0)
			return -1;
		len    -= ret;
		sndSum += ret;
	}
	return sndSum;
}

int gsf_local_tcp_recv(int fd, unsigned char *data, int readLen)
{
	int ret = 0;
	int tryCnt = 0;
	int leftByte = readLen;
	
	if (fd <= 0 || data == NULL || readLen == 0)
		return -1;
	
	while (leftByte > 0 && tryCnt++ < GSF_MAX_RETRY_TIME)
	{
		if ((ret = read(fd, data, leftByte)) < 0)
		{
			if (errno == EAGAIN || errno == EINTR)
			{
				usleep(10000);
				continue;
			}
			printf("[%s %d] read(%d, %p, %d) err %d(%s)\n", __func__, __LINE__, fd, data, leftByte, errno, strerror(errno));
			return -1;
		}
		leftByte -= ret;
	}
	return (readLen - leftByte);
}

int gsf_select_simple(int fd, int timeout_ms)
{
	fd_set rset;
	FD_ZERO(&rset); // 20201215201645
	struct timeval tv = {timeout_ms/1000, (timeout_ms % 1000)*1000};
	FD_SET(fd, &rset);
	//FD_SET(fd, &wset);
	//FD_SET(fd, &eset);
	return select(fd+1, &rset, NULL, NULL, &tv);
}

int gsf_select(int *fd_array, int fd_num, int fd_type, int time_out)
{
	int maxfd = 0;
	int index = 0;
	int ret   = 0;
	
	fd_set read_fd, *p_read_fd;
	fd_set write_fd, *p_write_fd;
	fd_set except_fd, *p_except_fd;
	struct timeval timeO, *p_time_out;

	if (fd_array == NULL || fd_num < 0)
		return GSF_RETURN_FAIL;
	p_time_out = NULL;
	if (time_out > 0)
	{
    	timeO.tv_sec = time_out / 1000;
    	timeO.tv_usec= (time_out % 1000)*1000;	
		p_time_out = &timeO;
	}

	p_read_fd = p_write_fd = p_except_fd = NULL;
	
	if (fd_type & 0x1)
	{
		p_read_fd = &read_fd;
		FD_ZERO(p_read_fd);
	}
	
	if (fd_type & 0x2)
	{
		p_write_fd = &write_fd;
		FD_ZERO(p_write_fd);
	}
	
	if (fd_type & 0x4)
	{
		p_except_fd = &except_fd;
		FD_ZERO(p_except_fd);
	}
	
	for (index = 0; index < fd_num; ++index)
	{
		if(fd_array[index] <= 0)
			continue;
		maxfd = maxfd > fd_array[index] ? maxfd : fd_array[index];

		if (p_read_fd)
			FD_SET(fd_array[index], p_read_fd);
		if (p_write_fd)
			FD_SET(fd_array[index], p_write_fd);
		if (p_except_fd)
			FD_SET(fd_array[index], p_except_fd);
	}
	if (maxfd <= 0)
		return GSF_RETURN_FAIL;
	
	maxfd += 1;

	while (1)
	{
		ret = select(maxfd, p_read_fd, p_write_fd, p_except_fd, p_time_out);
		if (ret < 0 && errno == EINTR)
		{
		    continue;
		}    
		else if (ret < 0)
			return GSF_RETURN_FAIL;
		else if (ret == 0)
			return GSF_RETURN_OK;
		else
		{ //rongp  ÃÂ¬Ã’Â»ÃŠÂ±Â¼Ã¤Ã–Â»Â´Â¦Ã€Ã­ÃÃ‹Ã’Â»Â¸Ã¶connect
			for (index = 0; index < fd_num; ++index)
			{
				if (fd_array[index] <= 0)  //socket error
					continue;
				
				if (p_read_fd)
				{
					if (FD_ISSET(fd_array[index], p_read_fd))
						return fd_array[index] | 0x10000;
				}
				else if (p_write_fd)
				{
					if (FD_ISSET(fd_array[index], p_write_fd))
						return fd_array[index] | 0x20000;
				}
				else if (p_except_fd)
				{
					if (FD_ISSET(fd_array[index], p_except_fd))
						return fd_array[index] | 0x40000;;
				}			
			}
			return GSF_RETURN_OK;
		}
	}	
	return GSF_RETURN_FAIL;	
}

int gsf_connect_local_svr(const char *server)
{
	//int nSndBufSize = 1<<20;
	int nSock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (nSock < 0)
	{
		printf("[%s %d] socket AF_UNIX SOCK_STREAM err %d(%s).\n", __func__, __LINE__, errno, strerror(errno));
		return -1;
	}
	//setsockopt(nSock, SOL_SOCKET, SO_SNDBUF, &nSndBufSize, sizeof(nSndBufSize));
	//setsockopt(nSock, SOL_SOCKET, SO_RCVBUF, &nSndBufSize, sizeof(nSndBufSize));
	// connect local server
	struct sockaddr_un server_address;
	socklen_t server_len = sizeof(server_address);
	server_address.sun_family = AF_UNIX;
	strcpy (server_address.sun_path, server);
	if (connect(nSock, (struct sockaddr *)&server_address, server_len) < 0)
	{
		printf("connect server(%s) err %d(%s)\n", server, errno, strerror(errno));
		close(nSock);
		return -2;
	}
	return nSock;
}

int gsf_connect_svr(const char *server, int port)
{
	int nSock = socket(AF_INET, SOCK_STREAM, 0);
	if (nSock < 0)
	{
		printf("[%s %d] socket AF_UNIX SOCK_STREAM err %d(%s).\n", __func__, __LINE__, errno, strerror(errno));
		return -1;
	}
	// connect local server
	struct sockaddr_in server_address;
	socklen_t server_len = sizeof(server_address);
	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(port);
	server_address.sin_addr.s_addr   = inet_addr(server);
	if (connect(nSock, (struct sockaddr *)&server_address, server_len) < 0)
	{
		printf("connect server(%s:%d) err %d(%s)\n", server, port, errno, strerror(errno));
		close(nSock);
		return -2;
	}
	return nSock;
}

int gsf_udp_recv(int sockfd, unsigned char *rcvBuf, int bufSize, int rcvSize, struct sockaddr *from, int *fromlen)
{
    int		ret;
    unsigned long	dwRecved = 0;
    unsigned long	nTryTime = 0;
    int             nSize    = rcvSize;

    if (rcvBuf == NULL || bufSize <= 0)
        return GSF_RETURN_FAIL;
    
    if (rcvSize <= 0)   //ä¸æŒ‡å®šæŽ¥æ”¶é•¿åº¦
        nSize = bufSize;             

    while(dwRecved < nSize)
    {
		ret = recvfrom(sockfd, rcvBuf + dwRecved, nSize - dwRecved, 
						0, from, (socklen_t *)fromlen);
		if(0 == ret)
			return GSF_RETURN_FAIL;
		else if(ret < 1)
		{
			if(ECONNRESET == errno)
				return GSF_RETURN_FAIL;
			else if(EWOULDBLOCK == errno  || errno == EINTR || errno == EAGAIN)
			{
				if(nTryTime++ < GSF_MAX_BLOCK_RECV_TIME)
				{				
					usleep(10000);
					continue;
				}
				else
					break;
			}
			
			return GSF_RETURN_FAIL;
		}
		nTryTime = 0;
		dwRecved += ret;

		if (rcvSize <= 0)
            break;
    }
    return dwRecved;
}

int gsf_udp_sendto(int hSock, unsigned char *pbuf, int size, struct sockaddr * distAddr, int nAddrLen)
{
    int  block  = 0;	
    int  alllen = size;
    int  sended = 0;

	if (hSock <= 0 || pbuf == NULL || size <= 0 || distAddr == NULL)
	{
		printf("[%s %d] invalid argment sock %d buf %p size %d distAddr %p\n", __func__, __FILE__, 
			hSock, pbuf, size, distAddr);
		return 0;
	}
	
    while(alllen > 0)
    {	
		sended = sendto(hSock, pbuf, alllen, 0, distAddr, nAddrLen);
		if(0 == sended)
		{
            return GSF_RETURN_FAIL;
		}
		else if(sended < 1)
		{
			if(block > GSF_MAX_BLOCK_SEND_TIME)
			{
				return GSF_RETURN_FAIL;
			}
			if(errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN)
			{			
				if(block++ < GSF_MAX_BLOCK_SEND_TIME)
				{
					usleep(1000);
                    continue;
				}
				else
				{
					break;
				}
			}
			return GSF_RETURN_FAIL;
		}
		else
		{		
			pbuf += sended;
			alllen -= sended;
		}
    }
    
    if(alllen > 0)
		return GSF_RETURN_FAIL;
    return size;
}
	 
int gsf_udp_send(int hSock, unsigned char *pbuf, int size, struct sockaddr * distAddr)
{
    int  block  = 0;	
    int  alllen = size;
    int  sended = 0;

	if (hSock <= 0 || pbuf == NULL || size <= 0 || distAddr == NULL)
	{
//		GSF_SYSLOG(GSF_LOG_ERR,"GSF_Udp_Send %d %d\n", hSock, size);
		return 0;
	}
	
    while(alllen > 0)
    {	
		sended = sendto(hSock, pbuf, alllen, 0, distAddr, sizeof(struct sockaddr));
		if(0 == sended)
		{
            return GSF_RETURN_FAIL;
		}
		else if(sended < 1)
		{
			if(block > GSF_MAX_BLOCK_SEND_TIME)
			{
				return GSF_RETURN_FAIL;
			}
			if(errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN)
			{			
				if(block++ < GSF_MAX_BLOCK_SEND_TIME)
				{
					usleep(1000);
                    continue;
				}
				else
				{
					break;
				}
			}
			return GSF_RETURN_FAIL;
		}
		else
		{		
			pbuf += sended;
			alllen -= sended;
		}
    }
    
    if(alllen > 0)
		return GSF_RETURN_FAIL;
    return size;
}

int gsf_create_local_udp_server(char *svrPath)
{
    int server_fd = 0;
    struct sockaddr_un server_address;      /*ÉùÃ÷Ò»¸öUNIXÓòÌ×½Ó×Ö½á¹¹*/
    int server_len= 0;

    if ((server_fd = gsf_create_sock(GSF_LOCAL_UDP_SOCK)) < 0)
        return GSF_RETURN_FAIL;
    
    memset(&server_address, 0, sizeof(struct sockaddr_un));

    server_address.sun_family = AF_UNIX;

    strncpy(server_address.sun_path, svrPath, sizeof(server_address.sun_path)-1);
	server_address.sun_path[sizeof(server_address.sun_path)-1] = '\0';

    server_len = strlen(server_address.sun_path) + sizeof(server_address.sun_family);

    unlink (server_address.sun_path);/*É¾³ýÔ­ÓÐserver_socket¶ÔÏó*/  

    if (bind(server_fd,(struct sockaddr*)&server_address, server_len) < 0)
    {
        gsf_close_socket(&server_fd);
        return GSF_RETURN_FAIL;
    }
    
    return server_fd;
}

int gsf_local_udp_send(int fd, char *svrPath, unsigned char *data, int nLen)
{
	struct sockaddr_un  server_address;

	memset(&server_address, 0, sizeof(server_address));
	
	server_address.sun_family = AF_UNIX;
	
	strncpy(server_address.sun_path, svrPath, sizeof(server_address.sun_path)-1);
	server_address.sun_path[sizeof(server_address.sun_path)-1] = '\0';
	
	if (nLen != sendto(fd, data, nLen, 0,(struct sockaddr *)&server_address, 
										 sizeof(struct sockaddr_un)))
	{
		return GSF_RETURN_FAIL;
	}
	
	return nLen;
}

int gsf_local_udp_recv(int fd, unsigned char *pData, int nLen)
{
	return recvfrom(fd, pData, nLen, 0, NULL, NULL);
}

GSF_U64 gsf_get_real_timestamp()
{
#if 1
	struct timeval tv = {0};
	if (gettimeofday(&tv, NULL) != 0)
	{
		printf("gettimeofday err %d(%s)\n", errno, strerror(errno));
		return 0;
	}
	return ((GSF_U64)tv.tv_sec*1000000+tv.tv_usec);
#else
	struct timespec tp = {0};
	if (clock_gettime(CLOCK_REALTIME, &tp) != 0)
	{
		gsf_face_log(LOG_WARNING, "clock_gettime CLOCK_REALTIME err %d(%s)\n", errno, strerror(errno));
		return 0;
	}
	return ((GSF_U64)tp.tv_sec * 1000000 + tp.tv_nsec/1000);
#endif
}

