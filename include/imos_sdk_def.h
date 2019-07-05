/*******************************************************************************
 Copyright (c) 2013,  Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_sdk_def.h
  Project Code: MW_SDK
   Module Name: SDK
  Date Created: 2012-01-14
        Author:  
   Description: 该文件定义了sdk相关的错误码、宏

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  2012-01-14

*******************************************************************************/
#ifndef __IMOS_SDK_DEF_H__
#define __IMOS_SDK_DEF_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#if 0
#endif
/*******************************************************************************
编译选项
*******************************************************************************/
#if defined(WIN32)
    #if defined(IMOS_EXPORTS_DLL)
        #define IMOS_EXPORT  __declspec( dllexport )
    #else
        #if defined(IMOS_AS_STATIC_LIB)
            #define IMOS_EXPORT
        #else
            #define IMOS_EXPORT  __declspec( dllimport )    
        #endif
    #endif
#else
    #define IMOS_EXPORT
#endif

/* 函数标准调用约定 */
#ifdef WIN32
    #define STDCALL         __stdcall
#else
    #ifdef i386
        #define STDCALL         __attribute__((stdcall))
    #else
        #define STDCALL
    #endif
#endif 

#if 0
#endif
/*******************************************************************************
通用数据类型定义
*******************************************************************************/
/** @brief 8位有符号整型 */
#ifndef CHAR
typedef char CHAR;
#endif

/** @brief 8位无符号整型 */
#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

/** @brief 16位有符号整型 */
#ifndef SHORT
typedef short SHORT;
#endif

/** @brief 16位无符号整型 */
typedef unsigned short USHORT;

/** @brief 32位有符号整型 */
typedef long LONG;

/** @brief 32位无符号整型 */
typedef unsigned long ULONG;

/** @brief 有符号整型 */
typedef int INT;

/** @brief 无符号整型 */
typedef unsigned int UINT;

/** @brief 64位有符号整型 */
#if defined (_MSC_VER)
typedef __int64 DLONG;
#else
typedef long long DLONG;
#endif

/** @brief 64位无符号整型 */
#if defined (_MSC_VER)
typedef unsigned __int64 DULONG;
#else
typedef unsigned long long DULONG;
#endif

/** @brief 双精度数据类型 */
typedef double DOUBLE;

/** @brief 浮点数据类型 */
typedef float FLOAT;

/** @brief 布尔型 */
typedef unsigned long BOOL_T;

/** @brief 表示长度数据类型 */
typedef unsigned long SIZE_T;

/** @brief 表示时间数据类型 */
typedef long TIME_T;

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;

/** @brief VOID型 */
#define VOID                            void

/** @brief 表示通用句柄类型 */
typedef VOID* HANDLE;

/** @brief IMOS ID 数据类型 */
typedef DULONG IMOS_ID;

/** @brief 通用句柄类型 IMOS_HANDLE */
#ifndef IMOS_HANDLE
#if defined (WIN32)
typedef HANDLE IMOS_HANDLE;
#else
typedef int IMOS_HANDLE;
#endif
#endif

/** @brief 函数参数类型定义 */
#define IN
#define OUT
#define INOUT

/** @brief IMOS编程规范布尔值 */
#define BOOL_TRUE                       ((BOOL_T) 1)
#define BOOL_FALSE                      ((BOOL_T) 0)

#ifndef NULL
#define NULL (void *)0
#endif

#if 0
#endif
/*******************************************************************************
SDK 错误码/状态码
*******************************************************************************/
/* 通用成功 */
#define ERR_COMMON_SUCCEED                       0x000000      /**< 执行成功 */

/*********************** SDK 状态码定义(257 - 356) *****************************/
#define ERR_SDK_LOG_CLOSE                        257            /**< SDK 日志关闭 */
#define ERR_SDK_DEVICE_STREAM_FULL               259            /**< 设备流已满 */
#define ERR_SDK_DEVICE_STREAM_CLOSED             260            /**< 设备流已关闭 */
#define ERR_SDK_DEVICE_STREAM_NONEXIST           261            /**< 设备流不存在 */
#define ERR_SDK_USER_PASSWORD_CHANGE             262            /**< 用户密码修改 */
#define ERR_SDK_USER_DELETED                     263            /**< 用户已删除 */

/*********************** SDK 错误码定义(357 - 506) *****************************/
#define ERR_SDK_COMMON_FAIL                      357            /**< 操作失败 */
#define ERR_SDK_COMMON_INVALID_PARAM             358            /**< 输入参数非法 */
#define ERR_SDK_COMMON_NO_MEMORY                 359            /**< 系统内存不足 */
#define ERR_SDK_COMMON_SYS_FAIL                  360            /**< 系统通用错误，Q: 暂未使用 */
#define ERR_SDK_COMMON_FAIL_STAT                 361            /**< 读取文件（目录）状态失败 */
#define ERR_SDK_COMMON_FILE_NONEXIST             362            /**< 文件不存在 */

#define ERR_SDK_USER_NOT_AUTHORIZED              457            /**< 用户未授权 */
#define ERR_SDK_USERFULL                         458            /**< 用户已满 */
#define ERR_SDK_USERNONEXIST                     459            /**< 用户不存在 */
#define ERR_SDK_USER_PASSWD_INVALID              460            /**< 用户密码错误 */
#define ERR_SDK_USER_KEEPALIVEFAIL               461            /**< SDK保活失败 */
#define ERR_SDK_NOTINIT                          462            /**< SDK未初始化 */
#define ERR_SDK_REINIT                           463            /**< SDK已初始化 */

#define ERR_SDK_CREATE_THREAD_FAIL               466            /**< 创建线程失败 */
#define ERR_SDK_NOENOUGH_BUF                     467            /**< 缓冲区太小: 接收设备数据的缓冲区 */
#define ERR_SDK_SOCKET_LSN_FAIL                  468            /**< 创建socket listen失败 */
#define ERR_SDK_SUBSCRIBE_FULL                   470            /**< 本用户订阅已满, 设备侧用户订阅已满的错误码不能返回 */
#define ERR_SDK_COMMAND_SEND_FAIL                471            /**< 请求发送失败 */
#define ERR_SDK_COMMAND_TIMEOUT                  472            /**< 请求超时 */
#define ERR_SDK_UPDATE_NOTADMIN                  473            /**< 非admin用户无法升级 */
#define ERR_SDK_UPDATE_INVALID                   474            /**< 升级 未开始 */
#define ERR_SDK_UPDATE_INPROCESS                 475            /**< 升级 处理中 */
#define ERR_SDK_UPDATE_NOMEMORY                  476            /**< 升级 内存空间不够 */
#define ERR_SDK_UPDATE_FILE_OPEN_ERR             477            /**< 升级 打开镜像文件出错 */
#define ERR_SDK_UPDATE_DEVICE_ERR                478            /**< 升级 FLASH出错 */
#define ERR_SDK_UPDATE_BUSY                      479            /**< 不能同时加载多个升级进程 */
#define ERR_SDK_UPDATE_FAIL_TIMEOUT              480            /**< 升级超时 */

#define ERR_SDK_IP_CONFLICT                      486            /**< IP地址冲突 */
#define ERR_SDK_NETWORKMODE_NOTSUPPORT           487            /**< 网口模式不支持 */


/*********************** SDK 业务模块错误码定义(507-656) *******************/
/* 设备维护相关(507 - 556) */
#define ERR_SDK_INVALID_CONFIGFILE               507            /**< 配置文件无效 */


/* 媒体相关(557 - 606) */
#define ERR_SDK_BIAUDIO_AUDIOBCAST_FULL          557            /**< 语音业务已满 */
#define ERR_SDK_STOR_RESOURCE_NOTINIT            558            /**< 存储资源未分配 */


/* 云台相关(607 - 656) */
#define ERR_SDK_PTZ_TRACK_ISUSED                 612            /**< 轨迹已使用，无法删除*/
#define ERR_SDK_PTZ_SERIALMODE_MISMATCH          614            /**< 串口模式不匹配*/
#define ERR_SDK_PTZ_TRACK_NOT_EXIST              618            /**< 轨迹不存在*/
#define ERR_SDK_PTZ_MODE_CRUISE_FULL             626            /**< 模式路径轨迹点个数已满 */

/*********************** SDK 播放器错误码定义(657 - 756) ****************************/
#define ERR_SDK_XP_INIT_FAILED                   657            /**< 播放器初始化失败 */
#define ERR_SDK_XP_PORT_ALLOC_FAILED             658            /**< 播放器通道分配失败 */
#define ERR_SDK_XP_PORT_NOT_EXIST                659            /**< 播放器通道不存在 */
#define ERR_SDK_XP_START_STREAM_FAILED           660            /**< 播放器启流失败 */
#define ERR_SDK_XP_START_PLAY_FAILED             662            /**< 开始播放失败 */
#define ERR_SDK_XP_FILE_PLAY_FINISHED            669            /**< 文件已播放完 */
#define ERR_SDK_XP_DISK_CAPACITY_WARN            670            /**< 硬盘剩余空间低于阈值 */
#define ERR_SDK_XP_DISK_CAPACITY_NOT_ENOUGH      671            /**< 硬盘剩余空间不足 */
#define ERR_SDK_XP_NO_PICTURE_TO_SNATCH          672            /**< 没有解码过的图片可供抓拍 */
#define ERR_SDK_XP_SERIES_SNATCH_FAILED          673            /**< 连续抓拍过程中抓拍失败 */
#define ERR_SDK_XP_WRITE_FILE_FAILED             674            /**< 写文件操作失败 */
#define ERR_SDK_XP_FILE_DESTROY                  675            /**< 文件已损坏 */
#define ERR_SDK_XP_NOT_SUPPORT_MEDIA_ENCODE_TYPE 676            /**< 媒体编码格式不支持录像操作 */
#define ERR_SDK_XP_PROCESS_MEDIA_DATA_FAILED     677            /**< 媒体数据处理失败 */
#define ERR_SDK_XP_RECV_DATA_FAILED              678            /**< 网络故障造成接收数据失败 */
#define ERR_SDK_XP_MEDIA_RESOLUTION_CHANGE       679            /**< 媒体流分辨率发生变化 */
#define ERR_SDK_XP_VOICE_RUNNING_ERROR           680            /**< 语音对讲或广播过程中出错 */
#define ERR_SDK_XP_AUDIO_DEVICE_UNRIPE           682            /**< 音频设备未准备好 */

/*******************************************************************************
SDK 宏定义 
*******************************************************************************/
#if 0
#endif
/**************************************** 设备状态相关 ***************************************/
#define IMOS_MW_STATUS_KEEPALIVE                   0            /**< SDK保活，状态码为: ERR_SDK_USER_KEEPALIVEFAIL */
#define IMOS_MW_STATUS_UPDATE                      1            /**< 升级结果，状态码为: ERR_SDK_UPDATE_INVALID 等 */

#define IMOS_MW_STATUS_USERINFO_CHANGE             3            /**< 用户信息修改, 状态码为: ERR_SDK_USER_PASSWORD_CHANGE 等 */ 
    
#define IMOS_MW_STATUS_MANAGE_SERVER_ONLINE        50           /**< 管理服务器的连接状态，对应参数BOOL_T: ON:1  OFF:0 */                  
#define IMOS_MW_STATUS_PHOTO_SERVER_ONLINE         51           /**< 照片服务器的连接状态，对应参数BOOL_T: ON:1  OFF:0 */     
#define IMOS_MW_STATUS_BASIC_INFO                  52           /**< 设备基本信息，对应参数类型: IMOS_MW_BASIC_DEVICE_INFO_S */              
#define IMOS_MW_STATUS_RUN_INFO                    53           /**< 设备运行状态，对应参数类型: IMOS_MW_DEVICE_RUN_INFO_S */  
#define IMOS_MW_STATUS_NETWORK_CHANGE              54           /**< 网口配置结果，状态码为: ERR_SDK_IP_CONFLICT 等 */
#define IMOS_MW_STATUS_STOR_MEMORY_CARD_FORMAT     90           /**< 本地存储设备格式化状态， 状态码为: ERR_COMMON_SUCCEED等 */  
#define IMOS_MW_STATUS_STOR_NAS_MOUNT              91           /**< NAS挂载状态 状态码为: ERR_COMMON_SUCCEED等 */  
   
#define IMOS_MW_STATUS_RADAR                       150          /**< 雷达状态  对应参数: IMOS_MW_RADAR_XXX */                           
#define IMOS_MW_STATUS_COIL                        151          /**< 线圈状态  信息内容为: IMOS_MW_COIL_STATE_S 的数组，支持 8个*/          
#define IMOS_MW_STATUS_POLARIZER                   152          /**< 偏振镜状态  对应参数: IMOS_MW_POLARIZER_STATUS_XXX */                     
#define IMOS_MW_STATUS_LED_STROBE                  153          /**< LED灯状态  对应参数: IMOS_MW_LED_STROBE_STATUS_XXX */ 
#define IMOS_MW_STATUS_ND_FILTER                   154          /**< ND滤镜状态  对应参数: IMOS_MW_POLARIZER_STATUS_INUSE */
#define IMOS_MW_STATUS_TRAFFICLIGHT                155          /**< 信号灯实时在线状态  对应参数: IMOS_MW_TRAFFICLIGHT_STATUS_XXX */
#define IMOS_MW_STATUS_SD                          156          /**< SD卡状态  对应参数: IMOS_MW_SD_STATUS_NOEXIST */

#define IMOS_MW_STATUS_TRAFFIC_PARAM_REPORT        169          /**< 交通参数上报 对应参数:IMOS_MW_TRAFFIC_PARA_RSLT_S */
#define IMOS_MW_STATUS_VEHICLE_PARAM_REPORT        170          /**< 车辆进出状态上报 对应参数:IMOS_MW_VEHICLE_STATE_S */

#define IMOS_MW_STATUS_PLAYER_RECORD_VIDEO         200          /**< 本地录像过程中上报运行信息，信息内容见: IMOS_MW_PLAYER_STATUS_S */
#define IMOS_MW_STATUS_PLAYER_MEDIA_PROCESS        201          /**< 视频媒体处理过程中的上报运行信息，信息内容见: IMOS_MW_PLAYER_STATUS_S */
#define IMOS_MW_STATUS_PLAYER_SERIES_SNATCH        202          /**< 连续抓拍过程中上报运行信息，信息内容见: IMOS_MW_PLAYER_STATUS_S */

/**@brief 设备工作模式 */
#define IMOS_MW_DEVICE_WORK_MODE_TOLLGATE          0             /**< 卡口 */
#define IMOS_MW_DEVICE_WORK_MODE_ELEC_POLICE       1             /**< 电警 */

/**@brief 雷达状态 */
#define IMOS_MW_RADAR_STATUS_DISCONNECT            0             /**< 雷达未连接 */
#define IMOS_MW_RADAR_STATUS_CONNECT               1             /**< 雷达连接 */
    
/**@brief 线圈状态 */
#define IMOS_MW_COIL_STATUS_DISCONNECT             0             /**< 线圈断开 */
#define IMOS_MW_COIL_STATUS_CONNECT                1             /**< 线圈连接 */
#define IMOS_MW_COIL_STATUS_UNUSE                  2             /**< 线圈未使用 */
    
/**@brief 偏振镜状态 */
#define IMOS_MW_POLARIZER_STATUS_INUSE             0             /**< 正在使用 */
#define IMOS_MW_POLARIZER_STATUS_UNUSE             1             /**< 未使用 */
#define IMOS_MW_POLARIZER_STATUS_DOWNING           2             /**< 正在下降 */
#define IMOS_MW_POLARIZER_STATUS_UPING             3             /**< 正在上升 */
#define IMOS_MW_POLARIZER_STATUS_UNKNOWN           4             /**< 异常 */
#define IMOS_MW_POLARIZER_STATUS_SWITCHING         5             /**< 切换中 */

/**@brief 交通灯检测器实时在线状态 */
#define IMOS_MW_TRAFFICLIGHT_STATUS_OFFLINE        0             /**< 离线 */
#define IMOS_MW_TRAFFICLIGHT_STATUS_ONLINE         1             /**< 在线 */
#define IMOS_MW_TRAFFIClIGHT_STATUS_UNUSED         2             /**< 未使用 */

/**@brief LED灯状态 */
#define IMOS_MW_LED_STROBE_STATUS_ON               0              /**< 打开 */
#define IMOS_MW_LED_STROBE_STATUS_OFF              1              /**< 关闭 */

/**@brief SD卡状态 */
#define IMOS_MW_SD_STATUS_NOEXIST                  0             /**< 不存在 */
#define IMOS_MW_SD_STATUS_FAULT                    1             /**< 故障 */
#define IMOS_MW_SD_STATUS_CHECKING                 2             /**< 检测中 */
#define IMOS_MW_SD_STATUS_NORMAL                   3             /**< 正常 */

/**@brief 服务器管理模式 */
#define IMOS_MW_MANAGE_SERVER_PROTOCOL_IMOS         0             /**< 管理协议为IMOS */
#define IMOS_MW_MANAGE_SERVER_PROTOCOL_GB           1             /**< 管理协议为GB */
#define IMOS_MW_MANAGE_SERVER_PROTOCOL_NONE         2             /**< 管理协议为无 */

/**@brief 抓拍照片类型 */
#define IMOS_MW_CAPTURE_AUTO_TYPE                   0             /**< 自动抓拍，包括外设抓拍、绊线触发 */
#define IMOS_MW_CAPTURE_MANUAL_TYPE                 1             /**< 手动抓拍 */

/**@brief 交通参数服务器上报车辆特征数量 */ 
#define IMOS_MW_CAR_TYPE_NUM                         12           /**< 车辆类型数目 */
#define IMOS_MW_LPR_COLOR_NUM                        5            /**< 车牌颜色数目 */
#define IMOS_MW_CAR_SPEED_NUM                        2            /**< 车速数目 */

#if 0
#endif
/*************************************** 配置相关 ********************************************/
/**@brief 配置操作命令字 */
/* 系统配置:  版本类型 、管理模式、缓存补录 */
#define IMOS_MW_SYSTEM_TIME_INFO            0          /**< 获取/设置 系统时间(含时区信息)，对应结构定义: IMOS_MW_SYSTEM_TIME_INFO_S */
#define IMOS_MW_SYSLOG_SERVER_INFO          1          /**< 获取/设置 syslog服务器信息，对应结构定义: IMOS_MW_SYSLOG_SERVER_INFO_S */ 
#define IMOS_MW_MANAGE_SERVER_CFG           3          /**< 获取/设置 管理服务器配置，对应结构定义: IMOS_MW_MANAGE_SERVER_CFG_S */
#define IMOS_MW_DEVICE_INSATLLATION_INFO    5          /**< 获取/设置 设备安装信息，参见: IMOS_MW_DEVICE_INSATLLATION_INFO_S */
#define IMOS_MW_BM_SERVER_CFG               6          /**< 获取/设置 BM服务器配置，对应结构定义: IMOS_MW_BM_SERVER_CFG_S */
#define IMOS_MW_DEVICE_NAME_CFG             7          /**< 获取/设置 设备名称配置，对应结构定义: IMOS_MW_DEVICE_NAME_CFG_S */
#define IMOS_MW_NTP_SERVER_CFG              8          /**< 获取/设置 NTP服务器配置，对应结构定义: IMOS_MW_NTP_SERVER_CFG_S  */
#define IMOS_MW_DCOUT_CFG                   9          /**< 获取/设置 电源输出配置，对应结构定义: IMOS_MW_DCOUT_CFG_S */

/* 网口、串口、透明通道 */
#define IMOS_MW_SERIAL_CFG                  10         /**< 获取/设置 串口配置，对应结构定义: IMOS_MW_SERIAL_CFG_S */
#define IMOS_MW_TRANSPORT_CFG               11         /**< 获取/设置 透明通道配置，对应结构定义: IMOS_MW_TRANSPORT_CFG_S */
#define IMOS_MW_NETWORK_INTERFACE_CFG       12         /**< 获取/设置 网口配置，对应结构定义: IMOS_MW_NETWORK_INTERFACE_CFG_S */
#define IMOS_MW_UNP_CFG                     13         /**< 获取/设置 UNP配置，对应结构定义: IMOS_MW_UNP_CFG_S */ 
#define IMOS_MW_WIFI_CFG                    14         /**< 获取/设置 WiFi连接配置 对应结构定义: IMOS_MW_WIFI_CFG_S */

#define IMOS_MW_MULTI_TRANSPORT_CFG         2005      /**< 获取/设置 多透明通道配置，对应结构定义: IMOS_MW_MULTI_TRANSPORT_CFG_S */    
     
/* 图像参数配置 */
#define IMOS_MW_EXPOSURE_CFG                20         /**< 获取/设置 曝光参数，对应结构定义: IMOS_MW_EXPOSURE_CFG_S */
#define IMOS_MW_EXPOSURE_CFG_VIDEO          507        /**< 获取/设置 曝光视频参数，对应结构定义: IMOS_MW_EXPOSURE_CFG_VIDEO */
#define IMOS_MW_EXPOSURE_CFG_IMAGE          508        /**< 获取/设置 曝光图像参数，对应结构定义: IMOS_MW_EXPOSURE_CFG_IMAGE */

#define IMOS_MW_WHITE_BALANCE_CFG           21         /**< 获取/设置 白平衡参数，对应结构定义: IMOS_MW_WHITE_BALANCE_CFG_S */
#define IMOS_MW_IMAGE_ENHANCE_CFG           22         /**< 获取/设置 图像增强参数，对应结构定义: IMOS_MW_IMAGE_ENHANCE_CFG_S */
#define IMOS_MW_FOCUS_CFG                   23         /**< 获取/设置 对焦参数，对应结构定义: IMOS_MW_FOCUS_CFG_S */

/* 视频编码、音频编解码参数、OSD */
#define IMOS_MW_VIDEOIN_MODE_CFG            30         /**< 获取/设置 视频编码制式，对应结构定义: IMOS_MW_VIDEOIN_MODE_CFG_S */
#define IMOS_MW_VIDEO_ENCODER_CFG           31         /**< 获取/设置 视频编码参数配置，对应结构定义:IMOS_MW_VIDEO_ENCODER_CFG_S */
#define IMOS_MW_AUDIO_IN_CFG                32         /**< 获取/设置 音频输入参数配置，对应结构定义:IMOS_MW_AUDIO_IN_CFG_S */
#define IMOS_MW_OSD_STYLE_CFG               35         /**< 获取/设置 叠加OSD样式配置，对应结构定义: IMOS_MW_OSD_STYLE_CFG_S */
#define IMOS_MW_INFO_OSD_CFG                36         /**< 获取/设置/删除 叠加OSD配置，对应结构定义: IMOS_MW_INFO_OSD_CFG_S */
    
/* 卡口相关: 基本配置、智能配置 */
#define IMOS_MW_PHOTO_SERVER_CFG            61         /**< 获取/设置 照片接收服务器信息，对应结构定义: IMOS_MW_PHOTO_SERVER_CFG_S */
#define IMOS_MW_FLASH_LIGHT_CFG             62         /**< 获取/设置 闪光灯配置，对应结构定义: IMOS_MW_FLASH_LIGHT_CFG_S */
#define IMOS_MW_VEHICLE_DETECTOR_CFG        63         /**< 获取/设置 车检器配置，对应结构定义: IMOS_MW_VEHICLE_DETECTOR_CFG_S */
#define IMOS_MW_RADAR_CFG                   64         /**< 获取/设置 雷达配置，对应结构定义: IMOS_MW_RADAR_CFG_S */
#define IMOS_MW_CAMERA_CAPTURE_CFG          65         /**< 获取/设置 抓拍配置，对应结构定义: IMOS_MW_CAMERA_CAPTURE_CFG_S */
#define IMOS_MW_VIDEO_DETECT_CFG            66         /**< 获取/设置 视频检测配置，对应结构定义: IMOS_MW_VIDEO_DETECT_CFG_S */
#define IMOS_MW_PLATE_IDENTIFY_CFG          67         /**< 获取/设置 车牌识别配置，对应结构定义: IMOS_MW_PLATE_IDENTIFY_CFG_S */
#define IMOS_MW_DRIVEWAY_CFG                68         /**< 获取/设置 车道配置，对应结构定义: IMOS_MW_DRIVEWAY_CFG_S */
#define IMOS_MW_SUBDEVICE_SWITCH_CFG        69         /**< 获取/设置 外设开关配置，对应结构定义: IMOS_MW_SUBDEVICE_SWITCH_CFG_S */
#define IMOS_MW_LASER_CFG                   70         /**< 获取/设置 激光配置，对应结构定义: IMOS_MW_LASER_CFG_S */
#define IMOS_MW_CAPTURE_FORMAT_CFG          71         /**< 获取/设置 抓拍图片参数配置，对应结构定义: IMOS_MW_CAPTURE_FORMAT_CFG_S */
#define IMOS_MW_POLARIZER_CFG               72         /**< 获取/设置 偏振镜配置，对应结构定义: IMOS_MW_POLARIZER_CFG_S */
#define IMOS_MW_ND_FILTER_CFG               73         /**< 获取/设置 ND滤镜配置，对应结构定义: IMOS_MW_ND_FILTER_CFG_S */

/* 电警相关: 视频源配置、智能配置、违章配置 */
#define IMOS_MW_CAMERA_BASIC_CFG            101        /**< 获取/设置 摄像机基本配置, 对应结构定义: IMOS_MW_CAMERA_BASIC_CFG_S */ 
#define IMOS_MW_EP_VIDEODETECT_CFG          102        /**< 获取/设置 视频检测配置，对应结构定义: IMOS_MW_EP_VIDEODETECT_CFG_S */
#define IMOS_MW_EP_DRIVEWAY_CFG             103        /**< 获取/设置 车道配置，对应结构定义: IMOS_MW_EP_DRIVEWAY_CFG_S */
#define IMOS_MW_DETECTAREA_CFG              104        /**< 获取/设置 检测区域配置, 对应结构定义: IMOS_MW_DETECTAREA_CFG_S */ 
#define IMOS_MW_TRIGGERLINE_CFG             105        /**< 获取/设置 触发线配置, 对应结构定义: IMOS_MW_TRIGGERLINE_CFG_S */ 
#define IMOS_MW_DRIVEWAYLINE_CFG            106        /**< 获取/设置 车道线配置, 对应结构定义: IMOS_MW_DRIVEWAYLINE_CFG_S */
#define IMOS_MW_TRAFFICLIGHT_CFG            107        /**< 获取/设置 红绿灯配置, 对应结构定义: IMOS_MW_TRAFFICLIGHT_CFG_S */ 
#define IMOS_MW_VEHICLETRACK_CFG            108        /**< 获取/设置 车辆跟踪配置, 对应结构定义: IMOS_MW_VEHICLETRACK_CFG_S */
#define IMOS_MW_TOLLGATE_CFG                109        /**< 获取/设置 卡口抓拍配置, 对应结构定义: IMOS_MW_TOLLGATE_CFG_S */
#define IMOS_MW_PECCANCY_CAPTURE_CFG        110        /**< 获取/设置 违章抓拍配置, 对应结构定义: IMOS_MW_PECCANCY_CAPTURE_CFG_S */
#define IMOS_MW_CAPTURE_IMGPROC_CFG         111        /**< 获取/设置 违章抓拍图片处理配置, 对应结构定义: IMOS_MW_CAPTURE_IMGPROC_CFG_S */
#define IMOS_MW_TRAFFICLIGHT_INTENSIFY_CFG  112        /**< 获取/设置 红绿灯加强配置, 对应结构定义: IMOS_MW_TRAFFICLIGHT_INTENSIFY_S */
/* 存储相关: 存储配置 */
#define IMOS_MW_LOCAL_STOR_INFO             150         /**< 获取/设置 本地存储信息，对应结构定义: IMOS_MW_LOCAL_STOR_INFO_S */
#define IMOS_MW_STOR_NAS_CFG                151         /**< 获取/设置 存储类型信息，参见：IMOS_MW_NAS_STOR_CFG_S */      
#define IMOS_MW_FTP_CFG                     152         /**< 获取/设置 FTP配置，参见：IMOS_MW_FTP_CFG_S */ 

#define IMOS_MW_DRIVEWAY_EXPAND_CFG         506         /**< 获取/设置 车道扩展配置，对应结构定义: IMOS_MW_DRIVEWAY_EXPAND_CFG_S */

#define IMOS_MW_RELEASE_STRATEGY_CFG        600       /**< 获取/设置 黑白名单放行策略，对应结构定义: IMOS_MW_RELEASE_STRATEGY_CFG_S */
#define IMOS_MW_WHITE_LIST_CFG              601       /**< 获取/设置 出入口白名单配置，对应结构定义: IMOS_MW_GATE_WHITE_LIST_CFG_S */
#define IMOS_MW_BLACK_LIST_CFG              602       /**< 获取/设置 出入口黑名单配置，对应结构定义: IMOS_MW_GATE_BLACK_LIST_CFG_S*/

#if 0
#endif
/*************************************** 设备维护相关 ********************************************/
/*@brief 通用CODE长度*/
#define IMOS_MW_CODE_LEN                    48   

/*@brief 通用名称字符串长度 */
#define IMOS_MW_NAME_LEN                    64     

/*@brief 文件名称字符串长度 */
#define IMOS_MW_FILE_NAME_LEN               IMOS_MW_NAME_LEN    

/*@brief 资源编码信息字符串长度 */
#define IMOS_MW_RES_CODE_LEN                IMOS_MW_CODE_LEN

/*@brief 系统默认路径长度 */
#define IMOS_MW_DEFAULT_PATH_LEN            256

/*@brief 系统最大文件名长度 */
#define IMOS_MW_FILE_NAME_LEN_MAX          256
/*@brief SDK 客户端版本号长度 */
#define IMOS_MW_SDK_CLIENT_VERSION_LEN      32

/*@brief IP地址信息字符串长度 
  * IPV4: "192.168.0.102"
  * IPV6: "A1:0:8:100:200:0:0:321A"
  * 域名: "AS_server_hangzhou" */
#define IMOS_MW_IPADDR_LEN                  64  

/*@brief 注册VM时设备ID 长度  */
#define IMOS_MW_DEVICE_ID_LEN               48
/*@brief 注册管理服务器密码 长度  */
#define IMOS_MW_MANAGE_SERVER_PASSWD_LEN    16

/*@brief SNMP 团体名长度  */
#define IMOS_MW_COMMUNITY_LEN               32

/*@brief 时间 格式 */
#define IMOS_MW_TIME_FORMAT_HHMMSS          0        /**< XX:XX:XX 常见于中国，美国，英国等*/
#define IMOS_MW_TIME_FORMAT_HHMMSS_PM       1        /**< XX:XX:XX p.m. 常见于除西班牙以外的西班牙语国家*/
#define IMOS_MW_TIME_FORMAT_HH_MM_SS_PM     2        /**< XX:XX:XX PM 常见于除英国以外的英语国家*/
#define IMOS_MW_TIME_FORMAT_PM_HHMMSS       3        /**< PM XX:XX:XX 常见于中文(新加坡)*/
#define IMOS_MW_TIME_FORMAT_HH_MM_SS        4        /**< 下午 XX:XX:XX 常见于中文(台湾)*/

/*@brief 日期 格式 */
#define IMOS_MW_DATE_FORMAT_YYYYMMDD        0        /**< XXXX-XX-XX 年月日 */
#define IMOS_MW_DATE_FORMAT_MMDDYYYY        1        /**< XX-XX-XXXX 月日年 */
#define IMOS_MW_DATE_FORMAT_YYYY_MM_DD      2        /**< XXXX年XX月XX日 */
#define IMOS_MW_DATE_FORMAT_MM_DD_YYYY      3        /**< XX月XX日XXXX年 */
#define IMOS_MW_DATE_FORMAT_YYYY_MM_DD_XX   4        /**< XXXX年XX月XX日 星期X */
/*@brief 版本信息长度 */
#define IMOS_MW_VERSION_LEN                 256 

/*@brief 用户名最大长度 */
#define IMOS_MW_USER_NAME_LEN               32   

/*@brief 用户密码最大长度 */
#define IMOS_MW_USER_PASSWORD_LEN           32  

/**@brief 用户级别 */
#define IMOS_MW_USERLEVEL_ADMINISTRATOR     0        /**< 管理员 */
#define IMOS_MW_USERLEVEL_OPERATOR          1        /**< 操作员 */
#define IMOS_MW_USERLEVEL_USER              2        /**< 普通用户 */
#define IMOS_MW_USERLEVEL_ANONYMOUS         3        /**< 匿名用户 */
#define IMOS_MW_USERLEVEL_EXTENDED          4        /**< 扩展用户 */

/**@brief 参数类型 */
#define IMOS_MW_PARAM_TYPE_OPTICS_VIDEO     0        /**< 视频的图像参数 */
#define IMOS_MW_PARAM_TYPE_OPTICS_PICTURE   1        /**< 照片的图像参数 */
#define IMOS_MW_PARAM_TYPE_PECCANCY         2        /**< 违章默认参数 */

/**@brief 日志级别 */
#define IMOS_SDK_LOG_CLOSE                  0        /**< 关闭日志 */
#define IMOS_SDK_LOG_DEBUG                  1        /**< debug级别 */
#define IMOS_SDK_LOG_INFO                   2        /**< info级别 */
#define IMOS_SDK_LOG_WARN                   3        /**< warn级别 */
#define IMOS_SDK_LOG_ERROR                  4        /**< error级别 */
#define IMOS_SDK_LOG_FATAL                  5        /**< fatal级别 */

#if 0
#endif
/**************************************** 串口相关 ***************************************/
/**@brief 串口类型 */
#define IMOS_MW_SERIAL_TYPE_RS232                    1             /**< rs232 */
#define IMOS_MW_SERIAL_TYPE_RS422                    2             /**< rs422 */
#define IMOS_MW_SERIAL_TYPE_RS485                    3             /**< rs485 */

/**@brief 串口模式 */
#define IMOS_MW_SERIAL_MODE_PTZ                      1             /**< 云台控制 */
#define IMOS_MW_SERIAL_MODE_TRANS                    2             /**< 透明通道 */
#define IMOS_MW_SERIAL_MODE_CONSOLE                  3             /**< 控制台 */
#define IMOS_MW_SERIAL_MODE_COMMON                   4             /**< 通用串口 */
#define IMOS_MW_SERIAL_MODE_VEHICLE_DETECTOR_H       5             /**< 车检器 H, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_VEHICLE_DETECTOR_S       6             /**< 车检器 S, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_RADAR_C                  7             /**< 雷达 C, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_LASER                    8             /**< 激光, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_CASEALARM                9             /**< 机箱告警 */  
#define IMOS_MW_SERIAL_MODE_TRAFFICLIGHT_S           10            /**< 红灯信号检测器 S */  
#define IMOS_MW_SERIAL_MODE_RADAR_A                  11            /**< 雷达 A, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_OSD                      12            /**< OSD叠加 */
#define IMOS_MW_SERIAL_MODE_RADAR_H                  13            /**< 雷达 H, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_VEHICLE_DETECTOR_Q       14            /**< 车检器 Q, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_RADAR_C_CONTROLLER       15            /**< 雷达 C控制器, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_LOCAL_PTZ_CTRL           16            /**< 本地云台控制 */
#define IMOS_MW_SERIAL_MODE_RFID                     17            /**< RFID, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_VEHICLE_DETECTOR_U       18            /**< 车检器 U, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_TRAFFICLIGHT_U           19            /**< 红灯信号检测器 U */
#define IMOS_MW_SERIAL_MODE_RADAR_W                  20            /**< 雷达 W, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_RADAR_Z                  21            /**< 雷达 Z, 使用模式参见: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_SWITCHVALUE_TO_485            22            /**< 外设开关量转485 */
#define IMOS_MW_SERIAL_MODE_COUNT                    22            /**< 串口模式最大个数 */ 

/*************************************** 网口相关 ********************************************/
/*@brief PPPoE用户名长度  */
#define IMOS_MW_PPPOE_USERNAME_LEN               32  

/*@brief PPPoE密码长度 */
#define IMOS_MW_PPPOE_PASSWORD_LEN               32

/*@brief UNP用户名长度  */
#define IMOS_MW_UNP_USERNAME_LEN                 32  

/*@brief UNP密码长度 */
#define IMOS_MW_UNP_PASSWORD_LEN                 32

/*@brief WIFI SSID长度 */
#define IMOS_MW_WIFI_SSID_LEN                    44

/*@brief WIFI KEY长度 */
#define IMOS_MW_WIFI_KEY_LEN                     44

/* 一次扫描最多支持的无线网络个数 */
#define IMOS_MW_WIFI_NETWORK_MAX_COUNT           40


/**@brief IP获取方式 */
#define IMOS_MW_IP_GET_TYPE_STATIC               0        /**< 静态获取 */
#define IMOS_MW_IP_GET_TYPE_PPPOE                1        /**< pppoe分配 */
#define IMOS_MW_IP_GET_TYPE_DHCP                 2        /**< DHCP分配 */

/**@brief 网口类型 */
#define IMOS_MW_PORT_MODE_ELECTRONIC             0        /**< 电口 */
#define IMOS_MW_PORT_MODE_FIBER                  1        /**< 光口 */
#define IMOS_MW_PORT_MODE_EPON                   2        /**< EPON */
#define IMOS_MW_PORT_MODE_LRE                    3        /**< LRE电口 */

/**@brief 网口工作模式 */
#define IMOS_MW_ETH_WORKMODE_AUTO_NEGOTIATION    0    /**< 自协商 */
#define IMOS_MW_ETH_WORKMODE_10M_FULL            1    /**< 10M FULL */
#define IMOS_MW_ETH_WORKMODE_10M_HALF            2    /**< 10M HALF */
#define IMOS_MW_ETH_WORKMODE_100M_FULL           3    /**< 100M FULL */
#define IMOS_MW_ETH_WORKMODE_100M_HALF           4    /**< 100M HALF */
#define IMOS_MW_ETH_WORKMODE_1000M_FULL          5    /**< 1000M FULL */

/*@brief 最大轨迹点个数*/
#define IMOS_MW_TRACK_POINT_NUM                64      

/*@brief 预置位描述信息最大长度 */ 
#define IMOS_MW_PTZ_PRESET_NAME                64  

/*@brief 云台协议名称长度 */ 
#define IMOS_MW_PTZ_NAME_LEN                   16

/*@brief 计划包括四个时间段，巡航计划、布防计划等 */ 
#define IMOS_MW_TIME_SECTION_NUM               4  

/*@brief 时间字符串长度 */ 
#define IMOS_MW_TIME_LEN                       12    

/*@brief 日期字符串长度 */ 
#define IMOS_MW_DAY_LEN                        12   

/*@brief 周的天数 */
#define IMOS_MW_WEEK_DAY                       7

/*@brief 扩展计划的最大天数 */
#define IMOS_MW_EXPLAN_DAYS                    64

#if 0
#endif
/*************************************** 图像相关 ********************************************/
/**@brief 图像类型 */
#define IMOS_MW_IMAGE_TYPE_VIDEO                0       /**< 视频图像 */
#define IMOS_MW_IMAGE_TYPE_CAPTURE              1       /**< 抓拍图像 */
#define IMOS_MW_IMAGE_TYPE_COUNT                2       /**< 图像类型个数 */

/**@brief 白平衡模式 */
#define IMOS_MW_WB_AUTO                         0       /**< 自动白平衡 */
#define IMOS_MW_WB_FINETUNE                     1       /**< 微调白平衡 */
#define IMOS_MW_WB_INDOOR                       2       /**< 室内白平衡 */
#define IMOS_MW_WB_OUTDOOR                      3       /**< 室外白平衡 */
#define IMOS_MW_WB_AUTO_MERCURY                 4       /**< 自动水银灯白平衡 */ 
#define IMOS_MW_WB_FINETUNE_NIGHT               5       /**< 夜间微调白平衡  */

/**@brief 对焦模式 */
#define IMOS_MW_FOCUS_AUTO                      0       /**< 自动对焦 */
#define IMOS_MW_FOCUS_MANUAL                    1       /**< 手动对焦 */
#define IMOS_MW_FOCUS_PUSH                      2       /**< 一键对焦 */
#define IMOS_MW_FOCUS_PUSH_NIGHTINFRARED        3       /**< 一键对焦(夜间红外) */

/**@brief 快门值单位 */
#define IMOS_MW_SHUTTER_UNIT_MICSEC             0       /**< 微秒 */
#define IMOS_MW_SHUTTER_UNIT_DENOMINATOR        1       /**< 分母表示法，单位: 1/秒 */

/**@brief 曝光模式 */
#define IMOS_MW_EXPOSURE_AUTO                   0       /**< 自动曝光 */
#define IMOS_MW_EXPOSURE_CUSTOM                 1       /**< 自定义曝光 */
#define IMOS_MW_EXPOSURE_SHUTTER                2       /**< 快门优先 */
#define IMOS_MW_EXPOSURE_IRIS                   3       /**< 光圈优先 */
#define IMOS_MW_EXPOSURE_GAIN                   4       /**< 增益优先 */
#define IMOS_MW_EXPOSURE_INDOOR_50HZ            5       /**< 室内50HZ */
#define IMOS_MW_EXPOSURE_INDOOR_60HZ            6       /**< 室内60HZ */
#define IMOS_MW_EXPOSURE_MANUAL                 7       /**< 手动曝光 */

/**@brief 宽动态模式 */
#define IMOS_MW_WDR_DISABLE                     0       /**< 关闭 */
#define IMOS_MW_WDR_ENABLE                      1       /**< 开启 */
#define IMOS_MW_WDR_AUTO                        2       /**< 自动 */

/**@brief 测光模式 */
#define IMOS_MW_METERING_CENTER                 0       /**< 中央权重 */
#define IMOS_MW_METERING_AREA                   1       /**< 区域测光权重 */
#define IMOS_MW_METERING_HME                    2       /**< 强光抑制 */
#define IMOS_MW_METERING_VEHICLE                3       /**< 车辆测光 */

/**@brief 昼夜模式 */
#define IMOS_MW_DAYNIGHT_AUTO                   0       /**< 自动模式 */
#define IMOS_MW_DAYNIGHT_COLOR                  1       /**< 彩色 */
#define IMOS_MW_DAYNIGHT_BW                     2       /**< 黑白 */

/**@brief 锐度模式 */
#define IMOS_MW_SHARPNESS_AUTO                  0       /**< 自动模式 */
#define IMOS_MW_SHARPNESS_MANUAL                1       /**< 手动模式 */

/**@brief 镜像模式 */
#define IMOS_MW_MIRROR_MODE_NONE                0       /**< 正常模式 */
#define IMOS_MW_MIRROR_MODE_FLIP                1       /**< 垂直翻转 */
#define IMOS_MW_MIRROR_MODE_MIRROR              2       /**< 水平翻转 */
#define IMOS_MW_MIRROR_MODE_FLIP_MIRROR         3       /**< 垂直水平同时翻转 */
#define IMOS_MW_MIRROR_MODE_ROTATE_CLOCKWISE    4       /**< 顺时针旋转90度 */

#if 0
#endif
/*************************************** 卡口相关 ********************************************/
/*@brief 虚拟线圈最大条数 */
#define IMOS_MW_VIRTUAL_COIL_MAX_NUM                 8    

/*@brief 最大车牌识别区域个数  */
#define IMOS_MW_LPR_AREA_MAX_NUM                     4 

/*@brief 最大车道数目  */
#define IMOS_MW_VEHICLE_WAY_MAX_NUM                  4 

/*@brief 注册照片接收服务器，信息字符串长度  */
#define IMOS_MW_PHOTOSERVER_CODE_LEN                 32     

/*@brief 卡口路口信息长度 */
#define IMOS_MW_ROAD_INFO_LEN                        100     

/*@brief 最大线圈个数 */
#define IMOS_MW_VEHICLE_DETECT_MAX_NUM               8

/*@brief 最大串口个数 */
#define IMOS_MW_SERIAL_MAX_NUM                       20

/**@brief 照片接收服务器通信协议类型 */
#define IMOS_MW_PHOTOSERVER_UNVIEW1                  0             /**< 宇视V1（对应泓鎏）*/                           
#define IMOS_MW_PHOTOSERVER_UNVIEW2                  1             /**< 宇视V2（对应当前tms）*/                           
#define IMOS_MW_PHOTOSERVER_XC                       2             /**< 先创 */
#define IMOS_MW_PHOTOSERVER_QST                      3             /**< 泉视通 */
#define IMOS_MW_PHOTOSERVER_FTP                      4             /**< FTP */
#define IMOS_MW_PHOTOSERVER_ZZ                       5             /**< 正直 */

/**@brief 车检器触发模式 */
#define IMOS_MW_TRRIGER_MODE_PULLINTO                0             /**< 进车触发 */
#define IMOS_MW_TRRIGER_MODE_PULLOUT                 1             /**< 出车触发 */

/**@brief 虚拟线圈(绊线) 开关 */
#define IMOS_MW_VIRTUAL_COIL_AUTO                    0             /**< 自动 */
#define IMOS_MW_VIRTUAL_COIL_ENABLE                  1             /**< 启用 */
#define IMOS_MW_VIRTUAL_COIL_DISABLE                 2             /**< 禁用 */
#define IMOS_MW_VIRTUAL_COIL_WITH_RADAR_SPEED        3             /**< 视频检测、雷达测速 */

/**@brief 卡口 工作模式 */
#define IMOS_MW_TG_MODE_TRAFFIC                      0             /**< 普通道路卡口 */
#define IMOS_MW_TG_MODE_TRAFFIC_EXPRESSWAY           1             /**< 高速路卡口 */
#define IMOS_MW_TG_MODE_TOLLGATE                     2             /**< 园区卡口 */
#define IMOS_MW_TG_MODE_MIX                          3             /**< 混合卡口 */

/**@brief 雷达 工作模式 */
#define IMOS_MW_RADAR_MODE_CONTINUOUS                0             /**< 连续模式 */
#define IMOS_MW_RADER_MODE_FRONT_TRIGGER             1             /**< 车头触发模式 */
#define IMOS_MW_RADER_MODE_REAR_TRIGGER              2             /**< 车尾触发模式 */
#define IMOS_MW_RADER_MODE_DOUBLE_TRIGGER            3             /**< 双触发模式 */

/**@brief 雷达 方向过滤模式 */
#define IMOS_MW_RADAR_DIRECTION_NONE_FILTER          0             /**< 不过滤 */
#define IMOS_MW_RADER_DIRECTION_COME                 1             /**< 输出来向车速 */
#define IMOS_MW_RADER_DIRECTION_GONE                 2             /**< 输出去向车速 */

/**@brief 雷达 车速格式 */
#define IMOS_MW_RADAR_SPEED_SINGLE_BYTE              0             /**< 单字节 */
#define IMOS_MW_RADAR_SPEED_DOUBLE_BYTE              1             /**< 双字节 */
#define IMOS_MW_RADAR_SPEED_ASCII_FORMAT             2             /**< ASCII 格式 */

/**@brief 激光测量模式 */
#define IMOS_MW_LASER_MEASURE_MODE_SIDE_DUAL         0             /**< 侧立双激光模式 */
#define IMOS_MW_LASER_MEASURE_MODE_TOP_DUAL          1             /**< 顶置双激光模式 */
#define IMOS_MW_LASER_MEASURE_MODE_AVERAGE           2             /**< 平均模式 */
#define IMOS_MW_LASER_MEASURE_MODE_CENTRAL           3             /**< 中央模式 */
#define IMOS_MW_LASER_MEASURE_MODE_TOP_SINGLE        4             /**< 顶置单激光模式 */
#define IMOS_MW_LASER_MEASURE_MODE_SIDE_SINGLE       5             /**< 侧立单激光模式 */
#define IMOS_MW_LASER_MEASURE_MODE_DEMO              6             /**< 演示模式 */

/**@brief 激光测量方式 */
#define IMOS_MW_LASER_MEASURE_WAY_MANUAL             0             /**< 手动测量 */
#define IMOS_MW_LASER_MEASURE_WAY_AUTO               1             /**< 自动测量 */

/**@brief 激光日志开关 */
#define IMOS_MW_LASER_LOG_CLOSE                      0             /**< 关闭 */
#define IMOS_MW_LASER_LOG_OPEN                       1             /**< 开启 */

/**@brief 电平触发开关 */
#define IMOS_MW_LASER_LEVER_TRIGGER_CLOSE            0             /**< 关闭 */
#define IMOS_MW_LASER_LEVER_TRIGGER_OPEN             1             /**< 开启 */
#if 0
#endif
/******************************* 电子警察相关定义 ***********************************/
/**@brief 单方向摄像机个数最大值 */
#define IMOS_MW_CAMERA_NUM_MAX          	     8
/**@brief 最大方向数, 电警上暂未使用，后续按需求修改该值 */
#define IMOS_MW_DIRECTION_NUM_MAX                1

/**@brief 单个车道上绊线条数 */
#define IMOS_MW_DRIVEWAY_VIRTUALCOIL_NUM         2

/**@brief 单个车道车道线条数 */
#define IMOS_MW_DRIVEWAY_LINE_NUM                2

/**@brief 通用时间段个数 */
#define IMOS_MW_COMMON_TIMESECTION_NUM           4

/**@brief 专用车道 时间段个数 */
#define IMOS_MW_DRIVEWAY_TIMESECTION_NUM         4

/**@brief 速度调整的速度段最大个数 */
#define IMOS_MW_DRIVEWAY_SPEED_ADJUST_NUM        5

/**@brief 单个摄像机 车道数最大值 */
#define IMOS_MW_DRIVEWAY_NUM_MAX                 8

/**@brief 单个摄像机 车道线数最大值 */
#define IMOS_MW_DRIVEWAY_LINE_NUM_MAX            (IMOS_MW_DRIVEWAY_NUM_MAX + 1)

/**@brief 检测区域顶点最大个数 */
#define IMOS_MW_DETECTAREA_POINTNUM_MAX          16

/**@brief 单个摄像机 检测区域最大个数 */
#define IMOS_MW_DETECTAREA_NUM_MAX               8

/**@brief 单个摄像机 标定水平线条数 */
#define IMOS_MW_HORIZONTAL_LINE_NUM              2

/**@brief 单个摄像机 触发线条数 */
#define IMOS_MW_TRIGGER_LINE_NUM                 16

/**@brief 单个摄像机 违章抓拍的触发线条数 */
#define IMOS_MW_PECCANCY_TRIGGERLINE_NUM         8

/**@brief 违章类型的最大个数 */
#define IMOS_MW_PECCANCYTYPE_MAX_NUM             32

/**@brief 红绿灯 颜色个数 */
#define IMOS_MW_TRAFFICLIGHT_COLOR_NUM           3

/**@brief 单个红绿灯灯组中 灯个数的最大值 */
#define IMOS_MW_LIGHT_NUM_MAX                    8    

/**@brief 单个摄像机的红绿灯 灯组个数的最大值 */
#define IMOS_MW_LIGHTGROUP_NUM_MAX               8

/**@brief 单个摄像机的红绿灯 模板最大个数 */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_NUM_MAX    16

/**@brief 红绿灯模板名称长度 */
#define  IMOS_MW_TRAFFICLIGHT_TEMPLATE_NAME_LEN  32

/*@brief 同步相位配置数目  */
#define IMOS_MW_ACSYNC_NUM                        4

/**@brief 红绿灯颜色 */
#define IMOS_MW_LIGHT_COLOR_RED                  0 
#define IMOS_MW_LIGHT_COLOR_GREEN                1
#define IMOS_MW_LIGHT_COLOR_YELLOW               2

/**@brief 方向 */
#define IMOS_MW_DIRECTION_EASTTOWEST             1                  /**< 自东向西 */
#define IMOS_MW_DIRECTION_WESTTOEAST       	     2                  /**< 自西向东 */
#define IMOS_MW_DIRECTION_SOUTHTONORTH           3                  /**< 自南向北 */
#define IMOS_MW_DIRECTION_NORTHTOSOUTH           4                  /**< 自北向南 */
#define IMOS_MW_DIRECTION_SOUTHEASTTONORTHWEST   5                  /**< 东南向西北 */
#define IMOS_MW_DIRECTION_NORTHWESTTOSOUTHEAST   6                  /**< 西北向东南 */
#define IMOS_MW_DIRECTION_NORTHEASTTOSOUTHWEST   7                  /**< 东北向西南 */
#define IMOS_MW_DIRECTION_SOUTHWESTTONORTHEAST   8                  /**< 西南向东北 */

/**@brief 图片场帧类型 */
#define IMOS_MW_IMAGE_FIELD_TYPE 		         0                  /**< 场类型 */
#define IMOS_MW_IMAGE_FRAME_TYPE		         1                  /**< 帧类型 */

/**@brief 视频源类型 */
#define IMOS_MW_VIDEOSRC_LOCAL                   0                  /**< 本地 */
#define IMOS_MW_VIDEOSRC_JPEG                    1                  /**< JPEG */
#define IMOS_MW_VIDEOSRC_MJPEG                   2	                /**< MJPEG */
#define IMOS_MW_VIDEOSRC_AVI                     3	                /**< AVI */

/**@brief 图片旋转 */
#define IMOS_MW_IMAGE_ROTATE_NONE                0                  /**< 不旋转 */                    
#define IMOS_MW_IMAGE_ROTATE_CLOCKWISE_90        1                  /**< 顺时针 90°*/  
#define IMOS_MW_IMAGE_ROTATE_ANTICLOCKWISE_90    2                  /**< 逆时针 90°*/ 

/**@brief 图片色彩空间 */
#define IMOS_MW_IMAGE_COLORSPACE_JPG             0                  /**< JPG */                    
#define IMOS_MW_IMAGE_COLORSPACE_RGB             1                  /**< RGB */                    
#define IMOS_MW_IMAGE_COLORSPACE_YUV420          2                  /**< YUV420 */  
#define IMOS_MW_IMAGE_COLORSPACE_YUV422          3                  /**< YUV422 */ 
#define IMOS_MW_IMAGE_COLORSPACE_GRAY            4                  /**< GRAY */ 

/**@brief 车道属性 */
#define IMOS_MW_DRIVEWAYTYPE_NORMAL              0                  /**< 普通车道 */
#define IMOS_MW_DRIVEWAYTYPE_SINGLE_LINE         1                  /**< 单行线车道 */

/**@brief 车道类型 */
#define IMOS_MW_DRIVEWAYTYPE_GENERAL             0                  /**< 普通车道 */
#define IMOS_MW_DRIVEWAYTYPE_BUSONLY             1                  /**< 公交车专用车道 */
#define IMOS_MW_DRIVEWAYTYPE_NOTRUNK             2                  /**< 货车禁入车道 */
#define IMOS_MW_DRIVEWAYTYPE_NOVEHICLE           3                  /**< 非机动车道 */

/**@brief 车辆行驶方向 */
#define IMOS_MW_DRIVING_DIRECTION_COME           0                  /**< 由上往下行驶（车头） */
#define IMOS_MW_DRIVING_DIRECTION_GONE           1                  /**< 由下往上行驶（车尾） */
#define IMOS_MW_DRIVING_DIRECTION_BOTH           3                  /**< 双向 */

/**@brief 车道的行驶类型 (可组合) */
#define IMOS_MW_DRIVING_TYPE_STRAIGHT            0x1               /**< 直行 */
#define IMOS_MW_DRIVING_TYPE_TURN_LEFT           0x2               /**< 左转 */
#define IMOS_MW_DRIVING_TYPE_TURN_RIGHT          0x4               /**< 右转 */

/**@brief 当前车道 非法行驶类型 (可组合) */
#define IMOS_MW_DRIVING_ILLEGAL_TURNLEFT                0x1               /**< 禁止左转 */
#define IMOS_MW_DRIVING_ILLEGAL_TURNRIGHT               0x2               /**< 禁止右转 */
#define IMOS_MW_DRIVING_ILLEGAL_DIRECT_STRAIGHT         0x4               /**< 禁止直行 */

/**@brief 检测区域类型 */
#define IMOS_MW_DETECTAREA_VEHICLE_DETECT        0               /**< 车辆检测区域 */
#define IMOS_MW_DETECTAREA_FOLLOW                1               /**< 跟踪区域 */
#define IMOS_MW_DETECTAREA_LOCKAREA              2               /**< 路面标定区域 */

/**@brief 车辆检测模式 */
#define IMOS_MW_VEHICLE_DETECTMODE_VIDEO         0               /**< 视频检测 */

/**@brief 车牌识别模式: 暂未使用 */
#define IMOS_MW_PLATEIDENTIFY_MODE_PICTURE       0               /**< 图片识别 */

/**@brief 车辆限速模式  */
#define IMOS_MW_SPEEDLIMIT_MODE_DRIVEWAY         0              /**< 按车道限速 */
#define IMOS_MW_SPEEDLIMIT_MODE_VEHICLETYPE      1              /**< 按车型限速 */

/**@brief 车型 (可组合) */
#define IMOS_MW_VEHICLETYPE_SMALL                0x1               /**< 小型车 */
#define IMOS_MW_VEHICLETYPE_LARGE                0x2               /**< 大型车 */
#define IMOS_MW_VEHICLETYPE_MOTOR                0x4               /**< 摩托车 */
#define IMOS_MW_VEHICLETYPE_ENHANCE              0x400            /**< 附加属性，增强对无牌车的检测，可能增加误报 */
#define IMOS_MW_VEHICLETYPE_REDUCE               0x800            /**< 附加属性，减少误报输出，可能降低检测率 */
#define IMOS_MW_VEHICLETYPE_NUM                  8                 /**< 车型类型最大值 */

/**@brief 触发线 类型 */
#define IMOS_MW_TRIGGER_LINE_DEFAULT1            1              /**< 触发线1 */
#define IMOS_MW_TRIGGER_LINE_STOP                2              /**< 停止线 */
#define IMOS_MW_TRIGGER_LINE_STRAIGHT2           3              /**< 直行触发线2 */
#define IMOS_MW_TRIGGER_LINE_STRAIGHT3           4              /**< 直行触发线3 */
#define IMOS_MW_TRIGGER_LINE_STRAIGHT_DELAY3     5              /**< 直行触发线3# */
#define IMOS_MW_TRIGGER_LINE_TURNLEFT2           6              /**< 左转触发线2 */
#define IMOS_MW_TRIGGER_LINE_TURNRIGHT2          7              /**< 右转触发线2 */

/**@brief 红绿灯 识别方式 */
#define IMOS_MW_TRAFFICLIGHT_IDENTIFYMODE_TEMPLATE        0     /**< 模板识别 */
#define IMOS_MW_TRAFFICLIGHT_IDENTIFYMODE_VIDEO           1     /**< 视频识别 */
#define IMOS_MW_TRAFFICLIGHT_IDENTIFYMODE_SERIAL          2     /**< 单串口 */
#define IMOS_MW_TRAFFICLIGHT_IDENTIFYMODE_MULTISERIAL     3     /**< 多串口 */

/**@brief 红绿灯 类型 */
#define IMOS_MW_TRAFFICLIGHT_TYPE_SIMPLE                  0     /**< 简单灯 */
#define IMOS_MW_TRAFFICLIGHT_TYPE_COMPLEX                 1     /**< 复杂灯 */
#define IMOS_MW_TRAFFICLIGHT_TYPE_MIX                     2     /**< 混合灯 */

/**@brief 红绿灯 颜色 (可组合) */
#define IMOS_MW_TRAFFICLIGHT_COLOR_RED                    0x1     /**< 红灯 */
#define IMOS_MW_TRAFFICLIGHT_COLOR_YELLOW                 0x2     /**< 黄灯 */
#define IMOS_MW_TRAFFICLIGHT_COLOR_GREEN                  0x4     /**< 绿灯 */

/**@brief 红绿灯 形状 */
#define IMOS_MW_TRAFFICLIGHT_SHAPE_ROUND                  0     /**< 圆形 */
#define IMOS_MW_TRAFFICLIGHT_SHAPE_ARROW                  1     /**< 箭头 */
#define IMOS_MW_TRAFFICLIGHT_SHAPE_BAR                    2     /**< 条形 */
#define IMOS_MW_TRAFFICLIGHT_SHAPE_OTHER                  4     /**< 其他 */


/**@brief 红绿灯 IO端口 */
#define IMOS_MW_TRAFFICLIGHT_IO_NONE     		          0     /**< 无 */
#define IMOS_MW_TRAFFICLIGHT_IO_SERIAL1                   1     /**< 串口1 */
#define IMOS_MW_TRAFFICLIGHT_IO_SERIAL2                   2     /**< 串口2 */   

/**@brief 红绿灯 指示方向 (可组合) */
#define IMOS_MW_TRAFFICLIGHT_DIRECT_LEFT                  0x1     /**< 左转 */
#define IMOS_MW_TRAFFICLIGHT_DIRECT_RIGHT                 0x2     /**< 右转 */
#define IMOS_MW_TRAFFICLIGHT_DIRECT_STRAIGHT              0x4     /**< 直行 */
#define IMOS_MW_TRAFFICLIGHT_DIRECT_NUM                   3       /**< 方向个数 */


/**@brief 红绿灯 排列方向 */
#define IMOS_MW_TRAFFICLIGHT_ARRANGE_HORIZATIONAL         0     /**< 横向 */
#define IMOS_MW_TRAFFICLIGHT_ARRANGE_VERTICAL             1     /**< 纵向 */

/**@brief 闯红灯 检测模式 */
#define IMOS_MW_RUNREDLIGHT_DETECTMODE_GENERAL            0     /**< 普通模式 */
#define IMOS_MW_RUNREDLIGHT_DETECTMODE_3LINE              1     /**< 3线模式 */

/**@brief 违章记录生成模式 */
#define IMOS_MW_MULTI_BREAKRULE_RECORD_EACH               0     /**< 每种违章均单独生成违章记录 */
#define IMOS_MW_MULTI_BREAKRULE_RECORD_HIGHEST            1     /**< 仅生成最高优先级违章的违章记录 */

/**@brief 车道线类型 */
#define IMOS_MW_DRIVEWAY_LINE_SOLID_WHITE                        0     /**< 白实线 */
#define IMOS_MW_DRIVEWAY_LINE_SOLID_YELLOW                       1     /**< 黄实线 */
#define IMOS_MW_DRIVEWAY_LINE_2SOLID_YELLOW                      2     /**< 双黄实线 */
#define IMOS_MW_DRIVEWAY_LINE_L_REALLINE_R_BROKENLINE            3     /**< 双黄左实右虚线 */
#define IMOS_MW_DRIVEWAY_LINE_L_BROKENLINE_R_REALLINE            4     /**< 双黄左虚右实线 */
#define IMOS_MW_DRIVEWAY_LINE_L_REALLINE_R_BROKENLINE_WHITE      5     /**< 双白左实右虚线 */
#define IMOS_MW_DRIVEWAY_LINE_L_BROKENLINE_R_REALLINE_WHITE      6     /**< 双白左虚右实线 */
#define IMOS_MW_DRIVEWAY_LINE_OTHER                              7     /**< 其它 */

/**@brief 车辆违章类型 (可组合) */
#define IMOS_MW_VEHICLE_PECCANCY_NONE                     0x0        /**< 没有违章 */
#define IMOS_MW_VEHICLE_PECCANCY_RUNREDLIGHT              0x1        /**< 闯红灯 */
#define IMOS_MW_VEHICLE_PECCANCY_PRESSLINE                0x2        /**< 压线 */
#define IMOS_MW_VEHICLE_PECCANCY_OVERLINE                 0x4        /**< 越线(违章变道) */
#define IMOS_MW_VEHICLE_PECCANCY_ILLEGAL_DRIVEWAY         0x8        /**< 未按车道行驶 */
#define IMOS_MW_VEHICLE_PECCANCY_TURNLEFT                 0x10       /**< 违章左转 */
#define IMOS_MW_VEHICLE_PECCANCY_TURNRIGHT                0x20       /**< 违章右转 */
#define IMOS_MW_VEHICLE_PECCANCY_CONVERSE                 0x40       /**< 逆行 */
#define IMOS_MW_VEHICLE_PECCANCY_OVERSPEED                0x80       /**< 超速 */
#define IMOS_MW_VEHICLE_PECCANCY_LOWSPEED                 0x100      /**< 慢速 */
#define IMOS_MW_VEHICLE_PECCANCY_RESTRICT_ODD_EVEN        0x200      /**< 单双号限行 */
#define IMOS_MW_VEHICLE_PECCANCY_SPECAIL_DRIVEWAY         0x400      /**< 专用车道 */
#define IMOS_MW_VEHICLE_PECCANCY_ILLEGALPARK              0x800      /**< 违法停车 */
#define IMOS_MW_VEHICLE_PECCANCY_DIRECT_STRAIGHT          0x1000     /**< 违章直行 */ 
#define IMOS_MW_VEHICLE_PECCANCY_BACK_CAR                 0x2000     /**< 违法倒车 */ 
#define IMOS_MW_VEHICLE_PECCANCY_OVER50PERCENTSPEED       0x4000     /**< 超速50% */
#define IMOS_MW_VEHICLE_PECCANCY_PRESS_2SOLID_YELLOW      0x8000     /**< 压双黄线 */
#define IMOS_MW_VEHICLE_PECCANCY_PRESS_SOLID_YELLOW       0x10000    /**< 压单黄线 */
#define IMOS_MW_VEHICLE_PECCANCY_PRESS_STOP_LINE          0x20000    /**< 压停车线 */
#define IMOS_MW_VEHICLE_PECCANCY_OVER20PERCENTSPEED       0x40000    /**< 超速20% */
#define IMOS_MW_VEHICLE_PECCANCY_OVER100PERCENTSPEED      0x80000    /**< 超速100% */
#define IMOS_MW_VEHICLE_PECCANCY_STOPREDLIGHT             0x100000   /**< 闯红灯停车 */
#define IMOS_MW_VEHICLE_PECCANCY_RUNREDLIGHT_L            0x200000   /**< 左转闯红灯 */
#define IMOS_MW_VEHICLE_PECCANCY_RUNREDLIGHT_S            0x400000   /**< 直行闯红灯 */
#define IMOS_MW_VEHICLE_PECCANCY_RUNREDLIGHT_R            0x800000   /**< 右转闯红灯 */
#define IMOS_MW_VEHICLEPECCANCY_TYPE_NUM                  24         /**< 车辆违章类型个数*/


/**@brief 图片合成模式 */
#define IMOS_MW_IMAGE_COMPOSITE_MODE_NONE                 0          /**< 不合成 */
#define IMOS_MW_IMAGE_COMPOSITE_MODE_LEFTRIGHT            1          /**< 两张图片: 一左一右 */

/**@brief 生成图片类型 */
#define IMOS_MW_IMAGE_MAKE_PLATE_COLOR                    0x1          /**< 生成车牌彩色小图 */
#define IMOS_MW_IMAGE_MAKE_PLATE_BINARY                   0x2          /**< 生成车牌二值化图 */
#define IMOS_MW_IMAGE_MAKE_PLATE_FEATURE                  0x4          /**< 生成车牌特写图 */
#define IMOS_MW_IMAGE_MAKE_COMPOSE                        0x8          /**< 生成照片合成图 */

/**@brief 图片类型 */
#define IMOS_MW_IMAGE_PECCANCY                            0          /**< 违法行为图片 */
#define IMOS_MW_IMAGE_COMPOSITE                           1          /**< 合成图片 */
#define IMOS_MW_IMAGE_TEMP                                2          /**< 临时图片 */
#define IMOS_MW_IMAGE_TYPE_NUM                            3          /**< 图片类型个数 */

/**@brief 红绿灯模板类型 (可组合) */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_STRAIGHT            0x1        /**< 直行通行 */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_NOSTRAIGHT          0x2        /**< 直行禁行 */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_TURNLEFT            0x4        /**< 左转通行 */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_NOTURNLEFT          0x8        /**< 左转禁行 */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_TURNRIGHT           0x10       /**< 右转通行 */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_NOTURNRIGHT         0x20       /**< 右转禁行 */

/**@brief 抓拍类型类型(可组合) */
#define IMOS_MW_CAPTURE_PECCANCY                          0x1          /**< 违章记录 */                   
#define IMOS_MW_CAPTURE_PECCANCY_PASSVEHICLE              0x2          /**< 违章过车记录 */  
#define IMOS_MW_CAPTURE_NOPECCANCY_PASSVEHICLE            0x4          /**< 不违章过车记录 */  

/**@brief 相机类型 */
#define IMOS_MW_FULLVIEW_CAMERA                           0            /**< 全景 */   
#define IMOS_MW_FEATURE_CAMERA                            1            /**< 特写 */   

/**@brief 运行环境 */
#define IMOS_MW_PRODUCT_TYPE_TG                           0            /**< 卡口 */   
#define IMOS_MW_PRODUCT_TYPE_EP                           1            /**< 电警 */   

/**@brief 照片类型 */
#define	IMOS_MW_IMAGE_VEHICLE                             1           /**< 车辆大图 */
#define IMOS_MW_IMAGE_PLATE_COLOR                         2           /**< 车牌彩色小图 */
#define IMOS_MW_IMAGE_PLATE_BINARY                        3           /**< 车牌二值化图 */

#if 0
#endif
/*************************************** OSD相关 ********************************************/
/*@brief OSD 叠加内容的长度 */
#define IMOS_MW_OSD_INFO_LEN                      60    

/*@brief 叠加 OSD 最大个数 */
#define IMOS_MW_INFO_OSD_MAX_NUM                  3   

/*@brief OSD 叠加内容最大行数 */
#define IMOS_MW_INFO_OSD_LINE_MAX                 8

/*@brief 遮盖 OSD 最大个数 */
#define IMOS_MW_COVER_OSD_MAX_NUM                 8 

/*@brief 叠加内容类型 */
#define IMOS_MW_OSD_INFO_TYPE_UNUSED              0    /* 不使用 */
#define IMOS_MW_OSD_INFO_TYPE_USERDEF             1    /* 自定义 */
#define IMOS_MW_OSD_INFO_TYPE_DATETIME            2    /* 时间日期 */
#define IMOS_MW_OSD_INFO_TYPE_PTZOPER             3    /* 云台控制者 */
#define IMOS_MW_OSD_INFO_TYPE_PTZCOORDINATS       4    /* 云台坐标 */
#define IMOS_MW_OSD_INFO_TYPE_CRUISEINFO          5    /* 巡航信息 */
#define IMOS_MW_OSD_INFO_TYPE_ZOOMINFO            6    /* 变倍信息 */
#define IMOS_MW_OSD_INFO_TYPE_PRESETINFO          7    /* 预置位信息 */
#define IMOS_MW_OSD_INFO_TYPE_ALARMINFO           8    /* 报警信息 */
#define IMOS_MW_OSD_INFO_TYPE_ENCODEINFO          9    /* 编码信息 */

/*@brief 其他 OSD 类型 */
#define IMOS_MW_OSD_PIC_TIME                      0     /**< 抓拍照片 时间OSD */
#define IMOS_MW_OSD_PIC_VEHICLE_SPEED             1     /**< 抓拍照片 车速OSD */
#define IMOS_MW_OSD_PIC_LIMITED_SPEED             2     /**< 抓拍照片 限速OSD */
#define IMOS_MW_OSD_PIC_PECCANCY_TYPE             3     /**< 抓拍照片 违章类型OSD */
#define IMOS_MW_OSD_PIC_VEHICLE_COLOR             4     /**< 抓拍照片 车身颜色OSD */
#define IMOS_MW_OSD_PIC_VEHICLE_LOGO              5     /**< 抓拍照片 车标OSD */
#define IMOS_MW_OSD_PIC_VEHICLE_TYPE              6     /**< 抓拍照片 车型OSD */
#define IMOS_MW_OSD_PIC_DRIVINGTYPE               7     /**< 抓拍照片 车道行驶类型OSD */
#define IMOS_MW_OSD_PIC_VEHICLE_PLATE             8     /**< 抓拍照片 车牌(包含车牌颜色) */
#define IMOS_MW_OSD_PIC_VEHICLE_WAYID             9     /**< 抓拍照片 车道号 */
#define IMOS_MW_OSD_PIC_VEHICLE_CAMERAID          10    /**< 抓拍照片 摄像机ID */
#define IMOS_MW_OSD_PIC_VEHICLE_DIRECTION         11    /**< 抓拍照片 方向 */
#define IMOS_MW_OSD_PIC_VEHICLE_CAP_DIRECTION     12    /**< 抓拍照片 抓拍方向 */
#define IMOS_MW_OSD_PIC_VEHICLE_VERIFY            13    /**< 抓拍照片 防伪码 */
#define IMOS_MW_OSD_PIC_VEHICLE_NAME              14    /**< 抓拍照片 配置项名称 */
#define IMOS_MW_OSD_PIC_VEHICLE_SPEED_PERCENT     15    /**< 抓拍照片 超速百分比OSD */ 
#define IMOS_MW_OSD_MJPEG_VEHICLE_CAP_DIRECTION   16    /**< MJPEG 行驶方向 */ 
#define IMOS_MW_OSD_MJPEG_VEHICLE_ROAD_MESSAGE    17    /**< MJPEG 路口信息 */
#define IMOS_MW_OSD_MJPEG_VEHICLE_TIME            18    /**< MJPEG 时间 */
#define IMOS_MW_OSD_PIC_DEVICE_ID                 19    /**< 抓拍照片 设备编号 */
#define IMOS_MW_OSD_TYPE_BUTT                     20    /**< 支持的抓拍OSD个数 */

/*@brief OSD 字体样式 */
#define IMOS_MW_OSD_FONT_STYLE_NORMAL             0    /**< 正常 */
#define IMOS_MW_OSD_FONT_STYLE_STROKES            1    /**< 描边 */ 
#define IMOS_MW_OSD_FONT_STYLE_HOLLOW             2    /**< 空心 */

/*@brief OSD 字体大小 */
#define IMOS_MW_OSD_FONT_SIZE_LARGE               0    /**< 大 */
#define IMOS_MW_OSD_FONT_SIZE_MIDDLE              1    /**< 中 */
#define IMOS_MW_OSD_FONT_SIZE_SMALL               2    /**< 小 */

/*@brief OSD 颜色 */
#define IMOS_MW_OSD_COLOR_WHITE                   0    /**< 白 */
#define IMOS_MW_OSD_COLOR_RED                     1    /**< 红 */
#define IMOS_MW_OSD_COLOR_YELLOW                  2    /**< 黄 */
#define IMOS_MW_OSD_COLOR_BLUE                    3    /**< 蓝 */
#define IMOS_MW_OSD_COLOR_BLACK                   4    /**< 黑 */
#define IMOS_MW_OSD_COLOR_GREEN                   5    /**< 绿 */
#define IMOS_MW_OSD_COLOR_PURPLE                  6    /**< 紫 */

/*@brief OSD 透明度 */
#define IMOS_MW_OSD_ALPHA_NO                      0    /**< 不透明 */
#define IMOS_MW_OSD_ALPHA_SEMI                    1    /**< 半透明 */
#define IMOS_MW_OSD_ALPHA_ALL                     2    /**< 全透明 */

#if 0
#endif
/*************************************** 媒体流相关 ********************************************/
/**@brief 传输模式 */
#define IMOS_MW_TRANSFER_MODE_RTP_UDP           0        /**< UDP＋RTP 报文模式 */
#define IMOS_MW_TRANSFER_MODE_RTP_TCP           1        /**< TCP＋RTP 报文模式 */
#define IMOS_MW_TRANSFER_MODE_UNIVIEW1_TCP      2        /**< TCP＋Uniview1 报文模式 */
#define IMOS_MW_TRANSFER_MODE_TS_TCP            3        /**< TCP＋TS 报文模式 */
#define IMOS_MW_TRANSFER_MODE_TS_UDP            4        /**< UDP＋TS 报文模式 */
#define IMOS_MW_TRANSFER_MODE_HTTP              5        /**< 基于RTP/RTSP/HTTP/TCP承载方式 */
#define IMOS_MW_TRANSFER_MODE_UNIVIEW2_TCP      6        /**< TCP＋Uniview2 报文模式 */
#define IMOS_MW_TRANSFER_MODE_LOCAL_IMAGE       7        /**< 本地照片导入 报文模式 */
#define IMOS_MW_TRANSFER_MODE_EZR_TMS           8        /**< TCP＋Uniview2 报文模式 EZR使用 */
#define IMOS_MW_TRANSFER_MODE_TMS_IMAGE         9        /**< TMS照片模式 报文模式 */

/**@brief 流ID */
#define IMOS_MW_STREAM_ID_MAIN                  0        /**< 主流 */
#define IMOS_MW_STREAM_ID_AUX                   1        /**< 辅流 */
#define IMOS_MW_STREAM_ID_THIRD                 2        /**< 三流 */
#define IMOS_MW_STREAM_ID_FOUTH                 3        /**< 四流 */

#if 0
#endif
/**************************************** 存储 (本地) 相关 ***************************************/
/**@brief 存储文件类型 */
#define IMOS_MW_STOR_FILE_TYPE_VIDEO        0          /**< 录像 */

/**@brief 存储策略 */
#define IMOS_MW_STOR_POLICY_FULL_STOP       0          /**< 满停止 */
#define IMOS_MW_STOR_POLICY_FULL_REWRITE    1          /**< 满覆盖 */

/**@brief 存储模式(开关) */
#define IMOS_MW_LOCAL_STOR_AUTO             0          /**< 自动 */
#define IMOS_MW_LOCAL_STOR_FOREVER          1          /**< 永存 */
#define IMOS_MW_LOCAL_STOR_SAVE             2          /**< 保留，上传后保留SD卡上的文件 */
#define IMOS_MW_LOCAL_STOR_START            10         /**< 开启 */
#define IMOS_MW_LOCAL_STOR_STOP             11         /**< 关闭 */

#if 0
#endif
/**************************************** 播放控制 ***************************************/
/*@brief 播放窗格最大个数 */
#define IMOS_MW_MAX_WND_NUM                      9
/**@brief 播放状态 */
#define IMOS_MW_PLAY_STATUS_16_BACKWARD          0       /**< 16倍速后退播放 */
#define IMOS_MW_PLAY_STATUS_8_BACKWARD           1       /**< 8倍速后退播放 */
#define IMOS_MW_PLAY_STATUS_4_BACKWARD           2       /**< 4倍速后退播放 */
#define IMOS_MW_PLAY_STATUS_2_BACKWARD           3       /**< 2倍速后退播放 */
#define IMOS_MW_PLAY_STATUS_1_BACKWARD           4       /**< 正常速度后退播放 */
#define IMOS_MW_PLAY_STATUS_HALF_BACKWARD        5       /**< 1/2倍速后退播放 */
#define IMOS_MW_PLAY_STATUS_QUARTER_BACKWARD     6       /**< 1/4倍速后退播放 */
#define IMOS_MW_PLAY_STATUS_QUARTER_FORWARD      7       /**< 1/4倍速播放 */
#define IMOS_MW_PLAY_STATUS_HALF_FORWARD         8       /**< 1/2倍速播放 */
#define IMOS_MW_PLAY_STATUS_1_FORWARD            9       /**< 正常速度前进播放 */
#define IMOS_MW_PLAY_STATUS_2_FORWARD            10      /**< 2倍速前进播放 */
#define IMOS_MW_PLAY_STATUS_4_FORWARD            11      /**< 4倍速前进播放 */
#define IMOS_MW_PLAY_STATUS_8_FORWARD            12      /**< 8倍速前进播放 */
#define IMOS_MW_PLAY_STATUS_16_FORWARD           13      /**< 16倍速前进播放 */

/**@brief 媒体文件格式 */
#define IMOS_MW_MEDIA_FILE_FORMAT_TS             0       /**< TS格式的媒体文件 */
#define IMOS_MW_MEDIA_FILE_FORMAT_FLV            1       /**< FLV格式的媒体文件 */

/**@brief 视频显示模式 */
#define IMOS_MW_RENDER_MODE_D3D                  0       /**< 使用D3D的方式进行显示 */
#define IMOS_MW_RENDER_MODE_DDRAW_NORMAL         1       /**< 使用Ddraw的Normal显示模式 */

/**@brief 视频像素格式 */
#define IMOS_MW_PIXEL_FORMAT_YUV420              0       /**< YUV420格式输出 */
#define IMOS_MW_PIXEL_FORMAT_RGB32               1       /**< RGB32格式输出 */

/**@brief 图像播放流畅性 */
#define IMOS_MW_PICTURE_TIME_PRIORITY            0       /**< 实时性优先 */
#define IMOS_MW_PICTURE_FLUENCY_PRIORITY         1       /**< 流畅性优先 */
#define IMOS_MW_PICTURE_AUTO                     2       /**< 自动 */
#define IMOS_MW_PICTURE_LOWDELAY                 3       /**< 低延时 */

/**@brief 播放窗格显示图像的比例 */
#define IMOS_MW_RENDER_SCALE_FULL                0       /**< 图像填充整个播放窗格 */
#define IMOS_MW_RENDER_SCALE_PROPORTION          1       /**< 图像按比例显示 */

/**@brief 视频帧类型 */
#define IMOS_MW_VIDEO_FRAME_TYPE_I               0       /**< I帧 */
#define IMOS_MW_VIDEO_FRAME_TYPE_P               1       /**< P帧 */
#define IMOS_MW_VIDEO_FRAME_TYPE_B               2       /**< B帧 */
/**@brief 时间戳类型 */
#define IMOS_MW_RENDER_TYPE_FRAME_RATE           0       /**< 使用帧率数据进行视频渲染 */
#define IMOS_MW_RENDER_TYPE_ABSOLUTE_TIME        1       /**< 使用绝对时间数据进行视频渲染 */

/**@brief 解码 对比度模式 */
#define IMOS_MW_PICTURE_CONTRAST_MODE_NONE        0      /**< 不进行对比度调节处理 */
#define IMOS_MW_PICTURE_CONTRAST_MODE_BRIGHTNESS  1      /**< 明亮模式 */
#define IMOS_MW_PICTURE_CONTRAST_MODE_SOFTNESS    2      /**< 柔和模式 */

/**@brief 图片格式 */
#define IMOS_MW_PICTURE_FORMAT_BMP                0      /**< BMP 格式 */ 
#define IMOS_MW_PICTURE_FORMAT_JPEG               1      /**< JPEG 格式，参数参见: IMOS_MW_CAPTURE_FORMAT_CFG_S */
#define IMOS_MW_PICTURE_FORMAT_CUSTOM             10     /**< 随流格式, 不需要其他参数 */

/*@brief 照片后端叠加信息类型 */
#define IMOS_MW_IMAGE_BACKEND_OSD                 0    /**< 叠加 OSD，结构定义: IMOS_MW_IMAGE_OSD_INFO_S */
#define IMOS_MW_IMAGE_BACKEND_RECTANGLE           1    /**< 叠加 信息框, 结构定义: IMOS_MW_RECTANGLE_S, 其中，坐标单位 0.01% */
#define IMOS_MW_IMAGE_BACKEND_OSD_OBJ             2    /**< 叠加OSD, 带目标信息框, 结构定义: IMOS_MW_IMAGE_OSD_WITH_OBJ_S, 信息框坐标单位 0.01% */


#if 0
#endif
/**************************************** 照片结构 ***************************************/
#define IMOS_MW_TRAFFIC_PIC_MAX_NUM                          8           /**< 最大照片数 */
#define IMOS_MW_UNIVIEW_MAX_TIME_LEN                         18          /**< 照片结构时间信息最大长度 */
#define IMOS_MW_PLACE_NAME_MAX_LEN                           256         /**< 照片结构地点名称最大长度 */
#define IMOS_MW_CAR_PLATE_MAX_LEN                            32          /**< 号牌号码最大长度 */
#define IMOS_MW_DEV_ID_MAX_LEN                               32          /**< 设备编号最大长度 */
#define IMOS_MW_TOLLGATE_NAME_MAX_LEN                        64          /**< 卡口设备名称最大长度 */
#define IMOS_MW_DIRECTION_NAME_MAX_LEN                       64          /**< 方向名称最大长度 */
#define IMOS_MW_CAR_VEHICLE_BRAND_LEN                        4           /**< 车标编码最大长度 */

#define IMOS_MW_STREAM_FORMAT_JPEG              12      /**< JPEG */ 

#if 0
#endif
#ifdef  __cplusplus
}
#endif

#endif /*end of __IMOS_SDK_DEF_H__*/

