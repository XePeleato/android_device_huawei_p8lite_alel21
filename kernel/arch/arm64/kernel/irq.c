/*
 * Based on arch/arm/kernel/irq.c
 *
 * Copyright (C) 1992 Linus Torvalds
 * Modifications for ARM processor Copyright (C) 1995-2000 Russell King.
 * Support for Dynamic Tick Timer Copyright (C) 2004-2005 Nokia Corporation.
 * Dynamic Tick Timer written by Tony Lindgren <tony@atomide.com> and
 * Tuukka Tikkanen <tuukka.tikkanen@elektrobit.com>.
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/kernel_stat.h>
#include <linux/irq.h>
#include <linux/smp.h>
#include <linux/init.h>
#include <linux/irqchip.h>
#include <linux/seq_file.h>
#include <linux/ratelimit.h>

#include <linux/huawei/rdr_private.h>
#ifdef CONFIG_HISI_RDR
#ifdef CONFIG_HISI_RDR_SWITCH
static unsigned int curr_int_num = 0xffffffff;
static unsigned int int_switch_flag;
#endif

static rdr_funcptr_3 int_switch_hook;

void int_switch_hook_add(rdr_funcptr_3 p_hook_func)
{
	int_switch_hook = p_hook_func;
}
EXPORT_SYMBOL(int_switch_hook_add);

void int_switch_hook_delete(void)
{
	int_switch_hook = NULL;
}
EXPORT_SYMBOL(int_switch_hook_delete);
#else

/*y00145322*/

static unsigned int curr_int_num = 0xffffffff;
static rdr_funcptr_3 int_switch_hook;
static unsigned int int_switch_flag;

void int_switch_hook_add(rdr_funcptr_3 p_hook_func)
{
	int_switch_hook = p_hook_func;
}
EXPORT_SYMBOL(int_switch_hook_add);

void int_switch_hook_delete(void)
{
	int_switch_hook = NULL;
}
EXPORT_SYMBOL(int_switch_hook_delete);
#endif

unsigned long irq_err_count;

int arch_show_interrupts(struct seq_file *p, int prec)
{
#ifdef CONFIG_SMP
	show_ipi_list(p, prec);
#endif
	seq_printf(p, "%*s: %10lu\n", prec, "Err", irq_err_count);
	return 0;
}

/*
 * handle_IRQ handles all hardware IRQ's.  Decoded IRQs should
 * not come via this function.  Instead, they should provide their
 * own 'handler'.  Used by platform code implementing C-based 1st
 * level decoding.
 */
void handle_IRQ(unsigned int irq, struct pt_regs *regs)
{
	struct pt_regs *old_regs = set_irq_regs(regs);

#ifdef CONFIG_HISI_RDR
#ifdef CONFIG_HISI_RDR_SWITCH
	unsigned int old_int_num = curr_int_num;

	curr_int_num = irq;

	if (NULL != int_switch_hook) {/*exc int hook func*/
		int_switch_hook(0, old_int_num, curr_int_num);
		int_switch_flag = 1;
	}
#endif
#else
    /*y00145322*/
	unsigned int old_int_num = curr_int_num;

	curr_int_num = irq;

	if (NULL != int_switch_hook) {/*exc int hook func*/
		int_switch_hook(0, old_int_num, curr_int_num);
		int_switch_flag = 1;
	}
#endif


	irq_enter();

	/*
	 * Some hardware gives randomly wrong interrupts.  Rather
	 * than crashing, do something sensible.
	 */
	if (unlikely(irq >= nr_irqs)) {
		pr_warn_ratelimited("Bad IRQ%u\n", irq);
		ack_bad_irq(irq);
	} else {
		generic_handle_irq(irq);
	}

	irq_exit();

#ifdef CONFIG_HISI_RDR
#ifdef CONFIG_HISI_RDR_SWITCH
	/*call exception interrupt hook func*/
	if ((NULL != int_switch_hook) && (0 != int_switch_flag))
		int_switch_hook(1, old_int_num, curr_int_num);
#endif
#else
    /*y00145322*/
	/*call exception interrupt hook func*/
	if ((NULL != int_switch_hook) && (0 != int_switch_flag))
		int_switch_hook(1, old_int_num, curr_int_num);
#endif

	set_irq_regs(old_regs);
}

void __init set_handle_irq(void (*handle_irq)(struct pt_regs *))
{
	if (handle_arch_irq)
		return;

	handle_arch_irq = handle_irq;
}

void __init init_IRQ(void)
{
	irqchip_init();
	if (!handle_arch_irq)
		panic("No interrupt controller found.");
}

#ifdef CONFIG_HOTPLUG_CPU
static bool migrate_one_irq(struct irq_desc *desc)
{
	struct irq_data *d = irq_desc_get_irq_data(desc);
	const struct cpumask *affinity = d->affinity;
	struct irq_chip *c;
	bool ret = false;

	/*
	 * If this is a per-CPU interrupt, or the affinity does not
	 * include this CPU, then we have nothing to do.
	 */
	if (irqd_is_per_cpu(d) || !cpumask_test_cpu(smp_processor_id(), affinity))
		return false;

	if (cpumask_any_and(affinity, cpu_online_mask) >= nr_cpu_ids) {
		affinity = cpu_online_mask;
		ret = true;
	}

	c = irq_data_get_irq_chip(d);
	if (!c->irq_set_affinity)
		pr_debug("IRQ%u: unable to set affinity\n", d->irq);
	else if (c->irq_set_affinity(d, affinity, true) == IRQ_SET_MASK_OK && ret)
		cpumask_copy(d->affinity, affinity);

	return ret;
}

/*
 * The current CPU has been marked offline.  Migrate IRQs off this CPU.
 * If the affinity settings do not allow other CPUs, force them onto any
 * available CPU.
 *
 * Note: we must iterate over all IRQs, whether they have an attached
 * action structure or not, as we need to get chained interrupts too.
 */
void migrate_irqs(void)
{
	unsigned int i;
	struct irq_desc *desc;
	unsigned long flags;

	local_irq_save(flags);

	for_each_irq_desc(i, desc) {
		bool affinity_broken;

		raw_spin_lock(&desc->lock);
		affinity_broken = migrate_one_irq(desc);
		raw_spin_unlock(&desc->lock);

		if (affinity_broken)
			pr_warn_ratelimited("IRQ%u no longer affine to CPU%u\n",
					    i, smp_processor_id());
	}

	local_irq_restore(flags);
}
#endif /* CONFIG_HOTPLUG_CPU */
