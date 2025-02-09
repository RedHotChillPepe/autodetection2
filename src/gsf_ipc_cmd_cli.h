#ifndef __GSF_IPC_CMD_CLI_H__
#define __GSF_IPC_CMD_CLI_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>
#include "devsdk.h"
#include "gsf_ipc_cmd.h"

typedef struct _GSF_IPC_CMD_HDL
{
	void *fCb;
	void *pUser;
	
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	int nTTL;
	pthread_t thid;
	int comm[4];
}GSF_IPC_CMD_HDL;

typedef struct _GSF_IPC_CMD_CAPTURE_HDL
{
	CaptureDataCallbackFunc fCallback;
	void *pUser;
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	int nTTL;
	pthread_t thid;
	// comm
	int nStream;
	unsigned int nStartTime;
	unsigned int nStopTime;
	unsigned int nChn;
}GSF_IPC_CMD_CAPTURE_HDL;


typedef struct _GSF_IPC_CMD_RAW_HDL
{
	RawDataCallbackFunc fCallback;
	void *pUser;
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	int nTTL;
	pthread_t thid;
	// comm
	int nStream;
	unsigned int nStartTime;
	unsigned int nStopTime;
	unsigned int nRes;
}GSF_IPC_CMD_RAW_HDL;

typedef struct _GSF_IPC_CMD_REAL_AV_HDL
{
	RealAVDataCallbackFunc fCallback;
	void *pUser;
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	int nTTL;
	pthread_t thid;
	// comm
	int nStream;
	unsigned int nStartTime;
	unsigned int nStopTime;
	unsigned int nRes;
}GSF_IPC_CMD_REAL_AV_HDL;


typedef struct _GSF_IPC_CMD_REC_HDL
{
	RecDataCallbackFunc fCallback;
	void *pUser;
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	int nTTL;
	pthread_t thid;
	// comm
	int nRecType;
	unsigned int nStartTime;
	unsigned int nStopTime;
	int nRes;
}GSF_IPC_CMD_REC_HDL;

typedef struct _GSF_IPC_CMD_RECFILE_HDL
{
	RecordFileCallbackFunc fCallback;
	void *pUser;
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	int nTTL;
	pthread_t thid;
	// comm
	int nRecType;
	unsigned int nStartTime;
	unsigned int nStopTime;
	int nRes;
}GSF_IPC_CMD_RECFILE_HDL;

typedef struct _GSF_IPC_CMD_VIDEO_HDL
{
	VideoFileCallbackFunc fCallback;
	void *pUser;
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	int nTTL;
	pthread_t thid;
	// comm
	int nStream;
	unsigned int nStartTime;
	unsigned int nStopTime;
	int nRes;
}GSF_IPC_CMD_VIDEO_HDL;

typedef struct _GSF_IPC_CMD_VIDEO_HDL_V2
{
	VideoFileCallbackFunc_V2 fCallback;
	void *pUser;
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	int nTTL;
	pthread_t thid;
	// comm
	int nStream;
	unsigned long long uStartTimeMs;
	unsigned long long uStopTimeMs;
	int nRes;
}GSF_IPC_CMD_VIDEO_HDL_V2;


typedef struct _GSF_IPC_CMD_REALAV_ENC_INFO_HDL
{
	RealAVEncInfoCallbackFunc fCallback;
	void *pUser;
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	int nTTL;
	pthread_t thid;
	// comm
	int nStream;
	unsigned int nRes[3];
}GSF_IPC_CMD_REALAV_ENC_INFO_HDL;

typedef struct _GSF_IPC_CMD_PD_RECT_HDL
{
	int nExit;
	int nSock;
	pthread_t thid;
}GSF_IPC_CMD_PD_RECT_HDL;

typedef struct _GSF_IPC_CMD_RAW_SENSOR_HDL
{
	RawSensorDataCallbackFunc fCallback;
	void *pUser;
	int nExit;
	int nSock;
	unsigned char *pBuf;
	int nBufSize;
	int nSeq;
	pthread_t thid;
	unsigned int nBitWidth;
	unsigned int nCompressMode;
}GSF_IPC_CMD_RAW_SENSOR_HDL;

// push data msg
typedef struct _GSF_IPC_CMD_PUSH_DATA_HDL
{
	int nSock;
	int nSeq;
}GSF_IPC_CMD_PUSH_DATA_HDL;

int ipc_cmd_cli_proc_pic(int nChn, const char *pszFileName);
void *gsf_ipc_cmd_cli_capture_task(void *h);
void *gsf_ipc_cmd_cli_raw_task(void *h);
void *gsf_ipc_cmd_cli_RealAV_task(void *h);
void *gsf_ipc_cmd_cli_RealAV_enc_info_task(void *h);
void *gsf_ipc_cmd_cli_rec_task(void *h);
void *gsf_ipc_cmd_cli_AVFile_task_v2(void *h);
void *gsf_ipc_cmd_cli_AVFile_task(void *h);
void *gsf_ipc_cmd_cli_RecFile_task(void *h);
void *gsf_ipc_cmd_get_pd_rect_info(void *h);
void *gsf_ipc_cmd_cli_RawSensor_task(void *h);
int ipc_cmd_cli_proc_pic_v2( int nChn, int nStream, const char *pszFileName);

int gsf_ipc_cmd_cli_stop(void *hdl, int type);

int gsf_ipc_cmd_cli_release_file(int nSock, const char *pszFileName);

int gsf_ipc_cmd_stop_get_pd_rect_info(PDRectHdl hdl);

//int gsf_ipc_cmd_cli_open_push_data(void *hdl);
int gsf_ipc_cmd_cli_push_data(void *hdl, DEVSDK_AI_DATA_S *pData);
int gsf_ipc_cmd_cli_close_push_data(void *hdl);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
