/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 *  fastbootlog.c    hisi fastbootlog module
 *                   record fastbootlog into filesystem when booting kernel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/stat.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/hisi/util.h>				/* For mach_call_usermoduleshell*/
#include <linux/uaccess.h>				/* For copy_to_user*/
#include <linux/vmalloc.h>				/* For vmalloc*/
#if defined CONFIG_ARCH_HI6XXX
#include "global_ddr_map.h"
#elif defined CONFIG_ARCH_HI3XXX
#define FASTBOOT_DUMP_LOG_ADDR	0x3FC00000
#define FASTBOOT_DUMP_LOG_SIZE	0x10000
#endif

#define FASTBOOT_MAGIC_1				0x5a5a5a5a
#define FASTBOOT_MAGIC_2				0x43474244

#ifndef CONFIG_ARCH_HI3XXX
extern unsigned int himntn_gobal_resetlog;
#endif

static void *fastboot_log_buff = NULL;
static size_t fastboot_log_size = 0;

struct fastbootlog_head{
    unsigned int magic;
    unsigned int log_offset;
};

/*read dump file content*/
static ssize_t fastbootlog_dump_file_read(struct file *file, char __user *userbuf, size_t bytes, loff_t *off)
{
	ssize_t copy;

	if (*off > fastboot_log_size){
		printk(KERN_ERR"read offset error 1\n");
		return 0;
	}

	if (*off == fastboot_log_size){
		/*end of file*/
		return 0;
	}

	copy = (ssize_t)min(bytes, (size_t)(fastboot_log_size - *off));

	if (copy_to_user(userbuf, fastboot_log_buff + *off, copy)){
		printk(KERN_ERR"%s copy to user error \n", __FUNCTION__);
		copy = -EFAULT;
		goto copy_err;
	}

	*off += copy;

copy_err:
	return copy;
}

static const struct file_operations fastbootlog_dump_file_fops = {
	.read = fastbootlog_dump_file_read,
};

static void bootloader_logger_dump(char *start, unsigned int size)
{
    unsigned int i;
    char *p = start;

    if (!p)
        return;

    printk(KERN_INFO "*****************************"
            "bootloader_log begin"
            "*****************************\n");
    for (i=0; i<size; i++) {
        if (start[i] == '\0')
            start[i] = ' ';
        if (start[i] == '\n') {
            start[i] = '\0';
            printk(KERN_INFO "bootloader_log: %s\n", p);
            start[i] = '\n';
            p = &start[i+1];
        }
    }
    printk(KERN_INFO "******************************"
            "bootloader_log end"
            "******************************\n");
}

/*****************************************************************************
Description : fastbootlog init
check emmc leaves log to record
if there is log, launch work queue, and create /proc/balong/fastbootlog
History
1.Date: 2012/9/13
Author : x00138766
Modification : Created function
 *****************************************************************************/
static int __init fastbootlog_dump_init(void)
{
	void *fastboot_buff;
	struct fastbootlog_head *head;
	char *start;
	unsigned int size;

#ifndef CONFIG_ARCH_HI3XXX
	if (!himntn_gobal_resetlog) {
		return 0;
	}
#endif

	fastboot_buff = ioremap(FASTBOOT_DUMP_LOG_ADDR, FASTBOOT_DUMP_LOG_SIZE);
	if (!fastboot_buff) {
		printk(KERN_ERR"%s fail to ioremap %#x bytes \n", __func__, FASTBOOT_DUMP_LOG_SIZE);
		return -1;
	}
	
	head = (struct fastbootlog_head *)fastboot_buff;
	start  = (char *)head + sizeof(struct fastbootlog_head);
	size = head->log_offset - sizeof(struct fastbootlog_head);
	if (head->magic==FASTBOOT_MAGIC_1 || head->magic==FASTBOOT_MAGIC_2)
		bootloader_logger_dump(start, size);
	
	fastboot_log_buff = vmalloc(FASTBOOT_DUMP_LOG_SIZE);
	if(!fastboot_log_buff){
		printk(KERN_ERR"%s fail to vmalloc %d bytes \n", __FUNCTION__, FASTBOOT_DUMP_LOG_SIZE);
		return -1;
	}

	memcpy(fastboot_log_buff, fastboot_buff, FASTBOOT_DUMP_LOG_SIZE);
	fastboot_log_size = FASTBOOT_DUMP_LOG_SIZE;
	iounmap(fastboot_buff);
	
	balong_create_log_proc_entry("fastboot", S_IRUGO, &fastbootlog_dump_file_fops, NULL);

	return 0;
}
module_init(fastbootlog_dump_init);

/*****************************************************************************
Description : fastbootlog exit
destroy the workqueue
History
1.Date: 2012/9/13
Author : x00138766
Modification : Created function
 *****************************************************************************/
static void __exit fastbootlog_dump_exit(void)
{
	balong_remove_log_proc_entry("fastboot");

	if (fastboot_log_buff){
		vfree(fastboot_log_buff);
		fastboot_log_buff = NULL;
	}
	fastboot_log_size = 0;

	return;
}
module_exit(fastbootlog_dump_exit);


