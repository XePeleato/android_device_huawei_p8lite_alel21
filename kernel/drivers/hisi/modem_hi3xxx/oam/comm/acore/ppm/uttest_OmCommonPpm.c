/* 头文件包含 */
#include "OmCommonPpm.h"
#include "cpm.h"
#include "OmUsbPpm.h"
#include "omprivate.h"

extern OM_CHANNLE_PORT_CFG_STRU                g_stPortCfg;

extern OM_ACPU_DEBUG_INFO                      g_stAcpuDebugInfo;

extern VOS_UINT32                              g_ulUSBSendErrCnt;

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_DisconnectGUPort_case1(VOS_VOID)
{
    return PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_CNF);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_DisconnectGUPort_case2(VOS_VOID)
{
    return PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_CNF);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_DisconnectTLPort_case1(VOS_VOID)
{
    return PPM_DisconnectTLPort();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_DisconnectTLPort_case2(VOS_VOID)
{
    return PPM_DisconnectTLPort();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_DisconnectAllPort_case1(VOS_VOID)
{
    return PPM_DisconnectAllPort(OM_LOGIC_CHANNEL_CNF);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_GetSendDataLen_case1(VOS_VOID)
{
    VOS_UINT32                          ulSendDataLen;
    CPM_PHY_PORT_ENUM_UINT32            enPhyport;

    PPM_GetSendDataLen(SOCP_CODER_DST_OM_IND, 60*1024 + 1, &ulSendDataLen, &enPhyport);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_GetSendDataLen_case2(VOS_VOID)
{
    VOS_UINT32                          ulSendDataLen;
    CPM_PHY_PORT_ENUM_UINT32            enPhyport;

    PPM_GetSendDataLen(SOCP_CODER_DST_OM_IND, 4000, &ulSendDataLen, &enPhyport);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortStatus_case1(VOS_VOID)
{
    PPM_PortStatus(OM_USB_CFG_PORT_HANDLE, CPM_CFG_PORT, ACM_EVT_DEV_SUSPEND);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortStatus_case2(VOS_VOID)
{
    PPM_PortStatus(OM_USB_IND_PORT_HANDLE, CPM_IND_PORT, ACM_EVT_DEV_SUSPEND);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortStatus_case3(VOS_VOID)
{
    PPM_PortStatus(OM_USB_CBT_PORT_HANDLE, CPM_IND_PORT, ACM_EVT_DEV_SUSPEND);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortStatus_case4(VOS_VOID)
{
    PPM_PortStatus(OM_USB_IND_PORT_HANDLE, CPM_IND_PORT, ACM_EVT_DEV_READY);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortStatus_case5(VOS_VOID)
{
    PPM_PortStatus(OM_USB_IND_PORT_HANDLE, CPM_IND_PORT, ACM_EVT_DEV_BOTTOM);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortCloseProc_case1(VOS_VOID)
{
    g_astOMPortUDIHandle[OM_USB_CFG_PORT_HANDLE] = VOS_ERROR;

    PPM_PortCloseProc(OM_USB_CFG_PORT_HANDLE, ACM_EVT_DEV_SUSPEND);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortCloseProc_case2(VOS_VOID)
{
    g_astOMPortUDIHandle[OM_USB_CFG_PORT_HANDLE] = VOS_OK;

    PPM_PortCloseProc(OM_USB_CFG_PORT_HANDLE, CPM_CFG_PORT);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortCloseProc_case3(VOS_VOID)
{
    g_astOMPortUDIHandle[OM_USB_IND_PORT_HANDLE] = VOS_OK;

    PPM_PortCloseProc(OM_USB_IND_PORT_HANDLE, CPM_IND_PORT);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortCloseProc_case4(VOS_VOID)
{
    g_astOMPortUDIHandle[OM_USB_IND_PORT_HANDLE] = VOS_OK;

    PPM_PortCloseProc(OM_USB_CBT_PORT_HANDLE, CPM_IND_PORT);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_INT32 uttest_PPM_ReadPortData_case1(VOS_VOID)
{
    return PPM_ReadPortData(CPM_IND_PORT, VOS_ERROR, OM_USB_CFG_PORT_HANDLE);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_INT32 uttest_PPM_ReadPortData_case2(VOS_VOID)
{
    return PPM_ReadPortData(CPM_IND_PORT, 2, OM_USB_CFG_PORT_HANDLE);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_INT32 uttest_PPM_ReadPortData_case3(VOS_VOID)
{
    return PPM_ReadPortData(CPM_IND_PORT, 2, OM_USB_CFG_PORT_HANDLE);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortPseudoSyncGetSmp_case1(VOS_VOID)
{
    PPM_PortPseudoSyncGetSmp(OM_USB_IND_PORT_HANDLE);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortPseudoSyncGetSmp_case2(VOS_VOID)
{
    PPM_PortPseudoSyncGetSmp(OM_USB_CFG_PORT_HANDLE);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortPseudoSyncGetSmp_case3(VOS_VOID)
{
    PPM_PortPseudoSyncGetSmp(OM_USB_CBT_PORT_HANDLE);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_QueryLogPort_case1(VOS_VOID)
{
    VOS_UINT32                          ulLogPort;

    return PPM_QueryLogPort(VOS_NULL_PTR);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_QueryLogPort_case2(VOS_VOID)
{
    VOS_UINT32                          ulLogPort;

    g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_WIFI;

    return PPM_QueryLogPort(&ulLogPort);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_QueryLogPort_case3(VOS_VOID)
{
    VOS_UINT32                          ulLogPort;

    g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_VCOM;

    return PPM_QueryLogPort(&ulLogPort);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_LogPortSwitch_case1(VOS_VOID)
{
    return PPM_LogPortSwitch(CPM_OM_PORT_TYPE_WIFI, VOS_FALSE);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_LogPortSwitch_case2(VOS_VOID)
{
    g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_VCOM;

    return PPM_LogPortSwitch(CPM_OM_PORT_TYPE_VCOM, VOS_FALSE);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_LogPortSwitch_case3(VOS_VOID)
{
    g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_USB;

    return PPM_LogPortSwitch(CPM_OM_PORT_TYPE_VCOM, VOS_FALSE);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_LogPortSwitch_case4(VOS_VOID)
{
    g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_VCOM;

    return PPM_LogPortSwitch(CPM_OM_PORT_TYPE_USB, VOS_TRUE);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_UdiRegCallBackFun_case1(VOS_VOID)
{
    return PPM_UdiRegCallBackFun(1, 2, VOS_NULL_PTR);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_UdiRegCallBackFun_case2(VOS_VOID)
{
    return PPM_UdiRegCallBackFun(1, 2, PPM_UsbIndStatusCB);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_UdiRegCallBackFun_case3(VOS_VOID)
{
    return PPM_UdiRegCallBackFun(1, 2, PPM_UsbIndStatusCB);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case1(VOS_VOID)
{
    PPM_ReadPortDataInit(CPM_FS_PORT, OM_HSIC_CFG_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case2(VOS_VOID)
{
    g_astOMPortUDIHandle[OM_USB_IND_PORT_HANDLE] = 2;

    PPM_ReadPortDataInit(CPM_IND_PORT, OM_USB_IND_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case3(VOS_VOID)
{
    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    PPM_ReadPortDataInit(CPM_CFG_PORT, OM_USB_CFG_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case4(VOS_VOID)
{
    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    PPM_ReadPortDataInit(CPM_HSIC_IND_PORT, OM_HSIC_IND_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case5(VOS_VOID)
{
    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    PPM_ReadPortDataInit(CPM_HSIC_CFG_PORT, OM_HSIC_CFG_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case6(VOS_VOID)
{
    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    PPM_ReadPortDataInit(CPM_HSIC_CFG_PORT, OM_HSIC_CFG_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case7(VOS_VOID)
{
    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    PPM_ReadPortDataInit(CPM_HSIC_CFG_PORT, OM_HSIC_CFG_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case8(VOS_VOID)
{
    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    PPM_ReadPortDataInit(CPM_HSIC_CFG_PORT, OM_HSIC_CFG_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case9(VOS_VOID)
{
    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    PPM_ReadPortDataInit(CPM_HSIC_CFG_PORT, OM_HSIC_CFG_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_ReadPortDataInit_case10(VOS_VOID)
{
    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    PPM_ReadPortDataInit(CPM_HSIC_CFG_PORT, OM_HSIC_CFG_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, VOS_NULL_PTR);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_PortSend_case1(VOS_VOID)
{
    return PPM_PortSend(OM_HSIC_CFG_PORT_HANDLE, VOS_NULL_PTR, VOS_NULL_PTR, 1000);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_PortSend_case2(VOS_VOID)
{
    VOS_UINT8                           aucVirAddr[100];
    VOS_UINT8                           aucPhyAddr[100];

    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    return PPM_PortSend(OM_HSIC_CFG_PORT_HANDLE, aucVirAddr, aucPhyAddr, 1000);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_PortSend_case3(VOS_VOID)
{
    VOS_UINT8                           aucVirAddr[100];
    VOS_UINT8                           aucPhyAddr[100];

    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    memset(&g_stAcpuDebugInfo, 0, sizeof(g_stAcpuDebugInfo));

    g_stAcpuDebugInfo.astPortInfo[OM_HSIC_CFG_PORT_HANDLE].ulUSBWriteMaxTime = 500;

    g_astOMPortUDIHandle[OM_HSIC_CFG_PORT_HANDLE] = 2;

    return PPM_PortSend(OM_HSIC_CFG_PORT_HANDLE, aucVirAddr, aucPhyAddr, 1000);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_PortSend_case4(VOS_VOID)
{
    VOS_UINT8                           aucVirAddr[100];
    VOS_UINT8                           aucPhyAddr[100];

    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    memset(&g_stAcpuDebugInfo, 0, sizeof(g_stAcpuDebugInfo));

    g_stAcpuDebugInfo.astPortInfo[OM_HSIC_CFG_PORT_HANDLE].ulUSBWriteMaxTime = 500;

    g_astOMPortUDIHandle[OM_HSIC_CFG_PORT_HANDLE] = 2;

    return PPM_PortSend(OM_HSIC_CFG_PORT_HANDLE, aucVirAddr, aucPhyAddr, 1000);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_PortSend_case5(VOS_VOID)
{
    VOS_UINT8                           aucVirAddr[100];
    VOS_UINT8                           aucPhyAddr[100];

    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    memset(&g_stAcpuDebugInfo, 0, sizeof(g_stAcpuDebugInfo));

    g_ulUSBSendErrCnt = 0;

    g_stAcpuDebugInfo.astPortInfo[OM_HSIC_CFG_PORT_HANDLE].ulUSBWriteMaxTime = 500;

    g_astOMPortUDIHandle[OM_HSIC_CFG_PORT_HANDLE] = 2;

    return PPM_PortSend(OM_HSIC_CFG_PORT_HANDLE, aucVirAddr, aucPhyAddr, 1000);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_PortSend_case6(VOS_VOID)
{
    VOS_UINT8                           aucVirAddr[100];
    VOS_UINT8                           aucPhyAddr[100];

    memset(&g_astOMPortUDIHandle, VOS_ERROR, OM_PORT_HANDLE_BUTT * sizeof(UDI_HANDLE));

    memset(&g_stAcpuDebugInfo, 0, sizeof(g_stAcpuDebugInfo));

    g_ulUSBSendErrCnt = 0;

    g_stAcpuDebugInfo.astPortInfo[OM_HSIC_CFG_PORT_HANDLE].ulUSBWriteMaxTime = 500;

    g_astOMPortUDIHandle[OM_HSIC_CFG_PORT_HANDLE] = 2;

    return PPM_PortSend(OM_HSIC_CFG_PORT_HANDLE, aucVirAddr, aucPhyAddr, 1000);
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortWriteAsyCB_case1(VOS_VOID)
{
    VOS_CHAR                            aucData[100];

    PPM_PortWriteAsyCB(OM_USB_IND_PORT_HANDLE, aucData, -10);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortWriteAsyCB_case2(VOS_VOID)
{
    VOS_CHAR                            aucData[100];

    PPM_PortWriteAsyCB(OM_USB_CFG_PORT_HANDLE, aucData, sizeof(aucData));

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortWriteAsyCB_case3(VOS_VOID)
{
    VOS_CHAR                            aucData[100];

    PPM_PortWriteAsyCB(OM_HSIC_IND_PORT_HANDLE, aucData, sizeof(aucData));

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_InitPhyPort_case1(VOS_VOID)
{
    return PPM_InitPhyPort();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_InitPhyPort_case2(VOS_VOID)
{
    return PPM_InitPhyPort();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_InitPhyPort_case3(VOS_VOID)
{
    return PPM_InitPhyPort();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_UINT32 uttest_PPM_PortInit_case1(VOS_VOID)
{
    return PPM_PortInit();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_OmPortInfoShow_case1(VOS_VOID)
{
    PPM_OmPortInfoShow(OM_USB_IND_PORT_HANDLE);

    return;
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
VOS_VOID uttest_PPM_PortSwitchInfoShow_case1(VOS_VOID)
{
    PPM_PortSwitchInfoShow();

    return;
}


