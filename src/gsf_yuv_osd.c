#include <stdio.h>
#include <string.h>
#include "gsf_yuv_draw_rect.h"
#include "gsf_yuv_osd.h"
#include <time.h>
#if 0
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define FT_OSD_H 2160
#define FT_OSD_W 3840
#endif
extern unsigned char * gsf_get_GB2312_char_bit(char *szBitmap, int nQu, int nWei, int nFontSize);
extern char*  gsf_utf8_to_gb2312(const char* utf8, int utf8Len, char *gb2312);
extern int  gsf_utf8_to_unicode(unsigned char  *byte, int index, int count, unsigned int* unicode);
extern int gsf_get_Utf8_byte_num(unsigned char firstCh);
void gsf_match_font_lib(stFont *pFont,int nFontsize)
{

	switch(nFontsize)
	{
		case TYPE_FONT_SIZE_8P:
			pFont->Font_Size = 8;
			pFont->Data_Size = 8;
			break;
		case TYPE_FONT_SIZE_12P:
			pFont->Font_Size = 12;
			pFont->Data_Size = 24;
			break;
		case TYPE_FONT_SIZE_16P:
			pFont->Font_Size = 16;
			pFont->Data_Size = 32;
			break;
		case TYPE_FONT_SIZE_24P:
			pFont->Font_Size = 24;
			pFont->Data_Size = 72;
			break;			
		case TYPE_FONT_SIZE_32P:
			pFont->Font_Size = 32;
			pFont->Data_Size = 128;
			break;
		case TYPE_FONT_SIZE_48P:
			pFont->Font_Size = 48;
			pFont->Data_Size = 288;
			break;
		default:
			pFont->Font_Size = 24;
			pFont->Data_Size = 72;
			break;
	}
}
#if 0
void draw_bitmap( FT_Bitmap*  bitmap,
             FT_Int      x,
             FT_Int      y, unsigned char data[FT_OSD_H][FT_OSD_W])
{
  FT_Int  i, j, p, q;
  FT_Int  x_max = x + bitmap->width;
  FT_Int  y_max = y + bitmap->rows;


  /* for simplicity, we assume that `bitmap->pixel_mode' */
  /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */
  //memset(&image, 0, sizeof(image));
  for ( i = x, p = 0; i < x_max; i++, p++ )
  {
    for ( j = y, q = 0; j < y_max; j++, q++ )
    {
      if ( i < 0      || j < 0       ||
           i >= FT_OSD_W || j >= FT_OSD_H )
        continue;

     *(data[j] + i) |= bitmap->buffer[q * bitmap->width + p];
    }
  }
}
#endif

void gsf_yuv_osd_refresh(stYuvOsdInfo *pYuvOsdCfg,stYuvBuffInfo *pYuvBuffInfo)
{
	int nQu = 0, nWei = 0, nOffset = 0, cur_byte = 0;	
	int nNewPos= 0, nNewOffset = 0, fontoffset = 0, nNewTitleLen = 0;
	int nOsdAreaWidth = 0, nOsdAreaHeight = 0;
	int sum = 1;
	char szData[512] = {0};
	char szBuffNew[256] = {'\0'};
    stFont stFontInfo = {0};
	stPoint draw_point = {0};
	stDrawLineInfo drawLineInfo = {0};
	uInt8 *YBuff = NULL, *UVBuff = NULL;
	int i = 0, j = 0;
	if(pYuvOsdCfg == NULL || pYuvBuffInfo == NULL || pYuvBuffInfo->pYuvBuff == NULL ||
		pYuvBuffInfo->height < 2 || pYuvBuffInfo->width < 2 || pYuvOsdCfg == NULL)
	{		
		printf("[%s %d] invalid argument!\n", __func__, __LINE__);
		return;
	}
    nNewTitleLen = strnlen(pYuvOsdCfg->OsdTitle, 127);
	if(nNewTitleLen > 0)
	{
		gsf_utf8_to_gb2312(pYuvOsdCfg->OsdTitle,strnlen(pYuvOsdCfg->OsdTitle, 127),szBuffNew);
	}
	
	szBuffNew[sizeof(szBuffNew)-1] = '\0';	
	gsf_match_font_lib(&stFontInfo,pYuvOsdCfg->FontSize);//匹配字库
	nOsdAreaHeight = stFontInfo.Font_Size;
	nOsdAreaWidth = nNewTitleLen * stFontInfo.Data_Size / (nOsdAreaHeight / 8);
	for(i = 0;szBuffNew[i] != '\0';)
	{
		if(szBuffNew[i] == '/' && szBuffNew[i + 1] == 'n')
		{
			sum++;
			i += 2;
		}
		else
			i++;
		
	}
	if(pYuvOsdCfg->Font_PosX < 0 || (pYuvOsdCfg->Font_PosX + nOsdAreaWidth) > pYuvBuffInfo->width )
	{
		printf("[%s %d] The osd area is incorrectly: X : %d",__func__, __LINE__, pYuvOsdCfg->Font_PosX);
		pYuvOsdCfg->Font_PosX = pYuvBuffInfo->width - nOsdAreaWidth - nOsdAreaHeight;
		printf(", Reset : X : %d\n", pYuvOsdCfg->Font_PosX);
	}

	if(pYuvOsdCfg->Font_PosY < 0 || (pYuvOsdCfg->Font_PosY + (sum * nOsdAreaHeight)) > pYuvBuffInfo->height)
	{		
		pYuvOsdCfg->Font_PosY = pYuvBuffInfo->height - (sum * nOsdAreaHeight);		
		printf("[%s %d] The osd area is incorrectly specified,Reset :Y %d\n",__func__, __LINE__,pYuvOsdCfg->Font_PosY);
	}
	draw_point.y = pYuvOsdCfg->Font_PosY;
	switch(pYuvBuffInfo->yuvType)
	{
		case TYPE_YUV422I_UYVY:
		case TYPE_YUV422I_YUYV:
		case TYPE_YUV444I:
		{
			YBuff = pYuvBuffInfo->pYuvBuff;
			UVBuff = NULL;
		}break;
		case TYPE_YUV420SP_NV12:
		case TYPE_YUV420SP_NV21:
		case TYPE_YUV444P:
		case TYPE_YUV422P:
        case TYPE_YUV420I:
		{
			YBuff = pYuvBuffInfo->pYuvBuff;
			UVBuff = pYuvBuffInfo->pYuvBuff + pYuvBuffInfo->width * pYuvBuffInfo->height;
		}break;
		default:
			return;
	}
	
	for (nNewPos = 0; szBuffNew[nNewPos] != '\0';)
	{
		if(szBuffNew[nNewPos] == '/' && szBuffNew[nNewPos+1] == 'n')
		{
			nNewOffset = 2;
			draw_point.y =pYuvOsdCfg->Font_PosY = pYuvOsdCfg->Font_PosY+stFontInfo.Font_Size;
			nNewPos += nNewOffset;
			fontoffset = 0;
		}
		if (szBuffNew[nNewPos]>= 128)
		{
			nQu = szBuffNew[nNewPos] - 160;
			nWei = szBuffNew[nNewPos+1] - 160;
			nOffset = 2;
			nNewOffset = 2;
			fontoffset++;
		 }
		 else
		 {
			nQu = 3;
			nWei = szBuffNew[nNewPos] - 32;
			nOffset = 1;
			nNewOffset = 1;
			fontoffset++;
		 }
		gsf_get_GB2312_char_bit(szData,nQu,nWei,stFontInfo.Font_Size);
		
		for(int i = 0; i < stFontInfo.Data_Size; ++i)
		{
			++cur_byte;						
			for(int k = 0; k < 8; ++k)
			{
				if(szData[i] & (0x80 >>k))
				{
					drawLineInfo.clrIdx =  pYuvOsdCfg->FontColor;
					draw_point.x = pYuvOsdCfg->Font_PosX+ k + ((fontoffset -1)*stFontInfo.Font_Size) + (cur_byte-1) *8 ;
					yuv_setdata(YBuff, UVBuff, pYuvBuffInfo->yuvType, pYuvBuffInfo->width, pYuvBuffInfo->height, draw_point, drawLineInfo.clrIdx); //设置该点数据为指定颜色线
				}
			}

			if(stFontInfo.Font_Size == 8 && cur_byte == 1)
			{
				cur_byte = 0;
				draw_point.y +=1;
			}				
			else if(stFontInfo.Font_Size == 12 && cur_byte == 2)
			{
				cur_byte = 0;
				draw_point.y +=1;
			}			
			else if(stFontInfo.Font_Size == 16 && cur_byte == 2)
			{
				cur_byte = 0;
				draw_point.y +=1;
			}
			else if(stFontInfo.Font_Size == 24 && cur_byte == 3)
			{
				cur_byte = 0;
				draw_point.y +=1;
			}
			else if(stFontInfo.Font_Size == 32 && cur_byte == 4)
			{
				cur_byte = 0;
				draw_point.y +=1;
			}				
			else if(stFontInfo.Font_Size == 48 && cur_byte == 6)
			{
				cur_byte = 0;
				draw_point.y +=1;
			}
		}
		memset(szData, 0, sizeof(szData));
		draw_point.y =pYuvOsdCfg->Font_PosY;
		nNewPos += nNewOffset;
	}
#if 0	
	FT_Library	  library;
	FT_Face 	  face;
	
	FT_GlyphSlot  slot;
	FT_Vector	  pen;					  /* untransformed origin  */
	FT_Error	  error;
	
	char		  filename[128] = "/mnt/Regular.ttf";
	char text[32]= "�кö�����";
	
	int 		  target_height;
	int 		  n, num_chars;
	
	static int lasti = 0;
	
	time_t timep;
	time(&timep);
	
	struct tm *p;
	p = gmtime(&timep);
	
	//snprintf(text, 32, "%d-%d-%d %d:%d:%d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,8 + p->tm_hour, p->tm_min, p->tm_sec);
	printf("[%s %d]get	filename %s text %s\n",__func__,__LINE__,filename,text);
	num_chars	  = strlen(text);
	target_height = FT_OSD_H;
	
	error = FT_Init_FreeType( &library );			   /* initialize library */
	/* error handling omitted */
	printf("[%s %d]FT_Init_FreeType ok\n",__func__,__LINE__);
	error = FT_New_Face( library, filename, 0, &face );/* create face object */
	/* error handling omitted */
	
	printf("[%s %d]FT_New_Face ok\n",__func__,__LINE__);
	/* use 50pt at 100dpi */
	//error = FT_Set_Char_Size( face, 50 * 64, 0,
	//						  100, 0 ); 			   /* set character size */
	/* error handling omitted */
	
	error = FT_Set_Pixel_Sizes(face, 256,256);
	printf("[%s %d]FT_Set_Char_Size ok\n",__func__,__LINE__);
	/* cmap selection omitted;										  */
	/* for simplicity we assume that the font contains a Unicode cmap */
	
	slot = face->glyph;
	static unsigned char bitmap[FT_OSD_H][FT_OSD_W] = {0};
	
	pen.x = 300 * 64;
	pen.y = ( target_height -200 ) * 64;
	int num = 0, uncodenum = 0;
	if(1)//lasti != p->tm_sec)
	{
	   lasti = p->tm_sec;
	   memset(bitmap, 0, sizeof(bitmap));
		for ( n = 0; n < num_chars; n+=3 )
		{

		  FT_Set_Transform( face, NULL, &pen );

			gsf_utf8_to_unicode((unsigned char *)&(text[n]), 0, 3, &uncodenum);
		  /* load glyph image into the slot (erase previous one) */
			printf("[%s %d] get char %s uncodenum %d\n",__func__,__LINE__,text,uncodenum);
		  error = FT_Load_Char( face, uncodenum, FT_LOAD_RENDER );
		  if ( error )
			continue;				  /* ignore errors */
		
		  /* now, draw to our target surface (convert position) */
		  draw_bitmap( &slot->bitmap,
					   slot->bitmap_left,
					   target_height - slot->bitmap_top, bitmap);
		
		  /* increment pen position */
		  pen.x += slot->advance.x;
		  pen.y += slot->advance.y;
		}
	}

	printf("[%s %d] get ponit x = %d y = %d\n",__func__,__LINE__,pen.x,pen.y);
	draw_point.x = pen.x;
	draw_point.y = pen.y;
	FT_Done_Face	( face );
	FT_Done_FreeType( library );
	for ( i = 0; i < FT_OSD_H; ++i)
	{
	  for (j = 0; j < FT_OSD_W; ++j)
	  {
	   
		   if(bitmap[i][j] != 0)
		   {			   
			   drawLineInfo.clrIdx =  pYuvOsdCfg->FontColor;
			   //printf();
			   draw_point.x =  j ;//+ ((fontoffset -1)*stFontInfo.Font_Size) + (cur_byte-1) *8 ;
			   yuv_setdata(YBuff, UVBuff, pYuvBuffInfo->yuvType, pYuvBuffInfo->width, pYuvBuffInfo->height, draw_point, drawLineInfo.clrIdx); //设置该点数据为指定颜色线
		   }
		//putchar( image[i][j] == 0 ? ' ': image[i][j] < 128 ? '+': '*' );
		   //draw_point.x = pen.x;
	  }
	  draw_point.y =pen.y+i;
	}
#endif
}

