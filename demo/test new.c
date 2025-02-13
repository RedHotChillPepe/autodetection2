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
#include <signal.h>
#include <string.h>
#include "devsdk.h"
#include "gsf_param.h"
#include "gsf_ptz.h"
#include <pthread.h>
#include "comm485.h"
#include "gsf_hi_sys_info.h"
#include "gsf_ipc_cmd.h"
#include "gsf_ptz_msg.h"

#define REC_USER	100
#define YUV_USER	101
#define ALARM_USER 	102
#define VIDEO_USER  103
#define CAPTURE_USER 104
#define BGR_USER 105
#define AV_ENC_INFO_USER 106
#define REALAV_USER 107
#define RAWSENSOR_USER 108
#define PTZ_USER 109

static int g_exit = 0;

static char g_video_filename[DEVSDK_PATH_LEN] = {0};
#define GSF_APP_TEST 1

int fd = -1;
int writeFlag = 0;


/**************IPC APP CTRL CMD******************/

#define GSF_APP_CRTL_CMD_PTZ 		"PTZ"
#define GSF_APP_CRTL_CMD_PIC 		"SNAP"
#define GSF_APP_CTRL_CMD_PIC_V2 	"SNAPV2"
#define GSF_APP_CTRL_CMD_PIC_DATA 	"SNAPDATA"
#define GSF_APP_CTRL_CMD_VIDEO 		"VIDEO"
#define GSF_APP_CTRL_CMD_VIDEO_V2 	"VIDEOV2"
#define GSF_APP_CTRL_CMD_REC 		"REC"
#define GSF_APP_CTRL_CMD_REC_FILE 	"RECFILE"
#define GSF_APP_CTRL_CMD_YUV 		"YUV"
#define GSF_APP_CTRL_CMD_YUV_PROC 	"YUVPROC"
#define GSF_APP_CTRL_CMD_YUV_PROC_V2 "YUVPROCV2"
#define GSF_APP_CTRL_CMD_REAL_AV	"REALAV"
#define GSF_APP_CTRL_CMD_BGR		"BGR"
#define GSF_APP_CTRL_CMD_AUDIO		"AUDIO"
#define GSF_APP_CTRL_CMD_ALARM		"ALARM"
#define GSF_APP_CTRL_CMD_COMM_485	"COMM485"
#define	GSF_APP_CTRL_CMD_MD			"MD"
#define GSF_APP_CTRL_CMD_MULTIPLE	"MULTIPLE"
#define GSF_APP_CTRL_CMD_IMA		"IMA"
#define	GSF_APP_CTRL_CMD_3A			"3A"
#define	GSF_APP_CTRL_CMD_TEMP		"TEMP"
#define	GSF_APP_CTRL_CMD_PD			"PD"
#define	GSF_APP_CTRL_CMD_DAY_NIGHT	"DAYNIGHT"
#define	GSF_APP_CTRL_CMD_USER		"USER"
#define	GSF_APP_CTRL_CMD_NET		"NET"
#define	GSF_APP_CTRL_CMD_HDMI		"HDMI"
#define	GSF_APP_CTRL_CMD_CAMPOS		"CAMPOS"
#define	GSF_APP_CTRL_CMD_ROI		"ROI"
#define	GSF_APP_CTRL_CMD_ALARMOUT	"ALARMOUT"
#define	GSF_APP_CTRL_CMD_VPSS_CROP	"VPSSCROP"
#define	GSF_APP_CTRL_CMD_UNIQUEID	"UNIQUEID"
#define	GSF_APP_CTRL_CMD_VER		"GETVER"
#define	GSF_APP_CTRL_CMD_REAL_AV_ENC	"REALAVENC"
#define	GSF_APP_CTRL_CMD_RAW_DATA	"RAW"
#define	GSF_APP_CTRL_CMD_VENC_SHOOT	"VENCSHOOT"
#define GSF_APP_CTRL_CMD_DEV_UPG    "UPG"
#define GSF_APP_CTRL_CMD_YUV_OSD_TIME    "YUVOSDTime"
#define GSF_APP_CTRL_CMD_PUSH_DATA  "PUSHDATA"



void exit_main(int signal)
{
	printf("Terminating Program ...\n");
	g_exit = 1;
}

void main_signal()
{
	signal(SIGPIPE, SIG_IGN);

	signal(SIGTERM, exit_main);
	signal(SIGINT,  exit_main);
}

int test_capture()
{
	int ret = DEVSDK_Capture( 0,"/memipcmd/ipc_cmd_pic.jpg");
	if (0 != ret)
	{
		printf("DEVSDK_Capture err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	return 0;
}

int test_capture_v2()
{
	int ret = DEVSDK_Capture_v2(2,0,"/memipcmd/ipc_cmd_pic.jpg");
	if (0 != ret)
	{
		printf("DEVSDK_Capture err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	return 0;
}

int captureDataCallback(unsigned char *pData, int nSize, void *pUser)
{
	printf("[%s %d] pData(%p) nSize(%d)\n",__FUNCTION__,__LINE__,pData,nSize);
	if (pData == NULL || pData[0] == '\0' || nSize <= 0)
	{
		printf("[%s %d] CaptureData(%p) err!!\n", __func__, __LINE__, pData);
		return 0;
	}

	if ((long)pUser != CAPTURE_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	} 


	return 0;
}

int captureDataCallback_v2(unsigned char *pData, int nSize, void *pUser)
{
	printf("[%s %d] pData(%p) nSize(%d)\n",__FUNCTION__,__LINE__,pData,nSize);
	if (pData == NULL || pData[0] == '\0' || nSize <= 0)
	{
		printf("[%s %d] CaptureData(%p) err!!\n", __func__, __LINE__, pData);
		return 0;
	}

	if ((long)pUser != CAPTURE_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	} 


	return 0;
}

int test_captureData()
{
	int ret = 0;
	ret = DEVSDK_CaptureData(0, captureDataCallback, (void *)CAPTURE_USER);
	if (0 != ret)
	{
		printf("DEVSDK_CaptureData err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	ret = DEVSDK_CaptureData(1, captureDataCallback_v2, (void *)CAPTURE_USER);
	if (0 != ret)
	{
		printf("DEVSDK_CaptureData err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	while(0 == g_exit)
	{
		sleep(1);
	}
	return 0;
}

int videoFileCallback(int nStatus, const char *pszVideoFile, void *pUser)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err, status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		return 0;
	}
	if (pszVideoFile == NULL || pszVideoFile[0] == '\0')
	{
		printf("[%s %d] videoFile(%p) err!!\n", __func__, __LINE__, pszVideoFile);
		return 0;
	}
	if ((long)pUser != VIDEO_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	}

	if (access(pszVideoFile, F_OK) != 0)
	{
		printf("[%s %d] access video file(%s) err(%s)\n", __func__, __LINE__, pszVideoFile, strerror(errno));
		return 0;
	}

	printf("[%s %d] tips: video file(%s) ready.\n must be call DEVSDK_ReleaseFile after use\n", __func__, __LINE__, pszVideoFile);

	strncpy(g_video_filename, pszVideoFile, sizeof(g_video_filename)-1);
	
	//must be call DEVSDK_ReleaseFile(pszVideoFile) to release video file after use;
	
	return 0;
}

int test_video()
{
	// 
	int ret = DEVSDK_GetVideoFile(0, time(NULL)-30, time(NULL)+30, videoFileCallback, (void *)VIDEO_USER);
	if (0 != ret)
	{
		printf("DEVSDK_GetVideoFile err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}

	while(0 == g_exit)
	{
		sleep(1);
	}
	return 0;
}

int videoFileCallback_v2(int nStatus, const char *pszVideoFile, void *pUser, unsigned long long nFirstPts, unsigned long long nLastPts)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err, status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		return 0;
	}
	if (pszVideoFile == NULL || pszVideoFile[0] == '\0')
	{
		printf("[%s %d] videoFile(%p) err!!\n", __func__, __LINE__, pszVideoFile);
		return 0;
	}
	if ((long)pUser != VIDEO_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	}

	if (access(pszVideoFile, F_OK) != 0)
	{
		printf("[%s %d] access video file(%s) err(%s)\n", __func__, __LINE__, pszVideoFile, strerror(errno));
		return 0;
	}

	printf("[%s %d] tips: video file(%s) ready.\n must be call DEVSDK_ReleaseFile after use\n", __func__, __LINE__, pszVideoFile);
	printf("[%s %d] nFirstPts = %llu, nLastPts = %llu\n",__func__,__LINE__,nFirstPts,nLastPts);
	strncpy(g_video_filename, pszVideoFile, sizeof(g_video_filename)-1);
	
	//must be call DEVSDK_ReleaseFile(pszVideoFile) to release video file after use;

	return 0;
}
int test_video_v2()
{
	unsigned long long uStopTimeMs = (unsigned long long)(time(NULL))*1000;
	unsigned long long uStartTimeMs = uStopTimeMs - 4000;
	uStopTimeMs = uStopTimeMs + 10*1000;
	
	printf("[%s %d]uStartTimeMs = %llu, uStopTimeMs = %llu\n",__func__,__LINE__,uStartTimeMs,uStopTimeMs);
	GetVideoFileHdl pHdl = DEVSDK_StartGetVideoFile(0,uStartTimeMs, uStopTimeMs, videoFileCallback_v2, (void *)VIDEO_USER);
	if (NULL != pHdl)
	{		
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetVideoFile(pHdl);
	}
	else
	{
		printf("[%s %d]DEVSDK_StartGetRealAVData err(%s)\n",__func__,__LINE__, DEVSDK_GetLastErrorMsg(NULL));
	}

	return 0;
}

int recFileCallback(int nStatus, const char *pszRecordFile, void *pUser)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err, status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		return 0;
	}
	if (pszRecordFile == NULL || pszRecordFile[0] == '\0')
	{
		printf("[%s %d] recFile(%p) err!!\n", __func__, __LINE__, pszRecordFile);
		return 0;
	}
	if ((long)pUser != REC_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	}

	if (access(pszRecordFile, F_OK) != 0)
	{
		printf("[%s %d] access rec file(%s) err(%s)\n", __func__, __LINE__, pszRecordFile, strerror(errno));
		return 0;
	}

	printf("[%s %d] tips: rec file(%s) ready.\n must be call DEVSDK_ReleaseFile after use\n", __func__, __LINE__, pszRecordFile);
	
	return 0;
}

int test_recfile()
{
	// 
	int ret = DEVSDK_GetRecordFile(0xFFFFFFFF, 1641440202, 1641455057, recFileCallback, (void *)REC_USER);
	if (0 != ret)
	{
		printf("DEVSDK_GetRecordFile err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}

	while(0 == g_exit)
	{
		sleep(1);
	}

	return 0;
}



int recDataCallback(int nStatus, DEVSDK_FRAME_HEAD_S *pFrameHead, unsigned char *pData, void *pUser)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		return 0;
	}
	if (pFrameHead == NULL || pData == NULL)
	{
		printf("[%s %d] reach end!!\n", __func__, __LINE__);
		exit_main(SIGTERM);
		return 0;
	}
	if ((long)pUser != REC_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	}
	if (DEVSDK_IS_V_FRAME(pFrameHead->u8FrameType))
	{
		if (DEVSDK_IS_I_FRAME(pFrameHead->u8FrameType))
		{
			printf("[%s %d] video frame: no(%u) type(%d) size(%u) time(%u) res(%d %d) fr(%d) enc(%d) \n", __func__, __LINE__, 
				pFrameHead->u32FrameNo, pFrameHead->u8FrameType, pFrameHead->u32FrameSize, pFrameHead->u32Time, 
				pFrameHead->unVideoInfo.u16ImageWidth, pFrameHead->unVideoInfo.u16ImageHeight, 
				pFrameHead->unVideoInfo.u8FrameRate, pFrameHead->unVideoInfo.u8EncodeType);
		}
	}
	else
	{
		printf("[%s %d] frame: no(%u) type(%d) size(%u) time(%u)\n", __func__, __LINE__,  
			pFrameHead->u32FrameNo, pFrameHead->u8FrameType, pFrameHead->u32FrameSize, pFrameHead->u32Time);
	}
	return 0;
}

int test_rec()
{
	int nRecType = DEVSDK_REC_TYPE_MD | DEVSDK_REC_TYPE_IO;
	RecHdl *pHdl = DEVSDK_StartGetRecData(nRecType, time(NULL)-86400, time(NULL), recDataCallback, (void *)REC_USER);
	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetRecData(pHdl);
	}
	else
	{
		printf("DEVSDK_StartGetRecData err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	return 0;
}

int yuvDataCallback(int nStatus, int nWidth, int nHeight, int nPixelFormat, int nSize, unsigned char *pYUVData, void *pUser)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		return 0;
	}
	if ((long)pUser != YUV_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	}
	
	printf("[%s %d] w_h(%d %d) pixelFormat(%d) size(%d) pYUV(%p)\n", __func__, __LINE__, 
		nWidth, nHeight, nPixelFormat, nSize, pYUVData);
	return 0;
}

int test_yuv()
{
	YuvHdl *pHdl = DEVSDK_StartGetYUV(0, 416, 416, yuvDataCallback, (void *)YUV_USER);
	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetYUV(pHdl);
	}
	else
	{
		printf("DEVSDK_StartGetYUV err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	return 0;
}

int realAVDataCallback(int nStatus, DEVSDK_FRAME_HEAD_S *pFrameHead, unsigned char *pData, void *pUser)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		g_exit = 1;
		return 0;
	}
	
	if ((long)pUser != REALAV_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		g_exit = 1;
		return 0;
	}
	if (pFrameHead->u8FrameType == 0xa) // audio
	{
		printf("[%s %d] video frmNo(%u) type(%d) size(%d) samplerate(%d) bit(%d) channel(%d)\n", __func__, __LINE__, 
			pFrameHead->u32FrameNo, pFrameHead->u8FrameType, pFrameHead->u32FrameSize, 
			pFrameHead->unAudioInfo.u8AudioSamples, pFrameHead->unAudioInfo.u8AudioBits, 
			pFrameHead->unAudioInfo.u8AudioChannels);
	}
	else if (pFrameHead->u8FrameType > 0xb) // skip type
	{
		// skip type != vide & audio
	}
	else // video
	{
		printf("[%s %d] video frmNo(%u) type(%d) size(%d) w_h(%d %d) timeStamp(%llu)\n", __func__, __LINE__, 
			pFrameHead->u32FrameNo, pFrameHead->u8FrameType, pFrameHead->u32FrameSize, 
			pFrameHead->unVideoInfo.u16ImageWidth, pFrameHead->unVideoInfo.u16ImageHeight,pFrameHead->u64TimeStamp);
	}
	return 0;
}

int test_realAV()
{
	RealAVHdl *pHdl = DEVSDK_StartGetRealAVData(1, time(NULL)-2, time(NULL)+86400, realAVDataCallback, (void *)REALAV_USER);
	RealAVHdl *pHdl1 = DEVSDK_StartGetRealAVData(0, time(NULL)-2, time(NULL)+3600, realAVDataCallback, (void *)REALAV_USER);
	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetRealAVData(pHdl);
		DEVSDK_StopGetRealAVData(pHdl1);
	}
	else
	{
		printf("DEVSDK_StartGetRealAVData err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	return 0;
}


int test_yuvDataProc()
{
	int nRotate = OT_ROTATION_90;
	int userMsg = 3;
	int nWidth = 3840,nHeight = 2160;

	//设置旋转需要设置正确分辨率，创建的vpss分辨率不能小于编码通道分辨率，不然送不出帧
	if(0 != DEVSDK_YuvDataProcPrepare( 0, nWidth, nHeight, userMsg,nRotate))
	{
		printf("DEVSDK_YuvDataProcPrepare err!!!\n");
		return -1;
	}
	YuvHdl *pHdl = DEVSDK_GetYUVDataProc( 0, 0, 0, 1, yuvDataCallback, (void *)YUV_USER);
	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetYUVDataProc(pHdl);
	}
	else
	{
		printf("DEVSDK_StartGetYUV err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}

}

int get_dev_chn()
{
	int nRet = 0;
	GSF_DEV_CFG_S stVer = {0};
	if (0 != (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_VER_PARAM, &stVer, sizeof(stVer))))
	{
		printf("[%s %d] get version param err(%d) msg(%s)\n", __func__, __LINE__, nRet, DEVSDK_GetLastErrorMsg(NULL));
	}
	else
	{
		printf("[%s %d] device version: \n\tserialId(%s) \n\tdevType(%d) \n\tsensorType(%d) \n\tchipType(%0#x) \n\tSoftVer(%d.%d.%d)\n\n", __func__, __LINE__, 
			stVer.u8SerialId, stVer.u16DevType, stVer.u32SensorType[0], stVer.u32ChipType, 
			(stVer.u32SoftVer>>24)&0xff, (stVer.u32SoftVer>>16)&0xff, (stVer.u32SoftVer>>8)&0xff);
	}
	return stVer.u8ChnNum;
}

int test_yuvDataProcV2()
{
	int nRotate = 0;

	if(0 != DEVSDK_YuvDataProcPrepare( 0, 0, 0, 1,nRotate))
	{
		printf("DEVSDK_YuvDataProcPrepare err!!!\n");
		return -1;
	}
	int i = 0;
	int valid_cnt = 0;
	int supp_chn = get_dev_chn();
	YuvHdl *pHdlGrp[GSF_MAX_SUPPORT_CHN] = {NULL};
	for (i = 0; i < supp_chn && i < GSF_MAX_SUPPORT_CHN; i++)
	{
		pHdlGrp[i] = DEVSDK_GetYUVDataProcV2(i, 0, 0, 0, 1, yuvDataCallback, (void *)YUV_USER);
		if (pHdlGrp[i] != NULL)
			valid_cnt++;
	}
	if (valid_cnt > 0)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		
		if (valid_cnt > 0)
		{
			for (i = 0; i < supp_chn && i < GSF_MAX_SUPPORT_CHN; i++)
			{
				if (pHdlGrp[i])
				{
					DEVSDK_StopGetYUVDataProc(pHdlGrp[i]);
					pHdlGrp[i] = NULL;
				}
			}
		}
	}
	else
	{
		printf("DEVSDK_StartGetYUV err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}

}

int bgrDataCallback(int nStatus, int nWidth, int nHeight, int nPlanar, int nSize, unsigned char *pBGRData, void *pUser)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		return 0;
	}
	if ((long)pUser != BGR_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	}

	//SaveBgrToBmpFile("/mnt/bgr.bmp",pBGRData,nWidth,nHeight);
	
	printf("[%s %d] w_h(%d %d) nPlanar(%d) size(%d) pBGR(%p)\n", __func__, __LINE__, 
		nWidth, nHeight, nPlanar, nSize, pBGRData);
	return 0;
}

int bgrDataCallback1(int nStatus, int nWidth, int nHeight, int nPlanar, int nSize, unsigned char *pBGRData, void *pUser)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		return 0;
	}
	if ((long)pUser != BGR_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	}

	//SaveBgrToBmpFile("/mnt/bgr1.bmp",pBGRData,nWidth,nHeight);
	
	printf("[%s %d] w_h(%d %d) nPlanar(%d) size(%d) pBGR(%p)\n", __func__, __LINE__, 
		nWidth, nHeight, nPlanar, nSize, pBGRData);
	return 0;
}


int test_bgr()
{
	BgrHdl *pHdl = DEVSDK_StartGetBGR(0, 0, 0, 0, bgrDataCallback, (void *)BGR_USER);
	//BgrHdl *pHdl1 = DEVSDK_StartGetBGR(0, 1, 640, 480, bgrDataCallback1, (void *)BGR_USER);
	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetBGR(pHdl);
		//DEVSDK_StopGetBGR(pHdl1);
	}
	else
	{
		printf("DEVSDK_StartGetBGR err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	return 0;
}


int test_play_audio(int argc, char **argv)
{
	char szFileName[128] = {0};
	if (argc > 1)
		strncpy(szFileName, argv[1], sizeof(szFileName)-1);
	else
		strcpy(szFileName, "/mnt/3.alaw");

	if (access(szFileName, F_OK) == 0)
	{
		DEVSDK_PLAY_AUDIO_PARAM_S stParam = {0};
		stParam.u8AEnc = DEVSDK_AENC_TYPE_G711A;
		stParam.u8DataType = 0;
		stParam.u8SampleRate = 0;
		stParam.u16Len = strnlen(szFileName, sizeof(szFileName)-1);
		strncpy(stParam.szFileName, szFileName, sizeof(stParam.szFileName)-1);
		return DEVSDK_PlayAudio(0, &stParam);
	}
	return 0;
}

int alarmMsgCallback(int nChn, int nAlarmType, int nAction, unsigned int u32Time, const char *pszJpegFile, void *pUser)
{
	if ((long)pUser != ALARM_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	}
	
	printf("[%s %d] ch(%d) nAlarmType(%d) action(%d) time(%u) jpeg(%s) pUser(%p)\n", __func__, __LINE__, 
		nChn, nAlarmType, nAction, u32Time, pszJpegFile, pUser);

	// lock file before process 
	if (access(pszJpegFile, F_OK) == 0)
	{
		if (DEVSDK_LockFile(pszJpegFile) == 0)
		{
			// proc finish, unlock file and release resource
			DEVSDK_UnLockFile(pszJpegFile);
		}
		else
		{
			printf("[%s %d] lock file(%s) err\n", __func__, __LINE__, pszJpegFile);
		}
	}
	return 0;
}

int test_alarm()
{
	AlarmHdl *pHdl = DEVSDK_StartGetAlarm(0, DEVSDK_ALARM_MASK_ALL, alarmMsgCallback, (void *)ALARM_USER);
	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetAlarm(pHdl);
	}
	else
	{
		printf("DEVSDK_StartGetAlarm err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	return 0;
}

void gsf_comm_write_task()
{
	char WriteBuff[64] = "";

	while(1)
	{
		fgets(WriteBuff, 64, stdin);
		if(strlen(WriteBuff) != 0)
			writeFlag = 1;
		if(writeFlag)
		{
			DEVSDK_Comm485Write( fd, WriteBuff);
			writeFlag = 0;
		}
	}
	
}


int test_comm485()
{
	char readBuff[64] = "";
	pthread_t	WriteThdId;
	COMM_PARA_S param = {0};

	param.baudrate = COMM_BAUDRATE_9600;
	param.databit = COMM_DATABIT_8;
	param.stopbit = COMM_STOPBIT_ONE;
	param.checkType = COMM_CHECKTYPE_NONE;
	
	fd = DEVSDK_Comm485Prepare( &param);
	
	if (pthread_create(&WriteThdId,0, (void *)gsf_comm_write_task, NULL) != 0)
	{
		printf("comm write task create err\n");
		exit(1);
	}
	while(1)
	{
		if(writeFlag == 0)
		{
			DEVSDK_Comm485Read( fd, readBuff);
			if(strlen(readBuff) != 0)
			{	
				printf("readBuff=%s\n",readBuff);
				bzero(readBuff,sizeof(readBuff));
			}
		}		
	}
}

// test for motion detection parameter
int test_param_md()
{
	int i = 0;
	int nRet = 0;
	GSF_MD_CFG_S stMd = {0};
	if (0 == (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_VMD_PARAM, &stMd, sizeof(stMd))))
	{
		printf("----------motion-detect-param-----------\n");
		printf("enable(%d) level(%d) keepTime(%d) \n", 
			stMd.u8Enable, stMd.u8Level, stMd.u16AlarmKeepTime);
		for (i = 0; i < GSF_VMD_REG_NUM; i++)
		{
			printf("rect%d(%d %d %d %d) ", i, stMd.struRect[i].s32X, stMd.struRect[i].s32Y, 
				stMd.struRect[i].u32Width, stMd.struRect[i].u32Height);
		}
		printf("\n----------------------------------------\n");
	}
	else
	{
		printf("[%s %d] DEVSDK_ParamConfig: get md param err(%d)\n", __func__, __LINE__, nRet);
	}
	stMd.u8Enable = 1 - stMd.u8Enable;
	// set
	if (0 == (nRet = DEVSDK_ParamConfig(0, GSF_SET_PARAM_MSG, GSF_VMD_PARAM, &stMd, sizeof(stMd))))
	{
		printf("set motion detect param ok!\n");
	}
	else
	{
		printf("[%s %d] DEVSDK_ParamConfig: set md param err(%d)\n", __func__, __LINE__, nRet);
	}
	// get after set
	if (0 == (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_VMD_PARAM, &stMd, sizeof(stMd))))
	{
		printf("[%s %d] get motion detect param ok!\n", __func__, __LINE__);
	}
	else
	{
		printf("[%s %d] DEVSDK_ParamConfig: get md param err(%d)\n", __func__, __LINE__, nRet);
	}
	return 0;
}

int test_param_ima()
{
	int nRet = 0;
	GSF_IMA_CFG_S stIma = {0};
	if (0 == (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_IMA_PARAM, &stIma, sizeof(stIma))))
	{
		printf("----------ima-param-----------\n");
		printf("ima supp mask(%0#x): bright(%d) ldc-enable(%d) ldc-strengh(%d) mirror(%d) flip(%d)\n", 
			stIma.eSuppMask, stIma.struParam[VCT_IMA_IDX_BRIGHTNESS].u8Value, 
			(stIma.struParam[VCT_IMA_IDX_LDC_STRENGH].u8Value==0)? 0:1, stIma.struParam[VCT_IMA_IDX_LDC_STRENGH].u8Value, 
			stIma.struParam[VCT_IMA_IDX_MIRROR].u8Value, stIma.struParam[VCT_IMA_IDX_FLIP].u8Value);
		printf("\n----------------------------------------\n");
	}
	else
	{
		printf("[%s %d] get ima param err(%d)\n", __func__, __LINE__, nRet);
	}
	// set param
	stIma.eSetMask = 0;
	if (stIma.eSuppMask & VCT_IMA_BRIGHTNESS)
	{
		stIma.eSetMask |= VCT_IMA_BRIGHTNESS; // set bright mask
		stIma.struParam[VCT_IMA_IDX_BRIGHTNESS].u8Value += 10;
	}
	if (stIma.eSuppMask & VCT_IMA_LDC_STRENGH)
	{
		stIma.eSetMask |= VCT_IMA_LDC_STRENGH; // set ldc-strengh mask
		stIma.struParam[VCT_IMA_IDX_LDC_STRENGH].u8Value += 20;
	}
	if (stIma.eSuppMask & VCT_IMA_MIRROR)
	{
		stIma.eSetMask |= VCT_IMA_MIRROR; // set mirror
		stIma.struParam[VCT_IMA_IDX_MIRROR].u8Value = 1 - stIma.struParam[VCT_IMA_IDX_MIRROR].u8Value;
	}
	if (stIma.eSuppMask & VCT_IMA_FLIP)
	{
		stIma.eSetMask |= VCT_IMA_FLIP; // set flip
		stIma.struParam[VCT_IMA_IDX_FLIP].u8Value = 1 - stIma.struParam[VCT_IMA_IDX_FLIP].u8Value;
	}
	if (0 != (nRet = DEVSDK_ParamConfig(0, GSF_SET_PARAM_MSG, GSF_IMA_PARAM, &stIma, sizeof(stIma))))
	{
		printf("[%s %d] set ima bright and ldc-strengh err(%d)\n", __func__, __LINE__, nRet);
	}
	// get after set
	memset(&stIma, 0, sizeof(stIma));
	if (0 != (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_IMA_PARAM, &stIma, sizeof(stIma))))
	{
		printf("[%s %d] get ima bright and ldc-strengh err(%d)\n", __func__, __LINE__, nRet);
	}
	else
	{
		printf("ima: bright(%d) ldc-strengh(%d)\n", stIma.struParam[VCT_IMA_IDX_BRIGHTNESS].u8Value, stIma.struParam[VCT_IMA_IDX_LDC_STRENGH].u8Value);
	}
	return 0;
}

int test_param_3a()
{
	int nRet = 0;
	GSF_3A_CFG_S st3A = {0};
	if (0 == (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_3A_PARAM, &st3A, sizeof(st3A))))
	{
		printf("----------3A-param-----------\n");
		printf("3A supp mask(%0#x): wdr(%d) ae(%d) awb(%d) shutter(%d) af-mode(%d)\n", 
			st3A.eSuppMask, st3A.struParam[VCT_3A_IDX_WDR_STRENGH].u8Value, st3A.struParam[VCT_3A_IDX_AE].u8Value, 
			st3A.struParam[VCT_3A_IDX_AWB].u8Value, st3A.struParam[VCT_3A_IDX_SHUTTER].u8Value, 
			st3A.struParam[VCT_3A_IDX_AF_MODE].u8Value);
		printf("\n----------------------------------------\n");
	}
	else
	{
		printf("[%s %d] get 3A param err(%d)\n", __func__, __LINE__, nRet);
	}
	// set param
	st3A.eSetMask = 0;
	if (st3A.eSuppMask & VCT_3A_WDR_STRENGH)
	{
		st3A.eSetMask |= VCT_3A_WDR_STRENGH;
		st3A.struParam[VCT_3A_IDX_WDR_STRENGH].u8Value += 10;
	}
	if (st3A.eSuppMask & VCT_3A_AWB)
	{
		st3A.eSetMask |= VCT_3A_AWB;
		st3A.struParam[VCT_3A_IDX_AWB].u8Value = 1;
	}
	if (0 != (nRet = DEVSDK_ParamConfig(0, GSF_SET_PARAM_MSG, GSF_3A_PARAM, &st3A, sizeof(st3A))))
	{
		printf("[%s %d] set 3A param err(%d)\n", __func__, __LINE__, nRet);
	}
	// get after set
	memset(&st3A, 0, sizeof(st3A));
	if (0 != (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_3A_PARAM, &st3A, sizeof(st3A))))
	{
		printf("[%s %d] get 3A param err(%d)\n", __func__, __LINE__, nRet);
	}
	else
	{
		printf("3A: wdr-strengh(%d) awb(%d)\n", st3A.struParam[VCT_3A_IDX_WDR_STRENGH].u8Value, st3A.struParam[VCT_3A_IDX_AWB].u8Value);
	}
	return 0;
}

int test_param_temp()
{
	int nRet = 0;
	GSF_TEMP_STATE_S stTempState = {0};

	if (nRet = DEVSDK_ParamConfig(0, GSF_SYS_STATE_MSG, GSF_TEMP_STATE_CMD, &stTempState, sizeof(GSF_TEMP_STATE_S)))
	{
		printf("[%s %d] get temp status err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
	printf("----------temperature-param-----------\n");

	printf("[%s %d]temperature=%d \n", __func__, __LINE__,stTempState.nTemperature[0]);
	
	printf("\n----------------------------------------\n");
	
	return 0;
}

int test_param_PD_rect()
{

	PDRectHdl *pHdl = DEVSDK_StartGetPDRectInfo();

	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetPDRectInfo(pHdl);
	}
	else
	{
		printf("DEVSDK_StartGetPDRectInfo err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	
	return 0;
}


int test_param_dayNight()
{
	GSF_DAYNIGHT_CFG_S stDayNight = {0};
    int nRet = -1;

    if (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_IRCUT_PARAM, &stDayNight, sizeof(GSF_DAYNIGHT_CFG_S)))
	{
		printf("[%s %d] get DayNight param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
		
    stDayNight.u8FilterType = 1;


	if (nRet = DEVSDK_ParamConfig(0, GSF_SET_PARAM_MSG, GSF_IRCUT_PARAM, &stDayNight, sizeof(GSF_DAYNIGHT_CFG_S)))
	{
		printf("[%s %d] set DayNight param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}

	return 0;
}

int test_param_user()
{
	int i = 0;
	GSF_USER_EXT_CFG_S    stUsrCfg = {0};
    int nRet = -1;

    if (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_USER_EXT_PARAM, &stUsrCfg, sizeof(GSF_USER_EXT_CFG_S)))
	{
		printf("[%s %d] get User param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}

	printf("[%s %d] UserNum(%d)\n", __func__, __LINE__, stUsrCfg.u32Num);
	
    for (i = 0; i < GSF_MAX_USR_EXT_NUM && i < stUsrCfg.u32Num; ++i)
	{
		printf("[%s %d]i(%d) szUsrName(%s) PassWord(%s)\n", __func__, __LINE__,
				i, stUsrCfg.stUsers[i].szUsrName,stUsrCfg.stUsers[i].szPsw);
	}
	

	if (nRet = DEVSDK_ParamConfig(0, GSF_SET_PARAM_MSG, GSF_USER_EXT_PARAM, &stUsrCfg, sizeof(GSF_USER_EXT_CFG_S)))
	{
		printf("[%s %d] set User param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
	
	return 0;
}

//网卡0为eth，1为WiFi
int test_param_net()
{
	GSF_NET_CFG_S stNetCfg = {0};
    int nRet = -1;

    if (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_NET_PARAM, &stNetCfg, sizeof(GSF_NET_CFG_S)))
	{
		printf("[%s %d] get Net param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
		
    printf("[%s %d]u32IpAddr(%x) u32GateWay(%x)\n", __func__, __LINE__, stNetCfg.struEtherCfg[0].u32IpAddr,stNetCfg.struEtherCfg[0].u32GateWay);

	if (nRet = DEVSDK_ParamConfig(0, GSF_SET_PARAM_MSG, GSF_NET_PARAM, &stNetCfg, sizeof(GSF_NET_CFG_S)))
	{
		printf("[%s %d] set Net param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
	
	return 0;
}

int test_param_cam_pos()
{
	GSF_CAM_POS_INFO    stCamPos = {0};
    int nRet = -1;


    if (nRet = DEVSDK_ParamConfig(0, GSF_GET_SYS_INFO, GSF_CAM_POS, &stCamPos, sizeof(GSF_CAM_POS_INFO)))
	{
		printf("[%s %d] get cam Pos err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
		
    printf("[%s %d]u16ZoomPos(%d) u16FocusPos(%d) u16IrisPos(%d)\n", __func__, __LINE__, 
    		stCamPos.u16ZoomPos,stCamPos.u16FocusPos,stCamPos.u16IrisPos);
    

	if (nRet = DEVSDK_ParamConfig(0, GSF_SET_SYS_INFO, GSF_CAM_POS, &stCamPos, sizeof(GSF_CAM_POS_INFO)))
	{
		printf("[%s %d] set cam Pos err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
	
	return 0;
}

int test_param_roi()
{
	GSF_ROI_CFG_S    stRoi = {0};
    int nRet = -1;

    if (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_ROI_PARAM, &stRoi, sizeof(GSF_ROI_CFG_S)))
	{
		printf("[%s %d] get roi param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}

	for(int i = 0;i < GSF_ROI_REG_NUM; i++)
	{
		printf("[%s %d]s32X(%d) s32Y(%d) u32Height(%d) u32Width(%d)\n", __func__, __LINE__, 
    		stRoi.stRect[i].s32X,stRoi.stRect[i].s32Y,stRoi.stRect[i].u32Height,stRoi.stRect[i].u32Width);
	}

	stRoi.u8Enable = 0;

	if (nRet = DEVSDK_ParamConfig(0, GSF_SET_PARAM_MSG, GSF_ROI_PARAM, &stRoi, sizeof(GSF_ROI_CFG_S)))
	{
		printf("[%s %d] set roi param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
	
	return 0;
}

int test_param_alarmout()
{
	GSF_ALARMOUT_CTRL_S    stAlarmOut = {0};
    int nRet = -1;


	stAlarmOut.u32SetManl = 1;
	stAlarmOut.u32ManlStatus = 1;

	if (nRet = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_ALARM_OUT_CMD, &stAlarmOut, sizeof(GSF_ALARMOUT_CTRL_S)))
	{
		printf("[%s %d] set AlarmOut param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
	
	return 0;
}


int test_param()
{
	//test_param_md();
	//test_param_ima();
	//test_param_3a();
	//test_param_temp();
	test_param_user();
	test_param_net();
}

int test_ptz()
{
	int ret = 0;
	GSF_PTZ_CMD_S stPtz = {0};
	// up
	stPtz.cmd = PTZ_CMD_UP;
	stPtz.data1 = stPtz.data2 = 50;
	printf("[%s %d] ptz ctrl: up start ..\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
	{
		printf("[%s %d] ptz up ctrl err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	}
	sleep(1); // 预留云台动作时间
	stPtz.cmd = PTZ_CMD_STOP;
	printf("[%s %d] ptz ctrl: up stop\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz up ctrl stop err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));

	// down
	stPtz.cmd = PTZ_CMD_DOWN;
	printf("[%s %d] ptz ctrl: down start ..\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz down ctrl err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	sleep(1);
	stPtz.cmd = PTZ_CMD_STOP;
	printf("[%s %d] ptz ctrl: down stop\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz down ctrl stop err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));

	// zoom+
	stPtz.cmd = PTZ_CMD_ZOOM_WIDE; // PTZ_CMD_FOCUS_NEAR
	printf("[%s %d] ptz ctrl: zoom+ start ..\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz zoom+ ctrl err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	sleep(1); // 预留变倍动作时间
	stPtz.cmd = PTZ_CMD_STOP;
	printf("[%s %d] ptz ctrl: zoom+ stop\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz zoom+ ctrl stop err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	
	// zoom-
	stPtz.cmd = PTZ_CMD_ZOOM_TELE; // PTZ_CMD_FOCUS_FAR
	printf("[%s %d] ptz ctrl: zoom- start ..\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz zoom- ctrl err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	sleep(1);
	stPtz.cmd = PTZ_CMD_STOP;
	printf("[%s %d] ptz ctrl: zoom- stop\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz zoom- ctrl stop err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	
	// iris+
	stPtz.cmd = PTZ_CMD_IRIS_OPEN;
	printf("[%s %d] ptz ctrl: iris+ start ..\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz iris+ ctrl err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	sleep(1);
	stPtz.cmd = PTZ_CMD_STOP;
	printf("[%s %d] ptz ctrl: iris+ stop\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz iris+ ctrl stop err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	
	// iris-
	stPtz.cmd = PTZ_CMD_IRIS_CLOSE;
	printf("[%s %d] ptz ctrl: iris- start ..\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz iris- ctrl err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	sleep(1);
	stPtz.cmd = PTZ_CMD_STOP;
	printf("[%s %d] ptz ctrl: iris- stop\n", __func__, __LINE__);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz iris- ctrl stop err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));

	// goto preset 1
	stPtz.cmd = PTZ_CMD_GOTO_PRESET;
	stPtz.data2 = 1; // preset no
	printf("[%s %d] ptz ctrl: goto preset %d\n", __func__, __LINE__, stPtz.data2);
	if ((ret = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_PTZ_CMD, &stPtz, sizeof(stPtz))) != 0)
		printf("[%s %d] ptz goto preset ctrl err ret(%d) msg(%s)\n", __func__, __LINE__, ret, DEVSDK_GetLastErrorMsg(NULL));
	return 0;
}

int test_vpss_crop()
{
	int nRet = -1;
	int nVpssGrpNo = 0;
	int nEnable = 0;
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	
	if(0 != (nRet = DEVSDK_Vpss_GetCrop(nVpssGrpNo, &nEnable, &x, &y, &width, &height)))
	{
		printf("[%s %d]get crop err!\n",__func__,__LINE__);
	}else
	{
		printf("[%s %d]nVpssGrpNo(%d) nEnable(%d) x(%d) y(%d) width(%d) height(%d)\n",__func__,__LINE__
				,nVpssGrpNo,nEnable,x,y,width,height);
	}


	if(0 != (nRet = DEVSDK_Vpss_SetCrop(0 , 1, 0, 0, 1280, 720)))
	{
		printf("[%s %d]set crop err!\n",__func__,__LINE__);
	}
	
}

int test_get_UniqueId()
{
	hi_unique_id unique_id = {0};
	
	DEVSDK_Get_UniqueId(&unique_id);

	for(int i = 0; i < 6; i++)
	{
		printf("Unique id is %#x\n", unique_id.id[i]);
	}
	
	return 0;
}

int get_param_ver()
{
	int nRet = 0;
	GSF_DEV_CFG_S stVer = {0};
	if (0 != (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_VER_PARAM, &stVer, sizeof(stVer))))
	{
		printf("[%s %d] get version param err(%d) msg(%s)\n", __func__, __LINE__, nRet, DEVSDK_GetLastErrorMsg(NULL));
	}
	else
	{
		printf("[%s %d] device version: \n\tserialId(%s) \n\tdevType(%d) \n\tsensorType(%d) \n\tchipType(%0#x) \n\tSoftVer(%d.%d.%d)\n\n", __func__, __LINE__, 
			stVer.u8SerialId, stVer.u16DevType, stVer.u32SensorType[0], stVer.u32ChipType, 
			(stVer.u32SoftVer>>24)&0xff, (stVer.u32SoftVer>>16)&0xff, (stVer.u32SoftVer>>8)&0xff);
	}
	return 0;
}

int realavEncInfoCallback(int nStatus, DEVSDK_AV_ENC_INFO_S *pstAVEncInfo, void *pUser)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		return 0;
	}
	if ((long)pUser != AV_ENC_INFO_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		return 0;
	}
	if (pstAVEncInfo)
	{
		printf("[%s %d] stream(%d) frmno(%d) utc(%u) pts(%llu)\n", __func__, __LINE__, 
			pstAVEncInfo->u8Stream, pstAVEncInfo->u32FrameNo, pstAVEncInfo->u32Time, pstAVEncInfo->u64TimeStamp);
	}
	else
	{
		printf("[%s %d] ERR: invalid arg: %p\n", __func__, __LINE__, pstAVEncInfo);
	}
	return 0;
}

int test_realav_enc_info()
{
	YuvHdl *pHdl = DEVSDK_StartGetRealAVEncInfo(0, realavEncInfoCallback, (void *)AV_ENC_INFO_USER);
	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetRealAVEncInfo(pHdl);
	}
	else
	{
		printf("DEVSDK_StartGetRealAVEncInfo err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	return 0;
}

int rawSensorDataCallback(int nStatus, DEVSDK_AV_PIPE_FRAME_INFO *pViPipeFrame, unsigned char *pData, void *pUser)
{
	if (nStatus != 0)
	{
		printf("[%s %d] some err status(%d) msg(%s)\n", __func__, __LINE__, nStatus, DEVSDK_GetLastErrorMsg(NULL));
		g_exit = 1;
		return 0;
	}
	
	if ((long)pUser != RAWSENSOR_USER)
	{
		printf("[%s %d] ERROR: pUser(%p)\n", __func__, __LINE__, pUser);
		g_exit = 1;
		return 0;
	}

	printf("[%s %d]u32Width(%d) u32Height(%d) u32FrameSize(%d) u32PixelFormat(%d) u32CompressMode(%d) u32Stride(%d) u64Pts(%llu)\n",__func__,__LINE__,
			pViPipeFrame->u32Width,pViPipeFrame->u32Height,pViPipeFrame->u32FrameSize,
			pViPipeFrame->u32PixelFormat,pViPipeFrame->u32CompressMode,pViPipeFrame->u32Stride,pViPipeFrame->u64Pts);

	return 0;
}

int test_RawSensor()
{
	int nBitWidth = GSF_IPC_CMD_BITWIDTH_FORMAT_12;
	int nCompressMode = GSF_IPC_CMD_COMPRESS_MODE_NONE;
	
	RawSensorHdl *pHdl = DEVSDK_StartGetRawSensorData(nBitWidth, nCompressMode, rawSensorDataCallback, (void *)RAWSENSOR_USER);
	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetRawSensorData(pHdl);
	}
	else
	{
		printf("DEVSDK_StartGetRawSensorData err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
	return 0;
}

int test_CreateVencChnToShoot()
{	
	int nVpssChn = 0;
	int nWidth = 2592;
	int nHeight = 1944;
	GSF_RECT_S stRect = {0};
	int nRet= -1;
	stRect.s32X = 0;
	stRect.s32Y = 0;
	stRect.u32Width = 0;
	stRect.u32Height = 0;
	GSF_U32 nFileSize = 0;
	nRet = DEVSDK_StartVencCapture(nVpssChn, &stRect, nWidth, nHeight, &nFileSize, NULL);
	if(nRet != 0)
	{
		printf("DEVSDK_StartVencCapture err!\n");
		return -1;
	}
	while(0 == g_exit) // 确保主进程不退出
	{	
		nRet = DEVSDK_VencCaptureRecv(nVpssChn,&stRect, &nFileSize, nWidth, nHeight);		
		if(nRet != 0)
		{
			printf("DEVSDK_VencCaptureRecv err!\n");
			break;
		}
		sleep(1);
	}
	DEVSDK_StopVencCapture(nVpssChn);	
}

int test_tempOsd(int chn)
{
	int nRet;
	GSF_OSD_CFG_S stOsd = {0};
	DEVSDK_ParamConfig(chn,GSF_GET_SYS_INFO,GSF_TEMP_OSD_PARAM,&stOsd,sizeof(GSF_OSD_CFG_S)); 
	printf("%s\n",stOsd.szChnName);
	stOsd.u8TextAlign=0;
	stOsd.u8EnableBmpOsd=0;
	stOsd.u8EnableUsrOsd=0;
	stOsd.u8EnableUsrOsd |= 1<<3;
	stOsd.struUsrOrg[3].s32X=200;
	stOsd.struUsrOrg[3].s32Y=200;
	char tempbuf[32]={0xE4,0xBD,0xA0,0xE5,0xA5,0xBD};//需要转换成UTF8传过去
	strcpy(stOsd.szUsrOsdInfo[3],tempbuf);
	strcpy(stOsd.szChnName,tempbuf);
	stOsd.u8EnableTitleOsd=1;		//Title也是一个osd，用户可以定义
	stOsd.struTitleOrg.s32X=500;
	stOsd.struTitleOrg.s32Y=600;
	DEVSDK_ParamConfig(chn,GSF_SET_SYS_INFO,GSF_TEMP_OSD_PARAM,&stOsd,sizeof(GSF_OSD_CFG_S));
}


//PtzMsgCallbackFunc 
int ptzMsgCallback(int cmd,int addr,int data1,int data2,int funcNo)
{
	switch(cmd)
	{
		case PTZ_CMD_SET_PRESET :
			break;
		case PTZ_CMD_CLR_PRESET :
			break;
		case PTZ_CMD_GOTO_PRESET:
			break;
		default :
			break;
	}
	printf("cmd[%s]speed[%d]PresetNo[%d]\n",cmd==PTZ_CMD_SET_PRESET?"SET_PRESET":cmd==PTZ_CMD_CLR_PRESET?"CLR_PRESET":"GOTO_PRESET"
	,data1,data2);
}


int test_PTZ_PRESET()
{
	GSF_PTZ_NOTIFY_MSG_HDL *PHdl = DEVSDK_StartGetPtz(0,ptzMsgCallback,(void *)PTZ_USER);
	if (NULL != PHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetptz(PHdl);
	}
	else
	{
		printf("DEVSDK_StartGetPtz err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}
}

int test_SET_TIMEZONE()
{
	int nRet=-1;
	GSF_NTP_CFG_S stNtpCfg = {0};
	 if (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_NTP_PARAM, &stNtpCfg, sizeof(GSF_NTP_CFG_S)))
	{
		printf("[%s %d] get DayNight param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
	//GMT+6:30：仰光 23400=6.5*3600
    stNtpCfg.nTimeZone = 23400;

	if (nRet = DEVSDK_ParamConfig(0, GSF_SET_PARAM_MSG, GSF_NTP_PARAM, &stNtpCfg, sizeof(GSF_NTP_CFG_S)))
	{
		printf("[%s %d] set DayNight param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
}

static int __random_ai_data(DEVSDK_AI_DATA_S *pAiData)
{
	static int nIdx = 0;
	static int nAiTargetNum[8] = {2, 1, 8, 12, 9, 6, 16, 15};
	static DEVSDK_AI_TARGET_S stAiTarget[16] = {
		{0.90, {100, 100}, {200, 160}, "AAAAAAAAAAAAAAAAAAAAA"},
		{0.99, {100, 200}, {320, 280}, "BBBBBBBBBBBBBBBBBBB"},
		{0.95, {100, 300}, {200, 360}, "CCCCCCCCCCCCCCCCCCCC"},
		{0.92, {100, 400}, {200, 460}, "DDDDDDDDDDDDDDDD"},
		{0.88, {100, 500}, {200, 560}, "EEEEEEEEEEEEEEEEEEEEEEEEEE"},
		{0.76, {200, 100}, {300, 260}, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"},
		{0.68, {200, 200}, {300, 360}, "GGGGGGGGGGGGGGGGGGGGGGGGGG"},
		{0.72, {200, 300}, {300, 460}, "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"},
		{0.91, {200, 400}, {300, 560}, "IIIIIIIIIIIIIIII"},
		{0.86, {200, 500}, {300, 560}, "JJJJJJJJJJJJJJJJJJJJJ"},
		{0.77, {300, 500}, {400, 560}, "KKKKKKKKKKKKK"},
		{0.93, {300, 400}, {400, 560}, "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL"},
		{0.89, {300, 200}, {400, 360}, "MMMMMMMMMMMMMMMM"},
		{0.79, {400, 200}, {500, 460}, "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"},
		{0.82, {400, 100}, {500, 260}, "OOOOOOOOOOOOOOOOOOOOOOOOOOOO"},
		{0.93, {500, 300}, {600, 460}, "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPppppppppppppppppppppppppppppppppp"},
	};
	pAiData->ref_w = 640;
	pAiData->ref_h = 640;
	pAiData->fps = 10;
	pAiData->seq = nIdx;
	pAiData->target_num = nAiTargetNum[nIdx%8];
	pAiData->target = &stAiTarget[0];
	printf("[%s %d] idx(%d) target num(%d)\n", __func__, __LINE__, nIdx, pAiData->target_num);
	nIdx++;
	return 0;
}

int test_pushData()
{
	PushDataHdl hdl = DEVSDK_OpenPushData();
	if (hdl)
	{
		DEVSDK_AI_DATA_S stAiData = {0};
		while(0 == g_exit)
		{
			__random_ai_data(&stAiData);
			DEVSDK_PushData(hdl, &stAiData);
			usleep(500000);
		}
		DEVSDK_ClosePushData(hdl);
	}
	else
	{
		printf("[%s %d] OpenPushData err!!\n", __func__, __LINE__);
	}
	return 0;
}


int test_yuvOsdPtsProc()
{
	int nRotate = 0;

	if(0 != DEVSDK_YuvDataProcPrepare( 0, 0, 0, 1,nRotate))
	{
		printf("DEVSDK_YuvDataProcPrepare err!!!\n");
		return -1;
	}
	YuvHdl *pHdl = DEVSDK_SetYuvOsdProc( 0, 0, 0, 1, yuvDataCallback, (void *)YUV_USER);
	if (NULL != pHdl)
	{
		while(0 == g_exit) // 确保主进程不退出
		{
			sleep(1);
		}
		DEVSDK_StopGetYUVDataProc(pHdl);
	}
	else
	{
		printf("DEVSDK_StartGetYUV err(%s)\n", DEVSDK_GetLastErrorMsg(NULL));
	}

}

int test_upgrade()
{
	int nRet=-1;
	GSF_UPGRADE_CMD_S stUpgCmd = {0};
	stUpgCmd.u8UpdType = 0;
	strncpy(stUpgCmd.szFilePath, "/tmp/3519dv5_emmc_v4.2.117.bin", GSF_PATH_LEN);
	if (nRet = DEVSDK_ParamConfig(0, GSF_SYS_CTRL_MSG, GSF_UPGRADE_CMD, &stUpgCmd, sizeof(GSF_UPGRADE_CMD_S)))
	{
		printf("[%s %d] upgrade err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
}

int test_dev_upg()
{
	int nStatus = 0; 
	char szFileName[] = "/tmp/upg.bin";
	if ((nStatus = DEVSDK_DeviceUpgrade(szFileName)) == 0)
	{
		int nTimeout = 0;	// 超过 10 分钟还没升级完成，说明升级异常导致某个地方阻塞了
		nStatus = DEVSDK_GetDevUpgradeStatus();
		while(g_exit == 0 && nTimeout < 600 && nStatus >= 0 && nStatus < 100) // 0-99 为正常进度
		{
			printf("upg status(%d) timeout(%d) cur-time(%d)\n", nStatus, nTimeout, time(NULL));
			sleep(1);
			nTimeout += 1;
			nStatus = DEVSDK_GetDevUpgradeStatus();
		}
		if (nStatus != 100)
		{
			printf("upg status(%d) timeout(%d) cur-time(%u) err\n", nStatus, nTimeout, time(NULL));
		}
		else
		{
			printf("upg finish! status(%d) ok.\n", nStatus);
		}
	}
	else
	{
		int nErr = DEVSDK_GetDevUpgradeStatus();
		printf("dev upg err(%d) code(%d)\n", nStatus, nErr);
	}
}



APP_CTRL_S ctrlFunc[] = 
{
	{GSF_APP_CRTL_CMD_PTZ, 			test_ptz},
	{GSF_APP_CRTL_CMD_PIC,			test_capture},
	{GSF_APP_CTRL_CMD_PIC_V2,		test_capture_v2},
	{GSF_APP_CTRL_CMD_PIC_DATA, 	test_captureData},
	{GSF_APP_CTRL_CMD_VIDEO,		test_video},
	{GSF_APP_CTRL_CMD_VIDEO_V2,		test_video_v2},
	{GSF_APP_CTRL_CMD_REC,			test_rec},
	{GSF_APP_CTRL_CMD_REC_FILE,		test_recfile},
	{GSF_APP_CTRL_CMD_YUV,			test_yuv},
	{GSF_APP_CTRL_CMD_YUV_PROC,		test_yuvDataProc},
	{GSF_APP_CTRL_CMD_YUV_PROC_V2,	test_yuvDataProcV2},
	{GSF_APP_CTRL_CMD_REAL_AV,		test_realAV},
	{GSF_APP_CTRL_CMD_BGR, 			test_bgr},
	{GSF_APP_CTRL_CMD_AUDIO,		test_play_audio},
	{GSF_APP_CTRL_CMD_ALARM,		test_alarm},
	{GSF_APP_CTRL_CMD_COMM_485,		test_comm485},
	{GSF_APP_CTRL_CMD_MD,			test_param_md},
	{GSF_APP_CTRL_CMD_IMA,			test_param_ima},
	{GSF_APP_CTRL_CMD_3A,			test_param_3a},
	{GSF_APP_CTRL_CMD_TEMP,			test_param_temp},
	{GSF_APP_CTRL_CMD_PD,			test_param_PD_rect},
	{GSF_APP_CTRL_CMD_DAY_NIGHT,	test_param_dayNight},
	{GSF_APP_CTRL_CMD_USER,			test_param_user},
	{GSF_APP_CTRL_CMD_NET,			test_param_net},
	{GSF_APP_CTRL_CMD_CAMPOS,		test_param_cam_pos},
	{GSF_APP_CTRL_CMD_ROI,			test_param_roi},
	{GSF_APP_CTRL_CMD_ALARMOUT,		test_param_alarmout},
	{GSF_APP_CTRL_CMD_VPSS_CROP,	test_vpss_crop},
	{GSF_APP_CTRL_CMD_UNIQUEID,		test_get_UniqueId},
	{GSF_APP_CTRL_CMD_VER,			get_param_ver},
	{GSF_APP_CTRL_CMD_REAL_AV_ENC,	test_realav_enc_info},
	{GSF_APP_CTRL_CMD_RAW_DATA,		test_RawSensor},
	{GSF_APP_CTRL_CMD_VENC_SHOOT,	test_CreateVencChnToShoot},	
	{GSF_APP_CTRL_CMD_YUV_OSD_TIME , test_yuvOsdPtsProc},
	{GSF_APP_CTRL_CMD_PUSH_DATA,	test_pushData},	
};



int main(int argc, char **argv)
{
	main_signal();
	printf("[%s %d] devsdk version(%s)\n\n", __func__, __LINE__, DEVSDK_GetVersion());

	DEVSDK_Init();


#ifdef GSF_APP_TEST
	int nStatus = -1;
	printf("[%s %d] Exit.\n", __func__, __LINE__);

	for(int index = 0; index < sizeof(ctrlFunc)/sizeof(APP_CTRL_S); ++index)
	{
		if(argc == 1)
		{
			printf("[%s %d]DEVSDK Supported operations:%s\n", __func__, __LINE__,ctrlFunc[index].szCmd);
		}
		else if((strcmp(ctrlFunc[index].szCmd, argv[1]) == 0) && ctrlFunc[index].func)
		{
			printf("[%s %d] beg test index: %d(%s)\n", __func__, __LINE__, index, ctrlFunc[index].szCmd);
			ctrlFunc[index].func();
			printf("[%s %d] end test index: %d(%s)\n", __func__, __LINE__, index, ctrlFunc[index].szCmd);
		}
	}	
#endif 	
	//test_video();
	//test_upgrade();
	//test_bgr();
	//get_param_ver();
	//test_tempOsd(0);
	//test_param_dayNight();
	//test_yuvOsdPtsProc();
	//test_yuvDataProc();
	//test_comm485();
	//test_captureData();
	//test_recfile();
	//test_capture();
	//test_capture_v2();
	//test_video();
	//test_video_v2();
	//test_realAV();
	// test_rec();
	//test_yuv();
	//test_bgr();
	//test_play_audio(argc, argv);
	//test_alarm();
	//test_param();
	//test_ptz();
	//test_realav_enc_info();
	//test_vpss_crop();
	//test_get_UniqueId();
	//test_param_cam_pos();
	//test_param_roi();
	//test_param_PD_rect();
	//test_RawSensor();
	//test_param_alarmout();
	//test_CreateVencChnToShoot();
	//test_PTZ_PRESET();
	//test_SET_TIMEZONE();
	while(g_exit == 0 && g_video_filename[0] != '\0')
	{
		sleep(1);
	}

	DEVSDK_ReleaseFile(g_video_filename);
	DEVSDK_Uninit();
	printf("[%s %d] Exit.\n", __func__, __LINE__);
}
