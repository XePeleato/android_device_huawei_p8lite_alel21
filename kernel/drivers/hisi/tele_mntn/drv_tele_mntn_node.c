#include <linux/types.h>
#include <linux/errno.h>
#include <linux/time.h>

#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/syslog.h>
#if 0  /*sys_class_telemntnsw_dev_onoff*/
#include <linux/module.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#endif
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/semaphore.h>
#include <linux/completion.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/debugfs.h>
#include <linux/of_platform.h>
/*#include <linux/delay.h>*/
#include "drv_tele_mntn_platform.h"


#include <linux/jiffies.h>
#include <linux/wait.h>
/*#include "mdrv.h" */
#include <linux/hisi/ipc_msg.h>

#define TELE_MNTN_NODE_READ_WAIT_THRESHOLD                    (256)
#define TELE_MNTN_NODE_READ_WAIT_TIMEOUT                      (1000)  /*ms*/
#define TELE_MNTN_NODE_READ_WAIT_DELAY_TIMES_THRESHOLD        (2)
#define TELE_MNTN_NODE_READ_WAIT_CONDITION_TRUE               (0x11)  /*wait*/
#define TELE_MNTN_NODE_READ_WAIT_CONDITION_FALSE              (0x10)  /*no wait*/

DECLARE_WAIT_QUEUE_HEAD(node_read_wait);
static int node_read_part_need_wait_delay_times = 0;
static int node_read_wait_timeout = 0;
static int node_read_wait_condition = TELE_MNTN_NODE_READ_WAIT_CONDITION_TRUE;

static struct semaphore rw_sem; /* get the access of modify the rp & wp, nobody can't modify them (means access to the buf) without rw_lock */

static int tele_mntn_node_open(struct inode * inode, struct file * file)
{
    /*printk(KERN_ERR"tele_mntn_node_open\n");*/
    tele_mntn_func_sw_bit_set(TELE_MNTN_NVME_LOGCAT);
    tele_mntn_func_sw_bit_set(TELE_MNTN_NVME_WAKEUP_ACPU);
	return 0;
}

static int tele_mntn_node_release(struct inode * inode, struct file * file)
{
    /*printk(KERN_ERR"tele_mntn_node_release\n");*/
    tele_mntn_func_sw_bit_clr(TELE_MNTN_NVME_WAKEUP_ACPU);
    tele_mntn_func_sw_bit_clr(TELE_MNTN_NVME_LOGCAT);
	return 0;
}
#if 0
/* we just read data for user, set the read pointer */
static ssize_t tele_mntn_node_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos)
{
    unsigned int size_read = 0;
    unsigned int need_read = count;
    int rv = 0;
    unsigned int to_bottom = 0;

    printk(KERN_ERR"need_read: %d;\n", need_read);

    if (count == 0) {
        printk(KERN_ERR"count == 0\n");
        return 0;
    }
        /* now count > 0 */
    down(&rw_sem);
    printk(KERN_ERR" get the rw_sem\n");
    if(tele_mntn_get_head(&log_queue))
    {
        goto copy_error;
    }
    printk(KERN_ERR" base=%x, len=%d, front=%x, rear=%x\n",log_queue.base, log_queue.length, log_queue.front,log_queue.rear);
    if(log_queue.front > log_queue.rear)
    {
        size_read = ((log_queue.front-log_queue.rear)<=need_read?(log_queue.front-log_queue.rear):need_read);
        printk(KERN_ERR" 111111 size_read=%d\n",size_read);
        if ((rv = copy_to_user(buf, log_queue.rear, size_read)) != 0) {
            goto copy_error;
        }
        log_queue.rear += size_read;
        tele_mntn_set_head(&log_queue);


    }
    else if(log_queue.front < log_queue.rear)
    {
        to_bottom = log_queue.base + log_queue.length - log_queue.rear;
        printk(KERN_ERR" 2222 to_bottom=%d\n",to_bottom);

        if(to_bottom >= need_read)
        {
            if ((rv = copy_to_user(buf, log_queue.rear, need_read)) != 0) {
                goto copy_error;
            }
            log_queue.rear += need_read;
            tele_mntn_set_head(&log_queue);

            size_read = need_read;

        }
        else
        {
            if ((rv = copy_to_user(buf, log_queue.rear, to_bottom)) != 0) {
                goto copy_error;
            }
            //size_read = to_bottom;
            buf += to_bottom;
            need_read -= to_bottom;

            size_read = ((log_queue.front-log_queue.base)<=need_read?(log_queue.front-log_queue.base):need_read);
            printk(KERN_ERR" 3333 size_read=%d\n",size_read);
            if ((size_read)&&((rv = copy_to_user(buf, log_queue.base, size_read)) != 0)) {
                goto copy_error;
            }

            log_queue.rear = log_queue.base + size_read;
            tele_mntn_set_head(&log_queue);
            size_read += to_bottom;
        }

    }
    else
    {
    }

    up(&rw_sem);
    printk(KERN_ERR"%d read, return", size_read);
	return size_read;

copy_error:
    up(&rw_sem);
    printk(KERN_ERR"copy_to_user return %d, some error happen!!!", rv);
    return EIO;
}
#endif


static ssize_t tele_mntn_node_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos)
{
    struct tele_mntn_queue log_queue = {0};
    int ret = 0;
    unsigned int size_read = 0;
    /*8 bytes align*/
    unsigned int need_read = TELE_MNTN_SHORTCUT_ALIGN(count, TELE_MNTN_ALIGN_SIZE);

    /*printk(KERN_INFO"need_read: %d;\n", need_read);*/

    if (need_read == 0) {
        return 0;
    }
        /* now count > 0 */
    down(&rw_sem);

	if(node_read_part_need_wait_delay_times >= TELE_MNTN_NODE_READ_WAIT_DELAY_TIMES_THRESHOLD)
	{
		node_read_part_need_wait_delay_times = 0;
		/*msleep(2000);*/

    	ret = wait_event_interruptible_timeout(node_read_wait,
    	    TELE_MNTN_NODE_READ_WAIT_CONDITION_FALSE == node_read_wait_condition,
			node_read_wait_timeout);
		/*printk(KERN_ERR"tele_mntn_node_read, wait_timeout, ret=%d, node_read_wait_condition=%d \n", ret, node_read_wait_condition);*/
		node_read_wait_condition = TELE_MNTN_NODE_READ_WAIT_CONDITION_TRUE;
	}

    if((ret = tele_mntn_get_head(&log_queue)) != 0)
    {
        goto copy_error;
    }

/*
    if(log_queue.front == log_queue.rear)
    {
        up(&rw_sem);
        return 0;
    }
*/
    size_read = (log_queue.front >= log_queue.rear)?(log_queue.front-log_queue.rear):(log_queue.base + log_queue.length - log_queue.rear);
    size_read = size_read <= need_read ? size_read:need_read;

    /*printk(KERN_ERR" base=%x, len=%d, front=%x, rear=%x, size_read=%d, need_read=%d\n",log_queue.base, log_queue.length, log_queue.front,log_queue.rear, size_read, need_read);*/


    if(size_read)
    {
        if ((ret = copy_to_user(buf, log_queue.rear, size_read)) != 0) {
            goto copy_error;
        }

        log_queue.rear += size_read;

        if(log_queue.rear >= log_queue.base + log_queue.length)
        {
            log_queue.rear -= log_queue.length;
        }

        tele_mntn_set_head(&log_queue);
    }

    if(size_read <= TELE_MNTN_NODE_READ_WAIT_THRESHOLD)
    {
		node_read_part_need_wait_delay_times ++;
    }
    else
    {
        node_read_part_need_wait_delay_times = 0;
    }

    up(&rw_sem);
    /*printk(KERN_INFO"%d read, return", size_read);*/
	return size_read;

copy_error:
    up(&rw_sem);
    printk(KERN_ERR"return %d, some error happen!!!\n", ret);
    return EIO;
}

static const struct file_operations proc_tele_mntn_node_operations = {
	.read		= tele_mntn_node_read,
	.open		= tele_mntn_node_open,
	.release	= tele_mntn_node_release,
};
#if 0  /*sys_class_telemntnsw_dev_onoff*/
static ssize_t tele_mntn_sw_show(struct device *_dev,
              struct device_attribute *attr, char *buf)
{
    unsigned long long sw_value = 0;
    unsigned int tmp_low = 0;
    unsigned int tmp_high = 0;

    sw_value = tele_mntn_func_sw_get();

    tmp_low = (unsigned int)(sw_value & (unsigned int)0xFFFFFFFF);

    tmp_high = (unsigned int)((sw_value >> 32) & (unsigned int)0xFFFFFFFF);

    return sprintf(buf, "0x%08x%08x\n", tmp_high, tmp_low);
}


static ssize_t tele_mntn_sw_on_store(struct device *_dev,
                 struct device_attribute *attr,
                 const char *buf, size_t count)
{
    unsigned int sw_bit = 0;
    sscanf(buf, "%u", &sw_bit);
    if(sw_bit < 64)
    {
        tele_mntn_func_sw_bit_set(sw_bit);
    }
    return count;
}

static ssize_t tele_mntn_sw_off_store(struct device *_dev,
                 struct device_attribute *attr,
                 const char *buf, size_t count)
{
    unsigned int sw_bit = 0;
    sscanf(buf, "%u", &sw_bit);
    if(sw_bit < 64)
    {
        tele_mntn_func_sw_bit_clr(sw_bit);
    }
    return count;
}
static DEVICE_ATTR(on, (S_IRUGO|S_IWUSR), tele_mntn_sw_show, tele_mntn_sw_on_store);
static DEVICE_ATTR(off, (S_IRUGO|S_IWUSR), tele_mntn_sw_show, tele_mntn_sw_off_store);


static struct attribute *tele_mntn_sw_attrs[] = {
	&dev_attr_on.attr,
	&dev_attr_off.attr,
	NULL,
};

static struct attribute_group tele_mntn_sw_attribute_group = {
	.attrs = tele_mntn_sw_attrs,
};

static struct class *tele_mntn_sw_dev_class = NULL;

static int tele_mntn_sw_dev_create(void)
{
    struct device *dev_t;
    int ret = 0;

    tele_mntn_sw_dev_class = class_create(THIS_MODULE, "telemntnsw");
    if (IS_ERR(tele_mntn_sw_dev_class))
    {
        printk("tele_mntn_sw_dev_create: exc class create fail\n");
        return -1;
    }

    dev_t = device_create(tele_mntn_sw_dev_class, NULL, MKDEV(0, 0),
                NULL, "dev");
    if (IS_ERR(dev_t))
    {
        printk("tele_mntn_sw_dev_create: exc device create fail\n");
        return -1;
    }

    ret = sysfs_create_group(&dev_t->kobj, &tele_mntn_sw_attribute_group);
    if (ret)
    {
        printk("tele_mntn_sw_dev_create: exc group create fail\n");
        return -1;
    }
    return 0;
}
#endif

static ssize_t tele_mntn_sw_show(struct file *filp,  char  __user *buffer, size_t count, loff_t *ppos)
{
    int len = 0;
    char buf[32];
    unsigned long long sw_value = 0;
    unsigned int tmp_low = 0;
    unsigned int tmp_high = 0;
    memset(buf, 0, sizeof(buf));
    sw_value = tele_mntn_func_sw_get();

    tmp_low = (unsigned int)(sw_value & (unsigned int)0xFFFFFFFF);

    tmp_high = (unsigned int)((sw_value >> 32) & (unsigned int)0xFFFFFFFF);

    len = sprintf(buf, "0x%08x%08x\n", tmp_high, tmp_low);
	return simple_read_from_buffer(buffer, count, ppos, (void *)buf, len);
}


static ssize_t tele_mntn_sw_on_store(struct file *filp,  const char  __user *buffer, size_t count, loff_t *ppos)
{
    char buf[32] = {0};
    unsigned int sw_bit = 0;
    if(count >= sizeof(buf))
    {
        printk(KERN_ERR"tele_mntn_sw_on_store: input count larger than buf size \n");
        return (-ENOMEM);
    }
    if (copy_from_user(buf,buffer, count))
    {
        printk(KERN_ERR"tele_mntn_sw_on_store: copy_from_user from fm is err \n");
        return (-EFAULT);
    }
    sscanf(buf, "%u", &sw_bit);

    if(sw_bit < 64)
    {
        tele_mntn_func_sw_bit_set(sw_bit);
    }
    return count;
}

static ssize_t tele_mntn_sw_off_store(struct file *filp,  const char  __user *buffer, size_t count, loff_t *ppos)
{
    char buf[32] = {0};
    unsigned int sw_bit = 0;
    if(count >= sizeof(buf))
    {
        printk(KERN_ERR"tele_mntn_sw_off_store: input count larger than buf size \n");
        return (-ENOMEM);
    }
    if (copy_from_user(buf,buffer, count))
    {
        printk(KERN_ERR"tele_mntn_sw_off_store: copy_from_user from fm is err \n");
        return (-EFAULT);
    }
    sscanf(buf, "%u", &sw_bit);
    if(sw_bit < 64)
    {
        tele_mntn_func_sw_bit_clr(sw_bit);
    }
    else if(sw_bit == 64)
    {
        tele_mntn_func_sw_set(0);
    }
    return count;
}

static const struct file_operations tele_mntn_sw_on_fops = {
        .read = tele_mntn_sw_show,
        .write = tele_mntn_sw_on_store,
};

static const struct file_operations tele_mntn_sw_off_fops = {
        .read = tele_mntn_sw_show,
        .write = tele_mntn_sw_off_store,
};


static int tele_mntn_sw_debugfs_create(void)
{
    struct dentry *debugfs_root;

    debugfs_root = debugfs_create_dir("telemntnsw", NULL);
    if (!debugfs_root)
        return (-ENOENT);

    debugfs_create_file("on", (S_IRUGO|S_IWUSR), debugfs_root, NULL, &tele_mntn_sw_on_fops);

    debugfs_create_file("off", (S_IRUGO|S_IWUSR), debugfs_root, NULL, &tele_mntn_sw_off_fops);

    return 0;
}

static int tele_mntn_wake_up_acore_irq(union ipc_data *param)
{
    node_read_wait_condition = TELE_MNTN_NODE_READ_WAIT_CONDITION_FALSE;
    return 0;
}

static int __init proc_tele_mntn_node_init(void)
{
    int ret = 0;
    struct device_node *np;
	np = of_find_compatible_node(NULL, NULL, "hisilicon,hisi-tele-mntn");
	if (!of_property_read_bool(np, "telemntn_enable"))
	{
        return 0;
	}
    node_read_wait_timeout = msecs_to_jiffies(TELE_MNTN_NODE_READ_WAIT_TIMEOUT);
    tele_mntn_init();
    sema_init(&rw_sem, 1);
    proc_create("telemntn", S_IRUSR | S_IRGRP | S_IROTH, NULL, &proc_tele_mntn_node_operations);

#if 0  /*sys_class_telemntnsw_dev_onoff*/
    tele_mntn_sw_dev_create();
#endif
    tele_mntn_sw_debugfs_create();

    ret = ipc_msg_req_callback(OBJ_TELEMNTN, CMD_SETTING, tele_mntn_wake_up_acore_irq);
    if (0 != ret){
        printk(KERN_ERR "failed to connect IPC irq handle\n");
    }

    pr_info("proc_tele_mntn_node_init success\n");
    return 0;

}

module_init(proc_tele_mntn_node_init);
