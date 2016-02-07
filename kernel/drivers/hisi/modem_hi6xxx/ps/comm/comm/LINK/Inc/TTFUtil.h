/******************************************************************************
  Copyright   : 2005-2007, Huawei Tech. Co., Ltd.
  File name   : TTFUtil.h
  Author      : g45205
  Version     : V100R001
  Date        : 2005-04-19
  Description : 该头文件定义了TTF组提供的队列操作和结构
  History     :
  1. Date:2005-04-19
     Author: g45205
     Modification:Create

  2.日    期   : 2007年6月1日
    作    者   : zengfei 57034
    修改内容   : 问题单A32D10791: TTF_ZC_BLK前加入内存块状态信息
******************************************************************************/

#ifndef __TTF_UTIL_H__
#define __TTF_UTIL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "vos.h"


/******************************************************************************
  2 宏定义
******************************************************************************/
#define TTF_Q_CNT(pQ)           ((pQ)->ulCnt)

#define TTF_MBUF_REMOVE_FROM_HDR(pMbuf, usNum)  \
{                                               \
    (pMbuf)->pData  += (usNum);                 \
    (pMbuf)->usLen  -= (usNum);                 \
}

#define TTF_MBUF_RESUME_TO_HDR(pMbuf, usNum)    \
{                                               \
    (pMbuf)->pData  -= (usNum);                 \
    (pMbuf)->usLen  += (usNum);                 \
}

#define TTF_MAX_IPV4_ADDR_LEN   15

#define TTF_REG_MAX_BIT         (31)

#define TTF_COPYLINK_UL_MAX_NUM    (100)
#define TTF_COPYLINK_DL_MAX_NUM    (250)

/* TTF SYS ERR ID */
#define TTF_MEM_RB_TASK_FAIL_SEM_ERR    (0x30000002)
#define GEA_SYSTEM_ERROR_ID             (0x30000003)
#define WTTF_CICOM_SYSTEM_ERROR_ID      (0x30000004)
#define HDLC_DEF_SYSTEM_ERROR_ID        (0x30000005)
#define HDLC_FRM_SYSTEM_ERROR_ID        (0x30000006)
#define TTF_MEM_ALLOC_FAIL_SYSTEM_ERROR (0x30000007)


#if (VOS_OS_VER == VOS_WIN32) || defined(_lint)

#define TTF_FORCE_ARM_INSTUCTION()

#else
/* 强制ARM执行前面所有指令，解决ARM指令乱序问题
   使用OM接口自适配L1,L2 Cache打开，需要flush buffer的问题 */
#define TTF_FORCE_ARM_INSTUCTION()      VOS_FlushCpuWriteBuf()
#endif

/******************************************************************************
  3 枚举定义
******************************************************************************/


/******************************************************************************
  4 全局变量声明
******************************************************************************/


/******************************************************************************
  5 消息头定义
******************************************************************************/


/******************************************************************************
  6 消息定义
******************************************************************************/


/******************************************************************************
  7 STRUCT定义
******************************************************************************/
#pragma pack(4)

/*================================*/        /* TTF静态队列管理定义 */
typedef struct _TTF_Q_LINK_ST
{
    struct _TTF_Q_LINK_ST  *pNext;
} TTF_Q_LINK_ST;                            /* TTF队列节点定义*/

typedef struct _TTF_Q_HDR_LINK_ST
{
    TTF_Q_LINK_ST          *pHead;          /* 队列头指针 */
    TTF_Q_LINK_ST          *pTail;          /* 队列尾指针 */
} TTF_Q_HDR_LINK_ST;

typedef struct
{
    TTF_Q_HDR_LINK_ST       stHdr;
    VOS_UINT32              ulCnt;
} TTF_Q_ST;


/*================================*/        /* TTF内存管理定义 */
/*============================*/            /* TTF内存单元定义 */
typedef struct _TTF_MBUF_ST
{
    TTF_Q_LINK_ST           stLink;         /* 队列节点 */
    struct _TTF_MBUF_ST    *pNext;          /*该数据的下一段*/
    VOS_UINT16              usLen;          /*数据的总长度，但不一定全部被使用*/
    VOS_UINT16              usUsed;         /*已经使用的数据长度*/
    VOS_UINT8              *pData;          /*存放数据的指针，物理上指向结构体的后续内存*/
} TTF_MBUF_ST;

/*TTF优先级结构定义*/
typedef struct
{
    VOS_UINT16              usMaxCnt;
    VOS_UINT16              usHighCnt;
    VOS_UINT16              usMidCnt;
    VOS_UINT16              usLowCnt;
    TTF_Q_ST                stQ;
} TTF_STREAM_ST;


/*****************************************************************************
 结构名    : TTF_COPYLINK_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 链式拷贝数据结构
*****************************************************************************/
typedef struct _TTF_COPYLINK_NODE_STRU
{
    VOS_UINT8                          *pucSrcData;
    VOS_UINT8                          *pucDstData;
    VOS_UINT32                          ulDataLen;
} TTF_COPYLINK_NODE_STRU;

typedef struct
{
    TTF_COPYLINK_NODE_STRU              astCopyLinkNodes[TTF_COPYLINK_DL_MAX_NUM];
    VOS_UINT32                          ulCopyNodeCnt;
}TTF_COPYLINK_DL_STRU;

typedef struct
{
    TTF_COPYLINK_NODE_STRU              astCopyLinkNodes[TTF_COPYLINK_UL_MAX_NUM];
    VOS_UINT32                          ulCopyNodeCnt;
}TTF_COPYLINK_UL_STRU;



#define    TTF_CPY_LINK_REG_DL(ulPid, pucRegDataDst, pucRegDataSrc, usRegDataLen)        TTF_CopyLink_DL_Reg(ulPid, pucRegDataDst, pucRegDataSrc, usRegDataLen, &g_stTtfCopyLinkDL)
#define    TTF_CPY_LINK_REG_UL(ulPid, pucRegDataDst, pucRegDataSrc, usRegDataLen)        TTF_CopyLink_UL_Reg(ulPid, pucRegDataDst, pucRegDataSrc, usRegDataLen, &g_stTtfCopyLinkUL)

#define    TTF_CPY_LINK_EXECUTE_DL(ulPid)                                                TTF_CopyLink_DL_Execute(ulPid, &g_stTtfCopyLinkDL)
#define    TTF_CPY_LINK_EXECUTE_UL(ulPid)                                                TTF_CopyLink_UL_Execute(ulPid, &g_stTtfCopyLinkUL)

#define    TTF_CPY_LINK_DEL_DL()                                                        TTF_CopyLink_DL_Rel(&g_stTtfCopyLinkDL)
#define    TTF_CPY_LINK_DEL_UL()                                                        TTF_CopyLink_UL_Rel(&g_stTtfCopyLinkUL)

#define    TTF_CPY_LINK_INIT()                                                           TTF_CopyLink_DL_Init(&g_stTtfCopyLinkDL); \
                                                                                         TTF_CopyLink_UL_Init(&g_stTtfCopyLinkUL)

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

/******************************************************************************
  8 UNION定义
******************************************************************************/


/******************************************************************************
  9 OTHERS定义
******************************************************************************/
#ifndef PCLINT_MEM
/*申请指定长度的内存单元，数据附在TTF_BLK_ST结构之后*/
extern TTF_MBUF_ST *TTF_MbufNew(VOS_UINT32 ulPid, VOS_UINT16 usLen);

/*释放指定TTF_MBUF_ST结构的内存*/
extern VOS_VOID     TTF_MbufFree(VOS_UINT32 ulPid, TTF_MBUF_ST *pMem);

extern VOS_UINT32   TTF_QIn(VOS_UINT32 ulPid, TTF_Q_ST *pQ, TTF_Q_LINK_ST *pLink);
extern VOS_VOID    *TTF_QOut(VOS_UINT32 ulPid, TTF_Q_ST *pQ);
#endif


extern VOS_UINT16   TTF_MbufGetLen(VOS_UINT32 ulPid, TTF_MBUF_ST *pMem);
extern VOS_VOID     TTF_QLink(VOS_UINT32 ulPid, VOS_VOID *pItem, TTF_Q_LINK_ST *pLink);
extern VOS_VOID     TTF_QInit(VOS_UINT32 ulPid, TTF_Q_ST *pQ);
extern VOS_UINT32   TTF_QIsEmpty(VOS_UINT32 ulPid, TTF_Q_ST *pQ);
extern VOS_UINT32   TTF_QCnt(VOS_UINT32 ulPid, TTF_Q_ST *pQ);
extern VOS_UINT32   TTF_LenStr2IpAddr(VOS_UINT8* pucStr, VOS_UINT8 *pucIpAddr);

extern VOS_VOID     TTF_Write32RegByBit(VOS_UINT32 ulRegAddr, VOS_UINT8 ucStartBit, VOS_UINT8 ucEndBit,VOS_UINT32 ulSetValue);
extern VOS_UINT32   TTF_Read32RegByBit(VOS_UINT32 ulRegAddr, VOS_UINT8 ucStartBit, VOS_UINT8 ucEndBit);

extern VOS_UINT32 TTF_CopyLink_DL_Init(TTF_COPYLINK_DL_STRU *pstTtfCpyLink);
extern VOS_UINT32 TTF_CopyLink_DL_Reg
(
    VOS_UINT32                          ulPid,
    VOS_UINT8                          *pucRegDataDst,
    VOS_UINT8                          *pucRegDataSrc,
    VOS_UINT16                          usRegDataLen,
    TTF_COPYLINK_DL_STRU                  *pstTtfCpyLink
);
extern VOS_UINT32 TTF_CopyLink_DL_Execute(VOS_UINT32 ulPid, TTF_COPYLINK_DL_STRU *pstTtfCpyLink);
extern VOS_VOID  TTF_CopyLink_DL_Rel(TTF_COPYLINK_DL_STRU *pstTtfCpyLink );

extern VOS_UINT32 TTF_CopyLink_UL_Init(TTF_COPYLINK_UL_STRU *pstTtfCpyLink);
extern VOS_UINT32 TTF_CopyLink_UL_Reg
(
    VOS_UINT32                          ulPid,
    VOS_UINT8                          *pucRegDataDst,
    VOS_UINT8                          *pucRegDataSrc,
    VOS_UINT16                          usRegDataLen,
    TTF_COPYLINK_UL_STRU                  *pstTtfCpyLink
);
extern VOS_UINT32 TTF_CopyLink_UL_Execute(VOS_UINT32 ulPid, TTF_COPYLINK_UL_STRU *pstTtfCpyLink);
extern VOS_VOID  TTF_CopyLink_UL_Rel(TTF_COPYLINK_UL_STRU *pstTtfCpyLink );


extern VOS_VOID TTF_InsertSortAsc16bit(VOS_UINT32 ulPid, VOS_UINT16    ausSortElement[], VOS_UINT32 ulElementCnt, VOS_UINT32 ulMaxCnt);
extern VOS_VOID TTF_RemoveDupElement16bit(VOS_UINT32 ulPid, VOS_UINT16    ausSortElement[], VOS_UINT32 *pulElementCnt, VOS_UINT32 ulMaxCnt);
extern VOS_VOID TTF_RingBufWrite(VOS_UINT32 ulPid, VOS_UINT32 ulDstRingBufBaseAddr,
    VOS_UINT16 usOffset, VOS_UINT8 *pucSrcData, VOS_UINT16 usDataLen, VOS_UINT16 usModLen);


extern TTF_COPYLINK_DL_STRU     g_stTtfCopyLinkDL;
extern TTF_COPYLINK_UL_STRU     g_stTtfCopyLinkUL;

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* TTFUtil.h */

