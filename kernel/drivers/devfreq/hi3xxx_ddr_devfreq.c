/*
 * ddr devfreq driver
 *
 * Copyright (c) 2013-2014 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/devfreq.h>
#include <linux/pm_qos.h>
#include <linux/opp.h>
#include <linux/clk.h>

#define MODULE_NAME "DDR_DEVFREQ"

struct ddr_devfreq_device {
	struct devfreq *devfreq;
	struct clk *set;
	struct clk *get;
	unsigned long freq;
};

static int
ddr_devfreq_target(struct device *dev, unsigned long *freq, u32 flags)
{
	struct platform_device *pdev = container_of(dev,
					struct platform_device, dev);
	struct ddr_devfreq_device *ddev = platform_get_drvdata(pdev);
	struct devfreq *devfreq;
	unsigned long _freq = *freq;
	int lev;

	if (ddev == NULL) {
		pr_err("%s %d have NULL pointer\n", __func__, __LINE__);
		return -1;
	}
        devfreq = ddev->devfreq;

	if (ddev->freq != *freq) {
		/* undate ddr freqency down threshold */
		(void)clk_set_rate(ddev->set, *freq);
		ddev->freq = *freq;
	}

	/* fix: fail to update devfreq freq_talbe state. */
	*freq = clk_get_rate(ddev->get);

	/* check */
	for (lev = 0; lev < devfreq->profile->max_state; lev++)
		if (*freq == devfreq->profile->freq_table[lev])
			goto out;

	/* exception */
	pr_err("\n");
	dev_err(dev,
		"odd freq status.\n<Target: %09lu hz>\n<Status: %09lu hz>\n%s",
		_freq, *freq, "--- freq table ---\n");
	for (lev = 0; lev < devfreq->profile->max_state; lev++) {
		pr_err("<%d> %09u hz\n",
			lev, devfreq->profile->freq_table[lev]);
	}
	pr_err("------- end ------\n");

out:
	return 0;
}

/*
 * we can ignore setting current devfreq state,
 * because governor, "pm_qos", could get status through pm qos.
 */
static int
ddr_devfreq_get_dev_status(struct device *dev, struct devfreq_dev_status *stat)
{
	return 0;
}

static int ddr_devfreq_get_cur_status(struct device *dev, unsigned long *freq)
{
	struct platform_device *pdev = container_of(dev,
					struct platform_device, dev);
	struct ddr_devfreq_device *ddev = platform_get_drvdata(pdev);
	if (ddev == NULL) {
		pr_err("%s %d have NULL pointer\n", __func__, __LINE__);
		return -1;
	}

	*freq = clk_get_rate(ddev->get);
	return 0;
}

static struct devfreq_dev_profile ddr_devfreq_dev_profile = {
	.polling_ms		= 0,
	.target			= ddr_devfreq_target,
	.get_dev_status		= ddr_devfreq_get_dev_status,
	.get_cur_freq		= ddr_devfreq_get_cur_status,
};

static struct devfreq_pm_qos_data ddr_devfreq_pm_qos_data = {
	.bytes_per_sec_per_hz = 16,
	.bd_utilization = 60,
	.pm_qos_class = PM_QOS_MEMORY_THROUGHPUT,
};

static struct devfreq_pm_qos_data ddr_devfreq_up_th_pm_qos_data = {
	.bytes_per_sec_per_hz = 16,
	.bd_utilization = 60,
	.pm_qos_class = PM_QOS_MEMORY_THROUGHPUT_UP_THRESHOLD,
};

static int ddr_devfreq_probe(struct platform_device *pdev)
{
	struct ddr_devfreq_device *ddev = NULL;
	struct device_node *np = pdev->dev.of_node;
	struct devfreq_pm_qos_data *ddata = NULL;
	const char *type = NULL;
	int ret = 0;

	if (!np) {
		pr_err("%s: %s %d, no device node\n",
			MODULE_NAME, __func__, __LINE__);
		ret = -ENODEV;
		goto out;
	}

	ret = of_property_read_string(np, "pm_qos_class", &type);
	if (ret) {
		pr_err("%s: %s %d, no type\n",
			MODULE_NAME, __func__, __LINE__);
		ret = -EINVAL;
		goto no_type;
	}

	if (!strcmp("memory_tput", type)) {
		ddata = &ddr_devfreq_pm_qos_data;
	} else if (!strcmp("memory_tput_up_threshold", type)) {
		ddata = &ddr_devfreq_up_th_pm_qos_data;
	} else {
		pr_err("%s: %s %d, err type\n",
			MODULE_NAME, __func__, __LINE__);
		ret = -EINVAL;
		goto err_type;
	}

	ddev = kmalloc(sizeof(struct ddr_devfreq_device), GFP_KERNEL);
	if (!ddev) {
		pr_err("%s: %s %d, no mem\n",
			MODULE_NAME, __func__, __LINE__);
		ret = -ENOMEM;
		goto no_men;
	}

	ddev->set = of_clk_get(np, 0);
	if (IS_ERR(ddev->set)) {
		pr_err("%s: %s %d, Failed to get set-clk\n",
			MODULE_NAME, __func__, __LINE__);
		ret = -ENODEV;
		goto no_clk1;
	}

	ddev->get = of_clk_get(np, 1);
	if (IS_ERR(ddev->get)) {
		pr_err("%s: %s %d, Failed to get get-clk\n",
			MODULE_NAME, __func__, __LINE__);
		ret = -ENODEV;
		goto no_clk2;
	}

	if (of_init_opp_table(&pdev->dev) ||
		opp_init_devfreq_table(&pdev->dev,
			&ddr_devfreq_dev_profile.freq_table)) {
		ddev->devfreq = NULL;
	} else {
		ddr_devfreq_dev_profile.initial_freq = clk_get_rate(ddev->get);
		rcu_read_lock();
		ddr_devfreq_dev_profile.max_state = opp_get_opp_count(&pdev->dev);
		rcu_read_unlock();
		ddev->devfreq = devfreq_add_device(&pdev->dev,
					&ddr_devfreq_dev_profile,
					"pm_qos",
					ddata);
	}

	if (IS_ERR(ddev->devfreq)) {
		pr_err("%s: %s %d, <%s>, Failed to init ddr devfreq_table\n",
			MODULE_NAME, __func__, __LINE__, type);
		ret = -ENODEV;
		goto no_devfreq;
	}

	/*
	 *	cache value.
	 *	It does not mean actual ddr clk currently,
	 *	but a frequency cache of every clk setting in the module.
	 *	Because, it is not obligatory that setting value is equal to
	 *	the currently actual ddr clk frequency.
	 */
	ddev->freq = 0;

	platform_set_drvdata(pdev, ddev);

	pr_info("%s: <%s> ready\n", MODULE_NAME, type);
	return ret;

no_devfreq:
	clk_put(ddev->get);
no_clk2:
	clk_put(ddev->set);
no_clk1:
	kfree(ddev);
no_men:
err_type:
no_type:
out:
	return ret;
}

static int ddr_devfreq_remove(struct platform_device *pdev)
{
	struct ddr_devfreq_device *ddev = platform_get_drvdata(pdev);
	if (ddev == NULL) {
		pr_err("%s %d have NULL pointer\n", __func__, __LINE__);
		return -1;
	}

	devfreq_remove_device(ddev->devfreq);

	platform_set_drvdata(pdev, NULL);

	clk_put(ddev->get);
	clk_put(ddev->set);

	kfree(ddev);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id ddr_devfreq_of_match[] = {
	{.compatible = "hisilicon,ddr_devfreq",},
	{.compatible = "hisilicon,ddr_devfreq_up_threshold",},
	{},
};
MODULE_DEVICE_TABLE(of, ddr_devfreq_of_match);
#endif

static struct platform_driver ddr_devfreq_driver = {
	.probe = ddr_devfreq_probe,
	.remove = ddr_devfreq_remove,
	.driver = {
		.name = "ddr_devfreq",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(ddr_devfreq_of_match),
	},
};
module_platform_driver(ddr_devfreq_driver);
