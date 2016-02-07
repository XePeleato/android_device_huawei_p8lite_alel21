

#ifndef __OMSDLOG_H__
#define __OMSDLOG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "omringbuffer.h"
#include "omprivate.h"

#pragma pack(4)

#if (VOS_OS_VER == VOS_LINUX)
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 用于判断SD卡是否为新卡 */
#define OM_SD_MAGIC_NUMBER            (0xAAAA5555)
#define OM_SD_OPPOSITE_MAGIC_NUMBER   (0x5555AAAA)

/* SD卡头部长度 */
#define OM_READ_SD_HEAD_LEN           (12)

/* SD卡缓存trace的buffer长度 */
#define OM_SD_BUFFER_LEN              (64*1024)

/* SD卡的每个cluster数据大小，单位为KB */
#define OM_SD_CLUSTER_DATA_LEN        (64)

/* SD卡缓存trace的buffer除data字段外的长度 */
#define OM_SD_BUFFER_INFO_LEN         (12 + OM_APP_MSG_EX_LEN + OM_READ_SD_HEAD_LEN)

/* SD卡缓存trace的buffer中data字段的最大长度 */
#define OM_SD_BUFFER_MAX_DATA_LEN     (OM_SD_BUFFER_LEN - OM_SD_BUFFER_INFO_LEN)

/* SD卡中每个block的字节大小 */
#define OM_SD_BLOCK_SIZE              (512)

/* SD卡中每个cluster所含的block数 */
#define OM_SD_BLOCK_NUM_ONE_CLUSTER   (128)

/* 定义1KB的数值 */
#define OM_SD_KB                      (1024)

/* SD卡中第0个block除了头部控制信息外的剩余字节 */
#define OM_SD_REMAIN_DATA_LEN         (492)

/* 给PC侧一次报SD卡中cluster数据的个数 */
#define OM_SD_IND_INTERVAL            (10)

/* 写SD卡时，数据长度的临界长度,用于debug */
#define OM_SD_MAX_DATA_LENTH          (0x7FFFFFFF)

#define DRV_SDMMC_USB_SEC_READ(ulAddr, ulBlocks, pucBuffer)  (0)
/*        sd_transfer(g_pstScList, ulAddr, ulBlocks, OM_SD_BLOCK_SIZE, OM_READ_SD); \
 */
/*        VOS_MemCpy(pucBuffer, g_pucBuffer, ulBlocks*OM_SD_BLOCK_SIZE); \
 */

#define DRV_SDMMC_USB_SEC_WRITE(ulAddr, ulBlocks, pucBuffer) (0)
/*        VOS_MemCpy(g_pucBuffer, pucBuffer, ulBlocks*OM_SD_BLOCK_SIZE); \
 */
/*        sd_transfer(g_pstScList, ulAddr, ulBlocks, OM_SD_BLOCK_SIZE, OM_WRITE_SD); \
 */

#define DRV_SDMMC_GET_CAPACITY() sd_get_capacity()

#if (VOS_OS_VER == VOS_LINUX)
#define SD_FS_OPEN(filename,flags,mode)         sys_open(filename,flags,mode)
#define SD_FS_OPENDIR(dirname,flags,mode)       sys_open(dirname,flags,mode)
#define SD_FS_CLOSE(fd)                         sys_close(fd)
#define SD_FS_CLOSEDIR(dir_handle)              sys_close(dir_handle)
#define SD_FS_READ(fd,buffer,count)             sys_read(fd,buffer,count)
#define SD_FS_GETDENTS(fd, buf, count)          sys_getdents(fd, buf, count)
#define SD_FS_WRITE(fd,buffer,count)            sys_write(fd,buffer,count)
#define SD_FS_MKDIR(dirname,mode)               sys_mkdir(dirname,mode)
#define SD_FS_RMDIR(dirname)                    sys_rmdir(dirname )
#define SD_FS_RMFILE(filename)                  sys_unlink(filename)
#define SD_FS_LSEEK(fd,offset,origin)           sys_lseek(fd,offset,origin)
#define SD_FS_STAT(path,buf)                    vfs_stat(path,buf)
#define SD_FS_ACCESS(path,mode)                 sys_access(path,mode)
#define SD_FS_SYNC()                            sys_sync()
#define SD_FS_FILE_SYNC(fd)                     sys_fsync(fd)

#define SD_FS_GETFS()                           get_fs()
#define SD_FS_SETFS(fs)                         set_fs(fs)
#else
#define SD_FS_OPEN(filename,flags,mode)
#define SD_FS_OPENDIR(dirname,flags,mode)
#define SD_FS_CLOSE(fd)
#define SD_FS_CLOSEDIR(dir_handle)
#define SD_FS_READ(fd,buffer,count)
#define SD_FS_READDIR(dir_handle)
#define SD_FS_WRITE(fd,buffer,count)
#define SD_FS_MKDIR(dirname,mode)
#define SD_FS_RMDIR(dirname)
#define SD_FS_RMFILE(filename)
#define SD_FS_LSEEK(fd,offset,origin)
#define SD_FS_FTELL(fp)
#define SD_FS_STAT(path,pStat)
#define SD_FS_SYNC()
#define SD_FS_FILE_SYNC(fd)

#define SD_FS_GETFS()
#define SD_FS_SETFS(fs)
#endif

/*FLASH保存Log文件默认大小*/
#define OM_FLASH_DEFAULT_FILE_SIZE              (1024*1024)

/*文件打开类型*/
#define OM_SD_FILE_MODE                         (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/*FALSH最大保存空间*/
#define OM_FLASH_MAX_SIZE                       (50)

#define OM_FLASH_DEFALUT_FILE_NUM               (20)

#define OM_SD_LOG_DIR_LEN                       (2048)

#define OM_SD_LOG_PATH_MAX_LENGTH               (128)

/* 日志文件写操作的最大次数 */
#define LOG_WRITE_MAX_COUNT                     (200)

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
/* 压缩文件头标记 */
#define UCMX_FILE_HEAD_TAG                      (0xAAAA)

/* 压缩文件内容标记 */
#define UCMX_DATA_TAG                           (0xA6A6A6A6)

/* 压缩后数据最大长度*/
#define OM_MAX_COMPRESS_DATA_LEN                (20*1024)

/* 最大组包长度,收到大于组包长度数据，直接压缩 */
#define OM_MAX_PACK_DATA_LEN                    (10*1024)
#endif

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum OM_SD_ERRNO
{
     OM_SD_OK                 = 0,
     OM_ERR_NOCARD,
     OM_ERR_NEWSDCARD,
     OM_ERR_VALIDPARA,
     OM_ERR_DRVAPI,
     OM_ERR_DATAERR,
     OM_ERR_MALLOCSPACE,
     OM_ERR_NOENOUGHDATA,
     OM_ERR_EMPTY,
     OM_SD_BUTT
};

typedef VOS_UINT32  OM_READSD_ERRNO_ENUM_UINT32;

/* 指示SD的操作方式 */
enum
{
    OM_READ_SD = 0,
    OM_WRITE_SD,
};

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
/* 文件头消息段类型枚举 */
enum OM_LOG_HEAD_SEG_ENUM
{
    OM_LOG_HEAD_SEG_TIMER    = 0x01, /* 创建当前时间 */
    OM_LOG_HEAD_SEG_UE_INFO  = 0x02, /* UE信息 */

    OM_LOG_HEAD_SEG_BUTT
};
typedef VOS_UINT8 OM_LOG_HEAD_SEG_ENUM_UINT8;


/* 压缩算法 */
enum OM_COMPRESS_TYPE_ENUM
{
    OM_COMPRESS_FASTLZ = 0x01, /* fastlz 压缩算法 */

    OM_COMPRESS_BUTT
};
typedef VOS_UINT16 OM_COMPRESS_TYPE_ENUM_UINT16;

/* 压缩算法版本 */
enum OM_COMPRESS_VERSION_ENUM
{
    OM_COMPRESS_VERSION_1 = 0x01, /* fastlz 压缩算法 */

    OM_COMPRESS_VERSION_BUTT
};
typedef VOS_UINT16 OM_COMPRESS_VERSION_ENUM_UINT16;
#endif

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef struct
{
    VOS_UINT32             ulResult;   /*0: ok 1: error*/
    VOS_UINT32             ulReportCNum;   /*需要读取的64KB数*/
    VOS_UINT32             ulHaveCNum;   /*实际有的cluster数*/
}OM_APP_READ_SD_CNF_STRU;

/*工具侧用来配置打印级别的结构*/
typedef struct
{
    VOS_UINT32             ulTotalSize;
    VOS_UINT32             ulOffsetSize;

}APP_OM_READ_SD_REQ_STRU;

typedef struct
{
    VOS_UINT32  ulClusterId;
    VOS_UINT32  ulErrCode;                          /*0 代表正确； 1 代表失败。当前ulClusterId为坏块时，此值为1*/
    VOS_UINT32  ulSn;                               /*64kB数据包编号,编号从0开始*/
    VOS_UINT8   aucDataContent[4];
}OM_APP_READ_SD_STRU;

typedef struct
{
    VOS_UINT32  ulMagicNum;                        /* 用于判断是否为新卡 */
    VOS_UINT32  ulOppositeMagicNum;                /* ulMagicNum的取反 */
    VOS_UINT32  ulTotalCNum;                      /* SD卡的ClusterNum，一个Cluster为64KB */
    VOS_UINT32  ulNextCid;                         /* 记录下一个待写入的Cluster ID */
    VOS_UINT32  ulLoopOutFlag;                     /* 标志SD卡数据是否发生翻转 */
    VOS_UINT8   aucReserved[OM_SD_REMAIN_DATA_LEN];
}OM_SD_HEAD_STRU;

typedef struct
{
    VOS_UINT32 ulTotalNeedCNum;
    VOS_UINT32 ulCurNeedCNum;
    VOS_UINT32 ulStartCID;
}OM_SD_READ_INFO_STRU;

typedef struct
{
    VOS_UINT32  ulClusterId;                           /* 当前cluster ID */
    VOS_UINT32  ulDataLen;                             /* buffer中数据长度 */
    VOS_UINT8   aucReserved[OM_APP_MSG_EX_LEN + OM_READ_SD_HEAD_LEN]; /* 保留字段，用于导出数据时填充 */
    VOS_UINT8   aucBuffer[OM_SD_BUFFER_MAX_DATA_LEN];  /* 数据内容 */
    VOS_UINT32  ulOppositeClusterId;                   /* cluster ID取反，用于判断数据完整性 */
}OM_SD_BUFFER_INNER_DATA_STRU;

typedef struct
{
    VOS_UINT32 ulTatalBlock;        /* 记录写数据时，总是Block数 */
    VOS_UINT32 ulOverNum;           /* 记录数据长度超过OM_SD_MAX_DATA_LENTH 的次数*/
    VOS_UINT32 ulDataLenth;         /* ulVoerNum　剩下的数据 */
    VOS_UINT32 ulRemainLenth;       /* 存在buffer中，还未写到SD卡中 */
    VOS_UINT32 ulReadHeadErrLenth;  /*读取头错误，丢掉的数据长度 */
}OM_SD_DEBUG_INFO;

/*****************************************************************************
 结构名    : OM_FLASH_CFG_INFO_STRU
 结构说明  : FLASH保存TRACE文件记录信息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32              ulFileMinId;
    VOS_UINT32              ulFileMaxId;
}OM_FLASH_CFG_INFO_STRU;

/*****************************************************************************
 结构名    : OM_FLASH_DEBUG_INFO
 结构说明  : FLASH保存TRACE文件信息结构体
*****************************************************************************/
typedef struct
{
    VOS_BOOL                bIsWritten;             /* Log文件可写标志 */
    VOS_INT                 lFileHandle;
    VOS_UINT32              ulRemainCount;
    VOS_UINT32              ulFileSize;             /* Log文件实际大小 */
    VOS_UINT32              ulFileMaxSize;          /* Log文件大小限制 */
    VOS_UINT32              ulSem;                  /* Log文件信号量保护 */
    VOS_UINT32              ulErrLog;
    VOS_UINT32              ulWriteFailNum;
    VOS_CHAR                acName[64];             /* Log文件乒乓文件名 */
}OM_FLASH_DEBUG_INFO_STRU;

#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#pragma pack(1)
/*****************************************************************************
 结构名    : OM_UCMX_HEAD_INFO_STRU
 结构说明  : UCMX文件头,非四字节对齐
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usFileTag;         /* 文件标签 */
    VOS_UINT8                           ucFileVersion;     /* 文件本身版本号 */
    VOS_UINT32                          ulSn;              /* 文件分组时的文件编号 */

    VOS_UINT8                           ucHeadSegNum;      /* 文件头个数 */

#if 0
    OM_LOG_HEAD_SEG_ENUM_UINT8          enSEGtype1;
    VOS_UINT8                           ucHeadLength1;     /* 第一个文件头的数据长度，这里固定为3 */
    OM_COMPRESS_TYPE_ENUM_UINT16        enSuport;          /* 压缩算发枚举 */
    OM_COMPRESS_VERSION_ENUM_UINT16     enCompressVersion; /* 压缩算法版本号 */

    OM_LOG_HEAD_SEG_ENUM_UINT8          enSEGtype2;
    VOS_UINT8                           ucHeadLength2;     /* 第二个文件头的数据长度, 这里固定为19 */
#endif

    OM_LOG_HEAD_SEG_ENUM_UINT8          enSEGType3;
    VOS_UINT8                           ucHeadLength3;     /* 第三个文件头的数据长度，这里为48 */
    VOS_UINT8                           aucImei[16];       /* imei号 */
    VOS_UINT8                           ucUEBoardType;     /* UE版本类型 */
    VOS_UINT8                           aucUEVersion[31];  /* UE版本号 */

}OM_UCMX_HEAD_INFO_STRU;

/*****************************************************************************
 结构名    : OM_UCMX_DATA_INFO_STRU
 结构说明  : UCMX文件内容格式
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulDataTag;       /* 数据校验字段，固定为 UCMX_DATA_TAG  */
    VOS_UINT32                          ulDataLength;    /* 从下一个字节开始，数据的长度。 */
    VOS_UINT8                           aucData[OM_MAX_COMPRESS_DATA_LEN];       /* 数据部分 */
}OM_UCMX_DATA_INFO_STRU;


/*****************************************************************************
 结构名    : OM_COMPRESS_DEBUG_OG_STRU
 结构说明  : 用于记录压缩文件可维可测信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulRcvLen;
    VOS_UINT32                          ulRcvNum;
    VOS_UINT32                          ulCompressLen;
}OM_COMPRESS_DEBUG_OG_STRU;

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#pragma pack(4)
#endif

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 OM_SDWriteCard(VOS_UINT8* pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT16 usLen);


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of OmSDLog.h */

