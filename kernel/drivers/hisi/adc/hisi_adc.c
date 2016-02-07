/*
 * hisi_adc.c for the hkadc driver.
 *
 * Copyright (c) 2013 Hisilicon Technologies CO.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/notifier.h>
#include <linux/delay.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/debugfs.h>
#include <linux/hisi/hisi_mailbox.h>
#include <linux/hisi/hisi_mailbox_dev.h>
#include <linux/huawei/hisi_adc.h>

#define	MODULE_NAME		"hisi_adc"
/*adc maybe IPC timeout*/
#define ADC_IPC_TIMEOUT		1500
#define	ADC_IPC_MAX_CNT		3

#define HISI_AP_ID		0x00
#define	ADC_OBJ_ID		0x0b
#define ADC_IPC_CMD_TYPE0	0x02
#define	ADC_IPC_CMD_TYPE1	0x0c
#define	ADC_IPC_DATA		((HISI_AP_ID << 24) | (ADC_OBJ_ID << 16) | (ADC_IPC_CMD_TYPE0 << 8) | ADC_IPC_CMD_TYPE1)

#define	ADC_DATA_BIT_R(d, n)	((d) >> (n))
#define	ADC_DATA_MASK		0xff
#define	ADC_RESULT_ERR		(-EINVAL)
#define	ADC_CHN_MAX		18

enum {
	ADC_IPC_CMD_TYPE = 0,
	ADC_IPC_CMD_CHANNEL,
	ADC_IPC_CMD_LEN
};

struct adc_info {
	int channel;
	int value;
};

struct hisi_adc_device {
	struct adc_info		info;
	mbox_msg_t		tx_msg[ADC_IPC_CMD_LEN];
	struct hisi_mbox	*mbox;
	struct notifier_block	*nb;
	struct mutex		mutex;
	struct completion	completion;
};

static struct hisi_adc_device	*hisi_adc_dev;

#ifdef CONFIG_DEBUG_FS
/* creat a debugfs directory for adc debugging. */
static struct dentry		*debugfs_root;
static int buff[2];
#endif

/* notifiers AP when LPM3 sends msgs*/
static int adc_dev_notifier(struct notifier_block *nb, unsigned long len, void *msg)
{
	u32 *_msg = (u32 *)msg;
	int is_complete = ADC_RESULT_ERR;
#ifdef HI3XXX_HKADC_DEBUG
	unsigned long i;
	for (i = 0; i < len; i++)
		pr_info("%s_debug:[notifier] msg[%lu] = 0x%x\n", MODULE_NAME, i, _msg[i]);
#endif

	if (_msg[0] == ADC_IPC_DATA) {
		if (!(ADC_DATA_BIT_R(_msg[1], 8) & ADC_DATA_MASK)) {
			if ((_msg[1] & ADC_DATA_MASK) == hisi_adc_dev->info.channel) {
				hisi_adc_dev->info.value = ADC_DATA_BIT_R(_msg[1], 16);
				is_complete = 0;
			} else {
				pr_info("%s: msg[1][0x%x]\n", MODULE_NAME, _msg[1]);
			}
		} else {
			is_complete = 0;
			hisi_adc_dev->info.value = ADC_RESULT_ERR;
			pr_err("%s: error value msg[1][0x%x]\n", MODULE_NAME, _msg[1]);
		}
	}

	if (!is_complete)
		complete(&hisi_adc_dev->completion);

	return 0;
}

/* IPC complete function. */
static void adc_complete_func(struct hisi_mbox_task *tx_task)
{
#ifdef HI3XXX_HKADC_DEBUG
	int i;
	for (i = 0; i < tx_task->ack_buffer_len; i++)
		pr_info("%s_debug:[complete] msg[%d] = 0x%x\n", MODULE_NAME, i, tx_task->ack_buffer[i]);
#endif
	if (tx_task->tx_error)
		pr_err("%s: mail box complete error!\n", MODULE_NAME);

	hisi_mbox_task_free(&tx_task);
	return;
}

/* AP sends msgs to LPM3 for adc sampling&converting. */
static int adc_send_ipc_to_lpm3(int channel)
{
	struct hisi_mbox_task *adc_tx_task;
	int loop = ADC_IPC_MAX_CNT;
	int ret = 0;

	if (channel > ADC_CHN_MAX) {
		pr_err("%s: invalid channel!\n", MODULE_NAME);
		ret = -EINVAL;
		goto err_adc_channel;
	}

	if (!hisi_adc_dev) {
		pr_err("%s: adc dev is not initialized yet!\n", MODULE_NAME);
		ret = -ENODEV;
		goto err_adc_dev;
	}

	if (hisi_adc_dev->tx_msg[ADC_IPC_CMD_TYPE] != ADC_IPC_DATA)
		hisi_adc_dev->tx_msg[ADC_IPC_CMD_TYPE] = ADC_IPC_DATA;

	hisi_adc_dev->info.channel = channel;
	hisi_adc_dev->tx_msg[ADC_IPC_CMD_CHANNEL] = channel;

	adc_tx_task = hisi_mbox_task_alloc(hisi_adc_dev->mbox, HISI_MAILBOX_RP_LPM3, hisi_adc_dev->tx_msg,
							ADC_IPC_CMD_LEN, 1, adc_complete_func, NULL);
	if (!adc_tx_task) {
		pr_err("%s: fail to alloc mbox task!\n", MODULE_NAME);
		ret = -ENOMEM;
		goto err_alloc_task;
	}

	do {
		ret = hisi_mbox_msg_send_async(hisi_adc_dev->mbox, adc_tx_task);
		loop--;
	} while (ret == -ENOMEM && loop > 0);
	if (ret) {
		pr_err("%s: fail to send mbox msg, ret = %d!\n", MODULE_NAME, ret);
		goto err_msg_async;
	}

	return ret;

err_msg_async:
	hisi_mbox_task_free(&adc_tx_task);
err_alloc_task:
	hisi_adc_dev->info.channel = ADC_RESULT_ERR;
err_adc_dev:
err_adc_channel:
	return ret;
}

/*
 * Function name:hisi_adc_get_value.
 * Discription:get value from hkadc.
 * Parameters:
 *      @ adc_channel
 * return value:
 *      @ adc value: negative-->failed, other-->succeed.
 */
int hisi_adc_get_value(int adc_channel)
{
	int ret = 0;

	mutex_lock(&hisi_adc_dev->mutex);
	INIT_COMPLETION(hisi_adc_dev->completion);

	ret = adc_send_ipc_to_lpm3(adc_channel);
	if (ret)
		goto err_send_msg;

	ret = wait_for_completion_timeout(&hisi_adc_dev->completion, msecs_to_jiffies(ADC_IPC_TIMEOUT));
	if (!ret) {
		pr_err("%s: channel-%d ipc timeout!\n", MODULE_NAME, adc_channel);
		ret =  -ETIMEOUT;
	} else {
		ret = 0;
	}

err_send_msg:
	hisi_adc_dev->info.channel = ADC_RESULT_ERR;
	mutex_unlock(&hisi_adc_dev->mutex);
	return ret ? ret : hisi_adc_dev->info.value;
}
EXPORT_SYMBOL(hisi_adc_get_value);

#ifdef CONFIG_DEBUG_FS

/* debugfs for adc */
static int adc_debugfs_show(struct seq_file *s, void *data)
{
	return seq_printf(s, "ch-%d = %d\n", buff[0], buff[1]);
}

static int adc_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, adc_debugfs_show, NULL);
}

static ssize_t adc_debugfs_write(struct file *file,
					const char __user *user_buf,
					size_t count,
					loff_t *ppos)
{
	char *buf;
	int ret;
	long val;

	buf = kzalloc(sizeof(char) * count, GFP_KERNEL);
	if (!buf) {
		pr_err("%s_debug: failed to alloc buffer for debugfs!\n", MODULE_NAME);
		return count;
	}

	if (copy_from_user(buf, user_buf, count)) {
		kfree(buf);
		return -EFAULT;
	}

	ret = kstrtol(buf, 10, &val);
	if (ret < 0) {
		pr_err("%s: input error!\n", MODULE_NAME);
		kfree(buf);
		return count;
	}

	buff[0] = (int)val;
	buff[1] = hisi_adc_get_value(buff[0]);
	pr_info("%s: channel[%d] value is %d\n",
					MODULE_NAME,
					buff[0],
					buff[1]);
	kfree(buf);
	return count;
}

static const struct file_operations adc_operations = {
	.open		= adc_debugfs_open,
	.read		= seq_read,
	.write		= adc_debugfs_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void adc_init_device_debugfs(void)
{
	buff[0] = ADC_RESULT_ERR;
	buff[1] = ADC_RESULT_ERR;

	debugfs_root = debugfs_create_dir(MODULE_NAME, NULL);
	if (IS_ERR(debugfs_root) || !debugfs_root) {
		pr_err("%s: failed to create debugfs directory!\n", MODULE_NAME);
		return;
	}

	debugfs_create_file("channel", S_IFREG | S_IRUGO,
			    debugfs_root, NULL, &adc_operations);
}

static void adc_remove_device_debugfs(void)
{
	debugfs_remove_recursive(debugfs_root);
}

#else

static void adc_init_device_debugfs(void)
{
}

static void adc_remove_device_debugfs(void)
{
}

#endif

/* hisi adc init function */
static int __init hisi_adc_driver_init(void)
{
	int ret = 0;

	hisi_adc_dev = kzalloc(sizeof(*hisi_adc_dev), GFP_KERNEL);
	if (!hisi_adc_dev) {
		pr_err("%s: fail to alloc adc dev!\n", MODULE_NAME);
		ret = -ENOMEM;
		goto err_adc_dev;
	}

	hisi_adc_dev->nb = kzalloc(sizeof(struct notifier_block), GFP_KERNEL);
	if (!hisi_adc_dev->nb) {
		pr_err("%s: fail to alloc notifier_block!\n", MODULE_NAME);
		ret =  -ENOMEM;
		goto err_adc_nb;
	}

	hisi_adc_dev->nb->next = NULL;
	hisi_adc_dev->nb->notifier_call = adc_dev_notifier;

	hisi_adc_dev->mbox = hisi_mbox_get(HISI_MAILBOX_RP_LPM3, hisi_adc_dev->nb);
	if (!hisi_adc_dev->mbox) {
		pr_err("%s: fail to get mail box!\n", MODULE_NAME);
		ret = -ENODEV;
		goto err_get_mbox;
	}

	hisi_adc_dev->tx_msg[ADC_IPC_CMD_TYPE] = ADC_IPC_DATA;
	hisi_adc_dev->tx_msg[ADC_IPC_CMD_CHANNEL] = ADC_RESULT_ERR;

	mutex_init(&hisi_adc_dev->mutex);
	init_completion(&hisi_adc_dev->completion);
	adc_init_device_debugfs();

	pr_info("%s: init ok!\n", MODULE_NAME);
	return ret;

err_get_mbox:
	kfree(hisi_adc_dev->nb);
	hisi_adc_dev->nb = NULL;
err_adc_nb:
	kfree(hisi_adc_dev);
	hisi_adc_dev = NULL;
err_adc_dev:
	return ret;
}

/* hisi adc exit function */
static void __exit hisi_adc_driver_exit(void)
{
	if (hisi_adc_dev) {
		hisi_mbox_put(&hisi_adc_dev->mbox);

		kfree(hisi_adc_dev->nb);
		hisi_adc_dev->nb = NULL;

		kfree(hisi_adc_dev);
		hisi_adc_dev = NULL;
	}

	adc_remove_device_debugfs();
}

fs_initcall(hisi_adc_driver_init);
module_exit(hisi_adc_driver_exit);
