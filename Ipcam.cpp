
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include "Ipcam.h"
#include "http.h"

#include "imos_sdk_def.h"
#include "imos_sdk_pub.h"
#include "imos_sdk_func.h"


#include <string>
using namespace std;

string StrIPcam;

#define EAT_INBUFF {\
    int c;\
    do{\
        c = getchar();\
        printf("eat %d\n",c);\
    }while(c != '\n' && c != EOF);\
}

BOOL_T gbLogin = BOOL_FALSE;
IMOS_MW_SYSTEM_TIME_INFO_S gstTime = {0};
unsigned char szTemp[92]= {0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0xfe, 0x02, 0x42, 0x00, 0xa3, 0x06, 0x02, 0x28, 0x00, 0x00, 0x01, 0x39, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
                           0x02, 0x02, 0x01, 0x00, 0x00, 0x0a, 0x1e, 0x00, 0x00, 0x00, 0xd5, 0xe3, 0x41, 0x35, 0x32, 0x39,
                           0x33, 0x5b, 0x02, 0x5c, 0x6e, 0xbd, 0xd8, 0xd6, 0xb9, 0xc8, 0xd5, 0xc6, 0xda, 0x5c, 0x6e, 0x32,
                           0x30, 0x31, 0x38, 0x2d, 0x30, 0x37, 0x2d, 0x32, 0x38, 0xdc, 0x2d, 0x5a};

LONG IMOS_snprintf(INOUT CHAR *pcBuf, IN SIZE_T ulMaxlen, IN const CHAR *pcFormat, ...)
{
    LONG lRet = 0;
    va_list stParalist;

    /* ն */
    va_start(stParalist, pcFormat);

    /* ACEϵͳ */
    lRet = (LONG) vsnprintf(pcBuf, (size_t) ulMaxlen, pcFormat, stParalist);

    /* ͷŲԴ */
    va_end(stParalist);

    return lRet;
}

VOID IMOS_MW_GetVideoData(IN ULONG ulStreamHandle,
                          IN CHAR *pcStreamBuf,
                          IN INT lBufLen)
{
    static ULONG ulTimer = 0;

    ulTimer++;
    if (ulTimer > 100)
    {
        printf("\n video len:%d ulStreamHandle:%lu\n", lBufLen, ulStreamHandle);
        ulTimer = 0;
    }

    return;
}

VOID IMOS_MW_GetPicture(IMOS_MW_MULTI_UNIVIEW_PROTOCOL_HEADER_S *pstUniviewData, ULONG ulStreamHandle)
{
    FILE *pFile = NULL;
    CHAR szNameBuf[64] = {0};
    ULONG i = 0;
    ULONG ulRet = 0;

   /*printf(
          "szCarPlate:%s szTollgateID:%s, szPlaceName:%s, szPassTime:%s"
          "szVehicleBrand:%s, szRedLightStartTime:%s, szRedLightEndTime:%s"
          "ulRecordID:%lu, lDirection:%d, lPlateColor:%d, lLaneID:%d"
          "lPlateType:%d, lVehicleSpeed:%d, lDriveStatus:%d, lVehicleType:%d"
          "lRedLightTime:%d, cVehicleColor:%c, VehicleXY[%lu, %lu][%lu, %lu]"
          "LPRRect[%lu, %lu][%lu, %lu], lMarkedSpeed:%d, lLimitedSpeed:%d ",
          pstUniviewData->szCarPlate, pstUniviewData->szTollgateID, pstUniviewData->szPlaceName, pstUniviewData->szPassTime,
          pstUniviewData->szVehicleBrand, pstUniviewData->szRedLightStartTime, pstUniviewData->szRedLightEndTime,
          pstUniviewData->ulRecordID, pstUniviewData->lDirection, pstUniviewData->lPlateColor, pstUniviewData->lLaneID,
          pstUniviewData->lPlateType, pstUniviewData->lVehicleSpeed, pstUniviewData->lDriveStatus, pstUniviewData->lVehicleType,
          pstUniviewData->lRedLightTime, pstUniviewData->cVehicleColor, pstUniviewData->aulVehicleXY[0],
          pstUniviewData->aulVehicleXY[1], pstUniviewData->aulVehicleXY[2], pstUniviewData->aulVehicleXY[3],
          pstUniviewData->aulLPRRect[0], pstUniviewData->aulLPRRect[1], pstUniviewData->aulLPRRect[2],
          pstUniviewData->aulLPRRect[3], pstUniviewData->lMarkedSpeed, pstUniviewData->lLimitedSpeed);*/

    printf("ucPlateScore:%d ucRearPlateScore:%d lPlateNumber%d\n",pstUniviewData->ucPlateScore, pstUniviewData->ucRearPlateScore, pstUniviewData->lPlateNumber);

    printf("ulStreamHandle:%d\n", ulStreamHandle);

    struct tm *t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);
    char timebuf[50];memset(timebuf,0x00,50);
    sprintf(timebuf,"%4d%02d%02d%02d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);


    for (i = 0; i < pstUniviewData->ulPicNumber; i++)
    {
        //IMOS_snprintf(szNameBuf, sizeof(szNameBuf), "./sdcard/one%d.jpeg", i);
        IMOS_snprintf(szNameBuf, sizeof(szNameBuf), "./sdcard/%d/%s%d.jpeg", ulStreamHandle,timebuf,i);
        pFile = fopen(szNameBuf, "w");
        printf("111 szNameBuf:%s, PicLen:%lu Pdata:%p\n", szNameBuf, pstUniviewData->aulDataLen[i], pstUniviewData->apcData[i]);
        if (NULL != pFile)
        {
            fwrite(pstUniviewData->apcData[i], 1, pstUniviewData->aulDataLen[i], pFile);
            fclose(pFile);
        }

        //http post jpeng stream to lyf server
       // HttpPostjpeng((unsigned char *)(pstUniviewData->apcData[i]),pstUniviewData->aulDataLen[i]);
        

    }

    return;
}

VOID IMOS_MW_GetPicture2(IN ULONG ulXmlLen,
                         IN CHAR *pcPicXmlInfo,
                         IN IMOS_MW_PIC_INFO_S *pstPicData,
                         IN ULONG ulStreamHandle)
{
    ULONG i = 0;
    FILE *pFile = NULL;
    CHAR szNameBuf[64] = {0};

    //printf("\nulPicNum:%lu, pcPicXmlInfo:%s", pstPicData->ulPicNum, pcPicXmlInfo);
    for (i = 0; i < pstPicData->ulPicNum; i++)
    {
        IMOS_snprintf(szNameBuf, sizeof(szNameBuf), "./sdcard/two%d.jpeg", i);
        pFile = fopen(szNameBuf, "w");
        printf("222 szNameBuf:%s, PicLen:%lu, Pdata:%p\n", szNameBuf, pstPicData->astPicInfo[i].ulPicLen,
               pstPicData->astPicInfo[i].pPicData);
        if (NULL != pFile)
        {
            fwrite(pstPicData->astPicInfo[i].pPicData, 1, pstPicData->astPicInfo[i].ulPicLen, pFile);
            fclose(pFile);
        }
    }

    return;
}


void StatusReportCallBackFunEx(CHAR *pcUserID, ULONG ulReportType, VOID *pParam)
{
    /*set status report callback function*/
    switch (ulReportType)
    {
        case IMOS_MW_STATUS_KEEPALIVE:
        {
            /*keepalive*/
            printf("KEEPALIVE:Please login again, pcUserID:%s\n", pcUserID);
            gbLogin = BOOL_FALSE;
            break;
        }

        case IMOS_MW_STATUS_NETWORK_CHANGE:
        {
            /*network change*/
            printf("NETWORK_CHANGE:Please login again, pcUserID:%s\n", pcUserID);
            break;
        }

        case IMOS_MW_STATUS_PLAYER_MEDIA_PROCESS:
        {
            printf("IMOS_MW_STATUS_PLAYER_MEDIA_PROCESS, pcUserID:%s\n", pcUserID);
            break;
            /*XP*/
        }

        case IMOS_MW_STATUS_TRAFFIC_PARAM_REPORT:
        {
            /*int i = 0;
            IMOS_MW_TRAFFIC_PARA_RSLT_S *pstTrafficStatus = (IMOS_MW_TRAFFIC_PARA_RSLT_S *)pParam;
            printf("IMOS_MW_STATUS_TRAFFIC_PARAM_REPORT, pcUserID:%s, szStartTime:%s\n", pcUserID, pstTrafficStatus->szStartTime);
            for (i = 0; i < pstTrafficStatus->ulDriveWayNum; ++i)
            {
                printf("ulLaneID:%lu, ulTrafficFlow:%lu, ulAveSpeed:%lu, fAveHeadTime:%f, fAveTimOcupyRat:%f \n",
                       pstTrafficStatus->astTrParaRslt[i].ulLaneID, pstTrafficStatus->astTrParaRslt[i].ulTrafficFlow,
                       pstTrafficStatus->astTrParaRslt[i].ulAveSpeed, pstTrafficStatus->astTrParaRslt[i].fAveHeadTime,
                       pstTrafficStatus->astTrParaRslt[i].fAveTimOcupyRat);
            }
            */
            printf("IMOS_MW_STATUS_TRAFFIC_PARAM_REPORT, pcUserID:%s\n", pcUserID);
            break;
        }

        case IMOS_MW_STATUS_VEHICLE_PARAM_REPORT:
        {
           /* int i = 0;
            IMOS_MW_VEHICLE_STATE_S *pstVehicleStatus = (IMOS_MW_VEHICLE_STATE_S *)pParam;
            printf("IMOS_MW_STATUS_VEHICLE_PARAM_REPORT, pcUserID:%s, szStatPassTime:%s\n", pcUserID, pstVehicleStatus->szStatPassTime);
            for (i = 0; i < pstVehicleStatus->ulDriveWayNum; ++i)
            {
                printf("aenVehicleState:%d, aulDriveWayCode:%lu \n", pstVehicleStatus->aenVehicleState[i], pstVehicleStatus->aulDriveWayCode[i]);
            }*/

            printf("IMOS_MW_STATUS_VEHICLE_PARAM_REPORT, pcUserID:%s\n", pcUserID);
            break;
        }

        default:
        {
            break;
        }
    }

    return;
}

ULONG pfnStreamData(ULONG ulStreamHandle, CHAR *pcStreamBuf, INT lBufLen)
{
    printf("Stream CallBack[%d] [%s] \n", lBufLen, pcStreamBuf);
    return 0;
}




void *IpcamThread(void *param)
{
    INT lChoice = 0;
    LONG lRet = 0;
    ULONG ulRet = 0;
    ULONG ulStreamHandle = 0xFFFFFFFF;
    ULONG ulCaptureStreamHandle = 0xFFFFFFFF;
    ULONG ulStreamIndex = 0;
    ULONG ulStreamID = 0;
    ULONG ulVideoEncoderCfgLen = 0;
    IMOS_MW_DEVICE_RUN_INFO_S *pstatusInfo = NULL;
    IMOS_MW_BASIC_DEVICE_INFO_S *pstBasicInfo = NULL;
    IMOS_MW_VIDEO_ENCODER_CFG_S stVideoEncoderCfg;
    CHAR szUserID[128] = {0};
/*
    ulRet = IMOS_MW_Initiate();
    if(ERR_COMMON_SUCCEED != ulRet)
    {
        printf("Initiate ret[%lu]\n", ulRet);
        return 0;
    }
*/
    /* Ŀ¼logѡ */
    lRet = mkdir("./sdcard", 0755);
    printf("mkdir lRet:%ld\n", lRet);

    ulRet = IMOS_MW_SetLog(IMOS_SDK_LOG_DEBUG, "./sdcard", 1024);
    printf("IMOS_MW_SetLog[%lu]\n", ulRet);

    /* ״̬ϱص */
    ulRet = IMOS_MW_SetStatusCallback((IMOS_MW_STATUS_REPORT_CALLBACK_PF )StatusReportCallBackFunEx);
    if(ERR_COMMON_SUCCEED != ulRet)
    {
        printf("IMOS_MW_SetStatusCallback\n");
    }

    /* login */
    ulRet = IMOS_MW_Login("admin", "admin", (CHAR *)StrIPcam.c_str(), 0, szUserID);
    if(ERR_COMMON_SUCCEED != ulRet)
    {
        printf("Login error ulRet[%lu]", ulRet);
    }
    else
    {
        gbLogin = BOOL_TRUE;
    }

/*
            ulRet = IMOS_MW_StartMediaStream(szUserID, 0, IMOS_MW_STREAM_ID_AUX, IMOS_MW_TRANSFER_MODE_UNIVIEW1_TCP, "", 0, BOOL_TRUE, &ulCaptureStreamHandle);
            if (ERR_COMMON_SUCCEED != ulRet)
            {
                printf("Start jpeg MediaStream Error[%lu]\n", ulRet);
                gbLogin = BOOL_FALSE;
            }

            printf("StartMediaStream jpeg succed handle[%lu]\n", ulCaptureStreamHandle);

            IMOS_MW_MultiUser_SetPicDataCallback((IMOS_MW_TMS_MULTIUSER_PIC_UPLOAD_PF)IMOS_MW_GetPicture, ulCaptureStreamHandle);
*/
            //IMOS_MW_SetPicDataAndXmlInfoCallback((IMOS_MW_PIC_XML_UPLOAD_PF)IMOS_MW_GetPicture2, ulCaptureStreamHandle);


    //while (lChoice != -1)
    while(1)
    {
        /*
        printf("\n\n*******************************\n");
        printf("*-1:quit                      *\n");
        printf("*01:DeviceRunStatu            *\n");
        printf("*02:System Reboot             *\n");
        printf("*03:Device Basic Info         *\n");
        printf("*04:Start video MediaStream   *\n");
        printf("*05:Stop  video MediaStream   *\n");
        printf("*06:Start jpeg  MediaStream   *\n");
        printf("*07:Stop  jpeg  MediaStream   *\n");
        printf("*08:capture                   *\n");
        printf("*09:get time                  *\n");
        printf("*10:set time                  *\n");
        printf("*11:send date to Com          *\n");
        printf("*12:get/set DrivWayExpandCfg  *\n");
        printf("*13:get/set Exposure Video    *\n");
        printf("*14:get/set Exposure Image    *\n");
        printf("*15:get/set whiteblack Cfg    *\n");
        printf("*16:Export whieblackfile      *\n");
        printf("*17:Import whieblackfile      *\n");
        printf("*******************************\n");
        printf("please chose:");
        if (EOF == scanf("%d", &lChoice))
        {
            printf("scanf error lChoice[%d]!\n", lChoice);
            break;
        }
*/
        lChoice = 1 ; 
        sleep(10);

        if (BOOL_FALSE == gbLogin)
        {
            ulRet = IMOS_MW_Login("admin", "admin", (CHAR *)StrIPcam.c_str(), 0, szUserID);
            if (ERR_COMMON_SUCCEED != ulRet)
            {
                printf("IMOS_MW_Login Failed[%lu]\n", ulRet);
            }
            else
            {
                gbLogin = BOOL_TRUE;
            }

        }

        if (BOOL_TRUE == gbLogin)
        {
            switch(lChoice)
            {
                case 1:
                {
                    pstatusInfo = (IMOS_MW_DEVICE_RUN_INFO_S *)malloc(sizeof(IMOS_MW_DEVICE_RUN_INFO_S));
                    if (NULL == pstatusInfo)
                    {
                        break;
                    }

                    memset(pstatusInfo, 0, sizeof(IMOS_MW_DEVICE_RUN_INFO_S));
                    ulRet = IMOS_MW_GetDeviceStatus(szUserID, 0, IMOS_MW_STATUS_RUN_INFO, (void *)pstatusInfo);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("IMOS_MW_GetDeviceStatus Failed[%lu]\n", ulRet);
                        break;
                    }

                    printf("RunTime:%lud/%luh/%lum\n", pstatusInfo->ulRunTime/60/60/24, pstatusInfo->ulRunTime/60/60%24,\
                                                       pstatusInfo->ulRunTime/60%60);
                    printf("Temperature:%lu 'C\n", pstatusInfo->lDeviceTemperature);
                    printf("CPU:%lu\n", pstatusInfo->ulCPUUtilization);
                    printf("Mem:%lu\n", pstatusInfo->ulMemUtilization);
                    free(pstatusInfo);
                    pstatusInfo = NULL;
                    break;
                }

                case 2:
                {
                    ulRet = IMOS_MW_Reboot(szUserID);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("Reboot Error[%lu]\n", ulRet);
                        break;
                    }
                    printf("Reboot now...press -1 for exit\n");
                    break;
                }

                case 3:
                {
                    pstBasicInfo = (IMOS_MW_BASIC_DEVICE_INFO_S *)malloc(sizeof(IMOS_MW_BASIC_DEVICE_INFO_S));
                    ulRet = IMOS_MW_GetDeviceStatus(szUserID, 0, IMOS_MW_STATUS_BASIC_INFO, (void *)pstBasicInfo);
                    if(ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("GetDeviceStatus Error[%lu]\n", ulRet);
                        break;
                    }
                    printf("Manufacturer   :%s\n", pstBasicInfo->szManufacturer);
                    printf("DeviceModel    :%s\n", pstBasicInfo->szDeviceModel);
                    printf("SerialNumber   :%s\n", pstBasicInfo->szSerialNumber);
                    printf("DeviceMAC      :%s\n", pstBasicInfo->szDeviceMAC);
                    printf("FirmwareVersion:%s\n", pstBasicInfo->szFirmwareVersion);
                    printf("HardwareID     :%s\n", pstBasicInfo->szHardwareID);
                    printf("PCBVersion     :%s\n", pstBasicInfo->szPCBVersion);
                    printf("UbootVersion   :%s\n", pstBasicInfo->szUbootVersion);
                    free(pstBasicInfo);
                    pstBasicInfo = NULL;
                    break;
                }

                case 4:
                {
                    ulRet = IMOS_MW_StartMediaStream(szUserID, 0, IMOS_MW_STREAM_ID_MAIN, IMOS_MW_TRANSFER_MODE_RTP_TCP, "0.0.0.0", 0, 1, &ulStreamHandle);
                    if(ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("\nStart vidoe MediaStream Error[%lu]\n", ulRet);
                        break;
                    }
                    printf("Start vidoe MediaStream succed handle[%lu]\n", ulStreamHandle);

                    IMOS_MW_SetStreamDataCallBack((IMOS_MW_STREAMDATA_CALLBACK_PF )IMOS_MW_GetVideoData, ulStreamHandle);

                    break;
                }

                case 5:
                {
                    ulRet = IMOS_MW_StopMediaStream(szUserID, ulStreamHandle);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("Stop video MediaStream Error[%lu]\n", ulRet);
                    }
                    else
                    {
                        printf("Stop vidoe MediaStream succed handle[%lu]\n", ulStreamHandle);
                    }

                    break;
                }

                case 6:
                {
                    ulStreamID = IMOS_MW_STREAM_ID_AUX;
                    /* ȡͼƬID */
                    for (ulStreamIndex = IMOS_MW_STREAM_ID_AUX; ulStreamIndex <= IMOS_MW_STREAM_ID_FOUTH; ulStreamIndex++)
                    {
                        ulVideoEncoderCfgLen = sizeof(stVideoEncoderCfg);
                        stVideoEncoderCfg.ulStreamID = ulStreamIndex;
                        IMOS_MW_GetConfig(szUserID, IMOS_MW_VIDEO_ENCODER_CFG, 0, &ulVideoEncoderCfgLen, &stVideoEncoderCfg);
                        if (IMOS_MW_STREAM_FORMAT_JPEG == stVideoEncoderCfg.stVideoStreamCfg.ulEncodeFmt)
                        {
                            ulStreamID = ulStreamIndex;
                            break ;
                        }
                    }

                    ulRet = IMOS_MW_StartMediaStream(szUserID, 0, ulStreamID, IMOS_MW_TRANSFER_MODE_UNIVIEW1_TCP, "", 0, BOOL_TRUE, &ulCaptureStreamHandle);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("Start jpeg MediaStream Error[%lu]\n", ulRet);
                        break;
                    }

                    printf("StartMediaStream jpeg succed handle[%lu]\n", ulCaptureStreamHandle);

                    IMOS_MW_MultiUser_SetPicDataCallback((IMOS_MW_TMS_MULTIUSER_PIC_UPLOAD_PF)IMOS_MW_GetPicture, ulCaptureStreamHandle);
                    //IMOS_MW_SetPicDataAndXmlInfoCallback((IMOS_MW_PIC_XML_UPLOAD_PF)IMOS_MW_GetPicture2, ulCaptureStreamHandle);

                    break;
                }

                case 7:
                {
                    ulRet = IMOS_MW_StopMediaStream(szUserID, ulCaptureStreamHandle);
                    if(ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("Stop jpeg MediaStream CaptureStreamHandle:%lu Error[%lu]\n", ulCaptureStreamHandle, ulRet);
                    }
                    else
                    {
                        printf("Stop jpeg MediaStream succed handle[%lu]\n", ulCaptureStreamHandle);
                    }

                    break;
                }

                case 8:
                {
                    ulRet = IMOS_MW_Capture(szUserID, 0, NULL, NULL);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("Capture Error[%lu]\n", ulRet);
                    }
                    else
                    {
                        printf("Capture OK\n");
                    }

                    break;
                }

                case 9:
                {
                    IMOS_MW_SYSTEM_TIME_INFO_S *pcData = &gstTime;
                    ULONG ulLen = sizeof(gstTime);

                    ulRet = IMOS_MW_GetConfig(szUserID, IMOS_MW_SYSTEM_TIME_INFO, 0, &ulLen, pcData);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("IMOS_MW_GetConfig failed");
                    }
                    else
                    {
                        printf("*********lTimeZone:%d, [%d,%d %d,%d,%d,%d]\n", pcData->lTimeZone,
                               pcData->stLocalTime.usYear, pcData->stLocalTime.usMonth,
                               pcData->stLocalTime.usMonthDay, pcData->stLocalTime.usHour,
                               pcData->stLocalTime.usMinute, pcData->stLocalTime.usSecond);
                    }

                    break;
                }

                case 10:
                {
                     IMOS_MW_SYSTEM_TIME_INFO_S *pcData = &gstTime;
                     ULONG ulLen = sizeof(gstTime);

                     pcData->stLocalTime.usHour++;
                     printf("*********lTimeZone:%d, [%d,%d %d,%d,%d,%d]\n", pcData->lTimeZone,
                               pcData->stLocalTime.usYear, pcData->stLocalTime.usMonth,
                               pcData->stLocalTime.usMonthDay, pcData->stLocalTime.usHour,
                               pcData->stLocalTime.usMinute, pcData->stLocalTime.usSecond);
                     ulRet = IMOS_MW_SetConfig(szUserID, IMOS_MW_SYSTEM_TIME_INFO, 0, ulLen, pcData);
                     break;
                }

                case 11:
                {
                    ulRet = IMOS_MW_SendDataToCom(szUserID, (CHAR *)szTemp, 92, 1);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("Send Error[%lu]\n", ulRet);
                    }
                    else
                    {
                        printf("Send  OK\n");
                    }
                    break;
                }

                case 12:
                {
                    #if 0
                    IMOS_MW_EXPOSURE_S stExposure;
                    ULONG ulBufferLen = sizeof(IMOS_MW_EXPOSURE_S);
                    memset(&stExposure, 0, sizeof(IMOS_MW_EXPOSURE_S));
                    ulRet = IMOS_MW_GetConfig(szUserID, IMOS_MW_EXPOSURE_CFG, 0, &ulBufferLen, &stExposure);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("IMOS_MW_EXPOSURE_CFG Error[%lu]\n", ulRet);
                    }
                    else
                    {
                        printf("IMOS_MW_EXPOSURE_CFG  OK\n");
                    }
                    #endif


                    IMOS_MW_DRIVEWAY_EXPAND_CFG_S stDrivWayExpandCfg;
                    ULONG ulBufferLen = sizeof(IMOS_MW_DRIVEWAY_EXPAND_CFG_S);
                    memset(&stDrivWayExpandCfg, 0, sizeof(IMOS_MW_DRIVEWAY_EXPAND_CFG_S));
                    ulRet = IMOS_MW_GetConfig(szUserID, IMOS_MW_DRIVEWAY_EXPAND_CFG, 0, &ulBufferLen, &stDrivWayExpandCfg);

                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("GET_12 IMOS_MW_EXPOSURE_CFG Error[%lu]\n", ulRet);
                    }
                    else
                    {
                        printf("GET_12 IMOS_MW_EXPOSURE_CFG  OK\n");

                        /*for(i = 0; i < 2; i++)
                        {
                            if(2 == stDrivWayExpandCfg.astSingDrivWayExpCfg[0].__unionReserve)
                        {
                          printf("\n");
                          printf("IMOS_MW_CAR_SPEED_LIMIT_S---Reserve:[%lu]\n", stDrivWayExpandCfg.astSingDrivWayExpCfg[i].__unionReserve);
                          printf("IMOS_MW_CAR_SPEED_LIMIT_S---Speed:[%lu]\n", stDrivWayExpandCfg.astSingDrivWayExpCfg[i].unDriveWaySpeedLimit.astVehicleTypeSpeedLimit[0].ulSpeed);
                          printf("IMOS_MW_CAR_SPEED_LIMIT_S---MinSpeed:[%lu]\n", stDrivWayExpandCfg.astSingDrivWayExpCfg[i].unDriveWaySpeedLimit.astVehicleTypeSpeedLimit[0].ulMinSpeedLimit);
                          printf("IMOS_MW_CAR_SPEED_LIMIT_S---Type:[%lu]\n", stDrivWayExpandCfg.astSingDrivWayExpCfg[i].unDriveWaySpeedLimit.astVehicleTypeSpeedLimit[0].ulVehicleType);
                          printf("IMOS_MW_CAR_SPEED_LIMIT_S---CarSpeedLimit---SpeedLimitValue:[%lu]\n",stDrivWayExpandCfg.astSingDrivWayExpCfg[i].unDriveWaySpeedLimit.astVehicleTypeSpeedLimit[0].astCarSpeedLimit[0].ulLimitSpeed);
                          printf("IMOS_MW_CAR_SPEED_LIMIT_S---CarSpeedLimit---TimeSection---Start:[%s]\n",stDrivWayExpandCfg.astSingDrivWayExpCfg[i].unDriveWaySpeedLimit.astVehicleTypeSpeedLimit[0].astCarSpeedLimit[0].stTimeSection.szStartTime);
                          printf("IMOS_MW_CAR_SPEED_LIMIT_S---CarSpeedLimit---TimeSection---End:[%s]\n",stDrivWayExpandCfg.astSingDrivWayExpCfg[i].unDriveWaySpeedLimit.astVehicleTypeSpeedLimit[0].astCarSpeedLimit[0].stTimeSection.szEndTime);
                         }
                        else if(1 == stDrivWayExpandCfg.astSingDrivWayExpCfg[0].__unionReserve)
                        {
                          printf("\n");
                          printf("IMOS_MW_WAY_SPEED_LIMIT_S---Reserve:[%lu]\n", stDrivWayExpandCfg.astSingDrivWayExpCfg[i].__unionReserve);
                          printf("IMOS_MW_WAY_SPEED_LIMIT_S---Speed:[%lu]\n", stDrivWayExpandCfg.astSingDrivWayExpCfg[i].unDriveWaySpeedLimit.stWaySpeedLimit.ulDrivewaySpeedLimit);
                          printf("IMOS_MW_WAY_SPEED_LIMIT_S---MinSpeed:[%lu]\n", stDrivWayExpandCfg.astSingDrivWayExpCfg[i].unDriveWaySpeedLimit.stWaySpeedLimit.ulDrivewayMinSpeedLimit);
                        }*/
                        stDrivWayExpandCfg.astSingDrivWayExpCfg[0].ulExeSpeedAdjustFactor =  stDrivWayExpandCfg.astSingDrivWayExpCfg[0].ulExeSpeedAdjustFactor + 4;
                        stDrivWayExpandCfg.astSingDrivWayExpCfg[0].ulDriWayType = IMOS_MW_DRIVING_TYPE_STRAIGHT;

                        ulRet = IMOS_MW_SetConfig(szUserID, IMOS_MW_DRIVEWAY_EXPAND_CFG, 0, ulBufferLen, &stDrivWayExpandCfg);
                        if (ERR_COMMON_SUCCEED != ulRet)
                        {
                           printf("SET_12 IMOS_MW_DRIVEWAY_EXPAND_CFG Error[%lu]\n", ulRet);
                        }
                        else
                        {
                           printf("SET_12 IMOS_MW_DRIVEWAY_EXPAND_CFG SUCCEED[%lu]\n", ulRet);
                        }

                     }

                     break;
                 }

                case 13:
                {
                    IMOS_MW_EXPOSURE_S stExposure;
                    ULONG ulBufferLen = sizeof(IMOS_MW_EXPOSURE_S);
                    memset(&stExposure, 0, sizeof(IMOS_MW_EXPOSURE_S));
                    ulRet = IMOS_MW_GetConfig(szUserID, IMOS_MW_EXPOSURE_CFG_VIDEO, 0, &ulBufferLen, &stExposure);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("GET 13 IMOS_MW_EXPOSURE_CFG_VIDEO Error[%lu]\n", ulRet);
                    }
                    else
                    {
                        printf("GET 13 IMOS_MW_EXPOSURE_CFG_VIDEO  OK\n");
                    }

                    /*Set Video*/
                    ulRet = IMOS_MW_SetConfig(szUserID, IMOS_MW_EXPOSURE_CFG_VIDEO, 0, ulBufferLen, &stExposure);
                    if (ERR_COMMON_SUCCEED != ulRet)
                    {
                        printf("SET 13 IMOS_MW_EXPOSURE_CFG_VIDEO Error[%lu]\n", ulRet);
                    }
                    else
                    {
                        printf("SET 13 IMOS_MW_EXPOSURE_CFG_VIDEO  OK\n");
                    }

                    break;
                }

                case 14:
                 {
                     IMOS_MW_EXPOSURE_S stExposure;
                     ULONG ulBufferLen = sizeof(IMOS_MW_EXPOSURE_S);
                     memset(&stExposure, 0, sizeof(IMOS_MW_EXPOSURE_S));
                     ulRet = IMOS_MW_GetConfig(szUserID, IMOS_MW_EXPOSURE_CFG_IMAGE, 0, &ulBufferLen, &stExposure);
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("GET 14 IMOS_MW_EXPOSURE_CFG_IMAGE Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("GET 14 IMOS_MW_EXPOSURE_CFG_IMAGE_IMAGE  OK\n");
                     }

                     /*Set Image*/
                     ulRet = IMOS_MW_SetConfig(szUserID, IMOS_MW_EXPOSURE_CFG_IMAGE, 0, ulBufferLen, &stExposure);
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("SET 14 IMOS_MW_EXPOSURE_CFG_IMAGE Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("SET 14 IMOS_MW_EXPOSURE_CFG_IMAGE  OK\n");
                     }

                     break;
                 }
                case 15:
                 {
     #if 0
                     IMOS_MW_RELEASE_STRATEGY_CFG_S stReleaseCfg;
                     ULONG ulBufferLen = sizeof(IMOS_MW_RELEASE_STRATEGY_CFG_S);
                     memset(&stReleaseCfg, 0, sizeof(IMOS_MW_RELEASE_STRATEGY_CFG_S));
                     ulRet = IMOS_MW_GetConfig(szUserID, IMOS_MW_RELEASE_STRATEGY_CFG, 0, &ulBufferLen, &stReleaseCfg);
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("GET 15 IMOS_MW_RELEASE_STRATEGY_CFG Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("GET 15 IMOS_MW_RELEASE_STRATEGY_CFG  OK\n");
                     }

                     stReleaseCfg.bUnidentifiedRelease = 1;
                     stReleaseCfg.ulCtrlMode = 1;
                     stReleaseCfg.ulReleaseTactics = 1;
                     ulRet = IMOS_MW_SetConfig(szUserID, IMOS_MW_RELEASE_STRATEGY_CFG, 0, ulBufferLen, &stReleaseCfg);
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("SET 15 IMOS_MW_RELEASE_STRATEGY_CFG Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("SET 15 IMOS_MW_RELEASE_STRATEGY_CFG  OK\n");
                     }
     #endif

     #if 0
                     IMOS_MW_GATE_WHITE_LIST_CFG_S stGateWhiteCfg;
                     ULONG ulBufferLen = sizeof(IMOS_MW_GATE_WHITE_LIST_CFG_S);
                     memset(&stGateWhiteCfg, 0, sizeof(IMOS_MW_GATE_WHITE_LIST_CFG_S));
                     ulRet = IMOS_MW_GetConfig(szUserID, IMOS_MW_WHITE_LIST_CFG, 0, &ulBufferLen, &stGateWhiteCfg);
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("GET 15 IMOS_MW_WHITE_LIST_CFG Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("GET 15 IMOS_MW_WHITE_LIST_CFG  OK\n");
                     }

                     stGateWhiteCfg.bEnabled = BOOL_TRUE;
                     stGateWhiteCfg.stMatchRule.ulMatchMode = 0;
                     stGateWhiteCfg.stMatchRule.bChineseIgnored = BOOL_TRUE;
                     stGateWhiteCfg.stMatchRule.ulLetterFuzzyMatchLevel = 1;

                     ulRet = IMOS_MW_SetConfig(szUserID, IMOS_MW_WHITE_LIST_CFG, 0, ulBufferLen, &stGateWhiteCfg);
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("SET 15 IMOS_MW_WHITE_LIST_CFG Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("SET 15 IMOS_MW_WHITE_LIST_CFG  OK\n");
                     }
     #endif
                     IMOS_MW_GATE_BLACK_LIST_CFG_S stGateBlackCfg;
                     ULONG ulBufferLen = sizeof(IMOS_MW_GATE_BLACK_LIST_CFG_S);
                     memset(&stGateBlackCfg, 0, sizeof(IMOS_MW_GATE_BLACK_LIST_CFG_S));
                     ulRet = IMOS_MW_GetConfig(szUserID, IMOS_MW_BLACK_LIST_CFG, 0, &ulBufferLen, &stGateBlackCfg);
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("GET 15 IMOS_MW_BLACK_LIST_CFG Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("GET 15 IMOS_MW_BLACK_LIST_CFG  OK\n");
                     }

                     stGateBlackCfg.bEnabled = BOOL_TRUE;
                     stGateBlackCfg.bLinkSwitch = BOOL_TRUE;
                     stGateBlackCfg.ulLinkSwitchID = 1;
                     stGateBlackCfg.stMatchRule.ulMatchMode = 0;
                     stGateBlackCfg.stMatchRule.ulLetterFuzzyMatchLevel = 1;
                     stGateBlackCfg.stMatchRule.bChineseIgnored = BOOL_TRUE;
                     ulRet = IMOS_MW_SetConfig(szUserID, IMOS_MW_BLACK_LIST_CFG, 0, ulBufferLen, &stGateBlackCfg);
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("SET 15 IMOS_MW_BLACK_LIST_CFG Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("SET 15 IMOS_MW_BLACK_LIST_CFG  OK\n");
                     }

                     break;
                 }
/*
                 case 16:
                 {
                     ulRet = IMOS_MW_ExportWhiteBlackListFile(szUserID, "./Export/GateBlacklist.csv");
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("IMOS_MW_ImportWhiteBlackListFile Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("OK\n");
                     }

                     ulRet = IMOS_MW_ExportWhiteBlackListFile(szUserID, "./Export/GateWhitelist.csv");
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("IMOS_MW_ImportWhiteBlackListFile Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("IMOS_MW_ImportWhiteBlackListFile OK\n");
                     }

                     break;
                 }

                 case 17 :
                 {
                     ulRet = IMOS_MW_ImportWhiteBlackListFile(szUserID, "./Import/GateBlacklist.csv");
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("IMOS_MW_ImportWhiteBlackListFile Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("IMOS_MW_ImportWhiteBlackListFile OK\n");
                     }

                     ulRet = IMOS_MW_ImportWhiteBlackListFile(szUserID, "./Import/GateWhitelist.csv");
                     if (ERR_COMMON_SUCCEED != ulRet)
                     {
                         printf("IMOS_MW_ImportWhiteBlackListFile Error[%lu]\n", ulRet);
                     }
                     else
                     {
                         printf("IMOS_MW_ImportWhiteBlackListFile OK\n");
                     }

                     break;
                 }
*/
                default:
                {
                    break;
                }
            }
        }
    }

    (VOID)IMOS_MW_Logout(szUserID);
    //(VOID)IMOS_MW_Cleanup();


    return 0 ;
}





int IpcamInit(void)
{
    int ulRet = IMOS_MW_Initiate();
    if(ERR_COMMON_SUCCEED != ulRet)
    {
        printf("Initiate ret[%lu]\n", ulRet);
        return 0;
    }

    printf("IPCamClient1\r\n");
    string StrIPcam = "128.8.82.100"; 
    pthread_t m_IpcamThread ;
    pthread_create(&m_IpcamThread,NULL,IpcamThread,NULL);

    return 0 ;
}


