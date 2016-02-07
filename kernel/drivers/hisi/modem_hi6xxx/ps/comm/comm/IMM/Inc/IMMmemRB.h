

#ifndef __IMMMEMRB_H__
#define __IMMMEMRB_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "product_config.h"
#include "TTFLink.h"
#include "vos.h"
#include "VosPidDef.h"
#include "om.h"


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

/* IMM 封装的打印级别定义 */
#define IMM_PRINT_OFF        PS_PRINT_OFF
#define IMM_PRINT_ERROR      PS_PRINT_ERROR
#define IMM_PRINT_WARNING    PS_PRINT_WARNING
#define IMM_PRINT_NORMAL     PS_PRINT_NORMAL
#define IMM_PRINT_INFO       PS_PRINT_INFO
#define IMM_PRINT_BUTT       PS_PRINT_BUTT

#ifdef __UT_CENTER__

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#define IMM_LOG(ModulePID, Level, String)\
            OM_LOG((ModulePID), 0, (Level), (String))
#define IMM_LOG1(ModulePID, Level, String, Para1)\
            OM_LOG1((ModulePID), 0, (Level), (String), (Para1))
#define IMM_LOG2(ModulePID, Level, String, Para1, Para2)\
            OM_LOG2((ModulePID), 0, (Level), (String), (Para1), (Para2))
#define IMM_LOG3(ModulePID, Level, String, Para1, Para2, Para3)\
            OM_LOG3((ModulePID), 0, (Level), (String), (Para1), (Para2), (Para3))
#define IMM_LOG4(ModulePID, Level, String, Para1, Para2, Para3, Para4)\
            OM_LOG4((ModulePID), 0, (Level), (String), (Para1), (Para2), (Para3), (Para4))
#elif (OSA_CPU_CCPU == VOS_OSA_CPU)
#define IMM_LOG(ModulePID, Level, String)\
            PS_LOG((ModulePID), 0, (Level), (String))
#define IMM_LOG1(ModulePID, Level, String, Para1)\
            PS_LOG1((ModulePID), 0, (Level), (String), (Para1))
#define IMM_LOG2(ModulePID, Level, String, Para1, Para2)\
            PS_LOG2((ModulePID), 0, (Level), (String), (Para1), (Para2))
#define IMM_LOG3(ModulePID, Level, String, Para1, Para2, Para3)\
            PS_LOG3((ModulePID), 0, (Level), (String), (Para1), (Para2), (Para3))
#define IMM_LOG4(ModulePID, Level, String, Para1, Para2, Para3, Para4)\
            PS_LOG4((ModulePID), 0, (Level), (String), (Para1), (Para2), (Para3), (Para4))
#endif

#else

#define IMM_LOG(ModulePID, Level, String)\
            IMM_DebugLOG((String))
#define IMM_LOG1(ModulePID, Level, String, Para1)\
            IMM_DebugLOG1((String), (Para1))
#define IMM_LOG2(ModulePID, Level, String, Para1, Para2)\
            IMM_DebugLOG2((String), (Para1), (Para2))
#define IMM_LOG3(ModulePID, Level, String, Para1, Para2, Para3)\
            IMM_DebugLOG3((String), (Para1), (Para2), (Para3))
#define IMM_LOG4(ModulePID, Level, String, Para1, Para2, Para3, Para4)\
            IMM_DebugLOG4((String), (Para1), (Para2), (Para3), (Para4))

#endif

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

/*****************************************************************************
 结构名    : IMM_MEM_RB_MNTN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 核间可维可测信息
*****************************************************************************/
typedef struct IMM_MEM_RB_MNTN_INFO
{
    VOS_UINT32          ulImmMemRbSendIntCnt;       /* ImmMemRb 向IPF发送中断次数 */
    VOS_UINT32          ulImmMemRbReceiveIntCnt;    /* ImmMemRb 收到IPF中断次数 */
    VOS_UINT32          ulImmMemRbEnQFailCnt;       /* ImmMemRb 入队失败次数 */
    VOS_UINT32          ulImmMemRbDeQFailCnt;       /* ImmMemRb 出队失败次数 */
    VOS_UINT32          ulImmMemRbEnQTotalCnt;      /* ImmMemRb 入队总数 */
    VOS_UINT32          ulImmMemRbDeQTotalCnt;      /* ImmMemRb 出队总数 */
}IMM_MEM_RB_MNTN_INFO_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID IMM_RbRemoteFreeMem(VOS_VOID *pucAddr);
extern VOS_VOID IMM_DebugLOG(VOS_CHAR *pcString);
extern VOS_VOID IMM_DebugLOG1(VOS_CHAR *pcString, VOS_INT32 lPara1);
extern VOS_VOID IMM_DebugLOG2(VOS_CHAR *pcString, VOS_INT32 lPara1, VOS_INT32 lPara2);
extern VOS_VOID IMM_DebugLOG3(VOS_CHAR *pcString, VOS_INT32 lPara1, VOS_INT32 lPara2, VOS_INT32 lPara3);
extern VOS_VOID IMM_DebugLOG4(VOS_CHAR *pcString, VOS_INT32 lPara1, VOS_INT32 lPara2, VOS_INT32 lPara3, VOS_INT32 lPara4);
extern VOS_VOID IMM_RbMemFreeFromQue(VOS_VOID);

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

#endif /* end of IMMmemRB.h */
