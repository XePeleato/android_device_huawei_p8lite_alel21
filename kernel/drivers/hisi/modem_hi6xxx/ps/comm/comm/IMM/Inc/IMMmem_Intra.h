

#ifndef __IMM_MEM_INTRA_H__
#define __IMM_MEM_INTRA_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"

#include "ImmInterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif




#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* IMM 内存池可能的最大长度 */
#define IMM_MEM_BLK_MAX_BYTE_LEN        (1953)

/* IMM 内存池可能的最多档位 */
#define IMM_MEM_BLK_MAX_CLUSTER_NUM     (8)

/* IMM 控制头添加内存指针 */
#define IMM_ADD_CTL_HEAD_MEM(pstImmHead, pMem)          ((pstImmHead)->p_Imm_Ttf_Mem = (pMem))

#define IMM_GET_4BYTE_ALIGN_VALUE(ulSize)   ( ((ulSize) + 0x03) & (~0x03) )

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
#define IMM_DEBUG_TRACE_FUNC            FEATURE_OFF
extern unsigned char                    g_ucImmMemDebugPrint;


#ifdef _lint

#define IMM_DEBUG_TRACE_FUNC_ENTER()
#define IMM_DEBUG_TRACE_FUNC_LEAVE()

#else

#if(IMM_DEBUG_TRACE_FUNC == FEATURE_ON)

#ifdef __UT_CENTER__
#define __func__ __FUNCTION__
#endif

/* 调试打印 */
/*
#define IMM_DEBUG_PRINT(...)\
             do{\
                 if ( PS_TRUE == g_ucMemDebugPrint )\
                 {\
                     vos_printf(__VA_ARGS__);
                 }\
             }while(0)
*/


#define IMM_DEBUG_TRACE_FUNC_ENTER()\
                do{\
                    if ( IMM_TRUE == g_ucImmMemDebugPrint )\
                    {\
                        printk("Enter Function: ");\
                        printk(__func__);\
                        printk("\n");\
                    }\
                }while(0)

#define IMM_DEBUG_TRACE_FUNC_LEAVE()\
                do{\
                    if ( IMM_TRUE == g_ucImmMemDebugPrint )\
                    {\
                        printk("Normal Leave Function: ");\
                        printk(__func__);\
                        printk("\n");\
                    }\
                }while(0)
#else
#define IMM_DEBUG_TRACE_FUNC_ENTER()
#define IMM_DEBUG_TRACE_FUNC_LEAVE()
#endif
#endif


enum IMM_MEM_USED_INFO_TRIG_TYPE_ENUM
{
    IMM_MEM_TRIG_TYPE_ALERT                      = 0x0,     /*内存超门限告警*/
    IMM_MEM_TRIG_TYPE_ALLOC_FAIL,                           /*内存申请失败*/
    IMM_MEM_TRIG_TYPE_LEAK,                                 /*检查到有内存泄露*/
    IMM_MEM_TRIG_TYPE_OVERTIME_OCCUPANCY,                   /* 检测到有内存被超长时间使用 */
    IMM_MEM_TRIG_TYPE_BUTT
};
typedef unsigned char IMM_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


enum IMM_MEM_TYPE_ID_ENUM
{
    IMM_MEM_TYPE_ID_SHARE = 0,              /* A核共享内存池 */
    IMM_MEM_TYPE_ID_EXT,                    /* 来自C核的跨核内存 */

    IMM_MEM_TYPE_ID_BUTT
};
typedef unsigned char IMM_MEM_TYPE_ID_ENUM_UINT8;



enum IMM_INVALID_MEM_TYPE_ENUM
{
    IMM_INVALID_MEM_TYPE_MAGICNUM = 1,              /* 内存魔术字错误 */
    IMM_INVALID_MEM_TYPE_POOLID,                    /* 内存池ID非法  */
    IMM_INVALID_MEM_TYPE_CTRLADDR,                  /* 内存地址非法 */
    IMM_INVALID_MEM_TYPE_STUTAS,                    /* 内存状态错误 */

    IMM_INVALID_MEM_TYPE_BUTT
};
typedef unsigned char IMM_INVALID_MEM_TYPE_ENUM_UINT8;


typedef struct
{
    /*本级数的IMM_MEM_STRU总个数*/
    unsigned short                      usTotalCnt;

    /*本级数的IMM_MEM_STRU空闲个数*/
    unsigned short                      usFreeCnt;

    /*本级内存的内容长度*/
    unsigned short                      usLen;

    unsigned char                       aucRsv[2];

    /*记录IMM_MEM_ST可用内存索引表的栈顶元素，其内容为IMM_MEM_ST指针 */
    IMM_MEM_STRU                      **apstFreeStack;
}IMM_MEM_CLUSTER_STRU;


typedef void (*IMM_MEM_EXT_FREE_CALLBACK)(void *pAddr);
typedef void (*IMM_MEM_USED_INFO_EVENT_RPT_CALLBACK)( unsigned char  ucPoolId, IMM_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8 enTrigType );
typedef unsigned int (*IMM_OM_GET_SLICE_CALLBACK)(void);


typedef struct
{
    /* 标识本内存池是否使用 */
    unsigned char                       ucUsedFlag;

    /* 本内存池一共有多少级*/
    unsigned char                       ucClusterCnt;

    /* 本内存池中使用的最大用户字节 */
    unsigned short                      usMaxByteLen;

    /* 指向长度索引表的指针 */
    unsigned char                       aucLenIndexTable[IMM_MEM_BLK_MAX_BYTE_LEN];
    unsigned char                       aucRsv1[3];

    /* 指向级数索引表的指针 */
    IMM_MEM_CLUSTER_STRU                astClusterTable[IMM_MEM_BLK_MAX_CLUSTER_NUM];

    unsigned short                      usImmMemTotalCnt;       /* 本内存池一共有多少IMM_MEM_ST */
    unsigned short                      usImmMemUsedCnt;        /* 使用了多少IMM_MEM_ST */

    unsigned short                      usImmExcThresholdCnt;   /* 申请内存失败超过门限次数 */
    unsigned short                      usImmAllocFailCnt;      /* 最后一次过门限申请内存失败次数 */

    unsigned int                        ulImmMemMaxBlkCcpuUsedCnt; /* 最高档A核传给C核的库存量 */

    unsigned char                       aucRsv2[4];

    IMM_MEM_STRU                       *pstImmMemStStartAddr;   /* 指向ImmMemST首地址的指针 */
    IMM_MEM_EVENT_CALLBACK              pMemAllocEvent;
    IMM_MEM_EVENT_CALLBACK              pMemFreeEvent;
    IMM_MEM_EXT_FREE_CALLBACK           pMemExtFreeFunc;        /* 指向外部注册的释放函数*/
} IMM_MEM_POOL_STRU;



typedef struct
{
    unsigned short                          usSize;    /* 本级数的大小 */
    unsigned short                          usCnt;     /* 本级个数 */
}IMM_MEM_CLUSTER_CFG_INFO_STRU;



typedef struct
{
    IMM_MEM_POOL_ID_ENUM_UINT8          enPoolId;           /* 本池 ID */
    unsigned char                       ucClusterCnt;       /* 本内存池有多少级 */
    unsigned char                       aucRsv[6];

    IMM_MEM_CLUSTER_CFG_INFO_STRU      *pstClusterCfgInfo;  /* 每级的具体信息 */
} IMM_MEM_POOL_CFG_INFO_STRU;
typedef struct
{
    IMM_OM_GET_SLICE_CALLBACK                   pImmOmGetSliceFunc;   /* 获取CPU CLICE 注册函数指针 */
    IMM_MEM_USED_INFO_EVENT_RPT_CALLBACK        pImmMemEventRptFunc;  /* 上报内存事件注册函数指针 */
}IMM_MEM_REG_MNTN_FUNC_STRU;

/*****************************************************************************
 结构名    : IMM_MEM_FREE_NULL_PTR_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IMM MEM FREE收到空指针信息
*****************************************************************************/
typedef struct IMM_MEM_FREE_NULL_PTR_INFO
{
    unsigned int           ulImmMemFreeNullPtrCnt;     /* ImmMemFree 收到空指针次数 */
    unsigned short          usImmMemFreeFileId;         /* ImmMemFree 收到空指针次数文件ID，仅保存最后一次收到空指针文件ID信息 */
    unsigned short          usImmMemFreeLineNum;        /* ImmMemFree 收到空指针次数文件ID，仅保存最后一次收到空指针行号信息 */
}IMM_MEM_FREE_NULL_PTR_INFO_STRU;

/*****************************************************************************
 结构名    : IMM_MEM_FREE_INVALIED_MEM_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IMM MEM FREE 收到的非法IMM MEM 内存信息
*****************************************************************************/
typedef struct IMM_MEM_FREE_INVALIED_MEM_INFO
{
    unsigned int                           ulInvalidImmMemCnt;         /* IMM MEM FREE 收到的非法内存个数 */
    unsigned int                           ulInvalidImmMemAddr;        /* IMM MEM FREE 最后一次收到的非法IMM MEM 内存地址 */
    unsigned short                          usImmMemFreeFileId;         /* IMM MEM FREE 最后一次收到的非法IMM MEM 内存文件ID信息 */
    unsigned short                          usImmMemFreeLineNum;        /* IMM MEM FREE 最后一次收到的非法IMM MEM 内存行号信息 */
    IMM_INVALID_MEM_TYPE_ENUM_UINT8         ucImmMemInvalidType;        /* IMM MEM FREE 最后一次收到的非法IMM MEM 内存类型信息 */
    unsigned char                           aucReserve[1];              /* 对齐 */
    unsigned short                          usImmMemInvalidValue;       /* IMM MEM FREE 最后一次收到的非法IMM MEM 内存错误值 */
}IMM_MEM_FREE_INVALIED_MEM_INFO_STRU;

/*****************************************************************************
 结构名    : IMM_MEM_FREE_MNTN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 保存IMM 内存释放可维可测信息
*****************************************************************************/
typedef struct IMM_MEM_FREE_MNTN_INFO
{
    IMM_MEM_FREE_INVALIED_MEM_INFO_STRU astImmMemFreeInvalidMemInfo;    /* IMM MEM FREE 收到的非法IMM MEM 内存信息 */
    IMM_MEM_FREE_NULL_PTR_INFO_STRU     astImmMemFreeNullPtrInfo;       /* IMM MEM FREE 收到一级指针为空的信息 */
    IMM_MEM_FREE_NULL_PTR_INFO_STRU     astImmMemFreeNullPPtrInfo;      /* IMM MEM FREE 收到二级指针为空的信息 */
    unsigned int                       ulImmMemExtFreeFailCnt;         /* IMM MEM 释放外部内存失败次数 */
}IMM_MEM_FREE_MNTN_INFO_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern IMM_MEM_POOL_STRU                        g_astImmMemPool[IMM_MEM_POOL_ID_BUTT];
extern IMM_MEM_POOL_STRU * const                g_pstImmExtMemPool;
extern const IMM_MEM_CLUSTER_CFG_INFO_STRU      g_astImmMemCtrlPoolClusterTableInfo[];
extern const unsigned int * const              g_pulImmMemCtrlMemSuffix;
extern const unsigned int * const              g_paulImmMemCtrlMemSpace;
extern unsigned short                           g_usImmAllocFailCntThreshold;
extern unsigned short * const                   g_apusImmExcThresholdCnt[];
extern unsigned short * const                   g_apusImmAllocFailCnt[];
extern IMM_MEM_FREE_MNTN_INFO_STRU              g_stImmMemFreeMntnEntity;


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern void IMM_MemRegMntnFuncCallBack
(
    IMM_OM_GET_SLICE_CALLBACK   pImmOmGetSliceFunc,
    IMM_MEM_USED_INFO_EVENT_RPT_CALLBACK    pImmMemEventRptFunc
);

extern  unsigned int IMM_MemRegExtFreeCallBack
(
    IMM_MEM_EXT_FREE_CALLBACK           pMemExtFreeFunc
);

extern  IMM_MEM_POOL_STRU *IMM_MemPoolGet(unsigned char ucPoolId);

extern void IMM_MemFreeShowMntnInfo(void);

/* IMM MEM 获取内存池指针 */
#define IMM_MEM_GET_POOL(ucPoolId)      (&g_astImmMemPool[(ucPoolId)])

#define IMM_MEM_CTRL_HEAD_ALLOC(usFileID, usLineNum)\
        IMM_MemAlloc_Debug(usFileID, usLineNum, IMM_MEM_POOL_ID_EXT, 0 )


#pragma pack(0)



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of IMM.h */

