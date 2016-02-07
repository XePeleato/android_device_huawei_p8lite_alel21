/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : OmCommonPpm.c
  版 本 号   : 初稿
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   : OM的物理端口管理模块
  函数列表   :
  修改历史   :
  1.日    期   : 2014年5月31日
    作    者   : L00256032
    修改内容   : V8R1 OM_Optimize项目新增

***************************************************************************** */

/*****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "OmCommonPpm.h"
#include "OmAppRl.h"
#include "cpm.h"
#include "omprivate.h"
#include "OmSdPpm.h"

#if (FEATURE_ON == FEATURE_LTE)
#include "msp_diag.h"
#include "diag_common.h"
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* lint -e767  */
#define    THIS_FILE_ID        PS_FILE_ID_OM_COMMON_PPM_C
/* lint +e767  */

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* 用于ACPU上USB设备的UDI句柄 */
UDI_HANDLE                              g_astOMPortUDIHandle[OM_PORT_HANDLE_BUTT];

/* USB承载的OM IND端口中，伪造为同步接口使用的数据结构体 */
OM_PSEUDO_SYNC_STRU                     g_stUsbIndPseudoSync;

/* USB承载的OM CNF端口中，伪造为同步接口使用的数据结构体 */
OM_PSEUDO_SYNC_STRU                     g_stUsbCfgPseudoSync;

VOS_UINT32                              g_ulUSBSendErrCnt   = 0;

/* 端口切换信息的数据结构体 */
PPM_PORT_CFG_INFO_STRU                  g_stPpmPortSwitchInfo;

/* 自旋锁，用来作AT命令端口切换的临界资源保护 */
VOS_SPINLOCK                            g_stPpmPortSwitchSpinLock;


/*****************************************************************************
  3 外部引用声明
*****************************************************************************/
extern OM_CHANNLE_PORT_CFG_STRU         g_stPortCfg;
extern VOS_SPINLOCK                     g_stScmSoftDecodeDataRcvSpinLock;
extern VOS_SPINLOCK                     g_stCbtScmDataRcvSpinLock;
extern PPM_PORT_DEBUG_INFO_STUR         g_stPpmDebugInfo;
/*****************************************************************************
  4 函数实现
*****************************************************************************/
#if (FEATURE_HISOCKET == FEATURE_ON)
extern VOS_UINT32 PPM_SockPortInit(VOS_VOID);
#endif

/*****************************************************************************
 函 数 名  : PPM_DisconnectGUPort
 功能描述  : GU断开OM端口
 输入参数  : ucCpuId:CpuId
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 修改历史  :
   1.日    期  : 2014年05月26日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel)
{
#if 0 /* 断链码流，仅供参考 */
    VOS_UINT8                           aucMsg[]={0x0a,         /* FuncType */
                                                  0x01,         /* CpuId */
                                                  0x08, 0x00,   /* Len */
                                                  0xB3, 0x80,   /* PrimId */
                                                  0x00, 0x00,   /* ToolId */
                                                  0x01, 0x00, 0x00, 0x00};
#endif
    MsgBlock                           *pstMsg;
    APP_OM_MSG_EX_STRU                 *pstAppOmMsg;

    pstMsg = (MsgBlock *)VOS_AllocMsg(UEPS_PID_OMRL, sizeof(MsgBlock) + sizeof(APP_OM_MSG_EX_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_ERR;
    }

    pstAppOmMsg = (APP_OM_MSG_EX_STRU*)pstMsg->aucValue;

    if (OM_LOGIC_CHANNEL_CBT == enChannel)
    {
        pstMsg->ulSenderPid = ACPU_PID_OM;
        pstMsg->ulReceiverPid = ACPU_PID_CBT;
    }
    else
    {
        pstMsg->ulReceiverPid = ACPU_PID_OM;
    }

    pstAppOmMsg->ucFuncType    = OM_QUERY_FUNC;
    pstAppOmMsg->ucCpuId       = (VOS_UINT8)enChannel;
    pstAppOmMsg->usLength      = sizeof(pstAppOmMsg->aucPara)
                                + sizeof(pstAppOmMsg->usToolId)
                                + sizeof(pstAppOmMsg->usPrimId);
    pstAppOmMsg->usToolId      = VOS_NULL;
    pstAppOmMsg->usPrimId      = APP_OM_RELEASE_REQ;

    (VOS_VOID)VOS_SendMsg(UEPS_PID_OMRL, pstMsg);

    return VOS_OK;
}
#if (FEATURE_ON == FEATURE_LTE)
/*****************************************************************************
 函 数 名  : PPM_DisconnectTLPort
 功能描述  : TL断开OM端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 修改历史  :
   1.日    期  : 2014年05月26日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_DisconnectTLPort(VOS_VOID)
{
    DIAG_DATA_MSG_STRU                 *pstMsg;

    pstMsg = (DIAG_DATA_MSG_STRU *)VOS_AllocMsg(UEPS_PID_OMRL, sizeof(DIAG_DATA_MSG_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_ERR;
    }

    pstMsg->ulReceiverPid = MSP_PID_DIAG_FW;
    pstMsg->ulMsgId       = ID_MSG_DIAG_HSO_DISCONN_IND;

    (VOS_VOID)VOS_SendMsg(UEPS_PID_OMRL, pstMsg);

    return VOS_OK;
}
#endif
/*****************************************************************************
 函 数 名  : PPM_DisconnectAllPort
 功能描述  : GUTL断开OM端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 修改历史  :
   1.日    期  : 2014年05月26日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID PPM_DisconnectAllPort(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel)
{
#if (FEATURE_ON == FEATURE_LTE)
    PPM_DisconnectTLPort();
#endif
    PPM_DisconnectGUPort(enChannel);

    return;
}

/*****************************************************************************
 函 数 名  : PPM_GetSendDataLen
 功能描述  : PPM对数据发送长度进行处理
 输入参数  : ulDataLen: SOCP通道的数据长度
             enChanID:  SOCP channel ID
 输出参数  : pulSendDataLen:实际发送出去的数据长度
             penPhyport:指向物理端口号
 返 回 值  : VOS_VOID

 修改历史      :
  1.日    期   : 2014年5月25日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID PPM_GetSendDataLen(SOCP_CODER_DST_ENUM_U32 enChanID, VOS_UINT32 ulDataLen, VOS_UINT32 *pulSendDataLen, CPM_PHY_PORT_ENUM_UINT32 *penPhyport)
{
    CPM_PHY_PORT_ENUM_UINT32    enPhyport;

    if (SOCP_CODER_DST_OM_CNF == enChanID)
    {
        enPhyport = CPM_QueryPhyPort(CPM_OM_CFG_COMM);
    }
    else
    {
        enPhyport = CPM_QueryPhyPort(CPM_OM_IND_COMM);
    }


    /*当发送是通过USB并且发送长度大于60k的时候，需要限制发送长度*/
    if (((CPM_IND_PORT == enPhyport) || (CPM_CFG_PORT == enPhyport))
        &&(ulDataLen > USB_MAX_DATA_LEN))
    {
        LogPrint1("PPM_GetSendDataLen: Change the send Data Len %d", (VOS_INT)ulDataLen);

        *pulSendDataLen = USB_MAX_DATA_LEN;
    }
    else
    {
        *pulSendDataLen = ulDataLen;  /*其他情况下不需要调整当前的大小，包括sd、wifi*/
    }

    *penPhyport = enPhyport;
    return;
}

/*****************************************************************************
 函 数 名  : PPM_PortStatus
 功能描述  : 用于ACPU上面处理物理端口断开后的OM链接断开
 输入参数  : enHandle:   UDI设备句柄
             enPhyPort:  物理端口
             enPortState:端口状态
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2014年5月25日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID PPM_PortStatus(OM_PROT_HANDLE_ENUM_UINT32 enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort,ACM_EVT_E enPortState)
{
    VOS_ULONG                           ulLockLevel;
    OM_LOGIC_CHANNEL_ENUM_UINT32        enChannel;
    VOS_BOOL                            ulSndMsg;

    if (ACM_EVT_DEV_SUSPEND == enPortState)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutTime = OM_GetSlice();

        VOS_SpinLockIntLock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

        ulSndMsg  = VOS_FALSE;
        enChannel = OM_LOGIC_CHANNEL_BUTT;

        /* CFG端口处理GU和TL的端口断开，发消息到GU和TL去处理，但不断开CPM的关联 */
        if (OM_USB_CFG_PORT_HANDLE == enHandle)
        {
            if (enPhyPort == CPM_QueryPhyPort(CPM_OM_CFG_COMM))
            {
                ulSndMsg  = VOS_TRUE;
                enChannel = OM_LOGIC_CHANNEL_CNF;
            }
        }
        /* IND端口断开时发消息到GU和TL去处理 */
        else if (OM_USB_IND_PORT_HANDLE == enHandle)
        {
            if (enPhyPort == CPM_QueryPhyPort(CPM_OM_IND_COMM))
            {
                ulSndMsg  = VOS_TRUE;
                enChannel = OM_LOGIC_CHANNEL_IND;
            }
        }
        else
        {

        }

        VOS_SpinUnlockIntUnlock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

        if (VOS_TRUE == ulSndMsg)
        {
            PPM_DisconnectAllPort(enChannel);
        }
    }
    else if(ACM_EVT_DEV_READY == enPortState)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINTime = OM_GetSlice();
    }
    else
    {
        LogPrint2("PPM_PortStatus: The USB Port %d State %d is Unknow", (VOS_INT)enPhyPort, (VOS_INT)enPortState);

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrTime = OM_GetSlice();
    }

    return;
}

/*****************************************************************************
 函 数 名  : PPM_PortCloseProc
 功能描述  : OM端口已经消失，需要关闭USB端口
 输入参数  : enHandle: 端口句柄枚举定义，关闭后端口设置为VOS_ERROR
             enPhyPort:物理端口
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2014年5月25日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID PPM_PortCloseProc(OM_PROT_HANDLE_ENUM_UINT32  enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort)
{
    VOS_ULONG                           ulLockLevel;
    OM_LOGIC_CHANNEL_ENUM_UINT32        enChannel;
    VOS_BOOL                            ulSndMsg;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseSlice = OM_GetSlice();

    if (VOS_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        return;
    }

    mdrv_udi_close(g_astOMPortUDIHandle[enHandle]);

    g_astOMPortUDIHandle[enHandle] = VOS_ERROR;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkSlice = OM_GetSlice();

    VOS_SpinLockIntLock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    ulSndMsg  = VOS_FALSE;
    enChannel = OM_LOGIC_CHANNEL_BUTT;

    /* CFG端口处理GU和TL的端口断开，发消息到GU和TL去处理，但不断开CPM的关联 */
    if (OM_USB_CFG_PORT_HANDLE == enHandle)
    {
        if (enPhyPort == CPM_QueryPhyPort(CPM_OM_CFG_COMM))
        {
            ulSndMsg  = VOS_TRUE;
            enChannel = OM_LOGIC_CHANNEL_CNF;
        }
    }
    /* IND端口断开时发消息到GU和TL去处理，但不断开CPM的关联 */
    else if (OM_USB_IND_PORT_HANDLE == enHandle)
    {
        if (enPhyPort == CPM_QueryPhyPort(CPM_OM_IND_COMM))
        {
            ulSndMsg  = VOS_TRUE;
            enChannel = OM_LOGIC_CHANNEL_IND;
        }
    }
    else
    {

    }

    VOS_SpinUnlockIntUnlock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    if (VOS_TRUE == ulSndMsg)
    {
        PPM_DisconnectAllPort(enChannel);
    }

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamUSBReadData
 功能描述  : 用于ACPU上面底软把数据通过ICC发送给OM模块
 输入参数  : enPhyPort: 物理端口
             UdiHandle:设备句柄
             enHandle: UDI设备句柄数组偏移
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK

 修改历史      :
  1.日    期   : 2014年5月25日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_INT32 PPM_ReadPortData(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, UDI_HANDLE UdiHandle, OM_PROT_HANDLE_ENUM_UINT32 enHandle)
{
    ACM_WR_ASYNC_INFO                   stInfo;

    if (VOS_ERROR == UdiHandle)
    {
        LogPrint("\r\n PPM_PortCloseProc: Input HANDLE  is err. \n");

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleErr++;

        return VOS_ERR;
    }

    VOS_MemSet(&stInfo, 0, sizeof(stInfo));

    /* 获取USB的IO CTRL口的读缓存 */
    if (VOS_OK != mdrv_udi_ioctl(UdiHandle, UDI_ACM_IOCTL_GET_READ_BUFFER_CB, &stInfo))
    {
        LogPrint("\r\n PPM_ReadPortData:Call mdrv_udi_ioctl is Failed\n");

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadGetBufferErr++;

        return VOS_ERR;
    }

    /*lint -e40*/
    OM_ACPU_DEBUG_CHANNEL_TRACE(enPhyPort, (VOS_UINT8*)stInfo.pVirAddr, stInfo.u32Size, OM_ACPU_USB_CB);
    /*lint +e40*/

    if(VOS_OK != CPM_ComRcv(enPhyPort, (VOS_UINT8*)stInfo.pVirAddr, stInfo.u32Size))
    {
        LogPrint1("\r\n Info: PPM_ReadPortData:Call CPM_ComRcv is NULL, PhyPort is %d\n", (VOS_INT)enPhyPort);

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiCommRcvNullPtrErr++;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktByte += stInfo.u32Size;

    if(VOS_OK != mdrv_udi_ioctl(UdiHandle, UDI_ACM_IOCTL_RETUR_BUFFER_CB, &stInfo))
    {
        LogPrint("\r\n PPM_ReadPortData:Call mdrv_udi_ioctl UDI_ACM_IOCTL_RETUR_BUFFER_CB is Failed\n");

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadBufferFreeErr++;
    }

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : PPM_PortPseudoSyncGetSmp
 功能描述  : 伪同步接口，需要在调用底软异步写函数后，获取信号量
 输入参数  : enHandle:UDI设备句柄
 输出参数  : VOS_VOID
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年5月25日
    作    者   : L00256032
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID PPM_PortPseudoSyncGetSmp(OM_PROT_HANDLE_ENUM_UINT32 enHandle)
{
#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
    if (OM_USB_IND_PORT_HANDLE == enHandle)
    {
        if (VOS_OK != VOS_SmP(g_stUsbIndPseudoSync.ulPseudoSyncSemId, 0))
        {
            g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBIndPseudoSyncFailNum++;
            g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBIndPseudoSyncFailSlice = OM_GetSlice();
        }
    }
    else if (OM_USB_CFG_PORT_HANDLE == enHandle)
    {
        if (VOS_OK != VOS_SmP(g_stUsbCfgPseudoSync.ulPseudoSyncSemId, 0))
        {
            g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCnfPseudoSyncFailNum++;
            g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCnfPseudoSyncFailSlice = OM_GetSlice();
        }
    }
    else
    {
        ;
    }
#endif
    return;
}

/*****************************************************************************
 函 数 名  : PPM_QueryLogPort
 功能描述  : 提供给NAS进行Log端口查询
 输入参数  : 无
 输出参数  : pulLogPort当前Log输出端口

 返 回 值  : VOS_ERR/VOS_OK

 修改历史  :
   1.日    期  : 2014年06月3日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_QueryLogPort(VOS_UINT32  *pulLogPort)
{
    if (VOS_NULL_PTR == pulLogPort)
    {
        vos_printf("\r\n PPM_QueryLogPort: para is NULL !!\n");
        return VOS_ERR;
    }

    *pulLogPort = g_stPortCfg.enPortNum;

    if ((CPM_OM_PORT_TYPE_USB != *pulLogPort) && (CPM_OM_PORT_TYPE_VCOM != *pulLogPort))
    {
        vos_printf("\r\n PPM_QueryLogPort: log prot is not support. port:%d!!\n", *pulLogPort);
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : PPM_LogPortSwitch
 功能描述  : 提供给NAS进行端口切换
 输入参数  : enPhyPort: 带切换物理端口枚举值
             ulEffect:是否立即生效
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK

 修改历史  :
   1.日    期  : 2014年6月3日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_LogPortSwitch(VOS_UINT32  ulPhyPort, VOS_BOOL ulEffect)
{
    CPM_PHY_PORT_ENUM_UINT32            enPhyCfgPort;
    CPM_PHY_PORT_ENUM_UINT32            enPhyIndPort;
    VOS_ULONG                           ulLockLevel;
    VOS_BOOL                            ulSndMsg;

    if ((CPM_OM_PORT_TYPE_USB != ulPhyPort) && (CPM_OM_PORT_TYPE_VCOM != ulPhyPort))
    {
        vos_printf("\r\n PPM_LogPortSwitch: enPhyPort is error.port %d\n", ulPhyPort);

        g_stPpmPortSwitchInfo.ulPortTypeErr++;

        return VOS_ERR;
    }

    /* 切换的端口与当前端口一致不切换 */
    if (ulPhyPort == g_stPortCfg.enPortNum)
    {
#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
        /* 切换到VCOM输出时在LOG延迟写入开启情况下需要重新设置SOCP的超时中断 */
        if (CPM_OM_PORT_TYPE_VCOM == g_stPortCfg.enPortNum)
        {
            mdrv_socp_set_log_int_timeout();
        }

        /* 为了规避USB输出时开启了延时写入无法连接工具,切换到USB输出时需要重新设置SOCP的超时中断到默认值 */
        if (CPM_OM_PORT_TYPE_USB == g_stPortCfg.enPortNum)
        {
            mdrv_socp_set_log_int_default_timeout();
        }
#endif
        return VOS_OK;
    }

    g_stPpmPortSwitchInfo.ulStartSlice = OM_GetSlice();

    enPhyCfgPort = CPM_QueryPhyPort(CPM_OM_CFG_COMM);
    enPhyIndPort = CPM_QueryPhyPort(CPM_OM_IND_COMM);

    ulSndMsg = VOS_FALSE;

    VOS_SpinLockIntLock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    /* 切换到VCOM输出 */
    if (CPM_OM_PORT_TYPE_VCOM == ulPhyPort)
    {
        /* 当前是USB输出 */
        if ((CPM_CFG_PORT == enPhyCfgPort) && (CPM_IND_PORT == enPhyIndPort))
        {
            /* 需要断开连接 */
            ulSndMsg = VOS_TRUE;

            CPM_DisconnectPorts(CPM_CFG_PORT, CPM_OM_CFG_COMM);
            CPM_DisconnectPorts(CPM_IND_PORT, CPM_OM_IND_COMM);
        }

        /* 当前OM走VCOM上报 */
        CPM_ConnectPorts(CPM_VCOM_CFG_PORT, CPM_OM_CFG_COMM);
        CPM_ConnectPorts(CPM_VCOM_IND_PORT, CPM_OM_IND_COMM);

        g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_VCOM;
    }
    /* 切换到USB输出 */
    else
    {
        /* 当前是VCOM输出 */
        if ((CPM_VCOM_CFG_PORT == enPhyCfgPort) && (CPM_VCOM_IND_PORT == enPhyIndPort))
        {
            /* 断开连接 */
            ulSndMsg = VOS_TRUE;

            CPM_DisconnectPorts(CPM_VCOM_CFG_PORT, CPM_OM_CFG_COMM);
            CPM_DisconnectPorts(CPM_VCOM_IND_PORT, CPM_OM_IND_COMM);
        }

        /* OM走USB上报 */
        CPM_ConnectPorts(CPM_CFG_PORT, CPM_OM_CFG_COMM);
        CPM_ConnectPorts(CPM_IND_PORT, CPM_OM_IND_COMM);

        g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_USB;
    }

    VOS_SpinUnlockIntUnlock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    if (VOS_TRUE == ulSndMsg)
    {
        PPM_DisconnectAllPort(OM_LOGIC_CHANNEL_CNF);
    }
#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
    /* 切换到VCOM输出时在LOG延迟写入开启情况下需要重新设置SOCP的超时中断 */
    if (CPM_OM_PORT_TYPE_VCOM == g_stPortCfg.enPortNum)
    {
        mdrv_socp_set_log_int_timeout();
    }

    /* 为了规避USB输出时开启了延时写入无法连接工具,切换到USB输出时需要重新设置SOCP的超时中断到默认值 */
    if (CPM_OM_PORT_TYPE_USB == g_stPortCfg.enPortNum)
    {
        mdrv_socp_set_log_int_default_timeout();
    }
#endif
    g_stPpmPortSwitchInfo.ulSwitchSucc++;
    g_stPpmPortSwitchInfo.ulEndSlice = OM_GetSlice();

    if (VOS_TRUE == ulEffect)
    {
        if (VOS_OK != NV_Write(en_NV_Item_Om_Port_Type, &g_stPortCfg, sizeof(OM_CHANNLE_PORT_CFG_STRU)))
        {
            vos_printf("\r\n PPM_LogPortSwitch: NV_Write fail\n");
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : PPM_UdiRegCallBackFun
 功能描述  : 用于注册UDI设备的回调函数
 输入参数  : enHandle: 端口的句柄
             ulCmdType: 命令类型
             pFunc: 注册的回调函数指针
 输出参数  : 无
 返 回 值  : VOS_OK/VOS_ERR
 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_UINT32 PPM_UdiRegCallBackFun(UDI_HANDLE enHandle, VOS_UINT32 ulCmdType, VOS_VOID* pFunc)
{
    if (VOS_NULL_PTR == pFunc)
    {
        return VOS_OK;
    }

    if (VOS_OK != mdrv_udi_ioctl(enHandle, ulCmdType, pFunc))
    {
        LogPrint("\r\n PPM_UdiRegCallBackFun, mdrv_udi_ioctl Failed\r\n");

        return VOS_ERR;
    }

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : PPM_ReadPortDataInit
 功能描述  : 用于初始化OM使用的设备
 输入参数  : enPhyPort: 物理端口号
             enHandle: 端口的句柄
             pReadCB: 该端口上面的读取回调函数
             pWriteCB: 该端口上面的异步写回调函数
             pStateCB: 该端口上面的状态回调函数
 输出参数  : 无
 返 回 值  : VOS_OK/VOS_ERR
 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID PPM_ReadPortDataInit(CPM_PHY_PORT_ENUM_UINT32        enPhyPort,
                                    OM_PROT_HANDLE_ENUM_UINT32          enHandle,
                                    VOS_VOID                            *pReadCB,
                                    VOS_VOID                            *pWriteCB,
                                    VOS_VOID                            *pStateCB)
{
    UDI_OPEN_PARAM_S                    stUdiPara;
    ACM_READ_BUFF_INFO                  stReadBuffInfo;

    /*初始化当前使用的USB通道*/
    /* Add by h59254 for V8R1 OM begin */
    if (CPM_IND_PORT == enPhyPort)
    {
        stUdiPara.devid            = UDI_ACM_LTE_DIAG_ID;
    }
    else if (CPM_CFG_PORT == enPhyPort)
    {
        stUdiPara.devid            = UDI_ACM_GPS_ID;
    }
    else if (CPM_HSIC_IND_PORT == enPhyPort)
    {
        stUdiPara.devid            = UDI_ACM_HSIC_ACM7_ID;
    }
    else if (CPM_HSIC_CFG_PORT == enPhyPort)
    {
        stUdiPara.devid            = UDI_ACM_HSIC_ACM8_ID;
    }
    /* Add by h59254 for V8R1 OM end */
    else
    {
        LogPrint1("\r\n PPM_ReadPortDataInit: Open Wrong Port %d!", (VOS_INT)enPhyPort);

        return;
    }
#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
    stReadBuffInfo.u32BuffSize = OM_ICC_BUFFER_SIZE;
    stReadBuffInfo.u32BuffNum  = OM_DRV_MAX_IO_COUNT;
#else
#define OM_USB_BUFFER_SIZE          (8*1024)
#define OM_USB_BUFFER_NUM           (8)
    stReadBuffInfo.u32BuffSize = OM_USB_BUFFER_SIZE;
    stReadBuffInfo.u32BuffNum  = OM_USB_BUFFER_NUM;
#endif
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice = OM_GetSlice();

    if (VOS_ERROR != g_astOMPortUDIHandle[enHandle])
    {
        LogPrint("\r\n PPM_ReadPortDataInit: The UDI Handle is not Null !");

        return;
    }

    /* 打开OM使用的USB通道 */
    g_astOMPortUDIHandle[enHandle] = mdrv_udi_open(&stUdiPara);

    if (VOS_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        LogPrint("\r\n PPM_ReadPortDataInit: Open UDI ACM failed!");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice = OM_GetSlice();

    /* 配置OM使用的USB通道缓存 */
    if (VOS_OK != mdrv_udi_ioctl(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo))
    {
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_RELLOC_READ_BUFF Failed\r\n");

        return;
    }

    /* 注册OM使用的USB读数据回调函数 */
    if (VOS_OK != PPM_UdiRegCallBackFun(g_astOMPortUDIHandle[enHandle], UDI_ACM_IOCTL_SET_READ_CB, pReadCB))
    {
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl UDI_ACM_IOCTL_SET_READ_CB Failed\r\n");

        return;
    }

    if(VOS_OK != PPM_UdiRegCallBackFun(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_SET_WRITE_CB, pWriteCB))
    {
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_SET_WRITE_CB Failed\r\n");

        return;
    }

    if(VOS_OK != PPM_UdiRegCallBackFun(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_SET_EVT_CB, pStateCB))
    {
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_SET_WRITE_CB Failed\r\n");

        return;
    }

    if (VOS_OK != mdrv_udi_ioctl(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_WRITE_DO_COPY, VOS_NULL_PTR))
    {
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_WRITE_DO_COPY Failed\r\n");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice = OM_GetSlice();

    return;
}

/*****************************************************************************
 函 数 名  : PPM_PortSend
 功能描述  : 将输入的数据通过异步发送给PC侧
 输入参数  :  enHandle: 发送端口句柄的枚举定义
              pucVirAddr:   数据虚地址
              pucPhyAddr:   数据实地址
              ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : VOS_ERROR/VOS_OK
 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_UINT32 PPM_PortSend(OM_PROT_HANDLE_ENUM_UINT32 enHandle, VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    VOS_INT32           lRet;
    ACM_WR_ASYNC_INFO   stVcom;
    VOS_UINT32          ulInSlice;
    VOS_UINT32          ulOutSlice;
    VOS_UINT32          ulWriteSlice;
    VOS_CHAR            aucUsbLog[100];

    if ((VOS_NULL_PTR == pucVirAddr) || (VOS_NULL_PTR == pucPhyAddr))
    {
        /* 打印错误 */
        LogPrint("\r\nPPM_PortSend: Vir or Phy Addr is Null \n");

        return CPM_SEND_PARA_ERR;
    }

    stVcom.pVirAddr = (VOS_CHAR*)pucVirAddr;
    stVcom.pPhyAddr = (VOS_CHAR*)pucPhyAddr;
    stVcom.u32Size  = ulDataLen;
    stVcom.pDrvPriv = VOS_NULL_PTR;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum1++;

    if (VOS_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        LogPrint("\r\nPPM_PortSend: USB HANDLE  is err. \n");

        return CPM_SEND_ERR;
    }

    ulInSlice = OM_GetSlice();

    /* 返回写入数据长度代表写操作成功 */
    lRet = mdrv_udi_ioctl(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_WRITE_ASYNC, &stVcom);

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum2++;

    ulOutSlice = OM_GetSlice();

    if(ulInSlice > ulOutSlice)
    {
        ulWriteSlice = ulInSlice - ulOutSlice;
    }
    else
    {
        ulWriteSlice = ulOutSlice - ulInSlice;
    }

    if(ulWriteSlice > g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime = ulWriteSlice;
    }

    if (MDRV_OK == lRet)     /*当前发送成功*/
    {
        /* 伪同步接口，获取信号量 */
        PPM_PortPseudoSyncGetSmp(enHandle);
#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
        return CPM_SEND_OK;
#else
        return CPM_SEND_AYNC;
#endif
    }
    else if(MDRV_OK > lRet)    /*临时错误*/
    {
        /*打印信息，调用UDI接口的错误信息*/
        LogPrint1("\r\nPPM_PortSend: mdrv_udi_ioctl Send Data return Error %d\n", lRet);

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrNum++;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrLen    += ulDataLen;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrValue  = (VOS_UINT32)lRet;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrTime   = OM_GetSlice();

        if (VOS_OK != OM_AcpuGetLogPath(aucUsbLog, OM_USB_LOG_FILE, OM_USB_UNITARY_LOG_FILE))
        {
            return CPM_SEND_FUNC_NULL;
        }

        if( 0 == (g_ulUSBSendErrCnt%USB_SEND_DATA_ERROR_MAX)) /*累计丢包超过限制需要记录到log文件中*/
        {
            OM_Acpu_WriteLogFile(aucUsbLog, (VOS_CHAR *)&g_stAcpuDebugInfo, sizeof(OM_ACPU_DEBUG_INFO));
        }

        g_ulUSBSendErrCnt++;

        return CPM_SEND_FUNC_NULL; /*对于临时错误，需要返回NULL丢弃数据*/
    }
    else    /*其他错误需要复位单板*/
    {
        /*打印信息，调用UDI接口*/
        LogPrint1("\r\nPPM_PortSend: mdrv_udi_ioctl Send Data return Error %d\n", lRet);

        VOS_ProtectionReboot(OAM_USB_SEND_ERROR, lRet, (VOS_INT)enHandle,
                             (VOS_CHAR *)&g_stAcpuDebugInfo, (VOS_INT)sizeof(OM_ACPU_DEBUG_INFO));

        return CPM_SEND_ERR;
    }
}

/*****************************************************************************
 函 数 名  :GU_OamPortWriteAsyCB
 功能描述  :OM异步发送回调函数
 输入参数  :enHandle:UDI句柄
            pucData:需要发送的数据内容
            lLen:数据长度
 输出参数  :无
 返 回 值  :无
 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID PPM_PortWriteAsyCB(OM_PROT_HANDLE_ENUM_UINT32 enHandle, VOS_UINT8* pucData, VOS_INT lLen)
{
    VOS_UINT32      ulRlsLen;

    if(lLen < 0)
    {
        ulRlsLen = 0;
    }
    else
    {
        ulRlsLen = (VOS_UINT32)lLen;
    }

    /* 伪同步接口，释放信号量 */
    if (OM_USB_IND_PORT_HANDLE == enHandle)
    {
        g_stUsbIndPseudoSync.ulLen          = ulRlsLen;
        g_stUsbIndPseudoSync.pucAsyncCBData = pucData;

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
        VOS_SmV(g_stUsbIndPseudoSync.ulPseudoSyncSemId);
#else
        SCM_RlsDestBuf(SOCP_CODER_DST_OM_IND, ulRlsLen);
#endif
    }
    else if (OM_USB_CFG_PORT_HANDLE == enHandle)
    {
        g_stUsbCfgPseudoSync.ulLen          = ulRlsLen;
        g_stUsbCfgPseudoSync.pucAsyncCBData = pucData;

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
        VOS_SmV(g_stUsbCfgPseudoSync.ulPseudoSyncSemId);
#else
        SCM_RlsDestBuf(SOCP_CODER_DST_OM_CNF, ulRlsLen);
#endif
    }
    else
    {
        ;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteCBNum++;

    return;
}

/*****************************************************************************
 函 数 名  : PPM_InitPhyPort
 功能描述  : 初始化物理通道
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败
 修改历史:
     1. 日期:2012-01-21
         修改人:s00207770
         修改原因:添加SOCKET端口初始化
     2. 日期:2013-12-29
         修改人:s00207770
         修改原因:修改LOG保存初始化接口
*****************************************************************************/
VOS_UINT32 PPM_InitPhyPort(VOS_VOID)
{
    if (VOS_OK != PPM_LogFileInit())
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "PPM_InitPhyPort: PPM_LogFileInit failed.\n");
        return VOS_ERR;
    }

    if (VOS_OK != PPM_PortInit())
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "PPM_InitPhyPort: PPM_PortInit failed.\n");
        return VOS_ERR;
    }

#if ((VOS_OS_VER == VOS_WIN32) || (FEATURE_HISOCKET == FEATURE_ON))
    if (VOS_OK != PPM_SockPortInit())
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : PPM_PortInit
 功能描述  : 端口通道的初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_OK   - 初始化成功
             VOS_ERR  - 初始化失败

 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_UINT32 PPM_PortInit(VOS_VOID)
{
    VOS_MemSet(&g_stAcpuDebugInfo, 0, sizeof(g_stAcpuDebugInfo));

    VOS_MemSet(g_astOMPortUDIHandle, VOS_ERROR, sizeof(g_astOMPortUDIHandle));

    VOS_MemSet(&g_stPpmPortSwitchInfo, 0, sizeof(g_stPpmPortSwitchInfo));

    VOS_MemSet(&g_stPpmDebugInfo, 0, sizeof(g_stPpmDebugInfo));

    VOS_SpinLockInit(&g_stPpmPortSwitchSpinLock);

    /* USB承载的虚拟端口通道的初始化 */
    PPM_UsbPortInit();

    /* Hsic承载的虚拟端口通道的初始化 */
    PPM_HsicPortInit();

    /* Vcom承载的虚拟端口通道的初始化 */
    PPM_VComPortInit();

    return VOS_OK;
}



/*****************************************************************************
 函 数 名  : PPM_OmPortInfoShow
 功能描述  : 用于打印当前通道的统计信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2011年3月10日
     作    者  : l46160
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID PPM_OmPortInfoShow(OM_PROT_HANDLE_ENUM_UINT32  enHandle)
{
    vos_printf("\r\nThe Port Write num 1 is          %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum1);
    vos_printf("\r\nThe Port Write num 2 is          %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum2);
    vos_printf("\r\nThe Port Write Max Time is       0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime);

    vos_printf("\r\nThe Port Write CB Num is         %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteCBNum);

    vos_printf("\r\nThe Port Write Err Time is       %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrTime);
    vos_printf("\r\nThe Port Write Err Num  is       %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrNum);
    vos_printf("\r\nThe Port Write Err Value is      0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrValue);
    vos_printf("\r\nThe Port Write Err Len is        0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrLen);

    vos_printf("\r\nThe Port In CB Num is            %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINNum);
    vos_printf("\r\nThe Port In CB Time is           0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINTime);
    vos_printf("\r\nThe Port Out CB Num is           %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutNum);
    vos_printf("\r\nThe Port Out CB Time is          0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutTime);
    vos_printf("\r\nThe Port State CB Err Num is     %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrNum);
    vos_printf("\r\nThe Port State CB Err Time is    0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrTime);

    vos_printf("\r\nThe Port Open num is            %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum);
    vos_printf("\r\nThe Port Open slice is          0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice);

    vos_printf("\r\nThe Port Open OK num is         %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum);
    vos_printf("\r\nThe Port Open OK slice is       0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice);

    vos_printf("\r\nThe Port Open OK2 num is        %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num);
    vos_printf("\r\nThe Port Open OK2 slice is      0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice);

    vos_printf("\r\nThe Port Close num is           %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseNum);
    vos_printf("\r\nThe Port Close slice is         0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseSlice);

    vos_printf("\r\nThe Port Close OK num is        %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkNum);
    vos_printf("\r\nThe Port Close OK slice is      0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkSlice);

    vos_printf("\r\nUSB IND Pseudo sync fail num is   %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBIndPseudoSyncFailNum);
    vos_printf("\r\nUSB IND Pseudo sync fail slice is 0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBIndPseudoSyncFailSlice);
    vos_printf("\r\nUSB CFG Pseudo sync fail num is   %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCnfPseudoSyncFailNum);
    vos_printf("\r\nUSB CFG Pseudo sync fail slice is 0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCnfPseudoSyncFailSlice);

    vos_printf("\r\nThe Port UDI Handle Err num is %d",                 g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleErr);
    vos_printf("\r\nThe Port UDI Handle Get Buffer Err num is %d",      g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadGetBufferErr);
    vos_printf("\r\nThe Port UDI Handle Comm Rcv Null Ptr num is %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiCommRcvNullPtrErr);
    vos_printf("\r\nThe Port UDI Handle Read Buffer Free Err num is %d",g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadBufferFreeErr);

    vos_printf("\r\nThe Port UDI Handle Total Rcv Pkt num is %d",       g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktNum);
    vos_printf("\r\nThe Port UDI Handle Total Rcv Byte is %d",          g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktByte);


    return;
}

/*****************************************************************************
 函 数 名  : PPM_PortSwitchInfoShow
 功能描述  : 用于打印端口切换的统计信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2014年6月10日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID PPM_PortSwitchInfoShow(VOS_VOID)
{
    vos_printf("\r\n Port Type Err num is %d", g_stPpmPortSwitchInfo.ulPortTypeErr);

    vos_printf("\r\n Port Switch Fail time is %d", g_stPpmPortSwitchInfo.ulSwitchFail);

    vos_printf("\r\n Port Switch Success time is %d", g_stPpmPortSwitchInfo.ulSwitchSucc);

    vos_printf("\r\n Port Switch Start slice is 0x%x", g_stPpmPortSwitchInfo.ulStartSlice);

    vos_printf("\r\n Port Switch End slice is 0x%x", g_stPpmPortSwitchInfo.ulEndSlice);

    return;
}



#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif




