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

    extern unsigned int uttest_OM_AcpuCallBackMsgProc_case1(void);
    extern unsigned int uttest_OM_AcpuCallBackMsgProc_case2(void);
    extern unsigned int uttest_OM_AcpuCallBackMsgProc_case3(void);
    extern unsigned int uttest_OM_AcpuCallBackMsgProc_case4(void);
    extern unsigned int uttest_OM_AcpuCallBackFidInit_case1(void);
    extern unsigned int uttest_OM_AcpuCallBackFidInit_case2(void);
    extern unsigned int uttest_OM_AcpuCallBackFidInit_case3(void);

    extern void OM_Log1(char *cFileName,  unsigned int ulLineNum,
             unsigned int  enModuleId, unsigned int   enSubModId,
             unsigned int      enLevel,    char          *pcString,
             int         lPara1);
    extern unsigned int VOS_RegisterPIDInfo(unsigned int ulPID, void* pfnInitFun, void* pfnMsgFun);
    extern unsigned int VOS_RegisterMsgTaskPrio(unsigned int ulFID, unsigned int TaskPrio);

#ifdef __cplusplus
}
#endif

#ifndef VOS_OK
#define VOS_OK                          0
#endif

#ifndef VOS_ERR
#define VOS_ERR                         1
#endif


TEST(OM_AcpuCallBackMsgProc1, UT)
{
	MOCKER(OM_Log1)
		.expects(once());

	uttest_OM_AcpuCallBackMsgProc_case1();

	GlobalMockObject::reset();
}


TEST(OM_AcpuCallBackMsgProc2, UT)
{
	MOCKER(OM_Log1)
		.expects(once());

	uttest_OM_AcpuCallBackMsgProc_case2();

	GlobalMockObject::reset();
}


TEST(OM_AcpuCallBackMsgProc3, UT)
{
	MOCKER(OM_Log1)
		.expects(once());

	uttest_OM_AcpuCallBackMsgProc_case3();

	GlobalMockObject::reset();
}


TEST(OM_AcpuCallBackMsgProc4, UT)
{
	uttest_OM_AcpuCallBackMsgProc_case4();
}


TEST(OM_AcpuCallBackFidInit1, UT)
{
	MOCKER(VOS_RegisterPIDInfo)
	    .stubs()
		.will(returnValue(VOS_ERR));

	EXPECT_EQ(VOS_OK, uttest_OM_AcpuCallBackFidInit_case1());

	GlobalMockObject::reset();
}


TEST(OM_AcpuCallBackFidInit2, UT)
{
	MOCKER(VOS_RegisterPIDInfo)
	    .stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(VOS_RegisterMsgTaskPrio)
	    .stubs()
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_OK, uttest_OM_AcpuCallBackFidInit_case2());

	GlobalMockObject::reset();
}


TEST(OM_AcpuCallBackFidInit3, UT)
{
	MOCKER(VOS_RegisterPIDInfo)
	    .stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(VOS_RegisterMsgTaskPrio)
	    .stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_OM_AcpuCallBackFidInit_case3());

	GlobalMockObject::reset();
}
