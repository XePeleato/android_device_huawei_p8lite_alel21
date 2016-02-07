/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_buf_ctrl.h
  Description     : diag_buf_ctrl.c header file
  History         :
     1.w00182550       2012-11-20     Draft Enact
     2.
******************************************************************************/

#ifndef __DIAG_BUF_CTRL_H__
#define __DIAG_BUF_CTRL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"
#include  "mdrv.h"
#include  "SOCPInterface.h"


#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/

#if (VOS_LINUX== VOS_OS_VER)
#define DIAG_CODER_SRC_CNF_BUFFER_SIZE               (1024*64)
#define DIAG_CODER_SRC_IND_BUFFER_SIZE               (1024*64*3)
        /*ACPU诊断CNF编码源数据*/
#define SOCP_CODER_SRC_LOM_CNF SOCP_CODER_SRC_LOM_CNF1
#else
#define DIAG_CODER_SRC_CNF_BUFFER_SIZE               (1024*128)
#define DIAG_CODER_SRC_IND_BUFFER_SIZE               (1024*128*3)
        /*MCPU诊断CNF编码源数据*/
#define SOCP_CODER_SRC_LOM_CNF SOCP_CODER_SRC_LOM_CNF2
#endif

#if (VOS_LINUX== VOS_OS_VER)
        /*ACPU高优先级诊断编码源数据*/
#define SOCP_CODER_SRC_LOM_PRECEDENCE_IND SOCP_CODER_SRC_LOM_IND1
#else
        /*MCPU高优先级诊断编码源数据*/
#define SOCP_CODER_SRC_LOM_PRECEDENCE_IND SOCP_CODER_SRC_LOM_IND2
#endif

#if (VOS_LINUX== VOS_OS_VER)
        /*ACPU低优先级诊断编码源数据*/
#define SOCP_CODER_SRC_LOM_NORMAL_IND SOCP_CODER_SRC_LOM_IND1
#else
        /*MCPU低优先级诊断编码源数据*/
#define SOCP_CODER_SRC_LOM_NORMAL_IND SOCP_CODER_SRC_LOM_IND3
#endif
/*发起主动复位时MSP传入的参数ID最小值为:0xB000000*/
#define DIAG_CALLED_IN_IRQ          (0xB0000000)


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
typedef enum
{
	DIAG_CODER_SRC_TYPE_CNF = 0,
	DIAG_CODER_SRC_TYPE_IND,
	DIAG_CODER_SRC_TYPE_BUTT
}DIAG_CODER_SRC_TYPE_ENUM;

/*****************************************************************************
   5 STRUCT
*****************************************************************************/
/*****************************************************************************
 结构名    : DIAG_BUF_CTRL_STRU
 结构说明  : 包含DIAG缓冲区管理控制信息
*****************************************************************************/
typedef struct
{
    VOS_INT32   lAlloc;             /* 指向分配内存相对于首地址的偏移地址 */
    VOS_INT32   lRelease;           /* 指向释放内存相对于首地址的偏移地址 */
    VOS_INT32   lPadding;          /* 保存填充内存的大小 */
    VOS_INT32   lBufSize;          /* 保存缓冲区总大小 */
    VOS_UINT8   *pucBuf;         /* 指向缓冲区虚拟首地址 */
    VOS_UINT8   *pucRealBuf;     /* 指向缓冲区实首地址 */
}DIAG_BUF_CTRL_STRU;

/*****************************************************************************
结构名    : DIAG_TO_HSO_FAIL_STRU
结构说明  : OM模块异常发送可维可测信息记录结构
*****************************************************************************/
typedef struct
{
	VOS_UINT16                          usNoRdNum;           /* 记录无RD可清次数 */
    VOS_UINT16                          usSemErr;            /* 记录获取信号量失败的次数 */
    VOS_UINT16                          usRlsRdErr;          /* 记录获取已发送RD失败的次数 */
    VOS_UINT16                          usRlsMemErr;         /* 记录获取释放缓冲区内存失败的次数 */
    VOS_UINT16                          usBDLackErr;         /* 记录BD不足的次数 */
    VOS_UINT16                          usAllocMemErr;       /* 记录获取buf错误的次数 */
    VOS_UINT16                          usNoMemErr;          /* 记录内存不足的次数 */
    VOS_UINT16                          usSendSocpDataErr;   /* 记录发送数据到SOCP错误的次数 */
    VOS_UINT16                          usSendAddrErr;       /* 记录发送地址转换错误 */
    VOS_UINT16                          usRlsAddrErr;        /* 记录释放地址转换错误 */
    VOS_UINT16                          usSendInIsrErr;      /* 记录在中断中发送数据错误次数 */
}DIAG_TO_HSO_FAIL_STRU;
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
extern DIAG_BUF_CTRL_STRU                        g_stDiagBufCtrl[DIAG_CODER_SRC_TYPE_BUTT];

/*记录往PC侧发送数据时发生的各类错误*/
extern DIAG_TO_HSO_FAIL_STRU                      g_stDiagToHsoErrRecord;

#ifndef CHIP_BB_HI6210
extern VOS_UINT8* g_DiagMemVirt;
extern VOS_UINT g_DiagMemPhy;
extern VOS_UINT32 g_DiagMemSize;
#endif
/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/



extern VOS_UINT8* diag_AllocLogMem(DIAG_BUF_CTRL_STRU *pstBufCtrl, VOS_INT32 lLen);
extern VOS_UINT32 diag_BufCtrlGlobalInit(VOS_VOID);
extern VOS_UINT8* diag_BuffPhyToVirt(VOS_UINT8 *pucCurPhyAddr, VOS_UINT8 *pucPhyStart, VOS_UINT8 *pucVirtStart, VOS_UINT32 ulBufLen);
extern VOS_UINT8* diag_BuffVirtToPhy(VOS_UINT8 *pucCurVirtAddr, VOS_UINT8 *pucVirtStart, VOS_UINT8 *pucPhyStart, VOS_UINT32 ulBufLen);
extern VOS_UINT32 diag_CreateLogBuf(DIAG_BUF_CTRL_STRU *pstBufCtrl, VOS_INT32 lBufSize);
extern VOS_UINT8* diag_GetCoderSrcBuf(VOS_UINT32 ulLen,SOCP_CODER_SRC_ENUM_U32 ulChanID);
extern VOS_VOID diag_LockCoderSrcBuf(VOS_VOID);
extern VOS_UINT32 diag_ReleaseLogBuf(SOCP_CODER_SRC_ENUM_U32 ulSrcChanID);
extern VOS_UINT32 diag_ReleaseLogMem(DIAG_BUF_CTRL_STRU *pstBufCtrl, VOS_UINT8 *pAddr, VOS_INT32 lLen);
extern VOS_UINT32 diag_SendCoderSrcToSocp(VOS_UINT8 *pBuf,VOS_UINT32 ulLen, SOCP_CODER_SRC_ENUM_U32 ulCodeSrcId);
extern VOS_VOID diag_UnLockCoderSrcBuf(VOS_VOID);
extern VOS_UINT32 diag_GetBufIndex(SOCP_CODER_SRC_ENUM_U32 ulChanID);
VOS_VOID diag_BufHelp(VOS_VOID);
VOS_VOID diag_BufDebug(VOS_VOID);



#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_WIN32)||(VOS_OS_VER == VOS_RTOSCK))
#define DIAG_FLUSH_CACHE(ptr, size)
#define DIAG_INVALID_CACHE(ptr, size)
#elif (VOS_OS_VER == VOS_LINUX)
#define DIAG_FLUSH_CACHE(ptr, size)      SCM_FlushCpuCache(ptr, size)
#define DIAG_INVALID_CACHE(ptr, size)    SCM_InvalidCpuCache(ptr, size)
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

#endif /* end of diag_buf_ctrl.h */

