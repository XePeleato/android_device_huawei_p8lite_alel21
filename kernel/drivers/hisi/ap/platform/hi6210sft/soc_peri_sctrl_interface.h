

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_PERI_SCTRL_INTERFACE_H__
#define __SOC_PERI_SCTRL_INTERFACE_H__

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
/* 寄存器说明：外设控制寄存器1。功能包含etr下电前排空，及hifi中断屏蔽位
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL1_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_ADDR(base)     ((base) + (0x000))

/* 寄存器说明：外设控制寄存器2。MMC CLK PHASE BYPASS控制，func_test选择，cssys时间戳使能，ddrt,g3d mux选择，testpin 选择，hifi memory控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL2_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_ADDR(base)     ((base) + (0x004))

/* 寄存器说明：外设控制寄存器3主要包括memory share相关控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL3_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_ADDR(base)     ((base) + (0x008))

/* 寄存器说明：外设系统控制寄存器，功能包含USBOGT、PICOPHY控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL4_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_ADDR(base)     ((base) + (0x00c))

/* 寄存器说明：外设系统控制寄存器5，功能包含PICOPHY控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL5_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_ADDR(base)     ((base) + (0x010))

/* 寄存器说明：外设控制寄存器6。功能包含CSSYS OFF的memroy控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL6_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_ADDR(base)     ((base) + (0x014))

/* 寄存器说明：外设控制寄存器8。功能包含PICOPHY控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL8_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_ADDR(base)     ((base) + (0x018))

/* 寄存器说明：外设控制寄存器9。功能包含PICOPHY控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL9_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL9_ADDR(base)     ((base) + (0x01C))

/* 寄存器说明：外设控制寄存器10。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL10_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL10_ADDR(base)    ((base) + (0x020))

/* 寄存器说明：外设控制寄存器12。功能包含PERISYS的memroy控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL12_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_ADDR(base)    ((base) + (0x024))

/* 寄存器说明：外设控制寄存器13。包括acpu，ccpu，hifi时钟切换使能，mmc环回时钟门控使能
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL13_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_ADDR(base)    ((base) + (0x028))

/* 寄存器说明：外设控制寄存器14。时钟频率检测
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CTRL14_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL14_ADDR(base)    ((base) + (0x02C))

/* 寄存器说明：aarm子系统控制寄存器，功能包含sram_ap_off/ipf/seceng_cc/seceng_c2/seceng_rc4/socp和rom的memory控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_MEM_CTRL0_UNION */
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_ADDR(base)        ((base) + (0x030))

/* 寄存器说明：时钟自动门控使能寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_UNION */
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_ADDR(base)   ((base) + (0x034))

/* 寄存器说明：时钟自动门控使能寄存器。包括NOC100M总线时钟自动降频功能是否使能的投票
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_AUTO_CLK_GATE1_UNION */
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE1_ADDR(base)   ((base) + (0x038))

/* 寄存器说明：时钟自动门控使能寄存器。包括NOC150M总线时钟自动降频功能是否使能的投票
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_AUTO_CLK_GATE2_UNION */
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE2_ADDR(base)   ((base) + (0x03c))

/* 寄存器说明：DDR配置寄存器0
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_DDR_CTRL0_UNION */
#define SOC_PERI_SCTRL_SC_DDR_CTRL0_ADDR(base)        ((base) + (0x050))

/* 寄存器说明：DDR 配置寄存器1
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_DDR_CTRL1_UNION */
#define SOC_PERI_SCTRL_SC_DDR_CTRL1_ADDR(base)        ((base) + (0x054))

/* 寄存器说明：DDR 配置寄存器2。功能包含DDR qos控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_DDR_CTRL2_UNION */
#define SOC_PERI_SCTRL_SC_DDR_CTRL2_ADDR(base)        ((base) + (0x058))

/* 寄存器说明：DDR 配置寄存器。功能包含DDR qos控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_DDR_CTRL3_UNION */
#define SOC_PERI_SCTRL_SC_DDR_CTRL3_ADDR(base)        ((base) + (0x060))

/* 寄存器说明：DDR 状态寄存器4。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_DDR_CTRL4_UNION */
#define SOC_PERI_SCTRL_SC_DDR_CTRL4_ADDR(base)        ((base) + (0x064))

/* 寄存器说明：NOC 状态寄存器0，包括sysnoc timeout 使能
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_NOC_CTRL0_UNION */
#define SOC_PERI_SCTRL_SC_NOC_CTRL0_ADDR(base)        ((base) + (0x070))

/* 寄存器说明：NOC 状态寄存器1，包括clkbus_low时钟选择使能和滤毛刺寄存器配置
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_NOC_CTRL1_UNION */
#define SOC_PERI_SCTRL_SC_NOC_CTRL1_ADDR(base)        ((base) + (0x074))

/* 寄存器说明：NOC 状态寄存器2，包括clkbus_high时钟选择使能和滤毛刺寄存器配置
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_NOC_CTRL2_UNION */
#define SOC_PERI_SCTRL_SC_NOC_CTRL2_ADDR(base)        ((base) + (0x078))

/* 寄存器说明：外设状态寄存器0。功能包含CSSYS ETR、SOCP、IPF、hifi状态、noc nopending信号等
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_STAT0_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_ADDR(base)     ((base) + (0x090))

/* 寄存器说明：periph stat1
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_STAT1_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_STAT1_ADDR(base)     ((base) + (0x094))

/* 寄存器说明：usb状态寄存器2。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_STAT2_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_ADDR(base)     ((base) + (0x098))

/* 寄存器说明：hifi组合中断状态寄存器
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_STAT3_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_STAT3_ADDR(base)     ((base) + (0x09C))

/* 寄存器说明：外设状态寄存器4。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_STAT4_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_ADDR(base)     ((base) + (0x0A0))

/* 寄存器说明：外设状态寄存器6。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_STAT6_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_STAT6_ADDR(base)     ((base) + (0x0A4))

/* 寄存器说明：
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_UNION */
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_ADDR(base)  ((base) + (0x0B0))

/* 寄存器说明：
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_UNION */
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_ADDR(base)  ((base) + (0x0B4))

/* 寄存器说明：ACPU唤醒中断二级中断使能信号
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_UNION */
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_ADDR(base) ((base) + (0x100))

/* 寄存器说明：ACPU唤醒中断二级中断原始中断状态
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_UNION */
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_ADDR(base) ((base) + (0x104))

/* 寄存器说明：ACPU唤醒中断二级中断屏蔽后中断状态
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_UNION */
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_ADDR(base) ((base) + (0x108))

/* 寄存器说明：MCU中断二级中断使能信号
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_UNION */
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_ADDR(base) ((base) + (0x110))

/* 寄存器说明：MCU中断二级中断原始中断状态
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_UNION */
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_ADDR(base) ((base) + (0x114))

/* 寄存器说明：MCU中断二级中断屏蔽后中断状态
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_UNION */
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_ADDR(base) ((base) + (0x118))

/* 寄存器说明：外设时钟使能寄存器0。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_ADDR(base)    ((base) + (0x200))

/* 寄存器说明：外设时钟禁止寄存器0。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_ADDR(base)   ((base) + (0x204))

/* 寄存器说明：外设时钟状态寄存器0。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_ADDR(base)  ((base) + (0x208))

/* 寄存器说明：外设时钟使能寄存器1。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKEN1_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN1_ADDR(base)    ((base) + (0x210))

/* 寄存器说明：外设时钟禁止寄存器1。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKDIS1_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS1_ADDR(base)   ((base) + (0x214))

/* 寄存器说明：外设时钟状态寄存器1。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT1_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT1_ADDR(base)  ((base) + (0x218))

/* 寄存器说明：外设时钟使能寄存器2。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_ADDR(base)    ((base) + (0x220))

/* 寄存器说明：外设时钟禁止寄存器2。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_ADDR(base)   ((base) + (0x224))

/* 寄存器说明：外设时钟状态寄存器2。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_ADDR(base)  ((base) + (0x228))

/* 寄存器说明：外设时钟使能寄存器3。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_ADDR(base)    ((base) + (0x230))

/* 寄存器说明：外设时钟禁止寄存器3。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_ADDR(base)   ((base) + (0x234))

/* 寄存器说明：外设时钟状态寄存器3。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_ADDR(base)  ((base) + (0x238))

/* 寄存器说明：外设时钟使能寄存器8。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_ADDR(base)    ((base) + (0x240))

/* 寄存器说明：外设时钟禁止寄存器8。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_ADDR(base)   ((base) + (0x244))

/* 寄存器说明：外设时钟状态寄存器8。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_ADDR(base)  ((base) + (0x248))

/* 寄存器说明：外设时钟使能寄存器9。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_ADDR(base)    ((base) + (0x250))

/* 寄存器说明：外设时钟禁止寄存器9。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_ADDR(base)   ((base) + (0x254))

/* 寄存器说明：外设时钟状态寄存器9。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_ADDR(base)  ((base) + (0x258))

/* 寄存器说明：外设时钟使能寄存器10。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_ADDR(base)   ((base) + (0x260))

/* 寄存器说明：外设时钟禁止寄存器10。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_ADDR(base)  ((base) + (0x264))

/* 寄存器说明：外设时钟状态寄存器10。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_ADDR(base) ((base) + (0x268))

/* 寄存器说明：外设时钟使能寄存器12。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_ADDR(base)   ((base) + (0x270))

/* 寄存器说明：外设时钟禁止寄存器12。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_ADDR(base)  ((base) + (0x274))

/* 寄存器说明：外设时钟状态寄存器12。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_ADDR(base) ((base) + (0x278))

/* 寄存器说明：外设时钟使能寄存器14。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKEN14_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN14_ADDR(base)   ((base) + (0x280))

/* 寄存器说明：外设时钟禁止寄存器14。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKDIS14_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS14_ADDR(base)  ((base) + (0x284))

/* 寄存器说明：外设时钟状态寄存器14。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT14_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT14_ADDR(base) ((base) + (0x288))

/* 寄存器说明：设复位使能寄存器0。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_ADDR(base)    ((base) + (0x300))

/* 寄存器说明：外设复位禁止寄存器0。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_ADDR(base)   ((base) + (0x304))

/* 寄存器说明：外设复位状态寄存器0。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。1为复位，0为非复位状态。默认CARM子系统处于解复位状态，AARM、ZSP、HIFI、BBP子系统处于复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_ADDR(base)  ((base) + (0x308))

/* 寄存器说明：设复位使能寄存器1。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTEN1_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN1_ADDR(base)    ((base) + (0x310))

/* 寄存器说明：外设复位禁止寄存器1。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTDIS1_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS1_ADDR(base)   ((base) + (0x314))

/* 寄存器说明：外设复位状态寄存器1。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。1为复位，0为非复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT1_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT1_ADDR(base)  ((base) + (0x318))

/* 寄存器说明：设复位使能寄存器2。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_ADDR(base)    ((base) + (0x320))

/* 寄存器说明：外设复位禁止寄存器2。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_ADDR(base)   ((base) + (0x324))

/* 寄存器说明：外设复位状态寄存器2。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。1为复位，0为非复位状态。默认CARM子系统处于解复位状态，AARM、ZSP、HIFI、BBP子系统处于复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_ADDR(base)  ((base) + (0x328))

/* 寄存器说明：设复位使能寄存器3。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_ADDR(base)    ((base) + (0x330))

/* 寄存器说明：外设复位禁止寄存器3。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_ADDR(base)   ((base) + (0x334))

/* 寄存器说明：外设复位状态寄存器3。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。1为复位，0为非复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_ADDR(base)  ((base) + (0x338))

/* 寄存器说明：设复位使能寄存器6。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_ADDR(base)    ((base) + (0x340))

/* 寄存器说明：外设复位禁止寄存器6。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_ADDR(base)   ((base) + (0x344))

/* 寄存器说明：外设复位状态寄存器6。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_ADDR(base)  ((base) + (0x348))

/* 寄存器说明：设复位使能寄存器6。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_ADDR(base)    ((base) + (0x350))

/* 寄存器说明：外设复位禁止寄存器6。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_ADDR(base)   ((base) + (0x354))

/* 寄存器说明：外设复位状态寄存器6。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_ADDR(base)  ((base) + (0x358))

/* 寄存器说明：设复位使能寄存器6。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTEN10_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN10_ADDR(base)   ((base) + (0x360))

/* 寄存器说明：外设复位禁止寄存器6。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTDIS10_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS10_ADDR(base)  ((base) + (0x364))

/* 寄存器说明：外设复位状态寄存器6。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT10_UNION */
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT10_ADDR(base) ((base) + (0x368))

/* 寄存器说明：时钟选择寄存器0
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLK_SEL0_UNION */
#define SOC_PERI_SCTRL_SC_CLK_SEL0_ADDR(base)         ((base) + (0x400))

/* 寄存器说明：时钟选择寄存器1
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLK_SEL1_UNION */
#define SOC_PERI_SCTRL_SC_CLK_SEL1_ADDR(base)         ((base) + (0x404))

/* 寄存器说明：时钟参数配置寄存器0
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFGALL0_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFGALL0_ADDR(base)       ((base) + (0x450))

/* 寄存器说明：时钟参数配置寄存器1
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFGALL1_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFGALL1_ADDR(base)       ((base) + (0x454))

/* 寄存器说明：时钟参数配置寄存器2
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFGALL2_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFGALL2_ADDR(base)       ((base) + (0x458))

/* 寄存器说明：时钟参数配置寄存器3
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFGALL3_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFGALL3_ADDR(base)       ((base) + (0x45C))

/* 寄存器说明：时钟参数配置寄存器4
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFGALL4_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFGALL4_ADDR(base)       ((base) + (0x460))

/* 寄存器说明：时钟参数配置寄存器5
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFGALL5_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFGALL5_ADDR(base)       ((base) + (0x464))

/* 寄存器说明：时钟参数配置寄存器7
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT0_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_ADDR(base)      ((base) + (0x490))

/* 寄存器说明：mmc0时钟参数配置寄存器
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT1_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT1_ADDR(base)      ((base) + (0x494))

/* 寄存器说明：mmc1时钟参数配置寄存器
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT2_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT2_ADDR(base)      ((base) + (0x498))

/* 寄存器说明：mmc2时钟参数配置寄存器
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT3_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT3_ADDR(base)      ((base) + (0x49C))

/* 寄存器说明：时钟参数配置寄存器4
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT4_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_ADDR(base)      ((base) + (0x4A0))

/* 寄存器说明：时钟参数配置寄存器5
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT5_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_ADDR(base)      ((base) + (0x4A4))

/* 寄存器说明：时钟参数配置寄存器6
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT6_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_ADDR(base)      ((base) + (0x4A8))

/* 寄存器说明：时钟参数配置寄存器7
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT7_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_ADDR(base)      ((base) + (0x4AC))

/* 寄存器说明：时钟参数配置寄存器8
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT8_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_ADDR(base)      ((base) + (0x4B0))

/* 寄存器说明：时钟参数配置寄存器16
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT9_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_ADDR(base)      ((base) + (0x4B4))

/* 寄存器说明：时钟参数配置寄存器17
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CLKCFG8BIT10_UNION */
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_ADDR(base)     ((base) + (0x4B8))

/* 寄存器说明：CARM控制寄存器0。CARM DEBUG等控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CARM_CTRL0_UNION */
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_ADDR(base)       ((base) + (0x500))

/* 寄存器说明：CARM控制寄存器1。用于控制CARM filter模式使能及起始和结束地址。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CARM_CTRL1_UNION */
#define SOC_PERI_SCTRL_SC_CARM_CTRL1_ADDR(base)       ((base) + (0x504))

/* 寄存器说明：CARM状态寄存器0。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_CARM_STAT0_UNION */
#define SOC_PERI_SCTRL_SC_CARM_STAT0_ADDR(base)       ((base) + (0x508))

/* 寄存器说明：HIFI控制寄存器。用于配置HIFI控制信号，同时也用于寄存HIFI的状态信号。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_UNION */
#define SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_ADDR(base) ((base) + (0x600))

/* 寄存器说明：HIFI NMI中断控制
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_HIFI_CTRL1_UNION */
#define SOC_PERI_SCTRL_SC_HIFI_CTRL1_ADDR(base)       ((base) + (0x604))

/* 寄存器说明：hifi instruction space remap
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_HIFI_CTRL0_UNION */
#define SOC_PERI_SCTRL_SC_HIFI_CTRL0_ADDR(base)       ((base) + (0x608))

/* 寄存器说明：hifi boot addr安全寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_HIFI_CTRL2_UNION */
#define SOC_PERI_SCTRL_SC_HIFI_CTRL2_ADDR(base)       ((base) + (0x60C))

/* 寄存器说明：hifi允许访问内存地址范围控制寄存器
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_HIFI_CTRL3_UNION */
#define SOC_PERI_SCTRL_SC_HIFI_CTRL3_ADDR(base)       ((base) + (0x0610))

/* 寄存器说明：hifi允许访问内存地址范围控制寄存器
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_HIFI_CTRL4_UNION */
#define SOC_PERI_SCTRL_SC_HIFI_CTRL4_ADDR(base)       ((base) + (0x0614))

/* 寄存器说明：Tsensor0温度比较滞回范围寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_LAG_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_LAG_ADDR(base)        ((base) + (0x700))

/* 寄存器说明：Tsensor0温度阈值寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_TH_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_TH_ADDR(base)         ((base) + (0x704))

/* 寄存器说明：Tsensor0复位阈值寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_RST_TH_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_RST_TH_ADDR(base)     ((base) + (0x708))

/* 寄存器说明：Tsensor0参数配置寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_CFG_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_CFG_ADDR(base)        ((base) + (0x70C))

/* 寄存器说明：Tsensor0温度检测模块使能寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_EN_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_EN_ADDR(base)         ((base) + (0x710))

/* 寄存器说明：Tsensor0温度检测中断屏蔽寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_INT_EN_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_INT_EN_ADDR(base)     ((base) + (0x714))

/* 寄存器说明：Tsensor0温度检测中断清除寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_INT_CLR_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_INT_CLR_ADDR(base)    ((base) + (0x718))

/* 寄存器说明：Tsensor0温度检测复位屏蔽寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_RST_MSK_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_RST_MSK_ADDR(base)    ((base) + (0x71C))

/* 寄存器说明：Tsensor0温度检测Raw中断寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_RAW_INT_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_RAW_INT_ADDR(base)    ((base) + (0x720))

/* 寄存器说明：Tsensor0温度检测Mask中断寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_MSK_INT_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_MSK_INT_ADDR(base)    ((base) + (0x724))

/* 寄存器说明：Tsensor0温度检测值寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_TEMP0_VALUE_UNION */
#define SOC_PERI_SCTRL_SC_TEMP0_VALUE_ADDR(base)      ((base) + (0x728))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED7_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED7_ADDR(base)        ((base) + (0xD00))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED8_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED8_ADDR(base)        ((base) + (0xD04))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED11_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED11_ADDR(base)       ((base) + (0xD08))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED12_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED12_ADDR(base)       ((base) + (0xD0C))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED13_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED13_ADDR(base)       ((base) + (0xD10))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED14_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED14_ADDR(base)       ((base) + (0xD14))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED15_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED15_ADDR(base)       ((base) + (0xD18))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED16_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED16_ADDR(base)       ((base) + (0xD1C))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED17_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED17_ADDR(base)       ((base) + (0xD20))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED18_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED18_ADDR(base)       ((base) + (0xD24))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED21_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED21_ADDR(base)       ((base) + (0xD28))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED22_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED22_ADDR(base)       ((base) + (0xD2C))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED23_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED23_ADDR(base)       ((base) + (0xD30))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED24_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED24_ADDR(base)       ((base) + (0xD34))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED25_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED25_ADDR(base)       ((base) + (0xD38))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED26_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED26_ADDR(base)       ((base) + (0xD3C))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED27_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED27_ADDR(base)       ((base) + (0xD40))

/* 寄存器说明：保留寄存器。
   位域定义UNION结构:  SOC_PERI_SCTRL_SC_RESERVED28_UNION */
#define SOC_PERI_SCTRL_SC_RESERVED28_ADDR(base)       ((base) + (0xD44))





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
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL1_UNION
 结构说明  : SC_PERIPH_CTRL1 寄存器结构定义。地址偏移量:0x000，初值:0x00000005，宽度:32
 寄存器说明: 外设控制寄存器1。功能包含etr下电前排空，及hifi中断屏蔽位
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  etr_axi_csysreq_n     : 1;  /* bit[0]    : ETR的axi接口的csysreq/csysack在下电前排空操作
                                                                 1'b0：请求排空操作
                                                                 1'b1：不请求排空操作 */
        unsigned int  hifi_int_mask         : 1;  /* bit[1]    : 送给hifi中断屏蔽位，只对[25:1]进行屏蔽；[0]为nmi，[25]为bbe16_gpio5中断，不开放给软件使用 */
        unsigned int  hifi_all_int_mask     : 1;  /* bit[2]    : 送给mcu的hifi组合中断屏蔽位 */
        unsigned int  reserved_0            : 13; /* bit[3-15] : 其他：保留。 */
        unsigned int  etr_axi_csysreq_n_msk : 1;  /* bit[16]   : ETR的axi接口的csysreq/csysack在下电前排空操作 */
        unsigned int  hifi_int_mask_msk     : 1;  /* bit[17]   : 送给hifi中断屏蔽位 */
        unsigned int  hifi_all_int_mask_msk : 1;  /* bit[18]   : 送给mcu的hifi组合中断屏蔽位 */
        unsigned int  reserved_1            : 13; /* bit[19-31]: [31：16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
                                                                 [31：28]：保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_etr_axi_csysreq_n_START      (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_etr_axi_csysreq_n_END        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_hifi_int_mask_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_hifi_int_mask_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_hifi_all_int_mask_START      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_hifi_all_int_mask_END        (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_etr_axi_csysreq_n_msk_START  (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_etr_axi_csysreq_n_msk_END    (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_hifi_int_mask_msk_START      (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_hifi_int_mask_msk_END        (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_hifi_all_int_mask_msk_START  (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL1_hifi_all_int_mask_msk_END    (18)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL2_UNION
 结构说明  : SC_PERIPH_CTRL2 寄存器结构定义。地址偏移量:0x004，初值:0x0C9B020F，宽度:32
 寄存器说明: 外设控制寄存器2。MMC CLK PHASE BYPASS控制，func_test选择，cssys时间戳使能，ddrt,g3d mux选择，testpin 选择，hifi memory控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mmc_clk_phase_bypass_en_mmc0 : 2;  /* bit[0-1]  : MMC0 [0]sample，[1]drv端
                                                                        0：clkphaseused
                                                                        1;clkphasebypass */
        unsigned int  mmc_clk_phase_bypass_en_mmc1 : 2;  /* bit[2-3]  : MMC1 [0]sample，[1]drv端
                                                                        0：clkphaseused
                                                                        1;clkphasebypass */
        unsigned int  reserved_0                   : 2;  /* bit[4-5]  : reserved */
        unsigned int  nand_sys_mem_sel             : 1;  /* bit[6]    : nand mem映射mux选择：(只有当ATE nandc启动时配置为0，其它场景软件配置为1）
                                                                        0：nand
                                                                        1：sram */
        unsigned int  g3d_ddrt_axi_sel             : 1;  /* bit[7]    : ddrt与g3d在ddr axi4口的mux选择：
                                                                        0：g3d
                                                                        1：ddrt */
        unsigned int  gu_mdm_bbp_testpin_sel       : 1;  /* bit[8]    : testpin选择信号：
                                                                        0：gubbp
                                                                        1：mdmbbp */
        unsigned int  codec_ssi_master_check       : 3;  /* bit[9-11] : codec ssi master check
                                                                        延迟时间=1.5+0.5*master_check */
        unsigned int  func_test_soft               : 3;  /* bit[12-14]: func_test_soft软件选择控制,总共选择8个分组
                                                                        000：soctest h3
                                                                        001：soctest s1
                                                                        010：soctest s2
                                                                        011：soctest s3
                                                                        100：soctest s4
                                                                        101：soctest s5
                                                                        110：soctest s6
                                                                        111：soctest s7 */
        unsigned int  cssys_ts_enable              : 1;  /* bit[15]   : cssys TS enable。默认0
                                                                        1'b0：timestampe计数不使能
                                                                        1'b1：timestampe计数使能 */
        unsigned int  hifi_ramctrl_s_ema           : 3;  /* bit[16-18]: memory读速率控制，随此值的增大，memory读延时增大 */
        unsigned int  reserved_1                   : 1;  /* bit[19]   : reserved */
        unsigned int  hifi_ramctrl_s_emaw          : 2;  /* bit[20-21]: memory写速率控制，随此值的增大，memory写延时增大（文档含义是这样，看LIB含义又不是，后面会更新文档） */
        unsigned int  hifi_ramctrl_s_emas          : 1;  /* bit[22]   : 对HSmemory有作用；
                                                                        对HDmemory为保留bit，无意义； */
        unsigned int  reserved_2                   : 3;  /* bit[23-25]: reserved */
        unsigned int  hifi_ramctrl_s_ret1n         : 1;  /* bit[26]   : retention控制1
                                                                        0：retention模式1；
                                                                        1：正常工作模式。 */
        unsigned int  hifi_ramctrl_s_ret2n         : 1;  /* bit[27]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                        retention控制2
                                                                        0：retention模式2；
                                                                        1：正常工作模式。 */
        unsigned int  hifi_ramctrl_s_pgen          : 1;  /* bit[28]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                        0：正常工作模式；
                                                                        1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_3                   : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_mmc_clk_phase_bypass_en_mmc0_START  (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_mmc_clk_phase_bypass_en_mmc0_END    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_mmc_clk_phase_bypass_en_mmc1_START  (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_mmc_clk_phase_bypass_en_mmc1_END    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_nand_sys_mem_sel_START              (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_nand_sys_mem_sel_END                (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_g3d_ddrt_axi_sel_START              (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_g3d_ddrt_axi_sel_END                (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_gu_mdm_bbp_testpin_sel_START        (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_gu_mdm_bbp_testpin_sel_END          (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_codec_ssi_master_check_START        (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_codec_ssi_master_check_END          (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_func_test_soft_START                (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_func_test_soft_END                  (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_cssys_ts_enable_START               (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_cssys_ts_enable_END                 (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_ema_START            (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_ema_END              (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_emaw_START           (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_emaw_END             (21)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_emas_START           (22)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_emas_END             (22)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_ret1n_START          (26)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_ret1n_END            (26)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_ret2n_START          (27)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_ret2n_END            (27)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_pgen_START           (28)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL2_hifi_ramctrl_s_pgen_END             (28)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL3_UNION
 结构说明  : SC_PERIPH_CTRL3 寄存器结构定义。地址偏移量:0x008，初值:0x0FFFC000，宽度:32
 寄存器说明: 外设控制寄存器3主要包括memory share相关控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hifi_ddr_harqmem_addr : 12; /* bit[0-11] : hifi映射到harq memory的地址 */
        unsigned int  hifi_harqmemrmp_en    : 1;  /* bit[12]   : 0：不允许映射
                                                                 1：允许映射 */
        unsigned int  harqmem_sys_med_sel   : 1;  /* bit[13]   : harq mem 64bit mux选择
                                                                 0：sysnoc
                                                                 1：mednoc */
        unsigned int  soc_ap_occupy_grp1    : 2;  /* bit[14-15]: 2’b10：group1被SOC ram1（MMU）占用
                                                                 2’b11：group 1被SOC ram2（MDE和SysNoC）占用
                                                                 2’b0X：group 1被BBP占用 */
        unsigned int  soc_ap_occupy_grp2    : 2;  /* bit[16-17]: 2’b10：group 2被SOC ram1（MMU）占用
                                                                 2’b11：group 2被SOC ram2（MDE和SysNoC）占用
                                                                 2’b0X：group 2被BBP占用 */
        unsigned int  soc_ap_occupy_grp3    : 2;  /* bit[18-19]: 2’b10：group 3被SOC ram1（MMU）占用
                                                                 2’b11：group 3被SOC ram2（MDE和SysNoC）占用
                                                                 2’b0X：group 3被BBP占用 */
        unsigned int  soc_ap_occupy_grp4    : 2;  /* bit[20-21]: 2’b10：group 4被SOC ram1（MMU）占用
                                                                 2’b11：group 4被SOC ram2（MDE和SysNoC）占用
                                                                 2’b0X：group 4被BBP占用 */
        unsigned int  soc_ap_occupy_grp5    : 2;  /* bit[22-23]: 2’b10：group 5被SOC ram1（MMU）占用
                                                                 2’b11：group 5被SOC ram2（MDE和SysNoC）占用
                                                                 2’b0X：group 5被BBP占用 */
        unsigned int  soc_ap_occupy_grp6    : 2;  /* bit[24-25]: 2’b10：group 6被SOC ram1（MMU）占用
                                                                 2’b11：group 6被SOC ram2（MDE和SysNoC）占用
                                                                 2’b0X：group 6被BBP占用 */
        unsigned int  reserved_0            : 2;  /* bit[26-27]: reserved */
        unsigned int  reserved_1            : 4;  /* bit[28-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_hifi_ddr_harqmem_addr_START  (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_hifi_ddr_harqmem_addr_END    (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_hifi_harqmemrmp_en_START     (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_hifi_harqmemrmp_en_END       (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_harqmem_sys_med_sel_START    (13)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_harqmem_sys_med_sel_END      (13)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp1_START     (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp1_END       (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp2_START     (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp2_END       (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp3_START     (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp3_END       (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp4_START     (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp4_END       (21)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp5_START     (22)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp5_END       (23)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp6_START     (24)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL3_soc_ap_occupy_grp6_END       (25)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL4_UNION
 结构说明  : SC_PERIPH_CTRL4 寄存器结构定义。地址偏移量:0x00c，初值:0xE0130363，宽度:32
 寄存器说明: 外设系统控制寄存器，功能包含USBOGT、PICOPHY控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pico_fselv                : 3;  /* bit[0-2]  : [2：0]：PICOPHY的fselv控制信号 */
        unsigned int  fpga_ext_phy_sel          : 1;  /* bit[3]    : [3]：fpga中使用外部phy还是内部phy
                                                                     0：使用内部phy
                                                                     1：使用外部phy */
        unsigned int  pico_refclksel            : 2;  /* bit[4-5]  : [5：4]：PICOPHY的refclksel配置 */
        unsigned int  pico_siddq                : 1;  /* bit[6]    : [6]siddq配置：
                                                                     1：使能iddq
                                                                     0：不使能 */
        unsigned int  pico_suspendm_sleepm      : 1;  /* bit[7]    : [7]：配置软件使能PICOPHY进入或退出suspendm或sleepm模式是否有效。
                                                                     1：为高，有效；
                                                                     0：为低，进入suspendm模式； */
        unsigned int  pico_ogdisable            : 1;  /* bit[8]    : [8]：PICOPHY的otgdisable配置 */
        unsigned int  pico_commononn            : 1;  /* bit[9]    : [9]：PICOPHY的commononn配置 */
        unsigned int  pico_vbusvldext           : 1;  /* bit[10]   : [10]：PICOPHY的vbusvldext配置 */
        unsigned int  pico_vbusvldextsel        : 1;  /* bit[11]   : [11]：PICOPHY的vbusvldextsel配置 */
        unsigned int  pico_vatestenb            : 2;  /* bit[12-13]: [13：12]：PICOPHY的vatestenb配置 */
        unsigned int  pico_suspendm             : 1;  /* bit[14]   : [14]：配置PICOPHY进入或退出suspendm模式，仅在sc_periph_ctrl4[7]有效时，此位才起作用。
                                                                     1：为高，退出suspendm模式；
                                                                     0：为低，进入suspendm模式； */
        unsigned int  pico_sleepm               : 1;  /* bit[15]   : [15]：配置PICOPHY进入或退出sleepm模式，仅在sc_periph_ctrl4[7]有效时，此位才起作用。
                                                                     1：为高，退出sleepm模式；
                                                                     0：为低，进入sleepm模式； */
        unsigned int  bc11_c                    : 1;  /* bit[16]   : [16]：配置USB BC1.1 c信号有效，在Device_only需要配成有效,在Host_only需要配成无效；仅在sc_periph_ctrl5[3]和sc_periph_ctrl5[10]均为无效时，此位才起作用。
                                                                     1：为高，有效；
                                                                     0：为低，无效； */
        unsigned int  bc11_b                    : 1;  /* bit[17]   : [17]：配置USB BC1.1 b信号有效，在Device_only需要配成有效,在Host_only需要配成无效；仅在sc_periph_ctrl5[3]和sc_periph_ctrl5[10]均为无效时，此位才起作用。
                                                                     1：为高，有效；
                                                                     0：为低，无效； */
        unsigned int  bc11_a                    : 1;  /* bit[18]   : [18]：配置USB BC1.1 a信号有效，在Host_only需要配成有效,在Device_only需要配成无效；仅在sc_periph_ctrl5[3]和sc_periph_ctrl5[10]均为无效时，此位才起作用。
                                                                     1：为高，有效；
                                                                     0：为低，无效； */
        unsigned int  bc11_gnd                  : 1;  /* bit[19]   : [19]：配置USB BC1.1 gnd信号有效，在Host_only需要配成有效,在Device_only需要配成无效；仅在sc_periph_ctrl5[3]和sc_periph_ctrl5[10]均为无效时，此位才起作用。
                                                                     1：为高，有效；
                                                                     0：为低，无效； */
        unsigned int  bc11_float                : 1;  /* bit[20]   : [20]：配置USB BC1.1 float信号有效，在Device_only需要配成有效,在Host_only需要配成无效；仅在sc_periph_ctrl5[3]和sc_periph_ctrl5[10]均为无效时，此位才起作用。
                                                                     1：为高，有效；
                                                                     0：为低，无效； */
        unsigned int  otg_phy_sel               : 1;  /* bit[21]   : 选择是否使用otg的phy：
                                                                     1：选择
                                                                     0：不选择 */
        unsigned int  usb_otg_ss_scaledown_mode : 2;  /* bit[22-23]: otg仅在仿真模式下使用
                                                                     00：使用实际时间仿真
                                                                     01：减少仿真时间（除了设备挂起和唤醒时间）
                                                                     10：只减少设备挂起和唤醒的时间
                                                                     11：都减少 */
        unsigned int  otg_dm_pulldown           : 1;  /* bit[24]   : [24]：配置USBOTG的dm_pulldown信号有效，在Host_only需要配成有效；在OTG模式下此位不起作用。
                                                                     1：dm_pulldown为高，有效；
                                                                     0：dm_pulldown为低，无效； */
        unsigned int  otg_dp_pulldown           : 1;  /* bit[25]   : [25]：配置USBOTG的dp_pulldown信号有效，在Host_only需要配成有效；在OTG模式下此位不起作用。
                                                                     1：dp_pulldown为高，有效；
                                                                     0：dp_pulldown为低，无效； */
        unsigned int  otg_idpullup              : 1;  /* bit[26]   : [26]： 配置USBOTG的idpullup信号有效，在不使能ACA功能，且使能OTG功能时，需要配成有效。
                                                                     1：idpullup为高，有效；
                                                                     0：idpullup为低，无效； */
        unsigned int  otg_drvbus                : 1;  /* bit[27]   : [27]：配置USBOTG的drvvbus信号有效，在Host_only需要配成有效；在OTG模式下此位不起作用。
                                                                     1：drvvbus为高，有效；
                                                                     0：drvvbus为低，无效； */
        unsigned int  otg_sessend               : 1;  /* bit[28]   : [28]：配置USBOTG的sessend信号有效，在Device_only或Host_only需要配成无效；在OTG模式下此位不起作用。
                                                                     1：sessend为高，有效；0：sessend为低，无效； */
        unsigned int  otg_bvalid                : 1;  /* bit[29]   : [29]：配置USBOTG的bvalid信号有效，在Device_only或Host_only需要配成有效；在OTG模式下此位不起作用。
                                                                     1：bvalid为高，有效；0：bvalid为低，无效； */
        unsigned int  otg_avalid                : 1;  /* bit[30]   : [30]：配置USBOTG的avalid信号有效，在Device_only或Host_only需要配成有效；在OTG模式下此位不起作用。
                                                                     1：avalid为高，有效；0：avalid为低，无效； */
        unsigned int  otg_vbusvalid             : 1;  /* bit[31]   : [31]：配置USBOTG的vbusvalid信号有效，在Device_only或Host_only需要配成有效；在OTG模式下此位不起作用。
                                                                     1：vbusvalid为高，有效；0：vbusvalid为低，无效； */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL4_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_fselv_START                 (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_fselv_END                   (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_fpga_ext_phy_sel_START           (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_fpga_ext_phy_sel_END             (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_refclksel_START             (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_refclksel_END               (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_siddq_START                 (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_siddq_END                   (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_suspendm_sleepm_START       (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_suspendm_sleepm_END         (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_ogdisable_START             (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_ogdisable_END               (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_commononn_START             (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_commononn_END               (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_vbusvldext_START            (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_vbusvldext_END              (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_vbusvldextsel_START         (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_vbusvldextsel_END           (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_vatestenb_START             (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_vatestenb_END               (13)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_suspendm_START              (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_suspendm_END                (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_sleepm_START                (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_pico_sleepm_END                  (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_c_START                     (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_c_END                       (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_b_START                     (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_b_END                       (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_a_START                     (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_a_END                       (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_gnd_START                   (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_gnd_END                     (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_float_START                 (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_bc11_float_END                   (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_phy_sel_START                (21)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_phy_sel_END                  (21)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_usb_otg_ss_scaledown_mode_START  (22)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_usb_otg_ss_scaledown_mode_END    (23)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_dm_pulldown_START            (24)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_dm_pulldown_END              (24)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_dp_pulldown_START            (25)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_dp_pulldown_END              (25)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_idpullup_START               (26)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_idpullup_END                 (26)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_drvbus_START                 (27)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_drvbus_END                   (27)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_sessend_START                (28)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_sessend_END                  (28)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_bvalid_START                 (29)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_bvalid_END                   (29)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_avalid_START                 (30)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_avalid_END                   (30)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_vbusvalid_START              (31)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL4_otg_vbusvalid_END                (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL5_UNION
 结构说明  : SC_PERIPH_CTRL5 寄存器结构定义。地址偏移量:0x010，初值:0x000000A0，宽度:32
 寄存器说明: 外设系统控制寄存器5，功能包含PICOPHY控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0         : 3;  /* bit[0-2]  : [2：0]：保留。 */
        unsigned int  usbotg_res_sel     : 1;  /* bit[3]    : [3]：USBOTG的电阻选择控制
                                                              1：选择PICOPHY的输出aca电阻；
                                                              0：选择sc_periph_ctrl4的[20：16]配置的电阻值。 */
        unsigned int  picophy_acaenb     : 1;  /* bit[4]    : [4]：PICOPHY的acaenb配置 */
        unsigned int  picophy_bc_mode    : 1;  /* bit[5]    : [5]：PICOPHY的bc_mode配置 */
        unsigned int  picophy_chrgsel    : 1;  /* bit[6]    : [6]：PICOPHY的chrgsel配置 */
        unsigned int  picophy_vdatsrcend : 1;  /* bit[7]    : [7]：PICOPHY的vdatsrcend配置 */
        unsigned int  picophy_vdatdetenb : 1;  /* bit[8]    : [8]：PICOPHY的vdatdetenb配置 */
        unsigned int  picophy_dcdenb     : 1;  /* bit[9]    : [9]：PICOPHY的dcdenb配置 */
        unsigned int  picophy_iddig      : 1;  /* bit[10]   : [10]：PICOPHY的iddig使能配置 */
        unsigned int  dbg_mux            : 2;  /* bit[11-12]: debug信号输出mux选择：
                                                              00：选择usb_state_internal
                                                              01：debug状态信号1
                                                              10：debug状态信号2
                                                              11：不使用 */
        unsigned int  reserved_1         : 19; /* bit[13-31]: [31：11]：保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL5_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_usbotg_res_sel_START      (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_usbotg_res_sel_END        (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_acaenb_START      (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_acaenb_END        (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_bc_mode_START     (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_bc_mode_END       (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_chrgsel_START     (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_chrgsel_END       (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_vdatsrcend_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_vdatsrcend_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_vdatdetenb_START  (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_vdatdetenb_END    (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_dcdenb_START      (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_dcdenb_END        (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_iddig_START       (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_picophy_iddig_END         (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_dbg_mux_START             (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL5_dbg_mux_END               (12)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL6_UNION
 结构说明  : SC_PERIPH_CTRL6 寄存器结构定义。地址偏移量:0x014，初值:0x00000C9B，宽度:32
 寄存器说明: 外设控制寄存器6。功能包含CSSYS OFF的memroy控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cssysoff_ramctrl_s_ema   : 3;  /* bit[0-2]  : memory读速率控制，随此值的增大，memory读延时增大 */
        unsigned int  reserved_0               : 1;  /* bit[3]    : reserved */
        unsigned int  cssysoff_ramctrl_s_emaw  : 2;  /* bit[4-5]  : memory写速率控制，随此值的增大，memory写延时增大（文档含义是这样，看LIB含义又不是，后面会更新文档） */
        unsigned int  cssysoff_ramctrl_s_emas  : 1;  /* bit[6]    : 对HSmemory有作用；
                                                                    对HDmemory为保留bit，无意义； */
        unsigned int  reserved_1               : 3;  /* bit[7-9]  : reserved */
        unsigned int  cssysoff_ramctrl_s_ret1n : 1;  /* bit[10]   : retention控制1
                                                                    0：retention模式1；
                                                                    1：正常工作模式。 */
        unsigned int  cssysoff_ramctrl_s_ret2n : 1;  /* bit[11]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    retention控制2
                                                                    0：retention模式2；
                                                                    1：正常工作模式。 */
        unsigned int  cssysoff_ramctrl_s_pgen  : 1;  /* bit[12]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    0：正常工作模式；
                                                                    1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_2               : 3;  /* bit[13-15]: reserved */
        unsigned int  reserved_3               : 16; /* bit[16-31]: 其他：保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL6_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_ema_START    (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_ema_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_emaw_START   (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_emaw_END     (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_emas_START   (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_emas_END     (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_ret1n_START  (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_ret1n_END    (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_ret2n_START  (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_ret2n_END    (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_pgen_START   (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL6_cssysoff_ramctrl_s_pgen_END     (12)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL8_UNION
 结构说明  : SC_PERIPH_CTRL8 寄存器结构定义。地址偏移量:0x018，初值:0x303344D1，宽度:32
 寄存器说明: 外设控制寄存器8。功能包含PICOPHY控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_ctrl8_picophy_txrisetune0        : 2;  /* bit[0-1]  : PICOPHY的txrisetune0配置 */
        unsigned int  periph_ctrl8_picophy_txpreempamptune0   : 2;  /* bit[2-3]  : PICOPHY的txpreempamptune0配置 */
        unsigned int  periph_ctrl8_picophy_txrestune0         : 2;  /* bit[4-5]  : PICOPHY的txrestune0配置 */
        unsigned int  periph_ctrl8_picophy_txhssvtune0        : 2;  /* bit[6-7]  : PICOPHY的txhssvtune0配置 */
        unsigned int  periph_ctrl8_picophy_compdistune0       : 3;  /* bit[8-10] : PICOPHY的compdistune0配置 */
        unsigned int  periph_ctrl8_picophy_txpreemppulsetune0 : 1;  /* bit[11]   : PICOPHY的txpreemppulsetune0配置 */
        unsigned int  periph_ctrl8_picophy_otgtune0           : 3;  /* bit[12-14]: PICOPHY的otgtune0配置 */
        unsigned int  reserved_0                              : 1;  /* bit[15]   : 保留。 */
        unsigned int  periph_ctrl8_picophy_sqrxtune0          : 3;  /* bit[16-18]: PICOPHY的sqrxtune0配置 */
        unsigned int  reserved_1                              : 1;  /* bit[19]   : 保留。 */
        unsigned int  periph_ctrl8_picophy_txvreftune0        : 4;  /* bit[20-23]: PICOPHY的txvreftune0配置 */
        unsigned int  reserved_2                              : 4;  /* bit[24-27]: 保留。 */
        unsigned int  periph_ctrl8_picophy_txfslstune0        : 4;  /* bit[28-31]: PICOPHY的txfslstune0配置 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL8_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txrisetune0_START         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txrisetune0_END           (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txpreempamptune0_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txpreempamptune0_END      (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txrestune0_START          (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txrestune0_END            (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txhssvtune0_START         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txhssvtune0_END           (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_compdistune0_START        (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_compdistune0_END          (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txpreemppulsetune0_START  (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txpreemppulsetune0_END    (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_otgtune0_START            (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_otgtune0_END              (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_sqrxtune0_START           (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_sqrxtune0_END             (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txvreftune0_START         (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txvreftune0_END           (23)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txfslstune0_START         (28)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL8_periph_ctrl8_picophy_txfslstune0_END           (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL9_UNION
 结构说明  : SC_PERIPH_CTRL9 寄存器结构定义。地址偏移量:0x01C，初值:0x00000000，宽度:32
 寄存器说明: 外设控制寄存器9。功能包含PICOPHY控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_ctrl9_picoply_testclken      : 1;  /* bit[0]    : PICOPHY的testclken */
        unsigned int  periph_ctrl9_picoply_testdataoutsel : 1;  /* bit[1]    : PICOPHY的testdataoutsel配置 */
        unsigned int  reserved_0                          : 2;  /* bit[2-3]  : 保留 */
        unsigned int  periph_ctrl9_picoply_testaddr       : 4;  /* bit[4-7]  : PICOPHY的testaddr配置 */
        unsigned int  periph_ctrl9_picoply_testdatain     : 8;  /* bit[8-15] : PICOPHY的testdatain配置 */
        unsigned int  reserved_1                          : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL9_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL9_periph_ctrl9_picoply_testclken_START       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL9_periph_ctrl9_picoply_testclken_END         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL9_periph_ctrl9_picoply_testdataoutsel_START  (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL9_periph_ctrl9_picoply_testdataoutsel_END    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL9_periph_ctrl9_picoply_testaddr_START        (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL9_periph_ctrl9_picoply_testaddr_END          (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL9_periph_ctrl9_picoply_testdatain_START      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL9_periph_ctrl9_picoply_testdatain_END        (15)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL10_UNION
 结构说明  : SC_PERIPH_CTRL10 寄存器结构定义。地址偏移量:0x020，初值:0x00000000，宽度:32
 寄存器说明: 外设控制寄存器10。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL10_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL12_UNION
 结构说明  : SC_PERIPH_CTRL12 寄存器结构定义。地址偏移量:0x024，初值:0x0C9B0C9B，宽度:32
 寄存器说明: 外设控制寄存器12。功能包含PERISYS的memroy控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  perisys_ramctrl_d1w2r_emaa       : 3;  /* bit[0-2]  : memory读速率控制，随此值的增大，memory读延时增大 */
        unsigned int  perisys_ramctrl_d1w2r_emab       : 3;  /* bit[3-5]  : 含义还不是很确定 */
        unsigned int  perisys_ramctrl_d1w2r_emasa      : 1;  /* bit[6]    : 含义还不是很确定 */
        unsigned int  perisys_ramctrl_d1w2r_colldisn   : 1;  /* bit[7]    : 同时读写一个地址时，memory的行为
                                                                            0：读写都不保证正确；
                                                                            1：写保证正确，读不保证 */
        unsigned int  reserved_0                       : 2;  /* bit[8-9]  : reserved */
        unsigned int  perisys_ramctrl_d1w2r_ret1n      : 1;  /* bit[10]   : retention控制1
                                                                            0：retention模式1；
                                                                            1：正常工作模式。 */
        unsigned int  perisys_ramctrl_d1w2r_ret2n      : 1;  /* bit[11]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                            retention控制2
                                                                            0：retention模式2；
                                                                            1：正常工作模式。 */
        unsigned int  perisys_ramctrl_d1w2r_pgen       : 1;  /* bit[12]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                            0：正常工作模式；
                                                                            1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_1                       : 3;  /* bit[13-15]: reserved */
        unsigned int  perisys_ramctrl_d1rw2rw_ema      : 3;  /* bit[16-18]: memory读速率控制，随此值的增大，memory读延时增大，memoryA口和B口共享此配置。 */
        unsigned int  reserved_2                       : 1;  /* bit[19]   : reserved */
        unsigned int  perisys_ramctrl_d1rw2rw_emaw     : 2;  /* bit[20-21]: memory写速率控制，随此值的增大，memory写延时增大（文档含义是这样，看LIB含义又不是，后面会更新文档）。memoryA口和B口共享此配置。 */
        unsigned int  perisys_ramctrl_d1rw2rw_emas     : 1;  /* bit[22]   : memoryA口和B口共享此配置。
                                                                            对HSmemory有作用；
                                                                            对HDmemory不确定，若无此配置，按无意义处理； */
        unsigned int  perisys_ramctrl_d1rw2rw_colldisn : 1;  /* bit[23]   : 同时读写一个地址时，memory的行为
                                                                            0：读写都不保证正确；
                                                                            1：写保证正确，读不保证 */
        unsigned int  reserved_3                       : 2;  /* bit[24-25]: reserved */
        unsigned int  perisys_ramctrl_d1rw2rw_ret1n    : 1;  /* bit[26]   : retention控制1
                                                                            0：retention模式1；
                                                                            1：正常工作模式。 */
        unsigned int  perisys_ramctrl_d1rw2rw_ret2n    : 1;  /* bit[27]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                            retention控制2
                                                                            0：retention模式2；
                                                                            1：正常工作模式。 */
        unsigned int  perisys_ramctrl_d1rw2rw_pgen     : 1;  /* bit[28]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                            0：正常工作模式；
                                                                            1：配合ret1n/ret2n完成retention。 */
        unsigned int  perisys_ramctrl_d1rw2rw          : 3;  /* bit[29-31]: perisys_ramctrl_d1rw2rw */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL12_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_emaa_START        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_emaa_END          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_emab_START        (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_emab_END          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_emasa_START       (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_emasa_END         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_colldisn_START    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_colldisn_END      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_ret1n_START       (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_ret1n_END         (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_ret2n_START       (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_ret2n_END         (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_pgen_START        (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1w2r_pgen_END          (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_ema_START       (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_ema_END         (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_emaw_START      (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_emaw_END        (21)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_emas_START      (22)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_emas_END        (22)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_colldisn_START  (23)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_colldisn_END    (23)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_ret1n_START     (26)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_ret1n_END       (26)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_ret2n_START     (27)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_ret2n_END       (27)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_pgen_START      (28)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_pgen_END        (28)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_START           (29)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL12_perisys_ramctrl_d1rw2rw_END             (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL13_UNION
 结构说明  : SC_PERIPH_CTRL13 寄存器结构定义。地址偏移量:0x028，初值:0x000007F8，宽度:32
 寄存器说明: 外设控制寄存器13。包括acpu，ccpu，hifi时钟切换使能，mmc环回时钟门控使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_ctrl13_acpu_clk_sel_bypass_en : 1;  /* bit[0]    : acpu 使用状态信号作为时钟切换条件 使能：
                                                                                0：使能
                                                                                1：不使能 */
        unsigned int  periph_ctrl13_ccpu_clk_sel_bypass_en : 1;  /* bit[1]    : ccpu 使用状态信号作为时钟切换条件 使能：
                                                                                0：使能
                                                                                1：不使能 */
        unsigned int  periph_ctrl13_hifi_clk_sel_bypass_en : 1;  /* bit[2]    : hifi 使用状态信号作为时钟切换条件 使能：
                                                                                0：使能
                                                                                1：不使能 */
        unsigned int  periph_ctrl13_mmc0_ckg_bypass_en     : 1;  /* bit[3]    : mmc0 通过mmc0产生的stop_clk信号关钟功能
                                                                                1：功能bypass
                                                                                0：功能使能 */
        unsigned int  periph_ctrl13_mmc1_ckg_bypass_en     : 1;  /* bit[4]    : mmc1 通过mmc1产生的stop_clk信号关钟功能
                                                                                1：功能bypass
                                                                                0：功能使能 */
        unsigned int  periph_ctrl13_mmc2_ckg_bypass_en     : 1;  /* bit[5]    : mmc2 通过mmc2产生的stop_clk信号关钟功能
                                                                                1：功能bypass
                                                                                0：功能使能 */
        unsigned int  periph_ctrl13_ssi_pad_ckg_bypass_en  : 1;  /* bit[6]    : ssi pad通过pumssi和codecssi idle信号自动门控送往pad时钟功能：
                                                                                1：功能bypass
                                                                                0：功能使能 */
        unsigned int  periph_ctrl13_hprot2_usb             : 1;  /* bit[7]    : usb hprot2配置，强制配置为bufferable。如果性能满足的情况下可以配置为0。 */
        unsigned int  periph_ctrl13_hprot2_mmc0            : 1;  /* bit[8]    : mmc0 hprot2配置，强制配置为bufferable。如果性能满足的情况下可以配置为0。 */
        unsigned int  periph_ctrl13_hprot2_mmc1            : 1;  /* bit[9]    : mmc1 hprot2配置，强制配置为bufferable。如果性能满足的情况下可以配置为0。 */
        unsigned int  periph_ctrl13_hprot2_mmc2            : 1;  /* bit[10]   : mmc2 hprot2配置，强制配置为bufferable。如果性能满足的情况下可以配置为0。 */
        unsigned int  reserved                             : 21; /* bit[11-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL13_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_acpu_clk_sel_bypass_en_START  (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_acpu_clk_sel_bypass_en_END    (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_ccpu_clk_sel_bypass_en_START  (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_ccpu_clk_sel_bypass_en_END    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hifi_clk_sel_bypass_en_START  (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hifi_clk_sel_bypass_en_END    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_mmc0_ckg_bypass_en_START      (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_mmc0_ckg_bypass_en_END        (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_mmc1_ckg_bypass_en_START      (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_mmc1_ckg_bypass_en_END        (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_mmc2_ckg_bypass_en_START      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_mmc2_ckg_bypass_en_END        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_ssi_pad_ckg_bypass_en_START   (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_ssi_pad_ckg_bypass_en_END     (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hprot2_usb_START              (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hprot2_usb_END                (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hprot2_mmc0_START             (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hprot2_mmc0_END               (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hprot2_mmc1_START             (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hprot2_mmc1_END               (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hprot2_mmc2_START             (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL13_periph_ctrl13_hprot2_mmc2_END               (10)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CTRL14_UNION
 结构说明  : SC_PERIPH_CTRL14 寄存器结构定义。地址偏移量:0x02C，初值:0x00000000，宽度:32
 寄存器说明: 外设控制寄存器14。时钟频率检测
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_ctrl14_fm_en      : 1;  /* bit[0]    : 时钟检测频率使能：
                                                                    1：使能
                                                                    0：不使能 */
        unsigned int  reserved_0               : 7;  /* bit[1-7]  :  */
        unsigned int  periph_ctrl14_fm_clk_sel : 8;  /* bit[8-15] : 检测时钟选择：
                                                                    00_0000： clk_syspll_div2_src
                                                                    00_0001： clk_300m_src
                                                                    00_0010： clk_syspll_div_src
                                                                    00_0011： clk_bbppll0_div5_src
                                                                    00_0100： clk_syspll_div8_src
                                                                    00_0101： clk_acpu_div_src
                                                                    00_0110： clk_ddrphy_ref_src
                                                                    00_0111： clk_mmc0_src
                                                                    00_1000： clk_mmc1_src
                                                                    00_1001： clk_mmc2_src
                                                                    00_1010： clk_picophy_test_src
                                                                    00_1011： clk_hifi_src
                                                                    00_1100： clk_cs_tpiu_src
                                                                    00_1101： clk_tsensor_src
                                                                    00_1110： clk_150m_src
                                                                    00_1111： clk_bus_src
                                                                    01_0000： clk_ddrc_axi1_src
                                                                    01_0001： clk_ddrc_axi3_src
                                                                    01_0010： clk_ddrc_axi_src
                                                                    01_0011： clk_sys_noc_150m_src
                                                                    01_0100： clk_harq_src
                                                                    01_0101： clk_cs_dapb_src
                                                                    01_0110： clk_sys_sel_src
                                                                    01_0111： clk_ddr_sel_src
                                                                    01_1000： clk_slow_off_src
                                                                    01_1001： clk_ccpu_sel_src
                                                                    01_1010： clk_uart1_src
                                                                    01_1011： clk_uart2_src
                                                                    01_1100： clk_uart3_src
                                                                    01_1101： clk_uart4_src
                                                                    01_1110： clk_acpupll_src
                                                                    01_1111： clk_gpupll_src
                                                                    10_0000： clk_medpll_src
                                                                    10_0001： clk_syspll_src
                                                                    10_0010： clk_ddrpll_src
                                                                    10_0011： clk_ddrpll1_src
                                                                    10_0100： clk_bbppll0_src
                                                                    10_0101： clk_sys_on
                                                                    10_0110： clk_tcxo
                                                                    10_0111： jtag_tck
                                                                    default： clk_bus_src */
        unsigned int  reserved_1               : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CTRL14_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL14_periph_ctrl14_fm_en_START       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL14_periph_ctrl14_fm_en_END         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL14_periph_ctrl14_fm_clk_sel_START  (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CTRL14_periph_ctrl14_fm_clk_sel_END    (15)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_MEM_CTRL0_UNION
 结构说明  : SC_MEM_CTRL0 寄存器结构定义。地址偏移量:0x030，初值:0x00230C9B，宽度:32
 寄存器说明: aarm子系统控制寄存器，功能包含sram_ap_off/ipf/seceng_cc/seceng_c2/seceng_rc4/socp和rom的memory控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  perisys_ramctrl_s_ema   : 3;  /* bit[0-2]  : memory读速率控制，随此值的增大，memory读延时增大 */
        unsigned int  reserved_0              : 1;  /* bit[3]    : reserved */
        unsigned int  perisys_ramctrl_s_emaw  : 2;  /* bit[4-5]  : memory写速率控制，随此值的增大，memory写延时增大（文档含义是这样，看LIB含义又不是，后面会更新文档） */
        unsigned int  perisys_ramctrl_s_emas  : 1;  /* bit[6]    : 对HSmemory有作用；
                                                                   对HDmemory为保留bit，无意义； */
        unsigned int  reserved_1              : 3;  /* bit[7-9]  : reserved */
        unsigned int  perisys_ramctrl_s_ret1n : 1;  /* bit[10]   : retention控制1
                                                                   0：retention模式1；
                                                                   1：正常工作模式。 */
        unsigned int  perisys_ramctrl_s_ret2n : 1;  /* bit[11]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                   retention控制2
                                                                   0：retention模式2；
                                                                   1：正常工作模式。 */
        unsigned int  perisys_ramctrl_s_pgen  : 1;  /* bit[12]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                   0：正常工作模式；
                                                                   1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_2              : 3;  /* bit[13-15]: reserved */
        unsigned int  rom_ctrl_ema            : 3;  /* bit[16-18]: memory读速率控制，随此值的增大，memory读延时增大，memoryA口和B口共享此配置。 */
        unsigned int  reserved_3              : 1;  /* bit[19]   : reserved */
        unsigned int  rom_ctrl_pgen           : 1;  /* bit[20]   : 0：正常工作模式；
                                                                   1：具有retention功能 */
        unsigned int  rom_ctrl_ken            : 1;  /* bit[21]   : keeperenable
                                                                   0：keepfunctionstateofthebitlinesbyholdingthebitlinevalues,saveleakagebutshowdowntheaccesstimeby1~2%
                                                                   1：keeperdisable */
        unsigned int  reserved_4              : 2;  /* bit[22-23]: reserved */
        unsigned int  reserved_5              : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_MEM_CTRL0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_ema_START    (0)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_ema_END      (2)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_emaw_START   (4)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_emaw_END     (5)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_emas_START   (6)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_emas_END     (6)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_ret1n_START  (10)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_ret1n_END    (10)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_ret2n_START  (11)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_ret2n_END    (11)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_pgen_START   (12)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_perisys_ramctrl_s_pgen_END     (12)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_rom_ctrl_ema_START             (16)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_rom_ctrl_ema_END               (18)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_rom_ctrl_pgen_START            (20)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_rom_ctrl_pgen_END              (20)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_rom_ctrl_ken_START             (21)
#define SOC_PERI_SCTRL_SC_MEM_CTRL0_rom_ctrl_ken_END               (21)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_UNION
 结构说明  : SC_AUTO_CLK_GATE0 寄存器结构定义。地址偏移量:0x034，初值:0x00000003，宽度:32
 寄存器说明: 时钟自动门控使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  apdmac_auto_clken0_sel     : 1;  /* bit[0]    : apdmac_auto_clken0_selPERIPH_MISC
                                                                      0：autoclkgatedisable
                                                                      1：autoclkgageenable */
        unsigned int  apdmac_auto_clken1_sel     : 1;  /* bit[1]    : apdmac_auto_clken1_selPERIPH_MISC
                                                                      0：autoclkgatedisable
                                                                      1：autoclkgageenable */
        unsigned int  reserved_0                 : 1;  /* bit[2]    : reserved */
        unsigned int  reserved_1                 : 1;  /* bit[3]    : reserved */
        unsigned int  reserved_2                 : 12; /* bit[4-15] : reserved */
        unsigned int  apdmac_auto_clken0_sel_msk : 1;  /* bit[16]   : apdmac_auto_clken0_selPERIPH_MISC */
        unsigned int  apdmac_auto_clken1_sel_msk : 1;  /* bit[17]   : apdmac_auto_clken1_selPERIPH_MISC */
        unsigned int  reserved_3                 : 1;  /* bit[18]   : reserved */
        unsigned int  reserved_4                 : 1;  /* bit[19]   : reserved */
        unsigned int  reserved_5                 : 12; /* bit[20-31]: [31：16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变；
                                                                      [31：28]：保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_apdmac_auto_clken0_sel_START      (0)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_apdmac_auto_clken0_sel_END        (0)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_apdmac_auto_clken1_sel_START      (1)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_apdmac_auto_clken1_sel_END        (1)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_apdmac_auto_clken0_sel_msk_START  (16)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_apdmac_auto_clken0_sel_msk_END    (16)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_apdmac_auto_clken1_sel_msk_START  (17)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE0_apdmac_auto_clken1_sel_msk_END    (17)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_AUTO_CLK_GATE1_UNION
 结构说明  : SC_AUTO_CLK_GATE1 寄存器结构定义。地址偏移量:0x038，初值:0x00000000，宽度:32
 寄存器说明: 时钟自动门控使能寄存器。包括NOC100M总线时钟自动降频功能是否使能的投票
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  noc_freq_switch_100m_vote     : 16; /* bit[0-15] : NOC100M总线时钟通过nopending信号自动降频功能是否使能，并且强制总线频率为100M的投票
                                                                         1：投票不使用该功能
                                                                         0：投票使用该功能 */
        unsigned int  noc_freq_switch_100m_vote_msk : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_AUTO_CLK_GATE1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE1_noc_freq_switch_100m_vote_START      (0)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE1_noc_freq_switch_100m_vote_END        (15)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE1_noc_freq_switch_100m_vote_msk_START  (16)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE1_noc_freq_switch_100m_vote_msk_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_AUTO_CLK_GATE2_UNION
 结构说明  : SC_AUTO_CLK_GATE2 寄存器结构定义。地址偏移量:0x03c，初值:0x00000000，宽度:32
 寄存器说明: 时钟自动门控使能寄存器。包括NOC150M总线时钟自动降频功能是否使能的投票
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  noc_freq_switch_150m_vote     : 16; /* bit[0-15] : NOC150M总线时钟通过nopending信号自动降频功能是否使能，并且强制总线频率为150M的投票
                                                                         1：投票不使用该功能
                                                                         0：投票使用该功能 */
        unsigned int  noc_freq_switch_150m_vote_msk : 16; /* bit[16-31]:  */
    } reg;
} SOC_PERI_SCTRL_SC_AUTO_CLK_GATE2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE2_noc_freq_switch_150m_vote_START      (0)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE2_noc_freq_switch_150m_vote_END        (15)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE2_noc_freq_switch_150m_vote_msk_START  (16)
#define SOC_PERI_SCTRL_SC_AUTO_CLK_GATE2_noc_freq_switch_150m_vote_msk_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_DDR_CTRL0_UNION
 结构说明  : SC_DDR_CTRL0 寄存器结构定义。地址偏移量:0x050，初值:0x00000005，宽度:32
 寄存器说明: DDR配置寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  byp_mode                : 1;  /* bit[0]    : phy的pll工作模式
                                                                   0：mission mode
                                                                   1：bypass mode */
        unsigned int  reserved_0              : 1;  /* bit[1]    : reserved */
        unsigned int  reserved_1              : 1;  /* bit[2]    : reserved */
        unsigned int  byp_mode_clk_sel        : 1;  /* bit[3]    : 多路时钟选择信号（软件不可见）：
                                                                   0：强制选择ddr_clka路时钟。
                                                                   1：根据byp_mode值来选择ddr_clka还是ddr_clkb时钟。 */
        unsigned int  ddr_axi2_sysnoc_mid_sel : 1;  /* bit[4]    : ddr axi2 sysnoc口 awid，arid[11：7]选择：
                                                                   0：原来的awid，arid[11：7]
                                                                   1：userbit[5：1] */
        unsigned int  ddr_axi1_smmu_mid_sel   : 1;  /* bit[5]    : ddr axi1 smmu 口 awid，arid[11：9]选择：
                                                                   0：选择原来awid，arid[11：9]
                                                                   1：userbit[3：1]
                                                                   【注意】：
                                                                   对于SMMU有一个软件约束: SMMU发出的Replace操作针对arid和awid附加的4-bit必须是1111。SMMU发的Replace操作相应的userbit[3:1]必须是111，就是SMMU的master ID。 */
        unsigned int  reserved_2              : 8;  /* bit[6-13] : hifimaster */
        unsigned int  reserved_3              : 2;  /* bit[14-15]: mcumaster */
        unsigned int  reserved_4              : 2;  /* bit[16-17]: dmaaximaster */
        unsigned int  reserved_5              : 2;  /* bit[18-19]: acpumaster */
        unsigned int  reserved_6              : 2;  /* bit[20-21]: modemmaster */
        unsigned int  reserved_7              : 2;  /* bit[22-23]: cssysmaster */
        unsigned int  reserved_8              : 1;  /* bit[24]   : systemnoc_timeout_en
                                                                   0：disable timeout
                                                                   1：enable timeout */
        unsigned int  reserved_9              : 7;  /* bit[25-31]: 保留
                                                                   NOC总线QOS优先级设置，可配置值为0～3 */
    } reg;
} SOC_PERI_SCTRL_SC_DDR_CTRL0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_DDR_CTRL0_byp_mode_START                 (0)
#define SOC_PERI_SCTRL_SC_DDR_CTRL0_byp_mode_END                   (0)
#define SOC_PERI_SCTRL_SC_DDR_CTRL0_byp_mode_clk_sel_START         (3)
#define SOC_PERI_SCTRL_SC_DDR_CTRL0_byp_mode_clk_sel_END           (3)
#define SOC_PERI_SCTRL_SC_DDR_CTRL0_ddr_axi2_sysnoc_mid_sel_START  (4)
#define SOC_PERI_SCTRL_SC_DDR_CTRL0_ddr_axi2_sysnoc_mid_sel_END    (4)
#define SOC_PERI_SCTRL_SC_DDR_CTRL0_ddr_axi1_smmu_mid_sel_START    (5)
#define SOC_PERI_SCTRL_SC_DDR_CTRL0_ddr_axi1_smmu_mid_sel_END      (5)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_DDR_CTRL1_UNION
 结构说明  : SC_DDR_CTRL1 寄存器结构定义。地址偏移量:0x054，初值:0x00000000，宽度:32
 寄存器说明: DDR 配置寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddr_qos_ar_acpu : 4;  /* bit[0-3]  : acpu读qos */
        unsigned int  ddr_qos_aw_acpu : 4;  /* bit[4-7]  : acpu写qos */
        unsigned int  reserved_0      : 4;  /* bit[8-11] : g3d读qos */
        unsigned int  reserved_1      : 4;  /* bit[12-15]: g3d写qos */
        unsigned int  ddr_qos_ar_ccpu : 4;  /* bit[16-19]: ccpu读qos */
        unsigned int  ddr_qos_aw_ccpu : 4;  /* bit[20-23]: ccpu写qos */
        unsigned int  reserved_2      : 4;  /* bit[24-27]: reserved */
        unsigned int  reserved_3      : 4;  /* bit[28-31]: DDR总线QOS优先级设置，可配置值为0～15，具体优先级高低由DDR寄存器AXI_QOSCFG配置
                                                           socp读qos */
    } reg;
} SOC_PERI_SCTRL_SC_DDR_CTRL1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_DDR_CTRL1_ddr_qos_ar_acpu_START  (0)
#define SOC_PERI_SCTRL_SC_DDR_CTRL1_ddr_qos_ar_acpu_END    (3)
#define SOC_PERI_SCTRL_SC_DDR_CTRL1_ddr_qos_aw_acpu_START  (4)
#define SOC_PERI_SCTRL_SC_DDR_CTRL1_ddr_qos_aw_acpu_END    (7)
#define SOC_PERI_SCTRL_SC_DDR_CTRL1_ddr_qos_ar_ccpu_START  (16)
#define SOC_PERI_SCTRL_SC_DDR_CTRL1_ddr_qos_ar_ccpu_END    (19)
#define SOC_PERI_SCTRL_SC_DDR_CTRL1_ddr_qos_aw_ccpu_START  (20)
#define SOC_PERI_SCTRL_SC_DDR_CTRL1_ddr_qos_aw_ccpu_END    (23)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_DDR_CTRL2_UNION
 结构说明  : SC_DDR_CTRL2 寄存器结构定义。地址偏移量:0x058，初值:0x00000000，宽度:32
 寄存器说明: DDR 配置寄存器2。功能包含DDR qos控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 4;  /* bit[0-3]  : acpu读hurry */
        unsigned int  reserved_1: 4;  /* bit[4-7]  : acpu读press */
        unsigned int  reserved_2: 4;  /* bit[8-11] : g3d读press */
        unsigned int  reserved_3: 4;  /* bit[12-15]: g3d读press */
        unsigned int  reserved_4: 4;  /* bit[16-19]: ccpu读hurry */
        unsigned int  reserved_5: 4;  /* bit[20-23]: ccpu读press */
        unsigned int  reserved_6: 8;  /* bit[24-31]: DDR总线QOS优先级设置，可配置值为0～15，具体优先级高低由DDR寄存器AXI_QOSCFG配置 */
    } reg;
} SOC_PERI_SCTRL_SC_DDR_CTRL2_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_DDR_CTRL3_UNION
 结构说明  : SC_DDR_CTRL3 寄存器结构定义。地址偏移量:0x060，初值:0x00000000，宽度:32
 寄存器说明: DDR 配置寄存器。功能包含DDR qos控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 4;  /* bit[0-3]  : acpu写hurry */
        unsigned int  reserved_1: 4;  /* bit[4-7]  : acpu写press */
        unsigned int  reserved_2: 4;  /* bit[8-11] : g3d写press */
        unsigned int  reserved_3: 4;  /* bit[12-15]: g3d写press */
        unsigned int  reserved_4: 4;  /* bit[16-19]: ccpu写hurry */
        unsigned int  reserved_5: 4;  /* bit[20-23]: ccpu写press */
        unsigned int  reserved_6: 4;  /* bit[24-27]: ipf写qos */
        unsigned int  reserved_7: 4;  /* bit[28-31]: DDR总线QOS优先级设置，可配置值为0～15，具体优先级高低由DDR寄存器AXI_QOSCFG配置
                                                     socp写qos */
    } reg;
} SOC_PERI_SCTRL_SC_DDR_CTRL3_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_DDR_CTRL4_UNION
 结构说明  : SC_DDR_CTRL4 寄存器结构定义。地址偏移量:0x064，初值:0x00000000，宽度:32
 寄存器说明: DDR 状态寄存器4。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 4;  /* bit[0-3]  : reserved */
        unsigned int  reserved_1: 4;  /* bit[4-7]  : reserved */
        unsigned int  reserved_2: 4;  /* bit[8-11] : reserved */
        unsigned int  reserved_3: 20; /* bit[12-31]: DDR总线QOS优先级设置，可配置值为0～15，具体优先级高低由DDR寄存器AXI_QOSCFG配置 */
    } reg;
} SOC_PERI_SCTRL_SC_DDR_CTRL4_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_NOC_CTRL0_UNION
 结构说明  : SC_NOC_CTRL0 寄存器结构定义。地址偏移量:0x070，初值:0x00000001，宽度:32
 寄存器说明: NOC 状态寄存器0，包括sysnoc timeout 使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sysnoc_timeout_en : 1;  /* bit[0]   : systemnoc_timeout_en
                                                            0：disable timeout
                                                            1：enable timeout */
        unsigned int  reserved          : 31; /* bit[1-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_NOC_CTRL0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_NOC_CTRL0_sysnoc_timeout_en_START  (0)
#define SOC_PERI_SCTRL_SC_NOC_CTRL0_sysnoc_timeout_en_END    (0)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_NOC_CTRL1_UNION
 结构说明  : SC_NOC_CTRL1 寄存器结构定义。地址偏移量:0x074，初值:0x00000000，宽度:32
 寄存器说明: NOC 状态寄存器1，包括clkbus_low时钟选择使能和滤毛刺寄存器配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clkbus_low_nopending_en      : 1;  /* bit[0]    : NOC nopending信号作为时钟选择功能使能：
                                                                        0：不使能该功能
                                                                        1：使能该功能 */
        unsigned int  reserved_0                   : 3;  /* bit[1-3]  : reserved */
        unsigned int  clkbus_low_debounce_num_high : 10; /* bit[4-13] : nopending信号在高电平时滤除低电平毛刺计数配置（num+1=最大被滤毛刺周期数）(时钟为100M） */
        unsigned int  reserved_1                   : 2;  /* bit[14-15]: reserved */
        unsigned int  clkbus_low_debounce_num_low  : 10; /* bit[16-25]: nopending信号在低电平时滤除高电平毛刺计数配置（num+1=最大被滤毛刺周期数）(时钟为100M） */
        unsigned int  reserved_2                   : 6;  /* bit[26-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_NOC_CTRL1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_NOC_CTRL1_clkbus_low_nopending_en_START       (0)
#define SOC_PERI_SCTRL_SC_NOC_CTRL1_clkbus_low_nopending_en_END         (0)
#define SOC_PERI_SCTRL_SC_NOC_CTRL1_clkbus_low_debounce_num_high_START  (4)
#define SOC_PERI_SCTRL_SC_NOC_CTRL1_clkbus_low_debounce_num_high_END    (13)
#define SOC_PERI_SCTRL_SC_NOC_CTRL1_clkbus_low_debounce_num_low_START   (16)
#define SOC_PERI_SCTRL_SC_NOC_CTRL1_clkbus_low_debounce_num_low_END     (25)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_NOC_CTRL2_UNION
 结构说明  : SC_NOC_CTRL2 寄存器结构定义。地址偏移量:0x078，初值:0x00000000，宽度:32
 寄存器说明: NOC 状态寄存器2，包括clkbus_high时钟选择使能和滤毛刺寄存器配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clkbus_high_nopending_en      : 1;  /* bit[0]    : NOC nopending信号作为时钟选择功能使能：
                                                                         0：不使能该功能
                                                                         1：使能该功能 */
        unsigned int  reserved_0                    : 3;  /* bit[1-3]  : reserved */
        unsigned int  clkbus_high_debounce_num_high : 10; /* bit[4-13] : nopending信号在高电平时滤除低电平毛刺计数配置（num+1=最大被滤毛刺周期数）(时钟为150M） */
        unsigned int  reserved_1                    : 2;  /* bit[14-15]: reserved */
        unsigned int  clkbus_high_debounce_num_low  : 10; /* bit[16-25]: nopending信号在低电平时滤除高电平毛刺计数配置（num+1=最大被滤毛刺周期数）(时钟为150M） */
        unsigned int  reserved_2                    : 6;  /* bit[26-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_NOC_CTRL2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_NOC_CTRL2_clkbus_high_nopending_en_START       (0)
#define SOC_PERI_SCTRL_SC_NOC_CTRL2_clkbus_high_nopending_en_END         (0)
#define SOC_PERI_SCTRL_SC_NOC_CTRL2_clkbus_high_debounce_num_high_START  (4)
#define SOC_PERI_SCTRL_SC_NOC_CTRL2_clkbus_high_debounce_num_high_END    (13)
#define SOC_PERI_SCTRL_SC_NOC_CTRL2_clkbus_high_debounce_num_low_START   (16)
#define SOC_PERI_SCTRL_SC_NOC_CTRL2_clkbus_high_debounce_num_low_END     (25)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_STAT0_UNION
 结构说明  : SC_PERIPH_STAT0 寄存器结构定义。地址偏移量:0x090，初值:0x00000000，宽度:32
 寄存器说明: 外设状态寄存器0。功能包含CSSYS ETR、SOCP、IPF、hifi状态、noc nopending信号等
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_state0_ipf_idle   : 1;  /* bit[0]   : ipf_idle
                                                                   0：空闲状态；
                                                                   1：工作状态。 */
        unsigned int  periph_state0_socp_idle  : 1;  /* bit[1]   : socp_idle
                                                                   0：空闲状态；
                                                                   1：工作状态。 */
        unsigned int  etr_axi_csysack_n        : 1;  /* bit[2]   : ETR的axi接口的csysreq/csysack在下电前排空操作
                                                                   1'b0：请求排空操作
                                                                   1'b1：不请求排空操作 */
        unsigned int  hifi_xocdmode            : 1;  /* bit[3]   : 处理器OCDhaltmode状态指示信号，高有效。
                                                                   0：未处于haltmode；
                                                                   1：处于haltmode。 */
        unsigned int  hifi_pwaitmode           : 1;  /* bit[4]   : 处理器sleepmode状态指示信号，高有效。指示当前处理器通过执行WAITI指令进入sleepmode（clockgating）并等待未屏蔽中断唤醒。
                                                                   0：未进入sleepmode；
                                                                   1：进入sleepmode。 */
        unsigned int  noc_nopending_raw        : 1;  /* bit[5]   : 原始noc nopending 信号指示 */
        unsigned int  clkbus_low_debounce_aft  : 1;  /* bit[6]   : clkbus low nopending去抖信号 */
        unsigned int  clkbus_high_debounce_aft : 1;  /* bit[7]   : clkbus high nopending去抖信号 */
        unsigned int  reserved                 : 24; /* bit[8-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_STAT0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_periph_state0_ipf_idle_START    (0)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_periph_state0_ipf_idle_END      (0)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_periph_state0_socp_idle_START   (1)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_periph_state0_socp_idle_END     (1)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_etr_axi_csysack_n_START         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_etr_axi_csysack_n_END           (2)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_hifi_xocdmode_START             (3)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_hifi_xocdmode_END               (3)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_hifi_pwaitmode_START            (4)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_hifi_pwaitmode_END              (4)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_noc_nopending_raw_START         (5)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_noc_nopending_raw_END           (5)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_clkbus_low_debounce_aft_START   (6)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_clkbus_low_debounce_aft_END     (6)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_clkbus_high_debounce_aft_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT0_clkbus_high_debounce_aft_END    (7)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_STAT1_UNION
 结构说明  : SC_PERIPH_STAT1 寄存器结构定义。地址偏移量:0x094，初值:0x00000000，宽度:32
 寄存器说明: periph stat1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  fm_state : 32; /* bit[0-31]: 时钟频率检测值 （单位：Hz) */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_STAT1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_STAT1_fm_state_START  (0)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT1_fm_state_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_STAT2_UNION
 结构说明  : SC_PERIPH_STAT2 寄存器结构定义。地址偏移量:0x098，初值:0x00000000，宽度:32
 寄存器说明: usb状态寄存器2。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  testdataout_picophy : 4;  /* bit[0-3]  : testdataout_picophy：在测试接口起作用的情况下，该值仅在sc_periph_ctrl9[1]为0的情况下，取决于sc_periph_ctrl9[7：4]的值。
                                                               periph_ctrl9[7：4]= 4'h1000 and periph_ctrl9[1] = 1'b0 
                                                               testdataout[3] BIST Enable Single-Ended Rx D– Output
                                                               testdataout[2] HS BIST Single-Ended Rx D+ Output
                                                               testdataout[1] FS BIST HS Differential Rx Data
                                                               testdataout[0] LS BIST FS Differential Rx  */
        unsigned int  otg_wkp_sts         : 1;  /* bit[4]    : otg_wkp_sts
                                                               中断唤醒状态，表示系统收到唤醒中断
                                                               0：无唤醒中断
                                                               1：收到唤醒中断 */
        unsigned int  chargedet_picophy   : 1;  /* bit[5]    : 充电检测指示：
                                                               1：状态有效
                                                               0：无效 */
        unsigned int  datcondet_sts       : 1;  /* bit[6]    : datcondet_sts
                                                               USB数据管脚接触检测，当时能充电检测功能时有效。
                                                               0：无管脚连接
                                                               1：表示数据管脚连接检测到。 */
        unsigned int  chgdet_sts          : 1;  /* bit[7]    : chgdet_sts
                                                               充电器检测，该信号为稳定20ms后的信号。仅在充电检测有效时有效
                                                               0：SDP
                                                               1：检测到充电器为DCP或CDP */
        unsigned int  chgdet_async        : 1;  /* bit[8]    : chgdet_async
                                                               充电器检测。为异步信号，与chg_det同源，但没有经过20ms延时以及32KHz始终的锁存。意义同chgdet_sts */
        unsigned int  fsvplus0_picophy    : 1;  /* bit[9]    : D+电平状态指示：
                                                               1：电平为高
                                                               0：电平为低 */
        unsigned int  fsvminus0_picophy   : 1;  /* bit[10]   : D-电平状态指示：
                                                               1：电平为高
                                                               0：电平为低 */
        unsigned int  reserved            : 21; /* bit[11-31]: [31：11]：保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_STAT2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_testdataout_picophy_START  (0)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_testdataout_picophy_END    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_otg_wkp_sts_START          (4)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_otg_wkp_sts_END            (4)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_chargedet_picophy_START    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_chargedet_picophy_END      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_datcondet_sts_START        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_datcondet_sts_END          (6)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_chgdet_sts_START           (7)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_chgdet_sts_END             (7)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_chgdet_async_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_chgdet_async_END           (8)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_fsvplus0_picophy_START     (9)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_fsvplus0_picophy_END       (9)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_fsvminus0_picophy_START    (10)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT2_fsvminus0_picophy_END      (10)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_STAT3_UNION
 结构说明  : SC_PERIPH_STAT3 寄存器结构定义。地址偏移量:0x09C，初值:0x00000000，宽度:32
 寄存器说明: hifi组合中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hifi_all_raw_int : 1;  /* bit[0]   : 送给mcu hifi组合原始中断 */
        unsigned int  hifi_all_msk_int : 1;  /* bit[1]   : 送给mcu hifi组合屏蔽后中断 */
        unsigned int  reserved         : 30; /* bit[2-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_STAT3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_STAT3_hifi_all_raw_int_START  (0)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT3_hifi_all_raw_int_END    (0)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT3_hifi_all_msk_int_START  (1)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT3_hifi_all_msk_int_END    (1)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_STAT4_UNION
 结构说明  : SC_PERIPH_STAT4 寄存器结构定义。地址偏移量:0x0A0，初值:0x00000000，宽度:32
 寄存器说明: 外设状态寄存器4。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  usb_nopendingtrans         : 1;  /* bit[0]    : noc usb no pending状态 */
        unsigned int  socp_nopendingtrans        : 1;  /* bit[1]    : noc socp no pending状态 */
        unsigned int  service_nopendingtrans     : 1;  /* bit[2]    : service no pending状态 */
        unsigned int  service_dma_nopendingtrans : 1;  /* bit[3]    : service_dma no pending状态 */
        unsigned int  service_ao_nopendingtrans  : 1;  /* bit[4]    : service ao no pending状态 */
        unsigned int  seceng_nopendingtrans      : 1;  /* bit[5]    : seceng no pending状态 */
        unsigned int  modem_nopendingtrans       : 1;  /* bit[6]    : modem no pending状态 */
        unsigned int  mmc2_nopendingtrans        : 1;  /* bit[7]    : mmc2 no pending状态 */
        unsigned int  mmc1_nopendingtrans        : 1;  /* bit[8]    : mmc1 no pending状态 */
        unsigned int  mmc0_nopendingtrans        : 1;  /* bit[9]    : mmc0 no pending状态 */
        unsigned int  mcu_nopendingtrans         : 1;  /* bit[10]   : mcu no pending状态 */
        unsigned int  ipf_nopendingtrans         : 1;  /* bit[11]   : ipf no pending状态 */
        unsigned int  hifi_nopendingtrans        : 1;  /* bit[12]   : hifi no pending状态 */
        unsigned int  dmac_nopendingtrans        : 1;  /* bit[13]   : dmac no pending状态 */
        unsigned int  dap_nopendingtrans         : 1;  /* bit[14]   : dap no pending状态 */
        unsigned int  dap_apb_nopendingtrans     : 1;  /* bit[15]   : dap_apb no pending状态 */
        unsigned int  cssys_nopendingtrans       : 1;  /* bit[16]   : cssys no pending状态 */
        unsigned int  acpu_nopendingtrans        : 1;  /* bit[17]   : acpu no pending状态 */
        unsigned int  reserved                   : 14; /* bit[18-31]: 
                                                                      保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_STAT4_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_usb_nopendingtrans_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_usb_nopendingtrans_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_socp_nopendingtrans_START         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_socp_nopendingtrans_END           (1)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_service_nopendingtrans_START      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_service_nopendingtrans_END        (2)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_service_dma_nopendingtrans_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_service_dma_nopendingtrans_END    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_service_ao_nopendingtrans_START   (4)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_service_ao_nopendingtrans_END     (4)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_seceng_nopendingtrans_START       (5)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_seceng_nopendingtrans_END         (5)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_modem_nopendingtrans_START        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_modem_nopendingtrans_END          (6)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_mmc2_nopendingtrans_START         (7)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_mmc2_nopendingtrans_END           (7)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_mmc1_nopendingtrans_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_mmc1_nopendingtrans_END           (8)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_mmc0_nopendingtrans_START         (9)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_mmc0_nopendingtrans_END           (9)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_mcu_nopendingtrans_START          (10)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_mcu_nopendingtrans_END            (10)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_ipf_nopendingtrans_START          (11)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_ipf_nopendingtrans_END            (11)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_hifi_nopendingtrans_START         (12)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_hifi_nopendingtrans_END           (12)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_dmac_nopendingtrans_START         (13)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_dmac_nopendingtrans_END           (13)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_dap_nopendingtrans_START          (14)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_dap_nopendingtrans_END            (14)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_dap_apb_nopendingtrans_START      (15)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_dap_apb_nopendingtrans_END        (15)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_cssys_nopendingtrans_START        (16)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_cssys_nopendingtrans_END          (16)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_acpu_nopendingtrans_START         (17)
#define SOC_PERI_SCTRL_SC_PERIPH_STAT4_acpu_nopendingtrans_END           (17)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_STAT6_UNION
 结构说明  : SC_PERIPH_STAT6 寄存器结构定义。地址偏移量:0x0A4，初值:0x00000000，宽度:32
 寄存器说明: 外设状态寄存器6。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_STAT6_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_UNION
 结构说明  : SC_HIFI_INT_STATR0 寄存器结构定义。地址偏移量:0x0B0，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_hifi_int_statr0_ns_ipc2hifi_intr0   : 1;  /* bit[0]    : ns_ipc2hifi_intr0中断 */
        unsigned int  sc_hifi_int_statr0_ns_ipc2hifi_intr1   : 1;  /* bit[1]    : ns_ipc2hifi_intr1中断 */
        unsigned int  sc_hifi_int_statr0_s_ipc2hifi_intr0    : 1;  /* bit[2]    : s_ipc2hifi_intr0中断 */
        unsigned int  sc_hifi_int_statr0_s_ipc2hifi_intr1    : 1;  /* bit[3]    : s_ipc2hifi_intr1中断 */
        unsigned int  sc_hifi_int_statr0_timer10_intr        : 1;  /* bit[4]    : timer10_intr中断 */
        unsigned int  sc_hifi_int_statr0_timer11_intr        : 1;  /* bit[5]    : timer11_intr中断 */
        unsigned int  sc_hifi_int_statr0_timer12_intr        : 1;  /* bit[6]    : timer12_intr中断 */
        unsigned int  sc_hifi_int_statr0_timer13_intr        : 1;  /* bit[7]    : timer13_intr中断 */
        unsigned int  reserved_0                             : 1;  /* bit[8]    : reserved */
        unsigned int  reserved_1                             : 1;  /* bit[9]    : reserved */
        unsigned int  reserved_2                             : 1;  /* bit[10]   : reserved */
        unsigned int  reserved_3                             : 1;  /* bit[11]   : reserved */
        unsigned int  sc_hifi_int_statr0_digacodec_intr      : 1;  /* bit[12]   : digacodec_intr中断 */
        unsigned int  sc_hifi_int_statr0_aedmac3_s_intr      : 1;  /* bit[13]   : aedmac3_s_intr中断 */
        unsigned int  sc_hifi_int_statr0_aedmac3_ns_intr     : 1;  /* bit[14]   : aedmac3_ns_intr中断 */
        unsigned int  reserved_4                             : 1;  /* bit[15]   : reserved */
        unsigned int  sc_hifi_int_statr0_uart0_2_4_comb_intr : 1;  /* bit[16]   : uart0_2_4_comb_intr中断 */
        unsigned int  sc_hifi_int_statr0_wdog1_intr          : 1;  /* bit[17]   : wdog1_intr中断 */
        unsigned int  reserved_5                             : 1;  /* bit[18]   : reserved */
        unsigned int  sc_hifi_int_statr0_seceng3_intr        : 1;  /* bit[19]   : seceng3_intr中断 */
        unsigned int  sc_hifi_int_statr0_ctu_lte2dsp_intr    : 1;  /* bit[20]   : ctu_lte2dsp_intr中断 */
        unsigned int  sc_hifi_int_statr0_ctu_tds2dsp_intr    : 1;  /* bit[21]   : ctu_tds2dsp_intr中断 */
        unsigned int  sc_hifi_int_statr0_hifi_vote_intr      : 1;  /* bit[22]   : hifi_vote_intr中断 */
        unsigned int  sc_hifi_int_statr0_gpio11_intr0        : 1;  /* bit[23]   : gpio11_intr0中断 */
        unsigned int  sc_hifi_int_statr0_tds_stu_hifi_int    : 1;  /* bit[24]   : tds_stu_hifi_int中断 */
        unsigned int  reserved_6                             : 7;  /* bit[25-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_ns_ipc2hifi_intr0_START    (0)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_ns_ipc2hifi_intr0_END      (0)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_ns_ipc2hifi_intr1_START    (1)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_ns_ipc2hifi_intr1_END      (1)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_s_ipc2hifi_intr0_START     (2)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_s_ipc2hifi_intr0_END       (2)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_s_ipc2hifi_intr1_START     (3)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_s_ipc2hifi_intr1_END       (3)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_timer10_intr_START         (4)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_timer10_intr_END           (4)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_timer11_intr_START         (5)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_timer11_intr_END           (5)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_timer12_intr_START         (6)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_timer12_intr_END           (6)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_timer13_intr_START         (7)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_timer13_intr_END           (7)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_digacodec_intr_START       (12)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_digacodec_intr_END         (12)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_aedmac3_s_intr_START       (13)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_aedmac3_s_intr_END         (13)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_aedmac3_ns_intr_START      (14)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_aedmac3_ns_intr_END        (14)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_uart0_2_4_comb_intr_START  (16)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_uart0_2_4_comb_intr_END    (16)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_wdog1_intr_START           (17)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_wdog1_intr_END             (17)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_seceng3_intr_START         (19)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_seceng3_intr_END           (19)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_ctu_lte2dsp_intr_START     (20)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_ctu_lte2dsp_intr_END       (20)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_ctu_tds2dsp_intr_START     (21)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_ctu_tds2dsp_intr_END       (21)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_hifi_vote_intr_START       (22)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_hifi_vote_intr_END         (22)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_gpio11_intr0_START         (23)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_gpio11_intr0_END           (23)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_tds_stu_hifi_int_START     (24)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATR0_sc_hifi_int_statr0_tds_stu_hifi_int_END       (24)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_UNION
 结构说明  : SC_HIFI_INT_STATM0 寄存器结构定义。地址偏移量:0x0B4，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_hifi_int_statm0_ns_ipc2hifi_intr0   : 1;  /* bit[0]    : ns_ipc2hifi_intr0中断 */
        unsigned int  sc_hifi_int_statm0_ns_ipc2hifi_intr1   : 1;  /* bit[1]    : ns_ipc2hifi_intr1中断 */
        unsigned int  sc_hifi_int_statm0_s_ipc2hifi_intr0    : 1;  /* bit[2]    : s_ipc2hifi_intr0中断 */
        unsigned int  sc_hifi_int_statm0_s_ipc2hifi_intr1    : 1;  /* bit[3]    : s_ipc2hifi_intr1中断 */
        unsigned int  sc_hifi_int_statm0_timer10_intr        : 1;  /* bit[4]    : timer10_intr中断 */
        unsigned int  sc_hifi_int_statm0_timer11_intr        : 1;  /* bit[5]    : timer11_intr中断 */
        unsigned int  sc_hifi_int_statm0_timer12_intr        : 1;  /* bit[6]    : timer12_intr中断 */
        unsigned int  sc_hifi_int_statm0_timer13_intr        : 1;  /* bit[7]    : timer13_intr中断 */
        unsigned int  reserved_0                             : 1;  /* bit[8]    : reserved */
        unsigned int  reserved_1                             : 1;  /* bit[9]    : reserved */
        unsigned int  reserved_2                             : 1;  /* bit[10]   : reserved */
        unsigned int  reserved_3                             : 1;  /* bit[11]   : reserved */
        unsigned int  sc_hifi_int_statm0_digacodec_intr      : 1;  /* bit[12]   : digacodec_intr中断 */
        unsigned int  sc_hifi_int_statm0_aedmac3_s_intr      : 1;  /* bit[13]   : aedmac3_s_intr中断 */
        unsigned int  sc_hifi_int_statm0_aedmac3_ns_intr     : 1;  /* bit[14]   : aedmac3_ns_intr中断 */
        unsigned int  reserved_4                             : 1;  /* bit[15]   : reserved */
        unsigned int  sc_hifi_int_statm0_uart0_2_4_comb_intr : 1;  /* bit[16]   : uart0_2_4_comb_intr中断 */
        unsigned int  sc_hifi_int_statm0_wdog1_intr          : 1;  /* bit[17]   : wdog1_intr中断 */
        unsigned int  reserved_5                             : 1;  /* bit[18]   : reserved */
        unsigned int  sc_hifi_int_statm0_seceng3_intr        : 1;  /* bit[19]   : seceng3_intr中断 */
        unsigned int  sc_hifi_int_statm0_ctu_lte2dsp_intr    : 1;  /* bit[20]   : ctu_lte2dsp_intr中断 */
        unsigned int  sc_hifi_int_statm0_ctu_tds2dsp_intr    : 1;  /* bit[21]   : ctu_tds2dsp_intr中断 */
        unsigned int  sc_hifi_int_statm0_hifi_vote_intr      : 1;  /* bit[22]   : hifi_vote_intr中断 */
        unsigned int  sc_hifi_int_statm0_gpio11_intr0        : 1;  /* bit[23]   : gpio11_intr0中断 */
        unsigned int  sc_hifi_int_statm0_tds_stu_hifi_int    : 1;  /* bit[24]   : tds_stu_hifi_int中断 */
        unsigned int  reserved_6                             : 7;  /* bit[25-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_ns_ipc2hifi_intr0_START    (0)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_ns_ipc2hifi_intr0_END      (0)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_ns_ipc2hifi_intr1_START    (1)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_ns_ipc2hifi_intr1_END      (1)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_s_ipc2hifi_intr0_START     (2)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_s_ipc2hifi_intr0_END       (2)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_s_ipc2hifi_intr1_START     (3)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_s_ipc2hifi_intr1_END       (3)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_timer10_intr_START         (4)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_timer10_intr_END           (4)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_timer11_intr_START         (5)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_timer11_intr_END           (5)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_timer12_intr_START         (6)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_timer12_intr_END           (6)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_timer13_intr_START         (7)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_timer13_intr_END           (7)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_digacodec_intr_START       (12)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_digacodec_intr_END         (12)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_aedmac3_s_intr_START       (13)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_aedmac3_s_intr_END         (13)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_aedmac3_ns_intr_START      (14)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_aedmac3_ns_intr_END        (14)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_uart0_2_4_comb_intr_START  (16)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_uart0_2_4_comb_intr_END    (16)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_wdog1_intr_START           (17)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_wdog1_intr_END             (17)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_seceng3_intr_START         (19)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_seceng3_intr_END           (19)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_ctu_lte2dsp_intr_START     (20)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_ctu_lte2dsp_intr_END       (20)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_ctu_tds2dsp_intr_START     (21)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_ctu_tds2dsp_intr_END       (21)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_hifi_vote_intr_START       (22)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_hifi_vote_intr_END         (22)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_gpio11_intr0_START         (23)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_gpio11_intr0_END           (23)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_tds_stu_hifi_int_START     (24)
#define SOC_PERI_SCTRL_SC_HIFI_INT_STATM0_sc_hifi_int_statm0_tds_stu_hifi_int_END       (24)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_UNION
 结构说明  : SC_PERI_2ND_INT_EN0 寄存器结构定义。地址偏移量:0x100，初值:0xFFFFFFFF，宽度:32
 寄存器说明: ACPU唤醒中断二级中断使能信号
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_peri_2nd_int_en0_gpio4_intr0        : 1;  /* bit[0]    : gpio4 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio5_intr0        : 1;  /* bit[1]    : gpio5 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio6_intr0        : 1;  /* bit[2]    : gpio6 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio7_intr0        : 1;  /* bit[3]    : gpio7 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio8_intr0        : 1;  /* bit[4]    : gpio8 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio9_intr0        : 1;  /* bit[5]    : gpio9 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio10_intr0       : 1;  /* bit[6]    : gpio10 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio11_intr0       : 1;  /* bit[7]    : gpio11 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio12_intr0       : 1;  /* bit[8]    : gpio12 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio13_intr0       : 1;  /* bit[9]    : gpio13 中断 */
        unsigned int  sc_peri_2nd_int_en0_ipf_intr0          : 1;  /* bit[10]   : ipf 中断0 */
        unsigned int  sc_peri_2nd_int_en0_ipf_intr1          : 1;  /* bit[11]   : ipf 中断1 */
        unsigned int  sc_peri_2nd_int_en0_socp_intr0         : 1;  /* bit[12]   : socp 中断0 */
        unsigned int  sc_peri_2nd_int_en0_socp_intr1         : 1;  /* bit[13]   : socp 中断1 */
        unsigned int  sc_peri_2nd_int_en0_gpio14_intr0       : 1;  /* bit[14]   : gpio14 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio15_intr0       : 1;  /* bit[15]   : gpio15 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio16_intr0       : 1;  /* bit[16]   : gpio16 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio17_intr0       : 1;  /* bit[17]   : gpio17 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio18_intr0       : 1;  /* bit[18]   : gpio18 中断 */
        unsigned int  sc_peri_2nd_int_en0_gpio19_intr0       : 1;  /* bit[19]   : gpio19 中断 */
        unsigned int  sc_peri_2nd_int_en0_sys_noc_err_intr   : 1;  /* bit[20]   : sysnoc 错误上报中断 */
        unsigned int  sc_peri_2nd_int_en0_media_noc_err_intr : 1;  /* bit[21]   : medianoc 错误上报中断 */
        unsigned int  sc_peri_2nd_int_en0_ddrc_sec_intr      : 1;  /* bit[22]   : ddrc安全中断 */
        unsigned int  sc_peri_2nd_int_en0_acpu_soft_fiq_intr : 1;  /* bit[23]   : acpu软fiq */
        unsigned int  reserved                               : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio4_intr0_START         (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio4_intr0_END           (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio5_intr0_START         (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio5_intr0_END           (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio6_intr0_START         (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio6_intr0_END           (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio7_intr0_START         (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio7_intr0_END           (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio8_intr0_START         (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio8_intr0_END           (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio9_intr0_START         (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio9_intr0_END           (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio10_intr0_START        (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio10_intr0_END          (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio11_intr0_START        (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio11_intr0_END          (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio12_intr0_START        (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio12_intr0_END          (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio13_intr0_START        (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio13_intr0_END          (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_ipf_intr0_START           (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_ipf_intr0_END             (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_ipf_intr1_START           (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_ipf_intr1_END             (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_socp_intr0_START          (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_socp_intr0_END            (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_socp_intr1_START          (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_socp_intr1_END            (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio14_intr0_START        (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio14_intr0_END          (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio15_intr0_START        (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio15_intr0_END          (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio16_intr0_START        (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio16_intr0_END          (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio17_intr0_START        (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio17_intr0_END          (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio18_intr0_START        (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio18_intr0_END          (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio19_intr0_START        (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_gpio19_intr0_END          (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_sys_noc_err_intr_START    (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_sys_noc_err_intr_END      (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_media_noc_err_intr_START  (21)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_media_noc_err_intr_END    (21)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_ddrc_sec_intr_START       (22)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_ddrc_sec_intr_END         (22)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_acpu_soft_fiq_intr_START  (23)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN0_sc_peri_2nd_int_en0_acpu_soft_fiq_intr_END    (23)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_UNION
 结构说明  : SC_PERI_2ND_INT_STATR0 寄存器结构定义。地址偏移量:0x104，初值:0x00000000，宽度:32
 寄存器说明: ACPU唤醒中断二级中断原始中断状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_peri_2nd_int_statr0_gpio4_intr0        : 1;  /* bit[0]    : gpio4 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio5_intr0        : 1;  /* bit[1]    : gpio5 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio6_intr0        : 1;  /* bit[2]    : gpio6 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio7_intr0        : 1;  /* bit[3]    : gpio7 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio8_intr0        : 1;  /* bit[4]    : gpio8 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio9_intr0        : 1;  /* bit[5]    : gpio9 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio10_intr0       : 1;  /* bit[6]    : gpio10 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio11_intr0       : 1;  /* bit[7]    : gpio11 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio12_intr0       : 1;  /* bit[8]    : gpio12 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio13_intr0       : 1;  /* bit[9]    : gpio13 中断 */
        unsigned int  sc_peri_2nd_int_statr0_ipf_intr0          : 1;  /* bit[10]   : ipf 中断0 */
        unsigned int  sc_peri_2nd_int_statr0_ipf_intr1          : 1;  /* bit[11]   : ipf 中断1 */
        unsigned int  sc_peri_2nd_int_statr0_socp_intr0         : 1;  /* bit[12]   : socp 中断0 */
        unsigned int  sc_peri_2nd_int_statr0_socp_intr1         : 1;  /* bit[13]   : socp 中断1 */
        unsigned int  sc_peri_2nd_int_statr0_gpio14_intr0       : 1;  /* bit[14]   : gpio14 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio15_intr0       : 1;  /* bit[15]   : gpio15 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio16_intr0       : 1;  /* bit[16]   : gpio16 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio17_intr0       : 1;  /* bit[17]   : gpio17 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio18_intr0       : 1;  /* bit[18]   : gpio18 中断 */
        unsigned int  sc_peri_2nd_int_statr0_gpio19_intr0       : 1;  /* bit[19]   : gpio19 中断 */
        unsigned int  sc_peri_2nd_int_statr0_sys_noc_err_intr   : 1;  /* bit[20]   : sysnoc 错误上报中断 */
        unsigned int  sc_peri_2nd_int_statr0_media_noc_err_intr : 1;  /* bit[21]   : medianoc 错误上报中断 */
        unsigned int  sc_peri_2nd_int_statr0_ddrc_sec_intr      : 1;  /* bit[22]   : ddrc安全中断 */
        unsigned int  sc_peri_2nd_int_statr0_acpu_soft_fiq_intr : 1;  /* bit[23]   : acpu软fiq */
        unsigned int  reserved                                  : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio4_intr0_START         (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio4_intr0_END           (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio5_intr0_START         (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio5_intr0_END           (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio6_intr0_START         (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio6_intr0_END           (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio7_intr0_START         (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio7_intr0_END           (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio8_intr0_START         (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio8_intr0_END           (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio9_intr0_START         (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio9_intr0_END           (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio10_intr0_START        (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio10_intr0_END          (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio11_intr0_START        (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio11_intr0_END          (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio12_intr0_START        (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio12_intr0_END          (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio13_intr0_START        (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio13_intr0_END          (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_ipf_intr0_START           (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_ipf_intr0_END             (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_ipf_intr1_START           (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_ipf_intr1_END             (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_socp_intr0_START          (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_socp_intr0_END            (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_socp_intr1_START          (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_socp_intr1_END            (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio14_intr0_START        (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio14_intr0_END          (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio15_intr0_START        (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio15_intr0_END          (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio16_intr0_START        (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio16_intr0_END          (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio17_intr0_START        (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio17_intr0_END          (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio18_intr0_START        (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio18_intr0_END          (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio19_intr0_START        (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_gpio19_intr0_END          (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_sys_noc_err_intr_START    (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_sys_noc_err_intr_END      (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_media_noc_err_intr_START  (21)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_media_noc_err_intr_END    (21)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_ddrc_sec_intr_START       (22)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_ddrc_sec_intr_END         (22)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_acpu_soft_fiq_intr_START  (23)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR0_sc_peri_2nd_int_statr0_acpu_soft_fiq_intr_END    (23)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_UNION
 结构说明  : SC_PERI_2ND_INT_STATM0 寄存器结构定义。地址偏移量:0x108，初值:0x00000000，宽度:32
 寄存器说明: ACPU唤醒中断二级中断屏蔽后中断状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_peri_2nd_int_statm0_gpio4_intr0        : 1;  /* bit[0]    : gpio4 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio5_intr0        : 1;  /* bit[1]    : gpio5 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio6_intr0        : 1;  /* bit[2]    : gpio6 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio7_intr0        : 1;  /* bit[3]    : gpio7 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio8_intr0        : 1;  /* bit[4]    : gpio8 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio9_intr0        : 1;  /* bit[5]    : gpio9 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio10_intr0       : 1;  /* bit[6]    : gpio10 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio11_intr0       : 1;  /* bit[7]    : gpio11 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio12_intr0       : 1;  /* bit[8]    : gpio12 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio13_intr0       : 1;  /* bit[9]    : gpio13 中断 */
        unsigned int  sc_peri_2nd_int_statm0_ipf_intr0          : 1;  /* bit[10]   : ipf 中断0 */
        unsigned int  sc_peri_2nd_int_statm0_ipf_intr1          : 1;  /* bit[11]   : ipf 中断1 */
        unsigned int  sc_peri_2nd_int_statm0_socp_intr0         : 1;  /* bit[12]   : socp 中断0 */
        unsigned int  sc_peri_2nd_int_statm0_socp_intr1         : 1;  /* bit[13]   : socp 中断1 */
        unsigned int  sc_peri_2nd_int_statm0_gpio14_intr0       : 1;  /* bit[14]   : gpio14 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio15_intr0       : 1;  /* bit[15]   : gpio15 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio16_intr0       : 1;  /* bit[16]   : gpio16 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio17_intr0       : 1;  /* bit[17]   : gpio17 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio18_intr0       : 1;  /* bit[18]   : gpio18 中断 */
        unsigned int  sc_peri_2nd_int_statm0_gpio19_intr0       : 1;  /* bit[19]   : gpio19 中断 */
        unsigned int  sc_peri_2nd_int_statm0_sys_noc_err_intr   : 1;  /* bit[20]   : sysnoc 错误上报中断 */
        unsigned int  sc_peri_2nd_int_statm0_media_noc_err_intr : 1;  /* bit[21]   : medianoc 错误上报中断 */
        unsigned int  sc_peri_2nd_int_statm0_ddrc_sec_intr      : 1;  /* bit[22]   : ddrc安全中断 */
        unsigned int  sc_peri_2nd_int_statm0_acpu_soft_fiq_intr : 1;  /* bit[23]   : acpu软fiq */
        unsigned int  reserved                                  : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio4_intr0_START         (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio4_intr0_END           (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio5_intr0_START         (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio5_intr0_END           (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio6_intr0_START         (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio6_intr0_END           (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio7_intr0_START         (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio7_intr0_END           (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio8_intr0_START         (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio8_intr0_END           (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio9_intr0_START         (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio9_intr0_END           (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio10_intr0_START        (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio10_intr0_END          (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio11_intr0_START        (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio11_intr0_END          (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio12_intr0_START        (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio12_intr0_END          (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio13_intr0_START        (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio13_intr0_END          (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_ipf_intr0_START           (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_ipf_intr0_END             (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_ipf_intr1_START           (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_ipf_intr1_END             (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_socp_intr0_START          (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_socp_intr0_END            (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_socp_intr1_START          (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_socp_intr1_END            (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio14_intr0_START        (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio14_intr0_END          (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio15_intr0_START        (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio15_intr0_END          (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio16_intr0_START        (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio16_intr0_END          (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio17_intr0_START        (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio17_intr0_END          (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio18_intr0_START        (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio18_intr0_END          (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio19_intr0_START        (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_gpio19_intr0_END          (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_sys_noc_err_intr_START    (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_sys_noc_err_intr_END      (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_media_noc_err_intr_START  (21)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_media_noc_err_intr_END    (21)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_ddrc_sec_intr_START       (22)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_ddrc_sec_intr_END         (22)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_acpu_soft_fiq_intr_START  (23)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM0_sc_peri_2nd_int_statm0_acpu_soft_fiq_intr_END    (23)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_UNION
 结构说明  : SC_PERI_2ND_INT_EN1 寄存器结构定义。地址偏移量:0x110，初值:0xFFFFFFFF，宽度:32
 寄存器说明: MCU中断二级中断使能信号
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_peri_2nd_int_en1_ddr_dfs_intr      : 1;  /* bit[0]    : ddr_dfs中断 */
        unsigned int  sc_peri_2nd_int_en1_g3d_lp_intr       : 1;  /* bit[1]    : g3d_lp中断 */
        unsigned int  sc_peri_2nd_int_en1_acpu_lp_intr      : 1;  /* bit[2]    : acpu_lp中断 */
        unsigned int  sc_peri_2nd_int_en1_ccpu_pmuirq_intr  : 1;  /* bit[3]    : ccpu_pumirq中断 */
        unsigned int  sc_peri_2nd_int_en1_acpu_pmuirq3_intr : 1;  /* bit[4]    : acpu_pmuirq3中断 */
        unsigned int  sc_peri_2nd_int_en1_acpu_pmuirq2_intr : 1;  /* bit[5]    : acpu_pmuirq2中断 */
        unsigned int  sc_peri_2nd_int_en1_acpu_pmuirq1_intr : 1;  /* bit[6]    : acpu_pmuirq1中断 */
        unsigned int  sc_peri_2nd_int_en1_acpu_pmuirq0_intr : 1;  /* bit[7]    : acpu_pmuirq0中断 */
        unsigned int  sc_peri_2nd_int_en1_sdiommc_intr      : 1;  /* bit[8]    : sdiommc中断 */
        unsigned int  sc_peri_2nd_int_en1_sdmmc_intr        : 1;  /* bit[9]    : sdmmc中断 */
        unsigned int  sc_peri_2nd_int_en1_emmc_intr         : 1;  /* bit[10]   : emmc中断 */
        unsigned int  sc_peri_2nd_int_en1_usb2otg_intr      : 1;  /* bit[11]   : usb2otg中断 */
        unsigned int  sc_peri_2nd_int_en1_g3d_intr          : 1;  /* bit[12]   : g3d中断 */
        unsigned int  sc_peri_2nd_int_en1_mipi_dsi_intr     : 1;  /* bit[13]   : mipi_dsi中断 */
        unsigned int  sc_peri_2nd_int_en1_ade_ldi_intr      : 1;  /* bit[14]   : ade_ldi中断 */
        unsigned int  sc_peri_2nd_int_en1_ade_intr          : 1;  /* bit[15]   : ade中断 */
        unsigned int  sc_peri_2nd_int_en1_mmu_intr          : 1;  /* bit[16]   : mmu中断 */
        unsigned int  sc_peri_2nd_int_en1_jpeg_intr         : 1;  /* bit[17]   : jpeg中断 */
        unsigned int  sc_peri_2nd_int_en1_vcodec_intr       : 1;  /* bit[18]   : vcodec中断 */
        unsigned int  sc_peri_2nd_int_en1_mipi_cs1_intr     : 1;  /* bit[19]   : mipi_cs1中断 */
        unsigned int  sc_peri_2nd_int_en1_mipi_cs0_intr     : 1;  /* bit[20]   : mipi_cs0中断 */
        unsigned int  sc_peri_2nd_int_en1_isp_intr          : 1;  /* bit[21]   : isp中断 */
        unsigned int  reserved                              : 10; /* bit[22-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_ddr_dfs_intr_START       (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_ddr_dfs_intr_END         (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_g3d_lp_intr_START        (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_g3d_lp_intr_END          (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_lp_intr_START       (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_lp_intr_END         (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_ccpu_pmuirq_intr_START   (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_ccpu_pmuirq_intr_END     (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_pmuirq3_intr_START  (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_pmuirq3_intr_END    (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_pmuirq2_intr_START  (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_pmuirq2_intr_END    (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_pmuirq1_intr_START  (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_pmuirq1_intr_END    (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_pmuirq0_intr_START  (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_acpu_pmuirq0_intr_END    (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_sdiommc_intr_START       (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_sdiommc_intr_END         (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_sdmmc_intr_START         (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_sdmmc_intr_END           (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_emmc_intr_START          (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_emmc_intr_END            (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_usb2otg_intr_START       (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_usb2otg_intr_END         (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_g3d_intr_START           (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_g3d_intr_END             (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_mipi_dsi_intr_START      (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_mipi_dsi_intr_END        (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_ade_ldi_intr_START       (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_ade_ldi_intr_END         (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_ade_intr_START           (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_ade_intr_END             (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_mmu_intr_START           (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_mmu_intr_END             (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_jpeg_intr_START          (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_jpeg_intr_END            (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_vcodec_intr_START        (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_vcodec_intr_END          (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_mipi_cs1_intr_START      (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_mipi_cs1_intr_END        (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_mipi_cs0_intr_START      (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_mipi_cs0_intr_END        (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_isp_intr_START           (21)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_EN1_sc_peri_2nd_int_en1_isp_intr_END             (21)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_UNION
 结构说明  : SC_PERI_2ND_INT_STATR1 寄存器结构定义。地址偏移量:0x114，初值:0x00000000，宽度:32
 寄存器说明: MCU中断二级中断原始中断状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_peri_2nd_int_statr1_ddr_dfs_intr      : 1;  /* bit[0]    : ddr_dfs中断 */
        unsigned int  sc_peri_2nd_int_statr1_g3d_lp_intr       : 1;  /* bit[1]    : g3d_lp中断 */
        unsigned int  sc_peri_2nd_int_statr1_acpu_lp_intr      : 1;  /* bit[2]    : acpu_lp中断 */
        unsigned int  sc_peri_2nd_int_statr1_ccpu_pmuirq_intr  : 1;  /* bit[3]    : ccpu_pumirq中断 */
        unsigned int  sc_peri_2nd_int_statr1_acpu_pmuirq3_intr : 1;  /* bit[4]    : acpu_pmuirq3中断 */
        unsigned int  sc_peri_2nd_int_statr1_acpu_pmuirq2_intr : 1;  /* bit[5]    : acpu_pmuirq2中断 */
        unsigned int  sc_peri_2nd_int_statr1_acpu_pmuirq1_intr : 1;  /* bit[6]    : acpu_pmuirq1中断 */
        unsigned int  sc_peri_2nd_int_statr1_acpu_pmuirq0_intr : 1;  /* bit[7]    : acpu_pmuirq0中断 */
        unsigned int  sc_peri_2nd_int_statr1_sdiommc_intr      : 1;  /* bit[8]    : sdiommc中断 */
        unsigned int  sc_peri_2nd_int_statr1_sdmmc_intr        : 1;  /* bit[9]    : sdmmc中断 */
        unsigned int  sc_peri_2nd_int_statr1_emmc_intr         : 1;  /* bit[10]   : emmc中断 */
        unsigned int  sc_peri_2nd_int_statr1_usb2otg_intr      : 1;  /* bit[11]   : usb2otg中断 */
        unsigned int  sc_peri_2nd_int_statr1_g3d_intr          : 1;  /* bit[12]   : g3d中断 */
        unsigned int  sc_peri_2nd_int_statr1_mipi_dsi_intr     : 1;  /* bit[13]   : mipi_dsi中断 */
        unsigned int  sc_peri_2nd_int_statr1_ade_ldi_intr      : 1;  /* bit[14]   : ade_ldi中断 */
        unsigned int  sc_peri_2nd_int_statr1_ade_intr          : 1;  /* bit[15]   : ade中断 */
        unsigned int  sc_peri_2nd_int_statr1_mmu_intr          : 1;  /* bit[16]   : mmu中断 */
        unsigned int  sc_peri_2nd_int_statr1_jpeg_intr         : 1;  /* bit[17]   : jpeg中断 */
        unsigned int  sc_peri_2nd_int_statr1_vcodec_intr       : 1;  /* bit[18]   : vcodec中断 */
        unsigned int  sc_peri_2nd_int_statr1_mipi_cs1_intr     : 1;  /* bit[19]   : mipi_cs1中断 */
        unsigned int  sc_peri_2nd_int_statr1_mipi_cs0_intr     : 1;  /* bit[20]   : mipi_cs0中断 */
        unsigned int  sc_peri_2nd_int_statr1_isp_intr          : 1;  /* bit[21]   : isp中断 */
        unsigned int  reserved                                 : 10; /* bit[22-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_ddr_dfs_intr_START       (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_ddr_dfs_intr_END         (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_g3d_lp_intr_START        (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_g3d_lp_intr_END          (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_lp_intr_START       (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_lp_intr_END         (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_ccpu_pmuirq_intr_START   (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_ccpu_pmuirq_intr_END     (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_pmuirq3_intr_START  (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_pmuirq3_intr_END    (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_pmuirq2_intr_START  (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_pmuirq2_intr_END    (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_pmuirq1_intr_START  (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_pmuirq1_intr_END    (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_pmuirq0_intr_START  (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_acpu_pmuirq0_intr_END    (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_sdiommc_intr_START       (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_sdiommc_intr_END         (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_sdmmc_intr_START         (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_sdmmc_intr_END           (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_emmc_intr_START          (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_emmc_intr_END            (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_usb2otg_intr_START       (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_usb2otg_intr_END         (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_g3d_intr_START           (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_g3d_intr_END             (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_mipi_dsi_intr_START      (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_mipi_dsi_intr_END        (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_ade_ldi_intr_START       (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_ade_ldi_intr_END         (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_ade_intr_START           (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_ade_intr_END             (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_mmu_intr_START           (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_mmu_intr_END             (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_jpeg_intr_START          (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_jpeg_intr_END            (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_vcodec_intr_START        (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_vcodec_intr_END          (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_mipi_cs1_intr_START      (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_mipi_cs1_intr_END        (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_mipi_cs0_intr_START      (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_mipi_cs0_intr_END        (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_isp_intr_START           (21)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATR1_sc_peri_2nd_int_statr1_isp_intr_END             (21)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_UNION
 结构说明  : SC_PERI_2ND_INT_STATM1 寄存器结构定义。地址偏移量:0x118，初值:0x00000000，宽度:32
 寄存器说明: MCU中断二级中断屏蔽后中断状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_peri_2nd_int_statm1_ddr_dfs_intr      : 1;  /* bit[0]    : ddr_dfs中断 */
        unsigned int  sc_peri_2nd_int_statm1_g3d_lp_intr       : 1;  /* bit[1]    : g3d_lp中断 */
        unsigned int  sc_peri_2nd_int_statm1_acpu_lp_intr      : 1;  /* bit[2]    : acpu_lp中断 */
        unsigned int  sc_peri_2nd_int_statm1_ccpu_pmuirq_intr  : 1;  /* bit[3]    : ccpu_pumirq中断 */
        unsigned int  sc_peri_2nd_int_statm1_acpu_pmuirq3_intr : 1;  /* bit[4]    : acpu_pmuirq3中断 */
        unsigned int  sc_peri_2nd_int_statm1_acpu_pmuirq2_intr : 1;  /* bit[5]    : acpu_pmuirq2中断 */
        unsigned int  sc_peri_2nd_int_statm1_acpu_pmuirq1_intr : 1;  /* bit[6]    : acpu_pmuirq1中断 */
        unsigned int  sc_peri_2nd_int_statm1_acpu_pmuirq0_intr : 1;  /* bit[7]    : acpu_pmuirq0中断 */
        unsigned int  sc_peri_2nd_int_statm1_sdiommc_intr      : 1;  /* bit[8]    : sdiommc中断 */
        unsigned int  sc_peri_2nd_int_statm1_sdmmc_intr        : 1;  /* bit[9]    : sdmmc中断 */
        unsigned int  sc_peri_2nd_int_statm1_emmc_intr         : 1;  /* bit[10]   : emmc中断 */
        unsigned int  sc_peri_2nd_int_statm1_usb2otg_intr      : 1;  /* bit[11]   : usb2otg中断 */
        unsigned int  sc_peri_2nd_int_statm1_g3d_intr          : 1;  /* bit[12]   : g3d中断 */
        unsigned int  sc_peri_2nd_int_statm1_mipi_dsi_intr     : 1;  /* bit[13]   : mipi_dsi中断 */
        unsigned int  sc_peri_2nd_int_statm1_ade_ldi_intr      : 1;  /* bit[14]   : ade_ldi中断 */
        unsigned int  sc_peri_2nd_int_statm1_ade_intr          : 1;  /* bit[15]   : ade中断 */
        unsigned int  sc_peri_2nd_int_statm1_mmu_intr          : 1;  /* bit[16]   : mmu中断 */
        unsigned int  sc_peri_2nd_int_statm1_jpeg_intr         : 1;  /* bit[17]   : jpeg中断 */
        unsigned int  sc_peri_2nd_int_statm1_vcodec_intr       : 1;  /* bit[18]   : vcodec中断 */
        unsigned int  sc_peri_2nd_int_statm1_mipi_cs1_intr     : 1;  /* bit[19]   : mipi_cs1中断 */
        unsigned int  sc_peri_2nd_int_statm1_mipi_cs0_intr     : 1;  /* bit[20]   : mipi_cs0中断 */
        unsigned int  sc_peri_2nd_int_statm1_isp_intr          : 1;  /* bit[21]   : isp中断 */
        unsigned int  reserved                                 : 10; /* bit[22-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_ddr_dfs_intr_START       (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_ddr_dfs_intr_END         (0)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_g3d_lp_intr_START        (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_g3d_lp_intr_END          (1)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_lp_intr_START       (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_lp_intr_END         (2)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_ccpu_pmuirq_intr_START   (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_ccpu_pmuirq_intr_END     (3)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_pmuirq3_intr_START  (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_pmuirq3_intr_END    (4)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_pmuirq2_intr_START  (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_pmuirq2_intr_END    (5)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_pmuirq1_intr_START  (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_pmuirq1_intr_END    (6)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_pmuirq0_intr_START  (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_acpu_pmuirq0_intr_END    (7)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_sdiommc_intr_START       (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_sdiommc_intr_END         (8)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_sdmmc_intr_START         (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_sdmmc_intr_END           (9)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_emmc_intr_START          (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_emmc_intr_END            (10)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_usb2otg_intr_START       (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_usb2otg_intr_END         (11)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_g3d_intr_START           (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_g3d_intr_END             (12)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_mipi_dsi_intr_START      (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_mipi_dsi_intr_END        (13)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_ade_ldi_intr_START       (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_ade_ldi_intr_END         (14)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_ade_intr_START           (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_ade_intr_END             (15)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_mmu_intr_START           (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_mmu_intr_END             (16)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_jpeg_intr_START          (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_jpeg_intr_END            (17)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_vcodec_intr_START        (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_vcodec_intr_END          (18)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_mipi_cs1_intr_START      (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_mipi_cs1_intr_END        (19)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_mipi_cs0_intr_START      (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_mipi_cs0_intr_END        (20)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_isp_intr_START           (21)
#define SOC_PERI_SCTRL_SC_PERI_2ND_INT_STATM1_sc_peri_2nd_int_statm1_isp_intr_END             (21)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_UNION
 结构说明  : SC_PERIPH_CLKEN0 寄存器结构定义。地址偏移量:0x200，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟使能寄存器0。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken0_mmc0         : 1;  /* bit[0]   : mmc0时钟 */
        unsigned int  periph_clken0_mmc1         : 1;  /* bit[1]   : mmc1时钟 */
        unsigned int  periph_clken0_mmc2         : 1;  /* bit[2]   : mmc2时钟 */
        unsigned int  periph_clken0_nandc        : 1;  /* bit[3]   : nandc时钟 */
        unsigned int  periph_clken0_usbotg       : 1;  /* bit[4]   : usbotg时钟 */
        unsigned int  periph_clken0_picophy_test : 1;  /* bit[5]   : picophy 测试时钟 */
        unsigned int  periph_clken0_pll_test     : 1;  /* bit[6]   : pll_test时钟 */
        unsigned int  reserved                   : 25; /* bit[7-31]: 时钟使能定义如下（写1时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_mmc0_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_mmc0_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_mmc1_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_mmc1_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_mmc2_START          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_mmc2_END            (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_nandc_START         (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_nandc_END           (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_usbotg_START        (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_usbotg_END          (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_picophy_test_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_picophy_test_END    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_pll_test_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN0_periph_clken0_pll_test_END        (6)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_UNION
 结构说明  : SC_PERIPH_CLKDIS0 寄存器结构定义。地址偏移量:0x204，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟禁止寄存器0。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis0_mmc0         : 1;  /* bit[0]   : mmc0时钟 */
        unsigned int  periph_clkdis0_mmc1         : 1;  /* bit[1]   : mmc1时钟 */
        unsigned int  periph_clkdis0_mmc2         : 1;  /* bit[2]   : mmc2时钟 */
        unsigned int  periph_clkdis0_nandc        : 1;  /* bit[3]   : nandc时钟 */
        unsigned int  periph_clkdis0_usbotg       : 1;  /* bit[4]   : usbotg时钟 */
        unsigned int  periph_clkdis0_picophy_test : 1;  /* bit[5]   : picophy 测试时钟 */
        unsigned int  periph_clkdis0_pll_test     : 1;  /* bit[6]   : pll_test时钟 */
        unsigned int  reserved                    : 25; /* bit[7-31]: 时钟禁止定义如下（写1时钟关闭）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_mmc0_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_mmc0_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_mmc1_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_mmc1_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_mmc2_START          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_mmc2_END            (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_nandc_START         (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_nandc_END           (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_usbotg_START        (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_usbotg_END          (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_picophy_test_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_picophy_test_END    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_pll_test_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS0_periph_clkdis0_pll_test_END        (6)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_UNION
 结构说明  : SC_PERIPH_CLKSTAT0 寄存器结构定义。地址偏移量:0x208，初值:0x00000008，宽度:32
 寄存器说明: 外设时钟状态寄存器0。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat0_mmc0         : 1;  /* bit[0]   : mmc0时钟 */
        unsigned int  periph_clkstat0_mmc1         : 1;  /* bit[1]   : mmc1时钟 */
        unsigned int  periph_clkstat0_mmc2         : 1;  /* bit[2]   : mmc2时钟 */
        unsigned int  periph_clkstat0_nandc        : 1;  /* bit[3]   : nandc时钟 */
        unsigned int  periph_clkstat0_usbotg       : 1;  /* bit[4]   : usbotg时钟 */
        unsigned int  periph_clkstat0_picophy_test : 1;  /* bit[5]   : picophy 测试时钟 */
        unsigned int  periph_clkstat0_pll_test     : 1;  /* bit[6]   : pll_test时钟 */
        unsigned int  reserved                     : 25; /* bit[7-31]: 时钟状态定义如下（高电平表示时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_mmc0_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_mmc0_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_mmc1_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_mmc1_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_mmc2_START          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_mmc2_END            (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_nandc_START         (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_nandc_END           (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_usbotg_START        (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_usbotg_END          (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_picophy_test_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_picophy_test_END    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_pll_test_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT0_periph_clkstat0_pll_test_END        (6)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKEN1_UNION
 结构说明  : SC_PERIPH_CLKEN1 寄存器结构定义。地址偏移量:0x210，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟使能寄存器1。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken1_hifi      : 1;  /* bit[0]   : hifi时钟 */
        unsigned int  reserved_0              : 1;  /* bit[1]   : sio0时钟 */
        unsigned int  reserved_1              : 1;  /* bit[2]   : sio1时钟 */
        unsigned int  reserved_2              : 1;  /* bit[3]   : sio2时钟 */
        unsigned int  reserved_3              : 1;  /* bit[4]   : sio3时钟 */
        unsigned int  periph_clken1_digacodec : 1;  /* bit[5]   : digital codec时钟 */
        unsigned int  reserved_4              : 26; /* bit[6-31]: 时钟使能定义如下（写1时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKEN1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN1_periph_clken1_hifi_START       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN1_periph_clken1_hifi_END         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN1_periph_clken1_digacodec_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN1_periph_clken1_digacodec_END    (5)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKDIS1_UNION
 结构说明  : SC_PERIPH_CLKDIS1 寄存器结构定义。地址偏移量:0x214，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟禁止寄存器1。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis1_hifi      : 1;  /* bit[0]   : hifi时钟 */
        unsigned int  reserved_0               : 1;  /* bit[1]   : sio0时钟 */
        unsigned int  reserved_1               : 1;  /* bit[2]   : sio1时钟 */
        unsigned int  reserved_2               : 1;  /* bit[3]   : sio2时钟 */
        unsigned int  reserved_3               : 1;  /* bit[4]   : sio3时钟 */
        unsigned int  periph_clkdis1_digacodec : 1;  /* bit[5]   : digital codec时钟 */
        unsigned int  reserved_4               : 26; /* bit[6-31]: 时钟禁止定义如下（写1时钟关闭）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKDIS1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS1_periph_clkdis1_hifi_START       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS1_periph_clkdis1_hifi_END         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS1_periph_clkdis1_digacodec_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS1_periph_clkdis1_digacodec_END    (5)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT1_UNION
 结构说明  : SC_PERIPH_CLKSTAT1 寄存器结构定义。地址偏移量:0x218，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟状态寄存器1。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat1_hifi      : 1;  /* bit[0]   : hifi时钟 */
        unsigned int  reserved_0                : 1;  /* bit[1]   : sio0时钟 */
        unsigned int  reserved_1                : 1;  /* bit[2]   : sio1时钟 */
        unsigned int  reserved_2                : 1;  /* bit[3]   : sio2时钟 */
        unsigned int  reserved_3                : 1;  /* bit[4]   : sio3时钟 */
        unsigned int  periph_clkstat1_digacodec : 1;  /* bit[5]   : digital codec时钟 */
        unsigned int  reserved_4                : 26; /* bit[6-31]: 时钟状态定义如下（高电平表示时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT1_periph_clkstat1_hifi_START       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT1_periph_clkstat1_hifi_END         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT1_periph_clkstat1_digacodec_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT1_periph_clkstat1_digacodec_END    (5)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_UNION
 结构说明  : SC_PERIPH_CLKEN2 寄存器结构定义。地址偏移量:0x220，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟使能寄存器2。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken2_ipf_acpu    : 1;  /* bit[0]   : acpu控制的ipf时钟 */
        unsigned int  periph_clken2_socp_acpu   : 1;  /* bit[1]   : acpu控制的socp时钟 */
        unsigned int  periph_clken2_dmac        : 1;  /* bit[2]   : dmac时钟 */
        unsigned int  periph_clken2_seceng_acpu : 1;  /* bit[3]   : acpu控制的seceng时钟 */
        unsigned int  reserved_0                : 1;  /* bit[4]   : reserved */
        unsigned int  periph_clken2_hpm0        : 1;  /* bit[5]   : hpm0时钟 */
        unsigned int  periph_clken2_hpm1        : 1;  /* bit[6]   : hpm1时钟 */
        unsigned int  periph_clken2_hpm2        : 1;  /* bit[7]   : hpm2时钟 */
        unsigned int  periph_clken2_hpm3        : 1;  /* bit[8]   : hpm3时钟 */
        unsigned int  reserved_1                : 23; /* bit[9-31]: 时钟使能定义如下（写1时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_ipf_acpu_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_ipf_acpu_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_socp_acpu_START    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_socp_acpu_END      (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_dmac_START         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_dmac_END           (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_seceng_acpu_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_seceng_acpu_END    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_hpm0_START         (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_hpm0_END           (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_hpm1_START         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_hpm1_END           (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_hpm2_START         (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_hpm2_END           (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_hpm3_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_hpm3_END           (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_UNION
 结构说明  : SC_PERIPH_CLKDIS2 寄存器结构定义。地址偏移量:0x224，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟禁止寄存器2。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis2_ipf_acpu    : 1;  /* bit[0]   : acpu控制的ipf时钟 */
        unsigned int  periph_clkdis2_socp_acpu   : 1;  /* bit[1]   : acpu控制的socp时钟 */
        unsigned int  periph_clkdis2_dmac        : 1;  /* bit[2]   : dmac时钟 */
        unsigned int  periph_clkdis2_seceng_acpu : 1;  /* bit[3]   : acpu控制的seceng时钟 */
        unsigned int  reserved_0                 : 1;  /* bit[4]   : reserved */
        unsigned int  periph_clkdis2_hpm0        : 1;  /* bit[5]   : hpm0时钟 */
        unsigned int  periph_clkdis2_hpm1        : 1;  /* bit[6]   : hpm1时钟 */
        unsigned int  periph_clkdis2_hpm2        : 1;  /* bit[7]   : hpm2时钟 */
        unsigned int  periph_clkdis2_hpm3        : 1;  /* bit[8]   : hpm3时钟 */
        unsigned int  reserved_1                 : 23; /* bit[9-31]: 时钟禁止定义如下（写1时钟关闭）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_ipf_acpu_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_ipf_acpu_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_socp_acpu_START    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_socp_acpu_END      (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_dmac_START         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_dmac_END           (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_seceng_acpu_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_seceng_acpu_END    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_hpm0_START         (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_hpm0_END           (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_hpm1_START         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_hpm1_END           (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_hpm2_START         (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_hpm2_END           (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_hpm3_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_periph_clkdis2_hpm3_END           (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_UNION
 结构说明  : SC_PERIPH_CLKSTAT2 寄存器结构定义。地址偏移量:0x228，初值:0x00000008，宽度:32
 寄存器说明: 外设时钟状态寄存器2。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat2_ipf_acpu    : 1;  /* bit[0]   : acpu控制的ipf时钟 */
        unsigned int  periph_clkstat2_socp_acpu   : 1;  /* bit[1]   : acpu控制的socp时钟 */
        unsigned int  periph_clkstat2_dmac        : 1;  /* bit[2]   : dmac时钟 */
        unsigned int  periph_clkstat2_seceng_acpu : 1;  /* bit[3]   : acpu控制的seceng时钟 */
        unsigned int  reserved_0                  : 1;  /* bit[4]   : reserved */
        unsigned int  periph_clkstat2_hpm0        : 1;  /* bit[5]   : hpm0时钟 */
        unsigned int  periph_clkstat2_hpm1        : 1;  /* bit[6]   : hpm1时钟 */
        unsigned int  periph_clkstat2_hpm2        : 1;  /* bit[7]   : hpm2时钟 */
        unsigned int  periph_clkstat2_hpm3        : 1;  /* bit[8]   : hpm3时钟 */
        unsigned int  reserved_1                  : 23; /* bit[9-31]: 时钟状态定义如下（高电平表示时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_ipf_acpu_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_ipf_acpu_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_socp_acpu_START    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_socp_acpu_END      (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_dmac_START         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_dmac_END           (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_seceng_acpu_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_seceng_acpu_END    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_hpm0_START         (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_hpm0_END           (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_hpm1_START         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_hpm1_END           (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_hpm2_START         (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_hpm2_END           (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_hpm3_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT2_periph_clkstat2_hpm3_END           (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_UNION
 结构说明  : SC_PERIPH_CLKEN3 寄存器结构定义。地址偏移量:0x230，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟使能寄存器3。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken3_cssys     : 1;  /* bit[0]    : coresight atb tpiu 时钟  */
        unsigned int  periph_clken3_i2c0      : 1;  /* bit[1]    : i2c0时钟 */
        unsigned int  periph_clken3_i2c1      : 1;  /* bit[2]    : i2c1时钟 */
        unsigned int  periph_clken3_i2c2      : 1;  /* bit[3]    : i2c2时钟 */
        unsigned int  periph_clken3_i2c3      : 1;  /* bit[4]    : i2c3时钟 */
        unsigned int  periph_clken3_uart1     : 1;  /* bit[5]    : uart1时钟 */
        unsigned int  periph_clken3_uart2     : 1;  /* bit[6]    : uart2时钟 */
        unsigned int  periph_clken3_uart3     : 1;  /* bit[7]    : uart3时钟 */
        unsigned int  periph_clken3_uart4     : 1;  /* bit[8]    : uart4时钟 */
        unsigned int  periph_clken3_ssp       : 1;  /* bit[9]    : ssp时钟 */
        unsigned int  periph_clken3_pwm       : 1;  /* bit[10]   : pwm时钟 */
        unsigned int  periph_clken3_blpwm     : 1;  /* bit[11]   : backlight pwm时钟 */
        unsigned int  periph_clken3_tsensor   : 1;  /* bit[12]   : tsensor时钟 */
        unsigned int  reserved_0              : 1;  /* bit[13]   : DJTAG PMC时钟 */
        unsigned int  reserved_1              : 1;  /* bit[14]   : DJTAG IOC时钟 */
        unsigned int  periph_clken3_gps       : 1;  /* bit[15]   : gps时钟 */
        unsigned int  periph_clken3_tcxo_pad0 : 1;  /* bit[16]   : tcxo pad0时钟 */
        unsigned int  periph_clken3_tcxo_pad1 : 1;  /* bit[17]   : tcxo pad1时钟 */
        unsigned int  periph_clken3_dapb      : 1;  /* bit[18]   : dapb 和sapb时钟 */
        unsigned int  periph_clken3_hkadc     : 1;  /* bit[19]   : hkadc时钟 */
        unsigned int  periph_clken3_codec_ssi : 1;  /* bit[20]   : codec ssi时钟 */
        unsigned int  periph_clken3_tzpc_dep  : 1;  /* bit[21]   : tzpc_dep时钟 */
        unsigned int  reserved_2              : 10; /* bit[22-31]: 时钟使能定义如下（写1时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_cssys_START      (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_cssys_END        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_i2c0_START       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_i2c0_END         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_i2c1_START       (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_i2c1_END         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_i2c2_START       (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_i2c2_END         (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_i2c3_START       (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_i2c3_END         (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_uart1_START      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_uart1_END        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_uart2_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_uart2_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_uart3_START      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_uart3_END        (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_uart4_START      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_uart4_END        (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_ssp_START        (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_ssp_END          (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_pwm_START        (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_pwm_END          (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_blpwm_START      (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_blpwm_END        (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_tsensor_START    (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_tsensor_END      (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_gps_START        (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_gps_END          (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_tcxo_pad0_START  (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_tcxo_pad0_END    (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_tcxo_pad1_START  (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_tcxo_pad1_END    (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_dapb_START       (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_dapb_END         (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_hkadc_START      (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_hkadc_END        (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_codec_ssi_START  (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_codec_ssi_END    (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_tzpc_dep_START   (21)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN3_periph_clken3_tzpc_dep_END     (21)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_UNION
 结构说明  : SC_PERIPH_CLKDIS3 寄存器结构定义。地址偏移量:0x234，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟禁止寄存器3。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis3_cssys     : 1;  /* bit[0]    : coresight sys时钟 */
        unsigned int  periph_clkdis3_i2c0      : 1;  /* bit[1]    : i2c0时钟 */
        unsigned int  periph_clkdis3_i2c1      : 1;  /* bit[2]    : i2c1时钟 */
        unsigned int  periph_clkdis3_i2c2      : 1;  /* bit[3]    : i2c2时钟 */
        unsigned int  periph_clkdis3_i2c3      : 1;  /* bit[4]    : i2c3时钟 */
        unsigned int  periph_clkdis3_uart1     : 1;  /* bit[5]    : uart1时钟 */
        unsigned int  periph_clkdis3_uart2     : 1;  /* bit[6]    : uart2时钟 */
        unsigned int  periph_clkdis3_uart3     : 1;  /* bit[7]    : uart3时钟 */
        unsigned int  periph_clkdis3_uart4     : 1;  /* bit[8]    : uart4时钟 */
        unsigned int  periph_clkdis3_ssp       : 1;  /* bit[9]    : ssp时钟 */
        unsigned int  periph_clkdis3_pwm       : 1;  /* bit[10]   : pwm时钟 */
        unsigned int  periph_clkdis3_blpwm     : 1;  /* bit[11]   : backlight pwm时钟 */
        unsigned int  periph_clkdis3_tsensor   : 1;  /* bit[12]   : tsensor时钟 */
        unsigned int  reserved_0               : 1;  /* bit[13]   : DJTAG PMC时钟 */
        unsigned int  reserved_1               : 1;  /* bit[14]   : DJTAG IOC时钟 */
        unsigned int  periph_clkdis3_gps       : 1;  /* bit[15]   : gps时钟 */
        unsigned int  periph_clkdis3_tcxo_pad0 : 1;  /* bit[16]   : tcxo pad0时钟 */
        unsigned int  periph_clkdis3_tcxo_pad1 : 1;  /* bit[17]   : tcxo pad1时钟 */
        unsigned int  periph_clkdis3_dapb      : 1;  /* bit[18]   : dapb 和sapb时钟 */
        unsigned int  periph_clkdis3_hkadc     : 1;  /* bit[19]   : hkadc时钟 */
        unsigned int  periph_clkdis3_codec_ssi : 1;  /* bit[20]   : codec ssi时钟 */
        unsigned int  periph_clkdis3_tzpc_dep  : 1;  /* bit[21]   : tzpc_dep时钟 */
        unsigned int  reserved_2               : 10; /* bit[22-31]: 时钟禁止定义如下（写1时钟关闭）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_cssys_START      (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_cssys_END        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_i2c0_START       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_i2c0_END         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_i2c1_START       (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_i2c1_END         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_i2c2_START       (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_i2c2_END         (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_i2c3_START       (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_i2c3_END         (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_uart1_START      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_uart1_END        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_uart2_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_uart2_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_uart3_START      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_uart3_END        (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_uart4_START      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_uart4_END        (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_ssp_START        (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_ssp_END          (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_pwm_START        (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_pwm_END          (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_blpwm_START      (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_blpwm_END        (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_tsensor_START    (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_tsensor_END      (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_gps_START        (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_gps_END          (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_tcxo_pad0_START  (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_tcxo_pad0_END    (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_tcxo_pad1_START  (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_tcxo_pad1_END    (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_dapb_START       (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_dapb_END         (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_hkadc_START      (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_hkadc_END        (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_codec_ssi_START  (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_codec_ssi_END    (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_tzpc_dep_START   (21)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS3_periph_clkdis3_tzpc_dep_END     (21)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_UNION
 结构说明  : SC_PERIPH_CLKSTAT3 寄存器结构定义。地址偏移量:0x238，初值:0x00240001，宽度:32
 寄存器说明: 外设时钟状态寄存器3。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat3_cssys     : 1;  /* bit[0]    : coresight sys时钟 */
        unsigned int  periph_clkstat3_i2c0      : 1;  /* bit[1]    : i2c0时钟 */
        unsigned int  periph_clkstat3_i2c1      : 1;  /* bit[2]    : i2c1时钟 */
        unsigned int  periph_clkstat3_i2c2      : 1;  /* bit[3]    : i2c2时钟 */
        unsigned int  periph_clkstat3_i2c3      : 1;  /* bit[4]    : i2c3时钟 */
        unsigned int  periph_clkstat3_uart1     : 1;  /* bit[5]    : uart1时钟 */
        unsigned int  periph_clkstat3_uart2     : 1;  /* bit[6]    : uart2时钟 */
        unsigned int  periph_clkstat3_uart3     : 1;  /* bit[7]    : uart3时钟 */
        unsigned int  periph_clkstat3_uart4     : 1;  /* bit[8]    : uart4时钟 */
        unsigned int  periph_clkstat3_ssp       : 1;  /* bit[9]    : ssp时钟 */
        unsigned int  periph_clkstat3_pwm       : 1;  /* bit[10]   : pwm时钟 */
        unsigned int  periph_clkstat3_blpwm     : 1;  /* bit[11]   : backlight pwm时钟 */
        unsigned int  periph_clkstat3_tsensor   : 1;  /* bit[12]   : tsensor时钟 */
        unsigned int  reserved_0                : 1;  /* bit[13]   : DJTAG PMC时钟 */
        unsigned int  reserved_1                : 1;  /* bit[14]   : DJTAG IOC时钟 */
        unsigned int  periph_clkstat3_gps       : 1;  /* bit[15]   : gps时钟 */
        unsigned int  periph_clkstat3_tcxo_pad0 : 1;  /* bit[16]   : tcxo pad0时钟 */
        unsigned int  periph_clkstat3_tcxo_pad1 : 1;  /* bit[17]   : tcxo pad1时钟 */
        unsigned int  periph_clkstat3_dapb      : 1;  /* bit[18]   : dapb 和sapb时钟 */
        unsigned int  periph_clkstat3_hkadc     : 1;  /* bit[19]   : hkadc时钟 */
        unsigned int  periph_clkstat3_codec_ssi : 1;  /* bit[20]   : codec ssi时钟 */
        unsigned int  periph_clkstat3_tzpc_dep  : 1;  /* bit[21]   : tzpc_dep时钟 */
        unsigned int  reserved_2                : 10; /* bit[22-31]: 时钟状态定义如下（高电平表示时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_cssys_START      (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_cssys_END        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_i2c0_START       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_i2c0_END         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_i2c1_START       (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_i2c1_END         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_i2c2_START       (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_i2c2_END         (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_i2c3_START       (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_i2c3_END         (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_uart1_START      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_uart1_END        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_uart2_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_uart2_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_uart3_START      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_uart3_END        (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_uart4_START      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_uart4_END        (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_ssp_START        (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_ssp_END          (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_pwm_START        (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_pwm_END          (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_blpwm_START      (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_blpwm_END        (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_tsensor_START    (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_tsensor_END      (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_gps_START        (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_gps_END          (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_tcxo_pad0_START  (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_tcxo_pad0_END    (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_tcxo_pad1_START  (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_tcxo_pad1_END    (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_dapb_START       (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_dapb_END         (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_hkadc_START      (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_hkadc_END        (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_codec_ssi_START  (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_codec_ssi_END    (20)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_tzpc_dep_START   (21)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT3_periph_clkstat3_tzpc_dep_END     (21)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_UNION
 结构说明  : SC_PERIPH_CLKEN8 寄存器结构定义。地址偏移量:0x240，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟使能寄存器8。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken8_rs0         : 1;  /* bit[0]    : rs0时钟 */
        unsigned int  periph_clken8_rs2         : 1;  /* bit[1]    : rs2时钟 */
        unsigned int  periph_clken8_rs3         : 1;  /* bit[2]    : rs3时钟 */
        unsigned int  periph_clken8_ms0         : 1;  /* bit[3]    : ms0时钟 */
        unsigned int  reserved_0                : 1;  /* bit[4]    : ms1时钟 */
        unsigned int  periph_clken8_ms2         : 1;  /* bit[5]    : ms2时钟 */
        unsigned int  periph_clken8_xg2ram0     : 1;  /* bit[6]    : xg2ram0时钟 */
        unsigned int  periph_clken8_x2sram_tzma : 1;  /* bit[7]    : x2sram和tzma时钟 */
        unsigned int  periph_clken8_sram        : 1;  /* bit[8]    : sram时钟 */
        unsigned int  periph_clken8_rom         : 1;  /* bit[9]    : rom时钟 */
        unsigned int  periph_clken8_harq        : 1;  /* bit[10]   : harq时钟 */
        unsigned int  periph_clken8_mmu         : 1;  /* bit[11]   : mmu时钟 */
        unsigned int  periph_clken8_ddrc        : 1;  /* bit[12]   : ddr时钟 */
        unsigned int  periph_clken8_ddrphy      : 1;  /* bit[13]   : ddrphy0相位 90相位时钟 */
        unsigned int  periph_clken8_ddrphy_ref  : 1;  /* bit[14]   : ddrphy ref时钟 */
        unsigned int  periph_clken8_x2x_sysnoc  : 1;  /* bit[15]   : sysnoc x2x异步桥时钟 */
        unsigned int  periph_clken8_x2x_ccpu    : 1;  /* bit[16]   : ccpu x2x异步桥时钟 */
        unsigned int  periph_clken8_ddrt        : 1;  /* bit[17]   : ddrt时钟 */
        unsigned int  periph_clken8_ddrpack_rs  : 1;  /* bit[18]   : ddrpack和rs时钟 */
        unsigned int  reserved_1                : 13; /* bit[19-31]: 时钟使能定义如下（写1时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_rs0_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_rs0_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_rs2_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_rs2_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_rs3_START          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_rs3_END            (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ms0_START          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ms0_END            (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ms2_START          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ms2_END            (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_xg2ram0_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_xg2ram0_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_x2sram_tzma_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_x2sram_tzma_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_sram_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_sram_END           (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_rom_START          (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_rom_END            (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_harq_START         (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_harq_END           (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_mmu_START          (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_mmu_END            (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrc_START         (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrc_END           (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrphy_START       (13)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrphy_END         (13)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrphy_ref_START   (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrphy_ref_END     (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_x2x_sysnoc_START   (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_x2x_sysnoc_END     (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_x2x_ccpu_START     (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_x2x_ccpu_END       (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrt_START         (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrt_END           (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrpack_rs_START   (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_periph_clken8_ddrpack_rs_END     (18)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_UNION
 结构说明  : SC_PERIPH_CLKDIS8 寄存器结构定义。地址偏移量:0x244，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟禁止寄存器8。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis8_rs0         : 1;  /* bit[0]    : rs0时钟 */
        unsigned int  periph_clkdis8_rs2         : 1;  /* bit[1]    : rs2时钟 */
        unsigned int  periph_clkdis8_rs3         : 1;  /* bit[2]    : rs3时钟 */
        unsigned int  periph_clkdis8_ms0         : 1;  /* bit[3]    : ms0时钟 */
        unsigned int  reserved_0                 : 1;  /* bit[4]    : ms1时钟 */
        unsigned int  periph_clkdis8_ms2         : 1;  /* bit[5]    : ms2时钟 */
        unsigned int  periph_clkdis8_xg2ram0     : 1;  /* bit[6]    : xg2ram0时钟 */
        unsigned int  periph_clkdis8_x2sram_tzma : 1;  /* bit[7]    : x2sram和tzma时钟 */
        unsigned int  periph_clkdis8_sram        : 1;  /* bit[8]    : sram时钟 */
        unsigned int  periph_clkdis8_rom         : 1;  /* bit[9]    : rom时钟 */
        unsigned int  periph_clkdis8_harq        : 1;  /* bit[10]   : harq时钟 */
        unsigned int  periph_clkdis8_mmu         : 1;  /* bit[11]   : mmu时钟 */
        unsigned int  periph_clkdis8_ddrc        : 1;  /* bit[12]   : ddr时钟 */
        unsigned int  periph_clkdis8_ddrphy      : 1;  /* bit[13]   : ddrphy0相位 90相位时钟 */
        unsigned int  periph_clkdis8_ddrphy_ref  : 1;  /* bit[14]   : ddrphy ref时钟 */
        unsigned int  periph_clkdis8_x2x_sysnoc  : 1;  /* bit[15]   : sysnoc x2x异步桥时钟 */
        unsigned int  periph_clkdis8_x2x_ccpu    : 1;  /* bit[16]   : ccpu x2x异步桥时钟 */
        unsigned int  periph_clkdis8_ddrt        : 1;  /* bit[17]   : ddrt时钟 */
        unsigned int  periph_clkdis8_ddrpack_rs  : 1;  /* bit[18]   : ddrpack和rs时钟 */
        unsigned int  reserved_1                 : 13; /* bit[19-31]: 时钟禁止定义如下（写1时钟关闭）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_rs0_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_rs0_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_rs2_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_rs2_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_rs3_START          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_rs3_END            (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ms0_START          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ms0_END            (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ms2_START          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ms2_END            (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_xg2ram0_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_xg2ram0_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_x2sram_tzma_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_x2sram_tzma_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_sram_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_sram_END           (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_rom_START          (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_rom_END            (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_harq_START         (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_harq_END           (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_mmu_START          (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_mmu_END            (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrc_START         (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrc_END           (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrphy_START       (13)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrphy_END         (13)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrphy_ref_START   (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrphy_ref_END     (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_x2x_sysnoc_START   (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_x2x_sysnoc_END     (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_x2x_ccpu_START     (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_x2x_ccpu_END       (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrt_START         (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrt_END           (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrpack_rs_START   (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS8_periph_clkdis8_ddrpack_rs_END     (18)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_UNION
 结构说明  : SC_PERIPH_CLKSTAT8 寄存器结构定义。地址偏移量:0x248，初值:0x000007C8，宽度:32
 寄存器说明: 外设时钟状态寄存器8。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat8_rs0         : 1;  /* bit[0]    : rs0时钟 */
        unsigned int  periph_clkstat8_rs2         : 1;  /* bit[1]    : rs2时钟 */
        unsigned int  periph_clkstat8_rs3         : 1;  /* bit[2]    : rs3时钟 */
        unsigned int  periph_clkstat8_ms0         : 1;  /* bit[3]    : ms0时钟 */
        unsigned int  reserved_0                  : 1;  /* bit[4]    : ms1时钟 */
        unsigned int  periph_clkstat8_ms2         : 1;  /* bit[5]    : ms2时钟 */
        unsigned int  periph_clkstat8_xg2ram0     : 1;  /* bit[6]    : xg2ram0时钟 */
        unsigned int  periph_clkstat8_x2sram_tzma : 1;  /* bit[7]    : x2sram和tzma时钟 */
        unsigned int  periph_clkstat8_sram        : 1;  /* bit[8]    : sram时钟 */
        unsigned int  periph_clkstat8_rom         : 1;  /* bit[9]    : rom时钟 */
        unsigned int  periph_clkstat8_harq        : 1;  /* bit[10]   : harq时钟 */
        unsigned int  periph_clkstat8_mmu         : 1;  /* bit[11]   : mmu时钟 */
        unsigned int  periph_clkstat8_ddrc        : 1;  /* bit[12]   : ddr时钟 */
        unsigned int  periph_clkstat8_ddrphy      : 1;  /* bit[13]   : ddrphy0相位 90相位时钟 */
        unsigned int  periph_clkstat8_ddrphy_ref  : 1;  /* bit[14]   : ddrphy ref时钟 */
        unsigned int  periph_clkstat8_x2x_sysnoc  : 1;  /* bit[15]   : sysnoc x2x异步桥时钟 */
        unsigned int  periph_clkstat8_x2x_ccpu    : 1;  /* bit[16]   : ccpu x2x异步桥时钟 */
        unsigned int  periph_clkstat8_ddrt        : 1;  /* bit[17]   : ddrt时钟 */
        unsigned int  periph_clkstat8_ddrpack_rs  : 1;  /* bit[18]   : ddrpack和rs时钟 */
        unsigned int  reserved_1                  : 13; /* bit[19-31]: 时钟状态定义如下（高电平表示时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_rs0_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_rs0_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_rs2_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_rs2_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_rs3_START          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_rs3_END            (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ms0_START          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ms0_END            (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ms2_START          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ms2_END            (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_xg2ram0_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_xg2ram0_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_x2sram_tzma_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_x2sram_tzma_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_sram_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_sram_END           (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_rom_START          (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_rom_END            (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_harq_START         (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_harq_END           (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_mmu_START          (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_mmu_END            (11)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrc_START         (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrc_END           (12)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrphy_START       (13)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrphy_END         (13)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrphy_ref_START   (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrphy_ref_END     (14)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_x2x_sysnoc_START   (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_x2x_sysnoc_END     (15)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_x2x_ccpu_START     (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_x2x_ccpu_END       (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrt_START         (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrt_END           (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrpack_rs_START   (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT8_periph_clkstat8_ddrpack_rs_END     (18)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_UNION
 结构说明  : SC_PERIPH_CLKEN9 寄存器结构定义。地址偏移量:0x250，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟使能寄存器9。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken9_carm_dap    : 1;  /* bit[0]   : carm debug apb时钟 */
        unsigned int  periph_clken9_carm_atb    : 1;  /* bit[1]   : carm atb时钟 */
        unsigned int  periph_clken9_carm_lbus   : 1;  /* bit[2]   : carm localbus时钟 */
        unsigned int  periph_clken9_carm_kernel : 1;  /* bit[3]   : carm内核时钟 */
        unsigned int  reserved                  : 28; /* bit[4-31]: 时钟使能定义如下（写1时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_dap_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_dap_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_atb_START     (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_atb_END       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_lbus_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_lbus_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_kernel_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_kernel_END    (3)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_UNION
 结构说明  : SC_PERIPH_CLKDIS9 寄存器结构定义。地址偏移量:0x254，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟禁止寄存器9。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis9_carm_dap    : 1;  /* bit[0]   : carm debug apb时钟 */
        unsigned int  periph_clkdis9_carm_atb    : 1;  /* bit[1]   : carm atb时钟 */
        unsigned int  periph_clkdis9_carm_lbus   : 1;  /* bit[2]   : carm localbus时钟 */
        unsigned int  periph_clkdis9_carm_kernel : 1;  /* bit[3]   : carm内核时钟 */
        unsigned int  reserved                   : 28; /* bit[4-31]: 时钟禁止定义如下（写1时钟关闭）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_periph_clkdis9_carm_dap_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_periph_clkdis9_carm_dap_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_periph_clkdis9_carm_atb_START     (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_periph_clkdis9_carm_atb_END       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_periph_clkdis9_carm_lbus_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_periph_clkdis9_carm_lbus_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_periph_clkdis9_carm_kernel_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS9_periph_clkdis9_carm_kernel_END    (3)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_UNION
 结构说明  : SC_PERIPH_CLKSTAT9 寄存器结构定义。地址偏移量:0x258，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟状态寄存器9。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat9_carm_dap    : 1;  /* bit[0]   : carm debug apb时钟 */
        unsigned int  periph_clkstat9_carm_atb    : 1;  /* bit[1]   : carm atb时钟 */
        unsigned int  periph_clkstat9_carm_lbus   : 1;  /* bit[2]   : carm localbus时钟 */
        unsigned int  periph_clkstat9_carm_kernel : 1;  /* bit[3]   : carm内核时钟 */
        unsigned int  reserved                    : 28; /* bit[4-31]: 时钟状态定义如下（高电平表示时钟打开）： */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_periph_clkstat9_carm_dap_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_periph_clkstat9_carm_dap_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_periph_clkstat9_carm_atb_START     (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_periph_clkstat9_carm_atb_END       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_periph_clkstat9_carm_lbus_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_periph_clkstat9_carm_lbus_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_periph_clkstat9_carm_kernel_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT9_periph_clkstat9_carm_kernel_END    (3)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_UNION
 结构说明  : SC_PERIPH_CLKEN10 寄存器结构定义。地址偏移量:0x260，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟使能寄存器10。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken10_ipf_ccpu    : 1;  /* bit[0]    : ccpu控制的ipf软门控 */
        unsigned int  periph_clken10_socp_ccpu   : 1;  /* bit[1]    : ccpu控制的socp软门控 */
        unsigned int  periph_clken10_seceng_ccpu : 1;  /* bit[2]    : ccpu控制的seceng软门控 */
        unsigned int  periph_clken10_harq_ccpu   : 1;  /* bit[3]    : ccpu控制的harq软门控 */
        unsigned int  reserved_0                 : 12; /* bit[4-15] : reserved */
        unsigned int  periph_clken10_ipf_mcu     : 1;  /* bit[16]   : mcu控制的ipf软门控 */
        unsigned int  periph_clken10_socp_mcu    : 1;  /* bit[17]   : mcu控制的socp软门控 */
        unsigned int  periph_clken10_seceng_mcu  : 1;  /* bit[18]   : mcu控制的seceng软门控 */
        unsigned int  periph_clken10_harq_mcu    : 1;  /* bit[19]   : mcu控制的harq软门控 */
        unsigned int  reserved_1                 : 12; /* bit[20-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_ipf_ccpu_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_ipf_ccpu_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_socp_ccpu_START    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_socp_ccpu_END      (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_seceng_ccpu_START  (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_seceng_ccpu_END    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_harq_ccpu_START    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_harq_ccpu_END      (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_ipf_mcu_START      (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_ipf_mcu_END        (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_socp_mcu_START     (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_socp_mcu_END       (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_seceng_mcu_START   (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_seceng_mcu_END     (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_harq_mcu_START     (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN10_periph_clken10_harq_mcu_END       (19)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_UNION
 结构说明  : SC_PERIPH_CLKDIS10 寄存器结构定义。地址偏移量:0x264，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟禁止寄存器10。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis10_ipf_ccpu    : 1;  /* bit[0]    : ccpu控制的ipf软门控 */
        unsigned int  periph_clkdis10_socp_ccpu   : 1;  /* bit[1]    : ccpu控制的socp软门控 */
        unsigned int  periph_clkdis10_seceng_ccpu : 1;  /* bit[2]    : ccpu控制的seceng软门控 */
        unsigned int  periph_clkdis10_harq_ccpu   : 1;  /* bit[3]    : ccpu控制的harq软门控 */
        unsigned int  reserved_0                  : 12; /* bit[4-15] : reserved */
        unsigned int  periph_clkdis10_ipf_mcu     : 1;  /* bit[16]   : mcu控制的ipf软门控 */
        unsigned int  periph_clkdis10_socp_mcu    : 1;  /* bit[17]   : mcu控制的socp软门控 */
        unsigned int  periph_clkdis10_seceng_mcu  : 1;  /* bit[18]   : mcu控制的seceng软门控 */
        unsigned int  periph_clkdis10_harq_mcu    : 1;  /* bit[19]   : mcu控制的harq软门控 */
        unsigned int  reserved_1                  : 12; /* bit[20-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_ipf_ccpu_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_ipf_ccpu_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_socp_ccpu_START    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_socp_ccpu_END      (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_seceng_ccpu_START  (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_seceng_ccpu_END    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_harq_ccpu_START    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_harq_ccpu_END      (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_ipf_mcu_START      (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_ipf_mcu_END        (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_socp_mcu_START     (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_socp_mcu_END       (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_seceng_mcu_START   (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_seceng_mcu_END     (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_harq_mcu_START     (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS10_periph_clkdis10_harq_mcu_END       (19)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_UNION
 结构说明  : SC_PERIPH_CLKSTAT10 寄存器结构定义。地址偏移量:0x268，初值:0x00040004，宽度:32
 寄存器说明: 外设时钟状态寄存器10。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat10_ipf_ccpu    : 1;  /* bit[0]    : ccpu控制的ipf软门控 */
        unsigned int  periph_clkstat10_socp_ccpu   : 1;  /* bit[1]    : ccpu控制的socp软门控 */
        unsigned int  periph_clkstat10_seceng_ccpu : 1;  /* bit[2]    : ccpu控制的seceng软门控 */
        unsigned int  periph_clkstat10_harq_ccpu   : 1;  /* bit[3]    : ccpu控制的harq软门控 */
        unsigned int  reserved_0                   : 12; /* bit[4-15] : reserved */
        unsigned int  periph_clkstat10_ipf_mcu     : 1;  /* bit[16]   : mcu控制的ipf软门控 */
        unsigned int  periph_clkstat10_socp_mcu    : 1;  /* bit[17]   : mcu控制的socp软门控 */
        unsigned int  periph_clkstat10_seceng_mcu  : 1;  /* bit[18]   : mcu控制的seceng软门控 */
        unsigned int  periph_clkstat10_harq_mcu    : 1;  /* bit[19]   : mcu控制的harq软门控 */
        unsigned int  reserved_1                   : 12; /* bit[20-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_ipf_ccpu_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_ipf_ccpu_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_socp_ccpu_START    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_socp_ccpu_END      (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_seceng_ccpu_START  (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_seceng_ccpu_END    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_harq_ccpu_START    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_harq_ccpu_END      (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_ipf_mcu_START      (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_ipf_mcu_END        (16)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_socp_mcu_START     (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_socp_mcu_END       (17)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_seceng_mcu_START   (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_seceng_mcu_END     (18)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_harq_mcu_START     (19)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT10_periph_clkstat10_harq_mcu_END       (19)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_UNION
 结构说明  : SC_PERIPH_CLKEN12 寄存器结构定义。地址偏移量:0x270，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟使能寄存器12。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken12_hifi_src       : 1;  /* bit[0]    : hifi 分频前src时钟门控 */
        unsigned int  periph_clken12_mmc0_src       : 1;  /* bit[1]    : mmc0 分频前src时钟门控 */
        unsigned int  periph_clken12_mmc1_src       : 1;  /* bit[2]    : mmc1 分频前src时钟门控 */
        unsigned int  periph_clken12_mmc2_src       : 1;  /* bit[3]    : mmc2 分频前src时钟门控 */
        unsigned int  periph_clken12_syspll_div_src : 1;  /* bit[4]    : syspll div 分频前src时钟门控 */
        unsigned int  periph_clken12_tpiu_src       : 1;  /* bit[5]    : tpiu 分频前src时钟门控 */
        unsigned int  periph_clken12_mmc0_hf        : 1;  /* bit[6]    : mmc0 移相高频时钟门控 */
        unsigned int  periph_clken12_mmc1_hf        : 1;  /* bit[7]    : mmc1 移相高频时钟门控 */
        unsigned int  periph_clken12_pll_test_src   : 1;  /* bit[8]    : pll test分频前时钟门控 */
        unsigned int  periph_clken12_codec_soc      : 1;  /* bit[9]    : codec时钟soc侧时钟门控 */
        unsigned int  periph_clken12_media          : 1;  /* bit[10]   : media子系统时钟前的门控 */
        unsigned int  reserved                      : 21; /* bit[11-31]: 时钟使能定义如下（写1时钟打开）：
                                                                         dsppll_48m_gated时钟 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_hifi_src_START        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_hifi_src_END          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc0_src_START        (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc0_src_END          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc1_src_START        (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc1_src_END          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc2_src_START        (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc2_src_END          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_syspll_div_src_START  (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_syspll_div_src_END    (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_tpiu_src_START        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_tpiu_src_END          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc0_hf_START         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc0_hf_END           (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc1_hf_START         (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_mmc1_hf_END           (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_pll_test_src_START    (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_pll_test_src_END      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_codec_soc_START       (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_codec_soc_END         (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_media_START           (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_periph_clken12_media_END             (10)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_UNION
 结构说明  : SC_PERIPH_CLKDIS12 寄存器结构定义。地址偏移量:0x274，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟禁止寄存器12。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis12_hifi_src       : 1;  /* bit[0]    : hifi 分频前src时钟门控 */
        unsigned int  periph_clkdis12_mmc0_src       : 1;  /* bit[1]    : mmc0 分频前src时钟门控 */
        unsigned int  periph_clkdis12_mmc1_src       : 1;  /* bit[2]    : mmc1 分频前src时钟门控 */
        unsigned int  periph_clkdis12_mmc2_src       : 1;  /* bit[3]    : mmc2 分频前src时钟门控 */
        unsigned int  periph_clkdis12_syspll_div_src : 1;  /* bit[4]    : syspll div 分频前src时钟门控 */
        unsigned int  periph_clkdis12_tpiu_src       : 1;  /* bit[5]    : tpiu 分频前src时钟门控 */
        unsigned int  periph_clkdis12_mmc0_hf        : 1;  /* bit[6]    : mmc0 移相高频时钟门控 */
        unsigned int  periph_clkdis12_mmc1_hf        : 1;  /* bit[7]    : mmc1 移相高频时钟门控 */
        unsigned int  periph_clkdis12_pll_test_src   : 1;  /* bit[8]    : pll test分频前时钟门控 */
        unsigned int  periph_clkdis12_codec_soc      : 1;  /* bit[9]    : codec时钟soc侧时钟门控 */
        unsigned int  periph_clkdis12_media          : 1;  /* bit[10]   : media子系统时钟前的门控 */
        unsigned int  reserved                       : 21; /* bit[11-31]: 时钟禁止定义如下（写1时钟关闭）：
                                                                          dsppll_48m_gated时钟 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_hifi_src_START        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_hifi_src_END          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc0_src_START        (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc0_src_END          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc1_src_START        (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc1_src_END          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc2_src_START        (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc2_src_END          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_syspll_div_src_START  (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_syspll_div_src_END    (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_tpiu_src_START        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_tpiu_src_END          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc0_hf_START         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc0_hf_END           (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc1_hf_START         (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_mmc1_hf_END           (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_pll_test_src_START    (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_pll_test_src_END      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_codec_soc_START       (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_codec_soc_END         (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_media_START           (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS12_periph_clkdis12_media_END             (10)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_UNION
 结构说明  : SC_PERIPH_CLKSTAT12 寄存器结构定义。地址偏移量:0x278，初值:0x0000003F，宽度:32
 寄存器说明: 外设时钟状态寄存器12。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat12_hifi_src       : 1;  /* bit[0]    : hifi 分频前src时钟门控 */
        unsigned int  periph_clkstat12_mmc0_src       : 1;  /* bit[1]    : mmc0 分频前src时钟门控 */
        unsigned int  periph_clkstat12_mmc1_src       : 1;  /* bit[2]    : mmc1 分频前src时钟门控 */
        unsigned int  periph_clkstat12_mmc2_src       : 1;  /* bit[3]    : mmc2 分频前src时钟门控 */
        unsigned int  periph_clkstat12_syspll_div_src : 1;  /* bit[4]    : syspll div 分频前src时钟门控 */
        unsigned int  periph_clkstat12_tpiu_src       : 1;  /* bit[5]    : tpiu 分频前src时钟门控 */
        unsigned int  periph_clkstat12_mmc0_hf        : 1;  /* bit[6]    : mmc0 移相高频时钟门控 */
        unsigned int  periph_clkstat12_mmc1_hf        : 1;  /* bit[7]    : mmc1 移相高频时钟门控 */
        unsigned int  periph_clkstat12_pll_test_src   : 1;  /* bit[8]    : pll test分频前时钟门控 */
        unsigned int  periph_clkstat12_codec_soc      : 1;  /* bit[9]    : codec时钟soc侧时钟门控 */
        unsigned int  periph_clkstat12_media          : 1;  /* bit[10]   : media子系统时钟前的门控 */
        unsigned int  reserved                        : 21; /* bit[11-31]: 时钟状态定义如下（高电平表示时钟打开）：
                                                                           dsppll_48m_gated时钟 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_hifi_src_START        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_hifi_src_END          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc0_src_START        (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc0_src_END          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc1_src_START        (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc1_src_END          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc2_src_START        (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc2_src_END          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_syspll_div_src_START  (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_syspll_div_src_END    (4)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_tpiu_src_START        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_tpiu_src_END          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc0_hf_START         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc0_hf_END           (6)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc1_hf_START         (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_mmc1_hf_END           (7)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_pll_test_src_START    (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_pll_test_src_END      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_codec_soc_START       (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_codec_soc_END         (9)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_media_START           (10)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT12_periph_clkstat12_media_END             (10)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKEN14_UNION
 结构说明  : SC_PERIPH_CLKEN14 寄存器结构定义。地址偏移量:0x280，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟使能寄存器14。用于使能外设时钟。对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clken14_reserved : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKEN14_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN14_periph_clken14_reserved_START  (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKEN14_periph_clken14_reserved_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKDIS14_UNION
 结构说明  : SC_PERIPH_CLKDIS14 寄存器结构定义。地址偏移量:0x284，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟禁止寄存器14。用于禁止外设时钟。对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkdis14_reserved : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKDIS14_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS14_periph_clkdis14_reserved_START  (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKDIS14_periph_clkdis14_reserved_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT14_UNION
 结构说明  : SC_PERIPH_CLKSTAT14 寄存器结构定义。地址偏移量:0x288，初值:0x00000000，宽度:32
 寄存器说明: 外设时钟状态寄存器14。用于回读各外设时钟的状态。外设时钟控制信号需经过在CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_clkstat14_reserved : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT14_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT14_periph_clkstat14_reserved_START  (0)
#define SOC_PERI_SCTRL_SC_PERIPH_CLKSTAT14_periph_clkstat14_reserved_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_UNION
 结构说明  : SC_PERIPH_RSTEN0 寄存器结构定义。地址偏移量:0x300，初值:0x00000000，宽度:32
 寄存器说明: 设复位使能寄存器0。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rsten0_mmc0        : 1;  /* bit[0]   : mmc0复位 */
        unsigned int  periph_rsten0_mmc1        : 1;  /* bit[1]   : mmc1复位 */
        unsigned int  periph_rsten0_mmc2        : 1;  /* bit[2]   : mmc2复位 */
        unsigned int  periph_rsten0_nandc       : 1;  /* bit[3]   : nandc复位 */
        unsigned int  periph_rsten0_usbotg_bus  : 1;  /* bit[4]   : usbotg总线复位 */
        unsigned int  periph_rsten0_por_picophy : 1;  /* bit[5]   : picophy 上电复位 */
        unsigned int  periph_rsten0_usbotg      : 1;  /* bit[6]   : usbotg工作时钟 */
        unsigned int  periph_rsten0_usbotg_32k  : 1;  /* bit[7]   : usbotg 32k时钟复位 */
        unsigned int  reserved                  : 24; /* bit[8-31]: 复位使能定义如下（写1复位使能）。
                                                                    保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_mmc0_START         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_mmc0_END           (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_mmc1_START         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_mmc1_END           (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_mmc2_START         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_mmc2_END           (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_nandc_START        (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_nandc_END          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_usbotg_bus_START   (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_usbotg_bus_END     (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_por_picophy_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_por_picophy_END    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_usbotg_START       (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_usbotg_END         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_usbotg_32k_START   (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN0_periph_rsten0_usbotg_32k_END     (7)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_UNION
 结构说明  : SC_PERIPH_RSTDIS0 寄存器结构定义。地址偏移量:0x304，初值:0x00000000，宽度:32
 寄存器说明: 外设复位禁止寄存器0。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rstdis0_mmc0        : 1;  /* bit[0]   : mmc0复位 */
        unsigned int  periph_rstdis0_mmc1        : 1;  /* bit[1]   : mmc1复位 */
        unsigned int  periph_rstdis0_mmc2        : 1;  /* bit[2]   : mmc2复位 */
        unsigned int  periph_rstdis0_nandc       : 1;  /* bit[3]   : nandc复位 */
        unsigned int  periph_rstdis0_usbotg_bus  : 1;  /* bit[4]   : usbotg总线复位 */
        unsigned int  periph_rstdis0_por_picophy : 1;  /* bit[5]   : picophy 上电复位 */
        unsigned int  periph_rstdis0_usbotg      : 1;  /* bit[6]   : usbotg工作时钟 */
        unsigned int  periph_rstdis0_usbotg_32k  : 1;  /* bit[7]   : usbotg 32k时钟复位 */
        unsigned int  reserved                   : 24; /* bit[8-31]: 复位禁止定义如下（写1复位解除）。
                                                                     保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_mmc0_START         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_mmc0_END           (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_mmc1_START         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_mmc1_END           (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_mmc2_START         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_mmc2_END           (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_nandc_START        (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_nandc_END          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_usbotg_bus_START   (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_usbotg_bus_END     (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_por_picophy_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_por_picophy_END    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_usbotg_START       (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_usbotg_END         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_usbotg_32k_START   (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS0_periph_rstdis0_usbotg_32k_END     (7)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_UNION
 结构说明  : SC_PERIPH_RSTSTAT0 寄存器结构定义。地址偏移量:0x308，初值:0xFFFFFFF7，宽度:32
 寄存器说明: 外设复位状态寄存器0。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。1为复位，0为非复位状态。默认CARM子系统处于解复位状态，AARM、ZSP、HIFI、BBP子系统处于复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rststat0_mmc0        : 1;  /* bit[0]   : mmc0复位 */
        unsigned int  periph_rststat0_mmc1        : 1;  /* bit[1]   : mmc1复位 */
        unsigned int  periph_rststat0_mmc2        : 1;  /* bit[2]   : mmc2复位 */
        unsigned int  periph_rststat0_nandc       : 1;  /* bit[3]   : nandc复位 */
        unsigned int  periph_rststat0_usbotg_bus  : 1;  /* bit[4]   : usbotg总线复位 */
        unsigned int  periph_rststat0_por_picophy : 1;  /* bit[5]   : picophy 上电复位 */
        unsigned int  periph_rststat0_usbotg      : 1;  /* bit[6]   : usbotg工作时钟 */
        unsigned int  periph_rststat0_usbotg_32k  : 1;  /* bit[7]   : usbotg 32k时钟复位 */
        unsigned int  reserved                    : 24; /* bit[8-31]: 复位状态定义如下（高电平表示处于复位）
                                                                      保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_mmc0_START         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_mmc0_END           (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_mmc1_START         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_mmc1_END           (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_mmc2_START         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_mmc2_END           (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_nandc_START        (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_nandc_END          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_usbotg_bus_START   (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_usbotg_bus_END     (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_por_picophy_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_por_picophy_END    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_usbotg_START       (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_usbotg_END         (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_usbotg_32k_START   (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT0_periph_rststat0_usbotg_32k_END     (7)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTEN1_UNION
 结构说明  : SC_PERIPH_RSTEN1 寄存器结构定义。地址偏移量:0x310，初值:0x00000000，宽度:32
 寄存器说明: 设复位使能寄存器1。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rsten1_hifi      : 1;  /* bit[0]   : hifi复位 */
        unsigned int  reserved_0              : 1;  /* bit[1]   : reserved */
        unsigned int  reserved_1              : 1;  /* bit[2]   : reserved */
        unsigned int  reserved_2              : 1;  /* bit[3]   : reserved */
        unsigned int  reserved_3              : 1;  /* bit[4]   : reserved */
        unsigned int  periph_rsten1_digacodec : 1;  /* bit[5]   : digital codec复位 */
        unsigned int  reserved_4              : 26; /* bit[6-31]: 复位使能定义如下（写1复位使能）。
                                                                  保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTEN1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN1_periph_rsten1_hifi_START       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN1_periph_rsten1_hifi_END         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN1_periph_rsten1_digacodec_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN1_periph_rsten1_digacodec_END    (5)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTDIS1_UNION
 结构说明  : SC_PERIPH_RSTDIS1 寄存器结构定义。地址偏移量:0x314，初值:0x00000000，宽度:32
 寄存器说明: 外设复位禁止寄存器1。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rstdis1_hifi      : 1;  /* bit[0]   : hifi复位 */
        unsigned int  reserved_0               : 1;  /* bit[1]   : reserved */
        unsigned int  reserved_1               : 1;  /* bit[2]   : reserved */
        unsigned int  reserved_2               : 1;  /* bit[3]   : reserved */
        unsigned int  reserved_3               : 1;  /* bit[4]   : reserved */
        unsigned int  periph_rstdis1_digacodec : 1;  /* bit[5]   : digital codec复位 */
        unsigned int  reserved_4               : 26; /* bit[6-31]: 复位禁止定义如下（写1复位解除）。
                                                                   保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTDIS1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS1_periph_rstdis1_hifi_START       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS1_periph_rstdis1_hifi_END         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS1_periph_rstdis1_digacodec_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS1_periph_rstdis1_digacodec_END    (5)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT1_UNION
 结构说明  : SC_PERIPH_RSTSTAT1 寄存器结构定义。地址偏移量:0x318，初值:0xFFFFFFFF，宽度:32
 寄存器说明: 外设复位状态寄存器1。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。1为复位，0为非复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rststat1_hifi      : 1;  /* bit[0]   : hifi复位 */
        unsigned int  reserved_0                : 1;  /* bit[1]   : reserved */
        unsigned int  reserved_1                : 1;  /* bit[2]   : reserved */
        unsigned int  reserved_2                : 1;  /* bit[3]   : reserved */
        unsigned int  reserved_3                : 1;  /* bit[4]   : reserved */
        unsigned int  periph_rststat1_digacodec : 1;  /* bit[5]   : digital codec复位 */
        unsigned int  reserved_4                : 26; /* bit[6-31]: 复位状态定义如下（高电平表示处于复位）
                                                                    保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT1_periph_rststat1_hifi_START       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT1_periph_rststat1_hifi_END         (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT1_periph_rststat1_digacodec_START  (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT1_periph_rststat1_digacodec_END    (5)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_UNION
 结构说明  : SC_PERIPH_RSTEN2 寄存器结构定义。地址偏移量:0x320，初值:0x00000000，宽度:32
 寄存器说明: 设复位使能寄存器2。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rsten2_ipf    : 1;  /* bit[0]   : ipf复位 */
        unsigned int  periph_rsten2_socp   : 1;  /* bit[1]   : socp复位 */
        unsigned int  periph_rsten2_dmac   : 1;  /* bit[2]   : dmac复位 */
        unsigned int  periph_rsten2_seceng : 1;  /* bit[3]   : seceng复位 */
        unsigned int  periph_rsten2_abb    : 1;  /* bit[4]   : abb复位 */
        unsigned int  periph_rsten2_hpm0   : 1;  /* bit[5]   : hpm0复位 */
        unsigned int  periph_rsten2_hpm1   : 1;  /* bit[6]   : hpm1复位 */
        unsigned int  periph_rsten2_hpm2   : 1;  /* bit[7]   : hpm2复位 */
        unsigned int  periph_rsten2_hpm3   : 1;  /* bit[8]   : hpm3复位 */
        unsigned int  reserved             : 23; /* bit[9-31]: 复位使能定义如下（写1复位使能）。
                                                               保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_ipf_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_ipf_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_socp_START    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_socp_END      (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_dmac_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_dmac_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_seceng_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_seceng_END    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_abb_START     (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_abb_END       (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_hpm0_START    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_hpm0_END      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_hpm1_START    (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_hpm1_END      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_hpm2_START    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_hpm2_END      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_hpm3_START    (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN2_periph_rsten2_hpm3_END      (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_UNION
 结构说明  : SC_PERIPH_RSTDIS2 寄存器结构定义。地址偏移量:0x324，初值:0x00000000，宽度:32
 寄存器说明: 外设复位禁止寄存器2。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rstdis2_ipf    : 1;  /* bit[0]   : ipf复位 */
        unsigned int  periph_rstdis2_socp   : 1;  /* bit[1]   : socp复位 */
        unsigned int  periph_rstdis2_dmac   : 1;  /* bit[2]   : dmac复位 */
        unsigned int  periph_rstdis2_seceng : 1;  /* bit[3]   : seceng复位 */
        unsigned int  periph_rstdis2_abb    : 1;  /* bit[4]   : abb复位 */
        unsigned int  periph_rstdis2_hpm0   : 1;  /* bit[5]   : hpm0复位 */
        unsigned int  periph_rstdis2_hpm1   : 1;  /* bit[6]   : hpm1复位 */
        unsigned int  periph_rstdis2_hpm2   : 1;  /* bit[7]   : hpm2复位 */
        unsigned int  periph_rstdis2_hpm3   : 1;  /* bit[8]   : hpm3复位 */
        unsigned int  reserved              : 23; /* bit[9-31]: 复位使能定义如下（写1复位解除）。
                                                                保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_ipf_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_ipf_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_socp_START    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_socp_END      (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_dmac_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_dmac_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_seceng_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_seceng_END    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_abb_START     (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_abb_END       (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_hpm0_START    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_hpm0_END      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_hpm1_START    (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_hpm1_END      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_hpm2_START    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_hpm2_END      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_hpm3_START    (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS2_periph_rstdis2_hpm3_END      (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_UNION
 结构说明  : SC_PERIPH_RSTSTAT2 寄存器结构定义。地址偏移量:0x328，初值:0xFFFFFFF7，宽度:32
 寄存器说明: 外设复位状态寄存器2。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。1为复位，0为非复位状态。默认CARM子系统处于解复位状态，AARM、ZSP、HIFI、BBP子系统处于复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rststat2_ipf    : 1;  /* bit[0]   : ipf复位 */
        unsigned int  periph_rststat2_socp   : 1;  /* bit[1]   : socp复位 */
        unsigned int  periph_rststat2_dmac   : 1;  /* bit[2]   : dmac复位 */
        unsigned int  periph_rststat2_seceng : 1;  /* bit[3]   : seceng复位 */
        unsigned int  periph_rststat2_abb    : 1;  /* bit[4]   : abb复位 */
        unsigned int  periph_rststat2_hpm0   : 1;  /* bit[5]   : hpm0复位 */
        unsigned int  periph_rststat2_hpm1   : 1;  /* bit[6]   : hpm1复位 */
        unsigned int  periph_rststat2_hpm2   : 1;  /* bit[7]   : hpm2复位 */
        unsigned int  periph_rststat2_hpm3   : 1;  /* bit[8]   : hpm3复位 */
        unsigned int  reserved               : 23; /* bit[9-31]: 复位状态定义如下（高电平表示处于复位）
                                                                 保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_ipf_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_ipf_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_socp_START    (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_socp_END      (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_dmac_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_dmac_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_seceng_START  (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_seceng_END    (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_abb_START     (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_abb_END       (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_hpm0_START    (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_hpm0_END      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_hpm1_START    (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_hpm1_END      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_hpm2_START    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_hpm2_END      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_hpm3_START    (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT2_periph_rststat2_hpm3_END      (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_UNION
 结构说明  : SC_PERIPH_RSTEN3 寄存器结构定义。地址偏移量:0x330，初值:0x00000000，宽度:32
 寄存器说明: 设复位使能寄存器3。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rsten3_cssys     : 1;  /* bit[0]    : coresight atb tpiu复位 */
        unsigned int  periph_rsten3_i2c0      : 1;  /* bit[1]    : i2c0复位 */
        unsigned int  periph_rsten3_i2c1      : 1;  /* bit[2]    : i2c1复位 */
        unsigned int  periph_rsten3_i2c2      : 1;  /* bit[3]    : i2c2复位 */
        unsigned int  periph_rsten3_i2c3      : 1;  /* bit[4]    : i2c3复位 */
        unsigned int  periph_rsten3_uart1     : 1;  /* bit[5]    : uart1复位 */
        unsigned int  periph_rsten3_uart2     : 1;  /* bit[6]    : uart2复位 */
        unsigned int  periph_rsten3_uart3     : 1;  /* bit[7]    : uart3复位 */
        unsigned int  periph_rsten3_uart4     : 1;  /* bit[8]    : uart4复位 */
        unsigned int  periph_rsten3_ssp       : 1;  /* bit[9]    : ssp复位 */
        unsigned int  periph_rsten3_pwm       : 1;  /* bit[10]   : pwm复位 */
        unsigned int  periph_rsten3_blpwm     : 1;  /* bit[11]   : 背光pwm复位 */
        unsigned int  periph_rsten3_tsensor   : 1;  /* bit[12]   : tsensor复位 */
        unsigned int  reserved_0              : 1;  /* bit[13]   : reserved */
        unsigned int  reserved_1              : 1;  /* bit[14]   : reserved */
        unsigned int  reserved_2              : 1;  /* bit[15]   : reserved */
        unsigned int  reserved_3              : 1;  /* bit[16]   : reserved */
        unsigned int  reserved_4              : 1;  /* bit[17]   : reserved */
        unsigned int  periph_rsten3_dapb      : 1;  /* bit[18]   : dapb复位 */
        unsigned int  periph_rsten3_hkadc     : 1;  /* bit[19]   : hkadc复位 */
        unsigned int  periph_rsten3_codec_ssi : 1;  /* bit[20]   : codec ssi 复位 */
        unsigned int  reserved_5              : 11; /* bit[21-31]: 复位禁止定义如下（写1复位解除）。
                                                                   保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_cssys_START      (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_cssys_END        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_i2c0_START       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_i2c0_END         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_i2c1_START       (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_i2c1_END         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_i2c2_START       (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_i2c2_END         (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_i2c3_START       (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_i2c3_END         (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_uart1_START      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_uart1_END        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_uart2_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_uart2_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_uart3_START      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_uart3_END        (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_uart4_START      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_uart4_END        (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_ssp_START        (9)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_ssp_END          (9)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_pwm_START        (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_pwm_END          (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_blpwm_START      (11)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_blpwm_END        (11)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_tsensor_START    (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_tsensor_END      (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_dapb_START       (18)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_dapb_END         (18)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_hkadc_START      (19)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_hkadc_END        (19)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_codec_ssi_START  (20)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN3_periph_rsten3_codec_ssi_END    (20)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_UNION
 结构说明  : SC_PERIPH_RSTDIS3 寄存器结构定义。地址偏移量:0x334，初值:0x00000000，宽度:32
 寄存器说明: 外设复位禁止寄存器3。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rstdis3_cssys     : 1;  /* bit[0]    : coresight sys复位 */
        unsigned int  periph_rstdis3_i2c0      : 1;  /* bit[1]    : i2c0复位 */
        unsigned int  periph_rstdis3_i2c1      : 1;  /* bit[2]    : i2c1复位 */
        unsigned int  periph_rstdis3_i2c2      : 1;  /* bit[3]    : i2c2复位 */
        unsigned int  periph_rstdis3_i2c3      : 1;  /* bit[4]    : i2c3复位 */
        unsigned int  periph_rstdis3_uart1     : 1;  /* bit[5]    : uart1复位 */
        unsigned int  periph_rstdis3_uart2     : 1;  /* bit[6]    : uart2复位 */
        unsigned int  periph_rstdis3_uart3     : 1;  /* bit[7]    : uart3复位 */
        unsigned int  periph_rstdis3_uart4     : 1;  /* bit[8]    : uart4复位 */
        unsigned int  periph_rstdis3_ssp       : 1;  /* bit[9]    : ssp复位 */
        unsigned int  periph_rstdis3_pwm       : 1;  /* bit[10]   : pwm复位 */
        unsigned int  periph_rstdis3_blpwm     : 1;  /* bit[11]   : 背光pwm复位 */
        unsigned int  periph_rstdis3_tsensor   : 1;  /* bit[12]   : tsensor复位 */
        unsigned int  reserved_0               : 1;  /* bit[13]   : reserved */
        unsigned int  reserved_1               : 1;  /* bit[14]   : reserved */
        unsigned int  reserved_2               : 1;  /* bit[15]   : reserved */
        unsigned int  reserved_3               : 1;  /* bit[16]   : reserved */
        unsigned int  reserved_4               : 1;  /* bit[17]   : reserved */
        unsigned int  periph_rstdis3_dapb      : 1;  /* bit[18]   : dapb复位 */
        unsigned int  periph_rstdis3_hkadc     : 1;  /* bit[19]   : hkadc复位 */
        unsigned int  periph_rstdis3_codec_ssi : 1;  /* bit[20]   : codec ssi 复位 */
        unsigned int  reserved_5               : 11; /* bit[21-31]: 复位禁止定义如下（写1复位解除）。
                                                                    保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_cssys_START      (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_cssys_END        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_i2c0_START       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_i2c0_END         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_i2c1_START       (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_i2c1_END         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_i2c2_START       (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_i2c2_END         (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_i2c3_START       (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_i2c3_END         (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_uart1_START      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_uart1_END        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_uart2_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_uart2_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_uart3_START      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_uart3_END        (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_uart4_START      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_uart4_END        (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_ssp_START        (9)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_ssp_END          (9)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_pwm_START        (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_pwm_END          (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_blpwm_START      (11)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_blpwm_END        (11)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_tsensor_START    (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_tsensor_END      (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_dapb_START       (18)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_dapb_END         (18)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_hkadc_START      (19)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_hkadc_END        (19)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_codec_ssi_START  (20)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS3_periph_rstdis3_codec_ssi_END    (20)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_UNION
 结构说明  : SC_PERIPH_RSTSTAT3 寄存器结构定义。地址偏移量:0x338，初值:0xFFFBFFFE，宽度:32
 寄存器说明: 外设复位状态寄存器3。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。1为复位，0为非复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rststat3_cssys     : 1;  /* bit[0]    : coresight sys复位 */
        unsigned int  periph_rststat3_i2c0      : 1;  /* bit[1]    : i2c0复位 */
        unsigned int  periph_rststat3_i2c1      : 1;  /* bit[2]    : i2c1复位 */
        unsigned int  periph_rststat3_i2c2      : 1;  /* bit[3]    : i2c2复位 */
        unsigned int  periph_rststat3_i2c3      : 1;  /* bit[4]    : i2c3复位 */
        unsigned int  periph_rststat3_uart1     : 1;  /* bit[5]    : uart1复位 */
        unsigned int  periph_rststat3_uart2     : 1;  /* bit[6]    : uart2复位 */
        unsigned int  periph_rststat3_uart3     : 1;  /* bit[7]    : uart3复位 */
        unsigned int  periph_rststat3_uart4     : 1;  /* bit[8]    : uart4复位 */
        unsigned int  periph_rststat3_ssp       : 1;  /* bit[9]    : ssp复位 */
        unsigned int  periph_rststat3_pwm       : 1;  /* bit[10]   : pwm复位 */
        unsigned int  periph_rststat3_blpwm     : 1;  /* bit[11]   : 背光pwm复位 */
        unsigned int  periph_rststat3_tsensor   : 1;  /* bit[12]   : tsensor复位 */
        unsigned int  reserved_0                : 1;  /* bit[13]   : reserved */
        unsigned int  reserved_1                : 1;  /* bit[14]   : reserved */
        unsigned int  reserved_2                : 1;  /* bit[15]   : reserved */
        unsigned int  reserved_3                : 1;  /* bit[16]   : reserved */
        unsigned int  reserved_4                : 1;  /* bit[17]   : reserved */
        unsigned int  periph_rststat3_dapb      : 1;  /* bit[18]   : dapb复位 */
        unsigned int  periph_rststat3_hkadc     : 1;  /* bit[19]   : hkadc复位 */
        unsigned int  periph_rststat3_codec_ssi : 1;  /* bit[20]   : codec ssi 复位 */
        unsigned int  reserved_5                : 11; /* bit[21-31]: 复位状态定义如下（高电平表示处于复位）
                                                                     保留 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_cssys_START      (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_cssys_END        (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_i2c0_START       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_i2c0_END         (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_i2c1_START       (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_i2c1_END         (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_i2c2_START       (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_i2c2_END         (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_i2c3_START       (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_i2c3_END         (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_uart1_START      (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_uart1_END        (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_uart2_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_uart2_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_uart3_START      (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_uart3_END        (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_uart4_START      (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_uart4_END        (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_ssp_START        (9)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_ssp_END          (9)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_pwm_START        (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_pwm_END          (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_blpwm_START      (11)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_blpwm_END        (11)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_tsensor_START    (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_tsensor_END      (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_dapb_START       (18)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_dapb_END         (18)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_hkadc_START      (19)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_hkadc_END        (19)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_codec_ssi_START  (20)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT3_periph_rststat3_codec_ssi_END    (20)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_UNION
 结构说明  : SC_PERIPH_RSTEN8 寄存器结构定义。地址偏移量:0x340，初值:0x00000000，宽度:32
 寄存器说明: 设复位使能寄存器6。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rsten8_rs0         : 1;  /* bit[0]    : rs0复位 */
        unsigned int  periph_rsten8_rs2         : 1;  /* bit[1]    : rs2复位 */
        unsigned int  periph_rsten8_rs3         : 1;  /* bit[2]    : rs3复位 */
        unsigned int  periph_rsten8_ms0         : 1;  /* bit[3]    : ms0复位 */
        unsigned int  reserved_0                : 1;  /* bit[4]    : reserved */
        unsigned int  periph_rsten8_ms2         : 1;  /* bit[5]    : ms2复位 */
        unsigned int  periph_rsten8_xg2ram0     : 1;  /* bit[6]    : xg2ram0复位 */
        unsigned int  periph_rsten8_x2sram_tzma : 1;  /* bit[7]    : x2sram和tzma复位 */
        unsigned int  periph_rsten8_sram        : 1;  /* bit[8]    : sram复位 */
        unsigned int  reserved_1                : 1;  /* bit[9]    : reserved */
        unsigned int  periph_rsten8_harq        : 1;  /* bit[10]   : harq复位 */
        unsigned int  reserved_2                : 1;  /* bit[11]   : reserved */
        unsigned int  periph_rsten8_ddrc        : 1;  /* bit[12]   : ddr复位 */
        unsigned int  periph_rsten8_ddrc_apb    : 1;  /* bit[13]   : ddrc apb总线复位 */
        unsigned int  periph_rsten8_ddrpack_apb : 1;  /* bit[14]   : ddrpack apb总线复位 */
        unsigned int  reserved_3                : 1;  /* bit[15]   : reserved */
        unsigned int  reserved_4                : 1;  /* bit[16]   : reserved */
        unsigned int  periph_rsten8_ddrt        : 1;  /* bit[17]   : ddrt复位 */
        unsigned int  reserved_5                : 14; /* bit[18-31]: 复位使能定义如下（写1复位使能）。
                                                                     MDM子系统中的AXI总线复位 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_rs0_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_rs0_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_rs2_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_rs2_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_rs3_START          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_rs3_END            (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ms0_START          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ms0_END            (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ms2_START          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ms2_END            (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_xg2ram0_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_xg2ram0_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_x2sram_tzma_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_x2sram_tzma_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_sram_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_sram_END           (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_harq_START         (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_harq_END           (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ddrc_START         (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ddrc_END           (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ddrc_apb_START     (13)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ddrc_apb_END       (13)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ddrpack_apb_START  (14)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ddrpack_apb_END    (14)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ddrt_START         (17)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN8_periph_rsten8_ddrt_END           (17)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_UNION
 结构说明  : SC_PERIPH_RSTDIS8 寄存器结构定义。地址偏移量:0x344，初值:0x00000000，宽度:32
 寄存器说明: 外设复位禁止寄存器6。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rstdis8_rs0         : 1;  /* bit[0]    : rs0复位 */
        unsigned int  periph_rstdis8_rs2         : 1;  /* bit[1]    : rs2复位 */
        unsigned int  periph_rstdis8_rs3         : 1;  /* bit[2]    : rs3复位 */
        unsigned int  periph_rstdis8_ms0         : 1;  /* bit[3]    : ms0复位 */
        unsigned int  reserved_0                 : 1;  /* bit[4]    : reserved */
        unsigned int  periph_rstdis8_ms2         : 1;  /* bit[5]    : ms2复位 */
        unsigned int  periph_rstdis8_xg2ram0     : 1;  /* bit[6]    : xg2ram0复位 */
        unsigned int  periph_rstdis8_x2sram_tzma : 1;  /* bit[7]    : x2sram和tzma复位 */
        unsigned int  periph_rstdis8_sram        : 1;  /* bit[8]    : sram复位 */
        unsigned int  reserved_1                 : 1;  /* bit[9]    : reserved */
        unsigned int  periph_rstdis8_harq        : 1;  /* bit[10]   : harq复位 */
        unsigned int  reserved_2                 : 1;  /* bit[11]   : reserved */
        unsigned int  periph_rstdis8_ddrc        : 1;  /* bit[12]   : ddr复位 */
        unsigned int  periph_rstdis8_ddrc_apb    : 1;  /* bit[13]   : ddrc apb总线复位 */
        unsigned int  periph_rstdis8_ddrpack_apb : 1;  /* bit[14]   : ddrpack apb总线复位 */
        unsigned int  reserved_3                 : 1;  /* bit[15]   : reserved */
        unsigned int  reserved_4                 : 1;  /* bit[16]   : reserved */
        unsigned int  periph_rstdis8_ddrt        : 1;  /* bit[17]   : ddrt复位 */
        unsigned int  reserved_5                 : 14; /* bit[18-31]: 复位禁止定义如下（写1复位解除）。
                                                                      MDM子系统中的AXI总线复位 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_rs0_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_rs0_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_rs2_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_rs2_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_rs3_START          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_rs3_END            (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ms0_START          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ms0_END            (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ms2_START          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ms2_END            (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_xg2ram0_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_xg2ram0_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_x2sram_tzma_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_x2sram_tzma_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_sram_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_sram_END           (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_harq_START         (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_harq_END           (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ddrc_START         (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ddrc_END           (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ddrc_apb_START     (13)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ddrc_apb_END       (13)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ddrpack_apb_START  (14)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ddrpack_apb_END    (14)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ddrt_START         (17)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS8_periph_rstdis8_ddrt_END           (17)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_UNION
 结构说明  : SC_PERIPH_RSTSTAT8 寄存器结构定义。地址偏移量:0x348，初值:0xFFFFFA37，宽度:32
 寄存器说明: 外设复位状态寄存器6。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rststat8_rs0         : 1;  /* bit[0]    : rs0复位 */
        unsigned int  periph_rststat8_rs2         : 1;  /* bit[1]    : rs2复位 */
        unsigned int  periph_rststat8_rs3         : 1;  /* bit[2]    : rs3复位 */
        unsigned int  periph_rststat8_ms0         : 1;  /* bit[3]    : ms0复位 */
        unsigned int  reserved_0                  : 1;  /* bit[4]    : reserved */
        unsigned int  periph_rststat8_ms2         : 1;  /* bit[5]    : ms2复位 */
        unsigned int  periph_rststat8_xg2ram0     : 1;  /* bit[6]    : xg2ram0复位 */
        unsigned int  periph_rststat8_x2sram_tzma : 1;  /* bit[7]    : x2sram和tzma复位 */
        unsigned int  periph_rststat8_sram        : 1;  /* bit[8]    : sram复位 */
        unsigned int  reserved_1                  : 1;  /* bit[9]    : reserved */
        unsigned int  periph_rststat8_harq        : 1;  /* bit[10]   : harq复位 */
        unsigned int  reserved_2                  : 1;  /* bit[11]   : reserved */
        unsigned int  periph_rststat8_ddrc        : 1;  /* bit[12]   : ddr复位 */
        unsigned int  periph_rststat8_ddrc_apb    : 1;  /* bit[13]   : ddrc apb总线复位 */
        unsigned int  periph_rststat8_ddrpack_apb : 1;  /* bit[14]   : ddrpack apb总线复位 */
        unsigned int  reserved_3                  : 1;  /* bit[15]   : reserved */
        unsigned int  reserved_4                  : 1;  /* bit[16]   : reserved */
        unsigned int  periph_rststat8_ddrt        : 1;  /* bit[17]   : ddrt复位 */
        unsigned int  reserved_5                  : 14; /* bit[18-31]: 复位状态定义如下（高电平表示处于复位）
                                                                       MDM子系统中的AXI总线复位 */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_rs0_START          (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_rs0_END            (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_rs2_START          (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_rs2_END            (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_rs3_START          (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_rs3_END            (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ms0_START          (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ms0_END            (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ms2_START          (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ms2_END            (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_xg2ram0_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_xg2ram0_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_x2sram_tzma_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_x2sram_tzma_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_sram_START         (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_sram_END           (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_harq_START         (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_harq_END           (10)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ddrc_START         (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ddrc_END           (12)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ddrc_apb_START     (13)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ddrc_apb_END       (13)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ddrpack_apb_START  (14)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ddrpack_apb_END    (14)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ddrt_START         (17)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT8_periph_rststat8_ddrt_END           (17)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_UNION
 结构说明  : SC_PERIPH_RSTEN9 寄存器结构定义。地址偏移量:0x350，初值:0x00000000，宽度:32
 寄存器说明: 设复位使能寄存器6。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rsten9_carm_dap    : 1;  /* bit[0]   : carm debug apb复位 */
        unsigned int  periph_rsten9_carm_atb    : 1;  /* bit[1]   : carm atb复位 */
        unsigned int  periph_rsten9_carm_lbus   : 1;  /* bit[2]   : carm local bus复位 */
        unsigned int  periph_rsten9_carm_por    : 1;  /* bit[3]   : carm 全局复位 */
        unsigned int  periph_rsten9_carm_core   : 1;  /* bit[4]   : carm 内核复位 */
        unsigned int  periph_rsten9_carm_dbg    : 1;  /* bit[5]   : carm dbg复位 */
        unsigned int  periph_rsten9_carm_l2     : 1;  /* bit[6]   : carm scu复位 */
        unsigned int  periph_rsten9_carm_socdbg : 1;  /* bit[7]   : carm socdbg复位 */
        unsigned int  periph_rsten9_carm_etm    : 1;  /* bit[8]   : carm etm复位 */
        unsigned int  reserved                  : 23; /* bit[9-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_dap_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_dap_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_atb_START     (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_atb_END       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_lbus_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_lbus_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_por_START     (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_por_END       (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_core_START    (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_core_END      (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_dbg_START     (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_dbg_END       (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_l2_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_l2_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_socdbg_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_socdbg_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_etm_START     (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_etm_END       (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_UNION
 结构说明  : SC_PERIPH_RSTDIS9 寄存器结构定义。地址偏移量:0x354，初值:0x00000000，宽度:32
 寄存器说明: 外设复位禁止寄存器6。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rsdist9_carm_dap    : 1;  /* bit[0]   : carm debug apb复位 */
        unsigned int  periph_rsdist9_carm_atb    : 1;  /* bit[1]   : carm atb复位 */
        unsigned int  periph_rsdist9_carm_lbus   : 1;  /* bit[2]   : carm local bus复位 */
        unsigned int  periph_rsdist9_carm_por    : 1;  /* bit[3]   : carm 全局复位 */
        unsigned int  periph_rsdist9_carm_core   : 1;  /* bit[4]   : carm 内核复位 */
        unsigned int  periph_rsdist9_carm_dbg    : 1;  /* bit[5]   : carm dbg复位 */
        unsigned int  periph_rsdist9_carm_l2     : 1;  /* bit[6]   : carm scu复位 */
        unsigned int  periph_rsdist9_carm_socdbg : 1;  /* bit[7]   : carm socdbg复位 */
        unsigned int  periph_rsdist9_carm_etm    : 1;  /* bit[8]   : carm etm复位 */
        unsigned int  reserved                   : 23; /* bit[9-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dap_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dap_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_atb_START     (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_atb_END       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_lbus_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_lbus_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_por_START     (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_por_END       (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_core_START    (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_core_END      (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dbg_START     (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dbg_END       (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_l2_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_l2_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_socdbg_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_socdbg_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_etm_START     (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_etm_END       (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_UNION
 结构说明  : SC_PERIPH_RSTSTAT9 寄存器结构定义。地址偏移量:0x358，初值:0xFFFFFFFF，宽度:32
 寄存器说明: 外设复位状态寄存器6。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  periph_rststat9_carm_dap    : 1;  /* bit[0]   : carm debug apb复位 */
        unsigned int  periph_rststat9_carm_atb    : 1;  /* bit[1]   : carm atb复位 */
        unsigned int  periph_rststat9_carm_lbus   : 1;  /* bit[2]   : carm local bus复位 */
        unsigned int  periph_rststat9_carm_por    : 1;  /* bit[3]   : carm 全局复位 */
        unsigned int  periph_rststat9_carm_core   : 1;  /* bit[4]   : carm 内核复位 */
        unsigned int  periph_rststat9_carm_dbg    : 1;  /* bit[5]   : carm dbg复位 */
        unsigned int  periph_rststat9_carm_l2     : 1;  /* bit[6]   : carm scu复位 */
        unsigned int  periph_rststat9_carm_socdbg : 1;  /* bit[7]   : carm socdbg复位 */
        unsigned int  periph_rststat9_carm_etm    : 1;  /* bit[8]   : carm etm复位 */
        unsigned int  reserved                    : 23; /* bit[9-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_UNION;
#endif
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_dap_START     (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_dap_END       (0)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_atb_START     (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_atb_END       (1)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_lbus_START    (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_lbus_END      (2)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_por_START     (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_por_END       (3)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_core_START    (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_core_END      (4)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_dbg_START     (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_dbg_END       (5)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_l2_START      (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_l2_END        (6)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_socdbg_START  (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_socdbg_END    (7)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_etm_START     (8)
#define SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT9_periph_rststat9_carm_etm_END       (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTEN10_UNION
 结构说明  : SC_PERIPH_RSTEN10 寄存器结构定义。地址偏移量:0x360，初值:0x00000000，宽度:32
 寄存器说明: 设复位使能寄存器6。用于使能外设复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTEN10_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTDIS10_UNION
 结构说明  : SC_PERIPH_RSTDIS10 寄存器结构定义。地址偏移量:0x364，初值:0x00000000，宽度:32
 寄存器说明: 外设复位禁止寄存器6。用于禁止外设复位。对比特写1，对应外设解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTDIS10_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT10_UNION
 结构说明  : SC_PERIPH_RSTSTAT10 寄存器结构定义。地址偏移量:0x368，初值:0x00000000，宽度:32
 寄存器说明: 外设复位状态寄存器6。用于回读各外设复位的状态。外设复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_PERIPH_RSTSTAT10_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLK_SEL0_UNION
 结构说明  : SC_CLK_SEL0 寄存器结构定义。地址偏移量:0x400，初值:0x00000401，宽度:32
 寄存器说明: 时钟选择寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sel_clk_hifi      : 1;  /* bit[0]    : HIFI源时钟选择：
                                                             0：选择syspll时钟
                                                             1：选择mediapll时钟 */
        unsigned int  sel_clk_uart1     : 1;  /* bit[1]    : UART1源时钟选择：
                                                             0：选择TCXO时钟；
                                                             1：选择SYS时钟。 */
        unsigned int  sel_clk_uart2     : 1;  /* bit[2]    : UART2源时钟选择：
                                                             0：选择TCXO时钟；
                                                             1：选择SYS时钟。 */
        unsigned int  sel_clk_uart3     : 1;  /* bit[3]    : UART3源时钟选择：
                                                             0：选择TCXO时钟；
                                                             1：选择SYS时钟。 */
        unsigned int  sel_clk_uart4     : 1;  /* bit[4]    : UART4源时钟选择：
                                                             0：选择TCXO时钟；
                                                             1：选择SYS时钟。 */
        unsigned int  sel_clk_mmc       : 1;  /* bit[5]    : mmc移相时钟源选择：
                                                             0：DDRPLL；
                                                             1：SYSPLL。 */
        unsigned int  sel_pll_test      : 3;  /* bit[6-8]  : pll_test时钟选择：
                                                             000：clk_acpu_src
                                                             001：clk_ddrpll1_src
                                                             010：clk_hifi_src
                                                             011：clk_bbppll0_src
                                                             100：clk_mmc0_sel_src
                                                             101：clk_gpupll_src */
        unsigned int  sel_codec_ssi     : 1;  /* bit[9]    : codec 为了兼容V9 12.288MHz codec，作了时钟选择：
                                                             0：clk_slow 19.2MHz
                                                             1：clk_bbppll0_div5_src 12.288MHz */
        unsigned int  sel_clk_ccpu      : 1;  /* bit[10]   : 送给ccpu时钟源头选择：
                                                             1：syspll
                                                             0：mediapll */
        unsigned int  reserved_0        : 5;  /* bit[11-15]: 保留。 */
        unsigned int  sel_clk_hifi_msk  : 1;  /* bit[16]   : HIFI源时钟选择 */
        unsigned int  sel_clk_uart1_msk : 1;  /* bit[17]   : UART1源时钟选择： */
        unsigned int  sel_clk_uart2_msk : 1;  /* bit[18]   : UART2源时钟选择： */
        unsigned int  sel_clk_uart3_msk : 1;  /* bit[19]   : UART3源时钟选择： */
        unsigned int  sel_clk_uart4_msk : 1;  /* bit[20]   : UART4源时钟选择： */
        unsigned int  sel_clk_mmc_msk   : 1;  /* bit[21]   : mmc时钟选择 */
        unsigned int  sel_pll_test_msk  : 3;  /* bit[22-24]: pll_test选择： */
        unsigned int  sel_codec_ssi_msk : 1;  /* bit[25]   : codec 为了兼容V9 12.288MHz codec，作了时钟选择： */
        unsigned int  sel_clk_ccpu_msk  : 1;  /* bit[26]   : 送给ccpu时钟源头选择： */
        unsigned int  reserved_1        : 5;  /* bit[27-31]: 保留
                                                             [31：16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变； */
    } reg;
} SOC_PERI_SCTRL_SC_CLK_SEL0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_hifi_START       (0)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_hifi_END         (0)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart1_START      (1)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart1_END        (1)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart2_START      (2)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart2_END        (2)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart3_START      (3)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart3_END        (3)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart4_START      (4)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart4_END        (4)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_mmc_START        (5)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_mmc_END          (5)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_pll_test_START       (6)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_pll_test_END         (8)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_codec_ssi_START      (9)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_codec_ssi_END        (9)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_ccpu_START       (10)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_ccpu_END         (10)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_hifi_msk_START   (16)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_hifi_msk_END     (16)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart1_msk_START  (17)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart1_msk_END    (17)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart2_msk_START  (18)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart2_msk_END    (18)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart3_msk_START  (19)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart3_msk_END    (19)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart4_msk_START  (20)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_uart4_msk_END    (20)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_mmc_msk_START    (21)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_mmc_msk_END      (21)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_pll_test_msk_START   (22)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_pll_test_msk_END     (24)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_codec_ssi_msk_START  (25)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_codec_ssi_msk_END    (25)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_ccpu_msk_START   (26)
#define SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_ccpu_msk_END     (26)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLK_SEL1_UNION
 结构说明  : SC_CLK_SEL1 寄存器结构定义。地址偏移量:0x404，初值:0x00000000，宽度:32
 寄存器说明: 时钟选择寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 16; /* bit[0-15] : reserved */
        unsigned int  reserved_1: 16; /* bit[16-31]: 保留PLL测试点选择
                                                     [31：16]：每一bit顺序对应bit15～bit0，仅当bit31～16的bit写1的同时，配置bit15～0才能生效；当bit31～16的bit写0时，配置bit15～0保持不变； */
    } reg;
} SOC_PERI_SCTRL_SC_CLK_SEL1_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFGALL0_UNION
 结构说明  : SC_CLKCFGALL0 寄存器结构定义。地址偏移量:0x450，初值:0x0000001F，宽度:32
 寄存器说明: 时钟参数配置寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: [32：31]：reserved
                                                   [30：24]： clk_hifi_src分频比配置 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFGALL0_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFGALL1_UNION
 结构说明  : SC_CLKCFGALL1 寄存器结构定义。地址偏移量:0x454，初值:0x0000001F，宽度:32
 寄存器说明: 时钟参数配置寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 时钟参数配置寄存器。
                                                   [31：24]：reserved */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFGALL1_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFGALL2_UNION
 结构说明  : SC_CLKCFGALL2 寄存器结构定义。地址偏移量:0x458，初值:0x0003071F，宽度:32
 寄存器说明: 时钟参数配置寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 6;  /* bit[0-5]  : [5：0]：SIO2XFS分频参数配置； */
        unsigned int  reserved_1: 2;  /* bit[6-7]  : reserved */
        unsigned int  reserved_2: 8;  /* bit[8-15] : [15：8]：SIO2时钟分频参数配置； */
        unsigned int  reserved_3: 1;  /* bit[16]   : [16]：SIO2Master模式下I2S或者PCM模式选择：
                                                     0：I2S格式；
                                                     1：PCM格式。 */
        unsigned int  reserved_4: 1;  /* bit[17]   : [17]：SIO2Master模式选择：
                                                     0：Master模式；
                                                     1：Slave模式。 */
        unsigned int  reserved_5: 14; /* bit[18-31]: 时钟参数配置寄存器。
                                                     [31：18]：reserved */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFGALL2_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFGALL3_UNION
 结构说明  : SC_CLKCFGALL3 寄存器结构定义。地址偏移量:0x45C，初值:0x0003071F，宽度:32
 寄存器说明: 时钟参数配置寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 6;  /* bit[0-5]  : [5：0]：SIO3XFS分频参数配置； */
        unsigned int  reserved_1: 2;  /* bit[6-7]  : reserved */
        unsigned int  reserved_2: 8;  /* bit[8-15] : [15：8]：SIO3时钟分频参数配置； */
        unsigned int  reserved_3: 1;  /* bit[16]   : [16]：SIO2Master模式下I2S或者PCM模式选择：
                                                     0：I2S格式；
                                                     1：PCM格式。 */
        unsigned int  reserved_4: 1;  /* bit[17]   : [17]：SIO2Master模式选择：
                                                     0：Master模式；
                                                     1：Slave模式。 */
        unsigned int  reserved_5: 14; /* bit[18-31]: 时钟参数配置寄存器。
                                                     [31：18]：保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFGALL3_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFGALL4_UNION
 结构说明  : SC_CLKCFGALL4 寄存器结构定义。地址偏移量:0x460，初值:0x00000000，宽度:32
 寄存器说明: 时钟参数配置寄存器4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFGALL4_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFGALL5_UNION
 结构说明  : SC_CLKCFGALL5 寄存器结构定义。地址偏移量:0x464，初值:0x00000000，宽度:32
 寄存器说明: 时钟参数配置寄存器5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFGALL5_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT0_UNION
 结构说明  : SC_CLKCFG8BIT0 寄存器结构定义。地址偏移量:0x490，初值:0x01020102，宽度:32
 寄存器说明: 时钟参数配置寄存器7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg0_value0_clkbus_low       : 7;  /* bit[0-6]  : 时钟参数。（源头为300MHz时钟）
                                                                                 [6：4]：保留
                                                                                 [3：0]：CLKBUS_low时钟的分频参数配置;（详情参考PMCTRL用户手册的sysnoc dfs章节） */
        unsigned int  clk_8bit_cfg0_vld0_clkbus_low         : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg0_value0_clkbus_high      : 7;  /* bit[8-14] : 时钟参数。（源头为300MHz时钟）
                                                                                 [14：12]：保留
                                                                                 [11：8]：CLKBUS_high时钟的分频参数配置;（详情参考PMCTRL用户手册的sysnoc dfs章节） */
        unsigned int  clk_8bit_cfg0_vld0_clkbus_high        : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg0_value0_clkbus_idle_low  : 7;  /* bit[16-22]: 时钟参数。（源头为300MHz时钟）
                                                                                 [22：20]：保留
                                                                                 [19：16]：noc idle下CLKBUS_low时钟的分频参数配置;（详情参考PMCTRL用户手册的sysnoc dfs章节） */
        unsigned int  clk_8bit_cfg0_vld0_clkbus_idle_low    : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg0_value0_clkbus_idle_high : 7;  /* bit[24-30]: 时钟参数。（源头为300MHz时钟）
                                                                                 [30：28]：保留
                                                                                 [27：24]：noc idle下CLKBUS_high时钟的分频参数配置;（详情参考PMCTRL用户手册的sysnoc dfs章节） */
        unsigned int  clk_8bit_cfg0_vld0_clkbus_idle_high   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_value0_clkbus_low_START        (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_value0_clkbus_low_END          (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_vld0_clkbus_low_START          (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_vld0_clkbus_low_END            (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_value0_clkbus_high_START       (8)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_value0_clkbus_high_END         (14)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_vld0_clkbus_high_START         (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_vld0_clkbus_high_END           (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_value0_clkbus_idle_low_START   (16)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_value0_clkbus_idle_low_END     (22)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_vld0_clkbus_idle_low_START     (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_vld0_clkbus_idle_low_END       (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_value0_clkbus_idle_high_START  (24)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_value0_clkbus_idle_high_END    (30)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_vld0_clkbus_idle_high_START    (31)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT0_clk_8bit_cfg0_vld0_clkbus_idle_high_END      (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT1_UNION
 结构说明  : SC_CLKCFG8BIT1 寄存器结构定义。地址偏移量:0x494，初值:0x0000007F，宽度:32
 寄存器说明: mmc0时钟参数配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg1_value0_mmc0 : 7;  /* bit[0-6]  : 时钟参数。
                                                                     [6：6]：保留
                                                                     [5：0]：CLK MMC0 时钟的分频参数配置; */
        unsigned int  clk_8bit_cfg1_vld0_mmc0   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  reserved_0                : 7;  /* bit[8-14] : 时钟参数。 */
        unsigned int  reserved_1                : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  reserved_2                : 7;  /* bit[16-22]: 时钟参数。保留。 */
        unsigned int  reserved_3                : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  reserved_4                : 7;  /* bit[24-30]: 时钟参数。保留。 */
        unsigned int  reserved_5                : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT1_clk_8bit_cfg1_value0_mmc0_START  (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT1_clk_8bit_cfg1_value0_mmc0_END    (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT1_clk_8bit_cfg1_vld0_mmc0_START    (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT1_clk_8bit_cfg1_vld0_mmc0_END      (7)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT2_UNION
 结构说明  : SC_CLKCFG8BIT2 寄存器结构定义。地址偏移量:0x498，初值:0x0000007F，宽度:32
 寄存器说明: mmc1时钟参数配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg2_value0_mmc1 : 7;  /* bit[0-6]  : 时钟参数。
                                                                     [6：6]：保留
                                                                     [5：0]：CLK MMC1 LOW时钟的分频参数配置; */
        unsigned int  clk_8bit_cfg2_vld0_mmc1   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  reserved_0                : 7;  /* bit[8-14] : 时钟参数。
                                                                     [14：12]：保留
                                                                     [11：8]：CLK MMC1 HIGH时钟的分频参数配置; */
        unsigned int  reserved_1                : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  reserved_2                : 7;  /* bit[16-22]: 时钟参数。保留。 */
        unsigned int  reserved_3                : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  reserved_4                : 7;  /* bit[24-30]: 时钟参数。保留。 */
        unsigned int  reserved_5                : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT2_clk_8bit_cfg2_value0_mmc1_START  (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT2_clk_8bit_cfg2_value0_mmc1_END    (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT2_clk_8bit_cfg2_vld0_mmc1_START    (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT2_clk_8bit_cfg2_vld0_mmc1_END      (7)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT3_UNION
 结构说明  : SC_CLKCFG8BIT3 寄存器结构定义。地址偏移量:0x49C，初值:0x0000007F，宽度:32
 寄存器说明: mmc2时钟参数配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg3_value0_mmc2 : 7;  /* bit[0-6]  : 时钟参数。
                                                                     [6：6]：保留
                                                                     [5：0]：CLK MMC2 LOW时钟的分频参数配置; */
        unsigned int  clk_8bit_cfg3_vld0_mmc2   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  reserved_0                : 7;  /* bit[8-14] : 时钟参数。
                                                                     [14：12]：保留
                                                                     [11：8]：CLK MMC2 HIGH时钟的分频参数配置; */
        unsigned int  reserved_1                : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  reserved_2                : 7;  /* bit[16-22]: 时钟参数。保留。 */
        unsigned int  reserved_3                : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  reserved_4                : 7;  /* bit[24-30]: 时钟参数。保留。 */
        unsigned int  reserved_5                : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT3_clk_8bit_cfg3_value0_mmc2_START  (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT3_clk_8bit_cfg3_value0_mmc2_END    (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT3_clk_8bit_cfg3_vld0_mmc2_START    (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT3_clk_8bit_cfg3_vld0_mmc2_END      (7)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT4_UNION
 结构说明  : SC_CLKCFG8BIT4 寄存器结构定义。地址偏移量:0x4A0，初值:0x017F0403，宽度:32
 寄存器说明: 时钟参数配置寄存器4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg4_value0_hifi     : 7;  /* bit[0-6]  : 时钟参数。
                                                                         [6：4]：保留
                                                                         [3：0]：HIFI时钟的分频参数配置; */
        unsigned int  clk_8bit_cfg4_vld0_hifi       : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg4_value1_dacodec  : 7;  /* bit[8-14] : 时钟参数。
                                                                         [14：14]：保留
                                                                         [13：8]：digacodec 分频。
                                                                         FPGA：1分频
                                                                         ASIC：5分频 */
        unsigned int  clk_8bit_cfg4_vld1_dacodec    : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg4_value2_pll_test : 7;  /* bit[16-22]: 时钟参数。
                                                                         [22：22]：保留
                                                                         [21：16]：pll_test分频比配置 */
        unsigned int  clk_8bit_cfg4_vld2_pll_test   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg4_value3_dapb     : 7;  /* bit[24-30]: [31：26]保留
                                                                         [25：24]clk_dapb时钟分频 */
        unsigned int  clk_8bit_cfg4_vld3_dapb       : 1;  /* bit[31]   : 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT4_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_value0_hifi_START      (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_value0_hifi_END        (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_vld0_hifi_START        (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_vld0_hifi_END          (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_value1_dacodec_START   (8)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_value1_dacodec_END     (14)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_vld1_dacodec_START     (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_vld1_dacodec_END       (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_value2_pll_test_START  (16)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_value2_pll_test_END    (22)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_vld2_pll_test_START    (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_vld2_pll_test_END      (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_value3_dapb_START      (24)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_value3_dapb_END        (30)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_vld3_dapb_START        (31)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT4_clk_8bit_cfg4_vld3_dapb_END          (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT5_UNION
 结构说明  : SC_CLKCFG8BIT5 寄存器结构定义。地址偏移量:0x4A4，初值:0x7F7F7F7F，宽度:32
 寄存器说明: 时钟参数配置寄存器5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg5_value0 : 7;  /* bit[0-6]  : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg5_vld0   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg5_value1 : 7;  /* bit[8-14] : 时钟参数。保留。 */
        unsigned int  clk_8bit_cfg5_vld1   : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg5_value2 : 7;  /* bit[16-22]: 时钟参数。
                                                                [22]：保留
                                                                [21：16]：CLK_PLL_0测试时钟的分频参数配置; */
        unsigned int  clk_8bit_cfg5_vld2   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg5_value3 : 7;  /* bit[24-30]: 时钟参数。
                                                                [30]：保留
                                                                [29：24]：CLK_PLL_1测试时钟的分频参数配置; */
        unsigned int  clk_8bit_cfg5_vld3   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT5_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_value0_START  (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_value0_END    (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_vld0_START    (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_vld0_END      (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_value1_START  (8)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_value1_END    (14)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_vld1_START    (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_vld1_END      (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_value2_START  (16)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_value2_END    (22)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_vld2_START    (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_vld2_END      (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_value3_START  (24)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_value3_END    (30)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_vld3_START    (31)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT5_clk_8bit_cfg5_vld3_END      (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT6_UNION
 结构说明  : SC_CLKCFG8BIT6 寄存器结构定义。地址偏移量:0x4A8，初值:0x00000000，宽度:32
 寄存器说明: 时钟参数配置寄存器6
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg6_value0 : 7;  /* bit[0-6]  : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg6_vld0   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg6_value1 : 7;  /* bit[8-14] : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg6_vld1   : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg6_value2 : 7;  /* bit[16-22]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg6_vld2   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg6_value3 : 7;  /* bit[24-30]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg6_vld3   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT6_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_value0_START  (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_value0_END    (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_vld0_START    (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_vld0_END      (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_value1_START  (8)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_value1_END    (14)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_vld1_START    (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_vld1_END      (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_value2_START  (16)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_value2_END    (22)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_vld2_START    (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_vld2_END      (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_value3_START  (24)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_value3_END    (30)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_vld3_START    (31)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT6_clk_8bit_cfg6_vld3_END      (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT7_UNION
 结构说明  : SC_CLKCFG8BIT7 寄存器结构定义。地址偏移量:0x4AC，初值:0x00000000，宽度:32
 寄存器说明: 时钟参数配置寄存器7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg7_value0 : 7;  /* bit[0-6]  : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg7_vld0   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg7_value1 : 7;  /* bit[8-14] : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg7_vld1   : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg7_value2 : 7;  /* bit[16-22]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg7_vld2   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg7_value3 : 7;  /* bit[24-30]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg7_vld3   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT7_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_value0_START  (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_value0_END    (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_vld0_START    (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_vld0_END      (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_value1_START  (8)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_value1_END    (14)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_vld1_START    (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_vld1_END      (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_value2_START  (16)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_value2_END    (22)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_vld2_START    (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_vld2_END      (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_value3_START  (24)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_value3_END    (30)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_vld3_START    (31)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT7_clk_8bit_cfg7_vld3_END      (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT8_UNION
 结构说明  : SC_CLKCFG8BIT8 寄存器结构定义。地址偏移量:0x4B0，初值:0x00000000，宽度:32
 寄存器说明: 时钟参数配置寄存器8
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg8_value0 : 7;  /* bit[0-6]  : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg8_vld0   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg8_value1 : 7;  /* bit[8-14] : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg8_vld1   : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg8_value2 : 7;  /* bit[16-22]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg8_vld2   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg8_value3 : 7;  /* bit[24-30]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg8_vld3   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT8_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_value0_START  (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_value0_END    (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_vld0_START    (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_vld0_END      (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_value1_START  (8)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_value1_END    (14)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_vld1_START    (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_vld1_END      (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_value2_START  (16)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_value2_END    (22)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_vld2_START    (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_vld2_END      (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_value3_START  (24)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_value3_END    (30)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_vld3_START    (31)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT8_clk_8bit_cfg8_vld3_END      (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT9_UNION
 结构说明  : SC_CLKCFG8BIT9 寄存器结构定义。地址偏移量:0x4B4，初值:0x7F7F7F7F，宽度:32
 寄存器说明: 时钟参数配置寄存器16
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg9_value0 : 7;  /* bit[0-6]  : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg9_vld0   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg9_value1 : 7;  /* bit[8-14] : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg9_vld1   : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg9_value2 : 7;  /* bit[16-22]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg9_vld2   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg9_value3 : 7;  /* bit[24-30]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg9_vld3   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT9_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_value0_START  (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_value0_END    (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_vld0_START    (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_vld0_END      (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_value1_START  (8)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_value1_END    (14)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_vld1_START    (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_vld1_END      (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_value2_START  (16)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_value2_END    (22)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_vld2_START    (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_vld2_END      (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_value3_START  (24)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_value3_END    (30)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_vld3_START    (31)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT9_clk_8bit_cfg9_vld3_END      (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CLKCFG8BIT10_UNION
 结构说明  : SC_CLKCFG8BIT10 寄存器结构定义。地址偏移量:0x4B8，初值:0x7F7F7F7F，宽度:32
 寄存器说明: 时钟参数配置寄存器17
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg10_value0 : 7;  /* bit[0-6]  : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg10_vld0   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg10_value1 : 7;  /* bit[8-14] : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg10_vld1   : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg10_value2 : 7;  /* bit[16-22]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg10_vld2   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_8bit_cfg10_value3 : 7;  /* bit[24-30]: 时钟参数。保留 */
        unsigned int  clk_8bit_cfg10_vld3   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PERI_SCTRL_SC_CLKCFG8BIT10_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_value0_START  (0)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_value0_END    (6)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_vld0_START    (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_vld0_END      (7)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_value1_START  (8)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_value1_END    (14)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_vld1_START    (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_vld1_END      (15)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_value2_START  (16)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_value2_END    (22)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_vld2_START    (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_vld2_END      (23)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_value3_START  (24)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_value3_END    (30)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_vld3_START    (31)
#define SOC_PERI_SCTRL_SC_CLKCFG8BIT10_clk_8bit_cfg10_vld3_END      (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CARM_CTRL0_UNION
 结构说明  : SC_CARM_CTRL0 寄存器结构定义。地址偏移量:0x500，初值:0x00000C9B，宽度:32
 寄存器说明: CARM控制寄存器0。CARM DEBUG等控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  carm_ramctrl_s_ema   : 3;  /* bit[0-2]  : memory读速率控制，随此值的增大，memory读延时增大 */
        unsigned int  reserved_0           : 1;  /* bit[3]    : reserved */
        unsigned int  carm_ramctrl_s_emaw  : 2;  /* bit[4-5]  : memory写速率控制，随此值的增大，memory写延时增大（文档含义是这样，看LIB含义又不是，后面会更新文档） */
        unsigned int  carm_ramctrl_s_emas  : 1;  /* bit[6]    : 对HSmemory有作用；
                                                                对HDmemory为保留bit，无意义； */
        unsigned int  reserved_1           : 3;  /* bit[7-9]  : reserved */
        unsigned int  carm_ramctrl_s_ret1n : 1;  /* bit[10]   : retention控制1
                                                                0：retention模式1；
                                                                1：正常工作模式。 */
        unsigned int  carm_ramctrl_s_ret2n : 1;  /* bit[11]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                retention控制2
                                                                0：retention模式2；
                                                                1：正常工作模式。 */
        unsigned int  carm_ramctrl_s_pgen  : 1;  /* bit[12]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                0：正常工作模式；
                                                                1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_2           : 3;  /* bit[13-15]: reserved */
        unsigned int  carm_cp15disable     : 1;  /* bit[16]   : 协处理器CP15寄存器的写保护：
                                                                0：不使能写保护；
                                                                1：使能写保护。 */
        unsigned int  carm_cfgdisable      : 1;  /* bit[17]   : ARM内部控制寄存器的写保护：
                                                                0：不使能写保护；
                                                                1：使能写保护。 */
        unsigned int  carm_l2rstdisable    : 1;  /* bit[18]   : CARM l2cache复位缓存数据是否使无效
                                                                0：否；
                                                                1：是。 */
        unsigned int  carm_l1rstdisable    : 1;  /* bit[19]   : CARM l1cache复位缓存数据是否使无效
                                                                0：否；
                                                                1：是。 */
        unsigned int  carm_dbgrestart      : 1;  /* bit[20]   : External restart requests */
        unsigned int  carm_debug_req       : 1;  /* bit[21]   : carmdebugrequest信号。
                                                                0：不使能请求；
                                                                1：使能请求。 */
        unsigned int  carm_dbgpwrup        : 1;  /* bit[22]   : carm debug上电信号 */
        unsigned int  reserved_3           : 3;  /* bit[23-25]: reserved */
        unsigned int  secure_cfg_tzpc_dep  : 1;  /* bit[26]   : 安全配置，控制tzpc_dep软门控是否可被软件控制：
                                                                0：tzpc_dep软件门控可被软件控制。
                                                                1：tzpc_dep软件门控不可被软件控制，为常开。 */
        unsigned int  secure_cfg_seceng    : 1;  /* bit[27]   : 安全配置，控制seceng软门控是否可被软件控制：
                                                                0：seceng软件门控可被软件控制。
                                                                1：seceng软件门控不可被软件控制，为常开。 */
        unsigned int  secure_cfg_i2c0      : 1;  /* bit[28]   : 安全配置，控制i2c0软门控是否可被软件控制：
                                                                0：i2c0软件门控可被软件控制。
                                                                1：i2c0软件门控不可被软件控制，为常开。 */
        unsigned int  secure_cfg_i2c1      : 1;  /* bit[29]   : 安全配置，控制i2c1软门控是否可被软件控制：
                                                                0：i2c1软件门控可被软件控制。
                                                                1：i2c1软件门控不可被软件控制，为常开。 */
        unsigned int  secure_cfg_i2c2      : 1;  /* bit[30]   : 安全配置，控制i2c2软门控是否可被软件控制：
                                                                0：i2c2软件门控可被软件控制。
                                                                1：i2c2软件门控不可被软件控制，为常开。 */
        unsigned int  secure_cfg_i2c3      : 1;  /* bit[31]   : 安全配置，控制i2c3软门控是否可被软件控制：
                                                                0：i2c3软件门控可被软件控制。
                                                                1：i2c3软件门控不可被软件控制，为常开。 */
    } reg;
} SOC_PERI_SCTRL_SC_CARM_CTRL0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_ema_START    (0)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_ema_END      (2)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_emaw_START   (4)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_emaw_END     (5)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_emas_START   (6)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_emas_END     (6)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_ret1n_START  (10)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_ret1n_END    (10)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_ret2n_START  (11)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_ret2n_END    (11)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_pgen_START   (12)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_ramctrl_s_pgen_END     (12)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_cp15disable_START      (16)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_cp15disable_END        (16)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_cfgdisable_START       (17)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_cfgdisable_END         (17)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_l2rstdisable_START     (18)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_l2rstdisable_END       (18)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_l1rstdisable_START     (19)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_l1rstdisable_END       (19)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_dbgrestart_START       (20)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_dbgrestart_END         (20)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_debug_req_START        (21)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_debug_req_END          (21)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_dbgpwrup_START         (22)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_dbgpwrup_END           (22)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_tzpc_dep_START   (26)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_tzpc_dep_END     (26)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_seceng_START     (27)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_seceng_END       (27)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_i2c0_START       (28)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_i2c0_END         (28)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_i2c1_START       (29)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_i2c1_END         (29)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_i2c2_START       (30)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_i2c2_END         (30)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_i2c3_START       (31)
#define SOC_PERI_SCTRL_SC_CARM_CTRL0_secure_cfg_i2c3_END         (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CARM_CTRL1_UNION
 结构说明  : SC_CARM_CTRL1 寄存器结构定义。地址偏移量:0x504，初值:0x00000000，宽度:32
 寄存器说明: CARM控制寄存器1。用于控制CARM filter模式使能及起始和结束地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ccpu_cfgte   : 1;  /* bit[0]   : Thumb扩展指令选择。该信号初始化CP15内部SCTLR寄存器TE位值。仅复位时被内核获取。
                                                       0：ARM指令
                                                       1：Thumb指令 */
        unsigned int  carm_cfgend  : 1;  /* bit[1]   : 大小端选择。该信号初始化CP15内部SCTLR寄存器的EE位值。复位时被内核获取。
                                                       0：小端模式。
                                                       1：大端模式。 */
        unsigned int  carm_vinithi : 1;  /* bit[2]   : 指示CARM零地址。
                                                       0：指示零地址在0x00000000,即ddr；
                                                       1：指示零地址在0xffff0000,即bootrom。 */
        unsigned int  reserved     : 29; /* bit[3-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_CARM_CTRL1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CARM_CTRL1_ccpu_cfgte_START    (0)
#define SOC_PERI_SCTRL_SC_CARM_CTRL1_ccpu_cfgte_END      (0)
#define SOC_PERI_SCTRL_SC_CARM_CTRL1_carm_cfgend_START   (1)
#define SOC_PERI_SCTRL_SC_CARM_CTRL1_carm_cfgend_END     (1)
#define SOC_PERI_SCTRL_SC_CARM_CTRL1_carm_vinithi_START  (2)
#define SOC_PERI_SCTRL_SC_CARM_CTRL1_carm_vinithi_END    (2)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_CARM_STAT0_UNION
 结构说明  : SC_CARM_STAT0 寄存器结构定义。地址偏移量:0x508，初值:0x00000000，宽度:32
 寄存器说明: CARM状态寄存器0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  carm_dbgtrigger   : 1;  /* bit[0]   : Debug external request taken */
        unsigned int  carm_dbgrestarted : 1;  /* bit[1]   : Handshake for DBGRESTART */
        unsigned int  carm_dbgnopwrdwn  : 1;  /* bit[2]   : 不下电请求
                                                            0：下电请求时，处理器下电
                                                            1：下电请求时，处理器不下电 */
        unsigned int  carm_dbgpwrupreq  : 1;  /* bit[3]   : 上电请求
                                                            0：下电请求
                                                            1：上电请求 */
        unsigned int  carm_smp_amp      : 1;  /* bit[4]   : CARM的AMP或者SMP模式指示位。
                                                            0：AMP模式；
                                                            1：SMP模式。 */
        unsigned int  carm_standby_wfi  : 1;  /* bit[5]   : CARMWFI状态指示，高有效。
                                                            0：不处于standbywfi状态；
                                                            1：处于standbywfi状态。 */
        unsigned int  carm_standby_wfe  : 1;  /* bit[6]   : CARMWFE状态指示，高有效。
                                                            0=processornotinWFEStandbymode
                                                            1=processorinWFEStandbymode. */
        unsigned int  carm_dbgack       : 1;  /* bit[7]   : carm_dbgack，carm的debugacknowledge状态指示信号；
                                                            0=notacknowledged
                                                            1=acknowledged. */
        unsigned int  carm_standbywfil2 : 1;  /* bit[8]   : L2 memory system WFI standby状态指示。当满足下面条件时输出高：
                                                             所有处理器处于WFI
                                                             ACINACTM为高
                                                             L2 memory system处于IDLE */
        unsigned int  reserved          : 23; /* bit[9-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_CARM_STAT0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgtrigger_START    (0)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgtrigger_END      (0)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgrestarted_START  (1)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgrestarted_END    (1)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgnopwrdwn_START   (2)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgnopwrdwn_END     (2)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgpwrupreq_START   (3)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgpwrupreq_END     (3)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_smp_amp_START       (4)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_smp_amp_END         (4)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_standby_wfi_START   (5)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_standby_wfi_END     (5)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_standby_wfe_START   (6)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_standby_wfe_END     (6)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgack_START        (7)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_dbgack_END          (7)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_standbywfil2_START  (8)
#define SOC_PERI_SCTRL_SC_CARM_STAT0_carm_standbywfil2_END    (8)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_UNION
 结构说明  : SC_DSP_SUBSYS_CTRL0 寄存器结构定义。地址偏移量:0x600，初值:0x00000004，宽度:32
 寄存器说明: HIFI控制寄存器。用于配置HIFI控制信号，同时也用于寄存HIFI的状态信号。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hifi_stat_vector_sel  : 1;  /* bit[0]   : 向量表基地址切换外部硬件控制信号。
                                                                0；选择默认向量表基地址；
                                                                1：选择用户定义的向量表基地址。
                                                                Boot关联重要信号，决定reset后TCM启动还是外部启动。默认为TCM启动，默认态为0。连接系统控制器，软件可配置。
                                                                复位（BReset）释放前至少稳定6个内核周期，Breset释放后保持至少10内核周期。 */
        unsigned int  hifi_ocd_halton_reset : 1;  /* bit[1]   : 复位调试硬件指示输入。HIFI处理器复位时该信号为1，解除复位后处理器将进入OCDhaltmode。拉到系统控制器，软件可配，默认态为0。
                                                                0：不复位；
                                                                1：进入复位。 */
        unsigned int  hifi_runstall         : 1;  /* bit[2]   : 处理器流水线暂停（stall）控制信号。
                                                                0：处理器流水线正常；
                                                                1：处理器流水线暂停。
                                                                主要有以下两个应用：
                                                                （1）启动时，在复位前至少2个周期有效，释放复位后，外部load程序和数据，load完成后释放该信号使处理器开始取指令执行；
                                                                （2）非启动时，用于外部控制进入和退出低功耗（sleep）模式。 */
        unsigned int  reserved_0            : 27; /* bit[3-29]: 保留。 */
        unsigned int  reserved_1            : 1;  /* bit[30]  : 处理器OCDhaltmode状态指示信号，高有效。
                                                                0：未处于haltmode；
                                                                1：处于haltmode。 */
        unsigned int  reserved_2            : 1;  /* bit[31]  : 处理器sleepmode状态指示信号，高有效。指示当前处理器通过执行WAITI指令进入sleepmode（clockgating）并等待未屏蔽中断唤醒。
                                                                0：未进入sleepmode；
                                                                1：进入sleepmode。 */
    } reg;
} SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_hifi_stat_vector_sel_START   (0)
#define SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_hifi_stat_vector_sel_END     (0)
#define SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_hifi_ocd_halton_reset_START  (1)
#define SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_hifi_ocd_halton_reset_END    (1)
#define SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_hifi_runstall_START          (2)
#define SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_hifi_runstall_END            (2)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_HIFI_CTRL1_UNION
 结构说明  : SC_HIFI_CTRL1 寄存器结构定义。地址偏移量:0x604，初值:0x00000000，宽度:32
 寄存器说明: HIFI NMI中断控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hifi_nmi : 16; /* bit[0-15] : hifi nmi中断产生
                                                    0：消除中断
                                                    1：产生中断 */
        unsigned int  reserved : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_HIFI_CTRL1_UNION;
#endif
#define SOC_PERI_SCTRL_SC_HIFI_CTRL1_hifi_nmi_START  (0)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL1_hifi_nmi_END    (15)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_HIFI_CTRL0_UNION
 结构说明  : SC_HIFI_CTRL0 寄存器结构定义。地址偏移量:0x608，初值:0x00000000，宽度:32
 寄存器说明: hifi instruction space remap
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hifi_instruction_remap_addr : 16; /* bit[0-15] : hifi指令空间remap地址 */
        unsigned int  reserved                    : 16; /* bit[16-31]:  */
    } reg;
} SOC_PERI_SCTRL_SC_HIFI_CTRL0_UNION;
#endif
#define SOC_PERI_SCTRL_SC_HIFI_CTRL0_hifi_instruction_remap_addr_START  (0)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL0_hifi_instruction_remap_addr_END    (15)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_HIFI_CTRL2_UNION
 结构说明  : SC_HIFI_CTRL2 寄存器结构定义。地址偏移量:0x60C，初值:0x00003000，宽度:32
 寄存器说明: hifi boot addr安全寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hifi_boot_addr : 16; /* bit[0-15] : HIFI片外启动地址配置，复位值为0x3000。 */
        unsigned int  reserved       : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_HIFI_CTRL2_UNION;
#endif
#define SOC_PERI_SCTRL_SC_HIFI_CTRL2_hifi_boot_addr_START  (0)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL2_hifi_boot_addr_END    (15)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_HIFI_CTRL3_UNION
 结构说明  : SC_HIFI_CTRL3 寄存器结构定义。地址偏移量:0x0610，初值:0x00000000，宽度:32
 寄存器说明: hifi允许访问内存地址范围控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hifi_access_start_addr0 : 12; /* bit[0-11] : hifi 访问地址段第一段起始地址（以1MB为单位） */
        unsigned int  hifi_access_end_addr0   : 12; /* bit[12-23]: hifi 访问地址段第一段结束地址（以1MB为单位） */
        unsigned int  hifi_access_permit      : 1;  /* bit[24]   : hifi起始结束地址段内允许访问还是屏蔽访问：
                                                                   0：允许访问
                                                                   1：屏蔽访问 */
        unsigned int  reserved_0              : 3;  /* bit[25-27]: reserved */
        unsigned int  hifi_access_en          : 1;  /* bit[28]   : hifi地址段屏蔽访问功能使能信号：（在完成其它配置后最后配置改bit）
                                                                   0：不使能
                                                                   1：使能 */
        unsigned int  reserved_1              : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_HIFI_CTRL3_UNION;
#endif
#define SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_start_addr0_START  (0)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_start_addr0_END    (11)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_end_addr0_START    (12)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_end_addr0_END      (23)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_permit_START       (24)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_permit_END         (24)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_en_START           (28)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_en_END             (28)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_HIFI_CTRL4_UNION
 结构说明  : SC_HIFI_CTRL4 寄存器结构定义。地址偏移量:0x0614，初值:0x00000000，宽度:32
 寄存器说明: hifi允许访问内存地址范围控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hifi_access_start_addr1 : 12; /* bit[0-11] : hifi 访问地址段第二段起始地址（以1MB为单位） */
        unsigned int  hifi_access_end_addr1   : 12; /* bit[12-23]: hifi 访问地址段第二段结束地址（以1MB为单位） */
        unsigned int  reserved                : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_PERI_SCTRL_SC_HIFI_CTRL4_UNION;
#endif
#define SOC_PERI_SCTRL_SC_HIFI_CTRL4_hifi_access_start_addr1_START  (0)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL4_hifi_access_start_addr1_END    (11)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL4_hifi_access_end_addr1_START    (12)
#define SOC_PERI_SCTRL_SC_HIFI_CTRL4_hifi_access_end_addr1_END      (23)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_LAG_UNION
 结构说明  : SC_TEMP0_LAG 寄存器结构定义。地址偏移量:0x700，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0温度比较滞回范围寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_lag : 5;  /* bit[0-4] : Tsensor0温度比较滞回范围，单位配置值对应的温度值为200/255=0.7843℃，配置范围0~24.3℃，配置值不能大于任何一个温度阈值配置寄存器。
                                                    说明：用于温度值与温度阈值比较产生中断时，避免温度值在阈值附近频繁抖动，频繁产生中断，实现中断滤抖处理。温度上升高于阈值时，直接产生中断，温度下降低于阈值，继续下降该配置值时，才产生中断。 */
        unsigned int  reserved  : 27; /* bit[5-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_LAG_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_LAG_temp0_lag_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_LAG_temp0_lag_END    (4)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_TH_UNION
 结构说明  : SC_TEMP0_TH 寄存器结构定义。地址偏移量:0x704，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0温度阈值寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_0th : 8;  /* bit[0-7]  : Tsensor0温度阈值0。
                                                     0x00：小于-60℃；
                                                     0x01：-60~-59.21569℃；
                                                     0x02：-59.21569~-58.43137℃；
                                                     ......
                                                     0xfd：+138.43137~+139.21569℃；
                                                     0xfe：+139.21569~+140℃；
                                                     0xff：大于+140℃。
                                                     说明：8Bits二进制码给出的是温度区间概念，例如，温度低于-60℃时，阈值为0x00，若温度在-60℃和&#45;-59.21569℃之间，阈值为0x01；若温度在139.21569℃和140℃之间，阈值为0xfe，若温度高于140℃，则阈值为0xff。连续阈值之间的间隔为200/255=0.7843℃。 */
        unsigned int  temp0_1th : 8;  /* bit[8-15] : Tsensor0温度阈值1。
                                                     编码同Tsensor0温度阈值0。 */
        unsigned int  temp0_2th : 8;  /* bit[16-23]: Tsensor0温度阈值2。
                                                     编码同Tsensor0温度阈值0。 */
        unsigned int  temp0_3th : 8;  /* bit[24-31]: Tsensor0温度阈值3。
                                                     编码同Tsensor0温度阈值0。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_TH_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_TH_temp0_0th_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_TH_temp0_0th_END    (7)
#define SOC_PERI_SCTRL_SC_TEMP0_TH_temp0_1th_START  (8)
#define SOC_PERI_SCTRL_SC_TEMP0_TH_temp0_1th_END    (15)
#define SOC_PERI_SCTRL_SC_TEMP0_TH_temp0_2th_START  (16)
#define SOC_PERI_SCTRL_SC_TEMP0_TH_temp0_2th_END    (23)
#define SOC_PERI_SCTRL_SC_TEMP0_TH_temp0_3th_START  (24)
#define SOC_PERI_SCTRL_SC_TEMP0_TH_temp0_3th_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_RST_TH_UNION
 结构说明  : SC_TEMP0_RST_TH 寄存器结构定义。地址偏移量:0x708，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0复位阈值寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_rst_th : 8;  /* bit[0-7] : Tsensor0复位阈值。
                                                       编码同Tsensor0温度阈值0。
                                                       说明：当配置值等于0xff时，无法产生复位，故配置需小于0xff */
        unsigned int  reserved     : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_RST_TH_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_RST_TH_temp0_rst_th_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_RST_TH_temp0_rst_th_END    (7)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_CFG_UNION
 结构说明  : SC_TEMP0_CFG 寄存器结构定义。地址偏移量:0x70C，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0参数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_trim_raw   : 4;  /* bit[0-3]  : reserved */
        unsigned int  temp0_ct_sel_raw : 2;  /* bit[4-5]  : Tsensor0HKADC转换时间选择。
                                                            00：0.768ms；
                                                            01：6.144ms；
                                                            10：49.152ms；
                                                            11：393.216ms； */
        unsigned int  reserved_0       : 6;  /* bit[6-11] : 保留。 */
        unsigned int  temp0_test_raw   : 8;  /* bit[12-19]: Tsensor0测试模式配置。用于Tsensor0调试。
                                                            000：local sensor （default）
                                                            001：remote sensor1
                                                            010：remote sensor2
                                                            others：reserved */
        unsigned int  reserved_1       : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_CFG_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_CFG_temp0_trim_raw_START    (0)
#define SOC_PERI_SCTRL_SC_TEMP0_CFG_temp0_trim_raw_END      (3)
#define SOC_PERI_SCTRL_SC_TEMP0_CFG_temp0_ct_sel_raw_START  (4)
#define SOC_PERI_SCTRL_SC_TEMP0_CFG_temp0_ct_sel_raw_END    (5)
#define SOC_PERI_SCTRL_SC_TEMP0_CFG_temp0_test_raw_START    (12)
#define SOC_PERI_SCTRL_SC_TEMP0_CFG_temp0_test_raw_END      (19)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_EN_UNION
 结构说明  : SC_TEMP0_EN 寄存器结构定义。地址偏移量:0x710，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0温度检测模块使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_en_raw : 1;  /* bit[0-0] : Tsensor0温度检测模块使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  reserved     : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_EN_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_EN_temp0_en_raw_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_EN_temp0_en_raw_END    (0)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_INT_EN_UNION
 结构说明  : SC_TEMP0_INT_EN 寄存器结构定义。地址偏移量:0x714，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0温度检测中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_int_en : 1;  /* bit[0-0] : Tsensor0温度检测中断屏蔽。
                                                       0：屏蔽中断；
                                                       1：使能中断。 */
        unsigned int  reserved     : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_INT_EN_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_INT_EN_temp0_int_en_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_INT_EN_temp0_int_en_END    (0)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_INT_CLR_UNION
 结构说明  : SC_TEMP0_INT_CLR 寄存器结构定义。地址偏移量:0x718，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0温度检测中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_int_clr : 1;  /* bit[0-0] : Tsensor0温度检测中断清零。
                                                        0：无效；
                                                        1：清除Raw中断和Mask中断。 */
        unsigned int  reserved      : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_INT_CLR_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_INT_CLR_temp0_int_clr_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_INT_CLR_temp0_int_clr_END    (0)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_RST_MSK_UNION
 结构说明  : SC_TEMP0_RST_MSK 寄存器结构定义。地址偏移量:0x71C，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0温度检测复位屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_rst_msk : 1;  /* bit[0-0] : Tsensor0温度检测复位屏蔽。
                                                        0：屏蔽复位；
                                                        1：使能复位。 */
        unsigned int  reserved      : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_RST_MSK_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_RST_MSK_temp0_rst_msk_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_RST_MSK_temp0_rst_msk_END    (0)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_RAW_INT_UNION
 结构说明  : SC_TEMP0_RAW_INT 寄存器结构定义。地址偏移量:0x720，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0温度检测Raw中断寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_rawint : 1;  /* bit[0-0] : Tsensor0温度检测Raw中断。
                                                       0：无效；
                                                       1：有效。 */
        unsigned int  reserved     : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_RAW_INT_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_RAW_INT_temp0_rawint_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_RAW_INT_temp0_rawint_END    (0)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_MSK_INT_UNION
 结构说明  : SC_TEMP0_MSK_INT 寄存器结构定义。地址偏移量:0x724，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0温度检测Mask中断寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_mskint : 1;  /* bit[0-0] : Tsensor0温度检测Mask中断。
                                                       0：无效；
                                                       1：有效。 */
        unsigned int  reserved     : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_MSK_INT_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_MSK_INT_temp0_mskint_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_MSK_INT_temp0_mskint_END    (0)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_TEMP0_VALUE_UNION
 结构说明  : SC_TEMP0_VALUE 寄存器结构定义。地址偏移量:0x728，初值:0x00000000，宽度:32
 寄存器说明: Tsensor0温度检测值寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  temp0_out_s : 8;  /* bit[0-7] : Tsensor0温度检测值。编码如下：
                                                      00000000：等于或低于-60℃；
                                                      …
                                                      00110011：-20℃；
                                                      …
                                                      01100110：20℃；
                                                      …
                                                      10011001：60℃；
                                                      …
                                                      11001100：100℃；
                                                      …
                                                      11111111：等于或高于140℃；
                                                      说明：8Bits二进制码给出的是温度区间概念，连续温度码之间的间隔为200/255=0.7843℃。 */
        unsigned int  reserved    : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_PERI_SCTRL_SC_TEMP0_VALUE_UNION;
#endif
#define SOC_PERI_SCTRL_SC_TEMP0_VALUE_temp0_out_s_START  (0)
#define SOC_PERI_SCTRL_SC_TEMP0_VALUE_temp0_out_s_END    (7)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED7_UNION
 结构说明  : SC_RESERVED7 寄存器结构定义。地址偏移量:0xD00，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved7 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED7_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED7_sc_reserved7_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED7_sc_reserved7_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED8_UNION
 结构说明  : SC_RESERVED8 寄存器结构定义。地址偏移量:0xD04，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved8 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED8_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED8_sc_reserved8_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED8_sc_reserved8_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED11_UNION
 结构说明  : SC_RESERVED11 寄存器结构定义。地址偏移量:0xD08，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved11 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED11_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED11_sc_reserved11_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED11_sc_reserved11_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED12_UNION
 结构说明  : SC_RESERVED12 寄存器结构定义。地址偏移量:0xD0C，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved12 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED12_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED12_sc_reserved12_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED12_sc_reserved12_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED13_UNION
 结构说明  : SC_RESERVED13 寄存器结构定义。地址偏移量:0xD10，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved13 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED13_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED13_sc_reserved13_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED13_sc_reserved13_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED14_UNION
 结构说明  : SC_RESERVED14 寄存器结构定义。地址偏移量:0xD14，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved14 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED14_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED14_sc_reserved14_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED14_sc_reserved14_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED15_UNION
 结构说明  : SC_RESERVED15 寄存器结构定义。地址偏移量:0xD18，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved15 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED15_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED15_sc_reserved15_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED15_sc_reserved15_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED16_UNION
 结构说明  : SC_RESERVED16 寄存器结构定义。地址偏移量:0xD1C，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved16 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED16_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED16_sc_reserved16_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED16_sc_reserved16_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED17_UNION
 结构说明  : SC_RESERVED17 寄存器结构定义。地址偏移量:0xD20，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved17 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED17_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED17_sc_reserved17_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED17_sc_reserved17_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED18_UNION
 结构说明  : SC_RESERVED18 寄存器结构定义。地址偏移量:0xD24，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved18 : 32; /* bit[0-31]: 保留。寄存器数值随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED18_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED18_sc_reserved18_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED18_sc_reserved18_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED21_UNION
 结构说明  : SC_RESERVED21 寄存器结构定义。地址偏移量:0xD28，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved21 : 32; /* bit[0-31]: 保留。寄存器数值不随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED21_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED21_sc_reserved21_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED21_sc_reserved21_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED22_UNION
 结构说明  : SC_RESERVED22 寄存器结构定义。地址偏移量:0xD2C，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved22 : 32; /* bit[0-31]: 保留。寄存器数值不随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED22_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED22_sc_reserved22_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED22_sc_reserved22_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED23_UNION
 结构说明  : SC_RESERVED23 寄存器结构定义。地址偏移量:0xD30，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved23 : 32; /* bit[0-31]: 保留。寄存器数值不随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED23_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED23_sc_reserved23_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED23_sc_reserved23_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED24_UNION
 结构说明  : SC_RESERVED24 寄存器结构定义。地址偏移量:0xD34，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved24 : 32; /* bit[0-31]: 保留。寄存器数值不随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED24_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED24_sc_reserved24_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED24_sc_reserved24_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED25_UNION
 结构说明  : SC_RESERVED25 寄存器结构定义。地址偏移量:0xD38，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved25 : 32; /* bit[0-31]: 保留。寄存器数值不随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED25_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED25_sc_reserved25_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED25_sc_reserved25_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED26_UNION
 结构说明  : SC_RESERVED26 寄存器结构定义。地址偏移量:0xD3C，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved26 : 32; /* bit[0-31]: 保留。寄存器数值不随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED26_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED26_sc_reserved26_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED26_sc_reserved26_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED27_UNION
 结构说明  : SC_RESERVED27 寄存器结构定义。地址偏移量:0xD40，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved27 : 32; /* bit[0-31]: 保留。寄存器数值不随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED27_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED27_sc_reserved27_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED27_sc_reserved27_END    (31)


/*****************************************************************************
 结构名    : SOC_PERI_SCTRL_SC_RESERVED28_UNION
 结构说明  : SC_RESERVED28 寄存器结构定义。地址偏移量:0xD44，初值:0x00000000，宽度:32
 寄存器说明: 保留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sc_reserved28 : 32; /* bit[0-31]: 保留。寄存器数值不随系统复位 */
    } reg;
} SOC_PERI_SCTRL_SC_RESERVED28_UNION;
#endif
#define SOC_PERI_SCTRL_SC_RESERVED28_sc_reserved28_START  (0)
#define SOC_PERI_SCTRL_SC_RESERVED28_sc_reserved28_END    (31)






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

#endif /* end of soc_peri_sctrl_interface.h */
