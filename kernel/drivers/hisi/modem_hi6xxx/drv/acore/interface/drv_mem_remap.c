/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_mem_remap.c
  版 本 号   : 初稿
  作    者   : c61362
  生成日期   : 2012年3月2日
  最近修改   :
  功能描述   : 底软给上层软件封装的接口层
  修改历史   :
  1.日    期   : 2012年3月2日
    作    者   : c61362
    修改内容   : 新建Drvinterface.c

  2.日    期   : 2013年2月19日
    作    者   : 蔡喜 220237
    修改内容   : 由Drvinterface.c拆分所得

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BSP.h"

#if (defined BSP_CORE_APP)
#include "soc_irqs.h"
#include <linux/module.h>
#include "soc_baseaddr_interface.h"
#include "soc_timer_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "MemoryMap.h"
#include <linux/dma-mapping.h>

#endif


#include <soc_ao_sctrl_interface.h>
#include "general_sram_map.h"

#include <mdrv_memory.h>
#include  <linux/hisi/hi6xxx-iomap.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/

#define NOT_EXIST 0

#if (defined BSP_CORE_APP)
/* 全局的结构体，记录DDR内存段分配信息 */
BSP_DDR_SECT_INFO_S g_stDdrSectInfo[] = {
    {BSP_DDR_SECT_TYPE_TTF,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)NOT_EXIST,            NOT_EXIST},
    {BSP_DDR_SECT_TYPE_ARMDSP,    BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)NOT_EXIST, NOT_EXIST},
    {BSP_DDR_SECT_TYPE_UPA,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_UPA_BASE_ADDR,            ECS_UPA_SIZE},
    {BSP_DDR_SECT_TYPE_CQI,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_CQI_BASE_ADDR,            ECS_CQI_SIZE},
    {BSP_DDR_SECT_TYPE_APT,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_APT_BASE_ADDR,            ECS_APT_SIZE},
    {BSP_DDR_SECT_TYPE_ET,        BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_ET_BASE_ADDR,             ECS_ET_SIZE},
    {BSP_DDR_SECT_TYPE_BBPMASTER, BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_BBP_MASTER_BASE_ADDR,     ECS_BBP_MASTER_SIZE},
    {BSP_DDR_SECT_TYPE_NV,        BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_NV_BASE_ADDR,             ECS_NV_SIZE},
    {BSP_DDR_SECT_TYPE_DICC,      BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_TTF_DICC_ADDR,            ECS_TTF_DICC_SIZE},
    {BSP_DDR_SECT_TYPE_WAN,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)CORESHARE_MEM_WAN_ADDR,       CORESHARE_MEM_WAN_SIZE},
    {BSP_DDR_SECT_TYPE_SHARE_MEM, BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)GLOBAL_MEM_CORE_SHARE_ADDR,   GLOBAL_MEM_CORE_SHARE_SIZE},
    {BSP_DDR_SECT_TYPE_EXCP,      BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)GLOBAL_MEM_EXCP_BASE_ADDR,    GLOBAL_MEM_EXCP_SIZE},
    {BSP_DDR_SECT_TYPE_HIFI,      BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)GLOBAL_MEM_HIFI_BASE_ADDR,    GLOBAL_MEM_HIFI_SIZE},
    {BSP_DDR_SECT_TYPE_TDS_LH2,   BSP_DDR_SECT_ATTR_NONCACHEABLE, NULL,                   NULL,                            0},
    {BSP_DDR_SECT_TYPE_BUTTOM,    0,                              NULL,                   NULL,                         0}
};

#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : BSP_DDR_GetSectInfo
 功能描述  : DDR内存段查询接口
 输入参数  : pstSectQuery: 需要查询的内存段类型、属性
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_DDR_GetSectInfo(BSP_DDR_SECT_QUERY_S *pstSectQuery, BSP_DDR_SECT_INFO_S *pstSectInfo)
{
    int i = 0;
    int NUM_TMP = sizeof(g_stDdrSectInfo)/sizeof(g_stDdrSectInfo[0]);
    if((BSP_NULL == pstSectQuery) || (BSP_NULL == pstSectInfo))
    {
        return BSP_ERROR;
    }

    for (i = 0;i < NUM_TMP;i++)
    {

        if (pstSectQuery->enSectType == g_stDdrSectInfo[i].enSectType)
        {
            memcpy((void *)pstSectInfo, (const void *)(&g_stDdrSectInfo[i]), sizeof(BSP_DDR_SECT_INFO_S));
#if (defined BSP_CORE_APP)
            if (pstSectInfo->pSectPhysAddr != 0)
            {
                pstSectInfo->pSectVirtAddr = (BSP_VOID*)HISI_VA_ADDRESS(pstSectInfo->pSectPhysAddr);
            }
#endif
            return BSP_OK;
        }
    }

    return BSP_ERROR;
}
/*************************内存虚实转换 start*********************************/
/*****************************************************************************
 函 数 名  : DRV_DDR_VIRT_TO_PHY
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
void * DRV_DDR_VIRT_TO_PHY(void* ulVAddr)
{

    return (void*)HISI_PA_ADDRESS((unsigned long)ulVAddr);
}

/*****************************************************************************
 函 数 名  : DRV_DDR_PHY_TO_VIRT
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
void* DRV_DDR_PHY_TO_VIRT(void* ulPAddr)
{
    return (void*)HISI_VA_ADDRESS((unsigned long)ulPAddr);
}

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
int mdrv_get_fix_ddr_addr (BSP_DDR_SECT_QUERY_S *pstSectQuery, BSP_DDR_SECT_INFO_S *pstSectInfo)
{
    return BSP_DDR_GetSectInfo(pstSectQuery, pstSectInfo);
}
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
void*  mdrv_virt_to_phy (MEM_MODE_TYPE_E mode, void* lPAddr)
{
    switch (mode){
        case MEM_DDR_MODE:
            return DRV_DDR_VIRT_TO_PHY(lPAddr);
        default:
            return NULL;

    }
}

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
void*  mdrv_phy_to_virt (MEM_MODE_TYPE_E mode, void* pMem)
{
    switch (mode){
        case MEM_DDR_MODE:
            return DRV_DDR_PHY_TO_VIRT(pMem);
        default:
            return NULL;
    }
}

/*****************************************************************************
* 函 数 名  : mdrv_memcpy
*
* 功能描述  :数据拷贝接口。底层基于汇编实现的最优化接口，目前在v8r1上使用。
*
* 输入参数  : dest :  拷贝的目的地址
*             src  :  拷贝的源地址
*             count:  拷贝数据的大小
* 输出参数  : 无
* 返 回 值  :  无
****************************************************************************/
void *mdrv_memcpy(void * dest, const void * src, unsigned long count)
{
    memcpy(dest,src,count);
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

