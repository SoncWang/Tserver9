/*******************************************************************************
 Copyright (c) 2013,  Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_sdk_def.h
  Project Code: MW_SDK
   Module Name: SDK
  Date Created: 2012-01-14
        Author:  
   Description: ���ļ�������sdk��صĴ����롢��

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
����ѡ��
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

/* ������׼����Լ�� */
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
ͨ���������Ͷ���
*******************************************************************************/
/** @brief 8λ�з������� */
#ifndef CHAR
typedef char CHAR;
#endif

/** @brief 8λ�޷������� */
#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

/** @brief 16λ�з������� */
#ifndef SHORT
typedef short SHORT;
#endif

/** @brief 16λ�޷������� */
typedef unsigned short USHORT;

/** @brief 32λ�з������� */
typedef long LONG;

/** @brief 32λ�޷������� */
typedef unsigned long ULONG;

/** @brief �з������� */
typedef int INT;

/** @brief �޷������� */
typedef unsigned int UINT;

/** @brief 64λ�з������� */
#if defined (_MSC_VER)
typedef __int64 DLONG;
#else
typedef long long DLONG;
#endif

/** @brief 64λ�޷������� */
#if defined (_MSC_VER)
typedef unsigned __int64 DULONG;
#else
typedef unsigned long long DULONG;
#endif

/** @brief ˫������������ */
typedef double DOUBLE;

/** @brief ������������ */
typedef float FLOAT;

/** @brief ������ */
typedef unsigned long BOOL_T;

/** @brief ��ʾ������������ */
typedef unsigned long SIZE_T;

/** @brief ��ʾʱ���������� */
typedef long TIME_T;

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;

/** @brief VOID�� */
#define VOID                            void

/** @brief ��ʾͨ�þ������ */
typedef VOID* HANDLE;

/** @brief IMOS ID �������� */
typedef DULONG IMOS_ID;

/** @brief ͨ�þ������ IMOS_HANDLE */
#ifndef IMOS_HANDLE
#if defined (WIN32)
typedef HANDLE IMOS_HANDLE;
#else
typedef int IMOS_HANDLE;
#endif
#endif

/** @brief �����������Ͷ��� */
#define IN
#define OUT
#define INOUT

/** @brief IMOS��̹淶����ֵ */
#define BOOL_TRUE                       ((BOOL_T) 1)
#define BOOL_FALSE                      ((BOOL_T) 0)

#ifndef NULL
#define NULL (void *)0
#endif

#if 0
#endif
/*******************************************************************************
SDK ������/״̬��
*******************************************************************************/
/* ͨ�óɹ� */
#define ERR_COMMON_SUCCEED                       0x000000      /**< ִ�гɹ� */

/*********************** SDK ״̬�붨��(257 - 356) *****************************/
#define ERR_SDK_LOG_CLOSE                        257            /**< SDK ��־�ر� */
#define ERR_SDK_DEVICE_STREAM_FULL               259            /**< �豸������ */
#define ERR_SDK_DEVICE_STREAM_CLOSED             260            /**< �豸���ѹر� */
#define ERR_SDK_DEVICE_STREAM_NONEXIST           261            /**< �豸�������� */
#define ERR_SDK_USER_PASSWORD_CHANGE             262            /**< �û������޸� */
#define ERR_SDK_USER_DELETED                     263            /**< �û���ɾ�� */

/*********************** SDK �����붨��(357 - 506) *****************************/
#define ERR_SDK_COMMON_FAIL                      357            /**< ����ʧ�� */
#define ERR_SDK_COMMON_INVALID_PARAM             358            /**< ��������Ƿ� */
#define ERR_SDK_COMMON_NO_MEMORY                 359            /**< ϵͳ�ڴ治�� */
#define ERR_SDK_COMMON_SYS_FAIL                  360            /**< ϵͳͨ�ô���Q: ��δʹ�� */
#define ERR_SDK_COMMON_FAIL_STAT                 361            /**< ��ȡ�ļ���Ŀ¼��״̬ʧ�� */
#define ERR_SDK_COMMON_FILE_NONEXIST             362            /**< �ļ������� */

#define ERR_SDK_USER_NOT_AUTHORIZED              457            /**< �û�δ��Ȩ */
#define ERR_SDK_USERFULL                         458            /**< �û����� */
#define ERR_SDK_USERNONEXIST                     459            /**< �û������� */
#define ERR_SDK_USER_PASSWD_INVALID              460            /**< �û�������� */
#define ERR_SDK_USER_KEEPALIVEFAIL               461            /**< SDK����ʧ�� */
#define ERR_SDK_NOTINIT                          462            /**< SDKδ��ʼ�� */
#define ERR_SDK_REINIT                           463            /**< SDK�ѳ�ʼ�� */

#define ERR_SDK_CREATE_THREAD_FAIL               466            /**< �����߳�ʧ�� */
#define ERR_SDK_NOENOUGH_BUF                     467            /**< ������̫С: �����豸���ݵĻ����� */
#define ERR_SDK_SOCKET_LSN_FAIL                  468            /**< ����socket listenʧ�� */
#define ERR_SDK_SUBSCRIBE_FULL                   470            /**< ���û���������, �豸���û����������Ĵ����벻�ܷ��� */
#define ERR_SDK_COMMAND_SEND_FAIL                471            /**< ������ʧ�� */
#define ERR_SDK_COMMAND_TIMEOUT                  472            /**< ����ʱ */
#define ERR_SDK_UPDATE_NOTADMIN                  473            /**< ��admin�û��޷����� */
#define ERR_SDK_UPDATE_INVALID                   474            /**< ���� δ��ʼ */
#define ERR_SDK_UPDATE_INPROCESS                 475            /**< ���� ������ */
#define ERR_SDK_UPDATE_NOMEMORY                  476            /**< ���� �ڴ�ռ䲻�� */
#define ERR_SDK_UPDATE_FILE_OPEN_ERR             477            /**< ���� �򿪾����ļ����� */
#define ERR_SDK_UPDATE_DEVICE_ERR                478            /**< ���� FLASH���� */
#define ERR_SDK_UPDATE_BUSY                      479            /**< ����ͬʱ���ض���������� */
#define ERR_SDK_UPDATE_FAIL_TIMEOUT              480            /**< ������ʱ */

#define ERR_SDK_IP_CONFLICT                      486            /**< IP��ַ��ͻ */
#define ERR_SDK_NETWORKMODE_NOTSUPPORT           487            /**< ����ģʽ��֧�� */


/*********************** SDK ҵ��ģ������붨��(507-656) *******************/
/* �豸ά�����(507 - 556) */
#define ERR_SDK_INVALID_CONFIGFILE               507            /**< �����ļ���Ч */


/* ý�����(557 - 606) */
#define ERR_SDK_BIAUDIO_AUDIOBCAST_FULL          557            /**< ����ҵ������ */
#define ERR_SDK_STOR_RESOURCE_NOTINIT            558            /**< �洢��Դδ���� */


/* ��̨���(607 - 656) */
#define ERR_SDK_PTZ_TRACK_ISUSED                 612            /**< �켣��ʹ�ã��޷�ɾ��*/
#define ERR_SDK_PTZ_SERIALMODE_MISMATCH          614            /**< ����ģʽ��ƥ��*/
#define ERR_SDK_PTZ_TRACK_NOT_EXIST              618            /**< �켣������*/
#define ERR_SDK_PTZ_MODE_CRUISE_FULL             626            /**< ģʽ·���켣��������� */

/*********************** SDK �����������붨��(657 - 756) ****************************/
#define ERR_SDK_XP_INIT_FAILED                   657            /**< ��������ʼ��ʧ�� */
#define ERR_SDK_XP_PORT_ALLOC_FAILED             658            /**< ������ͨ������ʧ�� */
#define ERR_SDK_XP_PORT_NOT_EXIST                659            /**< ������ͨ�������� */
#define ERR_SDK_XP_START_STREAM_FAILED           660            /**< ����������ʧ�� */
#define ERR_SDK_XP_START_PLAY_FAILED             662            /**< ��ʼ����ʧ�� */
#define ERR_SDK_XP_FILE_PLAY_FINISHED            669            /**< �ļ��Ѳ����� */
#define ERR_SDK_XP_DISK_CAPACITY_WARN            670            /**< Ӳ��ʣ��ռ������ֵ */
#define ERR_SDK_XP_DISK_CAPACITY_NOT_ENOUGH      671            /**< Ӳ��ʣ��ռ䲻�� */
#define ERR_SDK_XP_NO_PICTURE_TO_SNATCH          672            /**< û�н������ͼƬ�ɹ�ץ�� */
#define ERR_SDK_XP_SERIES_SNATCH_FAILED          673            /**< ����ץ�Ĺ�����ץ��ʧ�� */
#define ERR_SDK_XP_WRITE_FILE_FAILED             674            /**< д�ļ�����ʧ�� */
#define ERR_SDK_XP_FILE_DESTROY                  675            /**< �ļ����� */
#define ERR_SDK_XP_NOT_SUPPORT_MEDIA_ENCODE_TYPE 676            /**< ý������ʽ��֧��¼����� */
#define ERR_SDK_XP_PROCESS_MEDIA_DATA_FAILED     677            /**< ý�����ݴ���ʧ�� */
#define ERR_SDK_XP_RECV_DATA_FAILED              678            /**< ���������ɽ�������ʧ�� */
#define ERR_SDK_XP_MEDIA_RESOLUTION_CHANGE       679            /**< ý�����ֱ��ʷ����仯 */
#define ERR_SDK_XP_VOICE_RUNNING_ERROR           680            /**< �����Խ���㲥�����г��� */
#define ERR_SDK_XP_AUDIO_DEVICE_UNRIPE           682            /**< ��Ƶ�豸δ׼���� */

/*******************************************************************************
SDK �궨�� 
*******************************************************************************/
#if 0
#endif
/**************************************** �豸״̬��� ***************************************/
#define IMOS_MW_STATUS_KEEPALIVE                   0            /**< SDK���״̬��Ϊ: ERR_SDK_USER_KEEPALIVEFAIL */
#define IMOS_MW_STATUS_UPDATE                      1            /**< ���������״̬��Ϊ: ERR_SDK_UPDATE_INVALID �� */

#define IMOS_MW_STATUS_USERINFO_CHANGE             3            /**< �û���Ϣ�޸�, ״̬��Ϊ: ERR_SDK_USER_PASSWORD_CHANGE �� */ 
    
#define IMOS_MW_STATUS_MANAGE_SERVER_ONLINE        50           /**< ���������������״̬����Ӧ����BOOL_T: ON:1  OFF:0 */                  
#define IMOS_MW_STATUS_PHOTO_SERVER_ONLINE         51           /**< ��Ƭ������������״̬����Ӧ����BOOL_T: ON:1  OFF:0 */     
#define IMOS_MW_STATUS_BASIC_INFO                  52           /**< �豸������Ϣ����Ӧ��������: IMOS_MW_BASIC_DEVICE_INFO_S */              
#define IMOS_MW_STATUS_RUN_INFO                    53           /**< �豸����״̬����Ӧ��������: IMOS_MW_DEVICE_RUN_INFO_S */  
#define IMOS_MW_STATUS_NETWORK_CHANGE              54           /**< �������ý����״̬��Ϊ: ERR_SDK_IP_CONFLICT �� */
#define IMOS_MW_STATUS_STOR_MEMORY_CARD_FORMAT     90           /**< ���ش洢�豸��ʽ��״̬�� ״̬��Ϊ: ERR_COMMON_SUCCEED�� */  
#define IMOS_MW_STATUS_STOR_NAS_MOUNT              91           /**< NAS����״̬ ״̬��Ϊ: ERR_COMMON_SUCCEED�� */  
   
#define IMOS_MW_STATUS_RADAR                       150          /**< �״�״̬  ��Ӧ����: IMOS_MW_RADAR_XXX */                           
#define IMOS_MW_STATUS_COIL                        151          /**< ��Ȧ״̬  ��Ϣ����Ϊ: IMOS_MW_COIL_STATE_S �����飬֧�� 8��*/          
#define IMOS_MW_STATUS_POLARIZER                   152          /**< ƫ��״̬  ��Ӧ����: IMOS_MW_POLARIZER_STATUS_XXX */                     
#define IMOS_MW_STATUS_LED_STROBE                  153          /**< LED��״̬  ��Ӧ����: IMOS_MW_LED_STROBE_STATUS_XXX */ 
#define IMOS_MW_STATUS_ND_FILTER                   154          /**< ND�˾�״̬  ��Ӧ����: IMOS_MW_POLARIZER_STATUS_INUSE */
#define IMOS_MW_STATUS_TRAFFICLIGHT                155          /**< �źŵ�ʵʱ����״̬  ��Ӧ����: IMOS_MW_TRAFFICLIGHT_STATUS_XXX */
#define IMOS_MW_STATUS_SD                          156          /**< SD��״̬  ��Ӧ����: IMOS_MW_SD_STATUS_NOEXIST */

#define IMOS_MW_STATUS_TRAFFIC_PARAM_REPORT        169          /**< ��ͨ�����ϱ� ��Ӧ����:IMOS_MW_TRAFFIC_PARA_RSLT_S */
#define IMOS_MW_STATUS_VEHICLE_PARAM_REPORT        170          /**< ��������״̬�ϱ� ��Ӧ����:IMOS_MW_VEHICLE_STATE_S */

#define IMOS_MW_STATUS_PLAYER_RECORD_VIDEO         200          /**< ����¼��������ϱ�������Ϣ����Ϣ���ݼ�: IMOS_MW_PLAYER_STATUS_S */
#define IMOS_MW_STATUS_PLAYER_MEDIA_PROCESS        201          /**< ��Ƶý�崦������е��ϱ�������Ϣ����Ϣ���ݼ�: IMOS_MW_PLAYER_STATUS_S */
#define IMOS_MW_STATUS_PLAYER_SERIES_SNATCH        202          /**< ����ץ�Ĺ������ϱ�������Ϣ����Ϣ���ݼ�: IMOS_MW_PLAYER_STATUS_S */

/**@brief �豸����ģʽ */
#define IMOS_MW_DEVICE_WORK_MODE_TOLLGATE          0             /**< ���� */
#define IMOS_MW_DEVICE_WORK_MODE_ELEC_POLICE       1             /**< �羯 */

/**@brief �״�״̬ */
#define IMOS_MW_RADAR_STATUS_DISCONNECT            0             /**< �״�δ���� */
#define IMOS_MW_RADAR_STATUS_CONNECT               1             /**< �״����� */
    
/**@brief ��Ȧ״̬ */
#define IMOS_MW_COIL_STATUS_DISCONNECT             0             /**< ��Ȧ�Ͽ� */
#define IMOS_MW_COIL_STATUS_CONNECT                1             /**< ��Ȧ���� */
#define IMOS_MW_COIL_STATUS_UNUSE                  2             /**< ��Ȧδʹ�� */
    
/**@brief ƫ��״̬ */
#define IMOS_MW_POLARIZER_STATUS_INUSE             0             /**< ����ʹ�� */
#define IMOS_MW_POLARIZER_STATUS_UNUSE             1             /**< δʹ�� */
#define IMOS_MW_POLARIZER_STATUS_DOWNING           2             /**< �����½� */
#define IMOS_MW_POLARIZER_STATUS_UPING             3             /**< �������� */
#define IMOS_MW_POLARIZER_STATUS_UNKNOWN           4             /**< �쳣 */
#define IMOS_MW_POLARIZER_STATUS_SWITCHING         5             /**< �л��� */

/**@brief ��ͨ�Ƽ����ʵʱ����״̬ */
#define IMOS_MW_TRAFFICLIGHT_STATUS_OFFLINE        0             /**< ���� */
#define IMOS_MW_TRAFFICLIGHT_STATUS_ONLINE         1             /**< ���� */
#define IMOS_MW_TRAFFIClIGHT_STATUS_UNUSED         2             /**< δʹ�� */

/**@brief LED��״̬ */
#define IMOS_MW_LED_STROBE_STATUS_ON               0              /**< �� */
#define IMOS_MW_LED_STROBE_STATUS_OFF              1              /**< �ر� */

/**@brief SD��״̬ */
#define IMOS_MW_SD_STATUS_NOEXIST                  0             /**< ������ */
#define IMOS_MW_SD_STATUS_FAULT                    1             /**< ���� */
#define IMOS_MW_SD_STATUS_CHECKING                 2             /**< ����� */
#define IMOS_MW_SD_STATUS_NORMAL                   3             /**< ���� */

/**@brief ����������ģʽ */
#define IMOS_MW_MANAGE_SERVER_PROTOCOL_IMOS         0             /**< ����Э��ΪIMOS */
#define IMOS_MW_MANAGE_SERVER_PROTOCOL_GB           1             /**< ����Э��ΪGB */
#define IMOS_MW_MANAGE_SERVER_PROTOCOL_NONE         2             /**< ����Э��Ϊ�� */

/**@brief ץ����Ƭ���� */
#define IMOS_MW_CAPTURE_AUTO_TYPE                   0             /**< �Զ�ץ�ģ���������ץ�ġ����ߴ��� */
#define IMOS_MW_CAPTURE_MANUAL_TYPE                 1             /**< �ֶ�ץ�� */

/**@brief ��ͨ�����������ϱ������������� */ 
#define IMOS_MW_CAR_TYPE_NUM                         12           /**< ����������Ŀ */
#define IMOS_MW_LPR_COLOR_NUM                        5            /**< ������ɫ��Ŀ */
#define IMOS_MW_CAR_SPEED_NUM                        2            /**< ������Ŀ */

#if 0
#endif
/*************************************** ������� ********************************************/
/**@brief ���ò��������� */
/* ϵͳ����:  �汾���� ������ģʽ�����油¼ */
#define IMOS_MW_SYSTEM_TIME_INFO            0          /**< ��ȡ/���� ϵͳʱ��(��ʱ����Ϣ)����Ӧ�ṹ����: IMOS_MW_SYSTEM_TIME_INFO_S */
#define IMOS_MW_SYSLOG_SERVER_INFO          1          /**< ��ȡ/���� syslog��������Ϣ����Ӧ�ṹ����: IMOS_MW_SYSLOG_SERVER_INFO_S */ 
#define IMOS_MW_MANAGE_SERVER_CFG           3          /**< ��ȡ/���� ������������ã���Ӧ�ṹ����: IMOS_MW_MANAGE_SERVER_CFG_S */
#define IMOS_MW_DEVICE_INSATLLATION_INFO    5          /**< ��ȡ/���� �豸��װ��Ϣ���μ�: IMOS_MW_DEVICE_INSATLLATION_INFO_S */
#define IMOS_MW_BM_SERVER_CFG               6          /**< ��ȡ/���� BM���������ã���Ӧ�ṹ����: IMOS_MW_BM_SERVER_CFG_S */
#define IMOS_MW_DEVICE_NAME_CFG             7          /**< ��ȡ/���� �豸�������ã���Ӧ�ṹ����: IMOS_MW_DEVICE_NAME_CFG_S */
#define IMOS_MW_NTP_SERVER_CFG              8          /**< ��ȡ/���� NTP���������ã���Ӧ�ṹ����: IMOS_MW_NTP_SERVER_CFG_S  */
#define IMOS_MW_DCOUT_CFG                   9          /**< ��ȡ/���� ��Դ������ã���Ӧ�ṹ����: IMOS_MW_DCOUT_CFG_S */

/* ���ڡ����ڡ�͸��ͨ�� */
#define IMOS_MW_SERIAL_CFG                  10         /**< ��ȡ/���� �������ã���Ӧ�ṹ����: IMOS_MW_SERIAL_CFG_S */
#define IMOS_MW_TRANSPORT_CFG               11         /**< ��ȡ/���� ͸��ͨ�����ã���Ӧ�ṹ����: IMOS_MW_TRANSPORT_CFG_S */
#define IMOS_MW_NETWORK_INTERFACE_CFG       12         /**< ��ȡ/���� �������ã���Ӧ�ṹ����: IMOS_MW_NETWORK_INTERFACE_CFG_S */
#define IMOS_MW_UNP_CFG                     13         /**< ��ȡ/���� UNP���ã���Ӧ�ṹ����: IMOS_MW_UNP_CFG_S */ 
#define IMOS_MW_WIFI_CFG                    14         /**< ��ȡ/���� WiFi�������� ��Ӧ�ṹ����: IMOS_MW_WIFI_CFG_S */

#define IMOS_MW_MULTI_TRANSPORT_CFG         2005      /**< ��ȡ/���� ��͸��ͨ�����ã���Ӧ�ṹ����: IMOS_MW_MULTI_TRANSPORT_CFG_S */    
     
/* ͼ��������� */
#define IMOS_MW_EXPOSURE_CFG                20         /**< ��ȡ/���� �ع��������Ӧ�ṹ����: IMOS_MW_EXPOSURE_CFG_S */
#define IMOS_MW_EXPOSURE_CFG_VIDEO          507        /**< ��ȡ/���� �ع���Ƶ��������Ӧ�ṹ����: IMOS_MW_EXPOSURE_CFG_VIDEO */
#define IMOS_MW_EXPOSURE_CFG_IMAGE          508        /**< ��ȡ/���� �ع�ͼ���������Ӧ�ṹ����: IMOS_MW_EXPOSURE_CFG_IMAGE */

#define IMOS_MW_WHITE_BALANCE_CFG           21         /**< ��ȡ/���� ��ƽ���������Ӧ�ṹ����: IMOS_MW_WHITE_BALANCE_CFG_S */
#define IMOS_MW_IMAGE_ENHANCE_CFG           22         /**< ��ȡ/���� ͼ����ǿ��������Ӧ�ṹ����: IMOS_MW_IMAGE_ENHANCE_CFG_S */
#define IMOS_MW_FOCUS_CFG                   23         /**< ��ȡ/���� �Խ���������Ӧ�ṹ����: IMOS_MW_FOCUS_CFG_S */

/* ��Ƶ���롢��Ƶ����������OSD */
#define IMOS_MW_VIDEOIN_MODE_CFG            30         /**< ��ȡ/���� ��Ƶ������ʽ����Ӧ�ṹ����: IMOS_MW_VIDEOIN_MODE_CFG_S */
#define IMOS_MW_VIDEO_ENCODER_CFG           31         /**< ��ȡ/���� ��Ƶ����������ã���Ӧ�ṹ����:IMOS_MW_VIDEO_ENCODER_CFG_S */
#define IMOS_MW_AUDIO_IN_CFG                32         /**< ��ȡ/���� ��Ƶ����������ã���Ӧ�ṹ����:IMOS_MW_AUDIO_IN_CFG_S */
#define IMOS_MW_OSD_STYLE_CFG               35         /**< ��ȡ/���� ����OSD��ʽ���ã���Ӧ�ṹ����: IMOS_MW_OSD_STYLE_CFG_S */
#define IMOS_MW_INFO_OSD_CFG                36         /**< ��ȡ/����/ɾ�� ����OSD���ã���Ӧ�ṹ����: IMOS_MW_INFO_OSD_CFG_S */
    
/* �������: �������á��������� */
#define IMOS_MW_PHOTO_SERVER_CFG            61         /**< ��ȡ/���� ��Ƭ���շ�������Ϣ����Ӧ�ṹ����: IMOS_MW_PHOTO_SERVER_CFG_S */
#define IMOS_MW_FLASH_LIGHT_CFG             62         /**< ��ȡ/���� ��������ã���Ӧ�ṹ����: IMOS_MW_FLASH_LIGHT_CFG_S */
#define IMOS_MW_VEHICLE_DETECTOR_CFG        63         /**< ��ȡ/���� ���������ã���Ӧ�ṹ����: IMOS_MW_VEHICLE_DETECTOR_CFG_S */
#define IMOS_MW_RADAR_CFG                   64         /**< ��ȡ/���� �״����ã���Ӧ�ṹ����: IMOS_MW_RADAR_CFG_S */
#define IMOS_MW_CAMERA_CAPTURE_CFG          65         /**< ��ȡ/���� ץ�����ã���Ӧ�ṹ����: IMOS_MW_CAMERA_CAPTURE_CFG_S */
#define IMOS_MW_VIDEO_DETECT_CFG            66         /**< ��ȡ/���� ��Ƶ������ã���Ӧ�ṹ����: IMOS_MW_VIDEO_DETECT_CFG_S */
#define IMOS_MW_PLATE_IDENTIFY_CFG          67         /**< ��ȡ/���� ����ʶ�����ã���Ӧ�ṹ����: IMOS_MW_PLATE_IDENTIFY_CFG_S */
#define IMOS_MW_DRIVEWAY_CFG                68         /**< ��ȡ/���� �������ã���Ӧ�ṹ����: IMOS_MW_DRIVEWAY_CFG_S */
#define IMOS_MW_SUBDEVICE_SWITCH_CFG        69         /**< ��ȡ/���� ���迪�����ã���Ӧ�ṹ����: IMOS_MW_SUBDEVICE_SWITCH_CFG_S */
#define IMOS_MW_LASER_CFG                   70         /**< ��ȡ/���� �������ã���Ӧ�ṹ����: IMOS_MW_LASER_CFG_S */
#define IMOS_MW_CAPTURE_FORMAT_CFG          71         /**< ��ȡ/���� ץ��ͼƬ�������ã���Ӧ�ṹ����: IMOS_MW_CAPTURE_FORMAT_CFG_S */
#define IMOS_MW_POLARIZER_CFG               72         /**< ��ȡ/���� ƫ�����ã���Ӧ�ṹ����: IMOS_MW_POLARIZER_CFG_S */
#define IMOS_MW_ND_FILTER_CFG               73         /**< ��ȡ/���� ND�˾����ã���Ӧ�ṹ����: IMOS_MW_ND_FILTER_CFG_S */

/* �羯���: ��ƵԴ���á��������á�Υ������ */
#define IMOS_MW_CAMERA_BASIC_CFG            101        /**< ��ȡ/���� �������������, ��Ӧ�ṹ����: IMOS_MW_CAMERA_BASIC_CFG_S */ 
#define IMOS_MW_EP_VIDEODETECT_CFG          102        /**< ��ȡ/���� ��Ƶ������ã���Ӧ�ṹ����: IMOS_MW_EP_VIDEODETECT_CFG_S */
#define IMOS_MW_EP_DRIVEWAY_CFG             103        /**< ��ȡ/���� �������ã���Ӧ�ṹ����: IMOS_MW_EP_DRIVEWAY_CFG_S */
#define IMOS_MW_DETECTAREA_CFG              104        /**< ��ȡ/���� �����������, ��Ӧ�ṹ����: IMOS_MW_DETECTAREA_CFG_S */ 
#define IMOS_MW_TRIGGERLINE_CFG             105        /**< ��ȡ/���� ����������, ��Ӧ�ṹ����: IMOS_MW_TRIGGERLINE_CFG_S */ 
#define IMOS_MW_DRIVEWAYLINE_CFG            106        /**< ��ȡ/���� ����������, ��Ӧ�ṹ����: IMOS_MW_DRIVEWAYLINE_CFG_S */
#define IMOS_MW_TRAFFICLIGHT_CFG            107        /**< ��ȡ/���� ���̵�����, ��Ӧ�ṹ����: IMOS_MW_TRAFFICLIGHT_CFG_S */ 
#define IMOS_MW_VEHICLETRACK_CFG            108        /**< ��ȡ/���� ������������, ��Ӧ�ṹ����: IMOS_MW_VEHICLETRACK_CFG_S */
#define IMOS_MW_TOLLGATE_CFG                109        /**< ��ȡ/���� ����ץ������, ��Ӧ�ṹ����: IMOS_MW_TOLLGATE_CFG_S */
#define IMOS_MW_PECCANCY_CAPTURE_CFG        110        /**< ��ȡ/���� Υ��ץ������, ��Ӧ�ṹ����: IMOS_MW_PECCANCY_CAPTURE_CFG_S */
#define IMOS_MW_CAPTURE_IMGPROC_CFG         111        /**< ��ȡ/���� Υ��ץ��ͼƬ��������, ��Ӧ�ṹ����: IMOS_MW_CAPTURE_IMGPROC_CFG_S */
#define IMOS_MW_TRAFFICLIGHT_INTENSIFY_CFG  112        /**< ��ȡ/���� ���̵Ƽ�ǿ����, ��Ӧ�ṹ����: IMOS_MW_TRAFFICLIGHT_INTENSIFY_S */
/* �洢���: �洢���� */
#define IMOS_MW_LOCAL_STOR_INFO             150         /**< ��ȡ/���� ���ش洢��Ϣ����Ӧ�ṹ����: IMOS_MW_LOCAL_STOR_INFO_S */
#define IMOS_MW_STOR_NAS_CFG                151         /**< ��ȡ/���� �洢������Ϣ���μ���IMOS_MW_NAS_STOR_CFG_S */      
#define IMOS_MW_FTP_CFG                     152         /**< ��ȡ/���� FTP���ã��μ���IMOS_MW_FTP_CFG_S */ 

#define IMOS_MW_DRIVEWAY_EXPAND_CFG         506         /**< ��ȡ/���� ������չ���ã���Ӧ�ṹ����: IMOS_MW_DRIVEWAY_EXPAND_CFG_S */

#define IMOS_MW_RELEASE_STRATEGY_CFG        600       /**< ��ȡ/���� �ڰ��������в��ԣ���Ӧ�ṹ����: IMOS_MW_RELEASE_STRATEGY_CFG_S */
#define IMOS_MW_WHITE_LIST_CFG              601       /**< ��ȡ/���� ����ڰ��������ã���Ӧ�ṹ����: IMOS_MW_GATE_WHITE_LIST_CFG_S */
#define IMOS_MW_BLACK_LIST_CFG              602       /**< ��ȡ/���� ����ں��������ã���Ӧ�ṹ����: IMOS_MW_GATE_BLACK_LIST_CFG_S*/

#if 0
#endif
/*************************************** �豸ά����� ********************************************/
/*@brief ͨ��CODE����*/
#define IMOS_MW_CODE_LEN                    48   

/*@brief ͨ�������ַ������� */
#define IMOS_MW_NAME_LEN                    64     

/*@brief �ļ������ַ������� */
#define IMOS_MW_FILE_NAME_LEN               IMOS_MW_NAME_LEN    

/*@brief ��Դ������Ϣ�ַ������� */
#define IMOS_MW_RES_CODE_LEN                IMOS_MW_CODE_LEN

/*@brief ϵͳĬ��·������ */
#define IMOS_MW_DEFAULT_PATH_LEN            256

/*@brief ϵͳ����ļ������� */
#define IMOS_MW_FILE_NAME_LEN_MAX          256
/*@brief SDK �ͻ��˰汾�ų��� */
#define IMOS_MW_SDK_CLIENT_VERSION_LEN      32

/*@brief IP��ַ��Ϣ�ַ������� 
  * IPV4: "192.168.0.102"
  * IPV6: "A1:0:8:100:200:0:0:321A"
  * ����: "AS_server_hangzhou" */
#define IMOS_MW_IPADDR_LEN                  64  

/*@brief ע��VMʱ�豸ID ����  */
#define IMOS_MW_DEVICE_ID_LEN               48
/*@brief ע�������������� ����  */
#define IMOS_MW_MANAGE_SERVER_PASSWD_LEN    16

/*@brief SNMP ����������  */
#define IMOS_MW_COMMUNITY_LEN               32

/*@brief ʱ�� ��ʽ */
#define IMOS_MW_TIME_FORMAT_HHMMSS          0        /**< XX:XX:XX �������й���������Ӣ����*/
#define IMOS_MW_TIME_FORMAT_HHMMSS_PM       1        /**< XX:XX:XX p.m. �����ڳ�����������������������*/
#define IMOS_MW_TIME_FORMAT_HH_MM_SS_PM     2        /**< XX:XX:XX PM �����ڳ�Ӣ�������Ӣ�����*/
#define IMOS_MW_TIME_FORMAT_PM_HHMMSS       3        /**< PM XX:XX:XX ����������(�¼���)*/
#define IMOS_MW_TIME_FORMAT_HH_MM_SS        4        /**< ���� XX:XX:XX ����������(̨��)*/

/*@brief ���� ��ʽ */
#define IMOS_MW_DATE_FORMAT_YYYYMMDD        0        /**< XXXX-XX-XX ������ */
#define IMOS_MW_DATE_FORMAT_MMDDYYYY        1        /**< XX-XX-XXXX ������ */
#define IMOS_MW_DATE_FORMAT_YYYY_MM_DD      2        /**< XXXX��XX��XX�� */
#define IMOS_MW_DATE_FORMAT_MM_DD_YYYY      3        /**< XX��XX��XXXX�� */
#define IMOS_MW_DATE_FORMAT_YYYY_MM_DD_XX   4        /**< XXXX��XX��XX�� ����X */
/*@brief �汾��Ϣ���� */
#define IMOS_MW_VERSION_LEN                 256 

/*@brief �û�����󳤶� */
#define IMOS_MW_USER_NAME_LEN               32   

/*@brief �û�������󳤶� */
#define IMOS_MW_USER_PASSWORD_LEN           32  

/**@brief �û����� */
#define IMOS_MW_USERLEVEL_ADMINISTRATOR     0        /**< ����Ա */
#define IMOS_MW_USERLEVEL_OPERATOR          1        /**< ����Ա */
#define IMOS_MW_USERLEVEL_USER              2        /**< ��ͨ�û� */
#define IMOS_MW_USERLEVEL_ANONYMOUS         3        /**< �����û� */
#define IMOS_MW_USERLEVEL_EXTENDED          4        /**< ��չ�û� */

/**@brief �������� */
#define IMOS_MW_PARAM_TYPE_OPTICS_VIDEO     0        /**< ��Ƶ��ͼ����� */
#define IMOS_MW_PARAM_TYPE_OPTICS_PICTURE   1        /**< ��Ƭ��ͼ����� */
#define IMOS_MW_PARAM_TYPE_PECCANCY         2        /**< Υ��Ĭ�ϲ��� */

/**@brief ��־���� */
#define IMOS_SDK_LOG_CLOSE                  0        /**< �ر���־ */
#define IMOS_SDK_LOG_DEBUG                  1        /**< debug���� */
#define IMOS_SDK_LOG_INFO                   2        /**< info���� */
#define IMOS_SDK_LOG_WARN                   3        /**< warn���� */
#define IMOS_SDK_LOG_ERROR                  4        /**< error���� */
#define IMOS_SDK_LOG_FATAL                  5        /**< fatal���� */

#if 0
#endif
/**************************************** ������� ***************************************/
/**@brief �������� */
#define IMOS_MW_SERIAL_TYPE_RS232                    1             /**< rs232 */
#define IMOS_MW_SERIAL_TYPE_RS422                    2             /**< rs422 */
#define IMOS_MW_SERIAL_TYPE_RS485                    3             /**< rs485 */

/**@brief ����ģʽ */
#define IMOS_MW_SERIAL_MODE_PTZ                      1             /**< ��̨���� */
#define IMOS_MW_SERIAL_MODE_TRANS                    2             /**< ͸��ͨ�� */
#define IMOS_MW_SERIAL_MODE_CONSOLE                  3             /**< ����̨ */
#define IMOS_MW_SERIAL_MODE_COMMON                   4             /**< ͨ�ô��� */
#define IMOS_MW_SERIAL_MODE_VEHICLE_DETECTOR_H       5             /**< ������ H, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_VEHICLE_DETECTOR_S       6             /**< ������ S, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_RADAR_C                  7             /**< �״� C, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_LASER                    8             /**< ����, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_CASEALARM                9             /**< ����澯 */  
#define IMOS_MW_SERIAL_MODE_TRAFFICLIGHT_S           10            /**< ����źż���� S */  
#define IMOS_MW_SERIAL_MODE_RADAR_A                  11            /**< �״� A, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_OSD                      12            /**< OSD���� */
#define IMOS_MW_SERIAL_MODE_RADAR_H                  13            /**< �״� H, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_VEHICLE_DETECTOR_Q       14            /**< ������ Q, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_RADAR_C_CONTROLLER       15            /**< �״� C������, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_LOCAL_PTZ_CTRL           16            /**< ������̨���� */
#define IMOS_MW_SERIAL_MODE_RFID                     17            /**< RFID, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_VEHICLE_DETECTOR_U       18            /**< ������ U, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_TRAFFICLIGHT_U           19            /**< ����źż���� U */
#define IMOS_MW_SERIAL_MODE_RADAR_W                  20            /**< �״� W, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_MODE_RADAR_Z                  21            /**< �״� Z, ʹ��ģʽ�μ�: IMOS_MW_SUBDEVICE_XXX */
#define IMOS_MW_SERIAL_SWITCHVALUE_TO_485            22            /**< ���迪����ת485 */
#define IMOS_MW_SERIAL_MODE_COUNT                    22            /**< ����ģʽ������ */ 

/*************************************** ������� ********************************************/
/*@brief PPPoE�û�������  */
#define IMOS_MW_PPPOE_USERNAME_LEN               32  

/*@brief PPPoE���볤�� */
#define IMOS_MW_PPPOE_PASSWORD_LEN               32

/*@brief UNP�û�������  */
#define IMOS_MW_UNP_USERNAME_LEN                 32  

/*@brief UNP���볤�� */
#define IMOS_MW_UNP_PASSWORD_LEN                 32

/*@brief WIFI SSID���� */
#define IMOS_MW_WIFI_SSID_LEN                    44

/*@brief WIFI KEY���� */
#define IMOS_MW_WIFI_KEY_LEN                     44

/* һ��ɨ�����֧�ֵ������������ */
#define IMOS_MW_WIFI_NETWORK_MAX_COUNT           40


/**@brief IP��ȡ��ʽ */
#define IMOS_MW_IP_GET_TYPE_STATIC               0        /**< ��̬��ȡ */
#define IMOS_MW_IP_GET_TYPE_PPPOE                1        /**< pppoe���� */
#define IMOS_MW_IP_GET_TYPE_DHCP                 2        /**< DHCP���� */

/**@brief �������� */
#define IMOS_MW_PORT_MODE_ELECTRONIC             0        /**< ��� */
#define IMOS_MW_PORT_MODE_FIBER                  1        /**< ��� */
#define IMOS_MW_PORT_MODE_EPON                   2        /**< EPON */
#define IMOS_MW_PORT_MODE_LRE                    3        /**< LRE��� */

/**@brief ���ڹ���ģʽ */
#define IMOS_MW_ETH_WORKMODE_AUTO_NEGOTIATION    0    /**< ��Э�� */
#define IMOS_MW_ETH_WORKMODE_10M_FULL            1    /**< 10M FULL */
#define IMOS_MW_ETH_WORKMODE_10M_HALF            2    /**< 10M HALF */
#define IMOS_MW_ETH_WORKMODE_100M_FULL           3    /**< 100M FULL */
#define IMOS_MW_ETH_WORKMODE_100M_HALF           4    /**< 100M HALF */
#define IMOS_MW_ETH_WORKMODE_1000M_FULL          5    /**< 1000M FULL */

/*@brief ���켣�����*/
#define IMOS_MW_TRACK_POINT_NUM                64      

/*@brief Ԥ��λ������Ϣ��󳤶� */ 
#define IMOS_MW_PTZ_PRESET_NAME                64  

/*@brief ��̨Э�����Ƴ��� */ 
#define IMOS_MW_PTZ_NAME_LEN                   16

/*@brief �ƻ������ĸ�ʱ��Σ�Ѳ���ƻ��������ƻ��� */ 
#define IMOS_MW_TIME_SECTION_NUM               4  

/*@brief ʱ���ַ������� */ 
#define IMOS_MW_TIME_LEN                       12    

/*@brief �����ַ������� */ 
#define IMOS_MW_DAY_LEN                        12   

/*@brief �ܵ����� */
#define IMOS_MW_WEEK_DAY                       7

/*@brief ��չ�ƻ���������� */
#define IMOS_MW_EXPLAN_DAYS                    64

#if 0
#endif
/*************************************** ͼ����� ********************************************/
/**@brief ͼ������ */
#define IMOS_MW_IMAGE_TYPE_VIDEO                0       /**< ��Ƶͼ�� */
#define IMOS_MW_IMAGE_TYPE_CAPTURE              1       /**< ץ��ͼ�� */
#define IMOS_MW_IMAGE_TYPE_COUNT                2       /**< ͼ�����͸��� */

/**@brief ��ƽ��ģʽ */
#define IMOS_MW_WB_AUTO                         0       /**< �Զ���ƽ�� */
#define IMOS_MW_WB_FINETUNE                     1       /**< ΢����ƽ�� */
#define IMOS_MW_WB_INDOOR                       2       /**< ���ڰ�ƽ�� */
#define IMOS_MW_WB_OUTDOOR                      3       /**< �����ƽ�� */
#define IMOS_MW_WB_AUTO_MERCURY                 4       /**< �Զ�ˮ���ư�ƽ�� */ 
#define IMOS_MW_WB_FINETUNE_NIGHT               5       /**< ҹ��΢����ƽ��  */

/**@brief �Խ�ģʽ */
#define IMOS_MW_FOCUS_AUTO                      0       /**< �Զ��Խ� */
#define IMOS_MW_FOCUS_MANUAL                    1       /**< �ֶ��Խ� */
#define IMOS_MW_FOCUS_PUSH                      2       /**< һ���Խ� */
#define IMOS_MW_FOCUS_PUSH_NIGHTINFRARED        3       /**< һ���Խ�(ҹ�����) */

/**@brief ����ֵ��λ */
#define IMOS_MW_SHUTTER_UNIT_MICSEC             0       /**< ΢�� */
#define IMOS_MW_SHUTTER_UNIT_DENOMINATOR        1       /**< ��ĸ��ʾ������λ: 1/�� */

/**@brief �ع�ģʽ */
#define IMOS_MW_EXPOSURE_AUTO                   0       /**< �Զ��ع� */
#define IMOS_MW_EXPOSURE_CUSTOM                 1       /**< �Զ����ع� */
#define IMOS_MW_EXPOSURE_SHUTTER                2       /**< �������� */
#define IMOS_MW_EXPOSURE_IRIS                   3       /**< ��Ȧ���� */
#define IMOS_MW_EXPOSURE_GAIN                   4       /**< �������� */
#define IMOS_MW_EXPOSURE_INDOOR_50HZ            5       /**< ����50HZ */
#define IMOS_MW_EXPOSURE_INDOOR_60HZ            6       /**< ����60HZ */
#define IMOS_MW_EXPOSURE_MANUAL                 7       /**< �ֶ��ع� */

/**@brief ��̬ģʽ */
#define IMOS_MW_WDR_DISABLE                     0       /**< �ر� */
#define IMOS_MW_WDR_ENABLE                      1       /**< ���� */
#define IMOS_MW_WDR_AUTO                        2       /**< �Զ� */

/**@brief ���ģʽ */
#define IMOS_MW_METERING_CENTER                 0       /**< ����Ȩ�� */
#define IMOS_MW_METERING_AREA                   1       /**< ������Ȩ�� */
#define IMOS_MW_METERING_HME                    2       /**< ǿ������ */
#define IMOS_MW_METERING_VEHICLE                3       /**< ������� */

/**@brief ��ҹģʽ */
#define IMOS_MW_DAYNIGHT_AUTO                   0       /**< �Զ�ģʽ */
#define IMOS_MW_DAYNIGHT_COLOR                  1       /**< ��ɫ */
#define IMOS_MW_DAYNIGHT_BW                     2       /**< �ڰ� */

/**@brief ���ģʽ */
#define IMOS_MW_SHARPNESS_AUTO                  0       /**< �Զ�ģʽ */
#define IMOS_MW_SHARPNESS_MANUAL                1       /**< �ֶ�ģʽ */

/**@brief ����ģʽ */
#define IMOS_MW_MIRROR_MODE_NONE                0       /**< ����ģʽ */
#define IMOS_MW_MIRROR_MODE_FLIP                1       /**< ��ֱ��ת */
#define IMOS_MW_MIRROR_MODE_MIRROR              2       /**< ˮƽ��ת */
#define IMOS_MW_MIRROR_MODE_FLIP_MIRROR         3       /**< ��ֱˮƽͬʱ��ת */
#define IMOS_MW_MIRROR_MODE_ROTATE_CLOCKWISE    4       /**< ˳ʱ����ת90�� */

#if 0
#endif
/*************************************** ������� ********************************************/
/*@brief ������Ȧ������� */
#define IMOS_MW_VIRTUAL_COIL_MAX_NUM                 8    

/*@brief �����ʶ���������  */
#define IMOS_MW_LPR_AREA_MAX_NUM                     4 

/*@brief ��󳵵���Ŀ  */
#define IMOS_MW_VEHICLE_WAY_MAX_NUM                  4 

/*@brief ע����Ƭ���շ���������Ϣ�ַ�������  */
#define IMOS_MW_PHOTOSERVER_CODE_LEN                 32     

/*@brief ����·����Ϣ���� */
#define IMOS_MW_ROAD_INFO_LEN                        100     

/*@brief �����Ȧ���� */
#define IMOS_MW_VEHICLE_DETECT_MAX_NUM               8

/*@brief ��󴮿ڸ��� */
#define IMOS_MW_SERIAL_MAX_NUM                       20

/**@brief ��Ƭ���շ�����ͨ��Э������ */
#define IMOS_MW_PHOTOSERVER_UNVIEW1                  0             /**< ����V1����Ӧ���̣�*/                           
#define IMOS_MW_PHOTOSERVER_UNVIEW2                  1             /**< ����V2����Ӧ��ǰtms��*/                           
#define IMOS_MW_PHOTOSERVER_XC                       2             /**< �ȴ� */
#define IMOS_MW_PHOTOSERVER_QST                      3             /**< Ȫ��ͨ */
#define IMOS_MW_PHOTOSERVER_FTP                      4             /**< FTP */
#define IMOS_MW_PHOTOSERVER_ZZ                       5             /**< ��ֱ */

/**@brief ����������ģʽ */
#define IMOS_MW_TRRIGER_MODE_PULLINTO                0             /**< �������� */
#define IMOS_MW_TRRIGER_MODE_PULLOUT                 1             /**< �������� */

/**@brief ������Ȧ(����) ���� */
#define IMOS_MW_VIRTUAL_COIL_AUTO                    0             /**< �Զ� */
#define IMOS_MW_VIRTUAL_COIL_ENABLE                  1             /**< ���� */
#define IMOS_MW_VIRTUAL_COIL_DISABLE                 2             /**< ���� */
#define IMOS_MW_VIRTUAL_COIL_WITH_RADAR_SPEED        3             /**< ��Ƶ��⡢�״���� */

/**@brief ���� ����ģʽ */
#define IMOS_MW_TG_MODE_TRAFFIC                      0             /**< ��ͨ��·���� */
#define IMOS_MW_TG_MODE_TRAFFIC_EXPRESSWAY           1             /**< ����·���� */
#define IMOS_MW_TG_MODE_TOLLGATE                     2             /**< ԰������ */
#define IMOS_MW_TG_MODE_MIX                          3             /**< ��Ͽ��� */

/**@brief �״� ����ģʽ */
#define IMOS_MW_RADAR_MODE_CONTINUOUS                0             /**< ����ģʽ */
#define IMOS_MW_RADER_MODE_FRONT_TRIGGER             1             /**< ��ͷ����ģʽ */
#define IMOS_MW_RADER_MODE_REAR_TRIGGER              2             /**< ��β����ģʽ */
#define IMOS_MW_RADER_MODE_DOUBLE_TRIGGER            3             /**< ˫����ģʽ */

/**@brief �״� �������ģʽ */
#define IMOS_MW_RADAR_DIRECTION_NONE_FILTER          0             /**< ������ */
#define IMOS_MW_RADER_DIRECTION_COME                 1             /**< ��������� */
#define IMOS_MW_RADER_DIRECTION_GONE                 2             /**< ���ȥ���� */

/**@brief �״� ���ٸ�ʽ */
#define IMOS_MW_RADAR_SPEED_SINGLE_BYTE              0             /**< ���ֽ� */
#define IMOS_MW_RADAR_SPEED_DOUBLE_BYTE              1             /**< ˫�ֽ� */
#define IMOS_MW_RADAR_SPEED_ASCII_FORMAT             2             /**< ASCII ��ʽ */

/**@brief �������ģʽ */
#define IMOS_MW_LASER_MEASURE_MODE_SIDE_DUAL         0             /**< ����˫����ģʽ */
#define IMOS_MW_LASER_MEASURE_MODE_TOP_DUAL          1             /**< ����˫����ģʽ */
#define IMOS_MW_LASER_MEASURE_MODE_AVERAGE           2             /**< ƽ��ģʽ */
#define IMOS_MW_LASER_MEASURE_MODE_CENTRAL           3             /**< ����ģʽ */
#define IMOS_MW_LASER_MEASURE_MODE_TOP_SINGLE        4             /**< ���õ�����ģʽ */
#define IMOS_MW_LASER_MEASURE_MODE_SIDE_SINGLE       5             /**< ����������ģʽ */
#define IMOS_MW_LASER_MEASURE_MODE_DEMO              6             /**< ��ʾģʽ */

/**@brief ���������ʽ */
#define IMOS_MW_LASER_MEASURE_WAY_MANUAL             0             /**< �ֶ����� */
#define IMOS_MW_LASER_MEASURE_WAY_AUTO               1             /**< �Զ����� */

/**@brief ������־���� */
#define IMOS_MW_LASER_LOG_CLOSE                      0             /**< �ر� */
#define IMOS_MW_LASER_LOG_OPEN                       1             /**< ���� */

/**@brief ��ƽ�������� */
#define IMOS_MW_LASER_LEVER_TRIGGER_CLOSE            0             /**< �ر� */
#define IMOS_MW_LASER_LEVER_TRIGGER_OPEN             1             /**< ���� */
#if 0
#endif
/******************************* ���Ӿ�����ض��� ***********************************/
/**@brief ������������������ֵ */
#define IMOS_MW_CAMERA_NUM_MAX          	     8
/**@brief �������, �羯����δʹ�ã������������޸ĸ�ֵ */
#define IMOS_MW_DIRECTION_NUM_MAX                1

/**@brief ���������ϰ������� */
#define IMOS_MW_DRIVEWAY_VIRTUALCOIL_NUM         2

/**@brief ������������������ */
#define IMOS_MW_DRIVEWAY_LINE_NUM                2

/**@brief ͨ��ʱ��θ��� */
#define IMOS_MW_COMMON_TIMESECTION_NUM           4

/**@brief ר�ó��� ʱ��θ��� */
#define IMOS_MW_DRIVEWAY_TIMESECTION_NUM         4

/**@brief �ٶȵ������ٶȶ������� */
#define IMOS_MW_DRIVEWAY_SPEED_ADJUST_NUM        5

/**@brief ��������� ���������ֵ */
#define IMOS_MW_DRIVEWAY_NUM_MAX                 8

/**@brief ��������� �����������ֵ */
#define IMOS_MW_DRIVEWAY_LINE_NUM_MAX            (IMOS_MW_DRIVEWAY_NUM_MAX + 1)

/**@brief ������򶥵������� */
#define IMOS_MW_DETECTAREA_POINTNUM_MAX          16

/**@brief ��������� ������������� */
#define IMOS_MW_DETECTAREA_NUM_MAX               8

/**@brief ��������� �궨ˮƽ������ */
#define IMOS_MW_HORIZONTAL_LINE_NUM              2

/**@brief ��������� ���������� */
#define IMOS_MW_TRIGGER_LINE_NUM                 16

/**@brief ��������� Υ��ץ�ĵĴ��������� */
#define IMOS_MW_PECCANCY_TRIGGERLINE_NUM         8

/**@brief Υ�����͵������� */
#define IMOS_MW_PECCANCYTYPE_MAX_NUM             32

/**@brief ���̵� ��ɫ���� */
#define IMOS_MW_TRAFFICLIGHT_COLOR_NUM           3

/**@brief �������̵Ƶ����� �Ƹ��������ֵ */
#define IMOS_MW_LIGHT_NUM_MAX                    8    

/**@brief ����������ĺ��̵� ������������ֵ */
#define IMOS_MW_LIGHTGROUP_NUM_MAX               8

/**@brief ����������ĺ��̵� ģ�������� */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_NUM_MAX    16

/**@brief ���̵�ģ�����Ƴ��� */
#define  IMOS_MW_TRAFFICLIGHT_TEMPLATE_NAME_LEN  32

/*@brief ͬ����λ������Ŀ  */
#define IMOS_MW_ACSYNC_NUM                        4

/**@brief ���̵���ɫ */
#define IMOS_MW_LIGHT_COLOR_RED                  0 
#define IMOS_MW_LIGHT_COLOR_GREEN                1
#define IMOS_MW_LIGHT_COLOR_YELLOW               2

/**@brief ���� */
#define IMOS_MW_DIRECTION_EASTTOWEST             1                  /**< �Զ����� */
#define IMOS_MW_DIRECTION_WESTTOEAST       	     2                  /**< ������ */
#define IMOS_MW_DIRECTION_SOUTHTONORTH           3                  /**< ������ */
#define IMOS_MW_DIRECTION_NORTHTOSOUTH           4                  /**< �Ա����� */
#define IMOS_MW_DIRECTION_SOUTHEASTTONORTHWEST   5                  /**< ���������� */
#define IMOS_MW_DIRECTION_NORTHWESTTOSOUTHEAST   6                  /**< �������� */
#define IMOS_MW_DIRECTION_NORTHEASTTOSOUTHWEST   7                  /**< ���������� */
#define IMOS_MW_DIRECTION_SOUTHWESTTONORTHEAST   8                  /**< �����򶫱� */

/**@brief ͼƬ��֡���� */
#define IMOS_MW_IMAGE_FIELD_TYPE 		         0                  /**< ������ */
#define IMOS_MW_IMAGE_FRAME_TYPE		         1                  /**< ֡���� */

/**@brief ��ƵԴ���� */
#define IMOS_MW_VIDEOSRC_LOCAL                   0                  /**< ���� */
#define IMOS_MW_VIDEOSRC_JPEG                    1                  /**< JPEG */
#define IMOS_MW_VIDEOSRC_MJPEG                   2	                /**< MJPEG */
#define IMOS_MW_VIDEOSRC_AVI                     3	                /**< AVI */

/**@brief ͼƬ��ת */
#define IMOS_MW_IMAGE_ROTATE_NONE                0                  /**< ����ת */                    
#define IMOS_MW_IMAGE_ROTATE_CLOCKWISE_90        1                  /**< ˳ʱ�� 90��*/  
#define IMOS_MW_IMAGE_ROTATE_ANTICLOCKWISE_90    2                  /**< ��ʱ�� 90��*/ 

/**@brief ͼƬɫ�ʿռ� */
#define IMOS_MW_IMAGE_COLORSPACE_JPG             0                  /**< JPG */                    
#define IMOS_MW_IMAGE_COLORSPACE_RGB             1                  /**< RGB */                    
#define IMOS_MW_IMAGE_COLORSPACE_YUV420          2                  /**< YUV420 */  
#define IMOS_MW_IMAGE_COLORSPACE_YUV422          3                  /**< YUV422 */ 
#define IMOS_MW_IMAGE_COLORSPACE_GRAY            4                  /**< GRAY */ 

/**@brief �������� */
#define IMOS_MW_DRIVEWAYTYPE_NORMAL              0                  /**< ��ͨ���� */
#define IMOS_MW_DRIVEWAYTYPE_SINGLE_LINE         1                  /**< �����߳��� */

/**@brief �������� */
#define IMOS_MW_DRIVEWAYTYPE_GENERAL             0                  /**< ��ͨ���� */
#define IMOS_MW_DRIVEWAYTYPE_BUSONLY             1                  /**< ������ר�ó��� */
#define IMOS_MW_DRIVEWAYTYPE_NOTRUNK             2                  /**< �������복�� */
#define IMOS_MW_DRIVEWAYTYPE_NOVEHICLE           3                  /**< �ǻ������� */

/**@brief ������ʻ���� */
#define IMOS_MW_DRIVING_DIRECTION_COME           0                  /**< ����������ʻ����ͷ�� */
#define IMOS_MW_DRIVING_DIRECTION_GONE           1                  /**< ����������ʻ����β�� */
#define IMOS_MW_DRIVING_DIRECTION_BOTH           3                  /**< ˫�� */

/**@brief ��������ʻ���� (�����) */
#define IMOS_MW_DRIVING_TYPE_STRAIGHT            0x1               /**< ֱ�� */
#define IMOS_MW_DRIVING_TYPE_TURN_LEFT           0x2               /**< ��ת */
#define IMOS_MW_DRIVING_TYPE_TURN_RIGHT          0x4               /**< ��ת */

/**@brief ��ǰ���� �Ƿ���ʻ���� (�����) */
#define IMOS_MW_DRIVING_ILLEGAL_TURNLEFT                0x1               /**< ��ֹ��ת */
#define IMOS_MW_DRIVING_ILLEGAL_TURNRIGHT               0x2               /**< ��ֹ��ת */
#define IMOS_MW_DRIVING_ILLEGAL_DIRECT_STRAIGHT         0x4               /**< ��ֱֹ�� */

/**@brief ����������� */
#define IMOS_MW_DETECTAREA_VEHICLE_DETECT        0               /**< ����������� */
#define IMOS_MW_DETECTAREA_FOLLOW                1               /**< �������� */
#define IMOS_MW_DETECTAREA_LOCKAREA              2               /**< ·��궨���� */

/**@brief �������ģʽ */
#define IMOS_MW_VEHICLE_DETECTMODE_VIDEO         0               /**< ��Ƶ��� */

/**@brief ����ʶ��ģʽ: ��δʹ�� */
#define IMOS_MW_PLATEIDENTIFY_MODE_PICTURE       0               /**< ͼƬʶ�� */

/**@brief ��������ģʽ  */
#define IMOS_MW_SPEEDLIMIT_MODE_DRIVEWAY         0              /**< ���������� */
#define IMOS_MW_SPEEDLIMIT_MODE_VEHICLETYPE      1              /**< ���������� */

/**@brief ���� (�����) */
#define IMOS_MW_VEHICLETYPE_SMALL                0x1               /**< С�ͳ� */
#define IMOS_MW_VEHICLETYPE_LARGE                0x2               /**< ���ͳ� */
#define IMOS_MW_VEHICLETYPE_MOTOR                0x4               /**< Ħ�г� */
#define IMOS_MW_VEHICLETYPE_ENHANCE              0x400            /**< �������ԣ���ǿ�����Ƴ��ļ�⣬���������� */
#define IMOS_MW_VEHICLETYPE_REDUCE               0x800            /**< �������ԣ���������������ܽ��ͼ���� */
#define IMOS_MW_VEHICLETYPE_NUM                  8                 /**< �����������ֵ */

/**@brief ������ ���� */
#define IMOS_MW_TRIGGER_LINE_DEFAULT1            1              /**< ������1 */
#define IMOS_MW_TRIGGER_LINE_STOP                2              /**< ֹͣ�� */
#define IMOS_MW_TRIGGER_LINE_STRAIGHT2           3              /**< ֱ�д�����2 */
#define IMOS_MW_TRIGGER_LINE_STRAIGHT3           4              /**< ֱ�д�����3 */
#define IMOS_MW_TRIGGER_LINE_STRAIGHT_DELAY3     5              /**< ֱ�д�����3# */
#define IMOS_MW_TRIGGER_LINE_TURNLEFT2           6              /**< ��ת������2 */
#define IMOS_MW_TRIGGER_LINE_TURNRIGHT2          7              /**< ��ת������2 */

/**@brief ���̵� ʶ��ʽ */
#define IMOS_MW_TRAFFICLIGHT_IDENTIFYMODE_TEMPLATE        0     /**< ģ��ʶ�� */
#define IMOS_MW_TRAFFICLIGHT_IDENTIFYMODE_VIDEO           1     /**< ��Ƶʶ�� */
#define IMOS_MW_TRAFFICLIGHT_IDENTIFYMODE_SERIAL          2     /**< ������ */
#define IMOS_MW_TRAFFICLIGHT_IDENTIFYMODE_MULTISERIAL     3     /**< �മ�� */

/**@brief ���̵� ���� */
#define IMOS_MW_TRAFFICLIGHT_TYPE_SIMPLE                  0     /**< �򵥵� */
#define IMOS_MW_TRAFFICLIGHT_TYPE_COMPLEX                 1     /**< ���ӵ� */
#define IMOS_MW_TRAFFICLIGHT_TYPE_MIX                     2     /**< ��ϵ� */

/**@brief ���̵� ��ɫ (�����) */
#define IMOS_MW_TRAFFICLIGHT_COLOR_RED                    0x1     /**< ��� */
#define IMOS_MW_TRAFFICLIGHT_COLOR_YELLOW                 0x2     /**< �Ƶ� */
#define IMOS_MW_TRAFFICLIGHT_COLOR_GREEN                  0x4     /**< �̵� */

/**@brief ���̵� ��״ */
#define IMOS_MW_TRAFFICLIGHT_SHAPE_ROUND                  0     /**< Բ�� */
#define IMOS_MW_TRAFFICLIGHT_SHAPE_ARROW                  1     /**< ��ͷ */
#define IMOS_MW_TRAFFICLIGHT_SHAPE_BAR                    2     /**< ���� */
#define IMOS_MW_TRAFFICLIGHT_SHAPE_OTHER                  4     /**< ���� */


/**@brief ���̵� IO�˿� */
#define IMOS_MW_TRAFFICLIGHT_IO_NONE     		          0     /**< �� */
#define IMOS_MW_TRAFFICLIGHT_IO_SERIAL1                   1     /**< ����1 */
#define IMOS_MW_TRAFFICLIGHT_IO_SERIAL2                   2     /**< ����2 */   

/**@brief ���̵� ָʾ���� (�����) */
#define IMOS_MW_TRAFFICLIGHT_DIRECT_LEFT                  0x1     /**< ��ת */
#define IMOS_MW_TRAFFICLIGHT_DIRECT_RIGHT                 0x2     /**< ��ת */
#define IMOS_MW_TRAFFICLIGHT_DIRECT_STRAIGHT              0x4     /**< ֱ�� */
#define IMOS_MW_TRAFFICLIGHT_DIRECT_NUM                   3       /**< ������� */


/**@brief ���̵� ���з��� */
#define IMOS_MW_TRAFFICLIGHT_ARRANGE_HORIZATIONAL         0     /**< ���� */
#define IMOS_MW_TRAFFICLIGHT_ARRANGE_VERTICAL             1     /**< ���� */

/**@brief ����� ���ģʽ */
#define IMOS_MW_RUNREDLIGHT_DETECTMODE_GENERAL            0     /**< ��ͨģʽ */
#define IMOS_MW_RUNREDLIGHT_DETECTMODE_3LINE              1     /**< 3��ģʽ */

/**@brief Υ�¼�¼����ģʽ */
#define IMOS_MW_MULTI_BREAKRULE_RECORD_EACH               0     /**< ÿ��Υ�¾���������Υ�¼�¼ */
#define IMOS_MW_MULTI_BREAKRULE_RECORD_HIGHEST            1     /**< ������������ȼ�Υ�µ�Υ�¼�¼ */

/**@brief ���������� */
#define IMOS_MW_DRIVEWAY_LINE_SOLID_WHITE                        0     /**< ��ʵ�� */
#define IMOS_MW_DRIVEWAY_LINE_SOLID_YELLOW                       1     /**< ��ʵ�� */
#define IMOS_MW_DRIVEWAY_LINE_2SOLID_YELLOW                      2     /**< ˫��ʵ�� */
#define IMOS_MW_DRIVEWAY_LINE_L_REALLINE_R_BROKENLINE            3     /**< ˫����ʵ������ */
#define IMOS_MW_DRIVEWAY_LINE_L_BROKENLINE_R_REALLINE            4     /**< ˫��������ʵ�� */
#define IMOS_MW_DRIVEWAY_LINE_L_REALLINE_R_BROKENLINE_WHITE      5     /**< ˫����ʵ������ */
#define IMOS_MW_DRIVEWAY_LINE_L_BROKENLINE_R_REALLINE_WHITE      6     /**< ˫��������ʵ�� */
#define IMOS_MW_DRIVEWAY_LINE_OTHER                              7     /**< ���� */

/**@brief ����Υ������ (�����) */
#define IMOS_MW_VEHICLE_PECCANCY_NONE                     0x0        /**< û��Υ�� */
#define IMOS_MW_VEHICLE_PECCANCY_RUNREDLIGHT              0x1        /**< ����� */
#define IMOS_MW_VEHICLE_PECCANCY_PRESSLINE                0x2        /**< ѹ�� */
#define IMOS_MW_VEHICLE_PECCANCY_OVERLINE                 0x4        /**< Խ��(Υ�±��) */
#define IMOS_MW_VEHICLE_PECCANCY_ILLEGAL_DRIVEWAY         0x8        /**< δ��������ʻ */
#define IMOS_MW_VEHICLE_PECCANCY_TURNLEFT                 0x10       /**< Υ����ת */
#define IMOS_MW_VEHICLE_PECCANCY_TURNRIGHT                0x20       /**< Υ����ת */
#define IMOS_MW_VEHICLE_PECCANCY_CONVERSE                 0x40       /**< ���� */
#define IMOS_MW_VEHICLE_PECCANCY_OVERSPEED                0x80       /**< ���� */
#define IMOS_MW_VEHICLE_PECCANCY_LOWSPEED                 0x100      /**< ���� */
#define IMOS_MW_VEHICLE_PECCANCY_RESTRICT_ODD_EVEN        0x200      /**< ��˫������ */
#define IMOS_MW_VEHICLE_PECCANCY_SPECAIL_DRIVEWAY         0x400      /**< ר�ó��� */
#define IMOS_MW_VEHICLE_PECCANCY_ILLEGALPARK              0x800      /**< Υ��ͣ�� */
#define IMOS_MW_VEHICLE_PECCANCY_DIRECT_STRAIGHT          0x1000     /**< Υ��ֱ�� */ 
#define IMOS_MW_VEHICLE_PECCANCY_BACK_CAR                 0x2000     /**< Υ������ */ 
#define IMOS_MW_VEHICLE_PECCANCY_OVER50PERCENTSPEED       0x4000     /**< ����50% */
#define IMOS_MW_VEHICLE_PECCANCY_PRESS_2SOLID_YELLOW      0x8000     /**< ѹ˫���� */
#define IMOS_MW_VEHICLE_PECCANCY_PRESS_SOLID_YELLOW       0x10000    /**< ѹ������ */
#define IMOS_MW_VEHICLE_PECCANCY_PRESS_STOP_LINE          0x20000    /**< ѹͣ���� */
#define IMOS_MW_VEHICLE_PECCANCY_OVER20PERCENTSPEED       0x40000    /**< ����20% */
#define IMOS_MW_VEHICLE_PECCANCY_OVER100PERCENTSPEED      0x80000    /**< ����100% */
#define IMOS_MW_VEHICLE_PECCANCY_STOPREDLIGHT             0x100000   /**< �����ͣ�� */
#define IMOS_MW_VEHICLE_PECCANCY_RUNREDLIGHT_L            0x200000   /**< ��ת����� */
#define IMOS_MW_VEHICLE_PECCANCY_RUNREDLIGHT_S            0x400000   /**< ֱ�д���� */
#define IMOS_MW_VEHICLE_PECCANCY_RUNREDLIGHT_R            0x800000   /**< ��ת����� */
#define IMOS_MW_VEHICLEPECCANCY_TYPE_NUM                  24         /**< ����Υ�����͸���*/


/**@brief ͼƬ�ϳ�ģʽ */
#define IMOS_MW_IMAGE_COMPOSITE_MODE_NONE                 0          /**< ���ϳ� */
#define IMOS_MW_IMAGE_COMPOSITE_MODE_LEFTRIGHT            1          /**< ����ͼƬ: һ��һ�� */

/**@brief ����ͼƬ���� */
#define IMOS_MW_IMAGE_MAKE_PLATE_COLOR                    0x1          /**< ���ɳ��Ʋ�ɫСͼ */
#define IMOS_MW_IMAGE_MAKE_PLATE_BINARY                   0x2          /**< ���ɳ��ƶ�ֵ��ͼ */
#define IMOS_MW_IMAGE_MAKE_PLATE_FEATURE                  0x4          /**< ���ɳ�����дͼ */
#define IMOS_MW_IMAGE_MAKE_COMPOSE                        0x8          /**< ������Ƭ�ϳ�ͼ */

/**@brief ͼƬ���� */
#define IMOS_MW_IMAGE_PECCANCY                            0          /**< Υ����ΪͼƬ */
#define IMOS_MW_IMAGE_COMPOSITE                           1          /**< �ϳ�ͼƬ */
#define IMOS_MW_IMAGE_TEMP                                2          /**< ��ʱͼƬ */
#define IMOS_MW_IMAGE_TYPE_NUM                            3          /**< ͼƬ���͸��� */

/**@brief ���̵�ģ������ (�����) */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_STRAIGHT            0x1        /**< ֱ��ͨ�� */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_NOSTRAIGHT          0x2        /**< ֱ�н��� */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_TURNLEFT            0x4        /**< ��תͨ�� */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_NOTURNLEFT          0x8        /**< ��ת���� */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_TURNRIGHT           0x10       /**< ��תͨ�� */
#define IMOS_MW_TRAFFICLIGHT_TEMPLATE_NOTURNRIGHT         0x20       /**< ��ת���� */

/**@brief ץ����������(�����) */
#define IMOS_MW_CAPTURE_PECCANCY                          0x1          /**< Υ�¼�¼ */                   
#define IMOS_MW_CAPTURE_PECCANCY_PASSVEHICLE              0x2          /**< Υ�¹�����¼ */  
#define IMOS_MW_CAPTURE_NOPECCANCY_PASSVEHICLE            0x4          /**< ��Υ�¹�����¼ */  

/**@brief ������� */
#define IMOS_MW_FULLVIEW_CAMERA                           0            /**< ȫ�� */   
#define IMOS_MW_FEATURE_CAMERA                            1            /**< ��д */   

/**@brief ���л��� */
#define IMOS_MW_PRODUCT_TYPE_TG                           0            /**< ���� */   
#define IMOS_MW_PRODUCT_TYPE_EP                           1            /**< �羯 */   

/**@brief ��Ƭ���� */
#define	IMOS_MW_IMAGE_VEHICLE                             1           /**< ������ͼ */
#define IMOS_MW_IMAGE_PLATE_COLOR                         2           /**< ���Ʋ�ɫСͼ */
#define IMOS_MW_IMAGE_PLATE_BINARY                        3           /**< ���ƶ�ֵ��ͼ */

#if 0
#endif
/*************************************** OSD��� ********************************************/
/*@brief OSD �������ݵĳ��� */
#define IMOS_MW_OSD_INFO_LEN                      60    

/*@brief ���� OSD ������ */
#define IMOS_MW_INFO_OSD_MAX_NUM                  3   

/*@brief OSD ��������������� */
#define IMOS_MW_INFO_OSD_LINE_MAX                 8

/*@brief �ڸ� OSD ������ */
#define IMOS_MW_COVER_OSD_MAX_NUM                 8 

/*@brief ������������ */
#define IMOS_MW_OSD_INFO_TYPE_UNUSED              0    /* ��ʹ�� */
#define IMOS_MW_OSD_INFO_TYPE_USERDEF             1    /* �Զ��� */
#define IMOS_MW_OSD_INFO_TYPE_DATETIME            2    /* ʱ������ */
#define IMOS_MW_OSD_INFO_TYPE_PTZOPER             3    /* ��̨������ */
#define IMOS_MW_OSD_INFO_TYPE_PTZCOORDINATS       4    /* ��̨���� */
#define IMOS_MW_OSD_INFO_TYPE_CRUISEINFO          5    /* Ѳ����Ϣ */
#define IMOS_MW_OSD_INFO_TYPE_ZOOMINFO            6    /* �䱶��Ϣ */
#define IMOS_MW_OSD_INFO_TYPE_PRESETINFO          7    /* Ԥ��λ��Ϣ */
#define IMOS_MW_OSD_INFO_TYPE_ALARMINFO           8    /* ������Ϣ */
#define IMOS_MW_OSD_INFO_TYPE_ENCODEINFO          9    /* ������Ϣ */

/*@brief ���� OSD ���� */
#define IMOS_MW_OSD_PIC_TIME                      0     /**< ץ����Ƭ ʱ��OSD */
#define IMOS_MW_OSD_PIC_VEHICLE_SPEED             1     /**< ץ����Ƭ ����OSD */
#define IMOS_MW_OSD_PIC_LIMITED_SPEED             2     /**< ץ����Ƭ ����OSD */
#define IMOS_MW_OSD_PIC_PECCANCY_TYPE             3     /**< ץ����Ƭ Υ������OSD */
#define IMOS_MW_OSD_PIC_VEHICLE_COLOR             4     /**< ץ����Ƭ ������ɫOSD */
#define IMOS_MW_OSD_PIC_VEHICLE_LOGO              5     /**< ץ����Ƭ ����OSD */
#define IMOS_MW_OSD_PIC_VEHICLE_TYPE              6     /**< ץ����Ƭ ����OSD */
#define IMOS_MW_OSD_PIC_DRIVINGTYPE               7     /**< ץ����Ƭ ������ʻ����OSD */
#define IMOS_MW_OSD_PIC_VEHICLE_PLATE             8     /**< ץ����Ƭ ����(����������ɫ) */
#define IMOS_MW_OSD_PIC_VEHICLE_WAYID             9     /**< ץ����Ƭ ������ */
#define IMOS_MW_OSD_PIC_VEHICLE_CAMERAID          10    /**< ץ����Ƭ �����ID */
#define IMOS_MW_OSD_PIC_VEHICLE_DIRECTION         11    /**< ץ����Ƭ ���� */
#define IMOS_MW_OSD_PIC_VEHICLE_CAP_DIRECTION     12    /**< ץ����Ƭ ץ�ķ��� */
#define IMOS_MW_OSD_PIC_VEHICLE_VERIFY            13    /**< ץ����Ƭ ��α�� */
#define IMOS_MW_OSD_PIC_VEHICLE_NAME              14    /**< ץ����Ƭ ���������� */
#define IMOS_MW_OSD_PIC_VEHICLE_SPEED_PERCENT     15    /**< ץ����Ƭ ���ٰٷֱ�OSD */ 
#define IMOS_MW_OSD_MJPEG_VEHICLE_CAP_DIRECTION   16    /**< MJPEG ��ʻ���� */ 
#define IMOS_MW_OSD_MJPEG_VEHICLE_ROAD_MESSAGE    17    /**< MJPEG ·����Ϣ */
#define IMOS_MW_OSD_MJPEG_VEHICLE_TIME            18    /**< MJPEG ʱ�� */
#define IMOS_MW_OSD_PIC_DEVICE_ID                 19    /**< ץ����Ƭ �豸��� */
#define IMOS_MW_OSD_TYPE_BUTT                     20    /**< ֧�ֵ�ץ��OSD���� */

/*@brief OSD ������ʽ */
#define IMOS_MW_OSD_FONT_STYLE_NORMAL             0    /**< ���� */
#define IMOS_MW_OSD_FONT_STYLE_STROKES            1    /**< ��� */ 
#define IMOS_MW_OSD_FONT_STYLE_HOLLOW             2    /**< ���� */

/*@brief OSD �����С */
#define IMOS_MW_OSD_FONT_SIZE_LARGE               0    /**< �� */
#define IMOS_MW_OSD_FONT_SIZE_MIDDLE              1    /**< �� */
#define IMOS_MW_OSD_FONT_SIZE_SMALL               2    /**< С */

/*@brief OSD ��ɫ */
#define IMOS_MW_OSD_COLOR_WHITE                   0    /**< �� */
#define IMOS_MW_OSD_COLOR_RED                     1    /**< �� */
#define IMOS_MW_OSD_COLOR_YELLOW                  2    /**< �� */
#define IMOS_MW_OSD_COLOR_BLUE                    3    /**< �� */
#define IMOS_MW_OSD_COLOR_BLACK                   4    /**< �� */
#define IMOS_MW_OSD_COLOR_GREEN                   5    /**< �� */
#define IMOS_MW_OSD_COLOR_PURPLE                  6    /**< �� */

/*@brief OSD ͸���� */
#define IMOS_MW_OSD_ALPHA_NO                      0    /**< ��͸�� */
#define IMOS_MW_OSD_ALPHA_SEMI                    1    /**< ��͸�� */
#define IMOS_MW_OSD_ALPHA_ALL                     2    /**< ȫ͸�� */

#if 0
#endif
/*************************************** ý������� ********************************************/
/**@brief ����ģʽ */
#define IMOS_MW_TRANSFER_MODE_RTP_UDP           0        /**< UDP��RTP ����ģʽ */
#define IMOS_MW_TRANSFER_MODE_RTP_TCP           1        /**< TCP��RTP ����ģʽ */
#define IMOS_MW_TRANSFER_MODE_UNIVIEW1_TCP      2        /**< TCP��Uniview1 ����ģʽ */
#define IMOS_MW_TRANSFER_MODE_TS_TCP            3        /**< TCP��TS ����ģʽ */
#define IMOS_MW_TRANSFER_MODE_TS_UDP            4        /**< UDP��TS ����ģʽ */
#define IMOS_MW_TRANSFER_MODE_HTTP              5        /**< ����RTP/RTSP/HTTP/TCP���ط�ʽ */
#define IMOS_MW_TRANSFER_MODE_UNIVIEW2_TCP      6        /**< TCP��Uniview2 ����ģʽ */
#define IMOS_MW_TRANSFER_MODE_LOCAL_IMAGE       7        /**< ������Ƭ���� ����ģʽ */
#define IMOS_MW_TRANSFER_MODE_EZR_TMS           8        /**< TCP��Uniview2 ����ģʽ EZRʹ�� */
#define IMOS_MW_TRANSFER_MODE_TMS_IMAGE         9        /**< TMS��Ƭģʽ ����ģʽ */

/**@brief ��ID */
#define IMOS_MW_STREAM_ID_MAIN                  0        /**< ���� */
#define IMOS_MW_STREAM_ID_AUX                   1        /**< ���� */
#define IMOS_MW_STREAM_ID_THIRD                 2        /**< ���� */
#define IMOS_MW_STREAM_ID_FOUTH                 3        /**< ���� */

#if 0
#endif
/**************************************** �洢 (����) ��� ***************************************/
/**@brief �洢�ļ����� */
#define IMOS_MW_STOR_FILE_TYPE_VIDEO        0          /**< ¼�� */

/**@brief �洢���� */
#define IMOS_MW_STOR_POLICY_FULL_STOP       0          /**< ��ֹͣ */
#define IMOS_MW_STOR_POLICY_FULL_REWRITE    1          /**< ������ */

/**@brief �洢ģʽ(����) */
#define IMOS_MW_LOCAL_STOR_AUTO             0          /**< �Զ� */
#define IMOS_MW_LOCAL_STOR_FOREVER          1          /**< ���� */
#define IMOS_MW_LOCAL_STOR_SAVE             2          /**< �������ϴ�����SD���ϵ��ļ� */
#define IMOS_MW_LOCAL_STOR_START            10         /**< ���� */
#define IMOS_MW_LOCAL_STOR_STOP             11         /**< �ر� */

#if 0
#endif
/**************************************** ���ſ��� ***************************************/
/*@brief ���Ŵ��������� */
#define IMOS_MW_MAX_WND_NUM                      9
/**@brief ����״̬ */
#define IMOS_MW_PLAY_STATUS_16_BACKWARD          0       /**< 16���ٺ��˲��� */
#define IMOS_MW_PLAY_STATUS_8_BACKWARD           1       /**< 8���ٺ��˲��� */
#define IMOS_MW_PLAY_STATUS_4_BACKWARD           2       /**< 4���ٺ��˲��� */
#define IMOS_MW_PLAY_STATUS_2_BACKWARD           3       /**< 2���ٺ��˲��� */
#define IMOS_MW_PLAY_STATUS_1_BACKWARD           4       /**< �����ٶȺ��˲��� */
#define IMOS_MW_PLAY_STATUS_HALF_BACKWARD        5       /**< 1/2���ٺ��˲��� */
#define IMOS_MW_PLAY_STATUS_QUARTER_BACKWARD     6       /**< 1/4���ٺ��˲��� */
#define IMOS_MW_PLAY_STATUS_QUARTER_FORWARD      7       /**< 1/4���ٲ��� */
#define IMOS_MW_PLAY_STATUS_HALF_FORWARD         8       /**< 1/2���ٲ��� */
#define IMOS_MW_PLAY_STATUS_1_FORWARD            9       /**< �����ٶ�ǰ������ */
#define IMOS_MW_PLAY_STATUS_2_FORWARD            10      /**< 2����ǰ������ */
#define IMOS_MW_PLAY_STATUS_4_FORWARD            11      /**< 4����ǰ������ */
#define IMOS_MW_PLAY_STATUS_8_FORWARD            12      /**< 8����ǰ������ */
#define IMOS_MW_PLAY_STATUS_16_FORWARD           13      /**< 16����ǰ������ */

/**@brief ý���ļ���ʽ */
#define IMOS_MW_MEDIA_FILE_FORMAT_TS             0       /**< TS��ʽ��ý���ļ� */
#define IMOS_MW_MEDIA_FILE_FORMAT_FLV            1       /**< FLV��ʽ��ý���ļ� */

/**@brief ��Ƶ��ʾģʽ */
#define IMOS_MW_RENDER_MODE_D3D                  0       /**< ʹ��D3D�ķ�ʽ������ʾ */
#define IMOS_MW_RENDER_MODE_DDRAW_NORMAL         1       /**< ʹ��Ddraw��Normal��ʾģʽ */

/**@brief ��Ƶ���ظ�ʽ */
#define IMOS_MW_PIXEL_FORMAT_YUV420              0       /**< YUV420��ʽ��� */
#define IMOS_MW_PIXEL_FORMAT_RGB32               1       /**< RGB32��ʽ��� */

/**@brief ͼ�񲥷������� */
#define IMOS_MW_PICTURE_TIME_PRIORITY            0       /**< ʵʱ������ */
#define IMOS_MW_PICTURE_FLUENCY_PRIORITY         1       /**< ���������� */
#define IMOS_MW_PICTURE_AUTO                     2       /**< �Զ� */
#define IMOS_MW_PICTURE_LOWDELAY                 3       /**< ����ʱ */

/**@brief ���Ŵ�����ʾͼ��ı��� */
#define IMOS_MW_RENDER_SCALE_FULL                0       /**< ͼ������������Ŵ��� */
#define IMOS_MW_RENDER_SCALE_PROPORTION          1       /**< ͼ�񰴱�����ʾ */

/**@brief ��Ƶ֡���� */
#define IMOS_MW_VIDEO_FRAME_TYPE_I               0       /**< I֡ */
#define IMOS_MW_VIDEO_FRAME_TYPE_P               1       /**< P֡ */
#define IMOS_MW_VIDEO_FRAME_TYPE_B               2       /**< B֡ */
/**@brief ʱ������� */
#define IMOS_MW_RENDER_TYPE_FRAME_RATE           0       /**< ʹ��֡�����ݽ�����Ƶ��Ⱦ */
#define IMOS_MW_RENDER_TYPE_ABSOLUTE_TIME        1       /**< ʹ�þ���ʱ�����ݽ�����Ƶ��Ⱦ */

/**@brief ���� �Աȶ�ģʽ */
#define IMOS_MW_PICTURE_CONTRAST_MODE_NONE        0      /**< �����жԱȶȵ��ڴ��� */
#define IMOS_MW_PICTURE_CONTRAST_MODE_BRIGHTNESS  1      /**< ����ģʽ */
#define IMOS_MW_PICTURE_CONTRAST_MODE_SOFTNESS    2      /**< ���ģʽ */

/**@brief ͼƬ��ʽ */
#define IMOS_MW_PICTURE_FORMAT_BMP                0      /**< BMP ��ʽ */ 
#define IMOS_MW_PICTURE_FORMAT_JPEG               1      /**< JPEG ��ʽ�������μ�: IMOS_MW_CAPTURE_FORMAT_CFG_S */
#define IMOS_MW_PICTURE_FORMAT_CUSTOM             10     /**< ������ʽ, ����Ҫ�������� */

/*@brief ��Ƭ��˵�����Ϣ���� */
#define IMOS_MW_IMAGE_BACKEND_OSD                 0    /**< ���� OSD���ṹ����: IMOS_MW_IMAGE_OSD_INFO_S */
#define IMOS_MW_IMAGE_BACKEND_RECTANGLE           1    /**< ���� ��Ϣ��, �ṹ����: IMOS_MW_RECTANGLE_S, ���У����굥λ 0.01% */
#define IMOS_MW_IMAGE_BACKEND_OSD_OBJ             2    /**< ����OSD, ��Ŀ����Ϣ��, �ṹ����: IMOS_MW_IMAGE_OSD_WITH_OBJ_S, ��Ϣ�����굥λ 0.01% */


#if 0
#endif
/**************************************** ��Ƭ�ṹ ***************************************/
#define IMOS_MW_TRAFFIC_PIC_MAX_NUM                          8           /**< �����Ƭ�� */
#define IMOS_MW_UNIVIEW_MAX_TIME_LEN                         18          /**< ��Ƭ�ṹʱ����Ϣ��󳤶� */
#define IMOS_MW_PLACE_NAME_MAX_LEN                           256         /**< ��Ƭ�ṹ�ص�������󳤶� */
#define IMOS_MW_CAR_PLATE_MAX_LEN                            32          /**< ���ƺ�����󳤶� */
#define IMOS_MW_DEV_ID_MAX_LEN                               32          /**< �豸�����󳤶� */
#define IMOS_MW_TOLLGATE_NAME_MAX_LEN                        64          /**< �����豸������󳤶� */
#define IMOS_MW_DIRECTION_NAME_MAX_LEN                       64          /**< ����������󳤶� */
#define IMOS_MW_CAR_VEHICLE_BRAND_LEN                        4           /**< ���������󳤶� */

#define IMOS_MW_STREAM_FORMAT_JPEG              12      /**< JPEG */ 

#if 0
#endif
#ifdef  __cplusplus
}
#endif

#endif /*end of __IMOS_SDK_DEF_H__*/

