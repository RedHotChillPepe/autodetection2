#ifndef _GSF_YUV_OSD_H_
#define _GSF_YUV_OSD_H_
#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <string.h>

typedef enum
{
	TYPE_FONT_SIZE_8P,
	TYPE_FONT_SIZE_12P,
	TYPE_FONT_SIZE_16P,
	TYPE_FONT_SIZE_24P,
	TYPE_FONT_SIZE_32P,
	TYPE_FONT_SIZE_48P,
}enFontSize;

typedef struct
{
	uInt32 Font_Size;
	uInt32 Data_Size;
}stFont;
typedef struct
{
	Int8   OsdTitle[256];
	uInt32 FontSize;
	uInt32 FontColor;
	uInt32 Font_PosX;
	uInt32 Font_PosY;
	uInt8 Res;
}stYuvOsdInfo;

void gsf_yuv_osd_refresh(stYuvOsdInfo *pYuvOsdCfg,stYuvBuffInfo *pYuvBuffInfo);

void gsf_match_font_lib(stFont *pFont,int nFontsize);

#ifdef __cplusplus
}
#endif

#endif

