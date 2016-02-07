/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_buf_ctrl.c
  Description     :
  History         :
     1.w00182550       2012-11-20   Draft Enact

******************************************************************************/
/*lint --e{717}*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
/*lint -save -e537*/
#include  "diag_buf_ctrl.h"
#include  "diag_common.h"
#include  "msp_errno.h"
#include  "diag_debug.h"
#include  "SCMProc.h"
#include  "diag_mem.h"
#include  "SOCPInterface.h"
#include  <diag_debug.h>
/*lint -restore*/

/*lint -save -e767 原因:Log打印*/
#define    THIS_FILE_ID        MSP_FILE_ID_DIAG_BUF_CTRL_C
/*lint -restore +e767*/
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
DIAG_BUF_CTRL_STRU                        g_stDiagBufCtrl[DIAG_CODER_SRC_TYPE_BUTT] = {{0}};
VOS_SEM g_DiagLogSem =(VOS_SEM)0;

DIAG_TO_HSO_FAIL_STRU g_stDiagToHsoErrRecord = {0};
VOS_UINT32 g_LockBufTimes = 0;
VOS_UINT32 g_UnLockBufTimes = 0;
VOS_INT32 g_UnNeedMaxBuf[2] = {0};
VOS_UINT8* g_DiagMemVirt =0;
#ifndef CHIP_BB_HI6210
VOS_UINT g_DiagMemPhy =0;
VOS_UINT32 g_DiagMemSize = 0;
#endif
VOS_VOID diag_InitSocpChan(VOS_VOID);

/*****************************************************************************
  3 Function
*****************************************************************************/
/*****************************************************************************
 Function Name   : diag_BufCtrlGlobalInit
 Description     : DIAG BUF控制全局变量初始化
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-23  Draft Enact

*****************************************************************************/
VOS_VOID diag_GetBufLen(VOS_UINT32 ulLen[DIAG_CODER_SRC_TYPE_BUTT]);
VOS_VOID diag_GetBufLen(VOS_UINT32 ulLen[DIAG_CODER_SRC_TYPE_BUTT])
{
    /*lint -save -e40*/
	ulLen[DIAG_CODER_SRC_TYPE_CNF] = DIAG_CODER_SRC_CNF_BUFFER_SIZE;
	ulLen[DIAG_CODER_SRC_TYPE_IND] = DIAG_CODER_SRC_IND_BUFFER_SIZE;

#if (VOS_LINUX== VOS_OS_VER)
	g_stDiagBufCtrl[DIAG_CODER_SRC_TYPE_CNF].pucRealBuf = (VOS_UINT8*)DIAG_CODER_SRC_CNF_BUF_A_ADDR;
	g_stDiagBufCtrl[DIAG_CODER_SRC_TYPE_IND].pucRealBuf = (VOS_UINT8*)DIAG_CODER_SRC_IND_BUF_A_ADDR;
#else
	g_stDiagBufCtrl[DIAG_CODER_SRC_TYPE_CNF].pucRealBuf = (VOS_UINT8*)DIAG_CODER_SRC_CNF_BUF_C_ADDR;
	g_stDiagBufCtrl[DIAG_CODER_SRC_TYPE_IND].pucRealBuf = (VOS_UINT8*)DIAG_CODER_SRC_IND_BUF_C_ADDR;
#endif
    /*lint -restore*/
}

VOS_UINT32 diag_BufCtrlGlobalInit(VOS_VOID)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT32 i = 0;
	VOS_INT32 aulLen[DIAG_CODER_SRC_TYPE_BUTT] = {0,};
#ifndef CHIP_BB_HI6210
    BSP_DDR_SECT_QUERY_S stSectQuery = {};
    BSP_DDR_SECT_INFO_S stSectInfo = {};
#endif
    /* 创建 控制LOG上报的信号量,第二个参数1表示一开始能take到信号量，不需等待，0表示一开始take不到*/
    ret = VOS_SmBCreate(NULL, 1, VOS_SEMA4_FIFO,&g_DiagLogSem);
    if (ERR_MSP_SUCCESS != ret)
    {
    	diag_printf("[%s]:VOS_SmBCreate fail!\n",__FUNCTION__);
        return ret;
    }
#ifndef CHIP_BB_HI6210
    stSectQuery.enSectType = BSP_DDR_SECT_TYPE_SOCP;

    mdrv_get_fix_ddr_addr(&stSectQuery, &stSectInfo);

    g_DiagMemVirt = stSectInfo.pSectVirtAddr;
    g_DiagMemPhy = stSectInfo.pSectPhysAddr;
    g_DiagMemSize = stSectInfo.ulSectSize;
#else
    /*lint -save -e40 -e539*/
#ifdef DIAG_MEMMAP
	g_DiagMemVirt = (VOS_UINT8*)MSP_NOCACHEMAP(DIAG_MEM_ADDR_BASE,DIAG_MEM_TOTAL_SIZE);
#else
	g_DiagMemVirt = (VOS_UINT8*)SOCP_GLOBAL_MEM_ADDR_VIRT;
#endif
    /*lint -restore*/
#endif
    diag_GetBufLen((VOS_UINT32 *)aulLen);

    /* 创建log发送缓冲区 */
    for(i = 0 ; i< DIAG_CODER_SRC_TYPE_BUTT; i++)
    {
        if (VOS_OK != diag_CreateLogBuf(&g_stDiagBufCtrl[i], aulLen[i]))
        {
            diag_printf("diag_BufCtrlGlobalInit:diag_CreateLogBuffer Failed!\n");
            return ERR_MSP_FAILURE;
        }
    }

#if ((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
    /*lint -save -e539*/
	diag_InitSocpChan();
    /*lint -restore */
#endif
    return ret;
}

/*****************************************************************************
 Function Name   : diag_LockCoderSrcBuf
 Description     : 锁编码源buf 资源
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-23  Draft Enact

*****************************************************************************/
VOS_VOID diag_LockCoderSrcBuf(VOS_VOID)
{
    (VOS_VOID)VOS_SmP(g_DiagLogSem,0);

    g_LockBufTimes++;
}

/*****************************************************************************
 Function Name   : diag_UnLockCoderSrcBuf
 Description     : 解锁编码源buf 资源
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-23  Draft Enact

*****************************************************************************/
VOS_VOID diag_UnLockCoderSrcBuf(VOS_VOID)
{

    (VOS_VOID)VOS_SmV(g_DiagLogSem);
    g_UnLockBufTimes++;
}

/*****************************************************************************
 Function Name   : diag_BuffPhyToVirt
 Description     : 根据输入实地址转成虚拟地址
 Input           :VOS_UINT8 *pucCurPhyAddr
                VOS_UINT8 *pucPhyStart
                VOS_UINT8 *pucVirtStart
                VOS_UINT32 ulBufLen
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-2-16  Draft Enact

*****************************************************************************/
VOS_UINT8* diag_BuffPhyToVirt(VOS_UINT8 *pucCurPhyAddr, VOS_UINT8 *pucPhyStart, VOS_UINT8 *pucVirtStart, VOS_UINT32 ulBufLen)
{
#if (VOS_LINUX == VOS_OS_VER)
	if((pucCurPhyAddr < pucPhyStart) /*|| (pucCurPhyAddr >= (pucPhyStart+ulBufLen))*/)
	{
		vos_printf("\r\n diag_BuffPhyToVirt: The PHY Addr 0x%x, pucPhyStart: 0x%x, pucVirtStart: 0x%x, ulBufLen: 0x%x.\r\n",
					pucCurPhyAddr,
					pucPhyStart,
					pucVirtStart,
					ulBufLen);

		return VOS_NULL;
	}

	return ((pucCurPhyAddr - pucPhyStart) + pucVirtStart);
#else
    return (pucCurPhyAddr);
#endif
}

/*****************************************************************************
 函 数 名  : diag_BuffVirtToPhy
 功能描述  : 根据输入的虚地址，计算对应的实地址
 输入参数  : pucCurVirtAddr:  当前虚地址
             pucPhyStart: 通道配置内存起始的实地址
             pucVirtStart:通道配置内存起始的虚地址
             ulBufLen:    通道内存空间大小
 输出参数  : 无
 返 回 值  : VOS_NULL: 转换失败/other: 虚地址的值
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT8* diag_BuffVirtToPhy(VOS_UINT8 *pucCurVirtAddr, VOS_UINT8 *pucVirtStart, VOS_UINT8 *pucPhyStart, VOS_UINT32 ulBufLen)
{
#if (VOS_LINUX == VOS_OS_VER)
	if((pucCurVirtAddr < pucVirtStart) /*|| (pucCurVirtAddr >= (pucVirtStart+ulBufLen))*/)
	{
		vos_printf("\r\n diag_BuffVirtToPhy: The VIRT Addr 0x%x, pucPhyStart: 0x%x, pucVirtStart: 0x%x, ulBufLen: 0x%x.\r\n",
					pucCurVirtAddr,
					pucPhyStart,
					pucVirtStart,
					ulBufLen);

		return VOS_NULL;
	}

	return ((pucCurVirtAddr - pucVirtStart) + pucPhyStart);
#else
    return (pucCurVirtAddr);
#endif
}

/*****************************************************************************
 Function Name   : diag_CreateLogBuf
 Description     : 创建log缓冲区
 Input           :DIAG_BUF_CTRL_STRU *pstBufCtrl
                VOS_INT32 lBufSize
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-12-18  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_CreateLogBuf(DIAG_BUF_CTRL_STRU *pstBufCtrl, VOS_INT32 lBufSize)
{
    /* 参数检测 外部保证了不为空*/

    pstBufCtrl->lAlloc   = 0;
    pstBufCtrl->lRelease = 0;
    pstBufCtrl->lPadding = 0;
    pstBufCtrl->lBufSize = lBufSize;

    /*申请uncache的动态内存区*/
    /*lint -save -e40*/
    pstBufCtrl->pucBuf = diag_BuffPhyToVirt(pstBufCtrl->pucRealBuf,(VOS_UINT8*)DIAG_MEM_ADDR_BASE,(VOS_UINT8*)g_DiagMemVirt,(VOS_UINT32)(pstBufCtrl->lBufSize));
    /*lint -restore*/
    /* 分配内存失败 */
    if (VOS_NULL_PTR == pstBufCtrl->pucBuf)
    {
        g_stDiagToHsoErrRecord.usNoMemErr++;
        return ERR_MSP_MALLOC_FAILUE;
    }

    diag_printf("diag_CreateLogBuf: real addr=0x%p, virt addr==0x%p\n", pstBufCtrl->pucRealBuf, pstBufCtrl->pucBuf);

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 Function Name   : diag_AllocLogMem
 Description     : 从缓冲区中分配连续的物理内存.
 Input           :DIAG_BUF_CTRL_STRU *pstBufCtrl
                VOS_INT32 lLen
 Output          : VOS_UINT8*
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-12-18  Draft Enact

*****************************************************************************/
VOS_UINT8* diag_AllocLogMem(DIAG_BUF_CTRL_STRU *pstBufCtrl, VOS_INT32 lLen)
{
    VOS_INT32                           lFreeSize;
    VOS_UINT8                          *pucAddr;
    VOS_INT32                           lTmpAlloc;
    VOS_INT32                           lTmpPadding;

    /* 输入参数检测 */
    if ((lLen >= pstBufCtrl->lBufSize)|| (0 == lLen))
    {
        diag_printf("diag_AllocLogMem:入参不对!,lLen=%d\n",lLen);
        return VOS_NULL_PTR;
    }

    lTmpAlloc       = pstBufCtrl->lAlloc;
    lTmpPadding     = pstBufCtrl->lPadding;

    /* 没有回卷 */
    if (pstBufCtrl->lAlloc >= pstBufCtrl->lRelease)
    {
        /* 获取剩余内存大小 ，用于区分buffer空和满，需要-1*/
        if (0 == pstBufCtrl->lRelease)
        {
            lFreeSize = (pstBufCtrl->lBufSize - pstBufCtrl->lAlloc) - 1;
        }
		 /* 获取剩余内存大小 ，用于区分buffer空和满，由于release前面有空间，读写指针不会相等，不需要-1*/
        else
        {
            lFreeSize = pstBufCtrl->lBufSize - pstBufCtrl->lAlloc;
        }

        /* 满足用户申请内存大小则返回 */
        if (lFreeSize >= lLen)
        {
            pucAddr = pstBufCtrl->pucBuf + pstBufCtrl->lAlloc;
            pstBufCtrl->lAlloc += lLen;

            return pucAddr;
        }

        lTmpPadding  = lFreeSize;
        lTmpAlloc    = 0;
    }

    /* 回卷，获取剩余内存大小 ,用于区分buffer空和满*/
    lFreeSize = (pstBufCtrl->lRelease - lTmpAlloc)-1;

    if (lFreeSize >= lLen)
    {
        pucAddr = pstBufCtrl->pucBuf + lTmpAlloc;

        pstBufCtrl->lAlloc    = lTmpAlloc + lLen;
        pstBufCtrl->lPadding  = lTmpPadding;

        return pucAddr;
    }

    /* 否则分配失败，返回空指针 */
    //diag_printf("diag_AllocLogMem:可用大小不够!\n");
    return VOS_NULL_PTR;
}

/*****************************************************************************
 Function Name   : diag_ReleaseLogMem
 Description     : 释放从缓冲区中分配连续的物理内存
 Input           :DIAG_BUF_CTRL_STRU *pstBufCtrl
                VOS_UINT8 *pAddr
                VOS_INT32 lLen
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-12-18  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_ReleaseLogMem(DIAG_BUF_CTRL_STRU *pstBufCtrl,VOS_UINT8 *pAddr, VOS_INT32 lLen)
{
    VOS_INT32                           lUsedSize;
    VOS_INT32                           lTmpRls;

    /* 如果当前处于中断上下文,则复位系统,诊断接口不允许在中断中使用*/
    if(VOS_FALSE != VOS_CheckInterrupt())
    {
        mdrv_om_system_error(DIAG_CALLED_IN_IRQ, MSP_FILE_ID_DIAG_BUF_CTRL_C, __LINE__, VOS_NULL, 0);/*lint !e569*/
    }
    /* 输入参数检测 */
    if ((VOS_NULL_PTR == pstBufCtrl) || (lLen >= pstBufCtrl->lBufSize))
    {
        diag_printf("diag_ReleaseLogMem:入参不对,lLen=%d!\n",lLen);
        return ERR_MSP_FAILURE;
    }

    /* 释放内存地址进行检测 */
    if ((pAddr != (pstBufCtrl->pucBuf + pstBufCtrl->lRelease))
        && (pAddr != pstBufCtrl->pucBuf))
    {
        diag_printf("diag_ReleaseLogMem:待释放地址不对!pAddr=%p,pstBufCtrl->pucBuf=%p,pstBufCtrl->lRelease=0x%x\n",
                    pAddr,pstBufCtrl->pucBuf,pstBufCtrl->lRelease);
        /*DRV_SYSTEM_ERROR(DIAG_REALSE_POINTER_ERROR, MSP_FILE_ID_DIAG_BUF_CTRL_C, __LINE__, VOS_NULL, 0);*//*lint !e569*/
    }

    /* 未回卷 */
    if (pstBufCtrl->lAlloc >= pstBufCtrl->lRelease)
    {
        lUsedSize = pstBufCtrl->lAlloc - pstBufCtrl->lRelease;

        /* 长度错误 */
        if (lUsedSize < lLen)
        {
            diag_printf("diag_ReleaseLogMem:使用长度小于待释放长度!lAlloc=0x%x,lRelease=0x%x,lLen=%d!\n",
                        pstBufCtrl->lAlloc,pstBufCtrl->lRelease,lLen);
            return ERR_MSP_FAILURE;
        }

        pstBufCtrl->lRelease = pAddr + lLen - pstBufCtrl->pucBuf;

        return ERR_MSP_SUCCESS;
    }

    if ((pAddr - pstBufCtrl->pucBuf + lLen) > pstBufCtrl->lBufSize)
    {
        lTmpRls = ((pAddr - pstBufCtrl->pucBuf + lLen) + pstBufCtrl->lPadding) % pstBufCtrl->lBufSize;
    }
    else
    {
        lTmpRls = (pAddr - pstBufCtrl->pucBuf + lLen) % pstBufCtrl->lBufSize;
    }

    /* 输入ulLen不正确 */
    if ((lTmpRls > pstBufCtrl->lAlloc) && (lTmpRls < pstBufCtrl->lRelease))
    {
        diag_printf("diag_ReleaseLogMem:有回卷情况，待释放地址不对!\n");
        return ERR_MSP_FAILURE;
    }

    /* 如果发生回卷则将Padding值归0 */
    if (lTmpRls <= pstBufCtrl->lAlloc)
    {
        pstBufCtrl->lPadding = 0;
    }

    pstBufCtrl->lRelease = lTmpRls;

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 Function Name   : diag_GetBufIndex
 Description     : 根据编码源通道号获取BUF数组下标
 Input           :SOCP_CODER_SRC_ENUM_U32 ulChanID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2013-1-18  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_GetBufIndex(SOCP_CODER_SRC_ENUM_U32 ulChanID)
{
    VOS_UINT32 ulIndex = 0;
    if( SOCP_CODER_SRC_LOM_CNF1 ==  ulChanID)
    {
        ulIndex = DIAG_CODER_SRC_TYPE_CNF;
    }
    else if( SOCP_CODER_SRC_LOM_IND1 ==  ulChanID)
    {
        ulIndex = DIAG_CODER_SRC_TYPE_IND;
    }
    else if( SOCP_CODER_SRC_LOM_IND2 ==  ulChanID)
    {
        ulIndex = DIAG_CODER_SRC_TYPE_CNF;
    }
    else if( SOCP_CODER_SRC_LOM_IND3 ==  ulChanID)
    {
        ulIndex = DIAG_CODER_SRC_TYPE_IND;
    }
    else
    {
        ulIndex = DIAG_CODER_SRC_TYPE_CNF;
    }
    return ulIndex;
}

/*****************************************************************************
 Function Name   : diag_ReleaseLogBuf
 Description     : 释放RD缓冲区中数据和使用内存
 Input           :SOCP_CODER_SRC_ENUM_U32 ulSrcChanID
 Output          : VOS_UINT32
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_ReleaseLogBuf(SOCP_CODER_SRC_ENUM_U32 ulSrcChanID)
{
    VOS_UINT32 ulDataLen = 0;
    VOS_UINT_PTR ulDataAddr = 0;
    VOS_UINT32 ulIndex = 0;
    VOS_UINT8  *pucVirtAddr =NULL;

    /* 清空所有已发送RD */
    if (VOS_OK != SCM_RlsSrcRDAll(ulSrcChanID, &ulDataAddr, &ulDataLen))
    {
        g_stDiagToHsoErrRecord.usRlsRdErr++;
        diag_printf("Function:%s error, ulSrcChanID = %d  \n", __FUNCTION__, ulSrcChanID);
        return ERR_MSP_FAILURE;
    }

    /* 已经没有RD */
    if ((VOS_NULL == ulDataAddr) || (0 == ulDataLen))
    {
        //diag_printf("diag_ReleaseLogBuf:ulDataAddr=0x%x,ulDataLen=%d\n",ulDataAddr,ulDataLen);
        g_stDiagToHsoErrRecord.usNoRdNum++;
        return ERR_MSP_SUCCESS;
    }

    /*根据通道类型获取要释放哪个BUF数据*/
    ulIndex = diag_GetBufIndex(ulSrcChanID);
    /*从SOCP出来的地址为实地址，需要转成虚地址操作*/
    /*lint -save -e40*/
    pucVirtAddr = diag_BuffPhyToVirt((VOS_UINT8*)ulDataAddr,(VOS_UINT8*)DIAG_MEM_ADDR_BASE,(VOS_UINT8*)g_DiagMemVirt,(VOS_UINT32)(g_stDiagBufCtrl[ulIndex].lBufSize));
    /*lint -restore*/
    if (VOS_NULL_PTR == pucVirtAddr)
    {
        g_stDiagToHsoErrRecord.usRlsAddrErr++;
        diag_printf("Function:%s error ulDataAddr = %d \n", __FUNCTION__, ulDataAddr);
        return ERR_MSP_INVALID_OP;
    }

    /* 根据已发送RD总长度释放内存 */
    if (VOS_OK != diag_ReleaseLogMem(&g_stDiagBufCtrl[ulIndex], pucVirtAddr, (VOS_INT32)ulDataLen))
    {
        g_stDiagToHsoErrRecord.usRlsMemErr++;
        diag_printf("Function:%s ulIndex = %d Error\n", __FUNCTION__, ulIndex);
        return ERR_MSP_FAILURE;
    }

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 Function Name   : diag_GetCoderSrcBuf
 Description     : 申请编码源通道Buf的接口
 Input           :VOS_UINT32 ulLen
                SOCP_CODER_SRC_ENUM_U32 ulChanID
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_UINT8* diag_GetCoderSrcBuf(VOS_UINT32 ulLen,SOCP_CODER_SRC_ENUM_U32 ulChanID)
{
    VOS_UINT8* aucBuf = NULL;
    VOS_UINT32 ulIndex = 0;
    VOS_UINT32 ulRet = 0;

    DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_GET_SRC_BUF_START, ulChanID, (VOS_UINT32)ulLen, 0);
    ulLen = ALIGN_DDR_WITH_4BYTE(ulLen);
#if (VOS_LINUX== VOS_OS_VER)
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    if(g_diagSocpIsEnable == FALSE)
    {
        vos_printf("%s socp is disable\n", __FUNCTION__);
        return NULL;
    }
#endif
#endif

    /*clean 使用过的RD和MEM buffer*/
    ulRet = diag_ReleaseLogBuf(ulChanID);
    if(ERR_MSP_SUCCESS != ulRet)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_GET_SRC_BUF_START_ERROR, ulChanID, (VOS_UINT32)ulRet, 0);
        diag_printf("Function:%s ulRet = %d Error\n", __FUNCTION__, ulRet);
        return NULL;
    }

    if( SCM_CODER_SRC_MAX_LEN  <= ulLen)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_GET_SRC_BUF_START_ERROR, ulChanID, (VOS_UINT32)ulLen, 2);
        return NULL;
    }

    /*根据通道类型获取要申请哪个BUF数据*/
    ulIndex = diag_GetBufIndex(ulChanID);

    /*获取编码源通道Buf*/
    aucBuf = diag_AllocLogMem(&g_stDiagBufCtrl[ulIndex], (VOS_INT32)ulLen);

    if(aucBuf == NULL)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_GET_SRC_BUF_START_ERROR, ulChanID, (VOS_UINT32)ulLen, 1);
        g_stDiagToHsoErrRecord.usAllocMemErr++;
        return NULL;
    }
    else
    {
        return aucBuf;
    }

}

/*****************************************************************************
 Function Name   : diag_SendCoderSrcToSocp
 Description     : 写入编码源数据到SOCP
 Input           :VOS_UINT8 *pBuf
                VOS_UINT32 ulLen
                SOCP_CODER_SRC_ENUM_U32 ulCodeSrcId
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.w00182550      2012-11-19  Draft Enact

*****************************************************************************/
VOS_UINT32 diag_SendCoderSrcToSocp(VOS_UINT8 *pBuf,VOS_UINT32 ulLen, SOCP_CODER_SRC_ENUM_U32 ulCodeSrcId)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT8  *pucRealMemAddr;
    VOS_UINT32 ulIndex = 0;

    DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_CODE_PACKET_START, ulCodeSrcId, (VOS_UINT32)ulLen, 0);

    /*根据通道类型获取要申请哪个BUF数据*/
    ulIndex = diag_GetBufIndex(ulCodeSrcId);

    /*配给SOCP必须为实地址*/
    /*lint -save -e40*/
    pucRealMemAddr = diag_BuffVirtToPhy(pBuf,(VOS_UINT8*)g_DiagMemVirt,(VOS_UINT8*)DIAG_MEM_ADDR_BASE,(VOS_UINT32)(g_stDiagBufCtrl[ulIndex].lBufSize));
    /*lint -restore*/

    if (VOS_NULL_PTR == pucRealMemAddr)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_CODE_PACKET_START_ERROR, ulCodeSrcId, (VOS_UINT32)ulLen, 0);
        /*如果发送失败,将申请的buf归还*/
        g_stDiagBufCtrl[ulIndex].lAlloc -= (VOS_INT32)ulLen;

        g_stDiagToHsoErrRecord.usSendAddrErr++;

        return ERR_MSP_INVALID_OP;
    }

#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
    /*数据发送前，刷cache*/
    DIAG_FLUSH_CACHE(pBuf, ulLen);
#endif

    /*将数据发给SOCP*/
    ret = SCM_SendCoderSrc(ulCodeSrcId,pucRealMemAddr,ulLen);
    if(ret != ERR_MSP_SUCCESS)
    {
        DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_CODE_PACKET_START_ERROR, ulCodeSrcId, (VOS_UINT32)ulLen, 1);
        /*如果发送失败,将申请的buf归还*/
		g_stDiagBufCtrl[ulIndex].lAlloc -= (VOS_INT32)ulLen;

        g_stDiagToHsoErrRecord.usSendSocpDataErr++;
    }

    return ret;

}

VOS_VOID diag_BufHelp(VOS_VOID)
{
    VOS_UINT32 i =0;

    vos_printf("信号量锁次数=%x:\n",                            g_LockBufTimes);
    vos_printf("信号量解锁次数=%x:\n",                          g_UnLockBufTimes);

    vos_printf("\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");

    for(i = 0 ;i< 2 ;i++)
    {
        vos_printf("编码源BUF起始虚拟地址=%p:\n",                    g_stDiagBufCtrl[i].pucBuf);
        vos_printf("编码源BUF起始物理地址=%p:\n",                    g_stDiagBufCtrl[i].pucRealBuf);
        vos_printf("编码源BUF分配地址相对于起始虚拟地址偏移=0x%x:\n",  g_stDiagBufCtrl[i].lAlloc);
        vos_printf("编码源BUF释放地址相对于起始虚拟地址偏移=0x%x:\n",  g_stDiagBufCtrl[i].lRelease);
        vos_printf("编码源BUF填充地址相对于起始虚拟地址偏移=0x%x:\n",  g_stDiagBufCtrl[i].lPadding);
        vos_printf("编码源BUF大小=%d(K):\n",                           (g_stDiagBufCtrl[i].lBufSize)/1024);

        vos_printf("\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    }

    vos_printf("分配 uncache mem错误次数=%d:\n",                 g_stDiagToHsoErrRecord.usNoMemErr);
    vos_printf("分配 BUF错误次数=%d:\n",                         g_stDiagToHsoErrRecord.usAllocMemErr);
    vos_printf("释放 BUF错误次数=%d:\n",                         g_stDiagToHsoErrRecord.usRlsMemErr);
    vos_printf("释放 RD错误次数=%d:\n",                          g_stDiagToHsoErrRecord.usRlsRdErr);
    vos_printf("实地址转换虚地址错误次数=%d:\n",                 g_stDiagToHsoErrRecord.usRlsAddrErr);
    vos_printf("需地址转换实地址错误次数=%d:\n",                 g_stDiagToHsoErrRecord.usSendAddrErr);
    vos_printf("写入SOCP错误次数=%d:\n",                         g_stDiagToHsoErrRecord.usSendSocpDataErr);
	vos_printf("无RD可清次数=%d:\n",                             g_stDiagToHsoErrRecord.usNoRdNum);
}

VOS_VOID diag_BufDebug(VOS_VOID)
{
    VOS_UINT32 i =0;

    for(i = 0 ;i< 2 ;i++)
    {
		if(g_stDiagBufCtrl[i].lAlloc >= g_stDiagBufCtrl[i].lRelease)
		{
			g_UnNeedMaxBuf[i] = g_stDiagBufCtrl[i].lAlloc - g_stDiagBufCtrl[i].lRelease;
		}
		else
		{
			g_UnNeedMaxBuf[i] = g_stDiagBufCtrl[i].lBufSize - g_stDiagBufCtrl[i].lRelease + g_stDiagBufCtrl[i].lAlloc;
		}
		vos_printf("buf[%d]需要的最大长度 = %d(k);\n",		i,	g_UnNeedMaxBuf[i]/1024);
    }
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


