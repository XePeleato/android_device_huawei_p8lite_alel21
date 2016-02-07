/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : OmFileSystem.h
  版 本 号      : 初稿
  作    者      : 甘兰47350
  生成日期      : 2008年9月16日
  最近修改      :
  功能描述      :
  函数列表      :
  修改历史      :
  1.日    期    : 2008年9月16日
    作    者    : 甘兰47350
    修改内容    : 创建文件

******************************************************************************/
#ifndef _OM_FILESYSTEM_H_
#define _OM_FILESYSTEM_H_

#ifdef __cplusplus
  #if __cplusplus
  extern "C"{
  #endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include "vos.h"
#include "omprivate.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*判断打开文件的方式*/
#define ISWIRTE(Flags)                  (Flags&0x1)
#define ISREAD(Flags)                   (Flags&0x2)
#define ISCREATE(Flags)                 (Flags&0x4)

/*启动定时器*/
#define OM_START_FILETIMER(phTimer)     VOS_StartRelTimer(phTimer, \
                  WUEPS_PID_OM, 5000, OM_FILE_TIMER, 0, VOS_RELTIMER_NOLOOP, VOS_TIMER_NO_PRECISION)
/*关闭定时器*/
#define OM_STOP_FILETIMER(phTimer)      VOS_StopRelTimer(phTimer)

/*读取文件内容的最大长度*/
#define OM_READ_MAX_LEN                 (1024*32)

/*读文件消息包的头长度，详见:OM_APP_READ_FILE_STRU*/
#define OM_READ_FILE_HEAD_LEN           8
/*文件或文件夹每一项的结构的头长度，详见: OM_ITEM_INFO_STRU*/
#define OM_ITEM_INFO_HEAD_LEN           24
/*获取文件或文件夹基本信息的头长度，详见OM_APP_SCAN_DIR_STRU*/
#define OM_APP_SCAN_DIR_HEAD_LEN        8

#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
/* 硬加密文件路径 */
#define OM_CFDNV_FILE_PATH              ("/mnvm2:0/NvimUse/CFDNVTable.bin")
/* DSP文件路径 */
#define OM_ZSP_FILE_PATH                ("/data/modemlog/ZSP.bin")
#else
/* 硬加密文件路径 */
#define OM_CFDNV_FILE_PATH              ("/yaffs0/Nvim/CFDNVTable.bin")
/* DSP文件路径 */
#define OM_ZSP_FILE_PATH                ("/yaffs0/ZSP.bin")
#endif

#define OM_FILE_SEND_RESULT(enChannel, msg_fun_type, error_no, ReturnPrimId)\
{\
    g_ulOmSendToolErrNo = error_no;\
    OM_SendResultChannel(enChannel, msg_fun_type, error_no, ReturnPrimId);\
}\

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*文件夹类型*/
enum OM_FOLDER_TYPE_ENUM
{
    OM_ROOT_FOLDER = 0,
    OM_LOG_FOLDER,
    OM_DUMP_FOLDER,
    OM_FILE_BUTT
};
/*目录中各项的类型*/
typedef enum
{
    OM_ITEM_FOLDER = 0,
    OM_ITEM_FILE,
    OM_ITEM_BUTT
}OM_ITEM_TYPE_ENUM;

/* OM 上报给工具测错误码 */
typedef enum
{
    OM_DRV_FILE_OPENDIR_ERR       = 3,
    OM_DRV_FILE_MKDIR_ERR         = 4,
    OM_DRV_FILE_RMDIR_ERR         = 5,
    OM_DRV_FILE_CLOSEDIR_ERR      = 6,

    OM_DRV_FILE_OPEN_ERR          = 7,
    OM_DRV_FILE_READ_ERR          = 8,
    OM_DRV_FILE_WRITE_ERR         = 9,
    OM_DRV_FILE_RMFILE_ERR        = 10,
    OM_DRV_FILE_WRITE_LEN_ERR     = 11,

    OM_DRV_GET_DIRECTORY_ERR        = 12,
    OM_DRV_FILE_GET_DISKSPACE_ERR   = 13,
    OM_DRV_FILE_OPERATE_NOT_FINISH  = 14,

    OM_FILE_OPERATE_SEC_FILE_FORBID_CFD = 15,
    OM_FILE_OPERATE_SEC_FILE_FORBID_ZSP = 16,
    OM_FILE_RECEIVE_REQ_FIND_FUNC_ERR   = 17,
    OM_FILE_RECEIVE_ITEM_TYPE_ERR       = 18,
    OM_FILE_RECEIVE_FOLDER_TYPE_ERR     = 19,
    OM_FILE_START_FILE_TIMER_ERR        = 20,
    OM_FILE_MEM_ALLOC_ERR               = 21,
    OM_FILE_OPEN_SC_ERR                 = 22,

    OM_FILE_SEND_RESULT_ERR_BUTT
}OM_FILE_SEND_RESULT_ERR_ENUM;

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
/*保存操作文件的信息*/
typedef struct
{
    FILE *      lFile;
    VOS_UINT32  ulFileType;
    HTIMER      hTimer;
}OM_FILE_INFO_STRU;
/*工具侧与OM交互使用的查询目录结构*/
typedef struct
{
    VOS_UINT32  ulResult;
    VOS_UINT32  ulLength;
    VOS_UINT8   aucDirPath[DRV_NAME_MAX + 1];
}OM_APP_QUERY_DIR_STRU;
/*工具侧与OM交互使用的读取文件内容结构*/
typedef struct
{
    VOS_UINT32  ulResult;
    VOS_UINT32  ulLength;
    VOS_CHAR    acFileContent[OM_READ_MAX_LEN];
}OM_APP_READ_FILE_STRU;
/*文件或文件夹每一项的结构*/
typedef struct
{
    VOS_UINT32  ulItemType;
    VOS_UINT32  ulSize;      /*文件大小，目录没有大小*/
    DRV_TIME    st_atime;    /*最近访问时间，目录没有此字段*/
    DRV_TIME    st_mtime;    /*最近修改时间，目录没有此字段*/
    DRV_TIME    st_ctime;    /*创建时间*/
    VOS_UINT32  ulNameLen;   /*文件或文件夹名字长度*/
    VOS_UINT8   aucName[4];
}OM_ITEM_INFO_STRU;
/*工具侧与OM交互使用的获取文件或文件夹基本信息的结构*/
typedef struct
{
    VOS_UINT32        ulResult;
    VOS_UINT32        ulNumber;
    OM_ITEM_INFO_STRU stOmItemInfo[1];
}OM_APP_SCAN_DIR_STRU;
/*工具侧发给OM打开文件的结构体*/
typedef struct
{
    VOS_UINT32        ulFlag;
    VOS_UINT8         aucPara[4];
}APP_OM_OPEN_INFO_STRU;
/*工具侧与OM交互使用的查询文件系统空间大小*/
typedef struct
{
    VOS_OM_HEADER
    VOS_UINT16        usPrimId;   /*Indicates current msg type.*/
    VOS_UINT16        usToolId;   /*Not used now.*/
    VOS_UINT32        ulResult;   /*Result of operation*/
    VOS_UINT32        ulDiskSpace;
    VOS_UINT32        ulUsedSpace;
    VOS_UINT32        ulValidSpace;
}OM_APP_QUERY_DISKSPACE_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_VOID OM_CloseFile(VOS_VOID);
VOS_VOID OM_FileSystemMsgProc(OM_REQ_PACKET_STRU *pRspPacket,
                                      OM_RSP_FUNC *pRspFuncPtr);

extern VOS_VOID Log_PauseFile(VOS_VOID);
extern VOS_VOID Log_ResumeFile(VOS_VOID);

extern FILE* OM_Open(const VOS_CHAR *pcFileName, VOS_INT lFlag);
extern VOS_UINT32 OM_FileSystemInit(VOS_VOID);

#ifdef __cplusplus
  #if __cplusplus
  }
  #endif
#endif

#endif
