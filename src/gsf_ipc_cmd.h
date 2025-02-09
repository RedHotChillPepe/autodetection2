/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_ipc_cmd.h

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/01/16
  Description   :
  History       :
  1.Date        : 2021/01/16
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __GSF_IPC_CMD_H__
#define __GSF_IPC_CMD_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>

/* Config */
#define GSF_IPC_CMD_SUPP_ACK
#define GSF_IPC_CMD_MAGIC		0x20200624
#define GSF_IPC_CMD_SERV_PATH	"/tmp/ipccmd"
#define GSF_IPC_CMD_MAX_PATH_LEN	128
#define GSF_IPC_CMD_MAX_RETRY	5
#define GSF_IPC_CMD_MAX_SPEED	20


typedef enum _GSF_IPC_CMD_E
{
	GSF_IPC_CMD_PIC = 0,    // PICTURE
	GSF_IPC_CMD_MP4_VIDEO,	// MP4 VIDEO
	GSF_IPC_CMD_DEL_FILE,	// DELETE FILE
	GSF_IPC_CMD_REAL_AV_DATA,	// H264/H265/MJPG REAL AV DATA
	GSF_IPC_CMD_REC_DATA,    // H264/H265/MJPG RAW RECORD DATA from TF/SD
	GSF_IPC_CMD_MP4_REC,	// MP4 file from REC
	GSF_IPC_CMD_REALAV_ENC_INFO, // 
	GSF_IPC_CMD_RAW_SENSOR_DATA, //
	GSF_IPC_CMD_MP4_VIDEO_V2,  //MP4 VIDEO 支持回调时携带第一帧和最后一帧时间戳
	GSF_IPC_CMD_PIC_V2, 		//支持选择抓拍码流，码流类型参考：GSF_AV_VENC_STREAM_TYPE_E
	GSF_IPC_CMD_PUSH_DATA,  // 推送数据
}GSF_IPC_CMD_E;

typedef enum _GSF_IPC_CMD_DIR_E
{
	GSF_IPC_CMD_DIR_REQ = 0,
	GSF_IPC_CMD_DIR_RSP,
}GSF_IPC_CMD_DIR_E;

typedef enum _GSF_IPC_CMD_TYPE_E
{
	GSF_IPC_CMD_TYPE_FILE = 0,
	GSF_IPC_CMD_TYPE_DATA, // raw_data support
}GSF_IPC_CMD_TYPE_E;

typedef enum _GSF_IPC_CMD_VIDEO_CTRL_E
{
	GSF_IPC_CMD_VIDEO_CTRL_START = 0,
	GSF_IPC_CMD_VIDEO_CTRL_STOP,
}GSF_IPC_CMD_VIDEO_CTRL_E;

typedef enum _GSF_IPC_CMD_ERR_E
{
	GSF_IPC_CMD_ERR_FAILED = -1,
	GSF_IPC_CMD_ERR_OK = 0,
	GSF_IPC_CMD_ERR_NOTSUPP,
}GSF_IPC_CMD_ERR_E;

typedef enum _GSF_IPC_CMD_BITWIDTH_FORMAT_E
{
    GSF_IPC_CMD_BITWIDTH_FORMAT_8		= 8,		/* PIXEL_FORMAT_RGB_BAYER_8BPP */
    GSF_IPC_CMD_BITWIDTH_FORMAT_10		= 10,		/* PIXEL_FORMAT_RGB_BAYER_10BPP */
    GSF_IPC_CMD_BITWIDTH_FORMAT_12		= 12,		/* PIXEL_FORMAT_RGB_BAYER_12BPP */
    GSF_IPC_CMD_BITWIDTH_FORMAT_14		= 14,		/* PIXEL_FORMAT_RGB_BAYER_14BPP */
    GSF_IPC_CMD_BITWIDTH_FORMAT_16		= 16,		/* PIXEL_FORMAT_RGB_BAYER_16BPP */
} GSF_IPC_CMD_BITWIDTH_FORMAT_E;

typedef enum _GSF_IPC_CMD_COMPRESS_MODE_E
{
    GSF_IPC_CMD_COMPRESS_MODE_NONE		= 0x0,		/* no compress */
    GSF_IPC_CMD_COMPRESS_MODE_SEG		= 0x1,		/* compress unit is 256 bytes as a segment, default seg mode */
    GSF_IPC_CMD_COMPRESS_MODE_SEG128	= 0x2,		/* compress unit is 128 bytes as a segment */
    GSF_IPC_CMD_COMPRESS_MODE_LINE		= 0x3,		/* compress unit is the whole line */
    GSF_IPC_CMD_COMPRESS_MODE_FRAME		= 0x4,		/* compress unit is the whole frame */

    GSF_IPC_CMD_COMPRESS_MODE_BUTT
} GSF_IPC_CMD_COMPRESS_MODE_E;

typedef struct _GSF_IPC_CMD_PIC_PARAM_S
{
	int nCh;
	int nStream;
	unsigned char res[24];
}GSF_IPC_CMD_PIC_PARAM_S;

typedef struct _GSF_IPC_CMD_VIDEO_PARAM_S
{
	unsigned short mask;    // bit: 0x01-specify size;  0x02-specify time;
	unsigned short ctrl;    // GSF_IPC_CMD_VIDEO_CTRL_E: 0-start; 1-stop
	unsigned char audio;    // record audio: 0-closed;  1-open
	unsigned char stream;   // 0-main stream;  1-sub stream;  2-third stream ...
	unsigned char speed;    // 0 ~ GSF_IPC_CMD_MAX_SPEED
	unsigned char res[1];
	unsigned int  limit_size;// limit size of byte, valid when (mask & 0x01) == 0x01
	unsigned int  start_time;// utc time, valid when (mask & 0x02) == 0x02
	unsigned int  end_time;  // utc time, valid when (mask & 0x02) == 0x02
	unsigned char res2[12];
}GSF_IPC_CMD_VIDEO_PARAM_S;

typedef struct _GSF_IPC_CMD_VIDEO_PARAM_S_V2
{
	unsigned short mask;    // bit: 0x01-specify size;  0x02-specify time;
	unsigned short ctrl;    // GSF_IPC_CMD_VIDEO_CTRL_E: 0-start; 1-stop
	unsigned char audio;    // record audio: 0-closed;  1-open
	unsigned char stream;   // 0-main stream;  1-sub stream;  2-third stream ...
	unsigned char speed;    // 0 ~ GSF_IPC_CMD_MAX_SPEED
	unsigned char res[1];
	unsigned int  limit_size; // limit size of byte, valid when (mask & 0x01) == 0x01
	unsigned long long StartPts; // utc time, valid when (mask & 0x02) == 0x02 //鍗曚綅锛氭绉?
	unsigned long long StopPts;  // utc time, valid when (mask & 0x02) == 0x02 //鍗曚綅锛氭绉?
	unsigned int nRes;	
}GSF_IPC_CMD_VIDEO_PARAM_S_V2;

typedef enum _GSF_IPC_CMD_CTRL_E
{
	GSF_IPC_CMD_CTRL_START = 0,
	GSF_IPC_CMD_CTRL_STOP,
}GSF_IPC_CMD_CTRL_E;

typedef struct _GSF_IPC_CMD_ENC_PARAM_S
{
	unsigned char ctrl;    // GSF_IPC_CMD_CTRL_E: 0-start; 1-stop
	unsigned char stream;   // 0-main stream;  1-sub stream;  2-third stream ...
	unsigned char res[30];
}GSF_IPC_CMD_ENC_PARAM_S;

typedef struct _GSF_IPC_CMD_REC_PARAM_S
{
	unsigned int  type;     // record type: GSF_IPC_CMD_REC_TYPE_E
	unsigned short ctrl;    // GSF_IPC_CMD_VIDEO_CTRL_E: 0-start; 1-stop
	unsigned char audio;    // need audio: 0-closed;  1-open
	unsigned char speed;    // 0 ~ GSF_IPC_CMD_MAX_SPEED
	unsigned int  start_time;// record start time(UTC)
	unsigned int  end_time;  // record stop time(UTC)
	unsigned char res2[20];
}GSF_IPC_CMD_REC_PARAM_S;

typedef struct _GSF_IPC_CMD_VI_PARAM_S
{
	unsigned short ctrl;    		// GSF_IPC_CMD_VI_CTRL_E: 0-start; 1-stop
	unsigned char res[2];
	unsigned int nViPipe;			//0:ViPipe0 ~ 3:ViPipe 3
	unsigned int nBitWidth;			//The bit num to be dump GSF_IPC_CMD_PIXEL_FORMAT_E
	unsigned int nCompressMode;		//Whether to  dump compressed raw, default is 0  GSF_IPC_CMD_COMPRESS_MODE_E
	unsigned int nByteAlign;			//Whether convert to Byte align, default is 1
	unsigned char res2[12];
}GSF_IPC_CMD_VI_PARAM_S;

typedef struct _GSF_IPC_CMD_DATA_PARAM_S
{
	unsigned short ctrl;
	unsigned char res[2];
}GSF_IPC_CMD_DATA_PARAM_S;

typedef struct _GSF_IPC_CMD_PARAM_S
{
	char path[GSF_IPC_CMD_MAX_PATH_LEN]; // response for file path
	int  size; // for data size
	union{
		GSF_IPC_CMD_PIC_PARAM_S   pic;
		GSF_IPC_CMD_VIDEO_PARAM_S video;
		GSF_IPC_CMD_REC_PARAM_S   rec;
		GSF_IPC_CMD_ENC_PARAM_S   enc;
		GSF_IPC_CMD_VI_PARAM_S	  vi;
		GSF_IPC_CMD_VIDEO_PARAM_S_V2 videov2;
		GSF_IPC_CMD_DATA_PARAM_S  data;
	};
}GSF_IPC_CMD_PARAM_S;

typedef struct _GSF_IPC_CMD_MSG_S
{
	unsigned int  magic;	// GSF_IPC_CMD_MAGIC
	unsigned int  seq;		// 
	unsigned char cmd;		// GSF_IPC_CMD_E: command type
	unsigned char dir;      // GSF_IPC_CMD_DIR_E: request or response
	unsigned char type; 	// GSF_IPC_CMD_TYPE_E: 0-for file; 1-for data
	unsigned char res;
	int           err;		// GSF_IPC_CMD_ERR_E
	GSF_IPC_CMD_PARAM_S param;
}GSF_IPC_CMD_MSG_S;

// client send ack format when recv raw-data
typedef struct GSF_IPC_CMD_ACK_MSG_S
{
	unsigned int u32Magic;            // 0x20201215
	unsigned int u32FrameNo;          // success recv frameNo
	unsigned int u32FrameSize;        // success recv frameSize
	unsigned char u8Res[20];          // reserved data
}GSF_IPC_CMD_ACK_MSG_S;

typedef struct _GSF_NET_MANUAL_SHOOT_S_
{
	unsigned char u8SaveMode; //当无sd 卡时，u8SaveMode 有GSF_SAVE_TYPE_E GSF_SAVE_FTP等相应需求才抓拍到内存
	unsigned char u8StreamType;	//码流类型
	unsigned char u8Res[2];
	char szFileName[64];
	void *pCb;
	//GSF_TIME_S struTime;
}GSF_NET_MANUAL_SHOOT_S, *LPGSF_NET_MANUAL_SHOOT_S;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
