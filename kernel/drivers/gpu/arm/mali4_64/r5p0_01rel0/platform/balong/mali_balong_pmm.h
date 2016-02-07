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
 * @file mali_platform.h
 * Platform specific Mali driver functions
 */

#ifndef __MALI_BALONG_H__
#define __MALI_BALONG_H__

#include "mali_osk.h"
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/workqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************** defined for pmm ***************/
/*Macro defination*/
#define MALI_PWRON_NO_DRV               0
#define MALI_DVFS_ENABLED               1
#define GPU_MHZ                         1000000

#define MALI_UTILIZATION_NUM            5

#define MALI_BASE_ADDR                  (0xF4080000)
/* irq number */
#define MALI_IRQ_ID                     (142)

#define MALI_FRAME_BUFFER_ADDR          0x33000000
#define MALI_FRAME_BUFFER_SIZE          0x01000000

typedef enum mali_core_type_tag
{
    MALI_CORE_400_MP1   = 0,
    MALI_CORE_400_MP2   = 1,
    MALI_CORE_450_MP4   = 2,
    MALI_CORE_TYPE_MAX
} mali_core_type;

/** @brief description of power change reasons
 */
typedef enum mali_power_mode_tag
{
    MALI_POWER_MODE_ON,           /**< Power Mali on */
    MALI_POWER_MODE_LIGHT_SLEEP,  /**< Mali has been idle for a short time, or runtime PM suspend */
    MALI_POWER_MODE_DEEP_SLEEP,   /**< Mali has been idle for a long time, or OS suspend */
} mali_power_mode;

/*platform clk and power*/
_mali_osk_errcode_t mali_platform_powerup(void);
_mali_osk_errcode_t mali_platform_powerdown(void);

/** @brief Platform specific setup and initialisation of MALI
 *
 * This is called from the entrypoint of the driver to initialize the platform
 *
 * @return _MALI_OSK_ERR_OK on success otherwise, a suitable _mali_osk_errcode_t error.
 */
extern _mali_osk_errcode_t mali_platform_init(struct platform_device *pdev);

/** @brief Platform specific deinitialisation of MALI
 *
 * This is called on the exit of the driver to terminate the platform
 *
 * @return _MALI_OSK_ERR_OK on success otherwise, a suitable _mali_osk_errcode_t error.
 */
extern _mali_osk_errcode_t mali_platform_deinit(void);

/** @brief Platform specific powerdown sequence of MALI
 *
 * Notification from the Mali device driver stating the new desired power mode.
 * MALI_POWER_MODE_ON must be obeyed, while the other modes are optional.
 * @param power_mode defines the power modes
 * @return _MALI_OSK_ERR_OK on success otherwise, a suitable _mali_osk_errcode_t error.
 */
_mali_osk_errcode_t mali_platform_power_mode_change(mali_power_mode power_mode);

/** @brief Setting the power domain of MALI
 *
 * This function sets the power domain of MALI if Linux run time power management is enabled
 *
 * @param dev Reference to struct platform_device (defined in linux) used by MALI GPU
 */

mali_bool get_gpu_power_status(void);

void mali_hisi_mem_init(void);

/****************** defined for pmm ***************/

/****************** defined for dfs ***************/
typedef enum mali_dvfs_status_tag
{
    MALI_DVFS_DECREASE_LOWEST   = 0,            /**< Freq down to lowest level*/
    MALI_DVFS_DECREASE          = 2,            /**< Freq down */
    MALI_DVFS_HOLD              = 3,            /**< Freq No change */
    MALI_DVFS_INCREASE          = 4,            /**< Freq up */
    MALI_DVFS_INCREASE_HIGHEST  = 6,            /**< Freq up TO highest level*/
    MALI_DVFS_BUTT,
} mali_dvfs_status;

/* recorder the current status for test */
typedef struct mali_dvfs_statusTag{
    mali_dvfs_status  currentStep;
}mali_dvfs_currentstatus;

/* ioremapped addr for mali 64bit instead of phy_reg_readl/phy_reg_writel*/
typedef struct mali_soc_remap_addrTag{
    u8 *soc_media_sctrl_base_addr;
    u8 *soc_ao_sctrl_base_addr;
    u8 *soc_peri_sctrl_base_addr;
    u8 *soc_pmctl_base_addr;
} mali_soc_remap_addr_table;

/* dvfs threshold */
typedef struct mali_dvfs_thresholdTag{
    unsigned int downthreshold;
    unsigned int upthreshold;
}mali_dvfs_threshold_table;

/* dvfs profile */
typedef struct mali_dvfs_profileTag{
    unsigned int freq;                      /* pll0 should config freq,so calc is ness */
    unsigned int div;                       /* the div of pll0 or pll1*/
    unsigned int pllNum;                    /* pll0 or pll1 */
}mali_dvfs_profile_table;

typedef struct mali_dfs_para{
    u32 dfs_CurrPrf;                        /* current profile*/
    u32 dfs_BakPrf;                         /* backup  profile-record current profile before power down*/
    u32 dfs_LockOn;                         /* 锁频开关*/
    u32 dfs_Steps;                          /* 调频步数*/
    u32 dfs_limitMaxPrf;                    /* 限频时用到，所限的最大频率对应的profile*/
    u32 dfs_limitMinPrf;                    /* 限频时用到，所限的最小频率对应的profile*/
    u32 dfs_Utilization;                    /* GPU的负载，用于统计负载以决定调频策略 */
    u32 dfs_StayCount;                      /* 负载统计的计算次数*/
    u32 dfs_LockProfile;                    /* 锁频时的profile*/
    u32 dfs_DfsOn;                          /* 调频开关*/
    u32 dfs_GpuClkRate;                     /* 输出给freqdump的gpu频率*/
    u32 dfs_GpuUtilization;                 /* 输出给freqdump的gpu负载*/
    u32 dfs_limitMaxRate;                   /* 限频上限，给省电精灵用*/
    u32 dfs_limitMinRate;                   /* 限频下限，给省电精灵用*/
}mali_dfs_global_var;

void mali_dvfs_work_handler(struct work_struct *w);
u32 find_current_weight(u32 utilization);
u32 mali_find_dvfs_profile(u32 rate);
u32 mali_get_target_profile(u32 curr, int step);
mali_dvfs_status decideNextStatus(void);
void mali_remark_clockrate(u32 profile_id);

/*kernel acpu - mcu APIs*/
void pwrctrl_g3d_dfs_init(struct platform_device *pdev);
extern mali_bool pwrctrl_dfs_gpu_target(int step);
extern void pwrctrl_dfs_gpu_disable(void);
extern void pwrctrl_dfs_gpu_enable(void);

/*mali dvfs*/
mali_dvfs_status get_mali_dvfs_status(void);
mali_bool mali_dvfs_status_init(mali_dvfs_status maliDvfsStatus);
void mali_dvfs_status_deinit(void);
void mali_dvfs_handler(u32 utilization);
void mali_dvfs_status_update(u32 utilization);
/** @brief Platform specific handling of GPU utilization data
 *
 * When GPU utilization data is enabled, this function will be
 * periodically called.
 *
 * @param utilization The workload utilization of the Mali GPU. 0 = no utilization, 256 = full utilization.
 */
void mali_gpu_utilization_handler(u32 utilization);
int mali_dfs_set_pll(u32 target);
int mali_dfs_set_div(u32 target);
void mali_set_limprf_by_rate(u32 rate, u32 *profile);


void mali_dfs_target_profile(u32 target);
void pmqos_gpu_dfs_get_current_profile(u32 *profile_id);
void pmqos_gpu_get_current_func(unsigned int *curr_func);
void pmqos_gpu_set_current_func(unsigned int target_func);
void pmqos_gpu_dfs_lock(u32 lock_profile);
void pmqos_gpu_dfs_unlock(void);
void pmqos_gpu_dfs_limit_max(u32 flag, u32 max_freq_limit);
void pmqos_gpu_dfs_limit_min(u32 flag, u32 min_freq_limit);


#define CALC_REG_MASK(bit_start, bit_end) \
    (((0x1 << (bit_end - bit_start + 1)) - 1) << bit_start)
/* Function declaration */
/**********************************************
 * write value to physic registers
 * base_addr: virtual register base addr
 * reg_offset: offset of the register
 * start_bit: write value start bit
 * end_bit: write value end bit
 * val: write value
 *********************************************/
static inline void mali_reg_writel(u8* base_addr,
                unsigned int reg_offset,
                unsigned char start_bit,
                unsigned char end_bit,
                unsigned int val)
{
    int read_val = 0;
    //unsigned int comm_base_addr = 0;
    unsigned long flags;
    static spinlock_t reg_lock = SPIN_LOCK_UNLOCKED;

    //comm_base_addr = HISI_VA_ADDRESS(base_addr);
    //comm_base_addr = ioremap(base_addr);

    WARN_ON(0 == base_addr);

    spin_lock_irqsave(&reg_lock, flags);
    //read_val = readl(comm_base_addr + reg_offset) & ~(CALC_REG_MASK(start_bit, end_bit));
    read_val = readl((void __iomem*)(base_addr + reg_offset)) & ~(CALC_REG_MASK(start_bit, end_bit));
    read_val |= (CALC_REG_MASK(start_bit, end_bit) & (val << start_bit));
    //writel(read_val, comm_base_addr + reg_offset);
    writel(read_val, (void __iomem*)(base_addr + reg_offset));

    spin_unlock_irqrestore(&reg_lock, flags);

    return;
}


/**********************************************
 * read value from physical registers
 * base_addr: virtual register base addr
 * reg_offset: offset of the register
 * start_bit: read value start bit
 * end_bit: read value end bit
 *********************************************/
static inline unsigned int mali_reg_readl(u8* base_addr,
                unsigned int reg_offset,
                unsigned char start_bit,
                unsigned char end_bit)
{
    unsigned int val = 0;
//    unsigned int comm_base_addr = 0;

//    comm_base_addr = HISI_VA_ADDRESS(base_addr);
   // comm_base_addr = ioremap(base_addr);

    WARN_ON(0 == base_addr);

    //val = readl(comm_base_addr + reg_offset);
    val = readl((void __iomem*)(base_addr + reg_offset));
    val &= CALC_REG_MASK(start_bit, end_bit);

    return val >> start_bit;
}
/****************** defined for dfs ***************/


#ifdef __cplusplus
}
#endif
#endif
