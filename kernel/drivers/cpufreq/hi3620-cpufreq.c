/*
 *  linux/arch/arm/mach-k3v2/cpu-k3v2.c
 *
 *  CPU frequency scaling for K3V2
 *
 *  Copyright (C) 2005 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <asm/system.h>
#include <linux/ipps.h>
#include <linux/cpufreq-k3v2.h>
#include <linux/switch.h>
#include <linux/init.h>
#include <linux/module.h>

#include <asm/smp_plat.h>

struct switch_dev policy_switch;

static struct ipps_device *gpipps_device;
static struct ipps_client ipps_client;

static atomic_t freq_table_users = ATOMIC_INIT(0);

/*cpufreq table must be sorted by asc*/
struct cpufreq_frequency_table *cpufreq_table;

struct ipps_param gipps_param;

#define CPU_FREQ_MIN (200000)

/*******************freq attr setting begin*****************************/
#define DEFINE_ATTR_CUR_MODE(_obj, _OBJ)		\
static ssize_t	\
show_scaling_##_obj##_mode(struct cpufreq_policy *policy, char *buf)\
{\
	int ret = 0;\
	unsigned int mode = IPPS_ENABLE;\
	ret = ipps_get_mode(&ipps_client, _OBJ, &mode);\
	if (ret) {\
		pr_debug("Failed to get mode.\n");\
		return ret;\
	} \
	return sprintf(buf, "%s\n",\
			(mode == IPPS_ENABLE) ? "ENABLED" : "DISABLED");\
} \
static ssize_t store_scaling_##_obj##_mode(struct cpufreq_policy *policy,\
					const char *buf, size_t count)\
{\
	unsigned int mode;\
	int ret = 0;\
	if (!sscanf(buf, "%d", &mode))\
		return -EINVAL;\
	ret = ipps_set_mode(&ipps_client, _OBJ, &mode);\
	if (ret) {\
		pr_debug("Failed to set mode.\n");\
		return ret;\
	} \
	return count;\
}

DEFINE_ATTR_CUR_MODE(cpu, IPPS_OBJ_CPU)
cpufreq_freq_attr_rw(scaling_cpu_mode);

#define DEFINE_ATTR_CUR_FUNC(_obj, _OBJ)		\
static ssize_t	\
show_scaling_##_obj##_func(struct cpufreq_policy *policy, char *buf)\
{\
	int ret = 0;\
	unsigned int func = IPPS_DVFS_AVS_ENABLE;\
	ret = ipps_get_func(&ipps_client, _OBJ, &func);\
	if (ret) {\
		pr_debug("Failed to get func.\n");\
		return ret;\
	} \
	switch (func) {\
	case IPPS_DVFS_AVS_ENABLE:\
		ret = sprintf(buf, "%s\n", "dvfs avs enabled");\
		break;\
	case IPPS_DVFS_ENABLE:\
		ret = sprintf(buf, "%s\n", "dvfs enabled");\
		break;\
	case IPPS_AVS_ENABLE:\
		ret = sprintf(buf, "%s\n", "avs enabled");\
		break;\
	case IPPS_DVFS_AVS_DISABLE:\
		ret = sprintf(buf, "%s\n", "dvfs avs disabled");\
		break;\
	} \
	return ret;\
} \
static ssize_t store_scaling_##_obj##_func(struct cpufreq_policy *policy,\
					const char *buf, size_t count)\
{\
	unsigned int func;\
	int ret = 0;\
	if (!sscanf(buf, "%d", &func))\
		return -EINVAL;\
	ret = ipps_set_func(&ipps_client, _OBJ, &func);\
	if (ret) {\
		pr_debug("Failed to set func.\n");\
		return ret;\
	} \
	return count;\
}

DEFINE_ATTR_CUR_FUNC(cpu, IPPS_OBJ_CPU)
cpufreq_freq_attr_rw(scaling_cpu_func);

static struct freq_attr *k3v2_cpufreq_attr[] = {
	&cpufreq_freq_attr_scaling_available_freqs,
	&scaling_cpu_mode,
	&scaling_cpu_func,
	NULL,
};
/*******************freq attr setting end *****************************/

/*******************hotplug cpu operation ****************************/
static int __cpuinit hotcpu_k3_prepare(struct notifier_block *nfb,
					unsigned long action, void *hcpu)
{
	unsigned int umode = 0;

	switch (action & 0xF) {
	case CPU_UP_PREPARE:

		umode = IPPS_DISABLE;
		ipps_set_mode(&ipps_client, IPPS_OBJ_CPU, &umode);
		break;

	case CPU_DOWN_PREPARE:
		umode = IPPS_DISABLE;
		ipps_set_mode(&ipps_client, IPPS_OBJ_CPU, &umode);
		break;

	default:
		break;
	}

	return NOTIFY_OK;
}


static int __cpuinit hotcpu_k3_later(struct notifier_block *nfb,
					unsigned long action, void *hcpu)
{
	unsigned int umode = 0;

	switch (action & 0xF) {
	case CPU_ONLINE:
	case CPU_DOWN_FAILED:
		umode = IPPS_ENABLE;
		ipps_set_mode(&ipps_client, IPPS_OBJ_CPU, &umode);
		break;

	case CPU_UP_CANCELED:
	case CPU_DEAD:
		umode = IPPS_ENABLE;
		ipps_set_mode(&ipps_client, IPPS_OBJ_CPU, &umode);
		break;

	default:
		break;
	}

	return NOTIFY_OK;
}

static struct notifier_block __refdata hotcpu_prepare_notifier = {
	.notifier_call = hotcpu_k3_prepare,
	.priority      = INT_MAX,
};

static struct notifier_block __refdata hotcpu_later_notifier = {
	.notifier_call = hotcpu_k3_later,
	.priority      = INT_MIN,
};

/*******************hotplug cpu operation ****************************/


/*******************Cpufreq_driver interface *************/
static int k3v2_verify_freq(struct cpufreq_policy *policy)
{
	if (cpufreq_table)
		return cpufreq_frequency_table_verify(policy, cpufreq_table);

	if (policy->cpu)
		return -EINVAL;

	cpufreq_verify_within_limits(policy, policy->cpuinfo.min_freq,
				     policy->cpuinfo.max_freq);
	return 0;
}

static unsigned int k3v2_getfreq(unsigned int cpu)
{
	unsigned int rate = CPU_FREQ_MIN;

	pr_debug("[%s] %d cpu\n", __func__, __LINE__);

	if (cpu)
		return 0;

	ipps_get_current_freq(&ipps_client, IPPS_OBJ_CPU, &rate);

	pr_debug("[%s] %d cur=%d.\n", __func__, __LINE__, rate);

	return rate;
}

static int k3v2_target(struct cpufreq_policy *policy,
		       unsigned int target_freq,
		       unsigned int relation)
{
	struct cpufreq_freqs freqs;
	int ret = 0;
	unsigned int new_freq;
	unsigned int i;

	/*check if we have limits on the cpufreqs*/
	if (target_freq > PARAM_MAX(cpu))
		target_freq = PARAM_MAX(cpu);

	if (target_freq < PARAM_MIN(cpu))
		target_freq = PARAM_MIN(cpu);

	ret = cpufreq_frequency_table_target(policy, cpufreq_table, target_freq,
			relation, &i);
	if (ret) {
		pr_err("%s: cpu%d: no freq match for %d(ret=%d)\n",
			__func__, policy->cpu, target_freq, ret);
		return ret;
	}

	freqs.new = cpufreq_table[i].frequency;
	freqs.old = k3v2_getfreq(0);
	freqs.cpu = policy->cpu;

	new_freq = freqs.new;

	if (freqs.old == freqs.new)
		return ret;

	/* notifiers */
	for_each_cpu(i, policy->cpus) {
		freqs.cpu = i;
		cpufreq_notify_transition(policy, &freqs, CPUFREQ_PRECHANGE);
	}

	/*set cpu freq.*/
	ipps_set_current_freq(&ipps_client, IPPS_OBJ_CPU, &new_freq);

	/* notifiers */
	for_each_cpu(i, policy->cpus) {
		freqs.cpu = i;
		cpufreq_notify_transition(policy, &freqs, CPUFREQ_POSTCHANGE);
	}

	return ret;
}

static int __cpuinit k3v2_cpu_init(struct cpufreq_policy *policy)
{
	int result = 0;

	if (policy->cpu != 0)
		pr_err("[%s] %d cpu=%d\n", __func__, __LINE__, policy->cpu);

	policy->cpuinfo.transition_latency = 1 * 1000;

	if (!cpufreq_table) {
		cpufreq_table =
			kmalloc(sizeof(struct cpufreq_frequency_table) * 16,
					GFP_KERNEL);

		if (!cpufreq_table) {
			pr_err("[%s] %d cpufreq_table kmalloc err\n",
					__func__, __LINE__);
			goto ERROR;
		}
	}

	atomic_inc_return(&freq_table_users);

	if (policy->cpu == 0) {
		/*get freq_table*/
		ipps_get_freqs_table(&ipps_client, IPPS_OBJ_CPU, cpufreq_table);

		/*get freq limits*/
		ipps_get_parameter(&ipps_client,
			IPPS_OBJ_DDR|IPPS_OBJ_GPU|IPPS_OBJ_CPU, &gipps_param);

		policy_switch.name = "ippspolicy";
		result = switch_dev_register(&policy_switch);
		if (0 != result) {
			pr_err("%s line=%d, switch_dev_register err=%x\n",
					__func__, __LINE__, result);
			goto ERROR;
		}
	}

	/*get current profile*/
	policy->cur = k3v2_getfreq(0);
	if (cpufreq_table) {
		result = cpufreq_frequency_table_cpuinfo(policy, cpufreq_table);
		if (!result)
			cpufreq_frequency_table_get_attr(cpufreq_table,
							policy->cpu);
	} else {
		pr_err("freq_table does not exist.\n");
	}

	policy->min = PARAM_MIN(cpu);
	policy->max = PARAM_MAX(cpu);

	/*
	 * On Hi3620 SMP configuartion, all processors share the voltage
	 * and clock. So all CPUs needs to be scaled together and hence
	 * needs software co-ordination. Use cpufreq affected_cpus
	 * interface to handle this scenario. Additional is_smp() check
	 * is to keep SMP_ON_UP build working.
	 */
	if (is_smp())
		cpumask_setall(policy->cpus);

	return 0;

ERROR:

	if (atomic_dec_and_test(&freq_table_users)) {
		kfree(cpufreq_table);
		cpufreq_table = NULL;
	}

	pr_err("[%s] %d leave %x\n", __func__, __LINE__, EINVAL);

	return -EINVAL;
}

static int k3v2_cpu_exit(struct cpufreq_policy *policy)
{
	if (policy->cpu != 0) {
		pr_err("[%s] %d cpu=%d\n", __func__, __LINE__, policy->cpu);
		return -EINVAL;
	}

	if (atomic_dec_and_test(&freq_table_users)) {
		kfree(cpufreq_table);
		cpufreq_table = NULL;

		switch_dev_unregister(&policy_switch);
	}

	return 0;
}

static struct cpufreq_driver k3v2_driver = {
	.flags		= CPUFREQ_STICKY,
	.verify		= k3v2_verify_freq,
	.target		= k3v2_target,
	.get		= k3v2_getfreq,
	.init		= k3v2_cpu_init,
	.exit		= k3v2_cpu_exit,
	.name		= "k3v2cpufreq",
	.attr		= k3v2_cpufreq_attr,
};

/******************cpufreq_driver interface end*****************************/


/******************ipps driver interface begin*****************************/

static void ippsclient_add(struct ipps_device *device)
{
	int ret = 0;

	gpipps_device = device;

	ret = cpufreq_register_driver(&k3v2_driver);
	if (0 != ret)
		pr_err("[%s] cpufreq_register_driver err=%x\n", __func__, ret);

	register_hotcpu_notifier(&hotcpu_prepare_notifier);
	register_hotcpu_notifier(&hotcpu_later_notifier);
}

static void ippsclient_remove(struct ipps_device *device)
{
	int ret = 0;

	ret = cpufreq_unregister_driver(&k3v2_driver);
	if (ret != 0)
		pr_err("cpufreq_unregister_driver err=%x\n", ret);

	unregister_hotcpu_notifier(&hotcpu_later_notifier);
	unregister_hotcpu_notifier(&hotcpu_prepare_notifier);

	gpipps_device = NULL;
}

static struct ipps_client ipps_client = {
	.name   = "k3v2cpufreq",
	.add    = ippsclient_add,
	.remove = ippsclient_remove
};

/*****************ipps driver interface end ****************************/

static int __init k3v2_cpufreq_init(void)
{
	int ret = 0;

	ret = ipps_register_client(&ipps_client);
	if (ret != 0)
		pr_err("%s ipps_register_client err=%x\n",
			__func__, ret);

	return ret;
}

static void __exit k3v2_cpufreq_exit(void)
{
	ipps_unregister_client(&ipps_client);
}

MODULE_AUTHOR("LiXin <victor.lixin@huawei.com>");
MODULE_DESCRIPTION("Cpufreq driver for k3v2 processors.");
MODULE_LICENSE("GPL");

arch_initcall(k3v2_cpufreq_init);
module_exit(k3v2_cpufreq_exit);
