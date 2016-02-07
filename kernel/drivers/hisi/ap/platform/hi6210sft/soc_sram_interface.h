/******************************************************************************

                 版权所有 (C), 2001-2012, 华为技术有限公司

 ******************************************************************************
  文 件 名   : soc_sram_interface.h
  版 本 号   : 初稿
  作    者   : 
  生成日期   : 2012-06-05 09:48:59
  最近修改   :
  功能描述   : 接口头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月5日
    作    者   : 
    修改内容   : 

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_SRAM_INTERFACE_H__
#define __SOC_SRAM_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif

#include "soc/soc_baseaddr_interface.h"

#define MEMORY_AXI_ACORE_DPM_ADDR           (SOC_SRAM_BASE_ADDR)
#define MEMORY_AXI_ACORE_DPM_SIZE           (0x00001000)

#define MEMORY_AXI_SOFT_FLAG_ADDR           (MEMORY_AXI_ACORE_DPM_ADDR + MEMORY_AXI_ACORE_DPM_SIZE)
#define MEMORY_AXI_SOFT_FLAG_SIZE           (0x00001000)

#define MEMORY_AXI_ICC_CHANNEL_ADDR         (MEMORY_AXI_SOFT_FLAG_ADDR + MEMORY_AXI_SOFT_FLAG_SIZE)
#define MEMORY_AXI_ICC_CHANNEL_SIZE         (0x00008000)

#define MEMORY_AXI_SECURE_ADDR              (MEMORY_AXI_ICC_CHANNEL_ADDR + MEMORY_AXI_ICC_CHANNEL_SIZE)
#define MEMORY_AXI_SECURE_SIZE              (0x00028000)

/* soft flag */
#define MEMORY_AXI_SRESET_FLAG_ADDR         (MEMORY_AXI_SOFT_FLAG_ADDR) 
#define MEMORY_AXI_SRESET_FLAG_SIZE         (4)

#define MEMORY_AXI_SECOND_CPU_START_ADDR    (MEMORY_AXI_SRESET_FLAG_ADDR + MEMORY_AXI_SRESET_FLAG_SIZE) 
#define MEMORY_AXI_SECOND_CPU_START_SIZE    (4)

#define MEMORY_AXI_CPU1_STACK_ADDR          (MEMORY_AXI_SECOND_CPU_START_ADDR + MEMORY_AXI_SECOND_CPU_START_SIZE) 
#define MEMORY_AXI_CPU1_STACK_SIZE          (4)

#define MEMORY_AXI_CPU2_STACK_ADDR          (MEMORY_AXI_CPU1_STACK_ADDR + MEMORY_AXI_CPU1_STACK_SIZE) 
#define MEMORY_AXI_CPU2_STACK_SIZE          (4)

#define MEMORY_AXI_CPU3_STACK_ADDR          (MEMORY_AXI_CPU2_STACK_ADDR + MEMORY_AXI_CPU2_STACK_SIZE) 
#define MEMORY_AXI_CPU3_STACK_SIZE          (4)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of soc_sram_interface.h */
