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
    extern void * V_MemAlloc( unsigned int ulInfo, unsigned char  ucPtNo, unsigned int ulSize,
        unsigned int ulRebootFlag, unsigned int ulFileID, int usLineNo  );
    extern int BSP_ICC_Open(unsigned int u32ChanId, void *pChanAttr);
    extern void BSP_MNTN_SystemError(int modId, int arg1, int arg2, char * arg3, int arg3Length);
    extern FILE *BSP_fopen(const char *path, const char *mode);
    extern int   BSP_fwrite(void *ptr, unsigned int size, unsigned int number, FILE *stream);
    extern int   BSP_fclose(FILE *fp);
    extern void * V_AllocMsg(unsigned int Pid, unsigned int ulLength,
        unsigned int ulFileID, int usLineNo );
    extern unsigned int V_SendMsg(unsigned int Pid, void **ppMsg,
        unsigned int ulFileID, int lLineNo);
    extern void SCM_LogToFile(FILE *fp);
    extern void SOCP_LogToFile(FILE *fp);
    extern unsigned int VOS_GetSlice(void);
    extern unsigned int uttest_GU_OamSndPcMsgToCcpu_case1(void);
    extern unsigned int uttest_GU_OamSndPcMsgToCcpu_case2(void);
    extern unsigned int uttest_GU_OamSndPcMsgToCcpu_case3(void);
    extern int BSP_ICC_Write(unsigned int u32ChanId, unsigned char* pData, int s32Size);
    extern void uttest_GU_OamErrLogVComPortInit_case1(void);
    extern unsigned int uttest_GU_OamAcpu_ICCError_CB_case1(void);
    extern unsigned int uttest_GU_OamAcpu_ICCError_CB_case2(void);
    extern unsigned int uttest_GU_OamAcpu_ICCError_CB_case3(void);
    extern unsigned int uttest_OM_ComRx_ICC_Init_case1(void);
    extern unsigned int uttest_OM_ComRx_ICC_Init_case2(void);
    extern unsigned int uttest_OM_ComRx_ICC_Init_case3(void);
    extern unsigned int uttest_GU_OmAcpuSwitchOnOff_GetFlag(void);
    extern unsigned int uttest_GU_OmAcpuSwitchOnOff_case1(void);
    extern void uttest_OM_AcpuLogShowToFile_case1(void);
    extern void uttest_OM_AcpuLogShowToFile_case2(void);
    extern void uttest_OM_AcpuLogShowToFile_case3(void);
    extern void uttest_OM_AcpuLogShowToFile_case4(void);
    extern void uttest_OmAcpuPortInfoShow_case1(void);

#ifdef __cplusplus
}
#endif

#ifndef VOS_OK
#define VOS_OK                          0
#endif

#ifndef VOS_ERR
#define VOS_ERR                         1
#endif

#ifndef BSP_ERR_ICC_BASE
#define BSP_ERR_ICC_BASE                (int)(0x80000000 | 0x10180000)
#endif

#ifndef BSP_ERR_ICC_CCORE_RESETTING
#define BSP_ERR_ICC_CCORE_RESETTING     (BSP_ERR_ICC_BASE + 0x12)
#endif

#ifndef USB_MAX_DATA_LEN
#define USB_MAX_DATA_LEN                (60*1024)   /*USB发送的最大数据长度*/
#endif

#ifndef VOS_ERROR
#define VOS_ERROR                       (-1)
#endif
TEST(GU_OamSndPcMsgToCcpu1, UT)
{
    MOCKER(BSP_ICC_Write)
        .stubs()
        .will(returnValue(BSP_ERR_ICC_CCORE_RESETTING));

    EXPECT_EQ(VOS_ERR, uttest_GU_OamSndPcMsgToCcpu_case1());

    GlobalMockObject::reset();
}


TEST(GU_OamSndPcMsgToCcpu2, UT)
{
    MOCKER(BSP_ICC_Write)
        .stubs()
        .will(returnValue(USB_MAX_DATA_LEN));

    MOCKER(VOS_GetSlice)
        .stubs()
        .will(returnValue((unsigned int)1));

    EXPECT_EQ(VOS_ERR,  uttest_GU_OamSndPcMsgToCcpu_case2());

    GlobalMockObject::reset();
}


TEST(GU_OamSndPcMsgToCcpu3, UT)
{
    MOCKER(BSP_ICC_Write)
        .stubs()
        .will(returnValue(USB_MAX_DATA_LEN + 1));

    MOCKER(VOS_GetSlice)
        .stubs()
        .will(returnValue((unsigned int)1));

    EXPECT_EQ(VOS_OK,  uttest_GU_OamSndPcMsgToCcpu_case3());

    GlobalMockObject::reset();
}


TEST(GU_OamErrLogVComPortInit1, UT)
{
    uttest_GU_OamErrLogVComPortInit_case1();

    GlobalMockObject::reset();
}


TEST(GU_OamAcpu_ICCError_CB1, UT)
{
    EXPECT_EQ(VOS_OK, uttest_GU_OamAcpu_ICCError_CB_case1());

    GlobalMockObject::reset();
}

#if 0

TEST(GU_OamAcpu_ICCError_CB2, UT)
{
    EXPECT_EQ(VOS_OK, uttest_GU_OamAcpu_ICCError_CB_case2());

    GlobalMockObject::reset();
}
#endif


TEST(GU_OamAcpu_ICCError_CB3, UT)
{
    EXPECT_EQ(VOS_ERR, uttest_GU_OamAcpu_ICCError_CB_case3());

    GlobalMockObject::reset();
}


TEST(OM_ComRx_ICC_Init1, UT)
{
    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void *)0));

    EXPECT_EQ(VOS_ERR, uttest_OM_ComRx_ICC_Init_case1());

    GlobalMockObject::reset();
}


TEST(OM_ComRx_ICC_Init2, UT)
{
    unsigned char   aucICCAttr0[44];
    unsigned char   aucICCAttr1[44];

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnObjectList ((void *)aucICCAttr0, (void *)aucICCAttr1));

    MOCKER(BSP_ICC_Open)
        .stubs()
        .will(returnValue((int)VOS_ERROR));

    MOCKER(BSP_MNTN_SystemError)
        .expects(exactly(1));

    EXPECT_EQ(VOS_ERR, uttest_OM_ComRx_ICC_Init_case2());

    GlobalMockObject::reset();
}


TEST(OM_ComRx_ICC_Init3, UT)
{
    unsigned char   aucICCAttr0[44];
    unsigned char   aucICCAttr1[44];

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnObjectList ((void *)aucICCAttr0, (void *)aucICCAttr1));

    MOCKER(BSP_ICC_Open)
        .stubs()
        .will(returnValue((int)VOS_OK));

    MOCKER(BSP_MNTN_SystemError)
        .expects(exactly(0));

    EXPECT_EQ(VOS_OK, uttest_OM_ComRx_ICC_Init_case3());

    GlobalMockObject::reset();
}


TEST(GU_OmAcpuSwitchOnOff1, UT)
{
    uttest_GU_OmAcpuSwitchOnOff_case1();

    EXPECT_EQ(0, uttest_GU_OmAcpuSwitchOnOff_GetFlag());

    GlobalMockObject::reset();
}


TEST(OM_AcpuLogShowToFile1, UT)
{
    MOCKER(BSP_fopen)
        .stubs()
        .will(returnValue((FILE *)0));

    MOCKER(BSP_fwrite)
        .expects(exactly(0));

    uttest_OM_AcpuLogShowToFile_case1();

    GlobalMockObject::reset();
}


TEST(OM_AcpuLogShowToFile2, UT)
{
    MOCKER(BSP_fopen)
        .stubs()
        .will(returnValue((FILE *)1));

    MOCKER(BSP_fwrite)
        .stubs()
        .will(returnValue((int)0));

    MOCKER(SCM_LogToFile)
        .expects(exactly(1));

    MOCKER(SOCP_LogToFile)
        .expects(exactly(1));

    MOCKER(BSP_fclose)
        .expects(exactly(1))
        .will(returnValue((int)VOS_OK));

    MOCKER(V_AllocMsg)
        .expects(exactly(0))
        .will(returnValue((void *)0));

    MOCKER(V_SendMsg)
        .expects(exactly(0))
        .will(returnValue((unsigned int)VOS_OK));

    uttest_OM_AcpuLogShowToFile_case2();

    GlobalMockObject::reset();
}


TEST(OM_AcpuLogShowToFile3, UT)
{
    MOCKER(BSP_fopen)
        .stubs()
        .will(returnValue((FILE *)1));

    MOCKER(BSP_fwrite)
        .stubs()
        .will(returnValue((int)0));

    MOCKER(SCM_LogToFile)
        .expects(exactly(1));

    MOCKER(SOCP_LogToFile)
        .expects(exactly(1));

    MOCKER(BSP_fclose)
        .expects(exactly(1))
        .will(returnValue((int)VOS_OK));

    MOCKER(V_AllocMsg)
        .expects(exactly(1))
        .will(returnValue((void *)0));

    MOCKER(V_SendMsg)
        .expects(exactly(0))
        .will(returnValue((unsigned int)VOS_OK));

    uttest_OM_AcpuLogShowToFile_case3();

    GlobalMockObject::reset();
}


TEST(OM_AcpuLogShowToFile4, UT)
{
    unsigned char aucSendCnf[24];

    MOCKER(BSP_fopen)
        .stubs()
        .will(returnValue((FILE *)1));

    MOCKER(BSP_fwrite)
        .stubs()
        .will(returnValue((int)0));

    MOCKER(SCM_LogToFile)
        .expects(exactly(1));

    MOCKER(SOCP_LogToFile)
        .expects(exactly(1));

    MOCKER(BSP_fclose)
        .expects(exactly(1))
        .will(returnValue((int)VOS_OK));

    MOCKER(V_AllocMsg)
        .expects(exactly(1))
        .will(returnValue((void *)aucSendCnf));

    MOCKER(V_SendMsg)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    uttest_OM_AcpuLogShowToFile_case4();

    GlobalMockObject::reset();
}


TEST(OmAcpuPortInfoShow1, UT)
{
    uttest_OmAcpuPortInfoShow_case1();

    GlobalMockObject::reset();
}