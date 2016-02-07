/*
 * Detect Hung Task
 *
 * kernel/hung_task.c - kernel thread for detecting tasks stuck in D state
 *
 */

#include <linux/mm.h>
#include <linux/cpu.h>
#include <linux/nmi.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/freezer.h>
#include <linux/kthread.h>
#include <linux/lockdep.h>
#include <linux/export.h>
#include <linux/sysctl.h>

#ifdef CONFIG_HW_DETECT_HUNG_TASK

#include <linux/proc_fs.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <asm/traps.h>

#ifdef CONFIG_HISI_RDR
#include <linux/huawei/rdr_private.h>

static rdr_funcptr_2 g_rdr_hung_task_hook;

void rdr_hung_task_hook_add(rdr_funcptr_2 p_hook_func)
{
	g_rdr_hung_task_hook = p_hook_func;
}

void rdr_hung_task_hook_delete(void)
{
	g_rdr_hung_task_hook = NULL;
}
#else
typedef void (*rdr_funcptr_2)(u32, u32);
void rdr_hung_task_hook_add(rdr_funcptr_2 p_hook_func)
{
}

void rdr_hung_task_hook_delete(void)
{
}
#endif

/*
 * The number of tasks checked:
 */
#define NAME_NUM  16

#define NO_MATCH_INLIST -1
#define NOT_DEFINE 0
#define WHITE_LIST 1
#define BLACK_LIST 2
static unsigned int whitelist = BLACK_LIST;

#define HT_ENABLE 1
#define HT_DISABLE 0
static unsigned int hungtask_enable = HT_DISABLE;

/* storage names and last switch counts of process in D status */
struct tag_switch_count {
	pid_t p_pid;
	unsigned long last_swithc_count;
};

static char p_name[TASK_COMM_LEN*NAME_NUM] = {0};
/* storage proccess list in hung task mechanism */
struct name_table {
	char name[TASK_COMM_LEN];
	pid_t p_pid;
};

static struct name_table p_name_table[NAME_NUM];
static struct tag_switch_count last_switch_count_table[NAME_NUM];

static bool rcu_lock_break(struct task_struct *g, struct task_struct *t);

#endif /*CONFIG_HW_DETECT_HUNG_TASK*/

/*
 * The number of tasks checked:
 */
unsigned long __read_mostly sysctl_hung_task_check_count = PID_MAX_LIMIT;

/*
 * Limit number of tasks checked in a batch.
 *
 * This value controls the preemptibility of khungtaskd since preemption
 * is disabled during the critical section. It also controls the size of
 * the RCU grace period. So it needs to be upper-bound.
 */
#define HUNG_TASK_BATCHING 1024

/*
 * Zero means infinite timeout - no checking done:
 */
unsigned long __read_mostly sysctl_hung_task_timeout_secs = CONFIG_DEFAULT_HUNG_TASK_TIMEOUT;

unsigned long __read_mostly sysctl_hung_task_warnings = 10;

#ifdef CONFIG_ILOCKDEP
unsigned long __read_mostly sysctl_hung_task_show_count = 10;
#endif

static int __read_mostly did_panic;

static struct task_struct *watchdog_task;

/*
 * Should we panic (and reboot, if panic_timeout= is set) when a
 * hung task is detected:
 */
unsigned int __read_mostly sysctl_hung_task_panic =
				CONFIG_BOOTPARAM_HUNG_TASK_PANIC_VALUE;

static int __init hung_task_panic_setup(char *str)
{
	sysctl_hung_task_panic = simple_strtoul(str, NULL, 0);

	return 1;
}
__setup("hung_task_panic=", hung_task_panic_setup);

static int
hung_task_panic(struct notifier_block *this, unsigned long event, void *ptr)
{
	did_panic = 1;

	return NOTIFY_DONE;
}

static struct notifier_block panic_block = {
	.notifier_call = hung_task_panic,
};

#ifdef CONFIG_HW_DETECT_HUNG_TASK

static void dump_hungtask_trace(unsigned long where, unsigned long from)
{
#ifdef CONFIG_HISI_RDR
	if (0 == atomic_read(&rdr_crit_dump_stack))
		return;
	if ((NULL != g_rdr_dump_stack) && (g_rdr_dump_stack_len > 20)) {
		int i;
		u32 buf[9];

		*g_rdr_dump_stack = where;
		g_rdr_dump_stack++;
		g_rdr_dump_stack_len -= sizeof(u32);

		memset((void *)buf, 0, 9 * sizeof(u32));
		snprintf((char *)buf, 9 * sizeof(u32),
			"%pS", (unsigned long *)where);
		for (i = 0; i < 9; i++) {
			*g_rdr_dump_stack = buf[i];
			g_rdr_dump_stack++;
			g_rdr_dump_stack_len -= sizeof(u32);
		}

		*g_rdr_dump_stack = from;
		g_rdr_dump_stack++;
		g_rdr_dump_stack_len -= sizeof(u32);

		memset((void *)buf, 0, 9 * sizeof(u32));
		snprintf((char *)buf, 9 * sizeof(u32),
			"%pS", (unsigned long *)from);
		for (i = 0; i < 9; i++) {
			*g_rdr_dump_stack = buf[i];
			g_rdr_dump_stack++;
			g_rdr_dump_stack_len -= sizeof(u32);
		}
	}
#endif
}

static void findpnamepid(int index)
{
	int max_count = sysctl_hung_task_check_count;
	int batch_count = HUNG_TASK_BATCHING;
	struct task_struct *g, *t;

	rcu_read_lock();
	do_each_thread(g, t) {
		if (!max_count--)
			goto unlock_f;
		if (!--batch_count) {
			batch_count = HUNG_TASK_BATCHING;
			if (!rcu_lock_break(g, t))
				goto unlock_f;
		}
		if (strlen(t->comm) == strlen(p_name_table[index].name))
			if (strncmp(p_name_table[index].name,
				t->comm, strlen(t->comm)) == 0) {
				p_name_table[index].p_pid = t->pid;
				goto unlock_f;
			}
	} while_each_thread(g, t);
unlock_f:
	rcu_read_unlock();
}

static void findpname(void)
{
	int i = 0;
	while ((i < NAME_NUM) && ('\0' != p_name_table[i].name[0])) {
		p_name_table[i].p_pid = -1;
		findpnamepid(i);
		i++;
	}
}

static int checklist(pid_t ht_pid, pid_t ht_ppid, unsigned int list_category)
{
	int i = 0;
	int in_list = 0;

	findpname();

	if (WHITE_LIST == list_category) {
		while ((i < NAME_NUM) && (0 != p_name_table[i].p_pid)) {
			if ((p_name_table[i].p_pid == ht_pid)
				|| (p_name_table[i].p_pid == ht_ppid))
				in_list++;

			i++;
		}
	} else if (BLACK_LIST == list_category) {
		while ((i < NAME_NUM) && (0 != p_name_table[i].p_pid)) {
			if (p_name_table[i].p_pid == ht_pid)
				in_list++;

			i++;
		}
	}

	return in_list;
}

#endif /*CONFIG_HW_DETECT_HUNG_TASK*/

static void check_hung_task(struct task_struct *t, unsigned long timeout)
{
	unsigned long switch_count = t->nvcsw + t->nivcsw;

#ifdef CONFIG_HW_DETECT_HUNG_TASK

	int idx, first_empty_item = -1;
	int in_list = 0;
	unsigned int list_category = whitelist;
	pid_t ht_pid, ht_ppid;

#endif /*CONFIG_HW_DETECT_HUNG_TASK*/

	/*
	 * Ensure the task is not frozen.
	 * Also, skip vfork and any other user process that freezer should skip.
	 */
	if (unlikely(t->flags & (PF_FROZEN | PF_FREEZER_SKIP)))
	    return;

#ifdef CONFIG_HW_DETECT_HUNG_TASK

	/*
	 * When a freshly created task is scheduled once, changes its state to
	 * TASK_UNINTERRUPTIBLE without having ever been switched out once, it
	 * musn't be checked.
	 */
	if (unlikely(!switch_count))
		return;
	if (NOT_DEFINE == list_category)
		return;

	if ((WHITE_LIST == list_category) && ('\0' == p_name_table[0].name[0]))
		return;

	ht_pid = t->pid;
	ht_ppid = t->tgid;


	in_list = checklist(ht_pid, ht_ppid, list_category);

	if (0 == in_list) {
		if (WHITE_LIST == list_category) {
			pr_err("hung_task: %s NOT in whitelist.\n", t->comm);
			return;
		}
	} else {
		if (BLACK_LIST == list_category) {
			pr_err("hung_task: %s is in blacklist.\n", t->comm);
			return;
		}
		pr_err("hung_task: %s is in whitelist.\n", t->comm);
	}

	/* find last swich count record in last_switch_count_table */
	for (idx = 0; idx < NAME_NUM; idx++) {
		if (0 == last_switch_count_table[idx].p_pid) {
			if (-1 == first_empty_item)
				first_empty_item = idx;
		} else {
			if (last_switch_count_table[idx].p_pid == ht_pid)
				break;
		}
	}

	/* if current proccess is not in last switch count table,
	 * insert a new record
	 */
	if (NAME_NUM == idx) {
		if (first_empty_item == -1)
			return;
		last_switch_count_table[first_empty_item].p_pid = ht_pid;
		last_switch_count_table[first_empty_item].last_swithc_count = 0;
		idx = first_empty_item;
	}

	if (switch_count != last_switch_count_table[idx].last_swithc_count) {

		last_switch_count_table[idx].last_swithc_count = switch_count;
		return;
	}

#else

	if (switch_count != t->last_switch_count) {
		t->last_switch_count = switch_count;
		return;
	}

#endif /*CONFIG_HW_DETECT_HUNG_TASK*/

	if (!sysctl_hung_task_warnings)
		return;

#ifdef CONFIG_HW_DETECT_HUNG_TASK
	/* sysctl_hung_task_warnings--;*/
#else
	sysctl_hung_task_warnings--;
#endif /*CONFIG_HW_DETECT_HUNG_TASK*/

	/*
	 * Ok, the task did not get scheduled for more than 2 minutes,
	 * complain:
	 */

	pr_err("INFO: task %s:%d blocked for more than %ld s.\n",
			t->comm, t->pid, sysctl_hung_task_timeout_secs);

#ifdef CONFIG_HISI_RDR
	g_rdr_hung_task_hook((unsigned long)t, sysctl_hung_task_timeout_secs);
	sched_show_task(t);
#else
	sched_show_task(t);
#endif

	debug_show_held_locks(t);

	touch_nmi_watchdog();
/*
#ifdef CONFIG_ILOCKDEP
	if (sysctl_hung_task_show_count != 0) {
		show_state();
		sysctl_hung_task_show_count--;
	}
#endif
*/
	if (sysctl_hung_task_panic) {
		trigger_all_cpu_backtrace();
		panic("hung_task: blocked tasks");
	}
}

/*
 * To avoid extending the RCU grace period for an unbounded amount of time,
 * periodically exit the critical section and enter a new one.
 *
 * For preemptible RCU it is sufficient to call rcu_read_unlock in order
 * to exit the grace period. For classic RCU, a reschedule is required.
 */
static bool rcu_lock_break(struct task_struct *g, struct task_struct *t)
{
	bool can_cont;

	get_task_struct(g);
	get_task_struct(t);
	rcu_read_unlock();
	cond_resched();
	rcu_read_lock();
	can_cont = pid_alive(g) && pid_alive(t);
	put_task_struct(t);
	put_task_struct(g);

	return can_cont;
}

/*
 * Check whether a TASK_UNINTERRUPTIBLE does not get woken up for
 * a really long time (120 seconds). If that happens, print out
 * a warning.
 */
static void check_hung_uninterruptible_tasks(unsigned long timeout)
{
	int max_count = sysctl_hung_task_check_count;
	int batch_count = HUNG_TASK_BATCHING;
	struct task_struct *g, *t;

	/*
	 * If the system crashed already then all bets are off,
	 * do not report extra hung tasks:
	 */
	if (test_taint(TAINT_DIE) || did_panic)
		return;

	rcu_read_lock();
	do_each_thread(g, t) {
		if (!max_count--)
			goto unlock;
		if (!--batch_count) {
			batch_count = HUNG_TASK_BATCHING;
			if (!rcu_lock_break(g, t))
				goto unlock;
		}
		/* use "==" to skip the TASK_KILLABLE tasks waiting on NFS */
		if (t->state == TASK_UNINTERRUPTIBLE)
			check_hung_task(t, timeout);
	} while_each_thread(g, t);
 unlock:
	rcu_read_unlock();
}

static unsigned long timeout_jiffies(unsigned long timeout)
{
	/* timeout of 0 will disable the watchdog */
	return timeout ? timeout * HZ : MAX_SCHEDULE_TIMEOUT;
}

/*
 * Process updating of timeout sysctl
 */
int proc_dohung_task_timeout_secs(struct ctl_table *table, int write,
				  void __user *buffer,
				  size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_doulongvec_minmax(table, write, buffer, lenp, ppos);

	if (ret || !write)
		goto out;

	wake_up_process(watchdog_task);

	pr_err("proc_dohung_task_timeout_secs %ld.\n",
		sysctl_hung_task_timeout_secs);

 out:
	return ret;
}

/*
 * kthread which checks for tasks stuck in D state
 */
static int watchdog(void *dummy)
{
	set_user_nice(current, 0);

	for ( ; ; ) {
		unsigned long timeout = sysctl_hung_task_timeout_secs;

		while (schedule_timeout_interruptible(timeout_jiffies(timeout)))
			timeout = sysctl_hung_task_timeout_secs;

	pr_err("watchdog is wakeup-----------.\n");

#ifdef CONFIG_HW_DETECT_HUNG_TASK
		if (hungtask_enable)
			check_hung_uninterruptible_tasks(timeout);
#else
		check_hung_uninterruptible_tasks(timeout);
#endif /*CONFIG_HW_DETECT_HUNG_TASK*/
	}

	return 0;
}

#ifdef CONFIG_HW_DETECT_HUNG_TASK

static ssize_t enable_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	if (hungtask_enable)
		return snprintf(buf, sizeof(buf), "on\n");
	else
		return snprintf(buf, sizeof(buf), "off\n");
}

static ssize_t enable_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	char tmp[6];
	int len;
	char *p;

	if ((count < 2) || (count > (sizeof(tmp) - 1))) {
		pr_err("hung_task: string too long or too short.\n");
		return -EINVAL;
	}

	p = memchr(buf, '\n', count);
	len = p ? p - buf : count;

	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, buf, len);
	pr_err("hung_task:tmp=%s, count %d\n", tmp, count);

	if (strncmp(tmp, "on", strlen(tmp)) == 0) {
		hungtask_enable = HT_ENABLE;
		pr_err("hung_task: hungtask_enable is set to enable.\n");
	} else if (strncmp(tmp, "off", strlen(tmp)) == 0) {
		hungtask_enable = HT_DISABLE;
		pr_err("hung_task: hungtask_enable is set to disable.\n");
	} else
		pr_err("hung_task: only accept on or off !\n");

	return (ssize_t)count;
}
/*
*	monitorlist_show  -	Called when 'cat' method
*	is used on entry 'pname' in /proc fs.
*	most of the parameters is created by kernel.
*/
static ssize_t monitorlist_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	char *start = buf;

	if (whitelist == WHITE_LIST)
		buf += sprintf(buf, "whitelist: [%s]\n", p_name);
	else if (whitelist == BLACK_LIST)
		buf += sprintf(buf, "blacklist: [%s]\n", p_name);
	else
		buf += sprintf(buf, "\n");

	return buf - start;
}


/* storage proccess names in [pname] to
 * [pname_table], and return the numbers of process
 */
static int rebuild_name_table(char *pname, int pname_len)
{
	int count = 0;
	int proc_name_len;
	const char *curr = pname;
	char *curr_table;
	int idx = 0;
	int detected = 0;

	whitelist = NOT_DEFINE;


	/* reset the table to empty */

	memset(p_name_table, 0x00, sizeof(p_name_table));

	while ('\0' != *curr && pname_len) {
		/* proccess names are seperated by comma */
		while ((',' == *curr) && pname_len) {
			curr++;
			pname_len--;
		}

		/* check if the number of proccess exceed the limit,
		 * pointer [curr] not an end symbol indicates
		 * that the after [NAME_NUM] proccess,
		 * the [NAME_NUM + 1]th proccess was found
		 */
		if (NAME_NUM == count && '\0' != *curr)
			goto err_proc_num;

		/* if the user input [NAME_NUM] proccess name,
		 * but just end his input by a space or comma,
		 * we just jump out the loop
		 */
		if (NAME_NUM == count)
			break;

		/* the [count]th name should be storage in corresponding
		 * item in table, and [proc_name_len] is set to count
		 * the length of process name
		 */
		proc_name_len = 0;
		curr_table = p_name_table[count].name;

		while (',' != *curr && '\0' != *curr && pname_len) {
			*curr_table = *curr;
			curr_table++;
			curr++;
			proc_name_len++;

			/* check if the length of
			 * proccess name exceed the limit
			 */
			if (TASK_COMM_LEN == proc_name_len)
				goto err_proc_name;
			pname_len--;
		}
		*curr_table = '\0';

		if ((count == 0) && (detected == 0)) {
			detected++;
			if (strncmp(p_name_table[0].name, "whitelist",
					strlen(p_name_table[0].name)) == 0) {
				pr_err("hung_task:set to whitelist.\n");
				whitelist = WHITE_LIST;
				continue;
			} else if (strncmp(p_name_table[0].name, "blacklist",
					strlen(p_name_table[0].name)) == 0) {
				whitelist = BLACK_LIST;
				pr_err("hung_task:set to blacklist.\n");
				continue;
			} else {
				whitelist = BLACK_LIST;
				pr_err("hung_task:default is  blacklist.\n");
			}
		}
		pr_err("\nhung_task: name_table: %d, %s,name_len: %d\n",
			count, p_name_table[count].name, proc_name_len);

		/* count how many proccess,
		 * only when [proc_name_len] is not zero,
		 * one new proccess was added into [pname_table]
		 */
		if (proc_name_len)
			count++;
	}
	/*last_switch_count_table  reset*/
	for (idx = 0; idx < NAME_NUM; idx++) {
		last_switch_count_table[idx].p_pid = 0;
		last_switch_count_table[idx].last_swithc_count = 0;
	}

	return count;

err_proc_name:
	memset(p_name_table, 0x00, sizeof(p_name_table));
	memset(pname, 0x00, sizeof(pname));
	pr_err("hung_task: rebuild_name_table: Error: process name");
	pr_err(" is invallid, set monitorlist failed.\n");

	return 0;

err_proc_num:
	/* more than 16 processes,remove it */
	pr_err("hung_task: rebuild_name_table: Warnig: too many ");
	pr_err("processess, leave it and do nothing.\n");
	return count;
}

/* since the proccess name written into [pname_table]
 * may be different from original input,
 * [p_name] should be modified to adjust [pname_table]
 */
static int modify_pname(int num_count)
{
	int i, len_count;
	size_t tlen;

	memset((void *)p_name, 0x00, sizeof(p_name));

	for (i = 0; i < num_count; i++) {
		tlen = strlcat(p_name, p_name_table[i].name, sizeof(p_name));
		if (tlen >= sizeof(p_name))
			return -ENAMETOOLONG;

		/* seperate different proccess by a comma and a space */
		if (i != num_count - 1) {
			tlen = strlcat(p_name, ",", sizeof(p_name));
			if (tlen >= sizeof(p_name))
				return -ENAMETOOLONG;
		}
	}
	len_count = strlen(p_name);
	return len_count;
}

/*
*	monitorlist_store	-  Called when 'write/echo' method is
*	used on entry '/sys/kernel/hungtask/monitorlist'.
*/
static ssize_t monitorlist_store(struct kobject *kobj,
	struct kobj_attribute *attr, const char *buf, size_t n)
{
	int len;
	char *p;
	int num_count;
	int ret;

	p = memchr(buf, '\n', n);
	len = p ? p - buf : n;


	if ((len < 2) || (len > (sizeof(p_name) - 1))) {
		pr_err("hung_task: input string is too long or too short\n");
		return -EINVAL;
	}

	/* reset p_name to NULL */
	memset(p_name, 0x00, sizeof(p_name));

	/* -1: remove '\n'	*/
	strncpy(p_name, buf, len);

	/* convert [p_name] to a table [p_name_table],
	 * and refresh the buffer [p_name]
	 */
	num_count = rebuild_name_table(p_name, len);
	ret = modify_pname(num_count);

	if (ret < 0)
		return -EFAULT;

	return n;
}

static struct kobj_attribute timeout_attribute = {
	.attr   = {
		.name = "enable",
		.mode = 0644,
	},
	.show   = enable_show,
	.store  = enable_store,
};

static struct kobj_attribute monitorlist_attr = {
	.attr   = {
		.name = "monitorlist",
		.mode = 0644,
	},
	.show   = monitorlist_show,
	.store  = monitorlist_store,
};

static struct attribute *attrs[] = {
	&timeout_attribute.attr,
	&monitorlist_attr.attr,
	NULL
};

static struct attribute_group hungtask_attr_group = {
	.attrs = attrs,
};

struct kobject *hungtask_kobj;

int create_sysfs_hungtask(void)
{
	int retval;
	/* wait for kernel_kobj node ready: */
	while (kernel_kobj == NULL)
		msleep(1000);

	/* Create kobject named "hungtask",located under /sys/kernel/ */
	hungtask_kobj = kobject_create_and_add("hungtask", kernel_kobj);
	if (!hungtask_kobj)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(hungtask_kobj, &hungtask_attr_group);
	if (retval)
		kobject_put(hungtask_kobj);
	return retval;
}

typedef void (*funcptr2)(unsigned long, unsigned long);
static void register_hook_func(void)
{
	add_hw_hungtask_hook((funcptr2)dump_hungtask_trace);
}
#endif /*CONFIG_HW_DETECT_HUNG_TASK*/

static int __init hung_task_init(void)
{
#ifdef CONFIG_HW_DETECT_HUNG_TASK
	int ret;
	ret =	create_sysfs_hungtask();
	if (ret)
		pr_err("hung_task: create_sysfs_hungtask fail.\n");
#endif /*CONFIG_HW_DETECT_HUNG_TASK*/

	atomic_notifier_chain_register(&panic_notifier_list, &panic_block);
	register_hook_func();
	watchdog_task = kthread_run(watchdog, NULL, "khungtaskd");

	return 0;
}


module_init(hung_task_init);
