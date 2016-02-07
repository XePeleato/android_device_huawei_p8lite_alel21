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
	extern int OM_RingBufferFreeBytes( void * ringId);
	extern int OM_RingBufferPut( void * rngId, char *buffer, int nbytes );
	extern unsigned int VOS_SmV( unsigned long Sm_ID );
	extern unsigned int uttest_SCM_SoftDecodeDataRcv_case1(void);
	extern unsigned int uttest_SCM_SoftDecodeDataRcv_case2(void);
	extern unsigned int uttest_SCM_SoftDecodeDataRcv_case3(void);
	extern unsigned int uttest_SCM_SoftDecodeCfgDataRcv_case1(void);
	extern unsigned int uttest_SCM_SoftDecodeAcpuRcvData_case1(void);
	extern unsigned int uttest_SCM_SoftDecodeCfgHdlcInit_case1(void);
	extern unsigned int Om_HdlcDecap( void *pstHdlc, char ucChar );
	extern void spin_lock(void *lock);
	extern void spin_unlock(void *lock);
	extern void spin_lock_init(void *lock);
	extern unsigned int SCM_SoftDecodeCfgDataRcv(unsigned char *pucBuffer, unsigned int ulLen);
	extern void * V_MemAlloc( unsigned int ulInfo, unsigned char ucPtNo, unsigned int ulSize,
		unsigned int ulRebootFlag, unsigned int ulFileID, int usLineNo  );
	extern void Om_HdlcInit( void *pstHdlc );
	extern unsigned int uttest_SCM_SoftDecodeCfgRcvTaskInit_case1(void);
	extern unsigned char VOS_RegisterSelfTaskPrio( unsigned int             ulFID ,
		void *       pfnSelfTask,
		unsigned int                ulTaskRealPri,
		unsigned int                ulStackSize);

	extern void * V_MemSet( void * ToSet, unsigned char Char, unsigned int Count,
		unsigned int ulFileID, int usLineNo );

	extern unsigned int  VOS_SmCCreate( unsigned char acSmName[4],
		unsigned int ulSmInit,
		unsigned int ulFlags,
		void  *pSmID );

	extern void * OM_RingBufferCreate( int nbytes );

	extern unsigned int SCM_SoftDecodeDataRcv(void *pucBuffer, unsigned int ulLen);

	extern void uttest_SCM_RcvDataDispatch_case1(void);

	extern void uttest_SCM_RcvDataDispatch_case2(void);
    
	extern void SCM_RcvDataDispatch(
		void                       *pstHdlcCtrl,
		unsigned char               ucDataType);

	extern unsigned int SCM_SoftDecodeCfgHdlcInit(void *pstHdlc);
	extern unsigned int V_MemFree( unsigned int ulInfo, void **ppAddr,
		unsigned int ulFileID, int usLineNo );
#ifdef __cplusplus
}
#endif

#ifndef VOS_OK
#define VOS_OK                          0
#endif

#ifndef VOS_ERR
#define VOS_ERR                         1
#endif



TEST(SCM_SoftDecodeCfgDataRcv1, UT)
{
	MOCKER(spin_lock)
		.expects(once());

	MOCKER(spin_unlock)
		.expects(once());

	MOCKER(SCM_SoftDecodeDataRcv)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_SCM_SoftDecodeCfgDataRcv_case1());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeDataRcv1, UT)
{
	MOCKER(OM_RingBufferFreeBytes)
		.stubs()
		.will(returnValue((unsigned int)500));

	EXPECT_EQ(VOS_ERR, uttest_SCM_SoftDecodeDataRcv_case1());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeDataRcv2, UT)
{
	MOCKER(OM_RingBufferFreeBytes)
		.stubs()
		.will(returnValue((unsigned int)1024));

	MOCKER(OM_RingBufferPut)
		.stubs()
		.will(returnValue((unsigned int)500));

	EXPECT_EQ(VOS_ERR, uttest_SCM_SoftDecodeDataRcv_case2());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeDataRcv3, UT)
{
	MOCKER(OM_RingBufferFreeBytes)
		.stubs()
		.will(returnValue((unsigned int)1024));

	MOCKER(OM_RingBufferPut)
		.stubs()
		.will(returnValue((unsigned int)1000));

	MOCKER(VOS_SmV)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_SCM_SoftDecodeDataRcv_case3());

	GlobalMockObject::reset();
}


TEST(SCM_RcvDataDispatch1, UT)
{
	uttest_SCM_RcvDataDispatch_case1();

	GlobalMockObject::reset();
}


TEST(SCM_RcvDataDispatch2, UT)
{
	uttest_SCM_RcvDataDispatch_case2();

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeAcpuRcvData1, UT)
{
	MOCKER(Om_HdlcDecap)
		.stubs()
		.will(returnValue((unsigned int)2));

	MOCKER(Om_HdlcDecap)
		.stubs()
		.will(returnValue((unsigned int)1));

	MOCKER(Om_HdlcDecap)
		.stubs()
		.will(returnValue((unsigned int)0));

	MOCKER(SCM_RcvDataDispatch)
		.expects(once());

	EXPECT_EQ(VOS_OK, uttest_SCM_SoftDecodeAcpuRcvData_case1());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeCfgHdlcInit1, UT)
{
	MOCKER(V_MemAlloc)
		.stubs()
		.will(returnValue((void *)0));

	EXPECT_EQ(VOS_ERR, uttest_SCM_SoftDecodeCfgHdlcInit_case1());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeCfgHdlcInit2, UT)
{
	char  aucData[100];

	MOCKER(V_MemAlloc)
		.stubs()
		.will(returnValue((void *)aucData));

	MOCKER(Om_HdlcInit)
		.expects(once());

	EXPECT_EQ(VOS_OK, uttest_SCM_SoftDecodeCfgHdlcInit_case1());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeCfgRcvTaskInit1, UT)
{
	MOCKER(VOS_RegisterSelfTaskPrio)
		.stubs()
		.will(returnValue((unsigned int)0xff));

	EXPECT_EQ(VOS_ERR, uttest_SCM_SoftDecodeCfgRcvTaskInit_case1());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeCfgRcvTaskInit2, UT)
{
	MOCKER(VOS_RegisterSelfTaskPrio)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(V_MemSet)
		.stubs()
		.will(returnValue((void *)0));

	MOCKER(VOS_SmCCreate)
		.stubs()
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_SCM_SoftDecodeCfgRcvTaskInit_case1());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeCfgRcvTaskInit3, UT)
{
	MOCKER(VOS_RegisterSelfTaskPrio)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(V_MemSet)
		.stubs()
		.will(returnValue((void *)0));

	MOCKER(VOS_SmCCreate)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(SCM_SoftDecodeCfgHdlcInit)
		.stubs()
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_SCM_SoftDecodeCfgRcvTaskInit_case1());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeCfgRcvTaskInit4, UT)
{
	MOCKER(VOS_RegisterSelfTaskPrio)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(V_MemSet)
		.stubs()
		.will(returnValue((void *)0));

	MOCKER(VOS_SmCCreate)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));
		

	MOCKER(SCM_SoftDecodeCfgHdlcInit)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(OM_RingBufferCreate)
		.stubs()
		.will(returnValue((void *)0));

	MOCKER(V_MemFree)
		.expects(once())
	    .will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_ERR, uttest_SCM_SoftDecodeCfgRcvTaskInit_case1());

	GlobalMockObject::reset();
}


TEST(SCM_SoftDecodeCfgRcvTaskInit5, UT)
{
	char     cBuf[100];

	MOCKER(VOS_RegisterSelfTaskPrio)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(V_MemSet)
		.stubs()
		.will(returnValue((void *)0));

	MOCKER(VOS_SmCCreate)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(SCM_SoftDecodeCfgHdlcInit)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(OM_RingBufferCreate)
		.stubs()
		.will(returnValue((void *)cBuf));

	MOCKER(spin_lock_init)
		.expects(once());

	EXPECT_EQ(VOS_OK, uttest_SCM_SoftDecodeCfgRcvTaskInit_case1());

	GlobalMockObject::reset();
}