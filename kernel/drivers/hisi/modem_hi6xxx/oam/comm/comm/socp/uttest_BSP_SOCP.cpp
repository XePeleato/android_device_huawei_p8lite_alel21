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
    extern unsigned int NV_ReadEx(
        unsigned short           enModemID,
        unsigned short           usID,
        void                    *pItem,
        unsigned int             ulLength);
    extern unsigned int SOCP_MsConvertToCycle(unsigned int ulDelayTimeInMs);
    extern unsigned int VOS_SmV( unsigned long Sm_ID );
    extern void * SocpGetSDLogCfg(void);
    extern unsigned int OM_GetSlice(void);
    extern void OM_Inner_Log(
        const unsigned char         *pucStr,
        unsigned int              ulP1,
        unsigned int              ulP2,
        void                     *pstLogData ,
        unsigned int              ulFileID,
        int                       lLineNo);
    extern void SocpHandlerEncDstTrfInt(void);
    extern void SocpHandlerEncDstOutOvfInt(void);
    extern unsigned int VOS_SmCCreate(
        unsigned char             acSmName[4],
        unsigned int              ulSmInit,
        unsigned int              ulFlags,
        unsigned long            *pSmID );
    extern unsigned int VOS_CreateTask(
        unsigned char            *puchName,
        unsigned int             *pulTaskID,
        void                     *pfnFunc,
        unsigned int              ulPriority,
        unsigned int              ulStackSize,
        unsigned int              aulArgs[4] );
    extern void * BSP_GetIPBaseAddr(unsigned int enIPType);
    extern void *__ioremap(unsigned long phys_addr, unsigned long size, unsigned long flags);
    extern void * __arm_ioremap(unsigned long, size_t, unsigned int);
    extern void SocpCoreInit(void);
    extern int BSP_GetIntNO(unsigned int enIntType);
    extern int BSP_INT_Connect  (void **vector,void *routine, int parameter);;
    extern int BSP_INT_Enable ( int s32Lvl);
    extern int SocpAcpuCreateTask(void);


    extern unsigned int uttest_SOCP_MsConvertToCycle_case(unsigned int ulDelayTimeInMs);
    extern void uttest_SocpEncDstTaskProc_case1(unsigned int ulRealChannelId);
    extern void uttest_SocpEncDstTaskProc_case2(unsigned int ulRealChannelId);
    extern void uttest_SocpEncDstTaskProc_case3(unsigned int ulRealChannelId);
    extern void uttest_SocpHandlerEncDstTrfInt_case1(void);
    extern void uttest_SocpHandlerEncDst_case1(void);
    extern void uttest_SocpHandlerEncDst_case2(void);
    extern void uttest_SocpHandlerEncDst_case3(void);
    extern void uttest_SocpHandlerDecSrc_case1(void);
    extern void uttest_SocpHandlerDecSrc_case2(void);
    extern void uttest_SocpHandlerDecDst_case1(void);
    extern void uttest_SocpHandlerDecDst_case2(void);
    extern void uttest_SocpHandlerDecDst_case3(void);
    extern unsigned int uttest_BSP_SOCP_DecoderSetDestChan_case1(void);
    extern unsigned int uttest_BSP_SOCP_DecoderSetDestChan_case2(void);
    extern unsigned int uttest_BSP_SOCP_DecoderSetDestChan_case3(void);

    extern unsigned int uttest_BSP_SOCP_CoderSetDestChanAttr_case1(void);
    extern unsigned int uttest_BSP_SOCP_CoderSetDestChanAttr_case2(void);
    extern unsigned int uttest_BSP_SOCP_CoderSetDestChanAttr_case3(void);
    extern unsigned int uttest_BSP_SOCP_CoderSetDestChanAttr_case4(void);
    extern unsigned int uttest_BSP_SOCP_CoderSetDestChanAttr_case5(void);
    extern unsigned int uttest_BSP_SOCP_CoderSetDestChanAttr_case6(void);

    extern unsigned int uttest_BSP_SOCP_DecoderSetSrcChanAttr_case1(void);
    extern unsigned int uttest_BSP_SOCP_DecoderSetSrcChanAttr_case2(void);
    extern unsigned int uttest_BSP_SOCP_DecoderSetSrcChanAttr_case3(void);
    extern unsigned int uttest_BSP_SOCP_DecoderSetSrcChanAttr_case4(void);
    extern int uttest_SocpAcpuCreateTask_case(void);
    extern int uttest_SocpCoreInit_case(void);
    extern unsigned long uttest_SocpAcpuInit_GetaulRegAddr(void);
    extern int uttest_SocpAcpuInit_case(void);
    extern unsigned int uttest_BSP_SOCP_CoderSetSrcChan_case1(void);
    extern unsigned int uttest_BSP_SOCP_CoderSetSrcChan_case2(void);
    extern unsigned int uttest_BSP_SOCP_CoderSetSrcChan_case3(void);
    extern unsigned int uttest_BSP_SOCP_CoderSetSrcChan_case4(void);
    extern void uttest_BSP_SOCP_EncDstBufFlush_case(void);

#ifdef __cplusplus
}
#endif

#ifndef VOS_OK
#define VOS_OK                          0
#endif

#ifndef VOS_ERR
#define VOS_ERR                         1
#endif

#ifndef VOS_FALSE
#define VOS_FALSE                       0
#endif

#ifndef VOS_TRUE
#define VOS_TRUE                        1
#endif

#ifndef MDRV_OK
#define MDRV_OK                          (0)
#endif

#ifndef MDRV_ERROR
#define MDRV_ERROR                       (-1)
#endif

#ifndef BSP_ERR_SOCP_NOT_INIT
#define BSP_ERR_SOCP_NOT_INIT           2
#endif

#ifndef BSP_ERR_SOCP_SEM_CREATE
#define BSP_ERR_SOCP_SEM_CREATE         4
#endif

#ifndef BSP_ERR_SOCP_TSK_CREATE
#define BSP_ERR_SOCP_TSK_CREATE         5
#endif

#ifndef BSP_ERR_SOCP_INVALID_PARA
#define BSP_ERR_SOCP_INVALID_PARA       7
#endif

#ifndef BSP_ERR_SOCP_SET_FAIL
#define BSP_ERR_SOCP_SET_FAIL           9
#endif

#ifndef BSP_ERR_SOCP_DECSRC_SET
#define BSP_ERR_SOCP_DECSRC_SET         15
#endif



typedef struct
{
    //unsigned int IndWaterMark;    /* SOCP编码目的通道数据传输水线，IND通道 */
    //unsigned int CfgWaterMark;    /* SOCP编码目的通道数据传输水线, CFG通道 */
    //unsigned int overTime;        /* SOCP编码目的通道数据传输超时时间 */
    //unsigned int logOnFlag;       /* LOG2.0功能启动标志 */
    void*      pVirBuffer;      /* SOCP编码目的通道数据虚拟BUFFER、在32位系统上是4字节，在64位系统上是8字节 */
    unsigned int     ulPhyBufferAddr; /* SOCP编码目的通道数据物理BUFFER地址，在64位和32位系统上物理地址都是4字节 */
    unsigned int     BufferSize;      /* SOCP编码目的通道数据BUFFER大小 */
    unsigned int overTime;        /* SOCP编码目的通道数据传输超时时间 */
    unsigned int logOnFlag;       /* LOG2.0功能启动标志 */
}UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU;


/*******************************************************************
*测试项: SOCP_MsConvertToCycle1
*被测函数功能描述:时间单位转换
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SOCP_MsConvertToCycle1, UT)
{
    unsigned int    ulDelayTimeInMs = 1200000;
    unsigned int    ulResult = 0x29E9B0;

    EXPECT_EQ(ulResult, uttest_SOCP_MsConvertToCycle_case(ulDelayTimeInMs));

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_EncDstBufFlush1
*被测函数功能描述:LOG 2.0开关未打开
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_EncDstBufFlush1, UT)
{
    UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU stcfg;

    stcfg.logOnFlag = VOS_FALSE;

    MOCKER(SocpGetSDLogCfg)
        .stubs()
        .will(returnValue((void *)&stcfg));

    MOCKER(VOS_SmV)
        .expects(exactly(0))
        .will(returnValue((unsigned int)VOS_ERR));

    uttest_BSP_SOCP_EncDstBufFlush_case();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_EncDstBufFlush2
*被测函数功能描述:获取信号量失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_EncDstBufFlush2, UT)
{
    UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU stcfg;

    stcfg.logOnFlag = VOS_TRUE;

    MOCKER(SocpGetSDLogCfg)
        .stubs()
        .will(returnValue((void *)&stcfg));

    MOCKER(VOS_SmV)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_ERR));

    uttest_BSP_SOCP_EncDstBufFlush_case();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_EncDstBufFlush1
*被测函数功能描述:获取信号量成功
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_EncDstBufFlush3, UT)
{
    UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU stcfg;

    stcfg.logOnFlag = VOS_TRUE;

    MOCKER(SocpGetSDLogCfg)
        .stubs()
        .will(returnValue((void *)&stcfg));

    MOCKER(VOS_SmV)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_GetSlice)
        .expects(exactly(1))
        .will(returnValue((unsigned int)0));

    uttest_BSP_SOCP_EncDstBufFlush_case();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpEncDstTaskProc1
*被测函数功能描述:通道未配置
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpEncDstTaskProc1, UT)
{
    MOCKER(OM_GetSlice)
        .expects(exactly(0))
        .will(returnObjectList((unsigned int)10, (unsigned int)20));

    uttest_SocpEncDstTaskProc_case1(0);

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpEncDstTaskProc2
*被测函数功能描述:回调函数未注册
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpEncDstTaskProc2, UT)
{
    MOCKER(OM_GetSlice)
        .expects(exactly(0))
        .will(returnObjectList((unsigned int)10, (unsigned int)20));

    uttest_SocpEncDstTaskProc_case2(0);

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpEncDstTaskProc3
*被测函数功能描述:回调函数已注册
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpEncDstTaskProc3, UT)
{
    MOCKER(OM_GetSlice)
        .expects(exactly(2))
        .will(returnObjectList((unsigned int)10, (unsigned int)20));

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    uttest_SocpEncDstTaskProc_case3(0);

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpHandlerEncDstTrfInt1
*被测函数功能描述:处理编码目的溢出中断
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpHandlerEncDstTrfInt1, UT)
{
    MOCKER(OM_GetSlice)
        .expects(exactly(2))
        .will(returnObjectList((unsigned int)10, (unsigned int)20));

    MOCKER(VOS_SmV)
        .expects(exactly(2))
        .will(returnValue((unsigned int)VOS_OK));

    uttest_SocpHandlerEncDstTrfInt_case1();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpHandlerEncDst1
*被测函数功能描述:编码目的传输中断处理
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpHandlerEncDst1, UT)
{
    MOCKER(SocpHandlerEncDstTrfInt)
        .expects(exactly(1));

    uttest_SocpHandlerEncDst_case1();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpHandlerEncDst2
*被测函数功能描述:编码目的溢出中断处理
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpHandlerEncDst2, UT)
{
    MOCKER(SocpHandlerEncDstOutOvfInt)
        .expects(exactly(1));

    uttest_SocpHandlerEncDst_case2();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpHandlerEncDst3
*被测函数功能描述:中断位不正确
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpHandlerEncDst3, UT)
{
    MOCKER(SocpHandlerEncDstTrfInt)
        .expects(exactly(0));

    MOCKER(SocpHandlerEncDstOutOvfInt)
        .expects(exactly(0));

    uttest_SocpHandlerEncDst_case3();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpHandlerDecSrc1
*被测函数功能描述:中断位错误
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpHandlerDecSrc1, UT)
{
    uttest_SocpHandlerDecSrc_case1();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpHandlerDecSrc2
*被测函数功能描述:中断位正确
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpHandlerDecSrc2, UT)
{
    uttest_SocpHandlerDecSrc_case2();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpHandlerDecDst1
*被测函数功能描述:中断位错误
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpHandlerDecDst1, UT)
{
    uttest_SocpHandlerDecDst_case1();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpHandlerDecDst2
*被测函数功能描述:解码目的传输中断
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpHandlerDecDst2, UT)
{
    uttest_SocpHandlerDecDst_case2();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpHandlerDecDst3
*被测函数功能描述:解码目的溢出中断
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpHandlerDecDst3, UT)
{
    uttest_SocpHandlerDecDst_case3();

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetDestChan1
*被测函数功能描述:解码目的通道：bufThreshold超过上限
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_DecoderSetDestChan1, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_INVALID_PARA, uttest_BSP_SOCP_DecoderSetDestChan_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetDestChan2
*被测函数功能描述:解码目的通道：buflength超过上限
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_DecoderSetDestChan2, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_INVALID_PARA, uttest_BSP_SOCP_DecoderSetDestChan_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetDestChan3
*被测函数功能描述:解码目的通道配置成功
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_DecoderSetDestChan3, UT)
{
    EXPECT_EQ(VOS_OK, uttest_BSP_SOCP_DecoderSetDestChan_case3());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr1
*被测函数功能描述:编码目的通道:未完成初始化
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetDestChanAttr1, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_NOT_INIT, uttest_BSP_SOCP_CoderSetDestChanAttr_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr2
*被测函数功能描述:编码目的通道:buflength超过上限
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetDestChanAttr2, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_INVALID_PARA, uttest_BSP_SOCP_CoderSetDestChanAttr_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr3
*被测函数功能描述:编码目的通道:bufThreshold超过上限
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetDestChanAttr3, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_INVALID_PARA, uttest_BSP_SOCP_CoderSetDestChanAttr_case3());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr4
*被测函数功能描述:编码目的通道:u32Thrh异常
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetDestChanAttr4, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_INVALID_PARA, uttest_BSP_SOCP_CoderSetDestChanAttr_case4());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr5
*被测函数功能描述:编码目的通道:通道已经配置过
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetDestChanAttr5, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_SET_FAIL, uttest_BSP_SOCP_CoderSetDestChanAttr_case5());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr6
*被测函数功能描述:编码目的通道初始化成功
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetDestChanAttr6, UT)
{
    EXPECT_EQ(VOS_OK, uttest_BSP_SOCP_CoderSetDestChanAttr_case6());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetSrcChanAttr1
*被测函数功能描述:解码源通道：未完成初始化
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_DecoderSetSrcChanAttr1, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_NOT_INIT, uttest_BSP_SOCP_DecoderSetSrcChanAttr_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetSrcChanAttr2
*被测函数功能描述:解码源通道：buflength超过SOCP_DEC_SRC_BUFLGTH_MAX
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_DecoderSetSrcChanAttr2, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_INVALID_PARA, uttest_BSP_SOCP_DecoderSetSrcChanAttr_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetSrcChanAttr3
*被测函数功能描述:解码源通道：通道已经初始化
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_DecoderSetSrcChanAttr3, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_DECSRC_SET, uttest_BSP_SOCP_DecoderSetSrcChanAttr_case3());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetSrcChanAttr3
*被测函数功能描述:解码源通道配置成功
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_DecoderSetSrcChanAttr4, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(VOS_OK, uttest_BSP_SOCP_DecoderSetSrcChanAttr_case4());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuCreateTask1
*被测函数功能描述:创建CH0SCP信号量失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuCreateTask1, UT)
{
    MOCKER(VOS_SmCCreate)
        .stubs()
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(VOS_CreateTask)
        .expects(exactly(0))
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_SEM_CREATE, uttest_SocpAcpuCreateTask_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuCreateTask2
*被测函数功能描述:创建EncDstCh0任务失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuCreateTask2, UT)
{
    MOCKER(VOS_SmCCreate)
        .stubs()
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(VOS_CreateTask)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_ERR));

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_TSK_CREATE, uttest_SocpAcpuCreateTask_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuCreateTask3
*被测函数功能描述:创建CH1SCP信号量失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuCreateTask3, UT)
{
    MOCKER(VOS_SmCCreate)
        .expects(exactly(2))
        .will(returnObjectList ((unsigned int)VOS_OK, (unsigned int)VOS_ERR));

    MOCKER(VOS_CreateTask)
        .expects(exactly(1))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_SEM_CREATE, uttest_SocpAcpuCreateTask_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuCreateTask4
*被测函数功能描述:创建EncDstCh1任务失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuCreateTask4, UT)
{
    MOCKER(VOS_SmCCreate)
        .expects(exactly(2))
        .will(returnObjectList ((unsigned int)VOS_OK, (unsigned int)VOS_OK));

    MOCKER(VOS_CreateTask)
        .expects(exactly(2))
        .will(returnObjectList ((unsigned int)VOS_OK, (unsigned int)VOS_ERR));

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_TSK_CREATE, uttest_SocpAcpuCreateTask_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuCreateTask5
*被测函数功能描述:创建任务成功
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuCreateTask5, UT)
{
    MOCKER(VOS_SmCCreate)
        .expects(exactly(2))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(VOS_CreateTask)
        .expects(exactly(2))
        .will(returnValue((unsigned int)VOS_OK));

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(VOS_OK, uttest_SocpAcpuCreateTask_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpCoreInit1
*被测函数功能描述:SOCP器件初始化
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpCoreInit1, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(VOS_OK, uttest_SocpCoreInit_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuInit1
*被测函数功能描述:ACPU SOCP器件初始化:ioremap返回地址为0
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuInit1, UT)
{
    UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU stcfg;

    stcfg.logOnFlag = VOS_TRUE;

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    MOCKER(SocpGetSDLogCfg)
        .expects(exactly(1))
        .will(returnValue((void *)&stcfg));

    MOCKER(BSP_GetIPBaseAddr)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)0));

    MOCKER(__arm_ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)0));

    EXPECT_EQ(MDRV_ERROR, uttest_SocpAcpuInit_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuInit2
*被测函数功能描述:ACPU SOCP器件初始化:获取中断号失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuInit2, UT)
{
    UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU stcfg;

    stcfg.logOnFlag = VOS_TRUE;

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    MOCKER(SocpGetSDLogCfg)
        .expects(exactly(1))
        .will(returnValue((void *)&stcfg));

    MOCKER(BSP_GetIPBaseAddr)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__arm_ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(SocpCoreInit)
        .expects(exactly(1));

    MOCKER(SOCP_MsConvertToCycle)
        .expects(exactly(1))
        .will(returnValue((unsigned int)0));

    MOCKER(BSP_GetIntNO)
        .expects(exactly(1))
        .will(returnValue((int)MDRV_ERROR));

    MOCKER(BSP_INT_Connect)
        .expects(exactly(1))
        .will(returnValue((int)VOS_ERR));


    EXPECT_EQ(MDRV_ERROR, uttest_SocpAcpuInit_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuInit3
*被测函数功能描述:ACPU SOCP器件初始化:中断挂接失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuInit3, UT)
{
    UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU stcfg;

    stcfg.logOnFlag = VOS_TRUE;

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    MOCKER(SocpGetSDLogCfg)
        .expects(exactly(1))
        .will(returnValue((void *)&stcfg));

    MOCKER(BSP_GetIPBaseAddr)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__arm_ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(SocpCoreInit)
        .expects(exactly(1));

    MOCKER(SOCP_MsConvertToCycle)
        .expects(exactly(1))
        .will(returnValue((unsigned int)0));

    MOCKER(BSP_GetIntNO)
        .expects(exactly(1))
        .will(returnValue((int)MDRV_OK));

    MOCKER(BSP_INT_Connect)
        .expects(exactly(1))
        .will(returnValue((int)VOS_ERR));


    EXPECT_EQ(MDRV_ERROR, uttest_SocpAcpuInit_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuInit4
*被测函数功能描述:ACPU SOCP器件初始化:中断使能失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuInit4, UT)
{
    UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU stcfg;

    stcfg.logOnFlag = VOS_TRUE;

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    MOCKER(SocpGetSDLogCfg)
        .expects(exactly(1))
        .will(returnValue((void *)&stcfg));

    MOCKER(BSP_GetIPBaseAddr)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__arm_ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(SocpCoreInit)
        .expects(exactly(1));

    MOCKER(SOCP_MsConvertToCycle)
        .expects(exactly(1))
        .will(returnValue((unsigned int)0));

    MOCKER(BSP_GetIntNO)
        .expects(exactly(1))
        .will(returnValue((int)MDRV_OK));

    MOCKER(BSP_INT_Connect)
        .expects(exactly(1))
        .will(returnValue((int)VOS_OK));

    MOCKER(BSP_INT_Enable)
        .expects(exactly(1))
        .will(returnValue((int)VOS_ERR));

    EXPECT_EQ(MDRV_ERROR, uttest_SocpAcpuInit_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuInit5
*被测函数功能描述:ACPU SOCP器件初始化:A核创建任务失败
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuInit5, UT)
{
    UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU stcfg;

    stcfg.logOnFlag = VOS_TRUE;

    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    MOCKER(SocpGetSDLogCfg)
        .expects(exactly(1))
        .will(returnValue((void *)&stcfg));

    MOCKER(BSP_GetIPBaseAddr)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__arm_ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(SocpCoreInit)
        .expects(exactly(1));

    MOCKER(SOCP_MsConvertToCycle)
        .expects(exactly(1))
        .will(returnValue((unsigned int)0));

    MOCKER(BSP_GetIntNO)
        .expects(exactly(1))
        .will(returnValue((int)MDRV_OK));

    MOCKER(BSP_INT_Connect)
        .expects(exactly(1))
        .will(returnValue((int)VOS_OK));

    MOCKER(BSP_INT_Enable)
        .expects(exactly(1))
        .will(returnValue((int)VOS_OK));

    MOCKER(SocpAcpuCreateTask)
        .expects(exactly(1))
        .will(returnValue((int)VOS_ERR));

    EXPECT_EQ(MDRV_ERROR, uttest_SocpAcpuInit_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: SocpAcpuInit6
*被测函数功能描述:ACPU SOCP器件初始化:A核创建任务成功
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(SocpAcpuInit6, UT)
{
    UTTEST_BSP_SOCP_ENC_DST_BUF_LOG_CFG_STRU stcfg;

    stcfg.logOnFlag = VOS_TRUE;

    MOCKER(SocpGetSDLogCfg)
        .expects(exactly(1))
        .will(returnValue((void *)&stcfg));

    MOCKER(BSP_GetIPBaseAddr)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(__arm_ioremap)
        .expects(exactly(1))
        .will(returnValue((void *)uttest_SocpAcpuInit_GetaulRegAddr()));

    MOCKER(SocpCoreInit)
        .expects(exactly(1));

    MOCKER(SOCP_MsConvertToCycle)
        .expects(exactly(1))
        .will(returnValue((unsigned int)0));

    MOCKER(BSP_GetIntNO)
        .expects(exactly(1))
        .will(returnValue((int)MDRV_OK));

    MOCKER(BSP_INT_Connect)
        .expects(exactly(1))
        .will(returnValue((int)VOS_OK));

    MOCKER(BSP_INT_Enable)
        .expects(exactly(1))
        .will(returnValue((int)VOS_OK));

    MOCKER(SocpAcpuCreateTask)
        .expects(exactly(1))
        .will(returnValue((int)VOS_OK));

    EXPECT_EQ(VOS_OK, uttest_SocpAcpuInit_case());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetSrcChan1
*被测函数功能描述:未完成初始化
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月18日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetSrcChan1, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_NOT_INIT, uttest_BSP_SOCP_CoderSetSrcChan_case1());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetSrcChan2
*被测函数功能描述:eMode不正确
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月18日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetSrcChan2, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_INVALID_PARA, uttest_BSP_SOCP_CoderSetSrcChan_case2());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetSrcChan3
*被测函数功能描述:buflength超过上限
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月18日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetSrcChan3, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_INVALID_PARA, uttest_BSP_SOCP_CoderSetSrcChan_case3());

    GlobalMockObject::reset();
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetSrcChan4
*被测函数功能描述:Rdbuflength超过上限
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月18日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
TEST(BSP_SOCP_CoderSetSrcChan4, UT)
{
    MOCKER(OM_Inner_Log)
        .expects(exactly(1));

    EXPECT_EQ(BSP_ERR_SOCP_INVALID_PARA, uttest_BSP_SOCP_CoderSetSrcChan_case4());

    GlobalMockObject::reset();
}


