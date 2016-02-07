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
extern void * V_AllocMsg(void);
extern unsigned int V_SendMsg(void);
extern unsigned int CPM_QueryPhyPort(unsigned int enLogicPort);
extern unsigned int PPM_DisconnectGUPort(unsigned int enChannel);
extern unsigned int PPM_DisconnectTLPort(void);
extern void PPM_DisconnectAllPort(unsigned int enChannel);
extern unsigned int OM_GetSlice(void);
extern void BSP_SOCP_SetLogIntTimeOut(void);
extern void BSP_SOCP_SetLogIntTimeOutDefautl(void);
extern int udi_close(unsigned int handle);
extern int udi_ioctl(unsigned int handle, unsigned int u32Cmd, void* pParam);
extern int udi_open(void *pParam);
extern unsigned int CPM_ComRcv(unsigned int enPhyPort, char *pucData, unsigned int ulLen);
extern unsigned int VOS_SmP(unsigned int Sm_ID, unsigned int  ulTimeOutInMillSec);
extern unsigned int VOS_SmV(unsigned int Sm_ID);
extern unsigned long _raw_spin_lock_irqsave(void *lock);
extern void _raw_spin_unlock_irqrestore(void *lock, unsigned long flags);
extern void CPM_ConnectPorts(unsigned int enPhyPort, unsigned int enLogicPort);
extern void CPM_DisconnectPorts(unsigned int enPhyPort, unsigned int enLogicPort);
extern unsigned int NV_WriteEx(unsigned short enModemID,unsigned short usID,void *pItem,unsigned int ulLength);
extern unsigned int PPM_UdiRegCallBackFun(unsigned int enHandle, unsigned int ulCmdType, void* pFunc);
extern void PPM_PortPseudoSyncGetSmp(unsigned int enHandle);
extern unsigned int OM_AcpuGetLogPath(char *pucBuf, char *pucOldPath, char *pucUnitaryPath);
extern unsigned int OM_Acpu_WriteLogFile(char * cFileName, void *pRecord, unsigned int ulLen);
extern void BSP_MNTN_SystemError(int modId, int arg1, int arg2, char * arg3, int arg3Length);
extern unsigned int PPM_LogFileInit(void);
extern unsigned int PPM_PortInit(void);
extern unsigned int PPM_SockPortInit(void);
extern unsigned int PPM_UsbPortInit(void);
extern void PPM_HsicPortInit(void);
extern void PPM_VComPortInit(void);
extern unsigned int uttest_PPM_DisconnectGUPort_case1(void);
extern unsigned int uttest_PPM_DisconnectGUPort_case2(void);
extern unsigned int uttest_PPM_DisconnectTLPort_case1(void);
extern unsigned int uttest_PPM_DisconnectTLPort_case2(void);
extern void uttest_PPM_DisconnectAllPort_case1(void);
extern void uttest_PPM_GetSendDataLen_case1(void);
extern void uttest_PPM_GetSendDataLen_case2(void);
extern void uttest_PPM_PortStatus_case1(void);
extern void uttest_PPM_PortStatus_case2(void);
extern void uttest_PPM_PortStatus_case3(void);
extern void uttest_PPM_PortStatus_case4(void);
extern void uttest_PPM_PortStatus_case5(void);
extern void uttest_PPM_PortCloseProc_case1(void);
extern void uttest_PPM_PortCloseProc_case2(void);
extern void uttest_PPM_PortCloseProc_case3(void);
extern void uttest_PPM_PortCloseProc_case4(void);
extern int uttest_PPM_ReadPortData_case1(void);
extern int uttest_PPM_ReadPortData_case2(void);
extern int uttest_PPM_ReadPortData_case3(void);
extern void uttest_PPM_PortPseudoSyncGetSmp_case1(void);
extern void uttest_PPM_PortPseudoSyncGetSmp_case2(void);
extern void uttest_PPM_PortPseudoSyncGetSmp_case3(void);
extern unsigned int uttest_PPM_QueryLogPort_case1(void);
extern unsigned int uttest_PPM_QueryLogPort_case2(void);
extern unsigned int uttest_PPM_QueryLogPort_case3(void);
extern unsigned int uttest_PPM_LogPortSwitch_case1(void);
extern unsigned int uttest_PPM_LogPortSwitch_case2(void);
extern unsigned int uttest_PPM_LogPortSwitch_case3(void);
extern unsigned int uttest_PPM_LogPortSwitch_case4(void);
extern unsigned int uttest_PPM_UdiRegCallBackFun_case1(void);
extern unsigned int uttest_PPM_UdiRegCallBackFun_case2(void);
extern unsigned int uttest_PPM_UdiRegCallBackFun_case3(void);
extern void uttest_PPM_ReadPortDataInit_case1(void);
extern void uttest_PPM_ReadPortDataInit_case2(void);
extern void uttest_PPM_ReadPortDataInit_case3(void);
extern void uttest_PPM_ReadPortDataInit_case4(void);
extern void uttest_PPM_ReadPortDataInit_case5(void);
extern void uttest_PPM_ReadPortDataInit_case6(void);
extern void uttest_PPM_ReadPortDataInit_case7(void);
extern void uttest_PPM_ReadPortDataInit_case8(void);
extern void uttest_PPM_ReadPortDataInit_case9(void);
extern void uttest_PPM_ReadPortDataInit_case10(void);
extern unsigned int uttest_PPM_PortSend_case1(void);
extern unsigned int uttest_PPM_PortSend_case2(void);
extern unsigned int uttest_PPM_PortSend_case3(void);
extern unsigned int uttest_PPM_PortSend_case4(void);
extern unsigned int uttest_PPM_PortSend_case5(void);
extern unsigned int uttest_PPM_PortSend_case6(void);
extern void uttest_PPM_PortWriteAsyCB_case1(void);
extern void uttest_PPM_PortWriteAsyCB_case2(void);
extern void uttest_PPM_PortWriteAsyCB_case3(void);
extern unsigned int uttest_PPM_InitPhyPort_case1(void);
extern unsigned int uttest_PPM_InitPhyPort_case2(void);
extern unsigned int uttest_PPM_InitPhyPort_case3(void);
extern unsigned int uttest_PPM_PortInit_case1(void);
extern unsigned int uttest_PPM_OmPortInfoShow_case1(void);
extern void uttest_PPM_PortSwitchInfoShow_case1(void);
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
*测试项: 消息申请失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_DisconnectGUPort1, UT)
{
	MOCKER(V_AllocMsg)
		.stubs()
		.will(returnValue((void*)0));

	EXPECT_EQ(VOS_ERR, uttest_PPM_DisconnectGUPort_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 消息发送成功
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_DisconnectGUPort2, UT)
{
	char 			aucMem[60];

	MOCKER(V_AllocMsg)
		.stubs()
		.will(returnValue((void*)aucMem));

	MOCKER(V_SendMsg)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_DisconnectGUPort_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 消息申请失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_DisconnectTLPort1, UT)
{
	MOCKER(V_AllocMsg)
		.stubs()
		.will(returnValue((void*)0));

	EXPECT_EQ(VOS_ERR, uttest_PPM_DisconnectTLPort_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 消息发送成功
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_DisconnectTLPort2, UT)
{
	char 			aucMem[60];

	MOCKER(V_AllocMsg)
		.stubs()
		.will(returnValue((void*)aucMem));

	MOCKER(V_SendMsg)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_DisconnectTLPort_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 断开所有链接
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_DisconnectAllPort1, UT)
{
	MOCKER(PPM_DisconnectTLPort)
		.expects(once())
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(PPM_DisconnectGUPort)
		.expects(once())
		.will(returnValue((unsigned int)VOS_OK));

	uttest_PPM_DisconnectAllPort_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI设备上数据长度大于60K
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_GetSendDataLen1, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.stubs()
		.will(returnValue((unsigned int)4/*CPM_IND_PORT*/));

	uttest_PPM_GetSendDataLen_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: VCOM上数据长度小于60K
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_GetSendDataLen2, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.stubs()
		.will(returnValue((unsigned int)12/*CPM_VCOM_IND_PORT*/));

	uttest_PPM_GetSendDataLen_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI设备USB CFG端口断开
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortStatus1, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.stubs()
		.will(returnValue((unsigned int)5/*CPM_CFG_PORT*/));

	MOCKER(PPM_DisconnectAllPort)
		.expects(once());

    MOCKER(OM_GetSlice)
        .stubs()
        .will(returnValue((unsigned int)1));

	uttest_PPM_PortStatus_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI设备USB IND端口断开
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortStatus2, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.stubs()
		.will(returnValue((unsigned int)4/*CPM_IND_PORT*/));

	MOCKER(PPM_DisconnectAllPort)
		.expects(once());

    MOCKER(OM_GetSlice)
        .stubs()
        .will(returnValue((unsigned int)1));

	uttest_PPM_PortStatus_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 其它UDI设备端口断开
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortStatus3, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.stubs()
		.will(returnValue((unsigned int)4/*CPM_IND_PORT*/));

	MOCKER(PPM_DisconnectAllPort)
		.expects(never());

    MOCKER(OM_GetSlice)
        .stubs()
        .will(returnValue((unsigned int)1));

	uttest_PPM_PortStatus_case3();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 端口状态为READY
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortStatus4, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.expects(never());

	MOCKER(PPM_DisconnectAllPort)
		.expects(never());

	MOCKER(OM_GetSlice)
		.expects(once())
		.will(returnValue(1000));

	uttest_PPM_PortStatus_case4();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 其它端口状态
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortStatus5, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.expects(never());

	MOCKER(PPM_DisconnectAllPort)
		.expects(never());

	MOCKER(OM_GetSlice)
		.expects(once())
		.will(returnValue(1000));

	uttest_PPM_PortStatus_case5();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI设备句柄错误
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortCloseProc1, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.expects(never());

	MOCKER(PPM_DisconnectAllPort)
		.expects(never());

    MOCKER(OM_GetSlice)
        .stubs()
        .will(returnValue((unsigned int)1));

	uttest_PPM_PortCloseProc_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI设备USB CFG端口断开
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortCloseProc2, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.stubs()
		.will(returnValue((unsigned int)5/*CPM_CFG_PORT*/));

	MOCKER(PPM_DisconnectAllPort)
		.expects(once());

	MOCKER(udi_close)
		.expects(once())
		.will(returnValue(VOS_OK));

	MOCKER(OM_GetSlice)
		.expects(atLeast(2))
		.will(returnValue((unsigned int)1000));
#if 0
	MOCKER(_raw_spin_lock_irqsave)
		.stubs()
		.will(returnValue(0));

	MOCKER(_raw_spin_unlock_irqrestore)
		.stubs()
		.will(returnValue(0));
#endif
	uttest_PPM_PortCloseProc_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI设备USB IND端口断开
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortCloseProc3, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.stubs()
		.will(returnValue((unsigned int)4/*CPM_IND_PORT*/));

	MOCKER(PPM_DisconnectAllPort)
		.expects(once());

	MOCKER(udi_close)
		.expects(once())
		.will(returnValue(VOS_OK));

	MOCKER(OM_GetSlice)
		.expects(atLeast(2))
		.will(returnValue((unsigned int)1000));

	uttest_PPM_PortCloseProc_case3();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI设备其它端口断开
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortCloseProc4, UT)
{
	MOCKER(CPM_QueryPhyPort)
		//.stubs()
		.expects(never())
		.will(returnValue((unsigned int)4/*CPM_IND_PORT*/));

	MOCKER(PPM_DisconnectAllPort)
		.expects(never());

	MOCKER(udi_close)
		.expects(once())
		.will(returnValue(VOS_OK));

	MOCKER(OM_GetSlice)
		.expects(atLeast(2))
		.will(returnValue((unsigned int)1000));

	uttest_PPM_PortCloseProc_case4();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI设备句柄错误
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortData1, UT)
{
	EXPECT_EQ(VOS_ERR, uttest_PPM_ReadPortData_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 获取USB的IO CTRL口的读缓存失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortData2, UT)
{
	MOCKER(udi_ioctl)
		.stubs()
		.will(returnValue((int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_ReadPortData_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB的IO CTRL口的缓存释放失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortData3, UT)
{
	MOCKER(udi_ioctl)
		.expects(exactly(2))
		.will(returnObjectList((int)0, (int)1));

	MOCKER(CPM_ComRcv)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_ReadPortData_case3());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB IND端口的伪同步信号量释放失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortPseudoSyncGetSmp1, UT)
{
	MOCKER(VOS_SmP)
		.stubs()
		.will(returnValue((unsigned int)VOS_ERR));

	MOCKER(OM_GetSlice)
		.expects(once())
		.will(returnValue(1000));

	uttest_PPM_PortPseudoSyncGetSmp_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB CFG端口的伪同步信号量释放失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortPseudoSyncGetSmp2, UT)
{
	MOCKER(VOS_SmP)
		.stubs()
		.will(returnValue((unsigned int)VOS_ERR));

	MOCKER(OM_GetSlice)
		.expects(once())
		.will(returnValue((unsigned int)1000));

	uttest_PPM_PortPseudoSyncGetSmp_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 其它端口的处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortPseudoSyncGetSmp3, UT)
{
	MOCKER(VOS_SmP)
		.expects(never())
		.will(returnValue((unsigned int)VOS_ERR));

	MOCKER(OM_GetSlice)
		.expects(never())
		.will(returnValue((unsigned int)1000));

	uttest_PPM_PortPseudoSyncGetSmp_case3();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 传入空指针
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_QueryLogPort1, UT)
{
	EXPECT_EQ(VOS_ERR, uttest_PPM_QueryLogPort_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 当前端口类型不是VCOM也不是USB
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_QueryLogPort2, UT)
{
	EXPECT_EQ(VOS_ERR, uttest_PPM_QueryLogPort_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 当前端口类型是VCOM
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_QueryLogPort3, UT)
{
	EXPECT_EQ(VOS_OK, uttest_PPM_QueryLogPort_case3());

	GlobalMockObject::reset();
}


/*******************************************************************
*测试项: 切换的端口类型不正确
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_LogPortSwitch1, UT)
{
	EXPECT_EQ(VOS_ERR, uttest_PPM_LogPortSwitch_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 切换的端口与当前端口一致不切换
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_LogPortSwitch2, UT)
{
	MOCKER(mdrv_socp_set_log_int_timeout)
		.expects(exactly(1));

	EXPECT_EQ(VOS_OK, uttest_PPM_LogPortSwitch_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 当前是USB输出切换到VCOM输出
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_LogPortSwitch3, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.expects(exactly(2))
		.will(returnObjectList((unsigned int)5, (unsigned int)4));

	MOCKER(CPM_DisconnectPorts)
		.expects(exactly(2));

	MOCKER(CPM_ConnectPorts)
		.expects(exactly(2));

	MOCKER(PPM_DisconnectAllPort)
		.expects(exactly(2));

	MOCKER(BSP_SOCP_SetLogIntTimeOut)
		.expects(exactly(1));

	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnValue((unsigned int)1000));

	EXPECT_EQ(VOS_OK, uttest_PPM_LogPortSwitch_case3());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 当前是VCOM输出切换到USB输出
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_LogPortSwitch4, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.expects(exactly(2))
		.will(returnObjectList((unsigned int)13, (unsigned int)12));

	MOCKER(CPM_DisconnectPorts)
		.expects(exactly(2));

	MOCKER(CPM_ConnectPorts)
		.expects(exactly(2));

	MOCKER(PPM_DisconnectAllPort)
		.expects(exactly(2));

	MOCKER(BSP_SOCP_SetLogIntTimeOutDefautl)
		.expects(exactly(1));

	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnValue((unsigned int)1000));

	MOCKER(NV_WriteEx)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_LogPortSwitch_case4());

	GlobalMockObject::reset();
}

#if 0 /*这个用例跑不进去*/
/*******************************************************************
*测试项: 当前是WIFI输出，不允许切换
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_LogPortSwitch5, UT)
{
	MOCKER(CPM_QueryPhyPort)
		.expects(exactly(2))
		.will(returnObjectList((unsigned int)13, (unsigned int)12));

	MOCKER(CPM_DisconnectPorts)
		.expects(exactly(2));

	MOCKER(CPM_ConnectPorts)
		.expects(exactly(2));

	MOCKER(PPM_DisconnectAllPort)
		.expects(exactly(2));

	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnValue((unsigned int)1000));

	MOCKER(NV_WriteEx)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_LogPortSwitch_case5());

	GlobalMockObject::reset();
}
#endif

/*******************************************************************
*测试项: 输入的函数指针为空
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UdiRegCallBackFun1, UT)
{
	EXPECT_EQ(VOS_OK, uttest_PPM_UdiRegCallBackFun_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 调用UDI设备注册函数返回失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UdiRegCallBackFun2, UT)
{
	MOCKER(udi_ioctl)
		.expects(once())
		.will(returnValue(VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_UdiRegCallBackFun_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 调用UDI设备注册函数返回成功
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UdiRegCallBackFun3, UT)
{
	MOCKER(udi_ioctl)
		.expects(once())
		.will(returnValue(VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_UdiRegCallBackFun_case3());

	GlobalMockObject::reset();
}


/*******************************************************************
*测试项: 打开其它类型的UDI设备
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit1, UT)
{
	MOCKER(udi_open)
		.expects(never())
		.will(returnValue(VOS_OK));

	uttest_PPM_ReadPortDataInit_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 打开USB IND类型的UDI设备，设备已经打开
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit2, UT)
{
	MOCKER(udi_open)
		.expects(never())
		.will(returnValue(VOS_OK));

	MOCKER(OM_GetSlice)
		.expects(exactly(1))
		.will(returnValue((unsigned int)1000));

	uttest_PPM_ReadPortDataInit_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 打开USB CFG类型的UDI设备，设备打开失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit3, UT)
{
	MOCKER(udi_open)
		.expects(once())
		.will(returnValue((int)-1));

	MOCKER(OM_GetSlice)
		.expects(once())
		.will(returnValue((unsigned int)1000));

	uttest_PPM_ReadPortDataInit_case3();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 打开HSIC IND类型的UDI设备，配置OM使用的USB通道缓存失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit4, UT)
{
	MOCKER(udi_open)
		.expects(once())
		.will(returnValue((int)2));

	MOCKER(udi_ioctl)
		.expects(once())
		.will(returnValue((int)VOS_ERR));

	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnValue((unsigned int)1000));

	uttest_PPM_ReadPortDataInit_case4();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 打开HSIC CFG类型的UDI设备，获取USB的IO CTRL口的读缓存失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit5, UT)
{
	MOCKER(udi_open)
		.expects(once())
		.will(returnValue((int)2));

	MOCKER(udi_ioctl)
		.expects(exactly(2))
		.will(returnObjectList((int)VOS_OK, (int)VOS_ERR));

	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnValue((unsigned int)1000));

	uttest_PPM_ReadPortDataInit_case5();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 打开HSIC CFG类型的UDI设备，注册OM使用的读数据回调函数失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit6, UT)
{
	MOCKER(udi_open)
		.expects(once())
		.will(returnValue((int)2));

	MOCKER(udi_ioctl)
		.expects(exactly(2))
		.will(returnObjectList((int)VOS_OK, (int)VOS_OK));

	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnValue((unsigned int)1000));

	MOCKER(PPM_UdiRegCallBackFun)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	uttest_PPM_ReadPortDataInit_case6();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 打开HSIC CFG类型的UDI设备，注册OM使用的写数据回调函数失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit7, UT)
{
	MOCKER(udi_open)
		.expects(once())
		.will(returnValue((int)2));

	MOCKER(udi_ioctl)
		.expects(exactly(2))
		.will(returnObjectList((int)VOS_OK, (int)VOS_OK));

	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnValue((unsigned int)1000));

	MOCKER(PPM_UdiRegCallBackFun)
		.expects(exactly(2))
		.will(returnObjectList((unsigned int)VOS_OK, (unsigned int)VOS_ERR));

	uttest_PPM_ReadPortDataInit_case7();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 打开HSIC CFG类型的UDI设备，注册OM使用的状态回调函数失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit8, UT)
{
	MOCKER(udi_open)
		.expects(once())
		.will(returnValue((int)2));

	MOCKER(udi_ioctl)
		.expects(exactly(2))
		.will(returnObjectList((int)VOS_OK, (int)VOS_OK));

	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnValue((unsigned int)1000));

	MOCKER(PPM_UdiRegCallBackFun)
		.expects(exactly(3))
		.will(returnObjectList((unsigned int)VOS_OK, (unsigned int)VOS_OK, (unsigned int)VOS_ERR));

	uttest_PPM_ReadPortDataInit_case8();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 打开HSIC CFG类型的UDI设备，设备UDI设备的零拷贝
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit9, UT)
{
	MOCKER(udi_open)
		.expects(once())
		.will(returnValue((int)2));

	MOCKER(udi_ioctl)
		.expects(exactly(3))
		.will(returnObjectList((int)VOS_OK, (int)VOS_OK, (int)VOS_ERR));

	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnValue((unsigned int)1000));

	MOCKER(PPM_UdiRegCallBackFun)
		.expects(exactly(3))
		.will(returnObjectList((unsigned int)VOS_OK, (unsigned int)VOS_OK, (unsigned int)VOS_OK));

	uttest_PPM_ReadPortDataInit_case9();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 打开HSIC CFG类型的UDI设备，设备打开成功
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_ReadPortDataInit10, UT)
{
	MOCKER(udi_open)
		.expects(once())
		.will(returnValue((int)2));

	MOCKER(udi_ioctl)
		.expects(exactly(3))
		.will(returnObjectList((int)VOS_OK, (int)VOS_OK, (int)VOS_OK));

	MOCKER(OM_GetSlice)
		.expects(exactly(3))
		.will(returnValue((unsigned int)1000));

	MOCKER(PPM_UdiRegCallBackFun)
		.expects(exactly(3))
		.will(returnObjectList((unsigned int)VOS_OK, (unsigned int)VOS_OK, (unsigned int)VOS_OK));

	uttest_PPM_ReadPortDataInit_case10();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI发送数据虚实地址为空
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortSend1, UT)
{
	EXPECT_EQ(2/*CPM_SEND_PARA_ERR*/, uttest_PPM_PortSend_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: UDI句柄为空
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortSend2, UT)
{
	EXPECT_EQ(3/*CPM_SEND_ERR*/, uttest_PPM_PortSend_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 数据发送成功
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortSend3, UT)
{
	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnObjectList((unsigned int)1000, (unsigned int)2000));

	MOCKER(udi_ioctl)
		.expects(once())
		.will(returnValue((int)VOS_OK));

	MOCKER(PPM_PortPseudoSyncGetSmp)
		.expects(exactly(1));

	EXPECT_EQ(0/*CPM_SEND_OK*/, uttest_PPM_PortSend_case3());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 数据发送失败，获取保存信息的路径失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortSend4, UT)
{
	MOCKER(OM_GetSlice)
		.expects(exactly(3))
		.will(returnObjectList((unsigned int)2000, (unsigned int)1000, (unsigned int)3000));

	MOCKER(udi_ioctl)
		.expects(once())
		.will(returnValue((int)(-1)));

	MOCKER(PPM_PortPseudoSyncGetSmp)
		.expects(never());

	MOCKER(OM_AcpuGetLogPath)
		.expects(once())
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(1/*CPM_SEND_FUNC_NULL*/, uttest_PPM_PortSend_case4());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 数据发送失败，信息保存到单板
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortSend5, UT)
{
	MOCKER(OM_GetSlice)
		.expects(exactly(3))
		.will(returnObjectList((unsigned int)2000, (unsigned int)1000, (unsigned int)3000));

	MOCKER(udi_ioctl)
		.expects(once())
		.will(returnValue((int)(-1)));

	MOCKER(PPM_PortPseudoSyncGetSmp)
		.expects(never());

	MOCKER(OM_AcpuGetLogPath)
		.expects(once())
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(OM_Acpu_WriteLogFile)
		.expects(once())
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(1/*CPM_SEND_FUNC_NULL*/, uttest_PPM_PortSend_case5());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 数据发送失败，单板复位
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortSend6, UT)
{
	MOCKER(OM_GetSlice)
		.expects(exactly(2))
		.will(returnObjectList((unsigned int)2000, (unsigned int)1000));

	MOCKER(udi_ioctl)
		.expects(once())
		.will(returnValue((int)(2)));

	MOCKER(PPM_PortPseudoSyncGetSmp)
		.expects(never());

	MOCKER(OM_AcpuGetLogPath)
		.expects(never())
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(OM_Acpu_WriteLogFile)
		.expects(never())
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(BSP_MNTN_SystemError)
		.expects(once());

	EXPECT_EQ(3/*CPM_SEND_ERR*/, uttest_PPM_PortSend_case6());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 数据长度小于0，USB IND端口
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortWriteAsyCB1, UT)
{
	MOCKER(VOS_SmV)
		.expects(once())
		.will(returnValue((unsigned int)VOS_OK));

	uttest_PPM_PortWriteAsyCB_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 数据长度大于0，USB CFG端口
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortWriteAsyCB2, UT)
{
	MOCKER(VOS_SmV)
		.expects(once())
		.will(returnValue((unsigned int)VOS_OK));

	uttest_PPM_PortWriteAsyCB_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: HSIC CFG端口
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortWriteAsyCB3, UT)
{
	uttest_PPM_PortWriteAsyCB_case3();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: PPM_LogFileInit返回失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_InitPhyPort1, UT)
{
	MOCKER(PPM_LogFileInit)
		.expects(once())
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_InitPhyPort_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: PPM_PortInit返回失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_InitPhyPort2, UT)
{
	MOCKER(PPM_LogFileInit)
		.expects(once())
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(PPM_PortInit)
		.expects(once())
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_InitPhyPort_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 返回成功
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_InitPhyPort3, UT)
{
	MOCKER(PPM_LogFileInit)
		.expects(once())
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(PPM_PortInit)
		.expects(once())
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_InitPhyPort_case3());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: PPM_PortInit返回成功
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortInit1, UT)
{
	MOCKER(PPM_UsbPortInit)
		.expects(once())
		.will(returnValue((unsigned int)VOS_ERR));

	MOCKER(PPM_HsicPortInit)
		.expects(once());

	MOCKER(PPM_VComPortInit)
		.expects(once());

	EXPECT_EQ(VOS_OK, uttest_PPM_PortInit_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: PPM_OmPortInfoShow输出
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_OmPortInfoShow1, UT)
{
	uttest_PPM_OmPortInfoShow_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: PPM_PortSwitchInfoShow输出
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_PortSwitchInfoShow1, UT)
{
	uttest_PPM_PortSwitchInfoShow_case1();

	GlobalMockObject::reset();
}


