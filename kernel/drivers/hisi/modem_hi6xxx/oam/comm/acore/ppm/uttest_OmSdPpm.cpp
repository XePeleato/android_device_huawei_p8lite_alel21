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
extern unsigned int NV_ReadEx(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
extern unsigned int NV_ReadExPPMLogFileInitStub(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
extern unsigned int NV_ReadExPPMLogFileInitStub1(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
extern unsigned int NV_ReadExPPMLogFileInitStub2(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
extern unsigned int OM_SDInit(void);
extern unsigned int PPM_FSInitLogFile(void);


extern unsigned int uttest_PPM_LogFileInit_case1(void);
extern unsigned int uttest_PPM_LogFileInit_case2(void);
extern unsigned int uttest_PPM_LogFileInit_case3(void);
extern unsigned int uttest_PPM_LogFileInit_case4(void);
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
*测试项: NV项读取失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_LogFileInit1, UT)
{
	MOCKER(NV_ReadEx)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_OK, uttest_PPM_LogFileInit_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:NV项读取配置输出到SD卡
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_LogFileInit2, UT)
{
	MOCKER(NV_ReadEx)
		.expects(once())
		.will(invoke(NV_ReadExPPMLogFileInitStub));

	MOCKER(OM_SDInit)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_LogFileInit_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:NV项读取配置输出到文件系统
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_LogFileInit3, UT)
{
	MOCKER(NV_ReadEx)
		.expects(once())
		.will(invoke(NV_ReadExPPMLogFileInitStub1));

	MOCKER(PPM_FSInitLogFile)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_LogFileInit_case3());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:NV项读取配置是输出到USB
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_LogFileInit4, UT)
{
	MOCKER(NV_ReadEx)
		.expects(once())
		.will(invoke(NV_ReadExPPMLogFileInitStub2));

	EXPECT_EQ(VOS_OK, uttest_PPM_LogFileInit_case4());

	GlobalMockObject::reset();
}


