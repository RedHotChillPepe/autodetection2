#ifndef __GSF_VENC_CAPTURE_H__
#define __GSF_VENC_CAPTURE_H__

#include "gsf_param.h"

int gsf_venc_capture_start(int nVpssChn, GSF_RECT_S *pRect, int nWidth, int nHeight, GSF_U32 *nFileSize, void *pUser);

int gsf_venc_capture_recv(int nVpssChn,GSF_RECT_S *pRect, GSF_U32 *nFileSize, int nWidth, int nHeight);

int gsf_venc_capture_stop(int nVpssChn);

#endif

