
#include "om.h"
#include "OmApp.h"
#include "omprivate.h"
#include "pslog.h"
#include "omsdlog.h"
#include "cpm.h"
#include "Omappagent.h"
#include "NVIM_Interface.h"
#include "OamSpecTaskDef.h"
#include "GU_MSPComm.h"
#include "OmAppRl.h"
#include "TafNvInterface.h"
#include "SCMProc.h"
#include "OmHsicPpm.h"
#include "SCMSoftDecode.h"
#include "CbtPpm.h"

#if (RAT_MODE != RAT_GU)
#include "msp_nvim.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767*/
#define    THIS_FILE_ID PS_FILE_ID_PAM_APP_OM_C
/*lint +e767*/

/* 记录ICC核间消息文件路径 */
VOS_CHAR                                g_acICCLogFilePath[OM_ICC_MAX_NAME_LEN];

#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
#define  OM_ICC_LOG_PATH                "/data/modemlog/Log/Icc-log"
#define  OM_ICC_UNITARY_LOG_PATH        "/modem_log/Log/Icc-log"
#else
#define  OM_ICC_LOG_PATH                "/yaffs0/Log/Icc-log"
#define  OM_ICC_UNITARY_LOG_PATH        "/modem_log/Log/Icc-log"
#endif

/* 记录ICC核间消息文件序列值，默认0 */
VOS_UINT32                              g_ulICCLogFileIndex;

/* 记录ICC核间消息BUFF */
OM_ICC_INFO_STRU                        g_astIccRecordInfo[OAM_ICC_RECORD_MAX_NUM];

/* 记录ICC核间消息索引值 */
VOS_UINT32                              g_ulIccRecordIndex = 0;

/* 记录ICC核间消息定时器 */
HTIMER                                  g_AcpuIccLogTimerId;

extern VOS_UINT V_ICC_OSAMsg_CB(VOS_UINT ulChannelID,VOS_INT lLen);

extern VOS_UINT32 VOS_GetMsgName(VOS_UINT32 ulAddrress);

extern VOS_VOID Om_AcpuQueryDumpMsgProc(MsgBlock* pMsg);
VOS_VOID OM_AcpuICCInfoFileName( VOS_CHAR * cFileName )
{
    FILE                                *fp;

    if ( VOS_NULL_PTR == cFileName )
    {
        return;
    }

    /* 读取最后保存文件序列值 */
    fp = mdrv_file_open((VOS_CHAR*)g_acICCLogFilePath, "rb+");

    /* 如果没有读取到文件序列值文件，代表首次保存，文件序列值使用默认值0开始 */
    if (VOS_NULL_PTR == fp)
    {
        g_ulICCLogFileIndex = 0;
    }
    else
    {
        mdrv_file_read(&g_ulICCLogFileIndex, sizeof(VOS_UINT32), 1, fp);

        mdrv_file_close(fp);
    }

    VOS_MemSet(cFileName, 0, OM_ICC_MAX_NAME_LEN);

    VOS_StrNCpy(cFileName, g_acICCLogFilePath, VOS_StrLen(g_acICCLogFilePath));

    (VOS_VOID)VOS_nsprintf((VOS_CHAR *)(cFileName+VOS_StrLen(g_acICCLogFilePath)), 2, ".%d", g_ulICCLogFileIndex);

    return;
}


VOS_VOID OM_AcpuICCInfoWriteLog(VOS_UINT32 ulParam, VOS_UINT32 ulTimerName)
{
    VOS_UINT32                          ulLen;
    VOS_CHAR                            ucFileName[OM_ICC_MAX_NAME_LEN];
    FILE                                *fp;

    OM_AcpuICCInfoFileName(ucFileName);

    fp = mdrv_file_open((VOS_CHAR*)ucFileName, "ab+");  /*按照追加方式打开文件*/

    if(VOS_NULL_PTR == fp)                      /*如果文件打开失败说明建立文件也不成功*/
    {
        return;
    }

    if(VOS_OK != mdrv_file_seek(fp, 0, DRV_SEEK_END))
    {
        mdrv_file_close(fp);
        return;
    }

    ulLen = (VOS_UINT32)mdrv_file_tell(fp);

    if(ulLen >= OM_LOG_FILE_MAX_SIZE)            /*文件大小超过限制*/
    {
        mdrv_file_close(fp);

        g_ulICCLogFileIndex++;

        if (g_ulICCLogFileIndex > OAM_ICC_RECORD_MAX_FILE)
        {
            g_ulICCLogFileIndex = 0;
        }

        /* 将当前文件序列值记录至Icc-log文件中 */
        fp = mdrv_file_open((VOS_CHAR*)g_acICCLogFilePath, "wb+");

        if(VOS_NULL_PTR == fp)
        {
            return;
        }

        mdrv_file_write(&g_ulICCLogFileIndex, sizeof(VOS_UINT32), 1, fp);

        mdrv_file_close(fp);

        /* 获取新记录文件名 */
        OM_AcpuICCInfoFileName(ucFileName);

        fp = mdrv_file_open((VOS_CHAR*)ucFileName, "wb+");  /*清空文件内容*/
    }

    if(VOS_NULL_PTR == fp)                      /*如果文件打开失败说明建立文件也不成功*/
    {
        return;
    }

    /* 通过记录的消息索引值记录当前保存的ICC消息 */
    mdrv_file_write(g_astIccRecordInfo, sizeof(OM_ICC_INFO_STRU), OAM_ICC_RECORD_MAX_NUM, fp);

    mdrv_file_close(fp);

    /* 每次记录完成后，清空当前ICC可维可测BUFF */
    VOS_MemSet(g_astIccRecordInfo, 0, sizeof(OM_ICC_INFO_STRU)*OAM_ICC_RECORD_MAX_NUM);

    g_ulIccRecordIndex = 0;

    return;
}
VOS_UINT32  OM_AcpuICCInfoInit( VOS_VOID )
{
    NV_SOCP_SDLOG_CFG_STRU              stInfoCfg ;

    VOS_MemSet(&stInfoCfg, 0, sizeof(stInfoCfg));

    g_AcpuIccLogTimerId = VOS_NULL_PTR;

    if(NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_ID_SOCP_SDLOG_CFG, &stInfoCfg, sizeof(NV_SOCP_SDLOG_CFG_STRU)))
    {
        return VOS_ERR;
    }

    if(VOS_TRUE != stInfoCfg.ulSocpDelayWriteFlg)
    {
        return VOS_OK;
    }

    /* 使用延迟写入NV时长，启动定时保存定时器 */
    /*(VOS_VOID)VOS_StartRelTimer(&g_AcpuIccLogTimerId, ACPU_PID_PAM_OM, stInfoCfg.ulTimeOutValue,
                    OM_ICC_TIMER, 0, VOS_RELTIMER_LOOP, VOS_TIMER_PRECISION_5);*/

    return VOS_OK;
}


VOS_VOID OM_AcpuICCInfoRecord(MsgBlock *pMsgCtrlBlk)
{
    if (VOS_NULL_PTR == pMsgCtrlBlk)
    {
        return;
    }

    g_astIccRecordInfo[g_ulIccRecordIndex].usSendPid    = (VOS_UINT16)pMsgCtrlBlk->ulSenderPid;
    g_astIccRecordInfo[g_ulIccRecordIndex].usRecvPid    = (VOS_UINT16)pMsgCtrlBlk->ulReceiverPid;
    g_astIccRecordInfo[g_ulIccRecordIndex].ulMsgName    = VOS_GetMsgName((VOS_UINT_PTR)pMsgCtrlBlk);
    g_astIccRecordInfo[g_ulIccRecordIndex].ulSlice      = OM_GetSlice();

    g_ulIccRecordIndex++;

    /* 当记录的ICC消息超过BUFF数组最大值，从BUFF头开始重新记录 */
    if (g_ulIccRecordIndex >= OAM_ICC_RECORD_MAX_NUM)
    {
        g_ulIccRecordIndex = 0;
    }

    return;
}
VOS_VOID PAMOM_AcpuTimerMsgProc(MsgBlock* pMsg)
{
    VOS_UINT32 ulName;

    ulName = ((REL_TIMER_MSG*)pMsg)->ulName;

    if (OM_ICC_TIMER == ulName)
    {
        OM_AcpuICCInfoWriteLog(0, ulName);
    }
    else
    {
        /* blank */
    }
}


/*****************************************************************************
 Prototype       : CBT_AppMsgProc
 Description     : Handle all messages sent to OM.
 Input           : pMsg -- The pointer of the msg.
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2008-03-20
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_VOID PAMOM_AppMsgProc(MsgBlock* pMsg)
{
    if (VOS_PID_TIMER == pMsg->ulSenderPid)
    {
        PAMOM_AcpuTimerMsgProc(pMsg);
    }
    else if (CCPU_PID_PAM_OM == pMsg->ulSenderPid)
    {
        Om_AcpuQueryDumpMsgProc(pMsg);
    }
    else
    {
        /* blank */
    }

    return;
}

/*****************************************************************************
 Prototype       : OM_AcpuInit
 Description     : OM' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 PAMOM_AcpuInit(VOS_VOID)
{
    if (VOS_OK != OM_AcpuICCInfoInit())
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_AcpuInit: OM_AcpuICCInfoInit failed.\n");
    }

    if (VOS_OK != OM_AcpuGetLogPath(g_acICCLogFilePath, OM_ICC_LOG_PATH, OM_ICC_UNITARY_LOG_PATH))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 Prototype       : OM_AcpuPidInit
 Description     : ACPU OM PID' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 PAMOM_AppPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
            return PAMOM_AcpuInit();

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 Prototype       : OM_AcpuFidInit
 Description     : ACPU OM FID' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 PAMOM_APP_FID_Init(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32                          ulRslt;

    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
        {
            ulRslt = VOS_RegisterPIDInfo(ACPU_PID_PAM_OM,
                                        (Init_Fun_Type)VOS_NULL_PTR,
                                        (Msg_Fun_Type)PAMOM_AppMsgProc);
            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            ulRslt = VOS_RegisterMsgTaskPrio(ACPU_FID_PAM_OM, VOS_PRIORITY_M2);
            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            break;
        }

        default:
            break;
    }
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : VOS_ICCError_CB
 功能描述  : 用于ICC WIFI通道的数据读取回调函数
 输入参数  : ulChannelID:   当前通道ID
             ulLen:         当前通道数据长度
 输出参数  : 无
 返 回 值  : VOS_ERROR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2011年3月10日
     作    者  : l46160
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT VOS_ICCError_CB(VOS_UINT ulChanID, VOS_UINT ulEvent, VOS_VOID* pParam)
{
    VOS_UINT32 ulChannelID;

    if(UDI_ICC_GUOM4 == (UDI_HANDLE)ulChanID)
    {
        ulChannelID = OM_OSA_MSG_ICC_CHANNEL;
    }
    else
    {
        LogPrint("VOS_ICCError_CB: The Channel ID is not used by OAM\n");

        return VOS_ERR;
    }

    LogPrint2("VOS_ICCError_CB: The Error Channel ID is %d,and Event is %d\n",
                (VOS_INT32)ulChannelID, (VOS_INT32)ulEvent);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : OM_ComRx_ICC_Init
 功能描述  : 用于OM在CCPU初始化ICC通道
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2011年3月10日
     作    者  : l46160
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 VOS_ICC_Init(VOS_VOID)
{
    OM_ICC_UDI_CTRL_STRU                astACPUICCCtrlTable;

    /*初始化 ICC通道*/
    VOS_MemSet((&astACPUICCCtrlTable), 0, sizeof(astACPUICCCtrlTable));

    astACPUICCCtrlTable.pstICCAttr = (ICC_CHAN_ATTR_S*)VOS_MemAlloc(ACPU_PID_PAM_OM,
                                                                    STATIC_MEM_PT,
                                                                    sizeof(ICC_CHAN_ATTR_S));
    if(VOS_NULL_PTR == astACPUICCCtrlTable.pstICCAttr)
    {
        return VOS_ERR;             /*分配内存失败单板会重启，因此不需要释放之前已经申请的内存*/
    }

    astACPUICCCtrlTable.enICCId               = UDI_ICC_GUOM4;
    astACPUICCCtrlTable.pstICCAttr->read_cb   = V_ICC_OSAMsg_CB;
    astACPUICCCtrlTable.pstICCAttr->u32Priority    = OM_ICC_CHANNEL_PRIORITY;
    astACPUICCCtrlTable.pstICCAttr->u32TimeOut     = OM_ICC_HANDSHAKE_TIME_MAX;
    astACPUICCCtrlTable.pstICCAttr->u32FIFOInSize  = OSA_ICC_BUFFER_SIZE;
    astACPUICCCtrlTable.pstICCAttr->u32FIFOOutSize = OSA_ICC_BUFFER_SIZE;
    astACPUICCCtrlTable.pstICCAttr->enChanMode     = ICC_CHAN_MODE_PACKET;
    astACPUICCCtrlTable.pstICCAttr->event_cb       = VOS_ICCError_CB;
    astACPUICCCtrlTable.pstICCAttr->write_cb       = VOS_NULL_PTR;

    if(VOS_ERROR == DRV_ICC_OPEN(astACPUICCCtrlTable.enICCId, astACPUICCCtrlTable.pstICCAttr))
    {
        /* 打开失败时记录当前ICC通道信息 */
        VOS_ProtectionReboot(OM_APP_ICC_INIT_ERROR, THIS_FILE_ID, __LINE__,VOS_NULL_PTR,0);

        return VOS_ERR;
    }

    return VOS_OK;
}

#if (VOS_WIN32 != VOS_OS_VER)
/*****************************************************************************
 函 数 名  : OM_OSAEvent
 功能描述  : 上报OSA的可维可测消息
 输入参数  :
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2013年7月27日
    作    者   : x51137
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID OM_OSAEvent(VOS_VOID *pData, VOS_UINT32 ulLength)
{
    PS_OM_EVENT_IND_STRU    *pstEventInd;
    VOS_UINT32               ulEventLenth;

    ulEventLenth = (VOS_UINT32)sizeof(PS_OM_EVENT_IND_STRU)
                    + ulLength
                    - 4U * (VOS_UINT32)sizeof(VOS_UINT8);/*结构体中包含了4byte的数据所以需要减去*/

    pstEventInd = (PS_OM_EVENT_IND_STRU *)VOS_MemAlloc(ACPU_PID_OM, DYNAMIC_MEM_PT, ulEventLenth);
    if(VOS_NULL_PTR == pstEventInd)
    {
        return;
    }

    pstEventInd->ulLength   = ulEventLenth;

    pstEventInd->usEventId  = (VOS_UINT16)OAM_EVENT_TIMER;

    pstEventInd->ulModuleId = ACPU_PID_OM;

    /*lint -e419 -e416  */
    VOS_MemCpy((VOS_UINT8 *)pstEventInd->aucData, (VOS_UINT8 *)pData, ulLength);
    /*lint +e419 +e416*/

    if (VOS_OK != OM_AcpuEvent(pstEventInd))
    {
        PS_LOG(ACPU_PID_OM, VOS_NULL, PS_PRINT_WARNING, "OM_OSAEvent: send event fail.");
    }

    VOS_MemFree(ACPU_PID_OM, pstEventInd);

    return;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

