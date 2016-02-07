#ifndef __HISI_CLK_MAILBOX_H__
#define __HISI_CLK_MAILBOX_H__
#include <linux/hisi/hisi_mailbox.h>
#include <linux/completion.h>

#define	LPM3_CMD_LEN	2

#define AUTO_ACK		1
#define NO_AUTO_ACK		0

struct clk_mbox_context {
	struct completion complete;
	unsigned long freq;
};

struct hisi_mbox *hisi_clkmbox_get_mailbox(void);
int hisi_clkmbox_is_ready(void);
int hisi_clkmbox_send_msg(mbox_msg_t *msg, void *context, int autoack);
#endif
