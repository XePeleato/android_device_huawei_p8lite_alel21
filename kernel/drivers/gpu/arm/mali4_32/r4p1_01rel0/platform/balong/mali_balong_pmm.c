/*
 * Copyright (C) 2010-2012 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file mali_platform.c
 * Platform specific Mali driver functions for a default platform
 */
#include "mali_kernel_common.h"
#include "mali_osk.h"
#include "mali_balong_pmm.h"
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/pm.h>
#include <linux/mm.h>

#ifdef CONFIG_PM_RUNTIME
#include <linux/pm_runtime.h>
#endif
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/dma-mapping.h>
#include <linux/hisi/hisi_ion.h>
#include <linux/byteorder/generic.h>

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/string.h>
#endif

#include "soc_media_sctrl_interface.h"
#include "soc_ao_sctrl_interface.h"
#include <linux/hisi/reg_ops.h>
#include "soc_peri_sctrl_interface.h"
#include <linux/mali/mali_utgard.h>

#if MALI_PWRON_NO_DRV
#include <asm/io.h>
#include <hisi/hi6xxx-platform.h>
#include "soc_pmctrl_interface.h"
#include "soc_irqs.h"
#endif

/*Globle Variant Defination begin--------------------------------*/
#if 0
static struct clk              *gpu_pll = NULL;
static struct clk              *sys_pll = NULL;
static struct clk              *media_pll = NULL;
#endif
static struct clk              *mali_clock = NULL;         /* mali clk IP */
static struct clk              *media_clk_g3d = NULL;

static struct regulator     *mali_regulator = NULL;         /* mali g3d regulator */
static u32                       s_uwDebugFsPowerDown = 1;
static mali_bool               g_swGpuPowerState = MALI_FALSE;                  /* globle power state,1 up ;0 down*/


static struct device_node *np = NULL;

static struct resource mali_gpu_resources_m400_mp1[] =
{
    MALI_GPU_RESOURCES_MALI400_MP1(MALI_BASE_ADDR, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID)
};

static struct resource mali_gpu_resources_m400_mp2[] =
{
    MALI_GPU_RESOURCES_MALI400_MP2(MALI_BASE_ADDR, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID)
};

static struct resource mali_gpu_resources_m450_mp4[] =
{
    MALI_GPU_RESOURCES_MALI450_MP4(MALI_BASE_ADDR, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID, MALI_IRQ_ID)
};

extern mali_bool mali_gpu_class_is_mali450;
/*****************************************************************************
 function name  : mali_os_suspend
 description    : os suspend
 input vars     : void
 output vars    : NA
 return value   : void
 calls          : mali_platform_power_mode_change

 called         : os

 history        :
  1.data        : 18/10/2012
    author      : C66698
    modify      : new

*****************************************************************************/
static int mali_os_suspend(struct device *device)
{
    int ret = 0;

    MALI_DEBUG_PRINT(3, ("mali_os_suspend() called\n"));

    if (NULL != device->driver &&
        NULL != device->driver->pm &&
        NULL != device->driver->pm->suspend)
    {
 
        /* Need to notify Mali driver about this event */
       ret = device->driver->pm->suspend(device);
    }

    mali_platform_power_mode_change(MALI_POWER_MODE_DEEP_SLEEP);

    return ret;
}

/*****************************************************************************
 function name  : mali_os_resume
 description    : os resume
 input vars     : void
 output vars    : NA
 return value   : void
 calls          : mali_platform_power_mode_change

 called         : os

 history        :
  1.data        : 18/10/2012
    author      : C66698
    modify      : new

*****************************************************************************/
static int mali_os_resume(struct device *device)
{
    int ret = 0;

    MALI_DEBUG_PRINT(3, ("mali_os_resume() called\n"));

    mali_platform_power_mode_change(MALI_POWER_MODE_ON);

    if (NULL != device->driver &&
        NULL != device->driver->pm &&
        NULL != device->driver->pm->resume)
    {

        /* Need to notify Mali driver about this event */
        ret = device->driver->pm->resume(device);
    }

#ifdef CONFIG_PM_RUNTIME
    MALI_DEBUG_PRINT(3, ("mali_os_resume() enable pm_runtime\n"));
    pm_runtime_disable(device);
    pm_runtime_set_active(device);
    pm_runtime_enable(device);
#endif

    return ret;
}

/*****************************************************************************
 function name  : mali_os_freeze
 description    : os freeze
 input vars     : void
 output vars    : NA
 return value   : void
 calls          : NA

 called         : os

 history        :
  1.data        : 18/10/2012
    author      : C66698
    modify      : new

*****************************************************************************/
static int mali_os_freeze(struct device *device)
{
    int ret = 0;

    MALI_DEBUG_PRINT(3, ("mali_os_freeze() called\n"));

    if (NULL != device->driver &&
        NULL != device->driver->pm &&
        NULL != device->driver->pm->freeze)
    {

        /* Need to notify Mali driver about this event */
        ret = device->driver->pm->freeze(device);
    }

    return ret;
}
/*****************************************************************************
 function name  : mali_os_thaw
 description    : os thaw
 input vars     : void
 output vars    : NA
 return value   : void
 calls          : NA

 called         : os

 history        :
  1.data        : 18/10/2012
    author      : C66698
    modify      : new

*****************************************************************************/
static int mali_os_thaw(struct device *device)
{
    int ret = 0;

    MALI_DEBUG_PRINT(3, ("mali_os_thaw() called\n"));

    if (NULL != device->driver &&
        NULL != device->driver->pm &&
        NULL != device->driver->pm->thaw)
    {

        /* Need to notify Mali driver about this event */
       ret = device->driver->pm->thaw(device);
    }

    return ret;
}

#ifdef CONFIG_PM_RUNTIME
/*****************************************************************************
 function name  : mali_runtime_suspend
 description    : runtime suspend
 input vars     : void
 output vars    : NA
 return value   : void
 calls          : mali_platform_power_mode_change

 called         : os

 history        :
  1.data        : 18/10/2012
    author      : C66698
    modify      : new

*****************************************************************************/
static int mali_runtime_suspend(struct device *device)
{
    int ret = 0;

    MALI_DEBUG_PRINT(3, ("mali_runtime_suspend() called\n"));

    if (NULL != device->driver &&
        NULL != device->driver->pm &&
        NULL != device->driver->pm->runtime_suspend)
    {
        /* Need to notify Mali driver about this event */
        ret = device->driver->pm->runtime_suspend(device);
    }

    mali_platform_power_mode_change(MALI_POWER_MODE_LIGHT_SLEEP);

    return ret;
}

/*****************************************************************************
 function name  : mali_runtime_resume
 description    : runtime resume
 input vars     : void
 output vars    : NA
 return value   : void
 calls          : mali_platform_power_mode_change

 called         : os

 history        :
  1.data        : 18/10/2012
    author      : C66698
    modify      : new

*****************************************************************************/
static int mali_runtime_resume(struct device *device)
{
    int ret = 0;

    MALI_DEBUG_PRINT(3, ("mali_runtime_resume() called\n"));

    mali_platform_power_mode_change(MALI_POWER_MODE_ON);

    if (NULL != device->driver &&
        NULL != device->driver->pm &&
        NULL != device->driver->pm->runtime_resume)
    {
        /* Need to notify Mali driver about this event */
        ret = device->driver->pm->runtime_resume(device);
    }

    return ret;
}

/*****************************************************************************
 function name  : mali_runtime_resume
 description    : runtime resume
 input vars     : void
 output vars    : NA
 return value   : void
 calls          : mali_platform_power_mode_change

 called         : os

 history        :
  1.data        : 18/10/2012
    author      : C66698
    modify      : new

*****************************************************************************/
static int mali_runtime_idle(struct device *device)
{

    MALI_DEBUG_PRINT(3, ("mali_runtime_idle() called\n"));

    if (NULL != device->driver &&
        NULL != device->driver->pm &&
        NULL != device->driver->pm->runtime_idle)
    {
        /* Need to notify Mali driver about this event */
        int ret = device->driver->pm->runtime_idle(device);
        if (0 != ret)
        {
            return ret;
        }
    }

    pm_runtime_suspend(device);

    return 0;
}
#endif

/*Globle Variant Defination end----------------------------------*/


mali_bool mali_clk_get(struct platform_device *pdev)
{
    MALI_DEBUG_PRINT(3, ("mali_clk_get begin\n"));

#if 0
    /* open and get the clk and regulator without mali pmm */
    gpu_pll      = clk_get(&pdev->dev,"clk_gpupll_src");

    if (IS_ERR(gpu_pll))
    {
        MALI_PRINT( ("MALI Error : failed to get source gpu_pll\n"));
        return _MALI_OSK_ERR_FAULT;
    }

    /* open and get the clk and regulator without mali pmm */
    sys_pll      = clk_get(&pdev->dev,"clk_syspll_src");

    if (IS_ERR(sys_pll))
    {
        MALI_PRINT( ("MALI Error : failed to get source sys_pll\n"));
        return _MALI_OSK_ERR_FAULT;
    }

    /* open and get the clk and regulator without mali pmm */
    media_pll      = clk_get(&pdev->dev,"clk_medpll_src");

    if (IS_ERR(media_pll))
    {
        MALI_PRINT( ("MALI Error : failed to get source media_pll\n"));
        return _MALI_OSK_ERR_FAULT;
    }
#endif

    /* open and get the clk and regulator without mali pmm */
    mali_clock      = clk_get(&pdev->dev,"clk_g3d");

    if (IS_ERR(mali_clock))
    {
        MALI_PRINT( ("MALI Error : failed to get source CLK_G3D\n"));
        return _MALI_OSK_ERR_FAULT;
    }
    
    media_clk_g3d = clk_get(&pdev->dev,"pclk_g3d");

    if (IS_ERR(media_clk_g3d))
    {
        MALI_PRINT( ("MALI Error : failed to get source PCLK_G3D\n"));
        return _MALI_OSK_ERR_FAULT;
    }

    MALI_DEBUG_PRINT(3, ("mali_clk_get end\n"));
    return MALI_TRUE;
}


void mali_clk_put(void)
{
    MALI_DEBUG_PRINT(3, ("mali_clk_put begin\n"));
    if (NULL != mali_clock)
    {
        clk_put(mali_clock);
    }

    if (NULL != media_clk_g3d)
    {
        clk_put(media_clk_g3d);
    }

#if 0
    if (NULL != gpu_pll)
    {
        clk_put(gpu_pll);
    }

    if (NULL != media_pll)
    {
        clk_put(media_pll);
    }

    if (NULL != sys_pll)
    {
        clk_put(sys_pll);
    }
#endif

    MALI_DEBUG_PRINT(3, ("mali_clk_put end\n"));

}


_mali_osk_errcode_t mali_clock_on(void)
{
    int err;
    int core_freq = 0;
    int pclk_freq = 0;

#if 0
    if( IS_ERR_OR_NULL(gpu_pll) )
    {
        MALI_DEBUG_PRINT(1, ("****error on mali_clock_on : gpu_pll is null\n"));
        return _MALI_OSK_ERR_FAULT;
    }

    err = clk_prepare_enable(gpu_pll);
    if (0 != err)
    {
        MALI_DEBUG_PRINT(1, ("****error on mali_clock_on : gpu_pll enable error!\n"));
        return _MALI_OSK_ERR_FAULT;
    }
        
    if( IS_ERR_OR_NULL(media_pll) )
    {
        MALI_DEBUG_PRINT(1, ("****error on mali_clock_on : media_pll is null\n"));
        return _MALI_OSK_ERR_FAULT;
    }

    err = clk_prepare_enable(media_pll);
    if (0 != err)
    {
        MALI_DEBUG_PRINT(1, ("****error on mali_clock_on : media_pll enable error!\n"));
        return _MALI_OSK_ERR_FAULT;
    }
#endif

    if( IS_ERR_OR_NULL(media_clk_g3d))
    {
        MALI_DEBUG_PRINT(1, ("****error on mali_clock_on : media_clk_g3d is null\n"));
        return _MALI_OSK_ERR_FAULT;
    }

    err = clk_prepare_enable(media_clk_g3d);
    if (0 != err)
    {
        MALI_DEBUG_PRINT(1, ("****error on mali_clock_on : media_clk_g3d enable error!\n"));
        return _MALI_OSK_ERR_FAULT;
    }

    err = of_property_read_u32(np, "pclk_freq",&pclk_freq);
    
    if (err) {
        MALI_DEBUG_PRINT(1, ("ERROR: get mali pclk freq failed!\n"));
        return _MALI_OSK_ERR_FAULT;
    }
    
    if(0 != clk_set_rate(media_clk_g3d, (pclk_freq) * GPU_MHZ))
    {        
        MALI_DEBUG_PRINT(1, ("ERROR: fail to set media_clk_g3d!\n"));
    }

    if( IS_ERR_OR_NULL(mali_clock))
    {
        MALI_DEBUG_PRINT(1, ("****error on mali_clock_on : mali_clock is null\n"));
        return _MALI_OSK_ERR_FAULT;
    }

    err = of_property_read_u32(np, "mali_def_freq",&core_freq);
    
    if (err) {
        MALI_DEBUG_PRINT(1, ("ERROR: get mali default freq failed!\n"));
        return _MALI_OSK_ERR_FAULT;
    }

    if(0 != clk_set_rate(mali_clock, core_freq * GPU_MHZ))
    {
        MALI_DEBUG_PRINT(1, ("ERROR: fail to set mali_clock!\n"));
    }

    err = clk_prepare_enable(mali_clock);
    if (0 != err)
    {
        MALI_DEBUG_PRINT(1, ("****error on mali_clock_on : mali_clock enable error!\n"));
        return _MALI_OSK_ERR_FAULT;
    }
    
    MALI_DEBUG_PRINT(3, ("mali_clock_on syspll div off\n"));
    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_ADDR(0),17,17,1);
    
    MALI_DEBUG_PRINT(3, ("mali_clock_on finish\n"));
    return _MALI_OSK_ERR_OK;
}

void mali_clock_off(void)
{
    MALI_DEBUG_PRINT(3, ("mali_clock_off begin\n"));
    if( IS_ERR_OR_NULL(mali_clock) )
    {
        MALI_DEBUG_PRINT(1, ("error  mali_clock_off: mali_clock is null\n"));
        return;
    }

    clk_disable_unprepare(mali_clock);
/*
    if( IS_ERR_OR_NULL(media_clk_g3d) )
    {
        MALI_DEBUG_PRINT(1, ("error  mali_clock_off: media_clk_g3d is null\n"));
        return;
    }

    clk_disable_unprepare(media_clk_g3d);
*/
#if 0
    if( IS_ERR_OR_NULL(gpu_pll))
    {
        MALI_DEBUG_PRINT(1, ("error  mali_clock_off: gpu_pll is null\n"));
        return;
    }

    clk_disable_unprepare(gpu_pll);

    if( IS_ERR_OR_NULL(media_pll))
    {
        MALI_DEBUG_PRINT(1, ("error  mali_clock_off: media_pll is null\n"));
        return;
    }

    clk_disable_unprepare(media_pll);
#endif

    MALI_DEBUG_PRINT(3, ("mali_clock_off  end\n"));
}


void mali_domain_powerup_finish(void)
{
    unsigned int ret = 0;

    MALI_DEBUG_PRINT(3, ("mali power up start! \n"));
    
    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_RSTDIS0_ADDR(0),1,1,1);
    ret = phy_reg_readl(SOC_AO_SCTRL_BASE_ADDR, SOC_AO_SCTRL_SC_PW_RST_STAT0_ADDR(0), 1, 1);
    if(0 != ret)
    {
        MALI_DEBUG_PRINT(2, (" error:  SET SC_PW_RSTDIS0 failed!\n"));
    }

    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_ISODIS0_ADDR(0),1,1,1);
    ret = phy_reg_readl(SOC_AO_SCTRL_BASE_ADDR, SOC_AO_SCTRL_SC_PW_ISO_STAT0_ADDR(0), 1, 1);
    if(0 != ret)
    {
        MALI_DEBUG_PRINT(2, (" error:  SET SC_PW_ISODIS0 failed!\n"));
    }
    
    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_CLKEN0_ADDR(0),1,1,1);
    ret = phy_reg_readl(SOC_AO_SCTRL_BASE_ADDR, SOC_AO_SCTRL_SC_PW_CLK_STAT0_ADDR(0), 1, 1);
    if(1 != ret)
    {
        MALI_DEBUG_PRINT(2, (" error:  SET SC_PW_CLKEN0 failed!\n"));
    }
    
    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(0),0,0,1);
    ret = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_ADDR(0), 0, 0);
    if(0 != ret)
    {
        MALI_DEBUG_PRINT(2, (" error:  SET SC_MEDIA_RSTDIS failed!\n"));
    }
    
    MALI_DEBUG_PRINT(3, ("mali power up end! \n"));
}

void mali_domain_powerdown_begin(void)
{   
    MALI_DEBUG_PRINT(3, ("mali power down start! \n"));
    
    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(0),0,0,1);

    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_ADDR(0),1,1,1);

    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_CLKDIS0_ADDR(0),1,1,1);

    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_ISOEN0_ADDR(0),1,1,1);

    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_RSTEN0_ADDR(0),1,1,1);
    
    MALI_DEBUG_PRINT(3, ("mali power down end! \n"));
}


void mali_regulator_enable(void)
{
    int i, ret;
    if( IS_ERR_OR_NULL(mali_regulator) )
    {
        MALI_DEBUG_PRINT(1, ("error on mali_regulator_enable : g3d_regulator is null\n"));
        return;
    }

    ret = regulator_enable(mali_regulator);
    if(ret < 0)
    {
        return;
    }
    for (i = 0; i < 50; i++)
    {
        ret = phy_reg_readl(SOC_AO_SCTRL_BASE_ADDR, SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_ADDR(0), 1, 1);
        if (1 == ret)
        {
            MALI_DEBUG_PRINT(3, ("MTCMOS open OK\n"));
            break;
        }
        udelay(1);
    }
    if (50 == i)
    {
        MALI_DEBUG_PRINT(2, ("****error: MTCMOS open failed\n"));
        return;
    }
}


void mali_regulator_disable(void)
{
    mali_domain_powerdown_begin();

    if( IS_ERR_OR_NULL(mali_regulator) )
    {
        MALI_DEBUG_PRINT(1, ("error on mali_regulator_disable : g3d_regulator is null\n"));
        return;
    }

    regulator_disable(mali_regulator);
    
    MALI_DEBUG_PRINT(3, ("regulator_disable\n"));
}


static mali_bool init_mali_clock_regulator(struct platform_device *pdev)
{
    mali_bool ret       = MALI_TRUE;

    g_swGpuPowerState   = MALI_TRUE;

    /* regulator init */
    mali_regulator  = regulator_get(&pdev->dev, "G3D_PD_VDD");

    if (IS_ERR(mali_regulator))
    {
        MALI_PRINT( ("MALI Error : failed to get G3D_PD_VDD\n"));
        return MALI_FALSE;
    }

    mali_regulator_enable();

    /* clk init */

    if (mali_clock != 0)
    {
        return ret;
    }
    
    if (!mali_clk_get(pdev))
    {
        MALI_PRINT(("MALI Error: Failed to get Mali clock\n"));
        return MALI_FALSE;
    }

 
    /*使能媒体外设时钟*/
    ret = phy_reg_readl(SOC_PERI_SCTRL_BASE_ADDR, SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_ADDR(0), 10, 10);
    if(1 != ret)
    {
        phy_reg_writel(SOC_PERI_SCTRL_BASE_ADDR,SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_ADDR(0),10,10,1);
        ret = phy_reg_readl(SOC_PERI_SCTRL_BASE_ADDR, SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_ADDR(0), 10, 10);
        if(1 != ret)
        {
            MALI_DEBUG_PRINT(2, (" error:  SET SC_PERIPH_CLKEN12 failed!\n"));
        }            
    }
    
    /* CLK on and set rate */
    mali_clock_on();

    MALI_DEBUG_PRINT(2, (" init mali clock regulator ok\n"));

    /*时钟有效指示*/
    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR(0),15,15,1);
    ret = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR(0), 15, 15);
    if(1 != ret)
    {
        MALI_DEBUG_PRINT(2, (" error:  SET SC_MEDIA_CLKCFG2 failed!\n"));
    }
    
    mali_domain_powerup_finish();
    
    return MALI_TRUE;

}
static mali_bool deinit_mali_clock_regulator(void)
{
    if ( (NULL == mali_clock) || (NULL == mali_regulator))
    {
        return MALI_TRUE;
    }

    /* powerdown */
    mali_platform_powerdown();

    /* CLK */
    mali_clk_put();
    
    /* REGULATOR */
    regulator_put(mali_regulator);
    
    return MALI_TRUE;
}
void mali_gpu_utilization_handler(u32 utilization)
{
    /*DVFS when power on*/

    if ( MALI_TRUE == g_swGpuPowerState )
    {
    #if MALI_DVFS_ENABLED
    mali_dvfs_handler(utilization);
    #endif
    }
}

/*****************************************************************************
 function name  : mali_gpu_utilization_proc
 description    : mali_gpu_utilization_proc
 input vars     : void
 output vars    : NA
 return value   : void
 calls          : NA

 called         : os

 history        :
  1.data        : 20/11/2013
    author      : C66698
    modify      : new

*****************************************************************************/
static void mali_gpu_utilization_proc(struct mali_gpu_utilization_data *data)
{
    mali_gpu_utilization_handler(data->utilization_gpu);
}

static struct mali_gpu_device_data mali_gpu_data =
{   
    //.dedicated_mem_start = PLAT_MEM_GPU_ADDR,
    //.dedicated_mem_size = PLAT_MEM_GPU_SIZE,
    .shared_mem_size = 1024 * 1024 * 1024, /* 1024MB */
    .fb_start = MALI_FRAME_BUFFER_ADDR,
    .fb_size = MALI_FRAME_BUFFER_SIZE,
    .max_job_runtime = 2000, /* 2 seconds time out */
    .utilization_interval = 50,        /* 50ms */
    .utilization_callback = mali_gpu_utilization_proc,
};

static struct dev_pm_ops mali_gpu_device_type_pm_ops =
{
    .suspend = mali_os_suspend,
    .resume = mali_os_resume,
    .freeze = mali_os_freeze,
    .thaw = mali_os_thaw,
#ifdef CONFIG_PM_RUNTIME
    .runtime_suspend = mali_runtime_suspend,
    .runtime_resume = mali_runtime_resume,
    .runtime_idle = mali_runtime_idle,
#endif
};

static struct device_type mali_gpu_device_device_type =
{
    .pm = &mali_gpu_device_type_pm_ops,
};

extern unsigned long hisi_reserved_fb_phymem;

/* init the config.h with dynamic mem of hisi */
void mali_hisi_mem_init(void)
{    
    struct ion_heap_info_data mem_data;
    
    if (0 != hisi_ion_get_heap_info(ION_FB_HEAP_ID, &mem_data)) {
        MALI_PRINT( ("MALI Error : failed to get ION_FB_HEAP_ID \n"));
        return;
    }

    if (0 == mem_data.heap_size) {
        MALI_PRINT( ("MALI Error : fb size is 0! \n"));
        return;
    }

    mali_gpu_data.fb_size = mem_data.heap_size;//PLAT_MEM_FB_SIZE;
    mali_gpu_data.fb_start = (unsigned long)(mem_data.heap_phy);//hisi_reserved_fb_phymem;
    MALI_DEBUG_PRINT(2, ("mali_hisi_mem_init(): fb_size 0x%x, mali_gpu_data.fb_start 0x%x\n",  mali_gpu_data.fb_size,mali_gpu_data.fb_start));
}

static mali_core_type mali_get_gpu_type(void)
{
    u32 gpu_type = MALI_CORE_TYPE_MAX;
    int err = of_property_read_u32(np, "mali_type", &gpu_type);

    MALI_DEBUG_PRINT(2,("dts get gpu type %lu\n", gpu_type));

    if (err)
    {
        MALI_DEBUG_PRINT(1, ("mali_get_gpu_type() failed\n"));
        return MALI_CORE_TYPE_MAX;
    }
    
    return (mali_core_type)gpu_type;
}


_mali_osk_errcode_t mali_platform_powerdown(void)
{

    MALI_DEBUG_PRINT(3,("before mali_platform_powerdown state %x \n", g_swGpuPowerState));

    /* debug fs Powerdown */
    if (0==s_uwDebugFsPowerDown)
        MALI_SUCCESS;

    if (g_swGpuPowerState != MALI_FALSE)
    {
        MALI_DEBUG_PRINT( 3,("disable clock and power\n"));

        /*clk and regulator off*/
        mali_regulator_disable();
        
        mali_clock_off();

        g_swGpuPowerState = MALI_FALSE;
        /*info DVFS to stop*/
        #if MALI_DVFS_ENABLED
        pwrctrl_dfs_gpu_disable();
        #endif
    }
    else
    {
        MALI_PRINT(("mali_platform_powerdown gpu_power_state == 0 \n"));
    }

    MALI_DEBUG_PRINT(3,("after mali_platform_powerdown state %x \n", g_swGpuPowerState));
    MALI_SUCCESS;   
}
_mali_osk_errcode_t mali_platform_powerup(void)
{

    MALI_DEBUG_PRINT(3,("power up is called in mali_platform_powerup state %x \n", g_swGpuPowerState));

    if (MALI_FALSE == g_swGpuPowerState)
    {
        MALI_DEBUG_PRINT(3,("enable clock and power\n"));
        
        mali_regulator_enable();
        
        mali_clock_on();
        
        mali_domain_powerup_finish();

        g_swGpuPowerState = MALI_TRUE;
        /*enable DVFS to run*/
        #if MALI_DVFS_ENABLED
        pwrctrl_dfs_gpu_enable();
        #endif
    }

    MALI_DEBUG_PRINT(3,("power up is called in mali_platform_powerup state2 %x \n", g_swGpuPowerState));
    MALI_SUCCESS;
}

#if MALI_PWRON_NO_DRV
static void mali_platform_powerup_no_drv(void)
{
    MALI_DEBUG_PRINT(2, ("mali_platform_powerup_no_drv start!\n"));

    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_ADDR(0),1,1,1);

    phy_reg_writel(SOC_PERI_SCTRL_BASE_ADDR,SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_ADDR(0),0,31,0x400);
    
    phy_reg_writel(SOC_PMCTRL_BASE_ADDR,SOC_PMCTRL_GPUPLLCTRL_ADDR(0),0,31,0x7801);

    phy_reg_writel(SOC_PMCTRL_BASE_ADDR,SOC_PMCTRL_SYSPLLCTRL_ADDR(0),0,31,0x7801);

    phy_reg_writel(SOC_PMCTRL_BASE_ADDR,SOC_PMCTRL_MEDPLLCTRL_ADDR(0),0,31,0x7801);

    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR(0),0,31,0x8100);

    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_ADDR(0),0,31,0x84);//0x84
    
    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_RSTDIS0_ADDR(0),1,1,1);

    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_ISODIS0_ADDR(0),1,1,1);

    phy_reg_writel(SOC_AO_SCTRL_BASE_ADDR,SOC_AO_SCTRL_SC_PW_CLKEN0_ADDR(0),1,1,1);

    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(0),0,0,1);

    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_ADDR(0),0,31,0x20002);

    MALI_DEBUG_PRINT(2, ("mali_platform_powerup_no_drv end!\n"));
}
#endif
_mali_osk_errcode_t mali_platform_init(struct platform_device *pdev)
{
#if MALI_PWRON_NO_DRV
    mali_platform_powerup_no_drv();
    MALI_SUCCESS;
#endif

    pdev->dev.coherent_dma_mask = DMA_BIT_MASK(32);
    pdev->dev.type              = &mali_gpu_device_device_type;
    pdev->dev.platform_data     = &mali_gpu_data;
	pdev->dev.dma_mask 			= &pdev->dev.coherent_dma_mask;

    np = pdev->dev.of_node;
    
    switch(mali_get_gpu_type())
    {
        case MALI_CORE_400_MP1:
            pdev->num_resources         = ARRAY_SIZE(mali_gpu_resources_m400_mp1);
            pdev->resource              = mali_gpu_resources_m400_mp1;
            MALI_DEBUG_PRINT(2,("get gpu type MALI_CORE_400_MP1\n"));
            break;
        case MALI_CORE_400_MP2:
            pdev->num_resources         = ARRAY_SIZE(mali_gpu_resources_m400_mp2);
            pdev->resource              = mali_gpu_resources_m400_mp2;
            MALI_DEBUG_PRINT(2,("get gpu type MALI_CORE_400_MP2\n"));
            break;
        case MALI_CORE_450_MP4:
            mali_gpu_class_is_mali450 = MALI_TRUE;
            pdev->num_resources         = ARRAY_SIZE(mali_gpu_resources_m450_mp4);
            pdev->resource              = mali_gpu_resources_m450_mp4;    
            MALI_DEBUG_PRINT(2,("get gpu type MALI_CORE_450_MP4\n"));
            break;
        default:
            MALI_DEBUG_PRINT(1,("error get gpu type\n"));
            MALI_ERROR(_MALI_OSK_ERR_FAULT);
    }
    
    /* open and get the clk and regulator without mali pmm */
    init_mali_clock_regulator(pdev);

    #if MALI_DVFS_ENABLED
    pwrctrl_g3d_dfs_init(pdev);
    
    mali_dfs_target_profile(0);
    
    /* init the dvfs AND work queue */
    
    mali_dvfs_status_init(MALI_DVFS_HOLD);

    #endif

    
#ifdef CONFIG_PM_RUNTIME
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
    pm_runtime_set_autosuspend_delay(&(pdev->dev), 1);
    pm_runtime_use_autosuspend(&(pdev->dev));
#endif
    pm_runtime_enable(&pdev->dev);
#endif
    
    
    MALI_SUCCESS;   
}



_mali_osk_errcode_t mali_platform_deinit(void)
{
    #if MALI_DVFS_ENABLED
    mali_dvfs_status_deinit();
    #endif

    /* release clk and regulator */
    deinit_mali_clock_regulator();

    MALI_SUCCESS; 
}


mali_bool get_gpu_power_status(void)
{
    return g_swGpuPowerState;
}
_mali_osk_errcode_t mali_platform_power_mode_change(mali_power_mode power_mode)
{
    switch (power_mode)
    {
        case MALI_POWER_MODE_ON:
            if (MALI_FALSE == g_swGpuPowerState)
            {
                mali_platform_powerup(); /*power up include clk up*/
            }
        break;
        case MALI_POWER_MODE_LIGHT_SLEEP:
        case MALI_POWER_MODE_DEEP_SLEEP:

            if (MALI_TRUE == g_swGpuPowerState)
            {
                mali_platform_powerdown();/*power down include clk down*/
            }

        break;
        default:
            MALI_DEBUG_PRINT( 3,("error input to mali_platform_power_mode_change\n"));
        break;
    }

    MALI_SUCCESS;   
}

#ifdef CONFIG_DEBUG_FS

#define MALI_POWERDOWN_DEBUGFS_FILE_NAME "mit1_gpu_powerdown"

int __init mali_powerdown_debugfs_init(void)
{
    struct dentry *mit1_debugfs_dir = NULL;

    printk("mali_powerdown_debugfs_init\n");

    debugfs_create_u32(MALI_POWERDOWN_DEBUGFS_FILE_NAME, 0664, mit1_debugfs_dir,  &s_uwDebugFsPowerDown);
    return 0;
}
module_init(mali_powerdown_debugfs_init);

#endif

