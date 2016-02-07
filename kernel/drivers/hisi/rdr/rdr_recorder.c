/*
 * record the data to rdr. (RDR: kernel run data recorder.)
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cpumask.h>
#include <linux/timer.h>
#include <asm/thread_info.h>
#include <linux/kthread.h>
#include <linux/syslog.h>
#include <linux/errno.h>
#include <linux/delay.h>
/*lint -e451*/
#include <asm/current.h>
/*lint +e451*/
#include <linux/string.h>
#include <asm/traps.h>
#include <linux/time.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/atomic.h>
#include <linux/delay.h>
#include <linux/notifier.h>
#include <linux/hisi/hisi_rproc.h>
#include <linux/semaphore.h>
#include <linux/reboot.h>
#include <linux/slab.h>
#include <huawei_platform/log/log_exception.h>
#include <linux/timer.h>
#include <linux/rtc.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/mmc/core.h>
#include <linux/wakelock.h>
#include <linux/mtd/hisi_nve_interface.h>
#include <huawei_platform/dsm/dsm_pub.h>

#include <linux/hisi/hi3xxx/global_ddr_map.h>

extern int hisi_nve_direct_access_for_rdr(
	struct hisi_nve_info_user *user_info);

#ifdef CONFIG_HISI_BALONG_MODEM
#ifdef CONFIG_IPC_DRIVER    /* Configuration macro for ccore ipc module */
#include "bsp_ipc.h"
#endif
#include "bsp_reset.h"
#endif
#include "product_config.h"
#include <linux/preempt.h>
#include <linux/io.h>

#include <linux/huawei/rdr.h>
#include <linux/huawei/rdr_private.h>
#include <linux/suspend.h>
#include <linux/vmalloc.h>
#include "rdr_internal.h"

#include <linux/hisi/hi6402_hifi_misc.h>

#ifdef _HIFI_WD_DEBUG
#define HIFI_WD_DEBUG
#endif

#ifdef HIFI_WD_DEBUG
#include <drv_reset.h>
#endif

atomic_t volatile rdr_inited = ATOMIC_INIT(0);
struct rdr_struct_s *pbb;
struct rdr_struct_s *rdr_tmp_pbb;
struct dump_nv_s g_dump_cfg;

static struct dump_base_info_s *g_rdr_base;
static struct rdr_global_internal_s *g_rdr_global;
static struct semaphore rdr_sem;
struct rdr_system_error_param_s {
	struct list_head sys_err_list;
	u32 modid;
	u32 arg1;
	u32 arg2;
	char *data;
	u32 len;
};

u32 *g_rdr_dump_stack;
int g_rdr_dump_stack_len;
#define RDR_DUMP_STACK_DEEP (16 * 20)
static u32 rdr_dump_stack[RDR_DUMP_STACK_DEEP];

#define PID_PPID_GET(task_tcb) \
	((((struct task_struct *)task_tcb)->pid & 0xffff) | \
	((unsigned int)(((struct task_struct *)task_tcb)->real_parent->pid \
	& 0xffff) << 16))
/*lint -e750 */
#define RDR_TIMEOF_DAY 0
#define RDR_PRINTK_TIME 1


#define SCTRL_BASE	0xFFF0A000
#define SCBBPDRXSTAT1	0x534
#define SCBBPDRXSTAT2	0x538
#define SCLPM3CTRL      0x510
/*lint +e750 */

#define ASP_CFG_BASE    0xE804E000

#define CRG_PERI_BASE   0xFFF35000 /*need fixup*/
/*lint -e750 */
#define CRGPERIPHIOM3CTRL 0x1F0   /*IOM3 NMI,bit4*/
#define CRGPERIPHPERCTRL3 0x12C   /*CP NMI,bit12*/
/*lint +e750 */

static void __iomem *rdr_sctrl_base;
static void __iomem *rdr_aspcfg_base;
static void __iomem *rdr_crgperi_base;

#ifdef CONFIG_HISI_REBOOT_TYPE
extern void set_watchdog_resetflag(void);
#endif

#define NUM_STATION 4
#define NUM_LONG 5
#define CHANGE_BASE  10

struct linux_dirent {
          unsigned long   d_ino;
          unsigned long   d_off;
          unsigned short  d_reclen;
          char            d_name[1];
  };

#ifdef HIFI_WD_DEBUG
#define CFG_DSP_NMI 0x3C               /*DSP NMI ,bit0-bit15*/
#define HIFI_WTD_FLAG_BASE		(0x37C75400)
#define HIFI_WTD_FLAG_NMI		(HIFI_WTD_FLAG_BASE + 0x8)
#define UCOM_HIFI_WTD_NMI_COMPLETE             (0xB5B5B5B5)

#define DRV_DSP_POWER_STATUS_ADDR		(0x37c02020)
#define DRV_DSP_POWER_ON				(0x55AA55AA)
#define DRV_DSP_POWER_OFF				(0x55FF55FF)

extern int hifireset_runcbfun (DRV_RESET_CALLCBFUN_MOMENT eparam);


static int g_cpu_state = 1;
static DEFINE_SPINLOCK(rdr_v_hifi_mb_lock);
void rdr_system_error_process(struct rdr_system_error_param_s *p);
void rdr_reboot_from_error(int mod_id);

void hisi_rdr_nmi_notify_hifi(void)
{
	unsigned int value;

	value = readl(rdr_aspcfg_base + CFG_DSP_NMI);
	value &= ~(0x1 << 0);
	writel(value, (rdr_aspcfg_base + CFG_DSP_NMI));

	value |= (0x1 << 0);
	writel(value, (rdr_aspcfg_base + CFG_DSP_NMI));
	pr_info("%s\n", __func__);
	return;
}
void reset_set_cpu_status(unsigned int ioff, unsigned int iflag)
{
	spin_lock(&rdr_v_hifi_mb_lock);
	g_cpu_state = ioff;
	spin_unlock(&rdr_v_hifi_mb_lock);

	pr_info("%s: reset_set_cpu_status, ioff = %d, iflag = %d\n",
			__func__, ioff, iflag);
}

int BSP_CPU_StateGet(int cpuid)
{
	int ret = 0;

	spin_lock(&rdr_v_hifi_mb_lock);
	ret = g_cpu_state;
	spin_unlock(&rdr_v_hifi_mb_lock);

	return ret;
}
#endif

static int rdr_env_init(void)
{
	rdr_sctrl_base = ioremap(SCTRL_BASE, SZ_8K);
	if (!rdr_sctrl_base) {
		pr_err("%s():ioremap error\n", __func__);
		return -1;
	}

	rdr_aspcfg_base = ioremap(ASP_CFG_BASE, SZ_4K);
	if (!rdr_aspcfg_base) {
		pr_err("%s():ioremap error\n", __func__);
		return -1;
	}

	rdr_crgperi_base = ioremap(CRG_PERI_BASE, SZ_4K);
	if (!rdr_crgperi_base) {
		pr_err("%s():ioremap error\n", __func__);
		return -1;
	}
	return 0;
}

#if defined(CONFIG_HISI_TIME)
static void rdr_read_tick(u32 *high, u32 *low)
{
	u32 value[4] = {0};

	value[0] = readl(rdr_sctrl_base + SCBBPDRXSTAT1);
	value[1] = readl(rdr_sctrl_base + SCBBPDRXSTAT2);
	value[2] = readl(rdr_sctrl_base + SCBBPDRXSTAT1);
	value[3] = readl(rdr_sctrl_base + SCBBPDRXSTAT2);

	*high = (value[2] < value[0]) ? value[3] - 1 : value[1];
	*low = value[0];
}
#endif

static char *rdr_parse_tick(u32 high, u32 low, char *buf, size_t buf_len)
{
	u64 time;
	long unsigned int sec;
	long unsigned int msec;

	time = ((u64)high) << 32 | low;
	msec = do_div(time, 32000);
	msec = msec * 5 / 4 * 25;
	sec = (long unsigned int)time;
	sec %= 100000;


	memset(buf, 0, buf_len);
	snprintf(buf, buf_len, "%05lu.%06lu", sec, msec);
	return buf;
}

u32 rdr_gettick(u32 *usec)
{
#ifdef CONFIG_HISI_TIME
	u32 high;
	rdr_read_tick(&high, usec);
	return high;
#elif RDR_PRINTK_TIME
	u64 time = local_clock();
	*usec = time & 0xffffffff;
	return (time >> 32) & 0xffffffff;
#elif RDR_TIMEOF_DAY
	struct timeval tv;
	u32 sec;
	do_gettimeofday(&tv);
	sec = (u32)(tv.tv_sec);
	*msec = (u32)(tv.tv_usec);
	return sec;
#else
	/* use only one int value to save time: */
	struct timespec uptime;
	do_posix_clock_monotonic_gettime(&uptime);
	monotonic_to_bootbased(&uptime);
	return (((unsigned long)uptime.tv_sec) << 10) |
			((uptime.tv_nsec / (NSEC_PER_SEC / 1000)) & 0x3ff);
#endif
}

void queue_time_in(u32 *qbuf)
{
	u32 usec;
	qbuf[0] = rdr_gettick(&usec);
	qbuf[1] = usec;
}

#ifdef CONFIG_HISI_NVE
static struct rdr_nv_s rdr_nv;
int rdr_nv_init(void)
{
	struct hisi_nve_info_user nv;
	int ret, cnt = 0;

	pr_info("rdr:rdr nv init\n");
	atomic_set(&rdr_nv.ready, 0);
    atomic_set(&rdr_nv.update,0);
	memset(&nv, 0, sizeof(nv));
	/* write value to nv if necessary. */

	/* read nv to struct */
	nv.nv_operation = NV_READ;
	nv.nv_number = RDR_NV_NUM;
	nv.valid_size = 33; /* byte */
	strncpy(nv.nv_name, RDR_NV_NAME, sizeof(nv.nv_name));

	while (1) {
		ret = hisi_nve_direct_access_for_rdr(&nv);
		if (ret == -ENXIO)
			return ret;
		else if (ret) {
			cnt++;
			if ((cnt > 60) && ((cnt % 4) == 0)) {
				pr_info("rdr:%s():wait %ds, %s read err:%d\n",
					__func__, cnt, nv.nv_name, ret);
			}
			if (cnt > (60 * 10)) {
				pr_err("rdr:%s():%ds,return0\n", __func__, cnt);
				return 0;
			}
			msleep(1000);
		} else
			break;
	}
	/*rdr_nv.rdr_nv_cfg.uint_value = *(u32 *)nv.nv_data;*/
	rdr_nv.rdr_nv_cfg.bits.switch_def  = nv.nv_data[0];
	rdr_nv.rdr_nv_cfg.bits.rdr         = nv.nv_data[1];
	rdr_nv.rdr_nv_cfg.bits.rdr_loop    = nv.nv_data[2];
	rdr_nv.rdr_nv_cfg.bits.modem_sram  = nv.nv_data[3];
	rdr_nv.rdr_nv_cfg.bits.modem_mem   = nv.nv_data[4];
	rdr_nv.rdr_nv_cfg.bits.modem_share = nv.nv_data[5];
	rdr_nv.rdr_nv_cfg.bits.hifi        = nv.nv_data[6];
	rdr_nv.rdr_nv_cfg.bits.hifi_ocram  = nv.nv_data[7];
	rdr_nv.rdr_nv_cfg.bits.hifi_tcm    = nv.nv_data[8];
	rdr_nv.rdr_nv_cfg.bits.resetlog    = nv.nv_data[9];
	rdr_nv.rdr_nv_cfg.bits.lpm3_tcm    = nv.nv_data[10];
	rdr_nv.rdr_nv_cfg.bits.socp        = nv.nv_data[11];
	atomic_set(&rdr_nv.ready, 1);
	/* followed is for debug: */
	rdr_debug("switch_def  :%x", rdr_nv.rdr_nv_cfg.bits.switch_def);
	rdr_debug("rdr         :%x", rdr_nv.rdr_nv_cfg.bits.rdr);
	rdr_debug("rdr_loop    :%x", rdr_nv.rdr_nv_cfg.bits.rdr_loop);
	rdr_debug("modem_sram  :%x", rdr_nv.rdr_nv_cfg.bits.modem_sram);
	rdr_debug("modem_mem   :%x", rdr_nv.rdr_nv_cfg.bits.modem_mem);
	rdr_debug("modem_share :%x", rdr_nv.rdr_nv_cfg.bits.modem_share);
	rdr_debug("hifi        :%x", rdr_nv.rdr_nv_cfg.bits.hifi);
	rdr_debug("hifi_ocram  :%x", rdr_nv.rdr_nv_cfg.bits.hifi_ocram);
	rdr_debug("hifi_tcm    :%x", rdr_nv.rdr_nv_cfg.bits.hifi_tcm);
	rdr_debug("resetlog    :%x", rdr_nv.rdr_nv_cfg.bits.resetlog);
	rdr_debug("lpm3_tcm    :%x", rdr_nv.rdr_nv_cfg.bits.lpm3_tcm);
	rdr_debug("socp        :%x", rdr_nv.rdr_nv_cfg.bits.socp);
	rdr_debug("reserved    :%x", rdr_nv.rdr_nv_cfg.bits.reserved);

	return 0;
}

void rdr_nv_update(void)
{
    if (atomic_read(&rdr_nv.update) == 1) {
        pr_info("rdr:%s():rdr_nv has updated.\n", __func__);
        return;
    }
    if (atomic_read(&rdr_nv.ready) != 1) {
		pr_info("rdr:%s():GetNV Error.\n", __func__);
		return;
	}
    rdr_nv_init();
    atomic_set(&rdr_nv.update, 1);
    return;
}

u32 rdr_nv_get_value(enum rdr_nv_e nv_option)
{
	u32 ret;

	if (atomic_read(&rdr_nv.ready) != 1) {
		pr_info("rdr:%s():cantGetNV,Onlysave8M\n", __func__);
		return (nv_option == RDR_NV_RDR) ? (u32)1 : (u32)0;
	}
	switch (nv_option) {
	case RDR_NV_RDR:
		ret = rdr_nv.rdr_nv_cfg.bits.rdr;
		pr_info("rdr:%s():rdr 8m:%s\n", __func__, ret ? "yes" : "no");
		break;
	case RDR_NV_MODEM_SRAM:
		ret = rdr_nv.rdr_nv_cfg.bits.modem_sram;
		pr_info("rdr:%s():modem96K:%s\n", __func__, ret ? "yes" : "no");
		break;
	case RDR_NV_MODEM_MEM:
		ret = rdr_nv.rdr_nv_cfg.bits.modem_mem;
		pr_info("rdr:%s():modem:%s\n", __func__, ret ? "yes" : "no");
		break;
	case RDR_NV_MODEM_SHARE:
		ret = rdr_nv.rdr_nv_cfg.bits.modem_share;
		pr_info("rdr:%s:modemshare:%s\n", __func__, ret ? "yes" : "no");
		break;
	case RDR_NV_HIFI:
		ret = rdr_nv.rdr_nv_cfg.bits.hifi;
		pr_info("rdr:%s():hifi:%s\n", __func__, ret ? "yes" : "no");
		break;
	case RDR_NV_HIFI_OCRAM:
		ret = rdr_nv.rdr_nv_cfg.bits.hifi_ocram;
		pr_info("rdr:%s:hifi ocram:%s\n", __func__, ret ? "yes" : "no");
		break;
	case RDR_NV_HIFI_TCM:
		ret = rdr_nv.rdr_nv_cfg.bits.hifi_tcm;
		pr_info("rdr:%s:hifi tcm:%s\n", __func__, ret ? "yes" : "no");
		break;
	case RDR_NV_RESETLOG:
		ret = rdr_nv.rdr_nv_cfg.bits.resetlog;
		pr_info("rdr:%s():resetlog:%s\n", __func__, ret ? "yes" : "no");
		break;
	case RDR_NV_LPM3_TCM:
		ret = rdr_nv.rdr_nv_cfg.bits.lpm3_tcm;
		pr_info("rdr:%s():lpm3 tcm:%s\n", __func__, ret ? "yes" : "no");
		break;
	case RDR_NV_SOCP:
		ret = rdr_nv.rdr_nv_cfg.bits.socp;
		pr_info("rdr:%s():socp:%s\n", __func__, ret ? "yes" : "no");
		break;
	default:
		pr_info("rdr:%s():arg invalid, dont save\n", __func__);
		ret = 0;
		break;
	}
	return ret;
}
#else
int rdr_nv_init(void)
{
	pr_info("rdr:%s():hisi nv isnt config,dont init.\n", __func__);
	return 0;
}
u32 rdr_nv_get_value(enum rdr_nv_e nv_option)
{
	pr_info("rdr:%s():hisi nv isnt config,default not save.\n", __func__);
	return (nv_option == RDR_NV_RDR) ? 1 : 0;
}

void rdr_nv_update(void)
{
	pr_info("rdr:%s():hisi nv isnt config,dont update.\n", __func__);
	return;
}
#endif

#define DUMP_PRINT_SIZE (0x4000)
#define DUMP_USER_DATA_SIZE (0x1000)

struct field_rec_s {
/* global info: */
	u32 field_rec_start;
	u32 reboot_time;
/* app core */
	u32 print_area;
	u32 user_data;
	u32 task_switch_queue;
	u32 int_queue;
#if 0  //delete by xiehongliang for seattle.
	u32 int_list_queue;
#endif
	u32 task_stack;
	u32 exc_task_stack;
	u32 alltask_queue; /* record task command name */
	u32 task_info;
	u32 syscall_queue;
	u32 interrupts_ls;
	u32 buddy_fail;
#ifdef CONFIG_CPU_FREQ
	u32 cpu_on_off;
#endif
#ifdef CONFIG_DETECT_HUNG_TASK
	u32 hung_task;
#endif
	u32 suspend_resume_queue; /* suspend and resume */
	u32 warn_on;
	u32 base_info;
	u32 field_rec_end;

};
static struct field_rec_s fr;
#define RDR_FIELD_NOT_USED 0xffffffff

int rdr_record_register(void)
{
	u32 *field_idx;
	int j = 0;

	memset(&fr, RDR_FIELD_NOT_USED, sizeof(fr));
	RDR_ASSERT(rdr_afreg(fr.reboot_time, RDR_FID_REBOOT_TIME, RDR_U32, 8));
	RDR_ASSERT(rdr_afreg(fr.print_area,
			RDR_FID_PRINTK, RDR_STR, DUMP_PRINT_SIZE));
	RDR_ASSERT(rdr_afreg(fr.user_data,
			RDR_FID_USR_DATA, RDR_STR, DUMP_USER_DATA_SIZE));
	RDR_ASSERT(rdr_afreg(fr.task_switch_queue,
			RDR_FID_TASK_SWITCH, RDR_Q, 1024 * 4));
	RDR_ASSERT(rdr_afreg(fr.int_queue,
			RDR_FID_INT_SWITCH, RDR_Q, 1024 * 4));
#if 0  //delete by xiehongliang for seattle.
	RDR_ASSERT(rdr_afreg(fr.int_list_queue,
			RDR_FID_INT_LIST, RDR_Q, 1024 * 4));
#endif
	RDR_ASSERT(rdr_afreg(fr.task_stack,
			RDR_FID_ALLTASK_STACK, RDR_Q, 1024 * 1024));
	RDR_ASSERT(rdr_afreg(fr.exc_task_stack,
			RDR_FID_EXCTASK_STACK, RDR_Q, 4 * 1024));
	RDR_ASSERT(rdr_afreg(fr.alltask_queue,
			RDR_FID_TASKNAME_TAB, RDR_Q, 1024 * 4));
	RDR_ASSERT(rdr_afreg(fr.task_info,
			RDR_FID_ALLTASK_DUMP, RDR_STR, 1024 * 128));
	RDR_ASSERT(rdr_afreg(fr.syscall_queue,
			RDR_FID_SYSCALL, RDR_Q, 1024 * 4));
	RDR_ASSERT(rdr_afreg(fr.interrupts_ls,
			RDR_FID_PROC_INT_TAB, RDR_STR, 1024 * 4));
	RDR_ASSERT(rdr_afreg(fr.buddy_fail,
			RDR_FID_BUDDY_FAIL, RDR_Q, 1024 * 8));

#ifdef CONFIG_CPU_FREQ
	RDR_ASSERT(rdr_afreg(fr.cpu_on_off,
			RDR_FID_CPU_ONOFF, RDR_Q, 1024 * 1));
#endif
#ifdef CONFIG_DETECT_HUNG_TASK
	RDR_ASSERT(rdr_afreg(fr.hung_task, RDR_FID_HUNG_TASK, RDR_Q, 1024 * 8));
#endif
	RDR_ASSERT(rdr_afreg(fr.suspend_resume_queue,
			RDR_FID_ACPI, RDR_Q, 1024 * 1));
	RDR_ASSERT(rdr_afreg(fr.warn_on, RDR_FID_WARN, RDR_Q, 1024 * 4));
	RDR_ASSERT(rdr_afreg(fr.base_info,
		RDR_FID_BASE_INFO, RDR_U32, sizeof(struct dump_base_info_s)));

	for (field_idx = &fr.field_rec_start + 1;
			field_idx < &fr.field_rec_end; field_idx++, j++) {
		if (*field_idx == RDR_FIELD_NOT_USED) {
			pr_err("rdr:the %dth is not reg\n", j);
			return -1;
		}
	}

	return 0;
}

void dump_save_printk(void)
{
	int print_size = rdr_field_size(fr.print_area);

	/* save printk data: */
	if (print_size > *bsp_dump_log_buf_len)
		print_size = *bsp_dump_log_buf_len;

	(void)rdr_syslog_print_all(field_addr(char, fr.print_area), print_size);

	return;
}

static volatile int rdr_not_dump_stack;

int rdr_get_not_dump_stack(void)
{
	return rdr_not_dump_stack;
}

static void rdr_set_not_dump_stack(int value)
{
	rdr_not_dump_stack = value;
}

atomic_t rdr_crit_dump_stack = ATOMIC_INIT(0);
DEFINE_SPINLOCK(g_rdr_dump_stack_lock);
static u32 rdr_stack_queue_buf[RDR_DUMP_STACK_DEEP + 16];
static void stack_queue(u32 field, u32 *buf, u32 byte, struct task_struct *task)
{
	u32 i;
	struct hisi_ringbuffer_s *q =
			field_addr(struct hisi_ringbuffer_s, field);
	u32 *qbuf = rdr_stack_queue_buf;
	if (in_interrupt())
		return;
	rdr_spin_lock(&g_rdr_dump_stack_lock);
	atomic_set(&rdr_crit_dump_stack, 1);

	if (buf == NULL)
		byte = 0;
	if (byte != 0)
		memcpy((void *)qbuf, (void *)buf, byte);
	g_rdr_dump_stack = (u32 *)rdr_dump_stack;
	g_rdr_dump_stack_len = sizeof(rdr_dump_stack);
	memset(rdr_dump_stack, 0, (size_t)g_rdr_dump_stack_len);
	rdr_set_not_dump_stack(1);
	dump_stack_bl(task);
	rdr_set_not_dump_stack(0);
	g_rdr_dump_stack = NULL;
	g_rdr_dump_stack_len = 0;
	for (i = 0; i < RDR_DUMP_STACK_DEEP; i++)
		qbuf[i + byte / sizeof(u32)] = rdr_dump_stack[i];
	hisi_ringbuffer_write(q, qbuf);
	atomic_set(&rdr_crit_dump_stack, 0);
	rdr_spin_unlock(&g_rdr_dump_stack_lock);
	return;
}

void dump_save_all_task(void)
{
	char tmp_mem[512];
	u32 buf[4];
	u32 tmp_size, total_size = 0;
	struct cpu_context *cpu_con;
	struct task_struct *p_tid;
	char *task_data = field_addr(char, fr.task_info);
	u32 task_data_size = rdr_field_size(fr.task_info);

	snprintf(tmp_mem, 256, "pid\tppid\tstate\tpolicy\tprio\tstackbase\tstackend\tcomm\t"
		"x19\tx20\tx21\tx22\tx23\tx24\tx25\tx26\tx27\tx28\tfp\tsp\tpc\n");
	tmp_size = strlen(tmp_mem);
	strncpy(task_data, tmp_mem, tmp_size);
	task_data += tmp_size;
	total_size += tmp_size;

	rcu_read_lock();
	for_each_process(p_tid) {
		task_lock(p_tid);
		/*pr_info("============pid:%d====\n", p_tid->pid);*/
		cpu_con = &(p_tid->thread.cpu_context);
		snprintf(tmp_mem, 512, "%d\t%d\t%d\t%d\t%d\t0x%x\t0x%x\t%s\t"
			"0x%lx\t0x%lx\t0x%lx\t0x%lx\t0x%lx\t0x%lx\t0x%lx\t0x%lx\t"
			"0x%lx\t0x%lx\t0x%lx\t0x%lx\t0x%lx\n",
			p_tid->pid, p_tid->real_parent->pid, (int)p_tid->state,
			p_tid->policy, p_tid->prio,
			(u32)((char *)p_tid->stack + THREAD_SIZE),
			(u32)end_of_stack(p_tid),
			p_tid->comm,
			cpu_con->x19, cpu_con->x20, cpu_con->x21,
			cpu_con->x22, cpu_con->x23, cpu_con->x24,
			cpu_con->x25, cpu_con->x26, cpu_con->x27,
			cpu_con->x28, cpu_con->fp, cpu_con->sp,
			cpu_con->pc);
		tmp_size = strlen(tmp_mem);
		if (total_size + tmp_size >= task_data_size) {
			*task_data = '\0';
			task_unlock(p_tid);
			rcu_read_unlock();
			return;
		}

		strncpy(task_data, tmp_mem, tmp_size);
		task_data += tmp_size;
		total_size += tmp_size;

		queue_time_in(buf);
		buf[2] = 0x6b736174;/* task tag */
		buf[3] = (u32)p_tid->pid;
		stack_queue(fr.task_stack, buf, sizeof(buf), p_tid);
		task_unlock(p_tid);
	}
	*task_data = '\0';
	rcu_read_unlock();
	return;
}

void suspend_resume_hook(u32 old_state, u32 new_state)
{
	u32 qbuf[5];
	struct hisi_ringbuffer_s *q
		= field_addr(struct hisi_ringbuffer_s, fr.suspend_resume_queue);
	queue_time_in(qbuf);
	qbuf[2] = 0x70737573; /* susp tag */
	qbuf[3] = old_state;
	qbuf[4] = new_state;
	hisi_ringbuffer_write(q, qbuf);

	return;
}

void dump_task_switch_hook(const void *old_tcb, void *new_tcb)
{
#ifdef CONFIG_SMP
	u32 qbuf[5];
#else
	u32 qbuf[3];
#endif
	u32 pid_ppid;
	char comm[TASK_COMM_LEN];
	int magic = ((struct task_struct *)new_tcb)->dump_magic;
	struct hisi_ringbuffer_s *q;

	if (!g_rdr_global->app.task_switch_start)
		return;

	pid_ppid = PID_PPID_GET(new_tcb);

	q = field_addr(struct hisi_ringbuffer_s, fr.task_switch_queue);
	queue_time_in(qbuf);
	qbuf[2] = (u32)pid_ppid;
#ifdef CONFIG_SMP
	/* which cpu */
	qbuf[3] = (u32)((struct task_struct *)new_tcb)->on_cpu;
	/*current online cpu count*/
	qbuf[4] = (u32)(num_online_cpus());
#endif
	hisi_ringbuffer_write(q, qbuf);

	g_rdr_base->current_task = (u64)(((struct task_struct *)new_tcb)->pid);

	/*check whether we have recorded the task name:*/
	if (magic == (int)new_tcb)
		return;
	else if (magic == (int)new_tcb + 1) {
		u32 qbuf2[4];
		/* all tasks are named kthreadd first.*/
		q = field_addr(struct hisi_ringbuffer_s, fr.alltask_queue);

		qbuf2[0] = pid_ppid;
		(void)get_task_comm(comm, (struct task_struct *)(new_tcb));
		qbuf2[1] =  *((u32 *)(comm));
		qbuf2[2] =  *((u32 *)(comm + 4));
		qbuf2[3] =  *((u32 *)(comm + 8));
		hisi_ringbuffer_write(q, qbuf2);

		((struct task_struct *)new_tcb)->dump_magic = (int)new_tcb;
	} else
		((struct task_struct *)new_tcb)->dump_magic = (int)new_tcb + 1;

	return;
}

/*
 * param1:exit:when enter the irq,it is 0; when exit irq,it is 1
 */
void dump_int_switch_hook(u32 exit, u32 oldvec, u32 newvec)
{
	struct hisi_ringbuffer_s *q;
	u32 qbuf[3];

	/* rdr status: */
	if (!g_rdr_global->app.int_switch_start)
		return;

	if (exit)
		g_rdr_global->app.in_interrupt = 0;
	else
		g_rdr_global->app.in_interrupt = 1;

	q = field_addr(struct hisi_ringbuffer_s, fr.int_queue);
	queue_time_in(qbuf);
	qbuf[2] = (((exit ? INT_EXIT_FLAG : INT_IN_FLAG) << 16) | newvec);
	hisi_ringbuffer_write(q, qbuf);

	g_rdr_base->current_int = newvec;

	return;
}
#if 0  //delete by xiehongliang for seattle.
void interrupts_list_hook(u32 is_add, u32 irq, u64 irq_name)
{
	u32 buffer[3];
	u32 qbuf[5];
	struct hisi_ringbuffer_s *q;
	size_t min;

	if (!irq_name)
		return;

	q = field_addr(struct hisi_ringbuffer_s, fr.int_list_queue);
	qbuf[0] = is_add;

	/* copy irq name to buffer: */
	min = strlen((char *)irq_name) + 1;
	if (min > sizeof(buffer))
		min = sizeof(buffer) - 1;
	memset((char *)buffer, 0, sizeof(buffer));
	memcpy((char *)buffer, (char *)irq_name, min);

	qbuf[1] =  irq;
	qbuf[2] =  *((u32 *)(buffer));
	qbuf[3] =  *((u32 *)(buffer + 1));
	qbuf[4] =  *((u32 *)(buffer + 2));

	hisi_ringbuffer_write(q, qbuf);

	return;
}
#endif
static void rdr_save_global_info(u32 mod_id)
{
	u32 reason = (mod_id & HISI_RDR_MOD_EXCE_MASK);
	u32 c = (mod_id & HISI_RDR_MOD_CORE_MASK);

	if (rdr_is_cp_inap_modid(mod_id)) {
		pbb->top_head.except_reason = HISI_RDR_MOD_EXCE_NORMAL;
		pbb->top_head.except_core = RDR_MOD_CORE_AP;
	} else if ((reason == HISI_RDR_MOD_EXCE_WD) || (c == HISI_RDR_MOD_CORE_AP)) {
		if (mod_id != HISI_RDR_MOD_CP_PANIC) {
			pbb->top_head.except_reason = (mod_id & HISI_RDR_MOD_EXCE_MASK);
			pbb->top_head.except_core = (mod_id & RDR_MOD_NCP_MASK);
		}
	} else if (rdr_is_ap_reboot_cp_modid(mod_id)) {
	    pbb->top_head.except_reason = HISI_RDR_MOD_EXCE_NORMAL;
		pbb->top_head.except_core = RDR_MOD_CORE_CP;
	} else
		pr_info("rdr:reason should be filled by CP/M3,id:%x\n", mod_id);

	/* if been reset by top level, set the reset status: */
	if (HISI_RDR_MOD_EXCE_UNDEF == pbb->top_head.except_reason)
		pbb->top_head.except_reason = HISI_RDR_MOD_EXCE_NORMAL;
	/*
	if (HISI_RDR_MOD_EXCE_WD != pbb->top_head.except_reason)
		pbb->top_head.except_core = HISI_RDR_MOD_CORE_AP;
	*/

	g_rdr_global->app.start_flag = DUMP_START_EXCH;

	return;
}

static int rdr_inint(void)
{
	return g_rdr_global->app.in_interrupt;
}

static void rdr_save_base(u32 mod_id, u32 arg1, u32 arg2, char *data, u32 len)
{
	u32 core = (mod_id & HISI_RDR_MOD_CORE_MASK);
	u32 reason = (mod_id & HISI_RDR_MOD_EXCE_MASK);
	if ((HISI_RDR_MOD_CORE_AP == core) || (rdr_is_cp_inap_modid(mod_id))) {
		if (0 == rdr_inint()) { /* in task context */
			if ((mod_id == 0x11000025 || mod_id == 0x1100002A))
				g_rdr_base->reboot_task = arg1;
			else {
				struct task_struct *t;
				t = (struct task_struct *)g_rdr_base->current_task;
				g_rdr_base->reboot_task = (u64)t;
				if (NULL != (void *)t) {
					memcpy(g_rdr_base->task_name, t->comm, 16);
					stack_queue(fr.exc_task_stack, NULL, 0, t);
				}
			}

			g_rdr_base->reboot_int = 0xffffffff;
		} else { /* in interrupt context: */
			size_t taskname_len = sizeof(g_rdr_base->task_name);
			g_rdr_base->reboot_task = 0xffffffff;
			memset(g_rdr_base->task_name, 0, taskname_len);
			g_rdr_base->reboot_int = g_rdr_base->current_int;
		}
		if (reason == HISI_RDR_MOD_EXCE_PANIC)
			g_rdr_base->modid = 0x7;
		else
			g_rdr_base->modid = mod_id;
	} else if(rdr_is_ap_reboot_cp_modid(mod_id)) {
        g_rdr_base->reboot_int = 0xffffffff;
        g_rdr_base->reboot_task = 0xffffffff;
        memset(g_rdr_base->task_name, 0, sizeof(g_rdr_base->task_name));
        if(mod_id == HISI_RDR_MOD_CP_RILD) {
            g_rdr_base->reboot_task = 11;
            memcpy(g_rdr_base->task_name, RDR_STR_TASKNAME_RILD, strlen(RDR_STR_TASKNAME_RILD));
        } else if(mod_id == HISI_RDR_MOD_CP_3RD) {
            g_rdr_base->reboot_task = 12;
            memcpy(g_rdr_base->task_name, RDR_STR_TASKNAME_3RD_CP, strlen(RDR_STR_TASKNAME_3RD_CP));
        }
        g_rdr_base->modid = mod_id;
	}
    else {
		g_rdr_base->reboot_task = 0xffffffff;
		g_rdr_base->reboot_int = 0xffffffff;
		memset(g_rdr_base->task_name, 0, sizeof(g_rdr_base->task_name));
		if (core == HISI_RDR_MOD_CORE_CP)
			g_rdr_base->modid = 0xe;
		else
			g_rdr_base->modid = mod_id;
	}

	g_rdr_base->reboot_context = rdr_inint() ? DUMP_CTX_INT : DUMP_CTX_TASK;
	g_rdr_base->arg1 = arg1;
	g_rdr_base->arg2 = arg2;
	g_rdr_base->arg3 = (u32)data;
	g_rdr_base->arg3_length = len;

	return;
}

static void rdr_trace_stop(void)
{
	int_switch_hook_delete(); /* remove hooks: */
	(void)task_switch_hook_del((rdr_funcptr_2)dump_task_switch_hook);
	exc_hook_delete();
	g_rdr_global->app.task_switch_start = 0;
	g_rdr_global->app.int_switch_start = 0;
	g_rdr_global->app.task_switch_start = 0;
	g_rdr_global->app.int_switch_start = 0;

	return;
}

static void rdr_system_error_enter(void)
{
	rdr_trace_stop(); /* stop task schedule and irq etc... */
	/*TODO: after preempt disable, msleep should not use. */
	/*preempt_disable();*/
	return;
}

/* transfer value from hisi_system_error to here.the value is current task */
static u64 rdr_current_task;

static void dump_save_exc_task(void)
{
	g_rdr_base->current_task = rdr_current_task;
	return;
}

static void dump_save_usr_data(char *data, u32 length)
{
	struct rdr_field_s *f;
	struct rdr_a0_struct_s *a0;
	s32 len;

	if ((NULL == data) || (!length))
		return;

	len = rdr_field_size(fr.user_data);

	len = (len < length) ? len : length;
	a0 = RDR_ACORE_ADDR;
	f = (struct rdr_field_s *)&(a0->oam_reg) + fr.user_data;
	f->size = len;

	memcpy(field_addr(void, fr.user_data), (const void *)data, (size_t)len);

	return;
}

#define NORMAL_POWER_ON     "NORMAL_POWER_ON"

char *rdr_get_ecore(struct rdr_struct_s *bb)
{
	char *core;
	switch (bb->top_head.except_core | RDR_NCP) {
	case HISI_RDR_MOD_CORE_CP:
		core = RDR_STR_CORE_CP;
		break;
	case HISI_RDR_MOD_CORE_AP:
		core = RDR_STR_CORE_AP;
		break;
	case HISI_RDR_MOD_CORE_IOM3:
		core = RDR_STR_CORE_IOM3;
		break;
	case HISI_RDR_MOD_CORE_LPM3:
		core = RDR_STR_CORE_LPM3;
		break;
	case HISI_RDR_MOD_CORE_HIFI:
		core = RDR_STR_CORE_HIFI;
		break;
	case HISI_RDR_MOD_CORE_BBE:
		core = RDR_STR_CORE_BBE;
		break;
	case HISI_RDR_MOD_CORE_TEEOS:
		core = RDR_STR_CORE_TEEOS;
		break;
	default:
		core = RDR_STR_CORE_UNKNOWN;
		break;
	}
	return core;
}

void rdr_record_resetlog(void)
{
	s8 *reason, *core, *resetlog;
	struct rdr_a1_reserve_s *r;
	size_t siz_resetlog = 256;
	struct dump_base_info_s *base_info;

	r = ((struct rdr_a1_reserve_s *)(rdr_core_addr(pbb, RDR_AREA_RESERVE)));
	resetlog = r->ap_cp_share.content.resetlog;

	if ((DUMP_START_EXCH == g_rdr_global->app.start_flag)
		|| (DUMP_START_EXCH == g_rdr_global->comm.start_flag)
		|| (DUMP_START_EXCH == g_rdr_global->m3.start_flag)
		|| (DUMP_START_REBOOT == g_rdr_global->app.start_flag)
		|| (DUMP_START_REBOOT == g_rdr_global->comm.start_flag)
		|| (DUMP_START_REBOOT == g_rdr_global->m3.start_flag)) {
		switch (pbb->top_head.except_reason) {
		case HISI_RDR_MOD_EXCE_PRESS8S:
			reason = "AP_PRESSPOWER_8S";
			break;
		case HISI_RDR_MOD_EXCE_NORMAL:
			reason = "Software EXCE";
			break;
		case HISI_RDR_MOD_EXCE_WD:
			reason = "WATCHDOG EXCE";
			break;
		case HISI_RDR_MOD_EXCE_PANIC:
			reason = "ARM EXCE";
			break;
		case HISI_RDR_MOD_EXCE_PMU:
			reason = "PMU EXCE";
			break;
		case HISI_RDR_MOD_EXCE_STACKFLOW:
			reason = "STACK OVERFLOW EXCE";
			break;
		case HISI_RDR_MOD_EXCE_UNDEF:
			reason = "UNDEF EXCE";
			break;
		default:
			reason = "UNKNOW EXCE";
			break;
		}

		base_info =
		(pbb->top_head.except_core | RDR_NCP) == HISI_RDR_MOD_CORE_AP ?
				g_rdr_base : NULL;

		core = rdr_get_ecore(pbb);
	} else if ((DUMP_START_CRASH == g_rdr_global->app.start_flag)
		&& (DUMP_START_CRASH == g_rdr_global->comm.start_flag)) {
		reason = "CRASH";
		core = RDR_STR_CORE_UNKNOWN;
		base_info = NULL;
	} else {
		reason = NORMAL_POWER_ON;
		core = NULL;
		base_info = NULL;
	}

	if (NULL == core) {
		(void)snprintf(resetlog, siz_resetlog,
			"rdr:system reboot reason: %s %s\n", reason, "");
		pr_info("rdr:system reboot reason: %s %s\n", reason, "");
	} else {
		if (NULL == base_info) {
			(void)snprintf(resetlog,
				siz_resetlog,
				"rdr:system reboot reason: %s %s\n",
				reason, core);
			pr_info("rdr:sys reboot reason: %s %s\n", reason, core);
		} else {
			char clock[32];
			rdr_get_xtime(pbb, clock, 32);
			(void)snprintf(resetlog,
				siz_resetlog,
				"sysreboot reason: %s %s, tick: %s, systemError para: ModId=0x%x, Arg1=%u, Arg2=%u\n",
				reason, core, clock,
				base_info->modid,
				base_info->arg1,
				base_info->arg2);
			pr_info("sysreboot reason: %s %s, tick: %s, systemError para: ModId=0x%x, Arg1=%u, Arg2=%u\n",
				reason, core, clock,
				base_info->modid,
				base_info->arg1,
				base_info->arg2);
		}
	}

	return;
}

void rdr_save_cpinfo(struct rdr_struct_s *bb, char *timedir)
{
    struct rdr_a1_reserve_s *r;
    u8 *cpinfo;
	char reset_info[RDR_DUMP_CP_INFO_MAX_LEN] = {0};
    char xname[RDR_FNAME_LEN] = {0};

    r = ((struct rdr_a1_reserve_s *)(rdr_core_addr(bb, RDR_AREA_RESERVE)));
    cpinfo = r->cp_reserve.content.rdr_cpinfo;

    if(!rdr_is_cp_modid(g_rdr_base->modid)) {
        pr_info("rdr:reset core is not cp.\n");
        return;
    }

    memset(xname,0,sizeof(xname));
    snprintf(xname, RDR_FNAME_LEN-1,"%s%s/reset.log", OM_ROOT_PATH, timedir);/* [false alarm]: RDR_FNAME_LEN-1 As expected  */

    /*ap reboot cp*/
    if(rdr_is_ap_reboot_cp_modid(g_rdr_base->modid)) {
        pr_info("rdr:ap reboot cp, ap save to reset log\n");
        snprintf(reset_info, sizeof(reset_info)-1,"reboot_context:0x%x\n"
        "reboot_task:0x%x\ntask_name:%s\nreboot_int:0x%x\n"
        "modid:0x%x\narg1:0x%x\narg2:0x%x\narg3:0x%x\narg3_len:0x%x\nvec:0x%x\n",
        g_rdr_base->reboot_context,g_rdr_base->reboot_task,g_rdr_base->task_name,
        g_rdr_base->reboot_int,g_rdr_base->modid,g_rdr_base->arg1,
        g_rdr_base->arg2,g_rdr_base->arg3,g_rdr_base->arg3_length,g_rdr_base->vec);
        rdr_append_file(xname, (void *)reset_info,strlen(reset_info), RDR_RESET_LOG_MAX); /*RDR_RESET_LOG*/
        return;
    }

    /*cp info*/
    if(*cpinfo != '\0') {
        pr_info("rdr:save cpinfo to reset log\n");
        rdr_append_file(xname, (void *)cpinfo,RDR_DUMP_CP_INFO_MAX_LEN, RDR_RESET_LOG_MAX); /*RDR_RESET_LOG*/
    }
    return;
}

void rdr_save_resetlog(struct rdr_struct_s *bb, char *timedir)
{
	struct rdr_a1_reserve_s *r;
	u8 *resetlog;
	char xname[RDR_FNAME_LEN] = {0};

    /*save reset log all the time*/
    /*
	if (rdr_nv_get_value(RDR_NV_RESETLOG) == 0)
		return;
	*/
	r = ((struct rdr_a1_reserve_s *)(rdr_core_addr(bb, RDR_AREA_RESERVE)));
	resetlog = r->ap_cp_share.content.resetlog;

	if (*resetlog != '\0') {
		pr_info("save reset log:%s\n", resetlog);
		snprintf(xname, RDR_FNAME_LEN-1,
			"%s%s/reset.log", OM_ROOT_PATH, timedir);  /* [false alarm]: RDR_FNAME_LEN-1 As expected  */
		rdr_append_file(xname, (void *)resetlog,
			strlen(resetlog), RDR_RESET_LOG_MAX); /*RDR_RESET_LOG*/
	}

    rdr_save_cpinfo(bb, timedir);

	return;
}

#define dump_save_app_done() \
	({g_rdr_global->app.save_flag = DUMP_SAVE_SUCCESS; })

#define dump_save_ext_done() \
	({g_rdr_global->app.ext_save_flag = DUMP_SAVE_SUCCESS; })
/*lint -e750 */
#define dump_save_done()\
		({g_rdr_global->app.save_file_flag = DUMP_SAVE_FILE_END; })
/*lint +e750 */

struct rdr_field_hook_s {
	struct list_head hook_list;
	s32 field_id;
	rdr_funcptr_0 pfunc;
};

static LIST_HEAD(g_rdr_hook_list);
static DEFINE_SPINLOCK(g_rdr_hook_list_lock);

void rdr_save_ext(void)
{
	struct list_head *cur = NULL;
	struct rdr_field_hook_s *p = NULL;

	list_for_each(cur, &g_rdr_hook_list) {
		spin_lock(&g_rdr_hook_list_lock);
		p = list_entry(cur, struct rdr_field_hook_s, hook_list);
		spin_unlock(&g_rdr_hook_list_lock);

		if (p->pfunc)
			p->pfunc();
	}

	dump_save_ext_done();
	return;
}

s32 rdr_dump_register_hook(int field_id, void *func)
{
	struct rdr_field_hook_s *p;
	p = kmalloc(sizeof(struct rdr_field_hook_s), GFP_KERNEL);
	if (p == NULL)
		return -1;

	p->field_id  = field_id;
	p->pfunc     = (rdr_funcptr_0)func;

	spin_lock(&g_rdr_hook_list_lock);
	list_add(&p->hook_list, &g_rdr_hook_list);
	spin_unlock(&g_rdr_hook_list_lock);

	return 0;
}
EXPORT_SYMBOL_GPL(rdr_dump_register_hook);

/*lint -e750 */
#define HIFI_DIE_NOTIFY_LPM3 ((0 << 24) | (16 << 16) | (3 << 8) | (1 << 0))
#define NOT_HIFI_LPM3_NOTIFY_LPM3 ((0 << 24) | (16 << 16) | (3 << 8) | (3 << 0))
#define LPM3_SAVE_OK_NOTIFY_AP ((8 << 24) | (16 << 16) | (3 << 8) | (3 << 0))
#define LPM3_PWROFF_HIFI_OK_NOTIFY_AP ((8 << 24) | (16 << 16) | (3 << 8) | (1 << 0))
/*lint +e750 */

static void rdr_notify_other_core(u32 mod_id)
{
	u32 msg = 0xdead;/*need fixup*/

	if (rdr_is_cp_inap_modid(mod_id))
		goto noti_is_ap;

	switch (mod_id & HISI_RDR_MOD_CORE_MASK) {
	case HISI_RDR_MOD_CORE_AP:
noti_is_ap:
		if ((mod_id & HISI_RDR_MOD_EXCE_MASK) == HISI_RDR_MOD_EXCE_WD)
			pr_info("rdr:ap watchdog exception: impossible\n");

		pr_info("rdr:ap excep,nofify cp(ipc),iom3(nmi),lpm3(ipc)\n");
		hisi_rdr_ipc_notify_cp();
		//hisi_rdr_nmi_notify_iom3();
		msg = NOT_HIFI_LPM3_NOTIFY_LPM3;
		//hisi_rdr_ipc_notify_lpm3(&msg, 1);
		//hisi_rdr_nmi_notify_lpm3();
		/*hisi_rdr_ipc_notify_asp(&msg, 1);*/ /*need fixup*/
		break;
	case HISI_RDR_MOD_CORE_CP:
		pr_info("rdr:cp excep,nofify iom3(nmi),lpm3(ipc)\n");
		/*hisi_rdr_ipc_notify_cp();*/
		/*hisi_rdr_ipc_notify_iom3(&msg, 1);*/
        if(rdr_is_ap_reboot_cp_modid(mod_id)) {
            hisi_rdr_ipc_notify_cp();
        }
		msg = NOT_HIFI_LPM3_NOTIFY_LPM3;
		hisi_rdr_ipc_notify_lpm3(&msg, 1);


		//hisi_rdr_nmi_notify_iom3();
		/*hisi_rdr_ipc_notify_asp(&msg, 1);*/ /*need fixup*/
		break;
	case HISI_RDR_MOD_CORE_HIFI:
		if ((mod_id & HISI_RDR_MOD_EXCE_MASK) == HISI_RDR_MOD_EXCE_WD) {
#ifdef HIFI_WD_DEBUG
			hifireset_runcbfun(DRV_RESET_CALLCBFUN_RESET_BEFORE);
			reset_set_cpu_status(0, 0);
			hisi_rdr_nmi_notify_hifi();
#else
			pr_info("rdr:hifi excep,nofify lpm3(ipc)\n");
			msg = HIFI_DIE_NOTIFY_LPM3;
			hisi_rdr_ipc_notify_lpm3(&msg, 1);
#endif
		} else
			pr_info("rdr:hifi non-watchdog exception:impossible\n");
		break;
	case HISI_RDR_MOD_CORE_IOM3:
		if ((mod_id & HISI_RDR_MOD_EXCE_MASK) == HISI_RDR_MOD_EXCE_WD) {
			pr_info("rdr:iom3 watchdog excep,nofify iom3(nmi)\n");
			hisi_rdr_nmi_notify_iom3();
		}

		pr_info("rdr:iom3 excep,nofify cp(ipc)\n");
		/*hisi_rdr_ipc_notify_cp();*/
		/*hisi_rdr_ipc_notify_asp(&msg, 1);*/
		break;
	case HISI_RDR_MOD_CORE_LPM3:
		pr_info("rdr:lpm3 excep,nofify cp(ipc),iom3(nmi)\n");
		//hisi_rdr_nmi_notify_iom3();
		hisi_rdr_ipc_notify_cp();
		/*hisi_rdr_ipc_notify_lpm3(&msg, 1);*/
		/*hisi_rdr_ipc_notify_asp(&msg, 1);*/
		break;
	default:
		pr_info("rdr:unknown core,dont send ipc interrupt\n");
		break;
	}
}

static DEFINE_SPINLOCK(rdr_v_dump_enter_lock);
static int rdr_v_dump_enter;

static void rdr_system_dump(u32 mod_id, u32 arg1, u32 arg2, char *data, u32 len)
{
	u32 usec;
	unsigned long lock_flags;

	pr_info("rdr:%s() enter, begin to dump mem.\n", __func__);
	/* this fun has already been called, not enter again, return. */
	spin_lock_irqsave(&rdr_v_dump_enter_lock, lock_flags);
	if (rdr_v_dump_enter == 1) {
		spin_unlock_irqrestore(&rdr_v_dump_enter_lock, lock_flags);
		return;
	}
	rdr_v_dump_enter = 1;
	spin_unlock_irqrestore(&rdr_v_dump_enter_lock, lock_flags);

	preempt_disable();

	/* TODO: stop etm: */

	/* if rdr hadn't inited, do nothing, return or reboot at once. */
	if (DUMP_INIT_FLAG != g_rdr_global->app.init_flag) {
		/*wdt_reboot();*/
		preempt_enable();
		return;
	}

	/* TODO:send ipc message to other process core. */
	rdr_notify_other_core(mod_id);
	if ((mod_id & HISI_RDR_MOD_CORE_MASK) == HISI_RDR_MOD_CORE_HIFI) {
		preempt_enable();
		return;
	}

	/* stop rdr,not record anything. */
	rdr_system_error_enter();

	/*save system exception base info */
	field_set(u32, fr.reboot_time, rdr_gettick(&usec));
	*(field_addr(u32, fr.reboot_time) + 1) = usec;
	dump_save_exc_task(); /* save exception task info */

	/* save global and base info */
	rdr_save_global_info(mod_id);
	rdr_save_base(mod_id, arg1, arg2, data, len);

	dump_save_all_task();

	/*dump_save_regs();*/

	rdr_set_xtime(pbb);
	rdr_record_resetlog();
	dump_save_printk(); /* save printk info */
	pbb->area_info[RDR_AREA_RESERVE].length = 0;

	g_rdr_global->app.save_file_flag = DUMP_SAVE_FILE_NEED;

	preempt_enable();

	if (mod_id != HISI_RDR_MOD_AP_PANIC)
		rdr_save_ext();
	dump_save_app_done(); /* set app data save ok tag */
}

atomic_t volatile rdr_in_suspend = ATOMIC_INIT(0);
atomic_t volatile rdr_in_saving = ATOMIC_INIT(0);

static void rdr_wait_for_dump(int timeout_ms)
{
	int i;
	int inter_ms = 10;

	pr_info("rdr:we wait for all cpu cores write info to mem ok ...\n");
	for (i = 0; i < timeout_ms; i+= inter_ms) {
		msleep(inter_ms);
		/* wait for ccore dump OK: */
		if ((g_rdr_global->comm.save_flag == DUMP_SAVE_SUCCESS) &&
			(g_rdr_global->app.save_flag == DUMP_SAVE_SUCCESS) &&
			(g_rdr_global->m3.save_flag == DUMP_SAVE_SUCCESS)) {
			pr_info("rdr:all cpu cores write info to mem OK!\n");
			break;
		}
	}
	pr_info("rdr:wait dump timeout,result:cp:0x%08x,ap:0x%08x,m3:0x%08x\n",
		g_rdr_global->comm.save_flag, g_rdr_global->app.save_flag,
		g_rdr_global->m3.save_flag);
}

int rdr_need_save_before_reboot(u32 mod_id)
{
	if (CORE_EXCE_NOT_REBOOT) {
		pr_info("rdr:we should save file to emmc before reboot!\n");
		return 1;
	}

	pr_info("rdr:we dont save file to emmc before reboot!\n");
	return 0;
}

/*
0x00000000-0x0fffffff DRV
0x10000000-0x1fffffff OSA
0x20000000-0x2fffffff OM
0x30000000-0x3fffffff GU L2
0x40000000-0x4fffffff GU WAS
0x50000000-0x5fffffff GU GAS
0x60000000-0x6fffffff GU NAS
0x70000000-0x7fffffff GU DSP
0xa0000000-0xafffffff LPS

0x80000000-0x8fffffff: AP mod
*/
int rdr_is_cp_inap_modid(u32 mod)
{
	if (mod <= 0x7fffffff)
		return 1;
	if ((mod >= 0xa0000000) && (mod <= 0xafffffff))
		return 1;
	if (mod == HISI_RDR_MOD_CP_DO_RESET) /* this id is ap :( */
		return 1;

	return 0;
}

int rdr_is_cp_modid(u32 mod)
{
	if (mod <= 0x7fffffff)
		return 1;
	if ((mod >= 0xa0000000) && (mod <= 0xafffffff))
		return 1;
	if ((mod & HISI_RDR_MOD_CORE_MASK) == HISI_RDR_MOD_CORE_CP)
		return 1;
	return 0;
}

int rdr_is_ap_reboot_cp_modid(u32 mod)
{
    if(HISI_RDR_MOD_CP_RILD == mod || HISI_RDR_MOD_CP_3RD == mod) {
        return 1;
    }
    return 0;
}

int rdr_modid_is_known(u32 mod)
{
	if ((mod >= 0x80000000) && (mod <= 0x8fffffff)) /* ap iom3 lpm3 ... */
		return 1;
	if (rdr_is_cp_modid(mod))/*cp*/
		return 1;

	return 0;
}

int rdr_need_reboot(u32 modid)
{
	if (((modid & HISI_RDR_MOD_CORE_MASK) == HISI_RDR_MOD_CORE_HIFI)||
		((modid & HISI_RDR_MOD_CORE_MASK) == HISI_RDR_MOD_CORE_IOM3))
		return 0;  /*system will be have no reset when HIFI have exce*/
	if ((modid & HISI_RDR_MOD_CORE_MASK) == HISI_RDR_MOD_CORE_AP) {
		if ((modid & HISI_RDR_MOD_EXCE_MASK) == HISI_RDR_MOD_EXCE_PANIC)
			return 0; /*in arm exception,it can reboot sys itself*/
		if ((modid & HISI_RDR_MOD_EXCE_MASK) == HISI_RDR_MOD_EXCE_WD)
			return 0;
		if (modid == HISI_RDR_MOD_AP_PRESS8S)
			return 0;
	}
	if (CORE_EXCE_NOT_REBOOT) {
		if (rdr_is_cp_modid(modid)) /*if cp, reboot system now !!*/
			return 1;
		if (rdr_modid_is_known(modid))/* known, reboot!! */
			return 1;
		else /* unknown, not reboot */
			return 0;
	}

	return 1;
}

static void rdr_reboot(u32 mod_id)
{
	pr_info("rdr:%s() begin to reboot\n", __func__);
#ifdef CONFIG_HISI_REBOOT_TYPE
	/* set flag for reset_type*/
	if ((mod_id & HISI_RDR_MOD_EXCE_MASK) == HISI_RDR_MOD_EXCE_WD) {
		/*watchdog*/
		set_watchdog_resetflag();
	} else if ((mod_id & HISI_RDR_MOD_EXCE_MASK) ==
					HISI_RDR_MOD_EXCE_PANIC) {
		set_panic_resetflag();
	} else {
		/*default:panic*/
		set_panic_resetflag();
	}
#endif
	/* wait for other core save mem ok: */
	rdr_wait_for_dump(1000);

	/*Let SecureOS notify LPM3 to reboot.*/
	emergency_restart();
}

struct wake_lock rdr_wake_lock;

static LIST_HEAD(g_rdr_system_error_list);
static DEFINE_SPINLOCK(g_rdr_system_error_list_lock);

void hisi_register_system_error(enum rdr_modid_e mod_id,
			u32 arg1, u32 arg2, char *data, u32 length)
{
	struct rdr_system_error_param_s *p = NULL;
	p = kmalloc(sizeof(struct rdr_system_error_param_s), GFP_ATOMIC);
	if (p == NULL)
		return;

	p->modid = mod_id;
	p->arg1 = arg1;
	p->arg2 = arg2;
	p->data = data;
	p->len = length;

	spin_lock(&g_rdr_system_error_list_lock);
	list_add_tail(&p->sys_err_list, &g_rdr_system_error_list);
	spin_unlock(&g_rdr_system_error_list_lock);
}

void hisi_system_error(enum rdr_modid_e mod_id,
			u32 arg1, u32 arg2, char *data, u32 length)
{
	if (1 != atomic_read(&rdr_inited))
		return;
	rdr_current_task = (u64)get_current();
	/* save userdata to 8m. */
	dump_save_usr_data(data, length);
	hisi_register_system_error(mod_id, arg1, arg2, data, length);
	wake_lock(&rdr_wake_lock);
	up(&rdr_sem);
	return;
}

void dump_exc_hook(void *curr_task_id, int vec, const u32 *p_reg)
{
	u64 curr_task_pid = (u64)(((struct task_struct *)curr_task_id)->pid);
	static int v_exc_hook_enter;
	size_t reg_set_size = (size_t)(ARM_REGS_NUM * 4);

	/* this fun has already been called, not enter again, return. */
	if (v_exc_hook_enter == 1)
		return;
	v_exc_hook_enter = 1;

	g_rdr_base->vec = vec;
	g_rdr_base->cpsr = p_reg[16];
	g_rdr_base->current_task = curr_task_pid;

	/*copy register information*/
	memcpy(g_rdr_base->regset, (const void *)p_reg, reg_set_size);

	pr_info("rdr:onlyDumpMem,dontSave,id:0x%08x\n", HISI_RDR_MOD_AP_PANIC);
	rdr_current_task = (u64)curr_task_id;
	rdr_system_dump(HISI_RDR_MOD_AP_PANIC, HISI_RDR_MOD_EXCE_PANIC, 0, 0, 0);

	return;
}

atomic_t syscall_hook_on = ATOMIC_INIT(0);
static atomic_t buddy_fail_hook_on = ATOMIC_INIT(0);
static atomic_t warn_on_hook_on = ATOMIC_INIT(0);

static struct notifier_block rdr_reboot_nb;
static struct notifier_block rdr_sr_nb;
static int rdr_reboot_notifier(struct notifier_block *nb,
		unsigned long foo, void *bar)
{
	if (RDR_RESET_AS_EXCE)
		pbb->top_head.except_reason = HISI_RDR_MOD_EXCE_NORMAL;
	g_rdr_global->app.start_flag = DUMP_START_REBOOT;
	g_rdr_global->comm.start_flag = DUMP_START_REBOOT;
	g_rdr_global->m3.start_flag = DUMP_START_REBOOT;

	/* prevent access the emmc now: */
	pr_info("rdr:%s: shutdown +\n", __func__);
	atomic_set(&rdr_in_suspend, 1);
	while (1) {
		if (atomic_read(&rdr_in_saving))
			msleep(1000);
		else
			break;
		pr_info("rdr:is saving rdr, wait 1s ...\n");
	}
	pr_info("rdr:%s: shutdown -\n", __func__);

	return 0;
}

void rdr_warn_on_hook(void)
{
	if (atomic_read(&warn_on_hook_on)) {
		u32 buf[3];
		queue_time_in(buf);
		buf[2] = 0x6e726177;/* warn tag */
		stack_queue(fr.warn_on, buf, sizeof(buf), NULL);
	}

	return;
}
EXPORT_SYMBOL(rdr_warn_on_hook);

#ifdef CONFIG_DETECT_HUNG_TASK
static void rdr_hung_task_hook(unsigned long tsk, unsigned long timeout)
{
	struct task_struct *task = (struct task_struct *)tsk;
	u32 buf[5];
	queue_time_in(buf);
	buf[2] = 0x676e7568;/* hung tag */
	buf[3] = (u32)task->pid;
	buf[4] = timeout;
	stack_queue(fr.hung_task, buf, sizeof(buf), task);

	return;
}
#endif

void rdr_buddy_fail_hook(void)
{
	if (atomic_read(&buddy_fail_hook_on)) {
		u32 buf[3];
		queue_time_in(buf);
		buf[2] = 0x64647562;/* budd tag */
		stack_queue(fr.buddy_fail, buf, sizeof(buf), NULL);
	}

	return;
}
EXPORT_SYMBOL(rdr_buddy_fail_hook);

#ifdef CONFIG_CPU_FREQ
static void cpu_on_off_hook(u32 cpu, u32 cluster, u32 on)
{
	u32 qbuf[5];
	struct hisi_ringbuffer_s *q =
		field_addr(struct hisi_ringbuffer_s, fr.cpu_on_off);

	queue_time_in(qbuf);
	qbuf[2] = cluster;
	qbuf[3] = cpu;
	qbuf[4] = on;
	hisi_ringbuffer_write(q, qbuf);

	return;
}
#endif

static void rdr_register_hook_func(void)
{
	static atomic_t rdr_reboot_hook_on = ATOMIC_INIT(0);
#ifdef CONFIG_HISI_RDR_SWITCH
	if (1 == g_dump_cfg.dump_cfg.bits.task_switch) {
		(void)task_switch_hook_add(
			(rdr_funcptr_2)dump_task_switch_hook);
	}
	if (1 == g_dump_cfg.dump_cfg.bits.int_switch)
		int_switch_hook_add(dump_int_switch_hook);

	atomic_set(&syscall_hook_on, 1);
#endif
#if 0  //delete by xiehongliang for seattle.
	if (1 == g_dump_cfg.dump_cfg.bits.int_switch)
		interrupts_list_hook_add(interrupts_list_hook);
#endif
	if (1 == g_dump_cfg.dump_cfg.bits.arm_exc)
		exc_hook_add((rdr_funcptr_3)dump_exc_hook);

	atomic_set(&warn_on_hook_on, 1);
#ifdef CONFIG_DETECT_HUNG_TASK
	rdr_hung_task_hook_add(rdr_hung_task_hook);
#endif
	atomic_set(&buddy_fail_hook_on, 1);

	/*
	if (1 == g_dump_cfg.dump_cfg.bits.suspend_resume)
		suspend_resume_hook_add((rdr_funcptr_2)suspend_resume_hook);
	*/

#ifdef CONFIG_CPU_FREQ
	rdr_cpu_on_off_hook_add(cpu_on_off_hook);
#endif

	if (atomic_read(&rdr_reboot_hook_on) == 0) {
		rdr_reboot_nb.notifier_call = rdr_reboot_notifier;
		rdr_reboot_nb.priority = -1;
		(void)register_reboot_notifier(&rdr_reboot_nb);
		atomic_set(&rdr_reboot_hook_on, 1);
	}

	g_rdr_global->app.task_switch_start = 1;
	g_rdr_global->app.int_switch_start = 1;

	return;
}

/* compress the date and time to 16 bytes, size of out is 16 bytes. */
static void get_builddatetime(u8 *out)
{
	u8 *pout = out;
	u8 *p = (u8 *)__DATE__; /* Nov 11 2013 */
	int cnt = RDR_BUILD_DATE_TIME_LEN;

	memset((void *)out, 0, RDR_BUILD_DATE_TIME_LEN);

	while (*p) {
		if (!cnt)
			return;
		if (*p != ' ') {
			*pout++ = *p++;
			cnt--;
		} else
			p++;
	}

	p = (u8 *)__TIME__; /* 11:04:08 */
	while (*p) {
		if (!cnt)
			return;
		if (*p != ':') {
			*pout++ = *p++;
			cnt--;
		} else
			p++;
	}
	out[RDR_BUILD_DATE_TIME_LEN - 1] = '\0';
	return;
}

static int dump_need_save(struct rdr_struct_s *bb)
{
	u8 buildtime[RDR_BUILD_DATE_TIME_LEN];
	struct rdr_top_head_s *h = &bb->top_head;
	struct rdr_a1_reserve_s *r;
	struct rdr_global_internal_s *g;

	r = ((struct rdr_a1_reserve_s *)(rdr_core_addr(bb, RDR_AREA_RESERVE)));
	g = (struct rdr_global_internal_s *)
			(r->ap_cp_share.content.rdr_global_internal);

	get_builddatetime(buildtime);
	if (strncmp((char *)h->build_time, (char *)buildtime,
			strlen((char *)h->build_time))) {
		pr_info("rdr:%s():buildtime not match,dont save\n", __func__);
		return 0;
	}

	/* check version info :*/
	if (h->version != RDR_VERSION) {
		pr_info("rdr:%s():version not match,dont save\n", __func__);
		return 0;
	}

	if (strncmp((const char *)(h->product_name), RDR_PRODUCT,
				strlen(RDR_PRODUCT))) {
		pr_info("rdr:%s():productname not match,dont save\n", __func__);
		return 0;
	}

	if (strncmp((const char *)(h->product_version), RDR_PRODUCT_VERSION,
				strlen(RDR_PRODUCT_VERSION))) {
		pr_info("rdr:%s():product ver not match,dont save\n", __func__);
		return 0;
	}

	if (RDR_RESET_AS_EXCE) { /* press reset key as exception */
		if (HISI_RDR_MOD_EXCE_UNDEF == bb->top_head.except_reason) {
			pr_info("rdr:%s():sys reset,need save\n", __func__);
			return 1;
		}
	}

	if ((DUMP_START_EXCH == g->app.start_flag)
		|| (DUMP_START_EXCH == g->comm.start_flag)
		|| (DUMP_START_EXCH == g->m3.start_flag)) {
		if (DUMP_SAVE_FILE_NEED == g->app.save_file_flag) {
			pr_info("rdr:%s():flag need save\n", __func__);
			return 1;
		} else {
			pr_info("rdr:%s():flag neednt save\n", __func__);
			return 0;
		}
	} else if ((DUMP_START_REBOOT == g->app.start_flag)
		|| (DUMP_START_REBOOT == g->comm.start_flag)) {
		pr_info("rdr:%s():start flag ok,neednt save\n", __func__);
		return 0;
	} else if ((DUMP_START_CRASH == g->app.start_flag)
		|| (DUMP_START_CRASH == g->comm.start_flag)) {
		pr_info("rdr:%s():reset or burst die,need save\n", __func__);
		return 1;
	} else {
		pr_info("rdr:%s():dont save\n", __func__);
		return 0;
	}
}

/* save 0x3000 0000 - 0x3800 0000 */
#define RDR_SAVE_SRAM96K_START (HI_SRAM_MEM_ADDR)
#define RDR_SAVE_128M_START    (DDR_MEM_ADDR)
#define RDR_SAVE_MODEM_SHARE   (DDR_SHARED_MEM_ADDR)
#define RDR_SAVE_SOCP          (DDR_SOCP_ADDR)
#define RDR_SAVE_HIFI          (DDR_HIFI_ADDR)
#define RDR_LPM3_SRAM96K_START 0x00020000

/*lint -e750 */
#define RDR_MODEM_SZ (DDR_MEM_SIZE - DDR_SHARED_MEM_SIZE - DDR_HIFI_SIZE-DDR_SOCP_SIZE)
#define RDR_MODEM_SHARE_SZ  (DDR_SHARED_MEM_SIZE)
#define RDR_SOCP_SZ         (DDR_SOCP_SIZE)
#define RDR_HIFI_SZ         (DDR_HIFI_SIZE)
#define RDR_SRAM96K_SZ      (96 * 1024)

#define HIFI_TCM_PHY_START		(0xE8058000)
#define HIFI_TCM_SIZE			(84*1024)

#define HIFI_OCRAM_PHY_START	(0xE8000000)
#define HIFI_OCRAM_SIZE			(256*1024)
/*lint +e750 */

int dump_hifi_ddr(char *timedir)
{
	int ret = 0, fd = -1, i;
	int histep = RDR_HIFI_SZ / 3;
	void *start = NULL;
	char xname[RDR_FNAME_LEN] = {0};
	if (rdr_nv_get_value(RDR_NV_HIFI) == 0)
		return 0;

	/* save hifi 9M ddr mem */
	snprintf(xname, RDR_FNAME_LEN-1, "%s%s/hifi3.bin", OM_ROOT_PATH, timedir);  /* [false alarm]: RDR_FNAME_LEN-1 As expected  */
	ret = rdr_loopwrite_open(xname, &fd);/*RDR_HIFI_BIN*/
	if (ret != 0) {
		pr_err("rdr:open %s failed\n", xname);
		goto error;
	}

	pr_info("%s:ddr:0x%08x,sz:%d\n", __func__, RDR_SAVE_HIFI, RDR_HIFI_SZ);

	for (i = 0; i < RDR_HIFI_SZ; i += histep) {
		start = ioremap(RDR_SAVE_HIFI + i, histep);
		if (NULL == start) {
			pr_err("rdr:hifi 0x%x ioremap failed\n",
					RDR_SAVE_HIFI + i);
			ret = -1;
			goto error1;
		}
		ret = rdr_loopwrite_append(fd, start, histep);
		if (ret != 0) {
			iounmap(start);
			pr_err("rdr:dump %s %d failed\n", RDR_HIFI_BIN, i);
			goto error1;
		}
		iounmap(start);
	}

error1:
	rdr_loopwrite_close(fd);
error:
	return ret;
}

#ifdef HIFI_WD_DEBUG
int dump_hifi_tcm(char *timedir)
{
	int ret = 0, fd = -1;
	void *start = NULL, *buf = NULL;
	char xn[RDR_FNAME_LEN] = {0};

	snprintf(xn, sizeof(xn), "%s%s/hifi3_tcm.bin", OM_ROOT_PATH, timedir); /* [false alarm]: RDR_FNAME_LEN-1 As expected */
	ret = rdr_loopwrite_open(xn, &fd);/*RDR_HIFI_TCM*/
	if (ret != 0) {
		pr_err("rdr:open %s failed\n", xn);
		goto error;
	}

	pr_info("%s:tcm addr:0x%08x,size:%d\n", __func__,
			HIFI_TCM_PHY_START, HIFI_TCM_SIZE);

	start = kmalloc(HIFI_TCM_SIZE, GFP_KERNEL);
	if (NULL == start) {
		pr_err("%s:alloc buf failed\n", __func__);
		ret = -1;
		goto error1;
	}

	buf = ioremap(HIFI_TCM_PHY_START, HIFI_TCM_SIZE);
	if (NULL == buf) {
		pr_err("rdr:hifi 0x%x ioremap failed\n", HIFI_TCM_PHY_START);
		ret = -1;
		kfree(start);
		goto error1;
	}
	memcpy(start, buf, HIFI_TCM_SIZE);

	ret = rdr_loopwrite_append(fd, start, HIFI_TCM_SIZE);

	iounmap(buf);
	kfree(start);
error1:
	rdr_loopwrite_close(fd);
error:
	return ret;
}

int dump_hifi_ocram(char *timedir)
{
	int ret = 0, fd = -1;
	void *start = NULL;
	char xn[RDR_FNAME_LEN] = {0};

	snprintf(xn, sizeof(xn), "%s%s/hifi3_ocram.bin", OM_ROOT_PATH, timedir); /* [false alarm]: RDR_FNAME_LEN-1 As expected */
	ret = rdr_loopwrite_open(xn, &fd);/*RDR_HIFI_OCRAM*/
	if (ret != 0) {
		pr_err("rdr:open %s failed\n", xn);
		goto error;
	}

	pr_info("%s:ocram addr:0x%08x,size:%d\n", __func__,
			HIFI_OCRAM_PHY_START, HIFI_OCRAM_SIZE);

	start = ioremap(HIFI_OCRAM_PHY_START, HIFI_OCRAM_SIZE);
	if (NULL == start) {
		ret = -1;
		pr_err("rdr:hifi 0x%x ioremap failed\n", HIFI_OCRAM_PHY_START);
		goto error1;
	}

	ret = rdr_loopwrite_append(fd, start, HIFI_OCRAM_SIZE);
	iounmap(start);

error1:
	rdr_loopwrite_close(fd);
error:
	return ret;
}

void rdr_save_hifi(int mod_id, char *timedir)
{
	int ret = 0;
	unsigned int * power_status_addr = NULL;
	bool is_dump_tcm_ocram = false;

	power_status_addr = ioremap(DRV_DSP_POWER_STATUS_ADDR, 0x4);
	if (NULL == power_status_addr) {
		pr_err("DRV_DSP_POWER_STATUS_ADDR ioremap failed\n");
		is_dump_tcm_ocram = false;
	} else {
		is_dump_tcm_ocram = (DRV_DSP_POWER_ON == readl(power_status_addr)) ? true : false;
	}
	iounmap(power_status_addr);
	power_status_addr = NULL;

	rdr_looprw_set_state(1);

	ret = dump_hifi_ddr(timedir);
	pr_info("rdr:%s():save hifi:%s\n", __func__, ret ? "fail" : "success");

	/*HIFI WD ERR  save tcm and ocram data and reset hifi image*/
	if (mod_id == HISI_RDR_MOD_HIFI_WD) {
		/* save hifi tcm mem */
		if ((rdr_nv_get_value(RDR_NV_HIFI_TCM) == 1) && (is_dump_tcm_ocram)) {
			ret = dump_hifi_tcm(timedir);
			pr_info("%s:dump hifi tcm,%s\n", __func__,
					ret ? "fail" : "success");
		}
		/* save hifi ocram mem: */
		if ((rdr_nv_get_value(RDR_NV_HIFI_OCRAM) == 1) && (is_dump_tcm_ocram)) {
			ret = dump_hifi_ocram(timedir);
			pr_info("rdr:%s():dump_hifi_ocram,%s\n", __func__,
					ret ? "fail" : "success");
		}
		/*reset hifi ddr bss section*/
		ret = reset_hifi_sec();
		pr_info("rdr:%s():reset hifi sec,%s\n", __func__,
				ret ? "fail" : "success");
	}

	rdr_looprw_set_state(0); /* restore fs read/write to origin state */

	return;
}

#endif

int rdr_128m_modem(char *timedir)
{
	void *start = NULL;
	int i, ret, err;
	int fd = -1;
	char xname[RDR_FNAME_LEN] = {0};
	int modem_siz_left = RDR_MODEM_SZ % SZ_4M;
	int modem_siz = RDR_MODEM_SZ - modem_siz_left;
	if (rdr_nv_get_value(RDR_NV_MODEM_MEM) == 0)
		return 0;
	/* save modem phymem: */
	rdr_looprw_set_state(1);
	err = 0;
	snprintf(xname, RDR_FNAME_LEN-1, "%s%s/modem.bin", OM_ROOT_PATH, timedir); /* [false alarm]: RDR_FNAME_LEN-1 As expected  */
	ret = rdr_loopwrite_open(xname, &fd);/*RDR_MODEM_BIN*/
	if ((ret != 0) || (fd == -1)) {
		pr_err("rdr:open %s failed\n", xname);
		rdr_looprw_set_state(0);
		return -1;
	}
	for (i = 0; i < modem_siz; i += SZ_4M) {
		start = ioremap(RDR_SAVE_128M_START + i, SZ_4M);
		if (start == NULL) {
			DUMP_LOG(0);
			pr_err("4M in 128M mem io remap failed, i:%d\n", i);
			err = 1;
			break;
		}

		/* save append */
		ret = rdr_loopwrite_append(fd, start, SZ_4M);
		if (ret != 0) {
			pr_err("rdr:save 4m failed, i:%d\n", i);
			iounmap(start);
			err = 2;
			break;
		}

		/*release:*/
		iounmap(start);
	}
	start = ioremap(RDR_SAVE_128M_START + modem_siz, modem_siz_left);
	if (start == NULL) {
		DUMP_LOG(0);
		pr_err("modem left mem 3m ioremap failed\n");
		rdr_loopwrite_close(fd);
		rdr_looprw_set_state(0);
		return -2;
	}

	/* save append */
	ret = rdr_loopwrite_append(fd, start, modem_siz_left);
	if (ret != 0) {
		pr_err("rdr:save modem left mem 3m failed\n");
		iounmap(start);
		rdr_loopwrite_close(fd);
		rdr_looprw_set_state(0);
		return -3;
	}
	iounmap(start);
	rdr_loopwrite_close(fd);
	pr_info("rdr:save modem mem,size:99M %s\n", err ? "fail" : "success");
	return err;
}

int rdr_128m_modem_share(char *timedir)
{
	void *start = NULL;
	int ret;
	char xname[RDR_FNAME_LEN] = {0};

	if (rdr_nv_get_value(RDR_NV_MODEM_SHARE) == 0)
		return 0;
	/* save modem sharemem: 0x36200000 3m */
	snprintf(xname, RDR_FNAME_LEN-1, "%s%s/modem_share.bin", OM_ROOT_PATH, timedir); /* [false alarm]: RDR_FNAME_LEN-1 As expected  */
	rdr_remove_file(xname);/*RDR_MODEM_SHARE_BIN*/
	start = ioremap(RDR_SAVE_MODEM_SHARE, RDR_MODEM_SHARE_SZ);
	if (start == NULL) {
		DUMP_LOG(0);
		pr_err("modem share mem ioremap failed\n");
		return -1;
	}

	/* save append */
	ret = rdr_append_file(xname, start, RDR_MODEM_SHARE_SZ,
							RDR_MODEM_SHARE_SZ);
	if (ret != 0) {
		pr_err("rdr:save modem share mem 3m failed\n");
		iounmap(start);
		return ret;
	}
	/*release:*/
	iounmap(start);
	pr_info("rdr:save modem share mem,size : 3M OK\n");
	return 0;
}

int rdr_128m_socp(char *timedir)
{
	int err = 0;
	if (rdr_nv_get_value(RDR_NV_SOCP) == 0)
		return 0;
	else {
#if 0
	void *start = NULL;
	int i, ret;
	int fd = -1;
	char xname[RDR_FNAME_LEN] = {0};
	rdr_looprw_set_state(1);
	/* save debug socp mem: */
	snprintf(xname, sizeof(xname), "%s%s/socp.bin", OM_ROOT_PATH, timedir);
	ret = rdr_loopwrite_open(xname, &fd);
	if (ret != 0) {
		pr_err("rdr:open %s failed\n", xname);
		rdr_looprw_set_state(0);
		return -1;
	}
	for (i = 0; i < RDR_SOCP_SZ; i += SZ_4M) {
		start = ioremap(RDR_SAVE_SOCP + i, SZ_4M);
		if (start == NULL) {
			DUMP_LOG(0);
			pr_err("socp 4M in 16M mem ioremap failed, i:%d\n", i);
			err = 1;
			break;
		}

		/* save append */
		ret = rdr_loopwrite_append(fd, start, SZ_4M);
		if (ret != 0) {
			pr_err("rdr:save socp 16m failed, i:%d\n", i);
			iounmap(start);
			err = 2;
			break;
		}

		/*release:*/
		iounmap(start);
	}
	rdr_loopwrite_close(fd);
	pr_info("rdr:save socp mem,size : 16M %s\n", err ? "fail" : "success");
	rdr_looprw_set_state(0);
#endif
	}
	return err;
}

void rdr_save_128m(char *timedir)
{
	rdr_debug("enter,modem:0x%08x, left:0x%08x",
		RDR_MODEM_SZ - (RDR_MODEM_SZ % SZ_4M),
		RDR_MODEM_SZ % SZ_4M);
	rdr_debug("modem addr:0x%08x,size:%d,modemShare addr:0x%08x,size:%d,",
		RDR_SAVE_128M_START, RDR_MODEM_SZ, RDR_SAVE_MODEM_SHARE,
		RDR_MODEM_SHARE_SZ);
	rdr_debug("socp addr:0x%08x,size:%d", RDR_SAVE_SOCP, RDR_SOCP_SZ);
	rdr_debug("hifi addr:0x%08x,size:%d", RDR_SAVE_HIFI, RDR_HIFI_SZ);

	rdr_looprw_set_state(1);

	rdr_128m_modem(timedir);
	rdr_128m_modem_share(timedir);
#ifndef HIFI_WD_DEBUG
	dump_hifi_ddr(timedir);
#endif
	rdr_128m_socp(timedir);

	rdr_looprw_set_state(0); /* restore fs read/write to origin state */
}

#ifdef CONFIG_HISI_BALONG_MODEM
void rdr_save_sdram96k(char *time_dir)
{
#if 1
	void *start = NULL;
	int ret;
	char xname[RDR_FNAME_LEN] = {0};

	if (rdr_nv_get_value(RDR_NV_MODEM_SRAM) == 0)
		return;

	if (time_dir == NULL)
		return;
	pr_info("rdr:%s():enter\n", __func__);

	/* save sram96k: 0xE0800000 96k */
	snprintf(xname, RDR_FNAME_LEN-1,
		"%s%s/modem_sram.bin", OM_ROOT_PATH, time_dir);/* [false alarm]: RDR_FNAME_LEN-1 As expected  */
	rdr_remove_file(xname);/*RDR_MODEM_SRAM96K_BIN*/
	start = ioremap(RDR_SAVE_SRAM96K_START, RDR_SRAM96K_SZ);
	if (start == NULL) {
		DUMP_LOG(0);
		pr_err("save sram96k mem ioremap failed\n");
		return;
	}

	/* save append */
	ret = rdr_append_file(xname, start, RDR_SRAM96K_SZ, RDR_SRAM96K_SZ);
	if (ret != 0) {
		pr_err("rdr:save sram96k failed\n");
		iounmap(start);
		return;
	}

	/*release:*/
	iounmap(start);
	pr_info("rdr:save sram96k, size : 96k OK\n");
#else
	pr_info("rdr:modem sram maybe not powerup,dont save 96K sram.\n");
#endif
}
#endif

void rdr_save_lpm3_tcm(void)
{
	void *start = NULL;
	int ret;
	if (rdr_nv_get_value(RDR_NV_LPM3_TCM) == 0)
		return;

	pr_info("rdr:%s():enter\n", __func__);

	/* save lpm3 96k: 0x00020000 96k */
	rdr_remove_file(RDR_LPM3_SRAM96K_BIN);
	start = ioremap(RDR_LPM3_SRAM96K_START, RDR_SRAM96K_SZ);
	if (start == NULL) {
		DUMP_LOG(0);
		pr_err("save lpm3 sram96k mem ioremap failed\n");
		return;
	}

	/* save append */
	ret = rdr_append_file(RDR_LPM3_SRAM96K_BIN, start, RDR_SRAM96K_SZ,
							RDR_SRAM96K_SZ);
	if (ret != 0) {
		pr_err("rdr:save lpm3 sram96k failed\n");
		iounmap(start);
		return;
	}

	/*release:*/
	iounmap(start);
	pr_info("rdr:save lpm3 sram96k, size : 96k OK\n");
}

static void dump_global_info_init(void)
{
	struct rdr_top_head_s *g = &pbb->top_head;
	u8 build_datetime[RDR_BUILD_DATE_TIME_LEN];
	size_t min;
	g->magic = FILE_MAGIC;
	g->version = RDR_VERSION;

	strncpy((char *)g->product_name, RDR_PRODUCT, sizeof(g->product_name));

	min = strlen((char *)RDR_PRODUCT_VERSION) + 1;
	if (min > sizeof(g->product_version))
		min = sizeof(g->product_version) - 1;
	memset((char *)g->product_version, 0, sizeof(g->product_version));
	memcpy((char *)g->product_version, (char *)RDR_PRODUCT_VERSION, min);

	get_builddatetime(build_datetime);
	memcpy((char *)g->build_time, (char *)build_datetime,
		sizeof(g->build_time));

	pbb->top_head.except_core = 0;
	pbb->top_head.except_reason = HISI_RDR_MOD_EXCE_UNDEF;

	field_set(u32, fr.reboot_time, 0);

	g_rdr_global->app.start_flag = DUMP_START_CRASH;
	g_rdr_global->app.save_flag = 0;
	g_rdr_global->app.in_interrupt = 0;
	g_rdr_global->app.task_switch_start = 0;
	g_rdr_global->app.int_switch_start = 0;
	g_rdr_base->current_task = 0;

	return;
}

asmlinkage void rdr_syscall_hook(int number)
{
	if (atomic_read(&syscall_hook_on)) {
		u32 qbuf[3];
		struct hisi_ringbuffer_s *q =
			field_addr(struct hisi_ringbuffer_s, fr.syscall_queue);

		queue_time_in(qbuf);
		qbuf[2] = (u32)number;
		hisi_ringbuffer_write(q, qbuf);
	}

	return;
}

static void dump_queue_init(void)
{
	/*the queue init for task switch info:*/
#ifdef CONFIG_SMP
	hisi_ringbuffer_init(field_addr(struct hisi_ringbuffer_s,
		fr.task_switch_queue), rdr_field_size(fr.task_switch_queue), 5);
#else
	hisi_ringbuffer_init(field_addr(struct hisi_ringbuffer_s,
		fr.task_switch_queue), rdr_field_size(fr.task_switch_queue), 3);
#endif
	/*the queue init for interrupt switch information:*/
	hisi_ringbuffer_init(field_addr(struct hisi_ringbuffer_s,
		fr.int_queue), rdr_field_size(fr.int_queue), 3);
#if 0  //delete by xiehongliang for seattle.
	/*init the interrupts list queue:*/
	hisi_ringbuffer_init(field_addr(struct hisi_ringbuffer_s,
		fr.int_list_queue),
		rdr_field_size(fr.int_list_queue), 5);
#endif
	/*the queue init for all task info:*/
	hisi_ringbuffer_init(field_addr(struct hisi_ringbuffer_s,
		fr.alltask_queue),
		rdr_field_size(fr.alltask_queue), 4);

	/*the queue init for suspend or resume info:*/
	hisi_ringbuffer_init(field_addr(struct hisi_ringbuffer_s,
		fr.suspend_resume_queue),
		rdr_field_size(fr.suspend_resume_queue), 5);

	hisi_ringbuffer_init(field_addr(struct hisi_ringbuffer_s,
		fr.syscall_queue),
		rdr_field_size(fr.syscall_queue), 3);

	hisi_ringbuffer_init(
		field_addr(struct hisi_ringbuffer_s, fr.buddy_fail),
		rdr_field_size(fr.buddy_fail), RDR_DUMP_STACK_DEEP + 5);

#ifdef CONFIG_CPU_FREQ
	hisi_ringbuffer_init(
		field_addr(struct hisi_ringbuffer_s, fr.cpu_on_off),
		rdr_field_size(fr.cpu_on_off), 5);
#endif

#ifdef CONFIG_DETECT_HUNG_TASK
	hisi_ringbuffer_init(field_addr(struct hisi_ringbuffer_s, fr.hung_task),
		rdr_field_size(fr.hung_task), RDR_DUMP_STACK_DEEP + 5);
#endif

	hisi_ringbuffer_init(field_addr(struct hisi_ringbuffer_s, fr.warn_on),
		rdr_field_size(fr.warn_on), RDR_DUMP_STACK_DEEP + 5);

	hisi_ringbuffer_init(
		field_addr(struct hisi_ringbuffer_s, fr.task_stack),
		rdr_field_size(fr.task_stack), RDR_DUMP_STACK_DEEP + 5);

	hisi_ringbuffer_init(
		field_addr(struct hisi_ringbuffer_s, fr.exc_task_stack),
		rdr_field_size(fr.exc_task_stack), RDR_DUMP_STACK_DEEP + 0);

	return;
}

void get_time_stamp(char *timestamp_buf, unsigned int len)
{
	struct timeval tv;
	struct timespec now;
	struct rtc_time tm;

	if(NULL == timestamp_buf) {
	   return;
	}
	memset(&tv, 0, sizeof(struct timeval));
	memset(&tm, 0, sizeof(struct rtc_time));
	if (0/*inquiry_rtc_init_ok()*/) {
		now = current_kernel_time();
		rtc_time_to_tm(now.tv_sec, &tm);
	} else {
		do_gettimeofday(&tv);
		tv.tv_sec -= sys_tz.tz_minuteswest * 60;
		rtc_time_to_tm(tv.tv_sec, &tm);
	}
	snprintf(timestamp_buf, len, "%04d%02d%02d%02d%02d%02d",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);
}

/* Not use this function : */
void rdr_archive_log(void)
{
	char cmd[ARCH_NAME_MAX];
	char time_buf[RDR_TIME_LEN];
	int ret = 0;

	get_time_stamp(time_buf, RDR_TIME_LEN);
	snprintf(cmd, ARCH_NAME_MAX, "%s%s%s%s%s%s",
			 "archive -i ", OM_ROOT_PATH,
			 " -o ", time_buf, "_panic", " -z zip");

	ret = log_to_exception("kernel_panic", cmd);
	if(ret < 0 ){
		pr_err("rdr:logexception sysfs err.\n");
	}
}

/*20110101105748_00085.128125*/
int rdr_is_digital(char *string, size_t len)
{
	int i = 0;
	char* str = string;

	if (str == NULL)
		return 0;
	if (*str == '\0')
		return 0;
	while ((*str) && (i < len)) {
		if ((*str < '0') || (*str > '9')) {
			if (*str != '.')
				return 0;
		}
		i++;
		str++;
	}
	if (i == len)
		return 1;
	else
		return 0;
}

int rdr_set_xtime(struct rdr_struct_s *bb)
{
	char *datetime, *uptime;
	u32 *m = field_addr(u32, fr.reboot_time);

	datetime = bb->top_head.top_resv.con.datetime;
	uptime = bb->top_head.top_resv.con.uptime;

	memset(datetime, 0, RDR_TIME_LEN);
	memset(uptime, 0, RDR_TIME_LEN);

	get_time_stamp(datetime, RDR_TIME_LEN);
	rdr_parse_tick(*m, *(m + 1), uptime, RDR_TIME_LEN);

	pr_info("rdr:exception datetime:%s, uptime:%s\n", datetime, uptime);

	return 0;
}

int rdr_get_xtime(struct rdr_struct_s *bb, char *out_buf, size_t len)
{
	char *datetime, *uptime;

	datetime = bb->top_head.top_resv.con.datetime;
	uptime = bb->top_head.top_resv.con.uptime;

	if ((rdr_is_digital(datetime, strlen("20110101105748")) == 1) &&
		(rdr_is_digital(uptime, strlen("00085.128125")) == 1))
		snprintf(out_buf, len, "%s_%s", datetime, uptime);
	else {
		char clock[RDR_TIME_LEN];
		get_time_stamp(clock, RDR_TIME_LEN);
		snprintf(out_buf, len, "%s_xxxxx.xxxxxx", clock);
	}

	return 0;
}

int rdr_dfx_need_save(char *buf, int len)
{
	struct rdr_top_head_s *r_tophead;
	int i;
	char *str = "rdr:warn:dfxMagic inval,bit not 0,save!Data maybe inval\n";

	for (i = 0; i < len; i++) {
		if (buf[i] != 0) {
			r_tophead = &(((struct rdr_struct_s *)buf)->top_head);
			if (r_tophead->magic != FILE_MAGIC)
				pr_info("%s", str);
			return 1;
		}
	}

	return 0;
}

#define DFX_PART_NAME "dfx"

int rdr_save_pbb_to_p22(void)
{
	int ret, fd_p22;
	char buf[32] = {0};
	char *p22_path = "/dev/block/";
	char p22_name[BDEVNAME_SIZE + 12]; /*NOTE:"/dev/block/" length is 12 */
	mm_segment_t oldfs;

	if (rdr_nv_get_value(RDR_NV_RDR) == 0) {
		pr_info("rdr:nv:not save rdr 8m to p22(dfx), return.\n");
		return 0;
	}

	pr_info("rdr:%s() entered ...\n", __func__);

	/* get dfx partition number */
	ret = get_mmcpart_by_name(DFX_PART_NAME, buf); /* mmcblk0p22 */
	if (ret != 0) {
		pr_err("rdr:cant get dfx part devname,check Bordconfig.mk\n");
		return -1;
	}
	memset(p22_name, 0, sizeof(p22_name));
	strncpy(p22_name, p22_path, sizeof(p22_name));
	strncat(p22_name, buf, sizeof(p22_name) - strlen(p22_path));

	/* open /dev/block/mmcblk0p22 for read */
	oldfs = get_fs();
	set_fs(KERNEL_DS);

	fd_p22 = sys_open(p22_name, O_WRONLY, 0);
	if (fd_p22 < 0) {
		pr_err("rdr:open p22 partition failed [%s][%d]\n",
				p22_name, fd_p22);
		ret = fd_p22;
		goto out;
	}

	/* loop read p22 to filename */
	ret = sys_write((unsigned int)fd_p22, (const char *)pbb,
				sizeof(struct rdr_struct_s));
	if (ret != sizeof(struct rdr_struct_s)) {
		pr_err("rdr:write %s failed ...[%x]\n", p22_name, ret);
		sys_close(fd_p22);
		goto out;
	}
	sys_close(fd_p22);

	pr_info("rdr:write %s successed  ...\n", p22_name);
out:
	set_fs(oldfs);
	return ret;
}

int rdr_save_dfx(char *timedir)
{
	int ret, fd_p22, fd_dfx, i, bytes, cnt;
	int bufsize = SZ_16K;
	char *buf;
	char *p22_path = "/dev/block/";
	char p22_name[BDEVNAME_SIZE + 12]; /*NOTE:"/dev/block/" length is 12 */

	char fn[RDR_FNAME_LEN] = {0};

	pr_info("rdr:%s() entered ...\n", __func__);
	buf = kmalloc(bufsize, GFP_KERNEL);
	if (buf == NULL) {
		pr_err("rdr:%s():alloc buf failed\n", __func__);
		return -1;
	}
	ret = wait_for_fs_ready();
	if (ret > 0)
		pr_err("rdr:fs is not ready\n");
	else
		pr_info("rdr:fs ready\n");
	/* get dfx partition number */
	ret = get_mmcpart_by_name(DFX_PART_NAME, buf); /* mmcblk0p22 */
	if (ret != 0) {
		pr_err("rdr:cant get dfx part devname,check Bordconfig.mk\n");
		kfree(buf);
		return -1;
	}
	memset(p22_name, 0, sizeof(p22_name));
	strncpy(p22_name, p22_path, sizeof(p22_name));
	strncat(p22_name, buf, sizeof(p22_name) - strlen(p22_path));

	/* fs enter read/write state */
	rdr_looprw_set_state(1);

	/* open /dev/block/mmcblk0p22 for read */
	ret = rdr_loopread_open(p22_name, &fd_p22);
	if (ret < 0) {
		pr_err("rdr:open p22 partition failed, ret:%d\n", ret);
		goto out;
	}

	/* need save ? */
	bytes = rdr_loopread_read(fd_p22, buf, bufsize);
	if (bytes < bufsize) {
		pr_err("rdr:p22 partition read failed, ret:%d\n", bytes);
		rdr_loopread_close(fd_p22);
		ret = -1;
		goto out;
	}
	if (0 == rdr_dfx_need_save(buf, bytes)) {
		pr_info("rdr:dont save dfx partition!\n");
		rdr_loopread_close(fd_p22);
		ret = 0;
		goto out;
	}
	/* set file name */
	memset(fn, 0, sizeof(fn));
	snprintf(fn, sizeof(fn), "%s%s/", OM_ROOT_PATH, timedir);
	rdr_create_dir(fn);
	snprintf(fn, sizeof(fn), "%s%s/dfx.bin", OM_ROOT_PATH, timedir);

	/* delete old file for write */
	/*
	fd_dfx = -1;
	fd_dfx = rdr_open_dir(OM_ROOT_PATH);
	if (fd_dfx < 0) {
		rdr_loopread_close(fd_p22);
		goto out;
	}
	ret = del_old_file(OM_ROOT_PATH, RDR_DFX_BIN, (unsigned int)fd_dfx, RDR_DFX_BIN);
	if (0 != ret) {
		rdr_file_close(fd_dfx);
		rdr_loopread_close(fd_p22);
		goto out;
	}
	rdr_file_close(fd_dfx);
	*/

	/* open fn for write */
	fd_dfx = -1;
	ret = rdr_loopwrite_open(fn, &fd_dfx);
	if ((ret < 0) || (fd_dfx == -1)) {
		pr_err("rdr:open %s for write failed, ret:%d\n", fn, ret);
		rdr_loopread_close(fd_p22);
		goto out;
	}

	/* loop read p22 to fn */
	ret = rdr_loopwrite_append(fd_dfx, buf, bytes);
	if (ret == -1) {
		pr_err("rdr:write %s failed ...\n", fn);
		goto close;
	}

	cnt = (RDR_PHY_SIZE - bytes) / bufsize;
	for (i = 0; i < cnt; i++) {
		bytes = rdr_loopread_read(fd_p22, buf, bufsize);
		if (bytes < bufsize) {
			pr_err("rdr:p22 read failed, ret:%d\n", bytes);
			ret = -2;
			goto close;
		}
		ret = rdr_loopwrite_append(fd_dfx, buf, bytes);
		if (ret == -1) {
			pr_err("rdr:write %s failed\n", fn);
			goto close;
		}
	}
	pr_info("rdr:save dfx file %s success.\n", fn);

	/* erase p22 magic: */
	rdr_loopread_close(fd_p22);/*need close p22 first, then reopen.*/
	fd_p22 = sys_open(p22_name, O_RDWR, 0755);/* open file */
	if (fd_p22 < 0) {
		pr_err("rdr:open p22 for erase failed, ret:%d\n", fd_p22);
		rdr_loopread_close(fd_dfx);
		ret = -1;
		goto out;
	}
	memset(buf, 0, bufsize);
	ret = rdr_loopwrite_append(fd_p22, buf, bufsize);
	if (ret == -1) {
		pr_err("rdr:erase %s failed ...\n", p22_name);
		goto close;
	}
	pr_info("rdr:erase dfx file %s success.\n", p22_name);
	ret = 0;
close:
	rdr_loopread_close(fd_p22);
	rdr_loopwrite_close(fd_dfx);
out:
	/* fs EXIT read/write state */
	rdr_looprw_set_state(0);
	kfree(buf);

	return ret;
}

void rdr_reset_system_error(void)
{
	unsigned long lock_flags;
	g_rdr_global->app.save_file_flag = DUMP_SAVE_FILE_END;
	/*init phase1: init config and global_info */

	g_rdr_global->app.init_flag = DUMP_INIT_FLAG_PHASE1;
	g_rdr_global->app.save_file_flag = DUMP_SAVE_FILE_NORMAL;
	g_rdr_global->app.start_wait_flag = DUMP_INIT_FLAG_WAIT;
	g_rdr_global->comm.start_wait_flag = DUMP_INIT_FLAG_WAIT;

	dump_global_info_init(); /* fill global info */
	g_rdr_base->vec = DUMP_ARM_VEC_UNKNOW;
	dump_queue_init();

	rdr_register_hook_func();

	g_rdr_global->app.init_flag = DUMP_INIT_FLAG;
	memset(pbb->top_head.top_resv.con.datetime, 0, RDR_TIME_LEN);
	memset(pbb->top_head.top_resv.con.uptime, 0, RDR_TIME_LEN);
	memset(RDR_AREA_RESERVE_ADDR->ap_cp_share.content.resetlog,
		0, sizeof(RDR_AREA_RESERVE_ADDR->ap_cp_share.content.resetlog));

	rdr_rm_over3_file(OM_ROOT_PATH);
	spin_lock_irqsave(&rdr_v_dump_enter_lock, lock_flags);
	rdr_v_dump_enter = 0;
	spin_unlock_irqrestore(&rdr_v_dump_enter_lock, lock_flags);
}

extern void top_tmc_enable(void);
int rdr_reset_cp_only(void)
{
#ifdef CONFIG_HISI_BALONG_MODEM
	top_tmc_enable();
	return bsp_modem_reset();
#else
	pr_info("rdr:have no modem config,dont reset system.\n");
	return -1;
#endif
}

void rdr_save_bin_addr(void)
{
	struct rdr_a1_reserve_s *a1 = RDR_AREA_RESERVE_ADDR;
	u32 *addr = (u32 *)(a1->cp_reserve.content.rdr_bin_address);

	addr[0] = RDR_SAVE_SRAM96K_START;
	addr[1] = RDR_SAVE_128M_START;
	addr[2] = RDR_SAVE_MODEM_SHARE;
	addr[3] = RDR_SAVE_SOCP;
	addr[4] = RDR_SAVE_HIFI;
	addr[5] = HISI_RESERVED_DEBUG_PHYMEM_BASE;
}

#ifdef CONFIG_HISI_BALONG_MODEM
/* clean cp dump memory when modem reset */
void rdr_modem_reset_dumpmem(void)
{
	u8 *pcpdumpmem = (u8 *)rdr_core_addr(pbb, RDR_CCORE);

	memset(pcpdumpmem, 0, RDR_AREA_2);
	g_rdr_global->comm.init_flag = 0;

	pr_info("rdr:%s():rdr modem reset dump memory ok", __func__);
}

int rdr_modem_reset_cb(DRV_RESET_CALLCBFUN_MOMENT stage, int userdata)
{
	g_rdr_global->comm.start_wait_flag = DUMP_INIT_FLAG_WAIT;
	return 0;
}
#endif

extern void top_tmc_disable(char *pdir);
#ifdef HIFI_WD_DEBUG
extern void sochifi_watchdog_send_event(void);
extern struct dsm_client *dsm_audio_client;
static int rdr_do_hifi_reset(int mod_id)
{
	void *start = NULL;
	unsigned int * hifi_power_status_addr = NULL;
	int i = 0;
	int ret = 0;
	u32 msg = 0xdead;
	char timedir[32];

	if (!dsm_client_ocuppy(dsm_audio_client)) {
		dsm_client_record(dsm_audio_client, "DSM_SOC_HIFI_RESET\n");
		dsm_client_notify(dsm_audio_client, DSM_SOC_HIFI_RESET);
	}

	start = ioremap(HIFI_WTD_FLAG_NMI, 4);
	if (NULL == start ) {
		pr_err("HIFI_WTD_FLAG_NMI ioremap failed\n");
		return -1;
	}
	for (i = 0; i < 100; i++) {
		if (readl(start) == UCOM_HIFI_WTD_NMI_COMPLETE) {
			pr_info("hifi dsp dump ok\n");
			break;
		}
		msleep(20);
	}
	if (readl(start) != UCOM_HIFI_WTD_NMI_COMPLETE) {
		pr_info("hifi dsp dump fail\n");
		ret = -1;
	}
	iounmap(start);
	rdr_get_xtime(pbb, timedir, 32);
	rdr_save_hifi(mod_id, timedir);

	pr_info("rdr:hifi excep,nofify lpm3 pwr down dsp\n");
	hifi_power_status_addr = ioremap(DRV_DSP_POWER_STATUS_ADDR, 0x4);
	if (NULL == hifi_power_status_addr) {
		pr_err("DRV_DSP_POWER_STATUS_ADDR ioremap failed\n");
		return -1;
	}
	writel(DRV_DSP_POWER_OFF, hifi_power_status_addr);
	iounmap(hifi_power_status_addr);

	msg = HIFI_DIE_NOTIFY_LPM3;
	hisi_rdr_ipc_notify_lpm3(&msg, 1);

	for (i = 0; i < 100; i++) {
		if (g_rdr_global->hifi3.save_file_flag == DUMP_SAVE_SUCCESS)
			break;
		msleep(20);
	}
	if (g_rdr_global->hifi3.save_file_flag != DUMP_SAVE_SUCCESS) {
		pr_info("get lpm3 irq fail restart mailbox\n");
		ret = -1;
	}
	reset_set_cpu_status(1, 0);
	sochifi_watchdog_send_event();
	hifireset_runcbfun(DRV_RESET_CALLCBFUN_RESET_AFTER);
	g_rdr_global->hifi3.save_file_flag = DUMP_INIT_FLAG_WAIT;
	return ret;
}
#endif

static int rdr_write_thread(void *arg)
{
	char *ecore = NULL;
	char timedir[32];

	pr_info("rdr:wait_for_fs_ready start\n");
	while(wait_for_fs_ready() > 0);
	pr_info("rdr:wait_for_fs_ready OK end\n");

	rdr_nv_init();
	if (rdr_nv_get_value(RDR_NV_RDR) == 0)
		goto out;

	rdr_get_xtime(rdr_tmp_pbb, timedir, sizeof(timedir));
	if (rdr_tmp_pbb != NULL) {
		if (dump_need_save(rdr_tmp_pbb)) {
			pr_info("rdr:we need save dump file ...\n");
			if (0 != rdr_save_8m(OM_ROOT_PATH, timedir, "rdx",
			(void *)(rdr_tmp_pbb), sizeof(struct rdr_struct_s)))
				pr_err("rdr:when boot, save rdx failed\n");
			else
				pr_info("rdr:when boot save rdx successed\n");
			ecore = rdr_get_ecore(rdr_tmp_pbb);
		} else {
			pr_info("rdr:we need not save dump file ...\n");
		}
	}
	rdr_save_dfx(timedir);
	rdr_upload(OM_ROOT_PATH, timedir, ecore);
out:
	rdr_rm_over3_file(OM_ROOT_PATH);
	if (rdr_tmp_pbb != NULL) {
		vfree(rdr_tmp_pbb);
		rdr_tmp_pbb = NULL;
	}

	return 0;
}

void dump_save(void)
{
	char out_buf[32];
	if (pbb == NULL)
		return;
	rdr_get_xtime(pbb, out_buf, 32);
	if (0 != rdr_save_8m(OM_ROOT_PATH, out_buf, "rdx",
		(void *)(pbb), sizeof(struct rdr_struct_s)))
		pr_err("rdr:save 8m rdx file failed,time:%s ...\n", out_buf);
}

void rdr_reboot_from_error(int mod_id)
{
	if (((mod_id & HISI_RDR_MOD_CORE_MASK) == HISI_RDR_MOD_CORE_CP)
			&& (mod_id != HISI_RDR_MOD_CP_DO_RESET)) {
		pr_info("rdr:only reset cp now ...\n");
		if (rdr_reset_cp_only() == 0) {
			pr_info("rdr:reset system_error\n");
			rdr_reset_system_error();
		} else {
			pr_info("rdr:cpResetRet -1,resetAll\n");
			rdr_reboot(mod_id);
		}
	} else if (mod_id == HISI_RDR_MOD_CP_DO_RESET) {
		pr_info("rdr:cp DORESET,reset All now\n");
		rdr_reboot(mod_id);
	} else {
		pr_info("rdr:reset global system now ...\n");
		rdr_reboot(mod_id);
	}
}

static int compare_log_number(char *path1, char *path2)
{
    int i,num1,num2;
    char file_number[NUM_LONG+1] = {0};

    if ((strlen(path1) < (NUM_STATION+NUM_LONG+1))
            || (strlen(path2) < (NUM_STATION+NUM_LONG+1))) {
        return -EINVAL;
    }

    for (i=0;i<NUM_LONG;i++) {
        file_number[i]=path1[i+NUM_STATION];
    }
    if (kstrtoint(file_number,CHANGE_BASE, &num1))
        return -EINVAL;
    for (i=0;i<NUM_LONG;i++) {
        file_number[i]=path2[i+NUM_STATION];
    }
    if (kstrtoint(file_number,CHANGE_BASE, &num2))
        return -EINVAL;
    if (num1 > num2){
        return 1;
    }else{
        return 0;
    }
}
static void rdr_getbalonglogfile(char *path, char *path1, char *path2)
{
    if(strlen(path2) == 0) {
        snprintf(path2, RDR_FNAME_LEN, "%s", path);
    }
    else if (1 == compare_log_number(path , path2)){
        snprintf(path1, RDR_FNAME_LEN, "%s", path2);
        snprintf(path2, RDR_FNAME_LEN, "%s", path);
    }
    else if(strlen(path1) == 0
        || (1 == compare_log_number(path ,path1))) {
        snprintf(path1, RDR_FNAME_LEN, "%s", path);
    }
    return;
}
static void rdr_getfilepath2apr(char *path, char *path1, char *path2)
{
    int fd = -1, nread, bpos;
    char *buf = NULL;
    char d_type;
    struct linux_dirent *d;
    mm_segment_t old_fs;
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    fd = sys_open(path, O_RDONLY, 0755);
    if (fd < 0) {
        pr_err("rdr:%s(),open %s fail\n", __func__, path);
        goto out;
    }
    buf = vmalloc(DUMP_BUFF_MAX_SIZE);
    if (buf == NULL) {
        pr_err("rdr:%s():vmalloc failed\n", __func__);
        goto out_vfree;
    }
    nread = sys_getdents(fd, (struct linux_dirent *)buf, DUMP_BUFF_MAX_SIZE);
    if (nread == -1) {
        pr_err("rdr:%s():sys_getdents failed\n", __func__);
        goto out_vfree;
    }
    else if (nread == 0) {
        goto out_vfree;
    }
    for (bpos = 0; bpos < nread;) {
        d = (struct linux_dirent *)(buf + bpos);
        d_type = *(buf + bpos + d->d_reclen - 1);
        if (d_type == DT_REG){
            rdr_getbalonglogfile(d->d_name,path1,path2);
        }
        bpos += d->d_reclen;
    }
out_vfree:
    vfree(buf);
out:
    if (fd >= 0)
        sys_close(fd);
    set_fs(old_fs);
    return ;
}
//modem reset notify apr
void rdr_reset2apr(char *path, u32 mod_id)
{
    char update_path[DUMP_BUFF_MAX_SIZE] = {0};
    char local_time[RDR_TIME_LEN] = {0};
    char APR_CP_BALONGLTE_PATH_1[RDR_FNAME_LEN]={0};
    char APR_CP_BALONGLTE_PATH_2[RDR_FNAME_LEN]={0};
    if(path == NULL) {
        pr_err("rdr:rdr_reset2apr the dump dir is null.\n");
        return;
    }

    //cp reset notify apr
    if(((mod_id & HISI_RDR_MOD_CORE_MASK) == HISI_RDR_MOD_CORE_CP)
        && mod_id != HISI_RDR_MOD_CP_3RD) {
        get_time_stamp(local_time, RDR_TIME_LEN);

        if (rdr_nv_get_value(RDR_NV_MODEM_MEM) == 0) {/*for commercial*/
            snprintf(update_path,ARCH_NAME_MAX,"archive -i %setb.bin -i %sreset.log -o %s_modemcrash -z zip",
                      path,path,local_time);
        }
        else { /*for beta*/
            rdr_getfilepath2apr(BALONGLTE_LOG_DIR_PATH, APR_CP_BALONGLTE_PATH_1, APR_CP_BALONGLTE_PATH_2);
            snprintf(update_path,ARCH_NAME_MAX,"archive -i %s -i %s -i %s -i %s -i %s -i %s%s -i %s%s -o %s_modemcrash -z zip",
                      path,APR_CP_CSHELL_PATH_1,APR_CP_CSHELL_PATH_2,APR_CP_ETS_PATH_1,APR_CP_ETS_PATH_2,BALONGLTE_LOG_DIR_PATH,APR_CP_BALONGLTE_PATH_1,BALONGLTE_LOG_DIR_PATH,APR_CP_BALONGLTE_PATH_2,local_time);
        }
        //notify logserver
        if(log_to_exception("rild", update_path) < 0)
            pr_err("rdr:%s(): log_to_exception err.\n", __func__);
        else
            pr_info("rdr:rdr_reset2apr send cp reset dump to apr success.\n");
    }

    return;
}

/*
 * save 8m/128m and other mem to fs before reboot. FS is ready now.
 * Dont upload file to server before reboot.
 */
int rdr_save_mem2fs(u32 mod_id)
{
	int ret;
	char time_dir[32];
	char xname[RDR_FNAME_LEN] = {0};
	mm_segment_t old_fs = get_fs();
	set_fs(KERNEL_DS);

	rdr_get_xtime(pbb, time_dir, 32);
	memset(xname, 0, RDR_FNAME_LEN);
	snprintf(xname, RDR_FNAME_LEN-1, "%s%s/", OM_ROOT_PATH, time_dir);
	pr_info("rdr:create dir %s\n", xname);
	rdr_create_dir(xname);
	if (rdr_nv_get_value(RDR_NV_RDR) == 1) {
		ret = rdr_save_8m(OM_ROOT_PATH, time_dir, "rdr",
				(void *)(pbb), sizeof(struct rdr_struct_s));
		if (ret != 0)
			pr_err("rdr:save 8m failed\n");
	}

	if (mod_id == HISI_RDR_MOD_AP_PRESS8S) {
		pr_info("modid is press_8s, only save 8m\n");
		set_fs(old_fs);
		return 0;
	}

#ifdef CONFIG_HISI_BALONG_MODEM
	rdr_save_sdram96k(time_dir);
#endif
	/*rdr_save_lpm3_tcm();*/
	rdr_save_resetlog(pbb, time_dir);
	if ((mod_id & HISI_RDR_MOD_CORE_MASK) == HISI_RDR_MOD_CORE_CP)
		top_tmc_disable(xname);

	rdr_save_128m(time_dir);
#ifdef HIFI_WD_DEBUG
	if (mod_id != HISI_RDR_MOD_HIFI_WD)
		rdr_save_hifi(mod_id, time_dir);
#endif

	pr_info("rdr:dump info to:%s\n", xname);
	set_fs(old_fs);

    //cp reset notify apr
    rdr_reset2apr(xname,mod_id);
	return 0;
}

void rdr_system_error_process(struct rdr_system_error_param_s *p)
{
	int ret;
	u32 mod_id = p->modid;
	u32 arg1 = p->arg1;
	u32 arg2 = p->arg2;
	char *data = p->data;
	u32 length = p->len;
	static bool reboot_later_flag = false;
	bool list_empty_flag = false;
	char timedir[32];
	char file_dir[RDR_FNAME_LEN];

	if (mod_id != HISI_RDR_MOD_CP_REBOOT)
	{
		pr_info("rdr_system_error_process start mod_id [%08x] !!\n", mod_id);
		rdr_nv_update();
		if ((mod_id & HISI_RDR_MOD_EXCE_MASK) !=
				HISI_RDR_MOD_EXCE_PANIC) {
			show_mem(0);
			show_stack((struct task_struct *)rdr_current_task,
					NULL);
		}

		if (mod_id != HISI_RDR_MOD_AP_PANIC)
			rdr_system_dump(mod_id, arg1, arg2, data, length);
#ifdef HIFI_WD_DEBUG
		if (mod_id == HISI_RDR_MOD_HIFI_WD) {
			ret = rdr_do_hifi_reset(HISI_RDR_MOD_HIFI_WD);
			if (ret != 0)
				pr_info("%s:hifi reset error!\n", __func__);
		}
#endif

		rdr_get_xtime(pbb, timedir, 32);
		memset(file_dir, 0, RDR_FNAME_LEN);
		snprintf(file_dir, sizeof(file_dir), "%s%s/", OM_ROOT_PATH, timedir);

		if (mod_id == HISI_RDR_MOD_HIFI_6402_ERR) {
			pr_info("hi6402 watchdog coming......\n");
			ret = hi6402_hifi_save_log(file_dir);
			if (ret != 0)
				pr_info("%s:hifi 6402 reset error!\n", __func__);
		}

		if (rdr_need_save_before_reboot(mod_id)) {
			/*
		TODO:
		1, if save before reboot, save code add here.
		2, some control tag should resume.
			 */
			if (mod_id == HISI_RDR_MOD_AP_PRESS8S)
				rdr_wait_for_dump(500);/* other core save ok: */
			else
				rdr_wait_for_dump(5000);/* wait for other core*/

			while (1) {
				if (atomic_read(&rdr_in_suspend))
					msleep(1000);
				else {
					atomic_set(&rdr_in_saving, 1);
					break;
				}
			}
			if (0 != rdr_wait4partition("/data/lost+found", 1)) {
				pr_info("rdr: we dump info to p22.\n");
				rdr_save_pbb_to_p22();
			} else
				rdr_save_mem2fs(mod_id);

			atomic_set(&rdr_in_saving, 0);
		}
	}
	spin_lock(&g_rdr_system_error_list_lock);
	if (list_empty(&g_rdr_system_error_list))
		list_empty_flag = true;
	else
		list_empty_flag = false;
	spin_unlock(&g_rdr_system_error_list_lock);

	ret = rdr_need_reboot(mod_id);
	if ((list_empty_flag && reboot_later_flag) || ret) {
		if (list_empty_flag || rdr_is_cp_modid(mod_id)) {
			pr_info("we need reboot now ...\n");
			rdr_reboot_from_error(mod_id);
        } else {
			pr_info("we need reboot later ...\n");
			reboot_later_flag = true;
		}
    } else {
		pr_info("we dont reboot now.\n");
		rdr_rm_over3_file(OM_ROOT_PATH);
	}

	rdr_reset_system_error();
	wake_unlock(&rdr_wake_lock);

	return;
}

static int rdr_init_thread_body(void *arg)
{
	u32 *field_idx;
	size_t min;
#ifdef CONFIG_HISI_BALONG_MODEM
	char * rdr_reset_cb_name = "RDR";
#endif
	struct list_head *cur = NULL;
	struct rdr_system_error_param_s *p = NULL;
	struct list_head *next = NULL;

	RDR_ACORE_ADDR->head.magic = RDR_ACORE_MAGIC;

	/* fill AP name field: */
	min = strlen(RDR_ACORE_NAME) + 1;
	if (min > (sizeof(RDR_ACORE_ADDR->head.name)))
		min = sizeof(RDR_ACORE_ADDR->head.name) - 1;
	memset(RDR_ACORE_ADDR->head.name, 0, sizeof(RDR_ACORE_ADDR->head.name));
	memcpy(RDR_ACORE_ADDR->head.name, RDR_ACORE_NAME, min);

	strncpy(RDR_ACORE_ADDR->head.version, RDR_ACORE_VERSION,
					sizeof(RDR_ACORE_ADDR->head.version));
	/* reset the rdr fields area to 0xEE: */
	for (field_idx = &fr.field_rec_start + 1;
				field_idx < &fr.field_rec_end; field_idx++) {
		if (*field_idx != RDR_FIELD_NOT_USED)
			memset(field_addr(void, *field_idx), 0x0,
						rdr_field_size(*field_idx));
	}

	memset(pbb->top_head.top_resv.con.datetime, 0, RDR_TIME_LEN);
	memset(pbb->top_head.top_resv.con.uptime, 0, RDR_TIME_LEN);
	memset(RDR_AREA_RESERVE_ADDR->ap_cp_share.content.resetlog,
		0, sizeof(RDR_AREA_RESERVE_ADDR->ap_cp_share.content.resetlog));

	g_rdr_global->app.save_file_flag = DUMP_SAVE_FILE_END;
	/*init phase1: init config and global_info */
	dump_config_init(); /**< get config */
	rdr_save_bin_addr();

	g_rdr_global->app.init_flag = DUMP_INIT_FLAG_PHASE1;
	g_rdr_global->app.save_file_flag = DUMP_SAVE_FILE_NORMAL;
	g_rdr_global->app.start_wait_flag = DUMP_INIT_FLAG_WAIT;
	g_rdr_global->comm.start_wait_flag = DUMP_INIT_FLAG_WAIT;
#ifdef HIFI_WD_DEBUG
	g_rdr_global->hifi3.save_file_flag = DUMP_INIT_FLAG_WAIT;
#endif
#ifdef CONFIG_HISI_BALONG_MODEM
	if (RESET_OK != bsp_reset_cb_func_register(rdr_reset_cb_name, rdr_modem_reset_cb, 0, DRV_RESET_CB_PIOR_RDR))
		pr_err("rdr: register modem reset cb fail!\n");
#endif

	dump_global_info_init(); /* fill global info */
	g_rdr_base->vec = DUMP_ARM_VEC_UNKNOW;
	dump_queue_init();

	rdr_register_hook_func();

	g_rdr_global->app.init_flag = DUMP_INIT_FLAG;

	(void)sys_add_rdr_node();
#ifdef CONFIG_HISI_RDR_TEST
	(void)sys_add_rdr_test();
	(void)sys_add_rdr_ut();
#endif

#ifdef CONFIG_PROC_FS
	rdr_show_interrupts(field_addr(char, fr.interrupts_ls),
				rdr_field_size(fr.interrupts_ls));
#endif

	atomic_set(&rdr_inited, 1);

	while (1) {
		if (down_interruptible(&rdr_sem)) {
			wake_unlock(&rdr_wake_lock);
			return -1;
		}

		rdr_debug("current except task:0x%x, comm:%s",
			rdr_current_task,
			((struct task_struct *)rdr_current_task)->comm);

		spin_lock(&g_rdr_system_error_list_lock);
		list_for_each_safe(cur, next, &g_rdr_system_error_list) {
			p = list_entry(cur,
				struct rdr_system_error_param_s, sys_err_list);
			list_del(cur);
			spin_unlock(&g_rdr_system_error_list_lock);
			rdr_system_error_process(p);
			spin_lock(&g_rdr_system_error_list_lock);
			kfree(p);
		}
		spin_unlock(&g_rdr_system_error_list_lock);
	}
	return 0;
}

#define RDR_AREA_OAM_INIT(i) ({ \
	int len = RDR_AREA_##i; \
	if (len % sizeof(u32)) { \
		pr_err("rdr:area%d,len:%d,isnot 4 bytes align!\n", i, len);\
		BUG_ON(1); \
		return -1; \
	} \
	if (len) \
		rdr_debug("area%d: phy_addr:0x%lx", i, \
			HISI_RESERVED_DEBUG_PHYMEM_BASE + offset);\
	pbb->area_info[i].offset = offset; \
	pbb->area_info[i].length = len; \
	offset += len; \
	})

int rdr_area_init(void)
{
	u32 offset = rdr_offset(struct rdr_struct_s, area);

	if ((RDR_AREA_0 + RDR_AREA_1 + RDR_AREA_2 + RDR_AREA_3 + RDR_AREA_4 +
		RDR_AREA_5 + RDR_AREA_6 + RDR_AREA_7) > sizeof(pbb->area)) {
		pr_err("RDR_AREA_x total is too big,set up it again....");
		BUG_ON(1);
		return -1;
	}

	RDR_AREA_OAM_INIT(0);
	RDR_AREA_OAM_INIT(1);
	RDR_AREA_OAM_INIT(2);
	RDR_AREA_OAM_INIT(3);
	RDR_AREA_OAM_INIT(4);
	RDR_AREA_OAM_INIT(5);
	RDR_AREA_OAM_INIT(6);
	RDR_AREA_OAM_INIT(7);
	pbb->top_head.area_number = RDR_AREA_MAX;

	return 0;
}

s32 rdr_early_init(void)
{
	static int is_ok;

	if (is_ok == 1)
		return 0;
	is_ok = 1;
	if (rdr_mem_init()) {
		DUMP_LOG(0);
		return -1;
	}

	if (rdr_area_init()) {
		DUMP_LOG(0);
		pr_err("rdr:rdr_area_init fail\n");
		return -1;
	}

	return 0;
}

#ifdef CONFIG_HISI_BALONG_MODEM
#ifdef CONFIG_IPC_DRIVER
/*Configuration macro for ccore ipc module*/
void hisi_rdr_cp2ap_ipc_handler(u32 param)
{
	if (param == 0) {
		/* CP meet exception,and notify AP to dump something synchro*/
		hisi_system_error(HISI_RDR_MOD_CP_PANIC, 0, 0, (char *)0, 0);
	}

	return;
}
#endif
#endif

void hisi_rdr_ipc_notify_cp(void)
{
#ifdef CONFIG_HISI_BALONG_MODEM
#ifdef CONFIG_IPC_DRIVER
	/*notify cp something maybe have wrong.*/
	bsp_ipc_int_send(IPC_CORE_CCORE, IPC_CCPU_SRC_ACPU_DUMP);
#endif
#endif
}

#ifdef CONFIG_HI3XXX_RPROC
/* notifiers AP when LPM3 sends msgs*/
static int hisi_rdr_lpm2ap_ipc_handler(struct notifier_block *nb,
					unsigned long len, void *msg)
{
	u32 *_msg = (u32 *)msg;

#ifdef RDR_DEBUG
	unsigned long i;
	for (i = 0; i < len; i++)
		pr_info("rdr:[from lpm3] msg[%lu] = 0x%x\n", i, _msg[i]);
#endif

	if (_msg[0] == LPM3_SAVE_OK_NOTIFY_AP)
		g_rdr_global->m3.save_flag = DUMP_SAVE_SUCCESS;
	/*hisi_system_error(RDR_MODULE_LPM3, 0, 0, (char *)msg, len);*/
#ifdef HIFI_WD_DEBUG
	if (_msg[0] == LPM3_PWROFF_HIFI_OK_NOTIFY_AP) {
		/* 0x08100301  some time this irq can not receive! */
		g_rdr_global->hifi3.save_file_flag = DUMP_SAVE_SUCCESS;
		reset_set_cpu_status(1, 0);
		pr_info("rdr:[from lpm3] msg[0] = 0x%x\n", _msg[0]);
	}
#endif
	return 0;
}

#if 0
/* notifiers AP when IOM3 sends msgs*/
static int hisi_rdr_iom2ap_ipc_handler(struct notifier_block *nb,
					unsigned long len, void *msg)
{
	u32 *_msg = (u32 *)msg;
#ifdef RDR_DEBUG
	unsigned long i;

	for (i = 0; i < len; i++)
		pr_info("rdr:[from iom3] msg[%lu] = 0x%x\n", i, _msg[i]);
#endif
	if (_msg[0] == 0) {
		/* call system error here: */
		pr_info("rdr:from iom3 msg, maybe we need call system_error\n");
		/*
		hisi_system_error(HISI_RDR_MOD_IOM3_PANIC,
			0, 0, (char *)msg, len);
		*/
	}

	return 0;
}
#endif

/* notifiers AP when HIFI sends msgs*/
static int hisi_rdr_asp2ap_ipc_handler(struct notifier_block *nb,
					unsigned long len, void *msg)
{
	u32 *_msg = (u32 *)msg;
#ifdef RDR_DEBUG
	unsigned long i;

	for (i = 0; i < len; i++)
		pr_info("rdr:[from asp] msg[%lu] = 0x%x\n", i, _msg[i]);
#endif
	if (_msg[0] == 0) {
		pr_info("rdr:from hifi msg,we should call system_error here\n");
		/*
		hisi_system_error(HISI_RDR_MOD_HIFI_PANIC,
				0, 0, (char *)msg, len);
		*/
	}

	return 0;
}

static void lpm3_ipc_callback(rproc_msg_t *ack_buffer,
				rproc_msg_len_t ack_buffer_len,
				int error, void *data)
{
	pr_debug("rdr:buff[0] = 0x%08x, len = %d, error = %d\n", (unsigned int)(ack_buffer[0]), ack_buffer_len, error);

	/* m3 dump ok */
	g_rdr_global->m3.save_flag = DUMP_SAVE_SUCCESS;
}

int hisi_rdr_ipc_notify_iom3(u32 *msg, unsigned long len)
{
	int ret = 0;
	/*debug*/
	unsigned long i;
	for (i = 0; i < len; i++)
		pr_info("rdr:[ap2iom3 notifiy] msg[%lu] = 0x%x\n", i, msg[i]);

	/*notify lpm3 something maybe have wrong.*/
	ret = RPROC_ASYNC_SEND(HISI_RPROC_IOM3, msg, len,
						ASYNC_MSG, NULL, NULL);
	return ret;
}

int hisi_rdr_ipc_notify_asp(u32 *msg, unsigned long len)
{
	int r = 0;
	unsigned long i;

	for (i = 0; i < len; i++)
		pr_info("rdr:[ap2asp notifiy] msg[%lu] = 0x%x\n", i, msg[i]);

	/*notify lpm3 something maybe have wrong.*/
	r = RPROC_ASYNC_SEND(HISI_RPROC_HIFI, msg, len, ASYNC_MSG, NULL, NULL);
	return r;
}
#endif

int hisi_rdr_ipc_notify_lpm3(u32 *msg, unsigned long len)
{
	int ret = 0;
#ifdef CONFIG_HI3XXX_RPROC
	/*debug*/
	unsigned long i;
	for (i = 0; i < len; i++)
		pr_info("rdr:[ap2lpm3 notifiy] msg[%lu] = 0x%x\n", i, msg[i]);
	/*notify lpm3 something maybe have wrong.*/
	ret = RPROC_ASYNC_SEND(HISI_RPROC_LPM3_FOR_RDR, msg, len,
					ASYNC_MSG, lpm3_ipc_callback, NULL);
#endif
	return ret;
}

void hisi_rdr_nmi_notify_lpm3(void)
{
	unsigned int value;

	value = readl(rdr_sctrl_base + SCLPM3CTRL);
	value |= (0x1<<2);
	writel(value, (rdr_sctrl_base + SCLPM3CTRL));
	value &= ~(0x1<<2);
	writel(value, (rdr_sctrl_base + SCLPM3CTRL));

	return;
}

void hisi_rdr_nmi_notify_iom3(void)
{
	unsigned int value;
	value = readl(rdr_crgperi_base + CRGPERIPHIOM3CTRL);
	value |= (0x1 << 4); /* need fixup */
	writel(value, (rdr_crgperi_base + CRGPERIPHIOM3CTRL));
	value &= ~(0x1 << 4);
	writel(value, (rdr_crgperi_base + CRGPERIPHIOM3CTRL));

	return;
}

void hisi_rdr_nmi_notify_cp(void)
{
	unsigned int value;
	value = readl(rdr_crgperi_base + CRGPERIPHPERCTRL3);
	value |= (0x1 << 12); /* need fixup */
	writel(value, (rdr_crgperi_base + CRGPERIPHPERCTRL3));
	value &= ~(0x1 << 12);
	writel(value, (rdr_crgperi_base + CRGPERIPHPERCTRL3));

	return;
}

enum {
	RDR_IPC_LPM3,
	RDR_IPC_IOM3,
	RDR_IPC_HIFI,
	/* MCPU, */
	RDR_IPC_RPROC_MAX
}; /*follow the change of  hisi_rproc module*/
static struct notifier_block rdr_ipc_nb[RDR_IPC_RPROC_MAX];

/* regist ipc interrupt,so that other process cores' exception can notify us. */
static int hisi_rdr_all_ipc_reg(void)
{
	int ret = 0;

#ifdef CONFIG_HI3XXX_RPROC
	rdr_debug("cp ipc interface can be called");

	/*ap handler the ipc msg form other slave core exclude cp.*/

	rdr_ipc_nb[RDR_IPC_LPM3].next = NULL;
	rdr_ipc_nb[RDR_IPC_LPM3].notifier_call = hisi_rdr_lpm2ap_ipc_handler;
	ret |= RPROC_MONITOR_REGISTER(HISI_RPROC_LPM3_FOR_RDR,
					&rdr_ipc_nb[RDR_IPC_LPM3]);
#if 0
	rdr_ipc_nb[RDR_IPC_IOM3].next = NULL;
	rdr_ipc_nb[RDR_IPC_IOM3].notifier_call = hisi_rdr_iom2ap_ipc_handler;
	ret |= RPROC_MONITOR_REGISTER(HISI_RPROC_IOM3,
					&rdr_ipc_nb[RDR_IPC_IOM3]);
#endif/*we don't need IPC notify form IOM3 temp*/

	rdr_ipc_nb[RDR_IPC_HIFI].next = NULL;
	rdr_ipc_nb[RDR_IPC_HIFI].notifier_call = hisi_rdr_asp2ap_ipc_handler;
	ret |= RPROC_MONITOR_REGISTER(HISI_RPROC_HIFI,
					&rdr_ipc_nb[RDR_IPC_HIFI]);
	if (ret)
		pr_info("rdr:RPROC_MONITOR_REGISTER failed");

#endif

#ifdef CONFIG_HISI_BALONG_MODEM
#ifdef CONFIG_IPC_DRIVER
	/* Configuration macro for ccore ipc module ,
	this solution depend on ccore ipc was compiled firstly.*/
	rdr_debug("cp ipc interface can be called");

	/* resigter CP->AP IPC notify: */
	ret = bsp_ipc_int_connect(IPC_ACPU_SRC_CCPU_DUMP,
			(rdr_funcptr_1)hisi_rdr_cp2ap_ipc_handler, 0);
	if (0 != ret) {
		pr_err("rdr:bsp_ipc_int_connect failed");
		return -1;
	}

	ret = bsp_ipc_int_enable(IPC_ACPU_SRC_CCPU_DUMP);
	if (0 != ret) {
		pr_err("rdr:bsp_ipc_int_enable failed");
		return -1;
	}
#endif
#endif
	return ret;
}

#ifdef CONFIG_PM
static int rdr_sr_event(struct notifier_block *this,
		unsigned long event, void *ptr) {
	switch (event) {
	case PM_POST_HIBERNATION:
	case PM_POST_SUSPEND:
		pr_info("rdr:%s: resume +\n", __func__);
		atomic_set(&rdr_in_suspend, 0);
		pr_info("rdr:%s: resume -\n", __func__);
		break;

	case PM_HIBERNATION_PREPARE:
	case PM_SUSPEND_PREPARE:
		pr_info("rdr:%s: suspend +\n", __func__);
		atomic_set(&rdr_in_suspend, 1);
		while (1) {
			if (atomic_read(&rdr_in_saving))
				msleep(1000);
			else
				break;
		}
		pr_info("rdr:%s: suspend -\n", __func__);
		break;
	default:
		return NOTIFY_DONE;
	}
	return NOTIFY_OK;
}

#endif /* CONFIG_PM */

static s32 __init rdr_init(void)
{
	if (0 != rdr_early_init()) {
		DUMP_LOG(1);
		return -1;
	}

#ifdef CONFIG_PM
	/* Register to get PM events */
	rdr_sr_nb.notifier_call = rdr_sr_event;
	rdr_sr_nb.priority = -1;
	if (register_pm_notifier(&rdr_sr_nb)) {
		printk(KERN_ERR "%s: Failed to register for PM events\n",
			__func__);
		return -1;
	}
#endif

	sema_init(&rdr_sem, 0);

	if (0 != hisi_rdr_all_ipc_reg()) {
		DUMP_LOG(1);
		return -1;
	}

	if (0 != rdr_env_init()) {
		DUMP_LOG(1);
		return -1;
	}

	/* begin new regist function */
	if (0 != rdr_record_register()) {
		DUMP_LOG(0);
		return -1;
	}
	g_rdr_base = field_addr(struct dump_base_info_s, fr.base_info);
	rdr_debug("reserve content:%u padding:%u",
		sizeof(RDR_AREA_RESERVE_ADDR->ap_cp_share.content),
		sizeof(RDR_AREA_RESERVE_ADDR->ap_cp_share.padding));
	if (sizeof(RDR_AREA_RESERVE_ADDR->ap_cp_share.content) >
		sizeof(RDR_AREA_RESERVE_ADDR->ap_cp_share.padding)) {
		pr_err("RDR_GLOBAL_INTERNAL_SIZE is overflow\n");
		BUG_ON(1);
	}
	g_rdr_global = (struct rdr_global_internal_s *)\
	(RDR_AREA_RESERVE_ADDR->ap_cp_share.content.rdr_global_internal);

    memset(RDR_AREA_RESERVE_ADDR->cp_reserve.content.rdr_cpinfo,
		0, sizeof(RDR_AREA_RESERVE_ADDR->cp_reserve.content.rdr_cpinfo));

	wake_lock_init(&rdr_wake_lock, WAKE_LOCK_SUSPEND, "rdr-wakelock");

	if (!kthread_run(rdr_init_thread_body, NULL, "rdr_init_thread")) {
		DUMP_LOG(0);
		return -1;
	}

	if (!kthread_run(rdr_write_thread, NULL, "rdr_writethread")) {
		DUMP_LOG(0);
		return -1;
	}

	pr_info("<%s()> rdr init successed!!!!\n", __func__);
	return 0;
}

static void __exit rdr_exit(void)
{
	rdr_trace_stop();
#ifdef CONFIG_PM
	/* Unregister for PM events */
	unregister_pm_notifier(&rdr_sr_nb);
#endif
#ifdef CONFIG_HISI_RDR_TEST
	sys_del_rdr_test(); /* remove sys fs nodes */
#endif
	sys_del_rdr_node();

	return;
}

module_init(rdr_init);
module_exit(rdr_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dedong <wangdedong@huawei.com>");
MODULE_DESCRIPTION("kernel run data recorder");
