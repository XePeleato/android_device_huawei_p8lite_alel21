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
 */

#ifndef __VPU_DRV_H__
#define __VPU_DRV_H__

#include <linux/fs.h>
#include <linux/types.h>


#define VDI_IOCTL_MAGIC  'V'
#define VDI_IOCTL_ALLOCATE_PHYSICAL_MEMORY	_IO(VDI_IOCTL_MAGIC, 0)
#define VDI_IOCTL_FREE_PHYSICALMEMORY		_IO(VDI_IOCTL_MAGIC, 1)
#define VDI_IOCTL_WAIT_INTERRUPT			_IO(VDI_IOCTL_MAGIC, 2)
#define VDI_IOCTL_SET_CLOCK_GATE			_IO(VDI_IOCTL_MAGIC, 3)
#define VDI_IOCTL_RESET                     _IO(VDI_IOCTL_MAGIC, 4)
#define VDI_IOCTL_GET_INSTANCE_POOL			_IO(VDI_IOCTL_MAGIC, 5)
#define VDI_IOCTL_GET_COMMON_MEMORY			_IO(VDI_IOCTL_MAGIC, 6)
#define VDI_IOCTL_GET_RESERVED_VIDEO_MEMORY_INFO _IO(VDI_IOCTL_MAGIC, 8)
#define VDI_IOCTL_OPEN_INSTANCE				_IO(VDI_IOCTL_MAGIC, 9)
#define VDI_IOCTL_CLOSE_INSTANCE			_IO(VDI_IOCTL_MAGIC, 10)
#define VDI_IOCTL_GET_INSTANCE_NUM			_IO(VDI_IOCTL_MAGIC, 11)



#ifdef CONFIG_CNM_VPU_HISI_PLATFORM
#define VDI_IOCTL_GET_SYSTEMMMU_SURPPORT    _IO(VDI_IOCTL_MAGIC, 12)
#define VDI_IOCTL_RESERVED_MEMORY_ALLOC     _IO(VDI_IOCTL_MAGIC, 13)
#define VDI_IOCTL_FLUSH_ION_CACHE           _IO(VDI_IOCTL_MAGIC, 14)

#define VDI_IOCTL_ALLOCATE_SRAM_MEMORY      _IO(VDI_IOCTL_MAGIC, 16)
#define VDI_IOCTL_FREE_SRAM_MEMORY          _IO(VDI_IOCTL_MAGIC, 17)
#define VDI_IOCTL_GET_VSCREENINFO	    _IO(VDI_IOCTL_MAGIC, 18)

typedef struct SramDrvInfo {
   unsigned int sram_size;
   unsigned int phys_addr;
} SramDrvInfo;
#endif //CONFIG_CNM_VPU_HISI_PLATFORM


typedef struct vpudrv_buffer_t {
	unsigned int size;
	unsigned int phys_addr;
	unsigned long base;	     /*kernel logical address in use kernel*/
	unsigned long virt_addr; /* virtual user space address */
} vpudrv_buffer_t;

#ifdef CONFIG_CNM_VPU_HISI_PLATFORM
#ifdef CONFIG_COMPAT
typedef struct compat_vpudrv_buffer_t {
	unsigned int size;
	unsigned int phys_addr;
	unsigned long long base;	     /*kernel logical address in use kernel*/
	compat_ulong_t virt_addr; /* virtual user space address */
} compat_vpudrv_buffer_t;
#endif
#endif

typedef struct vpu_bit_firmware_info_t {
	unsigned int size;		/* size of this structure */
	unsigned int core_idx;
	unsigned int reg_base_offset;
	unsigned short bit_code[512];
} vpu_bit_firmware_info_t;

typedef struct vpudrv_inst_info_t {
	unsigned int core_idx;
	unsigned int inst_idx;
	int inst_open_count;   /* for output only */
} vpudrv_inst_info_t;



#define VPU_DEBUG 1


#define vpu_loge(fmt, ...) printk(KERN_ERR     "[ERROR][" LOG_TAG "]" "[F:%s-L:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define vpu_logw(fmt, ...) printk(KERN_WARNING "[WARN ][" LOG_TAG "]" "[F:%s-L:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define vpu_logi(fmt, ...) printk(KERN_WARNING "[INFO ][" LOG_TAG "]" "[F:%s-L:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#if VPU_DEBUG
#define vpu_logd(fmt, ...) printk(KERN_WARNING "[DEBUG][" LOG_TAG "]" "[F:%s-L:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define vpu_logd(fmt, ...)
#endif


#endif
