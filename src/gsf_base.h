/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_base.h

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/01/16
  Description   :
  History       :
  1.Date        : 2021/01/16
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __GSF_BASE_H__
#define __GSF_BASE_H__

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
#include <pthread.h>

#include "gsf_type.h"
#include "gsf_param.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef enum _GSF_SOCK_TYPE_
{
    GSF_TCP_SOCK         = 1,
    GSF_UDP_SOCK,
    GSF_LOCAL_TCP_SOCK,
    GSF_LOCAL_UDP_SOCK,
    GSF_TCP_SOCK_V6,
    GSF_UDP_SOCK_V6,
}GSF_SOCK_TYPE;

typedef enum _GSF_CHIP_TYPE_E_
{
    GSF_CHIP_3512    = 0x3512,
    GSF_CHIP_3515    = 0x3515,
    GSF_CHIP_3516    = 0x3516,
    GSF_CHIP_3518A   = 0x3518A,
    GSF_CHIP_3518C   = 0x3518C,
    GSF_CHIP_3518E	 = 0x3518E,
    GSF_CHIP_3518EV2 = 0x3518E2,
    GSF_CHIP_3516C   = 0x3516C,
    GSF_CHIP_3516CV2 = 0x3516C2,
    GSF_CHIP_3516CV3 = 0x3516C3,
    GSF_CHIP_3516A   = 0x3516A,
    GSF_CHIP_3516D   = 0x3516D,
    GSF_CHIP_3519    = 0x3519,
    GSF_CHIP_3516AV2= 0x3516A2,
    GSF_CHIP_3516E   = 0x3516E,
    GSF_CHIP_3516CV5= 0x3516C5,
    GSF_CHIP_3516DV3= 0x3516D3,
    GSF_CHIP_3516EV2= 0x3516E2,
    GSF_CHIP_3516EV3= 0x3516E3,
    GSF_CHIP_3518EV3= 0x3518E3,
    GSF_CHIP_3519A  = 0x3519A,
    GSF_CHIP_3516AV3= 0x3516A3,
    GSF_CHIP_7205V2=  0x72052,
    GSF_CHIP_7205V3=  0x72053,
    GSF_CHIP_7605V1=  0x76051,
    GSF_CHIP_SS928 = 0x9281,
    GSF_CHIP_3516DV5 = 0x3516D5,
    GSF_CHIP_3519DV5 = 0x3519D5,
}GSF_CHIP_TYPE_E;


typedef void *(*pthread_task_fun)(void *arg);
int gsf_pthread_create(pthread_t *tid, int stacksize, pthread_task_fun func, void *arg);
void *gsf_realloc(void *pOldAddr, int nNewSize, int *pNewBufSize);

unsigned long gsf_ip_a2n(char *pstrIP);
unsigned long gsf_ip_n2a(unsigned long ip, char *ourIp, int len);

int gsf_create_sock(GSF_SOCK_TYPE iType);
int gsf_close_socket(int *fd);

int gsf_create_local_udp_server(char *svrPath);
int gsf_local_udp_send(int fd, char *svrPath, unsigned char *data, int nLen);
int gsf_local_udp_recv(int fd, unsigned char *pData, int nLen);

int gsf_local_tcp_send(int fd, unsigned char *data, int len);
int gsf_local_tcp_recv(int fd, unsigned char *data, int readLen);
int gsf_select_simple(int fd, int timeout_ms);
int gsf_select(int *fd_array, int fd_num, int fd_type, int time_out);
int gsf_connect_local_svr(const char *server);
int gsf_connect_svr(const char *server, int port);

int gsf_udp_recv(int sockfd, unsigned char *rcvBuf, int bufSize, int rcvSize, struct sockaddr *from, int *fromlen);
int gsf_udp_sendto(int hSock, unsigned char *pbuf, int size, struct sockaddr * distAddr, int nAddrLen);
int gsf_udp_send(int hSock, unsigned char *pbuf, int size, struct sockaddr * distAddr);

int	gsf_get_system_chip_type();

int gsf_pthread_set_name(const char *fmt, ...);
GSF_U64 gsf_get_real_timestamp();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GSF_BASE_H__ */
