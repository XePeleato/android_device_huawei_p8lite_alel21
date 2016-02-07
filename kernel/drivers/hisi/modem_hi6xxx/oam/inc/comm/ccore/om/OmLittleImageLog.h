/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : OmLittleImageLog.h
  版 本 号      : 初稿
  作    者      : 王笑非184875
  生成日期      : 2012年12月27日
  最近修改      :
  功能描述      : 该h文件给出了OmLittleImageLog模块的实现
  函数列表      :
  修改历史      :
  1.日    期    : 2012年12月27日
    作    者    : 王笑非
    修改内容    : 创建文件

******************************************************************************/


#ifndef __OMLITTLEIMAGELOG_H__
#define __OMLITTLEIMAGELOG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/****************************************************************************
  1 其他头文件包含
*****************************************************************************/

/*****************************************************************************
  2 宏定义
******************************************************************************/
#define LOG_FILE_SIZE               (5)                 /* 默认LOG文件大小 */
#define LOG_FILE_UNIT_SIZE          (1024*1024)         /* 默认文件单元 */
#define LOG_FILE_ENABLE             (VOS_OK)
#define LOG_FILE_DISABLE            (VOS_ERR)           /* 默认使能为1，不是能LOG文件保存 */
#define LOG_FILE_PATH_MAX_LENGTH    (30)                /* LOG文件保存的路径长度 */
#define FILE_SIZE_THRESHOLD_PERCENT (0.9)
#define LOG_SEM_TIMEOUT_VALUE       (2000)

/*******************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 STRUCT定义
*****************************************************************************/

typedef struct
{
    VOS_UINT32              ulFileMinId;                /* 文件最早的序号 */
    VOS_UINT32              ulFileMaxId;                /* 文件daixi的序号 */
    VOS_UINT32              ulFSFreeSpace;              /* 文件系统空闲大小 */
    VOS_UINT32              ulCurLogFileSize;           /* 当前文件大小 */
    VOS_UINT32              ulCurLogFileMaxSize;        /* 单个文件默认大小 */
    VOS_UINT32              ulSem;                      /* 文件操作保护信号量 */
}LOG_FILE_SAVE_INFO_STRU;

typedef struct
{
    VOS_UINT32        ulDiskSpace;
    VOS_UINT32        ulUsedSpace;
    VOS_UINT32        ulValidSpace;
}LOG_FILE_DISK_INFO_STRU;

/*****************************************************************************
  5 外部接口定义
*****************************************************************************/
VOS_UINT32 OM_LittleImgWriteLogFile(VOS_CHAR *pcLogData, VOS_UINT32 ulLength);
VOS_UINT32 OM_StartUpLogSaveReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                            VOS_UINT16 usReturnPrimId);
VOS_UINT32 OM_StopLogSaveReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                            VOS_UINT16 usReturnPrimId);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PSLOG_H__ */