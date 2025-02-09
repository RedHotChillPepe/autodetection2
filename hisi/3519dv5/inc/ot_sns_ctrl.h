/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_SNS_CTRL_H
#define OT_SNS_CTRL_H

#include "ot_type.h"
#include "ot_common_3a.h"
#include "ot_debug.h"
#include "ot_common_ae.h"
#include "ot_common_awb.h"
#include "ot_common_sns.h"
#include "ot_common_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define ISP_SNS_SAVE_INFO_MAX 2
typedef struct {
    td_bool      init;                   /* TD_TRUE: Sensor init */
    td_bool      sync_init;              /* TD_TRUE: Sync Reg init */
    td_u8        img_mode;
    td_u8        hdr;                 /* TD_TRUE: HDR enable */
    ot_wdr_mode  wdr_mode;

    ot_isp_sns_regs_info regs_info[ISP_SNS_SAVE_INFO_MAX]; /* [0]: Sensor reg info of cur-frame;
                                                              [1]: Sensor reg info of pre-frame ; */

    td_u32      fl[ISP_SNS_SAVE_INFO_MAX];                /* [0]: FullLines of cur-frame;
                                                             [1]: Pre FullLines of pre-frame */
    td_u32      fl_std;                    /* FullLines std */
    td_u32      wdr_int_time[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32      sns_wb_gain[OT_ISP_BAYER_CHN_NUM];
} ot_isp_sns_state;

typedef enum {
    ISP_SNS_NORMAL      = 0,
    ISP_SNS_MIRROR      = 1,
    ISP_SNS_FLIP        = 2,
    ISP_SNS_MIRROR_FLIP = 3,
    ISP_SNS_BUTT
} ot_isp_sns_mirrorflip_type;

typedef struct {
    char bus_addr;
} ot_isp_sns_bus_ex;

typedef struct {
    td_bool blc_clamp_en; /* TD_TRUE: sensor black level correction enable */
} ot_isp_sns_blc_clamp;

typedef int (*pGetSnsSlaveAttr)(ot_slave_dev slave_dev, const ot_isp_slave_sns_sync *sns_sync);
typedef int (*pSetSnsSlaveAttr)(ot_slave_dev slave_dev, ot_isp_slave_sns_sync *sns_sync);

typedef int (*pRegSnsRegCb)(ot_vi_pipe ViPipe, ot_isp_sns_attr_info *pstSnsAttrInfo, const ot_isp_sns_register *pstRegister);
typedef int (*pRegSnsUnRegCb)(ot_vi_pipe ViPipe, ot_sensor_id SensorId);

typedef int (*pAeSnsRegCb)(ot_vi_pipe ViPipe,  ot_isp_3a_alg_lib *pstAeLib, const ot_isp_sns_attr_info *pstSnsAttrInfo,
                                    ot_isp_ae_sensor_register *pstRegister);
typedef int (*pAeSnsUnRegCb)(ot_vi_pipe ViPipe, ot_isp_3a_alg_lib *pstAeLib, 
								 ot_sensor_id SensorId);

typedef int (*pAwbSnsRegCb)(ot_vi_pipe ViPipe, ot_isp_3a_alg_lib *pstAwbLib, ot_isp_sns_attr_info *pstSnsAttrInfo,
                                     ot_isp_awb_sensor_register *pstRegister);
typedef int (*pAwbSnsUnRegCb)(ot_vi_pipe ViPipe, ot_isp_3a_alg_lib *pstAwbLib, ot_sensor_id SensorId);


typedef int (*pRegThermoSnsRegCb)(ot_vi_pipe vi_pipe, ot_isp_3a_alg_lib *thermo_lib,
    const ot_isp_sns_attr_info *sns_attr_info, ot_isp_ae_sensor_register *pregister);

typedef int (*pRegThermoSnsUnRegCb)(ot_vi_pipe vi_pipe, const ot_isp_3a_alg_lib *thermo_lib,
		ot_sensor_id sns_id);

typedef struct {
    td_s32  (*pfn_register_callback)(ot_vi_pipe vi_pipe, 
		pRegSnsRegCb snsRegCb, pAeSnsRegCb aeRegCb, pAwbSnsRegCb awbRegCb,
		pGetSnsSlaveAttr getSnsSlaveAttr, pSetSnsSlaveAttr setSnsSlaveAttr, 
		pRegThermoSnsRegCb thermoSnsRegCb,
		ot_isp_3a_alg_lib *ae_lib, ot_isp_3a_alg_lib *awb_lib);
    td_s32  (*pfn_un_register_callback)(ot_vi_pipe vi_pipe, pRegSnsUnRegCb snsUnRegCb, 
		pAeSnsUnRegCb aeUnRegCb,  pAwbSnsUnRegCb awbUnRegCb,
		pRegThermoSnsUnRegCb thermoSnsUnRegCb,
		ot_isp_3a_alg_lib *ae_lib, ot_isp_3a_alg_lib *awb_lib);
    td_s32  (*pfn_set_bus_info)(ot_vi_pipe vi_pipe, ot_isp_sns_commbus sns_bus_info);
    td_s32  (*pfn_set_bus_ex_info)(ot_vi_pipe vi_pipe, ot_isp_sns_bus_ex *serdes_info);
    td_void (*pfn_standby)(ot_vi_pipe vi_pipe);
    td_void (*pfn_restart)(ot_vi_pipe vi_pipe);
    td_void (*pfn_mirror_flip)(ot_vi_pipe vi_pipe, ot_isp_sns_mirrorflip_type sns_mirror_flip);
    td_void (*pfn_set_blc_clamp)(ot_vi_pipe vi_pipe, ot_isp_sns_blc_clamp sns_blc_clamp);
    td_s32  (*pfn_write_reg)(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data);
    td_s32  (*pfn_read_reg)(ot_vi_pipe vi_pipe, td_u32 addr);
    td_s32  (*pfn_set_init)(ot_vi_pipe vi_pipe, ot_isp_init_attr *init_attr);
} ot_isp_sns_obj;

ot_isp_sns_obj *cmos_get_sensor_obj();

#define sns_check_pointer_return(ptr) \
    do { \
        if ((ptr) == TD_NULL) { \
            isp_err_trace("Null Pointer!\n"); \
            return OT_ERR_ISP_NULL_PTR; \
        } \
    } while (0)

#define sns_check_pointer_void_return(ptr) \
    do { \
        if ((ptr) == TD_NULL) { \
            isp_err_trace("Null Pointer!\n"); \
            return; \
        } \
    } while (0)

#define sns_free(ptr) \
    do { \
        if ((ptr) != TD_NULL) { \
            free(ptr); \
            (ptr) = TD_NULL; \
        } \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_SNS_CTRL_H */
