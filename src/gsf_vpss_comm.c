#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "hi_math.h"
#include <sys/time.h>
#include <stdlib.h>
#include "hi_common_vi.h"
#include "hi_common_vgs.h"
#include "hi_common_venc.h"
#include "hi_common_video.h"
#include "hi_mpi_vi.h"
#include "hi_mpi_vgs.h"
#include "hi_mpi_venc.h"
#include "hi_mpi_vpss.h"
#include "gsf_vpss_comm.h"
#include "gsf_param.h"
#include "devsdk.h"
#include "hi_defines.h"

#define HI_MAX_VI_CHN 1
#define HI_VI_CHN     0

// 1. SetDepth: HI_MPI_VPSS_GetChnAttr -> HI_MPI_VPSS_SetChnAttr
// 2. 

int gsf_vpss_set_depth(int nVpssGrpNo, int nVpssChn, int nDepth)
{
	hi_s32 s32Ret = 0;
	int nOldDepth = 0;
	if (nVpssChn >= HI_VPSS_MAX_PHYS_CHN_NUM)
	{
		printf("[%s %d] nVpssChn(%d) invalid!!\n", __func__, __LINE__, nVpssChn);
		return -1;
	}

	hi_vpss_chn_attr chn_attr = {0};
	if (0 != (s32Ret = hi_mpi_vpss_get_chn_attr(nVpssGrpNo, nVpssChn, &chn_attr)))
	{
		printf("[%s %d] hi_mpi_vpss_get_chn_attr failed with %#x!\n", __func__, __LINE__, s32Ret);	
		return -1;
	}
	nOldDepth = chn_attr.depth;
	if (chn_attr.depth != nDepth)
	{
		chn_attr.depth = nDepth;
		if (0 != (s32Ret = hi_mpi_vpss_set_chn_attr(nVpssGrpNo, nVpssChn, &chn_attr)))
		{
			printf("[%s %d] hi_mpi_vpss_set_chn_attr failed with %#x!\n", __func__, __LINE__, s32Ret);	
			return -1;
		}
		else
			printf("[%s %d] Set Vpss GrpNo(%d) chn(%d) depth(%d) success!\n", __func__, __LINE__, nVpssGrpNo, nVpssChn, nDepth);
	}
	return nOldDepth;
}

int vpss_set_crop(int nVpssGrpNo,int nEnable,int x,int y,int width,int height)
{
	hi_s32 s32Ret = 0;
	hi_vpss_crop_info crop_info = {0};

	crop_info.enable = nEnable;
	crop_info.crop_mode = OT_COORD_ABS;
	crop_info.crop_rect.x = x;
	crop_info.crop_rect.y = y;
	crop_info.crop_rect.width = width;
	crop_info.crop_rect.height = height;

	if(0 != (s32Ret = hi_mpi_vpss_set_grp_crop(nVpssGrpNo, &crop_info)))
	{
		printf("[%s %d] hi_mpi_vpss_set_grp_crop failed with with %d!\n", __func__, __LINE__, s32Ret);
		return -1;
	}else
	{
		printf("[%s %d] Set Crop GrpNo(%d) x(%d) y(%d) width(%d) height(%d) success!\n", __func__, __LINE__,
					nVpssGrpNo, x, y, width, height);
	}
	
	return 0;
}

int vpss_get_crop(int nVpssGrpNo,int *nEnable,int *x,int *y,int *width,int *height)
{
	hi_s32 s32Ret = 0;
	hi_vpss_crop_info crop_info = {0};

	if(0 != (s32Ret = hi_mpi_vpss_get_grp_crop(nVpssGrpNo, &crop_info)))
	{
		printf("[%s %d] hi_mpi_vpss_get_grp_crop failed with with %d!\n", __func__, __LINE__, s32Ret);
		return -1;
	}

	*nEnable = crop_info.enable;
	*x = crop_info.crop_rect.x;
	*y = crop_info.crop_rect.y;
	*width = crop_info.crop_rect.width;
	*height = crop_info.crop_rect.height;

	return 0;
}

int get_vi_chn_pixel_fmt(int nViChn)
{
	if (nViChn != HI_VI_CHN)
	{
		printf("[%s %d] invalid nViChn(%d)!!\n", __func__, __LINE__, nViChn);
		return -1;
	}
	else
	{
		hi_s32 s32Ret = 0;
		hi_vi_chn_attr chn_attr;
		memset(&chn_attr, 0, sizeof(hi_vi_chn_attr));
		if (HI_SUCCESS != (s32Ret = hi_mpi_vi_get_chn_attr(0, nViChn, &chn_attr)))				
		{   
			printf("[%s %d] hi_mpi_vi_get_chn_attr err :%#x.\n", __func__, __LINE__, s32Ret);
			return s32Ret;
		} 
		return chn_attr.pixel_format;
	}
	return -1;
}

int get_vpss_get_mirror_flip(int *mirror, int *flip)
{
	int nRet = 0;
	GSF_IMA_CFG_S stIma = {0};
	if (0 == (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_IMA_PARAM, &stIma, sizeof(stIma))))
	{
		if (mirror)
			*mirror = stIma.struParam[VCT_IMA_IDX_MIRROR].u8Value;
		if (flip)
			*flip = stIma.struParam[VCT_IMA_IDX_FLIP].u8Value;
	}
	else
	{
		printf("[%s %d] get ima param err(%d)\n", __func__, __LINE__, nRet);
	}
	return 0;
}

int set_vpss_chn_config(int nVpssGrpNo, int nVpssChn, int nWidth, int nHeight)
{
	hi_s32 s32Ret = 0;
	if (nVpssChn >= HI_VPSS_MAX_PHYS_CHN_NUM)
	{
		printf("[%s %d] nVpssChn(%d) invalid!!\n", __func__, __LINE__, nVpssChn);
		return -1;
	}
	int mirror = 0, flip = 0;
	hi_vpss_chn_attr chn_attr = {0};
	get_vpss_get_mirror_flip(&mirror, &flip);
	chn_attr.chn_mode = OT_VPSS_CHN_MODE_USER;
	chn_attr.compress_mode = OT_COMPRESS_MODE_NONE;
	chn_attr.dynamic_range = OT_DYNAMIC_RANGE_SDR8;
	chn_attr.pixel_format = get_vi_chn_pixel_fmt(0);
	chn_attr.frame_rate.src_frame_rate  = -1;
	chn_attr.frame_rate.dst_frame_rate  = -1;
	chn_attr.depth                      = 0;
	chn_attr.mirror_en                  = mirror;
	chn_attr.flip_en                    = flip;
	chn_attr.video_format               = OT_VIDEO_FORMAT_LINEAR;
	chn_attr.aspect_ratio.mode          = OT_ASPECT_RATIO_NONE;
	chn_attr.width  = nWidth;
	chn_attr.height = nHeight;

	if ((s32Ret = hi_mpi_vpss_set_chn_attr(nVpssGrpNo, nVpssChn, &chn_attr)) != HI_SUCCESS)
	{
		printf("[%s %d] hi_mpi_vpss_set_chn_attr failed with %#x\n", __func__, __LINE__, s32Ret);
		return s32Ret;
	}
	printf("[%s %d] Set Phy Vpss grpNo(%d) chnNo(%d) Attr Ok\n", __func__, __LINE__, nVpssGrpNo, nVpssChn);
	return HI_SUCCESS;
}

int create_vpss_chn(int nVpssGrpNo, int nVpssChn, int nWidth, int nHeight,int nRotate)
{
	hi_s32 s32Ret = 0;
	if ((s32Ret = set_vpss_chn_config(nVpssGrpNo, nVpssChn, nWidth, nHeight)) != HI_SUCCESS)
	{
		return s32Ret;
	}
	if ((s32Ret = hi_mpi_vpss_enable_chn(nVpssGrpNo, nVpssChn)) != HI_SUCCESS)
	{
		printf("[%s %d] hi_mpi_vpss_enable_chn(%d, %d) failed with %#x\n", __func__, __LINE__, nVpssGrpNo, nVpssChn, s32Ret);
        return s32Ret;
	}
	if ((s32Ret = vpss_set_rotate(nVpssGrpNo, nVpssChn,nRotate)) != HI_SUCCESS)
	{
		printf("[%s %d] vpss_set_rotate %d, failed with %#x\n", __func__, __LINE__, nVpssGrpNo, nVpssChn, s32Ret);
        return s32Ret;
	}
	return HI_SUCCESS;
}

int release_vpss_chn(int nVpssGrpNo, int nVpssChn)
{
	return hi_mpi_vpss_disable_chn(nVpssGrpNo, nVpssChn);
}

int vpss_set_rotate(int nVpssGrpNo, int nVpssChn, ot_rotation nVal)
{
	int s32Ret = 0;

	hi_rotation_attr nRotate = {0};
	if ((s32Ret = hi_mpi_vpss_get_chn_rotation(nVpssGrpNo, nVpssChn, &nRotate)) != HI_SUCCESS)
	{
		printf("[%s %d] hi_mpi_vpss_get_chn_rotation(%d, %d) failed with %#x\n", __func__, __LINE__, nVpssGrpNo, nVpssChn, s32Ret);
        return s32Ret;
	}
	nRotate.enable = nVal > 0 ? 1 : 0;
	nRotate.rotation_type = OT_ROTATION_ANG_FIXED;
	if(nRotate.rotation_fixed == nVal)
		return HI_SUCCESS;
	if(nVal < 0 || nVal > OT_ROTATION_270)
		nVal = 0;
	
	nRotate.rotation_fixed = nVal;
	if ((s32Ret = hi_mpi_vpss_set_chn_rotation(nVpssGrpNo, nVpssChn, &nRotate)) != HI_SUCCESS)
	{
		printf("[%s %d] hi_mpi_vpss_set_chn_rotation(%d, %d) failed with %#x\n", __func__, __LINE__, nVpssGrpNo, nVpssChn, s32Ret);
        return s32Ret;
	}
	else
		printf("[%s %d] hi_mpi_vpss_set_chn_rotation(%d, %d) success with %#x\n", __func__, __LINE__, nVpssGrpNo, nVpssChn, s32Ret);
	
	return HI_SUCCESS;
}

