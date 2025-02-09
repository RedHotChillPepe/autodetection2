/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_param.h

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/08/19
  Description   :
  History       :
  1.Date        : 2021/08/19
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __GSF_PARAM_H__
#define __GSF_PARAM_H__

#include "devsdk.h"
#include "gsf_type.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#pragma pack(push, 4)

#define GSF_VMD_REG_NUM			    	4	//��Ƶ���������
#define GSF_MAX_SEG_NUM			    	4	//���ʱ���
#define GSF_MAX_SUPPORT_CPU				4   //���֧�� CPU ����
#define GSF_MAC_ADDR_LEN				6	//mac��ַ����
#define GSF_USER_NAME_LEN				32	//�û�������
#define GSF_PASS_WORD_LEN		    	32	//���볤��
#define GSF_MAX_SUPPORT_NETCARD        	2	//֧����������
#define GSF_MAX_USR_EXT_NUM				16	// ��չ�û�������
#define GSF_NAME_LEN					32	//���Ƴ���
#define GSF_VOD_REG_NUM            		4	//�ڵ��澯������
#define GSF_VMASK_REG_NUM		    	4	//��Ƶ�ڵ�����
#define GSF_OSD_NAME_LEN				128 // OSD ���Ƴ���
#define GSF_OSD_EXT_NAME_LEN			124 // OSD ���Ƴ���
#define GSF_IP_ADDR_LEN			    	64  //ip��ַ��������
#define GSF_PATH_LEN					64
#define GSF_MAX_EMAIL_LEN            	64
#define GSF_MAX_SUPPORT_CHN				4	//���֧��ͨ����
#define GSF_MAX_SUPPORT_DI		    	8	//���֧�ֱ����������
#define GSF_MAX_SUPPORT_GB28181_SERVER   4
#define GSF_ROI_REG_NUM            		4	//����Ȥ����������


typedef enum 
{
    GSF_GET_PARAM_MSG    = 0,
    GSF_SET_PARAM_MSG,

    GSF_GET_SYS_INFO,
    GSF_SET_SYS_INFO,

	GSF_SYS_STATE_MSG,
    GSF_SYS_CTRL_MSG,

	GSF_GET_PROTO_PARA,		// �ݲ�֧��
	GSF_SET_PROTO_PARA,		// �ݲ�֧��
	
    GSF_MAX_CMD_MSG
}DEVSDK_CMD_TYPE_E;

typedef enum
{
    GSF_TIME_INFO = 0, 		// 0 ʱ����Ϣ
    GSF_DISK_INFO,   		// 1 ������Ϣ
    GSF_WIFI_AP_INFO,		// 2 WIFI AP Info
    GSF_NETWORK_STATUS, 	// 3 network status   
    GSF_DEV_ABILITY,    	// 4 �豸������(2012-7-23)
    GSF_DEV_RUN_TIME,   	// 5�豸����ʱ��(2012-09-19)
	GSF_ALARM_STATUS, 		// 6�豸��ǰ�������״̬
	GSF_CHN_STATUS,			// 7�豸ͨ��״̬ ������״̬�ȣ�
	GSF_LOG_INFO,           // 8 ��־��Ϣ
	GSF_DOME_FIRMWARE,		// 9 ����̼��汾(INT), len=sizeof(int)
	GSF_SOFTVER_DATETIME,	// 10 �豸����汾����
	GSF_WIFI_LINKMODE,		// 11 WiFi����ģʽ
	GSF_PTZ_POS,			// 12
	GSF_CAM_POS,            // 13 
	GSF_RECORD_INFO,		// 14 ¼���¼
	GSF_SPORT_STATE,		// 15 GSF_SPORT_STATE_S;
	GSF_THIRD_PARTY_PF,		// 16 ������ƽ̨����
	GSF_MIC_STATUS,         // 17 ��˷羲��״̬
	GSF_TEMP_OSD_PARAM,     // 18 ��ʱϵͳOSD
	GSF_RECORD_INFO_V2,		// 19 ¼���¼
	GSF_PTZ_R_POS,		    // 20 ����ƶ�
	GSF_DOME_FIRMWARE_STR,  // 21 ����̼��汾(CHAR *), len=GSF_DOME_FIRMWARE_LEN
	GSF_EXPOSURE_SYN_INFO,	// 22 ͬ���ع���ز���
	GSF_NET_IPV6_RA,        // 23 IPv6·�ɹ���
	GSF_HISDK_VER_STR,      // 24 hisilion mpp version(CHAR *), len=GSF_HISDK_VERSION_LEN	
	GSF_PICTURE_INFO,		// 25 ͼƬ��¼
	GSF_DYN_PARAM,			// 26 ��̬����(AICFG)
    GSF_MAX_SYS_CMD,
}DEVSDK_SYS_INFO_CMD_E;

typedef enum 
{   	
    GSF_PTZ_CMD,     	 	// 0  PTZ��̨����
    GSF_DEV_CTRL, 		 	// 1  �豸����
	GSF_ALARM_OUT_CMD, 	 	// 2  ����������ƣ�״̬��ȡ����
	GSF_COMM_CMD,		 	// 3  д����
	GSF_MANUAL_SNAP_CMD, 	// 4  ���豸�ֶ�ץͼ,�����浽ָ��·��
	GSF_UPGRADE_CMD,	 	// 5  �豸����֪ͨ������״̬��ȡ
	GSF_FORMAT_CMD,         // 6  
	GSF_UPGRADE_PRE_CMD,	// 7  �豸����ǰ����֪ͨ
	GSF_PIX_CAL,            // 8
	GSF_SD_INSTALL,			// 9  ����SD��
	GSF_SD_UNINSTALL,		// 10 ж��SD��
	GSF_TWIN_ONE_ACTION,    // 11
	GSF_MANUAL_RECORD_CMD, 	// 12 �ֶ�����¼���ֶ�ֹͣ¼��
	GSF_VIDEO_ISO_CMD,      // 13
	GSF_DELETE_FILE_CMD,    // 14 ɾ���ļ�(¼��ͼƬ)
	GSF_ASS_LIFT_CTRL_CMD,  // 15 
	GSF_DB_LIFT_CTRL_CMD,   // 16 DB LIFT ��������
	GSF_DB_LIFT_STATE_CMD,  // 17 DB LIFT ״̬
    GSF_CAM_EFFECT_CTRL_CMD,// 18 PUSHLOCK�����Զ���ƽ��
    GSF_CAM_LENS_CTRL_CMD,  // 19 calibration��������
	GSF_CAM_DAYNIGHT_CTRL_CMD,// 20 
	GSF_CHECK_DISK_CMD,      // 21 
	GSF_IO_STATE_CMD,		//22 IO Status
	GSF_PARAM_OPERATE,
	GSF_BMP_UPLOAD,
	GSF_BMP_FILE_DELETE,
	GSF_TEMP_STATE_CMD,     // 26 �¶�
	GSF_PLAY_AUDIO_CMD,     // 27 ��������
	GSF_DB_LIFT_CTRL_SPEC_CMD, // 28 
	GSF_EXP_INFO_CMD,       // 29 
	GSF_BMP_UPLOAD_STATE_CMD, // 30 osd bmpͼƬ�ϴ�״̬
    GSF_MAX_MACHINE_CMD
}DEVSDK_SYS_CTRL_CMD_E;

typedef enum 
{
	GSF_VER_PARAM    	= 1,	// 1 �豸�汾��Ϣ(ֻ��) 			GSF_DEV_CFG_S
    GSF_SYS_PARAM		= 2,	// 2 ϵͳ������Ϣ					GSF_DEV_SYS_CFG_S
    GSF_NET_PARAM		= 3,    // 3 �豸��������					GSF_NET_CFG_S
	GSF_OSD_PARAM		= 5,	// 5 �豸��Ƶosd����				GSF_OSD_CFG_S
	GSF_VMASK_PARAM		= 6,	// 6 �豸��Ƶ�ڵ�����				GSF_VMASK_CFG_S
	GSF_VMD_PARAM		= 7,   	// 7 �豸��Ƶ�ƶ�������			GSF_MD_CFG_S
    GSF_OD_PARAM		= 8,   	// 8 ��Ƶ�ڵ�������				GSF_OD_CFG_S
	GSF_ALARMIN_PARAM	= 9,	// 9 �����������					GSF_DI_CFG_S
	GSF_VMAIN_PARAM		= 10,	// 10 �豸��Ƶ����������			GSF_VENC_CFG_S
	GSF_VSUB_PARAM		= 11,  	// 11 �豸��Ƶ����������			GSF_VENC_CFG_S
    GSF_VTHREE_PARAM	= 12,  	// 12 �豸������������ (���֧��)	GSF_VENC_CFG_S
	GSF_VCAP_PARAM		= 13,  	// 13 �豸��Ƶץ�Ĳ���				GSF_CAPTURE_CFG_S
	GSF_IMA_PARAM		= 14,  	// 14 �豸IMAͼ�����				GSF_IMA_CFG_S
	GSF_3A_PARAM		= 15,  	// 15 �豸3Aͼ�����				GSF_3A_CFG_S
	GSF_AUDIO_PARAM		= 16,	// 16 ��Ƶ����						GSF_AUDIO_CFG_S
	GSF_COMM_PARAM		= 17,  	// 17 ������Ϣ						GSF_COMM_CFG_S
	GSF_FTP_PARAM		= 18,  	// 18 ftp��������					GSF_FTP_CFG_S
	GSF_EMAIL_PARAM		= 19,	// 19 email��������					GSF_SMTP_CFG_S
	GSF_DDNS_PARAM		= 20,  	// 20 ddns��������					GSF_DDNS_CFG_S
	GSF_UPNP_PARAM		= 21,  	// 21 upnp��������					GSF_UPNP_CFG_S
	GSF_NTP_PARAM		= 22,  	// 22 ntp��������					GSF_NTP_CFG_S
	GSF_REC_PARAM		= 24,	// 24 ¼���������					GSF_REC_STRC_S
	GSF_ROI_PARAM		= 28,   // 28 ����Ȥ�������				GSF_ROI_CFG_S
	GSF_IRCUT_PARAM		= 33,	// 33 Ircut �л�ģʽ����			GSF_DAYNIGHT_CFG_S
	GSF_TIMER_CAP_PARAM	= 34,	// 34 ��ʱץ�Ĳ���					GSF_TIMER_CAP_S
	GSF_TIME_PARAM		= 44,	// 44 ϵͳʱ�����					GSF_TIME_S
	GSF_ALARMOUT_PARAM	= 51,	// 51 �������						GSF_DO_CFG_S
	GSF_USER_EXT_PARAM	= 54,	// 54 ��չ�û���Ϣ					GSF_USER_EXT_CFG_S
	GSF_ONVIF_PARAM		= 59,	// 59 ONVIF����						GSF_ONVIF_CFG_S
	GSF_GB28181_PARAM	= 64,	// 64 GB28181����					GSF_GB28181_CFG_S
	GSF_AUTOREBOOT_PARAM = 65,	// 65 �Զ�ά��						GSF_AUTO_REBOOT_CFG_S
	GSF_PARAM_MAX_CMD,			// ���������� 
}DEVSDK_CMD_PARAM_E;

typedef enum
{
	GSF_PROTOCOL_CUSTOM		= 0,    // ˽��Э��
	GSF_PROTOCOL_ONVIF	 	= 1,	// ONVIFЭ��
	GSF_PROTOCOL_G28181  	= 2,	// GB28181Э��
	GSF_PROTOCOL_ZSYH		= 3,  	// ��ʢ�滪ƽ̨Э��
	GSF_PROTOCOL_HXHT		= 4,  	// ���Ż�ͨƽ̨Э��
	GSF_PROTOCOL_HW			= 5, 	// ��Ϊƽ̨Э��	
	GSF_PROTOCOL_IST_NVR	= 6,
	GSF_PROTOCOL_IST_PF		= 7,
	GSF_PROTOCOL_HH  		= 8,
	GSF_PROTOCOL_GOOLINK    = 9,  	// ���� p2p
	GSF_PROTOCOL_HP			= 10,	// ��ƽ
	GSF_PROTOCOL_VV			= 11,	// ����
	GSF_PROTOCOL_ULK		= 12,	// �����
	GSF_PROTOCOL_DANA		= 13,	// ����
	GSF_PROTOCOL_QQ			= 14,	// QQ
	GSF_PROTOCOL_TUTK		= 15,	// TUTK
	GSF_PROTOCOL_HIK		= 16,	// ����
	GSF_PROTOCOL_JXJ		= 17,   // 0x20000 131072 JXJ
	GSF_PROTOCOL_MOBILE_KD  = 18,	// 0x40000 262144 �ֻ�����
	GSF_PROTOCOL_SICUNET    = 19,	// sicunet
	GSF_PROTOCOL_SIP_B      = 20,   // SIP BЭ��
	GSF_PROTOCOL_ZTE        = 21,   // ����
	GSF_PROTOCOL_CLOUD      = 22,   // reserved
	GSF_PROTOCOL_MTT		= 23,	// 
	GSF_PROTOCOL_AGBOX		= 24,   // agbox
	GSF_PROTOCOL_GI		    = 25,   // gi
	GSF_PROTOCOL_RTMP_PUB   = 26,   // RTMP PUBLISH
	GSF_PROTOCOL_MEDIA_SEND = 27,	// ������
	GSF_PROTOCOL_WUHAN_UNIVERSITY= 28, // wu han university
	GSF_PROTOCOL_AIMI       = 29,   // aimi
	GSF_PROTOCOL_TUYA       = 30,   // Ϳѻ P2P
	GSF_PROTOCOL_BUTT
}DEVSDK_PROTOCOL_TYPE_E;


//eth ����
typedef struct _GSF_ETHERNET_CFG_S_
{
    GSF_U32     u32Size;
    GSF_U32		u32IpAddr;						//ip��ַ
    GSF_U32		u32SubMask;						//���������ַ
    GSF_U32		u32GateWay;						//���ص�ַ
    GSF_U8		u8MacAddr[GSF_MAC_ADDR_LEN];	//mac��ַ
    GSF_U8		u8DhcpOn;						//dhcp����		1--���� 0--������
    GSF_U8		u8DhcpStatus;					//dhcp״̬(ֻ��) 0-û���� 1--���ڻ�ȡ 2--��ȡ�ɹ� 3--��ȡʧ��
    GSF_U8		u8EtherStatu;					//����״̬(ֻ��) 0-���ڵ� 1--��������(ֻ�ܼ����������)
    GSF_U8      u8UseDev;                       //�Ƿ�ʹ�ø����� 0-��ʹ�� 1-ʹ��
    GSF_U8		u8Res[10];
}GSF_ETHERNET_CFG_S, *LPGSF_ETHERNET_CFG_S;

//pppoe������Ϣ
typedef struct _GSF_PPPOE_CFG_S_
{
	GSF_U8		u8PppoeOn;							//pppoe����
	GSF_U8		u8PppoeStatus;						//pppoe ����״̬0 û�в���1 ������2���ųɹ�
	GSF_U8      u8UseEth;                           //0ʹ���������� 1ʹ����������
	GSF_U8		u8Res[5];
	GSF_CHAR	szPppoeUsr[GSF_USER_NAME_LEN];		//pppoe�û���
	GSF_CHAR	szPppoePsw[GSF_PASS_WORD_LEN];		//pppoe����
	GSF_U32		u32PppoeIp;							//pppoe��ȡ��ip
	GSF_U32		u32PppoeTimes;						//pppoe����ʱ��
}GSF_PPPOE_CFG_S, *LPGSF_PPPOE_CFG_S;


//��������
typedef struct _GSF_NET_CFG_S_
{
  GSF_U32				u32Size;
  GSF_ETHERNET_CFG_S    struEtherCfg[GSF_MAX_SUPPORT_NETCARD];	//������Ϣ	
  GSF_U32				u32AlarmHostIp;				//��������ip(Ԥ��)
  GSF_U16				u16AlarmHostPort;			//���������˿�(Ԥ��)
  GSF_U8				u8DnsDhcp;					//dns�Ƿ��Զ���ȡ
  GSF_U8				u8Rtsp2Auth;				//�Ƿ�ʹ��rtsp user  Authentication 
  GSF_U32				u32DnsIp1;					//dns1
  GSF_U32				u32DnsIp2;					//dns2
  GSF_U16				u16HttpPort;				//http����˿�
  GSF_U16				u16CmdPort;					//����˿� 
  GSF_U16               u16RtspPort;                //���ݶ˿�   
  GSF_U16               u16RtmpPort;                //RTMP�˿�    
  GSF_U32				u32MultiCastIp;				//�ಥ��ַ
  GSF_U16				u16MultiCastPort;			//�ಥ�˿�
  GSF_U16				u16IstPort;					//ist���ö˿�, 20210520��ʼ���� HikvisionЭ��˿�
  GSF_U16				u16HHPort;					//hh �����˿�
  GSF_U16				u16Rtsp2Port;				//rtsp server 2 port
  GSF_PPPOE_CFG_S		struPppoeCfg;				//pppoe����

  GSF_U16 				u16JxjCmdPort;				// jxj ����˿�
  GSF_U16 				u16JxjDataPort;				// jxj ���ݶ˿�
  GSF_U16 				u16JxjTalkPort;				// jxj �Խ��˿�
}GSF_NET_CFG_S,*LPGSF_NET_CFG_S;


typedef struct _GSF_USER_INFO_S_
{
	GSF_CHAR	szUsrName[GSF_USER_NAME_LEN];		//�û���
	GSF_CHAR	szPsw[GSF_PASS_WORD_LEN];			//����
	GSF_U64		u64LocalRight;						//���ز���Ȩ��
	GSF_U64		u64RemoteRight;						//Զ�̲���Ȩ��
	GSF_U8		u8Type;							//�û����ͣ�0:����Ա,1:����Ա,2:��ͨ�û�
	GSF_U8      u8Encrypt;     // GSF_ENCRYPT_TYPE_E
	GSF_U16     u16Count;
	GSF_CHAR    szSalt[18]; // authType=2/3����ʹ��
	GSF_U8		u8Res[2];
	GSF_U32     u32LastModify; // Epoch
}GSF_USER_INFO_S, *LPGSF_USER_INFO_S;


//��չ�û���Ϣ�ṹ���� 
//
typedef struct _GSF_USER_EXT_CFG_S_
{
	GSF_U32		u32Size;
	GSF_U32     u32Num;			//�û���
	GSF_USER_INFO_S 	stUsers[GSF_MAX_USR_EXT_NUM];
}GSF_USER_EXT_CFG_S, *LPGSF_USER_EXT_CFG_S;


//�¶�
typedef struct _GSF_TEMP_STATE_S
{
    int  nTemperature[GSF_MAX_SUPPORT_CPU];	 // �¶�
}GSF_TEMP_STATE_S;


//�豸�汾��Ϣ(ֻ��)
typedef struct _GSF_DEV_CFG_S_
{   
    GSF_U32     u32Size;
	GSF_U8		u8SerialId[DEVSDK_SERIALID_LEN];//�豸Id
	GSF_U8		u8DiNum;						//��������·��
	GSF_U8		u8DoNum;						//�������·��
	GSF_U8		u8ChnNum;						//�豸·��
	GSF_U8		u8Rs232Num;						//�豸232 ���ڸ���
	GSF_U8		u8Rs485Num;						//�豸485 ���ڸ���
	GSF_U8		u8NetCardNum;					//��������
	GSF_U16		u16DevType ;					//�豸����						
	GSF_U32		u32HwVer;						//Ӳ���汾��
	GSF_U32		u32CoreVer;						//�ں˰汾��
	GSF_U32		u32SoftVer;						//����汾��
	GSF_U32		u32SensorType[DEVSDK_CHN_NUM];	//ǰ��sensor ����
    GSF_U32     u32ChipType;	
    GSF_U32     u32DevFeature;                 	//�豸���� GSF_FEATURE_TYPE_E
    GSF_U32     u32IvsType;                 	//֧�ֵ����ܷ������ͣ���GSF_IVS_TYPE_E
    GSF_U32     u32Protocol;                	//֧�ֵ�����Э��
    GSF_CHAR    szProductName[DEVSDK_NAME_LEN];
}GSF_DEV_CFG_S, *LPGSF_DEV_CFG_S;

//����ṹ
typedef struct _GSF_RECT_S
{
	GSF_S32		s32X;							//���x����,����(704, 576)����ϵ
	GSF_S32		s32Y;							//���y����,����(704, 576)����ϵ
	GSF_U32		u32Width;						//������,����(704, 576)����ϵ
	GSF_U32		u32Height;						//����߶�,����(704, 576)����ϵ
}GSF_RECT_S, *LPGSF_RECT_S;

//ʱ��νṹ
typedef struct _GSF_SEGMENT_S
{
	GSF_U32 	u8Open:8;						//����
	GSF_U32		beginSec:24;					//��ʼʱ������ 3600*hour+60*min+sec
	GSF_U32		u8Rsvd:8;						//����
	GSF_U32		endSec:24;						//����ʱ������ 3600*hour+60*min+sec
}GSF_SEGMENT_S, *LPGSF_SEGMENT_S;

//�澯����
typedef struct _GSF_LINKACT_S
{
	GSF_U32		u32ShootChannel;	//ץ��ͨ��(bit)(0 ��ץ�� 1��һͨ��ץ��)
	GSF_U32		u32ShootInterval;	//ץ�ļ��(����),0:����ץ��
	GSF_U32		u32RecTime;         //¼��ʱ��(����ֹͣ��¼�񱣳�ʱ��: ��)
	GSF_U32		u32RecChannel;		//¼��ͨ��(bit) (0 ��¼�� 1��һͨ��¼��)
	GSF_U32		u32PCRecChannel;    //����PC¼��ͨ��	
	GSF_U32		u32OutPut;			//�������(bit)
	GSF_U32		u32OutPersist;		//�������ʱ��
	GSF_U8		u8ShootMode;		//����ץ��ģʽ,0���ش洢��1ftp�ϴ���2email
	GSF_U8		u8ShootNum;         //ץ������	
	GSF_U8		u8RecMode;			//����¼��ģʽ��0���ش洢��1ftp�ϴ�	
	GSF_U8		u8RecPreTime;		//Ԥ¼ʱ��(��)
	GSF_U8		u8Email;            //����email
	GSF_U8      u8BeepEnable;       //���÷��� 0-��Ч   1-��Ч
    GSF_U8      u8BeepTime;         //����ʱ��(�� , 0~255)
    GSF_U8      u8PtzAction;        //�� GSF_PTZ_JOIN_ACTION_TYPE
    GSF_U8      u8PtzNo;			//
	GSF_U8		u8ReportAlarmHost;  //�ϱ�����������: 1-�ϱ���0-���ϱ�
	GSF_U8		u8Ftp;				// ���� FTP
	GSF_U8      u8ShootDelay;       // �ӳ�ץ��: ��
	GSF_U8		u8Rsvd[8];
}GSF_LINKACT_S, *LPGSF_LINKACT_S;

//�ƶ��������
typedef struct _GSF_MD_CFG_S_
{
  GSF_U32			u32Size;
  GSF_U8			u8Enable;			//�Ƿ����ƶ���� 0--�ر� 1--����			
  GSF_U8			u8Level;			//��������� 0--�ܸ� 1--�� 2--�� 3--�� 4--�ܵ�	
  GSF_U16           u16AlarmKeepTime; 	//��������ʱ��(0 - 65535)s
  GSF_RECT_S		struRect[GSF_VMD_REG_NUM];		//�������
  GSF_SEGMENT_S		struSeg[GSF_MAX_SEG_NUM][8];  	//0--������1--����һ 2--���ڶ�7--ÿ��	
  GSF_LINKACT_S		struLinkAct;					//�ƶ����������Ϣ
  GSF_U8            u8AllDay[8];
  GSF_U8            u8Mask[GSF_VMD_REG_NUM];
  GSF_U8			u8Res2[4];
  GSF_RECT_S        struRect2[GSF_VMD_REG_NUM-1][GSF_VMD_REG_NUM];
}GSF_MD_CFG_S, *LPGSF_MD_CFG_S;

//��ṹ
typedef struct _GSF_POINT_S
{
	GSF_S32		s32X;							//���x����
	GSF_S32		s32Y;							//���y����
}GSF_POINT_S, *LPGSF_POINT_S;


typedef struct _GSF_DI_CFG_S_
{
  GSF_U32			u32Size;
  GSF_CHAR			chDiName[GSF_NAME_LEN];	//di̽ͷ����
  GSF_U8			u8Enable;				//�Ƿ���di������� 0--������ 1--����
  GSF_U8			u8DiType;				//0--����̽ͷ1--����̽ͷ
  GSF_U16           u16AlarmKeepTime;       //��������ʱ��(0 - 65535s)
  GSF_SEGMENT_S		struSeg[GSF_MAX_SEG_NUM][8];  //0--������1--����һ 2--���ڶ�7--ÿ��	
  GSF_LINKACT_S		struLinkAct;				//di����������Ϣ
  GSF_U8            u8AllDay[8];            //allDay
  GSF_U8			u8Res2[8];
}GSF_DI_CFG_S, *LPGSF_DI_CFG_S;

//��Ƶ�ڵ��澯
typedef struct _GSF_OD_CFG_S_
{
  GSF_U32			u32Size;
  GSF_U8			u8Enable;		//�Ƿ��� 0--�ر� 1--����
  GSF_U8            u8Level;    	//������
  GSF_U16           u16AlarmKeepTime;//��������ʱ��(0 - 65535s)
  GSF_RECT_S		struRect[GSF_VOD_REG_NUM];	//�������
  GSF_SEGMENT_S		struSeg[GSF_MAX_SEG_NUM][8];//0--������1--����һ 2--���ڶ�7--ÿ��	
  GSF_LINKACT_S		struLinkAct;				//������Ϣ
  GSF_U8            u8AllDay[8];
  GSF_U8			u8Res2[8];
}GSF_OD_CFG_S, *LPGSF_OD_CFG_S;

//��Ƶ�ڵ�����
typedef struct _GSF_VMASK_CFG_S_
{
    GSF_U32         u32Size;
    GSF_U8          u8Enable;	        //--1.������Ƶ�ڵ� 0--������Ƶ�ڵ�
    GSF_U8          u8Res1[3];
    GSF_RECT_S      struArea[GSF_VMASK_REG_NUM]; //��Ƶ�ڵ�����
    GSF_U32         u32Color;
    GSF_U8		    u8Res2[12];
}GSF_VMASK_CFG_S, *LPGSF_VMASK_CFG_S;


//osd ����
typedef struct _GSF_OSD_CFG_S_
{
    GSF_U32     u32Size;
    GSF_U32     u32Color;
    GSF_U8      u8InvertColor;      	//��ɫ���� 3518 ��֧��
    //=============ʱ��
    GSF_U8		u8EnableTimeOsd;		//--1 ��ʾʱ��osd 0--����ʱ��osd
    GSF_U8		u8DispWeek;				//--1 ��ʾ����0--��������
    GSF_U8		u8EnableBitrateOsd;		//--1��ʾ����osd  0--��������osd
    GSF_U8		u8TimeOsdType;			//--ʱ����ʾ����
    									//--0 YYYY-MM-DD HH:MM:SS
    									//--1 MM-DD-YYYY HH:MM:SS
    									//--2 DD-MM-YYYY HH:MM:SS
    								    //--3 YYYY/MM/DD HH:MM:SS
    								    //--4 MM/DD/YYYY HH:MM:SS
    								    //--5 DD/MM/YYYY HH:MM:SS
    GSF_U8		u8HourOsdType;			//--0 24 hour --1 12 hour
	GSF_U8		u8OsdSize;				//0 С 1 �� 2 ��
	GSF_U8      u8OsdMode;				// 0-�ı�  1-bmpͼƬ
    GSF_POINT_S struTimeOrg;			//ʱ��osdλ��
    //============�ַ�osd
    GSF_U8		u8EnableTitleOsd;		//--1 ��ʾ����osd 0--���ر���osd
    GSF_U8		u8EnableTemperature;	//--1 ��ʾ�¶�osd 0--�����¶�osd
    GSF_U8		u8EnableAngle;			//--1 ��ʾ�Ƕ�osd 0--���ؽǶ�osd
    GSF_U8      u8Res1;
    GSF_POINT_S struTitleOrg;			//����osdλ��
    GSF_CHAR	szChnName[GSF_OSD_NAME_LEN];	//ͨ������ 
    //============�Զ���osd
    GSF_U8		u8EnableUsrOsd; 		//bit0 ʹ�ܵ�һ���û��˵�
    									//bit1 ʹ�ܵڶ����û��˵�
    GSF_U8		u8EnableBmpOsd; 		//bit0 ʹ�ܵ�һ��ͼƬ
    									//bit1 ʹ�ܵڶ���ͼƬ
    GSF_U8		u8TextAlign;			//�ı����� 0-������ 1-����� 2-�Ҷ��� 3-��Ե����
    GSF_U8		u8Res2;	  							
    GSF_POINT_S struUsrOrg[4];			//�û��Զ���osdλ��
    GSF_CHAR	szUsrOsdInfo[4][GSF_OSD_EXT_NAME_LEN];//�û��Զ���osd��Ϣ
    GSF_U32     u32UsrColor[4];  //�û��Զ�����ɫ
    GSF_U8      u8DispZoom; // һ���/ �����ʾ��������
    GSF_U8		u8EnableSteamOsd;//--0 ȫ��������ʾosd,1-- ��������ʾ ��2-- ��������ʾ����������֮�������������
    GSF_U8		u8Res3[2];
    GSF_U32    u32TextColor;
}GSF_OSD_CFG_S, *LPGSF_OSD_CFG_S;


//�豸ϵͳ���ýṹ��
typedef struct _GSF_DEV_SYS_CFG_S_
{
    GSF_U32     u32Size;
	GSF_CHAR	szDevName[GSF_NAME_LEN];  		//�豸����
	GSF_U8		u8Language;						//����: 0-����  1-Ӣ��  2-�ձ��� GSF_LANGUAGE_TYPE_E
	GSF_U8		u8PalStand;						//��ʽ0-pal   1-ntsc
	GSF_U8      u8AutoChangeFR;               	//�Զ������ɼ�����֡��
	GSF_U8		u8GSensorLevel;					// GSensor������, 0-4:  0-���; 4-���
	GSF_U8		u8VoMode;						//GSF_AV_VO_RES_TYPE_E
	GSF_U8		u8Res[15];
}GSF_DEV_SYS_CFG_S, *LPGSF_DEV_SYS_CFG_S;

//��Ƶ�������	
typedef struct _GSF_VENC_CFG_S_
{
    GSF_U32      u32Size;
    GSF_U16		u16Width;			// ��Ƶ������
    GSF_U16		u16Height;			// ��Ƶ����߶�

    GSF_U8		u8VideoEncType;		// ��Ƶ�����ʽ0--h264 1--MJPEG 2--JPEG	3--MPEG4  4--H265
    GSF_U8		u8PicQuilty;		// ��������0-��ã�1-�κã�2-�Ϻã�3-һ�㣬4-�ϲ5-��	
    GSF_U8		u8Gop;				// I ֡���1-200
    GSF_U8		u8FlowType;			// �������� 0--������1--������2-- fix Qp
    GSF_U32		u32BitrateRate;		// �������� 32kbps-16Mkbps
    GSF_U8		u8FrameRate;		// ����֡��1-25/30 ����sensor����av���л�ȡ֧�ֵ�֡��
    GSF_U8      u8Profile;  		// h264�ȼ�0--baseline   1--main profile     2--high profile
    GSF_U8      u8FramePriority;	// �Ƿ�֡������ 1-��   0-��
    GSF_U8		u8HdmiEncAutoAdp;	// HDMI��������Ӧ
    GSF_U8		u8IQp;				// i frame qp value
	GSF_U8		u8PQp;				// p frame qp value
	GSF_U8		u8GopMode;			// GSF_VENC_GOPMODE_E: 0 NormalP  1 DualP 2 SmartP 
	GSF_U8		u8Res[1];
}GSF_VENC_CFG_S, *LPGSF_VENC_CFG_S;

//ץ�Ĳ���
typedef struct _GSF_CAPTURE_CFG_S_
{
    GSF_U32     u32Size;
    GSF_U16		u16Width;			//ץ��ͼƬ���
    GSF_U16		u16Height;			//ץ��ͼƬ�߶�

    GSF_U8		u8VideoEncType;		//��Ƶ�����ʽ 2--JPEG �̶�ֵ
    GSF_U8		u8Quilty;			//ץ������  [1, 99]

	GSF_U8      u8ValidQuality;		//���ڼ��ݾɰ汾���������ֵ
    GSF_U8		u8Res[5];
}GSF_CAPTURE_CFG_S, *LPGSF_CAPTURE_CFG_S;

//��Ƶ���ýṹ��
typedef struct _GSF_AUDIO_CFG_S_
{
	GSF_U32		u32Size;
	GSF_U32		u32SampleRate;		//������ 8k 8000
	GSF_U8		u8Open;				//�Ƿ�����Ƶ
	GSF_U8		u8AEncType;	        //GSF_AENC_TYPE_E
	GSF_U8		u8Type;      		// 0--mic���� 1--������
	GSF_U8		u8InVolume;			// ��������  CDV_VER -> 0-100  
	GSF_U8		u8OutVolume;		// �������  CDV_VER -> 0-100  
	GSF_U8		u8AudioTipsMask;	// ��ʾ������, ��ֻ֧��on/off����(����Ч: ��bit ����:  GSF_AUDIO_TIPS_MASK_E)
	GSF_U8		u8AudioDenoise;	//��Ƶ��������
	GSF_U8		u8Res[1];
}GSF_AUDIO_CFG_S, *LPGSF_AUDIO_CFG_S;

//�������ýṹ��
typedef struct _GSF_COMM_CFG_S_
{
	GSF_U32		u32Size;
	GSF_U32		u32BaudRate;//������(bps)��0-50��1-75��2-110��3-150��4-300��
						//s5-600��6-1200��7-2400��8-4800��9-9600��
						//10-19200��11-38400��12-57600��13-115.2k 
 	GSF_U8     	u8DataBit;	//����λ��0-5λ��1-6λ��2-7λ��3-8λ
 	GSF_U8     	u8StopBit;	//ֹͣλ��0-1λ��1-2λ
  	GSF_U8     	u8Parity;	//�Ƿ�У�飺0-��У�飬1-��У�飬2-żУ��
  	GSF_U8     	u8Flowcontrol;//�Ƿ����أ�0-�ޣ�1-������,2-Ӳ����
  	GSF_U8 		u8WorkMode;		// ����ģʽ: 0 ��͸��ͨ��, 1 - PTZ ģʽ(ʹ��PTZ�еĴ��ڲ���), 3 - ������ģʽ
  	GSF_U8		u8Res[7];
}GSF_COMM_CFG_S, *LPGSF_COMM_CFG_S;

//ftp��������
//
typedef struct _GSF_FTP_CFG_S
{
	GSF_U32		u32Size;
	GSF_CHAR 	szFtpSvr[GSF_IP_ADDR_LEN];     //��ftp������ip
	GSF_CHAR    szFtpUsr[GSF_USER_NAME_LEN];   //��½�û���
	GSF_CHAR    szFtpPsw[GSF_PASS_WORD_LEN];   //��½����
	GSF_U16		u16FtpPort;
    GSF_U8      u8EnableAnony;   	//����������0- ��1- ��
	GSF_U8		u8ReUpload;			// �ϴ�ʧ���ش�����
    GSF_U8      u8DirLevel;    		//0- ��ʹ��Ŀ¼�ṹ��ֱ�ӱ����ڸ�Ŀ¼��1- ʹ��1��Ŀ¼��2- ʹ��2��Ŀ¼ 
    GSF_U8      u8TopDirMode;  		//һ��Ŀ¼��0x1- ʹ���豸����0x2- ʹ���豸�ţ�0x3- ʹ���豸ip��ַ��
    								//0x4- ʹ�ü��㣬0x5- ʹ��ʱ��(����)��0x6- ʹ���Զ���
    GSF_U8      u8SubDirMode;  		//����Ŀ¼��0x1- ʹ��ͨ������0x2- ʹ��ͨ���ţ�0x3- ʹ��ʱ��(������)��
                               	 	//0x4- ʹ�ó����ţ�0x5- ʹ���Զ���
    GSF_U8		u8Rsvd[1];
    GSF_CHAR    szTopCustomDir[GSF_PATH_LEN]; //�Զ���һ��Ŀ¼
    GSF_CHAR    szSubCustomDir[GSF_PATH_LEN]; //�Զ������Ŀ¼
    GSF_U8      u8EnableSSL; // !=0 ���� FTP Over SSL(FTPS) 
	GSF_U8      u8ConnKeep;   // !=0 ���ӱ���, ��ʹ�ó�����: 1-NOOP
	GSF_U16     u16Keepalive; // �������, u8ConnKeep��Ϊ 0 ʱ��Ч
	GSF_U8		u8Rsvd1[24];
}GSF_FTP_CFG_S, *LPGSF_FTP_CFG_S;

//smtp����
typedef struct _GSF_SMTP_CFG_S
{
	GSF_U32		u32Size;
	char        szMailHub[GSF_IP_ADDR_LEN];      //smtp ��������ַ
	char        szMailAddr[GSF_MAX_EMAIL_LEN];   //�����ʼ� ��ַ
	char        szMailUsr[GSF_MAX_EMAIL_LEN];    //�����ʼ� �˺�
	char        szMailPsw[GSF_PASS_WORD_LEN];    //�����ʼ� ����
	char        szMailRctp1[GSF_MAX_EMAIL_LEN];  //�����ʼ� ��ַ1
	char        szMailRctp2[GSF_MAX_EMAIL_LEN];  //�����ʼ� ��ַ2
	char       	szMailRctp3[GSF_MAX_EMAIL_LEN];  //�����ʼ� ��ַ3
	GSF_U8		u8EnableSSL;					//����SSL
	GSF_U8		u8Rsvd;
	GSF_U16		u16MailPort;					//smtp �������˿�
	GSF_U8      u8Rsvd2[32];
}GSF_SMTP_CFG_S, *LPGSF_SMTP_CFG_S;

//ddns ������Ϣ
typedef struct _GSF_DDNS_CFG_S
{
	GSF_U32		u32Size;
	GSF_U8		u8DDNSOpen;						//DDNS����
	GSF_U8		u8DDNSType;						//0-dyndns 1-3322
	GSF_U16		u16DDNSPort;					//DDNS�������˿�
	GSF_CHAR	szDDNSAccount[GSF_NAME_LEN];	//DDNS ע����
	GSF_CHAR	szDDNSUsr[GSF_USER_NAME_LEN];	//�û�����
	GSF_CHAR	szDDNSPsw[GSF_PASS_WORD_LEN];	//�û�����	
	GSF_U32		u32DDNSTimes;					//����ʱ��
	GSF_U8		u8Res[16];
}GSF_DDNS_CFG_S,*LPGSF_DDNS_CFG_S;

//upnp �����ṹ
//
typedef struct _GSF_UPNP_CFG_S
{
	GSF_U32		u32Size;
	GSF_U8		u8UpnpOpen;		//upnp ����
	GSF_U8		u8UpnpEthNo;	//0ʹ�ñ�������ӳ�� 1 ʹ����������ӳ��
	GSF_U8		u8UpnpModel;    //0�̶�ӳ�� 1�Զ�ӳ��
	GSF_U8		u8UpnpRefTime;  //upnp ˢ��ʱ��(��λСʱ)
	GSF_U32		u32UpnpSvr;		//upnp ������IP
	GSF_U8		u8CmdPortEnable;
    GSF_U8		u8HttpPortEnable;
    GSF_U8		u8RtspPortEnable;
    GSF_U8		u8RtmpPortEnable;
    GSF_U16		u16UpnpCmdPort;//�̶�ӳ��ģʽ��, ���ݶ˿ڵ������˿�
	GSF_U16		u16UpnpHttpPort; //�̶�ӳ��ģʽ��, http�˿ڵ������˿�
	GSF_U16		u16UpnpRtspPort;//�̶�ӳ��ģʽ��, Rtsp�˿ڵ������˿�	  
	GSF_U16		u16UpnpRtmpPort;	//�̶�ӳ��ģʽ��, Rtmp�˿ڵ������˿�
	GSF_U16		u16UpnpCmdPortS; //���ݶ˿ڵ������˿� 0����û��ӳ��򲻳ɹ�
	GSF_U16		u16UpnpHttpPortS; //Web�˿ڵ������˿� 0����û��ӳ��򲻳ɹ�	
	GSF_U16		u16UpnpRtspPortS;//Rtsp�˿ڵ������˿� 0����û��ӳ��򲻳ɹ�
	GSF_U16		u16UpnpRtmpPortS;//Rtmp�˿ڵ������˿� 0����û��ӳ��򲻳ɹ�
	GSF_U32		u32UpnpWan;		//upnp ����������IP�������ֽ���
	GSF_U8		u8Res[4];
}GSF_UPNP_CFG_S, *LPGSF_UPNP_CFG_S;

typedef struct _GSF_NET_UPGRADE_PACK_S_
{
	GSF_U32      u32Seq;			// ����ţ���1 ��ʼ
	GSF_U32      u32PackType; 		// 0 ��һ��2 ������1 ����������
    GSF_U32		 u32DataLen;		// ��ǰ�����ݳ���u32DataLen <= sizeof(u8DataBuff);
	GSF_U8       u8DataBuff[1024];	// ������
}GSF_NET_UPGRADE_PACK_S, *LPGSF_NET_UPGRADE_PACK_S;


// ͨ��net server ����
typedef struct _GSF_UPGRADE_CMD_S_
{
	GSF_U8 u8UpdType; 	// 0 ָ�����������ļ�����, 1  �����������ݰ�
	GSF_U8 u8Packs;     // 0 ����Ƭ, >= 1 ��Ƭ����
	GSF_U8 u8Res[2];
    union
    {
        char szFilePath[GSF_PATH_LEN];  
        GSF_NET_UPGRADE_PACK_S stUpdPack;
    };

    int (*funcFinishNotify)(int nSuccess);
}GSF_UPGRADE_CMD_S, *LPGSF_UPGRADE_CMD_S;



//ntp �����ṹ����
typedef struct _GSF_NTP_CFG_S
{
	GSF_U32		u32Size;
	GSF_U8		u8NtpOpen;				//ntp����
	GSF_U8		u8Resv;
	GSF_U8		u8EuroTime;				//==1:��������ʱ 0:����������ʱ
	GSF_U8		u8NtpRefTime;			//ntp ��ʱ��ʱʱ��(��λСʱ)
	int			nTimeZone;				//ʱ�� ��λs(-12*3600~11*3600)
	GSF_CHAR    szNtpServer[GSF_IP_ADDR_LEN];
	GSF_U8      u8WebTimeDisableSync;   //�Ƿ����IE��½�ɹ����Զ���ʱ: 0-������; 1-����
	GSF_U8      u8Res1[2];
	GSF_U8      u8NtpRefTimeSecSupp;
	GSF_U32     u32NtpRefTimeSec;      // ntp ��ʱ��ʱʱ��(��λ��)
	GSF_U8		u8Res2[8];
}GSF_NTP_CFG_S, *LPGSF_NTP_CFG_S;

typedef struct _GSF_REC_STRC_S_
{
	GSF_U32		u32Size;		
	GSF_U8		u8OverWrite;		//0 ������1 ����
	GSF_U8		u8RecStream;		//0 ������¼��1 ������¼��
	GSF_U8		u8PreRecSecs;		//Ԥ¼ʱ��(��)
	GSF_U8		u8SaveMode;		//����ģʽ
	//��ʱ¼�����
	GSF_SEGMENT_S	struTimerSeg[GSF_MAX_SEG_NUM][8]; //0--������1--����һ 2--���ڶ�7--ÿ��	
	GSF_U8       u8AllDay[8];      // 
	GSF_U8		 u8EnableTime;
	//
	GSF_U8		 u8RecStreamType;	//0 ��Ƶ�� 1 ������
	GSF_U8		 u8RecPackTime;		// ¼����ʱ��(����)
	GSF_U8		 u8Res;
}GSF_REC_STRC_S, *LPGSF_REC_STRC_S;

typedef struct _GSF_TIMER_CAP_S_
{
	GSF_U32		u32Size;
    GSF_U8      u8Enable;		// 0 ���ã�1����
	GSF_U8		u8SaveMode;		// ����ģʽ0���ش洢��1ftp�ϴ���2 email
	GSF_U16     u16CapIntv;		// ץ�ļ��
	//��ʱץ�Ĳ���
	GSF_SEGMENT_S	struTimerSeg[GSF_MAX_SEG_NUM][8]; //0--������1--����һ 2--���ڶ�7--ÿ��	
	GSF_U8          u8AllDay[8];      // 
	GSF_U8          u8Quality;  // ץ������ֵ: 1-100
    GSF_U8          u8Ftp;
    GSF_U8          u8Res[2];
}GSF_TIMER_CAP_S, *LPGSF_TIMER_CAP_S;

//ʱ��ṹ
typedef struct _GSF_TIME_S
{
	GSF_U8		u8Year;							//��(��1900�꿪ʼ)
	GSF_U8		u8Month;						//��1~12
	GSF_U8		u8Day;							//��1~31
	GSF_U8		u8WDay;							//������0 ����һ1.....
	GSF_U8		u8Hour;							//ʱ0~23
	GSF_U8		u8Minute;						//��0~59
	GSF_U8		u8Second;						//��0~59
	GSF_U8		u8Millisec;						//����(0~99)(�ٷ�֮һ��)
	GSF_S32		s32Zone;						// timezone  second
}GSF_TIME_S, *LPGSF_TIME_S;

typedef struct _GSF_DO_CFG_S_
{
  GSF_U32			u32Size;
  GSF_CHAR			chDoName[GSF_NAME_LEN];	//�����������
  GSF_U8			u8Enable;				//�Ƿ���do�������0--������ 1--����
  GSF_U8			u8Res1[3];
  GSF_U32	        u32AlarmTime;        	//�������ʱ��(0 - 60s)
  GSF_SEGMENT_S		struSeg[GSF_MAX_SEG_NUM][8];  //0--������1--����һ 2--���ڶ�7--ÿ��	
  GSF_U8            u8AllDay[8];            //allDay
  GSF_U8			u8Res2[8];
}GSF_DO_CFG_S, *LPGSF_DO_CFG_S;

typedef struct _GSF_ONVIF_CFG_S_
{
	GSF_U32			u32Size;
	GSF_U8			u8Auth;           // �Ƿ�ʹ��ONVIF  Authentication 
	GSF_U8 			u8RtspAuth;       // �Ƿ�ʹ��rtsp user  Authentication 
	GSF_U16			u16Port;          // default  8000
	GSF_U16			u16RtspPort;      // default  8554
	GSF_U8          u8TZType;         // default  CST, GSF_ONVIF_TZ_TYPE_E: 20180714
	GSF_U8			u8Res2[21];
}GSF_ONVIF_CFG_S;

// GB28181 ƽ̨����v2
typedef struct _GSF_GB28181_INFO_S
{
	GSF_CHAR 		szDeviceID[32]; // �豸ID
	GSF_CHAR 		szDevicePwd[32];// �豸���� 
	GSF_CHAR 		szServerID[32]; // ������ID
	GSF_CHAR 		szServerIp[32];	// ��������ַ
	GSF_U16         u16ServerPort;  // �������˿�
	GSF_U16         u16MsgPort; 	// ������Ϣ�˿�    

	GSF_U8          u8Enable;       // ��������
	GSF_U8          u8DefStream;    // Ĭ������(��=0, ��=1, ��=2)
	GSF_U8			u8ProtocolType; // 0 -- udp  1---tcp
	GSF_U8 	        u8Parental;     // �Ƿ������豸

	GSF_U32         u32Expires;       // ע����Ч��(s)
	GSF_U32			u32RegInterval;	  // ע����(s)
	GSF_U32         u32Heartbeat;     // ��������(s)
	GSF_U32         u32TimeOutCnt;    // ������ʱ����, ��������������ע��
	GSF_U32         u32SecrectAttr;   // ��������
	
	GSF_CHAR        szMediaChID[GSF_MAX_SUPPORT_CHN][32];  // ý��ͨ��ID
	GSF_CHAR        szAlarmChID[GSF_MAX_SUPPORT_DI][32];   // ����ͨ��ID
	GSF_CHAR        szOwner[32];      // �豸����
	GSF_CHAR        szMaker[32];      // �豸������
	GSF_CHAR        szCivilCode[32];  // �豸��������������
	GSF_CHAR        szParentID[32];   // ���豸/����/ϵͳID
	GSF_CHAR        szInstallAddr[32];// �豸��װ��ַ
	GSF_S32         s32LatiTude;      // γ�� �Ŵ�1000000���ĸ�����: 80765432  -> 80.765432
	GSF_S32         s32LongtiTude;    // ���� �Ŵ�1000000���ĸ�����: 100234567 -> 100.234567
	GSF_CHAR        szBlock[32];      // �豸��������
}GSF_GB28181_INFO_S;

typedef struct _GSF_GB28181_CFG_S
{
	GSF_U32         u32Size;
	GSF_GB28181_INFO_S stGbCfgs[GSF_MAX_SUPPORT_GB28181_SERVER];
	GSF_S32         nStatus[GSF_MAX_SUPPORT_GB28181_SERVER];  // ע��״̬
}GSF_GB28181_CFG_S, *LPGSF_GB28181_CFG_S;

typedef struct _GSF_AUTO_REBOOT_CFG_S_
{
	GSF_U32			u32Size;
	
	GSF_U8			u8Enable;
	GSF_U8			u8WeekDay;
	GSF_U16			u16Res;
	GSF_U32			u32Second;
	GSF_U32			u32Res;
}GSF_AUTO_REBOOT_CFG_S; 

typedef struct _GSF_CAM_POS_INFO_
{
	GSF_U16		u16ZoomPos;		//��ͷλ��(0-16384)
	GSF_U16		u16FocusPos;	//�۽�λ��(0-16384)
	GSF_U16 	u16IrisPos;		//��Ȧλ��(40-900)
	GSF_U16 	u16Res;
}GSF_CAM_POS_INFO;

typedef struct _GSF_ROI_CFG_S_	//����Ȥ�������
{
    GSF_U32      u32Size;
    GSF_U8       u8Enable;          //ʹ��1:����  0: ������
    GSF_U8       u8AbsQp;           //0-���    1-��������
    GSF_U8       u8Res[2];
    GSF_U32      s32QpVal;          //����ֵ
	GSF_RECT_S   stRect[GSF_ROI_REG_NUM];
}GSF_ROI_CFG_S;

#define GSF_IVP_MAX_CLASS_NUM  2
#define GSF_IVP_MAX_OBJ_NUM    16

typedef struct{
    GSF_S32 x;
    GSF_S32 y;
    GSF_U32 width;
    GSF_U32 height;
}gsf_ivp_rect;

typedef struct _gsf_ivp_obj_
{
	gsf_ivp_rect rect;
	float quality;
}gsf_ivp_obj;

typedef  struct _GSF_IVP_PD_RECT_CFG_
{
	GSF_U8       u8ClassNum;
	GSF_U8       u8RectNum[GSF_IVP_MAX_CLASS_NUM];
	GSF_U8		 u8Res;
	gsf_ivp_obj	 rect[GSF_IVP_MAX_CLASS_NUM][GSF_IVP_MAX_OBJ_NUM];
}GSF_IVP_PD_RECT_CFG, *PGSF_IVP_PD_RECT_CFG;

// �������״̬�����ñ�������ṹ��
typedef struct _GSF_ALARMOUT_CTRL_S_
{
    GSF_U32     u32SetManl;			// 1 manual control	�ֶ� 0 alarm link	��������
    GSF_U32     u32ManlStatus;		// u32SetManl = 1, 0 open, 1 close		�ֶ�����״̬
    GSF_U32		u32AlarmTimes;		// u32SetManl = 0, alarm link alarm out time		�������ӱ������ʱ�� 
}GSF_ALARMOUT_CTRL_S, *LPGSF_ALARMOUT_CTRL_S;


//��ҹ�л�����
//-------------------------------------------------------------------
typedef struct _GSF_DAYNIGHT_CFG_S
{
    GSF_U32      u32Size;
    GSF_U8       u8FilterType;		 //GSF_IR_FILTER_TYPE_E, 0-���죬1-ҹ��2-�Զ���3-��ʱ��4-�������봥��  5-ispLum����
    GSF_U8       u8FilterLevel;        //�Զ��л�ʱ 0 �͵�ƽ�л� 1 �ߵ�ƽ�л�; TWHQ night-input 
    GSF_U8       u8AlarmTrigState;     //�������봥�� �������봥��״̬��0-���죬1-ҹ��
    GSF_U8       u8FilterSignal;		 // IRCUT�ź�����: 0-���� ;  1-����
    GSF_SEGMENT_S Segs[GSF_MAX_SEG_NUM]; //��ʱ�л�ʱ���
    GSF_U8       u8IrThreshold;  // PV, threshold; TWHQ, day->night threshold [0, 20]
    GSF_U8       u8IrThreshold2; // TWHQ, night->day threshold [0, 20]
    GSF_U8       u8IrOptimizer;  // TWHQ [0, 20]
    GSF_U8       u8AgcMax;       // TWHQ [0, 20]
    GSF_U8       u8LiveLevel;    // TWHQ [0, 20]
    GSF_U8       u8Delay;        // TWHQ [0, 255]  Delay(auto)
    GSF_U8       u8DNFocusMode;      // TWHQ 0-off, 1-focus   2-preset
    GSF_U8       u8DelayEx;      // TWHQ [0, 255]  Delay(external)
    GSF_U8       u8AgcMode;  
	GSF_U8       u8AgcManul;  
	GSF_U8       u8ND;  
	GSF_U8       u8DNDelay;  
	GSF_U8       u8NDDelay;
	GSF_U8       u8LampType;     // 0-�����  1-�׹��(LED)
	GSF_U8       u8LampCtrl;     // 0-�Զ�    1-����   2-����
	GSF_U8       u8Sensitive;    // ������    1-10, Ĭ�� 5 
}GSF_DAYNIGHT_CFG_S, *LPGSF_DAYNIGHT_CFG_S;


typedef enum _GSF_ROTATE_TYPE_
{
	GSF_ROTATE_NONE = 0,
	GSF_ROTATE_90,
	GSF_ROTATE_180,
	GSF_ROTATE_270,
}GSF_ROTATE_TYPE_E;

typedef enum _GSF_POWER_FREQ_TYPE_
{
	GSF_POWERFREQ_NONE = 0,
	GSF_POWERFREQ_50HZ,
	GSF_POWERFREQ_60HZ
}GSF_POWERFREQ_TYPE_E;

typedef enum _GSF_AWB_TYPE_
{
	GSF_AWB_AUTO = 0,
	GSF_AWB_INDOOR,
	GSF_AWB_OUTDOOR,
	GSF_AWB_AUTOTRACKING,
	GSF_AWB_MANUAL,
}GSF_AWB_TYPE_E;

//��������
typedef enum _GSF_SCENE_E_
{
      VCT_SCENE_DAY   = 0
    , VCT_SCENE_NIGHT = 1
    , VCT_SCENE_BUTT
}GSF_SCENE_E;

//IMAGE������Ŷ���
typedef enum _GSF_IMA_IDX_E_
{
      VCT_IMA_IDX_BRIGHTNESS = 0       // 0 
    , VCT_IMA_IDX_CONTRAST             // 1
    , VCT_IMA_IDX_SATURATION 	       // 2
    , VCT_IMA_IDX_HUE		  	       // 3
    , VCT_IMA_IDX_GAMMA		           // 4
    , VCT_IMA_IDX_SHARPNESS	           // 5
    , VCT_IMA_IDX_MIRROR		       // 6
    , VCT_IMA_IDX_FLIP		           // 7
    , VCT_IMA_IDX_MODE	               // 8
    , VCT_IMA_IDX_PWDFREQ	           // 9
    , VCT_IMA_IDX_DIS                  // 10
    , VCT_IMA_IDX_2DENOISE             // 11
    , VCT_IMA_IDX_3DNLEVEL             // 12
    , VCT_IMA_IDX_HLC                  // 13
    , VCT_IMA_IDX_3DNSFINCSTH          // 14
    , VCT_IMA_IDX_3DNTFSTH             // 15
    , VCT_IMA_IDX_SHARPEN              // 16
    , VCT_IMA_IDX_DYNBADPIX            // 17
    , VCT_IMA_IDX_3DNTFLIMIT           // 18
    , VCT_IMA_IDX_LDC_STRENGH          // 19
    , VCT_IMA_IDX_ANTI_FALSECOLOR_TYPE // 20
    , VCT_IMA_IDX_ANTI_FOG_TYPE        // 21
    , VCT_IMA_IDX_ROTATE_TYPE          // 22
    , VCT_IMA_IDX_ANTI_FOG_STRENGTH    // 23
    , VCT_IMA_IDX_3DCHROMARANGE        // 24
    , VCT_IMA_IDX_GAMMA_CURVE          // 25
    , VCT_IMA_IDX_SHADING_TYPE         // 26
    , VCT_IMA_IDX_BLC			       // 27
    , VCT_IMA_IDX_FROZEN		       // 28
    , VCT_IMA_IDX_SHARPEN_MANUAL       // 29
    , VCT_IMA_IDX_DIS_VI               // 30
    , VCT_IMA_IDX_OIS                  // warning ========>>> 31;
    , VCT_IMA_IDX_BUTT = 32
}GSF_IMA_IDX_E;


//��Ƶ�����������Ͷ���
typedef enum _GSF_IMA_MASK_E_
{
    VCT_IMA_BRIGHTNESS 	         = (1<<VCT_IMA_IDX_BRIGHTNESS            ) ,// 0 ����
    VCT_IMA_CONTRAST             = (1<<VCT_IMA_IDX_CONTRAST              ) ,// 1 �Աȶ�
    VCT_IMA_SATURATION 	         = (1<<VCT_IMA_IDX_SATURATION 	         ) ,// 2 ���Ͷ�
    VCT_IMA_HUE		  	         = (1<<VCT_IMA_IDX_HUE		  	         ) ,// 3 ɫ��
    VCT_IMA_GAMMA		         = (1<<VCT_IMA_IDX_GAMMA		         ) ,// 4 GAMMAֵ
    VCT_IMA_SHARPNESS	         = (1<<VCT_IMA_IDX_SHARPNESS	         ) ,// 5 ���
    VCT_IMA_MIRROR		         = (1<<VCT_IMA_IDX_MIRROR		         ) ,// 6 ����
    VCT_IMA_FLIP		         = (1<<VCT_IMA_IDX_FLIP		             ) ,// 7 ��ת
    VCT_IMA_MODE	         	 = (1<<VCT_IMA_IDX_MODE	                 ) ,// 8 �Զ���ת
    VCT_IMA_PWDFREQ	             = (1<<VCT_IMA_IDX_PWDFREQ	             ) ,// 9 ����Ƶ��1   
    VCT_IMA_DIS                  = (1<<VCT_IMA_IDX_DIS                   ) ,// 10 ���ַ���
    VCT_IMA_2DENOISE             = (1<<VCT_IMA_IDX_2DENOISE              ) ,// 11 2D����
    VCT_IMA_3DNLEVEL             = (1<<VCT_IMA_IDX_3DNLEVEL              ) ,// 12 3D����ȼ���0 �ر� 1-5 ǿ��Խ��Խǿ
    VCT_IMA_HLC                  = (1<<VCT_IMA_IDX_HLC                   ) ,// 13 ǿ������
    VCT_IMA_3DNSFINCSTH          = (1<<VCT_IMA_IDX_3DNSFINCSTH           ) ,// 14 3D�������ȥ��ǿ��2
    VCT_IMA_3DNTFSTH             = (1<<VCT_IMA_IDX_3DNTFSTH              ) ,// 15 3D����ʱ��ȥ��ǿ��
    VCT_IMA_SHARPEN              = (1<<VCT_IMA_IDX_SHARPEN               ) ,// 16 ͼ����
    VCT_IMA_DYNBADPIX            = (1<<VCT_IMA_IDX_DYNBADPIX             ) ,// 17 ��̬����У��
    VCT_IMA_3DNTFLIMIT           = (1<<VCT_IMA_IDX_3DNTFLIMIT            ) ,// 18 3D����ʱ������ֵ 
    VCT_IMA_LDC_STRENGH          = (1<<VCT_IMA_IDX_LDC_STRENGH           ) ,// 19 ��ͷ�������ǿ��
    VCT_IMA_ANTI_FALSECOLOR_TYPE = (1<<VCT_IMA_IDX_ANTI_FALSECOLOR_TYPE  ) ,// 20 ȥα��
    VCT_IMA_ANTI_FOG_TYPE        = (1<<VCT_IMA_IDX_ANTI_FOG_TYPE         ) ,// 21 ȥ��
    VCT_IMA_ROTATE_TYPE          = (1<<VCT_IMA_IDX_ROTATE_TYPE           ) ,// 22 ��ת GSF_ROTATE_TYPE_E
    VCT_IMA_ANTI_FOG_STRENGTH    = (1<<VCT_IMA_IDX_ANTI_FOG_STRENGTH     ) ,// 23 ȥ��ǿ��==2D����
    VCT_IMA_3DCHROMARANGE        = (1<<VCT_IMA_IDX_3DCHROMARANGE         ) ,// 24 3D����ɫ�Ƚ���ǿ��
    VCT_IMA_GAMMA_CURVE          = (1<<VCT_IMA_IDX_GAMMA_CURVE           ) ,// 25 GAMMA��������
    VCT_IMA_SHADING_TYPE         = (1<<VCT_IMA_IDX_SHADING_TYPE          ) ,// 26��ӰУ��
    VCT_IMA_BLC				     = (1<<VCT_IMA_IDX_BLC			         ) ,// 27 ���ⲹ��
    VCT_IMA_FROZEN			     = (1<<VCT_IMA_IDX_FROZEN                ) ,// 28 ��Ƶ����
    VCT_IMA_SHARPEN_MANUAL       = (1<<VCT_IMA_IDX_SHARPEN_MANUAL        ) ,// 29 �ֶ�ͼ����
    VCT_IMA_DIS_VI               = (1<<VCT_IMA_IDX_DIS_VI                ) ,// 30 VI���ַ���
    VCT_IMA_OIS                  = (1<<VCT_IMA_IDX_OIS                   ) ,// 31��ѧ����
    VCT_IMA_ALL			         = 0xFFFFFFFF,
}GSF_IMA_MASK_E;


//ͼ���������
typedef struct _GSF_IMA_PARA_S_
{
	GSF_U8	u8Value;			//����ֵ
	GSF_U8	u8Default;			//ȱʡ����ֵ��ֻ��
	GSF_U8	u8Step;				//����ֵ����ֵ��ֻ��
	GSF_U8	u8Res;
}GSF_IMA_PARA_S;

typedef struct _GSF_IMA_CFG_S_
{	
    GSF_U32              u32Size;
/* ��Ч֧�ֵĲ������ò����ڻ�ȡ��ʱ����Ч�������ж��豸�Ƿ�֧�ָò��� */
	GSF_IMA_MASK_E	eSuppMask;
/* ������Ч�Ĳ������ò��������õ�ʱ����Ч������ָ���������õĲ��� */
	GSF_IMA_MASK_E	eSetMask;	
	GSF_IMA_PARA_S	struParam[VCT_IMA_IDX_BUTT]; 
}GSF_IMA_CFG_S, *LPGSF_IMA_CFG_S;

typedef enum _GSF_IMA_EXT_IDX_E_
{
      VCT_IMA_EXT_IDX_COLOR_TONE_RED   = 0       // 0 ɫ����ɫ����
    , VCT_IMA_EXT_IDX_COLOR_TONE_GREEN           // 1 ɫ����ɫ����
    , VCT_IMA_EXT_IDX_COLOR_TONE_BLUE 	         // 2 ɫ����ɫ����
    , VCT_IMA_EXT_IDX_SCENCE_MODE				//  3
    , VCT_IMA_EXT_IDX_BUTT = 32
}GSF_IMA_IDX_EXT_E;


typedef enum _GSF_IMA_EXT_MASK_E_
{
    VCT_IMA_EXT_COLOR_TONE_RED 	 = (1<<VCT_IMA_EXT_IDX_COLOR_TONE_RED ) , // ɫ����ɫ����
    VCT_IMA_EXT_COLOR_TONE_GREEN = (1<<VCT_IMA_EXT_IDX_COLOR_TONE_GREEN), // ɫ����ɫ����
    VCT_IMA_EXT_COLOR_TONE_BLUE  = (1<<VCT_IMA_EXT_IDX_COLOR_TONE_BLUE),  // ɫ����ɫ����
    VCT_IMA_EXT_ALL			         = 0xFFFFFFFF,
}GSF_IMA_EXT_MASK_E;

typedef struct _GSF_IMA_EXT_PARA_S_
{
	GSF_U8	u8Value;			//����ֵ
	GSF_U8	u8Default;			//ȱʡ����ֵ��ֻ��
	GSF_U8	u8Step;				//����ֵ����ֵ��ֻ��
	GSF_U8	u8Res;
}GSF_IMA_EXT_PARA_S;

typedef struct _GSF_IMA_EXT_CFG_S_
{
    GSF_U32              u32Size;
/* ��Ч֧�ֵĲ������ò����ڻ�ȡ��ʱ����Ч�������ж��豸�Ƿ�֧�ָò��� */
	GSF_IMA_EXT_MASK_E	eSuppMask;
/* ������Ч�Ĳ������ò��������õ�ʱ����Ч������ָ���������õĲ��� */
	GSF_IMA_EXT_MASK_E	eSetMask;
	GSF_IMA_EXT_PARA_S	struParam[VCT_IMA_EXT_IDX_BUTT]; 
}GSF_IMA_EXT_CFG_S, *LPGSF_IMA_EXT_CFG_S;

//==================== 3A���� ====================== //
typedef enum _GSF_3A_WDR_MODE_E_
{
	GSF_3A_WDR_DRC,
	GSF_3A_WDR_COMPOSE,
	GSF_3A_WDR_CLOSE,
}GSF_3A_WDR_MODE_E;

typedef enum _GSF_3A_IDX_E_
{
      VCT_3A_IDX_WDR_STRENGH    = 0 // 0 wdr-strengh
    , VCT_3A_IDX_IRIS_TYPE      	// 1
    , VCT_3A_IDX_IRIS_PARAM     	// 2
    , VCT_3A_IDX_AE             	// 3
    , VCT_3A_IDX_AF             	// 4
    , VCT_3A_IDX_AWB            	// 5
    , VCT_3A_IDX_AWBRED         	// 6
    , VCT_3A_IDX_AWBBLUE        	// 7
    , VCT_3A_IDX_COLOUR         	// 8
    , VCT_3A_IDX_AE_MaxAG       	// 9
    , VCT_3A_IDX_AE_MaxDG       	// 10
    , VCT_3A_IDX_WDR_MinSlope   	// 11
    , VCT_3A_IDX_WDR_MaxSlope   	// 12
    , VCT_3A_IDX_AE_Compansation	// 13
    , VCT_3A_IDX_AE_MODE        	// 14
    , VCT_3A_IDX_SHUTTER        	// 15
    , VCT_3A_IDX_AE_STEP        	// 16
    , VCT_3A_IDX_WDR_MODE			// 17 GSF_3A_WDR_MODE_E
    , VCT_3A_IDX_AE_MINTIME			// 18
    , VCT_3A_IDX_AE_MAXTIME			// 19
    , VCT_3A_IDX_AE_SYSGAINMAX  	// 20
    , VCT_3A_IDX_AE_STRATEGY    	// 21
    , VCT_3A_IDX_AE_HISTOFFSET		// 22
    , VCT_3A_IDX_WDR_EXPO_RATIO		// 23
    , VCT_3A_IDX_WDR_EXPO_RATIO_MAX	// 24
    , VCT_3A_IDX_WDR_EXPO_RATIO_MIN	// 25
    , VCT_3A_IDX_AE_COMPANSATION_LEVEL // 26
    , VCT_3A_IDX_IRIS_STOPPARAM		  // 27
    , VCT_3A_IDX_AF_MODE              // 28
    , VCT_3A_IDX_AE_SYSGAINMIN        // 29
    , VCT_3A_IDX_AE_SLOW_SHUTTER	  // 30
    , VCT_3A_IDX_BUTT = 32
}GSF_3A_IDX_E;


//--3A�����������Ͷ���
typedef enum _GSF_3A_MASK_E_
{
    VCT_3A_WDR_STRENGH      = (1<<VCT_3A_IDX_WDR_STRENGH    ),// 0 ��̬ǿ��
    VCT_3A_IRIS_TYPE        = (1<<VCT_3A_IDX_IRIS_TYPE      ),// 1 ���ù�Ȧ����  //0 �Զ���Ȧ 1 �ֶ���Ȧ
    VCT_3A_IRIS_PARAM       = (1<<VCT_3A_IDX_IRIS_PARAM     ),// 2 ���ù�ȦУ��ֵ��
    VCT_3A_AE               = (1<<VCT_3A_IDX_AE             ),// 3 �Զ��ع�       //0: �Զ��ع⣬ 1~255: �ֶ��ع�ʱ��
    VCT_3A_AF               = (1<<VCT_3A_IDX_AF             ),// 4 �Զ��۽�
    VCT_3A_AWB              = (1<<VCT_3A_IDX_AWB            ),// 5 �Զ���ƽ��
    VCT_3A_AWBRED           = (1<<VCT_3A_IDX_AWBRED         ),// 6 �Զ���ƽ��RED
    VCT_3A_AWBBLUE          = (1<<VCT_3A_IDX_AWBBLUE        ),// 7 �Զ���ƽ��BLUE
    VCT_3A_COLOUR           = (1<<VCT_3A_IDX_COLOUR         ),// 8 ��ת��
    VCT_3A_AE_MaxAG         = (1<<VCT_3A_IDX_AE_MaxAG       ),// 9 �Զ��ع����ģ������
    VCT_3A_AE_MaxDG         = (1<<VCT_3A_IDX_AE_MaxDG       ),// 10 �Զ��ع������������  
    VCT_3A_WDR_MinSlope     = (1<<VCT_3A_IDX_WDR_MinSlope   ),// 11 ���ÿ�̬��С����
    VCT_3A_WDR_MaxSlope     = (1<<VCT_3A_IDX_WDR_MaxSlope   ),// 12 ���ÿ�̬�������
    VCT_3A_AE_Compansation  = (1<<VCT_3A_IDX_AE_Compansation),// 13 �Զ��ع�����ֵ
    VCT_3A_AE_MODE          = (1<<VCT_3A_IDX_AE_MODE        ),// 14 �Զ��ع�����
    VCT_3A_SHUTTER          = (1<<VCT_3A_IDX_SHUTTER        ),// 15 ���õ��ӿ���
    VCT_3A_AE_STEP          = (1<<VCT_3A_IDX_AE_STEP        ),// 16 �Զ��ع������ٶ�
    VCT_3A_WDR_MODE			= (1<<VCT_3A_IDX_WDR_MODE       ),// 17 ��̬ģʽ GSF_3A_WDR_MODE_E
    VCT_3A_AE_MINTIME		= (1<<VCT_3A_IDX_AE_MINTIME     ),// 18 �Զ��ع����С�ع�ʱ�� MinShutter
    VCT_3A_AE_MAXTIME		= (1<<VCT_3A_IDX_AE_MAXTIME     ),// 19 �Զ��ع������ع�ʱ�� MaxShutter
    VCT_3A_AE_SYSGAINMAX    = (1<<VCT_3A_IDX_AE_SYSGAINMAX  ),// 20 system gain max
    VCT_3A_AE_STRATEGY      = (1<<VCT_3A_IDX_AE_STRATEGY    ),// 21 ae strategy
    VCT_3A_AE_HISTOFFSET    = (1<<VCT_3A_IDX_AE_HISTOFFSET  ),// 22 ae histoffset
    VCT_3A_WDR_EXPO_RATIO     = (1<<VCT_3A_IDX_WDR_EXPO_RATIO),// 23 
    VCT_3A_WDR_EXPO_RATIO_MAX = (1<<VCT_3A_IDX_WDR_EXPO_RATIO_MAX),// 24
    VCT_3A_WDR_EXPO_RATIO_MIN = (1<<VCT_3A_IDX_WDR_EXPO_RATIO_MIN),// 25 
    VCT_3A_AE_COMPANSATION_LEVEL = (1<<VCT_3A_IDX_AE_COMPANSATION_LEVEL),// 26
    VCT_3A_IRIS_STOPPARAM	= (1<<VCT_3A_IDX_IRIS_STOPPARAM ),// 27 ���ù�Ȧֵֹͣ��
    VCT_3A_AF_MODE               = (1<<VCT_3A_IDX_AF_MODE   ),      // 28 0-AF 1-KF 2-MANUAL
    VCT_3A_AE_SYSGAINMIN         = (1<<VCT_3A_IDX_AE_SYSGAINMIN),   // 29 system gain min
    VCT_3A_AE_SLOW_SHUTTER       = (1<<VCT_3A_IDX_AE_SLOW_SHUTTER), // 30 
    VCT_3A_ALL		        = 0xFFFFFFFF,
}GSF_3A_MASK_E;

typedef struct _GSF_3A_PARA_S_
{
	GSF_U8	u8Value;			//����ֵ
	GSF_U8	u8Default;			//ȱʡ����ֵ
	GSF_U8	u8Step;				//����ֵ����ֵ
	GSF_U8	u8Res;
}GSF_3A_PARA_S;

typedef struct _GSF_3A_CFG_S_
{
	GSF_U32				u32Size;
/* ��Ч֧�ֵĲ������ò����ڻ�ȡ��ʱ����Ч�������ж��豸�Ƿ�֧�ָò��� */	
	GSF_3A_MASK_E		eSuppMask;
/* ������Ч�Ĳ������ò����������õ�ʱ����Ч������ָ���������õĲ��� */	
	GSF_3A_MASK_E		eSetMask;	
	GSF_3A_PARA_S		struParam[VCT_3A_IDX_BUTT];
}GSF_3A_CFG_S, *LPGSF_3A_CFG_S;


typedef struct _GSF_UPGRADE_STATE_S_
{
	GSF_U8	u8State; 	//0--����   1--����ʹ��  2---����дflash 3---�������
	GSF_U8	u8Percent;  //�ٷֱ�
	GSF_U8	u8Res[2];
}GSF_UPGRADE_STATE_S, *LPGSF_UPGRADE_STATE_S;


typedef int (*APP_CTRL_FUNC)();
typedef struct _APP_CTRL_S_
{
	char			*szCmd; 		// ���������ַ�
	APP_CTRL_FUNC	 func;			// ����ִ�нӿ�
	
}APP_CTRL_S, *LPAPP_CTRL_S;



#pragma pack(pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GSF_PARAM_H__ */
