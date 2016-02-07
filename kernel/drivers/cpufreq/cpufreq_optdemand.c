/*
 *  drivers/cpufreq/cpufreq_optdemand.c
 *
 *  Copyright (C)  2014 Yu Wei <yuwei3@hisilicon.com>
 *                      Mo Haoju <mohaoju@hisilicon.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/cpufreq.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/percpu-defs.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/tick.h>
#include <linux/types.h>
#include <linux/cpu.h>
#include <linux/input.h>
#include <linux/of.h>

#include "cpufreq_governor.h"

#define CREATE_TRACE_POINTS
#include <trace/events/cpufreq_optdemand.h>

/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
#include <linux/hisi/pm/hi6xxx-power-common.h>
#include <linux/hisi/hi6xxx_tele_mntn.h>
extern unsigned int omTimerGet(void);
#endif
#endif
/*===tele_mntn===*/

unsigned long loads[NR_CPUS];
unsigned int cur_cpu;


/* optdemand governor macros */
#define DEF_SAMPLING_DOWN_FACTOR		(4)
#define MAX_SAMPLING_DOWN_FACTOR		(100000)
#define MICRO_FREQUENCY_MIN_SAMPLE_RATE		(10000)
#define DEF_BOOST_PULSE_DURATION        (80000)
#define DEF_GO_HISPEED_LOAD         (95)
#define DEF_HISPEED_FREQ            (960000)
#define DEF_UP_THRESHOLD		    (40)
#define DEF_DOWN_THRESHOLD          (30)

static unsigned int default_up_threshold[] = {DEF_UP_THRESHOLD};
static unsigned int default_down_threshold[] = {DEF_DOWN_THRESHOLD};

/* save up&down threshold which get from dt */
static unsigned int *up_threshold;
static unsigned int *down_threshold;
static unsigned int nup_threshold;
static unsigned int ndown_threshold;

struct cpufreq_optdemand_cpuinfo {
	struct cpu_dbs_common_info cdbs;
	struct cpufreq_frequency_table *freq_table;
	unsigned int rate_mult;
};

static DEFINE_PER_CPU(struct cpufreq_optdemand_cpuinfo, cpuinfo);

struct cpufreq_optdemand_tunables {
	unsigned int sampling_rate;
	unsigned int sampling_down_factor;
	unsigned int go_hispeed_load;
	unsigned int hispeed_freq;
	spinlock_t threshold_lock;
	unsigned int *up_threshold;
	int nup_threshold;
	unsigned int *down_threshold;
	int ndown_threshold;
	/* Non-zero means indefinite speed boost active */
	int boost;
	/* Duration of a boot pulse in usecs */
	int boostpulse_duration;
	/* boost pulse of input event */
	bool input_event_boost;
};

#ifndef CONFIG_CPU_FREQ_DEFAULT_GOV_OPTDEMAND
static struct cpufreq_governor cpufreq_gov_optdemand;
#endif

#ifdef CONFIG_INPUT_PULSE_SUPPORT
struct cpufreq_optdemand_inputopen {
	struct input_handle *handle;
	struct work_struct inputopen_work;
};

static struct cpufreq_optdemand_inputopen inputopen;
static struct workqueue_struct *down_wq;
#endif

static int cpufreq_optdemand_initialized;
/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
static void tele_mntn_cpufreq_optdemand_boost_setrate(
    struct cpufreq_policy *policy, unsigned int tar_freq)
{
    PWC_TELE_MNTN_ACPU_DFS_BOOST_STRU *boost = NULL;
    PWC_TELE_MNTN_ACPU_DFS_BOOSTINFO_STRU * info = NULL;

    if(!g_pPwcAcpuLog)
        return;

    boost = &(g_pPwcAcpuLog->dfsAcpu.boost);
    info = &(boost->info);
    info->cpu = cur_cpu;
    info->tag_freq = tar_freq;
    info->new_freq = policy->cur;
    info->min_freq = policy->min;
    info->max_freq = policy->max;
    boost->boostSliceTime = omTimerGet();
    (void)tele_mntn_write_log(TELE_MNTN_PLI_ACPU, sizeof(PWC_TELE_MNTN_ACPU_DFS_BOOST_STRU), (void *)boost);
}

static void tele_mntn_cpufreq_optdemand_setrate(
    struct cpufreq_policy *policy, unsigned int tar_freq)
{
    PWC_TELE_MNTN_ACPU_DFS_POLICY_STRU *pli= NULL;
    PWC_TELE_MNTN_ACPU_DFS_POLICYINFO_STRU * info = NULL;
    unsigned int cpu_num = (ACPU_CPUIDLE_CUR_CPU_NUM <= NR_CPUS ? ACPU_CPUIDLE_CUR_CPU_NUM : NR_CPUS);
    unsigned int j = 0;

    if(!g_pPwcAcpuLog)
        return;

    pli = &(g_pPwcAcpuLog->dfsAcpu.policy);
    info = &(pli->info);
    info->cpu = cur_cpu;
    info->tag_freq = tar_freq;
    info->new_freq = policy->cur;
    info->min_freq = policy->min;
    info->max_freq = policy->max;

    for(j = 0; j < cpu_num; ++j)
        info->load[j] = (unsigned char)loads[j];

    pli->policySliceTime = omTimerGet();
    (void)tele_mntn_write_log(TELE_MNTN_PLI_ACPU, sizeof(PWC_TELE_MNTN_ACPU_DFS_POLICY_STRU), (void *)pli);
}
#endif
#endif
/*===tele_mntn===*/

static unsigned int get_up_threshold(
	struct cpufreq_optdemand_tunables *tunables, unsigned int freq)
{
	int i;
	unsigned int ret;
	unsigned long flags;

	spin_lock_irqsave(&tunables->threshold_lock, flags);

	for (i = 0; i < tunables->nup_threshold - 1 &&
		    freq >= tunables->up_threshold[i+1]; i += 2)
		;

	ret = tunables->up_threshold[i];
	spin_unlock_irqrestore(&tunables->threshold_lock, flags);
	return ret;
}

static unsigned int get_down_threshold(
	struct cpufreq_optdemand_tunables *tunables, unsigned int freq)
{
	int i;
	unsigned int ret;
	unsigned long flags;

	spin_lock_irqsave(&tunables->threshold_lock, flags);

	for (i = 0; i < tunables->ndown_threshold - 1 &&
		    freq >= tunables->down_threshold[i+1]; i += 2)
		;

	ret = tunables->down_threshold[i];
	spin_unlock_irqrestore(&tunables->threshold_lock, flags);
	return ret;
}

/*
 * Increase or decrease frequency according to the up threshold
 * and down threshold of the current freq.
 */
static unsigned int choose_freq(struct cpufreq_optdemand_cpuinfo *pcpu,
		unsigned int load_freq)
{
    struct cpufreq_policy *policy = pcpu->cdbs.cur_policy;
	unsigned int freq = policy->cur;
	struct dbs_data *dbs_data = policy->governor_data;
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
    unsigned int up_threshold, down_threshold;

	up_threshold = get_up_threshold(tunables, freq);
    if(!up_threshold)
        up_threshold = DEF_UP_THRESHOLD;

	/* frequency increase */
	if (load_freq > up_threshold * freq) {
		/* If load meet increase threshold, apply sampling_down_factor */
		pcpu->rate_mult = tunables->sampling_down_factor;
		if (policy->cur < policy->max)  {
			freq = load_freq / up_threshold;

    		if (load_freq >= tunables->go_hispeed_load * policy->cur)
                freq = max(freq, tunables->hispeed_freq);
		}
		return freq;
	}

	/* frequency decrease, need to wait N continuous freq decrease period
	 * since last freq increase.
	 */
	down_threshold = get_down_threshold(tunables, freq);
	if (load_freq < down_threshold * freq) {
		if (policy->cur > policy->min) {
		    if(pcpu->rate_mult > 0)
    			pcpu->rate_mult -= 1;

			if(!pcpu->rate_mult)
                freq = load_freq / down_threshold;
        }
	}

    return freq;
}

static void optdemand_check_cpu(int cpu, unsigned int load_freq)
{
	struct cpufreq_optdemand_cpuinfo *pcpu = &per_cpu(cpuinfo, cpu);
	struct cpufreq_policy *policy = pcpu->cdbs.cur_policy;
	unsigned int freq_cur = policy->cur;
	unsigned int freq_next;

    freq_next = choose_freq(pcpu, load_freq);

    __cpufreq_driver_target(policy, freq_next, CPUFREQ_RELATION_L);
	trace_cpufreq_optdemand_setrate(cur_cpu, &loads[0], freq_cur, freq_next);
/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
	tele_mntn_cpufreq_optdemand_setrate(policy, freq_next);
#endif
#endif
/*===tele_mntn===*/
}

#ifdef CONFIG_SCHED_HMP
extern unsigned int hmp_slower_domain_load;
extern unsigned int hmp_slower_domain_avg_load;
extern int hmp_load_ctrl_flag;
#endif

static void optdemand_dbs_check_cpu(struct dbs_data *dbs_data, int cpu)
{
	struct cpu_dbs_common_info *cdbs = dbs_data->cdata->get_cpu_cdbs(cpu);
	struct cpufreq_policy *policy = cdbs->cur_policy;
	unsigned int max_load = 0;
	unsigned int j;

#ifdef CONFIG_SCHED_HMP
	unsigned int min_load = 0;
	unsigned int little_total_load = 0;
	unsigned int cpuid = 0;
#endif

	/* Get all cpus Absolute Load, because of HMP,
           cluster1 do not have cpufreq, but we have to
           take cpus of cluster1 into consideration when
           cpufreq scaling.
         */
	for_each_online_cpu(j) {
		struct cpu_dbs_common_info *j_cdbs;
		u64 cur_wall_time, cur_idle_time;
		unsigned int idle_time, wall_time;
		unsigned int load;
		int freq_avg;

		j_cdbs = dbs_data->cdata->get_cpu_cdbs(j);

		/*
		 * For the purpose of ondemand, waiting for disk IO is
		 * an indication that you're performance critical, and
		 * not that the system is actually idle. So do not add
		 * the iowait time to the cpu idle time.
		 */
		cur_idle_time = get_cpu_idle_time(j, &cur_wall_time, 0);

		wall_time = (unsigned int)
			(cur_wall_time - j_cdbs->prev_cpu_wall);
		j_cdbs->prev_cpu_wall = cur_wall_time;

		idle_time = (unsigned int)
			(cur_idle_time - j_cdbs->prev_cpu_idle);
		j_cdbs->prev_cpu_idle = cur_idle_time;

		if (unlikely(!wall_time || wall_time < idle_time))
			continue;

		load = 100 * (wall_time - idle_time) / wall_time;
		loads[j] = load;


#ifdef CONFIG_SCHED_HMP
		if (j < 4) {
			if (j == 0) {
				min_load = load;
				little_total_load = load;
				cpuid = 0;
			} else {
				little_total_load += load;
			}
			if (min_load > load) {
				min_load = load;
				cpuid = j;
			}
		}
#endif

		freq_avg = __cpufreq_driver_getavg(policy, j);
		if (freq_avg <= 0)
			freq_avg = policy->cur;

		load *= freq_avg;

		if (load > max_load)
			max_load = load;
	}

#ifdef CONFIG_SCHED_HMP
	hmp_slower_domain_avg_load = little_total_load/4;
	/*hmp_slower_domain_load = min_load;*/
	hmp_slower_domain_load = hmp_slower_domain_avg_load;
#endif

	dbs_data->cdata->gov_check_cpu(cpu, max_load);
}

static void optdemand_dbs_timer(struct work_struct *work)
{
	struct cpufreq_optdemand_cpuinfo *optdemand_cpuinfo =
		container_of(work, struct cpufreq_optdemand_cpuinfo, cdbs.work.work);
	struct cpufreq_policy *policy = optdemand_cpuinfo->cdbs.cur_policy;
	if(!policy || !policy->governor_enabled)
		return;
	unsigned int cpu = policy->cpu;
	cur_cpu = optdemand_cpuinfo->cdbs.cpu;
	struct cpufreq_optdemand_cpuinfo *pcpu = &per_cpu(cpuinfo, cpu);
	struct dbs_data *dbs_data = policy->governor_data;
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	unsigned int freq_cur;
	int delay = 1;
	bool modify_all = true;
	bool boosted;

	mutex_lock(&pcpu->cdbs.timer_mutex);
        freq_cur = policy->cur;

	if (!need_load_eval(&pcpu->cdbs, tunables->sampling_rate)) {
		modify_all = false;
    	trace_cpufreq_optdemand_notyet(cur_cpu, freq_cur);
		goto max_delay;
	}

    boosted = tunables->boost || tunables->input_event_boost;

	if (boosted) {
	    tunables->input_event_boost = false;
	    pcpu->rate_mult = tunables->boostpulse_duration / tunables->sampling_rate;
        if(policy->cur < tunables->hispeed_freq)
            __cpufreq_driver_target(policy, tunables->hispeed_freq, CPUFREQ_RELATION_L);
		trace_cpufreq_optdemand_boost_setrate(cur_cpu, freq_cur, max(freq_cur, tunables->hispeed_freq));
/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
		tele_mntn_cpufreq_optdemand_boost_setrate(policy, tunables->hispeed_freq);
#endif
#endif
/*===tele_mntn===*/
    }
    else {
	    optdemand_dbs_check_cpu(dbs_data, cpu);
    }

max_delay:
	gov_queue_work(dbs_data, optdemand_cpuinfo->cdbs.cur_policy, delay, modify_all);
	mutex_unlock(&pcpu->cdbs.timer_mutex);
}

static void cpufreq_optdemand_boost(struct dbs_data *dbs_data)
{
	struct cpufreq_optdemand_tunables *tunables;

    if(!cpufreq_optdemand_initialized)
        return;

	tunables = dbs_data->tuners;
    tunables->input_event_boost = true;
}

#ifdef CONFIG_INPUT_PULSE_SUPPORT

/*
 * Pulsed boost on input event raises CPUs to hispeed_freq.
 */
static void cpufreq_optdemand_input_event(struct input_handle *handle,
					    unsigned int type,
					    unsigned int code, int value)
{
	struct cpufreq_policy *policy;
	struct dbs_data *dbs_data;
 	unsigned int cpu = 0;

	if (type == EV_SYN && code == SYN_REPORT) {
		trace_cpufreq_optdemand_boost("input");
		policy = cpufreq_cpu_get(cpu);
		if (policy != NULL) {
		    dbs_data = policy->governor_data;

			cpufreq_optdemand_boost(dbs_data);
			cpufreq_cpu_put(policy);
		}
	}
}

static void cpufreq_optdemand_input_open(struct work_struct *w)
{
	struct cpufreq_optdemand_inputopen *io =
		container_of(w, struct cpufreq_optdemand_inputopen,
			     inputopen_work);
	int error;

	error = input_open_device(io->handle);
	if (error)
		input_unregister_handle(io->handle);
}

static int cpufreq_optdemand_input_connect(struct input_handler *handler,
					     struct input_dev *dev,
					     const struct input_device_id *id)
{
	struct input_handle *handle;
	int error;

	pr_info("%s: connect to %s\n", __func__, dev->name);
	handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	handle->dev = dev;
	handle->handler = handler;
	handle->name = "cpufreq_optdemand";

	error = input_register_handle(handle);
	if (error)
		goto err;

	inputopen.handle = handle;
	queue_work(down_wq, &inputopen.inputopen_work);
	return 0;
err:
	kfree(handle);
	return error;
}

static void cpufreq_optdemand_input_disconnect(struct input_handle *handle)
{
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

static const struct input_device_id cpufreq_optdemand_ids[] = {
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
			 INPUT_DEVICE_ID_MATCH_ABSBIT,
		.evbit = { BIT_MASK(EV_ABS) },
		.absbit = { [BIT_WORD(ABS_MT_POSITION_X)] =
			    BIT_MASK(ABS_MT_POSITION_X) |
			    BIT_MASK(ABS_MT_POSITION_Y) },
	}, /* multi-touch touchscreen */
	{
		.flags = INPUT_DEVICE_ID_MATCH_KEYBIT |
			 INPUT_DEVICE_ID_MATCH_ABSBIT,
		.keybit = { [BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH) },
		.absbit = { [BIT_WORD(ABS_X)] =
			    BIT_MASK(ABS_X) | BIT_MASK(ABS_Y) },
	}, /* touchpad */
	{ },
};

static struct input_handler cpufreq_optdemand_input_handler = {
	.event          = cpufreq_optdemand_input_event,
	.connect        = cpufreq_optdemand_input_connect,
	.disconnect     = cpufreq_optdemand_input_disconnect,
	.name           = "cpufreq_optdemand",
	.id_table       = cpufreq_optdemand_ids,
};

#endif /*CONFIG_INPUT_PULSE_SUPPORT*/


/************************** sysfs interface ************************/
static struct common_dbs_data optdemand_dbs_cdata;

static unsigned int *get_tokenized_data(const char *buf, int *num_tokens)
{
	const char *cp;
	int i;
	int ntokens = 1;
	unsigned int *tokenized_data;
	int err = -EINVAL;

	cp = buf;
	while ((cp = strpbrk(cp + 1, " :")))
		ntokens++;

	if (!(ntokens & 0x1))
		goto err;

	tokenized_data = kmalloc(ntokens * sizeof(unsigned int), GFP_KERNEL);
	if (!tokenized_data) {
		err = -ENOMEM;
		goto err;
	}

	cp = buf;
	i = 0;
	while (i < ntokens) {
		if (sscanf(cp, "%u", &tokenized_data[i++]) != 1)
			goto err_kfree;

		cp = strpbrk(cp, " :");
		if (!cp)
			break;
		cp++;
	}

	if (i != ntokens)
		goto err_kfree;

	*num_tokens = ntokens;
	return tokenized_data;

err_kfree:
	kfree(tokenized_data);
err:
	return ERR_PTR(err);
}

/*
 * No matter what frequency is, its down threshold must be smaller
 * than its up threshold.
 */
static unsigned int is_threshold_invalid(unsigned int *up, unsigned int *down,
        int nup, int ndown)
{
    int i = 0, j = 0;
    int freq_up, freq_down;

    if(!up || !down)
        return -EINVAL;

    while(i < nup && j < ndown)
    {
        if(up[i] <= down[j])
            return -EINVAL;

        if(i + 1 < nup) {
            freq_up = up[i + 1];
        }else {
            j += 2;
            continue;
        }

        if(j + 1 < ndown) {
            freq_down = down[j + 1];
        }else {
            i += 2;
            continue;
        }

        if(freq_up < freq_down) {
            i += 2;
        }else if(freq_up > freq_down) {
            j += 2;
        }else {
            i += 2;
            j += 2;
        }
    }

	return 0;
}

/**
 * update_sampling_rate - update sampling rate effective immediately if needed.
 * @new_rate: new sampling rate
 *
 * If new rate is smaller than the old, simply updating
 * dbs_tuners_int.sampling_rate might not be appropriate. For example, if the
 * original sampling_rate was 1 second and the requested new sampling rate is 10
 * ms because the user needs immediate reaction from ondemand governor, but not
 * sure if higher frequency will be required or not, then, the governor may
 * change the sampling rate too late; up to 1 second later. Thus, if we are
 * reducing the sampling rate, we need to make the new value effective
 * immediately.
 */
static void update_sampling_rate(struct dbs_data *dbs_data,
		unsigned int new_rate)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	int cpu;

	tunables->sampling_rate = new_rate = max(new_rate,
			dbs_data->min_sampling_rate);

	for_each_online_cpu(cpu) {
		struct cpufreq_policy *policy;
		struct cpufreq_optdemand_cpuinfo *pcpu;
		unsigned long next_sampling, appointed_at;

		policy = cpufreq_cpu_get(cpu);
		if (!policy)
			continue;
		if (policy->governor != &cpufreq_gov_optdemand) {
			cpufreq_cpu_put(policy);
			continue;
		}
		pcpu = &per_cpu(cpuinfo, cpu);
		cpufreq_cpu_put(policy);

		mutex_lock(&pcpu->cdbs.timer_mutex);

		if (!delayed_work_pending(&pcpu->cdbs.work)) {
			mutex_unlock(&pcpu->cdbs.timer_mutex);
			continue;
		}

		next_sampling = jiffies + usecs_to_jiffies(new_rate);
		appointed_at = pcpu->cdbs.work.timer.expires;

		if (time_before(next_sampling, appointed_at)) {

			mutex_unlock(&pcpu->cdbs.timer_mutex);
			cancel_delayed_work_sync(&pcpu->cdbs.work);
			mutex_lock(&pcpu->cdbs.timer_mutex);

			gov_queue_work(dbs_data, pcpu->cdbs.cur_policy,
					usecs_to_jiffies(new_rate), true);

		}
		mutex_unlock(&pcpu->cdbs.timer_mutex);
	}
}

static ssize_t store_sampling_rate(struct dbs_data *dbs_data, const char *buf,
		size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	update_sampling_rate(dbs_data, input);
	return count;
}

static ssize_t store_go_hispeed_load(struct dbs_data *dbs_data, const char *buf,
		size_t count)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > 100)
		return -EINVAL;

	tunables->go_hispeed_load = input;
	return count;
}

static ssize_t store_sampling_down_factor(struct dbs_data *dbs_data,
		const char *buf, size_t count)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	unsigned int input, j;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > MAX_SAMPLING_DOWN_FACTOR || input < 1)
		return -EINVAL;

	tunables->sampling_down_factor = input;

	/* Reset down sampling multiplier in case it was active */
	for_each_online_cpu(j) {
		struct cpufreq_optdemand_cpuinfo *pcpu = &per_cpu(cpuinfo, j);
		pcpu->rate_mult = 1;
	}
	return count;
}

static ssize_t store_boost(struct dbs_data *dbs_data, const char *buf,
        size_t count)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	tunables->boost = input;

	if (tunables->boost) {
		trace_cpufreq_optdemand_boost("on");
		cpufreq_optdemand_boost(dbs_data);
	} else {
		trace_cpufreq_optdemand_unboost("off");
	}

	return count;
}

static ssize_t store_boostpulse(struct dbs_data *dbs_data, const char *buf,
		size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	trace_cpufreq_optdemand_boost("pulse");
	cpufreq_optdemand_boost(dbs_data);
	return count;
}

static ssize_t store_boostpulse_duration(struct dbs_data *dbs_data, const char *buf,
		size_t count)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	tunables->boostpulse_duration = input;
	return count;
}

static ssize_t store_hispeed_freq(struct dbs_data *dbs_data, const char *buf,
		size_t count)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	tunables->hispeed_freq = input;
	return count;
}

static ssize_t show_down_threshold(struct dbs_data *dbs_data, char *buf)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	int i;
	ssize_t ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&tunables->threshold_lock, flags);

	for (i = 0; i < tunables->ndown_threshold; i++)
		ret += sprintf(buf + ret, "%u%s", tunables->down_threshold[i],
			       i & 0x1 ? ":" : " ");

	sprintf(buf + ret - 1, "\n");
	spin_unlock_irqrestore(&tunables->threshold_lock, flags);
	return ret;
}

static ssize_t store_down_threshold(struct dbs_data *dbs_data, const char *buf,
        size_t count)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	int ntokens;
	unsigned int *new_threshold = NULL;
	unsigned long flags;

	new_threshold = get_tokenized_data(buf, &ntokens);
	if (IS_ERR(new_threshold))
		return PTR_RET(new_threshold);

	spin_lock_irqsave(&tunables->threshold_lock, flags);
    if (is_threshold_invalid(tunables->up_threshold, new_threshold,
        tunables->nup_threshold, ntokens)) {
    	spin_unlock_irqrestore(&tunables->threshold_lock, flags);
        return -EINVAL;
    }

	if (tunables->down_threshold != default_down_threshold)
		kfree(tunables->down_threshold);
	tunables->down_threshold = new_threshold;
	tunables->ndown_threshold = ntokens;
	spin_unlock_irqrestore(&tunables->threshold_lock, flags);
	return count;
}


static ssize_t show_up_threshold(struct dbs_data *dbs_data, char *buf)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	int i;
	ssize_t ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&tunables->threshold_lock, flags);

	for (i = 0; i < tunables->nup_threshold; i++)
		ret += sprintf(buf + ret, "%u%s", tunables->up_threshold[i],
			       i & 0x1 ? ":" : " ");

	sprintf(buf + ret - 1, "\n");
	spin_unlock_irqrestore(&tunables->threshold_lock, flags);
	return ret;
}

static ssize_t store_up_threshold(struct dbs_data *dbs_data, const char *buf,
        size_t count)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	int ntokens;
	unsigned int *new_threshold = NULL;
	unsigned long flags;

	new_threshold = get_tokenized_data(buf, &ntokens);
	if (IS_ERR(new_threshold))
		return PTR_RET(new_threshold);

	spin_lock_irqsave(&tunables->threshold_lock, flags);
    if (is_threshold_invalid(new_threshold, tunables->down_threshold,
        ntokens, tunables->ndown_threshold)) {
    	spin_unlock_irqrestore(&tunables->threshold_lock, flags);
        return -EINVAL;
    }

	if (tunables->up_threshold != default_up_threshold)
		kfree(tunables->up_threshold);
	tunables->up_threshold = new_threshold;
	tunables->nup_threshold = ntokens;
	spin_unlock_irqrestore(&tunables->threshold_lock, flags);
	return count;
}


/*
 * Create show/store routines
 * - sys: One governor instance for complete SYSTEM
 * - pol: One governor instance per struct cpufreq_policy
 */
#define show_optdemand_gov_pol_sys(file_name)					\
static ssize_t show_##file_name##_gov_sys				\
(struct kobject *kobj, struct attribute *attr, char *buf)		\
{									\
	struct cpufreq_optdemand_tunables *tunables = optdemand_dbs_cdata.gdbs_data->tuners;   \
	return sprintf(buf, "%u\n", tunables->file_name);		\
}									\
									\
static ssize_t show_##file_name##_gov_pol				\
(struct cpufreq_policy *policy, char *buf)				\
{									\
	struct dbs_data *dbs_data = policy->governor_data;		\
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;		\
	return sprintf(buf, "%u\n", tunables->file_name);	\
}

#define store_optdemand_gov_pol_sys(file_name)					\
static ssize_t store_##file_name##_gov_sys				\
(struct kobject *kobj, struct attribute *attr, const char *buf, size_t count)	\
{									\
	struct dbs_data *dbs_data = optdemand_dbs_cdata.gdbs_data;		\
	return store_##file_name(dbs_data, buf, count);		\
}									\
									\
static ssize_t store_##file_name##_gov_pol				\
(struct cpufreq_policy *policy, const char *buf, size_t count)		\
{									\
	struct dbs_data *dbs_data = policy->governor_data;	\
	return store_##file_name(dbs_data, buf, count);		\
}

#define show_store_optdemand(file_name)					\
show_optdemand_gov_pol_sys(file_name);						\
store_optdemand_gov_pol_sys(file_name)


#define show_optdemand_threshold(file_name)             \
static ssize_t show_##file_name##_gov_sys				\
(struct kobject *kobj, struct attribute *attr, char *buf)		\
{									\
	struct dbs_data *dbs_data = optdemand_dbs_cdata.gdbs_data;		\
	return show_##file_name(dbs_data, buf);	\
}									\
									\
static ssize_t show_##file_name##_gov_pol				\
(struct cpufreq_policy *policy, char *buf)				\
{									\
	struct dbs_data *dbs_data = policy->governor_data;		\
	return show_##file_name(dbs_data, buf);	\
}

#define show_store_optdemand_threshold(file_name)       \
show_optdemand_threshold(file_name);                    \
store_optdemand_gov_pol_sys(file_name)


show_store_optdemand(sampling_rate);
show_store_optdemand(go_hispeed_load);
show_store_optdemand(hispeed_freq);
show_store_optdemand(sampling_down_factor);
show_store_optdemand(boost);
show_store_optdemand(boostpulse_duration);
store_optdemand_gov_pol_sys(boostpulse);
show_store_optdemand_threshold(up_threshold);
show_store_optdemand_threshold(down_threshold);
declare_show_sampling_rate_min(optdemand);

gov_sys_pol_attr_rw(sampling_rate);
gov_sys_pol_attr_rw(go_hispeed_load);
gov_sys_pol_attr_rw(hispeed_freq);
gov_sys_pol_attr_rw(sampling_down_factor);
gov_sys_pol_attr_rw(boost);
gov_sys_pol_attr_rw(boostpulse_duration);
gov_sys_pol_attr_rw(up_threshold);
gov_sys_pol_attr_rw(down_threshold);
gov_sys_pol_attr_ro(sampling_rate_min);

static struct global_attr boostpulse_gov_sys =
	__ATTR(boostpulse, 0200, NULL, store_boostpulse_gov_sys);

static struct freq_attr boostpulse_gov_pol =
	__ATTR(boostpulse, 0200, NULL, store_boostpulse_gov_pol);


static struct attribute *dbs_attributes_gov_sys[] = {
	&sampling_rate_min_gov_sys.attr,
	&sampling_rate_gov_sys.attr,
	&go_hispeed_load_gov_sys.attr,
	&hispeed_freq_gov_sys.attr,
	&sampling_down_factor_gov_sys.attr,
	&boost_gov_sys.attr,
	&boostpulse_gov_sys.attr,
	&boostpulse_duration_gov_sys.attr,
	&up_threshold_gov_sys.attr,
	&down_threshold_gov_sys.attr,
	NULL
};

static struct attribute_group optdemand_attr_group_gov_sys = {
	.attrs = dbs_attributes_gov_sys,
	.name = "optdemand",
};

static struct attribute *dbs_attributes_gov_pol[] = {
	&sampling_rate_min_gov_pol.attr,
	&sampling_rate_gov_pol.attr,
	&go_hispeed_load_gov_pol.attr,
	&hispeed_freq_gov_pol.attr,
	&sampling_down_factor_gov_pol.attr,
	&boost_gov_pol.attr,
	&boostpulse_gov_pol.attr,
	&boostpulse_duration_gov_pol.attr,
	&up_threshold_gov_pol.attr,
	&down_threshold_gov_pol.attr,
	NULL
};

static struct attribute_group optdemand_attr_group_gov_pol = {
	.attrs = dbs_attributes_gov_pol,
	.name = "optdemand",
};

static struct attribute_group *get_sysfs_attr(void)
{
	if (have_governor_per_policy())
		return &optdemand_attr_group_gov_pol;
	else
		return &optdemand_attr_group_gov_sys;
}


/************************** sysfs end ************************/

/*
 * Get up&down threshold for optdemand governor from dt, if it is not
 * configed or not correct configed, use default threshold.
 */
static void dt_get_threshold(void)
{
	struct device_node *np = NULL;
	const struct property *prop;
	const __be32 *val;
	unsigned int *up_table, *down_table;
	int nr, i = 1;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,smp-scaling-threshold");
	if (!np)
	    return;

	prop = of_find_property(np, "operating-points", NULL);
	if (!prop)
		return;
	if (!prop->value)
		return;

	/*
	 * Each OPP is a set of tuples consisting of frequency and
	 * threshold like <freq-kHz upthreshold-% downthreshold-%>.
	 */
	nr = prop->length / sizeof(u32);
	if (nr % 3) {
		pr_err("%s: Invalid cpufreq threshold config\n", __func__);
		return;
	}

	up_table = kzalloc(sizeof(int) * (2 * nr / 3 + 1), GFP_KERNEL);
    if(!up_table) {
		pr_warn("%s: Unable to allocate up threshold table\n", __func__);
		return;
    }
    nup_threshold = 2 * nr / 3 + 1;

    down_table = kzalloc(sizeof(int) * (2 * nr / 3 + 1), GFP_KERNEL);
    if(!down_table) {
        kfree(up_table);
		pr_warn("%s: Unable to allocate down threshold table\n", __func__);
		return;
    }
    ndown_threshold = 2 * nr / 3 + 1;

	val = prop->value;

    /* the threshold must be arrayed like <load-% freq-kHz load-%.....>
     * the first and last element must be load.
     */
    up_table[0] = be32_to_cpup(val + 1);
    down_table[0] = be32_to_cpup(val + 2);
	while (nr) {
		unsigned int freq = be32_to_cpup(val++);
		unsigned int up_thres = be32_to_cpup(val++);
		unsigned int down_thres = be32_to_cpup(val++);

        up_table[i] = freq;
        down_table[i] = freq;
        up_table[i + 1] = up_thres;
        down_table[i + 1] = down_thres;
        i += 2;
		nr -= 3;
	}

	up_threshold = &up_table[0];
	down_threshold = &down_table[0];
}

static int optdemand_init(struct dbs_data *dbs_data)
{
	struct cpufreq_optdemand_tunables *tunables;
	u64 idle_time;
	int cpu;

	tunables = kzalloc(sizeof(struct cpufreq_optdemand_tunables), GFP_KERNEL);
	if (!tunables) {
		pr_err("%s: kzalloc failed\n", __func__);
		return -ENOMEM;
	}

	cpu = get_cpu();
	idle_time = get_cpu_idle_time_us(cpu, NULL);
	put_cpu();
	if (idle_time != -1ULL) {
		/*
		 * In nohz/micro accounting case we set the minimum frequency
		 * not depending on HZ, but fixed (very low). The deferred
		 * timer might skip some samples if idle/sleeping as needed.
		*/
		dbs_data->min_sampling_rate = MICRO_FREQUENCY_MIN_SAMPLE_RATE;
	} else {
		/* For correct statistics, we need 10 ticks for each measure */
		dbs_data->min_sampling_rate = MIN_SAMPLING_RATE_RATIO *
			jiffies_to_usecs(10);
	}

	tunables->sampling_down_factor = DEF_SAMPLING_DOWN_FACTOR;
	tunables->go_hispeed_load = DEF_GO_HISPEED_LOAD;
	tunables->boostpulse_duration = DEF_BOOST_PULSE_DURATION;

    /* check the threshold get from dt */
    if(is_threshold_invalid(up_threshold, down_threshold,
        nup_threshold, ndown_threshold)) {
    	tunables->up_threshold = default_up_threshold;
    	tunables->nup_threshold = ARRAY_SIZE(default_up_threshold);
    	tunables->down_threshold = default_down_threshold;
    	tunables->ndown_threshold = ARRAY_SIZE(default_down_threshold);
    }else {
    	tunables->up_threshold = up_threshold;
    	tunables->nup_threshold = nup_threshold;
    	tunables->down_threshold = down_threshold;
    	tunables->ndown_threshold = ndown_threshold;
    }

	spin_lock_init(&tunables->threshold_lock);

	dbs_data->tuners = tunables;
	mutex_init(&dbs_data->mutex);
	return 0;
}

static void optdemand_exit(struct dbs_data *dbs_data)
{
	kfree(dbs_data->tuners);
}

define_get_cpu_dbs_routines(cpuinfo);

static struct common_dbs_data optdemand_dbs_cdata = {
	.attr_group_gov_sys = &optdemand_attr_group_gov_sys,
	.attr_group_gov_pol = &optdemand_attr_group_gov_pol,
	.get_cpu_cdbs = get_cpu_cdbs,
	.get_cpu_dbs_info_s = get_cpu_dbs_info_s,
	.gov_dbs_timer = optdemand_dbs_timer,
	.gov_check_cpu = optdemand_check_cpu,
	.init = optdemand_init,
	.exit = optdemand_exit,
};

static inline void gov_cancel_work(struct dbs_data *dbs_data,
		struct cpufreq_policy *policy)
{
	struct cpu_dbs_common_info *cdbs;
	int i;

	for_each_cpu(i, policy->cpus) {
		cdbs = dbs_data->cdata->get_cpu_cdbs(i);
		cancel_delayed_work_sync(&cdbs->work);
	}
}

static void set_sampling_rate(struct dbs_data *dbs_data,
		unsigned int sampling_rate)
{
	struct cpufreq_optdemand_tunables *tunables = dbs_data->tuners;
	tunables->sampling_rate = sampling_rate;
}

static int cpufreq_governor_optdemand(struct cpufreq_policy *policy,
		unsigned int event)
{
    struct common_dbs_data *cdata = &optdemand_dbs_cdata;
	struct dbs_data *dbs_data;
	struct cpufreq_optdemand_cpuinfo *cpuinfo = NULL;
	struct cpufreq_optdemand_tunables *tunables = NULL;
	struct cpu_dbs_common_info *cpu_cdbs;
	unsigned int sampling_rate, latency, j, cpu = policy->cpu;

	int rc;

	if (have_governor_per_policy())
		dbs_data = policy->governor_data;
	else
		dbs_data = cdata->gdbs_data;

	WARN_ON(!dbs_data && (event != CPUFREQ_GOV_POLICY_INIT));

	switch (event) {
	case CPUFREQ_GOV_POLICY_INIT:
		if (have_governor_per_policy()) {
			WARN_ON(dbs_data);
		} else if (dbs_data) {
			dbs_data->usage_count++;
			policy->governor_data = dbs_data;
			return 0;
		}

		dbs_data = kzalloc(sizeof(*dbs_data), GFP_KERNEL);
		if (!dbs_data) {
			pr_err("%s: POLICY_INIT: kzalloc failed\n", __func__);
			return -ENOMEM;
		}

		dbs_data->cdata = cdata;
		dbs_data->usage_count = 1;
		rc = cdata->init(dbs_data);
		if (rc) {
			pr_err("%s: POLICY_INIT: init() failed\n", __func__);
			kfree(dbs_data);
			return rc;
		}

		rc = sysfs_create_group(get_governor_parent_kobj(policy), get_sysfs_attr());
		if (rc) {
			cdata->exit(dbs_data);
			kfree(dbs_data);
			return rc;
		}

#ifdef CONFIG_INPUT_PULSE_SUPPORT
		if (policy->cpu == 0) {
			rc = input_register_handler(&cpufreq_optdemand_input_handler);
			if (rc)
				pr_warn("%s: failed to register input handler\n",
					__func__);
		}
#endif /*#ifdef CONFIG_INPUT_PULSE_SUPPORT*/

		policy->governor_data = dbs_data;

		/* policy latency is in nS. Convert it to uS first */
		latency = policy->cpuinfo.transition_latency / 1000;
		if (latency == 0)
			latency = 1;

		/* Bring kernel and HW constraints together */
		dbs_data->min_sampling_rate = max(dbs_data->min_sampling_rate,
				MIN_LATENCY_MULTIPLIER * latency);
		set_sampling_rate(dbs_data, max(dbs_data->min_sampling_rate,
					latency * LATENCY_MULTIPLIER));

		if (!have_governor_per_policy())
			cdata->gdbs_data = dbs_data;

		return 0;
	case CPUFREQ_GOV_POLICY_EXIT:
		if (!--dbs_data->usage_count) {
			sysfs_remove_group(get_governor_parent_kobj(policy), get_sysfs_attr());

#ifdef CONFIG_INPUT_PULSE_SUPPORT
		if (policy->cpu == 0) {
			input_unregister_handler(&cpufreq_optdemand_input_handler);
		}
#endif /*#ifdef CONFIG_INPUT_PULSE_SUPPORT*/

			cdata->exit(dbs_data);
			kfree(dbs_data);
			cdata->gdbs_data = NULL;
		}

		policy->governor_data = NULL;
		return 0;
	}

	tunables = dbs_data->tuners;

	cpu_cdbs = dbs_data->cdata->get_cpu_cdbs(cpu);
	cpuinfo = dbs_data->cdata->get_cpu_dbs_info_s(cpu);
    sampling_rate = tunables->sampling_rate;

	switch (event) {
	case CPUFREQ_GOV_START:
		if (!policy->cur)
			return -EINVAL;

		mutex_lock(&dbs_data->mutex);

		if (!tunables->hispeed_freq)
    		tunables->hispeed_freq = DEF_HISPEED_FREQ;

		for_each_cpu(j, policy->cpus) {
			struct cpu_dbs_common_info *j_cdbs =
				dbs_data->cdata->get_cpu_cdbs(j);

			j_cdbs->cpu = j;
			j_cdbs->cur_policy = policy;
			j_cdbs->prev_cpu_idle = get_cpu_idle_time(j,
					       &j_cdbs->prev_cpu_wall, 0);

			mutex_init(&j_cdbs->timer_mutex);
			INIT_DEFERRABLE_WORK(&j_cdbs->work,
					     dbs_data->cdata->gov_dbs_timer);
		}

		cpuinfo->rate_mult = 1;
        cpufreq_optdemand_initialized = 1;
        #ifdef CONFIG_SCHED_HMP
        hmp_load_ctrl_flag = 1;
        #endif

		mutex_unlock(&dbs_data->mutex);

		/* Initiate timer time stamp */
		cpu_cdbs->time_stamp = ktime_get();

		gov_queue_work(dbs_data, policy,
				delay_for_sampling_rate(sampling_rate), true);

		break;

	case CPUFREQ_GOV_STOP:
        cpufreq_optdemand_initialized = 0;
        #ifdef CONFIG_SCHED_HMP
        hmp_load_ctrl_flag = 0;
        #endif
		gov_cancel_work(dbs_data, policy);

		mutex_lock(&dbs_data->mutex);
		mutex_destroy(&cpu_cdbs->timer_mutex);
		cpu_cdbs->cur_policy = NULL;
		mutex_unlock(&dbs_data->mutex);

		break;

	case CPUFREQ_GOV_LIMITS:
		mutex_lock(&dbs_data->mutex);
		if (!cpu_cdbs->cur_policy) {
			mutex_unlock(&dbs_data->mutex);
			break;
		}
		mutex_lock(&cpu_cdbs->timer_mutex);
		if (policy->max < cpu_cdbs->cur_policy->cur)
			__cpufreq_driver_target(cpu_cdbs->cur_policy,
					policy->max, CPUFREQ_RELATION_H);
		else if (policy->min > cpu_cdbs->cur_policy->cur)
			__cpufreq_driver_target(cpu_cdbs->cur_policy,
					policy->min, CPUFREQ_RELATION_L);
		optdemand_dbs_check_cpu(dbs_data, cpu);
		mutex_unlock(&cpu_cdbs->timer_mutex);
		mutex_unlock(&dbs_data->mutex);
		break;
	}
	return 0;
}

#ifndef CONFIG_CPU_FREQ_DEFAULT_GOV_OPTDEMAND
static
#endif
struct cpufreq_governor cpufreq_gov_optdemand = {
	.name			= "optdemand",
	.governor		= cpufreq_governor_optdemand,
	.max_transition_latency	= TRANSITION_LATENCY_LIMIT,
	.owner			= THIS_MODULE,
};

static int __init cpufreq_optdemand_init(void)
{
#ifdef CONFIG_INPUT_PULSE_SUPPORT
	/* No rescuer thread, bind to CPU queuing the work for possibly
	   warm cache (probably doesn't matter much). */
	down_wq = alloc_workqueue("optdemand_down", 0, 1);

	if (!down_wq)
    	return -ENOMEM;

	INIT_WORK(&inputopen.inputopen_work, cpufreq_optdemand_input_open);
#endif /*#ifdef CONFIG_INPUT_PULSE_SUPPORT*/

	dt_get_threshold();
	memset(loads, 0, sizeof(long) * NR_CPUS);

	return cpufreq_register_governor(&cpufreq_gov_optdemand);
}

static void __exit cpufreq_optdemand_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_gov_optdemand);
}

MODULE_AUTHOR("Yu Wei <yuwei3@hisilicon.com>");
MODULE_AUTHOR("Mo Haoju <mohaoju@hisilicon.com>");
MODULE_DESCRIPTION("'cpufreq_optdemand' - A dynamic cpufreq governor for "
	"Low Latency Frequency Transition capable processors with boost support");
MODULE_LICENSE("GPL");

#ifdef CONFIG_CPU_FREQ_DEFAULT_GOV_OPTDEMAND
fs_initcall(cpufreq_optdemand_init);
#else
module_init(cpufreq_optdemand_init);
#endif
module_exit(cpufreq_optdemand_exit);
