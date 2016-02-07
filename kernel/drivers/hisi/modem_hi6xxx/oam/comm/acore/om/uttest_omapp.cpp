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

    extern int uttest_OM_AcpuRegisterReqProc_case3(void);
    extern int uttest_OM_AcpuRegisterReqProc_case4(void);
    extern unsigned int uttest_OM_AcpuSendDataMNTNChannel_case1(void);
    extern unsigned int uttest_OM_AcpuSendDataMNTNChannel_case2(void);
    extern unsigned int uttest_OM_AcpuSendDataMNTNChannel_case3(void);
    extern unsigned int uttest_OM_AcpuSendDataMNTNChannel_case4(void);
    extern unsigned int uttest_OM_AcpuSendDataMNTNChannel_case5(void);
    extern unsigned int uttest_OM_AcpuSendDataCbtChannel_case1(void);
    extern unsigned int uttest_OM_AcpuSendDataCbtChannel_case2(void);
    extern unsigned int uttest_OM_AcpuSendDataCbtChannel_case3(void);
    extern unsigned int uttest_OM_AcpuSendDataCbtChannel_case4(void);
    extern unsigned int uttest_OM_AcpuSendDataCbtChannel_case5(void);
    extern unsigned int uttest_OM_AcpuSendDataChannel_case1(void);
    extern unsigned int uttest_OM_AcpuSendDataChannel_case2(void);
    extern unsigned int uttest_OM_AcpuSendDataChannel_case3(void);
    extern unsigned int uttest_OM_AcpuSendDataChannel_case4(void);
    extern void uttest_OM_AcpuSendResultChannel_case1(void);
    extern void uttest_OM_AcpuSendResult_case1(void);
    extern unsigned int uttest_OM_AcpuSendData_case1(void);
    extern void uttest_OM_AcpuSendContentChannel_case1(void);
    extern void uttest_OM_AcpuSendContent_case1(void);
    extern void uttest_OM_AcpuConnecInfoTimeOutMsgProc_case1(void);
    extern void uttest_OM_AcpuAtMsgProc_case1(void);
    extern void uttest_OM_AcpuAtMsgProc_case2(void);
    extern void uttest_OM_AcpuMsgProc_case1(void);
    extern unsigned int uttest_OM_AcpuInit_case1(void);
    extern void uttest_OM_AutoConfigProc_case1(void);
    extern unsigned int uttest_OM_AcpuPidInit_case1(void);
    extern unsigned int uttest_OM_AcpuFidInit_case1(void);
    extern unsigned int uttest_OM_AcpuFidInit_case2(void);
    extern unsigned int uttest_OM_AcpuFidInit_case3(void);

    extern int OM_AcpuRegisterReqProc(unsigned char ucModuleId, void *pReqPro);
    extern unsigned int VOS_CheckInterrupt(void);
    extern unsigned int OMRL_AcpuMsgSplit(void *pstCtrlInfo, unsigned char *pucMsg, unsigned short usMsgLen);
    extern unsigned int OM_AcpuSendDataMNTNChannel(void *pstCtrlInfo, void *pucMsg, unsigned short usMsgLen);
    extern unsigned int OM_AcpuSendDataCbtChannel(void *pucMsg, unsigned short usMsgLen);
    extern unsigned int OM_AcpuSendDataChannel(unsigned int enChannel, void *pucMsg, unsigned short usMsgLen);
    extern void OM_AcpuSendResultChannel(unsigned int enChannel, unsigned char ucFuncType, unsigned int ulResult, unsigned short usReturnPrimId);
    extern void OM_AcpuSendContentChannel(unsigned int enChannel, unsigned char ucFuncType, void *pstOmToAppMsg, unsigned short usReturnPrimId);
    extern void PPM_HsicConnectProc(void);
    extern void PPM_HsicDisconnectProc(void);
    extern void *V_MemAlloc(unsigned int ulInfo, unsigned char ucPtNo, unsigned int ulSize,
                        unsigned int ulRebootFlag, unsigned int ulFileID, int usLineNo);
    extern unsigned int V_MemFree(unsigned int ulInfo, void **ppAddr,
                          unsigned int ulFileID, int usLineNo);
    extern unsigned int mdrv_rtc_get_value(void);
    extern unsigned int OMRL_AcpuSendCbtData(unsigned char *pucSrc, unsigned short usSrcLen);
    extern void OM_RecordInfoStart(unsigned int enNumber, unsigned int ulSendPid, unsigned int ulRcvPid, unsigned int ulMsgName);
    extern void OM_RecordInfoEnd(unsigned int enNumber);
    extern void OM_AcpuErrLogMsgProc(void* pMsg);
    extern void OM_LogId1(unsigned int enModuleId, unsigned int enSubModId,
                       unsigned int enLevel, unsigned int  ulLogId,
                       int lPara1);
    extern unsigned int VOS_SmMCreate(unsigned char Sm_Name[4], unsigned int Flags, void *Sm_ID);
    extern unsigned int VOS_SmBCreate(unsigned char Sm_Name[4], unsigned int Flags, void *Sm_ID);
    extern unsigned int OMRL_AcpuInit(void);
    extern void OM_AcpuRegisterDrvCB(void);
    extern unsigned int OM_PrintfInit(void);
    extern unsigned int OM_AcpuStartEventTimer(void);
    extern unsigned int  OM_AcpuICCInfoInit(void);
    extern void OM_AcpuInitAuthVariable(void);
    extern unsigned int NV_ReadEx(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int uttest_NV_ReadEx_stub10(unsigned short enModemID, unsigned short usID, void *pItem, unsigned int ulLength);
    extern unsigned int GU_OamSndPcMsgToCcpu(void *pstCtrlInfo, unsigned char *pucData, unsigned int ulSize);
    extern unsigned int OM_AcpuGetLogPath(char *pucBuf, char *pucOldPath, char *pucUnitaryPath);
    extern void DRV_MSP_PROC_REG(unsigned int eFuncID, void *pFunc);
    extern void CBTPPM_OamCbtPortInit(void);
    extern unsigned int COMM_Init(void);
    extern unsigned int VOS_RegisterPIDInfo(unsigned int ulPID, void* pfnInitFun, void* pfnMsgFun);
    extern unsigned int VOS_RegisterMsgTaskPrio(unsigned int ulFID, unsigned int TaskPrio);
    extern unsigned int OM_AcpuSelfTaskReg(void);
    extern int ccorereset_regcbfunc(const char *pname, void* pcbfun, int userdata, int priolevel);
    extern unsigned int CBTSCM_SoftDecodeReqRcvTaskInit(void);
    extern unsigned int SCM_SoftDecodeCfgRcvTaskInit(void);

#ifdef __cplusplus
}
#endif

#ifndef VOS_OK
#define VOS_OK                          0
#endif

#ifndef VOS_ERR
#define VOS_ERR                         1
#endif

#ifndef VOS_TRUE
#define VOS_TRUE                        1
#endif

#ifndef VOS_FALSE
#define VOS_FALSE                       0
#endif

#ifndef VOS_NULL_PTR
#define VOS_NULL_PTR                       0
#endif


TEST(OM_AcpuRegisterReqProc1, UT)
{
	unsigned char ucModuleId = 35;

	EXPECT_EQ(VOS_ERR, OM_AcpuRegisterReqProc(ucModuleId, 0));
}



TEST(OM_AcpuRegisterReqProc2, UT)
{
	unsigned char ucModuleId = 65;

	EXPECT_EQ(VOS_ERR, OM_AcpuRegisterReqProc(ucModuleId, 0));
}


TEST(OM_AcpuRegisterReqProc3, UT)
{
	EXPECT_EQ(VOS_ERR, uttest_OM_AcpuRegisterReqProc_case3());
}


TEST(OM_AcpuRegisterReqProc4, UT)
{
	EXPECT_EQ(VOS_OK, uttest_OM_AcpuRegisterReqProc_case4());
}


TEST(OM_AcpuSendDataMNTNChannel1, UT)
{
	EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataMNTNChannel_case1());
}


TEST(OM_AcpuSendDataMNTNChannel2, UT)
{
	EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataMNTNChannel_case2());
}


TEST(OM_AcpuSendDataMNTNChannel3, UT)
{
    MOCKER(VOS_CheckInterrupt)
        .stubs()
        .will(returnValue((unsigned int)VOS_TRUE));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataMNTNChannel_case3());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataMNTNChannel4, UT)
{
    MOCKER(VOS_CheckInterrupt)
        .stubs()
        .will(returnValue((unsigned int)VOS_FALSE));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataMNTNChannel_case4());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataMNTNChannel5, UT)
{
    MOCKER(VOS_CheckInterrupt)
        .stubs()
        .will(returnValue((unsigned int)VOS_FALSE));

    MOCKER(OMRL_AcpuMsgSplit)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataMNTNChannel_case5());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataCbtChannel1, UT)
{
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataCbtChannel_case1());
}


TEST(OM_AcpuSendDataCbtChannel2, UT)
{
    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void*)VOS_NULL_PTR));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataCbtChannel_case2());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataCbtChannel3, UT)
{
    unsigned char                       aucData[1024];

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(mdrv_rtc_get_value)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OMRL_AcpuSendCbtData)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(V_MemFree)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataCbtChannel_case3());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataCbtChannel4, UT)
{
    unsigned char                       aucData[10240];

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(mdrv_rtc_get_value)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OMRL_AcpuSendCbtData)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(V_MemFree)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataCbtChannel_case4());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataCbtChannel5, UT)
{
    unsigned char                       aucData[10240];

    MOCKER(V_MemAlloc)
        .stubs()
        .will(returnValue((void*)&aucData[0]));

    MOCKER(mdrv_rtc_get_value)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OMRL_AcpuSendCbtData)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(V_MemFree)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataCbtChannel_case5());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataChannel1, UT)
{
    MOCKER(OM_AcpuSendDataMNTNChannel)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataChannel_case1());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataChannel2, UT)
{
    MOCKER(OM_AcpuSendDataMNTNChannel)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataChannel_case2());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataChannel3, UT)
{
    MOCKER(OM_AcpuSendDataCbtChannel)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataChannel_case3());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendDataChannel4, UT)
{
    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendDataChannel_case4());
}


TEST(OM_AcpuSendResultChannel1, UT)
{
    MOCKER(OM_AcpuSendDataChannel)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    uttest_OM_AcpuSendResultChannel_case1();

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendResult1, UT)
{
    MOCKER(OM_AcpuSendResultChannel)
        .expects(once());

    uttest_OM_AcpuSendResult_case1();

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendData1, UT)
{
    MOCKER(OM_AcpuSendDataChannel)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuSendData_case1());

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendContentChannel1, UT)
{
    MOCKER(OM_AcpuSendDataChannel)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    uttest_OM_AcpuSendContentChannel_case1();

    GlobalMockObject::reset();
}


TEST(OM_AcpuSendContent1, UT)
{
    MOCKER(OM_AcpuSendContentChannel)
        .expects(once());

    uttest_OM_AcpuSendContent_case1();

    GlobalMockObject::reset();
}


TEST(OM_AcpuConnecInfoTimeOutMsgProc1, UT)
{
    uttest_OM_AcpuConnecInfoTimeOutMsgProc_case1();
}


TEST(OM_AcpuAtMsgProc1, UT)
{
    MOCKER(PPM_HsicConnectProc)
        .expects(once());

    uttest_OM_AcpuAtMsgProc_case1();

    GlobalMockObject::reset();
}


TEST(OM_AcpuAtMsgProc2, UT)
{
    MOCKER(PPM_HsicDisconnectProc)
        .expects(once());

    uttest_OM_AcpuAtMsgProc_case2();

    GlobalMockObject::reset();
}


TEST(OM_AcpuMsgProc1, UT)
{
    MOCKER(OM_RecordInfoStart)
        .expects(once());

    MOCKER(OM_AcpuErrLogMsgProc)
        .expects(once());

    MOCKER(OM_LogId1)
            .expects(once());

    MOCKER(OM_RecordInfoEnd)
        .expects(once());

    uttest_OM_AcpuMsgProc_case1();

    GlobalMockObject::reset();
}


TEST(OM_AcpuInit1, UT)
{
    MOCKER(VOS_SmMCreate)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(VOS_SmBCreate)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OMRL_AcpuInit)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuRegisterDrvCB)
        .expects(once());

    MOCKER(OM_PrintfInit)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuStartEventTimer)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuICCInfoInit)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuGetLogPath)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuInitAuthVariable)
        .expects(once());

    uttest_OM_AcpuInit_case1();

    GlobalMockObject::reset();
}


TEST(OM_AutoConfigProc1, UT)
{
    MOCKER(NV_ReadEx)
        .stubs()
        .will(invoke(uttest_NV_ReadEx_stub10));

    MOCKER(GU_OamSndPcMsgToCcpu)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuGetLogPath)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    uttest_OM_AutoConfigProc_case1();

    GlobalMockObject::reset();
}


TEST(OM_AcpuPidInit1, UT)
{
    MOCKER(DRV_MSP_PROC_REG)
        .expects(once());

    MOCKER(CBTPPM_OamCbtPortInit)
        .expects(once());

    MOCKER(COMM_Init)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuPidInit_case1());

    GlobalMockObject::reset();
}


TEST(OM_AcpuFidInit1, UT)
{
    MOCKER(VOS_RegisterPIDInfo)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(VOS_RegisterMsgTaskPrio)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuSelfTaskReg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(ccorereset_regcbfunc)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(CBTSCM_SoftDecodeReqRcvTaskInit)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuFidInit_case1());

    GlobalMockObject::reset();
}


TEST(OM_AcpuFidInit2, UT)
{
    MOCKER(VOS_RegisterPIDInfo)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(VOS_RegisterMsgTaskPrio)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuSelfTaskReg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(ccorereset_regcbfunc)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(CBTSCM_SoftDecodeReqRcvTaskInit)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(SCM_SoftDecodeCfgRcvTaskInit)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuFidInit_case2());

    GlobalMockObject::reset();
}


TEST(OM_AcpuFidInit3, UT)
{
    MOCKER(VOS_RegisterPIDInfo)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(VOS_RegisterMsgTaskPrio)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_AcpuSelfTaskReg)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(ccorereset_regcbfunc)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(CBTSCM_SoftDecodeReqRcvTaskInit)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(SCM_SoftDecodeCfgRcvTaskInit)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_OM_AcpuFidInit_case3());

    GlobalMockObject::reset();
}

