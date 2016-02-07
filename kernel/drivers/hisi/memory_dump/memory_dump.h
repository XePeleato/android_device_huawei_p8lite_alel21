#ifndef __MEMORY_DUMP_H
#define __MEMORY_DUMP_H
#include <linux/mm_types.h>
struct mem_dump_cb
{
	/*the virtual base address of the struct page objects. all page objects occupy a contiguous memory region.*/
	struct page *page;
	u32 page_shift; //the value is log 2 (the size of a page which is the basic MMU unit).
	u32 struct_page_size; //the size of struct page.
	u64 phys_offset; //the start physical address managed by linux
	u64 page_offset; //the base logical address
	/*used to find the pa of a page object through translation table walk. For ARM32 it is 0, 
	for ARM64 it is the base va TTBR1 maps*/
	ulong kern_map_offset;
	/*the definition is different from linux. it is used to tell the difference 
	    between CONFIG_FLATMEM and CONFIG_SPARSEMEM_VMEMMAP*/
	u64 pfn_offset; 
	u64 ttbr; //the base address of the pgd
	struct memblock_type *mb_cb; //the physical address of memory memoryblock_type object.
	/*the size of the memory section. if CONFIG_FLATMEM is enabled it is 0, 
	    if CONFIG_SPARSEMEM_VMEMMAP is enabled it is the size of a memory section*/ 
	u64 section_size;
	u64 pmd_size; /*used to calculate the page objects for 4KB pages, becacuse they are mapped via the block type pmd entres.*/
       u64 img_base; /*physcal address of the linux kernel image */
       u32 img_size; /*the size of the linux kernel image */      
	u32 mbr_size; /*the size of struct memblock_region, the size of which is depended on kernel configuration. */
};
#endif
