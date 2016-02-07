

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_SMMU_INTERFACE_H__
#define __SOC_SMMU_INTERFACE_H__

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
/* 寄存器说明：SMMU控制寄存器
   位域定义UNION结构:  SOC_SMMU_CTRL_UNION */
#define SOC_SMMU_CTRL_ADDR(base)                      ((base) + (0x0000))

/* 寄存器说明：SMMU使能寄存器
   位域定义UNION结构:  SOC_SMMU_ENABLE_UNION */
#define SOC_SMMU_ENABLE_ADDR(base)                    ((base) + (0x0004))

/* 寄存器说明：SMMU页表基址寄存器
   位域定义UNION结构:  SOC_SMMU_PTBR_UNION */
#define SOC_SMMU_PTBR_ADDR(base)                      ((base) + (0x0008))

/* 寄存器说明：SMMU Preload /Invalid起始地址寄存器
   位域定义UNION结构:  SOC_SMMU_START_UNION */
#define SOC_SMMU_START_ADDR(base)                     ((base) + (0x000C))

/* 寄存器说明：SMMU Preload /Invalid结束地址寄存器
   位域定义UNION结构:  SOC_SMMU_END_UNION */
#define SOC_SMMU_END_ADDR(base)                       ((base) + (0x0010))

/* 寄存器说明：SMMU中断屏蔽寄存器
   位域定义UNION结构:  SOC_SMMU_INTMASK_UNION */
#define SOC_SMMU_INTMASK_ADDR(base)                   ((base) + (0x0014))

/* 寄存器说明：SMMU原始中断状态寄存器
   位域定义UNION结构:  SOC_SMMU_RINTSTS_UNION */
#define SOC_SMMU_RINTSTS_ADDR(base)                   ((base) + (0x0018))

/* 寄存器说明：SMMU屏蔽后中断状态寄存器
   位域定义UNION结构:  SOC_SMMU_MINTSTS_UNION */
#define SOC_SMMU_MINTSTS_ADDR(base)                   ((base) + (0x001C))

/* 寄存器说明：SMMU中断清除寄存器
   位域定义UNION结构:  SOC_SMMU_INTCLR_UNION */
#define SOC_SMMU_INTCLR_ADDR(base)                    ((base) + (0x0020))

/* 寄存器说明：SMMU状态寄存器
   位域定义UNION结构:  SOC_SMMU_STATUS_UNION */
#define SOC_SMMU_STATUS_ADDR(base)                    ((base) + (0x0024))

/* 寄存器说明：SMMU替换操作AXI ID寄存器
   位域定义UNION结构:  SOC_SMMU_AXIID_UNION */
#define SOC_SMMU_AXIID_ADDR(base)                     ((base) + (0x0028))

/* 寄存器说明：SMMU计数器控制寄存器
   位域定义UNION结构:  SOC_SMMU_CNTCTRL_UNION */
#define SOC_SMMU_CNTCTRL_ADDR(base)                   ((base) + (0x002C))

/* 寄存器说明：SMMU AXI操作计数器
   位域定义UNION结构:  SOC_SMMU_TRANSCNT_UNION */
#define SOC_SMMU_TRANSCNT_ADDR(base)                  ((base) + (0x0030))

/* 寄存器说明：SMMU L0 TLB命中计数器
   位域定义UNION结构:  SOC_SMMU_L0TLBHITCNT_UNION */
#define SOC_SMMU_L0TLBHITCNT_ADDR(base)               ((base) + (0x0034))

/* 寄存器说明：SMMU L1 TLB命中计数器
   位域定义UNION结构:  SOC_SMMU_L1TLBHITCNT_UNION */
#define SOC_SMMU_L1TLBHITCNT_ADDR(base)               ((base) + (0x0038))

/* 寄存器说明：SMMU 计数器卷回寄存器
   位域定义UNION结构:  SOC_SMMU_WRAPCNT_UNION */
#define SOC_SMMU_WRAPCNT_ADDR(base)                   ((base) + (0x003C))

/* 寄存器说明：SMMU安全区域起始地址寄存器
   位域定义UNION结构:  SOC_SMMU_SEC_START_UNION */
#define SOC_SMMU_SEC_START_ADDR(base)                 ((base) + (0x0040))

/* 寄存器说明：SMMU安全区域结束地址寄存器
   位域定义UNION结构:  SOC_SMMU_SEC_END_UNION */
#define SOC_SMMU_SEC_END_ADDR(base)                   ((base) + (0x0044))

/* 寄存器说明：SMMU版本寄存器
   位域定义UNION结构:  SOC_SMMU_VERSION_UNION */
#define SOC_SMMU_VERSION_ADDR(base)                   ((base) + (0x0048))

/* 寄存器说明：SMMU Invalid页表项地址寄存器
   位域定义UNION结构:  SOC_SMMU_IPTSRC_UNION */
#define SOC_SMMU_IPTSRC_ADDR(base)                    ((base) + (0x004C))

/* 寄存器说明：SMMU Invalid页表项对应物理地址
   位域定义UNION结构:  SOC_SMMU_IPTPA_UNION */
#define SOC_SMMU_IPTPA_ADDR(base)                     ((base) + (0x0050))

/* 寄存器说明：SMMU TAG RAM基地址
   位域定义UNION结构:  SOC_SMMU_TRBA_UNION */
#define SOC_SMMU_TRBA_ADDR(base)                      ((base) + (0x0054))

/* 寄存器说明：SMMU bypass空间起始地址
   位域定义UNION结构:  SOC_SMMU_BYS_START_UNION */
#define SOC_SMMU_BYS_START_ADDR(base)                 ((base) + (0x0058))

/* 寄存器说明：SMMU bypass空间结束地址
   位域定义UNION结构:  SOC_SMMU_BYS_END_UNION */
#define SOC_SMMU_BYS_END_ADDR(base)                   ((base) + (0x005C))





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
 结构名    : SOC_SMMU_CTRL_UNION
 结构说明  : CTRL 寄存器结构定义。地址偏移量:0x0000，初值:0x00000000，宽度:32
 寄存器说明: SMMU控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  smmu_rst        : 1;  /* bit[0]    : SMMU逻辑复位使能。写1后复位SMMU内部所有逻辑，L1 TLB恢复invalid状态、状态机返回IDLE态、所有计数器清零、清除所有中断状态。完成复位后自清零。 */
        unsigned int  page_size       : 1;  /* bit[1]    : SMMU页表大小。当cfg_lock为高时无法修改。
                                                           1'b0: 4KB
                                                           1'b1: 8KB
                                                           注意：page_size/L1 TLB_size/ddr_size/ va_size必须有固定的对应关系，超出对应关系的部分则上报cfg_error_int，SMMU无法正常工作。具体的对应关系请参考用户手册。 */
        unsigned int  ddr_size        : 2;  /* bit[2-3]  : SMMU对应的最大DDR容量。当cfg_lock为高时无法修改。
                                                           2'b00: 512MB
                                                           2'b01: 1GB
                                                           2'b10: 2GB
                                                           2'b11: reserved */
        unsigned int  l1tlb_size      : 2;  /* bit[4-5]  : SMMU L1 TLB大小。当cfg_lock为高时无法修改。
                                                           2'b00: reserved
                                                           2'b01: 64KB
                                                           2'b10: 128KB
                                                           2'b11: reserved
                                                           注意：当mem_sel为1'b0时，固定为64KB。 */
        unsigned int  ion_size        : 2;  /* bit[6-7]  : SMMU对应的虚拟地址空间大小。只有当va_ratio为高时有效。当cfg_lock为高时无法修改。
                                                           2'b00: 128KB
                                                           2'b01: 256KB
                                                           2'b10: 512KB
                                                           2'b11: reserved */
        unsigned int  ion_partial_en  : 1;  /* bit[8]    : SMMU对应的虚拟地址空间是否与物理地址空间一致的选择。
                                                           1'b0: 虚拟地址空间与物理地址空间一致，va_size位无效。
                                                           1'b1: 虚拟地址空间小于物理地址空间，通过va_size配置选择。 */
        unsigned int  preload_en      : 1;  /* bit[9]    : SMMU Preload使能。根据SMMU_START和SMMU_END寄存器配置的起始/结束地址，把地址范围内的页表load到L1 TLB中。完成后自清零。 */
        unsigned int  invalid_en      : 1;  /* bit[10]   : SMMU Invalid使能。根据SMMU_START和SMMU_END寄存器配置的起始/结束地址，释放范围内的entry，所有valid位为0。完成后自清零。 */
        unsigned int  auto_invalid_en : 1;  /* bit[11]   : L1 TLB自动invalid操作使能。当使能自动invalid时，一旦使能L1 TLB，由硬件自动根据SMMU_START/SMMU_END寄存器提供的地址范围，将L1 TLB的valid bit清零，清零过程中L1 TLB不可用。
                                                           1'b0: 不使能
                                                           1'b1: 使能 */
        unsigned int  reserved_0      : 1;  /* bit[12]   : 保留。 */
        unsigned int  tag_bit_sel     : 2;  /* bit[13-14]: L1 TLB tag bit选择。仅在USE_TAG_RAM宏无效时有效，用于选择不同的地址段。当tag bit位宽为1bit时，仅低位有效，高位无效。当无需使用tag bit时需配置为2'b0。 */
        unsigned int  reserved_1      : 17; /* bit[15-31]: 保留。 */
    } reg;
} SOC_SMMU_CTRL_UNION;
#endif
#define SOC_SMMU_CTRL_smmu_rst_START         (0)
#define SOC_SMMU_CTRL_smmu_rst_END           (0)
#define SOC_SMMU_CTRL_page_size_START        (1)
#define SOC_SMMU_CTRL_page_size_END          (1)
#define SOC_SMMU_CTRL_ddr_size_START         (2)
#define SOC_SMMU_CTRL_ddr_size_END           (3)
#define SOC_SMMU_CTRL_l1tlb_size_START       (4)
#define SOC_SMMU_CTRL_l1tlb_size_END         (5)
#define SOC_SMMU_CTRL_ion_size_START         (6)
#define SOC_SMMU_CTRL_ion_size_END           (7)
#define SOC_SMMU_CTRL_ion_partial_en_START   (8)
#define SOC_SMMU_CTRL_ion_partial_en_END     (8)
#define SOC_SMMU_CTRL_preload_en_START       (9)
#define SOC_SMMU_CTRL_preload_en_END         (9)
#define SOC_SMMU_CTRL_invalid_en_START       (10)
#define SOC_SMMU_CTRL_invalid_en_END         (10)
#define SOC_SMMU_CTRL_auto_invalid_en_START  (11)
#define SOC_SMMU_CTRL_auto_invalid_en_END    (11)
#define SOC_SMMU_CTRL_tag_bit_sel_START      (13)
#define SOC_SMMU_CTRL_tag_bit_sel_END        (14)


/*****************************************************************************
 结构名    : SOC_SMMU_ENABLE_UNION
 结构说明  : ENABLE 寄存器结构定义。地址偏移量:0x0004，初值:0x00000000，宽度:32
 寄存器说明: SMMU使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  smmu_enable : 1;  /* bit[0]   : SMMU整体使能控制。高电平表示SMMU使能，低电平表示SMMU不使能，所有操作被bypass通过。 */
        unsigned int  ch0_enable  : 1;  /* bit[1]   : AXI通道0使能。高电平表示AXI通道0使能，低电平表示AXI通道0不使能，所有通过AXI通道0的操作被bypass通过。当smmu_enable为低电平时，固定为0。 */
        unsigned int  reserved    : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_SMMU_ENABLE_UNION;
#endif
#define SOC_SMMU_ENABLE_smmu_enable_START  (0)
#define SOC_SMMU_ENABLE_smmu_enable_END    (0)
#define SOC_SMMU_ENABLE_ch0_enable_START   (1)
#define SOC_SMMU_ENABLE_ch0_enable_END     (1)


/*****************************************************************************
 结构名    : SOC_SMMU_PTBR_UNION
 结构说明  : PTBR 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: SMMU页表基址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trans_base_addr : 32; /* bit[0-31]: SMMU页表基地址。 */
    } reg;
} SOC_SMMU_PTBR_UNION;
#endif
#define SOC_SMMU_PTBR_trans_base_addr_START  (0)
#define SOC_SMMU_PTBR_trans_base_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_START_UNION
 结构说明  : START 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: SMMU Preload /Invalid起始地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr_start : 32; /* bit[0-31]: SMMU需要Preload或invalid操作的entry的起始地址，配置preload_en或invalid_en实现对应操作。 */
    } reg;
} SOC_SMMU_START_UNION;
#endif
#define SOC_SMMU_START_addr_start_START  (0)
#define SOC_SMMU_START_addr_start_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_END_UNION
 结构说明  : END 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: SMMU Preload /Invalid结束地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr_end : 32; /* bit[0-31]: SMMU需要Preload或invalid操作的entry的结束地址，配置preload_en或invalid_en实现对应操作。
                                                   注：该地址本身不在操作范围内 */
    } reg;
} SOC_SMMU_END_UNION;
#endif
#define SOC_SMMU_END_addr_end_START  (0)
#define SOC_SMMU_END_addr_end_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_INTMASK_UNION
 结构说明  : INTMASK 寄存器结构定义。地址偏移量:0x0014，初值:0x00000000，宽度:32
 寄存器说明: SMMU中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cfg_error_int       : 1;  /* bit[0]   : 配置错误中断。指示page_size/L1 TLB_size/ddr_size三个参数没有按照对应关系配置。 */
        unsigned int  pt_invalid_int      : 1;  /* bit[1]   : 页表invalid中断。指示对应的页表为invalid状态。 */
        unsigned int  master0_err_int     : 1;  /* bit[2]   : AXI Master0接口接收到error response后产生的中断。 */
        unsigned int  master1_err_int     : 1;  /* bit[3]   : AXI Master1接口接收到error response后产生的中断。 */
        unsigned int  master0_timeout_int : 1;  /* bit[4]   : AXI Master0接口操作超时中断。未接收到对端返回的握手信号。 */
        unsigned int  master1_timeout_int : 1;  /* bit[5]   : AXI Master1接口操作超时中断。未接收到对端返回的握手信号。
                                                              
                                                              中断屏蔽位（下同）。
                                                              1'b0: 不屏蔽中断
                                                              1'b1: 屏蔽中断 */
        unsigned int  reserved            : 26; /* bit[6-31]: 保留。 */
    } reg;
} SOC_SMMU_INTMASK_UNION;
#endif
#define SOC_SMMU_INTMASK_cfg_error_int_START        (0)
#define SOC_SMMU_INTMASK_cfg_error_int_END          (0)
#define SOC_SMMU_INTMASK_pt_invalid_int_START       (1)
#define SOC_SMMU_INTMASK_pt_invalid_int_END         (1)
#define SOC_SMMU_INTMASK_master0_err_int_START      (2)
#define SOC_SMMU_INTMASK_master0_err_int_END        (2)
#define SOC_SMMU_INTMASK_master1_err_int_START      (3)
#define SOC_SMMU_INTMASK_master1_err_int_END        (3)
#define SOC_SMMU_INTMASK_master0_timeout_int_START  (4)
#define SOC_SMMU_INTMASK_master0_timeout_int_END    (4)
#define SOC_SMMU_INTMASK_master1_timeout_int_START  (5)
#define SOC_SMMU_INTMASK_master1_timeout_int_END    (5)


/*****************************************************************************
 结构名    : SOC_SMMU_RINTSTS_UNION
 结构说明  : RINTSTS 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: SMMU原始中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cfg_error_int       : 1;  /* bit[0]   : 配置错误中断。指示page_size/L1 TLB_size/ddr_size三个参数没有按照对应关系配置。 */
        unsigned int  pt_invalid_int      : 1;  /* bit[1]   : 页表invalid中断。指示对应的页表为invalid状态。 */
        unsigned int  master0_err_int     : 1;  /* bit[2]   : AXI Master0接口接收到error response后产生的中断。 */
        unsigned int  master1_err_int     : 1;  /* bit[3]   : AXI Master1接口接收到error response后产生的中断。 */
        unsigned int  master0_timeout_int : 1;  /* bit[4]   : AXI Master0接口操作超时中断。未接收到对端返回的握手信号。 */
        unsigned int  master1_timeout_int : 1;  /* bit[5]   : AXI Master1接口操作超时中断。未接收到对端返回的握手信号。
                                                              
                                                              屏蔽前原始中断状态（下同）。
                                                              1'b0: 没有中断
                                                              1'b1: 有中断 */
        unsigned int  reserved            : 26; /* bit[6-31]: 保留。 */
    } reg;
} SOC_SMMU_RINTSTS_UNION;
#endif
#define SOC_SMMU_RINTSTS_cfg_error_int_START        (0)
#define SOC_SMMU_RINTSTS_cfg_error_int_END          (0)
#define SOC_SMMU_RINTSTS_pt_invalid_int_START       (1)
#define SOC_SMMU_RINTSTS_pt_invalid_int_END         (1)
#define SOC_SMMU_RINTSTS_master0_err_int_START      (2)
#define SOC_SMMU_RINTSTS_master0_err_int_END        (2)
#define SOC_SMMU_RINTSTS_master1_err_int_START      (3)
#define SOC_SMMU_RINTSTS_master1_err_int_END        (3)
#define SOC_SMMU_RINTSTS_master0_timeout_int_START  (4)
#define SOC_SMMU_RINTSTS_master0_timeout_int_END    (4)
#define SOC_SMMU_RINTSTS_master1_timeout_int_START  (5)
#define SOC_SMMU_RINTSTS_master1_timeout_int_END    (5)


/*****************************************************************************
 结构名    : SOC_SMMU_MINTSTS_UNION
 结构说明  : MINTSTS 寄存器结构定义。地址偏移量:0x001C，初值:0x00000000，宽度:32
 寄存器说明: SMMU屏蔽后中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cfg_error_int       : 1;  /* bit[0]   : 配置错误中断。指示page_size/L1 TLB_size/ddr_size三个参数没有按照对应关系配置。 */
        unsigned int  pt_invalid_int      : 1;  /* bit[1]   : 页表invalid中断。指示对应的页表为invalid状态。 */
        unsigned int  master0_err_int     : 1;  /* bit[2]   : AXI Master0接口接收到error response后产生的中断。 */
        unsigned int  master1_err_int     : 1;  /* bit[3]   : AXI Master1接口接收到error response后产生的中断。 */
        unsigned int  master0_timeout_int : 1;  /* bit[4]   : AXI Master0接口操作超时中断。未接收到对端返回的握手信号。 */
        unsigned int  master1_timeout_int : 1;  /* bit[5]   : AXI Master1接口操作超时中断。未接收到对端返回的握手信号。
                                                              
                                                              屏蔽后中断状态（下同）。
                                                              1'b0: 没有中断
                                                              1'b1: 有中断 */
        unsigned int  reserved            : 26; /* bit[6-31]: 保留。 */
    } reg;
} SOC_SMMU_MINTSTS_UNION;
#endif
#define SOC_SMMU_MINTSTS_cfg_error_int_START        (0)
#define SOC_SMMU_MINTSTS_cfg_error_int_END          (0)
#define SOC_SMMU_MINTSTS_pt_invalid_int_START       (1)
#define SOC_SMMU_MINTSTS_pt_invalid_int_END         (1)
#define SOC_SMMU_MINTSTS_master0_err_int_START      (2)
#define SOC_SMMU_MINTSTS_master0_err_int_END        (2)
#define SOC_SMMU_MINTSTS_master1_err_int_START      (3)
#define SOC_SMMU_MINTSTS_master1_err_int_END        (3)
#define SOC_SMMU_MINTSTS_master0_timeout_int_START  (4)
#define SOC_SMMU_MINTSTS_master0_timeout_int_END    (4)
#define SOC_SMMU_MINTSTS_master1_timeout_int_START  (5)
#define SOC_SMMU_MINTSTS_master1_timeout_int_END    (5)


/*****************************************************************************
 结构名    : SOC_SMMU_INTCLR_UNION
 结构说明  : INTCLR 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: SMMU中断清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cfg_error_int       : 1;  /* bit[0]   : 配置错误中断。指示page_size/L1 TLB_size/ddr_size三个参数没有按照对应关系配置。 */
        unsigned int  pt_invalid_int      : 1;  /* bit[1]   : 页表invalid中断。指示对应的页表为invalid状态。 */
        unsigned int  master0_err_int     : 1;  /* bit[2]   : AXI Master0接口接收到error response后产生的中断。 */
        unsigned int  master1_err_int     : 1;  /* bit[3]   : AXI Master1接口接收到error response后产生的中断。 */
        unsigned int  master0_timeout_int : 1;  /* bit[4]   : AXI Master0接口操作超时中断。未接收到对端返回的握手信号。 */
        unsigned int  master1_timeout_int : 1;  /* bit[5]   : AXI Master1接口操作超时中断。未接收到对端返回的握手信号。
                                                              
                                                              写1清除中断，清除后自清零。 */
        unsigned int  reserved            : 26; /* bit[6-31]: 保留。 */
    } reg;
} SOC_SMMU_INTCLR_UNION;
#endif
#define SOC_SMMU_INTCLR_cfg_error_int_START        (0)
#define SOC_SMMU_INTCLR_cfg_error_int_END          (0)
#define SOC_SMMU_INTCLR_pt_invalid_int_START       (1)
#define SOC_SMMU_INTCLR_pt_invalid_int_END         (1)
#define SOC_SMMU_INTCLR_master0_err_int_START      (2)
#define SOC_SMMU_INTCLR_master0_err_int_END        (2)
#define SOC_SMMU_INTCLR_master1_err_int_START      (3)
#define SOC_SMMU_INTCLR_master1_err_int_END        (3)
#define SOC_SMMU_INTCLR_master0_timeout_int_START  (4)
#define SOC_SMMU_INTCLR_master0_timeout_int_END    (4)
#define SOC_SMMU_INTCLR_master1_timeout_int_START  (5)
#define SOC_SMMU_INTCLR_master1_timeout_int_END    (5)


/*****************************************************************************
 结构名    : SOC_SMMU_STATUS_UNION
 结构说明  : STATUS 寄存器结构定义。地址偏移量:0x0024，初值:0x00000080，宽度:32
 寄存器说明: SMMU状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  l0tlb_fsm        : 3;  /* bit[0-2]  : L0 TLB状态机状态：
                                                            3'b000: IDLE
                                                            3'b001: JUDGE
                                                            3'b010: HIT
                                                            3'b011: REPLACE_REQ
                                                            3'b100: REPLACE_WAIT */
        unsigned int  axi_fifo_full    : 1;  /* bit[3]    : 地址转换后操作输出FIFO满状态指示。 */
        unsigned int  rpl_fifo_full    : 1;  /* bit[4]    : 替换操作返回的读数据FIFO满状态指示。 */
        unsigned int  master0_buf_full : 1;  /* bit[5]    : Master0操作Buf满状态指示。 */
        unsigned int  master1_buf_full : 1;  /* bit[6]    : Master1操作Buf满状态指示。 */
        unsigned int  bypass           : 1;  /* bit[7]    : SMMU bypass状态
                                                            0: SMMU enable
                                                            1: SMMU bypass */
        unsigned int  cfg_error        : 1;  /* bit[8]    : 为1表示page size/ddr size/L1 TLB size三个参数没有按照对应关系配置。 */
        unsigned int  cfg_lock         : 1;  /* bit[9]    : 为1表示page size/ddr size/L1 TLB size/ ram_latency几个参数寄存器被锁定。当L0 TLB/L1 TLB的状态机第一次跳转开始，则进入lock状态。软件只有配置SMMU_rst后，cfg_lock置0，参数才能重新配置。 */
        unsigned int  reserved         : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_SMMU_STATUS_UNION;
#endif
#define SOC_SMMU_STATUS_l0tlb_fsm_START         (0)
#define SOC_SMMU_STATUS_l0tlb_fsm_END           (2)
#define SOC_SMMU_STATUS_axi_fifo_full_START     (3)
#define SOC_SMMU_STATUS_axi_fifo_full_END       (3)
#define SOC_SMMU_STATUS_rpl_fifo_full_START     (4)
#define SOC_SMMU_STATUS_rpl_fifo_full_END       (4)
#define SOC_SMMU_STATUS_master0_buf_full_START  (5)
#define SOC_SMMU_STATUS_master0_buf_full_END    (5)
#define SOC_SMMU_STATUS_master1_buf_full_START  (6)
#define SOC_SMMU_STATUS_master1_buf_full_END    (6)
#define SOC_SMMU_STATUS_bypass_START            (7)
#define SOC_SMMU_STATUS_bypass_END              (7)
#define SOC_SMMU_STATUS_cfg_error_START         (8)
#define SOC_SMMU_STATUS_cfg_error_END           (8)
#define SOC_SMMU_STATUS_cfg_lock_START          (9)
#define SOC_SMMU_STATUS_cfg_lock_END            (9)


/*****************************************************************************
 结构名    : SOC_SMMU_AXIID_UNION
 结构说明  : AXIID 寄存器结构定义。地址偏移量:0x0028，初值:0x00000000，宽度:32
 寄存器说明: SMMU替换操作AXI ID寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  smmu_axiid   : 12; /* bit[0-11] : 当发生替换操作时AXI总线发出的ID号，必须与其他Master ID区分，否则会发生操作错误。 */
        unsigned int  rpl_sideband : 18; /* bit[12-29]: SMMU替换操作的sideband信息，用于配置替换操作的master id，QoS等与DDR访问优先级相关的内容。 */
        unsigned int  l0_bypass    : 1;  /* bit[30]   : L0 TLB bypass选择。
                                                        1'b0：非bypass
                                                        1'b1：bypass */
        unsigned int  s128_bypass  : 1;  /* bit[31]   : Replace same page 128功能bypass选择。
                                                        1'b0：非bypass
                                                        1'b1：bypass */
    } reg;
} SOC_SMMU_AXIID_UNION;
#endif
#define SOC_SMMU_AXIID_smmu_axiid_START    (0)
#define SOC_SMMU_AXIID_smmu_axiid_END      (11)
#define SOC_SMMU_AXIID_rpl_sideband_START  (12)
#define SOC_SMMU_AXIID_rpl_sideband_END    (29)
#define SOC_SMMU_AXIID_l0_bypass_START     (30)
#define SOC_SMMU_AXIID_l0_bypass_END       (30)
#define SOC_SMMU_AXIID_s128_bypass_START   (31)
#define SOC_SMMU_AXIID_s128_bypass_END     (31)


/*****************************************************************************
 结构名    : SOC_SMMU_CNTCTRL_UNION
 结构说明  : CNTCTRL 寄存器结构定义。地址偏移量:0x002C，初值:0x00000000，宽度:32
 寄存器说明: SMMU计数器控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  count_enable : 1;  /* bit[0]   : SMMU统计使能。写1后启动SMMU_TRANSCNT /L0TLBHITCNT/L1TLBHITCNT三个计数器，用于统计命中率。写0关闭计数器。 */
        unsigned int  count_clear  : 1;  /* bit[1]   : SMMU计数器清除。写1后清除所有计数器的内容，计数器归零。完成清除后自清零。 */
        unsigned int  reserved     : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_SMMU_CNTCTRL_UNION;
#endif
#define SOC_SMMU_CNTCTRL_count_enable_START  (0)
#define SOC_SMMU_CNTCTRL_count_enable_END    (0)
#define SOC_SMMU_CNTCTRL_count_clear_START   (1)
#define SOC_SMMU_CNTCTRL_count_clear_END     (1)


/*****************************************************************************
 结构名    : SOC_SMMU_TRANSCNT_UNION
 结构说明  : TRANSCNT 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: SMMU AXI操作计数器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  axi_trans_count : 32; /* bit[0-31]: AXI操作计数器，用于统计经过AXI Slave输入的有效的操作数量，不在页表范围内的不统计。 */
    } reg;
} SOC_SMMU_TRANSCNT_UNION;
#endif
#define SOC_SMMU_TRANSCNT_axi_trans_count_START  (0)
#define SOC_SMMU_TRANSCNT_axi_trans_count_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_L0TLBHITCNT_UNION
 结构说明  : L0TLBHITCNT 寄存器结构定义。地址偏移量:0x0034，初值:0x00000000，宽度:32
 寄存器说明: SMMU L0 TLB命中计数器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  l0tlb_hit_count : 32; /* bit[0-31]: L0 TLB命中计数器，用于统计L0 TLB命中的次数。 */
    } reg;
} SOC_SMMU_L0TLBHITCNT_UNION;
#endif
#define SOC_SMMU_L0TLBHITCNT_l0tlb_hit_count_START  (0)
#define SOC_SMMU_L0TLBHITCNT_l0tlb_hit_count_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_L1TLBHITCNT_UNION
 结构说明  : L1TLBHITCNT 寄存器结构定义。地址偏移量:0x0038，初值:0x00000000，宽度:32
 寄存器说明: SMMU L1 TLB命中计数器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  l1tlb_hit_count : 32; /* bit[0-31]: L1 TLB命中计数器，用于统计L1 TLB命中的次数。 */
    } reg;
} SOC_SMMU_L1TLBHITCNT_UNION;
#endif
#define SOC_SMMU_L1TLBHITCNT_l1tlb_hit_count_START  (0)
#define SOC_SMMU_L1TLBHITCNT_l1tlb_hit_count_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_WRAPCNT_UNION
 结构说明  : WRAPCNT 寄存器结构定义。地址偏移量:0x003C，初值:0x00000000，宽度:32
 寄存器说明: SMMU 计数器卷回寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  axi_trans_count_wrap : 4;  /* bit[0-3]  : AXI操作计数器的卷回统计 */
        unsigned int  l0tlb_hit_count_wrap : 4;  /* bit[4-7]  : L0 TLB命中计数器的卷回统计 */
        unsigned int  l1tlb_hit_count_wrap : 4;  /* bit[8-11] : L1 TLB命中计数器的卷回统计 */
        unsigned int  reserved             : 20; /* bit[12-31]: 保留。 */
    } reg;
} SOC_SMMU_WRAPCNT_UNION;
#endif
#define SOC_SMMU_WRAPCNT_axi_trans_count_wrap_START  (0)
#define SOC_SMMU_WRAPCNT_axi_trans_count_wrap_END    (3)
#define SOC_SMMU_WRAPCNT_l0tlb_hit_count_wrap_START  (4)
#define SOC_SMMU_WRAPCNT_l0tlb_hit_count_wrap_END    (7)
#define SOC_SMMU_WRAPCNT_l1tlb_hit_count_wrap_START  (8)
#define SOC_SMMU_WRAPCNT_l1tlb_hit_count_wrap_END    (11)


/*****************************************************************************
 结构名    : SOC_SMMU_SEC_START_UNION
 结构说明  : SEC_START 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: SMMU安全区域起始地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sec_addr_start : 32; /* bit[0-31]: DDR中安全区域的起始地址，访问该区域的操作不经过SMMU地址转换。该寄存器仅在安全访问下可见，非安全访问返回0。 */
    } reg;
} SOC_SMMU_SEC_START_UNION;
#endif
#define SOC_SMMU_SEC_START_sec_addr_start_START  (0)
#define SOC_SMMU_SEC_START_sec_addr_start_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_SEC_END_UNION
 结构说明  : SEC_END 寄存器结构定义。地址偏移量:0x0044，初值:0x00000000，宽度:32
 寄存器说明: SMMU安全区域结束地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sec_addr_end : 32; /* bit[0-31]: DDR中安全区域的结束地址，访问该区域的操作不经过SMMU地址转换。该寄存器仅在安全访问下可见，非安全访问返回0。
                                                       注：该地址本身不在操作范围内 */
    } reg;
} SOC_SMMU_SEC_END_UNION;
#endif
#define SOC_SMMU_SEC_END_sec_addr_end_START  (0)
#define SOC_SMMU_SEC_END_sec_addr_end_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_VERSION_UNION
 结构说明  : VERSION 寄存器结构定义。地址偏移量:0x0048，初值:0x48690100，宽度:32
 寄存器说明: SMMU版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  smmu_version : 32; /* bit[0-31]: SMMU IP版本寄存器。 */
    } reg;
} SOC_SMMU_VERSION_UNION;
#endif
#define SOC_SMMU_VERSION_smmu_version_START  (0)
#define SOC_SMMU_VERSION_smmu_version_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_IPTSRC_UNION
 结构说明  : IPTSRC 寄存器结构定义。地址偏移量:0x004C，初值:0x00000000，宽度:32
 寄存器说明: SMMU Invalid页表项地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  smmu_ipt_src : 32; /* bit[0-31]: 当pt_invalid_int中断产生时，存入当前的invalid页表项的地址。 */
    } reg;
} SOC_SMMU_IPTSRC_UNION;
#endif
#define SOC_SMMU_IPTSRC_smmu_ipt_src_START  (0)
#define SOC_SMMU_IPTSRC_smmu_ipt_src_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_IPTPA_UNION
 结构说明  : IPTPA 寄存器结构定义。地址偏移量:0x0050，初值:0xFFFFF000，宽度:32
 寄存器说明: SMMU Invalid页表项对应物理地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  smmu_ipt_pa : 32; /* bit[0-31]: 提供当pt_invalid_int中断产生时对应的物理地址，SMMU会丢弃invalid页表，而是用该地址发出总线操作。 */
    } reg;
} SOC_SMMU_IPTPA_UNION;
#endif
#define SOC_SMMU_IPTPA_smmu_ipt_pa_START  (0)
#define SOC_SMMU_IPTPA_smmu_ipt_pa_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_TRBA_UNION
 结构说明  : TRBA 寄存器结构定义。地址偏移量:0x0054，初值:0x00000000，宽度:32
 寄存器说明: SMMU TAG RAM基地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  smmu_tagram_baseaddr : 32; /* bit[0-31]: SMMU L1 TLB TAG RAM基地址。SMMU内部根据该地址重映射RAM零地址。 */
    } reg;
} SOC_SMMU_TRBA_UNION;
#endif
#define SOC_SMMU_TRBA_smmu_tagram_baseaddr_START  (0)
#define SOC_SMMU_TRBA_smmu_tagram_baseaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_BYS_START_UNION
 结构说明  : BYS_START 寄存器结构定义。地址偏移量:0x0058，初值:0x00000000，宽度:32
 寄存器说明: SMMU bypass空间起始地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bypass_start_addr : 32; /* bit[0-31]: SMMU bypass空间起始地址，访问该区域的操作不经过SMMU地址转换。 */
    } reg;
} SOC_SMMU_BYS_START_UNION;
#endif
#define SOC_SMMU_BYS_START_bypass_start_addr_START  (0)
#define SOC_SMMU_BYS_START_bypass_start_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SMMU_BYS_END_UNION
 结构说明  : BYS_END 寄存器结构定义。地址偏移量:0x005C，初值:0x00000000，宽度:32
 寄存器说明: SMMU bypass空间结束地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bypass_end_addr : 32; /* bit[0-31]: SMMU bypass空间结束地址，访问该区域的操作不经过SMMU地址转换。
                                                          注：该地址本身不在操作范围内 */
    } reg;
} SOC_SMMU_BYS_END_UNION;
#endif
#define SOC_SMMU_BYS_END_bypass_end_addr_START  (0)
#define SOC_SMMU_BYS_END_bypass_end_addr_END    (31)






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

#endif /* end of soc_smmu_interface.h */
