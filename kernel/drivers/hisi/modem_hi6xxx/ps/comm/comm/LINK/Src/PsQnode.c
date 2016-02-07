/******************************************************************************
  文 件 名   : PsQnode.c
  版 本 号   : 初稿
  作    者   : liukai
  生成日期   : 2013年9月16日
  最近修改   :
  功能描述   : 提供PsQnode功能
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月16日
    作    者   : liukai
    修改内容   : 创建文件
******************************************************************************/

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "PsQnode.h"
#include "TTFComm.h"
#include "om.h"                                         /* for PS_LOG */


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_PS_QNODE_C /*lint !e767*/




/******************************************************************************
   2 外部函数变量声明
******************************************************************************/


/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
******************************************************************************/
/* 存放PsQnode数据的空间, QNode结点支持在多任务环境下使用 */
PS_QNODE_STRU    g_astQnode[MAX_PS_QNODE_NUM];

/* 结点管理结构, 通过usNodeIdx将结点管理结构和结点衔接起来 */
PS_QNODE_MGMT_STRU     g_stMgmt;

#if (VOS_OS_VER == VOS_WIN32)
VOS_UINT32             g_ulPsQnodeNum;
#endif


/******************************************************************************
   5 函数实现
******************************************************************************/
/*****************************************************************************
 函 数 名  : PS_Qnode_Init
 功能描述  : 初始化并使能调试的接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 说    明  : 此函数只允许初始化一次

 修改历史      :
  1.日    期   : 2013年9月16日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID PS_Qnode_Init(VOS_VOID)
{
    VOS_UINT32                  ulLoop;
    VOS_UINT32                  ulCurrOffset;
    const VOS_UINT32            ul32Bit = 32;
    VOS_UINT32                  ulMaxLoop;


    PS_MEM_SET(&g_stMgmt, 0x0, sizeof(g_stMgmt));
    PS_MEM_SET(&g_astQnode[0], 0x0, sizeof(g_astQnode));

    g_stMgmt.enIsTracePrintOn   =   PS_QNODE_TRACE_PRINT_OFF;
    g_stMgmt.usNodeIdx          =   0;

#if (VOS_OS_VER == VOS_WIN32)
    /* 用于PC工程 */
    ulCurrOffset   =   /*lint --e(778)*/TTF_MOD_GET(g_ulPsQnodeNum, ul32Bit);
#else
    ulCurrOffset   =   /*lint --e(778)*/TTF_MOD_GET(MAX_PS_QNODE_NUM, ul32Bit);
#endif
    ulMaxLoop      =   TTF_MOD_GET((ul32Bit - ulCurrOffset), ul32Bit);

    /* 如果MAX_PS_QNODE_NUM不是32的整数倍, 那么将多余的bit置1 */
    for (ulLoop = 0; ulLoop < ulMaxLoop; ulLoop++)    /*lint !e681*/
    {
        TTF_SET_A_BIT((g_stMgmt.aulQuickMap[(MAX_PS_QNODE_4BYTE_ALIGN_NUM - 1)]), ulLoop);
    }

    for (ulLoop = 0; ulLoop < MAX_PS_QNODE_NUM; ulLoop++)
    {
        TTF_NodeInit(&(g_astQnode[ulLoop].stNode));
        g_astQnode[ulLoop].enState  =   PS_QNODE_FREE_STATE;
        g_astQnode[ulLoop].usIndex  =   (VOS_UINT16)ulLoop;
        g_astQnode[ulLoop].usMagic  =   PS_QNODE_MAGICNUM;
    }

    return;
} /* PS_Qnode_Init */

/*****************************************************************************
 函 数 名  : PS_Qnode_GetNode
 功能描述  : 申请Qnode结点
 输入参数  : usFileId   - 文件ID
             usLineId   - 行号
             ulPid      - PID
 输出参数  : 成功, 返回申请的Qnode结点; 失败, 返回NULL
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年9月16日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
PS_QNODE_STRU* PS_Qnode_GetNode(VOS_UINT16 usFileId,
                                VOS_UINT16 usLineId,
                                VOS_UINT32 ulPid)
{
    PS_QNODE_STRU              *pstQnode = VOS_NULL_PTR;
    VOS_UINT16                  usCurrQnodeIdx;
    VOS_UINT32                  ulTick;
    VOS_INT32                   lLockKey;
    const VOS_UINT16            us32Bit = 32;
    VOS_UINT16                  usStartPos;
    VOS_UINT32                  ulCurr32Bit;
    VOS_UINT16                  usCurrPos;
    VOS_UINT32                  ulTmp;
    VOS_UINT32                  ulLoop;
    const VOS_UINT32            ulMask = 0x80000000;//0x00000001;

    ulTick          =   VOS_GetTick();
    usCurrQnodeIdx  =   g_stMgmt.usNodeIdx;
    usStartPos      =   usCurrQnodeIdx / us32Bit;    /* 记录StartPos */
    usCurrPos       =   usStartPos;

    do
    {
        ulCurr32Bit = g_stMgmt.aulQuickMap[usCurrPos];

        if (PS_NULL_UINT32 != ulCurr32Bit)    /* 有值为0的bit, 说明有空位 */
        {
            /* 下边找第一个为1的bit */
            ulTmp   =   ~ulCurr32Bit;

            for (ulLoop = 0; ulLoop < us32Bit; ulLoop++)
            {
                if (ulTmp & ulMask) //找到
                {
                    usCurrQnodeIdx = (VOS_UINT16)(PS_MOD_ADD((usCurrPos * us32Bit), ulLoop, MAX_PS_QNODE_NUM));
                    break;
                }

                ulTmp <<= 1;//>>= 1;
            }

            pstQnode    =   &(g_astQnode[usCurrQnodeIdx]);

            lLockKey    =   VOS_SplIMP();

            if (PS_QNODE_FREE_STATE == pstQnode->enState)
            {
                pstQnode->enState               =   PS_QNODE_USED_STATE;

                pstQnode->stDbg.ulAllocTick     =   ulTick;
                pstQnode->stDbg.usAllocFileId   =   usFileId;
                pstQnode->stDbg.usAllocLineNum  =   usLineId;

                /* 刷新快表 */
                TTF_SET_A_BIT(ulCurr32Bit, (us32Bit - ulLoop - 1));
                g_stMgmt.aulQuickMap[usCurrPos] = ulCurr32Bit;

                /* 回写返回节点的索引值 */
                g_stMgmt.usNodeIdx = PS_MOD_ADD(usCurrQnodeIdx, 1, MAX_PS_QNODE_NUM);//usCurrQnodeIdx;

                /* 刷新统计 */
                g_stMgmt.stStat.ulAllocTotalCnt++;

                VOS_Splx(lLockKey);

                return pstQnode;
            }

            VOS_Splx(lLockKey);
        }

        usCurrPos = PS_MOD_ADD(usCurrPos, 1, MAX_PS_QNODE_4BYTE_ALIGN_NUM);
    }
    while (usCurrPos != usStartPos);

    lLockKey    =   VOS_SplIMP();

    g_stMgmt.stStat.ulAllocFailCnt++;
    g_stMgmt.stStat.ulAllocTotalCnt++;

    VOS_Splx(lLockKey);

    PS_LOG4(ulPid, 0, PS_PRINT_WARNING, "fileid, line, pid, tick, PS_Qnode_GetNode fail",
        usFileId, usLineId, (VOS_INT32)ulPid, (VOS_INT32)ulTick);

    return VOS_NULL_PTR;
} /* PS_Qnode_GetNode */

/*****************************************************************************
 函 数 名  : PS_Qnode_SendEvent
 功能描述  : 上报事件
 输入参数  : ulPid - 操作PID
             enEventId - 事件ID
             pEventData - 事件信息内容
             ulDataLen - 事件信息长度
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年10月9日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID PS_Qnode_SendEvent(VOS_UINT32  ulPid,
                            VOS_UINT16  usEventId,
                            VOS_VOID   *pEventData,
                            VOS_UINT32  ulDataLen)
{
    PS_OM_EVENT_IND_STRU               *pstOmEvent;
    VOS_UINT32                          ulLength;


    /* 计算事件总长度 */
    ulLength = PS_OFFSET_OF(PS_OM_EVENT_IND_STRU, aucData);    /*lint !e545 !e413*/
    ulLength += ulDataLen;

    /* 申请事件内存 */
    pstOmEvent = (PS_OM_EVENT_IND_STRU *)PS_MEM_ALLOC(ulPid, ulLength);

    if (VOS_NULL_PTR == pstOmEvent)
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR, "alloc cttf mntn event fail");

        return;
    }

    /* 填写事件内容 */
    pstOmEvent->ulLength
        = ulLength - sizeof(pstOmEvent->ulLength); /* 不包括ulLength自身所占用的字节 */
    pstOmEvent->usEventId  = usEventId;
    pstOmEvent->ulModuleId = ulPid;

    mdrv_memcpy(pstOmEvent->aucData, pEventData, ulDataLen);

    /* 调用OM的事件上报接口 */
    if ( VOS_OK != OM_Event((PS_OM_EVENT_IND_STRU*)pstOmEvent))
    {
        PS_LOG(ulPid, 0, PS_PRINT_ERROR, "cttf mntn event sent fail");
    }

    /* 释放申请的内存 */
    PS_MEM_FREE(ulPid, pstOmEvent);

    return;
}    /* PS_Qnode_SendEvent */

/*****************************************************************************
 函 数 名  : PS_Qnode_FreeNode
 功能描述  : 释放Qnode结点
 输入参数  : usFileId   - 文件ID
             usLineId   - 行号
             ulPid      - PID
             pstQnode   - Qnode结点
 输出参数  : 无
 返 回 值  : 操作结果, 成功 - PS_SUCC, 否则 - PS_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年9月16日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
PS_RSLT_CODE_ENUM_UINT32 PS_Qnode_FreeNode(VOS_UINT16       usFileId,
                                       VOS_UINT16      usLineId,
                                       VOS_UINT32      ulOpPid,
                                       PS_QNODE_STRU  *pstQnode)
{
    VOS_UINT32                      ulTick;
    VOS_INT32                       lLockKey;
    PS_QNODE_TAMPERED_EVENT_STRU    stEvent;
    const VOS_UINT16                us32Bit = 32;
    VOS_UINT16                      usCurrPos;
    VOS_UINT16                      usCurrOffset;
    VOS_UINT32                      ulQnodeAddr;
    VOS_UINT32                      ulFirstQnodeAddr;
    VOS_UINT32                      ulLastQnodeAddr;


    ulTick  =   VOS_GetTick();

    if (VOS_NULL_PTR == pstQnode)
    {
        lLockKey    =   VOS_SplIMP();

        g_stMgmt.stStat.ulFreeNullPtrCnt++;
        g_stMgmt.stStat.ulFreeTotalCnt++;

        VOS_Splx(lLockKey);

        PS_LOG4(ulOpPid, 0, PS_PRINT_WARNING, "FileId, LineId, Pid, tick, pstQnode is NULL",
            usFileId, usLineId, (VOS_INT32)ulOpPid, (VOS_INT32)ulTick);
        return PS_FAIL;
    }

    ulQnodeAddr         =   (VOS_UINT32)pstQnode;
    ulFirstQnodeAddr    =   (VOS_UINT32)(&g_astQnode[0]);
    ulLastQnodeAddr     =   (VOS_UINT32)(&g_astQnode[MAX_PS_QNODE_NUM - 1]);

    if ((0 != (ulQnodeAddr - ulFirstQnodeAddr) % sizeof(g_astQnode[0]))
        || ((ulQnodeAddr > ulLastQnodeAddr)
        || (ulQnodeAddr < ulFirstQnodeAddr)))
    {
        lLockKey    =   VOS_SplIMP();

        g_stMgmt.stStat.ulFreeInvalidPrtCnt++;
        g_stMgmt.stStat.ulFreeTotalCnt++;

        VOS_Splx(lLockKey);

        PS_LOG4(ulOpPid, 0, PS_PRINT_WARNING, "FileId, LineId, Pid, tick, pstQnode is invalid",
            usFileId, usLineId, (VOS_INT32)ulOpPid, (VOS_INT32)ulTick);
        return PS_FAIL;
    }

    if (PS_QNODE_MAGICNUM == pstQnode->usMagic)    /* 未篡改 */
    {
        if (PS_QNODE_USED_STATE != pstQnode->enState)
        {
            lLockKey    =   VOS_SplIMP();

            g_stMgmt.stStat.ulFreePtrBeforeCnt++;
            g_stMgmt.stStat.ulFreeTotalCnt++;

            VOS_Splx(lLockKey);

            PS_LOG4(ulOpPid, 0, PS_PRINT_WARNING,
                "FileId, LineId, Pid, tick, pstQnode has already been freed before",
                usFileId, usLineId, (VOS_INT32)ulOpPid, (VOS_INT32)ulTick);
            return PS_FAIL;
        }
    }
    else    /* 被篡改 */
    {
        lLockKey    =   VOS_SplIMP();

        g_stMgmt.stStat.ulMagicTamperedCnt++;

        /* 强制恢复 */
        pstQnode->usMagic   =   PS_QNODE_MAGICNUM;
        pstQnode->usIndex   =   (VOS_UINT16)((ulQnodeAddr - ulFirstQnodeAddr) / sizeof(g_astQnode[0]));

        VOS_Splx(lLockKey);

        /* 填写event */
        stEvent.usFileId    =   usFileId;
        stEvent.usLineId    =   usLineId;
        stEvent.ulPid       =   ulOpPid;
        stEvent.ulTick      =   ulTick;
        mdrv_memcpy(&(stEvent.stQnode), pstQnode, sizeof(stEvent.stQnode));

        /* SDT要求event id必须挂接在某个固定的PID下, 暂决定挂在WRLC下 */
        PS_Qnode_SendEvent(WUEPS_PID_RLC, ID_PS_QNODE_FOUND_TAMPERED, &stEvent,
            sizeof(stEvent));

        PS_LOG4(ulOpPid, 0, PS_PRINT_ERROR,
            "FileId, LineId, Pid, Qnode magic number tampered pstQnode",
            usFileId, usLineId, (VOS_INT32)ulOpPid, (VOS_INT32)pstQnode);
        PS_LOG1(ulOpPid, 0, PS_PRINT_ERROR,
            "found magic number tampered pstQnode at tick",
            (VOS_INT32)ulTick);
    }

    lLockKey    =   VOS_SplIMP();

    PS_MEM_SET(&(pstQnode->stMemSrc), 0x0, sizeof(pstQnode->stMemSrc));
    PS_MEM_SET(&(pstQnode->aucPara[0]), 0x0, sizeof(pstQnode->aucPara));

    pstQnode->stDbg.ulTraceTick     =   ulTick;
    pstQnode->stDbg.usTraceFileId   =   usFileId;
    pstQnode->stDbg.usTraceLineNum  =   usLineId;

    /* 刷新快表 */
    usCurrPos      =   (pstQnode->usIndex) / us32Bit;
    usCurrOffset   =   (pstQnode->usIndex) % us32Bit;
    TTF_CLEAR_A_BIT(g_stMgmt.aulQuickMap[usCurrPos], ((us32Bit - 1) - usCurrOffset));

    /* 刷新统计 */
    g_stMgmt.stStat.ulFreeTotalCnt++;

    /* 把更新状态放在最后, 因为放前面, 可能出现getnode中一旦发现state可以申请就取走结点了,
       多线程下会出现freenode未结束, 结点已经被取走使用 */
    pstQnode->enState   =   PS_QNODE_FREE_STATE;

    VOS_Splx(lLockKey);

    return PS_SUCC;
} /* PS_Qnode_FreeNode */

/*****************************************************************************
 函 数 名  : PS_Qnode_FillMemInfo
 功能描述  : 向Qnode结点填写挂接的数据源, 并填写自定义参数
 输入参数  : usFileId       - 文件ID
             usLineId       - 行号
             ulPid          - PID
             pstQnode       - Qnode结点
             pstMemSrc      - 描述内存中挂接的用户数据源的结构
             ucParaOctetCnt - 用户自定义参数长度, 单位: 字节
             aucPara        - 用户自定义参数
 输出参数  : 无
 返 回 值  : 操作结果, 成功 - PS_SUCC, 否则 - PS_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
PS_RSLT_CODE_ENUM_UINT32 PS_Qnode_FillMemInfo(VOS_UINT16   usFileId,
                            VOS_UINT16                     usLineId,
                            VOS_UINT32                     ulPid,
                            PS_QNODE_STRU                 *pstQnode,
                            PS_QNODE_MEM_SOURCE_STRU      *pstMemSrc,
                            VOS_UINT8                      ucParaOctetCnt,
                            VOS_UINT8                      aucPara[])
{
    VOS_UINT32  ulTick;

    ulTick  =   VOS_GetTick();

    if (VOS_NULL_PTR == pstQnode)
    {
        PS_LOG4(ulPid, 0, PS_PRINT_WARNING, "fileid, line, pid, tick, pstQnode is NULL",
            usFileId, usLineId, (VOS_INT32)ulPid, (VOS_INT32)ulTick);
        return PS_FAIL;
    }

    if (VOS_NULL_PTR == pstMemSrc)
    {
        PS_LOG4(ulPid, 0, PS_PRINT_WARNING, "fileid, line, pid, tick, pstMemSrc is NULL",
            usFileId, usLineId, (VOS_INT32)ulPid, (VOS_INT32)ulTick);
        return PS_FAIL;
    }

    if (PS_QNODE_FREE_STATE == pstQnode->enState)
    {
        PS_LOG4(ulPid, 0, PS_PRINT_WARNING, "fileid, line, pid, tick, pstQnode state is free",
            usFileId, usLineId, (VOS_INT32)ulPid, (VOS_INT32)ulTick);
        return PS_FAIL;
    }

    if (ucParaOctetCnt > MAX_PARA_OCTET_NUM)
    {
        PS_LOG(ulPid, 0, PS_PRINT_WARNING, "more than allowed para octet num");
        return PS_FAIL;
    }

    PS_MEM_CPY(&(pstQnode->stMemSrc), pstMemSrc, sizeof(pstQnode->stMemSrc));
    if (0 != ucParaOctetCnt)
    {
        PS_MEM_CPY(&(pstQnode->aucPara[0]), aucPara, (ucParaOctetCnt * sizeof(VOS_UINT8)));
    }

    return PS_SUCC;
} /* PS_Qnode_FillMemInfo */

/*****************************************************************************
 函 数 名  : PS_Qnode_Trace
 功能描述  : 向Qnode结点填写调试信息(调试类API)
 输入参数  : usFileId   - 文件ID
             usLineId   - 行号
             ulPid      - PID
             pstQnode   - Qnode结点
 输出参数  : 无
 返 回 值  : 操作结果, 成功 - PS_SUCC, 否则 - PS_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
PS_RSLT_CODE_ENUM_UINT32 PS_Qnode_Trace(VOS_UINT16 usFileID, VOS_UINT16 usLineNum,
            VOS_UINT32 ulPid, PS_QNODE_STRU *pstQnode)
{
    VOS_UINT32                      ulTick;
    VOS_UINT32                      ulQnodeAddr;
    VOS_UINT32                      ulFirstQnodeAddr;

    ulTick  =   VOS_GetTick();

    if (VOS_NULL_PTR == pstQnode)
    {
        PS_LOG4(ulPid, 0, PS_PRINT_WARNING, "FileId, LineId, Pid, tick, pstQnode is NULL",
            usFileID, usLineNum, (VOS_INT32)ulPid, (VOS_INT32)ulTick);
        return PS_FAIL;
    }

    ulQnodeAddr         =   (VOS_UINT32)pstQnode;
    ulFirstQnodeAddr    =   (VOS_UINT32)(&g_astQnode[0]);

    if (0 != (ulQnodeAddr - ulFirstQnodeAddr) % sizeof(g_astQnode[0]))
    {
        PS_LOG4(ulPid, 0, PS_PRINT_WARNING, "FileId, LineId, Pid, tick, pstQnode is invalid",
            usFileID, usLineNum, (VOS_INT32)ulPid, (VOS_INT32)ulTick);
        return PS_FAIL;
    }

    pstQnode->stDbg.ulTraceTick     =   ulTick;
    pstQnode->stDbg.usTraceFileId   =   usFileID;
    pstQnode->stDbg.usTraceLineNum  =   usLineNum;

    return PS_SUCC;
} /* PS_Qnode_Trace */

/*****************************************************************************
 函 数 名  : PS_Qnode_ShowBinaryInfo
 功能描述  : 二进制打印函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年10月11日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID PS_Qnode_ShowBinaryInfo(VOS_UINT32 ulValue)
{
    VOS_UINT32              ulLoop;
    const VOS_UINT32        ul32Bit = 32;
    VOS_UINT8               ucChar;

    for (ulLoop = 0; ulLoop < ul32Bit; ulLoop++)
    {
        if (TTF_GET_A_BIT(ulValue, (ul32Bit - ulLoop - 1)))
        {
            ucChar = '1';
        }
        else
        {
            ucChar = '0';
        }
        vos_printf("%c", ucChar);
    }

    return;
} /* PS_Qnode_ShowBinaryInfo */

/*****************************************************************************
 函 数 名  : PS_Qnode_ShowUsedInfo
 功能描述  : 显示在用的Qnode结点(调试类API)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年10月11日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID PS_Qnode_ShowUsedInfo(VOS_VOID)
{
    VOS_UINT32              ulLoop;
    VOS_UINT32              ulCurr;
    const VOS_UINT32        ul32Bit = 32;

    if (PS_QNODE_TRACE_PRINT_ON == g_stMgmt.enIsTracePrintOn)
    {
        for (ulLoop = 0; ulLoop < MAX_PS_QNODE_4BYTE_ALIGN_NUM; ulLoop++)
        {
            ulCurr = g_stMgmt.aulQuickMap[ulLoop];
            vos_printf("Index from %d to %d: [0b", (ulLoop * ul32Bit),
                (((ulLoop + 1) * ul32Bit) - 1));
            PS_Qnode_ShowBinaryInfo(ulCurr);
            vos_printf("]\n");
        }
    }
    else
    {
        vos_printf("Enable trace print function by PS_Qnode_SetTracePrint(1).\n");
    }
} /* PS_Qnode_ShowUsedInfo */

/*****************************************************************************
 函 数 名  : PS_Qnode_ShowUsedInfo
 功能描述  : 显示在用的Qnode结点明细(调试类API)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年9月20日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID PS_Qnode_ShowDetailUsedInfo(VOS_VOID)
{
    VOS_UINT32               ulLoop;
    PS_QNODE_DEBUG_STRU     *pstDbg;
    VOS_UINT32               ulUsedCnt;
    const VOS_UINT32         ulConsectivePrintCnt  = 200; /* 一次性打印个数上限 */

    if (PS_QNODE_TRACE_PRINT_ON == g_stMgmt.enIsTracePrintOn)
    {
        PS_Qnode_ShowUsedInfo();

        ulUsedCnt   = 0;

        for (ulLoop = 0; ulLoop < MAX_PS_QNODE_NUM; ulLoop++)
        {
            if (PS_QNODE_USED_STATE == g_astQnode[ulLoop].enState)
            {
                pstDbg  =   &(g_astQnode[ulLoop].stDbg);
                vos_printf("index = %u, Alloc fileid=%u, line=%u, tick=0x%x, trace fileid=%u, line=%u, tick=0x%x\n",
                    g_astQnode[ulLoop].usIndex, pstDbg->usAllocFileId,
                    pstDbg->usAllocLineNum, pstDbg->ulAllocTick,
                    pstDbg->usTraceFileId, pstDbg->usTraceLineNum,
                    pstDbg->ulTraceTick);

                ulUsedCnt++;
                /* 为了防止持续打印将串口打死, 每次打印一定量后, 则延迟, 然后继续打印 */
                if (ulUsedCnt >= ulConsectivePrintCnt)
                {
                    VOS_TaskDelay(10);
                    ulUsedCnt   =   0;
                }
            }
        }
    }
    else
    {
        vos_printf("Enable trace print function by PS_Qnode_SetTracePrint(1).\n");
    }

    return;
} /* PS_Qnode_ShowDetailUsedInfo */

/*****************************************************************************
 函 数 名  : PS_Qnode_SetTracePrint
 功能描述  : 使能Qnode结点trace print功能(调试类API)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年9月20日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID PS_Qnode_SetTracePrint(PS_QNODE_TRACE_PRINT_SWITCH_ENUM_UINT8 enPrint)
{
    if (PS_QNODE_TRACE_PRINT_BUTT <= enPrint)
    {
        vos_printf("para error\n");
        return;
    }

    g_stMgmt.enIsTracePrintOn = enPrint;

    return;
} /* PS_Qnode_SetTracePrint */

/*****************************************************************************
 函 数 名  : PS_Qnode_Statistics
 功能描述  : 显示Qnode结点统计(调试类API)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年9月20日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID PS_Qnode_Statistics(VOS_VOID)
{
    vos_printf("total alloc %u times\n",          g_stMgmt.stStat.ulAllocTotalCnt);
    vos_printf("alloc fail %u times\n",           g_stMgmt.stStat.ulAllocFailCnt);
    vos_printf("total free %u times\n",           g_stMgmt.stStat.ulFreeTotalCnt);
    vos_printf("free null ptr %u times\n",        g_stMgmt.stStat.ulFreeNullPtrCnt);
    vos_printf("free unused ptr %u times\n",      g_stMgmt.stStat.ulFreePtrBeforeCnt);
    vos_printf("found tampered qnode %u times\n", g_stMgmt.stStat.ulMagicTamperedCnt);
    vos_printf("free invalid ptr %u times\n",     g_stMgmt.stStat.ulFreeInvalidPrtCnt);

    return;
} /* PS_Qnode_Statistics */

/*****************************************************************************
 函 数 名  : PS_Qnode_Help
 功能描述  : Qnode结点调试帮助命令(调试类API)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年9月20日
    作    者   : liukai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID PS_Qnode_Help(VOS_VOID)
{
    vos_printf("********************TTF_Qnode DEBUG INFO************************\n");
    vos_printf("PS_Qnode_ShowUsedInfo(VOS_VOID)             Show used PsQnode when trace print is enabled.\n");
    vos_printf("PS_Qnode_ShowDetailUsedInfo(VOS_VOID)       Show detail of used PsQnode when trace print is enabled.\n");
    vos_printf("PS_Qnode_SetTracePrint(enPrint)             Enable/Disable trace print. Setting enPrint = 0, it will be disabled.\n");
    vos_printf("PS_Qnode_Statistics(VOS_VOID)               Show Qnode statistics.\n");

    return;
} /* PS_Qnode_Help */


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif


