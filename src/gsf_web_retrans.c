/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_web_retrans.c

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/01/16
  Description   :
  History       :
  1.Date        : 2021/01/16
    Author      :
    Modification: Created file

******************************************************************************/
/*
 * ת��������Ϣ
 */

#include <netdb.h>
#include "gsf_type.h"
#include "devsdk.h"
#include "gsf_web_retrans.h"
#include "gsf_base.h"
#include "gsf_param.h"

#define GSF_WEB_SVR_IP                    "127.0.0.1"
#define GSF_WEB_SVR_PORT                  9999
#define GSF_WEB_BUF_SIZE                  16384
#define GSF_WEB_MAGIC                     0x20210809

/*****************************************************************************
 �� �� ��  : gsf_web_retrans_proc
 ��������  :ƽ̨��ȡ�����ò����Ĵ���ӿ�
 �������  : 
             int nCmdType  : ��ȡ������������
             int nCmd        : ����
             int nCh           :ͨ����
             char *data      :����
             int nLen         : �����ṹ��С
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
*****************************************************************************/
int gsf_web_retrans_proc(DEVSDK_USER_S *pUsrInfo, int nCmdType, int nCmd, int nCh, char *data, int nLen)
{
    int nFd = 0, nRet = 0;
    if ((nFd = gsf_create_sock(2)) <= 0) //GSF_UDP_SOCK
    {
        printf("create local udp socket failed %d\n", errno);
        return GSF_FAILURE;
    }
    GSF_WEB_TRANS_MSG_S msg = {0};
    msg.u32Magic = GSF_WEB_MAGIC;
    msg.u8Ch = nCh;
    msg.u8Cmd = nCmd;
    msg.u8CmdType = nCmdType;
	if (pUsrInfo != NULL)
	{
		msg.stUsr.u32IpAddr = pUsrInfo->u32IpAddr;
		strncpy(msg.stUsr.szUsrName, pUsrInfo->szUsrName, DEVSDK_NAME_LEN - 1);
		msg.stUsr.szUsrName[DEVSDK_NAME_LEN - 1] = '\0';
	}
	else
	{
		msg.stUsr.u32IpAddr = 0x7F000001;
		strcpy(msg.stUsr.szUsrName, "Devsdk");
	}
    msg.u32Size = nLen;
    nRet = gsf_web_retrans_send(nFd, &msg, data, nLen);
    gsf_close_socket(&nFd);

    return nRet;
}

/*****************************************************************************
 �� �� ��  : gsf_web_retrans_send
 ��������  : ���ͱ���udp��Ϣ
 �������  : int fd         
             GSF_TRANS_MSG_S  *pMsg  
             char *data     
             int nDataLen       
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2011��12��14��
    ��    ��   : ������
    �޸�����   : �����ɺ���

*****************************************************************************/
int gsf_web_retrans_send(int fd, GSF_WEB_TRANS_MSG_S *pMsg, char *data, int nDataLen)
{
    char szBuf[GSF_WEB_BUF_SIZE] = {0};
	int nRet = 0, nSndLen = nDataLen+sizeof(GSF_WEB_TRANS_MSG_S);

    if (fd < 0 || pMsg == NULL || data == NULL || nDataLen <= 0)
        return -1;
    if (nSndLen > sizeof(szBuf))
    {
		printf("Buffer Overflow: nSndLen %d > BuffSize %d\n", nSndLen, sizeof(szBuf));
		return -1;
    }
    memcpy(szBuf, pMsg, sizeof(GSF_WEB_TRANS_MSG_S));
    memcpy(szBuf + sizeof(GSF_WEB_TRANS_MSG_S), data, nDataLen);
    struct sockaddr_in svr_addr;

    memset(&svr_addr, 0, sizeof(struct sockaddr_in));
    svr_addr.sin_family      = AF_INET;
	svr_addr.sin_addr.s_addr = htonl(gsf_ip_a2n(GSF_WEB_SVR_IP));
	svr_addr.sin_port        = htons(GSF_WEB_SVR_PORT);
    if ((nRet = gsf_udp_send(fd, (unsigned char *)szBuf, nSndLen, (struct sockaddr *)&svr_addr)) != nSndLen)
    {
        printf("gsf_client send msg failed \n");
        return GSF_FAILURE;
    }
     //�����ȴ�server ��Ӧ
    nRet = gsf_select(&fd, 0x1, 0x1, 5000);
    if (nRet == (fd | 0x10000))
    {
        do {
            if (gsf_udp_recv(fd, (unsigned char *)pMsg, sizeof(GSF_WEB_TRANS_MSG_S), 
                    sizeof(GSF_WEB_TRANS_MSG_S), NULL, NULL) != sizeof(GSF_WEB_TRANS_MSG_S))
            {
                printf("gsf client recv msg err\n");
                return GSF_FAILURE;
            }
            if (pMsg->u32Size == 0 || (pMsg->u8CmdType != GSF_GET_PARAM_MSG && pMsg->u8CmdType != GSF_GET_SYS_INFO
				&& pMsg->u8CmdType != GSF_SYS_STATE_MSG && pMsg->u8CmdType != GSF_GET_PROTO_PARA))
            {
            	break;
            }

            //�������ڲ����ṹ��һ�µ��µ��׽ӿ����������
            if (gsf_udp_recv(fd, (unsigned char *)data, nDataLen, pMsg->u32Size, NULL, NULL) != pMsg->u32Size)
            {
                printf("gsf client recv data err\n");
                return GSF_FAILURE;
            }
        }while(0);
        return pMsg->u8Rsp;
    }
    else
        printf("gsf web client recv response failed %d cmd %hhu[%hhu]\n", nRet, pMsg->u8Cmd, pMsg->u8CmdType);

    return GSF_FAILURE;
}