
/*
 * arch/arm/mach-hi6620/util.c
 *
 * balong platform misc utilities function
 *
 * Copyright (C) 2012 Hisilicon, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/hisi/util.h>
//#include <mach/platform.h>
#include <linux/uaccess.h>				/* For copy_to_user*/
#include <linux/delay.h>
#include <linux/mtd/hisi_nve_interface.h>
static char *shcmd = "/system/bin/sh";
static char *shenvp[] = { "HOME=/data", "TERM=vt100", "USER=root",\
        "PATH=/system/xbin:/system/bin", NULL };

int mach_call_usermodeshell(char *script, unsigned int params, ...)
{
	va_list args;
	char **argv;
	int i;
	int ret;
	int num = 0;

	argv = (char **)kmalloc(sizeof(char *)*(params + 3), GFP_KERNEL);
	if (NULL == argv){
		printk(KERN_ERR"%s kmalloc fail\n", __FUNCTION__);
		return -ENOMEM;
	}

	argv[0] = shcmd;
	argv[1] = script;

	va_start(args, params);
	for(i = 0 ; i < params; i++){
		argv[2 + i] = va_arg(args, char *);
	}
	va_end(args);
	argv[2 + i] = NULL;

	do
	{
		ret = call_usermodehelper(shcmd, argv, shenvp, UMH_WAIT_PROC);
		msleep(10);
		num++;
		if(num > 300)
			break;
	}while(ret == -EBUSY);
	if (ret < 0){
		printk(KERN_ERR"%s : call_usermodehelper fail %d\n", __FUNCTION__, ret);
	}

	kfree(argv);

	return ret;
}

/*create balong proc fs directory */
static struct proc_dir_entry *proc_balong_entry;			/*proc/balong*/
static struct proc_dir_entry *proc_balong_debug_entry; 		/*proc/balong/debug*/
static struct proc_dir_entry *proc_balong_stats_entry;		/*proc/balong/stats*/
static struct proc_dir_entry *proc_balong_memory_entry; 	/*proc/balong/memory*/
static struct proc_dir_entry *proc_balong_register_entry;	/*proc/balong/register*/
static struct proc_dir_entry *proc_balong_log_entry;		/*proc/balong/log*/
static struct proc_dir_entry *proc_balong_reboot_entry;     /*proc/balong/reboot*/
#ifdef CONFIG_FACTORY_MODE
static struct proc_dir_entry *proc_balong_ddrtest_entry;     /*proc/balong/factory*/
#endif
static int __init balong_proc_fs_init(void)
{
	proc_balong_entry = proc_mkdir("balong", NULL);
	if (!proc_balong_entry){
		panic("cannot create balong proc entry");
	}

	proc_balong_debug_entry = proc_mkdir("debug", proc_balong_entry);
	if (!proc_balong_debug_entry){
		panic("cannot create balong debug proc entry");
	}

	proc_balong_stats_entry = proc_mkdir("stats", proc_balong_entry);
	if (!proc_balong_stats_entry){
		panic("cannot create balong sys proc entry");
	}

	proc_balong_memory_entry = proc_mkdir("memory", proc_balong_entry);
	if (!proc_balong_memory_entry){
		panic("cannot create balong memory proc entry");
	}

	proc_balong_register_entry = proc_mkdir("register", proc_balong_entry);
	if (!proc_balong_register_entry){
		panic("cannot create balong register proc entry");
	}

	proc_balong_log_entry = proc_mkdir("log", proc_balong_entry);
	if (!proc_balong_log_entry){
		panic("cannot create balong log proc entry");
	}

    proc_balong_reboot_entry = proc_mkdir("reboot", proc_balong_entry);
    if (!proc_balong_reboot_entry){
        panic("cannot create balong reboot proc entry");
    }
#ifdef CONFIG_FACTORY_MODE
    proc_balong_ddrtest_entry = proc_mkdir("ddr_test", proc_balong_entry);
    if (!proc_balong_ddrtest_entry){
        panic("cannot create balong ddr_test proc entry");
    }
#endif
	return 0;
}
core_initcall(balong_proc_fs_init);

static inline struct proc_dir_entry *balong_create_proc_entry(const char *name, mode_t mode,
	struct proc_dir_entry *parent, const struct file_operations *proc_fops, void *data)
{
#ifdef CONFIG_PROC_FS
	return proc_create_data(name, mode, parent, proc_fops, data);
#endif

	return NULL;
}

static inline void balong_remove_proc_entry(const char *name, struct proc_dir_entry *parent)
{
#ifdef CONFIG_PROC_FS
	remove_proc_entry(name, parent);
#endif

	return;
}

#define CREATE_PROC_ENTRY_DECLARE(NAME, PARENT)\
struct proc_dir_entry *balong_create_ ## NAME ## _proc_entry(const char *name, \
	mode_t mode, const struct file_operations *proc_fops, void *data)\
{\
	return balong_create_proc_entry(name, mode, PARENT, proc_fops, data);\
}\
EXPORT_SYMBOL(balong_create_ ## NAME ## _proc_entry);\
\
void balong_remove_ ## NAME ## _proc_entry(const char *name)\
{\
	balong_remove_proc_entry(name, PARENT);\
\
	return;\
}\
EXPORT_SYMBOL(balong_remove_ ## NAME ## _proc_entry);

CREATE_PROC_ENTRY_DECLARE(base, proc_balong_entry)
CREATE_PROC_ENTRY_DECLARE(debug, proc_balong_debug_entry)
CREATE_PROC_ENTRY_DECLARE(stats, proc_balong_stats_entry)
CREATE_PROC_ENTRY_DECLARE(memory, proc_balong_memory_entry)
CREATE_PROC_ENTRY_DECLARE(register, proc_balong_register_entry)
CREATE_PROC_ENTRY_DECLARE(log, proc_balong_log_entry)
CREATE_PROC_ENTRY_DECLARE(reboot, proc_balong_reboot_entry)
#ifdef CONFIG_FACTORY_MODE
CREATE_PROC_ENTRY_DECLARE(ddrtest, proc_balong_ddrtest_entry)
#endif
ssize_t general_proc_file_read(char *base, loff_t size,
	char __user *userbuf, size_t bytes, loff_t *off)
{
	size_t copy;

	if (*off > size){
		printk(KERN_ERR"read offset error \n");
		return 0;
	}

	if (bytes > size - *off){
		copy = size - *off;
	} else {
		copy = bytes;
	}

	if (copy_to_user(userbuf, base + *off, copy)){
		printk(KERN_ERR"%s copy to user error \n", __FUNCTION__);
		return -EFAULT;
	}

	*off+=copy;

	return copy;
}


#ifdef CONFIG_SYSCTL
static struct ctl_path balong_sysctl_path[] = {
	{ .procname = "balong", },
	{ },
};
#endif

struct ctl_table_header *balong_register_sysctl_table(struct ctl_table *table)
{
#ifdef CONFIG_SYSCTL
	return register_sysctl_paths(balong_sysctl_path, table);
#endif

    return NULL;
}


static  int  fastboot_dumpmem=0;
static int __init  read_FST_DUMPMEM_flag(char* cmd)
{

    fastboot_dumpmem=1;
   return 0;
}


int is_fastboot_dumpmem_enable(void )
{

	if(fastboot_dumpmem==1)
		return 1;
	else
		return 0;

}

__setup("fst_dumpmem", read_FST_DUMPMEM_flag);

