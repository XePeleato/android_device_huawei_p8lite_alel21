/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : reset_ccoreloader.h
  版 本 号   : 初稿
  作    者   : 卢彦胜 00212112
  生成日期   : 2013年4月22日
  最近修改   :
  功能描述   : reset_ccoreloader.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年8月29日
    作    者   : 卢彦胜 00212112
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef RESET_CCORELOADER_H
#define RESET_CCORELOADER_H
#include <linux/hisi/reset.h>

/*#include  ###*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
宏以及变量定义
*****************************************************************************/
/*reference to copy from bootloader, fastboot, starts*/
#define MCU_SECTION_NUMBER_MAX 30
#define CCORE_SECTION_NUMBER_MAX MCU_SECTION_NUMBER_MAX

#define EFI_ENTRIES     128
#define EFI_ENTRY_SIZE  (sizeof(struct efi_entry))
#define EFI_MBR_SIZE    512
#define EFI_HEADER_SIZE 512
#define EFI_TOTAL_SIZE  (EFI_MBR_SIZE + EFI_HEADER_SIZE + EFI_ENTRY_SIZE*EFI_ENTRIES)
/* 支持最大的分区个数 */
#define MAX_PTN 32

#define EMMC_BLOCK_SIZE    (512)
#define PTABLE_START_STR                "EFI PART"


/*****************************************************************************
 实 体 名  : CCORE_IMAGE_SEC_TYPE_ENUM
 功能描述  : 镜像段类型
*****************************************************************************/
enum CCORE_IMAGE_SEC_TYPE_ENUM
{
    CCORE_IMAGE_SEC_TYPE_CODE = 0,        /* 代码 */
    CCORE_IMAGE_SEC_TYPE_DATA,            /* 数据 */
    CCORE_IMAGE_SEC_TYPE_BUTT
};
typedef unsigned char CCORE_IMAGE_SEC_TYPE_ENUM_UINT8;

/*****************************************************************************
 实 体 名  : CCORE_IMAGE_SEC_LOAD_ENUM
 功能描述  : 镜像段加载属性
*****************************************************************************/
enum CCORE_IMAGE_SEC_LOAD_ENUM
{
    CCORE_IMAGE_SEC_LOAD_STATIC           = 0,    /* 静态加载, 仅加载到运行地址 */
    CCORE_IMAGE_SEC_LOAD_DYNAMIC          = 1,    /* 动态加载, 同时加载到缓存buffer和运行地址 */
    CCORE_IMAGE_SEC_LOAD_BUFFER           = 2,    /* 动态加载, 仅加载到缓存buffer */
    CCORE_IMAGE_SEC_LOAD_MODEM_ENTRY      = 3,    /* 静态加载，且为MODEM运行起始地址 */

    CCORE_IMAGE_SEC_LOAD_BUTT
};
typedef unsigned char CCORE_IMAGE_SEC_LOAD_ENUM_UINT8;

/*****************************************************************************
 实 体 名  : CCORE_IMAGE_SEC_STRU
 功能描述  : 镜像段定义
*****************************************************************************/
typedef struct _CCORE_IMAGE_SEC_STRU_TAG
{
    unsigned short                      uhwSn;              /* 编号 */
    CCORE_IMAGE_SEC_TYPE_ENUM_UINT8       enType;             /* 类型 */
    CCORE_IMAGE_SEC_LOAD_ENUM_UINT8       enLoadAttrib;       /* 加载属性 */
    unsigned int                       uwSrcOffset;        /* 在文件中的偏移, bytes */
    unsigned int                       uwDesOffset;        /* 加载目的地址, bytes */
    unsigned int                       uwSize;             /* 段长度, bytes */

}CCORE_IMAGE_SEC_STRU;

/*****************************************************************************
 实 体 名  : CCORE_IMAGE_HEAD_STRU
 功能描述  : 镜像文件头定义
*****************************************************************************/
typedef struct _CCORE_IMAGE_HEAD_STRU_TAG
{
    char                                szTimeStamp[24];                        /* 镜像编译时间 */
    unsigned int                       uwImageSize;                            /* 镜像文件大小, bytes */
    unsigned int                       uwSecNum;                               /* 文件包含段数目 */
    CCORE_IMAGE_SEC_STRU                  astSections[CCORE_SECTION_NUMBER_MAX];    /* 段信息, 共uwSecNum个 */

}CCORE_IMAGE_HEAD_STRU;


/*ends*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
