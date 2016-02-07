//*****************************************************************************
//
//                  版权所有 (C), 1998-2010, 华为技术有限公司
//
//*****************************************************************************
//  文 件 名   : diag_sd.c
//  版 本 号   : V1.0
//  生成日期   : 2012-08-13
//  功能描述   : TODO: ...
//
//  函数列表   : TODO: ...
//  修改历史   :
//  1.日    期 : 2012-08-13
//    修改内容 : 创建文件
//
//*****************************************************************************

//*****************************************************************************
// PROJECT   :
// SUBSYSTEM :
// MODULE    :
// OWNER     :
//*****************************************************************************
/*lint -save -e7 -e537 -e322*/
#include "diag_port.h"
#include "diag_fs_proc.h"
#include "mdrv.h"
#include "msp_drx.h"
#include <string.h>
/*lint -restore*/
/*lint --e{718, 746} */

#define    THIS_FILE_ID        MSP_FILE_ID_DIAG_SD_C

/*SD   */
#define DIAG_SD_LTE_LOG_PATH      "/lte_log"
#define DIAG_SD_LTE_LOG_FILE_PRE  "ltelogfile_"
#define DIAG_SD_LTE_LOG_FILE_SUF  ".lpd"
#define DIAG_SD_LOG_DIR_LEN       2048


DIAG_SEND_LOG_CTRL_INFO_STRU stDiagSdLogCtrlInfo = {(ENUM_DMS_LOG_SEND_TYPE)0,0,0 ,0,0,0,0,0,-1,0};

VOS_UINT32 g_SdFileFdErr = 0;
VOS_UINT32 g_SdFileSeekErr = 0;
VOS_UINT32 g_SdFileWriteErr = 0;
VOS_UINT32 g_SdFileWriteOK= 0;

VOS_INT8* g_Diag_Log_Path;
VOS_UINT32 g_Diag_Log_MaxLen;
static VOS_INT8 g_Diag_Log_SdPath[]    = "/mnt/sd";
static VOS_INT8 g_Diag_Log_FlashPath[] = "/online";
static VOS_UINT32 g_Diag_Log_SdMaxLen    = 0xc0000000;   /*3g*/
static VOS_UINT32 g_Diag_Log_FlashMaxLen = 0x3c00000;    /*60m*/

#define DIAG_SD_ROOT_PATH         (g_Diag_Log_Path)
#define DIAG_SD_FILE_MAX_SIZE     (g_Diag_Log_MaxLen)  /*可以存储的最大长度*/

VOS_BOOL diag_IsCurrentFileFull(VOS_UINT32 ulBufLen);

/*****************************************************************************
 函 数 名  : diag_CloseSdFile
 功能描述  :
 输入参数  : fd:
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/
    /*lint -save -e40 -e63*/

VOS_UINT32 diag_CloseSdFile( VOS_INT32 fd)
{
    VOS_UINT32 ret;
    mm_segment_t sd_fs = 0;

    sd_fs = get_fs();
    set_fs(KERNEL_DS);

    if(fd < 0)
    {
        ret = ERR_MSP_SUCCESS;
    }
    else
    {
        ret = (VOS_UINT32)DIAG_FS_CLOSE(fd);
        DIAG_FS_SYNC();
    }
    set_fs(sd_fs);

    return ret;
}

/*****************************************************************************
 函 数 名  : diag_CleanDir
 功能描述  :
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 diag_CleanDir(VOS_UINT32 ulFileNum)
{
    VOS_CHAR dirName[128] = {0};
    VOS_INT32 ret = 0;
    VOS_CHAR filedirName[128] = {0};
    VOS_CHAR CurrentFileName[128] = {0};
    VOS_UINT32 i;

    mm_segment_t sd_fs = 0;

    sd_fs = get_fs();
    set_fs(KERNEL_DS);

    if(stDiagSdLogCtrlInfo.CurrentFileFd != -1)
    {
        diag_CloseSdFile(stDiagSdLogCtrlInfo.CurrentFileFd);
        stDiagSdLogCtrlInfo.CurrentFileFd = -1;
    }
    if(strlen(DIAG_SD_ROOT_PATH) > 128)
    {
        return ERR_MSP_FAILURE;
    }
    strncat(dirName, DIAG_SD_ROOT_PATH, strlen(DIAG_SD_ROOT_PATH));/* [false alarm]:屏蔽Fortify */
    strncat(dirName, DIAG_SD_LTE_LOG_PATH, strlen(DIAG_SD_LTE_LOG_PATH));/* [false alarm]:屏蔽Fortify */

    strncat(filedirName, dirName, strlen(dirName));/* [false alarm]:屏蔽Fortify */
    strncat(filedirName, "/", strlen("/"));/* [false alarm]:屏蔽Fortify */
    strncat(filedirName, DIAG_SD_LTE_LOG_FILE_PRE, strlen(DIAG_SD_LTE_LOG_FILE_PRE));/* [false alarm]:屏蔽Fortify */

    for( i=0;i<ulFileNum;i++)
    {
        /*lint -save -e18*/
        /* coverity[secure_coding] */
        sprintf(CurrentFileName, "%s%d.lpd",filedirName,(VOS_UINT8)i);/* [false alarm]:屏蔽Fortify */
        /*lint -restore +e18*/

        ret = DIAG_FS_RMFILE(CurrentFileName);

        VOS_MemCpy(CurrentFileName, 0, 128);
    }

    DIAG_FS_SYNC();

    set_fs(sd_fs);

    return  (VOS_UINT32)ret; /* [false alarm]:屏蔽Fortify */
}

/*****************************************************************************
 函 数 名  : diag_OpenSdFile
 功能描述  :
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 diag_OpenSdFile( VOS_VOID)
{
    VOS_UINT32 ret;
    VOS_CHAR filedirName[128] = {0};
    VOS_CHAR CurrenfileName[128] = {0};
    VOS_UINT32  ulFileNameLen;
    mm_segment_t sd_fs = 0;


    sd_fs = get_fs();
    set_fs(KERNEL_DS);

    stDiagSdLogCtrlInfo.ulCurrenFileSize = 0;
    if(strlen(DIAG_SD_ROOT_PATH) > 128)
    {
        ret  = ERR_MSP_FAILURE;/* [false alarm]:屏蔽Fortify */
    }
    strncat(filedirName, DIAG_SD_ROOT_PATH, strlen(DIAG_SD_ROOT_PATH));/* [false alarm]:屏蔽Fortify */

    strncat(filedirName, DIAG_SD_LTE_LOG_PATH, strlen(DIAG_SD_LTE_LOG_PATH));/* [false alarm]:屏蔽Fortify */
    ulFileNameLen = VOS_StrLen(filedirName);

    filedirName[ulFileNameLen] = '/';

    strncat(filedirName, DIAG_SD_LTE_LOG_FILE_PRE, strlen(DIAG_SD_LTE_LOG_FILE_PRE));/* [false alarm]:屏蔽Fortify */

    /*拼接文件编号*/
    /* coverity[secure_coding] */
    sprintf(CurrenfileName, "%s%d.lpd",filedirName,(VOS_UINT8)stDiagSdLogCtrlInfo.ulFileNum);/* [false alarm]:屏蔽Fortify */

    stDiagSdLogCtrlInfo.CurrentFileFd = DIAG_FS_OPEN(CurrenfileName, O_RDWR |O_CREAT, 0666);

    if(stDiagSdLogCtrlInfo.CurrentFileFd < 0)
    {
        ret  = ERR_MSP_FAILURE;
    }
    else
    {
        stDiagSdLogCtrlInfo.ulFileNum =stDiagSdLogCtrlInfo.ulFileNum  + 1;
        stDiagSdLogCtrlInfo.ulCurrenFileSize = 0;
        ret  =  ERR_MSP_SUCCESS;
    }

    DIAG_FS_FILE_SYNC( stDiagSdLogCtrlInfo.CurrentFileFd);
    set_fs(sd_fs);

    return  ret;

}

/*****************************************************************************
 函 数 名  : diag_ReCreatLogDir
 功能描述  :
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 diag_ReCreatLogDir(VOS_VOID)
{
    VOS_INT32 Fd;
    VOS_UINT32 nRead;
    VOS_CHAR dirName[128] = {0};
    VOS_UINT8 *pBuf =NULL;
    VOS_INT32 RsRet;
    mm_segment_t sd_fs;

#if (VOS_LINUX== VOS_OS_VER)
    pBuf = (VOS_UINT8 *)kmalloc(DIAG_SD_LOG_DIR_LEN, GFP_KERNEL|__GFP_DMA);
#endif
    if(pBuf == NULL)
    {
        return ERR_MSP_MALLOC_FAILUE;
    }

    sd_fs = get_fs();
    set_fs(KERNEL_DS);

    diag_CloseSdFile(stDiagSdLogCtrlInfo.CurrentFileFd);

    stDiagSdLogCtrlInfo.CurrentFileFd = -1;
    if(strlen(DIAG_SD_ROOT_PATH) > 128)
    {
#if (VOS_LINUX== VOS_OS_VER)
        kfree(pBuf);
#endif
        return ERR_MSP_FAILURE;
    }
    strncat(dirName, DIAG_SD_ROOT_PATH, strlen(DIAG_SD_ROOT_PATH));/* [false alarm]:屏蔽Fortify */
    strncat(dirName, DIAG_SD_LTE_LOG_PATH, strlen(DIAG_SD_LTE_LOG_PATH));/* [false alarm]:屏蔽Fortify */

    Fd = DIAG_FS_OPEN(dirName, O_DIRECTORY, 0);

    if(Fd <0)
    {
        /* 目录不存在，返回删除成功*/
#if (VOS_LINUX== VOS_OS_VER)
        kfree(pBuf);
#endif
    }
    else
    {
        nRead = (VOS_UINT32)DIAG_FS_GETDENTS((VOS_UINT)Fd, (VOS_VOID*)pBuf, DIAG_SD_LOG_DIR_LEN);
#if (VOS_LINUX== VOS_OS_VER)
        kfree(pBuf);
#endif

        if((VOS_UINT32)(-1) == nRead)
        {
            DIAG_FS_CLOSE(Fd);
            set_fs(sd_fs);
            return ERR_MSP_FAILURE;
        }

        /*目录是空的，不需要删除*/
        if(0 == nRead)
        {
            DIAG_FS_CLOSE(Fd);
            set_fs(sd_fs);
            return ERR_MSP_SUCCESS;
        }
        else
        {
            DIAG_FS_CLOSE(Fd);
            diag_CleanDir(nRead);
            return ERR_MSP_SUCCESS;
        }
    }

    RsRet =  DIAG_FS_MKDIR(dirName,0700);

    DIAG_FS_SYNC();

    set_fs(sd_fs);
    if(RsRet <0)
    {
        return MSP_SD_CREAT_DIR_ERROR;
    }
    else
    {
        return ERR_MSP_SUCCESS;
    }

}

/*****************************************************************************
 函 数 名  : diag_WriteDataToSDFile
 功能描述  :
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 diag_WriteDataToSDFile(VOS_UINT8* ucBuf , VOS_UINT32 ulBufLen)
{
    VOS_UINT32 ret;
    VOS_INT32 ulWriteLen = 0;
    mm_segment_t sd_fs = 0;
    sd_fs = get_fs();
    set_fs(KERNEL_DS);


    if(stDiagSdLogCtrlInfo.CurrentFileFd == -1)
    {
        g_SdFileFdErr++;
        ret  = ERR_MSP_NOT_FOUND;
    }
    else
    {
        if( -1 == DIAG_FS_LSEEK(stDiagSdLogCtrlInfo.CurrentFileFd, (int)stDiagSdLogCtrlInfo.ulCurrenFileSize, SEEK_SET) )
        {
            g_SdFileSeekErr++;
            ret  = ERR_MSP_FAILURE;
        }
        else
        {
            ulWriteLen = DIAG_FS_WRITE(stDiagSdLogCtrlInfo.CurrentFileFd, (VOS_INT8 *)ucBuf, ulBufLen);

            if(ulWriteLen == ulBufLen)
            {
                ret  = ERR_MSP_SUCCESS;
                stDiagSdLogCtrlInfo.StorageTotalSize += ulWriteLen;
                stDiagSdLogCtrlInfo.ulCurrenFileSize += ulWriteLen;
                g_SdFileWriteOK++;
            }
            else
            {
                g_SdFileWriteErr++;
                ret  = ERR_MSP_UNKNOWN;
            }
        }
    }


    DIAG_FS_FILE_SYNC(stDiagSdLogCtrlInfo.CurrentFileFd);

    set_fs(sd_fs);

    return ret;
}

/*lint -restore +e30 +e63*/

/*****************************************************************************
 函 数 名  : diag_SetLogCtrlInfo
 功能描述  :
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 diag_SetLogCtrlInfo(DIAG_CMD_SET_LPD_MODE_REQ_STRU* pstLdpReq)
{
    VOS_UINT32 ret;
    VOS_UINT32 ulFileSize;

    if(pstLdpReq->ulLpdMode >= EN_DIAG_SEND_LOG_BUTT )
    {
        return ERR_MSP_PHYNUM_INVALID;
    }

    BSP_BBPGetCurTime(&(stDiagSdLogCtrlInfo.StartTime));
    stDiagSdLogCtrlInfo.MaxFileSize = DIAG_SD_FILE_MAX_SIZE;
    stDiagSdLogCtrlInfo.ulCurrenFileSize = 0;
    stDiagSdLogCtrlInfo.ulFileNum = 0;
    stDiagSdLogCtrlInfo.StorageTotalSize = 0;

    if((pstLdpReq->ulLpdMode  == EN_DIAG_SEND_LOG_TO_SD)&&
       (pstLdpReq->ulStartEnd == PORT_SWITCH_ON))
    {
        diag_printf("Seting Log Save!\n");
        if((!DRV_SDMMC_GET_STATUS())&&(BSP_MODULE_SUPPORT == DRV_GET_SD_SUPPORT()))
        {
            diag_printf(" DRV_SDMMC_GET_STATUS FALSE\n");

            return MSP_SD_GET_STATE_ERROR;
        }

        /*删除SD卡中已有的log文件*/
        ret  = diag_ReCreatLogDir();

        if(ret == ERR_MSP_SUCCESS)
        {
            ret = diag_OpenSdFile();

            if(ret == ERR_MSP_SUCCESS)
            {
                ulFileSize = (VOS_UINT32)(pstLdpReq->ulSDFileSize)*1024*1024;
                if((ulFileSize == 0)||(ulFileSize > DIAG_SD_FILE_MAX_SIZE))
                {
                    diag_printf("parameter invalid!  max space :0x%x Byte !!!\n",(VOS_UINT32)DIAG_SD_FILE_MAX_SIZE);
                    return ERR_MSP_INVALID_PARAMETER;
                }
                stDiagSdLogCtrlInfo.enSendLogType =(ENUM_DMS_LOG_SEND_TYPE)EN_DIAG_SEND_LOG_TO_SD;
                stDiagSdLogCtrlInfo.ulPeriod = (pstLdpReq->ulSDFileSaveTime) *60; /*持续时间，HSO下发的单位为分钟，单板转换成秒计数 */
                stDiagSdLogCtrlInfo.ulStorageLogSize = (ulFileSize > DIAG_SD_FILE_MAX_SIZE)?\
                    DIAG_SD_FILE_MAX_SIZE:ulFileSize;/*HSO下发的是MB为单位，单板转换成字节计数*/
                stDiagSdLogCtrlInfo.StorageTotalSize = 0;
                stDiagSdLogCtrlInfo.ulCurrenFileSize = 0;
                stDiagSdLogCtrlInfo.ulStartFlag = 1;
                stDiagSdLogCtrlInfo.ulMinFileNum = 0;   /*设置时最小文件编号为0*/

                /*设置socp 为唤醒源*/
                DRV_PM_SET_WAKE_SRC(DRV_WAKE_SRC_SOCP);
            }
        }
    }
    else
    {
         stDiagSdLogCtrlInfo.enSendLogType = (ENUM_DMS_LOG_SEND_TYPE)EN_DIAG_SEND_LOG_TO_USB;

        /*关闭SD卡中已经打开的log文件*/
        diag_CloseSdFile(stDiagSdLogCtrlInfo.CurrentFileFd);
        stDiagSdLogCtrlInfo.CurrentFileFd = -1;
        stDiagSdLogCtrlInfo.ulStartFlag = 0;

        return ERR_MSP_SUCCESS;
    }

    return ret;
}

/*****************************************************************************
 函 数 名  : diag_GetLogSendType
 功能描述  :
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

ENUM_DMS_LOG_SEND_TYPE diag_GetLogSendType(VOS_VOID)
{
    if(stDiagSdLogCtrlInfo.enSendLogType  == EN_DIAG_SEND_LOG_TO_SD)
    {
        return EN_DIAG_SEND_LOG_TO_SD;
    }
    else
    {
        return EN_DIAG_SEND_LOG_TO_USB;
    }
}

/*****************************************************************************
 函 数 名  : diag_IsLogSendTimeOver
 功能描述  :
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_BOOL diag_IsLogSendTimeOver(VOS_VOID)
{
    VOS_UINT64 NewTime;
    VOS_UINT64 PeriodTime;
    VOS_UINT64 TempTime;

    if(stDiagSdLogCtrlInfo.ulPeriod == 0)
    {
        return FALSE;
    }

    (VOS_VOID)BSP_BBPGetCurTime(&NewTime);
     /*将以秒为单位的时间转换成32K频率的计数*/
     /*lint -save -e647*/
     /* coverity[overflow_before_widen] */
    PeriodTime = ( VOS_UINT64)(stDiagSdLogCtrlInfo.ulPeriod *32*1024 );
     /*lint -restore*/
    /*不需要 考虑翻转*/
    TempTime = NewTime - stDiagSdLogCtrlInfo.StartTime ;

    if(TempTime > PeriodTime)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
 函 数 名  : diag_IsCurrentFileFull
 功能描述  :  判断当前文件正在写入的文件是否达到设置的大小
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_BOOL diag_IsCurrentFileFull(VOS_UINT32 ulBufLen)
{
    if((stDiagSdLogCtrlInfo.ulCurrenFileSize +ulBufLen )  < stDiagSdLogCtrlInfo.ulStorageLogSize)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


/*****************************************************************************
 函 数 名  : diag_IsTotalFileFull
 功能描述  : 判断当前写入的文件是否已达到空间最大值
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_BOOL diag_IsTotalFileFull(VOS_UINT32 ulBufLen)
{
    /*判断已写入的数据总长度是否达到最大空间长度*/
    if((stDiagSdLogCtrlInfo.StorageTotalSize +ulBufLen )  < stDiagSdLogCtrlInfo.MaxFileSize)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
/*****************************************************************************
 函 数 名  : diag_DeleteOldFile
 功能描述  :
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 diag_DeleteOldFile(VOS_VOID)
{
    VOS_CHAR filedirName[128] = {0};
    VOS_CHAR CurrenfileName[128] = {0};
    VOS_UINT32  ulFileNameLen;
    VOS_INT32 fd;
    mm_segment_t sd_fs;
    VOS_INT32 ulFileLen;    /*记录最小序列号文件长度*/
    VOS_UINT32 ulRet;

    /*lint -save -e63*/
    sd_fs = get_fs();
    /*lint -restore +e63*/
    set_fs(KERNEL_DS);

    /*first close current file*/
    DIAG_FS_CLOSE(stDiagSdLogCtrlInfo.CurrentFileFd);
    stDiagSdLogCtrlInfo.CurrentFileFd = -1;

    if(strlen(DIAG_SD_ROOT_PATH) > 128)
    {
        return ERR_MSP_FAILURE;
    }
    strncat(filedirName, DIAG_SD_ROOT_PATH, strlen(DIAG_SD_ROOT_PATH));/* [false alarm]:屏蔽Fortify */

    strncat(filedirName, DIAG_SD_LTE_LOG_PATH, strlen(DIAG_SD_LTE_LOG_PATH));/* [false alarm]:屏蔽Fortify */
    ulFileNameLen = VOS_StrLen(filedirName);

    filedirName[ulFileNameLen] = '/';

    strncat(filedirName, DIAG_SD_LTE_LOG_FILE_PRE, strlen(DIAG_SD_LTE_LOG_FILE_PRE));/* [false alarm]:屏蔽Fortify */

    /*拼接文件编号*/
    /* coverity[secure_coding] */
    sprintf(CurrenfileName, "%s%d.lpd",filedirName,(VOS_UINT8)stDiagSdLogCtrlInfo.ulMinFileNum);/* [false alarm]:屏蔽Fortify */

    diag_printf("[%s]:%s\n",__func__,CurrenfileName);

    fd = DIAG_FS_OPEN(CurrenfileName, O_RDWR, 0666);
    if( fd<0 )
    {
        set_fs(sd_fs);
        diag_printf("[%s]:%d %s\n",__func__,__LINE__,CurrenfileName);
        return ERR_MSP_FAILURE;
    }
    ulFileLen = DIAG_FS_LSEEK(fd, 0, SEEK_END);
    DIAG_FS_LSEEK(fd, 0, SEEK_SET);

    diag_CloseSdFile(fd);
    DIAG_FS_RMFILE(CurrenfileName);
    stDiagSdLogCtrlInfo.StorageTotalSize -= ulFileLen;


    /*重新打开*/
    ulRet = diag_OpenSdFile();
    if(ulRet)
    {
        set_fs(sd_fs);
        stDiagSdLogCtrlInfo.ulStartFlag =0;
        diag_printf("[%s]:%d\n",__func__,__LINE__);
        return ulRet;
    }

    stDiagSdLogCtrlInfo.ulMinFileNum ++;

    set_fs(sd_fs);
    return ERR_MSP_SUCCESS;

}
/*****************************************************************************
 函 数 名  : diag_StorDataToSD
 功能描述  :
 输入参数  : :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年9月3日
     作    者  : heliping
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 diag_StorDataToSD(VOS_UINT8* ucBuf , VOS_UINT32 ulBufLen)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;

    if(stDiagSdLogCtrlInfo.ulStartFlag == 0)
    {
        return ulRet;
    }

    if((ucBuf == NULL) ||(ulBufLen == 0 ))
    {
        diag_CloseSdFile(stDiagSdLogCtrlInfo.CurrentFileFd);
        stDiagSdLogCtrlInfo.CurrentFileFd = -1;
        stDiagSdLogCtrlInfo.ulStartFlag =0;
        return ERR_MSP_INVALID_PARAMETER;
    }

    if((!DRV_SDMMC_GET_STATUS())&&(BSP_MODULE_SUPPORT == DRV_GET_SD_SUPPORT()))
    {
        diag_CloseSdFile(stDiagSdLogCtrlInfo.CurrentFileFd);
        stDiagSdLogCtrlInfo.CurrentFileFd = -1;
        stDiagSdLogCtrlInfo.ulStartFlag =0;
        return MSP_SD_GET_STATE_ERROR;
    }

    //存贮数据的总大小是否超过设定值,  返回成功
    if(diag_IsTotalFileFull(ulBufLen))
    {
        if(diag_DeleteOldFile()) /*清理最先写入的文件*/
        {
            return ERR_MSP_FAILURE;
        }
    }

    //存贮时间超过设定，返回成功
    if(diag_IsLogSendTimeOver())
    {
        diag_CloseSdFile(stDiagSdLogCtrlInfo.CurrentFileFd);
        stDiagSdLogCtrlInfo.CurrentFileFd = -1;
        stDiagSdLogCtrlInfo.ulStartFlag =0;
        return ERR_MSP_SUCCESS;
    }

    /*当前文件是否写满*/
    if( !diag_IsCurrentFileFull(ulBufLen))
    {
        ulRet = diag_WriteDataToSDFile(ucBuf ,ulBufLen);
    }
    else
    {
        /*先关闭已经写满的文件*/
        diag_CloseSdFile(stDiagSdLogCtrlInfo.CurrentFileFd);

        stDiagSdLogCtrlInfo.CurrentFileFd = -1;

        /*重新打开文件*/
        ulRet = diag_OpenSdFile();

        if(ulRet != ERR_MSP_SUCCESS)
        {
            stDiagSdLogCtrlInfo.ulStartFlag =0;
            return ulRet;
        }

        ulRet = diag_WriteDataToSDFile(ucBuf ,ulBufLen);
    }

    if(ulRet != ERR_MSP_SUCCESS)
    {
        if(stDiagSdLogCtrlInfo.CurrentFileFd != -1)
        {
            diag_CloseSdFile(stDiagSdLogCtrlInfo.CurrentFileFd);

            stDiagSdLogCtrlInfo.CurrentFileFd = -1;
        }
        stDiagSdLogCtrlInfo.ulStartFlag =0;
    }

    return ulRet;

}

VOS_UINT32 diag_SdInit(VOS_VOID)
{
    if(BSP_MODULE_SUPPORT == DRV_GET_SD_SUPPORT())
    {
        g_Diag_Log_Path = g_Diag_Log_SdPath;
        g_Diag_Log_MaxLen = g_Diag_Log_SdMaxLen;
    }
    else
    {
        g_Diag_Log_Path = g_Diag_Log_FlashPath;
        g_Diag_Log_MaxLen = g_Diag_Log_FlashMaxLen;
    }
	g_diagPort[EN_DIAG_SD_BEARER_DIAG].ucChanStat        = ACM_EVT_DEV_READY;
	g_diagPort[EN_DIAG_SD_BEARER_DIAG].ucHdlcFlag        = EN_HDLC_DATA;
	g_diagPort[EN_DIAG_SD_BEARER_DIAG].ulCodeDesChanId   = SOCP_CODER_DST_LOM_CNF;
	g_diagPort[EN_DIAG_SD_BEARER_DIAG].ulDecodeSrcChanId = SOCP_DECODER_SRC_LOM;
	g_diagPort[EN_DIAG_SD_BEARER_DIAG].ulRecivBufLen     = 0;
    g_diagPort[EN_DIAG_SD_BEARER_DIAG].slPortHandle      = UDI_INVALID_HANDLE;
    g_diagPort[EN_DIAG_SD_BEARER_DIAG].ulInitStat        = EN_PORT_INIT_SUCC;

    return ERR_MSP_SUCCESS;
}


 VOS_VOID diag_sdhelp(VOS_VOID)
 {
    /*lint -save -e516*/
    diag_printf(" 当前文件的FD = %d\n",stDiagSdLogCtrlInfo.CurrentFileFd);
    diag_printf(" 当前文件的SIZE = 0x%x\n",stDiagSdLogCtrlInfo.ulCurrenFileSize);
    diag_printf(" 当前文件数目 = %d\n",stDiagSdLogCtrlInfo.ulFileNum);
    diag_printf(" 当前最小文件序列号 = %d\n",stDiagSdLogCtrlInfo.ulMinFileNum);
    diag_printf(" 设置时间 = %d\n",stDiagSdLogCtrlInfo.ulPeriod);
    diag_printf(" 开始时间 = %d\n",(VOS_UINT32)(stDiagSdLogCtrlInfo.StartTime));
    diag_printf(" 设置存储SIZE = 0x%x\n",(VOS_UINT32)(stDiagSdLogCtrlInfo.ulStorageLogSize));
    diag_printf(" 已存储SIZE = 0x%x\n",(VOS_UINT32)(stDiagSdLogCtrlInfo.StorageTotalSize));
    diag_printf(" FD错误g_SdFileFdErr = %d\n",g_SdFileFdErr);
    diag_printf(" SEEK错误g_SdFileFdErr = %d\n",g_SdFileSeekErr);
    diag_printf(" WRITE错误g_SdFileFdErr = %d\n",g_SdFileWriteErr);
    diag_printf(" WRITE成功g_SdFileFdOK = %d\n",g_SdFileWriteOK);
    diag_printf(" SD卡存储开始标志 = %d\n",stDiagSdLogCtrlInfo.ulStartFlag);
    diag_printf("存储路径:      %s\n",DIAG_SD_ROOT_PATH);
    diag_printf("存储空间大小 : 0x%x\n",DIAG_SD_FILE_MAX_SIZE);
    /*lint -restore +e516*/
 }




