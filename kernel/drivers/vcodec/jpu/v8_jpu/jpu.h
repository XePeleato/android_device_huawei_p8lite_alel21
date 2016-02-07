#ifndef __JPU_DRV_H__
#define __JPU_DRV_H__

#include <linux/fs.h>
#include <linux/types.h>

#ifndef MIT2_UT_SWITCH
#define STATIC static
#else
#define STATIC
#endif //MIT2_UT_SWITCH

#define JDI_IOCTL_MAGIC  'J'


#define JDI_IOCTL_ALLOCATE_PHYSICAL_MEMORY			_IO(JDI_IOCTL_MAGIC, 0)
#define JDI_IOCTL_FREE_PHYSICALMEMORY			_IO(JDI_IOCTL_MAGIC, 1)
#define JDI_IOCTL_WAIT_INTERRUPT			_IO(JDI_IOCTL_MAGIC, 2)
#define JDI_IOCTL_SET_CLOCK_GATE			_IO(JDI_IOCTL_MAGIC, 3)
#define JDI_IOCTL_RESET						_IO(JDI_IOCTL_MAGIC, 4)
#define JDI_IOCTL_GET_INSTANCE_POOL			_IO(JDI_IOCTL_MAGIC, 5)
#define JDI_IOCTL_GET_RESERVED_VIDEO_MEMORY_INFO			_IO(JDI_IOCTL_MAGIC, 6)
#define JDI_IOCTL_GET_SYSTEMMMU_SURPPORT    _IO(JDI_IOCTL_MAGIC, 7)
#define JDI_IOCTL_RESERVED_MEMORY_ALLOC     _IO(JDI_IOCTL_MAGIC, 8)
#define JDI_IOCTL_FLUSH_ION_CACHE           _IO(JDI_IOCTL_MAGIC, 9)


typedef struct jpudrv_buffer_t {
	unsigned int size;
#ifdef CONFIG_CNM_JPU_HISI_PLATFORM
	unsigned int phys_addr;
#else
	unsigned long phys_addr;
#endif
	unsigned long base;							/* kernel logical address in use kernel */
	unsigned long virt_addr;				/* virtual user space address */
} jpudrv_buffer_t;

#ifdef CONFIG_CNM_JPU_HISI_PLATFORM
#ifdef CONFIG_COMPAT
typedef struct compat_jpudrv_buffer_t {
	unsigned int size;
	unsigned int phys_addr;
	unsigned long long base;							/* kernel logical address in use kernel */
	compat_ulong_t virt_addr;				/* virtual user space address */
} compat_jpudrv_buffer_t;
#endif
#endif

#define JPU_DEBUG 1

#define jpu_loge(fmt, ...) printk(KERN_ERR     "[ERROR]["LOG_TAG"]" "[%s:%d:]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define jpu_logw(fmt, ...) printk(KERN_WARNING "[WARN]["LOG_TAG"]" "[%s:%d:]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define jpu_logi(fmt, ...) printk(KERN_WARNING "[INFO]["LOG_TAG"]" "[%s:%d:]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#if JPU_DEBUG
#define jpu_logd(fmt, ...) printk(KERN_WARNING "[DEBUG]["LOG_TAG"]" "[%s:%d:]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define jpu_logd(fmt, ...)
#endif


#endif
