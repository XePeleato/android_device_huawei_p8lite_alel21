

#ifndef __OMPRINTF_H__
#define __OMPRINTF_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "omringbuffer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define OM_ALIGNMENT                  (0x03U)

#define OM_PRINTF_OFFSET              (2U*sizeof(VOS_UINT32))

/*支持DRV和APP的最大模块数*/
#define OM_PRINTF_MODULE_MAX_NUM      (LOG_MAX_MODULE_ID_APP - LOG_MIN_MODULE_ID_ACPU_DRV + 1)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum OM_PRINTF_ERRNO
{
     OM_OK                 = 0,
     OM_ERR_NOBUF,
     OM_ERR_FAILTAKESEM,
     OM_ERR_FAILPUTDATA,
     OM_ERR_FAILTAKETASKSEM,
     OM_ERR_DATADESTROY,
     OM_ERR_WRITEFLASH,
     OM_ERR_NOOMSNDBUF,
     OM_ERR_MALLOC,
     OM_ERR_SENDTOOMBUF,
     OM_ERR_LOWLEVEL,
     OM_ERR_UNVALIDPARA,
     OM_ERR_RUNINIRQ,
     OM_BUTT
};

typedef VOS_UINT32  OM_PRINTF_ERRNO_ENUM_UINT32;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
extern   OM_RING_ID                          g_RlOmSndBufId;


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
/* 记录由于无buffer造成数据丢失的定位信息结构体 */
typedef struct
{
    VOS_UINT32  ulOmPrintfRecvLen;                /* OM Printf转换后发送字符串数据总长度 */
    VOS_UINT32  ulOmPrintfNoPrintfBufferLostLen;  /* 由于没有OM Printf buffer丢失的数据长度 */

    VOS_UINT16  usOmPrintfRecvCount;              /* OM Printf转换后发送字符串数据总次数 */
    VOS_UINT16  usOmPrintfNoPrintfBufferLostCount;/* 由于没有OM Printf buffer丢失的数据次数 */
}OM_PRINTF_ERROR_RECORD_STRU;


typedef struct
{
    VOS_UINT32  ulModuleId;
    VOS_UINT32  ulPrintLev;
    VOS_UINT32  ulStrLen;
    VOS_CHAR    aucStr[4];
}OMPRITNF_MODULE_APP_STRU;


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern LOG_LEVEL_EN                     g_astOmPrintfOutPutLev[];

/*****************************************************************************
  10 函数声明
*****************************************************************************/
/*lint -e960 */
extern VOS_UINT32 OM_Printf(VOS_CHAR * pcformat, ... );
extern VOS_UINT32 OM_PrintfWithModule(VOS_UINT32 ulModuleId,
                        VOS_UINT32 ulLevel, VOS_CHAR * pcformat, ... );

/*lint +e960 */

extern VOS_UINT32 OM_PrintfSetModuleIdLev(VOS_UINT32 ulModuleId, VOS_UINT32 ulLev);

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

#endif /* end of OmPrintf.h */

