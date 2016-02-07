#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/notifier.h>
#include <linux/pm_qos.h>

#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>

#include <linux/hisi/hisi_mailbox.h>

#define MODULE_NAME	"hisilicon,lowpowerm3"

#define MAX_CMD		4
#define MAX_OBJ		17
#define MAX_TYPE	13

static struct dentry *lpm3_debug_dir = NULL;
static struct notifier_block *nb;
static struct notifier_block *nb_nor;
static struct hisi_mbox *mbox;
static struct hisi_mbox *mbox_nor;
unsigned int display_mailbox_rec;

char *cmd[MAX_CMD] = {"on", "off", "get", "set"};
char *obj[MAX_OBJ] = {"ap ", "little ", "big ", "gpu ", "ddr ", "asp ", "hifi ",
		"iom3 ", "lpm3 ", "modem ", "sys ", "hkadc ",
		"regulator ", "clock ", "temp ", "coul ", "psci "};
char *type[MAX_TYPE] = {"power ", "clk ", "core ", "cluster ", "sleep ",
		"sr ", "mode ", "uplimit ", "dnlimit ","freq ",
		"T ", "volt ", "test "};


struct hisi_lpm3_mbox_work {
	struct hisi_mbox_task *tx_task;
	struct work_struct work;
	struct hisi_mbox *p;
};

static int hisi_mbox_mbox_notifier(struct notifier_block *nb, unsigned long len, void *msg)
{
	unsigned int *_msg = (unsigned int *)msg;
	unsigned long i;

	if (display_mailbox_rec) {
		pr_info("%s: receive mail\n", MODULE_NAME);
		for (i = 0; i < len; i++)
			pr_info("msg[%d] = 0x%x\n", (int)i, _msg[i]);
	}

	return 0;
}

static void hisi_mbox_complete(struct hisi_mbox_task *tx_task)
{
	struct hisi_lpm3_mbox_work *dwork = NULL;
	int i;

	if (display_mailbox_rec) {
		pr_debug("%s: tx complete, tx error[%d], ack msg:\n", MODULE_NAME, tx_task->tx_error);
		for (i = 0; i < tx_task->ack_buffer_len; i++)
			pr_debug("msg[%d] = 0x%x\n", i, tx_task->ack_buffer[i]);
	}

	if (tx_task->tx_buffer)
		kfree((void *)tx_task->tx_buffer);
	tx_task->tx_buffer = NULL;

	dwork = (struct hisi_lpm3_mbox_work *)tx_task->context;
	if (dwork)
		dwork->tx_task = NULL;

	hisi_mbox_task_free(&tx_task);
	return;
}

static int hisi_lpm3_debugfs_show(struct seq_file *s, void *data)
{
	pr_debug("%s: %s\n", MODULE_NAME, __FUNCTION__);
	return 0;
}

static int hisi_lpm3_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, hisi_lpm3_debugfs_show, inode->i_private);
}

static ssize_t hisi_lpm3_debugfs_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	struct hisi_mbox_task *tx_task = NULL;
	char *debug_cmd = NULL;
	u32 result =0;
	u32 *msg;
	u32 len = 0;
	s32 ret, i, j;

	ret = 0;

	debug_cmd = kzalloc(sizeof(char) * count, GFP_KERNEL);
	if (!debug_cmd) {
		pr_err("%s: cannot allocate hisi lpm3 debugfs cmd.\n", MODULE_NAME);
		goto out;
	}

	msg = kzalloc(sizeof(u32) * 8, GFP_KERNEL);
	if (!msg) {
		pr_err("%s: cannot allocate msg space.\n", MODULE_NAME);
		goto mem_err;
	}

	memcpy(debug_cmd, buf, count - 1);

	debug_cmd[count - 1] = '\0';
	pr_info("%s: [cmd: %s[count: %d]]\n", MODULE_NAME, debug_cmd, (int)count);

	for (i = 0; i < MAX_OBJ; i++) {
		if (!strncmp(obj[i], debug_cmd + len, strlen(obj[i]))) {
			len += strlen(obj[i]);
			msg[0] |= (i << 16);
			break;
		}
	}
	if (i == MAX_OBJ ) {
		pr_err("error, no such obj\n");
		goto msg_err;
	}

	for (i = 0; i < MAX_TYPE; i++) {
		if (!strncmp(type[i], debug_cmd + len, strlen(type[i]))) {
			len += strlen(type[i]);
			msg[0] |= (i + 1);
			break;
		}
	}
	if (i == MAX_TYPE ) {
		pr_err("error, no such cmd type\n");
		goto msg_err;
	}

	for (i = 0; i < MAX_CMD; i++) {
		if (!strncmp(cmd[i], debug_cmd + len, strlen(cmd[i]))) {
			len += strlen(cmd[i]);
			msg[0] |= (i << 8);
			break;
		}
	}
	if (i == MAX_CMD ) {
		pr_err("error, no such cmd\n");
		goto msg_err;
	}

	len++;
	j = 1;
	while (len < count && j < 8) {
		for (i = len; i < count; i++) {
			if (debug_cmd[i] == ' ') {
				debug_cmd[i] = '\0';
				break;
			}
		}
		ret = kstrtou32(debug_cmd + len, 0, &result);
		if (ret != 0) {
			goto mem_err;
		}
		msg[j++] = result;
		len = i + 1;
	}

	for (i = 0; i < 8; i++)
		pr_info("0x%x\n",msg[i]);

	pr_info("%s: %s\n", MODULE_NAME, debug_cmd);

	tx_task = hisi_mbox_task_alloc(mbox,
				HISI_MAILBOX_RP_LPM3,
				msg,
				8,
				1,
				hisi_mbox_complete,
				NULL);

	ret=hisi_mbox_msg_send_async(mbox, tx_task);
        if(ret != 0) {
            /*just for pclint check*/
        }
msg_err:
mem_err:
	if (msg != NULL) {
    	kfree(msg);
	}
	if (debug_cmd != NULL) {
		kfree(debug_cmd);
	}

out:
	return count;
}

static const struct file_operations hisi_lpm3_debugfs_fops =
{
	.open		= hisi_lpm3_debugfs_open,
	.read		= seq_read,
	.write		= hisi_lpm3_debugfs_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init hisi_lpm3_init(void)
{
	int ret = 0;
	display_mailbox_rec = 0;

	nb = (struct notifier_block *)kzalloc(sizeof(struct notifier_block), GFP_KERNEL);
	if (!nb) {
		ret = -ENOMEM;
		goto err_alloc_nb;
	}

	nb_nor = (struct notifier_block *)kzalloc(sizeof(struct notifier_block), GFP_KERNEL);
	if (!nb_nor) {
		ret = -ENOMEM;
		goto err_alloc_nb_nor;
	}

	nb->next = NULL;
	nb->notifier_call = hisi_mbox_mbox_notifier;

	nb_nor->next = NULL;
	nb_nor->notifier_call = hisi_mbox_mbox_notifier;

	mbox = hisi_mbox_get(HISI_MAILBOX_RP_LPM3, nb);
	if (!mbox) {
		ret = -ENODEV;
		goto err_get_mbox;
	}

	mbox_nor = hisi_mbox_get(HISI_MAILBOX_RP_UNCERTAIN, nb_nor);
	if (!mbox_nor) {
		hisi_mbox_put(&mbox);
		ret = -ENODEV;
		goto err_get_mbox_nor;
	}
	lpm3_debug_dir = debugfs_create_dir("hisi_lpm3_debug", NULL);
	if (lpm3_debug_dir) {
		if (NULL == debugfs_create_file("lpm3_test", S_IRUSR, lpm3_debug_dir, NULL, &hisi_lpm3_debugfs_fops)) {
			debugfs_remove_recursive(lpm3_debug_dir);
			ret = -ENODEV;
			goto err_create_debugfs;
		}
	} else {
		ret = -ENODEV;
		goto err_create_debugfs;
	}

	return 0;

err_create_debugfs:
	hisi_mbox_put(&mbox_nor);
err_get_mbox_nor:
	hisi_mbox_put(&mbox);
err_get_mbox:
	kfree(nb_nor);
err_alloc_nb_nor:
	kfree(nb);
err_alloc_nb:
	return ret;
}
fs_initcall(hisi_lpm3_init);

static void __exit hisi_lpm3_exit(void)
{
	debugfs_remove_recursive(lpm3_debug_dir);
	hisi_mbox_put(&mbox_nor);
	hisi_mbox_put(&mbox);
	kfree(nb_nor);
	kfree(nb);
}
module_exit(hisi_lpm3_exit);

MODULE_AUTHOR("wangtao.jean@huawei.com>");
MODULE_DESCRIPTION("LOWPOWER M3 DEBUG DRIVER");
MODULE_LICENSE("GPL");
