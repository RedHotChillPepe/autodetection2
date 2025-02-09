/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_UVC_H__
#define __HI_MPI_UVC_H__

#include "hi_common.h"
#include "hi_common_uvc.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_uvc_create_chn(hi_uvc_chn uvc_chn, const hi_uvc_chn_attr *chn_attr);
hi_s32 hi_mpi_uvc_destroy_chn(hi_uvc_chn uvc_chn);

hi_s32 hi_mpi_uvc_start_chn(hi_uvc_chn uvc_chn);
hi_s32 hi_mpi_uvc_stop_chn(hi_uvc_chn uvc_chn);

hi_s32 hi_mpi_uvc_get_chn_attr(hi_uvc_chn uvc_chn, hi_uvc_chn_attr *chn_attr);

#ifdef __cplusplus
}
#endif

#endif /* __HI_MPI_UVC_H__ */
