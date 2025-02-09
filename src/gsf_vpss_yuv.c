/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_vpss_yuv.c

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
 * contact: liangym@szzhst.com
 */
#ifdef GSF_VPSS_YUV_SUPP
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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
#include "gsf_vpss_yuv.h"
#include "draw_yuv_rect.h"
#include "gsf_yuv_draw_rect.h"
#include "gsf_vpss_comm.h"
#include "gsf_vpss_yuv_vgs.h"
#include "gsf_vpss_yuv_rgb.h"
#include "gsf_get_jpeg.h"
#include "gsf_venc_jpeg.h"
#include "gsf_yuv_osd.h"
#include "hi_mpi_vb.h"
#include "gsf_param.h"
#include "gsf_web_retrans.h"
#include "gsf_base.h"
static int g_max_supp_stream = 3; // 

#ifdef GSF3519DV5_ARCH
#include "hi_mpi_sys_mem.h"
#endif

#define HI_MAX_VI_CHN 1
#define HI_VI_CHN     0
#define HI_VPSS_GRP   0
#define HI_VPSS_CHN   0
#define HI_VPSS_EXT_CHN 2
#define HI_VPSS_DEPTH 8

#define HI_EXPECTED_WIDTH  416
#define HI_EXPECTED_HEIGHT 416

static hi_video_frame_info g_stFrame;
static int g_nVpssChn = HI_VPSS_CHN;
static int g_nDepth = 0;
static int g_nExit  = 0;
static int g_bResize  = 0;
static int g_expected_width  = HI_EXPECTED_WIDTH;
static int g_expected_height = HI_EXPECTED_HEIGHT;
static int g_unbind_mode[3] = {0}; //index 0 main stream 1 sub stream 2 jpg channel
static unsigned int g_nIsUxcExist=0;



int gsf_vpss_yuv_osd_data_cb(GSF_VPSS_YUV_HDL *pHdl, int nStatus, hi_video_frame_info *pstVideo,unsigned char IsCutFlag)
{
	int nRet = 0;
	unsigned int u32Size = 0;
	unsigned char *pVirtAddr = NULL;
	static GSF_OSD_CFG_S stOsdCfg={0};
	static GSF_NTP_CFG_S stNtpCfg={0};
	static unsigned long long u64LastPts = 0; 
	if (nStatus != DEVSDK_OK)
		return pHdl->fCallback(nStatus, 0, 0, 0, 0, NULL, pHdl->pUser);
	
	if (OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 == pstVideo->video_frame.pixel_format || 
        OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420 == pstVideo->video_frame.pixel_format ||
        OT_PIXEL_FORMAT_YVU_PLANAR_420 == pstVideo->video_frame.pixel_format)
	{
		u32Size = (pstVideo->video_frame.stride[0] * pstVideo->video_frame.height * 3) / 2;
	}else if(OT_PIXEL_FORMAT_BGR_888 == pstVideo->video_frame.pixel_format)
	{
		u32Size = (pstVideo->video_frame.width * pstVideo->video_frame.height * 3);
	}else // 422
	{
		u32Size = (pstVideo->video_frame.width * pstVideo->video_frame.height*2);
	}
	if(!g_nIsUxcExist || IsCutFlag)
	{
		if (NULL == (pVirtAddr = (char *) hi_mpi_sys_mmap(pstVideo->video_frame.phys_addr[0], u32Size)))
		{
			printf("[%s %d] hi_mpi_sys_mmap err! size(%d) stride(%d) width(%d)\n", 
				__func__, __LINE__, u32Size, pstVideo->video_frame.stride[0], pstVideo->video_frame.width);
			return pHdl->fCallback(DEVSDK_ERR_MMAP, 0, 0, 0, 0, NULL, pHdl->pUser);
		}
	}
	else
	{
		if (NULL == (pVirtAddr = (char *) hi_mpi_sys_mmap_cached(pstVideo->video_frame.phys_addr[0], u32Size)))
		{
			printf("[%s %d] hi_mpi_sys_mmap_cached err! size(%d) stride(%d) width(%d)\n", 
				__func__, __LINE__, u32Size, pstVideo->video_frame.stride[0], pstVideo->video_frame.width);
			return pHdl->fCallback(DEVSDK_ERR_MMAP, 0, 0, 0, 0, NULL, pHdl->pUser);
		}
	}
	
	if(pstVideo->video_frame.pts - u64LastPts >= 1000000)
	{
		gsf_web_retrans_proc(NULL, GSF_GET_PARAM_MSG, GSF_OSD_PARAM, 0, (char *)&stOsdCfg, sizeof(GSF_OSD_CFG_S));
		gsf_web_retrans_proc(NULL, GSF_GET_PARAM_MSG, GSF_NTP_PARAM,0,(char *)&stNtpCfg,sizeof(GSF_NTP_CFG_S));
		u64LastPts = pstVideo->video_frame.pts;
	}
	struct tm *tmTime;
	time_t tsec = pstVideo->video_frame.pts/1000000;
	//tmTime = localtime(&tsec);
	tsec += stNtpCfg.nTimeZone;

	tmTime = gmtime(&tsec);
	stYuvOsdInfo pYuvOsdCfg={0};
	stYuvBuffInfo pYuvBuffInfo={0};
	switch(stOsdCfg.u8OsdSize)
	{
		case 0:pYuvOsdCfg.FontSize= TYPE_FONT_SIZE_32P;
			break;
		case 1:pYuvOsdCfg.FontSize= TYPE_FONT_SIZE_48P;
			break;
		case 2:pYuvOsdCfg.FontSize= TYPE_FONT_SIZE_48P;
			break;
		default:pYuvOsdCfg.FontSize= TYPE_FONT_SIZE_48P;
			break;
	}
	char DateType[GSF_OSD_DATE_NUM][32];
	char hourType[GSF_OSD_TIME_NUM][32];
	char TempStr[32];
	char szFlag[32];
	strcpy(DateType[0],"%Y-%m-%d");
    strcpy(DateType[1],"%m-%d-%Y");
    strcpy(DateType[2],"%d-%m-%Y");
    strcpy(DateType[3],"%Y/%m/%d");
    strcpy(DateType[4],"%m/%d/%Y");
    strcpy(DateType[5],"%d/%m/%Y");

    
    strcpy(hourType[0], "%H:%M:%S");
    strcpy(hourType[1],  "%r");

	sprintf(szFlag, ".%03lu", (pstVideo->video_frame.pts/1000)%1000);
	pYuvBuffInfo.pYuvBuff = pVirtAddr;
	pYuvBuffInfo.width =  pstVideo->video_frame.width;
	pYuvBuffInfo.height = pstVideo->video_frame.height;
	pYuvBuffInfo.yuvType = get_yuv_type(pstVideo->video_frame.pixel_format);
	pYuvOsdCfg.FontColor = YUV_WHITE;
	//pYuvOsdCfg.FontSize  = TYPE_FONT_SIZE_48P;
	pYuvOsdCfg.Font_PosX = stOsdCfg.struTimeOrg.s32X*pstVideo->video_frame.width *1.0/704;
	pYuvOsdCfg.Font_PosY = stOsdCfg.struTimeOrg.s32Y*pstVideo->video_frame.height *1.0/576;
	if(stOsdCfg.u8EnableTimeOsd)
	{
		sprintf(TempStr, "%s %s",DateType[stOsdCfg.u8TimeOsdType], 
			                        hourType[stOsdCfg.u8HourOsdType] );
		
		
		strftime(pYuvOsdCfg.OsdTitle, sizeof(pYuvOsdCfg.OsdTitle), TempStr, tmTime);
		strcat(pYuvOsdCfg.OsdTitle,szFlag);
	}
	else
	{
		strcpy(pYuvOsdCfg.OsdTitle,"");
	}
	gsf_yuv_osd_refresh(&pYuvOsdCfg,&pYuvBuffInfo);


	if (g_unbind_mode[0] == 1) // main stream unbind mode
	{
		// send to venc
		if ((nRet = hi_mpi_venc_send_frame(0, pstVideo, 500)) != 0)
			printf("[%s %d] error: main stream hi_mpi_venc_send_frame err(%0#x) \n", __func__, __LINE__, nRet);
	}
	
	if (g_unbind_mode[1] == 1) // sub stream unbind mode
	{
		// send to venc
		if ((nRet = hi_mpi_venc_send_frame(1, pstVideo, 500)) != 0)
			printf("[%s %d] error: sub stream hi_mpi_venc_send_frame err(%0#x) \n", __func__, __LINE__, nRet);
	}
	
	if (g_unbind_mode[2] == 1) // jpg channel unbind mode
	{
		// send to jeeg venc
		//before send to jpg channel ,you need to call get_jpeg_prepare first
		if ((nRet = hi_mpi_venc_send_frame(2, pstVideo, 500)) != 0) 
			printf("[%s %d] error: jpg channel hi_mpi_venc_send_frame err(%0#x) \n", __func__, __LINE__, nRet);
		else 
		{
			get_jpeg_pic_from_encoder(); //获取图片尽量不要在此处处\E7\90?
		}		
	}

	
	pHdl->fCallback(DEVSDK_OK, 
					pstVideo->video_frame.width, 
					pstVideo->video_frame.height, 
					pstVideo->video_frame.pixel_format, 
					u32Size, 
					pVirtAddr,
					pHdl->pUser);

	hi_mpi_sys_munmap(pVirtAddr, u32Size);
}



/////////////////////////////РЕДАКТИРОВАЛ/////////////////////////////////

int gsf_vpss_yuv_data_cb(GSF_VPSS_YUV_HDL *pHdl, int nStatus,
                         hi_video_frame_info *pstVideo, unsigned char IsCutFlag)
{
    int nRet = 0;
    unsigned int u32Size = 0;
    unsigned char *pVirtAddr = NULL;

    if (nStatus != DEVSDK_OK)
        return pHdl->fCallback(nStatus, 0, 0, 0, 0, NULL, pHdl->pUser);

    /* 1) вычисляем размер кадра */
    if (OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 == pstVideo->video_frame.pixel_format ||
        OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420 == pstVideo->video_frame.pixel_format ||
        OT_PIXEL_FORMAT_YVU_PLANAR_420 == pstVideo->video_frame.pixel_format)
    {
        u32Size = (pstVideo->video_frame.stride[0] * pstVideo->video_frame.height * 3) / 2;
    }
    else if (OT_PIXEL_FORMAT_BGR_888 == pstVideo->video_frame.pixel_format)
    {
        u32Size = (pstVideo->video_frame.width * pstVideo->video_frame.height * 3);
    }
    else
    {
        
        u32Size = (pstVideo->video_frame.width * pstVideo->video_frame.height * 2);
    }

    /* 2) mmap кадр (как раньше) */
    if (!g_nIsUxcExist || IsCutFlag)
    {
        pVirtAddr = (unsigned char *)hi_mpi_sys_mmap(pstVideo->video_frame.phys_addr[0], u32Size);
        if (NULL == pVirtAddr)
        {
            printf("[%s %d] hi_mpi_sys_mmap err!\n", __func__, __LINE__);
            return pHdl->fCallback(DEVSDK_ERR_MMAP, 0, 0, 0, 0, NULL, pHdl->pUser);
        }
    }
    else
    {
        pVirtAddr = (unsigned char *) hi_mpi_sys_mmap_cached(pstVideo->video_frame.phys_addr[0], u32Size);
        if (NULL == pVirtAddr)
        {
            printf("[%s %d] hi_mpi_sys_mmap_cached err!\n", __func__, __LINE__);
            return pHdl->fCallback(DEVSDK_ERR_MMAP, 0, 0, 0, 0, NULL, pHdl->pUser);
        }
    }

	
    pHdl->fCallback(DEVSDK_OK,
                    pstVideo->video_frame.width,
                    pstVideo->video_frame.height,
                    pstVideo->video_frame.pixel_format,
                    u32Size,
                    pVirtAddr,
                    pHdl->pUser);

    /* 3) Если нужно — применяем vpss_yuv_vgs_ai_proc. 
          Вместо локального TempData[1], 
          рисуем pHdl->stAiData[0..(nAiDataNum-1)]. */
		  
    if (pHdl->yuvDataProcFlag)
    {
        if (pHdl->nAiDataNum > 0)  // т.е. есть переданные прямоугольники
        {
            /* Передаём все разом на отрисовку */
            vpss_yuv_vgs_ai_proc(pstVideo, pHdl->stAiData, pHdl->nAiDataNum);
        }

        /* далее логика g_unbind_mode (какой канал заанбиндили) -> hi_mpi_venc_send_frame */
        if (g_unbind_mode[0] == 1) { /* main stream */
            nRet = hi_mpi_venc_send_frame(pHdl->nVpssGrp*g_max_supp_stream, pstVideo, 500);
            if (nRet != 0) {
                printf("[%s %d] error: main stream hi_mpi_venc_send_frame err(%#x)\n",
                       __func__, __LINE__, nRet);
            }
        }
        if (g_unbind_mode[1] == 1) { /* sub stream */
            nRet = hi_mpi_venc_send_frame(pHdl->nVpssGrp*g_max_supp_stream+1, pstVideo, 500);
            if (nRet != 0) {
                printf("[%s %d] error: sub stream hi_mpi_venc_send_frame err(%#x)\n",
                       __func__, __LINE__, nRet);
            }
        }
        if (g_unbind_mode[2] == 1) { /* jpg channel */
            nRet = hi_mpi_venc_send_frame(pHdl->nVpssGrp*g_max_supp_stream+2, pstVideo, 500);
            if (nRet != 0) {
                printf("[%s %d] error: jpg channel hi_mpi_venc_send_frame err(%#x)\n",
                       __func__, __LINE__, nRet);
            } else {
                get_jpeg_pic_from_encoder();
            }
        }
    }

    

    /* 5) unmap */
    hi_mpi_sys_munmap(pVirtAddr, u32Size);

    return 0;
}

// int gsf_vpss_yuv_data_cb(GSF_VPSS_YUV_HDL *pHdl, int nStatus,
//                          hi_video_frame_info *pstVideo, unsigned char IsCutFlag)
// {
//     if(nStatus != DEVSDK_OK){
//         // callback об ошибке
//         return pHdl->fCallback(nStatus, 0,0,0,0, NULL, pHdl->pUser);
//     }

//     unsigned int u32Size = 0;
//     // Определяем размер буфера по pixel_format
//     // (если уверены, что это YVU420, можно упростить).
//     if(pstVideo->video_frame.pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 ||
//        pstVideo->video_frame.pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420 ||
//        pstVideo->video_frame.pixel_format == OT_PIXEL_FORMAT_YVU_PLANAR_420)
//     {
//         u32Size = (pstVideo->video_frame.stride[0] * pstVideo->video_frame.height * 3) / 2;
//     }
//     else {
//         // ...другие случаи BGR888 / YUYV...
//         u32Size = (pstVideo->video_frame.width * pstVideo->video_frame.height * 2);
//     }

//     unsigned char *pVirtAddr = NULL;
//     // mmap
//     if(!g_nIsUxcExist || IsCutFlag) {
//         pVirtAddr = (unsigned char*)hi_mpi_sys_mmap(
//             pstVideo->video_frame.phys_addr[0],
//             u32Size
//         );
//     } else {
//         pVirtAddr = (unsigned char*)hi_mpi_sys_mmap_cached(
//             pstVideo->video_frame.phys_addr[0],
//             u32Size
//         );
//     }
//     if(!pVirtAddr){
//         printf("[%s %d] hi_mpi_sys_mmap err!\n", __func__, __LINE__);
//         return pHdl->fCallback(DEVSDK_ERR_MMAP, 0,0,0,0, NULL, pHdl->pUser);
//     }

//     // Если нужно — делаем отрисовку AI-боксов
//     if(pHdl->yuvDataProcFlag)
//     {
//         if(pHdl->nAiDataNum > 0) {
//             // Запускаем отрисовку (рамок, текста) в pstVideo
//             // (использует vpss_yuv_vgs_ai_proc из gsf_vpss_yuv_vgs.c)
//             vpss_yuv_vgs_ai_proc(pstVideo, pHdl->stAiData, pHdl->nAiDataNum);
//         }
//         // После этого отправляем кадр в encoder
//         // (main stream, sub stream, jpg)
//         int nRet = 0;
//         if(g_unbind_mode[0] == 1) {
//             // main stream
//             nRet = hi_mpi_venc_send_frame(pHdl->nVpssGrp*g_max_supp_stream,
//                                           pstVideo, 500);
//             if(nRet!=0){
//                 printf("[%s %d] error: main stream hi_mpi_venc_send_frame err(%#x)\n",
//                        __func__, __LINE__, nRet);
//             }
//         }
//         if(g_unbind_mode[1] == 1) {
//             // sub stream
//             nRet = hi_mpi_venc_send_frame(pHdl->nVpssGrp*g_max_supp_stream+1,
//                                           pstVideo, 500);
//             if(nRet!=0){
//                 printf("[%s %d] error: sub stream hi_mpi_venc_send_frame err(%#x)\n",
//                        __func__, __LINE__, nRet);
//             }
//         }
//         if(g_unbind_mode[2] == 1) {
//             // jpg channel
//             nRet = hi_mpi_venc_send_frame(pHdl->nVpssGrp*g_max_supp_stream+2,
//                                           pstVideo, 500);
//             if(nRet!=0){
//                 printf("[%s %d] error: jpg channel hi_mpi_venc_send_frame err(%#x)\n",
//                        __func__, __LINE__, nRet);
//             } else {
//                 get_jpeg_pic_from_encoder(); // ваш вызов
//             }
//         }
//     }

//     // И наконец, вызываем пользовательский callback (например, my_yuv_callback)
//     pHdl->fCallback(nStatus,
//                     pstVideo->video_frame.width,
//                     pstVideo->video_frame.height,
//                     pstVideo->video_frame.pixel_format,
//                     u32Size,
//                     pVirtAddr,
//                     pHdl->pUser);

//     // unmmap
//     hi_mpi_sys_munmap(pVirtAddr, u32Size);

//     return 0;
// }




/////////////////////////////РЕДАКТИРОВАЛ/////////////////////////////////


int gsf_vpss_yuv_osd_proc(GSF_VPSS_YUV_HDL *pHdl)
{

	if (pHdl->fCallback)
	{
		int nStatus = DEVSDK_OK;
		hi_video_frame_info stVideoDest = {0};
		if (pHdl->nWidth != 0 && pHdl->nHeight != 0 &&
			(pHdl->nWidth  != pHdl->stFrame.video_frame.width ||
			 pHdl->nHeight != pHdl->stFrame.video_frame.height))
		{
			if (vpss_yuv_vgs_proc(&pHdl->stFrame, &stVideoDest, pHdl->nWidth, pHdl->nHeight) != HI_SUCCESS)
			{
				nStatus = DEVSDK_ERR_SCALE;
			}
			return gsf_vpss_yuv_osd_data_cb(pHdl, nStatus, &stVideoDest,1);
		}
		return gsf_vpss_yuv_osd_data_cb(pHdl, nStatus, &pHdl->stFrame,0);
	}
	return 0;
}

//////////////////////////////////НАЧАЛО ИЗМЕНЕНИЯ/////////////////////////////////////////////

int gsf_vpss_yuv_proc(GSF_VPSS_YUV_HDL *pHdl)
{

	if (pHdl->fCallback)
	{
		int nStatus = DEVSDK_OK;
		hi_video_frame_info stVideoDest = {0};
		if (pHdl->nWidth != 0 && pHdl->nHeight != 0 &&
			(pHdl->nWidth  != pHdl->stFrame.video_frame.width ||
			 pHdl->nHeight != pHdl->stFrame.video_frame.height))
		{
			if (vpss_yuv_vgs_proc(&pHdl->stFrame, &stVideoDest, pHdl->nWidth, pHdl->nHeight) != HI_SUCCESS)
			{
				nStatus = DEVSDK_ERR_SCALE;
			}
			return gsf_vpss_yuv_data_cb(pHdl, nStatus, &stVideoDest,1);
		}
		return gsf_vpss_yuv_data_cb(pHdl, nStatus, &pHdl->stFrame,0);
	}
	return 0;
}

// int gsf_vpss_yuv_proc(GSF_VPSS_YUV_HDL *pHdl)
// {
//     if(pHdl->fCallback)
//     {
//         int nStatus = DEVSDK_OK;
//         return gsf_vpss_yuv_data_cb(pHdl, nStatus, &pHdl->stFrame, 0);
//     }
//     return 0;
// }


//////////////////////////////////КОНЕЦ ИЗМЕНЕНИЯ/////////////////////////////////////////////


void Gsf_Get_UXC_dev_Exist(void)
{
	char buff[36]={0};
	/*
	system("touch /tmp/getenv.txt; getenv devFeature >> /tmp/getenv.txt");
	FILE *fp=fopen("/tmp/getenv.txt","r");
	if(fp)
	{
		fread(buff,1,sizeof(buff),fp);
		sscanf(buff,"get env value devFeature=%u",&g_nIsUxcExist);
		printf("[%s,%d]buff[%s]\n",__FUNCTION__,__LINE__,buff);
		fclose(fp);
		unlink("/tmp/getenv.txt");
	}*/
	FILE* pp=popen("getenv devFeature","r");
	if(pp)
	{
		fgets(buff,sizeof(buff),pp);
		sscanf(buff,"get env value devFeature=%u",&g_nIsUxcExist);
		g_nIsUxcExist &= 0x200000;
		pclose(pp);
	}
}

void *gsf_vpss_yuv_osd_task(void *arg)
{
	GSF_VPSS_YUV_HDL *pHdl = (GSF_VPSS_YUV_HDL *)arg;
	int nRet = 0;
	printf("vpss grp(%d) chn(%d)\n", pHdl->nVpssGrp, pHdl->nVpssChn);
	if (0 > (pHdl->nDepth = gsf_vpss_set_depth(pHdl->nVpssGrp, pHdl->nVpssChn, HI_VPSS_DEPTH)))
	{
		printf("[%s %d] set frame depth(%d) err\n", __func__, __LINE__, HI_VPSS_DEPTH);
		return NULL;
	}


	Gsf_Get_UXC_dev_Exist();
	
	

	while(pHdl->nExit == 0)
	{
		usleep(20000);
		// get frame
		if ((nRet = hi_mpi_vpss_get_chn_frame(pHdl->nVpssGrp, pHdl->nVpssChn, &pHdl->stFrame, 100)) != 0)
		{
			printf("[%s %d] hi_mpi_vpss_get_chn_frame err(%0#x)\n", __func__, __LINE__, nRet);
			continue;
		}
		gsf_vpss_yuv_osd_proc(pHdl);
		// release frame
		if ((nRet = hi_mpi_vpss_release_chn_frame(pHdl->nVpssGrp, pHdl->nVpssChn, &pHdl->stFrame)) != 0)
		{
			printf("[%s %d] hi_mpi_vpss_release_chn_frame err %0#x\n", __func__, __LINE__, nRet);
		}
	}
	// resume depth
	if (0 > gsf_vpss_set_depth(pHdl->nVpssGrp, pHdl->nVpssChn, pHdl->nDepth))
	{
		
		printf("[%s %d] set frame depth(%d) err\n", __func__, __LINE__, pHdl->nDepth);
	}
	return NULL;
}

//////////////////////////////////НАЧАЛО ИЗМЕНЕНИЯ/////////////////////////////////////////////

// gsf_vpss_yuv.c
// void *gsf_vpss_yuv_task(void *arg)
// {
//     GSF_VPSS_YUV_HDL *pHdl = (GSF_VPSS_YUV_HDL *)arg;
//     int nRet = 0;
//     printf("vpss grp(%d) chn(%d)\n", pHdl->nVpssGrp, pHdl->nVpssChn);

//     if (0 > (pHdl->nDepth = gsf_vpss_set_depth(pHdl->nVpssGrp, pHdl->nVpssChn, HI_VPSS_DEPTH)))
//     {
//         printf("[%s %d] set frame depth(%d) err\n", __func__, __LINE__, HI_VPSS_DEPTH);
//         return NULL;
//     }
//     gsf_pthread_set_name("vpssYuvTask%d%d", pHdl->nVpssGrp, pHdl->nVpssChn);

//     // (проверяем наличие /dev/uxc)
//     Gsf_Get_UXC_dev_Exist();

//     while(!pHdl->nExit)
//     {
//         // уменьшили с 20000 до 3000 мкс
//         usleep(3000);

//         // get frame
//         nRet = hi_mpi_vpss_get_chn_frame(pHdl->nVpssGrp, pHdl->nVpssChn, &pHdl->stFrame, 50);
//         if(nRet!=0){
//             // можно в отладочной версии распечатывать
//             continue;
//         }
//         // вызов обработчика
//         gsf_vpss_yuv_proc(pHdl);

//         // release frame
//         nRet = hi_mpi_vpss_release_chn_frame(pHdl->nVpssGrp, pHdl->nVpssChn, &pHdl->stFrame);
//         // можно не печатать ошибку, если она может сыпаться часто
//     }
//     // восстанавливаем depth
//     gsf_vpss_set_depth(pHdl->nVpssGrp, pHdl->nVpssChn, pHdl->nDepth);
//     return NULL;
// }

void *gsf_vpss_yuv_task(void *arg)
{
    GSF_VPSS_YUV_HDL *pHdl = (GSF_VPSS_YUV_HDL *)arg;
    int nRet = 0;

    printf("vpss grp(%d) chn(%d)\n", pHdl->nVpssGrp, pHdl->nVpssChn);

    // Устанавливаем глубину кадров (сколько кадров VPSS может «держать»)
    if (0 > (pHdl->nDepth = gsf_vpss_set_depth(pHdl->nVpssGrp, pHdl->nVpssChn, HI_VPSS_DEPTH)))
    {
        printf("[%s %d] set frame depth(%d) err\n", __func__, __LINE__, HI_VPSS_DEPTH);
        return NULL;
    }
    gsf_pthread_set_name("vpssYuvTask%d%d", pHdl->nVpssGrp, pHdl->nVpssChn);

    Gsf_Get_UXC_dev_Exist(); // Проверяем наличие /dev/uxc (если нужно)
    vpss_yuv_vgs_init_osd(32);

    while(!pHdl->nExit)
    {
        // Снизим паузу c 20000 до 3000 мкс
        // чтобы не держать кадры слишком долго
        usleep(3000);

        // get frame
        nRet = hi_mpi_vpss_get_chn_frame(pHdl->nVpssGrp, pHdl->nVpssChn,
                                         &pHdl->stFrame, 50 /*timeout*/);
        if(nRet != HI_SUCCESS)
        {
            // В случае таймаута или др. ошибок —
            // не будем печатать миллион логов, просто continue
            continue;
        }

        // Обработка кадра (ваш callback)
        gsf_vpss_yuv_proc(pHdl);

        // Release frame
        nRet = hi_mpi_vpss_release_chn_frame(pHdl->nVpssGrp, pHdl->nVpssChn, &pHdl->stFrame);
        // Обычно не печатаем nRet, чтобы не заспамить логи
    }

    // Восстанавливаем глубину
	vpss_yuv_vgs_deinit_osd();
    gsf_vpss_set_depth(pHdl->nVpssGrp, pHdl->nVpssChn, pHdl->nDepth);

    return NULL;
}




//////////////////////////////////КОНЕЦ ИЗМЕНЕНИЯ/////////////////////////////////////////////



int gsf_vpss_yuv_stop(void *hdl)
{
	GSF_VPSS_YUV_HDL *pHdl = (GSF_VPSS_YUV_HDL *)hdl;
	pHdl->nExit = 1;
	pthread_join(pHdl->thid, NULL);
	if(pHdl->yuvDataProcFlag)
	{
		destroy_jpeg_chn();
		vpss_yuv_rgb_uninit();	//rgb去初始化

		if(pHdl->userMsg == 3)
		{
			if (release_vpss_chn(pHdl->nVpssGrp, pHdl->nVpssChn) != HI_SUCCESS)
			{
				printf("[%s %d] release vpss chn err!!!",__func__,__LINE__);
				return -1;
			}
		}
#if 0
		if(pHdl->nVpssChn == 0)
		{
			if(remove("/etc/vpss_unbind_venc") != 0)
			{
				printf("[%s %d] creat vpssVencChn(%d) fail!!!\n",__func__,__LINE__,pHdl->nVpssChn);
				return -1;
			}
		}
		if(pHdl->nVpssChn == 1)
		{
			if(remove("/etc/vpss_unbind_venc1") != 0)
			{
				printf("[%s %d] creat vpssVencChn(%d) fail!!!\n",__func__,__LINE__,pHdl->nVpssChn);
				return -1;
			}
		}
		if(pHdl->nVpssChn == 2)
		{
			if(get_jpeg_finish() != 0)
			{
				printf("[%s %d] jpeg finish err!!!\n",__func__,__LINE__);
				return -1;
			}
		
			if(remove("/etc/vpss_unbind_jpg") != 0)
			{
				printf("[%s %d] creat vpssVencChn(%d) fail!!!\n",__func__,__LINE__,pHdl->nVpssChn);
				return -1;
			}
		}
#endif
	}

	vpss_yuv_vgs_uninit();
	if (pHdl->nBufSize > 0 && pHdl->pBuf != NULL)
	{
		free(pHdl->pBuf);
		pHdl->pBuf = NULL;
		pHdl->nBufSize = 0;
	}
	return 0;
}

int gsf_vpss_bgr_data_cb(GSF_VPSS_BGR_HDL *pHdl, int nStatus, hi_video_frame_info *pstVideo)
{
/*
	unsigned int u32Size = 0;
	unsigned char *pVirtAddr = NULL;

	if (nStatus != DEVSDK_NOERROR)
		return pHdl->fCallback(nStatus, 0, 0, 0, NULL, pHdl->pUser);
	
	if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == pstVideo->stVFrame.enPixelFormat || 
        PIXEL_FORMAT_YVU_SEMIPLANAR_420 == pstVideo->stVFrame.enPixelFormat)
	{
		u32Size = (pstVideo->stVFrame.u32Stride[0] * pstVideo->stVFrame.u32Height * 3) / 2;
	}
	else // 422
	{
		u32Size = (pstVideo->stVFrame.u32Stride[0]*pstVideo->stVFrame.u32Height*2);
	}
	if (NULL == (pVirtAddr = (char *) HI_MPI_SYS_Mmap(pstVideo->stVFrame.u64PhyAddr[0], u32Size)))
	{
		printf("[%s %d] HI_MPI_SYS_Mmap err!\n", __func__, __LINE__);
		return pHdl->fCallback(DEVSDK_ERR_MEM, 0, 0, 0, NULL, pHdl->pUser);
	}
	
	printf("[%s %d] Data(%p) size(%d) Resolution(%d %d)\n", __func__, __LINE__, pVirtAddr, u32Size, 
		pstVideo->stVFrame.u32Width, pstVideo->stVFrame.u32Height);

	pHdl->fCallback(DEVSDK_NOERROR, 
					pstVideo->stVFrame.u32Width, 
					pstVideo->stVFrame.u32Height, 
					pstVideo->stVFrame.enPixelFormat, 
					u32Size, 
					pVirtAddr,
					pHdl->pUser);
	
	HI_MPI_SYS_Munmap(pVirtAddr, u32Size);
*/	
	return 0;
}


// 注意不能阻塞，否则会导致数据堆积，缓冲耗尽的问题！\EF\BC?
int gsf_vpss_bgr_proc(GSF_VPSS_YUV_HDL *pHdl)
{
	if (pHdl->fCallback)
	{
		int nStatus = DEVSDK_OK;
		hi_video_frame_info stVideoDest = {0};
		hi_video_frame_info *pstVideoRGBDest = NULL;
		if (pHdl->nWidth != 0 && pHdl->nHeight != 0 &&
			(pHdl->nWidth  != pHdl->stFrame.video_frame.width ||
			 pHdl->nHeight != pHdl->stFrame.video_frame.height ))
		{
			if (vpss_yuv_vgs_proc(&pHdl->stFrame, &stVideoDest, pHdl->nWidth, pHdl->nHeight) != HI_SUCCESS)
			{
				printf("[%s %d] vpss_yuv_vgs_proc err!!\n", __func__, __LINE__);
				return gsf_vpss_bgr_data_cb((GSF_VPSS_BGR_HDL *)pHdl, DEVSDK_ERR_SCALE, NULL);
			}
		}
		else
		{
			memcpy(&stVideoDest, &pHdl->stFrame,sizeof(hi_video_frame_info));
		}
		
		if (NULL == (pstVideoRGBDest = vpss_yuv_YUV420SP_to_BGR_P(pHdl->nVpssChn,&stVideoDest))) // 缩放后的图像
		{
			printf("[%s %d] vpss_yuv_YUV420SP_to_BGR_P err!!\n", __func__, __LINE__);
			return gsf_vpss_bgr_data_cb((GSF_VPSS_BGR_HDL *)pHdl, DEVSDK_ERR_CONVERT, NULL);
		}
		return gsf_vpss_yuv_data_cb(pHdl, nStatus, pstVideoRGBDest,0);
	}
	return 0;
}

void *gsf_vpss_bgr_task(void *arg)
{
	GSF_VPSS_YUV_HDL *pHdl = (GSF_VPSS_YUV_HDL *)arg;
	int nRet = 0;
	printf("vpss grp(%d) chn(%d)\n", pHdl->nVpssGrp, pHdl->nVpssChn);
	if (0 > (pHdl->nDepth = gsf_vpss_set_depth(pHdl->nVpssGrp, pHdl->nVpssChn, HI_VPSS_DEPTH)))
	{
		printf("[%s %d] set frame depth(%d) err\n", __func__, __LINE__, HI_VPSS_DEPTH);
		return NULL;
	}
	while(pHdl->nExit == 0)
	{
		usleep(20000);
		// get frame
		if ((nRet = hi_mpi_vpss_get_chn_frame(pHdl->nVpssGrp, pHdl->nVpssChn, &pHdl->stFrame, 100)) != 0)
		{
			printf("[%s %d] HI_MPI_VPSS_GetChnFrame err(%0#x)\n", __func__, __LINE__, nRet);
			continue;
		}
		gsf_vpss_bgr_proc(pHdl);
		//if (pHdl->nExit)
		//	destroy_jpeg_chn();
		// release frame
		if ((nRet = hi_mpi_vpss_release_chn_frame(pHdl->nVpssGrp, pHdl->nVpssChn, &pHdl->stFrame)) != 0)
		{
			printf("[%s %d] hi_mpi_vpss_release_chn_frame err %0#x\n", __func__, __LINE__, nRet);
		}
	}
	//destroy_jpeg_chn();
	//vpss_yuv_vgs_uninit();
	//vpss_yuv_rgb_uninit();
	// resume depth
	if (0 > gsf_vpss_set_depth(pHdl->nVpssGrp, pHdl->nVpssChn, pHdl->nDepth))
	{
		printf("[%s %d] set frame depth(%d) err\n", __func__, __LINE__, pHdl->nDepth);
	}
	return NULL;
}


int gsf_vpss_bgr_stop(void *hdl)
{
	GSF_VPSS_BGR_HDL *pHdl = (GSF_VPSS_BGR_HDL *)hdl;
	pHdl->nExit = 1;
	pthread_join(pHdl->thid, NULL);

	//release_vpss_chn(pHdl->nVpssGrp, pHdl->nVpssChn); //为什么要关闭vpss通道
	vpss_yuv_vgs_uninit();
	vpss_yuv_rgb_uninit();
	if (pHdl->nBufSize > 0 && pHdl->pBuf != NULL)
	{
		free(pHdl->pBuf);
		pHdl->pBuf = NULL;
		pHdl->nBufSize = 0;
	}
	return 0;
}

// 获取绘画区域
// 坐标需要基于YUV分辨\E7\8E?
int get_draw_rect(stDrawRectInfo *pstDrawRect, int width, int height)
{
	static RECT32_S stRect = {64, 32, 128, 64}; //区域坐标
	static int idx = 0;
	static int req = 0;
	if (++req % 25 == 0)
	{
		idx++;
		stRect.s32LeftX = 64;
		stRect.s32TopY  = 32;
		stRect.s32RightX = 64*(idx+1);
		stRect.s32BottomY = 32*(idx+1);
		if (stRect.s32LeftX >= width || stRect.s32TopY >= height ||
			stRect.s32RightX >= width || stRect.s32BottomY >= height)
		{
			idx = 0;
			stRect.s32LeftX = 64;
			stRect.s32TopY = 32;
			stRect.s32RightX = 128;
			stRect.s32BottomY = 64;
		}
	}
	pstDrawRect->clrIdx[0] = YUV_RED;	//color
	pstDrawRect->clrIdx[1] = YUV_RED;
	pstDrawRect->clrIdx[2] = YUV_RED;
	pstDrawRect->clrIdx[3] = YUV_RED;
	pstDrawRect->lineWidth = 1;			//线宽
	pstDrawRect->pointNum = 4;			//点的数量
	pstDrawRect->stPoint[0].x = stRect.s32LeftX;
	pstDrawRect->stPoint[0].y = stRect.s32TopY;

	pstDrawRect->stPoint[1].x = stRect.s32RightX;
	pstDrawRect->stPoint[1].y = stRect.s32TopY;

	pstDrawRect->stPoint[2].x = stRect.s32RightX;
	pstDrawRect->stPoint[2].y = stRect.s32BottomY;
	
	pstDrawRect->stPoint[3].x = stRect.s32LeftX;
	pstDrawRect->stPoint[3].y = stRect.s32BottomY;
	return 0;
}
//将视频图像像素格式匹配yuv类型
int get_yuv_type(int enPixelFormat)
{
	if (OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
	{
		return TYPE_YUV420SP_NV12;
	}
    else if (OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat)
    {
		return TYPE_YUV420SP_NV21;
    }
	else if (OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat ||
			 OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
	{
		return TYPE_YUV422P;
	}
	else if (OT_PIXEL_FORMAT_YVU_SEMIPLANAR_444 == enPixelFormat ||
			 OT_PIXEL_FORMAT_YUV_SEMIPLANAR_444 == enPixelFormat)
	{
		return TYPE_YUV444P;
	}
	return TYPE_YUV420SP_NV21;
}
int get_format_time(char *title, int len, GSF_U64 u64Pts)
{
	char szFlag[32] = "%Y-%m-%d %H:%M:%S";
	time_t sec = u64Pts/1000000;
	struct tm *ptmTime = localtime(&sec);
	strftime(title, len, szFlag, ptmTime);
	sprintf(szFlag, ".%03lu", (u64Pts%1000000)/1000);
	strcat(title, szFlag);
	return 0;
}
//从视频帧存储yuv数据
int save_yuv_data_from_video_frame(hi_video_frame_info *pstVideo)
{
	unsigned int u32Size = 0;
	unsigned char *pVirtAddr = NULL;
	
	if (OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420 == pstVideo->video_frame.pixel_format|| 
        OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 == pstVideo->video_frame.pixel_format)
	{
		u32Size = (pstVideo->video_frame.stride[0] * pstVideo->video_frame.height * 3) / 2;
	}
	else // 422
	{
		u32Size = (pstVideo->video_frame.stride[0] * pstVideo->video_frame.height * 2);
	}
	if(!g_nIsUxcExist)
	{
		if (NULL == (pVirtAddr = (char *) hi_mpi_sys_mmap(pstVideo->video_frame.phys_addr[0], u32Size))) //memory 存储映射接口
		{
			printf("[%s %d] hi_mpi_sys_mmap err!\n", __func__, __LINE__);
			return -1;
		}
	}
	else
	{
		if (NULL == (pVirtAddr = (char *) hi_mpi_sys_mmap_cached(pstVideo->video_frame.phys_addr[0], u32Size))) //memory 存储映射接口
		{
			printf("[%s %d] hi_mpi_sys_mmap err!\n", __func__, __LINE__);
			return -1;
		}
	}
	
	//printf("[%s %d] Data(%p) size(%d) stride(%d %d %d) Resolution(%d %d)\n", __func__, __LINE__, 
	//	pVirtAddr, u32Size, pstVideo->stVFrame.u32Stride[0], 
	//	pstVideo->stVFrame.u32Stride[1], pstVideo->stVFrame.u32Stride[2], 
	//	pstVideo->stVFrame.u32Width, pstVideo->stVFrame.u32Height);
	char font[256] = {0};
	sprintf(font,"%lld", pstVideo->video_frame.pts/1000);
	
	stYuvBuffInfo stYuvBuff = {0};
	stDrawRectInfo stDrawRect = {0};

	stYuvOsdInfo stYuvOsd = {0};
	// draw rect
	stYuvBuff.pYuvBuff = pVirtAddr;
	stYuvBuff.width = pstVideo->video_frame.stride[0];	//图像数据跨距
	stYuvBuff.height = pstVideo->video_frame.height;
	stYuvBuff.yuvType = get_yuv_type(pstVideo->video_frame.pixel_format);	//将视频图像像素格式匹配yuv类型
	get_draw_rect(&stDrawRect, stYuvBuff.width, stYuvBuff.height);	//获取绘画矩形
	yuv_draw_rect(&stYuvBuff, &stDrawRect);		//yuv绘画矩形
	// -- end

	stYuvOsd.FontColor = YUV_RED;
	stYuvOsd.FontSize  = TYPE_FONT_SIZE_48P;
	stYuvOsd.Font_PosX = stDrawRect.stPoint[0].x;
	stYuvOsd.Font_PosY = stDrawRect.stPoint[0].y;
	memcpy(stYuvOsd.OsdTitle,font,sizeof(font)-1);
	//stYuvOsd.Font_PosX = pstVideo->video_frame.width / 2;
	//stYuvOsd.Font_PosY = pstVideo->video_frame.height - 128;
	//snprintf(stYuvOsd.OsdTitle, sizeof(stYuvOsd.OsdTitle), "%llu", pstVideo->video_frame.pts/1000);
	//get_format_time(stYuvOsd.OsdTitle, sizeof(stYuvOsd.OsdTitle)-1, pstVideo->video_frame.pts);
	//printf("title: %s\n", stYuvOsd.OsdTitle);
	gsf_yuv_osd_refresh(&stYuvOsd, &stYuvBuff);//yuv叠加OSD

	hi_mpi_sys_munmap(pVirtAddr, u32Size);	//存储反映射接\E5\8F?
	
	return 0;
}

int EnableVpssVencChn(int VpssVencChn)
{
	FILE *fd = NULL;
	if(VpssVencChn == 0)
	{
		if((fd = fopen("/etc/vpss_unbind_venc","w+")) == NULL)
		{
			printf("[%s %d] creat vpssVencChn(%d) fail!!!\n",__func__,__LINE__,VpssVencChn);
			return -1;
		}
	}
	if(VpssVencChn == 1)
	{
		if((fd = fopen("/etc/vpss_unbind_venc1","w+")) == NULL)
		{
			printf("[%s %d] creat vpssVencChn(%d) fail!!!\n",__func__,__LINE__,VpssVencChn);
			return -1;
		}
	}
	if(VpssVencChn == 2)
	{
		if((fd = fopen("/etc/vpss_unbind_jpg","w+")) == NULL)
		{
			printf("[%s %d] creat vpssVencChn(%d) fail!!!\n",__func__,__LINE__,VpssVencChn);
			return -1;
		}
	}
	fclose(fd);

	return 0;
}


int yuvDataProcPrepare(int VpssVencChn, int nWidth, int nHeight,int userMsg,int nRotate)
{

	// 需\E8\A6?touch /etc/vpss_unbind_venc 启用YUV数据画矩形框并发送到主码流编码通道
	if (access("/etc/vpss_unbind_venc", F_OK) == 0 || 
		access("/tmp/vpss_unbind_venc", F_OK) == 0)
	{
		g_unbind_mode[0] = 1;
	}
	
	// 需\E8\A6?touch /etc/vpss_unbind_venc1 启用YUV数据画矩形框并发送到次码流编码通道
	if (access("/etc/vpss_unbind_venc1", F_OK) == 0 || 
		access("/tmp/vpss_unbind_venc1", F_OK) == 0)
	{
		g_unbind_mode[1] = 1;
	}
		
	// 需\E8\A6?touch /etc/vpss_unbind_jpg 启用YUV数据画矩形框并发送到jpg通道
	if (access("/etc/vpss_unbind_jpg", F_OK) == 0 || 
		access("/tmp/vpss_unbind_jpg", F_OK) == 0)
	{
		g_unbind_mode[2] = 1;
		
		if(get_jpeg_prepare() != 0)
		{
			printf("[%s %d] jpeg prepare err!!!\n",__func__,__LINE__);
			return DEVSDK_ERR_JPEGPREPAREERR;
		}
	}

	if(userMsg == 3)
	{
		release_vpss_chn(0, VpssVencChn); // release before create
		if (create_vpss_chn(0, VpssVencChn, nWidth, nHeight,nRotate) != HI_SUCCESS)
		{
			printf("[%s %d] create vpss chn err!!!",__func__,__LINE__);
			return DEVSDK_ERR_CREATECHNFAIL;
		}
	}

	return DEVSDK_OK;
}
int VB_Restore(int nSize)
{
	hi_vb_pool_cfg stVbPoolCfg;

	memset(&stVbPoolCfg, 0, sizeof(hi_vb_pool_cfg));
	stVbPoolCfg.blk_size	= nSize;
	stVbPoolCfg.blk_cnt= 1;
	stVbPoolCfg.remap_mode = OT_VB_REMAP_MODE_NONE;
	hi_vb_pool vbpool = HI_VB_INVALID_POOL_ID;
	hi_vb_blk vbblk = HI_VB_INVALID_HANDLE;
	/*create comm vb pool*/

	vbpool = hi_mpi_vb_create_pool(&stVbPoolCfg);
	if(HI_VB_INVALID_HANDLE == vbpool)
	{
		printf("[%s %d]hi_mpi_vb_create_pool err\n",__func__,__LINE__);
		return -1;
	}
	else
		printf("[%s %d]hi_mpi_vb_create_pool(%d)ok\n",__func__,__LINE__,vbpool);
	
	if((vbblk = hi_mpi_vb_get_blk(vbpool, nSize, HI_NULL)) == OT_VB_INVALID_HANDLE)
	{
		printf("[%s %d]hi_mpi_vb_get_blk err\n",__func__,__LINE__);
		return -1;
	}
	else		
		printf("[%s %d]hi_mpi_vb_get_blk ok\n",__func__,__LINE__);
	return 0;
}

int gsf_release_vb_before_start()		  		//You can call this function to release the requested VB pool
{
	printf("[%s %d]Try release vb\n", __func__,__LINE__);
	int nPoolMaxCnt = 32;
	int nBlockMaxCnt = 64;
	int nRet = -1;
	for (int i = 0; i < nPoolMaxCnt; ++i)
	{
		for (int k = 0; k < nBlockMaxCnt; ++k)
		{
			hi_vb_blk block = i << 16 | k; 			//magic block id			
			nRet = hi_mpi_vb_release_blk(block); 	//releases an already acquired cache block
			if (nRet == HI_SUCCESS)
				printf("[%s %d]Release block id: %d\n", __func__,__LINE__,block);
		}

		nRet = hi_mpi_vb_destroy_pool(i); 			//Destroy a video cache pool
		if (nRet == HI_SUCCESS)
			printf("[%s %d]Destroy pool id: %d\n",__func__,__LINE__, i);
	}
	return 0;
}


#endif