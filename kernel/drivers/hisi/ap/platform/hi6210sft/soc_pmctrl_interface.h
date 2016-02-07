

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_PMCTRL_INTERFACE_H__
#define __SOC_PMCTRL_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) register_define
 ****************************************************************************/
/* 寄存器说明：ACPU PLL控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUPLLCTRL_UNION */
#define SOC_PMCTRL_ACPUPLLCTRL_ADDR(base)             ((base) + (0x000))

/* 寄存器说明：ACPU PLL频率寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUPLLFREQ_UNION */
#define SOC_PMCTRL_ACPUPLLFREQ_ADDR(base)             ((base) + (0x004))

/* 寄存器说明：GPU PLL控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_GPUPLLCTRL_UNION */
#define SOC_PMCTRL_GPUPLLCTRL_ADDR(base)              ((base) + (0x008))

/* 寄存器说明：GPU PLL频率寄存器
   位域定义UNION结构:  SOC_PMCTRL_GPUPLLFREQ_UNION */
#define SOC_PMCTRL_GPUPLLFREQ_ADDR(base)              ((base) + (0x00C))

/* 寄存器说明：DDR PLL1控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL1CTRL_UNION */
#define SOC_PMCTRL_DDRPLL1CTRL_ADDR(base)             ((base) + (0x010))

/* 寄存器说明：DDR PLL1频率寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL1FREQ_UNION */
#define SOC_PMCTRL_DDRPLL1FREQ_ADDR(base)             ((base) + (0x014))

/* 寄存器说明：DDRPLL0控制影子寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL0ALIAS_UNION */
#define SOC_PMCTRL_DDRPLL0ALIAS_ADDR(base)            ((base) + (0x018))

/* 寄存器说明：SYS PLL控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_SYSPLLCTRL_UNION */
#define SOC_PMCTRL_SYSPLLCTRL_ADDR(base)              ((base) + (0x028))

/* 寄存器说明：SYS PLL频率寄存器
   位域定义UNION结构:  SOC_PMCTRL_SYSPLLFREQ_UNION */
#define SOC_PMCTRL_SYSPLLFREQ_ADDR(base)              ((base) + (0x02C))

/* 寄存器说明：DDR PLL0控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL0CTRL_UNION */
#define SOC_PMCTRL_DDRPLL0CTRL_ADDR(base)             ((base) + (0x030))

/* 寄存器说明：DDR PLL0频率寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL0FREQ_UNION */
#define SOC_PMCTRL_DDRPLL0FREQ_ADDR(base)             ((base) + (0x034))

/* 寄存器说明：MED PLL控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_MEDPLLCTRL_UNION */
#define SOC_PMCTRL_MEDPLLCTRL_ADDR(base)              ((base) + (0x038))

/* 寄存器说明：MED PLL频率寄存器
   位域定义UNION结构:  SOC_PMCTRL_MEDPLLFREQ_UNION */
#define SOC_PMCTRL_MEDPLLFREQ_ADDR(base)              ((base) + (0x03C))

/* 寄存器说明：MEDPLL控制影子寄存器
   位域定义UNION结构:  SOC_PMCTRL_MEDPLLALIAS_UNION */
#define SOC_PMCTRL_MEDPLLALIAS_ADDR(base)             ((base) + (0x040))

/* 寄存器说明：BBPPLL控制影子寄存器
   位域定义UNION结构:  SOC_PMCTRL_BBPPLLALIAS_UNION */
#define SOC_PMCTRL_BBPPLLALIAS_ADDR(base)             ((base) + (0x048))

/* 寄存器说明：DDRPLL0配置使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL0CFGEN_UNION */
#define SOC_PMCTRL_DDRPLL0CFGEN_ADDR(base)            ((base) + (0x04C))

/* 寄存器说明：SYS PLL小数分频寄存器
   位域定义UNION结构:  SOC_PMCTRL_SYSPLLFRAC_UNION */
#define SOC_PMCTRL_SYSPLLFRAC_ADDR(base)              ((base) + (0x050))

/* 寄存器说明：SYS PLL扩频控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_SYSPLLSSC_UNION */
#define SOC_PMCTRL_SYSPLLSSC_ADDR(base)               ((base) + (0x054))

/* 寄存器说明：DDR PLL0小数分频寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL0FRAC_UNION */
#define SOC_PMCTRL_DDRPLL0FRAC_ADDR(base)             ((base) + (0x058))

/* 寄存器说明：DDR PLL0扩频控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL0SSC_UNION */
#define SOC_PMCTRL_DDRPLL0SSC_ADDR(base)              ((base) + (0x05C))

/* 寄存器说明：DDR PLL1小数分频寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL1FRAC_UNION */
#define SOC_PMCTRL_DDRPLL1FRAC_ADDR(base)             ((base) + (0x074))

/* 寄存器说明：DDR PLL1扩频控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLL1SSC_UNION */
#define SOC_PMCTRL_DDRPLL1SSC_ADDR(base)              ((base) + (0x078))

/* 寄存器说明：时钟参数配置寄存器0
   位域定义UNION结构:  SOC_PMCTRL_CLKCFG4BIT0_UNION */
#define SOC_PMCTRL_CLKCFG4BIT0_ADDR(base)             ((base) + (0x080))

/* 寄存器说明：时钟参数配置寄存器1
   位域定义UNION结构:  SOC_PMCTRL_CLKCFG4BIT1_UNION */
#define SOC_PMCTRL_CLKCFG4BIT1_ADDR(base)             ((base) + (0x084))

/* 寄存器说明：时钟参数配置寄存器2
   位域定义UNION结构:  SOC_PMCTRL_CLKCFG4BIT2_UNION */
#define SOC_PMCTRL_CLKCFG4BIT2_ADDR(base)             ((base) + (0x088))

/* 寄存器说明：TIMER0控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_TIMER0CTRL_UNION */
#define SOC_PMCTRL_TIMER0CTRL_ADDR(base)              ((base) + (0x0C0))

/* 寄存器说明：TIMER1控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_TIMER1CTRL_UNION */
#define SOC_PMCTRL_TIMER1CTRL_ADDR(base)              ((base) + (0x0C4))

/* 寄存器说明：TIMER2控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_TIMER2CTRL_UNION */
#define SOC_PMCTRL_TIMER2CTRL_ADDR(base)              ((base) + (0x0C8))

/* 寄存器说明：TIMER3控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_TIMER3CTRL_UNION */
#define SOC_PMCTRL_TIMER3CTRL_ADDR(base)              ((base) + (0x0CC))

/* 寄存器说明：PMUSSI通道使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMUSSICHNEN_UNION */
#define SOC_PMCTRL_PMUSSICHNEN_ADDR(base)             ((base) + (0x0FC))

/* 寄存器说明：ACPU PLL选择寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUPLLSEL_UNION */
#define SOC_PMCTRL_ACPUPLLSEL_ADDR(base)              ((base) + (0x100))

/* 寄存器说明：ACPU时钟分频寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUCLKDIV_UNION */
#define SOC_PMCTRL_ACPUCLKDIV_ADDR(base)              ((base) + (0x104))

/* 寄存器说明：ACPU存储体RM参数寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUMEMRM_UNION */
#define SOC_PMCTRL_ACPUMEMRM_ADDR(base)               ((base) + (0x108))

/* 寄存器说明：ACPU电压检测寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUVOLDET_UNION */
#define SOC_PMCTRL_ACPUVOLDET_ADDR(base)              ((base) + (0x10C))

/* 寄存器说明：ACPU侧SYSPLL的配置寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUSYSPLLCFG_UNION */
#define SOC_PMCTRL_ACPUSYSPLLCFG_ADDR(base)           ((base) + (0x110))

/* 寄存器说明：ACPU DVFS时钟模式寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUCLKOFFCFG_UNION */
#define SOC_PMCTRL_ACPUCLKOFFCFG_ADDR(base)           ((base) + (0x114))

/* 寄存器说明：ACPU DVFS使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUDVFSEN_UNION */
#define SOC_PMCTRL_ACPUDVFSEN_ADDR(base)              ((base) + (0x118))

/* 寄存器说明：ACPU频率Profile寄存器0
   位域定义UNION结构:  SOC_PMCTRL_ACPUCLKPROFILE0_UNION */
#define SOC_PMCTRL_ACPUCLKPROFILE0_ADDR(base)         ((base) + (0x11C))

/* 寄存器说明：ACPU频率Profile寄存器1
   位域定义UNION结构:  SOC_PMCTRL_ACPUCLKPROFILE1_UNION */
#define SOC_PMCTRL_ACPUCLKPROFILE1_ADDR(base)         ((base) + (0x120))

/* 寄存器说明：ACPU电压Profile寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUVOLPROFILE_UNION */
#define SOC_PMCTRL_ACPUVOLPROFILE_ADDR(base)          ((base) + (0x124))

/* 寄存器说明：ACPU PLL分频状态寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUPLLSTAT_UNION */
#define SOC_PMCTRL_ACPUPLLSTAT_ADDR(base)             ((base) + (0x128))

/* 寄存器说明：ACPU时钟分频超时寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUDIVTIMEOUT_UNION */
#define SOC_PMCTRL_ACPUDIVTIMEOUT_ADDR(base)          ((base) + (0x12C))

/* 寄存器说明：ACPU AVS再次启动超时寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSONTIMEOUT_UNION */
#define SOC_PMCTRL_ACPUAVSONTIMEOUT_ADDR(base)        ((base) + (0x130))

/* 寄存器说明：ACPU AVS参数更新寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPARAUPD_UNION */
#define SOC_PMCTRL_ACPUAVSPARAUPD_ADDR(base)          ((base) + (0x180))

/* 寄存器说明：ACPU AVS参数寄存器0
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPARA0_UNION */
#define SOC_PMCTRL_ACPUAVSPARA0_ADDR(base)            ((base) + (0x184))

/* 寄存器说明：ACPU AVS参数寄存器1
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPARA1_UNION */
#define SOC_PMCTRL_ACPUAVSPARA1_ADDR(base)            ((base) + (0x188))

/* 寄存器说明：ACPU AVS参数寄存器2
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPARA2_UNION */
#define SOC_PMCTRL_ACPUAVSPARA2_ADDR(base)            ((base) + (0x18C))

/* 寄存器说明：ACPU AVS参数寄存器3
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPARA3_UNION */
#define SOC_PMCTRL_ACPUAVSPARA3_ADDR(base)            ((base) + (0x190))

/* 寄存器说明：ACPU AVS参数寄存器4
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPARA4_UNION */
#define SOC_PMCTRL_ACPUAVSPARA4_ADDR(base)            ((base) + (0x194))

/* 寄存器说明：ACPU AVS参数寄存器5
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPARA5_UNION */
#define SOC_PMCTRL_ACPUAVSPARA5_ADDR(base)            ((base) + (0x198))

/* 寄存器说明：ACPU AVS参数寄存器6
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPARA6_UNION */
#define SOC_PMCTRL_ACPUAVSPARA6_ADDR(base)            ((base) + (0x19C))

/* 寄存器说明：ACPU AVS参数寄存器7
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPARA7_UNION */
#define SOC_PMCTRL_ACPUAVSPARA7_ADDR(base)            ((base) + (0x1A0))

/* 寄存器说明：ACPU AVS使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSEN_UNION */
#define SOC_PMCTRL_ACPUAVSEN_ADDR(base)               ((base) + (0x1E4))

/* 寄存器说明：ACPU AVS周期寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUAVSPERIOD_UNION */
#define SOC_PMCTRL_ACPUAVSPERIOD_ADDR(base)           ((base) + (0x1E8))

/* 寄存器说明：ACPU AVS运行轮数寄存器
   位域定义UNION结构:  SOC_PMCTRL_AACPUAVSRUNROUND_UNION */
#define SOC_PMCTRL_AACPUAVSRUNROUND_ADDR(base)        ((base) + (0x1EC))

/* 寄存器说明：工艺检测参数更新寄存器
   位域定义UNION结构:  SOC_PMCTRL_PDPARAUDP_UNION */
#define SOC_PMCTRL_PDPARAUDP_ADDR(base)               ((base) + (0x280))

/* 寄存器说明：工艺检测参数寄存器0
   位域定义UNION结构:  SOC_PMCTRL_PDPARA0_UNION */
#define SOC_PMCTRL_PDPARA0_ADDR(base)                 ((base) + (0x284))

/* 寄存器说明：工艺检测参数寄存器1
   位域定义UNION结构:  SOC_PMCTRL_PDPARA1_UNION */
#define SOC_PMCTRL_PDPARA1_ADDR(base)                 ((base) + (0x288))

/* 寄存器说明：工艺检测参数寄存器2
   位域定义UNION结构:  SOC_PMCTRL_PDAVSPARA2_UNION */
#define SOC_PMCTRL_PDAVSPARA2_ADDR(base)              ((base) + (0x2A0))

/* 寄存器说明：工艺检测使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_PDAVSEN_UNION */
#define SOC_PMCTRL_PDAVSEN_ADDR(base)                 ((base) + (0x2E0))

/* 寄存器说明：ACPU HPM控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUHPMCTRL_UNION */
#define SOC_PMCTRL_ACPUHPMCTRL_ADDR(base)             ((base) + (0x300))

/* 寄存器说明：ACPU HPM OPC有效寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUHPMOPCVALID_UNION */
#define SOC_PMCTRL_ACPUHPMOPCVALID_ADDR(base)         ((base) + (0x314))

/* 寄存器说明：ACPU01 HPM OPC寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPU01HPMOPC_UNION */
#define SOC_PMCTRL_ACPU01HPMOPC_ADDR(base)            ((base) + (0x318))

/* 寄存器说明：ACPU23 HPM OPC寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPU23HPMOPC_UNION */
#define SOC_PMCTRL_ACPU23HPMOPC_ADDR(base)            ((base) + (0x31C))

/* 寄存器说明：ACPU01 HPM PC寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPU01HPMPC_UNION */
#define SOC_PMCTRL_ACPU01HPMPC_ADDR(base)             ((base) + (0x330))

/* 寄存器说明：ACPU23 HPM PC寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPU23HPMPC_UNION */
#define SOC_PMCTRL_ACPU23HPMPC_ADDR(base)             ((base) + (0x334))

/* 寄存器说明：ACPU HPM旁路寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUHPMBYPASS_UNION */
#define SOC_PMCTRL_ACPUHPMBYPASS_ADDR(base)           ((base) + (0x33C))

/* 寄存器说明：ACPU电压上升稳定时间寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUPMUVOLUPTIME_UNION */
#define SOC_PMCTRL_ACPUPMUVOLUPTIME_ADDR(base)        ((base) + (0x360))

/* 寄存器说明：ACPU电压下降稳定时间寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUPMUVOLDNTIME_UNION */
#define SOC_PMCTRL_ACPUPMUVOLDNTIME_ADDR(base)        ((base) + (0x364))

/* 寄存器说明：ACPU电压配置地址寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUVOLPMUADDR_UNION */
#define SOC_PMCTRL_ACPUVOLPMUADDR_ADDR(base)          ((base) + (0x368))

/* 寄存器说明：ACPU电压提升步进寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUVOLUPSTEP_UNION */
#define SOC_PMCTRL_ACPUVOLUPSTEP_ADDR(base)           ((base) + (0x36C))

/* 寄存器说明：ACPU电压降低步进寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUVOLDNSTEP_UNION */
#define SOC_PMCTRL_ACPUVOLDNSTEP_ADDR(base)           ((base) + (0x370))

/* 寄存器说明：ACPU默认电压设置寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUDFTVOL_UNION */
#define SOC_PMCTRL_ACPUDFTVOL_ADDR(base)              ((base) + (0x374))

/* 寄存器说明：ACPU目标电压设置寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUDESTVOL_UNION */
#define SOC_PMCTRL_ACPUDESTVOL_ADDR(base)             ((base) + (0x378))

/* 寄存器说明：ACPU电压稳定指示寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUVOLTIMEOUT_UNION */
#define SOC_PMCTRL_ACPUVOLTIMEOUT_ADDR(base)          ((base) + (0x37C))

/* 寄存器说明：工艺检测HPM控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_PDHPMCTRL_UNION */
#define SOC_PMCTRL_PDHPMCTRL_ADDR(base)               ((base) + (0x380))

/* 寄存器说明：工艺检测HPM OPC有效寄存器
   位域定义UNION结构:  SOC_PMCTRL_PDHPMOPCVALID_UNION */
#define SOC_PMCTRL_PDHPMOPCVALID_ADDR(base)           ((base) + (0x394))

/* 寄存器说明：工艺检测HPM OPC寄存器0
   位域定义UNION结构:  SOC_PMCTRL_PDHPMOPC0_UNION */
#define SOC_PMCTRL_PDHPMOPC0_ADDR(base)               ((base) + (0x398))

/* 寄存器说明：工艺检测HPM OPC寄存器1
   位域定义UNION结构:  SOC_PMCTRL_PDHPMOPC1_UNION */
#define SOC_PMCTRL_PDHPMOPC1_ADDR(base)               ((base) + (0x39C))

/* 寄存器说明：工艺检测HPM PC寄存器0
   位域定义UNION结构:  SOC_PMCTRL_PDHPMPC0_UNION */
#define SOC_PMCTRL_PDHPMPC0_ADDR(base)                ((base) + (0x3A8))

/* 寄存器说明：工艺检测HPM PC寄存器1
   位域定义UNION结构:  SOC_PMCTRL_PDHPMPC1_UNION */
#define SOC_PMCTRL_PDHPMPC1_ADDR(base)                ((base) + (0x3AC))

/* 寄存器说明：工艺检测的性能码寄存器
   位域定义UNION结构:  SOC_PMCTRL_PDHPMPC_UNION */
#define SOC_PMCTRL_PDHPMPC_ADDR(base)                 ((base) + (0x3B0))

/* 寄存器说明：PMC中断原始状态寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMCINTRAW_UNION */
#define SOC_PMCTRL_PMCINTRAW_ADDR(base)               ((base) + (0x480))

/* 寄存器说明：PMC中断清除寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMCINTCLR_UNION */
#define SOC_PMCTRL_PMCINTCLR_ADDR(base)               ((base) + (0x484))

/* 寄存器说明：PMC给MCU中断的掩码寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMCINTENMCU_UNION */
#define SOC_PMCTRL_PMCINTENMCU_ADDR(base)             ((base) + (0x488))

/* 寄存器说明：PMC给MCU中断的掩码后状态寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMCINTSTATMCU_UNION */
#define SOC_PMCTRL_PMCINTSTATMCU_ADDR(base)           ((base) + (0x48C))

/* 寄存器说明：PMC给ACPU中断的掩码寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMCINTENACPU_UNION */
#define SOC_PMCTRL_PMCINTENACPU_ADDR(base)            ((base) + (0x490))

/* 寄存器说明：PMC给ACPU中断的掩码后状态寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMCINTSTATACPU_UNION */
#define SOC_PMCTRL_PMCINTSTATACPU_ADDR(base)          ((base) + (0x494))

/* 寄存器说明：PMC给MCPU中断的掩码寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMCINTENMCPU_UNION */
#define SOC_PMCTRL_PMCINTENMCPU_ADDR(base)            ((base) + (0x498))

/* 寄存器说明：PMC给MCPU中断的掩码后状态寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMCINTSTATMCPU_UNION */
#define SOC_PMCTRL_PMCINTSTATMCPU_ADDR(base)          ((base) + (0x49C))

/* 寄存器说明：PMC状态寄存器
   位域定义UNION结构:  SOC_PMCTRL_PMCSTATUS_UNION */
#define SOC_PMCTRL_PMCSTATUS_ADDR(base)               ((base) + (0x4A0))

/* 寄存器说明：DDRC时钟选择寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRCLKSEL_UNION */
#define SOC_PMCTRL_DDRCLKSEL_ADDR(base)               ((base) + (0x580))

/* 寄存器说明：DDRC LP控制寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRLPCTRL_UNION */
#define SOC_PMCTRL_DDRLPCTRL_ADDR(base)               ((base) + (0x588))

/* 寄存器说明：DDRC DFS时钟模式寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRCCLKOFFCFG_UNION */
#define SOC_PMCTRL_DDRCCLKOFFCFG_ADDR(base)           ((base) + (0x590))

/* 寄存器说明：DDR DFS时钟分频状态寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRCLKDIVSTAT_UNION */
#define SOC_PMCTRL_DDRCLKDIVSTAT_ADDR(base)           ((base) + (0x594))

/* 寄存器说明：DDR PLL切换使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRPLLSWEN_UNION */
#define SOC_PMCTRL_DDRPLLSWEN_ADDR(base)              ((base) + (0x598))

/* 寄存器说明：DDR DFS使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRDFSEN_UNION */
#define SOC_PMCTRL_DDRDFSEN_ADDR(base)                ((base) + (0x59C))

/* 寄存器说明：DDRPHY时钟分频超时寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRDIVTIMEOUT_UNION */
#define SOC_PMCTRL_DDRDIVTIMEOUT_ADDR(base)           ((base) + (0x5A4))

/* 寄存器说明：DDR DFS时钟分频配置寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRCLKDIVCFG_UNION */
#define SOC_PMCTRL_DDRCLKDIVCFG_ADDR(base)            ((base) + (0x5A8))

/* 寄存器说明：DDR DFS时钟分频profile寄存器
   位域定义UNION结构:  SOC_PMCTRL_DDRCLKPROFILE_UNION */
#define SOC_PMCTRL_DDRCLKPROFILE_ADDR(base)           ((base) + (0x5AC))

/* 寄存器说明：ACPU HPM屏蔽使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUHPMMASKEN_UNION */
#define SOC_PMCTRL_ACPUHPMMASKEN_ADDR(base)           ((base) + (0xC00))

/* 寄存器说明：ACPU HPM屏蔽去使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUHPMMASKDIS_UNION */
#define SOC_PMCTRL_ACPUHPMMASKDIS_ADDR(base)          ((base) + (0xC04))

/* 寄存器说明：ACPU HPM屏蔽状态寄存器
   位域定义UNION结构:  SOC_PMCTRL_ACPUHPMMASKSTAT_UNION */
#define SOC_PMCTRL_ACPUHPMMASKSTAT_ADDR(base)         ((base) + (0xC08))

/* 寄存器说明：工艺检测 HPM屏蔽使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_PDHPMMASKEN_UNION */
#define SOC_PMCTRL_PDHPMMASKEN_ADDR(base)             ((base) + (0xC10))

/* 寄存器说明：工艺检测 HPM屏蔽去使能寄存器
   位域定义UNION结构:  SOC_PMCTRL_PDHPMMASKDIS_UNION */
#define SOC_PMCTRL_PDHPMMASKDIS_ADDR(base)            ((base) + (0xC14))

/* 寄存器说明：工艺检测 HPM屏蔽状态寄存器
   位域定义UNION结构:  SOC_PMCTRL_PDHPMMASKSTAT_UNION */
#define SOC_PMCTRL_PDHPMMASKSTAT_ADDR(base)           ((base) + (0xC18))

/* 寄存器说明：MEDPLL保留寄存器1
   位域定义UNION结构:  SOC_PMCTRL_MEDPLLRESERVED1_UNION */
#define SOC_PMCTRL_MEDPLLRESERVED1_ADDR(base)         ((base) + (0x904))

/* 寄存器说明：ACPUPLL保留寄存器1
   位域定义UNION结构:  SOC_PMCTRL_ACPUPLLRESERVED1_UNION */
#define SOC_PMCTRL_ACPUPLLRESERVED1_ADDR(base)        ((base) + (0x908))

/* 寄存器说明：GPUPLL保留寄存器1
   位域定义UNION结构:  SOC_PMCTRL_GPUPLLRESERVED1_UNION */
#define SOC_PMCTRL_GPUPLLRESERVED1_ADDR(base)         ((base) + (0x90C))





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
                     (1/1) register_define
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUPLLCTRL_UNION
 结构说明  : ACPUPLLCTRL 寄存器结构定义。地址偏移量:0x000，初值:0x20007800，宽度:32
 寄存器说明: ACPU PLL控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpupll_en_cfg  : 1;  /* bit[0-0]  : 软件配置PLL使能。
                                                           1'b1：使能；
                                                           1'b0：关闭。 */
        unsigned int  reserved_0      : 1;  /* bit[1-1]  : 保留 */
        unsigned int  acpupll_rst     : 1;  /* bit[2-2]  : PLL RST，高为复位态。 */
        unsigned int  reserved_1      : 1;  /* bit[3-3]  : 保留 */
        unsigned int  acpupll_time    : 24; /* bit[4-27] : PLL稳定时间，计数时钟为19.2M，默认为100us。 */
        unsigned int  acpupll_timeout : 1;  /* bit[28-28]: PLL稳定计时超时。 */
        unsigned int  acpupll_lock    : 1;  /* bit[29-29]: PLL锁定指示有效。 */
        unsigned int  reserved_2      : 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUPLLCTRL_UNION;
#endif
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_en_cfg_START   (0)
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_en_cfg_END     (0)
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_rst_START      (2)
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_rst_END        (2)
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_time_START     (4)
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_time_END       (27)
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_timeout_START  (28)
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_timeout_END    (28)
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_lock_START     (29)
#define SOC_PMCTRL_ACPUPLLCTRL_acpupll_lock_END       (29)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUPLLFREQ_UNION
 结构说明  : ACPUPLLFREQ 寄存器结构定义。地址偏移量:0x004，初值:0x000061E5，宽度:32
 寄存器说明: ACPU PLL频率寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpupll_freq_cfg : 32; /* bit[0-31]: [31:18] reserved
                                                           [17:12]：PLL后分频因子postdiv1。
                                                           [11:10]：reserved；
                                                           [9:4]：PLL FB programmable counter
                                                           [3:0]：PLL FB swallow counter。
                                                           PLL配置公式和约束：PLLout Clock=Cref*(2*P[5:0]+S[3:0])/POSTDIV[5:0];
                                                           约束：800Mhz <=Cref*(2*P[5:0]+S[3:0])<=2000Mhz */
    } reg;
} SOC_PMCTRL_ACPUPLLFREQ_UNION;
#endif
#define SOC_PMCTRL_ACPUPLLFREQ_acpupll_freq_cfg_START  (0)
#define SOC_PMCTRL_ACPUPLLFREQ_acpupll_freq_cfg_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_GPUPLLCTRL_UNION
 结构说明  : GPUPLLCTRL 寄存器结构定义。地址偏移量:0x008，初值:0x20007800，宽度:32
 寄存器说明: GPU PLL控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gpupll_en      : 1;  /* bit[0-0]  : 软件配置PLL使能。
                                                          1'b1：使能；
                                                          1'b0：关闭。 */
        unsigned int  reserved_0     : 1;  /* bit[1-1]  : 保留 */
        unsigned int  gpupll_rst     : 1;  /* bit[2-2]  : PLL RST，高为复位态。 */
        unsigned int  reserved_1     : 1;  /* bit[3-3]  : 保留 */
        unsigned int  gpupll_time    : 24; /* bit[4-27] : PLL稳定时间，计数时钟为19.2M，默认为100us。 */
        unsigned int  gpupll_timeout : 1;  /* bit[28-28]: PLL稳定计时超时。 */
        unsigned int  gpupll_lock    : 1;  /* bit[29-29]: PLL锁定指示有效。 */
        unsigned int  reserved_2     : 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_PMCTRL_GPUPLLCTRL_UNION;
#endif
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_en_START       (0)
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_en_END         (0)
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_rst_START      (2)
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_rst_END        (2)
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_time_START     (4)
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_time_END       (27)
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_timeout_START  (28)
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_timeout_END    (28)
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_lock_START     (29)
#define SOC_PMCTRL_GPUPLLCTRL_gpupll_lock_END       (29)


/*****************************************************************************
 结构名    : SOC_PMCTRL_GPUPLLFREQ_UNION
 结构说明  : GPUPLLFREQ 寄存器结构定义。地址偏移量:0x00C，初值:0x00001192，宽度:32
 寄存器说明: GPU PLL频率寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gpupll_freq : 32; /* bit[0-31]: [31:18] reserved
                                                      [17:12]：PLL后分频因子postdiv1。
                                                      [11:10]：reserved；
                                                      [9:4]：PLL FB programmable counter
                                                      [3:0]：PLL FB swallow counter。 */
    } reg;
} SOC_PMCTRL_GPUPLLFREQ_UNION;
#endif
#define SOC_PMCTRL_GPUPLLFREQ_gpupll_freq_START  (0)
#define SOC_PMCTRL_GPUPLLFREQ_gpupll_freq_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL1CTRL_UNION
 结构说明  : DDRPLL1CTRL 寄存器结构定义。地址偏移量:0x010，初值:0x00007800，宽度:32
 寄存器说明: DDR PLL1控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll1_en_cfg  : 1;  /* bit[0-0]  : 软件配置PLL使能。
                                                           1'b1：使能；
                                                           1'b0：关闭。 */
        unsigned int  reserved_0      : 2;  /* bit[1-2]  : 保留 */
        unsigned int  ddrpll1_en      : 1;  /* bit[3-3]  : PLL使能状态。 */
        unsigned int  ddrpll1_time    : 24; /* bit[4-27] : PLL稳定时间，计数时钟为19.2M，默认为100us。 */
        unsigned int  ddrpll1_timeout : 1;  /* bit[28-28]: PLL稳定计时超时。 */
        unsigned int  ddrpll1_lock    : 1;  /* bit[29-29]: PLL锁定指示有效。 */
        unsigned int  reserved_1      : 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRPLL1CTRL_UNION;
#endif
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_en_cfg_START   (0)
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_en_cfg_END     (0)
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_en_START       (3)
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_en_END         (3)
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_time_START     (4)
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_time_END       (27)
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_timeout_START  (28)
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_timeout_END    (28)
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_lock_START     (29)
#define SOC_PMCTRL_DDRPLL1CTRL_ddrpll1_lock_END       (29)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL1FREQ_UNION
 结构说明  : DDRPLL1FREQ 寄存器结构定义。地址偏移量:0x014，初值:0x51101037，宽度:32
 寄存器说明: DDR PLL1频率寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll1_freq : 32; /* bit[0-31]: [31]：PLL 旁路控制，高有效。
                                                       [30]：PLL FOUT4PHASEPD控制，高有效。
                                                       [29]：PLL FOUTPOSTDIVPD控制。高有效。
                                                       [28]：PLL FOUTVCOPD控制，高有效。
                                                       [27] reserved
                                                       [26:24]：PLL后分频因子postdiv2。
                                                       [23] reserved
                                                       [22:20]：PLL后分频因子postdiv1。
                                                       [19:18] reserved
                                                       [17:12]：PLL输入分频因子；
                                                       [11:0]：PLL反馈分频因子。
                                                       PLL配置公式和约束：FOUTPOSTDIV= Cref*（FBDIV+FRAC/2^24）/(REFDIV*POSTDIV1*POSTDIV2)
                                                       约束：400Mhz<=Cref*（FBDIV+FRAC/2^24）<=1600Mhz */
    } reg;
} SOC_PMCTRL_DDRPLL1FREQ_UNION;
#endif
#define SOC_PMCTRL_DDRPLL1FREQ_ddrpll1_freq_START  (0)
#define SOC_PMCTRL_DDRPLL1FREQ_ddrpll1_freq_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL0ALIAS_UNION
 结构说明  : DDRPLL0ALIAS 寄存器结构定义。地址偏移量:0x018，初值:0x00000000，宽度:32
 寄存器说明: DDRPLL0控制影子寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll0_en_alias : 1;  /* bit[0-0] : DDRPLL0使能影子寄存器。当MMC需要打开该PLL时配置该Bit。
                                                           1'b1：使能；
                                                           1'b0：关闭。 */
        unsigned int  reserved         : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRPLL0ALIAS_UNION;
#endif
#define SOC_PMCTRL_DDRPLL0ALIAS_ddrpll0_en_alias_START  (0)
#define SOC_PMCTRL_DDRPLL0ALIAS_ddrpll0_en_alias_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_SYSPLLCTRL_UNION
 结构说明  : SYSPLLCTRL 寄存器结构定义。地址偏移量:0x028，初值:0x00007800，宽度:32
 寄存器说明: SYS PLL控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  syspll_en_cfg  : 1;  /* bit[0-0]  : 软件配置PLL使能。
                                                          1'b1：使能；
                                                          1'b0：关闭。 */
        unsigned int  syspll_en_mod  : 1;  /* bit[1-1]  : 软件控制PLL使能方式(Hi6210项目该寄存器仅允许配置为0)。
                                                          0：由状态机控制；
                                                          1：由软件控制。 */
        unsigned int  reserved_0     : 1;  /* bit[2-2]  : 保留 */
        unsigned int  syspll_en      : 1;  /* bit[3-3]  : PLL使能状态。 */
        unsigned int  syspll_time    : 24; /* bit[4-27] : PLL稳定时间，计数时钟为19.2M，默认为100us。 */
        unsigned int  syspll_timeout : 1;  /* bit[28-28]: PLL稳定计时超时。 */
        unsigned int  syspll_lock    : 1;  /* bit[29-29]: PLL锁定指示有效。 */
        unsigned int  reserved_1     : 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_PMCTRL_SYSPLLCTRL_UNION;
#endif
#define SOC_PMCTRL_SYSPLLCTRL_syspll_en_cfg_START   (0)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_en_cfg_END     (0)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_en_mod_START   (1)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_en_mod_END     (1)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_en_START       (3)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_en_END         (3)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_time_START     (4)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_time_END       (27)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_timeout_START  (28)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_timeout_END    (28)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_lock_START     (29)
#define SOC_PMCTRL_SYSPLLCTRL_syspll_lock_END       (29)


/*****************************************************************************
 结构名    : SOC_PMCTRL_SYSPLLFREQ_UNION
 结构说明  : SYSPLLFREQ 寄存器结构定义。地址偏移量:0x02C，初值:0x5110103E，宽度:32
 寄存器说明: SYS PLL频率寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  syspll_freq : 32; /* bit[0-31]: [31]：PLL 旁路控制，高有效。
                                                      [30]：PLL FOUT4PHASEPD控制，高有效。
                                                      [29]：PLL FOUTPOSTDIVPD控制。高有效。
                                                      [28]：PLL FOUTVCOPD控制，高有效。
                                                      [27] reserved
                                                      [26:24]：PLL后分频因子postdiv2。
                                                      [23] reserved
                                                      [22:20]：PLL后分频因子postdiv1。
                                                      [19:18] reserved
                                                      [17:12]：PLL输入分频因子；
                                                      [11:0]：PLL反馈分频因子。 */
    } reg;
} SOC_PMCTRL_SYSPLLFREQ_UNION;
#endif
#define SOC_PMCTRL_SYSPLLFREQ_syspll_freq_START  (0)
#define SOC_PMCTRL_SYSPLLFREQ_syspll_freq_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL0CTRL_UNION
 结构说明  : DDRPLL0CTRL 寄存器结构定义。地址偏移量:0x030，初值:0x00007800，宽度:32
 寄存器说明: DDR PLL0控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll0_en_cfg  : 1;  /* bit[0-0]  : 软件配置PLL使能。当DDR DFS要使用此PLL时打开此bit。
                                                           1'b1：使能；
                                                           1'b0：关闭。 */
        unsigned int  reserved_0      : 2;  /* bit[1-2]  : 保留 */
        unsigned int  ddrpll0_en      : 1;  /* bit[3-3]  : PLL使能状态。 */
        unsigned int  ddrpll0_time    : 24; /* bit[4-27] : PLL稳定时间，计数时钟为19.2M，默认为100us。 */
        unsigned int  ddrpll0_timeout : 1;  /* bit[28-28]: PLL稳定计时超时。 */
        unsigned int  ddrpll0_lock    : 1;  /* bit[29-29]: PLL锁定指示有效。 */
        unsigned int  reserved_1      : 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRPLL0CTRL_UNION;
#endif
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_en_cfg_START   (0)
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_en_cfg_END     (0)
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_en_START       (3)
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_en_END         (3)
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_time_START     (4)
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_time_END       (27)
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_timeout_START  (28)
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_timeout_END    (28)
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_lock_START     (29)
#define SOC_PMCTRL_DDRPLL0CTRL_ddrpll0_lock_END       (29)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL0FREQ_UNION
 结构说明  : DDRPLL0FREQ 寄存器结构定义。地址偏移量:0x034，初值:0x51101053，宽度:32
 寄存器说明: DDR PLL0频率寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll0_freq : 32; /* bit[0-31]: [31]：PLL 旁路控制，高有效。
                                                       [30]：PLL FOUT4PHASEPD控制，高有效。
                                                       [29]：PLL FOUTPOSTDIVPD控制。高有效。
                                                       [28]：PLL FOUTVCOPD控制，高有效。
                                                       [27] reserved
                                                       [26:24]：PLL后分频因子postdiv2。
                                                       [23] reserved
                                                       [22:20]：PLL后分频因子postdiv1。
                                                       [19:18] reserved
                                                       [17:12]：PLL输入分频因子；
                                                       [11:0]：PLL反馈分频因子。 */
    } reg;
} SOC_PMCTRL_DDRPLL0FREQ_UNION;
#endif
#define SOC_PMCTRL_DDRPLL0FREQ_ddrpll0_freq_START  (0)
#define SOC_PMCTRL_DDRPLL0FREQ_ddrpll0_freq_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_MEDPLLCTRL_UNION
 结构说明  : MEDPLLCTRL 寄存器结构定义。地址偏移量:0x038，初值:0x20007800，宽度:32
 寄存器说明: MED PLL控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  medpll_en_cfg  : 1;  /* bit[0-0]  : 软件配置PLL使能。当ACPU需要打开该PLL时配置该Bit
                                                          1'b1：使能；
                                                          1'b0：关闭。 */
        unsigned int  reserved_0     : 1;  /* bit[1-1]  : 保留 */
        unsigned int  medpll_rst     : 1;  /* bit[2-2]  : PLL RST，高为复位态。 */
        unsigned int  medpll_en      : 1;  /* bit[3-3]  : PLL使能状态位。 */
        unsigned int  medpll_time    : 24; /* bit[4-27] : PLL稳定时间，计数时钟为19.2M，默认为100us。 */
        unsigned int  medpll_timeout : 1;  /* bit[28-28]: PLL稳定计时超时。 */
        unsigned int  medpll_lock    : 1;  /* bit[29-29]: PLL锁定指示有效。 */
        unsigned int  reserved_1     : 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_PMCTRL_MEDPLLCTRL_UNION;
#endif
#define SOC_PMCTRL_MEDPLLCTRL_medpll_en_cfg_START   (0)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_en_cfg_END     (0)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_rst_START      (2)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_rst_END        (2)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_en_START       (3)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_en_END         (3)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_time_START     (4)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_time_END       (27)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_timeout_START  (28)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_timeout_END    (28)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_lock_START     (29)
#define SOC_PMCTRL_MEDPLLCTRL_medpll_lock_END       (29)


/*****************************************************************************
 结构名    : SOC_PMCTRL_MEDPLLFREQ_UNION
 结构说明  : MEDPLLFREQ 寄存器结构定义。地址偏移量:0x03C，初值:0x00001251，宽度:32
 寄存器说明: MED PLL频率寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  medpll_freq : 32; /* bit[0-31]: [31:18] reserved
                                                      [17:12]：PLL后分频因子postdiv1。
                                                      [11:10]：reserved；
                                                      [9:4]：PLL FB programmable counter
                                                      [3:0]：PLL FB swallow counter。 */
    } reg;
} SOC_PMCTRL_MEDPLLFREQ_UNION;
#endif
#define SOC_PMCTRL_MEDPLLFREQ_medpll_freq_START  (0)
#define SOC_PMCTRL_MEDPLLFREQ_medpll_freq_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_MEDPLLALIAS_UNION
 结构说明  : MEDPLLALIAS 寄存器结构定义。地址偏移量:0x040，初值:0x00000000，宽度:32
 寄存器说明: MEDPLL控制影子寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  medpll_en_alias : 1;  /* bit[0-0] : MED PLL使能影子寄存器。当CCPU需要打开该PLL时配置该Bit。
                                                          1'b1：使能；
                                                          1'b0：关闭。 */
        unsigned int  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_MEDPLLALIAS_UNION;
#endif
#define SOC_PMCTRL_MEDPLLALIAS_medpll_en_alias_START  (0)
#define SOC_PMCTRL_MEDPLLALIAS_medpll_en_alias_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_BBPPLLALIAS_UNION
 结构说明  : BBPPLLALIAS 寄存器结构定义。地址偏移量:0x048，初值:0x00000000，宽度:32
 寄存器说明: BBPPLL控制影子寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bbppll_en_alias : 1;  /* bit[0-0] : BBP PLL使能影子寄存器。当ACPU需要打开该PLL时配置该Bit。
                                                          1'b1：使能；
                                                          1'b0：关闭。 */
        unsigned int  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_BBPPLLALIAS_UNION;
#endif
#define SOC_PMCTRL_BBPPLLALIAS_bbppll_en_alias_START  (0)
#define SOC_PMCTRL_BBPPLLALIAS_bbppll_en_alias_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL0CFGEN_UNION
 结构说明  : DDRPLL0CFGEN 寄存器结构定义。地址偏移量:0x04C，初值:0x00000000，宽度:32
 寄存器说明: DDRPLL0配置使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll0_cfg_en : 1;  /* bit[0-0] : DDR PLL0参数配置使能寄存器。当软件配置完ddrpll0_freq后，再配置该寄存器为1。硬件根据此信号决定采ddrpll0_freq的时间。读始终为0； */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRPLL0CFGEN_UNION;
#endif
#define SOC_PMCTRL_DDRPLL0CFGEN_ddrpll0_cfg_en_START  (0)
#define SOC_PMCTRL_DDRPLL0CFGEN_ddrpll0_cfg_en_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_SYSPLLFRAC_UNION
 结构说明  : SYSPLLFRAC 寄存器结构定义。地址偏移量:0x050，初值:0x08000005，宽度:32
 寄存器说明: SYS PLL小数分频寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sys_frac : 29; /* bit[0-28] : [28]：DSMPD；
                                                    [27:4]：FRAC；
                                                    [3:0]：DIVVAL，该值不允许配置为0x0或者0x1。 */
        unsigned int  reserved : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_SYSPLLFRAC_UNION;
#endif
#define SOC_PMCTRL_SYSPLLFRAC_sys_frac_START  (0)
#define SOC_PMCTRL_SYSPLLFRAC_sys_frac_END    (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_SYSPLLSSC_UNION
 结构说明  : SYSPLLSSC 寄存器结构定义。地址偏移量:0x054，初值:0x00000010，宽度:32
 寄存器说明: SYS PLL扩频控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sys_ssc  : 6;  /* bit[0-5] : [5]：RESET；
                                                   [4]：DISABLE_SSCG；
                                                   [3]：DOWNSPREAD；
                                                   [2:0]：SPREAD。 */
        unsigned int  reserved : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_PMCTRL_SYSPLLSSC_UNION;
#endif
#define SOC_PMCTRL_SYSPLLSSC_sys_ssc_START   (0)
#define SOC_PMCTRL_SYSPLLSSC_sys_ssc_END     (5)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL0FRAC_UNION
 结构说明  : DDRPLL0FRAC 寄存器结构定义。地址偏移量:0x058，初值:0x05555555，宽度:32
 寄存器说明: DDR PLL0小数分频寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll0_frac : 29; /* bit[0-28] : [28]：DSMPD；
                                                        [27:4]：FRAC；
                                                        [3:0]：DIVVAL，该值不允许配置为0x0或者0x1。 */
        unsigned int  reserved     : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRPLL0FRAC_UNION;
#endif
#define SOC_PMCTRL_DDRPLL0FRAC_ddrpll0_frac_START  (0)
#define SOC_PMCTRL_DDRPLL0FRAC_ddrpll0_frac_END    (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL0SSC_UNION
 结构说明  : DDRPLL0SSC 寄存器结构定义。地址偏移量:0x05C，初值:0x00000010，宽度:32
 寄存器说明: DDR PLL0扩频控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll0_ssc : 6;  /* bit[0-5] : [5]：RESET；
                                                      [4]：DISABLE_SSCG；
                                                      [3]：DOWNSPREAD；
                                                      [2:0]：SPREAD。 */
        unsigned int  reserved    : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRPLL0SSC_UNION;
#endif
#define SOC_PMCTRL_DDRPLL0SSC_ddrpll0_ssc_START  (0)
#define SOC_PMCTRL_DDRPLL0SSC_ddrpll0_ssc_END    (5)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL1FRAC_UNION
 结构说明  : DDRPLL1FRAC 寄存器结构定义。地址偏移量:0x074，初值:0x08555555，宽度:32
 寄存器说明: DDR PLL1小数分频寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll1_frac : 29; /* bit[0-28] : [28]：DSMPD；
                                                        [27:4]：FRAC；
                                                        [3:0]：DIVVAL，该值不允许配置为0x0或者0x1。 */
        unsigned int  reserved     : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRPLL1FRAC_UNION;
#endif
#define SOC_PMCTRL_DDRPLL1FRAC_ddrpll1_frac_START  (0)
#define SOC_PMCTRL_DDRPLL1FRAC_ddrpll1_frac_END    (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLL1SSC_UNION
 结构说明  : DDRPLL1SSC 寄存器结构定义。地址偏移量:0x078，初值:0x00000010，宽度:32
 寄存器说明: DDR PLL1扩频控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll1_ssc : 6;  /* bit[0-5] : [5]：RESET；
                                                      [4]：DISABLE_SSCG；
                                                      [3]：DOWNSPREAD；
                                                      [2:0]：SPREAD。 */
        unsigned int  reserved    : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRPLL1SSC_UNION;
#endif
#define SOC_PMCTRL_DDRPLL1SSC_ddrpll1_ssc_START  (0)
#define SOC_PMCTRL_DDRPLL1SSC_ddrpll1_ssc_END    (5)


/*****************************************************************************
 结构名    : SOC_PMCTRL_CLKCFG4BIT0_UNION
 结构说明  : CLKCFG4BIT0 寄存器结构定义。地址偏移量:0x080，初值:0x00000000，宽度:32
 寄存器说明: 时钟参数配置寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_4bit_cfg0_value0 : 3;  /* bit[0-2]  : 时钟参数。保留 */
        unsigned int  clk_4bit_cfg0_vld0   : 1;  /* bit[3-3]  : 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg0_value1 : 3;  /* bit[4-6]  : 时钟参数。保留 */
        unsigned int  clk_4bit_cfg0_vld1   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg0_value2 : 3;  /* bit[8-10] : 时钟参数。保留 */
        unsigned int  clk_4bit_cfg0_vld2   : 1;  /* bit[11-11]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg0_value3 : 3;  /* bit[12-14]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg0_vld3   : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg0_value4 : 3;  /* bit[16-18]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg0_vld4   : 1;  /* bit[19-19]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg0_value5 : 3;  /* bit[20-22]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg0_vld5   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg0_value6 : 3;  /* bit[24-26]: 时钟参数。保留。 */
        unsigned int  clk_4bit_cfg0_vld6   : 1;  /* bit[27-27]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg0_value7 : 3;  /* bit[28-30]: 时钟参数。保留。 */
        unsigned int  clk_4bit_cfg0_vld7   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PMCTRL_CLKCFG4BIT0_UNION;
#endif
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value0_START  (0)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value0_END    (2)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld0_START    (3)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld0_END      (3)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value1_START  (4)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value1_END    (6)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld1_START    (7)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld1_END      (7)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value2_START  (8)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value2_END    (10)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld2_START    (11)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld2_END      (11)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value3_START  (12)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value3_END    (14)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld3_START    (15)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld3_END      (15)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value4_START  (16)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value4_END    (18)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld4_START    (19)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld4_END      (19)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value5_START  (20)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value5_END    (22)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld5_START    (23)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld5_END      (23)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value6_START  (24)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value6_END    (26)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld6_START    (27)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld6_END      (27)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value7_START  (28)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_value7_END    (30)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld7_START    (31)
#define SOC_PMCTRL_CLKCFG4BIT0_clk_4bit_cfg0_vld7_END      (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_CLKCFG4BIT1_UNION
 结构说明  : CLKCFG4BIT1 寄存器结构定义。地址偏移量:0x084，初值:0x00000000，宽度:32
 寄存器说明: 时钟参数配置寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_4bit_cfg1_value0 : 3;  /* bit[0-2]  : 时钟参数。保留 */
        unsigned int  clk_4bit_cfg1_vld0   : 1;  /* bit[3-3]  : 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg1_value1 : 3;  /* bit[4-6]  : 时钟参数。保留 */
        unsigned int  clk_4bit_cfg1_vld1   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg1_value2 : 3;  /* bit[8-10] : 时钟参数。保留 */
        unsigned int  clk_4bit_cfg1_vld2   : 1;  /* bit[11-11]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg1_value3 : 3;  /* bit[12-14]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg1_vld3   : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg1_value4 : 3;  /* bit[16-18]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg1_vld4   : 1;  /* bit[19-19]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg1_value5 : 3;  /* bit[20-22]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg1_vld5   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg1_value6 : 3;  /* bit[24-26]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg1_vld6   : 1;  /* bit[27-27]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg1_value7 : 3;  /* bit[28-30]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg1_vld7   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PMCTRL_CLKCFG4BIT1_UNION;
#endif
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value0_START  (0)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value0_END    (2)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld0_START    (3)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld0_END      (3)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value1_START  (4)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value1_END    (6)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld1_START    (7)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld1_END      (7)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value2_START  (8)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value2_END    (10)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld2_START    (11)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld2_END      (11)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value3_START  (12)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value3_END    (14)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld3_START    (15)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld3_END      (15)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value4_START  (16)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value4_END    (18)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld4_START    (19)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld4_END      (19)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value5_START  (20)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value5_END    (22)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld5_START    (23)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld5_END      (23)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value6_START  (24)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value6_END    (26)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld6_START    (27)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld6_END      (27)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value7_START  (28)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_value7_END    (30)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld7_START    (31)
#define SOC_PMCTRL_CLKCFG4BIT1_clk_4bit_cfg1_vld7_END      (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_CLKCFG4BIT2_UNION
 结构说明  : CLKCFG4BIT2 寄存器结构定义。地址偏移量:0x088，初值:0x00000000，宽度:32
 寄存器说明: 时钟参数配置寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_4bit_cfg2_value0 : 3;  /* bit[0-2]  : 时钟参数。保留 */
        unsigned int  clk_4bit_cfg2_vld0   : 1;  /* bit[3-3]  : 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg2_value1 : 3;  /* bit[4-6]  : 时钟参数。保留 */
        unsigned int  clk_4bit_cfg2_vld1   : 1;  /* bit[7-7]  : 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg2_value2 : 3;  /* bit[8-10] : 时钟参数。保留。 */
        unsigned int  clk_4bit_cfg2_vld2   : 1;  /* bit[11-11]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg2_value3 : 3;  /* bit[12-14]: 时钟参数。保留。 */
        unsigned int  clk_4bit_cfg2_vld3   : 1;  /* bit[15-15]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg2_value4 : 3;  /* bit[16-18]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg2_vld4   : 1;  /* bit[19-19]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg2_value5 : 3;  /* bit[20-22]: 时钟参数。保留 */
        unsigned int  clk_4bit_cfg2_vld5   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg2_value6 : 3;  /* bit[24-26]: 时钟参数。保留。 */
        unsigned int  clk_4bit_cfg2_vld6   : 1;  /* bit[27-27]: 时钟参数有效指示。 */
        unsigned int  clk_4bit_cfg2_value7 : 3;  /* bit[28-30]: 时钟参数。保留。 */
        unsigned int  clk_4bit_cfg2_vld7   : 1;  /* bit[31-31]: 时钟参数有效指示。 */
    } reg;
} SOC_PMCTRL_CLKCFG4BIT2_UNION;
#endif
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value0_START  (0)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value0_END    (2)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld0_START    (3)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld0_END      (3)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value1_START  (4)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value1_END    (6)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld1_START    (7)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld1_END      (7)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value2_START  (8)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value2_END    (10)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld2_START    (11)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld2_END      (11)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value3_START  (12)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value3_END    (14)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld3_START    (15)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld3_END      (15)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value4_START  (16)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value4_END    (18)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld4_START    (19)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld4_END      (19)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value5_START  (20)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value5_END    (22)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld5_START    (23)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld5_END      (23)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value6_START  (24)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value6_END    (26)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld6_START    (27)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld6_END      (27)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value7_START  (28)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_value7_END    (30)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld7_START    (31)
#define SOC_PMCTRL_CLKCFG4BIT2_clk_4bit_cfg2_vld7_END      (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_TIMER0CTRL_UNION
 结构说明  : TIMER0CTRL 寄存器结构定义。地址偏移量:0x0C0，初值:0x00000000，宽度:32
 寄存器说明: TIMER0控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer0_en      : 1;  /* bit[0-0]  : 定时使能，高有效。 */
        unsigned int  reserved_0     : 3;  /* bit[1-3]  : 保留 */
        unsigned int  timer0_val     : 24; /* bit[4-27] : 定时时间，单位为TCXO周期。 */
        unsigned int  timer0_timeout : 1;  /* bit[28-28]: 定时器超时。 */
        unsigned int  reserved_1     : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_TIMER0CTRL_UNION;
#endif
#define SOC_PMCTRL_TIMER0CTRL_timer0_en_START       (0)
#define SOC_PMCTRL_TIMER0CTRL_timer0_en_END         (0)
#define SOC_PMCTRL_TIMER0CTRL_timer0_val_START      (4)
#define SOC_PMCTRL_TIMER0CTRL_timer0_val_END        (27)
#define SOC_PMCTRL_TIMER0CTRL_timer0_timeout_START  (28)
#define SOC_PMCTRL_TIMER0CTRL_timer0_timeout_END    (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_TIMER1CTRL_UNION
 结构说明  : TIMER1CTRL 寄存器结构定义。地址偏移量:0x0C4，初值:0x00000000，宽度:32
 寄存器说明: TIMER1控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer1_en      : 1;  /* bit[0-0]  : 定时使能，高有效。 */
        unsigned int  reserved_0     : 3;  /* bit[1-3]  : 保留 */
        unsigned int  timer1_val     : 24; /* bit[4-27] : 定时时间，单位为TCXO周期。 */
        unsigned int  timer1_timeout : 1;  /* bit[28-28]: 定时器超时。 */
        unsigned int  reserved_1     : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_TIMER1CTRL_UNION;
#endif
#define SOC_PMCTRL_TIMER1CTRL_timer1_en_START       (0)
#define SOC_PMCTRL_TIMER1CTRL_timer1_en_END         (0)
#define SOC_PMCTRL_TIMER1CTRL_timer1_val_START      (4)
#define SOC_PMCTRL_TIMER1CTRL_timer1_val_END        (27)
#define SOC_PMCTRL_TIMER1CTRL_timer1_timeout_START  (28)
#define SOC_PMCTRL_TIMER1CTRL_timer1_timeout_END    (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_TIMER2CTRL_UNION
 结构说明  : TIMER2CTRL 寄存器结构定义。地址偏移量:0x0C8，初值:0x00000000，宽度:32
 寄存器说明: TIMER2控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer2_en      : 1;  /* bit[0-0]  : 定时使能，高有效。 */
        unsigned int  reserved_0     : 3;  /* bit[1-3]  : 保留 */
        unsigned int  timer2_val     : 24; /* bit[4-27] : 定时时间，单位为TCXO周期。 */
        unsigned int  timer2_timeout : 1;  /* bit[28-28]: 定时器超时。 */
        unsigned int  reserved_1     : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_TIMER2CTRL_UNION;
#endif
#define SOC_PMCTRL_TIMER2CTRL_timer2_en_START       (0)
#define SOC_PMCTRL_TIMER2CTRL_timer2_en_END         (0)
#define SOC_PMCTRL_TIMER2CTRL_timer2_val_START      (4)
#define SOC_PMCTRL_TIMER2CTRL_timer2_val_END        (27)
#define SOC_PMCTRL_TIMER2CTRL_timer2_timeout_START  (28)
#define SOC_PMCTRL_TIMER2CTRL_timer2_timeout_END    (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_TIMER3CTRL_UNION
 结构说明  : TIMER3CTRL 寄存器结构定义。地址偏移量:0x0CC，初值:0x00000000，宽度:32
 寄存器说明: TIMER3控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer3_en      : 1;  /* bit[0-0]  : 定时使能，高有效。 */
        unsigned int  reserved_0     : 3;  /* bit[1-3]  : 保留 */
        unsigned int  timer3_val     : 24; /* bit[4-27] : 定时时间，单位为TCXO周期。 */
        unsigned int  timer3_timeout : 1;  /* bit[28-28]: 定时器超时 */
        unsigned int  reserved_1     : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_TIMER3CTRL_UNION;
#endif
#define SOC_PMCTRL_TIMER3CTRL_timer3_en_START       (0)
#define SOC_PMCTRL_TIMER3CTRL_timer3_en_END         (0)
#define SOC_PMCTRL_TIMER3CTRL_timer3_val_START      (4)
#define SOC_PMCTRL_TIMER3CTRL_timer3_val_END        (27)
#define SOC_PMCTRL_TIMER3CTRL_timer3_timeout_START  (28)
#define SOC_PMCTRL_TIMER3CTRL_timer3_timeout_END    (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMUSSICHNEN_UNION
 结构说明  : PMUSSICHNEN 寄存器结构定义。地址偏移量:0x0FC，初值:0x00000001，宽度:32
 寄存器说明: PMUSSI通道使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pmussi_chn_en : 1;  /* bit[0-0] : PMUSSI通道使能总控寄存器。 */
        unsigned int  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMUSSICHNEN_UNION;
#endif
#define SOC_PMCTRL_PMUSSICHNEN_pmussi_chn_en_START  (0)
#define SOC_PMCTRL_PMUSSICHNEN_pmussi_chn_en_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUPLLSEL_UNION
 结构说明  : ACPUPLLSEL 寄存器结构定义。地址偏移量:0x100，初值:0x00000000，宽度:32
 寄存器说明: ACPU PLL选择寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_pllsw_cfg  : 1;  /* bit[0-0] : CPU时钟选择哪个PLL。
                                                          0：CPU时钟选择ACPUPLL；
                                                          1：CPU时钟选择SYSPLL。 */
        unsigned int  acpu_pllsw_stat : 1;  /* bit[1-1] : 当前CPU时钟选择哪个PLL的状态指示。
                                                          0：CPU时钟未选择ACPUPLL；
                                                          1：CPU时钟选择ACPUPLL。 */
        unsigned int  syspll_sw_stat  : 1;  /* bit[2-2] : 当前CPU时钟选择哪个PLL的状态指示。
                                                          0：CPU时钟未选择SYSPLL；
                                                          1：CPU时钟选择SYSPLL。 */
        unsigned int  reserved        : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUPLLSEL_UNION;
#endif
#define SOC_PMCTRL_ACPUPLLSEL_acpu_pllsw_cfg_START   (0)
#define SOC_PMCTRL_ACPUPLLSEL_acpu_pllsw_cfg_END     (0)
#define SOC_PMCTRL_ACPUPLLSEL_acpu_pllsw_stat_START  (1)
#define SOC_PMCTRL_ACPUPLLSEL_acpu_pllsw_stat_END    (1)
#define SOC_PMCTRL_ACPUPLLSEL_syspll_sw_stat_START   (2)
#define SOC_PMCTRL_ACPUPLLSEL_syspll_sw_stat_END     (2)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUCLKDIV_UNION
 结构说明  : ACPUCLKDIV 寄存器结构定义。地址偏移量:0x104，初值:0x00010001，宽度:32
 寄存器说明: ACPU时钟分频寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cpuext_clk_div_cfg    : 2;  /* bit[0-1]  : 软件配置的ACPU bus接口时钟分频参数。 */
        unsigned int  reserved_0            : 6;  /* bit[2-7]  : 保留 */
        unsigned int  acpu_ddr_clk_div_cfg  : 2;  /* bit[8-9]  : ACPU送给DDR异步桥的时钟分频参数。 */
        unsigned int  reserved_1            : 6;  /* bit[10-15]: 保留。 */
        unsigned int  cpuext_clk_div_stat   : 2;  /* bit[16-17]: 系统使用的ACPU bus接口时钟分频参数。 */
        unsigned int  reserved_2            : 6;  /* bit[18-23]: 保留 */
        unsigned int  acpu_ddr_clk_div_stat : 2;  /* bit[24-25]: ACPU送给DDR异步桥的时钟分频参数。 */
        unsigned int  reserved_3            : 6;  /* bit[26-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUCLKDIV_UNION;
#endif
#define SOC_PMCTRL_ACPUCLKDIV_cpuext_clk_div_cfg_START     (0)
#define SOC_PMCTRL_ACPUCLKDIV_cpuext_clk_div_cfg_END       (1)
#define SOC_PMCTRL_ACPUCLKDIV_acpu_ddr_clk_div_cfg_START   (8)
#define SOC_PMCTRL_ACPUCLKDIV_acpu_ddr_clk_div_cfg_END     (9)
#define SOC_PMCTRL_ACPUCLKDIV_cpuext_clk_div_stat_START    (16)
#define SOC_PMCTRL_ACPUCLKDIV_cpuext_clk_div_stat_END      (17)
#define SOC_PMCTRL_ACPUCLKDIV_acpu_ddr_clk_div_stat_START  (24)
#define SOC_PMCTRL_ACPUCLKDIV_acpu_ddr_clk_div_stat_END    (25)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUMEMRM_UNION
 结构说明  : ACPUMEMRM 寄存器结构定义。地址偏移量:0x108，初值:0x009B009B，宽度:32
 寄存器说明: ACPU存储体RM参数寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_memrm_cfg  : 8;  /* bit[0-7]  : 软件配置的ACPU存储体RM参数。
                                                           [7]：未定义；
                                                           [6]：EMAS；
                                                           [5:4]：EMAW；
                                                           [3]：未定义；
                                                           [2:0]：EMA。 */
        unsigned int  reserved_0      : 8;  /* bit[8-15] : 保留 */
        unsigned int  acpu_memrm_stat : 8;  /* bit[16-23]: 系统使用的ACPU存储体RM参数。
                                                           [23]：未定义；
                                                           [22]：EMAS；
                                                           [21:20]：EMAW；
                                                           [19]：未定义；
                                                           [18:16]：EMA。 */
        unsigned int  reserved_1      : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUMEMRM_UNION;
#endif
#define SOC_PMCTRL_ACPUMEMRM_acpu_memrm_cfg_START   (0)
#define SOC_PMCTRL_ACPUMEMRM_acpu_memrm_cfg_END     (7)
#define SOC_PMCTRL_ACPUMEMRM_acpu_memrm_stat_START  (16)
#define SOC_PMCTRL_ACPUMEMRM_acpu_memrm_stat_END    (23)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUVOLDET_UNION
 结构说明  : ACPUVOLDET 寄存器结构定义。地址偏移量:0x10C，初值:0x00000000，宽度:32
 寄存器说明: ACPU电压检测寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_tune_cfg    : 5;  /* bit[0-4] : 软件配置的ACPU tune值。 */
        unsigned int  reserved_0       : 3;  /* bit[5-7] : 保留 */
        unsigned int  acpu_tune_en_cfg : 1;  /* bit[8-8] : ACPU的VOL DET功能使能中断上报。 */
        unsigned int  reserved_1       : 23; /* bit[9-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUVOLDET_UNION;
#endif
#define SOC_PMCTRL_ACPUVOLDET_acpu_tune_cfg_START     (0)
#define SOC_PMCTRL_ACPUVOLDET_acpu_tune_cfg_END       (4)
#define SOC_PMCTRL_ACPUVOLDET_acpu_tune_en_cfg_START  (8)
#define SOC_PMCTRL_ACPUVOLDET_acpu_tune_en_cfg_END    (8)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUSYSPLLCFG_UNION
 结构说明  : ACPUSYSPLLCFG 寄存器结构定义。地址偏移量:0x110，初值:0x00170117，宽度:32
 寄存器说明: ACPU侧SYSPLL的配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_syspll_div_cfg     : 3;  /* bit[0-2]  : 软件配置的ACPU子系统的SYSPLL时钟分频参数； */
        unsigned int  reserved_0              : 1;  /* bit[3-3]  : 保留 */
        unsigned int  acpu_syspll_clken_cfg   : 1;  /* bit[4-4]  : 软件配置的ACPU子系统的SYSPLL时钟分频后的门控； */
        unsigned int  reserved_1              : 3;  /* bit[5-7]  : 保留 */
        unsigned int  acpu_subsys_clk_div_cfg : 2;  /* bit[8-9]  : ACPU DVFS时，ACPU子系统的时钟分频参数； */
        unsigned int  reserved_2              : 6;  /* bit[10-15]: 保留 */
        unsigned int  acpu_syspll_div_stat    : 4;  /* bit[16-19]: 软件配置的ACPU子系统的SYSPLL时钟分频参数； */
        unsigned int  acpu_syspll_clken_stat  : 1;  /* bit[20-20]: 软件配置的ACPU子系统的SYSPLL时钟分频后的门控； */
        unsigned int  reserved_3              : 11; /* bit[21-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUSYSPLLCFG_UNION;
#endif
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_syspll_div_cfg_START      (0)
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_syspll_div_cfg_END        (2)
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_syspll_clken_cfg_START    (4)
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_syspll_clken_cfg_END      (4)
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_subsys_clk_div_cfg_START  (8)
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_subsys_clk_div_cfg_END    (9)
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_syspll_div_stat_START     (16)
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_syspll_div_stat_END       (19)
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_syspll_clken_stat_START   (20)
#define SOC_PMCTRL_ACPUSYSPLLCFG_acpu_syspll_clken_stat_END     (20)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUCLKOFFCFG_UNION
 结构说明  : ACPUCLKOFFCFG 寄存器结构定义。地址偏移量:0x114，初值:0x00000000，宽度:32
 寄存器说明: ACPU DVFS时钟模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_clkoff_vol      : 1;  /* bit[0-0]  : 控制CPU DVFS过程中，电压变化时时钟的状态
                                                                1'b0：CPU 电压改变时，时钟打开；
                                                                1'b1：CPU 电压改变时，时钟关闭。 */
        unsigned int  acpu_clkoff_memrm    : 1;  /* bit[1-1]  : 控制CPU DVFS过程中，RM变化时时钟的状态
                                                                1'b0：CPU RM改变时，时钟打开；
                                                                1'b1：CPU RM改变时，时钟关闭。 */
        unsigned int  reserved_0           : 2;  /* bit[2-3]  : 保留 */
        unsigned int  acpu_memrm_time      : 5;  /* bit[4-8]  : CPU DVFS时，改变MEM RM的稳定时间。 */
        unsigned int  reserved_1           : 3;  /* bit[9-11] : 保留 */
        unsigned int  acpu_dvfs_pwrctrl_en : 1;  /* bit[12-12]: CPU DVFS调频调压使能。
                                                                0：CPU DVFS调频调压功能关闭；
                                                                1：CPU DVFS调频调压功能开启。 */
        unsigned int  reserved_2           : 3;  /* bit[13-15]: 保留 */
        unsigned int  acpu_dvfs_ko_avs_dis : 1;  /* bit[16-16]: AVS电压调整强制打断禁止寄存器。
                                                                0：AVS电压调整过程中，允许DVFS插入；
                                                                1：AVS电压调整过程中，不允许DVFS插入。 */
        unsigned int  reserved_3           : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUCLKOFFCFG_UNION;
#endif
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_clkoff_vol_START       (0)
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_clkoff_vol_END         (0)
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_clkoff_memrm_START     (1)
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_clkoff_memrm_END       (1)
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_memrm_time_START       (4)
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_memrm_time_END         (8)
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_dvfs_pwrctrl_en_START  (12)
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_dvfs_pwrctrl_en_END    (12)
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_dvfs_ko_avs_dis_START  (16)
#define SOC_PMCTRL_ACPUCLKOFFCFG_acpu_dvfs_ko_avs_dis_END    (16)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUDVFSEN_UNION
 结构说明  : ACPUDVFSEN 寄存器结构定义。地址偏移量:0x118，初值:0x00000000，宽度:32
 寄存器说明: ACPU DVFS使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_dvfs_start   : 1;  /* bit[0-0] : CPU DVFS启动使能，向该位写1'b1启动1次DVFS，写0无效。 */
        unsigned int  acpu_profile_updn : 1;  /* bit[1-1] : CPU DVFS性能改变指示。
                                                            0：CPU DVFS降低Profile；
                                                            1：CPU DVFS提升Profile。 */
        unsigned int  reserved          : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUDVFSEN_UNION;
#endif
#define SOC_PMCTRL_ACPUDVFSEN_acpu_dvfs_start_START    (0)
#define SOC_PMCTRL_ACPUDVFSEN_acpu_dvfs_start_END      (0)
#define SOC_PMCTRL_ACPUDVFSEN_acpu_profile_updn_START  (1)
#define SOC_PMCTRL_ACPUDVFSEN_acpu_profile_updn_END    (1)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUCLKPROFILE0_UNION
 结构说明  : ACPUCLKPROFILE0 寄存器结构定义。地址偏移量:0x11C，初值:0x000061E5，宽度:32
 寄存器说明: ACPU频率Profile寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpupll_freq_prof : 28; /* bit[0-27] : [27:18]：reserved
                                                             [17:12]：PLL后分频因子postdiv1。
                                                             [11:10]：reserved；
                                                             [9:4]：PLL FB programmable counter。
                                                             [3:0]：PLL FB swallow counter。 */
        unsigned int  reserved          : 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUCLKPROFILE0_UNION;
#endif
#define SOC_PMCTRL_ACPUCLKPROFILE0_acpupll_freq_prof_START  (0)
#define SOC_PMCTRL_ACPUCLKPROFILE0_acpupll_freq_prof_END    (27)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUCLKPROFILE1_UNION
 结构说明  : ACPUCLKPROFILE1 寄存器结构定义。地址偏移量:0x120，初值:0x00000107，宽度:32
 寄存器说明: ACPU频率Profile寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_sys_div_prof     : 3;  /* bit[0-2]  : ACPU侧syspll的时钟分频参数。 */
        unsigned int  reserved_0            : 1;  /* bit[3-3]  : 保留 */
        unsigned int  acpu_ddr_clk_div_prof : 2;  /* bit[4-5]  : ACPU送给DDR异步桥的时钟分频参数。
                                                                 ACPU降压为0.95V时，配置为0x1；
                                                                 ACPU升压为1.05V时，配置为0x0； */
        unsigned int  reserved_1            : 2;  /* bit[6-7]  : 保留 */
        unsigned int  cpuext_clk_div_prof   : 2;  /* bit[8-9]  : ACPU BUS接口时钟分频参数。 */
        unsigned int  reserved_2            : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUCLKPROFILE1_UNION;
#endif
#define SOC_PMCTRL_ACPUCLKPROFILE1_acpu_sys_div_prof_START      (0)
#define SOC_PMCTRL_ACPUCLKPROFILE1_acpu_sys_div_prof_END        (2)
#define SOC_PMCTRL_ACPUCLKPROFILE1_acpu_ddr_clk_div_prof_START  (4)
#define SOC_PMCTRL_ACPUCLKPROFILE1_acpu_ddr_clk_div_prof_END    (5)
#define SOC_PMCTRL_ACPUCLKPROFILE1_cpuext_clk_div_prof_START    (8)
#define SOC_PMCTRL_ACPUCLKPROFILE1_cpuext_clk_div_prof_END      (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUVOLPROFILE_UNION
 结构说明  : ACPUVOLPROFILE 寄存器结构定义。地址偏移量:0x124，初值:0x00009B1F，宽度:32
 寄存器说明: ACPU电压Profile寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_vol_idx_prof : 7;  /* bit[0-6]  : CPU电压Profile参数。 */
        unsigned int  reserved_0        : 1;  /* bit[7-7]  : 保留 */
        unsigned int  acpu_memrm_prof   : 8;  /* bit[8-15] : CPU MEM RM参数。 */
        unsigned int  reserved_1        : 4;  /* bit[16-19]: 保留 */
        unsigned int  acpu_tune_en_prof : 1;  /* bit[20-20]: CPU voltage detect电路使能参数。
                                                             1'b1:voltage detect功能中断上报使能；
                                                             1'b0:voltage detect功能中断上报去使能； */
        unsigned int  reserved_2        : 11; /* bit[21-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUVOLPROFILE_UNION;
#endif
#define SOC_PMCTRL_ACPUVOLPROFILE_acpu_vol_idx_prof_START  (0)
#define SOC_PMCTRL_ACPUVOLPROFILE_acpu_vol_idx_prof_END    (6)
#define SOC_PMCTRL_ACPUVOLPROFILE_acpu_memrm_prof_START    (8)
#define SOC_PMCTRL_ACPUVOLPROFILE_acpu_memrm_prof_END      (15)
#define SOC_PMCTRL_ACPUVOLPROFILE_acpu_tune_en_prof_START  (20)
#define SOC_PMCTRL_ACPUVOLPROFILE_acpu_tune_en_prof_END    (20)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUPLLSTAT_UNION
 结构说明  : ACPUPLLSTAT 寄存器结构定义。地址偏移量:0x128，初值:0x000061E5，宽度:32
 寄存器说明: ACPU PLL分频状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpupll_freq : 28; /* bit[0-27] : [27:18]：reserved
                                                        [17:12]：PLL后分频因子postdiv1。
                                                        [11:10]：reserved；
                                                        [9:4]：PLL FB programmable counter。
                                                        [3:0]：PLL FB swallow counter。 */
        unsigned int  acpupll_en   : 1;  /* bit[28-28]: PLL使能。 */
        unsigned int  reserved     : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUPLLSTAT_UNION;
#endif
#define SOC_PMCTRL_ACPUPLLSTAT_acpupll_freq_START  (0)
#define SOC_PMCTRL_ACPUPLLSTAT_acpupll_freq_END    (27)
#define SOC_PMCTRL_ACPUPLLSTAT_acpupll_en_START    (28)
#define SOC_PMCTRL_ACPUPLLSTAT_acpupll_en_END      (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUDIVTIMEOUT_UNION
 结构说明  : ACPUDIVTIMEOUT 寄存器结构定义。地址偏移量:0x12C，初值:0x00000028，宽度:32
 寄存器说明: ACPU时钟分频超时寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_clkdiv_time : 6;  /* bit[0-5] : ACPU时钟分频更新时间。 */
        unsigned int  reserved         : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUDIVTIMEOUT_UNION;
#endif
#define SOC_PMCTRL_ACPUDIVTIMEOUT_acpu_clkdiv_time_START  (0)
#define SOC_PMCTRL_ACPUDIVTIMEOUT_acpu_clkdiv_time_END    (5)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSONTIMEOUT_UNION
 结构说明  : ACPUAVSONTIMEOUT 寄存器结构定义。地址偏移量:0x130，初值:0x00000014，宽度:32
 寄存器说明: ACPU AVS再次启动超时寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  avs_on_wait_time : 32; /* bit[0-31]: DVFS结束到AVS启动的延迟时间。 */
    } reg;
} SOC_PMCTRL_ACPUAVSONTIMEOUT_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSONTIMEOUT_avs_on_wait_time_START  (0)
#define SOC_PMCTRL_ACPUAVSONTIMEOUT_avs_on_wait_time_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPARAUPD_UNION
 结构说明  : ACPUAVSPARAUPD 寄存器结构定义。地址偏移量:0x180，初值:0x00000000，宽度:32
 寄存器说明: ACPU AVS参数更新寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avspara_update : 1;  /* bit[0-0] : CPU AVS参数实时更新控制。
                                                              通常，CPU AVS参数在CPU DVFS运行之后更新；如果对这个寄存器写1，AVS参数立即更新；如果对这个寄存器写0，无效。 */
        unsigned int  reserved            : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUAVSPARAUPD_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPARAUPD_acpu_avspara_update_START  (0)
#define SOC_PMCTRL_ACPUAVSPARAUPD_acpu_avspara_update_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPARA0_UNION
 结构说明  : ACPUAVSPARA0 寄存器结构定义。地址偏移量:0x184，初值:0x00510002，宽度:32
 寄存器说明: ACPU AVS参数寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_hpm_clk_div : 6;  /* bit[0-5]  : ACPU HPM时钟分频器。
                                                            该寄存器不允许配置为6'h0。
                                                            配置为6'h1代表环形震荡器工作时间为2个时钟周期，配置为6'h2代表环形震荡器工作时间为3个时钟周期，配置为6'h3代表环形震荡器工作时间为4个时钟周期，依此类推。 */
        unsigned int  reserved_0       : 10; /* bit[6-15] : 保留 */
        unsigned int  acpu_avs_opc_mod : 2;  /* bit[16-17]: CPU AVS中同一个HPM OPC值的多次采样如何处理。
                                                            00：OPC取均值；
                                                            01：OPC取最小值；
                                                            10：OPC取最大值；
                                                            11：保留。 */
        unsigned int  reserved_1       : 2;  /* bit[18-19]: 保留 */
        unsigned int  acpu_avs_hpm_sel : 3;  /* bit[20-22]: CPU AVS中多个HPM的PC值如何处理。
                                                            000：取CPU0 HPM的PC值；
                                                            001：取CPU1 HPM的PC值；
                                                            010：取CPU2 HPM的PC值；
                                                            011：取CPU3 HPM的PC值；
                                                            100：取SCU HPM的PC值；
                                                            101：取HPM中的最小值；
                                                            110：取HPM中的最大值；
                                                            111：保留。 */
        unsigned int  reserved_2       : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUAVSPARA0_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPARA0_acpu_hpm_clk_div_START  (0)
#define SOC_PMCTRL_ACPUAVSPARA0_acpu_hpm_clk_div_END    (5)
#define SOC_PMCTRL_ACPUAVSPARA0_acpu_avs_opc_mod_START  (16)
#define SOC_PMCTRL_ACPUAVSPARA0_acpu_avs_opc_mod_END    (17)
#define SOC_PMCTRL_ACPUAVSPARA0_acpu_avs_hpm_sel_START  (20)
#define SOC_PMCTRL_ACPUAVSPARA0_acpu_avs_hpm_sel_END    (22)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPARA1_UNION
 结构说明  : ACPUAVSPARA1 寄存器结构定义。地址偏移量:0x188，初值:0x00000000，宽度:32
 寄存器说明: ACPU AVS参数寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_opc_offset : 10; /* bit[0-9]  : CPU AVS原始性能码偏移。 */
        unsigned int  reserved_0          : 2;  /* bit[10-11]: 保留 */
        unsigned int  acpu_avs_rcc        : 5;  /* bit[12-16]: CPU AVS参考校准码。 */
        unsigned int  reserved_1          : 3;  /* bit[17-19]: 保留 */
        unsigned int  acpu_avs_irgap      : 5;  /* bit[20-24]: CPU AVS IRDrop裕量。 */
        unsigned int  reserved_2          : 3;  /* bit[25-27]: 保留 */
        unsigned int  acpu_avs_opc_shift  : 3;  /* bit[28-30]: CPU AVS原始性能码移位。 */
        unsigned int  reserved_3          : 1;  /* bit[31-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUAVSPARA1_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPARA1_acpu_avs_opc_offset_START  (0)
#define SOC_PMCTRL_ACPUAVSPARA1_acpu_avs_opc_offset_END    (9)
#define SOC_PMCTRL_ACPUAVSPARA1_acpu_avs_rcc_START         (12)
#define SOC_PMCTRL_ACPUAVSPARA1_acpu_avs_rcc_END           (16)
#define SOC_PMCTRL_ACPUAVSPARA1_acpu_avs_irgap_START       (20)
#define SOC_PMCTRL_ACPUAVSPARA1_acpu_avs_irgap_END         (24)
#define SOC_PMCTRL_ACPUAVSPARA1_acpu_avs_opc_shift_START   (28)
#define SOC_PMCTRL_ACPUAVSPARA1_acpu_avs_opc_shift_END     (30)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPARA2_UNION
 结构说明  : ACPUAVSPARA2 寄存器结构定义。地址偏移量:0x18C，初值:0x87654321，宽度:32
 寄存器说明: ACPU AVS参数寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_vol_up_gain1 : 4;  /* bit[0-3]  : CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain2 : 4;  /* bit[4-7]  : CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain3 : 4;  /* bit[8-11] : CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain4 : 4;  /* bit[12-15]: CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain5 : 4;  /* bit[16-19]: CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain6 : 4;  /* bit[20-23]: CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain7 : 4;  /* bit[24-27]: CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain8 : 4;  /* bit[28-31]: CPU AVS升压增益。 */
    } reg;
} SOC_PMCTRL_ACPUAVSPARA2_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain1_START  (0)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain1_END    (3)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain2_START  (4)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain2_END    (7)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain3_START  (8)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain3_END    (11)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain4_START  (12)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain4_END    (15)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain5_START  (16)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain5_END    (19)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain6_START  (20)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain6_END    (23)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain7_START  (24)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain7_END    (27)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain8_START  (28)
#define SOC_PMCTRL_ACPUAVSPARA2_acpu_avs_vol_up_gain8_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPARA3_UNION
 结构说明  : ACPUAVSPARA3 寄存器结构定义。地址偏移量:0x190，初值:0x0FEDCBA9，宽度:32
 寄存器说明: ACPU AVS参数寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_vol_up_gain9  : 4;  /* bit[0-3]  : CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain10 : 4;  /* bit[4-7]  : CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain11 : 4;  /* bit[8-11] : CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain12 : 4;  /* bit[12-15]: CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain13 : 4;  /* bit[16-19]: CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain14 : 4;  /* bit[20-23]: CPU AVS升压增益。 */
        unsigned int  acpu_avs_vol_up_gain15 : 7;  /* bit[24-30]: CPU AVS升压增益。 */
        unsigned int  reserved               : 1;  /* bit[31-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUAVSPARA3_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain9_START   (0)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain9_END     (3)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain10_START  (4)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain10_END    (7)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain11_START  (8)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain11_END    (11)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain12_START  (12)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain12_END    (15)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain13_START  (16)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain13_END    (19)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain14_START  (20)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain14_END    (23)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain15_START  (24)
#define SOC_PMCTRL_ACPUAVSPARA3_acpu_avs_vol_up_gain15_END    (30)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPARA4_UNION
 结构说明  : ACPUAVSPARA4 寄存器结构定义。地址偏移量:0x194，初值:0x87654321，宽度:32
 寄存器说明: ACPU AVS参数寄存器4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_vol_dn_gain1 : 4;  /* bit[0-3]  : CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain2 : 4;  /* bit[4-7]  : CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain3 : 4;  /* bit[8-11] : CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain4 : 4;  /* bit[12-15]: CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain5 : 4;  /* bit[16-19]: CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain6 : 4;  /* bit[20-23]: CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain7 : 4;  /* bit[24-27]: CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain8 : 4;  /* bit[28-31]: CPU AVS降压增益。 */
    } reg;
} SOC_PMCTRL_ACPUAVSPARA4_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain1_START  (0)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain1_END    (3)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain2_START  (4)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain2_END    (7)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain3_START  (8)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain3_END    (11)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain4_START  (12)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain4_END    (15)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain5_START  (16)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain5_END    (19)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain6_START  (20)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain6_END    (23)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain7_START  (24)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain7_END    (27)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain8_START  (28)
#define SOC_PMCTRL_ACPUAVSPARA4_acpu_avs_vol_dn_gain8_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPARA5_UNION
 结构说明  : ACPUAVSPARA5 寄存器结构定义。地址偏移量:0x198，初值:0x0FEDCBA9，宽度:32
 寄存器说明: ACPU AVS参数寄存器5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_vol_dn_gain9  : 4;  /* bit[0-3]  : CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain10 : 4;  /* bit[4-7]  : CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain11 : 4;  /* bit[8-11] : CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain12 : 4;  /* bit[12-15]: CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain13 : 4;  /* bit[16-19]: CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain14 : 4;  /* bit[20-23]: CPU AVS降压增益。 */
        unsigned int  acpu_avs_vol_dn_gain15 : 7;  /* bit[24-30]: CPU AVS降压增益。 */
        unsigned int  reserved               : 1;  /* bit[31-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUAVSPARA5_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain9_START   (0)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain9_END     (3)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain10_START  (4)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain10_END    (7)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain11_START  (8)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain11_END    (11)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain12_START  (12)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain12_END    (15)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain13_START  (16)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain13_END    (19)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain14_START  (20)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain14_END    (23)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain15_START  (24)
#define SOC_PMCTRL_ACPUAVSPARA5_acpu_avs_vol_dn_gain15_END    (30)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPARA6_UNION
 结构说明  : ACPUAVSPARA6 寄存器结构定义。地址偏移量:0x19C，初值:0x0000007F，宽度:32
 寄存器说明: ACPU AVS参数寄存器6
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_max_vol : 7;  /* bit[0-6]  : CPU AVS最大电压限制。 */
        unsigned int  reserved_0       : 1;  /* bit[7-7]  : 保留 */
        unsigned int  acpu_avs_min_vol : 7;  /* bit[8-14] : CPU AVS最小电压限制。 */
        unsigned int  reserved_1       : 17; /* bit[15-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUAVSPARA6_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPARA6_acpu_avs_max_vol_START  (0)
#define SOC_PMCTRL_ACPUAVSPARA6_acpu_avs_max_vol_END    (6)
#define SOC_PMCTRL_ACPUAVSPARA6_acpu_avs_min_vol_START  (8)
#define SOC_PMCTRL_ACPUAVSPARA6_acpu_avs_min_vol_END    (14)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPARA7_UNION
 结构说明  : ACPUAVSPARA7 寄存器结构定义。地址偏移量:0x1A0，初值:0x090001FF，宽度:32
 寄存器说明: ACPU AVS参数寄存器7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_sample_num   : 22; /* bit[0-21] : CPU AVS性能码采样次数。当acpu_avs_opc_mod为0x0时该值配置为2的n次方减1。 */
        unsigned int  reserved_0            : 2;  /* bit[22-23]: 保留 */
        unsigned int  acpu_avs_sample_shift : 5;  /* bit[24-28]: ACPU AVS性能码累加值的左移位数，通常该值配置为n，n满足(2^n)-1=acpu_avs_sample_num。 */
        unsigned int  reserved_1            : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUAVSPARA7_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPARA7_acpu_avs_sample_num_START    (0)
#define SOC_PMCTRL_ACPUAVSPARA7_acpu_avs_sample_num_END      (21)
#define SOC_PMCTRL_ACPUAVSPARA7_acpu_avs_sample_shift_START  (24)
#define SOC_PMCTRL_ACPUAVSPARA7_acpu_avs_sample_shift_END    (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSEN_UNION
 结构说明  : ACPUAVSEN 寄存器结构定义。地址偏移量:0x1E4，初值:0x00000000，宽度:32
 寄存器说明: ACPU AVS使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_en        : 1;  /* bit[0-0] : CPU AVS使能。
                                                             0：CPU AVS功能关闭；
                                                             1：CPU AVS功能开启。 */
        unsigned int  acpu_avs_pwctrl_en : 1;  /* bit[1-1] : CPU AVS调压使能。
                                                             0：CPU AVS调压功能关闭；
                                                             1：CPU AVS调压功能开启。 */
        unsigned int  reserved           : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUAVSEN_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSEN_acpu_avs_en_START         (0)
#define SOC_PMCTRL_ACPUAVSEN_acpu_avs_en_END           (0)
#define SOC_PMCTRL_ACPUAVSEN_acpu_avs_pwctrl_en_START  (1)
#define SOC_PMCTRL_ACPUAVSEN_acpu_avs_pwctrl_en_END    (1)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUAVSPERIOD_UNION
 结构说明  : ACPUAVSPERIOD 寄存器结构定义。地址偏移量:0x1E8，初值:0x00020000，宽度:32
 寄存器说明: ACPU AVS周期寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_period : 32; /* bit[0-31]: ACPU AVS运行周期。 */
    } reg;
} SOC_PMCTRL_ACPUAVSPERIOD_UNION;
#endif
#define SOC_PMCTRL_ACPUAVSPERIOD_acpu_avs_period_START  (0)
#define SOC_PMCTRL_ACPUAVSPERIOD_acpu_avs_period_END    (31)


/*****************************************************************************
 结构名    : SOC_PMCTRL_AACPUAVSRUNROUND_UNION
 结构说明  : AACPUAVSRUNROUND 寄存器结构定义。地址偏移量:0x1EC，初值:0x00000000，宽度:32
 寄存器说明: ACPU AVS运行轮数寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_avs_round : 8;  /* bit[0-7] : CPU AVS运行轮数。
                                                         CPU AVS每运行一轮，则计数加1。
                                                         当CPU AVS电压OK中断有效时，计数维持不变，以便软件读取。
                                                         当CPU AVS电压OK中断清除时，计数清0，重新开始。 */
        unsigned int  reserved       : 24; /* bit[8-31]: 保留 */
    } reg;
} SOC_PMCTRL_AACPUAVSRUNROUND_UNION;
#endif
#define SOC_PMCTRL_AACPUAVSRUNROUND_acpu_avs_round_START  (0)
#define SOC_PMCTRL_AACPUAVSRUNROUND_acpu_avs_round_END    (7)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDPARAUDP_UNION
 结构说明  : PDPARAUDP 寄存器结构定义。地址偏移量:0x280，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测参数更新寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_avspara_update : 1;  /* bit[0-0] : 工艺检测参数实时更新控制。
                                                            如果对这个寄存器写1，工艺检测参数立即更新；如果对这个寄存器写0，无效。 */
        unsigned int  reserved          : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDPARAUDP_UNION;
#endif
#define SOC_PMCTRL_PDPARAUDP_pd_avspara_update_START  (0)
#define SOC_PMCTRL_PDPARAUDP_pd_avspara_update_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDPARA0_UNION
 结构说明  : PDPARA0 寄存器结构定义。地址偏移量:0x284，初值:0x00510002，宽度:32
 寄存器说明: 工艺检测参数寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm0_clk_div : 6;  /* bit[0-5]  : process detect功能的HPM0时钟分频器。
                                                           该寄存器不允许配置为6'h0。
                                                           配置为6'h1代表环形震荡器工作时间为2个时钟周期，配置为6'h2代表环形震荡器工作时间为3个时钟周期，配置为6'h3代表环形震荡器工作时间为4个时钟周期，依此类推。 */
        unsigned int  reserved_0      : 10; /* bit[6-15] : 保留 */
        unsigned int  pd_avs_opc_mod  : 2;  /* bit[16-17]: process detec中同一个HPM OPC值的多次采样如何处理。
                                                           00：OPC取均值；
                                                           01：OPC取最小值；
                                                           10：OPC取最大值；
                                                           11：保留。 */
        unsigned int  reserved_1      : 2;  /* bit[18-19]: 保留 */
        unsigned int  pd_avs_hpm_sel  : 3;  /* bit[20-22]: process detect中多个HPM的PC值如何处理。
                                                           000：取PD HPM0的PC值；
                                                           001：取PD HPM1的PC值；
                                                           010：取PD HPM2的PC值；
                                                           011：取PD HPM3的PC值；
                                                           100：保留；
                                                           101：取HPM中的最小值；
                                                           110：取HPM中的最大值；
                                                           111：保留。 */
        unsigned int  reserved_2      : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDPARA0_UNION;
#endif
#define SOC_PMCTRL_PDPARA0_pd_hpm0_clk_div_START  (0)
#define SOC_PMCTRL_PDPARA0_pd_hpm0_clk_div_END    (5)
#define SOC_PMCTRL_PDPARA0_pd_avs_opc_mod_START   (16)
#define SOC_PMCTRL_PDPARA0_pd_avs_opc_mod_END     (17)
#define SOC_PMCTRL_PDPARA0_pd_avs_hpm_sel_START   (20)
#define SOC_PMCTRL_PDPARA0_pd_avs_hpm_sel_END     (22)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDPARA1_UNION
 结构说明  : PDPARA1 寄存器结构定义。地址偏移量:0x288，初值:0x00020202，宽度:32
 寄存器说明: 工艺检测参数寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm3_clk_div : 6;  /* bit[0-5]  : process detect功能的HPM3时钟分频器。
                                                           该寄存器不允许配置为6'h0。
                                                           配置为6'h1代表环形震荡器工作时间为2个时钟周期，配置为6'h2代表环形震荡器工作时间为3个时钟周期，配置为6'h3代表环形震荡器工作时间为4个时钟周期，依此类推。 */
        unsigned int  reserved_0      : 2;  /* bit[6-7]  : 保留 */
        unsigned int  pd_hpm2_clk_div : 6;  /* bit[8-13] : process detect功能的HPM2 时钟分频器。
                                                           该寄存器不允许配置为6'h0。
                                                           配置为6'h1代表环形震荡器工作时间为2个时钟周期，配置为6'h2代表环形震荡器工作时间为3个时钟周期，配置为6'h3代表环形震荡器工作时间为4个时钟周期，依此类推。 */
        unsigned int  reserved_1      : 2;  /* bit[14-15]: 保留 */
        unsigned int  pd_hpm1_clk_div : 6;  /* bit[16-21]: process detect功能的HPM1时钟分频器。
                                                           该寄存器不允许配置为6'h0。
                                                           配置为6'h1代表环形震荡器工作时间为2个时钟周期，配置为6'h2代表环形震荡器工作时间为3个时钟周期，配置为6'h3代表环形震荡器工作时间为4个时钟周期，依此类推。 */
        unsigned int  reserved_2      : 10; /* bit[22-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDPARA1_UNION;
#endif
#define SOC_PMCTRL_PDPARA1_pd_hpm3_clk_div_START  (0)
#define SOC_PMCTRL_PDPARA1_pd_hpm3_clk_div_END    (5)
#define SOC_PMCTRL_PDPARA1_pd_hpm2_clk_div_START  (8)
#define SOC_PMCTRL_PDPARA1_pd_hpm2_clk_div_END    (13)
#define SOC_PMCTRL_PDPARA1_pd_hpm1_clk_div_START  (16)
#define SOC_PMCTRL_PDPARA1_pd_hpm1_clk_div_END    (21)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDAVSPARA2_UNION
 结构说明  : PDAVSPARA2 寄存器结构定义。地址偏移量:0x2A0，初值:0x090001FF，宽度:32
 寄存器说明: 工艺检测参数寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_avs_sample_num   : 22; /* bit[0-21] : 工艺检测用HPM性能码采样次数。当pd_avs_opc_mod为0x0时该值配置为2的n次方减1。 */
        unsigned int  reserved_0          : 2;  /* bit[22-23]: 保留 */
        unsigned int  pd_avs_sample_shift : 5;  /* bit[24-28]: 工艺检测用HPM码性能码累加值的左移位数，通常该值配置为n，n满足(2^n)-1=pd_avs_sample_num。 */
        unsigned int  reserved_1          : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDAVSPARA2_UNION;
#endif
#define SOC_PMCTRL_PDAVSPARA2_pd_avs_sample_num_START    (0)
#define SOC_PMCTRL_PDAVSPARA2_pd_avs_sample_num_END      (21)
#define SOC_PMCTRL_PDAVSPARA2_pd_avs_sample_shift_START  (24)
#define SOC_PMCTRL_PDAVSPARA2_pd_avs_sample_shift_END    (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDAVSEN_UNION
 结构说明  : PDAVSEN 寄存器结构定义。地址偏移量:0x2E0，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_avs_en   : 1;  /* bit[0-0] : 工艺检测使能。
                                                      1'b0：工艺检测功能关闭；
                                                      1'b1：工艺检测功能开启。 */
        unsigned int  pd_avs_trig : 1;  /* bit[1-1] : 
                                                      如果对这个寄存器写1，输出的工艺检测的性能码立刻清零；如果对这个寄存器写0，无效。在进行工艺检测功能使能前，也要对此bit写1。 */
        unsigned int  reserved    : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDAVSEN_UNION;
#endif
#define SOC_PMCTRL_PDAVSEN_pd_avs_en_START    (0)
#define SOC_PMCTRL_PDAVSEN_pd_avs_en_END      (0)
#define SOC_PMCTRL_PDAVSEN_pd_avs_trig_START  (1)
#define SOC_PMCTRL_PDAVSEN_pd_avs_trig_END    (1)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUHPMCTRL_UNION
 结构说明  : ACPUHPMCTRL 寄存器结构定义。地址偏移量:0x300，初值:0x00000000，宽度:32
 寄存器说明: ACPU HPM控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_hpm_en : 5;  /* bit[0-4] : ACPU子系统HPM使能。
                                                      1'b0：HPM不工作；
                                                      1'b1：HPM工作；
                                                      [4:0]依次代表ACPU cluster HPM、Core3 HPM、Core2 HPM、Core1 HPM、Core0 HPM */
        unsigned int  reserved    : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUHPMCTRL_UNION;
#endif
#define SOC_PMCTRL_ACPUHPMCTRL_acpu_hpm_en_START  (0)
#define SOC_PMCTRL_ACPUHPMCTRL_acpu_hpm_en_END    (4)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUHPMOPCVALID_UNION
 结构说明  : ACPUHPMOPCVALID 寄存器结构定义。地址偏移量:0x314，初值:0x00001F00，宽度:32
 寄存器说明: ACPU HPM OPC有效寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_hpm_opc_vld  : 5;  /* bit[0-4]  : ACPU HPM原始性能码有效的状态指示。
                                                             1'b0：HPM OPC无效；
                                                             1'b1：HPM OPC有效； */
        unsigned int  reserved_0        : 3;  /* bit[5-7]  : 保留 */
        unsigned int  acpu_hpmx_opc_vld : 5;  /* bit[8-12] : ACPU HPMX原始性能码有效的状态指示。
                                                             1'b0：HPMX OPC无效；
                                                             1'b1：HPMX OPC有效； */
        unsigned int  reserved_1        : 15; /* bit[13-27]: 保留 */
        unsigned int  acpu_hpm_discard  : 1;  /* bit[28-28]: 保留 */
        unsigned int  reserved_2        : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUHPMOPCVALID_UNION;
#endif
#define SOC_PMCTRL_ACPUHPMOPCVALID_acpu_hpm_opc_vld_START   (0)
#define SOC_PMCTRL_ACPUHPMOPCVALID_acpu_hpm_opc_vld_END     (4)
#define SOC_PMCTRL_ACPUHPMOPCVALID_acpu_hpmx_opc_vld_START  (8)
#define SOC_PMCTRL_ACPUHPMOPCVALID_acpu_hpmx_opc_vld_END    (12)
#define SOC_PMCTRL_ACPUHPMOPCVALID_acpu_hpm_discard_START   (28)
#define SOC_PMCTRL_ACPUHPMOPCVALID_acpu_hpm_discard_END     (28)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPU01HPMOPC_UNION
 结构说明  : ACPU01HPMOPC 寄存器结构定义。地址偏移量:0x318，初值:0x00000000，宽度:32
 寄存器说明: ACPU01 HPM OPC寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu0_hpm_opc        : 10; /* bit[0-9]  : CPU0 HPM原始性能码。 */
        unsigned int  acpu1_hpm_opc        : 10; /* bit[10-19]: CPU1 HPM原始性能码。 */
        unsigned int  acpu_cluster_hpm_opc : 10; /* bit[20-29]: ACPU cluster HPM原始性能码。 */
        unsigned int  reserved             : 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPU01HPMOPC_UNION;
#endif
#define SOC_PMCTRL_ACPU01HPMOPC_acpu0_hpm_opc_START         (0)
#define SOC_PMCTRL_ACPU01HPMOPC_acpu0_hpm_opc_END           (9)
#define SOC_PMCTRL_ACPU01HPMOPC_acpu1_hpm_opc_START         (10)
#define SOC_PMCTRL_ACPU01HPMOPC_acpu1_hpm_opc_END           (19)
#define SOC_PMCTRL_ACPU01HPMOPC_acpu_cluster_hpm_opc_START  (20)
#define SOC_PMCTRL_ACPU01HPMOPC_acpu_cluster_hpm_opc_END    (29)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPU23HPMOPC_UNION
 结构说明  : ACPU23HPMOPC 寄存器结构定义。地址偏移量:0x31C，初值:0x00000000，宽度:32
 寄存器说明: ACPU23 HPM OPC寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu2_hpm_opc : 10; /* bit[0-9]  : ACPU2 HPM原始性能码。 */
        unsigned int  acpu3_hpm_opc : 10; /* bit[10-19]: ACPU3 HPM原始性能码。 */
        unsigned int  reserved      : 12; /* bit[20-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPU23HPMOPC_UNION;
#endif
#define SOC_PMCTRL_ACPU23HPMOPC_acpu2_hpm_opc_START  (0)
#define SOC_PMCTRL_ACPU23HPMOPC_acpu2_hpm_opc_END    (9)
#define SOC_PMCTRL_ACPU23HPMOPC_acpu3_hpm_opc_START  (10)
#define SOC_PMCTRL_ACPU23HPMOPC_acpu3_hpm_opc_END    (19)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPU01HPMPC_UNION
 结构说明  : ACPU01HPMPC 寄存器结构定义。地址偏移量:0x330，初值:0x00000000，宽度:32
 寄存器说明: ACPU01 HPM PC寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu0_hpm_pc_avs        : 10; /* bit[0-9]  : CPU0 HPM性能码。 */
        unsigned int  acpu1_hpm_pc_avs        : 10; /* bit[10-19]: CPU1 HPM性能码。 */
        unsigned int  acpu_cluster_hpm_pc_avs : 10; /* bit[20-29]: ACPU cluster HPM性能码。 */
        unsigned int  reserved                : 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPU01HPMPC_UNION;
#endif
#define SOC_PMCTRL_ACPU01HPMPC_acpu0_hpm_pc_avs_START         (0)
#define SOC_PMCTRL_ACPU01HPMPC_acpu0_hpm_pc_avs_END           (9)
#define SOC_PMCTRL_ACPU01HPMPC_acpu1_hpm_pc_avs_START         (10)
#define SOC_PMCTRL_ACPU01HPMPC_acpu1_hpm_pc_avs_END           (19)
#define SOC_PMCTRL_ACPU01HPMPC_acpu_cluster_hpm_pc_avs_START  (20)
#define SOC_PMCTRL_ACPU01HPMPC_acpu_cluster_hpm_pc_avs_END    (29)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPU23HPMPC_UNION
 结构说明  : ACPU23HPMPC 寄存器结构定义。地址偏移量:0x334，初值:0x00000000，宽度:32
 寄存器说明: ACPU23 HPM PC寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu2_hpm_pc_avs : 10; /* bit[0-9]  : CPU2 HPM性能码。 */
        unsigned int  acpu3_hpm_pc_avs : 10; /* bit[10-19]: CPU3 HPM性能码。 */
        unsigned int  reserved         : 12; /* bit[20-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPU23HPMPC_UNION;
#endif
#define SOC_PMCTRL_ACPU23HPMPC_acpu2_hpm_pc_avs_START  (0)
#define SOC_PMCTRL_ACPU23HPMPC_acpu2_hpm_pc_avs_END    (9)
#define SOC_PMCTRL_ACPU23HPMPC_acpu3_hpm_pc_avs_START  (10)
#define SOC_PMCTRL_ACPU23HPMPC_acpu3_hpm_pc_avs_END    (19)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUHPMBYPASS_UNION
 结构说明  : ACPUHPMBYPASS 寄存器结构定义。地址偏移量:0x33C，初值:0x00000000，宽度:32
 寄存器说明: ACPU HPM旁路寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_hpmclk_bypass : 5;  /* bit[0-4] : ACPU子系统HPM时钟旁路寄存器。
                                                             [4:0]依次代表ACPU cluster HPM、Core3 HPM、Core2 HPM、Core1 HPM、Core0 HPM。
                                                             1'b0：硬件检测到对应HPM时钟关闭时，自动去使能AVS对该HPM的控制；
                                                             1'b1：无论对应HPM时钟关闭与否，AVS均控制该HPM */
        unsigned int  reserved           : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUHPMBYPASS_UNION;
#endif
#define SOC_PMCTRL_ACPUHPMBYPASS_acpu_hpmclk_bypass_START  (0)
#define SOC_PMCTRL_ACPUHPMBYPASS_acpu_hpmclk_bypass_END    (4)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUPMUVOLUPTIME_UNION
 结构说明  : ACPUPMUVOLUPTIME 寄存器结构定义。地址偏移量:0x360，初值:0x00000080，宽度:32
 寄存器说明: ACPU电压上升稳定时间寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_vol_up_time : 20; /* bit[0-19] : CPU电压上升稳定时间。 */
        unsigned int  reserved         : 12; /* bit[20-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUPMUVOLUPTIME_UNION;
#endif
#define SOC_PMCTRL_ACPUPMUVOLUPTIME_acpu_vol_up_time_START  (0)
#define SOC_PMCTRL_ACPUPMUVOLUPTIME_acpu_vol_up_time_END    (19)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUPMUVOLDNTIME_UNION
 结构说明  : ACPUPMUVOLDNTIME 寄存器结构定义。地址偏移量:0x364，初值:0x00000080，宽度:32
 寄存器说明: ACPU电压下降稳定时间寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_vol_dw_time : 20; /* bit[0-19] : CPU电压下降稳定时间。 */
        unsigned int  reserved         : 12; /* bit[20-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUPMUVOLDNTIME_UNION;
#endif
#define SOC_PMCTRL_ACPUPMUVOLDNTIME_acpu_vol_dw_time_START  (0)
#define SOC_PMCTRL_ACPUPMUVOLDNTIME_acpu_vol_dw_time_END    (19)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUVOLPMUADDR_UNION
 结构说明  : ACPUVOLPMUADDR 寄存器结构定义。地址偏移量:0x368，初值:0x0000006C，宽度:32
 寄存器说明: ACPU电压配置地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_vol_addr        : 9;  /* bit[0-8]  : ACPU电压配置寄存器地址。 */
        unsigned int  reserved_0           : 7;  /* bit[9-15] : 保留 */
        unsigned int  acpu_force_pmussi_en : 1;  /* bit[16-16]: 强制打开PMUSSI模块调压接口通道使能。
                                                                1'b0：不使能；
                                                                1'b1：使能。 */
        unsigned int  reserved_1           : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUVOLPMUADDR_UNION;
#endif
#define SOC_PMCTRL_ACPUVOLPMUADDR_acpu_vol_addr_START         (0)
#define SOC_PMCTRL_ACPUVOLPMUADDR_acpu_vol_addr_END           (8)
#define SOC_PMCTRL_ACPUVOLPMUADDR_acpu_force_pmussi_en_START  (16)
#define SOC_PMCTRL_ACPUVOLPMUADDR_acpu_force_pmussi_en_END    (16)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUVOLUPSTEP_UNION
 结构说明  : ACPUVOLUPSTEP 寄存器结构定义。地址偏移量:0x36C，初值:0x00000001，宽度:32
 寄存器说明: ACPU电压提升步进寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_vol_up_step : 7;  /* bit[0-6] : ACPU电压提升步进寄存器。
                                                           该寄存器不允许配置为0。 */
        unsigned int  reserved         : 25; /* bit[7-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUVOLUPSTEP_UNION;
#endif
#define SOC_PMCTRL_ACPUVOLUPSTEP_acpu_vol_up_step_START  (0)
#define SOC_PMCTRL_ACPUVOLUPSTEP_acpu_vol_up_step_END    (6)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUVOLDNSTEP_UNION
 结构说明  : ACPUVOLDNSTEP 寄存器结构定义。地址偏移量:0x370，初值:0x00000001，宽度:32
 寄存器说明: ACPU电压降低步进寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_vol_dn_step : 7;  /* bit[0-6] : ACPU电压降低步进寄存器。
                                                           该寄存器不允许配置为0。 */
        unsigned int  reserved         : 25; /* bit[7-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUVOLDNSTEP_UNION;
#endif
#define SOC_PMCTRL_ACPUVOLDNSTEP_acpu_vol_dn_step_START  (0)
#define SOC_PMCTRL_ACPUVOLDNSTEP_acpu_vol_dn_step_END    (6)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUDFTVOL_UNION
 结构说明  : ACPUDFTVOL 寄存器结构定义。地址偏移量:0x374，初值:0x0000001F，宽度:32
 寄存器说明: ACPU默认电压设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_dft_vol : 7;  /* bit[0-6] : ACPU默认电压设置寄存器。
                                                       该寄存器用于设置ACPU的默认电压，模块初次按步进调整电压时需要在该寄存器设置电压的基础上进行。 */
        unsigned int  reserved     : 25; /* bit[7-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUDFTVOL_UNION;
#endif
#define SOC_PMCTRL_ACPUDFTVOL_acpu_dft_vol_START  (0)
#define SOC_PMCTRL_ACPUDFTVOL_acpu_dft_vol_END    (6)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUDESTVOL_UNION
 结构说明  : ACPUDESTVOL 寄存器结构定义。地址偏移量:0x378，初值:0x00001F1F，宽度:32
 寄存器说明: ACPU目标电压设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_dest_vol  : 7;  /* bit[0-6]  : ACPU目标电压设置寄存器。
                                                          该寄存器用于设置ACPU的目标电压，设置完成后模块会控制PMU将电压调整到该寄存器对应值。 */
        unsigned int  reserved_0     : 1;  /* bit[7-7]  : 保留 */
        unsigned int  acpu_vol_using : 7;  /* bit[8-14] : ACPU正在使用的电压。 */
        unsigned int  reserved_1     : 17; /* bit[15-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUDESTVOL_UNION;
#endif
#define SOC_PMCTRL_ACPUDESTVOL_acpu_dest_vol_START   (0)
#define SOC_PMCTRL_ACPUDESTVOL_acpu_dest_vol_END     (6)
#define SOC_PMCTRL_ACPUDESTVOL_acpu_vol_using_START  (8)
#define SOC_PMCTRL_ACPUDESTVOL_acpu_vol_using_END    (14)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUVOLTIMEOUT_UNION
 结构说明  : ACPUVOLTIMEOUT 寄存器结构定义。地址偏移量:0x37C，初值:0x00000000，宽度:32
 寄存器说明: ACPU电压稳定指示寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_vol_timeout : 1;  /* bit[0-0] : ACPU电压稳定指示。
                                                           1'b0：电压未稳定；
                                                           1'b1：电压已稳定。 */
        unsigned int  reserved         : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUVOLTIMEOUT_UNION;
#endif
#define SOC_PMCTRL_ACPUVOLTIMEOUT_acpu_vol_timeout_START  (0)
#define SOC_PMCTRL_ACPUVOLTIMEOUT_acpu_vol_timeout_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMCTRL_UNION
 结构说明  : PDHPMCTRL 寄存器结构定义。地址偏移量:0x380，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测HPM控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm_en : 4;  /* bit[0-3] : 工艺检测用HPM使能。
                                                    1'b0：HPM不工作；
                                                    1'b1：HPM工作；
                                                    [3:0]依次代表HPM3、HPM2、HPM1、HPM0。 */
        unsigned int  reserved  : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMCTRL_UNION;
#endif
#define SOC_PMCTRL_PDHPMCTRL_pd_hpm_en_START  (0)
#define SOC_PMCTRL_PDHPMCTRL_pd_hpm_en_END    (3)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMOPCVALID_UNION
 结构说明  : PDHPMOPCVALID 寄存器结构定义。地址偏移量:0x394，初值:0x00000F00，宽度:32
 寄存器说明: 工艺检测HPM OPC有效寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm0_opc_vld  : 1;  /* bit[0-0]  : 工艺检测用的HPM0原始性能码有效的状态指示。
                                                            1'b0：HPM OPC无效；
                                                            1'b1：HPM OPC有效； */
        unsigned int  pd_hpm1_opc_vld  : 1;  /* bit[1-1]  : 工艺检测用的HPM1原始性能码有效的状态指示。
                                                            1'b0：HPM OPC无效；
                                                            1'b1：HPM OPC有效； */
        unsigned int  pd_hpm2_opc_vld  : 1;  /* bit[2-2]  : 工艺检测用的HPM2原始性能码有效的状态指示。
                                                            1'b0：HPM OPC无效；
                                                            1'b1：HPM OPC有效； */
        unsigned int  pd_hpm3_opc_vld  : 1;  /* bit[3-3]  : 工艺检测用的HPM3原始性能码有效的状态指示。
                                                            1'b0：HPM OPC无效；
                                                            1'b1：HPM OPC有效； */
        unsigned int  reserved_0       : 4;  /* bit[4-7]  : 保留 */
        unsigned int  pd_hpmx0_opc_vld : 1;  /* bit[8-8]  : 工艺检测用的HPMX0原始性能码有效的状态指示。 */
        unsigned int  pd_hpmx1_opc_vld : 1;  /* bit[9-9]  : 工艺检测用的HPMX1原始性能码有效的状态指示。 */
        unsigned int  pd_hpmx2_opc_vld : 1;  /* bit[10-10]: 工艺检测用的HPMX2原始性能码有效的状态指示。 */
        unsigned int  pd_hpmx3_opc_vld : 1;  /* bit[11-11]: 工艺检测用的HPMX3原始性能码有效的状态指示。 */
        unsigned int  reserved_1       : 20; /* bit[12-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMOPCVALID_UNION;
#endif
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpm0_opc_vld_START   (0)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpm0_opc_vld_END     (0)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpm1_opc_vld_START   (1)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpm1_opc_vld_END     (1)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpm2_opc_vld_START   (2)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpm2_opc_vld_END     (2)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpm3_opc_vld_START   (3)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpm3_opc_vld_END     (3)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpmx0_opc_vld_START  (8)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpmx0_opc_vld_END    (8)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpmx1_opc_vld_START  (9)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpmx1_opc_vld_END    (9)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpmx2_opc_vld_START  (10)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpmx2_opc_vld_END    (10)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpmx3_opc_vld_START  (11)
#define SOC_PMCTRL_PDHPMOPCVALID_pd_hpmx3_opc_vld_END    (11)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMOPC0_UNION
 结构说明  : PDHPMOPC0 寄存器结构定义。地址偏移量:0x398，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测HPM OPC寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm0_opc : 10; /* bit[0-9]  : 工艺检测 HPM0原始性能码。 */
        unsigned int  pd_hpm1_opc : 10; /* bit[10-19]: 工艺检测 HPM1原始性能码。 */
        unsigned int  reserved    : 12; /* bit[20-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMOPC0_UNION;
#endif
#define SOC_PMCTRL_PDHPMOPC0_pd_hpm0_opc_START  (0)
#define SOC_PMCTRL_PDHPMOPC0_pd_hpm0_opc_END    (9)
#define SOC_PMCTRL_PDHPMOPC0_pd_hpm1_opc_START  (10)
#define SOC_PMCTRL_PDHPMOPC0_pd_hpm1_opc_END    (19)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMOPC1_UNION
 结构说明  : PDHPMOPC1 寄存器结构定义。地址偏移量:0x39C，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测HPM OPC寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm2_opc : 10; /* bit[0-9]  : 工艺检测 HPM1原始性能码。 */
        unsigned int  pd_hpm3_opc : 10; /* bit[10-19]: 工艺检测 HPM2原始性能码。 */
        unsigned int  reserved    : 12; /* bit[20-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMOPC1_UNION;
#endif
#define SOC_PMCTRL_PDHPMOPC1_pd_hpm2_opc_START  (0)
#define SOC_PMCTRL_PDHPMOPC1_pd_hpm2_opc_END    (9)
#define SOC_PMCTRL_PDHPMOPC1_pd_hpm3_opc_START  (10)
#define SOC_PMCTRL_PDHPMOPC1_pd_hpm3_opc_END    (19)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMPC0_UNION
 结构说明  : PDHPMPC0 寄存器结构定义。地址偏移量:0x3A8，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测HPM PC寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm0_pc_avs : 10; /* bit[0-9]  : 工艺检测 HPM0性能码。 */
        unsigned int  pd_hpm1_pc_avs : 10; /* bit[10-19]: 工艺检测 HPM1性能码。 */
        unsigned int  reserved       : 12; /* bit[20-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMPC0_UNION;
#endif
#define SOC_PMCTRL_PDHPMPC0_pd_hpm0_pc_avs_START  (0)
#define SOC_PMCTRL_PDHPMPC0_pd_hpm0_pc_avs_END    (9)
#define SOC_PMCTRL_PDHPMPC0_pd_hpm1_pc_avs_START  (10)
#define SOC_PMCTRL_PDHPMPC0_pd_hpm1_pc_avs_END    (19)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMPC1_UNION
 结构说明  : PDHPMPC1 寄存器结构定义。地址偏移量:0x3AC，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测HPM PC寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm2_pc_avs : 10; /* bit[0-9]  : 工艺检测 HPM2性能码。 */
        unsigned int  pd_hpm3_pc_avs : 10; /* bit[10-19]: 工艺检测 HPM3性能码。 */
        unsigned int  reserved       : 12; /* bit[20-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMPC1_UNION;
#endif
#define SOC_PMCTRL_PDHPMPC1_pd_hpm2_pc_avs_START  (0)
#define SOC_PMCTRL_PDHPMPC1_pd_hpm2_pc_avs_END    (9)
#define SOC_PMCTRL_PDHPMPC1_pd_hpm3_pc_avs_START  (10)
#define SOC_PMCTRL_PDHPMPC1_pd_hpm3_pc_avs_END    (19)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMPC_UNION
 结构说明  : PDHPMPC 寄存器结构定义。地址偏移量:0x3B0，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测的性能码寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hpm_opc_cmp : 10; /* bit[0-9]  : 用于工艺检测的4组HPM的运算后的性能码。 */
        unsigned int  reserved    : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMPC_UNION;
#endif
#define SOC_PMCTRL_PDHPMPC_hpm_opc_cmp_START  (0)
#define SOC_PMCTRL_PDHPMPC_hpm_opc_cmp_END    (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMCINTRAW_UNION
 结构说明  : PMCINTRAW 寄存器结构定义。地址偏移量:0x480，初值:0x00000000，宽度:32
 寄存器说明: PMC中断原始状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_raw  : 10; /* bit[0-9]  : 原始中断寄存器，高有效。
                                                    [9]：DDR DFS完成中断；
                                                    [8]：DDR DFS配置DDRC参数中断；
                                                    [7]：工艺检测HPM输出PC中断；
                                                    [6]：ACPU VD模块参数配置中断；
                                                    [5]：ACPU DVFS完成中断；
                                                    [4]：ACPU AVS完成中断；
                                                    [3]：ACPU AVS输出PC中断；
                                                    [2]：ACPU AVS降低电压错误中断；
                                                    [1]：ACPU AVS提升电压错误中断；
                                                    [0]：ACPU AVS电压OK中断。 */
        unsigned int  reserved : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMCINTRAW_UNION;
#endif
#define SOC_PMCTRL_PMCINTRAW_int_raw_START   (0)
#define SOC_PMCTRL_PMCINTRAW_int_raw_END     (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMCINTCLR_UNION
 结构说明  : PMCINTCLR 寄存器结构定义。地址偏移量:0x484，初值:0x00000000，宽度:32
 寄存器说明: PMC中断清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_clr  : 10; /* bit[0-9]  : 中断清除，每1Bit对应PMCINTRAW寄存器的对应Bit。
                                                    写0：无效；
                                                    写1：清除中断。 */
        unsigned int  reserved : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMCINTCLR_UNION;
#endif
#define SOC_PMCTRL_PMCINTCLR_int_clr_START   (0)
#define SOC_PMCTRL_PMCINTCLR_int_clr_END     (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMCINTENMCU_UNION
 结构说明  : PMCINTENMCU 寄存器结构定义。地址偏移量:0x488，初值:0x00000000，宽度:32
 寄存器说明: PMC给MCU中断的掩码寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_mcu_en : 10; /* bit[0-9]  : 送给MCU中断的中断使能，每1Bit对应PMCINTRAW寄存器的对应Bit。
                                                      写0：中断屏蔽；
                                                      写1：中断使能。 */
        unsigned int  reserved   : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMCINTENMCU_UNION;
#endif
#define SOC_PMCTRL_PMCINTENMCU_int_mcu_en_START  (0)
#define SOC_PMCTRL_PMCINTENMCU_int_mcu_en_END    (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMCINTSTATMCU_UNION
 结构说明  : PMCINTSTATMCU 寄存器结构定义。地址偏移量:0x48C，初值:0x00000000，宽度:32
 寄存器说明: PMC给MCU中断的掩码后状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_mcu_stat : 10; /* bit[0-9]  : 送给MCU中断的屏蔽后中断状态，每1Bit对应PMCINTRAW寄存器的对应Bit。
                                                        0：中断无效；
                                                        1：中断有效。 */
        unsigned int  reserved     : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMCINTSTATMCU_UNION;
#endif
#define SOC_PMCTRL_PMCINTSTATMCU_int_mcu_stat_START  (0)
#define SOC_PMCTRL_PMCINTSTATMCU_int_mcu_stat_END    (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMCINTENACPU_UNION
 结构说明  : PMCINTENACPU 寄存器结构定义。地址偏移量:0x490，初值:0x00000000，宽度:32
 寄存器说明: PMC给ACPU中断的掩码寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_acpu_en : 10; /* bit[0-9]  : 送给ACPU中断的中断使能，每1Bit对应PMCINTRAW寄存器的对应Bit。
                                                       写0：中断屏蔽；
                                                       写1：中断使能。 */
        unsigned int  reserved    : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMCINTENACPU_UNION;
#endif
#define SOC_PMCTRL_PMCINTENACPU_int_acpu_en_START  (0)
#define SOC_PMCTRL_PMCINTENACPU_int_acpu_en_END    (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMCINTSTATACPU_UNION
 结构说明  : PMCINTSTATACPU 寄存器结构定义。地址偏移量:0x494，初值:0x00000000，宽度:32
 寄存器说明: PMC给ACPU中断的掩码后状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_acpu_stat : 10; /* bit[0-9]  : 送给ACPU中断的屏蔽后中断状态，每1Bit对应PMCINTRAW寄存器的对应Bit。
                                                         0：中断无效；
                                                         1：中断有效。 */
        unsigned int  reserved      : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMCINTSTATACPU_UNION;
#endif
#define SOC_PMCTRL_PMCINTSTATACPU_int_acpu_stat_START  (0)
#define SOC_PMCTRL_PMCINTSTATACPU_int_acpu_stat_END    (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMCINTENMCPU_UNION
 结构说明  : PMCINTENMCPU 寄存器结构定义。地址偏移量:0x498，初值:0x00000000，宽度:32
 寄存器说明: PMC给MCPU中断的掩码寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_mcpu_en : 10; /* bit[0-9]  : 送给MCPU中断的中断使能，每1Bit对应PMCINTRAW寄存器的对应Bit。
                                                       写0：中断屏蔽；
                                                       写1：中断使能。 */
        unsigned int  reserved    : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMCINTENMCPU_UNION;
#endif
#define SOC_PMCTRL_PMCINTENMCPU_int_mcpu_en_START  (0)
#define SOC_PMCTRL_PMCINTENMCPU_int_mcpu_en_END    (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMCINTSTATMCPU_UNION
 结构说明  : PMCINTSTATMCPU 寄存器结构定义。地址偏移量:0x49C，初值:0x00000000，宽度:32
 寄存器说明: PMC给MCPU中断的掩码后状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_mcpu_stat : 10; /* bit[0-9]  : 送给MCPU中断的屏蔽后中断状态，每1Bit对应PMCINTRAW寄存器的对应Bit。
                                                         0：中断无效；
                                                         1：中断有效。 */
        unsigned int  reserved      : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMCINTSTATMCPU_UNION;
#endif
#define SOC_PMCTRL_PMCINTSTATMCPU_int_mcpu_stat_START  (0)
#define SOC_PMCTRL_PMCINTSTATMCPU_int_mcpu_stat_END    (9)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PMCSTATUS_UNION
 结构说明  : PMCSTATUS 寄存器结构定义。地址偏移量:0x4A0，初值:0x00000000，宽度:32
 寄存器说明: PMC状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_dvfs_stat      : 4;  /* bit[0-3]  : ACPU DVFS状态机当前状态。 */
        unsigned int  reserved_0          : 4;  /* bit[4-7]  : 保留 */
        unsigned int  acpu_avs_stat       : 3;  /* bit[8-10] : ACPU AVS状态机当前状态。 */
        unsigned int  reserved_1          : 1;  /* bit[11-11]: 保留 */
        unsigned int  ddr_pllsw_stat      : 3;  /* bit[12-14]: DDR PLLSW状态机当前状态。 */
        unsigned int  reserved_2          : 1;  /* bit[15-15]: 保留 */
        unsigned int  process_detect_stat : 3;  /* bit[16-18]: 工艺检测状态机当前状态。 */
        unsigned int  reserved_3          : 9;  /* bit[19-27]: 保留 */
        unsigned int  ddr_dfs_stat        : 3;  /* bit[28-30]: DDR DFS状态机当前状态。 */
        unsigned int  reserved_4          : 1;  /* bit[31-31]: 保留 */
    } reg;
} SOC_PMCTRL_PMCSTATUS_UNION;
#endif
#define SOC_PMCTRL_PMCSTATUS_acpu_dvfs_stat_START       (0)
#define SOC_PMCTRL_PMCSTATUS_acpu_dvfs_stat_END         (3)
#define SOC_PMCTRL_PMCSTATUS_acpu_avs_stat_START        (8)
#define SOC_PMCTRL_PMCSTATUS_acpu_avs_stat_END          (10)
#define SOC_PMCTRL_PMCSTATUS_ddr_pllsw_stat_START       (12)
#define SOC_PMCTRL_PMCSTATUS_ddr_pllsw_stat_END         (14)
#define SOC_PMCTRL_PMCSTATUS_process_detect_stat_START  (16)
#define SOC_PMCTRL_PMCSTATUS_process_detect_stat_END    (18)
#define SOC_PMCTRL_PMCSTATUS_ddr_dfs_stat_START         (28)
#define SOC_PMCTRL_PMCSTATUS_ddr_dfs_stat_END           (30)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRCLKSEL_UNION
 结构说明  : DDRCLKSEL 寄存器结构定义。地址偏移量:0x580，初值:0x00000101，宽度:32
 寄存器说明: DDRC时钟选择寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddr_pllsw_cfg      : 2;  /* bit[0-1]  : 当前DDRC时钟选择哪个时钟的配置。
                                                              00：reserved；
                                                              01：DDRC时钟来源于SYS PLL；
                                                              10：DDRC时钟来源于DDR PLL0；
                                                              11：DDRC时钟来源于DDR PLL1。 */
        unsigned int  reserved_0         : 6;  /* bit[2-7]  : 保留。 */
        unsigned int  ddr_syspll_sw_ack  : 1;  /* bit[8-8]  : 当前DDRC时钟选择哪个时钟的状态指示。
                                                              1'b1:DDRC时钟来源于SYSPLL；
                                                              1'b0:DDRC时钟不是来源于SYSPLL： */
        unsigned int  ddr_ddrpll0_sw_ack : 1;  /* bit[9-9]  : 当前DDRC时钟选择哪个时钟的状态指示。
                                                              1'b1:DDRC时钟来源于DDRPLL0；
                                                              1'b0:DDRC时钟不是来源于DDRPLL0： */
        unsigned int  ddr_ddrpll1_sw_ack : 1;  /* bit[10-10]: 当前DDRC时钟选择哪个时钟的状态指示。
                                                              1'b1:DDRC时钟来源于DDRPLL1；
                                                              1'b0:DDRC时钟不是来源于DDRPLL1： */
        unsigned int  reserved_1         : 21; /* bit[11-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRCLKSEL_UNION;
#endif
#define SOC_PMCTRL_DDRCLKSEL_ddr_pllsw_cfg_START       (0)
#define SOC_PMCTRL_DDRCLKSEL_ddr_pllsw_cfg_END         (1)
#define SOC_PMCTRL_DDRCLKSEL_ddr_syspll_sw_ack_START   (8)
#define SOC_PMCTRL_DDRCLKSEL_ddr_syspll_sw_ack_END     (8)
#define SOC_PMCTRL_DDRCLKSEL_ddr_ddrpll0_sw_ack_START  (9)
#define SOC_PMCTRL_DDRCLKSEL_ddr_ddrpll0_sw_ack_END    (9)
#define SOC_PMCTRL_DDRCLKSEL_ddr_ddrpll1_sw_ack_START  (10)
#define SOC_PMCTRL_DDRCLKSEL_ddr_ddrpll1_sw_ack_END    (10)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRLPCTRL_UNION
 结构说明  : DDRLPCTRL 寄存器结构定义。地址偏移量:0x588，初值:0x00000031，宽度:32
 寄存器说明: DDRC LP控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrc_csysreq      : 1;  /* bit[0-0] : 请求DDR通道0进入低功耗模式。
                                                            0：请求DDR进入低功耗模式；
                                                            1：请求DDR退出低功耗模式。 */
        unsigned int  reserved_0        : 3;  /* bit[1-3] : 保留。 */
        unsigned int  ddrc_csysreq_stat : 1;  /* bit[4-4] : DDR的低功耗请求接口当前值。 */
        unsigned int  ddrc_csysack      : 1;  /* bit[5-5] : DDR的低功耗应答接口当前值。 */
        unsigned int  reserved_1        : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRLPCTRL_UNION;
#endif
#define SOC_PMCTRL_DDRLPCTRL_ddrc_csysreq_START       (0)
#define SOC_PMCTRL_DDRLPCTRL_ddrc_csysreq_END         (0)
#define SOC_PMCTRL_DDRLPCTRL_ddrc_csysreq_stat_START  (4)
#define SOC_PMCTRL_DDRLPCTRL_ddrc_csysreq_stat_END    (4)
#define SOC_PMCTRL_DDRLPCTRL_ddrc_csysack_START       (5)
#define SOC_PMCTRL_DDRLPCTRL_ddrc_csysack_END         (5)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRCCLKOFFCFG_UNION
 结构说明  : DDRCCLKOFFCFG 寄存器结构定义。地址偏移量:0x590，初值:0x00000000，宽度:32
 寄存器说明: DDRC DFS时钟模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll_on_idle : 1;  /* bit[0-0] : DDR PLL空闲时开关控制。
                                                         1'b0: DDR PLL不使用时主动关闭；
                                                         1'b1: DDR PLL不使用时仍然打开。 */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRCCLKOFFCFG_UNION;
#endif
#define SOC_PMCTRL_DDRCCLKOFFCFG_ddrpll_on_idle_START  (0)
#define SOC_PMCTRL_DDRCCLKOFFCFG_ddrpll_on_idle_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRCLKDIVSTAT_UNION
 结构说明  : DDRCLKDIVSTAT 寄存器结构定义。地址偏移量:0x594，初值:0x00000007，宽度:32
 寄存器说明: DDR DFS时钟分频状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll_clk_div_stat     : 4;  /* bit[0-3]  : DDRPLL时钟分频状态。 */
        unsigned int  ddrc_axi_clk_div_stat   : 2;  /* bit[4-5]  : DDRC AXI时钟分频状态。 */
        unsigned int  reserved_0              : 2;  /* bit[6-7]  : 保留。 */
        unsigned int  ddr_mmu_clk_div_stat    : 2;  /* bit[8-9]  : mmu的时钟分频状态。 */
        unsigned int  reserved_1              : 2;  /* bit[10-11]: 保留。 */
        unsigned int  ddr_harq_clk_div_stat   : 2;  /* bit[12-13]: harq的时钟分频状态。 */
        unsigned int  reserved_2              : 2;  /* bit[14-15]: 保留。 */
        unsigned int  ddrphy_ref_clk_div_stat : 2;  /* bit[16-17]: DDRPHY refclk的时钟分频状态。 */
        unsigned int  reserved_3              : 14; /* bit[18-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRCLKDIVSTAT_UNION;
#endif
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddrpll_clk_div_stat_START      (0)
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddrpll_clk_div_stat_END        (3)
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddrc_axi_clk_div_stat_START    (4)
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddrc_axi_clk_div_stat_END      (5)
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddr_mmu_clk_div_stat_START     (8)
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddr_mmu_clk_div_stat_END       (9)
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddr_harq_clk_div_stat_START    (12)
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddr_harq_clk_div_stat_END      (13)
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddrphy_ref_clk_div_stat_START  (16)
#define SOC_PMCTRL_DDRCLKDIVSTAT_ddrphy_ref_clk_div_stat_END    (17)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRPLLSWEN_UNION
 结构说明  : DDRPLLSWEN 寄存器结构定义。地址偏移量:0x598，初值:0x00000010，宽度:32
 寄存器说明: DDR PLL切换使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddr_pllsw_start : 1;  /* bit[0-0] : DDRC 频率切换启动使能，向该位写1'b1启动1次频率切换，写0无效。 */
        unsigned int  reserved_0      : 3;  /* bit[1-3] : 保留。 */
        unsigned int  ddr_fs_done     : 1;  /* bit[4-4] : DDRC 频率切换完成寄存器，ddr_pllsw_start写1后再去判断。
                                                          1'b0：DDR 频率切换未完成；
                                                          1'b1：DDR 频率切换未完成； */
        unsigned int  reserved_1      : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRPLLSWEN_UNION;
#endif
#define SOC_PMCTRL_DDRPLLSWEN_ddr_pllsw_start_START  (0)
#define SOC_PMCTRL_DDRPLLSWEN_ddr_pllsw_start_END    (0)
#define SOC_PMCTRL_DDRPLLSWEN_ddr_fs_done_START      (4)
#define SOC_PMCTRL_DDRPLLSWEN_ddr_fs_done_END        (4)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRDFSEN_UNION
 结构说明  : DDRDFSEN 寄存器结构定义。地址偏移量:0x59C，初值:0x00000000，宽度:32
 寄存器说明: DDR DFS使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddr_dfs_start : 1;  /* bit[0-0] : DDRC DFS启动使能，向该位写1'b1启动1次DFS，写0无效。 */
        unsigned int  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRDFSEN_UNION;
#endif
#define SOC_PMCTRL_DDRDFSEN_ddr_dfs_start_START  (0)
#define SOC_PMCTRL_DDRDFSEN_ddr_dfs_start_END    (0)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRDIVTIMEOUT_UNION
 结构说明  : DDRDIVTIMEOUT 寄存器结构定义。地址偏移量:0x5A4，初值:0x00000028，宽度:32
 寄存器说明: DDRPHY时钟分频超时寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddr_clkdiv_time : 6;  /* bit[0-5] : DDR时钟分频更新时间。计数周期为150Mhz。 */
        unsigned int  reserved        : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRDIVTIMEOUT_UNION;
#endif
#define SOC_PMCTRL_DDRDIVTIMEOUT_ddr_clkdiv_time_START  (0)
#define SOC_PMCTRL_DDRDIVTIMEOUT_ddr_clkdiv_time_END    (5)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRCLKDIVCFG_UNION
 结构说明  : DDRCLKDIVCFG 寄存器结构定义。地址偏移量:0x5A8，初值:0x00000007，宽度:32
 寄存器说明: DDR DFS时钟分频配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll_clk_div_cfg     : 4;  /* bit[0-3]  : DDRPLL时钟分频配置。 */
        unsigned int  ddrc_axi_clk_div_cfg   : 2;  /* bit[4-5]  : DDRC AXI时钟分频配置，CCPU/SYS NOC x2x桥分频比
                                                                  在ddrpll_clk_div后的时钟上分频。 */
        unsigned int  reserved_0             : 2;  /* bit[6-7]  : 保留。 */
        unsigned int  ddr_mmu_clk_div_cfg    : 2;  /* bit[8-9]  : mmu，rs2,rs3的时钟分频比配置。
                                                                  在ddrpll_clk_div后的时钟上分频。 */
        unsigned int  reserved_1             : 2;  /* bit[10-11]: 保留。 */
        unsigned int  ddr_harq_clk_div_cfg   : 2;  /* bit[12-13]: harq,ms1的时钟分频比配置。
                                                                  在rs2/rs3后的时钟上分频。 */
        unsigned int  reserved_2             : 2;  /* bit[14-15]: 保留。 */
        unsigned int  ddrphy_ref_clk_div_cfg : 2;  /* bit[16-17]: DDRPHY refclk的时钟分频配置。
                                                                  在ddrpll_clk_div后的时钟上分频。 */
        unsigned int  reserved_3             : 14; /* bit[18-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRCLKDIVCFG_UNION;
#endif
#define SOC_PMCTRL_DDRCLKDIVCFG_ddrpll_clk_div_cfg_START      (0)
#define SOC_PMCTRL_DDRCLKDIVCFG_ddrpll_clk_div_cfg_END        (3)
#define SOC_PMCTRL_DDRCLKDIVCFG_ddrc_axi_clk_div_cfg_START    (4)
#define SOC_PMCTRL_DDRCLKDIVCFG_ddrc_axi_clk_div_cfg_END      (5)
#define SOC_PMCTRL_DDRCLKDIVCFG_ddr_mmu_clk_div_cfg_START     (8)
#define SOC_PMCTRL_DDRCLKDIVCFG_ddr_mmu_clk_div_cfg_END       (9)
#define SOC_PMCTRL_DDRCLKDIVCFG_ddr_harq_clk_div_cfg_START    (12)
#define SOC_PMCTRL_DDRCLKDIVCFG_ddr_harq_clk_div_cfg_END      (13)
#define SOC_PMCTRL_DDRCLKDIVCFG_ddrphy_ref_clk_div_cfg_START  (16)
#define SOC_PMCTRL_DDRCLKDIVCFG_ddrphy_ref_clk_div_cfg_END    (17)


/*****************************************************************************
 结构名    : SOC_PMCTRL_DDRCLKPROFILE_UNION
 结构说明  : DDRCLKPROFILE 寄存器结构定义。地址偏移量:0x5AC，初值:0x00100007，宽度:32
 寄存器说明: DDR DFS时钟分频profile寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddrpll_clk_div_prof     : 4;  /* bit[0-3]  : DDR DFS时，DDRPLL时钟分频比配置。控制DDRC时钟频率 */
        unsigned int  ddrc_axi_clk_div_prof   : 2;  /* bit[4-5]  : DDR DFS时，CCPU/SYS NOC x2x桥分频比配置
                                                                   在ddrpll_clk_div后的时钟上分频。
                                                                   DDRC工作在533M的时候配置为2分频，其余频点1分频 */
        unsigned int  reserved_0              : 2;  /* bit[6-7]  : 保留。 */
        unsigned int  ddr_mmu_clk_div_prof    : 2;  /* bit[8-9]  : DDR DFS时，mmu，rs2,rs3的时钟分频比配置。
                                                                   在ddrpll_clk_div后的时钟上分频。
                                                                   DDRC工作在533M的时候配置为2分频，其余频点1分频 */
        unsigned int  reserved_1              : 2;  /* bit[10-11]: 保留。 */
        unsigned int  ddr_harq_clk_div_prof   : 2;  /* bit[12-13]: DDR DFS时，harq,ms1的时钟分频比配置。
                                                                   在rs2/rs3后的时钟上分频。
                                                                   Rs2/rs3高于200M的时候，配置为2分频，其它1分频。 */
        unsigned int  reserved_2              : 2;  /* bit[14-15]: 保留。 */
        unsigned int  ddrphy_ref_clk_div_prof : 2;  /* bit[16-17]: DDR DFS时，DDRPHY refclk的时钟分频配置。
                                                                   在ddrpll_clk_div后的时钟上分频。 */
        unsigned int  reserved_3              : 2;  /* bit[18-19]: 保留。 */
        unsigned int  ddr_pllsw_prof          : 2;  /* bit[20-21]: 当前DDRC时钟选择哪个时钟的profile。
                                                                   00：reserved；
                                                                   01：DDRC时钟来源于SYS PLL；
                                                                   10：DDRC时钟来源于DDR PLL0；
                                                                   11：DDRC时钟来源于DDR PLL1。 */
        unsigned int  reserved_4              : 10; /* bit[22-31]: 保留 */
    } reg;
} SOC_PMCTRL_DDRCLKPROFILE_UNION;
#endif
#define SOC_PMCTRL_DDRCLKPROFILE_ddrpll_clk_div_prof_START      (0)
#define SOC_PMCTRL_DDRCLKPROFILE_ddrpll_clk_div_prof_END        (3)
#define SOC_PMCTRL_DDRCLKPROFILE_ddrc_axi_clk_div_prof_START    (4)
#define SOC_PMCTRL_DDRCLKPROFILE_ddrc_axi_clk_div_prof_END      (5)
#define SOC_PMCTRL_DDRCLKPROFILE_ddr_mmu_clk_div_prof_START     (8)
#define SOC_PMCTRL_DDRCLKPROFILE_ddr_mmu_clk_div_prof_END       (9)
#define SOC_PMCTRL_DDRCLKPROFILE_ddr_harq_clk_div_prof_START    (12)
#define SOC_PMCTRL_DDRCLKPROFILE_ddr_harq_clk_div_prof_END      (13)
#define SOC_PMCTRL_DDRCLKPROFILE_ddrphy_ref_clk_div_prof_START  (16)
#define SOC_PMCTRL_DDRCLKPROFILE_ddrphy_ref_clk_div_prof_END    (17)
#define SOC_PMCTRL_DDRCLKPROFILE_ddr_pllsw_prof_START           (20)
#define SOC_PMCTRL_DDRCLKPROFILE_ddr_pllsw_prof_END             (21)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUHPMMASKEN_UNION
 结构说明  : ACPUHPMMASKEN 寄存器结构定义。地址偏移量:0xC00，初值:0x00000000，宽度:32
 寄存器说明: ACPU HPM屏蔽使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_hpm_mask_en : 5;  /* bit[0-4] : AVS控制屏蔽HPM使能，写1有效，写0无效。
                                                           [4:0]分别代表acpu cluster hpm、core3 hpm、core2 hpm、core1 hpm、core0 hpm。 */
        unsigned int  reserved         : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUHPMMASKEN_UNION;
#endif
#define SOC_PMCTRL_ACPUHPMMASKEN_acpu_hpm_mask_en_START  (0)
#define SOC_PMCTRL_ACPUHPMMASKEN_acpu_hpm_mask_en_END    (4)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUHPMMASKDIS_UNION
 结构说明  : ACPUHPMMASKDIS 寄存器结构定义。地址偏移量:0xC04，初值:0x00000000，宽度:32
 寄存器说明: ACPU HPM屏蔽去使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_hpm_mask_dis : 5;  /* bit[0-4] : AVS控制屏蔽HPM去使能，写1有效，写0无效。
                                                            [4:0]分别代表acpu cluster hpm、core3 hpm、core2 hpm、core1 hpm、core0 hpm。 */
        unsigned int  reserved          : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUHPMMASKDIS_UNION;
#endif
#define SOC_PMCTRL_ACPUHPMMASKDIS_acpu_hpm_mask_dis_START  (0)
#define SOC_PMCTRL_ACPUHPMMASKDIS_acpu_hpm_mask_dis_END    (4)


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUHPMMASKSTAT_UNION
 结构说明  : ACPUHPMMASKSTAT 寄存器结构定义。地址偏移量:0xC08，初值:0x0000001F，宽度:32
 寄存器说明: ACPU HPM屏蔽状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acpu_hpm_mask : 5;  /* bit[0-4] : AVS控制屏蔽HPM状态。
                                                        [4:0]分别代表acpu cluster hpm、core3 hpm、core2 hpm、core1 hpm、core0 hpm。 */
        unsigned int  reserved      : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUHPMMASKSTAT_UNION;
#endif
#define SOC_PMCTRL_ACPUHPMMASKSTAT_acpu_hpm_mask_START  (0)
#define SOC_PMCTRL_ACPUHPMMASKSTAT_acpu_hpm_mask_END    (4)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMMASKEN_UNION
 结构说明  : PDHPMMASKEN 寄存器结构定义。地址偏移量:0xC10，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测 HPM屏蔽使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm_mask_en : 4;  /* bit[0-3] : 工艺检测控制屏蔽HPM使能。
                                                         [3:0]依次代表HPM3、HPM2、HPM1、HPM0。 */
        unsigned int  reserved       : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMMASKEN_UNION;
#endif
#define SOC_PMCTRL_PDHPMMASKEN_pd_hpm_mask_en_START  (0)
#define SOC_PMCTRL_PDHPMMASKEN_pd_hpm_mask_en_END    (3)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMMASKDIS_UNION
 结构说明  : PDHPMMASKDIS 寄存器结构定义。地址偏移量:0xC14，初值:0x00000000，宽度:32
 寄存器说明: 工艺检测 HPM屏蔽去使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm_mask_dis : 4;  /* bit[0-3] : 工艺检测控制屏蔽HPM去使能。
                                                          [3:0]依次代表HPM3、HPM2、HPM1、HPM0。 */
        unsigned int  reserved        : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMMASKDIS_UNION;
#endif
#define SOC_PMCTRL_PDHPMMASKDIS_pd_hpm_mask_dis_START  (0)
#define SOC_PMCTRL_PDHPMMASKDIS_pd_hpm_mask_dis_END    (3)


/*****************************************************************************
 结构名    : SOC_PMCTRL_PDHPMMASKSTAT_UNION
 结构说明  : PDHPMMASKSTAT 寄存器结构定义。地址偏移量:0xC18，初值:0x0000000F，宽度:32
 寄存器说明: 工艺检测 HPM屏蔽状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pd_hpm_mask : 4;  /* bit[0-3] : 工艺检测控制屏蔽HPM使能状态。
                                                      1'b1:屏蔽HPM；
                                                      1'b0:使能HPM；
                                                      [3:0]依次代表HPM3、HPM2、HPM1、HPM0。 */
        unsigned int  reserved    : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_PMCTRL_PDHPMMASKSTAT_UNION;
#endif
#define SOC_PMCTRL_PDHPMMASKSTAT_pd_hpm_mask_START  (0)
#define SOC_PMCTRL_PDHPMMASKSTAT_pd_hpm_mask_END    (3)


/*****************************************************************************
 结构名    : SOC_PMCTRL_MEDPLLRESERVED1_UNION
 结构说明  : MEDPLLRESERVED1 寄存器结构定义。地址偏移量:0x904，初值:0x00000800，宽度:32
 寄存器说明: MEDPLL保留寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_PMCTRL_MEDPLLRESERVED1_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PMCTRL_ACPUPLLRESERVED1_UNION
 结构说明  : ACPUPLLRESERVED1 寄存器结构定义。地址偏移量:0x908，初值:0x00000800，宽度:32
 寄存器说明: ACPUPLL保留寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_PMCTRL_ACPUPLLRESERVED1_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PMCTRL_GPUPLLRESERVED1_UNION
 结构说明  : GPUPLLRESERVED1 寄存器结构定义。地址偏移量:0x90C，初值:0x00000800，宽度:32
 寄存器说明: GPUPLL保留寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_PMCTRL_GPUPLLRESERVED1_UNION;
#endif






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

#endif /* end of soc_pmctrl_interface.h */
