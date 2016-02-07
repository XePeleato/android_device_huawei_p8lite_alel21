/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : LUPQueue.h
  Description     : TTFQueue.c header file，提供环形队列操作
  History         :
     1.hujianbo 42180      2009-03-10  Draft Enact
     2.
******************************************************************************/

#ifndef __LUPQUEUE_H__
#define __LUPQUEUE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "vos.h"
#include "PsTypeDef.h"


#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif


/*****************************************************************************
  2 macro
*****************************************************************************/


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/

/*****************************************************************************
   5 STRUCT
*****************************************************************************/
typedef struct LUP_QUEUE
{
    VOS_UINT32          ulHead;
    VOS_UINT32          ulTail;
    VOS_UINT32          ulMaxNum;
    VOS_UINT32          ulResv;/*为了与64位OS兼容*/
    VOS_VOID          **pBuff;
}LUP_QUEUE_STRU;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
  说明:
  1、队列用于存放指针
  2、出于通用性考虑，以下函数内存没有互斥操作，
     如果队列用于任务间操作，则由调用者完成互斥操作
*****************************************************************************/
extern VOS_UINT32 LUP_IsQueEmpty(const LUP_QUEUE_STRU *pstQue);
extern VOS_UINT32 LUP_IsQueFull(const LUP_QUEUE_STRU *pstQue);
extern VOS_UINT32 LUP_PeekQueHead(const LUP_QUEUE_STRU *pstQue, VOS_VOID **ppNode);
extern VOS_UINT32 LUP_EnQue(LUP_QUEUE_STRU *pstQue, VOS_VOID *pNode);
extern VOS_UINT32 LUP_DeQue(LUP_QUEUE_STRU *pstQue, VOS_VOID **ppNode);
extern VOS_UINT32 LUP_BatchDeQue(LUP_QUEUE_STRU *pstQue, VOS_VOID **ppBuf,VOS_UINT32 *pulNum);

extern VOS_UINT32 LUP_CreateQue(VOS_UINT32 ulPid, LUP_QUEUE_STRU **ppQue,
                                VOS_UINT32 ulMaxNodeNum);
extern VOS_UINT32 LUP_DestroyQue(VOS_UINT32 ulPid, LUP_QUEUE_STRU **ppQue);
extern VOS_UINT32 LUP_EnQuetoHead(LUP_QUEUE_STRU *pstQue, VOS_VOID *pNode, VOS_VOID** ppTailNode);
extern VOS_UINT32 LUP_QueCnt(const LUP_QUEUE_STRU *pstQue);


/*****************************************************************************
  9 OTHERS
*****************************************************************************/




#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif





#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of LUPQueue.h */
