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
    extern unsigned int uttest_OM_AcpuReadVComData_case1(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case2(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case3(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case4(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case5(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case6(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case7(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case8(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case9(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case10(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case11(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case12(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case13(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case14(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case15(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case16(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case17(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case18(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case19(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case20(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case21(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case22(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case23(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case24(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case25(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case26(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case27(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case28(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case29(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case30(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case31(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case32(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case33(void);
    extern unsigned int uttest_OM_AcpuReadVComData_case34(void);
    extern void         uttest_OM_AcpuMsgProc_case1(void);
    extern void         uttest_OM_AcpuMsgProc_case2(void);
    extern void         uttest_OM_AcpuMsgProc_case3(void);
    extern void         uttest_OM_AcpuMsgProc_case4(void);
    extern void         uttest_OM_AcpuMsgProc_case5(void);
    extern void         uttest_OM_AcpuMsgProc_case6(void);
    extern void         uttest_OM_AcpuMsgProc_case7(void);
    extern void         uttest_OM_AcpuMsgProc_case8(void);
    extern void         uttest_OM_AcpuMsgProc_case9(void);
    extern void         uttest_OM_AcpuMsgProc_case10(void);
    extern void         uttest_OM_AcpuMsgProc_case11(void);
    extern void         uttest_OM_AcpuMsgProc_case12(void);
    extern void         uttest_OM_AcpuMsgProc_case13(void);
	extern void         uttest_OM_AcpuMsgProc_case14(void);
	extern void         uttest_OM_AcpuErrLogHook_case1(void);

    extern unsigned int NV_WriteEx(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int NV_ReadEx(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int APP_VCOM_Send (unsigned char ucDevIndex, unsigned char *pData, unsigned int uslength);
    extern int OM_AcpuRcvAppMsgCheck(void *pstAppOmReqErrLog);
    extern void * V_AllocMsg(unsigned int Pid, unsigned int ulLength, unsigned int ulFileID, signed int usLineNo);
    extern unsigned int V_SendMsg(unsigned int Pid, void **ppMsg, unsigned int ulFileID, signed int lLineNo);
    extern unsigned int VOS_TaskUnlock(void);
    extern unsigned int uttest_NV_ReadEx_stub2(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int uttest_NV_ReadEx_stub3(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int uttest_NV_ReadEx_stub4(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int uttest_NV_ReadEx_stub5(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int uttest_NV_ReadEx_stub6(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int uttest_NV_ReadEx_stub7(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int uttest_NV_ReadEx_stub8(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int uttest_NV_ReadEx_stub9(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int V_StartRelTimer(void *phTm, unsigned int Pid, unsigned int ulLength,
                                        unsigned int ulName, unsigned int ulParam, unsigned char ucMode, unsigned int enPrecision,
                                        unsigned int ulFileID, int usLineNo);
    extern unsigned int V_StopRelTimer(void *phTm, unsigned int ulFileID, unsigned int usLineNo);

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
#define VOS_NULL_PTR                    0
#endif


TEST(OM_AcpuReadVComData1, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case1());
}


TEST(OM_AcpuReadVComData2, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case2());
}


TEST(OM_AcpuReadVComData3, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case3());
}

/************************************************************************/
/*                   下发Error Log开关                                  */
/************************************************************************/

TEST(OM_AcpuReadVComData4, UT)
{
    MOCKER(NV_WriteEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case4());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData5, UT)
{
    MOCKER(NV_WriteEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case5());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData6, UT)
{
     MOCKER(NV_WriteEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case6());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData7, UT)
{
    MOCKER(NV_WriteEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)VOS_NULL_PTR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case7());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData8, UT)
{
    unsigned char	                    aucData[100];

    MOCKER(NV_WriteEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(V_SendMsg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case8());

    GlobalMockObject::reset();
}

/************************************************************************/
/*                   下发FTM 模式开关                                   */
/************************************************************************/

TEST(OM_AcpuReadVComData9, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case9());
}


TEST(OM_AcpuReadVComData10, UT)
{
    MOCKER(NV_ReadEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case10());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData11, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case11());
}


TEST(OM_AcpuReadVComData12, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case12());
}


TEST(OM_AcpuReadVComData13, UT)
{
    unsigned char                       aucData[100];

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(V_SendMsg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case13());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData14, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case14());
}


TEST(OM_AcpuReadVComData15, UT)
{
    MOCKER(NV_ReadEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case15());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData16, UT)
{
#if 0
    MOCKER(NV_ReadEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)VOS_NULL_PTR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case16());

    GlobalMockObject::reset();
#endif
}


TEST(OM_AcpuReadVComData17, UT)
{
    unsigned char                       aucData[100];

    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub2));

    MOCKER(NV_WriteEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(V_SendMsg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case17());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData18, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case18());
}

/************************************************************************/
/*                   下发故障模式上报请求                               */
/************************************************************************/

TEST(OM_AcpuReadVComData19, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case19());
}


TEST(OM_AcpuReadVComData20, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case20());
}



TEST(OM_AcpuReadVComData21, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case21());
}


TEST(OM_AcpuReadVComData22, UT)
{
    MOCKER(NV_ReadEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case22());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData23, UT)
{
    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub3));

    MOCKER(APP_VCOM_Send)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case23());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData24, UT)
{
    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub4));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case24());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData25, UT)
{
    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub5));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case25());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData26, UT)
{
    MOCKER(OM_AcpuRcvAppMsgCheck)
        .stubs()
        .will(returnValue(VOS_OK));

    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub6));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case26());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData27, UT)
{
    MOCKER(OM_AcpuRcvAppMsgCheck)
        .stubs()
        .will(returnValue(VOS_OK));

    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub7));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case27());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData28, UT)
{
    unsigned char                       aucData[100];

    MOCKER(OM_AcpuRcvAppMsgCheck)
        .stubs()
        .will(returnValue(VOS_OK));

    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub8));

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(V_SendMsg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(V_StartRelTimer)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case28());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData29, UT)
{
    unsigned char                       aucData[100];

    MOCKER(OM_AcpuRcvAppMsgCheck)
        .stubs()
        .will(returnValue(VOS_OK));

    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub8));

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(V_SendMsg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(V_StartRelTimer)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case29());

    GlobalMockObject::reset();
}



TEST(OM_AcpuReadVComData30, UT)
{
    MOCKER(OM_AcpuRcvAppMsgCheck)
        .stubs()
        .will(returnValue(VOS_OK));

    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub8));

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)VOS_NULL_PTR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case30());

    GlobalMockObject::reset();
}


TEST(OM_AcpuReadVComData31, UT)
{
    MOCKER(OM_AcpuRcvAppMsgCheck)
        .stubs()
        .will(returnValue(VOS_OK));

    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub9));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case31());

    GlobalMockObject::reset();

}


TEST(OM_AcpuReadVComData32, UT)
{
    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case32());
}


TEST(OM_AcpuReadVComData33, UT)
{
	MOCKER(V_AllocMsg)
	    .stubs()
	    .will(returnValue((void*)VOS_NULL_PTR));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case33());

    GlobalMockObject::reset();
}

TEST(OM_AcpuReadVComData34, UT)
{
    unsigned char                       aucData[100];

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(V_SendMsg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    // 执行检查
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuReadVComData_case34());

    GlobalMockObject::reset();
}


/************************************************************************/
/*                         各组件上报消息内容                           */
/************************************************************************/

TEST(OM_AcpuMsgProc1, UT)
{
    uttest_OM_AcpuMsgProc_case1();
}


TEST(OM_AcpuMsgProc2, UT)
{
    uttest_OM_AcpuMsgProc_case2();
}


TEST(OM_AcpuMsgProc3, UT)
{
#if 0
    MOCKER(V_StopRelTimer)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    uttest_OM_AcpuMsgProc_case3();

    GlobalMockObject::reset();
#endif
}



TEST(OM_AcpuMsgProc4, UT)
{
#if 0
    uttest_OM_AcpuMsgProc_case4();
#endif
}


TEST(OM_AcpuMsgProc5, UT)
{
    uttest_OM_AcpuMsgProc_case5();
}


TEST(OM_AcpuMsgProc6, UT)
{
    unsigned char                       aucData[100];

    MOCKER(V_AllocMsg)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(V_SendMsg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(NV_WriteEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(NV_ReadEx)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    uttest_OM_AcpuMsgProc_case6();

    GlobalMockObject::reset();
}


TEST(OM_AcpuMsgProc7, UT)
{
    uttest_OM_AcpuMsgProc_case7();
}


TEST(OM_AcpuMsgProc8, UT)
{
    uttest_OM_AcpuMsgProc_case8();
}


TEST(OM_AcpuMsgProc9, UT)
{
    uttest_OM_AcpuMsgProc_case9();
}


TEST(OM_AcpuMsgProc10, UT)
{
    uttest_OM_AcpuMsgProc_case10();
}


TEST(OM_AcpuMsgProc11, UT)
{
    uttest_OM_AcpuMsgProc_case11();
}


TEST(OM_AcpuMsgProc12, UT)
{
    uttest_OM_AcpuMsgProc_case12();
}


TEST(OM_AcpuMsgProc13, UT)
{
    uttest_OM_AcpuMsgProc_case13();
}



TEST(OM_AcpuMsgProc14, UT)
{
    uttest_OM_AcpuMsgProc_case14();
}

TEST(OM_AcpuErrLogHook_case1, UT)
{
    unsigned char                       aucData[100];

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue(VOS_NULL_PTR));
    uttest_OM_AcpuErrLogHook_case1();
}


TEST(OM_AcpuErrLogHook_case2, UT)
{
    unsigned char                       aucData[100];

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void*)&aucData));
    uttest_OM_AcpuErrLogHook_case1();
}