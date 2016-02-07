/*
 * Copyright (c) Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.
 *
 * File name: hw_kcollect.c
 * Description: This file use to collect kernel state and report it.
 * Author: chenyouzhen@huawei.com
 * Version: 0.1
 * Date: 2014/07/21
 */
#include <linux/time.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <linux/suspend.h>
#include <linux/err.h>
#include <linux/hw_kcollect.h>
#include <linux/hw_kstate.h>

struct kcollect_info {
	s32 mask;
	u32 tv_sec;	/* seconds */
	u32 tv_usec;	/* microseconds */
	u32 len;
	char buffer[KCOLLECT_BUFFER_SIZE];
};

static int switch_mask = 0;
static spinlock_t ktime_lock;
static bool ktime_disable = false;
static int killed_pid = -1;

/*
 * Function: pg_set_ktime_enable
 * Description: set the ktime_disable
 * Input: val -- true(disable),false(enable)
**/
static void set_ktime_disable(bool val)
{
	spin_lock(&ktime_lock);
	ktime_disable = val;
	spin_unlock(&ktime_lock);
}

/*
 * Function: pg_set_ktime_enable
 * Description: set the ktime_disable
 * Return: ret -- true(disable),false(enable)
**/
static bool get_ktime_disable(void)
{
	bool ret;

	spin_lock(&ktime_lock);
	ret = ktime_disable;
	spin_unlock(&ktime_lock);
	return ret;
}

/*
  * Function: suspend_notify
  * Description: suspend notify call back
  * Ruturn: 0 -- success
 **/
static int suspend_notify(struct notifier_block *notify_block,
				unsigned long mode, void *unused)
{
	switch (mode) {
		case PM_POST_SUSPEND:
			set_ktime_disable(false);
			kcollect(KCOLLECT_SUSPEND_MASK, "PM_POST_SUSPEND");
			break;
		case PM_SUSPEND_PREPARE:
			kcollect(KCOLLECT_SUSPEND_MASK, "PM_SUSPEND_PREPARE");
			set_ktime_disable(true);
			break;
		default:
			break;
	}
	return 0;
}

static struct notifier_block suspend_notifier = {
	.notifier_call = suspend_notify,
	.priority = INT_MIN,
};

/*
 * Function: kstate_set_notifier
 * Description: register or unregister notifier
 * Input: mask
**/
#define PRE_MASK_IS_OFF(type, mask) (type & mask & (~switch_mask)) // true, previous state is off
#define PRE_MASK_IS_ON(type, mask) (type & ~mask & switch_mask)    // true, previous state is on
static void set_pm_notifier(int mask)
{
	int ret = 0;
	bool val = (mask & KCOLLECT_SUSPEND_MASK) ? true : false;

	if (val) { // previous state is off, and now is on
		if (PRE_MASK_IS_OFF(KCOLLECT_SUSPEND_MASK, mask)) {
			ret = register_pm_notifier(&suspend_notifier);
			if (ret < 0) {
				pr_err("hw_kcollect %s : register_pm_notifier failed!\n", __func__);
			} else {
				pr_debug("hw_kcollect %s : register_pm_notifier\n", __func__);
			}
		}
	} else { // previous state is on, and now is off
		if (PRE_MASK_IS_ON(KCOLLECT_SUSPEND_MASK, mask)) {
			ret = unregister_pm_notifier(&suspend_notifier);
			if (ret < 0) {
				pr_err("hw_kcollect %s : unregister_pm_notifier failed!\n", __func__);
			} else {
				pr_debug("hw_kcollect %s : unregister_pm_notifier\n", __func__);
			}
		}
	}
}

/*
  * Function: kcollect_cb
  * Description: kstate call back
  * Return: -1 -- failed, 0 -- success
**/
static int kcollect_cb(CHANNEL_ID src, PACKET_TAG tag, const char *data, size_t len)
{
	int mask = 0;

	if (IS_ERR_OR_NULL(data) || len != sizeof(int)) {
		pr_err("hw_kcollect %s: invalid data or len\n", __func__);
		return -1;
	}

	memcpy(&mask, data, len);
	set_pm_notifier(mask);
	switch_mask = mask;

	pr_debug("hw_kcollect %s: src=%d tag=%d len=%ld pg_switch_mask=%d\n", __func__, src, tag, len, switch_mask);
	return 0;
}

static struct kstate_opt kcollect_opt = {
	.name = "kcollect",
	.tag = PACKET_TAG_KCOLLECT,
	.dst = CHANNEL_ID_KCOLLECT,
	.hook = kcollect_cb,
};


/*
  * Function: report
  * Description: Packet and Send data to userspace by kstate
  * Input: mask -- message mask
  *        fmt -- string
  * Return: -1--failed, 0--success
**/
static int report(int mask, va_list args, const char *fmt)
{
	int ret = -1;
	struct kcollect_info info;
	struct timeval time;
	int length = 0;
	size_t info_len = 0;

	memset(&info, 0, sizeof(info));
	length = vscnprintf(info.buffer, KCOLLECT_BUFFER_SIZE - 1, fmt, args);
	if (length > 0) {
		info.mask = mask;
		info.len = length + 1;
		if (!get_ktime_disable()) {
			do_gettimeofday(&time);
		}
		info.tv_sec = (u32)time.tv_sec;
		info.tv_usec = (u32)time.tv_usec;
		info_len = sizeof(info) - KCOLLECT_BUFFER_SIZE + length + 1;
		ret = kstate(CHANNEL_ID_KCOLLECT, PACKET_TAG_KCOLLECT, (char*)&info, info_len);
		if (ret < 0) {
			pr_err("hw_kcollect %s: kstate error\n", __func__);
			ret = -1;
		}
	}
	pr_debug("hw_kcollect %s: length=%d mask=%d\n", __func__, length, mask);
	return ret;
}

int hwbinderinfo(int callingpid, int calledpid)
{
	return kcollect(KCOLLECT_BINDERCALL_MASK, "[TPID %d CALLED][PID %d]", calledpid, callingpid);
}

int hwkillinfo(int pid, int seg)
{
	int ret = -1;
	if (killed_pid != pid) {
		ret = kcollect(KCOLLECT_FREEZER_MASK, "[PID %d KILLED][SIG %d]", pid, seg);
		killed_pid = pid;
	}
	return ret;
}

/*
  * Function: kcollect
  * Description: collect the data and report system
  * Input: mask -- message mask
  *		fmt -- string
  * Return: -1--failed, 0--success
**/
int kcollect(int mask, const char *fmt, ...)
{
	va_list args;
	int ret = -1;

	if (mask & switch_mask) {
		va_start(args, fmt);
		ret = report(mask, args, fmt);
		va_end(args);
	}
	return ret;
}

static int __init kcollect_init(void)
{
	int ret = -1;

	ret = kstate_register_hook(&kcollect_opt);
	if (ret < 0) {
		pr_err("hw_kcollect %s: kstate_register_hook error\n", __func__);
	} else {
		pr_info("hw_kcollect %s: kstate_register_hook success\n", __func__);
	}
	return ret;
}

static void __exit kcollect_exit(void)
{
	kstate_unregister_hook(&kcollect_opt);
}

late_initcall(kcollect_init);
module_exit(kcollect_exit);

MODULE_LICENSE("Dual BSD/GPL");
