/*
 *  kernel/drivers/hisi/memory_dump/memory_dump.c
 *
 * balong memory/register proc-fs  dump implementation
 *
 * Copyright (C) 2012 Hisilicon, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 #include <linux/stddef.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/interrupt.h>
#include <linux/pagemap.h>
#include <linux/jiffies.h>
#include <linux/bootmem.h>
#include <linux/memblock.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/kmemcheck.h>
#include <linux/module.h>
#include <linux/suspend.h>
#include <linux/pagevec.h>
#include <linux/blkdev.h>
#include <linux/slab.h>
#include <linux/ratelimit.h>
#include <linux/oom.h>
#include <linux/notifier.h>
#include <linux/topology.h>
#include <linux/sysctl.h>
#include <linux/cpu.h>
#include <linux/cpuset.h>
#include <linux/memory_hotplug.h>
#include <linux/nodemask.h>
#include <linux/vmalloc.h>
#include <linux/vmstat.h>
#include <linux/mempolicy.h>
#include <linux/stop_machine.h>
#include <linux/sort.h>
#include <linux/pfn.h>
#include <linux/backing-dev.h>
#include <linux/fault-inject.h>
#include <linux/page-isolation.h>
#include <linux/page_cgroup.h>
#include <linux/debugobjects.h>
#include <linux/kmemleak.h>
#include <linux/compaction.h>
#include <trace/events/kmem.h>
#include <linux/ftrace_event.h>
#include <linux/memcontrol.h>
#include <linux/prefetch.h>
#include <linux/migrate.h>
#include <linux/page-debug-flags.h>
#include <linux/hugetlb.h>
#include <linux/sched/rt.h>
#include <linux/pasr.h>

#include <asm/tlbflush.h>
#include <asm/div64.h>
#include "memory_dump.h"

#define MAX_LEN_OF_MEMDUMP_ADDR_STR  (30)
#define TRANSFER_BASE    (16)

static struct mem_dump_cb *cb;

extern struct memblock memblock;
extern struct mm_struct init_mm;
extern char _text[];
extern char _end[];

static int g_memdump_reserved_addr=0;


static int __init early_parse_memdumpresevred_cmdline(char *p)
{
    char memdump_reserved_addr[MAX_LEN_OF_MEMDUMP_ADDR_STR];

    char *endptr = NULL;

    memcpy(memdump_reserved_addr, p, 10);
    memdump_reserved_addr[10] = 0;

	
    g_memdump_reserved_addr = simple_strtoul(memdump_reserved_addr, &endptr, TRANSFER_BASE);

    pr_err("[early_parse_memdumpresevred_cmdline] p:%s, g_memdump_reserved_addr:0x%lx \n", (const char *)p, (unsigned long)g_memdump_reserved_addr);


    return 0;
}
early_param("memdump_reserved", early_parse_memdumpresevred_cmdline);

int mem_dump_init(void)
{
	unsigned char *cb_reserve_mem = NULL;
	u32 val;
	int i=0;
	struct mem_dump_cb *cb;
	struct memblock_type *print_mb_cb;

	if(0 == g_memdump_reserved_addr)
	{
		    pr_err("kernel dump cmdline not transfer \n");
		    return 0;
	}
	
	cb_reserve_mem = ioremap_wc((unsigned char *)g_memdump_reserved_addr, PAGE_SIZE);

	cb = (struct mem_dump_cb *)kmalloc(sizeof(struct mem_dump_cb), GFP_KERNEL);

	printk(KERN_ERR"%s begin, 0x%x,0x%llx!\r\n", __FUNCTION__,g_memdump_reserved_addr,(unsigned long long)cb);
	if (NULL == cb){
		printk(KERN_ERR"%s kmalloc fail !\r\n", __FUNCTION__);
		return -1;
	}
	cb->page_shift = PAGE_SHIFT;
	cb->struct_page_size = sizeof(struct page);
	cb->phys_offset = PHYS_OFFSET;
	cb->page_offset = PAGE_OFFSET;
	cb->img_base = virt_to_phys(_text);
	cb->img_size = ALIGN(_end -_text, PAGE_SIZE);
	printk("_text:0x%p _end:0x%p\n", _text, _end);	

#ifdef CONFIG_FLATMEM
	cb->page = mem_map;
	cb->pfn_offset = PHYS_PFN_OFFSET;
	cb->section_size = 0;
#elif CONFIG_SPARSEMEM_VMEMMAP
	cb->page = vmemmap;
	cb->pfn_offset = 0;
	cb->pmd_size = PMD_SIZE;
	cb->section_size = 1UL << SECTION_SIZE_BITS;
#else
#error "Configurations other than CONFIG_PLATMEM and CONFIG_SPARSEMEM_VMEMMAP are not supported"
#endif
    #ifdef CONFIG_64BIT
       /*Subtract the base address that TTBR1 maps*/
	cb->kern_map_offset = (UL(0xffffffffffffffff) << VA_BITS);
    #else
	cb->kern_map_offset = 0;
    #endif 

	cb->ttbr = virt_to_phys(init_mm.pgd);
	printk("%s: ttbr:%p\n", __FUNCTION__, (void *)cb->ttbr);

	cb->mb_cb = virt_to_phys(&memblock.memory);
	print_mb_cb = &memblock.memory;
	cb->mbr_size = sizeof(struct memblock_region);
	memcpy(cb_reserve_mem, cb, sizeof(struct mem_dump_cb));
	printk("cb->page is 0x%llx\n", cb->page);
	printk("cb->page_shift is 0x%x\n", cb->page_shift);
	printk("cb->struct_page_size is 0x%x\n", cb->struct_page_size);
	printk("cb->phys_offset is 0x%llx\n", cb->phys_offset);
	printk("cb->page_offset is 0x%llx\n", cb->page_offset);
	printk("cb->pfn_offset is 0x%llx\n", cb->pfn_offset);
	printk("cb->ttbr is 0x%llx\n", cb->ttbr);
	printk("cb->mb_cb is 0x%llx\n", cb->mb_cb);
	printk("cb->section_size is 0x%llx\n", cb->section_size);
	printk("cb->pmd_size is 0x%llx\n", cb->pmd_size);
	printk("cb->mbr_size is 0x%x\n", cb->mbr_size);
	printk("cb->kern_map_offset is 0x%llx\n", cb->kern_map_offset);

	printk("mem_map is %p\n", mem_map);
	printk("vmemmap is %p\n", vmemmap);
	for (i=0; i< print_mb_cb->cnt; i++) {
		printk("print_mb_cb->regions is 0x%p\n", print_mb_cb->regions->base);
		printk("print_mb_cb->regions is 0x%p\n", print_mb_cb->regions->size);
	}

	return 0;
}
early_initcall(mem_dump_init);


