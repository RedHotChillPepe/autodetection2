#ifndef _GSF_FONT_UTF8_DRAW_LINE_H
#define _GSF_FONT_UTF8_DRAW_LINE_H
#ifdef GSF_OSD_FREETYPE_SUPP
#include "../inc/freetype/ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#endif
#include <pthread.h>
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#define GSF_OSD_MAX_CHAR 128
typedef struct _GSF_FONT_INVERT_INFO_S
{
	unsigned char u8FontFlag;				//���ֱ�ʶ
	unsigned char u8Invert;					// 0 - ���в���ɫ  1 - ���ȵͷ�ɫ��ɫ  2 - ���ȸ߷�ɫ��ɫ	(���)
	unsigned char u8LineNum;				// ���б�� 0-��һ�� 1-�ڶ��� �Դ�����
	unsigned char u8Res;
	unsigned int u32Width;					//������������			(����)
	unsigned int u32Height;	
	unsigned long long u64RegionLuma;		//����������Ⱥ�			(���)
	unsigned long long u64MaxRegionLuma;	//�������ʵ��������Ⱥ�	(���)
}GSF_FONT_INVERT_INFO_S;

typedef struct _GSF_FONT_INVERT_CFG_S
{
	int nCheckNum;		//�����������
	GSF_FONT_INVERT_INFO_S *pInvertInfo;
}GSF_FONT_INVERT_CFG_S;

typedef enum _GSF_FONT_EDGE_ALIGN_TYPE_E
{
    GSF_FONT_EDGE_ALIGN_LEFT_TOP 		= 100,		// ����
    GSF_FONT_EDGE_ALIGN_RIGHT_TOP 		= 101,		// ����
    GSF_FONT_EDGE_ALIGN_LEFT_BOTTOM 	= 102,		// ����
    GSF_FONT_EDGE_ALIGN_RIGHT_BOTTOM 	= 103,		// ����
}GSF_FONT_EDGE_ALIGN_TYPE_E;

typedef int (*FontCheckInvertFunc)(int nVencNo,unsigned int hRegHdl,GSF_FONT_INVERT_CFG_S *pFontInvertCfg);

typedef struct _GSF_FONT_CFG_S
{
	unsigned char u8Init;
	unsigned char u8VencChn;
	unsigned char u8Res[2];
	unsigned short u16StartFontColor;
	unsigned short u16StartEdgeColor;
	unsigned short u16FontColor[256];
	unsigned short u16EdgeColor[256];
	FontCheckInvertFunc pCheckInvert;
}GSF_FONT_CFG_S;

typedef struct _GSF_FREETYEP_CFG_S
{
	unsigned char u8Init;
	unsigned char u8InitLib;
	unsigned char u8Res[2];

	pthread_mutex_t mutex;
}GSF_FREETYEP_CFG_S;
typedef struct _GSF_FREETYPE_FONT_BIT_CFG
{
	unsigned short u16BitWidth;
	unsigned short u16BitHeight;
	unsigned short u16charwidth;
	unsigned short u16hoffset;
	unsigned short u16nNextoffset;
	unsigned short u16Res;
}GSF_FREETYPE_FONT_BIT_CFG;

int gsf_font_init(GSF_FONT_CFG_S *pFontCfg);

int gsf_font_uninit();

char * gsf_font_utf8_transfrom(char *pTitle,int nTitleLen);

int gsf_font_get_utf8_char_num(unsigned char u8Data);

int gsf_font_check_invert_title(int nFontInt,unsigned char u8FontW,unsigned char u8FontH,int nTitleLen,char *pTitle,GSF_FONT_INVERT_CFG_S *pFontInvertCfg);

int gsf_font_utf8_draw_line(unsigned char u8FontW, unsigned short u16OsdWidth, int nFontInt, char *pBuff,
							char *pNewTitle, char *pOldTitle, unsigned short u16FontColor, 
							unsigned short u16EdgeColor, unsigned short u16OldFontColor, 
							unsigned short u16OldEdgeColor, int nBufSize, int nAlign, int nLine,
							unsigned char u8FontH);

int gsf_font_utf8_draw_line_invert(unsigned char u8FontW, unsigned short u16OsdWidth, int nFontInt, char *pBuff,
							char *pNewTitle, char *pOldTitle, unsigned short u16FontColor, 
							unsigned short u16EdgeColor, unsigned short u16OldFontColor, 
							unsigned short u16OldEdgeColor, int nBufSize,unsigned char u8VencChn,
							unsigned char u8FontH,unsigned short u16OsdHeight,unsigned char u8InvertColor,
							unsigned int hRegHdl, int nAlign, int nLine);
							
int gsf_freetype_init();
int gsf_freetype_uninit();
int gsf_freetype_font_utf8_draw_line(unsigned char u8FontW, 
										  unsigned short u16OsdWidth, 
										  int nFontInt, 
										  char *pBuff,
										  char *pNewTitle, 
										  char *pOldTitle, 
										  unsigned short u16FontColor, 
										  unsigned short u16EdgeColor, 
										  unsigned short u16OldFontColor, 
										  unsigned short u16OldEdgeColor, 
										  int nBufSize, int nAlign, int nLine,unsigned char u8FontH);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _GSF_FONT_UTF8_DRAW_LINE_H */
