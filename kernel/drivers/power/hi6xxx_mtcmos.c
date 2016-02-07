/*
 * Device driver for PMU DRIVER	 in HI6xxx SOC
 *
 * Copyright (c) 2011 Hisilicon Co. Ltd
 *
 * Dongbin Yu <yudongbin@huawei.com> : 2014-2-7
 *
 * In V8R1 the mtcmos is same with V9R1's driver,so it is multiplex.
 * That the reason why name of the c document is "hi6xxx".
 *
 * I only code the mtcmos arch,there are some thing which not finished because
 * of the litter time.In process of iterative development you must add some
 * function into the module.
 *
 *
 */
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/of_platform.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/power/hi6xxx_mtcmos.h>
#include <linux/printk.h>
#include "soc_baseaddr_interface.h"
#include "soc_ao_sctrl_interface.h"

/*lint -emacro(750,*)*/
#define MTCMOS_REG(regu_type,reg_addr)        (void *)(reg_addr+((regu_type == MTCMOS_SC_ON_TYPE)? \
                                                       ((char *)mtcmos_dev->sc_on_regs) \
                                                       :((char *)mtcmos_dev->acpu_on_regs)))
/*lint +emacro(750,*)*/
#ifdef _DRV_LLT_
#define static 
#define udelay(n)	(n = n)
#endif
	
struct mtcmos_pwup_time {
    unsigned int pwup_time_reg;
    unsigned int pwup_time_shift;
    unsigned int pwup_time_mask;
};


struct hi6xxx_mtcmos{
	struct device	*dev;
    void __iomem	*sc_on_regs;
    void __iomem	*acpu_on_regs;    
	spinlock_t      mtcmos_spin_lock;
	int             steady_time;
	int             mtcmos_n_pwup_time;
};

static struct hi6xxx_mtcmos* mtcmos_dev = NULL;

static struct of_device_id of_hi6xxx_mtcmos_child_match_tbl[] = {
	/* regulators */
	{
		.compatible = "hisilicon,hi6xxx-regulator-mtcmos",
	},
	{ /* end */ }
};

static struct of_device_id of_hi6xxx_mtcmos_match_tbl[] = {
	{
		.compatible = "hisilicon,hi6xxx-mtcmos-driver",
	},
	{ /* end */ }
};

static struct hi6xxx_mtcmos* hi6xxx_mtcmos_get_ctx(void)
{
    return mtcmos_dev;
}
EXPORT_SYMBOL(hi6xxx_mtcmos_get_ctx);

/*
 * return value: off 0   on 1
 */
static int hi6xxx_mtcmos_is_on(struct hi6xxx_regulator_ctrl_regs  *regs,
                               unsigned int mask,int shift,int regu_type)
{
    unsigned int ret;
    ret = readl(MTCMOS_REG(regu_type,regs->status_reg));
    ret &= (mask << shift);
    return !!ret;
}

int hi6xxx_mtcmos_on(struct hi6xxx_regulator_ctrl_regs  *regs,
                               unsigned int mask,int shift,int regu_type)
{
    unsigned long mtcmos_spin_flag = 0;
    spin_lock_irqsave(&mtcmos_dev->mtcmos_spin_lock, mtcmos_spin_flag);
    writel(mask<<shift,MTCMOS_REG(regu_type,regs->enable_reg));
    udelay(mtcmos_dev->steady_time);

    if (0 == hi6xxx_mtcmos_is_on(regs,mask,shift,regu_type)) {
	udelay(mtcmos_dev->steady_time);
	if (0 == hi6xxx_mtcmos_is_on(regs,mask,shift,regu_type)) {
		printk("in %s hi6xxx_mtcmos_is_on is failed\n",__func__);
		spin_unlock_irqrestore(&mtcmos_dev->mtcmos_spin_lock, mtcmos_spin_flag);
		return -1;
	}
    }

    spin_unlock_irqrestore(&mtcmos_dev->mtcmos_spin_lock, mtcmos_spin_flag);
    return 0;
}
EXPORT_SYMBOL(hi6xxx_mtcmos_on);

int hi6xxx_mtcmos_off(struct hi6xxx_regulator_ctrl_regs  *regs,
                               unsigned int mask,int shift,int regu_type)
{
    unsigned long mtcmos_spin_flag = 0;
    spin_lock_irqsave(&mtcmos_dev->mtcmos_spin_lock, mtcmos_spin_flag);
    writel(mask << shift,MTCMOS_REG(regu_type,regs->disable_reg));
    udelay(mtcmos_dev->steady_time);
    spin_unlock_irqrestore(&mtcmos_dev->mtcmos_spin_lock, mtcmos_spin_flag);

    return 0;
}
EXPORT_SYMBOL(hi6xxx_mtcmos_off);

int hi6xxx_mtcmos_get_status(struct hi6xxx_regulator_ctrl_regs  *regs,
                               unsigned int mask,int shift,int regu_type)
{
    int ret=0;
    unsigned long mtcmos_spin_flag = 0;
    spin_lock_irqsave(&mtcmos_dev->mtcmos_spin_lock, mtcmos_spin_flag);
    ret = hi6xxx_mtcmos_is_on(regs,mask,shift,regu_type);
    spin_unlock_irqrestore(&mtcmos_dev->mtcmos_spin_lock, mtcmos_spin_flag);

    return ret;
}
EXPORT_SYMBOL(hi6xxx_mtcmos_get_status);

static int hi6xxx_mtcmos_probe(struct platform_device *pdev)
{
//    int id;
    int ret;
    const __be32 *sc_on_regs, *acpu_on_regs;
//    unsigned int addr;
 //   unsigned int val;
	struct hi6xxx_mtcmos *mtcmos = NULL;
//	unsigned int pwerup_time_table[3] = {};
//    struct mtcmos_pwup_time* pwup_time = NULL;
 	struct device *dev = NULL;
	struct device_node *np = NULL;

    if(pdev == NULL){
		printk(KERN_ERR "---------pdev------------ null\n");
		return -1;
    }
 	dev = &pdev->dev;

	if(dev == NULL){
		printk(KERN_ERR "---------dev------------ null\n");
		return -1;
    }
	np = dev->of_node;

    pr_info("----------------new [%s 1 ]---------------\n",__FUNCTION__);

    /*this is new feature in kernel 3.10*/
	mtcmos = devm_kzalloc(dev, sizeof(*mtcmos), GFP_KERNEL);
	if (!mtcmos) {
		printk("cannot allocate hi6xxx_mtcmos device info\n");
		return -ENOMEM;
	}

    mtcmos->dev = dev;
    mtcmos_dev = mtcmos;

	ret = of_property_read_u32(np, "hisilicon,mtcmos-steady-us",
						&mtcmos->steady_time);
	if (ret) {
		printk("of_property_read_u32 hisilicon,mtcmos-steady-us  fail\n");
		return -EINVAL;
	}
    pr_info("----------------[%s 2 ]---------------\n",__FUNCTION__);

    spin_lock_init((spinlock_t *)&mtcmos->mtcmos_spin_lock);


	ret = of_property_read_u32(np, "hisilicon,mtcmos-nn-time-numb",
						&mtcmos->mtcmos_n_pwup_time);
	if (ret) {
		printk("of_property_read_u32 hisilicon,mtcmos-nn-time-numb fail\n");
		return -EINVAL;
	}

    sc_on_regs = of_get_property(np, "hisilicon,mtcmos-sc-on-base", NULL);
    if (sc_on_regs)
		mtcmos->sc_on_regs = ioremap(be32_to_cpu(*sc_on_regs),0x1000);

    acpu_on_regs = of_get_property(np, "hisilicon,mtcmos-acpu-on-base", NULL);
    if (acpu_on_regs)
		mtcmos->acpu_on_regs = ioremap(be32_to_cpu(*acpu_on_regs),0x1000);

	pr_info("sc_on_regs [%p]VS[%p]    acpu_on_regs [%p] VS [%p]\n ",
	                   mtcmos->sc_on_regs,sc_on_regs,
	                   mtcmos->acpu_on_regs,acpu_on_regs);
    /*
     * bind pmic to dev
     */
	platform_set_drvdata(pdev, mtcmos);
	
    /* 
     * set mtcmos  defualt power uptime
     */
#if 0
	pwup_time = (struct mtcmos_pwup_time*)(&pwerup_time_table[0]);
    printk("----------------[%s 3 ]---------------\n",__FUNCTION__);
    for(id = 0; id < mtcmos->mtcmos_n_pwup_time; id++){
       if(!!(of_find_property(np, "regulator-always-on", NULL))){
           (void)of_property_read_u32_planar_array(np,"hisilicon,mtcmos-pwer-time",
                                                   &pwerup_time_table[0],3,id);
            printk("pwup_time_reg = %x \n pwup_time_mask = %x \n pwup_time_shift = %x \n",
                   pwup_time->pwup_time_reg,pwup_time->pwup_time_mask,pwup_time->pwup_time_shift);
            addr = (unsigned int)mtcmos->regs + pwup_time->pwup_time_reg;
            val = readl((void *)addr);
            val &= (~pwup_time->pwup_time_mask);
            val |= (MTCMOS_PWON_TIME_COUNTER << pwup_time->pwup_time_shift);
            writel(val, (void *)addr);
        }
    }
#endif
    pr_info("----------------[%s 4 ]---------------\n",__FUNCTION__);

	/* 
	 * populate sub nodes
	 */
	of_platform_populate(np, of_hi6xxx_mtcmos_child_match_tbl, NULL, dev);

    pr_info("----------------[%s 5 ]---------------\n",__FUNCTION__);

	return 0;
}

static struct platform_driver mtcmos_driver = {
	.driver = {
		.name	= "hisi_hi6xxx_mtcmos",
		.owner  = THIS_MODULE,
		.of_match_table = of_hi6xxx_mtcmos_match_tbl,
	},
	.probe	= hi6xxx_mtcmos_probe,
};

static int __init hi6xxx_mtcmos_init(void)
{
    pr_info("----------------[%s]---------------\n",__FUNCTION__);
    return platform_driver_register(&mtcmos_driver);
}

static void __exit hi6xxx_mtcmos_exit(void)
{
	platform_driver_unregister(&mtcmos_driver);
}

fs_initcall(hi6xxx_mtcmos_init);
module_exit(hi6xxx_mtcmos_exit);

MODULE_AUTHOR("Dongbin Yu<yudongbin@huawei.com>");
MODULE_DESCRIPTION("HI6xxx MTCMOS interface driver");
MODULE_LICENSE("GPL V2");
