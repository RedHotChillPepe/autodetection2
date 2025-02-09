#ifndef __GSF_VPSS_YUV_H__
#define __GSF_VPSS_YUV_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#ifdef GSF_VPSS_YUV_SUPP
#include <stdio.h>
#include "hi_math.h"
#include "devsdk.h"


#define GSF_OSD_DATE_NUM 6
#define GSF_OSD_TIME_NUM 2

#define GSF_VPSS_YUV_MAX_AI_DATA  50 // Максималькое кол-во детекций

/* Config */
typedef struct _GSF_VPSS_YUV_HDL
{
	YUVDataCallbackFunc fCallback;
	void *pUser;
	int nExit;
	unsigned char *pBuf;
	int nBufSize;
	pthread_t thid;
	int nVpssGrp;
	int nVpssChn;
	int nDepth;
	int nWidth; 
	int nHeight;
	int yuvDataProcFlag;
	int userMsg;
	hi_video_frame_info stFrame;
	
	
	/////////////////////////////НОВОЕ/////////////////////////////////
	
    /* ===== поля для списка прямоугольников с текстом ===== */
    AI_DATA_INFO_S stAiData[GSF_VPSS_YUV_MAX_AI_DATA]; /* coords + text */
    int nAiDataNum;  /* сколько реально данных заполнено */
	
	/////////////////////////////НОВОЕ/////////////////////////////////
	
}GSF_VPSS_YUV_HDL;

typedef struct _GSF_VPSS_BGR_HDL
{
	BGRDataCallbackFunc fCallback;
	void *pUser;
	int nExit;
	unsigned char *pBuf;
	int nBufSize;
	pthread_t thid;
	int nVpssGrp;
	int nVpssChn;
	int nDepth;
	int nWidth; 
	int nHeight;
	int nRes[2];
	hi_video_frame_info stFrame;
}GSF_VPSS_BGR_HDL;
int get_yuv_type(int enPixelFormat);

void *gsf_vpss_yuv_task(void *arg);
int gsf_vpss_yuv_stop(void *hdl);

void *gsf_vpss_yuv_osd_task(void *arg);


void *gsf_vpss_bgr_task(void *arg);
int gsf_vpss_bgr_stop(void *hdl);

int EnableVpssVencChn(int VpssVencChn);

int yuvDataProcPrepare(int VpssVencChn, int nWidth, int nHeight,int userMsg,int nRotate);


int save_yuv_data_from_video_frame(hi_video_frame_info *pstVideo);

int VB_Restore(int nSize);
int gsf_release_vb_before_start();

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
