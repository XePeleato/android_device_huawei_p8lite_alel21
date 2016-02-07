
#include <linux/module.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */

//#include <mach/hardware.h>
//#include <mach/system.h>
//#include <mach/irqs.h>
//#include <mach/io.h>
//#include <mach/platform.h>
#include "bsp_private_ipc.h"

#ifdef __cplusplus
extern "C" {
#endif


#define IPC2_NOSEC_CORE_ACPU 5

static unsigned long g_private_ipc_base = 0;
/*****************************************************************************
* 函 数 名  : BSP_DRV_PRIVATE_IPCIntInit
*
* 功能描述  : IPC模块初始化
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   wangjing  creat
*****************************************************************************/
int BSP_DRV_PRIVATE_IPCIntInit(void)
{
    /* make this funciton reentrant */
    if (g_private_ipc_base)
    {
        printk(KERN_INFO "private ipc is inited.\n");
        return 0;
    }

    g_private_ipc_base = (unsigned long)ioremap(SOC_IPC_BASE_ADDR,SZ_4K);
    if (!g_private_ipc_base)
    {
    	printk(KERN_ERR "ipc ioremap error.\n");
    	return -1;
    }

       /*写全0屏蔽所有互发源*/
    BSP_RegWr(SOC_IPC_CPU_INT_MASK_ADDR(g_private_ipc_base, IPC2_NOSEC_CORE_ACPU), 0x0);

    /*写全1清除所有中断*/
    BSP_RegWr(SOC_IPC_CPU_INT_CLR_ADDR(g_private_ipc_base, IPC2_NOSEC_CORE_ACPU), 0xFFFFFFFF);

    return 0;
}

void clear_private_ipc_int(unsigned int  enTarget, unsigned int enIntSrc)
{
    BSP_RegWr(SOC_IPC_CPU_INT_CLR_ADDR(g_private_ipc_base, enTarget), (0x1<<enIntSrc));
}


/*****************************************************************************
* 函 数 名  : BSP_IPC_IntEnable
*
* 功能描述  : 使能某个中断
*
* 输入参数  :   IPC_INT_CORE_E enCoreNum 要使能中断的core
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
int BSP_PRIVATE_IPC_IntEnable (unsigned int ulLvl)
{
    /*参数检查*/
    IPC_CHECK_PARA(ulLvl);

    /*写中断屏蔽寄存器*/
    BSP_RegWr(SOC_IPC_CPU_INT_EN_ADDR(g_private_ipc_base, IPC2_NOSEC_CORE_ACPU), (1 << ulLvl));

    return 0;
}


/*****************************************************************************
 * 函 数 名  : BSP_INT_Disable
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  : IPC_INT_CORE_E enCoreNum 要使能中断的core
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
int BSP_PRIVATE_IPC_IntDisable (NOSEC_IPC2_INT_LEV_E ulLvl)
{
    /*参数检查*/
    IPC_CHECK_PARA(ulLvl);

    /*写中断屏蔽寄存器*/
    BSP_RegWr(SOC_IPC_CPU_INT_DIS_ADDR(g_private_ipc_base, IPC2_NOSEC_CORE_ACPU), (1 << ulLvl));

    return 0;
}

#ifdef __cplusplus
}
#endif

