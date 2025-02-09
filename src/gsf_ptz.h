/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_param.h

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/12/20
  Description   :
  History       :
  1.Date        : 2021/12/20
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __GSF_PTZ_H__
#define __GSF_PTZ_H__

#include "devsdk.h"
#include "gsf_type.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#pragma pack(push, 4)

typedef enum _GSF_PTZ_CMD_E
{
	PTZ_CMD_UP				=	0,	//上
	PTZ_CMD_SENSE			=	1,	// 

	PTZ_CMD_DOWN			=	2,	//下
	PTZ_CMD_LEFT			=	3,	//左
	PTZ_CMD_RIGHT			=	4,	//右
	PTZ_CMD_LEFT_UP			=	5,	//左上
	PTZ_CMD_LEFT_DOWN		=	6,	//左下
	PTZ_CMD_RIGHT_UP		=	7,	//右上
	PTZ_CMD_RIGHT_DOWN		=	8,	//右下
	PTZ_CMD_AUTOSCAN		=	9,	//自动扫描
	PTZ_CMD_MANUALSCAN		=	10,	//手动扫描
	PTZ_CMD_FOCUS_NEAR		=	11,	//聚焦+
	PTZ_CMD_FOCUS_FAR		=	12, //聚焦-
	PTZ_CMD_IRIS_CLOSE		=	13, //光圈-
	PTZ_CMD_IRIS_OPEN		=	14,	//光圈+
	PTZ_CMD_ZOOM_WIDE		=	15,	//变倍+
	PTZ_CMD_ZOOM_TELE		=	16, //变倍-
	PTZ_CMD_STOP			=	17,	//停止

	PTZ_CMD_SET_PRESET		=	18, //预置位设置
	PTZ_CMD_CLR_PRESET		=	19,	//预置位清楚
	PTZ_CMD_GOTO_PRESET		=	20, //预置位调用
	PTZ_CMD_FLIP			=	21, //翻转
	PTZ_CMD_GOTO_ZEROPAN	=	22, //零位调用
	PTZ_CMD_SET_AUX			=	23, //设置辅助开关
	PTZ_CMD_CLR_AUX			=	24, //清除辅助开关
	PTZ_CMD_REMOTE_RESET	=	25, //远程恢复
	PTZ_CMD_ZONE_START		=	26, //设置花样扫描开始
	PTZ_CMD_ZONE_END		=	27, //设置花样扫描结束
	PTZ_CMD_WR_CHAR			=	28, //写字符
	PTZ_CMD_CLR_SCR			=	29, //清楚字符
	PTZ_CMD_ALM_ACK			=	30, //报警确认
	PTZ_CMD_ZONE_SCAN_ON	=	31, //开启花样扫描
	PTZ_CMD_ZONE_SCAN_OFF	=	32, //停止花样扫描
	PTZ_CMD_PTN_START		=	33, //开始轨迹录制
	PTZ_CMD_PTN_STOP		=	34, //停止轨迹录制
	PTZ_CMD_RUN_PTN			=	35, //调用轨迹
	PTZ_CMD_ZOOM_SPEED		=	36, //变倍速度
	PTZ_CMD_FOCUS_SPEED		=	37, //聚焦速度
	PTZ_CMD_RESET_CAMERA	=	38, //相机复位
	PTZ_CMD_AUTO_FOCUS		=	39, //自动聚焦
	PTZ_CMD_AUTO_IRIS		=	40,	//自动光圈 
	PTZ_CMD_AUTO_AGC		=	41, //自动增益
	PTZ_CMD_BACKLIGHT_COMP	= 	42,	//
	PTZ_CMD_AUTO_WB		    =	43,	//自动白平衡
	PTZ_CMD_DEV_PHASE_DELAY	=	44,	//
	PTZ_CMD_SET_SHTTER_SPEED=	45,	//设置快门速度
	PTZ_CMD_ADJ_LL_PHASE_DELAY=	46,
	PTZ_CMD_ADJ_WB_RB		=	47,	//调整自动白平衡红蓝
	PTZ_CMD_ADJ_WB_MG		=	48,	//调整自动白平衡
	PTZ_CMD_ADJ_GAIN		=	49, //调整自动增益
	PTZ_CMD_AUTO_IRIS_LV	=	50,	//调整自动光圈level
	PTZ_CMD_AUTO_IRIS_PEAK	=	51,	//调整自动光圈peak
	PTZ_CMD_QUERY			=	52, //查询
    PTZ_CMD_TURN_ON         =	53,	//开启灯光
    PTZ_CMD_TURN_OFF        =	54, //关闭灯光
    PTZ_CMD_WIPERS_ON       =	55, //开启雨刷
    PTZ_CMD_WIPERS_OFF      =	56,	//关闭雨刷	
    PTZ_CMD_START_CRZ       =	57,	//开始巡航
	PTZ_CMD_STOP_CRZ        =	58,	//停止巡航
	PTZ_CMD_3D              =	59,	//3D控球
	PTZ_CMD_LEFT_LIMIT      =	60,	//设置左限位
	PTZ_CMD_RIGHT_LIMIT     =	61,	//设置左限位
	PTZ_CMD_AB_SCAN         =	62,	//随机扫描
	PTZ_CMD_SET_PAN_POS     =   63,
	PTZ_CMD_SET_TILT_POS    =   64,
	PTZ_CMD_SET_ZOOM_POS    =   65,
	PTZ_CMD_QUERY_PAN_POS   =   66,
	PTZ_CMD_QUERY_TILT_POS  =   67,
	PTZ_CMD_QUERY_ZOOM_POS  =   68,
	PTZ_CMD_CAM_ADJUST      =   69,
	PTZ_CMD_ZOOM_TRIGGER    =   70, // KeepFocus(KF)，用于锁焦
	PTZ_CMD_MANUAL_FOCUS    =   71,
	PTZ_CMD_INVALID
}GSF_PTZ_CMD_E;

typedef struct _GSF_PTZ_CMD_S_
{
	int cmd;		// 云台命令 GSF_PTZ_CMD_E
	int addr;		// 云台地址 
	int data1;		// 移动云台时用作水平速度(1-100),其它请详细了解相应协议
	int data2; 		// 移动云台时用作垂直速度(1-100),预置位时用作预置位号(1-255)
					// 其它请详细了解协议
	int funNo;	
}GSF_PTZ_CMD_S;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GSF_PTZ_H__ */
