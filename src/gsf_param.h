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

#define GSF_VMD_REG_NUM			    	4	//视频侦测区域数
#define GSF_MAX_SEG_NUM			    	4	//最大时间段
#define GSF_MAX_SUPPORT_CPU				4   //最大支持 CPU 数量
#define GSF_MAC_ADDR_LEN				6	//mac地址长度
#define GSF_USER_NAME_LEN				32	//用户名长度
#define GSF_PASS_WORD_LEN		    	32	//密码长度
#define GSF_MAX_SUPPORT_NETCARD        	2	//支持网卡个数
#define GSF_MAX_USR_EXT_NUM				16	// 扩展用户名数量
#define GSF_NAME_LEN					32	//名称长度
#define GSF_VOD_REG_NUM            		4	//遮挡告警区域数
#define GSF_VMASK_REG_NUM		    	4	//视频遮挡个数
#define GSF_OSD_NAME_LEN				128 // OSD 名称长度
#define GSF_OSD_EXT_NAME_LEN			124 // OSD 名称长度
#define GSF_IP_ADDR_LEN			    	64  //ip地址域名长度
#define GSF_PATH_LEN					64
#define GSF_MAX_EMAIL_LEN            	64
#define GSF_MAX_SUPPORT_CHN				4	//最大支持通道数
#define GSF_MAX_SUPPORT_DI		    	8	//最大支持报警输入个数
#define GSF_MAX_SUPPORT_GB28181_SERVER   4
#define GSF_ROI_REG_NUM            		4	//感兴趣编码区域数


typedef enum 
{
    GSF_GET_PARAM_MSG    = 0,
    GSF_SET_PARAM_MSG,

    GSF_GET_SYS_INFO,
    GSF_SET_SYS_INFO,

	GSF_SYS_STATE_MSG,
    GSF_SYS_CTRL_MSG,

	GSF_GET_PROTO_PARA,		// 暂不支持
	GSF_SET_PROTO_PARA,		// 暂不支持
	
    GSF_MAX_CMD_MSG
}DEVSDK_CMD_TYPE_E;

typedef enum
{
    GSF_TIME_INFO = 0, 		// 0 时间信息
    GSF_DISK_INFO,   		// 1 磁盘信息
    GSF_WIFI_AP_INFO,		// 2 WIFI AP Info
    GSF_NETWORK_STATUS, 	// 3 network status   
    GSF_DEV_ABILITY,    	// 4 设备能力集(2012-7-23)
    GSF_DEV_RUN_TIME,   	// 5设备运行时间(2012-09-19)
	GSF_ALARM_STATUS, 		// 6设备当前报警检测状态
	GSF_CHN_STATUS,			// 7设备通道状态 （编码状态等）
	GSF_LOG_INFO,           // 8 日志信息
	GSF_DOME_FIRMWARE,		// 9 球机固件版本(INT), len=sizeof(int)
	GSF_SOFTVER_DATETIME,	// 10 设备软件版本日期
	GSF_WIFI_LINKMODE,		// 11 WiFi连接模式
	GSF_PTZ_POS,			// 12
	GSF_CAM_POS,            // 13 
	GSF_RECORD_INFO,		// 14 录像记录
	GSF_SPORT_STATE,		// 15 GSF_SPORT_STATE_S;
	GSF_THIRD_PARTY_PF,		// 16 第三方平台配置
	GSF_MIC_STATUS,         // 17 麦克风静音状态
	GSF_TEMP_OSD_PARAM,     // 18 临时系统OSD
	GSF_RECORD_INFO_V2,		// 19 录像记录
	GSF_PTZ_R_POS,		    // 20 相对移动
	GSF_DOME_FIRMWARE_STR,  // 21 球机固件版本(CHAR *), len=GSF_DOME_FIRMWARE_LEN
	GSF_EXPOSURE_SYN_INFO,	// 22 同步曝光相关参数
	GSF_NET_IPV6_RA,        // 23 IPv6路由公告
	GSF_HISDK_VER_STR,      // 24 hisilion mpp version(CHAR *), len=GSF_HISDK_VERSION_LEN	
	GSF_PICTURE_INFO,		// 25 图片记录
	GSF_DYN_PARAM,			// 26 动态参数(AICFG)
    GSF_MAX_SYS_CMD,
}DEVSDK_SYS_INFO_CMD_E;

typedef enum 
{   	
    GSF_PTZ_CMD,     	 	// 0  PTZ云台控制
    GSF_DEV_CTRL, 		 	// 1  设备控制
	GSF_ALARM_OUT_CMD, 	 	// 2  报警输出控制，状态获取命令
	GSF_COMM_CMD,		 	// 3  写串口
	GSF_MANUAL_SNAP_CMD, 	// 4  让设备手动抓图,并保存到指定路径
	GSF_UPGRADE_CMD,	 	// 5  设备升级通知，升级状态获取
	GSF_FORMAT_CMD,         // 6  
	GSF_UPGRADE_PRE_CMD,	// 7  设备升级前处理通知
	GSF_PIX_CAL,            // 8
	GSF_SD_INSTALL,			// 9  加载SD卡
	GSF_SD_UNINSTALL,		// 10 卸载SD卡
	GSF_TWIN_ONE_ACTION,    // 11
	GSF_MANUAL_RECORD_CMD, 	// 12 手动启动录像、手动停止录像
	GSF_VIDEO_ISO_CMD,      // 13
	GSF_DELETE_FILE_CMD,    // 14 删除文件(录像、图片)
	GSF_ASS_LIFT_CTRL_CMD,  // 15 
	GSF_DB_LIFT_CTRL_CMD,   // 16 DB LIFT 控制命令
	GSF_DB_LIFT_STATE_CMD,  // 17 DB LIFT 状态
    GSF_CAM_EFFECT_CTRL_CMD,// 18 PUSHLOCK控制自动白平衡
    GSF_CAM_LENS_CTRL_CMD,  // 19 calibration控制命令
	GSF_CAM_DAYNIGHT_CTRL_CMD,// 20 
	GSF_CHECK_DISK_CMD,      // 21 
	GSF_IO_STATE_CMD,		//22 IO Status
	GSF_PARAM_OPERATE,
	GSF_BMP_UPLOAD,
	GSF_BMP_FILE_DELETE,
	GSF_TEMP_STATE_CMD,     // 26 温度
	GSF_PLAY_AUDIO_CMD,     // 27 播放声音
	GSF_DB_LIFT_CTRL_SPEC_CMD, // 28 
	GSF_EXP_INFO_CMD,       // 29 
	GSF_BMP_UPLOAD_STATE_CMD, // 30 osd bmp图片上传状态
    GSF_MAX_MACHINE_CMD
}DEVSDK_SYS_CTRL_CMD_E;

typedef enum 
{
	GSF_VER_PARAM    	= 1,	// 1 设备版本信息(只读) 			GSF_DEV_CFG_S
    GSF_SYS_PARAM		= 2,	// 2 系统基本信息					GSF_DEV_SYS_CFG_S
    GSF_NET_PARAM		= 3,    // 3 设备网络配置					GSF_NET_CFG_S
	GSF_OSD_PARAM		= 5,	// 5 设备视频osd参数				GSF_OSD_CFG_S
	GSF_VMASK_PARAM		= 6,	// 6 设备视频遮挡参数				GSF_VMASK_CFG_S
	GSF_VMD_PARAM		= 7,   	// 7 设备视频移动侦测参数			GSF_MD_CFG_S
    GSF_OD_PARAM		= 8,   	// 8 视频遮挡侦测参数				GSF_OD_CFG_S
	GSF_ALARMIN_PARAM	= 9,	// 9 报警输入参数					GSF_DI_CFG_S
	GSF_VMAIN_PARAM		= 10,	// 10 设备视频主码流参数			GSF_VENC_CFG_S
	GSF_VSUB_PARAM		= 11,  	// 11 设备视频次码流参数			GSF_VENC_CFG_S
    GSF_VTHREE_PARAM	= 12,  	// 12 设备第三码流参数 (如果支持)	GSF_VENC_CFG_S
	GSF_VCAP_PARAM		= 13,  	// 13 设备视频抓拍参数				GSF_CAPTURE_CFG_S
	GSF_IMA_PARAM		= 14,  	// 14 设备IMA图像参数				GSF_IMA_CFG_S
	GSF_3A_PARAM		= 15,  	// 15 设备3A图像参数				GSF_3A_CFG_S
	GSF_AUDIO_PARAM		= 16,	// 16 音频参数						GSF_AUDIO_CFG_S
	GSF_COMM_PARAM		= 17,  	// 17 串口信息						GSF_COMM_CFG_S
	GSF_FTP_PARAM		= 18,  	// 18 ftp参数配置					GSF_FTP_CFG_S
	GSF_EMAIL_PARAM		= 19,	// 19 email参数配置					GSF_SMTP_CFG_S
	GSF_DDNS_PARAM		= 20,  	// 20 ddns参数配置					GSF_DDNS_CFG_S
	GSF_UPNP_PARAM		= 21,  	// 21 upnp参数配置					GSF_UPNP_CFG_S
	GSF_NTP_PARAM		= 22,  	// 22 ntp参数配置					GSF_NTP_CFG_S
	GSF_REC_PARAM		= 24,	// 24 录像策略配置					GSF_REC_STRC_S
	GSF_ROI_PARAM		= 28,   // 28 感兴趣区域编码				GSF_ROI_CFG_S
	GSF_IRCUT_PARAM		= 33,	// 33 Ircut 切换模式设置			GSF_DAYNIGHT_CFG_S
	GSF_TIMER_CAP_PARAM	= 34,	// 34 定时抓拍参数					GSF_TIMER_CAP_S
	GSF_TIME_PARAM		= 44,	// 44 系统时间参数					GSF_TIME_S
	GSF_ALARMOUT_PARAM	= 51,	// 51 报警输出						GSF_DO_CFG_S
	GSF_USER_EXT_PARAM	= 54,	// 54 扩展用户信息					GSF_USER_EXT_CFG_S
	GSF_ONVIF_PARAM		= 59,	// 59 ONVIF参数						GSF_ONVIF_CFG_S
	GSF_GB28181_PARAM	= 64,	// 64 GB28181参数					GSF_GB28181_CFG_S
	GSF_AUTOREBOOT_PARAM = 65,	// 65 自动维护						GSF_AUTO_REBOOT_CFG_S
	GSF_PARAM_MAX_CMD,			// 最大参数命令 
}DEVSDK_CMD_PARAM_E;

typedef enum
{
	GSF_PROTOCOL_CUSTOM		= 0,    // 私有协议
	GSF_PROTOCOL_ONVIF	 	= 1,	// ONVIF协议
	GSF_PROTOCOL_G28181  	= 2,	// GB28181协议
	GSF_PROTOCOL_ZSYH		= 3,  	// 中盛益华平台协议
	GSF_PROTOCOL_HXHT		= 4,  	// 互信互通平台协议
	GSF_PROTOCOL_HW			= 5, 	// 华为平台协议	
	GSF_PROTOCOL_IST_NVR	= 6,
	GSF_PROTOCOL_IST_PF		= 7,
	GSF_PROTOCOL_HH  		= 8,
	GSF_PROTOCOL_GOOLINK    = 9,  	// 浪涛 p2p
	GSF_PROTOCOL_HP			= 10,	// 华平
	GSF_PROTOCOL_VV			= 11,	// 威威
	GSF_PROTOCOL_ULK		= 12,	// 悠络客
	GSF_PROTOCOL_DANA		= 13,	// 大拿
	GSF_PROTOCOL_QQ			= 14,	// QQ
	GSF_PROTOCOL_TUTK		= 15,	// TUTK
	GSF_PROTOCOL_HIK		= 16,	// 海康
	GSF_PROTOCOL_JXJ		= 17,   // 0x20000 131072 JXJ
	GSF_PROTOCOL_MOBILE_KD  = 18,	// 0x40000 262144 手机看店
	GSF_PROTOCOL_SICUNET    = 19,	// sicunet
	GSF_PROTOCOL_SIP_B      = 20,   // SIP B协议
	GSF_PROTOCOL_ZTE        = 21,   // 中兴
	GSF_PROTOCOL_CLOUD      = 22,   // reserved
	GSF_PROTOCOL_MTT		= 23,	// 
	GSF_PROTOCOL_AGBOX		= 24,   // agbox
	GSF_PROTOCOL_GI		    = 25,   // gi
	GSF_PROTOCOL_RTMP_PUB   = 26,   // RTMP PUBLISH
	GSF_PROTOCOL_MEDIA_SEND = 27,	// 定向发送
	GSF_PROTOCOL_WUHAN_UNIVERSITY= 28, // wu han university
	GSF_PROTOCOL_AIMI       = 29,   // aimi
	GSF_PROTOCOL_TUYA       = 30,   // 涂鸦 P2P
	GSF_PROTOCOL_BUTT
}DEVSDK_PROTOCOL_TYPE_E;


//eth 设置
typedef struct _GSF_ETHERNET_CFG_S_
{
    GSF_U32     u32Size;
    GSF_U32		u32IpAddr;						//ip地址
    GSF_U32		u32SubMask;						//子网掩码地址
    GSF_U32		u32GateWay;						//网关地址
    GSF_U8		u8MacAddr[GSF_MAC_ADDR_LEN];	//mac地址
    GSF_U8		u8DhcpOn;						//dhcp开关		1--开启 0--不开启
    GSF_U8		u8DhcpStatus;					//dhcp状态(只读) 0-没开启 1--正在获取 2--获取成功 3--获取失败
    GSF_U8		u8EtherStatu;					//网口状态(只读) 0-网口掉 1--网口正常(只能检测物理连接)
    GSF_U8      u8UseDev;                       //是否使用该网卡 0-不使用 1-使用
    GSF_U8		u8Res[10];
}GSF_ETHERNET_CFG_S, *LPGSF_ETHERNET_CFG_S;

//pppoe配置信息
typedef struct _GSF_PPPOE_CFG_S_
{
	GSF_U8		u8PppoeOn;							//pppoe开关
	GSF_U8		u8PppoeStatus;						//pppoe 拨号状态0 没有拨号1 拨号中2拨号成功
	GSF_U8      u8UseEth;                           //0使用有线网卡 1使用无线网卡
	GSF_U8		u8Res[5];
	GSF_CHAR	szPppoeUsr[GSF_USER_NAME_LEN];		//pppoe用户名
	GSF_CHAR	szPppoePsw[GSF_PASS_WORD_LEN];		//pppoe密码
	GSF_U32		u32PppoeIp;							//pppoe获取的ip
	GSF_U32		u32PppoeTimes;						//pppoe拨号时间
}GSF_PPPOE_CFG_S, *LPGSF_PPPOE_CFG_S;


//网络配置
typedef struct _GSF_NET_CFG_S_
{
  GSF_U32				u32Size;
  GSF_ETHERNET_CFG_S    struEtherCfg[GSF_MAX_SUPPORT_NETCARD];	//网卡信息	
  GSF_U32				u32AlarmHostIp;				//报警主机ip(预留)
  GSF_U16				u16AlarmHostPort;			//报警主机端口(预留)
  GSF_U8				u8DnsDhcp;					//dns是否自动获取
  GSF_U8				u8Rtsp2Auth;				//是否使能rtsp user  Authentication 
  GSF_U32				u32DnsIp1;					//dns1
  GSF_U32				u32DnsIp2;					//dns2
  GSF_U16				u16HttpPort;				//http服务端口
  GSF_U16				u16CmdPort;					//命令端口 
  GSF_U16               u16RtspPort;                //数据端口   
  GSF_U16               u16RtmpPort;                //RTMP端口    
  GSF_U32				u32MultiCastIp;				//多播地址
  GSF_U16				u16MultiCastPort;			//多播端口
  GSF_U16				u16IstPort;					//ist配置端口, 20210520开始用作 Hikvision协议端口
  GSF_U16				u16HHPort;					//hh 监听端口
  GSF_U16				u16Rtsp2Port;				//rtsp server 2 port
  GSF_PPPOE_CFG_S		struPppoeCfg;				//pppoe配置

  GSF_U16 				u16JxjCmdPort;				// jxj 命令端口
  GSF_U16 				u16JxjDataPort;				// jxj 数据端口
  GSF_U16 				u16JxjTalkPort;				// jxj 对讲端口
}GSF_NET_CFG_S,*LPGSF_NET_CFG_S;


typedef struct _GSF_USER_INFO_S_
{
	GSF_CHAR	szUsrName[GSF_USER_NAME_LEN];		//用户名
	GSF_CHAR	szPsw[GSF_PASS_WORD_LEN];			//密码
	GSF_U64		u64LocalRight;						//本地操作权限
	GSF_U64		u64RemoteRight;						//远程操作权限
	GSF_U8		u8Type;							//用户类型，0:管理员,1:操作员,2:普通用户
	GSF_U8      u8Encrypt;     // GSF_ENCRYPT_TYPE_E
	GSF_U16     u16Count;
	GSF_CHAR    szSalt[18]; // authType=2/3必须使用
	GSF_U8		u8Res[2];
	GSF_U32     u32LastModify; // Epoch
}GSF_USER_INFO_S, *LPGSF_USER_INFO_S;


//扩展用户信息结构参数 
//
typedef struct _GSF_USER_EXT_CFG_S_
{
	GSF_U32		u32Size;
	GSF_U32     u32Num;			//用户数
	GSF_USER_INFO_S 	stUsers[GSF_MAX_USR_EXT_NUM];
}GSF_USER_EXT_CFG_S, *LPGSF_USER_EXT_CFG_S;


//温度
typedef struct _GSF_TEMP_STATE_S
{
    int  nTemperature[GSF_MAX_SUPPORT_CPU];	 // 温度
}GSF_TEMP_STATE_S;


//设备版本信息(只读)
typedef struct _GSF_DEV_CFG_S_
{   
    GSF_U32     u32Size;
	GSF_U8		u8SerialId[DEVSDK_SERIALID_LEN];//设备Id
	GSF_U8		u8DiNum;						//报警输入路数
	GSF_U8		u8DoNum;						//报警输出路数
	GSF_U8		u8ChnNum;						//设备路数
	GSF_U8		u8Rs232Num;						//设备232 串口个数
	GSF_U8		u8Rs485Num;						//设备485 串口个数
	GSF_U8		u8NetCardNum;					//网卡个数
	GSF_U16		u16DevType ;					//设备类型						
	GSF_U32		u32HwVer;						//硬件版本号
	GSF_U32		u32CoreVer;						//内核版本号
	GSF_U32		u32SoftVer;						//软件版本号
	GSF_U32		u32SensorType[DEVSDK_CHN_NUM];	//前端sensor 类型
    GSF_U32     u32ChipType;	
    GSF_U32     u32DevFeature;                 	//设备特性 GSF_FEATURE_TYPE_E
    GSF_U32     u32IvsType;                 	//支持的智能分析类型，见GSF_IVS_TYPE_E
    GSF_U32     u32Protocol;                	//支持的网络协议
    GSF_CHAR    szProductName[DEVSDK_NAME_LEN];
}GSF_DEV_CFG_S, *LPGSF_DEV_CFG_S;

//区域结构
typedef struct _GSF_RECT_S
{
	GSF_S32		s32X;							//起点x坐标,基于(704, 576)坐标系
	GSF_S32		s32Y;							//起点y坐标,基于(704, 576)坐标系
	GSF_U32		u32Width;						//区域宽度,基于(704, 576)坐标系
	GSF_U32		u32Height;						//区域高度,基于(704, 576)坐标系
}GSF_RECT_S, *LPGSF_RECT_S;

//时间段结构
typedef struct _GSF_SEGMENT_S
{
	GSF_U32 	u8Open:8;						//开关
	GSF_U32		beginSec:24;					//开始时间秒数 3600*hour+60*min+sec
	GSF_U32		u8Rsvd:8;						//保留
	GSF_U32		endSec:24;						//结束时间秒数 3600*hour+60*min+sec
}GSF_SEGMENT_S, *LPGSF_SEGMENT_S;

//告警联动
typedef struct _GSF_LINKACT_S
{
	GSF_U32		u32ShootChannel;	//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
	GSF_U32		u32ShootInterval;	//抓拍间隔(毫秒),0:连续抓拍
	GSF_U32		u32RecTime;         //录像时间(报警停止后录像保持时间: 秒)
	GSF_U32		u32RecChannel;		//录像通道(bit) (0 不录像 1第一通道录像)
	GSF_U32		u32PCRecChannel;    //联动PC录像通道	
	GSF_U32		u32OutPut;			//联动输出(bit)
	GSF_U32		u32OutPersist;		//输出持续时间
	GSF_U8		u8ShootMode;		//联动抓拍模式,0本地存储，1ftp上传，2email
	GSF_U8		u8ShootNum;         //抓拍张数	
	GSF_U8		u8RecMode;			//联动录像模式，0本地存储，1ftp上传	
	GSF_U8		u8RecPreTime;		//预录时间(秒)
	GSF_U8		u8Email;            //联动email
	GSF_U8      u8BeepEnable;       //启用蜂鸣 0-无效   1-有效
    GSF_U8      u8BeepTime;         //蜂鸣时间(秒 , 0~255)
    GSF_U8      u8PtzAction;        //见 GSF_PTZ_JOIN_ACTION_TYPE
    GSF_U8      u8PtzNo;			//
	GSF_U8		u8ReportAlarmHost;  //上报到报警主机: 1-上报，0-不上报
	GSF_U8		u8Ftp;				// 联动 FTP
	GSF_U8      u8ShootDelay;       // 延迟抓拍: 秒
	GSF_U8		u8Rsvd[8];
}GSF_LINKACT_S, *LPGSF_LINKACT_S;

//移动侦测配置
typedef struct _GSF_MD_CFG_S_
{
  GSF_U32			u32Size;
  GSF_U8			u8Enable;			//是否开启移动侦测 0--关闭 1--开启			
  GSF_U8			u8Level;			//侦测灵敏度 0--很高 1--高 2--中 3--低 4--很低	
  GSF_U16           u16AlarmKeepTime; 	//报警持续时间(0 - 65535)s
  GSF_RECT_S		struRect[GSF_VMD_REG_NUM];		//侦测区域
  GSF_SEGMENT_S		struSeg[GSF_MAX_SEG_NUM][8];  	//0--星期天1--星期一 2--星期二7--每天	
  GSF_LINKACT_S		struLinkAct;					//移动侦测联动消息
  GSF_U8            u8AllDay[8];
  GSF_U8            u8Mask[GSF_VMD_REG_NUM];
  GSF_U8			u8Res2[4];
  GSF_RECT_S        struRect2[GSF_VMD_REG_NUM-1][GSF_VMD_REG_NUM];
}GSF_MD_CFG_S, *LPGSF_MD_CFG_S;

//点结构
typedef struct _GSF_POINT_S
{
	GSF_S32		s32X;							//起点x坐标
	GSF_S32		s32Y;							//起点y坐标
}GSF_POINT_S, *LPGSF_POINT_S;


typedef struct _GSF_DI_CFG_S_
{
  GSF_U32			u32Size;
  GSF_CHAR			chDiName[GSF_NAME_LEN];	//di探头名称
  GSF_U8			u8Enable;				//是否开启di报警检测 0--不开启 1--开启
  GSF_U8			u8DiType;				//0--常开探头1--常闭探头
  GSF_U16           u16AlarmKeepTime;       //报警持续时间(0 - 65535s)
  GSF_SEGMENT_S		struSeg[GSF_MAX_SEG_NUM][8];  //0--星期天1--星期一 2--星期二7--每天	
  GSF_LINKACT_S		struLinkAct;				//di报警联动消息
  GSF_U8            u8AllDay[8];            //allDay
  GSF_U8			u8Res2[8];
}GSF_DI_CFG_S, *LPGSF_DI_CFG_S;

//视频遮挡告警
typedef struct _GSF_OD_CFG_S_
{
  GSF_U32			u32Size;
  GSF_U8			u8Enable;		//是否开启 0--关闭 1--开启
  GSF_U8            u8Level;    	//灵敏度
  GSF_U16           u16AlarmKeepTime;//报警持续时间(0 - 65535s)
  GSF_RECT_S		struRect[GSF_VOD_REG_NUM];	//侦测区域
  GSF_SEGMENT_S		struSeg[GSF_MAX_SEG_NUM][8];//0--星期天1--星期一 2--星期二7--每天	
  GSF_LINKACT_S		struLinkAct;				//联动消息
  GSF_U8            u8AllDay[8];
  GSF_U8			u8Res2[8];
}GSF_OD_CFG_S, *LPGSF_OD_CFG_S;

//视频遮挡区域
typedef struct _GSF_VMASK_CFG_S_
{
    GSF_U32         u32Size;
    GSF_U8          u8Enable;	        //--1.启动视频遮挡 0--屏蔽视频遮挡
    GSF_U8          u8Res1[3];
    GSF_RECT_S      struArea[GSF_VMASK_REG_NUM]; //视频遮挡区域
    GSF_U32         u32Color;
    GSF_U8		    u8Res2[12];
}GSF_VMASK_CFG_S, *LPGSF_VMASK_CFG_S;


//osd 设置
typedef struct _GSF_OSD_CFG_S_
{
    GSF_U32     u32Size;
    GSF_U32     u32Color;
    GSF_U8      u8InvertColor;      	//反色设置 3518 才支持
    //=============时间
    GSF_U8		u8EnableTimeOsd;		//--1 显示时间osd 0--隐藏时间osd
    GSF_U8		u8DispWeek;				//--1 显示星期0--隐藏星期
    GSF_U8		u8EnableBitrateOsd;		//--1显示码流osd  0--隐藏码流osd
    GSF_U8		u8TimeOsdType;			//--时间显示类型
    									//--0 YYYY-MM-DD HH:MM:SS
    									//--1 MM-DD-YYYY HH:MM:SS
    									//--2 DD-MM-YYYY HH:MM:SS
    								    //--3 YYYY/MM/DD HH:MM:SS
    								    //--4 MM/DD/YYYY HH:MM:SS
    								    //--5 DD/MM/YYYY HH:MM:SS
    GSF_U8		u8HourOsdType;			//--0 24 hour --1 12 hour
	GSF_U8		u8OsdSize;				//0 小 1 中 2 大
	GSF_U8      u8OsdMode;				// 0-文本  1-bmp图片
    GSF_POINT_S struTimeOrg;			//时间osd位置
    //============字符osd
    GSF_U8		u8EnableTitleOsd;		//--1 显示标题osd 0--隐藏标题osd
    GSF_U8		u8EnableTemperature;	//--1 显示温度osd 0--隐藏温度osd
    GSF_U8		u8EnableAngle;			//--1 显示角度osd 0--隐藏角度osd
    GSF_U8      u8Res1;
    GSF_POINT_S struTitleOrg;			//标题osd位置
    GSF_CHAR	szChnName[GSF_OSD_NAME_LEN];	//通道名称 
    //============自定义osd
    GSF_U8		u8EnableUsrOsd; 		//bit0 使能第一个用户菜单
    									//bit1 使能第二个用户菜单
    GSF_U8		u8EnableBmpOsd; 		//bit0 使能第一个图片
    									//bit1 使能第二个图片
    GSF_U8		u8TextAlign;			//文本对齐 0-不对齐 1-左对齐 2-右对齐 3-边缘对齐
    GSF_U8		u8Res2;	  							
    GSF_POINT_S struUsrOrg[4];			//用户自定义osd位置
    GSF_CHAR	szUsrOsdInfo[4][GSF_OSD_EXT_NAME_LEN];//用户自定义osd信息
    GSF_U32     u32UsrColor[4];  //用户自定义颜色
    GSF_U8      u8DispZoom; // 一体机/ 球机显示倍数开关
    GSF_U8		u8EnableSteamOsd;//--0 全部码流显示osd,1-- 主码流显示 ，2-- 辅码流显示（除主码流之外的所有码流）
    GSF_U8		u8Res3[2];
    GSF_U32    u32TextColor;
}GSF_OSD_CFG_S, *LPGSF_OSD_CFG_S;


//设备系统配置结构体
typedef struct _GSF_DEV_SYS_CFG_S_
{
    GSF_U32     u32Size;
	GSF_CHAR	szDevName[GSF_NAME_LEN];  		//设备名称
	GSF_U8		u8Language;						//语言: 0-中文  1-英文  2-日本语 GSF_LANGUAGE_TYPE_E
	GSF_U8		u8PalStand;						//制式0-pal   1-ntsc
	GSF_U8      u8AutoChangeFR;               	//自动调整采集输入帧率
	GSF_U8		u8GSensorLevel;					// GSensor灵敏度, 0-4:  0-最高; 4-最低
	GSF_U8		u8VoMode;						//GSF_AV_VO_RES_TYPE_E
	GSF_U8		u8Res[15];
}GSF_DEV_SYS_CFG_S, *LPGSF_DEV_SYS_CFG_S;

//视频编码参数	
typedef struct _GSF_VENC_CFG_S_
{
    GSF_U32      u32Size;
    GSF_U16		u16Width;			// 视频编码宽度
    GSF_U16		u16Height;			// 视频编码高度

    GSF_U8		u8VideoEncType;		// 视频编码格式0--h264 1--MJPEG 2--JPEG	3--MPEG4  4--H265
    GSF_U8		u8PicQuilty;		// 编码质量0-最好，1-次好，2-较好，3-一般，4-较差，5-差	
    GSF_U8		u8Gop;				// I 帧间隔1-200
    GSF_U8		u8FlowType;			// 码率类型 0--定码流1--变码流2-- fix Qp
    GSF_U32		u32BitrateRate;		// 编码码率 32kbps-16Mkbps
    GSF_U8		u8FrameRate;		// 编码帧率1-25/30 ，分sensor，从av库中获取支持的帧率
    GSF_U8      u8Profile;  		// h264等级0--baseline   1--main profile     2--high profile
    GSF_U8      u8FramePriority;	// 是否帧率优先 1-是   0-否
    GSF_U8		u8HdmiEncAutoAdp;	// HDMI编码自适应
    GSF_U8		u8IQp;				// i frame qp value
	GSF_U8		u8PQp;				// p frame qp value
	GSF_U8		u8GopMode;			// GSF_VENC_GOPMODE_E: 0 NormalP  1 DualP 2 SmartP 
	GSF_U8		u8Res[1];
}GSF_VENC_CFG_S, *LPGSF_VENC_CFG_S;

//抓拍参数
typedef struct _GSF_CAPTURE_CFG_S_
{
    GSF_U32     u32Size;
    GSF_U16		u16Width;			//抓拍图片宽度
    GSF_U16		u16Height;			//抓拍图片高度

    GSF_U8		u8VideoEncType;		//视频编码格式 2--JPEG 固定值
    GSF_U8		u8Quilty;			//抓拍质量  [1, 99]

	GSF_U8      u8ValidQuality;		//用于兼容旧版本程序的质量值
    GSF_U8		u8Res[5];
}GSF_CAPTURE_CFG_S, *LPGSF_CAPTURE_CFG_S;

//音频配置结构体
typedef struct _GSF_AUDIO_CFG_S_
{
	GSF_U32		u32Size;
	GSF_U32		u32SampleRate;		//采样率 8k 8000
	GSF_U8		u8Open;				//是否开启音频
	GSF_U8		u8AEncType;	        //GSF_AENC_TYPE_E
	GSF_U8		u8Type;      		// 0--mic输入 1--线输入
	GSF_U8		u8InVolume;			// 输入音量  CDV_VER -> 0-100  
	GSF_U8		u8OutVolume;		// 输出音量  CDV_VER -> 0-100  
	GSF_U8		u8AudioTipsMask;	// 提示音掩码, 暂只支持on/off操作(暂无效: 按bit 操作:  GSF_AUDIO_TIPS_MASK_E)
	GSF_U8		u8AudioDenoise;	//音频环境降噪
	GSF_U8		u8Res[1];
}GSF_AUDIO_CFG_S, *LPGSF_AUDIO_CFG_S;

//串口配置结构体
typedef struct _GSF_COMM_CFG_S_
{
	GSF_U32		u32Size;
	GSF_U32		u32BaudRate;//波特率(bps)，0-50，1-75，2-110，3-150，4-300，
						//s5-600，6-1200，7-2400，8-4800，9-9600，
						//10-19200，11-38400，12-57600，13-115.2k 
 	GSF_U8     	u8DataBit;	//数据位：0-5位，1-6位，2-7位，3-8位
 	GSF_U8     	u8StopBit;	//停止位：0-1位，1-2位
  	GSF_U8     	u8Parity;	//是否校验：0-无校验，1-奇校验，2-偶校验
  	GSF_U8     	u8Flowcontrol;//是否流控：0-无，1-软流控,2-硬流控
  	GSF_U8 		u8WorkMode;		// 工作模式: 0 －透明通道, 1 - PTZ 模式(使用PTZ中的串口参数), 3 - 报警盒模式
  	GSF_U8		u8Res[7];
}GSF_COMM_CFG_S, *LPGSF_COMM_CFG_S;

//ftp参数设置
//
typedef struct _GSF_FTP_CFG_S
{
	GSF_U32		u32Size;
	GSF_CHAR 	szFtpSvr[GSF_IP_ADDR_LEN];     //主ftp服务器ip
	GSF_CHAR    szFtpUsr[GSF_USER_NAME_LEN];   //登陆用户名
	GSF_CHAR    szFtpPsw[GSF_PASS_WORD_LEN];   //登陆密码
	GSF_U16		u16FtpPort;
    GSF_U8      u8EnableAnony;   	//启用匿名：0- 否，1- 是
	GSF_U8		u8ReUpload;			// 上传失败重传开关
    GSF_U8      u8DirLevel;    		//0- 不使用目录结构，直接保存在根目录；1- 使用1级目录；2- 使用2级目录 
    GSF_U8      u8TopDirMode;  		//一级目录：0x1- 使用设备名，0x2- 使用设备号，0x3- 使用设备ip地址，
    								//0x4- 使用监测点，0x5- 使用时间(年月)，0x6- 使用自定义
    GSF_U8      u8SubDirMode;  		//二级目录：0x1- 使用通道名，0x2- 使用通道号，0x3- 使用时间(年月日)，
                               	 	//0x4- 使用车道号，0x5- 使用自定义
    GSF_U8		u8Rsvd[1];
    GSF_CHAR    szTopCustomDir[GSF_PATH_LEN]; //自定义一级目录
    GSF_CHAR    szSubCustomDir[GSF_PATH_LEN]; //自定义二级目录
    GSF_U8      u8EnableSSL; // !=0 启用 FTP Over SSL(FTPS) 
	GSF_U8      u8ConnKeep;   // !=0 链接保持, 即使用长链接: 1-NOOP
	GSF_U16     u16Keepalive; // 心跳间隔, u8ConnKeep不为 0 时有效
	GSF_U8		u8Rsvd1[24];
}GSF_FTP_CFG_S, *LPGSF_FTP_CFG_S;

//smtp设置
typedef struct _GSF_SMTP_CFG_S
{
	GSF_U32		u32Size;
	char        szMailHub[GSF_IP_ADDR_LEN];      //smtp 服务器地址
	char        szMailAddr[GSF_MAX_EMAIL_LEN];   //发送邮件 地址
	char        szMailUsr[GSF_MAX_EMAIL_LEN];    //发送邮件 账号
	char        szMailPsw[GSF_PASS_WORD_LEN];    //发送邮件 密码
	char        szMailRctp1[GSF_MAX_EMAIL_LEN];  //接受邮件 地址1
	char        szMailRctp2[GSF_MAX_EMAIL_LEN];  //接受邮件 地址2
	char       	szMailRctp3[GSF_MAX_EMAIL_LEN];  //接受邮件 地址3
	GSF_U8		u8EnableSSL;					//启用SSL
	GSF_U8		u8Rsvd;
	GSF_U16		u16MailPort;					//smtp 服务器端口
	GSF_U8      u8Rsvd2[32];
}GSF_SMTP_CFG_S, *LPGSF_SMTP_CFG_S;

//ddns 参数信息
typedef struct _GSF_DDNS_CFG_S
{
	GSF_U32		u32Size;
	GSF_U8		u8DDNSOpen;						//DDNS开关
	GSF_U8		u8DDNSType;						//0-dyndns 1-3322
	GSF_U16		u16DDNSPort;					//DDNS服务器端口
	GSF_CHAR	szDDNSAccount[GSF_NAME_LEN];	//DDNS 注册名
	GSF_CHAR	szDDNSUsr[GSF_USER_NAME_LEN];	//用户名称
	GSF_CHAR	szDDNSPsw[GSF_PASS_WORD_LEN];	//用户密码	
	GSF_U32		u32DDNSTimes;					//更新时间
	GSF_U8		u8Res[16];
}GSF_DDNS_CFG_S,*LPGSF_DDNS_CFG_S;

//upnp 参数结构
//
typedef struct _GSF_UPNP_CFG_S
{
	GSF_U32		u32Size;
	GSF_U8		u8UpnpOpen;		//upnp 开关
	GSF_U8		u8UpnpEthNo;	//0使用本机网卡映射 1 使用无线网卡映射
	GSF_U8		u8UpnpModel;    //0固定映射 1自动映射
	GSF_U8		u8UpnpRefTime;  //upnp 刷新时间(单位小时)
	GSF_U32		u32UpnpSvr;		//upnp 服务器IP
	GSF_U8		u8CmdPortEnable;
    GSF_U8		u8HttpPortEnable;
    GSF_U8		u8RtspPortEnable;
    GSF_U8		u8RtmpPortEnable;
    GSF_U16		u16UpnpCmdPort;//固定映射模式下, 数据端口的外网端口
	GSF_U16		u16UpnpHttpPort; //固定映射模式下, http端口的外网端口
	GSF_U16		u16UpnpRtspPort;//固定映射模式下, Rtsp端口的外网端口	  
	GSF_U16		u16UpnpRtmpPort;	//固定映射模式下, Rtmp端口的外网端口
	GSF_U16		u16UpnpCmdPortS; //数据端口的外网端口 0代表没有映射或不成功
	GSF_U16		u16UpnpHttpPortS; //Web端口的外网端口 0代表没有映射或不成功	
	GSF_U16		u16UpnpRtspPortS;//Rtsp端口的外网端口 0代表没有映射或不成功
	GSF_U16		u16UpnpRtmpPortS;//Rtmp端口的外网端口 0代表没有映射或不成功
	GSF_U32		u32UpnpWan;		//upnp 服务器外网IP，主机字节序
	GSF_U8		u8Res[4];
}GSF_UPNP_CFG_S, *LPGSF_UPNP_CFG_S;

typedef struct _GSF_NET_UPGRADE_PACK_S_
{
	GSF_U32      u32Seq;			// 包序号，从1 开始
	GSF_U32      u32PackType; 		// 0 第一包2 结束包1 其他升级包
    GSF_U32		 u32DataLen;		// 当前包数据长度u32DataLen <= sizeof(u8DataBuff);
	GSF_U8       u8DataBuff[1024];	// 包数据
}GSF_NET_UPGRADE_PACK_S, *LPGSF_NET_UPGRADE_PACK_S;


// 通过net server 升级
typedef struct _GSF_UPGRADE_CMD_S_
{
	GSF_U8 u8UpdType; 	// 0 指定本地升级文件升级, 1  网络升级数据包
	GSF_U8 u8Packs;     // 0 不分片, >= 1 分片数量
	GSF_U8 u8Res[2];
    union
    {
        char szFilePath[GSF_PATH_LEN];  
        GSF_NET_UPGRADE_PACK_S stUpdPack;
    };

    int (*funcFinishNotify)(int nSuccess);
}GSF_UPGRADE_CMD_S, *LPGSF_UPGRADE_CMD_S;



//ntp 参数结构设置
typedef struct _GSF_NTP_CFG_S
{
	GSF_U32		u32Size;
	GSF_U8		u8NtpOpen;				//ntp开关
	GSF_U8		u8Resv;
	GSF_U8		u8EuroTime;				//==1:启动夏令时 0:不启动夏令时
	GSF_U8		u8NtpRefTime;			//ntp 定时对时时间(单位小时)
	int			nTimeZone;				//时区 单位s(-12*3600~11*3600)
	GSF_CHAR    szNtpServer[GSF_IP_ADDR_LEN];
	GSF_U8      u8WebTimeDisableSync;   //是否禁用IE登陆成功后自动对时: 0-不禁用; 1-禁用
	GSF_U8      u8Res1[2];
	GSF_U8      u8NtpRefTimeSecSupp;
	GSF_U32     u32NtpRefTimeSec;      // ntp 定时对时时间(单位秒)
	GSF_U8		u8Res2[8];
}GSF_NTP_CFG_S, *LPGSF_NTP_CFG_S;

typedef struct _GSF_REC_STRC_S_
{
	GSF_U32		u32Size;		
	GSF_U8		u8OverWrite;		//0 不覆盖1 覆盖
	GSF_U8		u8RecStream;		//0 主码流录像1 次码流录像
	GSF_U8		u8PreRecSecs;		//预录时间(秒)
	GSF_U8		u8SaveMode;		//保存模式
	//定时录像策略
	GSF_SEGMENT_S	struTimerSeg[GSF_MAX_SEG_NUM][8]; //0--星期天1--星期一 2--星期二7--每天	
	GSF_U8       u8AllDay[8];      // 
	GSF_U8		 u8EnableTime;
	//
	GSF_U8		 u8RecStreamType;	//0 视频流 1 复合流
	GSF_U8		 u8RecPackTime;		// 录像打包时间(分钟)
	GSF_U8		 u8Res;
}GSF_REC_STRC_S, *LPGSF_REC_STRC_S;

typedef struct _GSF_TIMER_CAP_S_
{
	GSF_U32		u32Size;
    GSF_U8      u8Enable;		// 0 禁用，1启用
	GSF_U8		u8SaveMode;		// 保存模式0本地存储，1ftp上传，2 email
	GSF_U16     u16CapIntv;		// 抓拍间隔
	//定时抓拍策略
	GSF_SEGMENT_S	struTimerSeg[GSF_MAX_SEG_NUM][8]; //0--星期天1--星期一 2--星期二7--每天	
	GSF_U8          u8AllDay[8];      // 
	GSF_U8          u8Quality;  // 抓拍质量值: 1-100
    GSF_U8          u8Ftp;
    GSF_U8          u8Res[2];
}GSF_TIMER_CAP_S, *LPGSF_TIMER_CAP_S;

//时间结构
typedef struct _GSF_TIME_S
{
	GSF_U8		u8Year;							//年(从1900年开始)
	GSF_U8		u8Month;						//月1~12
	GSF_U8		u8Day;							//日1~31
	GSF_U8		u8WDay;							//星期天0 星期一1.....
	GSF_U8		u8Hour;							//时0~23
	GSF_U8		u8Minute;						//分0~59
	GSF_U8		u8Second;						//秒0~59
	GSF_U8		u8Millisec;						//毫妙(0~99)(百分之一秒)
	GSF_S32		s32Zone;						// timezone  second
}GSF_TIME_S, *LPGSF_TIME_S;

typedef struct _GSF_DO_CFG_S_
{
  GSF_U32			u32Size;
  GSF_CHAR			chDoName[GSF_NAME_LEN];	//报警输出名称
  GSF_U8			u8Enable;				//是否开启do报警输出0--不开启 1--开启
  GSF_U8			u8Res1[3];
  GSF_U32	        u32AlarmTime;        	//报警输出时间(0 - 60s)
  GSF_SEGMENT_S		struSeg[GSF_MAX_SEG_NUM][8];  //0--星期天1--星期一 2--星期二7--每天	
  GSF_U8            u8AllDay[8];            //allDay
  GSF_U8			u8Res2[8];
}GSF_DO_CFG_S, *LPGSF_DO_CFG_S;

typedef struct _GSF_ONVIF_CFG_S_
{
	GSF_U32			u32Size;
	GSF_U8			u8Auth;           // 是否使能ONVIF  Authentication 
	GSF_U8 			u8RtspAuth;       // 是否使能rtsp user  Authentication 
	GSF_U16			u16Port;          // default  8000
	GSF_U16			u16RtspPort;      // default  8554
	GSF_U8          u8TZType;         // default  CST, GSF_ONVIF_TZ_TYPE_E: 20180714
	GSF_U8			u8Res2[21];
}GSF_ONVIF_CFG_S;

// GB28181 平台参数v2
typedef struct _GSF_GB28181_INFO_S
{
	GSF_CHAR 		szDeviceID[32]; // 设备ID
	GSF_CHAR 		szDevicePwd[32];// 设备密码 
	GSF_CHAR 		szServerID[32]; // 服务器ID
	GSF_CHAR 		szServerIp[32];	// 服务器地址
	GSF_U16         u16ServerPort;  // 服务器端口
	GSF_U16         u16MsgPort; 	// 本地消息端口    

	GSF_U8          u8Enable;       // 启用配置
	GSF_U8          u8DefStream;    // 默认码流(主=0, 次=1, 三=2)
	GSF_U8			u8ProtocolType; // 0 -- udp  1---tcp
	GSF_U8 	        u8Parental;     // 是否有子设备

	GSF_U32         u32Expires;       // 注册有效期(s)
	GSF_U32			u32RegInterval;	  // 注册间隔(s)
	GSF_U32         u32Heartbeat;     // 心跳周期(s)
	GSF_U32         u32TimeOutCnt;    // 心跳超时次数, 超过次数后重新注册
	GSF_U32         u32SecrectAttr;   // 保密属性
	
	GSF_CHAR        szMediaChID[GSF_MAX_SUPPORT_CHN][32];  // 媒体通道ID
	GSF_CHAR        szAlarmChID[GSF_MAX_SUPPORT_DI][32];   // 报警通道ID
	GSF_CHAR        szOwner[32];      // 设备归属
	GSF_CHAR        szMaker[32];      // 设备制造商
	GSF_CHAR        szCivilCode[32];  // 设备所属行政区域码
	GSF_CHAR        szParentID[32];   // 父设备/区域/系统ID
	GSF_CHAR        szInstallAddr[32];// 设备安装地址
	GSF_S32         s32LatiTude;      // 纬度 放大1000000倍的浮点数: 80765432  -> 80.765432
	GSF_S32         s32LongtiTude;    // 经度 放大1000000倍的浮点数: 100234567 -> 100.234567
	GSF_CHAR        szBlock[32];      // 设备所属警区
}GSF_GB28181_INFO_S;

typedef struct _GSF_GB28181_CFG_S
{
	GSF_U32         u32Size;
	GSF_GB28181_INFO_S stGbCfgs[GSF_MAX_SUPPORT_GB28181_SERVER];
	GSF_S32         nStatus[GSF_MAX_SUPPORT_GB28181_SERVER];  // 注册状态
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
	GSF_U16		u16ZoomPos;		//镜头位置(0-16384)
	GSF_U16		u16FocusPos;	//聚焦位置(0-16384)
	GSF_U16 	u16IrisPos;		//光圈位置(40-900)
	GSF_U16 	u16Res;
}GSF_CAM_POS_INFO;

typedef struct _GSF_ROI_CFG_S_	//感兴趣区域编码
{
    GSF_U32      u32Size;
    GSF_U8       u8Enable;          //使能1:开启  0: 不开启
    GSF_U8       u8AbsQp;           //0-相对    1-绝对质量
    GSF_U8       u8Res[2];
    GSF_U32      s32QpVal;          //质量值
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

// 报警输出状态、设置报警输出结构体
typedef struct _GSF_ALARMOUT_CTRL_S_
{
    GSF_U32     u32SetManl;			// 1 manual control	手动 0 alarm link	报警链接
    GSF_U32     u32ManlStatus;		// u32SetManl = 1, 0 open, 1 close		手动设置状态
    GSF_U32		u32AlarmTimes;		// u32SetManl = 0, alarm link alarm out time		报警链接报警输出时间 
}GSF_ALARMOUT_CTRL_S, *LPGSF_ALARMOUT_CTRL_S;


//日夜切换参数
//-------------------------------------------------------------------
typedef struct _GSF_DAYNIGHT_CFG_S
{
    GSF_U32      u32Size;
    GSF_U8       u8FilterType;		 //GSF_IR_FILTER_TYPE_E, 0-白天，1-夜晚，2-自动，3-定时，4-报警输入触发  5-ispLum触发
    GSF_U8       u8FilterLevel;        //自动切换时 0 低电平切换 1 高电平切换; TWHQ night-input 
    GSF_U8       u8AlarmTrigState;     //报警输入触发 报警输入触发状态：0-白天，1-夜晚
    GSF_U8       u8FilterSignal;		 // IRCUT信号线序: 0-正序 ;  1-反序
    GSF_SEGMENT_S Segs[GSF_MAX_SEG_NUM]; //定时切换时间段
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
	GSF_U8       u8LampType;     // 0-红外灯  1-白光灯(LED)
	GSF_U8       u8LampCtrl;     // 0-自动    1-常开   2-常关
	GSF_U8       u8Sensitive;    // 灵敏度    1-10, 默认 5 
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

//场景定义
typedef enum _GSF_SCENE_E_
{
      VCT_SCENE_DAY   = 0
    , VCT_SCENE_NIGHT = 1
    , VCT_SCENE_BUTT
}GSF_SCENE_E;

//IMAGE参数序号定义
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


//视频参数设置类型定义
typedef enum _GSF_IMA_MASK_E_
{
    VCT_IMA_BRIGHTNESS 	         = (1<<VCT_IMA_IDX_BRIGHTNESS            ) ,// 0 亮度
    VCT_IMA_CONTRAST             = (1<<VCT_IMA_IDX_CONTRAST              ) ,// 1 对比度
    VCT_IMA_SATURATION 	         = (1<<VCT_IMA_IDX_SATURATION 	         ) ,// 2 饱和度
    VCT_IMA_HUE		  	         = (1<<VCT_IMA_IDX_HUE		  	         ) ,// 3 色调
    VCT_IMA_GAMMA		         = (1<<VCT_IMA_IDX_GAMMA		         ) ,// 4 GAMMA值
    VCT_IMA_SHARPNESS	         = (1<<VCT_IMA_IDX_SHARPNESS	         ) ,// 5 锐度
    VCT_IMA_MIRROR		         = (1<<VCT_IMA_IDX_MIRROR		         ) ,// 6 镜向
    VCT_IMA_FLIP		         = (1<<VCT_IMA_IDX_FLIP		             ) ,// 7 翻转
    VCT_IMA_MODE	         	 = (1<<VCT_IMA_IDX_MODE	                 ) ,// 8 自动翻转
    VCT_IMA_PWDFREQ	             = (1<<VCT_IMA_IDX_PWDFREQ	             ) ,// 9 照明频率1   
    VCT_IMA_DIS                  = (1<<VCT_IMA_IDX_DIS                   ) ,// 10 数字防抖
    VCT_IMA_2DENOISE             = (1<<VCT_IMA_IDX_2DENOISE              ) ,// 11 2D降噪
    VCT_IMA_3DNLEVEL             = (1<<VCT_IMA_IDX_3DNLEVEL              ) ,// 12 3D降噪等级，0 关闭 1-5 强度越来越强
    VCT_IMA_HLC                  = (1<<VCT_IMA_IDX_HLC                   ) ,// 13 强光抑制
    VCT_IMA_3DNSFINCSTH          = (1<<VCT_IMA_IDX_3DNSFINCSTH           ) ,// 14 3D降噪空域去噪强度2
    VCT_IMA_3DNTFSTH             = (1<<VCT_IMA_IDX_3DNTFSTH              ) ,// 15 3D降噪时域去噪强度
    VCT_IMA_SHARPEN              = (1<<VCT_IMA_IDX_SHARPEN               ) ,// 16 图象锐化
    VCT_IMA_DYNBADPIX            = (1<<VCT_IMA_IDX_DYNBADPIX             ) ,// 17 动态坏点校正
    VCT_IMA_3DNTFLIMIT           = (1<<VCT_IMA_IDX_3DNTFLIMIT            ) ,// 18 3D降噪时域门限值 
    VCT_IMA_LDC_STRENGH          = (1<<VCT_IMA_IDX_LDC_STRENGH           ) ,// 19 镜头畸变矫正强度
    VCT_IMA_ANTI_FALSECOLOR_TYPE = (1<<VCT_IMA_IDX_ANTI_FALSECOLOR_TYPE  ) ,// 20 去伪彩
    VCT_IMA_ANTI_FOG_TYPE        = (1<<VCT_IMA_IDX_ANTI_FOG_TYPE         ) ,// 21 去雾
    VCT_IMA_ROTATE_TYPE          = (1<<VCT_IMA_IDX_ROTATE_TYPE           ) ,// 22 旋转 GSF_ROTATE_TYPE_E
    VCT_IMA_ANTI_FOG_STRENGTH    = (1<<VCT_IMA_IDX_ANTI_FOG_STRENGTH     ) ,// 23 去雾强度==2D降噪
    VCT_IMA_3DCHROMARANGE        = (1<<VCT_IMA_IDX_3DCHROMARANGE         ) ,// 24 3D降噪色度降噪强度
    VCT_IMA_GAMMA_CURVE          = (1<<VCT_IMA_IDX_GAMMA_CURVE           ) ,// 25 GAMMA曲线类型
    VCT_IMA_SHADING_TYPE         = (1<<VCT_IMA_IDX_SHADING_TYPE          ) ,// 26阴影校正
    VCT_IMA_BLC				     = (1<<VCT_IMA_IDX_BLC			         ) ,// 27 背光补偿
    VCT_IMA_FROZEN			     = (1<<VCT_IMA_IDX_FROZEN                ) ,// 28 视频冻结
    VCT_IMA_SHARPEN_MANUAL       = (1<<VCT_IMA_IDX_SHARPEN_MANUAL        ) ,// 29 手动图像锐化
    VCT_IMA_DIS_VI               = (1<<VCT_IMA_IDX_DIS_VI                ) ,// 30 VI数字防抖
    VCT_IMA_OIS                  = (1<<VCT_IMA_IDX_OIS                   ) ,// 31光学防抖
    VCT_IMA_ALL			         = 0xFFFFFFFF,
}GSF_IMA_MASK_E;


//图像参数设置
typedef struct _GSF_IMA_PARA_S_
{
	GSF_U8	u8Value;			//参数值
	GSF_U8	u8Default;			//缺省参数值，只读
	GSF_U8	u8Step;				//参数值步进值，只读
	GSF_U8	u8Res;
}GSF_IMA_PARA_S;

typedef struct _GSF_IMA_CFG_S_
{	
    GSF_U32              u32Size;
/* 有效支持的参数，该参数在获取的时候有效，用以判断设备是否支持该参数 */
	GSF_IMA_MASK_E	eSuppMask;
/* 设置有效的参数，该参数在设置的时候有效，用以指定具体设置的参数 */
	GSF_IMA_MASK_E	eSetMask;	
	GSF_IMA_PARA_S	struParam[VCT_IMA_IDX_BUTT]; 
}GSF_IMA_CFG_S, *LPGSF_IMA_CFG_S;

typedef enum _GSF_IMA_EXT_IDX_E_
{
      VCT_IMA_EXT_IDX_COLOR_TONE_RED   = 0       // 0 色调红色增益
    , VCT_IMA_EXT_IDX_COLOR_TONE_GREEN           // 1 色调绿色增益
    , VCT_IMA_EXT_IDX_COLOR_TONE_BLUE 	         // 2 色调蓝色增益
    , VCT_IMA_EXT_IDX_SCENCE_MODE				//  3
    , VCT_IMA_EXT_IDX_BUTT = 32
}GSF_IMA_IDX_EXT_E;


typedef enum _GSF_IMA_EXT_MASK_E_
{
    VCT_IMA_EXT_COLOR_TONE_RED 	 = (1<<VCT_IMA_EXT_IDX_COLOR_TONE_RED ) , // 色调红色增益
    VCT_IMA_EXT_COLOR_TONE_GREEN = (1<<VCT_IMA_EXT_IDX_COLOR_TONE_GREEN), // 色调绿色增益
    VCT_IMA_EXT_COLOR_TONE_BLUE  = (1<<VCT_IMA_EXT_IDX_COLOR_TONE_BLUE),  // 色调蓝色增益
    VCT_IMA_EXT_ALL			         = 0xFFFFFFFF,
}GSF_IMA_EXT_MASK_E;

typedef struct _GSF_IMA_EXT_PARA_S_
{
	GSF_U8	u8Value;			//参数值
	GSF_U8	u8Default;			//缺省参数值，只读
	GSF_U8	u8Step;				//参数值步进值，只读
	GSF_U8	u8Res;
}GSF_IMA_EXT_PARA_S;

typedef struct _GSF_IMA_EXT_CFG_S_
{
    GSF_U32              u32Size;
/* 有效支持的参数，该参数在获取的时候有效，用以判断设备是否支持该参数 */
	GSF_IMA_EXT_MASK_E	eSuppMask;
/* 设置有效的参数，该参数在设置的时候有效，用以指定具体设置的参数 */
	GSF_IMA_EXT_MASK_E	eSetMask;
	GSF_IMA_EXT_PARA_S	struParam[VCT_IMA_EXT_IDX_BUTT]; 
}GSF_IMA_EXT_CFG_S, *LPGSF_IMA_EXT_CFG_S;

//==================== 3A设置 ====================== //
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


//--3A参数设置类型定义
typedef enum _GSF_3A_MASK_E_
{
    VCT_3A_WDR_STRENGH      = (1<<VCT_3A_IDX_WDR_STRENGH    ),// 0 宽动态强度
    VCT_3A_IRIS_TYPE        = (1<<VCT_3A_IDX_IRIS_TYPE      ),// 1 设置光圈类型  //0 自动光圈 1 手动光圈
    VCT_3A_IRIS_PARAM       = (1<<VCT_3A_IDX_IRIS_PARAM     ),// 2 设置光圈校正值，
    VCT_3A_AE               = (1<<VCT_3A_IDX_AE             ),// 3 自动曝光       //0: 自动曝光， 1~255: 手动曝光时间
    VCT_3A_AF               = (1<<VCT_3A_IDX_AF             ),// 4 自动聚焦
    VCT_3A_AWB              = (1<<VCT_3A_IDX_AWB            ),// 5 自动白平衡
    VCT_3A_AWBRED           = (1<<VCT_3A_IDX_AWBRED         ),// 6 自动白平衡RED
    VCT_3A_AWBBLUE          = (1<<VCT_3A_IDX_AWBBLUE        ),// 7 自动白平衡BLUE
    VCT_3A_COLOUR           = (1<<VCT_3A_IDX_COLOUR         ),// 8 彩转黑
    VCT_3A_AE_MaxAG         = (1<<VCT_3A_IDX_AE_MaxAG       ),// 9 自动曝光最大模拟增益
    VCT_3A_AE_MaxDG         = (1<<VCT_3A_IDX_AE_MaxDG       ),// 10 自动曝光最大数字增益  
    VCT_3A_WDR_MinSlope     = (1<<VCT_3A_IDX_WDR_MinSlope   ),// 11 设置宽动态最小增益
    VCT_3A_WDR_MaxSlope     = (1<<VCT_3A_IDX_WDR_MaxSlope   ),// 12 设置宽动态最大增益
    VCT_3A_AE_Compansation  = (1<<VCT_3A_IDX_AE_Compansation),// 13 自动曝光期望值
    VCT_3A_AE_MODE          = (1<<VCT_3A_IDX_AE_MODE        ),// 14 自动曝光类型
    VCT_3A_SHUTTER          = (1<<VCT_3A_IDX_SHUTTER        ),// 15 设置电子快门
    VCT_3A_AE_STEP          = (1<<VCT_3A_IDX_AE_STEP        ),// 16 自动曝光收敛速度
    VCT_3A_WDR_MODE			= (1<<VCT_3A_IDX_WDR_MODE       ),// 17 宽动态模式 GSF_3A_WDR_MODE_E
    VCT_3A_AE_MINTIME		= (1<<VCT_3A_IDX_AE_MINTIME     ),// 18 自动曝光的最小曝光时间 MinShutter
    VCT_3A_AE_MAXTIME		= (1<<VCT_3A_IDX_AE_MAXTIME     ),// 19 自动曝光的最大曝光时间 MaxShutter
    VCT_3A_AE_SYSGAINMAX    = (1<<VCT_3A_IDX_AE_SYSGAINMAX  ),// 20 system gain max
    VCT_3A_AE_STRATEGY      = (1<<VCT_3A_IDX_AE_STRATEGY    ),// 21 ae strategy
    VCT_3A_AE_HISTOFFSET    = (1<<VCT_3A_IDX_AE_HISTOFFSET  ),// 22 ae histoffset
    VCT_3A_WDR_EXPO_RATIO     = (1<<VCT_3A_IDX_WDR_EXPO_RATIO),// 23 
    VCT_3A_WDR_EXPO_RATIO_MAX = (1<<VCT_3A_IDX_WDR_EXPO_RATIO_MAX),// 24
    VCT_3A_WDR_EXPO_RATIO_MIN = (1<<VCT_3A_IDX_WDR_EXPO_RATIO_MIN),// 25 
    VCT_3A_AE_COMPANSATION_LEVEL = (1<<VCT_3A_IDX_AE_COMPANSATION_LEVEL),// 26
    VCT_3A_IRIS_STOPPARAM	= (1<<VCT_3A_IDX_IRIS_STOPPARAM ),// 27 设置光圈停止值，
    VCT_3A_AF_MODE               = (1<<VCT_3A_IDX_AF_MODE   ),      // 28 0-AF 1-KF 2-MANUAL
    VCT_3A_AE_SYSGAINMIN         = (1<<VCT_3A_IDX_AE_SYSGAINMIN),   // 29 system gain min
    VCT_3A_AE_SLOW_SHUTTER       = (1<<VCT_3A_IDX_AE_SLOW_SHUTTER), // 30 
    VCT_3A_ALL		        = 0xFFFFFFFF,
}GSF_3A_MASK_E;

typedef struct _GSF_3A_PARA_S_
{
	GSF_U8	u8Value;			//参数值
	GSF_U8	u8Default;			//缺省参数值
	GSF_U8	u8Step;				//参数值步进值
	GSF_U8	u8Res;
}GSF_3A_PARA_S;

typedef struct _GSF_3A_CFG_S_
{
	GSF_U32				u32Size;
/* 有效支持的参数，该参数在获取的时候有效，用以判断设备是否支持该参数 */	
	GSF_3A_MASK_E		eSuppMask;
/* 设置有效的参数，该参数获在设置的时候有效，用以指定具体设置的参数 */	
	GSF_3A_MASK_E		eSetMask;	
	GSF_3A_PARA_S		struParam[VCT_3A_IDX_BUTT];
}GSF_3A_CFG_S, *LPGSF_3A_CFG_S;


typedef struct _GSF_UPGRADE_STATE_S_
{
	GSF_U8	u8State; 	//0--空闲   1--正在使用  2---正在写flash 3---升级完成
	GSF_U8	u8Percent;  //百分比
	GSF_U8	u8Res[2];
}GSF_UPGRADE_STATE_S, *LPGSF_UPGRADE_STATE_S;


typedef int (*APP_CTRL_FUNC)();
typedef struct _APP_CTRL_S_
{
	char			*szCmd; 		// 控制命令字符
	APP_CTRL_FUNC	 func;			// 控制执行接口
	
}APP_CTRL_S, *LPAPP_CTRL_S;



#pragma pack(pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GSF_PARAM_H__ */
