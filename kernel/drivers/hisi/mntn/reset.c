/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 *  reset.c    hisi watchdog v100 kernel device driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */
#define MERROR_LEVEL    1
#define MWARNING_LEVEL  1
#define MNOTICE_LEVEL   1
#define MINFO_LEVEL     0
#define MDEBUG_LEVEL    0
#define MLOG_TAG        "reset"


#include <linux/module.h>           /* For module specific items */
#include <linux/moduleparam.h>      /* For new moduleparam's */
#include <linux/types.h>            /* For standard types (like size_t) */
#include <linux/errno.h>            /* For the -ENODEV/... values */
#include <linux/kernel.h>           /* For printk/... */
#include <linux/init.h>             /* For __init/__exit/... */
#include <linux/fs.h>               /* For file operations */
#include <linux/wakelock.h>         /* For file operations */
#include <linux/ioport.h>           /* For io-port access */
#include <linux/spinlock.h>         /* For spin_lock/spin_unlock/... */
#include <linux/suspend.h>
#include <linux/completion.h>
#include <linux/uaccess.h>          /* For copy_to_user/put_user/... */
#include <linux/workqueue.h>        /* For workqueue... */
#include <linux/notifier.h>         /* For notifier... */
#include <linux/interrupt.h>        /* For request_irq*/
#include <linux/io.h>               /* For writel */
#include <linux/reboot.h>               /* For writel */
#include <general_sram_map.h>
#include <soc_modem_sctrl_interface.h>
#include "soc_peri_sctrl_interface.h"
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include <soc_ao_sctrl_interface.h>

#include <soc_irqs.h>
#include <linux/hisi/reset.h>
#include <soc_wdt_interface.h>
#include <soc_edmac_interface.h>
#include <linux/delay.h>
#include "partition.h"
#include "mntn_log.h"
#include "excDrv.h"
#include "bsp_drv_ipc.h"      /*for ipc api*/
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include "../mailbox/hi6xxx_mailbox/drv_mailbox.h"
#include "mntn_save_logdata.h"
#ifndef CONFIG_ARM64
#include <linux/pm_qos.h>
#include <linux/hardirq.h>
#endif

#define ANDROID_REBOOT_MAGIC 0xe8c7b756

#define DRV_RESET_NAME_MAX        (255)
#define DRV_RESET_BLK_MAX         (50)
#define DRV_RESET_BLK_DEV          "/dev/block/mmcblk0p"


static int in_suspend_state = 0;
static struct workqueue_struct *reset_wq = NULL;
static struct wake_lock reset_wake_lock;
static DECLARE_COMPLETION(reset_suspend_completed);
static struct notifier_block reset_pm_notifier;

static void __iomem *gic_base_addr = NULL;
static unsigned long ap_dmac_base = 0;
static unsigned long hifi_wdt_addr = 0;


static unsigned int s_file_store_ok = FALSE;
static ereset_irq_type s_reset_irq_type = BSP_RESET_NONE;


static char *reboot_reason[] = {"modem_panic", "modem_freeze", "modem_reboot", \
                                "hifi_freeze", "mcu_panic" };

/*declare delayed work*/
static struct delayed_work init_reset_irq_work;

/*declare delayed work*/
static struct delayed_work delay_to_do_sh_work;

static unsigned int   g_cpu_status;


void hifi_freeze_give_semaphone(void);
void android_freeze_give_semaphone(void);
void mcu_freeze_give_semaphone(void);
void mcu_panic_give_semaphone(void);
void android_reboot_give_semaphone(void);
void wdg_rstlog_save_give_semaphone(void);
void acpuwdg_historylog_write_give_semaphone(void);
void mculocalwdg_historylog_write_give_semaphone(void);
void mcuglbwdg_historylog_write_give_semaphone(void);
void mcu_puzzle_give_semaphone(void);
int platform_ddr_protect_init(int flag);
extern unsigned int omTimerGet(void);
static void acore_trigger_othercore_fiq(struct work_struct *work);
void break_for_debug(void);
void do_hifi_runstall(void);
#ifndef _DRV_LLT_
void Reset_save_delayed_log(void);
void Reset_set_fileok_flag(void);
#endif

extern unsigned int is_cpu_power_on(int cpu);
extern void platform_cpu_power_on(int cpu);
extern void platform_cpu_power_down(int cpu);
extern int acpu_core_pd_request_state(void);

extern int g_mcu_panic_cpuidle;
int g_mcu_panic_cpuidle = 0;
extern void arch_send_call_function_ipi_mask(const struct cpumask *mask);
extern const struct cpumask *const cpu_online_mask;

static int g_core_cpuidle_state[8];
static void __iomem *acpu_master_core_state_vbase = 0;
static void __iomem *mcu_exception_flags_vbase = 0;

int is_mcu_exception(void)
{
    return (0x911009 == readl(mcu_exception_flags_vbase));
}

void set_acore_state(unsigned int core, int state)
{
    if(0 == core) {
        writel(state, acpu_master_core_state_vbase);
    } else {
        g_core_cpuidle_state[core] = state;
    }
    return;
}

void clear_acore_state(unsigned int core)
{
    g_core_cpuidle_state[core] = 0;
    return;
}

static void power_on_core_do_work(void)
{
    unsigned int i =0;
    int core_state =0;
    printk(KERN_INFO "power_on_core_do_work in\n");
    mdelay(2);
    core_state = acpu_core_pd_request_state();
    for (i = 0; i < 8; i++)
    {
       if(core_state & (1<<i) || (g_core_cpuidle_state[i] > 0))
       {
            platform_cpu_power_down(i);
       }
    }
    arch_send_call_function_ipi_mask(cpu_online_mask);
    for (i = 1; i < 8; i++)
    {
     if((cpu_online(i)) &&(!is_cpu_power_on(i)))
         platform_cpu_power_on(i);
    }
    printk(KERN_INFO "power_on_core_do_work out\n");

    return;
}

#define RESET_WORK_DECLARE(name, type)\
\
static BLOCKING_NOTIFIER_HEAD(name ## _notifier_list);\
\
static struct type name ## _work;\
\
static void name ## _do_work(struct work_struct *work)\
{\
    if(strcmp(#name,"mcu_panic")==0 ){\
           power_on_core_do_work();\
       }\
    if (1 == in_suspend_state){\
        if (!wait_for_completion_timeout(&reset_suspend_completed, HZ*10))\
            goto fail;\
    }\
\
    acore_trigger_othercore_fiq(work);\
    blocking_notifier_call_chain(&name ## _notifier_list, 0, NULL);\
    name ## _give_semaphone();\
    return;\
\
fail:\
    machine_restart("system halt");\
    return;\
}\
\
int name ## _regsister_notifier(struct notifier_block *n)\
{\
    return  blocking_notifier_chain_register(&name ## _notifier_list, n);\
}\
EXPORT_SYMBOL(name ## _regsister_notifier);\
\
int name ## _unregsister_notifier(struct notifier_block *n)\
{\
    return  blocking_notifier_chain_unregister(&name ## _notifier_list, n);\
}\
EXPORT_SYMBOL(name ## _unregsister_notifier);


#define RESET_WORK_INIT(name)\
INIT_WORK(&name ## _work, name ## _do_work)

/*declare reset work*/
RESET_WORK_DECLARE(mcu_panic, work_struct)

RESET_WORK_DECLARE(hifi_freeze, work_struct)
//RESET_WORK_DECLARE(android_freeze, delayed_work)
//RESET_WORK_DECLARE(android_reboot, delayed_work)
//RESET_WORK_DECLARE(mcu_puzzle, work_struct)

#if 1 /*Modify_for_c_reset, l00212112,20130511*/
RESET_WORK_DECLARE(mcu_freeze, work_struct)
#else
RESET_WORK_DECLARE(mcu_freeze, delayed_work)
#endif

//RESET_WORK_DECLARE(wdg_rstlog_save, delayed_work)
//RESET_WORK_DECLARE(acpuwdg_historylog_write, delayed_work)
//RESET_WORK_DECLARE(mculocalwdg_historylog_write, delayed_work)
//RESET_WORK_DECLARE(mcuglbwdg_historylog_write, delayed_work)

static int reset_pm_notify(struct notifier_block *notify_block,
                    unsigned long mode, void *unused)
{
    switch (mode) {
    case PM_HIBERNATION_PREPARE:
    case PM_SUSPEND_PREPARE:
        in_suspend_state = 1;
        break;

    case PM_POST_SUSPEND:
    case PM_POST_HIBERNATION:
    case PM_POST_RESTORE:
        complete(&reset_suspend_completed);
        in_suspend_state = 0;
        break;
    }

    return 0;
}


static void print_banner(char *info)
{
    printk(KERN_ERR "############################\n");
    printk(KERN_ERR "timestamp: %d\n", omTimerGet());
    printk(KERN_ERR " %s\n", info);
    printk(KERN_ERR "############################\n");
}


static void prepare_reset(void)
{
    wake_lock(&reset_wake_lock);

    BSP_IPC_IntDisable(IPC_ACPU_INT_SRC_CCPU_EXC_REBOOT);

    BSP_IPC_IntDisable(IPC_ACPU_INT_SRC_CCPU_NORMAL_REBOOT);

    BSP_IPC_IntDisable(IPC_ACPU_INI_SRC_MCU_EXC_REBOOT);

    disable_irq_nosync(IRQ_WD3);

    disable_irq_nosync(IRQ_WD1);

    disable_irq_nosync(IRQ_CM3WD0);

    platform_ddr_protect_init(0); /*just for compile by x00195127*/

    return;
}
static void stop_watchdog1(void)
{
    if(!hifi_wdt_addr){
        printk(KERN_ERR "Bad WatchDog1 address\n");
        return;
    }

    printk(KERN_INFO "RESET LOG: stop_watchdog1_begin!\n");

    /* First, Unlock Wd1 to allow modifying registers */
    writel(DRV_WATCHDOG_UNLOCK_NUM, (void __iomem *)SOC_WDT_WDG_LOCK_ADDR(hifi_wdt_addr));
    /* Clear  Wd1 Interrupt register */
    writel(DRV_WATCHDOG_INTCLR_NUM, (void __iomem *)SOC_WDT_WDG_INTCLR_ADDR(hifi_wdt_addr));
    /* Disable Wd1  */
    writel(DRV_WATCHDOG_CONTROL_DISABLE, (void __iomem *)SOC_WDT_WDG_CONTROL_ADDR(hifi_wdt_addr));
    /* Finally, Lock Wd1 */
    writel(DRV_WATCHDOG_LOCK_NUM, (void __iomem *)SOC_WDT_WDG_LOCK_ADDR(hifi_wdt_addr));

    printk(KERN_INFO "RESET LOG: stop_watchdog1_end!\n");

}
/*runinstall hifi*/
void do_hifi_runstall(void)
{
    SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_UNION runstall_val;
    unsigned long sctrl_off = (unsigned long)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);

    /* runstall hifi */
    runstall_val.value             = 0x0;
    runstall_val.reg.hifi_runstall = 0x1;

    *(volatile unsigned int *)SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_ADDR(sctrl_off) = runstall_val.value;
}
/*reset system after saving log*/

static irqreturn_t hifi_freeze_irq(int irqno, void *param)
{
    SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_UNION runstall_val;
    unsigned long sctrl_off = (unsigned long)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);

    if(!ap_dmac_base){
           printk(KERN_ERR "Bad ap_dmac_base address\n");
           return IRQ_NONE;
    }

    print_banner("oops : hifi freeze ...");

    /* Stop WD1 */
    stop_watchdog1();

    /* runstall hifi */
    runstall_val.value             = 0x0;
    runstall_val.reg.hifi_runstall = 0x1;

    writel(runstall_val.value, (void __iomem *)SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_ADDR(sctrl_off));

    /* close dma, channel 1 - audio pcm play
        channel 2 - audio pcm capture
        channel 3 - voice pcm mic
        channel 4 - voice pcm spk
        channel 5 - voice pcm smart PA */
    writel(0, (void __iomem *)SOC_EDMAC_CX_CONFIG_ADDR(ap_dmac_base, 1));
    writel(0, (void __iomem *)SOC_EDMAC_CX_CONFIG_ADDR(ap_dmac_base, 2));
    writel(0, (void __iomem *)SOC_EDMAC_CX_CONFIG_ADDR(ap_dmac_base, 3));
    writel(0, (void __iomem *)SOC_EDMAC_CX_CONFIG_ADDR(ap_dmac_base, 4));
    writel(0, (void __iomem *)SOC_EDMAC_CX_CONFIG_ADDR(ap_dmac_base, 5));


/*Modify_for_c_reset, l00212112,20130511 ,starts*/
    /*clean hifi's wdt int*/
    reset_clean_wdt_int(BSP_RESET_MODULE_HIFI);
/*Modify_for_c_reset, l00212112,20130511 ,ends*/

    prepare_reset();

    queue_work(reset_wq, &hifi_freeze_work);

    return IRQ_HANDLED;
}


/*Modify_for_c_reset, l00212112,20130511 ,starts*/

static irqreturn_t mcu_freeze_irq(int irqno, void *param)
{
    print_banner("oops : mcu freeze ....\n");
    prepare_reset();

    break_for_debug();
    /*do as panic*/
    queue_work(reset_wq, &mcu_freeze_work);

    return IRQ_HANDLED;
}
#ifndef _DRV_LLT_
static int fiq_nmi_debug = 0;
void acore_trigger_othercore_fiq(struct work_struct *work)
{
    unsigned long sctrl_on = (unsigned long)ioremap(SOC_AO_SCTRL_BASE_ADDR, 4096);
    //unsigned long sctrl_modem = (unsigned long)ioremap(SOC_MODEM_SCTRL_BASE_ADDR, 4096);
    unsigned int    ureg_val = 0;

    if (1== fiq_nmi_debug)
    {
        return;
    }
    /*trigger c core's fiq*/

    if (&mcu_freeze_work == work)/*trigger m core's nmi*/
    {
        /*enable intr*/
        ureg_val = readl(SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_ADDR(sctrl_on));
        writel((ureg_val | BIT(SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_0bit_START)), SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_ADDR(sctrl_on));

        ureg_val = readl(SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_ADDR(sctrl_on));
        writel((ureg_val | BIT(SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_cm3_sft_nmi_intreq_START)), SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_ADDR(sctrl_on));
        printk(KERN_ERR"acore trigger mcu fiq, val = 0x%x\n", ureg_val);
        mdelay(100);
        /*clear int*/
     //   writel((ureg_val & ~BIT(SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_cm3_sft_nmi_intreq_START)), SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_ADDR(sctrl_on));
    }
    //iounmap((void*)sctrl_modem);
    iounmap((void*)sctrl_on);
}
#endif

void hifi_freeze_give_semaphone(void)
{
    mntn_hifi_reset_save_log("hifi freeze ...\n");
    reset_hifi_up_semaphone();
}

void android_freeze_give_semaphone(void)
{
    /*do nothing*/
}
void mcu_freeze_give_semaphone(void)
{
    mntn_mcu_reset_save_log("mcu freeze...\n");
}
void mcu_panic_give_semaphone(void)
{
    mntn_mcu_reset_save_log("mcu panic...\n");
}
void break_for_debug(void)
{
    /*for trace32 debugging*/
    if (check_himntn(HIMNTN_CLOSE_ACPU_PANIC_LOOP) == 0) {
        do {
        } while (1);
    }
}

void android_reboot_give_semaphone(void)
{
    /*do nothing*/
}

void wdg_rstlog_save_give_semaphone(void)
{
    /*do nothing*/
}

void acpuwdg_historylog_write_give_semaphone(void)
{
    /*do nothing*/
}

void mculocalwdg_historylog_write_give_semaphone(void)
{
    /*do nothing*/
}

void mcuglbwdg_historylog_write_give_semaphone(void)
{
    /*do nothing*/
}
void mcu_puzzle_give_semaphone(void)
{
    /*do nothing*/
}


void finish_reset_sub(void)
{
    platform_ddr_protect_init(1); /*just for compile by x00195127*/
    BSP_IPC_IntEnable(IPC_ACPU_INT_SRC_CCPU_EXC_REBOOT);

    BSP_IPC_IntEnable(IPC_ACPU_INT_SRC_CCPU_NORMAL_REBOOT);

    BSP_IPC_IntEnable(IPC_ACPU_INI_SRC_MCU_EXC_REBOOT);

    enable_irq(IRQ_CM3WD0);

    enable_irq(IRQ_WD1);
    enable_irq(IRQ_WD3);
    wake_unlock(&reset_wake_lock);
    return;
}
/*****************************************************************************
 函 数 名  : reset_set_cpu_status
 功能描述  : 当hifi或者ccpu复位时，记下状态，用于mailbox拦击发向hifi  的mail.
 输入参数  : unsigned int iOff  1-不使能;0-关闭
             unsigned int iflag 标识cpu状态对应的偏移
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/

/*****************************************************************************
 函 数 名  : BSP_CPU_StateSet
 功能描述  : 当hifi或者ccpu复位时，调用此函数记录cpu状态
             mailbox可以依此拦截发往hifi、ccpu、mcu等core的mail
 输入参数  : unsigned int iOff  :1-不使能(不能发送);0-关闭(mail可以发送)
             unsigned int offset:标识cpu状态对应的偏移，
             该入参仅可使用reset.h中的如下宏
             #define RESET_CPU_CCORE_STATUS_OFF            0x00000001
             #define RESET_CPU_MCU_STATUS_OFF              0x00000002
             #define RESET_CPU_HIFI_STATUS_OFF             0x00000004

 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void BSP_CPU_StateSet(unsigned int iOff,unsigned int offset)
{
    if (1 == iOff)
    {
        g_cpu_status = g_cpu_status | offset;

    }
    else
    {
        g_cpu_status = g_cpu_status & (~offset);

    }
}


/*****************************************************************************
 函 数 名  : BSP_CPU_StateGet
 功能描述  : 提供给mailbox模块，用于获得相关cpu状态
 输入参数  : int CpuID, MCU, HIFI,CCPU
 输出参数  : 无
 返 回 值  : int,1:使能；0:不使能
*****************************************************************************/
int BSP_CPU_StateGet(int CpuID)
{
    unsigned int u32CpuStatusOffset = 0;

    switch (CpuID)
    {
    case MAILBOX_CPUID_HIFI:
        u32CpuStatusOffset = RESET_CPU_HIFI_STATUS_OFF;
        break;
    case MAILBOX_CPUID_CCPU:
        u32CpuStatusOffset = RESET_CPU_CCORE_STATUS_OFF;
        break;
    default:
        break;
    }

    return (g_cpu_status & u32CpuStatusOffset) ? 0 : 1;
}

/*****************************************************************************
 函 数 名  : reset_clean_wdt_int
 功能描述  : 收到复位中断后，清理c核/hifi狗中断
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_clean_wdt_int(ereset_module emodule)
{
#ifdef _DRV_LLT_
#else
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE

	unsigned long sctrl_on  = (unsigned long)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);

    switch (emodule)
    {
    case BSP_RESET_MODULE_CCORE:
        writel(0x10, (void __iomem *)SOC_AO_SCTRL_SC_INT_CLEAR_ADDR(sctrl_on));
        break;
    case BSP_RESET_MODULE_HIFI:
        writel(0x4, (void __iomem *)SOC_AO_SCTRL_SC_INT_CLEAR_ADDR(sctrl_on));
        break;
    default:
        break;
    }
    printk(KERN_INFO "%s: reset_clean_wdt_int %d\n", __FUNCTION__, emodule);
#endif
#endif
}


/*Modify_for_c_reset, l00212112,20130511, ends*/
#ifndef _DRV_LLT_
/*****************************************************************************
 Description : save log file that is delayed to save, e.g, modem panic before file system is ok
  History
  1.Date: 2014/7/8
    Author :
    Modification : Created function
*****************************************************************************/
void Reset_set_fileok_flag(void)
{
    s_file_store_ok = TRUE;
}
/*****************************************************************************
 Description : save log file that is delayed to save, e.g, modem panic before file system is ok
  History
  1.Date: 2014/7/8
    Author :
    Modification : Created function
*****************************************************************************/
void Reset_save_delayed_log(void)
{
    switch (s_reset_irq_type)
    {
    case BSP_RESET_TYPE_MCU_PANIC:
        s_reset_irq_type = BSP_RESET_NONE;
        queue_work(reset_wq, &mcu_panic_work);
        break;
    default:
        printk(KERN_ERR"Reset_save_delayed_log,type is %d \n",s_reset_irq_type);
        break;
    }
}
static void delay_to_runsh_do_work(struct work_struct *work)
{
    switch (s_reset_irq_type)
    {
    case BSP_RESET_TYPE_MCU_PANIC:
        s_reset_irq_type = BSP_RESET_NONE;
        mcu_panic_do_work(&mcu_panic_work);
        break;
    default:
        printk(KERN_ERR"delay_to_runsh_do_work,type is %d \n",s_reset_irq_type);
        break;
    }
    s_file_store_ok = TRUE;
}
#endif


static void mcu_panic_irq(unsigned int param)
{
    print_banner("oops : mcu panic ...");

    prepare_reset();

    break_for_debug();
    if (FALSE == s_file_store_ok)
    {
        s_reset_irq_type = BSP_RESET_TYPE_MCU_PANIC;
        printk(KERN_ERR "mcu panic but delay to save log\n");
        /*delay 20 second*/
        queue_delayed_work(reset_wq, &delay_to_do_sh_work, 20*HZ);

        return;
    }
    queue_work_on(0,reset_wq, &mcu_panic_work);

    return;
}
static void init_reset_irq_do_work(struct work_struct *work)
{
    int ret;

    hifi_wdt_addr = (unsigned long)(ioremap_nocache(SOC_Watchdog2_BASE_ADDR, 4096));
    ap_dmac_base = (unsigned long)(ioremap_nocache(SOC_AP_DMAC_BASE_ADDR, 4096));


    wake_lock_init(&reset_wake_lock, WAKE_LOCK_SUSPEND, "reset_wake_lock");

    reset_pm_notifier.notifier_call = reset_pm_notify;
    reset_pm_notifier.priority = INT_MIN;
    register_pm_notifier(&reset_pm_notifier);
    /*modem单独复位注册notifier*/

    RESET_WORK_INIT(mcu_panic);
    RESET_WORK_INIT(hifi_freeze);

/*Modify_for_c_reset, l00212112,20130511, starts*/
    RESET_WORK_INIT(mcu_freeze);
/*Modify_for_c_reset, l00212112,20130511, ends*/
    /*init delayed work*/
    INIT_DELAYED_WORK(&delay_to_do_sh_work, delay_to_runsh_do_work);

    /*register exception irq handles */
    ret = request_irq(IRQ_WD1, hifi_freeze_irq, 0, "hifi_freeze",  NULL);
    if (0 != ret) {
        printk(KERN_ERR "failed to install hifi freezing irq handle (%d)\n", ret);
    }


/*Modify_for_c_reset, l00212112,20130511, starts*/
    ret = request_irq(IRQ_CM3WD0, mcu_freeze_irq, 0, "mcu_freeze",  NULL);
    if (0 != ret) {
        printk(KERN_ERR "failed to install mcu freezing irq handle (%d)\n", ret);
    }
/*Modify_for_c_reset, l00212112,20130511, ends*/



    ret = BSP_IPC_IntConnect(IPC_ACPU_INI_SRC_MCU_EXC_REBOOT, mcu_panic_irq, 0);
    if (0 != ret){
        printk(KERN_ERR "failed to connect IPC irq handle (%d)\n", IPC_ACPU_INI_SRC_MCU_EXC_REBOOT);
    }
    BSP_IPC_IntEnable(IPC_ACPU_INI_SRC_MCU_EXC_REBOOT);

    return;
}


static int reset_module_init(void)
{
    BUG_ON(!reset_wq);

    gic_base_addr = ioremap(REG_BASE_GIC, REG_GIC_IOSIZE);
    BUG_ON(!gic_base_addr);

    /*init delayed work*/
    INIT_DELAYED_WORK(&init_reset_irq_work, init_reset_irq_do_work);


    /*delay 2 second*/
    queue_delayed_work(reset_wq, &init_reset_irq_work, RESET_MOUDLE_DELAY_TIME*HZ);

    return 0;
}
static void __exit reset_device_module_exit(void)
{


    BSP_IPC_IntDisonnect(IPC_ACPU_INI_SRC_MCU_EXC_REBOOT, mcu_panic_irq, 0);

    free_irq(IRQ_CM3WD0, NULL);

    free_irq(IRQ_WD1, NULL);



    iounmap((void*)ap_dmac_base);
    iounmap((void*)hifi_wdt_addr);

    unregister_pm_notifier(&reset_pm_notifier);

    return;
}



static int __init system_freeze_flag_init(void)
{
    /*unsigned long sctrl_base = (unsigned long)HISI_VA_ADDRESS(SOC_SC_ON_BASE_ADDR);*/
    /*SOC_AO_SCTRL_SC_SYS_STAT0_UNION *stat0 = (SOC_AO_SCTRL_SC_SYS_STAT0_UNION *)SOC_AO_SCTRL_SC_SYS_STAT0_ADDR(sctrl_base);*/

    reset_wq = create_singlethread_workqueue("reset_wq");
    BUG_ON(!reset_wq);

    acpu_master_core_state_vbase = ioremap(ACPU_MASTER_CORE_STATE_ADDR, PAGE_ALIGN(SZ_4K));
    mcu_exception_flags_vbase = ioremap(MCU_EXCEPTION_FLAGS_ADDR, PAGE_ALIGN(SZ_4K));

    return 0;
}



static void __exit system_freeze_flag_exit(void)
{
    /*to some code when module exit*/
    iounmap(acpu_master_core_state_vbase);
    iounmap(mcu_exception_flags_vbase);

    return;
}

#ifdef BSP_C_HIFI_RESET_ALONE_TESET_VALID

int simulate_irq_hifi_reset(void)
{
    hifi_freeze_irq(0,0);
    return 0;
}
int simulate_irq_mcuwdt_reset(void)
{
    mcu_freeze_irq(0, 0);
    return 0;
}

#endif

#if 0
void save_mcu_log_info(void)
{
    blocking_notifier_call_chain(&mcu_puzzle_notifier_list, 0, NULL);
    printk("save_mcu_log_info over\n");
}
#endif

static int android_reboot_notify(struct notifier_block *this,
            unsigned long code, void *reason)
{
    unsigned long addr = (unsigned long) HISI_VA_ADDRESS(MEMORY_AXI_ANDROID_REBOOT_FLAG_ADDR);
    unsigned int val = ANDROID_REBOOT_MAGIC;
    char *cmd = (char *)reason;
    int i;

    /*skip none SYS_RESTART*/
    if (code != SYS_RESTART){
        goto out;
    }

    /*normal reboot*/
    if (NULL == cmd){
        goto out;
    }

    /*skip this case*/
    for (i = 0; i< ARRAY_SIZE(reboot_reason); i++){
        if (0 == strncmp(cmd, reboot_reason[i], strlen(reboot_reason[i]))){
            val = 0;
            goto out;
        }
    }

out:
    writel(val, (void __iomem *)addr);

    return NOTIFY_DONE;
}

static struct notifier_block android_reboot_notifier = {
    .notifier_call = android_reboot_notify,
    .priority = INT_MAX,
};


static int __init android_reboot_flag_init(void)
{
    unsigned long addr = (unsigned long) HISI_VA_ADDRESS(MEMORY_AXI_ANDROID_REBOOT_FLAG_ADDR);
    int err;

    writel(0, (void __iomem *)addr);

    err = register_reboot_notifier(&android_reboot_notifier);
    if (err) {
            return -1;
    }

    return 0;
}


static void __exit android_reboot_flag_exit(void)
{
    /*to some code when module exit*/

    return;
}

arch_initcall(system_freeze_flag_init);

module_init(android_reboot_flag_init);

module_init(reset_module_init);


module_exit(android_reboot_flag_exit);

module_exit(system_freeze_flag_exit);

module_exit(reset_device_module_exit);

void test_fiq(void)
{
    unsigned int ulRegValue = 0;

    void __iomem *addr = NULL;

    addr = ioremap(SOC_AO_SCTRL_BASE_ADDR, SZ_8K);
    if (!addr){
        printk(KERN_ERR "%s : failed to ioremap AO SCTRL \r\n", __FUNCTION__);
        return;
    }

    writel(BIT(SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_4acpu_sft_fiq_START), SOC_AO_SCTRL_SC_INT_EN_SET_ADDR(addr));
    ulRegValue = BIT(SOC_AO_SCTRL_SC_PW_CTRL1_acpu_sft_fiq_req_START) | BIT(SOC_AO_SCTRL_SC_PW_CTRL1_acpu_sft_fiq_req_msk_START);
    writel(ulRegValue,SOC_AO_SCTRL_SC_PW_CTRL1_ADDR(addr));

    iounmap(addr);
}

int drv_read_bin(const char *partion_name, unsigned int offset, unsigned int length, char *buffer)
{
    int ret          = -1;
    int i            = 0;
    int arry_number  = 0;
    char *pathname   = NULL;
    char blk_number[DRV_RESET_BLK_MAX] = {0};
    struct file *fp;
    mm_segment_t fs;
    loff_t pos = 0;

    if((NULL == partion_name) || (NULL == buffer)) {
        mlog_e("partion_name(%p) or buffer(%p) is null", partion_name, buffer);

        return BSP_RESET_ERROR;
    }

    /*获取资源*/
    pathname = (char *)kmalloc(DRV_RESET_NAME_MAX, GFP_KERNEL);
    if (!pathname) {
        mlog_e("pathname malloc failed" );
        return BSP_RESET_ERROR;
    }

    /*从C核访问的目录文件转换为A核目录文件*/
    arry_number = sizeof(partitions) / sizeof(struct partition);

    /*打开文件*/
    for (i = 0; i < arry_number; i++) {
        if (NULL == partitions[i].name) {
            mlog_e("partitions[%d].name: now we get a NULL pointer!", i);
            continue;
        }

        /*相等*/
        if (!strcmp(partion_name, partitions[i].name)) {
            mlog_n("partitions[%d]:%s is we want(%s)", i, partitions[i].name, partion_name);
            memset(pathname, 0, DRV_RESET_NAME_MAX);
            strncat(pathname, DRV_RESET_BLK_DEV, DRV_RESET_NAME_MAX-1);

            sprintf(blk_number, "%d", i);
            strncat(pathname, blk_number, DRV_RESET_NAME_MAX-1);

            break;
        } else {
            mlog_i("partitions[%d/%d]:%s is not we want(%s)", i, arry_number, partitions[i].name, partion_name);
        }
    }

    if (i >= arry_number) {
        mlog_e("partion_name(%s) is not in partion table!\n", partion_name);
        goto error;
    }

    fp = filp_open(pathname, O_RDONLY, 0600);
    if (IS_ERR(fp)) {
        mlog_e("filp_open(%s) failed", pathname);
        goto error;
    }

    ret = vfs_llseek(fp, offset, SEEK_SET);
    if(ret < 0) {
        mlog_e("seek ops failed, ret %d", ret);
        goto error2;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);

    pos = fp->f_pos;
    ret = vfs_read(fp, (char __user *)buffer, length, &pos);
    if (ret != length) {
        mlog_e("read ops failed, ret=%d(len=%d)", ret, length);
        set_fs(fs);
        goto error2;
    }
    set_fs(fs);

    filp_close(fp, NULL);


    /*释放资源*/
    if(NULL != pathname) {
       kfree(pathname);
       pathname = NULL;
    }

    mlog_i("success\n");

    return BSP_RESET_OK;

error2:
    filp_close(fp, NULL);

error:

    /*释放资源2*/
    if(NULL != pathname) {
       kfree(pathname);
       pathname = NULL;
    }

    mlog_i("failed");

    return BSP_RESET_ERROR;

}
EXPORT_SYMBOL(drv_read_bin);



MODULE_DESCRIPTION("Hisilicon device RESET Management module");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuyiping <xuyiping@huawei.com>");

