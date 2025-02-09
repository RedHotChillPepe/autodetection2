/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : devsdk.h

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/06/23
  Description   :
  History       :
  1.Date        : 2021/06/23
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __L_DEVSDK_H__
#define __L_DEVSDK_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>
#include "comm485.h"
#include "gsf_hi_sys_info.h"

/* Config */
#define DEVSDK_API

#define DEVSDK_CHN_NUM 4
#define DEVSDK_NAME_LEN 32
#define DEVSDK_SERIALID_LEN 32
#define DEVSDK_PATH_LEN 128


/////////////////////////////����������/////////////////////////////////
#ifndef GSF_VPSS_YUV_MAX_AI_DATA
#define GSF_VPSS_YUV_MAX_AI_DATA 50 // ���ѧܧ� ��ڧ�ݧ� ��֧�֧էѧӧѧ֧ާާ�� ����ާ��ԧ�ݧ�ߧڧܧ��
#endif

/* ����ݧ� �ܧѧܧѧ� ��� ����ҧݧ֧ާ� ������ާ� ��ҧ��ӧڧ� ���� */
typedef void* YuvHdl;

/* 
 * ������ܧ���� "����ާ��ԧ�ݧ�ߧڧ� + ��֧ܧ��"
 */
 
typedef struct _AI_DATA_INFO_S {  // �� �٧ѧާ֧ߧڧ� ���էߧ�� �����ܧ����, �ާ�ا֧� �ܧѧܧڧ� ��� �է��ԧڧ� ���ߧܧ�ڧ� �ܧ�ѧ�ߧ����, ���ݧ�ܧ� �էݧ� ���ڧާ֧��
    struct {
        int x;
        int y;
        int width;
        int height;
    } rect;
    char szText[64];
} AI_DATA_INFO_S;



/////////////////////////////����������/////////////////////////////////




typedef enum
{
	DEVSDK_OK = 0,					//OK
	DEVSDK_ERR_NOINIT,				//û�г�ʼ��SDK
	DEVSDK_ERR_INIT,				//��ʼ��SDK����
	DEVSDK_ERR_CONN,				//���ӷ����ʧ��
	DEVSDK_ERR_PARAM,				//��������
	DEVSDK_ERR_CMD,					//�������
	DEVSDK_ERR_ALLOC,				//������Դʧ��
	DEVSDK_ERR_SCALE,				//vgs����ʧ��
	DEVSDK_ERR_MMAP,				//Mmap��������
	DEVSDK_ERR_CONVERT,				//YUVתBGRʧ��
	DEVSDK_ERR_FILE_NOEXIST,		//�ļ�������
	DEVSDK_ERR_THREAD,				//�����̴߳���
	DEVSDK_ERR_MSG,					//���ͻ��߽�����Ϣ����
	DEVSDK_ERR_NOUNBIND,			//û�н���Ӧͨ��
	DEVSDK_ERR_CREATECHNFAIL,		//����ͨ��ʧ��
	DEVSDK_ERR_JPEGPREPAREERR,		//jpeg׼��ʧ��
	DEVSDK_ERR_GETORSETPARAMERR,	//���û��߻�ȡ����ʧ��
	DEVSDK_ERR_485COMMPREPAREERR,	//485������������ʧ��
	DEVSDK_ERR_485READERR,			//485��ȡ���ݴ���
	DEVSDK_ERR_485WRITEERR,			//485д���ݴ���
	DEVSDK_ERR_VPSSSETGROPERR,		//����vpss grop���Դ���
	DEVSDK_ERR_NOTSUPP,				//��֧�ֵ�����
	DEVSDK_ERR_SEND,				//��������ʧ��
	DEVSDK_ERR_RECV,				//��������ʧ��
	DEVSDK_ERR_UNKNOWN = 255,		//δ֪����
}DEVSDK_RET;

typedef enum _DEVSDK_REC_TYPE_E
{
	DEVSDK_REC_TYPE_TIME	= 0x1,		// 0 ��ʱ¼��
	DEVSDK_REC_TYPE_MD		= 0x2,		// 1 �ƶ����¼��
	DEVSDK_REC_TYPE_IO		= 0x4,		// 2 IO����¼��
	DEVSDK_REC_TYPE_MANUAL	= 0x8,		// 3 �ֶ�¼��
	DEVSDK_REC_TYPE_OD 		= 0x10,		// 4 ��Ƶ�ڵ�¼��
	DEVSDK_REC_TYPE_NET_FAULT	= 0x20,	// 5 �������¼��
	DEVSDK_REC_TYPE_PIR		= 0x40,		// 6 ��������Ӧ¼��
	DEVSDK_REC_TYPE_FTP_UNDONE	= 0x80,	// 7 FTPδ�ϴ��ɹ���ʹ��FTP�ش�ʱʹ��
	DEVSDK_REC_TYPE_GSENSOR	= 0x100,	// 8 GSensor �Զ�¼��,���Զ�����
	DEVSDK_REC_TYPE_RO		= 0x200,	// 9 ֻ��¼��,���Զ�����
	DEVSDK_REC_TYPE_FD 		= 0x400,	// 10 �������¼��, ���ڱ������Ͷ�Ӧ
	DEVSDK_REC_TYPE_RES		= 0x800,	// 11 ����
	DEVSDK_REC_TYPE_AUDIO	= 0x1000,	// 12 ������ⱨ��¼��
	DEVSDK_REC_TYPE_LSC		= 0x2000,	// 13 �����쳣ͣ������¼��	
	DEVSDK_REC_TYPE_AVD		= 0x4000,	// 14 
	DEVSDK_REC_TYPE_PEAOSC	= 0x8000,	// 15 
	DEVSDK_REC_TYPE_CPC		= 0x10000,	// 16 
	DEVSDK_REC_TYPE_SMOKE	= 0x20000,	// 17 
	DEVSDK_REC_TYPE_LAO		= 0x40000,	// 18 �����쳣��������¼��
	DEVSDK_REC_TYPE_LAC		= 0x80000,	// 19 �����쳣�رձ���¼��
	DEVSDK_REC_TYPE_LAR		= 0x100000,	// 20 �����쳣���ݱ���¼��
	DEVSDK_REC_TYPE_LM  	= 0x200000, // 21 ���ݼ��ޱ���¼��
	DEVSDK_REC_TYPE_LF    	= 0x400000, // 22 ���ݹ��ϱ���¼��
	DEVSDK_REC_TYPE_EBIKE	= 0x800000,	// 23 �綯������¼��
	DEVSDK_REC_TYPE_ALL		= 0xFFFFFFFF,
}DEVSDK_REC_TYPE_E;

// ------------------------------------------- //

// EncodeType
typedef enum _DEVSDK_VENC_TYPE_E_
{
	DEVSDK_VENC_TYPE_H264	= 0x0,
	DEVSDK_VENC_TYPE_MJPEG 	= 0x1,
	DEVSDK_VENC_TYPE_JPEG  	= 0x2,
	DEVSDK_VENC_TYPE_MPEG4 	= 0x3,
	DEVSDK_VENC_TYPE_H265	= 0x4,
	DEVSDK_VENC_TYPE_BUTT
}DEVSDK_VENC_TYPE_E;

//��Ƶ��������
typedef enum _DEVSDK_AENC_TYPE_E_
{
    DEVSDK_AENC_TYPE_PCM    = 0,    //PCM
    DEVSDK_AENC_TYPE_G711A,
    DEVSDK_AENC_TYPE_G711U,
    DEVSDK_AENC_TYPE_G726,
    DEVSDK_AENC_TYPE_AACLC,
    DEVSDK_AENC_TYPE_EAAC,
    DEVSDK_AENC_TYPE_EAACPLUS,
    DEVSDK_AENC_TYPE_AMR_WB,
    DEVSDK_AENC_TYPE_BUTT
}DEVSDK_AENC_TYPE_E;

// FrameType
typedef enum _DEVSDK_FRAME_TYPE_E_
{
	DEVSDK_FRAME_TYPE_RES0    = 0x0,
	DEVSDK_FRAME_TYPE_RES1    = 0x1,
	DEVSDK_FRAME_TYPE_RES2    = 0x2,
	DEVSDK_FRAME_TYPE_RES3,
	DEVSDK_FRAME_TYPE_RES4,
	DEVSDK_FRAME_TYPE_RES5,
	DEVSDK_FRAME_TYPE_RES6,
	DEVSDK_FRAME_TYPE_VIDEO_I = 0x7,
	DEVSDK_FRAME_TYPE_VIDEO_P = 0x8,
	DEVSDK_FRAME_TYPE_RES9,
	DEVSDK_FRAME_TYPE_AUDIO   = 0xa,
	DEVSDK_FRAME_TYPE_VIDEO_B = 0xb,
	DEVSDK_FRAME_TYPE_IVS_PEA = 0xc,
	DEVSDK_FRAME_TYPE_IVS_CPC = 0xd,
	DEVSDK_FRAME_TYPE_BUTT
}DEVSDK_FRAME_TYPE_E;

typedef struct 
{
	unsigned short	u16ImageWidth;	//ͼ����
	unsigned short	u16ImageHeight;	//ͼ��߶�
	unsigned char	u8EncodeType;	//��Ƶ�����ʽ0-h264 1-mjpeg 2-jpeg 3-mpeg4  4-h265
	unsigned char	u8FrameRate;	//֡��
	unsigned char	u8Pal;			//��ʽ	0-n��1-pal��
	unsigned char	u8Res[1];
}DEVSDK_VIDEO_INFO_S;

typedef struct 
{
	unsigned char	u8AudioSamples;	//������	0--8k 1--16k 2-32k
	unsigned char	u8EncodeType;	//��Ƶ�����ʽ DEVSDK_AENC_TYPE_E
	unsigned char	u8AudioChannels;//ͨ����	��ֻ֧��1	
	unsigned char	u8AudioBits;	//λ��		16bit
	unsigned char	u8Res[4];
}DEVSDK_AUDIO_INFO_S;

typedef struct 
{
	unsigned short	u16FrameFlag;		//0x3448 magic data
	unsigned char	u8FrameType;		//DEVSDK_FRAME_TYPE_E: I-0x7 p--0x8 b--0xb A--0xa  ivs_peaosc --0xc ivs_cpc --0xd
	unsigned char	u8Res[1];			//����
	unsigned int	u32FrameNo;			//֡��
	unsigned int	u32Time;			//ϵͳʱ��
	unsigned int	u32FrameSize;		//����������
	unsigned long long u64TimeStamp;	//ʱ�������λ΢��
	union
	{
		DEVSDK_VIDEO_INFO_S unVideoInfo;
		DEVSDK_AUDIO_INFO_S unAudioInfo;
	};
}DEVSDK_FRAME_HEAD_S;

typedef struct 
{
	unsigned int  u32PixelFormat;			//���ظ�ʽ
									/*	PIXEL_FORMAT_RGB_BAYER_8BPP,  17
									    PIXEL_FORMAT_RGB_BAYER_10BPP, 18
									    PIXEL_FORMAT_RGB_BAYER_12BPP, 19
									    PIXEL_FORMAT_RGB_BAYER_14BPP, 20
									    PIXEL_FORMAT_RGB_BAYER_16BPP, 21*/
	unsigned int  u32CompressMode;		//ѹ��ģʽ	GSF_IPC_CMD_COMPRESS_MODE_E
	unsigned int  u32Width;
    unsigned int  u32Height;
	unsigned int  u32FrameSize;
	unsigned int  u32Stride;
	unsigned long long  u64Pts;
}DEVSDK_AV_PIPE_FRAME_INFO;

typedef struct 
{
	unsigned short	u16FrameFlag;		//0x3448 magic data
	unsigned char   u8Stream;           //0-������, 1-������, 2-������(�����)
	unsigned char	u8Res[1];			//����
	unsigned int	u32FrameNo;			//֡��
	unsigned int	u32Time;			//ϵͳʱ��
	unsigned long long u64TimeStamp;	//ʱ�������λ΢��
}DEVSDK_AV_ENC_INFO_S;

typedef struct 
{
	unsigned int	u32IpAddr;
	char szUsrName[DEVSDK_NAME_LEN];
}DEVSDK_USER_S;

typedef struct 
{
	unsigned char  u8AEnc;			// DEVSDK_AENC_TYPE_E
	unsigned char  u8DataType;		// 0-file, 1-data
	unsigned char  u8SampleRate; 	// 0-8000 
	unsigned char  u8Res;
	unsigned short u16Res2;
	unsigned short u16Len;
	union{
		char      szFileName[128];
		unsigned char u8Data[160];
	};
}DEVSDK_PLAY_AUDIO_PARAM_S;

#define DEVSDK_IS_I_FRAME(t) ((t) == 0x7)
#define DEVSDK_IS_P_FRAME(t) ((t) == 0x8)
#define DEVSDK_IS_V_FRAME(t) ((t) == 0x7 || (t) == 0x8 || (t) == 0xb)
#define DEVSDK_IS_A_FRAME(t) ((t) == 0xa)
#define DEVSDK_IS_M_FRAME(t) ((t) == 0xc || (t) == 0xd)


typedef struct
{
	float x;
	float y;
}DEVSDK_AI_POINT_S;

typedef struct 
{
	float conf; // confidence
	DEVSDK_AI_POINT_S p1;
	DEVSDK_AI_POINT_S p2;
	char text[256];
}DEVSDK_AI_TARGET_S;

typedef struct {
	int ref_w;
	int ref_h;
	int fps;
	int seq;
	unsigned long long pts;
	int target_num;
	DEVSDK_AI_TARGET_S *target;
}DEVSDK_AI_DATA_S;



typedef enum
{
	DEVSDK_MOVE_ALARM = 0,		// �ƶ����
	DEVSDK_LOST_ALARM,			// ��Ƶ��ʧ
	DEVSDK_COVER_ALARM,			// ��Ƶ�ڵ�
	DEVSDK_IO_ALARM,			// IO����
	DEVSDK_PIR_ALARM,			// �������̽�ⱨ��: �����֧��
	DEVSDK_AVD_ALARM,			// ��Ƶ���: �����֧��
	DEVSDK_PEAOSC_ALARM,		// ���ߡ�����������Ʒ���ȱ���: �����֧��
	DEVSDK_CPC_ALARM,			// Խ������ͳ��: �����֧��
	DEVSDK_SMOKE_ALARM, 		// ������: �����֧��
	DEVSDK_AUDIO_ALARM, 		// ��������: �����֧��
	DEVSDK_LAS_ALARM,			// �����쳣ͣ��: �����֧��
	DEVSDK_LAC_ALARM,			// �������쳣�ر�: �����֧��
	DEVSDK_LAO_ALARM,			// �������쳣����: �����֧��
	DEVSDK_LAR_ALARM,			// ���������쳣: �����֧��
	DEVSDK_LM_ALARM,			// ���ݼ���: �����֧��
	DEVSDK_LF_ALARM,			// ���ݹ���: �����֧��
	DEVSDK_FACEDETC_ALARM,		// �������: �����֧��
	DEVSDK_EBIKE_ALARM, 		// �綯�����: �����֧��
	DEVSDK_ALARM_BUTT,
}DEVSDK_ALARM_TYPE_E;

typedef enum
{
	DEVSDK_MOVE_ALARM_MASK	= (1<<DEVSDK_MOVE_ALARM),		// �ƶ����
	DEVSDK_LOST_ALARM_MASK	= (1<<DEVSDK_LOST_ALARM),		// ��Ƶ��ʧ
	DEVSDK_COVER_ALARM_MASK	= (1<<DEVSDK_COVER_ALARM),		// ��Ƶ�ڵ�
	DEVSDK_IO_ALARM_MASK	= (1<<DEVSDK_IO_ALARM),			// IO����
	DEVSDK_PIR_ALARM_MASK	= (1<<DEVSDK_PIR_ALARM),		// �������̽�ⱨ��: �����֧��
	DEVSDK_AVD_ALARM_MASK	= (1<<DEVSDK_AVD_ALARM),		// ��Ƶ���: �����֧��
	DEVSDK_PEAOSC_ALARM_MASK= (1<<DEVSDK_PEAOSC_ALARM),		// ���ߡ�����������Ʒ���ȱ���: �����֧��
	DEVSDK_CPC_ALARM_MASK	= (1<<DEVSDK_CPC_ALARM),		// Խ������ͳ��: �����֧��
	DEVSDK_SMOKE_ALARM_MASK	= (1<<DEVSDK_SMOKE_ALARM), 		// ������: �����֧��
	DEVSDK_AUDIO_ALARM_MASK	= (1<<DEVSDK_AUDIO_ALARM), 		// ��������: �����֧��
	DEVSDK_LAS_ALARM_MASK	= (1<<DEVSDK_LAS_ALARM),		// �����쳣ͣ��: �����֧��
	DEVSDK_LAC_ALARM_MASK	= (1<<DEVSDK_LAC_ALARM),		// �������쳣�ر�: �����֧��
	DEVSDK_LAO_ALARM_MASK	= (1<<DEVSDK_LAO_ALARM),		// �������쳣����: �����֧��
	DEVSDK_LAR_ALARM_MASK	= (1<<DEVSDK_LAR_ALARM),		// ���������쳣: �����֧��
	DEVSDK_LM_ALARM_MASK	= (1<<DEVSDK_LM_ALARM),			// ���ݼ���: �����֧��
	DEVSDK_LF_ALARM_MASK	= (1<<DEVSDK_LF_ALARM),			// ���ݹ���: �����֧��
	DEVSDK_FACEDETC_ALARM_MASK	= (1<<DEVSDK_FACEDETC_ALARM),// �������: �����֧��
	DEVSDK_EBIKE_ALARM_MASK	= (1<<DEVSDK_EBIKE_ALARM), 		// �綯�����: �����֧��
	DEVSDK_ALARM_MASK_ALL 	= 0xFFFFFFFF,
}DEVSDK_ALARM_MASK_E;

typedef void * RawHdl;
typedef void * RealAVHdl;
typedef void * RecHdl;
typedef void * YuvHdl;
typedef void * BgrHdl;
typedef void * AlarmHdl;
typedef void * PtzHdl;
typedef void * RealAVEncInfoHdl;
typedef void * PDRectHdl;
typedef void * RawSensorHdl;
typedef void * GetVideoFileHdl;
typedef void * PushDataHdl;


typedef int (*CaptureDataCallbackFunc)(unsigned char *pData, int nSize, void *pUser);
typedef int (*RawDataCallbackFunc)(int nStatus, DEVSDK_FRAME_HEAD_S *pFrameHead, unsigned char *pData, void *pUser);
typedef int (*RealAVDataCallbackFunc)(int nStatus, DEVSDK_FRAME_HEAD_S *pFrameHead, unsigned char *pData, void *pUser);
typedef int (*RecDataCallbackFunc)(int nStatus, DEVSDK_FRAME_HEAD_S *pFrameHead, unsigned char *pData, void *pUser);
typedef int (*YUVDataCallbackFunc)(int nStatus, int nWidth, int nHeight, int nPixelFormat, int nSize, unsigned char *pYUVData, void *pUser);
typedef int (*BGRDataCallbackFunc)(int nStatus, int nWidth, int nHeight, int nPlanar, int nSize, unsigned char *pBGRData, void *pUser);
typedef int (*AlarmMsgCallbackFunc)(int nChn, int nAlarm, int nAction, unsigned int u32Time, const char *pszJpegFile, void *pUser);
typedef int (*VideoFileCallbackFunc)(int nStatus, const char *pszVideoFile, void *pUser);
typedef int (*VideoFileCallbackFunc_V2)(int nStatus, const char *pszVideoFile, void *pUser, unsigned long long nFirstPts, unsigned long long nLastPts);
typedef int (*RecordFileCallbackFunc)(int nStatus, const char *pszRecordFile, void *pUser);
typedef int (*RealAVEncInfoCallbackFunc)(int nStatus, DEVSDK_AV_ENC_INFO_S *pstAvEncInfo, void *pUser);
typedef int (*RawSensorDataCallbackFunc)(int nStatus, DEVSDK_AV_PIPE_FRAME_INFO *pViPipeFrame, unsigned char *pData, void *pUser);
typedef int (*PtzMsgCallbackFunc)(int nCmd,int nAddr,int nData1,int nData2,int nFuncNo);
DEVSDK_API int DEVSDK_Init();
DEVSDK_API int DEVSDK_Uninit();
DEVSDK_API char* DEVSDK_GetVersion();

DEVSDK_API int DEVSDK_GetLastError();
DEVSDK_API char* DEVSDK_GetLastErrorMsg(int *pLastErrorNo);

/*
 * ��֧�ֲ���Param����
 * nChn:	Ĭ��Ϊ0
 * nType:	DEVSDK_CMD_TYPE_E
 * nCmd:  	DEVSDK_CMD_PARAM_E/DEVSDK_SYS_CTRL_CMD_E
 * pData:    �����ռ�, ���ȡ/���ò���ʱ�Ķ�Ӧ�ṹ��
 * nDataSize:�����ռ��С
 */
DEVSDK_API int DEVSDK_ParamConfig(int nChn, int nType, int nCmd, void *pData, int nDataSize);

/* 
 * ץ��һ��ͼƬ, ����Ϊָ���ļ�, �첽�ӿ�
 * pszFileName: ͼƬ�ļ���, (Ĭ�����ļ���/memipcmd,���ɸ���)
 */
DEVSDK_API int DEVSDK_Capture(int nChn,const char *pszFileName);

/*
 * ץ��һ��ͼƬ, ��ͨ���ص���������ͼƬ����, �첽�ӿ�
 * fCallback: ͼƬ���ݻص�����
 * pUser: �û��Զ����͸������, �ص�����ʱ����
 */
DEVSDK_API int DEVSDK_CaptureData(int nChn,CaptureDataCallbackFunc fCallback, void *pUser);


/*
 * ��ȡָ��ʱ��η�Χ��ʵʱ��Ƶ����, ���ΪMP4�ļ�, �첽�ӿ�
 * nStream: 0-������, 1-������, 2-������(�����)
 * u32StartTime: ��ʼʱ��, ���ڼ���Ԥ¼ʱ��, unix ʱ���
 * u32StopTime:  ����ʱ��, ���ڼ����¼ʱ��, unix ʱ���
 * fCallback: �������֪ͨ�ص�����
 * pUser: �û��Զ����͸������, �ص����ʱ����
 */
DEVSDK_API int DEVSDK_GetVideoFile(int nStream, int nStartTime, int nStopTime, VideoFileCallbackFunc fCallback, void *pUser);

/*
 * ��ȡSD��¼����ָ��¼�����ͼ�ʱ��η�Χ������, ���ΪMP4�ļ�, �첽�ӿ�
 * nRecType: ¼������DEVSDK_REC_TYPE_E, �粻��ע¼�����Ϳ�����Ϊ DEVSDK_REC_TYPE_ALL
 * u32StartTime: ��ʼʱ��, unix ʱ���
 * u32StopTime:  ����ʱ��, unix ʱ���
 * fCallback: �������֪ͨ�ص�����
 * pUser: �û��Զ����͸������, �ص����ʱ����
 */
DEVSDK_API int DEVSDK_GetRecordFile(int nRecType, int nStartTime, int nStopTime, RecordFileCallbackFunc fCallback, void *pUser);

/*
 * �ͷ�ָ���ļ�
 * pszFileName: Ҫ�ͷŵ��ļ�, ��������·��
 * ע��: ��Ҫ devsdk ���ɵ��ļ������ͷ�, ����: jpg/video/record file
 */
DEVSDK_API int DEVSDK_ReleaseFile(const char *pszFileName);


/*
 * (��)��ȡʵʱ����Ƶ����, ͨ���ص����ÿ֡����
 * nStream: 0-������, 1-������, 2-������(�����)
 * nPreSec: Ԥ¼ʱ��, ��λ��: ע�������ڻ����С, ����޷�����ָ����Ԥ¼ʱ��, ��Ĭ��ʹ��֧�ֵ����ʱ��
 * fCallback: ֡���ݻص�����
 * pUser: �û��Զ����͸������, �ص�֡����ʱ����
 */
DEVSDK_API RawHdl DEVSDK_StartGetRawData(int nStream, int nStartTime, int nStopTime, RawDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetRawData(RawHdl hdl); 

/*
 * (��)��ȡʵʱ����Ƶ����, ͨ���ص����ÿ֡����
 * nStream: 0-������, 1-������, 2-������(�����)
 * nPreSec: Ԥ¼ʱ��, ��λ��: ע�������ڻ����С, ����޷�����ָ����Ԥ¼ʱ��, ��Ĭ��ʹ��֧�ֵ����ʱ��
 * fCallback: ֡���ݻص�����
 * pUser: �û��Զ����͸������, �ص�֡����ʱ����
 */

DEVSDK_API RealAVHdl DEVSDK_StartGetRealAVData(int nStream, int nStartTime, int nStopTime, RealAVDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetRealAVData(RealAVHdl hdl); 

/*
 * (��)��ȡʵʱ����Ƶ������Ϣ(SEI), ͨ���ص������ǰ������ɵ�֡�š�ʱ�估ʱ���
 * nStream: 0-������, 1-������, 2-������(�����)
 * fCallback: ������Ϣ�ص�����
 * pUser: �û��Զ����͸������, �ص�������Ϣʱ����
 */
DEVSDK_API RealAVEncInfoHdl DEVSDK_StartGetRealAVEncInfo(int nStream, RealAVEncInfoCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetRealAVEncInfo(RealAVEncInfoHdl hdl); 


/*
 * ��ȡ SD�� ¼����ָ��¼�����ͼ�ʱ��η�Χ������, ͨ���ص����ÿ֡����
 */
DEVSDK_API RecHdl DEVSDK_StartGetRecData(int nRecType, int nStartTime, int nStopTime, RecDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetRecData(RecHdl hdl); 

/*
 * @width:  0-auto, >0 for resolution width
 * @height: 0-auto, >0 for resolution height
 */
DEVSDK_API YuvHdl DEVSDK_StartGetYUV(int nChan, int nWidth, int nHeight, YUVDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetYUV(YuvHdl hdl);

/********************************���½ӿ�Ϊʾ���ӿڣ��ο�����**************************************/
/*
 * ����Vpss����ͨ�������øýӿں��û���Ҫ�����豸
 * nVpssVencChn: vpss����ͨ��
 * �û�Ҳ�ɲ����øýӿڣ����д�����Ӧ�ļ��������������DEVSDK���ָ��
 */
DEVSDK_API int DEVSDK_EnableVpssVencChn(int nVpssVencChn);

/*
 * ��ȡYUV���ݽ������ݴ����׼������
 * VpssVencChn: vpss����ͨ��
 * nWidth: �½�ָ���ֱ��ʵ�VPSSͨ��ʱ�ķֱ��ʿ��(��userMsgΪ3ʱ)
 * nHeight: �½�ָ���ֱ��ʵ�VPSSͨ��ʱ�ķֱ��ʸ߶�(��userMsgΪ3ʱ)
 * userMsg: �û���Ҫ�鿴��ģ��  
 * 			1-������VPSSͨ����ȡyuv���ݻ����� 
 *			2-������VPSSͨ����ȡyuv����ʹ��vgs�����ڽ����㷨������ץ��ͼƬ
 *			3-�½�ָ���ֱ��ʵ�VPSSͨ��ֱ�ӻ�ȡyuv�����ڽ����㷨������ץ��ͼƬ
 */
DEVSDK_API int DEVSDK_YuvDataProcPrepare(int VpssVencChn, int nWidth, int nHeight,int userMsg,int nRotate);

/*
 * ��ȡYUV���ݽ������ݴ���(�˽ӿ�Ϊʾ���ӿڣ��ο�����)
 * nVpssChn: vpssͨ�� Ĭ��Ϊ0
 * nWidth: �����Ŀ��
 * nHeight: �����ĸ߶�
 * userMsg: �û���Ҫ�鿴��ģ��  
 * 			1-������VPSSͨ����ȡyuv���ݻ����� 
 *			2-������VPSSͨ����ȡyuv����ʹ��vgs�����ڽ����㷨������ץ��ͼƬ
 *			3-�½�ָ���ֱ��ʵ�VPSSͨ��ֱ�ӻ�ȡyuv�����ڽ����㷨������ץ��ͼƬ
 * fCallback: ֡���ݻص�����
 * pUser: �û��Զ����͸������, �ص�֡����ʱ����
 * ���øýӿ�ǰ�������ȵ���DEVSDK_YuvDataProcPrepare�ӿڣ������������DEVSDK���ָ��
 */
DEVSDK_API YuvHdl DEVSDK_GetYUVDataProc(int nVpssChn, int nWidth, int nHeight,int userMsg,YUVDataCallbackFunc fCallback, void *pUser);
DEVSDK_API YuvHdl DEVSDK_GetYUVDataProcV2(int nVpssGrp, int nVpssChn, int nWidth, int nHeight,int userMsg,YUVDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetYUVDataProc(YuvHdl hdl);
/******************************************************************************************************/

/*
 *����OSDpts
 */
DEVSDK_API YuvHdl DEVSDK_SetYuvOsdProc(int nVpssChn, int nWidth, int nHeight,int userMsg,YUVDataCallbackFunc fCallback, void *pUser);


/*
 * ��ȡBGR����
 */
DEVSDK_API BgrHdl DEVSDK_StartGetBGR(int nVpssGrp, int nVpssChn, int nWidth, int nHeight, BGRDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetBGR(BgrHdl hdl);

/*
 * ch: 0
 * ��֧�� G711A ������ 8000 ����Ƶ��ʽ�ļ�������
 */
DEVSDK_API int DEVSDK_PlayAudio(int nChn, DEVSDK_PLAY_AUDIO_PARAM_S *pstParam);



/*
 * u32Mask: DEVSDK_ALARM_MASK_E
 */
DEVSDK_API AlarmHdl DEVSDK_StartGetAlarm(int nChn, unsigned int u32Mask, AlarmMsgCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetAlarm(AlarmHdl hdl);

/*
 * 485����׼���ӿڣ����ж�д������ǰ��
 * COMM_PARA_S *param:���ڲ���
 * ����ֵ���ü�comm485.h
 * ����ֵ:fd ���
 */
DEVSDK_API int DEVSDK_Comm485Prepare(COMM_PARA_S *param);

/*
 * 485����д������
 * fd: DEVSDK_Comm485Prepare�ӿڷ��صľ��
 * writeMsg: ��Ҫд�������
 */
DEVSDK_API int DEVSDK_Comm485Write(int fd,char *writeMsg);

/*
 * 485���ڶ�ȡ����
 * fd: DEVSDK_Comm485Prepare�ӿڷ��صľ��
 * readMsg: �Ӵ��ڽ��ܵ�������
 */
DEVSDK_API int DEVSDK_Comm485Read(int fd,char *readMsg);

/*
 * vpssͨ����ü����ýӿ�
 * nVpssGrpNo: vpss group��
 * nEnable: ���ÿ���
 * x: ������
 * y: ������
 * width: ���
 * height: �߶�
 */
int DEVSDK_Vpss_SetCrop(int nVpssGrpNo,int nEnable,int x,int y,int width,int height);

/*
 * vpssͨ����ü���ȡ�ӿ�
 * nVpssGrpNo: vpss group��
 * nEnable(���): ���ÿ���
 * x(���): ������
 * y(���): ������
 * width(���): ���
 * height(���): �߶�
 */
int DEVSDK_Vpss_GetCrop(int nVpssGrpNo,int *nEnable,int *x,int *y,int *width,int *height);

/*
 * ��ȡоƬΨһʶ����ӿ�
 * pstUniqueId(���):оƬΨһʶ����
 */
int DEVSDK_Get_UniqueId(hi_unique_id *unique_id);

/*
 * ��ʼ��ȡ���μ������λ��
 * ʹ��ǰӦ�������ܷ������μ�⹦��
 */
PDRectHdl DEVSDK_StartGetPDRectInfo();
int DEVSDK_StopGetPDRectInfo(PDRectHdl hdl);

/*
 * ��ȡʵʱraw sensor����, ͨ���ص������ǰ֡��Ϣ
 * nBitWidth: λ��	GSF_IPC_CMD_BITWIDTH_FORMAT_E
 * nCompressMode: ѹ��ģʽ	GSF_IPC_CMD_COMPRESS_MODE_E
 * pUser: �û��Զ����͸������, �ص�����ʱ����
 */
RawSensorHdl DEVSDK_StartGetRawSensorData(int nBitWidth,int nCompressMode, RawSensorDataCallbackFunc fCallback, void *pUser);
int DEVSDK_StopGetRawSensorData(RawSensorHdl hdl);

/*
 * ������չVencͨ������ץ�� ��ץȫͼ��Сͼ
 * pUserRect   ץСͼʱָ�������� ȫͼʱ�ɲ���
 * nWidth  ץСͼʱָ���Ŀ� ȫͼʱ�ɲ���
 * nHeight ץСͼʱָ���ĸ� ȫͼʱ�ɲ���
 * pUser: �û��Զ����͸������, �ص�����ʱ����
 */
int DEVSDK_StartVencCapture(int nVpssChn,void *pUserRect, int nWidth, int nHeight, unsigned int *pFileSize, void *pUser);
int DEVSDK_VencCaptureRecv(int nVpssChn, void *pUserRect, unsigned int *pFileSize, int nWidth, int nHeight);
int DEVSDK_StopVencCapture(int nVpssChn);
int DEVSDK_VB_Iint(int nSize);


/*
��ʼ��ȡPTZԤ��λ������Ϣ
*/

PtzHdl DEVSDK_StartGetPtz(int nChn, PtzMsgCallbackFunc fCallback, void *pUser);
int DEVSDK_StopGetptz(PtzHdl hdl);


/*
�ϴ�����
*/
PushDataHdl DEVSDK_OpenPushData();
int DEVSDK_PushData(PushDataHdl hdl, DEVSDK_AI_DATA_S *pData);
int DEVSDK_ClosePushData(PushDataHdl hdl);


/*
 * �����ļ������ⱻ�Զ�����: Ҫע��ʹ���꼰ʱʹ��UnLock�ӿ��ͷ�, ������Դ�޷�������ʹ��
 */
int DEVSDK_LockFile(const char *pszFileName);
int DEVSDK_UnLockFile(const char *pszFileName);
/*�ж��ļ��Ƿ��ѱ�����*/
int DEVSDK_IsFileLocked(const char *pszFileName);

int DEVSDK_Capture_v2(int nChn, int nStream, const char *pszFileName);

DEVSDK_API GetVideoFileHdl DEVSDK_StartGetVideoFile(int nStream, unsigned long long uStartTimeMs, unsigned long long uStopTimeMs, VideoFileCallbackFunc_V2 fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetVideoFile(GetVideoFileHdl hdl);


/*
 * �豸����: ʹ��ָ��·���������ļ�����
 * pszFileName: �����������ļ��ľ���·��
 * 
 * !!ע��!!: �����ӿڷ��سɹ�ֻ������������ɹ������������������Ѿ����!!
 *		 
 *			 �����������첽���еģ�������ͨ������״̬�ӿڻ�ȡ���뱣֤�����������豸���ȶ����磬������ܵ�������ʧ��!!
 *			 �豸������ɺ���Զ���������ȴ��豸�Զ��������
 *			 �豸��������Ҳ���Զ�����
 */
int DEVSDK_DeviceUpgrade(const char *pszFileName);

/*
 * ����ֵ: ����������: 0 - 99
 *		   �����ѳɹ�: 100
 *		   ����ʧ��:   < 0 ���� > 100
 *					101: �����ļ���ʽ����
 *					104: �汾����
 */
int DEVSDK_GetDevUpgradeStatus();


/////////////////////////////����������/////////////////////////////////

int DEVSDK_SetAiData(YuvHdl hdl, AI_DATA_INFO_S *pData, int nCount);

/////////////////////////////����������/////////////////////////////////



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
