/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 *  mntn.c    hisi mntn driver, maintain function s when ccore panic/freeze happens
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
#define MERROR_LEVEL    1
#define MNOTICE_LEVEL   1
#ifndef CONFIG_ARM64

#include <linux/module.h>			/* For module specific items */
#include <linux/moduleparam.h>		/* For new moduleparam's */
#include <linux/types.h>			/* For standard types (like size_t) */
#include <linux/errno.h>			/* For the -ENODEV/... values */
#include <linux/kernel.h>			/* For printk/... */
#include <linux/init.h>				/* For __init/__exit/... */
#include <linux/fs.h>				/* For file operations */
#include <linux/ioport.h>			/* For io-port access */
#include <linux/spinlock.h>			/* For spin_lock/spin_unlock/... */
#include <linux/uaccess.h>			/* For copy_to_user/put_user/... */
#include <linux/workqueue.h>		/* For workqueue... */
#include <linux/notifier.h>			/* For notifier... */
#include <linux/hisi/reset.h>			/* For register notifier*/
#include <linux/hisi/util.h>				/* For mach_call_usermoduleshell*/
#include <linux/delay.h>
#include <linux/stat.h>
#include <linux/mtd/hisi_nve_interface.h>
#include "excDrv.h"
#include "mntn_log.h"

extern unsigned int get_boot_into_recovery_flag(void);
extern unsigned int himntn_gobal_resetlog;
extern unsigned int himntn_modem_resetlog;
extern unsigned int himntn_hifi_resetlog;
extern unsigned int get_boot_into_recovery_flag(void);
extern int modem_panic_regsister_notifier(struct notifier_block *n);
extern int modem_freeze_regsister_notifier(struct notifier_block *n);
extern int modem_reboot_regsister_notifier(struct notifier_block *n);
extern int modem_panic_unregsister_notifier(struct notifier_block *n);
extern int modem_reboot_unregsister_notifier(struct notifier_block *n);
extern int modem_freeze_unregsister_notifier(struct notifier_block *n);
//extern int nve_direct_access(struct nve_info_user * user_info);
static int read_nogui_flag(void)
{
    int ret = 0;
    struct hisi_nve_info_user user_info;
    int nogui_flag = 0;

    user_info.nv_operation = 1;
    user_info.nv_number = 206;
    user_info.valid_size = 1;
    strncpy(user_info.nv_name, "NOGUI", sizeof("NOGUI"));
    ret = hisi_nve_direct_access(&user_info);
    if (ret)
    {
        mlog_e("nve_direct_access read error(%d)", ret);
        return -1;
    }
    nogui_flag = user_info.nv_data[0];
    mlog_n("nogui_flag = %d", nogui_flag);
    if (nogui_flag == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


#define CALLER_DECLARE(name, list, sh_cmd, prop, reset_type)\
static int name ## _notifier_call(struct notifier_block *cb, unsigned long code, void *p)\
{\
    struct kstat state;\
    mm_segment_t fs;\
    int num = 0;\
    if((get_boot_into_recovery_flag()==1)&& (read_nogui_flag()==0)) return 0;\
    fs = get_fs();\
    set_fs(KERNEL_DS);\
    while(1)\
    {\
        if((0 == vfs_stat("/data/hisi_logs/cp_log/coredump", &state)) && (0 == vfs_stat("/system/etc/log/", &state)))\
            break;\
        msleep(1000);\
        num++;\
        if(num > 10) {\
            mlog_e("vfs_stat num=%d, miss \"hisi_logs/cp_log/coredump\" or \"/system/etc/log/\"", num);\
            break;\
        }\
    }\
    set_fs(fs);\
    mach_call_usermodeshell(sh_cmd, 1, reset_type);\
    return 0;\
}\
\
static struct notifier_block name ## _notifier_block = {\
	.notifier_call = name ## _notifier_call,\
};\
\
static int __init name ## _mntn_init(void)\
{\
    unsigned int flag = 0;\
    flag = prop;\
	if (!flag){\
		return 0;\
	}\
	list ## _regsister_notifier(&name ## _notifier_block);\
	return 0;\
}\
\
static void __exit name ## _mntn_exit(void)\
{\
    unsigned int flag = 0;\
    flag = prop;\
	if (!flag){\
		return;\
	}\
	list ## _unregsister_notifier(&name ## _notifier_block);\
	return;\
}\
\
module_init(name ## _mntn_init);\
module_exit(name ## _mntn_exit);


CALLER_DECLARE(modem_panic_sh, modem_panic, "/system/etc/log/modem_exception.sh", himntn_modem_resetlog, "modem panic")
//CALLER_DECLARE(modem_reboot_sh, modem_reboot, "/system/etc/log/modem_exception.sh", himntn_modem_resetlog, "modem reboot by NAS or other")
CALLER_DECLARE(modem_freeze_sh, modem_freeze, "/system/etc/log/modem_exception.sh", himntn_modem_resetlog, "modem freeze")


MODULE_DESCRIPTION("Hisilicon Device Reset Maintain Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuyiping <xuyiping@huawei.com>");
#endif

