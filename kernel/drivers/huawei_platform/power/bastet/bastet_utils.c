/* bastet_utils.c
 *
 * Provide Bastet utilities.
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

#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/file.h>
#include <linux/inetdevice.h>
#include <linux/of.h>
#include <linux/wakelock.h>
#include <uapi/linux/if.h>
#include <asm/atomic.h>
#include <net/tcp.h>
#include <huawei_platform/power/bastet/bastet_utils.h>

#define BASTET_WAKE_LOCK				"bastet_wl"
#define BASTET_DEFAULT_NET_DEV			"rmnet0"

#define MIN_UID				0				/* minimum uid number */
#define MAX_UID				65535			/* maximum uid number */

static struct wake_lock wl_bastet;
static bool bastet_cfg_en = false;
bool bastet_dev_en = false;
char cur_netdev_name[IFNAMSIZ] = BASTET_DEFAULT_NET_DEV;

inline bool is_bastet_enabled(void)
{
	return bastet_cfg_en && bastet_dev_en;
}

void bastet_modem_reset_notify(void)
{
	if (!is_bastet_enabled()) {
		return;
	}
	post_indicate_packet(BST_IND_MODEM_RESET, NULL, 0);
}

static struct file *fget_by_pid(unsigned int fd, pid_t pid)
{
	struct file *file;
	struct task_struct *task;
	struct files_struct *files;

	task = find_task_by_vpid(pid);
	if (!task)
		return NULL;

	files = task->files;

	/*process is removed, task isn't null, but files is null*/
	if (NULL == files) {
		return NULL;
	}

	rcu_read_lock();
	file = fcheck_files(files, fd);
	if (file) {
		/* File object ref couldn't be taken */
		if (file->f_mode & FMODE_PATH ||
			!atomic_long_inc_not_zero(&file->f_count))
			file = NULL;
	}
	rcu_read_unlock();

	return file;
}

struct socket *sockfd_lookup_by_fd_pid(int fd, pid_t pid, int *err)
{
	struct file *file;
	struct socket *sock;

	file = fget_by_pid(fd, pid);
	if (!file) {
		*err = -EBADF;
		return NULL;
	}

	sock = sock_from_file(file, err);
	if (!sock)
		fput(file);

	return sock;
}

/*
 * Get sock by fd and pid.
 */
struct sock *get_sock_by_fd_pid(int fd, pid_t pid)
{
	int err;
	struct socket *sock;
	struct sock *sk;

	sock = sockfd_lookup_by_fd_pid(fd, pid, &err);
	if (NULL == sock) {
		return NULL;
	}

	sk = sock->sk;
	if (NULL != sk) {
		sock_hold(sk);
	}
	sockfd_put(sock);

	return sk;
}

/*
 * Get sock by ip and port.
 */
struct sock *get_sock_by_comm_prop(struct bst_sock_comm_prop* guide)
{
	struct net *net;
	struct net_device* dev;

	dev = dev_get_by_name(&init_net, cur_netdev_name);
	if(NULL == dev) {
		return NULL;
	}

	net = dev_net(dev);

	return __inet_lookup_established(net, &tcp_hashinfo, guide->remote_ip, guide->remote_port,
									guide->local_ip, ntohs(guide->local_port),dev->ifindex);
}

/*
 * Get two new continuous port, read inet_csk_get_port in inet_connection_sock.c for reference.
 * return the first port.
 */
int bind_local_ports(u16 *local_port)
{
	struct sock *sk, *exsk;
	struct inet_hashinfo *hashinfo = &tcp_hashinfo;
	struct inet_bind_hashbucket *head, *exhead;
	struct inet_bind_bucket *tb, *extb;
	struct net *net;
	struct net_device* dev;
	int ret = 1;
	int remaining, rover, exrover, low, high;

	dev = dev_get_by_name(&init_net, cur_netdev_name);
	if(NULL == dev) {
		*local_port = 0;
		return ret;
	}

	net = dev_net(dev);

	local_bh_disable();

	inet_get_local_port_range(&low, &high);
	remaining = (high - low) + 1;
	rover = net_random() % remaining + low;
	exrover = rover + 1;

	do {
		if (inet_is_reserved_local_port(rover)
			&& inet_is_reserved_local_port(exrover))
			goto next_nolock;
		head = &hashinfo->bhash[inet_bhashfn(net, rover,
					hashinfo->bhash_size)];
		spin_lock(&head->lock);
		inet_bind_bucket_for_each(tb, &head->chain) {
			if (net_eq(ib_net(tb), net) && tb->port == rover) {
				goto next_head;
			}
		}

		exhead = &hashinfo->bhash[inet_bhashfn(net, exrover,
					hashinfo->bhash_size)];
		spin_lock(&exhead->lock);
		inet_bind_bucket_for_each(extb, &exhead->chain) {
			if (net_eq(ib_net(extb), net) && extb->port == exrover) {
				goto next_exhead;
			}
		}
		break;
	next_exhead:
		spin_unlock(&exhead->lock);
	next_head:
		spin_unlock(&head->lock);
	next_nolock:
		if (++exrover > high)
			rover = low;
	} while (--remaining > 1); /* leave space for two ports */
	/* If remaining port is less than 2, goto fail */
	if (remaining <= 1) {
		goto fail;
	}

	tb = NULL;
	extb = NULL;

	if (!tb && (tb = inet_bind_bucket_create(hashinfo->bind_bucket_cachep,
				net, head, rover)) == NULL)
		goto fail_unlock;
	tb->fastreuse = 0;

	if (!extb && (extb = inet_bind_bucket_create(hashinfo->bind_bucket_cachep,
				net, exhead, exrover)) == NULL) {
		inet_bind_bucket_destroy(hashinfo->bind_bucket_cachep, tb);
		goto fail_unlock;
	}
	extb->fastreuse = 0;

	sk = sk_alloc(net, PF_INET, GFP_KERNEL, &tcp_prot);
	if (sk == NULL) {
		inet_bind_bucket_destroy(hashinfo->bind_bucket_cachep, tb);
		inet_bind_bucket_destroy(hashinfo->bind_bucket_cachep, extb);
		goto fail_unlock;
	}

	exsk = sk_alloc(net, PF_INET, GFP_KERNEL, &tcp_prot);
	if (exsk == NULL) {
		inet_bind_bucket_destroy(hashinfo->bind_bucket_cachep, tb);
		inet_bind_bucket_destroy(hashinfo->bind_bucket_cachep, extb);
		sk_free(sk);
		goto fail_unlock;
	}

	inet_bind_hash(sk, tb, rover);
	inet_bind_hash(exsk, extb, exrover);

	ret = 0;
	*local_port = rover;
fail_unlock:
	spin_unlock(&exhead->lock);
	spin_unlock(&head->lock);
fail:
	local_bh_enable();

	BASTET_LOGI("port: %d", rover);

	return ret;
}

/*
 * Release a port, read inet_put_port in inet_hashtables.c for reference.
 */
int unbind_local_ports(u16 local_port)
{
	struct sock *sk, *exsk;
	struct inet_hashinfo *hashinfo = &tcp_hashinfo;
	int bhash;
	struct inet_bind_hashbucket *head, *exhead;
	struct inet_bind_bucket *tb, *extb;
	struct net *net;
	struct net_device* dev;
	int port, ex_port;
	int find_port = 0, ex_find_port = 0;
	int ret = 1;

	BASTET_LOGI("port: %d", local_port);

	port = local_port;
	ex_port = local_port+1;

	dev = dev_get_by_name(&init_net, cur_netdev_name);
	if(NULL == dev)
		return ret;
	net = dev_net(dev);

	local_bh_disable();

	bhash = inet_bhashfn(net, port, hashinfo->bhash_size);
	head = &hashinfo->bhash[bhash];

	spin_lock(&head->lock);

	inet_bind_bucket_for_each(tb, &head->chain) {
		if (net_eq(ib_net(tb), net) && tb->port == port) {
			find_port = 1;
			break;
		}
	}

	bhash = inet_bhashfn(net, ex_port, hashinfo->bhash_size);
	exhead = &hashinfo->bhash[bhash];

	spin_lock(&exhead->lock);

	inet_bind_bucket_for_each(extb, &exhead->chain) {
		if (net_eq(ib_net(extb), net) && extb->port == ex_port) {
			ex_find_port = 1;
			break;
		}
	}

	if (find_port && ex_find_port) {
		atomic_dec(&hashinfo->bsockets);
		tb->num_owners--;
		sk = hlist_entry(tb->owners.first, struct sock, sk_bind_node);
		__sk_del_bind_node(sk);
		if (NULL != sk) {
			sk_free(sk); 
		}

		inet_bind_bucket_destroy(hashinfo->bind_bucket_cachep, tb);

		atomic_dec(&hashinfo->bsockets);
		extb->num_owners--;
		exsk = hlist_entry(extb->owners.first, struct sock, sk_bind_node);
		__sk_del_bind_node(exsk);
		if (NULL != exsk) {
			sk_free(exsk); 
		}

		inet_bind_bucket_destroy(hashinfo->bind_bucket_cachep, extb);

	} else {
		BASTET_LOGE("port: %u ex_port : %u not find in tcp_hashinfo!", port, ex_port);
	}

	spin_unlock(&exhead->lock);
	spin_unlock(&head->lock);

	local_bh_enable();

	return 0;
}

void bastet_wakelock_acquire(void)
{
	wake_lock(&wl_bastet);
}

void bastet_wakelock_acquire_timeout(long timeout)
{
	if (wake_lock_active(&wl_bastet)) {
		wake_unlock(&wl_bastet);
	}
	wake_lock_timeout(&wl_bastet, timeout);
}

void bastet_wakelock_release(void)
{
	wake_unlock(&wl_bastet);
}

inline bool is_uid_valid(__u32 uid)
{
	return uid >= MIN_UID && uid <= MAX_UID;
}

int set_current_net_device_name(char *iface)
{
	if (NULL == iface) {
		return -EINVAL;
	}

	memcpy(cur_netdev_name, iface, IFNAMSIZ);
	return 0;
}

void bastet_utils_init(void)
{
	BASTET_LOGI("bastet feature enabled");
	bastet_cfg_en = true;
	wake_lock_init(&wl_bastet, WAKE_LOCK_SUSPEND, BASTET_WAKE_LOCK);
}

void bastet_utils_exit(void)
{
	wake_lock_destroy(&wl_bastet);
}
