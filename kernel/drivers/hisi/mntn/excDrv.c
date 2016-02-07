

/*******************************************************************************
  1 头文件包含
*******************************************************************************/
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/thread_info.h>
#include <linux/syslog.h>
#include <linux/rtc.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/notifier.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/hardirq.h>
#include <linux/reboot.h>
#include <linux/kdebug.h>
#include <linux/printk.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */

#include "soc_modem_sctrl_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "soc_baseaddr_interface.h"
#include <soc_ao_sctrl_interface.h>
#include <queue.h>
#include "excDrv.h"
#include "product_config.h"
//#include "BSP.h"
#include "../mailbox/hi6xxx_mailbox/bsp_mailbox.h"
#include <linux/delay.h>
#include <linux/mtd/hisi_nve_interface.h>
#include <linux/mtd/hisi_nve_number.h>
#include <asm/cacheflush.h>
#include <linux/of.h>
#include <linux/hisi/reset.h>
#include <linux/clk.h>
#include <soc_irqs.h>

/*liuyi++*/
#include <linux/hisi/hi6xxx_mntn.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*#define EXCH_BIN_HEAD           "Exc_"
#define EXCH_BIN_BAK            ".bak"*/
#define GIC_INT_SPI_STS         ((volatile unsigned int *)(gic_base_addr + 0x1000 + 0xd04))
#define SOC_AO_SC_BASE          ((void *)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR))
#define SOC_PERI_SC_BASE         ((void *)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR))
#define SOC_PMCTRL_BASE         ((void *)HISI_VA_ADDRESS(SOC_PMCTRL_BASE_ADDR))
#define SOC_DDRC_AXI_BASE       (g_ddrc_axi_map_addr)
#define SOC_ACPU_SC_BASE        (g_acpu_sc_map_addr)
#define SOC_MODEM_SC_BASE       (g_modem_sc_map_addr)
#define SOC_MEDIA_SC_BASE       (g_media_sc_map_addr)


struct linux_dirent {
    unsigned long   d_ino;
    unsigned long   d_off;
    unsigned short  d_reclen;
    char            d_name[1];
};

struct dir_list
{
    struct linux_dirent *dirent;
    struct list_head    list;
};

#define PID_PPID_GET(task_tcb)  get_pid_ppid_of_task(task_tcb)

/*******************************************************************************
  2 全局变量定义
*******************************************************************************/
static EH_ROOT* g_pExchRoot = NULL;
static char g_reset_log_buf[EXCH_RST_LOG_BUF_SIZE] = {0};
static unsigned int g_exc_is_ready = EXCH_NOT_READY;
static unsigned long g_wdg_dwork_pointer = 0;
unsigned int g_exch_base_addr = 0;
unsigned char* g_exch_a_core_base_addr = NULL;
unsigned char* g_exch_c_core_base_addr = NULL;
unsigned char* g_exch_m_core_base_addr = NULL;

char *g_irq_trace_name[8] = {"irq trace cpu0",
                                    "irq trace cpu1",
                                    "irq trace cpu2",
                                     "irq trace cpu3",
                                      "irq trace cpu4",
                                       "irq trace cpu5",
                                        "irq trace cpu6",
                                    "irq trace cpu7"};

char *g_task_trace_name[8] = {"task trace cpu0",
                                     "task trace cpu1",
                                     "task trace cpu2",
                                     "task trace cpu3",
                                     "task trace cpu4",
                                     "task trace cpu5",
                                     "task trace cpu6",
                                     "task trace cpu7"};
static char *g_reboot_reason[EXCH_S_MAX] = {"DIE",
                                            "PANIC",
											"NOC",
											"PMU",
											"DORESET",
											"LOGDUMP",
											"DDRC_SEC",
											"WATCHPOINT",
											"GPIO",
											"MAILBOX",
                                            "OTHERS"};
unsigned int g_irq_record_len[8];
unsigned int g_irq_record_offset[8];
unsigned int g_task_record_len[8];
unsigned int g_task_record_offset[8];

static void __iomem *g_ddrc_axi_map_addr = NULL;
static void __iomem *g_acpu_sc_map_addr = NULL;
static void __iomem *g_modem_sc_map_addr = NULL;
static void __iomem *g_media_sc_map_addr = NULL;
static void __iomem *gic_base_addr = NULL;
static void __iomem *g_acpu_watchdog_flag = NULL;

static spinlock_t spinlock;
static struct clk *g_codec_vpu_clk = NULL;
static struct clk *g_codec_ade_clk = NULL;

unsigned int     g_acpu_unlock_flag_tmp = 0;
static exch_mdm_panic_reg g_exch_mdm_panic_reg = NULL;
/*******************************************************************************
  3 函数声明
*******************************************************************************/
extern void copy_err_sys_logger(void * uaddr,size_t len);
extern void copy_err_media_logger(void * uaddr,size_t len);
extern int  task_switch_hook_add ( FUNC_VOID switchHook);
extern void int_switch_hook_add(FUNC_VOID pHookFunc);
extern void exc_hook_add(FUNC_VOID pHookFunc);
extern void task_switch_hook_delete(void);
extern void int_switch_hook_delete(void);
extern void exc_hook_delete(void);
extern int  log_buf_copy(char *dest, int idx, int len);
extern void l2x0_clean_all(void);
extern unsigned int omTimerGet(void);
extern void exch_cb_reg(void);
#ifndef _DRV_LLT_
extern void Reset_save_delayed_log(void);
extern void Reset_set_fileok_flag(void);
#endif
static unsigned int exch_slice_timer_get(void);
static unsigned int exch_slice_timer_tick_get(void);
unsigned int exch_rtc_time_get(char *time);
static struct task_struct *  exch_pid_to_tid(pid_t exc_pid);
static int  exch_init(void);
static void exch_rtc_time_to_string(struct rtc_time *tm, char *rtc_str);
static int  exch_get_reset_info(char *reason, char *core);
static int  acpu_panic_loop_notify(struct notifier_block *nb, unsigned long event, void *buf);
static int  exch_panic_notify(struct notifier_block *nb, unsigned long event, void *buf);
static int exch_die_notify(struct notifier_block *nb, unsigned long event, void *pReg);
void exch_hex_dump(unsigned char *buf, unsigned int size, unsigned char per_row);
static int  exch_dev_create(void);
static ssize_t exch_init_ready_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t exch_init_ready_store(struct device *dev, struct device_attribute *attr,
                                        const  char *buf, size_t count);
static ssize_t exch_reboot_info_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t exch_reboot_info_store(struct device *dev, struct device_attribute *attr,
                                        const  char *buf, size_t count);
static ssize_t exch_exc_file_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t exch_exc_file_store(struct device *dev, struct device_attribute *attr,
                                        const  char *buf, size_t count);

extern void exch_exc_mdm_panic_cb(void);

int systemErrorCcore(int modId, int arg1, int arg2, char *arg3, int arg3Length, unsigned int timeout);

static unsigned int get_pid_ppid_of_task(struct task_struct * ptask);

static const struct device_attribute dev_attr_init_ready_value =
            __ATTR(value, 0644, exch_init_ready_show,  exch_init_ready_store);
static const struct device_attribute dev_attr_reboot_info_value =
            __ATTR(info,  0644, exch_reboot_info_show, exch_reboot_info_store);
static const struct device_attribute dev_attr_exc_file_value =
            __ATTR(exc,  0644, exch_exc_file_show, exch_exc_file_store);

static const struct attribute *exch_dev_attrs[] = {
    &dev_attr_init_ready_value.attr,
    &dev_attr_reboot_info_value.attr,
    &dev_attr_exc_file_value.attr,
    NULL,
};

static struct class *exch_dev_class = NULL;

static const struct attribute_group exch_attr_group = {
    .attrs = (struct attribute **) exch_dev_attrs,
};

static struct notifier_block acpu_panic_loop_block = {
    .notifier_call = acpu_panic_loop_notify,
    .priority = INT_MAX,
};

static struct notifier_block exch_panic_block = {
    .notifier_call = exch_panic_notify,
    .priority = INT_MIN,
};

static struct notifier_block exch_die_block = {
    .notifier_call = exch_die_notify,
    .priority = INT_MIN,
};

/*******************************************************************************
  4 函数实现
*******************************************************************************/

void exch_save_pointer_wdg(unsigned long pvalue)
{
    g_wdg_dwork_pointer = pvalue;
}

void exch_save_slice_wdg(unsigned long pvalue)
{
    if (EXCH_READY == g_exc_is_ready
		&& g_wdg_dwork_pointer == pvalue)
    {
        g_pExchRoot->ulslice_wdg = exch_slice_timer_get();
    }
}
EXPORT_SYMBOL(exch_save_pointer_wdg);
EXPORT_SYMBOL(exch_save_slice_wdg);
/*******************************************************************************
 函 数 名: get_pid_ppid_of_task
 功能描述: Add this function only for pc-lint warning clean!!
 instead of the original definition of PID_PPID_GET, original is:
((((struct task_struct *)task_tcb)->pid & 0xffff)| \
             ((((struct task_struct *)task_tcb)->real_parent->pid & 0xffff)<< 16))
*******************************************************************************/
static unsigned int get_pid_ppid_of_task(struct task_struct * ptask)
{
    unsigned int upid = 0;
    unsigned int uppid = 0;

    if (NULL != ptask)
    {
        upid = (unsigned int)(ptask->pid & 0xffff);
        uppid = (unsigned int)((ptask->real_parent->pid & 0xffff) << 16);
    }
    return (upid | uppid);
}

/*******************************************************************************
 函 数 名: get_domain_access_status
 功能描述: 用于判断某个域的地址是否可以访问
 输入参数: eACCESS_DOMAIN_TYPE etype, 域定义
 输出参数: 无
 返 回 值: 1:可以访问；0:不能访问
*******************************************************************************/
int get_domain_access_status(eACCESS_DOMAIN_TYPE etype)
{
    int     iresult = 0;
    volatile unsigned int uRegvalue = 0;
    volatile unsigned int uRegvalue2 = 0;
    volatile unsigned int uRegvalue3 = 0;
    unsigned long modem_sctrl_base = 0;
    unsigned int pw_mtcmos_stat = 0;
    unsigned int pw_clk_stat = 0;
    unsigned int pw_iso_stat = 0;
    unsigned int pw_rst_stat = 0;
    unsigned int bbe_tcm_sel = 0;

    uRegvalue = *(volatile unsigned int *)(SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_ADDR(SOC_AO_SC_BASE));
    uRegvalue2 = *(volatile unsigned int *)SOC_AO_SCTRL_SC_PW_CTRL_ADDR(SOC_AO_SC_BASE);

    uRegvalue3 = *(volatile unsigned int *)SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_ADDR(SOC_AO_SC_BASE);

    pr_info("%s: AO_SC_840 = 0x%x, AO_SC_8CC = 0x%x, AO_SC_6f8 = 0x%x\n", __FUNCTION__, uRegvalue, uRegvalue2, uRegvalue3);

    switch (etype)
    {
    case ACCESS_DOMAIN_MODEM_SC:
        if ((uRegvalue & BIT(SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_4mcpu_START))
            && (uRegvalue2 & BIT(SOC_AO_SCTRL_SC_PW_CTRL_clk_en_modemnoc_START))
            && (uRegvalue3 & BIT(SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_30mdm_subsys_glb_START)) == 0)
        {
            iresult = 1;
        }
        break;
    case ACCESS_DOMAIN_PERI_SC:
        if ((uRegvalue & BIT(SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_0peri_START)) > 0)
        {
            iresult = 1;
        }
        break;
    case ACCESS_DOMAIN_MEDIA_SC:
        if (IS_ERR(g_codec_vpu_clk))
        {
            printk(KERN_ERR "%s: pclk_codec_vpu_src is ERROR\n", __FUNCTION__);
            break;
        }
        if ((uRegvalue & BIT(SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_0peri_START)) > 0)
        {
            iresult = 1;
        }
        break;
    case ACCESS_DOMAIN_MEDIA_NOC:
        if (IS_ERR(g_codec_ade_clk))
        {
            printk(KERN_ERR "%s: g_codec_ade_clk is ERROR\n", __FUNCTION__);
            break;
        }

        if ((uRegvalue & BIT(SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_0peri_START)) > 0)
        {
            iresult = 1;
        }
        break;
    case ACCESS_DOMAIN_BBE16_DTCM:

        /* ioremap base address */
        modem_sctrl_base = (unsigned long)ioremap(SOC_MODEM_SCTRL_BASE_ADDR, 0x1000);
        if (!modem_sctrl_base)
	{
            printk(KERN_ERR "%s: unable to ioremap modem sctrl\n", __FUNCTION__);
            break;
        }
        /* return error, when bbe16 is power down or is selected for ccpu*/
        pw_mtcmos_stat = readl((void __iomem *)SOC_MODEM_SCTRL_MDMSC_PW_MTCMOS_TIMEOUT_STAT_ADDR(modem_sctrl_base));
        pw_clk_stat = readl((void __iomem *)SOC_MODEM_SCTRL_MDMSC_PW_CLKEN_STAT_ADDR(modem_sctrl_base));
        pw_iso_stat = readl((void __iomem *)SOC_MODEM_SCTRL_MDMSC_PW_ISOEN_STAT_ADDR(modem_sctrl_base));
        pw_rst_stat = readl((void __iomem *)SOC_MODEM_SCTRL_MDMSC_PW_RSTEN_STAT_ADDR(modem_sctrl_base));
        bbe_tcm_sel = readl((void __iomem *)SOC_MODEM_SCTRL_MDMSC_BBE16_TCM_SEL_ADDR(modem_sctrl_base));
        if ((0 == (pw_mtcmos_stat & (1 << SOC_MODEM_SCTRL_MDMSC_PW_MTCMOS_TIMEOUT_STAT_pw_mtcmos_timeout_stat_dspbbe_START))) ||
            (0 == (pw_clk_stat & (1 << SOC_MODEM_SCTRL_MDMSC_PW_CLKEN_STAT_pw_clken_stat_dspbbe_START))) ||
            (1 == (pw_iso_stat & (1 << SOC_MODEM_SCTRL_MDMSC_PW_ISOEN_STAT_pw_iso_stat_dspbbe_START))) ||
            (1 == (pw_rst_stat & (1 << SOC_MODEM_SCTRL_MDMSC_PW_RSTEN_STAT_pw_rsten_stat_dspbbe_START))) ||
            (0 != bbe_tcm_sel))
        {
            printk(KERN_ERR "%s: bbe16 is power down or is selected for ccpu\n", __FUNCTION__);
        }
	 else
	 {
             iresult = 1;
	 }
	 iounmap((void __iomem *)modem_sctrl_base);
	break;
    default:
        printk(KERN_ERR "%s: undefine type %d\n", __FUNCTION__, etype);
        break;
    }

    return iresult;
}

/*******************************************************************************
 函 数 名: reg_save
 功能描述: 寄存器备份
 输入参数: ulRamAddr 备份地址信息
           ulReg     待备份寄存器
           ulSize    备份寄存器大小
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
void reg_save(unsigned int* ulRamAddr, unsigned int* ulReg, unsigned int ulSize)
{
    volatile unsigned int i = 0;
    volatile unsigned int *pulAddr = (volatile unsigned int *)ulRamAddr;
    if(NULL == pulAddr || NULL == ulReg)
    {
        return;
    }
    if((0 != ((unsigned long)ulRamAddr & 0x03)) || (0 != ((unsigned long)ulReg & 0x03)))
    {
        printk("reg_save: addr is not align\n");
        return;
    }

    ulSize = (ulSize >> 2);
    for(i = 0; i < ulSize; i++)
    {
        *pulAddr = *(volatile unsigned int *)(ulReg + i);
        pulAddr++;
    }

}

EXPORT_SYMBOL(reg_save);

/*******************************************************************************
 函 数 名: exch_pid_to_tid
 功能描述: 通过task_struct的pid信息获取tid信息
 输入参数: pid task的pid信息
 输出参数: 无
 返 回 值: EXCH_ERROR-获取失败，其他-task的pid
*******************************************************************************/
static struct task_struct * exch_pid_to_tid(pid_t exc_pid)
{
    struct task_struct *tid;
    pid_t pid_temp = 0;

    for_each_process(tid)
    {
        pid_temp = tid->pid;
        if (exc_pid == pid_temp)
        {
            return tid;
        }
    }
    return NULL;
}

/*******************************************************************************
*                               time  operations                               *
*******************************************************************************/

/*******************************************************************************
 函 数 名:exch_slice_timer_get
 功能描述:获取系统timer的相对slice值
 输入参数:无
 输出参数:无
 返 回 值:timer slice value
*******************************************************************************/
static unsigned int exch_slice_timer_get(void)
{
    return omTimerGet();
}

/*******************************************************************************
 函 数 名:exch_slice_timer_tick_get
 功能描述:获取系统timer的相对tick值
 输入参数:无
 输出参数:无
 返 回 值:timer tick value
*******************************************************************************/
static unsigned int exch_slice_timer_tick_get(void)
{
    unsigned int slice_timer_value = 0;
    unsigned long long ticks;

    slice_timer_value = exch_slice_timer_get();
    ticks = ((unsigned long long)(0xFFFFFFFF - slice_timer_value)*100) >> (15);  //32K
    return (unsigned int)ticks;
}

/*******************************************************************************
 函 数 名: exch_rtc_time_get
 功能描述: 获取系统rtc时间
 输入参数: 无
 输出参数: rtc time value
 返 回 值: EXCH_OK rtc时间获取成功，EXCH_ERROR 获取失败
*******************************************************************************/
unsigned int exch_rtc_time_get(char *time)
{
    struct rtc_time exc_tm = {0};
    unsigned int rtc_time = 0;

    /* rtc_time = ? */
    rtc_time_to_tm(rtc_time, &exc_tm);
    exch_rtc_time_to_string(&exc_tm, time);

    return EXCH_OK;
}

/*******************************************************************************
 函 数 名: exch_rtc_time_to_string
 功能描述: 将rtc时间转换为特定格式的字符串形式
 输入参数: exc_tm  rtc时间
 输出参数: rtc_str rtc时间字符串
 返 回 值: 无
*******************************************************************************/
static void exch_rtc_time_to_string(struct rtc_time *exc_tm, char *rtc_str)
{
    char exc_rtc_time[EXCH_RTC_TIME_LEN] = {0};
    int i = 0;

    snprintf(exc_rtc_time, sizeof(exc_rtc_time), "%2d%2d%2d%2d%2d%2d", exc_tm->tm_year, exc_tm->tm_mon,
        exc_tm->tm_mday, exc_tm->tm_hour, exc_tm->tm_min, exc_tm->tm_sec);

    for (i = 0; i < EXCH_RTC_TIME_LEN; i ++)
    {
        if (' ' == exc_rtc_time[i])
        {
            exc_rtc_time[i] = '0';
        }
    }

    hisi_io_memcpy(rtc_str, exc_rtc_time, EXCH_RTC_TIME_LEN);
}

/*******************************************************************************
*                               external funciton                              *
*******************************************************************************/
/*******************************************************************************
 函 数 名: exch_get_drv_addr
 功能描述: 临终遗言模块提供对外接口，用于返回DRV可使用的异常记录地址
 输入参数: NA
 输出参数: 无
 返 回 值: 用户可使用地址空间
*******************************************************************************/
void* exch_get_drv_addr(void)
{
    return EXCH_DRV_CUSTOM_INFO_ADDR;
}
/*******************************************************************************
 函 数 名: save_ddrc_info
 功能描述: 保存soc 要求的ddrc数据
 输入参数: NA
 输出参数: 无
 返 回 值: NULL
*******************************************************************************/
static void save_ddrc_info(unsigned char* uaddr, unsigned int usize)
{
    unsigned char*    local_addr = NULL;
    void __iomem *pddrc = NULL;
    int     i = 0;

    /*ddrc*/
    if (0x120 < usize)
    {
        pddrc = SOC_DDRC_AXI_BASE;
        if (pddrc != NULL)
        {
            local_addr = uaddr;
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x600;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x600);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x610;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x610);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x8280;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x8280);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x8284;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x8284);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x8288;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x8288);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x8290;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x8290);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x8294;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x8294);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x8298;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x8298);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x82a0;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x82a0);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x82a4;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x82a4);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0xc008;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0xc008);

            local_addr = local_addr + sizeof(int);
            /*地址*/
            *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x4178;
            /*数据*/
            local_addr = local_addr + sizeof(int);
            *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x4178);

            for (i = 1;i < 5;i++)
            {
                local_addr = local_addr + sizeof(int);
                /*地址*/
                *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x1000 + 0x400*i + 0x004;
                /*数据*/
                local_addr = local_addr + sizeof(int);
                *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x1000 + 0x400*i + 0x004);

                local_addr = local_addr + sizeof(int);
                /*地址*/
                *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x1000 + 0x400*i + 0x020;
                /*数据*/
                local_addr = local_addr + sizeof(int);
                *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x1000 + 0x400*i + 0x020);

                local_addr = local_addr + sizeof(int);
                /*地址*/
                *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x1000 + 0x400*i + 0x024;
                /*数据*/
                local_addr = local_addr + sizeof(int);
                *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x1000 + 0x400*i + 0x024);

                local_addr = local_addr + sizeof(int);
                /*地址*/
                *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x1000 + 0x400*i + 0x080;
                /*数据*/
                local_addr = local_addr + sizeof(int);
                *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x1000 + 0x400*i + 0x080);

                local_addr = local_addr + sizeof(int);
                /*地址*/
                *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x1000 + 0x400*i + 0x084;
                /*数据*/
                local_addr = local_addr + sizeof(int);
                *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x1000 + 0x400*i + 0x084);

                local_addr = local_addr + sizeof(int);
                /*地址*/
                *(unsigned int*)local_addr = SOC_MDDRC_AXI_BASE_ADDR + 0x1000 + 0x400*i + 0x088;
                /*数据*/
                local_addr = local_addr + sizeof(int);
                *(unsigned int*)local_addr = *(unsigned int*)(pddrc + 0x1000 + 0x400*i + 0x088);

            }

        }
        else
        {
            printk("save_ddrc_info, base addr is NULL\n");
        }
    }
    else
    {
        printk("save_ddrc_info, no space to save ddrc info!!!\n");
    }
    return;
}

/*******************************************************************************
 函 数 名: sc_reg_save
 功能描述: 系统控制寄存器备份
 输入参数: void
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
void sc_reg_save(void)
{
    /*save AO_SC*/
    hisi_io_memcpy((void *)SOC_AO_SCTRL_ADDR, SOC_AO_SC_BASE, SOC_AO_SCTRL_SIZE);

    /*save ACPU_SC*/
    if(NULL != SOC_ACPU_SC_BASE)
    {
        hisi_io_memcpy((void*)SOC_ACPU_SC_ADDR, (void *)SOC_ACPU_SC_BASE, SOC_ACPU_SC_SIZE);
    }

    if (get_domain_access_status(ACCESS_DOMAIN_PERI_SC) == 1)
    {
        /*save PERI_SC*/
        hisi_io_memcpy((void *)SOC_PERI_SCTRL_ADDR, SOC_PERI_SC_BASE, SOC_PERI_SCTRL_SIZE);
        /*save SYS NOC*/
        copy_err_sys_logger((void*)SOC_SYS_NOC_ADDR, SOC_SYS_NOC_SIZE/2);
        /*save PMCTRL*/
        hisi_io_memcpy((void *)SOC_PMCTRL_ADDR, SOC_PMCTRL_BASE, SOC_PMCTRL_SIZE);

            /*save DDRC INFO*/
        if(NULL != SOC_DDRC_AXI_BASE)
        {
            save_ddrc_info((unsigned char*)SOC_DDRC_ADDR, SOC_DDRC_SIZE);
        }
    }

    if (get_domain_access_status(ACCESS_DOMAIN_MEDIA_SC) == 1)
    {
        /*enable clk*/
        (void)clk_prepare_enable(g_codec_vpu_clk);
            /*save MEDIA_SC*/
        if(NULL != SOC_MEDIA_SC_BASE)
        {
            hisi_io_memcpy((void*)SOC_MEDIA_SC_ADDR, (void *)SOC_MEDIA_SC_BASE, SOC_MEDIA_SC_SIZE);
        }
        /*disable clk*/
        clk_disable_unprepare(g_codec_vpu_clk);
        clk_put(g_codec_vpu_clk);
    }
    if (get_domain_access_status(ACCESS_DOMAIN_MEDIA_NOC) == 1)
    {
        /*enable clk*/
        (void)clk_prepare_enable(g_codec_ade_clk);
        /*save MEDIA NOC*/
        copy_err_media_logger((void*)(SOC_SYS_NOC_ADDR +SOC_SYS_NOC_SIZE/2), SOC_SYS_NOC_SIZE/2);
        /*disable clk*/
        clk_disable_unprepare(g_codec_ade_clk);
        clk_put(g_codec_ade_clk);
    }

        /*save MODEM_SC*/
    if((NULL != SOC_MODEM_SC_BASE)
        && get_domain_access_status(ACCESS_DOMAIN_MODEM_SC) == 1)
    {
        hisi_io_memcpy((void*)SOC_MODEM_SC_ADDR, (void *)SOC_MODEM_SC_BASE, SOC_MODEM_SC_SIZE);
    }

}
EXPORT_SYMBOL(sc_reg_save);
/*******************************************************************************
 函 数 名: systemError
 功能描述: 临终遗言模块提供对外接口，用于异常信息记录及复位
 输入参数: modId 模块ID
           arg1  复位原因，与modId一起表征复位原因
           arg2  保留参数
           arg3  订制信息buffer
           arg3Length    订制信息长度
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
void systemError(int modId, int arg1, int arg2, char *arg3, int arg3Length)
{
    u32 count = num_online_cpus();
    unsigned char *taskAllName = EXCH_ALL_TASK_NAME_ADDR;
    unsigned char *taskAllTCB  = EXCH_ALL_TASK_TCB_ADDR;
    unsigned char *pulCustTaskStack = EXCH_CUSTOM_TASK_INFO_ADDR;
    unsigned char *pulCustFuncInfo  = EXCH_CUSTOM_FUNC_INFO_ADDR;
    struct queue *pqTask = (struct queue *)EXCH_CUSTOM_TASK_QUEUE_ADDR;
    struct queue *pqCb   = (struct queue *)EXCH_CUSTOM_FUNC_QUEUE_ADDR;
    struct task_struct *ptcb = NULL;
    struct task_struct *pTid = NULL;
    struct task_struct *pPid = NULL;
    pid_t* tempPid = NULL;
    exch_cb_buf_t pstCbBuf;
    unsigned int ulTcbSize = 0;
    unsigned int ulBufSize = 0;
    unsigned int ulSize = 0;
    unsigned int ulCopyLen = 0;
    unsigned int ulTaskNum = 0;
    unsigned int ulRebootCore = 0;
    unsigned int len = 0;
    unsigned long flags = 0;

    pid_t    exc_pid = 0;
    int ret = 0;
    int i = 0;

    console_verbose();

    MNTN_PRINT("systemError: enter modId[0x%x], arg1[0x%x], arg2[0x%x]\n", modId, arg1, arg2);

    MNTN_PRINT("systemError: preempt_cnt=0x%x, code = %s/%d\n",preempt_count(),current->comm, current->pid);

    hisi_io_memset(&pstCbBuf, 0, sizeof(exch_cb_buf_t));

    /* 模块尚未初始化，直接复位 */
    if (EXCH_READY != g_exc_is_ready)
    {
        printk("systemError: not ready modId[0x%x], arg1[0x%x]\n", modId, arg1);

        /*系统复位*/
        //EXC_WRITE_REG(SOC_SCtrl_SC_SYS_STAT0_ADDR(SOC_SCTRL_BASE_ADDR), 0x48698284);
        machine_restart(NULL);
        return;
    }

    if (1 == g_pExchRoot->share_value.is_rebooting)
    {
        /* A核已经进入重启状态，直接返回 */
        return;
    }


    systemErrorCcore(BSP_MODU_MNTN, 0, BSP_MNTN_CACHE_FLUSH, 0, 0, 0); /*刷新C核cache*/

    if(!in_atomic() && !irqs_disabled() && !in_irq())
    {
        sys_sync();
        show_mem(0);
    }


    /* 复位中...禁止抢占，禁中断 (防重入) */
    spin_lock_irqsave(g_pExchRoot->spinlock, flags);

    /* 释放钩子函数 */
    //task_switch_hook_delete();
    int_switch_hook_delete();
    exc_hook_delete();

    /* A核进入重启状态 */
    g_pExchRoot->share_value.is_rebooting = 1;
    *(g_pExchRoot->share_addr.is_rebooting) = 1;

    /* 记录复位原因与复位核 */
    g_pExchRoot->share_value.reboot_reason = (unsigned int)arg1;
    *(g_pExchRoot->share_addr.reboot_reason) = (unsigned int)arg1;
    g_pExchRoot->reboot_src = EXCH_REBOOT_FROM_EXC;

    ulRebootCore = *(g_pExchRoot->share_addr.reboot_core);
    if (!((EXCH_C_CORE == ulRebootCore) || (EXCH_M_CORE == ulRebootCore)))
    {
        /* A核引起复位，写复位核标志，通知MCU进行全系统复位 */
        MNTN_PRINT("systemError: A core bring reboot\n");

        g_pExchRoot->share_value.reboot_core = EXCH_A_CORE;
        *(g_pExchRoot->share_addr.reboot_core) = EXCH_A_CORE;
    }

    /* 记录当前系统基本信息，这些信息可能显示在解析工具上 */
    g_pExchRoot->dumpMagic = EXCH_SYSTEM_ERROR;
    g_pExchRoot->modId = (unsigned int)modId;
    g_pExchRoot->arg1 = arg1;
    g_pExchRoot->arg2 = arg2;
    g_pExchRoot->rebootTask = (unsigned int)PID_PPID_GET(current);
    g_pExchRoot->rebootTicks = exch_slice_timer_tick_get();
    g_pExchRoot->omTimer = exch_slice_timer_get();

    /* 获取系统RTC时间 */
    exch_rtc_time_get(g_pExchRoot->rtcTime);

    /* 保存订制信息 */
    if (NULL != arg3)
    {
        len = (arg3Length > EXCH_CUSTOM_ARG_SIZE) ? EXCH_CUSTOM_ARG_SIZE : arg3Length;
        hisi_io_memcpy((void *)g_pExchRoot->customAddr, (const void * )arg3, (size_t)len);
    }

    /*保存A核的GIC的状态寄存器*/
    reg_save((unsigned int*)EXCH_A_GIC_INT_STS_ADDR, (unsigned int*)GIC_INT_SPI_STS, 32);

    sc_reg_save();

    /*modId第24 bit为1时,arg1表示pid,打印该pid调用栈*/
    if(modId & EXCH_MODID_PIDMASK)
    {
        pPid = pid_task(find_vpid(arg1), PIDTYPE_PID);
        if(NULL != pPid)
        {
            printk("backtrace for task (pid: %d, comm: %s):\n",arg1, pPid->comm);
            show_stack(pPid, NULL);
        }
    }
    /* 非die复位记录任务栈信息，die复位在hook函数中已实现 */
    if (!(((int)EXCH_S_DIE == arg1) || ((int)EXCH_S_PANIC == arg1)))
    {
        printk("backtrace for current task:\n");
        show_stack(current, NULL);
    }

    /* 保存最后一个任务的TCB、任务名及堆栈到异常信息区 */
    for (i = 0; i < count; i++)
    {
        ptcb = (struct task_struct *)(g_pExchRoot->taskId[i]);
        if (ptcb)
        {
            hisi_io_memcpy((void *)(g_pExchRoot->taskTcbAddr + EXCH_TASK_TCB_SIZE * i),  (const void *)ptcb, (size_t )EXCH_TASK_TCB_SIZE);
            hisi_io_memcpy((void *)(g_pExchRoot->taskStackAddr + THREAD_SIZE * i), (const void *)(ptcb->stack), (size_t )THREAD_SIZE);
        }
    }

    /* 保存所有任务的任务名 */
    ulSize = (sizeof(TASK_NAME_STRU)/sizeof(unsigned int)) * sizeof(unsigned int);
    ulTcbSize = EXCH_TASK_TCB_SIZE < sizeof(struct task_struct) ?
        EXCH_TASK_TCB_SIZE : sizeof(struct task_struct);

    for_each_process(pTid)
    {
        /* 最多记录124条 */
        if(ulTaskNum >=  EXCH_RECORD_TASK_NUM_MAX)
        {
            break;
        }

        /* 保存任务PID(ppid|pid)*/
        *(unsigned int *)(taskAllName) = (unsigned int)PID_PPID_GET(pTid);

        /*记录任务名*/
        strncpy((char * )(taskAllName + sizeof(unsigned int)), pTid->comm, EXCH_TASK_NAME_PART_LEN);
        taskAllName += ulSize;

        /*记录任务TCB, 前16字节记录任务PID与任务名 */
        *(unsigned int *)(taskAllTCB) = (unsigned int)PID_PPID_GET(pTid);
        strncpy((char * )(taskAllTCB + sizeof(unsigned int)), pTid->comm, EXCH_TASK_NAME_PART_LEN);

        hisi_io_memcpy((void * )(taskAllTCB + ulSize), (const void *)pTid, (size_t)(ulTcbSize - ulSize));
        taskAllTCB += ((EXCH_TASK_TCB_SIZE/sizeof(unsigned int))*sizeof(unsigned int));

        ulTaskNum++;
    }

    /* 保存上层订制任务栈信息 */
    for (i = 0; i < EXCH_CUST_TASK_NUM_MAX; i++)
    {
        /* 通过PID获取TCB */
        tempPid = (pid_t *)&(pqTask->data[i*sizeof(pid_t)]);
        exc_pid = (pid_t)*tempPid;
        ptcb = exch_pid_to_tid(exc_pid);
        if (NULL != ptcb)
        {
            /* 前16字节记录任务PID与任务名 */
            *((unsigned int *)pulCustTaskStack) = (unsigned int)exc_pid;
            strncpy((char * )(pulCustTaskStack + sizeof(pid_t)), (const char *)(ptcb->comm), EXCH_TASK_NAME_PART_LEN);
            /* 后(8K-16)字节记录任务栈信息 */
            hisi_io_memcpy((void * )((char *)pulCustTaskStack + sizeof(TASK_NAME_STRU)),
                (const void *)(ptcb->stack), (size_t )(EXCH_8K_SIZE - sizeof(TASK_NAME_STRU)));
            pulCustTaskStack = (char *)pulCustTaskStack + EXCH_8K_SIZE;
        }
    }

    /* 保存上层订制buffer信息 */
    for (i = 0; i < EXCH_CUST_FUNC_NUM_MAX; i++)
    {

        /* 获取CB */
        unsigned long * pdata = (unsigned long *)&pqCb->data[i*sizeof(exch_CBReg)];
        exch_CBReg ulCb = (exch_CBReg)*pdata;
        if (ulCb)
        {
            ret = ((exch_CBReg)ulCb)(&pstCbBuf);
            if (ret > 0)
            {
                /* 剩余空间记录回调返回信息数据段 */
                len = EXCH_CB_NAME_SIZE + sizeof(unsigned int);
                ulCopyLen =  pstCbBuf.ulDataLen;

                if ((ulBufSize + len + ulCopyLen) > EXCH_CUSTOM_FUNC_INFO_SIZE)
                {
                    printk("systemError: buf expire\n");
                    break;
                }

                /* 前32字节记录回调指定标识信息 */
                hisi_io_memcpy((void * )pulCustFuncInfo, (void * )(pstCbBuf.aucName), EXCH_CB_NAME_SIZE);
                /* 后4字节记录回调返回信息数据段长度 */
                hisi_io_memcpy((void * )(pulCustFuncInfo + EXCH_CB_NAME_SIZE),
                    (void * )(&(pstCbBuf.ulDataLen)), sizeof(unsigned int));

                hisi_io_memcpy((void * )((char *)pulCustFuncInfo + len), pstCbBuf.pucData, ulCopyLen);
                ulCopyLen += ((16 - ulCopyLen % 16) % 16); //16 bytes alignment
                pulCustFuncInfo += len + ulCopyLen;
                ulBufSize += len + ulCopyLen;
            }
        }
    }

    /*dump backtrace, reg,stack*/
#ifndef CONFIG_ARM64
    arch_trigger_all_cpu_backtrace();
#endif
    /*flush L1 cache*/
    flush_cache_all();
    /*flush L2 cache*/
#ifndef CONFIG_ARM64
    outer_flush_all();
#endif

    if (check_himntn(HIMNTN_CLOSE_ACPU_PANIC_LOOP) == 0)
    {
       do{
	   	/*this is unuseful, only for cleaning coverity warning!*/
            if (check_himntn(HIMNTN_CLOSE_ACPU_PANIC_LOOP) == 1)
	    {
                break;
	    }
       }while(1);
    }
    /*系统复位*/
    machine_restart(NULL);
    spin_unlock_irqrestore(g_pExchRoot->spinlock, flags);
}

/*******************************************************************************
 函 数 名: DRV_EXCH_TASK_INFO_ADD
 功能描述: 临终遗言模块提供对外接口，供上层指定复位时需要记录附加任务信息
 输入参数: taskPid 指定的任务pid
 输出参数: 无
 返 回 值: 0-添加成功，其他-添加失败
*******************************************************************************/
int DRV_EXCH_TASK_INFO_ADD(int taskPid)
{
    struct queue *pqTask = NULL;
    struct task_struct *pTid = NULL;
    pid_t* pdata = NULL;
    pid_t exc_pid = (pid_t)taskPid;
    pid_t pid_temp = 0;
    int ret = 0;
    unsigned int i = 0;

    /* 入参非法 */
    if (exc_pid < 0)
    {
        printk("DRV_EXCH_TASK_INFO_ADD:Invalid param \n");
        return EXCH_ERROR;
    }

    /* 模块尚未初始化 */
    if (EXCH_READY != g_exc_is_ready)
    {
        printk("DRV_EXCH_TASK_INFO_ADD:exch module is not init exc_pid[0X%x]\n", exc_pid);
        return EXCH_ERROR;
    }

    /* 系统已进入复位流程，不再接收任务添加 */
    if (0 != g_pExchRoot->share_value.is_rebooting)
    {
        printk("DRV_EXCH_TASK_INFO_ADD:A core is rebooting [0X%x]\n", exc_pid);
        return EXCH_ERROR;
    }

    pqTask = (struct queue *)EXCH_CUSTOM_TASK_QUEUE_ADDR;

    spin_lock(&pqTask->lock);

    /* 队列尚未初始化(临终遗言模块尚未准备好) */
    if (0 == pqTask->max)
    {
        spin_unlock(&pqTask->lock);
        printk("DRV_EXCH_TASK_INFO_ADD:queue is not init exc_pid[0X%x]\n", exc_pid);
        return EXCH_ERROR;
    }

    /* 队列已满 */
    ret = _queue_leftroom(pqTask);
    if (sizeof(exc_pid) > ret)
    {
        spin_unlock(&pqTask->lock);
        printk("DRV_EXCH_TASK_INFO_ADD:queue is full exc_pid[0X%x] Q->head[%d]  Q->tail[%d] Q->maxNum[%d] ret[%d]\n",
            exc_pid, pqTask->head,  pqTask->tail, pqTask->max, ret);
        return EXCH_ERROR;
    }

    /* 遍历任务列表查找指定任务 */
    for_each_process(pTid)
    {
        if (exc_pid == pTid->pid)
        {
            i = pqTask->tail;
            while(i != pqTask->head){
                pdata = (pid_t*)&(pqTask->data[i]);
                pid_temp = (pid_t)*pdata;
                if (pid_temp == exc_pid)
                {
                    spin_unlock(&pqTask->lock);
                    printk("DRV_EXCH_TASK_INFO_ADD:the task[0X%x] is already in queue\n", exc_pid);
                    return EXCH_ERROR;
                }
                i += sizeof(exc_pid) ;
         }

            /* 任务pid入队列 */
            pdata = (pid_t*)&(pqTask->data[pqTask->head]);
            *pdata = exc_pid;
            pqTask->head = MOD_ADD(pqTask->head, sizeof(exc_pid), pqTask->max);
            spin_unlock(&pqTask->lock);
            return EXCH_OK;
        }
    }

    printk("DRV_EXCH_TASK_INFO_ADD:no such a task exc_pid[0x%x]\n", exc_pid);
    spin_unlock(&pqTask->lock);
    return EXCH_ERROR;
}

/*******************************************************************************
 函 数 名: DRV_EXCH_CUST_FUNC_REG
 功能描述: 临终遗言模块提供对外接口，供上层注册回调，用于复位时保存上层指定信息
 输入参数: cb 上层指定回调接口
 输出参数: 无
 返 回 值: 0-添加成功，其他-添加失败
*******************************************************************************/
int echc_cust_funt_func_reg(exch_CBReg cb)
{
    struct queue *pqCb = NULL;
    unsigned int i = 0;
    int ret = 0;
    unsigned long *pdata = NULL;
    exch_CBReg  tempFuc;

    /* 入参为空 */
    if (!cb)
    {
        printk("DRV_EXCH_CUST_FUNC_REG:Invalid param NULL\n");
        return EXCH_ERROR;
    }

    /* 模块尚未初始化 */
    if (EXCH_READY != g_exc_is_ready)
    {
        printk("DRV_EXCH_CUST_FUNC_REG:exch module is not init [0X%lx]\n", (unsigned long)cb);
        return EXCH_ERROR;
    }

    /* 系统已进入复位流程，不再接收回调添加 */
    if (0 != g_pExchRoot->share_value.is_rebooting)
    {
        printk("DRV_EXCH_CUST_FUNC_REG:A core is rebooting [0X%lx]\n", (unsigned long)cb);
        return EXCH_ERROR;
    }

    pqCb = (struct queue *)EXCH_CUSTOM_FUNC_QUEUE_ADDR;

    spin_lock(&pqCb->lock);

    /* 队列尚未初始化(临终遗言模块尚未准备好) */
    if (0 == pqCb->max)
    {
        spin_unlock(&pqCb->lock);
        printk("DRV_EXCH_CUST_FUNC_REG:queue is not init cb[0X%lx]\n", (unsigned long)cb);
        return EXCH_ERROR;
    }


    /* 队列已满 */
    ret = _queue_leftroom(pqCb);
    if (sizeof(void *) > ret)
    {
        spin_unlock(&pqCb->lock);
        printk("DRV_EXCH_CUST_FUNC_REG:queue is full cb[0X%lx] Q->head[%d] Q->tail[%d] Q->maxNum[%d] ret[%d]\n",
            (unsigned long)cb, pqCb->head, pqCb->tail,  pqCb->max, ret);
        return EXCH_ERROR;
    }

    /*任务已在队列中，则直接返回*/
    i = pqCb->tail;
    while(i != pqCb->head)
    {
        pdata = (unsigned long *)&(pqCb->data[i]);
        tempFuc = (exch_CBReg)*pdata;
        if (cb == tempFuc)
        {
            spin_unlock(&pqCb->lock);
            printk("DRV_EXCH_CUST_FUNC_REG:the cb[0X%lx] is already in queue\n", (unsigned long)cb);
            return EXCH_ERROR;
        }
        i += sizeof(exch_CBReg) ;
    }

    /*cb入队列*/
    pdata = (unsigned long *)&(pqCb->data[pqCb->head]);
    *pdata = (unsigned long)cb;
    pqCb->head = MOD_ADD(pqCb->head, sizeof(exch_CBReg), pqCb->max);
    spin_unlock(&pqCb->lock);
    return EXCH_OK;
}

/*******************************************************************************
 函 数 名: exchMemMalloc
 功能描述: 临终遗言模块提供对外接口，用于将为OM预留的DDR地址返回给OM模块
 输入参数: ulSize 内存分配大小
 输出参数: 无
 返 回 值: 0-无空间，其他-内存起始地址
*******************************************************************************/
void* exchMemMalloc(unsigned int ulSize)
{
    return ((ulSize > EXCH_OSA_SIZE) ? (NULL) : (EXCH_OSA_ADDR));
}

EXPORT_SYMBOL(systemError);
EXPORT_SYMBOL(DRV_EXCH_TASK_INFO_ADD);
EXPORT_SYMBOL(echc_cust_funt_func_reg);


/*******************************************************************************
*                                 hook funciton                                *
*******************************************************************************/
#ifdef CONFIG_ARM64
#define this_cpu_id() \
({ \
    unsigned int cpunum; \
    __asm__("mrs %0," "mpidr_el1" \
        : "=r" (cpunum)); \
        cpunum = (cpunum & 0xF00) ? ((cpunum & 0x0F) + 4) : (cpunum & 0x0F); \
})
#else
#define this_cpu_id() \
({ \
    unsigned int cpunum; \
    __asm__("mrc p15, 0, %0, c0, c0, 5" \
        : "=r" (cpunum)); \
        cpunum = (cpunum & 0xF00) ? ((cpunum & 0x0F) + 4) : (cpunum & 0x0F); \
})
#endif

/*******************************************************************************
 函 数 名: exchTaskSwitchHook
 功能描述: 钩子函数，用于记录任务切换信息
           格式:pid_ppid(4B)|omTimer(4B)|cpu(4B)|reserved(4B)|taskName(16B)
 输入参数: pOldTcb  旧TCB
           pNewTcb  新TCB
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
void exchTaskSwitchHook(void *pOldTcb, void *pNewTcb)
{
    struct task_struct *new_task = (struct task_struct *)pNewTcb;
    struct queue *q = NULL;
    struct task_info info;

    info.pid = new_task->pid;
    info.slice = exch_slice_timer_get();
    info.jiff = jiffies;
    info.cpu = this_cpu_id();
    hisi_io_memcpy(info.taskname, new_task->comm, TASK_COMM_LEN);

    q = (struct queue *)(EXCH_TASK_SWITCH_ADDR + g_task_record_offset[info.cpu]);

    queue_loop_in(q, (void *)&info, sizeof(struct task_info));

    g_pExchRoot->taskId[info.cpu] = pNewTcb;
    g_pExchRoot->taskTicks[info.cpu] = exch_slice_timer_tick_get();

}


/*******************************************************************************
 函 数 名: exchIntSwitchHook
 功能描述: 钩子函数，用于记录中断切换信息
           格式:vec(4B)|omTimer(4B)
 输入参数: dir     方向，0-进入，1-退出
           oldVec  旧vec
           newVec  新vec
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
void exchIntSwitchHook(unsigned int dir, unsigned int oldVec, unsigned int newVec)
{
    struct irq_info info;
    int val = (0 == dir)? newVec : EXCH_INT_EXIT;
    struct queue *q = NULL;

    info.irq = val;
    info.slice = exch_slice_timer_get();
    info.jiff = jiffies;
    info.cpu = this_cpu_id();

    q = (struct queue *)(EXCH_INT_SWITCH_ADDR + g_irq_record_offset[info.cpu]);

    queue_loop_in(q, (void *)&info, sizeof(struct irq_info));

    g_pExchRoot->intId[info.cpu] = newVec;
    g_pExchRoot->intTicks[info.cpu] = exch_slice_timer_tick_get();
}

/*******************************************************************************
 函 数 名: exchDieHook
 功能描述: 钩子函数，用于系统die处理
 输入参数: currentTaskId  当前任务ID
           vec   vec信息(错误类型)
           pReg  寄存器信息
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
void exchDieHook(void * currentTaskId, int vec, void * pReg)
{
#ifdef CONFIG_ARM64
    struct die_args *args;
    struct pt_regs *regs;

    if (!pReg) {
        return;
    }
    args = (struct die_args *)pReg;
    regs = args->regs;
    g_pExchRoot->vec = (unsigned int)vec;
    g_pExchRoot->cpsr = regs->pstate;
    if (pReg)
    {
        hisi_io_memcpy((void * )(g_pExchRoot->regSet), (const void * )(pReg),
            (size_t )(EXCH_REG_NUM*sizeof(u64)));
    }
#else
    unsigned int *pulReg = NULL;

    if (!pReg) {
        return;
    }

    pulReg = (unsigned int *)pReg;

    g_pExchRoot->vec = (unsigned int)vec;
    g_pExchRoot->cpsr = *((unsigned int *)&(pulReg[16]));

    /*拷贝寄存器信息*/
    if (pReg)
    {
        hisi_io_memcpy((void * )(g_pExchRoot->regSet), (const void * )(pReg),
            (size_t )(EXCH_REG_NUM*sizeof(unsigned int)));
    }
#endif

    /*进行全局复位*/ //后续此调用在MCU中断通知处进行
    systemError((int)BSP_MODU_MNTN, (int)EXCH_S_DIE, 0, 0, 0);
}
/*******************************************************************************
 函 数 名: acpu_panic_loop_notify
 功能描述: 钩子函数，用于系统panic处理之前死循环，使现场保留
 输入参数: nb
           event
           buf
 输出参数: 无
 返 回 值: NOTIFY_OK
*******************************************************************************/
static int acpu_panic_loop_notify(struct notifier_block *nb, unsigned long event, void *buf)
{
    if (check_himntn(HIMNTN_CLOSE_ACPU_PANIC_LOOP) == 0)
    {
       do{
	   	 /*it is unuseful, only for coverity cleaning!*/
               if (check_himntn(HIMNTN_CLOSE_ACPU_PANIC_LOOP) == 1)
	        {
                   break;
	        }
	}while(1);
    }
    return NOTIFY_OK;
}
/*******************************************************************************
 函 数 名: exch_panic_notify
 功能描述: 钩子函数，用于系统panic处理
 输入参数: nb
           event
           buf
 输出参数: 无
 返 回 值: NOTIFY_OK
*******************************************************************************/
static int exch_panic_notify(struct notifier_block *nb, unsigned long event, void *buf)
{
	int i;

	unsigned int flagOfAcpuWdtRst = 0;

	/*A watchdog reset*/
	flagOfAcpuWdtRst = (0x10100101 == readl(g_acpu_watchdog_flag)) ? 1 : 0;
	if (flagOfAcpuWdtRst)
	{
		return NOTIFY_OK;
	}

	/* Wait for up to 5 seconds for all other CPUs to do the stop interrupt sent by smp_send_stop() in panic() */
	for (i=0; i < 5*1000; i++) {
		if (num_online_cpus() == 1U) {
			break;
		}
		mdelay(1);
	}
	if (num_online_cpus() > 1U) {
		printk(KERN_ERR "%s: failed to stop other CPUs\n", __FUNCTION__);
	}

	/*进行全局复位*/
	/*请注意，arg3保持为0，因为在systemError中保存了noc和ddrc信息*/
	systemError((int)BSP_MODU_MNTN, (int)EXCH_S_PANIC, 0, 0, 0);

	return NOTIFY_OK;
}

/*******************************************************************************
 函 数 名: exch_die_notify
 功能描述: 钩子函数，用于系统die处理
 输入参数: nb
           event
           pReg
 输出参数: 无
 返 回 值: NOTIFY_OK
*******************************************************************************/
static int exch_die_notify(struct notifier_block *nb, unsigned long event, void *pReg)
{

    struct die_args *args;
    struct pt_regs *regs;

    args = (struct die_args *)pReg;

#ifdef CONFIG_ARM64
    if(args)
    {
        regs = args->regs;
        g_pExchRoot->cpsr = regs->pstate;
        hisi_io_memcpy((void * )(g_pExchRoot->regSet), (const void * )(&regs->regs[0]),
            (size_t )(EXCH_REG_NUM*sizeof(u64)));
    }
#else
    if (args)
    {
        regs = args->regs;
        g_pExchRoot->cpsr = regs->uregs[16];
        hisi_io_memcpy((void * )(g_pExchRoot->regSet), (const void * )(&regs->uregs[0]),
            (size_t )(EXCH_REG_NUM*sizeof(unsigned int)));
    }
#endif
    return NOTIFY_OK;

}

/*******************************************************************************
*                                memory funciton                               *
*******************************************************************************/

/* 三段地址连续，顺序为ACM，故A核虚拟地址与基地址相同 */
static int exch_get_a_core_base_addr(void)
{
    g_exch_a_core_base_addr = ioremap(ANDROID_DUMP_LOG_ADDR, ANDROID_DUMP_LOG_SIZE);
    printk("g_exch_a_core_base_addr: phy[0x%x], vir[%p], size[0x%x]\n", ANDROID_DUMP_LOG_ADDR,
        g_exch_a_core_base_addr, ANDROID_DUMP_LOG_SIZE);
    return EXCH_OK;
}

/* C核虚拟地址为A核虚拟地址便宜2M */
static int exch_get_c_core_base_addr(void)
{
    g_exch_c_core_base_addr = ioremap(MODEM_DUMP_LOG_ADDR, MODEM_DUMP_LOG_SIZE);
    return EXCH_OK;
}

/* M核虚拟地址为C核虚拟地址便宜2M */
static int exch_get_m_core_base_addr(void)
{
    g_exch_m_core_base_addr = ioremap(MCU_DUMP_LOG_ADDR, MCU_DUMP_LOG_SIZE);
    return EXCH_OK;
}

/*******************************************************************************
*                                entry funciton                                *
*******************************************************************************/

/*******************************************************************************
 函 数 名: exch_dev_create
 功能描述: 此处为模块创建一虚拟设备，用于实现本模块与文件系统的同步，当用于保存
           异常信息的文件系统挂接完成，会通过该设备通知内核，内核开始模块初始化
 输入参数: 无
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
static int exch_dev_create(void)
{
    struct device *dev_t;
    int ret = 0;

    exch_dev_class = class_create(THIS_MODULE, "exc_class");
    if (IS_ERR(exch_dev_class))
    {
        printk("exch_dev_create: exc class create fail\n");
        return EXCH_ERROR;
    }

    dev_t = device_create(exch_dev_class, NULL, MKDEV(0, 0),
                NULL, "exc_dev");
    if (IS_ERR(dev_t))
    {
        printk("exch_dev_create: exc device create fail\n");
        return EXCH_ERROR;
    }

    ret = sysfs_create_group(&dev_t->kobj, &exch_attr_group);
    if (ret)
    {
        printk("exch_dev_create: exc group create fail\n");
        return EXCH_ERROR;
    }
    return 0;
}

/*******************************************************************************
 函 数 名: exch_init_ready_show
 功能描述: 本接口为设备输出接口，暂未使用
 输入参数: dev  设备句柄
           attr 设备属性
           buf  输入数据
 输出参数: 无
 返 回 值: buf长度
*******************************************************************************/
static ssize_t exch_init_ready_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    /*如果模块准备好了，将1=EXCH_READY存入buf中*/
    if (EXCH_READY == g_exc_is_ready)
    {
        strncpy(buf, "1\n", sizeof("1\n"));
    }
    else
    {
        strncpy(buf, "0\n", sizeof("0\n"));
    }
    pr_info("exch_init_ready_show: [%s]\n", buf);
    return (ssize_t)strlen(buf);
}

/*******************************************************************************
 函 数 名: exch_init_ready_store
 功能描述: 本接口为设备输入接口，执行模块初始化操作，文件系统挂接完成时触发
 输入参数: dev   设备句柄
           attr  设备属性
           buf   输出数据
           count 返回大小
 输出参数: 无
 返 回 值: EXCH_READY
*******************************************************************************/
static ssize_t exch_init_ready_store
(
    struct device *dev,
    struct device_attribute *attr,
    const  char *buf,
    size_t count
)
{


    pr_info("exch_init_ready_store: %s \n", buf);

    /* 若模块尚未准备好，执行初始化 */
    if (EXCH_READY != g_exc_is_ready)
    {
        exch_init();
    }

    return (ssize_t)count;
}

/*******************************************************************************
 函 数 名: exch_exc_file_show
 功能描述: 本接口为设备输出接口，是否需要生产exc文件
 输入参数: dev  设备句柄
           attr 设备属性
           buf  输入数据
 输出参数: 无
 返 回 值: buf长度
*******************************************************************************/
static ssize_t exch_exc_file_show(struct device *dev, struct device_attribute *attr, char *buf)
{

    if (g_pExchRoot->reboot_src == EXCH_REBOOT_FROM_EXC)
    {
        strncpy(buf,"1\n", sizeof("1\n"));
    }
    else
    {
        strncpy(buf,"0\n", sizeof("0\n"));
    }

    return (ssize_t)strlen(buf);
}

/*******************************************************************************
 函 数 名: exch_exc_file_store
 功能描述: 本接口为设备输入接口，执行模块初始化操作.
 输入参数: dev   设备句柄
           attr  设备属性
           buf   输出数据
           count 返回大小
 输出参数: 无
 返 回 值: EXCH_READY
*******************************************************************************/
static ssize_t exch_exc_file_store
(
    struct device *dev,
    struct device_attribute *attr,
    const  char *buf,
    size_t count
)
{
    pr_info("exch_exc_file_store: %s \n",buf);

    g_pExchRoot->reboot_src = 0;
#ifndef _DRV_LLT_
    exch_exc_mdm_panic_cb();
    Reset_set_fileok_flag();
    Reset_save_delayed_log();
#endif
    return (ssize_t)count;
}



/*******************************************************************************
 函 数 名: exch_reboot_info_show
 功能描述: 本接口为设备输出接口，用于获取复位信息
 输入参数: dev  设备句柄
           attr 设备属性
           buf  输入数据
 输出参数: 无
 返 回 值: buf长度
*******************************************************************************/
static ssize_t exch_reboot_info_show(struct device *dev,
                        struct device_attribute *attr, char *buf)
{
    /* get reboot reason and core */
    strncat(buf, g_reset_log_buf, sizeof(g_reset_log_buf));
    return (ssize_t)strlen(buf);
}

/*******************************************************************************
 函 数 名: exch_reboot_info_store
 功能描述: 本接口为设备输入接口，暂未使用
 输入参数: dev   设备句柄
           attr  设备属性
           buf   输出数据
           count 返回大小
 输出参数: 无
 返 回 值: EXCH_OK
*******************************************************************************/
static ssize_t exch_reboot_info_store
(
    struct device *dev,
    struct device_attribute *attr,
    const  char *buf,
    size_t count
)
{
    pr_info("exch_reboot_info_store: %s \n",buf);

    return (ssize_t)count;
}

/*******************************************************************************
 函 数 名: exch_get_reset_info
 功能描述: 获取复位信息，包括复位原因与复位核
 输入参数: 无
 输出参数: reason  复位原因
           core    复位核
 返 回 值: EXCH_ERROR 获取失败，EXCH_OK 获取成功
*******************************************************************************/
static int exch_get_reset_info(char *reason, char *core)
{
    unsigned int reboot_core = 0;
    unsigned int reboot_reason = 0;

    /* 确认复位原因 */
	reboot_reason = g_pExchRoot->share_value.reboot_reason;
    if (g_pExchRoot->modId >= 0x10000000 && g_pExchRoot->modId < 0x2fffffff)
    {
        snprintf(reason, sizeof("PAM"), "PAM");
    }
	else if (EXCH_S_MAX > reboot_reason)
	{
		snprintf(reason, strlen(g_reboot_reason[reboot_reason]) + 1, g_reboot_reason[reboot_reason]);
	}
	else
	{
	    snprintf(reason, sizeof("OTHERS"), "OTHERS");
	}

    /* 确认复位核 */
    reboot_core = g_pExchRoot->share_value.reboot_core;
    switch (reboot_core)
    {
        case EXCH_A_CORE:
            snprintf(core, sizeof("A"), "A");
            break;
        case EXCH_C_CORE:
            snprintf(core, sizeof("C"), "C");
            break;
        case EXCH_M_CORE:
            snprintf(core, sizeof("M"), "M");
            break;
        default:
            snprintf(core, sizeof("?"), "?");
            break;
    }
    return EXCH_OK;
}

/*
himntn[0] --- dump low 128MB
himntn[1] --- record irq and task switch info
himntn[2] --- volumeup + volumedown + power enter fastboot
 */
static char himntn[HIMNTN_VALID_SIZE + 1] = {'\0'};
unsigned int himntn_gobal_resetlog = 0;
unsigned int himntn_modem_resetlog = 0;
unsigned int himntn_hifi_resetlog = 0;

void set_himntn(int feature, int val)
{
    if (feature >= HIMNTN_BOTTOM)
        return;
    if (val)
        himntn[feature] = '1';
    else
        himntn[feature] = '0';
}


/*
若himntn[feature]为'1'，则函数check_himntn返回1
若himntn[feature]为'0'，则函数check_himntn返回0
*/
int  check_himntn(int feature)
{
    if (feature >= HIMNTN_BOTTOM)
        return 0;

    return himntn[feature] != '0';
}

static int __init early_parse_himntn_cmdline(char *himntn_cmdline)
{
    hisi_io_memset(himntn, 0x0, HIMNTN_VALID_SIZE + 1);
    if (strlen(himntn_cmdline) != HIMNTN_VALID_SIZE)
    {
        printk(KERN_ERR "error: invalid himn cmdline size!\n");
        return -1;
    }
    hisi_io_memcpy(himntn, himntn_cmdline, strlen(himntn_cmdline));
    if (himntn[HIMNTN_GOBAL_RESETLOG] == '1') {
        himntn_gobal_resetlog = 1;
        if (himntn[HIMNTN_MODEM_RESETLOG] == '1') {
            himntn_modem_resetlog = 1;
        }
        if (himntn[HIMNTN_HIFI_RESETLOG] == '1') {
            himntn_hifi_resetlog = 1;
        }
    }
    if ('1' == himntn[HIMNTN_UNLOCK_CORE_FOR_MCUSAVEPC]) {
        g_acpu_unlock_flag_tmp = 1;
    }
    return 0;
}

early_param("himntn", early_parse_himntn_cmdline);

static irqreturn_t fake_wdg_irq_handler(int irq, void *dev)
{
    printk(KERN_ERR "fake_wdg_irq_handler\n");
    return IRQ_HANDLED;//do nothing
}

/*******************************************************************************
 函 数 名: exch_init
 功能描述: 模块初始化入口
 输入参数: 无
 输出参数: 无
 返 回 值: EXCH_ERROR 初始化失败，EXCH_OK 初始化成功
*******************************************************************************/
static int exch_init(void)
{
    unsigned int g_acpu_aostl_addr[2] = {0};  /* address and size */
    u32 count = num_online_cpus();
    struct device_node *root;
    int i = 0;
    int ret;
	
    pr_info("exch_init: enter \n");

    /* 信息处理结束，清空DDR内存中A/C/M异常信息区 */
    /* 2M异常记录区清0，除了EXCH_LOG_KMSG_ADDR区域*/
    (void)hisi_io_memset((void *)EXCH_BASE_ADDR, 0, (size_t )(EXCH_LOG_KMSG_ADDR - EXCH_BASE_ADDR));
    (void)hisi_io_memset((void *)PWRCTRL_EXC_ACPU_ADDR, 0, (size_t )(EXCH_EXC_BIN_SIZE - (PWRCTRL_EXC_ACPU_ADDR - EXCH_BASE_ADDR)));

    g_pExchRoot->spinlock = &spinlock;
    spin_lock_init(g_pExchRoot->spinlock);

    /*保留产品和版本信息*/
    hisi_io_memcpy((void * )g_pExchRoot->version, (const void * )PRODUCT_CFG_FULL_VERSION_STR, ((size_t )strlen(PRODUCT_CFG_FULL_VERSION_STR) + 1));

    g_pExchRoot->excSize = EXCH_A_CORE_SIZE;
    exch_rtc_time_get(g_pExchRoot->rtcTime);

    /* 分配异常信息区 */
    g_pExchRoot->excBaseAddr   = (unsigned char*)g_pExchRoot;
    g_pExchRoot->taskTcbAddr   = EXCH_LAST_TASK_TCB_ADDR;
    g_pExchRoot->intStackAddr  = EXCH_LAST_INT_STACK_ADDR;
    g_pExchRoot->taskStackAddr = EXCH_LAST_TASK_STACK_ADDR;
    g_pExchRoot->allTaskNameAddr   = EXCH_ALL_TASK_NAME_ADDR;
    g_pExchRoot->customAddr     = EXCH_CUSTOM_ARG_ADDR;

    g_pExchRoot->taskSwitchAddr    = EXCH_TASK_SWITCH_ADDR;
    g_pExchRoot->intSwitchAddr     = EXCH_INT_SWITCH_ADDR;

    g_pExchRoot->customTaskQueueAddr = EXCH_CUSTOM_TASK_QUEUE_ADDR;
    g_pExchRoot->customTaskInfoAddr  = EXCH_CUSTOM_TASK_INFO_ADDR;
    g_pExchRoot->customFuncQueueAddr = EXCH_CUSTOM_FUNC_QUEUE_ADDR;
    g_pExchRoot->customFuncInfoAddr  = EXCH_CUSTOM_FUNC_INFO_ADDR;

    /* 共享信息内存区分配 */
    g_pExchRoot->share_addr.reboot_reason = (unsigned int*)(EXCH_CORES_SHARE_ADDR);
    g_pExchRoot->share_addr.reboot_core   = (unsigned int*)(EXCH_CORES_SHARE_ADDR + sizeof(void*));
    g_pExchRoot->share_addr.mem_ready     = (unsigned int*)(EXCH_CORES_SHARE_ADDR + sizeof(void*)*2);
    g_pExchRoot->share_addr.is_rebooting  = (unsigned int*)(EXCH_CORES_SHARE_ADDR + sizeof(void*)*3);

    /* 异常文件保存完成，内存清零，其他核可以开始初始化临终遗言模块 */ //中断通知???
    g_pExchRoot->share_value.mem_ready = EXCH_MEM_IS_READY;
    *(volatile unsigned int *) (g_pExchRoot->share_addr.mem_ready) = EXCH_MEM_IS_READY;

    /* 生成循环队列 */
    queue_init((struct queue *)(g_pExchRoot->customTaskQueueAddr), "CustTaskQue",  (EXCH_CUST_TASK_NUM_MAX + 1) * sizeof(unsigned int));
    queue_init((struct queue *)(g_pExchRoot->customTaskInfoAddr),  "CustTaskInfo", (EXCH_CUST_TASK_NUM_MAX + 1) * sizeof(unsigned int));
    queue_init((struct queue *)(g_pExchRoot->customFuncQueueAddr), "CustFuncQue",  (EXCH_CUST_FUNC_NUM_MAX + 1) * sizeof(void*));
    queue_init((struct queue *)(g_pExchRoot->customFuncInfoAddr),  "CustFuncInfo", (EXCH_CUST_FUNC_NUM_MAX + 1) * sizeof(unsigned int));

    printk("cpu num = %d\n", count);
    {
        for (i = 0; i < count; i++)
        {
            queue_init((struct queue *)(g_pExchRoot->intSwitchAddr + g_irq_record_offset[i]), g_irq_trace_name[i], (g_irq_record_len[i]  - sizeof(struct queue)));
        printk("irq g_pExchRoot->intSwitchAddr =0x%p  g_irq_record_offset[i] = 0x%x\n", g_pExchRoot->intSwitchAddr, g_irq_record_offset[i]);
         printk("%s  irq queue name =%s\n",__func__, ((struct queue *) (g_pExchRoot->intSwitchAddr + g_irq_record_offset[i]))->name);

        printk("task g_pExchRoot->taskSwitchAddr =0x%p  g_task_record_offset[i] = 0x%x  recode = 0x%x  len = 0x%x \n", g_pExchRoot->taskSwitchAddr,
            g_task_record_offset[i], g_task_record_len[i], (unsigned int)sizeof(struct queue));
            queue_init((struct queue *)(g_pExchRoot->taskSwitchAddr + g_task_record_offset[i]), g_task_trace_name[i], (g_task_record_len[i] - sizeof(struct queue)));
        printk("%s  task queue name =%s\n",__func__, ((struct queue *) (g_pExchRoot->taskSwitchAddr + g_task_record_offset[i]))->name);
        }        task_switch_hook_add((FUNC_VOID)exchTaskSwitchHook);

        int_switch_hook_add((FUNC_VOID)exchIntSwitchHook);
    }


    exc_hook_add((FUNC_VOID) exchDieHook);

    atomic_notifier_chain_register(&panic_notifier_list, &acpu_panic_loop_block);
    atomic_notifier_chain_register(&panic_notifier_list, &exch_panic_block);
    register_die_notifier(&exch_die_block);

    g_ddrc_axi_map_addr = ioremap(SOC_MDDRC_AXI_BASE_ADDR, PAGE_ALIGN(SZ_64K));
    if (NULL == g_ddrc_axi_map_addr)
    {
        printk(KERN_ERR"exch_init,g_ddrc_axi_map_addr remap error.\n");
    }

    gic_base_addr = ioremap(REG_BASE_GIC, REG_GIC_IOSIZE);
    if (NULL == gic_base_addr)
    {
        printk(KERN_ERR"exch_init, gic_base_addr remap error.\n");
    }


    /*TODO: Add acpu_sc in dts, as base addr of ACPU_SC is different between hi6210 and hi6220*/
    root = of_find_compatible_node(NULL,NULL,"hisilicon,hi6210");
    if (!root){
        printk(KERN_ERR "[%s] find root node fail!\n",
               __func__);
    }

    if (of_property_read_u32_array(root, "mntn,acpu-scaddr",
                                g_acpu_aostl_addr, 2)) {
        printk(KERN_ERR"[%s] node doesn't have acpu-scaddr property!\n",
                __func__);
    }

    g_acpu_sc_map_addr = ioremap(g_acpu_aostl_addr[0], PAGE_ALIGN(g_acpu_aostl_addr[1]));

    if (NULL == g_acpu_sc_map_addr)
    {
        printk(KERN_ERR"exch_init,g_acpu_sc_map_addr remap error.\n");
    }
    g_modem_sc_map_addr = ioremap(SOC_MODEM_SCTRL_BASE_ADDR, PAGE_ALIGN(SZ_4K));
    if (NULL == g_modem_sc_map_addr)
    {
        printk(KERN_ERR"exch_init,g_modem_sc_map_addr remap error.\n");
    }
    g_media_sc_map_addr = (void __iomem *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_BASE_ADDR);
    if (NULL == g_media_sc_map_addr)
    {
        printk(KERN_ERR"exch_init,g_media_sc_map_addr remap error.\n");
    }

    g_acpu_watchdog_flag = ioremap(MEMORY_AXI_ACPU_WDTRST_REBOOT_FLAG_ADDR, PAGE_SIZE);
    if (NULL == g_acpu_watchdog_flag)
    {
        printk(KERN_ERR"exch_init,g_acpu_watchdog_flag remap error.\n");
    }

	/*为了规避IRQ_ACPU_SOFT_FIQ低概率以IRQ类型发出的问题*/
    ret = request_irq(IRQ_ACPU_SOFT_FIQ, fake_wdg_irq_handler, IRQF_NO_SUSPEND, "irq_watchdog", NULL);
    if (ret)
    {
        printk(KERN_ERR"exch_init, fail to request irq for IRQ_ACPU_SOFT_FIQ.\n");
    }
    /* 临终遗言模块初始化完成 */
    g_exc_is_ready = EXCH_READY;

    return EXCH_OK;

}
/*******************************************************************************
 函 数 名: exch_entry
 功能描述: 临终遗言模块入口
 输入参数: 无
 输出参数: 无
 返 回 值: EXCH_OK 初始化成功，EXCH_ERROR 初始化失败
*******************************************************************************/
static int __init exch_entry(void)
{
    char reboot_reason[16] = {0};
    char reboot_core[4] = {0};
    int ret = EXCH_OK;
    struct device_node *np = NULL;

    pr_info("exch_entry: enter\n");
    pr_info("size is:0x%x\n",(unsigned int)sizeof(EH_ROOT));

    /* 模块初始化，先获取各核DDR内存地址 */
    /*move to early init*/
    exch_dev_create();

    g_pExchRoot = (EH_ROOT*)EXCH_ROOT_ADDR;

    ret = exch_get_reset_info(reboot_reason, reboot_core);
    if (EXCH_ERROR == ret)
    {
        printk("exch_entry: reset info get fail\n");
        return EXCH_ERROR;
    }

    *(char*)(g_pExchRoot->rtcTime + EXCH_RTC_TIME_LEN - 1) = '\0';

    /* 构建reset log内容及其长度信息 */
    snprintf(g_reset_log_buf, sizeof(g_reset_log_buf), " %s\n", reboot_reason);

    printk("exch_save:reboot_reason[%s] core[%s]\n", reboot_reason, reboot_core);

    /* 正常复位或者开机，则开始模块初始化 */
    if (0 == g_pExchRoot->modId)
    {
         exch_init();
    }

    np = of_find_compatible_node(NULL, NULL, "hisilicon, hi6xxx-mntn");
    if (NULL == g_codec_vpu_clk)
    {
        g_codec_vpu_clk = of_clk_get_by_name( np, "pclk_codec_vpu_src");
        if (IS_ERR(g_codec_vpu_clk))
        {
            printk(KERN_ERR "%s: g_codec_vpu_clk is ERROR\n", __FUNCTION__);
        }
    }

    if (NULL == g_codec_ade_clk)
    {
        g_codec_ade_clk = of_clk_get_by_name( np, "clk_ade_core_src");
        if (IS_ERR(g_codec_ade_clk))
        {
            printk(KERN_ERR "%s: g_codec_ade_clk is ERROR\n", __FUNCTION__);
        }
    }

    return EXCH_OK;
}

static int __init exch_early_init(void)
{
    exch_get_a_core_base_addr();
    exch_get_c_core_base_addr();
    exch_get_m_core_base_addr();
    return EXCH_OK;
}

static int __init exch_postcore_init(void)
{
    int i, irq_offset = 0, task_offset = 0;
    u32 count = num_online_cpus();
    unsigned int task_record_ratio[8][8] = {{16, 0, 0, 0, 0, 0, 0, 0},
                                               { 8, 8, 0, 0, 0, 0, 0, 0},
                                               { 8, 4, 4, 0, 0, 0, 0, 0},
                                               { 8, 4, 2, 2, 0, 0, 0, 0},
                                               { 4, 4, 4, 2, 2, 0, 0, 0},
                                               { 4, 4, 2, 2, 2, 2, 0, 0},
                                               { 4, 4, 2, 2, 2, 1, 1, 0},
                                               { 4, 2, 2, 2, 2, 2, 1, 1}};
    unsigned int irq_record_ratio[8][8] =  {{16, 0, 0, 0, 0, 0, 0, 0},
                                               { 8, 8, 0, 0, 0, 0, 0, 0},
                                               { 8, 4, 4, 0, 0, 0, 0, 0},
                                               { 8, 4, 2, 2, 0, 0, 0, 0},
                                               { 8, 4, 2, 1, 1, 0, 0, 0},
                                               { 8, 4, 1, 1, 1, 1, 0, 0},
                                               { 6, 4, 2, 1, 1, 1, 1, 0},
                                               { 6, 4, 1, 1, 1, 1, 1, 1}};

      printk(KERN_INFO"***** %s  cpu num = %u *****\n", __func__, count);

    for (i = 0; i < count; i++)
    {
        g_irq_record_len[i] = EXCH_INT_SWITCH_SIZE / 16 * irq_record_ratio[count - 1][i];
        g_irq_record_offset[i] = irq_offset;
        irq_offset += g_irq_record_len[i];

        g_task_record_len[i] = EXCH_TASK_SWITCH_SIZE / 16 * task_record_ratio[count - 1][i];
        g_task_record_offset[i] = task_offset;
        task_offset += g_task_record_len[i];
    }
    return 0;
}

/*防止异常发生时，循环dabt破坏现场*/
void exch_stand_guard(struct task_struct *tsk)
{
	if (!himntn_gobal_resetlog)
	{
		return;
	}

	if (unlikely((unsigned long)tsk < PAGE_OFFSET)
		|| unlikely((unsigned long)(init_mm.pgd) < PAGE_OFFSET) )
	{
		printk(KERN_ERR
			"exch_stand_guard, tsk addr [0x%p], init_mm.pgd addr [0x%p]!\n",
			tsk, init_mm.pgd);
		while(1){
			cpu_relax();
			/*it is unuseful below, only for coverity cleaning!!*/
			if (!himntn_gobal_resetlog)
			{
                         break;
			}
		}
	}
}

/*******************************************************************************
*                               maintain funciton                              *
*******************************************************************************/

/*******************************************************************************
 函 数 名: exch_task_stack_dump
 功能描述: 显示临终遗言模块指定任务调用栈信息
 输入参数: taskPid  指定任务PID
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
void exch_task_stack_dump(int taskPid)
{
    pid_t exc_pid = (pid_t)taskPid;
    struct task_struct *task = NULL;

    if (exc_pid < 0)
    {
        printk("exch_task_stack_dump:invalid param pid[0x%x]\n", exc_pid);
        return;
    }

    task = find_task_by_vpid(exc_pid);
    if (task)
    {
        show_stack(find_task_by_vpid(exc_pid),NULL);
    }
    else
    {
        printk("exch_task_stack_dump:no such a task pid[0x%x]\n", exc_pid);
    }
}

/*******************************************************************************
 函 数 名: exch_buf_show
 功能描述: 显示临终遗言模块指定内存地址内的内容
 输入参数: offset  DDR内存中的偏移地址
           size    显示内容大小
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
void exch_buf_show(unsigned int offset, unsigned int size)
{
    unsigned int add_offset = 0;

    if (!offset || !size)
    {
        printk("exch_buf_show:invalid param offset[0x%x] size[%d]\n", offset, size);
    }

    add_offset = (offset/(sizeof(unsigned int)))*sizeof(unsigned int);

    exch_hex_dump((unsigned char *)(EXCH_BASE_ADDR + add_offset), size, 16);
}

/*******************************************************************************
 函 数 名: exch_hex_dump
 功能描述: 打印buf信息，若为ASCII字符则打印字符
 输入参数: buf     待dump地址
           size    内容大小
           per_row 每行打印数据量
 输出参数: 无
 返 回 值: 无
*******************************************************************************/
void exch_hex_dump(unsigned char *buf, unsigned int size, unsigned char per_row)
{
    int i, row;
    unsigned char line[140];
    unsigned int left = size;

    if (!buf)
        return;

    per_row = (per_row > 32) ? 32 : per_row;
    if (!per_row)
    {
        per_row = 16;
    }

    printk("Dump buffer [%p] size [%d]:\n", buf, size);

#define TO_CHAR(a)      (((a) > 9 )? ((a) - 10 + 'A') : ((a) + '0'))
#define IS_PRINTABLE(a) ((a) > 31 && (a) < 127)

    for (row = 0; left; row++)
    {
        (void)hisi_io_memset(line, ' ', sizeof(line));

        for (i = 0; (i < per_row) && left; i++, left--, buf++)
        {
            unsigned char val = *buf;

            /* The HEX value */
            line[(i * 3)] = TO_CHAR(val >> 4);
            line[(i * 3) + 1] = TO_CHAR(val & 0x0F);

            /* 若为可显示ASCII字符，则打印该字符，否则打印"." */
            line[(per_row * 3) + 2 + i] = IS_PRINTABLE(val) ? val : '.';
        }

        line[(per_row * 3) + 2 + per_row] = '\0';

        printk("[%4u]: %s\n", row * per_row, line);
    }

#undef TO_CHAR
#undef IS_PRINTABLE
}



int exch_check_dump_space(unsigned long p_file)
{
    static unsigned long file_addr = 0x0;
    static char *shcmd    = "/system/bin/sh";
    static char *argv[]   = {"/system/bin/sh", "/system/etc/log/clean_log.sh", "110", NULL};
    static char *shenvp[] = {"HOME=/data", "TERM=vt100", "USER=root","PATH=/system/xbin:/system/bin", NULL};
    int  ret;

    if (file_addr != p_file)
    {
        file_addr = p_file;

        ret = call_usermodehelper(shcmd, argv, shenvp, UMH_NO_WAIT);
        if (ret < 0)
        {
            printk(KERN_ERR"%s : call_usermodehelper fail %d\n", __FUNCTION__, ret);
            return EXCH_ERROR;
        }
    }
    else
    {
        //just for pclint
    }

    return EXCH_OK;

}

IFC_GEN_CALL5(MAILBOX_IFC_ACPU_TO_CCPU_SYSTEMERROR, systemErrorCcore,
           IFC_INCNT,int, modid,0,
           IFC_INCNT,int, arg1,0,
           IFC_INCNT,int, arg2,0,
           IFC_INVAR,char*, arg3,0,
           IFC_INCNT,int, arg3Length,0)


/*******************************************************************************
*                                 test funciton                                *
*******************************************************************************/
#ifdef EXCH_TEST_FUNCTION_ON
/*******************************************************************************
 函 数 名: exchTest
 功能描述: 测试接口，用于触发不同类型系统异常
 输入参数: exc_type  异常类型
 输出参数: 无
 返 回 值: 无
*******************************************************************************/

int exchUNDEF(void *arg)
{
    int b = 0;
    FUNC_VOID a = (FUNC_VOID)(&b);
    b = 0x12345678;
    a();
    return 0;
}

int exchSWI(void *arg)
{
#ifdef CONFIG_ARM64
#else
    __asm__("        SWI   0x1234   ");
#endif
    return 0;
}

int exchPABT(void *arg)
{
    FUNC_VOID a = (FUNC_VOID)0xe0000000;
    a();
    return 0;
}

int exchDABT(void *arg)
{
    *(int *)0xa0000000 = 0x12345678;
    return 0;
}

int exchLOOP(void *arg)
{
    unsigned long flags = 0;
    spin_lock_irqsave(g_pExchRoot->spinlock, flags);
    for(;;)
    {
        ;
    }
    spin_unlock_irqrestore(g_pExchRoot->spinlock, flags);

	return 0;
}

void wdt_func(void *data)
{
    exchLOOP(data);
}
void exchWDT(void)
{
    smp_call_function_single(0, wdt_func, NULL, 0);
}

int exchDIV(void *arg)
{
#ifdef CONFIG_ARM64
    return 0;
#else
    int a = (int)arg;
    int b = 4;

    return  b/a;
#endif
}

int exchPANIC(void *arg)
{
    panic("exchPANIC!\n");

    return 0;
}
/*触发空指针操作时打开
void test_void_point(void)
{
    int* p = NULL;
    *p = 2;
}*/

int cb1(exch_cb_buf_t* p)
{
  printk(KERN_ERR"%s : save cust func reg is ok !\n", __FUNCTION__);
  return 0;
}
void exch_cb_reg(void)
{
  echc_cust_funt_func_reg((exch_CBReg)cb1);
}


void exchTest(int exc_type)
{
    switch (exc_type)
    {
    case EXC_UNDEFINE_INSTRUCTION:  //0
        kthread_run(exchUNDEF, 0, "exchUNDEF");
        break;
    case EXC_SOFTWARE_INTERRUPT:    //1
        kthread_run(exchSWI,   0, "exchSWI");
        break;
    case EXC_PREFETCH_ABORT:        //2
        kthread_run(exchPABT,  0, "exchPABT");
        break;
    case EXC_DATA_ABORT:            //3
        kthread_run(exchDABT,  0, "exchDABT");
        break;
    case EXC_DEVISION:              //4
        kthread_run(exchDIV,   0, "exchDIV");
        break;
    case EXC_PANIC:
        kthread_run(exchPANIC, 0, "exchPANIC");
        break;
    case EXC_LOOP:
        kthread_run(exchLOOP,  0, "exchLOOP");
        break;
    default:
        break;
    }
}
#endif

/* 测功a核异常文件的记录 */
void tsAcorePanic(int flag)
{
    if (0 == flag)
    {
    /* remove for coverity clean.
        *(unsigned int *)0x0 = 1;
        */
        printk(KERN_ERR "this case has been removed for coverity cleaning!!\n");
    }
    else if (1 == flag)
    {
        /*remove for coverity clean.
        while(1);*/
        printk(KERN_ERR "this case has been removed for coverity cleaning!!\n");
    }
    else
    {
    }
    printk(KERN_ERR "tsAcorePanic: flag=%d\n", flag);
}

/*******************************************************************************
 函 数 名:omTimerGet
 功能描述:获取系统timer的相对slice值
 输入参数:无
 输出参数:无
 返 回 值:timer slice value
*******************************************************************************/
unsigned int omTimerGet(void)
{
#if defined(_DRV_LLT_)
    return 0;
#else
    return (*(volatile unsigned int *)(SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_ADDR(HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR))));
#endif
}

/*******************************************************************************
 函 数 名:exch_mdm_panic_cb_reg
 功能描述:提供给CP,用于注册modem panic时所需执行的回调函数；
 输入参数:exch_mdm_panic_reg: pfunc 待注册的回调函数指针

 输出参数:无
 返 回 值:
*******************************************************************************/
unsigned int exch_mdm_panic_cb_reg(exch_mdm_panic_reg pfunc)
{
    if(NULL == pfunc)
    {
        printk("exch_mdm_panic_cb_reg:the input pfunc is NULL !!!!!!!\n");
        return 1;
    }
    g_exch_mdm_panic_reg = pfunc;
    return 0;
}

/*******************************************************************************
 函 数 名:exch_exc_mdm_panic_cb
 功能描述:AP 侧调用回调函数，执行modem panic操作；
 输入参数:无
 输出参数:无
 被调函数:
 返 回 值:
*******************************************************************************/
void exch_exc_mdm_panic_cb(void)
{
    if(NULL == g_exch_mdm_panic_reg)
    {
        printk("exch_exc_mdm_panic_cb:g_exch_mdm_panic_reg is NULL !!!!!!!\n");
        return;
    }
    g_exch_mdm_panic_reg();
    return;
}


EXPORT_SYMBOL(tsAcorePanic);

postcore_initcall(exch_postcore_init);
subsys_initcall(exch_early_init);
module_init(exch_entry);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

