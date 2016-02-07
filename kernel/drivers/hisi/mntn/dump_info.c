#include <asm/current.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/thread_info.h>
#include <linux/syslog.h>
#include <linux/rtc.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/kthread.h>
#include <linux/notifier.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/hardirq.h>
#include <linux/proc_fs.h>
//#include <mach/hisi_cortex-a9.h>
#include <queue.h>
#include "soc_peri_sctrl_interface.h"
#include <soc_ao_sctrl_interface.h>
#include "excDrv.h"
#include "product_config.h"
#include <linux/hisi/hi6xxx-iomap.h>

static char *last_kirq = NULL;
static char *last_ktask = NULL;

static int ktask_proc_show(struct seq_file *m, void *v)
{
	char *mem = NULL;
	void * __iomem exc_mem = NULL;
	struct dump_log *dump = NULL;
	struct queue *head = NULL;
	struct task_info info;
	unsigned long flags = 0;
	u32 count = num_online_cpus();
	unsigned int i = 0;

	exc_mem = (void *)ioremap(ANDROID_DUMP_LOG_ADDR, ANDROID_DUMP_LOG_SIZE);
	if (NULL == exc_mem) {
		printk("%s : failed to ioremap memory\r\n", __FUNCTION__);
		return 0;
	}

	dump = (struct dump_log *)exc_mem;

	mem = (char *)vmalloc(EXCH_TASK_SWITCH_SIZE);
	if (!mem) {
		printk("%s : failed to vmalloc memory\n",__FUNCTION__);
		goto ktask_unmap;
	}

	for (i = 0; i < count; i++) {
		head = (struct queue*)(dump->task_trace + g_task_record_offset[i]);
		spin_lock_irqsave(&head->lock, flags);
		hisi_io_memcpy(mem, &dump->task_trace[0], EXCH_TASK_SWITCH_SIZE);
		spin_unlock_irqrestore(&head->lock, flags);

		head = (struct queue *)(mem + g_task_record_offset[i]);
		seq_printf(m, "TASK Trace cpu[%d]\n", i);
		seq_printf(m, "in %u out %u size %u\n", head->in, head->out, head->max);

		do {
			if (_queue_out(head, (void *)&info, sizeof(struct task_info)))
				break;

			seq_printf(m, "[ %10lu ][ %10lu ] PID [ %6u ][ %18s ] scheduled in CPU [ %u ]\n", \
						info.jiff, info.slice, info.pid, info.taskname, info.cpu);
		} while(1);

		seq_printf(m, "\n");
	}

	vfree(mem);

ktask_unmap:
	iounmap(exc_mem);
	return 0;
}

static int ktask_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ktask_proc_show, NULL);
}

static const struct file_operations ktask_proc_fops = {
	.open = ktask_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init proc_ktask_init(void)
{
	proc_create("ktask", 0, NULL, &ktask_proc_fops);
	return 0;
}

static int kirq_proc_show(struct seq_file *m, void *v)
{
	char *mem = NULL ;
	void * __iomem exc_mem = NULL;
	struct dump_log *dump = NULL;
	struct queue *head = NULL;
	struct irq_info info;
	unsigned long flags = 0;
	u32 count = num_online_cpus();
	unsigned int i = 0;

	exc_mem = (void *)ioremap(ANDROID_DUMP_LOG_ADDR, ANDROID_DUMP_LOG_SIZE);
	if (NULL == exc_mem) {
		printk("%s : failed to ioremap memory\r\n", __FUNCTION__);
		return 0;
	}

	dump = (struct dump_log *)exc_mem;

	mem = (char *)vmalloc(EXCH_INT_SWITCH_SIZE);
	if (!mem) {
		printk("%s : failed to vmalloc memory\n",__FUNCTION__);
		goto kirq_unmap;
	}

	for (i = 0; i < count; i++) {
		head = (struct queue*)(dump->irq_trace + g_irq_record_offset[i]);
		spin_lock_irqsave(&head->lock, flags);
		hisi_io_memcpy(mem, &dump->irq_trace[0], EXCH_INT_SWITCH_SIZE);
		spin_unlock_irqrestore(&head->lock, flags);

		head = (struct queue *)(mem + g_irq_record_offset[i]);
		seq_printf(m, "IRQ Trace cpu[%d]\n", i);
		seq_printf(m, "in %u out %u size %u\n", head->in, head->out, head->max);

		do {
			if (_queue_out(head, (void *)&info, sizeof(struct irq_info)))
				break;

			if (EXCH_INT_EXIT == info.irq)
				seq_printf(m, "[ %10lu ][ %10lu ][ CPU %1d ] IRQ         OUT\n", \
							info.jiff, info.slice, info.cpu);
			else
				seq_printf(m, "[ %10lu ][ %10lu ][ CPU %1d ] IRQ [ %3d ] IN\n", \
							info.jiff, info.slice, info.cpu, info.irq);
		} while(1);

		seq_printf(m, "\n");
	}

	vfree(mem);

kirq_unmap:
	iounmap(exc_mem);
	return 0;
}

static int kirq_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, kirq_proc_show, NULL);
}

static const struct file_operations kirq_proc_fops = {
	.open = kirq_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init proc_kirq_init(void)
{
	proc_create("kirq", 0, NULL, &kirq_proc_fops);
	return 0;
}

static int __init dump_info_init(void)
{
	void * __iomem mem = NULL;
	struct dump_log *dump = NULL;
	struct queue *task_queue[NR_CPUS] = {0};
	struct queue *irq_queue[NR_CPUS] = {0};
	unsigned int i = 0;
	int irq_flag = 0, task_flag = 0;
	unsigned int saved_length = 0;
	u32 count = num_online_cpus();

	printk("%s ANDROID_DUMP_LOG_ADDR = %#x\n", __func__, ANDROID_DUMP_LOG_ADDR);
	mem = (void *)ioremap(ANDROID_DUMP_LOG_ADDR, ANDROID_DUMP_LOG_SIZE);
	if (NULL == mem) {
		printk(KERN_ERR "%s : failed to ioremap memory\n", __func__);
		return 0;
	}

	dump = (struct dump_log *)mem;

	for (i=0; i<count; i++) {
		irq_queue[i] = (struct queue*)(dump->irq_trace + g_irq_record_offset[i]);
		if (strncmp(irq_queue[i]->name, g_irq_trace_name[i], strlen(g_irq_trace_name[i]))) {
			printk(KERN_ERR "%s : irq trace cpu[%d] section header incorrect\n", __func__, i);
			printk(KERN_ERR "irq g:%s\n", g_irq_trace_name[i]);
			goto unmap;
		}
		/* 判断max值，防止队列头长度变换*/
		if (irq_queue[i]->max != (g_irq_record_len[i] - sizeof(struct queue))) {
			printk(KERN_ERR "%s : irq trace cpu[%d] section header, max length incorrect\n", __func__, i);
			goto unmap;
		}
		/* 判断head/tail值是否异常 */
		saved_length = MOD_SUB(irq_queue[i]->head, irq_queue[i]->tail, irq_queue[i]->max);
		if (saved_length%sizeof(struct irq_info) != 0) {
			printk(KERN_ERR "%s : irq trace cpu[%d] section header, head/tail incorrect\n", __func__, i);
			goto unmap;
		}
		if (irq_queue[i]->head != irq_queue[i]->tail) {
			irq_flag++;
		}

		task_queue[i] = (struct queue*)(dump->task_trace + g_task_record_offset[i]);
		if (strncmp(task_queue[i]->name, g_task_trace_name[i], strlen(g_task_trace_name[i]))) {
			printk(KERN_ERR "%s : task trace cpu[%d] section header incorrect\n", __func__, i);
			printk(KERN_ERR "task g:%s\n", g_task_trace_name[i]);
			goto unmap;
		}
		/* 判断max值，防止队列头长度变换*/
		if (task_queue[i]->max !=  (g_task_record_len[i] - sizeof(struct queue))) {
			printk(KERN_ERR "%s : task trace cpu[%d] section header, max length incorrect\n", __func__, i);
			goto unmap;
		}
		/* 判断head/tail值是否异常 */
		saved_length = MOD_SUB(task_queue[i]->head, task_queue[i]->tail, task_queue[i]->max);
		if (saved_length%sizeof(struct task_info) != 0) {
			printk(KERN_ERR "%s : task trace cpu[%d] section header, head/tail incorrect\n", __func__, i);
			goto unmap;
		}
		if (task_queue[i]->head != task_queue[i]->tail) {
			task_flag++;
		}
	}

	if (irq_flag) {
		last_kirq = (char *)vmalloc(EXCH_INT_SWITCH_SIZE);
		if (last_kirq) {
			hisi_io_memcpy(last_kirq, &dump->irq_trace[0], EXCH_INT_SWITCH_SIZE);
		}
	}

	if (task_flag) {
		last_ktask = (char *)vmalloc(EXCH_TASK_SWITCH_SIZE);
		if (last_ktask) {
			hisi_io_memcpy(last_ktask, &dump->task_trace[0], EXCH_TASK_SWITCH_SIZE);
		}
	}

unmap:
	iounmap(mem);
	return 0;
}

static int last_kirq_proc_show(struct seq_file *m, void *v)
{
	char *mem = NULL ;
	struct queue *head = NULL;
	struct irq_info info;
	u32 count = num_online_cpus();
	unsigned int i = 0;

	if (NULL == last_kirq) {
		printk(KERN_ERR "%s: last_kirq is null\n", __FUNCTION__);
		return 0;
	}
	mem = (char *)vmalloc(EXCH_INT_SWITCH_SIZE);
	if (!mem) {
		printk("%s : failed to vmalloc memory\n",__FUNCTION__);
		return 0;
	}

	hisi_io_memcpy(mem, last_kirq, EXCH_INT_SWITCH_SIZE);

	for (i = 0; i < count; i++) {
		head = (struct queue *)(mem + g_irq_record_offset[i]);
		seq_printf(m, "IRQ Trace cpu[%d]\n", i);
		seq_printf(m, "in %u out %u size %u\n", head->in, head->out, head->max);

		do {
			if (_queue_out(head, (void *)&info, sizeof(struct irq_info)))
				break;

			if (EXCH_INT_EXIT == info.irq)
				seq_printf(m, "[ %10lu ][ %10lu ][ CPU %1d ] IRQ         OUT\n", \
							info.jiff, info.slice, info.cpu);
			else
				seq_printf(m, "[ %10lu ][ %10lu ][ CPU %1d ] IRQ [ %3d ] IN\n", \
							info.jiff, info.slice, info.cpu, info.irq);
		} while(1);

		seq_printf(m, "\n");
	}

	vfree(mem);

	return 0;
}

static int last_kirq_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, last_kirq_proc_show, NULL);
}

static const struct file_operations last_kirq_proc_fops = {
	.open = last_kirq_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init proc_last_kirq_init(void)
{
	if (last_kirq) {
		proc_create("last_kirq", 0, NULL, &last_kirq_proc_fops);
	}
	return 0;
}


static int last_ktask_proc_show(struct seq_file *m, void *v)
{
	char *mem = NULL;
	struct queue *head = NULL;
	struct task_info info;
	u32 count = num_online_cpus();
	unsigned int i = 0;

	if (NULL == last_ktask) {
		printk(KERN_ERR "%s: last_ktask is null\n", __FUNCTION__);
		return 0;
	}

	mem = (char *)vmalloc(EXCH_TASK_SWITCH_SIZE);
	if (!mem) {
		printk("%s : failed to vmalloc memory\n",__FUNCTION__);
		return 0;
	}

	hisi_io_memcpy(mem, last_ktask, EXCH_TASK_SWITCH_SIZE);

	for (i = 0; i < count; i++) {
		head = (struct queue *)(mem + g_task_record_offset[i]);
		seq_printf(m, "TASK Trace cpu[%d]\n", i);
		seq_printf(m, "in %u out %u size %u\n", head->in, head->out, head->max);

		do {
			if (_queue_out(head, (void *)&info, sizeof(struct task_info)))
				break;

			seq_printf(m, "[ %10lu ][ %10lu ] PID [ %6u ][ %18s ] scheduled in CPU [ %u ]\n", \
						info.jiff, info.slice, info.pid, info.taskname, info.cpu);
		} while(1);

		seq_printf(m, "\n");
	}

	vfree(mem);

	return 0;
}

static int last_ktask_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, last_ktask_proc_show, NULL);
}

static const struct file_operations last_ktask_proc_fops = {
	.open = last_ktask_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init proc_last_ktask_init(void)
{
	if (last_ktask) {
		proc_create("last_ktask", 0, NULL, &last_ktask_proc_fops);
	}
	
	return 0;
}

static int last_modid_proc_show(struct seq_file *m, void *v)
{
	EH_ROOT* ehroot = (EH_ROOT*)EXCH_ROOT_ADDR;

	seq_printf(m, "modID: 0x%x\narg1 : 0x%x\narg2 : 0x%x\n",
				ehroot->modId, ehroot->arg1, ehroot->arg2);

	return 0;
}

static int last_modid_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, last_modid_proc_show, NULL);
}

static const struct file_operations last_modid_proc_fops = {
	.open = last_modid_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init proc_last_modid_init(void)
{
	proc_create("last_modid", 0, NULL, &last_modid_proc_fops);
	return 0;
}

arch_initcall(dump_info_init);
module_init(proc_last_kirq_init);
module_init(proc_last_ktask_init);
module_init(proc_last_modid_init);
module_init(proc_kirq_init);
module_init(proc_ktask_init);


