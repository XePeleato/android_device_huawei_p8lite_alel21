

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/hisi/pm/hi6xxx-pm-asm.h>
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include "soc_baseaddr_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
#define _ACPU_GET_CORE_ID(r0,r1)   \
        MOV r1, #0                  ;\
        MRC p15, 0, r0, c0, c0, 5   ;/*@ Read CPU MPIDR*/                        \
        LSR r1,r0,#0x8              ;/*cluster id*/                              \
        AND r1,r1,#0x3              ;                                            \
        AND r0, r0, #0x03           ;/*@ Mask off, leaving the CPU ID field*/    \

#define _ACPU_GET_GIC_PENDING(r0)   \
        MOV r2,#0x0                 ;\
        LDR r1,=ACPU_GIC_ICDISPR    ;\
Loop:                               ;\
        LDR r3,[r1]                 ;\
        STR r3,[r0]                 ;\
        ADD r1,r1,#0x4              ;\
        ADD r0,r0,#0x4              ;\
        ADD r2,r2,#0x4              ;\
        CMP r2,#0x10                ;\
        BNE Loop                    ;\

/*****************************************************************************
 函 数 名  : pm_asm_code_copy
 功能描述  : 内存copy
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月26日
    作    者   : 王振伟  176398
    修改内容   : 新生成函数

*****************************************************************************/
_ARM_FUNCTION_CALLED_FROM_C(pm_asm_code_copy)
        STMFD  sp!, {r0-r10, r14}

        ;/* Copy awake code to sram base addr */
        ADRL   r0, pm_asm_wake_entry
        LDR    r1, =PM_ACPU_ASM_MEM_VADDR
        LDR    r2, =pwrctrl_asm_code_end_flag
        ADD    r2,r2,#PM_ASM_CODE_COPY_OFFSET
CopyLoop:
        LDMIA  r0!, {r3-r10}
        STMIA  r1!, {r3-r10}
        CMP    r0,      r2
        BCC    CopyLoop

        LDMFD    sp!, {r0-r10, pc}

/*****************************************************************************
 函 数 名  : pm_asm_wake_entry
 功能描述  : 唤醒入口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2014年3月26日
    作    者   : 王振伟 176398
    修改内容   : 新生成函数
*****************************************************************************/
pm_asm_wake_entry:
#if 1
        LDR r0, =ACPU_UNLOCK_CORE_FLAGS_ADDR
        LDR r1, [r0]
        AND r1, r1, #1
        CMP r1, #1
        BNE over_unlock
        MOV r0, #0x0
        MCR p14, 0, r0, c1, c0, 4

over_unlock:
        LDR r0,=ACPU_A53_FLAGS_ADDR
        LDR r1,[r0]
        AND r1,r1,#0x1
        cmp r1,#0x1
        BNE normal_entry

        _ACPU_GET_CORE_ID(r0,r1)
        LDR r3,=SOC_ACPU_CCI_BASE_ADDR
        cmp r1,#0x0
        BNE cluster1_addr
        LDR r2,=0x94000
        ADD r4,r3,r2/*s3*/
        B set_value
cluster1_addr:
        LDR r2,=0x95000
        ADD r4,r3,r2/*s4*/
set_value:
        MOV r2,#0x3
        STR r2,[r4]
wait_loop:
        LDR r2,=0x9000c
        ADD r4,r3,r2
        LDR r5,[r4]
        CMP r5,#0x0
        BNE wait_loop

        /* clear cntvoff */
        mrs    r2,cpsr
        mrs    r3,cpsr
        and    r3,r3,#0xFFFFFFE0
        orr    r3,r3,#0x16
        msr    cpsr,r3
        ldr    r0,=0x1
        mcr    p15, 0, r0, c1, c1, 0
        ldr    r0,=0x0
        ldr    r1,=0x0
        mcrr   p15, 4, r0, r1, c14
        ldr    r0,=0x0
        mcr    p15, 0, r0, c1, c1, 0
        msr    cpsr,r2

        /* set A53 CPUACTLR register */
        mrrc   p15, 0, r0, r1, c15
        /* [28:27]=0b00  [26:25]=0b00 */
        bic    r0, r0, #0x1E000000
        /* [20:19]=0b11  [15:13]=0b111 */
        orr    r0, r0, #0x00180000
        orr    r0, r0, #0x0000E000
        mcrr   p15, 0, r0, r1, c15

        /*smp enable*/
        mrrc p15, 1, r0, r1, c15
        orr  r0,#0x40
        mcrr p15, 1, r0, r1, c15
normal_entry:
#endif
#if 0/*for tracing*/
        _ACPU_GET_CORE_ID(r0,r1)
        MOV r3,#0x8
        MUL r1,r1,r3
        ADD r0,r0,r1
        LDR r5,=SOC_HI6220_ACPU_SCTRL_BASE_ADDR

        MOV r3,#0x1
        LSL r0,r3,r0
        LDR r4,[r5,#0x2c]
        ORR r4,r4,r0
        STR r4,[r5,#0x2c]
#endif

        #if 0/*just for debug*/
        CMP     r0,#0x0
        BEQ     .

        LDR     r1,=PM_ASM_CORE_INFO_AREA_OFFSET
        MUL     r0, r1, r0
        LDR     r1,=PWRCTRL_ACPU_ASM_D_ARM_PARA_AD
        ADD     r0, r0, r1
        _ACPU_GET_GIC_PENDING(r0)
        LDR     r1,=ACPU_SLICE_ADDR
        LDR     r2,[r1]
        STR     r2,[r0]
        #endif
    	LDR     r1,=MEMORY_AXI_SEC_CORE_BOOT_ADDR
    	LDR		pc,[r1]
    .ltorg

/*asm end flg*/
pwrctrl_asm_code_end_flag:

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
