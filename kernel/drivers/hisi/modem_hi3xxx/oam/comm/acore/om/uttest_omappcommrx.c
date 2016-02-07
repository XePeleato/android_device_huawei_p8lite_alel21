/* 头文件包含 */
#include "OmAppRl.h"
#include "SOCPInterface.h"
#include "om.h"
#include "omrl.h"
#include "OmCommonPpm.h"


extern OMRL_RCV_CHAN_CTRL_INFO_STRU            g_stAcpuCnfCtrlInfo;;
/*******************************************************************
*测试项: GU_OamSndPcMsgToCcpu
*被测函数功能描述:C核复位写ICC失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_GU_OamSndPcMsgToCcpu_case1(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU stCtrlInfo;
    SOCP_CODER_DST_ENUM_U32 enChanID = SOCP_CODER_DST_OM_CNF;
    VOS_UINT8 aucData[USB_MAX_DATA_LEN + 1] = {0};
    VOS_UINT32 ulDataLen = USB_MAX_DATA_LEN + 1;

    return GU_OamSndPcMsgToCcpu(&stCtrlInfo, aucData, ulDataLen);
}

/*******************************************************************
*测试项: GU_OamSndPcMsgToCcpu
*被测函数功能描述:C核复位写ICC失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_GU_OamSndPcMsgToCcpu_case2(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU stCtrlInfo;
    SOCP_CODER_DST_ENUM_U32 enChanID = SOCP_CODER_DST_OM_CNF;
    VOS_UINT8 aucData[USB_MAX_DATA_LEN + 1] = {0};
    VOS_UINT32 ulDataLen = USB_MAX_DATA_LEN + 1;

    return GU_OamSndPcMsgToCcpu(&stCtrlInfo, aucData, ulDataLen);
}

/*******************************************************************
*测试项: GU_OamSndPcMsgToCcpu
*被测函数功能描述:C核复位写ICC失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_GU_OamSndPcMsgToCcpu_case3(VOS_VOID)
{
    OMRL_RCV_CHAN_CTRL_INFO_STRU stCtrlInfo;
    SOCP_CODER_DST_ENUM_U32 enChanID = SOCP_CODER_DST_OM_CNF;
    VOS_UINT8 aucData[USB_MAX_DATA_LEN + 1] = {0};
    VOS_UINT32 ulDataLen = USB_MAX_DATA_LEN + 1;

    return GU_OamSndPcMsgToCcpu(&stCtrlInfo, aucData, ulDataLen);
}

/*******************************************************************
*测试项: GU_OamErrLogVComPortInit
*被测函数功能描述:ErrLog Vcom 口通道的初始化
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_GU_OamErrLogVComPortInit_case1(VOS_VOID)
{
    GU_OamErrLogVComPortInit();
}

/*******************************************************************
*测试项: GU_OamAcpu_ICCError_CB
*被测函数功能描述:OM ICC通道
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT uttest_GU_OamAcpu_ICCError_CB_case1(VOS_VOID)
{
    VOS_UINT ulChanID;
    VOS_UINT ulEvent = 0;
    VOS_VOID* pParam = VOS_NULL_PTR;

    ulChanID = UDI_ICC_GUOM0;

    return GU_OamAcpu_ICCError_CB(ulChanID, ulEvent, pParam);
}

/*******************************************************************
*测试项: GU_OamAcpu_ICCError_CB
*被测函数功能描述:OSA MSG ICC通道
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT uttest_GU_OamAcpu_ICCError_CB_case2(VOS_VOID)
{
    VOS_UINT ulChanID;
    VOS_UINT ulEvent = 0;
    VOS_VOID* pParam = VOS_NULL_PTR;

    ulChanID = UDI_ICC_GUOM4;

    return GU_OamAcpu_ICCError_CB(ulChanID, ulEvent, pParam);
}

/*******************************************************************
*测试项: GU_OamAcpu_ICCError_CB
*被测函数功能描述:通道错误
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT uttest_GU_OamAcpu_ICCError_CB_case3(VOS_VOID)
{
    VOS_UINT ulChanID;
    VOS_UINT ulEvent = 0;
    VOS_VOID* pParam = VOS_NULL_PTR;

    ulChanID = UDI_ICC_GUOM2;

    return GU_OamAcpu_ICCError_CB(ulChanID, ulEvent, pParam);
}

/*******************************************************************
*测试项: OM_ComRx_ICC_Init
*被测函数功能描述:申请内存失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_ComRx_ICC_Init_case1(VOS_VOID)
{
    return OM_ComRx_ICC_Init();
}

/*******************************************************************
*测试项: OM_ComRx_ICC_Init
*被测函数功能描述:打开端口失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_ComRx_ICC_Init_case2(VOS_VOID)
{
    return OM_ComRx_ICC_Init();
}

/*******************************************************************
*测试项: OM_ComRx_ICC_Init
*被测函数功能描述:打开端口失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_ComRx_ICC_Init_case3(VOS_VOID)
{
    return OM_ComRx_ICC_Init();
}

/*******************************************************************
*测试项: GU_OmAcpuSwitchOnOff
*被测函数功能描述:获取ulOMSwitchOnOff标记
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_GU_OmAcpuSwitchOnOff_GetFlag(VOS_VOID)
{
    return g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff;
}

/*******************************************************************
*测试项: GU_OmAcpuSwitchOnOff
*被测函数功能描述:设置ulOMSwitchOnOff标记
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_GU_OmAcpuSwitchOnOff_case1(VOS_VOID)
{
    VOS_UINT32 ulFlag;

    ulFlag = OM_STATE_IDLE;

    g_stAcpuCnfCtrlInfo.ulOMSwitchOnOff = OM_STATE_ACTIVE;

    GU_OmAcpuSwitchOnOff(ulFlag);
}

/*******************************************************************
*测试项: OM_AcpuLogShowToFile
*被测函数功能描述:打开文件失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuLogShowToFile_case1(VOS_VOID)
{
    VOS_BOOL bIsSendMsg = VOS_FALSE;

    OM_AcpuLogShowToFile(bIsSendMsg);
}

/*******************************************************************
*测试项: OM_AcpuLogShowToFile
*被测函数功能描述:不发送消息到ACPU
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuLogShowToFile_case2(VOS_VOID)
{
    VOS_BOOL bIsSendMsg = VOS_FALSE;

    OM_AcpuLogShowToFile(bIsSendMsg);
}

/*******************************************************************
*测试项: OM_AcpuLogShowToFile
*被测函数功能描述:申请消息内存失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuLogShowToFile_case3(VOS_VOID)
{
    VOS_BOOL bIsSendMsg = VOS_TRUE;

    OM_AcpuLogShowToFile(bIsSendMsg);
}

/*******************************************************************
*测试项: OM_AcpuLogShowToFile
*被测函数功能描述:申请消息内存失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuLogShowToFile_case4(VOS_VOID)
{
    VOS_BOOL bIsSendMsg = VOS_TRUE;

    OM_AcpuLogShowToFile(bIsSendMsg);
}

/*******************************************************************
*测试项: OmAcpuPortInfoShow
*被测函数功能描述:端口信息可维可测打印
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OmAcpuPortInfoShow_case1(VOS_VOID)
{
    OM_PROT_HANDLE_ENUM_UINT32  enHandle = OM_USB_IND_PORT_HANDLE;

    OmAcpuPortInfoShow(enHandle);
}