/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_web_retrans.h

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/01/16
  Description   :
  History       :
  1.Date        : 2021/01/16
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __GSF_WEB_RETRANS_H__
#define __GSF_WEB_RETRANS_H__

#include "devsdk.h"
#include "gsf_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef struct _GSF_WEB_TRANS_MSG_S_
{
    GSF_U32      u32Magic;
    GSF_U8       u8Ch;       //通道
    GSF_U8       u8Cmd;      //
    GSF_U8       u8CmdType;  //
    GSF_U8       u8Rsp;      //响应消息
	DEVSDK_USER_S stUsr; // 操作使用的用户信�?
	GSF_U8       u8Res[4];   //
    GSF_U32      u32Size;    //data数据域的大小
    GSF_U8       data[0];    //数据域，用于访问数据
}GSF_WEB_TRANS_MSG_S;

int gsf_web_retrans_proc(DEVSDK_USER_S *pUsrInfo, int nCmdType, int nCmd, int nCh, char *data, int nLen);

int gsf_web_retrans_send(int fd, GSF_WEB_TRANS_MSG_S *pMsg, char *data, int nLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __GSF_NET_RETRANS_H__ */
