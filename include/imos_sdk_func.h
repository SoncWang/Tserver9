/*******************************************************************************
 Copyright (c) 2013,  Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_sdk_func.h
  Project Code: MW_SDK
   Module Name: SDK
  Date Created: 2012-01-14
        Author: c00510 
   Description: 该文件定义了sdk相关的错误码、宏、枚举、结构体、函数接口

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  2012-01-14

*******************************************************************************/
#ifndef __IMOS_SDK_FUNC_H__
#define __IMOS_SDK_FUNC_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#if 0
#endif
/*********************************** 公共函数 **************************************************
SDK Info
***************************************************************************************/
/** @defgroup groupSDKInfo SDK日志和信息
*   SDK日志记录和版本信息
*   @{
*/
/**
 * 获取SDK版本号 \n
 * @param [OUT]pucVersion     版本号，长度: IMOS_MW_SDK_CLIENT_VERSION_LEN
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * @note 
 * -     调用者需保证指针所指向的内存长度不小于: IMOS_MW_SDK_CLIENT_VERSION_LEN
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_GetSDKVersion(OUT UCHAR *pucVersion);

/**
 * 设置SDK日志信息 \n
 * @param [IN] ulLogLevel  日志级别，参见: IMOS_SDK_LOG_CLOSE 等
 * @param [IN] pcLogPath   保存路径
 * @param [IN] ulLogSize   日志大小
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_LOG_CLOSE                  日志已关闭
 * @note 
 * -     1、日志级别的含义，实际记录所有高于设置级别的日志信息，建议设置为: IMOS_SDK_LOG_INFO。
 * -     2、日志保存路径需指定，路径长度和操作系统有关，sdk不做限制，windows默认路径长度小于等于256字节（包括文件名在内）。
 * -     3、SDK默认日志文件名称 "IMOS_MW_SDK00.log", 默认单个日志文件大小为1.5M，超过该大小时，将已有日志文件备份为"IMOS_MW_SDK01.log", 后重新记录在原文件中。
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_SetLog(IN ULONG ulLogLevel, 
                                         IN CHAR *pcLogPath, 
                                         IN ULONG ulLogSize);
                                         
/** @defgroup groupDeviceStatus 获取设备状态
*   获取设备状态接口
*   @{
*/
/**
 * 获取状态函数 \n
 * @param [IN] pcUserID           用户ID
 * @param [IN] ulChannel          通道号
 * @param [IN] ulCmdType          状态命令类型, 参见: IMOS_MW_STATUS_KEEPALIVE 等
 * @param [INOUT] pStatusInfo     状态信息, 参见各状态命令类型的 说明
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_USERNONEXIST               用户不存在
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * -             #ERR_SDK_COMMON_NO_MEMORY           系统内存不足
 * @note 
 * - 无
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_GetDeviceStatus(IN CHAR *pcUserID,
                                                  IN ULONG ulChannel,
                                                  IN ULONG ulCmdType, 
                                                  INOUT VOID *pStatusInfo);

/**
 * 系统重启 \n
 * @param [IN] pcUserID    用户信息标识
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_USERNONEXIST               用户不存在
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * @note 无
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_Reboot(IN CHAR *pcUserID);

/*************************************************************************************
SDK Init
***************************************************************************************/
/** @defgroup groupSDKInit SDK初始化和注销
*   SDK初始化，详细的调用依赖关系请参见SDK说明文档。该模块包含初始化接口和清空接口。
*   @{
*/
/**
 * SDK初始化 \n
 * @param [IN] 无
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_CREATE_THREAD_FAIL         创建线程失败
 * -             #ERR_SDK_XP_INIT_FAILED             播放器初始化失败
 * -             #ERR_SDK_REINIT                     SDK 已经初始化
 * -             #ERR_SDK_COMMON_NO_MEMORY           缓存不足
 * @note 无
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_Initiate(VOID);

/**
 * SDK资源释放 \n
 * @param [IN] 无 
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * @note 无
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_Cleanup(VOID);
/** @} */ /* end of SDKInit Module */


/*************************************************************************************
Status Report
***************************************************************************************/
/** @defgroup groupStatusReport 设置状态上报回调函数
*   设置设备运行中的状态或异常消息的处理函数
*   @{
*/
/**
 * SDK状态上报回调函数 \n
 * @param [IN] pcUserID          用户ID, 表示当前上报状态的用户
 * @param [IN] ulReportType      消息或者异常消息类型，参见 IMOS_MW_STATUS_KEEPALIVE 等
 * @param [IN] pParam            存放消息或异常消息数据的缓冲区指针
 * @return ULONG
 * @note 
 * -     1、用户需要确保该回调函数尽快返回
 * -     2、不能在该回调函数中直接调用播放器的任何接口函数。
 * -     3、参数pParam所指的缓冲区中存放的数据的类型视消息或者异常消息类型而定，用户需要根据消息或者异常消息类型对其做类型转换
 */
typedef VOID (STDCALL* IMOS_MW_STATUS_REPORT_CALLBACK_PF)(IN CHAR *pcUserID,
                                                          IN ULONG ulReportType, 
                                                          IN VOID *pParam);
/**
 * 设置状态上报回调函数 \n
 * @param [IN] pfStatusReportCB       SDK状态上报回调函数的函数指针
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * @note 
 * -     1、用户需要确保该回调函数尽快返回。
 * -     2、不能在该回调函数中直接调用播放器SDK的任何接口函数。
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_SetStatusCallback(IN IMOS_MW_STATUS_REPORT_CALLBACK_PF pfStatusReportCBFun);

/*************************************************************************************
User Login
***************************************************************************************/
/** @defgroup groupUserLogin 用户登录
*   SDK用户登录、登出
*   @{
*/
/**
 * 用户登录 \n
 * @param [IN]  pcUserName        用户输入的登录名，长度为 IMOS_MW_USER_NAME_LEN
 * @param [IN]  pcPassword        用户输入的密码，长度为 IMOS_MW_USER_PASSWORD_LEN
 * @param [IN]  pcServerIP        目的终端的IP地址，长度为 IMOS_MW_IPADDR_LEN
 * @param [IN]  usServerPort      目的终端的端口号，若为0，则程序内部分配
 * @param [OUT] pcUserID          用户ID，长度为 IMOS_MW_RES_CODE_LEN
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_COMMON_INVALID_PARAM       输入参数非法
 * -             #ERR_SDK_NOTINIT                    设备SDK未初始化
 * -             #ERR_SDK_USERFULL                   设备用户已满
 * -             #ERR_SDK_USERNONEXIST               用户不存在
 * -             #ERR_SDK_USER_PASSWD_INVALID        用户密码错误
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * @note
 * -     1、该接口用于登录IPC设备
 * -     2、pcUserID 用户标识，具有唯一性，后续对设备的操作都需要通过此ID实现
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_Login(IN CHAR *pcUserName,
                                        IN CHAR *pcPassword,
                                        IN CHAR *pcServerIP,
                                        IN USHORT usServerPort,
                                        OUT CHAR *pcUserID);

/**
 * 用户注销 \n
 * @param [IN]  pcUserID      用户ID
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_COMMON_INVALID_PARAM       输入参数非法
 * -             #ERR_SDK_USERNONEXIST               用户非法
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * @note
 * -     输入参数中用户ID，是登录时获取到的。
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_Logout(IN CHAR *pcUserID);
 

/*************************************************************************************
Device Config
***************************************************************************************/
/** @defgroup groupDeviceConfig 参数配置
*   参数配置
*   @{
*/
/**
 * 获取设备配置信息 \n
 * @param [IN] pcUserID           用户ID
 * @param [IN] ulCmdType          命令类型，参见: IMOS_MW_SYSTEM_TIME_INFO 等
 * @param [IN] ulChannel          通道号
 * @param [INOUT] pulBufferLen    传入数据的缓冲区大小
 * @param [INOUT] pConfigDataBuf  存放输出参数的缓冲区
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_USERNONEXIST               用户非法
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * -             #ERR_SDK_COMMON_NO_MEMORY           系统内存不足
 * -             #ERR_SDK_NOENOUGH_BUF               缓存大小不足
 * @note
 * -     1、参数 ulChannel 对于各类型命令字有不同的含义，部分命令字不需要该参数，设置为无效值(0xFFFF)。
 * -     2、参数 pulBufferLen 入参时表示存放获取到配置的缓存区大小，出参表示实际获取到配置需要的缓存区大小；
 *          对于部分长度可变的配置项，若入参缓存区大小不够，需要根据出参重新获取配置。
 * -     3、参数 pConfigDataBuf 表示存放获取到配置的指针，其对应的结构定义参见各命令字的说明，其中部分参数的获取，需要传入指定参数。
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_GetConfig(IN CHAR *pcUserID,
                                            IN ULONG ulCmdType,
                                            IN ULONG ulChannel,
                                            INOUT ULONG *pulBufferLen,
                                            INOUT VOID *pConfigDataBuf);

/**
 * 设置设备配置信息 \n
 * @param [IN] pcUserID              用户ID
 * @param [IN] ulCmdType             命令类型，参见: IMOS_MW_SYSTEM_TIME_INFO 等
 * @param [IN] ulChannel             通道号
 * @param [IN] ulBufferLen           传入数据的缓冲区大小
 * @param [IN] pConfigDataBuf        存放输入参数的缓冲区
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_USERNONEXIST               用户非法
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * -             #ERR_SDK_COMMON_NO_MEMORY           系统内存不足
 * @note
 * -     1、参数 ulChannel 对于各类型命令字有不同的含义，部分命令字不需要该参数，设置为无效值(0xFFFF)。
 * -     2、参数 pulBufferLen 表示存放待设置的配置缓存区大小。
 * -     3、参数 pConfigDataBuf 表示存待设置配置的指针，其对应的结构定义参见各命令字的说明，其中部分参数的设置，需要传入指定参数。
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_SetConfig(IN CHAR *pcUserID,
                                            IN ULONG ulCmdType,
                                            IN ULONG ulChannel,
                                            IN ULONG ulBufferLen,
                                            IN VOID *pConfigDataBuf);

/**
 * 删除设备配置信息 \n
 * @param [IN] pcUserID              用户ID
 * @param [IN] ulCmdType             命令类型，参见: IMOS_MW_INFO_OSD_CFG 等
 * @param [IN] ulChannel             通道号
 * @param [IN] ulBufferLen           传入数据的缓冲区大小
 * @param [IN] pConfigDataBuf        存放输入参数的缓冲区
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_USERNONEXIST               用户非法
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * -             #ERR_SDK_COMMON_NO_MEMORY           系统内存不足
 * @note
 * -     1、参数 ulChannel 对于各类型命令字有不同的含义，部分命令字不需要该参数，设置为无效值(0xFFFF)。
 * -     2、参数 pulBufferLen 表示存放待设置的配置缓存区大小。
 * -     3、参数 pConfigDataBuf 表示存待设置配置的指针，其对应的结构定义参见各命令字的说明，其中部分参数的删除，需要传入指定参数。
 * -     4、支持删除的配置有：叠加OSD配置，遮盖OSD配置、运动检测区域配置、告警联动配置
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_DelConfig(IN CHAR *pcUserID,
                                            IN ULONG ulCmdType,
                                            IN ULONG ulChannel,
                                            IN ULONG ulBufferLen,
                                            IN VOID *pConfigDataBuf);
                                            
/***************************************************************************************/
/**
 * TCP 接受数据回调函数 \n
 * @param [IN] pcUserID           用户信息标识
 * @param [IN] pstAlarmInfo       事件信息(含事件类型、时间、来源)
 * @return ULONG
 * @note 无
 */
typedef ULONG (STDCALL* IMOS_MW_STREAMDATA_CALLBACK_PF)(IN ULONG ulStreamHandle, 
                                                        IN CHAR *pcStreamBuf,
                                                        IN INT lBufLen);   
                                                                                                            
/**
* 获取照片流回调,结构形式。 \n
* @param [IN] const IMOS_MW_UNIVIEW_PROTOCOL_HEADER_S *pstUniviewData   照片信息
* @param [IN] const CHAR* DeviceIP          设备IP (用于区分收到的照片来源)
* @return 无。
* @note ：带中文的自动为utf8格式
* -    
*/
typedef VOID (STDCALL* IMOS_MW_TMS_MULTIUSER_PIC_UPLOAD_PF)(IN const IMOS_MW_MULTI_UNIVIEW_PROTOCOL_HEADER_S *pstUniviewData, 
                                                            IN const ULONG ulStreamHandle);

/**
* 获取照片流回调(XML形式)。 \n
* @param [IN] const ULONG ulXmlLen, XML长度
* @param [IN] const CHAR *pcPicXmlInfo 照片相关信息, 以XML形式提供
* @param [IN] const IMOS_MW_PIC_INFO_S *pstPicData 照片数据
* @param [IN] const ULONG ulStreamHandle 流句柄
* @return 无。
* @note
* -    
*/
typedef VOID (STDCALL* IMOS_MW_PIC_XML_UPLOAD_PF)(IN const ULONG ulXmlLen,
                                                  IN const CHAR *pcPicXmlInfo,
                                                  IN const IMOS_MW_PIC_INFO_S *pstPicData, 
                                                  IN const ULONG ulStreamHandle);
                                                  
/**
 * 创建(启动)媒体流 \n
 * @param [IN] pcUserID              用户ID
 * @param [IN] ulChannel             通道号, 待扩展
 * @param [IN] ulStreamID            流ID, 参见 IMOS_MW_STREAM_ID_MAIN 等
 * @param [IN] ulTransMode           流传输模式，参见 IMOS_MW_TRANSFER_MODE_RTP_UDP 等
 * @param [IN] pcIpAddress           码流接收端IP地址, 
 * @param [IN] usPort                码流接收端端口, 若端口为0, 则由SDK内部分配接收端口
 * @param [IN] bKeepAlive            流保活标志，建立流用于第三方播放时，填 BOOL_FALSE
 * @param [OUT] pulStreamHandle      流句柄
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_USERNONEXIST               用户不存在  
 * -             #ERR_SDK_COMMON_NO_MEMORY           系统内存分配失败
 * -             #ERR_SDK_SOCKET_LSN_FAIL            创建Socket失败
 * -             #ERR_SDK_XP_PORT_ALLOC_FAILED       获取播放器资源失败
 * -             #ERR_SDK_DEVICE_STREAM_FULL         设备流已满
 * -             #ERR_SDK_XP_START_STREAM_FAILED     媒体流创建失败
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * @note 
 * -     1、码流接收端IP地址设为: ""或 NULL, 或 "0.0.0.0" 均表示本端ip, 接收端端ip为本端时可不指定端口号(置0)
 * -     2、用于IPC设备实况流的建立
 * -     3、获取到的流句柄，用于后续对流的处理
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_StartMediaStream(IN CHAR *pcUserID,
                                                   IN ULONG ulChannel,
                                                   IN ULONG ulStreamID,
                                                   IN ULONG ulTransMode,
                                                   IN CHAR *pcIpAddress,
                                                   IN USHORT usPort,
                                                   IN BOOL_T bKeepAlive,
                                                   OUT ULONG *pulStreamHandle);
/**
 * 删除(停止)媒体流 \n
 * @param [IN] pcUserID         用户ID
 * @param [IN] ulStreamHandle   流句柄，标识一条流
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMAND_SEND_FAIL          消息发送失败
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * -             #ERR_SDK_DEVICE_STREAM_NONEXIST     设备流不存在
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_USERNONEXIST               用户非法
 * @note 
 * -     用于IPC设备实况流的删除，流句柄是 IMOS_MW_StartMediaStream 获取到的
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_StopMediaStream(IN CHAR *pcUserID,
                                                  IN ULONG ulStreamHandle);

/**
 * 设置视频数据回调函数 \n
 * @param [IN] pfnStreamData    TCP码流回调函数
 * @param [IN] ulStreamHandle   流句柄，标识一条流
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMAND_SEND_FAIL          消息发送失败
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * -             #ERR_SDK_DEVICE_STREAM_NONEXIST     设备流不存在
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_USERNONEXIST               用户非法
 * @note 
 * -     用于IPC设备实况流的删除，流句柄是 IMOS_MW_StartMediaStream 获取到的
 *
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_SetStreamDataCallBack(IN IMOS_MW_STREAMDATA_CALLBACK_PF pfnStreamData,
                                                        IN ULONG ulStreamHandle);
                                                        
/**
 * 设置过车照片上传回调(卡口多用户) \n
 * @param [IN] IMOS_MW_TMS_MULTIUSER_PIC_UPLOAD_PF pfnPicDataCBFun 照片上传回调
 * @param [IN] ULONG ulStreamHandle 流句柄
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_DEVICE_STREAM_NONEXIST     设备流不存在
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_MultiUser_SetPicDataCallback(IN IMOS_MW_TMS_MULTIUSER_PIC_UPLOAD_PF pfnPicDataCBFun, IN ULONG ulStreamHandle);

/**
 * 设置多张照片上传回调，照片信息以XML格式上传 \n
 * @param [IN] IMOS_MW_PIC_XML_UPLOAD_PF pfnPicDataCBFun 照片上传回调
 * @param [IN] IN ULONG ulStreamHandle 流句柄
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_DEVICE_STREAM_NONEXIST     设备流不存在
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_SetPicDataAndXmlInfoCallback(IN IMOS_MW_PIC_XML_UPLOAD_PF pfnPicDataCBFun, 
                                                               IN ULONG ulStreamHandle);

/** @defgroup groupCameraCapture 手动前端抓拍
*   手动前端抓拍
*   @{
*/
/**
 * 手动前端抓拍 \n
 * @param [IN] pcUserID             用户ID
 * @param [IN] ulChannel            通道号
 * @param [INOUT] pulPicBuffSize    保存抓拍图片的缓冲区大小
 * @param [OUT] pcPicBuffer         保存抓拍图片的缓冲区
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_USERNONEXIST               用户非法
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * @note
 * -     1、pulPicBuffSize 入参时，表示 用户分配的缓存区大小，出参为实际的图片所需缓存区大小
 * -     2、返回值为 "分配的缓存区不足" 时，用户需根据出参中的文件实际缓存区大小重新分配足够缓存后，
 * -        重新调用该接口获取已抓拍的照片
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_Capture(IN CHAR *pcUserID,
                                          IN ULONG ulChannel,
                                          INOUT ULONG *pulPicBuffSize,
                                          OUT CHAR *pcPicBuffer);
/**
 * 输出开关量 \n
 * @param [IN] pcUserID         用户ID
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_COMMON_INVALID_PARAM       参数非法
 * -             #ERR_SDK_USERNONEXIST               用户不存在
 * -             #ERR_SDK_COMMAND_TIMEOUT            请求超时
 * @note 
 * -     
 */
IMOS_EXPORT ULONG STDCALL IMOS_MW_SetOutputSwitchStatusCfg(IN CHAR *pcUserID);                                                       

/**
 * 实现数据从SDK透传串口 \n
 * @param [IN] pcUserID      用户信息标识
 * @param [IN] pcDataBuffer        输入数据缓存(最大256 BYTE)
 * @param [IN] ulDataNum           输入数据长度(最大256 BYTE)
 * @param [IN] ulComIndex         串口号索引 (1----4)  (根据界面透明通道配置 port依次为 1025,1026,1027,1028)
 * @return ULONG，返回如下结果：
 * -             #ERR_COMMON_SUCCEED                 成功
 * -             #ERR_SDK_COMMON_FAIL                操作失败
 * -             #ERR_SDK_USERNONEXIST               用户不存在
 * @note 无
 * -
 */
ULONG STDCALL IMOS_MW_SendDataToCom(IN CHAR *pcUserID, IN CHAR *pcDataBuffer, IN LONG ulDataNum, IN ULONG ulComIndex);

#ifdef  __cplusplus
}
#endif

#endif /*end of __IMOS_SDK_FUNC_H__*/

