#include <linux/module.h>
#include <linux/init.h>
#include <linux/hisi/hisi_mailbox.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/suspend.h>
#include "hisi-clk-mailbox.h"

#define MAX_SEND_IPC_TRY	3
#define MAX_SEND_MSG_TRY	3

struct mailbox_clk {
	struct hisi_mbox *mbox;
};

struct mailbox_clk *mbox_clk;

static int mailbox_disable;

static int mailbox_pm_callback(struct notifier_block *nb,
			unsigned long action, void *ptr)
{

	switch (action) {

	case PM_SUSPEND_PREPARE:
		mailbox_disable = 1;
		break;

	case PM_POST_SUSPEND:
		mailbox_disable = 0;
		break;

	default:
		return NOTIFY_DONE;
	}

	return NOTIFY_OK;
}

static struct notifier_block mailbox_pm_notif_block = {
	.notifier_call = mailbox_pm_callback,
};



static void hisi_clkmbox_default_complete(struct hisi_mbox_task *tx_task)
{
	struct clk_mbox_context *context;

	if (tx_task->tx_error) {
		pr_err("[%s]: tx_task->tx_error = %d\n",
				__func__, tx_task->tx_error);
		goto err;
	}

	context = (struct clk_mbox_context *) tx_task->context;
	if (context) {
		if (tx_task->tx_buffer[0] == tx_task->ack_buffer[0]) {
			pr_info("compelte the context! freq = %d\n",
					tx_task->ack_buffer[1]);

			/*the unit of ack_buffer[1] is MHZ*/
			context->freq = tx_task->ack_buffer[1] * 1000000;
		} else {
			pr_info("compelte the context! freq = 0\n");
			context->freq = 0;
		}
		complete(&context->complete);
	}

err:
	/* reset the buffer */
	kfree(tx_task->tx_buffer);

	tx_task->tx_buffer = NULL;

	hisi_mbox_task_free(&tx_task);
}

struct hisi_mbox *hisi_clkmbox_get_mailbox(void)
{
	if (!mbox_clk) {
		pr_err("[%s] mbox_clk is not ready!\n", __func__);
		return NULL;
	} else {
		return mbox_clk->mbox;
	}
}

int hisi_clkmbox_is_ready(void)
{
	return  mbox_clk == NULL ? 0 : 1;
}

static int hisi_clkmbox_send_ipc(mbox_msg_t *msg, void *context, int autoack)
{
	struct hisi_mbox_task *tx_task;
	int loop = MAX_SEND_IPC_TRY;
	mbox_msg_t	*tx_msg;
	int ret, i;

	tx_msg = kzalloc(sizeof(mbox_msg_t) * LPM3_CMD_LEN, GFP_ATOMIC);
	if (!tx_msg) {
		pr_err("[%s] fail to alloc tx_msg!\n", __func__);
		ret = -ENOMEM;
		goto err;
	}

	/* construct cmd */
	for (i = 0; i < LPM3_CMD_LEN; i++) {
		tx_msg[i] = msg[i];
		pr_debug("[%s] tx_msg[i] = 0x%x\n",	__func__, tx_msg[i]);
	}


	tx_task = hisi_mbox_task_alloc(mbox_clk->mbox, HISI_MAILBOX_RP_LPM3,
							tx_msg, LPM3_CMD_LEN, autoack, hisi_clkmbox_default_complete, context);
	if (!tx_task) {
		pr_err("[%s] fail to alloc mbox task!\n", __func__);
		ret = -ENOMEM;
		goto err_task;
	}

	/* try again if failing to send */
	do {
		ret = hisi_mbox_msg_send_async(mbox_clk->mbox, tx_task);
		loop--;
	} while (ret == -ENOMEM && loop > 0);
	if (ret) {
		pr_err("[%s] fail to send mbox msg, ret = %d!\n",
				__func__, ret);
		goto err_snd_async;
	}

	return 0;

err_snd_async:
	hisi_mbox_task_free(&tx_task);
err_task:
	kfree(tx_msg);
	tx_msg = NULL;
err:
	return ret;
}

/*
 * Please pay attention to the following points:
 * (1) This function can be called from atomic and non-atomic context.
 * (2) Please check the return variable, as the mailbox may not ready.
 *
 */
int hisi_clkmbox_send_msg(mbox_msg_t *msg, void *context, int autoack)
{
	int ret = 0;
	int loop = MAX_SEND_MSG_TRY;

	if (mailbox_disable == 1)
		return 0;

	ret = hisi_clkmbox_is_ready();
	if (!ret) {
		pr_err("[%s]: mbox_clk is not initialized yet!\n", __func__);
		ret = -ENXIO;
		goto err;
	}

	/* try again when the mail box is full */
	do {
		ret = hisi_clkmbox_send_ipc(msg, context, autoack);
		loop--;
	} while (ret != 0 && loop > 0);
err:
	return ret;
}

static int __init hisi_clkmbox_init(void)
{
	int ret = 0;

	mbox_clk = kzalloc(sizeof(*mbox_clk), GFP_KERNEL);
	if (!mbox_clk) {
		pr_err("[%s] fail to alloc mbox_clk!\n", __func__);
		ret = -ENOMEM;
		goto err;
	}

	/* get mailbox to communicate with lpm3 */
	mbox_clk->mbox = hisi_mbox_get(HISI_MAILBOX_RP_LPM3, NULL);
	if (!mbox_clk->mbox) {
		pr_err("[%s] fail to get mbox!\n", __func__);
		ret = -ENODEV;
		goto err_mbox;
	}

	mailbox_disable = 0;
	register_pm_notifier(&mailbox_pm_notif_block);

	return ret;

err_mbox:
	kfree(mbox_clk);
	mbox_clk = NULL;
err:
	return ret;
}

static void __exit hisi_clkmbox_exit(void)
{
	if (mbox_clk) {
		hisi_mbox_put(&mbox_clk->mbox);
		kfree(mbox_clk);
		mbox_clk = NULL;
	}
	unregister_pm_notifier(&mailbox_pm_notif_block);
}

fs_initcall(hisi_clkmbox_init);
module_exit(hisi_clkmbox_exit);
