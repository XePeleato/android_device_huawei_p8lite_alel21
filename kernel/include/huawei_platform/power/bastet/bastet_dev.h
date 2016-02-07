/*
 * Copyright (c) Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.
 *
 * File name: bastet_dev.h
 * Description: Provide kernel device information for bastet.
 * Author: Pengyu  ID: 00188486
 * Version: 0.1
 * Date: 2014/06/21
 *
 */

#ifndef _BASTET_DEV_H
#define _BASTET_DEV_H

#include <linux/ioctl.h>

#define BASTET_DEV_NAME					"/dev/bastet"

#define BST_IOC_MAXNR					20
#define BST_IOC_MAGIC					'k'
#define BST_IOC_SOCK_SYNC_START			_IOW(BST_IOC_MAGIC,  1, struct bst_set_sock_sync_delay)
#define BST_IOC_SOCK_SYNC_STOP			_IOW(BST_IOC_MAGIC,  2, struct bst_sock_id)
#define BST_IOC_SOCK_SYNC_SET			_IOW(BST_IOC_MAGIC,  3, struct bst_set_sock_sync_prop)
#define BST_IOC_SOCK_COMM_GET			_IOWR(BST_IOC_MAGIC, 4, struct bst_get_sock_comm_prop)
#define BST_IOC_SOCK_CLOSE_SET			_IOW(BST_IOC_MAGIC,  5, struct bst_sock_comm_prop)
#define BST_IOC_SOCK_STATE_GET			_IOWR(BST_IOC_MAGIC, 6, struct bst_get_bastet_sock_state)
#define BST_IOC_APPLY_LOCAL_PORT		_IOR(BST_IOC_MAGIC,  7, uint16_t)
#define BST_IOC_RELEASE_LOCAL_PORT		_IOW(BST_IOC_MAGIC,  8, uint16_t)
#define BST_IOC_SET_TRAFFIC_FLOW		_IOW(BST_IOC_MAGIC,  9, struct bst_traffic_flow_prop)
#define BST_IOC_GET_TIMESTAMP_INFO		_IOR(BST_IOC_MAGIC, 10, struct bst_timestamp_info)
#define BST_IOC_SET_NET_DEV_NAME		_IOW(BST_IOC_MAGIC, 11, struct bst_net_dev_name)
#ifdef CONFIG_HUAWEI_BASTET_COMM
#define BST_IOC_GET_MODEM_RAB_ID		_IOR(BST_IOC_MAGIC, 12, struct bst_modem_rab_id)
#endif

typedef enum {
	BST_SOCK_NOT_USED = 0,
	BST_SOCK_INVALID,
	BST_SOCK_UPDATING,
	BST_SOCK_VALID,
} bst_sock_state;

typedef enum {
	BST_IND_INVALID = 0,
	BST_IND_HISICOM,
	BST_IND_SOCK_SYNC_REQ,
	BST_IND_SOCK_SYNC_PROP,
	BST_IND_SOCK_CLOSED,
	BST_IND_MODEM_RESET,
	BST_IND_NETFILTER_SYNC_UID,
	BST_IND_TRAFFIC_FLOW_REQ,
} bst_ind_type;

struct bst_device_ind {
	uint32_t cons;
	bst_ind_type type;
	uint32_t len;
	uint8_t value[0];
};

struct bst_sock_id {
	int fd;
	pid_t pid;
};

struct bst_sock_sync_prop {
	uint32_t seq;
	uint32_t rcv_nxt;
	uint32_t snd_wnd;
	uint32_t ts_recent;
	int mss;
	uint32_t tx;
	uint32_t rx;
	uint16_t snd_wscale;
	uint16_t rcv_wscale;
	uint32_t ts_current;
	uint32_t ts_recent_tick;
};

struct bst_sock_comm_prop {
	uint32_t local_ip;
	uint32_t remote_ip;
	uint16_t local_port;
	uint16_t remote_port;
};

struct bst_get_sock_comm_prop {
	struct bst_sock_id guide;
	struct bst_sock_comm_prop comm_prop;
};

struct bst_set_sock_sync_delay {
	struct bst_sock_id guide;
	uint32_t hold_time;
};

struct bst_set_sock_sync_prop {
	struct bst_sock_comm_prop guide;
	struct bst_sock_sync_prop sync_prop;
};

struct bst_ind_sock_sync_prop {
	struct bst_sock_id guide;
	struct bst_sock_sync_prop sync_prop;
};

struct bst_get_bastet_sock_state {
	struct bst_sock_id guide;
	uint32_t sync_state;
};

struct bst_traffic_flow_prop {
	int fd;
	pid_t pid;
	uid_t uid;
	int protocol;
	uint32_t tx;
	uint32_t rx;
};

struct bst_traffic_flow_pkg {
	uint32_t cnt;
	uint8_t value[0];
};

struct bst_timestamp_info {
	uint32_t time_now;
	uint32_t time_freq;
};

struct bst_net_dev_name {
	char iface[IFNAMSIZ];
};

#ifdef CONFIG_HUAWEI_BASTET_COMM
struct bst_modem_rab_id {
	uint16_t modem_id;
	uint16_t rab_id;
};
#endif

#define BST_MAX_WRITE_PAYLOAD			(1024)
#define BST_MAX_READ_PAYLOAD			(BST_MAX_WRITE_PAYLOAD + sizeof(bst_device_ind))

#endif /* _BASTET_DEV_H */
