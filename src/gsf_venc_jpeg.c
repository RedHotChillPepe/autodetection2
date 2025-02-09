/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_venc_jpeg.c

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/08/10
  Description   :
  History       :
  1.Date        : 2021/08/10
    Author      :
    Modification: Created file

******************************************************************************/
/*
 * 从YUV视频帧获取JPEG图片
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "hi_math.h"
#include <sys/time.h>
#include <stdlib.h>
#include "hi_common_vgs.h"
#include "hi_common_venc.h"
#include "hi_common_video.h"
#include "hi_mpi_vgs.h"
#include "hi_mpi_venc.h"
#include "hi_mpi_vpss.h"

#define RECT_ENCODER_CHN 5
#define FULLPIC_ENCODER_CHN 6

static int g_nRectEncCreateFlag = 0;
static int g_nFullPicEncCreateFlag = 0;
static hi_rect g_stRect = {0, 0, 0, 0};
static hi_size g_stSize = {0, 0};

/*
 * 在视频帧中加入矩形框
 */
int draw_rect_to_video_frame(hi_video_frame_info* pstFrmInfo, hi_rect *pstRect, int nRects)
{
    int nRet = HI_SUCCESS;
	
    hi_vgs_handle handle;
    hi_vgs_task_attr task;
	hi_cover cover = {0};
    int nIdx = 0;

    if (pstFrmInfo == NULL || pstRect == NULL || nRects == 0)
	{
		printf("[%s %d] Invalid argument: pstFrmInfo(%p) pstRect(%p) nRects(%d)\n", __func__, __LINE__, pstFrmInfo, pstRect, nRects);
        return -1;
    }
    nRet = hi_mpi_vgs_begin_job(&handle);
    if (nRet != HI_SUCCESS)
    {
        printf("[%s %d] Vgs begin job fail,Error(%#x)\n", __func__, __LINE__, nRet);
        return nRet;
    }

    memcpy(&task.img_in, pstFrmInfo, sizeof(ot_video_frame_info));
    memcpy(&task.img_out, pstFrmInfo, sizeof(ot_video_frame_info));

    while(nIdx < nRects)
    {
		cover.type = OT_COVER_QUAD;
	    cover.color = 0x00FF0000;
#ifdef GSFSS928_ARCH
	    cover.quad.is_solid = HI_FALSE;
	    cover.quad.thick = 2;
	    
        cover.quad.point[0].x = pstRect[nIdx].x & 0xFFFE;
        cover.quad.point[0].y = pstRect[nIdx].y & 0xFFFE;
        cover.quad.point[1].x = (pstRect[nIdx].x+pstRect[nIdx].width) & 0xFFFE;
        cover.quad.point[1].y = pstRect[nIdx].y & 0xFFFE;
        cover.quad.point[2].x = (pstRect[nIdx].x+pstRect[nIdx].width) & 0xFFFE;
        cover.quad.point[2].y = (pstRect[nIdx].y+pstRect[nIdx].height) & 0xFFFE;
        cover.quad.point[3].x = pstRect[nIdx].x & 0xFFFE;
        cover.quad.point[3].y = (pstRect[nIdx].y+pstRect[nIdx].height) & 0xFFFE;
#elif (defined GSF3519DV5_ARCH)
		cover.quad_attr.is_solid = HI_FALSE;
	    cover.quad_attr.thick = 2;
	    
        cover.quad_attr.point[0].x = pstRect[nIdx].x & 0xFFFE;
        cover.quad_attr.point[0].y = pstRect[nIdx].y & 0xFFFE;
        cover.quad_attr.point[1].x = (pstRect[nIdx].x+pstRect[nIdx].width) & 0xFFFE;
        cover.quad_attr.point[1].y = pstRect[nIdx].y & 0xFFFE;
        cover.quad_attr.point[2].x = (pstRect[nIdx].x+pstRect[nIdx].width) & 0xFFFE;
        cover.quad_attr.point[2].y = (pstRect[nIdx].y+pstRect[nIdx].height) & 0xFFFE;
        cover.quad_attr.point[3].x = pstRect[nIdx].x & 0xFFFE;
        cover.quad_attr.point[3].y = (pstRect[nIdx].y+pstRect[nIdx].height) & 0xFFFE;
#endif
		nIdx++;
    }
	nRet = hi_mpi_vgs_add_cover_task(handle, &task, &cover, nRects);
    if (nRet != HI_SUCCESS)
    {
#ifdef GSFSS928_ARCH
        printf("[%s %d] hi_mpi_vgs_add_cover_task Error(%#x) rangle((%d %d) (%d %d) (%d %d) (%d %d))\n", 
			__func__, __LINE__, nRet,
			cover.quad.point[0].x, cover.quad.point[0].y, 
			cover.quad.point[1].x, cover.quad.point[1].y,
			cover.quad.point[2].x, cover.quad.point[2].y,
			cover.quad.point[3].x, cover.quad.point[3].y);
#elif (defined GSF3519DV5_ARCH)
		printf("[%s %d] hi_mpi_vgs_add_cover_task Error(%#x) rangle((%d %d) (%d %d) (%d %d) (%d %d))\n", 
			__func__, __LINE__, nRet,
			cover.quad_attr.point[0].x, cover.quad_attr.point[0].y, 
			cover.quad_attr.point[1].x, cover.quad_attr.point[1].y,
			cover.quad_attr.point[2].x, cover.quad_attr.point[2].y,
			cover.quad_attr.point[3].x, cover.quad_attr.point[3].y);
#endif
		hi_mpi_vgs_cancel_job(handle);
        return nRet;
    }

    nRet = hi_mpi_vgs_end_job(handle);
    if (nRet != HI_SUCCESS)
    {
        printf("[%s %d] hi_mpi_vgs_end_job fail,Error(%#x)\n", __func__, __LINE__, nRet);
		hi_mpi_vgs_cancel_job(handle);
        return nRet;
    }
    printf("[%s %d] hi_mpi_vgs_end_job success!\n", __func__, __LINE__);
    return nRet;
}

int send_venc_frame(hi_video_frame_info *pstVideo, int nVencChn, int nDrawRect, hi_rect *pstRect, int nRects)
{
	if (nDrawRect && pstRect && nRects > 0)
    {
		// 需要在全景图片上增加矩形标记区域
        //draw_rect_to_video_frame(pstVideo, pstRect, nRects);
    }

	return hi_mpi_venc_send_frame(nVencChn, pstVideo, 120);
}

/* 
 * 保存为JPG图片文件
 * 注意每秒25张全景图片+25张区域图片，数据量较大
 */
int save_jpeg_file_from_stream(hi_venc_stream *pStream, hi_rect *pstRect, int nVenc)
{
	static int cnt[2] = {0};
	static int nLastTime[2] = {0};
	int index = 0;
	int nPicSize = 0;
	char szFileName[64] = {0};
	time_t tTime = 0;
	FILE *fp = NULL;
	if (pStream == NULL)
	{
		printf("[%s %d] Invalid argument: pStream(%p)\n", __func__, __LINE__, pStream);
		return -1;
	}
	tTime = time(NULL);
	// 以下代码启用后每秒仅保存一张全景图片+一张区域图片
	//if ((nVenc == RECT_ENCODER_CHN && tTime <= nLastTime[1]) ||
	//	(nVenc == FULLPIC_ENCODER_CHN && tTime <= nLastTime[0]))
	if (0)
	{
		return 0;
	}
	nLastTime[nVenc == RECT_ENCODER_CHN] = tTime;
	sprintf(szFileName, "/tmp/%d_%d_%u.jpg", nVenc, cnt[nVenc == RECT_ENCODER_CHN]++, tTime);
	if ((fp = fopen(szFileName, "wb")) != NULL)
	{
		for (index = 0; index < pStream->pack_cnt; index++)
		{
			if (pStream->pack[index].len> 0)
			{
				int nDataSize = (pStream->pack[index].len- pStream->pack[index].offset);
				fwrite((pStream->pack[index].addr + pStream->pack[index].offset), nDataSize, 1, fp);
				nPicSize += nDataSize;
			}
		}
		printf("[%s %d] jpg size(%d) file(%s)\n", __func__, __LINE__, nPicSize, szFileName);
		fclose(fp);
		fp = NULL;
	}
	return 0;
}

int get_picture_stream(hi_video_frame_info *pstVideo, int nVEncChn, hi_rect *pstRect)
{   
    int    s32Ret = -1;
    int    VencFd = 0;
    fd_set read_fds;
    struct timeval  TimeoutVal = {0, 0}; 
	
    hi_venc_chn_status status = {0};
    hi_venc_start_param recv_param = {0};
    hi_venc_stream stream = {0};

	printf("[%s %d] enc(%d) pstVideo(%0#x) rect(%d %d %d %d)\n", __func__, __LINE__, 
		nVEncChn, pstVideo, pstRect->x, pstRect->y, pstRect->width, pstRect->height);
    
    int nVEncWrPos = 0;         //前面加两个字节(0xff, 0xd8)才是标准jpg文件
    int count      = 0;
    int max_buff_size = 2048<<10; // 2MB
	
	static unsigned char *pVEncBuffer = NULL;

    if (pVEncBuffer == NULL && (pVEncBuffer = (char *)calloc(1, max_buff_size)) == NULL)
       goto __get_pic_stream_err;
   
    //抓图编码处理
    if ((VencFd = hi_mpi_venc_get_fd(nVEncChn)) <= 0)
    {
        printf("[%s %d] hi_mpi_venc_get_fd err %0#x\n", __func__, __LINE__, VencFd);
        goto __get_pic_stream_err;
    }
    recv_param.recv_pic_num = -1;
    if ((s32Ret =  hi_mpi_venc_start_chn(nVEncChn, &recv_param)) != HI_SUCCESS)
    {
        printf("[%s %d] start venc chn %d hi_mpi_venc_start_chn is err: %#x.\n", __func__, __LINE__, nVEncChn, s32Ret);
        goto __get_pic_stream_err;
    }
	hi_rect stDrawRect = {16, 16, 400, 400};
    while(count++ < 3)
    {
        if ((s32Ret = send_venc_frame(pstVideo, nVEncChn, nVEncChn == FULLPIC_ENCODER_CHN, &stDrawRect, 1)) != 0)
        {
			printf("[%s %d] gsf_av_face_send_venc_frame failed ret(%0#x).\n", __func__, __LINE__, s32Ret);
			if (HI_SUCCESS != (s32Ret = hi_mpi_venc_stop_chn(nVEncChn)))
            {
                printf("[%s %d] hi_mpi_venc_stop_chn err %0#x\n", __func__, __LINE__, s32Ret);
            }
            else
            {
            	hi_mpi_venc_reset_chn(nVEncChn);
            }
            goto __get_pic_stream_err;
        }
        FD_ZERO(&read_fds);
        FD_SET(VencFd, &read_fds);
        TimeoutVal.tv_sec  = 0;
        TimeoutVal.tv_usec = 120000;
        s32Ret = select(VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if(s32Ret <= 0 || !FD_ISSET(VencFd, &read_fds))
        {
            printf("[%s %d] select error %d\n", __func__, __LINE__, nVEncChn);   
            continue;
        }
        memset(&stream, 0, sizeof(hi_venc_stream));
        memset(&status,   0, sizeof(status));
        if ((s32Ret = hi_mpi_venc_query_status(nVEncChn, &status)) != HI_SUCCESS) 
        {
            printf("[%s %d] HI_MPI_VENC_Query: %0#x(%d %d)\n", __func__, __LINE__, s32Ret, 0, nVEncChn); 
            usleep(100 * 1000);
            continue;
        }
        //数据太大，咋处理 //数据包个数
        if( (sizeof(hi_venc_pack) * status.cur_packs) > max_buff_size || status.cur_packs< 1)
        {
            printf("[%s %d] fill data err = %d %d(%d %d)\n", __func__, __LINE__, sizeof(hi_venc_pack) * status.cur_packs,
                status.cur_packs, 0, nVEncChn);
            continue;
        }

		//printf("get_picture_stream: u32CurPacks %u, dataLen=sizeof(VENC_PACK_S)*%u=%u\n", 
		//	stStat.u32CurPacks, sizeof(VENC_PACK_S), sizeof(VENC_PACK_S) * stStat.u32CurPacks);
        
        //获取码流
        stream.pack= (hi_venc_pack *)pVEncBuffer;
        stream.pack_cnt = status.cur_packs;
        if ((s32Ret = hi_mpi_venc_get_stream(nVEncChn, &stream, HI_TRUE)) != HI_SUCCESS)
        {
            printf("[%s %d] hi_mpi_venc_get_stream err %0#x(%d, %d)\n", __func__, __LINE__, s32Ret, 0, nVEncChn); 
            continue;
        }

		save_jpeg_file_from_stream(&stream, pstRect, nVEncChn);

		hi_mpi_venc_release_stream(nVEncChn, &stream);
		
        //*pPicAddr = pDataBuffer;
        if (HI_SUCCESS != (s32Ret = hi_mpi_venc_stop_chn(nVEncChn)))
        {
            printf("[%s %d] hi_mpi_venc_stop_chn err %0#x\n", __func__, __LINE__, s32Ret);
        }   
        else
        {
        	hi_mpi_venc_reset_chn(nVEncChn);
        }
        return 0;
    }
    if (HI_SUCCESS != (s32Ret = hi_mpi_venc_stop_chn(nVEncChn)))
    {
        printf("[%s %d] hi_mpi_venc_stop_chn ch %d err %0#x\n", __func__, __LINE__, nVEncChn, s32Ret);
    } 
    else
    {
    	hi_mpi_venc_reset_chn(nVEncChn);
    }
	
  __get_pic_stream_err:
    return s32Ret;
}

int create_picture_encoder(int nVEncChn, int nEncWidth, int nEncHeight)
{
    hi_venc_chn_attr attr;
    hi_venc_jpeg_attr *pJpegAttr= NULL;
    
	memset(&attr, 0, sizeof(attr));

    pJpegAttr = &attr.venc_attr.jpeg_attr;
    attr.venc_attr.max_pic_width  = nEncWidth;
    attr.venc_attr.max_pic_height = nEncHeight;
    attr.venc_attr.pic_width  = nEncWidth;
    attr.venc_attr.pic_height = nEncHeight;
    attr.venc_attr.buf_size = HI_ALIGN_UP((HI_ALIGN_UP(nEncWidth, 64))*(HI_ALIGN_UP(nEncHeight, 64)), 4096);
    if (attr.venc_attr.buf_size < 32768)
		attr.venc_attr.buf_size = 32768;
	attr.venc_attr.is_by_frame = HI_TRUE;/*get stream mode is field mode  or frame mode*/    
    pJpegAttr->dcf_en = HI_FALSE;
    attr.venc_attr.type = OT_PT_JPEG;

	return hi_mpi_venc_create_chn(nVEncChn, &attr);
}


/* 通过YUV视频帧抓取全景图片 */
int shoot_full_picture(hi_video_frame_info *pstVideo)
{
	int nRet = 0;
    int nVEncChn = FULLPIC_ENCODER_CHN;
	
    hi_u32 u32Width = 0;
    hi_u32 u32Height = 0;

    if(pstVideo == NULL)
    {
        printf("[%s %d] Invalid argment: pSize(%p), pstVideo(%p)\n", pstVideo);
        return -1;
    }
	// 创建编码器
    u32Width  =  pstVideo->video_frame.width;
    u32Height =  pstVideo->video_frame.height;

    if (g_nFullPicEncCreateFlag == 0 ||
		g_stSize.width != u32Width ||
		g_stSize.height != u32Height)
    {
		if (g_nFullPicEncCreateFlag)
		{
			hi_mpi_venc_destroy_chn(FULLPIC_ENCODER_CHN);
			g_nFullPicEncCreateFlag = 0;
		}
	    if ((nRet = create_picture_encoder(FULLPIC_ENCODER_CHN, u32Width, u32Height)) != HI_SUCCESS)
	    {
	        return nRet;
	    }
		g_nFullPicEncCreateFlag = 1;
		g_stSize.width = u32Width;
		g_stSize.height = u32Height;
    }
	hi_rect stRect = {0, 0, u32Width, u32Height};
    return get_picture_stream(pstVideo, nVEncChn, &stRect);
}

int set_jpeg_attr(int nVencChn)
{
	hi_u32 u32QFactor = 99;
	hi_s32 s32Ret = 0;
	hi_venc_jpeg_param jpeg_param;

	if (HI_SUCCESS != (s32Ret = hi_mpi_venc_get_jpeg_param(nVencChn, &jpeg_param)))
	{
		printf("[%s %d] hi_mpi_venc_get_jpeg_param err %0#x\n", __func__, __LINE__, s32Ret);
		return -1;
	}
	if (jpeg_param.qfactor != u32QFactor)
	{
		jpeg_param.qfactor = u32QFactor;
		if (HI_SUCCESS != (s32Ret = hi_mpi_venc_set_jpeg_param(nVencChn, &jpeg_param)))
		{
			printf("[%s %d] hi_mpi_venc_set_jpeg_param err %0#x\n", __func__, __LINE__, s32Ret);
			return -1;
		}
	}
	return HI_SUCCESS;
}

/* 通过YUV视频帧抓取区域图片 */
int shoot_rect_picture(hi_video_frame_info *pstVideo, hi_rect *pstRect, int nPicWidth, int nPicHeight)
{
    int s32Ret = 0;
    int nVEncChn = RECT_ENCODER_CHN;
    hi_venc_chn_param chn_param;
    //内存是否正常
    if(pstVideo == NULL || pstRect == NULL)
    {
        printf("[%s %d] Invalid argument: pstVideo(%p) pstRect(%p)\n", pstVideo, pstRect);
        return -1;
    }

	printf("[%s %d] pic(%d %d) rect(%d %d %d %d)\n", __func__, __LINE__, nPicWidth, nPicHeight, 
		pstRect->x, pstRect->y, pstRect->width, pstRect->height);
	
    if (nPicWidth > pstRect->width)
    {
        nPicWidth = pstRect->width;
    }
    if (nPicHeight > pstRect->height)
    {
        nPicHeight = pstRect->height;
    }

	printf("[%s %d] pic(%d %d) rect(%d %d %d %d)\n", __func__, __LINE__, nPicWidth, nPicHeight, 
		pstRect->x, pstRect->y, pstRect->width, pstRect->height);
    
    // 创建编码器
    if (nPicWidth != g_stRect.width|| 
		nPicHeight != g_stRect.height)
    {
		if (g_nRectEncCreateFlag)
		{
			hi_mpi_venc_destroy_chn(nVEncChn);
			g_nRectEncCreateFlag = 0;
		}
	    if ((s32Ret = create_picture_encoder(nVEncChn, nPicWidth, nPicHeight)) != HI_SUCCESS)
	    {
	        printf("[%s %d] create encoder failed, venc ch(%d) wh(%d, %d) ret(%#x)\n", __func__, __LINE__, 
	                    nVEncChn, nPicWidth, nPicHeight, s32Ret);
	        return -2;
	    }
		set_jpeg_attr(nVEncChn);
		g_nRectEncCreateFlag   = 1;
		g_stRect.width = nPicWidth;
		g_stRect.height = nPicHeight;
	}

	if ((s32Ret = hi_mpi_venc_get_chn_param(nVEncChn, &chn_param)) != HI_SUCCESS)
    {
        printf("[%s %d] hi_mpi_venc_get_chn_param for crop set (%d, rect(%d %d %d %d)) err %0#x\n", __func__, __LINE__, 
                   nVEncChn, pstRect->x, pstRect->y, pstRect->width, pstRect->height, s32Ret);
        return -3;
    }
	chn_param.crop_info.enable = HI_TRUE;
	chn_param.crop_info.rect.x = pstRect->x;
	chn_param.crop_info.rect.y = pstRect->y;
	chn_param.crop_info.rect.width = pstRect->width;
	chn_param.crop_info.rect.height = pstRect->height;
	if ((s32Ret = hi_mpi_venc_set_chn_param(nVEncChn, &chn_param)) != HI_SUCCESS)
    {
        printf("[%s %d] hi_mpi_venc_set_chn_param for crop set (%d, rect(%d %d %d %d)) err %0#x\n", __func__, __LINE__, 
                   nVEncChn, pstRect->x, pstRect->y, pstRect->width, pstRect->height, s32Ret);
        return -4;
    }

    return get_picture_stream(pstVideo, nVEncChn, pstRect);
}

int rect_pos_auto_adj(hi_rect *pstRect, hi_u32 nLimitWidth, hi_u32 nLimitHeight)
{
	static int nAutoIdx[2] = {0};
	int nXStep = 16, nYStep = 4;
	if (pstRect->x + (nXStep * nAutoIdx[0]) + pstRect->width< nLimitWidth)
	{
		pstRect->x = (nXStep * nAutoIdx[0]);
		nAutoIdx[0]++;
	}
	else
		nAutoIdx[0] = 0;
	
	if (pstRect->y + (nYStep * nAutoIdx[1]) + pstRect->height < nLimitHeight)
	{
		pstRect->y = (nYStep * nAutoIdx[1]);
		nAutoIdx[1]++;
	}
	else
		nAutoIdx[1] = 0;
}

int save_jpeg_data_from_video_frame(hi_video_frame_info *pstVideo)
{
	hi_rect stRect = {0, 0, 500, 500}; // 注意：stRect.s32X需16像素对齐，stRect.s32Y需2像素对齐
	rect_pos_auto_adj(&stRect, pstVideo->video_frame.width, pstVideo->video_frame.height);
	shoot_rect_picture(pstVideo, &stRect, pstVideo->video_frame.width, pstVideo->video_frame.height);
	shoot_full_picture(pstVideo);
	return 0;
}

/* 注意退出前释放资源，销毁VENC通道，否则下次使用会报错！！ */
int destroy_jpeg_chn()
{
	int nRet = 0;
	if (g_nRectEncCreateFlag)
	{
		g_nRectEncCreateFlag = 0;
		nRet = hi_mpi_venc_destroy_chn(RECT_ENCODER_CHN);
		if (nRet != HI_SUCCESS)
			printf("[%s %d] destroy rect venc channel(%d) err nRet(%0#x)\n", __func__, __LINE__, RECT_ENCODER_CHN, nRet);
	}
	if (g_nFullPicEncCreateFlag)
	{
		g_nFullPicEncCreateFlag = 0;
		nRet = hi_mpi_venc_destroy_chn(FULLPIC_ENCODER_CHN);
		if (nRet != HI_SUCCESS)
			printf("[%s %d] destroy fullpic venc channel(%d) err nRet(%0#x)\n", __func__, __LINE__, FULLPIC_ENCODER_CHN, nRet);
	}
	return 0;
}