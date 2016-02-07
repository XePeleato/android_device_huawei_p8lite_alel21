/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  Drvinterface.c
*
*   作    者 :  c61362
*
*   描    述 :  底软给上层软件封装的接口层
*
*   修改记录 :  2012年3月2日 创建
*************************************************************************/
#include "BSP.h"
#include "soc_irqs.h"
#include <linux/module.h>

#include "soc_baseaddr_interface.h"
#include "soc_timer_interface.h"
#include "soc_peri_sctrl_interface.h"
#include <soc_ao_sctrl_interface.h>
#include "MemoryMap.h"
#include <linux/dma-mapping.h>
//#include <mach/bsp_timer.h>




/*************************内存虚实转换 start*********************************/

/*****************************************************************************
 函 数 名  : drv_uncache_mem_alloc
 功能描述  : 申请非cache的内存
 输入参数  : int size
 输出参数  : 无
 返 回 值  : void * 申请到内存的虚拟地址
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月5日
    作    者   : 王振伟 176398
    修改内容   : 新生成函数

*****************************************************************************/
void * drv_uncache_mem_alloc( int size )
{
    dma_addr_t phys_addr;	
	
#ifdef CONFIG_ARM64
    struct device        dev;
#endif

    if(size <= 0)
	{
        return NULL;
    }

    /*使用系统API或封装的接口申请uncache的内存*/
#ifdef CONFIG_ARM64
	memset(&dev, 0, sizeof(dev));
    return dma_alloc_coherent(&dev, size, &phys_addr, GFP_KERNEL); 
#else
   return dma_alloc_coherent(NULL, size, &phys_addr, GFP_KERNEL);
#endif
}
EXPORT_SYMBOL(drv_uncache_mem_alloc);
