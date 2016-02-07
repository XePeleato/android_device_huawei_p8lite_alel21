

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_MDDRC_PACK_INTERFACE_H__
#define __SOC_MDDRC_PACK_INTERFACE_H__

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
/* 寄存器说明：This register shows current version of the PHY Control Block
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_REVISION_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_REVISION_ADDR(base)  ((base) + (0x000))

/* 寄存器说明：This register control the initialization of the PHY. Please note that write to this register will be ignored if init_en is already at '1' state.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_ADDR(base) ((base) + (0x004))

/* 寄存器说明：This register shows the PHY status.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_ADDR(base) ((base) + (0x008))

/* 寄存器说明：This register control the clock gated of PHY
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_ADDR(base) ((base) + (0x000C))

/* 寄存器说明：This register specified the timing parameter required by the PHY.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER0_ADDR(base) ((base) + (0x010))

/* 寄存器说明：
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER1_ADDR(base) ((base) + (0x014))

/* 寄存器说明：This register specified the timing paramters for PLL in both address /command, and data block.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PLLCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PLLCTRL_ADDR(base)   ((base) + (0x018))

/* 寄存器说明：This register specified the control for PLL in both address /command, and data block.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PLLTIMER_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PLLTIMER_ADDR(base)  ((base) + (0x01C))

/* 寄存器说明：This register specify the control for delay measurement of the read delay line.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_ADDR(base) ((base) + (0x020))

/* 寄存器说明：This register specify the control the ZQ calibration.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_ADDR(base)   ((base) + (0x024))

/* 寄存器说明：This register specify the ZQ calibration result.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_ADDR(base) ((base) + (0x028))

/* 寄存器说明：This register is used to configure the DRAM system.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_ADDR(base)   ((base) + (0x02C))

/* 寄存器说明：This register specify the DRAM timing parameters. Please note that the command delay specified in this register only apply to command issued by the PHY controller internally.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_ADDR(base) ((base) + (0x030))

/* 寄存器说明：
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_ADDR(base) ((base) + (0x034))

/* 寄存器说明：
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_ADDR(base) ((base) + (0x038))

/* 寄存器说明：
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_ADDR(base) ((base) + (0x03C))

/* 寄存器说明：This register specify the DRAM Write Leveling timing parameter. This register only applied to DDR3/LPDDR3 type SDRAM.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_ADDR(base) ((base) + (0x040))

/* 寄存器说明：This register specify the ODT control on different rank while reading or writing to particular rank.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ODTCR_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ODTCR_ADDR(base)     ((base) + (0x044))

/* 寄存器说明：This register specify the training control.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_ADDR(base) ((base) + (0x048))

/* 寄存器说明：This register specify the training rank control.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_RANKEN_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_RANKEN_ADDR(base)    ((base) + (0x04C))

/* 寄存器说明：This register specify the starting memory address where can be used to do data training. The minimum space required for data training should be 64-bytes, and the allocated memory should be 64-bytes alignment.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_ADDR(base) ((base) + (0x0050))

/* 寄存器说明：This register specify the behavior of the built-in self test.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_ADDR(base)  ((base) + (0x0054))

/* 寄存器说明：This register specify the BIST data used for DRAM loopback test. Please note that this register only valid while the bist_pat is set to &quot;User Defined&quot;
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_ADDR(base) ((base) + (0x0058))

/* 寄存器说明：
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_ADDR(base) ((base) + (0x005C))

/* 寄存器说明：This register shows the result and status of BIST testing.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_ADDR(base) ((base) + (0x0060))

/* 寄存器说明：This register defines the contents of the Mode Register.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_MODEREG01_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG01_ADDR(base) ((base) + (0x0064))

/* 寄存器说明：This register defines the contents of the Mode Register.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_MODEREG23_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG23_ADDR(base) ((base) + (0x0068))

/* 寄存器说明：Write/Read DET Pattern Register.
            These fields are used to fine-tune the training procedure. Users are not recommend to modify them.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_ADDR(base) ((base) + (0x006C))

/* 寄存器说明：Miscellaneous control register
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_MISC_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_ADDR(base)      ((base) + (0x0070))

/* 寄存器说明：Rank to rank delay control register
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_ADDR(base)   ((base) + (0x0074))

/* 寄存器说明：PHY control registers 
            Register in this field are connected to PHY interface directly
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_ADDR(base)  ((base) + (0x0078))

/* 寄存器说明：PHY debug registers
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYDBG_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYDBG_ADDR(base)    ((base) + (0x007C))

/* 寄存器说明：ReTrain Control
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_ADDR(base)  ((base) + (0x0080))

/* 寄存器说明：DM Swap Selection
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DMSEL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DMSEL_ADDR(base)     ((base) + (0x0084))

/* 寄存器说明：IO control register
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_IOCTL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ADDR(base)     ((base) + (0x0088))

/* 寄存器说明：Swap of DQ in PHY
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DQSSEL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DQSSEL_ADDR(base)    ((base) + (0x008C))

/* 寄存器说明：AC PHY control registers 
            Register in this field are connected to PHY interface directly
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ADDR(base)  ((base) + (0x0090))

/* 寄存器说明：This register is for PLL phase select within the PHY. And should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNCKCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCKCTRL_ADDR(base) ((base) + (0x0094))

/* 寄存器说明：PHY PLL control registers 
            Register in this field are connected to PHY interface directly
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_AC_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_AC_ADDR(base) ((base) + (0x0098))

/* 寄存器说明：PHY PLL control registers 
            Register in this field are connected to PHY interface directly
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_DX_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_DX_ADDR(base) ((base) + (0x009C))

/* 寄存器说明：This register is for setting S/W training mode
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_ADDR(base)   ((base) + (0x00A0))

/* 寄存器说明：This register is for issuing write DQS in S/W write leveling training
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_SWTWLDQS_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_SWTWLDQS_ADDR(base)  ((base) + (0x00A4))

/* 寄存器说明：S/W training result
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_ADDR(base)    ((base) + (0x00A8))

/* 寄存器说明：PHY control registers 
            Register in this field are connected to PHY interface directly
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_ADDR(base)  ((base) + (0x00AC))

/* 寄存器说明：PHY control registers 
            Register in this field are connected to PHY interface directly
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_ADDR(base)    ((base) + (0x00B0))

/* 寄存器说明：PHY control registers 
            Register in this field are connected to PHY interface directly
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ADDR(base)    ((base) + (0x00B4))

/* 寄存器说明：This register control the period of code update during BIST running.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_BISTTIMER_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_BISTTIMER_ADDR(base) ((base) + (0x00B8))

/* 寄存器说明：This register control the data training.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_ADDR(base) ((base) + (0x00D0))

/* 寄存器说明：This register control the data training.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_ADDR(base) ((base) + (0x00D4))

/* 寄存器说明：This register control the register bank.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_REGBANKCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_REGBANKCTRL_ADDR(base) ((base) + (0x00D8))

/* 寄存器说明：This register control the data training.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_ADDR(base) ((base) + (0x00DC))

/* 寄存器说明：This register control the comparison while BIST activated.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_ADDR(base) ((base) + (0x100))

/* 寄存器说明：This register control the comparison while BIST activated.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_ADDR(base) ((base) + (0x104))

/* 寄存器说明：This register shows the results and status of the BIST test. Please note that this register is read-only, and can only be reset by bist_op setting to BIST Reset.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_ADDR(base) ((base) + (0x108))

/* 寄存器说明：This register shows the results and status of the BIST test. Please note that this register is read-only, and can only be reset by bist_op setting to BIST Reset.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS1_ADDR(base) ((base) + (0x10C))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL0_ADDR(base) ((base) + (0x120))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL1_ADDR(base) ((base) + (0x124))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL2_ADDR(base) ((base) + (0x128))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL3_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL3_ADDR(base) ((base) + (0x12C))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL4_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL4_ADDR(base) ((base) + (0x130))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL5_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL5_ADDR(base) ((base) + (0x134))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL6_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL6_ADDR(base) ((base) + (0x138))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL7_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL7_ADDR(base) ((base) + (0x13C))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL0_ADDR(base) ((base) + (0x140))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL1_ADDR(base) ((base) + (0x144))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL2_ADDR(base) ((base) + (0x148))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL3_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL3_ADDR(base) ((base) + (0x14C))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL4_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL4_ADDR(base) ((base) + (0x150))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL5_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL5_ADDR(base) ((base) + (0x154))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL6_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL6_ADDR(base) ((base) + (0x158))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL7_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL7_ADDR(base) ((base) + (0x15C))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL8_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL8_ADDR(base) ((base) + (0x160))

/* 寄存器说明：AC address bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL9_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL9_ADDR(base) ((base) + (0x164))

/* 寄存器说明：AC clock bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_ADDR(base)  ((base) + (0x168))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_ADDR(base) ((base) + (0x170))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_ADDR(base) ((base) + (0x174))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_ADDR(base) ((base) + (0x178))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_ADDR(base) ((base) + (0x17C))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_ADDR(base) ((base) + (0x180))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_ADDR(base) ((base) + (0x184))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_ADDR(base) ((base) + (0x188))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ADDR(base) ((base) + (0x018C))

/* 寄存器说明：This register is for PHY debug only.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_ADDR(base)   ((base) + (0x0190))

/* 寄存器说明：AC block PHY reserved control pins. This register is for PHY control and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDC_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDC_ADDR(base) ((base) + (0x0194))

/* 寄存器说明：AC block PHY reserved control pins. This register is for PHY control and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDS_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDS_ADDR(base) ((base) + (0x0198))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ADDR(base) ((base) + (0x019C))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL8_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL8_ADDR(base) ((base) + (0x1A0))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL9_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL9_ADDR(base) ((base) + (0x1A4))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL10_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL10_ADDR(base) ((base) + (0x1A8))

/* 寄存器说明：AC command bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL11_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL11_ADDR(base) ((base) + (0x1AC))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_ADDR(base)  ((base) + (0x01B0))

/* 寄存器说明：AC block PHY control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_ADDR(base) ((base) + (0x01B4))

/* 寄存器说明：CA Training Timer0
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_ADDR(base) ((base) + (0x01C0))

/* 寄存器说明：CA Training Timer1
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_CATTIMER1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER1_ADDR(base) ((base) + (0x01C4))

/* 寄存器说明：CA Training Configuration
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_ADDR(base) ((base) + (0x01C8))

/* 寄存器说明：CA Training result for debug
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_CATRESULT_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_CATRESULT_ADDR(base) ((base) + (0x01CC))

/* 寄存器说明：SW CA Training DQ result from PHY
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_PHYDQRESULT_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_PHYDQRESULT_ADDR(base) ((base) + (0x01D0))

/* 寄存器说明：CA Training addr phase boundary
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_ADDR(base) ((base) + (0x01D4))

/* 寄存器说明：SW CA Training pattern for  positive CK edge
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_P_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_P_ADDR(base) ((base) + (0x01D8))

/* 寄存器说明：SW CA Training pattern for  negative CK edge
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_N_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_N_ADDR(base) ((base) + (0x01DC))

/* 寄存器说明：Low Power Control Register
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_ADDR(base)    ((base) + (0x01E4))

/* 寄存器说明：This register is used to control if the loopback data is compre during BIST of Data Blocks.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_ADDR(base, n)  ((base) + (0x200+(n)*0x80))

/* 寄存器说明：This register shows the results and status of the BIST test. Please note that this register is read-only, and can only be reset by bist_op setting to BIST Reset.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_ADDR(base, n)  ((base) + (0x204+(n)*0x80))

/* 寄存器说明：This register is used to control Data Block
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCTRL_ADDR(base, n)  ((base) + (0x208+(n)*0x80))

/* 寄存器说明：This register is used to control the bit delay line of the DATA block.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_ADDR(base, n)  ((base) + (0x210+(n)*0x80))

/* 寄存器说明：This register is used to control the bit delay line of the DATA block.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_ADDR(base, n)  ((base) + (0x214+(n)*0x80))

/* 寄存器说明：This register is used to control the bit delay line of the DATA block.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL2_ADDR(base, n)  ((base) + (0x218+(n)*0x80))

/* 寄存器说明：This register is used to control the bit delay line of the DATA block.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_ADDR(base, n)  ((base) + (0x21C+(n)*0x80))

/* 寄存器说明：This register is used to control the bit delay line of the DATA block.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_ADDR(base, n)  ((base) + (0x220+(n)*0x80))

/* 寄存器说明：This register is used to control the bit delay line of the DATA block.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL2_ADDR(base, n)  ((base) + (0x224+(n)*0x80))

/* 寄存器说明：Output Enable Delay Line Control. This register specify the dely line value of the output enable delay line within PHY data block.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNOEBDL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNOEBDL_ADDR(base, n)  ((base) + (0x228+(n)*0x80))

/* 寄存器说明：This register is used to control the local delay line.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNRDQSDLY_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQSDLY_ADDR(base, n)  ((base) + (0x22C+(n)*0x80))

/* 寄存器说明：This register is used to control the write leveling DQS delays.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNWDQSDLY_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQSDLY_ADDR(base, n)  ((base) + (0x230+(n)*0x80))

/* 寄存器说明：This register is used to control the write leveling delay line.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNWDQDLY_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQDLY_ADDR(base, n)  ((base) + (0x234+(n)*0x80))

/* 寄存器说明：This reister is used to control if PHY controller add extra system latency.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNWLSL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWLSL_ADDR(base, n)  ((base) + (0x238+(n)*0x80))

/* 寄存器说明：This register is used to control the latch enable within the PHY to get the stable data.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNGDS_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNGDS_ADDR(base, n)  ((base) + (0x23C+(n)*0x80))

/* 寄存器说明：This register is used to control the local delay line.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_ADDR(base, n)  ((base) + (0x240+(n)*0x80))

/* 寄存器说明：This register is used to control the local delay line.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_ADDR(base, n)  ((base) + (0x640+(n)*0x80))

/* 寄存器说明：DATA block clock bit delay line setting
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNCLKBDL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCLKBDL_ADDR(base, n)  ((base) + (0x24C+(n)*0x80))

/* 寄存器说明：Read data eye boundary. This register indicates the left/right boundary of RDQSQDL of the data eye.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNRDBOUND_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDBOUND_ADDR(base, n)  ((base) + (0x250+(n)*0x80))

/* 寄存器说明：Write data eye boundary. This register indicates the left/right boundary of WDQSQDL of the data eye.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNWRBOUND_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWRBOUND_ADDR(base, n)  ((base) + (0x254+(n)*0x80))

/* 寄存器说明：DX PHY control registers 
            Register in this field are connected to PHY interface directly
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_ADDR(base, n)  ((base) + (0x258+(n)*0x80))

/* 寄存器说明：Data block PHY miscellaneous control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_ADDR(base, n)  ((base) + (0x260+(n)*0x80))

/* 寄存器说明：Data block PHY miscellaneous control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_ADDR(base, n)  ((base) + (0x264+(n)*0x80))

/* 寄存器说明：Data block PHY debug signals
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_ADDR(base, n)  ((base) + (0x268+(n)*0x80))

/* 寄存器说明：Data block PHY debug signals
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_ADDR(base, n)  ((base) + (0x26C+(n)*0x80))

/* 寄存器说明：Data block PHY reserved control pins. This register is for PHY control and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXPHYRSVD_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYRSVD_ADDR(base, n)  ((base) + (0x270+(n)*0x80))

/* 寄存器说明：Data block PHY miscellaneous control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_ADDR(base, n)  ((base) + (0x274+(n)*0x80))

/* 寄存器说明：Data block PHY debug control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_ADDR(base, n)  ((base) + (0x278+(n)*0x80))

/* 寄存器说明：Data block PHY DCC control register. This register is for PHY control, and should not be modified.
   位域定义UNION结构:  SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_UNION */
#define SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_ADDR(base, n)  ((base) + (0x27C+(n)*0x80))





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
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_REVISION_UNION
 结构说明  : DDRC_PACK_REVISION 寄存器结构定义。地址偏移量:0x000，初值:0x00000012，宽度:32
 寄存器说明: This register shows current version of the PHY Control Block
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  minor    : 8;  /* bit[0-7]  : This field shows the minor version of this PHY control block. */
        unsigned int  major    : 8;  /* bit[8-15] : This field shows the major version of this PHY control block */
        unsigned int  reserved : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_REVISION_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_REVISION_minor_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_REVISION_minor_END       (7)
#define SOC_MDDRC_PACK_DDRC_PACK_REVISION_major_START     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_REVISION_major_END       (15)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_UNION
 结构说明  : DDRC_PACK_PHYINITCTRL 寄存器结构定义。地址偏移量:0x004，初值:0x0000000E，宽度:32
 寄存器说明: This register control the initialization of the PHY. Please note that write to this register will be ignored if init_en is already at '1' state.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  init_en        : 1;  /* bit[0]    : PHY Initialization Enable. 
                                                          Writing a '1' to this field will trigger the PHY initialization seqiemce.The exact initializtion executed is specified in bits 1 ~ 15 of this register. If the PHY initialization finished, this bit will be cleared to a '0'. Please note that writing a '0' to this field takes no effect. */
        unsigned int  pll_init_en    : 1;  /* bit[1]    : PLL Initialization Enable. 
                                                          If this field is used to activate the PLL initilization. If set to a '1', the PLL will be reset, and waiting for a period of time for PLL to locked. */
        unsigned int  dlymeas_en     : 1;  /* bit[2]    : Delay Measurement Enable.
                                                          This field is used to enable if intial delay measurement of the read delay line is activated. If set to a '1', the delay measurement will be enabled. */
        unsigned int  zcal_en        : 1;  /* bit[3]    : Impedance Calibration Enable.
                                                          This field is used to specified if the impedance caliblration will be executed. This bit will be cleared to a '0', once if the impedance calibration done. */
        unsigned int  wl_en          : 1;  /* bit[4]    : Write Leveling Enable.
                                                          Writing this field to a '1' will start the write leveling sequence. This field will be cleared to a '0' after write leveling finished.
                                                          Please note that the write leveling only applied to DDR3 only. */
        unsigned int  gt_en          : 1;  /* bit[5]    : Gate Training Enable. Writing this field to a '1' will start the gate training sequence. This field will be cleared to a '0' after gate training finished. */
        unsigned int  gdst_en        : 1;  /* bit[6]    : PHY Read Data Latch Train Enable.
                                                          Writing this field to a '1' will start the read data latch position training. This field will be cleared to a '0' after write leveling finished. */
        unsigned int  wl2_en         : 1;  /* bit[7]    : Second Write Leveling Enable. 
                                                          Writing this field to a '1' will start the write latency adjustment. This field will be cleared to a '0' after adjustment finished.
                                                          Please note that the write leveing adjustment only applied to DDR3 only. */
        unsigned int  rdet_en        : 1;  /* bit[8]    : Read Data Eye Training Enable. 
                                                          Writing this field to a '1' will start the read data eye training sequence. This field will be cleared to a '0' after read data eye training finished. */
        unsigned int  wdet_en        : 1;  /* bit[9]    : Write Data Eye Training Enable. 
                                                          Writing this field to a '1' will start the write data eye training sequence. This field will be cleared to a '0' after write data eye training finished. */
        unsigned int  dram_init_en   : 1;  /* bit[10]   : DRAM Initialization Enable
                                                          This field is used to control if the internal DRAM initialization sequence will be executed after PHY initialization done. For LPDDR2/3, this field will complete the initial sequence including tINIT3~tINIT5 &amp; tZQC. */
        unsigned int  cat_en         : 1;  /* bit[11]   : HW CA Traninig Enable.
                                                          Writing this field to a '1' will start the HW CA Training sequence. This field will be cleared to a '0' after CA Training finished.
                                                          Please note that the CA Training only applied to LPDDR3 only. */
        unsigned int  dram_rst       : 1;  /* bit[12]   : DRAM Reset. 
                                                          This field is used to initiate the DRAM reset sequence. If this field is set to a '1', the RESET# and CKE will be pulled LOW for t_dram_rst. After t_dram_rst, the CKE RESET# was pulled to HIGH while keeping CKE at LOW for t_cke_low. After t_cke_low, the the CKE was pulled to HIGH, and after t_cke_high, this field is cleared to '0' automatically. For LPDDR2/3, only the CKE will be active during DRAM reset sequence. */
        unsigned int  phy_rst        : 1;  /* bit[13]   : PHY Reset. 
                                                          This field is used to set PHY reset signal. To issue reset signal to PHY, this field should be set to a '1'. To end the reset signal to PHY this field should be cleared to a '0'. 
                                                          Please note that the reset signal connect to A/C block and all of the Data blocks. */
        unsigned int  pack_rst       : 1;  /* bit[14]   : PACK Reset.
                                                          This field is used to issue reset to the all digital circuit of the PACK including register file. If this field is set to a '1', all of the digital part, including control register, of the PHY will be set to default value. The reset will be automatically executed, and this bit will be cleared to a '0' after reset finished. */
        unsigned int  phyconn_rst    : 1;  /* bit[15]   : PHY Counter Reset. 
                                                          This field is used to set PHY counter reset signal. To issue reset signal to PHY counter, this field should be set to a '1'. To end the reset signal to PHY counter this field should be cleared to a '0'. 
                                                          Please note that the reset signal connect to A/C block counter and all of the Data blocks counter. */
        unsigned int  reserved       : 15; /* bit[16-30]: reserved */
        unsigned int  ctl_cke_bypass : 1;  /* bit[31]   : PACK's CKE bypass function enable.
                                                          1'b1: CKE was controlled by MC even when PACK controls the DFI interface
                                                          1'b0: CKE was controlled by PACK when PACK controls the DFI interface */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_init_en_START         (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_init_en_END           (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_pll_init_en_START     (1)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_pll_init_en_END       (1)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_dlymeas_en_START      (2)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_dlymeas_en_END        (2)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_zcal_en_START         (3)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_zcal_en_END           (3)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_wl_en_START           (4)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_wl_en_END             (4)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_gt_en_START           (5)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_gt_en_END             (5)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_gdst_en_START         (6)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_gdst_en_END           (6)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_wl2_en_START          (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_wl2_en_END            (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_rdet_en_START         (8)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_rdet_en_END           (8)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_wdet_en_START         (9)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_wdet_en_END           (9)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_dram_init_en_START    (10)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_dram_init_en_END      (10)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_cat_en_START          (11)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_cat_en_END            (11)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_dram_rst_START        (12)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_dram_rst_END          (12)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_phy_rst_START         (13)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_phy_rst_END           (13)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_pack_rst_START        (14)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_pack_rst_END          (14)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_phyconn_rst_START     (15)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_phyconn_rst_END       (15)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_ctl_cke_bypass_START  (31)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITCTRL_ctl_cke_bypass_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_UNION
 结构说明  : DDRC_PACK_PHYINITSTATUS 寄存器结构定义。地址偏移量:0x008，初值:0x00000000，宽度:32
 寄存器说明: This register shows the PHY status.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0   : 1;  /* bit[0]    : reserved */
        unsigned int  pll_lock_err : 1;  /* bit[1]    : PLL Locked Fail.
                                                        This flag indicate if one of the PLL within the PHY fail to assert locked flag after t_pll_lock timer.
                                                        The error status is cleard by writing a '1' to this field. */
        unsigned int  dlymeas_err  : 1;  /* bit[2]    : Delay Measurement Fail.
                                                        This flag indicate if one of the initial delay measurement failed. The error status is cleard by writing a '1' to this field. */
        unsigned int  zcal_err     : 1;  /* bit[3]    : Impedance Calibration Error.
                                                        This flag indicate if an error happened during impedance calibration. The error status is cleard by writing a '1' to this field. */
        unsigned int  wl_err       : 1;  /* bit[4]    : Write Leveling Error.
                                                        This flag indicate if an error happened during write leveling. The error status is cleard by writing a '1' to this field. */
        unsigned int  gt_err       : 1;  /* bit[5]    : Gate Training Error.
                                                        This flag indicate if an error happened during gate training. The error status is cleard by writing a '1' to this field. */
        unsigned int  gdst_err     : 1;  /* bit[6]    : PHY Read Data Latch Train Error.
                                                        This flag indicate if an error happened during read data latch training. This error status is cleared by writing a '1' to this field. */
        unsigned int  wl2_err      : 1;  /* bit[7]    : Write Leveling Adjustment Error.
                                                        This flag indicate if an error happened during write leveling adjustment. The error status is cleard by writing a '1' to this field. */
        unsigned int  rdet_err     : 1;  /* bit[8]    : Read Data Eye Calibration Error.
                                                        This flag indicate if an error happened during read data eye calibration. The error status is cleard by writing a '1' to this field. */
        unsigned int  wdet_err     : 1;  /* bit[9]    : Write Data Eye Calibration Error.
                                                        This flag indicate if an error happened during write data eye calibration. The error status is cleard by writing a '1' to this field. */
        unsigned int  cat_err      : 1;  /* bit[10]   : CA Training Error.
                                                        This flag indicate if an error happened during CA Trainig. The error status is cleard by writing a '1' to this field. */
        unsigned int  reserved_1   : 21; /* bit[11-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_pll_lock_err_START  (1)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_pll_lock_err_END    (1)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_dlymeas_err_START   (2)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_dlymeas_err_END     (2)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_zcal_err_START      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_zcal_err_END        (3)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_wl_err_START        (4)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_wl_err_END          (4)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_gt_err_START        (5)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_gt_err_END          (5)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_gdst_err_START      (6)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_gdst_err_END        (6)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_wl2_err_START       (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_wl2_err_END         (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_rdet_err_START      (8)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_rdet_err_END        (8)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_wdet_err_START      (9)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_wdet_err_END        (9)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_cat_err_START       (10)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYINITSTATUS_cat_err_END         (10)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_UNION
 结构说明  : DDRC_PACK_PHYCLKGATED 寄存器结构定义。地址偏移量:0x000C，初值:0x00000400，宽度:32
 寄存器说明: This register control the clock gated of PHY
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dx_phy_clkgated : 4;  /* bit[0-3]  : Clock gated for circuits in DAT blocks
                                                           1: Clock is enabled
                                                           0: Clock is gated
                                                           Note: These bits are automatically set as '1' when PLL is clocked without error */
        unsigned int  reserved_0      : 4;  /* bit[4-7]  : reserved */
        unsigned int  ac_phy_clkgated : 1;  /* bit[8]    : Clock gated for the circuits in AC block except CK and CK related circuits
                                                           1: Clock is enabled
                                                           0: Clock is gated
                                                           Note: This bit is automatically set as '1' when PLL is clocked without error */
        unsigned int  ck_phy_clkgated : 1;  /* bit[9]    : Dedicated clock gated control for the output CK and releated circuits
                                                           1: Clock is enabled
                                                           0: Clock is gated
                                                           Note: This bit is automatically set as '1' when PLL is clocked without error */
        unsigned int  dx_phgated_en   : 1;  /* bit[10]   : PHY Phase Clock Gated Function (automatically controlled by PACK's FSM) Enabled.
                                                           1: enable
                                                           0: disable */
        unsigned int  dx_phclkgated   : 1;  /* bit[11]   : PHY Phase Clock Gated control. This filed takes effect only when dx_phgated_en=1'b0. If dx_phgated_en='1b1, this field will be used and the gated function is automatically controlled by PACK's FSM. 
                                                           1: Clock enable 
                                                           0: Clock disable */
        unsigned int  dx_phgated_men  : 1;  /* bit[12]   : PHY Phase Clock Gated manual control enable. This filed takes effect only when dx_phgated_en=1'b0. If dx_phgated_en is HIGH and this filed is HIGH, the user can use dx_phclkgated to manually control the clock gating of PHY Phase Clock. 
                                                           1: enable manual control
                                                           0: disable manual control */
        unsigned int  reserved_1      : 18; /* bit[13-30]: reserved */
        unsigned int  byp_pll_lock    : 1;  /* bit[31]   : Bypass the pll lock signal. Users can assert this field to bypass pll lock signal when doing the pll re-lock procedure in the bypass mode (DFS) or when the pll lock signal can not be used.
                                                           1: Bypass the pll lock (do not use the pll lock singal to gate the PHY clocks)
                                                           0: Use the pll lock to gate the PHY clocks to prevent clock glitch */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_dx_phy_clkgated_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_dx_phy_clkgated_END    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_ac_phy_clkgated_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_ac_phy_clkgated_END    (8)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_ck_phy_clkgated_START  (9)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_ck_phy_clkgated_END    (9)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_dx_phgated_en_START    (10)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_dx_phgated_en_END      (10)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_dx_phclkgated_START    (11)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_dx_phclkgated_END      (11)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_dx_phgated_men_START   (12)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_dx_phgated_men_END     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_byp_pll_lock_START     (31)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCLKGATED_byp_pll_lock_END       (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER0_UNION
 结构说明  : DDRC_PACK_PHYTIMER0 寄存器结构定义。地址偏移量:0x010，初值:0x2710000F，宽度:32
 寄存器说明: This register specified the timing parameter required by the PHY.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_dram_reset    : 8;  /* bit[0-7]  : DDR3 DRAM Reset. This field is used to specify the time issuing RESET signaling to the DRAM (number of PCLK clock cycles). */
        unsigned int  reserved        : 4;  /* bit[8-11] : reserved */
        unsigned int  t_dram_cke_high : 20; /* bit[12-31]: DRAM CKE High Initialization.
                                                           This field sepcify the number of PCLK clock cycles of first DRAM command from CKE pulled to HIGH. (LPDDR23 tINIT3) */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER0_t_dram_reset_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER0_t_dram_reset_END       (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER0_t_dram_cke_high_START  (12)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER0_t_dram_cke_high_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER1_UNION
 结构说明  : DDRC_PACK_PHYTIMER1 寄存器结构定义。地址偏移量:0x014，初值:0x00002000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_dram_cke_low : 16; /* bit[0-15] : DRAM CKE Low Initialization.
                                                          This field sepcify the number of PCLK clock cycles which CKE should be kept LOW after reset power and clock stable.  */
        unsigned int  reserved       : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER1_t_dram_cke_low_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYTIMER1_t_dram_cke_low_END    (15)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PLLCTRL_UNION
 结构说明  : DDRC_PACK_PLLCTRL 寄存器结构定义。地址偏移量:0x018，初值:0x00000000，宽度:32
 寄存器说明: This register specified the timing paramters for PLL in both address /command, and data block.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pll_pwdn       : 3;  /* bit[0-2] : PLL Power Down.
                                                         This field is used to set the power down status of the PLL input. If this field is set to a '1', the PLL power down pin will be activated. Writing a '0' to this field will remove the deactivated the PLL power down pin.
                                                         Bit[2] for AC PLL
                                                         Bit[1] for DX1 (DATA1) PLL
                                                         Bit[0] for DX0 (DATA0) PLL */
        unsigned int  reserved_0     : 1;  /* bit[3]   : reserved */
        unsigned int  pll_freq_range : 2;  /* bit[4-5] : PLL Frequency Range Select.
                                                         This field specify the frequency input range of the PLLs. */
        unsigned int  reserved_1     : 26; /* bit[6-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PLLCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PLLCTRL_pll_pwdn_START        (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PLLCTRL_pll_pwdn_END          (2)
#define SOC_MDDRC_PACK_DDRC_PACK_PLLCTRL_pll_freq_range_START  (4)
#define SOC_MDDRC_PACK_DDRC_PACK_PLLCTRL_pll_freq_range_END    (5)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PLLTIMER_UNION
 结构说明  : DDRC_PACK_PLLTIMER 寄存器结构定义。地址偏移量:0x01C，初值:0x4E200080，宽度:32
 寄存器说明: This register specified the control for PLL in both address /command, and data block.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_pll_rst  : 8;  /* bit[0-7]  : PLL Reset Time.
                                                      This field specified the number of PCLK clock cycles to issue reset to the PLL.  */
        unsigned int  reserved   : 8;  /* bit[8-15] : reserved */
        unsigned int  t_pll_lock : 16; /* bit[16-31]: PLL Lock Time.
                                                      This field specified the number of PCLK clock cycles wait for the PLLs to be locked. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PLLTIMER_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PLLTIMER_t_pll_rst_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PLLTIMER_t_pll_rst_END     (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PLLTIMER_t_pll_lock_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_PLLTIMER_t_pll_lock_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_UNION
 结构说明  : DDRC_PACK_DLYMEASCTRL 寄存器结构定义。地址偏移量:0x020，初值:0x00000471，宽度:32
 寄存器说明: This register specify the control for delay measurement of the read delay line.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dly_meas_type   : 1;  /* bit[0]    : Delay Measurement Type.
                                                           This field specify if the delay measurement is for 1 clock cycle or half clock cycle. If this field is set to a '1', a full clock cycle will be measured. */
        unsigned int  dly_track_type  : 1;  /* bit[1]    : Delay Tracking Type.
                                                           This field specify if priority updated from the PHY was used while dynamic tracking. If this field is set to a '1', the priority update from the PHY is used. */
        unsigned int  dynamic_dqsgth  : 2;  /* bit[2-3]  : Dynamic DQSG Update Threshold.
                                                           This field specify the number of taps accumulated before to update the RDQSG delay line.
                                                           2,3: means update every time
                                                           1: means update every 2 times
                                                           0: means update 4 times */
        unsigned int  dynamic_dqsgen  : 1;  /* bit[4]    : Dynamic DQSG Tracking Enable. */
        unsigned int  dynamic_dqsen   : 1;  /* bit[5]    : Dynamic DQS Tracking Enable.
                                                           This field should be set as HIGH when delay measurement. */
        unsigned int  phy_autoref_en  : 1;  /* bit[6]    : PHY Auto-Refresh Tracking Enable. */
        unsigned int  reserved_0      : 1;  /* bit[7]    : reserved */
        unsigned int  dly_track_limit : 7;  /* bit[8-14] : Delay Tracking Limit.
                                                           This field specify the maximum number of accumulated taps allowed while dynamic tracking.  */
        unsigned int  reserved_1      : 17; /* bit[15-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dly_meas_type_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dly_meas_type_END      (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dly_track_type_START   (1)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dly_track_type_END     (1)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dynamic_dqsgth_START   (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dynamic_dqsgth_END     (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dynamic_dqsgen_START   (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dynamic_dqsgen_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dynamic_dqsen_START    (5)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dynamic_dqsen_END      (5)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_phy_autoref_en_START   (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_phy_autoref_en_END     (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dly_track_limit_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DLYMEASCTRL_dly_track_limit_END    (14)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_UNION
 结构说明  : DDRC_PACK_IMPCTRL 寄存器结构定义。地址偏移量:0x024，初值:0x80008020，宽度:32
 寄存器说明: This register specify the control the ZQ calibration.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  zcomp_rsp_dly : 6;  /* bit[0-5]  : ZQ Comparator Response Time.
                                                         This field specify the delay that the zcomp result after apply the zctrl. */
        unsigned int  reserved_0    : 6;  /* bit[6-11] : reserved */
        unsigned int  zcomp_num     : 4;  /* bit[12-15]: ZQ Comparator iteration Time.
                                                         This field specify the iteration time of zcomp changing detection. */
        unsigned int  reserved_1    : 15; /* bit[16-30]: reserved */
        unsigned int  zcfuzzy_en    : 1;  /* bit[31]   : ZQ Comparator result selection by weighted method. When disable this function the precision could be improved but has the possibility to get failed ZQ Comparison. When enable this function it can be guaranteed to get successful ZQ Comparion.
                                                         0: disable
                                                         1: enable (default) */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_zcomp_rsp_dly_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_zcomp_rsp_dly_END    (5)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_zcomp_num_START      (12)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_zcomp_num_END        (15)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_zcfuzzy_en_START     (31)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPCTRL_zcfuzzy_en_END       (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_UNION
 结构说明  : DDRC_PACK_IMPSTATUS 寄存器结构定义。地址偏移量:0x028，初值:0x00200020，宽度:32
 寄存器说明: This register specify the ZQ calibration result.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  zcode_ndrv      : 6;  /* bit[0-5]  : N-Drive Impedance Control Code.
                                                           This field is used to specify the n-driver impedance control code. This register will be update by user or will be updated every time the ZQ calibration doen. */
        unsigned int  reserved_0      : 1;  /* bit[6]    : reserved */
        unsigned int  ndrv_cal_result : 1;  /* bit[7]    : N-Drive Calibration Result.
                                                           This field specify the ZQ calibration result. The valid encoding is:
                                                           0: Calibration done with OK.
                                                           1: Calibration down with Error.
                                                           Writing an '1' to this field will clear this field to a '0'. */
        unsigned int  zcode_ndrv_cal  : 7;  /* bit[8-14] : N-Drive Impedance Calibration Code.
                                                           This field shows the n-driver calibration code during impedance calibration. */
        unsigned int  reserved_1      : 1;  /* bit[15]   : reserved */
        unsigned int  zcode_pdrv      : 6;  /* bit[16-21]: P-Drive Impedance Control Code.
                                                           This field is used to specify the p-driver impedance control code. This register will be update by user or will be updated every time the ZQ calibration doen. */
        unsigned int  reserved_2      : 1;  /* bit[22]   : reserved */
        unsigned int  pdrv_cal_result : 1;  /* bit[23]   : P-Drive Calibration Result.
                                                           This field specify the ZQ calibration result. The valid encoding is:
                                                           0: Calibration done with OK.
                                                           1: Calibration down with Error.
                                                           Writing an '1' to this field will clear this field to a '0'. */
        unsigned int  zcode_pdrv_cal  : 7;  /* bit[24-30]: P-Drive Impedance Calibration Code.
                                                           This field shows the p-driver calibration code during impedance calibration. */
        unsigned int  reserved_3      : 1;  /* bit[31]   : reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_zcode_ndrv_START       (0)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_zcode_ndrv_END         (5)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_ndrv_cal_result_START  (7)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_ndrv_cal_result_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_zcode_ndrv_cal_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_zcode_ndrv_cal_END     (14)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_zcode_pdrv_START       (16)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_zcode_pdrv_END         (21)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_pdrv_cal_result_START  (23)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_pdrv_cal_result_END    (23)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_zcode_pdrv_cal_START   (24)
#define SOC_MDDRC_PACK_DDRC_PACK_IMPSTATUS_zcode_pdrv_cal_END     (30)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_UNION
 结构说明  : DDRC_PACK_DRAMCFG 寄存器结构定义。地址偏移量:0x02C，初值:0x00000405，宽度:32
 寄存器说明: This register is used to configure the DRAM system.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dcfg_type          : 3;  /* bit[0-2]  : DRAM Type Selection.
                                                              This field specify the DDR RAM type.The valid encoding is as below:
                                                              010: DDR3
                                                              101: LPDDR2/LPDDR3
                                                              others: reserved */
        unsigned int  reserved_0         : 1;  /* bit[3]    : reserved */
        unsigned int  nosra              : 1;  /* bit[4]    : No Simultaneous Rank Access.
                                                              This field specify if simultaneously rank access on the same clock cycle is allowed. If set to a '1', the rank access will be take in turn. 
                                                              It shall be noted this bit can only enabled when number of rank is 2 or 4.
                                                              This bit shall be set as 0 in single rank configuration */
        unsigned int  ma2t               : 1;  /* bit[5]    : Two Cycle on Address / Command.
                                                              This field specify if 2T timing should be used by PHY controller internally generated DRAM command. */
        unsigned int  maddr_mir          : 1;  /* bit[6]    : Address mirroring
                                                              This bit shall be set as 0 in single rank configuration */
        unsigned int  reserved_1         : 1;  /* bit[7]    : Address mirroring
                                                              This bit shall be set as 0 in single rank configuration */
        unsigned int  lpddr23_pre_en     : 1;  /* bit[8]    : LPDDR23 initalization precharge all enable.
                                                              1: Issue precharge all command before RESET command during initialization sequence.
                                                              0: disable (default) */
        unsigned int  lpddr23_mrw1617_en : 1;  /* bit[9]    : LPDDR23 initalization MRW16/17 enable.
                                                              1: Issue MRW16 then MRW17 commands during initialization sequence.
                                                              0: disable (default) */
        unsigned int  lpddr23_zqc_en     : 1;  /* bit[10]   : LPDDR23 initalization ZQC (init) enable.
                                                              1: Issue ZQC(init) commands during initialization sequence rank by rank (default).
                                                              0: disable */
        unsigned int  reserved_2         : 21; /* bit[11-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_dcfg_type_START           (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_dcfg_type_END             (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_nosra_START               (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_nosra_END                 (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_ma2t_START                (5)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_ma2t_END                  (5)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_maddr_mir_START           (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_maddr_mir_END             (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_lpddr23_pre_en_START      (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_lpddr23_pre_en_END        (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_lpddr23_mrw1617_en_START  (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_lpddr23_mrw1617_en_END    (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_lpddr23_zqc_en_START      (10)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMCFG_lpddr23_zqc_en_END        (10)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_UNION
 结构说明  : DDRC_PACK_DRAMTIMER0 寄存器结构定义。地址偏移量:0x030，初值:0x00000000，宽度:32
 寄存器说明: This register specify the DRAM timing parameters. Please note that the command delay specified in this register only apply to command issued by the PHY controller internally.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_rtp : 4;  /* bit[0-3]  : Read to Precharge Command Delay. */
        unsigned int  t_wtr : 4;  /* bit[4-7]  : Write to Read Command Delay. */
        unsigned int  t_rp  : 4;  /* bit[8-11] : Precharge Command Period.
                                                 The unit is 2 clock cycles. */
        unsigned int  t_rcd : 4;  /* bit[12-15]: Activated to Read / Write Delay. */
        unsigned int  t_ras : 6;  /* bit[16-21]: Activated to Precharge Command Delay. */
        unsigned int  t_rrd : 4;  /* bit[22-25]: Activated to Activated on Different Bank Command Delay. */
        unsigned int  t_rc  : 6;  /* bit[26-31]: Activated to Activated on Same Bank Command Delay. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rtp_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rtp_END    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_wtr_START  (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_wtr_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rp_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rp_END     (11)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rcd_START  (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rcd_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_ras_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_ras_END    (21)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rrd_START  (22)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rrd_END    (25)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rc_START   (26)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER0_t_rc_END     (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_UNION
 结构说明  : DDRC_PACK_DRAMTIMER1 寄存器结构定义。地址偏移量:0x034，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_mrd    : 4;  /* bit[0-3]  : Load Mode Register to Load Mode Register Command Delay. Not used. */
        unsigned int  t_mod    : 5;  /* bit[4-8]  : Load Mode Register to Non Load Mode Register Command Delay. This field is used as max(t_mod,t_mrd) in DDR3. This filed is used as max(t_mrw,tmrd) in LPDDR3 and used as t_mrw in LPDDR2. */
        unsigned int  t_faw    : 6;  /* bit[9-14] : Four Bank Activated Period. This field only applied to 8 bank device only. */
        unsigned int  reserved_0: 1;  /* bit[15]   : reserved */
        unsigned int  t_rfc    : 9;  /* bit[16-24]: Refresh to Refresh Command Delay. */
        unsigned int  t_rtw    : 5;  /* bit[25-29]: Read to Write Command Delay.
                                                    This field specify if extra cycle inserted between read and write command. (to compensate tdqsck)
                                                    This value should be 3 for common condition when DDR3 is used.
                                                    This value should be 6 for common condition when LPDDR2/3 is used. */
        unsigned int  reserved_1: 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_mrd_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_mrd_END       (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_mod_START     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_mod_END       (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_faw_START     (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_faw_END       (14)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_rfc_START     (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_rfc_END       (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_rtw_START     (25)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER1_t_rtw_END       (29)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_UNION
 结构说明  : DDRC_PACK_DRAMTIMER2 寄存器结构定义。地址偏移量:0x038，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_xs     : 10; /* bit[0-9]  : Self Refresh Exit Delay. */
        unsigned int  t_xp     : 5;  /* bit[10-14]: Power Down Exit Delay. */
        unsigned int  reserved_0: 1;  /* bit[15]   : reserved */
        unsigned int  t_cke    : 4;  /* bit[16-19]: CKE minimum pulse width. */
        unsigned int  t_dllk   : 10; /* bit[20-29]: DLL Lock time. */
        unsigned int  reserved_1: 1;  /* bit[30]   : reserved */
        unsigned int  t_ccd    : 1;  /* bit[31]   : Read to Read and Write to Write Command Delay.
                                                    0 = BL/2 (For DDR3, this value should be 4)
                                                    1 = BL/2 + 1 (For DDR3, this value should be 5) */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_xs_START      (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_xs_END        (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_xp_START      (10)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_xp_END        (14)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_cke_START     (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_cke_END       (19)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_dllk_START    (20)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_dllk_END      (29)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_ccd_START     (31)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER2_t_ccd_END       (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_UNION
 结构说明  : DDRC_PACK_DRAMTIMER3 寄存器结构定义。地址偏移量:0x03C，初值:0x00100200，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_zcal  : 10; /* bit[0-9]  : ZQ Calibration to Command Delay.
                                                   This field specify the number of clock cycles to wait after initial ZQ calibration.
                                                   For DDR3 ZQ normal operation full calibration, this value is divided by 2. For DDR3 ZQ initialization calibration, this value is divided by 4. For LPDDR2/3, this field specify the value of tZQINIT during initialization. */
        unsigned int  t_init5 : 14; /* bit[10-23]: LPDDR Device Initialization Time.
                                                   This field specify the time need for the LPDDR to set the DAI bit to a '1'. */
        unsigned int  t_wl    : 4;  /* bit[24-27]: Write latency (AL+CWL) */
        unsigned int  t_wr    : 4;  /* bit[28-31]: Write recovery cycle */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_t_zcal_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_t_zcal_END     (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_t_init5_START  (10)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_t_init5_END    (23)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_t_wl_START     (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_t_wl_END       (27)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_t_wr_START     (28)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER3_t_wr_END       (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_UNION
 结构说明  : DDRC_PACK_DRAMTIMER4 寄存器结构定义。地址偏移量:0x040，初值:0x00000000，宽度:32
 寄存器说明: This register specify the DRAM Write Leveling timing parameter. This register only applied to DDR3/LPDDR3 type SDRAM.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_wlmrd  : 6;  /* bit[0-5]  : Write Leveling Mode Programmed (Load Mode Register) to first DQS/DQS# rising edge delay. (dficlk cycle) */
        unsigned int  reserved_0: 1;  /* bit[6]    : reserved */
        unsigned int  t_wlo    : 5;  /* bit[7-11] : Write Leveling Output Delay.
                                                    This field specify the write leveling output delay in cycles. (dficlk cycle) */
        unsigned int  t_odt    : 4;  /* bit[12-15]: ODT turn-on cycles */
        unsigned int  reserved_1: 8;  /* bit[16-23]: reserved */
        unsigned int  t_odton  : 4;  /* bit[24-27]: ODT turn-on latency (Number of cycle from write command to assert of ODT) */
        unsigned int  reserved_2: 4;  /* bit[28-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_t_wlmrd_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_t_wlmrd_END     (5)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_t_wlo_START     (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_t_wlo_END       (11)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_t_odt_START     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_t_odt_END       (15)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_t_odton_START   (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DRAMTIMER4_t_odton_END     (27)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ODTCR_UNION
 结构说明  : DDRC_PACK_ODTCR 寄存器结构定义。地址偏移量:0x044，初值:0x00000000，宽度:32
 寄存器说明: This register specify the ODT control on different rank while reading or writing to particular rank.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wodt_rank0 : 4;  /* bit[0-3]  : Write Rank0 ODT Control.
                                                      This field specify the ODT issued while write on Rank0. The real valid bit-width is the rumber of rank. */
        unsigned int  wodt_rank1 : 4;  /* bit[4-7]  : Write Rank1 ODT Control.
                                                      This field specify the ODT issued while write on Rank1. The real valid bit-width is the rumber of rank. */
        unsigned int  wodt_rank2 : 4;  /* bit[8-11] : Write Rank2 ODT Control.
                                                      This field specify the ODT issued while write on Rank2. The real valid bit-width is the rumber of rank. */
        unsigned int  wodt_rank3 : 4;  /* bit[12-15]: Write Rank3 ODT Control.
                                                      This field specify the ODT issued while write on Rank3. The real valid bit-width is the rumber of rank. */
        unsigned int  reserved   : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ODTCR_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ODTCR_wodt_rank0_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ODTCR_wodt_rank0_END    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ODTCR_wodt_rank1_START  (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ODTCR_wodt_rank1_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_ODTCR_wodt_rank2_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ODTCR_wodt_rank2_END    (11)
#define SOC_MDDRC_PACK_DDRC_PACK_ODTCR_wodt_rank3_START  (12)
#define SOC_MDDRC_PACK_DDRC_PACK_ODTCR_wodt_rank3_END    (15)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_UNION
 结构说明  : DDRC_PACK_TRAINCTRL0 寄存器结构定义。地址偏移量:0x048，初值:0xD0441080，宽度:32
 寄存器说明: This register specify the training control.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dtr_rank         : 4;  /* bit[0-3]  : Training Rank.
                                                            This register specify the rank used to do training. And after training done, this field is used to select the source of rank specific settings. */
        unsigned int  dqsgsl_mrgn      : 6;  /* bit[4-9]  : Read DQS Gate Margin
                                                            This field specify the margin to reserve after DQS gate training. The number of phyase specify in this field will be decreased to create DQS gating margin.
                                                            The valid encoding of the setting is as below:
                                                            6'b00100: 1/4 cycle
                                                            6'b01000: 1/2 cycle (recommended)
                                                            6'b01100: 3/4 cycle
                                                            6'b10000: 1 cycle
                                                            Other values is reserved. */
        unsigned int  dqsgrt_shft      : 6;  /* bit[10-15]: Gate Retraing Shift Window.
                                                            This field specify the value to be shift if gate phase adjustment required after gate retraining checks. 
                                                            The phase movement will be the setting value multiple by 2. Please note that 3'b000 is reserved, and should not be used. The recommended value is set as dqsgrt_chk. */
        unsigned int  dqsgrt_chk       : 6;  /* bit[16-21]: Gate Retraing Check Window.
                                                            This field specify the delta value to be checked during gate retraining check. The delta will be added/substrated from the read DQS gate. This two position will be checked to determine if gating phase adjustment required. 
                                                            The phase movement will be the setting value multiple by 2. Please note that 3'b000 is reserved, and should not be used. The recommended values are 0x1~0x3. */
        unsigned int  dqsg_cstep       : 2;  /* bit[22-23]: Gate training coarse tuning step (The unit is a phase). The recommended values are 0x0~0x2.
                                                            2'd0: step=1
                                                            2'd1: step=2 (default)
                                                            2'd2: step=3
                                                            2'd3: step=4 */
        unsigned int  gtds_rspdly      : 6;  /* bit[24-29]: Gate / Read Latch Training Response Delay.
                                                            This field specify the waiting time to check the response from PHY while doing Gate and Read Latch training. (dficlk cycle) */
        unsigned int  dtrerrstop       : 1;  /* bit[30]   : Data Training Error Stop.
                                                            This field controls if the data training will be terminated if error happened. The data training will be stopped if this bit is set to a '1'. If this bit set to a '0', the training will continue even have error happened. The error status will be recorded either this bit set to a '0' or '1'. */
        unsigned int  skip_eye_opening : 1;  /* bit[31]   : Skip Eye Opening of Data Eye Training.
                                                            This field is to specified if the eye opening should be skipped. If this field is set to a '1', the eye opening before data eye training will be skipped. If this field is set to a '0', the eye opening will be executed before data eye training. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dtr_rank_START          (0)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dtr_rank_END            (3)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dqsgsl_mrgn_START       (4)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dqsgsl_mrgn_END         (9)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dqsgrt_shft_START       (10)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dqsgrt_shft_END         (15)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dqsgrt_chk_START        (16)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dqsgrt_chk_END          (21)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dqsg_cstep_START        (22)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dqsg_cstep_END          (23)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_gtds_rspdly_START       (24)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_gtds_rspdly_END         (29)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dtrerrstop_START        (30)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_dtrerrstop_END          (30)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_skip_eye_opening_START  (31)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL0_skip_eye_opening_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_RANKEN_UNION
 结构说明  : DDRC_PACK_RANKEN 寄存器结构定义。地址偏移量:0x04C，初值:0x00000003，宽度:32
 寄存器说明: This register specify the training rank control.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rank_en  : 16; /* bit[0-15] : Training Rank enable.
                                                    This register specify if the rank is enabled or not. This field is used to indicated if the dtr_rank is enabled. 
                                                    1: enable (default)
                                                    0: disable
                                                    Bit0 indicates the RANK0.
                                                    Bit1 indicates the RANK1.
                                                    …
                                                    Bit15 indicateds the RANK15.
                                                    The maximum number of RANK is specified by the system. */
        unsigned int  reserved : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_RANKEN_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_RANKEN_rank_en_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_RANKEN_rank_en_END     (15)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_UNION
 结构说明  : DDRC_PACK_TRAINMADDR 寄存器结构定义。地址偏移量:0x0050，初值:0x00000000，宽度:32
 寄存器说明: This register specify the starting memory address where can be used to do data training. The minimum space required for data training should be 64-bytes, and the allocated memory should be 64-bytes alignment.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dtbank   : 3;  /* bit[0-2]  : Data Training Bank Address.
                                                    This register is used as the memory bank address where is used during data training if required. */
        unsigned int  reserved : 1;  /* bit[3]    : reserved */
        unsigned int  dtcol    : 12; /* bit[4-15] : Data Training Column Address.
                                                    This register is used as the column address which is used during data training if required. The field should be 0. */
        unsigned int  dtrow    : 16; /* bit[16-31]: Data Training Row Address.
                                                    This register is used as the memory row address where is used during data training if required. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_dtbank_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_dtbank_END      (2)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_dtcol_START     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_dtcol_END       (15)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_dtrow_START     (16)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINMADDR_dtrow_END       (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_UNION
 结构说明  : DDRC_PACK_BISTCTRL 寄存器结构定义。地址偏移量:0x0054，初值:0x00000000，宽度:32
 寄存器说明: This register specify the behavior of the built-in self test.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bist_op        : 2;  /* bit[0-1]  : During Reg Write this field is for Bist Operation Select.
                                                           This field specify the BIST operation. The valid encoding is as below.
                                                           00: BIST Stop.
                                                           01: BIST Run.
                                                           10: BIST Reset.
                                                           11: Reserved.
                                                           Writing the Reset will clear all the status register, and this field will be cleared to BIST Stop.
                                                          
                                                          During Reg Read this field is for BIST active status (only bit0 is valid, bit1 is read as 1'b0)
                                                           1: BIST active
                                                           0: BIST not active */
        unsigned int  reserved_0     : 2;  /* bit[2-3]  : reserved */
        unsigned int  bist_mode      : 3;  /* bit[4-6]  : BIST Mode Select.
                                                          This field specify the mode of BIST testing. The valid encoding is as below.
                                                          000: Pattern generated only.
                                                          100: Loopback is before output buffer.
                                                          101: Loopback is after output buffer.
                                                          110: DRAM read / write mode.
                                                          Other encodings except above is reserved.
                                                          It shall be noted that bist_mode is only allowed to be changed when bist_done is 1. And when DATA Loopback mode is selected, swapwl_en/swap_en should be set as all zeros (no swap). */
        unsigned int  reserved_1     : 1;  /* bit[7]    : reserved */
        unsigned int  bist_pat       : 2;  /* bit[8-9]  : BIST Pattern Generation.
                                                          This field specify the BIST pattern generated for test. It shall be noted that bist_pat is only allowed to be changed when bist_done is 1.
                                                          00: Walking 0.
                                                          01: Walking 1.
                                                          10: LFSR-based random generation.
                                                          11: User programmed. */
        unsigned int  reserved_2     : 6;  /* bit[10-15]: reserved */
        unsigned int  bist_fail_stop : 8;  /* bit[16-23]: Bist Stop Error Count.
                                                          This field specify the number of failure which the BIST should stop.If this field is set to 0x00, the bist will run infinite until BIST operation is stopped or reset. If non-zero value specified, the BIST will stopped while the accumulated error reach the count specified in this field. */
        unsigned int  reserved_3     : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_bist_op_START         (0)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_bist_op_END           (1)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_bist_mode_START       (4)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_bist_mode_END         (6)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_bist_pat_START        (8)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_bist_pat_END          (9)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_bist_fail_stop_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTCTRL_bist_fail_stop_END    (23)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_UNION
 结构说明  : DDRC_PACK_BISTDATA0 寄存器结构定义。地址偏移量:0x0058，初值:0x00000000，宽度:32
 寄存器说明: This register specify the BIST data used for DRAM loopback test. Please note that this register only valid while the bist_pat is set to &quot;User Defined&quot;
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bist_data_b0 : 8;  /* bit[0-7]  : BIST Data Byte 0.
                                                        This field is used as the first beat of burst during BIST test. */
        unsigned int  bist_data_b1 : 8;  /* bit[8-15] : BIST Data Byte 1.
                                                        This field is used as the second beat of burst during BIST test. */
        unsigned int  bist_data_b2 : 8;  /* bit[16-23]: BIST Data Byte 2.
                                                        This field is used as the third beat of burst during BIST test. */
        unsigned int  bist_data_b3 : 8;  /* bit[24-31]: BIST Data Byte 3.
                                                        This field is used as the forth beat of burst during BIST test */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_bist_data_b0_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_bist_data_b0_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_bist_data_b1_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_bist_data_b1_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_bist_data_b2_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_bist_data_b2_END    (23)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_bist_data_b3_START  (24)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA0_bist_data_b3_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_UNION
 结构说明  : DDRC_PACK_BISTDATA1 寄存器结构定义。地址偏移量:0x005C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bist_data_b4 : 8;  /* bit[0-7]  : BIST Data Byte 4.
                                                        This field is used as the forth beat of burst during BIST test. */
        unsigned int  bist_data_b5 : 8;  /* bit[8-15] : BIST Data Byte 5.
                                                        This field is used as the fifth beat of burst during BIST test. */
        unsigned int  bist_data_b6 : 8;  /* bit[16-23]: BIST Data Byte 6.
                                                        This field is used as the sixth beat of burst during BIST test. */
        unsigned int  bist_data_b7 : 8;  /* bit[24-31]: BIST Data Byte 7.
                                                        This field is used as the eighth beat of burst during BIST test */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_bist_data_b4_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_bist_data_b4_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_bist_data_b5_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_bist_data_b5_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_bist_data_b6_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_bist_data_b6_END    (23)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_bist_data_b7_START  (24)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTDATA1_bist_data_b7_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_UNION
 结构说明  : DDRC_PACK_BISTSTATUS 寄存器结构定义。地址偏移量:0x0060，初值:0x00000001，宽度:32
 寄存器说明: This register shows the result and status of BIST testing.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bist_done  : 1;  /* bit[0]   : BIST Done 
                                                     This bit shows if the BIST is done. Please note that if the Bist Stop Error Count (bist_fail_stop) was set to 0, this field will never be asserted. */
        unsigned int  bist_acerr : 1;  /* bit[1]   : BIST Error on AC Block.
                                                     This bit shows if any error happened on the AC during BIST testing. */
        unsigned int  bist_dxerr : 1;  /* bit[2]   : BIST Error on DX Blocks.
                                                     This bit shows if any error happened on the DXn during BIST testing. */
        unsigned int  reserved   : 29; /* bit[3-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_bist_done_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_bist_done_END     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_bist_acerr_START  (1)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_bist_acerr_END    (1)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_bist_dxerr_START  (2)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTSTATUS_bist_dxerr_END    (2)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_MODEREG01_UNION
 结构说明  : DDRC_PACK_MODEREG01 寄存器结构定义。地址偏移量:0x0064，初值:0x00000000，宽度:32
 寄存器说明: This register defines the contents of the Mode Register.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mr0 : 16; /* bit[0-15] : Mode Register 0.
                                               For DDR/DDR2/DDR3/LPDDR, this register sepcify the content of Mode Register 0.
                                               For LPDDR2/LPDDR3, this register defines the MR1/MR2 of the mode register.
                                                bit[15:8]: MR2 OP
                                                bit[7:0]: MR1 OP */
        unsigned int  mr1 : 16; /* bit[16-31]: Mode Register 1.
                                               For DDR/DDR2/DDR3/LPDDR, this register sepcify the content of Extend Mode Register 1.
                                               For LPDDR2/LPDDR3, this register defines the content of MR3/MR9 ( mode register).
                                                bit[31:24]: MR9 OP
                                                bit[23:16]: MR3 OP */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_MODEREG01_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG01_mr0_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG01_mr0_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG01_mr1_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG01_mr1_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_MODEREG23_UNION
 结构说明  : DDRC_PACK_MODEREG23 寄存器结构定义。地址偏移量:0x0068，初值:0xFF0A0000，宽度:32
 寄存器说明: This register defines the contents of the Mode Register.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mr2 : 16; /* bit[0-15] : Mode Register 2.
                                               For DDR/DDR2/DDR3, this register is used to specify the Extend Mode Register 2. 
                                               For LPDDR2/LPDDR3, this register is used to specify the MR16/17.
                                                bit[15:8]: MR17 OP
                                                bit[ 7:0]: MR16 OP */
        unsigned int  mr3 : 16; /* bit[16-31]: Mode Register 3.
                                               For DDR/DDR2/DDR3, this register is used to specify the Extend Mode Register 3. 
                                               For LPDDR23, this register is for ZQ command - [31:24]OP, [23:16]MA. The ZQ commands will be issued rank by rank after tINIT5.
                                                bit[31:24]: MR10 OP
                                                bit[23:16]: MR10 MA */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_MODEREG23_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG23_mr2_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG23_mr2_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG23_mr3_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_MODEREG23_mr3_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_UNION
 结构说明  : DDRC_PACK_DETPATTERN 寄存器结构定义。地址偏移量:0x006C，初值:0x00FFAA55，宽度:32
 寄存器说明: Write/Read DET Pattern Register.
            These fields are used to fine-tune the training procedure. Users are not recommend to modify them.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  det_pat55 : 8;  /* bit[0-7]  : DET pattern0 */
        unsigned int  det_patAA : 8;  /* bit[8-15] : DET pattern1 */
        unsigned int  det_patFF : 8;  /* bit[16-23]: DET pattern2 */
        unsigned int  reserved  : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_det_pat55_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_det_pat55_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_det_patAA_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_det_patAA_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_det_patFF_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DETPATTERN_det_patFF_END    (23)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_MISC_UNION
 结构说明  : DDRC_PACK_MISC 寄存器结构定义。地址偏移量:0x0070，初值:0x00040000，宽度:32
 寄存器说明: Miscellaneous control register
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cfg_wl         : 5;  /* bit[0-4]  : PHY write latency.
                                                          It defines the number of cycles from wrire command to assert of wrdata_en and is equal to CWL+AL-1
                                                          (for LPDDR2/3, the filed should be WL+1). If the DFI clock ratio is 1:2, cfg_wl should be equal or greater than 2 */
        unsigned int  reserved_0     : 2;  /* bit[5-6]  : reserved */
        unsigned int  cfg_rl         : 5;  /* bit[7-11] : PHY read latency.
                                                          It defines the number of cycles from read command to assert of rddata_en and is equal to CL+AL-2
                                                          (for LPDDR2/3, the filed should be RL-2) */
        unsigned int  reserved_1     : 4;  /* bit[12-15]: reserved */
        unsigned int  scramb_en      : 1;  /* bit[16]   : Data scrambler enable
                                                          Set as '1' to enable data sramble function */
        unsigned int  addr_toggle    : 1;  /* bit[17]   : Toggle addess bus every clock cycle
                                                          This bit is set as '1' for burn-in test only */
        unsigned int  addr_delay     : 1;  /* bit[18]   : Add 1-T delay on address/command 
                                                          This bit shall be set as '1' when CWL=5 */
        unsigned int  cfg_dlyupd     : 1;  /* bit[19]   : To enable transparent mode of delay update module. When this bit is set to 1, the delay setting in registers will be applied to PHY immediately */
        unsigned int  swap_en        : 3;  /* bit[20-22]: Swap DQ bus
                                                          000: No Swap
                                                          001: Swap 1
                                                          010: Swap 2
                                                          011: Swap 3
                                                          100: Swap 4
                                                          Note: when DATA BIST Loopback mode is selected, this field should be set as 0x0. */
        unsigned int  swapaddr_en    : 2;  /* bit[23-24]: Swap ADDRESS bus
                                                          00: No Swap
                                                          01: 32-Bit Flyby-4, Flyby-2 Swap
                                                          10: 16-Bit T-2 Swap */
        unsigned int  swapdm_en      : 2;  /* bit[25-26]: Swap DM bus
                                                          00: No Swap
                                                          01: 16-Bit T2 Swap */
        unsigned int  swapwl_en      : 3;  /* bit[27-29]: Swap WL/CA Result
                                                          000: No Swap
                                                          001: Swap 1
                                                          010: Swap 2
                                                          011: Swap 3
                                                          100: Swap 4
                                                          Note: when DATA BIST Loopback mode is selected, this field should be set as 0x0. */
        unsigned int  swapdfibyte_en : 2;  /* bit[30-31]: Swap DFI Byte Lane
                                                          00: No Swap
                                                          01: Swap */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_MISC_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_cfg_wl_START          (0)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_cfg_wl_END            (4)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_cfg_rl_START          (7)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_cfg_rl_END            (11)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_scramb_en_START       (16)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_scramb_en_END         (16)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_addr_toggle_START     (17)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_addr_toggle_END       (17)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_addr_delay_START      (18)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_addr_delay_END        (18)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_cfg_dlyupd_START      (19)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_cfg_dlyupd_END        (19)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swap_en_START         (20)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swap_en_END           (22)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swapaddr_en_START     (23)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swapaddr_en_END       (24)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swapdm_en_START       (25)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swapdm_en_END         (26)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swapwl_en_START       (27)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swapwl_en_END         (29)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swapdfibyte_en_START  (30)
#define SOC_MDDRC_PACK_DDRC_PACK_MISC_swapdfibyte_en_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_UNION
 结构说明  : DDRC_PACK_RNK2RNK 寄存器结构定义。地址偏移量:0x0074，初值:0x00000000，宽度:32
 寄存器说明: Rank to rank delay control register
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trnkwtw  : 5;  /* bit[0-4]  : Delay from write to write command targeted to different ranks */
        unsigned int  trnkrtr  : 5;  /* bit[5-9]  : Delay from read to read command targeted to different ranks */
        unsigned int  trnkrtw  : 5;  /* bit[10-14]: Delay from read to write command targeted to different ranks */
        unsigned int  trnkwtr  : 5;  /* bit[15-19]: Delay from write to read command targeted to different ranks */
        unsigned int  reserved : 12; /* bit[20-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_trnkwtw_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_trnkwtw_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_trnkrtr_START   (5)
#define SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_trnkrtr_END     (9)
#define SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_trnkrtw_START   (10)
#define SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_trnkrtw_END     (14)
#define SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_trnkwtr_START   (15)
#define SOC_MDDRC_PACK_DDRC_PACK_RNK2RNK_trnkwtr_END     (19)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_UNION
 结构说明  : DDRC_PACK_PHYCTRL0 寄存器结构定义。地址偏移量:0x0078，初值:0x00003980，宽度:32
 寄存器说明: PHY control registers 
            Register in this field are connected to PHY interface directly
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0 : 2;  /* bit[0-1]  : reserved */
        unsigned int  sel_pos_rx : 1;  /* bit[2]    : PHY output trigger edge selection.
                                                      1: rising edge
                                                      0: falling edge */
        unsigned int  lp_ck_sel  : 2;  /* bit[3-4]  : set to 0 for Balong */
        unsigned int  reserved_1 : 2;  /* bit[5-6]  : reserved */
        unsigned int  cmdoen     : 1;  /* bit[7]    : oe control for IO,Must Enable for DRAM Access  */
        unsigned int  ckoen      : 3;  /* bit[8-10] : oe control for IO,Must Enable for DRAM Access  */
        unsigned int  ckeoen     : 1;  /* bit[11]   : oe control for IO,Must Enable for DRAM Access  */
        unsigned int  csoen      : 1;  /* bit[12]   : oe control for IO,Must Enable for DRAM Access  */
        unsigned int  resetoen   : 1;  /* bit[13]   : oe control for IO,Must Enable for DRAM Access . Not Used in LPDDR23 */
        unsigned int  reserved_2 : 18; /* bit[14-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_sel_pos_rx_START  (2)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_sel_pos_rx_END    (2)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_lp_ck_sel_START   (3)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_lp_ck_sel_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_cmdoen_START      (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_cmdoen_END        (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_ckoen_START       (8)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_ckoen_END         (10)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_ckeoen_START      (11)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_ckeoen_END        (11)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_csoen_START       (12)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_csoen_END         (12)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_resetoen_START    (13)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL0_resetoen_END      (13)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYDBG_UNION
 结构说明  : DDRC_PACK_PHYDBG 寄存器结构定义。地址偏移量:0x007C，初值:0x00000000，宽度:32
 寄存器说明: PHY debug registers
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dqs_gated_error  : 8;  /* bit[0-7]  : DQS gated error flag
                                                            This error flag can only be cleared by issuing PHY reset */
        unsigned int  reserved_0       : 8;  /* bit[8-15] : reserved */
        unsigned int  dqsb_gated_error : 8;  /* bit[16-23]: DQSN gated error flag
                                                            This error flag can only be cleared by issuing PHY reset */
        unsigned int  reserved_1       : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYDBG_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYDBG_dqs_gated_error_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYDBG_dqs_gated_error_END     (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYDBG_dqsb_gated_error_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYDBG_dqsb_gated_error_END    (23)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_UNION
 结构说明  : DDRC_PACK_RETCTRL0 寄存器结构定义。地址偏移量:0x0080，初值:0x00012400，宽度:32
 寄存器说明: ReTrain Control
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phyupd_resp  : 13; /* bit[0-12] : PHY update response latency
                                                        When PHY asserts dfi_phyupd_req to MC, MC should assert dfi_phyupd_ack no later than number of cycles set by the response latency */
        unsigned int  retrain_en   : 1;  /* bit[13]   : PHY Retraining Enable.
                                                        This field is used to control if PHY will be enabled to request memory controller for retraining. */
        unsigned int  gtrten       : 1;  /* bit[14]   : Gate Retraining Active.
                                                        This field controls if the gate retraining should be executed during retraining on-going. This filed should be asserted only after the initial gate training done. */
        unsigned int  reserved     : 1;  /* bit[15]   : reserved */
        unsigned int  retrain_thrd : 16; /* bit[16-31]: Retraining threshold
                                                        PHY counts number of AREF issued by MC and when the number is equal to the threshold value, the retraining operation is triggerd to be started. Only a non zero value is valid. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_phyupd_resp_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_phyupd_resp_END     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_retrain_en_START    (13)
#define SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_retrain_en_END      (13)
#define SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_gtrten_START        (14)
#define SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_gtrten_END          (14)
#define SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_retrain_thrd_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_RETCTRL0_retrain_thrd_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DMSEL_UNION
 结构说明  : DDRC_PACK_DMSEL 寄存器结构定义。地址偏移量:0x0084，初值:0x0000000A，宽度:32
 寄存器说明: DM Swap Selection
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dxctl_dmswap_sel : 4;  /* bit[0-3] : [3:2] DM1 Swap Selection
                                                           [1:0] DM0 Swap Selection */
        unsigned int  reserved         : 28; /* bit[4-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DMSEL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DMSEL_dxctl_dmswap_sel_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DMSEL_dxctl_dmswap_sel_END    (3)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_IOCTL_UNION
 结构说明  : DDRC_PACK_IOCTL 寄存器结构定义。地址偏移量:0x0088，初值:0x4BFFF801，宽度:32
 寄存器说明: IO control register
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  diff_dqs_en : 1;  /* bit[0]    : Differential DQS enable ,set to 1 for Balong */
        unsigned int  dxiopldn    : 1;  /* bit[1]    : set to 0 for Balong */
        unsigned int  aciopldn    : 1;  /* bit[2]    : set to 0 for Balong */
        unsigned int  reserved_0  : 3;  /* bit[3-5]  : reserved */
        unsigned int  reserved_1  : 2;  /* bit[6-7]  : reserved */
        unsigned int  reserved_2  : 1;  /* bit[8]    : reserved */
        unsigned int  reserved_3  : 1;  /* bit[9]    : reserved */
        unsigned int  reserved_4  : 1;  /* bit[10]   : reserved */
        unsigned int  ronsel      : 3;  /* bit[11-13]: set to 7 for Balong */
        unsigned int  ronsel_dqs  : 3;  /* bit[14-16]: set to 7 for Balong */
        unsigned int  ronsel_ac1t : 3;  /* bit[17-19]: set to 7 for Balong */
        unsigned int  ronsel_ac2t : 3;  /* bit[20-22]: set to 7 for Balong */
        unsigned int  ronsel_ck   : 3;  /* bit[23-25]: set to 7 for Balong */
        unsigned int  odtsel      : 3;  /* bit[26-28]: set to 2 for Balong */
        unsigned int  odtsel_dqs  : 3;  /* bit[29-31]: set to 2 for Balong */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_IOCTL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_diff_dqs_en_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_diff_dqs_en_END    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_dxiopldn_START     (1)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_dxiopldn_END       (1)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_aciopldn_START     (2)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_aciopldn_END       (2)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_START       (11)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_END         (13)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_dqs_START   (14)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_dqs_END     (16)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_ac1t_START  (17)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_ac1t_END    (19)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_ac2t_START  (20)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_ac2t_END    (22)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_ck_START    (23)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_ronsel_ck_END      (25)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_odtsel_START       (26)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_odtsel_END         (28)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_odtsel_dqs_START   (29)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL_odtsel_dqs_END     (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DQSSEL_UNION
 结构说明  : DDRC_PACK_DQSSEL 寄存器结构定义。地址偏移量:0x008C，初值:0xFF00FF00，宽度:32
 寄存器说明: Swap of DQ in PHY
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dqswap_sel : 32; /* bit[0-31]: DQ swap in PHY
                                                     Each bit decides the relative DQS with DQ
                                                     1: Relative to DQS1
                                                     0: Relative to DQS0  */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DQSSEL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DQSSEL_dqswap_sel_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DQSSEL_dqswap_sel_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_UNION
 结构说明  : DDRC_PACK_PHYCTRL1 寄存器结构定义。地址偏移量:0x0090，初值:0x00000000，宽度:32
 寄存器说明: AC PHY control registers 
            Register in this field are connected to PHY interface directly
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0     : 16; /* bit[0-15] : reserved */
        unsigned int  ac_pll_thrm    : 1;  /* bit[16]   : set to 0 for Balong */
        unsigned int  ac_pll_testpem : 1;  /* bit[17]   : set to 0 for Balong */
        unsigned int  ac_pll_testen  : 1;  /* bit[18]   : set to 0 for Balong */
        unsigned int  ac_pll_init    : 1;  /* bit[19]   : set to 0 for Balong */
        unsigned int  ac_pll_cpi     : 3;  /* bit[20-22]: set to 0 for Balong */
        unsigned int  ac_pll_sp      : 3;  /* bit[23-25]: set to 0 for Balong */
        unsigned int  ac_pll_lt      : 2;  /* bit[26-27]: set to 0 for Balong */
        unsigned int  ac_pll_calt    : 1;  /* bit[28]   : set to 0 for Balong */
        unsigned int  ac_pll_calm    : 1;  /* bit[29]   : set to 0 for Balong */
        unsigned int  ac_pll_cal     : 1;  /* bit[30]   : set to 0 for Balong */
        unsigned int  reserved_1     : 1;  /* bit[31]   : reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_thrm_START     (16)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_thrm_END       (16)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_testpem_START  (17)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_testpem_END    (17)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_testen_START   (18)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_testen_END     (18)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_init_START     (19)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_init_END       (19)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_cpi_START      (20)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_cpi_END        (22)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_sp_START       (23)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_sp_END         (25)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_lt_START       (26)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_lt_END         (27)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_calt_START     (28)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_calt_END       (28)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_calm_START     (29)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_calm_END       (29)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_cal_START      (30)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL1_ac_pll_cal_END        (30)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNCKCTRL_UNION
 结构说明  : DDRC_PACK_DXNCKCTRL 寄存器结构定义。地址偏移量:0x0094，初值:0x00000000，宽度:32
 寄存器说明: This register is for PLL phase select within the PHY. And should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dx_ck0p_mclk : 3;  /* bit[0-2] : set to 0 for Balong */
        unsigned int  dx_ckd2      : 3;  /* bit[3-5] : set to 0 for Balong */
        unsigned int  reserved     : 26; /* bit[6-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNCKCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCKCTRL_dx_ck0p_mclk_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCKCTRL_dx_ck0p_mclk_END    (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCKCTRL_dx_ckd2_START       (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCKCTRL_dx_ckd2_END         (5)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_AC_UNION
 结构说明  : DDRC_PACK_PHYPLLCTRL_AC 寄存器结构定义。地址偏移量:0x0098，初值:0x00000000，宽度:32
 寄存器说明: PHY PLL control registers 
            Register in this field are connected to PHY interface directly
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ac_pll_test : 8;  /* bit[0-7] : AC PLL test mode
                                                      1: Test mode
                                                      0: Normal mode */
        unsigned int  reserved    : 23; /* bit[8-30]: reserved */
        unsigned int  ac_pll_lock : 1;  /* bit[31]  : AC PLL LOCK signal
                                                      1: Locked
                                                      0: Not Locked */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_AC_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_AC_ac_pll_test_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_AC_ac_pll_test_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_AC_ac_pll_lock_START  (31)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_AC_ac_pll_lock_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_DX_UNION
 结构说明  : DDRC_PACK_PHYPLLCTRL_DX 寄存器结构定义。地址偏移量:0x009C，初值:0x00000000，宽度:32
 寄存器说明: PHY PLL control registers 
            Register in this field are connected to PHY interface directly
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dx_pll_test : 16; /* bit[0-15] : PLL Test for 16-Bit Data Block
                                                       7:0 - 16-Bit Block 0
                                                       15:8 - 16-Bit Block 1 */
        unsigned int  reserved    : 12; /* bit[16-27]: reserved */
        unsigned int  dx_pll_lock : 4;  /* bit[28-31]: ND: Number of DATA PHY Block
                                                       DX PLL LOCK signals for each bytelane
                                                       1: Locked
                                                       0: Not Locked
                                                       (In Balong, only bit0 is valid) */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_DX_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_DX_dx_pll_test_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_DX_dx_pll_test_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_DX_dx_pll_lock_START  (28)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYPLLCTRL_DX_dx_pll_lock_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_UNION
 结构说明  : DDRC_PACK_SWTMODE 寄存器结构定义。地址偏移量:0x00A0，初值:0x00000000，宽度:32
 寄存器说明: This register is for setting S/W training mode
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_wlmode     : 1;  /* bit[0]   : Set this bit to 1 when performing S/W write leveling training */
        unsigned int  sw_gtmode     : 1;  /* bit[1]   : Set this bit to 1 when performing S/W trianing gate training or GDS training */
        unsigned int  reserved      : 29; /* bit[2-30]: reserved */
        unsigned int  wl_wdqs_gated : 1;  /* bit[31]  : Used to disable Write Leveling wdqs strobe when tuning the related PH/BDL to prevent glitch.
                                                        1: gated (disable wdqs strobe)
                                                        0: not gated */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_sw_wlmode_START      (0)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_sw_wlmode_END        (0)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_sw_gtmode_START      (1)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_sw_gtmode_END        (1)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_wl_wdqs_gated_START  (31)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTMODE_wl_wdqs_gated_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_SWTWLDQS_UNION
 结构说明  : DDRC_PACK_SWTWLDQS 寄存器结构定义。地址偏移量:0x00A4，初值:0x00000000，宽度:32
 寄存器说明: This register is for issuing write DQS in S/W write leveling training
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_wl_dqs : 1;  /* bit[0]   : Write 1 to this bit once will issue one DQS pulse from PHY to DRAM */
        unsigned int  reserved  : 31; /* bit[1-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_SWTWLDQS_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_SWTWLDQS_sw_wl_dqs_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTWLDQS_sw_wl_dqs_END    (0)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_UNION
 结构说明  : DDRC_PACK_SWTRLT 寄存器结构定义。地址偏移量:0x00A8，初值:0x00000000，宽度:32
 寄存器说明: S/W training result
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wl_dq_result : 8;  /* bit[0-7]  : Each bit is corresponded to one byte lane */
        unsigned int  gt_result    : 8;  /* bit[8-15] : Each bit is corresponded to one byte lane */
        unsigned int  gds_result   : 8;  /* bit[16-23]: Each bit is corresponded to one byte lane */
        unsigned int  reserved     : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_wl_dq_result_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_wl_dq_result_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_gt_result_START     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_gt_result_END       (15)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_gds_result_START    (16)
#define SOC_MDDRC_PACK_DDRC_PACK_SWTRLT_gds_result_END      (23)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_UNION
 结构说明  : DDRC_PACK_PHYCTRL2 寄存器结构定义。地址偏移量:0x00AC，初值:0x0000000B，宽度:32
 寄存器说明: PHY control registers 
            Register in this field are connected to PHY interface directly
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_dqsgerror    : 1;  /* bit[0]   : Control DATA16BIT_blk.BUFRESETCONTN_gated_DQSGERROR */
        unsigned int  phy_gatetdc      : 1;  /* bit[1]   : Control DATA16BIT_blk.BUFRESETCONTN_gated_TDC */
        unsigned int  phy_odten_gated  : 1;  /* bit[2]   : Control DATA16BIT_blk.reg_ODTEN_Gated */
        unsigned int  phy_dummypad_use : 1;  /* bit[3]   : Control DATA16BIT_blk.reg_dummypad_use */
        unsigned int  phy_dyn_pupden   : 1;  /* bit[4]   : Control DATA16BIT_blk.reg_dynamic_PUPDEN_16b */
        unsigned int  reserved         : 27; /* bit[5-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_dqsgerror_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_dqsgerror_END       (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_gatetdc_START       (1)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_gatetdc_END         (1)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_odten_gated_START   (2)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_odten_gated_END     (2)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_dummypad_use_START  (3)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_dummypad_use_END    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_dyn_pupden_START    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYCTRL2_phy_dyn_pupden_END      (4)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_UNION
 结构说明  : DDRC_PACK_IOCTL1 寄存器结构定义。地址偏移量:0x00B0，初值:0x00000000，宽度:32
 寄存器说明: PHY control registers 
            Register in this field are connected to PHY interface directly
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ioctl_popmode    : 6;  /* bit[0-5]  : ioctl for popmode */
        unsigned int  ioctl_genvref_ie : 6;  /* bit[6-11] : ioctl for genvref_ie */
        unsigned int  ioctl_genvref_pd : 6;  /* bit[12-17]: ioctl for genvref_pd */
        unsigned int  reserved_0       : 6;  /* bit[18-23]: reserved */
        unsigned int  ioctl_squeach_pd : 4;  /* bit[24-27]: ioctl for squeach_pd */
        unsigned int  reserved_1       : 4;  /* bit[28-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_ioctl_popmode_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_ioctl_popmode_END       (5)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_ioctl_genvref_ie_START  (6)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_ioctl_genvref_ie_END    (11)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_ioctl_genvref_pd_START  (12)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_ioctl_genvref_pd_END    (17)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_ioctl_squeach_pd_START  (24)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL1_ioctl_squeach_pd_END    (27)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_UNION
 结构说明  : DDRC_PACK_IOCTL2 寄存器结构定义。地址偏移量:0x00B4，初值:0x00000000，宽度:32
 寄存器说明: PHY control registers 
            Register in this field are connected to PHY interface directly
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ioctl_genvref_rangel_0 : 6;  /* bit[0-5]  : ioctl for genvref_rangel_0 */
        unsigned int  ioctl_genvref_rangel_1 : 6;  /* bit[6-11] : ioctl for genvref_rangel_1 */
        unsigned int  ioctl_genvref_refsel_0 : 6;  /* bit[12-17]: ioctl for genvref_refsel_0 */
        unsigned int  ioctl_genvref_refsel_1 : 6;  /* bit[18-23]: ioctl for genvref_refsel_1 */
        unsigned int  ioctl_genvref_refsel_2 : 6;  /* bit[24-29]: ioctl for genvref_refsel_2 */
        unsigned int  reserved               : 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_rangel_0_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_rangel_0_END    (5)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_rangel_1_START  (6)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_rangel_1_END    (11)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_refsel_0_START  (12)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_refsel_0_END    (17)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_refsel_1_START  (18)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_refsel_1_END    (23)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_refsel_2_START  (24)
#define SOC_MDDRC_PACK_DDRC_PACK_IOCTL2_ioctl_genvref_refsel_2_END    (29)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_BISTTIMER_UNION
 结构说明  : DDRC_PACK_BISTTIMER 寄存器结构定义。地址偏移量:0x00B8，初值:0x00000000，宽度:32
 寄存器说明: This register control the period of code update during BIST running.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bstupdt_timer : 16; /* bit[0-15] : BIST Update Code Timer.
                                                         This register control the period of code update. If this register is programmed to non-zero, the BIST will temporarily stopped, and update the internal delay code to PHY while timer reached the setting value. The bist will continue if the code update complete.
                                                         If this register is programmed to zero, the update code mechanism will be disabled. */
        unsigned int  reserved      : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_BISTTIMER_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_BISTTIMER_bstupdt_timer_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_BISTTIMER_bstupdt_timer_END    (15)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_UNION
 结构说明  : DDRC_PACK_TRAINCTRL1 寄存器结构定义。地址偏移量:0x00D0，初值:0xF036F026，宽度:32
 寄存器说明: This register control the data training.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdet_samp_num   : 4;  /* bit[0-3]  : Write DET sample number control. The multiple sample is used to overcome the meta situations.
                                                           Valid value: 1~15 */
        unsigned int  wdet_rb_backtap : 4;  /* bit[4-7]  : Write DET Right Boundary back tap. This field is used to offset (-) the wdqphase before do the Right Boundary Search to overcome meta-sable zone. */
        unsigned int  reserved_0      : 4;  /* bit[8-11] : reserved */
        unsigned int  wrdet_av_en     : 1;  /* bit[12]   : Write DET average stage enable */
        unsigned int  wdet_rbs_en     : 1;  /* bit[13]   : Write DET right boundary search enable. */
        unsigned int  wdet_ds_en      : 1;  /* bit[14]   : Write DET deskew enable. */
        unsigned int  wdet_lbs_en     : 1;  /* bit[15]   : Write DET left boundary search enable. */
        unsigned int  rdet_samp_num   : 4;  /* bit[16-19]: Read DET sample number control. The multiple sample is used to overcome the meta situations.
                                                           Valid value: 1~15 */
        unsigned int  rdet_rb_backtap : 7;  /* bit[20-26]: Read DET Right Boundary back tap. This field is used to offset (+) the rdqsbdl before do the Right Boundary Search to overcome meta-sable zone. */
        unsigned int  reserved_1      : 1;  /* bit[27]   : reserved */
        unsigned int  rdet_av_en      : 1;  /* bit[28]   : Read DET average stage enable */
        unsigned int  rdet_rbs_en     : 1;  /* bit[29]   : Read DET right boundary search enable. */
        unsigned int  rdet_ds_en      : 1;  /* bit[30]   : Read DET deskew enable. */
        unsigned int  rdet_lbs_en     : 1;  /* bit[31]   : Read DET left boundary search enable. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_samp_num_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_samp_num_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_rb_backtap_START  (4)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_rb_backtap_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wrdet_av_en_START      (12)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wrdet_av_en_END        (12)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_rbs_en_START      (13)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_rbs_en_END        (13)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_ds_en_START       (14)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_ds_en_END         (14)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_lbs_en_START      (15)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_wdet_lbs_en_END        (15)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_samp_num_START    (16)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_samp_num_END      (19)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_rb_backtap_START  (20)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_rb_backtap_END    (26)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_av_en_START       (28)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_av_en_END         (28)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_rbs_en_START      (29)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_rbs_en_END        (29)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_ds_en_START       (30)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_ds_en_END         (30)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_lbs_en_START      (31)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL1_rdet_lbs_en_END        (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_UNION
 结构说明  : DDRC_PACK_TRAINCTRL2 寄存器结构定义。地址偏移量:0x00D4，初值:0x0000CC55，宽度:32
 寄存器说明: This register control the data training.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mrr_pat_A       : 8;  /* bit[0-7]  : MRR32 pattern.This filed should follow the LPDDR2/3 spec. */
        unsigned int  mrr_pat_B       : 8;  /* bit[8-15] : MRR40 pattern. This filed should follow the LPDDR2/3 spec. */
        unsigned int  mrr_pat_mask    : 8;  /* bit[16-23]: MRR pattern mask.
                                                           1: masked (does not compare data)
                                                           0: not masked */
        unsigned int  gds_automrg_en  : 1;  /* bit[24]   : GDS train/re-train auto tuning enable.
                                                           1'b1: enable gds auto margin tuning (default)
                                                           1'b0: disable gds auto margin tuning (will increase gds when GDS train/retrain is done) */
        unsigned int  reserved        : 2;  /* bit[25-26]: reserved */
        unsigned int  mrr_bl4         : 1;  /* bit[27]   : MRR burst 4 indicator. If MRR was used in the Read DET, pack needs to know if MRR burst is 4 (LPDDR2) or 8 (LPDDR3).
                                                           1: MRR32/40 burst length = 4
                                                           0: MRR32/40 burst length = 8 (default) */
        unsigned int  mrr_pat_order   : 3;  /* bit[28-30]: MRR pattern order. If MRR was used in the Read DET, pack will issue 3 MRR commands. The order is indicated by bit2->bit1->bit0.
                                                           1: use MRR40
                                                           0: use MRR32 */
        unsigned int  rdet_method_sel : 1;  /* bit[31]   : Read DET method selection. 
                                                           1: use MRR32/40 (only for LPDDR3, not for LPDDR2 and others)
                                                           0: use Write-then-Read */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_pat_A_START        (0)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_pat_A_END          (7)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_pat_B_START        (8)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_pat_B_END          (15)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_pat_mask_START     (16)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_pat_mask_END       (23)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_gds_automrg_en_START   (24)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_gds_automrg_en_END     (24)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_bl4_START          (27)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_bl4_END            (27)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_pat_order_START    (28)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_mrr_pat_order_END      (30)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_rdet_method_sel_START  (31)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL2_rdet_method_sel_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_REGBANKCTRL_UNION
 结构说明  : DDRC_PACK_REGBANKCTRL 寄存器结构定义。地址偏移量:0x00D8，初值:0x00000000，宽度:32
 寄存器说明: This register control the register bank.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wr_sel   : 1;  /* bit[0]   : register bank selection that related to DFS
                                                   0: select bank 0 while APB read/write
                                                   1: select bank 1 while APB read/write */
        unsigned int  op_sel   : 1;  /* bit[1]   : register bank selection that related to DFS
                                                   0: select bank 0 for PHY operating
                                                   1: select bank 1 for PHY operating */
        unsigned int  reserved : 30; /* bit[2-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_REGBANKCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_REGBANKCTRL_wr_sel_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_REGBANKCTRL_wr_sel_END      (0)
#define SOC_MDDRC_PACK_DDRC_PACK_REGBANKCTRL_op_sel_START    (1)
#define SOC_MDDRC_PACK_DDRC_PACK_REGBANKCTRL_op_sel_END      (1)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_UNION
 结构说明  : DDRC_PACK_TRAINCTRL3 寄存器结构定义。地址偏移量:0x00DC，初值:0x80020300，宽度:32
 寄存器说明: This register control the data training.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rpat_cnt_num      : 4;  /* bit[0-3]  : RDET pattern length control. This field is used to set the pattern length. The same RDET pattern will be extended as the following settings (field value+1):
                                                             0x0: pattern length=1
                                                             0x1: pattern length=2
                                                             …
                                                             0xF: pattern length=16 */
        unsigned int  reserved_0        : 4;  /* bit[4-7]  : reserved */
        unsigned int  rpat_cnt_num_open : 4;  /* bit[8-11] : RDET pattern length control for eyeopen. This field is used to set the pattern length for eyeopen. The same RDET pattern will be extended as the following settings (field value+1):
                                                             0x0: pattern length=1
                                                             0x1: pattern length=2
                                                             …
                                                             0xF: pattern length=16 */
        unsigned int  reserved_1        : 4;  /* bit[12-15]: reserved */
        unsigned int  gdsrt_backtap     : 3;  /* bit[16-18]: GDS re-train back tap. This field is used to control the number of back tap when GDS re-train.
                                                             The valid range is 1~7.
                                                             The recommended range is 1~2. */
        unsigned int  reserved_2        : 12; /* bit[19-30]: reserved */
        unsigned int  exd_gtres_en      : 1;  /* bit[31]   : Extended gate result enable. When set as HIGH, the pack will use additional information (EN_DQSx_regread) to help extend the gate result in the &quot;fine tune stage of gate train &quot;and &quot;gate re-train&quot;.
                                                             0: disable
                                                             1: enable (default) */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_rpat_cnt_num_START       (0)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_rpat_cnt_num_END         (3)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_rpat_cnt_num_open_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_rpat_cnt_num_open_END    (11)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_gdsrt_backtap_START      (16)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_gdsrt_backtap_END        (18)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_exd_gtres_en_START       (31)
#define SOC_MDDRC_PACK_DDRC_PACK_TRAINCTRL3_exd_gtres_en_END         (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_UNION
 结构说明  : DDRC_PACK_ACBISTCTRL0 寄存器结构定义。地址偏移量:0x100，初值:0x40000000，宽度:32
 寄存器说明: This register control the comparison while BIST activated.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr_cmp_mask : 16; /* bit[0-15] : Memory Address Loopback Compare Mask.
                                                         This field is used to control if the comparison of the loopback data on address bits are disabled. 
                                                         0: Not compare 
                                                         1: Compare */
        unsigned int  ba_cmp_mask   : 3;  /* bit[16-18]: Bank Address Loopback Compare Mask.
                                                         This field is used to control if the comparison of the loopback data on bank address bits are disabled.
                                                         0: Not compare 
                                                         1: Compare */
        unsigned int  we_cmp_mask   : 1;  /* bit[19]   : Write Enable Loopback Compare Mask.
                                                         This field is used to control if the comparison of the loopback data on WE are disabled.
                                                         0: Not compare 
                                                         1: Compare */
        unsigned int  cas_cmp_mask  : 1;  /* bit[20]   : CAS Loopback Compare Mask.
                                                         This field is used to control if the comparison of the loopback data on CAS are disabled.
                                                         0: Not compare 
                                                         1: Compare */
        unsigned int  ras_cmp_mask  : 1;  /* bit[21]   : RAS Loopback Compare Mask.
                                                         This field is used to control if the comparison of the loopback data on RAS are disabled.
                                                         0: Not compare 
                                                         1: Compare */
        unsigned int  cs_cmp_mask   : 4;  /* bit[22-25]: CS Loopback Compare Mask.
                                                         This field is used to control if the comparison of the loopback data on CS bits are disabled. 
                                                         0: Not compare 
                                                         1: Compare */
        unsigned int  rst_cmp_mask  : 1;  /* bit[26]   : RST Loopback Compare Mask
                                                         This field is used to control if the comparison of the loopback data on RST bit are disabled. 
                                                         0: Not compare 
                                                         1: Compare */
        unsigned int  acbist_en     : 1;  /* bit[27]   : Bist Enable.
                                                         This signal specify if the AC block is included for loopback test while bist_op is set to BIST Enable. */
        unsigned int  aclpbk_tg     : 1;  /* bit[28]   : AC Loopback toggle control. The user shold not modify this bit. It is related to the Analog PHY design.
                                                         1:40PHY
                                                         0:28LPP PHY (default) */
        unsigned int  ac_rdffsel    : 3;  /* bit[29-31]: AC Loopback Read Latch Select */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_addr_cmp_mask_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_addr_cmp_mask_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_ba_cmp_mask_START    (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_ba_cmp_mask_END      (18)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_we_cmp_mask_START    (19)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_we_cmp_mask_END      (19)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_cas_cmp_mask_START   (20)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_cas_cmp_mask_END     (20)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_ras_cmp_mask_START   (21)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_ras_cmp_mask_END     (21)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_cs_cmp_mask_START    (22)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_cs_cmp_mask_END      (25)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_rst_cmp_mask_START   (26)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_rst_cmp_mask_END     (26)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_acbist_en_START      (27)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_acbist_en_END        (27)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_aclpbk_tg_START      (28)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_aclpbk_tg_END        (28)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_ac_rdffsel_START     (29)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL0_ac_rdffsel_END       (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_UNION
 结构说明  : DDRC_PACK_ACBISTCTRL1 寄存器结构定义。地址偏移量:0x104，初值:0x00000000，宽度:32
 寄存器说明: This register control the comparison while BIST activated.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  odt_cmp_mask : 4;  /* bit[0-3]  : ODT Loopback Compare Mask.
                                                        This field is used to control if the comparison of the loopback data on ODT bits are disabled. 
                                                        0: Not compare 
                                                        1: Compare */
        unsigned int  cke_cmp_mask : 4;  /* bit[4-7]  : CKE Loopback Compare Mask.
                                                        This field is used to control if the comparison of the loopback data on CKE bits are disabled. 
                                                        0: Not compare 
                                                        1: Compare */
        unsigned int  ac_lfsr_seed : 8;  /* bit[8-15] : LFSR Seed Value.
                                                        This field is uesed to specify the initial seed value of the LFSR generator to this byte lane. */
        unsigned int  reserved     : 16; /* bit[16-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_odt_cmp_mask_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_odt_cmp_mask_END    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_cke_cmp_mask_START  (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_cke_cmp_mask_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_ac_lfsr_seed_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTCTRL1_ac_lfsr_seed_END    (15)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_UNION
 结构说明  : DDRC_PACK_ACBISTSTS0 寄存器结构定义。地址偏移量:0x108，初值:0x00000000，宽度:32
 寄存器说明: This register shows the results and status of the BIST test. Please note that this register is read-only, and can only be reset by bist_op setting to BIST Reset.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr_bist_err : 16; /* bit[0-15] : BIST Error on Address Bus. */
        unsigned int  ba_bist_err   : 3;  /* bit[16-18]: BIST Error on Bank Address. */
        unsigned int  we_bist_err   : 1;  /* bit[19]   : BIST Error on Write Enable. */
        unsigned int  cas_bist_err  : 1;  /* bit[20]   : BIST Error on CAS. */
        unsigned int  ras_bist_err  : 1;  /* bit[21]   : BIST Error on RAS. */
        unsigned int  cs_bist_err   : 4;  /* bit[22-25]: BIST Error on CS. */
        unsigned int  rst_bist_err  : 1;  /* bit[26]   : BIST Error on RST. */
        unsigned int  reserved      : 5;  /* bit[27-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_addr_bist_err_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_addr_bist_err_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_ba_bist_err_START    (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_ba_bist_err_END      (18)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_we_bist_err_START    (19)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_we_bist_err_END      (19)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_cas_bist_err_START   (20)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_cas_bist_err_END     (20)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_ras_bist_err_START   (21)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_ras_bist_err_END     (21)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_cs_bist_err_START    (22)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_cs_bist_err_END      (25)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_rst_bist_err_START   (26)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS0_rst_bist_err_END     (26)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS1_UNION
 结构说明  : DDRC_PACK_ACBISTSTS1 寄存器结构定义。地址偏移量:0x10C，初值:0x00000000，宽度:32
 寄存器说明: This register shows the results and status of the BIST test. Please note that this register is read-only, and can only be reset by bist_op setting to BIST Reset.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  odt_bist_err : 4;  /* bit[0-3] : BIST Error on ODT.  */
        unsigned int  cke_bist_err : 4;  /* bit[4-7] : BIST Error on CKE. */
        unsigned int  reserved     : 24; /* bit[8-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS1_odt_bist_err_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS1_odt_bist_err_END    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS1_cke_bist_err_START  (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACBISTSTS1_cke_bist_err_END    (7)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL0_UNION
 结构说明  : DDRC_PACK_ACCMDBDL0 寄存器结构定义。地址偏移量:0x120，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  odt0_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ODT0 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  odt1_bdl : 5;  /* bit[16-20]: Bit delay line setting of ODT1 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL0_odt0_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL0_odt0_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL0_odt1_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL0_odt1_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL1_UNION
 结构说明  : DDRC_PACK_ACCMDBDL1 寄存器结构定义。地址偏移量:0x124，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  odt2_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ODT2 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  odt3_bdl : 5;  /* bit[16-20]: Bit delay line setting of ODT3 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL1_odt2_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL1_odt2_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL1_odt3_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL1_odt3_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL2_UNION
 结构说明  : DDRC_PACK_ACCMDBDL2 寄存器结构定义。地址偏移量:0x128，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cs0_bdl  : 5;  /* bit[0-4]  : Bit delay line setting of CS0 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  cs1_bdl  : 5;  /* bit[16-20]: Bit delay line setting of CS1 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL2_cs0_bdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL2_cs0_bdl_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL2_cs1_bdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL2_cs1_bdl_END     (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL3_UNION
 结构说明  : DDRC_PACK_ACCMDBDL3 寄存器结构定义。地址偏移量:0x12C，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cs2_bdl  : 5;  /* bit[0-4]  : Bit delay line setting of CS2 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  cs3_bdl  : 5;  /* bit[16-20]: Bit delay line setting of CS3 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL3_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL3_cs2_bdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL3_cs2_bdl_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL3_cs3_bdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL3_cs3_bdl_END     (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL4_UNION
 结构说明  : DDRC_PACK_ACCMDBDL4 寄存器结构定义。地址偏移量:0x130，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cke0_bdl : 5;  /* bit[0-4]  : Bit delay line setting of CKE0 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  cke1_bdl : 5;  /* bit[16-20]: Bit delay line setting of CKE1 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL4_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL4_cke0_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL4_cke0_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL4_cke1_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL4_cke1_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL5_UNION
 结构说明  : DDRC_PACK_ACCMDBDL5 寄存器结构定义。地址偏移量:0x134，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cke2_bdl : 5;  /* bit[0-4]  : Bit delay line setting of CKE2 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  cke3_bdl : 5;  /* bit[16-20]: Bit delay line setting of CKE3 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL5_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL5_cke2_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL5_cke2_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL5_cke3_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL5_cke3_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL6_UNION
 结构说明  : DDRC_PACK_ACCMDBDL6 寄存器结构定义。地址偏移量:0x138，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  we_bdl   : 5;  /* bit[0-4]  : Bit delay line setting of WE */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  cas_bdl  : 5;  /* bit[16-20]: Bit delay line setting of CAS */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL6_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL6_we_bdl_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL6_we_bdl_END      (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL6_cas_bdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL6_cas_bdl_END     (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL7_UNION
 结构说明  : DDRC_PACK_ACCMDBDL7 寄存器结构定义。地址偏移量:0x13C，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ras_bdl    : 5;  /* bit[0-4]  : Bit delay line setting of RAS */
        unsigned int  reserved_0 : 11; /* bit[5-15] : reserved */
        unsigned int  resetn_bdl : 5;  /* bit[16-20]: Bit delay line setting of RESETN */
        unsigned int  reserved_1 : 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL7_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL7_ras_bdl_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL7_ras_bdl_END       (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL7_resetn_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL7_resetn_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL0_UNION
 结构说明  : DDRC_PACK_ACADDRBDL0 寄存器结构定义。地址偏移量:0x140，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr0_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ADDR0 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  addr1_bdl : 5;  /* bit[16-20]: Bit delay line setting of ADDR1 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL0_addr0_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL0_addr0_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL0_addr1_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL0_addr1_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL1_UNION
 结构说明  : DDRC_PACK_ACADDRBDL1 寄存器结构定义。地址偏移量:0x144，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr2_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ADDR2 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  addr3_bdl : 5;  /* bit[16-20]: Bit delay line setting of ADDR3 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL1_addr2_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL1_addr2_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL1_addr3_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL1_addr3_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL2_UNION
 结构说明  : DDRC_PACK_ACADDRBDL2 寄存器结构定义。地址偏移量:0x148，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr4_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ADDR4 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  addr5_bdl : 5;  /* bit[16-20]: Bit delay line setting of ADDR5 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL2_addr4_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL2_addr4_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL2_addr5_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL2_addr5_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL3_UNION
 结构说明  : DDRC_PACK_ACADDRBDL3 寄存器结构定义。地址偏移量:0x14C，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr6_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ADDR6 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  addr7_bdl : 5;  /* bit[16-20]: Bit delay line setting of ADDR7 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL3_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL3_addr6_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL3_addr6_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL3_addr7_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL3_addr7_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL4_UNION
 结构说明  : DDRC_PACK_ACADDRBDL4 寄存器结构定义。地址偏移量:0x150，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr8_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ADDR8 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  addr9_bdl : 5;  /* bit[16-20]: Bit delay line setting of ADDR9 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL4_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL4_addr8_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL4_addr8_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL4_addr9_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL4_addr9_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL5_UNION
 结构说明  : DDRC_PACK_ACADDRBDL5 寄存器结构定义。地址偏移量:0x154，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr10_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ADDR10 */
        unsigned int  reserved_0 : 11; /* bit[5-15] : reserved */
        unsigned int  addr11_bdl : 5;  /* bit[16-20]: Bit delay line setting of ADDR11 */
        unsigned int  reserved_1 : 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL5_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL5_addr10_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL5_addr10_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL5_addr11_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL5_addr11_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL6_UNION
 结构说明  : DDRC_PACK_ACADDRBDL6 寄存器结构定义。地址偏移量:0x158，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr12_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ADDR12 */
        unsigned int  reserved_0 : 11; /* bit[5-15] : reserved */
        unsigned int  addr13_bdl : 5;  /* bit[16-20]: Bit delay line setting of ADDR13 */
        unsigned int  reserved_1 : 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL6_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL6_addr12_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL6_addr12_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL6_addr13_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL6_addr13_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL7_UNION
 结构说明  : DDRC_PACK_ACADDRBDL7 寄存器结构定义。地址偏移量:0x15C，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr14_bdl : 5;  /* bit[0-4]  : Bit delay line setting of ADDR14 */
        unsigned int  reserved_0 : 11; /* bit[5-15] : reserved */
        unsigned int  addr15_bdl : 5;  /* bit[16-20]: Bit delay line setting of ADDR15 */
        unsigned int  reserved_1 : 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL7_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL7_addr14_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL7_addr14_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL7_addr15_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL7_addr15_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL8_UNION
 结构说明  : DDRC_PACK_ACADDRBDL8 寄存器结构定义。地址偏移量:0x160，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ba0_bdl  : 5;  /* bit[0-4]  : Bit delay line setting of BA0 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  ba1_bdl  : 5;  /* bit[16-20]: Bit delay line setting of BA1 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL8_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL8_ba0_bdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL8_ba0_bdl_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL8_ba1_bdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL8_ba1_bdl_END     (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL9_UNION
 结构说明  : DDRC_PACK_ACADDRBDL9 寄存器结构定义。地址偏移量:0x164，初值:0x00000000，宽度:32
 寄存器说明: AC address bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ba2_bdl  : 5;  /* bit[0-4] : Bit delay line setting of BA2 */
        unsigned int  reserved : 27; /* bit[5-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL9_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL9_ba2_bdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACADDRBDL9_ba2_bdl_END     (4)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_UNION
 结构说明  : DDRC_PACK_ACCLKBDL 寄存器结构定义。地址偏移量:0x168，初值:0x00000000，宽度:32
 寄存器说明: AC clock bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dramclk0_bdl : 5;  /* bit[0-4]  : Bit delay line setting of CK0 */
        unsigned int  reserved_0   : 3;  /* bit[5-7]  : reserved */
        unsigned int  dramclk1_bdl : 5;  /* bit[8-12] : Bit delay line setting of CK1 */
        unsigned int  reserved_1   : 3;  /* bit[13-15]: reserved */
        unsigned int  refclk_bdl   : 5;  /* bit[16-20]: Bit delay line setting of REFCLK */
        unsigned int  reserved_2   : 3;  /* bit[21-23]: reserved */
        unsigned int  fbclk_bdl    : 5;  /* bit[24-28]: Bit delay line setting of FBCLK */
        unsigned int  reserved_3   : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_dramclk0_bdl_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_dramclk0_bdl_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_dramclk1_bdl_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_dramclk1_bdl_END    (12)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_refclk_bdl_START    (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_refclk_bdl_END      (20)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_fbclk_bdl_START     (24)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCLKBDL_fbclk_bdl_END       (28)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_UNION
 结构说明  : DDRC_PACK_ACPHYCTL0 寄存器结构定义。地址偏移量:0x170，初值:0x00000010，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enshift_a0     : 1;  /* bit[0]   : PHY control registers. */
        unsigned int  margin_a0      : 1;  /* bit[1]   : PHY control registers. */
        unsigned int  margin_a1      : 1;  /* bit[2]   : PHY control registers. */
        unsigned int  dramclk_h      : 1;  /* bit[3]   : PHY control registers. */
        unsigned int  dramclk_l      : 1;  /* bit[4]   : PHY control registers. */
        unsigned int  lvdqclkdiv2    : 1;  /* bit[5]   : PHY control registers. */
        unsigned int  bufphyclk_div2 : 1;  /* bit[6]   : PHY control registers. */
        unsigned int  reserved       : 25; /* bit[7-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_enshift_a0_START      (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_enshift_a0_END        (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_margin_a0_START       (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_margin_a0_END         (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_margin_a1_START       (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_margin_a1_END         (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_dramclk_h_START       (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_dramclk_h_END         (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_dramclk_l_START       (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_dramclk_l_END         (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_lvdqclkdiv2_START     (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_lvdqclkdiv2_END       (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_bufphyclk_div2_START  (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL0_bufphyclk_div2_END    (6)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_UNION
 结构说明  : DDRC_PACK_ACPHYCTL1 寄存器结构定义。地址偏移量:0x174，初值:0x00000000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dram1sel_reset : 1;  /* bit[0]    : PHY control registers. */
        unsigned int  dram1sel_cs    : 1;  /* bit[1]    : PHY control registers. */
        unsigned int  dram1sel_cke   : 1;  /* bit[2]    : PHY control registers. */
        unsigned int  dram1sel_odt   : 1;  /* bit[3]    : PHY control registers. */
        unsigned int  dram1sel_ras   : 1;  /* bit[4]    : PHY control registers. */
        unsigned int  dram1sel_cas   : 1;  /* bit[5]    : PHY control registers. */
        unsigned int  dram1sel_we    : 1;  /* bit[6]    : PHY control registers. */
        unsigned int  reserved_0     : 1;  /* bit[7]    : reserved */
        unsigned int  dram1sel_ba    : 3;  /* bit[8-10] : PHY control registers. */
        unsigned int  reserved_1     : 5;  /* bit[11-15]: reserved */
        unsigned int  dram1sel_addr  : 16; /* bit[16-31]: PHY control registers. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_reset_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_reset_END    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_cs_START     (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_cs_END       (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_cke_START    (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_cke_END      (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_odt_START    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_odt_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_ras_START    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_ras_END      (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_cas_START    (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_cas_END      (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_we_START     (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_we_END       (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_ba_START     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_ba_END       (10)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_addr_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL1_dram1sel_addr_END     (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_UNION
 结构说明  : DDRC_PACK_ACPHYCTL2 寄存器结构定义。地址偏移量:0x178，初值:0x00000000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sdrsel_reset : 1;  /* bit[0]    : PHY control registers. */
        unsigned int  sdrsel_cs    : 1;  /* bit[1]    : PHY control registers. */
        unsigned int  sdrsel_cke   : 1;  /* bit[2]    : PHY control registers. */
        unsigned int  sdrsel_odt   : 1;  /* bit[3]    : PHY control registers. */
        unsigned int  sdrsel_ras   : 1;  /* bit[4]    : PHY control registers. */
        unsigned int  sdrsel_cas   : 1;  /* bit[5]    : PHY control registers. */
        unsigned int  sdrsel_we    : 1;  /* bit[6]    : PHY control registers. */
        unsigned int  reserved_0   : 1;  /* bit[7]    : reserved */
        unsigned int  sdrsel_ba    : 3;  /* bit[8-10] : PHY control registers. */
        unsigned int  reserved_1   : 5;  /* bit[11-15]: reserved */
        unsigned int  sdrsel_addr  : 16; /* bit[16-31]: PHY control registers. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_reset_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_reset_END    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_cs_START     (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_cs_END       (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_cke_START    (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_cke_END      (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_odt_START    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_odt_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_ras_START    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_ras_END      (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_cas_START    (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_cas_END      (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_we_START     (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_we_END       (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_ba_START     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_ba_END       (10)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_addr_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL2_sdrsel_addr_END     (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_UNION
 结构说明  : DDRC_PACK_ACPHYCTL3 寄存器结构定义。地址偏移量:0x17C，初值:0x00000000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  posedge_reset : 1;  /* bit[0]    : PHY control registers. */
        unsigned int  posedge_cs    : 1;  /* bit[1]    : PHY control registers. */
        unsigned int  posedge_cke   : 1;  /* bit[2]    : PHY control registers. */
        unsigned int  posedge_odt   : 1;  /* bit[3]    : PHY control registers. */
        unsigned int  posedge_ras   : 1;  /* bit[4]    : PHY control registers. */
        unsigned int  posedge_cas   : 1;  /* bit[5]    : PHY control registers. */
        unsigned int  posedge_we    : 1;  /* bit[6]    : PHY control registers. */
        unsigned int  reserved_0    : 1;  /* bit[7]    : reserved */
        unsigned int  posedge_ba    : 3;  /* bit[8-10] : PHY control registers. */
        unsigned int  reserved_1    : 5;  /* bit[11-15]: reserved */
        unsigned int  posedge_addr  : 16; /* bit[16-31]: PHY control registers. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_reset_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_reset_END    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_cs_START     (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_cs_END       (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_cke_START    (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_cke_END      (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_odt_START    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_odt_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_ras_START    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_ras_END      (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_cas_START    (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_cas_END      (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_we_START     (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_we_END       (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_ba_START     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_ba_END       (10)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_addr_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL3_posedge_addr_END     (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_UNION
 结构说明  : DDRC_PACK_ACPHYCTL4 寄存器结构定义。地址偏移量:0x180，初值:0x00000000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timing2T_reset : 1;  /* bit[0]    : PHY control registers. */
        unsigned int  timing2T_cs    : 1;  /* bit[1]    : PHY control registers. */
        unsigned int  timing2T_cke   : 1;  /* bit[2]    : PHY control registers. */
        unsigned int  timing2T_odt   : 1;  /* bit[3]    : PHY control registers. */
        unsigned int  timing2T_ras   : 1;  /* bit[4]    : PHY control registers. */
        unsigned int  timing2T_cas   : 1;  /* bit[5]    : PHY control registers. */
        unsigned int  timing2T_we    : 1;  /* bit[6]    : PHY control registers. */
        unsigned int  reserved_0     : 1;  /* bit[7]    : reserved */
        unsigned int  timing2T_ba    : 3;  /* bit[8-10] : PHY control registers. */
        unsigned int  reserved_1     : 5;  /* bit[11-15]: reserved */
        unsigned int  timing2T_addr  : 16; /* bit[16-31]: PHY control registers. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_reset_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_reset_END    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_cs_START     (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_cs_END       (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_cke_START    (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_cke_END      (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_odt_START    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_odt_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_ras_START    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_ras_END      (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_cas_START    (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_cas_END      (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_we_START     (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_we_END       (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_ba_START     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_ba_END       (10)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_addr_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL4_timing2T_addr_END     (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_UNION
 结构说明  : DDRC_PACK_ACPHYCTL5 寄存器结构定义。地址偏移量:0x184，初值:0x00000000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk1x_dramclk0   : 4;  /* bit[0-3]  : PHY control registers. */
        unsigned int  clk1x_dramclk1   : 4;  /* bit[4-7]  : PHY control registers. */
        unsigned int  clk1x_cmd2t_ctl  : 4;  /* bit[8-11] : PHY control registers. */
        unsigned int  clk1x_cmd2t      : 4;  /* bit[12-15]: PHY control registers. */
        unsigned int  clk1x_cmd1t      : 4;  /* bit[16-19]: PHY control registers. */
        unsigned int  clk1x_mclk       : 4;  /* bit[20-23]: PHY control registers. */
        unsigned int  clk1x_cmd2t_ctl2 : 4;  /* bit[24-27]: PHY control registers. */
        unsigned int  reserved         : 4;  /* bit[28-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_dramclk0_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_dramclk0_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_dramclk1_START    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_dramclk1_END      (7)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_cmd2t_ctl_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_cmd2t_ctl_END     (11)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_cmd2t_START       (12)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_cmd2t_END         (15)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_cmd1t_START       (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_cmd1t_END         (19)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_mclk_START        (20)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_mclk_END          (23)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_cmd2t_ctl2_START  (24)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL5_clk1x_cmd2t_ctl2_END    (27)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_UNION
 结构说明  : DDRC_PACK_ACPHYCTL6 寄存器结构定义。地址偏移量:0x188，初值:0x00000000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk2x_dramclk0   : 4;  /* bit[0-3]  : PHY control registers. */
        unsigned int  clk2x_dramclk1   : 4;  /* bit[4-7]  : PHY control registers. */
        unsigned int  clk2x_cmd2t_ctl  : 4;  /* bit[8-11] : PHY control registers. */
        unsigned int  clk2x_cmd2t      : 4;  /* bit[12-15]: PHY control registers. */
        unsigned int  clk2x_cmd1t      : 4;  /* bit[16-19]: PHY control registers. */
        unsigned int  clk2x_mclk       : 4;  /* bit[20-23]: PHY control registers. */
        unsigned int  clk2x_cmd2t_ctl2 : 4;  /* bit[24-27]: PHY control registers. */
        unsigned int  reserved         : 4;  /* bit[28-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_dramclk0_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_dramclk0_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_dramclk1_START    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_dramclk1_END      (7)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_cmd2t_ctl_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_cmd2t_ctl_END     (11)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_cmd2t_START       (12)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_cmd2t_END         (15)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_cmd1t_START       (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_cmd1t_END         (19)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_mclk_START        (20)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_mclk_END          (23)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_cmd2t_ctl2_START  (24)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL6_clk2x_cmd2t_ctl2_END    (27)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_UNION
 结构说明  : DDRC_PACK_ACPHYCTL7 寄存器结构定义。地址偏移量:0x018C，初值:0x00800000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ckd2_mclk       : 3;  /* bit[0-2]  : set to 0 for Balong */
        unsigned int  ck0p_mclk       : 3;  /* bit[3-5]  : set to 0 for Balong */
        unsigned int  ck1p_dclk0      : 3;  /* bit[6-8]  : Configure DRAM CK0 Clock Phase from PLL 1 tap : 1/16 T */
        unsigned int  ck2p_dclk1      : 3;  /* bit[9-11] : Configure DRAM CK1 Clock Phase from PLL 1 tap : 1/16 T */
        unsigned int  ck3p_cmd1t      : 3;  /* bit[12-14]: set to 0 for Balong */
        unsigned int  reserved_0      : 3;  /* bit[15-17]: reserved */
        unsigned int  ck5p_cmd2tc     : 3;  /* bit[18-20]: set to 0 for Balong */
        unsigned int  halft_cmd2tctl  : 1;  /* bit[21]   : set to 0 for Balong */
        unsigned int  reserved_1      : 1;  /* bit[22]   : reserved */
        unsigned int  halft_cmd1t     : 1;  /* bit[23]   : set to 1 for Balong */
        unsigned int  halft_dramclk1  : 1;  /* bit[24]   : set to 0 for Balong */
        unsigned int  halft_dramclk0  : 1;  /* bit[25]   : set to 0 for Balong */
        unsigned int  halft_cmd2tctl2 : 1;  /* bit[26]   : set to 0 for Balong */
        unsigned int  reserved_2      : 5;  /* bit[27-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ckd2_mclk_START        (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ckd2_mclk_END          (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck0p_mclk_START        (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck0p_mclk_END          (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck1p_dclk0_START       (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck1p_dclk0_END         (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck2p_dclk1_START       (9)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck2p_dclk1_END         (11)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck3p_cmd1t_START       (12)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck3p_cmd1t_END         (14)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck5p_cmd2tc_START      (18)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_ck5p_cmd2tc_END        (20)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_cmd2tctl_START   (21)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_cmd2tctl_END     (21)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_cmd1t_START      (23)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_cmd1t_END        (23)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_dramclk1_START   (24)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_dramclk1_END     (24)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_dramclk0_START   (25)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_dramclk0_END     (25)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_cmd2tctl2_START  (26)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL7_halft_cmd2tctl2_END    (26)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_UNION
 结构说明  : DDRC_PACK_ACDEBUG 寄存器结构定义。地址偏移量:0x0190，初值:0x00000000，宽度:32
 寄存器说明: This register is for PHY debug only.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ac_rdcnt       : 3;  /* bit[0-2]  : phy ac_rdcnt */
        unsigned int  reserved_0     : 5;  /* bit[3-7]  : reserved */
        unsigned int  DESKEW_regread : 1;  /* bit[8]    : read value for phy DESKEW_regread */
        unsigned int  reserved_1     : 7;  /* bit[9-15] : reserved */
        unsigned int  acdbg_config   : 4;  /* bit[16-19]: phy acdbg config */
        unsigned int  reserved_2     : 12; /* bit[20-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_ac_rdcnt_START        (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_ac_rdcnt_END          (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_DESKEW_regread_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_DESKEW_regread_END    (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_acdbg_config_START    (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACDEBUG_acdbg_config_END      (19)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDC_UNION
 结构说明  : DDRC_PACK_ACPHYRSVDC 寄存器结构定义。地址偏移量:0x0194，初值:0xFFFF0000，宽度:32
 寄存器说明: AC block PHY reserved control pins. This register is for PHY control and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ac_rsv_control : 32; /* bit[0-31]: Reserved registers for the future. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDC_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDC_ac_rsv_control_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDC_ac_rsv_control_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDS_UNION
 结构说明  : DDRC_PACK_ACPHYRSVDS 寄存器结构定义。地址偏移量:0x0198，初值:0xFFFF0000，宽度:32
 寄存器说明: AC block PHY reserved control pins. This register is for PHY control and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ac_rsv_status : 32; /* bit[0-31]: Reserved registers for the future. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDS_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDS_ac_rsv_status_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYRSVDS_ac_rsv_status_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_UNION
 结构说明  : DDRC_PACK_ACPHYCTL8 寄存器结构定义。地址偏移量:0x019C，初值:0x00000000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ac_ck6p_reserve  : 3;  /* bit[0-2]  : set to 0 for Balong */
        unsigned int  ac_ck7p_reserve  : 3;  /* bit[3-5]  : set to 0 for Balong */
        unsigned int  ac_ck8p_reserve  : 3;  /* bit[6-8]  : set to 0 for Balong */
        unsigned int  ac_ck9p_reserve  : 3;  /* bit[9-11] : set to 0 for Balong */
        unsigned int  ac_ck10p_reserve : 3;  /* bit[12-14]: set to 0 for Balong */
        unsigned int  ac_ck11p_reserve : 3;  /* bit[15-17]: set to 0 for Balong */
        unsigned int  ac_ck12p_reserve : 3;  /* bit[18-20]: set to 0 for Balong */
        unsigned int  reserved         : 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck6p_reserve_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck6p_reserve_END     (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck7p_reserve_START   (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck7p_reserve_END     (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck8p_reserve_START   (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck8p_reserve_END     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck9p_reserve_START   (9)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck9p_reserve_END     (11)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck10p_reserve_START  (12)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck10p_reserve_END    (14)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck11p_reserve_START  (15)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck11p_reserve_END    (17)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck12p_reserve_START  (18)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL8_ac_ck12p_reserve_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL8_UNION
 结构说明  : DDRC_PACK_ACCMDBDL8 寄存器结构定义。地址偏移量:0x1A0，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  odt1_bdl0 : 5;  /* bit[0-4]  : odt_bdl0 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  odt1_bdl1 : 5;  /* bit[16-20]: odt_bdl1 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL8_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL8_odt1_bdl0_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL8_odt1_bdl0_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL8_odt1_bdl1_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL8_odt1_bdl1_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL9_UNION
 结构说明  : DDRC_PACK_ACCMDBDL9 寄存器结构定义。地址偏移量:0x1A4，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  odt1_bdl2 : 5;  /* bit[0-4]  : odt1_bdl2 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  odt1_bdl3 : 5;  /* bit[16-20]: odt1_bdl3 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL9_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL9_odt1_bdl2_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL9_odt1_bdl2_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL9_odt1_bdl3_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL9_odt1_bdl3_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL10_UNION
 结构说明  : DDRC_PACK_ACCMDBDL10 寄存器结构定义。地址偏移量:0x1A8，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cs1_bdl0 : 5;  /* bit[0-4]  : cs1_bdl0 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  cs1_bdl1 : 5;  /* bit[16-20]: cs1_bdl1 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL10_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL10_cs1_bdl0_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL10_cs1_bdl0_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL10_cs1_bdl1_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL10_cs1_bdl1_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL11_UNION
 结构说明  : DDRC_PACK_ACCMDBDL11 寄存器结构定义。地址偏移量:0x1AC，初值:0x00000000，宽度:32
 寄存器说明: AC command bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cs1_bdl2 : 5;  /* bit[0-4]  : cs1_bdl2 */
        unsigned int  reserved_0: 11; /* bit[5-15] : reserved */
        unsigned int  cs1_bdl3 : 5;  /* bit[16-20]: cs1_bdl3 */
        unsigned int  reserved_1: 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL11_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL11_cs1_bdl2_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL11_cs1_bdl2_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL11_cs1_bdl3_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACCMDBDL11_cs1_bdl3_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_UNION
 结构说明  : DDRC_PACK_ACPHYDCC 寄存器结构定义。地址偏移量:0x01B0，初值:0x00000000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acdcc_dq1  : 3;  /* bit[0-2]  : set to 0 for Balong */
        unsigned int  reserved_0 : 1;  /* bit[3]    : reserved */
        unsigned int  acdcc_dq0  : 3;  /* bit[4-6]  : set to 0 for Balong */
        unsigned int  reserved_1 : 1;  /* bit[7]    : reserved */
        unsigned int  acdcc_dqs1 : 3;  /* bit[8-10] : set to 0 for Balong */
        unsigned int  reserved_2 : 1;  /* bit[11]   : reserved */
        unsigned int  acdcc_dqs0 : 3;  /* bit[12-14]: set to 0 for Balong */
        unsigned int  reserved_3 : 1;  /* bit[15]   : reserved */
        unsigned int  acdcc_mclk : 3;  /* bit[16-18]: set to 0 for Balong */
        unsigned int  reserved_4 : 13; /* bit[19-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_dq1_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_dq1_END     (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_dq0_START   (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_dq0_END     (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_dqs1_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_dqs1_END    (10)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_dqs0_START  (12)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_dqs0_END    (14)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_mclk_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYDCC_acdcc_mclk_END    (18)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_UNION
 结构说明  : DDRC_PACK_ACPHYCTL9 寄存器结构定义。地址偏移量:0x01B4，初值:0x00000000，宽度:32
 寄存器说明: AC block PHY control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acsdrsel_cs1    : 1;  /* bit[0]    : set to 0 for Balong */
        unsigned int  acsdrsel_cke1   : 1;  /* bit[1]    : set to 0 for Balong */
        unsigned int  acsdrsel_odt1   : 1;  /* bit[2]    : set to 0 for Balong */
        unsigned int  acposedge_cs1   : 1;  /* bit[3]    : set to 0 for Balong */
        unsigned int  acposedge_cke1  : 1;  /* bit[4]    : set to 0 for Balong */
        unsigned int  acposedge_odt1  : 1;  /* bit[5]    : set to 0 for Balong */
        unsigned int  acdram1sel_cs1  : 1;  /* bit[6]    : set to 0 for Balong */
        unsigned int  acdram1sel_cke1 : 1;  /* bit[7]    : set to 0 for Balong */
        unsigned int  acdram1sel_odt1 : 1;  /* bit[8]    : set to 0 for Balong */
        unsigned int  actiming2t_cs1  : 1;  /* bit[9]    : set to 0 for Balong */
        unsigned int  actiming2t_cke1 : 1;  /* bit[10]   : set to 0 for Balong */
        unsigned int  actiming2t_odt1 : 1;  /* bit[11]   : set to 0 for Balong */
        unsigned int  reserved        : 20; /* bit[12-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acsdrsel_cs1_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acsdrsel_cs1_END       (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acsdrsel_cke1_START    (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acsdrsel_cke1_END      (1)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acsdrsel_odt1_START    (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acsdrsel_odt1_END      (2)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acposedge_cs1_START    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acposedge_cs1_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acposedge_cke1_START   (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acposedge_cke1_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acposedge_odt1_START   (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acposedge_odt1_END     (5)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acdram1sel_cs1_START   (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acdram1sel_cs1_END     (6)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acdram1sel_cke1_START  (7)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acdram1sel_cke1_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acdram1sel_odt1_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_acdram1sel_odt1_END    (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_actiming2t_cs1_START   (9)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_actiming2t_cs1_END     (9)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_actiming2t_cke1_START  (10)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_actiming2t_cke1_END    (10)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_actiming2t_odt1_START  (11)
#define SOC_MDDRC_PACK_DDRC_PACK_ACPHYCTL9_actiming2t_odt1_END    (11)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_UNION
 结构说明  : DDRC_PACK_CATTIMER0 寄存器结构定义。地址偏移量:0x01C0，初值:0x0B170B0B，宽度:32
 寄存器说明: CA Training Timer0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tcackel  : 6;  /* bit[0-5]  : tCACKEL (dficlk cycle) */
        unsigned int  reserved_0: 2;  /* bit[6-7]  : reserved */
        unsigned int  tcaent   : 6;  /* bit[8-13] : tCAENT (dficlk cycle) */
        unsigned int  reserved_1: 2;  /* bit[14-15]: reserved */
        unsigned int  tcacd    : 6;  /* bit[16-21]: tCACD (dficlk cycle) */
        unsigned int  reserved_2: 2;  /* bit[22-23]: reserved */
        unsigned int  tcackeh  : 6;  /* bit[24-29]: tCACKEH (dficlk cycle) */
        unsigned int  reserved_3: 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_tcackel_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_tcackel_END     (5)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_tcaent_START    (8)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_tcaent_END      (13)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_tcacd_START     (16)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_tcacd_END       (21)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_tcackeh_START   (24)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER0_tcackeh_END     (29)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_CATTIMER1_UNION
 结构说明  : DDRC_PACK_CATTIMER1 寄存器结构定义。地址偏移量:0x01C4，初值:0x0000040B，宽度:32
 寄存器说明: CA Training Timer1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tcaext   : 6;  /* bit[0-5]  : tCAEXT (dficlk cycle) */
        unsigned int  reserved_0: 2;  /* bit[6-7]  : reserved */
        unsigned int  tmrz     : 6;  /* bit[8-13] : tMRZ (dficlk cycle) */
        unsigned int  reserved_1: 18; /* bit[14-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_CATTIMER1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER1_tcaext_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER1_tcaext_END      (5)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER1_tmrz_START      (8)
#define SOC_MDDRC_PACK_DDRC_PACK_CATTIMER1_tmrz_END        (13)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_UNION
 结构说明  : DDRC_PACK_CATCONFIG 寄存器结构定义。地址偏移量:0x01C8，初值:0x00082606，宽度:32
 寄存器说明: CA Training Configuration
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ca_samp_num_bdl   : 4;  /* bit[0-3]  : HW CA Training - sample count for CA BDL:
                                                             Valid value: 1~15 */
        unsigned int  reserved_0        : 4;  /* bit[4-7]  : reserved */
        unsigned int  ca_samp_num_ph    : 4;  /* bit[8-11] : HW CA Training - sample count for CA phase:
                                                             Valid value: 1~15 */
        unsigned int  cat_rb_backtap    : 4;  /* bit[12-15]: HW CA Training Right Boundary back tap. This field is used to offset the addrph_a before do the Right Boundary Search to overcome meta-sable zone. */
        unsigned int  reserved_1        : 2;  /* bit[16-17]: reserved */
        unsigned int  cat_cat_phydq_sel : 1;  /* bit[18]   : HW CA Trainig - PHY DQ CA result selection:
                                                             1: MSB selected
                                                             0: LSB selected (default) */
        unsigned int  cat_restore_en    : 1;  /* bit[19]   : HW CA Trainig - restore enable. If set as HIGH, the CA train will restore the ca phase before 2nd CA train (mapping stage) starts. This field is recommended to set as HIGH.
                                                             1: enable (default)
                                                             0: disable */
        unsigned int  cat_openeye_en    : 1;  /* bit[20]   : HW CA Trainig - openeye enable
                                                             1: enable
                                                             0: disable (default) */
        unsigned int  reserved_2        : 3;  /* bit[21-23]: reserved */
        unsigned int  sw_cat_mrw42      : 1;  /* bit[24]   : SW CA Training - issue the MRW42 command, cleared by HW */
        unsigned int  sw_cat_mrw48      : 1;  /* bit[25]   : SW CA Training - issue the MRW48 command, cleared by HW */
        unsigned int  sw_cat_mrw41      : 1;  /* bit[26]   : SW CA Training - issue the MRW41 command, cleared by HW */
        unsigned int  sw_cat_strobe     : 1;  /* bit[27]   : SW CA Training - issue the predefined SW CA pattern, cleared by HW */
        unsigned int  sw_cat_cke_high   : 1;  /* bit[28]   : SW CA Training - request the CKE to be HIGH, cleared by HW */
        unsigned int  sw_cat_cke_low    : 1;  /* bit[29]   : SW CA Training - request the CKE to be LOW, cleared by HW */
        unsigned int  sw_cat_dqvalid    : 1;  /* bit[30]   : SW CA Training - indicate if the CA result is ready. This bit will be cleared when sw_cat_strobe is asserted. Then after the timing tCACD, this bit will be asserted.
                                                             1: valid
                                                             0: not valid */
        unsigned int  sw_cat_en         : 1;  /* bit[31]   : SW CA Training - enable bit. Assert this bit, PACK will enter the SW CA Training state. After the SW CA Training is done, this bit should be de-assert by firmware.
                                                             1: enable
                                                             0: disable (default) */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_ca_samp_num_bdl_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_ca_samp_num_bdl_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_ca_samp_num_ph_START     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_ca_samp_num_ph_END       (11)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_cat_rb_backtap_START     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_cat_rb_backtap_END       (15)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_cat_cat_phydq_sel_START  (18)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_cat_cat_phydq_sel_END    (18)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_cat_restore_en_START     (19)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_cat_restore_en_END       (19)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_cat_openeye_en_START     (20)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_cat_openeye_en_END       (20)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_mrw42_START       (24)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_mrw42_END         (24)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_mrw48_START       (25)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_mrw48_END         (25)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_mrw41_START       (26)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_mrw41_END         (26)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_strobe_START      (27)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_strobe_END        (27)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_cke_high_START    (28)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_cke_high_END      (28)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_cke_low_START     (29)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_cke_low_END       (29)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_dqvalid_START     (30)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_dqvalid_END       (30)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_en_START          (31)
#define SOC_MDDRC_PACK_DDRC_PACK_CATCONFIG_sw_cat_en_END            (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_CATRESULT_UNION
 结构说明  : DDRC_PACK_CATRESULT 寄存器结构定义。地址偏移量:0x01CC，初值:0x00000000，宽度:32
 寄存器说明: CA Training result for debug
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cat_reault : 10; /* bit[0-9]  : CA Training - HW CA Training result for each address bit during each HW CA Trainig state. These bits are only meaningful after HW CA Training done and are only used for debug purpose.
                                                      1: OK
                                                      0: Failed */
        unsigned int  reserved   : 22; /* bit[10-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_CATRESULT_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_CATRESULT_cat_reault_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_CATRESULT_cat_reault_END    (9)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_PHYDQRESULT_UNION
 结构说明  : DDRC_PACK_PHYDQRESULT 寄存器结构定义。地址偏移量:0x01D0，初值:0x00000000，宽度:32
 寄存器说明: SW CA Training DQ result from PHY
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_dq_result : 32; /* bit[0-31]: SW CA Training - SW CA Training latched results from DRAM device.
                                                        For LPDDR3 CA Training, the meaningful bits are [15:0] (map to DQ[15:0]). PACK and PHY will not latch these bits, so these bits are what you get on the DRAM device's DQ. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_PHYDQRESULT_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_PHYDQRESULT_phy_dq_result_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_PHYDQRESULT_phy_dq_result_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_UNION
 结构说明  : DDRC_PACK_ADDRPHBOUND 寄存器结构定义。地址偏移量:0x01D4，初值:0x00080000，宽度:32
 寄存器说明: CA Training addr phase boundary
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addrph_a_right : 5;  /* bit[0-4]  : HW CA Training, these bits record the right boundary of addr phase after HW CA Traininig. (for debug purpose) */
        unsigned int  reserved_0     : 3;  /* bit[5-7]  : reserved */
        unsigned int  addrph_a_left  : 5;  /* bit[8-12] : HW CA Training, these bits record the left boundary of addr phase after HW CA Traininig. (for debug purpose) */
        unsigned int  reserved_1     : 3;  /* bit[13-15]: reserved */
        unsigned int  addrph_a       : 5;  /* bit[16-20]: HW CA Training, these bits record the left boundary of addr phase after HW CA Traininig. (for debug purpose) */
        unsigned int  reserved_2     : 3;  /* bit[21-23]: reserved */
        unsigned int  addrph_a_ori   : 5;  /* bit[24-28]: HW CA Training, these bits record the addr phase before HW CA Traininig. (for debug purpose) */
        unsigned int  reserved_3     : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_addrph_a_right_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_addrph_a_right_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_addrph_a_left_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_addrph_a_left_END     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_addrph_a_START        (16)
#define SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_addrph_a_END          (20)
#define SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_addrph_a_ori_START    (24)
#define SOC_MDDRC_PACK_DDRC_PACK_ADDRPHBOUND_addrph_a_ori_END      (28)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_P_UNION
 结构说明  : DDRC_PACK_SWCATPATTERN_P 寄存器结构定义。地址偏移量:0x01D8，初值:0x00000000，宽度:32
 寄存器说明: SW CA Training pattern for  positive CK edge
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_cat_pat_p : 10; /* bit[0-9]  : SW CA Training pattern for positive CK edge. User can fill any patterns. */
        unsigned int  reserved     : 22; /* bit[10-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_P_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_P_sw_cat_pat_p_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_P_sw_cat_pat_p_END    (9)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_N_UNION
 结构说明  : DDRC_PACK_SWCATPATTERN_N 寄存器结构定义。地址偏移量:0x01DC，初值:0x000003FF，宽度:32
 寄存器说明: SW CA Training pattern for  negative CK edge
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_cat_pat_n : 10; /* bit[0-9]  : SW CA Training pattern for negative CK edge. User can fill any patterns. */
        unsigned int  reserved     : 22; /* bit[10-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_N_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_N_sw_cat_pat_n_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_SWCATPATTERN_N_sw_cat_pat_n_END    (9)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_UNION
 结构说明  : DDRC_PACK_LPCTRL 寄存器结构定义。地址偏移量:0x01E4，初值:0xFE808688，宽度:32
 寄存器说明: Low Power Control Register
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_dram_clk_enable          : 4;  /* bit[0-3]  : DFI DRAM clock enable timing parameter. This field will be use to count down before dram CK is turned-on. */
        unsigned int  t_dram_clk_disable         : 4;  /* bit[4-7]  : DFI DRAM clock disable timing parameter. This field will be use to count down before dram CK is turned-off. */
        unsigned int  t_wakeup_thrd_lv3          : 4;  /* bit[8-11] : DFI Low Power contorl interafce timing threhold,
                                                                      >= wakeup_thrd_lv2: PHY clock gating, disable AC output enable
                                                                      >= wakeup_thrd_lv3: PHY clock gating, disable AC output enable, PHY PLL power down
                                                                      Note: wakeup_thrd_lv3 should >= wakeup_thrd_lv2 */
        unsigned int  t_resp                     : 4;  /* bit[12-15]: DFI Low Power control interface timing parameter. Should be smaller than DFI's tlp_resp. */
        unsigned int  lp_pll_powerdown_disable   : 1;  /* bit[16]   : Disable LP PLL Disable function when Low-Power Assert.
                                                                      1: Turn-off the related LP funtion
                                                                      0: Turn-on the related LP funtion (default) */
        unsigned int  lp_dx_phy_phgated_disable  : 1;  /* bit[17]   : Disable DX PHY Clock Phase Gating function when Low-Power Assert.
                                                                      1: Turn-off the related LP funtion
                                                                      0: Turn-on the related LP funtion (default) */
        unsigned int  lp_dx_phy_clkgated_disable : 1;  /* bit[18]   : Disable LP DX PHY Clock Gating function when Low-Power Assert.
                                                                      1: Turn-off the related LP funtion
                                                                      0: Turn-on the related LP funtion (default) */
        unsigned int  lp_ac_phy_clkgated_disable : 1;  /* bit[19]   : Disable LP AC PHY Clock Gating function when Low-Power Asser.
                                                                      1: Turn-off the related LP funtion
                                                                      0: Turn-on the related LP funtion (default) */
        unsigned int  lp_cmdoen_disable          : 1;  /* bit[20]   : Disable the LP CMDOEN function when Low-Power Assert. If turn-on this function, the IO output of CA will be floating to save IO power.
                                                                      1: Turn-off the related LP funtion
                                                                      0: Turn-on the related LP funtion (default) */
        unsigned int  t_wakeup_thrd_lv2          : 4;  /* bit[21-24]: DFI Low Power contorl interafce timing threhold,
                                                                      >= wakeup_thrd_lv2: PHY clock gating, disable AC output enable
                                                                      >= wakeup_thrd_lv3: PHY clock gating, disable AC output enable, PHY PLL disalbe
                                                                      Note: wakeup_thrd_lv3 should >= wakeup_thrd_lv2 */
        unsigned int  gcken_dxbist               : 1;  /* bit[25]   : Manual gating control - DATA BIST clock gating.
                                                                      1: clock on (default)
                                                                      0: clock off */
        unsigned int  gcken_acbist               : 1;  /* bit[26]   : Manual gating control - AC BIST clock gating.
                                                                      1: clock on (default)
                                                                      0: clock off */
        unsigned int  gcken_retrain              : 1;  /* bit[27]   : Manual gating control - Re-training clock gating.
                                                                      1: clock on (default)
                                                                      0: clock off */
        unsigned int  gcken_dtrain               : 1;  /* bit[28]   : Manual gating control - DATA Training clock gating.
                                                                      1: clock on (default)
                                                                      0: clock off */
        unsigned int  gcken_dinit                : 1;  /* bit[29]   : Manual gating control - DRAM initialization clock gating.
                                                                      1: clock on (default)
                                                                      0: clock off */
        unsigned int  gcken_dfi_main             : 1;  /* bit[30]   : Manual gating control - Main DFI clock gating switch .
                                                                      1: clock on (defalt)
                                                                      0: clock off
                                                                      When this field is set as LOW (clock off), it will affect some control registers which use dfi_main clock. */
        unsigned int  gcken_auto_en              : 1;  /* bit[31]   : Auto gated clock control enable. If this field is set as HIGH, HW will control the PACK high-level clock gating automatically/dynamically.
                                                                      1: on (default)
                                                                      0: off */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_dram_clk_enable_START           (0)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_dram_clk_enable_END             (3)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_dram_clk_disable_START          (4)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_dram_clk_disable_END            (7)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_wakeup_thrd_lv3_START           (8)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_wakeup_thrd_lv3_END             (11)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_resp_START                      (12)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_resp_END                        (15)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_pll_powerdown_disable_START    (16)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_pll_powerdown_disable_END      (16)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_dx_phy_phgated_disable_START   (17)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_dx_phy_phgated_disable_END     (17)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_dx_phy_clkgated_disable_START  (18)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_dx_phy_clkgated_disable_END    (18)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_ac_phy_clkgated_disable_START  (19)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_ac_phy_clkgated_disable_END    (19)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_cmdoen_disable_START           (20)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_lp_cmdoen_disable_END             (20)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_wakeup_thrd_lv2_START           (21)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_t_wakeup_thrd_lv2_END             (24)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_dxbist_START                (25)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_dxbist_END                  (25)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_acbist_START                (26)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_acbist_END                  (26)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_retrain_START               (27)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_retrain_END                 (27)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_dtrain_START                (28)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_dtrain_END                  (28)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_dinit_START                 (29)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_dinit_END                   (29)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_dfi_main_START              (30)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_dfi_main_END                (30)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_auto_en_START               (31)
#define SOC_MDDRC_PACK_DDRC_PACK_LPCTRL_gcken_auto_en_END                 (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_UNION
 结构说明  : DDRC_PACK_DXNBISTCTRL 寄存器结构定义。地址偏移量:0x200+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control if the loopback data is compre during BIST of Data Blocks.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bist_en      : 1;  /* bit[0]    : Bist Enable.
                                                        This signal specify if the Data block is included for loopback test while bist_op is set to BIST Enable. */
        unsigned int  reserved_0   : 7;  /* bit[1-7]  : reserved */
        unsigned int  dq_cmp_mask  : 8;  /* bit[8-15] : DQ Loopback Compare Mask.
                                                        This field is used to control if the comparison of the loopback data on DQ bits are disabled. 
                                                        0: Not compare 
                                                        1: Compare */
        unsigned int  dqm_cmp_mask : 1;  /* bit[16]   : DQM Loopback Compare Mask.
                                                        This field is used to control if the comparison of the loopback data on DQM bits are disabled. 
                                                        0: Not compare 
                                                        1: Compare */
        unsigned int  reserved_1   : 7;  /* bit[17-23]: reserved */
        unsigned int  lfsr_seed    : 8;  /* bit[24-31]: LFSR Seed Value.
                                                        This field is uesed to specify the initial seed value of the LFSR generator to this byte lane. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_bist_en_START       (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_bist_en_END         (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_dq_cmp_mask_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_dq_cmp_mask_END     (15)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_dqm_cmp_mask_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_dqm_cmp_mask_END    (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_lfsr_seed_START     (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTCTRL_lfsr_seed_END       (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_UNION
 结构说明  : DDRC_PACK_DXNBISTSTS 寄存器结构定义。地址偏移量:0x204+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register shows the results and status of the BIST test. Please note that this register is read-only, and can only be reset by bist_op setting to BIST Reset.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dq_cmp_rerr : 8;  /* bit[0-7]  : BIST Error on DQ at rising edge of DQS */
        unsigned int  dq_cmp_ferr : 8;  /* bit[8-15] : BIST Error on DQ at falling edge of DQS. */
        unsigned int  dqm_cmp_err : 1;  /* bit[16]   : BIST Error on DQM. */
        unsigned int  reserved    : 15; /* bit[17-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_dq_cmp_rerr_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_dq_cmp_rerr_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_dq_cmp_ferr_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_dq_cmp_ferr_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_dqm_cmp_err_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNBISTSTS_dqm_cmp_err_END    (16)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNCTRL_UNION
 结构说明  : DDRC_PACK_DXNCTRL 寄存器结构定义。地址偏移量:0x208+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control Data Block
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bl_dis   : 1;  /* bit[0]   : Byte Lane Disable. 
                                                   This field specify if the corresponding byte lane is in use. If the corresponding byte lane is not used, the PLL/DDL will be power down, and the output driver will be turn off. */
        unsigned int  dm_dis   : 1;  /* bit[1]   : Data mask disable.
                                                   This field specify if the corresponding write data mask (DM) is enabled or not. When this field is set to 1, the corresponding DM is not used and calibrated while performing training opreation. */
        unsigned int  reserved : 30; /* bit[2-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCTRL_bl_dis_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCTRL_bl_dis_END      (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCTRL_dm_dis_START    (1)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCTRL_dm_dis_END      (1)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_UNION
 结构说明  : DDRC_PACK_DXNWDQNBDL0 寄存器结构定义。地址偏移量:0x210+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control the bit delay line of the DATA block.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdq0bdl  : 5;  /* bit[0-4]  : Delay Select of WDQ0.
                                                    This field specify the delay value of the bit delay line on write path of DQ0. */
        unsigned int  reserved_0: 3;  /* bit[5-7]  : reserved */
        unsigned int  wdq1bdl  : 5;  /* bit[8-12] : Delay Select of WDQ1.
                                                    This field specify the delay value of the bit delay line on write path of DQ1. */
        unsigned int  reserved_1: 3;  /* bit[13-15]: reserved */
        unsigned int  wdq2bdl  : 5;  /* bit[16-20]: Delay Select of WDQ2.
                                                    This field specify the delay value of the bit delay line on write path of DQ2. */
        unsigned int  reserved_2: 3;  /* bit[21-23]: reserved */
        unsigned int  wdq3bdl  : 5;  /* bit[24-28]: Delay Select of WDQ3.
                                                    This field specify the delay value of the bit delay line on write path of DQ3. */
        unsigned int  reserved_3: 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_wdq0bdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_wdq0bdl_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_wdq1bdl_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_wdq1bdl_END     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_wdq2bdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_wdq2bdl_END     (20)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_wdq3bdl_START   (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL0_wdq3bdl_END     (28)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_UNION
 结构说明  : DDRC_PACK_DXNWDQNBDL1 寄存器结构定义。地址偏移量:0x214+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control the bit delay line of the DATA block.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdq4bdl  : 5;  /* bit[0-4]  : Delay Select of WDQ4.
                                                    This field specify the delay value of the bit delay line on write path of DQ0. */
        unsigned int  reserved_0: 3;  /* bit[5-7]  : reserved */
        unsigned int  wdq5bdl  : 5;  /* bit[8-12] : Delay Select of WDQ5.
                                                    This field specify the delay value of the bit delay line on write path of DQ1. */
        unsigned int  reserved_1: 3;  /* bit[13-15]: reserved */
        unsigned int  wdq6bdl  : 5;  /* bit[16-20]: Delay Select of WDQ6.
                                                    This field specify the delay value of the bit delay line on write path of DQ2. */
        unsigned int  reserved_2: 3;  /* bit[21-23]: reserved */
        unsigned int  wdq7bdl  : 5;  /* bit[24-28]: Delay Select of WDQ7.
                                                    This field specify the delay value of the bit delay line on write path of DQ3. */
        unsigned int  reserved_3: 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_wdq4bdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_wdq4bdl_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_wdq5bdl_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_wdq5bdl_END     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_wdq6bdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_wdq6bdl_END     (20)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_wdq7bdl_START   (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL1_wdq7bdl_END     (28)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL2_UNION
 结构说明  : DDRC_PACK_DXNWDQNBDL2 寄存器结构定义。地址偏移量:0x218+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control the bit delay line of the DATA block.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdmbdl   : 5;  /* bit[0-4] : Delay Select of WDQM.
                                                   This field specify the delay value of the bit delay line on DQM. */
        unsigned int  reserved : 27; /* bit[5-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL2_wdmbdl_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQNBDL2_wdmbdl_END      (4)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_UNION
 结构说明  : DDRC_PACK_DXNRDQNBDL0 寄存器结构定义。地址偏移量:0x21C+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control the bit delay line of the DATA block.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rdq0bdl  : 5;  /* bit[0-4]  : Delay Select of RDQ0.
                                                    This field specify the delay value of the bit delay line on write path of DQ0. */
        unsigned int  reserved_0: 3;  /* bit[5-7]  : reserved */
        unsigned int  rdq1bdl  : 5;  /* bit[8-12] : Delay Select of RDQ1.
                                                    This field specify the delay value of the bit delay line on write path of DQ1. */
        unsigned int  reserved_1: 3;  /* bit[13-15]: reserved */
        unsigned int  rdq2bdl  : 5;  /* bit[16-20]: Delay Select of RDQ2.
                                                    This field specify the delay value of the bit delay line on write path of DQ2. */
        unsigned int  reserved_2: 3;  /* bit[21-23]: reserved */
        unsigned int  rdq3bdl  : 5;  /* bit[24-28]: Delay Select of RDQ3.
                                                    This field specify the delay value of the bit delay line on write path of DQ3. */
        unsigned int  reserved_3: 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_rdq0bdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_rdq0bdl_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_rdq1bdl_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_rdq1bdl_END     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_rdq2bdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_rdq2bdl_END     (20)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_rdq3bdl_START   (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL0_rdq3bdl_END     (28)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_UNION
 结构说明  : DDRC_PACK_DXNRDQNBDL1 寄存器结构定义。地址偏移量:0x220+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control the bit delay line of the DATA block.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rdq4bdl  : 5;  /* bit[0-4]  : Delay Select of RDQ4.
                                                    This field specify the delay value of the bit delay line on write path of DQ0. */
        unsigned int  reserved_0: 3;  /* bit[5-7]  : reserved */
        unsigned int  rdq5bdl  : 5;  /* bit[8-12] : Delay Select of RDQ5.
                                                    This field specify the delay value of the bit delay line on write path of DQ1. */
        unsigned int  reserved_1: 3;  /* bit[13-15]: reserved */
        unsigned int  rdq6bdl  : 5;  /* bit[16-20]: Delay Select of RDQ6.
                                                    This field specify the delay value of the bit delay line on write path of DQ2. */
        unsigned int  reserved_2: 3;  /* bit[21-23]: reserved */
        unsigned int  rdq7bdl  : 5;  /* bit[24-28]: Delay Select of RDQ7.
                                                    This field specify the delay value of the bit delay line on write path of DQ3. */
        unsigned int  reserved_3: 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_rdq4bdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_rdq4bdl_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_rdq5bdl_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_rdq5bdl_END     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_rdq6bdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_rdq6bdl_END     (20)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_rdq7bdl_START   (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL1_rdq7bdl_END     (28)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL2_UNION
 结构说明  : DDRC_PACK_DXNRDQNBDL2 寄存器结构定义。地址偏移量:0x224+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control the bit delay line of the DATA block.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rdmbdl   : 5;  /* bit[0-4] : Delay Select of RDQM.
                                                   This field specify the delay value of the bit delay line on loopback read path of DQM. */
        unsigned int  reserved : 27; /* bit[5-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL2_rdmbdl_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQNBDL2_rdmbdl_END      (4)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNOEBDL_UNION
 结构说明  : DDRC_PACK_DXNOEBDL 寄存器结构定义。地址偏移量:0x228+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: Output Enable Delay Line Control. This register specify the dely line value of the output enable delay line within PHY data block.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  oen_bdl    : 5;  /* bit[0-4]  : DQ Output Enable Delay Control. */
        unsigned int  reserved_0 : 11; /* bit[5-15] : reserved */
        unsigned int  wdqsoe_bdl : 5;  /* bit[16-20]: Write DQS Output Enable Delay Control. */
        unsigned int  reserved_1 : 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNOEBDL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNOEBDL_oen_bdl_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNOEBDL_oen_bdl_END       (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNOEBDL_wdqsoe_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNOEBDL_wdqsoe_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNRDQSDLY_UNION
 结构说明  : DDRC_PACK_DXNRDQSDLY 寄存器结构定义。地址偏移量:0x22C+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control the local delay line.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rdqsbdl  : 7;  /* bit[0-6]  : Read DQS Delay Select.
                                                    This field specify the phase shift of the Read DQS to create 90 degree delays. */
        unsigned int  reserved_0: 9;  /* bit[7-15] : reserved */
        unsigned int  rdqscyc  : 7;  /* bit[16-22]: Read DQS Clock Phase Select.
                                                    This field indicate how many taps does one clock cycle has with 1-T delay measurement type. This field indicate how many taps does a half of clock cycle has with half-T delay measurement type. This field will be used to create 90 degree delays for RDQS. */
        unsigned int  reserved_1: 9;  /* bit[23-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNRDQSDLY_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQSDLY_rdqsbdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQSDLY_rdqsbdl_END     (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQSDLY_rdqscyc_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDQSDLY_rdqscyc_END     (22)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNWDQSDLY_UNION
 结构说明  : DDRC_PACK_DXNWDQSDLY 寄存器结构定义。地址偏移量:0x230+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control the write leveling DQS delays.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdqsbdl   : 5;  /* bit[0-4]  : Write Leveling DQS Delay Select.
                                                     This field specify the delay value of delay applied on WDQS for write leveling. */
        unsigned int  reserved_0: 3;  /* bit[5-7]  : reserved */
        unsigned int  wdqsphase : 4;  /* bit[8-11] : Write DQS Clock Phase Select. */
        unsigned int  reserved_1: 20; /* bit[12-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNWDQSDLY_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQSDLY_wdqsbdl_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQSDLY_wdqsbdl_END      (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQSDLY_wdqsphase_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQSDLY_wdqsphase_END    (11)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNWDQDLY_UNION
 结构说明  : DDRC_PACK_DXNWDQDLY 寄存器结构定义。地址偏移量:0x234+(n)*0x80，初值:0x00000C00，宽度:32
 寄存器说明: This register is used to control the write leveling delay line.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 8;  /* bit[0-7]  : reserved */
        unsigned int  wdqphase : 5;  /* bit[8-12] : Write data eye clock phase select. */
        unsigned int  reserved_1: 19; /* bit[13-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNWDQDLY_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQDLY_wdqphase_START  (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWDQDLY_wdqphase_END    (12)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNWLSL_UNION
 结构说明  : DDRC_PACK_DXNWLSL 寄存器结构定义。地址偏移量:0x238+(n)*0x80，初值:0x00010000，宽度:32
 寄存器说明: This reister is used to control if PHY controller add extra system latency.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 16; /* bit[0-15] : reserved */
        unsigned int  wlsl     : 2;  /* bit[16-17]: Write Leveling System Latency.
                                                    This is used to adjust the write latency after write leveling. */
        unsigned int  reserved_1: 14; /* bit[18-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNWLSL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWLSL_wlsl_START      (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWLSL_wlsl_END        (17)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNGDS_UNION
 结构说明  : DDRC_PACK_DXNGDS 寄存器结构定义。地址偏移量:0x23C+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: This register is used to control the latch enable within the PHY to get the stable data.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gds      : 3;  /* bit[0-2] : Rank0 Read Data Latch Delays.
                                                   This field is used as a reference stable region for the PHY to latch the read data. */
        unsigned int  reserved : 29; /* bit[3-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNGDS_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNGDS_gds_START       (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNGDS_gds_END         (2)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_UNION
 结构说明  : DDRC_PACK_RANK0DXNRDQSGDLY 寄存器结构定义。地址偏移量:0x240+(n)*0x80，初值:0x00000010，宽度:32
 寄存器说明: This register is used to control the local delay line.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rdqsgbdl    : 6;  /* bit[0-5]  : Read DQS Gateing Delay Select.
                                                       This field specify the delay value of the read gating delay after DQS dummy PAD. */
        unsigned int  reserved_0  : 2;  /* bit[6-7]  : reserved */
        unsigned int  rdqsgphase  : 6;  /* bit[8-13] : Read DQS Gating Clock Phase Select. */
        unsigned int  reserved_1  : 2;  /* bit[14-15]: reserved */
        unsigned int  rdqsgtxbdl  : 5;  /* bit[16-20]: Read DQS Gate Delay Select.
                                                       This field specify the delay value of the read gating delay before DQS dummy PAD. */
        unsigned int  reserved_2  : 3;  /* bit[21-23]: reserved */
        unsigned int  org_rdqsgph : 6;  /* bit[24-29]: Original Read DQS Gate phase before HW GATE training post-update stage. (debug purpose). */
        unsigned int  reserved_3  : 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_rdqsgbdl_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_rdqsgbdl_END       (5)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_rdqsgphase_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_rdqsgphase_END     (13)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_rdqsgtxbdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_rdqsgtxbdl_END     (20)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_org_rdqsgph_START  (24)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK0DXNRDQSGDLY_org_rdqsgph_END    (29)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_UNION
 结构说明  : DDRC_PACK_RANK1DXNRDQSGDLY 寄存器结构定义。地址偏移量:0x640+(n)*0x80，初值:0x00000010，宽度:32
 寄存器说明: This register is used to control the local delay line.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rdqsgbdl    : 6;  /* bit[0-5]  : Read DQS Gateing Delay Select.
                                                       This field specify the delay value of the read gating delay after DQS dummy PAD. */
        unsigned int  reserved_0  : 2;  /* bit[6-7]  : reserved */
        unsigned int  rdqsgphase  : 6;  /* bit[8-13] : Read DQS Gating Clock Phase Select. */
        unsigned int  reserved_1  : 2;  /* bit[14-15]: reserved */
        unsigned int  rdqsgtxbdl  : 5;  /* bit[16-20]: Read DQS Gate Delay Select.
                                                       This field specify the delay value of the read gating delay before DQS dummy PAD. */
        unsigned int  reserved_2  : 3;  /* bit[21-23]: reserved */
        unsigned int  org_rdqsgph : 6;  /* bit[24-29]: Original Read DQS Gate phase before HW GATE training post-update stage. (debug purpose). */
        unsigned int  reserved_3  : 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_rdqsgbdl_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_rdqsgbdl_END       (5)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_rdqsgphase_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_rdqsgphase_END     (13)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_rdqsgtxbdl_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_rdqsgtxbdl_END     (20)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_org_rdqsgph_START  (24)
#define SOC_MDDRC_PACK_DDRC_PACK_RANK1DXNRDQSGDLY_org_rdqsgph_END    (29)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNCLKBDL_UNION
 结构说明  : DDRC_PACK_DXNCLKBDL 寄存器结构定义。地址偏移量:0x24C+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: DATA block clock bit delay line setting
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dx_fbclk_bdl  : 5;  /* bit[0-4]  : Bit delay line setting of FBCLK */
        unsigned int  reserved_0    : 11; /* bit[5-15] : reserved */
        unsigned int  dx_refclk_bdl : 5;  /* bit[16-20]: Bit delay line setting of REFCLK */
        unsigned int  reserved_1    : 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNCLKBDL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCLKBDL_dx_fbclk_bdl_START   (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCLKBDL_dx_fbclk_bdl_END     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCLKBDL_dx_refclk_bdl_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNCLKBDL_dx_refclk_bdl_END    (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNRDBOUND_UNION
 结构说明  : DDRC_PACK_DXNRDBOUND 寄存器结构定义。地址偏移量:0x250+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: Read data eye boundary. This register indicates the left/right boundary of RDQSQDL of the data eye.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rdqsbdl_right : 7;  /* bit[0-6]  : Right boundary of read data eye. */
        unsigned int  reserved_0    : 9;  /* bit[7-15] : reserved */
        unsigned int  rdqsbdl_left  : 7;  /* bit[16-22]: Left boundary of read data eye. */
        unsigned int  reserved_1    : 9;  /* bit[23-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNRDBOUND_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDBOUND_rdqsbdl_right_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDBOUND_rdqsbdl_right_END    (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDBOUND_rdqsbdl_left_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNRDBOUND_rdqsbdl_left_END     (22)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNWRBOUND_UNION
 结构说明  : DDRC_PACK_DXNWRBOUND 寄存器结构定义。地址偏移量:0x254+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: Write data eye boundary. This register indicates the left/right boundary of WDQSQDL of the data eye.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdqphase_right : 5;  /* bit[0-4]  : Right boundary of the write data eye. */
        unsigned int  reserved_0     : 11; /* bit[5-15] : reserved */
        unsigned int  wdqphase_left  : 5;  /* bit[16-20]: Left boundary of the write data eye. */
        unsigned int  reserved_1     : 11; /* bit[21-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNWRBOUND_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWRBOUND_wdqphase_right_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWRBOUND_wdqphase_right_END    (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWRBOUND_wdqphase_left_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNWRBOUND_wdqphase_left_END     (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_UNION
 结构说明  : DDRC_PACK_DXPHYCTRL 寄存器结构定义。地址偏移量:0x258+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: DX PHY control registers 
            Register in this field are connected to PHY interface directly
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dx_pll_thrm    : 1;  /* bit[0]    : dx pll control. Default set to 0 */
        unsigned int  dx_pll_testpem : 1;  /* bit[1]    : dx pll control. Default set to 0 */
        unsigned int  dx_pll_testen  : 1;  /* bit[2]    : dx pll control. Default set to 0 */
        unsigned int  dx_pll_init    : 1;  /* bit[3]    : dx pll control. Default set to 0 */
        unsigned int  dx_pll_cpi     : 3;  /* bit[4-6]  : dx pll control. Default set to 0 */
        unsigned int  dx_pll_sp      : 3;  /* bit[7-9]  : dx pll control. Default set to 0 */
        unsigned int  dx_pll_lt      : 2;  /* bit[10-11]: dx pll control. Default set to 0 */
        unsigned int  dx_pll_calt    : 1;  /* bit[12]   : dx pll control. Default set to 0 */
        unsigned int  dx_pll_calm    : 1;  /* bit[13]   : dx pll control. Default set to 0 */
        unsigned int  dx_pll_cal     : 1;  /* bit[14]   : dx pll control. Default set to 0 */
        unsigned int  reserved       : 17; /* bit[15-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_thrm_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_thrm_END       (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_testpem_START  (1)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_testpem_END    (1)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_testen_START   (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_testen_END     (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_init_START     (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_init_END       (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_cpi_START      (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_cpi_END        (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_sp_START       (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_sp_END         (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_lt_START       (10)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_lt_END         (11)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_calt_START     (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_calt_END       (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_calm_START     (13)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_calm_END       (13)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_cal_START      (14)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYCTRL_dx_pll_cal_END        (14)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_UNION
 结构说明  : DDRC_PACK_DXNMISCCTRL0 寄存器结构定义。地址偏移量:0x260+(n)*0x80，初值:0x00020000，宽度:32
 寄存器说明: Data block PHY miscellaneous control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rxn_2nd_dq    : 8;  /* bit[0-7]  : After modify this field, it need the &quot;phyconn_rst&quot; to update the latest setting. When user want to use dly_track_type=1 tracking type, this field should be set as 0xFF before initial trainings and dly_track_type should be set as 0x1 after initial trainings. */
        unsigned int  rxp_2nd_dq    : 8;  /* bit[8-15] : After modify this field, it need the &quot;phyconn_rst&quot; to update the latest setting. When user want to use dly_track_type=1 tracking type, this field should be set as 0xFF before initial trainings and dly_track_type should be set as 0x1 after initial trainings. */
        unsigned int  dx_margin_a1  : 1;  /* bit[16]   : After modify this field, it need the &quot;phyconn_rst&quot; to update the latest setting. When user want to use dly_track_type=1 tracking type, this field should be set as 0x1 before initial trainings and dly_track_type should be set as 0x1 after initial trainings. */
        unsigned int  dx_dqs_l      : 1;  /* bit[17]   : phy control . Default set to 1 */
        unsigned int  dx_dqs_h      : 1;  /* bit[18]   : phy control . Default set to 0 */
        unsigned int  dx_rxn_2nd_dm : 1;  /* bit[19]   : After modify this field, it need the &quot;phyconn_rst&quot; to update the latest setting. When user want to use dly_track_type=1 tracking type, this field should be set as 0x1 before initial trainings and dly_track_type should be set as 0x1 after initial trainings. */
        unsigned int  dx_rxp_2nd_dm : 1;  /* bit[20]   : After modify this field, it need the &quot;phyconn_rst&quot; to update the latest setting. When user want to use dly_track_type=1 tracking type, this field should be set as 0x1 before initial trainings and dly_track_type should be set as 0x1 after initial trainings. */
        unsigned int  dqsgatedla    : 1;  /* bit[21]   : phy control . Default set to 0 */
        unsigned int  lvdqclkdiv2   : 1;  /* bit[22]   : phy control . Default set to 0 */
        unsigned int  bufphyclkdiv2 : 1;  /* bit[23]   : phy control . Default set to 0 */
        unsigned int  margin_a0     : 1;  /* bit[24]   : After modify this field, it need the &quot;phyconn_rst&quot; to update the latest setting. When user want to use dly_track_type=1 tracking type, this field should be set as 0x1 before initial trainings and dly_track_type should be set as 0x1 after initial trainings. */
        unsigned int  enshift_a0    : 1;  /* bit[25]   : phy control . Default set to 0 */
        unsigned int  ptrgated_en   : 1;  /* bit[26]   : phy control . Default set to 0 */
        unsigned int  reserved      : 5;  /* bit[27-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_rxn_2nd_dq_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_rxn_2nd_dq_END       (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_rxp_2nd_dq_START     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_rxp_2nd_dq_END       (15)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_margin_a1_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_margin_a1_END     (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_dqs_l_START       (17)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_dqs_l_END         (17)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_dqs_h_START       (18)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_dqs_h_END         (18)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_rxn_2nd_dm_START  (19)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_rxn_2nd_dm_END    (19)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_rxp_2nd_dm_START  (20)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dx_rxp_2nd_dm_END    (20)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dqsgatedla_START     (21)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_dqsgatedla_END       (21)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_lvdqclkdiv2_START    (22)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_lvdqclkdiv2_END      (22)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_bufphyclkdiv2_START  (23)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_bufphyclkdiv2_END    (23)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_margin_a0_START      (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_margin_a0_END        (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_enshift_a0_START     (25)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_enshift_a0_END       (25)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_ptrgated_en_START    (26)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL0_ptrgated_en_END      (26)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_UNION
 结构说明  : DDRC_PACK_DXNMISCCTRL1 寄存器结构定义。地址偏移量:0x264+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: Data block PHY miscellaneous control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mclk2x    : 4;  /* bit[0-3]  : phy control ,set to 0 for Balong */
        unsigned int  mclk1x    : 4;  /* bit[4-7]  : phy control ,set to 0 for Balong */
        unsigned int  dqsclk2x  : 4;  /* bit[8-11] : phy control ,set to 0 for Balong */
        unsigned int  dqsclk1x  : 4;  /* bit[12-15]: phy control ,set to 0 for Balong */
        unsigned int  dqsgclk2x : 4;  /* bit[16-19]: phy control ,set to 0 for Balong */
        unsigned int  dqsgclk1x : 4;  /* bit[20-23]: phy control ,set to 0 for Balong */
        unsigned int  dqclk2x   : 4;  /* bit[24-27]: phy control ,set to 0 for Balong */
        unsigned int  dqclk1x   : 4;  /* bit[28-31]: phy control ,set to 0 for Balong */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_mclk2x_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_mclk2x_END       (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_mclk1x_START     (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_mclk1x_END       (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqsclk2x_START   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqsclk2x_END     (11)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqsclk1x_START   (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqsclk1x_END     (15)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqsgclk2x_START  (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqsgclk2x_END    (19)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqsgclk1x_START  (20)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqsgclk1x_END    (23)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqclk2x_START    (24)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqclk2x_END      (27)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqclk1x_START    (28)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL1_dqclk1x_END      (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_UNION
 结构说明  : DDRC_PACK_DXDEBUG0 寄存器结构定义。地址偏移量:0x268+(n)*0x80，初值:0x00005415，宽度:32
 寄存器说明: Data block PHY debug signals
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dqs_cb    : 3;  /* bit[0-2]  : Debug signals : dqs_cb */
        unsigned int  dqs_ca    : 3;  /* bit[3-5]  : Debug signals : dqs_ca */
        unsigned int  dqs_rdcnt : 3;  /* bit[6-8]  : Debug signals : dqs_rdcnt */
        unsigned int  s2b       : 1;  /* bit[9]    : Debug signals : s2b */
        unsigned int  s2a       : 1;  /* bit[10]   : Debug signals : s2a */
        unsigned int  s1b       : 1;  /* bit[11]   : Debug signals : s1b */
        unsigned int  s1a       : 1;  /* bit[12]   : Debug signals : s1a */
        unsigned int  s0b       : 1;  /* bit[13]   : Debug signals : s0b */
        unsigned int  s0a       : 1;  /* bit[14]   : Debug signals : s0a */
        unsigned int  reserved  : 17; /* bit[15-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_dqs_cb_START     (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_dqs_cb_END       (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_dqs_ca_START     (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_dqs_ca_END       (5)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_dqs_rdcnt_START  (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_dqs_rdcnt_END    (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s2b_START        (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s2b_END          (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s2a_START        (10)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s2a_END          (10)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s1b_START        (11)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s1b_END          (11)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s1a_START        (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s1a_END          (12)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s0b_START        (13)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s0b_END          (13)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s0a_START        (14)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG0_s0a_END          (14)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_UNION
 结构说明  : DDRC_PACK_DXDEBUG1 寄存器结构定义。地址偏移量:0x26C+(n)*0x80，初值:0x00000001，宽度:32
 寄存器说明: Data block PHY debug signals
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dqs0_gold : 8;  /* bit[0-7]  : Debug signals : dqs0_gold */
        unsigned int  dqs0b     : 8;  /* bit[8-15] : Debug signals : dqs0b */
        unsigned int  dqs0      : 8;  /* bit[16-23]: Debug signals : dqs0 */
        unsigned int  reserved  : 8;  /* bit[24-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_dqs0_gold_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_dqs0_gold_END    (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_dqs0b_START      (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_dqs0b_END        (15)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_dqs0_START       (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUG1_dqs0_END         (23)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXPHYRSVD_UNION
 结构说明  : DDRC_PACK_DXPHYRSVD 寄存器结构定义。地址偏移量:0x270+(n)*0x80，初值:0x0000FF00，宽度:32
 寄存器说明: Data block PHY reserved control pins. This register is for PHY control and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dx_rsv_control : 16; /* bit[0-15] : Reserved registers for the future.
                                                          
                                                          The followings are for n=0,2:
                                                          Bit1 (reg_DQSG_extend_2T): 0/1:Extend DQSG 1T/2T dfi_clk, when reg_DQSG_extend_en=1
                                                          
                                                          Bit0 (reg_DQSG_extend_en): 0/1: Disable/Enable the DQSG extending function */
        unsigned int  dx_rsv_status  : 16; /* bit[16-31]: Reserved registers for the future. */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXPHYRSVD_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYRSVD_dx_rsv_control_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYRSVD_dx_rsv_control_END    (15)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYRSVD_dx_rsv_status_START   (16)
#define SOC_MDDRC_PACK_DDRC_PACK_DXPHYRSVD_dx_rsv_status_END     (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_UNION
 结构说明  : DDRC_PACK_DXNMISCCTRL2 寄存器结构定义。地址偏移量:0x274+(n)*0x80，初值:0x0000026F，宽度:32
 寄存器说明: Data block PHY miscellaneous control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  BUFRESETCONTN_gated_DQSGERROR : 1;  /* bit[0]    : phy control ,set to 1 for Balong */
        unsigned int  BUFRESETCONTN_gated_TDC       : 1;  /* bit[1]    : phy control ,set to 1 for Balong */
        unsigned int  reg_GDS_r1T_sel_16b           : 1;  /* bit[2]    : phy control ,set to 1 for Balong */
        unsigned int  reg_ODTEN_gated               : 1;  /* bit[3]    : phy control ,set to 1 for Balong */
        unsigned int  reg_dummypad_use              : 1;  /* bit[4]    : phy control ,set to 0 for Balong */
        unsigned int  reg_dynamic_PUPDEN_16b        : 1;  /* bit[5]    : phy control ,set to 1 for Balong */
        unsigned int  reg_sel_halfT_gated_16b       : 1;  /* bit[6]    : phy control ,set to 1 for Balong */
        unsigned int  reg_dqsglat1T_en              : 1;  /* bit[7]    : phy control ,set to 0 for Balong */
        unsigned int  reg_squeach_en                : 1;  /* bit[8]    : phy control ,set to 0 for Balong */
        unsigned int  reg_dqsg_TX_2path             : 1;  /* bit[9]    : phy control ,set to 1 for Balong */
        unsigned int  reg_OE_extend1T_en            : 1;  /* bit[10]   : phy control ,set to 0 for Balong */
        unsigned int  reserved                      : 21; /* bit[11-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_BUFRESETCONTN_gated_DQSGERROR_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_BUFRESETCONTN_gated_DQSGERROR_END    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_BUFRESETCONTN_gated_TDC_START        (1)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_BUFRESETCONTN_gated_TDC_END          (1)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_GDS_r1T_sel_16b_START            (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_GDS_r1T_sel_16b_END              (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_ODTEN_gated_START                (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_ODTEN_gated_END                  (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_dummypad_use_START               (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_dummypad_use_END                 (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_dynamic_PUPDEN_16b_START         (5)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_dynamic_PUPDEN_16b_END           (5)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_sel_halfT_gated_16b_START        (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_sel_halfT_gated_16b_END          (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_dqsglat1T_en_START               (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_dqsglat1T_en_END                 (7)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_squeach_en_START                 (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_squeach_en_END                   (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_dqsg_TX_2path_START              (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_dqsg_TX_2path_END                (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_OE_extend1T_en_START             (10)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNMISCCTRL2_reg_OE_extend1T_en_END               (10)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_UNION
 结构说明  : DDRC_PACK_DXDEBUGCONFIG 寄存器结构定义。地址偏移量:0x278+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: Data block PHY debug control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reg_dbg_REFFB_postdly : 1;  /* bit[0]   : phy control ,set to 1 for Balong */
        unsigned int  reg_dbg_REFFB_predly  : 1;  /* bit[1]   : phy control ,set to 1 for Balong */
        unsigned int  reg_dbg_pllclk_sel    : 1;  /* bit[2]   : phy control ,set to 1 for Balong */
        unsigned int  reg_dbg_prepost_sel   : 1;  /* bit[3]   : phy control ,set to 1 for Balong */
        unsigned int  DESKEW_regread        : 1;  /* bit[4]   : dx block phy debug signal: DESKEW_regread */
        unsigned int  reserved              : 27; /* bit[5-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_reg_dbg_REFFB_postdly_START  (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_reg_dbg_REFFB_postdly_END    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_reg_dbg_REFFB_predly_START   (1)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_reg_dbg_REFFB_predly_END     (1)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_reg_dbg_pllclk_sel_START     (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_reg_dbg_pllclk_sel_END       (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_reg_dbg_prepost_sel_START    (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_reg_dbg_prepost_sel_END      (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_DESKEW_regread_START         (4)
#define SOC_MDDRC_PACK_DDRC_PACK_DXDEBUGCONFIG_DESKEW_regread_END           (4)


/*****************************************************************************
 结构名    : SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_UNION
 结构说明  : DDRC_PACK_DXNDCC 寄存器结构定义。地址偏移量:0x27C+(n)*0x80，初值:0x00000000，宽度:32
 寄存器说明: Data block PHY DCC control register. This register is for PHY control, and should not be modified.
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dxctl_dq_dcc   : 3;  /* bit[0-2]  : [2]-dly_sel_xxx_DCC_16b
                                                          [1]-ctrlC_xxx_DCC_16b
                                                          [0]-ctrl2_xxx_DCC_16b */
        unsigned int  dxctl_dqsg_dcc : 3;  /* bit[3-5]  : [2]-dly_sel_xxx_DCC_16b
                                                          [1]-ctrlC_xxx_DCC_16b
                                                          [0]-ctrl2_xxx_DCC_16b */
        unsigned int  dxctl_dqs_dcc  : 3;  /* bit[6-8]  : [2]-dly_sel_xxx_DCC_16b
                                                          [1]-ctrlC_xxx_DCC_16b
                                                          [0]-ctrl2_xxx_DCC_16b */
        unsigned int  dxctl_mclk_dcc : 3;  /* bit[9-11] : [2]-dly_sel_xxx_DCC_16b
                                                          [1]-ctrlC_xxx_DCC_16b
                                                          [0]-ctrl2_xxx_DCC_16b */
        unsigned int  reserved       : 20; /* bit[12-31]: reserved */
    } reg;
} SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_UNION;
#endif
#define SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_dxctl_dq_dcc_START    (0)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_dxctl_dq_dcc_END      (2)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_dxctl_dqsg_dcc_START  (3)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_dxctl_dqsg_dcc_END    (5)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_dxctl_dqs_dcc_START   (6)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_dxctl_dqs_dcc_END     (8)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_dxctl_mclk_dcc_START  (9)
#define SOC_MDDRC_PACK_DDRC_PACK_DXNDCC_dxctl_mclk_dcc_END    (11)






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

#endif /* end of soc_mddrc_pack_interface.h */
