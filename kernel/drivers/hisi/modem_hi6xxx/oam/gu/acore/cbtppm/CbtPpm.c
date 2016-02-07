/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : CbtPpm.c
  版 本 号   : 初稿
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   : CBT的物理端口管理模块
  函数列表   :
  修改历史   :
  1.日    期   : 2014年5月31日
    作    者   : L00256032
    修改内容   : V8R1 OM_Optimize项目新增

***************************************************************************** */

/*****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "AtAppVcomInterface.h"
#include "CbtPpm.h"
#include "CbtCpm.h"
#include "OmCommonPpm.h"
#include "OmAppRl.h"
#include "OmApp.h"
#include "SCMSoftDecode.h"
#include "omprivate.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* lint -e767  */
#define    THIS_FILE_ID        PS_FILE_ID_CBT_PPM_C
/* lint +e767  */

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
VOS_SEM                                 g_ulCbtUsbPseudoSyncSemId;   /* 伪造为同步接口需使用的信号量 */

CBTPPM_PORT_DEBUG_INFO_STUR             g_stCbtPpmDebugInfo = {0, 0, 0};

/* 用于ACPU上CBT端口的UDI句柄 */
UDI_HANDLE                         g_ulCbtPortUDIHandle    = VOS_ERROR;

/*****************************************************************************
  3 外部引用声明
*****************************************************************************/
extern  CBTCPM_RCV_FUNC                 g_pCbtRcvFunc;
extern  CBTCPM_SEND_FUNC                g_pCbtSndFunc;
extern  OM_VCOM_DEBUG_INFO         g_stVComDebugInfo[3];
extern  OMRL_CBT_HDLC_ENCODE_MEM_CTRL           g_stCbtHdlcEncBuf;

extern VOS_UINT32 CBTSCM_SoftDecodeDataRcv(VOS_UINT8 *pucBuffer, VOS_UINT32 ulLen);

/*****************************************************************************
  4 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : CBTPPM_OamCbtPortDataSnd
 功能描述  : 发送CBT数据
 输入参数  : pucVirAddr:   数据虚地址
             pucPhyAddr:   数据实地址
             ulDataLen:    数据长度
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 修改历史  :
   1.日    期  : 2014年5月26日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 CBTPPM_OamCbtPortDataSnd(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    CBTCPM_SEND_FUNC                    pFunc;

    pFunc = CBTCPM_GetSndFunc();

    if (VOS_NULL_PTR == pFunc)
    {
        return VOS_ERR;
    }

    return pFunc(pucVirAddr, pucPhyAddr, ulDataLen);
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamUsbCbtSendData
 功能描述  : 将输入的数据通过USB(APP口)发送给PC侧
 输入参数  : pucVirAddr:   数据虚地址
             pucPhyAddr:   数据实地址
             ulDataLen:    数据长度
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2011年10月8日
     作    者  : g47350
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 CBTPPM_OamUsbCbtSendData(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    VOS_INT32                           lRet;
    ACM_WR_ASYNC_INFO                   stVcom;
    VOS_UINT32                          ulInSlice;
    VOS_UINT32                          ulOutSlice;
    VOS_UINT32                          ulWriteSlice;

    if ((VOS_NULL_PTR == pucVirAddr) || (VOS_NULL_PTR == pucPhyAddr))
    {
        /* 打印错误 */
        LogPrint("\r\n CBTPPM_OamUsbCbtSendData: Vir or Phy Addr is Null \n");

        return CPM_SEND_PARA_ERR;
    }

    stVcom.pVirAddr = (VOS_CHAR*)pucVirAddr;
    stVcom.pPhyAddr = (VOS_CHAR*)pucPhyAddr;
    stVcom.u32Size  = ulDataLen;
    stVcom.pDrvPriv = VOS_NULL_PTR;

    g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteNum1++;

    ulInSlice = OM_GetSlice();

    lRet = mdrv_udi_ioctl(g_ulCbtPortUDIHandle, ACM_IOCTL_WRITE_ASYNC, &stVcom);

    g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteNum2++;

    ulOutSlice = OM_GetSlice();

    if(ulInSlice > ulOutSlice)
    {
        ulWriteSlice = ulInSlice - ulOutSlice;
    }
    else
    {
        ulWriteSlice = ulOutSlice - ulInSlice;
    }

    if(ulWriteSlice > g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteMaxTime)
    {
        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteMaxTime = ulWriteSlice;
    }

    if (MDRV_OK == lRet)     /*当前发送成功*/
    {
        if (VOS_OK != VOS_SmP(g_ulCbtUsbPseudoSyncSemId, 0))
        {
            LogPrint1("\r\n CBTPPM_OamUsbCbtSendData: mdrv_udi_ioctl Send Data return Error %d\n", lRet);
            return VOS_ERR;
        }

        return VOS_OK;
    }
    else if (MDRV_OK > lRet)    /*临时错误*/
    {
        /*打印信息，调用UDI接口的错误信息*/
        LogPrint1("\r\n CBTPPM_OamUsbCbtSendData: mdrv_udi_ioctl Send Data return Error %d\n", lRet);

        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteErrNum++;
        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteErrLen    += ulDataLen;
        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteErrValue  = (VOS_UINT32)lRet;
        g_stAcpuDebugInfo.astPortInfo[OM_USB_CBT_PORT_HANDLE].ulUSBWriteErrTime   = OM_GetSlice();

        return VOS_ERR; /*对于临时错误，需要返回NULL丢弃数据*/
    }
    else    /*其他错误需要复位单板*/
    {
        /*打印信息，调用UDI接口*/
        LogPrint1("\r\n CBTPPM_OamUsbCbtSendData: mdrv_udi_ioctl Send Data return Error %d\n", lRet);

        VOS_ProtectionReboot(OAM_USB_SEND_ERROR, (VOS_INT)THIS_FILE_ID, (VOS_INT)__LINE__,
                             (VOS_CHAR *)&lRet, sizeof(VOS_INT32));

        return VOS_ERR;
    }

}

/*****************************************************************************
 函 数 名  : CBTPPM_OamUsbCbtPortClose
 功能描述  : 用于关闭USB校准端口的处理
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID CBTPPM_OamUsbCbtPortClose(VOS_VOID)
{
    if (VOS_ERROR == g_ulCbtPortUDIHandle)
    {
        return;
    }

    mdrv_udi_close(g_ulCbtPortUDIHandle);

    g_ulCbtPortUDIHandle = VOS_ERROR;

    /* CBT端口从USB或VCOM切换到UART时接收函数指针为空，收到USB状态变更时不做断开处理 */
    if (VOS_NULL_PTR == CBTCPM_GetRcvFunc())
    {
        return;
    }

    PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_CBT);

    return;
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamUsbCbtWriteDataCB
 功能描述  : USB承载的CBT端口异步发送数据的回调
 输入参数  : pucData:   发送的数据内容
             ulDataLen: 实际发送数据长度
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID CBTPPM_OamUsbCbtWriteDataCB(VOS_UINT8* pucVirData, VOS_UINT8* pucPhyData, VOS_INT lLen)
{
    if (lLen < 0)
    {
        LogPrint("\r\n CBTPPM_OamUsbCbtWriteDataCB: lLen < 0. \n");
    }

    /* 伪同步接口，释放信号量 */
    VOS_SmV(g_ulCbtUsbPseudoSyncSemId);

    return;
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamUsbCbtStatusCB
 功能描述  : 用于ACPU上面处理USB物理端口断开后的校准断开
 输入参数  : enPortState:   端口状态
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID CBTPPM_OamUsbCbtStatusCB(ACM_EVT_E enPortState)
{
    /* CBT端口从USB或VCOM切换到UART时接收函数指针为空，收到USB状态变更时不做断开处理 */
    if (VOS_NULL_PTR == CBTCPM_GetRcvFunc())
    {
        return;
    }

    /* CBT端口只处理GU的端口断开 */
    PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_CBT);

    return;
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamUsbCbtPortOpen
 功能描述  : 用于初始化USB承载的CBT使用的端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID CBTPPM_OamUsbCbtPortOpen(VOS_VOID)
{
    CBTPPM_OamCbtPortDataInit(OM_USB_CBT_PORT_HANDLE,
                              CBTPPM_OamUsbCbtReadDataCB,
                              CBTPPM_OamUsbCbtWriteDataCB,
                              CBTPPM_OamUsbCbtStatusCB);

    return;
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamUsbCbtReadDataCB
 功能描述  : 用于ACPU上面底软把USB承载的CBT口数据通过ICC发送给OM模块
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT32 CBTPPM_OamUsbCbtReadDataCB(VOS_VOID)
{
    ACM_WR_ASYNC_INFO                   stInfo;
    CBTCPM_RCV_FUNC                     pFunc;

    if (VOS_ERROR == g_ulCbtPortUDIHandle)
    {
        LogPrint("\r\n CBTPPM_OamUsbCbtReadDataCB: Input HANDLE  is err. \n");

        return VOS_ERR;
    }

    VOS_MemSet(&stInfo, 0, sizeof(stInfo));

    /* 获取USB的IO CTRL口的读缓存 */
    if (VOS_OK != mdrv_udi_ioctl(g_ulCbtPortUDIHandle, UDI_ACM_IOCTL_GET_READ_BUFFER_CB, &stInfo))
    {
        LogPrint("\r\n CBTPPM_OamUsbCbtReadDataCB:Call mdrv_udi_ioctl is Failed\n");

        return VOS_ERR;
    }

    /*lint -e40*/
    OM_ACPU_DEBUG_TRACE((VOS_UINT8*)stInfo.pVirAddr, stInfo.u32Size, OM_ACPU_USB_CB);
    /*lint +e40*/

    /* 数据接收函数 */
    pFunc = CBTCPM_GetRcvFunc();

    if (VOS_NULL_PTR != pFunc)
    {
        if (VOS_OK != pFunc((VOS_UINT8 *)stInfo.pVirAddr, stInfo.u32Size))
        {
            /* 增加可维可测计数 */
        }
    }

    if(VOS_OK != mdrv_udi_ioctl(g_ulCbtPortUDIHandle, UDI_ACM_IOCTL_RETUR_BUFFER_CB, &stInfo))
    {
        LogPrint("\r\n CBTPPM_OamUsbCbtReadDataCB:Call mdrv_udi_ioctl UDI_ACM_IOCTL_RETUR_BUFFER_CB is Failed\n");

        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamCbtPortDataInit
 功能描述  : 用于初始化CBT使用的端口设备
 输入参数  : enHandle: 端口的句柄
             pReadCB: 该端口上面的读取回调函数
             pWriteCB: 该端口上面的异步写回调函数
             pStateCB: 该端口上面的状态回调函数
 输出参数  : 无
 返 回 值  : VOS_OK/VOS_ERR
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID CBTPPM_OamCbtPortDataInit(OM_PROT_HANDLE_ENUM_UINT32          enHandle,
                                        VOS_VOID                            *pReadCB,
                                        VOS_VOID                            *pWriteCB,
                                        VOS_VOID                            *pStateCB)
{
    UDI_OPEN_PARAM_S                    stUdiPara;
    ACM_READ_BUFF_INFO                  stReadBuffInfo;


    stUdiPara.devid            = UDI_ACM_OM_ID;

    stReadBuffInfo.u32BuffSize = OM_ICC_BUFFER_SIZE;
    stReadBuffInfo.u32BuffNum  = OM_DRV_MAX_IO_COUNT;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice = OM_GetSlice();

    if (VOS_ERROR != g_ulCbtPortUDIHandle)
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit: The UDI Handle is not Null !");

        return;
    }

    /* 打开CBT使用的USB通道 */
    g_ulCbtPortUDIHandle = mdrv_udi_open(&stUdiPara);

    if (VOS_ERROR == g_ulCbtPortUDIHandle)
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit: Open UDI ACM failed!");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice = OM_GetSlice();

    /* 配置CBT使用的USB通道缓存 */
    if (VOS_OK != mdrv_udi_ioctl(g_ulCbtPortUDIHandle, ACM_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, mdrv_udi_ioctl ACM_IOCTL_RELLOC_READ_BUFF Failed\r\n");

        return;
    }

    /* 注册OM使用的USB读数据回调函数 */
    if (VOS_OK != mdrv_udi_ioctl(g_ulCbtPortUDIHandle, UDI_ACM_IOCTL_SET_READ_CB, pReadCB))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, mdrv_udi_ioctl UDI_ACM_IOCTL_SET_READ_CB Failed\r\n");

        return;
    }

    if(VOS_OK != mdrv_udi_ioctl(g_ulCbtPortUDIHandle, ACM_IOCTL_SET_WRITE_CB, pWriteCB))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, mdrv_udi_ioctl ACM_IOCTL_SET_WRITE_CB Failed\r\n");

        return;
    }

    if(VOS_OK != mdrv_udi_ioctl(g_ulCbtPortUDIHandle, ACM_IOCTL_SET_EVT_CB, pStateCB))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, mdrv_udi_ioctl ACM_IOCTL_SET_EVT_CB Failed\r\n");

        return;
    }

    if (VOS_OK != mdrv_udi_ioctl(g_ulCbtPortUDIHandle, ACM_IOCTL_WRITE_DO_COPY, VOS_NULL_PTR))
    {
        LogPrint("\r\n CBTPPM_OamCbtPortDataInit, mdrv_udi_ioctl ACM_IOCTL_WRITE_DO_COPY Failed\r\n");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice = OM_GetSlice();

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamUsbCbtPortInit
 功能描述  : 用于USB 上CBT端口通道的初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_OK/VOS_ERR
 修改历史  :
   1.日    期  : 2014年5月25日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 CBTPPM_OamUsbCbtPortInit(VOS_VOID)
{
    /* CBT端口与PCVOICE复用，动态注册数据接收函数 */
    CBTCPM_PortRcvReg(CBTSCM_SoftDecodeDataRcv);

    /* 动态注册数据发送函数 */
    CBTCPM_PortSndReg(CBTPPM_OamUsbCbtSendData);

    /* USB承载的CBT端口，调用底软的异步接口发送数据，现在需要伪造成同步接口，申请信号量 */
    if(VOS_OK != VOS_SmCCreate("UCBT", 0, VOS_SEMA4_FIFO, &g_ulCbtUsbPseudoSyncSemId))
    {
        vos_printf("\r\n CBTPPM_OamUsbCbtPortInit: create g_ulCbtUsbPseudoSyncSemId failedd\r\n");

        g_stCbtPpmDebugInfo.ulCbtPortSemErr++;

        return VOS_ERR;
    }

    /* USB承载的CBT端口初始化UDI设备句柄 */
    g_ulCbtPortUDIHandle = VOS_ERROR;

    /* CBT端口通过CBT的CPM管理，不注册物理发送函数 */
    mdrv_usb_reg_enablecb((USB_UDI_ENABLE_CB_T)CBTPPM_OamUsbCbtPortOpen);

    vos_printf("\r\nCBTPPM_OamUsbCbtPortInit mdrv_usb_reg_enablecb: reg %p", CBTPPM_OamUsbCbtPortOpen);

    g_stCbtPpmDebugInfo.ulCbtPortRegOpen++;

    mdrv_usb_reg_disablecb((USB_UDI_DISABLE_CB_T)CBTPPM_OamUsbCbtPortClose);

    vos_printf("\r\nCBTPPM_OamUsbCbtPortInit mdrv_usb_reg_disablecb: reg %p", CBTPPM_OamUsbCbtPortClose);

    g_stCbtPpmDebugInfo.ulCbtPortRegClose++;

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamVComCbtReadData
 功能描述  : 从VCOM端口读取CBT的数据
 输入参数  :  ucDevIndex: 物理端口
              pData    : 收到数据
              ullength : 数据长度

 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 修改历史  :
   1.日    期  : 2014年5月26日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_INT CBTPPM_OamVComCbtReadData(VOS_UINT8 ucDevIndex, VOS_UINT8 *pData, VOS_UINT32 ullength)
{
    CBTCPM_RCV_FUNC                     pFunc;

    if (ucDevIndex != APP_VCOM_DEV_INDEX_LOG)
    {
        vos_printf("\r\n CBTPPM_OamVComCbtReadData:PhyPort port is error: %d\n", ucDevIndex);

        return VOS_ERR;
    }

    g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMRcvNum++;
    g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMRcvLen += ullength;

    if ((VOS_NULL_PTR == pData) || (0 == ullength))
    {
        vos_printf("\r\n CBTPPM_OamVComCbtReadData:Send data is NULL\n");

        return VOS_ERR;
    }

    /*lint -e40*/
    OM_ACPU_DEBUG_TRACE((VOS_UINT8*)pData, ullength, OM_ACPU_VCOM_CB);
    /*lint +e40*/

    pFunc = CBTCPM_GetRcvFunc();

    /* 数据接收函数 */
    if (VOS_NULL_PTR == pFunc)
    {
        return VOS_ERR;
    }

    if (VOS_OK != pFunc((VOS_UINT8*)pData, ullength))
    {
        g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMRcvErrNum++;
        g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMRcvErrLen += ullength;

        vos_printf("\r\n Info: CBTPPM_OamVComCbtReadData:Call CBT Data Rcv Func fail\n");

        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamVComCbtSendData
 功能描述  : 从VCOM端口发送CBT数据
 输入参数  : pucVirAddr:   数据虚地址
             pucPhyAddr:   数据实地址
             ulDataLen:    数据长度
 输出参数  : 无
 返 回 值  : CPM_SEND_ERR/CPM_SEND_OK
 修改历史  :
   1.日    期  : 2014年5月26日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 CBTPPM_OamVComCbtSendData(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMSendNum++;
    g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMSendLen += ulDataLen;

    if (VOS_OK != APP_VCOM_Send(APP_VCOM_DEV_INDEX_LOG, pucVirAddr, ulDataLen))
    {
        g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMSendErrNum++;
        g_stVComDebugInfo[OM_LOGIC_CHANNEL_CBT ].ulVCOMSendErrLen += ulDataLen;

        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamVComCbtPortInit
 功能描述  : 用于 Vcom CBT端口的初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID CBTPPM_OamVComCbtPortInit(VOS_VOID)
{
    /* CBT端口与PCVOICE复用，动态注册数据接收函数 */
    CBTCPM_PortRcvReg(CBTSCM_SoftDecodeDataRcv);

    /* 校准走VCOM28，会有数据下发，数据的回复不走CPM，直接发送 */
    APP_VCOM_RegDataCallback(APP_VCOM_DEV_INDEX_LOG, CBTPPM_OamVComCbtReadData);

    CBTCPM_PortSndReg(CBTPPM_OamVComCbtSendData);

    return;
}

/*****************************************************************************
 函 数 名  : CBTPPM_OamCbtPortInit
 功能描述  : CBT端口初始化
 输入参数  : enCbtPort:CBT物理端口类型
 输出参数  : 无
 返 回 值  : VOS_VOID
 修改历史  :
  1.日    期   : 2014年5月25日
    作    者   : h59254
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID CBTPPM_OamCbtPortInit(VOS_VOID)
{
    OM_CHANNLE_PORT_CFG_STRU            stPortCfg;

    /* 读取OM的物理输出通道 */
    if (NV_OK != NV_Read(en_NV_Item_Om_Port_Type, &stPortCfg, sizeof(OM_CHANNLE_PORT_CFG_STRU)))
    {
        stPortCfg.enCbtPortNum = CPM_CBT_PORT_VCOM;
    }

    if (CPM_CBT_PORT_USB == stPortCfg.enCbtPortNum)
    {
        /* USB 承载的CBT端口的初始化 */
        CBTPPM_OamUsbCbtPortInit();
    }
    /* 默认都走VCOM做CBT */
    else
    {
        /* Vcom 口CBT通道的初始化 */
        CBTPPM_OamVComCbtPortInit();
    }

    return;
}

/*****************************************************************************
 函 数 名  : CBTPPM_UsbDebugInfoShow
 功能描述  : USB承载的CBT端口通道的调试信息输出函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
  1.日    期   : 2015年3月20日
    作    者   : H00300778
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID CBTPPM_UsbDebugInfoShow(VOS_VOID)
{
    vos_printf("\r\nCBTPPM_UsbDebugInfoShow:\r\n");

    vos_printf("USB CBT Port Sem Err Times: %d\r\n",        g_stCbtPpmDebugInfo.ulCbtPortSemErr);
    vos_printf("USB CBT Port Reg Open CB Times: %d\r\n",    g_stCbtPpmDebugInfo.ulCbtPortRegOpen);
    vos_printf("USB CBT Port Reg Close CB Times: %d\r\n",   g_stCbtPpmDebugInfo.ulCbtPortRegClose);

    return;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif




