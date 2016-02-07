/* 头文件包含 */
#include "SCMProc.h"
#include "SOCPInterface.h"
#include "OmCommonPpm.h"
#include "omprivate.h"

extern SCM_CODER_SRC_CFG_STRU      g_astSCMCoderSrcCfg[SCM_CODER_SRC_NUM];

extern OM_PSEUDO_SYNC_STRU         g_stUsbCfgPseudoSync;;

extern VOS_UINT32                  g_ulOmAcpuDbgFlag = VOS_NULL_DWORD;

extern VOS_VOID SCM_SocpSendDataToUDISucc(
    SOCP_CODER_DST_ENUM_U32             enChanID,
    CPM_PHY_PORT_ENUM_UINT32            enPhyport,
    OM_SOCP_CHANNEL_DEBUG_INFO         *pstDebugInfo,
    VOS_UINT32                         *pulSendDataLen
);

/*******************************************************************
*测试项: SCM_CoderSrcChannelCfg            
*被测函数功能描述:获取通道状态
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_CoderSrcChannelCfg_GetenInitState(VOS_UINT8 ucIndex)
{
    return g_astSCMCoderSrcCfg[ucIndex].enInitState;
}

/*******************************************************************
*测试项: SCM_CoderSrcChannelCfg            
*被测函数功能描述:编码源通道配失败
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_CoderSrcChannelCfg_case1(VOS_VOID)
{
    g_astSCMCoderSrcCfg[2].enInitState = SCM_CHANNEL_UNINIT;

    return SCM_CoderSrcChannelCfg(&g_astSCMCoderSrcCfg[2]);
}

/*******************************************************************
*测试项: SCM_CoderSrcChannelCfg            
*被测函数功能描述:编码源通道配成功
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_CoderSrcChannelCfg_case2(VOS_VOID)
{
    g_astSCMCoderSrcCfg[2].enInitState = SCM_CHANNEL_UNINIT;

    return SCM_CoderSrcChannelCfg(&g_astSCMCoderSrcCfg[2]);
}

/*******************************************************************
*测试项: SCM_CoderDstChannelInit
*被测函数功能描述:编码目的通道配置失败
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_CoderDstChannelInit_case1(VOS_VOID)
{
    return SCM_CoderDstChannelInit();
}

/*******************************************************************
*测试项: SCM_CoderDstChannelInit            
*被测函数功能描述:编码目的通道配置成功
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_CoderDstChannelInit_case2(VOS_VOID)
{
    return SCM_CoderDstChannelInit();
}

/*******************************************************************
*测试项: SCM_RegDecoderDestProc            
*被测函数功能描述:注册解码目的回调，但enChanlID异常
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_RegDecoderDestProc_case1(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32       enChanlID;
    SCM_DECODERDESTFUCN             func;

    enChanlID = SOCP_DECODER_DST_BUTT;
    func      = VOS_NULL_PTR;

    return SCM_RegDecoderDestProc(enChanlID, func);
}

/*******************************************************************
*测试项: SCM_RegDecoderDestProc            
*被测函数功能描述:注册解码目的回调，但func为空指针
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_RegDecoderDestProc_case2(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32       enChanlID;
    SCM_DECODERDESTFUCN             func;

    enChanlID = SOCP_DECODER_DST_LOM;
    func      = VOS_NULL_PTR;

    return SCM_RegDecoderDestProc(enChanlID, func);
}

/*******************************************************************
*测试项: SCM_RegDecoderDestProc            
*被测函数功能描述:注册解码目的回调，enChanlID为SOCP_DECODER_DST_LOM
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_RegDecoderDestProc_case3(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32       enChanlID;
    SCM_DECODERDESTFUCN             func;

    enChanlID = SOCP_DECODER_DST_LOM;
    func      = (SCM_DECODERDESTFUCN)1;

    return SCM_RegDecoderDestProc(enChanlID, func);
}

/*******************************************************************
*测试项: SCM_RegDecoderDestProc            
*被测函数功能描述:注册解码目的回调，enChanlID为SOCP_DECODER_DST_GUOM
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_RegDecoderDestProc_case4(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32       enChanlID;
    SCM_DECODERDESTFUCN             func;

    enChanlID = SOCP_DECODER_DST_GUOM;
    func      = (SCM_DECODERDESTFUCN)1;

    return SCM_RegDecoderDestProc(enChanlID, func);
}


/*******************************************************************
*测试项: SCM_RegDecoderDestProc            
*被测函数功能描述:注册解码目的回调，enChanlID为SOCP_DECODER_CBT
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_RegDecoderDestProc_case5(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32       enChanlID;
    SCM_DECODERDESTFUCN             func;

    enChanlID = SOCP_DECODER_CBT;
    func      = (SCM_DECODERDESTFUCN)1;

    return SCM_RegDecoderDestProc(enChanlID, func);
}

/*******************************************************************
*测试项: SCM_RegDecoderDestProc            
*被测函数功能描述:注册解码目的回调，enChanlID错误
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_RegDecoderDestProc_case6(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32       enChanlID;
    SCM_DECODERDESTFUCN             func;

    enChanlID = SOCP_DECODER_DST_RFU;
    func      = (SCM_DECODERDESTFUCN)1;

    return SCM_RegDecoderDestProc(enChanlID, func);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDISucc            
*被测函数功能描述:CNF通道，但len异常
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDISucc_case1(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    CPM_PHY_PORT_ENUM_UINT32            enPhyport;
    OM_SOCP_CHANNEL_DEBUG_INFO         *pstDebugInfo;
    VOS_UINT32                          ulSendDataLen;

    enChanID      = SOCP_CODER_DST_OM_CNF;
    enPhyport     = CPM_CFG_PORT;
    pstDebugInfo  = &g_stAcpuDebugInfo.stCnfDebugInfo;
    ulSendDataLen = 1;

    g_stUsbCfgPseudoSync.ulLen = 0;

    SCM_SocpSendDataToUDISucc(enChanID, enPhyport, pstDebugInfo, &ulSendDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDISucc            
*被测函数功能描述:CNF通道，len正常
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDISucc_case2(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    CPM_PHY_PORT_ENUM_UINT32            enPhyport;
    OM_SOCP_CHANNEL_DEBUG_INFO         *pstDebugInfo;
    VOS_UINT32                          ulSendDataLen;

    enChanID      = SOCP_CODER_DST_OM_CNF;
    enPhyport     = CPM_CFG_PORT;
    pstDebugInfo  = &g_stAcpuDebugInfo.stCnfDebugInfo;
    ulSendDataLen = 1;

    g_stUsbCfgPseudoSync.ulLen = 1;

    SCM_SocpSendDataToUDISucc(enChanID, enPhyport, pstDebugInfo, &ulSendDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDISucc            
*被测函数功能描述:IND通道，但len异常
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDISucc_case3(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    CPM_PHY_PORT_ENUM_UINT32            enPhyport;
    OM_SOCP_CHANNEL_DEBUG_INFO         *pstDebugInfo;
    VOS_UINT32                          ulSendDataLen;

    enChanID      = SOCP_CODER_DST_OM_IND;
    enPhyport     = CPM_IND_PORT;
    pstDebugInfo  = &g_stAcpuDebugInfo.stIndDebugInfo;
    ulSendDataLen = 1;

    g_stUsbCfgPseudoSync.ulLen = 0;

    SCM_SocpSendDataToUDISucc(enChanID, enPhyport, pstDebugInfo, &ulSendDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDISucc            
*被测函数功能描述:IND通道，len正常
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDISucc_case4(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    CPM_PHY_PORT_ENUM_UINT32            enPhyport;
    OM_SOCP_CHANNEL_DEBUG_INFO         *pstDebugInfo;
    VOS_UINT32                          ulSendDataLen;

    enChanID      = SOCP_CODER_DST_OM_IND;
    enPhyport     = CPM_IND_PORT;
    pstDebugInfo  = &g_stAcpuDebugInfo.stIndDebugInfo;
    ulSendDataLen = 1;

    g_stUsbCfgPseudoSync.ulLen = 1;

    SCM_SocpSendDataToUDISucc(enChanID, enPhyport, pstDebugInfo, &ulSendDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDI            
*被测函数功能描述:IND通道，len正常
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDI_case1(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    VOS_UINT8                          *pucVirData;
    VOS_UINT8                          *pucPHYData;
    VOS_UINT32                          ulDataLen;

    enChanID    = SOCP_CODER_DST_BUTT;
    pucVirData  = VOS_NULL_PTR;
    pucPHYData  = VOS_NULL_PTR;
    ulDataLen   = 0;

    SCM_SocpSendDataToUDI(enChanID, pucVirData, pucPHYData, ulDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDI            
*被测函数功能描述:ulDataLen异常
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDI_case2(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    VOS_UINT8                          *pucVirData;
    VOS_UINT8                          *pucPHYData;
    VOS_UINT32                          ulDataLen;

    enChanID    = SOCP_CODER_DST_OM_CNF;
    pucVirData  = (VOS_UINT8 *)1;
    pucPHYData  = (VOS_UINT8 *)1;
    ulDataLen   = 0;

    SCM_SocpSendDataToUDI(enChanID, pucVirData, pucPHYData, ulDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDI            
*被测函数功能描述:pucVirData异常
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDI_case3(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    VOS_UINT8                          *pucVirData;
    VOS_UINT8                          *pucPHYData;
    VOS_UINT32                          ulDataLen;

    enChanID    = SOCP_CODER_DST_OM_IND;
    pucVirData  = VOS_NULL_PTR;
    pucPHYData  = (VOS_UINT8 *)1;
    ulDataLen   = 1;

    SCM_SocpSendDataToUDI(enChanID, pucVirData, pucPHYData, ulDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDI            
*被测函数功能描述:CPM_ComSend返回结果CPM_SEND_ERR
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDI_case4(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    VOS_UINT8                          *pucVirData;
    VOS_UINT8                          *pucPHYData;
    VOS_UINT32                          ulDataLen;

    enChanID    = SOCP_CODER_DST_OM_IND;
    pucVirData  = (VOS_UINT8 *)1;
    pucPHYData  = (VOS_UINT8 *)1;
    ulDataLen   = 1;

    g_ulOmAcpuDbgFlag = 0;

    SCM_SocpSendDataToUDI(enChanID, pucVirData, pucPHYData, ulDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDI            
*被测函数功能描述:CPM_ComSend返回结果CPM_SEND_FUNC_NULL
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDI_case5(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    VOS_UINT8                          *pucVirData;
    VOS_UINT8                          *pucPHYData;
    VOS_UINT32                          ulDataLen;

    enChanID    = SOCP_CODER_DST_OM_IND;
    pucVirData  = (VOS_UINT8 *)1;
    pucPHYData  = (VOS_UINT8 *)1;
    ulDataLen   = 1;

    g_ulOmAcpuDbgFlag = 0;

    SCM_SocpSendDataToUDI(enChanID, pucVirData, pucPHYData, ulDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDI            
*被测函数功能描述:CPM_ComSend返回结果CPM_SEND_PARA_ERR
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDI_case6(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    VOS_UINT8                          *pucVirData;
    VOS_UINT8                          *pucPHYData;
    VOS_UINT32                          ulDataLen;

    enChanID    = SOCP_CODER_DST_OM_IND;
    pucVirData  = (VOS_UINT8 *)1;
    pucPHYData  = (VOS_UINT8 *)1;
    ulDataLen   = 1;

    g_ulOmAcpuDbgFlag = 0;

    SCM_SocpSendDataToUDI(enChanID, pucVirData, pucPHYData, ulDataLen);
}


/*******************************************************************
*测试项: SCM_SocpSendDataToUDI            
*被测函数功能描述:CPM_ComSend返回结果CPM_SEND_OK,ulUSBSendLen为0
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDI_case7(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    VOS_UINT8                          *pucVirData;
    VOS_UINT8                          *pucPHYData;
    VOS_UINT32                          ulDataLen;

    enChanID    = SOCP_CODER_DST_OM_IND;
    pucVirData  = (VOS_UINT8 *)1;
    pucPHYData  = (VOS_UINT8 *)1;
    ulDataLen   = 1;

    g_ulOmAcpuDbgFlag = 0;
    g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendLen = 0;

    SCM_SocpSendDataToUDI(enChanID, pucVirData, pucPHYData, ulDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDI            
*被测函数功能描述:CPM_ComSend返回结果CPM_SEND_OK,ulUSBSendLen为OM_DATA_MAX_LENGTH
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDI_case8(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    VOS_UINT8                          *pucVirData;
    VOS_UINT8                          *pucPHYData;
    VOS_UINT32                          ulDataLen;

    enChanID    = SOCP_CODER_DST_OM_IND;
    pucVirData  = (VOS_UINT8 *)1;
    pucPHYData  = (VOS_UINT8 *)1;
    ulDataLen   = 1;

    g_ulOmAcpuDbgFlag = 0;
    g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendLen = OM_DATA_MAX_LENGTH;

    SCM_SocpSendDataToUDI(enChanID, pucVirData, pucPHYData, ulDataLen);
}

/*******************************************************************
*测试项: SCM_SocpSendDataToUDI            
*被测函数功能描述:CPM_ComSend返回结果CPM_SEND_BUTT
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SCM_SocpSendDataToUDI_case9(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32             enChanID;
    VOS_UINT8                          *pucVirData;
    VOS_UINT8                          *pucPHYData;
    VOS_UINT32                          ulDataLen;

    enChanID    = SOCP_CODER_DST_OM_IND;
    pucVirData  = (VOS_UINT8 *)1;
    pucPHYData  = (VOS_UINT8 *)1;
    ulDataLen   = 1;

    g_ulOmAcpuDbgFlag = 0;

    SCM_SocpSendDataToUDI(enChanID, pucVirData, pucPHYData, ulDataLen);
}

/*******************************************************************
*测试项: SCM_Init            
*被测函数功能描述:SCM初始化
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SCM_Init_case(VOS_VOID)
{
    return SCM_Init();
}

/*******************************************************************
*测试项: COMM_Init            
*被测函数功能描述:COMM初始化
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_COMM_Init_case(VOS_VOID)
{
    return COMM_Init();
}