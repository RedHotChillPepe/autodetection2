#ifndef _GSF_FONT_LIB_H
#define _GSF_FONT_LIB_H


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

unsigned char * gsf_get_GB2312_char_bit(unsigned char *szBitmap, int nQu, int nWei, int nFontSize);

#ifdef DEF_USE_OSD_KOREAN
unsigned char * gsf_get_EUC_KR_char_bit(unsigned char *szBitmap, int nQu, int nWei, int nFontSize);
#endif

#ifdef GSF_OSD_FONT_BIG5_SUPP
unsigned char * gsf_get_BIG5_char_bit(unsigned char *szBitmap, int nQu, int nWei, int nFontSize);
unsigned char * gsf_get_ASCII_char_bit(unsigned char *szBitmap, int nQu, int nWei, int nFontSize);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
