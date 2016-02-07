/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : OmUsbPpm.c
  版 本 号   : 初稿
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   : OM的USB物理端口管理模块
  函数列表   :
  修改历史   :
  1.日    期   : 2014年5月31日
    作    者   : L00256032
    修改内容   : V8R1 OM_Optimize项目新增

***************************************************************************** */

/*****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "SCMProc.h"
#include "ombufmngr.h"
#include "OmCommonPpm.h"
#include "cpm.h"
#include "OmUsbPpm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* lint -e767  */
#define    THIS_FILE_ID        PS_FILE_ID_OM_USB_PPM_C
/* lint +e767  */

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
/* 端口调试信息的数据结构体 */
PPM_PORT_DEBUG_INFO_STUR                g_stPpmDebugInfo;
/*****************************************************************************
  3 外部引用声明
*****************************************************************************/


/*****************************************************************************
  4 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  :
 功能描述  :
 输入参数  :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : XXXXXXXX
    修改内容   : V8R1 OM_Optimize项目新增

*****************************************************************************/

/*****************************************************************************
 函 数 名  : PPM_UsbCfgSendData
 功能描述  : 将输入的数据通过USB(CFG口)发送给PC侧
 输入参数  : pucVirAddr:   数据虚地址
             pucPhyAddr:   数据实地址
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : VOS_ERROR/VOS_OK
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_UsbCfgSendData(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    return PPM_PortSend(OM_USB_CFG_PORT_HANDLE, pucVirAddr, pucPhyAddr, ulDataLen);
}


/*****************************************************************************
 函 数 名  : PPM_UsbCfgPortClose
 功能描述  : USB承载的OM CFG端口已经消失，需要关闭CFG端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID PPM_UsbCfgPortClose(VOS_VOID)
{
    PPM_PortCloseProc(OM_USB_CFG_PORT_HANDLE, CPM_CFG_PORT);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamUsbCfgStatusCB
 功能描述  : 用于ACPU上面处理USB物理端口断开后的OM链接断开
 输入参数  : enPortState:端口状态
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID PPM_UsbCfgStatusCB(ACM_EVT_E enPortState)
{
    PPM_PortStatus(OM_USB_CFG_PORT_HANDLE, CPM_CFG_PORT, enPortState);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamUsbCfgWriteDataCB
 功能描述  : 用于处理USB承载的OM CFG端口的异步发送数据的回调
 输入参数  : pucData:   需要发送的数据内容
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID PPM_UsbCfgWriteDataCB(VOS_UINT8* pucVirData, VOS_UINT8* pucPhyData, VOS_INT lLen)
{
    PPM_PortWriteAsyCB(OM_USB_CFG_PORT_HANDLE, pucVirData, lLen);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamUsbCfgReadDataCB
 功能描述  : 用于ACPU上面底软把USB承载的OM CFG端口数据通过ICC发送给OM模块
 输入参数  : enPhyPort: 物理端口
             UdiHandle:设备句柄
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_INT32 PPM_UsbCfgReadDataCB(VOS_VOID)
{
    return PPM_ReadPortData(CPM_CFG_PORT, g_astOMPortUDIHandle[OM_USB_CFG_PORT_HANDLE], OM_USB_CFG_PORT_HANDLE);
}

/*****************************************************************************
 函 数 名  : GU_OamUsbCfgPortOpen
 功能描述  : 用于初始化USB承载的OM CFG端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID PPM_UsbCfgPortOpen(VOS_VOID)
{
    PPM_ReadPortDataInit(CPM_CFG_PORT,
                            OM_USB_CFG_PORT_HANDLE,
                            PPM_UsbCfgReadDataCB,
                            PPM_UsbCfgWriteDataCB,
                            PPM_UsbCfgStatusCB);

    return;
}

/*****************************************************************************
 函 数 名  : PPM_UsbIndStatusCB
 功能描述  : 用于ACPU上面处理USB物理端口断开后的OM链接断开
 输入参数  : enPortState:   端口状态
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID PPM_UsbIndStatusCB(ACM_EVT_E enPortState)
{
    PPM_PortStatus(OM_USB_IND_PORT_HANDLE, CPM_IND_PORT, enPortState);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamUsbIndWriteDataCB
 功能描述  : 用于处理USB OM主动上报端口的异步发送数据的回调
 输入参数  : pucData:   需要发送的数据内容
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID PPM_UsbIndWriteDataCB(VOS_UINT8* pucVirData, VOS_UINT8* pucPhyData, VOS_INT lLen)
{
    PPM_PortWriteAsyCB(OM_USB_IND_PORT_HANDLE, pucVirData, lLen);

    return;
}

/*****************************************************************************
 函 数 名  : GU_OamUsbIndPortOpen
 功能描述  : 用于初始化USB承载的OM主动上报端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID PPM_UsbIndPortOpen(VOS_VOID)
{
    PPM_ReadPortDataInit(CPM_IND_PORT,
                            OM_USB_IND_PORT_HANDLE,
                            VOS_NULL_PTR,
                            PPM_UsbIndWriteDataCB,
                            PPM_UsbIndStatusCB);

    return;
}

/*****************************************************************************
 函 数 名  : PPM_UsbIndPortClose
 功能描述  : USB承载的OM主动上报端口已经消失，需要关闭USB端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID PPM_UsbIndPortClose(VOS_VOID)
{
    PPM_PortCloseProc(OM_USB_IND_PORT_HANDLE, CPM_IND_PORT);

    return;
}

/*****************************************************************************
 函 数 名  : PPM_UsbIndSendData
 功能描述  : 将输入的数据通过USB主动上报端口发送给PC侧
 输入参数  : pucVirAddr:   数据虚地址
             pucPhyAddr:   数据实地址
             ulDataLen: 数据长度
 输出参数  : 无
 返 回 值  : VOS_ERROR/VOS_OK
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_UsbIndSendData(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    return PPM_PortSend(OM_USB_IND_PORT_HANDLE, pucVirAddr, pucPhyAddr, ulDataLen);
}

/*****************************************************************************
 函 数 名  : PPM_UsbCfgPortInit
 功能描述  : 用于USB上OM配置端口通道的初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR -- 初始化失败
             VOS_OK  -- 初始化成功
 修改历史  :
   1.日    期  : 2014年5月24日
     作    者  : h59254
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_UsbCfgPortInit(VOS_VOID)
{
    /* USB承载的OM CFG端口，调用底软的异步接口发送数据，现在需要伪造成同步接口，申请信号量 */
    if(VOS_OK != VOS_SmCCreate("UCFG", 0, VOS_SEMA4_FIFO, &g_stUsbCfgPseudoSync.ulPseudoSyncSemId))
    {
        vos_printf("\r\nPPM_UsbCfgPortInit: create g_stUsbCnfPseudoSync.ulPseudoSyncSemId failedd\r\n");

        g_stPpmDebugInfo.ulUsbCnfSemErr++;

        return VOS_ERR;
    }

    mdrv_usb_reg_enablecb((USB_UDI_ENABLE_CB_T)PPM_UsbCfgPortOpen);

    vos_printf("\r\nPPM_UsbCfgPortInit mdrv_usb_reg_enablecb: reg %p", PPM_UsbCfgPortOpen);

    g_stPpmDebugInfo.ulUsbCnfRegOpen++;

    mdrv_usb_reg_disablecb((USB_UDI_DISABLE_CB_T)PPM_UsbCfgPortClose);

    vos_printf("\r\nPPM_UsbCfgPortInit mdrv_usb_reg_disablecb: reg %p", PPM_UsbCfgPortClose);

    g_stPpmDebugInfo.ulUsbCnfRegClose++;

    CPM_PhySendReg(CPM_CFG_PORT,  PPM_UsbCfgSendData);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : PPM_UsbIndPortInit
 功能描述  : 用于USB 上OM主动上报端口通道的初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR -- 初始化失败
             VOS_OK  -- 初始化成功
 修改历史  :
  1.日    期   : 2014年5月25日
    作    者   : L00256032
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_UINT32 PPM_UsbIndPortInit(VOS_VOID)
{
    /* USB承载的OM IND端口，调用底软的异步接口发送数据，现在需要伪造成同步接口，申请信号量 */
    if(VOS_OK != VOS_SmCCreate("UIND", 0, VOS_SEMA4_FIFO, &g_stUsbIndPseudoSync.ulPseudoSyncSemId))
    {
        vos_printf("\r\nPPM_UsbIndPortInit: create g_stUsbIndPseudoSync.ulPseudoSyncSemId failedd\r\n");

        g_stPpmDebugInfo.ulUsbIndSemErr++;

        return VOS_ERR;
    }

    mdrv_usb_reg_enablecb((USB_UDI_ENABLE_CB_T)PPM_UsbIndPortOpen);

    vos_printf("\r\nPPM_UsbIndPortInit mdrv_usb_reg_enablecb: reg %p", PPM_UsbIndPortOpen);

    g_stPpmDebugInfo.ulUsbIndRegOpen++;

    mdrv_usb_reg_disablecb((USB_UDI_DISABLE_CB_T)PPM_UsbIndPortClose);

    vos_printf("\r\nPPM_UsbIndPortInit mdrv_usb_reg_disablecb: reg %p", PPM_UsbIndPortClose);

    g_stPpmDebugInfo.ulUsbIndRegClose++;

    CPM_PhySendReg(CPM_IND_PORT,  PPM_UsbIndSendData);

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : PPM_UsbPortInit
 功能描述  : USB承载的虚拟端口通道的初始化:包括OM IND、OM CNF等端口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_OK   - 初始化成功
             VOS_ERR  - 初始化失败
 修改历史  :
  1.日    期   : 2014年5月25日
    作    者   : L00256032
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_UINT32 PPM_UsbPortInit(VOS_VOID)
{
    /* USB 承载的OM主动上报端口的初始化 */
    if (VOS_OK != PPM_UsbIndPortInit())
    {
        return VOS_ERR;
    }

    /* USB 承载的OM配置端口的初始化 */
    if (VOS_OK != PPM_UsbCfgPortInit())
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : PPM_UsbDebugInfoShow
 功能描述  : USB承载的虚拟端口通道的调试信息输出函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
  1.日    期   : 2015年3月20日
    作    者   : H00300778
    修改内容   : V8R1 OM_Optimize项目新增
*****************************************************************************/
VOS_VOID PPM_UsbDebugInfoShow(VOS_VOID)
{
    vos_printf("\r\nPPM_UsbDebugInfoShow:\r\n");

    vos_printf("USB Cnf Sem Err Times: %d\r\n",         g_stPpmDebugInfo.ulUsbCnfSemErr);
    vos_printf("USB Cnf Reg Open CB Times: %d\r\n",     g_stPpmDebugInfo.ulUsbCnfRegOpen);
    vos_printf("USB Cnf Reg Close CB Times: %d\r\n",    g_stPpmDebugInfo.ulUsbCnfRegClose);

    vos_printf("USB Ind Sem Err Times: %d\r\n",         g_stPpmDebugInfo.ulUsbIndSemErr);
    vos_printf("USB Ind Reg Open CB Times: %d\r\n",     g_stPpmDebugInfo.ulUsbIndRegOpen);
    vos_printf("USB Ind Reg Close CB Times: %d\r\n",    g_stPpmDebugInfo.ulUsbIndRegClose);

    return;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif




