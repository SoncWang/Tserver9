/*******************************************************************************
 Copyright (c) 2008-2009, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_sdk_pub.h
  Project Code: MW_SDK
   Module Name: SDK
  Date Created: 2012-01-14
        Author:  
   Description: 该文件定义了sdk相关的枚举、结构体

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  2012-01-14

*******************************************************************************/
#ifndef __IMOS_SDK_PUB_H__
#define __IMOS_SDK_PUB_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/**
 * @struct tagMwPoint
 * @brief 点坐标
 * @attention 无
 */
typedef struct tagMwPoint
{
    ULONG ulX;        /**< 横坐标 */
    ULONG ulY;        /**< 纵坐标 */
}IMOS_MW_POINT_S;

/**
 * @struct tagMwPoint
 * @brief 线坐标结构
 * @attention 无
 */
typedef struct tagMwLine
{
    IMOS_MW_POINT_S stHead;       /**< 头坐标 */
    IMOS_MW_POINT_S stTail;       /**< 尾坐标 */
}IMOS_MW_LINE_S;

/**
 * @struct tagMwRectangle
 * @brief 矩形框坐标结构
 * @attention 无
 */
typedef struct tagMwRectangle
{
    IMOS_MW_POINT_S stTopLeft;       /**< 左上角坐标 */
    IMOS_MW_POINT_S stBotRight;      /**< 右下角坐标 */
}IMOS_MW_RECTANGLE_S;

/**
 * @struct tagMwQuadrangle
 * @brief 四方形框坐标结构
 * @attention 无
 */
typedef struct tagMwQuadrangle
{
    IMOS_MW_POINT_S stTopLeft;       /**< 左上角坐标 */
    IMOS_MW_POINT_S stTopRight;      /**< 右上角坐标 */
    IMOS_MW_POINT_S stBotLeft;       /**< 左下角坐标 */
    IMOS_MW_POINT_S stBotRight;      /**< 右下角坐标 */
}IMOS_MW_QUADRANGLE_S;

/**
* @struct tagMwAddr
* @brief 地址结构
* @attention 
*/
typedef struct tagMwAddr
{
    CHAR szIPAddr[IMOS_MW_IPADDR_LEN];       /**< IP地址 */
    USHORT usPort;                           /**< 端口 */
    USHORT usReserve;                        /**< 保留 */
}IMOS_MW_ADDR_S;

#if 0
#endif 
/******************************* 系统、设备维护相关结构 ***********************************/
/**
 * @struct tagMwPlayerStatus
 * @brief 播放器状态
 * @attention 无
 */ 
typedef struct tagMwPlayerStatus
{
    ULONG ulStreamHandle;           /**< 流句柄 */
    ULONG ulErrCode;                /**< 错误码 */
}IMOS_MW_PLAYER_STATUS_S;


/**
 * @struct tagMwTime
 * @brief 时间信息
 * @attention 
 */
typedef struct tagMwTime
{
    USHORT usYear;           /**< 年，当前年数（如2009年时，该值为2009） */
    USHORT usMonth;          /**< 月，当前月份（1－12） */
    USHORT usMonthDay;       /**< 日，每月的几号（1－31） */
    USHORT usHour;           /**< 时，当前小时数（0－23） */
    USHORT usMinute;         /**< 分，当前分钟数（0－59） */
    USHORT usSecond;         /**< 秒，当前秒数（0－60） */
    USHORT usWeekday;        /**< 周几，每周的星期几（0－6），0 对应周日，1-6 对应周一 至 周六 ， 暂未使用 */
    USHORT usReserve;        /**< 保留 */
}IMOS_MW_TIME_S;

/**
 * @struct tagMwSystemTimeInfo
 * @brief 系统(本地)时间
 * @attention  lTimeZone为 本地时区与零时区的差，例如 北京时间UTC+8,则填写 8
 */
typedef struct tagMwSystemTimeInfo
{
    LONG lTimeZone;                   /**< 设置系统时区, -12到12，对应和标准时间的偏差 */
    IMOS_MW_TIME_S stLocalTime;       /**< 本地时间 */
}IMOS_MW_SYSTEM_TIME_INFO_S;
/**
 * @struct tagMwUserInfo
 * @brief 用户信息
 * @attention 删除用户信息时，只需传入用户名即可
 */
typedef struct tagMwUserInfo
{
    CHAR szUserName[IMOS_MW_USER_NAME_LEN + 4];           /**< 用户名 */
    CHAR szPassword[IMOS_MW_USER_PASSWORD_LEN + 4];       /**< 用户密码 */
    ULONG ulLevel;                                        /**< 用户级别, 参见: IMOS_MW_USERLEVEL_ADMINISTRATOR 等 */
}IMOS_MW_USER_INFO_S;


/**
 * @struct tagMwBasicDeviceInfo
 * @brief 设备基本信息
 * @attention 无
 */
typedef struct tagMwBasicDeviceInfo
{
    CHAR szManufacturer[IMOS_MW_VERSION_LEN];      /**< 厂商 */  
    CHAR szDeviceModel[IMOS_MW_VERSION_LEN];       /**< 设备类型 */  
    CHAR szSerialNumber[IMOS_MW_VERSION_LEN];      /**< 设备序列号 */  
    CHAR szDeviceMAC[IMOS_MW_VERSION_LEN];         /**< 设备MAC地址 */
    CHAR szFirmwareVersion[IMOS_MW_VERSION_LEN];   /**< 软件版本, program 版本 */  
    CHAR szHardwareID[IMOS_MW_VERSION_LEN];        /**< 硬件标识 */  
    CHAR szPCBVersion[IMOS_MW_VERSION_LEN];        /**< PCB版本 */  
    CHAR szUbootVersion[IMOS_MW_VERSION_LEN];      /**< UBOOT引导版本 */ 
}IMOS_MW_BASIC_DEVICE_INFO_S;

/**
* @struct tagMwSyslogServer
* @brief 日志主机的IP地址和端口
* @attention 无
*/
typedef struct tagMwSyslogServerInfo
{
    BOOL_T bEnable;                           /**< 使能日志主机 */
    CHAR szSyslogIP[IMOS_MW_IPADDR_LEN];      /**< 日志主机的IP地址 */
    USHORT usPort;                            /**< 日志主机端口号 */
    USHORT usReserve;                         /**< 保留字段，暂不使用 */
}IMOS_MW_SYSLOG_SERVER_INFO_S;

#if 0
#endif
/******************************* 网口信息 ***********************************/
/**
 * @struct tagMwPPPoE
 * @brief PPPoE配置
 * @attention 无
 */
typedef struct tagMwPPPoE
{
    CHAR szUserName[IMOS_MW_PPPOE_USERNAME_LEN + 4];     /**< PPPoE账号 */
    CHAR szPassword[IMOS_MW_PPPOE_PASSWORD_LEN + 4];     /**< PPPoE密码 */
}IMOS_MW_PPPOE_S;

/**
 * @struct tagMwUNPCfg
 * @brief UNP配置
 * @attention 
 */
typedef struct tagMwUNPCfg
{
    BOOL_T bEnable;                                    /**< UNP是否使能 */
    CHAR szAddress[IMOS_MW_IPADDR_LEN];                /**< UNP server 地址 */
    BOOL_T bIdentify;                                  /**< 是否鉴权 */
    CHAR szUserName[IMOS_MW_UNP_USERNAME_LEN + 4];     /**< UNP帐号 */
    CHAR szPassword[IMOS_MW_UNP_PASSWORD_LEN + 4];     /**< UNP密码 */
}IMOS_MW_UNP_CFG_S;
 
/**
 * @struct tagMwNetworkInterfaceCfg
 * @brief 网口信息
 * @attention 
 */
typedef struct tagMwNetworkInterfaceCfg
{
    ULONG ulIPGetType;                          /**< IP获取方式, 参见: IMOS_MW_IP_GET_TYPE_STATIC 等 */ 
    IMOS_MW_PPPOE_S stPPPoE;                    /**< PPPoE配置 */
    ULONG ulMTULen;                             /**< MTU长度, IP获取方式为PPPoE时该参数无效 */
    CHAR szIpAddress[IMOS_MW_IPADDR_LEN];       /**< IP地址 */
    CHAR szIpNetmask[IMOS_MW_IPADDR_LEN];       /**< 子网掩码 */
    CHAR szIpGateway[IMOS_MW_IPADDR_LEN];       /**< 网关 */
    ULONG ulWorkMode;                           /**< 网口工作模式, 参见: IMOS_MW_ETH_WORKMODE_AUTO_NEGOTIATION 等 */
}IMOS_MW_NETWORK_INTERFACE_CFG_S;

/**
 * @struct tagMwNetworkInterfaceInfo
 * @brief 网口状态信息
 * @attention 
 */
typedef struct tagMwNetworkInterfaceInfo
{
    CHAR szIpAddress[IMOS_MW_IPADDR_LEN];       /**< IP地址 */
    CHAR szIpNetmask[IMOS_MW_IPADDR_LEN];       /**< 子网掩码 */
    CHAR szIpGateway[IMOS_MW_IPADDR_LEN];       /**< 网关 */
    ULONG ulType;                               /**< 目前使用网口的类型，参见: IMOS_MW_PORT_MODE_ELECTRONIC 等 */
    ULONG ulWorkMode;                           /**< 网口当前工作模式, 参见: IMOS_MW_ETH_WORKMODE_AUTO_NEGOTIATION 等 */
}IMOS_MW_NETWORK_INTERFACE_INFO_S;

#if 0
#endif

/*************************** 图像相关结构 ****************************************/
/**
 * @struct tagMwSharpness
 * @brief 锐度参数 
 * @attention
 */
typedef struct tagMwSharpness
{
    ULONG ulMode;              /**< 锐度模式, 参见: IMOS_MW_SHARPNESS_AUTO 等 */
    ULONG ulSharpness;         /**< 锐度值 */
}IMOS_MW_SHARPNESS_S;

/**
 * @struct tagMwDenoise
 * @brief 图像降噪 
 * @attention 无
 */
typedef struct tagMwDenoise
{
    ULONG ul2DLevel;           /**< 2D降噪等级 */
    ULONG ul3DLevel;           /**< 3D降噪等级 */
}IMOS_MW_DENOISE_S;

/**
 * @struct tagMwWhiteBalance
 * @brief 白平衡参数 
 * @attention
 */
typedef struct tagMwWhiteBalance
{
    ULONG ulMode;                           /**< 白平衡模式, 参见: IMOS_MW_WB_AUTO 等 */
    LONG lBlueOffset;                       /**< 蓝色偏移量 */
    LONG lRedOffset;                        /**< 红色偏移量 */
}IMOS_MW_WHITE_BALANCE_S;

/**
 * @struct tagMwImageEnhance
 * @brief 图像增强参数 
 * @attention 
 */
typedef struct tagMwImageEnhance
{
    ULONG ulBrightness;                     /**< 亮度 */
    ULONG ulContrast;                       /**< 对比度 */
    ULONG ulSaturation;                     /**< 饱和度 */
    ULONG ulHue;                            /**< 色调 */
    IMOS_MW_SHARPNESS_S stSharpness;        /**< 锐度参数 */
    IMOS_MW_DENOISE_S stDenoise;            /**< 降噪参数 */
    ULONG ulMirrorMode;                     /**< 镜像模式, 参见: IMOS_MW_MIRROR_MODE_NONE 等 */
}IMOS_MW_IMAGE_ENHANCE_S;

/**
 * @struct tagMwFocus
 * @brief 对焦参数 
 * @attention 
 */
typedef struct tagMwFocus
{
    ULONG ulFocusMode;                       /**< 对焦模式, 参见: IMOS_MW_FOCUS_AUTO 等 */
    ULONG ulAutoFocusSense;                  /**< 自动对焦灵敏度 */
    ULONG ulManualFocusMinDistance;          /**< 最小对焦距离，手动对焦, 单位: 毫米 */
}IMOS_MW_FOCUS_S;


/**
 * @struct tagMwWhiteBalanceCfg
 * @brief 白平衡参数配置
 * @attention 
 */
typedef struct tagMwWhiteBalanceCfg
{
    ULONG ulImageType;                         /**< 图像类型, 参见: IMOS_MW_IMAGE_TYPE_VIDEO 等 */
    IMOS_MW_WHITE_BALANCE_S stWhiteBalance;    /**< 白平衡参数 */
}IMOS_MW_WHITE_BALANCE_CFG_S;

/**
 * @struct tagMwImageEnhanceCfg
 * @brief 图像增强参数配置
 * @attention 
 */
typedef struct tagMwImageEnhanceCfg
{
    ULONG ulImageType;                         /**< 图像类型, 参见: IMOS_MW_IMAGE_TYPE_VIDEO 等 */
    IMOS_MW_IMAGE_ENHANCE_S stImageEnhance;    /**< 图像增强参数 */
}IMOS_MW_IMAGE_ENHANCE_CFG_S;

/**
 * @struct tagMwImageEnhanceCfg
 * @brief 对焦参数配置
 * @attention 
 */
typedef struct tagMwFocusCfg
{
    ULONG ulImageType;                         /**< 图像类型, 参见: IMOS_MW_IMAGE_TYPE_VIDEO 等 */
    IMOS_MW_FOCUS_S stFocus;                   /**< 对焦参数 */
}IMOS_MW_FOCUS_CFG_S;

/**
 * @struct tagMwConditionCfgEx
 * @brief 场景 环境参数
 * @attention
 */
typedef struct tagMwConditionCfgEx
{
    ULONG ulType;              /**< 环境参数类型，参见: IMOS_MW_CONDITION_ILLUMINATION 等 */
    LONG lParam1;              /**< 环境参数1 */
    LONG lParam2;              /**< 环境参数2 */
    LONG lParam3;              /**< 环境参数3 */
}IMOS_MW_CONDITION_CFG_EX_S;

#if 0
#endif
/*************************** OSD 相关结构 ****************************************/
/**
 * @struct tagMwOSDInfoParam
 * @brief OSD 叠加内容
 * @attention 无
 */
typedef struct tagMwOSDInfoParam
{
    ULONG ulInfoType;                              /**< 叠加内容类型，参考: IMOS_MW_OSD_INFO_TYPE_UNUSED 等 */
    CHAR szValue[IMOS_MW_OSD_INFO_LEN + 4];        /**< 自定义 OSD 内容 */ 
}IMOS_MW_OSD_INFO_PARAM_S;

/**
* @struct tagMwOSDInfoConf
* @brief 叠加OSD 参数
* @attention 无
*/
typedef struct tagMwOSDInfoConf
{
    ULONG ulIndex;                                                       /**< 叠加区域ID */
    BOOL_T bEnable;                                                      /**< 使能开关 */ 
    IMOS_MW_OSD_INFO_PARAM_S astInfoParam[IMOS_MW_INFO_OSD_LINE_MAX];    /**< 叠加内容 */  
    IMOS_MW_RECTANGLE_S stArea;                                          /**< 叠加区域 */
}IMOS_MW_INFO_OSD_S;

/**
* @struct tagMwInfoOSDCfg
* @brief 叠加OSD 配置
* @attention 删除该配置时，只需指定待删除区域的ID
*/
typedef struct tagMwInfoOSDCfg
{
    ULONG ulAreaNum;                        /**< 叠加区域个数 */
    IMOS_MW_INFO_OSD_S astInfoOSD[1];       /**< 叠加OSD 配置，区域最大个数为: IMOS_MW_INFO_OSD_MAX_NUM */
}IMOS_MW_INFO_OSD_CFG_S;

/**
* @struct tagMwOSDStyleCfg
* @brief 叠加OSD 样式
* @attention 无
*/
typedef struct tagMwOSDStyleCfg
{
    ULONG ulFontStyle;     /**< 字体形式, 参见: IMOS_MW_OSD_FONT_STYLE_NORMAL 等 */
    ULONG ulFontSize;      /**< 字体大小, 参见: IMOS_MW_OSD_FONT_SIZE_LARGE 等 */
    ULONG ulFontType;      /**< 字体, 暂不支持设置 */
    ULONG ulColor;         /**< 颜色, 参见: IMOS_MW_OSD_COLOR_WHITE 等 */
    ULONG ulAlpha;         /**< 透明度, 参见: IMOS_MW_OSD_ALPHA_NO 等 */
    ULONG ulTimeFormat;    /**< 时间格式, 参见: IMOS_MW_TIME_FORMAT_HHMMSS 等 */
    ULONG ulDateFormat;    /**< 日期格式, 参见: IMOS_MW_DATE_FORMAT_YYYYMMDD 等 */
}IMOS_MW_OSD_STYLE_CFG_S;

#if 0
#endif
/******************************* 媒体流相关结构 ***********************************/
/**
 * @struct tagMwResolution
 * @brief 分辨率
 * @attention 
 */
typedef struct tagMwResolution
{
    ULONG ulWidth;           /**< 宽，单位: 像素 */
    ULONG ulHeight;          /**< 高，单位: 像素 */
}IMOS_MW_RESOLUTION_S;

/**
 * @struct tagMwVideoInModeCfg
 * @brief 采集制式
 * @attention 
 */
typedef struct tagMwVideoInModeCfg
{
    IMOS_MW_RESOLUTION_S stResolution;    /**< 采集分辨率 */ 
    ULONG ulFrameRate;                    /**< 采集帧率 */
}IMOS_MW_VIDEOIN_MODE_CFG_S;

/**
 * @struct tagMwVideoStreamConf
 * @brief 视频编码参数
 * @attention 
 */
typedef struct tagMwVideoStreamConf
{
    BOOL_T bIsEnable;                              /**< 启动/关闭编码 0-关闭 1-启动 */
    ULONG ulEncodeFmt;                             /**< 编码格式，参见: IMOS_MW_STREAM_FORMAT_MPEG1 等 */
    IMOS_MW_RESOLUTION_S stResolution;             /**< 编码分辨率 */ 
    ULONG ulBitRate;                               /**< 码率 */
    ULONG ulFrameRate;                             /**< 帧率 */ 
    ULONG ulGopType;                               /**< gop结构参数, 参考: IMOS_MW_GOP_TYPE_IP 等 */  
    ULONG ulIInterval;                             /**< I帧间隔 */
    ULONG ulEncMode;                               /**< 编码模式，参见: IMOS_MW_ENCODE_MODE_CBR 等 */ 
    ULONG ulQuality;                               /**< 编码压缩质量 */
    ULONG ulSmoothValue;                           /**< 码流平滑 */                            
}IMOS_MW_VIDEO_STREAM_CONF_S;

/**
 * @struct tagMwVideoEncoderCfg
 * @brief 视频编码参数配置
 * @attention 获取指定流的编码参数时，需要传入流ID
 */
typedef struct tagMwVideoEncoderCfg
{
    ULONG ulStreamID;                              /**< 流ID, 参见: IMOS_MW_STREAM_ID_MAIN 等 */
    IMOS_MW_VIDEO_STREAM_CONF_S stVideoStreamCfg;  /**< 视频编码参数 */
}IMOS_MW_VIDEO_ENCODER_CFG_S;

/**
 * @enum tagMwMediaStreamInfo
 * @brief 媒体流信息
 * @attention 无
 */
typedef struct tagMwMediaStreamInfo
{
    ULONG ulStreamHandle;                         /**< 流句柄 */
    ULONG ulStreamID;                             /**< 流ID, 参见: IMOS_MW_STREAM_ID_MAIN 等 */
    ULONG ulTransMode;                            /**< 传输模式，参见: IMOS_MW_TRANSFER_MODE_RTP_UDP 等 */
    CHAR szIPAddr[IMOS_MW_IPADDR_LEN];            /**< 流目的IP地址 */
    USHORT usPort;                                /**< 流目的端口 */
    USHORT usReserved;                            /**< 保留 */
    ULONG ulMediaType;                            /**< 媒体类型, 参见: IMOS_MW_MEDIA_VIDEO 等 */
    CHAR szUser[IMOS_MW_NAME_LEN];                /**< 流建立者 */
}IMOS_MW_VIDEO_STREAM_INFO_S;

/**
* @struct tagMwVideoParam
* @brief 视频解码参数
* @attention 无
*/
typedef struct tagMwVideoParam
{
    ULONG ulFrameRate;       /**< 帧率, 单位: fps */
    ULONG ulBitRate;         /**< 码率, 单位: bps */
    ULONG ulRecvPktNum;      /**< 已接收到包数 */
    ULONG ulLostPktNum;      /**< 丢弃包数 */
    ULONG ulAllFrameNum;     /**< 帧总数 */
    ULONG ulLostFrameNum;    /**< 丢弃的帧数 */
}IMOS_MW_VIDEO_PARAM_S;

#if 0
#endif
/******************************* 存储(本地)相关结构 ***********************************/
/**
* @struct tagMwLocalPicStor
* @brief 本地SD卡抓拍存储配置信息
* @attention 
*/
typedef struct tagMwLocalPicStor
{   
    ULONG ulAllocPercent;                       /**< 分配容量百分比，单位 %，默认0, 界面暂不使用，界面不显示 */ 
    ULONG ulAllocMemory;                        /**< 分配容量, 单位: MB */
    ULONG ulSpareMemory;                        /**< 剩余容量, 单位: MB */             
    ULONG ulCurrentFileNum;                     /**< 当前文件个数 */                  
    ULONG ulSpareFileNum;                       /**< 剩余可存文件个数 */    
    ULONG ulStorPolicy;                         /**< 存储策略 参见: IMOS_MW_STOR_POLICY_XXX */
    ULONG ulStorMode;                           /**< SD卡存储模式 参见: IMOS_MW_LOCAL_STOR_XXX */
}IMOS_MW_LOCAL_PIC_STORCFG_S;

/**
* @struct tagMwLocalVideoStor
* @brief 本地SD卡视频存储配置信息
* @attention 
*/
typedef struct tagMwLocalVideoStor
{   
    ULONG ulAllocPercent;                       /**< 分配容量百分比，单位 %，默认0, 界面暂不使用，界面不显示 */ 
    ULONG ulAllocMemory;                        /**< 分配容量, 单位: MB */
    ULONG ulPlanStreamID;                       /**< 计划流 流ID */
    ULONG ulManualStreamID;                     /**< 手动流 流ID */
    ULONG ulEventStreamID;                      /**< 事件流 流ID */
    ULONG ulStorPolicy;                         /**< 存储策略 参见: IMOS_MW_STOR_POLICY_XXX */
    ULONG ulStorMode;                           /**< SD卡存储模式 参见: IMOS_MW_LOCAL_STOR_XXX */
}IMOS_MW_LOCAL_VIDEO_STORCFG_S;

/**
* @struct tagMwLocalStorInfo
* @brief 本地SD 卡存储
* @attention 
*/
typedef struct tagMwLocalStorInfo
{       
    ULONG ulTotalMemory;                             /**< 本地存储总容量（SD卡容量）, 单位: MB */
    IMOS_MW_LOCAL_PIC_STORCFG_S stPicStorCfg;        /**< SD卡抓拍图片存储配置 */  
    IMOS_MW_LOCAL_VIDEO_STORCFG_S stVideoStorCfg;    /**< SD卡视频存储配置 */  
}IMOS_MW_LOCAL_STOR_INFO_S;

/**
* @struct tagMwLocalStorPicInfo
* @brief SD 卡文件信息
* @attention 
*/
typedef struct tagMwStorLocalPicInfo
{
     CHAR stFile[IMOS_MW_NAME_LEN];          /**< 文件ID, 单个文件ID 长度不超过 IMOS_MW_FILE_NAME_LEN */
     IMOS_MW_TIME_S stTime;                  /**< 时间(年月日时分秒) */
     CHAR szPictureInfo[IMOS_MW_NAME_LEN];   /**< 车牌号(卡口) */
}IMOS_MW_STOR_LOCAL_PIC_INFO_S;

/**
* @struct tagMwStorLocalVideo
* @brief 本地SD 卡 录像文件
* @attention 无
*/
typedef struct tagMwStorLocalVideo
{
    CHAR szFile[IMOS_MW_FILE_NAME_LEN];      /**< 录像文件ID */ 
    ULONG ulFileSize;                        /**< 录像文件大小 */ 
    IMOS_MW_TIME_S stSartTime;               /**< 录像文件开始时间 */ 
    IMOS_MW_TIME_S stEndTime;                /**< 录像文件结束时间 */ 
}IMOS_MW_STOR_LOCAL_VIDEO_S;

/**
* @struct tagMwStorPathInfo
* @brief 存储路径
* @attention 无
*/
typedef struct tagMwStorPathInfo
{
    CHAR szName[IMOS_MW_DEFAULT_PATH_LEN];       /**< 目录/文件 的名称 */
    ULONG ulDirType;                             /**< 路径属性: 目录/文件, 参见: IMOS_MW_STOR_PATH_DIR 等 */
    ULONG ulStorFileType;                        /**< 文件类型: 照片/录像, 参见: IMOS_MW_STOR_FILE_TYPE_VIDEO 等 */
}IMOS_MW_STOR_PATH_INFO_S;

#if 0
#endif
/******************************* 播放器相关结构 ***********************************/
/**
* @struct tagMwPhotoServerCfg
* @brief 照片接收服务器配置
* @attention 
*/
typedef struct tagMwPhotoServerCfg
{
    ULONG ulProtocol;                                               /**< 通信协议类型 */
    CHAR szCameraID[IMOS_MW_PHOTOSERVER_CODE_LEN + 4];              /**< 相机编号 */
    CHAR szTollgateID[IMOS_MW_PHOTOSERVER_CODE_LEN + 4];            /**< 卡口编号 */
    CHAR szDrivewayID[IMOS_MW_PHOTOSERVER_CODE_LEN + 4];            /**< 道路编号 */
    CHAR szDirectionID[IMOS_MW_PHOTOSERVER_CODE_LEN + 4];           /**< 方向编号 */
    CHAR szDeviceID[IMOS_MW_PHOTOSERVER_CODE_LEN + 4];              /**< 设备编号 */
    CHAR szPassword[IMOS_MW_PHOTOSERVER_CODE_LEN + 4];              /**< 平台接入密码 */
    CHAR szUserName[IMOS_MW_PHOTOSERVER_CODE_LEN + 4];              /**< 接入平台用户名 */
    CHAR szServerPassword[IMOS_MW_PHOTOSERVER_CODE_LEN + 4];        /**< 接入平台密码 */
    IMOS_MW_ADDR_S stServerAddr;                                    /**< 照片服务器地址 */
}IMOS_MW_PHOTO_SERVER_CFG_S;


/**
 * @struct tagMwUniviewMultiProtocolHeader
 * @brief 宇视协议信息头,多张照片
 * @attention 对外接口
 */
typedef struct tagMwUniviewMultiProtocolHeader
{
    CHAR *apcData[IMOS_MW_TRAFFIC_PIC_MAX_NUM];                                         /**< 数据指针 */
    ULONG aulDataLen[IMOS_MW_TRAFFIC_PIC_MAX_NUM];                                      /**< 数据长度 */
    ULONG aulPicType[IMOS_MW_TRAFFIC_PIC_MAX_NUM];                                      /**< 照片类型, 参照:IMOS_MW_IMAGE_VEHICLE */
    CHAR acPassTime[IMOS_MW_TRAFFIC_PIC_MAX_NUM][IMOS_MW_UNIVIEW_MAX_TIME_LEN];         /**< 经过时间 */
    ULONG ulPicNumber;                                                                  /**< 照片张数 */

    /* 设备信息 */ 
    LONG lApplicationType;                                        /**< 应用类型:对应相关产品 */ 
    CHAR szCamID[IMOS_MW_DEV_ID_MAX_LEN];                         /**< 设备编号:采集设备统一编号或卡口相机编码, 不可为空 */ 
    CHAR szTollgateID[IMOS_MW_DEV_ID_MAX_LEN];                    /**< 卡口编号:产生该信息的卡口代码 */ 
    CHAR szTollgateName[IMOS_MW_TOLLGATE_NAME_MAX_LEN];           /**< 卡口名称:可选字段 */ 
    
    ULONG ulCameraType;                                        /**< 相机类型 0 全景 1特性 */ 
    ULONG ulRecordID;                                          /**<车辆信息编号:由1开始自动增长(转换成字符串要求不超过16字节) */ 

    /* 时间、地点信息 */ 
    CHAR szPassTime[IMOS_MW_UNIVIEW_MAX_TIME_LEN];               /**< 经过时刻:YYYYMMDDHHMMSS, 时间按24小时制 */ 
    CHAR szPlaceName[IMOS_MW_PLACE_NAME_MAX_LEN];                /**< 地点名称 */ 

    LONG lLaneID;                /**< 车道编号:从1开始, 车辆行驶方向最左车道为1，由左向右顺序编号 */ 
    LONG lLaneType;             /**< 车道类型:0-机动车道，1-非机动车道 */ 

    /**< 方向编号:1-东向西 2-西向东 3-南向北 4-北向南 
       5-东南向西北 6-西北向东南 7-东北向西南 8-西南向东北 */ 
    LONG lDirection; 
    CHAR szDirectionName[IMOS_MW_DIRECTION_NAME_MAX_LEN];       /**< 方向名称:可选字段 */ 
    
    /* 车牌信息 */ 
    CHAR szCarPlate[IMOS_MW_CAR_PLATE_MAX_LEN];              /**< 号牌号码:不能自动识别的用"-"表示 */ 
    ULONG aulLPRRect[4];                                     /**< 车牌坐标:XL=a[0], YL=a[1], XR=a[2], YR=a[3] */ 
    LONG lPlateType;                                         /**< 号牌种类:按GA24.7编码 */   
    LONG lPlateColor;                                        /**< 号牌颜色:0-白色1-黄色 2-蓝色 3-黑色 4-其他 */ 
    LONG lPlateNumber;                                       /**< 号牌数量 */   
    
    /**< 号牌一致: 
       0-车头和车尾号牌号码不一致 
       1-车头和车尾号牌号码完全一致 
       2-车头号牌号码无法自动识别 
       3-车尾号牌号码无法自动识别 
       4-车头和车尾号牌号码均无法自动识别 */ 
    LONG lPlateCoincide;     
    CHAR szRearVehiclePlateID[IMOS_MW_CAR_PLATE_MAX_LEN];   /**< 尾部号牌号码:被查控车辆车尾号牌号码，允许车辆尾部号牌号码不全。不能自动识别的用"-"表示 */ 
    LONG lRearPlateColor;                                   /**< 尾部号牌颜色: 0-白色1-黄色 2-蓝色 3-黑色 4-其他 */ 
    LONG lRearPlateType;                                    /**< 尾部号牌种类: 按GA24.7编码; 或者1－单排 2－武警 3－警用 4－双排 5－其他 */ 

    /**< 车辆信息 */     
    ULONG aulVehicleXY[4];                                 /**< 车辆坐标:XL=a[0], YL=a[1], XR=a[2], YR=a[3] */ 
    CHAR szVehicleBrand[IMOS_MW_CAR_VEHICLE_BRAND_LEN];    /**< 车辆厂牌编码(自行编码) 考虑到字节对齐定义长度为4，实际使用长度为2 */ 
    LONG lVehicleBody;                                     /**< 车辆外型编码(自行编码) */ 
    LONG lVehicleType;                                     /**< 车辆类型 0-未知，1-小型车 2-中型车 3-大型车 4-其他 */ 
    LONG lVehicleLength;                                   /**< 车外廓长(以厘米为单位) */ 
    LONG lVehicleColorDept;                                /**< 车身颜色深浅:0-未知，1-浅，2-深 */ 
    /**< 车身颜色: 
       A：白，B：灰，C：黄，D：粉，E：红，F：紫，G：绿，H：蓝， 
       I：棕，J：黑，K：橙，L：青，M：银，N：银白，Z：其他(!) */ 
    CHAR cVehicleColor;            /* 车身颜色 */ 

    /* 识别，注:后面的UCHAR紧跟CHAR */ 
    UCHAR ucPlateScore;            /**< 此次识别中，整牌的置信度，100最大 */ 
    UCHAR ucRearPlateScore;        /**< 尾部号码置信度，100最大 */ 
    UCHAR ucPicType;               /**< 0:实时照片，1:历史照片 */ 
    LONG lIdentifyStatus;          /**< 识别状态:0－识别成功 1－不成功 2－不完整(!)  3-表示需要平台识别 */ 
    LONG lIdentifyTime;            /**< 识别时间, 单位毫秒 */       
    LONG lDressColor;              /**< 行人衣着颜色(!) */     
    LONG lDealTag;                 /**< 处理标记:0-初始状态未校对 1-已校对和保存 2-无效信息 3-已处理和保存(!) */ 

    /* 车速 */ 
    LONG lVehicleSpeed;            /**< 车辆速度: 单位km/h, -1-无测速功能 */ 
    LONG lLimitedSpeed;            /**< 执法限速: 车辆限速, 单位km/h */ 
    LONG lMarkedSpeed;             /**< 标识限速 */     
    LONG lDriveStatus;             /**< 行驶状态:0-正常 1-嫌疑或按GA408.1编码 */ 

    /**< 红灯信息 */ 
    LONG lRedLightTime;                                   /**< 红灯时间 */ 
    CHAR szRedLightStartTime[IMOS_MW_UNIVIEW_MAX_TIME_LEN];  /**< 红灯开始时间:YYYYMMDDHHMMSS, 精确到毫秒, 时间按24小时制 */ 
    CHAR szRedLightEndTime[IMOS_MW_UNIVIEW_MAX_TIME_LEN];    /**< 红灯结束时间:YYYYMMDDHHMMSS, 精确到毫秒, 时间按24小时制 */   
}IMOS_MW_MULTI_UNIVIEW_PROTOCOL_HEADER_S;

/******************************************************************************
标识：IMOS_MW_ONE_PIC_INFO_S
类型：结构体
目的：单张照片信息
定义：
******************************************************************************/ 
typedef struct tagMwOnePicInfo
{
    VOID *pPicData;                         /* 图像类型:1-车辆大图 2-车牌彩色小图 3-车牌二值化图 */
    ULONG ulPicLen;                         /* 图片长度 */
}IMOS_MW_SINGLE_PIC_INFO_S;

/**
 * @struct tagMwPicData
 * @brief  照片数据
 * @attention 对外接口
 */
typedef struct tagMwPicData
{
    ULONG ulPicNum;                                                    /* 照片张数 */
    IMOS_MW_SINGLE_PIC_INFO_S astPicInfo[IMOS_MW_TRAFFIC_PIC_MAX_NUM];    /* 照片信息 */
}IMOS_MW_PIC_INFO_S;

/**
 * @struct tagMwDeviceRunInfo
 * @brief 设备运行状态 (系统运行时间，设备温度，CPU使用率，内存使用率)
 * @attention 无
 */
typedef struct tagMwDeviceRunInfo
{
    ULONG ulRunTime;                 /**< 设备运行时间 */
    LONG lDeviceTemperature;         /**< 设备温度 */
    ULONG ulCPUUtilization;          /**< CPU使用率 */
    ULONG ulMemUtilization;          /**< 内存使用率 */
}IMOS_MW_DEVICE_RUN_INFO_S;

/**
 * @struct tagMwTransPortCfg
 * @brief 透明通道配置
 * @attention
 */
typedef struct tagMwTransPortCfg
{
    ULONG ulSerialID;               /**< 串口ID (详见串口能力描述) */
    ULONG ulEnable;                 /**< 使能透明通道: 0 不启用, 1 启用 */
    ULONG ulTransMode;              /**< 传输模式，暂不支持，留待扩展: UDP/TCP 等 */
    IMOS_MW_ADDR_S stPeerAddr;      /**< 远端地址 */
    IMOS_MW_ADDR_S stLocalAddr;     /**< 本端地址 */
}IMOS_MW_TRANSPORT_CFG_S;

/**
* @struct tagMwSingTransPortCfg
* @brief 单透明通道配置
* @attention
*/
typedef struct tagMwSingTransPortCfg
{
    ULONG ulChannelID;                             /**< 透明通道号 */   
    IMOS_MW_TRANSPORT_CFG_S stTransPortCfg;        /**< 透明通道配置 */
}IMOS_MW_SING_TRANSPORT_CFG_S;

/**
* @struct tagMwMultiTransPortCfg
* @brief 多透明通道配置
* @attention
*/
typedef struct tagMwMultiTransPortCfg
{
    ULONG ulTransPortNum;                                                           /**< 透明通道数 */   
    IMOS_MW_SING_TRANSPORT_CFG_S astSingTransPortCfg[IMOS_MW_SERIAL_MAX_NUM];       /**< 各透明通道配置 */
}IMOS_MW_MULTI_TRANSPORT_CFG_S;

/**
 * @struct tagMwTrParam
 * @brief 交通参数结果
 * @attention
 */
typedef struct tagMwTrParam
{
    ULONG ulLaneID;                  /**< 车道编号 */
    ULONG ulTrafficFlow;             /**< 汽车流量 */
    ULONG ulAveSpeed;                /**< 平均速度，单位:千米/小时 */
    FLOAT fAveHeadTime;              /**< 平均车头时距，单位:秒 */
    FLOAT fAveTimOcupyRat;           /**< 平均车道时间占有率,，单位:X100% */
    FLOAT fAveHeadSpac;              /**< 平均车头间距，单位:米 */
    FLOAT fVehQueueLen;              /**< 车道队列长度，单位:米 */
    FLOAT fAveSpaOcupyRat;           /**< 平均车道空间占有率,，单位:X100% */
    ULONG ulWayStat;                 /**< 车道通行状态，分为多个等级，例:拥堵，通畅等 */
    ULONG aulCarTypeCount[IMOS_MW_CAR_TYPE_NUM];      /**< 各种车辆类型的车流量 0-11分别代表轿车、SUV、面包车、小货车、
                                                           中巴车、大客车、大货车、大型车、中型车、小型车、摩托车、超长车 */ 
    ULONG aulLPRColorCount[IMOS_MW_LPR_COLOR_NUM];    /**< 各种车牌类型的车流量 0-4 白牌、黄牌、蓝牌、黑牌、其他 */
    ULONG aulSpeedCarCount[IMOS_MW_CAR_SPEED_NUM];    /**< 各车速统计 0 代表超速，1代表低速  未实现 */
    ULONG ulVehicles;                                 /**< 通过车辆总数 辆 未实现 */
    ULONG ulVehicleLength;                            /**< 车外轮廓 单位:厘米 未实现 */
    ULONG ulDensity;                                  /**< 车辆密度 每公里的车辆数，单位辆/km 未实现 */
    ULONG ulVolume;                                   /**< 交通量 辆/单位时间 未实现 */
    ULONG ulFlowRate;                                 /**< 流率 辆/小时 未实现 */
    ULONG ulBackOfQueue;                              /**< 旅行时间 秒 未实现 */
    ULONG ulDelay;                                    /**< 延误 秒 未实现 */
}IMOS_MW_TRAFFIC_PARA_S;

/**
 * @struct tagMwTrParamRes
 * @brief 各个车道交通参数结果
 * @attention
 */
typedef struct tagMwTrParamRes
{
    CHAR szStartTime[IMOS_MW_UNIVIEW_MAX_TIME_LEN];                     /* 开始时间 YYYYMMDDhhmmss*/
    ULONG ulDriveWayNum;                                                /* 实际配置的车道数 */    
    IMOS_MW_TRAFFIC_PARA_S astTrParaRslt[IMOS_MW_DRIVEWAY_NUM_MAX];     /* 各车道交通流量参数结果 */
}IMOS_MW_TRAFFIC_PARA_RSLT_S;

/**
 * @enum tagMwEpTgVehicleStat
 * @brief 车辆进出状态
 * @attention
 */
typedef enum tagMwEpTgVehicleStat
{
    VEHICLE_STAT_INVALID          = -1,          /* 进出状态未变化 */
    VEHICLE_STAT_NONE             = 0,           /* 车辆初始状态 */
    VEHICLE_STAT_IN_LINE          = 1,           /* 车辆进入拌线 */
}IMOS_MW_TRAFFIC_VEHICLE_STAT_E;

/**
 * @struct tagMwVehicleState
 * @brief 各个车道车辆进出状态
 * @attention
 */
typedef struct tagMwVehicleState
{
    ULONG ulDriveWayNum;                                                         /* 实际配置的车道数 */
    CHAR szStatPassTime[IMOS_MW_UNIVIEW_MAX_TIME_LEN];                           /* 通过时间 YYYYMMDDHHMMSSMMM */
    IMOS_MW_TRAFFIC_VEHICLE_STAT_E aenVehicleState[IMOS_MW_DRIVEWAY_NUM_MAX];    /* 各车道车辆进出状态 */
    ULONG aulDriveWayCode[IMOS_MW_DRIVEWAY_NUM_MAX];                             /* 各个车道编码 */
}IMOS_MW_VEHICLE_STATE_S;

/**
 * @struct tagMwDayNight
 * @brief 昼夜模式参数
 * @attention
 */
typedef struct tagMwDayNight
{
    ULONG ulMode;              /**< 昼夜模式, 参见: IMOS_MW_DAYNIGHT_AUTO 等 */
    ULONG ulSensitivity;       /**< 切换灵敏度 */
}IMOS_MW_DAYNIGHT_S;

/**
 * @struct tagMwMetering
 * @brief 测光参数
 * @attention
 */
typedef struct tagMwMetering
{
    ULONG ulMode;                  /**< 测光模式, 参见: IMOS_MW_METERING_CENTER 等 */
    IMOS_MW_RECTANGLE_S stArea;    /**< 测光区域 */
    ULONG ulRefBrightness;         /**< 参考亮度 */
    ULONG ulHoldTime;              /**< 持续时间 */
}IMOS_MW_METERING_S;

/**
 * @struct tagMwImgWDR
 * @brief 宽动态参数
 * @attention
 */
typedef struct tagMwImgWDR
{
    ULONG ulMode;              /**< 宽动态模式, 参见: IMOS_MW_WDR_DISABLE 等 */
    ULONG ulExpRatio;          /**< 曝光比率，暂不支持 */
    ULONG ulOnSense;           /**< 宽动态开启灵敏度 */
    ULONG ulOffSense;          /**< 宽动态关闭灵敏度 */
    BOOL_T bAntiFlicker;       /**< 宽动态条纹抑制 */
}MW_WDR_S;

/**
 * @struct tagMwSlowShutter
 * @brief 慢快门
 * @attention 无
 */
typedef struct tagMwSlowShutter
{
    BOOL_T bEnable;                         /**< 慢快门使能项 */
    ULONG ulMaxSlowShutter;                 /**< 最慢慢快门 */
}IMOS_MW_SLOW_SHUTTER_S;

/**
 * @struct tagMwCustomExposure
 * @brief 自定义曝光参数
 * @attention 无
 */
typedef struct tagMwCustomExposure
{
    ULONG ulMaxIris;                        /**< 最大光圈, 单位: f值*100 */
    ULONG ulMinIris;                        /**< 最小光圈, 单位: f值*100 */
    ULONG ulSlowShutter;                    /**< 最慢快门 */
    ULONG ulFastShutter;                    /**< 最快快门 */
    ULONG ulMaxGain;                        /**< 最大增益, 单位: db */
    ULONG ulMinGain;                        /**< 最小增益, 单位: db */
}IMOS_MW_CUSTOM_EXPOSURE_S;

/**
 * @struct tagMwImgExposure
 * @brief 曝光参数
 * @attention
 */
typedef struct tagMwImgExposure
{
    ULONG ulMode;                                /**< 曝光模式, 参见: IMOS_MW_EXPOSURE_AUTO 等 */
    LONG lExpCompensate;                         /**< 曝光补偿 */
    ULONG ulIris;                                /**< 光圈值, 单位: f值*100 */
    ULONG ulShutter;                             /**< 快门值 */
    ULONG ulGain;                                /**< 增益, 单位: db */
    IMOS_MW_CUSTOM_EXPOSURE_S stCustomExposure;  /**< 自定义曝光参数 */
    IMOS_MW_SLOW_SHUTTER_S stSlowShutter;        /**< 慢快门参数 */
    MW_WDR_S stWDR;                              /**< 宽动态参数 */
    IMOS_MW_METERING_S stMetering;               /**< 测光参数 */
    IMOS_MW_DAYNIGHT_S stDayNight;               /**< 昼夜参数 */
}IMOS_MW_EXPOSURE_S;

/**
 * @struct tagMwDriveExpandSerialParam
 * @brief 车道串口号参数
 * @attention 无
 */

typedef struct tagMwDriveExpandSerialParam
{
    ULONG aulSerialID[4];                                   /**< 串口号 */
}IMOS_MW_DRIVE_EXPAND_SERIAL_PARAM_S;

/**
 * @struct tagMwTimeSectionCfg
 * @brief 计划时间段配置
 * @attention
 * -         时间字符串格式: 由时分秒组成中间使用":"号隔开, 如 00:01:59 表示 0点1分59秒
 */
typedef struct tagMwTimeSectionCfg
{
    CHAR szStartTime[12];  /**< 起始时间 */
    CHAR szEndTime[12];    /**< 结束时间 */
}IMOS_MW_TIME_SECTION_S;

/**
 * @struct tagMwVehicleDrvCfg
 * @brief 车辆行驶方向配置
 * @attention
 */
typedef struct tagMwVehicleDrvCfg
{
    ULONG ulDrivingDirection;                      /**< 车辆行驶方向，参见: IMOS_MW_DRIVING_DIRECTION_COME 等 */
    IMOS_MW_TIME_SECTION_S stTimeSection;          /**< 行驶方向生效时间段 */
}IMOS_MW_VEHICLE_DRV_CFG_S;

/**
 * @struct tagMwDriveExpandParam
 * @brief 车道扩展参数
 * @attention 无
 */
typedef struct tagMwDriveExpandParam
{
    IMOS_MW_VEHICLE_DRV_CFG_S astDrvCfg[4];      /**< 车辆行驶方向配置 */
}IMOS_MW_DRIVE_EXPAND_PARAM_S;

/**
 * @struct tagMwSingSpeedLimit
 * @brief 限速非默认配置
 * @attention
 */
 typedef struct tagMwSingSpeedLimit
{
    ULONG ulLimitSpeed;                                     /**< 限速值 */
    IMOS_MW_TIME_SECTION_S stTimeSection;                   /**< 限速使能时间段*/
}IMOS_MW_SINGEL_SPEED_LIMIT_S;

/**
 * @struct tagMwWaySpeedLimit
 * @brief 分车道限速
 * @attention
 */
typedef struct tagMwWaySpeedLimit 
{
    ULONG ulDrivewaySpeedLimit;                                                     /**< 默认车道限速值 */
    ULONG ulDrivewayMinSpeedLimit;                                                  /**< 默认车道最低限速值，最低限速不需要分时段配置 */
    IMOS_MW_SINGEL_SPEED_LIMIT_S astWaySpeedLimit[6];      /**< 另配车道限速值 */
}IMOS_MW_WAY_SPEED_LIMIT_S;

/**
 * @struct tagMwCarSpeedLimit
 * @brief 分车型限速
 * @attention
 */
typedef struct tagMwCarSpeedLimit
{
    ULONG ulVehicleType;                                                            /**< 车型, 参见: IMOS_MW_VEHICLETYPE_SMALL 等 */
    ULONG ulMinSpeedLimit;                                                          /**< 默认最低限速值，最低限速不需要分时段配置 */
    ULONG ulSpeed;                                                                  /**< 默认当前车型限速值 */
    IMOS_MW_SINGEL_SPEED_LIMIT_S astCarSpeedLimit[6];      /**< 另配当前车型限速值 */
}IMOS_MW_CAR_SPEED_LIMIT_S;

/**
 * @struct tagMwSpeedLimitUnion
 * @brief 限速参数
 * @attention
 */
union tagMwSpeedLimitUnion
{
    IMOS_MW_WAY_SPEED_LIMIT_S stWaySpeedLimit;                                      /**< 按车道限速 */
    IMOS_MW_CAR_SPEED_LIMIT_S astVehicleTypeSpeedLimit[8];    /**< 车型限速信息 */
};

/**
 * @struct tagMwDriveWayType
 * @brief 车道行驶类型
 * @attention
 */
typedef struct tagMwDriveWayType
{
    ULONG ulWayType;                                                        /**< 车道行驶类型，参见IMOS_MW_DRIVING_TYPE_STRAIGHT */
    IMOS_MW_TIME_SECTION_S stTimeSection;                                   /**< 车道行驶类型使能时间段*/
}IMOS_MW_DRIVE_WAY_TYPE_S;

/**
 * @struct tagMwSingDrivewayExpandCfg
 * @brief 单个车道拓展配置
 * @attention 无
 */
 typedef struct tagMwSingleWayExpandCfg
{
    ULONG ulDriDirection;                                                            /**< 默认车辆行驶方向，参见IMOS_MW_DRIVING_DIRECTION_COME */
    IMOS_MW_VEHICLE_DRV_CFG_S astDriDirection[6];           /**< 独立配置车辆行驶方向 */
    ULONG ulDriWayType;                                                              /**< 默认车道行驶类型，参见: IMOS_MW_DRIVING_TYPE_STRAIGHT 等 */
    IMOS_MW_DRIVE_WAY_TYPE_S astDrivWayType[6];             /**< 独立配置车道行驶类型 */
    ULONG ulExeSpeedAdjustFactor;                                                    /**< 执行限速 */
    ULONG ulSpeedLimitMode;                                                          /**< 车辆限速模式, 参见: IMOS_MW_SPEEDLIMIT_MODE_DRIVEWAY 等 */
    int __unionReserve;                                                              /**< 保留(soap需要该字段来确认使用UNION的哪个) */
    union tagMwSpeedLimitUnion unDriveWaySpeedLimit;                                 /**< 车道限速值配置 */
}IMOS_MW_SING_DRIV_WAY_EXP_CFG_S;

/**
 * @struct tagMwDrivewayExpandCfg
 * @brief 车道扩展配置
 * @attention 无
 */
typedef struct tagMwDrivewayExpandCfg
{
    IMOS_MW_DRIVE_EXPAND_PARAM_S astDrivewayExpandParam[4];  /**< 车道扩展参数 */
    IMOS_MW_DRIVE_EXPAND_SERIAL_PARAM_S stDrivewayExpandSerialParam;                   /**< 串口号参数 */
    IMOS_MW_SING_DRIV_WAY_EXP_CFG_S astSingDrivWayExpCfg[4]; /**< 车道拓展配置参数 */
    ULONG aulPPort[4];                                       /**< 车道可配置对应大筒相机P口 */
}IMOS_MW_DRIVEWAY_EXPAND_CFG_S;

/**
* @struct tagMwIdentifyArea
* @brief 识别区域
* @attention
*/
typedef struct tagMwIdentifyArea
{
    ULONG ulAreaID;                         /**< 区域ID, 全F代表无效区域  */
    IMOS_MW_QUADRANGLE_S stIdentifyArea;    /**< 识别区域坐标 */
}IMOS_MW_IDENTIFY_AREA_S;

typedef struct tagMwPlateIdentifyCfg
{
    USHORT usMinLprWidthPix;                                            /**< 最小车牌检测宽度--以象素为单位 */
    USHORT usMaxLprWidthPix;                                            /**< 最大车牌检测宽度--以象素为单位 */
    ULONG ulAreaNum;                                                    /**< 识别区域个数 */
    IMOS_MW_IDENTIFY_AREA_S astIdentifyArea[4];  /**< 识别区域 */
    BOOL_T bNorthKoreaPlate;                                            /**< 是否支持朝鲜车牌 */
    BOOL_T bAgroPlate;                                                  /**< 是否支持农用车牌 */
    BOOL_T bVehicleLogo;                                                /**< 是否支持车标识别 */
    BOOL_T bVehicleColor;                                               /**< 是否支持车身颜色识别 */
    UCHAR ucDefaultProvince;                                            /**< 车牌缺省汉字 */
    UCHAR ucDefaultLetter;                                              /**< 车牌缺省字母 */
    UCHAR aucReserve[2];                                                /**< 保留:aucReserve[0] bit位
                                                                              0位:车牌矫正(0代表关，1代表开)，3位:大车识别,4位:遮阳板检测功能使能, 
                                                                              5位安全带检测, 6位:车型检测,7位使馆车牌 */
                                                                        /**< 保留:aucReserve[1] 此配置是否为"视频检测设置"页面下发，
                                                                             BOOL_TRUE:"视频检测设置"页面下发 */
    ULONG ulSpecialPlate;                                               /**< 特殊车牌 参见:IMOS_MW_PLATE_WG */
    CHAR szFilterPlateString[128];      /**< 异地牌照过滤字段，用/分割 */
    ULONG ulPlateParam;                                                 /**< 支持车牌识别参数，按位操作:0x01支持泥头车识别,不能按位 */
    ULONG ulCheckRepeatTime;                                            /**< 重复车牌过滤时间,单位 秒 */
    ULONG ulNewPlateParam;                                              /**< 新的车牌识别参数，按位操作，bit位0代表关，1代表开
                                                                             0位:摩托车识别，1位:白名单放行使能 */                                                                               
    ULONG ulMultiPlateIdentifyMode;                                     /**< 卡口出入口多帧识别模式, 参见:IMOS_MW_TG_MULTI_IDENTIFY_MODE_NO   */                                                                          
}IMOS_MW_PLATE_IDENTIFY_CFG_S;

/**
 * @struct tagMwTriggerLine
 * @brief 触发线信息
 * @attention
 */
typedef struct tagMwTriggerLine
{
    ULONG ulLineID;                         /**< 触发线ID */
    BOOL_T bEnable;                         /**< 是否使能 */
    ULONG ulLineType;                       /**< 触发线类型，参见: IMOS_MW_TRIGGER_LINE_DEFAULT1 等 */
    ULONG ulTriggerDelay;                   /**< 延时触发 */
    IMOS_MW_LINE_S stLine;                  /**< 触发线位置 */
}IMOS_MW_TRIGGERLINE_S;

/**
 * @struct tagMwTriggerLineCfg
 * @brief 触发线配置
 * @attention
 */
typedef struct tagMwTriggerLineCfg
{
    ULONG ulLineNum;                                                    /**< 触发线条数 */
    IMOS_MW_TRIGGERLINE_S astTriggerLine[IMOS_MW_TRIGGER_LINE_NUM];     /**< 触发线配置 */
}IMOS_MW_TRIGGERLINE_CFG_S;

/**
 * @struct tagMwDrivewayLine
 * @brief 车道线
 * @attention
 */
typedef struct tagMwDrivewayLine
{
    ULONG ulLineID;                           /**< 车道线ID */
    BOOL_T bEnable;                           /**< 车道线是否启用 */
    ULONG ulLineType;                         /**< 车道线类型, 参见: IMOS_MW_DRIVEWAY_LINE_SOLID_WHITE 等 */
    BOOL_T bEnableCoverDetect;                /**< 是否启用压线检测 */
    BOOL_T bDetectOverline;                   /**< 是否启用越线检测 */
    FLOAT fSensitivity;                       /**< 压线检测灵敏度，说明: 车牌宽度 * 压线检测灵敏度 = 车身宽度 */
    IMOS_MW_LINE_S stLine;                    /**< 位置坐标 */
}IMOS_MW_DRIVEWAY_LINE_S;

/**
 * @struct tagMwDrivewayLineCfg
 * @brief 车道线信息
 * @attention
 */
typedef struct tagMwDrivewayLineCfg
{
    ULONG ulLineNum;                                                             /**< 车道线数目 */
    IMOS_MW_DRIVEWAY_LINE_S astDrivewayLine[9];      /**< 车道线配置 */
}IMOS_MW_DRIVEWAYLINE_CFG_S;

/**
 * @union tagMwListMatchRule
 * @brief 名单匹配规则
 * @attention 
 */
typedef struct tagMwListMatchRule
{
    ULONG ulMatchMode;             /* 匹配模式 0 模糊匹配，1 精确匹配*/
    ULONG ulLetterFuzzyMatchLevel;  /* 字母数字模糊匹配等级,等级:0.1.2; 仅ulMatchMode为0生效 */
    BOOL_T bChineseIgnored;         /* 是否忽略汉字匹配; 仅ulMatchMode为0生效 */
}IMOS_MW_LIST_MATCH_RULE_CFG;

/**
* @struct tagMwGateWhiteList
* @brief 出入口白名单配置
* @attention
*/
typedef struct tagMwGateWhiteList
{
    BOOL_T bEnabled;       /* 是否使能名单 */
    IMOS_MW_LIST_MATCH_RULE_CFG stMatchRule;    /* 匹配规则 */
}IMOS_MW_GATE_WHITE_LIST_CFG_S;

/**
* @struct tagMwGateBlackList
* @brief 出入口黑名单配置
* @attention
*/

typedef struct tagMwGateBLACKList
{
    BOOL_T bEnabled;       /* 是否使能名单 */
    BOOL_T bLinkSwitch;    /* 是否联动开关量 */  
    ULONG ulLinkSwitchID;       /* 联动开关量ID */
    IMOS_MW_LIST_MATCH_RULE_CFG stMatchRule;    /* 匹配规则 */
}IMOS_MW_GATE_BLACK_LIST_CFG_S;

/**
 *@union tagMwReleaseStrategyCfg
 *@brief 放行策略
 *@attention
 */
typedef struct tagMwReleaseStrategyCfg
{    
    ULONG ulCtrlMode;                             /* 控制模式  0:服务器控制模式; 1:离线控制模式; 2:单机控制开闸 */
    ULONG ulReleaseTactics;                       /* 开关量控制模式,取值: 0  所有车辆放行 1 白名单车辆放行 2 非黑名单车辆放行 */ 
    BOOL_T bUnidentifiedRelease;                  /* 未识别车辆是否放行 */
    ULONG ulOutputSwitchID;                       /* 放行开关量 */
}IMOS_MW_RELEASE_STRATEGY_CFG_S;

#ifdef  __cplusplus
}
#endif

#endif /*end of __IMOS_SDK_PUB_H__*/




