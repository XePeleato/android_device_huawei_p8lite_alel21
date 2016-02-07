/* 头文件包含 */
#include "SOCPInterface.h"
#include "BSP_SOCP_DRV.h"

extern VOS_UINT32  SOCP_MsConvertToCycle(VOS_UINT32 ulDelayTimeInMs);

extern SOCP_GBLSTATE_STRU              g_strSocpStat;

VOS_UINT32 g_aulReg[SOCP_REGISTER_SIZE] = {0};

/*******************************************************************
*测试项: SOCP_MsConvertToCycle
*被测函数功能描述:时间单位转换
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_SOCP_MsConvertToCycle_case(VOS_UINT32 ulDelayTimeInMs)
{
    return SOCP_MsConvertToCycle(ulDelayTimeInMs);
}

/*******************************************************************
*测试项: BSP_SOCP_EncDstBufFlush
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_BSP_SOCP_EncDstBufFlush_case(VOS_VOID)
{
    BSP_SOCP_EncDstBufFlush();
}

/*******************************************************************
*测试项: SocpEncDstTaskProc
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpEncDstTaskProc_case1(VOS_UINT32 ulRealChannelId)
{
    g_strSocpStat.sEncDstChan[ulRealChannelId].u32SetStat = SOCP_CHN_UNSET;

    SocpEncDstTaskProc(ulRealChannelId);
}

/*******************************************************************
*测试项: SocpEncDstTaskProc
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpEncDstTaskProc_case2(VOS_UINT32 ulRealChannelId)
{
    g_strSocpStat.sEncDstChan[ulRealChannelId].u32SetStat = SOCP_CHN_SET;
    g_strSocpStat.sEncDstChan[ulRealChannelId].read_cb    = VOS_NULL_PTR;

    SocpEncDstTaskProc(ulRealChannelId);
}

VOS_UINT32 uttest_SocpEncDstCB(VOS_UINT32 u32ChanID)
{
    return VOS_OK;
}

/*******************************************************************
*测试项: SocpEncDstTaskProc
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpEncDstTaskProc_case3(VOS_UINT32 ulRealChannelId)
{
    g_strSocpStat.sEncDstChan[ulRealChannelId].u32SetStat = SOCP_CHN_SET;
    g_strSocpStat.sEncDstChan[ulRealChannelId].read_cb    = uttest_SocpEncDstCB;

    SocpEncDstTaskProc(ulRealChannelId);
}

/*******************************************************************
*测试项: SocpHandlerEncDstTrfInt
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpHandlerEncDstTrfInt_case1(VOS_VOID)
{
    memset(g_aulReg, 0, sizeof(g_aulReg));

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    g_aulReg[0x5C] = 3;

    SocpHandlerEncDstTrfInt();
}

/*******************************************************************
*测试项: SocpHandlerEncDst
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpHandlerEncDst_case1(VOS_VOID)
{
    memset(g_aulReg, 0, sizeof(g_aulReg));

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    g_aulReg[0x50] = 1 << 15;

    SocpHandlerEncDst();
}

/*******************************************************************
*测试项: SocpHandlerEncDst
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpHandlerEncDst_case2(VOS_VOID)
{
    memset(g_aulReg, 0, sizeof(g_aulReg));

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    g_aulReg[0x50] = 1 << 12;

    SocpHandlerEncDst();
}

/*******************************************************************
*测试项: SocpHandlerEncDst
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpHandlerEncDst_case3(VOS_VOID)
{
    memset(g_aulReg, 0, sizeof(g_aulReg));

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    g_aulReg[0x50] = 0;

    SocpHandlerEncDst();
}

/*******************************************************************
*测试项: SocpHandlerDecSrc
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpHandlerDecSrc_case1(VOS_VOID)
{
    memset(g_aulReg, 0, sizeof(g_aulReg));

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    g_aulReg[0x50] = 0;

    SocpHandlerDecSrc();
}

/*******************************************************************
*测试项: SocpHandlerDecSrc
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpHandlerDecSrc_case2(VOS_VOID)
{
    memset(g_aulReg, 0, sizeof(g_aulReg));

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    g_aulReg[0x50] = 2;
    g_aulReg[0xC4] = 0x1111110;

    SocpHandlerDecSrc();
}

/*******************************************************************
*测试项: SocpHandlerDecDst
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpHandlerDecDst_case1(VOS_VOID)
{
    memset(g_aulReg, 0, sizeof(g_aulReg));

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    g_aulReg[0x50] = 0;

    SocpHandlerDecSrc();
}

/*******************************************************************
*测试项: SocpHandlerDecDst
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpHandlerDecDst_case2(VOS_VOID)
{
    memset(g_aulReg, 0, sizeof(g_aulReg));

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    g_aulReg[0x50] = SOCP_CORE0_DEC_TFRINT_MASK;
    g_aulReg[0xB4] = 1;

    SocpHandlerDecSrc();
}

/*******************************************************************
*测试项: SocpHandlerDecDst
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_VOID uttest_SocpHandlerDecDst_case3(VOS_VOID)
{
    memset(g_aulReg, 0, sizeof(g_aulReg));

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    g_aulReg[0x50] = SOCP_CORE0_DEC_OUTOVFINT_MASK;
    g_aulReg[0xD4] = 1;

    SocpHandlerDecSrc();
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetDestChan
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_DecoderSetDestChan_case1(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32    enDestChanID;
    SOCP_DECODER_DEST_CHAN_STRU  stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enDestChanID = SOCP_DECODER_DST_GUOM;

    stAttr.eDataType = SOCP_DATA_TYPE_0;
    stAttr.u32SrcChanID = SOCP_DECODER_SRC_LOM;
    stAttr.sDecoderDstSetBuf.pucOutputStart = (VOS_UINT8 *)8;
    stAttr.sDecoderDstSetBuf.pucOutputEnd = (VOS_UINT8 *)15;
    stAttr.sDecoderDstSetBuf.u32Threshold = 0xFFFF;


    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;


    return BSP_SOCP_DecoderSetDestChan(enDestChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetDestChan
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_DecoderSetDestChan_case2(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32    enDestChanID;
    SOCP_DECODER_DEST_CHAN_STRU  stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enDestChanID = SOCP_DECODER_DST_GUOM;

    stAttr.eDataType = SOCP_DATA_TYPE_0;
    stAttr.u32SrcChanID = SOCP_DECODER_SRC_LOM;
    stAttr.sDecoderDstSetBuf.pucOutputStart = (VOS_UINT8 *)8;
    stAttr.sDecoderDstSetBuf.pucOutputEnd = (VOS_UINT8 *)0x10007;
    stAttr.sDecoderDstSetBuf.u32Threshold = 0x11;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    return BSP_SOCP_DecoderSetDestChan(enDestChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetDestChan
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_DecoderSetDestChan_case3(VOS_VOID)
{
    SOCP_DECODER_DST_ENUM_U32    enDestChanID;
    SOCP_DECODER_DEST_CHAN_STRU  stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enDestChanID = SOCP_DECODER_DST_GUOM;

    stAttr.eDataType = SOCP_DATA_TYPE_0;
    stAttr.u32SrcChanID = SOCP_DECODER_SRC_LOM;
    stAttr.sDecoderDstSetBuf.pucOutputStart = (VOS_UINT8 *)8;
    stAttr.sDecoderDstSetBuf.pucOutputEnd = (VOS_UINT8 *)15;
    stAttr.sDecoderDstSetBuf.u32Threshold = 0x11;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    return BSP_SOCP_DecoderSetDestChan(enDestChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_CoderSetDestChanAttr_case1(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32      enDestChanID;
    SOCP_CODER_DEST_CHAN_S       stAttr;

    g_strSocpStat.bInitFlag = VOS_FALSE;

    enDestChanID = SOCP_CODER_DST_OM_CNF;

    stAttr.u32EncDstThrh = 500;
    stAttr.sCoderSetDstBuf.pucOutputStart = (VOS_UINT8 *)8;
    stAttr.sCoderSetDstBuf.pucOutputEnd   = (VOS_UINT8 *)0x200007;
    stAttr.sCoderSetDstBuf.u32Threshold   = 0x11;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    SOCP_REG_WRITE(SOCP_REG_VERSION, SOCP_OLD_VER);

    return BSP_SOCP_CoderSetDestChanAttr(enDestChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_CoderSetDestChanAttr_case2(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32      enDestChanID;
    SOCP_CODER_DEST_CHAN_S       stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enDestChanID = SOCP_CODER_DST_OM_CNF;

    stAttr.u32EncDstThrh = 500;
    stAttr.sCoderSetDstBuf.pucOutputStart = (VOS_UINT8 *)8;
    stAttr.sCoderSetDstBuf.pucOutputEnd   = (VOS_UINT8 *)0x200007;
    stAttr.sCoderSetDstBuf.u32Threshold   = 0x11;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    SOCP_REG_WRITE(SOCP_REG_VERSION, SOCP_OLD_VER);

    return BSP_SOCP_CoderSetDestChanAttr(enDestChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_CoderSetDestChanAttr_case3(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32      enDestChanID;
    SOCP_CODER_DEST_CHAN_S       stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enDestChanID = SOCP_CODER_DST_OM_CNF;

    stAttr.u32EncDstThrh = 500;
    stAttr.sCoderSetDstBuf.pucOutputStart = (VOS_UINT8 *)8;
    stAttr.sCoderSetDstBuf.pucOutputEnd   = (VOS_UINT8 *)15;
    stAttr.sCoderSetDstBuf.u32Threshold   = 0x800;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    SOCP_REG_WRITE(SOCP_REG_VERSION, SOCP_OLD_VER);

    return BSP_SOCP_CoderSetDestChanAttr(enDestChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_CoderSetDestChanAttr_case4(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32      enDestChanID;
    SOCP_CODER_DEST_CHAN_S       stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enDestChanID = SOCP_CODER_DST_OM_CNF;

    stAttr.u32EncDstThrh = 255;
    stAttr.sCoderSetDstBuf.pucOutputStart = (VOS_UINT8 *)8;
    stAttr.sCoderSetDstBuf.pucOutputEnd   = (VOS_UINT8 *)15;
    stAttr.sCoderSetDstBuf.u32Threshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    SOCP_REG_WRITE(SOCP_REG_VERSION, SOCP_OLD_VER);

    return BSP_SOCP_CoderSetDestChanAttr(enDestChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr
*被测函数功能描述:通道已经配置过
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_CoderSetDestChanAttr_case5(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32      enDestChanID;
    SOCP_CODER_DEST_CHAN_S       stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enDestChanID = SOCP_CODER_DST_OM_CNF;

    stAttr.u32EncDstThrh = 500;
    stAttr.sCoderSetDstBuf.pucOutputStart = (VOS_UINT8 *)8;
    stAttr.sCoderSetDstBuf.pucOutputEnd   = (VOS_UINT8 *)15;
    stAttr.sCoderSetDstBuf.u32Threshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.sEncDstChan[SOCP_REAL_CHAN_ID(enDestChanID)].u32SetStat = SOCP_CHN_SET;

    SOCP_REG_WRITE(SOCP_REG_VERSION, SOCP_OLD_VER);

    return BSP_SOCP_CoderSetDestChanAttr(enDestChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetDestChanAttr
*被测函数功能描述:通道已经配置过
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_CoderSetDestChanAttr_case6(VOS_VOID)
{
    SOCP_CODER_DST_ENUM_U32      enDestChanID;
    SOCP_CODER_DEST_CHAN_S       stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enDestChanID = SOCP_CODER_DST_OM_CNF;

    stAttr.u32EncDstThrh = 500;
    stAttr.sCoderSetDstBuf.pucOutputStart = (VOS_UINT8 *)8;
    stAttr.sCoderSetDstBuf.pucOutputEnd   = (VOS_UINT8 *)15;
    stAttr.sCoderSetDstBuf.u32Threshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.sEncDstChan[SOCP_REAL_CHAN_ID(enDestChanID)].u32SetStat = SOCP_CHN_UNSET;

    SOCP_REG_WRITE(SOCP_REG_VERSION, SOCP_OLD_VER);

    return BSP_SOCP_CoderSetDestChanAttr(enDestChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetSrcChanAttr
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_DecoderSetSrcChanAttr_case1(VOS_VOID)
{
    SOCP_DECODER_SRC_ENUM_U32    enSrcChanID;
    SOCP_DECODER_SRC_CHAN_STRU   stAttr;

    g_strSocpStat.bInitFlag = VOS_FALSE;

    enSrcChanID = SOCP_DECODER_SRC_LOM;

    stAttr.eDataTypeEn = SOCP_DATA_TYPE_EN;

    stAttr.sDecoderSetSrcBuf.pucInputStart = (VOS_UINT8 *)8;
    stAttr.sDecoderSetSrcBuf.pucInputEnd   = (VOS_UINT8 *)0x10007;
    stAttr.sDecoderSetSrcBuf.pucRDStart    = (VOS_UINT8 *)8;
    stAttr.sDecoderSetSrcBuf.pucRDEnd      = (VOS_UINT8 *)15;
    stAttr.sDecoderSetSrcBuf.u32RDThreshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.sEncDstChan[SOCP_REAL_CHAN_ID(enSrcChanID)].u32SetStat = SOCP_CHN_UNSET;

    return BSP_SOCP_DecoderSetSrcChanAttr(enSrcChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetSrcChanAttr
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_DecoderSetSrcChanAttr_case2(VOS_VOID)
{
    SOCP_DECODER_SRC_ENUM_U32    enSrcChanID;
    SOCP_DECODER_SRC_CHAN_STRU   stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enSrcChanID = SOCP_DECODER_SRC_LOM;

    stAttr.eDataTypeEn = SOCP_DATA_TYPE_EN;

    stAttr.sDecoderSetSrcBuf.pucInputStart = (VOS_UINT8 *)8;
    stAttr.sDecoderSetSrcBuf.pucInputEnd   = (VOS_UINT8 *)0x10007;
    stAttr.sDecoderSetSrcBuf.pucRDStart    = (VOS_UINT8 *)8;
    stAttr.sDecoderSetSrcBuf.pucRDEnd      = (VOS_UINT8 *)15;
    stAttr.sDecoderSetSrcBuf.u32RDThreshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.sEncDstChan[SOCP_REAL_CHAN_ID(enSrcChanID)].u32SetStat = SOCP_CHN_UNSET;

    return BSP_SOCP_DecoderSetSrcChanAttr(enSrcChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetSrcChanAttr
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_DecoderSetSrcChanAttr_case3(VOS_VOID)
{
    SOCP_DECODER_SRC_ENUM_U32    enSrcChanID;
    SOCP_DECODER_SRC_CHAN_STRU   stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enSrcChanID = SOCP_DECODER_SRC_LOM;

    stAttr.eDataTypeEn = SOCP_DATA_TYPE_EN;

    stAttr.sDecoderSetSrcBuf.pucInputStart = (VOS_UINT8 *)8;
    stAttr.sDecoderSetSrcBuf.pucInputEnd   = (VOS_UINT8 *)15;
    stAttr.sDecoderSetSrcBuf.pucRDStart    = (VOS_UINT8 *)8;
    stAttr.sDecoderSetSrcBuf.pucRDEnd      = (VOS_UINT8 *)15;
    stAttr.sDecoderSetSrcBuf.u32RDThreshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.sDecSrcChan[SOCP_REAL_CHAN_ID(enSrcChanID)].u32SetStat = SOCP_CHN_SET;

    return BSP_SOCP_DecoderSetSrcChanAttr(enSrcChanID, &stAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_DecoderSetSrcChanAttr
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT32 uttest_BSP_SOCP_DecoderSetSrcChanAttr_case4(VOS_VOID)
{
    SOCP_DECODER_SRC_ENUM_U32    enSrcChanID;
    SOCP_DECODER_SRC_CHAN_STRU   stAttr;

    g_strSocpStat.bInitFlag = VOS_TRUE;

    enSrcChanID = SOCP_DECODER_SRC_LOM;

    stAttr.eDataTypeEn = SOCP_DATA_TYPE_EN;

    stAttr.sDecoderSetSrcBuf.pucInputStart = (VOS_UINT8 *)8;
    stAttr.sDecoderSetSrcBuf.pucInputEnd   = (VOS_UINT8 *)15;
    stAttr.sDecoderSetSrcBuf.pucRDStart    = (VOS_UINT8 *)8;
    stAttr.sDecoderSetSrcBuf.pucRDEnd      = (VOS_UINT8 *)15;
    stAttr.sDecoderSetSrcBuf.u32RDThreshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.sDecSrcChan[SOCP_REAL_CHAN_ID(enSrcChanID)].u32SetStat = SOCP_CHN_UNSET;

    return BSP_SOCP_DecoderSetSrcChanAttr(enSrcChanID, &stAttr);
}

/*******************************************************************
*测试项: SocpAcpuCreateTask
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_INT32 uttest_SocpAcpuCreateTask_case(VOS_VOID)
{
    return SocpAcpuCreateTask();
}

/*******************************************************************
*测试项: SocpCoreInit
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_INT32 uttest_SocpCoreInit_case(VOS_VOID)
{
    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    return SocpCoreInit();
}

/*******************************************************************
*测试项: SocpAcpuInit
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_INT32 uttest_SocpAcpuInit_case(VOS_VOID)
{
    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;

    return SocpAcpuInit();
}

/*******************************************************************
*测试项: SocpAcpuInit
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月17日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_UINT_PTR uttest_SocpAcpuInit_GetaulRegAddr(VOS_VOID)
{
    return (VOS_UINT_PTR)g_aulReg;
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetSrcChan
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月18日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_INT32 uttest_BSP_SOCP_CoderSetSrcChan_case1(VOS_VOID)
{
    SOCP_CODER_SRC_ENUM_U32       enSrcChanID;
    SOCP_CODER_SRC_CHAN_S         stSrcAttr;

    enSrcChanID = SOCP_CODER_SRC_LOM_CNF1;

    stSrcAttr.u32DestChanID = SOCP_CODER_DST_OM_CNF;
    stSrcAttr.eDataType     = SOCP_DATA_TYPE_0;
    stSrcAttr.eDataTypeEn   = SOCP_DATA_TYPE_EN;
    stSrcAttr.eDebugEn      = SOCP_ENC_DEBUG_DIS;
    stSrcAttr.eMode         = SOCP_ENCSRC_CHNMODE_BUTT;
    stSrcAttr.ePriority     = SOCP_CHAN_PRIORITY_0;

    stSrcAttr.sCoderSetSrcBuf.pucInputStart = (VOS_UINT8 *)8;
    stSrcAttr.sCoderSetSrcBuf.pucInputEnd   = (VOS_UINT8 *)15;
    stSrcAttr.sCoderSetSrcBuf.pucRDStart    = (VOS_UINT8 *)8;
    stSrcAttr.sCoderSetSrcBuf.pucRDEnd      = (VOS_UINT8 *)15;
    stSrcAttr.sCoderSetSrcBuf.u32RDThreshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.bInitFlag = VOS_FALSE;

    return BSP_SOCP_CoderSetSrcChan(enSrcChanID, &stSrcAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetSrcChan
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月18日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_INT32 uttest_BSP_SOCP_CoderSetSrcChan_case2(VOS_VOID)
{
    SOCP_CODER_SRC_ENUM_U32       enSrcChanID;
    SOCP_CODER_SRC_CHAN_S         stSrcAttr;

    enSrcChanID = SOCP_CODER_SRC_LOM_CNF1;

    stSrcAttr.u32DestChanID = SOCP_CODER_DST_OM_CNF;
    stSrcAttr.eDataType     = SOCP_DATA_TYPE_0;
    stSrcAttr.eDataTypeEn   = SOCP_DATA_TYPE_EN;
    stSrcAttr.eDebugEn      = SOCP_ENC_DEBUG_DIS;
    stSrcAttr.eMode         = SOCP_ENCSRC_CHNMODE_BUTT;
    stSrcAttr.ePriority     = SOCP_CHAN_PRIORITY_0;

    stSrcAttr.sCoderSetSrcBuf.pucInputStart = (VOS_UINT8 *)8;
    stSrcAttr.sCoderSetSrcBuf.pucInputEnd   = (VOS_UINT8 *)15;
    stSrcAttr.sCoderSetSrcBuf.pucRDStart    = (VOS_UINT8 *)8;
    stSrcAttr.sCoderSetSrcBuf.pucRDEnd      = (VOS_UINT8 *)15;
    stSrcAttr.sCoderSetSrcBuf.u32RDThreshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.bInitFlag = VOS_TRUE;

    return BSP_SOCP_CoderSetSrcChan(enSrcChanID, &stSrcAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetSrcChan
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月18日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_INT32 uttest_BSP_SOCP_CoderSetSrcChan_case3(VOS_VOID)
{
    SOCP_CODER_SRC_ENUM_U32       enSrcChanID;
    SOCP_CODER_SRC_CHAN_S         stSrcAttr;

    enSrcChanID = SOCP_CODER_SRC_LOM_CNF1;

    stSrcAttr.u32DestChanID = SOCP_CODER_DST_OM_CNF;
    stSrcAttr.eDataType     = SOCP_DATA_TYPE_0;
    stSrcAttr.eDataTypeEn   = SOCP_DATA_TYPE_EN;
    stSrcAttr.eDebugEn      = SOCP_ENC_DEBUG_DIS;
    stSrcAttr.eMode         = SOCP_ENCSRC_CHNMODE_CTSPACKET;
    stSrcAttr.ePriority     = SOCP_CHAN_PRIORITY_0;

    stSrcAttr.sCoderSetSrcBuf.pucInputStart = (VOS_UINT8 *)8;
    stSrcAttr.sCoderSetSrcBuf.pucInputEnd   = (VOS_UINT8 *)0x8000007;
    stSrcAttr.sCoderSetSrcBuf.pucRDStart    = (VOS_UINT8 *)8;
    stSrcAttr.sCoderSetSrcBuf.pucRDEnd      = (VOS_UINT8 *)15;
    stSrcAttr.sCoderSetSrcBuf.u32RDThreshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.bInitFlag = VOS_TRUE;

    SOCP_REG_WRITE(SOCP_REG_VERSION, SOCP_OLD_VER);

    return BSP_SOCP_CoderSetSrcChan(enSrcChanID, &stSrcAttr);
}

/*******************************************************************
*测试项: BSP_SOCP_CoderSetSrcChan
*被测函数功能描述:
*预期结果：
************************* 修改记录 *************************
#  1.日    期: 2014年6月18日
#    作    者: L00256032
#    修改内容: 新建CASE
*******************************************************************/
VOS_INT32 uttest_BSP_SOCP_CoderSetSrcChan_case4(VOS_VOID)
{
    SOCP_CODER_SRC_ENUM_U32       enSrcChanID;
    SOCP_CODER_SRC_CHAN_S         stSrcAttr;

    enSrcChanID = SOCP_CODER_SRC_LOM_CNF1;

    stSrcAttr.u32DestChanID = SOCP_CODER_DST_OM_CNF;
    stSrcAttr.eDataType     = SOCP_DATA_TYPE_0;
    stSrcAttr.eDataTypeEn   = SOCP_DATA_TYPE_EN;
    stSrcAttr.eDebugEn      = SOCP_ENC_DEBUG_DIS;
    stSrcAttr.eMode         = SOCP_ENCSRC_CHNMODE_LIST;
    stSrcAttr.ePriority     = SOCP_CHAN_PRIORITY_0;

    stSrcAttr.sCoderSetSrcBuf.pucInputStart = (VOS_UINT8 *)8;
    stSrcAttr.sCoderSetSrcBuf.pucInputEnd   = (VOS_UINT8 *)15;
    stSrcAttr.sCoderSetSrcBuf.pucRDStart    = (VOS_UINT8 *)8;
    stSrcAttr.sCoderSetSrcBuf.pucRDEnd      = (VOS_UINT8 *)0x10007;
    stSrcAttr.sCoderSetSrcBuf.u32RDThreshold   = 0x10;

    g_strSocpStat.uBaseAddr = (VOS_UINT_PTR)g_aulReg;
    g_strSocpStat.bInitFlag = VOS_TRUE;

    return BSP_SOCP_CoderSetSrcChan(enSrcChanID, &stSrcAttr);
}