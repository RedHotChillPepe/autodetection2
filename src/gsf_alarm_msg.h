/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_alarm_msg.h

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/08/29
  Description   :
  History       :
  1.Date        : 2021/08/29
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __GSF_ALARM_MSG_H__
#define __GSF_ALARM_MSG_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>
#include "devsdk.h"
#include "gsf_type.h"

#define GSF_ALARM_FILE_NAME_LEN 64

typedef struct 
{
	AlarmMsgCallbackFunc fCallback;
	void *pUser;
	int nExit;
	int nSock;
	pthread_t thid;	
	int nChn;
	unsigned int u32Mask;
}GSF_ALARM_MSG_HDL;

typedef struct 
{
	GSF_U8	u8Channel;	// alarm channel
	GSF_U8	u8Type;		// alarm type --> DEVSDK_ALARM_TYPE_E in devsdk.h
	GSF_U8	u8Action;	// alarm action: 0-begin     1-end
	GSF_U8  u8Jpeg;		// 0-no jpeg file, 1-jpeg file name after alarm data
	GSF_U8	u8Year;		// 年份，从1900开始，如2021年为121
	GSF_U8	u8Month;	// 月份，1~12
	GSF_U8	u8Date;		// 日期，0~31
	GSF_U8	u8Hour;		// 小时，0~23
	GSF_U8	u8Minute;	// 分钟，0~59
	GSF_U8	u8Second;	// 秒数，0~59
	GSF_U8	u8Weekday;	// 星期: 0-星期天, 1-星期一, ... 6-星期六	
	GSF_U8	u8MSec;		// 毫秒，0~99
}GSF_ALARM_DATA_S;

typedef struct 
{
    GSF_U32      u32Size;        // data size, not include head
    GSF_U16      u16Protocol;    // protocol, reserved
	GSF_U16      u16Res;
}GSF_ALARM_HEAD_S;

typedef struct 
{
	GSF_ALARM_HEAD_S stHead;
	GSF_ALARM_DATA_S stData;
	char szFileName[GSF_ALARM_FILE_NAME_LEN];
}GSF_ALARM_MSG_S;

void *gsf_alarm_msg_task(void *h);

int gsf_alarm_msg_stop(void *hdl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
