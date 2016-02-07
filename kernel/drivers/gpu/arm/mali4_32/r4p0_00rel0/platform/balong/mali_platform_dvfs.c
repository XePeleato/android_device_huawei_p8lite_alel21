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
 * @file mali_platform_dvfs.c
 * Platform specific Mali driver dvfs functions
 * dvfs by balong
 */
#include "mali_kernel_common.h"
#include "mali_osk.h"
#include "mali_balong_pmm.h"

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <linux/hisi/reg_ops.h>

#include "soc_pmctrl_interface.h"
#include "soc_media_sctrl_interface.h"
#include "soc_ao_sctrl_interface.h"

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/string.h>
#endif

#define DVFS_STEP_OVER          0
#define DVFS_FREQ_DIV(freq)    ((((freq) & 0xF) << 8) | (0x1 << 15))
#define MAX_CUNT                50  //最大检测次数
#define MID_DIV                 6   //异源调频时的过度频率
#define MAX_SFT_FREQ            50  //sft平台的最大频率
mali_dfs_global_var g_mali_dfs_var;
/*dvfs status*/
mali_dvfs_currentstatus g_stMaliDvfsStatus;

mali_dvfs_profile_table g_mali_dvfs_profile[5];//max profile step is 5

static mali_dvfs_threshold_table mali_dvfs_threshold[2]={
    /*step by step*/{((int)((255*40)/100)), ((int)((255*80)/100))},
    /*step over*/   {((int)((255*20)/100)), ((int)((255*90)/100))}};

/* utilization exchange */
static u32 s_auwUtilization[MALI_UTILIZATION_NUM]={0};

static struct workqueue_struct *mali_dvfs_wq = 0;

/*Declare the DVFS work method*/
static DECLARE_WORK(mali_dvfs_work, mali_dvfs_work_handler);

static void gpu_devfreq_init(void);
static void gpu_devfreq_set_maxfreq(void);
static void gpu_devfreq_set_minfreq(void);
static void gpu_devfreq_get_maxfreq(void);
static void gpu_devfreq_get_minfreq(void);
void mali_remark_clockrate(u32 profile_id)
{
    if(MALI_FALSE == get_gpu_power_status())
    {
        g_mali_dfs_var.dfs_GpuClkRate = 0;
    }
    else
    {
        g_mali_dfs_var.dfs_GpuClkRate = g_mali_dvfs_profile[profile_id].freq;
    }
}


u32 find_current_weight(u32 utilization)
{
    u32         weight;
    if (utilization <= (255*30/100))
    {
        weight  = 1;
    }
    else if (utilization > (255*95/100))
    {
        weight  = 5;
    }
    else
    {
        weight  = utilization*100/255/30;
    }

    return weight;
}


mali_dvfs_status get_mali_dvfs_status(void)
{
    return g_stMaliDvfsStatus.currentStep;
}


mali_dvfs_status decideNextStatus(void)
{
    mali_dvfs_status    dvfsStatus = MALI_DVFS_BUTT;
    u32                 utilization;
    int                 count;
    u32                 weight;
    u32                 sum_utilization=0;
    u32                 sum_weight=0;

    /*get the weighted utilization*/
    for (count = 0; count < MALI_UTILIZATION_NUM; count++ )
    {
        weight          = find_current_weight(s_auwUtilization[count]);

        sum_utilization += weight*s_auwUtilization[count];
        sum_weight      += weight;
    }

    utilization         = sum_utilization/sum_weight;

    /*get the decision*/
    if (utilization > mali_dvfs_threshold[1].upthreshold)
    {
        dvfsStatus      = MALI_DVFS_INCREASE_HIGHEST;
    }
    else if ( (utilization <= mali_dvfs_threshold[1].upthreshold)
            &&(utilization > mali_dvfs_threshold[0].upthreshold))
    {
        dvfsStatus      = MALI_DVFS_INCREASE;
    }
    else if ( (utilization <= mali_dvfs_threshold[0].downthreshold)
            &&(utilization > mali_dvfs_threshold[1].downthreshold))
    {
        dvfsStatus      = MALI_DVFS_DECREASE;
    }
    else if ( (utilization <= mali_dvfs_threshold[1].downthreshold))
    {
        dvfsStatus      = MALI_DVFS_DECREASE_LOWEST;
    }
    else
    {
        dvfsStatus      = MALI_DVFS_HOLD;
    }

    return dvfsStatus;
}


void mali_dvfs_status_update(u32 utilization)
{
    mali_dvfs_status curStatus  = MALI_DVFS_BUTT;
    mali_dvfs_status nextStatus = MALI_DVFS_BUTT;

    MALI_DEBUG_PRINT(3, ("mali_dvfs_status: utilization = %d with count %d \n",utilization,g_mali_dfs_var.dfs_StayCount));

    s_auwUtilization[g_mali_dfs_var.dfs_StayCount] = utilization;
    g_mali_dfs_var.dfs_StayCount++;

    /*count the dvfs utilization and cache them*/
    if (g_mali_dfs_var.dfs_StayCount < MALI_UTILIZATION_NUM)
    {
        /*wait for counter time out*/
        return;
    }

    g_mali_dfs_var.dfs_StayCount  = 0;

    /*decide next step*/
    curStatus   = get_mali_dvfs_status();
    nextStatus  = decideNextStatus();

    MALI_DEBUG_PRINT(3, ("nextStatus %d, currentStep %d \n", nextStatus, g_stMaliDvfsStatus.currentStep));

    /*if next status is same with current status, don't change anything*/
    if (nextStatus != MALI_DVFS_HOLD)
    {
        pwrctrl_dfs_gpu_target((s32)nextStatus - (s32)MALI_DVFS_HOLD);

        /*update the dvfs action status*/
        g_stMaliDvfsStatus.currentStep = nextStatus;
    }

    return;
}
void mali_dvfs_work_handler(struct work_struct *w)
{
    MALI_DEBUG_PRINT(3, ("=== mali_dvfs_work_handler\n"));

    mali_dvfs_status_update(g_mali_dfs_var.dfs_Utilization);
}


mali_bool mali_dvfs_status_init(mali_dvfs_status maliDvfsStatus)
{
    /*default status
    add here with the right function to get initilization value.
    */
    if (!mali_dvfs_wq)
    {
        mali_dvfs_wq = (struct workqueue_struct *)create_singlethread_workqueue("mali_dvfs");
    }
    else
    {
        return MALI_FALSE;
    }
    /*add a error handling here*/
    g_stMaliDvfsStatus.currentStep = maliDvfsStatus;

    return MALI_TRUE;
}
void mali_dvfs_status_deinit(void)
{
    if (mali_dvfs_wq)
    {
        destroy_workqueue(mali_dvfs_wq);
    }

    mali_dvfs_wq = NULL;
}


void mali_dvfs_handler(u32 utilization)
{
    g_mali_dfs_var.dfs_Utilization = utilization;
    queue_work_on(0, mali_dvfs_wq,&mali_dvfs_work);
    g_mali_dfs_var.dfs_GpuUtilization = utilization;
    /*add error handle here*/
}
void pwrctrl_g3d_dfs_init(struct platform_device *pdev)
{
    int ret;
    u32 pro_num = 0;
    struct device *dev = NULL;
    struct device_node *np = NULL;
    unsigned int dfs_prf_table[3] = {};
    mali_dvfs_profile_table* dfs_prf = NULL;
    u32 id = 0;

    g_mali_dfs_var.dfs_BakPrf = 0;
    g_mali_dfs_var.dfs_DfsOn = 1;
    g_mali_dfs_var.dfs_GpuClkRate = 0;
    g_mali_dfs_var.dfs_GpuUtilization = 0;
    g_mali_dfs_var.dfs_limitMinPrf = 0;
    g_mali_dfs_var.dfs_LockOn = 0;
    g_mali_dfs_var.dfs_StayCount = 0;
    g_mali_dfs_var.dfs_Utilization = 255;

    if (pdev == NULL){
        MALI_DEBUG_PRINT(1, ("ERROR: pdev is NULL!\n"));
        return;
    }

    dev = &pdev->dev;

    if (dev == NULL){
        MALI_DEBUG_PRINT(1, ("ERROR: dev is NULL!\n"));
        return;
    }

    np = dev->of_node;
    ret = of_property_read_u32(np, "dfs_lockprf",&g_mali_dfs_var.dfs_LockProfile);//s_uwDVFSLockPrf

    if (ret) {
        MALI_DEBUG_PRINT(1, ("ERROR: get dfs_lockprf failed!\n"));
        return;
    }

    ret = of_property_read_u32(np, "dfs_limit_max_prf",&g_mali_dfs_var.dfs_limitMaxPrf);//s_uwDvfslimitMaxPrf

    if (ret) {
        MALI_DEBUG_PRINT(1, ("ERROR: get dfs_limit_max_prf failed!\n"));
        return;
    }

    ret = of_property_read_u32(np, "dfs_steps",&g_mali_dfs_var.dfs_Steps);//s_uwDFSSteps

    if (ret) {
        MALI_DEBUG_PRINT(1, ("ERROR: get dfs_steps failed!\n"));
        return;
    }
    g_mali_dfs_var.dfs_CurrPrf = g_mali_dfs_var.dfs_limitMaxPrf;
    //s_uwDvfsCurrPrf = s_uwDvfslimitMaxPrf; //when power on,core freq is setted to Maxprof's freq
    mali_remark_clockrate(g_mali_dfs_var.dfs_CurrPrf);

    ret = of_property_read_u32(np, "dfs_profile_num",&pro_num);

    if (ret) {
        MALI_DEBUG_PRINT(1, ("ERROR: get profile num failed!\n"));
        return;
    }

    dfs_prf = (mali_dvfs_profile_table*)(&dfs_prf_table[0]);

    for(id = 0; id < pro_num; id++)
    {
        (void)of_property_read_u32_planar_array(np,"dfs_profiles",&dfs_prf_table[0],3,id);
        g_mali_dvfs_profile[id].freq = dfs_prf->freq;
        g_mali_dvfs_profile[id].div = dfs_prf->div;
        g_mali_dvfs_profile[id].pllNum = dfs_prf->pllNum;
        MALI_DEBUG_PRINT(3, (" mali dfs profiles: freq : %dM, div : %d, pllNum : %d !\n" \
                                            ,g_mali_dvfs_profile[id].freq \
                                            ,g_mali_dvfs_profile[id].div  \
                                            ,g_mali_dvfs_profile[id].pllNum));
    }

    g_mali_dfs_var.dfs_limitMaxRate = g_mali_dvfs_profile[g_mali_dfs_var.dfs_limitMaxPrf].freq;
    g_mali_dfs_var.dfs_limitMinRate = g_mali_dvfs_profile[g_mali_dfs_var.dfs_limitMinPrf].freq;
    gpu_devfreq_init();
    gpu_devfreq_set_maxfreq();
    gpu_devfreq_set_minfreq();

    MALI_DEBUG_PRINT(2, (" init mali dvfs ok\n"));
}

mali_bool pwrctrl_dfs_gpu_target(int step)
{
    u32 targetProfile;
    u32 byStep;

    if (1 == g_mali_dfs_var.dfs_DfsOn)//s_uwDebugFsDvfsOn
    {
        /* parameters check */
        if ((step > 3) || (step < -3))
        {
            MALI_DEBUG_PRINT(3,( "bad parameters input to pwrctrl_dfs_gpu_target \r\n"));
            return MALI_FALSE;
        }

        if (0 == step)
        {
            MALI_DEBUG_PRINT(3,( "step=0,no need call this dvfs func \r\n"));
            return MALI_TRUE;
        }

        /* dfs action */
        MALI_DEBUG_PRINT(3,( "current GPU DVFS Profile is %d \r\n",g_mali_dfs_var.dfs_CurrPrf));//s_uwDvfsCurrPrf

        targetProfile = mali_get_target_profile(g_mali_dfs_var.dfs_CurrPrf, step);

        if (g_mali_dfs_var.dfs_CurrPrf == targetProfile)
        {
            MALI_DEBUG_PRINT(3,( "current == target ,no need change \r\n"));
            return MALI_TRUE;
        }
        else
        {
            MALI_DEBUG_PRINT(3,( "current =%d, target=%d doing DVFS \r\n",g_mali_dfs_var.dfs_CurrPrf, targetProfile));
        }

        /* could support step over or step by step */
        #if DVFS_STEP_OVER
        mali_dfs_target_profile(targetProfile);
        #else
        if (g_mali_dfs_var.dfs_CurrPrf < targetProfile)
        {
            for ( byStep = g_mali_dfs_var.dfs_CurrPrf; byStep < targetProfile; byStep++ )
            {
                mali_dfs_target_profile(byStep+1);
            }
        }
        else
        {
            for ( byStep = g_mali_dfs_var.dfs_CurrPrf; byStep > targetProfile; byStep-- )
            {
                mali_dfs_target_profile(byStep-1);
            }
        }
        #endif

        g_mali_dfs_var.dfs_CurrPrf = targetProfile;
        mali_remark_clockrate(g_mali_dfs_var.dfs_CurrPrf);
    }
    return MALI_TRUE;
}
void pwrctrl_dfs_gpu_disable(void)
{
    if (1 == g_mali_dfs_var.dfs_DfsOn)//s_uwDebugFsDvfsOn
    {
        /* back the current profile for powerup */
        g_mali_dfs_var.dfs_BakPrf = g_mali_dfs_var.dfs_CurrPrf;
        mali_remark_clockrate(0);    //set gpu clock rate to 0MHZ
    }
}
void pwrctrl_dfs_gpu_enable(void)
{
   /* powerup with old profile */
    mali_dfs_target_profile(g_mali_dfs_var.dfs_BakPrf);//s_uwDvfsBakPrf
}
u32 mali_find_dvfs_profile(u32 rate)
{
    u32 i;
    for ( i = 0; i < g_mali_dfs_var.dfs_Steps; i++ )//s_uwDFSSteps
    {
        if (rate == g_mali_dvfs_profile[i].freq)
        {
            break;
        }
    }

    if (g_mali_dfs_var.dfs_Steps == i)
    {
        MALI_DEBUG_PRINT(3,("bad mali dvfs profile with freq =%d\n",rate));
    }
    return i;
}


u32 mali_get_target_profile(u32 curr, int step)
{
    u32 target;
    u32 maxProfile = g_mali_dfs_var.dfs_Steps - 1;

    /* target profile calculate */
    switch(step)
    {
        case 3:
            target = maxProfile;
            break;
        case 1:
            target = (maxProfile == curr) ? maxProfile : (curr+1);
            break;
        case -1:
            target = (0 == curr) ? 0 : (curr-1);
            break;
        case -3:
            target = 0;
            break;
        default:
            target = curr;
            break;
    }

    gpu_devfreq_get_maxfreq();
    gpu_devfreq_get_minfreq();

    if(g_mali_dfs_var.dfs_limitMaxRate != g_mali_dvfs_profile[g_mali_dfs_var.dfs_limitMaxPrf].freq)
    {
        pmqos_gpu_dfs_limit_max(1,g_mali_dfs_var.dfs_limitMaxRate);
    }

    if(g_mali_dfs_var.dfs_limitMinRate != g_mali_dvfs_profile[g_mali_dfs_var.dfs_limitMinPrf].freq)
    {
        pmqos_gpu_dfs_limit_min(1, g_mali_dfs_var.dfs_limitMinRate);
    }

    target  = (target > g_mali_dfs_var.dfs_limitMaxPrf) ? g_mali_dfs_var.dfs_limitMaxPrf : target;
    target  = (target < g_mali_dfs_var.dfs_limitMinPrf) ? g_mali_dfs_var.dfs_limitMinPrf : target;

    if( g_mali_dfs_var.dfs_LockOn == 1)
    {
        if(g_mali_dfs_var.dfs_LockProfile >= g_mali_dfs_var.dfs_Steps)
        {
            MALI_DEBUG_PRINT(1,("mali dfs Error: the input dfs_LockProfile is out of range!"));
            g_mali_dfs_var.dfs_LockProfile = g_mali_dfs_var.dfs_Steps - 1;
        }
        target = g_mali_dfs_var.dfs_LockProfile;
    }

    return target;
}
int mali_dfs_set_pll(u32 target)
{
    u32 i = 0;
    u32 ret = 0;
    if (g_mali_dvfs_profile[target].pllNum != g_mali_dvfs_profile[g_mali_dfs_var.dfs_CurrPrf].pllNum)
    {
        if (g_mali_dvfs_profile[target].freq > MAX_SFT_FREQ)                                       //ASIC下，GPU频率至少100M
        {
            phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR(0),0,31,DVFS_FREQ_DIV(6));
            for (i = 0; i < MAX_CUNT; i++)
            {
                ret = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR(0), 8, 11);
                if (MID_DIV == ret)
                {
                    break;
                }
                udelay(1);
            }
            if (MAX_CUNT == i)
            {
                MALI_DEBUG_PRINT(1,("mali dfs: ERROR! set mid div %d failed! Current div is %d!\n", g_mali_dvfs_profile[target].div, ret));
                return -1;
            }
        }
        phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(0),1,1,g_mali_dvfs_profile[target].pllNum);
    }
    return 0;
}

int mali_dfs_set_div(u32 target)
{
    u32 i = 0;
    u32 ret = 0;
    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR(0),0,31,DVFS_FREQ_DIV(g_mali_dvfs_profile[target].div));

    for (i = 0; i < MAX_CUNT; i++)
    {
        ret = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR(0), 8, 11);
        if (g_mali_dvfs_profile[target].div == ret)
        {
            return 0;
        }
        udelay(1);
    }

    MALI_DEBUG_PRINT(1,("mali dfs: ERROR! target %d  set div %d failed! Current div is %d!\n", target,g_mali_dvfs_profile[target].div, ret));
    return -1;
}


void mali_dfs_target_profile(u32 target)
{
    if (1 == g_mali_dfs_var.dfs_DfsOn)
    {
        if (0 != mali_dfs_set_pll(target))
        {
            MALI_DEBUG_PRINT(1,("mali dfs Error: set pll failed when target pll != current pll\n"));
            return;
        }

        if (0 != mali_dfs_set_div(target))
        {
            MALI_DEBUG_PRINT(1,("mali dfs Error: set div failed\n"));
            return;
        }
        MALI_DEBUG_PRINT(3,("mali dfs: from freq: %d to freq: %d\n", g_mali_dvfs_profile[g_mali_dfs_var.dfs_CurrPrf].freq, g_mali_dvfs_profile[target].freq));
        g_mali_dfs_var.dfs_CurrPrf = target;
        mali_remark_clockrate(g_mali_dfs_var.dfs_CurrPrf);
    }
}


void pmqos_gpu_dfs_get_current_profile(u32 *profile_id)
{
 //   *profile_id = g_mali_dfs_var.dfs_CurrPrf;
}

void pmqos_gpu_get_current_func(unsigned int *curr_func)
{
#if 0
    if (g_mali_dfs_var.dfs_DfsOn == 1)
    {
        *curr_func = 1;
    }
    else
    {
        *curr_func = 0;
    }
#endif
}

void pmqos_gpu_set_current_func(unsigned int target_func)
{
#if 0
    if ( target_func == 1 )
    {
        g_mali_dfs_var.dfs_DfsOn = 1;
    }
    else if ( target_func == 0 )
    {
        g_mali_dfs_var.dfs_DfsOn = 0;
    }
    else
    {
        //do something here;
    }
#endif
}

void pmqos_gpu_dfs_lock(u32 lock_profile)
{
    g_mali_dfs_var.dfs_LockOn = 1;//to control the dvfs caculator and run

    g_mali_dfs_var.dfs_LockProfile = lock_profile;
}

void pmqos_gpu_dfs_unlock(void)
{
    g_mali_dfs_var.dfs_LockOn = 0;//to control the dvfs caculator and run
}

void pmqos_gpu_dfs_limit_max(u32 flag,u32 max_freq_limit)
{
    u32 currPrf;
    if(0 == flag)
    {
        g_mali_dfs_var.dfs_limitMaxPrf = g_mali_dfs_var.dfs_Steps - 1;
    }
    else
    {
        for(currPrf = (g_mali_dfs_var.dfs_Steps - 1); currPrf > 0; currPrf--)
        {
            if( g_mali_dvfs_profile[currPrf].freq <= max_freq_limit)
            {
                g_mali_dfs_var.dfs_limitMaxPrf = currPrf;
                break;
            }
        }
        if(currPrf == 0)
        {
            g_mali_dfs_var.dfs_limitMaxPrf = 0;
        }
    }

    if(g_mali_dfs_var.dfs_limitMinPrf > g_mali_dfs_var.dfs_limitMaxPrf)
    {
        MALI_DEBUG_PRINT(1,("mali dfs : dfs_limitMinPrf > dfs_limitMaxPrf, so min <= max"));
        g_mali_dfs_var.dfs_limitMinPrf = g_mali_dfs_var.dfs_limitMaxPrf;
    }

    g_mali_dfs_var.dfs_limitMaxRate = g_mali_dvfs_profile[g_mali_dfs_var.dfs_limitMaxPrf].freq;
    gpu_devfreq_set_maxfreq();
    g_mali_dfs_var.dfs_limitMinRate = g_mali_dvfs_profile[g_mali_dfs_var.dfs_limitMinPrf].freq;
    gpu_devfreq_set_minfreq();
}

void pmqos_gpu_dfs_limit_min(u32 flag,u32 min_freq_limit)
{
    u32 currPrf;
    if(0 == flag)
    {
        g_mali_dfs_var.dfs_limitMinPrf = 0;
    }
    else
    {
        for(currPrf = 0; currPrf <= (g_mali_dfs_var.dfs_Steps - 1); currPrf++)
        {
            if(currPrf == (g_mali_dfs_var.dfs_Steps - 1))
            {
                g_mali_dfs_var.dfs_limitMinPrf = g_mali_dfs_var.dfs_Steps - 1;
                break;
            }

            if ( g_mali_dvfs_profile[currPrf].freq >= min_freq_limit)
            {
                g_mali_dfs_var.dfs_limitMinPrf = currPrf;
                break;
            }
        }
    }

    if(g_mali_dfs_var.dfs_limitMinPrf > g_mali_dfs_var.dfs_limitMaxPrf)
    {
        MALI_DEBUG_PRINT(1,("mali dfs : dfs_limitMinPrf > dfs_limitMaxPrf, so min => max"));
        g_mali_dfs_var.dfs_limitMaxPrf = g_mali_dfs_var.dfs_limitMinPrf;
    }

    g_mali_dfs_var.dfs_limitMaxRate = g_mali_dvfs_profile[g_mali_dfs_var.dfs_limitMaxPrf].freq;
    gpu_devfreq_set_maxfreq();
    g_mali_dfs_var.dfs_limitMinRate = g_mali_dvfs_profile[g_mali_dfs_var.dfs_limitMinPrf].freq;
    gpu_devfreq_set_minfreq();
}


/* Create /sys/class/devfreq/gpufreq for universal interface to powergenie */
#include <linux/platform_device.h>
#include <linux/devfreq.h>
#include <linux/pm_qos.h>

#define MODULE_NAME "GPU_DEVFREQ"

struct gpu_devfreq_device {
	struct devfreq *devfreq;
	struct clk *set;
	struct clk *get;
	unsigned int freq;
};

struct devfreq * g_gpudevfreq = NULL;

static int
gpu_devfreq_target(struct device *dev, unsigned long *freq, u32 flags)
{
    return 0;
}

static int
gpu_devfreq_get_dev_status(struct device *dev, struct devfreq_dev_status *stat)
{
    return 0;
}

static int gpu_devfreq_get_cur_status(struct device *dev, unsigned long *freq)
{
	*freq = g_mali_dfs_var.dfs_GpuClkRate;
	return 0;
}

static struct devfreq_dev_profile gpu_devfreq_dev_profile = {
	.polling_ms		= 0,
	.target			= gpu_devfreq_target,
	.get_dev_status		= gpu_devfreq_get_dev_status,
	.get_cur_freq		= gpu_devfreq_get_cur_status,
};

static struct devfreq_pm_qos_data gpu_devfreq_pm_qos_data = {
	.bytes_per_sec_per_hz = 8,
	.bd_utilization = 60,
	.pm_qos_class = PM_QOS_MEMORY_THROUGHPUT_UP_THRESHOLD,
};

static int gpu_devfreq_probe(struct platform_device *pdev)
{
	struct gpu_devfreq_device *ddev = NULL;
	struct device_node *np = pdev->dev.of_node;
	int ret = 0;

	if (!np) {
		pr_err("%s: %s %d, no device node\n",
			MODULE_NAME, __func__, __LINE__);
		ret = -ENODEV;
		goto out;
	}

	ddev = kmalloc(sizeof(struct gpu_devfreq_device), GFP_KERNEL);
	if (!ddev) {
		pr_err("%s: %s %d, no mem\n",
			MODULE_NAME, __func__, __LINE__);
		ret = -ENOMEM;
		goto no_men;
	}

	if (of_init_opp_table(&pdev->dev) ||
		opp_init_devfreq_table(&pdev->dev,
			&gpu_devfreq_dev_profile.freq_table)) {
		ddev->devfreq = NULL;
	} else {
		ddev->devfreq = devfreq_add_device(&pdev->dev,
					&gpu_devfreq_dev_profile,
					"pm_qos",
					&gpu_devfreq_pm_qos_data);
       }

	if (IS_ERR(ddev->devfreq)) {
		pr_err("%s: %s %d, Failed to init ddr devfreq_table\n",
			MODULE_NAME, __func__, __LINE__);
		ret = -ENODEV;
		goto no_devfreq;
	}

	g_gpudevfreq = ddev->devfreq;
	/*
	 *	cache value.
	 *	It does not mean actual ddr clk currently,
	 *	but a frequency cache of every clk setting in the module.
	 *	Because, it is not obligatory that setting value is equal to
	 *	the currently actual ddr clk frequency.
	 */
	ddev->freq = 0;

	platform_set_drvdata(pdev, ddev);

	pr_info("%s: ready\n", MODULE_NAME);
	return ret;

no_devfreq:
	kfree(ddev);
no_men:
out:
	return ret;
}

static int gpu_devfreq_remove(struct platform_device *pdev)
{
	struct gpu_devfreq_device *ddev = platform_get_drvdata(pdev);

	devfreq_remove_device(ddev->devfreq);
	g_gpudevfreq = NULL;

	platform_set_drvdata(pdev, NULL);
	kfree(ddev);
	return 0;
}

static void gpu_devfreq_set_maxfreq(void)
{
    if(g_gpudevfreq)
    {
        mutex_lock(&g_gpudevfreq->lock);
        g_gpudevfreq->max_freq = g_mali_dfs_var.dfs_limitMaxRate * GPU_MHZ;
        mutex_unlock(&g_gpudevfreq->lock);
    }

    return;
}

static void gpu_devfreq_set_minfreq(void)
{
    if(g_gpudevfreq)
    {
        mutex_lock(&g_gpudevfreq->lock);
        g_gpudevfreq->min_freq = g_mali_dfs_var.dfs_limitMinRate * GPU_MHZ;
        mutex_unlock(&g_gpudevfreq->lock);
    }

    return;
}

static void gpu_devfreq_get_maxfreq(void)
{
    if(g_gpudevfreq)
    {
        g_mali_dfs_var.dfs_limitMaxRate = g_gpudevfreq->max_freq / GPU_MHZ;
    }

    return;
}

static void gpu_devfreq_get_minfreq(void)
{
    if(g_gpudevfreq)
    {
        g_mali_dfs_var.dfs_limitMinRate = g_gpudevfreq->min_freq / GPU_MHZ;
    }

    return;
}

#ifdef CONFIG_OF
static const struct of_device_id gpu_devfreq_of_match[] = {
	{.compatible = "hisilicon,gpu_devfreq",},
	{},
};
MODULE_DEVICE_TABLE(of, gpu_devfreq_of_match);
#endif

static struct platform_driver gpu_devfreq_driver = {
	.probe = gpu_devfreq_probe,
	.remove = gpu_devfreq_remove,
	.driver = {
		.name = "gpu_devfreq",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(gpu_devfreq_of_match),
	},
};

void gpu_devfreq_init(void)
{
    int err = platform_driver_register(&gpu_devfreq_driver);

    if (0 != err) {
        pr_err("%s: %s %d, register gpu_devfreq fail\n", MODULE_NAME, __func__, __LINE__);
        return err;
    }

    return;
}

#ifdef CONFIG_DEBUG_FS

#define MALI_DFS_DEBUGFS_FILE_NAME_DFS_ON   "mit1_gpu_dfs_on"
#define MALI_DFS_DEBUGFS_FILE_NAME_LOCK   "mit1_gpu_dfs_lock"
#define MALI_DFS_DEBUGFS_FILE_NAME_LOCK_PROFILE   "mit1_gpu_dfs_lock_profile"
#define MALI_DFS_DEBUGFS_FILE_NAME_CLOCK_RATE   "mit1_gpu_clock_rate"
#define MALI_DFS_DEBUGFS_FILE_NAME_GPU_UTILIZATION   "mit1_gpu_utilization"
#define MALI_DFS_DEBUGFS_FILE_NAME_LIMIT_MAXRATE   "mit1_gpu_limit_max_rate"
#define MALI_DFS_DEBUGFS_FILE_NAME_LIMIT_MINRATE   "mit1_gpu_limit_min_rate"



int __init mali_dfs_debugfs_init(void)
{
    struct dentry *mit1_debugfs_dir = NULL;

    printk("mali_dfs_debugfs_init\n");

    debugfs_create_u32(MALI_DFS_DEBUGFS_FILE_NAME_DFS_ON, 0664, mit1_debugfs_dir,  &g_mali_dfs_var.dfs_DfsOn);
    debugfs_create_u32(MALI_DFS_DEBUGFS_FILE_NAME_LOCK, 0664, mit1_debugfs_dir,  &g_mali_dfs_var.dfs_LockOn);
    debugfs_create_u32(MALI_DFS_DEBUGFS_FILE_NAME_LOCK_PROFILE, 0664, mit1_debugfs_dir,  &g_mali_dfs_var.dfs_LockProfile);
    debugfs_create_u32(MALI_DFS_DEBUGFS_FILE_NAME_CLOCK_RATE, 0664, mit1_debugfs_dir,  &g_mali_dfs_var.dfs_GpuClkRate);
    debugfs_create_u32(MALI_DFS_DEBUGFS_FILE_NAME_GPU_UTILIZATION, 0664, mit1_debugfs_dir,  &g_mali_dfs_var.dfs_GpuUtilization);
    debugfs_create_u32(MALI_DFS_DEBUGFS_FILE_NAME_LIMIT_MAXRATE, 0664, mit1_debugfs_dir,  &g_mali_dfs_var.dfs_limitMaxRate);
    debugfs_create_u32(MALI_DFS_DEBUGFS_FILE_NAME_LIMIT_MINRATE, 0664, mit1_debugfs_dir,  &g_mali_dfs_var.dfs_limitMinRate);
    return 0;
}

module_init(mali_dfs_debugfs_init);

#endif

