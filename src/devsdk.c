/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : devsdk.h

  Version       : v1.0.5
  Author        : szzhst developer group
  Created       : 2021/06/23
  Description   :
  History       :
  1.Date        : 2021/06/23
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
#include <pthread.h>
#include "gsf_web_retrans.h"
#include "devsdk.h"
#include "gsf_ipc_cmd.h"
#include "gsf_ipc_cmd_cli.h"
#include "gsf_vpss_yuv.h"
#include "gsf_param.h"
#include "gsf_alarm_msg.h"
#include "comm485.h"
#include "gsf_vpss_comm.h"
#include "gsf_hi_sys_info.h"
#include "gsf_venc_capture.h"
#include "gsf_ptz_msg.h"

#define DEVSDK_VERSION_HEX 0x010019
#define DEVSDK_VERSION_MAJOR(v) (((v)>>16)&0xff)
#define DEVSDK_VERSION_MINOR(v) (((v)>>8)&0xff)
#define DEVSDK_VERSION_REV(v)   ((v)&0xff)
//#define DEVSDK_VERSION_STR "V1.0.1 Build "__DATE__" "__TIME__

static int g_nErrno = 0;

int DEVSDK_Init()
{
#ifdef GSF_VPSS_YUV_SUPP
	return gsf_release_vb_before_start();
#endif
	return 0;
}

int DEVSDK_Uninit()
{
	return 0;
}

int DEVSDK_VB_Iint(int nSize)
{
#ifdef GSF_VPSS_YUV_SUPP
	return VB_Restore(nSize);
#endif
	return 0;
}

long vpss_supp_compile_tips()
{
	printf("ERROR: please re-compile and type 'export GSF_VPSS_YUV_SUPP=1'  before make \n");
	return 0;
}

char *DEVSDK_GetVersion()
{
	static char szVersion[64] = {0};
	if (szVersion[0] == '\0')
	{
		snprintf(szVersion, sizeof(szVersion), "V%d.%d.%d Build %s %s", 
			DEVSDK_VERSION_MAJOR(DEVSDK_VERSION_HEX), 
			DEVSDK_VERSION_MINOR(DEVSDK_VERSION_HEX),
			DEVSDK_VERSION_REV(DEVSDK_VERSION_HEX),
			__DATE__, __TIME__);
	}
	return szVersion;
}

void DEVSDK_SetLastError(int nError)
{
	g_nErrno = nError;
}

int DEVSDK_GetLastError()
{
	return g_nErrno;
}

char* DEVSDK_GetLastErrorMsg(int *pLastErrorNo)
{
	if (pLastErrorNo)
		*pLastErrorNo = g_nErrno;
	
	switch(g_nErrno)
	{
		case DEVSDK_OK:
			return "OK";
		case DEVSDK_ERR_NOINIT:
			return "Please call DEVSDK_Init first";
		case DEVSDK_ERR_INIT:
			return "Init err";
		case DEVSDK_ERR_CONN:
			return "Connection err";
		case DEVSDK_ERR_PARAM:
			return "Invalid argument";
		case DEVSDK_ERR_CMD:
			return "Invalid Command";
		case DEVSDK_ERR_ALLOC:
			return "Alloc buffer err";
		case DEVSDK_ERR_SCALE:
			return "Scale video resolution err";
		case DEVSDK_ERR_MMAP:
			return "Mmap function err";
		case DEVSDK_ERR_CONVERT:
			return "Convert from YUV to RGB err";
		case DEVSDK_ERR_FILE_NOEXIST:
			return "No such file or directory";
		case DEVSDK_ERR_THREAD:
			return "Create thread err";
		case DEVSDK_ERR_MSG:
			return "Sending or receiving a message failed";
		case DEVSDK_ERR_NOUNBIND:
			return "There are no detaching channels";
		case DEVSDK_ERR_CREATECHNFAIL:
			return "Failed to create a channel";
		case DEVSDK_ERR_JPEGPREPAREERR:
			return "jpeg preparation failure";
		case DEVSDK_ERR_GETORSETPARAMERR:
			return "Get or Set param err";
		case DEVSDK_ERR_485COMMPREPAREERR:
			return "Set the param of the 485 serial port is err";
		case DEVSDK_ERR_485READERR:
			return "Failed to read data through the 485 serial port";
		case DEVSDK_ERR_485WRITEERR:
			return "Failed to write data to the 485 serial port";
		case DEVSDK_ERR_VPSSSETGROPERR:
			return "Failed to set vpss grop param";
		case DEVSDK_ERR_NOTSUPP:
			return "Function not-support";
		case DEVSDK_ERR_SEND:
			return "Send error";
		case DEVSDK_ERR_RECV:
			return "Recv error";
		default:
			return "Unknown error!";
	}
	return "OK";
}

int DEVSDK_ParamConfig(int nChn, int nType, int nCmd, void *pData, int nSize)
{
	// check cmd type
	// check param cmd
	// check sysinfo cmd
	// check ctrl cmd
	// check protocol para cmd
	if ((nType < 0 || nType >= GSF_MAX_CMD_MSG) ||
		((nType >= GSF_GET_PARAM_MSG && nType <= GSF_SET_PARAM_MSG) && (nCmd < GSF_VER_PARAM || nCmd >= GSF_PARAM_MAX_CMD)) ||
		((nType >= GSF_GET_SYS_INFO  && nType <= GSF_SET_SYS_INFO) &&  (nCmd < GSF_TIME_INFO || nCmd >= GSF_MAX_SYS_CMD)) ||
		((nType >= GSF_SYS_STATE_MSG && nType <= GSF_SYS_CTRL_MSG) &&  (nCmd < GSF_PTZ_CMD   || nCmd >= GSF_MAX_MACHINE_CMD)) ||
		((nType >= GSF_GET_PROTO_PARA && nType <= GSF_SET_PROTO_PARA) && (nCmd < GSF_PROTOCOL_CUSTOM || nCmd >= GSF_PROTOCOL_BUTT)))
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return -1;
	}
	return gsf_web_retrans_proc(NULL, nType, nCmd, nChn, (char *)pData, nSize);
}

int DEVSDK_Capture(int nChn,const char *pszFileName)
{
	if(pszFileName == NULL)
		return DEVSDK_ERR_PARAM;
	return ipc_cmd_cli_proc_pic(nChn, pszFileName);
}

int DEVSDK_Capture_v2(int nChn, int nStream, const char *pszFileName)
{
	if(pszFileName == NULL)
		return DEVSDK_ERR_PARAM;
	return ipc_cmd_cli_proc_pic_v2(nChn, nStream, pszFileName);
}

int DEVSDK_CaptureData(int nChn,CaptureDataCallbackFunc fCallback, void *pUser)
{
	if(fCallback == NULL)
	{
		printf("[%s %d] Invalid argument:callback(%p) !!\n", __func__, __LINE__, fCallback);
		g_nErrno = DEVSDK_ERR_PARAM;
		return DEVSDK_ERR_PARAM;
	}
	GSF_IPC_CMD_CAPTURE_HDL *pHdl = (GSF_IPC_CMD_CAPTURE_HDL *)calloc(1, sizeof(GSF_IPC_CMD_CAPTURE_HDL));
	if (pHdl == NULL)
	{
		g_nErrno = DEVSDK_ERR_ALLOC;
		return DEVSDK_ERR_ALLOC;
	}
	pHdl->fCallback= fCallback;
	pHdl->pUser = pUser;
	pHdl->nChn = nChn;

	if (pthread_create(&pHdl->thid, NULL, gsf_ipc_cmd_cli_capture_task, pHdl) != 0)
	{
		free(pHdl);
		g_nErrno = DEVSDK_ERR_ALLOC;
		return DEVSDK_ERR_THREAD;
	}

	return 0;
}

DEVSDK_API int DEVSDK_GetVideoFile(int nStream, int nStartTime, int nStopTime, VideoFileCallbackFunc fCallback, void *pUser)
{
	if (nStream < 0 || nStartTime > nStopTime || nStopTime == 0 || fCallback == NULL)
	{
		printf("[%s %d] Invalid argument: nStream(%d) Time(%d -> %d) callback(%p) !!\n", __func__, __LINE__, 
			nStream, nStartTime, nStopTime, fCallback);		
		g_nErrno = DEVSDK_ERR_PARAM;
		return DEVSDK_ERR_PARAM;
	}
	GSF_IPC_CMD_VIDEO_HDL *pHdl = (GSF_IPC_CMD_VIDEO_HDL *)calloc(1, sizeof(GSF_IPC_CMD_VIDEO_HDL));
	if (pHdl == NULL)
	{		
		g_nErrno = DEVSDK_ERR_ALLOC;
		return DEVSDK_ERR_ALLOC;
	}
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nStream = nStream;
	pHdl->nStartTime = nStartTime;
	pHdl->nStopTime = nStopTime;
	
	if (pthread_create(&pHdl->thid, NULL, gsf_ipc_cmd_cli_AVFile_task, pHdl) != 0)
	{
		free(pHdl);
		g_nErrno = DEVSDK_ERR_THREAD;
		return DEVSDK_ERR_THREAD;
	}
	return 0;
}

GetVideoFileHdl DEVSDK_StartGetVideoFile(int nStream, unsigned long long uStartTimeMs, unsigned long long uStopTimeMs, VideoFileCallbackFunc_V2 fCallback, void *pUser)
{
	if (nStream < 0 || uStartTimeMs > uStopTimeMs || uStopTimeMs == 0 || fCallback == NULL)
	{
		printf("[%s %d] Invalid argument: nStream(%d) Time(%d -> %d) callback(%p) !!\n", __func__, __LINE__, 
			nStream, uStartTimeMs, uStopTimeMs, fCallback);		
		g_nErrno = DEVSDK_ERR_PARAM;
		return NULL;
	}
	GSF_IPC_CMD_VIDEO_HDL_V2 *pHdl = (GSF_IPC_CMD_VIDEO_HDL_V2 *)calloc(1, sizeof(GSF_IPC_CMD_VIDEO_HDL_V2));
	if (pHdl == NULL)
	{		
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nStream = nStream;
	pHdl->uStartTimeMs = uStartTimeMs;
	pHdl->uStopTimeMs = uStopTimeMs;
	
	if (pthread_create(&pHdl->thid, NULL, gsf_ipc_cmd_cli_AVFile_task_v2, pHdl) != 0)
	{
		free(pHdl);
		g_nErrno = DEVSDK_ERR_THREAD;
		printf("[%s %d] create avfile task v2 err\n",__func__,__LINE__);
		return NULL;
	}
	return (GetVideoFileHdl)pHdl;
}

int DEVSDK_StopGetVideoFile(GetVideoFileHdl hdl)
{
	if (hdl)
	{
		// sync stop
		gsf_ipc_cmd_cli_stop(hdl, GSF_IPC_CMD_MP4_VIDEO_V2);
		free(hdl);
	}
	return 0;
}

DEVSDK_API int DEVSDK_GetRecordFile(int nRecType, int nStartTime, int nStopTime, RecordFileCallbackFunc fCallback, void *pUser)
{
	if (nRecType == 0 || nStartTime > nStopTime || nStopTime == 0 || fCallback == NULL)
	{
		printf("[%s %d] Invalid argument: nRecType(%0#x) Time(%d -> %d) callback(%p) !!\n", __func__, __LINE__, 
			nRecType, nStartTime, nStopTime, fCallback);		
		g_nErrno = DEVSDK_ERR_PARAM;
		return DEVSDK_ERR_PARAM;
	}
	GSF_IPC_CMD_RECFILE_HDL *pHdl = (GSF_IPC_CMD_RECFILE_HDL *)calloc(1, sizeof(GSF_IPC_CMD_RECFILE_HDL));
	if (pHdl == NULL)
	{
		g_nErrno = DEVSDK_ERR_ALLOC;
		return DEVSDK_ERR_ALLOC;
	}
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nRecType = nRecType;
	pHdl->nStartTime = nStartTime;
	pHdl->nStopTime = nStopTime;
	
	if (pthread_create(&pHdl->thid, NULL, gsf_ipc_cmd_cli_RecFile_task, pHdl) != 0)
	{
		free(pHdl);		
		g_nErrno = DEVSDK_ERR_THREAD;
		return DEVSDK_ERR_THREAD;
	}
	return 0;
}

DEVSDK_API int DEVSDK_ReleaseFile(const char *pszFileName)
{
	if (pszFileName == NULL || pszFileName[0] == '\0')
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return DEVSDK_ERR_PARAM;
	}
	if (access(pszFileName, F_OK) != 0)
	{		
		g_nErrno = DEVSDK_ERR_FILE_NOEXIST;
		return DEVSDK_ERR_FILE_NOEXIST;
	}
	DEVSDK_RET ret;
	ret = gsf_ipc_cmd_cli_release_file(-1, pszFileName);
	if(ret != DEVSDK_OK)
		g_nErrno = ret;
	return ret;
}

RawHdl DEVSDK_StartGetRawData(int nStream, int nStartTime, int nStopTime, RawDataCallbackFunc fCallback, void *pUser)
{
	return (RawHdl)DEVSDK_StartGetRealAVData( nStream, nStartTime, nStopTime, fCallback, pUser);
}

int DEVSDK_StopGetRawData(RawHdl hdl)
{
	return DEVSDK_StopGetRealAVData( hdl);
}


RealAVHdl DEVSDK_StartGetRealAVData(int nStream, int nStartTime, int nStopTime, RealAVDataCallbackFunc fCallback, void *pUser)
{
	
	if (nStream < 0 || nStartTime > nStopTime || nStopTime == 0 || fCallback == NULL)
	{
		printf("[%s %d] Invalid argument: nStream(%d) Time(%d -> %d) callback(%p) !!\n", __func__, __LINE__, 
			nStream, nStartTime, nStopTime, fCallback);
		g_nErrno = DEVSDK_ERR_PARAM;
		return NULL;
	}
	GSF_IPC_CMD_REAL_AV_HDL *pHdl = (GSF_IPC_CMD_REAL_AV_HDL *)calloc(1, sizeof(GSF_IPC_CMD_REAL_AV_HDL));
	if (pHdl == NULL)
	{
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nStream = nStream;
	pHdl->nStartTime = nStartTime;
	pHdl->nStopTime  = nStopTime;
	
	if (pthread_create(&pHdl->thid, NULL, gsf_ipc_cmd_cli_RealAV_task, pHdl) != 0)
	{
		free(pHdl);		
		g_nErrno = DEVSDK_ERR_THREAD;
		return NULL;
	}
	return (RealAVHdl)pHdl;
}

int DEVSDK_StopGetRealAVData(RawHdl hdl)
{
	if (hdl)
	{
		// sync stop
		gsf_ipc_cmd_cli_stop(hdl, GSF_IPC_CMD_REAL_AV_DATA);
		free(hdl);
	}
	return 0;
}


RawSensorHdl DEVSDK_StartGetRawSensorData(int nBitWidth,int nCompressMode, RawSensorDataCallbackFunc fCallback, void *pUser)
{
#ifdef GSF_VPSS_YUV_SUPP
	if ( fCallback == NULL )
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return NULL;
	}
	GSF_IPC_CMD_RAW_SENSOR_HDL *pHdl = (GSF_IPC_CMD_RAW_SENSOR_HDL *)calloc(1, sizeof(GSF_IPC_CMD_RAW_SENSOR_HDL));
	if (pHdl == NULL)
	{		
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nBitWidth = nBitWidth;
	pHdl->nCompressMode = nCompressMode;
	
	if (pthread_create(&pHdl->thid, NULL, gsf_ipc_cmd_cli_RawSensor_task, pHdl) != 0)
	{
		free(pHdl);		
		g_nErrno = DEVSDK_ERR_THREAD;
		return NULL;
	}
	return (RawSensorHdl)pHdl;
#else
	return (RawSensorHdl)vpss_supp_compile_tips();
#endif

}

int DEVSDK_StopGetRawSensorData(RawSensorHdl hdl)
{
#ifdef GSF_VPSS_YUV_SUPP
	if (hdl)
	{
		// sync stop
		gsf_ipc_cmd_cli_stop(hdl, GSF_IPC_CMD_RAW_SENSOR_DATA);
		free(hdl);
	}
	return 0;
#else
	return vpss_supp_compile_tips();
#endif

}

RealAVEncInfoHdl DEVSDK_StartGetRealAVEncInfo(int nStream, RealAVEncInfoCallbackFunc fCallback, void *pUser)
{
	
	if (nStream < 0 || fCallback == NULL)
	{
		printf("[%s %d] Invalid argument: nStream(%d) callback(%p) !!\n", __func__, __LINE__, nStream, fCallback);		
		g_nErrno = DEVSDK_ERR_PARAM;
		return NULL;
	}
	GSF_IPC_CMD_REALAV_ENC_INFO_HDL *pHdl = (GSF_IPC_CMD_REALAV_ENC_INFO_HDL *)calloc(1, sizeof(GSF_IPC_CMD_REALAV_ENC_INFO_HDL));
	if (pHdl == NULL)
	{		
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nStream = nStream;
	
	if (pthread_create(&pHdl->thid, NULL, gsf_ipc_cmd_cli_RealAV_enc_info_task, pHdl) != 0)
	{
		free(pHdl);		
		g_nErrno = DEVSDK_ERR_THREAD;
		return NULL;
	}
	return (RealAVEncInfoHdl)pHdl;
}

int DEVSDK_StopGetRealAVEncInfo(RealAVEncInfoHdl hdl)
{
	if (hdl)
	{
		// sync stop
		gsf_ipc_cmd_cli_stop(hdl, GSF_IPC_CMD_REALAV_ENC_INFO);
		free(hdl);
	}
	return 0;
}


RecHdl DEVSDK_StartGetRecData(int nRecType, int nStartTime, int nStopTime, RecDataCallbackFunc fCallback, void *pUser)
{
	if (nRecType == 0 || nStartTime > nStopTime || nStopTime == 0 || fCallback == NULL)
	{
		printf("[%s %d] Invalid argument: nRecType(%0#x) Time(%d -> %d) callback(%p) !!\n", __func__, __LINE__, 
			nRecType, nStartTime, nStopTime, fCallback);		
		g_nErrno = DEVSDK_ERR_PARAM;
		return NULL;
	}
	GSF_IPC_CMD_REC_HDL *pHdl = (GSF_IPC_CMD_REC_HDL *)calloc(1, sizeof(GSF_IPC_CMD_REC_HDL));
	if (pHdl == NULL)
	{		
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nRecType = nRecType;
	pHdl->nStartTime = nStartTime;
	pHdl->nStopTime = nStopTime;
	
	if (pthread_create(&pHdl->thid, NULL, gsf_ipc_cmd_cli_rec_task, pHdl) != 0)
	{
		free(pHdl);		
		g_nErrno = DEVSDK_ERR_THREAD;
		return NULL;
	}
	return (RecHdl)pHdl;
}

int DEVSDK_StopGetRecData(RecHdl hdl)
{
	if (hdl)
	{
		// sync stop
		gsf_ipc_cmd_cli_stop(hdl, GSF_IPC_CMD_REC_DATA);
		free(hdl);
	}
	return 0;
}

/*
int DEVSDK_YUVToBGR(VIDEO_FRAME_INFO *pstFrame, int nPanar, unsigned char *pBGRData, int nBufSize)
{
	return 0;
}

int DEVSDK_YUVResize(VIDEO_FRAME_INFO *pstFrame, VIDEO_FRAME_INFO *pDstFrame, int nDstWidth, int nDstHeight)
{
	return 0;
}
*/

YuvHdl DEVSDK_StartGetYUV(int nChan, int nWidth, int nHeight, YUVDataCallbackFunc fCallback, void *pUser)
{
#ifdef GSF_VPSS_YUV_SUPP
	if (fCallback == NULL || nChan < 0 || nWidth < 0 || nHeight < 0 || pUser == NULL)
	{
		printf("[%s %d] Invalid argument: callback(%p) !!\n", __func__, __LINE__, fCallback);		
		g_nErrno = DEVSDK_ERR_PARAM;
		return NULL;
	}
	GSF_VPSS_YUV_HDL *pHdl = (GSF_VPSS_YUV_HDL *)calloc(1, sizeof(GSF_VPSS_YUV_HDL));
	if (pHdl == NULL)
	{		
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nVpssGrp = 0;
	pHdl->nVpssChn = nChan;
	pHdl->nWidth = nWidth;
	pHdl->nHeight = nHeight;
	
	if (pthread_create(&pHdl->thid, NULL, gsf_vpss_yuv_task, pHdl) != 0)
	{
		free(pHdl);
		g_nErrno = DEVSDK_ERR_THREAD;
		return NULL;
	}
	return (YuvHdl)pHdl;
#else
	vpss_supp_compile_tips();
	return NULL;
#endif
}

int DEVSDK_StopGetYUV(YuvHdl hdl)
{
#ifdef GSF_VPSS_YUV_SUPP
	if (hdl)
	{
		gsf_vpss_yuv_stop(hdl);
		free(hdl);
		hdl = NULL;
	}
#else
	vpss_supp_compile_tips();
#endif
	return 0;
}

int DEVSDK_EnableVpssVencChn(int nVpssVencChn)
{
	int ret = 0;
#ifdef GSF_VPSS_YUV_SUPP

	//ret = EnableVpssVencChn( nVpssVencChn);
#else
	vpss_supp_compile_tips();
#endif
	if(ret != 0)
		g_nErrno = DEVSDK_ERR_NOUNBIND;
	return ret;
}

int DEVSDK_YuvDataProcPrepare(int VpssVencChn, int nWidth, int nHeight,int userMsg,int nRotate)
{
	int ret = 0;
#ifdef GSF_VPSS_YUV_SUPP

	ret = yuvDataProcPrepare( VpssVencChn, nWidth, nHeight, userMsg,nRotate);
#else
	vpss_supp_compile_tips();
#endif
	if(ret != 0)
		g_nErrno = ret;
	return ret;
}

/////////////////////////////НОВОЕ/////////////////////////////////


int DEVSDK_SetAiData(YuvHdl hdl, AI_DATA_INFO_S *pData, int nCount)
{
#ifdef GSF_VPSS_YUV_SUPP
    if (!hdl) {
        return -1;
    }
    GSF_VPSS_YUV_HDL *pH = (GSF_VPSS_YUV_HDL *)hdl;  // приводим
    /* ограничим размер */
    if (nCount > GSF_VPSS_YUV_MAX_AI_DATA) {
        nCount = GSF_VPSS_YUV_MAX_AI_DATA;
    }
    /* копируем */
    memcpy(pH->stAiData, pData, nCount * sizeof(AI_DATA_INFO_S));
    pH->nAiDataNum = nCount;
    return 0;
#else
    /* Если не скомпилировано с VPSS_YUV_SUPP */
    return vpss_supp_compile_tips();
#endif
}

/////////////////////////////НОВОЕ/////////////////////////////////

YuvHdl DEVSDK_GetYUVDataProcV2(int nVpssGrp, int nVpssChn, int nWidth, int nHeight,int userMsg,YUVDataCallbackFunc fCallback, void *pUser)
{
#ifdef GSF_VPSS_YUV_SUPP
		if (fCallback == NULL || nVpssChn < 0 || nWidth < 0 || nHeight < 0 || userMsg < 0 || pUser ==NULL)
		{
			printf("[%s %d] Invalid argument: callback(%p) !!\n", __func__, __LINE__, fCallback);
			g_nErrno = DEVSDK_ERR_PARAM;
			return NULL;
		}

		GSF_VPSS_YUV_HDL *pHdl = (GSF_VPSS_YUV_HDL *)calloc(1, sizeof(GSF_VPSS_YUV_HDL));
		if (pHdl == NULL)
		{
			g_nErrno = DEVSDK_ERR_ALLOC;
			return NULL;
		}
		pHdl->fCallback = fCallback;
		pHdl->pUser = pUser;
		pHdl->nVpssGrp = nVpssGrp;
		pHdl->nVpssChn = nVpssChn;
		pHdl->nWidth = nWidth;
		pHdl->nHeight = nHeight;
		pHdl->yuvDataProcFlag = 1;
		pHdl->userMsg = userMsg;
		
		if (pthread_create(&pHdl->thid, NULL, gsf_vpss_yuv_task, pHdl) != 0)
		{
			free(pHdl);			
			g_nErrno = DEVSDK_ERR_THREAD;
			return NULL;
		}
		return (YuvHdl)pHdl;
#else
		vpss_supp_compile_tips();
		return NULL;
#endif
}


YuvHdl DEVSDK_GetYUVDataProc(int nVpssChn, int nWidth, int nHeight,int userMsg,YUVDataCallbackFunc fCallback, void *pUser)
{
#ifdef GSF_VPSS_YUV_SUPP
		return DEVSDK_GetYUVDataProcV2(0, nVpssChn, nWidth, nHeight, userMsg, fCallback, pUser);
#else
		vpss_supp_compile_tips();
		return NULL;
#endif
}

int DEVSDK_StopGetYUVDataProc(YuvHdl hdl)
{
#ifdef GSF_VPSS_YUV_SUPP
	if (hdl)
	{
		gsf_vpss_yuv_stop(hdl);
		free(hdl);
		hdl = NULL;
	}
#else
	vpss_supp_compile_tips();
#endif
	return 0;
}


YuvHdl DEVSDK_SetYuvOsdProc(int nVpssChn, int nWidth, int nHeight,int userMsg,YUVDataCallbackFunc fCallback, void *pUser)
{
#ifdef GSF_VPSS_YUV_SUPP
		if (fCallback == NULL || nVpssChn < 0 || nWidth < 0 || nHeight < 0 || userMsg < 0 || pUser ==NULL)
		{
			printf("[%s %d] Invalid argument: callback(%p) !!\n", __func__, __LINE__, fCallback);
			g_nErrno = DEVSDK_ERR_PARAM;
			return NULL;
		}

		GSF_VPSS_YUV_HDL *pHdl = (GSF_VPSS_YUV_HDL *)calloc(1, sizeof(GSF_VPSS_YUV_HDL));
		if (pHdl == NULL)
		{
			g_nErrno = DEVSDK_ERR_ALLOC;
			return NULL;
		}
		pHdl->fCallback = fCallback;
		pHdl->pUser = pUser;
		pHdl->nVpssGrp = 0;
		pHdl->nVpssChn = nVpssChn;
		pHdl->nWidth = nWidth;
		pHdl->nHeight = nHeight;
		pHdl->yuvDataProcFlag = 1;
		pHdl->userMsg = userMsg;
		
		if (pthread_create(&pHdl->thid, NULL, gsf_vpss_yuv_osd_task, pHdl) != 0)
		{
			free(pHdl);			
			g_nErrno = DEVSDK_ERR_THREAD;
			return NULL;
		}
		return (YuvHdl)pHdl;
#else
		vpss_supp_compile_tips();
		return NULL;
#endif
}



BgrHdl DEVSDK_StartGetBGR(int nVpssGrp, int nVpssChn, int nWidth, int nHeight, BGRDataCallbackFunc fCallback, void *pUser)
{
#ifdef GSF_VPSS_YUV_SUPP
	if (fCallback == NULL || nVpssGrp < 0 || nVpssChn < 0 || nWidth < 0 || nHeight < 0 || pUser == NULL)
	{
		printf("[%s %d] Invalid argument: callback(%p) !!\n", __func__, __LINE__, fCallback);		
		g_nErrno = DEVSDK_ERR_PARAM;
		return NULL;
	}
	GSF_VPSS_BGR_HDL *pHdl = (GSF_VPSS_BGR_HDL *)calloc(1, sizeof(GSF_VPSS_BGR_HDL));
	if (pHdl == NULL)
	{
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nVpssGrp = nVpssGrp;
	pHdl->nVpssChn = nVpssChn;
	pHdl->nWidth = nWidth;
	pHdl->nHeight = nHeight;
	
	if (pthread_create(&pHdl->thid, NULL, gsf_vpss_bgr_task, pHdl) != 0)
	{
		free(pHdl);
		g_nErrno = DEVSDK_ERR_THREAD;
		return NULL;
	}
	return (BgrHdl)pHdl;
#else
	vpss_supp_compile_tips();
	return NULL;
#endif
}

int DEVSDK_StopGetBGR(BgrHdl hdl)
{
#ifdef GSF_VPSS_YUV_SUPP
	if (hdl)
	{
		gsf_vpss_bgr_stop(hdl);
		free(hdl);
		hdl = NULL;
	}
#else
	vpss_supp_compile_tips();
#endif
	return 0;
}

int DEVSDK_PlayAudio(int nChn, DEVSDK_PLAY_AUDIO_PARAM_S *pstParam)
{
	if (pstParam == NULL)
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return -1;
	}
	int nRet =  gsf_web_retrans_proc(NULL, GSF_SYS_CTRL_MSG, GSF_PLAY_AUDIO_CMD, nChn, (char *)pstParam, sizeof(DEVSDK_PLAY_AUDIO_PARAM_S));
	if(nRet != 0)
		g_nErrno = DEVSDK_ERR_GETORSETPARAMERR;
	return nRet;
}

PtzHdl DEVSDK_StartGetPtz(int nChn, PtzMsgCallbackFunc fCallback, void *pUser)
{
	if(fCallback == NULL)
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return NULL;
	}
	GSF_PTZ_NOTIFY_MSG_HDL *PHdl= (GSF_PTZ_NOTIFY_MSG_HDL* )calloc(1,sizeof(GSF_PTZ_NOTIFY_MSG_HDL));
	if(PHdl == NULL)
	{
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	PHdl->fCallback=fCallback;
	PHdl->User=pUser;
	if (pthread_create(&PHdl->thid, NULL, gsf_ptz_msg_task, PHdl) != 0)
	{
		free(PHdl);		
		g_nErrno = DEVSDK_ERR_THREAD;
		return NULL;
	}
	return (PtzHdl)PHdl;
}

int DEVSDK_StopGetptz(PtzHdl hdl)
{
	if (hdl)
	{
		gsf_ptz_msg_stop(hdl);
		free(hdl);
		hdl = NULL;
	}
	return 0;
}

AlarmHdl DEVSDK_StartGetAlarm(int nChn, unsigned int u32Mask, AlarmMsgCallbackFunc fCallback, void *pUser)
{
	if (u32Mask == 0 || fCallback == NULL)
	{
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}

	GSF_ALARM_MSG_HDL *pHdl = (GSF_ALARM_MSG_HDL *)calloc(1, sizeof(GSF_ALARM_MSG_HDL));
	if (pHdl == NULL)
	{
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	pHdl->fCallback = fCallback;
	pHdl->pUser = pUser;
	pHdl->nChn = nChn;
	pHdl->u32Mask = u32Mask;

	if (pthread_create(&pHdl->thid, NULL, gsf_alarm_msg_task, pHdl) != 0)
	{
		free(pHdl);		
		g_nErrno = DEVSDK_ERR_THREAD;
		return NULL;
	}
	return (AlarmHdl)pHdl;
}

int DEVSDK_StopGetAlarm(AlarmHdl hdl)

{
	if (hdl)
	{
		gsf_alarm_msg_stop(hdl);
		free(hdl);
		hdl = NULL;
	}
	return 0;
}

int DEVSDK_Comm485Prepare(COMM_PARA_S *param)
{
	if(param == NULL)
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return -1;
	}
	
	int  nRet = 0;
	nRet =  comm485_prepare( param);
	if(nRet < 0)
		g_nErrno = DEVSDK_ERR_485COMMPREPAREERR;
	
	return nRet; 
}

int DEVSDK_Comm485Write(int fd,char *writeMsg)
{
	if(writeMsg == NULL || fd < 0)
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return -1;
	}
	int nRet = 0;
	nRet = comm485_write(fd,writeMsg);
	if(nRet != 0)
		g_nErrno = DEVSDK_ERR_485WRITEERR;
	return nRet;
}

int DEVSDK_Comm485Read(int fd,char *readMsg)
{
	if(fd < 0)
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return -1;
	}
	int nRet = 0;
	nRet = comm485_read(fd,readMsg);
	if(nRet != 0)
	{
		g_nErrno = DEVSDK_ERR_485READERR;
	}
	return nRet;
}

int DEVSDK_Vpss_SetCrop(int nVpssGrpNo,int nEnable,int x,int y,int width,int height)
{
#ifdef GSF_VPSS_YUV_SUPP
	if(nVpssGrpNo < 0 ||  nEnable < 0 ||
	   0 > x || 0 > y || 1 > width || 1 > height)
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return -1;
	}
	int nRet =  vpss_set_crop(nVpssGrpNo, nEnable, x, y, width, height);
	if(nRet != 0)
		g_nErrno = DEVSDK_ERR_VPSSSETGROPERR;
	return nRet;
#else
	return vpss_supp_compile_tips();
#endif
}

int DEVSDK_Vpss_GetCrop(int nVpssGrpNo,int *nEnable,int *x,int *y,int *width,int *height)
{
#ifdef GSF_VPSS_YUV_SUPP
	return vpss_get_crop(nVpssGrpNo, nEnable, x, y, width, height);
#else
	return vpss_supp_compile_tips();
#endif
}

int DEVSDK_Get_UniqueId(hi_unique_id *unique_id)
{
#ifdef GSF_VPSS_YUV_SUPP
	return gsf_get_unique_id(unique_id);
#else
	return vpss_supp_compile_tips();
#endif
	
}

PDRectHdl DEVSDK_StartGetPDRectInfo()
{

	GSF_IPC_CMD_PD_RECT_HDL *pHdl = (GSF_IPC_CMD_PD_RECT_HDL *)calloc(1, sizeof(GSF_IPC_CMD_PD_RECT_HDL));
	if (pHdl == NULL)
	{
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	if (pthread_create(&pHdl->thid, NULL, gsf_ipc_cmd_get_pd_rect_info, pHdl) != 0)
	{
		free(pHdl);
		g_nErrno = DEVSDK_ERR_THREAD;
		return NULL;
	}
	
	return (PDRectHdl)pHdl;

}

int DEVSDK_StopGetPDRectInfo(PDRectHdl hdl)
{

	if (hdl)
	{
		gsf_ipc_cmd_stop_get_pd_rect_info(hdl);
		free(hdl);
	}
	
	return 0;
}

int DEVSDK_StartVencCapture(int nVpssChn,void *pUserRect, int nWidth, int nHeight, unsigned int *pFileSize, void *pUser)
{
#ifdef GSF_VPSS_YUV_SUPP
	GSF_RECT_S *pRect = (GSF_RECT_S *)pUserRect;
	return gsf_venc_capture_start(nVpssChn, pRect, nWidth, nHeight,pFileSize, pUser);
#else
	return vpss_supp_compile_tips();
#endif
	
}

int DEVSDK_VencCaptureRecv(int nVpssChn, void *pUserRect, unsigned int *pFileSize, int nWidth, int nHeight)
{
#ifdef GSF_VPSS_YUV_SUPP
	GSF_RECT_S *pRect = (GSF_RECT_S *)pUserRect;
	return gsf_venc_capture_recv(nVpssChn,pRect,pFileSize, nWidth, nHeight);
#else
	return vpss_supp_compile_tips();
#endif
}
int DEVSDK_StopVencCapture(int nVpssChn)
{
#ifdef GSF_VPSS_YUV_SUPP
	return gsf_venc_capture_stop(nVpssChn);
#else
	return vpss_supp_compile_tips();
#endif

}

PushDataHdl DEVSDK_OpenPushData()
{
	GSF_IPC_CMD_PUSH_DATA_HDL *pHdl = (GSF_IPC_CMD_PUSH_DATA_HDL *)calloc(1, sizeof(GSF_IPC_CMD_PUSH_DATA_HDL));
	if (pHdl == NULL)
	{		
		g_nErrno = DEVSDK_ERR_ALLOC;
		return NULL;
	}
	pHdl->nSeq = 0;
	pHdl->nSock = -1;
	//gsf_ipc_cmd_cli_open_push_data();
	return pHdl;
}

int DEVSDK_PushData(PushDataHdl hdl, DEVSDK_AI_DATA_S *pData)
{
	int ret = gsf_ipc_cmd_cli_push_data(hdl, pData);
	if (ret != DEVSDK_OK)
	{
		g_nErrno = ret;
	}
	return ret;
}

int DEVSDK_ClosePushData(PushDataHdl hdl)
{
	return gsf_ipc_cmd_cli_close_push_data(hdl);
}

int DEVSDK_LockFile(const char *pszFileName)
{
	struct stat stat_buf;
	mode_t st_mode = 0;

	if (pszFileName == NULL)
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return -1;
	}
	
	if (stat(pszFileName, &stat_buf) < 0)
	{
		g_nErrno = DEVSDK_ERR_FILE_NOEXIST;
		return -1;
	}
	
	st_mode = stat_buf.st_mode;
	st_mode |= S_IWOTH;
	st_mode |= S_IWGRP;

	return chmod(pszFileName, st_mode);
}

int DEVSDK_UnLockFile(const char *pszFileName)
{
	struct stat stat_buf;
	mode_t st_mode = 0;

	if (pszFileName == NULL)
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return -1;
	}
	
	if (stat(pszFileName, &stat_buf) < 0)
	{
		g_nErrno = DEVSDK_ERR_FILE_NOEXIST;
		return -1;
	}
	
	st_mode = stat_buf.st_mode;
	st_mode &= ~S_IWOTH;
	st_mode &= ~S_IWGRP;

	return chmod(pszFileName, st_mode);
}

int DEVSDK_IsFileLocked(const char *pszFileName)
{
	struct stat stat_buf;

	if (pszFileName == NULL)
	{
		g_nErrno = DEVSDK_ERR_PARAM;
		return -1;
	}
	
	if (stat(pszFileName, &stat_buf) < 0)
	{
		g_nErrno = DEVSDK_ERR_FILE_NOEXIST;
		return -1;
	}

	return ((stat_buf.st_mode & S_IWOTH) || (stat_buf.st_mode & S_IWGRP));
}

static int _AllocDevUpgReq(const char *pszFileName, void **ppReq)
{
	if (ppReq == NULL)
	{
		return 0;
	}
	GSF_UPGRADE_CMD_S *pCmd = (GSF_UPGRADE_CMD_S *)calloc(1, sizeof(GSF_UPGRADE_CMD_S));
	if (pCmd)
	{
		strncpy(pCmd->szFilePath, pszFileName, sizeof(pCmd->szFilePath)-1);
		*ppReq = pCmd;
		return sizeof(GSF_UPGRADE_CMD_S);
	}
	return 0;
}

static int _DelDevUpgReq(void *pReq)
{
	if (pReq) 
	{
		free(pReq);
	}
	return 0;
}

int DEVSDK_DeviceUpgrade(const char *pszFileName)
{
	if (pszFileName == NULL || pszFileName[0] == '\0')
	{
		printf("Invalid argument: \n");
		return -1;
	}
	if (access(pszFileName, F_OK) != 0)
	{
		printf("Invalid FileName: %s\n", pszFileName);
		return -2;
	}
	void *pDevUpgReq = NULL;
	int nSize = _AllocDevUpgReq(pszFileName, &pDevUpgReq);
	if (pDevUpgReq && nSize > 0)
	{
		int nRet = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_UPGRADE_CMD, pDevUpgReq, nSize);
		_DelDevUpgReq(pDevUpgReq);
		pDevUpgReq = NULL;
		return nRet;
	}
	return -3;
}

int DEVSDK_GetDevUpgradeStatus()
{
	GSF_UPGRADE_STATE_S stUpgState = {0};
	int nRet = DEVSDK_ParamConfig(0, GSF_SYS_STATE_MSG, GSF_UPGRADE_CMD, &stUpgState, sizeof(stUpgState));
	if (nRet == 0)
	{
		return (int)stUpgState.u8Percent;
	}
	return -1;
}

