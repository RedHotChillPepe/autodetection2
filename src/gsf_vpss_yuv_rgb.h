#ifndef __GSF_VPSS_YUV_RGB_H__
#define __GSF_VPSS_YUV_RGB_H__


hi_video_frame_info *vpss_yuv_YUV420SP_to_BGR_P(int nVpssChn,hi_video_frame_info *pstSrcFrm);
hi_video_frame_info *vpss_bgr_to_yuv_YUV420SP_P(int nVpssChn,hi_video_frame_info *pstSrcFrm);
int vpss_copy_data(int nVpssChn, hi_video_frame_info *pstSrcFrm,hi_video_frame_info *pstDstFrm);
int vpss_yuv_rgb_uninit();

#endif