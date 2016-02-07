

#ifndef __JPU_DRV_H__
#define __JPU_DRV_H__

#include <linux/fs.h>
#include <linux/types.h>

#define JDI_IOCTL_MAGIC  'J'
#if 0
#define CRGPERI_BASE_ADDR	0xFFF35000
#define CRGPERI_CLKDIV18	0x0F0
#define CRGPERI_CLKDIV19	0x0F4
#define CRGPERI_PEREN0	0x000
#define CRGPERI_PERRSTDIS0	0x064
#define CRGPERI_PERPWREN	0x150
#define CRGPERI_ISODIS	0x148
#define CRGPERI_PEREN5	0x050
#define CRGPERI_PEREN3	0x030
#define CRGPERI_PERRSTDIS2	0x07C
#define CRGPERI_PERRSTDIS3	0x088
#endif 
#define PMC_BASE_ADDR	0xFFF31000
#define PMC_NOC_POWER_IDLEREQ	0x380
#define PMC_NOC_POWER_IDLEACK	0x384
#define PMC_NOC_POWER_IDLE	0x388

//#define JPU_MMU
//#define CAMERA_TEST
#define JDI_IOCTL_ALLOCATE_PHYSICAL_MEMORY          _IO(JDI_IOCTL_MAGIC, 0)
#define JDI_IOCTL_FREE_PHYSICALMEMORY               _IO(JDI_IOCTL_MAGIC, 1)
#define JDI_IOCTL_WAIT_INTERRUPT                    _IO(JDI_IOCTL_MAGIC, 2)
#define JDI_IOCTL_SET_CLOCK_GATE                    _IO(JDI_IOCTL_MAGIC, 3)
#define JDI_IOCTL_RESET                             _IO(JDI_IOCTL_MAGIC, 4)
#define JDI_IOCTL_GET_INSTANCE_POOL                 _IO(JDI_IOCTL_MAGIC, 5)
#define JDI_IOCTL_GET_RESERVED_VIDEO_MEMORY_INFO    _IO(JDI_IOCTL_MAGIC, 6)


typedef struct jpudrv_buffer_t {
    unsigned int size;
    unsigned long long phys_addr;
    unsigned long base;                         /* kernel logical address in use kernel */
    unsigned long virt_addr;                /* virtual user space address */
} jpudrv_buffer_t;

#define JPU_BASE_ADDR 0xE8960000
#define JPU_REG_SIZE 0x7FFF


#ifdef JPU_MMU
#define JPU_MMU_SIZE            (0x400000) //MMU size is 4M
#define JPU_MMU_PHYSICAL_BASE   (0x00000000)
#define JPU_PHYSICAL_SIZE       (20 * 1024 * 1024)//YUV420 64*1.5 = 96M /YUV422 64*2 = 128M/YUV444 64*3 = 192M
#define JPU_MMU_PHYSICAL_END    (JPU_MMU_PHYSICAL_BASE + JPU_PHYSICAL_SIZE)

#define JPU_MMU_PAGE_BASE   (JPU_MMU_PHYSICAL_END + 0x1000)
#define JPU_MMU_PAGE_END    (JPU_MMU_PAGE_BASE + JPU_MMU_SIZE)
void creat_page_table(void);

#endif
//mmu register
#define JPU_MMU_ADDR        0xE8961000
#define JPU_MMU_REG_SIZE    0x6FFF
#define NPT_MMU_BASE        0x1000
#define JPU_MMU_Y_VA_BASE   (NPT_MMU_BASE + 0x0014)
#define JPU_MMU_Y_VA_END    (NPT_MMU_BASE + 0x0018)
#define JPU_MMU_U_VA_BASE   (NPT_MMU_BASE + 0x001C)
#define JPU_MMU_U_VA_END    (NPT_MMU_BASE + 0x0020)
#define JPU_MMU_V_VA_BASE   (NPT_MMU_BASE + 0x0024)
#define JPU_MMU_V_VA_END    (NPT_MMU_BASE + 0x0028)
#define JPU_MMU_B_VA_BASE   (NPT_MMU_BASE + 0x002C)
#define JPU_MMU_B_VA_END    (NPT_MMU_BASE + 0x0030)
#define JPU_MMU_Y_PT_BASE   (NPT_MMU_BASE + 0x0034)
#define JPU_MMU_U_PT_BASE   (NPT_MMU_BASE + 0x0038)
#define JPU_MMU_V_PT_BASE   (NPT_MMU_BASE + 0x003C)
#define JPU_MMU_B_PT_BASE   (NPT_MMU_BASE + 0x0040)
#define JPU_MMU_CTRL        (NPT_MMU_BASE + 0x0008)
#define JPU_MMU_EN          (NPT_MMU_BASE + 0x000C)
#define JPU_MMU_MODE        (NPT_MMU_BASE + 0x0010)
#define JPU_MJPEG_SIZE      (NPT_MMU_BASE + 0x0044)
#define JPU_MJPEG_STRIDE    (NPT_MMU_BASE + 0x0048)

#endif
