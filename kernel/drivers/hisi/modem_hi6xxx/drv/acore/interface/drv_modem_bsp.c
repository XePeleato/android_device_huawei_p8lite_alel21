/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_modem_bsp.c
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
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#include "stdio.h"

#include "osal.h"
#ifdef __OS_VXWORKS__
#include <vxWorks.h>
#include <private/vmLibP.h>
#include <cacheLib.h>
#include "arm_pbxa9.h"
#include <cacheLib.h>
#endif
#include "BSP_TIMER.h"
#include "sys_config.h"
#include "soc_baseaddr_interface.h"
#include "string.h"
#endif

#if (defined BSP_CORE_APP)
#include "soc_irqs.h"
#include <linux/module.h>
#include "MemoryMap.h"
#include <linux/dma-mapping.h>
#endif

#include "soc_baseaddr_interface.h"
#include "soc_timer_interface.h"
#if defined (CHIP_BB_HI6210)
#include "soc_peri_sctrl_interface.h"
#elif defined (CHIP_BB_HI6620)
#include "soc_sctrl_interface.h"
#endif
#include <soc_ao_sctrl_interface.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SOC_INVALID_BASE_ADDR 0

/* IP NUMBER DESC INFO */
typedef struct BSP_IPADDR_INFO_S
{
    BSP_IP_TYPE_E         ip_type;
    BSP_VOID *            ip_addr;
}BSP_IPADDR_INFO;

/* INTERPUTER NUMBER DESC INFO */
typedef struct BSP_INT_NUM_INFO_S
{
    BSP_INT_TYPE_E         int_type;
    BSP_S32                int_num;
}BSP_INT_NUM_INFO;

/*****************************************************************************
  2 函数声明
*****************************************************************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)

/* 全局变量，记录中断号 */
BSP_INT_NUM_INFO g_slIntNO[] = {
    {BSP_INT_TYPE_WDT,                      INT_VEC_WDT},
    {BSP_INT_TYPE_IPF_C,                    INT_VEC_IPF_C},
    {BSP_INT_TYPE_IPF_A,                    INT_VEC_IPF_A},
    {BSP_INT_TYPE_SOCP_A,                   INT_VEC_SOCP_A},
    {BSP_INT_TYPE_SOCP_C,                   INT_VEC_SOCP_C},
    {BSP_INT_TYPE_WBBP_BBPMST,              INT_VEC_WBBP_BBPMST_INTR},
    {BSP_INT_TYPE_GBBP,                     INT_VEC_GBBP_GSM1_NODRX_INTR},
    {BSP_INT_TYPE_GBBP_AWAKE,               INT_VEC_GBBP_GSM1_DRX_INTR},
    {BSP_INT_TYPE_WBBP_0MS,                 INT_VEC_WBBP_CARD1_0MS_INTR},
    {BSP_INT_TYPE_WBBP1_0MS,                INT_VEC_WBBP_CARD2_0MS_INTR},
    {BSP_INT_TYPE_WBBP_AWAKE,               INT_VEC_WDRX_SLP_AWK_SW_INTR},
    {BSP_INT_TYPE_WBBP_SWITCH,              INT_VEC_WBBP_CLK_SW_INTR},
    {BSP_INT_TYPE_UPACC_INTR,               INT_VEC_UPACC_ARM_INT},
    {BSP_INT_TYPE_UPACC_DSP,                INT_VEC_UPACC_DSP_INT},
    {BSP_INT_TYPE_CICOM_UL,                 INT_VEC_CICOM_UL},
    {BSP_INT_TYPE_CICOM_DL,                 INT_VEC_CICOM_DL},
#if defined(CHIP_BB_HI6210)
#else
    {BSP_INT_TYPE_CICOM1_UL,                INT_VEC_CICOM1_UL},
    {BSP_INT_TYPE_CICOM1_DL,                INT_VEC_CICOM1_DL},
#endif
    {BSP_INT_TYPE_GBBP_DSP,                 INT_VEC_GBBP_GSM1_RESERVED_INTR},
    {BSP_INT_TYPE_GBBP_GSML_RESERVED_INTR,  INT_VEC_GBBP_GSM1_DRX_WAKEUP_INTR},
    {BSP_INT_TYPE_GBBP_AWAKE_DSP,           INT_VEC_GBBP_GSM1_DRX_WAKEUP_RESERV},
    {BSP_INT_TYPE_GBBP1,                    INT_VEC_GBBP_GSM2_NODRX},
    {BSP_INT_TYPE_GBBP1_AWAKE,              INT_VEC_GBBP_GSM2_DRX_WAKEUP},
    {BSP_INT_TYPE_GBBP1_DSP,                INT_VEC_GBBP_GSM2_NODRX_RESERV},
    {BSP_INT_TYPE_GBBP1_AWAKE_DSP,          INT_VEC_GBBP_GSM2_DRX_WAKEUP_RESERV},
    {BSP_INT_TYPE_WBBP_SEARCH,              INT_VEC_WBBP_SEARCH_INTR},
    {BSP_INT_TYPE_WBBP_RAKE,                INT_VEC_WBBP_RAKE_INTR},
    {BSP_INT_TYPE_WBBP_DECODE,              INT_VEC_WBBP_DECODE_INTR},
    {BSP_INT_TYPE_WBBP_TIME,                INT_VEC_WBBP_CARD1_TIME_INTR},
    {BSP_INT_TYPE_WBBP1_TIME,               INT_VEC_WBBP_CARD2_TIME_INTR},
    {BSP_INT_TYPE_WBBP_MULTI_SEARCH,        INT_VEC_WBBP_MULTI_SEARCH_INTR},
    {BSP_INT_TYPE_CTU_INT_G,                INT_VEC_GBBP_CTU1_TO_ARM_INT},
    {BSP_INT_TYPE_CTU_INT_W,                INT_VEC_BBP_COMMON_CTU_W2ARM_INT},
    {BSP_INT_TYPE_INT_OSRTC,                INT_VEC_RTC},
    {BSP_INT_TYPE_INT_SMIM,                 INT_VEC_SMIM},
    {BSP_INT_TYPE_LTE_CIPHER,               INT_VEC_L_CIPHER_INTR},
    {BSP_INT_TYPE_LTE_APP_ARM_PUB,          INT_VEC_LTE_APP_ARM_PUB_INT},
    {BSP_INT_TYPE_LTE_ARM_POSITIONS,        INT_VEC_LTE_ARM_POSITION_INTS},
    {BSP_INT_TYPE_LTE_DL_DMA,               INT_VEC_LTE_DL_DMA_INTS},
    {BSP_INT_TYPE_LTE_ARM_POSITION,         INT_VEC_LTE_ARM_POSITION_INT},
    {BSP_INT_TYPE_LTE_ARM_DL_DMA,           INT_VEC_LTE_ARM_DL_DMA_INT},
    {BSP_INT_TYPE_LTE_ARM_CLK_SWITCH,       INT_VEC_LTE_ARM_CLK_SWITCH_INT},
    {BSP_INT_TYPE_LBBP_AWAKE,               INT_VEC_LTE_ARM_WAKEUP_INT},
    {BSP_INT_TYPE_LTE_PUB,                  INT_VEC_LTE_PUB_INT},
    {BSP_INT_TYPE_TDS_STU_ARM1,             INT_VEC_TDS_STU_ARM1_INT},
    {BSP_INT_TYPE_TBBP_AWAKE,               INT_VEC_TDS_DRX_ARM_INT1},
    {BSP_INT_TYPE_TDS_DRX_ARM2,             INT_VEC_TDS_DRX_ARM_INT2},
    {BSP_INT_TYPE_BBE16_GPIO1,              INT_VEC_BBE16_GPIO1_INT},
    {BSP_INI_TYPE_COMM_32K_INT,         INT_LVL_COMM_32K_INT},
    {BSP_INT_TYPE_BUTTOM,                   0xFFFFFFFF}
};
#endif

#if (defined BSP_CORE_APP)

#if defined (CHIP_BB_HI6210)
/* 全局变量，记录IP基地址 */
BSP_IPADDR_INFO g_ulIPBaseAddr[] = {
    {BSP_IP_TYPE_SOCP,      (BSP_VOID *)SOC_SOCP_BASE_ADDR},
    {BSP_IP_TYPE_CICOM0,    (BSP_VOID *)SOC_GUCipher_BASE_ADDR},
    {BSP_IP_TYPE_AHB,       (BSP_VOID *)ZSP_UP_ADDR},
    {BSP_IP_TYPE_SYSCTRL,   (BSP_VOID *)SOC_AO_SCTRL_BASE_ADDR},
    {BSP_IP_TYPE_BUTTOM,    (BSP_VOID *)SOC_INVALID_BASE_ADDR}
};
#endif

#define INT_LVL_INVALID (-1)
BSP_INT_NUM_INFO g_slIntNO[] = {
    {BSP_INT_TYPE_IPF_C,        IRQ_IPF0},
    {BSP_INT_TYPE_IPF_A,        IRQ_IPF1},
    {BSP_INT_TYPE_SOCP_A,       IRQ_SOCP0},
    {BSP_INT_TYPE_SOCP_C,       IRQ_SOCP1},
    {BSP_INT_TYPE_CTU_INT_W,    IRQ_TIMER2},
    {BSP_INT_TYPE_CTU_INT_TDS,  IRQ_WD1},
    {BSP_INT_TYPE_BUTTOM,       INT_LVL_INVALID}
};
#endif


#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#ifdef __OS_VXWORKS__
extern MMU_LIB_FUNCS mmuLibFuncs;
#endif
extern int BSP_DMR_ATAnalyze(void * pstMem,unsigned int uiSize, unsigned int * puiModIdSum);
extern int  max_freeblock_size_get(void);
extern BSP_VOID SendNMIInterrupt(unsigned int SocBitNO, unsigned int ZspBitNO);
extern unsigned int exchIntGetContext(void);
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/


/*****************************************************************************
 函 数 名  : mdrv_misc_get_ip_baseaddr
 功能描述  : IP基地址查询
 输入参数  : enIPType: 需要查询的IP类型
 输出参数  : 无
 返回值    ：查询到的IP基地址，如查不到返回NULL
*****************************************************************************/
void * mdrv_misc_get_ip_baseaddr(BSP_IP_TYPE_E enIPType)
{
    unsigned int retValue;
    switch(enIPType)
    {
    case BSP_IP_TYPE_SOCP:
        retValue = SOC_SOCP_BASE_ADDR;
        break;
    case BSP_IP_TYPE_CICOM0:
        retValue = SOC_GUCipher_BASE_ADDR;
        break;
#if (defined BSP_CORE_APP)        
    case BSP_IP_TYPE_SYSCTRL:
        retValue = SOC_AO_SCTRL_BASE_ADDR;
        break;
#endif
    case BSP_IP_TYPE_AHB:
        retValue = ZSP_UP_ADDR;
        break;
        
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    case BSP_IP_TYPE_SYSCTRL:
        retValue = SOC_MODEM_SCTRL_BASE_ADDR;
        break;
    case BSP_IP_TYPE_BBPMASTER:
        retValue = BBP_MASTER_REGBASE_ADDR;  
        break;
    case BSP_IP_TYPE_WBBP:
        retValue = WBBP_BASE_ADDR;
        break;
    case BSP_IP_TYPE_GBBP:
        retValue = GBBP_BASE_ADDR;
        break;
    case BSP_IP_TYPE_GBBP1:
        retValue = GBBP1_BASE_ADDR;
        break;
    case BSP_IP_TYPE_CTU:
        retValue = CTU_BASE_ADDR;
        break;
    case BSP_IP_TYPE_TDSSYS:
        retValue = TDSSYS_BASE_ADDR;
        break;
    case BSP_IP_TYPE_WBBP_DRX:
        retValue = WBBP_DRX_ADDR;
        break;
    case BSP_IP_TYPE_GBBP_DRX:
        retValue = GBBP_DRX_ADDR;
        break;
    case BSP_IP_TYPE_GBBP1_DRX:
        retValue = GBBP1_DRX_ADDR;
        break;
    case BSP_IP_TYPE_BBPDMA:
        retValue = BBP_DMA_BASE_ADDR;
        break;
    case BSP_IP_TYPE_BBPDBG:
        retValue = BBP_DBG_BASE_ADDR;
        break;
    case BSP_IP_TYPE_BBPSRC:
        retValue = BBP_SRC_BASE_ADDR;
        break;
#endif    
    default :
        retValue = SOC_INVALID_BASE_ADDR;
        break;
    }

    return (BSP_VOID *)retValue;
}

/*****************************************************************************
 函 数 名  : mdrv_int_get_num
 功能描述  : 中断号查询
 输入参数  : enIntType: 需要查询的中断类型
 输出参数  : 无
 返回值    ：查询到的中断号，如查不到返回BSP_ERROR
*****************************************************************************/
int mdrv_int_get_num(BSP_INT_TYPE_E enIntType)
{
    int i = 0;
    int NUM_TMP = sizeof(g_slIntNO)/sizeof(g_slIntNO[0]);
    for (i = 0;i < NUM_TMP;i++)
    {

        if (enIntType == g_slIntNO[i].int_type)
        {
            return g_slIntNO[i].int_num;
        }
    }

    return BSP_ERROR;
}

/************************************************************************
 * FUNCTION
 *       vmEnable
 * DESCRIPTION
 *       内存读写保护功能使能函数
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
int vmEnable(int enable)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return (OSAL_MmuEnable(enable));
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*****************************************************************************
 函 数 名  : BSP_DMR_ATANALYZE
 功能描述  : 供NAS查询动态内存占用信息，通过AT命令查询
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 成功0，失败-1
*****************************************************************************/
int BSP_DMR_ATANALYZE(void * pstMem,unsigned int uiSize, unsigned int * puiModIdSum)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#if( FEATURE_MEM_MONITOR == FEATURE_ON )
#ifdef __OS_VXWORKS__
    return BSP_DMR_ATAnalyze(pstMem, uiSize, puiModIdSum);
#else
    return 0;   /* 打桩 */
#endif
#else
    DRV_PRINTF("Function BSP_DMR_ATANALYZE unsupport!\n");
    return BSP_ERROR;
#endif
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*****************************************************************************
 函 数 名  : DRV_GET_FREE_BLOCK_SIZE
 功能描述  : get memory max free block size
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : max free block size
*****************************************************************************/
int DRV_GET_FREE_BLOCK_SIZE(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return max_freeblock_size_get();
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*****************************************************************************
 函 数 名  : BSP_SendNMIInterrupt
 功能描述  : 发送一个NMI中断
 输入参数  : NMI中断Bit位
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
BSP_VOID BSP_SendNMIInterrupt(unsigned int SocBitNO, unsigned int ZspBitNO)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    SendNMIInterrupt(SocBitNO, ZspBitNO);
#endif

#if (defined BSP_CORE_APP)
    return;   /* 打桩 */
#endif
}

/*****************************************************************************
 函 数 名  : BSP_CACHE_DataCacheFlush
 功能描述  :
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
void BSP_CACHE_DATA_FLUSH(void * addr, int size)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    /*BSP_CACHE_DataCacheFlush(addr, size);*/
    OSAL_CacheFlush(OSAL_DATA_CACHE,addr, size);
#endif

#if (defined BSP_CORE_APP)
    return;   /* 打桩 */
#endif
}

/*****************************************************************************
 函数名: mdrv_int_is_inside_context
 功能描述 : 判断当前任务是否在中断上下文
 输入参数 : 无
 输出参数 : 无
 返回值   ： 0 代表非中断
           ：1  代表中断
*****************************************************************************/
unsigned int mdrv_int_is_inside_context(void)
{
    return 0;   /* 打桩 */
}


BSP_S32 BSP_NANDF_Erase(BSP_U32 u32BlockID)
{
    return 0;
}

BSP_S32 BSP_NANDF_Read(BSP_VOID* pRamAddr, BSP_VOID* FlashAddr, BSP_U32 u32NumByte)
{
    return 0;
}

BSP_S32 BSP_NANDF_Write(BSP_VOID* FlashAddr, BSP_VOID *pRamAddr,  BSP_U32 u32NumByte)
{
    return 0;
}

/*stub,为了适配其它组件编译，打桩，等其它组件代码修改后删除*/
unsigned int free_mem_size_get(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return 0;
#endif
}

#if (defined BSP_CORE_APP)
EXPORT_SYMBOL(free_mem_size_get);
#endif



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

