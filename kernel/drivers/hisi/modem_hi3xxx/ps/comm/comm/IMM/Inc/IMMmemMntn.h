

#ifndef __IMMMEMMNTN_H__
#define __IMMMEMMNTN_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "PsTypeDef.h"
#include "product_config.h"

#if ( FEATURE_ON == FEATURE_SKB_EXP )
#include "IMMmem.h"
#else
#include "IMMmem_Intra.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

#if (FEATURE_ON == FEATURE_SKB_EXP)
/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


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


enum IMM_MNTN_CHECK_POOL_LEAK_TYPE_ENUM
{
    IMM_MNTN_CHECK_POOL_LEAK_TYPE_USDCNT             = 0,
    IMM_MNTN_CHECK_POOL_LEAK_TYPE_OVERTIME,
    IMM_MNTN_CHECK_POOL_LEAK_TYPE_BUTT
};
typedef VOS_UINT8 IMM_MNTN_CHECK_POOL_LEAK_TYPE_ENUM_UINT8;


/*****************************************************************************
 枚举名    : IMM_OM_EVENT_ID_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 事件上报ID
*****************************************************************************/
enum IMM_OM_EVENT_ID_ENUM
{
    IMM_EVENT_BLK_MEM_USED_INFO             = 0X0101,   /* _H2ASN_MsgChoice TTF_OM_EVENT_BLK_MEM_USED_INFO_STRU */
    IMM_EVENT_BUTT
};
typedef VOS_UINT16 IMM_OM_EVENT_ID_ENUM_UINT16;

/*****************************************************************************
 结构名    : IMM_OM_EVENT_USED_BLK_MEM_INFO_STRU
 协议表格  :
 ASN.1描述 :
*****************************************************************************/
typedef struct
{
    VOS_UINT32                      ulAllocTick;        /* CPU tick when alloc or free */
    VOS_UINT16                      usAllocFileId;      /* File ID when alloc or free */
    VOS_UINT16                      usAllocLineNum;     /* File Line when alloc or free */
    VOS_UINT32                      ulTraceTick;        /* CPU tick when traced */
    VOS_UINT16                      usTraceFileId;      /* File ID when traced */
    VOS_UINT16                      usTraceLineNum;     /* File Line when traced */
    VOS_UINT8                       aucMemData[4];
}IMM_BLK_MEM_TRACE_INFO_STRU;


typedef struct
{
    VOS_UINT8                                   ucPoolId;
    VOS_UINT8                                   ucRes[3];
    IMM_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8      enTrigType;
    VOS_UINT8                                   ucNo;
    VOS_UINT16                                  usRptCnt;
    VOS_UINT32                                  ulTotalUsedCnt;
    IMM_BLK_MEM_TRACE_INFO_STRU                 astBlkMemTraceInfo[1];
}IMM_OM_EVENT_BLK_MEM_USED_INFO_STRU;


enum IMM_MEM_MSG_ID_ENUM
{
    ID_IMM_MEM_LEAK_INFO_IND            = 0x315,  /* _H2ASN_MsgChoice IMM_MEM_LEAK_INFO_IND_MSG */

    ID_IMM_MEM_LEAK_ENUM_BUTT
};
typedef VOS_UINT16  IMM_MEM_MSG_ID_ENUM_UINT16;
typedef struct
{
    VOS_MSG_HEADER                                /* 消息头 */        /* _H2ASN_Skip */
    IMM_MEM_MSG_ID_ENUM_UINT16          enMsgID;  /* 消息类型 */      /* _H2ASN_Skip */
    VOS_UINT8                           aucReserve[2];
}IMM_MEM_LEAK_INFO_IND_MSG;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID IMM_MntnInit( VOS_VOID );
extern VOS_VOID IMM_MntnRptPoolLeakInfo(VOS_VOID);


#endif





#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of IMMmemMntn.h */
