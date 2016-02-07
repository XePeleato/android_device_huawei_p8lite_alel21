#include <linux/module.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/syscore_ops.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/debugfs.h>
#include <asm/io.h>
#include <linux/string.h>
#include <linux/clk.h>

#ifdef CONFIG_HISI_NOC_HI6220_PLATFORM
#include "soc_peri_sctrl_interface.h"
#include "soc_baseaddr_interface.h"
#include "global_ddr_map.h"
//#include <BSP.h>
#endif
#include "hisi_noc.h"
#include "hisi_err_probe.h"
#include "hisi_noc_packet.h"
#include "hisi_noc_bus.h"
#include "hisi_noc_transcation.h"

static struct noc_node *hi6220_nodes_array[HI6220_MAX_NODES_NR] = {NULL};
static struct hisi_noc_irqd hi6220_noc_irqdata[HI6220_NOC_MAX_IRQ_NR];
static struct noc_node *hi3xxx_nodes_array[HI3XXX_MAX_NODES_NR] = {NULL};
static struct hisi_noc_irqd hi3xxx_noc_irqdata[HI3XXX_NOC_MAX_IRQ_NR];

static unsigned int nodes_array_idx = 0;
void __iomem *error_logger = NULL;
void __iomem *error_logger_media = NULL;

static struct dentry *noc_root;
static struct dentry *u32_packet_timer_period;
static struct dentry *u32_packet_stastic_times;
static struct dentry *u32_packet_f0_master_id;
static struct dentry *u32_packet_f1_master_id;
static struct dentry *packet_on;
static struct dentry *packet_off;
static struct dentry *packet_data;

static struct noc_statstic_data *noc_statstic_datas=NULL;

static struct hrtimer hr_timer;
static unsigned int timer_irq_counter =0;
static unsigned int timer_cnt_max =10;
static unsigned int timer_cnt_max_of_file =10;
static unsigned int timer_period =200; //200ms
static unsigned int f0_master_id =0x7;//mcu
static unsigned int f1_master_id =0x5;//hifi

const struct hisi_noc_data *g_config_hisi_noc_data = 0;

static unsigned int g_packet_enable = 0;
static unsigned int g_transcation_enable = 0;
static unsigned int g_error_enable = 0;
static unsigned int g_noc_debug = 0;
static unsigned int g_noc_timeout_enable = 0;

//Partland platform
static const struct hisi_noc_data hisi6220_noc_data = {
	.platform_id=HISI_NOC_HI6220
};

//Seattle platform
static const struct hisi_noc_data hisi3635_noc_data = {
	.platform_id=HISI_NOC_HI3635
};

// Austin platform
static const struct hisi_noc_data hisi3650_noc_data = {
	.platform_id=HISI_NOC_HI3650
};

static struct of_device_id hisi_noc_match[] = {
	{ .compatible = "hisilicon,hi6220-noc",
	   .data=&hisi6220_noc_data,
	},
	{ .compatible = "hisilicon,hi3635-noc" ,
	   .data=&hisi3635_noc_data,
	},
	{ .compatible = "hisilicon,hi3650-noc" ,
	   .data=&hisi3650_noc_data,
    },
    { /* end */ }
};



extern void bsp_modem_error_handler(u32  p1,  void* p2, void* p3, void* p4);
extern void regulator_address_info_list(void);

static irqreturn_t hisi_noc_timeout_irq_handler(int irq, void *data)
{
	void __iomem *pmctrl_base;
	struct hisi_noc_device *noc_dev = (struct hisi_noc_device *)data;
	unsigned int pending;
	pmctrl_base = noc_dev->pmctrl_base;
	pending = readl_relaxed(pmctrl_base + PMCTRL_PERI_INT_STAT0_OFFSET);
	if (pending)
		pr_err("NoC timeout IRQ occured, PERI_INT_STAT0 = 0x%x\n", pending);
	else
		pr_err("PMCTRL other interrupt source occurs! \n");

	writel_relaxed(pending, pmctrl_base + PMCTRL_PERI_INT_MASK_OFFSET);

	return IRQ_HANDLED;
}

static irqreturn_t hisi_noc_irq_handler(int irq, void *data)
{
       unsigned long pending;
	int offset;
	void __iomem *pctrl_base;
	struct noc_node *node=NULL;
	void __iomem *porbe_base=NULL;

	struct hisi_noc_device *noc_dev = (struct hisi_noc_device *)data;
	pctrl_base = noc_dev->pctrl_base;

#if defined(CONFIG_HISI_NOC_HI3635_PLATFORM) ||defined(CONFIG_HISI_NOC_HI3650_PLATFORM)
#ifdef CONFIG_HISI_BALONG_MODEM_HI3XXX
    bsp_modem_error_handler(0,(void*)0,(void*)0,(void*)0);
#endif
#endif

#ifdef CONFIG_HISI_NOC_HI3635_PLATFORM
    regulator_address_info_list();
#endif

    if (1 == g_noc_debug)
        pr_err("Get into hisi_noc_irq_handler.\n");


	pending = readl_relaxed(pctrl_base + PCTRL_NOC_IRQ_STAT2);
    	pending = (pending << 32) | readl_relaxed(pctrl_base + PCTRL_NOC_IRQ_STAT1);
   	pending = pending & HI3XXX_NOC_PCTRL_IRQ_MASK;
	if (pending) {
		for_each_set_bit(offset, &pending, BITS_PER_LONG) {
			if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
				node = hi3xxx_noc_irqdata[offset].node;
			if(NULL == node){
				pr_err("node is null pointer !\n");
				return IRQ_HANDLED;
			}
			porbe_base = node->eprobe_offset + node->base;

			switch (hi3xxx_noc_irqdata[offset].type) {
			case NOC_ERR_PROBE_IRQ:
	                if (g_error_enable)
	                {
    				pr_err("NoC Error Probe:\n");
				pr_err("noc_bus: %s\n", hi3xxx_noc_buses_info[node->bus_id].name);
    				pr_err("noc_node: %s\n", node->name);

    				hi3xxx_noc_err_probe_hanlder(porbe_base, node->bus_id);
 	               }
				break;
			case NOC_PACKET_PROBE_IRQ:
	                if (g_error_enable)
	                {
	    				/* FixMe: packet probe irq handler */
	    				pr_err("NoC PACKET Probe:\n");
	    				pr_err("noc_bus: %s\n", hi3xxx_noc_buses_info[node->bus_id].name);
	    				pr_err("noc_node: %s\n", node->name);

	    				noc_packet_probe_hanlder(node, porbe_base);
	                }
			break;
			case NOC_TRANS_PROBE_IRQ:
	                if (g_error_enable)
	                {
	    				/* FixMe: trans probe irq handler */
	    				pr_err("NoC TRANSCATION Probe:\n");
	    				pr_err("noc_bus: %s\n", hi3xxx_noc_buses_info[node->bus_id].name);
	    				pr_err("noc_node: %s\n", node->name);

	    				noc_transcation_probe_hanlder(node, porbe_base, node->bus_id);
	                }
				break;
			default:
				pr_err("NoC IRQ type is wrong!\n");

			}
		}
	}

	return IRQ_HANDLED;
}
static void *noc_seq_start(struct seq_file *m, loff_t *pos)
{
    if (*pos >= timer_cnt_max)
        return NULL;
    return pos;
}

static void *noc_seq_next(struct seq_file *m, void *v, loff_t *pos)
{
    (*pos)++;
    if (*pos >= timer_cnt_max)
        return NULL;
    return pos;
}

static void noc_seq_stop(struct seq_file *m, void *v)
{

}

static int noc_seq_show(struct seq_file *m, void *v)
{
    int i;
    u64 sBeginTime;
    unsigned long nsBeginTime;
    i = *(int *)v;
    pr_info("%s,i=%d\n",__func__,i);

    if(i==0)
    {
       seq_printf(m, "time_second counter3 counter2 counter1 counter0\n");
    }

    if (noc_statstic_datas != NULL)
    {
      sBeginTime = noc_statstic_datas[i].time;
      nsBeginTime = do_div(sBeginTime, 1000000000);
      seq_printf(m, "%05lu.%06lu  %u  %u  %u %u\n",
      (unsigned long)sBeginTime,
      nsBeginTime/1000,
      noc_statstic_datas[i].counter3,
      noc_statstic_datas[i].counter2,
      noc_statstic_datas[i].counter1,
      noc_statstic_datas[i].counter0
      );
    }
    return 0;
}

static struct seq_operations packet_data_seq_ops = {
    .start = noc_seq_start,
    .next  = noc_seq_next,
    .stop  = noc_seq_stop,
    .show  = noc_seq_show
};
static int noc_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &packet_data_seq_ops);
}

static const struct file_operations packet_data_fops = {
    .open = noc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};
static enum hrtimer_restart hrtimer_callback(struct hrtimer *timer)
{

   if((timer_irq_counter < timer_cnt_max)&&(timer_cnt_max <= TIMES_MAX))
   {
      if(timer_period < TIMER_PERIOD_MAX)
      {
          hrtimer_start(timer,ktime_set(timer_period/1000,(timer_period%1000)*1000000),HRTIMER_MODE_REL);
      }

      if (noc_statstic_datas != NULL)
      {
          noc_statstic_datas[timer_irq_counter].time = sched_clock();
          noc_clear_bit(error_logger+0x2000, PACKET_CFGCTL, 0);
          noc_statstic_datas[timer_irq_counter].counter3 = readl(error_logger+0x2000 + PACKET_COUNTERS_3_VAL);
          noc_statstic_datas[timer_irq_counter].counter2 = readl(error_logger+0x2000+ PACKET_COUNTERS_2_VAL);
          noc_statstic_datas[timer_irq_counter].counter1 = readl(error_logger+0x2000 + PACKET_COUNTERS_1_VAL);
          noc_statstic_datas[timer_irq_counter].counter0 = readl(error_logger+0x2000 + PACKET_COUNTERS_0_VAL);
          noc_clear_bit(error_logger+0x2000, PACKET_MAINCTL, 3);
	      noc_set_bit(error_logger+0x2000, PACKET_MAINCTL, 3);
	      noc_set_bit(error_logger+0x2000, PACKET_CFGCTL, 0);
       }
   }
   timer_irq_counter++;
   return HRTIMER_NORESTART;
}

static ssize_t packet_off_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
   pr_info("%s +.\n", __func__);
   vfree(noc_statstic_datas);
   noc_statstic_datas =NULL;
   return (ssize_t)cnt;
}

static const struct file_operations packet_off_fops = {
    .write = packet_off_write,
};

static ssize_t packet_on_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
    timer_irq_counter = 0;

    pr_info("%s +.\n", __func__);
    /*memmory for recode noc stastic data*/

    if(timer_cnt_max > TIMES_MAX)
    {
        printk(KERN_ERR"%s : timer_cnt_max too big \n", __FUNCTION__);
        return -1;
    }

    if(noc_statstic_datas == NULL)
    {
        timer_cnt_max = timer_cnt_max_of_file;
        noc_statstic_datas = (struct noc_statstic_data *)vmalloc((timer_cnt_max+1)*sizeof(struct noc_statstic_data));
        if (noc_statstic_datas ==NULL)
        {
           printk(KERN_ERR"%s : vmalloc failed \n", __FUNCTION__);
           return -1;
        }
        memset(noc_statstic_datas,0,((timer_cnt_max+1)*sizeof(struct noc_statstic_data)));
    }

    /*select master*/
    if(error_logger != NULL)
    {
        noc_clear_bit(error_logger+0x2000, PACKET_CFGCTL, 0);
        noc_clear_bit(error_logger+0x2000, PACKET_MAINCTL, 3);
	    noc_set_bit(error_logger+0x2000, PACKET_MAINCTL, 3);

        writel((f0_master_id &0xf)<<14,error_logger+0x2000 + PACKET_F0_ROUTEIDBASE);
        writel((f1_master_id &0xf)<<14,error_logger+0x2000 + PACKET_F1_ROUTEIDBASE);
        noc_set_bit(error_logger+0x2000, PACKET_CFGCTL, 0);
     }

    if(timer_period <= TIMER_PERIOD_MAX)
    {
        hrtimer_start(&hr_timer,ktime_set(timer_period/1000,(timer_period%1000)*1000000),HRTIMER_MODE_REL);
    }
    else
    {
        printk(KERN_ERR"%s : timer_period is too big \n", __FUNCTION__);
        return -1;
    }

    return (ssize_t)cnt;
}

static const struct file_operations packet_on_fops = {
    .write = packet_on_write,
};

static irqreturn_t sysnoc_err_irq_handler(int irq, void *data)
{
    hi6220_noc_err_probe_hanlder(hi6220_noc_irqdata[0].node->base);
    return IRQ_HANDLED;
}

static irqreturn_t media_err_irq_handler(int irq, void *data)
{
    hi6220_noc_err_probe_hanlder(hi6220_noc_irqdata[1].node->base);
    return IRQ_HANDLED;
}

static irqreturn_t sysnoc_alarm_irq_handler(int irq, void *data)
{
    noc_packet_probe_hanlder(hi6220_noc_irqdata[2].node, hi6220_noc_irqdata[2].node->base);
    return IRQ_HANDLED;
}

static irqreturn_t media_alarm_irq_handler(int irq, void *data)
{
    noc_packet_probe_hanlder(hi6220_noc_irqdata[3].node,hi6220_noc_irqdata[3].node->base);
	return IRQ_HANDLED;
}
#ifdef CONFIG_HISI_NOC_HI6220_PLATFORM
/*****************************************************************************
 º¯ Êý Ãû  : platform_ddr_protect_forhifi_init
 ¹¦ÄÜÃèÊö  : ³õÊ¼»¯ddr ±£»¤,ÉèÖÃ²»ÔÊÐíhifi·ÃÎÊµÄÇøÓò,²¢¿ªÆô±£»¤
 ÊäÈë²ÎÊý  : void
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : 0: ³õÊ¼»¯³É¹¦
             -1: ³õÊ¼»¯Ê§°Ü
 µ÷ÓÃº¯Êý  :
 ±»µ÷º¯Êý  :

 ÐÞ¸ÄÀúÊ·      :
  1.ÈÕ    ÆÚ   : 2014Äê1ÔÂ21ÈÕ
    ×÷    Õß   : Â¬ÑåÊ¤ 00212112
    ÐÞ¸ÄÄÚÈÝ   : ÐÂÉú³Éº¯Êý

*****************************************************************************/
static int platform_ddr_protect_forhifi_init(void)
{

    unsigned int    uaddr_start = 0;
    unsigned int    uaddr_end = 0;
    unsigned int    ureg_value = 0;
    void __iomem *  sctrl_base = NULL;

    sctrl_base = (void __iomem *)ioremap(SOC_PERI_SCTRL_BASE_ADDR, SZ_8K);
    /*start addr and end addr of ddr to protect setting
    unit is M bytes*/

    /*soc only check high 12 bits,so we have to set like this:*/
    /*the first section*/
    uaddr_start = 0;
    uaddr_end = ((HIFI_SYS_MEM_ADDR & 0xfff00000) - 0x100000) >> 20;

    ureg_value = (uaddr_start | (uaddr_end << SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_end_addr0_START));
    writel(ureg_value, SOC_PERI_SCTRL_SC_HIFI_CTRL3_ADDR(sctrl_base));

    /*the second section*/
    uaddr_start = ((HIFI_SYS_MEM_ADDR + HIFI_SYS_MEM_SIZE) & 0xfff00000) >> 20;
    uaddr_end = (((MCU_SYS_MEM_ADDR + MCU_SYS_MEM_SIZE) & 0xfff00000) - 0x100000) >> 20;

    ureg_value = 0;
    ureg_value = (uaddr_start | (uaddr_end << SOC_PERI_SCTRL_SC_HIFI_CTRL4_hifi_access_end_addr1_START));
    writel(ureg_value, SOC_PERI_SCTRL_SC_HIFI_CTRL4_ADDR(sctrl_base));

    /*enable*/
    ureg_value = readl(SOC_PERI_SCTRL_SC_HIFI_CTRL3_ADDR(sctrl_base));
    ureg_value = ureg_value
                    | BIT(SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_permit_START)
                            | BIT(SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_en_START);
    writel(ureg_value, SOC_PERI_SCTRL_SC_HIFI_CTRL3_ADDR(sctrl_base));
    iounmap(sctrl_base);

    return 0;
}
#endif
/*****************************************************************************
 Description : set sys noc for dfs.
  History
  1.Date: 2014/5/27
    Author : l00212112
    Modification : Created function
*****************************************************************************/
static  int  sysnoc_set_for_dfs(void)
{
    void __iomem * logger_ao = NULL;
    void __iomem * logger = NULL;
    void __iomem * logger_dma = NULL;

    logger_ao = (void __iomem *)ioremap(SOC_SYSNOC_SERVICE_AO_BASE_ADDR, SZ_4K);
    logger = (void __iomem *)ioremap(SOC_SYSNOC_SERVICE_BASE_ADDR, SZ_4K);
    logger_dma = (void __iomem *)ioremap(SOC_SYSNOC_SERVICE_DMA_BASE_ADDR, SZ_4K);

    /*sys noc*/
    writel(0x0, logger_ao+0x10C);
    writel(0x0404, logger_ao+0x108);
    writel(0x0505, logger_ao+0x208);
    writel(0x0, logger_ao+0x20C);

    writel(0x1, logger+0x20C);
    writel(0x0404, logger+0x208);
    writel(0x140, logger+0x210);
    writel(0x10, logger+0x214);
    writel(0x202, logger+0x308);
    writel(0x202, logger+0x408);
    writel(0x202, logger+0x508);
    writel(0x202, logger+0x608);

    writel(0x202, logger_dma+0x108);
    writel(0x202, logger_dma+0x208);
    writel(0x202, logger_dma+0x308);
    writel(0x202, logger_dma+0x408);

    pr_info("sysnoc_set_for_dfs end...");

    iounmap(logger_ao);
    iounmap(logger);
    iounmap(logger_dma);

    return 0;
}

static int find_bus_id_by_name(const char *bus_name)
{
	int i;
	int temp = 0;

    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
        temp = HI3XXX_MAX_NOC_BUSES_NR;
    }
    else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
    {
        temp = HI6220_MAX_NOC_BUSES_NR;
    }
	for (i = 0; i < temp; i++) {
	    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
	    {
		    if (strncmp(bus_name, hi3xxx_noc_buses_info[i].name,256) == 0)
			    return i;
	    }
	    else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
	    {
		    if (strncmp(bus_name, hi6220_noc_buses_info[i].name,256) == 0)
			    return i;
	    }
	}
	return -ENODEV;
}

static int  register_noc_node(struct device_node *np)
{
    struct noc_node *node;
    int ret = 0;
    const char *bus_name;

    node = kzalloc(sizeof(struct noc_node), GFP_KERNEL);
    if (!node) {
        pr_err("fail to alloc memory, noc_node=%s!\n", np->name);
        ret = -ENOMEM;
        goto err;
    }
    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
    	node->base = of_iomap(np, 0);
    	if (!node->base) {
    		WARN_ON(1);
    		goto err_iomap;
    	}
    }
	/* err probe property */
    if (g_error_enable)
    {
    	if (of_property_read_bool(np, "eprobe-autoenable"))
    		node->eprobe_autoenable = true;

    	ret = of_property_read_u32(np, "eprobe-hwirq", &node->eprobe_hwirq);
    	if (ret < 0) {
    		node->eprobe_hwirq = -1;
    		pr_debug("the node doesnot have err probe!\n");
    	}

    	if (node->eprobe_hwirq >= 0) {
    		ret = of_property_read_u32(np, "eprobe-offset", &node->eprobe_offset);
    		if (ret < 0){
    			node->eprobe_hwirq = -1;
    			pr_debug("the node doesnot have err probe!\n");
    		}
    	}
    }

    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
    	if (node->eprobe_hwirq >= 0)
    	{
	      	ret = of_property_read_u32(np, "hwirq-type", &node->hwirq_type);
	    	if (ret < 0) {
	    		node->hwirq_type = -1;
	    		pr_err("the node doesnot have hwirq type!\n");
	    	}
	  }
    	ret = of_property_read_u32(np, "pwrack-bit", &node->pwrack_bit);
    	if (ret < 0) {
    		node->pwrack_bit = -1;
    		pr_debug("the node doesnot have pwrack_bit property!\n");
    		ret = -ENODEV;
    		goto err_iomap;
    	}
    }
    else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
    {
    	ret = of_property_read_u32(np, "interrupts-num", &node->interrupt_num);
    	if (ret < 0) {
    		pr_debug("the node doesnot have interrupt_num property!\n");
    		ret = -ENODEV;
    		goto err_iomap;
    	}
    }

    if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
    {
        pr_info("interrupt_num =%d +++++++++++ !\n",node->interrupt_num);

        switch(node->interrupt_num)
        {
           case SYS_ERR_INT:
               node->base = error_logger;
               break;
           case MEDIA_ERR_INT:
               node->base = error_logger_media;
               break;
           case SYS_PACKET_INT:
               node->base =error_logger + 0x2000;
               break;
           case MEDIA_PACKET_INT:
               node->base =error_logger_media +0x2000;
               break;
           default:
               node->base = error_logger;
               pr_err("noc interrupt_num =%d is wrong !\n",ret);
               break;
        }
    }
    node->name = kstrdup(np->name, GFP_KERNEL);
    if (!node->name) {
        ret = -ENOMEM;
        goto err_iomap;
    }

    ret = of_property_read_string(np, "bus-name", &bus_name);
    if (ret < 0) {
        WARN_ON(1);
        goto err_property;
    }

    ret = find_bus_id_by_name(bus_name);
    if (ret < 0) {
        WARN_ON(1);
        goto err_property;
    }
    node->bus_id = ret;

	/* FIXME: handle the transprobe & packet probe property */
	/* Debug info */
    if (g_noc_debug)
    {
    	pr_debug("[%s]: nodes_array_idx = %d\n", __func__, nodes_array_idx);
    	pr_debug("np->name = %s\n", np->name);
    	pr_debug("node->base = 0x%p\n", node->base);
    	pr_debug("node->eprobe_hwirq = %d\n", node->eprobe_hwirq);
    	pr_debug("node->eprobe_offset = 0x%x\n", node->eprobe_offset);
    }

    /* put the node into nodes_arry */
    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
        hi3xxx_nodes_array[nodes_array_idx] = node;
    else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
        hi6220_nodes_array[nodes_array_idx] = node;

    nodes_array_idx++;

    /* FIXME: handle the other irq */

    return 0;

err_property:
	kfree(node->name);
    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    	iounmap(node->base);
err_iomap:
	kfree(node);
err:
	return ret;
}

static void register_irqdata(void)
{
	struct noc_node *node = NULL;
	unsigned int i = 0;

	for (i = 0; i < nodes_array_idx; i++) {
        GET_NODE_FROM_ARRAY
        if (!node) {
            pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
            continue;
        }
        if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
        {
            if (node->eprobe_hwirq >= HI3XXX_NOC_MAX_IRQ_NR)
            {
                pr_err("[%s]: node->eprobe_hwirq(%d) is out of range(%d)!\n", __func__, node->eprobe_hwirq, HI3XXX_NOC_MAX_IRQ_NR);
                continue;
            }
            if ((node->hwirq_type >= NOC_ERR_PROBE_IRQ) && (node->hwirq_type <= NOC_TRANS_PROBE_IRQ))
            {
                hi3xxx_noc_irqdata[node->eprobe_hwirq].type = node->hwirq_type;
                hi3xxx_noc_irqdata[node->eprobe_hwirq].node = node;

                if(NOC_TRANS_PROBE_IRQ == node->hwirq_type)
                {
                    init_transcation_info(node);
                }
                else if (NOC_PACKET_PROBE_IRQ == node->hwirq_type)
                {
                    init_packet_info(node);
                 }
            }
            else {
                pr_err("[%s]: the node type error!!!\n", __func__);
            }
        }
        else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
        {
            if ((node->eprobe_hwirq >= 0) && (node->eprobe_hwirq <= 1)) {
                hi6220_noc_irqdata[node->eprobe_hwirq].type =  NOC_ERR_PROBE_IRQ;
                hi6220_noc_irqdata[node->eprobe_hwirq].node = node;
            } else if ((node->eprobe_hwirq >= 2) && (node->eprobe_hwirq <= 3)) {
                hi6220_noc_irqdata[node->eprobe_hwirq].type =  NOC_PACKET_PROBE_IRQ;
                hi6220_noc_irqdata[node->eprobe_hwirq].node = node;
                init_packet_info(node);
            } else {
            pr_err("[%s]: the node type error!!!\n", __func__);
            }
        }
    }
}

static void register_noc_nodes(void)
{
	struct device_node *np;
	int ret =0;
    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
	    	for_each_compatible_node(np, NULL, "hisilicon,hi3xxx-noc-node") {
    		register_noc_node(np);
        }
    }
    else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id)
    {
        for_each_compatible_node(np, NULL, "hisilicon,hi6xxx-noc-node") {
    		register_noc_node(np);
    	}
    }
    /* register_irqdata */
    register_irqdata();
    if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id)
    {
        /*request irq*/
        ret = request_irq(hi6220_noc_irqdata[0].node->interrupt_num, sysnoc_err_irq_handler, IRQF_NO_SUSPEND, "sysnoc_err", NULL);
        if (ret) {
            printk("%s: irq_request  %d failed!   \n",__func__,hi6220_noc_irqdata[0].node->interrupt_num);
        }
        ret = request_irq(hi6220_noc_irqdata[1].node->interrupt_num, media_err_irq_handler, IRQF_NO_SUSPEND, "media_err", NULL);
        if (ret) {
            printk("%s: irq_request  %d failed!   \n",__func__,hi6220_noc_irqdata[1].node->interrupt_num);
        }
        ret = request_irq(hi6220_noc_irqdata[2].node->interrupt_num,sysnoc_alarm_irq_handler, IRQF_NO_SUSPEND, "sysnoc_alarm", NULL);
        if (ret) {
            printk("%s: irq_request  %d failed!   \n",__func__,hi6220_noc_irqdata[2].node->interrupt_num);
        }

        ret = request_irq(hi6220_noc_irqdata[3].node->interrupt_num,media_alarm_irq_handler, IRQF_NO_SUSPEND, "media_alarm", NULL);
        if (ret) {
            printk("%s: irq_request  %d failed!   \n",__func__,hi6220_noc_irqdata[3].node->interrupt_num);
        }
    }
}

static void unregister_noc_nodes(void)
{
	struct noc_node *node = NULL;
	int i = 0;

    for (i = 0; i < nodes_array_idx; i++) {
        GET_NODE_FROM_ARRAY
        if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
        }

        if (g_error_enable)
        {
            if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id )
            {
			if (NOC_ERR_PROBE_IRQ == node->hwirq_type) {
				disable_err_probe(node->base + node->eprobe_offset);
			} else if (NOC_TRANS_PROBE_IRQ == node->hwirq_type) {
				disable_transcation_probe(node->base + node->eprobe_offset);
			} else if (NOC_PACKET_PROBE_IRQ == node->hwirq_type) {
				disable_packet_probe(node->base + node->eprobe_offset);
			} else {
				pr_err("[%s]: the node type error!!!\n", __func__);
			}

            }
            else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
            {
        		if ((node->eprobe_hwirq >= 0) && (node->eprobe_hwirq <= 1)) {
        			disable_err_probe(node->base + node->eprobe_offset);
        		} else if ((node->eprobe_hwirq >= 2) && (node->eprobe_hwirq <= 3)) {
        			disable_packet_probe(node->base + node->eprobe_offset);
        		} else {
        			pr_err("[%s]: the node type error!!!\n", __func__);
        		}
            }
        }
        if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
		    iounmap(node->base);

	    kfree(node);
    }

	nodes_array_idx = 0;
}

void __iomem * get_errprobe_base(const char *name)
{
	struct noc_node *node = NULL;
	int i = 0;

	for (i = 0; i < nodes_array_idx; i++) {
        GET_NODE_FROM_ARRAY
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

		if (!strncmp(name, node->name,256))
			return node->base + node->eprobe_offset;
	}

	pr_warn("[%s]  cannot get node base name = %s\n", __func__, name);
	return NULL;
}
EXPORT_SYMBOL(get_errprobe_base);

struct noc_node * get_probe_node(const char *name)
{
	struct noc_node *node = NULL;
	int i = 0;

	for (i = 0; i < nodes_array_idx; i++) {
        GET_NODE_FROM_ARRAY
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

		if (!strncmp(name, node->name,256))
			return node;
	}

	pr_warn("[%s]  cannot get node base name = %s\n", __func__, name);
	return NULL;
}
EXPORT_SYMBOL(get_probe_node);

static void enable_errprobe(struct device *dev)
{
	struct noc_node *node = NULL;
	unsigned int i;
	struct platform_device *pdev = NULL;
	struct hisi_noc_device *noc_dev = NULL;
	unsigned long status = 0;

   	 if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    	{
	    	pdev = to_platform_device(dev);
	    	noc_dev = platform_get_drvdata(pdev);
		if(NULL == noc_dev){
			pr_err("[%s]: noc_dev not get,return null.\n", __func__);
			return;
		}
	    	status = readl_relaxed(noc_dev->pwrctrl_reg);
    	}
	for (i = 0; i < nodes_array_idx; i++) {
        GET_NODE_FROM_ARRAY
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}
	        if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id )
	        {
	        	if ((NOC_ERR_PROBE_IRQ == node->hwirq_type)&& (node->eprobe_autoenable) && !(status & (1 << node->pwrack_bit))) {

                        if (1 == g_noc_debug)
                            pr_err("Start to enable error probe for bus %s.\n", node->name);
			    enable_err_probe(node->base + node->eprobe_offset);
			}
	        }
	        else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
	        {
	    		if ((node->eprobe_hwirq >= 0) && (node->eprobe_hwirq <= 1) && (node->eprobe_autoenable))
			{
		    	      enable_err_probe(node->base + node->eprobe_offset);
                	}
	        }
	}
}
static void disable_errprobe(struct device *dev)
{
	struct noc_node *node = NULL;
	unsigned int i = 0;
	struct platform_device *pdev = NULL;
	struct hisi_noc_device *noc_dev = NULL;
	unsigned long status = 0;

    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
    	pdev = to_platform_device(dev);
    	noc_dev = platform_get_drvdata(pdev);
	if(NULL == noc_dev){
		pr_err("[%s]: noc_dev not get,return null.\n", __func__);
		return;
	}
    	status = readl_relaxed(noc_dev->pwrctrl_reg);
    }
	for (i = 0; i < nodes_array_idx; i++) {
        GET_NODE_FROM_ARRAY
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

        if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
        {
        	if ((NOC_ERR_PROBE_IRQ == node->hwirq_type) && (node->eprobe_autoenable) && !(status & (1 << node->pwrack_bit)))
			disable_err_probe(node->base + node->eprobe_offset);

        }
        else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
        {
    		if ((node->eprobe_hwirq >= 0) && (node->eprobe_hwirq <= 1) && (node->eprobe_autoenable))
            {
    		    disable_err_probe(node->base + node->eprobe_offset);
            }
        }
	}
}
static void enable_noc_transcation_probe(struct device *dev)
{
	struct noc_node *node = NULL;
	unsigned int i = 0;
	struct platform_device *pdev = NULL;
	struct hisi_noc_device *noc_dev = NULL;
	unsigned long status = 0;

    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
    	pdev = to_platform_device(dev);
    	noc_dev = platform_get_drvdata(pdev);
	if(NULL == noc_dev){
		pr_err("[%s]: noc_dev not get,return null.\n", __func__);
		return;
	}
    	status = readl_relaxed(noc_dev->pwrctrl_reg);
    }
	for (i = 0; i < nodes_array_idx; i++) {
        node = hi3xxx_nodes_array[i];
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}
		if ((NOC_TRANS_PROBE_IRQ == node->hwirq_type) && (node->eprobe_autoenable) && !(status & (1 << node->pwrack_bit))) {
                  if (1 == g_noc_debug)
                        pr_err("Start to enable transaction probe for bus %s.\n", node->name);

			enable_transcation_probe(node, node->base + node->eprobe_offset);
		}
	}

}

static void disable_noc_transcation_probe(struct device *dev)
{
	struct noc_node *node = NULL;
	unsigned int i = 0;
	struct platform_device *pdev = NULL;
	struct hisi_noc_device *noc_dev = NULL;
	unsigned long status = 0;

    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
    	pdev = to_platform_device(dev);
    	noc_dev = platform_get_drvdata(pdev);
	if(NULL == noc_dev){
		pr_err("[%s]: noc_dev not get,return null.\n", __func__);
		return;
	}
    	status = readl_relaxed(noc_dev->pwrctrl_reg);
    }
	for (i = 0; i < nodes_array_idx; i++) {
        GET_NODE_FROM_ARRAY
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

		if ((NOC_ERR_PROBE_IRQ == node->hwirq_type) && (node->eprobe_autoenable) && !(status & (1 << node->pwrack_bit)))
			disable_transcation_probe(node->base + node->eprobe_offset);
	}
}
static void enable_noc_packet_probe(struct device *dev)
{
	struct noc_node *node = NULL;
	unsigned int i = 0;
	struct platform_device *pdev = NULL;
	struct hisi_noc_device *noc_dev = NULL;
	unsigned long status = 0;

    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
    	pdev = to_platform_device(dev);
    	noc_dev = platform_get_drvdata(pdev);
	if(NULL == noc_dev){
		pr_err("[%s]: noc_dev not get,return null.\n", __func__);
		return;
	}
    	status = readl_relaxed(noc_dev->pwrctrl_reg);
    }
	for (i = 0; i < nodes_array_idx; i++) {
        GET_NODE_FROM_ARRAY
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

	    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id )
	    {
		    if ((NOC_PACKET_PROBE_IRQ == node->hwirq_type) && (node->eprobe_autoenable) && !(status & (1 << node->pwrack_bit))) {
                if (1 == g_noc_debug)
                    pr_err("Start to enable packet probe for bus %s.\n", node->name);
				enable_packet_probe(node, node->base + node->eprobe_offset);
		    }
         }
		 else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
		 {
             if ((node->eprobe_hwirq >= 2) && (node->eprobe_hwirq <= 3) && (node->eprobe_autoenable))
             {
		    	 enable_packet_probe(node, node->base + node->eprobe_offset);
             }
		 }
	}
}

static void disable_noc_packet_probe(struct device *dev)
{
	struct noc_node *node = NULL;
	unsigned int i = 0;
	struct platform_device *pdev = NULL;
	struct hisi_noc_device *noc_dev = NULL;
	unsigned long status = 0;

    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
        pdev = to_platform_device(dev);
        noc_dev = platform_get_drvdata(pdev);
	if(NULL == noc_dev){
		pr_err("[%s]: noc_dev not get,return null.\n", __func__);
		return;
	}
	 status = readl_relaxed(noc_dev->pwrctrl_reg);
    }
    for (i = 0; i < nodes_array_idx; i++) {
        GET_NODE_FROM_ARRAY
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}
        if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
        {
		    if ((NOC_PACKET_PROBE_IRQ == node->hwirq_type) && (node->eprobe_autoenable) && !(status & (1 << node->pwrack_bit)))
			disable_packet_probe(node->base + node->eprobe_offset);
        }
        else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
        {
    		if ((node->eprobe_hwirq >= 2) && (node->eprobe_hwirq <= 3) && (node->eprobe_autoenable))
            {
                disable_packet_probe(node->base + node->eprobe_offset);
            }
        }
	}
}

static int hisi_noc_suspend(struct device *dev)
{


    	pr_err("%s+\n", __func__);
        if (g_error_enable)
        {
        	disable_errprobe(dev);

            if (g_transcation_enable)
            	disable_noc_transcation_probe(dev);
            if (g_packet_enable)
            	disable_noc_packet_probe(dev);
        }

    	pr_err("%s-\n", __func__);

	return 0;
}


static int hisi_noc_resume(struct device *dev)
{
    	pr_err("%s+\n", __func__);
        if (g_error_enable)
        {
        	enable_errprobe(dev);

            if (g_transcation_enable)
        	    enable_noc_transcation_probe(dev);
        	if (g_packet_enable)
        	    enable_noc_packet_probe(dev);
        }
    	pr_err("%s-\n", __func__);


	return 0;
}

static SIMPLE_DEV_PM_OPS(noc_pm_ops, hisi_noc_suspend, hisi_noc_resume);

static int hisi_noc_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct hisi_noc_device *noc_dev;
    struct device_node *np;
    const struct of_device_id *match;
    int ret = 0;
    u32 offset = 0;

    pr_info("HISI NOC PROBE START!!!\n");

    /* to check which type of regulator this is */
    match = of_match_device(hisi_noc_match, dev);
    if (NULL == match) {
        pr_err("hisi_noc_probe: mismatch of hisi noc driver \n\r");
        BUG_ON(1);
        goto err;
    }

    g_config_hisi_noc_data=(struct hisi_noc_data *)match->data;

    if (NULL == g_config_hisi_noc_data) {
        pr_err("hisi_noc_probe: get Hisi noc data fail!\n\r");
        BUG_ON(1);
        goto err;
    }
    /* get the pwrctrl_reg offset  */
    np = dev->of_node;

    if (of_property_read_bool(np, "packet_enable"))
    {
        g_packet_enable = 1;
    }
    if (of_property_read_bool(np, "transcation_enable"))
    {
        g_transcation_enable = 1;
    }
    if (of_property_read_bool(np, "error_enable"))
    {
       	g_error_enable = 1;
    }
    if (of_property_read_bool(np, "noc_debug"))
    {
        g_noc_debug = 1;
    }
    if (of_property_read_bool(np, "noc_timeout_enable"))
    {
        g_noc_timeout_enable = 1;
    }

    if ( HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
    {
#ifdef CONFIG_HISI_NOC_HI6220_PLATFORM
        /*for hifi ddr protect*/
        platform_ddr_protect_forhifi_init();
#endif
        noc_root = debugfs_create_dir("noc", NULL);
        u32_packet_timer_period= debugfs_create_u32("packet_timer_period",0755,noc_root,&timer_period);
        u32_packet_stastic_times =debugfs_create_u32("packet_stastic_times",0755,noc_root,&timer_cnt_max_of_file);
        u32_packet_f0_master_id =debugfs_create_u32("packet_f0_master_id",0755,noc_root,&f0_master_id);
        u32_packet_f1_master_id =debugfs_create_u32("packet_f1_master_id",0755,noc_root,&f1_master_id);
        packet_on = debugfs_create_file("packet_on", S_IRUGO | S_IWUSR, noc_root, NULL, &packet_on_fops);
        packet_off = debugfs_create_file("packet_off", S_IRUGO | S_IWUSR, noc_root, NULL, &packet_off_fops);
        packet_data = debugfs_create_file("packet_data", S_IRUGO, noc_root, NULL, &packet_data_fops);

        /*add hrtimer for statistic */
        hrtimer_init(&hr_timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
        hr_timer.function =&hrtimer_callback;
    }

    noc_dev = devm_kzalloc(&pdev->dev, sizeof(struct hisi_noc_device), GFP_KERNEL);
    if (!noc_dev) {
        dev_err(dev, "cannot get memory\n");
        ret = -ENOMEM;
        goto err;
    }

    platform_set_drvdata(pdev, noc_dev);
    if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
    {
        np = of_find_compatible_node(NULL, NULL, "hisilicon,sysnoc-service-base-addr");
        if(np == NULL){
           pr_err("node sysnoc-service-base-addr not find !\n");
           goto err;
        }
        noc_dev->sys_base = of_iomap(np, 0);
        error_logger =noc_dev->sys_base;
    	 if (!noc_dev->sys_base) {
    	    BUG_ON(1);
    	    goto error_sys_base;
    	 }

        np = of_find_compatible_node(NULL, NULL, "hisilicon,media-service-base-addr");

        noc_dev->media_base = of_iomap(np, 0);
        error_logger_media = noc_dev->media_base;
    	 if (!noc_dev->media_base) {
    		BUG_ON(1);
    		goto error_media_base;
    	}

    	pr_info("++++++++++++++error_logger=%p \n", error_logger);
    	pr_info("++++++++++++++error_logger_media=%p \n", error_logger_media);
    }
    else if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
    	np = of_find_node_by_name(NULL, "pctrl");
    	BUG_ON(!np);

    	noc_dev->pctrl_base = of_iomap(np, 0);
    	BUG_ON(!noc_dev->pctrl_base);

    	np = of_find_compatible_node(NULL, NULL, "hisilicon,pmctrl");

    	noc_dev->pmctrl_base = of_iomap(np, 0);
    	if (!noc_dev->pmctrl_base) {
    		BUG_ON(1);
    		goto err_pmctrl;
    	}

    	/* get the pwrctrl_reg offset  */
    	np = dev->of_node;
    	ret = of_property_read_u32(np, "pwrack-reg", &offset);
    	if (ret < 0) {
    		pr_err("cannot found pwrack-reg property!\n");
    		goto err_irq;
    	}
    	noc_dev->pwrctrl_reg = noc_dev->pmctrl_base + offset;

    	ret = platform_get_irq(pdev, 0);
    	if (ret < 0) {
    		dev_err(&pdev->dev, "cannot find IRQ\n");
    		goto err_irq;
    	}
    	noc_dev->hi3xxx_irq = ret;
        if (1 == g_noc_debug)
            pr_err("[%s] noc_irq = %d\n", __func__, noc_dev->hi3xxx_irq);

    	ret = devm_request_irq(&pdev->dev, noc_dev->hi3xxx_irq, hisi_noc_irq_handler,
    		IRQF_TRIGGER_HIGH | IRQF_DISABLED, "hisi_noc", noc_dev);
    	if (ret < 0) {
    		dev_err(&pdev->dev, "request_irq fail!\n");
    		goto err_irq;
    	}

        if (g_noc_timeout_enable)
        {
        	ret = platform_get_irq(pdev, 1);
        	if (ret < 0) {
        	    dev_err(&pdev->dev, "cannot find noc timeout IRQ\n");
        	    goto err_irq;
        	}
        	noc_dev->timeout_irq = ret;
        	pr_debug("[%s] timeout_irq = %d\n", __func__, noc_dev->timeout_irq);
        	ret = devm_request_irq(&pdev->dev, noc_dev->timeout_irq, hisi_noc_timeout_irq_handler,
        		IRQF_TRIGGER_HIGH | IRQF_DISABLED, "hisi_noc_timeout_irq", noc_dev);
        	if (ret < 0) {
        	    dev_err(&pdev->dev, "request timeout irq fail!\n");
        	    goto err_irq;
        	}
        }
    }
    /* process each noc node */
    register_noc_nodes();
    /* enable err probe */
    enable_errprobe(dev);
    if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
    {
        /*add error handler when panic*/
        atomic_notifier_chain_register(&panic_notifier_list, &noc_on_sys_panic_nb);
        atomic_notifier_chain_register(&panic_notifier_list, &noc_on_media_panic_nb);
    }

    if (g_transcation_enable)
    {
      /* enable noc transcation probe*/
        enable_noc_transcation_probe(dev);
    }
    if (g_packet_enable)
    {
        /* enable noc packet probe */
        enable_noc_packet_probe(dev);
    }

    if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
    {
        /*set sys noc for dfs,l00212112*/
        sysnoc_set_for_dfs();

        /*for saving ADE reg value*/
        ade_reg_addr_init();
    }
#ifdef CONFIG_ARCH_HI6XXX
    /*Modem Noc print*/
    modem_reg_addr_init();
#endif
	return 0;

err_irq:
	if (noc_dev->pmctrl_base)
	    iounmap(noc_dev->pmctrl_base);
err_pmctrl:
	if (noc_dev->pctrl_base)
	    iounmap(noc_dev->pctrl_base);
error_media_base:
	if (noc_dev->media_base)
	    iounmap(noc_dev->media_base);
error_sys_base:
	if (noc_dev->sys_base)
	    iounmap(noc_dev->sys_base);
err:
    return ret;
}

static int hisi_noc_remove(struct platform_device *pdev)
{
    struct hisi_noc_device *noc_dev = platform_get_drvdata(pdev);

    if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)
    {
    	unregister_noc_nodes();
        if(noc_dev != NULL)
        {
        	free_irq(noc_dev->hi3xxx_irq, noc_dev);
    		iounmap(noc_dev->pctrl_base);
        }
    }
    else if(HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
    {
        /*remove error handler when panic*/
        atomic_notifier_chain_unregister(&panic_notifier_list, &noc_on_media_panic_nb);
        atomic_notifier_chain_unregister(&panic_notifier_list, &noc_on_sys_panic_nb);

        unregister_noc_nodes();

        error_logger =NULL;
        error_logger_media = NULL;

        if(noc_dev != NULL)
        {
            iounmap(noc_dev->media_base);
            iounmap(noc_dev->sys_base);
        }

        /*for saving ADE reg value*/
        ade_reg_addr_exit();
    }
	return 0;
}



MODULE_DEVICE_TABLE(of, hisi_noc_match);

static struct platform_driver hisi_noc_driver = {
	.probe		= hisi_noc_probe,
	.remove		= hisi_noc_remove,
	.driver = {
		.name = MODULE_NAME,
		.owner = THIS_MODULE,
#ifdef CONFIG_PM_SLEEP
		.pm	= &noc_pm_ops,
#endif
		.of_match_table = of_match_ptr(hisi_noc_match),
	},
};

static int __init hisi_noc_init(void)
{
    return platform_driver_register(&hisi_noc_driver);
}

static void __exit hisi_noc_exit(void)
{
    if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id )
    {
        debugfs_remove(packet_data);
        debugfs_remove(packet_off);
        debugfs_remove(packet_on);
        debugfs_remove(u32_packet_stastic_times);
        debugfs_remove(u32_packet_timer_period);
        debugfs_remove(u32_packet_f0_master_id);
        debugfs_remove(u32_packet_f1_master_id);
        debugfs_remove(noc_root);
        hrtimer_cancel(&hr_timer);
        free_irq(hi6220_noc_irqdata[3].node->interrupt_num,NULL);
        free_irq(hi6220_noc_irqdata[2].node->interrupt_num,NULL);
        free_irq(hi6220_noc_irqdata[1].node->interrupt_num,NULL);
        free_irq(hi6220_noc_irqdata[0].node->interrupt_num,NULL);
    }

    platform_driver_unregister(&hisi_noc_driver);
    g_config_hisi_noc_data=NULL;
}

fs_initcall(hisi_noc_init);
module_exit(hisi_noc_exit);
