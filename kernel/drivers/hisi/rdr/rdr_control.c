/*
 * control the rdr. (RDR: kernel run data recorder.)
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/errno.h>
#include <linux/delay.h>

#include <linux/huawei/rdr.h>
#include <linux/huawei/rdr_private.h>
#include "rdr_internal.h"

#ifdef CONFIG_HISI_RDR_SWITCH
#define rdr_attr(_name) \
static struct kobj_attribute _name##_attr = {   \
	.attr   = {             \
		.name = __stringify(_name), \
		.mode = 0644,           \
	},                  \
	.show   = _name##_show,         \
	.store  = _name##_store,        \
}
#endif

static ssize_t dump_info_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	if ((buf != NULL) && (*buf != '\0') && (!strncmp(buf, "dump", 4)))
		dump_save();
	rdr_rm_over3_file(OM_ROOT_PATH);

	return count;
}

#ifdef CONFIG_HISI_RDR_SWITCH
static int rdr_task_switch_state;
static ssize_t task_switch_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE - 1, "%d\n", rdr_task_switch_state);
}

static ssize_t task_switch_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val;

	if (kstrtoint(buf, 10, &val))
		return -EINVAL;

	if (rdr_task_switch_state == val)
		return (ssize_t)count;

	if (val > 1)
		return -EINVAL;

	if (val == 1) {
		(void)task_switch_hook_add(
			(rdr_funcptr_2)dump_task_switch_hook);
	} else {
		(void)task_switch_hook_del(
			(rdr_funcptr_2)dump_task_switch_hook);
	}

	rdr_task_switch_state = val;
	return (ssize_t)count;
}
rdr_attr(task_switch);

static int rdr_int_switch_state;
static ssize_t int_switch_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE - 1, "%d\n", rdr_int_switch_state);
}

static ssize_t int_switch_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val;

	if (kstrtoint(buf, 10, &val))
		return -EINVAL;

	if (rdr_int_switch_state == val)
		return (ssize_t)count;

	if (val > 1)
		return -EINVAL;

	if (val == 1)
		int_switch_hook_add(dump_int_switch_hook);
	else
		int_switch_hook_delete();

	rdr_int_switch_state = val;
	return (ssize_t)count;
}
rdr_attr(int_switch);

static int rdr_system_call_state;
static ssize_t sys_call_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return snprintf(buf, PAGE_SIZE - 1, "%d\n", rdr_system_call_state);
}

static ssize_t sys_call_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val;

	if (kstrtoint(buf, 10, &val))
		return -EINVAL;

	if (rdr_system_call_state == val)
		return (ssize_t)count;

	if (val > 1)
		return -EINVAL;

	if (val == 1)
		atomic_set(&syscall_hook_on, 1);
	else
		atomic_set(&syscall_hook_on, 0);

	rdr_system_call_state = val;
	return (ssize_t)count;
}
rdr_attr(sys_call);
#endif

static struct kobj_attribute dump_info_attribute =
	__ATTR(savefile, 0220, NULL, dump_info_store);

static struct attribute *attrs[] = {
	&dump_info_attribute.attr,
#ifdef CONFIG_HISI_RDR_SWITCH
	&task_switch_attr.attr,
	&int_switch_attr.attr,
	&sys_call_attr.attr,
#endif
	NULL
};
static struct attribute_group rdr_attr_group = {
	.attrs = attrs
};

struct kobject *rdr_kobj;

int sys_add_rdr_node(void)
{
	int retval;

	/* wait for kernel_kobj node ready: */
	while (kernel_kobj == NULL)
		msleep(1000);

	/* Create kobject named "rdr",located under /sys/kernel/ */
	rdr_kobj = kobject_create_and_add("rdr", kernel_kobj);
	if (!rdr_kobj)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(rdr_kobj, &rdr_attr_group);
	if (retval)
		kobject_put(rdr_kobj);
	return retval;
}

void sys_del_rdr_node(void)
{
	sysfs_remove_group(rdr_kobj, &rdr_attr_group);
	kobject_put(rdr_kobj);
}

void dump_config_init(void)
{
	/* use default: */
	g_dump_cfg.dump_cfg.bits.dump_switch	= 0x1;
	g_dump_cfg.dump_cfg.bits.arm_exc	= 0x1;
	g_dump_cfg.dump_cfg.bits.stack_flow	= 0x1;
	g_dump_cfg.dump_cfg.bits.task_switch	= 0x1;
	g_dump_cfg.dump_cfg.bits.int_switch	= 0x1;
	g_dump_cfg.dump_cfg.bits.int_lock	= 0x1;
	g_dump_cfg.dump_cfg.bits.app_reg_save1	= 0x1;
	g_dump_cfg.dump_cfg.bits.app_reg_save2	= 0x1;
	g_dump_cfg.dump_cfg.bits.app_reg_save3	= 0x1;
	g_dump_cfg.dump_cfg.bits.comm_reg_save1	= 0x0;
	g_dump_cfg.dump_cfg.bits.comm_reg_save2	= 0x0;
	g_dump_cfg.dump_cfg.bits.comm_reg_save3	= 0x0;
	g_dump_cfg.dump_cfg.bits.sys_err_reboot	= 0x1;
	g_dump_cfg.dump_cfg.bits.reset_log	= 0x1;
	g_dump_cfg.dump_cfg.bits.fetal_err	= 0x1;
	g_dump_cfg.dump_cfg.bits.suspend_resume	= 0x1;

	return;
}
