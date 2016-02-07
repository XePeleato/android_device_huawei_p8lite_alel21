/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : pwrctrl_common.c
  版 本 号   : 初稿
  作    者   : 刘永富 56193
  生成日期   : 2012年7月25日
  最近修改   :
  功能描述   :  低功耗公共模块
  函数列表   :
              pwrctrl_clr_bits
              pwrctrl_get_slice_time
              pwrctrl_is_bit_set
              pwrctrl_is_func_on
              pwrctrl_mem_cpy
              pwrctrl_read_reg32
              pwrctrl_timer_delay
              pwrctrl_write_reg32
              pwrctrl_set_bits

  修改历史   :
  1.日    期   : 2012年7月25日
    作    者   : 刘永富 56193
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#ifndef WIN32
#include <linux/string.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
/*#include <asm/memory.h>*/
/*#include <mach/system.h>*/
#include <linux/hisi/hi6xxx-platform.h>
#include <linux/delay.h>
/*#include <mach/irqs.h>*/
/*#include <asm/hardware/gic.h>*/
/*#include <asm/hardware/arm_timer.h>*/
#include <linux/slab.h>
#include <linux/wakelock.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/of.h>

/*#include <mach/io.h>*/
/*#include <mach/platform.h>*/
/*#include "product_config.h"*/

#endif
#include <linux/hisi/pm/hi6xxx-power-common.h>
#include <linux/hisi/pm/hi6xxx-power-dfs.h>
#include "soc_peri_sctrl_interface.h"
#include "soc_acpu_sctrl_interface.h"

#if 0
#include <soc_baseaddr_interface.h>
#include <soc_sctrl_interface.h>
#include <mach/pwrctrl/pwrctrl_perimgr.h>
#include <mach/pwrctrl/pwrctrl_sleepmgr.h>
#include <mach/pwrctrl/pwrctrl_sleep.h>
#endif
/*#include "drv_timer.h"*/
#include <linux/hisi/hi6xxx-iomap.h>

#if defined (CONFIG_HISILICON_PLATFORM_MAINTAIN)
#include "hisi/mntn/excDrv.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
#if 0
PWRCTRL_ACPU_EXC_LOG_STRU * g_stAcpuPwcExcLog = NULL;
#endif

PWC_ACPU_LOG_STRU * g_pPwcAcpuLog = NULL;
static PWC_CCPU_LOG_STRU * g_pPwcCcpuLog = NULL;
static PWC_MCU_LOG_STRU * g_pPwcMcuLog = NULL;

s8_t g_pwcAcpuWakeFlagIcc = 0;
s8_t g_pwcAcpuWakeFlagRfile = 0;

extern u32_t omTimerGet(void);
#if 0
struct wake_lock g_ulTstwakelock;

u32_t   g_ulPwcDebugTrace = 0;

u32_t g_ulpwrctrl_print_switch = 0;

u32_t g_pwc_init_flag = 0;

extern s32_t pwrctrl_timer_addr_remap(void);
#endif
#if 0
u32_t g_aAcpuHwVoteBaseAddr[] =
{
    IO_ADDRESS(SOC_AO_SCTRL_SC_MCPU_VOTEEN0_ADDR(SOC_SC_ON_BASE_ADDR)),
    IO_ADDRESS(SOC_AO_SCTRL_SC_PERI_VOTEEN0_ADDR(SOC_SC_ON_BASE_ADDR)),
    IO_ADDRESS(SOC_AO_SCTRL_SC_ACPU_VOTEEN0_ADDR(SOC_SC_ON_BASE_ADDR)),
    IO_ADDRESS(SOC_AO_SCTRL_SC_MCU_VOTEEN0_ADDR(SOC_SC_ON_BASE_ADDR)),
    IO_ADDRESS(SOC_AO_SCTRL_SC_MCU_VOTE1EN0_ADDR(SOC_SC_ON_BASE_ADDR)),
    IO_ADDRESS(SOC_AO_SCTRL_SC_MCU_VOTE2EN0_ADDR(SOC_SC_ON_BASE_ADDR)),

};
#endif
/*****************************************************************************
  3 函数实现
*****************************************************************************/
#if 0
/*****************************************************************************
 函 数 名  : pwrctrl_print_switch_set
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 根据模块设置打印开关
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月22日
    作    者   : 张柬 00179562
    修改内容   : 新生成函数

*****************************************************************************/
void pwrctrl_print_switch_set(unsigned int flag,int mode)
{
    /*bit模式*/
    if(0 == mode)
    {
        PWC_TRACE(PWC_LOG_INFO, "pwrctrl_print_switch_set,old value:0x%x\n",g_ulpwrctrl_print_switch);
        g_ulpwrctrl_print_switch |= (1<<flag);
        PWC_TRACE(PWC_LOG_INFO, "pwrctrl_print_switch_set,new value:0x%x\n",g_ulpwrctrl_print_switch);
    }
    else/*一次设置模式*/
    {
        g_ulpwrctrl_print_switch = flag;
    }
}
/*****************************************************************************
 函 数 名  : print_switch_get
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 获取打印开关值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月22日
    作    者   : 张柬 00179562
    修改内容   : 新生成函数

*****************************************************************************/
u32_t pwrctrl_print_switch_get(void)
{
    return g_ulpwrctrl_print_switch;
}
#endif

/*****************************************************************************
 函 数 名  : pwrctrl_get_slice_time
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 定时器当前值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月31日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
u32_t  pwrctrl_get_slice_time( void_t )
{
    return omTimerGet();
}

int  dt_insmod_pm_ap_general(void)
{
    struct device_node *root = NULL;
    const char *out_string;

    root = of_find_compatible_node((struct device_node *)NULL, (const char *)NULL, "hisilicon,hi6xxxx-pm-general");
    if (!root)
    {
        pr_err("%s of_find_compatible_node failed\n",__func__);
        return -1;
    }

	if(of_property_read_string(root,"status",&out_string))
	{
		pr_err("%s of_property_read_string failed\n",__func__);
		return -1;
	}

	if(0 == strcmp("ok",out_string))
	{
		pr_info("%s\n",__func__);
		return 0;
	}

	return -1;

}

static s32_t __init pwrctrl_common_init(void_t)
{
    void __iomem *acpu_sc_base;

    volatile SOC_ACPU_SCTRL_ACPU_SC_CLKEN_UNION *acpusc_clken;
    acpu_sc_base = (void __iomem *)HISI_VA_ADDRESS(SOC_ACPU_SCTRL_BASE_ADDR);

    /*ACPU auto clk gating*/
    acpusc_clken = (volatile SOC_ACPU_SCTRL_ACPU_SC_CLKEN_UNION *)SOC_ACPU_SCTRL_ACPU_SC_CLKEN_ADDR(acpu_sc_base);
    acpusc_clken->reg.bypass_decbrg_clkgten = 1;
    acpusc_clken->reg.bypass_ace_clkgten = 1;

    pwrctrl_dfs_init();

    return RET_OK;
}

/*****************************************************************************
 函 数 名  : pwrctrl_common_initial
 功能描述  : 低功耗初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : RET_OK         成功
             RET_ERR        失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月25日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
#if 0
s32_t __init pwrctrl_common_initial ( void_t )
{
/*lint -e553*/
#if 1/*(FEATURE_POWER_DRV == FEATURE_ON)*/
/*lint +e553*/
    local_t u32_t startTm=0;
    u32_t tmpTm = 0;

    g_stAcpuPwcExcLog = (PWRCTRL_ACPU_EXC_LOG_STRU *)(EXCH_ACPU_CORE_PWC_ADDR_V);
    pwrctrl_write_reg32(IO_ADDRESS(EXCH_A_CORE_POWRCTRL_CONV_ADDR), EXCH_ACPU_CORE_PWC_ADDR);

    /*保存SLICE TIMER 基地址*/
#if defined(CHIP_BB_HI6210) /*B020 Modify*/
#else
    pwrctrl_write_reg32(PWRCTRL_ACPU_ASM_SLICE_BAK_ADDR, IO_ADDRESS(SOC_AO_SCTRL_SC_SLICER_COUNT0_ADDR(SOC_SC_ON_BASE_ADDR)));
#endif
    wake_lock_init(&g_ulTstwakelock,WAKE_LOCK_SUSPEND,"pwrctrl_tst");
    pwrctrl_sleep_initial();
    pwrctrl_sleep_mgr_sleep_init();

    if(RET_OK != pwrctrl_is_func_on(PWC_SWITCH_ASLEEP))
    {
        wake_lock(&g_ulTstwakelock);
        PWC_TRACE(PWC_LOG_MAX, "MCU: pwc_common_initial,msleep not support!\r\n",0,0,0,0,0,0);
    }
    else
    {
        /*lint*/
    }

    /* 低功耗初始化完成*/
    PWC_TRACE(PWC_LOG_MAX, "ACPU: Enter pwrctrl initial routing! \n",0,0,0,0,0,0);

    register_reboot_notifier(&pm_reboot_nb);
    pwrctrl_write_reg32(IO_ADDRESS(MEMORY_AXI_SEC_CORE_BOOT_TEST_ADDR), 0);
    printk("sec core boot test addr:0x%x\n", MEMORY_AXI_SEC_CORE_BOOT_TEST_ADDR);
#endif
    g_pwc_init_flag = PWRCTRL_INIT_FLAG;

    return RET_OK;
}

int g_acpu_pm_test_flag = 0;

void set_acpu_pm_test_flag(int flag)
{
	printk("%s old:%d new:%d\n", __FUNCTION__, g_acpu_pm_test_flag, flag);
	g_acpu_pm_test_flag = flag;
}

void acpu_pm_test()
{
	if(0 == g_acpu_pm_test_flag)
	{
		wake_lock(&g_ulTstwakelock);
       }
}

void release_acpu_test_wakelock()
{
	printk("%s wake unlock\n", __FUNCTION__);
	wake_unlock(&g_ulTstwakelock);
}
#endif

/*****************************************************************************
 函 数 名  : pwrctrl_timer_delay
 功能描述  : 延时函数
 输入参数  : s32_t time
 输出参数  : 无
 返 回 值  : RET_OK         成功
             RET_ERR        失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
s32_t pwrctrl_timer_delay (u32_t time)
{
    u32_t uDecValue = (u32_t)(time * PWRCTRL_SLICE_CLK) / PWRCTRL_SLICE_UNIT_CONV;
    u32_t uCurValue, uTargValue;

    /* 读Timer的当前计数值 */
    uCurValue = pwrctrl_get_slice_time();
    PWC_TRACE(PWC_LOG_DEBUG, "Current TIME Value1 = 0x%x\n", uCurValue);

    /* 求Timer的递减目标值 */
    if (uDecValue <= uCurValue)
    {
        uTargValue = uCurValue - uDecValue;
    }
    else
    {
        uTargValue = PWRCTRL_SLICETIMER_XLOAD - (uDecValue - uCurValue);
    }

    /* 读取，并比较 */
    for (;;)
    {
        uCurValue = pwrctrl_get_slice_time();
        if (uCurValue <= uTargValue)
        {
            PWC_TRACE(PWC_LOG_DEBUG, "Curent,TIME Value2 = 0x%x\n", uCurValue);
            break;
        }
    }
    return RET_OK;
}

/*****************************************************************************
 函 数 名  : pwrctrl_write_reg32
 功能描述  : 寄存器写接口
 输入参数  : s32_t ulRegAddr
             s32_t ulRegVal
 输出参数  : 无
 返 回 值  : RET_OK         成功
             RET_ERR        失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
s32_t pwrctrl_write_reg32(unsigned long ulRegAddr, u32_t ulRegVal)
{
    if (0 != (ulRegAddr & 0x03))/* not 4byte aligned */
    {
        PWC_TRACE(PWC_LOG_MAX, "pwrctrl_write_reg32 Address: 0x%x not aligned.\r\n", (int)ulRegAddr);
        return RET_ERR;
    }

    writel(ulRegVal, ulRegAddr);
    return RET_OK;
}
/*****************************************************************************
 函 数 名  : pwrctrl_read_reg32
 功能描述  : 寄存器读接口
 输入参数  : s32_t ulRegAddr
 输出参数  : 无
 返 回 值  : int            目标寄存器的值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
u32_t pwrctrl_read_reg32 (unsigned long ulRegAddr )
{
    if (0 != (ulRegAddr & 0x03))/* not 4byte aligned */
    {
        PWC_TRACE(PWC_LOG_MAX, "pwrctrl_read_reg32 Address: 0x%x not aligned.\r\n", (int)ulRegAddr);
        return 0;
    }
    return readl(ulRegAddr);
}

/*****************************************************************************
 函 数 名  : pwrctrl_write_reg32_mask
 功能描述  : 带mask的寄存器写接口
 输入参数  : s32_t ulRegAddr
             s32_t ulRegVal
             u32_t ulMask
 输出参数  : 无
 返 回 值  : RET_OK         成功
             RET_ERR        失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
void pwrctrl_write_reg32_mask (unsigned long ulRegAddr, u32_t ulRegVal, u32_t ulMask)
{
    u32_t reg_val = pwrctrl_read_reg32(ulRegAddr);

    pwrctrl_clr_bits((unsigned long)&reg_val, ulMask);

    pwrctrl_set_bits((unsigned long)&reg_val, ulRegVal & ulMask);

    pwrctrl_write_reg32(ulRegAddr, reg_val);
}

/*****************************************************************************
 函 数 名  : pwrctrl_clear_reg32_mask
 功能描述  : 带mask的寄存器读接口
 输入参数  : s32_t ulRegAddr
             u32_t ulMask
 输出参数  : 无
 返 回 值  : int            目标寄存器的值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
u32_t pwrctrl_read_reg32_mask(unsigned long ulRegAddr, u32_t ulMask)
{
    return pwrctrl_read_reg32(ulRegAddr) & ulMask;
}

/*****************************************************************************
 函 数 名  : pwrctrl_set_bits
 功能描述  : 寄存器BIT设置接口
 输入参数  : s32_t ulRegAddr
             s32_t ulMask
 输出参数  : 无
 返 回 值  : RET_OK         成功
             RET_ERR        失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
s32_t pwrctrl_set_bits (unsigned long ulRegAddr, u32_t ulMask)
{
    if (0 != (ulRegAddr & 0x03))/* not 4byte aligned */
    {
        PWC_TRACE(PWC_LOG_MAX, "pwrctrl_set_bits Address: 0x%x not aligned.\r\n", (int)ulRegAddr);
        return RET_ERR;
    }

    pwrctrl_write_reg32(ulRegAddr, pwrctrl_read_reg32(ulRegAddr)|ulMask);
    return RET_OK;
}

/*****************************************************************************
 函 数 名  : pwrctrl_clr_bits
 功能描述  : 寄存器BIT清除接口
 输入参数  : s32_t ulRegAddr
             s32_t ulMask
 输出参数  : 无
 返 回 值  : RET_OK         成功
             RET_ERR        失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
s32_t pwrctrl_clr_bits (unsigned long ulRegAddr, u32_t ulMask)
{
    if (0 != (ulRegAddr & 0x03))/* not 4byte aligned */
    {
        PWC_TRACE(PWC_LOG_MAX, "pwrctrl_clr_bits Address: 0x%x not aligned.\r\n", (int)ulRegAddr);
        return RET_ERR;
    }

    pwrctrl_write_reg32(ulRegAddr, pwrctrl_read_reg32(ulRegAddr)&(~ulMask));
    return RET_OK;
}

/*****************************************************************************
 函 数 名  : pwrctrl_is_bits_set
 功能描述  : 寄存器BITS判断接口
 输入参数  : s32_t ulRegAddr
             s32_t ulMask
 输出参数  : 无
 返 回 值  : TRUE / FALSE
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
bool_t pwrctrl_is_bits_set(unsigned long ulRegAddr, u32_t ulMask)
{
    if(0 != (ulRegAddr & 0x03))
    {
        PWC_TRACE(PWC_LOG_ERROR, "pwrctrl_is_bits_set para error.\r\n");
    }

    return (pwrctrl_read_reg32(ulRegAddr) & ulMask) == ulMask;
}

/*****************************************************************************
 函 数 名  : pwrctrl_is_bits_clr
 功能描述  : 寄存器BITS判断接口
 输入参数  : s32_t ulRegAddr
             s32_t ulMask
 输出参数  : 无
 返 回 值  : TRUE / FALSE
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
bool_t pwrctrl_is_bits_clr(unsigned long ulRegAddr, u32_t ulMask)
{
    if(0 != (ulRegAddr & 0x03))
    {
        PWC_TRACE(PWC_LOG_ERROR, "pwrctrl_is_bits_clr para error.\r\n");
    }

    return (pwrctrl_read_reg32(ulRegAddr) & ulMask) == 0x0;
}

/*****************************************************************************
 函 数 名  : pwrctrl_is_bit_set
 功能描述  : 检查寄存器BIT状态
 输入参数  : u32_t addr    寄存器地址
             u32_t offset  bit 偏移
 输出参数  : 无
 返 回 值  : PWC_TRUE         1
             PWC_FALSE        0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
u32_t pwrctrl_is_bit_set (unsigned long addr,  u32_t offset)
{
    if ((pwrctrl_read_reg32(addr) & (1<<offset)) == (1<<offset))
    {
        return PWRCTRL_TRUE ;
    }
    else
    {
        return PWRCTRL_FALSE;
    }
}

/*****************************************************************************
 函 数 名  : pwrctrl_mem_cpy
 功能描述  : 低功耗内存拷贝
 输入参数  : dest   目标内存
             src    源内存
             size   长度，单位 32bit
 输出参数  : 无
 返 回 值  : RET_OK         成功
             RET_ERR        失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
s32_t pwrctrl_mem_cpy ( void_t * dest, void_t * src, u32_t size )
{
    u32_t length = 0;
    u32_t * tDest = (u32_t *)(dest);
    u32_t * tSrc = (u32_t *)(src);

    for(; length<size; length++)
    {
        pwrctrl_write_reg32((unsigned long)tDest,pwrctrl_read_reg32((unsigned long)tSrc));
        tDest ++;
        tSrc ++;
    }

    return RET_OK;
}
/*****************************************************************************
 函 数 名  : pwrctrl_debug_info_show
 功能描述  : 低功耗调试信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : RET_OK         成功
             RET_ERR        失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月30日
    作    者   : 刘永富 56193
    修改内容   : 新生成函数

*****************************************************************************/
#if 0
s32_t pwrctrl_acpu_debug_info_show( void_t )
{
    PWC_TRACE(PWC_LOG_MAX, "\n################ A核 core0, Current slice: 0x%x ##############\n", pwrctrl_get_slice_time());
    PWC_TRACE(PWC_LOG_MAX, "睡眠次数: 0x%x, 唤醒次数: 0x%x      \n", \
         g_stAcpuPwcExcLog->core0.SlpCnt, g_stAcpuPwcExcLog->core0.WkCnt);
    PWC_TRACE(PWC_LOG_MAX, "睡眠开始时间:0x%x, 睡眠时间:0x%x, 唤醒时间:0x%x , 唤醒结束时间:0x%x \n", \
         g_stAcpuPwcExcLog->core0.SlpMgrSTm, g_stAcpuPwcExcLog->core0.SlpSTm, \
         g_stAcpuPwcExcLog->core0.WkSTm, g_stAcpuPwcExcLog->core0.WkMgrSTm);
#ifndef WIN32
    PWC_TRACE(PWC_LOG_MAX, "硬件投票状态:0x%x, \n", \
         (((volatile ST_VOTE_HW_INFO *)g_aAcpuHwVoteBaseAddr[PWRCTRL_TELE_ACPU])->voteWakeStat));
#endif
#if 0
    PWC_TRACE(PWC_LOG_MAX, "################################ A核 core1 ############################\n", 0, 0, 0, 0, 0, 0);
    PWC_TRACE(PWC_LOG_MAX, "睡眠次数: 0x%x, 唤醒次数: 0x%x      \n", \
         g_stAcpuPwcExcLog->core1.SlpCnt, g_stAcpuPwcExcLog->core1.WkCnt, 0, 0, 0, 0);
    PWC_TRACE(PWC_LOG_MAX, "睡眠开始时间:0x%x, 睡眠时间:0x%x, 唤醒时间:0x%x , 唤醒结束时间:0x%x \n", \
         g_stAcpuPwcExcLog->core1.SlpMgrSTm, g_stAcpuPwcExcLog->core1.SlpSTm, \
         g_stAcpuPwcExcLog->core1.WkSTm, g_stAcpuPwcExcLog->core1.WkMgrSTm, 0, 0);

    PWC_TRACE(PWC_LOG_MAX, "################################ A核 core2 ############################\n", 0, 0, 0, 0, 0, 0);
    PWC_TRACE(PWC_LOG_MAX, "睡眠次数: 0x%x, 唤醒次数: 0x%x      \n", \
         g_stAcpuPwcExcLog->core2.SlpCnt, g_stAcpuPwcExcLog->core2.WkCnt, 0, 0, 0, 0);
    PWC_TRACE(PWC_LOG_MAX, "睡眠开始时间:0x%x, 睡眠时间:0x%x, 唤醒时间:0x%x , 唤醒结束时间:0x%x \n", \
         g_stAcpuPwcExcLog->core2.SlpMgrSTm, g_stAcpuPwcExcLog->core2.SlpSTm, \
         g_stAcpuPwcExcLog->core2.WkSTm, g_stAcpuPwcExcLog->core2.WkMgrSTm, 0, 0);

    PWC_TRACE(PWC_LOG_MAX, "################################ A核 core3 ############################\n", 0, 0, 0, 0, 0, 0);
    PWC_TRACE(PWC_LOG_MAX, "睡眠次数: 0x%x, 唤醒次数: 0x%x      \n", \
         g_stAcpuPwcExcLog->core3.SlpCnt, g_stAcpuPwcExcLog->core3.WkCnt, 0, 0, 0, 0);
    PWC_TRACE(PWC_LOG_MAX, "睡眠开始时间:0x%x, 睡眠时间:0x%x, 唤醒时间:0x%x , 唤醒结束时间:0x%x \n", \
         g_stAcpuPwcExcLog->core3.SlpMgrSTm, g_stAcpuPwcExcLog->core3.SlpSTm, \
         g_stAcpuPwcExcLog->core3.WkSTm, g_stAcpuPwcExcLog->core3.WkMgrSTm, 0, 0);
#endif

    return RET_OK;
}
#endif

s32_t pwrctrl_mcu_debug_info_show( void_t )
{
#if defined (CONFIG_HISILICON_PLATFORM_MAINTAIN)
    int i;
    u32_t  tmp = 0;
    if(NULL == g_pPwcMcuLog)
    {
        tmp = pwrctrl_read_reg32(HISI_VA_ADDRESS(MEMORY_AXI_MCU_LOG_ADDR));
        if(tmp < MCU_DUMP_LOG_ADDR || tmp > (MCU_DUMP_LOG_ADDR + MCU_DUMP_LOG_SIZE))
        {
            return 0;
        }
        if(NULL == g_exch_m_core_base_addr)
        {
            return 0;
        }
        /*g_pPwcMcuLog = (PWC_MCU_LOG_STRU *)ioremap(tmp, sizeof(PWC_MCU_LOG_STRU));*/
        g_pPwcMcuLog = (PWC_MCU_LOG_STRU *)(tmp - MCU_DUMP_LOG_ADDR + g_exch_m_core_base_addr);

    }
    PWC_TRACE(PWC_LOG_ERROR, "######## MCU Debug Info, Current slice: 0x%x ########\r\n", pwrctrl_get_slice_time());

    PWC_TRACE(PWC_LOG_ERROR, "###### SYSTEM[%d] ###### \r\n", g_pPwcMcuLog->mcu.sX);

    for(i = 0; i < PERI_SLEEP_CUR_STATE_NUM; i++)
    {
	PWC_TRACE(PWC_LOG_ERROR, "###### PERI[%d][%d] ###### \r\n", i,g_pPwcMcuLog->mcu.info[i].stat.sX);
	PWC_TRACE(PWC_LOG_ERROR, "sleep cnt: 0x%x, wake cnt: 0x%x      \r\n", \
		g_pPwcMcuLog->mcu.info[i].stat.sleepCount, g_pPwcMcuLog->mcu.info[i].stat.wakeCount);
	PWC_TRACE(PWC_LOG_ERROR, "sleep slice: 0x%x, wake slice: 0x%x, \r\n", \
		g_pPwcMcuLog->mcu.info[i].stat.sleepSliceTime, g_pPwcMcuLog->mcu.info[i].stat.wakeSliceTime);
	PWC_TRACE(PWC_LOG_ERROR, "wake src: 0x%x, 0x%x, \r\n", \
		g_pPwcMcuLog->mcu.info[i].stat.wakeSrc0, g_pPwcMcuLog->mcu.info[i].stat.wakeSrc1);
        PWC_TRACE(PWC_LOG_ERROR, "can deep sleep ret: 0x%x, hard vote:0x%x, soft vote:0x%x,\r\n", \
            g_pPwcMcuLog->mcu.info[i].notSleepReason.canSleepRet, \
            g_pPwcMcuLog->mcu.info[i].notSleepReason.HWVoteMap, \
            g_pPwcMcuLog->mcu.info[i].notSleepReason.voteMap);
        PWC_TRACE(PWC_LOG_ERROR, "vote stat:0x%x, map:0x%x, cli:0x%x, task:0x%x, slice:0x%x, \r\n", \
            g_pPwcMcuLog->vote[i].voteStat.stat, g_pPwcMcuLog->vote[i].voteStat.voteMap, \
            g_pPwcMcuLog->vote[i].voteStat.clientId, g_pPwcMcuLog->vote[i].voteStat.taskId, \
            g_pPwcMcuLog->vote[i].sliceTime);
     }

    PWC_TRACE(PWC_LOG_ERROR, "###### C core ###### \r\n");
    PWC_TRACE(PWC_LOG_ERROR, "sleep cnt: 0x%x, wake cnt: 0x%x      \r\n", \
         g_pPwcMcuLog->ccpu.pdStat.sleepCount, g_pPwcMcuLog->ccpu.puStat.wakeCount);
    PWC_TRACE(PWC_LOG_ERROR, "sleep slice: 0x%x, wake slice: 0x%x, \r\n", \
         g_pPwcMcuLog->ccpu.pdSliceTime, g_pPwcMcuLog->ccpu.puSliceTime);
    PWC_TRACE(PWC_LOG_ERROR, "WFI wait fail cnt: 0x%x, WFI wait fail slice:0x%x, \r\n", \
         g_pPwcMcuLog->ccpu.WFIFailStat.WFIFailCount, g_pPwcMcuLog->ccpu.WFIFailSliceTime);
    PWC_TRACE(PWC_LOG_ERROR, "wake src: 0x%x, 0x%x, IPC sta: 0x%x,\r\n", \
         g_pPwcMcuLog->ccpu.puStat.wakeSrc0, g_pPwcMcuLog->ccpu.puStat.wakeSrc1, \
         g_pPwcMcuLog->ccpu.puStat.wakeSrcIPC);
    PWC_TRACE(PWC_LOG_ERROR, "GPIO sta: (GPIO7-4)0x%08x, (GPIO3-0)0x%08x,\r\n", \
         *((u32_t*)&(g_pPwcMcuLog->ccpu.puStat.wakeSrcGPIO[4])), \
         *((u32_t*)&(g_pPwcMcuLog->ccpu.puStat.wakeSrcGPIO[0])));


    PWC_TRACE(PWC_LOG_ERROR, "###### A core ###### \r\n");
    PWC_TRACE(PWC_LOG_ERROR, "sleep cnt: 0x%x, wake cnt: 0x%x      \r\n", \
         g_pPwcMcuLog->acpu.pdStat.sleepCount, g_pPwcMcuLog->acpu.puStat.wakeCount);
    PWC_TRACE(PWC_LOG_ERROR, "sleep slice: 0x%x, wake slice: 0x%x, \r\n", \
         g_pPwcMcuLog->acpu.pdSliceTime, g_pPwcMcuLog->acpu.puSliceTime);
    PWC_TRACE(PWC_LOG_ERROR, "WFI wait fail cnt: 0x%x, WFI wait fail slice:0x%x, \r\n", \
         g_pPwcMcuLog->acpu.WFIFailStat.WFIFailCount, g_pPwcMcuLog->acpu.WFIFailSliceTime);
    PWC_TRACE(PWC_LOG_ERROR, "wake src: 0x%x, 0x%x, IPC sta: 0x%x, 2ndInt: 0x%x, \r\n", \
         g_pPwcMcuLog->acpu.puStat.wakeSrc0, g_pPwcMcuLog->acpu.puStat.wakeSrc1, \
         g_pPwcMcuLog->acpu.puStat.wakeSrcIPC, g_pPwcMcuLog->acpu.puStat.wake2ndInt);
    PWC_TRACE(PWC_LOG_ERROR, "GPIO sta: (GPIO7-4)0x%08x, (GPIO3-0)0x%08x,\r\n", \
         *((u32_t*)&(g_pPwcMcuLog->acpu.puStat.wakeSrcGPIO[4])), \
         *((u32_t*)&(g_pPwcMcuLog->acpu.puStat.wakeSrcGPIO[0])));


    PWC_TRACE(PWC_LOG_ERROR, "###### HIFI ###### \r\n");
    PWC_TRACE(PWC_LOG_ERROR, "sleep cnt: 0x%x, wake cnt: 0x%x      \r\n", \
         g_pPwcMcuLog->hifi.pdStat.sleepCount, g_pPwcMcuLog->hifi.puStat.wakeCount);
    PWC_TRACE(PWC_LOG_ERROR, "sleep slice: 0x%x, wake slice: 0x%x, \r\n", \
         g_pPwcMcuLog->hifi.pdSliceTime, g_pPwcMcuLog->hifi.puSliceTime);
    PWC_TRACE(PWC_LOG_ERROR, "WFI wait fail cnt: 0x%x, WFI wait fail slice:0x%x, \r\n", \
         g_pPwcMcuLog->hifi.WFIFailStat.WFIFailCount, g_pPwcMcuLog->hifi.WFIFailSliceTime);
    PWC_TRACE(PWC_LOG_ERROR, "wake src: 0x%x, 0x%x, IPC sta: 0x%x,\r\n", \
         g_pPwcMcuLog->hifi.puStat.wakeSrc0, g_pPwcMcuLog->hifi.puStat.wakeSrc1, \
         g_pPwcMcuLog->hifi.puStat.wakeSrcIPC);
#endif

    return RET_OK;
}

s32_t pwrctrl_ccpu_debug_info_show( void_t )
{
#if defined (CONFIG_HISILICON_PLATFORM_MAINTAIN)
    u32_t  tmp = 0;
    if(NULL == g_pPwcCcpuLog)
    {
        tmp = pwrctrl_read_reg32(HISI_VA_ADDRESS(MEMORY_AXI_CCPU_LOG_ADDR));
        if(tmp < MODEM_DUMP_LOG_ADDR || tmp > (MODEM_DUMP_LOG_ADDR + MODEM_DUMP_LOG_SIZE))
        {
            return 0;
        }
        if(NULL == g_exch_c_core_base_addr)
        {
            return 0;
        }
        /*g_pPwcCcpuLog = (PWC_CCPU_LOG_STRU *)ioremap(tmp, sizeof(PWC_CCPU_LOG_STRU));*/
        g_pPwcCcpuLog = (PWC_CCPU_LOG_STRU *)(tmp - MODEM_DUMP_LOG_ADDR + g_exch_c_core_base_addr);

    }

    PWC_TRACE(PWC_LOG_ERROR, "######## CCPU Debug Info, Current slice: 0x%x ########\r\n", pwrctrl_get_slice_time());
    PWC_TRACE(PWC_LOG_ERROR, "sleep cnt: 0x%x, wake cnt: 0x%x      \r\n", \
        g_pPwcCcpuLog->ccpu.stat.cSleepCount, g_pPwcCcpuLog->ccpu.stat.cWakeCount);
    PWC_TRACE(PWC_LOG_ERROR, "sleep start slice: 0x%x, sleep slice: 0x%x, wake slice: 0x%x, wake end slice: 0x%x, \r\n", \
        g_pPwcCcpuLog->ccpu.stat.cSleepMgrSliceTime, g_pPwcCcpuLog->ccpu.stat.cSleepSliceTime, \
        g_pPwcCcpuLog->ccpu.stat.cWakeSliceTime, g_pPwcCcpuLog->ccpu.stat.cWakeMgrSliceTime);
    PWC_TRACE(PWC_LOG_ERROR, "IPC sleep cnt: 0x%x, IPC sleep slice: 0x%x, WFI return cnt: 0x%x, WFI return slice:0x%x, \r\n", \
        g_pPwcCcpuLog->ccpu.stat.cSleepIpcCount, g_pPwcCcpuLog->ccpu.stat.cSleepIpcSliceTime, \
        g_pPwcCcpuLog->ccpu.stat.cWFIRetCount, g_pPwcCcpuLog->ccpu.stat.cWFIRetSliceTime);
    PWC_TRACE(PWC_LOG_ERROR, "can deep sleep ret: 0x%x, hard vote:0x%x, soft vote:0x%x, sch time id:0x%x, sch time val:0x%x,\r\n", \
        g_pPwcCcpuLog->ccpu.notSleepReason.canSleepRet, g_pPwcCcpuLog->ccpu.notSleepReason.HWVoteMap, \
        g_pPwcCcpuLog->ccpu.notSleepReason.voteMap, g_pPwcCcpuLog->ccpu.notSleepReason.nextSchTimeId, \
        g_pPwcCcpuLog->ccpu.notSleepReason.nextSchTimeValue);
    PWC_TRACE(PWC_LOG_ERROR, "vote stat:0x%x, map:0x%x, cli:0x%x, task:0x%x, slice:0x%x, \r\n", \
        g_pPwcCcpuLog->vote.voteStat.stat, g_pPwcCcpuLog->vote.voteStat.voteMap, \
        g_pPwcCcpuLog->vote.voteStat.clientId, g_pPwcCcpuLog->vote.voteStat.taskId, \
        g_pPwcCcpuLog->vote.sliceTime);
#endif

    return RET_OK;
}

static s32_t __init pwrctrl_late_init(void_t)
{
#if defined (CONFIG_HISILICON_PLATFORM_MAINTAIN)
    if(g_exch_a_core_base_addr)
        g_pPwcAcpuLog = (PWC_ACPU_LOG_STRU*)PWRCTRL_EXC_ACPU_ADDR;
#endif

    return RET_OK;
}
late_initcall(pwrctrl_late_init);

#if 0
s32_t pwrctrl_mem_show( u32_t addr )
{
    u32_t *addrV=0;

    addrV = (u32_t *)ioremap(addr, 0x1000);

    PWC_TRACE(PWC_LOG_ERROR, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x \r\n", \
         addr, *(addrV++), *(addrV++), *(addrV++), *(addrV++));

    PWC_TRACE(PWC_LOG_ERROR, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x \r\n", \
         (addr+0x10), *(addrV++), *(addrV++), *(addrV++), *(addrV++));

    PWC_TRACE(PWC_LOG_ERROR, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x \r\n", \
         (addr+0x20), *(addrV++), *(addrV++), *(addrV++), *(addrV++));

    PWC_TRACE(PWC_LOG_ERROR, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x \r\n", \
         (addr+0x30), *(addrV++), *(addrV++), *(addrV++), *(addrV++));

    iounmap(addrV);

    return RET_OK;
}




void_t pwrctrl_log_mem(u8_t* info, void_t* addr, u32_t size)
{
    u32_t idx;

    if(!info || !addr)
    {
        PWC_TRACE(PWC_LOG_ERROR, "pwrctrl_log_mem: null parameters.");

        return ;
    }

    PWC_TRACE(PWC_LOG_INFO, "%s, 0x%ux, %u", info, (u32_t)addr, size);

    for(idx = 0; idx < size; idx = idx + 16)
    {
        PWC_TRACE(PWC_LOG_INFO, " 0x%.8X:  0x%.8X  0x%.8X  0x%.8X  0x%.8X ", (int)addr + idx,
            *(int*)((int)addr + idx), *(int*)((int)addr + idx + 0x4), *(int*)((int)addr + idx + 0x8), *(int*)((int)addr + idx + 0xC));
    }

}




late_initcall(pwrctrl_common_initial);
#endif
arch_initcall(pwrctrl_common_init);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
