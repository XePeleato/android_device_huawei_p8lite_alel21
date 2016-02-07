/*
 * hisi rproc debugfs
 *
 * Copyright (c) 2013- Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/notifier.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/hisi/hisi_rproc.h>

#define RPROC_LISTENER_NUM 5
#define RPROC_TX_WORK_NUM 5
#define ASYNC_CALL 0
#define SYNC_CALL 1

struct rproc_debugfs_listener {
	unsigned char id;
	struct notifier_block nb;
};

struct rproc_debugfs_tx_work {
	unsigned char id;
	rproc_msg_t msg[8];
	rproc_msg_len_t len;
	int is_sync_call;
	int times;
	int tx_num;
	struct work_struct work;
};

static int listener_cnt;
struct rproc_debugfs_listener *listener[RPROC_LISTENER_NUM];
struct rproc_debugfs_tx_work *rproc_work_table[RPROC_TX_WORK_NUM];

static int stoa(char *cmd, mbox_msg_t *msg)
{
	char *_cmd = cmd;
	int i = 0;

	while (1) {
		if (i >= 8)
			break;

		if (' ' == *_cmd) {
			*_cmd = '\0';
			if (kstrtou32(cmd, 16, &msg[i]))
				return -EINVAL;
			cmd = _cmd + 1;
			*_cmd = ' ';
			i++;
		} else if ('\0' == *_cmd) {
			if (kstrtou32(cmd, 16, &msg[i]))
				return -EINVAL;
			i++;
			break;
		} else {
			;
		}
		_cmd++;
	}
	return i;
}

static void
rproc_debugfs_complete(rproc_msg_t *ack_buffer, rproc_msg_len_t ack_buffer_len,
						int error, void *data)
{
	struct rproc_debugfs_tx_work *rproc_work =
				(struct rproc_debugfs_tx_work *)data;
	rproc_msg_len_t i;

	rproc_work->tx_num++;
	if (error) {
		pr_err("[RPROC DEBUGFS] ASYNC-CALL complete error: %d ---- No.%d\n",
			error, rproc_work->tx_num);
		return;
	}

	pr_info("[RPROC DEBUGFS] ASYNC-CALL complete ---- No.%d\n",
		rproc_work->tx_num);
	for (i = 0; i < ack_buffer_len; i++) {
		pr_debug("[RPROC DEBUGFS] ASYNC-ACK[%d]: 0x%08x\n",
							i, ack_buffer[i]);
	}

	return;
}

static void rproc_debugfs_work(struct work_struct *work)
{
	struct rproc_debugfs_tx_work *rproc_work =
		container_of(work, struct rproc_debugfs_tx_work, work);
	rproc_msg_t ack_buffer[8] = {0};
	rproc_msg_len_t ack_buffer_len = 8;
	int i, j, ret, times;

	times = rproc_work->times;
	rproc_work->tx_num = 0;
	for (i = 0; i < times; i++) {
		if (rproc_work->is_sync_call) {
			pr_info("[RPROC DEBUGFS] SYNC-CALL start ---- No.%d\n",
				i + 1);
			ret = RPROC_SYNC_SEND(rproc_work->id,
						rproc_work->msg,
						rproc_work->len,
						ASYNC_MSG,
						ack_buffer,
						ack_buffer_len);
			if (ret) {
				pr_err("[RPROC DEBUGFS] SYNC-CALL error: %d ---- No.%d\n",
					ret, i + 1);
			} else {
				pr_info("[RPROC DEBUGFS] SYNC-CALL complete ---- No.%d\n",
						i + 1);
				for (j = 0; j < ack_buffer_len; j++)
					pr_debug("[RPROC DEBUGFS] SYNC-ACK[%d]: 0x%08x\n",
						j, ack_buffer[j]);
			}
		} else {
			pr_info("[RPROC DEBUGFS] ASYNC-CALL start ---- No.%d\n",
				i + 1);
			ret = RPROC_ASYNC_SEND(rproc_work->id,
						rproc_work->msg,
						rproc_work->len,
						ASYNC_MSG,
						rproc_debugfs_complete,
						rproc_work);
			if (ret) {
				rproc_work->tx_num++;
				pr_err("[RPROC DEBUGFS] ASYNC-CALL error: %d ---- No.%d\n",
					ret, i + 1);
				msleep(10);
			}
		}
	}

	return;
}

static void
rproc_debugfs_schedule_work(unsigned char id, rproc_msg_t *msg,
		rproc_msg_len_t len, int times, int is_sync_call)
{
	struct rproc_debugfs_tx_work *rproc_work;
	int i;

	for (i = 0; i < RPROC_TX_WORK_NUM; i++) {
		rproc_work = rproc_work_table[i];
		if (!work_pending(&rproc_work->work)) {
			rproc_work->id = id;
			memset(rproc_work->msg, 0, sizeof(rproc_work->msg));
			memcpy((void *)rproc_work->msg,
				(void *)msg,
				sizeof(rproc_msg_t) / sizeof(u8) * len);

			rproc_work->len = len;
			rproc_work->times = times;
			rproc_work->is_sync_call = is_sync_call;
			schedule_work(&rproc_work->work);

			break;
		}
	}

	if (RPROC_TX_WORK_NUM == i)
		pr_err("[RPROC DEBUGFS] no free work\n");

	return;
}

static void
rproc_debugfs_sync_call_test(unsigned char rproc_id, rproc_msg_t *msg,
				rproc_msg_len_t len, int times)
{
	rproc_debugfs_schedule_work(rproc_id, msg, len, times, SYNC_CALL);
}

static void
rproc_debugfs_async_call_test(unsigned char rproc_id, rproc_msg_t *msg,
				rproc_msg_len_t len, int times)
{
	rproc_debugfs_schedule_work(rproc_id, msg, len, times, ASYNC_CALL);
}

static int
rproc_debugfs_notifier(struct notifier_block *nb, unsigned long len, void *msg)
{
	struct rproc_debugfs_listener *someone =
			container_of(nb, struct rproc_debugfs_listener, nb);
	rproc_msg_t *_msg = (rproc_msg_t *)msg;
	unsigned long i;

	pr_info("[RPROC DEBUGFS] %d rx notifier\n", someone->id);
	for (i = 0; i < len; i++)
		pr_debug("[RPROC DEBUGFS] MSG[%lu]: 0x%08x\n", i, _msg[i]);

	return 0;
}

static void rproc_debugfs_rx_register(unsigned char rproc_id)
{
	struct rproc_debugfs_listener *new_listener = NULL;

	if (listener_cnt >= RPROC_LISTENER_NUM) {
		pr_err("[RPROC DEBUGFS] no more listener\n");
		return;
	}

	new_listener = kmalloc(sizeof(*new_listener), GFP_KERNEL);
	if (!new_listener)
		return;
	new_listener->id = rproc_id;
	new_listener->nb.next = NULL;
	new_listener->nb.notifier_call = rproc_debugfs_notifier;

	if (RPROC_MONITOR_REGISTER(rproc_id, &new_listener->nb)) {
		pr_info("[RPROC DEBUGFS] fail to register rx listener, rproc %d\n",
			rproc_id);
		kfree(new_listener);
		return;
	}

	listener[listener_cnt] = new_listener;
	listener_cnt++;
	return;
}

static void rproc_debugfs_rx_unregister(int listeners)
{
	struct rproc_debugfs_listener *de_listener = NULL;
	int i;

	if (listeners > listener_cnt || listeners <= 0) {
		pr_info("[RPROC DEBUGFS] no such listener\n");
		return;
	}

	de_listener = listener[listeners - 1];
	(void)RPROC_MONITOR_UNREGISTER(de_listener->id, &de_listener->nb);

	kfree(de_listener);
	listener[listeners - 1] = NULL;
	for (i = listeners; i < RPROC_LISTENER_NUM; i++) {
		listener[i - 1] = listener[i];
		listener[i] = NULL;
	}

	listener_cnt--;
	return;
}

static ssize_t
rproc_debugfs_write(struct file *filp, const char __user *ubuf,
					size_t cnt, loff_t *ppos)
{
	char buf[196];
	char *cmd = NULL;
	char *_cmd = NULL;
       int rproc_id;
	int new_listener;
	rproc_msg_t msg[8];
	rproc_msg_len_t len;
	int times = 0;

	if (copy_from_user(buf, ubuf, cnt - 1)) {
		pr_err("[DDR DEVFREQ DEBUGFS] can not copy from user\n");
		cnt = -EINVAL;
		goto out;
	}

	buf[cnt - 1] = '\0';
	cmd = buf;
	_cmd = buf;
	pr_info("[RPROC DEBUGFS] [cmd: %s[cnt: %d]]\n", cmd, (int)cnt);

	if (!strncmp("register ", _cmd, strlen("register "))) {
		cmd = cmd + strlen("register ");
		_cmd = cmd;
		while ((' ' != *_cmd) && ('\0' != *_cmd))
			_cmd++;
		*_cmd = '\0';

		if (kstrtos32(cmd, 10, &rproc_id)) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}

		rproc_debugfs_rx_register((unsigned char)rproc_id);
	} else if (!strncmp("unregister listener-", _cmd,
					strlen("unregister listener-"))) {
		cmd = cmd + strlen("unregister listener-");
		_cmd = cmd;
		while ((' ' != *_cmd) && ('\0' != *_cmd))
			_cmd++;
		*_cmd = '\0';

		if (kstrtos32(cmd, 10, &new_listener)) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}

		rproc_debugfs_rx_unregister(new_listener);
	} else if (!strncmp("sync-send ", _cmd, strlen("sync-send "))) {
		cmd = cmd + strlen("sync-send ");
		_cmd = cmd;
		while ((' ' != *_cmd) && ('\0' != *_cmd))
			_cmd++;

		if ('\0' == *_cmd) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}
		*_cmd = '\0';

		if (kstrtos32(cmd, 10, &times)) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}

		cmd = _cmd + 1;
		_cmd = cmd;
		while ((' ' != *_cmd) && ('\0' != *_cmd))
			_cmd++;

		if ('\0' == *_cmd) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}
		*_cmd = '\0';

		if (kstrtos32(cmd, 10, &rproc_id)) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}

		cmd = _cmd + 1;
		len = stoa(cmd, msg);

		rproc_debugfs_sync_call_test((unsigned char)rproc_id, msg, len, times);
	} else if (!strncmp("async-send ", _cmd, strlen("async-send "))) {
		cmd = cmd + strlen("async-send ");
		_cmd = cmd;
		while ((' ' != *_cmd) && ('\0' != *_cmd))
			_cmd++;

		if ('\0' == *_cmd) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}
		*_cmd = '\0';

		if (kstrtos32(cmd, 10, &times)) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}

		cmd = _cmd + 1;
		_cmd = cmd;
		while ((' ' != *_cmd) && ('\0' != *_cmd))
			_cmd++;

		if ('\0' == *_cmd) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}
		*_cmd = '\0';

		if (kstrtos32(cmd, 10, &rproc_id)) {
			pr_err("[RPROC DEBUGFS] cmd error\n");
			cnt = -EINVAL;
			goto out;
		}

		cmd = _cmd + 1;
		len = stoa(cmd, msg);

		rproc_debugfs_async_call_test((unsigned char)rproc_id, msg, len, times);
	} else {
		pr_err("[RPROC DEBUGFS] cmd error\n");
		cnt = -EINVAL;
	}
out:
	return cnt;
}

static int rproc_debugfs_show(struct seq_file *s, void *data)
{
	return 0;
}

static int rproc_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, rproc_debugfs_show, inode->i_private);
}

static const struct file_operations rproc_debugfs_fops = {
	.open		= rproc_debugfs_open,
	.read		= seq_read,
	.write		= rproc_debugfs_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static struct dentry *rproc_debug_dir;
static struct dentry *rproc_debug_fn;

static int __init rproc_debugfs_init(void)
{
	struct rproc_debugfs_tx_work *new_work;
	int i;

	rproc_debug_dir = debugfs_create_dir("rproc", NULL);
	if (rproc_debug_dir)
		rproc_debug_fn = debugfs_create_file("debug",
			S_IRUGO, rproc_debug_dir, NULL, &rproc_debugfs_fops);

	for (i = 0; i < RPROC_TX_WORK_NUM; i++) {
		new_work = kzalloc(sizeof(struct rproc_debugfs_tx_work),
								GFP_KERNEL);
		if (!new_work)
			goto err;

		INIT_WORK(&new_work->work, rproc_debugfs_work);
		rproc_work_table[i] = new_work;
	}

	listener_cnt = 0;
	return 0;

err:
	while (i--) {
		new_work = rproc_work_table[i];
		flush_work(&new_work->work);
		kfree(new_work);
		rproc_work_table[i] = NULL;
	}

	debugfs_remove(rproc_debug_fn);
	debugfs_remove(rproc_debug_dir);
	return 0;
}
module_init(rproc_debugfs_init);

static void __exit rproc_debugfs_exit(void)
{
	struct rproc_debugfs_tx_work *de_work;
	int i;

	debugfs_remove(rproc_debug_fn);
	debugfs_remove(rproc_debug_dir);

	for (i = 0; i < RPROC_TX_WORK_NUM; i++) {
		de_work = rproc_work_table[i];
		flush_work(&de_work->work);
		kfree(de_work);
	}

	return;
}
module_exit(rproc_debugfs_exit);

MODULE_DESCRIPTION("HISI rproc debugfs");
MODULE_LICENSE("GPL V2");
