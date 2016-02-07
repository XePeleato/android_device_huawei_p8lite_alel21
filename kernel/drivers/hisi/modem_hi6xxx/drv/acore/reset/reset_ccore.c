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
#define MLOG_TAG        "reset_ccore"


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
#include "BSP.h"
#include <soc_irqs.h>
#include <linux/hisi/reset.h>
#include <soc_wdt_interface.h>
#include <soc_edmac_interface.h>
#include <linux/delay.h>
#include "partition.h"
#include "mntn_log.h"
#include "excDrv.h"
#include "bsp_drv_ipc.h"      /*for ipc api*/
#include "reset_sub_ccore.h"

#ifndef CONFIG_ARM64
#include <linux/pm_qos.h>
#include <linux/hardirq.h>
#endif

#define DRV_RESET_NAME_MAX        (255)
#define DRV_RESET_BLK_MAX         (50)
#define DRV_RESET_BLK_DEV          "/dev/block/mmcblk0p"
/*c核单独复位模块初始化任务初始化延迟时间*/
#define CCORE_RESET_MOUDLE_DELAY_TIME          (2)


static int ccore_in_suspend_state = 0;
static struct workqueue_struct *reset_wq_ccore = NULL;
static struct wake_lock reset_wake_lock_ccore;
static DECLARE_COMPLETION(reset_suspend_completed);
static struct notifier_block reset_pm_notifier;
static struct notifier_block modem_reboot_cpu_notifier;
static struct notifier_block modem_panic_cpu_notifier;
static struct notifier_block modem_freeze_cpu_notifier;

static void __iomem *gic_base_addr = NULL;
static unsigned long ccpu_wdt_addr = 0;

static unsigned int s_ccore_file_store_ok = FALSE;
static ereset_irq_type s_ccore_reset_irq_type = BSP_RESET_NONE;

static unsigned int g_modem_panic_dummy_cnt = 0;
unsigned int modem_reset_reason = 0; /*0: reboot, 1: panic*/

/*declare delayed work*/
static struct delayed_work init_ccore_reset_irq_work;

/*declare delayed work*/
static struct delayed_work ccore_delay_to_do_sh_work;
void modem_panic_give_semaphone(void);
void modem_reboot_give_semaphone(void);
void modem_freeze_give_semaphone(void);
extern int platform_ddr_protect_init(int flag);
int ccorereset_notify_other_cpu(struct notifier_block *cb, unsigned long code, void *p);
extern unsigned int omTimerGet(void);
static void ccorereset_trigger_othercore_fiq(struct work_struct *work);
#ifndef _DRV_LLT_
void Reset_save_delayed_ccore_log(void);
void Reset_set_ccorefileok_flag(void);
#endif
extern void check_modem_run(void);
void do_modem_panic_work(void);
void reset_clean_ccore_wdt_int(void);
extern void __iomem *reset_sub_mstrl;/*defined in reset_sub_ccore.c,for the virt addr of mdm sctrl*/

#ifdef CONFIG_ARM64
#if defined(MNTN_SAVE_LOG_BY_FILESYS)
extern int mntn_mdm_reset_save_log(const char *preason);
#endif
#endif
#define RESET_WORK_DECLARE(name, type)\
\
static BLOCKING_NOTIFIER_HEAD(name ## _notifier_list);\
\
static struct type name ## _work;\
\
static void name ## _do_work(struct work_struct *work)\
{\
    if (1 == ccore_in_suspend_state){\
        if (!wait_for_completion_timeout(&reset_suspend_completed, HZ*10))\
            goto fail;\
    }\
\
    ccorereset_trigger_othercore_fiq(work);\
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
RESET_WORK_DECLARE(modem_panic, work_struct)
RESET_WORK_DECLARE(modem_reboot, work_struct)
RESET_WORK_DECLARE(modem_freeze, work_struct)
//RESET_WORK_DECLARE(android_freeze, delayed_work)
//RESET_WORK_DECLARE(android_reboot, delayed_work)
//RESET_WORK_DECLARE(mcu_puzzle, work_struct)


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
        ccore_in_suspend_state = 1;
        break;

    case PM_POST_SUSPEND:
    case PM_POST_HIBERNATION:
    case PM_POST_RESTORE:
        complete(&reset_suspend_completed);
        ccore_in_suspend_state = 0;
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
    wake_lock(&reset_wake_lock_ccore);

    BSP_IPC_IntDisable(IPC_ACPU_INT_SRC_CCPU_EXC_REBOOT);

    BSP_IPC_IntDisable(IPC_ACPU_INT_SRC_CCPU_NORMAL_REBOOT);

    BSP_IPC_IntDisable(IPC_ACPU_INI_SRC_MCU_EXC_REBOOT);

    disable_irq_nosync(IRQ_WD3);

    disable_irq_nosync(IRQ_WD1);

    disable_irq_nosync(IRQ_CM3WD0);

    platform_ddr_protect_init(0); /*just for compile by x00195127*/

    return;
}
void finish_reset_sub_ccore(void)
{
    platform_ddr_protect_init(1); /*just for compile by x00195127*/
    BSP_IPC_IntEnable(IPC_ACPU_INT_SRC_CCPU_EXC_REBOOT);

    BSP_IPC_IntEnable(IPC_ACPU_INT_SRC_CCPU_NORMAL_REBOOT);

    BSP_IPC_IntEnable(IPC_ACPU_INI_SRC_MCU_EXC_REBOOT);

    enable_irq(IRQ_CM3WD0);

    enable_irq(IRQ_WD1);
    enable_irq(IRQ_WD3);
    wake_unlock(&reset_wake_lock_ccore);
    return;
}
static int regsister_modem_rst_notifier(void)
{
    modem_reboot_cpu_notifier.notifier_call = ccorereset_notify_other_cpu;
    modem_reboot_cpu_notifier.priority = INT_MAX;/*优先级最高*/
    modem_panic_cpu_notifier.notifier_call = ccorereset_notify_other_cpu;
    modem_panic_cpu_notifier.priority = INT_MAX;
    modem_freeze_cpu_notifier.notifier_call = ccorereset_notify_other_cpu;
    modem_freeze_cpu_notifier.priority = INT_MAX;
    modem_reboot_regsister_notifier(&modem_reboot_cpu_notifier);
    modem_panic_regsister_notifier(&modem_panic_cpu_notifier);
    modem_freeze_regsister_notifier(&modem_freeze_cpu_notifier);

    return 0;
}

static irqreturn_t modem_freeze_irq(int irqno, void *param)
{
    unsigned int    ureg_val = 0;

    print_banner("oops : modem freeze ...");

    if(!ccpu_wdt_addr){
        printk(KERN_ERR "bad ccpu_wdt_addr address\n");
        return IRQ_HANDLED;
    }

    writel(DRV_WATCHDOG_UNLOCK_NUM,(void __iomem *)SOC_WDT_WDG_LOCK_ADDR(ccpu_wdt_addr));

    writel(0x0,(void __iomem *)SOC_WDT_WDG_CONTROL_ADDR(ccpu_wdt_addr));

    writel(0x0,(void __iomem *)SOC_WDT_WDG_LOCK_ADDR(ccpu_wdt_addr));

/*Modify_for_c_reset, l00212112,20130511 ,starts*/
    /*clean c core's wdt int*/
    reset_clean_ccore_wdt_int();
/*Modify_for_c_reset, l00212112,20130511 ,ends*/

    prepare_reset();

	/*Dump TCM 之前Hold BBE16,防止LBBP操作TCM造成Modem Noc*/
	if(NULL != reset_sub_mstrl && 1 == get_domain_access_status(ACCESS_DOMAIN_MODEM_SC))
	{
        ureg_val = readl(SOC_MODEM_SCTRL_MDMSC_BBE16_CTRL0_ADDR(reset_sub_mstrl));
        ureg_val |= (BSP_U32)BIT(SOC_MODEM_SCTRL_MDMSC_BBE16_CTRL0_bbe16_runstall_START);
        writel(ureg_val,(SOC_MODEM_SCTRL_MDMSC_BBE16_CTRL0_ADDR(reset_sub_mstrl)));
    }

    queue_work(reset_wq_ccore, &modem_freeze_work);

    return IRQ_HANDLED;
}

#ifndef _DRV_LLT_

static int fiq_nmi_debug = 0;
static void ccorereset_trigger_othercore_fiq(struct work_struct *work)
{
    unsigned long sctrl_on = (unsigned long)ioremap(SOC_AO_SCTRL_BASE_ADDR, 4096);
    unsigned long sctrl_modem = (unsigned long)ioremap(SOC_MODEM_SCTRL_BASE_ADDR, 4096);
    unsigned int    ureg_val = 0;

    if (1== fiq_nmi_debug)
    {
        return;
    }
    /*trigger c core's fiq*/
    if (&modem_freeze_work == work)
    {
        writel(BIT(SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_31ccpu_sft_fiq_START),
                SOC_AO_SCTRL_SC_INT_EN_SET_ADDR(sctrl_on));
        printk(KERN_ERR"acore trigger modem fiq, val = 0x%x\n", ureg_val);

        ureg_val = BIT(SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_sft_fiq_req_START)
                    | BIT(SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_sft_fiq_req_msk_START);
        writel(ureg_val,SOC_AO_SCTRL_SC_PW_CTRL1_ADDR(sctrl_on));

        /*modem subsys ctrl set*/
        ureg_val = readl(SOC_MODEM_SCTRL_MDMSC_SECONDARY_NEWINT1_EN_ADDR(sctrl_modem));
        ureg_val = ureg_val | BIT(SOC_MODEM_SCTRL_MDMSC_SECONDARY_NEWINT1_EN_sc_secondary_int_en_ccpu_sftfiq_intr_START);
        writel(ureg_val,SOC_MODEM_SCTRL_MDMSC_SECONDARY_NEWINT1_EN_ADDR(sctrl_modem));

        mdelay(100);
    }

    iounmap((void*)sctrl_modem);
    iounmap((void*)sctrl_on);
}
#endif
void modem_panic_give_semaphone(void)
{
#ifdef CONFIG_ARM64
#if defined(MNTN_SAVE_LOG_BY_FILESYS)
    mntn_mdm_reset_save_log("modem panic...\n");
#endif
#endif
    reset_ccore_up_semaphone();
}

void modem_reboot_give_semaphone(void)
{
    reset_ccore_up_semaphone();
}

void modem_reboot_himself(int is_normal)
{
    reset_ccore_up_byhimself(is_normal);
}
void modem_freeze_give_semaphone(void)
{
#ifdef CONFIG_ARM64
#if defined(MNTN_SAVE_LOG_BY_FILESYS)
    mntn_mdm_reset_save_log("modem freeze...\n");
#endif
#endif
    reset_ccore_up_semaphone();
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
void Reset_set_ccorefileok_flag(void)
{
    s_ccore_file_store_ok = TRUE;
}
/*****************************************************************************
 Description : save log file that is delayed to save, e.g, modem panic before file system is ok
  History
  1.Date: 2014/7/8
    Author :
    Modification : Created function
*****************************************************************************/
void Reset_save_delayed_ccorelog(void)
{
    switch (s_ccore_reset_irq_type)
    {
    case BSP_RESET_TYPE_MDM_PANIC:
        s_ccore_reset_irq_type = BSP_RESET_NONE;
        queue_work(reset_wq_ccore, &modem_panic_work);
        break;
    default:
        printk(KERN_ERR"Reset_save_delayed_log,type is %d \n",s_ccore_reset_irq_type);
        break;
    }
}
static void ccore_delay_to_runsh_do_work(struct work_struct *work)
{
    switch (s_ccore_reset_irq_type)
    {
    case BSP_RESET_TYPE_MDM_PANIC:
        s_ccore_reset_irq_type = BSP_RESET_NONE;
        modem_panic_do_work(&modem_panic_work);
        break;
    default:
        printk(KERN_ERR"delay_to_runsh_do_work,type is %d \n",reset_wake_lock_ccore);
        break;
    }
    s_ccore_file_store_ok = TRUE;
}
#endif

static void modem_panic_irq(unsigned int param)
{
    void __iomem *gic_dist_pendclr_addr = NULL;
    unsigned int    ureg_val = 0;

    /*检查SRAM中的modem异常复位标志*/
    if(AXI_MODEM_PANIC_FLAG != *(volatile unsigned int *)HISI_VA_ADDRESS(AXI_MODEM_PANIC_FLAG_ADDR)) {
        g_modem_panic_dummy_cnt++;
        print_banner("oops : modem panic dummy");
        printk(KERN_ERR "total count is %d\n", g_modem_panic_dummy_cnt);
        return;
    }
    else {
        *(volatile unsigned int *)HISI_VA_ADDRESS(AXI_MODEM_PANIC_FLAG_ADDR) = 0;
    }

    print_banner("oops : modem panic ...");

    if(!ccpu_wdt_addr){
        printk(KERN_ERR "bad ccpu_wdt_addr address\n");
        return;
    }

    writel(DRV_WATCHDOG_UNLOCK_NUM,(void __iomem *)SOC_WDT_WDG_LOCK_ADDR(ccpu_wdt_addr));

    writel(0x0,(void __iomem *)SOC_WDT_WDG_CONTROL_ADDR(ccpu_wdt_addr));

    writel(0x0,(void __iomem *)SOC_WDT_WDG_LOCK_ADDR(ccpu_wdt_addr));

    reset_clean_ccore_wdt_int();

    /* clear modem wdt int pending */
    gic_dist_pendclr_addr = gic_base_addr + 0x1000 + 0x280 + 4*(IRQ_WD3/32);
    writel(1 << (IRQ_WD3%32), gic_dist_pendclr_addr);

    prepare_reset();

	/*Dump TCM 之前Hold BBE16,防止LBBP操作TCM造成Modem Noc*/
	if(NULL != reset_sub_mstrl && 1 == get_domain_access_status(ACCESS_DOMAIN_MODEM_SC))
	{
        ureg_val = readl(SOC_MODEM_SCTRL_MDMSC_BBE16_CTRL0_ADDR(reset_sub_mstrl));
        ureg_val |= (BSP_U32)BIT(SOC_MODEM_SCTRL_MDMSC_BBE16_CTRL0_bbe16_runstall_START);
        writel(ureg_val,(SOC_MODEM_SCTRL_MDMSC_BBE16_CTRL0_ADDR(reset_sub_mstrl)));
    }

    if (FALSE == s_ccore_file_store_ok)
    {
        s_ccore_reset_irq_type = BSP_RESET_TYPE_MDM_PANIC;
        printk(KERN_ERR "modem panic but delay to save log\n");

        /*delay 20 second*/
        queue_delayed_work(reset_wq_ccore, &ccore_delay_to_do_sh_work, 20*HZ);

        return;
    }

    modem_reset_reason = 1;
    queue_work(reset_wq_ccore, &modem_panic_work);

    return;
}


static void modem_reboot_irq(unsigned int param)
{
    unsigned int    ureg_val = 0;
    print_banner("info : reboot modem by NAS or other");

    prepare_reset();

    modem_reset_reason = 0;

    /*Dump TCM 之前Hold BBE16,防止LBBP操作TCM造成Modem Noc*/
    if(NULL != reset_sub_mstrl && 1 == get_domain_access_status(ACCESS_DOMAIN_MODEM_SC))
    {
        ureg_val = readl(SOC_MODEM_SCTRL_MDMSC_BBE16_CTRL0_ADDR(reset_sub_mstrl));
        ureg_val |= (BSP_U32)BIT(SOC_MODEM_SCTRL_MDMSC_BBE16_CTRL0_bbe16_runstall_START);
        writel(ureg_val,(SOC_MODEM_SCTRL_MDMSC_BBE16_CTRL0_ADDR(reset_sub_mstrl)));
    }

    queue_work(reset_wq_ccore, &modem_reboot_work);

    return;
}
static void init_ccore_reset_irq_do_work(struct work_struct *work)
{
    int ret;

    ccpu_wdt_addr  = (unsigned long)(ioremap(SOC_MWatchDog0_BASE_ADDR, 4096));

    wake_lock_init(&reset_wake_lock_ccore, WAKE_LOCK_SUSPEND, "reset_wake_lock_ccore");

    reset_pm_notifier.notifier_call = reset_pm_notify;
    reset_pm_notifier.priority = INT_MIN;
    register_pm_notifier(&reset_pm_notifier);
    /*modem单独复位注册notifier*/
    regsister_modem_rst_notifier();
    RESET_WORK_INIT(modem_panic);
    RESET_WORK_INIT(modem_freeze);
    RESET_WORK_INIT(modem_reboot);
    exch_mdm_panic_cb_reg((exch_mdm_panic_reg)do_modem_panic_work);
    /*init delayed work*/
    INIT_DELAYED_WORK(&ccore_delay_to_do_sh_work, ccore_delay_to_runsh_do_work);

    ret = request_irq(IRQ_WD3, modem_freeze_irq, 0, "modem_freeze",  NULL);
    if (0 != ret) {
        printk(KERN_ERR "failed to install modem freezing irq handle (%d)\n", ret);
    }

    ret = BSP_IPC_IntConnect(IPC_ACPU_INT_SRC_CCPU_EXC_REBOOT, modem_panic_irq, 0);
    if (BSP_OK != ret){
        printk(KERN_ERR "failed to connect IPC irq handle (%d)\n", IPC_ACPU_INT_SRC_CCPU_EXC_REBOOT);
    }
    BSP_IPC_IntEnable(IPC_ACPU_INT_SRC_CCPU_EXC_REBOOT);

    ret = BSP_IPC_IntConnect(IPC_ACPU_INT_SRC_CCPU_NORMAL_REBOOT, modem_reboot_irq, 0);
    if (BSP_OK != ret){
        printk(KERN_ERR "failed to connect IPC irq handle (%d)\n", IPC_ACPU_INT_SRC_CCPU_NORMAL_REBOOT);
    }
    BSP_IPC_IntEnable(IPC_ACPU_INT_SRC_CCPU_NORMAL_REBOOT);

    return;
}


int ccore_reset_module_init(void)
{
    BUG_ON(!reset_wq_ccore);

    gic_base_addr = ioremap(REG_BASE_GIC, REG_GIC_IOSIZE);
    BUG_ON(!gic_base_addr);
    exch_mdm_panic_cb_reg((exch_mdm_panic_reg)Reset_set_ccorefileok_flag);
    /*init delayed work*/
    INIT_DELAYED_WORK(&init_ccore_reset_irq_work, init_ccore_reset_irq_do_work);

    /*delay 2 second*/
    queue_delayed_work(reset_wq_ccore, &init_ccore_reset_irq_work, CCORE_RESET_MOUDLE_DELAY_TIME*HZ);

    return 0;
}


static void __exit reset_device_module_exit(void)
{
    BSP_IPC_IntDisonnect(IPC_ACPU_INT_SRC_CCPU_EXC_REBOOT, modem_panic_irq, 0);

    BSP_IPC_IntDisonnect(IPC_ACPU_INT_SRC_CCPU_NORMAL_REBOOT, modem_reboot_irq, 0);

    free_irq(IRQ_WD3, NULL);

    iounmap((void*)ccpu_wdt_addr);

    unregister_pm_notifier(&reset_pm_notifier);

    return;
}



static int __init system_freeze_flag_init(void)
{
    /*unsigned long sctrl_base = (unsigned long)HISI_VA_ADDRESS(SOC_SC_ON_BASE_ADDR);*/
    /*SOC_AO_SCTRL_SC_SYS_STAT0_UNION *stat0 = (SOC_AO_SCTRL_SC_SYS_STAT0_UNION *)SOC_AO_SCTRL_SC_SYS_STAT0_ADDR(sctrl_base);*/

    reset_wq_ccore = create_singlethread_workqueue("reset_wq_ccore");
    BUG_ON(!reset_wq_ccore);

    return 0;
}



static void __exit system_freeze_flag_exit(void)
{
    /*to some code when module exit*/

    return;
}

#ifdef BSP_C_HIFI_RESET_ALONE_TESET_VALID

int simulate_irq_c_freeze(void)
{
    modem_freeze_irq(0,0);
    return 0;
}
int simulate_irq_c_panic(void)
{
    modem_panic_irq(0);
    return 0;
}
/*0: normal, 1: exception */
int simulate_irq_c_reset(int is_normal)
{
    modem_reboot_himself(is_normal);

    return 0;
}

#endif

arch_initcall(system_freeze_flag_init);

//module_init(reset_module_init);

module_exit(system_freeze_flag_exit);

module_exit(reset_device_module_exit);

void ccore_test_fiq(void)
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

int ccore_drv_read_bin(const char *partion_name, unsigned int offset, unsigned int length, char *buffer)
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
            /*mlog_n("partitions[%d]:%s is we want(%s)", i, partitions[i].name, partion_name);*/
            memset(pathname, 0, DRV_RESET_NAME_MAX);
            strncat(pathname, DRV_RESET_BLK_DEV, DRV_RESET_NAME_MAX-1);

            sprintf(blk_number, "%d", i);
            strncat(pathname, blk_number, DRV_RESET_NAME_MAX-1);

            break;
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

/*****************************************************************************
 Description : a func for ap to do modem panic work;
 called by   : exch_mdm_panic_cb_reg
  History
  1.Date: 2014/7/8
    Author :
    Modification : Created function
*****************************************************************************/
void do_modem_panic_work(void)
{
    check_modem_run();
    Reset_set_ccorefileok_flag();
    Reset_save_delayed_ccorelog();
}

/*****************************************************************************
 函 数 名  : reset_clean_ccore_wdt_int
 功能描述  : 收到复位中断后，清理c核狗中断
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_clean_ccore_wdt_int(void)
{
#ifdef _DRV_LLT_
#else
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
    unsigned long sctrl_on  = (unsigned long)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);
    writel(0x10, (void __iomem *)SOC_AO_SCTRL_SC_INT_CLEAR_ADDR(sctrl_on));
    printk(KERN_INFO "%s: reset_clean_ccore_wdt_int\n", __FUNCTION__);
#endif
#endif
}

EXPORT_SYMBOL(ccore_drv_read_bin);



MODULE_DESCRIPTION("Hisilicon device ccore RESET Management module");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuyiping <xuyiping@huawei.com>");

