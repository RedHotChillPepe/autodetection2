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
	PTZ_CMD_UP				=	0,	//��
	PTZ_CMD_SENSE			=	1,	// 

	PTZ_CMD_DOWN			=	2,	//��
	PTZ_CMD_LEFT			=	3,	//��
	PTZ_CMD_RIGHT			=	4,	//��
	PTZ_CMD_LEFT_UP			=	5,	//����
	PTZ_CMD_LEFT_DOWN		=	6,	//����
	PTZ_CMD_RIGHT_UP		=	7,	//����
	PTZ_CMD_RIGHT_DOWN		=	8,	//����
	PTZ_CMD_AUTOSCAN		=	9,	//�Զ�ɨ��
	PTZ_CMD_MANUALSCAN		=	10,	//�ֶ�ɨ��
	PTZ_CMD_FOCUS_NEAR		=	11,	//�۽�+
	PTZ_CMD_FOCUS_FAR		=	12, //�۽�-
	PTZ_CMD_IRIS_CLOSE		=	13, //��Ȧ-
	PTZ_CMD_IRIS_OPEN		=	14,	//��Ȧ+
	PTZ_CMD_ZOOM_WIDE		=	15,	//�䱶+
	PTZ_CMD_ZOOM_TELE		=	16, //�䱶-
	PTZ_CMD_STOP			=	17,	//ֹͣ

	PTZ_CMD_SET_PRESET		=	18, //Ԥ��λ����
	PTZ_CMD_CLR_PRESET		=	19,	//Ԥ��λ���
	PTZ_CMD_GOTO_PRESET		=	20, //Ԥ��λ����
	PTZ_CMD_FLIP			=	21, //��ת
	PTZ_CMD_GOTO_ZEROPAN	=	22, //��λ����
	PTZ_CMD_SET_AUX			=	23, //���ø�������
	PTZ_CMD_CLR_AUX			=	24, //�����������
	PTZ_CMD_REMOTE_RESET	=	25, //Զ�ָ̻�
	PTZ_CMD_ZONE_START		=	26, //���û���ɨ�迪ʼ
	PTZ_CMD_ZONE_END		=	27, //���û���ɨ�����
	PTZ_CMD_WR_CHAR			=	28, //д�ַ�
	PTZ_CMD_CLR_SCR			=	29, //����ַ�
	PTZ_CMD_ALM_ACK			=	30, //����ȷ��
	PTZ_CMD_ZONE_SCAN_ON	=	31, //��������ɨ��
	PTZ_CMD_ZONE_SCAN_OFF	=	32, //ֹͣ����ɨ��
	PTZ_CMD_PTN_START		=	33, //��ʼ�켣¼��
	PTZ_CMD_PTN_STOP		=	34, //ֹͣ�켣¼��
	PTZ_CMD_RUN_PTN			=	35, //���ù켣
	PTZ_CMD_ZOOM_SPEED		=	36, //�䱶�ٶ�
	PTZ_CMD_FOCUS_SPEED		=	37, //�۽��ٶ�
	PTZ_CMD_RESET_CAMERA	=	38, //�����λ
	PTZ_CMD_AUTO_FOCUS		=	39, //�Զ��۽�
	PTZ_CMD_AUTO_IRIS		=	40,	//�Զ���Ȧ 
	PTZ_CMD_AUTO_AGC		=	41, //�Զ�����
	PTZ_CMD_BACKLIGHT_COMP	= 	42,	//
	PTZ_CMD_AUTO_WB		    =	43,	//�Զ���ƽ��
	PTZ_CMD_DEV_PHASE_DELAY	=	44,	//
	PTZ_CMD_SET_SHTTER_SPEED=	45,	//���ÿ����ٶ�
	PTZ_CMD_ADJ_LL_PHASE_DELAY=	46,
	PTZ_CMD_ADJ_WB_RB		=	47,	//�����Զ���ƽ�����
	PTZ_CMD_ADJ_WB_MG		=	48,	//�����Զ���ƽ��
	PTZ_CMD_ADJ_GAIN		=	49, //�����Զ�����
	PTZ_CMD_AUTO_IRIS_LV	=	50,	//�����Զ���Ȧlevel
	PTZ_CMD_AUTO_IRIS_PEAK	=	51,	//�����Զ���Ȧpeak
	PTZ_CMD_QUERY			=	52, //��ѯ
    PTZ_CMD_TURN_ON         =	53,	//�����ƹ�
    PTZ_CMD_TURN_OFF        =	54, //�رյƹ�
    PTZ_CMD_WIPERS_ON       =	55, //������ˢ
    PTZ_CMD_WIPERS_OFF      =	56,	//�ر���ˢ	
    PTZ_CMD_START_CRZ       =	57,	//��ʼѲ��
	PTZ_CMD_STOP_CRZ        =	58,	//ֹͣѲ��
	PTZ_CMD_3D              =	59,	//3D����
	PTZ_CMD_LEFT_LIMIT      =	60,	//��������λ
	PTZ_CMD_RIGHT_LIMIT     =	61,	//��������λ
	PTZ_CMD_AB_SCAN         =	62,	//���ɨ��
	PTZ_CMD_SET_PAN_POS     =   63,
	PTZ_CMD_SET_TILT_POS    =   64,
	PTZ_CMD_SET_ZOOM_POS    =   65,
	PTZ_CMD_QUERY_PAN_POS   =   66,
	PTZ_CMD_QUERY_TILT_POS  =   67,
	PTZ_CMD_QUERY_ZOOM_POS  =   68,
	PTZ_CMD_CAM_ADJUST      =   69,
	PTZ_CMD_ZOOM_TRIGGER    =   70, // KeepFocus(KF)����������
	PTZ_CMD_MANUAL_FOCUS    =   71,
	PTZ_CMD_INVALID
}GSF_PTZ_CMD_E;

typedef struct _GSF_PTZ_CMD_S_
{
	int cmd;		// ��̨���� GSF_PTZ_CMD_E
	int addr;		// ��̨��ַ 
	int data1;		// �ƶ���̨ʱ����ˮƽ�ٶ�(1-100),��������ϸ�˽���ӦЭ��
	int data2; 		// �ƶ���̨ʱ������ֱ�ٶ�(1-100),Ԥ��λʱ����Ԥ��λ��(1-255)
					// ��������ϸ�˽�Э��
	int funNo;	
}GSF_PTZ_CMD_S;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GSF_PTZ_H__ */
