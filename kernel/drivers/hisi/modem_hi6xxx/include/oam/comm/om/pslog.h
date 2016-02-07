/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : PsLog.h
  版 本 号   : 初稿
  作    者   : 李霄 46160
  生成日期   : 2007年4月23日
  最近修改   :
  功能描述   : PsLog.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2007年4月23日
    作    者   : 李霄 46160
    修改内容   : 创建文件

******************************************************************************/


#ifndef __PSLOG_H__
#define __PSLOG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include "PsLogdef.h"
#include "errorlog.h"

/*****************************************************************************
  2 宏定义
******************************************************************************/
#define LOG_INVALID             0xFFFF

#define LOG_MAX_FILENAME_LEN    25

#define LOG_MAX_PARA_CNT        4
#define LOG_MAX_PARA_LEN        8

#define LOG_MAX_FLASH_SIZE      0x20000

#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
#define LOG_FILE_NAME           "/data/modemlog/OmLogFile.dat"
#else
#define LOG_FILE_NAME           "/yaffs0/OmLogFile.dat"
#endif

/*全路径文件名的最大支持长度*/
#define LOG_FILE_PATH_MAX_LENGTH      (30)

/*日志文件支持的大小限制 */
#define LOG_FILE_MAX_SIZE             (64*1024)

/* 信号量名字长度 */
#define LOG_SEM_NAME_LEN              (6)

/* PS_LOG日志文件保护标志 */
#define PS_LOG_FILE_PROTECT           (0)

#define LOG_MAX_COLUMN_VAL           900

/*由于发送给工具侧的LOG需要添加模块ID,打印级别, 所以在分配内存时需要加8*/
#define LOG_MODULE_ID_LEN                   8

#define LOG_MAX_COLUMN_VAL_EX               (LOG_MAX_COLUMN_VAL + LOG_MODULE_ID_LEN)

/* 由于缓冲区实际大小为申请长度加 1 ,为保证数据存放在缓冲区中的偶地址,提高读写效率,此处加 3 */
/* OM_Printf环形缓冲区大小 */
#define OM_PRINTF_BUFFER_SIZE               ((32*1024)+3)


#ifdef __LOG_BBIT__
#define LOG_MAX_ROW_VAL                     128
#else
#define LOG_MAX_ROW_VAL                     8
#endif

#define LOG_BUF_VOLUMN                      (LOG_MAX_COLUMN_VAL_EX * LOG_MAX_ROW_VAL)

/* 确认c条件成立的宏，如果不成立则返回 */
#define LOG_AFFIRM(c) \
    if(!(c)) \
    { \
        vos_printf("LOG_ASSERT\n" ); \
        return; \
    }



/* 协议栈打点方式而增加 */
#define PsLogId( FILEID, LINE ) \
            ( ( ( FILEID & 0x0000FFFF ) << 16 ) | ( LINE & 0x0000FFFF ) )

/* PS封装的打印级别定义 */
#define PS_PRINT_OFF        LOG_LEVEL_OFF        /* 关闭打印     */
#define PS_PRINT_ERROR      LOG_LEVEL_ERROR      /* Error级别    */
#define PS_PRINT_WARNING    LOG_LEVEL_WARNING    /* Warning级别  */
#define PS_PRINT_NORMAL     LOG_LEVEL_NORMAL     /* Normal级别   */
#define PS_PRINT_INFO       LOG_LEVEL_INFO       /* Info级别     */
#define PS_PRINT_BUTT       LOG_LEVEL_BUTT       /* 保留级别     */

/*******************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  5 外部接口定义
*****************************************************************************/
/* BBIT版本提供的外部接口 */
#ifdef __LOG_BBIT__
extern VOS_VOID OM_Log(VOS_CHAR  *cFileName,  VOS_UINT32         ulLineNum,
                   LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN    enSubModId,
                   LOG_LEVEL_EN      enLevel,    VOS_CHAR           *pcString);
extern VOS_VOID OM_Log1(VOS_CHAR *cFileName,  VOS_UINT32         ulLineNum,
                    LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
                    LOG_LEVEL_EN      enLevel,    VOS_CHAR          *pcString,
                    VOS_INT32         lPara1);
extern VOS_VOID OM_Log2(VOS_CHAR *cFileName,  VOS_UINT32         ulLineNum,
                    LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
                    LOG_LEVEL_EN      enLevel,    VOS_CHAR          *pcString,
                    VOS_INT32         lPara1,     VOS_INT32          lPara2);
extern VOS_VOID OM_Log3(VOS_CHAR *cFileName,  VOS_UINT32         ulLineNum,
                    LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
                    LOG_LEVEL_EN      enLevel,    VOS_CHAR          *pcString,
                    VOS_INT32         lPara1,     VOS_INT32          lPara2,
                    VOS_INT32         lPara3);
extern VOS_VOID OM_Log4(VOS_CHAR *cFileName,  VOS_UINT32         ulLineNum,
                    LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
                    LOG_LEVEL_EN      enLevel,    VOS_CHAR          *pcString,
                    VOS_INT32         lPara1,     VOS_INT32          lPara2,
                    VOS_INT32         lPara3,     VOS_INT32          lPara4);
#endif

/* Release版本提供的外部接口 */
#ifdef __LOG_RELEASE__
extern VOS_VOID OM_LogId(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
                     LOG_LEVEL_EN       enLevel,    VOS_UINT32     ulLogId);
extern VOS_VOID OM_LogId1(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
                      LOG_LEVEL_EN      enLevel,    VOS_UINT32     ulLogId,
                      VOS_INT32              lPara1);
extern VOS_VOID OM_LogId2(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
                      LOG_LEVEL_EN      enLevel,    VOS_UINT32     ulLogId,
                      VOS_INT32              lPara1,     VOS_INT32              lPara2);
extern VOS_VOID OM_LogId3(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
                      LOG_LEVEL_EN      enLevel,    VOS_UINT32     ulLogId,
                      VOS_INT32              lPara1,     VOS_INT32              lPara2,
                      VOS_INT32              lPara3);
extern VOS_VOID OM_LogId4(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
                      LOG_LEVEL_EN      enLevel,    VOS_UINT32     ulLogId,
                      VOS_INT32              lPara1,     VOS_INT32              lPara2,
                      VOS_INT32              lPara3,     VOS_INT32              lPara4);
#endif



/*****************************************************************************
  6 PS封装的外部接口函数定义
*****************************************************************************/
#ifdef __LOG_BBIT__
#define PS_LOG(ModulePID, SubMod, Level, String) \
     OM_Log(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String)

#define PS_LOG1(ModulePID, SubMod, Level, String, Para1) \
    OM_Log1(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String, Para1)

#define PS_LOG2(ModulePID, SubMod, Level, String, Para1, Para2) \
    OM_Log2(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String,\
               Para1, Para2)

#define PS_LOG3(ModulePID, SubMod, Level, String,\
            Para1, Para2, Para3) \
    OM_Log3(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String,\
            Para1, Para2, Para3)

#define PS_LOG4(ModulePID, SubMod, Level, String, \
            Para1, Para2, Para3, Para4) \
    OM_Log4(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String,\
            Para1, Para2, Para3, Para4)

#endif

#ifdef __LOG_RELEASE__

#define PS_LOG(ModulePID, SubMod, Level, String) \
     OM_LogId((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__))

#define PS_LOG1(ModulePID, SubMod, Level, String, Para1) \
     OM_LogId1((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__), Para1)

#define PS_LOG2(ModulePID, SubMod, Level, String, Para1, Para2) \
     OM_LogId2((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__), Para1, Para2)

#define PS_LOG3(ModulePID, SubMod, Level, String, \
            Para1, Para2, Para3) \
     OM_LogId3((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__), Para1, Para2, Para3)

#define PS_LOG4(ModulePID, SubMod, Level, String,\
            Para1, Para2, Para3, Para4) \
     OM_LogId4((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__), Para1, Para2, Para3, Para4)
#endif

/*****************************************************************************
  提供给外部模块使用的LOG接口，例如DRV、MEDIA
*****************************************************************************/

#ifdef __LOG_BBIT__
#define OM_LOG(ModulePID, SubMod, Level, String) \
     OM_Log(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String)

#define OM_LOG1(ModulePID, SubMod, Level, String, Para1) \
     OM_Log1(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String, Para1)

#define OM_LOG2(ModulePID, SubMod, Level, String, Para1, Para2) \
    OM_Log2(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String,\
               Para1, Para2)

#define OM_LOG3(ModulePID, SubMod, Level, String,\
            Para1, Para2, Para3) \
    OM_Log3(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String,\
            Para1, Para2, Para3)

#define OM_LOG4(ModulePID, SubMod, Level, String, \
            Para1, Para2, Para3, Para4) \
    OM_Log4(__FILE__, __LINE__, (LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, String,\
            Para1, Para2, Para3, Para4)

#endif

#ifdef __LOG_RELEASE__

#define OM_LOG(ModulePID, SubMod, Level, String) \
     OM_LogId((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__))

#define OM_LOG1(ModulePID, SubMod, Level, String, Para1) \
     OM_LogId1((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__), Para1)

#define OM_LOG2(ModulePID, SubMod, Level, String, Para1, Para2) \
     OM_LogId2((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__), Para1, Para2)

#define OM_LOG3(ModulePID, SubMod, Level, String, \
            Para1, Para2, Para3) \
     OM_LogId3((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__), Para1, Para2, Para3)

#define OM_LOG4(ModulePID, SubMod, Level, String,\
            Para1, Para2, Para3, Para4) \
     OM_LogId4((LOG_MODULE_ID_EN)ModulePID, (LOG_SUBMOD_ID_EN)SubMod, Level, \
            PsLogId(THIS_FILE_ID, __LINE__), Para1, Para2, Para3, Para4)
#endif

VOS_UINT32 LOG_GetTick(VOS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PSLOG_H__ */

