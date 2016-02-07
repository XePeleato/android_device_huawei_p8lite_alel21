#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <soc_irqs.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/io.h>
#include <asm/io.h>
#include <soc_mddrc_dmc_interface.h>
#include <soc_mddrc_security_interface.h>
#include <soc_mddrc_qos_interface.h>
#include <linux/hisi/hi6xxx-platform.h>
#include <linux/hisi/reg_ops.h>
#include "soc_pmctrl_interface.h"

#include <linux/hisi/hi6xxx_mntn.h>
#include <linux/hisi/hi6xxx_ipc_enum.h>
#include "../ipc_hi6xxx/bsp_drv_ipc.h"

#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include <linux/workqueue.h>        /* For workqueue... */
#include "../mntn/excDrv.h"
#include "soc_ao_sctrl_interface.h"
#include <soc_baseaddr_interface.h>
#include <soc_mddrc_dmc_interface.h> /* config ddr autorefresh in sram_reboot() */
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/hisi/util.h>
#define MEDIA_NOC_ERR_PORT 1
#define SYS_NOC_ERR_PORT   2

/*#define MAX_PROTECT_DDR_18M 0x1200000*/
#ifdef CONFIG_DEBUG_FS

struct ddrc_info_stru
{
    unsigned int count; /* counts */
    unsigned int ddrc_intmask;
    unsigned int ddrc_rint;
    unsigned int ddrc_intstat;
    char error_logger_info[REG_ERROR_LOGGER_0_IOSIZE];
    char ddrc_sec_info[1];
};

static int ddrc_sec_irq_num = 0;      

static void __iomem *ddrc_dmc0_protect_base_virt = 0;
static void __iomem *ddrc_sec0_base_virt = 0;
static void __iomem *ddrc_qos_base_virt = 0;
/*static unsigned int ddrc_info_save_addr = 0;*/
static struct workqueue_struct *ddr_reset_wq = NULL;
static struct work_struct ddr_sec_work;
static DEFINE_SPINLOCK(ddrc_sec_lock);

#define DDRC_SEC_LOCK(flags)            spin_lock_irqsave(&ddrc_sec_lock,flags)
#define DDRC_SEC_UNLOCK(flags)          spin_unlock_irqrestore(&ddrc_sec_lock,flags)

#define REG_BASE_DDRC_DMC0_PROTECT_VIRT ddrc_dmc0_protect_base_virt
#define REG_BASE_DDRC_SEC0_VIRT         ddrc_sec0_base_virt
#define REG_BASE_DDRC_QOS_VIRT          ddrc_qos_base_virt
#define DDRC_INFO_SAVE_ADDR             exch_get_drv_addr()


#ifdef _DRV_LLT_
#define STATIC
#else
#define STATIC    static
#endif

extern void dump_err_sys_logger(void);
extern void copy_err_sys_logger(void * uaddr,size_t len);
extern void dump_err_media_logger(void);
extern void copy_err_media_logger(void * uaddr,size_t len);
void platform_ddr_protect_18M(int portnum, int flag)
{
    unsigned long flags;
    volatile SOC_MDDRC_SECURITY_SEC_INT_EN_UNION *sec_int_en_reg;
    volatile SOC_MDDRC_SECURITY_SEC_RGN_MAP_UNION *sec_map_region1_reg,*sec_map_region2_reg;
    volatile SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_UNION *sec_rgn_region1_reg,*sec_rgn_region2_reg;
    char *base_addr = (char *)REG_BASE_DDRC_SEC0_VIRT;

#ifdef _DRV_LLT_
#else
    DDRC_SEC_LOCK(flags);

    sec_int_en_reg =(volatile SOC_MDDRC_SECURITY_SEC_INT_EN_UNION *)(SOC_MDDRC_SECURITY_SEC_INT_EN_ADDR(base_addr) + 0x400*portnum);
    sec_map_region1_reg =(volatile SOC_MDDRC_SECURITY_SEC_RGN_MAP_UNION *)(SOC_MDDRC_SECURITY_SEC_RGN_MAP_ADDR(base_addr, 0x1)+ 0x400*portnum);
    sec_map_region2_reg =(volatile SOC_MDDRC_SECURITY_SEC_RGN_MAP_UNION *)(SOC_MDDRC_SECURITY_SEC_RGN_MAP_ADDR(base_addr, 0x2)+ 0x400*portnum);
    sec_rgn_region1_reg =(volatile SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_UNION *)(SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_ADDR(base_addr, 0x1)+ 0x400*portnum);
    sec_rgn_region2_reg =(volatile SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_UNION *)(SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_ADDR(base_addr, 0x2)+ 0x400*portnum);

    sec_int_en_reg->reg.int_en =0x1; /*enable interrupt*/
    /*region0*/
    //*(volatile unsigned int*)(base_addr + 0x104 + 0x400*portnum) |= 0xf;   //y_todo:

    /*region1,addr start*/
    sec_map_region1_reg->reg.rgn_base_addr =0x0;
#ifndef BSP_COMPILE_ALLY
    if( 2 != portnum)
#endif
    {
    if (flag)/*start ddr protect*/
    {
        /*region1,addr end*/
        sec_map_region1_reg->reg.rgn_size =0x0;
        sec_rgn_region1_reg->reg.subrgn_disable =0x0;
        sec_rgn_region1_reg->reg.sp=0xF;

        sec_map_region1_reg->reg.rgn_size = 0x17;/*16M*/
        sec_map_region1_reg->reg.rgn_en =0x1;
        /*region2,addr start*/
        sec_map_region2_reg->reg.rgn_base_addr= 0x100; /*高位地址*/
        /*region2,addr end*/
        sec_map_region2_reg->reg.rgn_size =0x0;
        sec_rgn_region2_reg->reg.subrgn_disable =0x0;
        sec_rgn_region2_reg->reg.sp=0xF;

        sec_map_region2_reg->reg.rgn_size = 0x14;/*2M*/
        sec_map_region2_reg->reg.rgn_en =0x1;
        //*(volatile unsigned int*)(base_addr + 0x004 + 0x400*portnum)  &= (~0x1);    /*安全功能使能*/
    }
    else/*stop ddr protect*/
    {
        sec_map_region1_reg->reg.rgn_size =0x0;
        sec_map_region1_reg->reg.rgn_en =0x0;
        sec_rgn_region1_reg->reg.subrgn_disable =0x0;
        sec_rgn_region1_reg->reg.sp=0x0;

        sec_map_region2_reg->reg.rgn_base_addr= 0x0;; /*高位地址设为0*/
        sec_map_region2_reg->reg.rgn_size =0x0;
        sec_map_region2_reg->reg.rgn_en =0x0;
        sec_rgn_region2_reg->reg.subrgn_disable =0x0;
        sec_rgn_region2_reg->reg.sp=0x0;
        //*(volatile unsigned int*)(base_addr + 0x004 + 0x400*portnum)  |= 0x1;    /*安全功能bypass*/
    }
    }
    DDRC_SEC_UNLOCK(flags);
#endif
}

int platform_ddr_protect_init(int flag)
{
    int port = 1;
    int end_port = 4;   /*v8 有5个端口，端口0给ccpu，是否可以使用DTS*/

    pr_info("platform_ddr_protect_18M_init %d\n",flag);
    while (port <= end_port) {
        platform_ddr_protect_18M(port, flag);
        port++;
    }

    return 0;
}

int ddr_protect_init(void)
{
    ddrc_dmc0_protect_base_virt = ioremap(SOC_MDDRC_DMC_BASE_ADDR, PAGE_ALIGN(SZ_4K)); //y_todo:是否可以使用DTS

    if (0 == ddrc_dmc0_protect_base_virt) {
        pr_err("%s: ioremap failed\n",__func__);
        return -1;
    }

    ddrc_sec0_base_virt = ioremap(SOC_MDDRC_SECURITY_BASE_ADDR, PAGE_ALIGN(SZ_1K * 5));  //y_todo: 是否可以使用DTS

    if (0 == ddrc_sec0_base_virt) {
        iounmap(ddrc_dmc0_protect_base_virt);
        ddrc_dmc0_protect_base_virt = 0;
        pr_err("%s: ioremap failed\n",__func__);
        return -1;
    }
	
	ddrc_qos_base_virt = ioremap(SOC_MDDRC_QOS_BASE_ADDR, PAGE_ALIGN(SZ_4K));
	if (0 == ddrc_qos_base_virt) {
        iounmap(ddrc_sec0_base_virt);
	ddrc_sec0_base_virt = 0;
	iounmap(ddrc_dmc0_protect_base_virt);
    ddrc_dmc0_protect_base_virt = 0;
    pr_err("%s: ioremap failed\n",__func__);
    return -1;
    }
    /*platform_ddr_protect_init(1);*/
    return 0;
}
early_initcall(ddr_protect_init);

STATIC void dump_ddr_protect_logger(void *base, unsigned int size)
{
    unsigned int offset;
    unsigned int *reg;

    printk(KERN_ERR "ddrc logger dump : \n");

    for(offset = 0 ; offset < size ; offset += 4 * sizeof(unsigned int)){
        reg = (unsigned int *)((char *)base + offset);
        printk(KERN_ERR "[%08x] : %08x %08x %08x %08x\n", offset, *reg, \
            *(reg + 1), *(reg + 2), *(reg + 3));
    }

    return;
}
static void ddrc_sec_work_handle(void)
{
	systemError((int)BSP_MODU_MNTN, (int)EXCH_S_DDRC_SEC, 0, 0, 0);
	return;
}
#define PORTNUM1_MAX    8
#define PORTNUM2_MAX    32
#define PORTNUM3_MAX    1
#define PORTNUM4_MAX    1

char *portnum1[PORTNUM1_MAX] = 
				{"NOP","ADE_M0","ADE_M1","ISP","VIDEO_CODEC_M0","VIDEO_CODEC_M1","JPEG_CODEC","NOP"};
char *portnum2[PORTNUM2_MAX] = 
				{"Acpu","BBP_M0","BBP_M1","BBP_M2","BBP_M3","MDMAC","GU CIPHER","LC CIPHER",
				  "NOP","NOP","NOP","NOP","NOP","NOP","NOP","NOP",
				  "CSSYS","MMC0","MMC1","MMC2","USB","SECENG","APDMAC","IPF",
				  "SOCP","HIFI","DAP","MCU","NOP","UPACC","BBE16","Ccpu"};
char *portnum3[PORTNUM3_MAX] = {"Acpu"};
char *portnum4[PORTNUM4_MAX] = {"GPU"};

char *acess_type[2] = {"Read","Write"};
char *Secure_type[2] = {"Secure","NonSecure"};
char *Previlege_type[2] = {"UnPrivilege","Privilege"};

/* process the ddrc security & other events */
void ddrc_sec_errinfo(unsigned int portnum, unsigned int fail_addr,unsigned int masterval,unsigned int accesstype)
{
	unsigned int i = 0;
	char * master = NULL;
	char * err_type = NULL;
	char * sec_type = NULL;
	char * pri_type = NULL;
	
	err_type = ((accesstype & (1<<20)) == 0)? acess_type[0]:acess_type[1];
	sec_type = ((accesstype & (1<<17)) == 0)? Secure_type[0]:Secure_type[1];
	pri_type = ((accesstype & (1<<16)) == 0)? Previlege_type[0]:Previlege_type[1];

	switch(portnum)
	{
		case(1):
			i = (masterval & 0xE000) >> 13;
			master = portnum1[i];
			break;
		case(2):
			i = (masterval & 0xF800) >> 11;
			master = portnum2[i];
			break;	
		case(3):
			i = 0;
			master = portnum3[i];
			break;	
		case(4):
			i = 0;
			master = portnum4[i];
			break;	
		default:
			pr_err("warning! DDR SEC has something wrong! \n");
			break;
	}
	if(master)
	{
	pr_err("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	pr_err("Oops: Some speacial addr has been visited by some illegal action \n");
	pr_err(" Portnum	 %d \n Master_id	 %s \n Acess_type	 %s \n Secure_type	 %s \n Previlege_type  %s \n Err_Addr	 0x%x", \
							portnum,master,err_type,sec_type,pri_type,fail_addr);
	pr_err("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	}
	return;
}
static irqreturn_t ddrc_events_handler(int irq, void *dev)
{
    int portnum = 1;

    int end_port = 4;

    char * base_addr = (char *)REG_BASE_DDRC_SEC0_VIRT;
    int first_invader;
    unsigned int fail_addr;
    unsigned int masterval = 0;
    unsigned int accesstype = 0;
    unsigned int ddrc_rint = 0;
    unsigned int ddrc_sec_event = 0;
    unsigned int ddrc_tmod_rint = 0;
    struct ddrc_info_stru *p_ddrc_info = (struct ddrc_info_stru *)DDRC_INFO_SAVE_ADDR;

#ifdef _DRV_LLT_
#else
    unsigned int tmp; 

    p_ddrc_info->count++;

    p_ddrc_info->ddrc_intmask = readl(SOC_MDDRC_DMC_DDRC_INTMSK_ADDR(REG_BASE_DDRC_DMC0_PROTECT_VIRT));
    ddrc_rint = readl(SOC_MDDRC_DMC_DDRC_RINT_ADDR(REG_BASE_DDRC_DMC0_PROTECT_VIRT));
    p_ddrc_info->ddrc_rint = ddrc_rint;
    p_ddrc_info->ddrc_intstat = readl(SOC_MDDRC_DMC_DDRC_INTSTS_ADDR(REG_BASE_DDRC_DMC0_PROTECT_VIRT));

    pr_err("%s:count=%d,ddrc_intmask=0x%x,ddrc_rint=0x%x,ddrc_intstat=0x%x\n",__func__,p_ddrc_info->count, \
            p_ddrc_info->ddrc_intmask,p_ddrc_info->ddrc_rint,p_ddrc_info->ddrc_intstat);
    pr_err("%s:count=%d,qos_intmask=0x%x,qos_rint=0x%x,qos_intstat=0x%x\n",__func__,p_ddrc_info->count, \
					readl(SOC_MDDRC_QOS_QOSB_INTMSK_ADDR(REG_BASE_DDRC_QOS_VIRT)),	\
					readl(SOC_MDDRC_QOS_QOSB_RINT_ADDR(REG_BASE_DDRC_QOS_VIRT)),	\
					readl(SOC_MDDRC_QOS_QOSB_INTSTS_ADDR(REG_BASE_DDRC_QOS_VIRT)));
	ddrc_tmod_rint = p_ddrc_info->ddrc_intstat & 0xc0;
    if (ddrc_tmod_rint) {
        pr_err("warning! the DDR temperture >= 85C \n");
        tmp = ddrc_tmod_rint | p_ddrc_info->ddrc_intmask;
        writel(tmp, SOC_MDDRC_DMC_DDRC_INTMSK_ADDR(REG_BASE_DDRC_DMC0_PROTECT_VIRT));

        writel(ddrc_tmod_rint, SOC_MDDRC_DMC_DDRC_RINT_ADDR(REG_BASE_DDRC_DMC0_PROTECT_VIRT));


        return IRQ_HANDLED;
    }

    while (portnum <= end_port) {
        if (readl(SOC_MDDRC_SECURITY_SEC_INT_STATUS_ADDR(base_addr) + 0x400*portnum)) {  /*中断状态有效*/
            fail_addr = readl(SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_0_ADDR(base_addr) + 0x400*portnum); /*记录越权地址*/
            masterval = (readl(SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_ADDR(base_addr) + 0x400*portnum))& 0xffffff; /*越权者*/
            accesstype = (readl (SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_ADDR(base_addr) + 0x400*portnum));/*读/写*/

			first_invader = portnum;
			printk(KERN_ERR "ddr_protect_irq: portnum! (0x%x)\n", portnum);
			dump_ddr_protect_logger((void *)(base_addr + 0x400*portnum), 0x400);
			memcpy((&p_ddrc_info->ddrc_sec_info[0] + portnum*0x100),(char*)(base_addr + 0x400*portnum),0x100);
			ddrc_sec_event++;
			printk(KERN_ERR "ddr_protect_irq:  portnum (0x%x),addr(0x%x),master(0x%x), type(0x%x)\n", portnum, fail_addr,masterval,accesstype);
			ddrc_sec_errinfo(portnum, fail_addr,masterval,accesstype);

            if(portnum == MEDIA_NOC_ERR_PORT)
            {
               dump_err_media_logger();
               copy_err_media_logger(&p_ddrc_info->error_logger_info[0],REG_ERROR_LOGGER_0_IOSIZE);
            }
            else if(portnum == SYS_NOC_ERR_PORT)
            {
               dump_err_sys_logger();
               copy_err_sys_logger(&p_ddrc_info->error_logger_info[0],REG_ERROR_LOGGER_0_IOSIZE);
            }
            writel(0x1,SOC_MDDRC_SECURITY_SEC_INT_CLEAR_ADDR(base_addr) + 0x400*portnum); /*清除中断和越权失效地址*/
       }
       portnum++;
    }

    if ((ddrc_sec_event || ddrc_rint) && check_himntn(HIMNTN_ACCESS_ERROR_REBOOT)){
        systemError((int)BSP_MODU_MNTN, (int)EXCH_S_DDRC_SEC, 0, 0, 0);
    }

#endif

    return IRQ_HANDLED;
}

static struct of_device_id ddrc_protect_of_device_ids[] = {
    {.compatible = "hisilicon,ddrc-protect"},
    {},
};

static int ddrc_protect_device_probe(struct platform_device *pdev)
{
    struct resource *ddrc_res = NULL;
    int ret = 0;

    ddrc_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if(NULL == ddrc_res)
    {
        return -1;
    }
    ddrc_sec_irq_num = ddrc_res->start;
	
    ddr_reset_wq = create_singlethread_workqueue("ddr_reset_wq");
    INIT_WORK(&ddr_sec_work,(void *)ddrc_sec_work_handle);
    ret = request_irq(ddrc_sec_irq_num, ddrc_events_handler, IRQF_NO_SUSPEND, "ddrc_dmc_sec", NULL);
    if (ret) {
        printk("%s: irq_request  ddrc_sec_int %d failed!   \n","DDRC on write", ddrc_sec_irq_num);
		return ret;
    }

    /*register ipc interrupts*/
    ret = BSP_IPC_IntConnect(IPC_ACPU_INT_SRC_MCU_DDR_EXC, (IPCVOIDFUNCPTR)ddrc_events_handler, 0);
    if (ret) {
        pr_err("failed to connect IPC irq handle (%d)\n", IPC_ACPU_INT_SRC_MCU_DDR_EXC);
        return ret;
    }
    BSP_IPC_IntEnable(IPC_ACPU_INT_SRC_MCU_DDR_EXC);

    return ret;
}

static struct platform_driver ddrc_protect_driver = {
    .driver		= {
		.name	= "ddrc-protect",
		.owner	= THIS_MODULE,
		.of_match_table = ddrc_protect_of_device_ids,
    },
    .probe		= ddrc_protect_device_probe,
};
#ifdef CONFIG_FACTORY_MODE
unsigned int * g_factory_ddrtest_flag = NULL;

static int hisi_dt_show(struct seq_file *m, void *v)
{
    unsigned int flag_status = *g_factory_ddrtest_flag;

    seq_printf(m, "hisi_dt status is 0x%x\n",flag_status);

	return 0;
}

static int hisi_dt_open(struct inode *inode, struct file *file)
{
	return single_open(file, hisi_dt_show, NULL);
}
ssize_t dt_on_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{

    *g_factory_ddrtest_flag = MEMORY_FACTORY_DDRTEST_FLAG_STEP1;

    return (ssize_t)cnt;
}


static const struct file_operations dt_on_fops = {
    .open    = hisi_dt_open,
    .write   = dt_on_write,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};


ssize_t dt_off_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
    unsigned int flag_status = *g_factory_ddrtest_flag;
    if(flag_status == MEMORY_FACTORY_DDRTEST_FLAG_STEP2)
    {
        *g_factory_ddrtest_flag = MEMORY_FACTORY_DDRTEST_FLAG_STEP3;
    }

    return (ssize_t)cnt;
}


static const struct file_operations dt_off_fops = {
    .open    = hisi_dt_open,
    .write   = dt_off_write,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
#endif

STATIC int __init ddrc_protect_init(void)
{
    pr_info("start %s, %d\n",__func__,__LINE__);
#ifdef CONFIG_FACTORY_MODE
    g_factory_ddrtest_flag = HISI_VA_ADDRESS(MEMORY_FACTORY_DDRTEST_FLAG_ADDR);

    balong_create_ddrtest_proc_entry("hisi_dt_on", 0664 , &dt_on_fops, NULL);

    balong_create_ddrtest_proc_entry("hisi_dt_off", 0664 , &dt_off_fops, NULL);

#endif

    return(platform_driver_register(&ddrc_protect_driver));
}

STATIC void __exit ddrc_protect_exit(void)
{
    iounmap(REG_BASE_DDRC_DMC0_PROTECT_VIRT);
    iounmap(REG_BASE_DDRC_SEC0_VIRT);
    iounmap(REG_BASE_DDRC_QOS_VIRT);
    platform_driver_unregister(&ddrc_protect_driver);

    disable_irq(ddrc_sec_irq_num);
	flush_workqueue(ddr_reset_wq);  
	destroy_workqueue(ddr_reset_wq);
}

#else
static int __init ddrc_protect_init(void)
{
    return 0;
}
static void __exit ddrc_protect_exit(void)
{

}
#endif
module_init(ddrc_protect_init);
module_exit(ddrc_protect_exit);

MODULE_DESCRIPTION("DDRC PROTECT Driver");
MODULE_LICENSE("GPL");

