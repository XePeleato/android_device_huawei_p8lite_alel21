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
    extern int DRV_SD_SG_INIT_TABLE(const void *buf,unsigned int buflen);
    extern int DRV_SD_MULTI_TRANSFER(unsigned dev_addr,unsigned blocks,unsigned blksz,int write);
    extern int DRV_SD_GET_CAPACITY(void);
    extern unsigned int OM_SDReadHeadInfo(void);
    extern unsigned int OM_SDFlushHeadInfo(void);
    extern void OM_AcpuSendResult(unsigned char ucFuncType, unsigned int ulResult, unsigned short usReturnPrimId);
    extern void OM_AcpuSendContent(unsigned char ucFuncType, void *pstOmToAppMsg, unsigned short usReturnPrimId);
    extern void* BSP_Malloc(unsigned int u32Size, unsigned int enFlags);
    extern unsigned int NV_ReadEx(unsigned short           enModemID,
                                unsigned short                usID,
                                void                         *pItem,
                                unsigned int                  ulLength);
    extern void CPM_PhySendReg(unsigned int enPhyPort, void *pSendFunc);
    extern int DRV_SD_GET_STATUS(void);
    extern void OM_SDReadReqProc(void * pstReadSDReq);
    extern void OM_SDReadCluster(unsigned int ulPacketSn);

    extern unsigned int uttest_OM_SDReadHeadInfo_case1(void);
    extern unsigned int uttest_OM_SDReadHeadInfo_case2(void);
    extern unsigned int uttest_OM_SDReadHeadInfo_case3(void);
    extern unsigned int uttest_OM_SDReadHeadInfo_case4(void);
    extern unsigned int uttest_OM_SDReadHeadInfo_case5(void);
    extern unsigned int uttest_OM_SDFlushHeadInfo_case1(void);
    extern unsigned int uttest_OM_SDFlushHeadInfo_case2(void);
    extern unsigned int uttest_OM_SDWriteCard_case1(void);
    extern unsigned int uttest_OM_SDWriteCard_case2(void);
    extern unsigned int uttest_OM_SDWriteCard_case3(void);
    extern unsigned int uttest_OM_SDWriteCard_case4(void);
    extern void uttest_OM_SDReadCluster_case1(void);
    extern void uttest_OM_SDReadCluster_case2(void);
    extern void uttest_OM_SDReadCluster_case3(void);
    extern void uttest_OM_SDReadCluster_case4(void);
    extern unsigned int uttest_OM_SDParaCheck_case1(void);
    extern unsigned int uttest_OM_SDParaCheck_case2(void);
    extern unsigned int uttest_OM_SDParaCheck_case3(void);
    extern unsigned int uttest_OM_SDParaCheck_case4(void);
    extern unsigned int uttest_OM_SDInit_case1(void);
    extern unsigned int uttest_OM_SDInit_case2(void);
    extern unsigned int uttest_OM_SDInit_case3(void);
    extern unsigned int uttest_OM_SDInit_case4(void);
    extern void uttest_OM_PrintfSDError_case1(void);
    extern void uttest_OM_ReadSdMsgProc_case1(void);
    extern void uttest_OM_ReadSdMsgProc_case2(void);
    extern void uttest_OM_ReadSdMsgProc_case3(void);
    extern void uttest_OM_ReadSdMsgProc_case4(void);
    extern void *uttest_GetPortCfgPtr(void);
    extern void *uttest_GetstSdHeaPtr(void);
    extern void *uttest_GetstSdBufferPtr(void);



#ifdef __cplusplus
}
#endif

#ifndef VOS_OK
#define VOS_OK                          0
#endif

#ifndef VOS_ERR
#define VOS_ERR                         1
#endif

#ifndef VOS_NULL_PTR
#define VOS_NULL_PTR                    (0L)
#endif

/*******************************************************************
*测试项:
*被测函数功能描述:写入头部信息失败
*预期结果：
************************* 修改记录 *********************************
#  1.日    期: 2014年6月12日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(OM_SDReadHeadInfo1, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
		.expects(once())
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_OM_SDReadHeadInfo_case1());

    GlobalMockObject::reset();
}

TEST(OM_SDReadHeadInfo2, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
		.expects(once())
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_OM_SDReadHeadInfo_case2());

    GlobalMockObject::reset();
}

TEST(OM_SDReadHeadInfo3, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
		.expects(once())
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_OK));

	MOCKER(DRV_SD_GET_CAPACITY)
        .stubs()
		.will(returnValue(0));

	EXPECT_EQ(VOS_ERR, uttest_OM_SDReadHeadInfo_case3());

    GlobalMockObject::reset();
}

TEST(OM_SDReadHeadInfo4, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
		.expects(exactly(2))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnObjectList(VOS_OK,VOS_ERR));

	MOCKER(DRV_SD_GET_CAPACITY)
        .stubs()
		.will(returnValue((int)0xffffffff));

	EXPECT_EQ(VOS_ERR, uttest_OM_SDReadHeadInfo_case4());

    GlobalMockObject::reset();
}

TEST(OM_SDReadHeadInfo5, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(2))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnObjectList(VOS_OK,VOS_ERR));

	MOCKER(DRV_SD_GET_CAPACITY)
        .stubs()
		.will(returnValue((int)0xffffffff));

	EXPECT_EQ(VOS_ERR, uttest_OM_SDReadHeadInfo_case5());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *********************************
#  1.日    期:
#    作    者:
#    修改内容: 新建CASE
*******************************************************************/
TEST(OM_SDFlushHeadInfo1, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(1))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_OM_SDFlushHeadInfo_case1());

    GlobalMockObject::reset();
}

TEST(OM_SDFlushHeadInfo2, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(1))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_OM_SDFlushHeadInfo_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *********************************
#  1.日    期:
#    作    者:
#    修改内容: 新建CASE
*******************************************************************/
TEST(OM_SDWriteCard1, UT)
{
	EXPECT_EQ(VOS_OK, uttest_OM_SDWriteCard_case1());

    GlobalMockObject::reset();
}

TEST(OM_SDWriteCard2, UT)
{
	MOCKER(OM_SDReadHeadInfo)
        .stubs()
		.will(returnValue(VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_OM_SDWriteCard_case2());

    GlobalMockObject::reset();
}

TEST(OM_SDWriteCard3, UT)
{
	MOCKER(OM_SDReadHeadInfo)
        .stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(1))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_OM_SDWriteCard_case3());

    GlobalMockObject::reset();
}

TEST(OM_SDWriteCard4, UT)
{
	MOCKER(OM_SDReadHeadInfo)
        .stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(1))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_OK));

	MOCKER(OM_SDFlushHeadInfo)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_OM_SDWriteCard_case4());

    GlobalMockObject::reset();
}


/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *********************************
#  1.日    期:
#    作    者:
#    修改内容: 新建CASE
*******************************************************************/
TEST(OM_SDReadCluster1, UT)
{
	MOCKER(OM_AcpuSendResult)
        .expects(exactly(1));

	uttest_OM_SDReadCluster_case1();

    GlobalMockObject::reset();
}

TEST(OM_SDReadCluster2, UT)
{
	MOCKER(OM_AcpuSendResult)
        .expects(exactly(1));

	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(1))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_ERR));

    uttest_OM_SDReadCluster_case2();

    GlobalMockObject::reset();
}

TEST(OM_SDReadCluster3, UT)
{
	MOCKER(OM_AcpuSendResult)
        .expects(exactly(1));

	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(5))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_OK));

	MOCKER(OM_AcpuSendContent)
        .expects(exactly(5));

    uttest_OM_SDReadCluster_case3();

    GlobalMockObject::reset();
}

TEST(OM_SDReadCluster4, UT)
{
	MOCKER(OM_AcpuSendResult)
        .expects(exactly(1));

	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(4))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_OK));

	MOCKER(OM_AcpuSendContent)
        .expects(exactly(4));

    uttest_OM_SDReadCluster_case4();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *********************************
#  1.日    期:
#    作    者:
#    修改内容: 新建CASE
*******************************************************************/
TEST(OM_SDParaCheck1, UT)
{
	EXPECT_EQ(VOS_ERR,uttest_OM_SDParaCheck_case1());

    GlobalMockObject::reset();
}

TEST(OM_SDParaCheck2, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(1))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_ERR));

	EXPECT_EQ(VOS_ERR,uttest_OM_SDParaCheck_case2());

    GlobalMockObject::reset();
}

TEST(OM_SDParaCheck3, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(1))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_OK));

	EXPECT_EQ(VOS_ERR,uttest_OM_SDParaCheck_case3());

    GlobalMockObject::reset();
}

TEST(OM_SDParaCheck4, UT)
{
	MOCKER(DRV_SD_SG_INIT_TABLE)
        .expects(exactly(1))
        .will(returnValue(1));

	MOCKER(DRV_SD_MULTI_TRANSFER)
        .stubs()
		.will(returnValue(VOS_OK));

	EXPECT_EQ(VOS_OK,uttest_OM_SDParaCheck_case4());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *********************************
#  1.日    期:
#    作    者:
#    修改内容: 新建CASE
*******************************************************************/
TEST(OM_SDInit1, UT)
{
	MOCKER(BSP_Malloc)
        .expects(exactly(1))
		.will(returnValue(uttest_GetstSdHeaPtr()));

	MOCKER(BSP_Malloc)
        .expects(exactly(1))
		.will(returnValue(uttest_GetstSdBufferPtr()));

	EXPECT_EQ(VOS_OK,uttest_OM_SDInit_case1());

    GlobalMockObject::reset();
}

TEST(OM_SDInit2, UT)
{
	MOCKER(NV_ReadEx)
        .stubs()
		.with(any(), any(),outBoundP(uttest_GetPortCfgPtr()), any())
		.will(returnValue(VOS_OK));

	MOCKER(CPM_PhySendReg)
        .expects(exactly(1));

	MOCKER(BSP_Malloc)
        .stubs()
		.will(returnValue((void *)VOS_NULL_PTR));

	EXPECT_EQ(VOS_ERR,uttest_OM_SDInit_case2());

    GlobalMockObject::reset();
}

TEST(OM_SDInit3, UT)
{
	MOCKER(CPM_PhySendReg)
        .expects(exactly(1));

	MOCKER(BSP_Malloc)
        .expects(exactly(2))
		.will(returnObjectList(uttest_GetstSdHeaPtr(), (void *)VOS_NULL_PTR));

	EXPECT_EQ(VOS_ERR,uttest_OM_SDInit_case3());

    GlobalMockObject::reset();
}

TEST(OM_SDInit4, UT)
{
	MOCKER(NV_ReadEx)
        .stubs()
		.with(any(), any(),outBoundP(uttest_GetPortCfgPtr()), any())
		.will(returnValue(VOS_OK));

	MOCKER(CPM_PhySendReg)
        .expects(exactly(1));

	MOCKER(BSP_Malloc)
        .expects(exactly(1))
		.will(returnValue(uttest_GetstSdHeaPtr()));

	MOCKER(BSP_Malloc)
        .expects(exactly(2))
		.will(returnValue(uttest_GetstSdBufferPtr()));

	EXPECT_EQ(VOS_OK,uttest_OM_SDInit_case4());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *********************************
#  1.日    期:
#    作    者:
#    修改内容: 新建CASE
*******************************************************************/
TEST(OM_PrintfSDError1, UT)
{
	uttest_OM_PrintfSDError_case1();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *********************************
#  1.日    期:
#    作    者:
#    修改内容: 新建CASE
*******************************************************************/
TEST(OM_ReadSdMsgProc1, UT)
{
	MOCKER(DRV_SD_GET_STATUS)
        .stubs()
		.will(returnValue(VOS_ERR));

    MOCKER(OM_AcpuSendResult)
        .expects(exactly(1));

    uttest_OM_ReadSdMsgProc_case1();

    GlobalMockObject::reset();
}

TEST(OM_ReadSdMsgProc2, UT )
{
	MOCKER(DRV_SD_GET_STATUS)
        .stubs()
		.will(returnValue(VOS_OK));
    
    MOCKER(OM_SDReadReqProc)
        .expects(exactly(1));

    uttest_OM_ReadSdMsgProc_case2();

    GlobalMockObject::reset();
}

TEST(OM_ReadSdMsgProc3, UT )
{
	MOCKER(DRV_SD_GET_STATUS)
        .stubs()
		.will(returnValue(VOS_OK));

    MOCKER(OM_SDReadCluster)
        .expects(exactly(1));

    uttest_OM_ReadSdMsgProc_case3();

    GlobalMockObject::reset();
}

TEST(OM_ReadSdMsgProc4, UT )
{
	MOCKER(DRV_SD_GET_STATUS)
        .stubs()
		.will(returnValue(VOS_OK));

	uttest_OM_ReadSdMsgProc_case4();

    GlobalMockObject::reset();
}
