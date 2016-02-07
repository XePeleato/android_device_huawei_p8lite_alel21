#include <asm/cacheflush.h>
#include <asm/cache.h>
#include <asm/tlbflush.h>
/*#include <asm/system.h>*/
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */

#include  <bsp_mem.h>
#include  <bsp_mem_drv.h>
#include  "BSP.h"
//#include "bsp_drv_ipc.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**************************************************************************
  宏定义
**************************************************************************/
#define __BSP_MEM_DEBUG__

#ifdef __BSP_MEM_DEBUG__
/* 用于打印查找次数, 方便设置 MostUseItem 的值 */
/*#define MEM_PRINT_FIND_TIMES*/
#endif

/* 每个内存节点的最大值定义 */
#define MEM_NODE_MGR_SIZE           32

/* MAGIC定义码 */
#define MEM_MAGIC_NUM               0x11223344

/* 内存尺寸分配表 */
BSP_U32 sg_AllocListSize[] =        {32, 128, 512, 1024, 2048, 4096, 8192, 0x4000, 0x8000, 0x20000};

#define MEM_ALLOC_LIST_NUM          (sizeof(sg_AllocListSize) / sizeof(BSP_U32))


/**************************************************************************
  类型定义
**************************************************************************/
/* 内存节点状态类型 */
typedef enum tagMEM_STATUS
{
    MEM_FREE = 0,
    MEM_ALLOC = 1
}MEM_STATUS;

/* 内存池管理信息 */
typedef struct tagMEM_POOL_INFO
{
    BSP_U32 u32BaseAddr;            /* 内存池基地址 */
    BSP_U32 u32Size;                /* 内存池总大小 */
    BSP_U32 u32CurPosAddr;          /* 内存池当前分配到的位置 */
    BSP_U32 u32Left;                /* 内存池剩余大小 */
    BSP_U32 u32MgrSize;             /* 管理结构大小 */
}MEM_POOL_INFO;

/* 每个内存节点的管理信息(注意,最大不要超过 32bytes) */
typedef struct tagMEM_MGR_INFO
{
    BSP_U32 u32MagicNumber;
    BSP_U32 u32Next;
    BSP_U32 u32Size;
    BSP_U32 u32Flags;
#ifdef __BSP_MEM_DEBUG__
    BSP_U32 u32FileName;
    BSP_U32 u32Line;
    BSP_U32 u32Status;
#endif
}MEM_MGR_INFO;

/* 每个内存节点的使用计数 */
typedef struct tagMEM_USED_INFO
{
    BSP_U32 u32CurNum;              /* 当前使用个数计数 */
    BSP_U32 u32MaxNum;              /* 使用个数峰值 */
    BSP_U32 u32TotalMallocNum;      /* 累积malloc次数 */
    BSP_U32 u32TotalFreeNum;        /* 累积free次数 */
}MEM_USED_INFO;

/* 内存分配管理信息 */
typedef struct tagMEM_ALLOC_INFO
{
    BSP_U32           allocList[MEM_ALLOC_LIST_NUM];
    MEM_USED_INFO   allocUsedInfoList[MEM_ALLOC_LIST_NUM];
    MEM_POOL_INFO   memPoolInfo;
    BSP_U32 mostUsedItem;
}MEM_ALLOC_INFO;

/**************************************************************************
  函数申明
**************************************************************************/
extern void BSP_IPC_SpinLock (unsigned int u32SignalNum);
extern void BSP_IPC_SpinUnLock (unsigned int u32SignalNum);


/**************************************************************************
  全局变量
**************************************************************************/
#ifdef MEM_PRINT_FIND_TIMES
BSP_U32 g_MemIsPrintTimes=0;
#endif

LOCAL MEM_ALLOC_INFO sg_stLocalAllocInfo[1];

#define MEM_CTX_RESERVED 4
static BSP_U32* sg_pAllocSizeTbl = 0;
static MEM_ALLOC_INFO* sg_pIccAllocInfo = 0;

spinlock_t g_ipc_sem_mem_lock = __SPIN_LOCK_UNLOCKED(old_style_spin_init);
/**************************************************************************
  宏实现
**************************************************************************/
#define MEM_GET_ALLOC_SIZE(i)       (*(sg_pAllocSizeTbl+i))
#define MEM_GET_ALLOC_INFO(type)    (((type) >= MEM_ICC_DDR_POOL) ? \
                                    (((MEM_ALLOC_INFO*)(sg_pIccAllocInfo)) + ((BSP_U32)(type)-(BSP_U32)MEM_ICC_DDR_POOL)) : \
                                    (&sg_stLocalAllocInfo[MEM_NORM_DDR_POOL]))
#define MEM_MGR_SIZE_FOR_CACHE      MEM_NODE_MGR_SIZE

#define MEM_GET_ALLOC_ADDR(x)       ((VOID*)((unsigned long)(x)-MEM_NODE_MGR_SIZE))
#define MEM_OFFSET_OF(type, member) ((unsigned long) (&((type *)0)->member))
#define MEM_ITEM_NEXT(x)            (*(BSP_U32*)(((unsigned long)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Next))))
#define MEM_ITEM_SIZE(x)            (*(BSP_U32*)(((unsigned long)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Size))))
#define MEM_ITEM_FLAGS(x)           (*(BSP_U32*)(((unsigned long)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Flags))))
#define MEM_ITEM_MAGIC(x)           (*(BSP_U32*)(((unsigned long)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32MagicNumber))))
#define MEM_ITEM_FILE_NAME(x)       (*(BSP_U32*)(((unsigned long)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32FileName))))
#define MEM_ITEM_LINE(x)            (*(BSP_U32*)(((unsigned long)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Line))))
#define MEM_ITEM_STATUS(x)          (*(BSP_U32*)(((unsigned long)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Status))))


#define MEM_LOCAL_LOCK(flag) \
do{\
    spin_lock_irqsave(&g_ipc_sem_mem_lock, flag);\
}while(0)

#define MEM_LOCAL_UNLOCK(flag)   spin_unlock_irqrestore(&g_ipc_sem_mem_lock, flag)

#define MEM_SPIN_INIT() memset((VOID*)HISI_VA_ADDRESS(MEMORY_RAM_CORESHARE_MEM_ADDR), 0, (MEMORY_RAM_CORESHARE_SYNC_RESV+MEMORY_RAM_CORESHARE_MEM_RESV))
#define MEM_SPIN_LOCK(flag) \
do{\
    MEM_LOCAL_LOCK(flag);\
    BSP_IPC_SpinLock(IPC_SEM_MEM);\
}while(0)

#define MEM_SPIN_UNLOCK(flag) \
do{\
    BSP_IPC_SpinUnLock(IPC_SEM_MEM);\
    MEM_LOCAL_UNLOCK(flag);\
}while(0)

#define MEM_LOCK_BY_TYPE(type, flag)  \
do{\
    if ((BSP_U32)type >= MEM_ICC_DDR_POOL)\
    {\
        MEM_SPIN_LOCK(flag);\
    }\
    else\
    {\
        MEM_LOCAL_LOCK(flag);\
    }\
}while(0)

#define MEM_UNLOCK_BY_TYPE(type, flag) \
do{\
    if ((BSP_U32)type >= MEM_ICC_DDR_POOL)\
    {\
        MEM_SPIN_UNLOCK(flag);\
    }\
    else\
    {\
        MEM_LOCAL_UNLOCK(flag);\
    }\
}while(0)

/* Cache操作定义 */
#ifdef  MEM_USE_ACP
#define MEM_FLUSH_CACHE(ptr, size)
#define MEM_INVALID_CACHE(ptr, size)
#define MEM_FLUSH_CACHE_BY_TYPE(ptr, size, type)
#define MEM_INVALID_CACHE_BY_TYPE(ptr, size, type)
#else   /*MEM_USE_ACP*/
#ifdef CONFIG_ARM64
/*ARM64 stub*/
#define MEM_FLUSH_CACHE(ptr, size)  (VOID)__flush_dcache_area(ptr,size)
#define MEM_INVALID_CACHE(ptr, size) /*(VOID)invalidate_kernel_vmap_range(ptr, size)*/
#else
#define MEM_FLUSH_CACHE(ptr, size)  (VOID)flush_kernel_vmap_range(ptr,size)
#define MEM_INVALID_CACHE(ptr, size) (VOID)invalidate_kernel_vmap_range(ptr, size)
#endif
#define MEM_FLUSH_CACHE_BY_TYPE(ptr, size, type) \
do{\
    if ((BSP_U32)type == MEM_ICC_DDR_POOL)\
    {\
        MEM_FLUSH_CACHE(ptr, size);\
    }\
}while(0)

#define MEM_INVALID_CACHE_BY_TYPE(ptr, size, type)\
do{\
    if ((BSP_U32)type == MEM_ICC_DDR_POOL)\
    {\
        MEM_INVALID_CACHE(ptr, size);\
    }\
}while(0)
#endif /*MEM_USE_ACP*/


#ifdef MEM_PRINT_FIND_TIMES
#define MEM_DEFINE_TIMES() BSP_U32 u32FindTimes=0
#define MEM_INC_TIMES() (u32FindTimes++)
#define MEM_PRINT_TIMES(size) \
do{\
    if (g_MemIsPrintTimes)\
        printf("mem find right item, find times:%d, find size:%d\n", u32FindTimes, size);\
}while(0)
#else
#define MEM_DEFINE_TIMES()
#define MEM_INC_TIMES()
#define MEM_PRINT_TIMES(size)
#endif

/* 在size list 中查找合适的内存节点,如果找不到则返回 MEM_ALLOC_LIST_NUM */
#define MEM_FIND_RIGHT_ITEM(item, size, most_used) \
do {\
    MEM_DEFINE_TIMES();\
    if ((size) > MEM_GET_ALLOC_SIZE(most_used))\
    {\
        MEM_INC_TIMES();\
        for ((item) = (most_used+1); (item) < MEM_ALLOC_LIST_NUM && size > MEM_GET_ALLOC_SIZE(item); (item)++)\
        {\
            MEM_INC_TIMES();\
        }\
    }\
    else\
    {\
        MEM_INC_TIMES();\
        for ((item) = 0; (item) <= (most_used) && (size) > MEM_GET_ALLOC_SIZE(item); (item)++)\
        {\
             MEM_INC_TIMES();\
        }\
        /* 如果无效, 将无效值改为 MEM_ALLOC_LIST_NUM */\
        if ((item) > (most_used))\
        {\
            (item) = MEM_ALLOC_LIST_NUM;\
        }\
    }\
    MEM_PRINT_TIMES(size);\
}while(0)


/**************************************************************************
  内部函数
**************************************************************************/
LOCAL BSP_S32 memInitPool(BSP_U32 u32PoolType)
{
    MEM_ALLOC_INFO* pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);

    /* 分配基地址和大小 */
    switch((MEM_POOL_TYPE)u32PoolType)
    {
        case MEM_NORM_DDR_POOL:
            {
                return OK;
            }
        case MEM_ICC_DDR_POOL:
            {
                pAllocInfo->memPoolInfo.u32CurPosAddr =
                pAllocInfo->memPoolInfo.u32BaseAddr = (BSP_U32)MEMORY_RAM_CORESHARE_MEMMGR_ADDR;
                pAllocInfo->memPoolInfo.u32Left =
                pAllocInfo->memPoolInfo.u32Size = MEM_ICC_DDR_POOL_SIZE;
                pAllocInfo->memPoolInfo.u32MgrSize = MEM_MGR_SIZE_FOR_CACHE;
                break;
            }
        default:
            {
                printk("Invalid pool type:%d, line:%d\n",  u32PoolType,  __LINE__);
                return ERROR;
            }
    }

    if ((BSP_U32)NULL == pAllocInfo->memPoolInfo.u32BaseAddr)
    {
        printk("Invalid pool ptr, line:%d\n", __LINE__);
        return ERROR;
    }

    /* 初始化其他全局变量 */
    pAllocInfo->mostUsedItem = 0;
    return OK;
}

LOCAL INLINE BSP_BOOL memPtrInvalid(VOID* pMem)
{
    BSP_U32 u32Type;
    MEM_POOL_INFO* pPoolInfo;
    BSP_U32 u32FindMem = 0;

    for (u32Type = MEM_NORM_DDR_POOL; u32Type < MEM_ICC_AXI_POOL; u32Type++)
    {
        pPoolInfo = &(MEM_GET_ALLOC_INFO(u32Type)->memPoolInfo);
        if ((BSP_U32)(HISI_PA_ADDRESS(pMem)) >= pPoolInfo->u32BaseAddr ||
            (BSP_U32)(HISI_PA_ADDRESS(pMem)) < pPoolInfo->u32BaseAddr + pPoolInfo->u32Size)
        {
            u32FindMem = 1;
        }
    }
    if (!u32FindMem ||
        MEM_MAGIC_NUM != MEM_ITEM_MAGIC(pMem) ||
        MEM_ITEM_FLAGS(pMem) >= (BSP_U32)MEM_ICC_AXI_POOL)
    {
        return TRUE;
    }
    return FALSE;
}

LOCAL INLINE VOID* memPoolAlloc(MEM_ALLOC_INFO* pAllocInfo, BSP_U32 u32Size)
{
    unsigned long u32RetAddr = 0;

    if (pAllocInfo->memPoolInfo.u32Left < u32Size)
    {
        return NULL;
    }


    u32RetAddr = (unsigned long)pAllocInfo->memPoolInfo.u32CurPosAddr;

    u32RetAddr = HISI_VA_ADDRESS(u32RetAddr);

    pAllocInfo->memPoolInfo.u32CurPosAddr += u32Size;
    pAllocInfo->memPoolInfo.u32Left -= u32Size;

    return (VOID*)(u32RetAddr+pAllocInfo->memPoolInfo.u32MgrSize);
}

LOCAL INLINE VOID* memGetItem(MEM_ALLOC_INFO* pAllocInfo, BSP_U32 cnt, BSP_U32 u32PoolType, BSP_U32 u32Size)
{
    VOID *pItem;
    BSP_U32 *ppHead = &(pAllocInfo->allocList[cnt]);

    /* 如果链表中没有节点,则从内存池中分配 */
    if ((BSP_U32)BSP_NULL == *ppHead)
    {
        /* 注意从内存池中分配的尺寸要额外包含 MGR 的部分 */
    	pItem = memPoolAlloc(pAllocInfo, u32Size+pAllocInfo->memPoolInfo.u32MgrSize);

    	if (NULL == pItem)
        {
            printk("memPoolAlloc Fail, size:%d, line:%d\n", u32Size, __LINE__);
            return NULL;
        }

    	MEM_ITEM_MAGIC(pItem) = (BSP_U32)MEM_MAGIC_NUM;
    	MEM_ITEM_SIZE(pItem) = u32Size;
    	MEM_ITEM_FLAGS(pItem) = u32PoolType;

#ifdef __BSP_MEM_DEBUG__
    	MEM_ITEM_FILE_NAME(pItem) = 0;
    	MEM_ITEM_LINE(pItem) = 0;
#endif
    }
    /* 从链表中取出节点 */
    else
    {
	    pItem = (VOID*)HISI_VA_ADDRESS(((unsigned long)(*ppHead)));

        /* Invalid Cache */
        MEM_INVALID_CACHE_BY_TYPE(MEM_GET_ALLOC_ADDR(pItem), MEM_MGR_SIZE_FOR_CACHE, u32PoolType);
        *ppHead = (BSP_U32)MEM_ITEM_NEXT(pItem);
    }
    return pItem;
}

LOCAL INLINE BSP_U8* memAlloc(BSP_U32 u32PoolType, BSP_U32 u32Size)
{
    BSP_U32 cnt;
    VOID *pItem;
	unsigned long flag = 0;
    MEM_ALLOC_INFO* pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);
    BSP_U32 u32MostUsedItem = pAllocInfo->mostUsedItem;

    /* 先查找AllocList中是否有可用的内存节点 */
    MEM_FIND_RIGHT_ITEM(cnt, u32Size, u32MostUsedItem);

    /* 如果没有找到则直接返回失败 */
    if (cnt >= MEM_ALLOC_LIST_NUM)
    {
        printk("Invalid malloc size:%d, line:%d\n", u32Size, __LINE__);
        return NULL;
    }

    /* 更新size为列表中的size */
    u32Size = MEM_GET_ALLOC_SIZE(cnt);
    MEM_LOCK_BY_TYPE(u32PoolType, flag);
    pItem = memGetItem(pAllocInfo, cnt, u32PoolType, u32Size);

    if (NULL != pItem)
    {
#ifdef __BSP_MEM_DEBUG__
        pAllocInfo->allocUsedInfoList[cnt].u32CurNum++;
        pAllocInfo->allocUsedInfoList[cnt].u32TotalMallocNum++;

        if (pAllocInfo->allocUsedInfoList[cnt].u32CurNum >
            pAllocInfo->allocUsedInfoList[cnt].u32MaxNum)
        {
            pAllocInfo->allocUsedInfoList[cnt].u32MaxNum =
               pAllocInfo->allocUsedInfoList[cnt].u32CurNum;
        }
        MEM_ITEM_STATUS(pItem) = MEM_ALLOC;
#endif
        /* 多核要 Flush Cache, 确保管理信息写入 */
        MEM_FLUSH_CACHE_BY_TYPE(MEM_GET_ALLOC_ADDR(pItem), MEM_MGR_SIZE_FOR_CACHE, u32PoolType);
    }

    MEM_UNLOCK_BY_TYPE(u32PoolType, flag);

    return pItem;
}

INLINE VOID memFree(BSP_U32 u32PoolType, BSP_VOID* pMem, BSP_U32 u32Size)
{

    BSP_U32 cnt;
    BSP_U32 u32MostUsedItem;
    unsigned long flag = 0;
    MEM_ALLOC_INFO* pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);
    u32MostUsedItem = pAllocInfo->mostUsedItem;

    /* 先查找AllocList中是否有可用的内存节点 */
    MEM_FIND_RIGHT_ITEM(cnt, u32Size, u32MostUsedItem);

#ifdef __BSP_MEM_DEBUG__
    /* 判断该节点是否有效 */
    if (cnt >= MEM_ALLOC_LIST_NUM)
    {
        printk("memPoolAlloc Fail, size:%d, line:%d\n", u32Size, __LINE__);
        return;
    }
#endif

    MEM_LOCK_BY_TYPE(u32PoolType, flag);

    /* 将item挂回到链表 */
    MEM_ITEM_NEXT(pMem) = (BSP_U32)(pAllocInfo->allocList[cnt]);
    pAllocInfo->allocList[cnt] = (BSP_U32)HISI_PA_ADDRESS((unsigned long)pMem);

#ifdef __BSP_MEM_DEBUG__
    pAllocInfo->allocUsedInfoList[cnt].u32CurNum--;
    pAllocInfo->allocUsedInfoList[cnt].u32TotalFreeNum++;
    MEM_ITEM_STATUS(pMem) = MEM_FREE;
#endif
    /* Flush Cache */
    MEM_FLUSH_CACHE_BY_TYPE(MEM_GET_ALLOC_ADDR(pMem), MEM_MGR_SIZE_FOR_CACHE, u32PoolType);

    MEM_UNLOCK_BY_TYPE(u32PoolType, flag);
    return;
}

int memUsrInitFunction(void)
{

    printk("===== beg mem usr function =====\n");

    memset((void *)HISI_VA_ADDRESS(ECS_NV_BASE_ADDR), 0, 16);

    return 0;
}
/**************************************************************************
  接口实现
**************************************************************************/
BSP_S32 BSP_MEM_Init(BSP_VOID)
{
    BSP_U32 u32PoolType;
    BSP_U32 u32MaxInitNum = 0;
    BSP_S32 ret = 0;
    BSP_U32 state = 0;

    sg_pAllocSizeTbl = (BSP_U32*)(HISI_VA_ADDRESS(MEMORY_AXI_MEMMGR_FLAG_ADDR) + MEM_CTX_RESERVED);

    sg_pIccAllocInfo = (MEM_ALLOC_INFO*)((unsigned long)sg_pAllocSizeTbl + sizeof(sg_AllocListSize));

    /* 如果查找表已经赋值说明 ICC 的部分已经初始化 */
    ret = mdrv_sync_lock(SYNC_MODULE_MEM, &state, 10000);
    if(ret)
    {
        printk("BSP_MEM_Init sync lock timeout ret=0x%x, line:%d\n", ret, __LINE__);
    }

    if(0 == state)
    {
        MEM_SPIN_INIT();
        memset((VOID*)sg_pIccAllocInfo, 0, (sizeof(MEM_ALLOC_INFO)*MEM_ICC_AXI_POOL));
        memcpy(sg_pAllocSizeTbl, sg_AllocListSize, sizeof(sg_AllocListSize));
        u32MaxInitNum = MEM_ICC_AXI_POOL;
         if (memUsrInitFunction() != OK)
        {
            printk("memUsrInitFunction call fail, line:%d\n", __LINE__);
        }
    }
    else
    {
        u32MaxInitNum = 1; /* 只初始化单核 DDR的部分 */
    }


    for (u32PoolType = (BSP_U32)MEM_NORM_DDR_POOL; u32PoolType < (BSP_U32)u32MaxInitNum; u32PoolType++)
    {
        /* 分配内存池 */
        if (OK != memInitPool(u32PoolType))
        {
            return ERROR;
        }

        (VOID)BSP_MEM_SetMostUsedSize(512, u32PoolType);
    }

    (BSP_VOID)mdrv_sync_unlock(SYNC_MODULE_MEM, 1);

    return OK;
}

BSP_S32 BSP_MEM_SetMostUsedSize(BSP_U32 u32Size, BSP_U32 u32PoolType)
{
    BSP_U32 u32Item;

    if (u32PoolType >= MEM_ICC_AXI_POOL)
    {
        printk("invalid type:%d, line:%d\n", u32PoolType, __LINE__);
        return ERROR;
    }

    for ((u32Item) = 0; (u32Item) < MEM_ALLOC_LIST_NUM && u32Size > MEM_GET_ALLOC_SIZE(u32Item); (u32Item)++)
        ;

    if (u32Item >= MEM_ALLOC_LIST_NUM)
    {
        printk("invalid size:%d, line:%d\n", u32Size, __LINE__);
        return ERROR;
    }

    /* 设置时要设置 MostItem - 1, 方便查找 */
    MEM_GET_ALLOC_INFO(u32PoolType)->mostUsedItem  = (u32Item == 0) ? (0) : (u32Item-1);

    return OK;
}

/*****************************************************************************
* 函 数 名  : BSP_Malloc
*
* 功能描述  : BSP 动态内存分配
*
* 输入参数  : u32Size: 分配的大小(byte)
*             enFlags: 内存属性(暂不使用,预留)
* 输出参数  : 无
* 返 回 值  : 分配出来的内存指针
*****************************************************************************/
BSP_VOID* BSP_Malloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags)
{
    BSP_U8 *pItem;

    if(MEM_NORM_DDR_POOL == enFlags)
    {
	    return kmalloc(u32Size, GFP_KERNEL);
    }

    /* 分配内存 */
    pItem = memAlloc(enFlags, u32Size);

    return (BSP_VOID*)pItem;
}
EXPORT_SYMBOL(BSP_Malloc);

/*****************************************************************************
* 函 数 名  : BSP_MallocDbg
*
* 功能描述  : BSP 动态内存分配(Debug接口)
*
* 输入参数  : u32Size: 分配的大小(byte)
*             enFlags: 内存属性(暂不使用,预留)
*             pFileName: 使用的源文件
*             u32Line:   所在文件的行号
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_VOID* BSP_MallocDbg(BSP_U32 u32Size, MEM_POOL_TYPE enFlags, BSP_U8* pFileName, BSP_U32 u32Line)
{
    BSP_U8 *pItem;

    /* 分配内存 */
    pItem = memAlloc(MEM_NORM_DDR_POOL, (BSP_U32)u32Size);

#ifdef __BSP_MEM_DEBUG__
    if (NULL != pItem)
    {
        /* Debug模式要更新MGR 信息 */
        MEM_ITEM_LINE(pItem) = u32Line;
        MEM_ITEM_FILE_NAME(pItem) = (BSP_U32)pFileName;
    }
#endif
    return (BSP_VOID*)pItem;
}

/*****************************************************************************
* 函 数 名  : BSP_Free
*
* 功能描述  : BSP 动态内存释放
*
* 输入参数  : pMem: 动态内存指针
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID  BSP_Free(BSP_VOID* pMem)
{
    BSP_U32 u32Size;
    BSP_U32 u32Flags;
    BSP_U32 u32MagicNumber;
    unsigned long flag = 0;

    MEM_SPIN_LOCK(flag);
    u32Size = MEM_ITEM_SIZE(pMem);
    u32Flags = MEM_ITEM_FLAGS(pMem);
    u32MagicNumber= MEM_ITEM_MAGIC(pMem);
    MEM_SPIN_UNLOCK(flag);

    if(MEM_MAGIC_NUM != u32MagicNumber)
    {
        kfree(pMem);
    }
    else if(MEM_NORM_DDR_POOL < u32Flags)
    {
        memFree(u32Flags, pMem, u32Size);
    }
    else
    {
        printk("BSP_Free failed: pMem is  0x%p, u32MagicNumber is 0x%x, u32Flags is 0x%x\n",pMem, u32MagicNumber, u32Flags);
    }
    return;
}
EXPORT_SYMBOL(BSP_Free);

/*****************************************************************************
* 函 数 名  : BSP_Free
*
* 功能描述  : BSP 动态内存释放(Debug接口)
*
* 输入参数  : pMem: 动态内存指针
*             pFileName: 使用的源文件
*             u32Line:   所在文件的行号
*
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID  BSP_FreeDbg(BSP_VOID* pMem, BSP_U8* pFileName, BSP_U32 u32Line)
{
#ifdef __BSP_MEM_DEBUG__
    /* 检查当前内存是否有效 */
    if (memPtrInvalid(pMem))
    {
        printk("invalid mem block, ptr:0x%p, line:%d\n", pMem, __LINE__);
        return;
    }

    if (MEM_FREE == MEM_ITEM_STATUS(pMem) ||
        MEM_NORM_DDR_POOL != MEM_ITEM_FLAGS(pMem))
    {
        printk("warning! ptr:0x%p, may free twice, or wrong mem flags line:%d\n", pMem, __LINE__);
        return;
    }
#endif

#ifdef __BSP_MEM_DEBUG__
    /* 更新MGR Debug信息 */
    MEM_ITEM_LINE(pMem) = u32Line;
    MEM_ITEM_FILE_NAME(pMem) = (BSP_U32)pFileName;
#endif

    BSP_Free(pMem);

    return;
}

/*****************************************************************************
* 函 数 名  : BSP_SMalloc
*
* 功能描述  : BSP 动态内存分配(加spin lock保护,多核场景使用)
*
* 输入参数  : u32Size: 分配的大小(byte)
*             enFlags: 内存属性(暂不使用,预留)
* 输出参数  : 无
* 返 回 值  : 分配出来的内存指针
*****************************************************************************/
BSP_VOID* BSP_SMalloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags)
{
    BSP_U8 *pItem;

    /* 分配内存 */
    pItem = memAlloc((BSP_U32)enFlags, u32Size);

    return (BSP_VOID*)pItem;
}
EXPORT_SYMBOL(BSP_SMalloc);

/*****************************************************************************
* 函 数 名  : BSP_SMallocDbg
*
* 功能描述  : BSP 动态内存分配(加spin lock保护,多核场景使用)(Debug接口)
*
* 输入参数  : u32Size: 分配的大小(byte)
*             enFlags: 内存属性(暂不使用,预留)
*             pFileName: 使用的源文件
*             u32Line:   所在文件的行号
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_VOID* BSP_SMallocDbg(BSP_U32 u32Size, MEM_POOL_TYPE enFlags, BSP_U8* pFileName, BSP_U32 u32Line)
{
    BSP_U8 *pItem;

#ifdef __BSP_MEM_DEBUG__
    if ((BSP_U32)enFlags >= MEM_ICC_AXI_POOL)
    {
        printk("invalid mem enFlags:%d, line:%d\n", (BSP_U32)enFlags, __LINE__);
        return NULL;
    }
#endif

    /* 分配内存 */
    pItem = memAlloc((BSP_U32)enFlags, u32Size);

    return (BSP_VOID*)pItem;
}

/*****************************************************************************
* 函 数 名  : BSP_SFree
*
* 功能描述  : BSP 动态内存释放(加spin lock保护,多核场景使用)
*
* 输入参数  : pMem: 动态内存指针
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID  BSP_SFree(BSP_VOID* pMem)
{
    BSP_U32 u32Size;
    BSP_U32 u32Flags;


    /* Invalid Cache */
    MEM_INVALID_CACHE(MEM_GET_ALLOC_ADDR(pMem), MEM_MGR_SIZE_FOR_CACHE);

#ifdef __BSP_MEM_DEBUG__
    /* 检查当前内存是否有效 */
    if (memPtrInvalid(pMem)                     ||
        MEM_FREE == MEM_ITEM_STATUS(pMem)       ||
        MEM_ITEM_FLAGS(pMem) == MEM_NORM_DDR_POOL)
    {
        printk("warning! ptr:0x%p, invalid mem block, or may free twice, or wrong mem flags line:%d\n", pMem, __LINE__);
        return;
    }
#endif

    u32Size = MEM_ITEM_SIZE(pMem);
    u32Flags = MEM_ITEM_FLAGS(pMem);

    memFree(u32Flags, pMem, u32Size);
    return;
}
EXPORT_SYMBOL(BSP_SFree);

/*****************************************************************************
* 函 数 名  : BSP_SFree
*
* 功能描述  : BSP 动态内存释放(加spin lock保护,多核场景使用)(Debug接口)
*
* 输入参数  : pMem: 动态内存指针
*             pFileName: 使用的源文件
*             u32Line:   所在文件的行号
*
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID  BSP_SFreeDbg(BSP_VOID* pMem, BSP_U8* pFileName, BSP_U32 u32Line)
{
    BSP_SFree(pMem);

    return;
}

/**************************************************************************
  调试信息实现
**************************************************************************/
static const char* g_memPoolName[] =
{
    "MEM_NORM_DDR_POOL",
    "MEM_ICC_DDR_POOL",
    "MEM_ICC_AXI_POOL",
    "MEM_POOL_MAX"
};

BSP_S32 BSP_MEM_Info(BSP_U32 u32MemType)
{
    BSP_U32 u32Item;
    MEM_ALLOC_INFO* pAllocInfo;

    if (u32MemType >= MEM_ICC_AXI_POOL)
    {
        printk("invalid pool type:%d\n", u32MemType);
        return ERROR;
    }

    for (u32MemType = MEM_NORM_DDR_POOL; u32MemType < MEM_ICC_AXI_POOL; u32MemType++)
    {
        pAllocInfo = MEM_GET_ALLOC_INFO(u32MemType);
        printk("Dump Mem (%s):\n", g_memPoolName[u32MemType]);
        printk("================================\n");
        printk("mem pool info:\n");
        printk("--------------------------------\n");
        printk("Base Addr:          0x%x\n", pAllocInfo->memPoolInfo.u32BaseAddr);
        printk("Total Size:         %d(0x%x)\n", pAllocInfo->memPoolInfo.u32Size, pAllocInfo->memPoolInfo.u32Size);
        printk("Left Size:          %d(0x%x)\n", pAllocInfo->memPoolInfo.u32Left, pAllocInfo->memPoolInfo.u32Left);
        printk("Cur Pos Addr:       0x%x\n", pAllocInfo->memPoolInfo.u32CurPosAddr);
        printk("Mgr Size:           %d\n", pAllocInfo->memPoolInfo.u32MgrSize);
        printk("Most Used Item:     %d\n", pAllocInfo->mostUsedItem);

        printk("\n");
        printk("================================\n");
        printk("mem list used info:\n");
        printk("--------------------------------\n");
        for (u32Item = 0; u32Item < MEM_ALLOC_LIST_NUM; u32Item++)
        {
            printk("+-- dump size:%d list info:\n", MEM_GET_ALLOC_SIZE(u32Item));
            printk("|-- cur alloc num:  %d\n", pAllocInfo->allocUsedInfoList[u32Item].u32CurNum);
            printk("|-- max alloc num:  %d\n", pAllocInfo->allocUsedInfoList[u32Item].u32MaxNum);
            printk("|-- total alloc num:%d\n", pAllocInfo->allocUsedInfoList[u32Item].u32TotalMallocNum);
            printk("|-- total free num: %d\n", pAllocInfo->allocUsedInfoList[u32Item].u32TotalFreeNum);
            printk("+-- \n");
        }
        printk("================================\n");
    }
    return 0;
}

BSP_S32 BSP_MEM_DumpBlock(BSP_U32 u32Addr)
{
    BSP_BOOL bBlockInvalid = FALSE;

    if (!u32Addr)
    {
        printk("can't dump the block, the input ptr is NULL\n");
        return ERROR;
    }

    if (MEM_MAGIC_NUM != MEM_ITEM_MAGIC(u32Addr))
    {
        bBlockInvalid = TRUE;
        printk("The Block:0x%x may Invalid!\n", u32Addr);
    }

    /* 为确保正确,统一刷下Cache */
    MEM_INVALID_CACHE(MEM_GET_ALLOC_ADDR(u32Addr), MEM_MGR_SIZE_FOR_CACHE);

    printk("================================\n");
    printk("dump mem block info:\n");
    printk("================================\n");
    printk("usr ptr:            0x%x\n",    u32Addr);
    printk("alloc ptr:          0x%x\n",    (BSP_U32)MEM_GET_ALLOC_ADDR(u32Addr));
    printk("magic num:          0x%x\n",    MEM_ITEM_MAGIC(u32Addr));
    printk("block size:         %d\n",      MEM_ITEM_SIZE(u32Addr));
    printk("mem flags:          0x%x\n",    MEM_ITEM_FLAGS(u32Addr));
    printk("mem item next:      0x%x\n",    MEM_ITEM_NEXT(u32Addr));
#ifdef __BSP_MEM_DEBUG__
    printk("used file name:     %s\n",      (bBlockInvalid) ? ("invalid") : (BSP_CHAR*)MEM_ITEM_FILE_NAME(u32Addr));
    printk("used line:          %d\n",      MEM_ITEM_LINE(u32Addr));
    printk("mem status:         %s\n",      (MEM_ITEM_STATUS(u32Addr)) ? ("ALLOC") : ("FREE"));
#endif
    printk("================================\n");
    return 0;
}

BSP_S32 BSP_MEM_DumpAllocList(BSP_U32 u32Size, BSP_U32 u32PoolType)
{
    BSP_U32 u32Item;
    BSP_U32 u32MostUsedItem;
    VOID* pCurAddr;
    BSP_U32 u32Num = 0;
    MEM_ALLOC_INFO* pAllocInfo;

    if (u32PoolType >= MEM_ICC_AXI_POOL)
    {
        printk("invalid pool type:%d\n", u32PoolType);
        return ERROR;
    }
    pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);
    u32MostUsedItem = pAllocInfo->mostUsedItem;
    MEM_FIND_RIGHT_ITEM(u32Item, u32Size, u32MostUsedItem);

    if (u32Item >= MEM_ALLOC_LIST_NUM)
    {
        printk("invalid size:%d\n", u32Size);
        return -1;
    }

    printk("dump alloc list beg:\n");

    pCurAddr = (VOID*)HISI_VA_ADDRESS((unsigned long)pAllocInfo->allocList[u32Item]);/*lint124,modify by z00212992,2012.4.17*/
    for (;NULL != pCurAddr; pCurAddr = (VOID*)MEM_ITEM_NEXT(pCurAddr))/*lint124,modify by z00212992,2012.4.17*/
    {
        (VOID)BSP_MEM_DumpBlock((BSP_U32)pCurAddr);
        u32Num++;
    }

    printk("dump alloc list end, num:%d\n", u32Num);
    return 0;
}

BSP_S32 BSP_MEM_Scan(BSP_U32 u32PoolType)
{
    MEM_ALLOC_INFO* pAllocInfo = NULL;
    BSP_U32 u32CurScan;
    BSP_U32 u32EndAddr;
    BSP_U32 u32MgrSize;

    pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);
    if (u32PoolType >= MEM_ICC_AXI_POOL)
    {
        printk("invalid pool type:%d\n", u32PoolType);
        return ERROR;
    }
    u32CurScan = pAllocInfo->memPoolInfo.u32BaseAddr;
    u32MgrSize = pAllocInfo->memPoolInfo.u32MgrSize;
    u32EndAddr = pAllocInfo->memPoolInfo.u32CurPosAddr;

    printk("Beg Scan Mem (%s):\n", g_memPoolName[u32PoolType]);
    printk("================================\n");
    printk("mem pool info:\n");
    printk("--------------------------------\n");
    printk("Base Addr:          0x%x\n", pAllocInfo->memPoolInfo.u32BaseAddr);
    printk("Total Size:         %d(0x%x)\n", pAllocInfo->memPoolInfo.u32Size, pAllocInfo->memPoolInfo.u32Size);
    printk("Left Size:          %d(0x%x)\n", pAllocInfo->memPoolInfo.u32Left, pAllocInfo->memPoolInfo.u32Left);
    printk("Cur Pos Addr:       0x%x\n", pAllocInfo->memPoolInfo.u32CurPosAddr);
    printk("Mgr Size:           %d\n", pAllocInfo->memPoolInfo.u32MgrSize);
    printk("Most Used Item:     %d\n", pAllocInfo->mostUsedItem);
    printk("\n");

    /* 扫描 MemPool */
    while(u32CurScan < u32EndAddr)
    {
        /* 先拿到usr ptr */
        u32CurScan += u32MgrSize;

        /* 打印当前内存块信息 */
        (VOID)BSP_MEM_DumpBlock(u32CurScan);

        /* 移动到下一个内存块 */
        u32CurScan += MEM_ITEM_SIZE(u32CurScan);

        printk("\npress \'Enter\' to continue, press \'q\' to stop scan\n");
    }

    return OK;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */



