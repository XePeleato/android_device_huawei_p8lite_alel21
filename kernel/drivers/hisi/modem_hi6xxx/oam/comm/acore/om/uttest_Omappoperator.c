/* 头文件包含 */
#include "omappoperator.h"
#include "SysNvId.h"
#include "OmCommonPpm.h"
#include "om.h"

extern VOS_UINT32 OM_IsAcpuAuthNv(VOS_UINT16 usNvId);

/*******************************************************************
*测试项: OM_AcpuReadNv
*被测函数功能描述:读取NV的长度错误
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuReadNv_case1(VOS_VOID)
{
    VOS_UINT8				aucData[1024] = {0};
    APP_OM_MSG_EX_STRU		*pstMsg;
    APP_OM_READ_NV_STRU		*pstNV;
    VOS_UINT32               ulResult;

    pstMsg = (APP_OM_MSG_EX_STRU*)aucData;
    pstNV  = (APP_OM_READ_NV_STRU*)pstMsg->aucPara;

    pstNV->ulCount = 2;
    pstNV->ausNvItemId[0] = 5;
    pstNV->ausNvItemId[1] = 6;

    ulResult = OM_AcpuReadNv(pstMsg, APP_OM_READ_NV_REQ);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuReadNv
*被测函数功能描述:内存申请错误
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuReadNv_case2(VOS_VOID)
{
    VOS_UINT8				aucData[1024] = {0};
    APP_OM_MSG_EX_STRU		*pstMsg;
    APP_OM_READ_NV_STRU		*pstNV;

    pstMsg = (APP_OM_MSG_EX_STRU*)aucData;
    pstNV  = (APP_OM_READ_NV_STRU*)pstMsg->aucPara;

    pstNV->ulCount = 2;
    pstNV->ausNvItemId[0] = 5;
    pstNV->ausNvItemId[1] = 6;

    return OM_AcpuReadNv(pstMsg, APP_OM_READ_NV_REQ);
}

/*******************************************************************
*测试项: OM_AcpuReadNv
*被测函数功能描述:NV读取错误
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuReadNv_case3(VOS_VOID)
{
    VOS_UINT8				aucData[1024] = {0};
    APP_OM_MSG_EX_STRU		*pstMsg;
    APP_OM_READ_NV_STRU		*pstNV;

    pstMsg = (APP_OM_MSG_EX_STRU*)aucData;
    pstNV  = (APP_OM_READ_NV_STRU*)pstMsg->aucPara;

    pstNV->ulCount = 2;
    pstNV->ausNvItemId[0] = 5;
    pstNV->ausNvItemId[1] = 6;

    return OM_AcpuReadNv(pstMsg, APP_OM_READ_NV_REQ);
}

/*******************************************************************
*测试项: OM_AcpuReadNv
*被测函数功能描述:NV读取OK
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuReadNv_case4(VOS_VOID)
{
    VOS_UINT8				aucData[1024] = {0};
    APP_OM_MSG_EX_STRU		*pstMsg;
    APP_OM_READ_NV_STRU		*pstNV;
    VOS_UINT32				ulResult;

    pstMsg = (APP_OM_MSG_EX_STRU*)aucData;
    pstNV  = (APP_OM_READ_NV_STRU*)pstMsg->aucPara;

    pstNV->ulCount = 2;
    pstNV->ausNvItemId[0] = 5;
    pstNV->ausNvItemId[1] = 6;

    return OM_AcpuReadNv(pstMsg, APP_OM_READ_NV_REQ);
}

/*******************************************************************
*测试项: OM_AcpuWriteNv
*被测函数功能描述:NV需要鉴权
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuWriteNv_case1(VOS_VOID)
{
    VOS_UINT8				aucData[1024] = {0};
    APP_OM_MSG_EX_STRU		*pstMsg;
    APP_OM_WRITE_NV_STRU		*pstNV;

    pstMsg = (APP_OM_MSG_EX_STRU*)aucData;
    pstNV  = (APP_OM_WRITE_NV_STRU*)pstMsg->aucPara;

    pstNV->ulCount = 2;
    pstNV->ausNvItemData[0] = en_NV_Item_USB_Enum_Status;

    return OM_AcpuWriteNv(pstMsg, APP_OM_WRITE_NV_REQ);
}

/*******************************************************************
*测试项: OM_AcpuWriteNv
*被测函数功能描述:NV更新失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuWriteNv_case2(VOS_VOID)
{
    VOS_UINT8				aucData[1024] = {0};
    APP_OM_MSG_EX_STRU		*pstMsg;
    APP_OM_WRITE_NV_STRU	*pstNV;
    VOS_UINT16				*pusNVData;

    pstMsg = (APP_OM_MSG_EX_STRU*)aucData;
    pstMsg->ucFuncType = 0;

    pstNV  = (APP_OM_WRITE_NV_STRU*)pstMsg->aucPara;

    pstNV->ulCount = 2;

    pusNVData = &pstNV->ausNvItemData[0];

    pusNVData[0] = 5;
    pusNVData[1] = 10;
    pusNVData[6] = 6;
    pusNVData[7] = 10;

    return OM_AcpuWriteNv(pstMsg, APP_OM_WRITE_NV_REQ);
}

/*******************************************************************
*测试项: OM_AcpuWriteNv
*被测函数功能描述:NV更新OK
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuWriteNv_case3(VOS_VOID)
{
    VOS_UINT8				aucData[1024] = {0};
    APP_OM_MSG_EX_STRU		*pstMsg;
    APP_OM_WRITE_NV_STRU	*pstNV;
    VOS_UINT16				*pusNVData;

    pstMsg = (APP_OM_MSG_EX_STRU*)aucData;
    pstMsg->ucFuncType = 0;
    pstNV  = (APP_OM_WRITE_NV_STRU*)pstMsg->aucPara;

    pstNV->ulCount = 2;

    pusNVData = &pstNV->ausNvItemData[0];

    pusNVData[0] = 5;
    pusNVData[1] = 10;
    pusNVData[6] = 6;
    pusNVData[7] = 10;

    return OM_AcpuWriteNv(pstMsg, APP_OM_WRITE_NV_REQ);
}

/*******************************************************************
*测试项: OM_AcpuGetNvIdList
*被测函数功能描述:正常处理
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuGetNvIdList_case1(VOS_VOID)
{
    VOS_UINT8							*pucTemp;
    APP_OM_MSG_EX_STRU					*pstAppToOmMsg;
    VOS_UINT32                           ulresult;

    pucTemp	= (VOS_UINT8*)malloc(100);
    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pucTemp;
    pstAppToOmMsg->usPrimId = APP_OM_NV_ID_LIST_REQ;

    ulresult = OM_AcpuGetNvIdList((APP_OM_MSG_EX_STRU *)pucTemp, VOS_NULL_PTR);

    free(pucTemp);

    return ulresult;
}

/*******************************************************************
*测试项: OM_AcpuGetNvIdList
*被测函数功能描述:没有NV
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuGetNvIdList_case2(VOS_VOID)
{
    VOS_UINT8							*pucTemp;
    APP_OM_MSG_EX_STRU					*pstAppToOmMsg;
    VOS_UINT32                           ulresult;

    pucTemp	= (VOS_UINT8*)malloc(100);
    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pucTemp;
    pstAppToOmMsg->usPrimId = APP_OM_NV_ID_LIST_REQ;

    ulresult = OM_AcpuGetNvIdList((APP_OM_MSG_EX_STRU *)pucTemp, VOS_NULL_PTR);

    free(pucTemp);

    return ulresult;
}

/*******************************************************************
*测试项: OM_AcpuGetNvIdList
*被测函数功能描述:分配空间失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuGetNvIdList_case3(VOS_VOID)
{
    VOS_UINT8							*pucTemp;
    APP_OM_MSG_EX_STRU					*pstAppToOmMsg;
    VOS_UINT32                           ulresult;


    pucTemp	= (VOS_UINT8*)malloc(100);
    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pucTemp;
    pstAppToOmMsg->usPrimId = APP_OM_NV_ID_LIST_REQ;

    ulresult = OM_AcpuGetNvIdList((APP_OM_MSG_EX_STRU *)pucTemp, VOS_NULL_PTR);

    free(pucTemp);

    return ulresult;
}

/*******************************************************************
*测试项: OM_AcpuGetNvIdList
*被测函数功能描述:获取NV项长度和列表失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuGetNvIdList_case4(VOS_VOID)
{
    VOS_UINT8							*pucTemp;
    APP_OM_MSG_EX_STRU					*pstAppToOmMsg;
    VOS_UINT32                           ulresult;

    pucTemp	= (VOS_UINT8*)malloc(100);
    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pucTemp;
    pstAppToOmMsg->usPrimId = APP_OM_NV_ID_LIST_REQ;

    ulresult = OM_AcpuGetNvIdList((APP_OM_MSG_EX_STRU *)pucTemp, VOS_NULL_PTR);

    free(pucTemp);

    return ulresult;
}

/*******************************************************************
*测试项: OM_IsAcpuAuthNv
*被测函数功能描述:不在鉴权列表中
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_IsAcpuAuthNv_case1(VOS_VOID)
{
    VOS_UINT32  ulResult;

    ulResult= OM_IsAcpuAuthNv(en_NV_Item_IMEI);

    return ulResult;
}

/*******************************************************************
*测试项: OM_IsAcpuAuthNv
*被测函数功能描述:在鉴权列表中
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_IsAcpuAuthNv_case2(VOS_VOID)
{
    VOS_UINT32  ulResult;

    ulResult= OM_IsAcpuAuthNv(en_NV_Item_LC_Ctrl_PARA);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuCbtEstablishProc
*被测函数功能描述:给CCPU发送建链消息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuCbtEstablishProc_case1(VOS_VOID)
{
    VOS_UINT32              ulResult;
    APP_OM_MSG_EX_STRU      stAppToOmMsg;
    VOS_UINT16 usReturnPrimId = OM_APP_ESTABLISH_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));

    ulResult = OM_AcpuCbtEstablishProc(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuCbtEstablishProc
*被测函数功能描述:给CCPU发送建链消息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuCbtEstablishProc_case2(VOS_VOID)
{
    VOS_UINT32              ulResult;
    APP_OM_MSG_EX_STRU      stAppToOmMsg;
    VOS_UINT16 usReturnPrimId = OM_APP_ESTABLISH_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));

    ulResult = OM_AcpuCbtEstablishProc(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuCnfEstablishProc
*被测函数功能描述:给CCPU发送建链消息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuCnfEstablishProc_case1(VOS_VOID)
{
    VOS_UINT32              ulResult;
    APP_OM_MSG_EX_STRU      stAppToOmMsg;
    VOS_UINT16 usReturnPrimId = OM_APP_ESTABLISH_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));

    ulResult = OM_AcpuCnfEstablishProc(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuCnfEstablishProc
*被测函数功能描述:给CCPU发送建链消息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuCnfEstablishProc_case2(VOS_VOID)
{
    VOS_UINT32              ulResult;
    APP_OM_MSG_EX_STRU      stAppToOmMsg;
    VOS_UINT16 usReturnPrimId = OM_APP_ESTABLISH_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));

    ulResult = OM_AcpuCnfEstablishProc(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuReleaseReq
*被测函数功能描述:通过ICC通道通知CCPU链路断开失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuReleaseReq_case1(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    APP_OM_MSG_EX_STRU          stAppToOmMsg;
    VOS_UINT16 usReturnPrimId = OM_APP_RELEASE_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));
    stAppToOmMsg.ucCpuId = OM_LOGIC_CHANNEL_CBT;

    ulResult = OM_AcpuReleaseReq(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuReleaseReq
*被测函数功能描述:通过ICC通道通知CCPU链路断开失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuReleaseReq_case2(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    APP_OM_MSG_EX_STRU          stAppToOmMsg;
    VOS_UINT16 usReturnPrimId   = OM_APP_RELEASE_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));
    stAppToOmMsg.ucCpuId = OM_LOGIC_CHANNEL_CNF;

    ulResult = OM_AcpuReleaseReq(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuEstablishReq
*被测函数功能描述:对应CBT通道
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuEstablishReq_case1(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    APP_OM_MSG_EX_STRU          stAppToOmMsg;
    VOS_UINT16 usReturnPrimId   = OM_APP_ESTABLISH_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));
    stAppToOmMsg.ucCpuId = OM_LOGIC_CHANNEL_CBT;

    ulResult = OM_AcpuEstablishReq(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuEstablishReq
*被测函数功能描述:对应CFG通道
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuEstablishReq_case2(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    APP_OM_MSG_EX_STRU          stAppToOmMsg;
    VOS_UINT16 usReturnPrimId   = OM_APP_ESTABLISH_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));
    stAppToOmMsg.ucCpuId = OM_LOGIC_CHANNEL_CNF;

    ulResult = OM_AcpuEstablishReq(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_NotifyOtherCpuReleaseLink
*被测函数功能描述:对应CFG通道
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_NotifyOtherCpuReleaseLink_case1(VOS_VOID)
{
    return OM_NotifyOtherCpuReleaseLink();
}

/*******************************************************************
*测试项: OM_AcpuCbtReleaseProc
*被测函数功能描述:CBT通道断开
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuCbtReleaseProc_case1(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    APP_OM_MSG_EX_STRU          stAppToOmMsg;
    VOS_UINT16 usReturnPrimId   = OM_APP_RELEASE_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));
    stAppToOmMsg.ucCpuId = OM_LOGIC_CHANNEL_CBT;

    ulResult = OM_AcpuCbtReleaseProc(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuCnfReleaseProc
*被测函数功能描述:CBT通道断开
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuCnfReleaseProc_case1(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    APP_OM_MSG_EX_STRU          stAppToOmMsg;
    VOS_UINT16 usReturnPrimId   = OM_APP_RELEASE_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));
    stAppToOmMsg.ucCpuId = OM_LOGIC_CHANNEL_CNF;

    ulResult = OM_AcpuCnfReleaseProc(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

/*******************************************************************
*测试项: OM_AcpuCnfReleaseProc
*被测函数功能描述:CBT通道断开
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_AcpuCnfReleaseProc_case2(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    APP_OM_MSG_EX_STRU          stAppToOmMsg;
    VOS_UINT16 usReturnPrimId   = OM_APP_RELEASE_CNF;

    memset(&stAppToOmMsg, 0, sizeof(stAppToOmMsg));
    stAppToOmMsg.ucCpuId = OM_LOGIC_CHANNEL_CNF;

    ulResult = OM_AcpuCnfReleaseProc(&stAppToOmMsg, usReturnPrimId);

    return ulResult;
}

VOS_UINT32 RspFuncPtr(OM_RSP_PACKET_STRU *pstRspPacket, VOS_UINT16 usLength)
{
    return 0;
}

/*******************************************************************
*测试项: OM_AcpuQueryMsgProc
*被测函数功能描述:查询消息处理
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuQueryMsgProc_case1(VOS_VOID)
{
    OM_REQ_PACKET_STRU  RspPacket;
    OM_RSP_FUNC         RspFuncPtr;
    APP_OM_MSG_EX_STRU *pstAppToOmMsg;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)&RspPacket;
    pstAppToOmMsg->usPrimId = APP_OM_ESTABLISH_REQ;

    OM_AcpuQueryMsgProc(&RspPacket, (OM_RSP_FUNC *)RspFuncPtr);

    return;
}

/*******************************************************************
*测试项: OM_AcpuQueryMsgProc
*被测函数功能描述:查询消息处理
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuQueryMsgProc_case2(VOS_VOID)
{
    OM_REQ_PACKET_STRU RspPacket;
    OM_RSP_FUNC RspFuncPtr;
    APP_OM_MSG_EX_STRU *pstAppToOmMsg;
    VOS_UINT32 ulRet;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)&RspPacket;
    pstAppToOmMsg->usPrimId = 0;

    OM_AcpuQueryMsgProc(&RspPacket, (OM_RSP_FUNC *)RspFuncPtr);

    return;
}

/*******************************************************************
*测试项: OmAcpuShowOnOff
*被测函数功能描述:查询消息处理
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OmAcpuShowOnOff_case1(VOS_VOID)
{
    OmAcpuShowOnOff();

    return;
}
