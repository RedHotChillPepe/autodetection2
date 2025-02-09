#ifndef _DRAW_YUV_RECT_H_
#define _DRAW_YUV_RECT_H_

#include <stdio.h>
#include <string.h>

typedef unsigned char  GSF_U8;
typedef unsigned short GSF_U16;
typedef unsigned int   GSF_U32;

typedef char  GSF_S8;
typedef short GSF_S16;
typedef int   GSF_S32;

typedef struct _RECT32_S
{
	GSF_S32 s32LeftX;
	GSF_S32 s32TopY;
	GSF_S32 s32RightX;
	GSF_S32 s32BottomY;
}RECT32_S;

//void gsf_draw_yuv_rect32(char *pVBufVirt_Y, int nPixelFormat, GSF_U32 u32Stride[], GSF_U32 u32Height, RECT32_S *rc);

#endif