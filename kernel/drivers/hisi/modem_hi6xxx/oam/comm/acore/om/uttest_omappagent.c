/* 头文件包含 */
#include "Omappagent.h"

extern VOS_VOID   OM_AcpuAgentMsgProc(MsgBlock* pMsg);

extern MsgBlock *g_pstOmAcpuCnfMsg;

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc
*被测函数功能描述:g_pstOmAcpuCnfMsg为空
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuAgentMsgProc_case1(VOS_VOID)
{
    OM_AUTOCONFIG_CNF_STRU stMsg;

    stMsg.usPrimId = OM_AUTOCONFIG_CNF;

    g_pstOmAcpuCnfMsg = VOS_NULL_PTR;

    OM_AcpuAgentMsgProc((MsgBlock*)&stMsg);
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc
*被测函数功能描述:g_pstOmAcpuCnfMsg不为空
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuAgentMsgProc_case2(VOS_VOID)
{
    OM_WRITELOG_REQ_STRU stMsg;

    stMsg.usPrimId = OM_WRITE_LOG_REQ;

    g_pstOmAcpuCnfMsg = (MsgBlock *)malloc(sizeof(MsgBlock));

    OM_AcpuAgentMsgProc((MsgBlock*)&stMsg);
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc
*被测函数功能描述:处理OM_OLED_CLEAR_REQ消息
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuAgentMsgProc_case3(VOS_VOID)
{
    OM_OLED_CLEAR_REQ_STRU stMsg;

    stMsg.usPrimId = OM_OLED_CLEAR_REQ;

    OM_AcpuAgentMsgProc((MsgBlock*)&stMsg);
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc
*被测函数功能描述:处理OM_OLED_DISPLAY_REQ消息
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuAgentMsgProc_case4(VOS_VOID)
{
    OM_OLED_DISPLAY_REQ_STRU stMsg;

    stMsg.usPrimId = OM_OLED_DISPLAY_REQ;

    OM_AcpuAgentMsgProc((MsgBlock*)&stMsg);
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc
*被测函数功能描述:处理OM_HSIC_CONNECT_REQ消息
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuAgentMsgProc_case5(VOS_VOID)
{
    OM_HSIC_CONNECT_REQ_STRU stMsg;

    stMsg.usPrimId = OM_HSIC_CONNECT_REQ;

    OM_AcpuAgentMsgProc((MsgBlock*)&stMsg);
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc
*被测函数功能描述:处理OM_CBT_SEND_DATA_REQ消息
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuAgentMsgProc_case6(VOS_VOID)
{
    OM_CBT_MSG_STRU stMsg;

    stMsg.usPrimId = OM_CBT_SEND_DATA_REQ;

    OM_AcpuAgentMsgProc((MsgBlock*)&stMsg);
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc
*被测函数功能描述:处理OM_RECORD_DBU_INFO_REQ消息
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuAgentMsgProc_case7(VOS_VOID)
{
    OM_AUTOCONFIG_CNF_STRU stMsg;

    stMsg.usPrimId = OM_RECORD_DBU_INFO_REQ;

    OM_AcpuAgentMsgProc(&stMsg);
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc
*被测函数功能描述:处理OM_CONNECT_INFO_CNF消息
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月16日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_OM_AcpuAgentMsgProc_case8(VOS_VOID)
{
    OM_CONNECT_INFO_CNF_STRU stMsg;

    stMsg.usPrimId = OM_CONNECT_INFO_CNF;

    OM_AcpuAgentMsgProc((MsgBlock*)&stMsg);
}