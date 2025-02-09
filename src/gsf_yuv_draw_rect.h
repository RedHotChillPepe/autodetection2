#ifndef _YUV_DRAW_RECT_H_
#define _YUV_DRAW_RECT_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <string.h>

#define POINT_MAX_NUM 8
 
typedef unsigned char  uInt8;
typedef unsigned short uInt16;
typedef unsigned int uInt32;
typedef char Int8;
typedef short Int16;
typedef int Int32;
 
typedef enum
{
	TYPE_YUV422I_UYVY,
	TYPE_YUV422I_YUYV,
	TYPE_YUV420SP_NV12,
	TYPE_YUV420SP_NV21,
	TYPE_YUV422P,
	TYPE_YUV444I,
	TYPE_YUV444P,
    TYPE_YUV420I,
}enYuvType;
 
typedef enum
{
	YUV_GREEN = 0,
	YUV_RED,
	YUV_BLUE,
	YUV_LIGHT_PURPLE,
	YUV_DARK_GREEN,
	YUV_YELLOW,
	YUV_LIGHT_BLUE,
	YUV_PURPLE,
	YUV_DARK_BLACK,
	YUV_GRAY,
	YUV_WHITE,
	YUV_COLOR_MAX,
}enYuvColorIdx;
 
typedef struct
{
	uInt8 Y;
	uInt8 U;
	uInt8 V;
	uInt8 Res;
}stYuvColor;
 
typedef struct
{
	uInt16 x;
	uInt16 y;
}stPoint;
 
typedef struct
{
	stPoint startPoint;
	stPoint endPoint;
	uInt16 lineWidth;
	uInt16 res;
	enYuvColorIdx clrIdx;
}stDrawLineInfo;

typedef struct
{
	uInt16 lineWidth;
	uInt16 pointNum;
	stPoint stPoint[POINT_MAX_NUM];
	enYuvColorIdx clrIdx[POINT_MAX_NUM];
}stDrawRectInfo;

typedef struct
{
	enYuvType yuvType;
	uInt8 *pYuvBuff;
	uInt16 width;
	uInt16 height;
}stYuvBuffInfo;

void yuv_drawline(stYuvBuffInfo *pYuvBuffInfo, stDrawLineInfo *pDrawLineInfo);
void yuv_draw_rect(stYuvBuffInfo* yuvBuffInfo, stDrawRectInfo *pDrawRectInfo);
void yuv_setdata(
	uInt8* YBuff,
	uInt8* UVBuff,
	enYuvType yuvType,
	uInt16 width,
	uInt16 height,
	stPoint draw_point,
	enYuvColorIdx clrIdx);

#ifdef __cplusplus
}
#endif

#endif
