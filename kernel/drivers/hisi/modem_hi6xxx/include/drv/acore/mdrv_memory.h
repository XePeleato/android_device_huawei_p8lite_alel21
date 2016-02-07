
/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  mdrv_memory.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"mdrv.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __MDRV_ACORE_MEMORY_H__
#define __MDRV_ACORE_MEMORY_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include <mdrv_memory_common.h>

typedef enum tagMEM_MODE_TYPE
{
    MEM_DDR_MODE = 0,
    MEM_IPF_MODE,
    MEM_TTF_MODE,
    MEM_AXI_MODE,
    MEM_MODE_MAX
}MEM_MODE_TYPE_E;

typedef enum tagBSP_DDR_SECT_TYPE_E
{
    BSP_DDR_SECT_TYPE_TTF = 0x0,
    BSP_DDR_SECT_TYPE_ARMDSP,
    BSP_DDR_SECT_TYPE_UPA,
    BSP_DDR_SECT_TYPE_CQI,
    BSP_DDR_SECT_TYPE_APT,
    BSP_DDR_SECT_TYPE_ET,
    BSP_DDR_SECT_TYPE_BBPMASTER,
    BSP_DDR_SECT_TYPE_NV,
    BSP_DDR_SECT_TYPE_DICC,
    BSP_DDR_SECT_TYPE_WAN,
    BSP_DDR_SECT_TYPE_SHARE_MEM,
    BSP_DDR_SECT_TYPE_EXCP,
    BSP_DDR_SECT_TYPE_HIFI,
    BSP_DDR_SECT_TYPE_HARQ,
    BSP_DDR_SECT_TYPE_ZSP_UP,
    BSP_DDR_SECT_TYPE_HUTAF,
    BSP_DDR_SECT_TYPE_BBPSAMPLE,
    BSP_DDR_SECT_TYPE_TDS_LH2,
    BSP_DDR_SECT_TYPE_SOCP,
    BSP_DDR_SECT_TYPE_BUTTOM
}BSP_DDR_SECT_TYPE_E;

typedef enum tagBSP_DDR_SECT_ATTR_E
{
    BSP_DDR_SECT_ATTR_CACHEABLE = 0x0,
    BSP_DDR_SECT_ATTR_NONCACHEABLE,
    BSP_DDR_SECT_ATTR_BUTTOM
}BSP_DDR_SECT_ATTR_E;

typedef enum tagBSP_DDR_SECT_PVADDR_E
{
    BSP_DDR_SECT_PVADDR_EQU = 0x0,
    BSP_DDR_SECT_PVADDR_NOT_EQU,
    BSP_DDR_SECT_PVADDR_BUTTOM
}BSP_DDR_SECT_PVADDR_E;


typedef struct tagBSP_DDR_SECT_QUERY_S
{
    BSP_DDR_SECT_TYPE_E     enSectType;
    BSP_DDR_SECT_ATTR_E     enSectAttr;
    BSP_DDR_SECT_PVADDR_E   enPVAddr;
    unsigned int                 ulSectSize;
}BSP_DDR_SECT_QUERY_S;

typedef struct tagBSP_DDR_SECT_INFO_S
{
    BSP_DDR_SECT_TYPE_E    enSectType;
    BSP_DDR_SECT_ATTR_E    enSectAttr;
    void *                 pSectVirtAddr;
    void *                 pSectPhysAddr;
    unsigned int           ulSectSize;
}BSP_DDR_SECT_INFO_S;

/*****************************************************************************
* 函 数 名  : mdrv_get_fix_ddr_addr
*
* 功能描述  :DDR内存段查询接口。
*
* 输入参数  : pstSectQuery: 需要查询内存段的属性，类型
*             pstSectInfo:  查询到的内存段信息
*
* 输出参数  : 无
* 返 回 值  :  无
****************************************************************************/
int mdrv_get_fix_ddr_addr (BSP_DDR_SECT_QUERY_S *pstSectQuery, BSP_DDR_SECT_INFO_S *pstSectInfo);

/*****************************************************************************
* 函 数 名  : mdrv_virt_to_phy
*
* 功能描述  :虚地址转换成物理地址
*
* 输入参数  : lPAddr: 需要转化为物理地址的虚拟地址的地址值
*             mode:   模块属性
* 输出参数  : 无
* 返 回 值  : 物理地址值
****************************************************************************/
void*  mdrv_virt_to_phy (MEM_MODE_TYPE_E mode, void* lPAddr);

/*****************************************************************************
* 函 数 名  : mdrv_phy_to_virt
*
* 功能描述  :物理地址转换成虚拟地址
*
* 输入参数  : pMem: 需要进行转换的地址
*             mode: 模块属性
*
* 输出参数  : 无
* 返 回 值  : 虚拟地址值
****************************************************************************/
void*  mdrv_phy_to_virt (MEM_MODE_TYPE_E mode, void* pMem);

/*****************************************************************************
* 函 数 名  : mdrv_memcpy
*
* 功能描述  : 数据拷贝接口，底层基于汇编实现的最优化接口，只能用于memory到
*             memory的拷贝
* 约束条件  :
*             只能用于memory 到memory的拷贝，不能用于memory到寄存器的拷贝
*             需要4字节对齐
*             在A核 ARMv8 架构中，需要8字节对齐或者是normal memory 属性，
*             采用ioremap_wc映射的 uncachable 的内存 或者cachable的内存
*
* 输入参数  : dest :  拷贝的目的地址
*             src  :  拷贝的源地址
*             count:  拷贝数据的大小
* 输出参数  : 无
* 返 回 值  :  无
****************************************************************************/
void *mdrv_memcpy(void * dest, const void * src, unsigned long count);

#ifdef __cplusplus
}
#endif
#endif
