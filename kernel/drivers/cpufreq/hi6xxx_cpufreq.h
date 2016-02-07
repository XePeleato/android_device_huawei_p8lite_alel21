/*
 * ARM big.LITTLE platform's CPUFreq header file
 *
 * Copyright (C) 2013 ARM Ltd.
 * Sudeep KarkadaNagesha <sudeep.karkadanagesha@arm.com>
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
#ifndef CPUFREQ_HI6XXX_SMP_H
#define CPUFREQ_HI6XXX_SMP_H

#include <linux/cpufreq.h>
#include <linux/device.h>
#include <linux/types.h>

/* Currently we support only one cluster */
#define A53_CLUSTER_0	0
#define A53_CLUSTER_1	1
#define MAX_CLUSTERS	2

#define is_bL_switching_enabled()		false
#define set_switching_enabled(x) 		do { } while (0)

typedef struct
{
    unsigned int cur_acpu_freq;
    unsigned int ddr_max_freq_limit;
    unsigned int ddr_min_freq_limit;
}ACPU_DDR_FREQ_LINK_ATTR;

typedef struct ACPU_DDR_FREQLINK_DATA
{
	ACPU_DDR_FREQ_LINK_ATTR acpu_ddr_freqlink_cfg[10];
	unsigned int acpu_ddr_minfreq_req_id;
	unsigned int acpu_ddr_maxfreq_req_id;
	int acpu_not_suspend;
       int acpu_ddr_freq_switch;
       int acpu_ddr_link_min_running_flag;
       int acpu_ddr_link_max_running_flag;
}ACPU_DDR_FREQLINK_DATA_STRU;
struct cpufreq_hi6xxx_smp_ops {
	char name[CPUFREQ_NAME_LEN];
	int (*get_transition_latency)(struct device *cpu_dev);

	/*
	 * This must set opp table for cpu_dev in a similar way as done by
	 * of_init_opp_table().
	 */
	int (*init_opp_table)(struct device *cpu_dev);
};

static inline int cpu_to_cluster(int cpu)
{
	return is_bL_switching_enabled() ? MAX_CLUSTERS:
		topology_physical_package_id(cpu);
}

int smp_cpufreq_register(struct cpufreq_hi6xxx_smp_ops *ops);
void smp_cpufreq_unregister(struct cpufreq_hi6xxx_smp_ops *ops);

#endif /* CPUFREQ_HI6XXX_SMP_H */
