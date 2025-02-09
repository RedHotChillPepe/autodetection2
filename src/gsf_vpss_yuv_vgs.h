#ifndef __GSF_VPSS_YUV_VGS_H__
#define __GSF_VPSS_YUV_VGS_H__


// Инициировать заранее нужное кол-во overlay-регионов (например, 32 или 100)
int vpss_yuv_vgs_init_osd(int maxRegions);
int vpss_yuv_vgs_deinit_osd(void);

int vpss_yuv_vgs_proc(hi_video_frame_info *pstInVideo, hi_video_frame_info *pstOutVideo, hi_u32 u32Width, hi_u32 u32Height);

int vpss_yuv_vgs_crop_proc(hi_video_frame_info *pstInVideo, hi_video_frame_info *pstOutVideo, hi_rect *pRect);
int vpss_yuv_vgs_uninit();
int vpss_yuv_vgs_ai_proc(hi_video_frame_info *pVideoFrame,AI_DATA_INFO_S *pAiDataInfo,int nArrayNum);
#endif