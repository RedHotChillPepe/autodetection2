#include <hi_math.h>
#include <sys/time.h>
#include <stdlib.h>
#include "gsf_param.h"
#include "hi_common_vgs.h"
#include "hi_common_venc.h"
#include "hi_common_video.h"
#include "hi_mpi_vgs.h"
#include "hi_mpi_venc.h"
#include "hi_mpi_vpss.h"
#include "hi_mpi_vb.h"
#include "hi_common_vb.h"
#include "hi_common_vpss.h"

#ifdef GSF_VPSS_YUV_SUPP
#include "gsf_vpss_comm.h"

#ifndef CEILING_2_POWER
#define CEILING_2_POWER(x,a)     ( ((x) + ((a) - 1) ) & ( ~((a) - 1) ) )
#endif
#define GSF_VENC_CAPTURE_FILE_PATH "/tmp"

typedef struct _GSF_VENC_CAPTURE_RECT_
{
	unsigned short u16Left;
	unsigned short u16Top;
	unsigned short u16Right;
	unsigned short u16Bottom;
}GSF_VENC_CAPTURE_RECT_S;

typedef struct _GSF_AV_FACE_MAN_S
{
	int nVEncChn;
	int nRectEncCreateFlag;
	int nPicEncCreateFlag;
	hi_size stRectSize;
	hi_size stPicSize;
	unsigned int u32FrameNo;
	int nRectNum;
	void *pUser;
}GSF_VENC_CAPTURE_MAN_S;
#define VENC_MAX_CH_NUM 16
static GSF_VENC_CAPTURE_MAN_S gVencCapMan[VENC_MAX_CH_NUM] = {0};


#define GSF_VENC_CAPTURE_RESOLUTION_WIDTH  1920
#define GSF_VENC_CAPTURE_RESOLUTION_HEIGHT 1080

#define GSF_VENC_CAPTURE_RECT_ENCODER_CHN 9
#define GSF_VENC_CAPTURE_PIC_ENCODER_CHN  10
hi_vb_blk g_VencBlock = HI_INVALID_HANDLE;
hi_vb_pool g_venchPool = HI_VB_INVALID_POOL_ID;

//#define GSF_VENC_CAPTURE_SAVE_TEST_FILE 1

int gsf_venc_capture_draw_rect(hi_video_frame_info* pstFrmInfo,int nCh)
{
    hi_s32 s32Ret = HI_SUCCESS;

    hi_vgs_handle handle;
    hi_vgs_task_attr task;
    GSF_VENC_CAPTURE_RECT_S *pRect = NULL; 
    int nRectCnt = gVencCapMan[nCh].nRectNum;
	hi_cover cover = {0};	//多个矩形创建数组
	
    if (nRectCnt == 0)
        return -1;

    s32Ret = hi_mpi_vgs_begin_job(&handle);
    if (s32Ret != HI_SUCCESS)
    {
        printf("Vgs begin job fail,Error(%#x)\n", s32Ret);
        return s32Ret;
    }

    memcpy(&task.img_in, pstFrmInfo, sizeof(ot_video_frame_info));
    memcpy(&task.img_out, pstFrmInfo, sizeof(ot_video_frame_info));

    while(nRectCnt-- > 0)
    {
    	cover.type = OT_COVER_QUAD;
	    cover.color = 0x00FF0000;
#ifdef GSFSS928_ARCH
	    cover.quad.is_solid = HI_FALSE;
	    cover.quad.thick = 2;
	    
        cover.quad.point[0].x = pRect->u16Left & 0xFFFE;
        cover.quad.point[0].y = pRect->u16Top & 0xFFFE;
        cover.quad.point[1].x = pRect->u16Right & 0xFFFE;
        cover.quad.point[1].y = pRect->u16Top & 0xFFFE;
        cover.quad.point[2].x = pRect->u16Right & 0xFFFE;
        cover.quad.point[2].y = pRect->u16Bottom & 0xFFFE;
        cover.quad.point[3].x = pRect->u16Left & 0xFFFE;
        cover.quad.point[3].y = pRect->u16Bottom & 0xFFFE;
#elif (defined GSF3519DV5_ARCH)
		cover.quad_attr.is_solid = HI_FALSE;
	    cover.quad_attr.thick = 2;
	    
        cover.quad_attr.point[0].x = pRect->u16Left & 0xFFFE;
        cover.quad_attr.point[0].y = pRect->u16Top & 0xFFFE;
        cover.quad_attr.point[1].x = pRect->u16Right & 0xFFFE;
        cover.quad_attr.point[1].y = pRect->u16Top & 0xFFFE;
        cover.quad_attr.point[2].x = pRect->u16Right & 0xFFFE;
        cover.quad_attr.point[2].y = pRect->u16Bottom & 0xFFFE;
        cover.quad_attr.point[3].x = pRect->u16Left & 0xFFFE;
        cover.quad_attr.point[3].y = pRect->u16Bottom & 0xFFFE;
#endif
        pRect++;
    }
	s32Ret = hi_mpi_vgs_add_cover_task(handle, &task, &cover, nRectCnt);
    if (s32Ret != HI_SUCCESS)
    {
#ifdef GSFSS928_ARCH
        printf("hi_mpi_vgs_add_cover_task Error(%#x) rangle((%d %d) (%d %d) (%d %d) (%d %d))\n", s32Ret, 
			cover.quad.point[0].x, cover.quad.point[0].y, 
			cover.quad.point[1].x, cover.quad.point[1].y,
			cover.quad.point[2].x, cover.quad.point[2].y,
			cover.quad.point[3].x, cover.quad.point[3].y);
#elif (defined GSF3519DV5_ARCH)
		printf("hi_mpi_vgs_add_cover_task Error(%#x) rangle((%d %d) (%d %d) (%d %d) (%d %d))\n", s32Ret, 
			cover.quad_attr.point[0].x, cover.quad_attr.point[0].y, 
			cover.quad_attr.point[1].x, cover.quad_attr.point[1].y,
			cover.quad_attr.point[2].x, cover.quad_attr.point[2].y,
			cover.quad_attr.point[3].x, cover.quad_attr.point[3].y);
#endif
		hi_mpi_vgs_cancel_job(handle);
        return s32Ret;
    }
    
    s32Ret = hi_mpi_vgs_end_job(handle);
    if (s32Ret != HI_SUCCESS)
    {
        printf("hi_mpi_vgs_end_job fail,Error(%#x)\n", s32Ret);
        hi_mpi_vgs_cancel_job(handle);
        return s32Ret;
    }
    printf("hi_mpi_vgs_end_job success!\n");

    return s32Ret;
}

int gsf_venc_capture_send_venc_frame(int nVencChn, int nDrawRect, hi_video_frame_info *pVideoFrame)
{
	int    s32Ret = 0;
	int nCh = 0;
	if (nDrawRect)
    {
        gsf_venc_capture_draw_rect(pVideoFrame,nCh);
    }

	s32Ret = hi_mpi_venc_send_frame(nVencChn, pVideoFrame, 120);

	if(s32Ret != HI_SUCCESS)
	{
		printf("[%s %d]VencChn(%d) hi_mpi_venc_send_frame err(%#x)\n",__func__,__LINE__,nVencChn,s32Ret);
		return s32Ret;
	}
	
	return 0;
}

int gsf_venc_capture_vpss_get_frame(int nVpssGrpNo, int nVpssChn,hi_video_frame_info *pFrame)
{
    int s32Ret = 0;

	gsf_vpss_set_depth(nVpssGrpNo, nVpssChn, 1);

	if ((s32Ret = hi_mpi_vpss_get_chn_frame(nVpssGrpNo, nVpssChn, pFrame, 100)) != HI_SUCCESS)
	{
		printf( "hi_mpi_vpss_get_chn_frame ERR :%#x\n", s32Ret);	
		return s32Ret;
	}
	return 0;
}


char *gsf_venc_capture_get_picture_stream(int nVEncChn, int *pSize, hi_video_frame_info *pFrame)
{   
    int    s32Ret = 0;
    int    VencFd = 0;
    fd_set read_fds;
    struct timeval  TimeoutVal = {0, 0};   
    hi_venc_chn_status status = {0};
    hi_venc_start_param recv_param = {0};
    hi_venc_stream stream = {0};
    
    int nVEncWrPos = 0;         //前面加两个字节(0xff, 0xd8)才是标准jpg文件
    int count      = 0;
    int max_buff_size = 1024 * 1024 * 2;
    static char  *pVEncBuffer = NULL;
    static char  *pDataBuffer = NULL;

    if(pSize == NULL)
    {
        printf("invalid argment: pSize %p\n", pSize);
        return NULL;
    }

    if (pVEncBuffer == NULL && (pVEncBuffer = (char *)calloc(1, max_buff_size)) == NULL)
        goto venc_capture_get_pic_stream_err;     

     if (pDataBuffer == NULL)
     {
         if ((pDataBuffer = (char *)calloc(1, max_buff_size + 200)) == NULL)
            goto venc_capture_get_pic_stream_err;
         else
            pDataBuffer = pDataBuffer + 200;
    }
   
    //抓图编码处理
    if ((VencFd = hi_mpi_venc_get_fd(nVEncChn)) <= 0)
    {
        printf("hi_mpi_venc_get_fd err %0#x\n", VencFd);
        goto venc_capture_get_pic_stream_err;
    }

    recv_param.recv_pic_num = -1;
    if ((s32Ret =  hi_mpi_venc_start_chn(nVEncChn, &recv_param)) != HI_SUCCESS)
    {
        printf("start venc chn %d hi_mpi_venc_start_chn is err: %#x.\n", nVEncChn, s32Ret);
        goto venc_capture_get_pic_stream_err;
    }       
    printf("[%s %d]\n",__func__,__LINE__);
    while(count++ < 3)
    {
        if (gsf_venc_capture_send_venc_frame(nVEncChn, nVEncChn == GSF_VENC_CAPTURE_PIC_ENCODER_CHN, pFrame) != 0)
        {
            *pSize = 0;
            if (HI_SUCCESS != (s32Ret = hi_mpi_venc_stop_chn(nVEncChn)))
            {
                printf("hi_mpi_venc_stop_chn err %0#x\n", s32Ret);
            }
            else
            {
            	hi_mpi_venc_reset_chn(nVEncChn);
            }
			if(HI_SUCCESS !=(s32Ret = hi_mpi_vpss_release_chn_frame(0, 0, pFrame)))		
			{
				printf("[%s %d]HI_MPI_VPSS_ReleaseChnFrame ch %d err %0#x\n",__func__,__LINE__, nVEncChn, s32Ret);
			}	
			else			
				printf("[%s %d]HI_MPI_VPSS_ReleaseChnFrame ch %d ok\n",__func__,__LINE__, nVEncChn);

            printf("gsf_venc_capture_send_venc_frame failed.\n");
            
            return NULL;
        }

        FD_ZERO(&read_fds);
        FD_SET(VencFd, &read_fds);
        TimeoutVal.tv_sec  = 0;
        TimeoutVal.tv_usec = 120000;
        s32Ret = select(VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if(s32Ret <= 0 || !FD_ISSET(VencFd, &read_fds))
        {
            printf("select error %d\n", nVEncChn);   
            continue;
        }
        memset(&stream, 0, sizeof(hi_venc_stream));
        memset(&status,   0, sizeof(status));

		if ((s32Ret = hi_mpi_venc_query_status(nVEncChn, &status)) != HI_SUCCESS) 
        {
            printf("get_picture_encoder_stream: hi_mpi_venc_query_status: %0#x(%d %d)\n", s32Ret, 0, nVEncChn); 
            usleep(100 * 1000);
            continue;
        }
        //数据太大，咋处理 //数据包个数
        if( (sizeof(hi_venc_pack) * status.cur_packs ) > max_buff_size || status.cur_packs < 1)
        {
            printf("get_picture_encoder_stream: fill data err = %d %d(%d %d)\n", sizeof(hi_venc_pack) * status.cur_packs,
                status.cur_packs, 0, nVEncChn);
            continue;
        }

		//printf("get_picture_stream: u32CurPacks %u, dataLen=sizeof(VENC_PACK_S)*%u=%u\n", 
		//	stStat.u32CurPacks, sizeof(VENC_PACK_S), sizeof(VENC_PACK_S) * stStat.u32CurPacks);
        
        //获取码流
        stream.pack = (hi_venc_pack *)pVEncBuffer;
        stream.pack_cnt = status.cur_packs;
        s32Ret = hi_mpi_venc_get_stream(nVEncChn, &stream, HI_TRUE);
        if (s32Ret) 
        {
            printf("get_picture_encoder_stream: hi_mpi_venc_get_stream err %0#x(%d, %d)\n", s32Ret, 0, nVEncChn); 
            continue;
        }

#ifdef GSF_VENC_CAPTURE_SAVE_TEST_FILE
		int i = 0;
        for (i = 0; i < stream.pack_cnt; i++)
        {
            if(nVEncWrPos + stream.pack[i].len> max_buff_size)                   
            {
                printf("######### get_picture_encoder_stream data too long: WrPos = %d u32Len[0] = %d (buf = %d)\n", nVEncWrPos, stream.pack[i].len, max_buff_size);
                break;
            }
            memcpy((char *)(pDataBuffer + nVEncWrPos), stream.pack[i].addr + stream.pack[i].offset, stream.pack[i].len-stream.pack[i].offset);
            nVEncWrPos += stream.pack[i].len-stream.pack[i].offset;
        }
#endif
		hi_mpi_venc_release_stream(nVEncChn, &stream);
		if (pSize)
        	*pSize = nVEncWrPos;

		if (HI_SUCCESS != (s32Ret = hi_mpi_venc_stop_chn(nVEncChn)))
        {
            printf("hi_mpi_venc_stop_chn err %0#x\n", s32Ret);
        }      
        else
        {
        	hi_mpi_venc_reset_chn(nVEncChn);
        }
		
		if(HI_SUCCESS !=(s32Ret = hi_mpi_vpss_release_chn_frame(0, 0, pFrame))) 	
		{
			printf("[%s %d]HI_MPI_VPSS_ReleaseChnFrame ch %d err %0#x\n",__func__,__LINE__, nVEncChn, s32Ret);
		}	
		else			
			printf("[%s %d]HI_MPI_VPSS_ReleaseChnFrame ch %d ok\n",__func__,__LINE__, nVEncChn);
        return pDataBuffer;
    }

	if (HI_SUCCESS != (s32Ret = hi_mpi_venc_stop_chn(nVEncChn)))
    {
        printf("hi_mpi_venc_stop_chn ch %d err %0#x\n", nVEncChn, s32Ret);
    }   
    else
    {
    	hi_mpi_venc_reset_chn(nVEncChn);
    }
	
	if(HI_SUCCESS !=(s32Ret = hi_mpi_vpss_release_chn_frame(0, 0, pFrame))) 	
	{
		printf("[%s %d]HI_MPI_VPSS_ReleaseChnFrame ch %d err %0#x\n",__func__,__LINE__, nVEncChn, s32Ret);
	}	
	else			
		printf("[%s %d]HI_MPI_VPSS_ReleaseChnFrame ch %d ok\n",__func__,__LINE__, nVEncChn);
venc_capture_get_pic_stream_err:
    *pSize = 0;
    return NULL;
}

int gsf_venc_capture_create_picture_encoder(int nVEncChn, int nEncWidth, int nEncHeight)
{
    hi_venc_chn_attr attr;
    hi_venc_jpeg_attr *pJpegAttr= NULL;
    
	memset(&attr, 0, sizeof(attr));

    pJpegAttr = &attr.venc_attr.jpeg_attr;
    attr.venc_attr.max_pic_width  = nEncWidth;
    attr.venc_attr.max_pic_height = nEncHeight;
    attr.venc_attr.pic_width  = nEncWidth;
    attr.venc_attr.pic_height = nEncHeight;
    attr.venc_attr.buf_size = HI_ALIGN_UP((HI_ALIGN_UP(nEncWidth, 64))*(HI_ALIGN_UP(nEncHeight, 64)), 4096);//CEILING_2_POWER(nEncWidth, 16) * CEILING_2_POWER(nEncHieght, 16) * 2; 
    if (attr.venc_attr.buf_size < 32768)
		attr.venc_attr.buf_size = 32768;
	attr.venc_attr.is_by_frame = HI_TRUE;/*get stream mode is field mode  or frame mode*/    
    pJpegAttr->dcf_en = HI_FALSE;
    attr.venc_attr.type = OT_PT_JPEG;

	return hi_mpi_venc_create_chn(nVEncChn, &attr);
}

int gsf_venc_capture_set_jpeg_attr(int nVencChn)
{
	int u32QFactor = 99;
	int s32Ret = 0;
	hi_venc_jpeg_param jpeg_param;

	if (HI_SUCCESS != (s32Ret = hi_mpi_venc_get_jpeg_param(nVencChn, &jpeg_param)))
	{
		printf("hi_mpi_venc_get_jpeg_param err %0#x\n", s32Ret);
		return -1;
	}
	if (jpeg_param.qfactor != u32QFactor)
	{
		jpeg_param.qfactor = u32QFactor;
		if (HI_SUCCESS != (s32Ret = hi_mpi_venc_set_jpeg_param(nVencChn, &jpeg_param)))
		{
			printf("hi_mpi_venc_set_jpeg_param err %0#x\n", s32Ret);
			return -1;
		}
	}
	return HI_SUCCESS;
}

char *gsf_venc_get_capture_shoot_rect_picture(GSF_U32 *pSize, int nVpssChn)
{
	
    int    s32Ret = 0;
	hi_video_frame_info stFrame = {0};
	int nVpssGrpNo = 0;// nVpssChn = 0;
	int    nVEncChn = GSF_VENC_CAPTURE_RECT_ENCODER_CHN + nVpssChn;

	if(( s32Ret = gsf_venc_capture_vpss_get_frame(nVpssGrpNo, nVpssChn, &stFrame)) != HI_SUCCESS)
	{
		printf("[%s %d]nVpssGrpNo(%d) nVpssChn(%d) get frame err(%#x)\n",__func__,__LINE__,
				nVpssGrpNo,nVpssChn,s32Ret);
		return NULL;
	}
	
    return gsf_venc_capture_get_picture_stream(nVEncChn, pSize, &stFrame);
}

int gsf_venc_create_rect_picture_encoder(GSF_U32 *pSize, GSF_RECT_S *pRect, int nPicWidth, int nPicHeight, void *pUser, int nVpssChn)
{
	  int    s32Ret = 0;
    int    nVEncChn = GSF_VENC_CAPTURE_RECT_ENCODER_CHN+nVpssChn;
    hi_venc_chn_param chn_param;
	int nVpssGrpNo = 0;
	
    //内存是否正常
    if(pSize == NULL || nPicWidth == 0 || nPicHeight == 0)
    {
        printf("shoot rect picture argument invalid!! pSize %p, Pic(%u, %u)\n", pSize, nPicWidth, nPicHeight);
        return -1;
    }

    if( pRect->s32X % 16 != 0 || pRect->s32Y % 16 != 0)
    {
    	pRect->s32X = pRect->s32X/16 * 16 + 16;
    	pRect->s32Y = pRect->s32Y/16 * 16 + 16;
    }
	
    if (nPicWidth > pRect->u32Width)
    {
        nPicWidth = pRect->u32Width;
    }
    if (nPicHeight > pRect->u32Height)
    {
        nPicHeight = pRect->u32Height;
    }

	
	if (gVencCapMan[nVEncChn].nPicEncCreateFlag)
	{
		hi_mpi_venc_destroy_chn(GSF_VENC_CAPTURE_RECT_ENCODER_CHN);
		gVencCapMan[nVEncChn].nPicEncCreateFlag = 0;
	}
	if (gsf_venc_capture_create_picture_encoder(GSF_VENC_CAPTURE_RECT_ENCODER_CHN, nPicWidth, nPicHeight) != GSF_SUCCESS)
	{
		*pSize = 0;
		return -1;
	}
	gsf_venc_capture_set_jpeg_attr(nVEncChn);

	if ((s32Ret = hi_mpi_venc_get_chn_param(nVEncChn, &chn_param)) != HI_SUCCESS)
    {
        printf("hi_mpi_venc_get_chn_param for crop set (%d, rect(%d %d %d %d)) err %0#x\n", 
                   nVEncChn, pRect->s32X, pRect->s32Y, pRect->u32Width, pRect->u32Height, s32Ret);
        *pSize = 0;

        return -1;
    }

	chn_param.crop_info.enable = HI_TRUE;
	chn_param.crop_info.rect.x = pRect->s32X;
	chn_param.crop_info.rect.y = pRect->s32Y;
	chn_param.crop_info.rect.width = pRect->u32Width;
	chn_param.crop_info.rect.height = pRect->u32Height;
	if ((s32Ret = hi_mpi_venc_set_chn_param(nVEncChn, &chn_param)) != HI_SUCCESS)
    {
        printf("hi_mpi_venc_set_chn_param for crop set (%d, rect(%d %d %d %d)) err %0#x\n", 
                   nVEncChn, pRect->s32X, pRect->s32Y, pRect->u32Width, pRect->u32Height, s32Ret);
        *pSize = 0;

        return -1;
    }
	gVencCapMan[nVEncChn].nVEncChn = nVEncChn;
	return 0;
}
char *gsf_venc_get_capture_shoot_picture(GSF_U32 *pSize, int nVpssChn)
{
	int    s32Ret = 0;
	hi_video_frame_info stFrame = {0};
	int nVpssGrpNo = 0;// nVpssChn = 0;
    int nVEncChn = GSF_VENC_CAPTURE_PIC_ENCODER_CHN + nVpssChn;

    if(pSize == NULL)
    {
        printf("invalid argment: pSize %p\n", pSize);
        return NULL;
    }

	if(( s32Ret = gsf_venc_capture_vpss_get_frame(nVpssGrpNo, nVpssChn, &stFrame)) != HI_SUCCESS)
	{
		printf("[%s %d]nVpssGrpNo(%d) nVpssChn(%d) get frame err(%#x)\n",__func__,__LINE__,
				nVpssGrpNo,nVpssChn,s32Ret);
		return NULL;
	}	
    return gsf_venc_capture_get_picture_stream(nVEncChn, pSize, &stFrame);

}

int gsf_venc_create_picture_encoder(GSF_U32 *pSize, int nVpssChn)
{
	int    s32Ret = 0;
    int nVEncChn = GSF_VENC_CAPTURE_PIC_ENCODER_CHN + nVpssChn;
    GSF_U32 u32Width = 0;
    GSF_U32 u32Height = 0;
	int nVpssGrp = 0;
	u32Width  =  GSF_VENC_CAPTURE_RESOLUTION_WIDTH;
	u32Height =  GSF_VENC_CAPTURE_RESOLUTION_HEIGHT;

    if(pSize == NULL)
    {
        printf("invalid argment: pSize %p\n", pSize);
        return -1;
    }
	hi_vpss_chn_attr stChnAttr = {0};
	if (0 != (s32Ret = hi_mpi_vpss_get_chn_attr(nVpssGrp, nVpssChn, &stChnAttr)))
	{
		printf("[%s %d] hi_mpi_vpss_get_chn_attr failed with %#x!\n", __func__, __LINE__, s32Ret);	
		return -1;
	}

	if(stChnAttr.width != 0 || stChnAttr.height != 0)
	{
		u32Width = stChnAttr.width;
		u32Height = stChnAttr.height;
	}
	
	if (gVencCapMan[nVEncChn].nPicEncCreateFlag)
	{
		hi_mpi_venc_destroy_chn(GSF_VENC_CAPTURE_PIC_ENCODER_CHN);
		gVencCapMan[nVEncChn].nPicEncCreateFlag = 0;
	}
	int nRet = 0;
	if ((nRet = gsf_venc_capture_create_picture_encoder(GSF_VENC_CAPTURE_PIC_ENCODER_CHN, u32Width, u32Height)) != GSF_SUCCESS)
	{
		printf("[%s %d]gsf_venc_capture_create_picture_encoder err(%0#x)\n",__func__,__LINE__,nRet);
		*pSize = 0;
		return -1;
	}

	gVencCapMan[nVEncChn].nPicEncCreateFlag =1;
	gVencCapMan[nVEncChn].nVEncChn = nVEncChn;

    return 0;
}


#ifdef GSF_VENC_CAPTURE_SAVE_TEST_FILE
int gsf_venc_capture_save_pic_file(char *pFileName, char *pData, int nLen)
{
	if (pFileName == NULL || pData == NULL || nLen <= 0)
		return -1;
	FILE *fp = fopen(pFileName, "wb");
	if (fp)
	{
		if (fwrite(pData, nLen, 1, fp) == 1)
			printf("[%s %d]: write file[%s] nLen[%d] ok!!\n", __FILE__, __LINE__, pFileName, nLen);
		fclose(fp);
		fp = NULL;
	}
	else
		printf("[%s %d]: open file[%s] nLen[%d] err!!\n", __FILE__, __LINE__,
				pFileName, nLen);
	return 0;
}
#endif

int gsf_venc_capture_start(int nVpssChn, GSF_RECT_S *pRect, int nWidth, int nHeight,GSF_U32 *nFileSize, void *pUser)
{
	int nRet = -1;
	if(pRect->u32Width != 0 || pRect->u32Height != 0)
	{		
		nRet = gsf_venc_create_rect_picture_encoder(nFileSize, pRect, nWidth, nHeight, pUser, nVpssChn);
	}
	else
	{			
		nRet = gsf_venc_create_picture_encoder(nFileSize, nVpssChn);
	}

	if(nRet != 0)
	{
		printf("[%s %d] gsf_venc_capture_start_s err\n",__func__,__LINE__);
		return -1;
	}
	return 0;
}
int gsf_venc_capture_recv(int nVpssChn,GSF_RECT_S *pRect, GSF_U32 *nFileSize,int nWidth, int nHeight)
{

	char *pPicAddr = NULL;
	if(pRect->u32Width != 0 || pRect->u32Height != 0)
	{
		pPicAddr = gsf_venc_get_capture_shoot_rect_picture(nFileSize,nVpssChn);
	}
	else
	{
		pPicAddr = gsf_venc_get_capture_shoot_picture(nFileSize,nVpssChn);
	}
#ifdef GSF_VENC_CAPTURE_SAVE_TEST_FILE
		if(pPicAddr == NULL || *nFileSize == 0)
		{
			printf("shoot rect(%d %d %d %d) picture(%d %d) err!!\n", 
				pRect->s32X, pRect->s32Y, pRect->u32Width, pRect->u32Height, nWidth, nHeight);
		}
		else
		{
			char szFileName[128] = {0};
			struct timeval tvTime;
			gettimeofday(&tvTime, NULL);
			if (pRect->u32Width == 0 && pRect->u32Height == 0)
			{
				sprintf(szFileName, "%s/full_%ld_%03d.jpg", GSF_VENC_CAPTURE_FILE_PATH, tvTime.tv_sec, tvTime.tv_usec/1000);
			}
			else
			{
				sprintf(szFileName, "%s/rect_%ld_%03d.jpg", GSF_VENC_CAPTURE_FILE_PATH, tvTime.tv_sec, tvTime.tv_usec/1000);
			}
			gsf_venc_capture_save_pic_file(szFileName, pPicAddr, *nFileSize);
		}
#endif
		return 0;
}
int gsf_venc_capture_stop(int nVpssChn)
{	
	int s32Ret = 0;
	if (HI_SUCCESS != (s32Ret = hi_mpi_venc_stop_chn(gVencCapMan[nVpssChn].nVEncChn)))
    {
        printf("HI_MPI_VENC_StopRecvFrame ch %d err %0#x\n", gVencCapMan[nVpssChn].nVEncChn, s32Ret);
    }   
    else
    {
        hi_mpi_venc_reset_chn(gVencCapMan[nVpssChn].nVEncChn);
    }
	
	if ((s32Ret = hi_mpi_venc_destroy_chn(gVencCapMan[nVpssChn].nVEncChn)) != GSF_SUCCESS)
	{
		printf("venc chn %d HI_MPI_VENC_DestroyChn err %#x.\n", s32Ret);
	}

	return 0;
}

#endif //GSF_VPSS_YUV_SUPP

