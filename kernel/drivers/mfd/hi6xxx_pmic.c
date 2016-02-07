/*
 * Device driver for PMU DRIVER	 in HI6xxx SOC
 *
 * Copyright (c) 2011 Hisilicon Co. Ltd
 *
 * Dongbin Yu <yudongbin@huawei.com> : 2014-1-26
 *
 * In V8R1 the pmic is same with V9R1's driver,so it is multiplex.
 * That the reason why name of the c document is "hi6xxx".
 *
 * I only code the pmic arch,there are some thing which not finished because
 * of the litter time.In process of iterative development you must add some
 * function into the module.
 *
 *
 */
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hardirq.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/irqdomain.h>
#include <linux/mfd/hisi_hi6xxx_pmic.h>
#include "soc_baseaddr_interface.h"
#include "soc_ao_sctrl_interface.h"
#include "soc_smart_interface.h"
#include "general_sram_map.h"
#if defined (CONFIG_HUAWEI_DSM)
#include <huawei_platform/dsm/dsm_pub.h>
#endif

#if defined(CONFIG_HISI_HI6551_PMIC)
#error ydb6551
#include "chip-hi6551.h"
#elif defined(CONFIG_HISI_HI6552_PMIC)
#include "chip-hi6552.h"
#elif defined(CONFIG_HISI_HI6553_PMIC)
#error ydb6553
#include "chip-hi6553.h"
#else
#error no_pmu
#endif

#ifdef _DRV_LLT_
#define static
#endif

#define PMUSSI_REG(reg_addr)        (((reg_addr)<<2) + (char *)g_pmussi_baseaddr)
#if defined (CONFIG_HUAWEI_DSM)
#define DSM_PMU_IRQ1_MASK 	(0x0f)
#define DSM_PMU_IRQ2_MASK 	(0x01)
#define DSM_PMU_IRQ_COUNT		(2)
#endif

struct hi6xxx_pmic{
	struct resource		*res;
	struct device		*dev;
	spinlock_t ssi_hw_lock;
    struct clk          *clk;
	/*for phase1 these domain is NULL*/
	struct irq_domain	*domain;
	int			irq;
	int			gpio;
	unsigned int			irqs[HI6XXX_NR_IRQ];
	unsigned int       ver;
};

static struct hi6xxx_pmic *pmic_dev = NULL;
static void __iomem *g_pmussi_baseaddr = NULL;

#if defined (CONFIG_HUAWEI_DSM)
static struct dsm_dev dsm_pmu_irq = {
	.name = "dsm_pmu_irq",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 1024,
};
static struct dsm_client *pmu_irq_dclient = NULL;
#endif

/*
 * This is debug if,in your code must not call it.
 */
static struct hi6xxx_pmic *hi6xxx_pmic_get_ctx(void)
{
	return pmic_dev;
}
EXPORT_SYMBOL(hi6xxx_pmic_get_ctx);


unsigned char hi6xxx_pmic_reg_read (unsigned int reg_addr)
{
    unsigned char regval;
    regval =  *(volatile unsigned char*)PMUSSI_REG(reg_addr);
    return regval;
}
EXPORT_SYMBOL(hi6xxx_pmic_reg_read);


void hi6xxx_pmic_reg_write (unsigned int reg_addr, unsigned char regval)
{
    /*
     *
     */
    *(volatile unsigned char*)PMUSSI_REG(reg_addr) = regval;
}
EXPORT_SYMBOL(hi6xxx_pmic_reg_write);

unsigned char hi6xxx_pmic_reg_read_ex (void *pmu_base, unsigned int reg_addr)
{
    unsigned char regval;
    regval =  *(volatile unsigned char*)PMUSSI_REG_EX(pmu_base, reg_addr);
    return regval;
}
EXPORT_SYMBOL(hi6xxx_pmic_reg_read_ex);

void hi6xxx_pmic_reg_write_ex (void *pmu_base, unsigned int reg_addr, unsigned char reg_val)
{
    *(volatile unsigned char*)PMUSSI_REG_EX(pmu_base, reg_addr) = reg_val;
}
EXPORT_SYMBOL(hi6xxx_pmic_reg_write_ex);

int hi6xxx_pmic_recv_data (unsigned int reg_addr, char *buffer, int len)
{
    int i;

    if ((len > 32) || (NULL == buffer)) {
        return -EINVAL;
    }

    /*
     * Here is a bug in the pmu die.
     * the coul driver will read 4 bytes,
     * but the ssi bus only read 1 byte, and the pmu die
     * will make sampling 1/10669us about vol cur,so the driver
     * read the data is not the same sampling
     */
    for (i = 0; i < len; i++) {
        *(buffer + i) = hi6xxx_pmic_reg_read(reg_addr+i);
    }

    return 0;
}
EXPORT_SYMBOL(hi6xxx_pmic_recv_data);

int hi6xxx_pmic_send_data (unsigned int reg_addr, char *buffer, int len)
{
    int i;

    if ((len > 32) || (NULL == buffer)) {
        return -EINVAL;
    }

    for (i = 0; i < len; i++) {
        hi6xxx_pmic_reg_write(reg_addr+i,*(buffer + i));
    }

    return 0;
}
EXPORT_SYMBOL(hi6xxx_pmic_send_data);


static struct of_device_id of_hi6xxx_pmic_child_match_tbl[] = {
	{
		.compatible = "hisilicon,hi6xxx-regulator-pmic",
	},
	{
		.compatible = "hisilicon,hisi-powerkey",
	},
       {
       	.compatible = "hisilicon,hi6xxx-usbvbus",
       },
       {
          	.compatible = "hisilicon,hi6xxx-coul",
       },
       {
        .compatible = "hisilicon,hi6552-pmu-rtc",
    },
    {
        .compatible = "hisilicon,hi6552-pmic-mntn",
    },
    { /* end */ }
};

static struct of_device_id of_hi6xxx_pmic_match_tbl[] = {
    /*
     *  the pmu driver can drive hi6551,hi6552,hi6553
     *  why not match id "6xxx-pmic-driver".
     *  beause we can tell difference between difference dtb or productor.
     */
	{
		.compatible = "hisilicon,hi6551-pmic-driver",
	},
	{
		.compatible = "hisilicon,hi6552-pmic-driver",
	},
	{
		.compatible = "hisilicon,hi6553-pmic-driver",
	},

	{ /* end */ }
};

unsigned int hi6xxx_pmic_get_version(void)
{
    unsigned int    uvalue = 0;
    uvalue = (unsigned int)hi6xxx_pmic_reg_read(HI6XXX_VER_REG);
    uvalue = uvalue & HI6XXX_REG_WIDTH;

    return uvalue;
}

static int hi6xxx_pmic_version_check(void)
{
    int ret = SSI_DEVICE_ERR;
    int ver = 0;

    ver = hi6xxx_pmic_get_version();
    if((ver >= PMU_VER_START) && (ver <= PMU_VER_END))
    {
        return SSI_DEVICE_OK;
    }

    return ret;
}

static void  hi6xxx_pmic_rstdis(void)
{
#if 0
    unsigned long sctrl_addr = (unsigned long)IO_ADDRESS(SOC_SC_ON_BASE_ADDR);
    *(volatile int *)SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_ADDR(sctrl_addr) = \
    	(1<<SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_1pmussi_START);
#endif
}

static void  hi6xxx_pmic_rsten(void)
{
#if 0
    unsigned long sctrl_addr = (unsigned long)IO_ADDRESS(SOC_SC_ON_BASE_ADDR);
    *(volatile int *)SOC_AO_SCTRL_SC_PERIPH_RSTEN4_ADDR(sctrl_addr) = \
    	(1<<SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_1pmussi_START);
#endif
}

static void of_fake_enable_clk(void)
{

	return;
}


static irqreturn_t hi6xxx_pmic_irq_handler(int irq, void *data)
{
	struct hi6xxx_pmic *pmic = (struct hi6xxx_pmic *)data;
	unsigned long pending;
	unsigned int ret = IRQ_NONE;
	int i, offset;
#if defined (CONFIG_HUAWEI_DSM)
	int dsm_irq_error_found = -1;

	unsigned long dsm_pmu_irq_mask[DSM_PMU_IRQ_COUNT] = {
		[0] = DSM_PMU_IRQ1_MASK,
		 [1] = DSM_PMU_IRQ2_MASK,
	};
#endif
	printk(KERN_INFO"%x %p********* in %s***********\n",(unsigned int)irq,data,__func__);
	for (i = 0; i < HI6XXX_IRQ_ARRAY; i++) {
		pending = hi6xxx_pmic_reg_read((i + HI6XXX_IRQ_STAT_BASE));
		pending &= HI6XXX_REG_WIDTH;
		if (pending != 0) {
			pr_debug("pending[%d]=0x%lx\n\r", i, pending);
		}

#if defined (CONFIG_HUAWEI_DSM)
		if (i < DSM_PMU_IRQ_COUNT && (pending &\
			dsm_pmu_irq_mask[i]) && (dsm_irq_error_found >= 0)) {
			dsm_client_record(pmu_irq_dclient, "pending irq[%d] 0x%x.\n", i,
				pending);
			dsm_irq_error_found++;
		}
#endif
		/* clear pmic-sub-interrupt */
		hi6xxx_pmic_reg_write((i + HI6XXX_IRQ_STAT_BASE), pending);

	    printk(KERN_INFO"========pending =========== %#x\n",(unsigned int)pending);
		if (pending) {
			for_each_set_bit(offset, &pending, HI6XXX_BITS)
				generic_handle_irq(pmic->irqs[offset + i * HI6XXX_BITS]);
			ret = IRQ_HANDLED;
		}
	}

#if defined (CONFIG_HUAWEI_DSM)
	if (dsm_irq_error_found > 0) {
		dsm_client_notify(pmu_irq_dclient, DSM_PMU_IRQ_ERROR_NO);
	} else if (!dsm_irq_error_found) {
		dsm_client_unocuppy(pmu_irq_dclient);
	} else {
		/* dsm irq_dclient ocuppy failed */
	}
#endif
	return ret;
}


static void hi6xxx_pmic_irq_mask(struct irq_data *d)
{

	//struct hi6xxx_pmic *pmic = irq_data_get_irq_chip_data(d);
	u32 data, offset;
    unsigned long pmic_spin_flag = 0;
	offset = ((irqd_to_hwirq(d) >> 3) + HI6XXX_IRQ_MASK_BASE);
    spin_lock_irqsave(&pmic_dev->ssi_hw_lock, pmic_spin_flag);
	data = hi6xxx_pmic_reg_read(offset);
	data |= (1 << (irqd_to_hwirq(d) & 0x07));
	hi6xxx_pmic_reg_write(offset, data);
    spin_unlock_irqrestore(&pmic_dev->ssi_hw_lock, pmic_spin_flag);

    printk(KERN_INFO"%p ********* in %s***********\n",d,__func__);

}

static void hi6xxx_pmic_irq_unmask(struct irq_data *d)
{
	//struct hi6xxx_pmic *pmic = irq_data_get_irq_chip_data(d);
	u32 data, offset;
    unsigned long pmic_spin_flag = 0;
	offset = ((irqd_to_hwirq(d) >> 3) + HI6XXX_IRQ_MASK_BASE);
    printk(KERN_INFO"(irqd_to_hwirq(d)>>3 %#x offset %#x\n",(unsigned int)(irqd_to_hwirq(d) >> 3), (unsigned int)offset);
    spin_lock_irqsave(&pmic_dev->ssi_hw_lock, pmic_spin_flag);
    printk(KERN_INFO"******spinlock in********\n");
	data = hi6xxx_pmic_reg_read(offset);
	data &= ~(1 << (irqd_to_hwirq(d) & 0x07));
	hi6xxx_pmic_reg_write(offset, data);
	printk(KERN_INFO"******spinlock out********\n");
    spin_unlock_irqrestore(&pmic_dev->ssi_hw_lock, pmic_spin_flag);
    printk(KERN_INFO"%p ********* in %s***********\n",d,__func__);

}


static struct irq_chip hi6xxx_pmic_irqchip = {
	.name		= "hisi-hi6xxx-pmic-irqchip",
	.irq_mask	= hi6xxx_pmic_irq_mask,
	.irq_unmask	= hi6xxx_pmic_irq_unmask,
};

static int hi6xxx_pmic_irq_map(struct irq_domain *d, unsigned int virq,
			  irq_hw_number_t hw)
{
	struct hi6xxx_pmic *pmic = d->host_data;
    printk(KERN_INFO"*********in %s***********\n",__func__);
	irq_set_chip_and_handler_name(virq, &hi6xxx_pmic_irqchip,
				      handle_simple_irq, "hisi-hi6xxx-pmic-irqchip");
	irq_set_chip_data(virq, pmic);
	irq_set_irq_type(virq, IRQ_TYPE_NONE);

	return 0;
}

static struct irq_domain_ops hi6xxx_domain_ops = {
	.map	= hi6xxx_pmic_irq_map,
	.xlate	= irq_domain_xlate_twocell,
};

static inline void hi6xxx_pmic_clear_int(void)
{
    int addr;
	for(addr = HI6XXX_IRQ_STAT_BASE; addr < (HI6XXX_IRQ_STAT_BASE + HI6XXX_IRQ_ARRAY); addr++)
	{
        hi6xxx_pmic_reg_write(addr, HI6XXX_IRQ_CLR);
	}

}

static inline void hi6xxx_pmic_mask_int(void)
{
    int addr;
	for(addr = HI6XXX_IRQ_MASK_BASE; addr < (HI6XXX_IRQ_MASK_BASE + HI6XXX_IRQ_ARRAY); addr++)
	{
        hi6xxx_pmic_reg_write(addr, HI6XXX_IRQ_MASK);
	}

}

static int   hi6xxx_pmic_probe(struct platform_device *pdev)
{
    int i = 0;
    int ret = 0 ;
    int dev_stat = 0;
    unsigned int virq = 0;
    int pmu_on = 1;
    enum of_gpio_flags gpio_flags;

	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct hi6xxx_pmic *pmic = NULL;

    /*
     * this is new feature in kernel 3.10
     */
	pmic = devm_kzalloc(dev, sizeof(*pmic), GFP_KERNEL);
	if (!pmic) {
		printk("cannot allocate hi6xxx_pmic device info\n");
		return -ENOMEM;
	}
    pmic_dev = pmic;

	printk(KERN_INFO"===============[ydb new in %s : 1]=============\n",__func__);


    /*
     * init spin lock
     */
	spin_lock_init(&pmic->ssi_hw_lock);

	/*
	 * get resources
	 */
	pmic->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!pmic->res) {
		printk("platform_get_resource err\n");
		return -ENOENT;
	}
	if (!devm_request_mem_region(dev, pmic->res->start,
				     resource_size(pmic->res),
				     pdev->name)) {
		printk("cannot claim register memory\n");
		return -ENOMEM;
	}
	g_pmussi_baseaddr = ioremap(pmic->res->start,
				  resource_size(pmic->res));
	if (!g_pmussi_baseaddr) {
		printk("cannot map register memory\n");
		return -ENOMEM;
	}

    printk(KERN_INFO"virt[%p] VS phy[%#x]\n",g_pmussi_baseaddr,(unsigned int)pmic->res->start);
	printk(KERN_INFO"===============[in %s : 2]=============\n",__func__);


    /*
     * confirm pmu is exist&effective
     */
    pmic->ver = hi6xxx_pmic_get_version();
    if((pmic->ver < PMU_VER_START) || (pmic->ver > PMU_VER_END))
    {
		printk("it is wrong pmu version\n");
		pmu_on = 0;
    }
    printk(KERN_INFO"===============[in %#x : 55]=============\n",(unsigned int)pmic->ver);

    /*规避代码，CODEC上电就上报中断，这里先将所有CODEC中断屏蔽*/
    hi6xxx_pmic_reg_write(0x1b5,0xff);

   /*
    * must finish the gpio&irq is function
    */
    pmic->gpio = of_get_gpio_by_prop(np,"pmu_irq_gpio",0,0, &gpio_flags);
	if (pmic->gpio < 0){
		printk("failed to request gpio %d\n", pmic->gpio);
		return pmic->gpio;
	}
	if (!gpio_is_valid(pmic->gpio)){
		printk("it is invalid gpio %d\n", pmic->gpio);
		return -EINVAL;
	}
	ret = gpio_request_one(pmic->gpio, GPIOF_IN, "hi6xxx_pmic_irq");
	if (ret < 0) {
		printk("failed to request gpio %d\n", pmic->gpio);
		return ret;
	}
	pmic->irq = gpio_to_irq(pmic->gpio);

	printk(KERN_INFO"=======pmic->irq %d========\n",(int)pmic->irq);

    /*
     * clear PMIC sub-interrupt
     */
     hi6xxx_pmic_clear_int();

    /*
     * mask PMIC sub-interrupt
     */
     hi6xxx_pmic_mask_int();

    /*
     * register irq domain
     */
	pmic->domain = irq_domain_add_simple(np, HI6XXX_NR_IRQ, 0,
					     &hi6xxx_domain_ops, pmic);
	if (!pmic->domain) {
		printk( "in %s failed irq domain add simple!\n",__func__);
		ret = -ENODEV;
		return ret;
	}

    /*
     * here call map function
     */
	for (i = 0; i < HI6XXX_NR_IRQ; i++) {
		virq = irq_create_mapping(pmic->domain, i);
		if (0 == virq) {
			printk("Failed mapping hwirq\n");
			ret = -ENOSPC;
			return ret;
		}
		printk(KERN_INFO"virq %d\n",(int)virq);
		pmic->irqs[i] = virq;
	}
	printk(KERN_INFO"===============[in  mid %s : 2]=============\n",__func__);

    /*
     *  We must make sure the GPIO status which is high.
     */
    if(pmu_on)
    {
    	ret = request_threaded_irq(pmic->irq, hi6xxx_pmic_irq_handler, NULL,
    				IRQF_TRIGGER_LOW | IRQF_SHARED | IRQF_NO_SUSPEND,
    				   "hi6xxx-pmic-irq", pmic);
    	if (ret < 0) {
    		printk("*************could not claim pmic %d\n", ret);
    		ret = -ENODEV;
    		return ret;
     	}
 	}
	printk(KERN_INFO"===============[in %s : 2]=============\n",__func__);

    /*
	pmic->clk = clk_get(dev,"pmic_clk");
	if (likely(pmic->clk)){
		ret = of_fake_enable_clk();
        if (ret) {
            printk("%s:failed to enable clock\n", __func__);
            return -EINVAL;
        }

	}
	*/
	of_fake_enable_clk();

    pmic->dev = dev;

    /*
     * this ops at fastboot,now only stub
     */
    hi6xxx_pmic_rstdis();

    /*
     * bind pmic to device
     */
	platform_set_drvdata(pdev, pmic);

    /*
     * set pmu defualt vol
     * on sft platform there is no set defualt vol
     */

	printk(KERN_INFO"===============[in %s : 3]=============\n",__func__);

	/*
	 * populate sub nodes
	 */
	of_platform_populate(np, of_hi6xxx_pmic_child_match_tbl, NULL, dev);

	printk(KERN_INFO"===============[in %s : 4]=============\n",__func__);

    dev_stat = hi6xxx_pmic_version_check();
	ret = hi6xxx_pmic_device_stat_notify("PMU", dev_stat);
	if(ret < 0)
	{
        printk("*************pmu dev notify err\n");
	}
#if defined (CONFIG_HUAWEI_DSM)
    if (!pmu_irq_dclient) {
        pmu_irq_dclient = dsm_register_client(&dsm_pmu_irq);
    }
#endif

	return 0;
}

/*TODO:In V8R1 this function must finish*/
static int hi6xxx_pmic_remove(struct platform_device *pdev)
{
    hi6xxx_pmic_rsten();
	return 0;
}
#ifdef CONFIG_PM
static int hi6xxx_pmic_suspend(struct platform_device *pdev, pm_message_t pm)
{
    printk(KERN_INFO"[%s],+\n", __func__);
    printk(KERN_INFO"[%s],-\n", __func__);

    return 0;
}

static int hi6xxx_pmic_resume(struct platform_device *pdev)
{
    printk(KERN_INFO"[%s],+\n", __func__);
    printk(KERN_INFO"[%s],-\n", __func__);

    return 0;
}

#endif

static struct platform_driver pmic_driver = {
	.driver	= {
		.name =	"hisi,hi6xxx-pmic",
		.owner = THIS_MODULE,
		.of_match_table = of_hi6xxx_pmic_match_tbl,
	},
	.probe  = hi6xxx_pmic_probe,
	.remove	= hi6xxx_pmic_remove,
#ifdef CONFIG_PM
	.suspend	= hi6xxx_pmic_suspend,
	.resume		= hi6xxx_pmic_resume,
#endif
};
/*
 * init the module.
 */
static int __init hi6xxx_pmic_init(void)
{
	int	ret = 0;
	printk(KERN_INFO"===============[in %s ]=============\n",__func__);

	ret	= platform_driver_register(&pmic_driver);
	if (ret) {
		printk("%s:	platform_driver_register failed %d\n",
				__func__, ret);
	}

	return ret;
}

static void	__exit hi6xxx_pmic_exit(void)
{
#if 0 //def CONFIG_DEBUG_FS
    driver_remove_file(&(pmic_driver.driver), &driver_attr_pmic_regaddr);
    driver_remove_file(&(pmic_driver.driver), &driver_attr_pmic_regval);
#endif
	platform_driver_unregister(&pmic_driver);
}

/*there are lots of init functions like of regulator bus*/
fs_initcall(hi6xxx_pmic_init);
module_exit(hi6xxx_pmic_exit);

MODULE_AUTHOR("Dongbin Yu<yudongbin@huawei.com>");
MODULE_DESCRIPTION("HI6xxx PMU SSI interface driver");
MODULE_LICENSE("GPL V2");
