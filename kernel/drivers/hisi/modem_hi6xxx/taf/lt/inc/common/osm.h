/*****************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

*****************************************************************************
  文 件 名   : osm.h
  版 本 号   : V1.0
  作    者   : 沈汉坤/00130424
  生成日期   : 2008年07月07日
  功能描述   : OSL接口申明
  函数列表   : 无
  修改历史   :
  1.日    期 : 2008年07月07日
    作    者 : 沈汉坤/00130424
    修改内容 : 创建文件
*****************************************************************************/

#ifndef __OSM_H__
#define __OSM_H__

#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct
{
    VOS_UINT32 ulMsgId;
    VOS_UINT32 ulParam2;
    VOS_UINT8 *ulParam1;
} OS_MSG_STRU;

typedef struct
{
    VOS_UINT32 ulTimeId;
    VOS_UINT32 ulPara;    /* 对应原来的ulParam2*/
} OSM_MSG_TIMER_STRU;


#define HAL_SDMLOG  (VOS_VOID)vos_printf
#define MSP_MEMCPY(dst,src,count)   VOS_MemCpy(dst,src,count)
#define MSP_MEMSET(dst, val, count) VOS_MemSet(dst, val, count)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __OSM_H__*/

