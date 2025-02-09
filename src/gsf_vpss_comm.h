#ifndef __GSF_VPSS_COMM_H__
#define __GSF_VPSS_COMM_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "hi_math.h"
#include "devsdk.h"

/* Config */
int gsf_vpss_set_depth(int nVpssGrpNo, int nVpssChn, int nDepth);

int release_vpss_chn(int nVpssGrpNo, int nVpssChn);

int create_vpss_chn(int nVpssGrpNo, int nVpssChn, int nWidth, int nHeight,int nRotate);

int vpss_set_rotate(int nVpssGrpNo, int nVpssChn, ot_rotation nVal);

int vpss_set_crop(int nVpssGrpNo,int nEnable,int x,int y,int width,int height);

int vpss_get_crop(int nVpssGrpNo,int *nEnable,int *x,int *y,int *width,int *height);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
