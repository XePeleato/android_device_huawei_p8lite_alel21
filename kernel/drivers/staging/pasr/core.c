/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Maxime Coquelin <maxime.coquelin@stericsson.com> for ST-Ericsson.
 * License terms:  GNU General Public License (GPL), version 2
 */

#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/pasr.h>

#include "helper.h"

#ifdef CONFIG_ARCH_HI6XXX
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#endif

enum pasr_state {
	PASR_REFRESH,
	PASR_NO_REFRESH,
};

struct pasr_fw {
	struct pasr_map *map;
};

static struct pasr_fw pasr;

void pasr_update_mask(struct pasr_section *section, enum pasr_state state)
{
	struct pasr_die *die = section->die;
	phys_addr_t addr = section->start - die->start;
	u8 bit = addr / die->section_size;
    unsigned int old_mem_reg = die->mem_reg;
    //printk(KERN_ERR"%s:%d: rank: %d; section->start:0x%x; section->size:0x%x state: %s;\n", __func__, __LINE__, die->cookie, section->start, die->section_size, state == PASR_REFRESH ? "PASR_REFRESH":"PASR_NO_REFRESH");
	if (state == PASR_REFRESH)
		die->mem_reg &= ~(1 << bit);    /* need refresh */
	else
		die->mem_reg |= (1 << bit);     /* no need refresh, that means we can throw away the data of this segment */

    //printk(KERN_ERR"die: 0x%x; die->mem_reg: 0x%x -> 0x%x\n", die, old_mem_reg, die->mem_reg);
	//pr_debug("%s(): %s refresh section 0x%08x. Die%d mem_reg = 0x%08x\n"
			//, __func__, state == PASR_REFRESH ? "Start" : "Stop"
			//, section->start, die->idx, die->mem_reg);
	//pr_debug("%s(): bit = %d\n", __func__, bit);
	if (die->apply_mask)
    {
        //printk(KERN_ERR"%s:%d: now apply_mask, %d, %d\n", __func__, __LINE__, die->mem_reg, die->cookie);
		die->apply_mask(&die->mem_reg, die->cookie);
    }

	return;
}

void pasr_put(phys_addr_t paddr, unsigned long size)
{
	struct pasr_section *s;
	unsigned long cur_sz;
	unsigned long flags = 0;

	if (!pasr.map) {
		/*WARN_ONCE(1, KERN_INFO"%s(): Map not initialized.\n"
			"\tCommand line parameters missing or incorrect\n"
			, __func__);*/
		goto out;
	}

	//pr_err("%s: paddr=0x%08x, size=0x%08lx\n", __func__, paddr, size);

	do {
		s = pasr_addr2section(pasr.map, paddr);
		if (!s)
			goto out;

		cur_sz = ((paddr + size) < (s->start + s->die->section_size)) ?
			size : s->start + s->die->section_size - paddr;

		if (s->lock)
			spin_lock_irqsave(s->lock, flags);

		s->free_size += cur_sz;
		BUG_ON(s->free_size > s->die->section_size);

		if (s->free_size < s->die->section_size)
			goto unlock;

		if (!s->pair)
			pasr_update_mask(s, PASR_NO_REFRESH);
		else if (s->pair->free_size == s->die->section_size) {
				pasr_update_mask(s, PASR_NO_REFRESH);
				pasr_update_mask(s->pair, PASR_NO_REFRESH);
		}
unlock:
		if (s->lock)
			spin_unlock_irqrestore(s->lock, flags);

		paddr += cur_sz;
		size -= cur_sz;
	} while (size);

out:
	return;
}

void pasr_get(phys_addr_t paddr, unsigned long size)
{
	unsigned long flags = 0;
	unsigned long cur_sz;
	struct pasr_section *s;

	if (!pasr.map) {
		/*WARN_ONCE(1, KERN_INFO"%s(): Map not initialized.\n"
			"\tCommand line parameters missing or incorrect\n"
			, __func__);*/
		return;
	}

	//pr_debug("%s: paddr=0x%08x, size=0x%08lx\n", __func__, paddr, size);

	do {
		s = pasr_addr2section(pasr.map, paddr);
		if (!s)
			goto out;

		cur_sz = ((paddr + size) < (s->start + s->die->section_size)) ?
			size : s->start + s->die->section_size - paddr;

		if (s->lock)
			spin_lock_irqsave(s->lock, flags);

		if (s->free_size < s->die->section_size)
			goto unlock;

		if (!s->pair)
			pasr_update_mask(s, PASR_REFRESH);
		else if (s->pair->free_size == s->die->section_size) {
				pasr_update_mask(s, PASR_REFRESH);
				pasr_update_mask(s->pair, PASR_REFRESH);
		}
unlock:
		BUG_ON(cur_sz > s->free_size);
		s->free_size -= cur_sz;

		if (s->lock)
			spin_unlock_irqrestore(s->lock, flags);

		paddr += cur_sz;
		size -= cur_sz;
	} while (size);

out:
	return;
}

int pasr_register_mask_function(phys_addr_t addr, void *function, void *cookie)
{
	struct pasr_die *die = pasr_addr2die(pasr.map, addr);

	if (!die) {
		pr_err("%s: No DDR die corresponding to address 0x%08x\n",
				__func__, addr);
		return -EINVAL;
	}

	if (addr != die->start)
		pr_warning("%s: Addresses mismatch (Die = 0x%08x, addr = 0x%08x\n"
				, __func__, die->start, addr);

	die->cookie = cookie;
	die->apply_mask = function;
    printk(KERN_ERR"%s:%d: die: 0x%x; addr: 0x%x; die->cookie: %d; die->mem_reg: 0x%x\n", 
            __func__, __LINE__, die, addr, die->cookie, die->mem_reg);
	die->apply_mask(&die->mem_reg, die->cookie);

	return 0;
}

int __init pasr_init_core(struct pasr_map *map)
{
	pasr.map = map;
	return 0;
}

