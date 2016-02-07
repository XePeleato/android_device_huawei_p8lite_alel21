#include "gtest/gtest.h"
#include "llt_mockcpp.h"

#include <stdio.h>
#include <stdlib.h>

//建议这样引用，避免下面用关键字时需要加前缀 testing::
using namespace testing;

#ifdef __cplusplus
extern "C"
{
#endif
    extern void OM_RecordInfoStart(unsigned int enNumber, unsigned int ulSendPid, unsigned int ulRcvPid, unsigned int ulMsgName);
    extern void OM_RecordInfoEnd(unsigned int enNumber);
    extern unsigned int V_ReserveMsg(unsigned int Pid, void * pMsg,
        unsigned int ulFileID, int usLineNo );
    extern unsigned int VOS_SmV( unsigned long Sm_ID );
    extern void OM_AutoConfigProc(void);
    extern void PPM_HsicConnectProc(void);
    extern unsigned int OM_AcpuSendDataChannel(unsigned int enChannel, void *pucMsg, unsigned short usMsgLen);
    extern void OM_AcpuLogShowToFile(unsigned int bIsSendMsg);
    extern void OMRL_AcpuConnectInfoReportEvent(void *pstConnectInfo);

    extern void uttest_OM_AcpuAgentMsgProc_case1(void);
    extern void uttest_OM_AcpuAgentMsgProc_case2(void);
    extern void uttest_OM_AcpuAgentMsgProc_case3(void);
    extern void uttest_OM_AcpuAgentMsgProc_case4(void);
    extern void uttest_OM_AcpuAgentMsgProc_case5(void);
    extern void uttest_OM_AcpuAgentMsgProc_case6(void);
    extern void uttest_OM_AcpuAgentMsgProc_case7(void);
    extern void uttest_OM_AcpuAgentMsgProc_case8(void);

#ifdef __cplusplus
}
#endif

#ifndef VOS_OK
#define VOS_OK                          0
#endif

#ifndef VOS_ERR
#define VOS_ERR                         1
#endif

#ifndef VOS_FALSE
#define VOS_FALSE                       0
#endif

#ifndef VOS_TRUE
#define VOS_TRUE                        1
#endif


/*******************************************************************
*测试项: OM_AcpuAgentMsgProc1
*被测函数功能描述:g_pstOmAcpuCnfMsg为空
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 
#    作    者: 
#    修改内容: 
*******************************************************************/
TEST(OM_AcpuAgentMsgProc1, UT)
{
    MOCKER(OM_RecordInfoStart)
        .expects(exactly(1));

    MOCKER(V_ReserveMsg)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(VOS_SmV)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AutoConfigProc)
        .expects(exactly(1));

    MOCKER(OM_RecordInfoEnd)
        .expects(exactly(1));

    uttest_OM_AcpuAgentMsgProc_case1();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc2
*被测函数功能描述:g_pstOmAcpuCnfMsg不为空
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 
#    作    者: 
#    修改内容: 
*******************************************************************/
TEST(OM_AcpuAgentMsgProc2, UT)
{
    MOCKER(OM_RecordInfoStart)
        .expects(exactly(1));

    MOCKER(V_ReserveMsg)
        .expects(exactly(0))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(VOS_SmV)
        .expects(exactly(0))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AutoConfigProc)
        .expects(exactly(0));

    MOCKER(OM_RecordInfoEnd)
        .expects(exactly(1));

    uttest_OM_AcpuAgentMsgProc_case2();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc3
*被测函数功能描述:处理OM_OLED_CLEAR_REQ消息
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 
#    作    者: 
#    修改内容: 
*******************************************************************/
TEST(OM_AcpuAgentMsgProc3, UT)
{
    MOCKER(OM_RecordInfoStart)
        .expects(exactly(1));

    MOCKER(OM_RecordInfoEnd)
        .expects(exactly(1));

    uttest_OM_AcpuAgentMsgProc_case3();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc4
*被测函数功能描述:处理OM_OLED_DISPLAY_REQ消息
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 
#    作    者: 
#    修改内容: 
*******************************************************************/
TEST(OM_AcpuAgentMsgProc4, UT)
{
    MOCKER(OM_RecordInfoStart)
        .expects(exactly(1));

    MOCKER(OM_RecordInfoEnd)
        .expects(exactly(1));

    uttest_OM_AcpuAgentMsgProc_case4();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc5
*被测函数功能描述:处理OM_HSIC_CONNECT_REQ消息
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 
#    作    者: 
#    修改内容: 
*******************************************************************/
TEST(OM_AcpuAgentMsgProc5, UT)
{
    MOCKER(OM_RecordInfoStart)
        .expects(exactly(1));

    MOCKER(OM_RecordInfoEnd)
        .expects(exactly(1));

    MOCKER(PPM_HsicConnectProc)
        .expects(exactly(1));

    uttest_OM_AcpuAgentMsgProc_case5();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc6
*被测函数功能描述:处理OM_CBT_SEND_DATA_REQ消息
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 
#    作    者: 
#    修改内容: 
*******************************************************************/
TEST(OM_AcpuAgentMsgProc6, UT)
{
    MOCKER(OM_RecordInfoStart)
        .expects(exactly(1));

    MOCKER(OM_RecordInfoEnd)
        .expects(exactly(1));

    MOCKER(OM_AcpuSendDataChannel)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    uttest_OM_AcpuAgentMsgProc_case6();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc7
*被测函数功能描述:处理OM_RECORD_DBU_INFO_REQ消息
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 
#    作    者: 
#    修改内容: 
*******************************************************************/
TEST(OM_AcpuAgentMsgProc7, UT)
{
    MOCKER(OM_RecordInfoStart)
        .expects(exactly(1));

    MOCKER(OM_RecordInfoEnd)
        .expects(exactly(1));

    MOCKER(OM_AcpuLogShowToFile)
        .expects(exactly(1))
        .with(eq((unsigned int)VOS_FALSE));

    uttest_OM_AcpuAgentMsgProc_case7();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: OM_AcpuAgentMsgProc8
*被测函数功能描述:处理OM_CONNECT_INFO_CNF消息
*预期结果：           
************************* 修改记录 *************************
#  1.日    期: 
#    作    者: 
#    修改内容: 
*******************************************************************/
TEST(OM_AcpuAgentMsgProc8, UT)
{
    MOCKER(OM_RecordInfoStart)
        .expects(exactly(1));

    MOCKER(OM_RecordInfoEnd)
        .expects(exactly(1));

    MOCKER(OMRL_AcpuConnectInfoReportEvent)
        .expects(exactly(1));

    uttest_OM_AcpuAgentMsgProc_case8();

    GlobalMockObject::reset();
}


