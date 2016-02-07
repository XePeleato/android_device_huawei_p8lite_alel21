
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
extern unsigned int PPM_VComCfgSendData(unsigned char *pucVirAddr, unsigned char *pucPhyAddr, unsigned int ulDataLen);
extern unsigned int SCM_SoftDecodeCfgDataRcv(unsigned char *pucBuffer, unsigned int ulLen);
extern unsigned int BSP_CheckModuleSupport(unsigned int enModuleType);
extern void uttest_CPM_PhySendReg_case1(void);
extern void uttest_CPM_PhySendReg_case2(void);
extern void uttest_CPM_LogicRcvReg_case1(void);
extern void uttest_CPM_LogicRcvReg_case2(void);
extern void uttest_CPM_ConnectPorts_case1(void);
extern void uttest_CPM_ConnectPorts_case2(void);
extern void uttest_CPM_ConnectPorts_case3(void);
extern void uttest_CPM_DisconnectPorts_case1(void);
extern void uttest_CPM_DisconnectPorts_case2(void);
extern void uttest_CPM_DisconnectPorts_case3(void);
extern void uttest_CPM_DisconnectPorts_case4(void);
extern unsigned int uttest_CPM_ComSend_case1(void);
extern unsigned int uttest_CPM_ComSend_case2(void);
extern unsigned int uttest_CPM_ComSend_case3(void);
extern unsigned int uttest_CPM_ComSend_case4(void);
extern unsigned int uttest_CPM_ComSend_case5(void);
extern unsigned int uttest_CPM_ComSend_case6(void);
extern unsigned int uttest_CPM_ComRcv_case1(void);
extern unsigned int uttest_CPM_ComRcv_case2(void);
extern unsigned int uttest_CPM_ComRcv_case3(void);
extern unsigned int uttest_CPM_ComRcv_case4(void);
extern void uttest_CPM_Show_case1(void);
extern unsigned int uttest_CPM_PortAssociateInit_case1(void);
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
*测试项:
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
/*
TEST(被测函数名, 用例名)
{
	MOCKER(Om_AcpuDumpReboot)
	    .stubs()
		.will(returnValue((unsigned int)VOS_OK));

	uttest_Om_AcpuBbpConfigMsgProc_case1();
}
*/

/*******************************************************************
*测试项: 注册的物理端口号越界
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_PhySendReg1, UT)
{
	uttest_CPM_PhySendReg_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 注册物理发送函数成功
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_PhySendReg2, UT)
{
	uttest_CPM_PhySendReg_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 注册的逻辑端口号越界
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_LogicRcvReg1, UT)
{
	uttest_CPM_LogicRcvReg_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 注册的逻辑接收函数成功
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_LogicRcvReg2, UT)
{
	uttest_CPM_LogicRcvReg_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 连接端口的物理通道号越界
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ConnectPorts1, UT)
{
	uttest_CPM_ConnectPorts_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 连接端口的逻辑通道号越界
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ConnectPorts2, UT)
{
	uttest_CPM_ConnectPorts_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 连接端口成功
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ConnectPorts3, UT)
{
	uttest_CPM_ConnectPorts_case3();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 断开端口的物理通道号越界
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_DisconnectPorts1, UT)
{
	uttest_CPM_DisconnectPorts_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 断开端口的逻辑通道号越界
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_DisconnectPorts2, UT)
{
	uttest_CPM_DisconnectPorts_case2();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 断开的逻辑通道没有使用此物理通道
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_DisconnectPorts3, UT)
{
	uttest_CPM_DisconnectPorts_case3();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 断开成功
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_DisconnectPorts4, UT)
{
	uttest_CPM_DisconnectPorts_case4();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 发送的逻辑通道号越界
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComSend1, UT)
{
	EXPECT_EQ(2, uttest_CPM_ComSend_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 发送数据地址为空
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComSend2, UT)
{
	EXPECT_EQ(2, uttest_CPM_ComSend_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 发送数据长度为0
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComSend3, UT)
{
	EXPECT_EQ(2, uttest_CPM_ComSend_case3());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 发送函数指针为空
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComSend4, UT)
{
	EXPECT_EQ(1, uttest_CPM_ComSend_case4());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 发送函数返回失败
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComSend5, UT)
{
	MOCKER(PPM_VComCfgSendData)
		.stubs()
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_CPM_ComSend_case5());

	GlobalMockObject::reset();
}


/*******************************************************************
*测试项: 发送函数返回成功
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComSend6, UT)
{
	MOCKER(PPM_VComCfgSendData)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_CPM_ComSend_case6());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 接收的物理通道号越界
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComRcv1, UT)
{
	EXPECT_EQ(VOS_ERR, uttest_CPM_ComRcv_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 接收数据地址为空
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComRcv2, UT)
{
	EXPECT_EQ(VOS_ERR, uttest_CPM_ComRcv_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 接收函数指针为空
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComRcv3, UT)
{
	EXPECT_EQ(VOS_ERR, uttest_CPM_ComRcv_case3());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项:接收函数返回成功
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_ComRcv4, UT)
{
	MOCKER(SCM_SoftDecodeCfgDataRcv)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_CPM_ComRcv_case4());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: CPM_Show输出打印信息
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_Show1, UT)
{
	uttest_CPM_Show_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: CPM_PortAssociateInit完成端口关联，支持HSIC
*被测函数功能描述:
*预期结果：           返回
************************* 修改记录 *************************
#  1.日    期: 2014年6月12日
#    作    者: h59254
#    修改内容: 新建CASE
*******************************************************************/
TEST(CPM_PortAssociateInit1, UT)
{
	MOCKER(BSP_CheckModuleSupport)
		.stubs()
		.will(returnValue((unsigned int)0));

	EXPECT_EQ(VOS_OK, uttest_CPM_PortAssociateInit_case1());

	GlobalMockObject::reset();
}