/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_vpss_yuv_rgb.c

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/08/10
  Description   :
  History       :
  1.Date        : 2021/08/10
    Author      :
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "hi_math.h"
#include <sys/time.h>
#include <stdlib.h>
#include "hi_common_vi.h"
#include "hi_common_vb.h"
#include "hi_common_vgs.h"
#include "hi_common_svp.h"
#include "hi_common_ive.h"
#include "hi_common_venc.h"
#include "hi_common_video.h"
#include "hi_mpi_vi.h"
#include "hi_mpi_vb.h"
#include "hi_mpi_vgs.h"
#include "hi_mpi_ive.h"
#include "hi_mpi_venc.h"
#include "hi_mpi_vpss.h"
#include "hi_mpi_sys.h"

#ifdef GSF3519DV5_ARCH
#include "hi_mpi_sys_mem.h"
#endif

#define MIN_WIDTH  (32)
#define MIN_HEIGHT (32)

typedef enum _GSF351X_FRAME_TYPE_E_
{
	GSF351X_FRAME_TYPE_YUV420  = 0,
	GSF351X_FRAME_TYPE_YUV422, 
	GSF351X_FRAME_TYPE_BGR,
}GSF351X_FRAME_TYPE_E;

/*typedef struct _GSF351X_VB_FRAME_S
{
	HI_U32 	u32VbBlk;
	HI_U32	u32FrameSize;
	HI_U32 	u32AlignWidth;
	HI_U32 	u32AlignHeight;
	VIDEO_FRAME_INFO_S stHiFrame;
}GSF351X_VB_FRAME_S;
*/

typedef struct hiDUMP_MEMBUF_S
{
    hi_vb_blk  hBlock;
    hi_vb_pool hPool;
    hi_u32  u32PoolId;
    hi_u64  u64PhyAddr;
    hi_u8*  pVirAddr;
    hi_s32  s32Mdev;
} DUMP_MEMBUF_S;

//static VB_CAL_CONFIG_S g_stCalConfig = {0};
static hi_vb_pool g_hPool = HI_VB_INVALID_POOL_ID;
static DUMP_MEMBUF_S g_stMem = {0};
static hi_video_frame_info g_stFrame = {0};
static hi_video_frame_info g_stYuvFrame = {0};

static hi_video_frame_info g_stFrame1 = {0};
static hi_u32 g_u32BlkSize = 0;
static int g_eFrmType = 0, g_nPlanar = 0, g_nWidth = 0, g_nHeight = 0;

static inline int VPSS_Restore()
{
	if (HI_VB_INVALID_POOL_ID != g_stMem.hPool)
    {
    	hi_mpi_vb_release_blk(g_stMem.hBlock);
        g_stMem.hPool = HI_VB_INVALID_POOL_ID;
    }

    if (HI_VB_INVALID_POOL_ID != g_hPool)
    {
    	hi_mpi_vb_destroy_pool(g_hPool);
        g_hPool = HI_VB_INVALID_POOL_ID;
    }
	return 0;
}

int vpss_yuv_rgb_uninit()
{
	if (HI_VB_INVALID_HANDLE != g_stMem.hBlock)
	{
		hi_mpi_vb_release_blk(g_stMem.hBlock);
	}
	g_stMem.hPool = HI_VB_INVALID_POOL_ID;

	if (HI_NULL != g_stMem.pVirAddr)
	{
		hi_mpi_sys_munmap((hi_void *)g_stMem.pVirAddr, g_u32BlkSize);
		g_stMem.u64PhyAddr = HI_NULL;
	}

	if (g_hPool != HI_VB_INVALID_POOL_ID)
	{
		hi_mpi_vb_destroy_pool(g_hPool);
		g_hPool = HI_VB_INVALID_POOL_ID;
	}
}

int vpss_yuv_rgb_init(hi_u32 u32Size)
{
	hi_vb_pool_cfg vb_pool_cfg;
	hi_u32 u32Width = 0;
    hi_u32 u32Height = 0;
    hi_u32 u32Align = 32;
	
	if (g_u32BlkSize > 0 && g_u32BlkSize == u32Size && HI_VB_INVALID_POOL_ID != g_hPool)
	{
		printf("[%s %d] g_u32BlkSize(%d) u32Size(%d) g_hPool(%d)\n", __func__, __LINE__, g_u32BlkSize, u32Size, g_hPool);
		return 0;
	}
	if (g_u32BlkSize > 0 && g_u32BlkSize != u32Size)
	{
		printf("[%s %d] g_u32BlkSize(%d) != u32Size(%d)\n", __func__, __LINE__, g_u32BlkSize, u32Size);
		vpss_yuv_rgb_uninit();
	}
	
	memset(&vb_pool_cfg, 0, sizeof(hi_vb_pool_cfg));
	vb_pool_cfg.blk_size  = u32Size;
	vb_pool_cfg.blk_cnt   = 1;
	vb_pool_cfg.remap_mode = OT_VB_REMAP_MODE_NONE;

	/*create comm vb pool*/
	g_hPool = hi_mpi_vb_create_pool(&vb_pool_cfg);
	if (HI_VB_INVALID_POOL_ID == g_hPool)
	{
		printf("hi_mpi_vb_create_pool failed! \n");
		VPSS_Restore();
		return HI_NULL;
	}

	g_stMem.hPool = g_hPool;

	while ((g_stMem.hBlock = hi_mpi_vb_get_blk(g_stMem.hPool, u32Size, HI_NULL)) == HI_VB_INVALID_HANDLE)
	{
		;
	}
	return 0;
}

int vpss_yuv_rgb_vb_alloc(int eFrmType, int nPlanar, int nWidth, int nHeight, hi_video_frame_info *pstVFrame)
{
	int    nSize = 0;
    hi_vb_blk VbBlk;
    
    if(nWidth <= MIN_WIDTH || nHeight <= MIN_HEIGHT || 
       0 != (nWidth % 16) || 0 != (nHeight % 2) || NULL == pstVFrame)
    {
       printf("[%s %d] input params invalid wh(%d, %d) frm_ptr(%p) \n", __func__, __LINE__, nWidth, nHeight, pstVFrame);
       return -1;
    }

	if (GSF351X_FRAME_TYPE_YUV420 == eFrmType)
	{
		nSize = nWidth * nHeight * 3 / 2;
	}
	else if (GSF351X_FRAME_TYPE_YUV422 == eFrmType)
	{
    	nSize = nWidth * nHeight * 2;
    }
    else
    {
		nSize = nWidth * nHeight * 3;
    }
	
	vpss_yuv_rgb_init(nSize);
	
	if (g_stMem.hBlock == HI_VB_INVALID_HANDLE)
	{ 
		printf("[%s %d] rgb init err! size:%d, wxh[%d %d]\n", __func__, __LINE__, nSize, nWidth, nHeight);
        return -1;
	}
	if (g_u32BlkSize > 0 && g_nWidth > 0 && g_nHeight > 0 && g_eFrmType == eFrmType && g_nPlanar == nPlanar)
	{
		return 0;
	}
	
	g_nWidth = nWidth;
	g_nHeight = nHeight;
	g_eFrmType = eFrmType; 
	g_nPlanar = nPlanar;
	
	VbBlk = g_stMem.hBlock;
	
    pstVFrame->mod_id = OT_ID_USER;
    pstVFrame->video_frame.width = nWidth;
    pstVFrame->video_frame.height = nHeight;
    pstVFrame->video_frame.dynamic_range = OT_DYNAMIC_RANGE_SDR8;
    pstVFrame->video_frame.compress_mode = OT_COMPRESS_MODE_NONE;
    if (GSF351X_FRAME_TYPE_YUV420 == eFrmType)
    {
		pstVFrame->video_frame.pixel_format = (0 == nPlanar) ? OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 : OT_PIXEL_FORMAT_YVU_PLANAR_420;
    }
    else if (GSF351X_FRAME_TYPE_YUV422 == eFrmType)
    {
		pstVFrame->video_frame.pixel_format = (0 == nPlanar) ? OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422 : OT_PIXEL_FORMAT_YVU_PLANAR_422;
    }
    else
    {
		pstVFrame->video_frame.pixel_format = (0 == nPlanar)  ? OT_PIXEL_FORMAT_BGR_888_PLANAR : OT_PIXEL_FORMAT_BGR_888;
    }
    pstVFrame->video_frame.video_format = OT_VIDEO_FORMAT_LINEAR;
    pstVFrame->video_frame.field = OT_VIDEO_FIELD_FRAME;
    pstVFrame->video_frame.color_gamut = OT_COLOR_GAMUT_BT709;
    //pstVFrame->video_frame.u32MaxLuminance     = 1200;
    //pstVFrame->video_frame.u32MinLuminance     = 200;	

    pstVFrame->video_frame.header_phys_addr[0] = hi_mpi_vb_handle_to_phys_addr(VbBlk);
    pstVFrame->video_frame.header_phys_addr[1] = pstVFrame->video_frame.header_phys_addr[0];
    pstVFrame->video_frame.header_phys_addr[2] = pstVFrame->video_frame.header_phys_addr[1];
    		
    pstVFrame->video_frame.header_virt_addr[0] = (hi_u64 *)hi_mpi_sys_mmap(pstVFrame->video_frame.header_phys_addr[0], nSize);
    pstVFrame->video_frame.header_virt_addr[1] = pstVFrame->video_frame.header_virt_addr[0];
    pstVFrame->video_frame.header_virt_addr[2] = pstVFrame->video_frame.header_virt_addr[1];
    pstVFrame->video_frame.header_stride[0]  = 0;
    pstVFrame->video_frame.header_stride[1]  = 0;
    pstVFrame->video_frame.header_stride[2]  = 0;

    pstVFrame->video_frame.phys_addr[0] = pstVFrame->video_frame.header_phys_addr[0];
    pstVFrame->video_frame.virt_addr[0] = pstVFrame->video_frame.header_virt_addr[0];
    if (GSF351X_FRAME_TYPE_YUV420 == eFrmType || 
    	GSF351X_FRAME_TYPE_YUV422 == eFrmType)
    {
	    pstVFrame->video_frame.phys_addr[1] = pstVFrame->video_frame.phys_addr[0] + nWidth*nHeight;
	    pstVFrame->video_frame.virt_addr[1] = pstVFrame->video_frame.virt_addr[0] + nWidth*nHeight;
	    		
	    if (nPlanar)
	    {
	    	if (GSF351X_FRAME_TYPE_YUV420 == eFrmType)
	    	{
		    	pstVFrame->video_frame.phys_addr[2] = pstVFrame->video_frame.phys_addr[1] + nWidth*nHeight / 4;
		    	pstVFrame->video_frame.virt_addr[2] = pstVFrame->video_frame.virt_addr[1] + nWidth*nHeight / 4;				
		    }
		    else
		    {
				pstVFrame->video_frame.phys_addr[2] = pstVFrame->video_frame.phys_addr[1] + nWidth*nHeight / 2;
				pstVFrame->video_frame.virt_addr[2] = pstVFrame->video_frame.virt_addr[1] + nWidth*nHeight / 2;
		    }
		}
		else
		{
			pstVFrame->video_frame.phys_addr[2] = pstVFrame->video_frame.phys_addr[1];
			pstVFrame->video_frame.virt_addr[2] = pstVFrame->video_frame.virt_addr[1];	
		}
	}
	else
	{
		if (nPlanar)
		{
			pstVFrame->video_frame.phys_addr[1] = pstVFrame->video_frame.phys_addr[0] + nWidth*nHeight;    		
    		pstVFrame->video_frame.phys_addr[2] = pstVFrame->video_frame.phys_addr[1] + nWidth*nHeight;

			pstVFrame->video_frame.virt_addr[1] = pstVFrame->video_frame.virt_addr[0] + nWidth*nHeight;    		
    		pstVFrame->video_frame.virt_addr[2] = pstVFrame->video_frame.virt_addr[1] + nWidth*nHeight;    			
    	}
    	else
    	{
			pstVFrame->video_frame.phys_addr[1] = pstVFrame->video_frame.phys_addr[0];
			pstVFrame->video_frame.phys_addr[2] = pstVFrame->video_frame.phys_addr[1];

			pstVFrame->video_frame.virt_addr[1] = pstVFrame->video_frame.virt_addr[0];
			pstVFrame->video_frame.virt_addr[2] = pstVFrame->video_frame.virt_addr[1];
    	}
	}
	
    pstVFrame->video_frame.stride[0]  = nWidth;
    pstVFrame->video_frame.stride[1]  = nWidth;
    pstVFrame->video_frame.stride[2]  = nWidth;

    /*pstVFrame->video_frame.u64ExtPhyAddr[0] = pstVFrame->video_frame.u64PhyAddr[0] + nSize;
    pstVFrame->video_frame.u64ExtPhyAddr[1] = pstVFrame->video_frame.u64ExtPhyAddr[0];
    pstVFrame->video_frame.u64ExtPhyAddr[2] = pstVFrame->video_frame.u64ExtPhyAddr[1];
    pstVFrame->video_frame.u64ExtVirAddr[1] = pstVFrame->video_frame.u64ExtVirAddr[0];
    pstVFrame->video_frame.u64ExtVirAddr[2] = pstVFrame->video_frame.u64ExtVirAddr[1];
    pstVFrame->video_frame.u32ExtStride[0]  = 0;
    pstVFrame->video_frame.u32ExtStride[1]  = 0;
    pstVFrame->video_frame.u32ExtStride[2]  = 0;*/
}

hi_video_frame_info *vpss_yuv_YUV420SP_to_BGR_P(int nVpssChn,hi_video_frame_info *pstSrcFrm)
{
    hi_s32 s32Ret = 0;
    hi_bool bFinish = HI_FALSE;
    hi_ive_handle handle;
    hi_svp_src_img src;
    hi_svp_dst_img dst;
    hi_ive_csc_ctrl ctrl; 
    hi_video_frame_info *pstDstFrm = NULL;
	if(nVpssChn == 1)	//次码流
		pstDstFrm = &g_stFrame1;
	else
		pstDstFrm = &g_stFrame;
	int nOutputType = OT_SVP_IMG_TYPE_U8C3_PACKAGE;  // 交错格式: BGRBGRBGR
	//int nOutputType = IVE_IMAGE_TYPE_U8C3_PLANAR;  // 平面格式: BBBGGGRRR
	
	if ((pstSrcFrm->video_frame.width != pstDstFrm->video_frame.width) ||
        (pstSrcFrm->video_frame.height != pstDstFrm->video_frame.height))
    {
		printf("[%s %d] src-frame(%d %d) != dest-frame(%d %d), vb_alloc ...\n", __func__, __LINE__, 
			pstSrcFrm->video_frame.width, pstSrcFrm->video_frame.height, 
			pstDstFrm->video_frame.width, pstDstFrm->video_frame.height);
		
		vpss_yuv_rgb_vb_alloc(GSF351X_FRAME_TYPE_BGR, 1, pstSrcFrm->video_frame.width, pstSrcFrm->video_frame.height, pstDstFrm);
	}

    if ((pstSrcFrm->video_frame.width != pstDstFrm->video_frame.width) ||
        (pstSrcFrm->video_frame.height != pstDstFrm->video_frame.height))
    {
        printf("[%s %d] WH NOT THE SAME, src wh(%u, %u) dst wh(%u, %u).\n", __func__, __LINE__, 
               pstSrcFrm->video_frame.width, pstSrcFrm->video_frame.height, 
               pstDstFrm->video_frame.width, pstDstFrm->video_frame.height);
        return NULL;
        
    }

	if (/*PIXEL_FORMAT_YVU_PLANAR_422 != pstSrcFrm->stVFrame.enPixelFormat && 
		PIXEL_FORMAT_YVU_PLANAR_420 != pstSrcFrm->stVFrame.enPixelFormat && 
		PIXEL_FORMAT_YVU_SEMIPLANAR_422 != pstSrcFrm->stVFrame.enPixelFormat && 
		*/OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 != pstSrcFrm->video_frame.pixel_format)
	{
		printf("[%s %d] src pixel format(%d) not support\n", __func__, __LINE__, pstSrcFrm->video_frame.pixel_format);

		return NULL;
	}
	src.type		= OT_SVP_IMG_TYPE_YUV420SP;
	src.width		= pstSrcFrm->video_frame.width;
	src.height		= pstSrcFrm->video_frame.height;

    src.stride[0]  = pstSrcFrm->video_frame.stride[0];
    src.stride[1]  = src.stride[0];    
    src.stride[2]  = pstSrcFrm->video_frame.stride[2];

    src.phys_addr[0] = pstSrcFrm->video_frame.phys_addr[0];    	
    src.phys_addr[1] = pstSrcFrm->video_frame.phys_addr[1];
    src.phys_addr[2] = pstSrcFrm->video_frame.phys_addr[2];

    src.virt_addr[0] = (hi_u64)pstSrcFrm->video_frame.virt_addr[0];
    src.virt_addr[1] = (hi_u64)pstSrcFrm->video_frame.virt_addr[1];
    src.virt_addr[2] = (hi_u64)pstSrcFrm->video_frame.virt_addr[2];

    dst.type         = nOutputType; 
    dst.width        = pstDstFrm->video_frame.width;
    dst.height       = pstDstFrm->video_frame.height;

	dst.stride[0]    = pstDstFrm->video_frame.stride[0];
	dst.stride[1]    = dst.stride[0];
	dst.stride[2]    = dst.stride[1];

	dst.phys_addr[0] = pstDstFrm->video_frame.phys_addr[0];
	dst.phys_addr[1] = pstDstFrm->video_frame.phys_addr[1];
    dst.phys_addr[2] = pstDstFrm->video_frame.phys_addr[2];

    dst.virt_addr[0] = (hi_u64)pstDstFrm->video_frame.virt_addr[0];
    dst.virt_addr[1] = (hi_u64)pstDstFrm->video_frame.virt_addr[1];
    dst.virt_addr[2] = (hi_u64)pstDstFrm->video_frame.virt_addr[2];
    
    ctrl.mode = OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB;
    
    s32Ret = hi_mpi_ive_csc(&handle, &src, &dst, &ctrl, HI_FALSE);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[%s %d] hi_mpi_ive_csc ret(%#x).\n", __func__, __LINE__, s32Ret);
        return NULL;
    }

	s32Ret = hi_mpi_ive_query(handle,&bFinish, HI_TRUE);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[%s %d] hi_mpi_ive_query ret(%#x).\n", __func__, __LINE__, s32Ret);
        return NULL;
    }
#if 0
    if (1)
    {
		HI_U64 u64Pts[2] = {0};
    	static int s_iSaveCnt = 0;
		char szFileName[64] = {0};
		FILE *fp = NULL;
		
		if (s_iSaveCnt <= 5)
			HI_MPI_SYS_GetCurPTS(&u64Pts[0]);
	#if 1
		sprintf(szFileName, "/mnt/rgb/pic_%d.bmp", s_iSaveCnt);
        if (s_iSaveCnt <= 4)
        {
			SaveBgrToBmpFile(szFileName, (unsigned char *)(HI_U32)stHiImageDst.au64VirAddr[0], 
							pstSrcFrm->stVFrame.u32Width, pstSrcFrm->stVFrame.u32Height);
        }
		memset(szFileName, 0, sizeof(szFileName));
        sprintf(szFileName, "/mnt/rgb/bgr_%d_package.bgr", s_iSaveCnt++);
        if (s_iSaveCnt <= 5 && NULL != (fp = fopen(szFileName, "wb+")))
        {
            if (1 != fwrite((void *)(HI_U32)stHiImageDst.au64VirAddr[0], 
							 pstSrcFrm->stVFrame.u32Width*pstSrcFrm->stVFrame.u32Height*3, 1, fp))
            {
                printf("[%s %d] write dst file data failed.\n", __func__, __LINE__);
            }

            fclose(fp);
            fp = NULL;
        }
		else if (NULL == fp && s_iSaveCnt <= 5)
		{
			printf("[%s %d] fopen file(%s) err %d(%s)\n", __func__, __LINE__, szFileName, errno, strerror(errno));
		}
	#endif
		if (s_iSaveCnt <= 6)
		{
			HI_MPI_SYS_GetCurPTS(&u64Pts[1]);
			if (u64Pts[1] - u64Pts[0] > 5000)
			{
				printf("[%s %d] write file data cost(%llu)us\n", __func__, __LINE__, u64Pts[1] - u64Pts[0]);
			}
		}
    }
#endif    
    return pstDstFrm;
}

int vpss_copy_data(int nVpssChn, hi_video_frame_info *pstSrcFrm,hi_video_frame_info *pstDstFrm)
{
	hi_s32 s32Ret = 0;
    hi_bool bFinish = HI_FALSE;
	hi_ive_handle handle;
	hi_svp_data src;
	hi_svp_dst_data dst;
	hi_ive_dma_ctrl ctrl;

	src.height = pstSrcFrm->video_frame.height;
	src.width = pstSrcFrm->video_frame.width;
	src.stride = pstSrcFrm->video_frame.stride[0];
    src.phys_addr = pstSrcFrm->video_frame.phys_addr[0];    	
    src.virt_addr = (hi_u64)pstSrcFrm->video_frame.virt_addr[0];

	dst.height = pstDstFrm->video_frame.height;
	dst.width = pstDstFrm->video_frame.width;
	dst.stride = pstDstFrm->video_frame.stride[0];
    dst.phys_addr = pstDstFrm->video_frame.phys_addr[0];    	
    dst.virt_addr = (hi_u64)pstDstFrm->video_frame.virt_addr[0];
	
	ctrl.mode = OT_IVE_DMA_MODE_DIRECT_COPY;

	s32Ret = hi_mpi_ive_dma(&handle, &src, &dst, &ctrl, HI_FALSE);
	if (HI_SUCCESS != s32Ret)
    {
        printf("[%s %d] hi_mpi_ive_csc ret(%#x).\n", __func__, __LINE__, s32Ret);
        return -1;
    }

	s32Ret = hi_mpi_ive_query(handle,&bFinish, HI_TRUE);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[%s %d] hi_mpi_ive_query ret(%#x).\n", __func__, __LINE__, s32Ret);
       	return -1;
    }
	return 0;
}
hi_video_frame_info *vpss_bgr_to_yuv_YUV420SP_P(int nVpssChn,hi_video_frame_info *pstSrcFrm)
{
    hi_s32 s32Ret = 0;
    hi_bool bFinish = HI_FALSE;
    hi_ive_handle handle;
    hi_svp_src_img src;
    hi_svp_dst_img dst;
    hi_ive_csc_ctrl ctrl; 
    hi_video_frame_info *pstDstFrm = NULL;
	if(nVpssChn == 1)	//次码流
		pstDstFrm = &g_stFrame1;
	else
		pstDstFrm = &g_stYuvFrame;


	if ((pstSrcFrm->video_frame.width != pstDstFrm->video_frame.width) ||
        (pstSrcFrm->video_frame.height != pstDstFrm->video_frame.height))
    {
		printf("[%s %d] src-frame(%d %d) != dest-frame(%d %d), vb_alloc ...\n", __func__, __LINE__, 
			pstSrcFrm->video_frame.width, pstSrcFrm->video_frame.height, 
			pstDstFrm->video_frame.width, pstDstFrm->video_frame.height);
		
		vpss_yuv_rgb_vb_alloc(GSF351X_FRAME_TYPE_YUV420, 0, pstSrcFrm->video_frame.width, pstSrcFrm->video_frame.height, pstDstFrm);
	}

	if ((pstSrcFrm->video_frame.width != pstDstFrm->video_frame.width) ||
		(pstSrcFrm->video_frame.height != pstDstFrm->video_frame.height))
	{
		printf("[%s %d] WH NOT THE SAME, src wh(%u, %u) dst wh(%u, %u).\n", __func__, __LINE__, 
			   pstSrcFrm->video_frame.width, pstSrcFrm->video_frame.height, 
			   pstDstFrm->video_frame.width, pstDstFrm->video_frame.height);
		return NULL;
		
	}

	if(OT_PIXEL_FORMAT_BGR_888 != pstSrcFrm->video_frame.pixel_format)
	{
		printf("[%s %d] src pixel format(%d) not support\n", __func__, __LINE__, pstSrcFrm->video_frame.pixel_format);

		return NULL;
	}
	
	src.type		= OT_SVP_IMG_TYPE_U8C3_PACKAGE;
	src.width		= pstSrcFrm->video_frame.width;
	src.height		= pstSrcFrm->video_frame.height;

    src.stride[0]  = pstSrcFrm->video_frame.stride[0];
    src.stride[1]  = src.stride[0];    
    src.stride[2]  = pstSrcFrm->video_frame.stride[2];

    src.phys_addr[0] = pstSrcFrm->video_frame.phys_addr[0];    	
    src.phys_addr[1] = pstSrcFrm->video_frame.phys_addr[1];
    src.phys_addr[2] = pstSrcFrm->video_frame.phys_addr[2];

    src.virt_addr[0] = (hi_u64)pstSrcFrm->video_frame.virt_addr[0];
    src.virt_addr[1] = (hi_u64)pstSrcFrm->video_frame.virt_addr[1];
    src.virt_addr[2] = (hi_u64)pstSrcFrm->video_frame.virt_addr[2];

    dst.type         = OT_SVP_IMG_TYPE_YUV420SP; 
    dst.width        = pstDstFrm->video_frame.width;
    dst.height       = pstDstFrm->video_frame.height;

	dst.stride[0]    = pstDstFrm->video_frame.stride[0];
	dst.stride[1]    = dst.stride[0];
	dst.stride[2]    = dst.stride[1];

	dst.phys_addr[0] = pstDstFrm->video_frame.phys_addr[0];
	dst.phys_addr[1] = pstDstFrm->video_frame.phys_addr[1];
    dst.phys_addr[2] = pstDstFrm->video_frame.phys_addr[2];

    dst.virt_addr[0] = (hi_u64)pstDstFrm->video_frame.virt_addr[0];
    dst.virt_addr[1] = (hi_u64)pstDstFrm->video_frame.virt_addr[1];
    dst.virt_addr[2] = (hi_u64)pstDstFrm->video_frame.virt_addr[2];
    
    ctrl.mode = OT_IVE_CSC_MODE_VIDEO_BT601_RGB_TO_YUV;
    
    s32Ret = hi_mpi_ive_csc(&handle, &src, &dst, &ctrl, HI_FALSE);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[%s %d] hi_mpi_ive_csc ret(%#x).\n", __func__, __LINE__, s32Ret);
        return NULL;
    }
#if 1
	s32Ret = hi_mpi_ive_query(handle,&bFinish, HI_TRUE);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[%s %d] hi_mpi_ive_query ret(%#x).\n", __func__, __LINE__, s32Ret);
        return NULL;
    }
#endif
	return pstDstFrm;
}

