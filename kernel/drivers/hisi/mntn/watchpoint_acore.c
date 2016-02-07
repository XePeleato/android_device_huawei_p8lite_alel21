/*
 * arch/arm/mach-hi6620/debug_watchpoint.c
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define MERROR_LEVEL    1
#define MWARNING_LEVEL  1
#define MNOTICE_LEVEL   1
#define MINFO_LEVEL     0
#define MDEBUG_LEVEL    0
#define MLOG_TAG        "watchpoint"

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/notifier.h>
#include <linux/wait.h>
#include <linux/init.h>
#include <linux/signal.h>
#include <linux/hardirq.h>
#include <linux/device.h>
#include <linux/cpu.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/kallsyms.h>
#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#endif
#include <asm/system.h>
#include <asm/stacktrace.h>
#include "mntn_log.h"
#include "watchpoint_if.h"
#include "watchpoint_common.h"
#include "watchpoint_acore.h"
#include "watchpoint_test.h"
#include "excDrv.h"
#include <linux/hisi/hi6xxx_mntn.h> /*for BSP_MODU_MNTN*/

static uint32_t debug_vaddr = 0;
/*static struct class *watchpoint_dev_class;*/
static int debug_watchpoint_initialized = 0;
static uint32_t max_debug_watchpoint=0;
static bool wp_suspended = false;
#ifdef CONFIG_DEBUG_FS
static struct dentry *wp_df_dir;        /* debugfs dir*/

static unsigned int tshowi_cpu = 0x01;
static unsigned int tshowr_cpu = 0x01;
static char *tget_mem_addr = NULL;
static unsigned int tget_mem_size = 0;
static unsigned int tacc_addr = 0;
static unsigned int tacc_value = 0;
#endif
static unsigned int watchpoint_num[MAX_CPU]={0};

void dump_stack(void);
static int watchpoint_probe(struct platform_device *dev);

void wp_write_register(unsigned int value, unsigned int reg ,unsigned int cpu);
unsigned int wp_read_register(unsigned int reg , unsigned int cpu);
void watchpoint_shutdown (struct platform_device * pdev);
int watchpoint_suspend(struct platform_device *pdev, pm_message_t state);
int watchpoint_resume(struct platform_device * pdev);
int debug_watchpoint_map(void);
int is_illegal_debug_watchpoint_exception(unsigned int cpu ,unsigned long addr, unsigned int fsr, struct pt_regs *regs);


/*
  *wp_write_register - write register .
  *@ unsigned int value: the register value
  *@ unsigned int reg: the register offset 
  *@ unsigned int cpu: the cpu identify
  * this function is supply to watchpoint_common.c to write a register.
  */
void wp_write_register(unsigned int value, unsigned int reg ,unsigned int cpu)
{
    unsigned int raw_cpu_id;

    raw_cpu_id = raw_smp_processor_id();
    if (raw_cpu_id == cpu) {
        writel(value, ((char*)debug_vaddr+cpu*ACPU_DEBUG_REG_DIST + reg*4));
    } else {
        mlog_i("X cpu=%u / %u, reg=0x%x, value=0x%x", cpu, raw_cpu_id, reg, value);
    }
}

/*
  *wp_read_register - read register .
  *
  *@ unsigned int reg: the register offset 
  *@ unsigned int cpu: the cpu identify
  * this function is supply to watchpoint_common.c to read a register.
  */
unsigned int wp_read_register(unsigned int reg, unsigned int cpu)
{
    unsigned int raw_cpu_id;
    unsigned int reg_value;

    raw_cpu_id = raw_smp_processor_id();
    if (raw_cpu_id == cpu) {
        reg_value = readl(((char*)debug_vaddr+cpu*ACPU_DEBUG_REG_DIST + reg*4));
    } else {
        reg_value = WP_READ_VALUE_ERROR; 
        mlog_i("X cpu=%u / %u, reg=0x%x", cpu, raw_cpu_id, reg);
    }

    return reg_value;
}



static int compare_address(struct stackframe *frame, void *d)
{
    watchpoint_cmp_addr_t *cmpaddr_p = (watchpoint_cmp_addr_t *)d;
    unsigned long size = 0;
    char str[KSYM_NAME_LEN];
    const char *name;

    name = kallsyms_lookup(frame->pc, &size, NULL, NULL, str);
    if (!name) {
        mlog_e("lookup err, size = %lu,  pc = 0x%lx", size, frame->pc);
        return 0;
    }
    mlog_i("addr 0x%lx(0x%lx) vs pc 0x%lx", cmpaddr_p->ref_addr, size, frame->pc);

    if ((frame->pc >= cmpaddr_p->ref_addr) && (frame->pc < cmpaddr_p->ref_addr + size)) {
        cmpaddr_p->is_caller_in_stack = 1;

        return 1;
    }

    return 0;
}

/*
  *watchpoint_check_caller - to check the caller who bring about watchpoint exception.
  *@ *caller : the caller which is set by watchpoint user. this is as a reference.
  *@ addr : the modified virtual address
  *@ fsr  : fault state register value
  *@ *reg : the pointer of register 
  *return value. 1:the reference caller is not in the exception stack.
  *              0:the reference caller is in the exception stack.
  * when watchpoint exception occurs ,we want to check the function which bring about this exception .
  * if the function is equal to our reference caller ,then the exception process will be exit ,kernel still run normally.
  * else kernel will be panic and stack information will be dump out.
  *
  * this function is supply to watchpoint user as a call back function.
  */
static int watchpoint_check_caller(void *addr, void *regs, void *caller)
{
    unsigned int fp;
    struct stackframe frame;
    watchpoint_cmp_addr_t cmpaddr;

    mlog_i("addr=0x%p, regs=0x%p, caller=0x%p", addr, regs, caller);


    fp = ((struct pt_regs*)regs)->ARM_fp;
    if (!fp) {
       mlog_e("frame pointer is NULL, so can't recognise caller");
       return 0;
    }
    frame.fp = ((struct pt_regs*)regs)->ARM_fp;
    frame.lr = ((struct pt_regs*)regs)->ARM_lr;
    frame.pc = ((struct pt_regs*)regs)->ARM_pc;
    frame.sp = ((struct pt_regs*)regs)->ARM_sp;

    cmpaddr.ref_addr = (unsigned long)caller;
    cmpaddr.is_caller_in_stack = 0;

    walk_stackframe(&frame, compare_address, &cmpaddr);
    if (cmpaddr.is_caller_in_stack) {
        mlog_i("caller is in the stack");
        return 1;

    } else {
        mlog_i("caller is not in the stack");
    }

    return 0;

}

/*
  *wp_tst_cb_func - callback for test .
  *
  */
static int wp_tst_cb_func(unsigned int addr, void* regs, void* user_param)
{
    mlog_w("tst cb called");

    return 0;
}

/*
  *wp_tst_cb_v2_func - callback for test .
  *
  */
static int wp_tst_cb_v2_func(unsigned int addr, void* regs, void* user_param)
{
    mlog_w("tst cb called, addr=0x%x, regs=0x%p, usrp=0x%p", addr, regs, user_param);

    dump_stack();

    return 0;
}

/*
  *wp_std_cb_func - standard callback function .
  *
  */
static int wp_std_cb_func(unsigned int addr, void* regs, void* user_param)
{
    int ret;
    int is_in_stack;

    mlog_i("std cb called, addr=0x%x, regs=0x%p, usrp=0x%p", addr, regs, user_param);

    /* user callback function called below */

    is_in_stack = watchpoint_check_caller((void *)addr, regs, user_param);
    if (is_in_stack) {
        /* do nothing */
        mlog_n("expected caller");
        ret = 0;
    } else {
        /* to die */
        mlog_n("Unexpected caller, addr=0x%x, regs=0x%p, usrp=0x%p", addr, regs, user_param);
        ret = 1;
    }

    return ret;
}

/*
  *wp_std_cb_v2_func - standard callback function.
  *
  */
static int wp_std_cb_v2_func(unsigned int addr, void* regs, void* user_param)
{
    int ret;
    int is_in_stack;

    mlog_i("std cb called, addr=0x%x, regs=0x%p, usrp=0x%p", addr, regs, user_param);

    /* user callback function called below */

    is_in_stack = watchpoint_check_caller((void *)addr, regs, user_param);
    if (is_in_stack) {
        /* do nothing */
        mlog_n("expected caller");
        ret = 0;
    } else {
        /* dump stack and run as usual */
        mlog_n("Unexpected caller, addr=0x%x, regs=0x%p, usrp=0x%p", addr, regs, user_param);
        dump_stack();
        ret = 0;
    }

    return ret;
}

 /*
  *debug_watchpoint_cpu_notify - cpu hotplug notify function .
  *
  *@ struct notifier_block *nfb: the notifier block structure
  *@ unsigned long status: the cpu status
  *@ void *cpuid: cpuid
  * this function is response to handle watchpoint when the cpu is hot plug or removed
  */
static int debug_watchpoint_cpu_notify(struct notifier_block *nfb,
                    unsigned long status, void *cpuid)
{
    unsigned int cpu = (unsigned int)cpuid; 

    /* to judge if the watchpoint is exist on this cpu
    * if no watchpoint , then we just return.
    */
    if(!watchpoint_num[cpu]) {
        mlog_d("cpu %d has no watchpoint", cpu);
        return NOTIFY_OK;
    }

    switch(status) {
    case CPU_ONLINE:
    case CPU_ONLINE_FROZEN:
        mlog_n("cpu %d is online", cpu);
        break;

    case CPU_DEAD:
    case CPU_DEAD_FROZEN:
        mlog_n("cpu %d is shutdown", cpu);
        break;
    default:
        /*mlog_n("cpu %d status is 0x%lx", cpu, status);*/
        break;
    }
    
    return NOTIFY_OK;
}

static struct notifier_block watchpoint_cpu_notifier = {
    .notifier_call = debug_watchpoint_cpu_notify,
};


static int cpu_idle_notifier(struct notifier_block *nb, unsigned long val, void *data)
{
    unsigned int cpu = raw_smp_processor_id();

    if(!watchpoint_num[cpu]) {
        /* notified once a TICK */
        return NOTIFY_OK;
    }

    /* during suspend, do not operate */
    if (wp_suspended) {
        return NOTIFY_OK;
    }

    switch (val) {
    case IDLE_START:
        /* ilde时配置自动失效 */

        /*mlog_i("cpu %d i_start", cpu);*/
        break;
    case IDLE_END:
        /*mlog_i("cpu %d i_end", cpu);*/

        /* idle end时配置使生效 */
        direct_enable_monitor_mode(cpu);
        direct_set_all_watchpoints_one_cpu(cpu);
        break;
    default:
        mlog_w("cpu %d is %ld", cpu, val);
        break;
    }

    return 0;	
}

static struct notifier_block cpu_idle_nb = {
    .notifier_call = cpu_idle_notifier,
};


/*
  *set_debug_watchpoint - set a watchpoint on cpu .
  *@ unsigned int cpu: the cpu identify
  *
  *
  * this function is supply to user to set a watchpoint.
  */
static int set_debug_watchpoint(uint32_t cpu, watchpoint_t* wp)
{
    uint32_t index;

    if (cpu > ACPU3 || cpu < ACPU0) {
        mlog_e("INPUT cpu(%d) is illegal, larger than ACPU3!", cpu);
        goto error;
    }
    
    if (!wp) {
        mlog_e("INPUT wp is NULL");
        goto error;
    }
    
    if (wp->addr < PAGE_OFFSET) {
        mlog_e("INPUT addr=0x%x(0x%lx) is illegal", wp->addr, PAGE_OFFSET);
        goto error;
    }
    
    if (wp->addr_mask_bits > 31) {
        mlog_e("INPUT mask_bits(0x%x) is illegal", wp->addr_mask_bits);
        goto error;
    }
    
    if ((wp->point_type > WP_MATCH_RW) || (wp->point_type < WP_MATCH_READ)) {
        mlog_e("INPUT access_type(%d) is illegal", wp->point_type);
        goto error;
    }

    if (!debug_watchpoint_initialized) {
        mlog_e("watchpoint not initalized!\n");
        goto error;
    }
    
    if (raw_smp_processor_id() == cpu) {
        direct_enable_monitor_mode(cpu); /*deal with the conflict with NOc bus monitor*/
    }
    
    index = find_and_clear_one_watchpoint(wp->addr, cpu, WP_WT);
    if(index < max_debug_watchpoint) {
        mlog_d("address(0x%x) has setted, now delete it and set again.", wp->addr);
    }

    index = find_and_init_one_watchpoint(wp, cpu, WP_WT);
    if(index >= max_debug_watchpoint) {
        mlog_e("cpu(%u) index(%d) exceed max(%d)", cpu, index, max_debug_watchpoint);
        goto error;
    }
    else {
        watchpoint_num[cpu]++; /*set flag to mean that there is a watchpoint in the cpu*/
    }

    return 0;
error:
     return -1;
}

/**
 * wp_set_watchpoints_ex - set debug watchpoint on specific cpus
 * @watchpoint_t* wp:[in]the watchpoint structure where the watchpoint information in .
 * @cpus   :[in] which cpu to clear. 0 to 3 is for single cpu, 4(ACPUMAX) is for all.
 *
 * Can return negative error values, returns 0 on success.
 * 对外接口
 */
int wp_set_watchpoints_ex(watchpoint_t* wp, uint32_t cpus)
{
    int ret = 0;
    uint32_t cpu, cpu_start, cpu_end;

    if(!wp) {
        mlog_e("INPUT wp is NULL");
        return -1;
    }

    if (cpus > ACPUMAX) {
        mlog_e("INPUT cpus(%d) is illegal", cpus);
        return -EINVAL;
    }

    if (ACPUMAX == cpus) {
        /* all cpus */
        cpu_start = ACPU0;
        cpu_end   = ACPUMAX;
    } else {
        /* single cpu */
        cpu_start = cpus;
        cpu_end   = cpus + 1;
    }

    for (cpu = cpu_start; cpu < cpu_end; cpu++) {
        if((ret = set_debug_watchpoint(cpu, wp)) != 0) {
            mlog_e("cpu %u, set_debug_watchpoint", cpu);
            return -1;
        }
    }
    return 0;
}
EXPORT_SYMBOL(wp_set_watchpoints_ex);

/**
 * wp_set_watchpoints - set debug watchpoint on the cpu
 * @addr   :[in] addr to be watched.
 * @osize  :[in] size in order, size=2^osize, min is 8, 0/1/2/3--8.
 * @rw_type:[in] watched type, 1-read, 2-write, 3-rw.
 * @cb     :[in] callback, 0-no callback, 1-standard cb, 1x-test cb.
                 1-cb, if caller not euqal to cbp then die
                 2-cb2, if caller not euqal to cbp then dump stack
                 11-test cb, just print a message
                 12-test cb2, just dump stack
 * @cb_param:[in] callback param
 * @cpus   :[in] which cpu to set. 0 to 3 is for single cpu, 4(ACPUMAX) is for all.
 *
 * Can return negative error values, returns 0 on success.
 * 对外接口
 */
int wp_set_watchpoints(uint32_t addr, uint32_t osize, uint32_t rw_type, uint32_t cb, uint32_t cb_param, uint32_t cpus)
{
    int ret = 0;
    uint32_t cpu, cpu_start, cpu_end;
    watchpoint_t wp;

    if (cpus > ACPUMAX) {
        mlog_e("INPUT cpus(%d) is illegal", cpus);
        return -EINVAL;
    }

    /* build watchpoint */
    wp.addr = addr;
    wp.point_type = rw_type;
    wp.addr_mask_bits = osize;
    if (1 == cb) {
        wp.user_param = (void *)cb_param;
        wp.watchpoint_callback = wp_std_cb_func;
    } else if (2 == cb) {
        wp.user_param = (void *)cb_param;
        wp.watchpoint_callback = wp_std_cb_v2_func;

    } else if (11 == cb) {
        wp.user_param = (void *)cb_param;
        wp.watchpoint_callback = wp_tst_cb_func;
    } else if (12 == cb) {
        wp.user_param = (void *)cb_param;
        wp.watchpoint_callback = wp_tst_cb_v2_func;

    } else {
        wp.user_param = NULL;
        wp.watchpoint_callback = NULL;
    }
    
    if (ACPUMAX == cpus) {
        /* all cpus */
        cpu_start = ACPU0;
        cpu_end   = ACPUMAX;
    } else {
        /* single cpu */
        cpu_start = cpus;
        cpu_end   = cpus + 1;
    }

    for (cpu = cpu_start; cpu < cpu_end; cpu++) {
        if((ret = set_debug_watchpoint(cpu, &wp)) != 0) {
            mlog_e("cpu %u, set_debug_watchpoint", cpu);
            return -1;
        }
    }
    return 0;
}
EXPORT_SYMBOL(wp_set_watchpoints);

/*
  *clear_debug_watchpoint - clear a watchpoint on cpu .
  *@ unsigned int cpu: the cpu identify
  *@ uint32_t addr: the addr for watchpoint
  *
  * this function is supply to user to clear a watchpoint.
  */
static int clear_debug_watchpoint(uint32_t cpu, uint32_t addr)
{
    uint32_t index;

    if (cpu > ACPU3 || cpu < ACPU0) {
        mlog_e("INPUT cpu(%d) is illegal, larger than ACPU3!", cpu);
        goto error;
    }
    
    if (addr < PAGE_OFFSET) {
        mlog_e("INPUT addr=0x%x(0x%lx) is illegal", addr, PAGE_OFFSET);
        goto error;
    }

    if (!debug_watchpoint_initialized) {
        mlog_e("watchpoint not initalized!\n");
        goto error;
    }

    index = find_and_clear_one_watchpoint(addr, cpu, WP_WT);
    if (index < max_debug_watchpoint) {
        watchpoint_num[cpu]--;
    } else {
        /* not found, do nothing */
    }

    return 0;

error:
    return -EINVAL;
}

/**
 * wp_clear_watchpoints - clear debug watchpoint which on the cpu
 * @addr:start addr where the watchpoint will be clear.
 * @cpus:which cpu to clear. 0 to 3 is for single cpu, 4(ACPUMAX) is for all.
 *
 * Can return negative error values, returns 0 on success.
 * 对外接口
 */
int wp_clear_watchpoints(uint32_t addr, uint32_t cpus)
{
    int ret = 0;
    uint32_t cpu, cpu_start, cpu_end;

    if (cpus > ACPUMAX) {
        mlog_e("cpus(%d) is illegal", cpus);
        return -EINVAL;
    }

    if (ACPUMAX == cpus) {
        /* all cpus */
        cpu_start = ACPU0;
        cpu_end   = ACPUMAX;
    } else {
        /* single cpu */
        cpu_start = cpus;
        cpu_end   = cpus + 1;
    }

    for (cpu = cpu_start; cpu < cpu_end; cpu++) {
        ret = clear_debug_watchpoint(cpu, addr);
        if (ret != 0) {
            mlog_e("cpu %u, clear watchpoint", cpu);
            return ret;
        }
    }
    return 0;
}
EXPORT_SYMBOL(wp_clear_watchpoints);


/**
 * watchpoint_shutdown - clear debug watchpoint
 * @pdev:platform device pointer.
 *
 */
void watchpoint_shutdown (struct platform_device * pdev)
{
    
    /*disable the watchpoints of all cpu*/
    mlog_n("called");
}


/**
 * watchpoint_suspend - watchpoint suspend function
 * @pdev:platfrom device pointer
 * @state: the state of device
 */
int watchpoint_suspend(struct platform_device *pdev, pm_message_t state)
{

    mlog_n("called");
    
    wp_suspended = true;

    return 0;
}

/**
 * watchpoint_resume - watchpoint resume function
 * @pdev:platfrom device pointer
 *
 */
int watchpoint_resume(struct platform_device * pdev)
{
    
    mlog_n("called");

    wp_suspended = false;

    /* 实际寄存器的写入只在idle end时进行 */

    return 0;
}

/**
 * debug_watchpoint_map - map the address
 * 
 */
int debug_watchpoint_map(void)
{
    debug_vaddr = (uint32_t)ioremap(ACPU_DEBUG_REG_BASE_ADDR + ACPU0_DEBUG_REG_OFFSET, 0x8000);
    if (!debug_vaddr) {
        mlog_e("ioremap error");
        goto error ;
    }
    mlog_n("debug_vaddr=0x%x", debug_vaddr);

    return 0;
error:
    return -EINVAL;
}
/*
  *is_illegal_debug_watchpoint_exception-to call function that is register by watchpoint user 
  *and to judge whether the exception is illegal .
  *@ cpu: the cpu identification 
  *@ addr: the modified virtual address
  *@ fsr : fault state register value
  *@ *reg : the pointer of register 
  *return value . -1:exception is legal , we will dump stack information and make kernel panic.
  *0:exception is illegal , exception process exit and kernel run normally.
  */
int is_illegal_debug_watchpoint_exception(unsigned int cpu ,unsigned long addr, unsigned int fsr, struct pt_regs *regs)
{
    int ret = 0;

    ret = do_watchpoint_callback(addr, cpu, (void*)regs);

    return ret;
}

/**
 * do_debug_exception - watchpoint exception function
 * @addr:start addr where the watchpoint will be set.
 * @fsr:fault state register
 * @regs:the pointer of register
 * Can return negative error values, returns 0 on success.
 */
static int do_debug_exception(unsigned long addr, unsigned int fsr, struct pt_regs *regs)
{
    uint32_t value, moe;
    uint32_t cpu = raw_smp_processor_id();

    mlog_e("Begin cpu_id=%d, exception triggered address is 0x%lx", cpu, addr); 

    if (!is_illegal_debug_watchpoint_exception(cpu, addr, fsr, regs)) {
        mlog_e("End, callback called");
        return 0;
    }

    value = wp_read_register(DBGDSCR, cpu);

    moe = (value >> DBGDSCR_MOE_SHIFT) & 0xf;
    mlog_e("DBGDSCR=0x%x MOE=0x%x", value, moe);

    if (0x02 == moe) {
        mlog_e("Asynchronous watchpoint debug event");
    } else if (0x0a == moe) {
        mlog_e("Synchronous watchpoint debug event");
    } else {  
        mlog_e("Not a watchpoint debug event !");
    }

    value = wp_read_register(DBGWFAR, cpu);
    mlog_e("DBGWFAR=0x%08x addr=0x%lx", value, addr);

    mlog_e("End, to systemError");
    systemError((int)BSP_MODU_MNTN, (int)EXCH_S_WATCHPOINT, 0, 0, 0);
    //do_exit(SIGKILL);
    
    return 0;

}



static struct of_device_id of_watchpoint_match_tbl[] = {
    {
        .compatible = "hisilicon,watchpoint",
    },
    { /* end */ }
};

static struct platform_driver plat_watchpoint_driver= {
    .driver = {
        .owner = THIS_MODULE,
        .name = "watchpoint",
        .of_match_table = of_watchpoint_match_tbl,
    },
    .shutdown = watchpoint_shutdown,
    .probe   = watchpoint_probe,
#ifdef CONFIG_PM
    .suspend = watchpoint_suspend,
    .resume  = watchpoint_resume,
#endif
};


#ifdef CONFIG_DEBUG_FS
static void wp_state_dp(char *buf)
{

    memset(buf, 0, WP_DBG_SIZE_PAGE);

    mlog_i("B");

    snprintf(buf, WP_DBG_SIZE_PAGE, "initialized = %d\n", debug_watchpoint_initialized);
    snprintf(buf, WP_DBG_SIZE_PAGE, "%smax_debug_point = %d\n", buf, max_debug_watchpoint);

    snprintf(buf, WP_DBG_SIZE_PAGE, "%swatchpoint_num[] = %u %u %u %u\n", 
             buf, watchpoint_num[0], watchpoint_num[1], watchpoint_num[2], watchpoint_num[3]);
    snprintf(buf, WP_DBG_SIZE_PAGE, "%swp_suspended = %d\n", buf, wp_suspended);

    snprintf(buf, WP_DBG_SIZE_PAGE, "%s<end>\n", buf);

    mlog_i("E");
}

/*
"stat" : state

cat stat
*/
static int wp_state_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_read = 0;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return simple_read_from_buffer(user_buf, count, ppos, "kn_buf alloc fail\n", 18);
    }

    wp_state_dp(kn_buf);

    byte_read = simple_read_from_buffer(user_buf, count, ppos, kn_buf, strlen(kn_buf));

    kfree(kn_buf);
    kn_buf = NULL;

    mlog_i("E");

    return byte_read;
}

/*
watchpoint config 说明
*/
static int wp_config_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_read = 0;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return simple_read_from_buffer(user_buf, count, ppos, "kn_buf alloc fail\n", 18);
    }

    memset(kn_buf, 0, WP_DBG_SIZE_PAGE);

    /* context */
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "usage:\n");
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%ss addr osize(0-31) rw(1-3) cb(0-9) cbp\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    set addr in the specific cpu\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    addr : such as 0xDD001000\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    osize: size=2^osize, min is 8 Byte(0/1/2/3--8)\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    rw   : 1-r, 2-w, 3-rw\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    cb   : 0-no cb\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s           1-cb, if caller not euqal to cbp then die\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s           2-cb2, if caller not euqal to cbp then dump stack\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s           11-test cb, just print a message\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s           12-test cb2, just dump stack\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    cbp  : callback param, in hex\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    example: s 0xDD001000 0 3 1 0x00\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%sc addr\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    clear addr in all cpus\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    addr : such as 0xDD001000\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    example: c 0x..\n", kn_buf);

    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s<end>\n", kn_buf);

    byte_read = simple_read_from_buffer(user_buf, count, ppos, kn_buf, strlen(kn_buf));

    kfree(kn_buf);
    kn_buf = NULL;

    mlog_i("E");

    return byte_read;
}


/*
"cfg" : set/clear watchpoint

echo "s addr osize rw cb" > cfg
        -- addr : address(0x.) to be watched, 8 Byte aligned
        -- osize : size in order, size=2^osize
                   (0/1/2)3--8 Byte, 4--16, 5--32 .. 31--2^31
        -- rw : 1(read), 2(write), 3(read/write)
        -- cb : 0-no callback
                1-cb, if caller not euqal to cbp then die
                2-cb2, if caller not euqal to cbp then dump stack
                11-test cb, just print a message
                12-test cb2, just dump stack
        -- cb_param : callback param, in hex
echo "c addr" > cfg
        -- addr : address to be cleared, 8 Byte aligned
*/
static int wp_config_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_writen = 0;
    int num = 0;
    unsigned int addr, osize, rw, cb, cb_param;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return -EFAULT;
    }

    byte_writen = simple_write_to_buffer(kn_buf, WP_DBG_SIZE_PAGE, ppos, user_buf, count);

    if ('s' == kn_buf[0]) {
        /* set watchpoint */
        num = sscanf(kn_buf, "s 0x%x %u %u %u 0x%x", &addr, &osize, &rw, &cb, &cb_param);
        if (5 == num) {
            wp_set_watchpoints(addr, osize, rw, cb, cb_param, ACPUMAX);

        } else {
            byte_writen = -EINVAL;
        }

    } else if ('c' == kn_buf[0]) {
        /* clear watchpoint */
        num = sscanf(kn_buf, "c 0x%x", &addr);
        if (1 == num) {
            if (addr != 0) {
                /* clear the specific addr in all cpu */
                wp_clear_watchpoints(addr, ACPUMAX);

            } else {
                byte_writen = -EINVAL;
            }

        } else {
            byte_writen = -EINVAL;
        }

    } else {
        /* 异常 */
        byte_writen = -EINVAL;
    }

    kfree(kn_buf);
    kn_buf = NULL;
    
    mlog_i("E");

    return byte_writen;
}


static void wp_tshow_info(char *buf)
{

    memset(buf, 0, WP_DBG_SIZE_PAGE);

    if (0x01 == (0x01 & tshowi_cpu)) {
        show_watchpoint_info(0, buf);
    }

    if (0x02 == (0x02 & tshowi_cpu)) {
        show_watchpoint_info(1, buf);
    }

    if (0x04 == (0x04 & tshowi_cpu)) {
        show_watchpoint_info(2, buf);
    }

    if (0x08 == (0x08 & tshowi_cpu)) {
        show_watchpoint_info(3, buf);
    }

    snprintf(buf, WP_DBG_SIZE_PAGE, "%susage:\n", buf);
    snprintf(buf, WP_DBG_SIZE_PAGE, "%secho 0/1/2/3/a to show corresponding cpu info\n", buf);
    snprintf(buf, WP_DBG_SIZE_PAGE, "%s<end>\n", buf);

}

/*
"wshowi" : show info

cat tshowi
*/
static int wp_tshowi_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_read = 0;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return simple_read_from_buffer(user_buf, count, ppos, "kn_buf alloc fail\n", 18);
    }

    wp_tshow_info(kn_buf);

    byte_read = simple_read_from_buffer(user_buf, count, ppos, kn_buf, strlen(kn_buf));

    kfree(kn_buf);
    kn_buf = NULL;

    mlog_i("E");

    return byte_read;
}

/*
"tshowi" : show info

echo "cpuid" > tshowi
        -- cpuid : 0/1/2/3 or "a" for all cpus
*/
static int wp_tshowi_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_writen = 0;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return -EFAULT;
    }

    byte_writen = simple_write_to_buffer(kn_buf, WP_DBG_SIZE_PAGE, ppos, user_buf, count);

    tshowi_cpu = 0;
    if ('0' == kn_buf[0]) {
        tshowi_cpu |= 0x01;
    }

    if ('1' == kn_buf[0]) {
        tshowi_cpu |= 0x02;
    }

    if ('2' == kn_buf[0]) {
        tshowi_cpu |= 0x04;
    }

    if ('3' == kn_buf[0]) {
        tshowi_cpu |= 0x08;
    }

    if ('a' == kn_buf[0]) {
        tshowi_cpu = 0x0F;
    }

    kfree(kn_buf);
    kn_buf = NULL;

    mlog_i("E");

    return byte_writen;
}

static void wp_tshow_regs(char *buf)
{
    unsigned int raw_cpu_id;
    unsigned int raw_cpu_idx;
    unsigned int tshowr_cpu_tmp;
    int i;

    mlog_i("B");

    memset(buf, 0, WP_DBG_SIZE_PAGE);

    tshowr_cpu &= 0x0F;

    tshowr_cpu_tmp = tshowr_cpu;
    
    mlog_i("B, show=0x%x", tshowr_cpu_tmp);

    for (i = 0; i < 99; i++) {
        if (0 == tshowr_cpu_tmp) {
            break;
        }

        raw_cpu_id = raw_smp_processor_id();
        raw_cpu_idx = 0x01 << raw_cpu_id;

        if (raw_cpu_idx == (0x01 & tshowr_cpu_tmp)) {
            show_watchpoint_regs(0, buf + WP_DBG_SIZE_ONE_SHOW * 0);
            tshowr_cpu_tmp &= ~raw_cpu_idx;

        } else if (raw_cpu_idx == (0x02 & tshowr_cpu_tmp)) {
            show_watchpoint_regs(1, buf + WP_DBG_SIZE_ONE_SHOW * 1);
            tshowr_cpu_tmp &= ~raw_cpu_idx;

        } else if (raw_cpu_idx == (0x04 & tshowr_cpu_tmp)) {
            show_watchpoint_regs(2, buf + WP_DBG_SIZE_ONE_SHOW * 2);
            tshowr_cpu_tmp &= ~raw_cpu_idx;

        } else if (raw_cpu_idx == (0x08 & tshowr_cpu_tmp)) {
            show_watchpoint_regs(3, buf + WP_DBG_SIZE_ONE_SHOW * 3);
            tshowr_cpu_tmp &= ~raw_cpu_idx;

        }

        mlog_i("i=%d, raw_cpu_id=%d, 0x%x, show=0x%x", i, raw_cpu_id, raw_cpu_idx, tshowr_cpu_tmp);
        /* sleep to switch cpu */
        msleep(10);
    }

    /* 顺序显示 */
    snprintf(buf, WP_DBG_SIZE_PAGE, "%s%s", buf, buf + WP_DBG_SIZE_ONE_SHOW * 1);
    snprintf(buf, WP_DBG_SIZE_PAGE, "%s%s", buf, buf + WP_DBG_SIZE_ONE_SHOW * 2);
    snprintf(buf, WP_DBG_SIZE_PAGE, "%s%s", buf, buf + WP_DBG_SIZE_ONE_SHOW * 3);

    /* 动态刷新，每次可能不同 
    snprintf(buf, WP_DBG_SIZE_PAGE, "%stried %d/99 times\n", buf, i)*/;
    snprintf(buf, WP_DBG_SIZE_PAGE, "%s<end>\n", buf);

    mlog_i("E, show=0x%x, i=%d", tshowr_cpu_tmp, i);

}

/*
"showi" : show info

cat showi
*/
static int wp_tshowr_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_read = 0;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return simple_read_from_buffer(user_buf, count, ppos, "kn_buf alloc fail\n", 18);
    }

    wp_tshow_regs(kn_buf);

    byte_read = simple_read_from_buffer(user_buf, count, ppos, kn_buf, strlen(kn_buf));

    kfree(kn_buf);
    kn_buf = NULL;

    mlog_i("E");

    return byte_read;
}

/*
"showr" : show regs

echo "cpuid" > tshowr
        -- cpuid : 0/1/2/3 or "a" for all cpus
*/
static int wp_tshowr_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_writen = 0;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return -EFAULT;
    }

    byte_writen = simple_write_to_buffer(kn_buf, WP_DBG_SIZE_PAGE, ppos, user_buf, count);

    tshowr_cpu = 0;
    if ('0' == kn_buf[0]) {
        tshowr_cpu |= 0x01;
    }

    if ('1' == kn_buf[0]) {
        tshowr_cpu |= 0x02;
    }

    if ('2' == kn_buf[0]) {
        tshowr_cpu |= 0x04;
    }

    if ('3' == kn_buf[0]) {
        tshowr_cpu |= 0x08;
    }

    if ('a' == kn_buf[0]) {
        tshowr_cpu = 0x0F;
    }

    kfree(kn_buf);
    kn_buf = NULL;

    mlog_i("E");

    return byte_writen;
}

/*
"tgetm" : get memory, for test

cat tgetm
        -- addr: 0x1000 //size: 0x1000 type: k/v/d
*/
static int wp_tgetmem_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_read = 0;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return simple_read_from_buffer(user_buf, count, ppos, "kn_buf alloc fail\n", 18);
    }

    memset(kn_buf, 0, WP_DBG_SIZE_PAGE);

    /* context */
    if (tget_mem_addr != NULL) {
        snprintf(kn_buf, WP_DBG_SIZE_PAGE, "addr got = 0x%p, size=0x%08x\n", tget_mem_addr, tget_mem_size);
        snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s\n", kn_buf);
    }
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%susage:\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%sm osize type\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s      malloc memory\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s      osize  : size in order, (0/1/2)3--8 Byte, 4--16, .. 31--2^31\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s      type   : 0-kmalloc, 1-vmalloc, 2-dma\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s      example: m 0 0\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%sf addr type\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s      free memory\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s      addr  : in hex, such as 0x..\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s      type  : 0-kmalloc, 1-vmalloc, 2-dma\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s      example: f 0x.. 0\n", kn_buf);

    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s<end>\n", kn_buf);

    byte_read = simple_read_from_buffer(user_buf, count, ppos, kn_buf, strlen(kn_buf));

    kfree(kn_buf);
    kn_buf = NULL;

    mlog_i("E");

    return byte_read;
}


/*
"tgetm" : get memory, for test

echo "m osize" > tgetm
        -- malloc a memory
        -- osize : size in order, size=2^osize
                   (0/1/2)3--8 Byte, 4--16, 5--32 .. 31--2^31
        -- type  : 0-kmalloc, 1-vmalloc, 2-dma
*/
static int wp_tgetmem_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_writen = 0;
    int num = 0;
    unsigned int osize, type;
    char * addr;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return -EFAULT;
    }

    byte_writen = simple_write_to_buffer(kn_buf, WP_DBG_SIZE_PAGE, ppos, user_buf, count);

    if ('m' == kn_buf[0]) {
        /* malloc */
        num = sscanf(kn_buf, "m %u %u", &osize, &type);
        if (2 == num) {
            tget_mem_addr = wp_tst_get_mem(type, osize, &tget_mem_size);

        } else {
            byte_writen = -EINVAL;
        }

    } else if ('f' == kn_buf[0]) {
        /* free */
        num = sscanf(kn_buf, "f 0x%p %u", &addr, &type);
        if (2 == num) {
            wp_tst_free_mem(addr, type);
            if (tget_mem_addr == addr) {
                tget_mem_addr = NULL;
                tget_mem_size = 0;
            }

        } else {
            byte_writen = -EINVAL;
        }

    } else {
        /* 异常 */
        byte_writen = -EINVAL;
    }

    kfree(kn_buf);
    kn_buf = NULL;
    
    mlog_i("E");

    return byte_writen;
}

/*
"tacc" : access, read or write, for test
               unit is interger

cat tacc
*/
static int wp_taccess_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_read = 0;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return simple_read_from_buffer(user_buf, count, ppos, "kn_buf alloc fail\n", 18);
    }

    memset(kn_buf, 0, WP_DBG_SIZE_PAGE);

    /* context */
    if (tacc_addr != 0) {
        snprintf(kn_buf, WP_DBG_SIZE_PAGE, "addr(0x%x) = 0x%x\n", tacc_addr, tacc_value);
        snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s\n", kn_buf);
    }
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%susage:\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%sw addr value\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    addr and value should in hex, such as 0x..\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    example: w 0x.. 0x..\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%sr addr\n", kn_buf);
    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s    example: r 0x..\n", kn_buf);

    snprintf(kn_buf, WP_DBG_SIZE_PAGE, "%s<end>\n", kn_buf);

    byte_read = simple_read_from_buffer(user_buf, count, ppos, kn_buf, strlen(kn_buf));

    kfree(kn_buf);
    kn_buf = NULL;

    mlog_i("E");

    return byte_read;
}


/*
"tacc" : access, read or write, for test
               unit is interger
echo "r addr" > tacc
echo "w addr value" > tacc
*/
static int wp_taccess_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_writen = 0;
    int num = 0;
    unsigned int addr, value;

    mlog_i("B");

    kn_buf = kmalloc(WP_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return -EFAULT;
    }

    byte_writen = simple_write_to_buffer(kn_buf, WP_DBG_SIZE_PAGE, ppos, user_buf, count);

    if ('w' == kn_buf[0]) {
        /* write */
        num = sscanf(kn_buf, "w 0x%x 0x%x", &addr, &value);
        if (2 == num) {
            tacc_addr = addr;
            wp_tst_write_data(addr, value);

        } else {
            byte_writen = -EINVAL;
        }

    } else if ('r' == kn_buf[0]) {
        /* read */
        num = sscanf(kn_buf, "r 0x%x", &addr);
        if (1 == num) {
            tacc_addr = addr;
            tacc_value = wp_tst_read_data(addr);

        } else {
            byte_writen = -EINVAL;
        }


    } else {
        /* 异常 */
        byte_writen = -EINVAL;
    }

    kfree(kn_buf);
    kn_buf = NULL;
    
    mlog_i("E");

    return byte_writen;
}


static const struct file_operations wp_state_fops = {
    .read  = wp_state_read,
};

static const struct file_operations wp_config_fops = {
    .read  = wp_config_read,
    .write = wp_config_write,
};

/*
"tshowi" : show info, for test

cat tshowi
*/
static const struct file_operations wp_tshowi_fops = {
    .read  = wp_tshowi_read,
    .write = wp_tshowi_write,
};

/*
"tshowr" : show register, for test

cat tshowr
*/
static const struct file_operations wp_tshowr_fops = {
    .read  = wp_tshowr_read,
    .write = wp_tshowr_write,
};

static const struct file_operations wp_tgetm_fops = {
    .read  = wp_tgetmem_read,
    .write = wp_tgetmem_write,
};

static const struct file_operations wp_taccess_fops = {
    .read  = wp_taccess_read,
    .write = wp_taccess_write,
};


#endif /* CONFIG_DEBUG_FS */


/**
 * watchpoint_probe - watchpoint device initalize
 * @dev:	platform device pointer.
 *
 */
static int watchpoint_probe(struct platform_device *pdev)
{
    int cpu;

    debug_watchpoint_map();

    cpu = raw_smp_processor_id();
    max_debug_watchpoint = get_max_watchpoint_number(cpu);
    if (!max_debug_watchpoint) {
        mlog_e("get max watchpoint");
        goto error ;
    }
    mlog_i("max_debug_watchpoint = %d", max_debug_watchpoint);

    /*disable all the watchpoint*/
    for (cpu = ACPU0; cpu <= ACPU3; cpu++) {
        clear_all_watchpoint_one_cpu(cpu, WP_WT);      
    }

    /* Register notifier */
    register_hotcpu_notifier(&watchpoint_cpu_notifier);
    idle_notifier_register(&cpu_idle_nb);

    /* Register debug fault handler. */
#ifdef CONFIG_ARM64
#else
    hook_fault_code(DEBUG_EXCEPTION_TYPE, do_debug_exception, SIGTRAP, TRAP_HWBKPT, "debug data exception");
#endif
    mlog_d("after hook_fault_code\n");

#ifdef CONFIG_DEBUG_FS
    wp_df_dir = debugfs_create_dir("watchpt", NULL);
    if (!wp_df_dir) {
        mlog_e("create wp debugfs dir");
        return -ENOMEM;
    }

    /* status */
    if (!debugfs_create_file("stat", 0640, wp_df_dir, NULL, &wp_state_fops)) {
        mlog_e("create wp debugfs stat\n");
        return -ENOMEM;
    }

    /* set */
    if (!debugfs_create_file("cfg", 0640, wp_df_dir, NULL, &wp_config_fops)) {
        mlog_e("create wp debugfs cfg\n");
        return -ENOMEM;
    }

    /* show info */
    if (!debugfs_create_file("showi", 0640, wp_df_dir, NULL, &wp_tshowi_fops)) {
        mlog_e("create wp debugfs tshowi\n");
        return -ENOMEM;
    }

    /* show regs */
    if (!debugfs_create_file("showr", 0640, wp_df_dir, NULL, &wp_tshowr_fops)) {
        mlog_e("create wp debugfs tshowr\n");
        return -ENOMEM;
    }

    /* show regs */
    if (!debugfs_create_file("tgetm", 0640, wp_df_dir, NULL, &wp_tgetm_fops)) {
        mlog_e("create wp debugfs tgetm\n");
        return -ENOMEM;
    }

    /* show regs */
    if (!debugfs_create_file("tacc", 0640, wp_df_dir, NULL, &wp_taccess_fops)) {
        mlog_e("create wp debugfs tacc\n");
        return -ENOMEM;
    }

#endif /* CONFIG_DEBUG_FS */


    debug_watchpoint_initialized = 1;

    return 0;

error:
    debug_watchpoint_initialized = 0;
    return -1;
}

/**
 * watchpoint_init - initialize watchpoint device
 *
 */
static int __init watchpoint_init(void)
{
	return platform_driver_register(&plat_watchpoint_driver);
}

module_init(watchpoint_init);

