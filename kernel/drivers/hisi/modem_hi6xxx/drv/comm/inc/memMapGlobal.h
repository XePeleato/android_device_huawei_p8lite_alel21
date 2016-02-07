/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  memMapGlobal.h
*
*   作    者 :  liumengcun
*
*   描    述 :  单板整体内存分配
*
*   修改记录 :  2011年12月20日  v1.00  liumengcun  创建
*
*************************************************************************/
#ifndef __MEM_MAP_GLOBAL_H__
#define __MEM_MAP_GLOBAL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "product_config.h"
#include "MemoryMap.h"

/* 老编译工具使用的部分，新工具移到了product_config.h中
 */
 #ifdef  _DRV_LLT_
#ifndef BALONG_OBUILD_VERSION
#define BALONG_OBUILD_VERSION 10 
#endif 
#endif 
#ifndef BALONG_OBUILD_VERSION
/*-------------------------------------------------------------------------------
                            单板内存使用分配,V7R1/PV500
  -------------------------------------------------------------------------------

                        | SOCP_BASE(0x1800000)          |UDP板可以扩展使用后128M地址空间
                         ------------------------------- 0x38000000
                        | ACORE_ACP(0x200000)           |
                         ------------------------------- 0x37E00000
                        | SOCP_BASE(0x800000)           | USB STICK,E5采用中间的8M地址空间
                         ------------------------------- 0x37600000
                        | ACORE_BASE(0x2000000)         |
                         ------------------------------- 0x35600000
                        | GU_RESERVED(0x1200000)        |
                         ------------------------------- 0x34400000
                        | CORE_SHARE(0x400000)          |
                         ------------------------------- 0x34000000
                        | PM_USER_RESERVED(0x6000)      |
                         ------------------------------- 0x33FFA000
                        | MCORE_USER_RESERVED(0x100000) |
                         ------------------------------- 0x33EFA000
                        | MCORE BASE(0x3CFA000)         |
                         ------------------------------- 0x30200000
                        | MCORE ACP(0x200000)           |
                         ------------------------------- 0x30000000
-------------------------------------------------------------------------------*/


/* 单板内存基地址*/
#if((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (defined(BOARD_ASIC) || defined (BOARD_SFT)))
#define GLOBAL_MEM_BASE_ADDR                   0x30000000
#elif ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && defined(BOARD_FPGA))
#define GLOBAL_MEM_BASE_ADDR                   0xC0000000
#else
#error "wrong board type"
#endif

/* ACORE ACP使用基地址*/
#define GLOBAL_MEM_ACORE_ACP_ADDR              GLOBAL_MEM_BASE_ADDR
#define GLOBAL_MEM_ACORE_ACP_SIZE              0x000000

/* MCORE使用内存基地址*/
#define GLOBAL_MEM_MCORE_BASE_ADDR             ((GLOBAL_MEM_ACORE_ACP_ADDR) + (GLOBAL_MEM_ACORE_ACP_SIZE))
#if (  defined(CHIP_BB_6920CS) \
     &&(defined(BOARD_FPGA) || defined(BOARD_SFT) || defined(FEATURE_BOARD_STUB_BJ_UDP)))
#define GLOBAL_MEM_MCORE_BASE_SIZE             0x4FFA000
#else
#define GLOBAL_MEM_MCORE_BASE_SIZE             0x36FA000
#endif

/* MCORE操作系统保留内存，记录EDR内容*/
#define GLOBAL_MEM_MCORE_PM_RESERVED_ADDR      ((GLOBAL_MEM_MCORE_BASE_ADDR) + (GLOBAL_MEM_MCORE_BASE_SIZE))
#define GLOBAL_MEM_MCORE_PM_RESERVED_SIZE      0x6000

/* MCORE保留内存基地址，底软内部使用，可维可测等*/
#define GLOBAL_MEM_MCORE_USER_RESERVED_ADDR    ((GLOBAL_MEM_MCORE_PM_RESERVED_ADDR) + (GLOBAL_MEM_MCORE_PM_RESERVED_SIZE))
#define GLOBAL_MEM_MCORE_USER_RESERVED_SIZE    0x100000

/* 核间共享内存基地址*/
#define GLOBAL_MEM_CORE_SHARE_ADDR             ((GLOBAL_MEM_MCORE_USER_RESERVED_ADDR) + (GLOBAL_MEM_MCORE_USER_RESERVED_SIZE))
#define GLOBAL_MEM_CORE_SHARE_SIZE             0x100000

/* ACORE使用内存基地址*/
#define GLOBAL_MEM_ACORE_BASE_ADDR             ((GLOBAL_MEM_CORE_SHARE_ADDR) + (GLOBAL_MEM_CORE_SHARE_SIZE))
#define GLOBAL_MEM_ACORE_BASE_SIZE             0x2C00000

/* 给GU预留的内存基地址*/
#define GLOBAL_MEM_GU_RESERVED_ADDR            ((GLOBAL_MEM_ACORE_BASE_ADDR) + (GLOBAL_MEM_ACORE_BASE_SIZE))
#if (  defined(CHIP_BB_6920CS) \
     &&(defined(BOARD_FPGA) || defined(BOARD_SFT) || defined(FEATURE_BOARD_STUB_BJ_UDP)))
    #define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       0xF3200000
#else
    #define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       0xF3900000
#endif
#define GLOBAL_MEM_GU_RESERVED_SIZE            0x1500000

#define GLOBAL_LNV_BASE_ADDR                   ((GLOBAL_MEM_GU_RESERVED_ADDR) + (GLOBAL_MEM_GU_RESERVED_SIZE))
#define GLOBAL_LNV_BASE_SIZE                   0x80000

/* LCS基地址*/

#define GLOBAL_MEM_LCS_ADDR                   ((GLOBAL_LNV_BASE_ADDR) + (GLOBAL_LNV_BASE_SIZE))
#define GLOBAL_MEM_LCS_SIZE                   0x280000

#if((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (defined(BOARD_ASIC) || defined (BOARD_SFT)))
#if defined(FEATURE_BOARD_STUB_BJ_E5776S) || defined (FEATURE_BOARD_STUB_BJ_STICK)
/* SOCP使用基地址*/
#define GLOBAL_MEM_SOCP_ADDR                   ((GLOBAL_MEM_LCS_ADDR) + (GLOBAL_MEM_LCS_SIZE))
#define GLOBAL_MEM_SOCP_SIZE                   0x200000

#elif (defined(FEATURE_BOARD_STUB_BJ_UDP) || defined(FEATURE_BOARD_STUB_BJ_SFT)) /* UDP单板
 */
/* HIFI使用地址*/
#define GLOBAL_MEM_HIFI_ADDR                  ((GLOBAL_MEM_LCS_ADDR) + (GLOBAL_MEM_LCS_SIZE))
#define GLOBAL_MEM_HIFI_SIZE                  0x100000

/* SOCP使用基地址*/
#define GLOBAL_MEM_SOCP_ADDR                   ((GLOBAL_MEM_HIFI_ADDR) + (GLOBAL_MEM_HIFI_SIZE))
#define GLOBAL_MEM_SOCP_SIZE                   0x1800000

#else
#error "wrong board type"
#endif
#elif ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && defined(BOARD_FPGA))
/* SOCP使用基地址*/
#define GLOBAL_MEM_SOCP_ADDR                   ((GLOBAL_MEM_LCS_ADDR) + (GLOBAL_MEM_LCS_SIZE))
#define GLOBAL_MEM_SOCP_SIZE                   0x1800000

#else
#error "wrong board type"
#endif

/* MCORE ACP基地址*/
#define GLOBAL_MEM_MCORE_ACP_ADDR              ((GLOBAL_MEM_SOCP_ADDR) + (GLOBAL_MEM_SOCP_SIZE))
#define GLOBAL_MEM_MCORE_ACP_SIZE              0x000000


/*-------------------------------------------------------------------------------
                            共享内存,V7R1
  -------------------------------------------------------------------------------
                         ----------------------- 0x38400000
                        |  SYNC(0x60)           |
                         ----------------------- 0x383FFFA0
                        |  MEM spinlock(0x20)   |
                         ----------------------- 0x383FFF80
                        |  ICC(0x80)            |
                         ----------------------- 0x383FFF00
                        |  MULT BAND(0x8000)   |
                         ----------------------- 0x383F7F00
                        |  IPC(0x20)           |
                         ----------------------- 0x383F7EE0               
                        |  WAN(0x5000)         |
                         ----------------------- 0x383F2EE0
                        |  MEM MGR M            |
                         ----------------------- 0x38000000
-------------------------------------------------------------------------------*/
/* 从顶往下分配，以下地址和大小不可改变 */
#define CORESHARE_MEM_ADDR                     GLOBAL_MEM_CORE_SHARE_ADDR
#define CORESHARE_MEM_SIZE                     GLOBAL_MEM_CORE_SHARE_SIZE
#define CORESHARE_MEM_TOP_ADDR                 ((CORESHARE_MEM_ADDR) + (CORESHARE_MEM_SIZE))

#define CORESHARE_MEM_SYNC_SIZE                0x60
#define CORESHARE_MEM_SYNC_ADDR                ((CORESHARE_MEM_TOP_ADDR) - (CORESHARE_MEM_SYNC_SIZE))
    
#define CORESHARE_MEM_SPINLOCK_SIZE            0x20
#define CORESHARE_MEM_SPINLOCK_ADDR            ((CORESHARE_MEM_SYNC_ADDR) - (CORESHARE_MEM_SPINLOCK_SIZE))

#define CORESHARE_MEM_ICC_SIZE                 0x80
#define CORESHARE_MEM_ICC_ADDR                 ((CORESHARE_MEM_SPINLOCK_ADDR) - (CORESHARE_MEM_ICC_SIZE))

/* 多band预留内存,装备CT BT使用*/
#define CORESHARE_MEM_TENCILICA_MULT_BAND_SIZE  0x8000
#define CORESHARE_MEM_TENCILICA_MULT_BAND_ADDR ((CORESHARE_MEM_ICC_ADDR) - (CORESHARE_MEM_TENCILICA_MULT_BAND_SIZE))

#define CORESHARE_MEM_IPC_SIZE                 0x20
#define CORESHARE_MEM_IPC_ADDR                 ((CORESHARE_MEM_TENCILICA_MULT_BAND_ADDR) - (CORESHARE_MEM_IPC_SIZE))

/* 硬件信息保留区，根据HKADC动态填充 */
#define CORESHARE_MEM_HW_INFO_SIZE             0x40
#define CORESHARE_MEM_HW_INFO_ADDR              ((CORESHARE_MEM_IPC_ADDR) - (CORESHARE_MEM_HW_INFO_SIZE))

#define CORESHARE_MEM_WAN_SIZE                 0x8000
#define CORESHARE_MEM_WAN_ADDR                 ((CORESHARE_MEM_HW_INFO_ADDR) - (CORESHARE_MEM_WAN_SIZE))

#define CORESHARE_MEM_MEMMGR_SIZE              ((CORESHARE_MEM_WAN_ADDR) - (CORESHARE_MEM_MEMMGR_ADDR))
#define CORESHARE_MEM_MEMMGR_ADDR              CORESHARE_MEM_ADDR


/*GU NV预留内存空间宏定义,P500基地址为0xCxxxxxxx，使用IO_ADDRESS可能导致两块不同
的实地址映射到同一块虚地址,所以此虚地址单独直接定义*/
#define GLOBAL_MEM_GUNV_RESV_VIRT       (0xFF3B0000)
#define GLOBAL_MEM_GUNV_RESV_PHYS       (GLOBAL_MEM_GU_RESERVED_ADDR + GLOBAL_MEM_GU_RESERVED_SIZE - 0x50000)
#define GLOBAL_MEM_GUNV_RESV_SIZE       (0x30000)

#if 0
#if((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (defined(BOARD_ASIC) || defined (BOARD_SFT)))

/* 温保特性OM与GUL DSP共享地址，用于传递温度值，需要与BSP_MEMORY.h保持一致，该地址不能被修改*/
#define GLOBAL_AXI_TEMP_PROTECT_ADDR        0x2FFFE218
#define GLOBAL_AXI_TEMP_PROTECT_SIZE        (128)

/* GU使用的DICC特性地址，需要与BSP_MEMORY.h保持一致，该地址不能被修改*/
#define GLOBAL_AXI_DICC_ADDR                0x2FFFFEC0
#define GLOBAL_AXI_DICC_SIZE                (32)
#elif ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && defined(BOARD_FPGA))
/* GU使用的DICC特性地址，需要与BSP_MEMORY.h保持一致，该地址不能被修改*/
#define GLOBAL_AXI_DICC_ADDR                0x3000D3C0
#define GLOBAL_AXI_DICC_SIZE                (32)

/* 温保特性OM与GUL DSP共享地址，用于传递温度值，需要与BSP_MEMORY.h保持一致，该地址不能被修改*/
#define GLOBAL_AXI_TEMP_PROTECT_ADDR        0x3000D3E0
#define GLOBAL_AXI_TEMP_PROTECT_SIZE        (128)

#else
#error "wrong board type"
#endif
#endif

#endif /*#ifndef BALONG_OBUILD_VERSION
 */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


#endif /* end of __MEM_MAP_GLOBAL_H__ */

