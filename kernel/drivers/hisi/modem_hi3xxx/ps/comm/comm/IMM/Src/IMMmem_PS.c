

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "IMMmem_Intra.h"
#include "mdrv.h"

#if (VOS_WIN32 != VOS_OS_VER)
#include <linux/module.h>
#include <linux/interrupt.h>
#endif



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if 0
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 存放C核查询A核内存的预留数目的地址 */
#define TTF_MEM_ACPU_FREE_MEM_CNT_ADDR  (ECS_TTF_ACPU_FREE_MEM_CNT_ADDR)

/* A 核向C核上报内存空闲块数的地址 */
unsigned int *                         g_pACpuFreeMemBlkCnt;

/* IMM MEM 数据内存池档位控制结构 */
const IMM_MEM_CLUSTER_CFG_INFO_STRU     g_astImmMemSharePoolClusterTableInfo[] =
{
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_0},             /* 内存块长度必须32字节对齐 */
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_1}
};

const IMM_MEM_CLUSTER_CFG_INFO_STRU     g_astImmMemCtrlPoolClusterTableInfo[] =
{
    {0,                 IMM_MEM_CTRL_MEM_CNT}
};

#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
/* IMM MEM 控制头内存池使用的内存长度 Byte(每块36B) */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE     (IMM_MEM_CTRL_MEM_TOTAL_CNT*36)
#else
/* IMM MEM 控制头内存池使用的内存长度 Byte(每块16B) */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE     (IMM_MEM_CTRL_MEM_TOTAL_CNT*16)
#endif

/* IMM MEM 控制头内存池内存块已使用的长度 */
unsigned int                           g_ulImmMemCtrlMemSuffix = 0;
const  unsigned int * const            g_pulImmMemCtrlMemSuffix = &g_ulImmMemCtrlMemSuffix;

/* IMM MEM 控制头内存池内存 */
unsigned int                           g_aulImmMemCtrlMemSpace[IMM_MEM_CTRL_MEM_TOTAL_SIZE/4];
const unsigned int * const             g_paulImmMemCtrlMemSpace = &g_aulImmMemCtrlMemSpace[0];


/* IMM MEM 数据内存池档位个数 */
const unsigned char IMM_MEM_SHARE_POOL_CLUSTER_CNT = (sizeof(g_astImmMemSharePoolClusterTableInfo)/sizeof(IMM_MEM_CLUSTER_CFG_INFO_STRU));

/* IMM MEM 控制头内存池档位个数 */
const unsigned char IMM_MEM_CTRL_POOL_CLUSTER_CNT  = (sizeof(g_astImmMemCtrlPoolClusterTableInfo)/sizeof(IMM_MEM_CLUSTER_CFG_INFO_STRU));

/* IMM MEM 控制块魔术字，检查该区域是否被踩 */
const unsigned short IMM_MAGIC_NUM = 0x4E8F;

/* 零拷贝内存池初始化标志，防止重复调用初始化函数 */
unsigned int                           g_ulImmMemInitFlag  = IMM_FALSE;

/* IMM MEM 数据内存池总控制结构 */
IMM_MEM_POOL_STRU                       g_astImmMemPool[IMM_MEM_POOL_ID_BUTT];

/* IMM MEM 外部内存池总控制结构指针 */
IMM_MEM_POOL_STRU *const                g_pstImmExtMemPool = &g_astImmMemPool[IMM_MEM_POOL_ID_EXT];


/* 内存申请失败指针数据 */
unsigned short *const                   g_apusImmAllocFailCnt[IMM_MEM_POOL_ID_BUTT] =
                                        {
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_SHARE].usImmAllocFailCnt,
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_EXT].usImmAllocFailCnt
                                        };


/* 内存申请失败超过门限次数 */
unsigned short *const                   g_apusImmExcThresholdCnt[IMM_MEM_POOL_ID_BUTT] =
                                        {
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_SHARE].usImmExcThresholdCnt,
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_EXT].usImmExcThresholdCnt
                                        };


/* IMM MEM MNTN管理控制全局变量 */
IMM_MEM_REG_MNTN_FUNC_STRU              g_stImmRegMntnFuc;
IMM_MEM_REG_MNTN_FUNC_STRU * const      g_pstImmRegMntnFuc = &g_stImmRegMntnFuc;

/* IMM MEM 获取该长度所在的档位参数 */
#define IMM_GET_LEN_INDEX(pstImmMemPoolInfo, usLen) \
            ((pstImmMemPoolInfo)->aucLenIndexTable[(usLen)])

#if(IMM_DEBUG_TRACE_FUNC == FEATURE_ON)
/* IMM 内存块调试打印开关 */
unsigned char                           g_ucImmMemDebugPrint = IMM_FALSE;
#endif

/* 每次发生内存上报事件时，每次所能内存事件次数门限 */
unsigned short                           g_usImmAllocFailCntThreshold = 1000;

/* IMM MEM FREE 可维可测实体 */
IMM_MEM_FREE_MNTN_INFO_STRU             g_stImmMemFreeMntnEntity;

VOS_SPINLOCK                      g_astImmSpinLock[IMM_MEM_POOL_ID_BUTT];

VOS_SPINLOCK                      g_stImmFreeMntnSpinLock;


/* 保存内存释放空指针信息 */
#define IMM_MEM_FREE_SAVE_NULL_PTR_INFO(stNullPtrInfo,usFileId,usLineNum,pstSpinLock,ulFlags) \
            do{\
                VOS_SpinLockIntLock(pstSpinLock, ulFlags);\
                (stNullPtrInfo).ulImmMemFreeNullPtrCnt++;\
                (stNullPtrInfo).usImmMemFreeFileId      = (usFileId);\
                (stNullPtrInfo).usImmMemFreeLineNum     = (usLineNum);\
                VOS_SpinUnlockIntUnlock(pstSpinLock , ulFlags);\
            }while(0)

/* 保存内存释放错误内存信息 */
#define IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(astImmMemFreeInvalidMemInfo,pstTtfMem,ucType,usInvalidvalue,usFileId,usLineNum,pstSpinLock,ulFlags) \
            do{\
                VOS_SpinLockIntLock(pstSpinLock , ulFlags);\
                (astImmMemFreeInvalidMemInfo).ulInvalidImmMemCnt++;\
                (astImmMemFreeInvalidMemInfo).ulInvalidImmMemAddr   = (unsigned int)pstTtfMem;\
                (astImmMemFreeInvalidMemInfo).usImmMemFreeFileId    = (usFileId);\
                (astImmMemFreeInvalidMemInfo).usImmMemFreeLineNum   = (usLineNum);\
                (astImmMemFreeInvalidMemInfo).ucImmMemInvalidType   = (ucType);\
                (astImmMemFreeInvalidMemInfo).usImmMemInvalidValue  = (usInvalidvalue);\
                VOS_SpinUnlockIntUnlock(pstSpinLock , ulFlags);\
            }while(0)
void *IMM_MemCtrlMemAlloc(unsigned int ulSize)
{
    unsigned char                       *pucSpace;
    void                                *pAlloc;


    /* 申请长度范围检查 */
    if ( IMM_MEM_CTRL_MEM_TOTAL_SIZE < (g_ulImmMemCtrlMemSuffix + ulSize) )
    {
        /*lint -e718 -e746*/
        panic(" IMM_MemCtrlMemAlloc Fail, Ctrl MEM Over run! \n");
        /*lint +e718 +e746*/
        return VOS_NULL_PTR;
    }

    pucSpace = (unsigned char *)g_aulImmMemCtrlMemSpace;

    pAlloc   = pucSpace + g_ulImmMemCtrlMemSuffix;

    /* 四字节对齐，控制块使用长度偏移 */
    g_ulImmMemCtrlMemSuffix += IMM_GET_4BYTE_ALIGN_VALUE(ulSize);

    return pAlloc;

} /* IMM_MemCtrlMemAlloc */

unsigned int IMM_MemPoolInit
(
    unsigned char                            ucPoolId,
    unsigned char                          **ppucDataMemAddr
)
{
    IMM_MEM_POOL_STRU                  *pstImmMemPool;
    IMM_MEM_STRU                      **ppst1stImmMem;
    IMM_MEM_STRU                       *pstImmMem;
    IMM_MEM_CLUSTER_STRU               *pstImmMemCluster;
    unsigned int                       ulImmMemCnt;
    unsigned char                       ucClusterId;
    IMM_MEM_STRU                      **ppTempMem;

    /*lint -e778*/
    g_pACpuFreeMemBlkCnt = (unsigned int *)DRV_AXI_PHY_TO_VIRT(TTF_MEM_ACPU_FREE_MEM_CNT_ADDR);
    /*lint +e778*/

    pstImmMemPool = IMM_MEM_GET_POOL(ucPoolId);

    /* 申请对应内存池的可用内存索引表，每个POOL申请一次，后面分割给每个cluster使用 */
    ppst1stImmMem = (IMM_MEM_STRU **)IMM_MemCtrlMemAlloc(sizeof(IMM_MEM_STRU *) * pstImmMemPool->usImmMemTotalCnt);

    if (VOS_NULL_PTR == ppst1stImmMem)
    {
        return IMM_FAIL;
    }

    /* 申请对应内存池的IMM_MEM，每个POOL申请一次，后面分割给每个cluster使用 */
    pstImmMem = (IMM_MEM_STRU *)IMM_MemCtrlMemAlloc(sizeof(IMM_MEM_STRU) * pstImmMemPool->usImmMemTotalCnt);

    if (VOS_NULL_PTR == pstImmMem)
    {
        return IMM_FAIL;
    }

    memset(ppst1stImmMem, 0, sizeof(IMM_MEM_STRU *) * pstImmMemPool->usImmMemTotalCnt);
    memset(pstImmMem, 0, sizeof(IMM_MEM_STRU) * pstImmMemPool->usImmMemTotalCnt);

    ppTempMem = ppst1stImmMem;

    /* 记录该内存池所有IMM_MEM指针，检查内存泄露使用 */
    pstImmMemPool->pstImmMemStStartAddr = pstImmMem;

    /* 建立每个cluster的IMM_MEM */
    for (ucClusterId = 0; ucClusterId < pstImmMemPool->ucClusterCnt; ++ucClusterId )
    {
        pstImmMemCluster    = &(pstImmMemPool->astClusterTable[ucClusterId]);

        /* 建立级数索引表和可用内存索引表的关系 */
        pstImmMemCluster->apstFreeStack = ppTempMem;

        for(ulImmMemCnt = 0;
            ulImmMemCnt < pstImmMemCluster->usFreeCnt;
            ulImmMemCnt ++)
        {
            pstImmMem->usMagicNum   = IMM_MAGIC_NUM;
            pstImmMem->enPoolId     = ucPoolId;
            pstImmMem->ucClusterId  = ucClusterId;

            if ( IMM_MEM_POOL_ID_EXT == pstImmMem->enPoolId )
            {
                pstImmMem->pstMemBlk = VOS_NULL_PTR;
            }
            else
            {
                pstImmMem->pstMemBlk    = *ppucDataMemAddr; /* 建立IMM_MEM_STRU和对应数据的关系 */
            }

            /* 建立可用内存索引表和IMM_MEM_STRU的关系 */
           *ppTempMem               = pstImmMem;

            pstImmMem++;
            ppTempMem++;

            *ppucDataMemAddr       += pstImmMemCluster->usLen;
        }

    }

    return IMM_SUCC;
} /* IMM_MemPoolInit */



unsigned int IMM_MemPoolCreate
(
    const IMM_MEM_POOL_CFG_INFO_STRU   *pstPoolCfg,
    unsigned char                     **ppucDataMemAddr

)
{
    IMM_MEM_POOL_STRU                  *pstImmMemPool;
    IMM_MEM_CLUSTER_CFG_INFO_STRU      *pstPoolClusterCfgTable;
    IMM_MEM_CLUSTER_CFG_INFO_STRU      *pstClusterCfgInfo;
    IMM_MEM_CLUSTER_STRU               *pstMemCluster;
    unsigned short                      usLastClusterMaxLen;
    unsigned short                      usDataLen;
    unsigned char                       ucPoolId;
    unsigned char                       ucClusterId;
    unsigned char                      *pucDataMemAddr;
    unsigned char                       ucClusterCnt = 0;


    if ( VOS_NULL_PTR == *ppucDataMemAddr )
    {
        /*lint -e718 -e746*/
        panic("IMM_MemPoolCreate Fail, input *ppucDataMemAddr is NULL PRT \n");
        /*lint +e718 +e746*/
        return  IMM_FAIL;
    }

    if ( IMM_MEM_POOL_ID_BUTT <= pstPoolCfg->enPoolId )
    {
        /*lint -e515 -e830*/
        panic("IMM_MemPoolCreate Fail, ulPoolId %d exceed the Ranger \n",
            pstPoolCfg->enPoolId);
        /*lint +e515 +e830*/
        return  IMM_FAIL;
    }


    pucDataMemAddr  = (unsigned char *)(*ppucDataMemAddr);

    ucPoolId        = pstPoolCfg->enPoolId;

    pstImmMemPool   = IMM_MEM_GET_POOL(ucPoolId);

    /* 检查对应ID的内存池是否有被重复创建 */
    if ( IMM_TRUE == pstImmMemPool->ucUsedFlag )
    {
        /*lint -e515 -e830*/
        panic("IMM_MemPoolCreate Fail, ulPoolId %d is already used\r\n",
            ucPoolId);
        /*lint +e515 +e830*/
        return IMM_FAIL;
    }


    ucClusterCnt                    = pstPoolCfg->ucClusterCnt;
    pstPoolClusterCfgTable          = pstPoolCfg->pstClusterCfgInfo;

    /* 初始化内存池的通用变量 */
    pstImmMemPool->ucUsedFlag       = IMM_FALSE;
    pstImmMemPool->ucClusterCnt     = ucClusterCnt;
    pstImmMemPool->usImmMemTotalCnt = 0;

    /* 每个POOL的最后一级size最大，所以偏移到最后一级，求出该POOL的最大size */
    pstClusterCfgInfo               = pstPoolClusterCfgTable + (ucClusterCnt - 1);
    pstImmMemPool->usMaxByteLen     = pstClusterCfgInfo->usSize;


    /* 申请长度索引表，以长度为数组下标，所以最大长度要比数组个数小1 */
    if (pstImmMemPool->usMaxByteLen > (IMM_MEM_BLK_MAX_BYTE_LEN - 1))
    {
        /*lint -e515 -e830*/
        panic("IMM_MemPoolCreate Fail, MaxByteLen:%d too big! \n",
            pstImmMemPool->usMaxByteLen);
        /*lint +e515 +e830*/
        return IMM_FAIL;
    }

    if (pstImmMemPool->ucClusterCnt > IMM_MEM_BLK_MAX_CLUSTER_NUM)
    {
        /*lint -e515 -e830*/
        panic("IMM_MemPoolCreate Fail, ClusterCnt:%d exceed! \n",
            pstImmMemPool->ucClusterCnt);
        /*lint -e515 -e830*/
        return IMM_FAIL;
    }

    memset(pstImmMemPool->aucLenIndexTable, 0, IMM_MEM_BLK_MAX_BYTE_LEN);

    memset(pstImmMemPool->astClusterTable, 0,
        sizeof(IMM_MEM_CLUSTER_STRU) * IMM_MEM_BLK_MAX_CLUSTER_NUM);

    usLastClusterMaxLen   = 1;

    for (ucClusterId = 0; ucClusterId < ucClusterCnt; ucClusterId++)
    {
        pstClusterCfgInfo               = pstPoolClusterCfgTable + ucClusterId;
        pstMemCluster                   = &(pstImmMemPool->astClusterTable[ucClusterId]);

        pstMemCluster->usTotalCnt       = pstClusterCfgInfo->usCnt;
        pstMemCluster->usFreeCnt        = pstClusterCfgInfo->usCnt;
        pstMemCluster->usLen            = pstClusterCfgInfo->usSize;

       *ppucDataMemAddr                += (pstClusterCfgInfo->usSize) * (pstClusterCfgInfo->usCnt);

        for(usDataLen = usLastClusterMaxLen;
            usDataLen <= pstClusterCfgInfo->usSize;
            usDataLen++)
        {
            pstImmMemPool->aucLenIndexTable[usDataLen]      = ucClusterId;
        }

        usLastClusterMaxLen                 = usDataLen;

        pstImmMemPool->usImmMemTotalCnt    += pstClusterCfgInfo->usCnt;
    }

    if( IMM_SUCC != IMM_MemPoolInit(ucPoolId, &pucDataMemAddr))
    {
        return IMM_FAIL;
    }

    pstImmMemPool->ucUsedFlag = IMM_TRUE;

    return IMM_SUCC;
} /* IMM_MemPoolCreate */

unsigned int IMM_MemGetBaseAddr(void)
{
    BSP_DDR_SECT_QUERY             stQuery;
    BSP_DDR_SECT_INFO              stInfo;
    unsigned int                  ulBaseAddr;

    stQuery.enSectType = BSP_DDR_SECT_TYPE_TTF;
    DRV_GET_FIX_DDR_ADDR(&stQuery, &stInfo);

    ulBaseAddr = stInfo.ulSectVirtAddr;

    return (IMM_MEM_POOL_BASE_ADDR(ulBaseAddr));
}
unsigned int IMM_MemBlkInit(void)
{
    unsigned int                        ulRet1;
    unsigned int                        ulRet2;
    unsigned char                      *pucBaseAddr;
    IMM_MEM_POOL_CFG_INFO_STRU          stSharePoolCfgInfo;
    IMM_MEM_POOL_CFG_INFO_STRU          stExtPoolCfgInfo;
    unsigned int                       ulBaseAddr;

    /*初始化函数只会被skbuf_init 调用 */
    if ( IMM_TRUE == g_ulImmMemInitFlag )
    {
        vos_printf( "IMM_MemBlkInit, Warning, Memory pools were already initialized!\n");

        return IMM_SUCC;
    }

    /* 设置内存池信息 */
    memset(g_astImmMemPool, 0, sizeof(IMM_MEM_POOL_STRU)* IMM_MEM_POOL_ID_BUTT);
    memset(&g_stImmRegMntnFuc, 0, sizeof(IMM_MEM_REG_MNTN_FUNC_STRU));

    /* 初始化 IMM MEM FREE 可维可测实体 */
    memset(&g_stImmMemFreeMntnEntity, 0, sizeof(IMM_MEM_FREE_MNTN_INFO_STRU));

    memset(&stSharePoolCfgInfo, 0, sizeof(IMM_MEM_POOL_CFG_INFO_STRU));
    memset(&stExtPoolCfgInfo, 0, sizeof(IMM_MEM_POOL_CFG_INFO_STRU));

    /* 保存共享内存池配置参数 */
    stSharePoolCfgInfo.pstClusterCfgInfo       = (IMM_MEM_CLUSTER_CFG_INFO_STRU *)g_astImmMemSharePoolClusterTableInfo;
    stSharePoolCfgInfo.ucClusterCnt            = IMM_MEM_SHARE_POOL_CLUSTER_CNT;
    stSharePoolCfgInfo.enPoolId                = IMM_MEM_POOL_ID_SHARE;

    /* 保存外部内存池配置参数 */
    stExtPoolCfgInfo.pstClusterCfgInfo        = (IMM_MEM_CLUSTER_CFG_INFO_STRU *)g_astImmMemCtrlPoolClusterTableInfo;
    stExtPoolCfgInfo.ucClusterCnt             = IMM_MEM_CTRL_POOL_CLUSTER_CNT;
    stExtPoolCfgInfo.enPoolId                 = IMM_MEM_POOL_ID_EXT;

    ulBaseAddr = IMM_MemGetBaseAddr();

    /* 内存块基地址，进行实地址转虚地址 */
    pucBaseAddr = (unsigned char *)ulBaseAddr;

    /*创建IMM_MEM内存块释放保存可维可测信息所需的自旋锁*/
    VOS_SpinLockInit(&g_stImmFreeMntnSpinLock);

    /*创建共享内存池对应的自旋锁*/
    VOS_SpinLockInit(&g_astImmSpinLock[IMM_MEM_POOL_ID_SHARE]);

    /* 创建共享内存池 */
    ulRet1 = IMM_MemPoolCreate(&stSharePoolCfgInfo, &pucBaseAddr);

    /*创建外部内存池对应的自旋锁*/
    VOS_SpinLockInit(&g_astImmSpinLock[IMM_MEM_POOL_ID_EXT]);

    /* 创建外部内存池 */
    ulRet2 = IMM_MemPoolCreate(&stExtPoolCfgInfo, &pucBaseAddr);

    /* 置上已被初始化的标志 */
    g_ulImmMemInitFlag = IMM_TRUE;

    if ( ( IMM_FAIL == ulRet1 ) || ( IMM_FAIL == ulRet2 ) )
    {
        /*lint -e515 -e830*/
        panic("Create IMM failed ulRet1 = %d, ulRet2 = %d ! \n", ulRet1, ulRet2);
        /*lint +e515 +e830*/
        return IMM_FAIL;
    }

    return IMM_SUCC;

} /* IMM_MemBlkInit */

unsigned int IMM_MemFreeMemCheck(unsigned short usFileId, unsigned short usLineNum, IMM_MEM_STRU **ppstImm, VOS_SPINLOCK *pstSpinLock, VOS_UINT32 ulFlags)
{
    if ( VOS_NULL_PTR == ppstImm )
    {
        vos_printf( "Error: IMM_MemFreeMemCheck Input pstMem is NULL! FileId=%d,LineNum=%d \n", usFileId, usLineNum);
        /*lint -e717*/
        IMM_MEM_FREE_SAVE_NULL_PTR_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeNullPPtrInfo,usFileId,usLineNum,pstSpinLock,ulFlags);
        /*lint +e717*/
        return IMM_FAIL;
    }

    if ( VOS_NULL_PTR == *ppstImm )
    {
        vos_printf("Error: IMM_MemFreeMemCheck Input *pstMem is NULL! FileId=%d,LineNum=%d \n", usFileId, usLineNum);
        /*lint -e717*/
        IMM_MEM_FREE_SAVE_NULL_PTR_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeNullPtrInfo,usFileId,usLineNum,pstSpinLock,ulFlags);
        /*lint +e717*/
        return IMM_FAIL;
    }

    if ( IMM_MAGIC_NUM != (*ppstImm)->usMagicNum)
    {
        vos_printf("IMM_MemFreeMemCheck, Magic Num %d is invalid! FileId=%d,LineNum=%d \n",
                    (int)(*ppstImm)->usMagicNum, usFileId, usLineNum);
        /*lint -e717*/
        IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo,*ppstImm,IMM_INVALID_MEM_TYPE_MAGICNUM,(*ppstImm)->usMagicNum,usFileId,usLineNum,pstSpinLock,ulFlags);
        /*lint +e717*/
        return IMM_FAIL;
    }

    if( IMM_MEM_POOL_ID_BUTT <= (*ppstImm)->enPoolId )
    {
        vos_printf("IMM_MemFreeMemCheck, PoolId %d is invalid! FileId=%d,LineNum=%d \n",
                    (int)(*ppstImm)->enPoolId,usFileId, usLineNum );
        /*lint -e717*/
        IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo,*ppstImm,IMM_INVALID_MEM_TYPE_POOLID,(*ppstImm)->enPoolId,usFileId,usLineNum,pstSpinLock,ulFlags);
        /*lint +e717*/
        return IMM_FAIL;
    }

    return IMM_SUCC;
}


#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)

unsigned int IMM_MemGetSlice(void)
{
    if ( VOS_NULL_PTR == g_pstImmRegMntnFuc->pImmOmGetSliceFunc )
    {
        return 0;
    }

    return g_pstImmRegMntnFuc->pImmOmGetSliceFunc();
}/* IMM_MemGetSlice */

void IMM_MemSaveAllocDebugInfo
(
    IMM_MEM_STRU *pstImmMem,
    unsigned short usFileID,
    unsigned short usLineNum
)
{
    IMM_BLK_MEM_DEBUG_STRU             *pstDbgInfo  =  &pstImmMem->stDbgInfo;


    IMM_DEBUG_TRACE_FUNC_ENTER();

    pstDbgInfo->enMemStateFlag  = MEM_BLK_STATE_ALLOC;
    pstDbgInfo->usAllocFileID   = usFileID;
    pstDbgInfo->usAllocLineNum  = usLineNum;
    pstDbgInfo->ulAllocTick     = IMM_MemGetSlice();
    pstDbgInfo->usTraceFileID   = usFileID;
    pstDbgInfo->usTraceLineNum  = usLineNum;

    IMM_DEBUG_TRACE_FUNC_LEAVE();

    return;
}/* IMM_MemSaveAllocDebugInfo */

unsigned char IMM_MemSaveFreeDebugInfo(unsigned short usFileId, unsigned short usLineNum, IMM_MEM_STRU *pstImmMem, VOS_SPINLOCK *pstSpinLock, VOS_UINT32 ulFlags)
{
    unsigned int                ulMemAddr;
    IMM_BLK_MEM_DEBUG_STRU      *pstImmMemDebugInfo = &pstImmMem->stDbgInfo;


    IMM_DEBUG_TRACE_FUNC_ENTER();

    ulMemAddr = (unsigned int)pstImmMem;

    /*pstImmMem 地址范围检查*/
    if (( (unsigned int)g_paulImmMemCtrlMemSpace > ulMemAddr )
        ||(((unsigned int)g_paulImmMemCtrlMemSpace + (*g_pulImmMemCtrlMemSuffix)) < ulMemAddr))
    {
        /*lint -e717*/
        IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo, pstImmMem,IMM_INVALID_MEM_TYPE_CTRLADDR, 0, usFileId, usLineNum,pstSpinLock,ulFlags);
        /*lint +e717*/
        vos_printf( "Error: IMM_MemFreeSaveDebugInfo, Mem addr %d is invalid! \n", (int)pstImmMem);

        return IMM_FAIL;
    }

    /* 判断内存指针状态是否正确 */
    if ( MEM_BLK_STATE_ALLOC != pstImmMemDebugInfo->enMemStateFlag )
    {
        /*lint -e717*/
        IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo, pstImmMem,IMM_INVALID_MEM_TYPE_STUTAS, (unsigned short)pstImmMemDebugInfo->enMemStateFlag, usFileId, usLineNum,pstSpinLock,ulFlags);
        /*lint +e717*/
        vos_printf("Error: IMM_MemFreeSaveDebugInfo, free a free status block is not allowed \n");

        return IMM_FAIL;
    }

    pstImmMemDebugInfo->enMemStateFlag   = MEM_BLK_STATE_FREE;
    pstImmMemDebugInfo->ulAllocTick      = IMM_MemGetSlice();
    pstImmMemDebugInfo->usAllocFileID    = 0;

    IMM_DEBUG_TRACE_FUNC_LEAVE();

    return IMM_SUCC;

}/* IMM_MemFreeSaveDebugInfo */
#endif
IMM_MEM_POOL_STRU *IMM_MemPoolGet(unsigned char ucPoolId)
{
    if ( IMM_MEM_POOL_ID_BUTT > ucPoolId )
    {
        return &g_astImmMemPool[ucPoolId];
    }
    else
    {
        return VOS_NULL_PTR;
    }

} /* IMM_MemPoolGet */

unsigned int IMM_ZcGetLocalFreeMemCnt(void)
{
    IMM_DEBUG_TRACE_FUNC_ENTER();
    IMM_DEBUG_TRACE_FUNC_LEAVE();

    return *g_pACpuFreeMemBlkCnt;
} /* IMM_ZcGetLocalFreeMemCnt */

void IMM_ZcSetLocalFreeMemCnt(unsigned int ulMemValue)
{
    IMM_DEBUG_TRACE_FUNC_ENTER();
    IMM_DEBUG_TRACE_FUNC_LEAVE();

    *g_pACpuFreeMemBlkCnt = ulMemValue;
    return;
}/* IMM_ZcSetLocalFreeMemCnt */

IMM_MEM_STRU *IMM_MemAlloc_Debug(unsigned short usFileID, unsigned short usLineNum,
                        unsigned char ucPoolId, unsigned int ulLen)
{
    IMM_MEM_POOL_STRU              *pstImmMemPool;
    IMM_MEM_CLUSTER_STRU           *pstImmMemCluster;
    IMM_MEM_STRU                   *pMem;
    unsigned int                    ulMaxClusterFreeCnt;
    unsigned char                   ucClusterId;
    unsigned char                   ucMostFitLev;
    unsigned int                    ulSaveFlags  = 0;
    VOS_SPINLOCK                   *pstImmSpinLock;


    IMM_DEBUG_TRACE_FUNC_ENTER();

    if (IMM_MEM_POOL_ID_EXT <= ucPoolId )   /*如果为EXT直接退出*/
    {
        vos_printf("IMM_MemBlkAlloc_Debug,Poolid %d is invalid,FileId=%d,LineNum=%d!\n", ucPoolId,usFileID,usLineNum);
        return VOS_NULL_PTR;
    }

     /* 获取内存池 */
    pstImmMemPool = IMM_MEM_GET_POOL(ucPoolId);

    /* 获取自旋锁 */
    pstImmSpinLock = &g_astImmSpinLock[ucPoolId];

    /* 检查内存池是否已经初始化 */
    if ( IMM_TRUE != pstImmMemPool->ucUsedFlag )
    {
        vos_printf("IMM_MemBlkAlloc_Debug,Pool %d is not used,FileId=%d,LineNum=%d!\n", ucPoolId,usFileID,usLineNum);
        return VOS_NULL_PTR;
    }


    if ( (0 == ulLen ) || (ulLen > pstImmMemPool->usMaxByteLen) )
    {
        vos_printf("IMM_MemBlkAlloc_Debug,usLen %d exceed the Pool %d ranger,FileId=%d,LineNum=%d!\n", (int)ulLen, ucPoolId,usFileID,usLineNum);
        return VOS_NULL_PTR;
    }

    /* 从内存池的多个簇里寻找合适的IMM_MEM */
    ucMostFitLev = IMM_GET_LEN_INDEX(pstImmMemPool, ulLen);

    for(ucClusterId = ucMostFitLev;
        ucClusterId < pstImmMemPool->ucClusterCnt;
         ++ucClusterId )
    {
        pstImmMemCluster  = &(pstImmMemPool->astClusterTable[ucClusterId]);

        VOS_SpinLockIntLock(pstImmSpinLock, ulSaveFlags);

        if (0 != pstImmMemCluster->usFreeCnt)
        {
            /* 获取一个没有使用的IMM_MEM_ST结点 */
            pMem    = pstImmMemCluster->apstFreeStack[--pstImmMemCluster->usFreeCnt];
            ++pstImmMemPool->usImmMemUsedCnt;
            ulMaxClusterFreeCnt = pstImmMemPool->astClusterTable[pstImmMemPool->ucClusterCnt - 1].usFreeCnt;

            IMM_ZcSetLocalFreeMemCnt(ulMaxClusterFreeCnt);

            VOS_SpinUnlockIntUnlock(pstImmSpinLock ,ulSaveFlags);

            /* 向流控模块上报内存申请事件 */
            if (VOS_NULL_PTR != pstImmMemPool->pMemAllocEvent)
            {
                pstImmMemPool->pMemAllocEvent( ulMaxClusterFreeCnt );
            }

#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
            IMM_MemSaveAllocDebugInfo( pMem, usFileID, usLineNum );
#endif
            IMM_DEBUG_TRACE_FUNC_LEAVE();

            return pMem;
        }

        VOS_SpinUnlockIntUnlock(pstImmSpinLock ,ulSaveFlags);
    }

    VOS_SpinLockIntLock(pstImmSpinLock, ulSaveFlags);

    (*g_apusImmAllocFailCnt[ucPoolId])++;

    if ( *g_apusImmAllocFailCnt[ucPoolId] < g_usImmAllocFailCntThreshold )
    {
        VOS_SpinUnlockIntUnlock(pstImmSpinLock, ulSaveFlags);
        return VOS_NULL_PTR;
    }

    *g_apusImmAllocFailCnt[ucPoolId] = 0;
    (*g_apusImmExcThresholdCnt[ucPoolId])++;

    VOS_SpinUnlockIntUnlock(pstImmSpinLock, ulSaveFlags);

    /* 向SDT上报内存申请失败事件 */
    if ( VOS_NULL_PTR != g_pstImmRegMntnFuc->pImmMemEventRptFunc)
    {
        g_pstImmRegMntnFuc->pImmMemEventRptFunc(ucPoolId, IMM_MEM_TRIG_TYPE_ALLOC_FAIL);
    }


    return VOS_NULL_PTR;
} /* IMM_MemAlloc_Debug */

void IMM_MemFree_Debug(unsigned short usFileId, unsigned short usLineNum, IMM_MEM_STRU **ppstMem)
{
    IMM_MEM_POOL_STRU                  *pstImmMemPool;
    IMM_MEM_CLUSTER_STRU               *pstImmMemCluster;
    IMM_MEM_STRU                       *pstImmMem;
    unsigned int                        ulMaxClusterFreeCnt;
    unsigned int                        ulSaveFlags  = 0;
    VOS_SPINLOCK                       *pstImmSpinLock;


    IMM_DEBUG_TRACE_FUNC_ENTER();

    /* 待释放内存合法性检查 */
    if ( IMM_SUCC != IMM_MemFreeMemCheck(usFileId, usLineNum, ppstMem, &g_stImmFreeMntnSpinLock, ulSaveFlags) )
    {
        vos_printf( "Error: IMM_MemFree_Debug input ppstMem is invalid! FileId=%d,LineNum=%d \n", usFileId, usLineNum);
        return;
    }

    pstImmMem       = *ppstMem;

    pstImmMemPool   = IMM_MEM_GET_POOL(pstImmMem->enPoolId);

    /* 检查对应的内存是否使用 */
    if (IMM_TRUE != pstImmMemPool->ucUsedFlag)
    {
        vos_printf("Error: IMM_MemFree_Debug, Pool %d is not used! \n",
                pstImmMem->enPoolId);
        return ;
    }

#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
    if ( IMM_SUCC != IMM_MemSaveFreeDebugInfo(usFileId, usLineNum, pstImmMem, &g_stImmFreeMntnSpinLock, ulSaveFlags) )
    {
        vos_printf("Error: IMM_MemFree_Debug, free IMM MEM fail! FileId=%d, LineNum=%d \n", usFileId, usLineNum);
        return;
    }
#endif

    /* 外部内存池，需要释放外部内存块 */
    if ( IMM_MEM_POOL_ID_EXT == pstImmMem->enPoolId )
    {
        vos_printf("Error: IMM_MemFree_Debug, Pool ID is IMM_MEM_POOL_ID_EXT! \n");
        return ;
    }

    /* 获取内存块所在的档位 */
    pstImmMemCluster = &(pstImmMemPool->astClusterTable[pstImmMem->ucClusterId]);

    /* 获取自旋锁 */
    pstImmSpinLock  = &g_astImmSpinLock[pstImmMem->enPoolId];

    VOS_SpinLockIntLock(pstImmSpinLock, ulSaveFlags);

    if (pstImmMemCluster->usFreeCnt >= pstImmMemCluster->usTotalCnt)
    {
        VOS_SpinUnlockIntUnlock(pstImmSpinLock ,ulSaveFlags);
        vos_printf("Error: IMM_MemFree_Debug, free cnt error:FreeCnt %d, TotalCnt %d ! \n",
                pstImmMemCluster->usFreeCnt, pstImmMemCluster->usTotalCnt);

        return ;
    }

    /* 将内存块挂接回内存池 */
    pstImmMemCluster->apstFreeStack[pstImmMemCluster->usFreeCnt] = pstImmMem;
    ++pstImmMemCluster->usFreeCnt;
    --pstImmMemPool->usImmMemUsedCnt;
    /* 计算内存池最大档位剩余内存块数 */
    ulMaxClusterFreeCnt = pstImmMemPool->astClusterTable[pstImmMemPool->ucClusterCnt - 1].usFreeCnt;

    IMM_ZcSetLocalFreeMemCnt(ulMaxClusterFreeCnt);

    VOS_SpinUnlockIntUnlock(pstImmSpinLock ,ulSaveFlags);


    /* 向流控模块上报内存释放事件 */
    if ( VOS_NULL_PTR != pstImmMemPool->pMemFreeEvent )
    {
        pstImmMemPool->pMemFreeEvent(ulMaxClusterFreeCnt );
    }

    *ppstMem = VOS_NULL_PTR;

    IMM_DEBUG_TRACE_FUNC_LEAVE();

    return ;
} /* IMM_MemFree_Debug */

unsigned int IMM_MemRegExtFreeCallBack
(
    IMM_MEM_EXT_FREE_CALLBACK           pMemExtFreeFunc
)
{
    IMM_MEM_POOL_STRU * const           pstIMMMemPool = g_pstImmExtMemPool;


    if ( VOS_NULL_PTR == pMemExtFreeFunc )
    {
        return IMM_FAIL;
    }

    pstIMMMemPool->pMemExtFreeFunc = pMemExtFreeFunc;

    return IMM_SUCC;
} /* IMM_MemRegExtFreeCallBack */

void IMM_MemRegMntnFuncCallBack
(
    IMM_OM_GET_SLICE_CALLBACK               pImmOmGetSliceFunc,
    IMM_MEM_USED_INFO_EVENT_RPT_CALLBACK    pImmMemEventRptFunc
)
{
    /* 参数检查，pImmOmGetSliceFunc 可以在非DEBUG模式下为空 */
    if ( VOS_NULL_PTR == pImmMemEventRptFunc )
    {
        vos_printf("IMM_MemRegPsFuncCallBack fail: invalid input paras! \n");
        return;
    }

    g_pstImmRegMntnFuc->pImmOmGetSliceFunc    = pImmOmGetSliceFunc;
    g_pstImmRegMntnFuc->pImmMemEventRptFunc   = pImmMemEventRptFunc;

    return;
}
unsigned int IMM_MemRegEventCallBack
(
    IMM_MEM_POOL_ID_ENUM_UINT8          enPoolId,
    IMM_MEM_EVENT_CALLBACK              pMemAllocEvent,
    IMM_MEM_EVENT_CALLBACK              pMemFreeEvent
)
{
    IMM_MEM_POOL_STRU              *pstImmMemPool;
    unsigned int                    ulMaxClusterFreeCnt;

    pstImmMemPool   = IMM_MemPoolGet(enPoolId);

    if ( VOS_NULL_PTR == pstImmMemPool )
    {
        return IMM_FAIL;
    }

    pstImmMemPool->pMemAllocEvent = pMemAllocEvent;
    pstImmMemPool->pMemFreeEvent  = pMemFreeEvent;

    /* 获取最大档位内存的剩余块数 */
    ulMaxClusterFreeCnt = pstImmMemPool->astClusterTable[pstImmMemPool->ucClusterCnt - 1].usFreeCnt;
    IMM_ZcSetLocalFreeMemCnt(ulMaxClusterFreeCnt);

    return IMM_SUCC;
} /* IMM_MemRegEventCallBack */



void IMM_MemFreeShowMntnInfo( void )
{
    IMM_MEM_FREE_INVALIED_MEM_INFO_STRU    *pstInvalidMemInfo   = &g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo;
    IMM_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPtrInfo      = &g_stImmMemFreeMntnEntity.astImmMemFreeNullPtrInfo;
    IMM_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPPtrInfo     = &g_stImmMemFreeMntnEntity.astImmMemFreeNullPPtrInfo;


    vos_printf( "IMM MEM FREE 可维可测信息 :\n");
    vos_printf( "==========================================\n");
    vos_printf( "IMM MEM FREE 非法内存信息 :\n");
    vos_printf( "IMM MEM FREE 收到的非法内存个数       : %u \n", (unsigned int)pstInvalidMemInfo->ulInvalidImmMemCnt);
    vos_printf( "IMM MEM FREE 非法IMM MEM 内存地址     : 0x%u \n", (unsigned int)pstInvalidMemInfo->ulInvalidImmMemAddr);
    vos_printf( "IMM MEM FREE 非法IMM MEM 内存文件ID   : %d \n", pstInvalidMemInfo->usImmMemFreeFileId);
    vos_printf( "IMM MEM FREE 非法IMM MEM 内存行号信息 : %d \n", pstInvalidMemInfo->usImmMemFreeLineNum);
    vos_printf( "IMM MEM FREE 非法IMM MEM 内存类型     : %d \n", pstInvalidMemInfo->ucImmMemInvalidType);
    vos_printf( "IMM MEM FREE 非法IMM MEM 内存值       : %d \n", pstInvalidMemInfo->usImmMemInvalidValue);
    vos_printf( "IMM MEM 内存控制头起始地址            : 0x%u ~ 0x%u \n\n", (unsigned int)g_paulImmMemCtrlMemSpace,((unsigned int)((unsigned int)g_paulImmMemCtrlMemSpace + (*g_pulImmMemCtrlMemSuffix))) );


    vos_printf( "==========================================\n");
    vos_printf( "IMM MEM FREE 空指针信息 :\n");
    vos_printf( "IMM MEM FREE 一级指针为空的次数    : %u \n", (unsigned int)pstNullPtrInfo->ulImmMemFreeNullPtrCnt);
    vos_printf( "IMM MEM FREE 输入指针文件ID        : %d \n", pstNullPtrInfo->usImmMemFreeFileId);
    vos_printf( "IMM MEM FREE 输入指针内存行号信息  : %d \n", pstNullPtrInfo->usImmMemFreeLineNum);

    vos_printf( "IMM MEM FREE 二级指针为空的次数    : %u \n", (unsigned int)pstNullPPtrInfo->ulImmMemFreeNullPtrCnt);
    vos_printf( "IMM MEM FREE 输入指针内存文件ID    : %d \n", pstNullPPtrInfo->usImmMemFreeFileId);
    vos_printf( "IMM MEM FREE 输入指针内存行号信息  : %d \n", pstNullPPtrInfo->usImmMemFreeLineNum);

    vos_printf( "IMM MEM Pool Status: IMM_MEM_POOL_ID_DL_SHARE %d\n", g_astImmMemPool[IMM_MEM_POOL_ID_SHARE].ucUsedFlag);
    vos_printf( "                     IMM_MEM_POOL_ID_EXT      %d\n", g_astImmMemPool[IMM_MEM_POOL_ID_EXT].ucUsedFlag);

    vos_printf( "IMM_MEM_POOL_ID_EXT 释放函数      : 0x%u\n", (unsigned int)g_astImmMemPool[IMM_MEM_POOL_ID_EXT].pMemExtFreeFunc);
    vos_printf( "IMM Ext MEM FREE 失败次数         : %u \n", (unsigned int)g_stImmMemFreeMntnEntity.ulImmMemExtFreeFailCnt);

}

#endif

/******************************************************************************/
/*******************************V9R1版本***************************************/
/******************************************************************************/

unsigned int IMM_MemRegEventCallBack
(
    IMM_MEM_POOL_ID_ENUM_UINT8          enPoolId,
    IMM_MEM_EVENT_CALLBACK              pMemAllocEvent,
    IMM_MEM_EVENT_CALLBACK              pMemFreeEvent
)
{
    return IMM_SUCC;
} /* IMM_MemRegEventCallBack */

unsigned int IMM_ZcGetLocalFreeMemCnt(void)
{
    return  0;
} /* IMM_ZcGetLocalFreeMemCnt */



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

