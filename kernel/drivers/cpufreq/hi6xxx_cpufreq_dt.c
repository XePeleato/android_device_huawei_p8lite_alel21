/*
 * Generic big.LITTLE CPUFreq Interface driver
 *
 * It provides necessary ops to arm_big_little cpufreq driver and gets
 * Frequency information from Device Tree. Freq table in DT must be in KHz.
 *
 * Copyright (C) 2013 Linaro.
 * Viresh Kumar <viresh.kumar@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/device.h>
#include <linux/export.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/opp.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/notifier.h>
#include <linux/suspend.h>
#include "hi6xxx_cpufreq.h"
#include <linux/hisi/pm/pwrctrl_multi_def.h>
#include <linux/hisi/pm/hi6xxx-power-common.h>
#include <linux/hisi/pm/hi6xxx-power-dfs.h>
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>




ACPU_DDR_FREQLINK_DATA_STRU g_acpu_ddr_freqlink;
void __iomem *acpu_cur_freq_addr ;
int g_adfreq_sr_switch;


unsigned int g_acpu_chip_max_freq;


/* get cpu node with valid operating-points */
static struct device_node *get_cpu_node_with_valid_op(int cpu)
{
	struct device_node *np = NULL, *parent;
	int count = 0;

	parent = of_find_node_by_path("/cpus");
	if (!parent) {
		pr_err("failed to find OF /cpus\n");
		return NULL;
	}

	for_each_child_of_node(parent, np) {
		if (count++ != cpu)
			continue;
		if (!of_get_property(np, "operating-points", NULL)) {
			of_node_put(np);
			np = NULL;
		}

		break;
	}

	of_node_put(parent);
	return np;
}


static int smp_cpufreq_pm_notify(struct notifier_block *this, unsigned long event, void *ptr)
{
	int ret=0;
	unsigned int cur_freq=0;
	int i;
	switch (event) {
	case PM_HIBERNATION_PREPARE:
        /*todo here*/
		break;

	case PM_POST_HIBERNATION:
        /*todo here*/
		break;

	case PM_SUSPEND_PREPARE:
        /*todo here*/
		if(g_acpu_ddr_freqlink.acpu_ddr_freq_switch == 1)
		{
			g_acpu_ddr_freqlink.acpu_not_suspend=0;
                     if(g_acpu_ddr_freqlink.acpu_ddr_link_min_running_flag != 0)
                     {
			    ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MIN,&(g_acpu_ddr_freqlink.acpu_ddr_minfreq_req_id),ACPU_SUSPEND_RLQOS_DDR_MIN_VALUE);
                     }
                     else
                     {
                         ret=RET_OK;
                     }
			if(RET_OK != ret)
			{
				pr_err(" pwrctrl_dfs_cmd_rlqos MIN failed .\n");

			}
                     if(g_acpu_ddr_freqlink.acpu_ddr_link_max_running_flag != 0)
                     {
			    ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MAX,&(g_acpu_ddr_freqlink.acpu_ddr_maxfreq_req_id),ACPU_SUSPEND_RLQOS_DDR_MAX_VALUE);
                     }
                     else
                     {
                         ret=RET_OK;
                     }
			if(RET_OK != ret)
			{
				pr_err(" pwrctrl_dfs_cmd_rlqos MAX failed .\n");

			}
		}

		break;
	case PM_POST_SUSPEND:
        /*todo here*/

		if(g_acpu_ddr_freqlink.acpu_ddr_freq_switch == 1)
		{
			g_acpu_ddr_freqlink.acpu_not_suspend=1;

			if (g_adfreq_sr_switch == 1)
			{
				cur_freq=*((unsigned int *)acpu_cur_freq_addr);
				pr_err("  cur_freq:%d.\n",cur_freq);
				for(i=0;i<10;i++)
				{
					if(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].cur_acpu_freq ==cur_freq )
						break;
				}
                            if(i >= 10) {
                                pr_err("%s %d cur_freq not found!\n", __func__, __LINE__);
                                return NOTIFY_DONE;
                            }
                            if(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_min_freq_limit != 0)
                            {
				    ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MIN,&(g_acpu_ddr_freqlink.acpu_ddr_minfreq_req_id),g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_min_freq_limit);
                                g_acpu_ddr_freqlink.acpu_ddr_link_min_running_flag = 1;
                            }
                            else
                            {
                                if(g_acpu_ddr_freqlink.acpu_ddr_link_min_running_flag != 0)
                                {
                                    ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MIN,&(g_acpu_ddr_freqlink.acpu_ddr_minfreq_req_id),ACPU_SUSPEND_RLQOS_DDR_MIN_VALUE);
                                    g_acpu_ddr_freqlink.acpu_ddr_link_min_running_flag = 0;
                                }
                                else
                                {
                                    ret=RET_OK;
                                }
                            }
				if(RET_OK != ret)
				{
					pr_err(" pwrctrl_dfs_qos_update MIN failed .\n");

				}
                            if(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_max_freq_limit != 0)
                            {
				    ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MAX,&(g_acpu_ddr_freqlink.acpu_ddr_maxfreq_req_id),g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_max_freq_limit);
                                g_acpu_ddr_freqlink.acpu_ddr_link_max_running_flag = 1;
                            }
                            else
                            {
                                if(g_acpu_ddr_freqlink.acpu_ddr_link_max_running_flag != 0)
                                {
                                    ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MAX,&(g_acpu_ddr_freqlink.acpu_ddr_maxfreq_req_id),ACPU_SUSPEND_RLQOS_DDR_MAX_VALUE);
                                    g_acpu_ddr_freqlink.acpu_ddr_link_max_running_flag = 0;
                                }
                                else
                                {
                                    ret=RET_OK;
                                }
                            }
				if(RET_OK != ret)
				{
					pr_err(" pwrctrl_dfs_qos_update MAX failed .\n");

				}
				g_adfreq_sr_switch=0;
			}
		}
		break;

    }
    return NOTIFY_DONE;
}

static struct notifier_block smp_freq_pm_notifier = {
    .notifier_call = smp_cpufreq_pm_notify,
    /*.priority = INT_MAX,*/ /*to change notifier's priority*/
};
static int of_get_acpu_ddr_freq_link_attr(void)
{

	struct device_node *root = NULL;
	int err;
       int ret=0;
	memset(&g_acpu_ddr_freqlink,0,sizeof(ACPU_DDR_FREQLINK_DATA_STRU));

	root = of_find_compatible_node(NULL, NULL, "hisilicon,cpu-link-ddr");
	if (!root)
	{
		pr_err("%s hisilicon,cpu-link-ddr no root node\n",__func__);
		return RET_ERR;
	}

	g_acpu_ddr_freqlink.acpu_ddr_maxfreq_req_id=DFS_INVALID_ID;
	g_acpu_ddr_freqlink.acpu_ddr_minfreq_req_id=DFS_INVALID_ID;
	g_acpu_ddr_freqlink.acpu_not_suspend=1;
	g_acpu_ddr_freqlink.acpu_ddr_freq_switch=0;
       g_acpu_ddr_freqlink.acpu_ddr_link_max_running_flag=0;
       g_acpu_ddr_freqlink.acpu_ddr_link_min_running_flag=0;
	g_adfreq_sr_switch=0;
	acpu_cur_freq_addr = ioremap(MEMORY_FREQDUMP_ADDR+4,4);
	of_property_read_u32_array(root, "switch-value", (u32 *)&(g_acpu_ddr_freqlink.acpu_ddr_freq_switch), 0x1);

	of_property_read_u32_array(root, "freq-208000", (u32 *)&(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[0]), 0x3);

	of_property_read_u32_array(root, "freq-432000", (u32 *)&(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[1]), 0x3);

	of_property_read_u32_array(root, "freq-729000", (u32 *)&(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[2]), 0x3);

	of_property_read_u32_array(root, "freq-960000", (u32 *)&(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[3]), 0x3);

	of_property_read_u32_array(root, "freq-1200000", (u32 *)&(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[4]), 0x3);

	of_property_read_u32_array(root, "freq-1500000", (u32 *)&(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[5]), 0x3);

	err = register_pm_notifier(&smp_freq_pm_notifier);
	if (err){
		printk(KERN_ERR "failed to register freq pm notifier !\r\n");
	}
       ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MIN,&(g_acpu_ddr_freqlink.acpu_ddr_minfreq_req_id),ACPU_SUSPEND_RLQOS_DDR_MIN_VALUE);
       if(RET_OK != ret)
       {
           pr_err(" pwrctrl_dfs_qos_update MIN failed .\n");

       }
       ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MAX,&(g_acpu_ddr_freqlink.acpu_ddr_maxfreq_req_id),ACPU_SUSPEND_RLQOS_DDR_MAX_VALUE);
	if(RET_OK != ret)
	{
		pr_err(" pwrctrl_dfs_qos_update MAX failed .\n");

	}
	pr_err("acpu_ddr_freq_switch switch is %d\n",g_acpu_ddr_freqlink.acpu_ddr_freq_switch);
	return 0;
}

static int dt_init_opp_table(struct device *cpu_dev)
{
	struct device_node *np;
	int ret;

	np = get_cpu_node_with_valid_op(cpu_dev->id);
	if (!np)
		return -ENODATA;

	cpu_dev->of_node = np;
	ret = of_init_opp_table(cpu_dev);
	of_node_put(np);

	return ret;
}

static int dt_get_transition_latency(struct device *cpu_dev)
{
	struct device_node *np;
        int ret = 0;
	u32 transition_latency = CPUFREQ_ETERNAL;

	np = get_cpu_node_with_valid_op(cpu_dev->id);
	if (!np)
		return CPUFREQ_ETERNAL;

	ret=of_property_read_u32(np, "clock-latency", &transition_latency);
        if(ret != 0) {
            /*just for pclint check*/
        }

	of_node_put(np);

	pr_debug("%s: clock-latency: %d\n", __func__, transition_latency);
	return transition_latency;
}

static struct cpufreq_hi6xxx_smp_ops dt_smp_ops = {
	.name	= "dt-smp",
	.get_transition_latency = dt_get_transition_latency,
	.init_opp_table = dt_init_opp_table,
};

static int generic_smp_probe(struct platform_device *pdev)
{
	struct device_node *np;
       void __iomem *cur_freq_addr = NULL;
	int ret=0;
	unsigned int cur_freq_prof=0;
	/*int i;*/

	pr_info("%s\n",__func__);

	g_acpu_chip_max_freq = readl(ioremap(ACPU_CHIP_MAX_FREQ, ACPU_CHIP_MAX_FREQ_SIZE));

	np = get_cpu_node_with_valid_op(0);
	if (!np)
		return -ENODEV;
	if(RET_OK != of_get_acpu_ddr_freq_link_attr())
	{
		pr_err(" acpu_ddr_freq_link load from dts failed .\n");
	}


	of_node_put(np);
#if 0
	for(i=0;i<5;i++)
	{
		pr_err("g_acpu_ddr_freqlink[%d] ddr_min_freq_limit:%d \r\n",i,g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_min_freq_limit);

	}
	for(i=0;i<5;i++)
	{
		pr_err("g_acpu_ddr_freqlink[%d] ddr_max_freq_limit:%d \r\n",i,g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_max_freq_limit);

	}
#endif
	if(RET_OK != smp_cpufreq_register(&dt_smp_ops))
	{
		pr_err(" smp_cpufreq_registers failed .\n");
		return -1;
	}
	cur_freq_addr=ioremap(MEMORY_AXI_CUR_FREQ_ADDR,4);
	cur_freq_prof=*(unsigned int *)cur_freq_addr;
	pr_err("  cur_freq_prof is :%d.\n",cur_freq_prof);

	if(g_acpu_ddr_freqlink.acpu_ddr_freq_switch == 1)
	{
		if(0 != g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[cur_freq_prof].ddr_max_freq_limit)
		{

			ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MAX,&(g_acpu_ddr_freqlink.acpu_ddr_maxfreq_req_id), g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[cur_freq_prof].ddr_max_freq_limit);
                     g_acpu_ddr_freqlink.acpu_ddr_link_max_running_flag=1;
			if(RET_OK != ret)
			{
				pr_err(" pwrctrl_dfs_qos_update MAX failed .\n");

			}
		}
		if(0 !=  g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[cur_freq_prof].ddr_min_freq_limit)
		{

			ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MIN,&(g_acpu_ddr_freqlink.acpu_ddr_minfreq_req_id),g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[cur_freq_prof].ddr_min_freq_limit);
                     g_acpu_ddr_freqlink.acpu_ddr_link_min_running_flag=1;
			if(RET_OK != ret)
			{
				pr_err(" pwrctrl_dfs_qos_update MIN failed .\n");

			}
		}
	}
	return 0;
}

static int generic_smp_remove(struct platform_device *pdev)
{
	smp_cpufreq_unregister(&dt_smp_ops);
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id generic_smp_cpufreq[] = {
	{ .compatible = "hi6xxx,generic-smp-cpufreq" },
	{},
};
MODULE_DEVICE_TABLE(of, generic_smp_cpufreq);
#endif

static struct platform_driver generic_smp_platdrv = {
	.driver = {
		.name	= "hi6xxx-smp-cpufreq-dt",
		.owner	= THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(generic_smp_cpufreq),
#endif
	},
	.probe		= generic_smp_probe,
	.remove		= generic_smp_remove,
};
module_platform_driver(generic_smp_platdrv);

MODULE_AUTHOR("Viresh Kumar <viresh.kumar@linaro.org>");
MODULE_DESCRIPTION("Generic ARM big LITTLE cpufreq driver via DT");
MODULE_LICENSE("GPL");
