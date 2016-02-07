/*
 * =====================================================================================
 *
 *       Filename:  chr_devs.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/18/2014 02:53:55 PM
 *       Revision:  none
 *       Compiler:
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
 #ifdef __cplusplus
	#if __cplusplus
		extern "C" {
	#endif
#endif

#ifndef __CHR_DEVS_H__
#define __CHR_DEVS_H__

/*
 * 1 Other Header File Including
 */

#include <linux/debugfs.h>

/*
 * 2 Macro Definition
 */
#define CHR_DEV_KMSG_BFG  "chrKmsgBFG"
#define CHR_DEV_KMSG_WIFI "chrKmsgWifi"
#define CHR_DEV_APP_WIFI  "chrAppWifi"
#define CHR_DEV_APP_BT    "chrAppBt"
#define CHR_DEV_APP_GNSS  "chrAppGnss"

#define __USER_BUFF_LEN_BFG         (1 << 14)
#define __USER_BUFF_LEN_WIFI_KMSG   (1 << 14)
#define __USER_BUFF_LEN_WIFI_APP    (1 << 15)
#define __USER_BUFF_LEN_BT          (1 << 14)
#define __USER_BUFF_LEN_GNSS        (1 << 17)
#define CHR_MSG_MAX_LEN 		    256
#define CHR_LOG_ENABLE  		    1
#define CHR_LOG_DISABLE  		    0
#define CHR_LOG_STAT
#define CHR_LOG_MAX_ALLOC_RTY		100

/*
 * 3 Macro Function Defintion
 */

//#define CHR_DEBUG

#ifdef CHR_DEBUG
#define CHR_DBG(s, args...)            printk(KERN_DEBUG KBUILD_MODNAME ":[chr %s:DEBUG]: " s,__func__, ## args)
#else
#define CHR_DBG(s, args...)
#endif
#define CHR_ALERT(s, args...)          printk(KERN_ALERT KBUILD_MODNAME ":[chr %s:ALERT]: " s,__func__, ## args)
#define CHR_ERR(s, args...)            printk(KERN_ERR KBUILD_MODNAME ":[chr %s:ERR]: " s,__func__, ## args)
#define CHR_SUC(s, args...)            printk(KERN_ERR KBUILD_MODNAME ":[chr %s:SUC]: " s,__func__, ## args)
#define CHR_WARNING(s, args...)        printk(KERN_WARNING KBUILD_MODNAME ":[chr %s:WARNING]: " s,__func__, ## args)
#define CHR_INFO(s, args...)           printk(KERN_INFO KBUILD_MODNAME ":[chr %s:INFO]: " s,__func__, ## args)


#define OPS_OPEN(_name, _index, _nd, _fp)   \
    static int _name##_open(struct inode* _nd, struct file* _fp) \
{                                           \
    return chr_misc_open(_nd, _fp, _index); \
}                                           \

#define OPS_RELEASE(_name, _index, _nd, _fp)     \
    static int _name##_release(struct inode* _nd, struct file* _fp) \
{                                                \
    return chr_misc_release(_nd, _fp, _index);;  \
}

#define OPS_READ(_name, _index, _fp, _buf, _count, _pos) \
    static ssize_t _name##_read(struct file* _fp, char __user * _buf, size_t _count, loff_t* _pos) \
{                                                       \
    return chr_misc_read(_fp, _buf, _count, _index);    \
}

#define OPS_WRITE(_name, _index, _fp, _buf, _count, _pos)   \
    static ssize_t _name##_write(struct file* _fp, const char __user * _buf, size_t _count, loff_t* _pos) \
{                                                           \
    return chr_misc_write(_fp, _buf, _count, _index);       \
}

#define OPS_POLL(_name, _index, _fp, _pts)   \
    static unsigned int _name##_poll(struct file* _fp, struct poll_table_struct* _pts) \
{                                            \
    return chr_misc_poll(_fp, _pts, _index); \
}

#define FILE_OPS(_name) \
    static const struct file_operations _name##_fops = _FILE_OPS(_name)
#define _FILE_OPS(_name) {        \
    .owner   = THIS_MODULE,       \
	.open    = _name##_##open,    \
	.read    = _name##_##read,    \
	.write   = _name##_##write,   \
	.poll    = _name##_##poll,    \
	.release = _name##_##release, \
}

#define MISC_DEV(_name, _str)      \
    static struct miscdevice _name##_dev = _MISC_DEV(_name, _str)
#define _MISC_DEV(_name, _str) {   \
    .minor = MISC_DYNAMIC_MINOR,   \
    .name  = _str,                 \
    .fops  = &_name##_fops,         \
}

#define CHR_WRITE_WORK(_name, _dev_index)      \
    static void _name##_write_work(struct work_struct *work)   \
{                     \
    chr_write_work_do(_dev_index); \
}

/*
 * 4 Enum Type Definition
 */

typedef enum chr_dev_index{
	CHR_INDEX_KMSG_BFG = 0,
	CHR_INDEX_KMSG_WIFI,
	CHR_INDEX_APP_WIFI,
	CHR_INDEX_APP_GNSS,
	CHR_INDEX_APP_BT,
}CHR_DEV_INDEX;

typedef enum chr_LogPriority{
	CHR_LOG_DEBUG = 0,
	CHR_LOG_INFO,
	CHR_LOG_WARN,
	CHR_LOG_ERROR,
}CHR_LOGPRIORITY;

typedef enum chr_LogTag{
	CHR_LOG_TAG_PLAT = 0,
	CHR_LOG_TAG_WIFI,
	CHR_LOG_TAG_GNSS,
	CHR_LOG_TAG_BT,
	CHR_LOG_TAG_FM,
	CHR_LOG_TAG_NFC,
}CHR_LOG_TAG;

enum return_type
{
    CHR_SUCC = 0,
    CHR_EFAIL,
};
/*
 * 5 Type Definition
 */
typedef unsigned char               uint8;
typedef char                        int8;
typedef unsigned short              uint16;
typedef short                       int16;
typedef unsigned int                uint32;
typedef int                         int32;
typedef unsigned long               uint64;
typedef long                        int64;

#ifdef CHR_LOG_STAT
typedef struct {
	uint32 total;
	uint32 user_dropped;
}CHR_LOG_STAT_INFO;
#endif

typedef struct chr_logger_entry {
	uint16			  len;    /*The length of payload*/
	char		      msg[0]; /*The msg that is to be logged*/
}CHR_LOGGER_ENTRY;

typedef struct chr_logger_log {
	char              *buffer;
	atomic_t		  r_off;
	atomic_t		  w_off;
	size_t            size;
	struct semaphore  buf_sema;
}CHR_LOGGER_LOG;

typedef struct {
    CHR_LOGGER_LOG     user;
	CHR_DEV_INDEX	   dev_index;
	struct miscdevice *miscdev;
	wait_queue_head_t  read_waitqueue;
	atomic_t           ref_count;
	atomic_t		  alloc_rty;
#ifdef CHR_LOG_STAT
	CHR_LOG_STAT_INFO  log_stat;
#endif
}CHR_DEV;

#endif

#ifdef __cplusplus
	#if __cplusplus
		}
	#endif
#endif
