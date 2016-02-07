/*
 * rdr test unit. (RDR: kernel run data recorder.)
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
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/thread_info.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#include <linux/huawei/rdr.h>
#include "rdr_internal.h"

#include <linux/vmalloc.h>
void test_rdr_create_dir(void)
{
	rdr_create_dir("/data/log1/log2/log3/a/b/c/d/1/2/3/");
}

void rdr_test_vmalloc(void)
{
	char *p = vmalloc(400 * 1024 * 1024);
	if (p == NULL)
		pr_err("rdr:vmalloc failed\n");
	else {
		memset(p, 0, 400 * 1024 * 1024);
		pr_info("rdr:vmalloc success\n");
		vfree(p);
	}
}

/**
 * Due to file permission test failed in cts, we should not call function
 * test_exc_dataabort() in sys node. We may call this function in ecall.
 */
static void exc_dataabort(void)
{
	int a;
	a = *(int *)0x1;
	*(int *)0x1 = 0x12345678;
	pr_info("rdr:a:%d\n", a);
}

void test_exc_dataabort(void)
{
	exc_dataabort();
	return;
}
EXPORT_SYMBOL_GPL(test_exc_dataabort);

static u8 *rdr_028_buf;
void rdr_st_hook(void)
{
	pr_info("rdr:dump_st_hook enter\n");
	if (rdr_028_buf) {
		memset(rdr_028_buf, 0xaa, 0x800);
		memset(rdr_028_buf + 0x800, 0xbb, 0x800);
	}
}

/*
 * reg a field,size is 4K.
 * write 0xCC and 0xDD to this field.
 * regist exc hook,this hook write 0xAA and 0xBB to this field.
 * call system_error, first copy field to user field(0xCC/0xDD)
 * then 0xCC/0xDD is modified by hook, change to 0xAA/0xBB.
*/
int rdr_st_028(void)
{
	int ret;

	rdr_028_buf = rdr_balong_reg_field(0x01100002, 0x1000);

	if (NULL == rdr_028_buf) {
		pr_err("rdr:get buffer failed.  test case 28 failed!\n");
		return -2;
	}

	memset(rdr_028_buf, 0xCC, 0x800);
	memset(rdr_028_buf + 0x800, 0xDD, 0x800);

	ret = rdr_dump_register_hook(0x01100002, (void *)rdr_st_hook);
	if (0 != ret) {
		pr_err("rdr:register_hook failed.  test case 28 failed!\n");
		return -1;
	}

	hisi_system_error(12, 1, 2, rdr_028_buf, 0x1000);
	return 0;
}
EXPORT_SYMBOL_GPL(rdr_st_028);

static ssize_t test_rdr_interface(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	int ret = snprintf(buf, PAGE_SIZE - 1,
		"Please run \"ecall test_exc_dataabort\"\n");

	return ret;
}

static ssize_t test_buddy_fail(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	int alloc_size = 5 * 1024 * 1024;
	int ret = snprintf(buf, PAGE_SIZE - 1, "test:%s\n", __func__);

	char *my_mem = kmalloc(alloc_size, GFP_KERNEL);
	if (my_mem == NULL)
		pr_err("%s() kmalloc failed ...\n", __func__);

	if (my_mem) {
		strncpy(my_mem, "abccddddddddd", alloc_size - 1);
		my_mem[alloc_size - 1] = '\0';
		pr_info("wdd:my_mem:%s\n", my_mem);
		kfree(my_mem);
	}

	return ret;
}

#ifdef CONFIG_DETECT_HUNG_TASK
static int rdr_test_hung_task_body(void *arg)
{
	int cnt = 1000;
	while (cnt) {
		pr_info("rdr test hung task ... sleep 300s\n");
		msleep(300000);
		cnt--;
	}
	return 0;
}

static ssize_t test_hung_task(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	int ret = snprintf(buf, PAGE_SIZE - 1, "test:%s\n", __func__);

	if (!kthread_run(rdr_test_hung_task_body, NULL, "hung_task_thread")) {
		DUMP_LOG(0);
		return -1;
	}

	return ret;
}
#endif

static struct kobj_attribute test_rdr_interface_attr =
	__ATTR(test_rdr_interface, 0444, test_rdr_interface, NULL);
static struct kobj_attribute test_buddy_fail_attr =
	__ATTR(test_buddy_fail, 0444, test_buddy_fail, NULL);
#ifdef CONFIG_DETECT_HUNG_TASK
static struct kobj_attribute test_hung_task_attr =
	__ATTR(test_hung_task, 0444, test_hung_task, NULL);
#endif

static const struct attribute *rdr_test_attrs[] = {
	&test_rdr_interface_attr.attr,
	&test_buddy_fail_attr.attr,
#ifdef CONFIG_DETECT_HUNG_TASK
	&test_hung_task_attr.attr,
#endif
	NULL
};

int sys_add_rdr_test(void)
{
	while (rdr_kobj == NULL)
		msleep(1000);
	return sysfs_create_files(rdr_kobj, rdr_test_attrs);
}

void sys_del_rdr_test(void)
{
	if (rdr_kobj != NULL)
		sysfs_remove_files(rdr_kobj, rdr_test_attrs);
}
