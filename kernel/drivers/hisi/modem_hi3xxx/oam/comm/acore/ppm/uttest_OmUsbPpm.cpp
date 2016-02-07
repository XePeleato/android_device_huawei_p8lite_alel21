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
extern unsigned int PPM_PortSend(unsigned int enHandle, unsigned char *pucVirAddr, unsigned char *pucPhyAddr, unsigned int ulDataLen);
extern void PPM_PortCloseProc(unsigned int enHandle, unsigned int enPhyPort);
extern void PPM_PortStatus(unsigned int enHandle, unsigned int enPhyPort, unsigned int enPortState);
extern void PPM_PortWriteAsyCB(unsigned int enHandle, unsigned char* pucData, int lLen);
extern int PPM_ReadPortData(unsigned int enPhyPort, unsigned int UdiHandle);
extern void PPM_ReadPortDataInit(unsigned int enPhyPort, unsigned int enHandle, void *pReadCB, void *pWriteCB, void *pStateCB);
extern unsigned int VOS_SmCCreate(unsigned char acSmName[4], unsigned int ulSmInit, unsigned int ulFlags, unsigned int *pSmID );
extern unsigned int BSP_USB_RegUdiEnableCB(void *pFunc);
extern unsigned int BSP_USB_RegUdiDisableCB(void *pFunc);
extern void CPM_PhySendReg(unsigned int enPhyPort, void* pSendFunc);
extern unsigned int PPM_UsbIndPortInit(void);
extern unsigned int PPM_UsbCfgPortInit(void);

extern unsigned int uttest_PPM_UsbCfgSendData_case1(void);
extern unsigned int uttest_PPM_UsbIndSendData_case1(void);
extern void uttest_PPM_UsbCfgPortClose_case1(void);
extern void uttest_PPM_UsbIndPortClose_case1(void);
extern void uttest_PPM_UsbCfgStatusCB_case1(void);
extern void uttest_PPM_UsbIndStatusCB_case1(void);
extern void uttest_PPM_UsbCfgWriteDataCB_case1(void);
extern void uttest_PPM_UsbIndWriteDataCB_case1(void);
extern int uttest_PPM_UsbCfgReadDataCB_case1(void);
extern void uttest_PPM_UsbCfgPortOpen_case1(void);
extern void uttest_PPM_UsbIndPortOpen_case1(void);
extern unsigned int uttest_PPM_UsbCfgPortInit_case1(void);
extern unsigned int uttest_PPM_UsbCfgPortInit_case2(void);
extern unsigned int uttest_PPM_UsbIndPortInit_case1(void);
extern unsigned int uttest_PPM_UsbIndPortInit_case2(void);
extern unsigned int uttest_PPM_UsbPortInit_case1(void);
extern unsigned int uttest_PPM_UsbPortInit_case2(void);
extern unsigned int uttest_PPM_UsbPortInit_case3(void);
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
*测试项: USB CFG端口数据发送
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbCfgSendData1, UT)
{
	MOCKER(PPM_PortSend)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_UsbCfgSendData_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB IND端口数据发送
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbIndSendData1, UT)
{
	MOCKER(PPM_PortSend)
		.stubs()
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_UsbIndSendData_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB CFG端口关闭处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbCfgPortClose1, UT)
{
	MOCKER(PPM_PortCloseProc)
		.expects(exactly(1));

	uttest_PPM_UsbCfgPortClose_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB IND端口关闭处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbIndPortClose1, UT)
{
	MOCKER(PPM_PortCloseProc)
		.expects(exactly(1));

	uttest_PPM_UsbIndPortClose_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB CFG端口状态变更处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbCfgStatusCB1, UT)
{
	MOCKER(PPM_PortStatus)
		.expects(exactly(1));

	uttest_PPM_UsbCfgStatusCB_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB IND端口状态变更处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbIndStatusCB1, UT)
{
	MOCKER(PPM_PortStatus)
		.expects(exactly(1));

	uttest_PPM_UsbIndStatusCB_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB CFG端口异步写处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbCfgWriteDataCB1, UT)
{
	MOCKER(PPM_PortWriteAsyCB)
		.expects(exactly(1));

	uttest_PPM_UsbCfgWriteDataCB_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB IND端口异步写处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbIndWriteDataCB1, UT)
{
	MOCKER(PPM_PortWriteAsyCB)
		.expects(exactly(1));

	uttest_PPM_UsbIndWriteDataCB_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB CFG端口异步写处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbCfgReadDataCB1, UT)
{
	MOCKER(PPM_ReadPortData)
		.expects(exactly(1))
		.will(returnValue((int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_UsbCfgReadDataCB_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB CFG端口打开处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbCfgPortOpen1, UT)
{
	MOCKER(PPM_ReadPortDataInit)
		.expects(exactly(1));

	uttest_PPM_UsbCfgPortOpen_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB IND端口打开处理
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbIndPortOpen1, UT)
{
	MOCKER(PPM_ReadPortDataInit)
		.expects(exactly(1));

	uttest_PPM_UsbIndPortOpen_case1();

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB CFG端口初始化，申请信号量失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbCfgPortInit1, UT)
{
	MOCKER(VOS_SmCCreate)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_UsbCfgPortInit_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB CFG端口初始化
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbCfgPortInit2, UT)
{
	MOCKER(VOS_SmCCreate)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(BSP_USB_RegUdiEnableCB)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(BSP_USB_RegUdiDisableCB)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(CPM_PhySendReg)
		.expects(exactly(1));

	EXPECT_EQ(VOS_OK, uttest_PPM_UsbCfgPortInit_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB IND端口初始化，申请信号量失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbIndPortInit1, UT)
{
	MOCKER(VOS_SmCCreate)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_UsbIndPortInit_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB IND端口初始化
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbIndPortInit2, UT)
{
	MOCKER(VOS_SmCCreate)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(BSP_USB_RegUdiEnableCB)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(BSP_USB_RegUdiDisableCB)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(CPM_PhySendReg)
		.expects(exactly(1));

	EXPECT_EQ(VOS_OK, uttest_PPM_UsbIndPortInit_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB 承载的OM主动上报端口的初始化失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbPortInit1, UT)
{
	MOCKER(PPM_UsbIndPortInit)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_UsbPortInit_case1());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: USB 承载的OM配置端口的初始化失败
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbPortInit2, UT)
{
	MOCKER(PPM_UsbIndPortInit)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(PPM_UsbCfgPortInit)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_ERR));

	EXPECT_EQ(VOS_ERR, uttest_PPM_UsbPortInit_case2());

	GlobalMockObject::reset();
}

/*******************************************************************
*测试项: 初始化成功
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期:
#    作    者:
#    修改内容:
*******************************************************************/
TEST(PPM_UsbPortInit3, UT)
{
	MOCKER(PPM_UsbIndPortInit)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	MOCKER(PPM_UsbCfgPortInit)
		.expects(exactly(1))
		.will(returnValue((unsigned int)VOS_OK));

	EXPECT_EQ(VOS_OK, uttest_PPM_UsbPortInit_case3());

	GlobalMockObject::reset();
}