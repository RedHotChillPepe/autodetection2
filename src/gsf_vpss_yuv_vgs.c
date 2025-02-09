/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_base.h

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/08/10
  Description   :
  History       :
  1.Date        : 2021/08/10
    Author      :
    Modification: Created file

******************************************************************************/
// 1. HI_S32 HI_MPI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
// 2. HI_S32 HI_MPI_VENC_SendFrame(VENC_CHN VeChn, const VIDEO_FRAME_INFO_S *pstFrame, HI_S32 s32MilliSec);
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
#include "hi_buffer.h"
#include "hi_common_region.h"
#include "hi_mpi_region.h"
#include "gsf_font_draw.h"
#include "devsdk.h"

#ifdef GSF3519DV5_ARCH
#include "hi_mpi_sys_mem.h"
#endif

#define HI_EXPECTED_WIDTH  416
#define HI_EXPECTED_HEIGHT 416

typedef struct hiDUMP_MEMBUF_S
{
    hi_vb_blk hBlock;
    hi_vb_pool hPool;
    hi_u32  u32PoolId;
    hi_u64  u64PhyAddr;
    hi_u8*  pVirAddr;
    hi_s32  s32Mdev;
} DUMP_MEMBUF_S;

typedef struct hiRGN_DATA_S
{
	int nRgnCreateFlag;
	int nWidth;
	int nHeight;
}RGN_DATA_S;

static hi_vb_calc_cfg g_stCalConfig = {0};
static hi_vb_pool g_hPool = HI_VB_INVALID_POOL_ID;
static DUMP_MEMBUF_S g_stMem = {0};
//static VGS_HANDLE g_hHandle = -1;
static hi_u32 g_u32BlkSize = 0;

static hi_vb_calc_cfg g_stCropCalConfig = {0};
static hi_vb_pool g_hCropPool = HI_VB_INVALID_POOL_ID;
static DUMP_MEMBUF_S g_stCropMem = {0};
static hi_u32 g_u32CropBlkSize = 0;

//static int nRgnCreatFlag[32] = {0};
static RGN_DATA_S g_stRgnData[32] = {0};

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

int vpss_yuv_vgs_init(hi_video_frame_info *pstVideo)
{
	hi_vb_pool_cfg vb_pool_cfg = {0};
	hi_u32 u32Width = 0;
    hi_u32 u32Height = 0;
    hi_u32 u32Align = OT_DEFAULT_ALIGN;
    hi_pixel_format enPixelFormat;
    hi_data_bit_width enBitWidth;
    hi_compress_mode enCmpMode;

    hi_pic_buf_attr buf_attr = {0};
	
	if (g_u32BlkSize > 0 && HI_VB_INVALID_POOL_ID != g_hPool)
	{
		return 0;
	}
	
	enCmpMode = OT_COMPRESS_MODE_NONE;
	enPixelFormat = pstVideo->video_frame.pixel_format;
	enBitWidth = (OT_DYNAMIC_RANGE_SDR8 == pstVideo->video_frame.dynamic_range) ? OT_DATA_BIT_WIDTH_8 : OT_DATA_BIT_WIDTH_16;
	u32Width  = pstVideo->video_frame.width;
	u32Height = pstVideo->video_frame.height;

	buf_attr.width = u32Width;
	buf_attr.height = u32Height;
	buf_attr.pixel_format = enPixelFormat;
	buf_attr.bit_width = enBitWidth;
	buf_attr.compress_mode = enCmpMode;
	buf_attr.align = u32Align;

	hi_common_get_pic_buf_cfg(&buf_attr,&g_stCalConfig);

	g_u32BlkSize = g_stCalConfig.vb_size;

	memset(&vb_pool_cfg, 0, sizeof(hi_vb_pool_cfg));
	vb_pool_cfg.blk_size  = g_u32BlkSize;
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

	while ((g_stMem.hBlock = hi_mpi_vb_get_blk(g_stMem.hPool, g_u32BlkSize, HI_NULL)) == HI_VB_INVALID_HANDLE)
	{
		;
	}

	g_stMem.u64PhyAddr = hi_mpi_vb_handle_to_phys_addr(g_stMem.hBlock);

	g_stMem.pVirAddr = (hi_u8 *)hi_mpi_sys_mmap(g_stMem.u64PhyAddr, g_u32BlkSize);
	if (HI_NULL == g_stMem.pVirAddr)
	{
		printf("Mem dev may not open\n");
		VPSS_Restore();
		return HI_NULL;
	}
	return 0;
}

int vpss_yuv_vgs_crop_init(hi_video_frame_info *pstVideo)
{
	hi_vb_pool_cfg vb_pool_cfg = {0};
	hi_u32 u32Width = 0;
    hi_u32 u32Height = 0;
    hi_u32 u32Align = OT_DEFAULT_ALIGN;
    hi_pixel_format enPixelFormat;
    hi_data_bit_width enBitWidth;
    hi_compress_mode enCmpMode;

    hi_pic_buf_attr buf_attr = {0};
	
	if (g_u32CropBlkSize > 0 && HI_VB_INVALID_POOL_ID != g_hCropPool)
	{
		return 0;
	}
	
	enCmpMode = OT_COMPRESS_MODE_NONE;
	enPixelFormat = pstVideo->video_frame.pixel_format;
	enBitWidth = (OT_DYNAMIC_RANGE_SDR8 == pstVideo->video_frame.dynamic_range) ? OT_DATA_BIT_WIDTH_8 : OT_DATA_BIT_WIDTH_16;
	u32Width  = pstVideo->video_frame.width;
	u32Height = pstVideo->video_frame.height;

	buf_attr.width = u32Width;
	buf_attr.height = u32Height;
	buf_attr.pixel_format = enPixelFormat;
	buf_attr.bit_width = enBitWidth;
	buf_attr.compress_mode = enCmpMode;
	buf_attr.align = u32Align;

	hi_common_get_pic_buf_cfg(&buf_attr,&g_stCropCalConfig);

	g_u32CropBlkSize = g_stCropCalConfig.vb_size;

	memset(&vb_pool_cfg, 0, sizeof(hi_vb_pool_cfg));
	vb_pool_cfg.blk_size  = g_u32CropBlkSize;
	vb_pool_cfg.blk_cnt   = 1;
	vb_pool_cfg.remap_mode = OT_VB_REMAP_MODE_NONE;

	/*create comm vb pool*/
	g_hCropPool = hi_mpi_vb_create_pool(&vb_pool_cfg);
	if (HI_VB_INVALID_POOL_ID == g_hCropPool)
	{
		printf("hi_mpi_vb_create_pool failed! \n");
		VPSS_Restore();
		return HI_NULL;
	}

	g_stCropMem.hPool = g_hCropPool;

	while ((g_stCropMem.hBlock = hi_mpi_vb_get_blk(g_stCropMem.hPool, g_u32CropBlkSize, HI_NULL)) == HI_VB_INVALID_HANDLE)
	{
		;
	}

	g_stCropMem.u64PhyAddr = hi_mpi_vb_handle_to_phys_addr(g_stCropMem.hBlock);

	g_stCropMem.pVirAddr = (hi_u8 *)hi_mpi_sys_mmap(g_stCropMem.u64PhyAddr, g_u32CropBlkSize);
	if (HI_NULL == g_stCropMem.pVirAddr)
	{
		printf("Mem dev may not open\n");
		VPSS_Restore();
		return HI_NULL;
	}
	return 0;
}


int vpss_yuv_vgs_uninit()
{
	if (HI_VB_INVALID_HANDLE != g_stMem.hBlock)
	{
		hi_mpi_vb_release_blk(g_stMem.hBlock);
	}
	g_stMem.hPool = HI_VB_INVALID_POOL_ID;

	if (HI_NULL != g_stMem.pVirAddr)
	{
		hi_mpi_sys_munmap((hi_void*)g_stMem.pVirAddr, g_u32BlkSize);
		g_stMem.u64PhyAddr = HI_NULL;
	}

	if (g_hPool != HI_VB_INVALID_POOL_ID)
	{
		hi_mpi_vb_destroy_pool(g_hPool);
		g_hPool = HI_VB_INVALID_POOL_ID;
	}

	if (HI_VB_INVALID_HANDLE != g_stCropMem.hBlock)
	{
		hi_mpi_vb_release_blk(g_stCropMem.hBlock);
	}
	g_stCropMem.hPool = HI_VB_INVALID_POOL_ID;

	if (HI_NULL != g_stCropMem.pVirAddr)
	{
		hi_mpi_sys_munmap((hi_void*)g_stCropMem.pVirAddr, g_u32CropBlkSize);
		g_stCropMem.u64PhyAddr = HI_NULL;
	}

	if (g_hCropPool != HI_VB_INVALID_POOL_ID)
	{
		hi_mpi_vb_destroy_pool(g_hCropPool);
		g_hCropPool = HI_VB_INVALID_POOL_ID;
	}

	int i = 0,nHandle = 0;

	for(i = 0; i < 32; i++)
	{
		if(g_stRgnData[i].nRgnCreateFlag > 0)
		{
			nHandle = 140 + i;
			hi_mpi_rgn_destroy(nHandle);
		}
	}
	gsf_freetype_uninit();
}

int vpss_yuv_vgs_proc(hi_video_frame_info *pstInVideo, hi_video_frame_info *pstOutVideo, hi_u32 u32Width, hi_u32 u32Height)
{
	if(u32Width%2 != 0 || u32Height%2 != 0)
	{
		printf("[%s %d]vpss_yuv_vgs_proc parameter setting error\n",__func__,__LINE__);
		return -1;
	}
	hi_s32 s32Ret = 0;
	hi_vgs_task_attr task;
	hi_vgs_handle g_hHandle = -1;
	vpss_yuv_vgs_init(pstInVideo);
	//����ü��ӿ�ͬʱʹ�����ȵ��õĽӿڳ�ʼ����һ�ӿ�vb
	//vpss_yuv_vgs_crop_init(pstInVideo);
	int nMainsize = u32Width*u32Height;
	memset(pstOutVideo, 0, sizeof(hi_video_frame_info));
	pstOutVideo->video_frame.phys_addr[0] = g_stMem.u64PhyAddr;
	pstOutVideo->video_frame.phys_addr[1] = pstOutVideo->video_frame.phys_addr[0] + nMainsize;//g_stCalConfig.main_y_size;
	pstOutVideo->video_frame.phys_addr[2] = pstOutVideo->video_frame.phys_addr[1] + (nMainsize>>1);
	pstOutVideo->video_frame.virt_addr[0] = (hi_u64 *)g_stMem.pVirAddr;
	pstOutVideo->video_frame.virt_addr[1] = pstOutVideo->video_frame.virt_addr[0] + nMainsize;//g_stCalConfig.main_y_size;
	pstOutVideo->video_frame.virt_addr[2] = pstOutVideo->video_frame.virt_addr[1] + (nMainsize>>1);
	pstOutVideo->video_frame.width = u32Width;
	pstOutVideo->video_frame.height = u32Height;
	pstOutVideo->video_frame.stride[0] = HI_ALIGN_DOWN(u32Width,16);//g_stCalConfig.main_stride;
	pstOutVideo->video_frame.stride[1] = HI_ALIGN_DOWN(u32Width,16);//g_stCalConfig.main_stride;
	pstOutVideo->video_frame.stride[2] = HI_ALIGN_DOWN(u32Width,16);

	pstOutVideo->video_frame.compress_mode = OT_COMPRESS_MODE_NONE;
	pstOutVideo->video_frame.pixel_format = pstInVideo->video_frame.pixel_format;
	pstOutVideo->video_frame.video_format = OT_VIDEO_FORMAT_LINEAR;
	pstOutVideo->video_frame.dynamic_range = pstInVideo->video_frame.dynamic_range;

	pstOutVideo->video_frame.pts = pstInVideo->video_frame.pts;
	pstOutVideo->video_frame.time_ref = pstInVideo->video_frame.time_ref;

	pstOutVideo->pool_id = g_hPool;
	pstOutVideo->mod_id = HI_ID_VGS;

	s32Ret = hi_mpi_vgs_begin_job(&g_hHandle);
	if (s32Ret != HI_SUCCESS)
	{
		printf("hi_mpi_vgs_begin_job failed\n");
		g_hHandle = -1;
		return s32Ret;
	}

	memcpy(&task.img_in, pstInVideo, sizeof(hi_video_frame_info));
	memcpy(&task.img_out, pstOutVideo, sizeof(hi_video_frame_info));
	s32Ret = hi_mpi_vgs_add_scale_task(g_hHandle, &task, OT_VGS_SCALE_COEF_NORM);
	if (s32Ret != HI_SUCCESS)
	{
		printf("hi_mpi_vgs_add_scale_task failed\n");
		hi_mpi_vgs_cancel_job(g_hHandle);
        g_hHandle = -1;
		return s32Ret;
	}

	s32Ret = hi_mpi_vgs_end_job(g_hHandle);
	if (s32Ret != HI_SUCCESS)
	{
		printf("hi_mpi_vgs_end_job failed\n");
		hi_mpi_vgs_cancel_job(g_hHandle);
        g_hHandle = -1;
		return s32Ret;
	}
	g_hHandle = -1;
	memcpy(pstOutVideo, &task.img_out, sizeof(hi_video_frame_info));
	return HI_SUCCESS;
}

int vpss_yuv_vgs_crop_proc(hi_video_frame_info *pstInVideo, hi_video_frame_info *pstOutVideo, hi_rect *pRect)
{
	if( pRect->x % 2 != 0 || pRect->y % 2 != 0 ||
		pRect->width % 2 != 0 || pRect->height % 2 != 0 ||
		pRect->width < 32 || pRect->width > pstInVideo->video_frame.width ||
		pRect->height < 32 || pRect->height > pstInVideo->video_frame.height)
	{
		printf("[%s %d]vpss_yuv_vgs_crop_proc parameter setting error\n",__func__,__LINE__);
		return -1;
	}
	hi_s32 s32Ret = 0;
	hi_vgs_task_attr task = {0};
	hi_vgs_online online = {0};
	hi_vgs_handle g_hHandle = -1;
	//�������Žӿ�ͬʱʹ�����ȵ��õĽӿڳ�ʼ����һ�ӿ�vb
	//vpss_yuv_vgs_init(pstInVideo);
	vpss_yuv_vgs_crop_init(pstInVideo);
	int nMainsize = pRect->width*pRect->height;
	memset(pstOutVideo, 0, sizeof(hi_video_frame_info));
	pstOutVideo->video_frame.phys_addr[0] = g_stCropMem.u64PhyAddr;
	pstOutVideo->video_frame.phys_addr[1] = pstOutVideo->video_frame.phys_addr[0] + nMainsize;
	pstOutVideo->video_frame.phys_addr[2] = pstOutVideo->video_frame.phys_addr[1] + (nMainsize>>1);
	pstOutVideo->video_frame.virt_addr[0] = (hi_u64 *)g_stCropMem.pVirAddr;
	pstOutVideo->video_frame.virt_addr[1] = pstOutVideo->video_frame.virt_addr[0] + nMainsize;
	pstOutVideo->video_frame.virt_addr[2] = pstOutVideo->video_frame.virt_addr[1] + (nMainsize>>1);
	pstOutVideo->video_frame.width = pRect->width;
	pstOutVideo->video_frame.height = pRect->height;
	pstOutVideo->video_frame.stride[0] = HI_ALIGN_DOWN(pRect->width,16);
	pstOutVideo->video_frame.stride[1] = HI_ALIGN_DOWN(pRect->width,16);
	pstOutVideo->video_frame.stride[2] = HI_ALIGN_DOWN(pRect->width,16);

	pstOutVideo->video_frame.compress_mode = OT_COMPRESS_MODE_NONE;
	pstOutVideo->video_frame.pixel_format = pstInVideo->video_frame.pixel_format;
	pstOutVideo->video_frame.video_format = OT_VIDEO_FORMAT_LINEAR;
	pstOutVideo->video_frame.dynamic_range = pstInVideo->video_frame.dynamic_range;

	pstOutVideo->video_frame.pts = pstInVideo->video_frame.pts;
	pstOutVideo->video_frame.time_ref = pstInVideo->video_frame.time_ref;

	pstOutVideo->pool_id = g_hCropPool;
	pstOutVideo->mod_id = HI_ID_VGS;

	online.crop_en = HI_TRUE;
	online.crop_rect.x = pRect->x;
	online.crop_rect.y = pRect->y;
	online.crop_rect.width = pRect->width;
	online.crop_rect.height = pRect->height;

	s32Ret = hi_mpi_vgs_begin_job(&g_hHandle);
	if (s32Ret != HI_SUCCESS)
	{
		printf("hi_mpi_vgs_begin_job failed\n");
		g_hHandle = -1;
		return s32Ret;
	}

	memcpy(&task.img_in, pstInVideo, sizeof(hi_video_frame_info));
	memcpy(&task.img_out, pstOutVideo, sizeof(hi_video_frame_info));
	s32Ret = hi_mpi_vgs_add_online_task(g_hHandle, &task, &online);
	if (s32Ret != HI_SUCCESS)
	{
		printf("hi_mpi_vgs_add_online_task failed\n");
		hi_mpi_vgs_cancel_job(g_hHandle);
        g_hHandle = -1;
		return s32Ret;
	}

	s32Ret = hi_mpi_vgs_end_job(g_hHandle);
	if (s32Ret != HI_SUCCESS)
	{
		printf("hi_mpi_vgs_end_job failed\n");
		hi_mpi_vgs_cancel_job(g_hHandle);
        g_hHandle = -1;
		return s32Ret;
	}
	g_hHandle = -1;
	memcpy(pstOutVideo, &task.img_out, sizeof(hi_video_frame_info));
	return HI_SUCCESS;
}

/////////////////////////////Изменения/////////////////////////////////


#define OSD_COLOR 0xFFFFFF

int vpss_osd_transform_color(unsigned int u32Color)
{
    return (0x8000 | ((u32Color>>3)&0x1F) | 
    			((u32Color>>6)&0x3E0) | ((u32Color>>9)&0x7C00));
}


#define MAX_OSD_REGION_SUPPORT 100
static int g_osdRegionCount = 0;
static hi_rgn_handle g_osdHandle[MAX_OSD_REGION_SUPPORT];
static int g_osdWidth  = 320;  // по умолчанию
static int g_osdHeight = 32;   // по умолчанию
static int g_osdInitFlag = 0;

/* Инициализация OSD region’ов (создаём один раз) */
int vpss_yuv_vgs_init_osd(int maxRegions)
{
    if(g_osdInitFlag){
        // уже инициализировано
        return 0;
    }
    if(maxRegions>MAX_OSD_REGION_SUPPORT) {
        maxRegions = MAX_OSD_REGION_SUPPORT;
    }
    g_osdRegionCount = maxRegions;
    // Если нужно - инициализируем FreeType и т.п.:
#ifdef GSF_OSD_FREETYPE_SUPP
    gsf_freetype_init();
#endif

    for(int i=0; i<g_osdRegionCount; i++){
        g_osdHandle[i] = 140 + i; // например
        hi_rgn_attr stRgnAttr;
        memset(&stRgnAttr, 0, sizeof(stRgnAttr));
        stRgnAttr.type = HI_RGN_OVERLAYEX;
        stRgnAttr.attr.overlayex.pixel_format = HI_PIXEL_FORMAT_ARGB_1555;
        // Ваша цветность фона
        stRgnAttr.attr.overlayex.bg_color = 0x7FFF; // ARGB1555 = белый фон
        stRgnAttr.attr.overlayex.size.width  = g_osdWidth;
        stRgnAttr.attr.overlayex.size.height = g_osdHeight;
        stRgnAttr.attr.overlayex.canvas_num  = 2; // 2 canvas

        int ret = hi_mpi_rgn_create(g_osdHandle[i], &stRgnAttr);
        if(ret!=HI_SUCCESS){
            printf("OSD create handle=%d fail ret=%#x\n", g_osdHandle[i], ret);
        }
    }

    g_osdInitFlag=1;
    return 0;
}

/* Деинициализация OSD region’ов */
int vpss_yuv_vgs_deinit_osd(void)
{
    if(!g_osdInitFlag) return 0;

    for(int i=0; i<g_osdRegionCount; i++){
        hi_mpi_rgn_destroy(g_osdHandle[i]);
    }
#ifdef GSF_OSD_FREETYPE_SUPP
    gsf_freetype_uninit();
#endif
    g_osdInitFlag=0;
    return 0;
}

/* 
   Функция рисует текст в уже созданном region.
   nIndex - какой region используем (от 0 до g_osdRegionCount-1).
   pRect  - прямоугольник объекта (используем для позиционирования текста).
   pText  - текстовая строка (название класса, score).
   pOsd   - структура HI_VGS_OSD для hi_mpi_vgs_add_osd_task().
*/
static int vpss_yuv_vgs_osd_proc(int nIndex,
                                 hi_video_frame_info *pVideo,
                                 hi_rect *pRect,
                                 const char *pText,
                                 hi_vgs_osd *pOsd)
{
    if(nIndex<0 || nIndex>=g_osdRegionCount){
        // Нет такого region
        return -1;
    }
    hi_rgn_handle handle = g_osdHandle[nIndex];

    hi_rgn_canvas_info stRgnCanInfo;
    memset(&stRgnCanInfo, 0, sizeof(stRgnCanInfo));

    int ret = hi_mpi_rgn_get_canvas_info(handle, &stRgnCanInfo);
    if(ret!=HI_SUCCESS){
        printf("hi_mpi_rgn_get_canvas_info handle=%d fail, ret=%#x\n", handle, ret);
        return ret;
    }
    // Очищаем фон (ARGB1555 => 0x7FFF)
    memset(stRgnCanInfo.virt_addr, 0x7F, g_osdWidth*g_osdHeight*2);

    // Рисуем текст. У вас может быть своя функция gsf_font_utf8_draw_line(...)
    unsigned short color = (unsigned short)vpss_osd_transform_color(OSD_COLOR);
    // Простой пример:
#ifdef GSF_OSD_FREETYPE_SUPP
    if(pText && pText[0]){
        // Шрифт ~16 пикс, рисуем в левом верхнем
        gsf_freetype_font_utf8_draw_line(
              16, g_osdWidth, 0, 
              (char*)stRgnCanInfo.virt_addr,
              (char*)pText, "",
              color, color,
              0, 0,
              g_osdWidth*g_osdHeight*2,
              0, 1, 16
        );
    }
#else
    if(pText && pText[0]){
        gsf_font_utf8_draw_line(
              16, g_osdWidth, 0,
              (char*)stRgnCanInfo.virt_addr,
              (char*)pText, "",
              color, color,
              0, 0,
              g_osdWidth*g_osdHeight*2,
              0, 1, 16
        );
    }
#endif

    ret = hi_mpi_rgn_update_canvas(handle);
    if(ret!=HI_SUCCESS){
        printf("hi_mpi_rgn_update_canvas handle=%d fail ret=%#x\n", handle, ret);
        return ret;
    }

    // Определяем, куда ставить OSD
    int localX = pRect->x;
    int localY = pRect->y - g_osdHeight; // чуть выше бокса
    if(localY<0) localY=0;
    if(localX<0) localX=0;
    int fw = pVideo->video_frame.width;
    int fh = pVideo->video_frame.height;
    if(localX + g_osdWidth > fw){
        localX = fw - g_osdWidth;
        if(localX<0) localX=0;
    }
    if(localY + g_osdHeight > fh){
        localY = fh - g_osdHeight;
        if(localY<0) localY=0;
    }

    memset(pOsd, 0, sizeof(*pOsd));
    pOsd->pixel_format = HI_PIXEL_FORMAT_ARGB_1555;
    pOsd->phys_addr    = stRgnCanInfo.phys_addr;
    pOsd->stride       = stRgnCanInfo.stride;
    pOsd->bg_alpha     = 0;    // фон
    pOsd->fg_alpha     = 255;  // текст
    pOsd->osd_inverted_color = HI_VGS_OSD_INVERTED_COLOR_NONE;

    pOsd->rect.x = HI_ALIGN_UP(localX,2);
    pOsd->rect.y = HI_ALIGN_UP(localY,2);
    pOsd->rect.width  = HI_ALIGN_UP(g_osdWidth,2);
    pOsd->rect.height = HI_ALIGN_UP(g_osdHeight,2);

    return 0;
}

/*
   Основная функция отрисовки AI-боксов и текстов (рамка + OSD).
   Вызывается из gsf_vpss_yuv_data_cb, если pHdl->nAiDataNum>0.
*/
int vpss_yuv_vgs_ai_proc(hi_video_frame_info *pVideoFrame,
                         AI_DATA_INFO_S *pAiDataInfo,
                         int nArrayNum)
{
    if(!pVideoFrame || !pAiDataInfo || nArrayNum<=0){
        return 0;
    }
    hi_vgs_handle handle = -1;
    int ret = hi_mpi_vgs_begin_job(&handle);
    if(ret != HI_SUCCESS){
        printf("Vgs begin job fail, ret=%#x\n", ret);
        return ret;
    }
    hi_vgs_task_attr stTask;
    memset(&stTask, 0, sizeof(stTask));
    memcpy(&stTask.img_in,  pVideoFrame, sizeof(hi_video_frame_info));
    memcpy(&stTask.img_out, pVideoFrame, sizeof(hi_video_frame_info));

    // ограничим кол-во боксов
    if(nArrayNum>MAX_OSD_REGION_SUPPORT) nArrayNum = MAX_OSD_REGION_SUPPORT;

    // Массивы cover + osd
    hi_cover   stCover[MAX_OSD_REGION_SUPPORT];
    hi_vgs_osd stOsd[  MAX_OSD_REGION_SUPPORT];
    memset(stCover, 0, sizeof(stCover));
    memset(stOsd,   0, sizeof(stOsd));

    // Заполняем cover + OSD для каждого детекта
    for(int i=0; i<nArrayNum; i++){
        // 1) cover (рамка)
        stCover[i].color = 0xFF0000; // красный
        stCover[i].type  = HI_COVER_RECT;
        stCover[i].rect_attr.is_solid = HI_FALSE;
        stCover[i].rect_attr.thick = 2;

        int lx = pAiDataInfo[i].rect.x;
        int ly = pAiDataInfo[i].rect.y;
        int w  = pAiDataInfo[i].rect.width;
        int h  = pAiDataInfo[i].rect.height;

        // защита от выхода за границы
        if(lx<0) lx=0;
        if(ly<0) ly=0;
        if(lx + w > (int)pVideoFrame->video_frame.width)
            w = pVideoFrame->video_frame.width - lx;
        if(ly + h > (int)pVideoFrame->video_frame.height)
            h = pVideoFrame->video_frame.height - ly;

        stCover[i].rect_attr.rect.x = HI_ALIGN_UP(lx,2);
        stCover[i].rect_attr.rect.y = HI_ALIGN_UP(ly,2);
        stCover[i].rect_attr.rect.width  = HI_ALIGN_UP(w,2);
        stCover[i].rect_attr.rect.height = HI_ALIGN_UP(h,2);

        // 2) osd-текст
        vpss_yuv_vgs_osd_proc(i, pVideoFrame,
                              &stCover[i].rect_attr.rect,
                              pAiDataInfo[i].szText,
                              &stOsd[i]);
    }

    // Добавляем OSD
    ret = hi_mpi_vgs_add_osd_task(handle, &stTask, stOsd, nArrayNum);
    if(ret!=HI_SUCCESS){
        printf("hi_mpi_vgs_add_osd_task fail, ret=%#x\n", ret);
        hi_mpi_vgs_cancel_job(handle);
        return ret;
    }

    // Добавляем рамки
    ret = hi_mpi_vgs_add_cover_task(handle, &stTask, stCover, nArrayNum);
    if(ret!=HI_SUCCESS){
        printf("hi_mpi_vgs_add_cover_task fail, ret=%#x\n", ret);
        hi_mpi_vgs_cancel_job(handle);
        return ret;
    }

    ret = hi_mpi_vgs_end_job(handle);
    if(ret!=HI_SUCCESS){
        printf("hi_mpi_vgs_end_job fail, ret=%#x\n", ret);
        hi_mpi_vgs_cancel_job(handle);
        return ret;
    }

    return 0;
}









// int vpss_yuv_vgs_osd_proc(int nIndex, int nFrameWidth, int nFrameHeight, hi_rect *pRect, char *pText,hi_vgs_osd *pOsd)
// {
// 	int nRet = 0,nLine = 0,nLocalX = 0,nLocalY = 0;
// 	hi_rgn_handle nRgnHandle = 140 + nIndex;
//     hi_rgn_attr stRgnAttr = {0};
//     hi_rgn_canvas_info stRgnCanInfo = {0};
//     int nColor = OSD_COLOR;
//     unsigned short u16Color = 0;
//     int nBmpBuffSize = 0,nNewPos = 0,nFontInt = 0;
//     int nFontW = 0,nFontH = 0,nOsdW = 0,nOsdH = 0;
// 	static int nLastOsdW = 0;

// 	/*if (nFrameWidth > 1920)
// 		nFontW = 64;
// 	else */if (nFrameWidth >= 1280)
// 		nFontW = 32;
// 	else
// 		nFontW = 16;

// 	nFontH = nFontW;
// 	nOsdH = nFontH;

// 	for(nNewPos = 0; pText[nNewPos] != '\0';)
// 	{
// 		if(pText[nNewPos] >= 128 || pText[nNewPos] < 0)
// 			nNewPos += gsf_font_get_utf8_char_num(pText[nNewPos]);
// 		else
// 			nNewPos++;

// 		nOsdW += (nFontW + nFontInt);
// 	}

// 	nOsdW = HI_ALIGN_UP(nOsdW, 64);
// 	nBmpBuffSize = nOsdW * nOsdH * 2;
	
// 	if (nLastOsdW != nOsdW)
// 	{
// 		printf("nOsdW = %d, last = %d, bufsize = %d\n", nOsdW, nLastOsdW, nBmpBuffSize);
// 		nLastOsdW = nOsdW;
// 	}

// 	stRgnAttr.type = HI_RGN_OVERLAYEX;
//     stRgnAttr.attr.overlayex.pixel_format = HI_PIXEL_FORMAT_ARGB_1555;
//     stRgnAttr.attr.overlayex.bg_color = 0x7FFF;
//     stRgnAttr.attr.overlayex.size.width = HI_ALIGN_UP(nOsdW, 2);
//     stRgnAttr.attr.overlayex.size.height = HI_ALIGN_UP(nOsdH, 2);
//     stRgnAttr.attr.overlayex.canvas_num = 2; /* 2 canvas */

// 	if (g_stRgnData[nIndex].nRgnCreateFlag == 0 || 
// 		g_stRgnData[nIndex].nWidth  != stRgnAttr.attr.overlayex.size.width ||
// 		g_stRgnData[nIndex].nHeight != stRgnAttr.attr.overlayex.size.height)
// 	{
// #ifdef GSF_OSD_FREETYPE_SUPP
// 		if(g_stRgnData[0].nRgnCreateFlag == 0)
// 		{
// 			gsf_freetype_init();
// 		}
// #endif
// 		if (g_stRgnData[nIndex].nRgnCreateFlag)
// 		{
// 			if ((nRet = hi_mpi_rgn_destroy(nRgnHandle)) != HI_SUCCESS)
// 			{
// 				printf("hi_mpi_rgn_destroy failed, ret:0x%x\n", nRet);
// 				return nRet;
// 			}
// 		}
// 		g_stRgnData[nIndex].nRgnCreateFlag = 1;
// 	    if ((nRet = hi_mpi_rgn_create(nRgnHandle, &stRgnAttr)) != HI_SUCCESS) 
// 	    {
// 	        printf("hi_mpi_rgn_create failed, ret:0x%x\n", nRet);
// 	        return nRet;
// 	    }
// 		g_stRgnData[nIndex].nWidth  = stRgnAttr.attr.overlayex.size.width;
// 		g_stRgnData[nIndex].nHeight = stRgnAttr.attr.overlayex.size.height;
// 	}
	
//     if ((nRet = hi_mpi_rgn_get_canvas_info(nRgnHandle, &stRgnCanInfo)) != HI_SUCCESS) 
//     {
//         printf("hi_mpi_rgn_get_canvas_info failed, ret:0x%x\n", nRet);
//         return nRet;
//     }

//     u16Color = vpss_osd_transform_color(nColor);
// #ifdef GSF_OSD_FREETYPE_SUPP	
// 	nRet = gsf_freetype_font_utf8_draw_line(
// 							nFontW,nOsdW,nFontInt, 
// 							stRgnCanInfo.virt_addr,
// 							pText, "",
// 							u16Color, u16Color,
// 							0, 0,
// 							nBmpBuffSize,0,1,nFontH
// 							);
// #else
// 	nRet = gsf_font_utf8_draw_line(nFontW,nOsdW,nFontInt,
// 								   (char *)stRgnCanInfo.virt_addr,
// 								   pText, "",
// 								   u16Color, u16Color,
// 								   0,0,
// 								   nBmpBuffSize,0,1,nFontH
// 							);
// #endif
//     if ((nRet = hi_mpi_rgn_update_canvas(nRgnHandle)) != HI_SUCCESS) 
//     {
//         printf("hi_mpi_rgn_get_canvas_info failed, ret:0x%x\n", nRet);
//         return nRet;
//     }

// 	nLocalX = pRect->x;

// 	if(pRect->y < nFontH)
// 		nLocalY = pRect->y + pRect->height+2;
// 	else
// 		nLocalY = pRect->y - nFontH;

// 	if (nFrameWidth <= nOsdW)
// 		nLocalX = 0;
// 	else if (nLocalX + nOsdW > nFrameWidth)
// 		nLocalX = nFrameWidth - nOsdW;
		
// 	if (nFrameHeight <= nOsdH)
// 		nLocalY = 0;
// 	else if (nLocalY + nOsdH > nFrameHeight )
// 		nLocalY = nFrameHeight - nOsdH;

// 	if(nLocalX < 0)
// 		nLocalX = 0;
// 	if(nLocalY < 0)
// 		nLocalY = 0;
	
// 	pOsd->rect.x = HI_ALIGN_UP(nLocalX,2);
// 	pOsd->rect.y = HI_ALIGN_UP(nLocalY,2);
// 	pOsd->rect.width = HI_ALIGN_UP(nOsdW,2);
// 	pOsd->rect.height = HI_ALIGN_UP(nOsdH,2);
// 	pOsd->pixel_format = HI_PIXEL_FORMAT_ARGB_1555;
// 	pOsd->phys_addr = stRgnCanInfo.phys_addr;
// 	pOsd->stride = stRgnCanInfo.stride;
// 	pOsd->bg_alpha = 0;
// 	pOsd->fg_alpha = 255;
// 	pOsd->osd_inverted_color = HI_VGS_OSD_INVERTED_COLOR_NONE;

// 	return 0;
// }

// //������������Ϳ��߰�����֡���߱�������

// /////////////////////////////���� ����������/////////////////////////////////
// int vpss_yuv_vgs_ai_proc(hi_video_frame_info *pVideoFrame,AI_DATA_INFO_S *pAiDataInfo,int nArrayNum)
// {
// 	int nLocalX = 0,nLocalY = 0,nLocalW = 0,nLocalH = 0;
// 	hi_vgs_handle handle = -1;
//     int i = 0, j = 0,nRet = 0;
//     hi_vgs_task_attr stTask = {0};
//     hi_cover stCover[32] = {0};  // ��ҧ�ѧ�ڧ�� �ӧߧڧާѧߧڧ� - ���� 32 �է֧�֧ܧ�ڧ� �ާѧܧ�ڧާ��
//     hi_vgs_osd stOsd[32] = {0};

// 	if(nArrayNum <= 0 || pAiDataInfo == NULL || pVideoFrame == NULL)
// 		return 0;
	
//     if ((nRet = hi_mpi_vgs_begin_job(&handle)) != HI_SUCCESS) 
//     {
//         printf("Vgs begin job fail, Error(%#x)\n", nRet);
//         return nRet;
//     }

//     memcpy(&stTask.img_in, pVideoFrame, sizeof(hi_video_frame_info));
//     memcpy(&stTask.img_out, pVideoFrame, sizeof(hi_video_frame_info));

//     for (i = 0; i < nArrayNum; i++) 
//     {
//     	stCover[i].color = 0xFF0000;
//     	stCover[i].type = HI_COVER_RECT;
//     	stCover[i].rect_attr.is_solid = HI_FALSE;
//     	stCover[i].rect_attr.thick = 2;

// 		nLocalX = pAiDataInfo[i].rect.x;
// 		nLocalY = pAiDataInfo[i].rect.y;
// 		nLocalW = pAiDataInfo[i].rect.width;
// 		nLocalH = pAiDataInfo[i].rect.height;
		
// 		if(pVideoFrame->video_frame.width <= nLocalW)
// 			nLocalX = 0;
// 		else if(nLocalX + nLocalW > pVideoFrame->video_frame.width)
// 			nLocalX = pVideoFrame->video_frame.width - nLocalW;

// 		if (pVideoFrame->video_frame.height <= nLocalH)
// 			nLocalY = 0;
// 		else if (nLocalY + nLocalH > pVideoFrame->video_frame.height)
// 			nLocalY = pVideoFrame->video_frame.height - nLocalH;
			
// 		if(nLocalX < 0)
// 			nLocalX = 0;
// 		if(nLocalY < 0)
// 			nLocalY = 0;

//     	stCover[i].rect_attr.rect.x = HI_ALIGN_UP(nLocalX,2);
//     	stCover[i].rect_attr.rect.y = HI_ALIGN_UP(nLocalY,2);
//     	stCover[i].rect_attr.rect.width = HI_ALIGN_UP(pAiDataInfo[i].rect.width,2);
//     	stCover[i].rect_attr.rect.height = HI_ALIGN_UP(pAiDataInfo[i].rect.height,2);

// 		nRet = vpss_yuv_vgs_osd_proc(i,pVideoFrame->video_frame.width,pVideoFrame->video_frame.height,
// 							&stCover[i].rect_attr.rect,pAiDataInfo[i].szText, &stOsd[i]);
//     }

// 	if ((nRet = hi_mpi_vgs_add_osd_task(handle, &stTask, &stOsd[0], nArrayNum)) != HI_SUCCESS) 
// 	{
//         printf("hi_mpi_vgs_add_osd_task fail, Error(%#x)\n", nRet);
// 		hi_mpi_vgs_cancel_job(handle);
//         return nRet;
//     }

// 	if ((nRet = hi_mpi_vgs_add_cover_task(handle, &stTask, &stCover[0], nArrayNum)) != HI_SUCCESS) 
// 	{
//         printf("hi_mpi_vgs_add_cover_task fail, Error(%#x)\n", nRet);
// 		hi_mpi_vgs_cancel_job(handle);
//         return nRet;
//     }

//     if ((nRet = hi_mpi_vgs_end_job(handle)) != HI_SUCCESS) 
//     {
//         printf("hi_mpi_vgs_end_job fail, Error(%#x)\n", nRet);
// 		hi_mpi_vgs_cancel_job(handle);
//         return nRet;
//     }
    
//     handle = -1;
// 	return 0;
// }


