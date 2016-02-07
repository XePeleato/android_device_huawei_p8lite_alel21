/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : RnicDemandDialFileIO.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2012年1月31日
  最近修改   :
  功能描述   : RnicDemandDialFileIO.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年1月31日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/

#ifndef __RNICDEMANDDIALFILEIO_H__
#define __RNICDEMANDDIALFILEIO_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "dopra_def.h"
#if (VOS_OS_VER == VOS_LINUX)
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kallsyms.h>
#include <asm/uaccess.h>
#else
#include "RnicLinuxInterface.h"
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


#define RNIC_ONDEMAND_FILE_LEN          (4)

#define RNIC_IDLETIMEROUT_FILE_LEN      (16)

#define RNIC_EVENTFLAG_FILE_LEN         (4)

/*lint -e960 修改人:l60609;原因:64bit*/
#define RNIC_VFILE_CRT_LEVEL            (0770)
/*lint +e960 修改人:l60609;原因:64bit*/

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
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 RNIC_InitDialEventReportFile(struct proc_dir_entry *pstParentFileDirEntry);
extern VOS_UINT32 RNIC_InitIdleTimerOutFile(struct proc_dir_entry *pstParentFileDirEntry);
extern VOS_UINT32 RNIC_InitOnDemandFile(struct proc_dir_entry *pstParentFileDirEntry);
extern ssize_t RNIC_ReadDialEventReportFile(
           struct file                        *file,
           char __user                        *buf,
           size_t                              len,
           loff_t                             *ppos
       );
extern ssize_t RNIC_ReadIdleTimerOutFile(
           struct file                        *file,
           char __user                        *buf,
           size_t                              len,
           loff_t                             *ppos
       );
extern ssize_t RNIC_ReadOnDemandFile(
           struct file                        *file,
           char __user                        *buf,
           size_t                              len,
           loff_t                             *ppos
       );
extern ssize_t RNIC_WriteDialEventReportFile(
           struct file                        *file,
           const char __user                  *buf,
           size_t                              len,
           loff_t                             *ppos
       );
extern ssize_t RNIC_WriteIdleTimerOutFile(
           struct file                        *file,
           const char __user                  *buf,
           size_t                              len,
           loff_t                             *ppos
       );
extern ssize_t RNIC_WriteOnDemandFile(
           struct file                        *file,
           const char __user                  *buf,
           size_t                              len,
           loff_t                             *ppos
       );
extern VOS_UINT32 RNIC_InitDemandDialFile(VOS_VOID);









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

#endif /* end of RnicDemandDialFileIO.h */
