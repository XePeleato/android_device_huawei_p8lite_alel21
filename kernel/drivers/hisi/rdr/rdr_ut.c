/*
 * rdr unit test . (RDR: kernel run data recorder.)
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
#include <linux/kallsyms.h>
#include <linux/huawei/rdr.h>
#include "rdr_internal.h"

#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>

/* temp var for ut */
static u32 v1[100];
static char buf2[256];
static u8 v4[100];
static const char filename5[] = "/data/rdr/unit_test.bin";
static const char create_dir7[32] = "/data/rdr/ut_test";
extern u32 rdr_gettick(u32 *usec);
extern void queue_time_in(u32 *qbuf);
extern int rdr_record_register(void);
extern void dump_save_printk(void);
extern int rdr_get_not_dump_stack(void);
extern void rdr_record_resetlog(void);
extern void rdr_save_resetlog(struct rdr_struct_s *bb, char *timedir);
extern void rdr_save_ext(void);
extern int rdr_need_save_before_reboot(u32 mod_id);
extern int rdr_modid_is_known(u32 mod);
extern int rdr_need_reboot(u32 modid);
extern void rdr_save_128m(char *timedir);
extern void rdr_save_sdram96k(char *time_dir);
extern void get_time_stamp(char *timestamp_buf, unsigned int len);
extern void rdr_archive_log(void);
extern int rdr_save_dfx(char *timedir);
extern s32 rdr_early_init(void);
extern void hisi_rdr_ipc_notify_cp(void);
extern void hisi_rdr_nmi_notify_iom3(void);
extern void hisi_rdr_nmi_notify_cp(void);
extern void dump_config_init(void);
extern int wait_for_fs_ready(void);
extern void rdr_looprw_set_state(int in);
extern int rdr_create_dir(const char *path);
extern int rdr_remove_file(char *filename);
int rdr_remove_file(char *filename);
void hisi_system_error(enum rdr_modid_e mod_id,
			u32 arg1, u32 arg2, char *data, u32 length);

static ssize_t rdr_ut_read(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
    int ret = 0;
    printk(KERN_ERR "start   rdr_gettick((long)v1)\n");
    rdr_gettick((long)v1);
    printk(KERN_ERR "start   queue_time_in((long)v1)\n");
    queue_time_in((long)v1);
    printk(KERN_ERR "start   rdr_get_not_dump_stack()\n");
    rdr_get_not_dump_stack();
    printk(KERN_ERR "start   rdr_set_not_dump_stack(1)\n");
    printk(KERN_ERR "start   rdr_record_resetlog()\n");
    rdr_record_resetlog();
    printk(KERN_ERR "start   rdr_save_ext()\n");
    rdr_save_ext();
    printk(KERN_ERR "start   rdr_need_save_before_reboot(0)\n");
    rdr_need_save_before_reboot(0);
    printk(KERN_ERR "start   rdr_need_save_before_reboot(0x82000002)\n");
    rdr_need_save_before_reboot(0x82000002);
    printk(KERN_ERR "start   rdr_need_save_before_reboot(0x84000004)\n");
    rdr_need_save_before_reboot(0x84000004);
    printk(KERN_ERR "start   rdr_need_save_before_reboot(0x86000001)\n");
    rdr_need_save_before_reboot(0x86000001);
    printk(KERN_ERR "start   rdr_need_save_before_reboot(0x20000020)\n");
    rdr_need_save_before_reboot(0x20000020);
    printk(KERN_ERR "start   rdr_need_save_before_reboot(0x12000012)\n");
    rdr_need_save_before_reboot(0x12000012);
    printk(KERN_ERR "start   rdr_modid_is_known(0)\n");
    rdr_modid_is_known(0);
    printk(KERN_ERR "start   rdr_modid_is_known(0x01000001)\n");
    rdr_modid_is_known(0x01000001);
    printk(KERN_ERR "start   rdr_modid_is_known(0x00000001)\n");
    rdr_modid_is_known(0x00000001);
    printk(KERN_ERR "start   rdr_modid_is_known(0x01000010)\n");
    rdr_modid_is_known(0x01000010);
    printk(KERN_ERR "start   rdr_modid_is_known(0x02000008)\n");
    rdr_modid_is_known(0x02000008);
    printk(KERN_ERR "start   rdr_modid_is_known(0x81000004)\n");
    rdr_modid_is_known(0x81000004);
    printk(KERN_ERR "start   rdr_need_reboot(0)\n");
    rdr_need_reboot(0);
    printk(KERN_ERR "start   rdr_need_reboot(0x01000004)\n");
    rdr_need_reboot(0x01000004);
    printk(KERN_ERR "start   rdr_need_reboot(0x83000002)\n");
    rdr_need_reboot(0x83000002);
    printk(KERN_ERR "start   rdr_need_reboot(0x06000001)\n");
    rdr_need_reboot(0x06000001);
    printk(KERN_ERR "start   dump_save()\n");
    dump_save();
    printk(KERN_ERR "start   rdr_save_128m(0)\n");
    rdr_save_128m(0);
    printk(KERN_ERR "start   get_time_stamp((long)buf2, 0)\n");
    get_time_stamp((long)buf2, 0);
    printk(KERN_ERR "start   get_time_stamp((long)buf2, 256)\n");
    get_time_stamp((long)buf2, 256);
    printk(KERN_ERR "start   rdr_archive_log()\n");
    rdr_archive_log();
    printk(KERN_ERR "start   rdr_save_dfx(0)\n");
    rdr_save_dfx(0);
    printk(KERN_ERR "start   rdr_save_dfx((long)filename5)\n");
    rdr_save_dfx((long)filename5);
    printk(KERN_ERR "start   rdr_early_init()\n");
    if ( 0 == rdr_early_init() ) ret = 0;
    printk(KERN_ERR "start   hisi_rdr_ipc_notify_cp()\n");
    hisi_rdr_ipc_notify_cp();
    printk(KERN_ERR "start   hisi_rdr_nmi_notify_iom3()\n");
    hisi_rdr_nmi_notify_iom3();
    printk(KERN_ERR "start   hisi_rdr_nmi_notify_cp()\n");
    hisi_rdr_nmi_notify_cp();
    printk(KERN_ERR "start   dump_config_init()\n");
    dump_config_init();
    printk(KERN_ERR "start   wait_for_fs_ready()\n");
    if ( 0 == wait_for_fs_ready() ) ret = 0;
    printk(KERN_ERR "start   rdr_create_dir(0) --- error\n");
    printk(KERN_ERR "start   rdr_create_dir(\"/data/log1/log2/log3/a/b/c/d/1/2/3/\")\n");
    rdr_create_dir("/data/log1/log2/log3/a/b/c/d/1/2/3/");
    printk(KERN_ERR "start   rdr_create_dir((long)create_dir7)\n");
    rdr_create_dir((long)create_dir7);
    printk(KERN_ERR "start   rdr_remove_file((long)create_dir7)\n");
    rdr_remove_file((long)create_dir7);
    printk(KERN_ERR "start   rdr_remove_file(\"/data/log1/log2/log3/a/b/c/d/1/2/3/\")\n");
    rdr_remove_file("/data/log1");
    printk(KERN_ERR "start   rdr_remove_file(0)\n");
    rdr_remove_file(0);
    printk(KERN_ERR "start   rdr_looprw_set_state(1)\n");
    rdr_looprw_set_state(1);
    printk(KERN_ERR "start   rdr_looprw_set_state(0)\n");
    rdr_looprw_set_state(0);
    printk(KERN_ERR "start   dump_save_printk()\n");
    dump_save_printk();
    printk(KERN_ERR "start   rdr_record_register()\n");
    rdr_record_register();
    printk(KERN_ERR "all call end`````\n");

    return ret;
}
static ssize_t rdr_system_error_read(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
    printk(KERN_ERR "start   hisi_system_error(0x81000000, 0, 0, 0, 0)\n");
    hisi_system_error(0x81000000, 0, 0, 0, 0);
    printk(KERN_ERR "start   hisi_system_error(0x81000001, 0, 0, 0, 0)\n");
    hisi_system_error(0x81000001, 0, 0, 0, 0);
    printk(KERN_ERR "start   hisi_system_error(0x82000002, 0, 0, 0, 0)\n");
    hisi_system_error(0x81000002, 0, 0, 0, 0);
    printk(KERN_ERR "start   hisi_system_error(0x83000003, 0, 0, 0, 0)\n");
    hisi_system_error(0x83000003, 0, 0, 0, 0);
    printk(KERN_ERR "start   hisi_system_error(0x84000004, 0, 0, 0, 0)\n");
    hisi_system_error(0x84000004, 0, 0, 0, 0);
    printk(KERN_ERR "start   hisi_system_error(0x85000005, 0, 0, 0, 0)\n");
    hisi_system_error(0x85000005, 0, 0, 0, 0);
    printk(KERN_ERR "start   hisi_system_error(0x86000006, 0, 0, 0, 0)\n");
    hisi_system_error(0x86000006, 0, 0, 0, 0);
    return 0;
}

static struct kobj_attribute rdr_ut_attr =
			__ATTR(ut, 0444, rdr_ut_read, 0);
static struct kobj_attribute rdr_system_error_attr =
			__ATTR(system_error, 0444, rdr_system_error_read, 0);

static const struct attribute *rdr_test_attrs[] = {
/* del for seattle cts test by xiehongliang 20141028
	&rdr_ut_attr.attr,
	&rdr_system_error_attr.attr, */
	NULL
};

int sys_add_rdr_ut(void)
{
	while (rdr_kobj == NULL)
		msleep(1000);
	return sysfs_create_files(rdr_kobj, rdr_test_attrs);
}

void sys_del_rdr_ut(void)
{
	if (rdr_kobj != NULL)
		sysfs_remove_files(rdr_kobj, rdr_test_attrs);
}
