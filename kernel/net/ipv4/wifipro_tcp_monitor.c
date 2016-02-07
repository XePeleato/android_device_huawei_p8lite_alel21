

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/completion.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/compiler.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/skbuff.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <uapi/linux/netlink.h>
#include <linux/kthread.h>
#include <huawei_platform/log/hw_log.h>
#include <linux/string.h>
#include <net/tcp.h>
#include "wifipro_tcp_monitor.h"

#ifndef CONFIG_HW_WIFIPRO
#undef CONFIG_HW_WIFIPRO_PROC
#endif

#ifdef CONFIG_HW_WIFIPRO_PROC
#include <net/net_namespace.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/rtc.h>
#endif

MODULE_LICENSE("GPL");

/************************************************************
                    MOCRO   DEFINES
*************************************************************/
DEFINE_MUTEX(wifipro_nl_receive_sem);
DEFINE_MUTEX(wifipro_nl_send_sem);
DEFINE_MUTEX(wifipro_google_sock_sem);
DEFINE_MUTEX(wifipro_rtt_sem);
DEFINE_MUTEX(wifipro_congestion_sem);
DEFINE_MUTEX(wifipro_trigger_sock_sem);
DEFINE_MUTEX(wifipro_tcp_trigger_inf_sem);

static DEFINE_SPINLOCK(wifipro_google_sock_spin);

#define LINK_UNKNOWN                0
#define LINK_POOR                   1
#define LINK_MAYBE_POOR             2
#define LINK_MAYBE_GOOD             3
#define LINK_GOOD                   4

/********************************
    netlink variables for
    communicate between kernel and apk
*********************************/
enum wifipro_KnlMsgType {
    NETLINK_WIFIPRO_START_MONITOR = 0,
    NETLINK_WIFIPRO_GET_MSG,
    NETLINK_WIFIPRO_STOP_MONITOR,
    NETLINK_WIFIPRO_PAUSE_MONITOR,
    NETLINK_WIFIPRO_CONTINUE_MONITOR,
    NETLINK_WIFIPRO_NOTIFY_MCC
};

enum wifipro_msg_from {
    WIFIPRO_APP_QUERY = 0,
    WIFIPRO_KNL_NOTIFY
};

struct wifipro_nl_packet_msg {
    unsigned int msg_from;  //kernel notify or app query
	unsigned int rtt;   //average rtt within last 3 seconds
	unsigned int rtt_pkts;  //packet counts of rtt calculation
	unsigned int rtt_when;  //when recorded rtt
	unsigned int congestion;    //congestion stat
	unsigned int cong_when;     //when recorded congestion
	unsigned int tcp_quality;  //tcp quality
	unsigned int tcp_tx_pkts;
	unsigned int tcp_rx_pkts;
	unsigned int tcp_retrans_pkts;
};

#ifdef CONFIG_HW_WIFIPRO_PROC
static const struct snmp_mib wifipro_snmp_tcp_list[] = {
	SNMP_MIB_ITEM("InSegs", WIFIPRO_TCP_MIB_INSEGS),
	SNMP_MIB_ITEM("OutSegs", WIFIPRO_TCP_MIB_OUTSEGS),
	SNMP_MIB_ITEM("RetransSegs", WIFIPRO_TCP_MIB_RETRANSSEGS),
	SNMP_MIB_ITEM("InErrs", WIFIPRO_TCP_MIB_INERRS),
	SNMP_MIB_ITEM("OutRsts", WIFIPRO_TCP_MIB_OUTRSTS),
	SNMP_MIB_ITEM("ACKs", WIFIPRO_TCP_MIB_ACKS),
	SNMP_MIB_SENTINEL
};
#endif

static int wifipro_tcp_monitor_init();
static void wifipro_tcp_monitor_deinit();

static unsigned int g_user_space_pid = 0;
static bool is_wifipro_running = true;
static bool is_delayed_work_handling = false;

static struct sock *g_wifipro_nlfd = NULL;
static struct wifipro_tcp_monitor_inf *wifipro_tcp_trigger_inf = NULL;
static struct delayed_work wifipro_tcp_monitor_work;
static struct work_struct wifipro_tcp_retrans_work;

bool is_wifipro_on = false;
bool is_mcc_china = true;
unsigned int wifipro_rtt_average = 0;
unsigned int wifipro_rtt_calc_pkg = 0;
unsigned int wifipro_log_level = WIFIPRO_INFO;
unsigned long wifipro_when_recorded_rtt = 0;

wifipro_trigger_sock_t *wifipro_trigger_sock = NULL;
wifipro_retrans_sock_t *wifipro_retrans_sock = NULL;
wifipro_rtt_second_stat_t *wifipro_rtt_second_stat_head = NULL;
wifipro_g_sock_bl_t *wifipro_g_sock_head = NULL;
wifipro_cong_sock_t *wifipro_congestion_stat = NULL;


/************************************************************
                    FUNCTION  DEFINES
*************************************************************/
char *wifipro_ntoa(int addr)
{
    int len = 0;
    static char buf[18];
    memset(buf, 0, sizeof(buf));

    len += sprintf(buf+len, "%d.", (addr>>24) & 0xff);
    len += sprintf(buf+len, "%d.", (addr>>16) & 0xff);
    len += sprintf(buf+len, "%d.", (addr>>8) & 0xff);
    len += sprintf(buf+len, "%d", addr & 0xff);

    return buf;
}

void wifipro_update_tcp_statistics(struct sock *sk, int mib_type, int count)
{
	if(!is_wifipro_on){
	    return;
	}

    struct inet_sock *inet = inet_sk(sk);
    if(NULL == inet){
        return;
    }

    unsigned int dest_addr = htonl(inet->inet_daddr);
    unsigned int dest_port = htons(inet->inet_dport);

    if(!wifipro_is_not_local_or_lan_sock(dest_addr)){
        return;
    }

    if(WIFIPRO_TCP_MIB_OUTSEGS == mib_type){
        WIFIPRO_TCP_ADD_STATS(&init_net, mib_type, count);
        WIFIPRO_VERBOSE("Outsegs to %s increase", wifipro_ntoa(dest_addr));
        if(wifipro_is_trigger_sock(dest_addr, dest_port)) {
            wifipro_trigger_sock->OutSegs += count;
            WIFIPRO_VERBOSE("%s:%d  trigger socket OutSegs = %d",
                wifipro_ntoa(dest_addr), dest_port, wifipro_trigger_sock->OutSegs);
        }
    }else if(WIFIPRO_TCP_MIB_INSEGS == mib_type){
        WIFIPRO_TCP_INC_STATS_BH(&init_net, mib_type);
        WIFIPRO_VERBOSE("Insegs from %s increase", wifipro_ntoa(dest_addr));
    }

    return;
}

wifipro_g_sock_bl_t *wifipro_google_sock_add(wifipro_g_sock_bl_t *head, unsigned int dst_addr, char *proc_name, unsigned int pid)
{
    wifipro_g_sock_bl_t *backlist = NULL;
    wifipro_g_sock_bl_t *curr = NULL;
    wifipro_g_sock_bl_t *prev = NULL;
    unsigned int len = strnlen(proc_name, WIFIPRO_MAX_PROC_NAME -1);

    //mutex_lock(&wifipro_google_sock_sem);
    curr = head;
    prev = head;
    if(NULL == head){
        backlist = kmalloc(sizeof(wifipro_g_sock_bl_t), GFP_KERNEL);
        if(NULL != backlist) {
            backlist->owner_count = 1;
            backlist->dst_addr = dst_addr;
            backlist->pid = pid;
            strncpy(backlist->proc_name, proc_name, len);
            backlist->proc_name[len] = '\0';
            backlist->next = NULL;
            head = backlist;
        }else{
            WIFIPRO_ERROR("kmalloc fail!");
        }
        goto end;
    }else{
        while(curr){
            if(curr->dst_addr == dst_addr){    //it exist
                curr->owner_count++;
                goto end;
            }else{
                prev = curr;
                curr = curr->next;
            }
        }

        /*it dosen't exist, prev point to the tail*/
        backlist = kmalloc(sizeof(wifipro_g_sock_bl_t), GFP_KERNEL);
        if(NULL != backlist){
            backlist->owner_count = 1;
            backlist->dst_addr = dst_addr;
            backlist->pid = pid;
            strncpy(backlist->proc_name, proc_name, len);
            backlist->proc_name[len] = '\0';
            backlist->next = NULL;
            prev->next = backlist;
        }else{
            WIFIPRO_ERROR("kmalloc fail!");
            goto end;
        }
    }

end:
    //mutex_unlock(&wifipro_google_sock_sem);
	return head;
}

static bool wifipro_is_in_google_sock_list(wifipro_g_sock_bl_t *head, unsigned int dst_addr)
{
	wifipro_g_sock_bl_t *curr_bl;
	bool ret = false;

    if(!is_mcc_china){
        return false;
    }


	spin_lock(&wifipro_google_sock_spin);
	curr_bl = head;
	if(head == NULL){
		ret = false;
		goto end;
	}else{
		while(curr_bl){
			if(curr_bl->dst_addr == dst_addr) {      //it's exist
			    WIFIPRO_VERBOSE("find the backlist:%s", wifipro_ntoa(curr_bl->dst_addr));
                ret = true;
                goto end;
			}else{
			    WIFIPRO_VERBOSE("current backlist:%s", wifipro_ntoa(curr_bl->dst_addr));
                curr_bl = curr_bl->next;
			}
        }
	}

end:
    spin_unlock(&wifipro_google_sock_spin);
	return ret;
}

static wifipro_g_sock_bl_t *wifipro_hendle_google_sock_del(wifipro_g_sock_bl_t *head, unsigned int dest_addr)
{
	if(NULL == head)
		return NULL;

	wifipro_g_sock_bl_t *prev_bl;
	wifipro_g_sock_bl_t *curr_bl;

    //spin_lock(&wifipro_google_sock_spin);
	if(head->dst_addr == dest_addr){    //it's head;
		curr_bl = head->next;
		head->owner_count--;
		if(!head->owner_count){
		    kfree(head);
		}
        WIFIPRO_DEBUG("del a google sock:%s", wifipro_ntoa(dest_addr));
        head = curr_bl;
		goto end;
	}

	prev_bl = head;
	curr_bl = head->next;
	while(curr_bl) {
		if(curr_bl->dst_addr == dest_addr) {
			prev_bl->next = curr_bl->next;
			curr_bl->owner_count--;
            if(!curr_bl->owner_count){
			    kfree(curr_bl);
			}
            WIFIPRO_DEBUG("del a google sock:%s", wifipro_ntoa(dest_addr));
            goto end;
		}
		prev_bl = curr_bl;
		curr_bl = curr_bl->next;
	}

end:
	//spin_unlock(&wifipro_google_sock_spin);
	return head;
}

void wifipro_google_sock_del(unsigned int dest_addr)
{
    spin_lock(&wifipro_google_sock_spin);
    wifipro_g_sock_head = wifipro_hendle_google_sock_del(wifipro_g_sock_head, dest_addr);
    spin_unlock(&wifipro_google_sock_spin);
}

static void wifipro_google_sock_free(wifipro_g_sock_bl_t *head)
{
    wifipro_g_sock_bl_t *curr_bl;
    wifipro_g_sock_bl_t *prev_bl;

    spin_lock(&wifipro_google_sock_spin);
	if(NULL == head)
		return;

    curr_bl = head;
	while(curr_bl){
		prev_bl = curr_bl;
		curr_bl = curr_bl->next;

		kfree(prev_bl);
	}
    spin_unlock(&wifipro_google_sock_spin);
	return;
}

static int wifipro_get_proc_name(struct task_struct *task, char *buffer)
{
	int res = 0;
	unsigned int len;
	struct mm_struct *mm = get_task_mm(task);
	if (!mm)
		goto out;
	if (!mm->arg_end)
		goto out_mm;

    len = mm->arg_end - mm->arg_start;

	if (len > PAGE_SIZE)
		len = PAGE_SIZE;

	res = access_process_vm(task, mm->arg_start, buffer, len, 0);

	// If the nul at the end of args has been overwritten, then
	// assume application is using setproctitle(3).
	if (res > 0 && buffer[res-1] != '\0' && len < PAGE_SIZE) {
		len = strnlen(buffer, res);
		if (len < res) {
		    res = len;
		} else {
			len = mm->env_end - mm->env_start;
			if (len > PAGE_SIZE - res)
				len = PAGE_SIZE - res;
			res += access_process_vm(task, mm->env_start, buffer+res, len, 0);
			res = strnlen(buffer, res);
		}
	}
out_mm:
	mmput(mm);
out:
	return res;
}

bool wifipro_is_google_sock(struct task_struct *task, unsigned int dest_addr)
{
    static char proc_name[PAGE_SIZE];
    int ret = 0;

    if(task){
        ret = wifipro_get_proc_name(task, proc_name);
        if(ret > 0){
            if(strstr(proc_name, "oogle")){ //google or Google
                spin_lock(&wifipro_google_sock_spin);
                wifipro_g_sock_head = wifipro_google_sock_add(wifipro_g_sock_head, dest_addr, proc_name, task->pid);
                spin_unlock(&wifipro_google_sock_spin);
                WIFIPRO_DEBUG("find a process name %s match google. The pid = %d", proc_name, task->pid);
                return true;
            }else if(strstr(proc_name, "system_server")){ //system_server will setup google socket
                spin_lock(&wifipro_google_sock_spin);
                wifipro_g_sock_head = wifipro_google_sock_add(wifipro_g_sock_head, dest_addr, proc_name, task->pid);
                spin_unlock(&wifipro_google_sock_spin);
                WIFIPRO_DEBUG("find a process name %s match system_server. ip is %s", proc_name, wifipro_ntoa(dest_addr));
                return true;
            }else{
                WIFIPRO_DEBUG("the process name %s is doesn't match google", proc_name);
                return false;
            }
        }
    }else{
        WIFIPRO_WARNING("Task is NULL");
    }

    return false;
}

static void wifipro_rtt_free(wifipro_rtt_second_stat_t *head)
{
	if(NULL == head)
		return;

    int i = 0;
    wifipro_rtt_second_stat_t *curr_bl;
    wifipro_rtt_second_stat_t *prev_bl;

    mutex_lock(&wifipro_rtt_sem);
    curr_bl = head;
	while(curr_bl && i < WIFIPRO_RTT_RECORD_SECONDS){
		prev_bl = curr_bl;
		curr_bl = curr_bl->next;

		kfree(prev_bl);
		i++;
	}
	mutex_unlock(&wifipro_rtt_sem);
}

bool wifipro_is_trigger_sock(unsigned int dest_addr, unsigned int dest_port)
{
    bool ret = false;
    if(is_delayed_work_handling && wifipro_trigger_sock->dst_addr == dest_addr && wifipro_trigger_sock->dst_port == dest_port){
        ret = true;
        goto end;
    }else{
        ret = false;
        goto end;
    }

end:
    return ret;
}

static bool wifipro_is_cong_occured(wifipro_cong_sock_t *src, unsigned int offset)
{
    int ret = false;
    mutex_lock(&wifipro_congestion_sem);
    if(src){
        wifipro_cong_sock_t *dst = src + offset;
        if(time_after(dst->when + WIFIPRO_MONITOR_DELAY, jiffies)){
            ret = true;
            goto end;
        }else{
            ret = false;
            goto end;
        }
    }

end:
    mutex_unlock(&wifipro_congestion_sem);
    return ret;
}

static void wifipro_set_cong_stat(unsigned int dest_addr, unsigned int dest_port, wifipro_cong_sock_t *src, unsigned int offset)
{
    if(!wifipro_congestion_stat){
        return;
    }
    wifipro_cong_sock_t *dst = src + offset;
    dst->amount++;
    dst->dst_addr = dest_addr;
    dst->dst_port = dest_port;
    dst->when = jiffies;
}

static void wifipro_tcp_retrans_work_handler(struct work_struct *work)
{
    unsigned int dest_addr = wifipro_retrans_sock->dst_addr;
    unsigned int dest_port = wifipro_retrans_sock->dst_port;
    unsigned int src_addr = wifipro_retrans_sock->src_addr;
    unsigned int src_port = wifipro_retrans_sock->src_port;

    if(wifipro_is_in_google_sock_list(wifipro_g_sock_head, dest_addr)){ //google server socket
        WIFIPRO_TCP_DEC_STATS(wifipro_retrans_sock->net, WIFIPRO_TCP_MIB_RETRANSSEGS);
        return 0;
    }

    /* record retrans before wifipro_tcp_monitor_work run */
	if(is_delayed_work_handling){
	    mutex_lock(&wifipro_trigger_sock_sem);
	    if(wifipro_trigger_sock->dst_addr == dest_addr && wifipro_trigger_sock->dst_port == dest_port) {
            wifipro_trigger_sock->retransmits++;
            WIFIPRO_DEBUG("%s:%d  trigger socket retransmit = %d",
                wifipro_ntoa(dest_addr), dest_port, wifipro_trigger_sock->retransmits);
        }else{
            wifipro_trigger_sock->other_sock_retrans++;
	    }
	    mutex_unlock(&wifipro_trigger_sock_sem);
	}

    //int ret = 0;
    if(is_wifipro_running && !is_delayed_work_handling
        && wifipro_retrans_sock->icsk_rto >= WIFIPRO_RTO_THRESHOLD){
        is_delayed_work_handling = true;
        mutex_lock(&wifipro_tcp_trigger_inf_sem);
        if(wifipro_tcp_trigger_inf){
            wifipro_tcp_trigger_inf->dst_addr = dest_addr ;
            wifipro_tcp_trigger_inf->dst_port = dest_port;
            wifipro_tcp_trigger_inf->src_addr = src_addr;
            wifipro_tcp_trigger_inf->src_port = src_port;

            wifipro_tcp_trigger_inf->retransmits = wifipro_retrans_sock->retrans;
            wifipro_tcp_trigger_inf->sock_state = wifipro_retrans_sock->sock_state;
            wifipro_tcp_trigger_inf->InSegs = snmp_fold_field((void __percpu **)(wifipro_retrans_sock->net)->mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_INSEGS);
            wifipro_tcp_trigger_inf->OutSegs = snmp_fold_field((void __percpu **)(wifipro_retrans_sock->net)->mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_OUTSEGS);
            wifipro_tcp_trigger_inf->RetransSegs = snmp_fold_field((void __percpu **)(wifipro_retrans_sock->net)->mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_RETRANSSEGS);
            wifipro_tcp_trigger_inf->InErrs = snmp_fold_field((void __percpu **)(wifipro_retrans_sock->net)->mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_INERRS);
        }
        mutex_unlock(&wifipro_tcp_trigger_inf_sem);

        mutex_lock(&wifipro_trigger_sock_sem);
        if(wifipro_trigger_sock){
            memset(wifipro_trigger_sock, 0, sizeof(wifipro_trigger_sock_t));
            wifipro_trigger_sock->dst_addr = dest_addr;
            wifipro_trigger_sock->dst_port = dest_port;
        }
        mutex_unlock(&wifipro_trigger_sock_sem);

        if(wifipro_log_level >= WIFIPRO_DEBUG){
            char printk_buf[2048];
            int buf_len = 0;

            char socket_state[20];
            switch(wifipro_retrans_sock->sock_state){
                case TCP_ESTABLISHED:
                    strncpy(socket_state, "Established", 12);
                    break;

                case TCP_SYN_SENT:
                    strncpy(socket_state, "SYN sent", 9);
                    break;

                case TCP_SYN_RECV:
                    strncpy(socket_state, "SYN recv", 9);
                    break;

                default:
                    strncpy(socket_state, "Other State", 12);
            }

            if(wifipro_tcp_trigger_inf){
                buf_len += sprintf(printk_buf + buf_len, "\n===================wifipro_tcp_trigger_inf begin======================\n");
                buf_len += sprintf(printk_buf + buf_len, "\ticsk_retransmits = %d\n", wifipro_tcp_trigger_inf->retransmits);
                buf_len += sprintf(printk_buf + buf_len, "\tsock_state= %lu\n", wifipro_tcp_trigger_inf->sock_state);
                buf_len += sprintf(printk_buf + buf_len, "\tCurrEstab = %lu\n", wifipro_tcp_trigger_inf->CurrEstab);
                buf_len += sprintf(printk_buf + buf_len, "\tInSegs = %lu\n", wifipro_tcp_trigger_inf->InSegs);
                buf_len += sprintf(printk_buf + buf_len, "\tOutSegs = %lu\n", wifipro_tcp_trigger_inf->OutSegs);
                buf_len += sprintf(printk_buf + buf_len, "\tRetransSegs = %lu\n", wifipro_tcp_trigger_inf->RetransSegs);
                buf_len += sprintf(printk_buf + buf_len, "\tInErrs = %lu\n", wifipro_tcp_trigger_inf->InErrs);
            }
            buf_len += sprintf(printk_buf + buf_len, "\n\tTrigger sock is %s:%d  state is %s\n", wifipro_ntoa(dest_addr), dest_port, socket_state);
            buf_len += sprintf(printk_buf + buf_len, "===================wifipro_tcp_trigger_inf end======================\n");
            printk("%s", printk_buf);
        }

        schedule_delayed_work(&wifipro_tcp_monitor_work, WIFIPRO_MONITOR_DELAY);
    }
}

int wifipro_handle_retrans(struct sock *sk, struct inet_connection_sock *icsk)
{
    struct inet_sock *inet = inet_sk(sk);
    if(NULL == inet) {
        WIFIPRO_ERROR("GET NULL POINTER!");
        return -1;
	}

    unsigned int dest_addr = htonl(inet->inet_daddr);
    unsigned int dest_port = htons(inet->inet_dport);
    unsigned int src_addr = htons(inet->inet_saddr);
    unsigned int src_port = htons(inet->inet_sport);

	if(!wifipro_is_not_local_or_lan_sock(dest_addr)){
        return 0;
	}

    //if it's not local, LAN or google socket, record it.
    WIFIPRO_TCP_INC_STATS(sock_net(sk), WIFIPRO_TCP_MIB_RETRANSSEGS);

    WIFIPRO_DEBUG("%s:%d retrans=%d  sk_state=%d  rto=%d",
	    wifipro_ntoa(dest_addr), dest_port, icsk->icsk_retransmits, sk->sk_state, icsk->icsk_rto);

    if((1 << sk->sk_state) & (TCPF_SYN_SENT | TCPF_ESTABLISHED)){
        if(!schedule_work(&wifipro_tcp_retrans_work)){
            WIFIPRO_DEBUG("wifipro_tcp_retrans_work was already queued");
            return 0;
        }
    }else{
        return 0;
    }

    struct net *temp_net = sock_net(sk);
    if(NULL == temp_net){
        WIFIPRO_ERROR("GET net FAIL");
        return -1;
    }
    
    wifipro_retrans_sock->net = temp_net;
    wifipro_retrans_sock->dst_addr = dest_addr;
    wifipro_retrans_sock->dst_port = dest_port;
    wifipro_retrans_sock->src_addr = src_addr;
    wifipro_retrans_sock->src_port = src_port;
    wifipro_retrans_sock->retrans = icsk->icsk_retransmits;
    wifipro_retrans_sock->icsk_rto = icsk->icsk_rto;
    wifipro_retrans_sock->sock_state = sk->sk_state;
    WIFIPRO_DEBUG("wifipro_tcp_retrans_work queued successfully");
    return 0;
}

static int wifipro_tcp_monitor_send_msg(int pid, unsigned int msg_from, unsigned int quality)
{
    int ret;
    int size;
    int i;
    struct sk_buff *skb = NULL;
    struct nlmsghdr *nlh = NULL;
    struct wifipro_nl_packet_msg *packet = NULL;

    if (!pid || !g_wifipro_nlfd){
        WIFIPRO_ERROR("cannot notify event, pid = %d", pid);
        return -1;
    }

    mutex_lock(&wifipro_nl_send_sem);
    size = sizeof(struct wifipro_nl_packet_msg);
    skb = nlmsg_new(size, GFP_ATOMIC);
    if (!skb) {
        WIFIPRO_ERROR("alloc skb fail");
        ret = -1;
        goto end;
    }

    nlh = nlmsg_put(skb, 0, 0, NETLINK_WIFIPRO_GET_MSG, size, 0);
    if (!nlh) {
        kfree_skb(skb);
        skb = NULL;
        ret = -1;
        goto end;
    }

    packet = nlmsg_data(nlh);
    memset(packet, 0, sizeof(struct wifipro_nl_packet_msg));

    packet->msg_from = msg_from;
	packet->rtt = wifipro_rtt_average;
	packet->rtt_pkts = wifipro_rtt_calc_pkg;
	packet->rtt_when = (jiffies - wifipro_when_recorded_rtt)/HZ;
	mutex_lock(&wifipro_congestion_sem);
    if(NULL != wifipro_congestion_stat){
        wifipro_cong_sock_t *dst = NULL;
        for(i = TCP_CA_Disorder; i <= TCP_CA_Loss; i++){
            dst = wifipro_congestion_stat + i;
            if(time_after(dst->when + 10*HZ, jiffies)){
                packet->congestion |= 1 << i;
                if(dst->when > packet->cong_when){
                    packet->cong_when = dst->when;
                }
            }
        }
    }else{
        packet->congestion = 0;
        packet->cong_when = 0;
	}
	mutex_unlock(&wifipro_congestion_sem);
    packet->tcp_rx_pkts= snmp_fold_field((void __percpu **)init_net.mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_INSEGS);
    packet->tcp_tx_pkts = snmp_fold_field((void __percpu **)init_net.mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_OUTSEGS);
    packet->tcp_retrans_pkts= snmp_fold_field((void __percpu **)init_net.mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_RETRANSSEGS);
    packet->tcp_quality = quality;

    if(0 != pid){
        ret = netlink_unicast(g_wifipro_nlfd, skb, pid, MSG_DONTWAIT); //skb will be freed in netlink_unicast
    }

end:
    mutex_unlock(&wifipro_nl_send_sem);
    if(wifipro_log_level >= WIFIPRO_DEBUG && ret != -1){
        char printk_buf[2048];
        int buf_len = 0;
        buf_len += sprintf(printk_buf + buf_len, "\n\n@@@@@@@@@@@@@@@@@@@@@@@@@ Netlink struct @@@@@@@@@@@@@@@@@@@@@@@@@\n");
        buf_len += sprintf(printk_buf + buf_len, "%s:send a msg to wifipro pid=%d:\n",__func__, pid);
        buf_len += sprintf(printk_buf + buf_len, "rtt=%dms  packet=%d  when=%ds  ", packet->rtt, packet->rtt_pkts, (jiffies - packet->rtt_when)/HZ);
        buf_len += sprintf(printk_buf + buf_len, "congestion=0x%x  quality=%d", packet->congestion, packet->tcp_quality);
        buf_len += sprintf(printk_buf + buf_len, "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");
        printk("%s", printk_buf);
    }
    return ret;
}


static void wifipro_tcp_monitor_nl_receive(struct sk_buff *__skb)
{
    struct nlmsghdr *nlh;
    struct sk_buff *skb;
    skb = skb_get(__skb);

    mutex_lock(&wifipro_nl_receive_sem);
    if (skb->len >= NLMSG_HDRLEN) {
        nlh = nlmsg_hdr(skb);

        if ((nlh->nlmsg_len >= sizeof(struct nlmsghdr))&&
            (skb->len >= nlh->nlmsg_len)) {
            WIFIPRO_INFO("netlink receive a packet, nlmsg_type=%d",nlh->nlmsg_type);
            if (NETLINK_WIFIPRO_START_MONITOR == nlh->nlmsg_type) {
                int ret = wifipro_tcp_monitor_init();
                if(ret == 0){
                    is_wifipro_on = true;
                    g_user_space_pid = nlh->nlmsg_pid;
                    WIFIPRO_INFO("netlink start monitor: g_user_space_pid = %d",nlh->nlmsg_pid);
                }
                if(BETA_USER == nlh->nlmsg_flags){
                    WIFIPRO_INFO("BETA_USER: set DEBUG log level", nlh->nlmsg_pid);
                    wifipro_log_level = WIFIPRO_DEBUG;
                }
            } else if(NETLINK_WIFIPRO_STOP_MONITOR == nlh->nlmsg_type) {
                is_wifipro_on = false;
                g_user_space_pid = 0;
                wifipro_tcp_monitor_deinit();
                WIFIPRO_INFO("netlink stop monitor, sender pid = %d",nlh->nlmsg_pid);
            } else if(NETLINK_WIFIPRO_GET_MSG == nlh->nlmsg_type) {
                WIFIPRO_INFO("netlink send msg to %d", nlh->nlmsg_pid);
                wifipro_tcp_monitor_send_msg(nlh->nlmsg_pid, WIFIPRO_APP_QUERY, LINK_UNKNOWN);
            } else if(NETLINK_WIFIPRO_PAUSE_MONITOR == nlh->nlmsg_type) {
                is_wifipro_running = false;
                is_delayed_work_handling = false;
                cancel_delayed_work(&wifipro_tcp_monitor_work);
            } else if(NETLINK_WIFIPRO_CONTINUE_MONITOR == nlh->nlmsg_type) {
                is_wifipro_running = true;
            }
            else if(NETLINK_WIFIPRO_NOTIFY_MCC == nlh->nlmsg_type) {
                if(MCC_CHINA == nlh->nlmsg_flags){
                    WIFIPRO_INFO("MCC = 460", nlh->nlmsg_pid);
                    is_mcc_china = true;
                }else{
                    WIFIPRO_INFO("MCC != 460", nlh->nlmsg_pid);
                    is_mcc_china = false;
                }
            }
        }
    }
    mutex_unlock(&wifipro_nl_receive_sem);
}

//TODO: work should be cancled if net interface switch
static void wifipro_tcp_monitor_work_handler(struct work_struct *work)
{
    struct wifipro_tcp_monitor_inf wifipro_tcp_curr_inf;
    unsigned int retrans_rate = 0;
    unsigned int diff_OutSegs = 0;
    unsigned int Interval_OutSegs = 0;
    unsigned int Interval_RetransSegs = 0;
    unsigned int Interval_retrans_rate = 0;
    unsigned int Interval_InSegs = 0;
    unsigned int tcp_quality = LINK_UNKNOWN;

    /*current tcp mib information*/
    wifipro_tcp_curr_inf.InSegs = snmp_fold_field((void __percpu **)init_net.mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_INSEGS);
    wifipro_tcp_curr_inf.OutSegs = snmp_fold_field((void __percpu **)init_net.mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_OUTSEGS);
    wifipro_tcp_curr_inf.RetransSegs = snmp_fold_field((void __percpu **)init_net.mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_RETRANSSEGS);
    wifipro_tcp_curr_inf.InErrs = snmp_fold_field((void __percpu **)init_net.mib.wifipro_tcp_statistics, WIFIPRO_TCP_MIB_INERRS);

    mutex_lock(&wifipro_tcp_trigger_inf_sem);
    if(wifipro_tcp_trigger_inf){
        diff_OutSegs = wifipro_tcp_curr_inf.OutSegs - wifipro_tcp_trigger_inf->OutSegs;
        if(0 != diff_OutSegs){
            retrans_rate = (wifipro_tcp_curr_inf.RetransSegs - wifipro_tcp_trigger_inf->RetransSegs) * 100 / diff_OutSegs;
        }
    }
    mutex_unlock(&wifipro_tcp_trigger_inf_sem);

    /*tcp mib information between WIFIPRO_MONITOR_DELAY*/
    mutex_lock(&wifipro_tcp_trigger_inf_sem);
    mutex_lock(&wifipro_trigger_sock_sem);
    if(wifipro_tcp_trigger_inf && wifipro_trigger_sock){
        Interval_OutSegs = wifipro_tcp_curr_inf.OutSegs - wifipro_tcp_trigger_inf->OutSegs - wifipro_trigger_sock->OutSegs;
        Interval_RetransSegs = wifipro_tcp_curr_inf.RetransSegs - wifipro_tcp_trigger_inf->RetransSegs - wifipro_trigger_sock->RetransSegs;
    }
    mutex_unlock(&wifipro_trigger_sock_sem);
    mutex_unlock(&wifipro_tcp_trigger_inf_sem);
    if(0 != Interval_OutSegs){
        Interval_retrans_rate = Interval_RetransSegs * 100 / Interval_OutSegs;
    }
    mutex_lock(&wifipro_tcp_trigger_inf_sem);
    if(wifipro_tcp_trigger_inf){
        Interval_InSegs = wifipro_tcp_curr_inf.InSegs - wifipro_tcp_trigger_inf->InSegs;
    }
    mutex_unlock(&wifipro_tcp_trigger_inf_sem);

	/* uplink quality */
	if(Interval_InSegs >= 50){
        tcp_quality = LINK_GOOD;
    }else if(Interval_InSegs >= 20){
        tcp_quality = LINK_MAYBE_GOOD;
    }else if(wifipro_rtt_average <= 1000 && wifipro_rtt_calc_pkg > 3 && time_after(wifipro_when_recorded_rtt + WIFIPRO_MONITOR_DELAY, jiffies)){
        tcp_quality = LINK_MAYBE_GOOD;
    }else if(Interval_retrans_rate <= 10 && Interval_OutSegs >= 10){
        if(Interval_OutSegs >= 20) {
            tcp_quality = LINK_GOOD;
        }else{  //20 >= Interval_OutSegs >= 10
            tcp_quality = LINK_MAYBE_GOOD;
        }
    }else if(Interval_retrans_rate > 40 && Interval_OutSegs > 5){
        if(wifipro_rtt_average <= WIFIPRO_RTT_THRESHOLD && wifipro_rtt_calc_pkg > 5 && time_after(wifipro_when_recorded_rtt + WIFIPRO_MONITOR_DELAY, jiffies)){
            tcp_quality = LINK_UNKNOWN;
        }else if(wifipro_is_cong_occured(wifipro_congestion_stat, TCP_CA_Disorder) || wifipro_is_cong_occured(wifipro_congestion_stat, TCP_CA_Recovery)){
            tcp_quality = LINK_POOR;
        }else{
            tcp_quality = LINK_MAYBE_POOR;
        }
	}else{
        tcp_quality = LINK_UNKNOWN;
	}

    if(wifipro_log_level >= WIFIPRO_DEBUG){
        char printk_buf[2048];
        int buf_len = 0;

        buf_len += sprintf(printk_buf + buf_len, "\n##################### after 3s, wifipro_tcp_curr_inf #####################\n");
        buf_len += sprintf(printk_buf + buf_len, "\tCurrEstab = %lu\n", wifipro_tcp_curr_inf.CurrEstab);
        buf_len += sprintf(printk_buf + buf_len, "\tInSegs = %lu\n", wifipro_tcp_curr_inf.InSegs);
        buf_len += sprintf(printk_buf + buf_len, "\tOutSegs = %lu\n", wifipro_tcp_curr_inf.OutSegs);
        buf_len += sprintf(printk_buf + buf_len, "\tRetransSegs = %lu\n", wifipro_tcp_curr_inf.RetransSegs);
        buf_len += sprintf(printk_buf + buf_len, "\tInErrs = %lu\n", wifipro_tcp_curr_inf.InErrs);
        buf_len += sprintf(printk_buf + buf_len, "\tretrans_rate = %%%d\n\n", retrans_rate);

        buf_len += sprintf(printk_buf + buf_len, "\tInterval_InSegs = %lu\n", Interval_InSegs);
        buf_len += sprintf(printk_buf + buf_len, "\tInterval_OutSegs = %lu\n", Interval_OutSegs);
        buf_len += sprintf(printk_buf + buf_len, "\tInterval_RetransSegs = %lu\n", Interval_RetransSegs);
        buf_len += sprintf(printk_buf + buf_len, "\tInterval_retrans_rate = %%%d\n", Interval_retrans_rate);

        buf_len += sprintf(printk_buf + buf_len, "\trtt_average = %d\n", wifipro_rtt_average);
        buf_len += sprintf(printk_buf + buf_len, "\trtt_calc_pkg = %d\n", wifipro_rtt_calc_pkg);
        buf_len += sprintf(printk_buf + buf_len, "\trecord rtt %ds ago\n", (jiffies - wifipro_when_recorded_rtt)/HZ);
        buf_len += sprintf(printk_buf + buf_len, "\ttcp_quality = %d\n", tcp_quality);
        buf_len += sprintf(printk_buf + buf_len, "##################### wifipro_tcp_curr_inf #####################\n");

        printk("%s", printk_buf);
    }

    if(tcp_quality == LINK_MAYBE_POOR || tcp_quality == LINK_POOR){
        wifipro_tcp_monitor_send_msg(g_user_space_pid, WIFIPRO_KNL_NOTIFY, tcp_quality);
    }

    is_delayed_work_handling = false;
	return;
}

void wifipro_handle_congestion(struct sock *sk, u8 ca_state)
{
    struct inet_sock *inet = inet_sk(sk);
    if(NULL == inet) {
        WIFIPRO_ERROR("GET NULL POINTER!");
        return;
	}
    unsigned int dest_addr = htonl(inet->inet_daddr);
    unsigned int dest_port = htons(inet->inet_dport);

	if(!wifipro_is_not_local_or_lan_sock(dest_addr)){
        return;
	}

    if(wifipro_is_in_google_sock_list(wifipro_g_sock_head, dest_addr)){ //google server socket
        return;
    }

    wifipro_cong_sock_t *dst = NULL;

    switch(ca_state){
        case TCP_CA_Open:
            wifipro_set_cong_stat(dest_addr, dest_port, wifipro_congestion_stat, TCP_CA_Open);
            break;

        case TCP_CA_Disorder:
            dst = wifipro_congestion_stat + TCP_CA_Disorder;
            if(wifipro_log_level >= WIFIPRO_DEBUG){
                char printk_buf[2048];
                int buf_len = 0;
                buf_len += sprintf(printk_buf + buf_len, "\n************************** TCP_CA_Disorder **************************\n");
                buf_len += sprintf(printk_buf + buf_len, "%s: %s:%d  sk_state=%d  amount=%d, %dms ago",
                    __func__, wifipro_ntoa(dest_addr), dest_port, sk->sk_state, dst->amount+1, (jiffies - dst->when)*WIFIPRO_TICK_TO_MS);
                buf_len += sprintf(printk_buf + buf_len, "\n*********************************************************************\n");
                printk("%s", printk_buf);
            }
            wifipro_set_cong_stat(dest_addr, dest_port, wifipro_congestion_stat, TCP_CA_Disorder);
            break;

        case TCP_CA_CWR:
            dst = wifipro_congestion_stat + TCP_CA_CWR;
            if(wifipro_log_level >= WIFIPRO_DEBUG){
                char printk_buf[2048];
                int buf_len = 0;
                buf_len += sprintf(printk_buf + buf_len, "\n************************** TCP_CA_CWR **************************\n");
                buf_len += sprintf(printk_buf + buf_len, "%s: %s:%d  sk_state=%d  amount=%d, %dms ago",
                    __func__, wifipro_ntoa(dest_addr), dest_port, sk->sk_state, dst->amount+1, (jiffies - dst->when)*WIFIPRO_TICK_TO_MS);
                buf_len += sprintf(printk_buf + buf_len, "\n*********************************************************************\n");
                printk("%s", printk_buf);
            }
            wifipro_set_cong_stat(dest_addr, dest_port, wifipro_congestion_stat, TCP_CA_CWR);
            break;

        case TCP_CA_Recovery:
            dst = wifipro_congestion_stat + TCP_CA_Recovery;
            if(wifipro_log_level >= WIFIPRO_DEBUG){
                char printk_buf[2048];
                int buf_len = 0;
                buf_len += sprintf(printk_buf + buf_len, "\n************************** TCP_CA_Recovery **************************\n");
                buf_len += sprintf(printk_buf + buf_len, "%s: %s:%d  sk_state=%d  amount=%d, %dms ago",
                    __func__, wifipro_ntoa(dest_addr), dest_port, sk->sk_state, dst->amount+1, (jiffies - dst->when)*WIFIPRO_TICK_TO_MS);
                buf_len += sprintf(printk_buf + buf_len, "\n*********************************************************************\n");
                printk("%s", printk_buf);
            }
            wifipro_set_cong_stat(dest_addr, dest_port, wifipro_congestion_stat, TCP_CA_Recovery);
            break;

        case TCP_CA_Loss:
            wifipro_set_cong_stat(dest_addr, dest_port, wifipro_congestion_stat, TCP_CA_Loss);
            break;

        defult:
            WIFIPRO_WARNING("unvalid state, ca_state = %d", ca_state);
    }
}

void wifipro_update_rtt(unsigned int rtt, struct sock *sk)
{
    if(!wifipro_rtt_second_stat_head){
        return;
    }
    unsigned int rtt_total = 0;
    unsigned int trans_total = 0;
    unsigned int time_to_now = 0;
    bool is_found = false;
    bool is_head_set = false;
    wifipro_rtt_second_stat_t *curr = wifipro_rtt_second_stat_head;
    int i;

    struct inet_sock *inet = inet_sk(sk);
    if(NULL == inet) {
        WIFIPRO_ERROR("GET NULL POINTER!");
        return;
	}
    unsigned int dest_addr = htonl(inet->inet_daddr);
    unsigned int src_addr = htonl(inet->inet_saddr);
    if(!wifipro_is_not_local_or_lan_sock(dest_addr)){
        return;
    }
    struct inet_connection_sock *icsk = inet_csk(sk);
    struct tcp_sock *tp = tcp_sk(sk);

    mutex_lock(&wifipro_rtt_sem);
    for(i = 0; i < WIFIPRO_RTT_RECORD_SECONDS; i++){
        if(time_after(curr->expire + (WIFIPRO_RTT_RECORD_SECONDS - 1)*HZ, jiffies)){ //it's valid now
            if(time_after(curr->expire, jiffies)){
                curr->total_rtt += rtt;
                curr->amount++;
                wifipro_rtt_second_stat_head = curr;
                is_head_set = true;
            }
            rtt_total += curr->total_rtt;
            trans_total += curr->amount;
        }else{
            if(!is_head_set){  //set it to head
                curr->expire = jiffies + HZ;
                curr->total_rtt = rtt;
                curr->amount = 1;
                wifipro_rtt_second_stat_head = curr;
                is_head_set = true;
            }else{
                curr->total_rtt = 0;
                curr->amount = 0;
                curr->expire = jiffies - time_to_now * HZ;
                time_to_now++;
            }
        }
        curr = curr->next;
    }
    mutex_unlock(&wifipro_rtt_sem);

    if(0 != trans_total){
        wifipro_rtt_average = (rtt_total / trans_total) * WIFIPRO_TICK_TO_MS;
        wifipro_rtt_calc_pkg = trans_total;
        wifipro_when_recorded_rtt = jiffies;
    }

    //WIFIPRO_DEBUG("%d packets, wifipro_rtt_average=%d", wifipro_rtt_calc_pkg, wifipro_rtt_average);
    WIFIPRO_VERBOSE("-->%s  rto=%d  srtt=%d  pks=%d, rtt_avg=%d",wifipro_ntoa(dest_addr),
        icsk->icsk_rto, tp->srtt, wifipro_rtt_calc_pkg, wifipro_rtt_average);
}

static void wifipro_cong_stat_init(wifipro_cong_sock_t *src, unsigned char offset, const char *name)
{
    wifipro_cong_sock_t *dst = src + offset;
    strncpy(dst->state_name, name, WIFIPRO_MAX_CA_NAME);
}


static int wifipro_tcp_monitor_init()
{
    /* The wifipro_rtt_second_stat_head always point to the newest node
     * The expire time of eath node of the list is
    */
    mutex_lock(&wifipro_rtt_sem);
    wifipro_rtt_second_stat_head = kzalloc(sizeof(wifipro_rtt_second_stat_t), GFP_KERNEL);
    if(NULL == wifipro_rtt_second_stat_head){
        WIFIPRO_ERROR("kzalloc failed");
        mutex_unlock(&wifipro_rtt_sem);
        return -1;
    }
    wifipro_rtt_second_stat_head->expire = jiffies + HZ;
    wifipro_rtt_second_stat_head->second_num = 0;

    int i;
    wifipro_rtt_second_stat_t *curr = NULL;
    wifipro_rtt_second_stat_t *prev = wifipro_rtt_second_stat_head;
    for(i = 1; i < WIFIPRO_RTT_RECORD_SECONDS; i++){
        curr = kzalloc(sizeof(wifipro_rtt_second_stat_t), GFP_KERNEL);
        if(NULL == curr){
            WIFIPRO_ERROR("kzalloc failed");
            mutex_unlock(&wifipro_rtt_sem);
            return -1;
        }
        //memset(curr, 0, sizeof(wifipro_rtt_second_stat_t));
        curr->second_num = i;
        curr->expire = jiffies + (1 - i)*HZ;
        prev->next = curr;
        prev = curr;
    }
    curr->next = wifipro_rtt_second_stat_head;
    mutex_unlock(&wifipro_rtt_sem);

    return 0;
}


static void wifipro_tcp_monitor_deinit(void)
{
    is_delayed_work_handling = false;
    cancel_work_sync(&wifipro_tcp_retrans_work);
    cancel_delayed_work(&wifipro_tcp_monitor_work);

    wifipro_rtt_free(wifipro_rtt_second_stat_head);
    wifipro_rtt_second_stat_head = NULL;

    return;
}

static int __init wifipro_tcp_monitor_module_init(void)
{
    struct netlink_kernel_cfg wifipro_tcp_monitor_nl_cfg = {
          .input = wifipro_tcp_monitor_nl_receive,
    };
    g_wifipro_nlfd = netlink_kernel_create(&init_net,
                        NETLINK_WIFIPRO_EVENT_NL,
                        &wifipro_tcp_monitor_nl_cfg);
    if(!g_wifipro_nlfd){
        WIFIPRO_ERROR("netlink init fail");
        return -1;
    }

    INIT_WORK(&wifipro_tcp_retrans_work, wifipro_tcp_retrans_work_handler);
    INIT_DELAYED_WORK(&wifipro_tcp_monitor_work, wifipro_tcp_monitor_work_handler);

    wifipro_tcp_trigger_inf = kzalloc(sizeof(struct wifipro_tcp_monitor_inf), GFP_KERNEL);
    if(wifipro_tcp_trigger_inf == NULL){
        WIFIPRO_ERROR("kzalloc failed");
        return -1;
	}

    wifipro_trigger_sock = kzalloc(sizeof(wifipro_trigger_sock_t), GFP_KERNEL);
    if(wifipro_trigger_sock == NULL){
        WIFIPRO_ERROR("kzalloc failed");
        return -1;
	}

    wifipro_retrans_sock = kzalloc(sizeof(wifipro_retrans_sock_t), GFP_KERNEL);
    if(wifipro_retrans_sock == NULL){
        WIFIPRO_ERROR("kzalloc failed");
        return -1;
	}

    wifipro_congestion_stat = (wifipro_cong_sock_t*)kzalloc(WIFIPRO_CONG_ARRAY*sizeof(*wifipro_congestion_stat), GFP_KERNEL);
    if(NULL == wifipro_congestion_stat){
        WIFIPRO_ERROR("kzalloc failed");
        return -1;
    }
    wifipro_cong_stat_init(wifipro_congestion_stat, TCP_CA_Open, "Open");
    wifipro_cong_stat_init(wifipro_congestion_stat, TCP_CA_Disorder, "Disorder");
    wifipro_cong_stat_init(wifipro_congestion_stat, TCP_CA_CWR, "CWR");
    wifipro_cong_stat_init(wifipro_congestion_stat, TCP_CA_Recovery, "Recovery");
    wifipro_cong_stat_init(wifipro_congestion_stat, TCP_CA_Loss, "Loss");
    return 0;
}

static void __exit wifipro_tcp_monitor_module_exit(void)
{
    wifipro_tcp_monitor_deinit();

    if(g_wifipro_nlfd && g_wifipro_nlfd->sk_socket)
    {
        sock_release(g_wifipro_nlfd->sk_socket);
        g_wifipro_nlfd = NULL;
    }

    if(wifipro_tcp_trigger_inf){
        kfree(wifipro_tcp_trigger_inf);
        wifipro_tcp_trigger_inf = NULL;
    }

    if(wifipro_trigger_sock){
        kfree(wifipro_trigger_sock);
        wifipro_trigger_sock = NULL;
    }

    if(wifipro_retrans_sock){
        kfree(wifipro_retrans_sock);
        wifipro_retrans_sock = NULL;
    }

    if(wifipro_congestion_stat){
        kfree(wifipro_congestion_stat);
        wifipro_congestion_stat = NULL;
    }
}

module_init(wifipro_tcp_monitor_module_init);
module_exit(wifipro_tcp_monitor_module_exit);

#ifdef CONFIG_HW_WIFIPRO_PROC

//Output /proc/net/wifipro_log_level
static int wifipro_log_level_show(struct seq_file *seq, void *v)
{
	seq_printf(seq, "wifipro_log_level = %d", wifipro_log_level);
    seq_puts(seq, "\n");
	return 0;
}

static int wifipro_log_level_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{
    unsigned char log_level = *buffer - '0';
    if(log_level >= WIFIPRO_ERR && log_level <= WIFIPRO_LOG_ALL){
        wifipro_log_level = log_level;
    }else{
        WIFIPRO_WARNING("unvalid log level");
    }

	return count;
}

static int wifipro_log_level_open(struct inode *inode, struct file *file)
{
	return single_open_net(inode, file, wifipro_log_level_show);
}

static const struct file_operations wifipro_log_level_seq_fops = {
	.owner	 = THIS_MODULE,
	.open	 = wifipro_log_level_open,
	.read	 = seq_read,
	.write   = wifipro_log_level_write,
	.llseek	 = seq_lseek,
	.release = single_release_net,
};

//Called from the PROCfs module. This outputs /proc/net/wifipro_tcp_stat.
static int wifipro_snmp_seq_show(struct seq_file *seq, void *v)
{
	int i;
	struct net *net = seq->private;
	struct timeval tv;
	do_gettimeofday(&tv);

    if(is_wifipro_on){
       seq_puts(seq, "Wi-Fi Pro is on\n");
    }else{
       seq_puts(seq, "Wi-Fi Pro is off\n");
    }
    seq_puts(seq, "Compiled: ");
    seq_puts(seq, __DATE__);
    seq_puts(seq, " at ");
    seq_puts(seq, __TIME__);
	seq_puts(seq, "\nCurrent Time: ");
    struct timex  txc;
    struct rtc_time tm;
    do_gettimeofday(&(txc.time));
    rtc_time_to_tm(txc.time.tv_sec,&tm);
    seq_printf(seq, "%d:%d:%d\n", tm.tm_hour + 8, tm.tm_min, tm.tm_sec);

	for (i = 0; wifipro_snmp_tcp_list[i].name != NULL; i++) {
		seq_printf(seq, " %s = ", wifipro_snmp_tcp_list[i].name);

		if (wifipro_snmp_tcp_list[i].entry == TCP_MIB_MAXCONN)
			seq_printf(seq, " %ld\n",
				   snmp_fold_field((void __percpu **)net->mib.wifipro_tcp_statistics,
						   wifipro_snmp_tcp_list[i].entry));
		else
			seq_printf(seq, " %lu\n",
				   snmp_fold_field((void __percpu **)net->mib.wifipro_tcp_statistics,
						   wifipro_snmp_tcp_list[i].entry));
    }

    if(NULL != wifipro_g_sock_head){
        wifipro_g_sock_bl_t *curr_bl = wifipro_g_sock_head;

        seq_printf(seq, "\nGoogle socket backlist:\n");
        while(curr_bl){
            seq_printf(seq, "%s owned by process %s pid %d\n",
                wifipro_ntoa(curr_bl->dst_addr), curr_bl->proc_name, curr_bl->pid);
            curr_bl = curr_bl->next;
        }
    }

    if(NULL != wifipro_congestion_stat){
        seq_printf(seq, "\nwifipro congestion state:\n");
        wifipro_cong_sock_t *dst = NULL;
        for(i = 1; i < WIFIPRO_CONG_ARRAY; i++){
            dst = wifipro_congestion_stat + i;
            seq_printf(seq, "%s: occured %dtimes, last occured from %s:%d %ds ago\n",
                dst->state_name,
                dst->amount,
                wifipro_ntoa(dst->dst_addr), dst->dst_port,
                (jiffies - dst->when)/HZ);
        }
    }

    if(NULL != wifipro_rtt_second_stat_head){
        wifipro_rtt_second_stat_t *curr = wifipro_rtt_second_stat_head;
        seq_printf(seq, "\nwifipro rtt:\n");
        for(i=0; i <= WIFIPRO_RTT_RECORD_SECONDS; i++){
            seq_printf(seq, "%d: rtt %d, trans %d, %dms %s\n",
               curr->second_num, curr->total_rtt, curr->amount,
               time_after(curr->expire, jiffies)?(curr->expire - jiffies)*WIFIPRO_TICK_TO_MS:(jiffies - curr->expire)*WIFIPRO_TICK_TO_MS,
               time_after(curr->expire, jiffies)?"later":"ago" );
            curr = curr->next;
        }
        seq_printf(seq, "average rtt is %dms\npacket trans:%d\nrecorded %ds ago\n", wifipro_rtt_average, wifipro_rtt_calc_pkg, (jiffies - wifipro_when_recorded_rtt)/HZ);
    }

    seq_puts(seq, "\n");
	return 0;
}

static int wifipro_snmp_seq_open(struct inode *inode, struct file *file)
{
	return single_open_net(inode, file, wifipro_snmp_seq_show);
}

static const struct file_operations wifipro_snmp_seq_fops = {
	.owner	 = THIS_MODULE,
	.open	 = wifipro_snmp_seq_open,
	.read	 = seq_read,
	.llseek	 = seq_lseek,
	.release = single_release_net,
};

int wifipro_init_proc(struct net *net)
{
	if (!proc_create("wifipro_tcp_stat", S_IRUGO, net->proc_net, &wifipro_snmp_seq_fops))
	    goto out_wifipro_tcp_stat;
	if (!proc_create("wifipro_log_level", (S_IRUSR|S_IRGRP) | (S_IWUSR|S_IWGRP), net->proc_net, &wifipro_log_level_seq_fops))
		goto out_wifipro_log_level;

	return 0;

out_wifipro_tcp_stat:
	return -ENOMEM;
out_wifipro_log_level:
	return -ENOMEM;
}
#endif

