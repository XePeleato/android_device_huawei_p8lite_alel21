/*
 * mailbox debugfs user
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
#include <linux/hisi/hisi_mailbox.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>

#define MODULE_NAME "MAILBOX_DEBUGFS"
#define DEBUGFS_MBOXES_NUM 10

/* mailbox debugfs user mailboxes information structure */
struct mbox_info {
	char *rp_name;
	struct hisi_mbox *mbox;
	struct notifier_block *nb;
	mbox_msg_len_t len;
	mbox_msg_t *msg;
	int index;
	int initialized;
	struct work_struct work;
};

/* mailbox debuffs user mailbox tx task information */
struct mbox_task {
	int mbox_index;
	int mail_index;
	mbox_msg_t *tx_buffer;
};

/* mailbox debugfs user mailboxes information */
static struct mbox_info debugfs_mboxes[DEBUGFS_MBOXES_NUM];

/* to monitor mailboxes of mailbox core driver */
static struct hisi_mbox_device **debugfs_mdevices;

/* debugfs control parameter */
static int debugfs_ipc_times;

static void mbox_debugfs_complete(struct hisi_mbox_task *tx_task)
{
	struct mbox_task *task_info = (struct mbox_task *)tx_task->context;
	int i;

	if (task_info) {
		pr_debug("mailbox-%d debugfs msg NO.%d complete\n",
			task_info->mbox_index, task_info->mail_index);

		kfree(task_info->tx_buffer);
		kfree(task_info);
		tx_task->context = NULL;
	}

	for (i = 0; i < tx_task->ack_buffer_len; i++)
		pr_debug("msg[%d] = 0x%x\n", i, tx_task->ack_buffer[i]);

	hisi_mbox_task_free(&tx_task);
	return;
}

static struct hisi_mbox_task *mbox_debugfs_get_task(struct hisi_mbox *mbox,
					mbox_msg_t *msg,
					mbox_msg_len_t len,
					char *rp_name,
					struct mbox_task *context)
{
	struct hisi_mbox_task *tx_task = NULL;
	mbox_msg_t *tx_buffer = NULL;

	tx_buffer = kmalloc(sizeof(mbox_msg_t) * len, GFP_KERNEL);
	if (!tx_buffer)
		return tx_task;
	memcpy(tx_buffer, msg, sizeof(mbox_msg_t) * len);
	context->tx_buffer = tx_buffer;

	tx_task = hisi_mbox_task_alloc(mbox,
				rp_name,
				tx_buffer,
				len,
				1,
				mbox_debugfs_complete,
				context);

	if (!tx_task)
		kfree(tx_buffer);

	return tx_task;
}

static void mbox_debugfs_send_work(struct work_struct *work)
{
	struct mbox_info *mbox_info =
		container_of(work, struct mbox_info, work);
	struct mbox_task *task_info = NULL;
	struct hisi_mbox_task *tx_task = NULL;
	int i = 0;
	int time = debugfs_ipc_times;

	while (time) {
		pr_info("%s debugfs msg send NO.%d\n",
				mbox_info->mbox->tx->name, ++i);
		task_info = kmalloc(sizeof(*task_info), GFP_KERNEL);
		if (!task_info)
			continue;
		task_info->mbox_index = mbox_info->index;
		task_info->mail_index = i;

		tx_task = mbox_debugfs_get_task(mbox_info->mbox,
						mbox_info->msg,
						mbox_info->len,
						mbox_info->rp_name,
						task_info);
		if (!tx_task) {
			msleep(100);
			continue;
		}

		if (hisi_mbox_msg_send_async(mbox_info->mbox, tx_task)) {
			kfree(task_info->tx_buffer);
			kfree(task_info);
			hisi_mbox_task_free(&tx_task);

			if (i == debugfs_ipc_times) {
				debugfs_ipc_times -= 1;
				break;
			}

			msleep(100);
		}

		time--;
	}

	return;
}

static int mbox_debugfs_msg_send(int index, mbox_msg_t *msg, mbox_msg_len_t len)
{
	if ((index > DEBUGFS_MBOXES_NUM) || (index < 1)) {
		pr_err("mailbox debugfs index %d error\n", index);
		return -EINVAL;
	}

	if (!debugfs_mboxes[index - 1].mbox) {
		pr_err("mailbox-%d debugfs no such mbox\n", index);
		return -EINVAL;
	}

	debugfs_mboxes[index - 1].msg =
			kmalloc(sizeof(mbox_msg_t) * len, GFP_KERNEL);
	if (!debugfs_mboxes[index - 1].msg) {
		pr_err("mailbox-%d debugfs no mem\n", index);
		return -ENOMEM;
	}
	memcpy(debugfs_mboxes[index - 1].msg, msg, sizeof(mbox_msg_t) * len);
	debugfs_mboxes[index - 1].len = len;
	debugfs_mboxes[index - 1].index = index;

	if (!debugfs_mboxes[index - 1].initialized) {
		INIT_WORK(&debugfs_mboxes[index - 1].work,
					mbox_debugfs_send_work);
		debugfs_mboxes[index - 1].initialized = 1;
	} else {
		pr_err("mailbox-%d debugfs has initialize its work\n", index);
	}

	if (!work_busy(&debugfs_mboxes[index - 1].work))
		schedule_work(&debugfs_mboxes[index - 1].work);
	else
		pr_err("mailbox-%d debugfs work busy, wait a moment\n", index);

	return 0;
}

static int mbox_debugfs_msg_get(char *cmd, mbox_msg_t *msg)
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

static int mbox_debugfs_mbox_config(int index, char *rp_name)
{
	char *_rp_name = NULL;

	if ((index > DEBUGFS_MBOXES_NUM) || (index < 1)) {
		pr_err("mailbox-%d debugfs index error\n", index);
		return -EINVAL;
	}

	if (!debugfs_mboxes[index - 1].mbox) {
		pr_err("mailbox-%d debugfs no such mbox\n", index);
		return -EINVAL;
	}

	_rp_name = kmalloc(strlen(rp_name) + 1, GFP_KERNEL);
	if (!_rp_name)
		return -ENOMEM;
	strncpy(_rp_name, rp_name, strlen(rp_name));
	*(_rp_name + strlen(rp_name)) = '\0';

	kfree(debugfs_mboxes[index - 1].rp_name);
	debugfs_mboxes[index - 1].rp_name = _rp_name;
	return 0;
}

static void mbox_debufs_mbox_show(void)
{
	int i;

	for (i = 0; (debugfs_mboxes[i].mbox) && (i < DEBUGFS_MBOXES_NUM); i++) {
		pr_info("mailbox-%d: %s, ->%s\n",
			i + 1, debugfs_mboxes[i].mbox->name,
			debugfs_mboxes[i].rp_name);
	}

	if (i >= DEBUGFS_MBOXES_NUM)
		pr_err("no mailbox gotten\n");
	return;
}

static int
mbox_debugfs_notifier(struct notifier_block *nb, unsigned long len, void *msg)
{
	int i;

	pr_info("--------------------mailbox debugfs notifier rx msg--------------------\n");

	for (i = 0; i < len; i++)
		pr_debug("msg[%d] = [0x%x]\n", i, ((mbox_msg_t *)msg)[i]);

	return 0;
}

static int mbox_debugfs_mbox_put(u32 index)
{
	struct hisi_mbox *mbox = NULL;
	struct notifier_block *nb = NULL;
	int i;

	if (index > DEBUGFS_MBOXES_NUM || index < 1)
		return -EINVAL;

	mbox = debugfs_mboxes[index - 1].mbox;
	if (!mbox)
		return -EINVAL;
	else
		nb = debugfs_mboxes[index - 1].nb;

	hisi_mbox_put(&mbox);

	debugfs_mboxes[index - 1].mbox = NULL;
	kfree(debugfs_mboxes[index - 1].rp_name);
	debugfs_mboxes[index - 1].rp_name = NULL;
	kfree(nb);

	for (i = index; (i < DEBUGFS_MBOXES_NUM) && (debugfs_mboxes[i].mbox);
									i++) {
		debugfs_mboxes[i - 1].mbox = debugfs_mboxes[i].mbox;
		debugfs_mboxes[i - 1].nb = debugfs_mboxes[i].nb;
		debugfs_mboxes[i - 1].rp_name = debugfs_mboxes[i].rp_name;
		debugfs_mboxes[i].mbox = NULL;
		debugfs_mboxes[i].nb = NULL;
		debugfs_mboxes[i].rp_name = NULL;
	}
	return 0;
}

static int mbox_debugfs_mbox_get(char *name)
{
	struct notifier_block *nb = NULL;
	struct hisi_mbox *mbox = NULL;
	char *rp_name = NULL;
	int i;

	if (!name)
		return -EINVAL;

	for (i = 0; (debugfs_mboxes[i].mbox) && (i < DEBUGFS_MBOXES_NUM); i++)
		;

	if (i >= DEBUGFS_MBOXES_NUM)
		return -ENOMEM;

	nb = kmalloc(sizeof(*nb), GFP_KERNEL);
	if (!nb)
		return -ENOMEM;

	nb->next = NULL;
	nb->notifier_call = mbox_debugfs_notifier;
	mbox = hisi_mbox_get(name, nb);
	if (!mbox) {
		kfree(nb);
		return -ENODEV;
	}

	rp_name = kmalloc(sizeof(char) * (strlen(name) + 1), GFP_KERNEL);
	if (!rp_name) {
		hisi_mbox_put(&mbox);
		kfree(nb);
		return -ENOMEM;
	}
	strncpy(rp_name, name, strlen(name) + 1);
	*(rp_name + strlen(name)) = '\0';

	MBOX_DEBUG_ON(mbox);
	debugfs_mboxes[i].mbox = mbox;
	debugfs_mboxes[i].nb = nb;
	debugfs_mboxes[i].rp_name = rp_name;
	debugfs_mboxes[i].initialized = 0;
	return 0;
}

static ssize_t
mbox_debugfs_write(struct file *filp, const char __user *ubuf,
					size_t cnt, loff_t *ppos)
{
	char debugfs_cmd[128];
	char *_cmd = NULL;
	char *_tmp = NULL;
	char rp_name[24];
	u32 _mbox_index;
	mbox_msg_t msg[8];
	mbox_msg_len_t len;

	if (copy_from_user(debugfs_cmd, ubuf, cnt - 1)) {
		cnt = -EINVAL;
		goto out;
	}

	debugfs_cmd[cnt - 1] = '\0';
	_cmd = debugfs_cmd;
	pr_debug("%s: [cmd: %s[cnt: %d]]\n", MODULE_NAME, debugfs_cmd, (int)cnt);

	if (!strncmp("get ", _cmd, strlen("get "))) {
		_cmd = _cmd + strlen("get ");
		if (mbox_debugfs_mbox_get(_cmd)) {
			cnt = -EINVAL;
			goto out;
		}
	} else if (!strncmp("put ", _cmd, strlen("put "))) {
		_cmd = _cmd + strlen("put ");
		if (!strncmp("mailbox-", _cmd, strlen("mailbox-"))) {
			_cmd = _cmd + strlen("mailbox-");
			if (kstrtou32(_cmd, 10, &_mbox_index)) {
				cnt = -EINVAL;
				goto out;
			} else {
				if (mbox_debugfs_mbox_put(_mbox_index)) {
					cnt = -EINVAL;
					goto out;
				}
			}
		} else {
			cnt = -EINVAL;
			goto out;
		}
	} else if (!strncmp("show", _cmd, strlen("show"))) {
		mbox_debufs_mbox_show();
	} else if (!strncmp("config ", _cmd, strlen("config "))) {
		_cmd = _cmd + strlen("config ");
		if (!strncmp("mailbox-", _cmd, strlen("mailbox-"))) {
			_cmd = _cmd + strlen("mailbox-");
			_tmp = _cmd;
			while ((' ' != *_tmp) && ('\0' != *_tmp))
				_tmp++;
			if ('\0' == *_tmp) {
				cnt = -EINVAL;
				goto out;
			}

			*_tmp = '\0';
			if (kstrtou32(_cmd, 10, &_mbox_index)) {
				cnt = -EINVAL;
				goto out;
			}

			*_tmp = ' ';
			_cmd = _tmp + 1;
			_tmp = _cmd;
			while ((' ' != *_tmp) && ('\0' != *_tmp))
				_tmp++;
			strncpy(rp_name, _cmd, _tmp - _cmd);
			rp_name[_tmp - _cmd] = '\0';

			if (mbox_debugfs_mbox_config(_mbox_index, rp_name)) {
				cnt = -EINVAL;
				goto out;
			}
		} else {
			cnt = -EINVAL;
			goto out;
		}
	} else if (!strncmp("send ", _cmd, strlen("send "))) {
		_cmd = _cmd + strlen("send ");
		if (!strncmp("mailbox-", _cmd, strlen("mailbox-"))) {
			_cmd = _cmd + strlen("mailbox-");
			_tmp = _cmd;
			while ((' ' != *_tmp) && ('\0' != *_tmp))
				_tmp++;
			if ('\0' == *_tmp) {
				cnt = -EINVAL;
				goto out;
			}
			*_tmp = '\0';
			if (kstrtou32(_cmd, 10, &_mbox_index)) {
				cnt = -EINVAL;
				goto out;
			}
			*_tmp = ' ';
			_cmd = _tmp + 1;

			len = mbox_debugfs_msg_get(_cmd, msg);
			if (len < 0) {
				cnt = -EINVAL;
				goto out;
			}

			if (mbox_debugfs_msg_send(_mbox_index, msg, len)) {
				cnt = -EINVAL;
				goto out;
			}
		} else {
			cnt = -EINVAL;
			goto out;
		}
	} else if (!strncmp("set times ", _cmd, strlen("set times "))) {
		_cmd = _cmd + strlen("set times ");
		_tmp = _cmd;
		while ((' ' != *_tmp) && ('\0' != *_tmp))
				_tmp++;
		*_tmp = '\0';
		if (kstrtos32(_cmd, 10, &debugfs_ipc_times)) {
			cnt = -EINVAL;
			goto out;
		}

		pr_info("set ipc times %d\n", debugfs_ipc_times);
	} else {
		cnt = -EINVAL;
		goto out;
	}

out:
	return cnt;
}

static int mbox_debugfs_show(struct seq_file *s, void *data)
{
	struct hisi_mbox_device **list = debugfs_mdevices;
	struct hisi_mbox_device *mdev = NULL;
	int i, j;

	pr_info("------------------------------hisi mailboxes status------------------------------\n\n");

	if (!list) {
		pr_info("no mailbox device resources currently\n");
		goto out;
	}

	for (i = 0; (mdev = list[i]); i++) {
		pr_info("----------------%s----------------\n", mdev->name);
		pr_info("--status:         %s\n",
				mdev->configured ? "startup" : "shutdown");
		pr_info("--fifo length:    %d\n", kfifo_len(&mdev->fifo));
		pr_info("--fifo avail:     %d\n", kfifo_avail(&mdev->fifo));

		if (mdev->rx_buffer) {
			pr_info("--rx buffer:\n");
			for (j = 0; j < 8; j++)
				pr_info("0x%x\n", mdev->rx_buffer[i]);
		} else {
			pr_info("--rx buffer:      NULL\n");
		}

		if (mdev->ack_buffer) {
			pr_info("--ack buffer:\n");
			for (j = 0; j < 8; j++)
				pr_info("0x%x\n", mdev->ack_buffer[i]);
		} else {
			pr_info("--ack buffer:     NULL\n");
		}

		if (mdev->tx_task) {
			pr_info("--tx task:        SENDING\n");
			pr_info("--rp name:        %s\n",
				mdev->tx_task->rp_name ?
				mdev->tx_task->rp_name : "NULL");

			if (mdev->tx_task->tx_buffer) {
				pr_info("--tx buffer:\n");
					for (j = 0; j < mdev->tx_task->tx_buffer_len; j++)
						pr_info("0x%x\n", mdev->tx_task->tx_buffer[i]);
			} else {
				pr_info("--tx buffer:      NULL\n");
			}

			if (mdev->tx_task->ack_buffer) {
				pr_info("--ack buffer:\n");
					for (j = 0; j < mdev->tx_task->ack_buffer_len; j++)
						pr_info("0x%x\n", mdev->tx_task->ack_buffer[i]);
			} else {
				pr_info("--ack buffer:     NULL\n");
			}

			pr_info("--auto ack:       %s\n",
				mdev->tx_task->need_auto_ack ? "ON" : "OFF");
		} else {
			pr_info("--tx task:        NULL\n");
			pr_info("--rp name:        NULL\n");
			pr_info("--tx buffer:      NULL\n");
			pr_info("--ack buffer:     NULL\n");
			pr_info("--auto ack:       NULL\n");
		}
	}

out:
	return 0;
}

static int mbox_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, mbox_debugfs_show, inode->i_private);
}

static const struct file_operations mbox_debugfs_fops = {
	.open		= mbox_debugfs_open,
	.read		= seq_read,
	.write		= mbox_debugfs_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void mbox_debugfs_register(struct hisi_mbox_device **list)
{
	debugfs_mdevices = list;
	return;
}
EXPORT_SYMBOL(mbox_debugfs_register);

void mbox_debugfs_unregister(void)
{
	debugfs_mdevices = NULL;
	return;
}
EXPORT_SYMBOL(mbox_debugfs_unregister);

static struct dentry *mbox_debug_dir;
static struct dentry *mbox_debug_fn;

static int __init mbox_debugfs_init(void)
{
	mbox_debug_dir = debugfs_create_dir("hisi_mbox_debug", NULL);
	if (mbox_debug_dir)
		mbox_debug_fn = debugfs_create_file("debug",
			S_IRUGO, mbox_debug_dir, NULL, &mbox_debugfs_fops);

	return 0;
}
module_init(mbox_debugfs_init);

static void __exit mbox_debugfs_exit(void)
{
	debugfs_remove(mbox_debug_fn);
	debugfs_remove(mbox_debug_dir);

	return;
}
module_exit(mbox_debugfs_exit);

MODULE_DESCRIPTION("HS mailbox debugfs user");
MODULE_LICENSE("GPL V2");
