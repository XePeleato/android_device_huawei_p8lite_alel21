/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#define pr_fmt(fmt) "OF_HISI_ION  " fmt
#include <linux/export.h>
#include <linux/err.h>
#include <linux/hisi/hisi_ion.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/sched.h>
#include <linux/rwsem.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/dma-contiguous.h>
#include <asm/cacheflush.h>
#include <linux/uaccess.h>
#include "../ion_priv.h"
#include <linux/memblock.h>
#include <linux/ion.h>
#include <linux/seq_file.h>
#include <linux/hisi/util.h>

#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include <linux/hisi/pm/pwrctrl_multi_def.h>

#ifndef CONFIG_ARM64
#include <asm/outercache.h>
#endif

struct hisi_ion_name_id_table {
    const char *name;
    unsigned int id;
};

static struct hisi_ion_name_id_table name_id_table[] __initdata = {
    {"fb", ION_FB_HEAP_ID},
    {"vpu", ION_VPU_HEAP_ID},
    {"jpu", ION_JPU_HEAP_ID},
    {"gralloc-carveout", ION_GRALLOC_HEAP_ID},
    {"overlay", ION_OVERLAY_HEAP_ID},
    {"sys_user", ION_SYSTEM_HEAP_ID},
    {"sys_contig", ION_SYSTEM_CONTIG_HEAP_ID},
};

struct hisi_ion_type_id_table {
    const char *name;
    enum ion_heap_type type;
};

static struct hisi_ion_type_id_table type_id_table[] = {
    {"ion_system_contig", ION_HEAP_TYPE_SYSTEM_CONTIG},
    {"ion_system", ION_HEAP_TYPE_SYSTEM},
    {"ion_carveout", ION_HEAP_TYPE_CARVEOUT},
    {"ion_chunk", ION_HEAP_TYPE_CHUNK},
    {"ion_dma", ION_HEAP_TYPE_DMA},
    {"ion_cpudraw", ION_HEAP_TYPE_CPUDRAW},
    {"ion_custom", ION_HEAP_TYPE_CUSTOM},
    {"reserved", ION_HEAP_TYPE_RESERVED},
};

#define HISI_ION_HEAP_NUM 16
#define FREQDUMP_ACPU_ONLINE_OFFSET 12
#define CLUSTER1_CPU4 	1 << 4
#define CLUSTER1_STAT 	1 << 1

static struct ion_platform_data hisi_ion_platform_data = {0};
static struct ion_platform_heap hisi_ion_platform_heap[HISI_ION_HEAP_NUM] = {{0}};

static struct ion_device *hisi_ion_device = NULL;
static struct ion_heap *hisi_ion_heap[HISI_ION_HEAP_NUM] = {NULL};

int hisi_ion_get_heap_info(unsigned int id, struct ion_heap_info_data* data)
{
    int i;

    BUG_ON(!data);
    
    for(i = 0; i < hisi_ion_platform_data.nr; i++){
        if (hisi_ion_platform_heap[i].type!= ION_HEAP_TYPE_RESERVED)
            continue;

        if (hisi_ion_platform_heap[i].id == id){
            data->heap_phy  = hisi_ion_platform_heap[i].base;
            data->heap_size = hisi_ion_platform_heap[i].size;
            strncpy((void* )data->name, (void* )hisi_ion_platform_heap[i].name, HISI_ION_NAME_LEN);
            pr_info("heap info : id %d name %s phy 0x%#x size %#x\n",
                id, data->name, data->heap_phy, data->heap_size);
            return 0;
        }
    }
    pr_err("in %s please check the id %d\n",__func__, id);

    return -EINVAL;
}
EXPORT_SYMBOL(hisi_ion_get_heap_info);

struct ion_device *get_ion_device(void)		
{		
    return hisi_ion_device;		
}		
EXPORT_SYMBOL(get_ion_device);


static int __init get_id_by_name(const char *name, unsigned int *id)
{
    int i, n;

    n = sizeof(name_id_table)/sizeof(name_id_table[0]);
    for (i = 0; i< n ;i++){
        if (strncmp(name, name_id_table[i].name, HISI_ION_NAME_LEN))
            continue;

        *id = name_id_table[i].id;
        return 0;
    }
    return -1;
}

static const char *get_type_by_id(enum ion_heap_type type)
{
    int i, n;
    const char *unknown = "unknown";

    n = sizeof(type_id_table)/sizeof(type_id_table[0]);
    for (i = 0; i< n ; i++){
        if (type_id_table[i].type == type)
            return type_id_table[i].name;
    }
    
    return unknown;
}

static int __init get_type_by_name(const char *name, enum ion_heap_type *type)
{
    int i, n;

    n = sizeof(type_id_table)/sizeof(type_id_table[0]);
    for (i = 0; i< n ;i++){
        if (strncmp(name, type_id_table[i].name, HISI_ION_NAME_LEN))
            continue;
        
        *type = type_id_table[i].type;
        return 0;
    }
    
    return -1;
}

static int __init of_hisi_ion_setup_platform_data(struct platform_device *dev)
{
	struct device_node *node, *np;
    const char *heap_name;
    const char *type_name;
    unsigned int base, size, id;
    enum ion_heap_type type;
    int ret;
    int index = 0;
    
	node = dev->dev.of_node;
	for_each_child_of_node(node, np) {
		ret = of_property_read_string(np, "heap-name", &heap_name);
		if (ret < 0){
            pr_err("in node %s please check the name property of node %s\n",__func__, np->name);
            continue;
        }
        
        ret = get_id_by_name(heap_name, &id);
		if (ret < 0){
            pr_err("in node %s please check the name %s\n",__func__, heap_name);
            continue;
        }

		ret = of_property_read_u32(np, "heap-base", &base);
		if (ret < 0){
            pr_err("in node %s please check the base property of node %s\n",__func__, np->name);
			continue;
        }
        
		ret = of_property_read_u32(np, "heap-size", &size);
		if (ret < 0){
            pr_err("in node %s please check the size property of node %s\n",__func__, np->name);
            continue;
        }

		ret = of_property_read_string(np, "heap-type", &type_name);
		if (ret < 0){
            pr_err("in node %s please check the type property of node %s\n",__func__, np->name);
            continue;
        }
        
        ret = get_type_by_name(type_name, &type);
		if (ret < 0){
            pr_err("in node %s please check the type %s\n",__func__, type_name);
            continue;
        }
        pr_info("ion_platform_heap index %d : name %s base 0x%x size 0x%x id %d type %d\n",
            index, heap_name, base, size, id, type);

        hisi_ion_platform_heap[index].name = heap_name;
        hisi_ion_platform_heap[index].base = base;
        hisi_ion_platform_heap[index].size = size;
        hisi_ion_platform_heap[index].id = id;
        hisi_ion_platform_heap[index].type = type;
        index ++;
	}

    hisi_ion_platform_data.nr = index;
    hisi_ion_platform_data.heaps = hisi_ion_platform_heap;

    return 0;
}

static int check_vaddr_bounds(unsigned long start, unsigned long end)
{
	struct mm_struct *mm = current->active_mm;
	struct vm_area_struct *vma;
	int ret = 1;

	if(end < start)
		goto out;

	vma = find_vma(mm, start);
	if(vma && (vma->vm_start < end)){
		if(start < vma->vm_start)
			goto out;
		if(end > vma->vm_end)
			goto out;
		ret = 0;
	}
out:
	return ret;
}

#ifndef CONFIG_ARM64
static int ion_no_pages_cache_ops(struct ion_client *client,
			struct ion_handle *handle,
			void *vaddr,
			unsigned int offset, unsigned int length,
			unsigned int cmd)
{
	unsigned long size_to_vmap, total_size;
	int i, j, ret;
	void *ptr = NULL;
	ion_phys_addr_t buff_phys = 0;
	ion_phys_addr_t buff_phys_start = 0;
	size_t buf_length = 0;

	ret = ion_phys(client, handle, &buff_phys_start, &buf_length);
	if (ret)
		return -EINVAL;

	buff_phys = buff_phys_start;

	if (!vaddr) {
		/*
		 * Split the vmalloc space into smaller regions in
		 * order to clean and/or invalidate the cache.
		 */
		size_to_vmap = ((VMALLOC_END - VMALLOC_START)/8);
		total_size = buf_length;

		for (i = 0; i < total_size; i += size_to_vmap) {
			size_to_vmap = min(size_to_vmap, total_size - i);
			for (j = 0; j < 10 && size_to_vmap; ++j) {
				ptr = ioremap(buff_phys, size_to_vmap);
				if (ptr) {
					switch (cmd) {
					case ION_HISI_CLEAN_CACHES:
						dmac_clean_range(ptr,
							ptr + size_to_vmap);
						break;
					case ION_HISI_INV_CACHES:
						dmac_inv_range(ptr,
							ptr + size_to_vmap);
						break;
					case ION_HISI_CLEAN_INV_CACHES:
						dmac_flush_range(ptr,
							ptr + size_to_vmap);
						break;
					default:
						return -EINVAL;
					}
					buff_phys += size_to_vmap;
					break;
				} else {
					size_to_vmap >>= 1;
				}
			}
			if (!ptr) {
				pr_err("Couldn't io-remap the memory\n");
				return -EINVAL;
			}
			iounmap(ptr);
		}
	} else {
		switch (cmd) {
		case ION_HISI_CLEAN_CACHES:
			dmac_clean_range(vaddr, vaddr + length);
			break;
		case ION_HISI_INV_CACHES:
			dmac_inv_range(vaddr, vaddr + length);
			break;
		case ION_HISI_CLEAN_INV_CACHES:
			dmac_flush_range(vaddr, vaddr + length);
			break;
		default:
			return -EINVAL;
		}
	}

	return 0;
}

static int ion_pages_cache_ops(struct ion_client *client,
			struct ion_handle *handle,
			void *vaddr, unsigned int offset, unsigned int length,
			unsigned int cmd)
{
	struct sg_table *table = NULL;

	table = ion_sg_table(client, handle);
	if (IS_ERR_OR_NULL(table))
		return PTR_ERR(table);

	switch (cmd) {
	case ION_HISI_CLEAN_CACHES:
		if (!vaddr)
			dma_sync_sg_for_device(NULL, table->sgl,
				table->nents, DMA_TO_DEVICE);
		else
			dmac_clean_range(vaddr, vaddr + length);
		break;
	case ION_HISI_INV_CACHES:
		if (!vaddr)
			dma_sync_sg_for_cpu(NULL, table->sgl,
				table->nents, DMA_FROM_DEVICE);
		else
			dmac_inv_range(vaddr, vaddr + length);
		break;
	case ION_HISI_CLEAN_INV_CACHES:
		if (!vaddr) {
			dma_sync_sg_for_device(NULL, table->sgl,
				table->nents, DMA_TO_DEVICE);
			dma_sync_sg_for_cpu(NULL, table->sgl,
				table->nents, DMA_FROM_DEVICE);
		} else {
			dmac_flush_range(vaddr, vaddr + length);
		}
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

int ion_do_cache_op(struct ion_client *client, struct ion_handle *handle,
			void *uaddr, unsigned long offset, unsigned long len,
			unsigned int cmd)
{
	int ret = -EINVAL;
	unsigned long flags;
	struct sg_table *table;
	struct page *page;

	ret = ion_handle_get_flags(client, handle, &flags);
	if (ret)
		return -EINVAL;

	if (!ION_IS_CACHED(flags))
		return 0;

	table = ion_sg_table(client, handle);

	if (IS_ERR_OR_NULL(table))
		return PTR_ERR(table);

	page = sg_page(table->sgl);

	if (page)
		ret = ion_pages_cache_ops(client, handle, uaddr,
					offset, len, cmd);
	else
		ret = ion_no_pages_cache_ops(client, handle, uaddr,
					offset, len, cmd);

	return ret;

}
#endif

static long hisi_ion_custom_ioctl(struct ion_client *client, unsigned int cmd,
				            unsigned long arg)
{
	int ret = 0;

	switch (cmd) {
    	case ION_HISI_CUSTOM_PHYS:
    	{
    		struct ion_phys_data data;
    		struct ion_handle *handle;
        	ion_phys_addr_t phys;
        	size_t size;
		
    		if (copy_from_user(&data, (void __user *)arg,
    				sizeof(data))) {
    			return -EFAULT;
    		}

    		handle = ion_import_dma_buf(client, data.fd_buffer);

    		if (IS_ERR(handle)) {
    			pr_err("%s %d : handle %p phy_h 0x%x phy_h 0x%x size 0x%x fd 0x%x\n", __FUNCTION__,
    				   __LINE__, handle, data.phys_h, data.phys_l,
    				  data.size,(unsigned int)data.fd_buffer);
    			return PTR_ERR(handle);
    		}			
		
    		ret = ion_phys(client, handle, &phys, &size);
    		if (ret)
    		{
    			ion_free(client, handle);
    			return ret;
            }
    		data.phys_h = (unsigned int)((unsigned long long)phys >> 32);
    		data.phys_l = (unsigned int )(phys & 0xffffffff);
    		data.size = (unsigned int)size;
    		if (copy_to_user((void __user *)arg, &data, sizeof(data)))
    		{
    			ion_free(client, handle);
    			return -EFAULT;
    		}
    		ion_free(client, handle);

    		break;
    	}

#ifdef CONFIG_ARM64
	case ION_HISI_CLEAN_CACHES:
	case ION_HISI_CLEAN_INV_CACHES:
	{
		struct ion_flush_data data;
		if (copy_from_user(&data, (void __user *)arg,
				sizeof(data))) {
			return -EFAULT;
		}
		ion_sync_for_device(client, data.fd);
		break;
	}
	case ION_HISI_INV_CACHES:
	{
		struct ion_flush_data data;
		if (copy_from_user(&data, (void __user *)arg,
				sizeof(data))) {
			return -EFAULT;
		}
		ion_sync_for_cpu(client, data.fd);
		break;
	}
#else
	case ION_HISI_CLEAN_CACHES:
	case ION_HISI_INV_CACHES:
	case ION_HISI_CLEAN_INV_CACHES:
    {
		struct ion_flush_data data;
		unsigned long start, end;
		struct ion_handle *handle = NULL;
		struct mm_struct *mm = current->active_mm;

		if (copy_from_user(&data, (void __user *)arg,
				sizeof(data))) {
			return -EFAULT;
		}

		handle = ion_import_dma_buf(client, data.fd);
		if (IS_ERR(handle)) {
			pr_info("%s: Could not import handle: %p\n",
				__func__, handle);
			return -EINVAL;
		}

		down_read(&mm->mmap_sem);

		start = (unsigned long) data.vaddr;
		end = (unsigned long) data.vaddr
			+ data.length;

		if (start && check_vaddr_bounds(start, end)) {
			pr_err("%s: virtual address %p is out of bounds\n",
				__func__, data.vaddr);
			ret = -EINVAL;
		} else {
			ret = ion_do_cache_op(
				client, handle, data.vaddr,
				data.offset,
				data.length, cmd);
		}
		up_read(&mm->mmap_sem);

		ion_free(client, handle);

		if (ret < 0)
			return ret;
		break;
	}
#endif
	default:
		return -ENOTTY;
	}

	return ret;
}

static unsigned int *cpuidle_stat = NULL;
static unsigned int *clusteridle_stat = NULL;

void ion_pm_init(void)
{
	static void __iomem * addr;
	addr = ioremap(MEMORY_FREQDUMP_ADDR, MEMORY_FREQDUMP_SIZE);
	clusteridle_stat = ioremap(ACPU_CLUSTER_IDLE_ADDR, sizeof(int));
	cpuidle_stat = (unsigned int *)(addr + FREQDUMP_ACPU_ONLINE_OFFSET);
	return;
}

static void ion_flush_cache_all(void *dummy)
{
	flush_cache_all();

	return;
}

void hi6xxx_ion_flush_cache_all(void)
{
	int cpu;
	unsigned int stat = 0;
	cpumask_t mask;
	preempt_disable();

	stat = readl(cpuidle_stat) & 0xff;

	if(0 == (readl(clusteridle_stat) & CLUSTER1_STAT)){
		stat = stat | CLUSTER1_CPU4;
	}

	cpumask_clear(&mask);

	for_each_online_cpu(cpu) {
		if (stat & (1 << cpu))
			cpumask_set_cpu(cpu, &mask);
	}

	on_each_cpu_mask(&mask, ion_flush_cache_all, NULL, 1);
	preempt_enable();

	return;
}


static int __init hisi_ion_probe(struct platform_device *pdev)
{
	int i, err;
	struct ion_heap *heap;
	struct ion_platform_heap *heap_data;

	if (of_hisi_ion_setup_platform_data(pdev)){
		pr_err("of_hisi_ion_build_media_mem_body is failed\n");
		return -EINVAL;
	}

	hisi_ion_device = ion_device_create(&hisi_ion_custom_ioctl);
	if (IS_ERR_OR_NULL(hisi_ion_device)) {
		return PTR_ERR(hisi_ion_device);
	}
	/* 
	 * create the heaps as specified in the board file
	 */
	for (i = 0; i < hisi_ion_platform_data.nr; i++) {
		heap_data = &hisi_ion_platform_data.heaps[i];
		if (heap_data->type == ION_HEAP_TYPE_RESERVED)
			continue;

		heap = ion_heap_create(heap_data);
		if (IS_ERR_OR_NULL(heap)) {
			err = PTR_ERR(heap);
			goto out;
		}

		ion_device_add_heap(hisi_ion_device, heap);
		hisi_ion_heap[i] = heap;

		pr_info("%s: adding heap %s of type %d with %lx@%lx\n",
				__func__, heap_data->name, heap_data->type,
				heap_data->base, (unsigned long)heap_data->size);
	}
	platform_set_drvdata(pdev, hisi_ion_device);

	ion_pm_init();

	return 0;
out:
	for (i = 0; i < HISI_ION_HEAP_NUM; i++) {
		if (!hisi_ion_heap[i])
            continue;
		ion_heap_destroy(hisi_ion_heap[i]);
        hisi_ion_heap[i] = NULL;
	}
	return err;
}


static int __init hisi_ion_iommu_probe(struct platform_device *pdev)
{
    return hisi_ion_probe(pdev);
}

static int hisi_ion_remove(struct platform_device *pdev)
{
	int i;

	ion_device_destroy(hisi_ion_device);
	for (i = 0; i < HISI_ION_HEAP_NUM; i++) {
		if (!hisi_ion_heap[i])
            continue;
		ion_heap_destroy(hisi_ion_heap[i]);
        hisi_ion_heap[i] = NULL;
	}

	return 0;
}

static int hisi_ion_iommu_remove(struct platform_device *pdev)
{
    int ret = hisi_ion_remove(pdev);

    return ret;
}

static struct of_device_id hisi_ion_iommu_match_table[] = {
	{.compatible = "hisi,hi6xxx-ion-iommu"},
	{},
};

static struct platform_driver hisi_ion_iommu_driver = {
	.probe = hisi_ion_iommu_probe,
	.remove = hisi_ion_iommu_remove,
	.driver = {
		.name = "hi6xxx-ion-iommu",
		.of_match_table = hisi_ion_iommu_match_table,
	},
};

static int __init hisi_ion_init(void)
{
	int ret;
	ret = platform_driver_register(&hisi_ion_iommu_driver);
	return ret;
}

static void __exit hisi_ion_exit(void)
{
	platform_driver_unregister(&hisi_ion_iommu_driver);
	return;
}

subsys_initcall(hisi_ion_init);
module_exit(hisi_ion_exit);

static int hisi_ion_proc_show(struct seq_file *m, void *v)
{
    int i;

    seq_printf(m, "name\t\t\t id\t type\t\t\t base\t\t size\r\n");
    for(i = 0; i < hisi_ion_platform_data.nr; i++){
        seq_printf(m, "%-16s\t %u\t %-18s\t 0x%08lx\t 0x%08x\r\n", hisi_ion_platform_heap[i].name, \
            hisi_ion_platform_heap[i].id, get_type_by_id(hisi_ion_platform_heap[i].type), \
            hisi_ion_platform_heap[i].base, hisi_ion_platform_heap[i].size);
    }

	return 0;
}

static int hisi_ion_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, hisi_ion_proc_show, NULL);
}

static const struct file_operations hisi_ion_proc_fops = {
	.open = hisi_ion_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init hisi_ion_proc_init(void)
{
	balong_create_debug_proc_entry("hisi_ion", S_IRUGO, &hisi_ion_proc_fops, NULL);

	return 0;
}
module_init(hisi_ion_proc_init);

