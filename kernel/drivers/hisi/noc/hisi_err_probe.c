#include <linux/io.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include <linux/notifier.h>
#include <linux/jiffies.h>
#include <linux/nmi.h>
#include "hisi_err_probe.h"
#include "hisi_noc.h"
#include "hisi_noc_bus.h"
#include <linux/hisi/reset.h>
#ifdef CONFIG_ARCH_HI6XXX
#include "soc_baseaddr_interface.h"
#endif

#define ERR_CODE_NR	7
static char *err_code[] = {
	"Slave: returns Error response",
	"Master: Accesses reserved memory space",
	"Master: send the Burst to slave",
	"Master: accesses the powerdown area",
	"Master: Access Previlege error",
	"Master: received Hide Error Response from Firewall",
	"Master: accessed slave timeout and returned Error reponse",
};

#define MAX_OPC_NR	9

static char *opc[] = {
	"RD: INCR READ",
	"RDW: WRAP READ",
	"RDL: EXCLUSIVE READ",
	"RDX: LOCK READ",
	"WR: INCR WRITE",
	"WDW: WRAP WRITE",
	"Reversed",
	"WRC: EXCLUSIVE WRITE",
	"PRE: FIXED BURST"
};

static enoc_init_type s_noc_err_init_type = NOC_INIT_NONE;

/*offset of ADE regs*/
static const int s_ade_regs_offset[] =
{
    0x4,
    0x78,
    0x7c,
    0x80,
    0x84,
    0x88,
    0x98,
    0x1508,
    0x1514,
    0x1588,
    0x1594
};
static void __iomem *ade_reg_baseaddr_group1 = NULL;
static void __iomem *ade_reg_baseaddr_group2 = NULL;

#ifdef CONFIG_ARCH_HI6XXX
unsigned char*    g_ModemNocBaseAddr = NULL;
unsigned char*    g_ModemCtrlBaseAddr = NULL;
#endif

extern int  check_himntn(int feature);
extern void do_hifi_runstall(void);
extern void systemError(int modId, int arg1, int arg2, char *arg3, int arg3Length);
extern void *hisi_io_memcpy(void *dest, const void *src, unsigned int count);

#ifdef CONFIG_ARCH_HI6XXX
/*remap Modem???*/
void modem_reg_addr_init(void)
{

    if (NULL == g_ModemNocBaseAddr)
    {
        g_ModemNocBaseAddr = (unsigned char *)ioremap(SOC_MODEMNOC_SERVICE_BASE_ADDR, SZ_1K);
        if (NULL == g_ModemNocBaseAddr)
        {
            printk(KERN_ERR "the g_ModemNocBaseAddr ioremap fail!!\n");
            return;
        }
    }

    if (NULL == g_ModemCtrlBaseAddr)
    {
        g_ModemCtrlBaseAddr = (unsigned char *)ioremap(SOC_MODEM_SCTRL_BASE_ADDR, SZ_1K);
        if (NULL == g_ModemCtrlBaseAddr)
        {
            printk(KERN_ERR "the g_ModemCtrlBaseAddr ioremap fail!!\n");
            return;
        }
    }

}
#endif

/*输出base+0x14的值的含义*/
static void print_errlog0(unsigned int val)
{
	unsigned int idx;

	pr_err("[ERR_LOG0 = 0x%x]:\n", val);

	idx = (val & ERR_LOG0_ERRCODE_MASK) >> ERR_LOG0_ERRCODE_SHIFT;
	if (idx < ERR_CODE_NR)
		pr_err("\t[err_code=%d] %s\n", idx, err_code[idx]);
	else
		pr_err("\t[err_code=%d] out of range!\n",idx);

	idx = (val & ERR_LOG0_OPC_MASK) >> ERR_LOG0_OPC_SHIFT;
	if (idx < MAX_OPC_NR)
		pr_err("\t[opc=%d] %s\n", idx, opc[idx]);
	else
		pr_err("\t[opc=%d] out of range!\n",idx);
}
/*输出输出base+0x18的值的含义*/
static unsigned int print_errlog1(unsigned int val, unsigned int idx)
{
	struct noc_bus_info *noc_bus = 0;
	unsigned int shift = 0;

	int initflow;
	int targetflow;
	int targetsubrange;

        if(HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id)
            noc_bus=&hi6220_noc_buses_info[idx];
        else
            noc_bus= &hi3xxx_noc_buses_info[idx];


	pr_err("[ERR_LOG1 = 0x%x]\n", val);

	shift = ffs(noc_bus->initflow_mask) - 1;
	initflow = (val & (noc_bus->initflow_mask)) >> shift;
	if (initflow < noc_bus->initflow_array_size)
		pr_err("\t[init_flow=%d]: %s\n", initflow, noc_bus->initflow_array[initflow]);
	else
		pr_err("\t[init_flow=%d]: %s\n", initflow, "index is out of range!");

	shift = ffs(noc_bus->targetflow_mask) - 1;
	targetflow = (val & (noc_bus->targetflow_mask)) >> shift;
	if (targetflow < noc_bus->targetflow_array_size)
		pr_err("\t[target_flow=%d]: %s\n", targetflow, noc_bus->targetflow_array[targetflow]);
	else
		pr_err("\t[target_flow=%d]: %s\n", targetflow, "index is out of range!");

	shift = ffs(noc_bus->targ_subrange_mask) - 1;
	targetsubrange = (val & (noc_bus->targ_subrange_mask)) >> shift;
   	pr_err("\t[target_subrange]: %d\n", targetsubrange);

    return noc_find_addr_from_routeid(idx, initflow, targetflow, targetsubrange);

}
/*Output Error log buffer, one-track code for both Hi6xxx and Hi3xxx */
static void print_errlog(void __iomem *base, unsigned int idx)
{

	unsigned int val;
	unsigned int base_addr;
	/* dump all the err_log register */
	val = readl_relaxed(base + ERR_PORBE_ERRLOG0_OFFSET);
	print_errlog0(val);

	val = readl_relaxed(base + ERR_PORBE_ERRLOG1_OFFSET);
	base_addr=print_errlog1(val, idx);

	val = readl_relaxed(base + ERR_PORBE_ERRLOG3_OFFSET);
	pr_err("[ERR_LOG3]: ADDRESS_LOW = 0x%x, adjusted ADDRESS_LOW = 0x%x\n", val, val+base_addr);

	val = readl_relaxed(base + ERR_PORBE_ERRLOG4_OFFSET);
	pr_err("[ERR_LOG4]: ADDRESS_HIGH = 0x%x\n", val);


	val = readl_relaxed(base + ERR_PORBE_ERRLOG5_OFFSET);
	pr_err("[ERR_LOG5]: USER_SIGNAL = 0x%x\n", val);

	val = readl_relaxed(base + ERR_PORBE_ERRLOG7_OFFSET);
	pr_err("[ERR_LOG7]: SECURITY = %d\n", val);

}


void hi3xxx_noc_err_probe_hanlder(void __iomem *base, unsigned int idx)
{
        unsigned int offset;
        unsigned int *reg;
        unsigned int  size=SZ_128;
        //output error log buffer
        print_errlog(base,idx);

        printk(KERN_ERR "Error logger dump: \n");
        for(offset = 0 ; offset < size ; offset += 4 * sizeof(unsigned int)){
            reg = (unsigned int *)((char *)base + offset);
            printk(KERN_ERR "[%08x] : %08x %08x %08x %08x\n", offset, *reg, \
                *(reg + 1), *(reg + 2), *(reg + 3));
        }

        /* clear interrupt */
        writel_relaxed(ERR_PROBE_CLEAR_BIT, base + ERR_PORBE_ERRCLR_OFFSET);
}
void ade_reg_addr_init(void)
{
    if (NULL == ade_reg_baseaddr_group1)
    {
        ade_reg_baseaddr_group1 = (void __iomem *)ioremap(SOC_ADE_S_BASE_ADDR, SZ_8K);
        if (NULL == ade_reg_baseaddr_group1)
        {
            printk(KERN_ERR "the first ioremap fail!!\n");
            return;
        }
    }

    if (NULL == ade_reg_baseaddr_group2)
    {
        ade_reg_baseaddr_group2 = (void __iomem *)ioremap(SOC_ADE_S_BASE_ADDR, SZ_8K);
        if (NULL == ade_reg_baseaddr_group2)
        {
            printk(KERN_ERR "the first ioremap fail!!\n");
            return;
        }
    }
}
void ade_reg_addr_exit(void)
{
    if (NULL != ade_reg_baseaddr_group1)
    {
        iounmap(ade_reg_baseaddr_group1);
        ade_reg_baseaddr_group1 = NULL;
    }

    if (NULL != ade_reg_baseaddr_group2)
    {
        iounmap(ade_reg_baseaddr_group2);
        ade_reg_baseaddr_group2 = NULL;
    }
}
void check_doreset_for_noc(void)
{
#ifdef CONFIG_HISI_NOC_HI6220_PLATFORM
    if (check_himntn(HIMNTN_NOC_ERR_LOGGER_RESET))
    {
        switch (s_noc_err_init_type)
        {
        case NOC_INIT_MODEM:
            systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, 0, 0);
            break;
        case NOC_INIT_HIFI:
        /*    systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, 0, 0);*/
            break;
        default:
            break;
        }
    }
#endif
}
/*ADE require to print these value for debug*/
static void print_ade_reg_value(void)
{
    int i = 0;
    unsigned long addr= 0;
    unsigned long addr_phy= 0;

    int value = 0;

    int reg_size = sizeof(s_ade_regs_offset)/sizeof(s_ade_regs_offset[0]);

    printk(KERN_ERR "ade reg dump : \n");


    if (NULL == ade_reg_baseaddr_group1)
    {
        printk(KERN_ERR "ade_reg_baseaddr_group1 is NULL!!\n");
        return;
    }
    /*print the first group*/
    for (i = 0;i < reg_size;i++)
    {
        addr = (unsigned long)ade_reg_baseaddr_group1 + s_ade_regs_offset[i];
        value = *(int*)addr;
        addr_phy = SOC_ADE_S_BASE_ADDR + s_ade_regs_offset[i];
        printk(KERN_ERR "[0x%p]: 0x%x\n", (void*)addr_phy, value);
    }

    if (NULL == ade_reg_baseaddr_group2)
    {
        printk(KERN_ERR "ade_reg_baseaddr_group2 is NULL!!\n");
        return;
    }
    /*print the second group*/
    for (i = 0;i < reg_size;i++)
    {
        addr = (unsigned long)ade_reg_baseaddr_group2 + s_ade_regs_offset[i];
        addr_phy = (SOC_ADE_S_BASE_ADDR + 0x8000) + s_ade_regs_offset[i];
        printk(KERN_ERR "[0x%p]: 0x%x\n", (void*)addr_phy, value);
    }
}

static void translate_error_logger(void *base)
{
    unsigned int *reg = 0;
    unsigned int reg_init_value = 0;
    unsigned int reg_target_value = 0;

    reg = (unsigned int *)(base + ERR_PORBE_ERRLOG1_OFFSET);

    if (error_logger == base)
    {
        /* dump all the err_log register */
        print_errlog(base,0);
        reg_init_value = ((*reg)&0x0003c000) >> 0xe;
        reg_target_value = ((*reg)&0x00003c00) >> 0xa;
        if (reg_init_value < sizeof(sysnoc_initflow_array)/sizeof(char*)
            && reg_target_value < sizeof(sysnoc_targetflow_array)/sizeof(char*))
        {
            printk(KERN_ERR "sysnoc, %s access %s:\n",sysnoc_initflow_array[reg_init_value], sysnoc_targetflow_array[reg_target_value]);
            /*if it is modem that accesses some ip, we will treat separately*/
            if (NULL != strstr((const char*)sysnoc_initflow_array[reg_init_value], (const char*)"modem"))
            {
                s_noc_err_init_type = NOC_INIT_MODEM;
            }
            /*if it is hifi that accesses some ip, we will treat separately*/
            if (NULL != strstr((const char*)sysnoc_initflow_array[reg_init_value], (const char*)"hifi"))
            {
                s_noc_err_init_type = NOC_INIT_HIFI;
            }
        }
        printk(KERN_ERR "sysnoc error logger dump:\n");

    }
    else if (error_logger_media == base)
    {

        /* dump all the err_log register */
        print_errlog(base,1);

        /*print ADE reg's value*/
        print_ade_reg_value();

        reg_init_value = ((*reg)&0x0001c000) >> 0xe;
        reg_target_value = ((*reg)&0x00003c00) >> 0xa;
        if (reg_init_value < sizeof(media_initflow_array)/sizeof(char*)
            && reg_target_value < sizeof(media_targetflow_array)/sizeof(char*))
        {
            printk(KERN_ERR "medianoc, %s access %s:\n",media_initflow_array[reg_init_value], media_targetflow_array[reg_target_value]);
        }
        s_noc_err_init_type = NOC_INIT_MEDIA;
        printk(KERN_ERR "medianoc error logger dump : \n");
    }
    else
    {
        printk(KERN_ERR "other error logger dump : \n");
    }
    return;
}

static void dump_error_logger(void *base, unsigned int size)
{
    unsigned int offset;
    unsigned int *reg;

    reg = (unsigned int *)(base + ERR_PORBE_ERRVLD_OFFSET);

    if (0 == *reg)
    {
        return;
    }

    for(offset = 0 ; offset < size ; offset += 4 * sizeof(unsigned int)){
        reg = (unsigned int *)((char *)base + offset);
        MNTN_PRINT("[%08x] : %08x %08x %08x %08x\n", offset, *reg, \
            *(reg + 1), *(reg + 2), *(reg + 3));
    }
    translate_error_logger(base);

#ifdef CONFIG_ARCH_HI6XXX
    /*如果是Modem Noc 那么打印Noc信息*/
    if(NULL != g_ModemNocBaseAddr && NOC_INIT_MODEM == s_noc_err_init_type)
    {
        MNTN_PRINT("modem noc error logger dump:\n");
        for(offset = 0 ; offset < size ; offset += 4 * sizeof(unsigned int)){
            reg = (unsigned int *)((char *)g_ModemNocBaseAddr + offset);
            MNTN_PRINT("[%08x] : %08x %08x %08x %08x\n", offset, *reg, \
                *(reg + 1), *(reg + 2), *(reg + 3));
        }
    }
#endif
    return;
}

void dump_err_sys_logger(void)
{
     if(error_logger != NULL)
     {
         dump_error_logger(error_logger, SZ_128);
     }
     else
     {
         printk(KERN_ERR" error_logger is NULL\n");
     }
}

EXPORT_SYMBOL(dump_err_sys_logger);

void dump_err_media_logger(void)
{
     if(error_logger_media != NULL)
     {
         dump_error_logger(error_logger_media, SZ_128);
     }
     else
     {
         printk(KERN_ERR" error_logger_meida is NULL\n");
     }
}

EXPORT_SYMBOL(dump_err_media_logger);

void copy_err_sys_logger(void * uaddr,size_t len)
{
     if( uaddr == NULL)
     {
        printk(KERN_ERR" uaddr is NULL in %s\n",__func__);
        return;
     }
     if(error_logger != NULL)
     {
#ifdef CONFIG_HISI_NOC_HI6220_PLATFORM
         hisi_io_memcpy(uaddr, (const void * )error_logger,len);
#endif
     }
     else
     {
         printk(KERN_ERR" error_logger is NULL\n");
     }
}

EXPORT_SYMBOL(copy_err_sys_logger);

void  copy_err_media_logger(void * uaddr,size_t len)
{
     if( uaddr == NULL)
     {
         printk(KERN_ERR" uaddr is NULL in %s\n",__func__);
         return;
     }
     if(error_logger_media != NULL)
     {
#ifdef CONFIG_HISI_NOC_HI6220_PLATFORM
         hisi_io_memcpy(uaddr, (const void * )error_logger_media,len);
#endif
     }
     else
     {
         printk(KERN_ERR" error_logger_meida is NULL\n");
     }
}

EXPORT_SYMBOL(copy_err_media_logger);

/*call_noc_onoff used by ecall*/
void call_noc_onoff(int ioper, unsigned int ionoff)
{
    switch (ioper)
    {
    case 1:/*sysnoc off*/
        if (error_logger > 0)
        {
            writel(ionoff, SOC_SYSNOC_ERR_LOGGER_FAULTEN_ADDR(error_logger));
        }
        break;
    case 2:/*media noc off*/
        if (error_logger_media > 0)
        {
            writel(ionoff, SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_ADDR(error_logger_media));
        }
        break;
    case 3:/*both off*/
        if (error_logger > 0)
        {
            writel(ionoff, SOC_SYSNOC_ERR_LOGGER_FAULTEN_ADDR(error_logger));
        }
        if (error_logger_media > 0)
        {
            writel(ionoff, SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_ADDR(error_logger_media));
        }
        break;
    default:
        break;
    }
}

static int noc_sys_panic_notify(struct notifier_block *self,
			  unsigned long event, void *data)
{
    dump_err_sys_logger();
	return NOTIFY_OK;
}

struct notifier_block noc_on_sys_panic_nb = {
	.notifier_call = noc_sys_panic_notify,
	.priority = INT_MAX,
};


static int noc_media_panic_notify(struct notifier_block *self,
			  unsigned long event, void *data)
{
    dump_err_media_logger();
	return NOTIFY_OK;
}

struct notifier_block noc_on_media_panic_nb = {
	.notifier_call = noc_media_panic_notify,
	.priority = INT_MAX,
};



void hi6220_noc_err_probe_hanlder(void __iomem *base)
{
#ifdef CONFIG_HISI_NOC_HI6220_PLATFORM
    if(NULL == base)
    {
        pr_err("noc err, but base is NULL!!\n");
        systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, base, SZ_128);
        return;
    }

    /*Modem Noc不触发全局复位，需要将该变量置NONE，避免Modem Noc之后，其他异常触发的Noc进Modem Noc流程*/
    s_noc_err_init_type = NOC_INIT_NONE;

    /*show error logger data*/
    dump_error_logger(base,SZ_128);
    if (check_himntn(HIMNTN_NOC_ERR_LOGGER_RESET))
    {
        switch (s_noc_err_init_type)
        {
        case NOC_INIT_MODEM:
            /*don't reset system now if it is modem that triger this noc irq,
            for saving modem log.
            Reset after C starts running again.*/
            break;
        case NOC_INIT_HIFI:
            /*runstall hifi*/
            do_hifi_runstall();
            /*don't reset system now if it is hifi that triger this noc irq,
            for saving hifi log.
            Reset after hifi starts running again.*/
            systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, base, SZ_128);
            break;
        case NOC_INIT_MEDIA:
            /*don't reset system if media noc, for printking logs*/
            break;
        default:
            systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, base, SZ_128);
            break;
        }
    }
    else
    {
        bool    bret = false;
        /*change print level*/
        console_verbose();
        bret = trigger_all_cpu_backtrace();
	 if (false == bret)
	 {
            pr_err("fail to trigger all cpus to print backtrace in noc irq handler!!\n");
	 }
    }
#endif
    /*use disable irq here, from luhua*/
	writel_relaxed(0x0, base + ERR_PORBE_FAULTEN_OFFSET);
}

void enable_err_probe(void __iomem *base)
{
	unsigned int val;
	val = readl_relaxed(base + ERR_PORBE_ERRVLD_OFFSET);
	if (val & ERR_PORBE_ERRVLD_BIT) {
			pr_err("ErrProbe happened before enabling interrupt\n");

			val = readl_relaxed(base + ERR_PORBE_ERRLOG0_OFFSET);
			print_errlog0(val);
			val = readl_relaxed(base + ERR_PORBE_ERRLOG1_OFFSET);
			pr_err("err_log1 = 0x%x\n", val);

			/* clear errvld */
			writel_relaxed(ERR_PROBE_CLEAR_BIT, base + ERR_PORBE_ERRCLR_OFFSET);
			wmb();
	}

	/* enable err probe intrrupt */
	writel_relaxed(ERR_PORBE_ENABLE_BIT, base + ERR_PORBE_FAULTEN_OFFSET);
}

void disable_err_probe(void __iomem *base)
{
	writel_relaxed(~ERR_PORBE_ENABLE_BIT, base + ERR_PORBE_FAULTEN_OFFSET);
}

void enable_err_probe_by_name(const char *name)
{
	void __iomem *base = get_errprobe_base(name);
	if (base == NULL) {
		pr_err("%s cannot get the node!\n", __func__);
		return;
	}

	enable_err_probe(base);
}
EXPORT_SYMBOL(enable_err_probe_by_name);


void disable_err_probe_by_name(char *name)
{
	void __iomem *base = get_errprobe_base(name);
	if (base == NULL) {
		pr_err("%s cannot get the node!\n", __func__);
		return;
	}

	disable_err_probe(base);
}
EXPORT_SYMBOL(disable_err_probe_by_name);
