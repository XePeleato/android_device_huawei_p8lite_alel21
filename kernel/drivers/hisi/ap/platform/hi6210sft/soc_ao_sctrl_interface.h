

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_AO_SCTRL_INTERFACE_H__
#define __SOC_AO_SCTRL_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) reg_define
 ****************************************************************************/
/* 寄存器说明：系统控制寄存器0，用于定义系统模式配置。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SYS_CTRL0_UNION */
#define SOC_AO_SCTRL_SC_SYS_CTRL0_ADDR(base)          ((base) + (0x000))

/* 寄存器说明：系统控制寄存器1，功能包含MCU时钟门控、总线DFS硬件控制、MCU WDG0 RST选择、ACPU WDG RST选择、ACPU REMAP、eFUSEC REMAP、USIM_HPD、EXT_PLL配置。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SYS_CTRL1_UNION */
#define SOC_AO_SCTRL_SC_SYS_CTRL1_ADDR(base)          ((base) + (0x004))

/* 寄存器说明：系统控制寄存器2，WDG复位状态清除、全局复位状态清除。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SYS_CTRL2_UNION */
#define SOC_AO_SCTRL_SC_SYS_CTRL2_ADDR(base)          ((base) + (0x008))

/* 寄存器说明：系统状态寄存器0，用于监测系统的系统复位原因查询，
            往该寄存器写入0x48698284触发系统软复位，写入其它值无影响。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SYS_STAT0_UNION */
#define SOC_AO_SCTRL_SC_SYS_STAT0_ADDR(base)          ((base) + (0x010))

/* 寄存器说明：系统状态寄存器1，用于监测系统的状态，包括efuse状态NAND BOOT和NAND BITWIDTH、JTAG模式、BOOTMODE、BOOTSEL、系统模式状态、FUNCMODE等系统状态查询。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SYS_STAT1_UNION */
#define SOC_AO_SCTRL_SC_SYS_STAT1_ADDR(base)          ((base) + (0x014))

/* 寄存器说明：MCU中断模式控制寄存器，用于控制MCU中断发生时的系统模式。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_IMCTRL_UNION */
#define SOC_AO_SCTRL_SC_MCU_IMCTRL_ADDR(base)         ((base) + (0x018))

/* 寄存器说明：MCU中断模式状态寄存器，用于监测和控制MCU系统中断模式。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_IMSTAT_UNION */
#define SOC_AO_SCTRL_SC_MCU_IMSTAT_ADDR(base)         ((base) + (0x01C))

/* 寄存器说明：系统二级中断使能寄存器，用于对系统二级中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_UNION */
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_ADDR(base)   ((base) + (0x044))

/* 寄存器说明：系统二级中断中断寄存器，用于指示系统二级原始中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_UNION */
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_ADDR(base) ((base) + (0x048))

/* 寄存器说明：系统二级中断屏蔽后寄存器，用于指示系统二级中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_UNION */
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_ADDR(base) ((base) + (0x04C))

/* 寄存器说明：MCU唤醒中断使能寄存器，对应MCU的唤醒中断bit32～63，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_ADDR(base)   ((base) + (0x054))

/* 寄存器说明：MCU原始唤醒中断寄存器，对应MCU的唤醒中断bit32～63，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_ADDR(base) ((base) + (0x058))

/* 寄存器说明：MCU屏蔽后唤醒中断寄存器，对应MCU的唤醒中断bit32～63，用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_ADDR(base) ((base) + (0x05C))

/* 寄存器说明：MCU唤醒中断使能寄存器，对应MCU的唤醒中断bit31～0，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_ADDR(base)   ((base) + (0x064))

/* 寄存器说明：MCU原始唤醒中断寄存器，对应MCU的唤醒中断bit31～0，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_ADDR(base) ((base) + (0x068))

/* 寄存器说明：MCU屏蔽后唤醒中断寄存器，对应MCU的唤醒中断bit31～0，用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_ADDR(base) ((base) + (0x06C))

/* 寄存器说明：HiFi唤醒中断送给MCU的唤醒中断使能寄存器，对应HiFi的唤醒中断bit0～31，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_ADDR(base)   ((base) + (0x094))

/* 寄存器说明：HiFi唤醒中断送给MCU的原始唤醒中断寄存器，对应HiFi的唤醒中断bit0～31，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_ADDR(base) ((base) + (0x098))

/* 寄存器说明：HiFi唤醒中断送给MCU的屏蔽后唤醒中断寄存器，对应HiFi的唤醒中断bit0～31，用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_ADDR(base) ((base) + (0x09C))

/* 寄存器说明：ACPU唤醒中断送给MCU的唤醒中断使能寄存器，对应ACPU的唤醒中断bit0～31，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_ADDR(base)   ((base) + (0x0A8))

/* 寄存器说明：ACPU唤醒中断送给MCU的原始唤醒中断寄存器，对应ACPU的唤醒中断bit0～31，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_ADDR(base) ((base) + (0x0AC))

/* 寄存器说明：ACPU唤醒中断送给MCU的屏蔽后唤醒中断寄存器，对应ACPU的唤醒中断bit0～31，用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_ADDR(base) ((base) + (0x0B0))

/* 寄存器说明：ACPU唤醒中断送给MCU的唤醒中断使能寄存器，对应ACPU的唤醒中断bit32～63，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_ADDR(base)   ((base) + (0x0B4))

/* 寄存器说明：ACPU唤醒中断送给MCU的原始唤醒中断寄存器，对应ACPU的唤醒中断bit32～63，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_ADDR(base) ((base) + (0x0B8))

/* 寄存器说明：ACPU唤醒中断送给MCU的屏蔽后唤醒中断寄存器，对应ACPU的唤醒中断bit32～63。用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_UNION */
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_ADDR(base) ((base) + (0x0BC))

/* 寄存器说明：原始内部中断寄存器，用于指示SC内部的原始中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_STATR_UNION */
#define SOC_AO_SCTRL_SC_INT_STATR_ADDR(base)          ((base) + (0x0C4))

/* 寄存器说明：屏蔽后内部中断寄存器，用于指示SC当前内部中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_STATM_UNION */
#define SOC_AO_SCTRL_SC_INT_STATM_ADDR(base)          ((base) + (0x0C8))

/* 寄存器说明：内部中断清除寄存器，给相应中断比特位写1，表示清除对应的中断状态，包括原始内部中断寄存器和屏蔽后内部中断寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_CLEAR_UNION */
#define SOC_AO_SCTRL_SC_INT_CLEAR_ADDR(base)          ((base) + (0x0CC))

/* 寄存器说明：内部中断使能寄存器0，对比特写1，SC_INT_EN_STAT的对应bit置1；写0不影响屏蔽的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_EN_SET_UNION */
#define SOC_AO_SCTRL_SC_INT_EN_SET_ADDR(base)         ((base) + (0x0D0))

/* 寄存器说明：内部中断屏蔽寄存器0，对比特写1，SC_INT_EN_STAT的对应bit清0；写0不影响时钟屏蔽的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_EN_DIS_UNION */
#define SOC_AO_SCTRL_SC_INT_EN_DIS_ADDR(base)         ((base) + (0x0D4))

/* 寄存器说明：内部中断使能状态寄存器，通过配置SC_INT_EN_SET0和SC_INT_EN_DIS0修改状态，用于SC内部中断的使能和屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_EN_STAT_UNION */
#define SOC_AO_SCTRL_SC_INT_EN_STAT_ADDR(base)        ((base) + (0x0D8))

/* 寄存器说明：原始内部中断寄存器，用于指示SC内部的原始中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_STATR1_UNION */
#define SOC_AO_SCTRL_SC_INT_STATR1_ADDR(base)         ((base) + (0x0E4))

/* 寄存器说明：屏蔽后内部中断寄存器，用于指示SC当前内部中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_STATM1_UNION */
#define SOC_AO_SCTRL_SC_INT_STATM1_ADDR(base)         ((base) + (0x0E8))

/* 寄存器说明：内部中断清除寄存器，给相应中断比特位写1，表示清除对应的中断状态，包括原始内部中断寄存器和屏蔽后内部中断寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_CLEAR1_UNION */
#define SOC_AO_SCTRL_SC_INT_CLEAR1_ADDR(base)         ((base) + (0x0EC))

/* 寄存器说明：内部中断使能寄存器0，对比特写1，SC_INT_EN_STAT1的对应bit置1；写0不影响屏蔽的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_EN_SET1_UNION */
#define SOC_AO_SCTRL_SC_INT_EN_SET1_ADDR(base)        ((base) + (0x0F0))

/* 寄存器说明：内部中断屏蔽寄存器0，对比特写1，SC_INT_EN_STAT1的对应bit清0；写0不影响时钟屏蔽的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_EN_DIS1_UNION */
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_ADDR(base)        ((base) + (0x0F4))

/* 寄存器说明：内部中断使能状态寄存器，通过配置SC_INT_EN_SET1和SC_INT_EN_DIS1修改状态，用于SC内部中断的使能和屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_INT_EN_STAT1_UNION */
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_ADDR(base)       ((base) + (0x0F8))

/* 寄存器说明：timer使能寄存器0。
            寄存器写入时需要msk有效，[31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
   位域定义UNION结构:  SOC_AO_SCTRL_SC_TIMER_EN0_UNION */
#define SOC_AO_SCTRL_SC_TIMER_EN0_ADDR(base)          ((base) + (0x1D0))

/* 寄存器说明：timer使能寄存器1。
            寄存器写入时需要msk有效，[31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
   位域定义UNION结构:  SOC_AO_SCTRL_SC_TIMER_EN1_UNION */
#define SOC_AO_SCTRL_SC_TIMER_EN1_ADDR(base)          ((base) + (0x1D4))

/* 寄存器说明：timer使能寄存器4。
            寄存器写入时需要msk有效，[31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
   位域定义UNION结构:  SOC_AO_SCTRL_SC_TIMER_EN4_UNION */
#define SOC_AO_SCTRL_SC_TIMER_EN4_ADDR(base)          ((base) + (0x1F0))

/* 寄存器说明：timer使能寄存器5。
            寄存器写入时需要msk有效，[31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
   位域定义UNION结构:  SOC_AO_SCTRL_SC_TIMER_EN5_UNION */
#define SOC_AO_SCTRL_SC_TIMER_EN5_ADDR(base)          ((base) + (0x1F4))

/* 寄存器说明：MCU子系统控制寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL0_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL0_ADDR(base)   ((base) + (0x400))

/* 寄存器说明：MCU子系统控制寄存器，功能包含MCU的DAP enable,Tick时钟校准,MCU NMI中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_ADDR(base)   ((base) + (0x404))

/* 寄存器说明：MCU子系统控制寄存器,功能包含MCU的DDR REMAP起始地址配置。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL2_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL2_ADDR(base)   ((base) + (0x408))

/* 寄存器说明：MCU子系统控制寄存器，功能包含PMUSSI控制、cssys_ctrl_prot控制。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_ADDR(base)   ((base) + (0x40C))

/* 寄存器说明：MCU子系统控制寄存器，功能包含COREON Memory控制。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_ADDR(base)   ((base) + (0x410))

/* 寄存器说明：MCU子系统控制寄存器，功能包含MCU SHARE MEM地址配置。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_ADDR(base)   ((base) + (0x414))

/* 寄存器说明：MCU子系统控制寄存器，功能包括CM3保护地址空间起始地址。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_ADDR(base)   ((base) + (0x418))

/* 寄存器说明：MCU子系统控制寄存器，功能包括CM3保护地址空间结束地址。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL7_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL7_ADDR(base)   ((base) + (0x41C))

/* 寄存器说明：MCU子系统状态寄存器，功能包含MCU状态信号。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_ADDR(base)   ((base) + (0x440))

/* 寄存器说明：MCU子系统状态寄存器，功能包含保留。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT1_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT1_ADDR(base)   ((base) + (0x444))

/* 寄存器说明：MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT2_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT2_ADDR(base)   ((base) + (0x448))

/* 寄存器说明：MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT3_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT3_ADDR(base)   ((base) + (0x44C))

/* 寄存器说明：MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_ADDR(base)   ((base) + (0x450))

/* 寄存器说明：MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT5_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT5_ADDR(base)   ((base) + (0x454))

/* 寄存器说明：MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT6_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT6_ADDR(base)   ((base) + (0x458))

/* 寄存器说明：MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_UNION */
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_ADDR(base)   ((base) + (0x45C))

/* 寄存器说明：ALWAYSON分区时钟使能寄存器4。对比特写1，使能SC_PERIPH_CLKSTAT4对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_CLKEN4_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(base)      ((base) + (0x630))

/* 寄存器说明：ALWAYSON分区时钟禁止寄存器4。对比特写1，禁止SC_PERIPH_CLKSTAT4对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(base)     ((base) + (0x634))

/* 寄存器说明：ALWAYSON分区时钟状态寄存器4。由SC_PERIPH_CLKEN4和SC_PERIPH_CLKDIS4配置进行状态修改。
            外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
            每个Bit含义：
            0：时钟关闭；
            1：时钟打开。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_ADDR(base)    ((base) + (0x638))

/* 寄存器说明：ALWAYSON分区时钟使能寄存器5。对比特写1，使能SC_PERIPH_CLKSTAT5对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_CLKEN5_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_ADDR(base)      ((base) + (0x63C))

/* 寄存器说明：ALWAYSON分区时钟禁止寄存器5。对比特写1，禁止SC_PERIPH_CLKSTAT5对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_ADDR(base)     ((base) + (0x640))

/* 寄存器说明：ALWAYSON分区时钟状态寄存器5。由SC_PERIPH_CLKEN5和SC_PERIPH_CLKDIS5配置进行状态修改。
            外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
            每个Bit含义：
            0：时钟关闭；
            1：时钟打开。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_ADDR(base)    ((base) + (0x644))

/* 寄存器说明：ALWAYSON分区复位使能寄存器4。用于使能SC_PERIPH_RSTSTAT4复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_RSTEN4_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_ADDR(base)      ((base) + (0x6F0))

/* 寄存器说明：ALWAYSON分区设复位禁止寄存器4。用于禁止SC_PERIPH_RSTSTAT4复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_ADDR(base)     ((base) + (0x6F4))

/* 寄存器说明：ALWAYSON分区设复位状态寄存器4。由SC_PERIPH_RSTEN4和SC_PERIPH_RSTDIS4配置进行状态修改。解复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
            每个Bit含义：
            0：复位不使能；
            1：复位使能。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_ADDR(base)    ((base) + (0x6F8))

/* 寄存器说明：ALWAYSON分区复位使能寄存器5。用于使能SC_PERIPH_RSTSTAT5复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_RSTEN5_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN5_ADDR(base)      ((base) + (0x6FC))

/* 寄存器说明：ALWAYSON分区设复位禁止寄存器5。用于禁止SC_PERIPH_RSTSTAT5复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_RSTDIS5_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS5_ADDR(base)     ((base) + (0x700))

/* 寄存器说明：ALWAYSON分区设复位状态寄存器5。由SC_PERIPH_RSTEN5和SC_PERIPH_RSTDIS5配置进行状态修改。解复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
            每个Bit含义：
            0：复位不使能；
            1：复位使能。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERIPH_RSTSTAT5_UNION */
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT5_ADDR(base)    ((base) + (0x704))

/* 寄存器说明：掉电区时钟使能寄存器。用于控制各掉电区总的时钟使能。对相应位写1使能SC_PW_CLK_STAT0相应电压域时钟，写0无影响。该寄存器只在上电复位时被复位。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_CLKEN0_UNION */
#define SOC_AO_SCTRL_SC_PW_CLKEN0_ADDR(base)          ((base) + (0x800))

/* 寄存器说明：掉电区时钟禁止寄存器。用于控制各掉电区总的时钟禁止。对相应位写1禁止SC_PW_CLK_STAT0相应掉电区时钟，写0无影响。该寄存器只在上电复位时被复位。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_CLKDIS0_UNION */
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_ADDR(base)         ((base) + (0x804))

/* 寄存器说明：下电区时钟状态，通过SC_PW_CLKEN0和SC_PW_CLKDIS0修改状态。
            每个Bit含义：
            0：关闭；
            1：打开。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_CLK_STAT0_UNION */
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_ADDR(base)       ((base) + (0x808))

/* 寄存器说明：掉电区复位使能寄存器。用于控制各掉电区总的复位使能。对相应位写1使能SC_PW_RST_STAT0相应掉电区复位信号（即进入复位态），写0无影响。该寄存器只在上电复位时被复位。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_RSTEN0_UNION */
#define SOC_AO_SCTRL_SC_PW_RSTEN0_ADDR(base)          ((base) + (0x810))

/* 寄存器说明：掉电区复位禁止寄存器。用于控制各掉电区总的复位禁止。对相应位写1禁止SC_PW_RST_STAT0相应掉电区复位信号（即退出复位态），写0无影响。该寄存器只在上电复位时被复位。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_RSTDIS0_UNION */
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_ADDR(base)         ((base) + (0x814))

/* 寄存器说明：下电区复位状态，通过SC_PW_RSTEN0和SC_PW_RSTDIS0修改状态。
            每个Bit含义：
            0：复位不使能；
            1：复位使能。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_RST_STAT0_UNION */
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_ADDR(base)       ((base) + (0x818))

/* 寄存器说明：掉电区ISO使能寄存器。用于控制各掉电区相关的Isolation Cell使能。对相应位写1使能SC_PW_ISO_STAT0相应掉电区IsoLation Cell（即信号隔断），写0无影响。该寄存器只在上电复位时被复位。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_ISOEN0_UNION */
#define SOC_AO_SCTRL_SC_PW_ISOEN0_ADDR(base)          ((base) + (0x820))

/* 寄存器说明：掉电区ISO禁止寄存器。用于控制各掉电区相关的Isolation Cell禁止。对相应位写1禁止SC_PW_ISO_STAT0相应掉电区IsoLation Cell（即信号不隔断），写0无影响。该寄存器只在上电复位时被复位。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_ISODIS0_UNION */
#define SOC_AO_SCTRL_SC_PW_ISODIS0_ADDR(base)         ((base) + (0x824))

/* 寄存器说明：下电区ISO状态，通过SC_PW_ISOEN0和SC_PW_ISODIS0修改状态。
            每个Bit含义：
            0：isolation不使能；
            1：isolation使能。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_ISO_STAT0_UNION */
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_ADDR(base)       ((base) + (0x828))

/* 寄存器说明：掉电区电源使能寄存器。用于控制各掉电区的电源使能，对相应位写1打开SC_PW_MTCMOS_STAT0相应电压域电源，写0无影响。该寄存器存只在上电复位时被复位。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_UNION */
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_ADDR(base)      ((base) + (0x830))

/* 寄存器说明：掉电区电源禁止寄存器。用于控制各掉电区的电源禁止，对相应位写1关闭SC_PW_MTCMOS_STAT0相应电压域电源，写0无影响。该寄存器存只在上电复位时被复位。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_UNION */
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_ADDR(base)     ((base) + (0x834))

/* 寄存器说明：下电区MTCMOS配置状态，通过SC_PW_EN0和SC_PW_DIS0修改MTCMOS控制状态。
            每个Bit含义：
            0：电源关闭；
            1：电源打开。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_UNION */
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_ADDR(base)    ((base) + (0x838))

/* 寄存器说明：下电区MTCMOS状态，通过SC_PW_EN0和SC_PW_DIS0修改MTCMOS控制状态，再经过掉电分区反馈回来。
            每个Bit含义：
            0：电源关闭；
            1：电源打开。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_UNION */
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_ADDR(base) ((base) + (0x83C))

/* 寄存器说明：系统POWER状态寄存器1。用于监测系统所处的POWER状态。
            0：电源稳定计数器未溢出；
            1：电源稳定计数器，MTCMOS或外部电源稳定已状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_UNION */
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_ADDR(base) ((base) + (0x840))

/* 寄存器说明：系统POWER状态0寄存器，用于监测系统所处的POWER状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_STAT0_UNION */
#define SOC_AO_SCTRL_SC_PW_STAT0_ADDR(base)           ((base) + (0x850))

/* 寄存器说明：系统POWER状态1寄存器，POWER DOMAIN IDLEACK状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_STAT1_UNION */
#define SOC_AO_SCTRL_SC_PW_STAT1_ADDR(base)           ((base) + (0x854))

/* 寄存器说明：系统状态寄存器1，用于监测系统的状态。狗复位请求、JTAG模式、nand_boot_en、efuse_hw_rd_finish、usim_hpd_sta等系统状态查询。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SYSTEST_STAT_UNION */
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_ADDR(base)       ((base) + (0x880))

/* 寄存器说明：64bit递增计数器的低32bit，SLICERl_CNT[31:0]，32KHz参考时钟。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_UNION */
#define SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_ADDR(base) ((base) + (0x890))

/* 寄存器说明：64bit递增计数器的高32bit，SLICERl_CNT[63:32]，32KHz参考时钟。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_UNION */
#define SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_ADDR(base) ((base) + (0x894))

/* 寄存器说明：低功耗控制寄存器1，功能包括分区掉电后的总线DOMAIN IDLE请求。
            [31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_CTRL1_UNION */
#define SOC_AO_SCTRL_SC_PW_CTRL1_ADDR(base)           ((base) + (0x8C8))

/* 寄存器说明：低功耗控制寄存器0，功能包括外设IO retention，ddr io retention，总线门控，ipc_ns和ipc_s自动门控，外设区掉电控制。
            [31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PW_CTRL_UNION */
#define SOC_AO_SCTRL_SC_PW_CTRL_ADDR(base)            ((base) + (0x8CC))

/* 寄存器说明：MCPU睡眠唤醒投票使能寄存器0。对比特写1，产生投票状态；写0不影响投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCPU_VOTEEN_UNION */
#define SOC_AO_SCTRL_SC_MCPU_VOTEEN_ADDR(base)        ((base) + (0x8D0))

/* 寄存器说明：MCPU睡眠唤醒投票禁止寄存器0。对比特写1，清除投票状态；写0不影响投票使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCPU_VOTEDIS_UNION */
#define SOC_AO_SCTRL_SC_MCPU_VOTEDIS_ADDR(base)       ((base) + (0x8D4))

/* 寄存器说明：MCPU睡眠唤醒投票状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCPU_VOTESTAT_UNION */
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT_ADDR(base)      ((base) + (0x8D8))

/* 寄存器说明：MCPU睡眠唤醒投票屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCPU_VOTE_MSK0_UNION */
#define SOC_AO_SCTRL_SC_MCPU_VOTE_MSK0_ADDR(base)     ((base) + (0x8E0))

/* 寄存器说明：MCPU睡眠唤醒投票屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCPU_VOTE_MSK1_UNION */
#define SOC_AO_SCTRL_SC_MCPU_VOTE_MSK1_ADDR(base)     ((base) + (0x8E4))

/* 寄存器说明：MCPU睡眠投票屏蔽后状态寄存器0。用于回读各投票的状态。1为投票，0为非投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_UNION */
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_ADDR(base) ((base) + (0x8E8))

/* 寄存器说明：MCPU睡眠唤醒投票屏蔽后状态寄存器1。用于回读各投票的状态。1为投票，0为非投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_UNION */
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_ADDR(base) ((base) + (0x8EC))

/* 寄存器说明：PERI睡眠唤醒投票使能寄存器0。对比特写1，产生投票状态；写0不影响投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERI_VOTEEN_UNION */
#define SOC_AO_SCTRL_SC_PERI_VOTEEN_ADDR(base)        ((base) + (0x8F0))

/* 寄存器说明：PERI睡眠唤醒投票禁止寄存器0。对比特写1，清除投票状态；写0不影响投票使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERI_VOTEDIS_UNION */
#define SOC_AO_SCTRL_SC_PERI_VOTEDIS_ADDR(base)       ((base) + (0x8F4))

/* 寄存器说明：PERI睡眠唤醒投票状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERI_VOTESTAT_UNION */
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT_ADDR(base)      ((base) + (0x8F8))

/* 寄存器说明：PERI睡眠唤醒投票屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERI_VOTE_MSK0_UNION */
#define SOC_AO_SCTRL_SC_PERI_VOTE_MSK0_ADDR(base)     ((base) + (0x900))

/* 寄存器说明：PERI睡眠唤醒投票屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERI_VOTE_MSK1_UNION */
#define SOC_AO_SCTRL_SC_PERI_VOTE_MSK1_ADDR(base)     ((base) + (0x904))

/* 寄存器说明：PERI睡眠投票屏蔽后状态寄存器0。用于回读各投票的状态。1为投票，0为非投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_UNION */
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_ADDR(base) ((base) + (0x908))

/* 寄存器说明：PERI睡眠唤醒投票屏蔽后状态寄存器1。用于回读各投票的状态。1为投票，0为非投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_UNION */
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_ADDR(base) ((base) + (0x90C))

/* 寄存器说明：ACPU睡眠唤醒投票使能寄存器0。对比特写1，产生投票状态；写0不影响投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ACPU_VOTEEN_UNION */
#define SOC_AO_SCTRL_SC_ACPU_VOTEEN_ADDR(base)        ((base) + (0x910))

/* 寄存器说明：ACPU睡眠唤醒投票禁止寄存器0。对比特写1，清除投票状态；写0不影响投票使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ACPU_VOTEDIS_UNION */
#define SOC_AO_SCTRL_SC_ACPU_VOTEDIS_ADDR(base)       ((base) + (0x914))

/* 寄存器说明：ACPU睡眠唤醒投票状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ACPU_VOTESTAT_UNION */
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT_ADDR(base)      ((base) + (0x918))

/* 寄存器说明：ACPU睡眠唤醒投票屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ACPU_VOTE_MSK0_UNION */
#define SOC_AO_SCTRL_SC_ACPU_VOTE_MSK0_ADDR(base)     ((base) + (0x920))

/* 寄存器说明：ACPU睡眠唤醒投票屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ACPU_VOTE_MSK1_UNION */
#define SOC_AO_SCTRL_SC_ACPU_VOTE_MSK1_ADDR(base)     ((base) + (0x924))

/* 寄存器说明：ACPU睡眠投票屏蔽后状态寄存器0。用于回读各投票的状态。1为投票，0为非投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_UNION */
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_ADDR(base) ((base) + (0x928))

/* 寄存器说明：ACPU睡眠唤醒投票屏蔽后状态寄存器1。用于回读各投票的状态。1为投票，0为非投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_UNION */
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_ADDR(base) ((base) + (0x92C))

/* 寄存器说明：MCU投票产生寄存器0。对比特写1，对应产生投票状态；写0不影响投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTEEN_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTEEN_ADDR(base)         ((base) + (0x930))

/* 寄存器说明：MCU投票禁止寄存器0。对比特写1，清除投票状态；写0不影响投票使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTEDIS_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTEDIS_ADDR(base)        ((base) + (0x934))

/* 寄存器说明：MCU投票状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTESTAT_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTESTAT_ADDR(base)       ((base) + (0x938))

/* 寄存器说明：MCU投票寄存器0的屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE_MSK0_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE_MSK0_ADDR(base)      ((base) + (0x940))

/* 寄存器说明：MCU投票寄存器0的屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE_MSK1_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE_MSK1_ADDR(base)      ((base) + (0x944))

/* 寄存器说明：MCU投票寄存器0屏蔽后状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTESTAT0_MSK_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTESTAT0_MSK_ADDR(base)  ((base) + (0x948))

/* 寄存器说明：MCU投票寄存器0屏蔽后状态寄存器1。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTESTAT1_MSK_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTESTAT1_MSK_ADDR(base)  ((base) + (0x94C))

/* 寄存器说明：MCU投票产生寄存器1。对比特写1，对应产生投票状态；写0不影响投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE1EN_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE1EN_ADDR(base)        ((base) + (0x960))

/* 寄存器说明：MCU投票禁止寄存器1。对比特写1，清除投票状态；写0不影响投票使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE1DIS_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE1DIS_ADDR(base)       ((base) + (0x964))

/* 寄存器说明：MCU投票状态寄存器1。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE1STAT_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE1STAT_ADDR(base)      ((base) + (0x968))

/* 寄存器说明：MCU投票寄存器1的屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE1_MSK0_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE1_MSK0_ADDR(base)     ((base) + (0x970))

/* 寄存器说明：MCU投票寄存器1的屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE1_MSK1_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE1_MSK1_ADDR(base)     ((base) + (0x974))

/* 寄存器说明：MCU投票寄存器1屏蔽后状态寄存器0，可用于SYSNOC 150MHz总线DFS控制。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE1STAT0_MSK_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE1STAT0_MSK_ADDR(base) ((base) + (0x978))

/* 寄存器说明：MCU投票寄存器1屏蔽后状态寄存器1。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE1STAT1_MSK_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE1STAT1_MSK_ADDR(base) ((base) + (0x97C))

/* 寄存器说明：MCU投票产生寄存器2。对比特写1，对应产生投票状态；写0不影响投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE2EN_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE2EN_ADDR(base)        ((base) + (0x980))

/* 寄存器说明：MCU投票禁止寄存器2。对比特写1，清除投票状态；写0不影响投票使能的状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE2DIS_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE2DIS_ADDR(base)       ((base) + (0x984))

/* 寄存器说明：MCU投票状态寄存器2。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE2STAT_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE2STAT_ADDR(base)      ((base) + (0x988))

/* 寄存器说明：MCU投票寄存器2的屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE2_MSK0_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE2_MSK0_ADDR(base)     ((base) + (0x990))

/* 寄存器说明：MCU投票寄存器2的屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE2_MSK1_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE2_MSK1_ADDR(base)     ((base) + (0x994))

/* 寄存器说明：MCU投票寄存器2屏蔽后状态寄存器0，可用于SYSNOC/CFGBUS/PERI_DMA BUS三个100MHz总线DFS控制。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE2STAT0_MSK_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE2STAT0_MSK_ADDR(base) ((base) + (0x998))

/* 寄存器说明：MCU投票寄存器2屏蔽后状态寄存器1。用于回读各投票的状态。1为有投票状态，0为无投票状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_MCU_VOTE2STAT1_MSK_UNION */
#define SOC_AO_SCTRL_SC_MCU_VOTE2STAT1_MSK_ADDR(base) ((base) + (0x99C))

/* 寄存器说明：投票控制寄存器，功能包含投票旗语互斥。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_VOTE_CTRL_UNION */
#define SOC_AO_SCTRL_SC_VOTE_CTRL_ADDR(base)          ((base) + (0x9A0))

/* 寄存器说明：投票控制寄存器，功能包含投票旗语互斥状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_VOTE_STAT_UNION */
#define SOC_AO_SCTRL_SC_VOTE_STAT_ADDR(base)          ((base) + (0x9A4))

/* 寄存器说明：ECO 版本号寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ECONUM_UNION */
#define SOC_AO_SCTRL_SC_ECONUM_ADDR(base)             ((base) + (0xF00))

/* 寄存器说明：CHIPID寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SCCHIPID_UNION */
#define SOC_AO_SCTRL_SCCHIPID_ADDR(base)              ((base) + (0xF10))

/* 寄存器说明：SOCID寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SCSOCID_UNION */
#define SOC_AO_SCTRL_SCSOCID_ADDR(base)               ((base) + (0xF1C))

/* 寄存器说明：外设ID寄存器0。FPGA版本使用，由宏定义确定数值
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SOC_FPGA_RTL_DEF_UNION */
#define SOC_AO_SCTRL_SC_SOC_FPGA_RTL_DEF_ADDR(base)   ((base) + (0xFE0))

/* 寄存器说明：外设ID寄存器1。FPGA版本使用，由宏定义确定数值
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SOC_FPGA_PR_DEF_UNION */
#define SOC_AO_SCTRL_SC_SOC_FPGA_PR_DEF_ADDR(base)    ((base) + (0xFE4))

/* 寄存器说明：外设ID寄存器2。FPGA版本使用，由宏定义确定数值
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF0_UNION */
#define SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF0_ADDR(base)  ((base) + (0xFE8))

/* 寄存器说明：外设ID寄存器3。FPGA版本使用，由宏定义确定数值
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF1_UNION */
#define SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF1_ADDR(base)  ((base) + (0xFEC))

/* 寄存器说明：晶振控制寄存器。用于对tcxo0晶振进行控制。系统在SLOW和NORMAL模式下，tcxo必须被使能。tcxo ABB稳定延迟配置。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_XTAL_CTRL0_UNION */
#define SOC_AO_SCTRL_SC_XTAL_CTRL0_ADDR(base)         ((base) + (0x1028))

/* 寄存器说明：晶振控制寄存器。用于对tcxo1晶振进行控制。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_XTAL_CTRL1_UNION */
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_ADDR(base)         ((base) + (0x102c))

/* 寄存器说明：晶振控制寄存器。tcxo0 PMU侧使能，仅在调试模式或系统进入SLEEP需要打开tcxo的场景使用。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_XTAL_CTRL3_UNION */
#define SOC_AO_SCTRL_SC_XTAL_CTRL3_ADDR(base)         ((base) + (0x1034))

/* 寄存器说明：晶振控制寄存器。tcxo0下电流程，tcxo0电源关闭到tcxo0 ABB侧使能关闭延迟时间配置。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_XTAL_CTRL5_UNION */
#define SOC_AO_SCTRL_SC_XTAL_CTRL5_ADDR(base)         ((base) + (0x103C))

/* 寄存器说明：晶振tcxo0状态寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_XTAL_STAT0_UNION */
#define SOC_AO_SCTRL_SC_XTAL_STAT0_ADDR(base)         ((base) + (0x1060))

/* 寄存器说明：晶振tcxo1状态寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_XTAL_STAT1_UNION */
#define SOC_AO_SCTRL_SC_XTAL_STAT1_ADDR(base)         ((base) + (0x1070))

/* 寄存器说明：EFUSE_CHIPID状态寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_EFUSE_CHIPID0_UNION */
#define SOC_AO_SCTRL_SC_EFUSE_CHIPID0_ADDR(base)      ((base) + (0x1080))

/* 寄存器说明：EFUSE_CHIPID状态寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_EFUSE_CHIPID1_UNION */
#define SOC_AO_SCTRL_SC_EFUSE_CHIPID1_ADDR(base)      ((base) + (0x1084))

/* 寄存器说明：EFUSE_SYS_CTRL状态寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_EFUSE_SYS_CTRL_UNION */
#define SOC_AO_SCTRL_SC_EFUSE_SYS_CTRL_ADDR(base)     ((base) + (0x1088))

/* 寄存器说明：安全DEBUG控制寄存器1。用于控制ACPU、CCPU、MCU、CSSYS、HIFI、BBE调试、DJTAG功能，保证SC_DEBUG_CTRL1寄存器的值不被DFT控制（这个通过安全DFT实现），此寄存器只能写入1次。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_DEBUG_CTRL1_UNION */
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_ADDR(base)        ((base) + (0x1288))

/* 寄存器说明：安全DEBUG状态寄存器。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_DBG_STAT_UNION */
#define SOC_AO_SCTRL_SC_DBG_STAT_ADDR(base)           ((base) + (0x12B4))

/* 寄存器说明：安全DEBUG密码寄存器，此寄存器可写入3次，此寄存器不可读。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ARM_DBG_KEY0_UNION */
#define SOC_AO_SCTRL_SC_ARM_DBG_KEY0_ADDR(base)       ((base) + (0x12B8))

/* 寄存器说明：保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_RESERVED31_UNION */
#define SOC_AO_SCTRL_SC_RESERVED31_ADDR(base)         ((base) + (0x13A0))

/* 寄存器说明：保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_RESERVED32_UNION */
#define SOC_AO_SCTRL_SC_RESERVED32_ADDR(base)         ((base) + (0x13A4))

/* 寄存器说明：保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_RESERVED33_UNION */
#define SOC_AO_SCTRL_SC_RESERVED33_ADDR(base)         ((base) + (0x13A8))

/* 寄存器说明：保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_RESERVED34_UNION */
#define SOC_AO_SCTRL_SC_RESERVED34_ADDR(base)         ((base) + (0x13AC))

/* 寄存器说明：保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_RESERVED35_UNION */
#define SOC_AO_SCTRL_SC_RESERVED35_ADDR(base)         ((base) + (0x13B0))

/* 寄存器说明：保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_RESERVED36_UNION */
#define SOC_AO_SCTRL_SC_RESERVED36_ADDR(base)         ((base) + (0x13B4))

/* 寄存器说明：保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_RESERVED37_UNION */
#define SOC_AO_SCTRL_SC_RESERVED37_ADDR(base)         ((base) + (0x13B8))

/* 寄存器说明：保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_RESERVED38_UNION */
#define SOC_AO_SCTRL_SC_RESERVED38_ADDR(base)         ((base) + (0x13BC))

/* 寄存器说明：ALWAYSON控制寄存器，功能包含CLKSYS_ON分频比配置。
            [31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_ADDR(base) ((base) + (0x1480))

/* 寄存器说明：ALWAYSON控制寄存器，功能包含TCXO占空比配置，DCXO分频控制。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_ADDR(base) ((base) + (0x1484))

/* 寄存器说明：ALWAYSON控制寄存器，功能包含PDM分频配置。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL2_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL2_ADDR(base) ((base) + (0x1488))

/* 寄存器说明：ALWAYSON控制寄存器，功能32K时钟选择和RTC分频配置。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL3_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL3_ADDR(base) ((base) + (0x148C))

/* 寄存器说明：ALWAYSON控制寄存器，功能包含CLK MONITOR控制。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_ADDR(base) ((base) + (0x14A8))

/* 寄存器说明：ALWAYSON控制寄存器，功能包含CLK MONITOR控制,IO RET。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_ADDR(base) ((base) + (0x14AC))

/* 寄存器说明：ALWAYSON状态寄存器，功能包含CLK MONITOR控制。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_ADDR(base) ((base) + (0x14C0))

/* 寄存器说明：ALWAYSON状态寄存器，功能BBP DRX深度。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_ADDR(base) ((base) + (0x14C4))

/* 寄存器说明：ALWAYSON状态寄存器，功能包含usb chgdet_sts和datcondet_sts状态。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT2_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT2_ADDR(base) ((base) + (0x14C8))

/* 寄存器说明：ALWAYSON状态寄存器，功能保留。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_UNION */
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_ADDR(base) ((base) + (0x14CC))

/* 寄存器说明：掉电区电源稳定计数器0寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PWUP_TIME0_UNION */
#define SOC_AO_SCTRL_SC_PWUP_TIME0_ADDR(base)         ((base) + (0x1880))

/* 寄存器说明：掉电区电源稳定计数器1寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PWUP_TIME1_UNION */
#define SOC_AO_SCTRL_SC_PWUP_TIME1_ADDR(base)         ((base) + (0x1884))

/* 寄存器说明：掉电区电源稳定计数器2寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PWUP_TIME2_UNION */
#define SOC_AO_SCTRL_SC_PWUP_TIME2_ADDR(base)         ((base) + (0x1888))

/* 寄存器说明：掉电区电源稳定计数器3寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PWUP_TIME3_UNION */
#define SOC_AO_SCTRL_SC_PWUP_TIME3_ADDR(base)         ((base) + (0x188C))

/* 寄存器说明：掉电区电源稳定计数器4寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PWUP_TIME4_UNION */
#define SOC_AO_SCTRL_SC_PWUP_TIME4_ADDR(base)         ((base) + (0x1890))

/* 寄存器说明：掉电区电源稳定计数器5寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PWUP_TIME5_UNION */
#define SOC_AO_SCTRL_SC_PWUP_TIME5_ADDR(base)         ((base) + (0x1894))

/* 寄存器说明：掉电区电源稳定计数器6寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PWUP_TIME6_UNION */
#define SOC_AO_SCTRL_SC_PWUP_TIME6_ADDR(base)         ((base) + (0x1898))

/* 寄存器说明：掉电区电源稳定计数器7寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_PWUP_TIME7_UNION */
#define SOC_AO_SCTRL_SC_PWUP_TIME7_ADDR(base)         ((base) + (0x189C))

/* 寄存器说明：安全控制寄存器，控制AO_SC的寄存器BIT安全访问。
   位域定义UNION结构:  SOC_AO_SCTRL_SC_SECURITY_CTRL1_UNION */
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_ADDR(base)     ((base) + (0x1C04))





/*****************************************************************************
  3 枚举定义
*****************************************************************************/



/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/



/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/



/*****************************************************************************
  7 UNION定义
*****************************************************************************/

/****************************************************************************
                     (1/1) reg_define
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SYS_CTRL0_UNION
 结构说明  : SC_SYS_CTRL0 寄存器结构定义。地址偏移量:0x000，初值:0x00000002，宽度:32
 寄存器说明: 系统控制寄存器0，用于定义系统模式配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  modectrl0_sftcfg : 3;  /* bit[0-2] : 定义系统期望的工作模式。系统从SLEEP唤醒时，如果使用中断响应模式，则此寄存器受硬件控制，中断唤醒后会自动切换到SLOW；
                                                           000：SLEEP；
                                                           001：DOZE；
                                                           010：SLOW；
                                                           100：NORMAL。 */
        unsigned int  reserved         : 29; /* bit[3-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SYS_CTRL0_UNION;
#endif
#define SOC_AO_SCTRL_SC_SYS_CTRL0_modectrl0_sftcfg_START  (0)
#define SOC_AO_SCTRL_SC_SYS_CTRL0_modectrl0_sftcfg_END    (2)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SYS_CTRL1_UNION
 结构说明  : SC_SYS_CTRL1 寄存器结构定义。地址偏移量:0x004，初值:0x00000000，宽度:32
 寄存器说明: 系统控制寄存器1，功能包含MCU时钟门控、总线DFS硬件控制、MCU WDG0 RST选择、ACPU WDG RST选择、ACPU REMAP、eFUSEC REMAP、USIM_HPD、EXT_PLL配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  aarm_wd_rst_cfg             : 1;  /* bit[0] : ACPU wdg复位是否引起全局复位
                                                                    1'b0：不引起全局复位；
                                                                    1'b1：引起全局复位。 */
        unsigned int  remap_sram_aarm             : 1;  /* bit[1] : 指示ACPU地址是否remap到sram，从sram启动。
                                                                    0：不从sram启动；
                                                                    1：从sram启动。 */
        unsigned int  efusec_remap                : 1;  /* bit[2] : 指示efusec读取地址remap功能
                                                                    0：从efuse读取；
                                                                    1：从镜像读取。 */
        unsigned int  ext_pll_sel                 : 1;  /* bit[3] : external pll sel
                                                                    0：internal pll sel；
                                                                    1：external pll sel
                                                                    Note:This is a debug function bit and can only be configured in SLOW sysmode state。 */
        unsigned int  mcu_wdg0_rstmcu_cfg         : 1;  /* bit[4] : MCU Watchdog0复位请求是否引起硬件自动复位MCU子系统选择。
                                                                    0：MCU Watchdog0复位请求不引起硬件自动复位MCU子系统；
                                                                    1：MCU Watchdog0复位请求引起硬件自动复位MCU子系统。 */
        unsigned int  reserved_0                  : 1;  /* bit[5] : reserved */
        unsigned int  usim0_hpd_de_bounce_cfg     : 1;  /* bit[6] : usim0_hpd 输入去抖配置
                                                                    0：不去抖；
                                                                    1:150us去抖。 */
        unsigned int  usim0_hpd_oe_cfg            : 1;  /* bit[7] : usim0 端口设置
                                                                    0：SIM卡在有插入或拔出动作时，SIM卡的CLK和RST，DATA不做高阻控制；
                                                                    1:SIM卡在有插入或拔出动作时，SIM卡CLK，RST，DATA控制为输出高阻。 */
        unsigned int  usim1_hpd_de_bounce_cfg     : 1;  /* bit[8] : usim1_hpd 输入去抖配置
                                                                    0：不去抖；
                                                                    1:150us去抖。 */
        unsigned int  usim1_hpd_oe_cfg            : 1;  /* bit[9] : usim1 端口设置
                                                                    0：SIM卡在有插入或拔出动作时，SIM卡的CLK和RST，DATA不做高阻控制；
                                                                    1:SIM卡在有插入或拔出动作时，SIM卡CLK，RST，DATA控制为输出高阻。 */
        unsigned int  bus_dfs_fore_hd_cfg         : 1;  /* bit[10]: 使能下电区的SYSNOC/CFGBUS/PERI_DMA BUS三个100MHz总线DFS由硬件投票状态确定。
                                                                    0：软件可以通过修改总线分频比修改总线频率；
                                                                    1：在总线DFS投票寄存器“SC_MCU_VOTE2STAT”在非全0下，软件无法通过修改总线分频比修改总线频率；SC_MCU_VOTE2STAT”在全0下，软件可以通过修改总线分频比修改总线频率。 */
        unsigned int  bus_dfs_fore_hd_cfg1        : 1;  /* bit[11]: 使能下电区的SYSNOC 150MHz总线DFS由硬件投票状态确定。
                                                                    0：软件可以通过修改总线分频比修改总线频率；
                                                                    1：在总线DFS投票寄存器“SC_MCU_VOTE1STAT”在非全0下，软件无法通过修改总线分频比修改总线频率；SC_MCU_VOTE1STAT”在全0下，软件可以通过修改总线分频比修改总线频率。 */
        unsigned int  usim0_hpd_oe_sft            : 1;  /* bit[12]: 软件配置USMI0的端口，此配置仅在usim0_hpd_oe_cfg=1'b1时有效
                                                                    0：SIM卡的CLK和RST信号设置为输出；DATA由内部模块控制；
                                                                    1:SIM卡的CLK和RST，DATA控制为输出高阻。
                                                                    注：此bit功能软件需连续配置两次 */
        unsigned int  usim1_hpd_oe_sft            : 1;  /* bit[13]: 软件配置USMI1的端口，此配置仅在usim1_hpd_oe_cfg=1'b1时有效
                                                                    0：SIM卡的CLK和RST信号设置为输出；DATA由内部模块控制；
                                                                    1:SIM卡的CLK和RST，DATA控制为输出高阻。
                                                                    注：此bit功能软件需连续配置两次 */
        unsigned int  reserved_1                  : 1;  /* bit[14]: reserved */
        unsigned int  mcu_clken_hardcfg           : 1;  /* bit[15]: MCU进入wfi时强制关闭MCU HCLK时钟，该功能可屏蔽
                                                                    0：MCU进入wfi时不关闭MCU HCLK时钟；
                                                                    1：MCU进入wfi时强制关闭MCU HCLK时钟。 */
        unsigned int  aarm_wd_rst_cfg_msk         : 1;  /* bit[16]: ACPU wdg复位是否引起全局复位 */
        unsigned int  remap_sram_aarm_msk         : 1;  /* bit[17]: 指示ACPU地址是否remap到sram，从sram启动。 */
        unsigned int  efusec_remap_msk            : 1;  /* bit[18]: 指示efusec读取地址remap功能 */
        unsigned int  ext_pll_sel_msk             : 1;  /* bit[19]: ext_pll_sel msk */
        unsigned int  mcu_wdg0_rstmcu_cfg_msk     : 1;  /* bit[20]: mcu_wdg0_rstmcu_cfg的写msk位 */
        unsigned int  reserved_2                  : 1;  /* bit[21]: reserved */
        unsigned int  usim0_hpd_de_bounce_cfg_msk : 1;  /* bit[22]: usim0_hpd_de_bounce_cfg msk */
        unsigned int  usim0_hpd_oe_cfg_msk        : 1;  /* bit[23]: usim0_hpd_oe_cfg msk */
        unsigned int  usim1_hpd_de_bounce_cfg_msk : 1;  /* bit[24]: usim1_hpd_de_bounce_cfg msk */
        unsigned int  usim1_hpd_oe_cfg_msk        : 1;  /* bit[25]: usim1_hpd_oe_cfg msk */
        unsigned int  bus_dfs_fore_hd_cfg_msk     : 1;  /* bit[26]: bus_dfs_fore_hd_cfg msk */
        unsigned int  bus_dfs_fore_hd_cfg1_msk    : 1;  /* bit[27]: bus_dfs_fore_hd_cfg1 msk */
        unsigned int  usim0_hpd_oe_sft_msk        : 1;  /* bit[28]: usim0_hpd_oe_sft msk */
        unsigned int  usim1_hpd_oe_sft_msk        : 1;  /* bit[29]: usim1_hpd_oe_sft msk */
        unsigned int  reserved_3                  : 1;  /* bit[30]: reserved */
        unsigned int  mcu_clken_hardcfg_msk       : 1;  /* bit[31]: [31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
                                                                    MCU进入wfi时强制关闭MCU HCLK时钟，该功能可屏蔽 */
    } reg;
} SOC_AO_SCTRL_SC_SYS_CTRL1_UNION;
#endif
#define SOC_AO_SCTRL_SC_SYS_CTRL1_aarm_wd_rst_cfg_START              (0)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_aarm_wd_rst_cfg_END                (0)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_remap_sram_aarm_START              (1)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_remap_sram_aarm_END                (1)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_efusec_remap_START                 (2)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_efusec_remap_END                   (2)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_ext_pll_sel_START                  (3)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_ext_pll_sel_END                    (3)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_mcu_wdg0_rstmcu_cfg_START          (4)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_mcu_wdg0_rstmcu_cfg_END            (4)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_de_bounce_cfg_START      (6)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_de_bounce_cfg_END        (6)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_oe_cfg_START             (7)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_oe_cfg_END               (7)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_de_bounce_cfg_START      (8)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_de_bounce_cfg_END        (8)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_oe_cfg_START             (9)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_oe_cfg_END               (9)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_bus_dfs_fore_hd_cfg_START          (10)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_bus_dfs_fore_hd_cfg_END            (10)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_bus_dfs_fore_hd_cfg1_START         (11)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_bus_dfs_fore_hd_cfg1_END           (11)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_oe_sft_START             (12)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_oe_sft_END               (12)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_oe_sft_START             (13)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_oe_sft_END               (13)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_mcu_clken_hardcfg_START            (15)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_mcu_clken_hardcfg_END              (15)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_aarm_wd_rst_cfg_msk_START          (16)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_aarm_wd_rst_cfg_msk_END            (16)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_remap_sram_aarm_msk_START          (17)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_remap_sram_aarm_msk_END            (17)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_efusec_remap_msk_START             (18)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_efusec_remap_msk_END               (18)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_ext_pll_sel_msk_START              (19)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_ext_pll_sel_msk_END                (19)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_mcu_wdg0_rstmcu_cfg_msk_START      (20)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_mcu_wdg0_rstmcu_cfg_msk_END        (20)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_de_bounce_cfg_msk_START  (22)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_de_bounce_cfg_msk_END    (22)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_oe_cfg_msk_START         (23)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_oe_cfg_msk_END           (23)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_de_bounce_cfg_msk_START  (24)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_de_bounce_cfg_msk_END    (24)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_oe_cfg_msk_START         (25)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_oe_cfg_msk_END           (25)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_bus_dfs_fore_hd_cfg_msk_START      (26)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_bus_dfs_fore_hd_cfg_msk_END        (26)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_bus_dfs_fore_hd_cfg1_msk_START     (27)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_bus_dfs_fore_hd_cfg1_msk_END       (27)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_oe_sft_msk_START         (28)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim0_hpd_oe_sft_msk_END           (28)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_oe_sft_msk_START         (29)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_usim1_hpd_oe_sft_msk_END           (29)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_mcu_clken_hardcfg_msk_START        (31)
#define SOC_AO_SCTRL_SC_SYS_CTRL1_mcu_clken_hardcfg_msk_END          (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SYS_CTRL2_UNION
 结构说明  : SC_SYS_CTRL2 寄存器结构定义。地址偏移量:0x008，初值:0x00000000，宽度:32
 寄存器说明: 系统控制寄存器2，WDG复位状态清除、全局复位状态清除。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved                : 26; /* bit[0-25]: reserved */
        unsigned int  mcu_sft_rst_stat_clear  : 1;  /* bit[26]  : 写1清除MCU Watchdog0 Core复位指示状态SC_SYS_STAT0.mcu_softrst_stat，写0无效； */
        unsigned int  mcu_wdg0_rst_stat_clear : 1;  /* bit[27]  : 写1清除MCU Watchdog0 Core复位指示状态SC_SYS_STAT0.mcu_wdgrst_stat，写0无效； */
        unsigned int  tsensor_rst_stat_clear  : 1;  /* bit[28]  : 写1清除Tsensor引起全局软复位指示状态SC_SYS_STAT0.tsensor_hardrst_stat，写0无效； */
        unsigned int  acpu_wdg_rst_stat_clear : 1;  /* bit[29]  : 写1清除ACPU Watchdog全局复指示状态SC_SYS_STAT0.acpu_wd_glb_rst_stat，写0无效； */
        unsigned int  mcu_wdg1_rst_stat_clear : 1;  /* bit[30]  : 写1清除MCU Watchdog1全局复位指示状态SC_SYS_STAT0.cm3_wdg1_rst_stat，写0无效； */
        unsigned int  glb_srst_stat_clear     : 1;  /* bit[31]  : 写1清除MCU系统软复位指示状态SC_SYS_STAT0.glb_srst_stat，写0无效； */
    } reg;
} SOC_AO_SCTRL_SC_SYS_CTRL2_UNION;
#endif
#define SOC_AO_SCTRL_SC_SYS_CTRL2_mcu_sft_rst_stat_clear_START   (26)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_mcu_sft_rst_stat_clear_END     (26)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_mcu_wdg0_rst_stat_clear_START  (27)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_mcu_wdg0_rst_stat_clear_END    (27)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_tsensor_rst_stat_clear_START   (28)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_tsensor_rst_stat_clear_END     (28)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_acpu_wdg_rst_stat_clear_START  (29)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_acpu_wdg_rst_stat_clear_END    (29)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_mcu_wdg1_rst_stat_clear_START  (30)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_mcu_wdg1_rst_stat_clear_END    (30)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_glb_srst_stat_clear_START      (31)
#define SOC_AO_SCTRL_SC_SYS_CTRL2_glb_srst_stat_clear_END        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SYS_STAT0_UNION
 结构说明  : SC_SYS_STAT0 寄存器结构定义。地址偏移量:0x010，初值:0x00000000，宽度:32
 寄存器说明: 系统状态寄存器0，用于监测系统的系统复位原因查询，
            往该寄存器写入0x48698284触发系统软复位，写入其它值无影响。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved             : 25; /* bit[0-24]: reserved */
        unsigned int  mcu_rst_stat         : 1;  /* bit[25]  : reserved */
        unsigned int  mcu_softrst_stat     : 1;  /* bit[26]  : reserved */
        unsigned int  mcu_wdgrst_stat      : 1;  /* bit[27]  : MCU_wdgrst_stat为1时，指示引起MCU单独复位源为MCU Watchdog单Core复位。起始值未知为x。
                                                               0：指示引起MCU单独复位源不是MCU Watchdog单Core复位；
                                                               1：指示引起MCU单独复位源为MCU Watchdog单Core复位。 */
        unsigned int  tsensor_hardrst_stat : 1;  /* bit[28]  : Tsensor引起全局软复位指示，高有效。起始值未知为x，不受上电复位控制，表示引起全局软复位的原因是Tsensor复位请求
                                                               0：不是Tsensor全局复位引起的系统复位；
                                                               1：Tsensor全局复位引起的系统复位。 */
        unsigned int  acpu_wd_glb_rst_stat : 1;  /* bit[29]  : 指示引起系统复位的原因。起始值未知为x，不受上电复位控制
                                                               0：不是ACPU Watchdog全局复位引起的系统复位；
                                                               1：ACPU Watchdog全局复位引起的系统复位。 */
        unsigned int  cm3_wdg1_rst_stat    : 1;  /* bit[30]  : 指示引起系统复位的原因。起始值未知为x，不受上电复位控制
                                                               0：不是MCU Watchdog全局复位引起的系统复位；
                                                               1：MCU Watchdog全局复位引起的系统复位。
                                                               注意：该bit在对寄存器SC_SYS_CTRL0 bit[30]进行写1操作时被清除。 */
        unsigned int  glb_srst_stat        : 1;  /* bit[31]  : 指示引起系统复位的原因。起始值未知为x，不受上电复位控制
                                                               0：非系统软复位；
                                                               1：系统软复位。
                                                               注意：该bit在对寄存器SC_SYS_CTRL.glb_srst_stat_clear进行写1操作时被清除。 */
    } reg;
} SOC_AO_SCTRL_SC_SYS_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_SYS_STAT0_mcu_rst_stat_START          (25)
#define SOC_AO_SCTRL_SC_SYS_STAT0_mcu_rst_stat_END            (25)
#define SOC_AO_SCTRL_SC_SYS_STAT0_mcu_softrst_stat_START      (26)
#define SOC_AO_SCTRL_SC_SYS_STAT0_mcu_softrst_stat_END        (26)
#define SOC_AO_SCTRL_SC_SYS_STAT0_mcu_wdgrst_stat_START       (27)
#define SOC_AO_SCTRL_SC_SYS_STAT0_mcu_wdgrst_stat_END         (27)
#define SOC_AO_SCTRL_SC_SYS_STAT0_tsensor_hardrst_stat_START  (28)
#define SOC_AO_SCTRL_SC_SYS_STAT0_tsensor_hardrst_stat_END    (28)
#define SOC_AO_SCTRL_SC_SYS_STAT0_acpu_wd_glb_rst_stat_START  (29)
#define SOC_AO_SCTRL_SC_SYS_STAT0_acpu_wd_glb_rst_stat_END    (29)
#define SOC_AO_SCTRL_SC_SYS_STAT0_cm3_wdg1_rst_stat_START     (30)
#define SOC_AO_SCTRL_SC_SYS_STAT0_cm3_wdg1_rst_stat_END       (30)
#define SOC_AO_SCTRL_SC_SYS_STAT0_glb_srst_stat_START         (31)
#define SOC_AO_SCTRL_SC_SYS_STAT0_glb_srst_stat_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SYS_STAT1_UNION
 结构说明  : SC_SYS_STAT1 寄存器结构定义。地址偏移量:0x014，初值:0x00010002，宽度:32
 寄存器说明: 系统状态寄存器1，用于监测系统的状态，包括efuse状态NAND BOOT和NAND BITWIDTH、JTAG模式、BOOTMODE、BOOTSEL、系统模式状态、FUNCMODE等系统状态查询。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode_status        : 4;  /* bit[0-3]  : 指示系统控制状态机当前工作模式。
                                                              0000：SLEEP；
                                                              0001：DOZE；
                                                              0011：XTALCTRL；
                                                              1011：SWtoXTAL；
                                                              1001：SWfromXTAL；
                                                              0010：SLOW；
                                                              0110：PLLCTRL；
                                                              1110：SWtoPLL；
                                                              1010：SWfromPLL；
                                                              0100：NORMAL。 */
        unsigned int  reserved_0         : 12; /* bit[4-15] : reserved */
        unsigned int  boot_sel_lock      : 1;  /* bit[16]   : PAD信号BOOT_SEL_PIN经过上电复位锁存的状态指示信号。
                                                              0：USB强制自举启动；
                                                              1：正常Onchiprom启动，然后从MMC直接读取启动程序。 */
        unsigned int  func_mode_lock     : 2;  /* bit[17-18]: 功能复用模式状态寄存器。
                                                              00：基本功能；
                                                              01：ABB单独测试模式；
                                                              10：SOC/BBP Test；
                                                              11：EFUSE，NAND，TSENSOR测试模式。 */
        unsigned int  boot_mode_lock     : 1;  /* bit[19]   : 启动模式锁存指示，仅bit0有含义
                                                              0：正常启动；从片内Onchiprom启动；
                                                              1：NAND启动指示；仅在ATE测试，从片外NAND启动。 */
        unsigned int  fun_jtag_mode_out  : 2;  /* bit[20-21]: JTAG调试模式选择状态寄存器。
                                                              {JTAG_SEL1,JTAG_SEL0}
                                                               2’b00 ：DAP；（ACPU+CCPU+MCU）
                                                               2’b01 ：BBE16 DSP；
                                                               2’b10 ：HiFi DSP；
                                                               2’b11 ：安全鉴权 JTAG链。  */
        unsigned int  reserved_1         : 5;  /* bit[22-26]: reserved */
        unsigned int  security_boot_flg  : 1;  /* bit[27]   : efuse安全启动指示
                                                              0：非安全启动；
                                                              1：安全启动。 */
        unsigned int  efuse_nandboot_msk : 1;  /* bit[28]   : efuse nand boot msk指示
                                                              0：禁止nand启动，从Onchiprom启动后不进行nand boot；
                                                              1：可以nand启动，从Onchiprom启动后进行nand boot。 */
        unsigned int  efuse_nand_bitwide : 1;  /* bit[29]   : efuse nand bit wide指示
                                                              0：8 bit；
                                                              1：16 bit。
                                                              此版本只支持8bit */
        unsigned int  reserved_2         : 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SYS_STAT1_UNION;
#endif
#define SOC_AO_SCTRL_SC_SYS_STAT1_mode_status_START         (0)
#define SOC_AO_SCTRL_SC_SYS_STAT1_mode_status_END           (3)
#define SOC_AO_SCTRL_SC_SYS_STAT1_boot_sel_lock_START       (16)
#define SOC_AO_SCTRL_SC_SYS_STAT1_boot_sel_lock_END         (16)
#define SOC_AO_SCTRL_SC_SYS_STAT1_func_mode_lock_START      (17)
#define SOC_AO_SCTRL_SC_SYS_STAT1_func_mode_lock_END        (18)
#define SOC_AO_SCTRL_SC_SYS_STAT1_boot_mode_lock_START      (19)
#define SOC_AO_SCTRL_SC_SYS_STAT1_boot_mode_lock_END        (19)
#define SOC_AO_SCTRL_SC_SYS_STAT1_fun_jtag_mode_out_START   (20)
#define SOC_AO_SCTRL_SC_SYS_STAT1_fun_jtag_mode_out_END     (21)
#define SOC_AO_SCTRL_SC_SYS_STAT1_security_boot_flg_START   (27)
#define SOC_AO_SCTRL_SC_SYS_STAT1_security_boot_flg_END     (27)
#define SOC_AO_SCTRL_SC_SYS_STAT1_efuse_nandboot_msk_START  (28)
#define SOC_AO_SCTRL_SC_SYS_STAT1_efuse_nandboot_msk_END    (28)
#define SOC_AO_SCTRL_SC_SYS_STAT1_efuse_nand_bitwide_START  (29)
#define SOC_AO_SCTRL_SC_SYS_STAT1_efuse_nand_bitwide_END    (29)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_IMCTRL_UNION
 结构说明  : SC_MCU_IMCTRL 寄存器结构定义。地址偏移量:0x018，初值:0x00000004，宽度:32
 寄存器说明: MCU中断模式控制寄存器，用于控制MCU中断发生时的系统模式。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  itmd_en0   : 1;  /* bit[0]   : MCU中断模式使能。
                                                     0：中断模式禁止；
                                                     1：中断模式使能。 */
        unsigned int  itmd_ctrl0 : 3;  /* bit[1-3] : MCU设置中断模式下系统最低的工作模式。作为MCU中断发生后系统所处的工作模式。默认进入slow模式，不建议软件修改此寄存器 */
        unsigned int  reserved   : 28; /* bit[4-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_IMCTRL_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_IMCTRL_itmd_en0_START    (0)
#define SOC_AO_SCTRL_SC_MCU_IMCTRL_itmd_en0_END      (0)
#define SOC_AO_SCTRL_SC_MCU_IMCTRL_itmd_ctrl0_START  (1)
#define SOC_AO_SCTRL_SC_MCU_IMCTRL_itmd_ctrl0_END    (3)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_IMSTAT_UNION
 结构说明  : SC_MCU_IMSTAT 寄存器结构定义。地址偏移量:0x01C，初值:0x00000000，宽度:32
 寄存器说明: MCU中断模式状态寄存器，用于监测和控制MCU系统中断模式。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  itmd_stat0_sftcfg : 1;  /* bit[0]   : MCU中断模式状态指示。
                                                            0：中断模式没有激活；
                                                            1：中断响应模式被激活。
                                                            注意：当中断服务程序结束执行时，必须将这比特位写0清除中断响应状态。 */
        unsigned int  reserved          : 31; /* bit[1-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_IMSTAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_IMSTAT_itmd_stat0_sftcfg_START  (0)
#define SOC_AO_SCTRL_SC_MCU_IMSTAT_itmd_stat0_sftcfg_END    (0)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_UNION
 结构说明  : SC_SECONDRY_INT_EN0 寄存器结构定义。地址偏移量:0x044，初值:0x00000000，宽度:32
 寄存器说明: 系统二级中断使能寄存器，用于对系统二级中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_secondry_int_en0_0bit  : 1;  /* bit[0]    : CM3_SOFT_NMI_intr中断。 */
        unsigned int  sc_secondry_int_en0_1bit  : 1;  /* bit[1]    : CM3WDog0_intr中断。 */
        unsigned int  sc_secondry_int_en0_2bit  : 1;  /* bit[2]    : DDRC_intr中断。 */
        unsigned int  sc_secondry_int_en0_15bit : 13; /* bit[3-15] : reserved */
        unsigned int  sc_secondry_int_en0_16bit : 1;  /* bit[16]   : reserved */
        unsigned int  sc_secondry_int_en0_17bit : 1;  /* bit[17]   : reserved */
        unsigned int  sc_secondry_int_en0_18bit : 1;  /* bit[18]   : reserved */
        unsigned int  sc_secondry_int_en0_19bit : 1;  /* bit[19]   : reserved */
        unsigned int  sc_secondry_int_en0_20bit : 1;  /* bit[20]   : reserved */
        unsigned int  sc_secondry_int_en0_21bit : 1;  /* bit[21]   : UART0中断，用于产线校准（用于产品板上HiFi串口调试）。 */
        unsigned int  sc_secondry_int_en0_22bit : 1;  /* bit[22]   : UART2中断，用于BT。 */
        unsigned int  sc_secondry_int_en0_23bit : 1;  /* bit[23]   : UART4中断，用于串口调试。 */
        unsigned int  sc_secondry_int_en0_31bit : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_UNION;
#endif
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_0bit_START   (0)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_0bit_END     (0)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_1bit_START   (1)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_1bit_END     (1)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_2bit_START   (2)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_2bit_END     (2)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_15bit_START  (3)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_15bit_END    (15)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_16bit_START  (16)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_16bit_END    (16)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_17bit_START  (17)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_17bit_END    (17)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_18bit_START  (18)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_18bit_END    (18)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_19bit_START  (19)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_19bit_END    (19)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_20bit_START  (20)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_20bit_END    (20)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_21bit_START  (21)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_21bit_END    (21)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_22bit_START  (22)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_22bit_END    (22)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_23bit_START  (23)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_23bit_END    (23)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_31bit_START  (24)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_EN0_sc_secondry_int_en0_31bit_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_UNION
 结构说明  : SC_SECONDRY_INT_STATR0 寄存器结构定义。地址偏移量:0x048，初值:0x00000000，宽度:32
 寄存器说明: 系统二级中断中断寄存器，用于指示系统二级原始中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_secondry_int_raw0_0bit  : 1;  /* bit[0]    : CM3_SOFT_NMI_intr中断。 */
        unsigned int  sc_secondry_int_raw0_1bit  : 1;  /* bit[1]    : CM3WDog0_intr中断。 */
        unsigned int  sc_secondry_int_raw0_2bit  : 1;  /* bit[2]    : DDRC_intr中断。 */
        unsigned int  sc_secondry_int_raw0_15bit : 13; /* bit[3-15] : reserved */
        unsigned int  sc_secondry_int_raw0_16bit : 1;  /* bit[16]   : reserved */
        unsigned int  sc_secondry_int_raw0_17bit : 1;  /* bit[17]   : reserved */
        unsigned int  sc_secondry_int_raw0_18bit : 1;  /* bit[18]   : reserved */
        unsigned int  sc_secondry_int_raw0_19bit : 1;  /* bit[19]   : reserved */
        unsigned int  sc_secondry_int_raw0_20bit : 1;  /* bit[20]   : reserved */
        unsigned int  sc_secondry_int_raw0_21bit : 1;  /* bit[21]   : UART0中断，用于产线校准（用于产品板上HiFi串口调试）。 */
        unsigned int  sc_secondry_int_raw0_22bit : 1;  /* bit[22]   : UART2中断，用于BT。 */
        unsigned int  sc_secondry_int_raw0_23bit : 1;  /* bit[23]   : UART4中断，用于串口调试。 */
        unsigned int  sc_secondry_int_raw0_31bit : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_UNION;
#endif
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_0bit_START   (0)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_0bit_END     (0)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_1bit_START   (1)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_1bit_END     (1)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_2bit_START   (2)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_2bit_END     (2)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_15bit_START  (3)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_15bit_END    (15)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_16bit_START  (16)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_16bit_END    (16)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_17bit_START  (17)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_17bit_END    (17)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_18bit_START  (18)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_18bit_END    (18)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_19bit_START  (19)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_19bit_END    (19)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_20bit_START  (20)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_20bit_END    (20)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_21bit_START  (21)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_21bit_END    (21)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_22bit_START  (22)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_22bit_END    (22)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_23bit_START  (23)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_23bit_END    (23)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_31bit_START  (24)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATR0_sc_secondry_int_raw0_31bit_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_UNION
 结构说明  : SC_SECONDRY_INT_STATM0 寄存器结构定义。地址偏移量:0x04C，初值:0x00000000，宽度:32
 寄存器说明: 系统二级中断屏蔽后寄存器，用于指示系统二级中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_secondry_int_mask0_0bit  : 1;  /* bit[0]    : CM3_SOFT_NMI_intr中断。 */
        unsigned int  sc_secondry_int_mask0_1bit  : 1;  /* bit[1]    : CM3WDog0_intr中断。 */
        unsigned int  sc_secondry_int_mask0_2bit  : 1;  /* bit[2]    : DDRC_intr中断。 */
        unsigned int  sc_secondry_int_mask0_15bit : 13; /* bit[3-15] : reserved */
        unsigned int  sc_secondry_int_mask0_16bit : 1;  /* bit[16]   : reserved */
        unsigned int  sc_secondry_int_mask0_17bit : 1;  /* bit[17]   : reserved */
        unsigned int  sc_secondry_int_mask0_18bit : 1;  /* bit[18]   : reserved */
        unsigned int  sc_secondry_int_mask0_19bit : 1;  /* bit[19]   : reserved */
        unsigned int  sc_secondry_int_mask0_20bit : 1;  /* bit[20]   : reserved */
        unsigned int  sc_secondry_int_mask0_21bit : 1;  /* bit[21]   : UART0中断，用于产线校准（用于产品板上HiFi串口调试）。 */
        unsigned int  sc_secondry_int_mask0_22bit : 1;  /* bit[22]   : UART2中断，用于BT。 */
        unsigned int  sc_secondry_int_mask0_23bit : 1;  /* bit[23]   : UART4中断，用于串口调试。 */
        unsigned int  sc_secondry_int_mask0_31bit : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_UNION;
#endif
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_0bit_START   (0)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_0bit_END     (0)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_1bit_START   (1)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_1bit_END     (1)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_2bit_START   (2)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_2bit_END     (2)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_15bit_START  (3)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_15bit_END    (15)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_16bit_START  (16)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_16bit_END    (16)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_17bit_START  (17)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_17bit_END    (17)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_18bit_START  (18)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_18bit_END    (18)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_19bit_START  (19)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_19bit_END    (19)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_20bit_START  (20)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_20bit_END    (20)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_21bit_START  (21)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_21bit_END    (21)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_22bit_START  (22)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_22bit_END    (22)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_23bit_START  (23)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_23bit_END    (23)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_31bit_START  (24)
#define SOC_AO_SCTRL_SC_SECONDRY_INT_STATM0_sc_secondry_int_mask0_31bit_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_UNION
 结构说明  : SC_MCU_WKUP_INT_EN6 寄存器结构定义。地址偏移量:0x054，初值:0x00000000，宽度:32
 寄存器说明: MCU唤醒中断使能寄存器，对应MCU的唤醒中断bit32～63，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_en6_WatchDog0    : 1;  /* bit[0]    : WatchDog0超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_en6_WatchDog1    : 1;  /* bit[1]    : WatchDog1超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_en6_WatchDog2    : 1;  /* bit[2]    : WatchDog2超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_en6_CM3WatchDog1 : 1;  /* bit[3]    : CM3 WatchDog1 超时中断。 */
        unsigned int  mcu_wakeup_int_en6_MWatchDog0   : 1;  /* bit[4]    : MDM WatchDog0超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_en6_GPIO14       : 1;  /* bit[5]    : GPIO14中断。 */
        unsigned int  mcu_wakeup_int_en6_GPIO15       : 1;  /* bit[6]    : GPIO15中断。 */
        unsigned int  mcu_wakeup_int_en6_GPIO16       : 1;  /* bit[7]    : GPIO16中断。 */
        unsigned int  mcu_wakeup_int_en6_GPIO17       : 1;  /* bit[8]    : GPIO17中断。 */
        unsigned int  mcu_wakeup_int_en6_GPIO18       : 1;  /* bit[9]    : GPIO18中断。 */
        unsigned int  mcu_wakeup_int_en6_GPIO19       : 1;  /* bit[10]   : GPIO19中断。 */
        unsigned int  mcu_wakeup_int_en6_CM3Timer2    : 1;  /* bit[11]   : CM3Timer2中断。 */
        unsigned int  mcu_wakeup_int_en6_CM3Timer3    : 1;  /* bit[12]   : CM3Timer3中断。 */
        unsigned int  mcu_wakeup_int_en6_PWM0         : 1;  /* bit[13]   : PWM0 int中断。 */
        unsigned int  mcu_wakeup_int_en6_PWM1         : 1;  /* bit[14]   : PWM1 int中断。 */
        unsigned int  mcu_wakeup_int_en6_PWMIN        : 1;  /* bit[15]   : PWMIN int中断。 */
        unsigned int  mcu_wakeup_int_en6_reserved     : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_WatchDog0_START     (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_WatchDog0_END       (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_WatchDog1_START     (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_WatchDog1_END       (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_WatchDog2_START     (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_WatchDog2_END       (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_CM3WatchDog1_START  (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_CM3WatchDog1_END    (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_MWatchDog0_START    (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_MWatchDog0_END      (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO14_START        (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO14_END          (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO15_START        (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO15_END          (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO16_START        (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO16_END          (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO17_START        (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO17_END          (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO18_START        (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO18_END          (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO19_START        (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_GPIO19_END          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_CM3Timer2_START     (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_CM3Timer2_END       (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_CM3Timer3_START     (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_CM3Timer3_END       (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_PWM0_START          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_PWM0_END            (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_PWM1_START          (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_PWM1_END            (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_PWMIN_START         (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_PWMIN_END           (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_reserved_START      (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN6_mcu_wakeup_int_en6_reserved_END        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_UNION
 结构说明  : SC_MCU_WKUP_INT_STATR6 寄存器结构定义。地址偏移量:0x058，初值:0x00000000，宽度:32
 寄存器说明: MCU原始唤醒中断寄存器，对应MCU的唤醒中断bit32～63，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_raw6_WatchDog0    : 1;  /* bit[0]    : WatchDog0超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_raw6_WatchDog1    : 1;  /* bit[1]    : WatchDog1超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_raw6_WatchDog2    : 1;  /* bit[2]    : WatchDog2超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_raw6_CM3WatchDog1 : 1;  /* bit[3]    : CM3 WatchDog1 超时中断。 */
        unsigned int  mcu_wakeup_int_raw6_MWatchDog0   : 1;  /* bit[4]    : MDM WatchDog0超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_raw6_GPIO14       : 1;  /* bit[5]    : GPIO14中断。 */
        unsigned int  mcu_wakeup_int_raw6_GPIO15       : 1;  /* bit[6]    : GPIO15中断。 */
        unsigned int  mcu_wakeup_int_raw6_GPIO16       : 1;  /* bit[7]    : GPIO16中断。 */
        unsigned int  mcu_wakeup_int_raw6_GPIO17       : 1;  /* bit[8]    : GPIO17中断。 */
        unsigned int  mcu_wakeup_int_raw6_GPIO18       : 1;  /* bit[9]    : GPIO18中断。 */
        unsigned int  mcu_wakeup_int_raw6_GPIO19       : 1;  /* bit[10]   : GPIO19中断。 */
        unsigned int  mcu_wakeup_int_raw6_CM3Timer2    : 1;  /* bit[11]   : CM3Timer2中断。 */
        unsigned int  mcu_wakeup_int_raw6_CM3Timer3    : 1;  /* bit[12]   : CM3Timer3中断。 */
        unsigned int  mcu_wakeup_int_raw6_PWM0         : 1;  /* bit[13]   : PWM0 int中断。 */
        unsigned int  mcu_wakeup_int_raw6_PWM1         : 1;  /* bit[14]   : PWM1 int中断。 */
        unsigned int  mcu_wakeup_int_raw6_PWMIN        : 1;  /* bit[15]   : PWMIN int中断。 */
        unsigned int  mcu_wakeup_int_raw6_reserved     : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_WatchDog0_START     (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_WatchDog0_END       (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_WatchDog1_START     (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_WatchDog1_END       (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_WatchDog2_START     (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_WatchDog2_END       (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_CM3WatchDog1_START  (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_CM3WatchDog1_END    (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_MWatchDog0_START    (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_MWatchDog0_END      (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO14_START        (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO14_END          (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO15_START        (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO15_END          (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO16_START        (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO16_END          (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO17_START        (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO17_END          (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO18_START        (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO18_END          (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO19_START        (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_GPIO19_END          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_CM3Timer2_START     (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_CM3Timer2_END       (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_CM3Timer3_START     (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_CM3Timer3_END       (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_PWM0_START          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_PWM0_END            (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_PWM1_START          (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_PWM1_END            (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_PWMIN_START         (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_PWMIN_END           (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_reserved_START      (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR6_mcu_wakeup_int_raw6_reserved_END        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_UNION
 结构说明  : SC_MCU_WKUP_INT_STATM6 寄存器结构定义。地址偏移量:0x05C，初值:0x00000000，宽度:32
 寄存器说明: MCU屏蔽后唤醒中断寄存器，对应MCU的唤醒中断bit32～63，用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_mask6_WatchDog0    : 1;  /* bit[0]    : WatchDog0超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_mask6_WatchDog1    : 1;  /* bit[1]    : WatchDog1超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_mask6_WatchDog2    : 1;  /* bit[2]    : WatchDog2超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_mask6_CM3WatchDog1 : 1;  /* bit[3]    : CM3 WatchDog1 超时中断。 */
        unsigned int  mcu_wakeup_int_mask6_MWatchDog0   : 1;  /* bit[4]    : MDM WatchDog0超时复位请求中断。 */
        unsigned int  mcu_wakeup_int_mask6_GPIO14       : 1;  /* bit[5]    : GPIO14中断。 */
        unsigned int  mcu_wakeup_int_mask6_GPIO15       : 1;  /* bit[6]    : GPIO15中断。 */
        unsigned int  mcu_wakeup_int_mask6_GPIO16       : 1;  /* bit[7]    : GPIO16中断。 */
        unsigned int  mcu_wakeup_int_mask6_GPIO17       : 1;  /* bit[8]    : GPIO17中断。 */
        unsigned int  mcu_wakeup_int_mask6_GPIO18       : 1;  /* bit[9]    : GPIO18中断。 */
        unsigned int  mcu_wakeup_int_mask6_GPIO19       : 1;  /* bit[10]   : GPIO19中断。 */
        unsigned int  mcu_wakeup_int_mask6_CM3Timer2    : 1;  /* bit[11]   : CM3Timer2中断。 */
        unsigned int  mcu_wakeup_int_mask6_CM3Timer3    : 1;  /* bit[12]   : CM3Timer3中断。 */
        unsigned int  mcu_wakeup_int_mask6_PWM0         : 1;  /* bit[13]   : PWM0 int中断。 */
        unsigned int  mcu_wakeup_int_mask6_PWM1         : 1;  /* bit[14]   : PWM1 int中断。 */
        unsigned int  mcu_wakeup_int_mask6_PWMIN        : 1;  /* bit[15]   : PWMIN int中断。 */
        unsigned int  mcu_wakeup_int_mask6_reserved     : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_WatchDog0_START     (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_WatchDog0_END       (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_WatchDog1_START     (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_WatchDog1_END       (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_WatchDog2_START     (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_WatchDog2_END       (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_CM3WatchDog1_START  (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_CM3WatchDog1_END    (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_MWatchDog0_START    (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_MWatchDog0_END      (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO14_START        (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO14_END          (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO15_START        (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO15_END          (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO16_START        (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO16_END          (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO17_START        (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO17_END          (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO18_START        (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO18_END          (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO19_START        (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_GPIO19_END          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_CM3Timer2_START     (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_CM3Timer2_END       (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_CM3Timer3_START     (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_CM3Timer3_END       (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_PWM0_START          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_PWM0_END            (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_PWM1_START          (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_PWM1_END            (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_PWMIN_START         (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_PWMIN_END           (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_reserved_START      (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM6_mcu_wakeup_int_mask6_reserved_END        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_UNION
 结构说明  : SC_MCU_WKUP_INT_EN5 寄存器结构定义。地址偏移量:0x064，初值:0x00000000，宽度:32
 寄存器说明: MCU唤醒中断使能寄存器，对应MCU的唤醒中断bit31～0，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_en5_GPIO0        : 1;  /* bit[0] : 0： GPIO0 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO1        : 1;  /* bit[1] : 1： GPIO1 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO2        : 1;  /* bit[2] : 2： GPIO2 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO3        : 1;  /* bit[3] : 3： GPIO3 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO4        : 1;  /* bit[4] : 4： GPIO4 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO5        : 1;  /* bit[5] : 5： GPIO5 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO6        : 1;  /* bit[6] : 6： GPIO6 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO7        : 1;  /* bit[7] : [7] ：GPIO7 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO8        : 1;  /* bit[8] : [8] ：GPIO8中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO9        : 1;  /* bit[9] : [9] ：GPIO9中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO10       : 1;  /* bit[10]: [10]：GPIO10 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO11       : 1;  /* bit[11]: [11]：GPIO11中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO12       : 1;  /* bit[12]: [12]：GPIO12 中断。 */
        unsigned int  mcu_wakeup_int_en5_GPIO13       : 1;  /* bit[13]: [13]：GPIO13中断。 */
        unsigned int  mcu_wakeup_int_en5_CSYSPWRUPREQ : 1;  /* bit[14]: [14]：CSYSPWRUPREQ CoreSight system power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_en5_CDBGPWRUPREQ : 1;  /* bit[15]: [15]：CDBGPWRUPREQ CoreSight debug power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_en5_16reserved   : 1;  /* bit[16]: reserved */
        unsigned int  mcu_wakeup_int_en5_RTC1_INT     : 1;  /* bit[17]: [17]：RTC1_INT */
        unsigned int  mcu_wakeup_int_en5_RTC0_INT     : 1;  /* bit[18]: [18]：RTC0_INT  */
        unsigned int  mcu_wakeup_int_en5_19reserved   : 1;  /* bit[19]: 19： reserved */
        unsigned int  mcu_wakeup_int_en5_20reserved   : 1;  /* bit[20]: 20： reserved */
        unsigned int  mcu_wakeup_int_en5_Timer14      : 1;  /* bit[21]: 21： Timer14 AP Dual_Timer7_0中断。 */
        unsigned int  mcu_wakeup_int_en5_Timer15      : 1;  /* bit[22]: 22： Timer15 AP Dual_Timer7_1中断。 */
        unsigned int  mcu_wakeup_int_en5_Timer16      : 1;  /* bit[23]: 23： Timer16 AP Dual_Timer8_0中断。 */
        unsigned int  mcu_wakeup_int_en5_Timer17      : 1;  /* bit[24]: 24： Timer17 AP Dual_Timer8_1中断。 */
        unsigned int  mcu_wakeup_int_en5_CM3Timer0    : 1;  /* bit[25]: 25： CM3 Timer0 CM3 Dual_Timer0_0中断。 */
        unsigned int  mcu_wakeup_int_en5_CM3Timer1    : 1;  /* bit[26]: 26： CM3 Timer1 CM3 Dual_Timer0_1中断。 */
        unsigned int  mcu_wakeup_int_en5_MTimer16     : 1;  /* bit[27]: 27： MTimer16 Modem Dual_Timer8_0中断。 */
        unsigned int  mcu_wakeup_int_en5_MTimer17     : 1;  /* bit[28]: 28： MTimer17 Modem Dual_Timer8_1中断。 */
        unsigned int  mcu_wakeup_int_en5_MTimer18     : 1;  /* bit[29]: 29： MTimer18 Modem Dual_Timer9_0中断。 */
        unsigned int  mcu_wakeup_int_en5_MTimer19     : 1;  /* bit[30]: 30： MTimer19 Modem Dual_Timer9_1中断。 */
        unsigned int  mcu_wakeup_int_en5_CM3WatchDog0 : 1;  /* bit[31]: 31： CM3 WatchDog0复位请求中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO0_START         (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO0_END           (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO1_START         (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO1_END           (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO2_START         (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO2_END           (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO3_START         (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO3_END           (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO4_START         (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO4_END           (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO5_START         (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO5_END           (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO6_START         (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO6_END           (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO7_START         (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO7_END           (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO8_START         (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO8_END           (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO9_START         (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO9_END           (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO10_START        (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO10_END          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO11_START        (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO11_END          (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO12_START        (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO12_END          (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO13_START        (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_GPIO13_END          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CSYSPWRUPREQ_START  (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CSYSPWRUPREQ_END    (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CDBGPWRUPREQ_START  (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CDBGPWRUPREQ_END    (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_16reserved_START    (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_16reserved_END      (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_RTC1_INT_START      (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_RTC1_INT_END        (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_RTC0_INT_START      (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_RTC0_INT_END        (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_19reserved_START    (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_19reserved_END      (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_20reserved_START    (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_20reserved_END      (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_Timer14_START       (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_Timer14_END         (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_Timer15_START       (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_Timer15_END         (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_Timer16_START       (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_Timer16_END         (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_Timer17_START       (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_Timer17_END         (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CM3Timer0_START     (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CM3Timer0_END       (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CM3Timer1_START     (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CM3Timer1_END       (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_MTimer16_START      (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_MTimer16_END        (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_MTimer17_START      (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_MTimer17_END        (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_MTimer18_START      (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_MTimer18_END        (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_MTimer19_START      (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_MTimer19_END        (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CM3WatchDog0_START  (31)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN5_mcu_wakeup_int_en5_CM3WatchDog0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_UNION
 结构说明  : SC_MCU_WKUP_INT_STATR5 寄存器结构定义。地址偏移量:0x068，初值:0x00000000，宽度:32
 寄存器说明: MCU原始唤醒中断寄存器，对应MCU的唤醒中断bit31～0，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_raw5_GPIO0        : 1;  /* bit[0] : 0： GPIO0 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO1        : 1;  /* bit[1] : 1： GPIO1 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO2        : 1;  /* bit[2] : 2： GPIO2 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO3        : 1;  /* bit[3] : 3： GPIO3 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO4        : 1;  /* bit[4] : 4： GPIO4 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO5        : 1;  /* bit[5] : 5： GPIO5 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO6        : 1;  /* bit[6] : 6： GPIO6 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO7        : 1;  /* bit[7] : [7] ：GPIO7 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO8        : 1;  /* bit[8] : [8] ：GPIO8中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO9        : 1;  /* bit[9] : [9] ：GPIO9中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO10       : 1;  /* bit[10]: [10]：GPIO10 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO11       : 1;  /* bit[11]: [11]：GPIO11中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO12       : 1;  /* bit[12]: [12]：GPIO12 中断。 */
        unsigned int  mcu_wakeup_int_raw5_GPIO13       : 1;  /* bit[13]: [13]：GPIO13中断。 */
        unsigned int  mcu_wakeup_int_raw5_CSYSPWRUPREQ : 1;  /* bit[14]: [14]：CSYSPWRUPREQ CoreSight system power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_raw5_CDBGPWRUPREQ : 1;  /* bit[15]: [15]：CDBGPWRUPREQ CoreSight debug power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_raw5_16reserved   : 1;  /* bit[16]: reserved */
        unsigned int  mcu_wakeup_int_raw5_RTC1_INT     : 1;  /* bit[17]: [17]：RTC1_INT */
        unsigned int  mcu_wakeup_int_raw5_RTC0_INT     : 1;  /* bit[18]: [18]：RTC0_INT  */
        unsigned int  mcu_wakeup_int_raw5_19reserved   : 1;  /* bit[19]: 19： reserved */
        unsigned int  mcu_wakeup_int_raw5_20reserved   : 1;  /* bit[20]: 20： reserved */
        unsigned int  mcu_wakeup_int_raw5_Timer14      : 1;  /* bit[21]: 21： Timer14 AP Dual_Timer7_0中断。 */
        unsigned int  mcu_wakeup_int_raw5_Timer15      : 1;  /* bit[22]: 22： Timer15 AP Dual_Timer7_1中断。 */
        unsigned int  mcu_wakeup_int_raw5_Timer16      : 1;  /* bit[23]: 23： Timer16 AP Dual_Timer8_0中断。 */
        unsigned int  mcu_wakeup_int_raw5_Timer17      : 1;  /* bit[24]: 24： Timer17 AP Dual_Timer8_1中断。 */
        unsigned int  mcu_wakeup_int_raw5_CM3Timer0    : 1;  /* bit[25]: 25： CM3 Timer0 CM3 Dual_Timer0_0中断。 */
        unsigned int  mcu_wakeup_int_raw5_CM3Timer1    : 1;  /* bit[26]: 26： CM3 Timer1 CM3 Dual_Timer0_1中断。 */
        unsigned int  mcu_wakeup_int_raw5_MTimer16     : 1;  /* bit[27]: 27： MTimer16 Modem Dual_Timer8_0中断。 */
        unsigned int  mcu_wakeup_int_raw5_MTimer17     : 1;  /* bit[28]: 28： MTimer17 Modem Dual_Timer8_1中断。 */
        unsigned int  mcu_wakeup_int_raw5_MTimer18     : 1;  /* bit[29]: 29： MTimer18 Modem Dual_Timer9_0中断。 */
        unsigned int  mcu_wakeup_int_raw5_MTimer19     : 1;  /* bit[30]: 30： MTimer19 Modem Dual_Timer9_1中断。 */
        unsigned int  mcu_wakeup_int_raw5_CM3WatchDog0 : 1;  /* bit[31]: 31： CM3 WatchDog0复位请求中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO0_START         (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO0_END           (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO1_START         (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO1_END           (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO2_START         (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO2_END           (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO3_START         (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO3_END           (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO4_START         (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO4_END           (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO5_START         (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO5_END           (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO6_START         (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO6_END           (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO7_START         (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO7_END           (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO8_START         (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO8_END           (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO9_START         (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO9_END           (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO10_START        (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO10_END          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO11_START        (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO11_END          (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO12_START        (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO12_END          (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO13_START        (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_GPIO13_END          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CSYSPWRUPREQ_START  (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CSYSPWRUPREQ_END    (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CDBGPWRUPREQ_START  (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CDBGPWRUPREQ_END    (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_16reserved_START    (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_16reserved_END      (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_RTC1_INT_START      (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_RTC1_INT_END        (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_RTC0_INT_START      (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_RTC0_INT_END        (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_19reserved_START    (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_19reserved_END      (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_20reserved_START    (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_20reserved_END      (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_Timer14_START       (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_Timer14_END         (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_Timer15_START       (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_Timer15_END         (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_Timer16_START       (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_Timer16_END         (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_Timer17_START       (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_Timer17_END         (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CM3Timer0_START     (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CM3Timer0_END       (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CM3Timer1_START     (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CM3Timer1_END       (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_MTimer16_START      (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_MTimer16_END        (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_MTimer17_START      (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_MTimer17_END        (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_MTimer18_START      (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_MTimer18_END        (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_MTimer19_START      (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_MTimer19_END        (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CM3WatchDog0_START  (31)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR5_mcu_wakeup_int_raw5_CM3WatchDog0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_UNION
 结构说明  : SC_MCU_WKUP_INT_STATM5 寄存器结构定义。地址偏移量:0x06C，初值:0x00000000，宽度:32
 寄存器说明: MCU屏蔽后唤醒中断寄存器，对应MCU的唤醒中断bit31～0，用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_mask5_GPIO0        : 1;  /* bit[0] : 0： GPIO0 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO1        : 1;  /* bit[1] : 1： GPIO1 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO2        : 1;  /* bit[2] : 2： GPIO2 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO3        : 1;  /* bit[3] : 3： GPIO3 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO4        : 1;  /* bit[4] : 4： GPIO4 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO5        : 1;  /* bit[5] : 5： GPIO5 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO6        : 1;  /* bit[6] : 6： GPIO6 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO7        : 1;  /* bit[7] : [7] ：GPIO7 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO8        : 1;  /* bit[8] : [8] ：GPIO8中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO9        : 1;  /* bit[9] : [9] ：GPIO9中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO10       : 1;  /* bit[10]: [10]：GPIO10 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO11       : 1;  /* bit[11]: [11]：GPIO11中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO12       : 1;  /* bit[12]: [12]：GPIO12 中断。 */
        unsigned int  mcu_wakeup_int_mask5_GPIO13       : 1;  /* bit[13]: [13]：GPIO13中断。 */
        unsigned int  mcu_wakeup_int_mask5_CSYSPWRUPREQ : 1;  /* bit[14]: [14]：CSYSPWRUPREQ CoreSight system power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_mask5_CDBGPWRUPREQ : 1;  /* bit[15]: [15]：CDBGPWRUPREQ CoreSight debug power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_mask5_16reserved   : 1;  /* bit[16]: reserved */
        unsigned int  mcu_wakeup_int_mask5_RTC1_INT     : 1;  /* bit[17]: [17]：RTC1_INT */
        unsigned int  mcu_wakeup_int_mask5_RTC0_INT     : 1;  /* bit[18]: [18]：RTC0_INT  */
        unsigned int  mcu_wakeup_int_mask5_19reserved   : 1;  /* bit[19]: 19： reserved */
        unsigned int  mcu_wakeup_int_mask5_20reserved   : 1;  /* bit[20]: 20： reserved */
        unsigned int  mcu_wakeup_int_mask5_Timer14      : 1;  /* bit[21]: 21： Timer14 AP Dual_Timer7_0中断。 */
        unsigned int  mcu_wakeup_int_mask5_Timer15      : 1;  /* bit[22]: 22： Timer15 AP Dual_Timer7_1中断。 */
        unsigned int  mcu_wakeup_int_mask5_Timer16      : 1;  /* bit[23]: 23： Timer16 AP Dual_Timer8_0中断。 */
        unsigned int  mcu_wakeup_int_mask5_Timer17      : 1;  /* bit[24]: 24： Timer17 AP Dual_Timer8_1中断。 */
        unsigned int  mcu_wakeup_int_mask5_CM3Timer0    : 1;  /* bit[25]: 25： CM3 Timer0 CM3 Dual_Timer0_0中断。 */
        unsigned int  mcu_wakeup_int_mask5_CM3Timer1    : 1;  /* bit[26]: 26： CM3 Timer1 CM3 Dual_Timer0_1中断。 */
        unsigned int  mcu_wakeup_int_mask5_MTimer16     : 1;  /* bit[27]: 27： MTimer16 Modem Dual_Timer8_0中断。 */
        unsigned int  mcu_wakeup_int_mask5_MTimer17     : 1;  /* bit[28]: 28： MTimer17 Modem Dual_Timer8_1中断。 */
        unsigned int  mcu_wakeup_int_mask5_MTimer18     : 1;  /* bit[29]: 29： MTimer18 Modem Dual_Timer9_0中断。 */
        unsigned int  mcu_wakeup_int_mask5_MTimer19     : 1;  /* bit[30]: 30： MTimer19 Modem Dual_Timer9_1中断。 */
        unsigned int  mcu_wakeup_int_mask5_CM3WatchDog0 : 1;  /* bit[31]: 31： CM3 WatchDog0复位请求中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO0_START         (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO0_END           (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO1_START         (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO1_END           (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO2_START         (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO2_END           (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO3_START         (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO3_END           (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO4_START         (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO4_END           (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO5_START         (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO5_END           (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO6_START         (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO6_END           (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO7_START         (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO7_END           (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO8_START         (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO8_END           (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO9_START         (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO9_END           (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO10_START        (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO10_END          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO11_START        (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO11_END          (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO12_START        (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO12_END          (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO13_START        (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_GPIO13_END          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CSYSPWRUPREQ_START  (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CSYSPWRUPREQ_END    (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CDBGPWRUPREQ_START  (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CDBGPWRUPREQ_END    (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_16reserved_START    (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_16reserved_END      (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_RTC1_INT_START      (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_RTC1_INT_END        (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_RTC0_INT_START      (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_RTC0_INT_END        (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_19reserved_START    (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_19reserved_END      (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_20reserved_START    (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_20reserved_END      (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_Timer14_START       (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_Timer14_END         (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_Timer15_START       (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_Timer15_END         (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_Timer16_START       (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_Timer16_END         (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_Timer17_START       (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_Timer17_END         (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CM3Timer0_START     (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CM3Timer0_END       (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CM3Timer1_START     (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CM3Timer1_END       (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_MTimer16_START      (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_MTimer16_END        (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_MTimer17_START      (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_MTimer17_END        (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_MTimer18_START      (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_MTimer18_END        (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_MTimer19_START      (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_MTimer19_END        (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CM3WatchDog0_START  (31)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM5_mcu_wakeup_int_mask5_CM3WatchDog0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_UNION
 结构说明  : SC_MCU_WKUP_INT_EN4 寄存器结构定义。地址偏移量:0x094，初值:0x00000000，宽度:32
 寄存器说明: HiFi唤醒中断送给MCU的唤醒中断使能寄存器，对应HiFi的唤醒中断bit0～31，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_en4_CSYSPWRUPREQ   : 1;  /* bit[0]    : CSYSPWRUPREQ 中断。  */
        unsigned int  mcu_wakeup_int_en4_CDBGPWRUPREQ   : 1;  /* bit[1]    : CDBGPWRUPREQ 中断。  */
        unsigned int  mcu_wakeup_int_en4_MTimer10       : 1;  /* bit[2]    : MTimer10 中断。  */
        unsigned int  mcu_wakeup_int_en4_MTimer11       : 1;  /* bit[3]    : MTimer11中断。  */
        unsigned int  mcu_wakeup_int_en4_MTimer12       : 1;  /* bit[4]    : MTimer12中断。 */
        unsigned int  mcu_wakeup_int_en4_MTimer13       : 1;  /* bit[5]    : MTimer13中断。 */
        unsigned int  mcu_wakeup_int_en4_WatchDog1      : 1;  /* bit[6]    : Hifi WatchDog中断。 */
        unsigned int  mcu_wakeup_int_en4_7reserved      : 1;  /* bit[7]    : reserved */
        unsigned int  mcu_wakeup_int_en4_IPC2HIFI_0     : 1;  /* bit[8]    : mcu_wakeup_int_en4_IPC2HIFI_0
                                                                             IPC2HIFI_0 非安全IPC输出给HIFI2的核间中断。 */
        unsigned int  mcu_wakeup_int_en4_HIFI_NMI_INT   : 1;  /* bit[9]    : HIFI_NMI_INT 中断。 */
        unsigned int  mcu_wakeup_int_en4_AP_EDMAC_NSORS : 1;  /* bit[10]   : AP EDMAC安全中断3和AP EDMAC非安全中断3的相或后输出。 */
        unsigned int  mcu_wakeup_int_en4_11reserved     : 1;  /* bit[11]   : reserved */
        unsigned int  mcu_wakeup_int_en4_SEC_IPC2HIFI_0 : 1;  /* bit[12]   : 安全IPC输出给HIFI2的核间中断。 */
        unsigned int  mcu_wakeup_int_en4_13GPIO11       : 1;  /* bit[13]   : GPIO11的8bit组合中断。 */
        unsigned int  mcu_wakeup_int_en4_14reserved     : 1;  /* bit[14]   : reserved */
        unsigned int  mcu_wakeup_int_en4_15reserved     : 1;  /* bit[15]   : reserved */
        unsigned int  mcu_wakeup_int_en4_31reserved     : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_CSYSPWRUPREQ_START    (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_CSYSPWRUPREQ_END      (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_CDBGPWRUPREQ_START    (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_CDBGPWRUPREQ_END      (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_MTimer10_START        (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_MTimer10_END          (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_MTimer11_START        (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_MTimer11_END          (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_MTimer12_START        (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_MTimer12_END          (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_MTimer13_START        (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_MTimer13_END          (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_WatchDog1_START       (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_WatchDog1_END         (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_7reserved_START       (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_7reserved_END         (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_IPC2HIFI_0_START      (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_IPC2HIFI_0_END        (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_HIFI_NMI_INT_START    (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_HIFI_NMI_INT_END      (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_AP_EDMAC_NSORS_START  (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_AP_EDMAC_NSORS_END    (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_11reserved_START      (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_11reserved_END        (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_SEC_IPC2HIFI_0_START  (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_SEC_IPC2HIFI_0_END    (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_13GPIO11_START        (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_13GPIO11_END          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_14reserved_START      (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_14reserved_END        (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_15reserved_START      (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_15reserved_END        (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_31reserved_START      (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN4_mcu_wakeup_int_en4_31reserved_END        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_UNION
 结构说明  : SC_MCU_WKUP_INT_STATR4 寄存器结构定义。地址偏移量:0x098，初值:0x00000000，宽度:32
 寄存器说明: HiFi唤醒中断送给MCU的原始唤醒中断寄存器，对应HiFi的唤醒中断bit0～31，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_raw4_CSYSPWRUPREQ   : 1;  /* bit[0]    : CSYSPWRUPREQ 中断。  */
        unsigned int  mcu_wakeup_int_raw4_CDBGPWRUPREQ   : 1;  /* bit[1]    : CDBGPWRUPREQ 中断。  */
        unsigned int  mcu_wakeup_int_raw4_MTimer10       : 1;  /* bit[2]    : MTimer10 中断。  */
        unsigned int  mcu_wakeup_int_raw4_MTimer11       : 1;  /* bit[3]    : MTimer11中断。  */
        unsigned int  mcu_wakeup_int_raw4_MTimer12       : 1;  /* bit[4]    : MTimer12中断。 */
        unsigned int  mcu_wakeup_int_raw4_MTimer13       : 1;  /* bit[5]    : MTimer13中断。 */
        unsigned int  mcu_wakeup_int_raw4_WatchDog1      : 1;  /* bit[6]    : Hifi WatchDog中断。 */
        unsigned int  mcu_wakeup_int_raw4_7reserved      : 1;  /* bit[7]    : reserved */
        unsigned int  mcu_wakeup_int_raw4_IPC2HIFI_0     : 1;  /* bit[8]    : mcu_wakeup_int_en4_IPC2HIFI_0
                                                                              IPC2HIFI_0 非安全IPC输出给HIFI2的核间中断。 */
        unsigned int  mcu_wakeup_int_raw4_HIFI_NMI_INT   : 1;  /* bit[9]    : HIFI_NMI_INT 中断。 */
        unsigned int  mcu_wakeup_int_raw4_AP_EDMAC_NSORS : 1;  /* bit[10]   : AP EDMAC安全中断3和AP EDMAC非安全中断3的相或后输出。 */
        unsigned int  mcu_wakeup_int_raw4_11reserved     : 1;  /* bit[11]   : reserved */
        unsigned int  mcu_wakeup_int_raw4_SEC_IPC2HIFI_0 : 1;  /* bit[12]   : 安全IPC输出给HIFI2的核间中断。 */
        unsigned int  mcu_wakeup_int_raw4_13GPIO11       : 1;  /* bit[13]   : GPIO11的8bit组合中断。 */
        unsigned int  mcu_wakeup_int_raw4_14reserved     : 1;  /* bit[14]   : reserved */
        unsigned int  mcu_wakeup_int_raw4_15reserved     : 1;  /* bit[15]   : reserved */
        unsigned int  mcu_wakeup_int_raw4_31reserved     : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_CSYSPWRUPREQ_START    (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_CSYSPWRUPREQ_END      (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_CDBGPWRUPREQ_START    (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_CDBGPWRUPREQ_END      (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_MTimer10_START        (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_MTimer10_END          (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_MTimer11_START        (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_MTimer11_END          (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_MTimer12_START        (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_MTimer12_END          (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_MTimer13_START        (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_MTimer13_END          (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_WatchDog1_START       (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_WatchDog1_END         (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_7reserved_START       (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_7reserved_END         (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_IPC2HIFI_0_START      (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_IPC2HIFI_0_END        (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_HIFI_NMI_INT_START    (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_HIFI_NMI_INT_END      (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_AP_EDMAC_NSORS_START  (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_AP_EDMAC_NSORS_END    (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_11reserved_START      (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_11reserved_END        (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_SEC_IPC2HIFI_0_START  (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_SEC_IPC2HIFI_0_END    (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_13GPIO11_START        (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_13GPIO11_END          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_14reserved_START      (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_14reserved_END        (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_15reserved_START      (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_15reserved_END        (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_31reserved_START      (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR4_mcu_wakeup_int_raw4_31reserved_END        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_UNION
 结构说明  : SC_MCU_WKUP_INT_STATM4 寄存器结构定义。地址偏移量:0x09C，初值:0x00000000，宽度:32
 寄存器说明: HiFi唤醒中断送给MCU的屏蔽后唤醒中断寄存器，对应HiFi的唤醒中断bit0～31，用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_mask4_CSYSPWRUPREQ   : 1;  /* bit[0]    : CSYSPWRUPREQ 中断。  */
        unsigned int  mcu_wakeup_int_mask4_CDBGPWRUPREQ   : 1;  /* bit[1]    : CDBGPWRUPREQ 中断。  */
        unsigned int  mcu_wakeup_int_mask4_MTimer10       : 1;  /* bit[2]    : MTimer10 中断。  */
        unsigned int  mcu_wakeup_int_mask4_MTimer11       : 1;  /* bit[3]    : MTimer11中断。  */
        unsigned int  mcu_wakeup_int_mask4_MTimer12       : 1;  /* bit[4]    : MTimer12中断。 */
        unsigned int  mcu_wakeup_int_mask4_MTimer13       : 1;  /* bit[5]    : MTimer13中断。 */
        unsigned int  mcu_wakeup_int_mask4_WatchDog1      : 1;  /* bit[6]    : Hifi WatchDog中断。 */
        unsigned int  mcu_wakeup_int_mask4_7reserved      : 1;  /* bit[7]    : reserved */
        unsigned int  mcu_wakeup_int_mask4_IPC2HIFI_0     : 1;  /* bit[8]    : mcu_wakeup_int_en4_IPC2HIFI_0
                                                                               IPC2HIFI_0 非安全IPC输出给HIFI2的核间中断。 */
        unsigned int  mcu_wakeup_int_mask4_HIFI_NMI_INT   : 1;  /* bit[9]    : HIFI_NMI_INT 中断。 */
        unsigned int  mcu_wakeup_int_mask4_AP_EDMAC_NSORS : 1;  /* bit[10]   : AP EDMAC安全中断3和AP EDMAC非安全中断3的相或后输出。 */
        unsigned int  mcu_wakeup_int_mask4_11reserved     : 1;  /* bit[11]   : reserved */
        unsigned int  mcu_wakeup_int_mask4_SEC_IPC2HIFI_0 : 1;  /* bit[12]   : 安全IPC输出给HIFI2的核间中断。 */
        unsigned int  mcu_wakeup_int_mask4_13GPIO11       : 1;  /* bit[13]   : GPIO11的8bit组合中断。 */
        unsigned int  mcu_wakeup_int_mask4_14reserved     : 1;  /* bit[14]   : reserved */
        unsigned int  mcu_wakeup_int_mask4_15reserved     : 1;  /* bit[15]   : reserved */
        unsigned int  mcu_wakeup_int_mask4_31reserved     : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_CSYSPWRUPREQ_START    (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_CSYSPWRUPREQ_END      (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_CDBGPWRUPREQ_START    (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_CDBGPWRUPREQ_END      (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_MTimer10_START        (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_MTimer10_END          (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_MTimer11_START        (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_MTimer11_END          (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_MTimer12_START        (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_MTimer12_END          (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_MTimer13_START        (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_MTimer13_END          (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_WatchDog1_START       (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_WatchDog1_END         (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_7reserved_START       (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_7reserved_END         (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_IPC2HIFI_0_START      (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_IPC2HIFI_0_END        (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_HIFI_NMI_INT_START    (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_HIFI_NMI_INT_END      (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_AP_EDMAC_NSORS_START  (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_AP_EDMAC_NSORS_END    (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_11reserved_START      (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_11reserved_END        (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_SEC_IPC2HIFI_0_START  (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_SEC_IPC2HIFI_0_END    (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_13GPIO11_START        (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_13GPIO11_END          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_14reserved_START      (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_14reserved_END        (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_15reserved_START      (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_15reserved_END        (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_31reserved_START      (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM4_mcu_wakeup_int_mask4_31reserved_END        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_UNION
 结构说明  : SC_MCU_WKUP_INT_EN0 寄存器结构定义。地址偏移量:0x0A8，初值:0x00000000，宽度:32
 寄存器说明: ACPU唤醒中断送给MCU的唤醒中断使能寄存器，对应ACPU的唤醒中断bit0～31，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_en0_GPIO0            : 1;  /* bit[0] : [0] ：GPIO0中断。  */
        unsigned int  mcu_wakeup_int_en0_GPIO1            : 1;  /* bit[1] : [1] ：GPIO1中断。 */
        unsigned int  mcu_wakeup_int_en0_GPIO2            : 1;  /* bit[2] : [2] ：GPIO2中断。  */
        unsigned int  mcu_wakeup_int_en0_GPIO3            : 1;  /* bit[3] : [3] ：GPIO3中断。  */
        unsigned int  mcu_wakeup_int_en0_4reserved        : 1;  /* bit[4] : reserved */
        unsigned int  mcu_wakeup_int_en0_5reserved        : 1;  /* bit[5] : reserved */
        unsigned int  mcu_wakeup_int_en0_6reserved        : 1;  /* bit[6] : reserved */
        unsigned int  mcu_wakeup_int_en0_7reserved        : 1;  /* bit[7] : reserved */
        unsigned int  mcu_wakeup_int_en0_8reserved        : 1;  /* bit[8] : reserved */
        unsigned int  mcu_wakeup_int_en0_9reserved        : 1;  /* bit[9] : reserved */
        unsigned int  mcu_wakeup_int_en0_10reserved       : 1;  /* bit[10]: reserved */
        unsigned int  mcu_wakeup_int_en0_11reserved       : 1;  /* bit[11]: reserved */
        unsigned int  mcu_wakeup_int_en0_12reserved       : 1;  /* bit[12]: reserved */
        unsigned int  mcu_wakeup_int_en0_13reserved       : 1;  /* bit[13]: reserved */
        unsigned int  mcu_wakeup_int_en0_CSYSPWRUPREQ     : 1;  /* bit[14]: [14]：CSYSPWRUPREQ CoreSight system power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_en0_CDBGPWRUPREQ     : 1;  /* bit[15]: [15]：CDBGPWRUPREQ CoreSight debug power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_en0_scoff_2acpu_comb : 1;  /* bit[16]: [16]：SCOFF到ACPU的组合唤醒中断，具体参考SCOFF二级中断寄存器。 */
        unsigned int  mcu_wakeup_int_en0_RTC1_INT         : 1;  /* bit[17]: [17]：RTC1_INT中断。  */
        unsigned int  mcu_wakeup_int_en0_RTC0_INT         : 1;  /* bit[18]: [18]：RTC0_INT中断。  */
        unsigned int  mcu_wakeup_int_en0_Timer0           : 1;  /* bit[19]: [27]：Timer0 AP Dual_Timer0_0中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer1           : 1;  /* bit[20]: [28]：Timer1 AP Dual_Timer0_1中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer2           : 1;  /* bit[21]: [29]：Timer2 AP Dual_Timer1_0中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer3           : 1;  /* bit[22]: [30]：Timer3 AP Dual_Timer1_1中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer4           : 1;  /* bit[23]: [23]：Timer4 AP Dual_Timer2_0中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer5           : 1;  /* bit[24]: [24]：Timer5 AP Dual_Timer2_1中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer6           : 1;  /* bit[25]: [25]：Timer6 AP Dual_Timer3_0中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer7           : 1;  /* bit[26]: [26]：Timer7 AP Dual_Timer3_1中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer8           : 1;  /* bit[27]: [27]：Timer8 AP Dual_Timer4_0中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer9           : 1;  /* bit[28]: [28]：Timer9 AP Dual_Timer4_1中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer10          : 1;  /* bit[29]: [29]：Timer10 AP Dual_Timer5_0中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer11          : 1;  /* bit[30]: [30]：Timer11 AP Dual_Timer5_1中断。 */
        unsigned int  mcu_wakeup_int_en0_Timer12          : 1;  /* bit[31]: [31]： Timer12 AP Dual_Timer6_0中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_GPIO0_START             (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_GPIO0_END               (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_GPIO1_START             (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_GPIO1_END               (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_GPIO2_START             (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_GPIO2_END               (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_GPIO3_START             (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_GPIO3_END               (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_4reserved_START         (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_4reserved_END           (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_5reserved_START         (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_5reserved_END           (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_6reserved_START         (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_6reserved_END           (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_7reserved_START         (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_7reserved_END           (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_8reserved_START         (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_8reserved_END           (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_9reserved_START         (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_9reserved_END           (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_10reserved_START        (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_10reserved_END          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_11reserved_START        (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_11reserved_END          (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_12reserved_START        (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_12reserved_END          (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_13reserved_START        (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_13reserved_END          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_CSYSPWRUPREQ_START      (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_CSYSPWRUPREQ_END        (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_CDBGPWRUPREQ_START      (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_CDBGPWRUPREQ_END        (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_scoff_2acpu_comb_START  (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_scoff_2acpu_comb_END    (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_RTC1_INT_START          (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_RTC1_INT_END            (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_RTC0_INT_START          (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_RTC0_INT_END            (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer0_START            (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer0_END              (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer1_START            (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer1_END              (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer2_START            (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer2_END              (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer3_START            (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer3_END              (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer4_START            (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer4_END              (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer5_START            (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer5_END              (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer6_START            (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer6_END              (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer7_START            (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer7_END              (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer8_START            (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer8_END              (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer9_START            (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer9_END              (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer10_START           (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer10_END             (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer11_START           (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer11_END             (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer12_START           (31)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN0_mcu_wakeup_int_en0_Timer12_END             (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_UNION
 结构说明  : SC_MCU_WKUP_INT_STATR0 寄存器结构定义。地址偏移量:0x0AC，初值:0x00000000，宽度:32
 寄存器说明: ACPU唤醒中断送给MCU的原始唤醒中断寄存器，对应ACPU的唤醒中断bit0～31，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_raw0_GPIO0            : 1;  /* bit[0] : [0] ：GPIO0中断。  */
        unsigned int  mcu_wakeup_int_raw0_GPIO1            : 1;  /* bit[1] : [1] ：GPIO1中断。 */
        unsigned int  mcu_wakeup_int_raw0_GPIO2            : 1;  /* bit[2] : [2] ：GPIO2中断。  */
        unsigned int  mcu_wakeup_int_raw0_GPIO3            : 1;  /* bit[3] : [3] ：GPIO3中断。  */
        unsigned int  mcu_wakeup_int_raw0_4reserved        : 1;  /* bit[4] : reserved */
        unsigned int  mcu_wakeup_int_raw0_5reserved        : 1;  /* bit[5] : reserved */
        unsigned int  mcu_wakeup_int_raw0_6reserved        : 1;  /* bit[6] : reserved */
        unsigned int  mcu_wakeup_int_raw0_7reserved        : 1;  /* bit[7] : reserved */
        unsigned int  mcu_wakeup_int_raw0_8reserved        : 1;  /* bit[8] : reserved */
        unsigned int  mcu_wakeup_int_raw0_9reserved        : 1;  /* bit[9] : reserved */
        unsigned int  mcu_wakeup_int_raw0_10reserved       : 1;  /* bit[10]: reserved */
        unsigned int  mcu_wakeup_int_raw0_11reserved       : 1;  /* bit[11]: reserved */
        unsigned int  mcu_wakeup_int_raw0_12reserved       : 1;  /* bit[12]: reserved */
        unsigned int  mcu_wakeup_int_raw0_13reserved       : 1;  /* bit[13]: reserved */
        unsigned int  mcu_wakeup_int_raw0_CSYSPWRUPREQ     : 1;  /* bit[14]: [14]：CSYSPWRUPREQ CoreSight system power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_raw0_CDBGPWRUPREQ     : 1;  /* bit[15]: [15]：CDBGPWRUPREQ CoreSight debug power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_raw0_scoff_2acpu_comb : 1;  /* bit[16]: [16]：SCOFF到ACPU的组合唤醒中断，具体参考SCOFF二级中断寄存器。 */
        unsigned int  mcu_wakeup_int_raw0_RTC1_INT         : 1;  /* bit[17]: [17]：RTC1_INT中断。  */
        unsigned int  mcu_wakeup_int_raw0_RTC0_INT         : 1;  /* bit[18]: [18]：RTC0_INT中断。  */
        unsigned int  mcu_wakeup_int_raw0_Timer0           : 1;  /* bit[19]: [27]：Timer0 AP Dual_Timer0_0中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer1           : 1;  /* bit[20]: [28]：Timer1 AP Dual_Timer0_1中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer2           : 1;  /* bit[21]: [29]：Timer2 AP Dual_Timer1_0中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer3           : 1;  /* bit[22]: [30]：Timer3 AP Dual_Timer1_1中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer4           : 1;  /* bit[23]: [23]：Timer4 AP Dual_Timer2_0中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer5           : 1;  /* bit[24]: [24]：Timer5 AP Dual_Timer2_1中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer6           : 1;  /* bit[25]: [25]：Timer6 AP Dual_Timer3_0中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer7           : 1;  /* bit[26]: [26]：Timer7 AP Dual_Timer3_1中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer8           : 1;  /* bit[27]: [27]：Timer8 AP Dual_Timer4_0中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer9           : 1;  /* bit[28]: [28]：Timer9 AP Dual_Timer4_1中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer10          : 1;  /* bit[29]: [29]：Timer10 AP Dual_Timer5_0中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer11          : 1;  /* bit[30]: [30]：Timer11 AP Dual_Timer5_1中断。 */
        unsigned int  mcu_wakeup_int_raw0_Timer12          : 1;  /* bit[31]: [31]： Timer12 AP Dual_Timer6_0中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_GPIO0_START             (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_GPIO0_END               (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_GPIO1_START             (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_GPIO1_END               (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_GPIO2_START             (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_GPIO2_END               (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_GPIO3_START             (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_GPIO3_END               (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_4reserved_START         (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_4reserved_END           (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_5reserved_START         (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_5reserved_END           (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_6reserved_START         (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_6reserved_END           (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_7reserved_START         (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_7reserved_END           (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_8reserved_START         (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_8reserved_END           (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_9reserved_START         (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_9reserved_END           (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_10reserved_START        (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_10reserved_END          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_11reserved_START        (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_11reserved_END          (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_12reserved_START        (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_12reserved_END          (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_13reserved_START        (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_13reserved_END          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_CSYSPWRUPREQ_START      (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_CSYSPWRUPREQ_END        (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_CDBGPWRUPREQ_START      (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_CDBGPWRUPREQ_END        (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_scoff_2acpu_comb_START  (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_scoff_2acpu_comb_END    (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_RTC1_INT_START          (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_RTC1_INT_END            (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_RTC0_INT_START          (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_RTC0_INT_END            (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer0_START            (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer0_END              (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer1_START            (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer1_END              (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer2_START            (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer2_END              (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer3_START            (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer3_END              (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer4_START            (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer4_END              (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer5_START            (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer5_END              (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer6_START            (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer6_END              (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer7_START            (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer7_END              (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer8_START            (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer8_END              (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer9_START            (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer9_END              (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer10_START           (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer10_END             (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer11_START           (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer11_END             (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer12_START           (31)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR0_mcu_wakeup_int_raw0_Timer12_END             (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_UNION
 结构说明  : SC_MCU_WKUP_INT_STATM0 寄存器结构定义。地址偏移量:0x0B0，初值:0x00000000，宽度:32
 寄存器说明: ACPU唤醒中断送给MCU的屏蔽后唤醒中断寄存器，对应ACPU的唤醒中断bit0～31，用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_mask0_GPIO0            : 1;  /* bit[0] : [0] ：GPIO0中断。  */
        unsigned int  mcu_wakeup_int_mask0_GPIO1            : 1;  /* bit[1] : [1] ：GPIO1中断。 */
        unsigned int  mcu_wakeup_int_mask0_GPIO2            : 1;  /* bit[2] : [2] ：GPIO2中断。  */
        unsigned int  mcu_wakeup_int_mask0_GPIO3            : 1;  /* bit[3] : [3] ：GPIO3中断。  */
        unsigned int  mcu_wakeup_int_mask0_4reserved        : 1;  /* bit[4] : reserved */
        unsigned int  mcu_wakeup_int_mask0_5reserved        : 1;  /* bit[5] : reserved */
        unsigned int  mcu_wakeup_int_mask0_6reserved        : 1;  /* bit[6] : reserved */
        unsigned int  mcu_wakeup_int_mask0_7reserved        : 1;  /* bit[7] : reserved */
        unsigned int  mcu_wakeup_int_mask0_8reserved        : 1;  /* bit[8] : reserved */
        unsigned int  mcu_wakeup_int_mask0_9reserved        : 1;  /* bit[9] : reserved */
        unsigned int  mcu_wakeup_int_mask0_10reserved       : 1;  /* bit[10]: reserved */
        unsigned int  mcu_wakeup_int_mask0_11reserved       : 1;  /* bit[11]: reserved */
        unsigned int  mcu_wakeup_int_mask0_12reserved       : 1;  /* bit[12]: reserved */
        unsigned int  mcu_wakeup_int_mask0_13reserved       : 1;  /* bit[13]: reserved */
        unsigned int  mcu_wakeup_int_mask0_CSYSPWRUPREQ     : 1;  /* bit[14]: [14]：CSYSPWRUPREQ CoreSight system power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_mask0_CDBGPWRUPREQ     : 1;  /* bit[15]: [15]：CDBGPWRUPREQ CoreSight debug power up请求状态信号，高表示请求有效。 */
        unsigned int  mcu_wakeup_int_mask0_scoff_2acpu_comb : 1;  /* bit[16]: [16]：SCOFF到ACPU的组合唤醒中断，具体参考SCOFF二级中断寄存器。 */
        unsigned int  mcu_wakeup_int_mask0_RTC1_INT         : 1;  /* bit[17]: [17]：RTC1_INT中断。  */
        unsigned int  mcu_wakeup_int_mask0_RTC0_INT         : 1;  /* bit[18]: [18]：RTC0_INT中断。  */
        unsigned int  mcu_wakeup_int_mask0_Timer0           : 1;  /* bit[19]: [27]：Timer0 AP Dual_Timer0_0中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer1           : 1;  /* bit[20]: [28]：Timer1 AP Dual_Timer0_1中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer2           : 1;  /* bit[21]: [29]：Timer2 AP Dual_Timer1_0中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer3           : 1;  /* bit[22]: [30]：Timer3 AP Dual_Timer1_1中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer4           : 1;  /* bit[23]: [23]：Timer4 AP Dual_Timer2_0中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer5           : 1;  /* bit[24]: [24]：Timer5 AP Dual_Timer2_1中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer6           : 1;  /* bit[25]: [25]：Timer6 AP Dual_Timer3_0中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer7           : 1;  /* bit[26]: [26]：Timer7 AP Dual_Timer3_1中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer8           : 1;  /* bit[27]: [27]：Timer8 AP Dual_Timer4_0中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer9           : 1;  /* bit[28]: [28]：Timer9 AP Dual_Timer4_1中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer10          : 1;  /* bit[29]: [29]：Timer10 AP Dual_Timer5_0中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer11          : 1;  /* bit[30]: [30]：Timer11 AP Dual_Timer5_1中断。 */
        unsigned int  mcu_wakeup_int_mask0_Timer12          : 1;  /* bit[31]: [31]： Timer12 AP Dual_Timer6_0中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_GPIO0_START             (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_GPIO0_END               (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_GPIO1_START             (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_GPIO1_END               (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_GPIO2_START             (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_GPIO2_END               (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_GPIO3_START             (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_GPIO3_END               (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_4reserved_START         (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_4reserved_END           (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_5reserved_START         (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_5reserved_END           (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_6reserved_START         (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_6reserved_END           (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_7reserved_START         (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_7reserved_END           (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_8reserved_START         (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_8reserved_END           (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_9reserved_START         (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_9reserved_END           (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_10reserved_START        (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_10reserved_END          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_11reserved_START        (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_11reserved_END          (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_12reserved_START        (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_12reserved_END          (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_13reserved_START        (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_13reserved_END          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_CSYSPWRUPREQ_START      (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_CSYSPWRUPREQ_END        (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_CDBGPWRUPREQ_START      (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_CDBGPWRUPREQ_END        (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_scoff_2acpu_comb_START  (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_scoff_2acpu_comb_END    (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_RTC1_INT_START          (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_RTC1_INT_END            (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_RTC0_INT_START          (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_RTC0_INT_END            (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer0_START            (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer0_END              (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer1_START            (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer1_END              (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer2_START            (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer2_END              (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer3_START            (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer3_END              (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer4_START            (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer4_END              (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer5_START            (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer5_END              (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer6_START            (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer6_END              (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer7_START            (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer7_END              (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer8_START            (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer8_END              (27)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer9_START            (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer9_END              (28)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer10_START           (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer10_END             (29)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer11_START           (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer11_END             (30)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer12_START           (31)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM0_mcu_wakeup_int_mask0_Timer12_END             (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_UNION
 结构说明  : SC_MCU_WKUP_INT_EN1 寄存器结构定义。地址偏移量:0x0B4，初值:0x00000000，宽度:32
 寄存器说明: ACPU唤醒中断送给MCU的唤醒中断使能寄存器，对应ACPU的唤醒中断bit32～63，用于对MCU的唤醒中断进行屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_en1_Timer13            : 1;  /* bit[0]    : [0] ：Timer13 AP Dual_Timer6_1中断。 */
        unsigned int  mcu_wakeup_int_en1_Timer14            : 1;  /* bit[1]    : [1] ：Timer14 AP Dual_Timer7_0中断。  */
        unsigned int  mcu_wakeup_int_en1_Timer15            : 1;  /* bit[2]    : [2] ：Timer15 AP Dual_Timer7_1中断。  */
        unsigned int  mcu_wakeup_int_en1_Timer16            : 1;  /* bit[3]    : [3] ：Timer16 AP Dual_Timer8_0中断。  */
        unsigned int  mcu_wakeup_int_en1_Timer17            : 1;  /* bit[4]    : [4] ：Timer17 AP Dual_Timer8_1中断。  */
        unsigned int  mcu_wakeup_int_en1_WatchDog0          : 1;  /* bit[5]    : [5] ：WatchDog0 ACPU WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_en1_WatchDog1          : 1;  /* bit[6]    : [6] ：WatchDog1 HIFI WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_en1_MWatchDog2         : 1;  /* bit[7]    : [7] ：WatchDog2 安全WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_en1_ACPU_WKUP_VOTE     : 1;  /* bit[8]    : [8] ：ACPU_WKUP_VOTE中断。 */
        unsigned int  mcu_wakeup_int_en1_NS_IPC2ACPU_0      : 1;  /* bit[9]    : [9] ：非安全IPC输出给ACPU的核间中断0（ipc_cpu_intr0[0]）。 */
        unsigned int  mcu_wakeup_int_en1_10reserved         : 1;  /* bit[10]   : reserved */
        unsigned int  mcu_wakeup_int_en1_11reserved         : 1;  /* bit[11]   : reserved */
        unsigned int  mcu_wakeup_int_en1_12reserved         : 1;  /* bit[12]   : reserved */
        unsigned int  mcu_wakeup_int_en1_13reserved         : 1;  /* bit[13]   : reserved */
        unsigned int  mcu_wakeup_int_en1_14reserved         : 1;  /* bit[14]   : reserved */
        unsigned int  mcu_wakeup_int_en1_15reserved         : 1;  /* bit[15]   : reserved */
        unsigned int  mcu_wakeup_int_en1_16reserved         : 1;  /* bit[16]   : reserved */
        unsigned int  mcu_wakeup_int_en1_17reserved         : 1;  /* bit[17]   : reserved */
        unsigned int  mcu_wakeup_int_en1_18reserved         : 1;  /* bit[18]   : reserved */
        unsigned int  mcu_wakeup_int_en1_19reserved         : 1;  /* bit[19]   : reserved */
        unsigned int  mcu_wakeup_int_en1_NS_IPC2ACPU_2      : 1;  /* bit[20]   : 非安全IPC输出给ACPU的核间中断1（ipc_cpu_intr0[5]）。 */
        unsigned int  mcu_wakeup_int_en1_SEC_IPC2ACPU_2     : 1;  /* bit[21]   : 安全IPC输出给ACPU的核间中断1（ipc_cpu_intr0[0]）。 */
        unsigned int  mcu_wakeup_int_en1_MWDog0_rstreq_intr : 1;  /* bit[22]   : CCPU WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_en1_PWM0               : 1;  /* bit[23]   : PWM0 int中断。 */
        unsigned int  mcu_wakeup_int_en1_PWM1               : 1;  /* bit[24]   : PWM1 int中断。 */
        unsigned int  mcu_wakeup_int_en1_PWMIN              : 1;  /* bit[25]   : PWMIN int中断。 */
        unsigned int  mcu_wakeup_int_en1_31reserved         : 6;  /* bit[26-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer13_START             (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer13_END               (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer14_START             (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer14_END               (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer15_START             (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer15_END               (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer16_START             (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer16_END               (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer17_START             (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_Timer17_END               (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_WatchDog0_START           (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_WatchDog0_END             (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_WatchDog1_START           (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_WatchDog1_END             (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_MWatchDog2_START          (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_MWatchDog2_END            (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_ACPU_WKUP_VOTE_START      (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_ACPU_WKUP_VOTE_END        (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_NS_IPC2ACPU_0_START       (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_NS_IPC2ACPU_0_END         (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_10reserved_START          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_10reserved_END            (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_11reserved_START          (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_11reserved_END            (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_12reserved_START          (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_12reserved_END            (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_13reserved_START          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_13reserved_END            (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_14reserved_START          (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_14reserved_END            (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_15reserved_START          (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_15reserved_END            (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_16reserved_START          (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_16reserved_END            (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_17reserved_START          (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_17reserved_END            (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_18reserved_START          (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_18reserved_END            (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_19reserved_START          (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_19reserved_END            (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_NS_IPC2ACPU_2_START       (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_NS_IPC2ACPU_2_END         (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_SEC_IPC2ACPU_2_START      (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_SEC_IPC2ACPU_2_END        (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_MWDog0_rstreq_intr_START  (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_MWDog0_rstreq_intr_END    (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_PWM0_START                (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_PWM0_END                  (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_PWM1_START                (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_PWM1_END                  (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_PWMIN_START               (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_PWMIN_END                 (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_31reserved_START          (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_mcu_wakeup_int_en1_31reserved_END            (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_UNION
 结构说明  : SC_MCU_WKUP_INT_STATR1 寄存器结构定义。地址偏移量:0x0B8，初值:0x00000000，宽度:32
 寄存器说明: ACPU唤醒中断送给MCU的原始唤醒中断寄存器，对应ACPU的唤醒中断bit32～63，用于指示MCU的原始唤醒中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_raw1_Timer13            : 1;  /* bit[0]    : [0] ：Timer13 AP Dual_Timer6_1中断。 */
        unsigned int  mcu_wakeup_int_raw1_Timer14            : 1;  /* bit[1]    : [1] ：Timer14 AP Dual_Timer7_0中断。  */
        unsigned int  mcu_wakeup_int_raw1_Timer15            : 1;  /* bit[2]    : [2] ：Timer15 AP Dual_Timer7_1中断。  */
        unsigned int  mcu_wakeup_int_raw1_Timer16            : 1;  /* bit[3]    : [3] ：Timer16 AP Dual_Timer8_0中断。  */
        unsigned int  mcu_wakeup_int_raw1_Timer17            : 1;  /* bit[4]    : [4] ：Timer17 AP Dual_Timer8_1中断。  */
        unsigned int  mcu_wakeup_int_raw1_WatchDog0          : 1;  /* bit[5]    : [5] ：WatchDog0 ACPU WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_raw1_WatchDog1          : 1;  /* bit[6]    : [6] ：WatchDog1 HIFI WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_raw1_MWatchDog2         : 1;  /* bit[7]    : [7] ：WatchDog2 安全WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_raw1_ACPU_WKUP_VOTE     : 1;  /* bit[8]    : [8] ：ACPU_WKUP_VOTE中断。 */
        unsigned int  mcu_wakeup_int_raw1_NS_IPC2ACPU_0      : 1;  /* bit[9]    : [9] ：非安全IPC输出给ACPU的核间中断0（ipc_cpu_intr0[0]）。 */
        unsigned int  mcu_wakeup_int_raw1_10reserved         : 1;  /* bit[10]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_11reserved         : 1;  /* bit[11]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_12reserved         : 1;  /* bit[12]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_13reserved         : 1;  /* bit[13]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_14reserved         : 1;  /* bit[14]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_15reserved         : 1;  /* bit[15]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_16reserved         : 1;  /* bit[16]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_17reserved         : 1;  /* bit[17]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_18reserved         : 1;  /* bit[18]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_19reserved         : 1;  /* bit[19]   : reserved */
        unsigned int  mcu_wakeup_int_raw1_NS_IPC2ACPU_2      : 1;  /* bit[20]   : 非安全IPC输出给ACPU的核间中断1（ipc_cpu_intr0[5]）。 */
        unsigned int  mcu_wakeup_int_raw1_SEC_IPC2ACPU_2     : 1;  /* bit[21]   : 安全IPC输出给ACPU的核间中断1（ipc_cpu_intr0[0]）。 */
        unsigned int  mcu_wakeup_int_raw1_MWDog0_rstreq_intr : 1;  /* bit[22]   : CCPU WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_raw1_PWM0               : 1;  /* bit[23]   : PWM0 int中断。 */
        unsigned int  mcu_wakeup_int_raw1_PWM1               : 1;  /* bit[24]   : PWM1 int中断。 */
        unsigned int  mcu_wakeup_int_raw1_PWMIN              : 1;  /* bit[25]   : PWMIN int中断。 */
        unsigned int  mcu_wakeup_int_raw1_31reserved         : 6;  /* bit[26-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer13_START             (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer13_END               (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer14_START             (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer14_END               (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer15_START             (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer15_END               (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer16_START             (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer16_END               (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer17_START             (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_Timer17_END               (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_WatchDog0_START           (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_WatchDog0_END             (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_WatchDog1_START           (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_WatchDog1_END             (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_MWatchDog2_START          (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_MWatchDog2_END            (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_ACPU_WKUP_VOTE_START      (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_ACPU_WKUP_VOTE_END        (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_NS_IPC2ACPU_0_START       (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_NS_IPC2ACPU_0_END         (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_10reserved_START          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_10reserved_END            (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_11reserved_START          (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_11reserved_END            (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_12reserved_START          (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_12reserved_END            (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_13reserved_START          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_13reserved_END            (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_14reserved_START          (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_14reserved_END            (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_15reserved_START          (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_15reserved_END            (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_16reserved_START          (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_16reserved_END            (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_17reserved_START          (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_17reserved_END            (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_18reserved_START          (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_18reserved_END            (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_19reserved_START          (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_19reserved_END            (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_NS_IPC2ACPU_2_START       (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_NS_IPC2ACPU_2_END         (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_SEC_IPC2ACPU_2_START      (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_SEC_IPC2ACPU_2_END        (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_MWDog0_rstreq_intr_START  (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_MWDog0_rstreq_intr_END    (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_PWM0_START                (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_PWM0_END                  (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_PWM1_START                (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_PWM1_END                  (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_PWMIN_START               (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_PWMIN_END                 (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_31reserved_START          (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATR1_mcu_wakeup_int_raw1_31reserved_END            (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_UNION
 结构说明  : SC_MCU_WKUP_INT_STATM1 寄存器结构定义。地址偏移量:0x0BC，初值:0x00000000，宽度:32
 寄存器说明: ACPU唤醒中断送给MCU的屏蔽后唤醒中断寄存器，对应ACPU的唤醒中断bit32～63。用于指示MCU的当前唤醒中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_wakeup_int_mask1_Timer13            : 1;  /* bit[0]    : [0] ：Timer13 AP Dual_Timer6_1中断。 */
        unsigned int  mcu_wakeup_int_mask1_Timer14            : 1;  /* bit[1]    : [1] ：Timer14 AP Dual_Timer7_0中断。  */
        unsigned int  mcu_wakeup_int_mask1_Timer15            : 1;  /* bit[2]    : [2] ：Timer15 AP Dual_Timer7_1中断。  */
        unsigned int  mcu_wakeup_int_mask1_Timer16            : 1;  /* bit[3]    : [3] ：Timer16 AP Dual_Timer8_0中断。  */
        unsigned int  mcu_wakeup_int_mask1_Timer17            : 1;  /* bit[4]    : [4] ：Timer17 AP Dual_Timer8_1中断。  */
        unsigned int  mcu_wakeup_int_mask1_WatchDog0          : 1;  /* bit[5]    : [5] ：WatchDog0 ACPU WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_mask1_WatchDog1          : 1;  /* bit[6]    : [6] ：WatchDog1 HIFI WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_mask1_MWatchDog2         : 1;  /* bit[7]    : [7] ：WatchDog2 安全WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_mask1_ACPU_WKUP_VOTE     : 1;  /* bit[8]    : [8] ：ACPU_WKUP_VOTE中断。 */
        unsigned int  mcu_wakeup_int_mask1_NS_IPC2ACPU_0      : 1;  /* bit[9]    : [9] ：非安全IPC输出给ACPU的核间中断0（ipc_cpu_intr0[0]）。 */
        unsigned int  mcu_wakeup_int_mask1_10reserved         : 1;  /* bit[10]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_11reserved         : 1;  /* bit[11]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_12reserved         : 1;  /* bit[12]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_13reserved         : 1;  /* bit[13]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_14reserved         : 1;  /* bit[14]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_15reserved         : 1;  /* bit[15]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_16reserved         : 1;  /* bit[16]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_17reserved         : 1;  /* bit[17]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_18reserved         : 1;  /* bit[18]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_19reserved         : 1;  /* bit[19]   : reserved */
        unsigned int  mcu_wakeup_int_mask1_NS_IPC2ACPU_2      : 1;  /* bit[20]   : 非安全IPC输出给ACPU的核间中断1（ipc_cpu_intr0[5]）。 */
        unsigned int  mcu_wakeup_int_mask1_SEC_IPC2ACPU_2     : 1;  /* bit[21]   : 安全IPC输出给ACPU的核间中断1（ipc_cpu_intr0[0]）。 */
        unsigned int  mcu_wakeup_int_mask1_MWDog0_rstreq_intr : 1;  /* bit[22]   : CCPU WatchDog超时复位请求。 */
        unsigned int  mcu_wakeup_int_mask1_PWM0               : 1;  /* bit[23]   : PWM0 int中断。 */
        unsigned int  mcu_wakeup_int_mask1_PWM1               : 1;  /* bit[24]   : PWM1 int中断。 */
        unsigned int  mcu_wakeup_int_mask1_PWMIN              : 1;  /* bit[25]   : PWMIN int中断。 */
        unsigned int  mcu_wakeup_int_mask1_31reserved         : 6;  /* bit[26-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer13_START             (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer13_END               (0)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer14_START             (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer14_END               (1)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer15_START             (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer15_END               (2)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer16_START             (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer16_END               (3)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer17_START             (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_Timer17_END               (4)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_WatchDog0_START           (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_WatchDog0_END             (5)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_WatchDog1_START           (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_WatchDog1_END             (6)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_MWatchDog2_START          (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_MWatchDog2_END            (7)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_ACPU_WKUP_VOTE_START      (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_ACPU_WKUP_VOTE_END        (8)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_NS_IPC2ACPU_0_START       (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_NS_IPC2ACPU_0_END         (9)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_10reserved_START          (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_10reserved_END            (10)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_11reserved_START          (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_11reserved_END            (11)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_12reserved_START          (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_12reserved_END            (12)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_13reserved_START          (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_13reserved_END            (13)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_14reserved_START          (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_14reserved_END            (14)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_15reserved_START          (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_15reserved_END            (15)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_16reserved_START          (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_16reserved_END            (16)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_17reserved_START          (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_17reserved_END            (17)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_18reserved_START          (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_18reserved_END            (18)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_19reserved_START          (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_19reserved_END            (19)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_NS_IPC2ACPU_2_START       (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_NS_IPC2ACPU_2_END         (20)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_SEC_IPC2ACPU_2_START      (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_SEC_IPC2ACPU_2_END        (21)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_MWDog0_rstreq_intr_START  (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_MWDog0_rstreq_intr_END    (22)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_PWM0_START                (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_PWM0_END                  (23)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_PWM1_START                (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_PWM1_END                  (24)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_PWMIN_START               (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_PWMIN_END                 (25)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_31reserved_START          (26)
#define SOC_AO_SCTRL_SC_MCU_WKUP_INT_STATM1_mcu_wakeup_int_mask1_31reserved_END            (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_STATR_UNION
 结构说明  : SC_INT_STATR 寄存器结构定义。地址偏移量:0x0C4，初值:0x00000000，宽度:32
 寄存器说明: 原始内部中断寄存器，用于指示SC内部的原始中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_statr_0awd           : 1;  /* bit[0] : [0]： ACPU Watchdog复位请求中断，送给MCU。 */
        unsigned int  sc_int_statr_1cm3wdg0       : 1;  /* bit[1] : [1]： CM3 Watchdog0复位请求中断，送给ACPU。 */
        unsigned int  sc_int_statr_2hifiwd        : 1;  /* bit[2] : [2]： HIFI Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_statr_3bakwd         : 1;  /* bit[3] : [3]： 备份Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_statr_4acpu_sft_fiq  : 1;  /* bit[4] : [4]： acpu fiq int，送给acpu，用于调试，由acpu_sft_fiq_req配置产生。 */
        unsigned int  sc_int_statr_5vote0mcuei    : 1;  /* bit[5] : [5]： 投票寄存器0允许中断，送给MCU。 */
        unsigned int  sc_int_statr_6vote0mcudi    : 1;  /* bit[6] : [6]： 投票寄存器0不允许中断，送给MCU。 */
        unsigned int  sc_int_statr_7vote1mcuei    : 1;  /* bit[7] : [7]： 投票寄存器1允许中断，送给MCU。 */
        unsigned int  sc_int_statr_8vote1mcudi    : 1;  /* bit[8] : [8]： 投票寄存器1不允许中断，送给MCU。 */
        unsigned int  sc_int_statr_9cei           : 1;  /* bit[9] : [9]： CCPU 睡眠投票允许中断，送给CCPU。 */
        unsigned int  sc_int_statr_10cdi          : 1;  /* bit[10]: [10]： CCPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_statr_11aei          : 1;  /* bit[11]: [11]： ACPU 睡眠投票允许中断，送给ACPU。 */
        unsigned int  sc_int_statr_12adi          : 1;  /* bit[12]: [12]： ACPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_statr_13vote2mcudi   : 1;  /* bit[13]: [13]： 投票寄存器2允许中断，送给MCU。 */
        unsigned int  sc_int_statr_14vote0mcuei   : 1;  /* bit[14]: [14]： 投票寄存器2不允许中断，送给MCU。 */
        unsigned int  sc_int_statr_15periei       : 1;  /* bit[15]: [15]： PERI 睡眠投票允许中断，送给MCU。 */
        unsigned int  sc_int_statr_16peridi       : 1;  /* bit[16]: [16]： PERI 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_statr_17vote0aarmei  : 1;  /* bit[17]: [17]： 投票寄存器0允许中断，送给ACPU。 */
        unsigned int  sc_int_statr_18vote0aarmdi  : 1;  /* bit[18]: [18]： 投票寄存器0不允许中断，送给ACPU。 */
        unsigned int  sc_int_statr_19vote1aarmei  : 1;  /* bit[19]: [19]： 投票寄存器1允许中断，送给ACPU。 */
        unsigned int  sc_int_statr_20vote1aarmdi  : 1;  /* bit[20]: [20]： 投票寄存器1不允许中断，送给ACPU。 */
        unsigned int  sc_int_statr_21vote2aarmdi  : 1;  /* bit[21]: [21]： 投票寄存器2允许中断，送给ACPU。 */
        unsigned int  sc_int_statr_22vote0aarmei  : 1;  /* bit[22]: [22]： 投票寄存器2不允许中断，送给ACPU。 */
        unsigned int  sc_int_statr_23vote0carmei  : 1;  /* bit[23]: [23]： 投票寄存器0允许中断，送给CCPU。 */
        unsigned int  sc_int_statr_24vote0carmdi  : 1;  /* bit[24]: [24]： 投票寄存器0不允许中断，送给CCPU。 */
        unsigned int  sc_int_statr_25vote1carmei  : 1;  /* bit[25]: [25]： 投票寄存器1允许中断，送给CCPU。 */
        unsigned int  sc_int_statr_26vote1carmdi  : 1;  /* bit[26]: [26]： 投票寄存器1不允许中断，送给CCPU。 */
        unsigned int  sc_int_statr_27vote2hifidi  : 1;  /* bit[27]: [27]： 投票寄存器2允许中断，送给HiFi。 */
        unsigned int  sc_int_statr_28vote0hifiei  : 1;  /* bit[28]: [28]： 投票寄存器2不允许中断，送给HiFi。 */
        unsigned int  sc_int_statr_29otgchg       : 1;  /* bit[29]: [29]： 当USBOTG chgdet_sts中断，送给MCU和ACPU。 */
        unsigned int  sc_int_statr_30otgdat       : 1;  /* bit[30]: [30]： 当USBOTG datcondet_sts中断。送给MCU和ACPU。 */
        unsigned int  sc_int_statr_31ccpu_sft_fiq : 1;  /* bit[31]: [31]：ccpu fiq int，送给ccpu，用于调试，由ccpu_sft_fiq_req配置产生。 */
    } reg;
} SOC_AO_SCTRL_SC_INT_STATR_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_0awd_START            (0)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_0awd_END              (0)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_1cm3wdg0_START        (1)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_1cm3wdg0_END          (1)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_2hifiwd_START         (2)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_2hifiwd_END           (2)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_3bakwd_START          (3)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_3bakwd_END            (3)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_4acpu_sft_fiq_START   (4)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_4acpu_sft_fiq_END     (4)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_5vote0mcuei_START     (5)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_5vote0mcuei_END       (5)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_6vote0mcudi_START     (6)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_6vote0mcudi_END       (6)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_7vote1mcuei_START     (7)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_7vote1mcuei_END       (7)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_8vote1mcudi_START     (8)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_8vote1mcudi_END       (8)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_9cei_START            (9)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_9cei_END              (9)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_10cdi_START           (10)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_10cdi_END             (10)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_11aei_START           (11)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_11aei_END             (11)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_12adi_START           (12)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_12adi_END             (12)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_13vote2mcudi_START    (13)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_13vote2mcudi_END      (13)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_14vote0mcuei_START    (14)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_14vote0mcuei_END      (14)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_15periei_START        (15)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_15periei_END          (15)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_16peridi_START        (16)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_16peridi_END          (16)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_17vote0aarmei_START   (17)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_17vote0aarmei_END     (17)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_18vote0aarmdi_START   (18)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_18vote0aarmdi_END     (18)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_19vote1aarmei_START   (19)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_19vote1aarmei_END     (19)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_20vote1aarmdi_START   (20)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_20vote1aarmdi_END     (20)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_21vote2aarmdi_START   (21)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_21vote2aarmdi_END     (21)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_22vote0aarmei_START   (22)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_22vote0aarmei_END     (22)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_23vote0carmei_START   (23)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_23vote0carmei_END     (23)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_24vote0carmdi_START   (24)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_24vote0carmdi_END     (24)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_25vote1carmei_START   (25)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_25vote1carmei_END     (25)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_26vote1carmdi_START   (26)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_26vote1carmdi_END     (26)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_27vote2hifidi_START   (27)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_27vote2hifidi_END     (27)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_28vote0hifiei_START   (28)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_28vote0hifiei_END     (28)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_29otgchg_START        (29)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_29otgchg_END          (29)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_30otgdat_START        (30)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_30otgdat_END          (30)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_31ccpu_sft_fiq_START  (31)
#define SOC_AO_SCTRL_SC_INT_STATR_sc_int_statr_31ccpu_sft_fiq_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_STATM_UNION
 结构说明  : SC_INT_STATM 寄存器结构定义。地址偏移量:0x0C8，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后内部中断寄存器，用于指示SC当前内部中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_statm_0awd           : 1;  /* bit[0] : [0]： ACPU Watchdog复位请求中断，送给MCU。 */
        unsigned int  sc_int_statm_1cm3wdg0       : 1;  /* bit[1] : [1]： CM3 Watchdog0复位请求中断，送给ACPU。 */
        unsigned int  sc_int_statm_2hifiwd        : 1;  /* bit[2] : [2]： HIFI Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_statm_3bakwd         : 1;  /* bit[3] : [3]： 备份Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_statm_4acpu_sft_fiq  : 1;  /* bit[4] : [4]： acpu fiq int，送给acpu，用于调试，由acpu_sft_fiq_req配置产生。 */
        unsigned int  sc_int_statm_5vote0mcuei    : 1;  /* bit[5] : [5]： 投票寄存器0允许中断，送给MCU。 */
        unsigned int  sc_int_statm_6vote0mcudi    : 1;  /* bit[6] : [6]： 投票寄存器0不允许中断，送给MCU。 */
        unsigned int  sc_int_statm_7vote1mcuei    : 1;  /* bit[7] : [7]： 投票寄存器1允许中断，送给MCU。 */
        unsigned int  sc_int_statm_8vote1mcudi    : 1;  /* bit[8] : [8]： 投票寄存器1不允许中断，送给MCU。 */
        unsigned int  sc_int_statm_9cei           : 1;  /* bit[9] : [9]： CCPU 睡眠投票允许中断，送给CCPU。 */
        unsigned int  sc_int_statm_10cdi          : 1;  /* bit[10]: [10]： CCPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_statm_11aei          : 1;  /* bit[11]: [11]： ACPU 睡眠投票允许中断，送给ACPU。 */
        unsigned int  sc_int_statm_12adi          : 1;  /* bit[12]: [12]： ACPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_statm_13vote2mcudi   : 1;  /* bit[13]: [13]： 投票寄存器2允许中断，送给MCU。 */
        unsigned int  sc_int_statm_14vote0mcuei   : 1;  /* bit[14]: [14]： 投票寄存器2不允许中断，送给MCU。 */
        unsigned int  sc_int_statm_15periei       : 1;  /* bit[15]: [15]： PERI 睡眠投票允许中断，送给MCU。 */
        unsigned int  sc_int_statm_16peridi       : 1;  /* bit[16]: [16]： PERI 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_statm_17vote0aarmei  : 1;  /* bit[17]: [17]： 投票寄存器0允许中断，送给ACPU。 */
        unsigned int  sc_int_statm_18vote0aarmdi  : 1;  /* bit[18]: [18]： 投票寄存器0不允许中断，送给ACPU。 */
        unsigned int  sc_int_statm_19vote1aarmei  : 1;  /* bit[19]: [19]： 投票寄存器1允许中断，送给ACPU。 */
        unsigned int  sc_int_statm_20vote1aarmdi  : 1;  /* bit[20]: [20]： 投票寄存器1不允许中断，送给ACPU。 */
        unsigned int  sc_int_statm_21vote2aarmdi  : 1;  /* bit[21]: [21]： 投票寄存器2允许中断，送给ACPU。 */
        unsigned int  sc_int_statm_22vote0aarmei  : 1;  /* bit[22]: [22]： 投票寄存器2不允许中断，送给ACPU。 */
        unsigned int  sc_int_statm_23vote0carmei  : 1;  /* bit[23]: [23]： 投票寄存器0允许中断，送给CCPU。 */
        unsigned int  sc_int_statm_24vote0carmdi  : 1;  /* bit[24]: [24]： 投票寄存器0不允许中断，送给CCPU。 */
        unsigned int  sc_int_statm_25vote1carmei  : 1;  /* bit[25]: [25]： 投票寄存器1允许中断，送给CCPU。 */
        unsigned int  sc_int_statm_26vote1carmdi  : 1;  /* bit[26]: [26]： 投票寄存器1不允许中断，送给CCPU。 */
        unsigned int  sc_int_statm_27vote2hifidi  : 1;  /* bit[27]: [27]： 投票寄存器2允许中断，送给HiFi。 */
        unsigned int  sc_int_statm_28vote0hifiei  : 1;  /* bit[28]: [28]： 投票寄存器2不允许中断，送给HiFi。 */
        unsigned int  sc_int_statm_29otgchg       : 1;  /* bit[29]: [29]： 当USBOTG chgdet_sts中断，送给MCU和ACPU。 */
        unsigned int  sc_int_statm_30otgdat       : 1;  /* bit[30]: [30]： 当USBOTG datcondet_sts中断。送给MCU和ACPU。 */
        unsigned int  sc_int_statm_31ccpu_sft_fiq : 1;  /* bit[31]: [31]：ccpu fiq int，送给ccpu，用于调试，由ccpu_sft_fiq_req配置产生。 */
    } reg;
} SOC_AO_SCTRL_SC_INT_STATM_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_0awd_START            (0)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_0awd_END              (0)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_1cm3wdg0_START        (1)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_1cm3wdg0_END          (1)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_2hifiwd_START         (2)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_2hifiwd_END           (2)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_3bakwd_START          (3)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_3bakwd_END            (3)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_4acpu_sft_fiq_START   (4)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_4acpu_sft_fiq_END     (4)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_5vote0mcuei_START     (5)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_5vote0mcuei_END       (5)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_6vote0mcudi_START     (6)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_6vote0mcudi_END       (6)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_7vote1mcuei_START     (7)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_7vote1mcuei_END       (7)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_8vote1mcudi_START     (8)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_8vote1mcudi_END       (8)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_9cei_START            (9)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_9cei_END              (9)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_10cdi_START           (10)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_10cdi_END             (10)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_11aei_START           (11)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_11aei_END             (11)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_12adi_START           (12)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_12adi_END             (12)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_13vote2mcudi_START    (13)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_13vote2mcudi_END      (13)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_14vote0mcuei_START    (14)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_14vote0mcuei_END      (14)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_15periei_START        (15)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_15periei_END          (15)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_16peridi_START        (16)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_16peridi_END          (16)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_17vote0aarmei_START   (17)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_17vote0aarmei_END     (17)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_18vote0aarmdi_START   (18)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_18vote0aarmdi_END     (18)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_19vote1aarmei_START   (19)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_19vote1aarmei_END     (19)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_20vote1aarmdi_START   (20)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_20vote1aarmdi_END     (20)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_21vote2aarmdi_START   (21)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_21vote2aarmdi_END     (21)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_22vote0aarmei_START   (22)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_22vote0aarmei_END     (22)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_23vote0carmei_START   (23)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_23vote0carmei_END     (23)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_24vote0carmdi_START   (24)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_24vote0carmdi_END     (24)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_25vote1carmei_START   (25)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_25vote1carmei_END     (25)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_26vote1carmdi_START   (26)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_26vote1carmdi_END     (26)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_27vote2hifidi_START   (27)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_27vote2hifidi_END     (27)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_28vote0hifiei_START   (28)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_28vote0hifiei_END     (28)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_29otgchg_START        (29)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_29otgchg_END          (29)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_30otgdat_START        (30)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_30otgdat_END          (30)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_31ccpu_sft_fiq_START  (31)
#define SOC_AO_SCTRL_SC_INT_STATM_sc_int_statm_31ccpu_sft_fiq_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_CLEAR_UNION
 结构说明  : SC_INT_CLEAR 寄存器结构定义。地址偏移量:0x0CC，初值:0x00000000，宽度:32
 寄存器说明: 内部中断清除寄存器，给相应中断比特位写1，表示清除对应的中断状态，包括原始内部中断寄存器和屏蔽后内部中断寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_clear_0awd           : 1;  /* bit[0] : [0]： ACPU Watchdog复位请求中断，送给MCU。 */
        unsigned int  sc_int_clear_1cm3wdg0       : 1;  /* bit[1] : [1]： CM3 Watchdog0复位请求中断，送给ACPU。 */
        unsigned int  sc_int_clear_2hifiwd        : 1;  /* bit[2] : [2]： HIFI Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_clear_3bakwd         : 1;  /* bit[3] : [3]： 备份Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_clear_4acpu_sft_fiq  : 1;  /* bit[4] : [4]： acpu fiq int，送给acpu，用于调试，由acpu_sft_fiq_req配置产生。 */
        unsigned int  sc_int_clear_5vote0mcuei    : 1;  /* bit[5] : [5]： 投票寄存器0允许中断，送给MCU。 */
        unsigned int  sc_int_clear_6vote0mcudi    : 1;  /* bit[6] : [6]： 投票寄存器0不允许中断，送给MCU。 */
        unsigned int  sc_int_clear_7vote1mcuei    : 1;  /* bit[7] : [7]： 投票寄存器1允许中断，送给MCU。 */
        unsigned int  sc_int_clear_8vote1mcudi    : 1;  /* bit[8] : [8]： 投票寄存器1不允许中断，送给MCU。 */
        unsigned int  sc_int_clear_9cei           : 1;  /* bit[9] : [9]： CCPU 睡眠投票允许中断，送给CCPU。 */
        unsigned int  sc_int_clear_10cdi          : 1;  /* bit[10]: [10]： CCPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_clear_11aei          : 1;  /* bit[11]: [11]： ACPU 睡眠投票允许中断，送给ACPU。 */
        unsigned int  sc_int_clear_12adi          : 1;  /* bit[12]: [12]： ACPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_clear_13vote2mcudi   : 1;  /* bit[13]: [13]： 投票寄存器2允许中断，送给MCU。 */
        unsigned int  sc_int_clear_14vote0mcuei   : 1;  /* bit[14]: [14]： 投票寄存器2不允许中断，送给MCU。 */
        unsigned int  sc_int_clear_15periei       : 1;  /* bit[15]: [15]： PERI 睡眠投票允许中断，送给MCU。 */
        unsigned int  sc_int_clear_16peridi       : 1;  /* bit[16]: [16]： PERI 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_clear_17vote0aarmei  : 1;  /* bit[17]: [17]： 投票寄存器0允许中断，送给ACPU。 */
        unsigned int  sc_int_clear_18vote0aarmdi  : 1;  /* bit[18]: [18]： 投票寄存器0不允许中断，送给ACPU。 */
        unsigned int  sc_int_clear_19vote1aarmei  : 1;  /* bit[19]: [19]： 投票寄存器1允许中断，送给ACPU。 */
        unsigned int  sc_int_clear_20vote1aarmdi  : 1;  /* bit[20]: [20]： 投票寄存器1不允许中断，送给ACPU。 */
        unsigned int  sc_int_clear_21vote2aarmdi  : 1;  /* bit[21]: [21]： 投票寄存器2允许中断，送给ACPU。 */
        unsigned int  sc_int_clear_22vote0aarmei  : 1;  /* bit[22]: [22]： 投票寄存器2不允许中断，送给ACPU。 */
        unsigned int  sc_int_clear_23vote0carmei  : 1;  /* bit[23]: [23]： 投票寄存器0允许中断，送给CCPU。 */
        unsigned int  sc_int_clear_24vote0carmdi  : 1;  /* bit[24]: [24]： 投票寄存器0不允许中断，送给CCPU。 */
        unsigned int  sc_int_clear_25vote1carmei  : 1;  /* bit[25]: [25]： 投票寄存器1允许中断，送给CCPU。 */
        unsigned int  sc_int_clear_26vote1carmdi  : 1;  /* bit[26]: [26]： 投票寄存器1不允许中断，送给CCPU。 */
        unsigned int  sc_int_clear_27vote2hifidi  : 1;  /* bit[27]: [27]： 投票寄存器2允许中断，送给HiFi。 */
        unsigned int  sc_int_clear_28vote0hifiei  : 1;  /* bit[28]: [28]： 投票寄存器2不允许中断，送给HiFi。 */
        unsigned int  sc_int_clear_29otgchg       : 1;  /* bit[29]: [29]： 当USBOTG chgdet_sts中断，送给MCU和ACPU。 */
        unsigned int  sc_int_clear_30otgdat       : 1;  /* bit[30]: [30]： 当USBOTG datcondet_sts中断。送给MCU和ACPU。 */
        unsigned int  sc_int_clear_31ccpu_sft_fiq : 1;  /* bit[31]: [31]：ccpu fiq int，送给ccpu，用于调试，由ccpu_sft_fiq_req配置产生。 */
    } reg;
} SOC_AO_SCTRL_SC_INT_CLEAR_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_0awd_START            (0)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_0awd_END              (0)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_1cm3wdg0_START        (1)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_1cm3wdg0_END          (1)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_2hifiwd_START         (2)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_2hifiwd_END           (2)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_3bakwd_START          (3)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_3bakwd_END            (3)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_4acpu_sft_fiq_START   (4)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_4acpu_sft_fiq_END     (4)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_5vote0mcuei_START     (5)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_5vote0mcuei_END       (5)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_6vote0mcudi_START     (6)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_6vote0mcudi_END       (6)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_7vote1mcuei_START     (7)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_7vote1mcuei_END       (7)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_8vote1mcudi_START     (8)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_8vote1mcudi_END       (8)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_9cei_START            (9)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_9cei_END              (9)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_10cdi_START           (10)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_10cdi_END             (10)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_11aei_START           (11)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_11aei_END             (11)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_12adi_START           (12)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_12adi_END             (12)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_13vote2mcudi_START    (13)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_13vote2mcudi_END      (13)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_14vote0mcuei_START    (14)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_14vote0mcuei_END      (14)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_15periei_START        (15)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_15periei_END          (15)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_16peridi_START        (16)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_16peridi_END          (16)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_17vote0aarmei_START   (17)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_17vote0aarmei_END     (17)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_18vote0aarmdi_START   (18)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_18vote0aarmdi_END     (18)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_19vote1aarmei_START   (19)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_19vote1aarmei_END     (19)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_20vote1aarmdi_START   (20)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_20vote1aarmdi_END     (20)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_21vote2aarmdi_START   (21)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_21vote2aarmdi_END     (21)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_22vote0aarmei_START   (22)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_22vote0aarmei_END     (22)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_23vote0carmei_START   (23)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_23vote0carmei_END     (23)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_24vote0carmdi_START   (24)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_24vote0carmdi_END     (24)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_25vote1carmei_START   (25)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_25vote1carmei_END     (25)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_26vote1carmdi_START   (26)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_26vote1carmdi_END     (26)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_27vote2hifidi_START   (27)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_27vote2hifidi_END     (27)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_28vote0hifiei_START   (28)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_28vote0hifiei_END     (28)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_29otgchg_START        (29)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_29otgchg_END          (29)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_30otgdat_START        (30)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_30otgdat_END          (30)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_31ccpu_sft_fiq_START  (31)
#define SOC_AO_SCTRL_SC_INT_CLEAR_sc_int_clear_31ccpu_sft_fiq_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_EN_SET_UNION
 结构说明  : SC_INT_EN_SET 寄存器结构定义。地址偏移量:0x0D0，初值:0x00000000，宽度:32
 寄存器说明: 内部中断使能寄存器0，对比特写1，SC_INT_EN_STAT的对应bit置1；写0不影响屏蔽的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_en0_0awd           : 1;  /* bit[0] : [0]： ACPU Watchdog复位请求中断，送给MCU。 */
        unsigned int  sc_int_en0_1cm3wdg0       : 1;  /* bit[1] : [1]： CM3 Watchdog0复位请求中断，送给ACPU。 */
        unsigned int  sc_int_en0_2hifiwd        : 1;  /* bit[2] : [2]： HIFI Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_en0_3bakwd         : 1;  /* bit[3] : [3]： 备份Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_en0_4acpu_sft_fiq  : 1;  /* bit[4] : [4]： acpu fiq int，送给acpu，用于调试，由acpu_sft_fiq_req配置产生。 */
        unsigned int  sc_int_en0_5vote0mcuei    : 1;  /* bit[5] : [5]： 投票寄存器0允许中断，送给MCU。 */
        unsigned int  sc_int_en0_6vote0mcudi    : 1;  /* bit[6] : [6]： 投票寄存器0不允许中断，送给MCU。 */
        unsigned int  sc_int_en0_7vote1mcuei    : 1;  /* bit[7] : [7]： 投票寄存器1允许中断，送给MCU。 */
        unsigned int  sc_int_en0_8vote1mcudi    : 1;  /* bit[8] : [8]： 投票寄存器1不允许中断，送给MCU。 */
        unsigned int  sc_int_en0_9cei           : 1;  /* bit[9] : [9]： CCPU 睡眠投票允许中断，送给CCPU。 */
        unsigned int  sc_int_en0_10cdi          : 1;  /* bit[10]: [10]： CCPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_en0_11aei          : 1;  /* bit[11]: [11]： ACPU 睡眠投票允许中断，送给ACPU。 */
        unsigned int  sc_int_en0_12adi          : 1;  /* bit[12]: [12]： ACPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_en0_13vote2mcudi   : 1;  /* bit[13]: [13]： 投票寄存器2允许中断，送给MCU。 */
        unsigned int  sc_int_en0_14vote0mcuei   : 1;  /* bit[14]: [14]： 投票寄存器2不允许中断，送给MCU。 */
        unsigned int  sc_int_en0_15periei       : 1;  /* bit[15]: [15]： PERI 睡眠投票允许中断，送给MCU。 */
        unsigned int  sc_int_en0_16peridi       : 1;  /* bit[16]: [16]： PERI 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_en0_17vote0aarmei  : 1;  /* bit[17]: [17]： 投票寄存器0允许中断，送给ACPU。 */
        unsigned int  sc_int_en0_18vote0aarmdi  : 1;  /* bit[18]: [18]： 投票寄存器0不允许中断，送给ACPU。 */
        unsigned int  sc_int_en0_19vote1aarmei  : 1;  /* bit[19]: [19]： 投票寄存器1允许中断，送给ACPU。 */
        unsigned int  sc_int_en0_20vote1aarmdi  : 1;  /* bit[20]: [20]： 投票寄存器1不允许中断，送给ACPU。 */
        unsigned int  sc_int_en0_21vote2aarmdi  : 1;  /* bit[21]: [21]： 投票寄存器2允许中断，送给ACPU。 */
        unsigned int  sc_int_en0_22vote0aarmei  : 1;  /* bit[22]: [22]： 投票寄存器2不允许中断，送给ACPU。 */
        unsigned int  sc_int_en0_23vote0carmei  : 1;  /* bit[23]: [23]： 投票寄存器0允许中断，送给CCPU。 */
        unsigned int  sc_int_en0_24vote0carmdi  : 1;  /* bit[24]: [24]： 投票寄存器0不允许中断，送给CCPU。 */
        unsigned int  sc_int_en0_25vote1carmei  : 1;  /* bit[25]: [25]： 投票寄存器1允许中断，送给CCPU。 */
        unsigned int  sc_int_en0_26vote1carmdi  : 1;  /* bit[26]: [26]： 投票寄存器1不允许中断，送给CCPU。 */
        unsigned int  sc_int_en0_27vote2hifidi  : 1;  /* bit[27]: [27]： 投票寄存器2允许中断，送给HiFi。 */
        unsigned int  sc_int_en0_28vote0hifiei  : 1;  /* bit[28]: [28]： 投票寄存器2不允许中断，送给HiFi。 */
        unsigned int  sc_int_en0_29otgchg       : 1;  /* bit[29]: [29]： 当USBOTG chgdet_sts中断，送给MCU和ACPU。 */
        unsigned int  sc_int_en0_30otgdat       : 1;  /* bit[30]: [30]： 当USBOTG datcondet_sts中断。送给MCU和ACPU。 */
        unsigned int  sc_int_en0_31ccpu_sft_fiq : 1;  /* bit[31]: [31]：ccpu fiq int，送给ccpu，用于调试，由ccpu_sft_fiq_req配置产生。 */
    } reg;
} SOC_AO_SCTRL_SC_INT_EN_SET_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_0awd_START            (0)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_0awd_END              (0)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_1cm3wdg0_START        (1)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_1cm3wdg0_END          (1)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_2hifiwd_START         (2)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_2hifiwd_END           (2)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_3bakwd_START          (3)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_3bakwd_END            (3)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_4acpu_sft_fiq_START   (4)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_4acpu_sft_fiq_END     (4)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_5vote0mcuei_START     (5)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_5vote0mcuei_END       (5)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_6vote0mcudi_START     (6)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_6vote0mcudi_END       (6)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_7vote1mcuei_START     (7)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_7vote1mcuei_END       (7)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_8vote1mcudi_START     (8)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_8vote1mcudi_END       (8)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_9cei_START            (9)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_9cei_END              (9)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_10cdi_START           (10)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_10cdi_END             (10)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_11aei_START           (11)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_11aei_END             (11)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_12adi_START           (12)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_12adi_END             (12)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_13vote2mcudi_START    (13)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_13vote2mcudi_END      (13)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_14vote0mcuei_START    (14)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_14vote0mcuei_END      (14)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_15periei_START        (15)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_15periei_END          (15)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_16peridi_START        (16)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_16peridi_END          (16)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_17vote0aarmei_START   (17)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_17vote0aarmei_END     (17)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_18vote0aarmdi_START   (18)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_18vote0aarmdi_END     (18)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_19vote1aarmei_START   (19)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_19vote1aarmei_END     (19)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_20vote1aarmdi_START   (20)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_20vote1aarmdi_END     (20)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_21vote2aarmdi_START   (21)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_21vote2aarmdi_END     (21)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_22vote0aarmei_START   (22)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_22vote0aarmei_END     (22)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_23vote0carmei_START   (23)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_23vote0carmei_END     (23)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_24vote0carmdi_START   (24)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_24vote0carmdi_END     (24)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_25vote1carmei_START   (25)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_25vote1carmei_END     (25)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_26vote1carmdi_START   (26)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_26vote1carmdi_END     (26)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_27vote2hifidi_START   (27)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_27vote2hifidi_END     (27)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_28vote0hifiei_START   (28)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_28vote0hifiei_END     (28)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_29otgchg_START        (29)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_29otgchg_END          (29)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_30otgdat_START        (30)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_30otgdat_END          (30)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_31ccpu_sft_fiq_START  (31)
#define SOC_AO_SCTRL_SC_INT_EN_SET_sc_int_en0_31ccpu_sft_fiq_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_EN_DIS_UNION
 结构说明  : SC_INT_EN_DIS 寄存器结构定义。地址偏移量:0x0D4，初值:0x00000000，宽度:32
 寄存器说明: 内部中断屏蔽寄存器0，对比特写1，SC_INT_EN_STAT的对应bit清0；写0不影响时钟屏蔽的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_dis0_0awd           : 1;  /* bit[0] : [0]： ACPU Watchdog复位请求中断，送给MCU。 */
        unsigned int  sc_int_dis0_1cm3wdg0       : 1;  /* bit[1] : [1]： CM3 Watchdog0复位请求中断，送给ACPU。 */
        unsigned int  sc_int_dis0_2hifiwd        : 1;  /* bit[2] : [2]： HIFI Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_dis0_3bakwd         : 1;  /* bit[3] : [3]： 备份Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_dis0_4acpu_sft_fiq  : 1;  /* bit[4] : [4]： acpu fiq int，送给acpu，用于调试，由acpu_sft_fiq_req配置产生。 */
        unsigned int  sc_int_dis0_5vote0mcuei    : 1;  /* bit[5] : [5]： 投票寄存器0允许中断，送给MCU。 */
        unsigned int  sc_int_dis0_6vote0mcudi    : 1;  /* bit[6] : [6]： 投票寄存器0不允许中断，送给MCU。 */
        unsigned int  sc_int_dis0_7vote1mcuei    : 1;  /* bit[7] : [7]： 投票寄存器1允许中断，送给MCU。 */
        unsigned int  sc_int_dis0_8vote1mcudi    : 1;  /* bit[8] : [8]： 投票寄存器1不允许中断，送给MCU。 */
        unsigned int  sc_int_dis0_9cei           : 1;  /* bit[9] : [9]： CCPU 睡眠投票允许中断，送给CCPU。 */
        unsigned int  sc_int_dis0_10cdi          : 1;  /* bit[10]: [10]： CCPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_dis0_11aei          : 1;  /* bit[11]: [11]： ACPU 睡眠投票允许中断，送给ACPU。 */
        unsigned int  sc_int_dis0_12adi          : 1;  /* bit[12]: [12]： ACPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_dis0_13vote2mcudi   : 1;  /* bit[13]: [13]： 投票寄存器2允许中断，送给MCU。 */
        unsigned int  sc_int_dis0_14vote0mcuei   : 1;  /* bit[14]: [14]： 投票寄存器2不允许中断，送给MCU。 */
        unsigned int  sc_int_dis0_15periei       : 1;  /* bit[15]: [15]： PERI 睡眠投票允许中断，送给MCU。 */
        unsigned int  sc_int_dis0_16peridi       : 1;  /* bit[16]: [16]： PERI 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_dis0_17vote0aarmei  : 1;  /* bit[17]: [17]： 投票寄存器0允许中断，送给ACPU。 */
        unsigned int  sc_int_dis0_18vote0aarmdi  : 1;  /* bit[18]: [18]： 投票寄存器0不允许中断，送给ACPU。 */
        unsigned int  sc_int_dis0_19vote1aarmei  : 1;  /* bit[19]: [19]： 投票寄存器1允许中断，送给ACPU。 */
        unsigned int  sc_int_dis0_20vote1aarmdi  : 1;  /* bit[20]: [20]： 投票寄存器1不允许中断，送给ACPU。 */
        unsigned int  sc_int_dis0_21vote2aarmdi  : 1;  /* bit[21]: [21]： 投票寄存器2允许中断，送给ACPU。 */
        unsigned int  sc_int_dis0_22vote0aarmei  : 1;  /* bit[22]: [22]： 投票寄存器2不允许中断，送给ACPU。 */
        unsigned int  sc_int_dis0_23vote0carmei  : 1;  /* bit[23]: [23]： 投票寄存器0允许中断，送给CCPU。 */
        unsigned int  sc_int_dis0_24vote0carmdi  : 1;  /* bit[24]: [24]： 投票寄存器0不允许中断，送给CCPU。 */
        unsigned int  sc_int_dis0_25vote1carmei  : 1;  /* bit[25]: [25]： 投票寄存器1允许中断，送给CCPU。 */
        unsigned int  sc_int_dis0_26vote1carmdi  : 1;  /* bit[26]: [26]： 投票寄存器1不允许中断，送给CCPU。 */
        unsigned int  sc_int_dis0_27vote2hifidi  : 1;  /* bit[27]: [27]： 投票寄存器2允许中断，送给HiFi。 */
        unsigned int  sc_int_dis0_28vote0hifiei  : 1;  /* bit[28]: [28]： 投票寄存器2不允许中断，送给HiFi。 */
        unsigned int  sc_int_dis0_29otgchg       : 1;  /* bit[29]: [29]： 当USBOTG chgdet_sts中断，送给MCU和ACPU。 */
        unsigned int  sc_int_dis0_30otgdat       : 1;  /* bit[30]: [30]： 当USBOTG datcondet_sts中断。送给MCU和ACPU。 */
        unsigned int  sc_int_dis0_31ccpu_sft_fiq : 1;  /* bit[31]: [31]：ccpu fiq int，送给ccpu，用于调试，由ccpu_sft_fiq_req配置产生。 */
    } reg;
} SOC_AO_SCTRL_SC_INT_EN_DIS_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_0awd_START            (0)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_0awd_END              (0)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_1cm3wdg0_START        (1)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_1cm3wdg0_END          (1)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_2hifiwd_START         (2)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_2hifiwd_END           (2)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_3bakwd_START          (3)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_3bakwd_END            (3)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_4acpu_sft_fiq_START   (4)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_4acpu_sft_fiq_END     (4)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_5vote0mcuei_START     (5)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_5vote0mcuei_END       (5)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_6vote0mcudi_START     (6)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_6vote0mcudi_END       (6)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_7vote1mcuei_START     (7)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_7vote1mcuei_END       (7)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_8vote1mcudi_START     (8)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_8vote1mcudi_END       (8)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_9cei_START            (9)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_9cei_END              (9)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_10cdi_START           (10)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_10cdi_END             (10)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_11aei_START           (11)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_11aei_END             (11)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_12adi_START           (12)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_12adi_END             (12)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_13vote2mcudi_START    (13)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_13vote2mcudi_END      (13)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_14vote0mcuei_START    (14)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_14vote0mcuei_END      (14)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_15periei_START        (15)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_15periei_END          (15)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_16peridi_START        (16)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_16peridi_END          (16)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_17vote0aarmei_START   (17)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_17vote0aarmei_END     (17)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_18vote0aarmdi_START   (18)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_18vote0aarmdi_END     (18)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_19vote1aarmei_START   (19)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_19vote1aarmei_END     (19)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_20vote1aarmdi_START   (20)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_20vote1aarmdi_END     (20)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_21vote2aarmdi_START   (21)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_21vote2aarmdi_END     (21)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_22vote0aarmei_START   (22)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_22vote0aarmei_END     (22)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_23vote0carmei_START   (23)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_23vote0carmei_END     (23)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_24vote0carmdi_START   (24)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_24vote0carmdi_END     (24)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_25vote1carmei_START   (25)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_25vote1carmei_END     (25)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_26vote1carmdi_START   (26)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_26vote1carmdi_END     (26)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_27vote2hifidi_START   (27)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_27vote2hifidi_END     (27)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_28vote0hifiei_START   (28)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_28vote0hifiei_END     (28)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_29otgchg_START        (29)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_29otgchg_END          (29)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_30otgdat_START        (30)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_30otgdat_END          (30)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_31ccpu_sft_fiq_START  (31)
#define SOC_AO_SCTRL_SC_INT_EN_DIS_sc_int_dis0_31ccpu_sft_fiq_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_EN_STAT_UNION
 结构说明  : SC_INT_EN_STAT 寄存器结构定义。地址偏移量:0x0D8，初值:0x00000000，宽度:32
 寄存器说明: 内部中断使能状态寄存器，通过配置SC_INT_EN_SET0和SC_INT_EN_DIS0修改状态，用于SC内部中断的使能和屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_en_sta_0awd           : 1;  /* bit[0] : [0]： ACPU Watchdog复位请求中断，送给MCU。 */
        unsigned int  sc_int_en_sta_1cm3wdg0       : 1;  /* bit[1] : [1]： CM3 Watchdog0复位请求中断，送给ACPU。 */
        unsigned int  sc_int_en_sta_2hifiwd        : 1;  /* bit[2] : [2]： HIFI Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_en_sta_3bakwd         : 1;  /* bit[3] : [3]： 备份Watchdog复位请求中断，送给MCU和ACPU。 */
        unsigned int  sc_int_en_sta_4acpu_sft_fiq  : 1;  /* bit[4] : [4]： acpu fiq int，送给acpu，用于调试，由acpu_sft_fiq_req配置产生。 */
        unsigned int  sc_int_en_sta_5vote0mcuei    : 1;  /* bit[5] : [5]： 投票寄存器0允许中断，送给MCU。 */
        unsigned int  sc_int_en_sta_6vote0mcudi    : 1;  /* bit[6] : [6]： 投票寄存器0不允许中断，送给MCU。 */
        unsigned int  sc_int_en_sta_7vote1mcuei    : 1;  /* bit[7] : [7]： 投票寄存器1允许中断，送给MCU。 */
        unsigned int  sc_int_en_sta_8vote1mcudi    : 1;  /* bit[8] : [8]： 投票寄存器1不允许中断，送给MCU。 */
        unsigned int  sc_int_en_sta_9cei           : 1;  /* bit[9] : [9]： CCPU 睡眠投票允许中断，送给CCPU。 */
        unsigned int  sc_int_en_sta_10cdi          : 1;  /* bit[10]: [10]： CCPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_en_sta_11aei          : 1;  /* bit[11]: [11]： ACPU 睡眠投票允许中断，送给ACPU。 */
        unsigned int  sc_int_en_sta_12adi          : 1;  /* bit[12]: [12]： ACPU 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_en_sta_13vote2mcudi   : 1;  /* bit[13]: [13]： 投票寄存器2允许中断，送给MCU。 */
        unsigned int  sc_int_en_sta_14vote0mcuei   : 1;  /* bit[14]: [14]： 投票寄存器2不允许中断，送给MCU。 */
        unsigned int  sc_int_en_sta_15periei       : 1;  /* bit[15]: [15]： PERI 睡眠投票允许中断，送给MCU。 */
        unsigned int  sc_int_en_sta_16peridi       : 1;  /* bit[16]: [16]： PERI 睡眠投票不允许，即唤醒中断，送给MCU。 */
        unsigned int  sc_int_en_sta_17vote0aarmei  : 1;  /* bit[17]: [17]： 投票寄存器0允许中断，送给ACPU。 */
        unsigned int  sc_int_en_sta_18vote0aarmdi  : 1;  /* bit[18]: [18]： 投票寄存器0不允许中断，送给ACPU。 */
        unsigned int  sc_int_en_sta_19vote1aarmei  : 1;  /* bit[19]: [19]： 投票寄存器1允许中断，送给ACPU。 */
        unsigned int  sc_int_en_sta_20vote1aarmdi  : 1;  /* bit[20]: [20]： 投票寄存器1不允许中断，送给ACPU。 */
        unsigned int  sc_int_en_sta_21vote2aarmdi  : 1;  /* bit[21]: [21]： 投票寄存器2允许中断，送给ACPU。 */
        unsigned int  sc_int_en_sta_22vote0aarmei  : 1;  /* bit[22]: [22]： 投票寄存器2不允许中断，送给ACPU。 */
        unsigned int  sc_int_en_sta_23vote0carmei  : 1;  /* bit[23]: [23]： 投票寄存器0允许中断，送给CCPU。 */
        unsigned int  sc_int_en_sta_24vote0carmdi  : 1;  /* bit[24]: [24]： 投票寄存器0不允许中断，送给CCPU。 */
        unsigned int  sc_int_en_sta_25vote1carmei  : 1;  /* bit[25]: [25]： 投票寄存器1允许中断，送给CCPU。 */
        unsigned int  sc_int_en_sta_26vote1carmdi  : 1;  /* bit[26]: [26]： 投票寄存器1不允许中断，送给CCPU。 */
        unsigned int  sc_int_en_sta_27vote2hifidi  : 1;  /* bit[27]: [27]： 投票寄存器2允许中断，送给HiFi。 */
        unsigned int  sc_int_en_sta_28vote0hifiei  : 1;  /* bit[28]: [28]： 投票寄存器2不允许中断，送给HiFi。 */
        unsigned int  sc_int_en_sta_29otgchg       : 1;  /* bit[29]: [29]： 当USBOTG chgdet_sts中断，送给MCU和ACPU。 */
        unsigned int  sc_int_en_sta_30otgdat       : 1;  /* bit[30]: [30]： 当USBOTG datcondet_sts中断。送给MCU和ACPU。 */
        unsigned int  sc_int_en_sta_31ccpu_sft_fiq : 1;  /* bit[31]: [31]：ccpu fiq int，送给ccpu，用于调试，由ccpu_sft_fiq_req配置产生。 */
    } reg;
} SOC_AO_SCTRL_SC_INT_EN_STAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_0awd_START            (0)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_0awd_END              (0)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_1cm3wdg0_START        (1)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_1cm3wdg0_END          (1)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_2hifiwd_START         (2)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_2hifiwd_END           (2)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_3bakwd_START          (3)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_3bakwd_END            (3)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_4acpu_sft_fiq_START   (4)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_4acpu_sft_fiq_END     (4)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_5vote0mcuei_START     (5)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_5vote0mcuei_END       (5)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_6vote0mcudi_START     (6)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_6vote0mcudi_END       (6)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_7vote1mcuei_START     (7)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_7vote1mcuei_END       (7)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_8vote1mcudi_START     (8)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_8vote1mcudi_END       (8)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_9cei_START            (9)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_9cei_END              (9)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_10cdi_START           (10)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_10cdi_END             (10)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_11aei_START           (11)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_11aei_END             (11)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_12adi_START           (12)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_12adi_END             (12)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_13vote2mcudi_START    (13)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_13vote2mcudi_END      (13)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_14vote0mcuei_START    (14)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_14vote0mcuei_END      (14)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_15periei_START        (15)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_15periei_END          (15)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_16peridi_START        (16)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_16peridi_END          (16)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_17vote0aarmei_START   (17)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_17vote0aarmei_END     (17)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_18vote0aarmdi_START   (18)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_18vote0aarmdi_END     (18)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_19vote1aarmei_START   (19)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_19vote1aarmei_END     (19)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_20vote1aarmdi_START   (20)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_20vote1aarmdi_END     (20)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_21vote2aarmdi_START   (21)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_21vote2aarmdi_END     (21)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_22vote0aarmei_START   (22)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_22vote0aarmei_END     (22)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_23vote0carmei_START   (23)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_23vote0carmei_END     (23)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_24vote0carmdi_START   (24)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_24vote0carmdi_END     (24)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_25vote1carmei_START   (25)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_25vote1carmei_END     (25)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_26vote1carmdi_START   (26)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_26vote1carmdi_END     (26)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_27vote2hifidi_START   (27)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_27vote2hifidi_END     (27)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_28vote0hifiei_START   (28)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_28vote0hifiei_END     (28)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_29otgchg_START        (29)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_29otgchg_END          (29)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_30otgdat_START        (30)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_30otgdat_END          (30)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_31ccpu_sft_fiq_START  (31)
#define SOC_AO_SCTRL_SC_INT_EN_STAT_sc_int_en_sta_31ccpu_sft_fiq_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_STATR1_UNION
 结构说明  : SC_INT_STATR1 寄存器结构定义。地址偏移量:0x0E4，初值:0x00000000，宽度:32
 寄存器说明: 原始内部中断寄存器，用于指示SC内部的原始中断状态（屏蔽前）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_statr_0usim0_detect_int : 1;  /* bit[0] : [0]： USIM0 插入拔出检测中断。 */
        unsigned int  sc_int_statr_1usim1_detect_int : 1;  /* bit[1] : [1]： USIM1 插入拔出检测中断。 */
        unsigned int  sc_int_statr_2reserved         : 1;  /* bit[2] : [2]： reserved */
        unsigned int  sc_int_statr_3reserved         : 1;  /* bit[3] : [3]： reserved */
        unsigned int  sc_int_statr_4reserved         : 1;  /* bit[4] : [4]： reserved */
        unsigned int  sc_int_statr_5reserved         : 1;  /* bit[5] : [5]： reserved */
        unsigned int  sc_int_statr_6reserved         : 1;  /* bit[6] : [6]： reserved */
        unsigned int  sc_int_statr_7reserved         : 1;  /* bit[7] : [7]： reserved */
        unsigned int  sc_int_statr_8reserved         : 1;  /* bit[8] : [8]： reserved */
        unsigned int  sc_int_statr_9reserved         : 1;  /* bit[9] : [9]： reserved */
        unsigned int  sc_int_statr_10reserved        : 1;  /* bit[10]: [10]： reserved */
        unsigned int  sc_int_statr_11reserved        : 1;  /* bit[11]: [11]： reserved */
        unsigned int  sc_int_statr_12reserved        : 1;  /* bit[12]: [12]： reserved */
        unsigned int  sc_int_statr_13reserved        : 1;  /* bit[13]: [13]： reserved */
        unsigned int  sc_int_statr_14reserved        : 1;  /* bit[14]: [14]： reserved */
        unsigned int  sc_int_statr_15reserved        : 1;  /* bit[15]: [15]： reserved */
        unsigned int  sc_int_statr_16reserved        : 1;  /* bit[16]: [16]： reserved */
        unsigned int  sc_int_statr_17reserved        : 1;  /* bit[17]: [17]： reserved */
        unsigned int  sc_int_statr_18reserved        : 1;  /* bit[18]: [18]： reserved */
        unsigned int  sc_int_statr_19reserved        : 1;  /* bit[19]: [19]： reserved */
        unsigned int  sc_int_statr_20reserved        : 1;  /* bit[20]: [20]： reserved */
        unsigned int  sc_int_statr_21reserved        : 1;  /* bit[21]: [21]： reserved */
        unsigned int  sc_int_statr_22reserved        : 1;  /* bit[22]: [22]： reserved */
        unsigned int  sc_int_statr_23reserved        : 1;  /* bit[23]: [23]： reserved */
        unsigned int  sc_int_statr_24reserved        : 1;  /* bit[24]: [24]： reserved */
        unsigned int  sc_int_statr_25reserved        : 1;  /* bit[25]: [25]： reserved */
        unsigned int  sc_int_statr_26reserved        : 1;  /* bit[26]: [26]： reserved */
        unsigned int  sc_int_statr_27reserved        : 1;  /* bit[27]: [27]： reserved */
        unsigned int  sc_int_statr_28reserved        : 1;  /* bit[28]: [28]： reserved */
        unsigned int  sc_int_statr_29reserved        : 1;  /* bit[29]: [29]： reserved */
        unsigned int  sc_int_statr_30reserved        : 1;  /* bit[30]: [30]： reserved */
        unsigned int  sc_int_statr_31reserved        : 1;  /* bit[31]: [31]：reserved */
    } reg;
} SOC_AO_SCTRL_SC_INT_STATR1_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_0usim0_detect_int_START  (0)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_0usim0_detect_int_END    (0)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_1usim1_detect_int_START  (1)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_1usim1_detect_int_END    (1)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_2reserved_START          (2)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_2reserved_END            (2)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_3reserved_START          (3)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_3reserved_END            (3)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_4reserved_START          (4)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_4reserved_END            (4)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_5reserved_START          (5)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_5reserved_END            (5)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_6reserved_START          (6)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_6reserved_END            (6)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_7reserved_START          (7)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_7reserved_END            (7)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_8reserved_START          (8)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_8reserved_END            (8)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_9reserved_START          (9)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_9reserved_END            (9)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_10reserved_START         (10)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_10reserved_END           (10)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_11reserved_START         (11)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_11reserved_END           (11)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_12reserved_START         (12)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_12reserved_END           (12)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_13reserved_START         (13)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_13reserved_END           (13)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_14reserved_START         (14)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_14reserved_END           (14)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_15reserved_START         (15)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_15reserved_END           (15)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_16reserved_START         (16)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_16reserved_END           (16)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_17reserved_START         (17)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_17reserved_END           (17)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_18reserved_START         (18)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_18reserved_END           (18)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_19reserved_START         (19)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_19reserved_END           (19)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_20reserved_START         (20)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_20reserved_END           (20)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_21reserved_START         (21)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_21reserved_END           (21)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_22reserved_START         (22)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_22reserved_END           (22)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_23reserved_START         (23)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_23reserved_END           (23)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_24reserved_START         (24)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_24reserved_END           (24)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_25reserved_START         (25)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_25reserved_END           (25)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_26reserved_START         (26)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_26reserved_END           (26)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_27reserved_START         (27)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_27reserved_END           (27)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_28reserved_START         (28)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_28reserved_END           (28)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_29reserved_START         (29)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_29reserved_END           (29)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_30reserved_START         (30)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_30reserved_END           (30)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_31reserved_START         (31)
#define SOC_AO_SCTRL_SC_INT_STATR1_sc_int_statr_31reserved_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_STATM1_UNION
 结构说明  : SC_INT_STATM1 寄存器结构定义。地址偏移量:0x0E8，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后内部中断寄存器，用于指示SC当前内部中断状态（屏蔽后）。对应bit为1表示有中断，为0表示无中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_statm_0usim0_detect_int : 1;  /* bit[0] : [0]： USIM0 插入拔出检测中断。 */
        unsigned int  sc_int_statm_1usim1_detect_int : 1;  /* bit[1] : [1]： USIM1 插入拔出检测中断。 */
        unsigned int  sc_int_statm_2reserved         : 1;  /* bit[2] : [2]： reserved */
        unsigned int  sc_int_statm_3reserved         : 1;  /* bit[3] : [3]： reserved */
        unsigned int  sc_int_statm_4reserved         : 1;  /* bit[4] : [4]： reserved */
        unsigned int  sc_int_statm_5reserved         : 1;  /* bit[5] : [5]： reserved */
        unsigned int  sc_int_statm_6reserved         : 1;  /* bit[6] : [6]： reserved */
        unsigned int  sc_int_statm_7reserved         : 1;  /* bit[7] : [7]： reserved */
        unsigned int  sc_int_statm_8reserved         : 1;  /* bit[8] : [8]： reserved */
        unsigned int  sc_int_statm_9reserved         : 1;  /* bit[9] : [9]： reserved */
        unsigned int  sc_int_statm_10reserved        : 1;  /* bit[10]: [10]： reserved */
        unsigned int  sc_int_statm_11reserved        : 1;  /* bit[11]: [11]： reserved */
        unsigned int  sc_int_statm_12reserved        : 1;  /* bit[12]: [12]： reserved */
        unsigned int  sc_int_statm_13reserved        : 1;  /* bit[13]: [13]： reserved */
        unsigned int  sc_int_statm_14reserved        : 1;  /* bit[14]: [14]： reserved */
        unsigned int  sc_int_statm_15reserved        : 1;  /* bit[15]: [15]： reserved */
        unsigned int  sc_int_statm_16reserved        : 1;  /* bit[16]: [16]： reserved */
        unsigned int  sc_int_statm_17reserved        : 1;  /* bit[17]: [17]： reserved */
        unsigned int  sc_int_statm_18reserved        : 1;  /* bit[18]: [18]： reserved */
        unsigned int  sc_int_statm_19reserved        : 1;  /* bit[19]: [19]： reserved */
        unsigned int  sc_int_statm_20reserved        : 1;  /* bit[20]: [20]： reserved */
        unsigned int  sc_int_statm_21reserved        : 1;  /* bit[21]: [21]： reserved */
        unsigned int  sc_int_statm_22reserved        : 1;  /* bit[22]: [22]： reserved */
        unsigned int  sc_int_statm_23reserved        : 1;  /* bit[23]: [23]： reserved */
        unsigned int  sc_int_statm_24reserved        : 1;  /* bit[24]: [24]： reserved */
        unsigned int  sc_int_statm_25reserved        : 1;  /* bit[25]: [25]： reserved */
        unsigned int  sc_int_statm_26reserved        : 1;  /* bit[26]: [26]： reserved */
        unsigned int  sc_int_statm_27reserved        : 1;  /* bit[27]: [27]： reserved */
        unsigned int  sc_int_statm_28reserved        : 1;  /* bit[28]: [28]： reserved */
        unsigned int  sc_int_statm_29reserved        : 1;  /* bit[29]: [29]： reserved */
        unsigned int  sc_int_statm_30reserved        : 1;  /* bit[30]: [30]： reserved */
        unsigned int  sc_int_statm_31reserved        : 1;  /* bit[31]: [31]：reserved */
    } reg;
} SOC_AO_SCTRL_SC_INT_STATM1_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_0usim0_detect_int_START  (0)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_0usim0_detect_int_END    (0)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_1usim1_detect_int_START  (1)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_1usim1_detect_int_END    (1)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_2reserved_START          (2)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_2reserved_END            (2)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_3reserved_START          (3)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_3reserved_END            (3)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_4reserved_START          (4)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_4reserved_END            (4)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_5reserved_START          (5)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_5reserved_END            (5)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_6reserved_START          (6)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_6reserved_END            (6)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_7reserved_START          (7)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_7reserved_END            (7)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_8reserved_START          (8)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_8reserved_END            (8)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_9reserved_START          (9)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_9reserved_END            (9)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_10reserved_START         (10)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_10reserved_END           (10)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_11reserved_START         (11)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_11reserved_END           (11)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_12reserved_START         (12)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_12reserved_END           (12)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_13reserved_START         (13)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_13reserved_END           (13)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_14reserved_START         (14)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_14reserved_END           (14)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_15reserved_START         (15)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_15reserved_END           (15)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_16reserved_START         (16)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_16reserved_END           (16)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_17reserved_START         (17)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_17reserved_END           (17)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_18reserved_START         (18)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_18reserved_END           (18)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_19reserved_START         (19)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_19reserved_END           (19)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_20reserved_START         (20)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_20reserved_END           (20)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_21reserved_START         (21)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_21reserved_END           (21)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_22reserved_START         (22)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_22reserved_END           (22)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_23reserved_START         (23)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_23reserved_END           (23)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_24reserved_START         (24)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_24reserved_END           (24)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_25reserved_START         (25)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_25reserved_END           (25)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_26reserved_START         (26)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_26reserved_END           (26)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_27reserved_START         (27)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_27reserved_END           (27)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_28reserved_START         (28)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_28reserved_END           (28)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_29reserved_START         (29)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_29reserved_END           (29)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_30reserved_START         (30)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_30reserved_END           (30)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_31reserved_START         (31)
#define SOC_AO_SCTRL_SC_INT_STATM1_sc_int_statm_31reserved_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_CLEAR1_UNION
 结构说明  : SC_INT_CLEAR1 寄存器结构定义。地址偏移量:0x0EC，初值:0x00000000，宽度:32
 寄存器说明: 内部中断清除寄存器，给相应中断比特位写1，表示清除对应的中断状态，包括原始内部中断寄存器和屏蔽后内部中断寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_clear_0usim0_detect_int : 1;  /* bit[0] : [0]： USIM0 插入拔出检测中断。 */
        unsigned int  sc_int_clear_1usim1_detect_int : 1;  /* bit[1] : [1]： USIM1 插入拔出检测中断。 */
        unsigned int  sc_int_clear_2reserved         : 1;  /* bit[2] : [2]： reserved */
        unsigned int  sc_int_clear_3reserved         : 1;  /* bit[3] : [3]： reserved */
        unsigned int  sc_int_clear_4reserved         : 1;  /* bit[4] : [4]： reserved */
        unsigned int  sc_int_clear_5reserved         : 1;  /* bit[5] : [5]： reserved */
        unsigned int  sc_int_clear_6reserved         : 1;  /* bit[6] : [6]： reserved */
        unsigned int  sc_int_clear_7reserved         : 1;  /* bit[7] : [7]： reserved */
        unsigned int  sc_int_clear_8reserved         : 1;  /* bit[8] : [8]： reserved */
        unsigned int  sc_int_clear_9reserved         : 1;  /* bit[9] : [9]： reserved */
        unsigned int  sc_int_clear_10reserved        : 1;  /* bit[10]: [10]： reserved */
        unsigned int  sc_int_clear_11reserved        : 1;  /* bit[11]: [11]： reserved */
        unsigned int  sc_int_clear_12reserved        : 1;  /* bit[12]: [12]： reserved */
        unsigned int  sc_int_clear_13reserved        : 1;  /* bit[13]: [13]： reserved */
        unsigned int  sc_int_clear_14reserved        : 1;  /* bit[14]: [14]： reserved */
        unsigned int  sc_int_clear_15reserved        : 1;  /* bit[15]: [15]： reserved */
        unsigned int  sc_int_clear_16reserved        : 1;  /* bit[16]: [16]： reserved */
        unsigned int  sc_int_clear_17reserved        : 1;  /* bit[17]: [17]： reserved */
        unsigned int  sc_int_clear_18reserved        : 1;  /* bit[18]: [18]： reserved */
        unsigned int  sc_int_clear_19reserved        : 1;  /* bit[19]: [19]： reserved */
        unsigned int  sc_int_clear_20reserved        : 1;  /* bit[20]: [20]： reserved */
        unsigned int  sc_int_clear_21reserved        : 1;  /* bit[21]: [21]： reserved */
        unsigned int  sc_int_clear_22reserved        : 1;  /* bit[22]: [22]： reserved */
        unsigned int  sc_int_clear_23reserved        : 1;  /* bit[23]: [23]： reserved */
        unsigned int  sc_int_clear_24reserved        : 1;  /* bit[24]: [24]： reserved */
        unsigned int  sc_int_clear_25reserved        : 1;  /* bit[25]: [25]： reserved */
        unsigned int  sc_int_clear_26reserved        : 1;  /* bit[26]: [26]： reserved */
        unsigned int  sc_int_clear_27reserved        : 1;  /* bit[27]: [27]： reserved */
        unsigned int  sc_int_clear_28reserved        : 1;  /* bit[28]: [28]： reserved */
        unsigned int  sc_int_clear_29reserved        : 1;  /* bit[29]: [29]： reserved */
        unsigned int  sc_int_clear_30reserved        : 1;  /* bit[30]: [30]： reserved */
        unsigned int  sc_int_clear_31reserved        : 1;  /* bit[31]: [31]：reserved */
    } reg;
} SOC_AO_SCTRL_SC_INT_CLEAR1_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_0usim0_detect_int_START  (0)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_0usim0_detect_int_END    (0)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_1usim1_detect_int_START  (1)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_1usim1_detect_int_END    (1)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_2reserved_START          (2)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_2reserved_END            (2)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_3reserved_START          (3)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_3reserved_END            (3)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_4reserved_START          (4)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_4reserved_END            (4)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_5reserved_START          (5)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_5reserved_END            (5)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_6reserved_START          (6)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_6reserved_END            (6)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_7reserved_START          (7)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_7reserved_END            (7)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_8reserved_START          (8)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_8reserved_END            (8)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_9reserved_START          (9)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_9reserved_END            (9)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_10reserved_START         (10)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_10reserved_END           (10)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_11reserved_START         (11)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_11reserved_END           (11)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_12reserved_START         (12)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_12reserved_END           (12)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_13reserved_START         (13)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_13reserved_END           (13)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_14reserved_START         (14)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_14reserved_END           (14)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_15reserved_START         (15)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_15reserved_END           (15)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_16reserved_START         (16)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_16reserved_END           (16)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_17reserved_START         (17)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_17reserved_END           (17)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_18reserved_START         (18)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_18reserved_END           (18)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_19reserved_START         (19)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_19reserved_END           (19)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_20reserved_START         (20)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_20reserved_END           (20)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_21reserved_START         (21)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_21reserved_END           (21)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_22reserved_START         (22)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_22reserved_END           (22)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_23reserved_START         (23)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_23reserved_END           (23)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_24reserved_START         (24)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_24reserved_END           (24)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_25reserved_START         (25)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_25reserved_END           (25)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_26reserved_START         (26)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_26reserved_END           (26)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_27reserved_START         (27)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_27reserved_END           (27)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_28reserved_START         (28)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_28reserved_END           (28)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_29reserved_START         (29)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_29reserved_END           (29)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_30reserved_START         (30)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_30reserved_END           (30)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_31reserved_START         (31)
#define SOC_AO_SCTRL_SC_INT_CLEAR1_sc_int_clear_31reserved_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_EN_SET1_UNION
 结构说明  : SC_INT_EN_SET1 寄存器结构定义。地址偏移量:0x0F0，初值:0x00000000，宽度:32
 寄存器说明: 内部中断使能寄存器0，对比特写1，SC_INT_EN_STAT1的对应bit置1；写0不影响屏蔽的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_en0_0usim0_detect_int : 1;  /* bit[0] : [0]： USIM0 插入拔出检测中断。 */
        unsigned int  sc_int_en0_1usim1_detect_int : 1;  /* bit[1] : [1]： USIM1 插入拔出检测中断。 */
        unsigned int  sc_int_en0_2reserved         : 1;  /* bit[2] : [2]： reserved */
        unsigned int  sc_int_en0_3reserved         : 1;  /* bit[3] : [3]： reserved */
        unsigned int  sc_int_en0_4reserved         : 1;  /* bit[4] : [4]： reserved */
        unsigned int  sc_int_en0_5reserved         : 1;  /* bit[5] : [5]： reserved */
        unsigned int  sc_int_en0_6reserved         : 1;  /* bit[6] : [6]： reserved */
        unsigned int  sc_int_en0_7reserved         : 1;  /* bit[7] : [7]： reserved */
        unsigned int  sc_int_en0_8reserved         : 1;  /* bit[8] : [8]： reserved */
        unsigned int  sc_int_en0_9reserved         : 1;  /* bit[9] : [9]： reserved */
        unsigned int  sc_int_en0_10reserved        : 1;  /* bit[10]: [10]： reserved */
        unsigned int  sc_int_en0_11reserved        : 1;  /* bit[11]: [11]： reserved */
        unsigned int  sc_int_en0_12reserved        : 1;  /* bit[12]: [12]： reserved */
        unsigned int  sc_int_en0_13reserved        : 1;  /* bit[13]: [13]： reserved */
        unsigned int  sc_int_en0_14reserved        : 1;  /* bit[14]: [14]： reserved */
        unsigned int  sc_int_en0_15reserved        : 1;  /* bit[15]: [15]： reserved */
        unsigned int  sc_int_en0_16reserved        : 1;  /* bit[16]: [16]： reserved */
        unsigned int  sc_int_en0_17reserved        : 1;  /* bit[17]: [17]： reserved */
        unsigned int  sc_int_en0_18reserved        : 1;  /* bit[18]: [18]： reserved */
        unsigned int  sc_int_en0_19reserved        : 1;  /* bit[19]: [19]： reserved */
        unsigned int  sc_int_en0_20reserved        : 1;  /* bit[20]: [20]： reserved */
        unsigned int  sc_int_en0_21reserved        : 1;  /* bit[21]: [21]： reserved */
        unsigned int  sc_int_en0_22reserved        : 1;  /* bit[22]: [22]： reserved */
        unsigned int  sc_int_en0_23reserved        : 1;  /* bit[23]: [23]： reserved */
        unsigned int  sc_int_en0_24reserved        : 1;  /* bit[24]: [24]： reserved */
        unsigned int  sc_int_en0_25reserved        : 1;  /* bit[25]: [25]： reserved */
        unsigned int  sc_int_en0_26reserved        : 1;  /* bit[26]: [26]： reserved */
        unsigned int  sc_int_en0_27reserved        : 1;  /* bit[27]: [27]： reserved */
        unsigned int  sc_int_en0_28reserved        : 1;  /* bit[28]: [28]： reserved */
        unsigned int  sc_int_en0_29reserved        : 1;  /* bit[29]: [29]： reserved */
        unsigned int  sc_int_en0_30reserved        : 1;  /* bit[30]: [30]： reserved */
        unsigned int  sc_int_en0_31reserved        : 1;  /* bit[31]: [31]：reserved */
    } reg;
} SOC_AO_SCTRL_SC_INT_EN_SET1_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_0usim0_detect_int_START  (0)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_0usim0_detect_int_END    (0)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_1usim1_detect_int_START  (1)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_1usim1_detect_int_END    (1)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_2reserved_START          (2)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_2reserved_END            (2)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_3reserved_START          (3)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_3reserved_END            (3)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_4reserved_START          (4)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_4reserved_END            (4)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_5reserved_START          (5)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_5reserved_END            (5)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_6reserved_START          (6)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_6reserved_END            (6)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_7reserved_START          (7)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_7reserved_END            (7)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_8reserved_START          (8)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_8reserved_END            (8)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_9reserved_START          (9)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_9reserved_END            (9)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_10reserved_START         (10)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_10reserved_END           (10)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_11reserved_START         (11)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_11reserved_END           (11)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_12reserved_START         (12)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_12reserved_END           (12)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_13reserved_START         (13)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_13reserved_END           (13)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_14reserved_START         (14)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_14reserved_END           (14)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_15reserved_START         (15)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_15reserved_END           (15)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_16reserved_START         (16)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_16reserved_END           (16)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_17reserved_START         (17)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_17reserved_END           (17)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_18reserved_START         (18)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_18reserved_END           (18)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_19reserved_START         (19)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_19reserved_END           (19)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_20reserved_START         (20)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_20reserved_END           (20)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_21reserved_START         (21)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_21reserved_END           (21)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_22reserved_START         (22)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_22reserved_END           (22)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_23reserved_START         (23)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_23reserved_END           (23)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_24reserved_START         (24)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_24reserved_END           (24)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_25reserved_START         (25)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_25reserved_END           (25)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_26reserved_START         (26)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_26reserved_END           (26)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_27reserved_START         (27)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_27reserved_END           (27)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_28reserved_START         (28)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_28reserved_END           (28)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_29reserved_START         (29)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_29reserved_END           (29)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_30reserved_START         (30)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_30reserved_END           (30)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_31reserved_START         (31)
#define SOC_AO_SCTRL_SC_INT_EN_SET1_sc_int_en0_31reserved_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_EN_DIS1_UNION
 结构说明  : SC_INT_EN_DIS1 寄存器结构定义。地址偏移量:0x0F4，初值:0x00000000，宽度:32
 寄存器说明: 内部中断屏蔽寄存器0，对比特写1，SC_INT_EN_STAT1的对应bit清0；写0不影响时钟屏蔽的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_dis0_0usim0_detect_int : 1;  /* bit[0] : [0]： USIM0 插入拔出检测中断。 */
        unsigned int  sc_int_dis0_1usim1_detect_int : 1;  /* bit[1] : [1]： USIM1 插入拔出检测中断。 */
        unsigned int  sc_int_dis0_2reserved         : 1;  /* bit[2] : [2]： reserved */
        unsigned int  sc_int_dis0_3reserved         : 1;  /* bit[3] : [3]： reserved */
        unsigned int  sc_int_dis0_4reserved         : 1;  /* bit[4] : [4]： reserved */
        unsigned int  sc_int_dis0_5reserved         : 1;  /* bit[5] : [5]： reserved */
        unsigned int  sc_int_dis0_6reserved         : 1;  /* bit[6] : [6]： reserved */
        unsigned int  sc_int_dis0_7reserved         : 1;  /* bit[7] : [7]： reserved */
        unsigned int  sc_int_dis0_8reserved         : 1;  /* bit[8] : [8]： reserved */
        unsigned int  sc_int_dis0_9reserved         : 1;  /* bit[9] : [9]： reserved */
        unsigned int  sc_int_dis0_10reserved        : 1;  /* bit[10]: [10]： reserved */
        unsigned int  sc_int_dis0_11reserved        : 1;  /* bit[11]: [11]： reserved */
        unsigned int  sc_int_dis0_12reserved        : 1;  /* bit[12]: [12]： reserved */
        unsigned int  sc_int_dis0_13reserved        : 1;  /* bit[13]: [13]： reserved */
        unsigned int  sc_int_dis0_14reserved        : 1;  /* bit[14]: [14]： reserved */
        unsigned int  sc_int_dis0_15reserved        : 1;  /* bit[15]: [15]： reserved */
        unsigned int  sc_int_dis0_16reserved        : 1;  /* bit[16]: [16]： reserved */
        unsigned int  sc_int_dis0_17reserved        : 1;  /* bit[17]: [17]： reserved */
        unsigned int  sc_int_dis0_18reserved        : 1;  /* bit[18]: [18]： reserved */
        unsigned int  sc_int_dis0_19reserved        : 1;  /* bit[19]: [19]： reserved */
        unsigned int  sc_int_dis0_20reserved        : 1;  /* bit[20]: [20]： reserved */
        unsigned int  sc_int_dis0_21reserved        : 1;  /* bit[21]: [21]： reserved */
        unsigned int  sc_int_dis0_22reserved        : 1;  /* bit[22]: [22]： reserved */
        unsigned int  sc_int_dis0_23reserved        : 1;  /* bit[23]: [23]： reserved */
        unsigned int  sc_int_dis0_24reserved        : 1;  /* bit[24]: [24]： reserved */
        unsigned int  sc_int_dis0_25reserved        : 1;  /* bit[25]: [25]： reserved */
        unsigned int  sc_int_dis0_26reserved        : 1;  /* bit[26]: [26]： reserved */
        unsigned int  sc_int_dis0_27reserved        : 1;  /* bit[27]: [27]： reserved */
        unsigned int  sc_int_dis0_28reserved        : 1;  /* bit[28]: [28]： reserved */
        unsigned int  sc_int_dis0_29reserved        : 1;  /* bit[29]: [29]： reserved */
        unsigned int  sc_int_dis0_30reserved        : 1;  /* bit[30]: [30]： reserved */
        unsigned int  sc_int_dis0_31reserved        : 1;  /* bit[31]: [31]：reserved */
    } reg;
} SOC_AO_SCTRL_SC_INT_EN_DIS1_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_0usim0_detect_int_START  (0)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_0usim0_detect_int_END    (0)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_1usim1_detect_int_START  (1)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_1usim1_detect_int_END    (1)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_2reserved_START          (2)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_2reserved_END            (2)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_3reserved_START          (3)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_3reserved_END            (3)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_4reserved_START          (4)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_4reserved_END            (4)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_5reserved_START          (5)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_5reserved_END            (5)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_6reserved_START          (6)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_6reserved_END            (6)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_7reserved_START          (7)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_7reserved_END            (7)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_8reserved_START          (8)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_8reserved_END            (8)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_9reserved_START          (9)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_9reserved_END            (9)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_10reserved_START         (10)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_10reserved_END           (10)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_11reserved_START         (11)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_11reserved_END           (11)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_12reserved_START         (12)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_12reserved_END           (12)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_13reserved_START         (13)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_13reserved_END           (13)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_14reserved_START         (14)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_14reserved_END           (14)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_15reserved_START         (15)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_15reserved_END           (15)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_16reserved_START         (16)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_16reserved_END           (16)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_17reserved_START         (17)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_17reserved_END           (17)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_18reserved_START         (18)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_18reserved_END           (18)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_19reserved_START         (19)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_19reserved_END           (19)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_20reserved_START         (20)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_20reserved_END           (20)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_21reserved_START         (21)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_21reserved_END           (21)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_22reserved_START         (22)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_22reserved_END           (22)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_23reserved_START         (23)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_23reserved_END           (23)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_24reserved_START         (24)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_24reserved_END           (24)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_25reserved_START         (25)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_25reserved_END           (25)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_26reserved_START         (26)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_26reserved_END           (26)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_27reserved_START         (27)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_27reserved_END           (27)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_28reserved_START         (28)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_28reserved_END           (28)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_29reserved_START         (29)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_29reserved_END           (29)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_30reserved_START         (30)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_30reserved_END           (30)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_31reserved_START         (31)
#define SOC_AO_SCTRL_SC_INT_EN_DIS1_sc_int_dis0_31reserved_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_INT_EN_STAT1_UNION
 结构说明  : SC_INT_EN_STAT1 寄存器结构定义。地址偏移量:0x0F8，初值:0x00000000，宽度:32
 寄存器说明: 内部中断使能状态寄存器，通过配置SC_INT_EN_SET1和SC_INT_EN_DIS1修改状态，用于SC内部中断的使能和屏蔽设置。对应bit为1表示使能相应中断，为0表示屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_int_en_sta_0usim0_detect_int : 1;  /* bit[0] : [0]： USIM0 插入拔出检测中断。 */
        unsigned int  sc_int_en_sta_1usim1_detect_int : 1;  /* bit[1] : [1]： USIM1 插入拔出检测中断。 */
        unsigned int  sc_int_en_sta_2reserved         : 1;  /* bit[2] : [2]： reserved */
        unsigned int  sc_int_en_sta_3reserved         : 1;  /* bit[3] : [3]： reserved */
        unsigned int  sc_int_en_sta_4reserved         : 1;  /* bit[4] : [4]： reserved */
        unsigned int  sc_int_en_sta_5reserved         : 1;  /* bit[5] : [5]： reserved */
        unsigned int  sc_int_en_sta_6reserved         : 1;  /* bit[6] : [6]： reserved */
        unsigned int  sc_int_en_sta_7reserved         : 1;  /* bit[7] : [7]： reserved */
        unsigned int  sc_int_en_sta_8reserved         : 1;  /* bit[8] : [8]： reserved */
        unsigned int  sc_int_en_sta_9reserved         : 1;  /* bit[9] : [9]： reserved */
        unsigned int  sc_int_en_sta_10reserved        : 1;  /* bit[10]: [10]： reserved */
        unsigned int  sc_int_en_sta_11reserved        : 1;  /* bit[11]: [11]： reserved */
        unsigned int  sc_int_en_sta_12reserved        : 1;  /* bit[12]: [12]： reserved */
        unsigned int  sc_int_en_sta_13reserved        : 1;  /* bit[13]: [13]： reserved */
        unsigned int  sc_int_en_sta_14reserved        : 1;  /* bit[14]: [14]： reserved */
        unsigned int  sc_int_en_sta_15reserved        : 1;  /* bit[15]: [15]： reserved */
        unsigned int  sc_int_en_sta_16reserved        : 1;  /* bit[16]: [16]： reserved */
        unsigned int  sc_int_en_sta_17reserved        : 1;  /* bit[17]: [17]： reserved */
        unsigned int  sc_int_en_sta_18reserved        : 1;  /* bit[18]: [18]： reserved */
        unsigned int  sc_int_en_sta_19reserved        : 1;  /* bit[19]: [19]： reserved */
        unsigned int  sc_int_en_sta_20reserved        : 1;  /* bit[20]: [20]： reserved */
        unsigned int  sc_int_en_sta_21reserved        : 1;  /* bit[21]: [21]： reserved */
        unsigned int  sc_int_en_sta_22reserved        : 1;  /* bit[22]: [22]： reserved */
        unsigned int  sc_int_en_sta_23reserved        : 1;  /* bit[23]: [23]： reserved */
        unsigned int  sc_int_en_sta_24reserved        : 1;  /* bit[24]: [24]： reserved */
        unsigned int  sc_int_en_sta_25reserved        : 1;  /* bit[25]: [25]： reserved */
        unsigned int  sc_int_en_sta_26reserved        : 1;  /* bit[26]: [26]： reserved */
        unsigned int  sc_int_en_sta_27reserved        : 1;  /* bit[27]: [27]： reserved */
        unsigned int  sc_int_en_sta_28reserved        : 1;  /* bit[28]: [28]： reserved */
        unsigned int  sc_int_en_sta_29reserved        : 1;  /* bit[29]: [29]： reserved */
        unsigned int  sc_int_en_sta_30reserved        : 1;  /* bit[30]: [30]： reserved */
        unsigned int  sc_int_en_sta_31reserved        : 1;  /* bit[31]: [31]：reserved */
    } reg;
} SOC_AO_SCTRL_SC_INT_EN_STAT1_UNION;
#endif
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_0usim0_detect_int_START  (0)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_0usim0_detect_int_END    (0)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_1usim1_detect_int_START  (1)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_1usim1_detect_int_END    (1)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_2reserved_START          (2)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_2reserved_END            (2)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_3reserved_START          (3)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_3reserved_END            (3)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_4reserved_START          (4)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_4reserved_END            (4)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_5reserved_START          (5)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_5reserved_END            (5)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_6reserved_START          (6)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_6reserved_END            (6)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_7reserved_START          (7)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_7reserved_END            (7)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_8reserved_START          (8)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_8reserved_END            (8)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_9reserved_START          (9)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_9reserved_END            (9)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_10reserved_START         (10)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_10reserved_END           (10)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_11reserved_START         (11)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_11reserved_END           (11)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_12reserved_START         (12)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_12reserved_END           (12)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_13reserved_START         (13)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_13reserved_END           (13)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_14reserved_START         (14)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_14reserved_END           (14)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_15reserved_START         (15)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_15reserved_END           (15)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_16reserved_START         (16)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_16reserved_END           (16)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_17reserved_START         (17)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_17reserved_END           (17)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_18reserved_START         (18)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_18reserved_END           (18)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_19reserved_START         (19)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_19reserved_END           (19)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_20reserved_START         (20)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_20reserved_END           (20)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_21reserved_START         (21)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_21reserved_END           (21)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_22reserved_START         (22)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_22reserved_END           (22)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_23reserved_START         (23)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_23reserved_END           (23)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_24reserved_START         (24)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_24reserved_END           (24)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_25reserved_START         (25)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_25reserved_END           (25)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_26reserved_START         (26)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_26reserved_END           (26)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_27reserved_START         (27)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_27reserved_END           (27)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_28reserved_START         (28)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_28reserved_END           (28)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_29reserved_START         (29)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_29reserved_END           (29)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_30reserved_START         (30)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_30reserved_END           (30)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_31reserved_START         (31)
#define SOC_AO_SCTRL_SC_INT_EN_STAT1_sc_int_en_sta_31reserved_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_TIMER_EN0_UNION
 结构说明  : SC_TIMER_EN0 寄存器结构定义。地址偏移量:0x1D0，初值:0x00000000，宽度:32
 寄存器说明: timer使能寄存器0。
            寄存器写入时需要msk有效，[31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer_en0_1acpu_dualtimer0_en0      : 2;  /* bit[0-1]  : [1:0] acpu dualtimer0 的en0，对应acpu timer0
                                                                               时钟使能控制含义：
                                                                               x0：en＝1，参考时钟选择clk_slow，系统模式处于Normal和SLOW时为19.2MHz，系统模式进入Sleep后，参考时钟为32KHz；
                                                                               x1：参考时钟选择32KHz。 */
        unsigned int  timer_en0_3acpu_dualtimer0_en1      : 2;  /* bit[2-3]  : [3:2] acpu dualtimer0 的en1 ，对应acpu timer1，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en0_5acpu_dualtimer1_en0      : 2;  /* bit[4-5]  : [5:4] acpu dualtimer1 的en0，对应acpu timer2，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en0_7acpu_dualtimer1_en1      : 2;  /* bit[6-7]  : [7:6] acpu dualtimer1 的en1 ，对应acpu timer3，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en0_9acpu_dualtimer2_en0      : 2;  /* bit[8-9]  : [9:8] acpu dualtimer2 的en0，对应acpu timer4，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en0_11acpu_dualtimer2_en1     : 2;  /* bit[10-11]: [11:10] acpu dualtimer2 的en1，对应acpu timer5，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en0_13acpu_dualtimer3_en0     : 2;  /* bit[12-13]: [13:12] acpu dualtimer3 的en0，对应acpu timer6，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en0_15acpu_dualtimer3_en1     : 2;  /* bit[14-15]: [15:14] acpu dualtimer3 的en1，对应acpu timer7，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。  */
        unsigned int  timer_en0_msk_1acpu_dualtimer0_en0  : 2;  /* bit[16-17]: [17:16] acpu timer0 msk */
        unsigned int  timer_en0_msk_3acpu_dualtimer0_en1  : 2;  /* bit[18-19]: [19:18] acpu timer1 msk */
        unsigned int  timer_en0_msk_5acpu_dualtimer1_en0  : 2;  /* bit[20-21]: [21:20] acpu timer2 msk */
        unsigned int  timer_en0_msk_7acpu_dualtimer1_en1  : 2;  /* bit[22-23]: [23:22] acpu timer3 msk */
        unsigned int  timer_en0_msk_9acpu_dualtimer2_en0  : 2;  /* bit[24-25]: [25:24] acpu timer4 msk */
        unsigned int  timer_en0_msk_11acpu_dualtimer2_en1 : 2;  /* bit[26-27]: [27:26] acpu timer5 msk */
        unsigned int  timer_en0_msk_13acpu_dualtimer3_en0 : 2;  /* bit[28-29]: [29:28] acpu timer6 msk */
        unsigned int  timer_en0_msk_15acpu_dualtimer3_en1 : 2;  /* bit[30-31]: [30:30] acpu timer7 msk */
    } reg;
} SOC_AO_SCTRL_SC_TIMER_EN0_UNION;
#endif
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_1acpu_dualtimer0_en0_START       (0)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_1acpu_dualtimer0_en0_END         (1)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_3acpu_dualtimer0_en1_START       (2)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_3acpu_dualtimer0_en1_END         (3)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_5acpu_dualtimer1_en0_START       (4)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_5acpu_dualtimer1_en0_END         (5)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_7acpu_dualtimer1_en1_START       (6)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_7acpu_dualtimer1_en1_END         (7)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_9acpu_dualtimer2_en0_START       (8)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_9acpu_dualtimer2_en0_END         (9)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_11acpu_dualtimer2_en1_START      (10)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_11acpu_dualtimer2_en1_END        (11)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_13acpu_dualtimer3_en0_START      (12)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_13acpu_dualtimer3_en0_END        (13)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_15acpu_dualtimer3_en1_START      (14)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_15acpu_dualtimer3_en1_END        (15)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_1acpu_dualtimer0_en0_START   (16)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_1acpu_dualtimer0_en0_END     (17)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_3acpu_dualtimer0_en1_START   (18)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_3acpu_dualtimer0_en1_END     (19)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_5acpu_dualtimer1_en0_START   (20)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_5acpu_dualtimer1_en0_END     (21)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_7acpu_dualtimer1_en1_START   (22)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_7acpu_dualtimer1_en1_END     (23)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_9acpu_dualtimer2_en0_START   (24)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_9acpu_dualtimer2_en0_END     (25)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_11acpu_dualtimer2_en1_START  (26)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_11acpu_dualtimer2_en1_END    (27)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_13acpu_dualtimer3_en0_START  (28)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_13acpu_dualtimer3_en0_END    (29)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_15acpu_dualtimer3_en1_START  (30)
#define SOC_AO_SCTRL_SC_TIMER_EN0_timer_en0_msk_15acpu_dualtimer3_en1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_TIMER_EN1_UNION
 结构说明  : SC_TIMER_EN1 寄存器结构定义。地址偏移量:0x1D4，初值:0x00000000，宽度:32
 寄存器说明: timer使能寄存器1。
            寄存器写入时需要msk有效，[31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer_en1_1acpu_dualtimer4_en0      : 2;  /* bit[0-1]  : [1:0] acpu dualtimer4 的en0，对应acpu timer8
                                                                               时钟使能控制含义：
                                                                               x0：en＝1，参考时钟选择clk_slow，系统模式处于Normal和SLOW时为19.2MHz，系统模式进入Sleep后，参考时钟为32KHz；
                                                                               x1：参考时钟选择32KHz。 */
        unsigned int  timer_en1_3acpu_dualtimer4_en1      : 2;  /* bit[2-3]  : [3:2] acpu dualtimer4 的en1 ，对应acpu timer9，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en1_5acpu_dualtimer5_en0      : 2;  /* bit[4-5]  : [5:4] acpu dualtimer5 的en0，对应acpu timer10，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en1_7acpu_dualtimer5_en1      : 2;  /* bit[6-7]  : [7:6] acpu dualtimer5 的en1 ，对应acpu timer11，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en1_9acpu_dualtimer6_en0      : 2;  /* bit[8-9]  : [9:8] acpu dualtimer6 的en0，对应acpu timer12，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en1_11acpu_dualtimer6_en1     : 2;  /* bit[10-11]: [11:10] acpu dualtimer6 的en1，对应acpu timer13，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en1_13acpu_dualtimer7_en0     : 2;  /* bit[12-13]: [13:12] acpu dualtimer7 的en0，对应acpu timer14，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en1_15acpu_dualtimer7_en1     : 2;  /* bit[14-15]: [15:14] acpu dualtimer7 的en1，对应acpu timer15，
                                                                               时钟使能控制含义参考timer_en0_1acpu_dualtimer0_en0。 */
        unsigned int  timer_en1_msk_1acpu_dualtimer4_en0  : 2;  /* bit[16-17]: [17:16] acpu timer8 msk */
        unsigned int  timer_en1_msk_3acpu_dualtimer4_en1  : 2;  /* bit[18-19]: [19:18] acpu timer9 msk */
        unsigned int  timer_en1_msk_5acpu_dualtimer5_en0  : 2;  /* bit[20-21]: [21:20] acpu timer10 msk */
        unsigned int  timer_en1_msk_7acpu_dualtimer5_en1  : 2;  /* bit[22-23]: [23:22] acpu timer11 msk */
        unsigned int  timer_en1_msk_9acpu_dualtimer6_en0  : 2;  /* bit[24-25]: [25:24] acpu timer12 msk */
        unsigned int  timer_en1_msk_11acpu_dualtimer6_en1 : 2;  /* bit[26-27]: [27:26] acpu timer13 msk */
        unsigned int  timer_en1_msk_13acpu_dualtimer7_en0 : 2;  /* bit[28-29]: [29:28] acpu timer14 msk */
        unsigned int  timer_en1_msk_15acpu_dualtimer7_en1 : 2;  /* bit[30-31]: [30:30] acpu timer15 msk */
    } reg;
} SOC_AO_SCTRL_SC_TIMER_EN1_UNION;
#endif
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_1acpu_dualtimer4_en0_START       (0)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_1acpu_dualtimer4_en0_END         (1)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_3acpu_dualtimer4_en1_START       (2)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_3acpu_dualtimer4_en1_END         (3)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_5acpu_dualtimer5_en0_START       (4)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_5acpu_dualtimer5_en0_END         (5)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_7acpu_dualtimer5_en1_START       (6)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_7acpu_dualtimer5_en1_END         (7)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_9acpu_dualtimer6_en0_START       (8)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_9acpu_dualtimer6_en0_END         (9)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_11acpu_dualtimer6_en1_START      (10)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_11acpu_dualtimer6_en1_END        (11)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_13acpu_dualtimer7_en0_START      (12)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_13acpu_dualtimer7_en0_END        (13)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_15acpu_dualtimer7_en1_START      (14)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_15acpu_dualtimer7_en1_END        (15)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_1acpu_dualtimer4_en0_START   (16)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_1acpu_dualtimer4_en0_END     (17)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_3acpu_dualtimer4_en1_START   (18)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_3acpu_dualtimer4_en1_END     (19)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_5acpu_dualtimer5_en0_START   (20)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_5acpu_dualtimer5_en0_END     (21)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_7acpu_dualtimer5_en1_START   (22)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_7acpu_dualtimer5_en1_END     (23)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_9acpu_dualtimer6_en0_START   (24)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_9acpu_dualtimer6_en0_END     (25)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_11acpu_dualtimer6_en1_START  (26)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_11acpu_dualtimer6_en1_END    (27)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_13acpu_dualtimer7_en0_START  (28)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_13acpu_dualtimer7_en0_END    (29)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_15acpu_dualtimer7_en1_START  (30)
#define SOC_AO_SCTRL_SC_TIMER_EN1_timer_en1_msk_15acpu_dualtimer7_en1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_TIMER_EN4_UNION
 结构说明  : SC_TIMER_EN4 寄存器结构定义。地址偏移量:0x1F0，初值:0x00000000，宽度:32
 寄存器说明: timer使能寄存器4。
            寄存器写入时需要msk有效，[31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer_en4_mcu_dualtimer0_en0        : 2;  /* bit[0-1]  : [1:0] mcu Dualtimer0的en0，对应mcu timer0
                                                                               mcu timer0~1时钟使能控制。
                                                                               x0：en＝1，参考时钟选择clk_slow，系统模式处于Normal和SLOW时为19.2MHz，系统模式进入Sleep后，参考时钟为32KHz；
                                                                               x1：参考时钟选择32KHz。 */
        unsigned int  timer_en4_3mcu_dualtimer0_en1       : 2;  /* bit[2-3]  : [3:2] mcu Dualtimer0的en1，对应mcu timer1
                                                                               mcu timer0~1时钟使能控制。
                                                                               x0：en＝1，参考时钟选择clk_slow，系统模式处于Normal和SLOW时为19.2MHz，系统模式进入Sleep后，参考时钟为32KHz；
                                                                               x1：参考时钟选择32KHz。 */
        unsigned int  timer_en4_5mcu_dualtimer1_en0       : 2;  /* bit[4-5]  : [5:4] mcu Dualtimer1的en0，对应mcu timer2 */
        unsigned int  timer_en4_7mcu_dualtimer1_en1       : 2;  /* bit[6-7]  : [7:6] mcu Dualtimer1的en1，对应mcu timer3 */
        unsigned int  reserved_0                          : 8;  /* bit[8-15] : reserved */
        unsigned int  timer_en4_msk_acpu_dualtimer9_en0   : 2;  /* bit[16-17]: [17:16] mcu timer0 */
        unsigned int  timer_en4_msk_3acpu_dualtimer9_en1  : 2;  /* bit[18-19]: [19:18] mcu timer1 */
        unsigned int  timer_en4_msk_5acpu_dualtimer10_en0 : 2;  /* bit[20-21]: [21:20] mcu timer2 */
        unsigned int  timer_en4_msk_7acpu_dualtimer10_en1 : 2;  /* bit[22-23]: [23:22] mcu timer3 */
        unsigned int  reserved_1                          : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_TIMER_EN4_UNION;
#endif
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_mcu_dualtimer0_en0_START         (0)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_mcu_dualtimer0_en0_END           (1)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_3mcu_dualtimer0_en1_START        (2)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_3mcu_dualtimer0_en1_END          (3)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_5mcu_dualtimer1_en0_START        (4)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_5mcu_dualtimer1_en0_END          (5)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_7mcu_dualtimer1_en1_START        (6)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_7mcu_dualtimer1_en1_END          (7)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_msk_acpu_dualtimer9_en0_START    (16)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_msk_acpu_dualtimer9_en0_END      (17)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_msk_3acpu_dualtimer9_en1_START   (18)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_msk_3acpu_dualtimer9_en1_END     (19)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_msk_5acpu_dualtimer10_en0_START  (20)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_msk_5acpu_dualtimer10_en0_END    (21)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_msk_7acpu_dualtimer10_en1_START  (22)
#define SOC_AO_SCTRL_SC_TIMER_EN4_timer_en4_msk_7acpu_dualtimer10_en1_END    (23)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_TIMER_EN5_UNION
 结构说明  : SC_TIMER_EN5 寄存器结构定义。地址偏移量:0x1F4，初值:0x00000000，宽度:32
 寄存器说明: timer使能寄存器5。
            寄存器写入时需要msk有效，[31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer_en5_acpu_dualtimer8_en0     : 3;  /* bit[0-2]  : [2:0] acpu dualtimer8 的en0时钟使能控制，含义如下：
                                                                             xx0：en＝1，参考时钟选择clk_slow，系统模式处于Normal和SLOW时为19.2MHz，系统模式进入Sleep后，参考时钟为32KHz；
                                                                             xx1：参考时钟选择32KHz。 */
        unsigned int  timer_en5_acpu_dualtimer8_en1     : 3;  /* bit[3-5]  : [5:3] acpu dualtimer8 的en1时钟使能控制，含义如下：
                                                                             xx0：en＝1，参考时钟选择clk_slow，系统模式处于Normal和SLOW时为19.2MHz，系统模式进入Sleep后，参考时钟为32KHz；
                                                                             xx1：参考时钟选择32KHz。 */
        unsigned int  timer_en5_reserved                : 10; /* bit[6-15] : reserved */
        unsigned int  timer_en5_msk_acpu_dualtimer8_en0 : 3;  /* bit[16-18]: [18:16] acpu dualtimer8 的en0 */
        unsigned int  timer_en5_msk_acpu_dualtimer8_en1 : 3;  /* bit[19-21]: [21:19] acpu dualtimer8 的en1 */
        unsigned int  timer_en5_msk                     : 10; /* bit[22-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_TIMER_EN5_UNION;
#endif
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_acpu_dualtimer8_en0_START      (0)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_acpu_dualtimer8_en0_END        (2)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_acpu_dualtimer8_en1_START      (3)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_acpu_dualtimer8_en1_END        (5)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_reserved_START                 (6)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_reserved_END                   (15)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_msk_acpu_dualtimer8_en0_START  (16)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_msk_acpu_dualtimer8_en0_END    (18)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_msk_acpu_dualtimer8_en1_START  (19)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_msk_acpu_dualtimer8_en1_END    (21)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_msk_START                      (22)
#define SOC_AO_SCTRL_SC_TIMER_EN5_timer_en5_msk_END                        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL0_UNION
 结构说明  : SC_MCU_SUBSYS_CTRL0 寄存器结构定义。地址偏移量:0x400，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_share_mem_waterlevel : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL0_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL0_mcu_share_mem_waterlevel_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL0_mcu_share_mem_waterlevel_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_UNION
 结构说明  : SC_MCU_SUBSYS_CTRL1 寄存器结构定义。地址偏移量:0x404，初值:0x0516E360，宽度:32
 寄存器说明: MCU子系统控制寄存器，功能包含MCU的DAP enable,Tick时钟校准,MCU NMI中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_ctrl1_dbgen_stcalib   : 26; /* bit[0-25] : [26:1]：DBGEN STCALIB[25:0],系统Tick时钟的校准,默认值0x116e360 */
        unsigned int  mcu_subsys_ctrl1_dbgen_dapenable : 1;  /* bit[26]   : MCU DAP enable,复位值1
                                                                            0：debug disable；
                                                                            1：debug enable。 */
        unsigned int  reserved                         : 2;  /* bit[27-28]: reserved */
        unsigned int  cm3_sft_nmi_intreq               : 1;  /* bit[29]   : 配置产生CM3 NMI软件中断
                                                                            0：消除CM3 NMI软件中断；
                                                                            1：产生CM3 NMI软件中断。 */
        unsigned int  cache_hready_sel                 : 1;  /* bit[30]   : cm3 cache hsel signal
                                                                            0：hsel_cm3cache&amp;hreadyin_cachereg；
                                                                            1：hsel_cm3cache。 */
        unsigned int  mcu_subsys_ctrl1                 : 1;  /* bit[31]   : reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_mcu_subsys_ctrl1_dbgen_stcalib_START    (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_mcu_subsys_ctrl1_dbgen_stcalib_END      (25)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_mcu_subsys_ctrl1_dbgen_dapenable_START  (26)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_mcu_subsys_ctrl1_dbgen_dapenable_END    (26)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_cm3_sft_nmi_intreq_START                (29)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_cm3_sft_nmi_intreq_END                  (29)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_cache_hready_sel_START                  (30)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_cache_hready_sel_END                    (30)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_mcu_subsys_ctrl1_START                  (31)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL1_mcu_subsys_ctrl1_END                    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL2_UNION
 结构说明  : SC_MCU_SUBSYS_CTRL2 寄存器结构定义。地址偏移量:0x408，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统控制寄存器,功能包含MCU的DDR REMAP起始地址配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_remap_ddr_startaddr : 32; /* bit[0-31]: [31:0]：ddr rmp addr for cm3，mcu需要执行代码地址空间大于48KB时，可以配置此地址实现访问DDR时的起始地址 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL2_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL2_mcu_remap_ddr_startaddr_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL2_mcu_remap_ddr_startaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_UNION
 结构说明  : SC_MCU_SUBSYS_CTRL3 寄存器结构定义。地址偏移量:0x40C，初值:0x00000711，宽度:32
 寄存器说明: MCU子系统控制寄存器，功能包含PMUSSI控制、cssys_ctrl_prot控制。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_ctrl3_pmussi   : 3;  /* bit[0-2]  : [5：3]：pmussi mst count 默认3'b001
                                                                     该值用来配置读操作时，master check读同步位的延迟时间
                                                                     000：1.5 clk； 
                                                                     001：2.0 clk；
                                                                     010：2.5 clk；
                                                                     011：3.0 clk；
                                                                     100：3.5 clk；
                                                                     101：4.0 clk；
                                                                     110：4.5 clk；
                                                                     111：5.0 clk。 */
        unsigned int  cssys_ctrl_prot           : 1;  /* bit[3]    : cssys　dbg pwup and sys pwup ack ctrl
                                                                     0：set cssys_dbg/sys_pwup_ack to 1'b0 when cssys_dbg/sys_pwup_req comes in pw_stat of not PWON ；
                                                                     1：no set cssys_dbg/sys_pwup_ack to 1'b0 when cssys_dbg/sys_pwup_req comes in pw_stat of not PWON 。 */
        unsigned int  tcxo_afc_oen_crg          : 1;  /* bit[4]    : tcxo afc output enable
                                                                     0：output enable；
                                                                     1：output disable,high Z。 */
        unsigned int  reserved                  : 3;  /* bit[5-7]  : reserved */
        unsigned int  aob_io_sel18_usim1        : 1;  /* bit[8]    : USIM1 IO Voltage Select
                                                                     0：IO_SEL 1'b1,3.0V；
                                                                     1：IO_SEL 1'b0,1.8V。
                                                                     Note:Integrated by inv-logic。 */
        unsigned int  aob_io_sel18_usim0        : 1;  /* bit[9]    : USIM0 IO Voltage Select
                                                                     0：IO_SEL 1'b1,3.0V；
                                                                     1：IO_SEL 1'b0,1.8V。
                                                                     Note:Integrated by inv-logic。 */
        unsigned int  aob_io_sel18_sd           : 1;  /* bit[10]   : SD IO Voltage Select
                                                                     0：IO_SEL 1'b1,3.0V；
                                                                     1：IO_SEL 1'b0,1.8V。
                                                                     Note:Integrated by inv-logic。 */
        unsigned int  mcu_subsys_ctrl3_reserved : 21; /* bit[11-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_mcu_subsys_ctrl3_pmussi_START    (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_mcu_subsys_ctrl3_pmussi_END      (2)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_cssys_ctrl_prot_START            (3)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_cssys_ctrl_prot_END              (3)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_tcxo_afc_oen_crg_START           (4)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_tcxo_afc_oen_crg_END             (4)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_aob_io_sel18_usim1_START         (8)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_aob_io_sel18_usim1_END           (8)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_aob_io_sel18_usim0_START         (9)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_aob_io_sel18_usim0_END           (9)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_aob_io_sel18_sd_START            (10)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_aob_io_sel18_sd_END              (10)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_mcu_subsys_ctrl3_reserved_START  (11)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL3_mcu_subsys_ctrl3_reserved_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_UNION
 结构说明  : SC_MCU_SUBSYS_CTRL4 寄存器结构定义。地址偏移量:0x410，初值:0x00000C9B，宽度:32
 寄存器说明: MCU子系统控制寄存器，功能包含COREON Memory控制。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coreon_ramctrl_s_ema   : 3;  /* bit[0-2]  : memory读速率控制，随此值的增大，memory读延时增大。 */
        unsigned int  reserved_0             : 1;  /* bit[3]    : reserved */
        unsigned int  coreon_ramctrl_s_emaw  : 2;  /* bit[4-5]  : memory写速率控制，随此值的增大，memory写延时增大（文档含义是这样，看LIB含义又不是，后面会更新文档）。 */
        unsigned int  coreon_ramctrl_s_emas  : 1;  /* bit[6]    : 对HS memory有作用；
                                                                  对HD memory为保留bit，无意义。 */
        unsigned int  reserved_1             : 3;  /* bit[7-9]  : reserved */
        unsigned int  coreon_ramctrl_s_ret1n : 1;  /* bit[10]   : retention 控制1
                                                                  0：retention模式1；
                                                                  1：正常工作模式。
                                                                  注：此功能由硬件控制，软件无法控制。 */
        unsigned int  coreon_ramctrl_s_ret2n : 1;  /* bit[11]   : 只有当打开power gating选项时有此功能，对没有power gating功能的memory，此端口为保留，无意义。
                                                                  retention 控制2
                                                                  0：retention模式2；
                                                                  1：正常工作模式。
                                                                  注：此功能由硬件控制，软件无法控制。 */
        unsigned int  coreon_ramctrl_s_pgen  : 1;  /* bit[12]   : 只有当打开power gating选项时有此功能，对没有power gating功能的memory，此端口为保留，无意义。
                                                                  0：正常工作模式；
                                                                  1：配合ret1n/ret2n完成retention。
                                                                  注：此功能由硬件控制，软件无法控制。 */
        unsigned int  reserved_2             : 3;  /* bit[13-15]: reserved */
        unsigned int  reserved_3             : 16; /* bit[16-31]: 
                                                                  reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_ema_START    (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_ema_END      (2)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_emaw_START   (4)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_emaw_END     (5)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_emas_START   (6)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_emas_END     (6)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_ret1n_START  (10)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_ret1n_END    (10)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_ret2n_START  (11)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_ret2n_END    (11)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_pgen_START   (12)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL4_coreon_ramctrl_s_pgen_END     (12)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_UNION
 结构说明  : SC_MCU_SUBSYS_CTRL5 寄存器结构定义。地址偏移量:0x414，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统控制寄存器，功能包含MCU SHARE MEM地址配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_share_mem_addr_mapaddr : 15; /* bit[0-14] : mcu访问share mem的映射基地址。即当remap使能时,且mcu访问外部地址空间，mcu访问这个基地址时会被映射到share mem空间（比如Harq mem），访问偏移地址为：{mcu_share_mem_addr_mapaddr,mcu_share_mem_pageaddr,17'h0}。 */
        unsigned int  reserved                   : 1;  /* bit[15]   : reserved */
        unsigned int  mcu_share_mem_pageaddr     : 3;  /* bit[16-18]: mcu访问share mem的映射页地址,每个page为128KB。 */
        unsigned int  mcu_share_mem_remap_en     : 1;  /* bit[19]   : mcu访问share mem的映射使能
                                                                      0：disable；
                                                                      1：enable。 */
        unsigned int  mcu_share_mem_addr         : 12; /* bit[20-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_mcu_share_mem_addr_mapaddr_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_mcu_share_mem_addr_mapaddr_END    (14)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_mcu_share_mem_pageaddr_START      (16)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_mcu_share_mem_pageaddr_END        (18)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_mcu_share_mem_remap_en_START      (19)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_mcu_share_mem_remap_en_END        (19)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_mcu_share_mem_addr_START          (20)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL5_mcu_share_mem_addr_END            (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_UNION
 结构说明  : SC_MCU_SUBSYS_CTRL6 寄存器结构定义。地址偏移量:0x418，初值:0x0EFF0000，宽度:32
 寄存器说明: MCU子系统控制寄存器，功能包括CM3保护地址空间起始地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cm3_acc_ddr_protect_str_addr : 12; /* bit[0-11] : CM3保护地址起始地址,单位1MByte,对应地址为[cm3_acc_ddr_protect_str_addr,0x00000]，此配置仅在cm3_acc_ddr_protect_ctrl=1'b1时有效。 */
        unsigned int  reserved_0                   : 4;  /* bit[12-15]: reserved */
        unsigned int  cm3_acc_ddr_protect_end_addr : 12; /* bit[16-27]: CM3保护地址结束地址,单位1MByte,对应地址为[cm3_acc_ddr_protect_end_addr,0xFFFFF]，此配置仅在cm3_acc_ddr_protect_ctrl=1'b1时有效。 */
        unsigned int  reserved_1                   : 2;  /* bit[28-29]: reserved */
        unsigned int  cm3_acc_ddr_protect_sel      : 1;  /* bit[30]   : CM3访问DDR保护空间选择,此配置仅在cm3_acc_ddr_protect_ctrl=1'b1时有效
                                                                        0：cm3_acc_ddr_protect_beg和cm3_acc_ddr_protect_end定义的空间可正常访问，访问此空间以外mcu_remap_ddr_startaddr定义的DDR空间返回总线error；
                                                                        1：cm3_acc_ddr_protect_beg和cm3_acc_ddr_protect_end定义的空间返回总线error，此空间以外mcu_remap_ddr_startaddr定义的DDR空间可正常访问。 */
        unsigned int  cm3_acc_ddr_protect_ctrl     : 1;  /* bit[31]   : CM3访问DDR保护空间功能控制,
                                                                        0：屏蔽空间保护功能；
                                                                        1：使能空间保护功能。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_cm3_acc_ddr_protect_str_addr_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_cm3_acc_ddr_protect_str_addr_END    (11)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_cm3_acc_ddr_protect_end_addr_START  (16)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_cm3_acc_ddr_protect_end_addr_END    (27)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_cm3_acc_ddr_protect_sel_START       (30)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_cm3_acc_ddr_protect_sel_END         (30)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_cm3_acc_ddr_protect_ctrl_START      (31)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL6_cm3_acc_ddr_protect_ctrl_END        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL7_UNION
 结构说明  : SC_MCU_SUBSYS_CTRL7 寄存器结构定义。地址偏移量:0x41C，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统控制寄存器，功能包括CM3保护地址空间结束地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved      : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_CTRL7_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_UNION
 结构说明  : SC_MCU_SUBSYS_STAT0 寄存器结构定义。地址偏移量:0x440，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统状态寄存器，功能包含MCU状态信号。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_state0_sysresetreq : 1;  /* bit[0]    : [0]：SYSRESETREQ
                                                                         1'b0：MCU无复位请求；
                                                                         1'b1：MCU有复位请求。 */
        unsigned int  mcu_subsys_state0_sleeping    : 1;  /* bit[1]    : [1]：SLEEPING
                                                                         1'b0：MCU未进入睡眠；
                                                                         1'b1：MCU已进入睡眠。 */
        unsigned int  mcu_subsys_state0_sleepdeep   : 1;  /* bit[2]    : [2]：SLEEPDEEP
                                                                         1'b0：MCU未进入深睡；
                                                                         1'b1：MCU已进入深睡。 */
        unsigned int  mcu_subsys_state0_lockup      : 1;  /* bit[3]    : [3]：LOCKUP：LOCKUP gives immediate indication of seriously errant kernel software.
                                                                         1'b0：MCU未锁核；
                                                                         1'b1：MCU已锁核，表示软件的严重错误。 */
        unsigned int  mcu_subsys_state0_currpri     : 8;  /* bit[4-11] : [11:4]：CURRPRI[7:0]表示当前正在被处理的中断的优先级 */
        unsigned int  mcu_subsys_state0_brchstat    : 4;  /* bit[12-15]: [15:12]：BRCHSTAT[3:0]
                                                                         0000 No hint；
                                                                         0001 Conditional branch backwards in decode；
                                                                         0010 Conditional branch in decode；
                                                                         0011 Conditional branch in execute；
                                                                         0100 Unconditional branch in decode；
                                                                         0101 Unconditional branch in execute；
                                                                         0110 Reserved；
                                                                         0111 Reserved；
                                                                         1000 Conditional branch in decode taken (cycle after IHTRANS)；
                                                                         1001 ... 1111 Reserved。 */
        unsigned int  mcu_subsys_state0_reserved    : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_sysresetreq_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_sysresetreq_END    (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_sleeping_START     (1)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_sleeping_END       (1)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_sleepdeep_START    (2)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_sleepdeep_END      (2)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_lockup_START       (3)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_lockup_END         (3)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_currpri_START      (4)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_currpri_END        (11)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_brchstat_START     (12)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_brchstat_END       (15)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_reserved_START     (16)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT0_mcu_subsys_state0_reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT1_UNION
 结构说明  : SC_MCU_SUBSYS_STAT1 寄存器结构定义。地址偏移量:0x444，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统状态寄存器，功能包含保留。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_state1 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT1_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT1_mcu_subsys_state1_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT1_mcu_subsys_state1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT2_UNION
 结构说明  : SC_MCU_SUBSYS_STAT2 寄存器结构定义。地址偏移量:0x448，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_state2 : 32; /* bit[0-31]: INTERNALSTATE[148:0]
                                                            bits [31:0] = Instruction in execute。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT2_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT2_mcu_subsys_state2_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT2_mcu_subsys_state2_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT3_UNION
 结构说明  : SC_MCU_SUBSYS_STAT3 寄存器结构定义。地址偏移量:0x44C，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_state3 : 32; /* bit[0-31]: INTERNALSTATE[148:0]
                                                            bits [63:32] = Register bank write port data。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT3_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT3_mcu_subsys_state3_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT3_mcu_subsys_state3_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_UNION
 结构说明  : SC_MCU_SUBSYS_STAT4 寄存器结构定义。地址偏移量:0x450，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_state4_bank_write_port_enable   : 1;  /* bit[0]    : INTERNALSTATE[148:0]
                                                                                      bit [64] = Register bank write port enable。 */
        unsigned int  mcu_subsys_state4_bank_write_port_addres   : 4;  /* bit[1-4]  : INTERNALSTATE[148:0]
                                                                                      bits [68:65] = Register bank write port address。 */
        unsigned int  mcu_subsys_state4_bank_read_port_A_address : 4;  /* bit[5-8]  : INTERNALSTATE[148:0]
                                                                                      bits [72:69] = Register bank read port A address。 */
        unsigned int  mcu_subsys_state4_bank_read_port_B_address : 4;  /* bit[9-12] : INTERNALSTATE[148:0]
                                                                                      bits [76:73] = Register bank read port B address。 */
        unsigned int  mcu_subsys_state4                          : 19; /* bit[13-31]: [31:13]reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_bank_write_port_enable_START    (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_bank_write_port_enable_END      (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_bank_write_port_addres_START    (1)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_bank_write_port_addres_END      (4)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_bank_read_port_A_address_START  (5)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_bank_read_port_A_address_END    (8)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_bank_read_port_B_address_START  (9)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_bank_read_port_B_address_END    (12)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_START                           (13)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT4_mcu_subsys_state4_END                             (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT5_UNION
 结构说明  : SC_MCU_SUBSYS_STAT5 寄存器结构定义。地址偏移量:0x454，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_state5 : 32; /* bit[0-31]: INTERNALSTATE[148:0]
                                                            bits [108:77] = Register bank read port A data。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT5_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT5_mcu_subsys_state5_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT5_mcu_subsys_state5_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT6_UNION
 结构说明  : SC_MCU_SUBSYS_STAT6 寄存器结构定义。地址偏移量:0x458，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_state6 : 32; /* bit[0-31]: INTERNALSTATE[148:0]
                                                            bits [140:109] = Register bank read port B data。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT6_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT6_mcu_subsys_state6_START  (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT6_mcu_subsys_state6_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_UNION
 结构说明  : SC_MCU_SUBSYS_STAT7 寄存器结构定义。地址偏移量:0x45C，初值:0x00000000，宽度:32
 寄存器说明: MCU子系统状态寄存器，功能包含MCU状态信号INTERNALSTATE。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_subsys_state7_DAP_HTRANS                  : 2;  /* bit[0-1] : INTERNALSTATE
                                                                                        bits [142:141] = DAP HTRANS。 */
        unsigned int  mcu_subsys_state7_DAP_abort                   : 1;  /* bit[2]   : INTERNALSTATE
                                                                                        bit [143] = DAP abort。 */
        unsigned int  mcu_subsys_state7_Watchpoint                  : 1;  /* bit[3]   : INTERNALSTATE
                                                                                        bit [144] = Watchpoint。 */
        unsigned int  mcu_subsys_state7_FPB_instruction_remap_match : 1;  /* bit[4]   : INTERNALSTATE
                                                                                        bit [145] = FPB instruction remap match。 */
        unsigned int  mcu_subsys_state7_FPB_literal_remap_match     : 1;  /* bit[5]   : INTERNALSTATE
                                                                                        bit [146] = FPB literal remap match。 */
        unsigned int  mcu_subsys_state7_FPB_BKPT_match              : 1;  /* bit[6]   : INTERNALSTATE
                                                                                        bit [147] = FPB BKPT match。 */
        unsigned int  mcu_subsys_state7_SysTick_interrupt_request   : 1;  /* bit[7]   : INTERNALSTATE
                                                                                        bit [148] = SysTick interrupt request。 */
        unsigned int  mcu_subsys_state7                             : 24; /* bit[8-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_DAP_HTRANS_START                   (0)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_DAP_HTRANS_END                     (1)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_DAP_abort_START                    (2)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_DAP_abort_END                      (2)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_Watchpoint_START                   (3)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_Watchpoint_END                     (3)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_FPB_instruction_remap_match_START  (4)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_FPB_instruction_remap_match_END    (4)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_FPB_literal_remap_match_START      (5)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_FPB_literal_remap_match_END        (5)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_FPB_BKPT_match_START               (6)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_FPB_BKPT_match_END                 (6)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_SysTick_interrupt_request_START    (7)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_SysTick_interrupt_request_END      (7)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_START                              (8)
#define SOC_AO_SCTRL_SC_MCU_SUBSYS_STAT7_mcu_subsys_state7_END                                (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_CLKEN4_UNION
 结构说明  : SC_PERIPH_CLKEN4 寄存器结构定义。地址偏移量:0x630，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON分区时钟使能寄存器4。对比特写1，使能SC_PERIPH_CLKSTAT4对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken4_0hclk_mcu        : 1;  /* bit[0] : 0hclk_mcu */
        unsigned int  periph_clken4_lreserved        : 1;  /* bit[1] : lreserved */
        unsigned int  periph_clken4_2reserved        : 1;  /* bit[2] : 2reserved */
        unsigned int  periph_clken4_3clk_mcu_dap     : 1;  /* bit[3] : 3clk_mcu_dap */
        unsigned int  periph_clken4_4pclk_cm3_timer0 : 1;  /* bit[4] : 4pclk_cm3_timer0 */
        unsigned int  periph_clken4_5pclk_cm3_timer1 : 1;  /* bit[5] : 5pclk_cm3_timer1 */
        unsigned int  periph_clken4_6pclk_cm3_wdt0   : 1;  /* bit[6] : 6pclk_cm3_wdt0 */
        unsigned int  periph_clken4_7pclk_cm3_wdt1   : 1;  /* bit[7] : 7pclk_cm3_wdt1 */
        unsigned int  periph_clken4_8hclk_ipc_s      : 1;  /* bit[8] : 8hclk_ipc_s for acpu config */
        unsigned int  periph_clken4_9hclk_ipc_ns     : 1;  /* bit[9] : 9hclk_ipc_ns for acpu config */
        unsigned int  periph_clken4_10pclk_efusec    : 1;  /* bit[10]: 10pclk_efusec for acpu config */
        unsigned int  periph_clken4_11pclk_tzpc      : 1;  /* bit[11]: 11pclk_tzpc */
        unsigned int  periph_clken4_12pclk_wdt0      : 1;  /* bit[12]: 12pclk_wdt0 */
        unsigned int  periph_clken4_13pclk_wdt1      : 1;  /* bit[13]: 13pclk_wdt1 */
        unsigned int  periph_clken4_14pclk_wdt2      : 1;  /* bit[14]: 14pclk_wdt2 */
        unsigned int  periph_clken4_15pclk_timer0    : 1;  /* bit[15]: 15pclk_timer0 */
        unsigned int  periph_clken4_16pclk_timer1    : 1;  /* bit[16]: 16pclk_timer1 */
        unsigned int  periph_clken4_17pclk_timer2    : 1;  /* bit[17]: 17pclk_timer2 */
        unsigned int  periph_clken4_18pclk_timer3    : 1;  /* bit[18]: 18pclk_timer3 */
        unsigned int  periph_clken4_19pclk_timer4    : 1;  /* bit[19]: 19pclk_timer4 */
        unsigned int  periph_clken4_20pclk_timer5    : 1;  /* bit[20]: 20pclk_timer5 */
        unsigned int  periph_clken4_21pclk_timer6    : 1;  /* bit[21]: 21pclk_timer6 */
        unsigned int  periph_clken4_22pclk_timer7    : 1;  /* bit[22]: 22pclk_timer7 */
        unsigned int  periph_clken4_23pclk_timer8    : 1;  /* bit[23]: 23pclk_timer8 */
        unsigned int  periph_clken4_24clk_uart0      : 1;  /* bit[24]: 24clk_uart0 */
        unsigned int  periph_clken4_25clk_rtc0       : 1;  /* bit[25]: 25clk_rtc0 */
        unsigned int  periph_clken4_26clk_rtc1       : 1;  /* bit[26]: 26clk_rtc1 */
        unsigned int  periph_clken4_27pclk_pmussi    : 1;  /* bit[27]: 27pclk_pmussi for acpu config */
        unsigned int  periph_clken4_28clk_jtag_auth  : 1;  /* bit[28]: 28clk_jtag_auth */
        unsigned int  periph_clken4_29clk_cs_dapb_on : 1;  /* bit[29]: 29clk_cs_dapb_on */
        unsigned int  periph_clken4_30clk_pdm        : 1;  /* bit[30]: 30clk_pdm */
        unsigned int  periph_clken4_31clk_ssi_pad    : 1;  /* bit[31]: 31clk_ssi_pad */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_CLKEN4_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_0hclk_mcu_START         (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_0hclk_mcu_END           (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_lreserved_START         (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_lreserved_END           (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_2reserved_START         (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_2reserved_END           (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_3clk_mcu_dap_START      (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_3clk_mcu_dap_END        (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_4pclk_cm3_timer0_START  (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_4pclk_cm3_timer0_END    (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_5pclk_cm3_timer1_START  (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_5pclk_cm3_timer1_END    (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_6pclk_cm3_wdt0_START    (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_6pclk_cm3_wdt0_END      (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_7pclk_cm3_wdt1_START    (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_7pclk_cm3_wdt1_END      (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_8hclk_ipc_s_START       (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_8hclk_ipc_s_END         (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_9hclk_ipc_ns_START      (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_9hclk_ipc_ns_END        (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_10pclk_efusec_START     (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_10pclk_efusec_END       (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_11pclk_tzpc_START       (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_11pclk_tzpc_END         (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_12pclk_wdt0_START       (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_12pclk_wdt0_END         (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_13pclk_wdt1_START       (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_13pclk_wdt1_END         (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_14pclk_wdt2_START       (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_14pclk_wdt2_END         (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_15pclk_timer0_START     (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_15pclk_timer0_END       (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_16pclk_timer1_START     (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_16pclk_timer1_END       (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_17pclk_timer2_START     (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_17pclk_timer2_END       (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_18pclk_timer3_START     (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_18pclk_timer3_END       (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_19pclk_timer4_START     (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_19pclk_timer4_END       (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_20pclk_timer5_START     (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_20pclk_timer5_END       (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_21pclk_timer6_START     (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_21pclk_timer6_END       (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_22pclk_timer7_START     (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_22pclk_timer7_END       (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_23pclk_timer8_START     (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_23pclk_timer8_END       (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_24clk_uart0_START       (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_24clk_uart0_END         (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_25clk_rtc0_START        (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_25clk_rtc0_END          (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_26clk_rtc1_START        (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_26clk_rtc1_END          (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_27pclk_pmussi_START     (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_27pclk_pmussi_END       (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_28clk_jtag_auth_START   (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_28clk_jtag_auth_END     (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_29clk_cs_dapb_on_START  (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_29clk_cs_dapb_on_END    (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_30clk_pdm_START         (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_30clk_pdm_END           (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_31clk_ssi_pad_START     (31)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_31clk_ssi_pad_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_UNION
 结构说明  : SC_PERIPH_CLKDIS4 寄存器结构定义。地址偏移量:0x634，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON分区时钟禁止寄存器4。对比特写1，禁止SC_PERIPH_CLKSTAT4对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis4_0hclk_mcu        : 1;  /* bit[0] : 0hclk_mcu */
        unsigned int  periph_clkdis4_lreserved        : 1;  /* bit[1] : lreserved */
        unsigned int  periph_clkdis4_2reserved        : 1;  /* bit[2] : 2reserved */
        unsigned int  periph_clkdis4_3clk_mcu_dap     : 1;  /* bit[3] : 3clk_mcu_dap */
        unsigned int  periph_clkdis4_4pclk_cm3_timer0 : 1;  /* bit[4] : 4pclk_cm3_timer0 */
        unsigned int  periph_clkdis4_5pclk_cm3_timer1 : 1;  /* bit[5] : 5pclk_cm3_timer1 */
        unsigned int  periph_clkdis4_6pclk_cm3_wdt0   : 1;  /* bit[6] : 6pclk_cm3_wdt0 */
        unsigned int  periph_clkdis4_7pclk_cm3_wdt1   : 1;  /* bit[7] : 7pclk_cm3_wdt1 */
        unsigned int  periph_clkdis4_8hclk_ipc_s      : 1;  /* bit[8] : 8hclk_ipc_s for acpu config */
        unsigned int  periph_clkdis4_9hclk_ipc_ns     : 1;  /* bit[9] : 9hclk_ipc_ns for acpu config */
        unsigned int  periph_clkdis4_10pclk_efusec    : 1;  /* bit[10]: 10pclk_efusec for acpu config */
        unsigned int  periph_clkdis4_11pclk_tzpc      : 1;  /* bit[11]: 11pclk_tzpc */
        unsigned int  periph_clkdis4_12pclk_wdt0      : 1;  /* bit[12]: 12pclk_wdt0 */
        unsigned int  periph_clkdis4_13pclk_wdt1      : 1;  /* bit[13]: 13pclk_wdt1 */
        unsigned int  periph_clkdis4_14pclk_wdt2      : 1;  /* bit[14]: 14pclk_wdt2 */
        unsigned int  periph_clkdis4_15pclk_timer0    : 1;  /* bit[15]: 15pclk_timer0 */
        unsigned int  periph_clkdis4_16pclk_timer1    : 1;  /* bit[16]: 16pclk_timer1 */
        unsigned int  periph_clkdis4_17pclk_timer2    : 1;  /* bit[17]: 17pclk_timer2 */
        unsigned int  periph_clkdis4_18pclk_timer3    : 1;  /* bit[18]: 18pclk_timer3 */
        unsigned int  periph_clkdis4_19pclk_timer4    : 1;  /* bit[19]: 19pclk_timer4 */
        unsigned int  periph_clkdis4_20pclk_timer5    : 1;  /* bit[20]: 20pclk_timer5 */
        unsigned int  periph_clkdis4_21pclk_timer6    : 1;  /* bit[21]: 21pclk_timer6 */
        unsigned int  periph_clkdis4_22pclk_timer7    : 1;  /* bit[22]: 22pclk_timer7 */
        unsigned int  periph_clkdis4_23pclk_timer8    : 1;  /* bit[23]: 23pclk_timer8 */
        unsigned int  periph_clkdis4_24clk_uart0      : 1;  /* bit[24]: 24clk_uart0 */
        unsigned int  periph_clkdis4_25clk_rtc0       : 1;  /* bit[25]: 25clk_rtc0 */
        unsigned int  periph_clkdis4_26clk_rtc1       : 1;  /* bit[26]: 26clk_rtc1 */
        unsigned int  periph_clkdis4_27pclk_pmussi    : 1;  /* bit[27]: 27pclk_pmussi for acpu config */
        unsigned int  periph_clkdis4_28clk_jtag_auth  : 1;  /* bit[28]: 28clk_jtag_auth */
        unsigned int  periph_clkdis4_29clk_cs_dapb_on : 1;  /* bit[29]: 29clk_cs_dapb_on */
        unsigned int  periph_clkdis4_30clk_pdm        : 1;  /* bit[30]: 30clk_pdm */
        unsigned int  periph_clkdis4_31clk_ssi_pad    : 1;  /* bit[31]: 31clk_ssi_pad */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_0hclk_mcu_START         (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_0hclk_mcu_END           (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_lreserved_START         (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_lreserved_END           (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_2reserved_START         (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_2reserved_END           (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_3clk_mcu_dap_START      (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_3clk_mcu_dap_END        (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_4pclk_cm3_timer0_START  (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_4pclk_cm3_timer0_END    (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_5pclk_cm3_timer1_START  (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_5pclk_cm3_timer1_END    (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_6pclk_cm3_wdt0_START    (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_6pclk_cm3_wdt0_END      (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_7pclk_cm3_wdt1_START    (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_7pclk_cm3_wdt1_END      (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_8hclk_ipc_s_START       (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_8hclk_ipc_s_END         (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_9hclk_ipc_ns_START      (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_9hclk_ipc_ns_END        (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_10pclk_efusec_START     (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_10pclk_efusec_END       (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_11pclk_tzpc_START       (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_11pclk_tzpc_END         (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_12pclk_wdt0_START       (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_12pclk_wdt0_END         (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_13pclk_wdt1_START       (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_13pclk_wdt1_END         (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_14pclk_wdt2_START       (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_14pclk_wdt2_END         (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_15pclk_timer0_START     (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_15pclk_timer0_END       (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_16pclk_timer1_START     (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_16pclk_timer1_END       (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_17pclk_timer2_START     (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_17pclk_timer2_END       (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_18pclk_timer3_START     (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_18pclk_timer3_END       (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_19pclk_timer4_START     (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_19pclk_timer4_END       (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_20pclk_timer5_START     (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_20pclk_timer5_END       (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_21pclk_timer6_START     (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_21pclk_timer6_END       (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_22pclk_timer7_START     (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_22pclk_timer7_END       (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_23pclk_timer8_START     (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_23pclk_timer8_END       (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_24clk_uart0_START       (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_24clk_uart0_END         (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_25clk_rtc0_START        (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_25clk_rtc0_END          (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_26clk_rtc1_START        (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_26clk_rtc1_END          (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_27pclk_pmussi_START     (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_27pclk_pmussi_END       (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_28clk_jtag_auth_START   (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_28clk_jtag_auth_END     (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_29clk_cs_dapb_on_START  (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_29clk_cs_dapb_on_END    (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_30clk_pdm_START         (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_30clk_pdm_END           (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_31clk_ssi_pad_START     (31)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_31clk_ssi_pad_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_UNION
 结构说明  : SC_PERIPH_CLKSTAT4 寄存器结构定义。地址偏移量:0x638，初值:0x31000C00，宽度:32
 寄存器说明: ALWAYSON分区时钟状态寄存器4。由SC_PERIPH_CLKEN4和SC_PERIPH_CLKDIS4配置进行状态修改。
            外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
            每个Bit含义：
            0：时钟关闭；
            1：时钟打开。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat4_0hclk_mcu        : 1;  /* bit[0] : 0hclk_mcu */
        unsigned int  periph_clkstat4_lreserved        : 1;  /* bit[1] : lreserved */
        unsigned int  periph_clkstat4_2reserved        : 1;  /* bit[2] : 2reserved */
        unsigned int  periph_clkstat4_3clk_mcu_dap     : 1;  /* bit[3] : 3clk_mcu_dap */
        unsigned int  periph_clkstat4_4pclk_cm3_timer0 : 1;  /* bit[4] : 4pclk_cm3_timer0 */
        unsigned int  periph_clkstat4_5pclk_cm3_timer1 : 1;  /* bit[5] : 5pclk_cm3_timer1 */
        unsigned int  periph_clkstat4_6pclk_cm3_wdt0   : 1;  /* bit[6] : 6pclk_cm3_wdt0 */
        unsigned int  periph_clkstat4_7pclk_cm3_wdt1   : 1;  /* bit[7] : 7pclk_cm3_wdt1 */
        unsigned int  periph_clkstat4_8hclk_ipc_s      : 1;  /* bit[8] : 8hclk_ipc_s for acpu config */
        unsigned int  periph_clkstat4_9hclk_ipc_ns     : 1;  /* bit[9] : 9hclk_ipc_ns for acpu config */
        unsigned int  periph_clkstat4_10pclk_efusec    : 1;  /* bit[10]: 10pclk_efusec for acpu config */
        unsigned int  periph_clkstat4_11pclk_tzpc      : 1;  /* bit[11]: 11pclk_tzpc */
        unsigned int  periph_clkstat4_12pclk_wdt0      : 1;  /* bit[12]: 12pclk_wdt0 */
        unsigned int  periph_clkstat4_13pclk_wdt1      : 1;  /* bit[13]: 13pclk_wdt1 */
        unsigned int  periph_clkstat4_14pclk_wdt2      : 1;  /* bit[14]: 14pclk_wdt2 */
        unsigned int  periph_clkstat4_15pclk_timer0    : 1;  /* bit[15]: 15pclk_timer0 */
        unsigned int  periph_clkstat4_16pclk_timer1    : 1;  /* bit[16]: 16pclk_timer1 */
        unsigned int  periph_clkstat4_17pclk_timer2    : 1;  /* bit[17]: 17pclk_timer2 */
        unsigned int  periph_clkstat4_18pclk_timer3    : 1;  /* bit[18]: 18pclk_timer3 */
        unsigned int  periph_clkstat4_19pclk_timer4    : 1;  /* bit[19]: 19pclk_timer4 */
        unsigned int  periph_clkstat4_20pclk_timer5    : 1;  /* bit[20]: 20pclk_timer5 */
        unsigned int  periph_clkstat4_21pclk_timer6    : 1;  /* bit[21]: 21pclk_timer6 */
        unsigned int  periph_clkstat4_22pclk_timer7    : 1;  /* bit[22]: 22pclk_timer7 */
        unsigned int  periph_clkstat4_23pclk_timer8    : 1;  /* bit[23]: 23pclk_timer8 */
        unsigned int  periph_clkstat4_24clk_uart0      : 1;  /* bit[24]: 24clk_uart0 */
        unsigned int  periph_clkstat4_25clk_rtc0       : 1;  /* bit[25]: 25clk_rtc0 */
        unsigned int  periph_clkstat4_26clk_rtc1       : 1;  /* bit[26]: 26clk_rtc1 */
        unsigned int  periph_clkstat4_27pclk_pmussi    : 1;  /* bit[27]: 27pclk_pmussi for acpu config */
        unsigned int  periph_clkstat4_28clk_jtag_auth  : 1;  /* bit[28]: 28clk_jtag_auth */
        unsigned int  periph_clkstat4_29clk_cs_dapb_on : 1;  /* bit[29]: 29clk_cs_dapb_on */
        unsigned int  periph_clkstat4_30clk_pdm        : 1;  /* bit[30]: 30clk_pdm */
        unsigned int  periph_clkstat4_31clk_ssi_pad    : 1;  /* bit[31]: 31clk_ssi_pad */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_0hclk_mcu_START         (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_0hclk_mcu_END           (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_lreserved_START         (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_lreserved_END           (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_2reserved_START         (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_2reserved_END           (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_3clk_mcu_dap_START      (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_3clk_mcu_dap_END        (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_4pclk_cm3_timer0_START  (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_4pclk_cm3_timer0_END    (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_5pclk_cm3_timer1_START  (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_5pclk_cm3_timer1_END    (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_6pclk_cm3_wdt0_START    (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_6pclk_cm3_wdt0_END      (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_7pclk_cm3_wdt1_START    (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_7pclk_cm3_wdt1_END      (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_8hclk_ipc_s_START       (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_8hclk_ipc_s_END         (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_9hclk_ipc_ns_START      (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_9hclk_ipc_ns_END        (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_10pclk_efusec_START     (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_10pclk_efusec_END       (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_11pclk_tzpc_START       (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_11pclk_tzpc_END         (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_12pclk_wdt0_START       (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_12pclk_wdt0_END         (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_13pclk_wdt1_START       (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_13pclk_wdt1_END         (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_14pclk_wdt2_START       (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_14pclk_wdt2_END         (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_15pclk_timer0_START     (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_15pclk_timer0_END       (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_16pclk_timer1_START     (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_16pclk_timer1_END       (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_17pclk_timer2_START     (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_17pclk_timer2_END       (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_18pclk_timer3_START     (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_18pclk_timer3_END       (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_19pclk_timer4_START     (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_19pclk_timer4_END       (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_20pclk_timer5_START     (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_20pclk_timer5_END       (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_21pclk_timer6_START     (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_21pclk_timer6_END       (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_22pclk_timer7_START     (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_22pclk_timer7_END       (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_23pclk_timer8_START     (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_23pclk_timer8_END       (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_24clk_uart0_START       (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_24clk_uart0_END         (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_25clk_rtc0_START        (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_25clk_rtc0_END          (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_26clk_rtc1_START        (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_26clk_rtc1_END          (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_27pclk_pmussi_START     (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_27pclk_pmussi_END       (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_28clk_jtag_auth_START   (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_28clk_jtag_auth_END     (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_29clk_cs_dapb_on_START  (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_29clk_cs_dapb_on_END    (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_30clk_pdm_START         (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_30clk_pdm_END           (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_31clk_ssi_pad_START     (31)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT4_periph_clkstat4_31clk_ssi_pad_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_CLKEN5_UNION
 结构说明  : SC_PERIPH_CLKEN5 寄存器结构定义。地址偏移量:0x63C，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON分区时钟使能寄存器5。对比特写1，使能SC_PERIPH_CLKSTAT5对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken5_0pclk_pmussi_ccpu : 1;  /* bit[0] : 0pclk_pmussi_ccpu */
        unsigned int  periph_clken5_1pclk_efusec_ccpu : 1;  /* bit[1] : 1pclk_efusec_ccpu */
        unsigned int  periph_clken5_2hclk_ipc_ccpu    : 1;  /* bit[2] : 2hclk_ipc_ccpu */
        unsigned int  periph_clken5_3hclk_ipc_ns_ccpu : 1;  /* bit[3] : 3hclk_ipc_ns_ccpu */
        unsigned int  periph_clken5_4reserved         : 1;  /* bit[4] : 4reserved */
        unsigned int  periph_clken5_5reserved         : 1;  /* bit[5] : 5reserved */
        unsigned int  periph_clken5_6reserved         : 1;  /* bit[6] : 6reserved */
        unsigned int  periph_clken5_7reserved         : 1;  /* bit[7] : 7reserved */
        unsigned int  periph_clken5_8reserved         : 1;  /* bit[8] : 8reserved */
        unsigned int  periph_clken5_9reserved         : 1;  /* bit[9] : 9reserved */
        unsigned int  periph_clken5_10reserved        : 1;  /* bit[10]: 10reserved */
        unsigned int  periph_clken5_11reserved        : 1;  /* bit[11]: 11reserved */
        unsigned int  periph_clken5_12reserved        : 1;  /* bit[12]: 12reserved */
        unsigned int  periph_clken5_13reserved        : 1;  /* bit[13]: 13reserved */
        unsigned int  periph_clken5_14reserved        : 1;  /* bit[14]: 14reserved */
        unsigned int  periph_clken5_15reserved        : 1;  /* bit[15]: 15reserved */
        unsigned int  periph_clken5_16pclk_pmussi_mcu : 1;  /* bit[16]: 16pclk_pmussi_mcu */
        unsigned int  periph_clken5_17pclk_efusec_mcu : 1;  /* bit[17]: 17pclk_efusec_mcu */
        unsigned int  periph_clken5_18hclk_ipc_mcu    : 1;  /* bit[18]: 18hclk_ipc_mcu */
        unsigned int  periph_clken5_19hclk_ipc_ns_mcu : 1;  /* bit[19]: 19hclk_ipc_ns_mcu */
        unsigned int  periph_clken5_20reserved        : 1;  /* bit[20]: 20reserved */
        unsigned int  periph_clken5_21reserved        : 1;  /* bit[21]: 21reserved */
        unsigned int  periph_clken5_22reserved        : 1;  /* bit[22]: 22reserved */
        unsigned int  periph_clken5_23reserved        : 1;  /* bit[23]: 23reserved */
        unsigned int  periph_clken5_24reserved        : 1;  /* bit[24]: 24reserved */
        unsigned int  periph_clken5_25reserved        : 1;  /* bit[25]: 25reserved */
        unsigned int  periph_clken5_26reserved        : 1;  /* bit[26]: 26reserved */
        unsigned int  periph_clken5_27reserved        : 1;  /* bit[27]: 27reserved */
        unsigned int  periph_clken5_28reserved        : 1;  /* bit[28]: 28reserved */
        unsigned int  periph_clken5_29reserved        : 1;  /* bit[29]: 29reserved */
        unsigned int  periph_clken5_30reserved        : 1;  /* bit[30]: 30reserved */
        unsigned int  periph_clken5_31reserved        : 1;  /* bit[31]: 31reserved */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_CLKEN5_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_0pclk_pmussi_ccpu_START  (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_0pclk_pmussi_ccpu_END    (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_1pclk_efusec_ccpu_START  (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_1pclk_efusec_ccpu_END    (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_2hclk_ipc_ccpu_START     (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_2hclk_ipc_ccpu_END       (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_3hclk_ipc_ns_ccpu_START  (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_3hclk_ipc_ns_ccpu_END    (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_4reserved_START          (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_4reserved_END            (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_5reserved_START          (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_5reserved_END            (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_6reserved_START          (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_6reserved_END            (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_7reserved_START          (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_7reserved_END            (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_8reserved_START          (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_8reserved_END            (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_9reserved_START          (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_9reserved_END            (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_10reserved_START         (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_10reserved_END           (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_11reserved_START         (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_11reserved_END           (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_12reserved_START         (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_12reserved_END           (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_13reserved_START         (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_13reserved_END           (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_14reserved_START         (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_14reserved_END           (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_15reserved_START         (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_15reserved_END           (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_16pclk_pmussi_mcu_START  (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_16pclk_pmussi_mcu_END    (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_17pclk_efusec_mcu_START  (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_17pclk_efusec_mcu_END    (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_18hclk_ipc_mcu_START     (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_18hclk_ipc_mcu_END       (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_19hclk_ipc_ns_mcu_START  (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_19hclk_ipc_ns_mcu_END    (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_20reserved_START         (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_20reserved_END           (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_21reserved_START         (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_21reserved_END           (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_22reserved_START         (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_22reserved_END           (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_23reserved_START         (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_23reserved_END           (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_24reserved_START         (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_24reserved_END           (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_25reserved_START         (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_25reserved_END           (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_26reserved_START         (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_26reserved_END           (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_27reserved_START         (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_27reserved_END           (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_28reserved_START         (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_28reserved_END           (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_29reserved_START         (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_29reserved_END           (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_30reserved_START         (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_30reserved_END           (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_31reserved_START         (31)
#define SOC_AO_SCTRL_SC_PERIPH_CLKEN5_periph_clken5_31reserved_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_UNION
 结构说明  : SC_PERIPH_CLKDIS5 寄存器结构定义。地址偏移量:0x640，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON分区时钟禁止寄存器5。对比特写1，禁止SC_PERIPH_CLKSTAT5对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis5_0pclk_pmussi_ccpu : 1;  /* bit[0] : 0pclk_pmussi_ccpu */
        unsigned int  periph_clkdis5_1pclk_efusec_ccpu : 1;  /* bit[1] : 1pclk_efusec_ccpu */
        unsigned int  periph_clkdis5_2hclk_ipc_ccpu    : 1;  /* bit[2] : 2hclk_ipc_ccpu */
        unsigned int  periph_clkdis5_3hclk_ipc_ns_ccpu : 1;  /* bit[3] : 3hclk_ipc_ns_ccpu */
        unsigned int  periph_clkdis5_4reserved         : 1;  /* bit[4] : 4reserved */
        unsigned int  periph_clkdis5_5reserved         : 1;  /* bit[5] : 5reserved */
        unsigned int  periph_clkdis5_6reserved         : 1;  /* bit[6] : 6reserved */
        unsigned int  periph_clkdis5_7reserved         : 1;  /* bit[7] : 7reserved */
        unsigned int  periph_clkdis5_8reserved         : 1;  /* bit[8] : 8reserved */
        unsigned int  periph_clkdis5_9reserved         : 1;  /* bit[9] : 9reserved */
        unsigned int  periph_clkdis5_10reserved        : 1;  /* bit[10]: 10reserved */
        unsigned int  periph_clkdis5_11reserved        : 1;  /* bit[11]: 11reserved */
        unsigned int  periph_clkdis5_12reserved        : 1;  /* bit[12]: 12reserved */
        unsigned int  periph_clkdis5_13reserved        : 1;  /* bit[13]: 13reserved */
        unsigned int  periph_clkdis5_14reserved        : 1;  /* bit[14]: 14reserved */
        unsigned int  periph_clkdis5_15reserved        : 1;  /* bit[15]: 15reserved */
        unsigned int  periph_clkdis5_16pclk_pmussi_mcu : 1;  /* bit[16]: 16pclk_pmussi_mcu */
        unsigned int  periph_clkdis5_17pclk_efusec_mcu : 1;  /* bit[17]: 17pclk_efusec_mcu */
        unsigned int  periph_clkdis5_18hclk_ipc_mcu    : 1;  /* bit[18]: 18hclk_ipc_mcu */
        unsigned int  periph_clkdis5_19hclk_ipc_ns_mcu : 1;  /* bit[19]: 19hclk_ipc_ns_mcu */
        unsigned int  periph_clkdis5_20reserved        : 1;  /* bit[20]: 20reserved */
        unsigned int  periph_clkdis5_21reserved        : 1;  /* bit[21]: 21reserved */
        unsigned int  periph_clkdis5_22reserved        : 1;  /* bit[22]: 22reserved */
        unsigned int  periph_clkdis5_23reserved        : 1;  /* bit[23]: 23reserved */
        unsigned int  periph_clkdis5_24reserved        : 1;  /* bit[24]: 24reserved */
        unsigned int  periph_clkdis5_25reserved        : 1;  /* bit[25]: 25reserved */
        unsigned int  periph_clkdis5_26reserved        : 1;  /* bit[26]: 26reserved */
        unsigned int  periph_clkdis5_27reserved        : 1;  /* bit[27]: 27reserved */
        unsigned int  periph_clkdis5_28reserved        : 1;  /* bit[28]: 28reserved */
        unsigned int  periph_clkdis5_29reserved        : 1;  /* bit[29]: 29reserved */
        unsigned int  periph_clkdis5_30reserved        : 1;  /* bit[30]: 30reserved */
        unsigned int  periph_clkdis5_31reserved        : 1;  /* bit[31]: 31reserved */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_0pclk_pmussi_ccpu_START  (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_0pclk_pmussi_ccpu_END    (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_1pclk_efusec_ccpu_START  (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_1pclk_efusec_ccpu_END    (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_2hclk_ipc_ccpu_START     (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_2hclk_ipc_ccpu_END       (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_3hclk_ipc_ns_ccpu_START  (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_3hclk_ipc_ns_ccpu_END    (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_4reserved_START          (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_4reserved_END            (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_5reserved_START          (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_5reserved_END            (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_6reserved_START          (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_6reserved_END            (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_7reserved_START          (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_7reserved_END            (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_8reserved_START          (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_8reserved_END            (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_9reserved_START          (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_9reserved_END            (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_10reserved_START         (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_10reserved_END           (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_11reserved_START         (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_11reserved_END           (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_12reserved_START         (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_12reserved_END           (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_13reserved_START         (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_13reserved_END           (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_14reserved_START         (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_14reserved_END           (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_15reserved_START         (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_15reserved_END           (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_16pclk_pmussi_mcu_START  (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_16pclk_pmussi_mcu_END    (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_17pclk_efusec_mcu_START  (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_17pclk_efusec_mcu_END    (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_18hclk_ipc_mcu_START     (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_18hclk_ipc_mcu_END       (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_19hclk_ipc_ns_mcu_START  (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_19hclk_ipc_ns_mcu_END    (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_20reserved_START         (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_20reserved_END           (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_21reserved_START         (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_21reserved_END           (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_22reserved_START         (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_22reserved_END           (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_23reserved_START         (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_23reserved_END           (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_24reserved_START         (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_24reserved_END           (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_25reserved_START         (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_25reserved_END           (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_26reserved_START         (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_26reserved_END           (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_27reserved_START         (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_27reserved_END           (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_28reserved_START         (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_28reserved_END           (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_29reserved_START         (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_29reserved_END           (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_30reserved_START         (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_30reserved_END           (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_31reserved_START         (31)
#define SOC_AO_SCTRL_SC_PERIPH_CLKDIS5_periph_clkdis5_31reserved_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_UNION
 结构说明  : SC_PERIPH_CLKSTAT5 寄存器结构定义。地址偏移量:0x644，初值:0x00020002，宽度:32
 寄存器说明: ALWAYSON分区时钟状态寄存器5。由SC_PERIPH_CLKEN5和SC_PERIPH_CLKDIS5配置进行状态修改。
            外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
            每个Bit含义：
            0：时钟关闭；
            1：时钟打开。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat5_0pclk_pmussi_ccpu : 1;  /* bit[0] : 0pclk_pmussi_ccpu */
        unsigned int  periph_clkstat5_1pclk_efusec_ccpu : 1;  /* bit[1] : 1pclk_efusec_ccpu */
        unsigned int  periph_clkstat5_2hclk_ipc_ccpu    : 1;  /* bit[2] : 2hclk_ipc_ccpu */
        unsigned int  periph_clkstat5_3hclk_ipc_ns_ccpu : 1;  /* bit[3] : 3hclk_ipc_ns_ccpu */
        unsigned int  periph_clkstat5_4reserved         : 1;  /* bit[4] : 4reserved */
        unsigned int  periph_clkstat5_5reserved         : 1;  /* bit[5] : 5reserved */
        unsigned int  periph_clkstat5_6reserved         : 1;  /* bit[6] : 6reserved */
        unsigned int  periph_clkstat5_7reserved         : 1;  /* bit[7] : 7reserved */
        unsigned int  periph_clkstat5_8reserved         : 1;  /* bit[8] : 8reserved */
        unsigned int  periph_clkstat5_9reserved         : 1;  /* bit[9] : 9reserved */
        unsigned int  periph_clkstat5_10reserved        : 1;  /* bit[10]: 10reserved */
        unsigned int  periph_clkstat5_11reserved        : 1;  /* bit[11]: 11reserved */
        unsigned int  periph_clkstat5_12reserved        : 1;  /* bit[12]: 12reserved */
        unsigned int  periph_clkstat5_13reserved        : 1;  /* bit[13]: 13reserved */
        unsigned int  periph_clkstat5_14reserved        : 1;  /* bit[14]: 14reserved */
        unsigned int  periph_clkstat5_15reserved        : 1;  /* bit[15]: 15reserved */
        unsigned int  periph_clkstat5_16pclk_pmussi_mcu : 1;  /* bit[16]: 16pclk_pmussi_mcu */
        unsigned int  periph_clkstat5_17pclk_efusec_mcu : 1;  /* bit[17]: 17pclk_efusec_mcu */
        unsigned int  periph_clkstat5_18hclk_ipc_mcu    : 1;  /* bit[18]: 18hclk_ipc_mcu */
        unsigned int  periph_clkstat5_19hclk_ipc_ns_mcu : 1;  /* bit[19]: 19hclk_ipc_ns_mcu */
        unsigned int  periph_clkstat5_20reserved        : 1;  /* bit[20]: 20reserved */
        unsigned int  periph_clkstat5_21reserved        : 1;  /* bit[21]: 21reserved */
        unsigned int  periph_clkstat5_22reserved        : 1;  /* bit[22]: 22reserved */
        unsigned int  periph_clkstat5_23reserved        : 1;  /* bit[23]: 23reserved */
        unsigned int  periph_clkstat5_24reserved        : 1;  /* bit[24]: 24reserved */
        unsigned int  periph_clkstat5_25reserved        : 1;  /* bit[25]: 25reserved */
        unsigned int  periph_clkstat5_26reserved        : 1;  /* bit[26]: 26reserved */
        unsigned int  periph_clkstat5_27reserved        : 1;  /* bit[27]: 27reserved */
        unsigned int  periph_clkstat5_28reserved        : 1;  /* bit[28]: 28reserved */
        unsigned int  periph_clkstat5_29reserved        : 1;  /* bit[29]: 29reserved */
        unsigned int  periph_clkstat5_30reserved        : 1;  /* bit[30]: 30reserved */
        unsigned int  periph_clkstat5_31reserved        : 1;  /* bit[31]: 31reserved */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_0pclk_pmussi_ccpu_START  (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_0pclk_pmussi_ccpu_END    (0)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_1pclk_efusec_ccpu_START  (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_1pclk_efusec_ccpu_END    (1)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_2hclk_ipc_ccpu_START     (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_2hclk_ipc_ccpu_END       (2)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_3hclk_ipc_ns_ccpu_START  (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_3hclk_ipc_ns_ccpu_END    (3)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_4reserved_START          (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_4reserved_END            (4)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_5reserved_START          (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_5reserved_END            (5)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_6reserved_START          (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_6reserved_END            (6)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_7reserved_START          (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_7reserved_END            (7)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_8reserved_START          (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_8reserved_END            (8)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_9reserved_START          (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_9reserved_END            (9)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_10reserved_START         (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_10reserved_END           (10)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_11reserved_START         (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_11reserved_END           (11)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_12reserved_START         (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_12reserved_END           (12)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_13reserved_START         (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_13reserved_END           (13)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_14reserved_START         (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_14reserved_END           (14)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_15reserved_START         (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_15reserved_END           (15)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_16pclk_pmussi_mcu_START  (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_16pclk_pmussi_mcu_END    (16)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_17pclk_efusec_mcu_START  (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_17pclk_efusec_mcu_END    (17)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_18hclk_ipc_mcu_START     (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_18hclk_ipc_mcu_END       (18)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_19hclk_ipc_ns_mcu_START  (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_19hclk_ipc_ns_mcu_END    (19)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_20reserved_START         (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_20reserved_END           (20)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_21reserved_START         (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_21reserved_END           (21)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_22reserved_START         (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_22reserved_END           (22)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_23reserved_START         (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_23reserved_END           (23)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_24reserved_START         (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_24reserved_END           (24)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_25reserved_START         (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_25reserved_END           (25)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_26reserved_START         (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_26reserved_END           (26)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_27reserved_START         (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_27reserved_END           (27)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_28reserved_START         (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_28reserved_END           (28)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_29reserved_START         (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_29reserved_END           (29)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_30reserved_START         (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_30reserved_END           (30)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_31reserved_START         (31)
#define SOC_AO_SCTRL_SC_PERIPH_CLKSTAT5_periph_clkstat5_31reserved_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_RSTEN4_UNION
 结构说明  : SC_PERIPH_RSTEN4 寄存器结构定义。地址偏移量:0x6F0，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON分区复位使能寄存器4。用于使能SC_PERIPH_RSTSTAT4复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rsten4_0reset_mcu_ectr_n    : 1;  /* bit[0] : 0reset_mcu_ectr_n */
        unsigned int  periph_rsten4_1reset_mcu_sys_n     : 1;  /* bit[1] : 1reset_mcu_sys_n */
        unsigned int  periph_rsten4_2reset_mcu_por_n     : 1;  /* bit[2] : 2reset_mcu_por_n */
        unsigned int  periph_rsten4_3reset_mcu_dap_n     : 1;  /* bit[3] : 3reset_mcu_dap_n */
        unsigned int  periph_rsten4_4preset_cm3_timer0_n : 1;  /* bit[4] : 4preset_cm3_timer0_n */
        unsigned int  periph_rsten4_5preset_cm3_timer1_n : 1;  /* bit[5] : 5preset_cm3_timer1_n */
        unsigned int  periph_rsten4_6preset_cm3_wdt0_n   : 1;  /* bit[6] : 6preset_cm3_wdt0_n */
        unsigned int  periph_rsten4_7preset_cm3_wdt1_n   : 1;  /* bit[7] : 7preset_cm3_wdt1_n */
        unsigned int  periph_rsten4_8hreset_ipc_s_n      : 1;  /* bit[8] : 8hreset_ipc_s_n */
        unsigned int  periph_rsten4_9hreset_ipc_ns_n     : 1;  /* bit[9] : 9hreset_ipc_ns_n */
        unsigned int  periph_rsten4_10preset_efusec_n    : 1;  /* bit[10]: 10preset_efusec_n */
        unsigned int  periph_rsten4_11reserved           : 1;  /* bit[11]: 11reserved */
        unsigned int  periph_rsten4_12preset_wdt0_n      : 1;  /* bit[12]: 12preset_wdt0_n */
        unsigned int  periph_rsten4_13preset_wdt1_n      : 1;  /* bit[13]: 13preset_wdt1_n */
        unsigned int  periph_rsten4_14preset_wdt2_n      : 1;  /* bit[14]: 14preset_wdt2_n */
        unsigned int  periph_rsten4_15preset_timer0_n    : 1;  /* bit[15]: 15preset_timer0_n */
        unsigned int  periph_rsten4_16preset_timer1_n    : 1;  /* bit[16]: 16preset_timer1_n */
        unsigned int  periph_rsten4_17preset_timer2_n    : 1;  /* bit[17]: 17preset_timer2_n */
        unsigned int  periph_rsten4_18preset_timer3_n    : 1;  /* bit[18]: 18preset_timer3_n */
        unsigned int  periph_rsten4_19preset_timer4_n    : 1;  /* bit[19]: 19preset_timer4_n */
        unsigned int  periph_rsten4_20preset_timer5_n    : 1;  /* bit[20]: 20preset_timer5_n */
        unsigned int  periph_rsten4_21preset_timer6_n    : 1;  /* bit[21]: 21preset_timer6_n */
        unsigned int  periph_rsten4_22preset_timer7_n    : 1;  /* bit[22]: 22preset_timer7_n */
        unsigned int  periph_rsten4_23preset_timer8_n    : 1;  /* bit[23]: 23preset_timer8_n */
        unsigned int  periph_rsten4_24preset_uart0_n     : 1;  /* bit[24]: 24preset_uart0_n */
        unsigned int  periph_rsten4_25reset_rtc0_n       : 1;  /* bit[25]: 25reset_rtc0_n */
        unsigned int  periph_rsten4_26reset_rtc1_n       : 1;  /* bit[26]: 26reset_rtc1_n */
        unsigned int  periph_rsten4_27preset_pmussi_n    : 1;  /* bit[27]: 27preset_pmussi_n */
        unsigned int  periph_rsten4_28reset_jtag_auth_n  : 1;  /* bit[28]: 28reset_jtag_auth_n */
        unsigned int  periph_rsten4_29reset_cs_dapb_on_n : 1;  /* bit[29]: 29reset_cs_dapb_on_n */
        unsigned int  periph_rsten4_30mdm_subsys_glb     : 1;  /* bit[30]: 30mdm_subsys_glb Modem子系统全局复位 */
        unsigned int  periph_rsten4_31reserved           : 1;  /* bit[31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_RSTEN4_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_0reset_mcu_ectr_n_START     (0)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_0reset_mcu_ectr_n_END       (0)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_1reset_mcu_sys_n_START      (1)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_1reset_mcu_sys_n_END        (1)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_2reset_mcu_por_n_START      (2)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_2reset_mcu_por_n_END        (2)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_3reset_mcu_dap_n_START      (3)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_3reset_mcu_dap_n_END        (3)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_4preset_cm3_timer0_n_START  (4)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_4preset_cm3_timer0_n_END    (4)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_5preset_cm3_timer1_n_START  (5)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_5preset_cm3_timer1_n_END    (5)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_6preset_cm3_wdt0_n_START    (6)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_6preset_cm3_wdt0_n_END      (6)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_7preset_cm3_wdt1_n_START    (7)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_7preset_cm3_wdt1_n_END      (7)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_8hreset_ipc_s_n_START       (8)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_8hreset_ipc_s_n_END         (8)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_9hreset_ipc_ns_n_START      (9)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_9hreset_ipc_ns_n_END        (9)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_10preset_efusec_n_START     (10)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_10preset_efusec_n_END       (10)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_11reserved_START            (11)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_11reserved_END              (11)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_12preset_wdt0_n_START       (12)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_12preset_wdt0_n_END         (12)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_13preset_wdt1_n_START       (13)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_13preset_wdt1_n_END         (13)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_14preset_wdt2_n_START       (14)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_14preset_wdt2_n_END         (14)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_15preset_timer0_n_START     (15)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_15preset_timer0_n_END       (15)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_16preset_timer1_n_START     (16)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_16preset_timer1_n_END       (16)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_17preset_timer2_n_START     (17)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_17preset_timer2_n_END       (17)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_18preset_timer3_n_START     (18)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_18preset_timer3_n_END       (18)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_19preset_timer4_n_START     (19)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_19preset_timer4_n_END       (19)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_20preset_timer5_n_START     (20)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_20preset_timer5_n_END       (20)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_21preset_timer6_n_START     (21)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_21preset_timer6_n_END       (21)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_22preset_timer7_n_START     (22)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_22preset_timer7_n_END       (22)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_23preset_timer8_n_START     (23)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_23preset_timer8_n_END       (23)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_24preset_uart0_n_START      (24)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_24preset_uart0_n_END        (24)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_25reset_rtc0_n_START        (25)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_25reset_rtc0_n_END          (25)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_26reset_rtc1_n_START        (26)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_26reset_rtc1_n_END          (26)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_27preset_pmussi_n_START     (27)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_27preset_pmussi_n_END       (27)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_28reset_jtag_auth_n_START   (28)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_28reset_jtag_auth_n_END     (28)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_29reset_cs_dapb_on_n_START  (29)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_29reset_cs_dapb_on_n_END    (29)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_30mdm_subsys_glb_START      (30)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_30mdm_subsys_glb_END        (30)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_31reserved_START            (31)
#define SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_31reserved_END              (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_UNION
 结构说明  : SC_PERIPH_RSTDIS4 寄存器结构定义。地址偏移量:0x6F4，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON分区设复位禁止寄存器4。用于禁止SC_PERIPH_RSTSTAT4复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rstdis4_0reset_mcu_ectr_n    : 1;  /* bit[0] : 0reset_mcu_ectr_n */
        unsigned int  periph_rstdis4_1reset_mcu_sys_n     : 1;  /* bit[1] : 1reset_mcu_sys_n */
        unsigned int  periph_rstdis4_2reset_mcu_por_n     : 1;  /* bit[2] : 2reset_mcu_por_n */
        unsigned int  periph_rstdis4_3reset_mcu_dap_n     : 1;  /* bit[3] : 3reset_mcu_dap_n */
        unsigned int  periph_rstdis4_4preset_cm3_timer0_n : 1;  /* bit[4] : 4preset_cm3_timer0_n */
        unsigned int  periph_rstdis4_5preset_cm3_timer1_n : 1;  /* bit[5] : 5preset_cm3_timer1_n */
        unsigned int  periph_rstdis4_6preset_cm3_wdt0_n   : 1;  /* bit[6] : 6preset_cm3_wdt0_n */
        unsigned int  periph_rstdis4_7preset_cm3_wdt1_n   : 1;  /* bit[7] : 7preset_cm3_wdt1_n */
        unsigned int  periph_rstdis4_8hreset_ipc_s_n      : 1;  /* bit[8] : 8hreset_ipc_s_n */
        unsigned int  periph_rstdis4_9hreset_ipc_ns_n     : 1;  /* bit[9] : 9hreset_ipc_ns_n */
        unsigned int  periph_rstdis4_10preset_efusec_n    : 1;  /* bit[10]: 10preset_efusec_n */
        unsigned int  periph_rstdis4_11reserved           : 1;  /* bit[11]: 11reserved */
        unsigned int  periph_rstdis4_12preset_wdt0_n      : 1;  /* bit[12]: 12preset_wdt0_n */
        unsigned int  periph_rstdis4_13preset_wdt1_n      : 1;  /* bit[13]: 13preset_wdt1_n */
        unsigned int  periph_rstdis4_14preset_wdt2_n      : 1;  /* bit[14]: 14preset_wdt2_n */
        unsigned int  periph_rstdis4_15preset_timer0_n    : 1;  /* bit[15]: 15preset_timer0_n */
        unsigned int  periph_rstdis4_16preset_timer1_n    : 1;  /* bit[16]: 16preset_timer1_n */
        unsigned int  periph_rstdis4_17preset_timer2_n    : 1;  /* bit[17]: 17preset_timer2_n */
        unsigned int  periph_rstdis4_18preset_timer3_n    : 1;  /* bit[18]: 18preset_timer3_n */
        unsigned int  periph_rstdis4_19preset_timer4_n    : 1;  /* bit[19]: 19preset_timer4_n */
        unsigned int  periph_rstdis4_20preset_timer5_n    : 1;  /* bit[20]: 20preset_timer5_n */
        unsigned int  periph_rstdis4_21preset_timer6_n    : 1;  /* bit[21]: 21preset_timer6_n */
        unsigned int  periph_rstdis4_22preset_timer7_n    : 1;  /* bit[22]: 22preset_timer7_n */
        unsigned int  periph_rstdis4_23preset_timer8_n    : 1;  /* bit[23]: 23preset_timer8_n */
        unsigned int  periph_rstdis4_24preset_uart0_n     : 1;  /* bit[24]: 24preset_uart0_n */
        unsigned int  periph_rstdis4_25reset_rtc0_n       : 1;  /* bit[25]: 25reset_rtc0_n */
        unsigned int  periph_rstdis4_26reset_rtc1_n       : 1;  /* bit[26]: 26reset_rtc1_n */
        unsigned int  periph_rstdis4_27preset_pmussi_n    : 1;  /* bit[27]: 27preset_pmussi_n */
        unsigned int  periph_rstdis4_28reset_jtag_auth_n  : 1;  /* bit[28]: 28reset_jtag_auth_n */
        unsigned int  periph_rstdis4_29reset_cs_dapb_on_n : 1;  /* bit[29]: 29reset_cs_dapb_on_n */
        unsigned int  periph_rstdis4_30mdm_subsys_glb     : 1;  /* bit[30]: 30mdm_subsys_glb Modem子系统全局复位 */
        unsigned int  periph_rstdis4_31reserved           : 1;  /* bit[31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_0reset_mcu_ectr_n_START     (0)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_0reset_mcu_ectr_n_END       (0)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_1reset_mcu_sys_n_START      (1)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_1reset_mcu_sys_n_END        (1)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_2reset_mcu_por_n_START      (2)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_2reset_mcu_por_n_END        (2)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_3reset_mcu_dap_n_START      (3)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_3reset_mcu_dap_n_END        (3)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_4preset_cm3_timer0_n_START  (4)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_4preset_cm3_timer0_n_END    (4)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_5preset_cm3_timer1_n_START  (5)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_5preset_cm3_timer1_n_END    (5)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_6preset_cm3_wdt0_n_START    (6)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_6preset_cm3_wdt0_n_END      (6)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_7preset_cm3_wdt1_n_START    (7)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_7preset_cm3_wdt1_n_END      (7)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_8hreset_ipc_s_n_START       (8)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_8hreset_ipc_s_n_END         (8)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_9hreset_ipc_ns_n_START      (9)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_9hreset_ipc_ns_n_END        (9)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_10preset_efusec_n_START     (10)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_10preset_efusec_n_END       (10)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_11reserved_START            (11)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_11reserved_END              (11)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_12preset_wdt0_n_START       (12)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_12preset_wdt0_n_END         (12)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_13preset_wdt1_n_START       (13)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_13preset_wdt1_n_END         (13)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_14preset_wdt2_n_START       (14)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_14preset_wdt2_n_END         (14)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_15preset_timer0_n_START     (15)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_15preset_timer0_n_END       (15)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_16preset_timer1_n_START     (16)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_16preset_timer1_n_END       (16)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_17preset_timer2_n_START     (17)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_17preset_timer2_n_END       (17)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_18preset_timer3_n_START     (18)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_18preset_timer3_n_END       (18)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_19preset_timer4_n_START     (19)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_19preset_timer4_n_END       (19)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_20preset_timer5_n_START     (20)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_20preset_timer5_n_END       (20)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_21preset_timer6_n_START     (21)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_21preset_timer6_n_END       (21)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_22preset_timer7_n_START     (22)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_22preset_timer7_n_END       (22)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_23preset_timer8_n_START     (23)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_23preset_timer8_n_END       (23)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_24preset_uart0_n_START      (24)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_24preset_uart0_n_END        (24)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_25reset_rtc0_n_START        (25)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_25reset_rtc0_n_END          (25)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_26reset_rtc1_n_START        (26)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_26reset_rtc1_n_END          (26)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_27preset_pmussi_n_START     (27)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_27preset_pmussi_n_END       (27)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_28reset_jtag_auth_n_START   (28)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_28reset_jtag_auth_n_END     (28)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_29reset_cs_dapb_on_n_START  (29)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_29reset_cs_dapb_on_n_END    (29)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_30mdm_subsys_glb_START      (30)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_30mdm_subsys_glb_END        (30)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_31reserved_START            (31)
#define SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_periph_rstdis4_31reserved_END              (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_UNION
 结构说明  : SC_PERIPH_RSTSTAT4 寄存器结构定义。地址偏移量:0x6F8，初值:0x0000000F，宽度:32
 寄存器说明: ALWAYSON分区设复位状态寄存器4。由SC_PERIPH_RSTEN4和SC_PERIPH_RSTDIS4配置进行状态修改。解复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
            每个Bit含义：
            0：复位不使能；
            1：复位使能。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rststat4_0reset_mcu_ectr_n    : 1;  /* bit[0] : 0reset_mcu_ectr_n */
        unsigned int  periph_rststat4_1reset_mcu_sys_n     : 1;  /* bit[1] : 1reset_mcu_sys_n */
        unsigned int  periph_rststat4_2reset_mcu_por_n     : 1;  /* bit[2] : 2reset_mcu_por_n */
        unsigned int  periph_rststat4_3reset_mcu_dap_n     : 1;  /* bit[3] : 3reset_mcu_dap_n */
        unsigned int  periph_rststat4_4preset_cm3_timer0_n : 1;  /* bit[4] : 4preset_cm3_timer0_n */
        unsigned int  periph_rststat4_5preset_cm3_timer1_n : 1;  /* bit[5] : 5preset_cm3_timer1_n */
        unsigned int  periph_rststat4_6preset_cm3_wdt0_n   : 1;  /* bit[6] : 6preset_cm3_wdt0_n */
        unsigned int  periph_rststat4_7preset_cm3_wdt1_n   : 1;  /* bit[7] : 7preset_cm3_wdt1_n */
        unsigned int  periph_rststat4_8hreset_ipc_s_n      : 1;  /* bit[8] : 8hreset_ipc_s_n */
        unsigned int  periph_rststat4_9hreset_ipc_ns_n     : 1;  /* bit[9] : 9hreset_ipc_ns_n */
        unsigned int  periph_rststat4_10preset_efusec_n    : 1;  /* bit[10]: 10preset_efusec_n */
        unsigned int  periph_rststat4_11reserved           : 1;  /* bit[11]: 11reserved */
        unsigned int  periph_rststat4_12preset_wdt0_n      : 1;  /* bit[12]: 12preset_wdt0_n */
        unsigned int  periph_rststat4_13preset_wdt1_n      : 1;  /* bit[13]: 13preset_wdt1_n */
        unsigned int  periph_rststat4_14preset_wdt2_n      : 1;  /* bit[14]: 14preset_wdt2_n */
        unsigned int  periph_rststat4_15preset_timer0_n    : 1;  /* bit[15]: 15preset_timer0_n */
        unsigned int  periph_rststat4_16preset_timer1_n    : 1;  /* bit[16]: 16preset_timer1_n */
        unsigned int  periph_rststat4_17preset_timer2_n    : 1;  /* bit[17]: 17preset_timer2_n */
        unsigned int  periph_rststat4_18preset_timer3_n    : 1;  /* bit[18]: 18preset_timer3_n */
        unsigned int  periph_rststat4_19preset_timer4_n    : 1;  /* bit[19]: 19preset_timer4_n */
        unsigned int  periph_rststat4_20preset_timer5_n    : 1;  /* bit[20]: 20preset_timer5_n */
        unsigned int  periph_rststat4_21preset_timer6_n    : 1;  /* bit[21]: 21preset_timer6_n */
        unsigned int  periph_rststat4_22preset_timer7_n    : 1;  /* bit[22]: 22preset_timer7_n */
        unsigned int  periph_rststat4_23preset_timer8_n    : 1;  /* bit[23]: 23preset_timer8_n */
        unsigned int  periph_rststat4_24preset_uart0_n     : 1;  /* bit[24]: 24preset_uart0_n */
        unsigned int  periph_rststat4_25reset_rtc0_n       : 1;  /* bit[25]: 25reset_rtc0_n */
        unsigned int  periph_rststat4_26reset_rtc1_n       : 1;  /* bit[26]: 26reset_rtc1_n */
        unsigned int  periph_rststat4_27preset_pmussi_n    : 1;  /* bit[27]: 27preset_pmussi_n */
        unsigned int  periph_rststat4_28reset_jtag_auth_n  : 1;  /* bit[28]: 28reset_jtag_auth_n */
        unsigned int  periph_rststat4_29reset_cs_dapb_on_n : 1;  /* bit[29]: 29reset_cs_dapb_on_n */
        unsigned int  periph_rststat4_30mdm_subsys_glb     : 1;  /* bit[30]: 30mdm_subsys_glb Modem子系统全局复位 */
        unsigned int  periph_rststat4_31reserved           : 1;  /* bit[31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_0reset_mcu_ectr_n_START     (0)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_0reset_mcu_ectr_n_END       (0)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_1reset_mcu_sys_n_START      (1)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_1reset_mcu_sys_n_END        (1)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_2reset_mcu_por_n_START      (2)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_2reset_mcu_por_n_END        (2)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_3reset_mcu_dap_n_START      (3)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_3reset_mcu_dap_n_END        (3)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_4preset_cm3_timer0_n_START  (4)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_4preset_cm3_timer0_n_END    (4)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_5preset_cm3_timer1_n_START  (5)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_5preset_cm3_timer1_n_END    (5)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_6preset_cm3_wdt0_n_START    (6)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_6preset_cm3_wdt0_n_END      (6)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_7preset_cm3_wdt1_n_START    (7)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_7preset_cm3_wdt1_n_END      (7)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_8hreset_ipc_s_n_START       (8)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_8hreset_ipc_s_n_END         (8)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_9hreset_ipc_ns_n_START      (9)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_9hreset_ipc_ns_n_END        (9)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_10preset_efusec_n_START     (10)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_10preset_efusec_n_END       (10)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_11reserved_START            (11)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_11reserved_END              (11)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_12preset_wdt0_n_START       (12)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_12preset_wdt0_n_END         (12)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_13preset_wdt1_n_START       (13)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_13preset_wdt1_n_END         (13)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_14preset_wdt2_n_START       (14)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_14preset_wdt2_n_END         (14)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_15preset_timer0_n_START     (15)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_15preset_timer0_n_END       (15)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_16preset_timer1_n_START     (16)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_16preset_timer1_n_END       (16)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_17preset_timer2_n_START     (17)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_17preset_timer2_n_END       (17)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_18preset_timer3_n_START     (18)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_18preset_timer3_n_END       (18)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_19preset_timer4_n_START     (19)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_19preset_timer4_n_END       (19)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_20preset_timer5_n_START     (20)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_20preset_timer5_n_END       (20)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_21preset_timer6_n_START     (21)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_21preset_timer6_n_END       (21)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_22preset_timer7_n_START     (22)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_22preset_timer7_n_END       (22)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_23preset_timer8_n_START     (23)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_23preset_timer8_n_END       (23)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_24preset_uart0_n_START      (24)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_24preset_uart0_n_END        (24)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_25reset_rtc0_n_START        (25)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_25reset_rtc0_n_END          (25)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_26reset_rtc1_n_START        (26)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_26reset_rtc1_n_END          (26)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_27preset_pmussi_n_START     (27)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_27preset_pmussi_n_END       (27)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_28reset_jtag_auth_n_START   (28)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_28reset_jtag_auth_n_END     (28)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_29reset_cs_dapb_on_n_START  (29)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_29reset_cs_dapb_on_n_END    (29)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_30mdm_subsys_glb_START      (30)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_30mdm_subsys_glb_END        (30)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_31reserved_START            (31)
#define SOC_AO_SCTRL_SC_PERIPH_RSTSTAT4_periph_rststat4_31reserved_END              (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_RSTEN5_UNION
 结构说明  : SC_PERIPH_RSTEN5 寄存器结构定义。地址偏移量:0x6FC，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON分区复位使能寄存器5。用于使能SC_PERIPH_RSTSTAT5复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 复位使能定义如下（写1复位使能）。
                                                   [31:0] ： Reserved */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_RSTEN5_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_RSTDIS5_UNION
 结构说明  : SC_PERIPH_RSTDIS5 寄存器结构定义。地址偏移量:0x700，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON分区设复位禁止寄存器5。用于禁止SC_PERIPH_RSTSTAT5复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 复位禁止定义如下（写1复位解除）。
                                                   [31:0] ： Reserved */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_RSTDIS5_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERIPH_RSTSTAT5_UNION
 结构说明  : SC_PERIPH_RSTSTAT5 寄存器结构定义。地址偏移量:0x704，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON分区设复位状态寄存器5。由SC_PERIPH_RSTEN5和SC_PERIPH_RSTDIS5配置进行状态修改。解复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
            每个Bit含义：
            0：复位不使能；
            1：复位使能。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 复位状态定义如下（高电平表示处于复位）。
                                                   [31:0] ： Reserved */
    } reg;
} SOC_AO_SCTRL_SC_PERIPH_RSTSTAT5_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_CLKEN0_UNION
 结构说明  : SC_PW_CLKEN0 寄存器结构定义。地址偏移量:0x800，初值:0x00000000，宽度:32
 寄存器说明: 掉电区时钟使能寄存器。用于控制各掉电区总的时钟使能。对相应位写1使能SC_PW_CLK_STAT0相应电压域时钟，写0无影响。该寄存器只在上电复位时被复位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_clken0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_clken0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_clken0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_clken0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_clken0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_clken0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_clken0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_clken0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_clken0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_clken0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_clken0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_clken0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_clken0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_clken0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_clken0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_clken0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_clken0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_CLKEN0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_CLKDIS0_UNION
 结构说明  : SC_PW_CLKDIS0 寄存器结构定义。地址偏移量:0x804，初值:0x00000000，宽度:32
 寄存器说明: 掉电区时钟禁止寄存器。用于控制各掉电区总的时钟禁止。对相应位写1禁止SC_PW_CLK_STAT0相应掉电区时钟，写0无影响。该寄存器只在上电复位时被复位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_clkdis0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_clkdis0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_clkdis0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_clkdis0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_clkdis0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_clkdis0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_clkdis0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_clkdis0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_clkdis0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_clkdis0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_clkdis0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_clkdis0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_clkdis0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_clkdis0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_clkdis0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_clkdis0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_clkdis0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_CLKDIS0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_CLKDIS0_pw_clkdis0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_CLK_STAT0_UNION
 结构说明  : SC_PW_CLK_STAT0 寄存器结构定义。地址偏移量:0x808，初值:0x00003001，宽度:32
 寄存器说明: 下电区时钟状态，通过SC_PW_CLKEN0和SC_PW_CLKDIS0修改状态。
            每个Bit含义：
            0：关闭；
            1：打开。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_clk_stat0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_clk_stat0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_clk_stat0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_clk_stat0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_clk_stat0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_clk_stat0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_clk_stat0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_clk_stat0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_clk_stat0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_clk_stat0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_clk_stat0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_clk_stat0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_clk_stat0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_clk_stat0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_clk_stat0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_clk_stat0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_clk_stat0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_CLK_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_CLK_STAT0_pw_clk_stat0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_RSTEN0_UNION
 结构说明  : SC_PW_RSTEN0 寄存器结构定义。地址偏移量:0x810，初值:0x00000000，宽度:32
 寄存器说明: 掉电区复位使能寄存器。用于控制各掉电区总的复位使能。对相应位写1使能SC_PW_RST_STAT0相应掉电区复位信号（即进入复位态），写0无影响。该寄存器只在上电复位时被复位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_rsten0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_rsten0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_rsten0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_rsten0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_rsten0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_rsten0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_rsten0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_rsten0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_rsten0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_rsten0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_rsten0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_rsten0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_rsten0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_rsten0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_rsten0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_rsten0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_rsten0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_RSTEN0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_RSTEN0_pw_rsten0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_RSTDIS0_UNION
 结构说明  : SC_PW_RSTDIS0 寄存器结构定义。地址偏移量:0x814，初值:0x00000000，宽度:32
 寄存器说明: 掉电区复位禁止寄存器。用于控制各掉电区总的复位禁止。对相应位写1禁止SC_PW_RST_STAT0相应掉电区复位信号（即退出复位态），写0无影响。该寄存器只在上电复位时被复位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_rstdis0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_rstdis0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_rstdis0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_rstdis0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_rstdis0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_rstdis0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_rstdis0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_rstdis0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_rstdis0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_rstdis0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_rstdis0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_rstdis0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_rstdis0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_rstdis0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_rstdis0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_rstdis0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_rstdis0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_RSTDIS0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_RST_STAT0_UNION
 结构说明  : SC_PW_RST_STAT0 寄存器结构定义。地址偏移量:0x818，初值:0xFFFFCFFE，宽度:32
 寄存器说明: 下电区复位状态，通过SC_PW_RSTEN0和SC_PW_RSTDIS0修改状态。
            每个Bit含义：
            0：复位不使能；
            1：复位使能。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_rst_stat0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_rst_stat0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_rst_stat0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_rst_stat0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_rst_stat0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_rst_stat0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_rst_stat0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_rst_stat0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_rst_stat0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_rst_stat0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_rst_stat0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_rst_stat0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_rst_stat0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_rst_stat0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_rst_stat0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_rst_stat0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_rst_stat0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_RST_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_RST_STAT0_pw_rst_stat0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_ISOEN0_UNION
 结构说明  : SC_PW_ISOEN0 寄存器结构定义。地址偏移量:0x820，初值:0x00000000，宽度:32
 寄存器说明: 掉电区ISO使能寄存器。用于控制各掉电区相关的Isolation Cell使能。对相应位写1使能SC_PW_ISO_STAT0相应掉电区IsoLation Cell（即信号隔断），写0无影响。该寄存器只在上电复位时被复位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_isoen0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_isoen0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_isoen0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_isoen0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_isoen0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_isoen0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_isoen0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_isoen0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_isoen0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_isoen0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_isoen0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_isoen0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_isoen0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_isoen0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_isoen0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_isoen0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_isoen0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_ISOEN0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_ISODIS0_UNION
 结构说明  : SC_PW_ISODIS0 寄存器结构定义。地址偏移量:0x824，初值:0x00000000，宽度:32
 寄存器说明: 掉电区ISO禁止寄存器。用于控制各掉电区相关的Isolation Cell禁止。对相应位写1禁止SC_PW_ISO_STAT0相应掉电区IsoLation Cell（即信号不隔断），写0无影响。该寄存器只在上电复位时被复位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_isodis0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_isodis0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_isodis0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_isodis0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_isodis0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_isodis0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_isodis0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_isodis0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_isodis0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_isodis0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_isodis0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_isodis0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_isodis0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_isodis0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_isodis0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_isodis0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_isodis0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_ISODIS0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_ISO_STAT0_UNION
 结构说明  : SC_PW_ISO_STAT0 寄存器结构定义。地址偏移量:0x828，初值:0xFFFFCFFE，宽度:32
 寄存器说明: 下电区ISO状态，通过SC_PW_ISOEN0和SC_PW_ISODIS0修改状态。
            每个Bit含义：
            0：isolation不使能；
            1：isolation使能。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_iso_stat0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_iso_stat0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_iso_stat0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_iso_stat0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_iso_stat0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_iso_stat0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_iso_stat0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_iso_stat0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_iso_stat0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_iso_stat0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_iso_stat0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_iso_stat0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_iso_stat0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_iso_stat0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_iso_stat0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_iso_stat0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_iso_stat0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_ISO_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_ISO_STAT0_pw_iso_stat0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_UNION
 结构说明  : SC_PW_MTCMOS_EN0 寄存器结构定义。地址偏移量:0x830，初值:0x00000000，宽度:32
 寄存器说明: 掉电区电源使能寄存器。用于控制各掉电区的电源使能，对相应位写1打开SC_PW_MTCMOS_STAT0相应电压域电源，写0无影响。该寄存器存只在上电复位时被复位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_en0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_en0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_en0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_en0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_en0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_en0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_en0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_en0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_en0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_en0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_en0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_en0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_en0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_en0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_en0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_en0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_en0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_EN0_pw_en0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_UNION
 结构说明  : SC_PW_MTCMOS_DIS0 寄存器结构定义。地址偏移量:0x834，初值:0x00000000，宽度:32
 寄存器说明: 掉电区电源禁止寄存器。用于控制各掉电区的电源禁止，对相应位写1关闭SC_PW_MTCMOS_STAT0相应电压域电源，写0无影响。该寄存器存只在上电复位时被复位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_dis0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_dis0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_dis0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_dis0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_dis0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_dis0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_dis0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_dis0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_dis0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_dis0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_dis0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_dis0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_dis0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_dis0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_dis0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_dis0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_dis0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_DIS0_pw_dis0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_UNION
 结构说明  : SC_PW_MTCMOS_STAT0 寄存器结构定义。地址偏移量:0x838，初值:0xFFFFFFFF，宽度:32
 寄存器说明: 下电区MTCMOS配置状态，通过SC_PW_EN0和SC_PW_DIS0修改MTCMOS控制状态。
            每个Bit含义：
            0：电源关闭；
            1：电源打开。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_mtcmos_ack_stat0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_mtcmos_ack_stat0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_mtcmos_ack_stat0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_mtcmos_ack_stat0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_mtcmos_ack_stat0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_mtcmos_ack_stat0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_mtcmos_ack_stat0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_mtcmos_ack_stat0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_mtcmos_ack_stat0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_STAT0_pw_mtcmos_ack_stat0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_UNION
 结构说明  : SC_PW_MTCMOS_ACK_STAT0 寄存器结构定义。地址偏移量:0x83C，初值:0x0001FFFF，宽度:32
 寄存器说明: 下电区MTCMOS状态，通过SC_PW_EN0和SC_PW_DIS0修改MTCMOS控制状态，再经过掉电分区反馈回来。
            每个Bit含义：
            0：电源关闭；
            1：电源打开。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_mtcmos_ack_stat0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_mtcmos_ack_stat0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_mtcmos_ack_stat0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_mtcmos_ack_stat0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_mtcmos_ack_stat0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_mtcmos_ack_stat0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_mtcmos_ack_stat0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_mtcmos_ack_stat0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_mtcmos_ack_stat0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_mtcmos_ack_stat0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_ACK_STAT0_pw_mtcmos_ack_stat0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_UNION
 结构说明  : SC_PW_MTCMOS_TIMEOUT_STAT0 寄存器结构定义。地址偏移量:0x840，初值:0x00000000，宽度:32
 寄存器说明: 系统POWER状态寄存器1。用于监测系统所处的POWER状态。
            0：电源稳定计数器未溢出；
            1：电源稳定计数器，MTCMOS或外部电源稳定已状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_mtcmos_timeout_stat0_0peri         : 1;  /* bit[0]    : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。 */
        unsigned int  pw_mtcmos_timeout_stat0_1g3d          : 1;  /* bit[1]    : [1] ：G3D下电区。 */
        unsigned int  pw_mtcmos_timeout_stat0_2codecisp     : 1;  /* bit[2]    : [2] ：CODEC+ISP下电区。  */
        unsigned int  pw_mtcmos_timeout_stat0_3reserved     : 1;  /* bit[3]    : [3] ：reserved */
        unsigned int  pw_mtcmos_timeout_stat0_4mcpu         : 1;  /* bit[4]    : [4] ：MCPU下电区。  */
        unsigned int  pw_mtcmos_timeout_stat0_5bbpharqmem   : 1;  /* bit[5]    : [5] ：BBP Harq Memory所在下电分区。  */
        unsigned int  pw_mtcmos_timeout_stat0_6reserved     : 1;  /* bit[6]    : [6] ：reserved  */
        unsigned int  pw_mtcmos_timeout_stat0_7reserved     : 1;  /* bit[7]    : [7] ：reserved */
        unsigned int  pw_mtcmos_timeout_stat0_8hifi         : 1;  /* bit[8]    : [8] ：HIFI下电区 。 */
        unsigned int  pw_mtcmos_timeout_stat0_9reserved     : 1;  /* bit[9]    : [9] ：reserved */
        unsigned int  pw_mtcmos_timeout_stat0_10reserved    : 1;  /* bit[10]   : [10] ：reserved */
        unsigned int  pw_mtcmos_timeout_stat0_11reserved    : 1;  /* bit[11]   : [11] ：reserved */
        unsigned int  pw_mtcmos_timeout_stat0_12acpucluster : 1;  /* bit[12]   : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。 */
        unsigned int  pw_mtcmos_timeout_stat0_13reserved    : 1;  /* bit[13]   : [13] ：reserved */
        unsigned int  pw_mtcmos_timeout_stat0_14reserved    : 1;  /* bit[14]   : [14] ：reserved */
        unsigned int  pw_mtcmos_timeout_stat0_15reserved    : 1;  /* bit[15]   : [15] ：reserved */
        unsigned int  pw_mtcmos_timeout_stat0_31reserved    : 16; /* bit[16-31]: [31:16] ：reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_0peri_START          (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_0peri_END            (0)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_1g3d_START           (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_1g3d_END             (1)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_2codecisp_START      (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_2codecisp_END        (2)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_3reserved_START      (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_3reserved_END        (3)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_4mcpu_START          (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_4mcpu_END            (4)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_5bbpharqmem_START    (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_5bbpharqmem_END      (5)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_6reserved_START      (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_6reserved_END        (6)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_7reserved_START      (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_7reserved_END        (7)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_8hifi_START          (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_8hifi_END            (8)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_9reserved_START      (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_9reserved_END        (9)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_10reserved_START     (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_10reserved_END       (10)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_11reserved_START     (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_11reserved_END       (11)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_12acpucluster_START  (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_12acpucluster_END    (12)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_13reserved_START     (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_13reserved_END       (13)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_14reserved_START     (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_14reserved_END       (14)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_15reserved_START     (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_15reserved_END       (15)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_31reserved_START     (16)
#define SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_31reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_STAT0_UNION
 结构说明  : SC_PW_STAT0 寄存器结构定义。地址偏移量:0x850，初值:0x03008000，宽度:32
 寄存器说明: 系统POWER状态0寄存器，用于监测系统所处的POWER状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  buck3_rise_cnt          : 8;  /* bit[0-7]  : Buck3上升沿检测次数，表示外设区掉电次数统计，计数范围0~255。 */
        unsigned int  hifi2mcu_wakeup_int_out : 1;  /* bit[8]    : HIFI唤醒中断状态寄存器。
                                                                   0：无唤醒中断；
                                                                   1：有唤醒中断。 */
        unsigned int  mdm_wakeup_int          : 1;  /* bit[9]    : CCPU唤醒中断状态寄存器。
                                                                   0：无唤醒中断；
                                                                   1：有唤醒中断。 */
        unsigned int  acpu2mcu_wakeup_int_out : 1;  /* bit[10]   : ACPU唤醒中断状态寄存器。
                                                                   0：无唤醒中断；
                                                                   1：有唤醒中断。 */
        unsigned int  mcu_wakeup_int_out      : 1;  /* bit[11]   : SOC系统唤醒中断状态寄存器，包括ACPU、MCU、MDM和HiFi唤醒中断总和。
                                                                   0：无唤醒中断；
                                                                   1：有唤醒中断。 */
        unsigned int  soc_pw_up_stat          : 1;  /* bit[12]   : 指示外设下电区是否为掉电后重新启动。
                                                                   0：系统上电启动；
                                                                   1：掉电后重新启动。 */
        unsigned int  mcu2mcu_wakeup_int_out  : 1;  /* bit[13]   : MCU唤醒中断状态寄存器。
                                                                   0：无唤醒中断；
                                                                   1：有唤醒中断。 */
        unsigned int  pll_sw_ok               : 1;  /* bit[14]   : pll_sw_ok状态寄存器。
                                                                   0：切换未完成；
                                                                   1：切换完成。 */
        unsigned int  xtal_sw_ok              : 1;  /* bit[15]   : xtal_sw_ok状态寄存器。
                                                                   0：切换未完成；
                                                                   1：切换完成。 */
        unsigned int  reserved_0              : 8;  /* bit[16-23]: reserved */
        unsigned int  SLEEPHOLDREQn           : 1;  /* bit[24]   : MCU SLEEPHOLDREQn
                                                                   0： req extend the sleep state；
                                                                   1： not req extend the sleep state。 */
        unsigned int  SLEEPHOLDACKn           : 1;  /* bit[25]   : MCU SLEEPHOLDACKn
                                                                   0： ack extend the sleep state；
                                                                   1： ack not extend the sleep state。 */
        unsigned int  reserved_1              : 2;  /* bit[26-27]: reserved */
        unsigned int  pw_stat                 : 4;  /* bit[28-31]: 系统电压控制状态机当前状态指示。
                                                                   0x0：PWON；
                                                                   0x1：PWDNCLK；
                                                                   0x2：PWDNISO；
                                                                   0x3：PWDNRST；
                                                                   0x4：MTCMOFF；
                                                                   0x5：PWOFF；
                                                                   0x6：MTCMON；
                                                                   0x7：PWONISO；
                                                                   0x8：PWONCLK；
                                                                   0x9：PWONRST；
                                                                   0xB：PWDNRET；
                                                                   0xD：PWONRET；
                                                                   0xF：RETSTB；
                                                                   其它：reserved。 */
    } reg;
} SOC_AO_SCTRL_SC_PW_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_STAT0_buck3_rise_cnt_START           (0)
#define SOC_AO_SCTRL_SC_PW_STAT0_buck3_rise_cnt_END             (7)
#define SOC_AO_SCTRL_SC_PW_STAT0_hifi2mcu_wakeup_int_out_START  (8)
#define SOC_AO_SCTRL_SC_PW_STAT0_hifi2mcu_wakeup_int_out_END    (8)
#define SOC_AO_SCTRL_SC_PW_STAT0_mdm_wakeup_int_START           (9)
#define SOC_AO_SCTRL_SC_PW_STAT0_mdm_wakeup_int_END             (9)
#define SOC_AO_SCTRL_SC_PW_STAT0_acpu2mcu_wakeup_int_out_START  (10)
#define SOC_AO_SCTRL_SC_PW_STAT0_acpu2mcu_wakeup_int_out_END    (10)
#define SOC_AO_SCTRL_SC_PW_STAT0_mcu_wakeup_int_out_START       (11)
#define SOC_AO_SCTRL_SC_PW_STAT0_mcu_wakeup_int_out_END         (11)
#define SOC_AO_SCTRL_SC_PW_STAT0_soc_pw_up_stat_START           (12)
#define SOC_AO_SCTRL_SC_PW_STAT0_soc_pw_up_stat_END             (12)
#define SOC_AO_SCTRL_SC_PW_STAT0_mcu2mcu_wakeup_int_out_START   (13)
#define SOC_AO_SCTRL_SC_PW_STAT0_mcu2mcu_wakeup_int_out_END     (13)
#define SOC_AO_SCTRL_SC_PW_STAT0_pll_sw_ok_START                (14)
#define SOC_AO_SCTRL_SC_PW_STAT0_pll_sw_ok_END                  (14)
#define SOC_AO_SCTRL_SC_PW_STAT0_xtal_sw_ok_START               (15)
#define SOC_AO_SCTRL_SC_PW_STAT0_xtal_sw_ok_END                 (15)
#define SOC_AO_SCTRL_SC_PW_STAT0_SLEEPHOLDREQn_START            (24)
#define SOC_AO_SCTRL_SC_PW_STAT0_SLEEPHOLDREQn_END              (24)
#define SOC_AO_SCTRL_SC_PW_STAT0_SLEEPHOLDACKn_START            (25)
#define SOC_AO_SCTRL_SC_PW_STAT0_SLEEPHOLDACKn_END              (25)
#define SOC_AO_SCTRL_SC_PW_STAT0_pw_stat_START                  (28)
#define SOC_AO_SCTRL_SC_PW_STAT0_pw_stat_END                    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_STAT1_UNION
 结构说明  : SC_PW_STAT1 寄存器结构定义。地址偏移量:0x854，初值:0x00000000，宽度:32
 寄存器说明: 系统POWER状态1寄存器，POWER DOMAIN IDLEACK状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0             : 8;  /* bit[0-7]  : reserved */
        unsigned int  mdm_nsp_domain_idleack : 1;  /* bit[8]    : mdm_nsp_domain_idleack
                                                                  0： idle req not ack；
                                                                  1： idle req ack。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  mdm_nsp_domain_idle    : 1;  /* bit[9]    : mdm_nsp_domain_idle
                                                                  0： domian not idle；
                                                                  1： domain in idle。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  dma_domain_idleack     : 1;  /* bit[10]   : dma_domain_idleack
                                                                  0： idle req not ack；
                                                                  1： idle req ack。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  dma_domain_idle        : 1;  /* bit[11]   : dma_domain_idle
                                                                  0： domian not idle；
                                                                  1： domain in idle。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  ao_domain_idleack      : 1;  /* bit[12]   : ao_domain_idleack
                                                                  0： idle req not ack；
                                                                  1： idle req ack。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  ao_domain_idle         : 1;  /* bit[13]   : ao_domain_idle
                                                                  0： domian not idle；
                                                                  1： domain in idle。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  sysnoc_domain_idleack  : 1;  /* bit[14]   : sysnoc_domain_idleack
                                                                  0： idle req not ack
                                                                  1： idle req ack
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid */
        unsigned int  sysnoc_domain_idle     : 1;  /* bit[15]   : sysnoc_domain_idle
                                                                  0： domian not idle；
                                                                  1： domain in idle。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  g3d_domain_idleack     : 1;  /* bit[16]   : g3d_domain_idleack
                                                                  0： idle req not ack；
                                                                  1： idle req ack。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  g3d_domain_idle        : 1;  /* bit[17]   : g3d_domain_idle
                                                                  0： domian not idle；
                                                                  1： domain in idle。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  ccpu_domain_idleack    : 1;  /* bit[18]   : ccpu_domain_idleack
                                                                  0： idle req not ack；
                                                                  1： idle req ack。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  ccpu_domain_idle       : 1;  /* bit[19]   : ccpu_domain_idle
                                                                  0： domian not idle
                                                                  1： domain in idle
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid */
        unsigned int  mnoc_domain_idleack    : 1;  /* bit[20]   : mnoc_domain_idleack
                                                                  0： idle req not ack；
                                                                  1： idle req ack。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  mnoc_domain_idle       : 1;  /* bit[21]   : mnoc_domain_idle
                                                                  0： domian not idle；
                                                                  1： domain in idle。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  video_domain_idleack   : 1;  /* bit[22]   : video_domain_idleack
                                                                  0： idle req not ack；
                                                                  1： idle req ack。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  video_domain_idle      : 1;  /* bit[23]   : video_domain_idle
                                                                  0： domian not idle；
                                                                  1： domain in idle。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  ade_domain_idleack     : 1;  /* bit[24]   : ade_domain_idleack
                                                                  0： idle req not ack；
                                                                  1： idle req ack。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  ade_domain_idle        : 1;  /* bit[25]   : ade_domain_idle
                                                                  0： domian not idle；
                                                                  1： domain in idle。
                                                                  when this domain pwdn or rst,it still can be accessed if idleack and idle both valid。 */
        unsigned int  reserved_1             : 6;  /* bit[26-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_STAT1_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idleack_START  (8)
#define SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idleack_END    (8)
#define SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idle_START     (9)
#define SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idle_END       (9)
#define SOC_AO_SCTRL_SC_PW_STAT1_dma_domain_idleack_START      (10)
#define SOC_AO_SCTRL_SC_PW_STAT1_dma_domain_idleack_END        (10)
#define SOC_AO_SCTRL_SC_PW_STAT1_dma_domain_idle_START         (11)
#define SOC_AO_SCTRL_SC_PW_STAT1_dma_domain_idle_END           (11)
#define SOC_AO_SCTRL_SC_PW_STAT1_ao_domain_idleack_START       (12)
#define SOC_AO_SCTRL_SC_PW_STAT1_ao_domain_idleack_END         (12)
#define SOC_AO_SCTRL_SC_PW_STAT1_ao_domain_idle_START          (13)
#define SOC_AO_SCTRL_SC_PW_STAT1_ao_domain_idle_END            (13)
#define SOC_AO_SCTRL_SC_PW_STAT1_sysnoc_domain_idleack_START   (14)
#define SOC_AO_SCTRL_SC_PW_STAT1_sysnoc_domain_idleack_END     (14)
#define SOC_AO_SCTRL_SC_PW_STAT1_sysnoc_domain_idle_START      (15)
#define SOC_AO_SCTRL_SC_PW_STAT1_sysnoc_domain_idle_END        (15)
#define SOC_AO_SCTRL_SC_PW_STAT1_g3d_domain_idleack_START      (16)
#define SOC_AO_SCTRL_SC_PW_STAT1_g3d_domain_idleack_END        (16)
#define SOC_AO_SCTRL_SC_PW_STAT1_g3d_domain_idle_START         (17)
#define SOC_AO_SCTRL_SC_PW_STAT1_g3d_domain_idle_END           (17)
#define SOC_AO_SCTRL_SC_PW_STAT1_ccpu_domain_idleack_START     (18)
#define SOC_AO_SCTRL_SC_PW_STAT1_ccpu_domain_idleack_END       (18)
#define SOC_AO_SCTRL_SC_PW_STAT1_ccpu_domain_idle_START        (19)
#define SOC_AO_SCTRL_SC_PW_STAT1_ccpu_domain_idle_END          (19)
#define SOC_AO_SCTRL_SC_PW_STAT1_mnoc_domain_idleack_START     (20)
#define SOC_AO_SCTRL_SC_PW_STAT1_mnoc_domain_idleack_END       (20)
#define SOC_AO_SCTRL_SC_PW_STAT1_mnoc_domain_idle_START        (21)
#define SOC_AO_SCTRL_SC_PW_STAT1_mnoc_domain_idle_END          (21)
#define SOC_AO_SCTRL_SC_PW_STAT1_video_domain_idleack_START    (22)
#define SOC_AO_SCTRL_SC_PW_STAT1_video_domain_idleack_END      (22)
#define SOC_AO_SCTRL_SC_PW_STAT1_video_domain_idle_START       (23)
#define SOC_AO_SCTRL_SC_PW_STAT1_video_domain_idle_END         (23)
#define SOC_AO_SCTRL_SC_PW_STAT1_ade_domain_idleack_START      (24)
#define SOC_AO_SCTRL_SC_PW_STAT1_ade_domain_idleack_END        (24)
#define SOC_AO_SCTRL_SC_PW_STAT1_ade_domain_idle_START         (25)
#define SOC_AO_SCTRL_SC_PW_STAT1_ade_domain_idle_END           (25)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SYSTEST_STAT_UNION
 结构说明  : SC_SYSTEST_STAT 寄存器结构定义。地址偏移量:0x880，初值:0x00000009，宽度:32
 寄存器说明: 系统状态寄存器1，用于监测系统的状态。狗复位请求、JTAG模式、nand_boot_en、efuse_hw_rd_finish、usim_hpd_sta等系统状态查询。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  efuse_hw_rd_finish          : 1;  /* bit[0]    : efuse数据读取完成指示信号。
                                                                       0：未完成上电时efuse数据读取；
                                                                       1：已完成上电时efuse数据读取。 */
        unsigned int  nand_boot_en                : 1;  /* bit[1]    : 启动模式查询
                                                                       0： 片内Onchiprom启动；
                                                                       1： 片外指示NAND启动，仅用于ATE测试，仅在boot_mode_lock[0]&amp;boot_sel_lock为1'b1有效。 */
        unsigned int  jtag_trst_n_pin             : 1;  /* bit[2]    : JTAG复位状态寄存器。
                                                                       0：复位；
                                                                       1：不复位。 */
        unsigned int  pmu_rstin_n_ff2             : 1;  /* bit[3]    : 系统复位去抖前指示，仅供调试用
                                                                       0：系统复位；
                                                                       1：系统解复位；
                                                                       注：此接口软件不可见，已删除 */
        unsigned int  usim0_hpd_de_bounce_sel_ff3 : 1;  /* bit[4]    : usim0_hpd管脚输入去抖后状态
                                                                       0：IO输入1'b0；
                                                                       1：IO输入1'b1。 */
        unsigned int  usim1_hpd_de_bounce_sel_ff3 : 1;  /* bit[5]    : usim1_hpd管脚输入去抖后状态
                                                                       0：IO输入1'b0；
                                                                       1：IO输入1'b1。 */
        unsigned int  usim0_hpd_oen               : 1;  /* bit[6]    : usim0_hpd管脚OEN输出状态
                                                                       0：OEN输入1'b0；
                                                                       1：OEN输入1'b1。 */
        unsigned int  usim1_hpd_oen               : 1;  /* bit[7]    : usim1_hpd管脚OEN输出状态
                                                                       0：OEN输入1'b0；
                                                                       1：OEN输入1'b1。 */
        unsigned int  reserved_0                  : 8;  /* bit[8-15] : reserved */
        unsigned int  wdg_rst_req                 : 10; /* bit[16-25]: wdg reset req state
                                                                       [9]： reserved；
                                                                       [8]： reserved；
                                                                       [7]： reserved；
                                                                       [6]： reserved；
                                                                       [5]： reserved；
                                                                       [4]： reserved；
                                                                       [3]： 备份Watchdog复位请求中断。
                                                                       [2]： HIFI Watchdog复位请求中断。
                                                                       [1]： CM3 Watchdog0复位请求中断。
                                                                       [0]： ACPU 外部Watchdog复位请求中断。 */
        unsigned int  reserved_1                  : 6;  /* bit[26-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SYSTEST_STAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_efuse_hw_rd_finish_START           (0)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_efuse_hw_rd_finish_END             (0)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_nand_boot_en_START                 (1)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_nand_boot_en_END                   (1)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_jtag_trst_n_pin_START              (2)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_jtag_trst_n_pin_END                (2)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_pmu_rstin_n_ff2_START              (3)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_pmu_rstin_n_ff2_END                (3)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_usim0_hpd_de_bounce_sel_ff3_START  (4)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_usim0_hpd_de_bounce_sel_ff3_END    (4)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_usim1_hpd_de_bounce_sel_ff3_START  (5)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_usim1_hpd_de_bounce_sel_ff3_END    (5)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_usim0_hpd_oen_START                (6)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_usim0_hpd_oen_END                  (6)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_usim1_hpd_oen_START                (7)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_usim1_hpd_oen_END                  (7)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_wdg_rst_req_START                  (16)
#define SOC_AO_SCTRL_SC_SYSTEST_STAT_wdg_rst_req_END                    (25)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_UNION
 结构说明  : SC_SYSTEST_SLICER_CNT0 寄存器结构定义。地址偏移量:0x890，初值:0x00000000，宽度:32
 寄存器说明: 64bit递增计数器的低32bit，SLICERl_CNT[31:0]，32KHz参考时钟。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  slicer_cnt : 32; /* bit[0-31]: 64bit递增计数器的低32bit，SLICERl_CNT[31:0]，32KHz参考时钟。 */
    } reg;
} SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_slicer_cnt_START  (0)
#define SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_slicer_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_UNION
 结构说明  : SC_SYSTEST_SLICER_CNT1 寄存器结构定义。地址偏移量:0x894，初值:0x00000000，宽度:32
 寄存器说明: 64bit递增计数器的高32bit，SLICERl_CNT[63:32]，32KHz参考时钟。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  slicer_cnt_high : 32; /* bit[0-31]: 64bit递增计数器的高32bit，SLICERl_CNT[63:32]，32KHz参考时钟。 */
    } reg;
} SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_UNION;
#endif
#define SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_slicer_cnt_high_START  (0)
#define SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_slicer_cnt_high_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_CTRL1_UNION
 结构说明  : SC_PW_CTRL1 寄存器结构定义。地址偏移量:0x8C8，初值:0x00000000，宽度:32
 寄存器说明: 低功耗控制寄存器1，功能包括分区掉电后的总线DOMAIN IDLE请求。
            [31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_sft_fiq_req           : 1;  /* bit[0]    : acpu soft fiq req,when this bit changed from 1'b0 to 1'b1，ao sc internal int sc_int_en_sta_4acpu_sft_fiq occure。 */
        unsigned int  ccpu_sft_fiq_req           : 1;  /* bit[1]    : ccpu soft fiq req,when this bit changed from 1'b0 to 1'b1，ao sc internal int sc_int_en_sta_31ccpu_sft_fiq occure。 */
        unsigned int  reserved_0                 : 5;  /* bit[2-6]  : reserved */
        unsigned int  mdm_nsp_domain_idlereq     : 1;  /* bit[7]    : mdm_nsp_domain_idlereq
                                                                      0： normal mode；
                                                                      1： idle req when this domain idle,power down or rst。 */
        unsigned int  dma_domain_idlereq         : 1;  /* bit[8]    : dma_domain_idlereq
                                                                      0： normal mode；
                                                                      1： idle req when this domain idle,power down or rst。 */
        unsigned int  ao_domain_idlereq          : 1;  /* bit[9]    : ao_domain_idlereq (this bit is logic or with another idlreq from peri_sc)
                                                                      0： normal mode；
                                                                      1： hardware produce idle req when muc wdg0 rst req announce this domain abnormal。 */
        unsigned int  sysnoc_domain_idlereq      : 1;  /* bit[10]   : sysnoc_domain_idlereq
                                                                      0： normal mode；
                                                                      1： idle req when this domain idle,power down or rst。 */
        unsigned int  g3d_domain_idlereq         : 1;  /* bit[11]   : g3d_domain_idlereq
                                                                      0： normal mode；
                                                                      1： idle req when this domain idle,power down or rst。 */
        unsigned int  ccpu_domain_idlereq        : 1;  /* bit[12]   : ccpu_domain_idlereq
                                                                      0： normal mode；
                                                                      1： idle req when this domain idle,power down or rst。 */
        unsigned int  mnoc_domain_idlereq        : 1;  /* bit[13]   : mnoc_domain_idlereq
                                                                      0： normal mode；
                                                                      1： idle req when this domain idle,power down or rst。 */
        unsigned int  video_domain_idlereq       : 1;  /* bit[14]   : video_domain_idlereq
                                                                      0： normal mode；
                                                                      1： idle req when this domain idle,power down or rst。 */
        unsigned int  ade_domain_idlereq         : 1;  /* bit[15]   : ade_domain_idlereq
                                                                      0： normal mode；
                                                                      1： idle req when this domain idle,power down or rst。 */
        unsigned int  acpu_sft_fiq_req_msk       : 1;  /* bit[16]   : acpu_sft_fiq_req msk */
        unsigned int  ccpu_sft_fiq_req_msk       : 1;  /* bit[17]   : ccpu_sft_fiq_req msk */
        unsigned int  reserved_1                 : 5;  /* bit[18-22]: reserved */
        unsigned int  mdm_nsp_domain_idlereq_msk : 1;  /* bit[23]   : mdm_nsp_domain_idlereq msk */
        unsigned int  dma_domain_idlereq_msk     : 1;  /* bit[24]   : dma_domain_idlereq msk */
        unsigned int  ao_domain_idlereq_msk      : 1;  /* bit[25]   : ao_domain_idlereq msk */
        unsigned int  sysnoc_domain_idlereq_msk  : 1;  /* bit[26]   : sysnoc_domain_idlereq msk */
        unsigned int  g3d_domain_idlereq_msk     : 1;  /* bit[27]   : g3d_domain_idlereq msk */
        unsigned int  ccpu_domain_idlereq_msk    : 1;  /* bit[28]   : ccpu_domain_idlereq msk */
        unsigned int  mnoc_domain_idlereq_msk    : 1;  /* bit[29]   : mnoc_domain_idlereq msk */
        unsigned int  video_domain_idlereq_msk   : 1;  /* bit[30]   : video_domain_idlereq msk */
        unsigned int  ade_domain_idlereq_msk     : 1;  /* bit[31]   : [31]：ade_domain_idlereq msk */
    } reg;
} SOC_AO_SCTRL_SC_PW_CTRL1_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_CTRL1_acpu_sft_fiq_req_START            (0)
#define SOC_AO_SCTRL_SC_PW_CTRL1_acpu_sft_fiq_req_END              (0)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_sft_fiq_req_START            (1)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_sft_fiq_req_END              (1)
#define SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_START      (7)
#define SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_END        (7)
#define SOC_AO_SCTRL_SC_PW_CTRL1_dma_domain_idlereq_START          (8)
#define SOC_AO_SCTRL_SC_PW_CTRL1_dma_domain_idlereq_END            (8)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ao_domain_idlereq_START           (9)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ao_domain_idlereq_END             (9)
#define SOC_AO_SCTRL_SC_PW_CTRL1_sysnoc_domain_idlereq_START       (10)
#define SOC_AO_SCTRL_SC_PW_CTRL1_sysnoc_domain_idlereq_END         (10)
#define SOC_AO_SCTRL_SC_PW_CTRL1_g3d_domain_idlereq_START          (11)
#define SOC_AO_SCTRL_SC_PW_CTRL1_g3d_domain_idlereq_END            (11)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_domain_idlereq_START         (12)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_domain_idlereq_END           (12)
#define SOC_AO_SCTRL_SC_PW_CTRL1_mnoc_domain_idlereq_START         (13)
#define SOC_AO_SCTRL_SC_PW_CTRL1_mnoc_domain_idlereq_END           (13)
#define SOC_AO_SCTRL_SC_PW_CTRL1_video_domain_idlereq_START        (14)
#define SOC_AO_SCTRL_SC_PW_CTRL1_video_domain_idlereq_END          (14)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ade_domain_idlereq_START          (15)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ade_domain_idlereq_END            (15)
#define SOC_AO_SCTRL_SC_PW_CTRL1_acpu_sft_fiq_req_msk_START        (16)
#define SOC_AO_SCTRL_SC_PW_CTRL1_acpu_sft_fiq_req_msk_END          (16)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_sft_fiq_req_msk_START        (17)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_sft_fiq_req_msk_END          (17)
#define SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_msk_START  (23)
#define SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_msk_END    (23)
#define SOC_AO_SCTRL_SC_PW_CTRL1_dma_domain_idlereq_msk_START      (24)
#define SOC_AO_SCTRL_SC_PW_CTRL1_dma_domain_idlereq_msk_END        (24)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ao_domain_idlereq_msk_START       (25)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ao_domain_idlereq_msk_END         (25)
#define SOC_AO_SCTRL_SC_PW_CTRL1_sysnoc_domain_idlereq_msk_START   (26)
#define SOC_AO_SCTRL_SC_PW_CTRL1_sysnoc_domain_idlereq_msk_END     (26)
#define SOC_AO_SCTRL_SC_PW_CTRL1_g3d_domain_idlereq_msk_START      (27)
#define SOC_AO_SCTRL_SC_PW_CTRL1_g3d_domain_idlereq_msk_END        (27)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_domain_idlereq_msk_START     (28)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ccpu_domain_idlereq_msk_END       (28)
#define SOC_AO_SCTRL_SC_PW_CTRL1_mnoc_domain_idlereq_msk_START     (29)
#define SOC_AO_SCTRL_SC_PW_CTRL1_mnoc_domain_idlereq_msk_END       (29)
#define SOC_AO_SCTRL_SC_PW_CTRL1_video_domain_idlereq_msk_START    (30)
#define SOC_AO_SCTRL_SC_PW_CTRL1_video_domain_idlereq_msk_END      (30)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ade_domain_idlereq_msk_START      (31)
#define SOC_AO_SCTRL_SC_PW_CTRL1_ade_domain_idlereq_msk_END        (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PW_CTRL_UNION
 结构说明  : SC_PW_CTRL 寄存器结构定义。地址偏移量:0x8CC，初值:0x000000F0，宽度:32
 寄存器说明: 低功耗控制寄存器0，功能包括外设IO retention，ddr io retention，总线门控，ipc_ns和ipc_s自动门控，外设区掉电控制。
            [31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  soc_pw_dn_en                   : 1;  /* bit[0] : soc外设下电区掉电使能
                                                                       0：不掉电；
                                                                       1：掉电。 */
        unsigned int  ao_noc_timeout_en              : 1;  /* bit[1] : ao subsys noc bus timeout enable
                                                                       0： disable；
                                                                       1： enable。 */
        unsigned int  reserved_0                     : 1;  /* bit[2] : reserved */
        unsigned int  ddrphy0_ret_en_i               : 1;  /* bit[3] : ddrphy coreside ret_en_i data retention和cke retention的控制信号 
                                                                       0： data retention disable；
                                                                       1： data retention enable。 */
        unsigned int  clk_en_sysnoc                  : 1;  /* bit[4] : sysnoc门控使能
                                                                       0：noc总线时钟关闭；
                                                                       1：noc总线时钟打开。 */
        unsigned int  clk_en_modemnoc                : 1;  /* bit[5] : modemnoc门控使能
                                                                       0：noc总线时钟关闭；
                                                                       1：noc总线时钟打开。 */
        unsigned int  ipc_ns_auto_clk_gate_en        : 1;  /* bit[6] : (ipc_ns)auto_clk_gate_en
                                                                       1'b0： 硬件自动门控不使能；
                                                                       1'b1： 硬件自动门控使能。 */
        unsigned int  ipc_s_auto_clk_gate_en         : 1;  /* bit[7] : (ipc_s)auto_clk_gate_en
                                                                       1'b0： 硬件自动门控不使能；
                                                                       1'b1： 硬件自动门控使能。 */
        unsigned int  reserved_1                     : 1;  /* bit[8] : reserved */
        unsigned int  m3_ram_ctrl_sftbypass          : 1;  /* bit[9] : m3_ram_ctrl_sftbypass。
                                                                       0：m3 ram ctrl by software config reg of &quot;m3_ramctrl_pgen/ret2n/ret1n&quot;；
                                                                       1：m3 ram ctrl by hardware。 */
        unsigned int  aarm_l2_ram_ctrl_sftbypass     : 1;  /* bit[10]: reserved */
        unsigned int  acpu0_pwctrl_lp_set            : 1;  /* bit[11]: acpu0 power ctrl
                                                                       0： acpu cluster和acpu0同时上电；
                                                                       1： acpu cluster上电时，acpu0不上电。
                                                                       注：在配置acpu0_pwctrl_lp_set=1前,需要确保ACPU_SC 中acpu0的两组低功耗控制信号*_hotplug和*_cpuidle控制状态保持一致，并且在后续配置中做到同时配置。 */
        unsigned int  dual_tcxo_en                   : 1;  /* bit[12]: dual tcxo enable
                                                                       0： signle tcxo；
                                                                       1： dual tcxo。 */
        unsigned int  bbpdrx_tcxo1_sel               : 1;  /* bit[13]: bbpdrx int open tcxo1 figure： 
                                                                       0： ch1 bbp drx int open tcxo1；
                                                                       1： ch0 bbp drdx int open tcxo1。 */
        unsigned int  reserved_2                     : 1;  /* bit[14]: reserved */
        unsigned int  reserved_3                     : 1;  /* bit[15]: reserved */
        unsigned int  soc_pw_dn_en_msk               : 1;  /* bit[16]: 指示soc外设下电区掉电使能 msk */
        unsigned int  ao_noc_timeout_en_msk          : 1;  /* bit[17]: ao subsys noc bus timeout enable mask */
        unsigned int  reserved_4                     : 1;  /* bit[18]: reserved */
        unsigned int  ddrphy0_ret_en_i_msk           : 1;  /* bit[19]: ddrphy coreside ret_en_i data retention和cke retention的控制信号 msk */
        unsigned int  clk_en_sysnoc_msk              : 1;  /* bit[20]: sysnoc门控使能 msk */
        unsigned int  clk_en_modemnoc_msk            : 1;  /* bit[21]: modemnoc门控使能 msk */
        unsigned int  ipc_ns_auto_clk_gate_en_msk    : 1;  /* bit[22]: (ipc_ns)auto_clk_gate_en msk */
        unsigned int  ipc_s_auto_clk_gate_en_msk     : 1;  /* bit[23]: (ipc_s)auto_clk_gate_en msk */
        unsigned int  reserved_5                     : 1;  /* bit[24]: reserved */
        unsigned int  m3_ram_ctrl_sftbypass_msk      : 1;  /* bit[25]: m3_ram_ctrl_sftbypass msk */
        unsigned int  aarm_l2_ram_ctrl_sftbypass_msk : 1;  /* bit[26]: reserved */
        unsigned int  acpu0_pwctrl_lp_set_msk        : 1;  /* bit[27]: acpu0_pwctrl_lp_set msk */
        unsigned int  dual_tcxo_en_msk               : 1;  /* bit[28]: dual_tcxo_en msk */
        unsigned int  bbpdrx_tcxo1_sel_msk           : 1;  /* bit[29]: bbpdrx_tcxo1_sel msk */
        unsigned int  reserved_6                     : 1;  /* bit[30]: reserved */
        unsigned int  reserved_7                     : 1;  /* bit[31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_PW_CTRL_UNION;
#endif
#define SOC_AO_SCTRL_SC_PW_CTRL_soc_pw_dn_en_START                    (0)
#define SOC_AO_SCTRL_SC_PW_CTRL_soc_pw_dn_en_END                      (0)
#define SOC_AO_SCTRL_SC_PW_CTRL_ao_noc_timeout_en_START               (1)
#define SOC_AO_SCTRL_SC_PW_CTRL_ao_noc_timeout_en_END                 (1)
#define SOC_AO_SCTRL_SC_PW_CTRL_ddrphy0_ret_en_i_START                (3)
#define SOC_AO_SCTRL_SC_PW_CTRL_ddrphy0_ret_en_i_END                  (3)
#define SOC_AO_SCTRL_SC_PW_CTRL_clk_en_sysnoc_START                   (4)
#define SOC_AO_SCTRL_SC_PW_CTRL_clk_en_sysnoc_END                     (4)
#define SOC_AO_SCTRL_SC_PW_CTRL_clk_en_modemnoc_START                 (5)
#define SOC_AO_SCTRL_SC_PW_CTRL_clk_en_modemnoc_END                   (5)
#define SOC_AO_SCTRL_SC_PW_CTRL_ipc_ns_auto_clk_gate_en_START         (6)
#define SOC_AO_SCTRL_SC_PW_CTRL_ipc_ns_auto_clk_gate_en_END           (6)
#define SOC_AO_SCTRL_SC_PW_CTRL_ipc_s_auto_clk_gate_en_START          (7)
#define SOC_AO_SCTRL_SC_PW_CTRL_ipc_s_auto_clk_gate_en_END            (7)
#define SOC_AO_SCTRL_SC_PW_CTRL_m3_ram_ctrl_sftbypass_START           (9)
#define SOC_AO_SCTRL_SC_PW_CTRL_m3_ram_ctrl_sftbypass_END             (9)
#define SOC_AO_SCTRL_SC_PW_CTRL_aarm_l2_ram_ctrl_sftbypass_START      (10)
#define SOC_AO_SCTRL_SC_PW_CTRL_aarm_l2_ram_ctrl_sftbypass_END        (10)
#define SOC_AO_SCTRL_SC_PW_CTRL_acpu0_pwctrl_lp_set_START             (11)
#define SOC_AO_SCTRL_SC_PW_CTRL_acpu0_pwctrl_lp_set_END               (11)
#define SOC_AO_SCTRL_SC_PW_CTRL_dual_tcxo_en_START                    (12)
#define SOC_AO_SCTRL_SC_PW_CTRL_dual_tcxo_en_END                      (12)
#define SOC_AO_SCTRL_SC_PW_CTRL_bbpdrx_tcxo1_sel_START                (13)
#define SOC_AO_SCTRL_SC_PW_CTRL_bbpdrx_tcxo1_sel_END                  (13)
#define SOC_AO_SCTRL_SC_PW_CTRL_soc_pw_dn_en_msk_START                (16)
#define SOC_AO_SCTRL_SC_PW_CTRL_soc_pw_dn_en_msk_END                  (16)
#define SOC_AO_SCTRL_SC_PW_CTRL_ao_noc_timeout_en_msk_START           (17)
#define SOC_AO_SCTRL_SC_PW_CTRL_ao_noc_timeout_en_msk_END             (17)
#define SOC_AO_SCTRL_SC_PW_CTRL_ddrphy0_ret_en_i_msk_START            (19)
#define SOC_AO_SCTRL_SC_PW_CTRL_ddrphy0_ret_en_i_msk_END              (19)
#define SOC_AO_SCTRL_SC_PW_CTRL_clk_en_sysnoc_msk_START               (20)
#define SOC_AO_SCTRL_SC_PW_CTRL_clk_en_sysnoc_msk_END                 (20)
#define SOC_AO_SCTRL_SC_PW_CTRL_clk_en_modemnoc_msk_START             (21)
#define SOC_AO_SCTRL_SC_PW_CTRL_clk_en_modemnoc_msk_END               (21)
#define SOC_AO_SCTRL_SC_PW_CTRL_ipc_ns_auto_clk_gate_en_msk_START     (22)
#define SOC_AO_SCTRL_SC_PW_CTRL_ipc_ns_auto_clk_gate_en_msk_END       (22)
#define SOC_AO_SCTRL_SC_PW_CTRL_ipc_s_auto_clk_gate_en_msk_START      (23)
#define SOC_AO_SCTRL_SC_PW_CTRL_ipc_s_auto_clk_gate_en_msk_END        (23)
#define SOC_AO_SCTRL_SC_PW_CTRL_m3_ram_ctrl_sftbypass_msk_START       (25)
#define SOC_AO_SCTRL_SC_PW_CTRL_m3_ram_ctrl_sftbypass_msk_END         (25)
#define SOC_AO_SCTRL_SC_PW_CTRL_aarm_l2_ram_ctrl_sftbypass_msk_START  (26)
#define SOC_AO_SCTRL_SC_PW_CTRL_aarm_l2_ram_ctrl_sftbypass_msk_END    (26)
#define SOC_AO_SCTRL_SC_PW_CTRL_acpu0_pwctrl_lp_set_msk_START         (27)
#define SOC_AO_SCTRL_SC_PW_CTRL_acpu0_pwctrl_lp_set_msk_END           (27)
#define SOC_AO_SCTRL_SC_PW_CTRL_dual_tcxo_en_msk_START                (28)
#define SOC_AO_SCTRL_SC_PW_CTRL_dual_tcxo_en_msk_END                  (28)
#define SOC_AO_SCTRL_SC_PW_CTRL_bbpdrx_tcxo1_sel_msk_START            (29)
#define SOC_AO_SCTRL_SC_PW_CTRL_bbpdrx_tcxo1_sel_msk_END              (29)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCPU_VOTEEN_UNION
 结构说明  : SC_MCPU_VOTEEN 寄存器结构定义。地址偏移量:0x8D0，初值:0x00000000，宽度:32
 寄存器说明: MCPU睡眠唤醒投票使能寄存器0。对比特写1，产生投票状态；写0不影响投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcpusleep_vote_en0_7reserved : 8;  /* bit[0-7] : reserved */
        unsigned int  sc_mcpusleep_vote_en0           : 24; /* bit[8-31]: [31:8]：MCPU唤醒投票寄存器0。用于产生唤醒投票。对比特写1，产生唤醒投票；写0不影响投票的状态。
                                                                          [7:0]：硬件状态投票使用。 */
    } reg;
} SOC_AO_SCTRL_SC_MCPU_VOTEEN_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCPU_VOTEEN_sc_mcpusleep_vote_en0_7reserved_START  (0)
#define SOC_AO_SCTRL_SC_MCPU_VOTEEN_sc_mcpusleep_vote_en0_7reserved_END    (7)
#define SOC_AO_SCTRL_SC_MCPU_VOTEEN_sc_mcpusleep_vote_en0_START            (8)
#define SOC_AO_SCTRL_SC_MCPU_VOTEEN_sc_mcpusleep_vote_en0_END              (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCPU_VOTEDIS_UNION
 结构说明  : SC_MCPU_VOTEDIS 寄存器结构定义。地址偏移量:0x8D4，初值:0x00000000，宽度:32
 寄存器说明: MCPU睡眠唤醒投票禁止寄存器0。对比特写1，清除投票状态；写0不影响投票使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcpusleep_vote_dis0_7reserved : 8;  /* bit[0-7] : reserved */
        unsigned int  sc_mcpusleep_vote_dis0           : 24; /* bit[8-31]: [31:8]：MCPU睡眠投票使能禁止寄存器0。用于产生睡眠投票。对比特写1，产生睡眠投票；写0不影响投票的状态。
                                                                           [7:0]：硬件状态投票使用。 */
    } reg;
} SOC_AO_SCTRL_SC_MCPU_VOTEDIS_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCPU_VOTEDIS_sc_mcpusleep_vote_dis0_7reserved_START  (0)
#define SOC_AO_SCTRL_SC_MCPU_VOTEDIS_sc_mcpusleep_vote_dis0_7reserved_END    (7)
#define SOC_AO_SCTRL_SC_MCPU_VOTEDIS_sc_mcpusleep_vote_dis0_START            (8)
#define SOC_AO_SCTRL_SC_MCPU_VOTEDIS_sc_mcpusleep_vote_dis0_END              (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCPU_VOTESTAT_UNION
 结构说明  : SC_MCPU_VOTESTAT 寄存器结构定义。地址偏移量:0x8D8，初值:0x00000000，宽度:32
 寄存器说明: MCPU睡眠唤醒投票状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcpusleep_vote_stat0_7reserved : 8;  /* bit[0-7] : reserved */
        unsigned int  sc_mcpusleep_vote_stat0           : 24; /* bit[8-31]: [31:8]：MCPU睡眠投票状态寄存器0。用于回读各投票的状态。1为投票，0为非投票状态。
                                                                            为0表示允许睡眠，为1表示不允许睡眠，需要唤醒
                                                                            [7:0]：硬件状态投票使用。 */
    } reg;
} SOC_AO_SCTRL_SC_MCPU_VOTESTAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT_sc_mcpusleep_vote_stat0_7reserved_START  (0)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT_sc_mcpusleep_vote_stat0_7reserved_END    (7)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT_sc_mcpusleep_vote_stat0_START            (8)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT_sc_mcpusleep_vote_stat0_END              (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCPU_VOTE_MSK0_UNION
 结构说明  : SC_MCPU_VOTE_MSK0 寄存器结构定义。地址偏移量:0x8E0，初值:0x00000000，宽度:32
 寄存器说明: MCPU睡眠唤醒投票屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcpusleep_vote_msk0 : 32; /* bit[0-31]: 屏蔽送给MCPU的睡眠投票状态。 */
    } reg;
} SOC_AO_SCTRL_SC_MCPU_VOTE_MSK0_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCPU_VOTE_MSK0_sc_mcpusleep_vote_msk0_START  (0)
#define SOC_AO_SCTRL_SC_MCPU_VOTE_MSK0_sc_mcpusleep_vote_msk0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCPU_VOTE_MSK1_UNION
 结构说明  : SC_MCPU_VOTE_MSK1 寄存器结构定义。地址偏移量:0x8E4，初值:0x00000000，宽度:32
 寄存器说明: MCPU睡眠唤醒投票屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcpusleep_vote_msk1 : 32; /* bit[0-31]: 屏蔽送给MCU的唤醒投票状态。 */
    } reg;
} SOC_AO_SCTRL_SC_MCPU_VOTE_MSK1_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCPU_VOTE_MSK1_sc_mcpusleep_vote_msk1_START  (0)
#define SOC_AO_SCTRL_SC_MCPU_VOTE_MSK1_sc_mcpusleep_vote_msk1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_UNION
 结构说明  : SC_MCPU_VOTESTAT0_MSK 寄存器结构定义。地址偏移量:0x8E8，初值:0x00000000，宽度:32
 寄存器说明: MCPU睡眠投票屏蔽后状态寄存器0。用于回读各投票的状态。1为投票，0为非投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcpusleep_vote_stat0_msk_0coresight : 1;  /* bit[0]   : [0]： CoreSight掉电请求csyspwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_mcpusleep_vote_stat0_msk_1coresight : 1;  /* bit[1]   : [1]： CoreSight掉电请求cdbgpwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_mcpusleep_vote_stat0_msk_2ipf       : 1;  /* bit[2]   : [2]： IPF工作状态信号。 */
        unsigned int  sc_mcpusleep_vote_stat0_msk_3socp      : 1;  /* bit[3]   : [3]： SOCP工作状态信号。 */
        unsigned int  sc_mcpusleep_vote_stat0_msk_7reserved  : 4;  /* bit[4-7] : [7:4]： reserved */
        unsigned int  sc_mcpusleep_vote_stat0_msk            : 24; /* bit[8-31]: 屏蔽后送给MCPU的睡眠投票状态状态
                                                                                 有任意1bit为1表示不允许睡眠，所有bit为1'b0表示允许睡眠，当投票状态从不允许MCPU睡眠转化为允许MCPU睡眠时，投票模块向MCPU产生睡眠中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_0coresight_START  (0)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_0coresight_END    (0)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_1coresight_START  (1)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_1coresight_END    (1)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_2ipf_START        (2)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_2ipf_END          (2)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_3socp_START       (3)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_3socp_END         (3)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_7reserved_START   (4)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_7reserved_END     (7)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_START             (8)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_END               (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_UNION
 结构说明  : SC_MCPU_VOTESTAT1_MSK 寄存器结构定义。地址偏移量:0x8EC，初值:0x00000000，宽度:32
 寄存器说明: MCPU睡眠唤醒投票屏蔽后状态寄存器1。用于回读各投票的状态。1为投票，0为非投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcpusleep_vote_stat1_msk_0coresight : 1;  /* bit[0]   : [0]： CoreSight掉电请求csyspwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_mcpusleep_vote_stat1_msk_1coresight : 1;  /* bit[1]   : [1]： CoreSight掉电请求cdbgpwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_mcpusleep_vote_stat1_msk_2ipf       : 1;  /* bit[2]   : [2]： IPF工作状态信号。 */
        unsigned int  sc_mcpusleep_vote_stat1_msk_3socp      : 1;  /* bit[3]   : [3]： SOCP工作状态信号。 */
        unsigned int  sc_mcpusleep_vote_stat1_msk_7reserved  : 4;  /* bit[4-7] : [7:4]：reserved */
        unsigned int  sc_mcpusleep_vote_stat1_msk            : 24; /* bit[8-31]: 屏蔽后送给MCU的唤醒投票状态状态
                                                                                 有任意1bit为1表示不允许睡眠，所有bit为1'b0表示允许睡眠，当投票状态从允许MCPU睡眠转化为不允许MCPU睡眠时，投票模块向MCU产生唤醒中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_0coresight_START  (0)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_0coresight_END    (0)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_1coresight_START  (1)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_1coresight_END    (1)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_2ipf_START        (2)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_2ipf_END          (2)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_3socp_START       (3)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_3socp_END         (3)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_7reserved_START   (4)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_7reserved_END     (7)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_START             (8)
#define SOC_AO_SCTRL_SC_MCPU_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_END               (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERI_VOTEEN_UNION
 结构说明  : SC_PERI_VOTEEN 寄存器结构定义。地址偏移量:0x8F0，初值:0x00000000，宽度:32
 寄存器说明: PERI睡眠唤醒投票使能寄存器0。对比特写1，产生投票状态；写0不影响投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_perisleep_vote_en0_reserved : 8;  /* bit[0-7] : reserved */
        unsigned int  sc_perisleep_vote_en0          : 24; /* bit[8-31]: [31:8]：PERI唤醒投票寄存器0。用于产生唤醒投票。对比特写1，产生唤醒投票；写0不影响投票的状态。
                                                                         [7:0]：硬件状态投票使用。 */
    } reg;
} SOC_AO_SCTRL_SC_PERI_VOTEEN_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERI_VOTEEN_sc_perisleep_vote_en0_reserved_START  (0)
#define SOC_AO_SCTRL_SC_PERI_VOTEEN_sc_perisleep_vote_en0_reserved_END    (7)
#define SOC_AO_SCTRL_SC_PERI_VOTEEN_sc_perisleep_vote_en0_START           (8)
#define SOC_AO_SCTRL_SC_PERI_VOTEEN_sc_perisleep_vote_en0_END             (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERI_VOTEDIS_UNION
 结构说明  : SC_PERI_VOTEDIS 寄存器结构定义。地址偏移量:0x8F4，初值:0x00000000，宽度:32
 寄存器说明: PERI睡眠唤醒投票禁止寄存器0。对比特写1，清除投票状态；写0不影响投票使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_perisleep_vote_dis0_reserved : 8;  /* bit[0-7] : reserved */
        unsigned int  sc_perisleep_vote_dis0          : 24; /* bit[8-31]: [31:8]：PERI睡眠投票使能禁止寄存器0。用于产生睡眠投票。对比特写1，产生睡眠投票；写0不影响投票的状态。
                                                                          [7:0]：硬件状态投票使用。 */
    } reg;
} SOC_AO_SCTRL_SC_PERI_VOTEDIS_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERI_VOTEDIS_sc_perisleep_vote_dis0_reserved_START  (0)
#define SOC_AO_SCTRL_SC_PERI_VOTEDIS_sc_perisleep_vote_dis0_reserved_END    (7)
#define SOC_AO_SCTRL_SC_PERI_VOTEDIS_sc_perisleep_vote_dis0_START           (8)
#define SOC_AO_SCTRL_SC_PERI_VOTEDIS_sc_perisleep_vote_dis0_END             (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERI_VOTESTAT_UNION
 结构说明  : SC_PERI_VOTESTAT 寄存器结构定义。地址偏移量:0x8F8，初值:0x00000000，宽度:32
 寄存器说明: PERI睡眠唤醒投票状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_perisleep_vote_stat0_reserved : 8;  /* bit[0-7] : reserved */
        unsigned int  sc_perisleep_vote_stat0          : 24; /* bit[8-31]: [31:8]：PERI睡眠投票状态寄存器0。用于回读各投票的状态。1为投票，0为非投票状态。
                                                                           为0表示允许睡眠，为1表示不允许睡眠，需要唤醒
                                                                           [7:0]：硬件状态投票使用。 */
    } reg;
} SOC_AO_SCTRL_SC_PERI_VOTESTAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT_sc_perisleep_vote_stat0_reserved_START  (0)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT_sc_perisleep_vote_stat0_reserved_END    (7)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT_sc_perisleep_vote_stat0_START           (8)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT_sc_perisleep_vote_stat0_END             (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERI_VOTE_MSK0_UNION
 结构说明  : SC_PERI_VOTE_MSK0 寄存器结构定义。地址偏移量:0x900，初值:0x00000000，宽度:32
 寄存器说明: PERI睡眠唤醒投票屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_perisleep_vote_msk0 : 32; /* bit[0-31]: 屏蔽送给MCU的睡眠投票状态。 */
    } reg;
} SOC_AO_SCTRL_SC_PERI_VOTE_MSK0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERI_VOTE_MSK0_sc_perisleep_vote_msk0_START  (0)
#define SOC_AO_SCTRL_SC_PERI_VOTE_MSK0_sc_perisleep_vote_msk0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERI_VOTE_MSK1_UNION
 结构说明  : SC_PERI_VOTE_MSK1 寄存器结构定义。地址偏移量:0x904，初值:0x00000000，宽度:32
 寄存器说明: PERI睡眠唤醒投票屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_perisleep_vote_msk1 : 32; /* bit[0-31]: 屏蔽送给MCU的唤醒投票状态。 */
    } reg;
} SOC_AO_SCTRL_SC_PERI_VOTE_MSK1_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERI_VOTE_MSK1_sc_perisleep_vote_msk1_START  (0)
#define SOC_AO_SCTRL_SC_PERI_VOTE_MSK1_sc_perisleep_vote_msk1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_UNION
 结构说明  : SC_PERI_VOTESTAT0_MSK 寄存器结构定义。地址偏移量:0x908，初值:0x00000000，宽度:32
 寄存器说明: PERI睡眠投票屏蔽后状态寄存器0。用于回读各投票的状态。1为投票，0为非投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcpusleep_vote_stat0_msk_0coresight : 1;  /* bit[0]   : [0]： CoreSight掉电请求csyspwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_mcpusleep_vote_stat0_msk_1coresight : 1;  /* bit[1]   : [1]： CoreSight掉电请求cdbgpwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_perisleep_vote_stat0_msk_2ipf       : 1;  /* bit[2]   : [2]： IPF工作状态信号。 */
        unsigned int  sc_perisleep_vote_stat0_msk_3socp      : 1;  /* bit[3]   : [3]： SOCP工作状态信号。 */
        unsigned int  sc_perisleep_vote_stat0_msk_acpu0      : 1;  /* bit[4]   : [4]： ACPU Cluster分区掉电状态。 */
        unsigned int  sc_perisleep_vote_stat0_msk_mcpu       : 1;  /* bit[5]   : [5]： MCPU分区掉电状态。 */
        unsigned int  sc_perisleep_vote_stat0_msk_hifi       : 1;  /* bit[6]   : [6]： HiFi分区掉电状态。 */
        unsigned int  sc_perisleep_vote_stat0_msk_codec      : 1;  /* bit[7]   : [7]： CODEC+ISP分区掉电状态。 */
        unsigned int  sc_perisleep_vote_stat0_msk            : 24; /* bit[8-31]: 屏蔽后送给PERI的睡眠投票状态状态
                                                                                 有任意1bit为1表示不允许睡眠，所有bit为1'b0表示允许睡眠，当投票状态从不允许PERI睡眠转化为允许PERI睡眠时，投票模块向MCU产生睡眠中断。 */
    } reg;
} SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_0coresight_START  (0)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_0coresight_END    (0)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_1coresight_START  (1)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_mcpusleep_vote_stat0_msk_1coresight_END    (1)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_2ipf_START        (2)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_2ipf_END          (2)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_3socp_START       (3)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_3socp_END         (3)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_acpu0_START       (4)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_acpu0_END         (4)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_mcpu_START        (5)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_mcpu_END          (5)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_hifi_START        (6)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_hifi_END          (6)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_codec_START       (7)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_codec_END         (7)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_START             (8)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT0_MSK_sc_perisleep_vote_stat0_msk_END               (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_UNION
 结构说明  : SC_PERI_VOTESTAT1_MSK 寄存器结构定义。地址偏移量:0x90C，初值:0x00000000，宽度:32
 寄存器说明: PERI睡眠唤醒投票屏蔽后状态寄存器1。用于回读各投票的状态。1为投票，0为非投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcpusleep_vote_stat1_msk_0coresight : 1;  /* bit[0]   : [0]： CoreSight掉电请求csyspwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_mcpusleep_vote_stat1_msk_1coresight : 1;  /* bit[1]   : [1]： CoreSight掉电请求cdbgpwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_perisleep_vote_stat1_msk_2ipf       : 1;  /* bit[2]   : [2]： IPF工作状态信号。 */
        unsigned int  sc_perisleep_vote_stat1_msk_3socp      : 1;  /* bit[3]   : [3]： SOCP工作状态信号。 */
        unsigned int  sc_perisleep_vote_stat1_msk_acpu0      : 1;  /* bit[4]   : [4]： ACPU Cluster分区掉电状态。 */
        unsigned int  sc_perisleep_vote_stat1_msk_mcpu       : 1;  /* bit[5]   : [5]： MCPU分区掉电状态。 */
        unsigned int  sc_perisleep_vote_stat1_msk_hifi       : 1;  /* bit[6]   : [6]： HiFi分区掉电状态。 */
        unsigned int  sc_perisleep_vote_stat1_msk_codec      : 1;  /* bit[7]   : [7]： CODEC+ISP分区掉电状态。 */
        unsigned int  sc_perisleep_vote_stat1_msk            : 24; /* bit[8-31]: 屏蔽后送给MCU的唤醒投票状态状态
                                                                                 有任意1bit为1表示不允许睡眠，所有bit为1'b0表示允许睡眠，当投票状态从允许PERI睡眠转化为不允许PERI睡眠时，投票模块向MCU产生唤醒中断。 */
    } reg;
} SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_0coresight_START  (0)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_0coresight_END    (0)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_1coresight_START  (1)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_mcpusleep_vote_stat1_msk_1coresight_END    (1)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_2ipf_START        (2)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_2ipf_END          (2)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_3socp_START       (3)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_3socp_END         (3)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_acpu0_START       (4)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_acpu0_END         (4)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_mcpu_START        (5)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_mcpu_END          (5)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_hifi_START        (6)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_hifi_END          (6)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_codec_START       (7)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_codec_END         (7)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_START             (8)
#define SOC_AO_SCTRL_SC_PERI_VOTESTAT1_MSK_sc_perisleep_vote_stat1_msk_END               (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ACPU_VOTEEN_UNION
 结构说明  : SC_ACPU_VOTEEN 寄存器结构定义。地址偏移量:0x910，初值:0x00000000，宽度:32
 寄存器说明: ACPU睡眠唤醒投票使能寄存器0。对比特写1，产生投票状态；写0不影响投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_acpusleep_vote_en0_7reserved : 8;  /* bit[0-7] : reserved */
        unsigned int  sc_acpusleep_vote_en0           : 24; /* bit[8-31]: [31:8]：ACPU唤醒投票寄存器0。用于产生唤醒投票。对比特写1，产生唤醒投票；写0不影响投票的状态。
                                                                          [7:0]：硬件状态投票使用。 */
    } reg;
} SOC_AO_SCTRL_SC_ACPU_VOTEEN_UNION;
#endif
#define SOC_AO_SCTRL_SC_ACPU_VOTEEN_sc_acpusleep_vote_en0_7reserved_START  (0)
#define SOC_AO_SCTRL_SC_ACPU_VOTEEN_sc_acpusleep_vote_en0_7reserved_END    (7)
#define SOC_AO_SCTRL_SC_ACPU_VOTEEN_sc_acpusleep_vote_en0_START            (8)
#define SOC_AO_SCTRL_SC_ACPU_VOTEEN_sc_acpusleep_vote_en0_END              (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ACPU_VOTEDIS_UNION
 结构说明  : SC_ACPU_VOTEDIS 寄存器结构定义。地址偏移量:0x914，初值:0x00000000，宽度:32
 寄存器说明: ACPU睡眠唤醒投票禁止寄存器0。对比特写1，清除投票状态；写0不影响投票使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_acpusleep_vote_dis0_7reserved : 8;  /* bit[0-7] : reserved */
        unsigned int  sc_acpusleep_vote_dis0           : 24; /* bit[8-31]: [31:8]： ACPU睡眠投票使能禁止寄存器0。用于产生睡眠投票。对比特写1，产生睡眠投票；写0不影响投票的状态。
                                                                           [7:0]： 硬件状态投票使用。 */
    } reg;
} SOC_AO_SCTRL_SC_ACPU_VOTEDIS_UNION;
#endif
#define SOC_AO_SCTRL_SC_ACPU_VOTEDIS_sc_acpusleep_vote_dis0_7reserved_START  (0)
#define SOC_AO_SCTRL_SC_ACPU_VOTEDIS_sc_acpusleep_vote_dis0_7reserved_END    (7)
#define SOC_AO_SCTRL_SC_ACPU_VOTEDIS_sc_acpusleep_vote_dis0_START            (8)
#define SOC_AO_SCTRL_SC_ACPU_VOTEDIS_sc_acpusleep_vote_dis0_END              (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ACPU_VOTESTAT_UNION
 结构说明  : SC_ACPU_VOTESTAT 寄存器结构定义。地址偏移量:0x918，初值:0x00000000，宽度:32
 寄存器说明: ACPU睡眠唤醒投票状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_acpusleep_vote_stat0_7reserved : 8;  /* bit[0-7] : reserved */
        unsigned int  sc_acpusleep_vote_stat0           : 24; /* bit[8-31]: [31:8]： ACPU睡眠投票状态寄存器0。用于回读各投票的状态。1为投票，0为非投票状态。
                                                                            为0表示允许睡眠，为1表示不允许睡眠，需要唤醒
                                                                            [7:0]： 硬件状态投票使用。 */
    } reg;
} SOC_AO_SCTRL_SC_ACPU_VOTESTAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT_sc_acpusleep_vote_stat0_7reserved_START  (0)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT_sc_acpusleep_vote_stat0_7reserved_END    (7)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT_sc_acpusleep_vote_stat0_START            (8)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT_sc_acpusleep_vote_stat0_END              (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ACPU_VOTE_MSK0_UNION
 结构说明  : SC_ACPU_VOTE_MSK0 寄存器结构定义。地址偏移量:0x920，初值:0x00000000，宽度:32
 寄存器说明: ACPU睡眠唤醒投票屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_acpusleep_vote_msk0 : 32; /* bit[0-31]: 屏蔽送给ACPU的睡眠投票状态。 */
    } reg;
} SOC_AO_SCTRL_SC_ACPU_VOTE_MSK0_UNION;
#endif
#define SOC_AO_SCTRL_SC_ACPU_VOTE_MSK0_sc_acpusleep_vote_msk0_START  (0)
#define SOC_AO_SCTRL_SC_ACPU_VOTE_MSK0_sc_acpusleep_vote_msk0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ACPU_VOTE_MSK1_UNION
 结构说明  : SC_ACPU_VOTE_MSK1 寄存器结构定义。地址偏移量:0x924，初值:0x00000000，宽度:32
 寄存器说明: ACPU睡眠唤醒投票屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_acpusleep_vote_msk1 : 32; /* bit[0-31]: 屏蔽送给MCU的唤醒投票状态。 */
    } reg;
} SOC_AO_SCTRL_SC_ACPU_VOTE_MSK1_UNION;
#endif
#define SOC_AO_SCTRL_SC_ACPU_VOTE_MSK1_sc_acpusleep_vote_msk1_START  (0)
#define SOC_AO_SCTRL_SC_ACPU_VOTE_MSK1_sc_acpusleep_vote_msk1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_UNION
 结构说明  : SC_ACPU_VOTESTAT0_MSK 寄存器结构定义。地址偏移量:0x928，初值:0x00000000，宽度:32
 寄存器说明: ACPU睡眠投票屏蔽后状态寄存器0。用于回读各投票的状态。1为投票，0为非投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_acpusleep_vote_stat0_msk_0coresight : 1;  /* bit[0]   : [0]： CoreSight掉电请求csyspwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_acpusleep_vote_stat0_msk_1coresight : 1;  /* bit[1]   : [1]： CoreSight掉电请求cdbgpwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_acpusleep_vote_stat0_msk_2ipf       : 1;  /* bit[2]   : [2]： IPF工作状态信号。 */
        unsigned int  sc_acpusleep_vote_stat0_msk_3socp      : 1;  /* bit[3]   : [3]： SOCP工作状态信号。 */
        unsigned int  sc_acpusleep_vote_stat0_msk_7reserved  : 4;  /* bit[4-7] : [7:4]： reserved */
        unsigned int  sc_acpusleep_vote_stat0_msk            : 24; /* bit[8-31]: 屏蔽后送给ACPU的睡眠投票状态
                                                                                 有任意1bit为1表示不允许睡眠，所有bit为1'b0表示允许睡眠，当投票状态从不允许ACPU睡眠转化为允许ACPU睡眠时，投票模块向ACPU产生睡眠中断。 */
    } reg;
} SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_0coresight_START  (0)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_0coresight_END    (0)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_1coresight_START  (1)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_1coresight_END    (1)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_2ipf_START        (2)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_2ipf_END          (2)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_3socp_START       (3)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_3socp_END         (3)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_7reserved_START   (4)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_7reserved_END     (7)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_START             (8)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT0_MSK_sc_acpusleep_vote_stat0_msk_END               (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_UNION
 结构说明  : SC_ACPU_VOTESTAT1_MSK 寄存器结构定义。地址偏移量:0x92C，初值:0x00000000，宽度:32
 寄存器说明: ACPU睡眠唤醒投票屏蔽后状态寄存器1。用于回读各投票的状态。1为投票，0为非投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_acpusleep_vote_stat1_msk_0coresight : 1;  /* bit[0]   : [0]： CoreSight掉电请求csyspwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_acpusleep_vote_stat1_msk_1coresight : 1;  /* bit[1]   : [1]： CoreSight掉电请求cdbgpwupreq
                                                                                 为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  sc_acpusleep_vote_stat1_msk_2ipf       : 1;  /* bit[2]   : [2]： IPF工作状态信号。 */
        unsigned int  sc_acpusleep_vote_stat1_msk_3socp      : 1;  /* bit[3]   : [3]： SOCP工作状态信号。 */
        unsigned int  sc_acpusleep_vote_stat1_msk_7reserved  : 4;  /* bit[4-7] : [7:4]： reserved */
        unsigned int  sc_acpusleep_vote_stat1_msk            : 24; /* bit[8-31]: 屏蔽后送给MCU的唤醒投票状态
                                                                                 有任意1bit为1表示不允许睡眠，所有bit为1'b0表示允许睡眠，当投票状态从允许ACPU睡眠转化为不允许ACPU睡眠时，投票模块向MCU产生唤醒中断。 */
    } reg;
} SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_0coresight_START  (0)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_0coresight_END    (0)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_1coresight_START  (1)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_1coresight_END    (1)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_2ipf_START        (2)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_2ipf_END          (2)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_3socp_START       (3)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_3socp_END         (3)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_7reserved_START   (4)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_7reserved_END     (7)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_START             (8)
#define SOC_AO_SCTRL_SC_ACPU_VOTESTAT1_MSK_sc_acpusleep_vote_stat1_msk_END               (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTEEN_UNION
 结构说明  : SC_MCU_VOTEEN 寄存器结构定义。地址偏移量:0x930，初值:0x00000000，宽度:32
 寄存器说明: MCU投票产生寄存器0。对比特写1，对应产生投票状态；写0不影响投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote_en0 : 32; /* bit[0-31]: MCU/ACPU/CCPU/HiFi投票使能寄存器0。用于使能投票。对比特写1，投票状态有效；写0不影响投票状态。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTEEN_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTEEN_sc_mcu_vote_en0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTEEN_sc_mcu_vote_en0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTEDIS_UNION
 结构说明  : SC_MCU_VOTEDIS 寄存器结构定义。地址偏移量:0x934，初值:0x00000000，宽度:32
 寄存器说明: MCU投票禁止寄存器0。对比特写1，清除投票状态；写0不影响投票使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote_dis0 : 32; /* bit[0-31]: MCU/ACPU/CCPU/HiFi投票禁止寄存器0。用于禁止投票。对比特写1，解除投票状态；写0不影响投票使能的状态。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTEDIS_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTEDIS_sc_mcu_vote_dis0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTEDIS_sc_mcu_vote_dis0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTESTAT_UNION
 结构说明  : SC_MCU_VOTESTAT 寄存器结构定义。地址偏移量:0x938，初值:0x00000000，宽度:32
 寄存器说明: MCU投票状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote_stat0 : 32; /* bit[0-31]: MCU/ACPU/CCPU/HiFi投票投票状态寄存器0。用于回读各投票的状态。1为投票状态，0为非投票状态。
                                                            软件配置产生中断，有任意1bit为1表示不允许，所有bit为1'b0表示允许
                                                            1、当投票状态从不允许状态转化为允许状态时，投票模块可选向MCU/ACPU/CCPU/HiFi产生中断
                                                            2、当投票状态从允许转化为不允许时，投票模块可选向MCU/ACPU/CCPU/HiFi产生中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTESTAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTESTAT_sc_mcu_vote_stat0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTESTAT_sc_mcu_vote_stat0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE_MSK0_UNION
 结构说明  : SC_MCU_VOTE_MSK0 寄存器结构定义。地址偏移量:0x940，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器0的屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote_msk0 : 32; /* bit[0-31]: 屏蔽送给MCU/ACPU/CCPU/HiFi的允许状态(SC_MCU_VOTESTAT0由非全0变成全0)。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE_MSK0_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE_MSK0_sc_mcu_vote_msk0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE_MSK0_sc_mcu_vote_msk0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE_MSK1_UNION
 结构说明  : SC_MCU_VOTE_MSK1 寄存器结构定义。地址偏移量:0x944，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器0的屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote_msk1 : 32; /* bit[0-31]: 屏蔽送给MCU/ACPU/CCPU/HiFi的不允许状态(SC_MCU_VOTESTAT0由全0变成非全0)。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE_MSK1_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE_MSK1_sc_mcu_vote_msk1_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE_MSK1_sc_mcu_vote_msk1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTESTAT0_MSK_UNION
 结构说明  : SC_MCU_VOTESTAT0_MSK 寄存器结构定义。地址偏移量:0x948，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器0屏蔽后状态寄存器0。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote_stat0_msk : 32; /* bit[0-31]: 屏蔽后送给MCU/ACPU/CCPU/HiFi的允许中断状态
                                                                有任意1bit为1表示不允许状态，所有bit为1'b0表示允许状态，当投票状态从不允许状态转化为允许状态时，投票模块向MCU/ACPU/CCPU/HiFi产生允许中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTESTAT0_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTESTAT0_MSK_sc_mcu_vote_stat0_msk_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTESTAT0_MSK_sc_mcu_vote_stat0_msk_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTESTAT1_MSK_UNION
 结构说明  : SC_MCU_VOTESTAT1_MSK 寄存器结构定义。地址偏移量:0x94C，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器0屏蔽后状态寄存器1。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote_stat1_msk : 32; /* bit[0-31]: 屏蔽后送给MCU/ACPU/CCPU/HiFi的不允许中断状态
                                                                有任意1bit为1表示不允状态，所有bit为1'b0表示允许状态，当投票状态从允许状态转化为不允许状态时，投票模块向MCU/ACPU/CCPU/HiFi产生唤醒中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTESTAT1_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTESTAT1_MSK_sc_mcu_vote_stat1_msk_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTESTAT1_MSK_sc_mcu_vote_stat1_msk_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE1EN_UNION
 结构说明  : SC_MCU_VOTE1EN 寄存器结构定义。地址偏移量:0x960，初值:0x00000000，宽度:32
 寄存器说明: MCU投票产生寄存器1。对比特写1，对应产生投票状态；写0不影响投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote1_en0 : 32; /* bit[0-31]: MCU/ACPU/CCPU/HiFi投票使能寄存器1。用于使能投票。对比特写1，投票状态有效；写0不影响投票状态。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE1EN_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE1EN_sc_mcu_vote1_en0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE1EN_sc_mcu_vote1_en0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE1DIS_UNION
 结构说明  : SC_MCU_VOTE1DIS 寄存器结构定义。地址偏移量:0x964，初值:0x00000000，宽度:32
 寄存器说明: MCU投票禁止寄存器1。对比特写1，清除投票状态；写0不影响投票使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote1_dis0 : 32; /* bit[0-31]: MCU/ACPU/CCPU/HiFi投票禁止寄存器1。用于禁止投票。对比特写1，解除投票状态；写0不影响投票使能的状态。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE1DIS_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE1DIS_sc_mcu_vote1_dis0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE1DIS_sc_mcu_vote1_dis0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE1STAT_UNION
 结构说明  : SC_MCU_VOTE1STAT 寄存器结构定义。地址偏移量:0x968，初值:0x00000000，宽度:32
 寄存器说明: MCU投票状态寄存器1。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote1_stat0 : 32; /* bit[0-31]: MCU/ACPU/CCPU/HiFi投票投票状态寄存器1。用于回读各投票的状态。1为投票状态，0为非投票状态。
                                                             软件配置产生中断，有任意1bit为1表示不允许，所有bit为1'b0表示允许
                                                             1、当投票状态从不允许状态转化为允许状态时，投票模块可选向MCU/ACPU/CCPU/HiFi产生中断
                                                             2、当投票状态从允许转化为不允许时，投票模块可选向MCU/ACPU/CCPU/HiFi产生中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE1STAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE1STAT_sc_mcu_vote1_stat0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE1STAT_sc_mcu_vote1_stat0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE1_MSK0_UNION
 结构说明  : SC_MCU_VOTE1_MSK0 寄存器结构定义。地址偏移量:0x970，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器1的屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote1_msk0 : 32; /* bit[0-31]: 屏蔽送给MCU/ACPU/CCPU/HiFi的允许状态
                                                            (SC_MCU_VOTE1STAT0由非全0变成全0)。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE1_MSK0_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE1_MSK0_sc_mcu_vote1_msk0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE1_MSK0_sc_mcu_vote1_msk0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE1_MSK1_UNION
 结构说明  : SC_MCU_VOTE1_MSK1 寄存器结构定义。地址偏移量:0x974，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器1的屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote1_msk1 : 32; /* bit[0-31]: 屏蔽送给MCU/ACPU/CCPU/HiFi的不允许状态
                                                            (SC_MCU_VOTE1STAT0由全0变成非全0)。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE1_MSK1_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE1_MSK1_sc_mcu_vote1_msk1_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE1_MSK1_sc_mcu_vote1_msk1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE1STAT0_MSK_UNION
 结构说明  : SC_MCU_VOTE1STAT0_MSK 寄存器结构定义。地址偏移量:0x978，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器1屏蔽后状态寄存器0，可用于SYSNOC 150MHz总线DFS控制。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote1_stat0_msk : 32; /* bit[0-31]: 屏蔽后送给MCU/ACPU/CCPU/HiFi的允许中断状态
                                                                 有任意1bit为1表示不允许状态，所有bit为1'b0表示允许状态，当投票状态从不允许状态转化为允许状态时，投票模块向MCU/ACPU/CCPU/HiFi产生允许中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE1STAT0_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE1STAT0_MSK_sc_mcu_vote1_stat0_msk_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE1STAT0_MSK_sc_mcu_vote1_stat0_msk_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE1STAT1_MSK_UNION
 结构说明  : SC_MCU_VOTE1STAT1_MSK 寄存器结构定义。地址偏移量:0x97C，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器1屏蔽后状态寄存器1。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote1_stat1_msk : 32; /* bit[0-31]: 屏蔽后送给MCU/ACPU/CCPU/HiFi的不允许中断状态
                                                                 有任意1bit为1表示不允状态，所有bit为1'b0表示允许状态，当投票状态从允许状态转化为不允许状态时，投票模块向MCU/ACPU/CCPU/HiFi产生唤醒中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE1STAT1_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE1STAT1_MSK_sc_mcu_vote1_stat1_msk_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE1STAT1_MSK_sc_mcu_vote1_stat1_msk_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE2EN_UNION
 结构说明  : SC_MCU_VOTE2EN 寄存器结构定义。地址偏移量:0x980，初值:0x00000000，宽度:32
 寄存器说明: MCU投票产生寄存器2。对比特写1，对应产生投票状态；写0不影响投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote2_en0 : 32; /* bit[0-31]: MCU/ACPU/CCPU/HiFi投票使能寄存器2。用于使能投票。对比特写1，投票状态有效；写0不影响投票状态。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE2EN_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE2EN_sc_mcu_vote2_en0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE2EN_sc_mcu_vote2_en0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE2DIS_UNION
 结构说明  : SC_MCU_VOTE2DIS 寄存器结构定义。地址偏移量:0x984，初值:0x00000000，宽度:32
 寄存器说明: MCU投票禁止寄存器2。对比特写1，清除投票状态；写0不影响投票使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote2_dis0 : 32; /* bit[0-31]: MCU/ACPU/CCPU/HiFi投票禁止寄存器2。用于禁止投票。对比特写1，解除投票状态；写0不影响投票使能的状态。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE2DIS_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE2DIS_sc_mcu_vote2_dis0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE2DIS_sc_mcu_vote2_dis0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE2STAT_UNION
 结构说明  : SC_MCU_VOTE2STAT 寄存器结构定义。地址偏移量:0x988，初值:0x00000000，宽度:32
 寄存器说明: MCU投票状态寄存器2。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote2_stat0 : 32; /* bit[0-31]: MCU/ACPU/CCPU/HiFi投票投票状态寄存器2。用于回读各投票的状态。1为投票状态，0为非投票状态。
                                                             软件配置产生中断，有任意1bit为1表示不允许，所有bit为1'b0表示允许
                                                             1、当投票状态从不允许状态转化为允许状态时，投票模块可选向MCU/ACPU/CCPU/HiFi产生中断
                                                             2、当投票状态从允许转化为不允许时，投票模块可选向MCU/ACPU/CCPU/HiFi产生中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE2STAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE2STAT_sc_mcu_vote2_stat0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE2STAT_sc_mcu_vote2_stat0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE2_MSK0_UNION
 结构说明  : SC_MCU_VOTE2_MSK0 寄存器结构定义。地址偏移量:0x990，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器2的屏蔽寄存器0。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote2_msk0 : 32; /* bit[0-31]: 屏蔽送给MCU/ACPU/CCPU/HiFi的允许状态
                                                            (SC_MCU_VOTE2STAT0由非全0变成全0)。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE2_MSK0_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE2_MSK0_sc_mcu_vote2_msk0_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE2_MSK0_sc_mcu_vote2_msk0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE2_MSK1_UNION
 结构说明  : SC_MCU_VOTE2_MSK1 寄存器结构定义。地址偏移量:0x994，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器2的屏蔽寄存器1。对比特写1，对应取消屏蔽；写0屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote2_msk1 : 32; /* bit[0-31]: 屏蔽送给MCU/ACPU/CCPU/HiFi的不允许状态
                                                            (SC_MCU_VOTE2STAT0由全0变成非全0)。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE2_MSK1_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE2_MSK1_sc_mcu_vote2_msk1_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE2_MSK1_sc_mcu_vote2_msk1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE2STAT0_MSK_UNION
 结构说明  : SC_MCU_VOTE2STAT0_MSK 寄存器结构定义。地址偏移量:0x998，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器2屏蔽后状态寄存器0，可用于SYSNOC/CFGBUS/PERI_DMA BUS三个100MHz总线DFS控制。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote2_stat0_msk : 32; /* bit[0-31]: 屏蔽后送给MCU/ACPU/CCPU/HiFi的允许中断状态
                                                                 可用于指示是否允许总线降频模式；
                                                                 有任意1bit为1表示不允许状态，所有bit为1'b0表示允许状态，当投票状态从不允许状态转化为允许状态时，投票模块向MCU/ACPU/CCPU/HiFi产生允许中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE2STAT0_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE2STAT0_MSK_sc_mcu_vote2_stat0_msk_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE2STAT0_MSK_sc_mcu_vote2_stat0_msk_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_MCU_VOTE2STAT1_MSK_UNION
 结构说明  : SC_MCU_VOTE2STAT1_MSK 寄存器结构定义。地址偏移量:0x99C，初值:0x00000000，宽度:32
 寄存器说明: MCU投票寄存器2屏蔽后状态寄存器1。用于回读各投票的状态。1为有投票状态，0为无投票状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_mcu_vote2_stat1_msk : 32; /* bit[0-31]: 屏蔽后送给MCU/ACPU/CCPU/HiFi的不允许中断状态
                                                                 有任意1bit为1表示不允状态，所有bit为1'b0表示允许状态，当投票状态从允许状态转化为不允许状态时，投票模块向MCU/ACPU/CCPU/HiFi产生唤醒中断。 */
    } reg;
} SOC_AO_SCTRL_SC_MCU_VOTE2STAT1_MSK_UNION;
#endif
#define SOC_AO_SCTRL_SC_MCU_VOTE2STAT1_MSK_sc_mcu_vote2_stat1_msk_START  (0)
#define SOC_AO_SCTRL_SC_MCU_VOTE2STAT1_MSK_sc_mcu_vote2_stat1_msk_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_VOTE_CTRL_UNION
 结构说明  : SC_VOTE_CTRL 寄存器结构定义。地址偏移量:0x9A0，初值:0x00000000，宽度:32
 寄存器说明: 投票控制寄存器，功能包含投票旗语互斥。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vote0_reg_semphen_req : 1;  /* bit[0]   : 投票寄存器0对应的旗语互斥寄存器，用于软件申请投票状态，CPU1在SC_MCU_VOTE0投票寄存器为全0时，通过配置此bit占用系统某种资源，如果其他CPU需要释放此资源，则需要等待vote0_semphsta=1'b0后并且将SC_MCU_VOTE0投票寄存器改为非全0才能实现。
                                                                1'b0： 不申请占用资源；
                                                                1'b1： 申请占用资源。 */
        unsigned int  vote1_reg_semphen_req : 1;  /* bit[1]   : 投票寄存器1对应的旗语互斥寄存器，用于软件申请投票状态，CPU1在SC_MCU_VOTE1投票寄存器为全0时，通过配置此bit占用系统某种资源，如果其他CPU需要释放此资源，则需要等待vote1_semphsta=1'b0后并且将SC_MCU_VOTE1投票寄存器改为非全0才能实现。
                                                                1'b0： 不申请占用资源；
                                                                1'b1： 申请占用资源。 */
        unsigned int  vote2_reg_semphen_req : 1;  /* bit[2]   : 投票寄存器2对应的旗语互斥寄存器，用于软件申请投票状态，CPU1在SC_MCU_VOTE2投票寄存器为全0时，通过配置此bit占用系统某种资源，如果其他CPU需要释放此资源，则需要等待vote2_semphsta=1'b0后并且将SC_MCU_VOTE2投票寄存器改为非全0才能实现。
                                                                1'b0： 不申请占用资源；
                                                                1'b1： 申请占用资源。 */
        unsigned int  reserved              : 29; /* bit[3-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_VOTE_CTRL_UNION;
#endif
#define SOC_AO_SCTRL_SC_VOTE_CTRL_vote0_reg_semphen_req_START  (0)
#define SOC_AO_SCTRL_SC_VOTE_CTRL_vote0_reg_semphen_req_END    (0)
#define SOC_AO_SCTRL_SC_VOTE_CTRL_vote1_reg_semphen_req_START  (1)
#define SOC_AO_SCTRL_SC_VOTE_CTRL_vote1_reg_semphen_req_END    (1)
#define SOC_AO_SCTRL_SC_VOTE_CTRL_vote2_reg_semphen_req_START  (2)
#define SOC_AO_SCTRL_SC_VOTE_CTRL_vote2_reg_semphen_req_END    (2)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_VOTE_STAT_UNION
 结构说明  : SC_VOTE_STAT 寄存器结构定义。地址偏移量:0x9A4，初值:0x00000000，宽度:32
 寄存器说明: 投票控制寄存器，功能包含投票旗语互斥状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vote0_semphsta      : 1;  /* bit[0]   : 投票寄存器0对应的旗语互斥状态寄存器，用于软件查看投票状态，CPU1在SC_MCU_VOTE0投票寄存器为全0时，通过配置vote0_reg_semphen_req占用系统某种资源，申请成功后此bit变为1'1b，如果其他CPU需要释放此资源，则需要等待vote0_semphsta=1'b0后并且将SC_MCU_VOTE0投票寄存器改为非全0才能实现。
                                                              1'b0： 占用资源已释放。
                                                              1'b1： 申请获得允许占用资源。 */
        unsigned int  vote1_semphsta      : 1;  /* bit[1]   : 投票寄存器1对应的旗语互斥状态寄存器，用于软件查看投票状态，，CPU1在SC_MCU_VOTE1投票寄存器为全0时，通过配置vote1_reg_semphen_req占用系统某种资源，申请成功后此bit变为1'1b，如果其他CPU需要释放此资源，则需要等待vote1_semphsta=1'b0后并且将SC_MCU_VOTE1投票寄存器改为非全0才能实现。
                                                              1'b0： 占用资源已释放；
                                                              1'b1： 申请获得允许占用资源。 */
        unsigned int  vote2_semphsta      : 1;  /* bit[2]   : 投票寄存器2对应的旗语互斥状态寄存器，用于软件查看投票状态，CPU1在SC_MCU_VOTE2投票寄存器为全0时，通过配置vote2_reg_semphen_req占用系统某种资源，申请成功后此bit变为1'1b，如果其他CPU需要释放此资源，则需要等待vote2_semphsta=1'b0后并且将SC_MCU_VOTE2投票寄存器改为非全0才能实现。
                                                              1'b0： 占用资源已释放；
                                                              1'b1： 申请获得允许占用资源。 */
        unsigned int  reserved_0          : 1;  /* bit[3]   : reserved */
        unsigned int  coresight_wakeup_en : 2;  /* bit[4-5] : coresight_wakeup_en[1]： CoreSight掉电请求cdbgpwupreq
                                                              为0表示允许睡眠，为1表示不允许睡眠，需要唤醒
                                                              coresight_wakeup_en[0]： CoreSight掉电请求csyspwupreq
                                                              为0表示允许睡眠，为1表示不允许睡眠，需要唤醒。 */
        unsigned int  ipf_wakeup_en       : 1;  /* bit[6]   : IPF工作状态信号。 */
        unsigned int  socp_wakeup_en      : 1;  /* bit[7]   : SOCP工作状态信号。 */
        unsigned int  reserved_1          : 24; /* bit[8-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_VOTE_STAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_VOTE_STAT_vote0_semphsta_START       (0)
#define SOC_AO_SCTRL_SC_VOTE_STAT_vote0_semphsta_END         (0)
#define SOC_AO_SCTRL_SC_VOTE_STAT_vote1_semphsta_START       (1)
#define SOC_AO_SCTRL_SC_VOTE_STAT_vote1_semphsta_END         (1)
#define SOC_AO_SCTRL_SC_VOTE_STAT_vote2_semphsta_START       (2)
#define SOC_AO_SCTRL_SC_VOTE_STAT_vote2_semphsta_END         (2)
#define SOC_AO_SCTRL_SC_VOTE_STAT_coresight_wakeup_en_START  (4)
#define SOC_AO_SCTRL_SC_VOTE_STAT_coresight_wakeup_en_END    (5)
#define SOC_AO_SCTRL_SC_VOTE_STAT_ipf_wakeup_en_START        (6)
#define SOC_AO_SCTRL_SC_VOTE_STAT_ipf_wakeup_en_END          (6)
#define SOC_AO_SCTRL_SC_VOTE_STAT_socp_wakeup_en_START       (7)
#define SOC_AO_SCTRL_SC_VOTE_STAT_socp_wakeup_en_END         (7)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ECONUM_UNION
 结构说明  : SC_ECONUM 寄存器结构定义。地址偏移量:0xF00，初值:0x00000000，宽度:32
 寄存器说明: ECO 版本号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_econum : 8;  /* bit[0-7] : ECO版本号，软件可读不可写。 */
        unsigned int  reserved  : 24; /* bit[8-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_ECONUM_UNION;
#endif
#define SOC_AO_SCTRL_SC_ECONUM_sc_econum_START  (0)
#define SOC_AO_SCTRL_SC_ECONUM_sc_econum_END    (7)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SCCHIPID_UNION
 结构说明  : SCCHIPID 寄存器结构定义。地址偏移量:0xF10，初值:0x48696210，宽度:32
 寄存器说明: CHIPID寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  chip_id       : 16; /* bit[0-15] : 16'h6210。 */
        unsigned int  chip_designer : 16; /* bit[16-31]: 16'h4869,”H”的ASCII码是0x48，”i”的ASCII码是0x69。 */
    } reg;
} SOC_AO_SCTRL_SCCHIPID_UNION;
#endif
#define SOC_AO_SCTRL_SCCHIPID_chip_id_START        (0)
#define SOC_AO_SCTRL_SCCHIPID_chip_id_END          (15)
#define SOC_AO_SCTRL_SCCHIPID_chip_designer_START  (16)
#define SOC_AO_SCTRL_SCCHIPID_chip_designer_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SCSOCID_UNION
 结构说明  : SCSOCID 寄存器结构定义。地址偏移量:0xF1C，初值:0x48690010，宽度:32
 寄存器说明: SOCID寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  soc_id       : 16; /* bit[0-15] : 16'h0010。 */
        unsigned int  soc_designer : 16; /* bit[16-31]: 16'h4869,”H”的ASCII码是0x48，”i”的ASCII码是0x69。 */
    } reg;
} SOC_AO_SCTRL_SCSOCID_UNION;
#endif
#define SOC_AO_SCTRL_SCSOCID_soc_id_START        (0)
#define SOC_AO_SCTRL_SCSOCID_soc_id_END          (15)
#define SOC_AO_SCTRL_SCSOCID_soc_designer_START  (16)
#define SOC_AO_SCTRL_SCSOCID_soc_designer_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SOC_FPGA_RTL_DEF_UNION
 结构说明  : SC_SOC_FPGA_RTL_DEF 寄存器结构定义。地址偏移量:0xFE0，初值:0x00000000，宽度:32
 寄存器说明: 外设ID寄存器0。FPGA版本使用，由宏定义确定数值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  soc_fpga_rtl_def : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SOC_FPGA_RTL_DEF_UNION;
#endif
#define SOC_AO_SCTRL_SC_SOC_FPGA_RTL_DEF_soc_fpga_rtl_def_START  (0)
#define SOC_AO_SCTRL_SC_SOC_FPGA_RTL_DEF_soc_fpga_rtl_def_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SOC_FPGA_PR_DEF_UNION
 结构说明  : SC_SOC_FPGA_PR_DEF 寄存器结构定义。地址偏移量:0xFE4，初值:0x00000000，宽度:32
 寄存器说明: 外设ID寄存器1。FPGA版本使用，由宏定义确定数值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  soc_fpga_pr_def : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SOC_FPGA_PR_DEF_UNION;
#endif
#define SOC_AO_SCTRL_SC_SOC_FPGA_PR_DEF_soc_fpga_pr_def_START  (0)
#define SOC_AO_SCTRL_SC_SOC_FPGA_PR_DEF_soc_fpga_pr_def_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF0_UNION
 结构说明  : SC_SOC_FPGA_RES_DEF0 寄存器结构定义。地址偏移量:0xFE8，初值:0x00000000，宽度:32
 寄存器说明: 外设ID寄存器2。FPGA版本使用，由宏定义确定数值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  soc_fpga_res_def0 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF0_UNION;
#endif
#define SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF0_soc_fpga_res_def0_START  (0)
#define SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF0_soc_fpga_res_def0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF1_UNION
 结构说明  : SC_SOC_FPGA_RES_DEF1 寄存器结构定义。地址偏移量:0xFEC，初值:0x00000000，宽度:32
 寄存器说明: 外设ID寄存器3。FPGA版本使用，由宏定义确定数值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  soc_fpga_res_def1 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF1_UNION;
#endif
#define SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF1_soc_fpga_res_def1_START  (0)
#define SOC_AO_SCTRL_SC_SOC_FPGA_RES_DEF1_soc_fpga_res_def1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_XTAL_CTRL0_UNION
 结构说明  : SC_XTAL_CTRL0 寄存器结构定义。地址偏移量:0x1028，初值:0x00000640，宽度:32
 寄存器说明: 晶振控制寄存器。用于对tcxo0晶振进行控制。系统在SLOW和NORMAL模式下，tcxo必须被使能。tcxo ABB稳定延迟配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xtalover0  : 1;  /* bit[0]    : ABB侧设置芯片晶振使能是由软件控制还是由系统状态机控制。仅在调试模式或系统进入SLEEP需要打开TCXO的场景使用；
                                                      0：由状态机控制；
                                                      1：由软件配置xtalen0直接控制。 */
        unsigned int  xtalen0    : 1;  /* bit[1]    : ABB侧晶振使能。
                                                      当xtalover为1时，芯片输出的晶振使能TCXO_EN将由该比特直接驱动。仅在调试模式或系统进入SLEEP需要打开TCXO的场景使用；
                                                      0：关闭晶振；
                                                      1：使能晶振。 */
        unsigned int  reserved_0 : 2;  /* bit[2-3]  : reserved */
        unsigned int  xtal_time0 : 25; /* bit[4-28] : 设置ABB侧TCXO0的稳定时间，单位为32K时钟周期。采用递增计数方式。默认100个周期（约3ms）。 */
        unsigned int  reserved_1 : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_XTAL_CTRL0_UNION;
#endif
#define SOC_AO_SCTRL_SC_XTAL_CTRL0_xtalover0_START   (0)
#define SOC_AO_SCTRL_SC_XTAL_CTRL0_xtalover0_END     (0)
#define SOC_AO_SCTRL_SC_XTAL_CTRL0_xtalen0_START     (1)
#define SOC_AO_SCTRL_SC_XTAL_CTRL0_xtalen0_END       (1)
#define SOC_AO_SCTRL_SC_XTAL_CTRL0_xtal_time0_START  (4)
#define SOC_AO_SCTRL_SC_XTAL_CTRL0_xtal_time0_END    (28)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_XTAL_CTRL1_UNION
 结构说明  : SC_XTAL_CTRL1 寄存器结构定义。地址偏移量:0x102c，初值:0x00000640，宽度:32
 寄存器说明: 晶振控制寄存器。用于对tcxo1晶振进行控制。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xtalover1          : 1;  /* bit[0]    : ABB侧设置芯片晶振使能是由软件控制还是由系统状态机控制（调试功能）。仅在调试模式或系统进入SLEEP需要打开TCXO的场景使用；
                                                              0：由状态机控制；
                                                              1：由软件配置xtalen1直接控制。 */
        unsigned int  xtalen1            : 1;  /* bit[1]    : ABB侧晶振使能（调试功能）。
                                                              当xtalover为1时，芯片输出的晶振使能TCXO_EN将由该比特直接驱动。仅在调试模式或系统进入SLEEP需要打开TCXO的场景使用；
                                                              0：关闭晶振；
                                                              1：使能晶振。 */
        unsigned int  tcxo1_hard_ctrl_en : 1;  /* bit[2]    : tcxo1由硬件控制开关选择
                                                              0：tcxo1支持软件打开和关闭；同时支持唤醒过程中bbpdrx唤醒中断自动打开；支持睡眠过程中自动关闭；
                                                              1：tcxo1只支持唤醒和睡眠过程中由硬件进行打开和关闭。 */
        unsigned int  xtal1_sft_cfg      : 1;  /* bit[3]    : ABB侧晶振使能。
                                                              0：关闭晶振；
                                                              1：使能晶振。
                                                              注：软件在清除gbbp1唤醒中断前，需要保证软件使用软件方式打开此bit。 */
        unsigned int  xtal_time1         : 25; /* bit[4-28] : 设置ABB侧TCXO1的稳定时间，单位为32K时钟周期。采用递增计数方式。默认100个周期（约3ms），软件方式打开时，tcxo1的pmu侧电源，公用tcxo0的，在系统唤醒时默认打开，无需电源等待稳定，只需要配置等待1ms就可以。 */
        unsigned int  reserved           : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_XTAL_CTRL1_UNION;
#endif
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_xtalover1_START           (0)
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_xtalover1_END             (0)
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_xtalen1_START             (1)
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_xtalen1_END               (1)
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_tcxo1_hard_ctrl_en_START  (2)
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_tcxo1_hard_ctrl_en_END    (2)
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_xtal1_sft_cfg_START       (3)
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_xtal1_sft_cfg_END         (3)
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_xtal_time1_START          (4)
#define SOC_AO_SCTRL_SC_XTAL_CTRL1_xtal_time1_END            (28)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_XTAL_CTRL3_UNION
 结构说明  : SC_XTAL_CTRL3 寄存器结构定义。地址偏移量:0x1034，初值:0x00000000，宽度:32
 寄存器说明: 晶振控制寄存器。tcxo0 PMU侧使能，仅在调试模式或系统进入SLEEP需要打开tcxo的场景使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xtalover0_pmu_up : 1;  /* bit[0]   : TCXO的上电设置芯片晶振电源TCXO_EN_PMU是由软件控制还是由系统状态机控制。仅在调试模式或系统进入SLEEP需要打开TCXO的场景使用；
                                                           0：由状态机控制；
                                                           1：由软件控制。 */
        unsigned int  xtalen0_pmu_up   : 1;  /* bit[1]   : TCXO的上电晶振使能。
                                                           当xtalover为1时，芯片输出的晶振电源TCXO_EN_PMU将由该比特直接驱动。仅在调试模式或系统进入SLEEP需要打开TCXO的场景使用；
                                                           0：关闭晶振；
                                                           1：使能晶振。 */
        unsigned int  reserved         : 30; /* bit[2-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_XTAL_CTRL3_UNION;
#endif
#define SOC_AO_SCTRL_SC_XTAL_CTRL3_xtalover0_pmu_up_START  (0)
#define SOC_AO_SCTRL_SC_XTAL_CTRL3_xtalover0_pmu_up_END    (0)
#define SOC_AO_SCTRL_SC_XTAL_CTRL3_xtalen0_pmu_up_START    (1)
#define SOC_AO_SCTRL_SC_XTAL_CTRL3_xtalen0_pmu_up_END      (1)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_XTAL_CTRL5_UNION
 结构说明  : SC_XTAL_CTRL5 寄存器结构定义。地址偏移量:0x103C，初值:0x00000030，宽度:32
 寄存器说明: 晶振控制寄存器。tcxo0下电流程，tcxo0电源关闭到tcxo0 ABB侧使能关闭延迟时间配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0        : 4;  /* bit[0-3]  : reserved */
        unsigned int  xtal0_time_pmu_dn : 25; /* bit[4-28] : 设置TCXO的下电时PMU侧关闭到ABB Buffer关闭之间的延时时间，单位为32K时钟周期。采用递增计数方式。默认7个周期，推荐值0x5=5个周期，约150us。
                                                             计数器=软件配置值+2。 */
        unsigned int  reserved_1        : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_XTAL_CTRL5_UNION;
#endif
#define SOC_AO_SCTRL_SC_XTAL_CTRL5_xtal0_time_pmu_dn_START  (4)
#define SOC_AO_SCTRL_SC_XTAL_CTRL5_xtal0_time_pmu_dn_END    (28)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_XTAL_STAT0_UNION
 结构说明  : SC_XTAL_STAT0 寄存器结构定义。地址偏移量:0x1060，初值:0x33680000，宽度:32
 寄存器说明: 晶振tcxo0状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0         : 19; /* bit[0-18] : reserved */
        unsigned int  tcxo0_abb_timeout  : 1;  /* bit[19]   : ABB侧XTAL状态指示。
                                                              0：未稳定；
                                                              1：已稳定。 */
        unsigned int  tcxo0_pwdn_timeout : 1;  /* bit[20]   : TCXO的下电状态指示。
                                                              0：未稳定；
                                                              1：已稳定。 */
        unsigned int  tcxo_en_hard       : 1;  /* bit[21]   : 系统模式切换状态控制的tcxo使能状态。 */
        unsigned int  buck3_en_out       : 1;  /* bit[22]   : 状态机输出外设区电源使能状态。 */
        unsigned int  reserved_1         : 1;  /* bit[23]   : reserved */
        unsigned int  tcxo0_curr_st      : 4;  /* bit[24-27]: tcxo0状态机状态
                                                              001： TCXO_S_ON_PMU；
                                                              010： TCXO_S_ON_ABB；
                                                              011： TCXO_S_ON_STB；
                                                              100： TCXO_S_OFF_STB；
                                                              110： TCXO_S_OFF_PMU；
                                                              others： reserved。 */
        unsigned int  xtal_en0_pmu       : 1;  /* bit[28]   : tcxo0状态机和软件控制综合输出pmu侧上电使能状态
                                                              0：tcxo0 pmu侧使能信号关闭；
                                                              1：tcxo0 pmu侧使能信号打开。 */
        unsigned int  xtal_en0_abb       : 1;  /* bit[29]   : tcxo0状态机和软件控制综合输出abb侧使能后状态
                                                              0：tcxo0 abb侧使能信号关闭；
                                                              1：tcxo0 abb侧使能信号打开。 */
        unsigned int  reserved_2         : 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_XTAL_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_XTAL_STAT0_tcxo0_abb_timeout_START   (19)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_tcxo0_abb_timeout_END     (19)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_tcxo0_pwdn_timeout_START  (20)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_tcxo0_pwdn_timeout_END    (20)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_tcxo_en_hard_START        (21)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_tcxo_en_hard_END          (21)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_buck3_en_out_START        (22)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_buck3_en_out_END          (22)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_tcxo0_curr_st_START       (24)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_tcxo0_curr_st_END         (27)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_xtal_en0_pmu_START        (28)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_xtal_en0_pmu_END          (28)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_xtal_en0_abb_START        (29)
#define SOC_AO_SCTRL_SC_XTAL_STAT0_xtal_en0_abb_END          (29)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_XTAL_STAT1_UNION
 结构说明  : SC_XTAL_STAT1 寄存器结构定义。地址偏移量:0x1070，初值:0x04000000，宽度:32
 寄存器说明: 晶振tcxo1状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0         : 19; /* bit[0-18] : reserved */
        unsigned int  tcxo1_abb_timeout  : 1;  /* bit[19]   : ABB侧XTAL状态指示。
                                                              0：未稳定；
                                                              1：已稳定。 */
        unsigned int  tcxo1_en_nosoc_drx : 1;  /* bit[20]   : 系统唤醒过程中，gbb1中断唤醒系统时，自动开启tcxo1请求
                                                              0：tcxo1 no open req，在系统状态唤醒时无gbbp1中断，或唤醒后，清除gbb1中断，此bit清零；
                                                              1：tcxo1 open req，在系统状态唤醒时有gbbp1中断时，此bit置1。 */
        unsigned int  reserved_1         : 1;  /* bit[21]   : reserved */
        unsigned int  reserved_2         : 1;  /* bit[22]   : reserved */
        unsigned int  reserved_3         : 1;  /* bit[23]   : reserved */
        unsigned int  tcxo1_curr_st      : 4;  /* bit[24-27]: tcxo1状态机状态
                                                              001： TCXO_S_ON_PMU；
                                                              010： TCXO_S_ON_ABB；
                                                              011： TCXO_S_ON_STB；
                                                              100： TCXO_S_OFF_STB；
                                                              110： TCXO_S_OFF_PMU；
                                                              others： reserved。 */
        unsigned int  reserved_4         : 1;  /* bit[28]   : reserved */
        unsigned int  xtal_en1_abb       : 1;  /* bit[29]   : tcxo1状态机和软件控制综合输出abb侧使能后状态
                                                              0：tcxo1 abb侧使能信号关闭；
                                                              1：tcxo1 abb侧使能信号打开。 */
        unsigned int  reserved_5         : 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_XTAL_STAT1_UNION;
#endif
#define SOC_AO_SCTRL_SC_XTAL_STAT1_tcxo1_abb_timeout_START   (19)
#define SOC_AO_SCTRL_SC_XTAL_STAT1_tcxo1_abb_timeout_END     (19)
#define SOC_AO_SCTRL_SC_XTAL_STAT1_tcxo1_en_nosoc_drx_START  (20)
#define SOC_AO_SCTRL_SC_XTAL_STAT1_tcxo1_en_nosoc_drx_END    (20)
#define SOC_AO_SCTRL_SC_XTAL_STAT1_tcxo1_curr_st_START       (24)
#define SOC_AO_SCTRL_SC_XTAL_STAT1_tcxo1_curr_st_END         (27)
#define SOC_AO_SCTRL_SC_XTAL_STAT1_xtal_en1_abb_START        (29)
#define SOC_AO_SCTRL_SC_XTAL_STAT1_xtal_en1_abb_END          (29)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_EFUSE_CHIPID0_UNION
 结构说明  : SC_EFUSE_CHIPID0 寄存器结构定义。地址偏移量:0x1080，初值:0x00000000，宽度:32
 寄存器说明: EFUSE_CHIPID状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  efuse_chipio0 : 32; /* bit[0-31]: EFUSE_CHIPID[31;0]。 */
    } reg;
} SOC_AO_SCTRL_SC_EFUSE_CHIPID0_UNION;
#endif
#define SOC_AO_SCTRL_SC_EFUSE_CHIPID0_efuse_chipio0_START  (0)
#define SOC_AO_SCTRL_SC_EFUSE_CHIPID0_efuse_chipio0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_EFUSE_CHIPID1_UNION
 结构说明  : SC_EFUSE_CHIPID1 寄存器结构定义。地址偏移量:0x1084，初值:0x00000000，宽度:32
 寄存器说明: EFUSE_CHIPID状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  efuse_chipio1 : 32; /* bit[0-31]: EFUSE_CHIPID[63;32]。 */
    } reg;
} SOC_AO_SCTRL_SC_EFUSE_CHIPID1_UNION;
#endif
#define SOC_AO_SCTRL_SC_EFUSE_CHIPID1_efuse_chipio1_START  (0)
#define SOC_AO_SCTRL_SC_EFUSE_CHIPID1_efuse_chipio1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_EFUSE_SYS_CTRL_UNION
 结构说明  : SC_EFUSE_SYS_CTRL 寄存器结构定义。地址偏移量:0x1088，初值:0x00000000，宽度:32
 寄存器说明: EFUSE_SYS_CTRL状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  efuse_sys_ctrl : 32; /* bit[0-31]: EFUSE_SYS_CTRL
                                                         EFUSE控制信息的990~1023。 */
    } reg;
} SOC_AO_SCTRL_SC_EFUSE_SYS_CTRL_UNION;
#endif
#define SOC_AO_SCTRL_SC_EFUSE_SYS_CTRL_efuse_sys_ctrl_START  (0)
#define SOC_AO_SCTRL_SC_EFUSE_SYS_CTRL_efuse_sys_ctrl_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_DEBUG_CTRL1_UNION
 结构说明  : SC_DEBUG_CTRL1 寄存器结构定义。地址偏移量:0x1288，初值:0x00004400，宽度:32
 寄存器说明: 安全DEBUG控制寄存器1。用于控制ACPU、CCPU、MCU、CSSYS、HIFI、BBE调试、DJTAG功能，保证SC_DEBUG_CTRL1寄存器的值不被DFT控制（这个通过安全DFT实现），此寄存器只能写入1次。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0      : 1;  /* bit[0] : reserved */
        unsigned int  reserved_1      : 1;  /* bit[1] : reserved */
        unsigned int  reserved_2      : 1;  /* bit[2] : reserved */
        unsigned int  reserved_3      : 1;  /* bit[3] : reserved */
        unsigned int  reserved_4      : 1;  /* bit[4] : reserved */
        unsigned int  reserved_5      : 1;  /* bit[5] : reserved */
        unsigned int  reserved_6      : 1;  /* bit[6] : reserved */
        unsigned int  reserved_7      : 1;  /* bit[7] : reserved */
        unsigned int  aarm_spniden    : 1;  /* bit[8] : ACPU Secure privileged noninvasive debug使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  aarm_spiden     : 1;  /* bit[9] : ACPU Secure privileged invasive debug 使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  aarm_niden      : 1;  /* bit[10]: ACPU Noninvasive debug使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  aarm_dbgen      : 1;  /* bit[11]: ACPU Invasive debug使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  carm_spniden    : 1;  /* bit[12]: Secure privileged noninvasive debug使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  carm_spiden     : 1;  /* bit[13]: Secure privileged invasive debug 使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  carm_niden      : 1;  /* bit[14]: Noninvasive debug使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  carm_dbgen      : 1;  /* bit[15]: Invasive debug使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  cssys_spniden   : 1;  /* bit[16]: cssys spniden 
                                                        0： Secure Non-Invasive Debug Disable；
                                                        1： Secure Non-Invasive Debug Enable。 */
        unsigned int  cssys_spiden    : 1;  /* bit[17]: cssys spiden 
                                                        0： Secure Invasive Debug Disable；
                                                        1： Secure Invasive Debug Enable。 */
        unsigned int  cssys_niden     : 1;  /* bit[18]: cssys niden 
                                                        0： Non-Invasive Debug Disable；
                                                        1：Non-Invasive Debug Enable。 */
        unsigned int  cssys_dbgen     : 1;  /* bit[19]: cssys dbgen 
                                                        0： Invasive Debug Disable；
                                                        1：Invasive Debug Enable。 */
        unsigned int  cssys_device_en : 1;  /* bit[20]: cssys device enable APB AP接口使能
                                                        0： cssys device disable；
                                                        1： cssys device enable。
                                                        If APB-AP is connected to the Debug APB, that is, a bus connected only to
                                                        debug and trace components, it must be permanently enabled by tying
                                                        DEVICEEN HIGH. This ensures that trace components can still be
                                                        programmed when DBGEN is LOW. In practice, it is expected that the
                                                        APB-AP is almost always used in this way.
                                                        If APB-AP is connected to a system APB dedicated to the non-secure world,
                                                        DEVICEEN must be connected to DBGEN.
                                                        If APB-AP is connected to a system APB dedicated to the secure world,
                                                        DEVICEEN must be connected to SPIDEN。 */
        unsigned int  m3_dbgen        : 1;  /* bit[21]: m3 debug enable 
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  hifi_jtag_en    : 1;  /* bit[22]: hifi jtag enable
                                                        0： jtag disable；
                                                        1： jtag enable。 */
        unsigned int  bbe_jtag_en     : 1;  /* bit[23]: bbe jtag enable
                                                        0： jtag disable；
                                                        1： jtag enable。 */
        unsigned int  g3d_dbgen       : 1;  /* bit[24]: g3d ddr debug enable 
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  djtag_en        : 1;  /* bit[25]: debug_jtag enable
                                                        0： jtag disable；
                                                        1： jtag enable。 */
        unsigned int  cssys_jtag_en   : 1;  /* bit[26]: cssys_jtag enable
                                                        0： jtag disable；
                                                        1： jtag enable。 */
        unsigned int  reserved_8      : 1;  /* bit[27]: reserved */
        unsigned int  reserved_9      : 1;  /* bit[28]: reserved */
        unsigned int  reserved_10     : 1;  /* bit[29]: reserved */
        unsigned int  reserved_11     : 1;  /* bit[30]: reserved */
        unsigned int  reserved_12     : 1;  /* bit[31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_DEBUG_CTRL1_UNION;
#endif
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_aarm_spniden_START     (8)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_aarm_spniden_END       (8)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_aarm_spiden_START      (9)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_aarm_spiden_END        (9)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_aarm_niden_START       (10)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_aarm_niden_END         (10)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_aarm_dbgen_START       (11)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_aarm_dbgen_END         (11)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_carm_spniden_START     (12)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_carm_spniden_END       (12)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_carm_spiden_START      (13)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_carm_spiden_END        (13)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_carm_niden_START       (14)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_carm_niden_END         (14)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_carm_dbgen_START       (15)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_carm_dbgen_END         (15)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_spniden_START    (16)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_spniden_END      (16)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_spiden_START     (17)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_spiden_END       (17)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_niden_START      (18)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_niden_END        (18)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_dbgen_START      (19)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_dbgen_END        (19)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_device_en_START  (20)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_device_en_END    (20)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_m3_dbgen_START         (21)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_m3_dbgen_END           (21)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_hifi_jtag_en_START     (22)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_hifi_jtag_en_END       (22)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_bbe_jtag_en_START      (23)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_bbe_jtag_en_END        (23)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_g3d_dbgen_START        (24)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_g3d_dbgen_END          (24)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_djtag_en_START         (25)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_djtag_en_END           (25)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_jtag_en_START    (26)
#define SOC_AO_SCTRL_SC_DEBUG_CTRL1_cssys_jtag_en_END      (26)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_DBG_STAT_UNION
 结构说明  : SC_DBG_STAT 寄存器结构定义。地址偏移量:0x12B4，初值:0x00000000，宽度:32
 寄存器说明: 安全DEBUG状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_cfg_arm_dbg_wr_dis : 1;  /* bit[0]   : 安全debug寄存器SC_DEBUG_CTRL1是否写入指示
                                                                0：SC_DEBUG_CTRL1未写入；
                                                                1：:SC_DEBUG_CTRL1已写入1次，并且不再可写。 */
        unsigned int  sc_cfg_arm_dbg_wr     : 1;  /* bit[1]   : 安全debug寄存器SC_DEBUG_CTRL1可写指示
                                                                0：鉴权失败或SC_DEBUG_CTRL1已写过1次，故SC_DEBUG_CTRL1不再可写入；
                                                                1：鉴权通过且SC_DEBUG_CTRL1未写过1次，故SC_DEBUG_CTRL1可写入。 */
        unsigned int  reserved_0            : 1;  /* bit[2]   : reserved */
        unsigned int  sc_arm_dbg_key_wr_en0 : 1;  /* bit[3]   : 安全debug寄存器SC_ARM_DBG_KEY0可写入指示
                                                                11：SC_ARM_DBG_KEY0不可写入；
                                                                others：SC_ARM_DBG_KEY0可写入。 */
        unsigned int  reserved_1            : 2;  /* bit[4-5] : reserved */
        unsigned int  arm_dbg_key_cnt0      : 2;  /* bit[6-7] : 安全debug寄存器SC_ARM_DBG_KEY0写入次数指示。 */
        unsigned int  sec_dbg_key_pass      : 1;  /* bit[8]   : 安全鉴权指示
                                                                0：鉴权未通过；
                                                                1： 鉴权通过。 */
        unsigned int  reserved_2            : 23; /* bit[9-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_DBG_STAT_UNION;
#endif
#define SOC_AO_SCTRL_SC_DBG_STAT_sc_cfg_arm_dbg_wr_dis_START  (0)
#define SOC_AO_SCTRL_SC_DBG_STAT_sc_cfg_arm_dbg_wr_dis_END    (0)
#define SOC_AO_SCTRL_SC_DBG_STAT_sc_cfg_arm_dbg_wr_START      (1)
#define SOC_AO_SCTRL_SC_DBG_STAT_sc_cfg_arm_dbg_wr_END        (1)
#define SOC_AO_SCTRL_SC_DBG_STAT_sc_arm_dbg_key_wr_en0_START  (3)
#define SOC_AO_SCTRL_SC_DBG_STAT_sc_arm_dbg_key_wr_en0_END    (3)
#define SOC_AO_SCTRL_SC_DBG_STAT_arm_dbg_key_cnt0_START       (6)
#define SOC_AO_SCTRL_SC_DBG_STAT_arm_dbg_key_cnt0_END         (7)
#define SOC_AO_SCTRL_SC_DBG_STAT_sec_dbg_key_pass_START       (8)
#define SOC_AO_SCTRL_SC_DBG_STAT_sec_dbg_key_pass_END         (8)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ARM_DBG_KEY0_UNION
 结构说明  : SC_ARM_DBG_KEY0 寄存器结构定义。地址偏移量:0x12B8，初值:0x00000000，宽度:32
 寄存器说明: 安全DEBUG密码寄存器，此寄存器可写入3次，此寄存器不可读。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_arm_dbg_key0 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_ARM_DBG_KEY0_UNION;
#endif
#define SOC_AO_SCTRL_SC_ARM_DBG_KEY0_sc_arm_dbg_key0_START  (0)
#define SOC_AO_SCTRL_SC_ARM_DBG_KEY0_sc_arm_dbg_key0_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_RESERVED31_UNION
 结构说明  : SC_RESERVED31 寄存器结构定义。地址偏移量:0x13A0，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved31 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_RESERVED31_UNION;
#endif
#define SOC_AO_SCTRL_SC_RESERVED31_sc_reserved31_START  (0)
#define SOC_AO_SCTRL_SC_RESERVED31_sc_reserved31_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_RESERVED32_UNION
 结构说明  : SC_RESERVED32 寄存器结构定义。地址偏移量:0x13A4，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved32 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_RESERVED32_UNION;
#endif
#define SOC_AO_SCTRL_SC_RESERVED32_sc_reserved32_START  (0)
#define SOC_AO_SCTRL_SC_RESERVED32_sc_reserved32_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_RESERVED33_UNION
 结构说明  : SC_RESERVED33 寄存器结构定义。地址偏移量:0x13A8，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved33 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_RESERVED33_UNION;
#endif
#define SOC_AO_SCTRL_SC_RESERVED33_sc_reserved33_START  (0)
#define SOC_AO_SCTRL_SC_RESERVED33_sc_reserved33_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_RESERVED34_UNION
 结构说明  : SC_RESERVED34 寄存器结构定义。地址偏移量:0x13AC，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved34 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_RESERVED34_UNION;
#endif
#define SOC_AO_SCTRL_SC_RESERVED34_sc_reserved34_START  (0)
#define SOC_AO_SCTRL_SC_RESERVED34_sc_reserved34_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_RESERVED35_UNION
 结构说明  : SC_RESERVED35 寄存器结构定义。地址偏移量:0x13B0，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved35 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_RESERVED35_UNION;
#endif
#define SOC_AO_SCTRL_SC_RESERVED35_sc_reserved35_START  (0)
#define SOC_AO_SCTRL_SC_RESERVED35_sc_reserved35_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_RESERVED36_UNION
 结构说明  : SC_RESERVED36 寄存器结构定义。地址偏移量:0x13B4，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved36 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_RESERVED36_UNION;
#endif
#define SOC_AO_SCTRL_SC_RESERVED36_sc_reserved36_START  (0)
#define SOC_AO_SCTRL_SC_RESERVED36_sc_reserved36_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_RESERVED37_UNION
 结构说明  : SC_RESERVED37 寄存器结构定义。地址偏移量:0x13B8，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved37 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_RESERVED37_UNION;
#endif
#define SOC_AO_SCTRL_SC_RESERVED37_sc_reserved37_START  (0)
#define SOC_AO_SCTRL_SC_RESERVED37_sc_reserved37_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_RESERVED38_UNION
 结构说明  : SC_RESERVED38 寄存器结构定义。地址偏移量:0x13BC，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器，提供软件测试和调试使用。寄存器数值不随系统复位，芯片上电默认值为x。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved38 : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_RESERVED38_UNION;
#endif
#define SOC_AO_SCTRL_SC_RESERVED38_sc_reserved38_START  (0)
#define SOC_AO_SCTRL_SC_RESERVED38_sc_reserved38_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_UNION
 结构说明  : SC_ALWAYSON_SYS_CTRL0 寄存器结构定义。地址偏移量:0x1480，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON控制寄存器，功能包含CLKSYS_ON分频比配置。
            [31:16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clksys_on_div_cfg              : 3;  /* bit[0-2]  : clksys_on上电区150MHz时钟软件分频比配置，在mcu wfi=0的时候生效，分频比为数值+1。 */
        unsigned int  clksys_on_div_cfg_hard_en      : 1;  /* bit[3]    : clksys_on_div_cfg_hard_wfi功能使能配置
                                                                          0： clksys_on_div_cfg_hard_wfi功能关闭；
                                                                          1： clksys_on_div_cfg_hard_wfi功能使能，clksys_on_div_cfg_hard_wfi功能分频比在mcu wfi=1的时候生效。 */
        unsigned int  clksys_on_div_cfg_hard_wfi     : 3;  /* bit[4-6]  : clksys_on上电区150MHz时钟硬件分频比配置，在mcu wfi=1的时候生效，分频比为数值+1。 */
        unsigned int  reserved_0                     : 9;  /* bit[7-15] : reserved */
        unsigned int  clksys_on_div_cfg_msk          : 3;  /* bit[16-18]: clksys_on_div_cfg的msk位 */
        unsigned int  clksys_on_div_cfg_hard_en_msk  : 1;  /* bit[19]   : clksys_on_div_cfg_hard_en的msk位 */
        unsigned int  clksys_on_div_cfg_hard_wfi_msk : 3;  /* bit[20-22]: clksys_on_div_cfg_hard_wfi的msk位 */
        unsigned int  reserved_1                     : 9;  /* bit[23-31]: reserved  */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_START               (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_END                 (2)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_hard_en_START       (3)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_hard_en_END         (3)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_hard_wfi_START      (4)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_hard_wfi_END        (6)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_msk_START           (16)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_msk_END             (18)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_hard_en_msk_START   (19)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_hard_en_msk_END     (19)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_hard_wfi_msk_START  (20)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL0_clksys_on_div_cfg_hard_wfi_msk_END    (22)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_UNION
 结构说明  : SC_ALWAYSON_SYS_CTRL1 寄存器结构定义。地址偏移量:0x1484，初值:0x0000000A，宽度:32
 寄存器说明: ALWAYSON控制寄存器，功能包含TCXO占空比配置，DCXO分频控制。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pdm_width : 4;  /* bit[0-3]  : PDM待调制数据位宽寄存器，调试用
                                                     4'b0000-数据位宽为6；
                                                     4'b0001-数据位宽为7；
                                                     4'b0010-数据位宽为8；
                                                     4'b0011-数据位宽为9；
                                                     4'b0100-数据位宽为10；
                                                     4'b0101-数据位宽为11；
                                                     4'b0110-数据位宽为12；
                                                     4'b0111-数据位宽为13；
                                                     4'b1000-数据位宽为14；
                                                     4'b1001-数据位宽为15；
                                                     4'b1010-数据位宽为16；
                                                     4'b1011~4'b1111-数据位宽为6。 */
        unsigned int  pdm_mode  : 1;  /* bit[4]    : PDM生效方式：调试用
                                                     0： reserved；
                                                     1： 立即生效。 */
        unsigned int  pdm_en    : 1;  /* bit[5]    : PDM使能生效方式：调试用
                                                     0： disable；
                                                     1： enable。 */
        unsigned int  reserved  : 10; /* bit[6-15] : reserved */
        unsigned int  pdm_modu  : 16; /* bit[16-31]: PDM待调制数据输出配置寄存器
                                                     。 */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_pdm_width_START  (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_pdm_width_END    (3)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_pdm_mode_START   (4)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_pdm_mode_END     (4)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_pdm_en_START     (5)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_pdm_en_END       (5)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_pdm_modu_START   (16)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL1_pdm_modu_END     (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL2_UNION
 结构说明  : SC_ALWAYSON_SYS_CTRL2 寄存器结构定义。地址偏移量:0x1488，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON控制寄存器，功能包含PDM分频配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pdm_div1 : 16; /* bit[0-15] : PDM分频配置1，分频之后的时钟频率为：
                                                    div_clk=sys_clk/(pdm_div1+1)(pdm_div2+1)
                                                    sys_clk在系统Normal状态为150MHz；在SLOW状态为19.2MHz；在SLEEP状态为32KHz。 */
        unsigned int  pdm_div2 : 16; /* bit[16-31]: PDM分频配置2，分频之后的时钟频率为：
                                                    div_clk=sys_clk/(pdm_div1+1)(pdm_div2+1)
                                                    sys_clk在系统Normal状态为150MHz；在SLOW状态为19.2MHz；在SLEEP状态为32KHz。 */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL2_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL2_pdm_div1_START  (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL2_pdm_div1_END    (15)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL2_pdm_div2_START  (16)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL2_pdm_div2_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL3_UNION
 结构说明  : SC_ALWAYSON_SYS_CTRL3 寄存器结构定义。地址偏移量:0x148C，初值:0x0000FFFB，宽度:32
 寄存器说明: ALWAYSON控制寄存器，功能32K时钟选择和RTC分频配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rtl_div_cfg   : 15; /* bit[0-14] : rtc分频配置，rtc时钟由clk32k分频获得，此配置数值仅在clk32k_sel_nv=0的时候有效，默认值为32764=0x7FFB+1；clk32k_sel_nv=1时，选择固定分频32768。 */
        unsigned int  clk32k_sel_nv : 1;  /* bit[15]   : clk32k选择
                                                         0：选择pmu输入32KHz时钟；
                                                         1：选择晶体输入32KHz时钟。 */
        unsigned int  reserved      : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL3_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL3_rtl_div_cfg_START    (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL3_rtl_div_cfg_END      (14)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL3_clk32k_sel_nv_START  (15)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL3_clk32k_sel_nv_END    (15)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_UNION
 结构说明  : SC_ALWAYSON_SYS_CTRL10 寄存器结构定义。地址偏移量:0x14A8，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON控制寄存器，功能包含CLK MONITOR控制。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_monitor_cfg_cnt_periodcfg : 16; /* bit[0-15] : clk monitor时钟周期配置
                                                                         clk monitor使用19.2MHz参考时钟,对32.768KHz时钟进行周期统计,如果统计结果大于clk_monitor_cfg_cnt_periodcfg,则产生中断。 */
        unsigned int  reserved                      : 14; /* bit[16-29]: reserved */
        unsigned int  clk_monitor_cnt_update_en     : 1;  /* bit[30]   : clk monitor计数使能控制，当此bit有效时，clk monitor使用19.2MHz参考时钟,对32.768KHz时钟进行周期统计，并将结果存到寄存器clk_monitor_clk_cnt中。
                                                                         0： disable；
                                                                         1： enable。 */
        unsigned int  clk_monitor_cfg_cnt_en        : 1;  /* bit[31]   : clk monitor周期统计比较产生中断使能控制
                                                                         0： disable；
                                                                         1： enable。
                                                                         clk monitor使用19.2MHz参考时钟,对32.768KHz时钟进行周期统计,如果统计结果大于clk_monitor_cfg_cnt_periodcfg,则产生中断。 */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_clk_monitor_cfg_cnt_periodcfg_START  (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_clk_monitor_cfg_cnt_periodcfg_END    (15)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_clk_monitor_cnt_update_en_START      (30)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_clk_monitor_cnt_update_en_END        (30)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_clk_monitor_cfg_cnt_en_START         (31)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL10_clk_monitor_cfg_cnt_en_END           (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_UNION
 结构说明  : SC_ALWAYSON_SYS_CTRL11 寄存器结构定义。地址偏移量:0x14AC，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON控制寄存器，功能包含CLK MONITOR控制,IO RET。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_monitor_int_mask           : 1;  /* bit[0]    : clk monitor中断屏蔽
                                                                          0： int not mask；
                                                                          1： int mask。 */
        unsigned int  clk_monitor_tcxo_en            : 1;  /* bit[1]    : clk monitor tcxo enable
                                                                          0： disable；
                                                                          1： enable。
                                                                          注：此功能暂时不使用。 */
        unsigned int  clk_monitor_int_clr            : 1;  /* bit[2]    : clk monitor中断清除
                                                                          0： int not clear；
                                                                          1： int clear。
                                                                          注： 中断清除后，需要将此bit写1'b0，否则，下次中断软件无法收到。 */
        unsigned int  reserved                       : 13; /* bit[3-15] : reserved */
        unsigned int  io_ret_pret_0                  : 1;  /* bit[16]   : io retention for io domain pret0
                                                                          0： not retention；
                                                                          1： retention。 */
        unsigned int  io_ret_pret_1                  : 1;  /* bit[17]   : io retention for io domain pret1
                                                                          0： not retention；
                                                                          1： retention。 */
        unsigned int  io_ret_pret_2                  : 1;  /* bit[18]   : io retention for io domain pret2
                                                                          0： not retention；
                                                                          1： retention。 */
        unsigned int  io_ret_pret_3                  : 1;  /* bit[19]   : io retention for io domain pret3
                                                                          0： not retention；
                                                                          1： retention。 */
        unsigned int  io_ret_pret_4                  : 1;  /* bit[20]   : io retention for io domain pret4
                                                                          0： not retention；
                                                                          1： retention。 */
        unsigned int  io_ret_pret_sd                 : 1;  /* bit[21]   : io retention for io domain pret sd
                                                                          0： not retention；
                                                                          1： retention。 */
        unsigned int  io_ret_pret_usim0              : 1;  /* bit[22]   : io retention for io domain pret usim0
                                                                          0： not retention；
                                                                          1： retention。 */
        unsigned int  io_ret_pret_usim1              : 1;  /* bit[23]   : io retention for io domain pret usim1
                                                                          0： not retention；
                                                                          1： retention。 */
        unsigned int  alwayson_and_periph_sys_ctrl11 : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_clk_monitor_int_mask_START            (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_clk_monitor_int_mask_END              (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_clk_monitor_tcxo_en_START             (1)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_clk_monitor_tcxo_en_END               (1)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_clk_monitor_int_clr_START             (2)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_clk_monitor_int_clr_END               (2)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_0_START                   (16)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_0_END                     (16)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_1_START                   (17)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_1_END                     (17)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_2_START                   (18)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_2_END                     (18)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_3_START                   (19)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_3_END                     (19)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_4_START                   (20)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_4_END                     (20)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_sd_START                  (21)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_sd_END                    (21)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_usim0_START               (22)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_usim0_END                 (22)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_usim1_START               (23)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_io_ret_pret_usim1_END                 (23)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_alwayson_and_periph_sys_ctrl11_START  (24)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_CTRL11_alwayson_and_periph_sys_ctrl11_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_UNION
 结构说明  : SC_ALWAYSON_SYS_STAT0 寄存器结构定义。地址偏移量:0x14C0，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON状态寄存器，功能包含CLK MONITOR控制。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ipc_s_clk_state       : 3;  /* bit[0-2]  : ipc_s clk state
                                                                 000：START；
                                                                 001：WAIT；
                                                                 010：ACCESS；
                                                                 011：BUSY；
                                                                 100：IDLE；
                                                                 others：no meanings。 */
        unsigned int  ipc_ns_clk_state      : 3;  /* bit[3-5]  : ipc_ns clk state
                                                                 000：START；
                                                                 001：WAIT；
                                                                 010：ACCESS；
                                                                 011：BUSY；
                                                                 100：IDLE；
                                                                 others：no meanings。 */
        unsigned int  clk_monitor_mask_intr : 1;  /* bit[6]    : clk monitor mask int
                                                                 0： 无中断；
                                                                 1： 有中断。 */
        unsigned int  clk_monitor_raw_intr  : 1;  /* bit[7]    : clk monitor raw int
                                                                 0： 无中断；
                                                                 1： 有中断。 */
        unsigned int  ahb_rev_bit           : 1;  /* bit[8]    : cm3 hreadyin signal
                                                                 0： not reversed；
                                                                 1： reversed。 */
        unsigned int  reserved              : 7;  /* bit[9-15] : reserved */
        unsigned int  clk_monitor_clk_cnt   : 16; /* bit[16-31]: clk_monitor_clk_cnt。 */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_ipc_s_clk_state_START        (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_ipc_s_clk_state_END          (2)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_ipc_ns_clk_state_START       (3)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_ipc_ns_clk_state_END         (5)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_clk_monitor_mask_intr_START  (6)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_clk_monitor_mask_intr_END    (6)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_clk_monitor_raw_intr_START   (7)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_clk_monitor_raw_intr_END     (7)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_ahb_rev_bit_START            (8)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_ahb_rev_bit_END              (8)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_clk_monitor_clk_cnt_START    (16)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT0_clk_monitor_clk_cnt_END      (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_UNION
 结构说明  : SC_ALWAYSON_SYS_STAT1 寄存器结构定义。地址偏移量:0x14C4，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON状态寄存器，功能BBP DRX深度。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdrx_depth_flag_1card          : 1;  /* bit[0]   : wdrx_depth_flag_1card
                                                                         WBBP送给ARM的卡一唤醒指示，WDRX在唤醒前提前一段时间给ARM指示，时间提前量可配置, ARM使用该信号判断是否进行深睡眠，高有效。 */
        unsigned int  g1_gdrx_depth_flag             : 1;  /* bit[1]   : g1_gdrx_depth_flag
                                                                         GBBP1送给ARM的唤醒指示，GDRX在唤醒前提前一段时间给ARM指示，时间提前量可配置, ARM使用该信号判断是否进行深睡眠，高有效。 */
        unsigned int  g2_gdrx_depth_flag             : 1;  /* bit[2]   : g2_gdrx_depth_flag
                                                                         GBBP2送给ARM的唤醒指示，GDRX在唤醒前提前一段时间给ARM指示，时间提前量可配置, ARM使用该信号判断是否进行深睡眠，高有效。 */
        unsigned int  cdma_evdo_gdrx_depth_flag      : 1;  /* bit[3]   : CDMA EVDO送给ARM的唤醒指示，GDRX在唤醒前提前一段时间给ARM指示，时间提前量可配置, ARM使用该信号判断是否进行深睡眠，高有效。 */
        unsigned int  cdma_1x_gdrx_depth_flag        : 1;  /* bit[4]   : CDMA 1X送给ARM的唤醒指示，GDRX在唤醒前提前一段时间给ARM指示，时间提前量可配置, ARM使用该信号判断是否进行深睡眠，高有效。 */
        unsigned int  alwayson_and_periph_sys_state1 : 27; /* bit[5-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_wdrx_depth_flag_1card_START           (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_wdrx_depth_flag_1card_END             (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_g1_gdrx_depth_flag_START              (1)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_g1_gdrx_depth_flag_END                (1)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_g2_gdrx_depth_flag_START              (2)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_g2_gdrx_depth_flag_END                (2)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_cdma_evdo_gdrx_depth_flag_START       (3)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_cdma_evdo_gdrx_depth_flag_END         (3)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_cdma_1x_gdrx_depth_flag_START         (4)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_cdma_1x_gdrx_depth_flag_END           (4)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_alwayson_and_periph_sys_state1_START  (5)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT1_alwayson_and_periph_sys_state1_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT2_UNION
 结构说明  : SC_ALWAYSON_SYS_STAT2 寄存器结构定义。地址偏移量:0x14C8，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON状态寄存器，功能包含usb chgdet_sts和datcondet_sts状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0    : 6;  /* bit[0-5] : reserved */
        unsigned int  datcondet_sts : 1;  /* bit[6]   : datcondet_sts
                                                        USB数据管脚接触检测，当时能充电检测功能时有效。
                                                        0： 无管脚连接；
                                                        1： 表示数据管脚连接检测到。 */
        unsigned int  chgdet_sts    : 1;  /* bit[7]   : chgdet_sts
                                                        充电器检测，该信号为稳定20ms后的信号。仅在充电检测有效时有效
                                                        0：SDP；
                                                        1： 检测到充电器为DCP或CDP。 */
        unsigned int  reserved_1    : 24; /* bit[8-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT2_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT2_datcondet_sts_START  (6)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT2_datcondet_sts_END    (6)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT2_chgdet_sts_START     (7)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT2_chgdet_sts_END       (7)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_UNION
 结构说明  : SC_ALWAYSON_SYS_STAT3 寄存器结构定义。地址偏移量:0x14CC，初值:0x00000000，宽度:32
 寄存器说明: ALWAYSON状态寄存器，功能保留。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sysnoc_cfpnoc_timeout  : 1;  /* bit[0]   : sysnoc cfgnoc bus tmieout figure
                                                                 0：normal；
                                                                 1：timeout。 */
        unsigned int  sysnoc_ddrc_timeout    : 1;  /* bit[1]   : sysnoc ddrc bus tmieout figure
                                                                 0：normal；
                                                                 1：timeout。 */
        unsigned int  sysnoc_xg2ram0_timeout : 1;  /* bit[2]   : sysnoc xg2ram0 bus tmieout figure
                                                                 0：normal；
                                                                 1：timeout。 */
        unsigned int  aonoc_modem_s_timeout  : 1;  /* bit[3]   : aonoc modem_s bus tmieout figure
                                                                 0：normal；
                                                                 1：timeout。 */
        unsigned int  aonoc_mcu_s_timeout    : 1;  /* bit[4]   : aonoc mcu_s bus tmieout figure
                                                                 0：normal；
                                                                 1：timeout。 */
        unsigned int  reserved               : 27; /* bit[5-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_UNION;
#endif
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_sysnoc_cfpnoc_timeout_START   (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_sysnoc_cfpnoc_timeout_END     (0)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_sysnoc_ddrc_timeout_START     (1)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_sysnoc_ddrc_timeout_END       (1)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_sysnoc_xg2ram0_timeout_START  (2)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_sysnoc_xg2ram0_timeout_END    (2)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_aonoc_modem_s_timeout_START   (3)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_aonoc_modem_s_timeout_END     (3)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_aonoc_mcu_s_timeout_START     (4)
#define SOC_AO_SCTRL_SC_ALWAYSON_SYS_STAT3_aonoc_mcu_s_timeout_END       (4)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PWUP_TIME0_UNION
 结构说明  : SC_PWUP_TIME0 寄存器结构定义。地址偏移量:0x1880，初值:0x00FF0016，宽度:32
 寄存器说明: 掉电区电源稳定计数器0寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_up_time_0peri : 16; /* bit[0-15] : [0] ：外设区（低功耗控制信号由硬件控制，软件配置无效，电源由外部PMU直接提供，仅mtcmos timeout指示观测有效。分区电源稳定时间，采用32K时钟递减计数；默认0x16=22个周期。 */
        unsigned int  pw_up_time_1g3d  : 16; /* bit[16-31]: [1] ：G3D下电区。分区电源稳定时间，采用TCXO时钟递减计数。 */
    } reg;
} SOC_AO_SCTRL_SC_PWUP_TIME0_UNION;
#endif
#define SOC_AO_SCTRL_SC_PWUP_TIME0_pw_up_time_0peri_START  (0)
#define SOC_AO_SCTRL_SC_PWUP_TIME0_pw_up_time_0peri_END    (15)
#define SOC_AO_SCTRL_SC_PWUP_TIME0_pw_up_time_1g3d_START   (16)
#define SOC_AO_SCTRL_SC_PWUP_TIME0_pw_up_time_1g3d_END     (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PWUP_TIME1_UNION
 结构说明  : SC_PWUP_TIME1 寄存器结构定义。地址偏移量:0x1884，初值:0x00FF00FF，宽度:32
 寄存器说明: 掉电区电源稳定计数器1寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_up_time_2codecisp : 16; /* bit[0-15] : [2] ：CODEC+ISP下电区。 分区电源稳定时间，采用TCXO时钟递减计数。 */
        unsigned int  pw_up_time_3reserved : 16; /* bit[16-31]: [3] ：reserved分区电源稳定时间，采用TCXO时钟递减计数。 */
    } reg;
} SOC_AO_SCTRL_SC_PWUP_TIME1_UNION;
#endif
#define SOC_AO_SCTRL_SC_PWUP_TIME1_pw_up_time_2codecisp_START  (0)
#define SOC_AO_SCTRL_SC_PWUP_TIME1_pw_up_time_2codecisp_END    (15)
#define SOC_AO_SCTRL_SC_PWUP_TIME1_pw_up_time_3reserved_START  (16)
#define SOC_AO_SCTRL_SC_PWUP_TIME1_pw_up_time_3reserved_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PWUP_TIME2_UNION
 结构说明  : SC_PWUP_TIME2 寄存器结构定义。地址偏移量:0x1888，初值:0x00FF00FF，宽度:32
 寄存器说明: 掉电区电源稳定计数器2寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_up_time_4mcpu       : 16; /* bit[0-15] : [4] ：MCPU下电区。 分区电源稳定时间，采用TCXO时钟递减计数。 */
        unsigned int  pw_up_time_5bbpharqmem : 16; /* bit[16-31]: [5] ：BBP Harq Memory所在下电分区。 分区电源稳定时间，采用TCXO时钟递减计数。 */
    } reg;
} SOC_AO_SCTRL_SC_PWUP_TIME2_UNION;
#endif
#define SOC_AO_SCTRL_SC_PWUP_TIME2_pw_up_time_4mcpu_START        (0)
#define SOC_AO_SCTRL_SC_PWUP_TIME2_pw_up_time_4mcpu_END          (15)
#define SOC_AO_SCTRL_SC_PWUP_TIME2_pw_up_time_5bbpharqmem_START  (16)
#define SOC_AO_SCTRL_SC_PWUP_TIME2_pw_up_time_5bbpharqmem_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PWUP_TIME3_UNION
 结构说明  : SC_PWUP_TIME3 寄存器结构定义。地址偏移量:0x188C，初值:0x00FF00FF，宽度:32
 寄存器说明: 掉电区电源稳定计数器3寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_up_time_6reserved : 16; /* bit[0-15] : [6] ：reserved 分区电源稳定时间，采用TCXO时钟递减计数。 */
        unsigned int  pw_up_time_7reserved : 16; /* bit[16-31]: [7] ：reserved分区电源稳定时间，采用TCXO时钟递减计数。 */
    } reg;
} SOC_AO_SCTRL_SC_PWUP_TIME3_UNION;
#endif
#define SOC_AO_SCTRL_SC_PWUP_TIME3_pw_up_time_6reserved_START  (0)
#define SOC_AO_SCTRL_SC_PWUP_TIME3_pw_up_time_6reserved_END    (15)
#define SOC_AO_SCTRL_SC_PWUP_TIME3_pw_up_time_7reserved_START  (16)
#define SOC_AO_SCTRL_SC_PWUP_TIME3_pw_up_time_7reserved_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PWUP_TIME4_UNION
 结构说明  : SC_PWUP_TIME4 寄存器结构定义。地址偏移量:0x1890，初值:0x00FF00FF，宽度:32
 寄存器说明: 掉电区电源稳定计数器4寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_up_time_8hifi     : 16; /* bit[0-15] : [8] ：HIFI下电区 。分区电源稳定时间，采用TCXO时钟递减计数。 */
        unsigned int  pw_up_time_9reserved : 16; /* bit[16-31]: [9] ：reserved分区电源稳定时间，采用TCXO时钟递减计数。 */
    } reg;
} SOC_AO_SCTRL_SC_PWUP_TIME4_UNION;
#endif
#define SOC_AO_SCTRL_SC_PWUP_TIME4_pw_up_time_8hifi_START      (0)
#define SOC_AO_SCTRL_SC_PWUP_TIME4_pw_up_time_8hifi_END        (15)
#define SOC_AO_SCTRL_SC_PWUP_TIME4_pw_up_time_9reserved_START  (16)
#define SOC_AO_SCTRL_SC_PWUP_TIME4_pw_up_time_9reserved_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PWUP_TIME5_UNION
 结构说明  : SC_PWUP_TIME5 寄存器结构定义。地址偏移量:0x1894，初值:0x00FF00FF，宽度:32
 寄存器说明: 掉电区电源稳定计数器5寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_up_time_10reserved : 16; /* bit[0-15] : [10] ：reserved分区电源稳定时间，采用TCXO时钟递减计数。 */
        unsigned int  pw_up_time_11reserved : 16; /* bit[16-31]: [11] ：reserved分区电源稳定时间，采用TCXO时钟递减计数。 */
    } reg;
} SOC_AO_SCTRL_SC_PWUP_TIME5_UNION;
#endif
#define SOC_AO_SCTRL_SC_PWUP_TIME5_pw_up_time_10reserved_START  (0)
#define SOC_AO_SCTRL_SC_PWUP_TIME5_pw_up_time_10reserved_END    (15)
#define SOC_AO_SCTRL_SC_PWUP_TIME5_pw_up_time_11reserved_START  (16)
#define SOC_AO_SCTRL_SC_PWUP_TIME5_pw_up_time_11reserved_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PWUP_TIME6_UNION
 结构说明  : SC_PWUP_TIME6 寄存器结构定义。地址偏移量:0x1898，初值:0x00FF00FF，宽度:32
 寄存器说明: 掉电区电源稳定计数器6寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_up_time_12acpucluster : 16; /* bit[0-15] : [12] ：ACPU SCU＋L2C逻辑下电区（无MTCMOS，分区电源通过配置PMU控制实现开关，寄存器配置pw_en0_12acpucluter用于配置电源稳定时间，电源稳定时间可查询）；ACPU SCU＋L2C分区ISO同时控制ACPU L2掉电MEM输入信号。分区电源稳定时间，采用TCXO时钟递减计数。 */
        unsigned int  pw_up_time_13reserved    : 16; /* bit[16-31]: [13] ：reserved分区电源稳定时间，采用TCXO时钟递减计数。 */
    } reg;
} SOC_AO_SCTRL_SC_PWUP_TIME6_UNION;
#endif
#define SOC_AO_SCTRL_SC_PWUP_TIME6_pw_up_time_12acpucluster_START  (0)
#define SOC_AO_SCTRL_SC_PWUP_TIME6_pw_up_time_12acpucluster_END    (15)
#define SOC_AO_SCTRL_SC_PWUP_TIME6_pw_up_time_13reserved_START     (16)
#define SOC_AO_SCTRL_SC_PWUP_TIME6_pw_up_time_13reserved_END       (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_PWUP_TIME7_UNION
 结构说明  : SC_PWUP_TIME7 寄存器结构定义。地址偏移量:0x189C，初值:0x00FF00FF，宽度:32
 寄存器说明: 掉电区电源稳定计数器7寄存器。用于指示各掉电区电源调整后电压稳定所需的时间。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pw_up_time_14reserved : 16; /* bit[0-15] : [14] ：reserved分区电源稳定时间，采用TCXO时钟递减计数。 */
        unsigned int  pw_up_time_15reserved : 16; /* bit[16-31]: [15] ：reserved分区电源稳定时间，采用TCXO时钟递减计数。 */
    } reg;
} SOC_AO_SCTRL_SC_PWUP_TIME7_UNION;
#endif
#define SOC_AO_SCTRL_SC_PWUP_TIME7_pw_up_time_14reserved_START  (0)
#define SOC_AO_SCTRL_SC_PWUP_TIME7_pw_up_time_14reserved_END    (15)
#define SOC_AO_SCTRL_SC_PWUP_TIME7_pw_up_time_15reserved_START  (16)
#define SOC_AO_SCTRL_SC_PWUP_TIME7_pw_up_time_15reserved_END    (31)


/*****************************************************************************
 结构名    : SOC_AO_SCTRL_SC_SECURITY_CTRL1_UNION
 结构说明  : SC_SECURITY_CTRL1 寄存器结构定义。地址偏移量:0x1C04，初值:0x00000000，宽度:32
 寄存器说明: 安全控制寄存器，控制AO_SC的寄存器BIT安全访问。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_secure_ctrl_bit_0rtc          : 1;  /* bit[0]    : [0] ： RTC时钟门控和软复位 涉及寄存器0x630,634,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_1timer0       : 1;  /* bit[1]    : [1] ： ACPU侧DualTimer0 时钟门控、软复位和参考时钟选择,涉及寄存器0x630,634,1d0,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_2timer1       : 1;  /* bit[2]    : [2] ： ACPU侧DualTimer1 时钟门控、软复位和参考时钟选择,涉及寄存器0x630,634,1d0,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_3timer2       : 1;  /* bit[3]    : [3] ： ACPU侧DualTimer2 时钟门控、软复位和参考时钟选择,涉及寄存器0x630,634,1d0,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_4timer3       : 1;  /* bit[4]    : [4] ： ACPU侧DualTimer3 时钟门控、软复位和参考时钟选择,涉及寄存器0x630,634,1d0,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_5timer4       : 1;  /* bit[5]    : [5] ： ACPU侧DualTimer4 时钟门控、软复位和参考时钟选择,涉及寄存器0x630,634,1d4,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_6timer5       : 1;  /* bit[6]    : [6] ： ACPU侧DualTimer5 时钟门控、软复位和参考时钟选择,涉及寄存器0x630,634,1d4,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_7timer6       : 1;  /* bit[7]    : [7] ： ACPU侧DualTimer6 时钟门控、软复位和参考时钟选择,涉及寄存器0x630,634,1d4,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_8timer7       : 1;  /* bit[8]    : [8] ： ACPU侧DualTimer7 时钟门控、软复位和参考时钟选择,涉及寄存器0x630,634,1d4,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_9timer8       : 1;  /* bit[9]    : [9] ： ACPU侧DualTimer8 时钟门控、软复位和参考时钟选择,涉及寄存器0x630,634,1f4,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_10watchdog0   : 1;  /* bit[10]   : [10] ： ACPU侧 Watchdog0 时钟门控和软复位,涉及寄存器0x630,634,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_11watchdog1   : 1;  /* bit[11]   : [11] ： ACPU侧 Watchdog1 时钟门控和软复位,涉及寄存器0x630,634,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_12watchdog2   : 1;  /* bit[12]   : [12] ： ACPU侧 Watchdog2 时钟门控和软复位,涉及寄存器0x630,634,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_13rtc1        : 1;  /* bit[13]   : [13] ： RTC1时钟门控和软复位 涉及寄存器0x630,634,0x6F0,6F4。
                                                                            0：寄存器安全/非安全操作都可改写；
                                                                            1：寄存器仅安全操作才可改写。 */
        unsigned int  sc_secure_ctrl_bit_14tzpc        : 1;  /* bit[14]   : [14] ： TZPC时钟门控安全控制。
                                                                            0：TZPC门控软件通过SC_PERIPH_CLKEN/IDS4配置控制；
                                                                            1：TZPC门控软件通过sc_secure_ctrl_bit_16tzpc_clken配置控制。 */
        unsigned int  sc_secure_ctrl_bit_15ipc_s       : 1;  /* bit[15]   : [15] ： IPC_SC时钟门控和软复位安全控制。
                                                                            0：IPC_SC时钟门控和软复位软件通过SC_PERIPH_CLKEN/IDS4，SC_PERIPH_CLKEN/IDS5，SC_PERIPH_RSTEN/IDS4配置控制；
                                                                            1：IPC_SC时钟门控通过sc_secure_ctrl_bit_17ipc_s_clken配置控制，软复位软件不可配置。 */
        unsigned int  sc_secure_ctrl_bit_16tzpc_clken  : 1;  /* bit[16]   : [16] ： TZPC时钟门控安全控制，此bit配置仅在sc_secure_ctrl_bit_14tzpc为1'b1时有效。
                                                                            0：TZPC时钟门控关闭；
                                                                            1：TZPC时钟门控打开。 */
        unsigned int  sc_secure_ctrl_bit_17ipc_s_clken : 1;  /* bit[17]   : [17] ： IPC_SC时钟门控安全控制，此bit配置仅在sc_secure_ctrl_bit_15ipc_s为1'b1时有效。
                                                                            0：IPC_SC时钟门控关闭；
                                                                            1：IPC_SC时钟门控打开； */
        unsigned int  sc_secure_ctrl_bit               : 14; /* bit[18-31]: reserved */
    } reg;
} SOC_AO_SCTRL_SC_SECURITY_CTRL1_UNION;
#endif
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_0rtc_START           (0)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_0rtc_END             (0)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_1timer0_START        (1)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_1timer0_END          (1)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_2timer1_START        (2)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_2timer1_END          (2)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_3timer2_START        (3)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_3timer2_END          (3)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_4timer3_START        (4)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_4timer3_END          (4)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_5timer4_START        (5)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_5timer4_END          (5)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_6timer5_START        (6)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_6timer5_END          (6)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_7timer6_START        (7)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_7timer6_END          (7)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_8timer7_START        (8)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_8timer7_END          (8)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_9timer8_START        (9)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_9timer8_END          (9)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_10watchdog0_START    (10)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_10watchdog0_END      (10)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_11watchdog1_START    (11)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_11watchdog1_END      (11)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_12watchdog2_START    (12)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_12watchdog2_END      (12)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_13rtc1_START         (13)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_13rtc1_END           (13)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_14tzpc_START         (14)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_14tzpc_END           (14)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_15ipc_s_START        (15)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_15ipc_s_END          (15)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_16tzpc_clken_START   (16)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_16tzpc_clken_END     (16)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_17ipc_s_clken_START  (17)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_17ipc_s_clken_END    (17)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_START                (18)
#define SOC_AO_SCTRL_SC_SECURITY_CTRL1_sc_secure_ctrl_bit_END                  (31)






/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of soc_ao_sctrl_interface.h */
