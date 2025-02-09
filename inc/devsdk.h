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


/////////////////////////////НОВОЕ/////////////////////////////////
#ifndef GSF_VPSS_YUV_MAX_AI_DATA
#define GSF_VPSS_YUV_MAX_AI_DATA 50 // Макс число передаваеммых прямоугольников
#endif

/* Была какая то проблема поэтому объявил тут */
typedef void* YuvHdl;

/* 
 * Структура "прямоугольник + текст"
 */
 
typedef struct _AI_DATA_INFO_S {  // Я заменил родную структуру, может какие то другие функции крашнутся, только для примера
    struct {
        int x;
        int y;
        int width;
        int height;
    } rect;
    char szText[64];
} AI_DATA_INFO_S;



/////////////////////////////НОВОЕ/////////////////////////////////




typedef enum
{
	DEVSDK_OK = 0,					//OK
	DEVSDK_ERR_NOINIT,				//没有初始化SDK
	DEVSDK_ERR_INIT,				//初始化SDK错误
	DEVSDK_ERR_CONN,				//连接服务端失败
	DEVSDK_ERR_PARAM,				//参数错误
	DEVSDK_ERR_CMD,					//命令错误
	DEVSDK_ERR_ALLOC,				//申请资源失败
	DEVSDK_ERR_SCALE,				//vgs缩放失败
	DEVSDK_ERR_MMAP,				//Mmap函数出错
	DEVSDK_ERR_CONVERT,				//YUV转BGR失败
	DEVSDK_ERR_FILE_NOEXIST,		//文件不存在
	DEVSDK_ERR_THREAD,				//创建线程错误
	DEVSDK_ERR_MSG,					//发送或者接收消息错误
	DEVSDK_ERR_NOUNBIND,			//没有解绑对应通道
	DEVSDK_ERR_CREATECHNFAIL,		//创建通道失败
	DEVSDK_ERR_JPEGPREPAREERR,		//jpeg准备失败
	DEVSDK_ERR_GETORSETPARAMERR,	//设置或者获取参数失败
	DEVSDK_ERR_485COMMPREPAREERR,	//485串口设置属性失败
	DEVSDK_ERR_485READERR,			//485读取数据错误
	DEVSDK_ERR_485WRITEERR,			//485写数据错误
	DEVSDK_ERR_VPSSSETGROPERR,		//设置vpss grop属性错误
	DEVSDK_ERR_NOTSUPP,				//不支持的请求
	DEVSDK_ERR_SEND,				//发送数据失败
	DEVSDK_ERR_RECV,				//接收数据失败
	DEVSDK_ERR_UNKNOWN = 255,		//未知错误
}DEVSDK_RET;

typedef enum _DEVSDK_REC_TYPE_E
{
	DEVSDK_REC_TYPE_TIME	= 0x1,		// 0 定时录像
	DEVSDK_REC_TYPE_MD		= 0x2,		// 1 移动侦测录像
	DEVSDK_REC_TYPE_IO		= 0x4,		// 2 IO报警录像
	DEVSDK_REC_TYPE_MANUAL	= 0x8,		// 3 手动录像
	DEVSDK_REC_TYPE_OD 		= 0x10,		// 4 视频遮挡录像
	DEVSDK_REC_TYPE_NET_FAULT	= 0x20,	// 5 网络错误录像
	DEVSDK_REC_TYPE_PIR		= 0x40,		// 6 人体红外感应录像
	DEVSDK_REC_TYPE_FTP_UNDONE	= 0x80,	// 7 FTP未上传成功，使能FTP重传时使用
	DEVSDK_REC_TYPE_GSENSOR	= 0x100,	// 8 GSensor 自动录像,不自动覆盖
	DEVSDK_REC_TYPE_RO		= 0x200,	// 9 只读录像,不自动覆盖
	DEVSDK_REC_TYPE_FD 		= 0x400,	// 10 人脸检测录像, 用于报警类型对应
	DEVSDK_REC_TYPE_RES		= 0x800,	// 11 保留
	DEVSDK_REC_TYPE_AUDIO	= 0x1000,	// 12 声音检测报警录像
	DEVSDK_REC_TYPE_LSC		= 0x2000,	// 13 电梯异常停留报警录像	
	DEVSDK_REC_TYPE_AVD		= 0x4000,	// 14 
	DEVSDK_REC_TYPE_PEAOSC	= 0x8000,	// 15 
	DEVSDK_REC_TYPE_CPC		= 0x10000,	// 16 
	DEVSDK_REC_TYPE_SMOKE	= 0x20000,	// 17 
	DEVSDK_REC_TYPE_LAO		= 0x40000,	// 18 电梯异常开启报警录像
	DEVSDK_REC_TYPE_LAC		= 0x80000,	// 19 电梯异常关闭报警录像
	DEVSDK_REC_TYPE_LAR		= 0x100000,	// 20 电梯异常走梯报警录像
	DEVSDK_REC_TYPE_LM  	= 0x200000, // 21 电梯检修报警录像
	DEVSDK_REC_TYPE_LF    	= 0x400000, // 22 电梯故障报警录像
	DEVSDK_REC_TYPE_EBIKE	= 0x800000,	// 23 电动车报警录像
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

//音频编码类型
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
	unsigned short	u16ImageWidth;	//图像宽度
	unsigned short	u16ImageHeight;	//图像高度
	unsigned char	u8EncodeType;	//视频编码格式0-h264 1-mjpeg 2-jpeg 3-mpeg4  4-h265
	unsigned char	u8FrameRate;	//帧率
	unsigned char	u8Pal;			//制式	0-n制1-pal制
	unsigned char	u8Res[1];
}DEVSDK_VIDEO_INFO_S;

typedef struct 
{
	unsigned char	u8AudioSamples;	//采样率	0--8k 1--16k 2-32k
	unsigned char	u8EncodeType;	//音频编码格式 DEVSDK_AENC_TYPE_E
	unsigned char	u8AudioChannels;//通道数	暂只支持1	
	unsigned char	u8AudioBits;	//位数		16bit
	unsigned char	u8Res[4];
}DEVSDK_AUDIO_INFO_S;

typedef struct 
{
	unsigned short	u16FrameFlag;		//0x3448 magic data
	unsigned char	u8FrameType;		//DEVSDK_FRAME_TYPE_E: I-0x7 p--0x8 b--0xb A--0xa  ivs_peaosc --0xc ivs_cpc --0xd
	unsigned char	u8Res[1];			//保留
	unsigned int	u32FrameNo;			//帧号
	unsigned int	u32Time;			//系统时间
	unsigned int	u32FrameSize;		//数据流长度
	unsigned long long u64TimeStamp;	//时间戳，单位微秒
	union
	{
		DEVSDK_VIDEO_INFO_S unVideoInfo;
		DEVSDK_AUDIO_INFO_S unAudioInfo;
	};
}DEVSDK_FRAME_HEAD_S;

typedef struct 
{
	unsigned int  u32PixelFormat;			//像素格式
									/*	PIXEL_FORMAT_RGB_BAYER_8BPP,  17
									    PIXEL_FORMAT_RGB_BAYER_10BPP, 18
									    PIXEL_FORMAT_RGB_BAYER_12BPP, 19
									    PIXEL_FORMAT_RGB_BAYER_14BPP, 20
									    PIXEL_FORMAT_RGB_BAYER_16BPP, 21*/
	unsigned int  u32CompressMode;		//压缩模式	GSF_IPC_CMD_COMPRESS_MODE_E
	unsigned int  u32Width;
    unsigned int  u32Height;
	unsigned int  u32FrameSize;
	unsigned int  u32Stride;
	unsigned long long  u64Pts;
}DEVSDK_AV_PIPE_FRAME_INFO;

typedef struct 
{
	unsigned short	u16FrameFlag;		//0x3448 magic data
	unsigned char   u8Stream;           //0-主码流, 1-次码流, 2-三码流(如存在)
	unsigned char	u8Res[1];			//保留
	unsigned int	u32FrameNo;			//帧号
	unsigned int	u32Time;			//系统时间
	unsigned long long u64TimeStamp;	//时间戳，单位微秒
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
	DEVSDK_MOVE_ALARM = 0,		// 移动侦测
	DEVSDK_LOST_ALARM,			// 视频丢失
	DEVSDK_COVER_ALARM,			// 视频遮挡
	DEVSDK_IO_ALARM,			// IO报警
	DEVSDK_PIR_ALARM,			// 人体红外探测报警: 需程序支持
	DEVSDK_AVD_ALARM,			// 视频诊断: 需程序支持
	DEVSDK_PEAOSC_ALARM,		// 拌线、警戒区、物品检测等报警: 需程序支持
	DEVSDK_CPC_ALARM,			// 越线人数统计: 需程序支持
	DEVSDK_SMOKE_ALARM, 		// 烟雾报警: 需程序支持
	DEVSDK_AUDIO_ALARM, 		// 声音报警: 需程序支持
	DEVSDK_LAS_ALARM,			// 电梯异常停留: 需程序支持
	DEVSDK_LAC_ALARM,			// 电梯门异常关闭: 需程序支持
	DEVSDK_LAO_ALARM,			// 电梯门异常开启: 需程序支持
	DEVSDK_LAR_ALARM,			// 电梯走梯异常: 需程序支持
	DEVSDK_LM_ALARM,			// 电梯检修: 需程序支持
	DEVSDK_LF_ALARM,			// 电梯故障: 需程序支持
	DEVSDK_FACEDETC_ALARM,		// 人脸检测: 需程序支持
	DEVSDK_EBIKE_ALARM, 		// 电动车检测: 需程序支持
	DEVSDK_ALARM_BUTT,
}DEVSDK_ALARM_TYPE_E;

typedef enum
{
	DEVSDK_MOVE_ALARM_MASK	= (1<<DEVSDK_MOVE_ALARM),		// 移动侦测
	DEVSDK_LOST_ALARM_MASK	= (1<<DEVSDK_LOST_ALARM),		// 视频丢失
	DEVSDK_COVER_ALARM_MASK	= (1<<DEVSDK_COVER_ALARM),		// 视频遮挡
	DEVSDK_IO_ALARM_MASK	= (1<<DEVSDK_IO_ALARM),			// IO报警
	DEVSDK_PIR_ALARM_MASK	= (1<<DEVSDK_PIR_ALARM),		// 人体红外探测报警: 需程序支持
	DEVSDK_AVD_ALARM_MASK	= (1<<DEVSDK_AVD_ALARM),		// 视频诊断: 需程序支持
	DEVSDK_PEAOSC_ALARM_MASK= (1<<DEVSDK_PEAOSC_ALARM),		// 拌线、警戒区、物品检测等报警: 需程序支持
	DEVSDK_CPC_ALARM_MASK	= (1<<DEVSDK_CPC_ALARM),		// 越线人数统计: 需程序支持
	DEVSDK_SMOKE_ALARM_MASK	= (1<<DEVSDK_SMOKE_ALARM), 		// 烟雾报警: 需程序支持
	DEVSDK_AUDIO_ALARM_MASK	= (1<<DEVSDK_AUDIO_ALARM), 		// 声音报警: 需程序支持
	DEVSDK_LAS_ALARM_MASK	= (1<<DEVSDK_LAS_ALARM),		// 电梯异常停留: 需程序支持
	DEVSDK_LAC_ALARM_MASK	= (1<<DEVSDK_LAC_ALARM),		// 电梯门异常关闭: 需程序支持
	DEVSDK_LAO_ALARM_MASK	= (1<<DEVSDK_LAO_ALARM),		// 电梯门异常开启: 需程序支持
	DEVSDK_LAR_ALARM_MASK	= (1<<DEVSDK_LAR_ALARM),		// 电梯走梯异常: 需程序支持
	DEVSDK_LM_ALARM_MASK	= (1<<DEVSDK_LM_ALARM),			// 电梯检修: 需程序支持
	DEVSDK_LF_ALARM_MASK	= (1<<DEVSDK_LF_ALARM),			// 电梯故障: 需程序支持
	DEVSDK_FACEDETC_ALARM_MASK	= (1<<DEVSDK_FACEDETC_ALARM),// 人脸检测: 需程序支持
	DEVSDK_EBIKE_ALARM_MASK	= (1<<DEVSDK_EBIKE_ALARM), 		// 电动车检测: 需程序支持
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
 * 仅支持部分Param参数
 * nChn:	默认为0
 * nType:	DEVSDK_CMD_TYPE_E
 * nCmd:  	DEVSDK_CMD_PARAM_E/DEVSDK_SYS_CTRL_CMD_E
 * pData:    参数空间, 如获取/设置参数时的对应结构体
 * nDataSize:参数空间大小
 */
DEVSDK_API int DEVSDK_ParamConfig(int nChn, int nType, int nCmd, void *pData, int nDataSize);

/* 
 * 抓拍一张图片, 保存为指定文件, 异步接口
 * pszFileName: 图片文件名, (默认在文件夹/memipcmd,不可更改)
 */
DEVSDK_API int DEVSDK_Capture(int nChn,const char *pszFileName);

/*
 * 抓拍一张图片, 并通过回调函数返回图片数据, 异步接口
 * fCallback: 图片数据回调函数
 * pUser: 用户自定义的透传参数, 回调数据时带回
 */
DEVSDK_API int DEVSDK_CaptureData(int nChn,CaptureDataCallbackFunc fCallback, void *pUser);


/*
 * 获取指定时间段范围的实时视频数据, 输出为MP4文件, 异步接口
 * nStream: 0-主码流, 1-次码流, 2-三码流(如存在)
 * u32StartTime: 开始时间, 用于计算预录时长, unix 时间戳
 * u32StopTime:  结束时间, 用于计算后录时长, unix 时间戳
 * fCallback: 操作结果通知回调函数
 * pUser: 用户自定义的透传参数, 回调结果时带回
 */
DEVSDK_API int DEVSDK_GetVideoFile(int nStream, int nStartTime, int nStopTime, VideoFileCallbackFunc fCallback, void *pUser);

/*
 * 获取SD卡录像中指定录像类型及时间段范围的数据, 输出为MP4文件, 异步接口
 * nRecType: 录像类型DEVSDK_REC_TYPE_E, 如不关注录像类型可设置为 DEVSDK_REC_TYPE_ALL
 * u32StartTime: 开始时间, unix 时间戳
 * u32StopTime:  结束时间, unix 时间戳
 * fCallback: 操作结果通知回调函数
 * pUser: 用户自定义的透传参数, 回调结果时带回
 */
DEVSDK_API int DEVSDK_GetRecordFile(int nRecType, int nStartTime, int nStopTime, RecordFileCallbackFunc fCallback, void *pUser);

/*
 * 释放指定文件
 * pszFileName: 要释放的文件, 包含绝对路径
 * 注意: 需要 devsdk 生成的文件才能释放, 包含: jpg/video/record file
 */
DEVSDK_API int DEVSDK_ReleaseFile(const char *pszFileName);


/*
 * (旧)获取实时音视频数据, 通过回调输出每帧数据
 * nStream: 0-主码流, 1-次码流, 2-三码流(如存在)
 * nPreSec: 预录时长, 单位秒: 注意受限于缓冲大小, 如果无法做到指定的预录时长, 则默认使用支持的最大时长
 * fCallback: 帧数据回调函数
 * pUser: 用户自定义的透传参数, 回调帧数据时带回
 */
DEVSDK_API RawHdl DEVSDK_StartGetRawData(int nStream, int nStartTime, int nStopTime, RawDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetRawData(RawHdl hdl); 

/*
 * (新)获取实时音视频数据, 通过回调输出每帧数据
 * nStream: 0-主码流, 1-次码流, 2-三码流(如存在)
 * nPreSec: 预录时长, 单位秒: 注意受限于缓冲大小, 如果无法做到指定的预录时长, 则默认使用支持的最大时长
 * fCallback: 帧数据回调函数
 * pUser: 用户自定义的透传参数, 回调帧数据时带回
 */

DEVSDK_API RealAVHdl DEVSDK_StartGetRealAVData(int nStream, int nStartTime, int nStopTime, RealAVDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetRealAVData(RealAVHdl hdl); 

/*
 * (新)获取实时音视频编码信息(SEI), 通过回调输出当前编码完成的帧号、时间及时间戳
 * nStream: 0-主码流, 1-次码流, 2-三码流(如存在)
 * fCallback: 编码信息回调函数
 * pUser: 用户自定义的透传参数, 回调编码信息时带回
 */
DEVSDK_API RealAVEncInfoHdl DEVSDK_StartGetRealAVEncInfo(int nStream, RealAVEncInfoCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetRealAVEncInfo(RealAVEncInfoHdl hdl); 


/*
 * 获取 SD卡 录像中指定录像类型及时间段范围的数据, 通过回调输出每帧数据
 */
DEVSDK_API RecHdl DEVSDK_StartGetRecData(int nRecType, int nStartTime, int nStopTime, RecDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetRecData(RecHdl hdl); 

/*
 * @width:  0-auto, >0 for resolution width
 * @height: 0-auto, >0 for resolution height
 */
DEVSDK_API YuvHdl DEVSDK_StartGetYUV(int nChan, int nWidth, int nHeight, YUVDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetYUV(YuvHdl hdl);

/********************************以下接口为示例接口，参考作用**************************************/
/*
 * 启用Vpss编码通道，调用该接口后，用户需要重启设备
 * nVpssVencChn: vpss编码通道
 * 用户也可不调用该接口，自行创建对应文件，操作流程详见DEVSDK编程指南
 */
DEVSDK_API int DEVSDK_EnableVpssVencChn(int nVpssVencChn);

/*
 * 获取YUV数据进行数据处理的准备过程
 * VpssVencChn: vpss编码通道
 * nWidth: 新建指定分辨率的VPSS通道时的分辨率宽度(即userMsg为3时)
 * nHeight: 新建指定分辨率的VPSS通道时的分辨率高度(即userMsg为3时)
 * userMsg: 用户需要查看的模块  
 * 			1-在已有VPSS通道获取yuv数据画矩形 
 *			2-在已有VPSS通道获取yuv数据使用vgs缩放在进行算法分析并抓拍图片
 *			3-新建指定分辨率的VPSS通道直接获取yuv数据在进行算法分析并抓拍图片
 */
DEVSDK_API int DEVSDK_YuvDataProcPrepare(int VpssVencChn, int nWidth, int nHeight,int userMsg,int nRotate);

/*
 * 获取YUV数据进行数据处理(此接口为示例接口，参考作用)
 * nVpssChn: vpss通道 默认为0
 * nWidth: 期望的宽度
 * nHeight: 期望的高度
 * userMsg: 用户需要查看的模块  
 * 			1-在已有VPSS通道获取yuv数据画矩形 
 *			2-在已有VPSS通道获取yuv数据使用vgs缩放在进行算法分析并抓拍图片
 *			3-新建指定分辨率的VPSS通道直接获取yuv数据在进行算法分析并抓拍图片
 * fCallback: 帧数据回调函数
 * pUser: 用户自定义的透传参数, 回调帧数据时带回
 * 调用该接口前，必须先调用DEVSDK_YuvDataProcPrepare接口，操作流程详见DEVSDK编程指南
 */
DEVSDK_API YuvHdl DEVSDK_GetYUVDataProc(int nVpssChn, int nWidth, int nHeight,int userMsg,YUVDataCallbackFunc fCallback, void *pUser);
DEVSDK_API YuvHdl DEVSDK_GetYUVDataProcV2(int nVpssGrp, int nVpssChn, int nWidth, int nHeight,int userMsg,YUVDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetYUVDataProc(YuvHdl hdl);
/******************************************************************************************************/

/*
 *设置OSDpts
 */
DEVSDK_API YuvHdl DEVSDK_SetYuvOsdProc(int nVpssChn, int nWidth, int nHeight,int userMsg,YUVDataCallbackFunc fCallback, void *pUser);


/*
 * 获取BGR数据
 */
DEVSDK_API BgrHdl DEVSDK_StartGetBGR(int nVpssGrp, int nVpssChn, int nWidth, int nHeight, BGRDataCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetBGR(BgrHdl hdl);

/*
 * ch: 0
 * 仅支持 G711A 采样率 8000 的音频格式文件或数据
 */
DEVSDK_API int DEVSDK_PlayAudio(int nChn, DEVSDK_PLAY_AUDIO_PARAM_S *pstParam);



/*
 * u32Mask: DEVSDK_ALARM_MASK_E
 */
DEVSDK_API AlarmHdl DEVSDK_StartGetAlarm(int nChn, unsigned int u32Mask, AlarmMsgCallbackFunc fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetAlarm(AlarmHdl hdl);

/*
 * 485串口准备接口，就行读写操作的前提
 * COMM_PARA_S *param:串口参数
 * 参数值设置见comm485.h
 * 返回值:fd 句柄
 */
DEVSDK_API int DEVSDK_Comm485Prepare(COMM_PARA_S *param);

/*
 * 485串口写入数据
 * fd: DEVSDK_Comm485Prepare接口返回的句柄
 * writeMsg: 需要写入的数据
 */
DEVSDK_API int DEVSDK_Comm485Write(int fd,char *writeMsg);

/*
 * 485串口读取数据
 * fd: DEVSDK_Comm485Prepare接口返回的句柄
 * readMsg: 从串口接受到的数据
 */
DEVSDK_API int DEVSDK_Comm485Read(int fd,char *readMsg);

/*
 * vpss通道组裁剪设置接口
 * nVpssGrpNo: vpss group号
 * nEnable: 启用开关
 * x: 横坐标
 * y: 纵坐标
 * width: 宽度
 * height: 高度
 */
int DEVSDK_Vpss_SetCrop(int nVpssGrpNo,int nEnable,int x,int y,int width,int height);

/*
 * vpss通道组裁剪获取接口
 * nVpssGrpNo: vpss group号
 * nEnable(输出): 启用开关
 * x(输出): 横坐标
 * y(输出): 纵坐标
 * width(输出): 宽度
 * height(输出): 高度
 */
int DEVSDK_Vpss_GetCrop(int nVpssGrpNo,int *nEnable,int *x,int *y,int *width,int *height);

/*
 * 获取芯片唯一识别码接口
 * pstUniqueId(输出):芯片唯一识别码
 */
int DEVSDK_Get_UniqueId(hi_unique_id *unique_id);

/*
 * 开始获取人形检测区域位置
 * 使用前应开启智能分析人形检测功能
 */
PDRectHdl DEVSDK_StartGetPDRectInfo();
int DEVSDK_StopGetPDRectInfo(PDRectHdl hdl);

/*
 * 获取实时raw sensor数据, 通过回调输出当前帧信息
 * nBitWidth: 位宽	GSF_IPC_CMD_BITWIDTH_FORMAT_E
 * nCompressMode: 压缩模式	GSF_IPC_CMD_COMPRESS_MODE_E
 * pUser: 用户自定义的透传参数, 回调数据时带回
 */
RawSensorHdl DEVSDK_StartGetRawSensorData(int nBitWidth,int nCompressMode, RawSensorDataCallbackFunc fCallback, void *pUser);
int DEVSDK_StopGetRawSensorData(RawSensorHdl hdl);

/*
 * 创建扩展Venc通道进行抓拍 可抓全图和小图
 * pUserRect   抓小图时指定的区域 全图时可不填
 * nWidth  抓小图时指定的宽 全图时可不填
 * nHeight 抓小图时指定的高 全图时可不填
 * pUser: 用户自定义的透传参数, 回调数据时带回
 */
int DEVSDK_StartVencCapture(int nVpssChn,void *pUserRect, int nWidth, int nHeight, unsigned int *pFileSize, void *pUser);
int DEVSDK_VencCaptureRecv(int nVpssChn, void *pUserRect, unsigned int *pFileSize, int nWidth, int nHeight);
int DEVSDK_StopVencCapture(int nVpssChn);
int DEVSDK_VB_Iint(int nSize);


/*
开始获取PTZ预置位设置消息
*/

PtzHdl DEVSDK_StartGetPtz(int nChn, PtzMsgCallbackFunc fCallback, void *pUser);
int DEVSDK_StopGetptz(PtzHdl hdl);


/*
上传数据
*/
PushDataHdl DEVSDK_OpenPushData();
int DEVSDK_PushData(PushDataHdl hdl, DEVSDK_AI_DATA_S *pData);
int DEVSDK_ClosePushData(PushDataHdl hdl);


/*
 * 锁定文件，避免被自动覆盖: 要注意使用完及时使用UnLock接口释放, 否则资源无法回收再使用
 */
int DEVSDK_LockFile(const char *pszFileName);
int DEVSDK_UnLockFile(const char *pszFileName);
/*判断文件是否已被锁定*/
int DEVSDK_IsFileLocked(const char *pszFileName);

int DEVSDK_Capture_v2(int nChn, int nStream, const char *pszFileName);

DEVSDK_API GetVideoFileHdl DEVSDK_StartGetVideoFile(int nStream, unsigned long long uStartTimeMs, unsigned long long uStopTimeMs, VideoFileCallbackFunc_V2 fCallback, void *pUser);
DEVSDK_API int DEVSDK_StopGetVideoFile(GetVideoFileHdl hdl);


/*
 * 设备升级: 使用指定路径的升级文件升级
 * pszFileName: 本地升级包文件的绝对路径
 * 
 * !!注意!!: 升级接口返回成功只代表升级请求成功，不代表升级过程已经完成!!
 *		 
 *			 升级过程是异步进行的，进度请通过升级状态接口获取，请保证升级过程中设备的稳定供电，否则可能导致升级失败!!
 *			 设备升级完成后会自动重启，请等待设备自动重启完成
 *			 设备升级出错，也会自动重启
 */
int DEVSDK_DeviceUpgrade(const char *pszFileName);

/*
 * 返回值: 升级进行中: 0 - 99
 *		   升级已成功: 100
 *		   升级失败:   < 0 或者 > 100
 *					101: 升级文件格式错误
 *					104: 版本错误
 */
int DEVSDK_GetDevUpgradeStatus();


/////////////////////////////НОВОЕ/////////////////////////////////

int DEVSDK_SetAiData(YuvHdl hdl, AI_DATA_INFO_S *pData, int nCount);

/////////////////////////////НОВОЕ/////////////////////////////////



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
