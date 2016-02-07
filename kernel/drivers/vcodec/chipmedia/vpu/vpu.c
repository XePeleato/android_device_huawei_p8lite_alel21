/*
 * vpu.c
 *
 * linux device driver for VPU.
 *
 * Copyright (C) 2006 - 2013  CHIPS&MEDIA INC.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 *
 */

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/wakelock.h>

#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/hisi/hisi_ion.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/ion.h>
#include <linux/mman.h>
#include <asm/cacheflush.h>
#ifndef CONFIG_ARM64
#include <asm/outercache.h>
#else
#include <linux/compat.h>
#endif
#include "soc_irqs.h"
#include "soc_ao_sctrl_interface.h"
#include "soc_media_sctrl_interface.h"
#include "soc_pmctrl_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "soc_baseaddr_interface.h"
#include "video_reg_ops.h"
#include "video_harden.h"
#include "video_config.h"

#include <linux/fb.h>
#include <asm/fb.h>

#define LOG_TAG "CNM_VPU_DRV"
#include "vpu.h"


/* note: this is from vpuconfig.h*/
#define MAX_NUM_INSTANCE        4
#define MAX_NUM_VPU_CORE        1
#define MAX_INST_HANDLE_SIZE    (32*1024)

#define VPU_AUTO_CCLK               1

#define VPU_DEV_NAME "cnm_vpu"
#define VPU_IRQ_NAME "VPU_CODEC_IRQ"


#ifndef VM_RESERVED	/*for kernel up to 3.7.0 version*/
# define  VM_RESERVED   (VM_DONTEXPAND | VM_DONTDUMP)
#endif

//#define DUMP_VPU_POWER_REG

static struct ion_device *s_ion_dev;
static struct ion_client *s_ion_instance_pool_client;
static struct ion_handle *s_ion_instance_pool_handle;
static struct ion_client *s_ion_reserved_mem_client;
static struct ion_handle *s_ion_reserved_mem_handle;
static int s_vpu_systemMMU_support = 0;

typedef struct
{
    struct clk *clk_gpupll_src;
    struct clk *clk_medpll_src;
	struct clk *clk_medpll_src_gated;
	struct clk *pclk_codec_vpu;
	struct clk *aclk_codec_vpu;
	struct clk *clk_codec_vpu;

	unsigned int clk_gpupll_src_bit         : 1;
	unsigned int clk_medpll_src_bit         : 1;
	unsigned int clk_medpll_src_gated_bit   : 1;
	unsigned int pclk_codec_vpu_bit         : 1;
	unsigned int aclk_codec_vpu_bit         : 1;
	unsigned int clk_codec_vpu_bit          : 1;

	unsigned int reserve                    : 26;

    /*for common image, z62576, 20140429, begin*/
    u32 clk_freq;
    u32 aclk_freq;

    u32 wait_timeout_coeff;
    u32 sram_alloc_timeout;
    /*for common image, z62576, 20140429, end*/
} vpu_clk;



typedef struct vpu_drv_context_t {
	struct fasync_struct *async_queue;
    u32 open_count;                     /*!<< device reference count. Not instance count */
} vpu_drv_context_t;

/* To track the allocated memory buffer */
typedef struct vpudrv_buffer_pool_t {
	struct list_head list;
	struct vpudrv_buffer_t vb;
	struct file *filp;
} vpudrv_buffer_pool_t;

/* To track the instance index and buffer in instance pool */
typedef struct vpudrv_instanace_list_t {
	struct list_head list;
	unsigned long inst_idx;
	unsigned long core_idx;
	struct file *filp;
} vpudrv_instanace_list_t;

typedef struct vpu_buffer_t {
	unsigned int size;
	unsigned int  phys_addr;
	unsigned long long base;
	unsigned long virt_addr;
} vpu_buffer_t;

#include "vmm.h"
static video_mm_t s_vmem;
static vpudrv_buffer_t s_video_memory = {0};


typedef struct vpudrv_instance_pool_t {
	unsigned char codecInstPool[MAX_NUM_INSTANCE][MAX_INST_HANDLE_SIZE];
	int vpu_instance_num;
} vpudrv_instance_pool_t;

static int vpu_hw_reset(void);

static void vpu_varible_init(void);
static int vpu_regulator_enable(void);
static void vpu_regulator_disable(void);
static int vpu_clk_get(void);
static void vpu_clk_put(void);
static int vpu_clk_enable(void);
static void vpu_clk_disable(void);
static int vpu_pll_clk_enable(void);
static void vpu_pll_clk_disable(void);

extern struct ion_device * get_ion_device(void);

static SramDrvInfo s_vpu_sramDrvInfo;
static bool s_vpu_sramTimeout               = false;
static struct semaphore s_vpu_semaphore     = __SEMAPHORE_INITIALIZER(s_vpu_semaphore, 0);

int vpu_sram_callback(unsigned int memSize, unsigned int memAddress);
int vpu_allocate_sram_memory(unsigned int memSize, unsigned int alloc_sram_timeout);

//state: 0, suspend; 1, resume.
extern void vpu_pm_notify_ade(int state);


typedef int (*pADEMemCB)(unsigned int, unsigned int);
extern int DRV_ADEMemAlloc(unsigned int memSize, pADEMemCB AllocNotifyCB);
extern void DRV_ADEMemFree(void);

extern void vpu_on_notice_ade(void);
extern void vpu_off_notice_ade(void);




/* end customer definition */
static vpudrv_buffer_t s_instance_pool = {0};
static vpudrv_buffer_t s_common_memory = {0};
static vpu_drv_context_t s_vpu_drv_context;
static int s_vpu_major;
static struct cdev s_vpu_cdev;

static vpu_clk s_vpu_clk;

static int s_vpu_irq = IRQ_VDEC;
static u32 s_vpu_reg_phy_addr = SOC_VPU_S_BASE_ADDR;

static void __iomem *s_vpu_reg_virt_addr;
static void __iomem *s_vpu_qos_virt_addr;

static int s_interrupt_flag;
static wait_queue_head_t s_interrupt_wait_q;

static DEFINE_SEMAPHORE(s_vpu_sem);
static struct list_head s_vbp_head = LIST_HEAD_INIT(s_vbp_head);
static struct list_head s_inst_list_head = LIST_HEAD_INIT(s_inst_list_head);

static struct class *cnm_vpu_class = NULL;
static struct device *cnm_vpu_dev = NULL;

static vpu_bit_firmware_info_t s_bit_firmware_info[MAX_NUM_VPU_CORE];
static u32 s_vpu_reg_store[MAX_NUM_VPU_CORE][64];
static u32 s_run_index;
static u32 s_run_codstd;

static u32 s_vpu_frame_clock_state  = 0;
static u32 s_vpu_suspend_state      = 0;
static struct wake_lock s_vpu_wakelock;

#define VPU_SLEEP               10
#define VPU_WAKE                11


#define BIT_BASE                    0x0000
#define BIT_CODE_RUN                (BIT_BASE + 0x000)
#define BIT_CODE_DOWN               (BIT_BASE + 0x004)
#define BIT_INT_CLEAR               (BIT_BASE + 0x00C)
#define BIT_INT_ENABLE              (BIT_BASE + 0x170)
#define BIT_INT_STS                 (BIT_BASE + 0x010)
#define BIT_CODE_RESET				(BIT_BASE + 0x014)
#define BIT_CUR_PC                  (BIT_BASE + 0x018)
#define BIT_BUSY_FLAG               (BIT_BASE + 0x160)
#define BIT_RUN_COMMAND             (BIT_BASE + 0x164)
#define BIT_RUN_INDEX               (BIT_BASE + 0x168)
#define BIT_RUN_COD_STD             (BIT_BASE + 0x16C)



#define	ReadVpuRegister(addr) *(volatile unsigned int *)(s_vpu_reg_virt_addr + s_bit_firmware_info[core].reg_base_offset + addr)
/*lint -e773*/
#define	WriteVpuRegister(addr, val)	*(volatile unsigned int *)(s_vpu_reg_virt_addr + s_bit_firmware_info[core].reg_base_offset + addr) = (unsigned int)val
/*lint +e773*/

static int vpu_alloc_dma_buffer(vpudrv_buffer_t *vb)
{
	if (!vb)
		return -1;

	vb->phys_addr = (unsigned long)vmem_alloc(&s_vmem, vb->size, 0);
	if (vb->phys_addr  == (unsigned int)-1) {
		//vpu_loge("Physical memory allocation error size=%d\n", vb->size);
		return -1;
	}
    vb->base = 0;

	//vb->base = (unsigned long)(s_video_memory.base + (vb->phys_addr - s_video_memory.phys_addr));

   //vpu_logd("[s_vmem] vb->size=%u, vb->phys_addr=0x%x, vb->base=0x%lx, num_pages=%d, alloc_page_count=%d, free_page_count=%d\n",
             //vb->size, vb->phys_addr, vb->base, s_vmem.num_pages, s_vmem.alloc_page_count, s_vmem.free_page_count);

	return 0;
}

static void vpu_free_dma_buffer(vpudrv_buffer_t *vb)
{
	if (!vb)
		return;

	if (vb->size)
		vmem_free(&s_vmem, vb->phys_addr, 0);

    //vpu_logd("[s_vmem] vb->phys_addr=0x%x, num_pages=%d, alloc_page_count=%d, free_page_count=%d\n",
             //vb->phys_addr, s_vmem.num_pages, s_vmem.alloc_page_count, s_vmem.free_page_count);
}

static int vpu_free_instances(struct file *filp)
{
	vpudrv_instanace_list_t *vil, *n;
	vpudrv_instance_pool_t *vip;
	void *vip_base;
	int instance_pool_size_per_core;

	//vpu_logd("vpu_free_instances inter. sizeof(vpudrv_instance_pool_t)=%d \n", sizeof(vpudrv_instance_pool_t));

	instance_pool_size_per_core = (s_instance_pool.size/MAX_NUM_VPU_CORE); /* s_instance_pool.size  assigned to the size of all core once call VDI_IOCTL_GET_INSTANCE_POOL by user. */

	list_for_each_entry_safe(vil, n, &s_inst_list_head, list)
	{
		if (vil->filp == filp) {
			vip_base = (void *)(s_instance_pool.base + (instance_pool_size_per_core*vil->core_idx));
			//vpu_logd("vpu_free_instances detect instance crash instIdx=%d, coreIdx=%d, vip_base=%p, instance_pool_size_per_core=%d\n", (int)vil->inst_idx, (int)vil->core_idx, vip_base, (int)instance_pool_size_per_core);
			vip = (vpudrv_instance_pool_t *)vip_base;
			if (vip) {
				memset(&vip->codecInstPool[vil->inst_idx], 0x00, 4);	/* only first 4 byte is key point to free the corresponding instance. */
				vip->vpu_instance_num--;
			}
			list_del(&vil->list);
			kfree(vil);
		}
	}

	return 1;
}

static int vpu_free_buffers(struct file *filp)
{
	vpudrv_buffer_pool_t *pool, *n;
	vpudrv_buffer_t vb;

	//vpu_logd("vpu_free_buffers\n");

	list_for_each_entry_safe(pool, n, &s_vbp_head, list)
	{
		if (pool->filp == filp) {
			vb = pool->vb;
			if (vb.size) {
				vpu_free_dma_buffer(&vb);
				list_del(&pool->list);
				kfree(pool);
			}
		}

	}

	return 0;
}

static irqreturn_t vpu_irq_handler(int irq, void *dev_id)
{
    if (s_vpu_drv_context.open_count == 0)
    {
        return IRQ_HANDLED;
    }

	vpu_drv_context_t *dev = (vpu_drv_context_t *)dev_id;

	/* this can be removed. it also work in VPU_WaitInterrupt of API function */
	int core;

	//DPRINTK("[VPUDRV] vpu_irq_handler\n");

	for (core = 0; core < MAX_NUM_VPU_CORE; core++)	{
		if (s_bit_firmware_info[core].size == 0) /*it means that we didn't get an information the current core from API layer. No core activated.*/
			continue;

		if (ReadVpuRegister(BIT_INT_STS))
		{
            //vpu_logd("BIT_INT_STS. int reason=0x%x", ReadVpuRegister(BIT_INT_REASON));
			WriteVpuRegister(BIT_INT_CLEAR, 0x1);
		}
	}

	if (dev->async_queue)
		kill_fasync(&dev->async_queue, SIGIO, POLL_IN);	/* notify the interrupt to user space */

	s_interrupt_flag = 1;

	wake_up_interruptible(&s_interrupt_wait_q);
	return IRQ_HANDLED;
}

static int vpu_open(struct inode *inode, struct file *filp)
{
	int ret = 0;

    //vpu_logi("enter. s_vpu_open_ref_count=%d \n", s_vpu_drv_context.open_count);

    wake_lock(&s_vpu_wakelock);

    ret = down_interruptible(&s_vpu_sem);
	if (0 != ret)
	{
		//vpu_loge("vpu_open down_interruptible failed\n");
        wake_unlock(&s_vpu_wakelock);
		return -EINTR;
	}

    if (s_vpu_drv_context.open_count > 0)
    {
        //vpu_logi("don't need to open device, the open count is %d\n", s_vpu_drv_context.open_count);
        s_vpu_drv_context.open_count++;
        filp->private_data = (void *)(&s_vpu_drv_context);
        up(&s_vpu_sem);
        wake_unlock(&s_vpu_wakelock);
        return 0;
    }

    ret = vpu_regulator_enable();
    if ( 0 != ret )
    {
        //vpu_loge("vpu regulator enable failed.\n");
        up(&s_vpu_sem);
        wake_unlock(&s_vpu_wakelock);
        return -EIO;
    }

    vpu_clk_disable();

    s_vpu_drv_context.open_count++;

    filp->private_data = (void *)(&s_vpu_drv_context);
    //vpu_logi("VCODEC DEV OPENED! s_vpu_drv_context.open_count=%d\n", s_vpu_drv_context.open_count);
    vpu_on_notice_ade();
    up(&s_vpu_sem);

    wake_unlock(&s_vpu_wakelock);

	return 0;
}

/*static int vpu_ioctl(struct inode *inode, struct file *filp, u_int cmd, u_long arg) // for kernel 2.6.9 of C&M*/
static long vpu_ioctl(struct file *filp, u_int cmd, u_long arg)
{
	int ret = 0;
	unsigned long ret_value = 0;

	if (_IOC_TYPE(cmd) != VDI_IOCTL_MAGIC)
		return -ENOTTY;
	switch (cmd) {
	case VDI_IOCTL_ALLOCATE_PHYSICAL_MEMORY:
		{
			vpudrv_buffer_pool_t *vbp;

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

			vbp = kzalloc(sizeof(*vbp), GFP_KERNEL);
			if (!vbp) {
				up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -ENOMEM;
			}
			ret_value = copy_from_user(&(vbp->vb), (vpudrv_buffer_t *)arg, sizeof(vpudrv_buffer_t));
			if (ret_value) {
				kfree(vbp);
				up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -EFAULT;
			}

			ret = vpu_alloc_dma_buffer(&(vbp->vb));
			if (ret == -1) {
				kfree(vbp);
				up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -ENOMEM;
			}
            ret_value = copy_to_user((void __user *)arg, &(vbp->vb), sizeof(vpudrv_buffer_t));
			if (ret_value) {
				kfree(vbp);
				up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -EFAULT;
			}

			vbp->filp = filp;
			list_add(&vbp->list, &s_vbp_head);

			up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
		}
		break;
	case VDI_IOCTL_FREE_PHYSICALMEMORY:
		{
			vpudrv_buffer_pool_t *vbp, *n;
			vpudrv_buffer_t vb;

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

			ret = copy_from_user(&vb, (vpudrv_buffer_t *)arg, sizeof(vpudrv_buffer_t));
			if (ret) {
				up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -EACCES;
			}

			if (vb.size)
				vpu_free_dma_buffer(&vb);

			list_for_each_entry_safe(vbp, n, &s_vbp_head, list)
			{
				if (vbp->vb.phys_addr == vb.phys_addr) {
					list_del(&vbp->list);
					kfree(vbp);
					break;
				}
			}

			up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
		}
		break;
	case VDI_IOCTL_GET_RESERVED_VIDEO_MEMORY_INFO:
		{
		    unsigned int drv_reserved_size = 0;

            wake_lock(&s_vpu_wakelock);

		    ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

            drv_reserved_size = s_vmem.free_page_count * VMEM_PAGE_SIZE;

            ret_value = copy_to_user((void __user *) arg, &drv_reserved_size, sizeof(unsigned int));
            if (ret_value != 0)
            {
                //vpu_loge("VDI_IOCTL_GET_RESERVED_VIDEO_MEMORY_INFO copy_to_user fail\n");
                ret = -EFAULT;
            }

            up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
		}
		break;

	case VDI_IOCTL_WAIT_INTERRUPT:
		{
            /* for common image, z62576, 20140429, begin */
            //u32 timeout = (u32) arg * s_vpu_clk.wait_timeout_coeff;
            u32 timeout = 1000;
            /* for common image, z62576, 20140429, end */

            wake_lock(&s_vpu_wakelock);

			if (!wait_event_interruptible_timeout(s_interrupt_wait_q, s_interrupt_flag != 0, msecs_to_jiffies(timeout))) {
				//vpu_loge("###### error wait_event_interruptible_timeout: timeout=%u\n", timeout);

                wake_unlock(&s_vpu_wakelock);
				return -ETIME;
			}

			if (signal_pending(current)) {
				//e("###### error signal_pending failed\n");

                wake_unlock(&s_vpu_wakelock);
				return -ERESTARTSYS;
			}

			s_interrupt_flag = 0;

            wake_unlock(&s_vpu_wakelock);
		}
		break;

	case VDI_IOCTL_SET_CLOCK_GATE:
		{
            u32 clkgate = 0;

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

			if (get_user(clkgate, (u32 __user *) arg))
            {
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -EFAULT;
            }

            if (clkgate)
            {
                vpu_clk_enable();
                s_vpu_frame_clock_state = 1;
            }
			else
            {
                vpu_clk_disable();
                s_vpu_frame_clock_state = 0;
            }

            up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
		}
		break;
	case VDI_IOCTL_GET_INSTANCE_POOL:
		{
            struct iommu_map_format iommu_format;

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

			if (s_instance_pool.base != 0)
            {
                ret_value = copy_to_user((void __user *)arg, &s_instance_pool, sizeof(vpudrv_buffer_t));

				if (ret_value != 0)
					ret = -EFAULT;
			}
            else
			{
    			ret_value = copy_from_user(&s_instance_pool, (vpudrv_buffer_t *)arg, sizeof(vpudrv_buffer_t));
                if (ret_value != 0)
                {
    				ret = -EFAULT;
                }
				else
                {
                    if (s_ion_dev == NULL)
                    {
                        s_ion_dev = get_ion_device();
                        if (NULL == s_ion_dev)
                        {
                            //vpu_loge("fail to get ion device.\n");
                            ret = -EFAULT;
                            goto ERROR_INSTANCE_MEMORY_ALLOC;
                        }
                       //vpu_loge("[DEBUG, not ERROR] s_ion_dev = %p.\n", s_ion_dev);
                    }

                    s_ion_instance_pool_client = ion_client_create(s_ion_dev, "vpu_instance_pool");
                    if (IS_ERR(s_ion_instance_pool_client) || s_ion_instance_pool_client == NULL)
                    {
                        //vpu_loge("fail to create s_ion_instance_pool_client.\n");
                        ret = -EFAULT;
                        goto ERROR_INSTANCE_MEMORY_ALLOC;
                    }
                    //vpu_loge("[DEBUG, not ERROR] s_ion_instance_pool_client = %p.\n", s_ion_instance_pool_client);

                    s_ion_instance_pool_handle = ion_alloc(s_ion_instance_pool_client, ION_8K_ALIGN(s_instance_pool.size), 0, ION_HEAP(ION_SYSTEM_HEAP_ID), 0);
                    if (IS_ERR(s_ion_instance_pool_handle) || s_ion_instance_pool_handle == NULL)
                    {
                        //vpu_loge("fail to ion alloc instance pool memory.\n");
                        ret = -EFAULT;
                        goto ERROR_INSTANCE_MEMORY_ALLOC;
                    }
                    //vpu_loge("[DEBUG, not ERROR] s_ion_instance_pool_handle = %p.\n", s_ion_instance_pool_handle);

                    iommu_format.is_tile = 0;
                    if (0 != ion_map_iommu(s_ion_instance_pool_client,s_ion_instance_pool_handle,&iommu_format))
                    {
                        //vpu_loge("fail to  ion_phys().\n");
                        ret = -EFAULT;
                        goto ERROR_INSTANCE_MEMORY_ALLOC;
                    }
        			
                    s_instance_pool.phys_addr = iommu_format.iova_start;
                    s_instance_pool.size = (unsigned int)iommu_format.iova_size;
                    s_instance_pool.base = (unsigned long)ion_map_kernel(s_ion_instance_pool_client, s_ion_instance_pool_handle);
                    
                    //vpu_loge("[DEBUG, not ERROR] s_instance_pool.phys_addr = 0x%x.\n", s_instance_pool.phys_addr);
                    if (IS_ERR((void *)s_instance_pool.base) || s_instance_pool.base == 0)
                    {
                        //vpu_loge("fail to ion_map_kernel() for instance pool.\n");
                        ret = -EFAULT;
                        goto ERROR_INSTANCE_MEMORY_ALLOC;
                    }
                    //vpu_loge("[DEBUG, not ERROR] s_instance_pool.base = 0x%lx.\n", s_instance_pool.base);

					memset((void *)s_instance_pool.base, 0x0, s_instance_pool.size); /*clearing memory*/

                    ret_value = copy_to_user((void __user *)arg, &s_instance_pool, sizeof(vpudrv_buffer_t));
                    if (ret_value != 0)
                    {
                       //vpu_loge("fail to copy_to_user\n");
                        ret = -EFAULT;
                    }
            
ERROR_INSTANCE_MEMORY_ALLOC:
                    if (0 != ret)
                    {
                        if ((!IS_ERR((void *)s_instance_pool.base)) && (s_instance_pool.base != 0))
                        {
                            //vpu_loge("s_instance_pool.base = 0x%lx s_ion_instance_pool_client = %p s_ion_instance_pool_handle = %p",
                                    //s_instance_pool.base, s_ion_instance_pool_client, s_ion_instance_pool_handle);

                            ion_unmap_kernel(s_ion_instance_pool_client, s_ion_instance_pool_handle);
                        }

                        memset(&s_instance_pool, 0x00, sizeof(vpudrv_buffer_t));

                        if ((!IS_ERR(s_ion_instance_pool_handle)) && (s_ion_instance_pool_handle != NULL))
                        {
                            //vpu_loge("s_ion_instance_pool_client = %p s_ion_instance_pool_handle = %p", 
                                    //s_ion_instance_pool_client, s_ion_instance_pool_handle);

                            ion_free(s_ion_instance_pool_client, s_ion_instance_pool_handle);
                            s_ion_instance_pool_handle = NULL;
                        }

                        if ((!IS_ERR(s_ion_instance_pool_client)) && (s_ion_instance_pool_client != NULL))
                        {
                            //vpu_loge("s_ion_instance_pool_client = %p", s_ion_instance_pool_client);

                            ion_client_destroy(s_ion_instance_pool_client);
                            s_ion_instance_pool_client = NULL;
                        }
                    }
				}                      
    		}
			up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
		}
		break;
	case VDI_IOCTL_GET_COMMON_MEMORY:
		{
            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

			if (s_common_memory.size != 0)
            {
                ret_value = copy_to_user((void __user *)arg, &s_common_memory, sizeof(vpudrv_buffer_t));

				if (ret_value != 0)
					ret = -EFAULT;
			} 
            else
            {
     			ret_value = copy_from_user(&s_common_memory, (vpudrv_buffer_t *)arg, sizeof(vpudrv_buffer_t));
				if (ret_value == 0)
                {
					if (vpu_alloc_dma_buffer(&s_common_memory) != -1)
                    {
                        ret_value = copy_to_user((void __user *)arg, &s_common_memory, sizeof(vpudrv_buffer_t));
					    if (ret_value == 0)
                        {
							/* success to get memory for common memory */
							up(&s_vpu_sem);
                            wake_unlock(&s_vpu_wakelock);
							break;
						}
					}
				}

				ret = -EFAULT;
			}

			up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
		}
		break;
	case VDI_IOCTL_OPEN_INSTANCE:
		{
			vpudrv_inst_info_t inst_info;
			vpudrv_instanace_list_t *vil, *n;

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

			vil = kzalloc(sizeof(*vil), GFP_KERNEL);
			if (!vil)
			{
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -ENOMEM;
			}

			if (copy_from_user(&inst_info, (vpudrv_inst_info_t *)arg, sizeof(vpudrv_inst_info_t)))
			{
				kfree(vil);
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -EFAULT;
			}

			vil->inst_idx = inst_info.inst_idx;
			vil->core_idx = inst_info.core_idx;
			vil->filp = filp;

			list_add(&vil->list, &s_inst_list_head);

			inst_info.inst_open_count = 0; /* counting the current open instance number */

			list_for_each_entry_safe(vil, n, &s_inst_list_head, list)
			{
				if (vil->core_idx == inst_info.core_idx)
					inst_info.inst_open_count++;
			}

			if (copy_to_user((void __user *)arg, &inst_info, sizeof(vpudrv_inst_info_t)))
			{
				kfree(vil);
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -EFAULT;
			}

            up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
		}
		break;
	case VDI_IOCTL_CLOSE_INSTANCE:
		{
			vpudrv_inst_info_t inst_info;
			vpudrv_instanace_list_t *vil, *n;

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

			if (copy_from_user(&inst_info, (vpudrv_inst_info_t *)arg, sizeof(vpudrv_inst_info_t)))
			{
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -EFAULT;
			}

			list_for_each_entry_safe(vil, n, &s_inst_list_head, list)
			{
				if (vil->inst_idx == inst_info.inst_idx && vil->core_idx == inst_info.core_idx) {
					list_del(&vil->list);
					kfree(vil);
					break;
				}
			}

			inst_info.inst_open_count = 0; /* counting the current open instance number */
			list_for_each_entry_safe(vil, n, &s_inst_list_head, list)
			{
				if (vil->core_idx == inst_info.core_idx)
					inst_info.inst_open_count++;
			}

			if (copy_to_user((void __user *)arg, &inst_info, sizeof(vpudrv_inst_info_t)))
			{
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -EFAULT;
			}

            up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
		}
		break;
	case VDI_IOCTL_GET_INSTANCE_NUM:
		{
			vpudrv_inst_info_t inst_info;
			vpudrv_instanace_list_t *vil, *n;

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

			ret = copy_from_user(&inst_info, (vpudrv_inst_info_t *)arg, sizeof(vpudrv_inst_info_t));
			if (ret != 0)
			{
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -EFAULT;
			}

			inst_info.inst_open_count = 0;

			list_for_each_entry_safe(vil, n, &s_inst_list_head, list)
			{
				if (vil->core_idx == inst_info.core_idx)
					inst_info.inst_open_count++;
			}

			ret_value = copy_to_user((void __user *)arg, &inst_info, sizeof(vpudrv_inst_info_t));

			//vpu_logd("VDI_IOCTL_GET_INSTANCE_NUM core_idx=%d, inst_idx=%d, open_count=%d\n",
			         //(int)inst_info.core_idx, (int)inst_info.inst_idx, inst_info.inst_open_count);

		    if (ret_value != 0)
            {
                ret = -EFAULT;
            }

            up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
		}
		break;
	case VDI_IOCTL_RESET:
		{
			vpu_hw_reset();
		}
		break;
    case VDI_IOCTL_GET_SYSTEMMMU_SURPPORT:
        {
            //vpu_logd("VDI_IOCTL_GET_SYSTEMMMU_SURPPORT \n");

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

            ret_value = copy_to_user((void __user *)arg, &s_vpu_systemMMU_support, sizeof(int));
            if (ret_value != 0)
            {
                ret = -EFAULT;
            }

            up(&s_vpu_sem);
            //vpu_logd("s_vpu_systemMMU_support =%d\n", s_vpu_systemMMU_support);

            wake_unlock(&s_vpu_wakelock);
        }
        break;
    case VDI_IOCTL_FLUSH_ION_CACHE:
#if 0
        {
            //vpu_logd("VDI_IOCTL_FLUSH_ION_CACHE.\n");
            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                return -EINTR;
            }

            if (NULL != s_ion_client && NULL != s_ion_handle)
            {
                //ion_flush_all_cache();
                flush_cache_all();
#ifndef CONFIG_ARM64
                outer_flush_all();
#endif
            }
            else
            {
                //vpu_loge("s_ion_client or s_ion_handle is NULL.\n");
                ret = -EFAULT;
            }
            up(&s_vpu_sem);
        }
#endif
        break;

    case VDI_IOCTL_RESERVED_MEMORY_ALLOC:
        {
            struct iommu_map_format iommu_format;

            //vpu_loge("[DEBUG, not ERROR]VDI_IOCTL_RESERVED_MEMORY_ALLOC \n");

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

            if (1 != s_vpu_systemMMU_support)
            {
                //vpu_loge("systemMMU is not support.\n");
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
                return -EFAULT;
            }

            if (s_video_memory.size != 0)
            {
                ret_value = copy_to_user((void __user *)arg, &s_video_memory, sizeof(vpudrv_buffer_t));
                if (ret_value != 0)
                {
                    //vpu_loge("fail to copy_to_user\n");
                    ret = -EFAULT;
                }

                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
                return ret;
            }

            /*call ION function to get reserved phyical address and virtual address*/
            ret_value = copy_from_user(&s_video_memory, (vpudrv_buffer_t *)arg, sizeof(vpudrv_buffer_t));
            if (0 != ret_value)
            {
                //vpu_loge("fail to copy_from_user.\n");
                ret = -EFAULT;
                goto ERROR_RESERVED_MEMORY_ALLOC;
            }

            if (s_ion_dev == NULL)
            {
                s_ion_dev = get_ion_device();
                if (NULL == s_ion_dev)
                {
                    //vpu_loge("fail to get ion device.\n");
                    ret = -EFAULT;
                    goto ERROR_RESERVED_MEMORY_ALLOC;
                }
                //vpu_loge("[DEBUG, not ERROR] s_ion_dev = %p.\n", s_ion_dev);
            }
       

            s_ion_reserved_mem_client = ion_client_create(s_ion_dev, "vpu");
            if (IS_ERR(s_ion_reserved_mem_client) || s_ion_reserved_mem_client == NULL)
            {
                //vpu_loge("fail to create s_ion_reserved_mem_client.\n");
                ret = -EFAULT;
                goto ERROR_RESERVED_MEMORY_ALLOC;
            }
            //vpu_loge("[DEBUG, not ERROR] s_ion_reserved_mem_client= %p.\n", s_ion_reserved_mem_client);

            s_ion_reserved_mem_handle = ion_alloc(s_ion_reserved_mem_client, ION_8K_ALIGN(s_video_memory.size), 0, ION_HEAP(ION_SYSTEM_HEAP_ID), 0);
            if (IS_ERR(s_ion_reserved_mem_handle) || s_ion_reserved_mem_handle == NULL)
            {
                //vpu_loge("fail to  ion alloc reserved memory.\n");
                ret = -EFAULT;
                goto ERROR_RESERVED_MEMORY_ALLOC;
            }
            //vpu_loge("[DEBUG, not ERROR] s_ion_reserved_mem_handle = %p.\n", s_ion_reserved_mem_handle);

            iommu_format.is_tile = 0;
            if (0 != ion_map_iommu(s_ion_reserved_mem_client,s_ion_reserved_mem_handle,&iommu_format))
            {
                //vpu_loge("fail to  ion_phys().\n");
                ret = -EFAULT;
                goto ERROR_RESERVED_MEMORY_ALLOC;
            }
			
            s_video_memory.phys_addr = iommu_format.iova_start;
            s_video_memory.size = (unsigned int)iommu_format.iova_size;
            //vpu_loge("[DEBUG, not ERROR] s_video_memory.phys_addr = 0x%x.\n", s_video_memory.phys_addr);

            if (vmem_init(&s_vmem, s_video_memory.phys_addr, s_video_memory.size) < 0)
            {
                //vpu_loge("fail to init vmem system\n");
                ret = -EFAULT;
                goto ERROR_RESERVED_MEMORY_ALLOC;
            }

            ret_value = copy_to_user((void __user *)arg, &s_video_memory, sizeof(vpudrv_buffer_t));
            if (ret_value != 0)
            {
                //vpu_loge("fail to copy_to_user\n");
                ret = -EFAULT;
            }

ERROR_RESERVED_MEMORY_ALLOC:

            if (0 != ret)
            {
                vmem_exit(&s_vmem);

                memset(&s_video_memory, 0x00, sizeof(vpudrv_buffer_t));

                if ((!IS_ERR(s_ion_reserved_mem_handle)) && (s_ion_reserved_mem_handle != NULL))
                {
                    //vpu_loge("s_ion_reserved_mem_client = %p s_ion_reserved_mem_handle = %p\n", s_ion_reserved_mem_client, s_ion_reserved_mem_handle);

                    ion_free(s_ion_reserved_mem_client, s_ion_reserved_mem_handle);
                    s_ion_reserved_mem_handle = NULL;
                }

                if ((!IS_ERR(s_ion_reserved_mem_client)) && (s_ion_reserved_mem_client != NULL))
                {
                    //vpu_loge("s_ion_reserved_mem_client = %p\n", s_ion_reserved_mem_client);

                    ion_client_destroy(s_ion_reserved_mem_client);
                    s_ion_reserved_mem_client = NULL;
                }
            }
            up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
        }
        break;

    case VDI_IOCTL_ALLOCATE_SRAM_MEMORY:
        {
            SramDrvInfo vdiSramInfo     = {0};

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

            //vpu_logd("VDI_IOCTL_ALLOCATE_SRAM_MEMORY enter. SRAM_ALLOCATE_TIME_OUT(350)\n");

            /* todo enter lock */
            memset(&vdiSramInfo, 0x00, sizeof(SramDrvInfo));

            ret = copy_from_user(&vdiSramInfo, (void __user *)arg, sizeof(SramDrvInfo));
            if (0 != ret)
            {
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
                return -EFAULT;
            }

            if (0 == s_vpu_sramDrvInfo.sram_size)
            {
                s_vpu_sramTimeout = false;
                ret = vpu_allocate_sram_memory(vdiSramInfo.sram_size, 350);//s_vpu_clk.sram_alloc_timeout

                if (0 != ret)
                {
                    //vpu_logw("VDI_IOCTL_ALLOCATE_SRAM_MEMORY leave: alloc sram memory fail\n");

                    /* leave lock */
					up(&s_vpu_sem);
                    wake_unlock(&s_vpu_wakelock);
                    return -EPERM;
                }

                if ((s_vpu_sramDrvInfo.sram_size > 0) && (vdiSramInfo.sram_size > s_vpu_sramDrvInfo.sram_size))
                {
                    /* need sram size > ade alloc size */
                    //vpu_loge("VDI_IOCTL_ALLOCATE_SRAM_MEMORY leave: alloc sram size(%d)  < VDI need sram size(%d) -->DRV_ADEMemFree \n",
                             //s_vpu_sramDrvInfo.sram_size ,vdiSramInfo.sram_size);

                    s_vpu_sramDrvInfo.sram_size = 0;
                    s_vpu_sramDrvInfo.phys_addr = 0;

                    DRV_ADEMemFree();
                    /* todo leave lock */
					up(&s_vpu_sem);
                    wake_unlock(&s_vpu_wakelock);
                    return -EPERM;
                }
            }

            ret = copy_to_user((void __user *)arg, &s_vpu_sramDrvInfo, sizeof(SramDrvInfo));
            if (0 != ret)
            {
                //vpu_loge("VDI_IOCTL_ALLOCATE_SRAM_MEMORY leave: fail to copy Drv sramInfo to VDI sramInfo \n");
                if (0 < s_vpu_sramDrvInfo.sram_size)
                {
                    s_vpu_sramDrvInfo.phys_addr = 0;
                    s_vpu_sramDrvInfo.sram_size = 0;

                    DRV_ADEMemFree();
                }
                ret =  -EFAULT;
            }
            //vpu_logd("VDI_IOCTL_ALLOCATE_SRAM_MEMORY leave: alloc ade sram memSize=%d, ret=%d\n", s_vpu_sramDrvInfo.sram_size, ret);
			up(&s_vpu_sem);
            /* todo leave lock */

            wake_unlock(&s_vpu_wakelock);
        }
        break;

    case VDI_IOCTL_FREE_SRAM_MEMORY:
        {
            wake_lock(&s_vpu_wakelock);
            //vpu_logd("VDI_IOCTL_FREE_SRAM_MEMORY enter. sramSize=%d\n", s_vpu_sramDrvInfo.sram_size);
            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

            /* enter lock */
            if ((1 == s_vpu_drv_context.open_count ) && (0 < s_vpu_sramDrvInfo.sram_size))
            {
                s_vpu_sramDrvInfo.phys_addr = 0;
                s_vpu_sramDrvInfo.sram_size = 0;

                DRV_ADEMemFree();
            }

			up(&s_vpu_sem);
            /* leave lock */

            wake_unlock(&s_vpu_wakelock);
        }
        break;

    case  VDI_IOCTL_GET_VSCREENINFO:
        {
            extern struct fb_info *get_fb_info(unsigned int idx);

            struct fb_var_screeninfo var;
            struct fb_info  *info = NULL;

            wake_lock(&s_vpu_wakelock);

            ret = down_interruptible(&s_vpu_sem);
            if (0 != ret)
            {
                //vpu_loge("cmd=%d down_interruptible failed\n", cmd);
                wake_unlock(&s_vpu_wakelock);
                return -EINTR;
            }

            info = get_fb_info(0);

            if (info)
            {
                var = info->var;
                ret = copy_to_user((void __user *)arg, &var, sizeof(struct fb_var_screeninfo));
                if (ret)
                {
                    //vpu_loge("fail to copy_to_user\n");
                    ret = -EFAULT;
                }
            }
            else
            {
                ret = -EFAULT;
                //vpu_loge("get fb info err.\n");
            }

            up(&s_vpu_sem);

            wake_unlock(&s_vpu_wakelock);
       }
       break;

	default:
		{
			//vpu_loge("No such IOCTL, cmd is %d\n", cmd);
		}
		break;
	}

	return ret;
}

#ifdef CONFIG_COMPAT
long compat_get_vpudrv_data(u_int cmd, void __user *data, void __user *data32)
{

    long ret = 0;
    unsigned int ui = 0;
    unsigned long long ull =0;
    compat_ulong_t ul = 0;

    ret  = get_user(ui, &(((compat_vpudrv_buffer_t *)data32)->size));
    ret |= put_user(ui, &(((vpudrv_buffer_t *)data)->size));
    ret |= get_user(ui, &(((compat_vpudrv_buffer_t *)data32)->phys_addr));
    ret |= put_user(ui, &(((vpudrv_buffer_t *)data)->phys_addr));
    ret |= get_user(ull, &(((compat_vpudrv_buffer_t *)data32)->base));
    ret |= put_user(ull, &(((vpudrv_buffer_t *)data)->base));
    ret |= get_user(ul, &(((compat_vpudrv_buffer_t *)data32)->virt_addr));
    ret |= put_user(ul, &(((vpudrv_buffer_t *)data)->virt_addr));

    return ret;
}

long compat_put_vpudrv_data(u_int cmd, void __user *data32, void __user *data)
{
    long ret = 0;
    unsigned int ui = 0;
    unsigned long long ull =0;
    compat_ulong_t ul = 0;

    ret  = get_user(ui, &(((vpudrv_buffer_t *)data)->size));
    ret |= put_user(ui, &(((compat_vpudrv_buffer_t *)data32)->size));
    ret |= get_user(ui, &(((vpudrv_buffer_t *)data)->phys_addr));
    ret |= put_user(ui, &(((compat_vpudrv_buffer_t *)data32)->phys_addr));
    ret |= get_user(ull, &(((vpudrv_buffer_t *)data)->base));
    ret |= put_user(ull, &(((compat_vpudrv_buffer_t *)data32)->base));
    ret |= get_user(ul, &(((vpudrv_buffer_t *)data)->virt_addr));
    ret |= put_user(ul, &(((compat_vpudrv_buffer_t *)data32)->virt_addr));

    return ret;
}

static long vpu_compat_ioctl(struct file *filp, u_int cmd, u_long arg)
{
    long ret = 0;
    void __user *data32 = NULL;
    void __user *data = NULL;
    data32 = compat_ptr(arg);

    switch (cmd)
    {
        case VDI_IOCTL_ALLOCATE_PHYSICAL_MEMORY:
        case VDI_IOCTL_FREE_PHYSICALMEMORY:
        case VDI_IOCTL_GET_RESERVED_VIDEO_MEMORY_INFO:
        case VDI_IOCTL_GET_INSTANCE_POOL:
        case VDI_IOCTL_GET_COMMON_MEMORY:
        case VDI_IOCTL_RESERVED_MEMORY_ALLOC:
            {
                data = compat_alloc_user_space(sizeof(vpudrv_buffer_t));
                if (NULL == data)
			return -EFAULT;

                ret = compat_get_vpudrv_data(cmd, data, data32);
                if (0 != ret)
                    return ret;
                ret = vpu_ioctl(filp,  cmd,  (unsigned long)data);
                if (0 != ret)
                    return ret;
                ret = compat_put_vpudrv_data(cmd, data32, data);
                return ret;
            }
            break;

        default:
            {
                ret = vpu_ioctl(filp,  cmd,  (unsigned long)data32);
                return ret;
            }
    }
}
#endif


static ssize_t vpu_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{

	return -1;
}

static ssize_t vpu_write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{
	int ret = 0;

    wake_lock(&s_vpu_wakelock);

	if (!buf) {
		//vpu_loge("vpu_write buf = NULL error \n");
        wake_unlock(&s_vpu_wakelock);
		return -EFAULT;
	}
    ret = down_interruptible(&s_vpu_sem);
	if (0 != ret)
	{
        //vpu_loge("down_interruptible() failed.\n");
        wake_unlock(&s_vpu_wakelock);
		return -EINTR;
	}

	if (len == sizeof(vpu_bit_firmware_info_t))	{
		vpu_bit_firmware_info_t *bit_firmware_info;

		bit_firmware_info = kzalloc(sizeof(vpu_bit_firmware_info_t), GFP_KERNEL);
		if (!bit_firmware_info) {
			//vpu_loge("vpu_write  bit_firmware_info allocation error \n");
			up(&s_vpu_sem);
            wake_unlock(&s_vpu_wakelock);
			return -EFAULT;
		}

		if (copy_from_user(bit_firmware_info, buf, len)) {
			//vpu_loge("vpu_write copy_from_user error for bit_firmware_info\n");
			kfree(bit_firmware_info);
			up(&s_vpu_sem);
            wake_unlock(&s_vpu_wakelock);
            return -EFAULT;
		}

		if (bit_firmware_info->size == sizeof(vpu_bit_firmware_info_t)) {
			//vpu_logd("vpu_write set bit_firmware_info coreIdx=0x%x, reg_base_offset=0x%x size=0x%x, bit_code[0]=0x%x\n",
                    //bit_firmware_info->core_idx, (int)bit_firmware_info->reg_base_offset, bit_firmware_info->size, bit_firmware_info->bit_code[0]);

			if (bit_firmware_info->core_idx >= MAX_NUM_VPU_CORE) {
				//vpu_loge("vpu_write coreIdx[%d] is exceeded than MAX_NUM_VPU_CORE[%d]\n", bit_firmware_info->core_idx, MAX_NUM_VPU_CORE);
				kfree(bit_firmware_info);
                up(&s_vpu_sem);
                wake_unlock(&s_vpu_wakelock);
				return -ENODEV;
			}

			memcpy((void *)&s_bit_firmware_info[bit_firmware_info->core_idx], bit_firmware_info, sizeof(vpu_bit_firmware_info_t));
            kfree(bit_firmware_info);
            up(&s_vpu_sem);
            wake_unlock(&s_vpu_wakelock);
			return len;
		}

		kfree(bit_firmware_info);
	}

	up(&s_vpu_sem);

    wake_unlock(&s_vpu_wakelock);

	return -1;
}

static int vpu_release(struct inode *inode, struct file *filp)
{
    int ret = 0;
    int core = 0;
	unsigned long timeout = jiffies + HZ/2;	/* vpu wait timeout to 1/2 sec */

    //vpu_logi("enter. s_vpu_open_ref_count=%d \n", s_vpu_drv_context.open_count);

    wake_lock(&s_vpu_wakelock);

    ret = down_interruptible(&s_vpu_sem);
	if (0 != ret)
	{
        //vpu_loge("down_interruptible() failed.\n");
        wake_unlock(&s_vpu_wakelock);
		return -EINTR;
	}

    /* found and free the not handled buffer by user applications */
    vpu_free_buffers(filp);

    /* found and free the not closed instance by user applications */
    vpu_free_instances(filp);

    s_vpu_drv_context.open_count--;

    if (s_vpu_drv_context.open_count > 0)
    {
        //vpu_logi("can't close device, s_vpu_drv_context.open_count=%d\n", s_vpu_drv_context.open_count);
        up(&s_vpu_sem);
        wake_unlock(&s_vpu_wakelock);
        return 0;
    }

    vpu_clk_enable();
    /*====================END======================*/
    WriteVpuRegister(BIT_INT_ENABLE, 0);
    WriteVpuRegister(BIT_INT_CLEAR, 0x1);

    if (ReadVpuRegister(BIT_CUR_PC))
    {
        while (ReadVpuRegister(BIT_BUSY_FLAG))
        {
            if (time_after(jiffies, timeout))
            {
                //vpu_loge("############### wait vpu firmware idle timeout !!! ##############\n");
                break;
            }
        }
    }

	// Disable VPU hardware first.
	vpu_regulator_disable();

    /* free ade sram */
    if (s_vpu_sramDrvInfo.sram_size > 0)
    {
        //vpu_logi("DRV_ADEMemFree !!!\n");
        DRV_ADEMemFree();
    }
    memset(&s_vpu_sramDrvInfo, 0x00, sizeof(s_vpu_sramDrvInfo));

    if (s_instance_pool.size != 0)
    {
        if ((!IS_ERR((void *)s_instance_pool.base)) && (s_instance_pool.base != 0))
        {
            //vpu_logi(" ion_unmap_kernel:s_ion_instance_pool_client = %p, s_ion_instance_pool_handle = %p, s_instance_pool.base = 0x%lx\n",
                //s_ion_instance_pool_client, s_ion_instance_pool_handle, s_instance_pool.base);

            ion_unmap_kernel(s_ion_instance_pool_client, s_ion_instance_pool_handle);
        }

        memset(&s_instance_pool, 0x00, sizeof(vpudrv_buffer_t));

        if ((!IS_ERR(s_ion_instance_pool_handle)) && (s_ion_instance_pool_handle != NULL))
        {
            //vpu_logi("ion_free: s_ion_instance_pool_client = %p, s_ion_instance_pool_handle = %p\n", 
                //s_ion_instance_pool_client, s_ion_instance_pool_handle);

            ion_free(s_ion_instance_pool_client, s_ion_instance_pool_handle);
            s_ion_instance_pool_handle = NULL;
        }

        if ((!IS_ERR(s_ion_instance_pool_client)) && (s_ion_instance_pool_client != NULL))
        {
            //vpu_logi("ion_client_destroy: ion_instance_pool_client = %p\n", s_ion_instance_pool_client);

            ion_client_destroy(s_ion_instance_pool_client);
            s_ion_instance_pool_client = NULL;
        }
    }

    if (s_common_memory.size)
    {
        vpu_free_dma_buffer(&s_common_memory);
        memset(&s_common_memory, 0x00, sizeof(s_common_memory));
    }

    if (1 == s_vpu_systemMMU_support)
    {
        //vpu_logi("ion_unmap_kernel()\n");
        vmem_exit(&s_vmem);

        memset(&s_video_memory, 0x00, sizeof(vpudrv_buffer_t));

        if ((!IS_ERR(s_ion_reserved_mem_handle)) && (s_ion_reserved_mem_handle != NULL))
        {
            //vpu_logi("s_ion_reserved_mem_client = %p, s_ion_reserved_mem_handle = %p\n", s_ion_reserved_mem_client, s_ion_reserved_mem_handle);

            ion_free(s_ion_reserved_mem_client, s_ion_reserved_mem_handle);
            s_ion_reserved_mem_handle = NULL;
        }

        if ((!IS_ERR(s_ion_reserved_mem_client)) && (s_ion_reserved_mem_client != NULL))
        {
            //vpu_logi("s_ion_reserved_mem_client = %p\n", s_ion_reserved_mem_client);

            ion_client_destroy(s_ion_reserved_mem_client);
            s_ion_reserved_mem_client = NULL;
        }
    }

    vpu_off_notice_ade();
    up(&s_vpu_sem);

    wake_unlock(&s_vpu_wakelock);
    //vpu_logi("leave. s_vpu_open_ref_count=%d \n", s_vpu_drv_context.open_count);

	return 0;
}


static int vpu_fasync(int fd, struct file *filp, int mode)
{
	struct vpu_drv_context_t *dev = (struct vpu_drv_context_t *)filp->private_data;
	return fasync_helper(fd, filp, mode, &dev->async_queue);
}


static int vpu_map_to_register(struct file *fp, struct vm_area_struct *vm)
{
	unsigned long pfn;

	vm->vm_flags |= VM_IO | VM_RESERVED;
	vm->vm_page_prot = pgprot_noncached(vm->vm_page_prot);
	pfn = s_vpu_reg_phy_addr >> PAGE_SHIFT;

	return remap_pfn_range(vm, vm->vm_start, pfn, vm->vm_end-vm->vm_start, vm->vm_page_prot) ? -EAGAIN : 0;
}

static int hisi_vpu_mmap(struct ion_client *client, struct ion_handle *handle, struct vm_area_struct * vma)
{
        struct sg_table *table = NULL;
        struct scatterlist *sg = NULL;
        struct page *page = NULL;
        unsigned long remainder = 0;
        unsigned long len = 0;
        unsigned long addr = 0;
        int i = 0;
        int ret = 0;
        table = ion_sg_table(client, handle);
        BUG_ON(table == NULL);
        vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
        addr = vma->vm_start;
        for_each_sg(table->sgl, sg, table->nents, i) {
                page = sg_page(sg);
                remainder = vma->vm_end - addr;
                len = min(sg_dma_len(sg), remainder);
                ret = remap_pfn_range(vma, addr, page_to_pfn(page), len,
                        vma->vm_page_prot);
                if (ret != 0) {
                        printk(KERN_ERR "failed to remap_pfn_range! ret=%d\n",ret);
                }
                addr += len;
                if (addr >= vma->vm_end)
                        return 0;
        }

        return 0;
}


static int vpu_map_to_physical_memory(struct file *fp, struct vm_area_struct *vm)
{
    int ret = 0;

    //vpu_logd("vm_pgoff=%lu, vm_start=0x%lx, vm_end=0x%lx\n",
            //vm->vm_pgoff,
            //vm->vm_start,
            //vm->vm_end);

    if (1 == s_vpu_systemMMU_support)
    {

        if(vm->vm_pgoff == (s_instance_pool.phys_addr>>PAGE_SHIFT))        
        {
            if (NULL == s_ion_instance_pool_client)
            {
                return -EAGAIN;
            }
            else
            {
                vm->vm_page_prot = pgprot_noncached(vm->vm_page_prot);
                ret = hisi_vpu_mmap(s_ion_instance_pool_client, s_ion_instance_pool_handle, vm);
                if (ret != 0)
                {
                    //vpu_loge("hisi_vpu_mmap err.\n");
                    ret = -EAGAIN;
                }
            }
        
        }
        else 
        {
            if (NULL == s_ion_reserved_mem_handle)
            {
                return -EAGAIN;
            }
            else
            {
                vm->vm_page_prot = pgprot_noncached(vm->vm_page_prot);
                ret = hisi_vpu_mmap(s_ion_reserved_mem_client, s_ion_reserved_mem_handle, vm);
                if (ret != 0)
                {
                    //vpu_loge("hisi_vpu_mmap err.\n");
                    ret = -EAGAIN;
                }
            } 
        } 

    }
    else
    {
        vm->vm_flags |= VM_IO | VM_RESERVED;
        vm->vm_page_prot = pgprot_writecombine(vm->vm_page_prot);

        ret = remap_pfn_range(vm, vm->vm_start, vm->vm_pgoff, vm->vm_end-vm->vm_start, vm->vm_page_prot);
        if (ret != 0)
        {
            ret = -EAGAIN;
            //vpu_loge("remap_pfn_range err.\n");
        }
    }

    return ret;
}

static int vpu_map_to_instance_pool_memory(struct file *fp, struct vm_area_struct *vm)
{
    int ret = 0;

    //vpu_logd("vm_pgoff=%lu, vm_start=0x%lx, vm_end=0x%lx\n",vm->vm_pgoff,vm->vm_start,vm->vm_end);

    ret = remap_pfn_range(vm, vm->vm_start, vm->vm_pgoff, vm->vm_end-vm->vm_start, vm->vm_page_prot);
    if (ret != 0)
    {
        ret = -EAGAIN;
        //vpu_loge("remap_pfn_range err.\n");
    }

    return  ret;
}


/*!
 * @brief memory map interface for vpu file operation
 * @return  0 on success or negative error code on error
 */
static int vpu_mmap(struct file *fp, struct vm_area_struct *vm)
{
	if (vm->vm_pgoff) {
        if (s_vpu_systemMMU_support == 0)
        {
            if (vm->vm_pgoff == (s_instance_pool.phys_addr>>PAGE_SHIFT))
                return vpu_map_to_instance_pool_memory(fp, vm);
        }

		return vpu_map_to_physical_memory(fp, vm);
	} else {
		return vpu_map_to_register(fp, vm);
	}
}

struct file_operations vpu_fops = {
	.owner = THIS_MODULE,
	.open = vpu_open,
	.read = vpu_read,
	.write = vpu_write,
	/*.ioctl = vpu_ioctl, // for kernel 2.6.9 of C&M*/
	.unlocked_ioctl = vpu_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = vpu_compat_ioctl,
#endif //CONFIG_COMPAT
	.release = vpu_release,
	.fasync = vpu_fasync,
	.mmap = vpu_mmap,
};


static int vpu_probe(struct platform_device *pdev)
{
    int err = 0;
    struct resource *res = NULL;
    struct ion_heap_info_data mem_data;

    //vpu_logd("enter.\n");

    /* V8R2B020 not support vpu, z62576, 20140412, begin */
    if(0 == video_get_support_vpu()) {
        //vpu_loge("vpu_probe, platform not support vpu.\n");
        return -1;
    }
    /* V8R2B020 not support vpu, z62576, 20140412, end */


    s_vpu_systemMMU_support = 1;


	memset(&s_vpu_clk,0,sizeof(vpu_clk));

    if (pdev == NULL)
    {
        //vpu_loge("vpu_probe fail, pdev is NULL\n");
        return -1;
    }

    /* get vpu reg base addr */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (res) 	// if platform driver is implemented
    {
        s_vpu_reg_phy_addr = res->start;
        s_vpu_reg_virt_addr = ioremap(res->start, (res->end - res->start + 1));
        //vpu_logi("vpu reg phy addr=0x%x, reg virtual base=%p\n", s_vpu_reg_phy_addr , s_vpu_reg_virt_addr);
    }
    else
    {
        err = -ENODEV;
        //vpu_loge("vpu platform_get_resource(IORESOURCE_MEM) failed\n");
        goto ERROR_PROVE_DEVICE;
    }

    /* get the major number of the character device */
    if ((alloc_chrdev_region(&s_vpu_major, 0, 1, VPU_DEV_NAME)) < 0)
    {
        err = -EBUSY;
        //vpu_loge("could not allocate major number\n");
        goto ERROR_PROVE_DEVICE;
    }

    /* initialize the device structure and register the device with the kernel */
    cdev_init(&s_vpu_cdev, &vpu_fops);
    if ((cdev_add(&s_vpu_cdev, s_vpu_major, 1)) < 0)
    {
        err = -EBUSY;
        //vpu_loge("could not allocate chrdev\n");
        goto ERROR_PROVE_DEVICE;
    }

    /* register sysfs */
    cnm_vpu_class = class_create(THIS_MODULE, VPU_DEV_NAME);
    if (IS_ERR(cnm_vpu_class))
    {
        err = -EBUSY;
        //vpu_loge("can't creat vpu class\n");
        goto ERROR_PROVE_DEVICE;
    }

    cnm_vpu_dev = device_create(cnm_vpu_class, NULL, s_vpu_major, NULL, VPU_DEV_NAME);
    if (cnm_vpu_dev == NULL)
    {
        err = -EBUSY;
        //vpu_loge("can't creat vpu device\n");
        goto ERROR_PROVE_DEVICE;
    }

    /* get vpu irq */
    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (res) 	// if platform driver is implemented
    {
        s_vpu_irq = res->start;
        //vpu_logi("vpu irq number get from platform driver irq=0x%x\n", s_vpu_irq );
    }
    else
    {
        err = -ENODEV;
        //vpu_loge("vpu platform_get_resource(IORESOURCE_IRQ) failed\n");
        goto ERROR_PROVE_DEVICE;
    }

    err = request_irq(s_vpu_irq, vpu_irq_handler, 0, VPU_IRQ_NAME, (void *)(&s_vpu_drv_context));
    if (err)
    {
        //vpu_loge("fail to register interrupt handler\n");
        goto ERROR_PROVE_DEVICE;
    }

    if (1 != s_vpu_systemMMU_support)
    {
        if (0 != hisi_ion_get_heap_info(ION_VPU_HEAP_ID, &mem_data)) {
            //vpu_loge("hisi_ion_get_heap_info(codec) failed\n");
            goto ERROR_PROVE_DEVICE;
        }

        if (0 == mem_data.heap_size) {
            //vpu_loge("codec memory size is 0.\n");
            goto ERROR_PROVE_DEVICE;
        }

        s_video_memory.size = mem_data.heap_size;//HISI_MEM_CODEC_SIZE;
        s_video_memory.phys_addr = (unsigned long)(mem_data.heap_phy);
        s_video_memory.base = (unsigned long)video_ioremap(s_video_memory.phys_addr, PAGE_ALIGN(s_video_memory.size));

        //vpu_logd("s_video_memory.size = %u ,s_video_memory.phys_addr = 0x%x\n",
                 //s_video_memory.size, s_video_memory.phys_addr);

        if (!s_video_memory.base)
        {
            //vpu_loge("fail to remap video memory physical phys_addr=0x%x, base=0x%lx, size=%d\n",
                    //s_video_memory.phys_addr, s_video_memory.base, (int)s_video_memory.size);
            goto ERROR_PROVE_DEVICE;
        }

        if (vmem_init(&s_vmem, s_video_memory.phys_addr, s_video_memory.size) < 0)
        {
            //vpu_loge("fail to init vmem system\n");
            goto ERROR_PROVE_DEVICE;
        }

        //vpu_logd("success to probe vpu device with reserved video memory phys_addr=0x%x, base = 0x%x\n",
                //(int) s_video_memory.phys_addr, (int)s_video_memory.base);
    }

    return 0;

ERROR_PROVE_DEVICE:

    if (cnm_vpu_dev)
    {
        device_destroy(cnm_vpu_class, s_vpu_major);
    }

    if (cnm_vpu_class)
    {
        class_destroy(cnm_vpu_class);
    }

    cdev_del(&s_vpu_cdev);

    if (s_vpu_major)
    {
        unregister_chrdev_region(s_vpu_major, 1);
    }

    if (s_vpu_reg_virt_addr)
    {
        iounmap((void *)s_vpu_reg_virt_addr);
    }

    if (1 != s_vpu_systemMMU_support && s_video_memory.base)
    {
        video_iounmap((void *)s_video_memory.base);
    }

    return err;
}

static int vpu_remove(struct platform_device *pdev)
{
	//vpu_logd("vpu_remove\n");

	if (s_instance_pool.base) {
		vpu_free_dma_buffer(&s_instance_pool);
		s_instance_pool.base = 0;
	}

	if (s_common_memory.base) {
		vpu_free_dma_buffer(&s_common_memory);
		s_common_memory.base = 0;
	}


	if (s_video_memory.base) {
		video_iounmap((void *)s_video_memory.base);
        memset(&s_video_memory, 0x00, sizeof(vpudrv_buffer_t));
		vmem_exit(&s_vmem);
	}

    if (cnm_vpu_dev)
    {
        device_destroy(cnm_vpu_class, s_vpu_major);
    }

    if (cnm_vpu_class)
    {
        class_destroy(cnm_vpu_class);
    }

    cdev_del(&s_vpu_cdev);

	if (s_vpu_major > 0) {
		cdev_del(&s_vpu_cdev);
		unregister_chrdev_region(s_vpu_major, 1);
		s_vpu_major = 0;
	}

	if (s_vpu_irq)
		free_irq(s_vpu_irq, &s_vpu_drv_context);

	if (s_vpu_reg_virt_addr)
		iounmap(s_vpu_reg_virt_addr);

	return 0;
}

#ifdef CONFIG_PM
static int vpu_suspend(struct platform_device *pdev, pm_message_t state)
{
    int i;
    int core = 0;
    int ret_timeout                     = 0;
    unsigned long timeout               = jiffies + HZ/2;                       /* vpu wait timeout to 1/2 sec */
    unsigned long sem_timeout_jiffies   = 0;

    disable_irq(s_vpu_irq);
    //vpu_logi("enter vpu_suspend. \n");

    sem_timeout_jiffies = msecs_to_jiffies(2000);                               /* 2s */
    ret_timeout         = down_timeout(&s_vpu_sem, sem_timeout_jiffies);
    if (0 != ret_timeout)
    {
        /* if get semphore failed, it means ioctl in processing */
        //vpu_loge("down_timeout failed!!! \n");
        s_vpu_suspend_state = 0;
        return 0;
    }

    s_vpu_suspend_state = 1;

    if (s_vpu_drv_context.open_count > 0)
    {
        vpu_clk_enable();

        for (core = 0; core < MAX_NUM_VPU_CORE; core++)
        {
            if (s_bit_firmware_info[core].size == 0)
            {
                continue;
            }

            while (ReadVpuRegister(BIT_BUSY_FLAG))
            {
                if (time_after(jiffies, timeout))
                {
                    //vpu_logw("############### wait vpu firmware ilde timeout !!! \n");
                    break;
                }
            }

            s_run_index = ReadVpuRegister(BIT_RUN_INDEX);
            s_run_codstd = ReadVpuRegister(BIT_RUN_COD_STD);
            //vpu_logi("############### s_run_index=%d, s_run_codstd=%u \n", s_run_index, s_run_codstd);

            WriteVpuRegister(BIT_BUSY_FLAG, 1);
            WriteVpuRegister(BIT_RUN_COMMAND, VPU_SLEEP);

            while (ReadVpuRegister(BIT_BUSY_FLAG))
            {
                if (time_after(jiffies, timeout))
                {
                    //vpu_logw("############### wait vpu firmware ilde timeout !!! \n");
                    break;
                }
            }
        }

        for (core = 0; core < MAX_NUM_VPU_CORE; core++)
        {
            if (s_bit_firmware_info[core].size == 0)
            {
                continue;
            }

            for (i = 0; i < 64; i++)
            {
                s_vpu_reg_store[core][i] = ReadVpuRegister(BIT_BASE+(0x100+(i*4)));
            }
        }

        vpu_regulator_disable();

        if (s_vpu_sramDrvInfo.sram_size > 0)
        {
            //notify ade suspend
            vpu_pm_notify_ade(0);
            //vpu_logi("vpu pm notify ade suspend succ. \n");
        }
    }

    up(&s_vpu_sem);

    return 0;
}

static int vpu_resume(struct platform_device *pdev)
{
    int i;
    int core = 0;
    u32 val;
    int ret_timeout                     = 0;
    unsigned long timeout               = jiffies + HZ/2;	                    /* vpu wait timeout to 1/2 sec */
    unsigned long sem_timeout_jiffies   = 0;

    //vpu_logi("enter vpu_resume. \n");

    if (s_vpu_suspend_state == 0)
    {
        enable_irq(s_vpu_irq);
        //vpu_logw("s_vpu_suspend_state == 0 \n");
        return 0;
    }

    sem_timeout_jiffies = msecs_to_jiffies(2000);                               /* 2s */
    ret_timeout = down_timeout(&s_vpu_sem, sem_timeout_jiffies);
    if (0 != ret_timeout)
    {
        //vpu_loge("down_timeout failed!!! \n");
    }

    if (s_vpu_drv_context.open_count > 0)
    {
        if (s_vpu_sramDrvInfo.sram_size > 0)
        {
            //notify ade resume
            vpu_pm_notify_ade(1);
            //vpu_logi("vpu pm notify ade resume succ \n");
        }

        vpu_regulator_enable();

    	for (core = 0; core < MAX_NUM_VPU_CORE; core++)
        {
    		if (s_bit_firmware_info[core].size == 0)
            {
                continue;
            }

    		WriteVpuRegister(BIT_CODE_RUN, 0);

    		/*---- LOAD BOOT CODE*/
    		for (i = 0; i < 512; i++)
            {
    			val = s_bit_firmware_info[core].bit_code[i];
    			WriteVpuRegister(BIT_CODE_DOWN, ((i << 16) | val));
    		}

    		for (i = 0; i < 64; i++)
            {
                WriteVpuRegister(BIT_BASE+(0x100+(i * 4)), s_vpu_reg_store[core][i]);
            }

    		WriteVpuRegister(BIT_BUSY_FLAG, 1);
    		WriteVpuRegister(BIT_CODE_RESET, 1);
    		WriteVpuRegister(BIT_CODE_RUN, 1);

    		while (ReadVpuRegister(BIT_BUSY_FLAG))
            {
    			if (time_after(jiffies, timeout))
                {
                    //vpu_logw("############### wait vpu firmware ilde timeout !!! \n");
                    break;
                }
    		}

    		WriteVpuRegister(BIT_BUSY_FLAG, 1);
    		WriteVpuRegister(BIT_RUN_INDEX,   s_run_index);
    		WriteVpuRegister(BIT_RUN_COD_STD, s_run_codstd);
            //vpu_logi("############### s_run_index=%d, s_run_codstd=%u \n", s_run_index, s_run_codstd);

    		WriteVpuRegister(BIT_RUN_COMMAND, VPU_WAKE);

    		while (ReadVpuRegister(BIT_BUSY_FLAG))
            {
    			if (time_after(jiffies, timeout))
                {
                    //vpu_logw("############### wait vpu firmware ilde timeout !!! \n");
                    break;
                }
    		}

            if (ReadVpuRegister(BIT_CUR_PC) == 0)
            {
                //vpu_logw("############### BIT_CUR_PC == 0 \n");
            }
    	}

        if (s_vpu_frame_clock_state == 0)
        {
            //vpu_logi("############### s_vpu_frame_clock_state == 0 \n");
            vpu_clk_disable();
        }
    }

    s_vpu_suspend_state = 0;

    up(&s_vpu_sem);

    enable_irq(s_vpu_irq);

	return 0;
}
#else
#define	vpu_suspend	NULL
#define	vpu_resume	NULL
#endif				/* !CONFIG_PM */


static const struct of_device_id hisi_vpu_dt_match[] = {
	{.compatible = "hisi,cnm_vpu", },
	{}
};

static struct platform_driver vpu_driver = {
	.driver = {
           .name = VPU_DEV_NAME,
		   .of_match_table = hisi_vpu_dt_match
		   },
	.probe = vpu_probe,
	.remove = vpu_remove,
	.suspend = vpu_suspend,
	.resume = vpu_resume,
};

static void vpu_varible_init(void)
{
    //vpu_logd("enter.\n");

    memset(&s_vmem, 0x00, sizeof(s_vmem));
    memset(&s_video_memory, 0x00, sizeof(s_video_memory));
    memset(&s_instance_pool, 0x00, sizeof(s_instance_pool));
    memset(&s_common_memory, 0x00, sizeof(s_common_memory));
    memset(&s_vpu_drv_context, 0x00, sizeof(s_vpu_drv_context));
    memset(&s_vpu_cdev, 0x00, sizeof(s_vpu_cdev));
    memset(&s_interrupt_wait_q, 0x00, sizeof(s_interrupt_wait_q));
    memset(s_bit_firmware_info, 0x00, sizeof(s_bit_firmware_info));

    memset(&s_vpu_sramDrvInfo, 0x00, sizeof(s_vpu_sramDrvInfo));
    s_vpu_sramTimeout = false;

    s_vpu_major             = 0;
    s_vpu_irq               = IRQ_VDEC;
    s_vpu_reg_phy_addr      = SOC_VPU_S_BASE_ADDR;
    s_interrupt_flag        = 0;
    s_vpu_reg_virt_addr     = NULL;
    s_vpu_qos_virt_addr     = NULL;

    s_ion_dev               = NULL;
    
    s_ion_instance_pool_client  = NULL;
    s_ion_instance_pool_handle  = NULL;

    s_ion_reserved_mem_client   = NULL;
    s_ion_reserved_mem_handle   = NULL;    

    memset(s_vpu_reg_store, 0x00, sizeof(s_vpu_reg_store));
    s_run_index             = 0;
    s_run_codstd            = 0;
    s_vpu_frame_clock_state = 0;
    s_vpu_suspend_state     = 0;
}

static int __init vpu_init(void)
{
	int res;

	//vpu_logd("begin vpu_init\n");

	vpu_varible_init();

	init_waitqueue_head(&s_interrupt_wait_q);

    wake_lock_init(&s_vpu_wakelock, WAKE_LOCK_SUSPEND, "cnm_vpu");

	res = platform_driver_register(&vpu_driver);

	//vpu_logd("end vpu_init result=0x%x\n", res);
	return res;
}

static void __exit vpu_exit(void)
{
	//vpu_logd("vpu_exit\n");
	platform_driver_unregister(&vpu_driver);
	return;
}

static int vpu_regulator_enable(void)
{
	int ret = -1;

    //vpu_logd("vpu regulator enable.\n");

    ret = video_harden_regulator_enable(VIDEO_HARDEN_DEV_ID_VCODEC);
    if (0 !=ret )
    {
        //vpu_loge("video harden regulator enable failed!\n");
        return ret;
    }

    ret = vpu_clk_get();
	if (0 != ret) {
        //vpu_loge("get vcodec clock failed!\n");
        goto err;
	}

    ret = vpu_pll_clk_enable();
    if (0 != ret) {
        //vpu_loge("vpu pll clk enable failed!\n");
        goto err;
    }

#if 0
    /*media noc clk rate. config this when only run kernel*/
    /* MEDIA_SC NOC CLK DIV [0xCBC]: NOC rate 288M, BIT[7][6-0]*/
     phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,
                    SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_ADDR(CALC_REG_OFFSET),
                    SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_noc_value0_START,
                    SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_isp_sclk2_vld3_END,
                    0x85);
#endif

    /*for common image, z62576, 20140429, begin*/
    /* MEDIA_SC VPU DIV [0xCC4]: vpu cclk rate 333M*/
    if (0 != clk_set_rate(s_vpu_clk.clk_codec_vpu, s_vpu_clk.clk_freq)) {
        //vpu_loge("clk_codec_vpu set rate failed.\n");
        goto err;
    }

    /* MEDIA_SC VPU AXI CLK DIV [0xCC0]: vpu aclk rate 360M
       use the same clk dev register as the ADE core clk.*/
    if (0 != clk_set_rate(s_vpu_clk.aclk_codec_vpu, s_vpu_clk.aclk_freq)) {
        //vpu_loge("aclk_codec_vpu set rate failed.\n");
        goto err;
    }
    /*for common image, z62576, 20140429, end*/

#if VPU_AUTO_CCLK
    /* MEDIA_SC SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_ADDR [0x508]:codec vpu auto cclk */
     phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,
                    SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_ADDR(CALC_REG_OFFSET),
                    SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_auto_codec_vpu_clken_START,
                    SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_auto_codec_vpu_clken_END,
                    0x1);
#endif

    ret = video_harden_rstdis_isodis_clken(VIDEO_HARDEN_DEV_ID_VCODEC);
    if (0 != ret) {
        //vpu_loge("video harden rstdis_iso_dis_clken failed!\n");
        goto err;
    }

    /* MEDIA_SC VPU RST DIS [0x530]: VPU gate rst dis */
	phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,
				   SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(CALC_REG_OFFSET),
				   SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_codec_vpu_START,
			       SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_codec_vpu_END,
			       0x1);

    ret = vpu_clk_enable();
    if (0 != ret) {
        //vpu_loge("vpu clk enable failed!\n");
        goto err;
    }

    ret = video_harden_video_noc_enable(VIDEO_HARDEN_DEV_ID_VCODEC);
    if ( 0 != ret ) {
        //vpu_loge("video_harden_video_noc_enable failed!\n");
        goto err;
    }

    s_vpu_qos_virt_addr = ioremap(SOC_MEDIANOC_SERVICE_VIDEO_BASE_ADDR, PAGE_ALIGN(SZ_8K));
    //vpu_logi("qos phy addr=0x%x, qos vir addr=%p\n", SOC_MEDIANOC_SERVICE_VIDEO_BASE_ADDR, s_vpu_qos_virt_addr);

    if (s_vpu_qos_virt_addr)
    {
        /* MEDNOC SERVICE VIDEO Qos [0x30C]: VPU fixed Mode */
        iowrite32(0x0, s_vpu_qos_virt_addr+0x30c);

        /* MEDNOC SERVICE VIDEO Qos [0x308]: VPU priority */
        iowrite32(0x0303, s_vpu_qos_virt_addr+0x308);

        /* MEDNOC SERVICE VIDEO Qos [0x40C]: VPU_SEC fixed Mode */
        iowrite32(0x0, s_vpu_qos_virt_addr+0x40c);

        /* MEDNOC SERVICE VIDEO Qos [0x408]: VPU_SEC priority */
        iowrite32(0x0303, s_vpu_qos_virt_addr+0x408);

        iounmap((void *)s_vpu_qos_virt_addr);
    }

    /* SecAxi share Ade Sram */
    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,
                   SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(CALC_REG_OFFSET),
                   SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_vpu_mem_share_sel_START,
                   SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_vpu_mem_share_sel_END,
                   0x1);

#ifdef DUMP_VPU_POWER_REG
    vpu_dump_power_reg(1);
#endif

    //vpu_logi("regulator enable sucessful.\n");

    return 0;

 err:
    //vpu_loge("regulator enable  failed!\n");
    vpu_regulator_disable();
	return -1;
}

static void vpu_regulator_disable(void)
{
    //vpu_logd("vpu regulator disable.\n");

    video_harden_video_noc_disable(VIDEO_HARDEN_DEV_ID_VCODEC);

    vpu_clk_disable();

    /* MEDIA_SC SC_MEDIA_RSTEN [0x52C]: vpu gate rst enable */
    phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR,
                   SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(CALC_REG_OFFSET),
                   SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_codec_vpu_START,
                   SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_codec_vpu_END,
                   0x1);

    video_harden_clkdis_isoen_rsten(VIDEO_HARDEN_DEV_ID_VCODEC);

    vpu_pll_clk_disable();

    video_harden_regulator_disable(VIDEO_HARDEN_DEV_ID_VCODEC);

	vpu_clk_put();

#ifdef DUMP_VPU_POWER_REG
    vpu_dump_power_reg(0);
#endif

    //vpu_logi("regulator disable sucessful.\n");
}

static int vpu_clk_get(void)
{
    int ret = -1;
    struct device_node *np = NULL;
	struct platform_device *pdev=NULL;
	struct device *dev;

    //vpu_logi("enter. vpu_clk_get.\n");
	np = of_find_compatible_node(NULL, NULL, "hisi,cnm_vpu");
	if (np ==NULL) {
		//vpu_loge("the device node cnm_vpu is null\n");
		return ret;
	}

	pdev=of_find_device_by_node(np);
	if (pdev ==NULL) {
		//vpu_loge("the device cnm_vpu is null\n");
		return ret;
	}
	dev=&pdev->dev;

#if 0
    /*not need enable gpupll&medpll clock, it's invalid. 20140505*/
	/* get G3D PLL handle */
	s_vpu_clk.clk_gpupll_src  = clk_get(dev, "CLK_GPUPLL_SRC");
    if (IS_ERR(s_vpu_clk.clk_gpupll_src))
    {
    	//vpu_loge("vpu_clk_get:get CLK_GPUPLL_SRC clock failed.\n");
		return ret;
    }

	/* get MEDIA PLL handle */
	s_vpu_clk.clk_medpll_src  = clk_get(dev, "CLK_MEDPLL_SRC");
    if (IS_ERR(s_vpu_clk.clk_medpll_src))
    {
    	//vpu_loge("vpu_clk_get:get CLK_MEDPLL_SRC clock failed.\n");
		return ret;
    }
#endif

    /* 1. must enable clk_medpll_src_gated before all media_sctrl handle
       2. must disalbe clk_medpll_src_gated after all media_sctrl handle */
	/* get medpll src gated handle */
	s_vpu_clk.clk_medpll_src_gated = clk_get(dev, "CLK_MEDPLL_SRC_GATED");
    if (IS_ERR(s_vpu_clk.clk_medpll_src_gated)) {
        //vpu_loge("vpu_clk_get:get clk_medpll_src_gated failed.\n");
		return ret;
    }

	/* get VCODEC pclk handle */
	s_vpu_clk.pclk_codec_vpu  = clk_get(dev, "PCLK_CODEC_VPU");
    if (IS_ERR(s_vpu_clk.pclk_codec_vpu))
    {
    	//vpu_loge("vpu_clk_get:get PCLK_CODEC_VPU clock failed.\n");
		return ret;
    }

	/* get VCODEC aclk handle */
	s_vpu_clk.aclk_codec_vpu  = clk_get(dev, "ACLK_CODEC_VPU");
    if (IS_ERR(s_vpu_clk.aclk_codec_vpu))
    {
        //vpu_loge("vpu_clk_get:get ACLK_CODEC_VPU clock failed.\n");
		return ret;
    }

	/* get VCODEC cclk handle */
	s_vpu_clk.clk_codec_vpu   = clk_get(dev, "CLK_CODEC_VPU");
    if (IS_ERR(s_vpu_clk.clk_codec_vpu)) {
        //vpu_loge("vpu_clk_get:get CLK_CODEC_VPU clock failed.\n");
		return ret;
    }

    /*for common image, z62576, 20140429, begin*/
	if (of_property_read_u32(np, "VPU_CORE_CLOCK_FREQ", &(s_vpu_clk.clk_freq))){
		//vpu_loge("%s: read VPU_CORE_CLOCK_FREQ error.\n", __func__);
		return ret;
    }
    //vpu_logi("%s: read VPU_CORE_CLOCK_FREQ: %u.\n", __func__, s_vpu_clk.clk_freq);

	if (of_property_read_u32(np, "VPU_AXI_CLOCK_FREQ", &(s_vpu_clk.aclk_freq))){
		//vpu_loge("%s: read VPU_AXI_CLOCK_FREQ error.\n", __func__);
		return ret;
    }
    //vpu_logi("%s: read VPU_AXI_CLOCK_FREQ: %u.\n", __func__, s_vpu_clk.aclk_freq);

	if (of_property_read_u32(np, "VPU_WAIT_TIMEOUT_COEFF", &(s_vpu_clk.wait_timeout_coeff))){
		//vpu_loge("%s: read VPU_WAIT_TIMEOUT_COEFF error.\n", __func__);
		return ret;
    }
    //vpu_logi("%s: read VPU_WAIT_TIMEOUT_COEFF: %u.\n", __func__, s_vpu_clk.wait_timeout_coeff);

	if (of_property_read_u32(np, "VPU_SRAM_ALLOC_TIMEOUT", &(s_vpu_clk.sram_alloc_timeout))){
		//vpu_loge("%s: read VPU_SRAM_ALLOC_TIMEOUT error.\n", __func__);
		return ret;
    }
    //vpu_logi("%s: read VPU_SRAM_ALLOC_TIMEOUT: %u.\n", __func__, s_vpu_clk.sram_alloc_timeout);
    /*for common image, z62576, 20140429, end*/

    //vpu_logi("vpu_clk_get is successfull.\n");

	return 0;
}

static void vpu_clk_put(void)
{
    //vpu_logi("enter. vpu_clk_put.\n");

    /* put VCODEC cclk handle */
    if (NULL != s_vpu_clk.clk_codec_vpu)
    {
        clk_put(s_vpu_clk.clk_codec_vpu);
        s_vpu_clk.clk_codec_vpu = NULL;
    }

    /* put VCODEC aclk handle */
    if (NULL != s_vpu_clk.aclk_codec_vpu)
    {
        clk_put(s_vpu_clk.aclk_codec_vpu);
        s_vpu_clk.aclk_codec_vpu = NULL;
    }

    /* put VCODEC pclk handle */
    if (NULL != s_vpu_clk.pclk_codec_vpu)
    {
        clk_put(s_vpu_clk.pclk_codec_vpu);
        s_vpu_clk.pclk_codec_vpu = NULL;
    }

    /* put medpll src gated handle */
    if (NULL != s_vpu_clk.clk_medpll_src_gated)
    {
        clk_put(s_vpu_clk.clk_medpll_src_gated);
        s_vpu_clk.clk_medpll_src_gated = NULL;
    }

#if 0
    /*not need enable gpupll&medpll clock, it's invalid. 20140505*/
    /* put MEDIA PLL handle */
    if (NULL != s_vpu_clk.clk_medpll_src)
    {
        clk_put(s_vpu_clk.clk_medpll_src);
        s_vpu_clk.clk_medpll_src = NULL;
    }

    /* put G3D PLL handle */
    if (NULL != s_vpu_clk.clk_gpupll_src)
    {
        clk_put(s_vpu_clk.clk_gpupll_src);
        s_vpu_clk.clk_gpupll_src = NULL;
    }
#endif

    //vpu_logi("vpu_clk_put is successfull.\n");

    return;
}

static int vpu_clk_enable(void)
{
    int ret = -1;

    //vpu_logi("vpu clock enable.\n");

    if ((s_vpu_clk.clk_codec_vpu_bit == 1) && (s_vpu_clk.aclk_codec_vpu_bit == 1) && (s_vpu_clk.pclk_codec_vpu_bit == 1))
    {
        return 0;
    }

    /* open VCODEC pclk gate */
    if (s_vpu_clk.clk_codec_vpu_bit == 0)
    {
        if (0 != clk_prepare_enable(s_vpu_clk.pclk_codec_vpu))
        {
            //vpu_loge("pclk_codec_vpu enable failed!.\n");
            vpu_clk_disable();
            return ret;
        }
        s_vpu_clk.pclk_codec_vpu_bit  = 1;
    }

    /* open VCODEC aclk gate */
    if (s_vpu_clk.aclk_codec_vpu_bit == 0)
    {
        if (0 != clk_prepare_enable(s_vpu_clk.aclk_codec_vpu))
        {
            //vpu_loge("aclk_codec_vpu enable failed!.\n");
            vpu_clk_disable();
            return ret;
        }
        s_vpu_clk.aclk_codec_vpu_bit  = 1;
    }

    /* open VCODEC cclk gate */
    if (s_vpu_clk.clk_codec_vpu_bit == 0)
    {
        if (0 != clk_prepare_enable(s_vpu_clk.clk_codec_vpu))
        {
            //vpu_loge("clk_codec_vpu enable failed!.\n");
            vpu_clk_disable();
            return ret;
        }
        s_vpu_clk.clk_codec_vpu_bit   = 1;
    }

    //vpu_logi("vpu_clk_enable is successful\n");

    return 0;
}

static void vpu_clk_disable(void)
{
    //vpu_logi("enter. vpu clock disable.\n");

    if ((s_vpu_clk.clk_codec_vpu_bit == 0) && (s_vpu_clk.aclk_codec_vpu_bit == 0) && (s_vpu_clk.pclk_codec_vpu_bit == 0))
    {
        return;
    }

	/* VCODEC cclk disable */
	if ( (NULL != s_vpu_clk.clk_codec_vpu) && (1 == s_vpu_clk.clk_codec_vpu_bit) )
	{
	    clk_disable_unprepare(s_vpu_clk.clk_codec_vpu);
	    s_vpu_clk.clk_codec_vpu_bit = 0;
	}

	/* VCODEC aclk disable */
	if ( (NULL != s_vpu_clk.aclk_codec_vpu) && (1 == s_vpu_clk.aclk_codec_vpu_bit) )
	{
	    clk_disable_unprepare(s_vpu_clk.aclk_codec_vpu);
	    s_vpu_clk.aclk_codec_vpu_bit= 0;
	}

	/* VCODEC pclk disable */
	if ( (NULL != s_vpu_clk.pclk_codec_vpu) && (1 == s_vpu_clk.pclk_codec_vpu_bit) )
	{
	    clk_disable_unprepare(s_vpu_clk.pclk_codec_vpu);
	    s_vpu_clk.pclk_codec_vpu_bit = 0;
	}

    //vpu_logi("vpu_clk_disable is successful\n");

    return;
}

static int vpu_pll_clk_enable(void)
{
    int ret = -1;

    //vpu_logi("enter. vpu pll clock enable.\n");

    /*not need enable gpupll&medpll clock, it's invalid. 20140505*/
#if 0
    /* PMCTRL GPUPLLCTRL EN [0x008]: enable G3D PLL */
    if (0 != clk_prepare_enable(s_vpu_clk.clk_gpupll_src))
    {
        //vpu_loge("clk_gpupll_src enable failed!.");
        vpu_pll_clk_disable();
        return ret;
    }
    s_vpu_clk.clk_gpupll_src_bit= 1;

    /* PMCTRL MEDPLLCTRL EN [0x038]: enable MEDIA PLL */
    if (0 != clk_prepare_enable(s_vpu_clk.clk_medpll_src))
    {
        //vpu_loge("clk_medpll_src enable failed!.");
        vpu_pll_clk_disable();
        return ret;
    }
    s_vpu_clk.clk_medpll_src_bit= 1;
#endif

    /* medpll src gated enable */
    if (0 != clk_prepare_enable(s_vpu_clk.clk_medpll_src_gated))
    {
        //vpu_loge("clk_medpll_src_gated enable failed!.\n");
        vpu_pll_clk_disable();
        return ret;
    }
    s_vpu_clk.clk_medpll_src_gated_bit = 1;

    //vpu_logi("vpu_pll_clk_enable is successful.\n");

    return 0;
}

static void vpu_pll_clk_disable(void)
{
    //vpu_logi("enter. vpu pll clock disable.\n");

	/* medpll src gated disable */
	if ( (NULL != s_vpu_clk.clk_medpll_src_gated) && (1 == s_vpu_clk.clk_medpll_src_gated_bit) )
	{
	    clk_disable_unprepare(s_vpu_clk.clk_medpll_src_gated);
	    s_vpu_clk.clk_medpll_src_gated_bit = 0;
	}

    /*not need disable gpupll&medpll clock, it's invalid. 20140505*/
#if 0
	/* MEDIA PLL disable */
	if ( (NULL != s_vpu_clk.clk_medpll_src) && (1 == s_vpu_clk.clk_medpll_src_bit) )
	{
	    clk_disable_unprepare(s_vpu_clk.clk_medpll_src);
	    s_vpu_clk.clk_medpll_src_bit = 0;
	}

    /* G3D PLL disable */
	if ( (NULL != s_vpu_clk.clk_gpupll_src) && (1 == s_vpu_clk.clk_gpupll_src_bit) )
	{
	    clk_disable_unprepare(s_vpu_clk.clk_gpupll_src);
	    s_vpu_clk.clk_gpupll_src_bit = 0;
	}
#endif

    //vpu_logi("vpu_pll_clk_disable is successful.\n");

    return;
}

#ifdef DUMP_VPU_POWER_REG
static void vpu_dump_power_reg(int power)
{
    unsigned int reg_value;

    reg_value = phy_reg_readl(SOC_AO_SCTRL_BASE_ADDR, SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_ADDR(0), 0, 31);
    //vpu_logi("SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_ADDR = 0x%0x, VIDEO_HARDEN, BIT[2].\n", reg_value);

    if (1 == power) {
        reg_value = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(0), 0, 31);
        //vpu_logi("SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR = 0x%0x,  CLK SOURCE, BIT[6].\n", reg_value);
    }

    reg_value = phy_reg_readl(SOC_PMCTRL_BASE_ADDR, SOC_PMCTRL_GPUPLLCTRL_ADDR(0), 0, 31);
    //vpu_logi("SOC_PMCTRL_GPUPLLCTRL_ADDR = 0x%0x, GPUPLL, BIT[0].\n", reg_value);

    reg_value = phy_reg_readl(SOC_PMCTRL_BASE_ADDR, SOC_PMCTRL_MEDPLLCTRL_ADDR(0), 0, 31);
    //vpu_logi("SOC_PMCTRL_MEDPLLCTRL_ADDR = 0x%0x, MEDPLL, BIT[0].\n", reg_value);

    if (1 == power) {
        reg_value = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_ADDR(0), 0, 31);
        //vpu_logi("SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_ADDR = 0x%0x, VPU, BIT[7][3:0].\n", reg_value);

        reg_value = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR(0), 0, 31);
        //vpu_logi("SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR = 0x%0x, VPU, BIT[31][29:24].\n", reg_value);

        reg_value = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_ADDR(0), 0, 31);
        //vpu_logi("SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_ADDR = 0x%0x, VPU, BIT[23][18:16].\n", reg_value);
    }

    //reg_value = phy_reg_readl(SOC_PMCTRL_BASE_ADDR, SOC_PMCTRL_DDRCLKDIVCFG_ADDR(0), 0, 31);
    //vpu_logi("SOC_PMCTRL_DDRCLKDIVCFG_ADDR = 0x%0x,\n", reg_value);

    if (1 == power) {
        reg_value = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_ADDR(0), 0, 31);
        //vpu_logi("SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_ADDR = 0x%0x,  VPU, BIT[0].\n", reg_value);
    }

    reg_value = phy_reg_readl(SOC_AO_SCTRL_BASE_ADDR, SOC_AO_SCTRL_SC_PW_RST_STAT0_ADDR(0), 0, 31);
    //vpu_logi("SOC_AO_SCTRL_SC_PW_RST_STAT0_ADDR = 0x%0x, VIDEO_HARDEN, BIT[2].\n", reg_value);

    reg_value = phy_reg_readl(SOC_AO_SCTRL_BASE_ADDR, SOC_AO_SCTRL_SC_PW_ISO_STAT0_ADDR(0), 0, 31);
    //vpu_logi("SOC_AO_SCTRL_SC_PW_ISO_STAT0_ADDR = 0x%0x, VIDEO_HARDEN, BIT[2].\n", reg_value);

    reg_value = phy_reg_readl(SOC_AO_SCTRL_BASE_ADDR, SOC_AO_SCTRL_SC_PW_CLK_STAT0_ADDR(0), 0, 31);
    //vpu_logi("SOC_AO_SCTRL_SC_PW_CLK_STAT0_ADDR = 0x%0x, VIDEO_HARDEN, BIT[2].\n", reg_value);

    if (1 == power) {
        reg_value = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_ADDR(0), 0, 31);
        //vpu_logi("SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_ADDR = 0x%0x, VPU, BIT[2].\n", reg_value);

        reg_value = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_ADDR(0), 0, 31);
        //vpu_logi("SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_ADDR = 0x%0x, VPU, BIT[3].\n", reg_value);
    }

    reg_value = phy_reg_readl(SOC_AO_SCTRL_BASE_ADDR, SOC_AO_SCTRL_SC_PW_STAT1_ADDR(0), 0, 31);
    //vpu_logi("SOC_AO_SCTRL_SC_PW_STAT1_ADDR = 0x%0x, VIDEO_HARDEN, BIT[23][22].\n", reg_value);

    if (1 == power) {
        reg_value = phy_reg_readl(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(0), 0, 31);
        //pu_logi("SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR = 0x%0x,  VPU, BIT[8].\n", reg_value);
    }
}
#endif

int vpu_sram_callback(unsigned int memSize, unsigned int memAddress)
{
    if (!s_vpu_sramTimeout)
    {
        if (0 != memSize && 0 != memAddress)
        {
            s_vpu_sramDrvInfo.phys_addr = memAddress;
            s_vpu_sramDrvInfo.sram_size = memSize;
        }
        up(&s_vpu_semaphore);
    }
    return 0;
}

int vpu_allocate_sram_memory(unsigned int memSize, unsigned int alloc_sram_timeout)
{
    int ret                 = 0;
    unsigned long jiffies   = 0;
    int ret_timeout         = 0;

    if (0 == memSize || 0 == alloc_sram_timeout)
    {
        //vpu_loge("memSize=%d, alloc_sram_timeout=%d\n", memSize, alloc_sram_timeout);
        return -1;
    }

    jiffies = msecs_to_jiffies(alloc_sram_timeout);

    ret = DRV_ADEMemAlloc(memSize, &vpu_sram_callback);
    if (0 == ret)
    {
        ret_timeout = down_timeout(&s_vpu_semaphore, jiffies);
        if (0 != ret_timeout)
        {
            //vpu_logw("DRV_ADEMemAlloc timeout happens ---> DRV_ADEMemFree \n");
            s_vpu_sramTimeout = true;
            DRV_ADEMemFree();
            memset(&s_vpu_sramDrvInfo, 0x00, sizeof(s_vpu_sramDrvInfo));
            ret = -1;
        }
    }
    else
    {
        //vpu_logw("DRV_ADEMemAlloc fail\n");
        ret = -1;
    }

    return ret;
}


int vpu_hw_reset(void)
{
	//vpu_logd("request vpu reset from application. \n");
	return 0;
}



MODULE_AUTHOR("A customer using C&M VPU, Inc.");
MODULE_DESCRIPTION("VPU linux driver");
MODULE_LICENSE("GPL");

module_init(vpu_init);
module_exit(vpu_exit);

