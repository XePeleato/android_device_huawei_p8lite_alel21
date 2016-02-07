/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 *  mntn.c    hisi mntn driver, maintain function s when panic/freeze happens
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
//extern int nve_direct_access(struct nve_info_user * user_info);
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
void mntn_print_to_ramconsole(const char *fmt, ...)
{
    char    pbuf[MNTN_PRINTBUF_LEN_MAX] = {0};
    va_list ap;

    if (NULL == fmt)
    {
        return;
    }
    memset(pbuf, 0, MNTN_PRINTBUF_LEN_MAX);
    va_start(ap, fmt);
    vsnprintf(pbuf, MNTN_PRINTBUF_LEN_MAX, fmt, ap);
    va_end(ap);
    ramoops_console_write_buf(pbuf, strlen(pbuf));
}

