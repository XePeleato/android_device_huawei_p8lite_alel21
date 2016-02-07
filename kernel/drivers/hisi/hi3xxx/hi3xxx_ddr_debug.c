/*
 *	drivers/hisi/hi3xxx/hi3xxx_ddr_debug.c
 *
 * Copyright (C) 2013 Hisilicon
 * License terms: GNU General Public License (GPL) version 2
 *
 */
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>

#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/mm_types.h>
#include <linux/rwsem.h>

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/of_address.h>

#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/pgtable-hwdef.h>

#include <asm/current.h>
#include <asm/thread_info.h>
#include <linux/thread_info.h>

#include <linux/kernel_stat.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <asm/io.h>
#include <asm/pgalloc.h>
#include <asm/tlb.h>
#include <asm/tlbflush.h>

static unsigned long virt_addr = 0;
static unsigned int virt_len = 0;
static unsigned int pid;

static ssize_t show_phys_pte(char *buf, ssize_t size, struct mm_struct *mm, unsigned long addr)
{
	pgd_t *pgd;
	ssize_t temp = size;

	size += snprintf(buf+size, PAGE_SIZE - size, "pgd = %p\n", mm->pgd);
	pgd = pgd_offset(mm, addr);
	size += snprintf(buf+size, PAGE_SIZE - size, "[%08lx] *pgd=%08llx",
			addr, (long long)pgd_val(*pgd));

	do {
		pud_t *pud;
		pmd_t *pmd;
		pte_t *pte;

		if (pgd_none(*pgd))
			break;

		if (pgd_bad(*pgd)) {
			size += snprintf(buf+size, PAGE_SIZE - size, "(bad)");
			break;
		}

		pud = pud_offset(pgd, addr);
		if (PTRS_PER_PUD != 1)
			size += snprintf(buf+size, PAGE_SIZE - size, ", *pud=%08llx", (long long)pud_val(*pud));

		if (pud_none(*pud))
			break;

		if (pud_bad(*pud)) {
			size += snprintf(buf+size, PAGE_SIZE - size, "(bad)");
			break;
		}

		pmd = pmd_offset(pud, addr);
		if (PTRS_PER_PMD != 1)
			size += snprintf(buf+size, PAGE_SIZE - size, ", *pmd=%08llx", (long long)pmd_val(*pmd));

		if (pmd_none(*pmd))
			break;

		if (pmd_bad(*pmd)) {
			size += snprintf(buf+size, PAGE_SIZE - size, "(bad)");
			break;
		}

		/* We must not map this if we have highmem enabled */
		if (PageHighMem(pfn_to_page(pmd_val(*pmd) >> PAGE_SHIFT)))
			break;

		pte = pte_offset_map(pmd, addr);
		size += snprintf(buf+size, PAGE_SIZE - size, ", *pte=%08llx", (long long)pte_val(*pte));
		pte_unmap(pte);
	} while(0);

	size += snprintf(buf+size, PAGE_SIZE - size, "\n");

	return size - temp;
}

static ssize_t show_phys_addr(struct device *dev, struct device_attribute *attr,
	char *buf)
{
	ssize_t size;
	unsigned long phys_addr;
	pgd_t *pgd_tmp = NULL;
	pud_t *pud_tmp = NULL;
	pmd_t *pmd_tmp = NULL;
	pte_t *pte_tmp = NULL;
	int i;
	unsigned long va_addr = virt_addr;

	size = snprintf(buf, PAGE_SIZE, "%u, 0x%lx, %u\n", pid, virt_addr, virt_len);

	struct task_struct *tgt_task = NULL;

	if (pid == 0) {
		tgt_task = current;
	} else {
		tgt_task = pid_task(find_vpid(pid), PIDTYPE_PID);
	}
	/*tgt_task = find_task_by_vpid(pid);*/

	if(!tgt_task) {
		size += snprintf(buf+size, PAGE_SIZE - size, "[ddr_debug] tgt_task is NULL!\n");
		return ;
	}

	for(i = (virt_len*1024/PAGE_SIZE + 1);i > 0;i--)
	{
		if(!find_vma(tgt_task->mm, va_addr)) {
			size += snprintf(buf+size, PAGE_SIZE - size, "[ddr_debug] cannot find vma!\n");
			return ;
		}
		size += snprintf(buf+size, PAGE_SIZE - size, "[ddr_debug]i:%d\n", i);

		size += show_phys_pte(buf, size, tgt_task->mm, va_addr);

		va_addr += PAGE_SIZE;
	}

#if 0
	size += snprintf(buf + size, PAGE_SIZE - size, "task_struct = 0x%lx\n", tgt_task);
	tgt_task = find_task_by_vpid(pid);

	if(!find_vma(tgt_task->mm, virt_addr)) {
		size += snprintf(buf + size, PAGE_SIZE - size, "virt_addr 0x%lx not available.\n", virt_addr);
		return 0;
	}

	pgd_tmp = pgd_offset(tgt_task->mm, virt_addr);
	size += snprintf(buf + size, PAGE_SIZE - size, "pgd_tmp = 0x%p\n", pgd_tmp);

	if(pgd_none(*pgd_tmp)){
		size += snprintf(buf + size, PAGE_SIZE - size, "not mapped in pgd.\n");
		return 0;
	}

	pud_tmp = pud_offset(pgd_tmp, virt_addr);
	size += snprintf(buf + size, PAGE_SIZE - size, "pud_tmp = 0x%p\n", pud_tmp);

	if(pud_none(*pud_tmp)){
		size += snprintf(buf + size, PAGE_SIZE - size, "not mapped in pud.\n");
		return 0;
	}

	pmd_tmp = pmd_offset(pud_tmp, virt_addr);
	size += snprintf(buf + size, PAGE_SIZE - size, "pmd_tmp = 0x%p\n", pmd_tmp);
	if(pmd_none(*pmd_tmp)){
		size += snprintf(buf + size, PAGE_SIZE - size, "not mapped in pmd.\n");
		return 0;
	}

	pte_tmp = pte_offset_kernel(pmd_tmp, virt_addr);
	size += snprintf(buf + size, PAGE_SIZE - size, "pte_tmp = 0x%p\n", pte_tmp);
	size += snprintf(buf + size, PAGE_SIZE - size, "*pte_tmp = 0x%p\n", (void *)*pte_tmp);

	if(pte_none(*pte_tmp)){
		size += snprintf(buf + size, PAGE_SIZE - size, "not mapped in pte.\n");
		return 0;
	}

	if(!pte_present(*pte_tmp)){
		size += snprintf(buf + size, PAGE_SIZE - size, "pte not in RAM.\n");
		return 0;

	}

	phys_addr = (pte_val(*pte_tmp) & PAGE_MASK) | (virt_addr & ~PAGE_MASK);
	size += snprintf(buf + size, PAGE_SIZE - size, "phys_addr = 0x%lx\n", phys_addr);
#endif

	return size;
}

void transfer_virt_addr_to_phys_addr(unsigned int pid,unsigned long virt_addr,unsigned int virt_len)
{
	int i;
	unsigned long va_addr = virt_addr;

	pr_info("[ddr_debug] pid:%u, va:0x%lx, %u\n", pid, virt_addr, virt_len);

	struct task_struct *tgt_task = NULL;

	if (pid == 0) {
		tgt_task = current;
	} else {
		tgt_task = pid_task(find_vpid(pid), PIDTYPE_PID);
	}
	/*tgt_task = find_task_by_vpid(pid);*/

	if(!tgt_task) {
		pr_err("[ddr_debug] tgt_task is NULL!\n");
		return ;
	}

	for(i = (virt_len*1024/PAGE_SIZE + 1);i > 0;i--)
	{
		if(!find_vma(tgt_task->mm, va_addr)) {
			pr_err("[ddr_debug] cannot find vma!\n");
			return ;
		}
		pr_info("[ddr_debug]i:%d\n", i);
		show_pte(tgt_task->mm, va_addr);

		va_addr += PAGE_SIZE;
	}

	return ;
}

static ssize_t store_virt_addr(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t count)
{
	unsigned int args = 0, argl = 0;
	unsigned int len = count;
	const char *head = buf;
	char *p;

	while ((len > 0) && (args < 3)) {
		p = memchr(head, ' ', len);
		argl = p ? p - head : 0;

		switch (args) {
		case 0:
			sscanf(head, "%u", &pid);
			break;
		case 1:
			sscanf(head, "%lx", &virt_addr);
			break;
		case 2:
			sscanf(head, "%u", &virt_len);
			break;
		default:
			pr_err("[%s]:%d invalid args!\n", __func__, __LINE__);
		}

		head = head + argl + 1;
		len = len - argl - 1;
		args++;
	}

	transfer_virt_addr_to_phys_addr(pid, virt_addr, virt_len);

	return count;
}

static DEVICE_ATTR(virt_to_phys, 0644, show_phys_addr, store_virt_addr);

static struct attribute *ddr_debug_attrs[] = {
	&dev_attr_virt_to_phys.attr,
	NULL,
};

static struct attribute_group ddr_debug_attr_grp = {
	.attrs = ddr_debug_attrs,
};

static int ddr_debug_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	int ret;

	pr_info("[%s]:%d enter.\n", __func__, __LINE__);

	ret = sysfs_create_group(&(pdev->dev.kobj), &ddr_debug_attr_grp);
	if (ret) {
		pr_err("Failed to create sysfs ddr_debug_attr_grp\n");
		return ret;
	}

	pr_info("[%s]:%d leave.\n", __func__, __LINE__);

	return ret;
}

static int ddr_debug_remove(struct platform_device *pdev)
{
	return 0;
}

#define MODULE_NAME		"hisilicon,ddr_debug"

static const struct of_device_id ddr_debug_match[] = {
	{ .compatible = MODULE_NAME },
	{},
};

static struct platform_driver ddr_debug_drv = {
	.probe		= ddr_debug_probe,
	.remove		= ddr_debug_remove,
	.driver = {
		.name	= MODULE_NAME,
		.of_match_table	= of_match_ptr(ddr_debug_match),
	},
};

static int __init ddr_debug_init(void)
{
	return platform_driver_register(&ddr_debug_drv);
}
module_init(ddr_debug_init);

static void __exit ddr_debug_exit(void)
{
	platform_driver_unregister(&ddr_debug_drv);
}
module_exit(ddr_debug_exit);

MODULE_AUTHOR("cuiyong1@huawei.com>");
MODULE_DESCRIPTION("DDR DEBUG MODULE");
MODULE_LICENSE("GPL v2");
