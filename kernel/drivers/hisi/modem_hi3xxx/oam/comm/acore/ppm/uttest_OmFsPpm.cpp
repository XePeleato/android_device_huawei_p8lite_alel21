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

extern unsigned int OM_FSInitCfgFile(void);
extern unsigned int OM_FSCheckSpace(void);
extern unsigned int OM_FSWriteLogFile(char *pcLogData, unsigned int ulLength);
extern unsigned int NV_ReadEx(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
extern unsigned int NV_ReadExStub(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
extern unsigned int NV_ReadExStub1(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
extern void CPM_PhySendReg(unsigned int enPhyPort, void* pSendFunc);
extern unsigned int PPM_FSStartLog(void);

extern unsigned int uttest_PPM_FSStartLog_case1(void);
extern unsigned int uttest_PPM_FSStartLog_case2(void);
extern unsigned int uttest_PPM_FSStartLog_case3(void);
extern unsigned int uttest_PPM_FSStartLog_case4(void);
extern unsigned int uttest_PPM_FSWriteLogProc_case1(void);
extern unsigned int uttest_PPM_FSInitLogFile_case1(void);
extern unsigned int uttest_PPM_FSInitLogFile_case2(void);
extern unsigned int uttest_PPM_FSInitLogFile_case3(void);
extern unsigned int uttest_PPM_FSInitLogFile_case4(void);
#ifdef __cplusplus
}
#endif

#ifndef VOS_OK
#define VOS_OK                          0
#endif

#ifndef VOS_ERR
#define VOS_ERR                         1
#endif


/*******************************************************************
*测试项:GU配置保存空间配置为0，则不需要进行保存
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_FSStartLog1, UT)
{
	EXPECT_EQ(VOS_OK, uttest_PPM_FSStartLog_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:初始化配置文件失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_FSStartLog2, UT)
{
	MOCKER(OM_FSInitCfgFile)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_FSStartLog_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:检查空间失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_FSStartLog3, UT)
{
	MOCKER(OM_FSInitCfgFile)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(OM_FSCheckSpace)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_FSStartLog_case3());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:初始化失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_FSStartLog4, UT)
{
	MOCKER(OM_FSInitCfgFile)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(OM_FSCheckSpace)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_FSStartLog_case4());

	GlobalMockObject::reset();
}


/*******************************************************************
*测试项:根据Nv项配置，将收到Log数据处理后写到文件系统
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_FSWriteLogProc1, UT)
{
	MOCKER(OM_FSWriteLogFile)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_FSWriteLogProc_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:读取Nv项配置失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_FSInitLogFile1, UT)
{
	MOCKER(NV_ReadEx)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_OK, uttest_PPM_FSInitLogFile_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:读取Nv项配置，GU配置的文件大小为0
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/

TEST(PPM_FSInitLogFile2, UT)
{
	MOCKER(NV_ReadEx)
		.expects(once())
		.will(invoke(NV_ReadExStub));

	EXPECT_EQ(VOS_OK, uttest_PPM_FSInitLogFile_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:PPM_FSStartLog返回失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/

TEST(PPM_FSInitLogFile3, UT)
{
	MOCKER(NV_ReadEx)
		.expects(once())
		.will(invoke(NV_ReadExStub1));

	MOCKER(CPM_PhySendReg)
		.expects(exactly(1));

	MOCKER(PPM_FSStartLog)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_FSInitLogFile_case3());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:PPM_FSStartLog返回失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/

TEST(PPM_FSInitLogFile4, UT)
{
	MOCKER(NV_ReadEx)
		.expects(once())
		.will(invoke(NV_ReadExStub1));

	MOCKER(CPM_PhySendReg)
		.expects(exactly(1));

	MOCKER(PPM_FSStartLog)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_FSInitLogFile_case4());

	GlobalMockObject::reset();
}