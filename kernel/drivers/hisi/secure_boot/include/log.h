#ifndef _LOG_H 
#define _LOG_H 


#include <linux/kernel.h>
#include <linux/module.h>                       /* For module specific items */
#include <linux/moduleparam.h>          /* For new moduleparam's */
#include <linux/types.h>                        /* For standard types (like size_t) */
#include <linux/errno.h>                        /* For the -ENODEV/... values */
#include <linux/kernel.h>                       /* For printk/... */
#include <linux/watchdog.h>             /* For the watchdog specific items */
#include <linux/init.h>                         /* For __init/__exit/... */
#include <linux/fs.h>                           /* For file operations */
#include <linux/platform_device.h>      /* For platform_driver framework */
#include <linux/ioport.h>                       /* For io-port access */
#include <linux/spinlock.h>                     /* For spin_lock/spin_unlock/... */
#include <linux/uaccess.h>                      /* For copy_to_user/put_user/... */
#include <linux/io.h>                           /* For inb/outb/... */
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/page.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/genalloc.h>

//#define DEBUG

#ifdef KM_DEBUG
#define LOGE(format, arg...)  printk(KERN_ERR "[KM] [%s]: %d " format "\n" , __FUNCTION__ , __LINE__, ## arg)
#define LOGD(format, arg...)  printk(KERN_ERR "[KM] [%s]: %d " format "\n" , __FUNCTION__ , __LINE__, ## arg)
#define LOGI(format, arg...)  printk(KERN_ERR "[KM] [%s]: %d " format "\n" , __FUNCTION__ , __LINE__, ## arg)
#define PRINT_INFO(format, arg...)  printk(KERN_ERR "[KM] [%s]: %d " format "\n" , __FUNCTION__ , __LINE__, ## arg)

#else
#define LOGE(format, arg...)  printk(KERN_ERR "[KM] [%s]: %d " format "\n" , __FUNCTION__ , __LINE__, ## arg)
#define LOGD(format, arg...)  
#define PRINT_INFO(format, arg...)  
#define LOGI(format, arg...) 
#endif

#endif
