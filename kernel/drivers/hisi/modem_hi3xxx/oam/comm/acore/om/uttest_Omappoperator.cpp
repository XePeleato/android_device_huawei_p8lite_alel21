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
    extern unsigned int NV_GetLength(unsigned short usID, unsigned int *pulLength);
    extern void OM_AcpuSendResultChannel(unsigned int enChannel, unsigned char ucFuncType,
        unsigned int ulResult, unsigned short usReturnPrimId);
    extern unsigned int GU_OamSndPcMsgToCcpu(void *pstCtrlInfo, unsigned char *pucData, unsigned int ulSize);
    extern void * V_MemAlloc( unsigned int ulInfo, unsigned char  ucPtNo, unsigned int ulSize,
                        unsigned int ulRebootFlag, unsigned int ulFileID, int usLineNo  );
    extern unsigned int NV_ReadEx(unsigned short           enModemID,
                            unsigned short                  usID,
                            void                   *pItem,
                            unsigned int                  ulLength);
    extern unsigned int NV_WriteEx(unsigned short          enModemID,
                            unsigned short                  usID,
                            void                    *pItem,
                            unsigned int                  ulLength);
    extern unsigned int V_MemFree( unsigned int ulInfo, void **ppAddr,
                          unsigned int ulFileID, int usLineNo );
    extern void OM_SendResult(unsigned char ucFuncType,
                            unsigned int ulResult, unsigned short usReturnPrimId);
    extern unsigned int NV_GetNVIdListNum(void);
    extern unsigned int NV_GetNVIdList(void *pstNvIdList);
    extern unsigned int OM_AcpuCbtEstablishProc(void *pstAppToOmMsg,
                                           unsigned short usReturnPrimId);
    extern unsigned int OM_AcpuCnfEstablishProc(void *pstAppToOmMsg,
                                           unsigned short usReturnPrimId);
    extern unsigned int OMRL_AcpuSendMsg(unsigned char *pucData, unsigned int ulDataLen, unsigned int ulSenderPid, unsigned int ulReceiverPid);
    extern void OM_AcpuSendResult(unsigned char ucFuncType,
                            unsigned int ulResult, unsigned short usReturnPrimId);
    extern void OM_NotifyOtherCpuReleaseLink(void);
    extern unsigned int OM_GetSlice(void);
    extern unsigned int OM_AcpuEstablishReq(void *pstAppToOmMsg,
                                           unsigned short usReturnPrimId);
    extern unsigned int OM_AcpuCbtReleaseProc(void *pstAppToOmMsg,
        unsigned short usReturnPrimId);
    extern unsigned int OM_AcpuCnfReleaseProc(void *pstAppToOmMsg,
        unsigned short usReturnPrimId);
    extern void OM_AcpuSendContentChannel(unsigned int enChannel,unsigned char ucFuncType,
        void *pstOmToAppMsg, unsigned short usReturnPrimId);

    extern unsigned int uttest_OM_AcpuReadNv_case1(void);
    extern unsigned int uttest_OM_AcpuReadNv_case2(void);
    extern unsigned int uttest_OM_AcpuReadNv_case3(void);
    extern unsigned int uttest_OM_AcpuReadNv_case4(void);
    extern unsigned int uttest_OM_AcpuWriteNv_case1(void);
    extern unsigned int uttest_OM_AcpuWriteNv_case2(void);
    extern unsigned int uttest_OM_AcpuWriteNv_case3(void);
    extern unsigned int uttest_OM_AcpuGetNvIdList_case1(void);
    extern unsigned int uttest_OM_AcpuGetNvIdList_case2(void);
    extern unsigned int uttest_OM_AcpuGetNvIdList_case3(void);
    extern unsigned int uttest_OM_AcpuGetNvIdList_case4(void);
    extern unsigned int uttest_OM_IsAcpuAuthNv_case1(void);
    extern unsigned int uttest_OM_IsAcpuAuthNv_case2(void);
    extern unsigned int uttest_OM_AcpuCbtEstablishProc_case1(void);
    extern unsigned int uttest_OM_AcpuCbtEstablishProc_case2(void);
    extern unsigned int uttest_OM_AcpuCnfEstablishProc_case1(void);
    extern unsigned int uttest_OM_AcpuCnfEstablishProc_case2(void);
    extern unsigned int uttest_OM_AcpuReleaseReq_case1(void);
    extern unsigned int uttest_OM_AcpuReleaseReq_case2(void);
    extern unsigned int uttest_OM_AcpuEstablishReq_case1(void);
    extern unsigned int uttest_OM_AcpuEstablishReq_case2(void);
    extern void uttest_OM_NotifyOtherCpuReleaseLink_case1(void);
    extern unsigned int uttest_OM_AcpuCbtReleaseProc_case1(void);
    extern unsigned int uttest_OM_AcpuCnfReleaseProc_case1(void);
    extern unsigned int uttest_OM_AcpuCnfReleaseProc_case2(void);
    extern void uttest_OM_AcpuQueryMsgProc_case1(void);
    extern void uttest_OM_AcpuQueryMsgProc_case2(void);
    extern void uttest_OmAcpuShowOnOff_case1(void);

#ifdef __cplusplus
}
#endif

#ifndef VOS_OK
#define VOS_OK                          0
#endif

#ifndef VOS_ERR
#define VOS_ERR                         1
#endif

#ifndef NV_OK
#define NV_OK                           0
#endif

#ifndef MODEM_ID_0
#define MODEM_ID_0                      0
#endif

#ifndef MODEM_ID_1
#define MODEM_ID_1                      1
#endif

#ifndef MODEM_ID_BUTT
#define MODEM_ID_BUTT                   2
#endif

#ifndef VOS_YES
#define VOS_YES                         1
#endif

#ifndef VOS_NO
#define VOS_NO                          0
#endif

#ifndef VOS_NULL_PTR
#define VOS_NULL_PTR                    (0L)
#endif


/*******************************************************************
测试用例编号      : OM_AcpuReadNv
测试用例标题      : 读取NV的长度错误
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuReadNv1, UT)
{
    unsigned int                ulResult;

    MOCKER(NV_GetLength)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    ulResult = ((5<<16) + VOS_ERR);

    EXPECT_EQ(ulResult, uttest_OM_AcpuReadNv_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : OM_AcpuReadNv
测试用例标题      : 内存申请错误
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuReadNv2, UT)
{
    unsigned int        ulNVLen = 10;

    MOCKER(NV_GetLength)
        .stubs()
        .with(any(), outBoundP(&ulNVLen))
        .will(returnValue((unsigned int)NV_OK));

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void *)VOS_NULL_PTR));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    EXPECT_EQ(VOS_ERR, uttest_OM_AcpuReadNv_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : OM_AcpuReadNv
测试用例标题      : NV读取错误
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuReadNv3, UT)
{
    unsigned int                ulNVLen = 10;
    unsigned int                ulResult;
    unsigned char               aucCnf[1024];

    MOCKER(NV_GetLength)
        .stubs()
        .with(any(), outBoundP(&ulNVLen))
        .will(returnValue((unsigned int)NV_OK));

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void *)aucCnf));

    MOCKER(NV_ReadEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    MOCKER(V_MemFree)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    ulResult = ((5<<16) + VOS_ERR);

    EXPECT_EQ(ulResult, uttest_OM_AcpuReadNv_case3());

    GlobalMockObject::reset();
}


/*******************************************************************
测试用例编号      : OM_AcpuReadNv
测试用例标题      : NV读取OK
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuReadNv4, UT)
{
    unsigned char               aucCnf[1024];
    unsigned int                ulNVLen = 10;

    MOCKER(NV_GetLength)
        .stubs()
        .with(any(), outBoundP(&ulNVLen))
        .will(returnValue((unsigned int)NV_OK));

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void *)aucCnf));

    MOCKER(NV_ReadEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuSendContentChannel)
        .expects(exactly(1));

    MOCKER(V_MemFree)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadNv_case4());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : OM_AcpuWriteNv
测试用例标题      : NV需要鉴权
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuWriteNv1, UT)
{
    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    EXPECT_EQ(VOS_ERR, uttest_OM_AcpuWriteNv_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : OM_AcpuWriteNv
测试用例标题      : NV更新失败
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuWriteNv2, UT)
{
    MOCKER(NV_WriteEx)
        .stubs()
        .with(eq((unsigned short)MODEM_ID_0), any(), any(), any())
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    EXPECT_EQ(VOS_ERR, uttest_OM_AcpuWriteNv_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : OM_AcpuWriteNv
测试用例标题      : NV更新OK
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuWriteNv3, UT)
{
    MOCKER(NV_WriteEx)
        .stubs()
        .with(eq((unsigned short)MODEM_ID_0), any(), any(), any())
        .will(returnValue((unsigned int)NV_OK));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuWriteNv_case3());

    GlobalMockObject::reset();
}


/*****************************************************************************
* 用 例 名  : OM_AcpuGetNvIdList
*
* 测试描述  : 正常处理
*
* 输入数据  : 无
*
* 输出数据  : 无
*
* 判断依据  : 初始化结果
*****************************************************************************/
TEST(OM_AcpuGetNvIdList1, UT)
{
    unsigned char               aucNv[1024];

    MOCKER(NV_GetNVIdListNum)
        .stubs()
        .will(returnValue((unsigned int)2));

    MOCKER(NV_GetNVIdList)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void *)aucNv));

    MOCKER(OM_AcpuSendContentChannel)
        .expects(exactly(1));

    MOCKER(V_MemFree)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuGetNvIdList_case1());


    GlobalMockObject::reset();
}

/*****************************************************************************
* 用 例 名  : OM_AcpuGetNvIdList
*
* 测试描述  : 没有NV
*
* 输入数据  : 无
*
* 输出数据  : 无
*
* 判断依据  : 初始化结果
*****************************************************************************/
TEST(OM_AcpuGetNvIdList2, UT)
{
    MOCKER(NV_GetNVIdListNum)
        .stubs()
        .will(returnValue((unsigned int)0));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    EXPECT_EQ(VOS_ERR, uttest_OM_AcpuGetNvIdList_case2());

    GlobalMockObject::reset();
}

/*****************************************************************************
* 用 例 名  : OM_AcpuGetNvIdList
*
* 测试描述  : 分配空间失败
*
* 输入数据  : 无
*
* 输出数据  : 无
*
* 判断依据  : 初始化结果
*****************************************************************************/
TEST(OM_AcpuGetNvIdList3, UT)
{
    MOCKER(NV_GetNVIdListNum)
        .stubs()
        .will(returnValue((unsigned int)2));

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void *)VOS_NULL_PTR));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    EXPECT_EQ(VOS_ERR, uttest_OM_AcpuGetNvIdList_case3());

    GlobalMockObject::reset();
}

/*****************************************************************************
* 用 例 名  : OM_AcpuGetNvIdList
*
* 测试描述  : 获取NV项长度和列表失败
*
* 输入数据  : 无
*
* 输出数据  : 无
*
* 判断依据  : 初始化结果
*****************************************************************************/
TEST(OM_AcpuGetNvIdList4, UT)
{
    unsigned char               aucNv[1024];

    MOCKER(NV_GetNVIdListNum)
        .stubs()
        .will(returnValue((unsigned int)2));

    MOCKER(NV_GetNVIdList)
        .stubs()
        .will(returnValue((int)VOS_ERR));

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void *)aucNv));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    MOCKER(V_MemFree)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_ERR, uttest_OM_AcpuGetNvIdList_case4());

    GlobalMockObject::reset();
}

/*****************************************************************************
* 用 例 名  : OM_IsAcpuAuthNv
*
* 测试描述  : 不在鉴权列表中
*
* 输入数据  : 无
*
* 输出数据  : 无
*
* 判断依据  : 初始化结果
*****************************************************************************/
TEST(OM_IsAcpuAuthNv1, UT)
{
    EXPECT_EQ(VOS_NO, uttest_OM_IsAcpuAuthNv_case1());

    GlobalMockObject::reset();
}

/*****************************************************************************
* 用 例 名  : OM_IsAcpuAuthNv
*
* 测试描述  : 在鉴权列表中
*
* 输入数据  : 无
*
* 输出数据  : 无
*
* 判断依据  : 初始化结果
*****************************************************************************/
TEST(OM_IsAcpuAuthNv2, UT)
{
    EXPECT_EQ(VOS_YES, uttest_OM_IsAcpuAuthNv_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : Test_OM_AcpuCbtEstablishProc1
测试用例标题      : 给CCPU发送建链消息失败
预期结果          : 函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(Test_OM_AcpuCbtEstablishProc1, UT)
{
    MOCKER(GU_OamSndPcMsgToCcpu)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuCbtEstablishProc_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : Test_OM_AcpuCbtEstablishProc2
测试用例标题      : 给CCPU发送建链消息成功
预期结果          : 函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(Test_OM_AcpuCbtEstablishProc2, UT)
{
    MOCKER(GU_OamSndPcMsgToCcpu)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuCbtEstablishProc_case2());

    GlobalMockObject::reset();
}


/*******************************************************************
测试用例编号      : Test_OM_AcpuCnfEstablishProc1
测试用例标题      : 给CCPU发送建链消息失败
预期结果          : 函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(Test_OM_AcpuCnfEstablishProc1, UT)
{
    MOCKER(GU_OamSndPcMsgToCcpu)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuCnfEstablishProc_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : Test_OM_AcpuCnfEstablishProc2
测试用例标题      : 给CCPU发送建链消息成功
预期结果          : 函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(Test_OM_AcpuCnfEstablishProc2, UT)
{
    MOCKER(GU_OamSndPcMsgToCcpu)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuSendResultChannel)
        .expects(exactly(1));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuCnfEstablishProc_case2());

    GlobalMockObject::reset();
}
#if 0
/*******************************************************************
测试用例编号      : Test_OM_AcpuEstablishReq_001
测试用例标题      : 对应CBT通道
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuEstablishReq1, UT)
{
    // 调用被测函数
    MOCKER(OM_AcpuCbtEstablishProc)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuEstablishReq_case1());

    GlobalMockObject::reset();
}
#endif

/*******************************************************************
测试用例编号      : OM_AcpuEstablishReq2
测试用例标题      : 对应CFG通道
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuEstablishReq2, UT)
{
    // 调用被测函数
    MOCKER(OM_AcpuCnfEstablishProc)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuEstablishReq_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : OM_NotifyOtherCpuReleaseLink1
测试用例标题      : 通知连接断开
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_NotifyOtherCpuReleaseLink1, UT)
{
    // 调用被测函数
    MOCKER(OMRL_AcpuSendMsg)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    uttest_OM_NotifyOtherCpuReleaseLink_case1();

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : OM_AcpuCbtReleaseProc1
测试用例标题      : CBT通道断开
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuCbtReleaseProc1, UT)
{
    // 调用被测函数
    MOCKER(GU_OamSndPcMsgToCcpu)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuCbtReleaseProc_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : OM_AcpuCnfReleaseProc1
测试用例标题      : CFG通道断开，但发送消息失败
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuCnfReleaseProc1, UT)
{
    // 调用被测函数
    MOCKER(GU_OamSndPcMsgToCcpu)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(OM_NotifyOtherCpuReleaseLink)
        .expects(exactly(1));

    MOCKER(OM_GetSlice)
        .expects(exactly(1))
        .will(returnValue((unsigned int)0));

        // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuCnfReleaseProc_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : OM_AcpuCnfReleaseProc2
测试用例标题      : CFG通道断，发送消息成功
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(OM_AcpuCnfReleaseProc2, UT)
{
    // 调用被测函数
    MOCKER(GU_OamSndPcMsgToCcpu)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_NotifyOtherCpuReleaseLink)
        .expects(exactly(1));

    MOCKER(OM_GetSlice)
        .expects(exactly(1))
        .will(returnValue((unsigned int)0));

        // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuCnfReleaseProc_case2());

    GlobalMockObject::reset();
}



/*******************************************************************
测试用例编号      : Test_OM_ReleaseReq_001
测试用例标题      : 通过ICC通道通知CCPU链路断开失败
预期结果          : 记录错误，函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(Test_OM_AcpuReleaseReq1, UT)
{
    MOCKER(OM_AcpuCbtReleaseProc)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuCnfReleaseProc)
        .expects(exactly(0))
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReleaseReq_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : Test_OM_ReleaseReq_002
测试用例标题      : 给其他CPU发送链路断开消息成功
预期结果          : 函数返回VOS_OK
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(Test_OM_AcpuReleaseReq2, UT)
{
    MOCKER(OM_AcpuCbtReleaseProc)
        .expects(exactly(0))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuCnfReleaseProc)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReleaseReq_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : Test_OM_AcpuQueryMsgProc1
测试用例标题      : 查询消息处理
预期结果          :
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(Test_OM_AcpuQueryMsgProc1, UT)
{
    MOCKER(OM_AcpuSendResult)
        .expects(exactly(0));

    MOCKER(OM_AcpuEstablishReq)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    // 调用被测函数
    uttest_OM_AcpuQueryMsgProc_case1();


    // 执行检查

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : Test_OM_AcpuQueryMsgProc2
测试用例标题      : 未找到消息处理函数
预期结果          :
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(Test_OM_AcpuQueryMsgProc2, UT)
{
    MOCKER(OM_AcpuSendResult)
        .expects(exactly(1));

    // 调用被测函数
    uttest_OM_AcpuQueryMsgProc_case2();

    // 执行检查

    GlobalMockObject::reset();
}

/*******************************************************************
测试用例编号      : Test_OmAcpuShowOnOff1
测试用例标题      : 查看链路开关状态
预期结果          :
修改历史          :
1.日   期  : 2012-08-14
  作   者  : J00168360
  修改内容 : v9r1MSP拆分和可维可测开发项目新增函数
*******************************************************************/
TEST(Test_OmAcpuShowOnOff1, UT)
{
    uttest_OmAcpuShowOnOff_case1();

    // 执行检查

    GlobalMockObject::reset();
}




