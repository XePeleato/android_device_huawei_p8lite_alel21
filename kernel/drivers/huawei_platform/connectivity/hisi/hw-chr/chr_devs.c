/*
 * =====================================================================================
 *
 *       Filename:  chr_devs.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/28/2014 04:43:10 PM
 *       Revision:  none
 *       Compiler:
 *
 *         Author:  swx230420
 *   Organization:
 *
 * =====================================================================================
 */
#ifdef __cplusplus
	#if __cplusplus
		extern "C" {
	#endif
#endif

/*
 * 1 Head Files Includeing
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/workqueue.h>
#include <asm/atomic.h>
#include <stdarg.h>     /*sprintf() etc.*/
#include <linux/slab.h> /* for kmalloc/kfree */
#ifdef CONFIG_HWCONNECTIVITY
#include <linux/huawei/hw_connectivity.h>
#endif
#include "chr_devs.h"

/*
 * 2 Global Variable Definition
 */
static CHR_DEV 	 g_log_devs[];
static int32     g_buffer_size[];
static int32     g_log_enable = CHR_LOG_DISABLE;
struct kobject 	*g_sysfs_chr_log = NULL;
static char 	*g_chr_logprio_str[] = {
	"D",
	"I",
	"W",
	"E",
};
/*
 * 3 Function Definition
 */

#ifdef CHR_LOG_STAT
#define log_info_total_inc(_dev_index) do{ \
		g_log_devs[_dev_index].log_stat.total++;           \
		if (0 == g_log_devs[_dev_index].log_stat.total)    \
		{ \
		    g_log_devs[_dev_index].log_stat.user_dropped = 0;  \
		} \
	}while(0)

#define log_info_user_dropped_inc(_dev_index) do{ \
		g_log_devs[_dev_index].log_stat.user_dropped++;\
	}while(0)

#else
#define log_info_total_inc(_dev_index)
#define log_info_user_dropped_inc(_dev_index)
#endif

#define PTR_BUFINFO(dev_idx)  do{ \
		CHR_DBG("Log_Info.name = %s,r_off = %d,w_off = %d\n",\
		g_log_devs[dev_idx].miscdev->name, \
		atomic_read(&g_log_devs[dev_idx].user.r_off), \
		atomic_read(&g_log_devs[dev_idx].user.w_off)); \
	}while(0)
/*
 * Prototype    : chr_buf_alloc
 * Description  : malloc memory for log buf and initialize buf_info data
 * Input        : void
 * Output       :
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */
static int32 chr_buf_alloc(CHR_DEV_INDEX dev_index)
{
    uint32 buf_len = 0;

    down(&g_log_devs[dev_index].user.buf_sema);
    if(NULL != g_log_devs[dev_index].user.buffer)
	{
        CHR_INFO("Reentrant in alloc user buff, return\n");
        up(&g_log_devs[dev_index].user.buf_sema);
        return CHR_SUCC;
	}
    buf_len = g_log_devs[dev_index].user.size;
    g_log_devs[dev_index].user.buffer = kmalloc(buf_len, GFP_KERNEL);
    if(NULL == g_log_devs[dev_index].user.buffer)
    {
        CHR_ERR("has no enough space for %s user bufer\n", g_log_devs[dev_index].miscdev->name);
        up(&g_log_devs[dev_index].user.buf_sema);
        return -ENOMEM;
    }
    CHR_DBG("--------->>>>finish init user buffer:size=%d\n", g_log_devs[dev_index].user.size);
	PTR_BUFINFO(dev_index);
    CHR_DBG("red_count = %d ", atomic_read(&g_log_devs[dev_index].ref_count));
	up(&g_log_devs[dev_index].user.buf_sema);

	return CHR_SUCC;
}

/*
 * Prototype    : chr_buf_free
 * Description  : free memory for log buf and clear buf_info data
 * Input        : void
 * Output       :
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */
static int32 chr_buf_free(CHR_DEV_INDEX dev_index)
{
    down(&g_log_devs[dev_index].user.buf_sema);
    if (NULL != g_log_devs[dev_index].user.buffer)
    {
        kfree(g_log_devs[dev_index].user.buffer);
        g_log_devs[dev_index].user.buffer = NULL;
        atomic_set(&g_log_devs[dev_index].user.r_off, 0);
        atomic_set(&g_log_devs[dev_index].user.w_off, 0);
		atomic_set(&g_log_devs[dev_index].alloc_rty, 0);
    }
    CHR_DBG("free dev:%s\n", g_log_devs[dev_index].miscdev->name);
	PTR_BUFINFO(dev_index);
	up(&g_log_devs[dev_index].user.buf_sema);

	return CHR_SUCC;
}

/*
 * Prototype    : logger_offset
 * Description  : give a number n, calc new index of write/read
 * Input        :
 * Output       :
 * Return Value : size_t
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/10/15
 *     Author       : z00299054
 *     Modification : Created function
 *
 */
 static size_t logger_offset(CHR_DEV_INDEX dev_index, size_t n)
 {
    return n & (g_log_devs[dev_index].user.size - 1);
 }

 /*
 * Prototype    : is_between
 * Description  : is a < c < b, accounting for wrapping of a, b, and c positions.
 *                that is, if a < b, check for c between a and b
 *                and if a> b, check for c outside (not between) a and b
 *                |------- a xxxxxxx b -------|
 *                              c^
 *                |xxxxxxx b ------- a xxxxxxx|
 *                   c^                    c^
 *                return 1: c is between a and b
 *                       0: c is not between a and b
 * Input        :
 * Output       :
 * Return Value : int
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/10/15
 *     Author       : z00299054
 *     Modification : Created function
 *
 */
 static inline int is_between(size_t a, size_t b, size_t c)
 {
    if (a > b)
    {
        /*is c outside of b through a?*/
        if (c <= b || a <= c)
        {
            return 1;
        }
    }
    else
    {
        /*is c between a and b?*/
        if (a <= c && c <= b)
        {
            return 1;
        }
    }

    return 0;
 }

/*
 * Prototype    : format_prefixstr
 * Description  : format log str with time and prio info
 * Input        :
 * Output       :
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */
static void format_prefixstr( int8 * prefix_str, CHR_LOGPRIORITY prio)
{
	struct tm      p_tm;
	struct timeval tv;

	do_gettimeofday(&tv);
	time_to_tm(tv.tv_sec, 0, &p_tm);
	sprintf(prefix_str, "%04d.%02d.%02d:%02d.%02d.%02d:%03d]%s]", (int)(p_tm.tm_year+1900),\
					(p_tm.tm_mon+1), p_tm.tm_mday, p_tm.tm_hour,\
					p_tm.tm_min, p_tm.tm_sec, (int)(tv.tv_usec/1000), g_chr_logprio_str[prio]);
}

  /*
 * Prototype    : get_entry_header
 * Description  :
 * Input        :
 * Output       :
 * Return Value : struct chr_logger_entry *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/10/15
 *     Author       : z00299054
 *     Modification : Created function
 *
 */
 static struct chr_logger_entry *get_entry_header(CHR_DEV_INDEX dev_index, struct chr_logger_entry *scratch, size_t r_Index)
 {
    size_t len = min(sizeof(struct chr_logger_entry), (g_log_devs[dev_index].user.size - r_Index));
    char *dest = &g_log_devs[dev_index].user.buffer[r_Index];
    if (len != sizeof(struct chr_logger_entry))
    {
        memcpy(((void *)scratch), dest, len);
        memcpy(((void *)scratch) + len, g_log_devs[dev_index].user.buffer, sizeof(struct chr_logger_entry) - len);
        return scratch;
    }

    return (struct chr_logger_entry *)dest;
 }

/*
 * Prototype    : do_write_logger_entry
 * Description  :
 * Input        :
 * Output       :
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/10/15
 *     Author       : z00299054
 *     Modification : Created function
 *
 */
 static void do_write_logger_entry(CHR_DEV_INDEX dev_index, const void *buf, size_t index, size_t count)
 {
    size_t len = 0;

    len = min(count, g_log_devs[dev_index].user.size - index);
    memcpy(&g_log_devs[dev_index].user.buffer[index], buf, len);
    if (count != len)
    {
        memcpy(g_log_devs[dev_index].user.buffer, buf + len, count - len);
    }
 }

 /*
 * Prototype    : do_write_log_from_user
 * Description  :
 * Input        :
 * Output       :
 * Return Value : ssize_t
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/10/15
 *     Author       : z00299054
 *     Modification : Created function
 *
 */
 static ssize_t do_write_log_from_user(CHR_DEV_INDEX dev_index, const void __user *buf, size_t index, size_t count)
 {
    size_t len;

    len = min(count, g_log_devs[dev_index].user.size - index);
    if (len && copy_from_user(&g_log_devs[dev_index].user.buffer[index], buf, len))
    {
        return -CHR_EFAIL;
    }

    if (count != len)
    {
        if(copy_from_user(g_log_devs[dev_index].user.buffer, buf + len, count - len))
        {
            return -CHR_EFAIL;
        }
    }

    return count;
 }

   /*
 * Prototype    : do_read_log_to_user
 * Description  :
 * Input        :
 * Output       :
 * Return Value : ssize_t
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/10/15
 *     Author       : z00299054
 *     Modification : Created function
 *
 */
 static ssize_t do_read_log_to_user(CHR_DEV_INDEX dev_index, void __user *buf, size_t index, size_t count)
 {
    size_t len;

    len = min(count, g_log_devs[dev_index].user.size - index);
    if (copy_to_user(buf, &g_log_devs[dev_index].user.buffer[index], len))
    {
        return -CHR_EFAIL;
    }

    if (count != len)
    {
        if(copy_to_user(buf + len, g_log_devs[dev_index].user.buffer, count - len))
        {
            return -CHR_EFAIL;
        }
    }

    return count;
 }

 /*
 * Prototype    : do_write_log_from_kernel
 * Description  :
 * Input        :
 * Output       :
 * Return Value : ssize_t
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/10/15
 *     Author       : z00299054
 *     Modification : Created function
 *
 */
 static ssize_t do_write_log_from_kernel(CHR_DEV_INDEX dev_index, const void *buf, size_t index, size_t count)
 {
    size_t len;

    len = min(count, g_log_devs[dev_index].user.size - index);
    memcpy(&g_log_devs[dev_index].user.buffer[index], buf, len);
    if (count != len)
    {
        memcpy(g_log_devs[dev_index].user.buffer, buf + len, count - len);
    }

    return count;
 }

/*
 * Prototype    : chr_buf_write
 * Description  : copy user buf to driver's log buf
 * Input        :
 * Output       :
 * Return Value : ssize_t
 * Calls        :
 * Called By    : chr_misc_write
 *
 *   History        :
 *   1.Date         : 2014/10/15
 *     Author       : z00299054
 *     Modification : Created function
 *
 */
static ssize_t chr_buf_write(CHR_DEV_INDEX dev_index, const int8 __user * buf, size_t count)
{
	int ret = -CHR_EFAIL;
	size_t tmp_wIndex = 0;
	size_t tmp_rIndex = 0;
	size_t new_wIndex = 0;
	struct chr_logger_entry header;

	if (count > CHR_MSG_MAX_LEN)
	{
        /* count must not big than CHR_MSG_MAX_LEN, or count=CHR_MSG_MAX_LEN */
        count = CHR_MSG_MAX_LEN;
	}
	header.len = count;
    log_info_total_inc(dev_index);

    tmp_wIndex = atomic_read(&g_log_devs[dev_index].user.w_off);
    tmp_rIndex = atomic_read(&g_log_devs[dev_index].user.r_off);
    new_wIndex = logger_offset(dev_index, (tmp_wIndex + sizeof(struct chr_logger_entry) + count));
    if(is_between(tmp_rIndex, tmp_wIndex, new_wIndex))
    {
	    CHR_DBG("-----------------------(user->kernel)has no enough space in user buf, abandon logs\n");
	    CHR_DBG("******************rIndex=%d, wIndex=%d, new_wIndex=%d\n*********************", tmp_rIndex, tmp_wIndex, new_wIndex);
	    log_info_user_dropped_inc(dev_index);
        wake_up_interruptible(&g_log_devs[dev_index].read_waitqueue);
        return 0;
    }
    atomic_set(&g_log_devs[dev_index].user.w_off, new_wIndex);

	do_write_logger_entry(dev_index, &header, tmp_wIndex, sizeof(struct chr_logger_entry));
	tmp_wIndex = logger_offset(dev_index, tmp_wIndex + sizeof(struct chr_logger_entry));
    ret = do_write_log_from_user(dev_index, buf, tmp_wIndex, count);
    if (unlikely(ret < 0))
    {
        return ret;
    }

    if(new_wIndex > tmp_rIndex)
    {
        if (new_wIndex - tmp_rIndex > g_log_devs[dev_index].user.size / 10)
        {
            wake_up_interruptible(&g_log_devs[dev_index].read_waitqueue);
        }
    }
    else
    {
        if (g_log_devs[dev_index].user.size - tmp_rIndex + new_wIndex > g_log_devs[dev_index].user.size / 10)
        {
            wake_up_interruptible(&g_log_devs[dev_index].read_waitqueue);
        }
    }

    return count;
}

/*
 * Prototype    : chr_buf_read
 * Description  : copy driver's log buf to user buf
 * Input        :
 * Output       :
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       : z00299054
 *     Modification : Created function
 *
 */
static ssize_t chr_buf_read(CHR_DEV_INDEX dev_index, int8 __user * buf, size_t count)
{
    struct chr_logger_entry scrath;
    struct chr_logger_entry *entry;
    int32 ret         = 0;
    size_t len        = 0;
    size_t msg_len    = 0;
    size_t msg_start  = 0;
    size_t tmp_rIndex = 0;
    size_t tmp_wIndex = 0;

    CHR_DBG("-----------------------enter chr_buf_read(),logd read size:count=%d\n",count);
    PTR_BUFINFO(dev_index);
    while (len < count)
    {
        tmp_rIndex = atomic_read(&g_log_devs[dev_index].user.r_off);
        tmp_wIndex = atomic_read(&g_log_devs[dev_index].user.w_off);
        CHR_DBG("--------tmp_rIndex=%d,tmp_wIndex=%d\n", tmp_rIndex, tmp_wIndex);
        /*There is nothing to read, buffer is empty.*/
        if (tmp_rIndex == tmp_wIndex)
        {
            break;
        }

        entry   = get_entry_header(dev_index, &scrath, tmp_rIndex);
        msg_len = entry->len;
        if (msg_len > CHR_MSG_MAX_LEN)
        {
            CHR_ERR("!!!!!!reset buffer,msg_len=%ld,tmp_rIndex=%ld,tmp_wIndex=%ld\n", msg_len, tmp_rIndex, tmp_wIndex);
            atomic_set(&g_log_devs[dev_index].user.r_off, tmp_wIndex);
            break;
        }
        /*There is no enough space in __user buffer to contain next msg*/
        CHR_DBG("--------msg_len=%d,len=%d\n", msg_len, len);
        if (len + msg_len > count)
        {
            break;
        }

        msg_start  = logger_offset(dev_index, tmp_rIndex + sizeof(struct chr_logger_entry));
        ret        = do_read_log_to_user(dev_index, buf + len, msg_start, msg_len);
        /*copy_to _user() failed*/
        if (unlikely(ret != msg_len))
        {
            return ret;
        }

        len += ret;
        atomic_set(&g_log_devs[dev_index].user.r_off, logger_offset(dev_index, msg_start + msg_len));
        CHR_DBG("--------one msg copy to user success:msg_start=%d,len=%d\n", msg_start, ret);
    }

    CHR_DBG("-----------------------leave chr_buf_read(),total read count=%d\n", len);
    PTR_BUFINFO(dev_index);

    return len;
}

/*
 * Prototype    : __chr_printLog
 * Description  : copy print buf to driver's log buf
 * Input        :
 * Output       :
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */
int32 __chr_printLog(CHR_LOGPRIORITY prio, CHR_DEV_INDEX dev_index, const int8 *fmt,...)
{
	int32   ret;
	int32	prefix_len;
	int32	fmt_strlen;
	size_t  tmp_wIndex = 0;
	size_t  tmp_rIndex = 0;
	size_t  new_wIndex = 0;
	va_list	argptr;
    int8	tmpbuf_str[CHR_MSG_MAX_LEN];
    struct chr_logger_entry header;

    if (CHR_LOG_ENABLE != g_log_enable)
	{
	    return 0;
	}
	if((NULL == g_log_devs[dev_index].user.buffer) && \
		(0 == atomic_read(&g_log_devs[dev_index].alloc_rty)))
	{
	    ret = chr_buf_alloc(dev_index);
		if(CHR_SUCC != ret)
		{
			atomic_inc(&g_log_devs[dev_index].alloc_rty);
			log_info_user_dropped_inc(dev_index);
		}
		else
		{
			atomic_inc(&g_log_devs[dev_index].ref_count);
		}
	}
	if (0 < atomic_read(&g_log_devs[dev_index].alloc_rty))
	{
		atomic_inc(&g_log_devs[dev_index].alloc_rty);
		log_info_user_dropped_inc(dev_index);
		if (CHR_LOG_MAX_ALLOC_RTY < atomic_read(&g_log_devs[dev_index].alloc_rty))
		{
			atomic_set(&g_log_devs[dev_index].alloc_rty, 0);
		}
	    return 0;
	}

	format_prefixstr(tmpbuf_str, prio);
	prefix_len = strlen(tmpbuf_str);
	va_start(argptr, fmt);
	fmt_strlen = vsnprintf(tmpbuf_str + prefix_len, CHR_MSG_MAX_LEN - prefix_len - 2, fmt, argptr);
	va_end(argptr);
	strcat(tmpbuf_str,"\n");
	fmt_strlen = strlen(tmpbuf_str);

	if (fmt_strlen > CHR_MSG_MAX_LEN)
	{
	    /* count must not big than CHR_MSG_MAX_LEN, or count=CHR_MSG_MAX_LEN */
        fmt_strlen = CHR_MSG_MAX_LEN;
	}
	header.len = fmt_strlen;
    log_info_total_inc(dev_index);

    tmp_wIndex = atomic_read(&g_log_devs[dev_index].user.w_off);
    tmp_rIndex = atomic_read(&g_log_devs[dev_index].user.r_off);
    new_wIndex = logger_offset(dev_index, (tmp_wIndex + sizeof(struct chr_logger_entry) + fmt_strlen));
    if(is_between(tmp_rIndex, tmp_wIndex, new_wIndex))
    {
        CHR_DBG("-----------------------has no enough space in user buf, abandon logs\n");
        CHR_DBG("******************rIndex=%d, wIndex=%d, new_wIndex=%d \n*********************", tmp_rIndex, tmp_wIndex, new_wIndex);
        log_info_user_dropped_inc(dev_index);
        wake_up_interruptible(&g_log_devs[dev_index].read_waitqueue);
        return 0;
    }
    atomic_set(&g_log_devs[dev_index].user.w_off, new_wIndex);

	do_write_logger_entry(dev_index, &header, tmp_wIndex, sizeof(struct chr_logger_entry));
	tmp_wIndex = logger_offset(dev_index, tmp_wIndex + sizeof(struct chr_logger_entry));
    do_write_log_from_kernel(dev_index, tmpbuf_str, tmp_wIndex, fmt_strlen);

	PTR_BUFINFO(dev_index);
    if(new_wIndex > tmp_rIndex)
    {
        if (new_wIndex - tmp_rIndex > g_log_devs[dev_index].user.size / 10)
        {
            wake_up_interruptible(&g_log_devs[dev_index].read_waitqueue);
        }
    }
    else
    {
        if (g_log_devs[dev_index].user.size - tmp_rIndex + new_wIndex > g_log_devs[dev_index].user.size / 10)
        {
            wake_up_interruptible(&g_log_devs[dev_index].read_waitqueue);
        }
    }

	return fmt_strlen;
}
EXPORT_SYMBOL(__chr_printLog);

/*
 * Prototype    : chr_misc_read
 * Description  : miscdevice read function
 * Input        :
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */
static ssize_t chr_misc_read(struct file* fp, int8 __user * buf, size_t count, CHR_DEV_INDEX dev_index)
{
	int32 ret;
    if (CHR_LOG_ENABLE != g_log_enable)
	{
	    return 0;
	}
    CHR_DBG("g_log_devs[%d].miscdev->name=%s\n", dev_index, g_log_devs[dev_index].miscdev->name);
	ret = chr_buf_read(dev_index, buf, count);
	return ret;

}

/*
 * Prototype    : chr_misc_write
 * Description  : miscdevice write function
 * Input        :
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */
static ssize_t chr_misc_write(struct file *fp,const int8 __user *buf, size_t count, CHR_DEV_INDEX dev_index)
{
	int32 ret;

	if (CHR_LOG_ENABLE != g_log_enable)
	{
	    return 0;
	}
	if((NULL == g_log_devs[dev_index].user.buffer) && \
		(0 == atomic_read(&g_log_devs[dev_index].alloc_rty)))
	{
	    ret = chr_buf_alloc(dev_index);
		if(CHR_SUCC != ret)
		{
			atomic_inc(&g_log_devs[dev_index].alloc_rty);
			log_info_user_dropped_inc(dev_index);
		}
	}
	if (0 < atomic_read(&g_log_devs[dev_index].alloc_rty))
	{
		atomic_inc(&g_log_devs[dev_index].alloc_rty);
		log_info_user_dropped_inc(dev_index);
		if (CHR_LOG_MAX_ALLOC_RTY < atomic_read(&g_log_devs[dev_index].alloc_rty))
		{
			atomic_set(&g_log_devs[dev_index].alloc_rty, 0);
		}
	    return 0;
	}
	CHR_DBG("write g_log_devs[%d].miscdev->name=%s\n", dev_index, g_log_devs[dev_index].miscdev->name);
	ret = chr_buf_write(dev_index, buf, count);
	return ret;
}

/*
 * Prototype    : chr_misc_open
 * Description  : miscdevice open function
 * Input        :
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */
static int32 chr_misc_open(struct inode *nd, struct file* fp, CHR_DEV_INDEX dev_index)
{
    int32 ret = -CHR_EFAIL;

    if (CHR_LOG_ENABLE != g_log_enable)
	{
	    return 0;
	}
	CHR_DBG("g_log_devs[%d].miscdev->name=%s\n", dev_index, g_log_devs[dev_index].miscdev->name);
	atomic_inc(&g_log_devs[dev_index].ref_count);
	ret = CHR_SUCC;
	return ret;
}

/*
 * Prototype    : chr_misc_release
 * Description  : miscdevice release function
 * Input        :
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */
static int32 chr_misc_release(struct inode *nd, struct file* fp, CHR_DEV_INDEX dev_index)
{
    int32 ret = -CHR_EFAIL;

    if (CHR_LOG_ENABLE != g_log_enable)
	{
	    return 0;
	}
	CHR_DBG("g_log_devs[%d].miscdev->name=%s\n", dev_index, g_log_devs[dev_index].miscdev->name);
	if(atomic_dec_and_test(&g_log_devs[dev_index].ref_count))
	{
	    ret = chr_buf_free(dev_index);
	}

	return ret;
}

/*
 * Prototype    : chr_misc_poll
 * Description  : miscdevice poll function
 * Input        :
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */
static uint32 chr_misc_poll(struct file *fp, struct poll_table_struct * pts, CHR_DEV_INDEX dev_index)
{
	uint32 mask = 0;
	size_t tmp_rIndex = 0;
	size_t tmp_wIndex = 0;

    if (CHR_LOG_ENABLE != g_log_enable)
	{
	    return 0;
	}

	PTR_BUFINFO(dev_index);
	tmp_rIndex = atomic_read(&g_log_devs[dev_index].user.r_off);
	tmp_wIndex = atomic_read(&g_log_devs[dev_index].user.w_off);
	CHR_DBG("chr_misc_poll----->r_off=%d,w_off=%d\n", tmp_rIndex, tmp_wIndex);
	poll_wait(fp,&g_log_devs[dev_index].read_waitqueue, pts);
	if(tmp_rIndex != tmp_wIndex)
	{
		mask |= (POLLIN | POLLRDNORM);
	}

	return mask;
}

/* --------------- /dev/chrKmsgWifi ops and dev define  ---------------*/
OPS_OPEN(chrKmsgWifi,   CHR_INDEX_KMSG_WIFI, nd, fp);
OPS_RELEASE(chrKmsgWifi,CHR_INDEX_KMSG_WIFI, nd, fp);
OPS_READ(chrKmsgWifi,   CHR_INDEX_KMSG_WIFI, fp, buf, count, pos);
OPS_WRITE(chrKmsgWifi,  CHR_INDEX_KMSG_WIFI, fp, buf, count, pos);
OPS_POLL(chrKmsgWifi,   CHR_INDEX_KMSG_WIFI, fp, pts);
FILE_OPS(chrKmsgWifi);
MISC_DEV(chrKmsgWifi,   CHR_DEV_KMSG_WIFI);

/* --------------- /dev/chrKmsgBFG ops and dev define  ---------------*/
OPS_OPEN(chrKmsgBFG,    CHR_INDEX_KMSG_BFG, nd, fp);
OPS_RELEASE(chrKmsgBFG, CHR_INDEX_KMSG_BFG, nd, fp);
OPS_READ(chrKmsgBFG,    CHR_INDEX_KMSG_BFG, fp, buf, count, pos);
OPS_WRITE(chrKmsgBFG,   CHR_INDEX_KMSG_BFG, fp, buf, count, pos);
OPS_POLL(chrKmsgBFG,    CHR_INDEX_KMSG_BFG, fp, pts);
FILE_OPS(chrKmsgBFG);
MISC_DEV(chrKmsgBFG,    CHR_DEV_KMSG_BFG);

/* --------------- /dev/chrAppWifi ops and dev define  ---------------*/
OPS_OPEN(chrAppWifi,    CHR_INDEX_APP_WIFI, nd, fp);
OPS_RELEASE(chrAppWifi, CHR_INDEX_APP_WIFI, nd, fp);
OPS_READ(chrAppWifi,    CHR_INDEX_APP_WIFI, fp, buf, count, pos);
OPS_WRITE(chrAppWifi,   CHR_INDEX_APP_WIFI, fp, buf, count, pos);
OPS_POLL(chrAppWifi,    CHR_INDEX_APP_WIFI, fp, pts);
FILE_OPS(chrAppWifi);
MISC_DEV(chrAppWifi,    CHR_DEV_APP_WIFI);

/* --------------- /dev/chrAppBt ops and dev define ---------------*/
OPS_OPEN(chrAppBt,      CHR_INDEX_APP_BT, nd, fp);
OPS_RELEASE(chrAppBt,   CHR_INDEX_APP_BT, nd, fp);
OPS_READ(chrAppBt,      CHR_INDEX_APP_BT, fp, buf, count, pos);
OPS_WRITE(chrAppBt,     CHR_INDEX_APP_BT, fp, buf, count, pos);
OPS_POLL(chrAppBt,      CHR_INDEX_APP_BT, fp, pts);
FILE_OPS(chrAppBt);
MISC_DEV(chrAppBt,      CHR_DEV_APP_BT);

/* --------------- /dev/chrAppGnss ops and dev define ---------------*/
OPS_OPEN(chrAppGnss,    CHR_INDEX_APP_GNSS, nd, fp);
OPS_RELEASE(chrAppGnss, CHR_INDEX_APP_GNSS, nd, fp);
OPS_READ(chrAppGnss,    CHR_INDEX_APP_GNSS, fp, buf, count, pos);
OPS_WRITE(chrAppGnss,   CHR_INDEX_APP_GNSS, fp, buf, count, pos);
OPS_POLL(chrAppGnss,    CHR_INDEX_APP_GNSS, fp, pts);
FILE_OPS(chrAppGnss);
MISC_DEV(chrAppGnss,    CHR_DEV_APP_GNSS);

static CHR_DEV g_log_devs[] = {
	{ .dev_index = CHR_INDEX_KMSG_BFG,  .miscdev = &chrKmsgBFG_dev,},
	{ .dev_index = CHR_INDEX_KMSG_WIFI, .miscdev = &chrKmsgWifi_dev,},
	{ .dev_index = CHR_INDEX_APP_WIFI,  .miscdev = &chrAppWifi_dev,},
	{ .dev_index = CHR_INDEX_APP_GNSS,  .miscdev = &chrAppGnss_dev,},
	{ .dev_index = CHR_INDEX_APP_BT,    .miscdev = &chrAppBt_dev,},
};

static int32 g_buffer_size[] = {
    __USER_BUFF_LEN_BFG,
    __USER_BUFF_LEN_WIFI_KMSG,
    __USER_BUFF_LEN_WIFI_APP,
    __USER_BUFF_LEN_GNSS,
    __USER_BUFF_LEN_BT,
};

#ifdef CHR_LOG_STAT
static ssize_t show_chr_log_stat_info(struct device *dev, \
        struct device_attribute *attr, int8 *buf)
{
    int32 i 		= 0;
    int32 devs 		= 0;
    int32 len 		= 0;
    int32 str_cnt 	= 0;
    devs = sizeof(g_log_devs) / sizeof(CHR_DEV);
    for (i = 0; i < devs; i++)
    {
        len = sprintf(buf + str_cnt, "name:%s totol:%d user_drop:%d\n", \
        g_log_devs[i].miscdev->name, g_log_devs[i].log_stat.total, g_log_devs[i].log_stat.user_dropped);
        str_cnt += len;
    }
    return str_cnt;
}

static struct kobj_attribute chr_log_stat =
    __ATTR(chr_log_stat, 0644, (void *)show_chr_log_stat_info, (void *)0);

#endif

/* show curr chr log switch enable or disable */
static ssize_t show_chr_log_switch(struct device *dev, \
        struct device_attribute *attr, int8 *buf)
{
    CHR_INFO("show g_log_enable=%d\n", g_log_enable);
    return sprintf(buf, "1:enable other:disable value=%d\n", g_log_enable);
}

/*  store curr chr log switch enable or disable */
static ssize_t store_chr_log_switch(struct device *dev, \
			struct kobj_attribute *attr, const char *buf, size_t count)
{
    int32 log_switch = 0;
    sscanf(buf, "%d", &log_switch);
    if (CHR_LOG_ENABLE == log_switch)
    {
        g_log_enable = log_switch;
    }
    else if (CHR_LOG_DISABLE == log_switch)
    {
        g_log_enable = log_switch;
    }
    CHR_INFO("store g_log_enable=%d\n", g_log_enable);
    return count;
}

static struct kobj_attribute chr_log_switch =
    __ATTR(chr_log_switch, 0644, (void *)show_chr_log_switch, (void *)store_chr_log_switch);

static struct attribute *chr_attrs[] = {
        &chr_log_switch.attr,
#ifdef CHR_LOG_STAT
		&chr_log_stat.attr,
#endif
        NULL,
};

static struct attribute_group chr_attr_grp = {
    .attrs = chr_attrs,
};
/*
 * Prototype    : miscdev_init
 * Description  : initialize struct data and register miscdev
 * Input        :
 * Output       : None
 * Return Value :
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2014/9/28
 *     Author       :
 *     Modification : Created function
 *
 */

static int32 chr_miscdev_init(CHR_DEV_INDEX dev_index)
{
	int32 ret = 0;

    sema_init(&g_log_devs[dev_index].user.buf_sema, 1);
	init_waitqueue_head(&g_log_devs[dev_index].read_waitqueue);
    g_log_devs[dev_index].user.size = g_buffer_size[dev_index];
    atomic_set(&g_log_devs[dev_index].user.r_off, 0);
    atomic_set(&g_log_devs[dev_index].user.w_off, 0);
    atomic_set(&g_log_devs[dev_index].alloc_rty, 0);
    atomic_set(&g_log_devs[dev_index].ref_count, 0);

#if 0
    if (CHR_INDEX_KMSG_BFG == dev_index || CHR_INDEX_KMSG_WIFI == dev_index)
    {
        ret = chr_buf_alloc(dev_index);
        if (CHR_SUCC != ret)
        {
            CHR_ERR("alloc buf memory err\n");
            return ret;
        }
    }
#endif
	#ifdef CHR_LOG_STAT
	g_log_devs[dev_index].log_stat.total        = 0;
	g_log_devs[dev_index].log_stat.user_dropped = 0;
	#endif

	ret = misc_register(g_log_devs[dev_index].miscdev);
	return ret;
}


static int32 chr_miscdevs_register(void)
{
	int32 i, ret = -CHR_EFAIL;
	int32 devs;
	int32 status;

    g_sysfs_chr_log = kobject_create_and_add("chr_log", NULL);
    if (NULL == g_sysfs_chr_log)
    {
        CHR_ERR("Failed to creat g_sysfs_chr_log!!!\n ");
		return -ENOMEM;
    }

    status = sysfs_create_group(g_sysfs_chr_log, &chr_attr_grp);
    if (status)
    {
		CHR_ERR("failed to create sysfs entries\n");
		kobject_put(g_sysfs_chr_log);
		g_sysfs_chr_log = NULL;
		g_log_enable = CHR_LOG_DISABLE;
        return -EFAULT;
    }

    devs = sizeof(g_log_devs) / sizeof(CHR_DEV);
	for (i = 0; i < devs; i++)
	{
		ret = chr_miscdev_init(i);
		if (0 != ret)
		{
			for (; i >= 0; i--)
			{
                if(NULL != g_log_devs[i].user.buffer)
                {
                    kfree(g_log_devs[i].user.buffer);
                }
				misc_deregister(g_log_devs[i].miscdev);
			}
			CHR_ERR("chr devs register error!!!\n");
			kobject_put(g_sysfs_chr_log);
		    g_sysfs_chr_log = NULL;
		    g_log_enable = CHR_LOG_DISABLE;
			return ret;
		}
		CHR_INFO("chr dev %s register ok\n", g_log_devs[i].miscdev->name);
	}
	g_log_enable = CHR_LOG_ENABLE;
	return ret;
}

static void chr_miscdevs_unregister(void)
{
	int32 i;
	int32 devs = sizeof(g_log_devs) / sizeof(CHR_DEV);

	if (CHR_LOG_ENABLE != g_log_enable)
	{
	    CHR_INFO("chr log is diabled\n");
	    return;
	}
	for(i = 0; i < devs; i++)
	{
		if(NULL != g_log_devs[i].user.buffer)
		{
			kfree(g_log_devs[i].user.buffer);
		}
		misc_deregister(g_log_devs[i].miscdev);
	}
	kobject_put(g_sysfs_chr_log);
    g_sysfs_chr_log = NULL;
    g_log_enable = CHR_LOG_DISABLE;

	CHR_INFO("chr dev usregister \n");
}

int32 __init chr_miscdevs_init(void)
{
	int32 ret = -CHR_EFAIL;
#ifdef CONFIG_HWCONNECTIVITY
    if (!isMyConnectivityChip(CHIP_TYPE_HI110X)) {
        CHR_INFO("cfg chr log chip type is not match, skip driver init");
		g_log_enable = CHR_LOG_DISABLE;
        return -EINVAL;
    } else {
        CHR_INFO("cfg chr log is matched with hi110x, continue");
    }
#endif
	ret = chr_miscdevs_register();
	return ret;
}
void __exit chr_miscdevs_exit(void)
{

#ifdef CONFIG_HWCONNECTIVITY
    if (!isMyConnectivityChip(CHIP_TYPE_HI110X)) {
        CHR_INFO("cfg chr log chip type is not match, skip driver init");
        return;
    } else {
        CHR_INFO("cfg chr log is matched with hi110x, continue");
    }
#endif
	chr_miscdevs_unregister();
}
module_init(chr_miscdevs_init);
module_exit(chr_miscdevs_exit);
MODULE_AUTHOR("Hisilicon platform Driver Group");
MODULE_DESCRIPTION("hi1101 chr log driver");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
	#if __cplusplus
		}
	#endif
#endif

