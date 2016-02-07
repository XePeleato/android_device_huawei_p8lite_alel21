/******************************************************************************

                版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : PsLogAdapterApp.h
  版 本 号   : 初稿
  作    者   : 甘兰 47350
  生成日期   : 2008年7月16日
  最近修改   :
  功能描述   : PsLogAdapterApp的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2008年7月16日
    作    者   : 甘兰 47350
    修改内容   : 创建文件

******************************************************************************/
#ifndef __PSLOGOSAA_H__
#define __PSLOGOSAA_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
/*#include <stdio.h> */
#include "pslog.h"
#include "omringbuffer.h"
#include "vos.h"
#include "omnvinterface.h"

#if (VOS_OS_VER == VOS_VXWORKS)
#include "mdrv.h"
#endif

#ifndef _lint

#if (VOS_OS_VER == VOS_VXWORKS)
#include <vxWorks.h>
#include <taskLib.h>
#include <semLib.h>
#include <String.h>
#include <tickLib.h>
#elif (VOS_OS_VER == VOS_WIN32)
#include <Windows.h>
#include <String.h>
#endif

#endif

/*****************************************************************************
  1 类型定义
*****************************************************************************/

typedef VOS_VOID          (*LOG_PFUN)(VOS_VOID);



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define LOG_TRUE                1
#define LOG_FALSE               0

#define LOG_OK                  0
#define LOG_ERR                 1

#define LOG_NULL_PTR            0

/*LOG支持的协议栈最大模块数目*/
#define LOG_PS_MODULE_MAX_NUM      (VOS_PID_BUTT - VOS_PID_DOPRAEND)
/*LOG支持的底软最大模块数目*/
#define LOG_DRV_MODULE_MAX_NUM   (LOG_MAX_MODULE_ID_ACPU_DRV- LOG_MIN_MODULE_ID_ACPU_DRV+1)
/*LOG支持模块ID的区间范围为(LOG_MODULE_MIN_ID, LOG_MODULE_MAX_ID]*/
#define LOG_MODULE_MIN_ID       VOS_PID_DOPRAEND
#define LOG_MODULE_MAX_ID       (LOG_MODULE_MIN_ID + LOG_PS_MODULE_MAX_NUM)

#define MAX_ERRLOG_BUF_LEN                  (4096)
#define ERRLOG_BUF_COUNT                    (2)
#define ERRLOG_ENABLE                       (1)
#define ERRLOG_DISABLE                      (0)
#define ERRLOG_WRITE_INTERVAL_DEFAULT_VALUE (3000)
#define ERRLOG_TOTAL_RECORD_TIME            (51840)
#define MNTN_ERRLOR0_ID                     (0)                             /*Errorlog0.bin 文件ID*/
#define MNTN_ERRLOR1_ID                     (1)                             /*Errorlog1.bin 文件ID*/

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
typedef enum
{
    LOG_FILE_1 = 0,
    LOG_FILE_2,
    LOG_FILE_BUTT
}LOG_NUM_ENUM;

/* LOG写入FLASH操作出错信息类型 */
typedef enum
{
    LOG_OPERATION_OK =0,
    LOG_OPERATION_CREATE_SEM_FAIL,
    LOG_OPERATION_POST_SEM_FAIL,
    LOG_OPERATION_MKDIR_FAIL,
    LOG_OPERATION_OPEN_FILE_FAIL,
    LOG_OPERATION_LSEEK_FILE_FAIL,
    LOG_OPERATION_WRITE_FILE_FAIL,
    LOG_OPERATION_WRITE_PROTECT,
    LOG_OPERATION_LENGTH_TOOBIG,
    LOG_OPERATION_ERROR_BUTT
}LOG_OPERATION_ERROR_ENUM;
/*****************************************************************************
  4 STRUCT定义
*****************************************************************************/
typedef struct
{
    LOG_MODULE_ID_EN        enModuleId;
    LOG_SUBMOD_ID_EN        enSubModId;
    LOG_LEVEL_EN            enPrintLev;
} LOG_ID_LEVEL_ST;

/*typedef struct
{
    LOG_MODULE_ID_EN        enMinModId;
    LOG_MODULE_ID_EN        enMaxModId;
} LOG_LEVEL_RANGE_ST;*/

typedef struct
{
    VOS_UINT32                   ulPrintSwitch;        /* 是否启动打印          */
    VOS_UINT32                   ulLogOutput;          /* 输出到串口、写入Flash */
    OM_RING_ID                  rngOmRbufId;          /* 环形缓存Id            */
    VOS_SEM                     semOmPrint;           /* 互斥信号量            */
} LOG_ENTITY_ST;
/*指示每一个模块的ID号和打印级别*/
typedef struct
{
    VOS_UINT32               ulModuleId;
    LOG_LEVEL_EN            enPrintLev;
}LOG_MODULE_LEVEL_STRU;
/*工具侧用来配置打印级别的结构*/
typedef struct
{
    VOS_UINT32                ulModuleNum;
    LOG_MODULE_LEVEL_STRU    astModuleLev[1];
}LOG_ID_LEVEL_STRU;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_INT32 Log_BufInput(VOS_CHAR *pcLogStr, VOS_UINT32 ulLen);
extern VOS_INT32 Log_GetPathOffset(VOS_CHAR* pcFileName);
extern VOS_UINT32 Log_GetTableIndex(LOG_MODULE_ID_EN enModuleId, LOG_SUBMOD_ID_EN enSubModId);
extern VOS_VOID Log_Output(VOS_UINT32 ulOutputType, VOS_CHAR *pcStr, VOS_UINT32 ulLen);
extern VOS_VOID Log_Init(VOS_VOID);
extern VOS_VOID Log_SelfTask(VOS_UINT32 ulPara1, VOS_UINT32 ulPara2,
                         VOS_UINT32 ulPara3, VOS_UINT32 ulPara4);
extern VOS_UINT32 Log_CheckPara(LOG_ID_LEVEL_STRU *pstLogIdLevel, VOS_UINT32 ulLength);

/* BBIT版本使用的内部函数 */
#ifdef __LOG_BBIT__
extern VOS_VOID Log_BuildStr(VOS_CHAR          *pcFileName,  VOS_UINT32  ulLineNum,
                         LOG_LEVEL_EN   enPrintLev,  VOS_CHAR          *pcOriStr,
                         VOS_UINT8  ucParaCnt,   VOS_INT32          *plPara,
                         VOS_CHAR          *pcDstStr,    VOS_UINT32 *pulLen);
extern int Log_StrNParam(VOS_CHAR             *cFileName,  VOS_UINT32      ulLineNum,
                    LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
                    LOG_LEVEL_EN      enLevel,    VOS_CHAR              *pcOriStr,
                    VOS_UINT8     ucParaCnt,  VOS_INT32              *plPara);
#endif

/* Release版本使用的内部函数 */
#ifdef __LOG_RELEASE__
extern VOS_VOID Log_BuildId(VOS_UINT32  ulLogId, VOS_UINT8  ucParaCnt, VOS_INT32 *plPara,
                           VOS_CHAR          *pcDst,   VOS_UINT32 *pulLen);
extern int Log_IdNParam(LOG_MODULE_ID_EN enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
                      LOG_LEVEL_EN     enLevel,    VOS_UINT32     ulLogId,
                      VOS_UINT8    ucParaCnt,  VOS_INT32             *plPara);
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __PSLOGOSA_H__ */
