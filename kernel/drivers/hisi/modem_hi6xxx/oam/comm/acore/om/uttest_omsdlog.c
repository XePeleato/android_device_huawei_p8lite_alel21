/* 头文件包含 */
#include "omsdlog.h"

extern OM_SD_HEAD_STRU                     *g_pstHeadInfo;
extern OM_SD_BUFFER_INNER_DATA_STRU        *g_pstSdBuffer;
extern OM_SD_DEBUG_INFO                     g_stOmSDDebugInfo;
extern OM_SD_READ_INFO_STRU                 g_stReadInfo;
extern VOS_UINT32                           g_ulInterval;

OM_PORT_CFG_STRU                            stPortCfg;
OM_SD_HEAD_STRU                             stSdHea;
OM_SD_BUFFER_INNER_DATA_STRU                stSdBuffer;



/*******************************************************************
*测试项: OM_SDReadHeadInfo
*被测函数功能描述:写入头部信息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDReadHeadInfo_case1(VOS_VOID)
{
    return OM_SDReadHeadInfo();
}


/*******************************************************************
*测试项: OM_SDReadHeadInfo
*被测函数功能描述:写入头部信息成功
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDReadHeadInfo_case2(VOS_VOID)
{
    VOS_UINT32  ulResult;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));

    g_pstHeadInfo->ulMagicNum = OM_SD_MAGIC_NUMBER;

    g_pstHeadInfo->ulOppositeMagicNum = OM_SD_OPPOSITE_MAGIC_NUMBER;

    ulResult = OM_SDReadHeadInfo();

    free(g_pstHeadInfo);

    return ulResult;
}

/*******************************************************************
*测试项: OM_SDReadHeadInfo
*被测函数功能描述:获取新SD卡的头部信息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDReadHeadInfo_case3(VOS_VOID)
{
    VOS_UINT32  ulResult;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));

    g_pstHeadInfo->ulMagicNum = 0xff;

    ulResult = OM_SDReadHeadInfo();

    free(g_pstHeadInfo);

    return ulResult;
}

/*******************************************************************
*测试项: OM_SDReadHeadInfo
*被测函数功能描述:初始化新卡失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDReadHeadInfo_case4(VOS_VOID)
{
    VOS_UINT32  ulResult;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));

    g_pstHeadInfo->ulMagicNum = 0xff;

    ulResult = OM_SDReadHeadInfo();

    free(g_pstHeadInfo);

    return ulResult;
}

/*******************************************************************
*测试项: OM_SDReadHeadInfo
*被测函数功能描述:读取卡头部信息成功
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDReadHeadInfo_case5(VOS_VOID)
{
    VOS_UINT32  ulResult;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));

    g_pstHeadInfo->ulMagicNum = 0xff;

    ulResult = OM_SDReadHeadInfo();

    free(g_pstHeadInfo);

    return ulResult;
}

/*******************************************************************
*测试项: OM_SDFlushHeadInfo
*被测函数功能描述:更新头部信息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDFlushHeadInfo_case1(VOS_VOID)
{
    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));

    g_pstHeadInfo->ulNextCid = 11;

    g_pstHeadInfo->ulTotalCNum = 10;

    return OM_SDFlushHeadInfo();
}

/*******************************************************************
*测试项: OM_SDFlushHeadInfo
*被测函数功能描述:更新头部信息成功
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDFlushHeadInfo_case2(VOS_VOID)
{
    VOS_UINT32  ulResult;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));

    g_pstHeadInfo->ulNextCid = 9;

    g_pstHeadInfo->ulTotalCNum = 10;

    ulResult = OM_SDFlushHeadInfo();

    free(g_pstHeadInfo);

    return ulResult;
}


/*******************************************************************
*测试项: OM_SDWriteCard
*被测函数功能描述:空间足够存放消息
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDWriteCard_case1(VOS_VOID)
{
    VOS_UINT8   pucData[100];
    VOS_UINT32  ulResult;

    g_pstSdBuffer = (OM_SD_BUFFER_INNER_DATA_STRU*)malloc(sizeof(OM_SD_BUFFER_INNER_DATA_STRU));

    g_pstSdBuffer->ulDataLen = OM_SD_BUFFER_MAX_DATA_LEN - 0xff;

    ulResult = OM_SDWriteCard(pucData, VOS_NULL_PTR, 0xf);

    free(g_pstSdBuffer);

    return ulResult;
}

/*******************************************************************
*测试项: OM_SDWriteCard
*被测函数功能描述:空间不够存放Trace_读头信息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDWriteCard_case2(VOS_VOID)
{
    VOS_UINT8   pucData[100];
    VOS_UINT32  ulResult;

    g_pstSdBuffer = (OM_SD_BUFFER_INNER_DATA_STRU*)malloc(sizeof(OM_SD_BUFFER_INNER_DATA_STRU));

    g_pstSdBuffer->ulDataLen = OM_SD_BUFFER_MAX_DATA_LEN - 0xf;

    ulResult = OM_SDWriteCard(pucData, VOS_NULL_PTR, 0xff);

    free(g_pstSdBuffer);

    return ulResult;
}

/*******************************************************************
*测试项: OM_SDWriteCard
*被测函数功能描述:存放消息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDWriteCard_case3(VOS_VOID)
{
    VOS_UINT8   pucData[100];
    VOS_UINT32  ulResult;

    g_pstSdBuffer = (OM_SD_BUFFER_INNER_DATA_STRU*)malloc(sizeof(OM_SD_BUFFER_INNER_DATA_STRU));

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));

    g_pstSdBuffer->ulDataLen = OM_SD_BUFFER_MAX_DATA_LEN - 0xf;

    g_pstHeadInfo->ulNextCid = 10;

    g_pstHeadInfo->ulNextCid = 20;

    g_stOmSDDebugInfo.ulDataLenth = OM_SD_MAX_DATA_LENTH;

    ulResult = OM_SDWriteCard(pucData, VOS_NULL_PTR, 0xff);

    free(g_pstSdBuffer);
    free(g_pstHeadInfo);

    return ulResult;
}

/*******************************************************************
*测试项: OM_SDWriteCard
*被测函数功能描述:存放消息成功
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDWriteCard_case4(VOS_VOID)
{
    VOS_UINT8   pucData[256];
    VOS_UINT32  ulResult;

    g_pstSdBuffer = (OM_SD_BUFFER_INNER_DATA_STRU*)malloc(sizeof(OM_SD_BUFFER_INNER_DATA_STRU));

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));

    g_pstSdBuffer->ulDataLen = OM_SD_BUFFER_MAX_DATA_LEN - 0xf;

    g_pstHeadInfo->ulNextCid = 10;

    g_pstHeadInfo->ulNextCid = 20;

    ulResult = OM_SDWriteCard(pucData, VOS_NULL_PTR, 0xff);

    free(g_pstSdBuffer);
    free(g_pstHeadInfo);

    return ulResult;
}

/*******************************************************************
*测试项: OM_SDReadCluster
*被测函数功能描述:PC侧发的数据包序号超过需要读的序号
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_SDReadCluster_case1(VOS_VOID)
{
    g_stReadInfo.ulTotalNeedCNum = 100;

    OM_SDReadCluster(100);

    g_stReadInfo.ulTotalNeedCNum = 0;
}

/*******************************************************************
*测试项: OM_SDReadCluster
*被测函数功能描述:读SD卡数据失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_SDReadCluster_case2(VOS_VOID)
{
    g_stReadInfo.ulTotalNeedCNum = 100;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(OM_SD_BLOCK_SIZE);

    g_pstHeadInfo->ulTotalCNum = 100;

    g_ulInterval = 10;

    OM_SDReadCluster(30);

    g_stReadInfo.ulTotalNeedCNum = 0;

    g_ulInterval = 0;
}

/*******************************************************************
*测试项: OM_SDReadCluster
*被测函数功能描述:当前cluster数据损坏
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_SDReadCluster_case3(VOS_VOID)
{
    g_stReadInfo.ulTotalNeedCNum = 100;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(OM_SD_BLOCK_SIZE);
    g_pstHeadInfo->ulTotalCNum = 100;

    g_pstSdBuffer = (OM_SD_BUFFER_INNER_DATA_STRU*)malloc(sizeof(OM_SD_BUFFER_INNER_DATA_STRU));

    g_ulInterval = 5;

    g_stReadInfo.ulStartCID = 20;

    g_pstSdBuffer->ulClusterId = 10;


    g_pstSdBuffer->ulOppositeClusterId = ~(g_pstSdBuffer->ulClusterId);

    OM_SDReadCluster(80);

    g_stReadInfo.ulTotalNeedCNum = 0;

    g_ulInterval = 0;

    free(g_pstSdBuffer);
}

/*******************************************************************
*测试项: OM_SDReadCluster
*被测函数功能描述:读取成功
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_SDReadCluster_case4(VOS_VOID)
{
    g_stReadInfo.ulTotalNeedCNum = 100;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(OM_SD_BLOCK_SIZE);
    g_pstHeadInfo->ulTotalCNum = 100;

    g_ulInterval = 5;

    g_pstSdBuffer = (OM_SD_BUFFER_INNER_DATA_STRU*)malloc(sizeof(OM_SD_BUFFER_INNER_DATA_STRU));

    g_stReadInfo.ulStartCID = 20;

    g_pstSdBuffer->ulClusterId = 10;

    g_pstSdBuffer->ulOppositeClusterId = ~(g_pstSdBuffer->ulClusterId + 1);

    OM_SDReadCluster(96);

    g_stReadInfo.ulTotalNeedCNum = 0;

    g_ulInterval = 0;

    free(g_pstSdBuffer);
}


/*******************************************************************
*测试项: OM_SDParaCheck
*被测函数功能描述:用户需要的trace大小为0
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDParaCheck_case1(VOS_VOID)
{
    VOS_UINT32  ulErrorNum;

    return OM_SDParaCheck(0,&ulErrorNum);
}

/*******************************************************************
*测试项: OM_SDParaCheck
*被测函数功能描述:读数据失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDParaCheck_case2(VOS_VOID)
{
    VOS_UINT32  ulErrorNum;

    return OM_SDParaCheck(100,&ulErrorNum);
}

/*******************************************************************
*测试项: OM_SDParaCheck
*被测函数功能描述:SD卡为新卡
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDParaCheck_case3(VOS_VOID)
{
    VOS_UINT32  ulErrorNum;
    VOS_UINT32  ulresult;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));


    g_pstHeadInfo->ulMagicNum = 0xffff ;

    g_pstHeadInfo->ulOppositeMagicNum =0xffff ;


    ulresult =  OM_SDParaCheck(100,&ulErrorNum);

    free(g_pstHeadInfo);

    return ulresult;
}


/*******************************************************************
*测试项: OM_SDParaCheck
*被测函数功能描述:读数据成功
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDParaCheck_case4(VOS_VOID)
{
    VOS_UINT32  ulErrorNum;
    VOS_UINT32  ulresult;

    g_pstHeadInfo = (OM_SD_HEAD_STRU*)malloc(sizeof(OM_SD_HEAD_STRU));


    g_pstHeadInfo->ulMagicNum = OM_SD_MAGIC_NUMBER ;

    g_pstHeadInfo->ulOppositeMagicNum = OM_SD_OPPOSITE_MAGIC_NUMBER ;

    ulresult =  OM_SDParaCheck(100,&ulErrorNum);

    free(g_pstHeadInfo);

    return ulresult;
}

/*******************************************************************
*测试项: OM_SDInit
*被测函数功能描述:NV项中SD卡保存trace没有使能
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDInit_case1(VOS_VOID)
{
    stPortCfg.enPortType = 0;
    stPortCfg.ulMaxFileSize = 123456;

    return OM_SDInit();
}

/*******************************************************************
*测试项: OM_SDInit
*被测函数功能描述:分配头信息地址失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDInit_case2(VOS_VOID)
{
    stPortCfg.enPortType = 1;
    stPortCfg.ulMaxFileSize = 123456;

    return OM_SDInit();
}

/*******************************************************************
*测试项: OM_SDInit
*被测函数功能描述:分配SD卡保存信息buffer失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDInit_case3(VOS_VOID)
{
    stPortCfg.enPortType = 1;
    stPortCfg.ulMaxFileSize = 123456;

    return OM_SDInit();
}

/*******************************************************************
*测试项: OM_SDInit
*被测函数功能描述:初始化成功
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_OM_SDInit_case4(VOS_VOID)
{
    stPortCfg.enPortType = 1;
    stPortCfg.ulMaxFileSize = 123456;

    return OM_SDInit();
}

/*******************************************************************
*测试项: OM_PrintfSDError
*被测函数功能描述:查看SD卡Debug信息
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_PrintfSDError_case1(VOS_VOID)
{
    OM_PrintfSDError();
}

/*******************************************************************
*测试项: OM_ReadSdMsgProc
*被测函数功能描述:检测到SD卡不在位
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_ReadSdMsgProc_case1(VOS_VOID)
{
    OM_REQ_PACKET_STRU *pstRspPacke;
    OM_RSP_FUNC *pstRspFuncPtr=VOS_NULL_PTR;

    pstRspPacke = (OM_REQ_PACKET_STRU *)malloc(200);

    OM_ReadSdMsgProc(pstRspPacke,pstRspFuncPtr);

    free(pstRspPacke);
}

/*******************************************************************
*测试项: OM_ReadSdMsgProc
*被测函数功能描述:PC下发导出SD卡数据的消息
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_ReadSdMsgProc_case2(VOS_VOID)
{
    OM_REQ_PACKET_STRU	*pstRspPacket;
    OM_RSP_FUNC			*pstRspFuncPtr=VOS_NULL_PTR;
    APP_OM_MSG_EX_STRU              *pstAppToOmMsg;

    pstRspPacket = (OM_REQ_PACKET_STRU *)malloc(200);
    pstAppToOmMsg = (APP_OM_MSG_EX_STRU *)pstRspPacket;
    pstAppToOmMsg->usPrimId = APP_OM_READ_SD_DATA_REQ;

    OM_ReadSdMsgProc(pstRspPacket,pstRspFuncPtr);

    free(pstRspPacket);
}

/*******************************************************************
*测试项: OM_ReadSdMsgProc
*被测函数功能描述:PC下发读SD卡cluster消息
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_ReadSdMsgProc_case3(VOS_VOID)
{
    OM_REQ_PACKET_STRU	*pstRspPacket;
    OM_RSP_FUNC			*pstRspFuncPtr=VOS_NULL_PTR;
    APP_OM_MSG_EX_STRU              *pstAppToOmMsg;

    pstRspPacket = (OM_REQ_PACKET_STRU *)malloc(200);
    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pstRspPacket;
    pstAppToOmMsg->usPrimId = APP_OM_READ_SD_CLUSTER_REQ;

    OM_ReadSdMsgProc(pstRspPacket,pstRspFuncPtr);

    free(pstRspPacket);
}

/*******************************************************************
*测试项: OM_ReadSdMsgProc
*被测函数功能描述:PC下消息错误
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_ReadSdMsgProc_case4(VOS_VOID)
{
    OM_REQ_PACKET_STRU	*pstRspPacket;
    OM_RSP_FUNC			*pstRspFuncPtr=VOS_NULL_PTR;
    APP_OM_MSG_EX_STRU              *pstAppToOmMsg;

    pstRspPacket = (OM_REQ_PACKET_STRU *)malloc(200);
    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pstRspPacket;
    pstAppToOmMsg->usPrimId = 0x1;

    OM_ReadSdMsgProc(pstRspPacket,pstRspFuncPtr);

    free(pstRspPacket);
}

VOS_VOID *uttest_GetPortCfgPtr(VOS_VOID)
{
    return &stPortCfg;
}

VOS_VOID *uttest_GetstSdHeaPtr(VOS_VOID)
{
    return &stSdHea;
}

VOS_VOID *uttest_GetstSdBufferPtr(VOS_VOID)
{
    return &stSdBuffer;
}
