/* bastet.h
 *
 * Bastet Head File.
 *
 * Copyright (C) 2014 Huawei Device Co.,Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _BASTET_H
#define _BASTET_H

#include <linux/netfilter/x_tables.h>

#include "bastet_dev.h"

struct bastet_sock {
	u8 bastet_sock_state;
	u8 bastet_timer_event;
	u8 user_ctrl;

	int fd;
	pid_t pid;

	unsigned long bastet_timeout;
	struct timer_list bastet_timer;
	unsigned long delay_sync_time_section;
	unsigned long last_sock_active_time_point;

	bool sync_retry;
	struct bst_sock_sync_prop *sync_p;

	struct sk_buff_head recv_queue;
	int recv_len;

	int need_repair;
};

bool bastet_sock_send_prepare(struct sock *sk);
bool bastet_sock_recv_prepare(struct sock *sk, struct sk_buff *skb);
void bastet_sock_release(struct sock *sk);
void bastet_delay_sock_sync_notify(struct sock *sk);
bool bastet_sock_repair_prepare_notify(struct sock *sk, int val);
unsigned char *bastet_save_uid_info(const char *table_name, struct xt_table_info *newinfo, int *buf_len);
void bastet_indicate_uid_info(unsigned char *buf, int buf_len);
void bastet_wait_traffic_flow(void);
void bastet_modem_reset_notify(void);

#endif  /* _BASTET_H */
