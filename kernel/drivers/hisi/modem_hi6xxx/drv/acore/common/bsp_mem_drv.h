
#ifndef    _BSP_MEM_DRV_H_
#define    _BSP_MEM_DRV_H_

#include <linux/slab.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include "BSP.h"
#include "MemoryLayout.h"
#include "MemoryMap.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* ASIC2 调试 */

#ifndef INLINE
#define INLINE __inline__
#endif

/**************************************************************************
  宏定义
**************************************************************************/
/* 长度定义 */
#define MEM_ICC_DDR_POOL_SIZE       (MEMORY_RAM_CORESHARE_MEMMGR_SIZE)

/* Mem 全局变量放在AXI中 */
#define MEM_CTX_ADDR                (IO_ADDRESS(MEMORY_AXI_MEMMGR_FLAG_ADDR))

/* 基地址定义 */
#define MEM_ICC_DDR_POOL_BASE_ADDR  (IO_ADDRESS(MEMORY_RAM_CORESHARE_MEMMGR_ADDR))
#define MEM_AXI_DICC_ADDR           (IO_ADDRESS(MEMORY_AXI_DICC_ADDR))
#define MEM_AXI_TEMP_PROTECT_ADDR   (IO_ADDRESS(MEMORY_AXI_TEMP_PROTECT_ADDR))
#define MEM_AXI_USB_CSHELL_ADDR     (IO_ADDRESS(MEMORY_AXI_USB_CSHELL_ADDR))
#define MEM_CORESHARE_MEM_ADDR      (IO_ADDRESS(MEMORY_RAM_CORESHARE_MEM_ADDR))
#define MEM_CORESHARE_SYNC_ADDR     (IO_ADDRESS(MEMORY_RAM_CORESHARE_SYNC_ADDR))



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _BSP_MEM_DRV_H_ */



