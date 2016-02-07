

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_MMC_INTERFACE_H__
#define __SOC_MMC_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) MMC
 ****************************************************************************/
/* 寄存器说明：控制寄存器。
   位域定义UNION结构:  SOC_MMC_CTRL_UNION */
#define SOC_MMC_CTRL_ADDR(base)                       ((base) + (0x0000))

/* 寄存器说明：接口时钟分频因子寄存器。
   位域定义UNION结构:  SOC_MMC_CLKDIV_UNION */
#define SOC_MMC_CLKDIV_ADDR(base)                     ((base) + (0x0008))

/* 寄存器说明：MMC接口时钟使能寄存器。
   位域定义UNION结构:  SOC_MMC_CLKENA_UNION */
#define SOC_MMC_CLKENA_ADDR(base)                     ((base) + (0x0010))

/* 寄存器说明：超时参数寄存器。
   位域定义UNION结构:  SOC_MMC_TMOUT_UNION */
#define SOC_MMC_TMOUT_ADDR(base)                      ((base) + (0x0014))

/* 寄存器说明：接口位宽配置寄存器。
   位域定义UNION结构:  SOC_MMC_CTYPE_UNION */
#define SOC_MMC_CTYPE_ADDR(base)                      ((base) + (0x0018))

/* 寄存器说明：块大小配置寄存器。
   位域定义UNION结构:  SOC_MMC_BLKSIZ_UNION */
#define SOC_MMC_BLKSIZ_ADDR(base)                     ((base) + (0x001C))

/* 寄存器说明：传输数据大小寄存器。
   位域定义UNION结构:  SOC_MMC_BYTCNT_UNION */
#define SOC_MMC_BYTCNT_ADDR(base)                     ((base) + (0x0020))

/* 寄存器说明：中断屏蔽寄存器。
   位域定义UNION结构:  SOC_MMC_INTMASK_UNION */
#define SOC_MMC_INTMASK_ADDR(base)                    ((base) + (0x0024))

/* 寄存器说明：指令参数寄存器。
   位域定义UNION结构:  SOC_MMC_CMDARG_UNION */
#define SOC_MMC_CMDARG_ADDR(base)                     ((base) + (0x0028))

/* 寄存器说明：指令寄存器。
   位域定义UNION结构:  SOC_MMC_CMD_UNION */
#define SOC_MMC_CMD_ADDR(base)                        ((base) + (0x002C))

/* 寄存器说明：指令响应寄存器。
   位域定义UNION结构:  SOC_MMC_RESP0_UNION */
#define SOC_MMC_RESP0_ADDR(base)                      ((base) + (0x0030))

/* 寄存器说明：指令响应寄存器。
   位域定义UNION结构:  SOC_MMC_RESP1_UNION */
#define SOC_MMC_RESP1_ADDR(base)                      ((base) + (0x0034))

/* 寄存器说明：指令响应寄存器。
   位域定义UNION结构:  SOC_MMC_RESP2_UNION */
#define SOC_MMC_RESP2_ADDR(base)                      ((base) + (0x0038))

/* 寄存器说明：指令响应寄存器。
   位域定义UNION结构:  SOC_MMC_RESP3_UNION */
#define SOC_MMC_RESP3_ADDR(base)                      ((base) + (0x003C))

/* 寄存器说明：屏蔽后的中断状态寄存器。
   位域定义UNION结构:  SOC_MMC_MINTSTS_UNION */
#define SOC_MMC_MINTSTS_ADDR(base)                    ((base) + (0x0040))

/* 寄存器说明：原始中断状态寄存器。
   位域定义UNION结构:  SOC_MMC_RINTSTS_UNION */
#define SOC_MMC_RINTSTS_ADDR(base)                    ((base) + (0x0044))

/* 寄存器说明：MMC状态寄存器。
   位域定义UNION结构:  SOC_MMC_STATUS_UNION */
#define SOC_MMC_STATUS_ADDR(base)                     ((base) + (0x0048))

/* 寄存器说明：MMC FIFO参数寄存器。
   位域定义UNION结构:  SOC_MMC_FIFOTH_UNION */
#define SOC_MMC_FIFOTH_ADDR(base)                     ((base) + (0x004C))

/* 寄存器说明：接口传输计数寄存器。
   位域定义UNION结构:  SOC_MMC_TCBCNT_UNION */
#define SOC_MMC_TCBCNT_ADDR(base)                     ((base) + (0x005C))

/* 寄存器说明：FIFO传输计数寄存器。
   位域定义UNION结构:  SOC_MMC_TBBCNT_UNION */
#define SOC_MMC_TBBCNT_ADDR(base)                     ((base) + (0x0060))

/* 寄存器说明：去抖计数寄存器。
   位域定义UNION结构:  SOC_MMC_DEBNCE_UNION */
#define SOC_MMC_DEBNCE_ADDR(base)                     ((base) + (0x0064))

/* 寄存器说明：版本寄存器。
   位域定义UNION结构:  SOC_MMC_VERID_UNION */
#define SOC_MMC_VERID_ADDR(base)                      ((base) + (0x006C))

/* 寄存器说明：UHS-1寄存器。
   位域定义UNION结构:  SOC_MMC_UHSREG_UNION */
#define SOC_MMC_UHSREG_ADDR(base)                     ((base) + (0x0074))

/* 寄存器说明：硬件复位寄存器。
   位域定义UNION结构:  SOC_MMC_RST_n_UNION */
#define SOC_MMC_RST_n_ADDR(base)                      ((base) + (0x0078))

/* 寄存器说明：总线模式寄存器
   位域定义UNION结构:  SOC_MMC_BMOD_UNION */
#define SOC_MMC_BMOD_ADDR(base)                       ((base) + (0x0080))

/* 寄存器说明：投票寄存器。
   位域定义UNION结构:  SOC_MMC_PLDMND_UNION */
#define SOC_MMC_PLDMND_ADDR(base)                     ((base) + (0x0084))

/* 寄存器说明：链表地址寄存器。
   位域定义UNION结构:  SOC_MMC_DBADDR_UNION */
#define SOC_MMC_DBADDR_ADDR(base)                     ((base) + (0x0088))

/* 寄存器说明：IDMAC状态寄存器。
   位域定义UNION结构:  SOC_MMC_IDSTS_UNION */
#define SOC_MMC_IDSTS_ADDR(base)                      ((base) + (0x008C))

/* 寄存器说明：IDMAC中断使能寄存器
   位域定义UNION结构:  SOC_MMC_IDINTEN_UNION */
#define SOC_MMC_IDINTEN_ADDR(base)                    ((base) + (0x0090))

/* 寄存器说明：当前Discriptor地址寄存器
   位域定义UNION结构:  SOC_MMC_DSCADDR_UNION */
#define SOC_MMC_DSCADDR_ADDR(base)                    ((base) + (0x0094))

/* 寄存器说明：当前Buffer地址寄存器。
   位域定义UNION结构:  SOC_MMC_BUFADDR_UNION */
#define SOC_MMC_BUFADDR_ADDR(base)                    ((base) + (0x0098))

/* 寄存器说明：读卡FIFO门限寄存器
   位域定义UNION结构:  SOC_MMC_CARDTHRCTL_UNION */
#define SOC_MMC_CARDTHRCTL_ADDR(base)                 ((base) + (0x0100))

/* 寄存器说明：UHS寄存器
   位域定义UNION结构:  SOC_MMC_UHS_REG_EXT_UNION */
#define SOC_MMC_UHS_REG_EXT_ADDR(base)                ((base) + (0x0108))

/* 寄存器说明：eMMC4.5 DDR模式起始位控制寄存器
   位域定义UNION结构:  SOC_MMC_EMMC_DDR_REG_UNION */
#define SOC_MMC_EMMC_DDR_REG_ADDR(base)               ((base) + (0x010C))

/* 寄存器说明：移相使能寄存器
   位域定义UNION结构:  SOC_MMC_ENABLE_SHIFT_UNION */
#define SOC_MMC_ENABLE_SHIFT_ADDR(base)               ((base) + (0x0110))





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
                     (1/1) MMC
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_MMC_CTRL_UNION
 结构说明  : CTRL 寄存器结构定义。地址偏移量:0x0000，初值:0x00000000，宽度:32
 寄存器说明: 控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  controller_reset  : 1;  /* bit[0]    : 0：不改变
                                                             1：复位 MMC/SD/SDIO HOST IP 模块
                                                             设 1 使能模块复位。复位在两个 cclk_in 周期内被自动复位。复位包括：
                                                             l BIU/CIU 模块
                                                             l CIU 和所有状态机
                                                             l Abort_read_data, send_irq_response, 和 read_wait 比特位
                                                             l Start_cmd 比特位
                                                             不影响其他寄存器值，DMA接口，FIFO 或 中断。 */
        unsigned int  fifo_reset        : 1;  /* bit[1]    : FIFO复位控制位。
                                                             0：不复位FIFO读写指针；
                                                             1：复位FIFO读写指针。
                                                             写1使能复位，该bit在完成复位后自动清0。 */
        unsigned int  dma_reset         : 1;  /* bit[2]    : DMA接口功能复位控制。
                                                             0：不复位；
                                                             1：复位。
                                                             写1使能复位，该bit在完成复位后自动清0。 */
        unsigned int  reserved_0        : 1;  /* bit[3]    : 保留。默认为0，不可写1。 */
        unsigned int  int_enable        : 1;  /* bit[4]    : 全局中断使能位。
                                                             0：禁止；
                                                             1：使能。
                                                             只有该bit有效且有中断源被使能的情况下，中断输出才有效。 */
        unsigned int  dma_enable        : 1;  /* bit[5]    : DMA传输模式使能位。
                                                             0：禁止；
                                                             1：使能。
                                                             即使DMA模式已经使能，CPU仍然可以对FIFO进行读写，但在实际操作中应该避免。如果DMA和CPU同时对FIFO进行读写，控制器未仲裁其优先级。 */
        unsigned int  read_wait         : 1;  /* bit[6]    : 0：清除读等待
                                                             1：使能读等待
                                                             发送读等待到 SDIO。 */
        unsigned int  send_irq_response : 1;  /* bit[7]    : 自动IRQ响应控制。
                                                             0：默认保持值；
                                                             1：发送自动IRQ响应。
                                                             该bit在发送响应后自动清0。
                                                             为了等待MMC产生中断，控制器发送CMD40并等待来自MMC中断响应。同时，如果控制器希望不再停留在中断等待状态，可以使能该bit，送出CMD40响应并回到IDLE状态。 */
        unsigned int  abort_read_data   : 1;  /* bit[8]    : 0：不改变
                                                             1：当在读操作过程中输出 suspend 指令后，软件会让卡去查找问么时候 suspend 出现。一旦suspend出现，软件把该bit使能使数据传输状态机回到初始状态IDLE，即回到等待下一块数据传输。当状态机回到 IDLE 后，该比特会自动被清掉。 */
        unsigned int  reserved_1        : 16; /* bit[9-24] :  */
        unsigned int  use_internal_dmac : 1;  /* bit[25]   : 0：外部host接口通过slave 接口传输数据
                                                             1：使用内部DMA传输数据 */
        unsigned int  reserved_2        : 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_MMC_CTRL_UNION;
#endif
#define SOC_MMC_CTRL_controller_reset_START   (0)
#define SOC_MMC_CTRL_controller_reset_END     (0)
#define SOC_MMC_CTRL_fifo_reset_START         (1)
#define SOC_MMC_CTRL_fifo_reset_END           (1)
#define SOC_MMC_CTRL_dma_reset_START          (2)
#define SOC_MMC_CTRL_dma_reset_END            (2)
#define SOC_MMC_CTRL_int_enable_START         (4)
#define SOC_MMC_CTRL_int_enable_END           (4)
#define SOC_MMC_CTRL_dma_enable_START         (5)
#define SOC_MMC_CTRL_dma_enable_END           (5)
#define SOC_MMC_CTRL_read_wait_START          (6)
#define SOC_MMC_CTRL_read_wait_END            (6)
#define SOC_MMC_CTRL_send_irq_response_START  (7)
#define SOC_MMC_CTRL_send_irq_response_END    (7)
#define SOC_MMC_CTRL_abort_read_data_START    (8)
#define SOC_MMC_CTRL_abort_read_data_END      (8)
#define SOC_MMC_CTRL_use_internal_dmac_START  (25)
#define SOC_MMC_CTRL_use_internal_dmac_END    (25)


/*****************************************************************************
 结构名    : SOC_MMC_CLKDIV_UNION
 结构说明  : CLKDIV 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: 接口时钟分频因子寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_divider : 8;  /* bit[0-7] : 接口时钟分频系数，偶数分频。
                                                      如0x0为无分频、0x1为2分频、0xFF为510分频。
                                                      只有当MMC_CMD[start_cmd]和MMC_CMD[Update_clk_only]置1，本寄存器的值才会被载入。 */
        unsigned int  reserved    : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_MMC_CLKDIV_UNION;
#endif
#define SOC_MMC_CLKDIV_clk_divider_START  (0)
#define SOC_MMC_CLKDIV_clk_divider_END    (7)


/*****************************************************************************
 结构名    : SOC_MMC_CLKENA_UNION
 结构说明  : CLKENA 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: MMC接口时钟使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cclk_enable    : 1;  /* bit[0]    : 卡的时钟使能控制。
                                                          0：时钟关闭；
                                                          1：时钟使能。 */
        unsigned int  reserved       : 15; /* bit[1-15] : 保留。 */
        unsigned int  cclk_low_power : 16; /* bit[16-31]: 卡的低功耗控制。
                                                          0：非低功耗模式；
                                                          1：低功耗模式。
                                                          当卡处于IDLE状态，控制器自动关闭接口时钟。该功能一般只使用在MMC、SD存储卡。 */
    } reg;
} SOC_MMC_CLKENA_UNION;
#endif
#define SOC_MMC_CLKENA_cclk_enable_START     (0)
#define SOC_MMC_CLKENA_cclk_enable_END       (0)
#define SOC_MMC_CLKENA_cclk_low_power_START  (16)
#define SOC_MMC_CLKENA_cclk_low_power_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_TMOUT_UNION
 结构说明  : TMOUT 寄存器结构定义。地址偏移量:0x0014，初值:0xFFFFFF40，宽度:32
 寄存器说明: 超时参数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  response_timeout : 8;  /* bit[0-7] : 响应超时参数。单位为接口时钟周期，建议值为0xFF。 */
        unsigned int  data_timeout     : 24; /* bit[8-31]: 数据读操作超时参数，该值也做data starvation中断超时参数。单位为接口时钟周期，建议设置为0xFF_FFFF。 */
    } reg;
} SOC_MMC_TMOUT_UNION;
#endif
#define SOC_MMC_TMOUT_response_timeout_START  (0)
#define SOC_MMC_TMOUT_response_timeout_END    (7)
#define SOC_MMC_TMOUT_data_timeout_START      (8)
#define SOC_MMC_TMOUT_data_timeout_END        (31)


/*****************************************************************************
 结构名    : SOC_MMC_CTYPE_UNION
 结构说明  : CTYPE 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: 接口位宽配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  card_width_l : 1;  /* bit[0]    : 配置卡接口的总线宽度。
                                                        0：1bit模式；
                                                        1：4bit模式。 */
        unsigned int  reserved_0   : 15; /* bit[1-15] : 保留。 */
        unsigned int  card_width   : 1;  /* bit[16]   : 配置卡的总线宽度。
                                                        0：非 8-bit 模式
                                                        1：8-bit 模式 */
        unsigned int  reserved_1   : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_MMC_CTYPE_UNION;
#endif
#define SOC_MMC_CTYPE_card_width_l_START  (0)
#define SOC_MMC_CTYPE_card_width_l_END    (0)
#define SOC_MMC_CTYPE_card_width_START    (16)
#define SOC_MMC_CTYPE_card_width_END      (16)


/*****************************************************************************
 结构名    : SOC_MMC_BLKSIZ_UNION
 结构说明  : BLKSIZ 寄存器结构定义。地址偏移量:0x001C，初值:0x00000200，宽度:32
 寄存器说明: 块大小配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  block_size : 16; /* bit[0-15] : 块大小配置。
                                                      如配置为0x0200，表示块大小为512byte。 */
        unsigned int  reserved   : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_MMC_BLKSIZ_UNION;
#endif
#define SOC_MMC_BLKSIZ_block_size_START  (0)
#define SOC_MMC_BLKSIZ_block_size_END    (15)


/*****************************************************************************
 结构名    : SOC_MMC_BYTCNT_UNION
 结构说明  : BYTCNT 寄存器结构定义。地址偏移量:0x0020，初值:0x00000200，宽度:32
 寄存器说明: 传输数据大小寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  byte_count : 32; /* bit[0-31]: 待传输数据字节数。
                                                     如配置为0x0200，表示待传输数据为512byte。块传输方式时应该设置为block size的整数倍。 */
    } reg;
} SOC_MMC_BYTCNT_UNION;
#endif
#define SOC_MMC_BYTCNT_byte_count_START  (0)
#define SOC_MMC_BYTCNT_byte_count_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_INTMASK_UNION
 结构说明  : INTMASK 寄存器结构定义。地址偏移量:0x0024，初值:0x00000000，宽度:32
 寄存器说明: 中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0    : 1;  /* bit[0]    : 保留。 */
        unsigned int  re_int_mask   : 1;  /* bit[1]    : RE（Response Error）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  dd_int_mask   : 1;  /* bit[2]    : CD（Command Done）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  dto_int_mask  : 1;  /* bit[3]    : DTO（Data Transfer Over）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  txdr_int_mask : 1;  /* bit[4]    : TXDR（Transmit FIFO Data Request）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  rxdr_int_mask : 1;  /* bit[5]    : RXDR（Receive FIFO Data Request）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  rcrc_int_mask : 1;  /* bit[6]    : RCRC（Response CRC error）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  dcrc_int_mask : 1;  /* bit[7]    : DCRC（Data CRC error）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  rto_int_mask  : 1;  /* bit[8]    : RTO（Response Timeout）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  drto_int_mask : 1;  /* bit[9]    : DRTO（Data Read Timeout）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  hto_int_mask  : 1;  /* bit[10]   : HTO（Data starvation-by-host timeout /Volt_Switch_int）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  frun_int_mask : 1;  /* bit[11]   : FRUN（FIFO underrun/overrun error）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  hle_int_mask  : 1;  /* bit[12]   : HLE（Hardware Locked Write Error）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  sbe_int_mask  : 1;  /* bit[13]   : SBE（Start-bit Error）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  acd_int_mask  : 1;  /* bit[14]   : ACD（Auto Command Done）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  ebe_int_mask  : 1;  /* bit[15]   : EBE（End-bit error）中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  sdio_int_mask : 1;  /* bit[16]   : SDIO 中断屏蔽。
                                                         0：屏蔽该中断；
                                                         1：不屏蔽该中断。 */
        unsigned int  reserved_1    : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_MMC_INTMASK_UNION;
#endif
#define SOC_MMC_INTMASK_re_int_mask_START    (1)
#define SOC_MMC_INTMASK_re_int_mask_END      (1)
#define SOC_MMC_INTMASK_dd_int_mask_START    (2)
#define SOC_MMC_INTMASK_dd_int_mask_END      (2)
#define SOC_MMC_INTMASK_dto_int_mask_START   (3)
#define SOC_MMC_INTMASK_dto_int_mask_END     (3)
#define SOC_MMC_INTMASK_txdr_int_mask_START  (4)
#define SOC_MMC_INTMASK_txdr_int_mask_END    (4)
#define SOC_MMC_INTMASK_rxdr_int_mask_START  (5)
#define SOC_MMC_INTMASK_rxdr_int_mask_END    (5)
#define SOC_MMC_INTMASK_rcrc_int_mask_START  (6)
#define SOC_MMC_INTMASK_rcrc_int_mask_END    (6)
#define SOC_MMC_INTMASK_dcrc_int_mask_START  (7)
#define SOC_MMC_INTMASK_dcrc_int_mask_END    (7)
#define SOC_MMC_INTMASK_rto_int_mask_START   (8)
#define SOC_MMC_INTMASK_rto_int_mask_END     (8)
#define SOC_MMC_INTMASK_drto_int_mask_START  (9)
#define SOC_MMC_INTMASK_drto_int_mask_END    (9)
#define SOC_MMC_INTMASK_hto_int_mask_START   (10)
#define SOC_MMC_INTMASK_hto_int_mask_END     (10)
#define SOC_MMC_INTMASK_frun_int_mask_START  (11)
#define SOC_MMC_INTMASK_frun_int_mask_END    (11)
#define SOC_MMC_INTMASK_hle_int_mask_START   (12)
#define SOC_MMC_INTMASK_hle_int_mask_END     (12)
#define SOC_MMC_INTMASK_sbe_int_mask_START   (13)
#define SOC_MMC_INTMASK_sbe_int_mask_END     (13)
#define SOC_MMC_INTMASK_acd_int_mask_START   (14)
#define SOC_MMC_INTMASK_acd_int_mask_END     (14)
#define SOC_MMC_INTMASK_ebe_int_mask_START   (15)
#define SOC_MMC_INTMASK_ebe_int_mask_END     (15)
#define SOC_MMC_INTMASK_sdio_int_mask_START  (16)
#define SOC_MMC_INTMASK_sdio_int_mask_END    (16)


/*****************************************************************************
 结构名    : SOC_MMC_CMDARG_UNION
 结构说明  : CMDARG 寄存器结构定义。地址偏移量:0x0028，初值:0x00000000，宽度:32
 寄存器说明: 指令参数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cmd_arg : 32; /* bit[0-31]: 配置传输给卡的指令参数。
                                                  指令参数与协议相关，每个指令都对应一个具体的指令参数。 */
    } reg;
} SOC_MMC_CMDARG_UNION;
#endif
#define SOC_MMC_CMDARG_cmd_arg_START  (0)
#define SOC_MMC_CMDARG_cmd_arg_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_CMD_UNION
 结构说明  : CMD 寄存器结构定义。地址偏移量:0x002C，初值:0x00000000，宽度:32
 寄存器说明: 指令寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cmd_index                   : 6;  /* bit[0-5]  : 指令序号。 */
        unsigned int  response_expect             : 1;  /* bit[6]    : 有无响应指令选择。
                                                                       0：无响应指令；
                                                                       1：有响应指令。 */
        unsigned int  response_length             : 1;  /* bit[7]    : 响应长度。
                                                                       0：短响应指令；
                                                                       1：长响应指令。
                                                                       长response是128bit，短response是32bit。 */
        unsigned int  check_repsonse_crc          : 1;  /* bit[8]    : CRC检查。
                                                                       0：不检查指令响应CRC；
                                                                       1：检查指令响应CRC。
                                                                       一些指令回复没有返回有效的CRC。为了禁止控制器对CRC进行检查，软件需要针对这些指令禁止该功能。 */
        unsigned int  data_transfer_expected      : 1;  /* bit[9]    : 指令类型。
                                                                       0：非数据指令；
                                                                       1：数据指令。 */
        unsigned int  read_write                  : 1;  /* bit[10]   : 读写控制。
                                                                       0：从卡读取数据；
                                                                       1：往卡写数据。
                                                                       在非数据传输时，该位被忽略。 */
        unsigned int  transfer_mode               : 1;  /* bit[11]   : 传输模式。
                                                                       0：用于块读写指令；
                                                                       1：用于流数据读写指令。
                                                                       在非数据传输时，该位被忽略。 */
        unsigned int  send_auto_stop              : 1;  /* bit[12]   : 自动停止发送指令控制。
                                                                       0：数据传完以后不会发送停止指令；
                                                                       1：数据传完以后发送停止指令。
                                                                       当被使能以后，控制器在每次数据传输完以后就会自动送出停止指令。
                                                                       predefined block count方式、cmd53等不要停止指令。
                                                                       open-ended传输方式需要停止指令。
                                                                       在非数据传输时，该位被忽略。 */
        unsigned int  wait_prvdata_complete       : 1;  /* bit[13]   : 指令发送时机选择。
                                                                       0：立即发送指令（即使前一个数据传输还没完成）；
                                                                       1：等到前一个数据传输完成后才开始发送指令。
                                                                       在指令发送时将该位配置为0可用来在数据传输时读取卡状态或中止本次传输。 */
        unsigned int  stop_abort_cmd              : 1;  /* bit[14]   : 数据读写状态机复位控制位。
                                                                       0：无影响；
                                                                       1：在数据传输过程中发送停止指令异常中止本次传输时将该位置1，用于将控制器从数据传输状态恢复到IDLE状态。 */
        unsigned int  send_initialization         : 1;  /* bit[15]   : 发送初始化控制。
                                                                       0：在发送该指令前不要送出初始序列；
                                                                       1：在发送该指令前送出初始序列（80个时钟周期）。
                                                                       在上电以后，卡需要80个时钟周期进行初始化，因此需要在往卡发送第一条指令时设置该位。 */
        unsigned int  card_number                 : 5;  /* bit[16-20]: 正在使用的卡的序号，应设置为0。 */
        unsigned int  update_clock_registes_only_ : 1;  /* bit[21]   : 指令顺序控制。
                                                                       0：正常指令顺序；
                                                                       1：不发送命令，重新载入接口时钟控制寄存器（MMC_CLKDIV、MMC_CLKENA）的值。在不需要发指令给卡的前提下，用来调整接口时钟的频率和控制接口时钟的开关。
                                                                       在正常命令顺序中，即该位置为0，下面的寄存器值会被控制器载入：MMC_CMD、MMC_CMDARG、MMC_TMOUT、MMC_CTYPE、MMC_BLKSIZ、MMC_BYTCNT。控制器将新的寄存器值使用到新的指令中去。
                                                                       如果该bit设为1，指令不会传到卡中，不会产生Command Done中断。 */
        unsigned int  reserved_0                  : 2;  /* bit[22-23]: 保留。 */
        unsigned int  enable_boot                 : 1;  /* bit[24]   : 使能Boot模式。此Bit只在[mandatory boot mode]下有效。当此Bit与[start_cmd]一起置位时，CIU将通过拉低CMD信号线启动Boot操作。不能将[disable_boot]和[enable_boot]同时置位。 */
        unsigned int  expect_boot_ack             : 1;  /* bit[25]   : 使能Boot应答。当此Bit与[enable_boot]一起置位时，CIU期望设备发送过来以“0-1-0”为起始格式的应答信号。 */
        unsigned int  disable_boot                : 1;  /* bit[26]   : 不使能Boot模式。当此Bit与[start_cmd]一起置位时，CIU将终止Boot操作。不能将[disable_boot]和[enable_boot]同时置位。 */
        unsigned int  boot_mode                   : 1;  /* bit[27]   : Boot模式选择。
                                                                       0：mandatory boot mode；
                                                                       1：alternative boot mode。 */
        unsigned int  volt_switch                 : 1;  /* bit[28]   : 控制电压切换
                                                                       0：没有电压切换
                                                                       1：电压切换使能 */
        unsigned int  use_hold_reg                : 1;  /* bit[29]   : Hold 寄存器
                                                                       0：CMD和DATA传送不需要经过hold寄存器延迟。
                                                                       1：CMD和DATA传送需要经过hold寄存器延迟。
                                                                       （使用SDR12和SDR25模式时需要将此寄存器配置为1；使用SDR50和SDR100等高速模式时需要将此寄存器配置为0） */
        unsigned int  reserved_1                  : 1;  /* bit[30]   : 保留。 */
        unsigned int  start_cmd                   : 1;  /* bit[31]   : 指令执行或加载接口时钟参数启动位。
                                                                       与[update_clk_register_only]位配合使用，在[update_clk_register_only]为0时将该位置1，用于启动指令执行；在[update_clk_reg_only]为1时将该位置1，用于加载接口时钟参数。
                                                                       当指令被执行或时钟参数已载入，该bit自动清0。当该bit为1时，CPU不允许修改时钟和指令相关的寄存器。如果修改，hardware lock error中断就会产生。 */
    } reg;
} SOC_MMC_CMD_UNION;
#endif
#define SOC_MMC_CMD_cmd_index_START                    (0)
#define SOC_MMC_CMD_cmd_index_END                      (5)
#define SOC_MMC_CMD_response_expect_START              (6)
#define SOC_MMC_CMD_response_expect_END                (6)
#define SOC_MMC_CMD_response_length_START              (7)
#define SOC_MMC_CMD_response_length_END                (7)
#define SOC_MMC_CMD_check_repsonse_crc_START           (8)
#define SOC_MMC_CMD_check_repsonse_crc_END             (8)
#define SOC_MMC_CMD_data_transfer_expected_START       (9)
#define SOC_MMC_CMD_data_transfer_expected_END         (9)
#define SOC_MMC_CMD_read_write_START                   (10)
#define SOC_MMC_CMD_read_write_END                     (10)
#define SOC_MMC_CMD_transfer_mode_START                (11)
#define SOC_MMC_CMD_transfer_mode_END                  (11)
#define SOC_MMC_CMD_send_auto_stop_START               (12)
#define SOC_MMC_CMD_send_auto_stop_END                 (12)
#define SOC_MMC_CMD_wait_prvdata_complete_START        (13)
#define SOC_MMC_CMD_wait_prvdata_complete_END          (13)
#define SOC_MMC_CMD_stop_abort_cmd_START               (14)
#define SOC_MMC_CMD_stop_abort_cmd_END                 (14)
#define SOC_MMC_CMD_send_initialization_START          (15)
#define SOC_MMC_CMD_send_initialization_END            (15)
#define SOC_MMC_CMD_card_number_START                  (16)
#define SOC_MMC_CMD_card_number_END                    (20)
#define SOC_MMC_CMD_update_clock_registes_only__START  (21)
#define SOC_MMC_CMD_update_clock_registes_only__END    (21)
#define SOC_MMC_CMD_enable_boot_START                  (24)
#define SOC_MMC_CMD_enable_boot_END                    (24)
#define SOC_MMC_CMD_expect_boot_ack_START              (25)
#define SOC_MMC_CMD_expect_boot_ack_END                (25)
#define SOC_MMC_CMD_disable_boot_START                 (26)
#define SOC_MMC_CMD_disable_boot_END                   (26)
#define SOC_MMC_CMD_boot_mode_START                    (27)
#define SOC_MMC_CMD_boot_mode_END                      (27)
#define SOC_MMC_CMD_volt_switch_START                  (28)
#define SOC_MMC_CMD_volt_switch_END                    (28)
#define SOC_MMC_CMD_use_hold_reg_START                 (29)
#define SOC_MMC_CMD_use_hold_reg_END                   (29)
#define SOC_MMC_CMD_start_cmd_START                    (31)
#define SOC_MMC_CMD_start_cmd_END                      (31)


/*****************************************************************************
 结构名    : SOC_MMC_RESP0_UNION
 结构说明  : RESP0 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: 指令响应寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  response0 : 32; /* bit[0-31]: 48bit短响应的bit[39:8]或136bit长响应的bit[31:0]。 */
    } reg;
} SOC_MMC_RESP0_UNION;
#endif
#define SOC_MMC_RESP0_response0_START  (0)
#define SOC_MMC_RESP0_response0_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_RESP1_UNION
 结构说明  : RESP1 寄存器结构定义。地址偏移量:0x0034，初值:0x00000000，宽度:32
 寄存器说明: 指令响应寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  response1 : 32; /* bit[0-31]: 指令长响应bit[63:32]或控制器auto-stop指令的响应。
                                                    当控制器发出auto-stop指令，响应的bit[39:8]就会被保存在本寄存器。上一个指令的响应仍然会被保存在MMC_RESP0寄存器内。Auto-stop只供数据传输使用，而且回复的类型总是短响应。 */
    } reg;
} SOC_MMC_RESP1_UNION;
#endif
#define SOC_MMC_RESP1_response1_START  (0)
#define SOC_MMC_RESP1_response1_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_RESP2_UNION
 结构说明  : RESP2 寄存器结构定义。地址偏移量:0x0038，初值:0x00000000，宽度:32
 寄存器说明: 指令响应寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  response2 : 32; /* bit[0-31]: 指令长响应的bit[95:64]。 */
    } reg;
} SOC_MMC_RESP2_UNION;
#endif
#define SOC_MMC_RESP2_response2_START  (0)
#define SOC_MMC_RESP2_response2_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_RESP3_UNION
 结构说明  : RESP3 寄存器结构定义。地址偏移量:0x003C，初值:0x00000000，宽度:32
 寄存器说明: 指令响应寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  response3 : 32; /* bit[0-31]: 指令长响应bit[127:96]。 */
    } reg;
} SOC_MMC_RESP3_UNION;
#endif
#define SOC_MMC_RESP3_response3_START  (0)
#define SOC_MMC_RESP3_response3_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_MINTSTS_UNION
 结构说明  : MINTSTS 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后的中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 1;  /* bit[0]    : 保留。 */
        unsigned int  re_int   : 1;  /* bit[1]    : RE（Response Error）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  dd_int   : 1;  /* bit[2]    : CD（Command Done）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  dto_int  : 1;  /* bit[3]    : DTO（Data Transfer Over）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  txdr_int : 1;  /* bit[4]    : TXDR（Transmit FIFO Data Request）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  rxdr_int : 1;  /* bit[5]    : RXDR（Receive FIFO Data Request）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  rcrc_int : 1;  /* bit[6]    : RCRC（Response CRC error）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  dcrc_int : 1;  /* bit[7]    : DCRC（Data CRC error）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  rto_int  : 1;  /* bit[8]    : RTO（Response Timeout）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  drto_int : 1;  /* bit[9]    : DRTO（Data Read Timeout）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  hto_int  : 1;  /* bit[10]   : HTO（Data starvation-by-host timeout/Volt_switch_int）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  frun_int : 1;  /* bit[11]   : FRUN（FIFO underrun/overrun error）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  hle_int  : 1;  /* bit[12]   : HLE（Hardware Locked Write Error）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  sbe_int  : 1;  /* bit[13]   : SBE（Start-bit Error）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  acd_int  : 1;  /* bit[14]   : ACD（Auto Command Done）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  ebe_int  : 1;  /* bit[15]   : EBE（End-bit error）中断屏蔽。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  sdio_int : 1;  /* bit[16]   : SDIO中断。
                                                    0：无SDIO中断从卡输出；
                                                    1：有SDIO中断从卡输出。 */
        unsigned int  reserved_1: 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_MMC_MINTSTS_UNION;
#endif
#define SOC_MMC_MINTSTS_re_int_START    (1)
#define SOC_MMC_MINTSTS_re_int_END      (1)
#define SOC_MMC_MINTSTS_dd_int_START    (2)
#define SOC_MMC_MINTSTS_dd_int_END      (2)
#define SOC_MMC_MINTSTS_dto_int_START   (3)
#define SOC_MMC_MINTSTS_dto_int_END     (3)
#define SOC_MMC_MINTSTS_txdr_int_START  (4)
#define SOC_MMC_MINTSTS_txdr_int_END    (4)
#define SOC_MMC_MINTSTS_rxdr_int_START  (5)
#define SOC_MMC_MINTSTS_rxdr_int_END    (5)
#define SOC_MMC_MINTSTS_rcrc_int_START  (6)
#define SOC_MMC_MINTSTS_rcrc_int_END    (6)
#define SOC_MMC_MINTSTS_dcrc_int_START  (7)
#define SOC_MMC_MINTSTS_dcrc_int_END    (7)
#define SOC_MMC_MINTSTS_rto_int_START   (8)
#define SOC_MMC_MINTSTS_rto_int_END     (8)
#define SOC_MMC_MINTSTS_drto_int_START  (9)
#define SOC_MMC_MINTSTS_drto_int_END    (9)
#define SOC_MMC_MINTSTS_hto_int_START   (10)
#define SOC_MMC_MINTSTS_hto_int_END     (10)
#define SOC_MMC_MINTSTS_frun_int_START  (11)
#define SOC_MMC_MINTSTS_frun_int_END    (11)
#define SOC_MMC_MINTSTS_hle_int_START   (12)
#define SOC_MMC_MINTSTS_hle_int_END     (12)
#define SOC_MMC_MINTSTS_sbe_int_START   (13)
#define SOC_MMC_MINTSTS_sbe_int_END     (13)
#define SOC_MMC_MINTSTS_acd_int_START   (14)
#define SOC_MMC_MINTSTS_acd_int_END     (14)
#define SOC_MMC_MINTSTS_ebe_int_START   (15)
#define SOC_MMC_MINTSTS_ebe_int_END     (15)
#define SOC_MMC_MINTSTS_sdio_int_START  (16)
#define SOC_MMC_MINTSTS_sdio_int_END    (16)


/*****************************************************************************
 结构名    : SOC_MMC_RINTSTS_UNION
 结构说明  : RINTSTS 寄存器结构定义。地址偏移量:0x0044，初值:0x00000000，宽度:32
 寄存器说明: 原始中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0      : 1;  /* bit[0]    : 保留。 */
        unsigned int  re_int_status   : 1;  /* bit[1]    : 屏蔽前的RE（Response Error）中断。
                                                           0：未产生中断；
                                                           1：收到的指令响应有错误时，该中断产生。 */
        unsigned int  dd_int_status   : 1;  /* bit[2]    : 屏蔽前的CD（Command Done）中断。
                                                           0：未产生中断；
                                                           1：命令执行完毕并收到响应产生中断。即使出现response error、response CRC error或response timeout，该中断仍然产生。 */
        unsigned int  dto_int_status  : 1;  /* bit[3]    : 屏蔽后的DTO（Data Transfer Over）中断。
                                                           0：未产生中断；
                                                           1：数据传输完毕中断。即使出现了Start Bit Error、CRC error或Read Data Timeout，该中断仍然产生。 */
        unsigned int  txdr_int_status : 1;  /* bit[4]    : 屏蔽前的TXDR（Transmit FIFO Data Request）中断。
                                                           0：未产生中断；
                                                           1：当往卡写数据时，FIFO中数据少于或等于FIFO写阈值tx_wmark时产生中断。使用DMA进行数据传输时，应屏蔽该中断。 */
        unsigned int  rxdr_int_status : 1;  /* bit[5]    : 屏蔽前的RXDR（Receive FIFO Data Request）中断。
                                                           0：未产生中断；
                                                           1：当从卡读数据时，FIFO中数据多于FIFO读阈值rx_wmark时产生中断。使用DMA进行数据传输时，应屏蔽该中断。 */
        unsigned int  rcrc_int_status : 1;  /* bit[6]    : 屏蔽前的RCRC（Response CRC error）中断。
                                                           0：未产生中断；
                                                           1：指令响应CRC校验错误中断。 */
        unsigned int  dcrc_int_status : 1;  /* bit[7]    : 屏蔽前的DCRC（Data CRC error）中断。
                                                           0：未产生中断；
                                                           1：接收数据CRC校验错误中断。 */
        unsigned int  rto_int_status  : 1;  /* bit[8]    : 屏蔽前RTO（Response Timeout）中断。
                                                           0：未产生中断；
                                                           1：指令响应超时中断（未收到响应）。 */
        unsigned int  drto_int_status : 1;  /* bit[9]    : 屏蔽前的DRTO（Data Read Timeout）中断。
                                                           0：未产生中断；
                                                           1：接收数据超时中断。 */
        unsigned int  hto_int_status  : 1;  /* bit[10]   : 屏蔽前的Data starvation-by-host timeout/Volt_switch_int（HTO）中断。
                                                           0：未产生中断；
                                                           1：为了防止数据丢失，如果在发送数据给卡时FIFO是空的，或从卡接收数据时FIFO是满的，控制器输出时钟（MMC_CCLK）会被停止。当时钟被停止后，data-starvation计数器就会启动。如果计数器数满溢出，而此时系统仍然没有往空的FIFO写数据或从满的FIFO读数据，该中断产生。此时，需要系统对FIFO进行读写操作，输出时钟才会重新启动。 */
        unsigned int  frun_int_status : 1;  /* bit[11]   : 屏蔽前的FRUN（FIFO underrun/overrun error）中断。
                                                           0：未产生中断；
                                                           1：当FIFO满时仍往FIFO写数据，或FIFO空时仍从FIFO读数据时该中断产生。 */
        unsigned int  hle_int_status  : 1;  /* bit[12]   : 屏蔽前的HLE（Hardware Locked Write Error）中断。
                                                           0：未产生中断；
                                                           1：在硬件锁定寄存器值时，仍试图对这些寄存器进行写操作。 */
        unsigned int  sbe_int_status  : 1;  /* bit[13]   : 屏蔽前的SBE（Start-bit Error）中断。
                                                           0：未产生中断；
                                                           1：当从卡读取数据时，数据的start bit错误中断。在4bit模式下，如果数据都没有start bit，该中断产生。 */
        unsigned int  acd_int_status  : 1;  /* bit[14]   : 屏蔽前的ACD中断。
                                                           0：未产生中断；
                                                           1：控制器自动发送的auto-stop指令执行中断。 */
        unsigned int  ebe_int_status  : 1;  /* bit[15]   : 屏蔽前的EBE（End-bit error (read/Write） no CRC）中断。
                                                           0：未产生中断；
                                                           1：读操作时出现end bit错误，写操作时卡未返回CRC状态或返回负的CRC状态时，该中断产生。 */
        unsigned int  sdio_int_status : 1;  /* bit[16]   : SDIO中断。
                                                           0：无SDIO中断从卡输出；
                                                           1：有SDIO中断从卡输出。
                                                           中断状态位的与interrupt mask无关。 */
        unsigned int  reserved_1      : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_MMC_RINTSTS_UNION;
#endif
#define SOC_MMC_RINTSTS_re_int_status_START    (1)
#define SOC_MMC_RINTSTS_re_int_status_END      (1)
#define SOC_MMC_RINTSTS_dd_int_status_START    (2)
#define SOC_MMC_RINTSTS_dd_int_status_END      (2)
#define SOC_MMC_RINTSTS_dto_int_status_START   (3)
#define SOC_MMC_RINTSTS_dto_int_status_END     (3)
#define SOC_MMC_RINTSTS_txdr_int_status_START  (4)
#define SOC_MMC_RINTSTS_txdr_int_status_END    (4)
#define SOC_MMC_RINTSTS_rxdr_int_status_START  (5)
#define SOC_MMC_RINTSTS_rxdr_int_status_END    (5)
#define SOC_MMC_RINTSTS_rcrc_int_status_START  (6)
#define SOC_MMC_RINTSTS_rcrc_int_status_END    (6)
#define SOC_MMC_RINTSTS_dcrc_int_status_START  (7)
#define SOC_MMC_RINTSTS_dcrc_int_status_END    (7)
#define SOC_MMC_RINTSTS_rto_int_status_START   (8)
#define SOC_MMC_RINTSTS_rto_int_status_END     (8)
#define SOC_MMC_RINTSTS_drto_int_status_START  (9)
#define SOC_MMC_RINTSTS_drto_int_status_END    (9)
#define SOC_MMC_RINTSTS_hto_int_status_START   (10)
#define SOC_MMC_RINTSTS_hto_int_status_END     (10)
#define SOC_MMC_RINTSTS_frun_int_status_START  (11)
#define SOC_MMC_RINTSTS_frun_int_status_END    (11)
#define SOC_MMC_RINTSTS_hle_int_status_START   (12)
#define SOC_MMC_RINTSTS_hle_int_status_END     (12)
#define SOC_MMC_RINTSTS_sbe_int_status_START   (13)
#define SOC_MMC_RINTSTS_sbe_int_status_END     (13)
#define SOC_MMC_RINTSTS_acd_int_status_START   (14)
#define SOC_MMC_RINTSTS_acd_int_status_END     (14)
#define SOC_MMC_RINTSTS_ebe_int_status_START   (15)
#define SOC_MMC_RINTSTS_ebe_int_status_END     (15)
#define SOC_MMC_RINTSTS_sdio_int_status_START  (16)
#define SOC_MMC_RINTSTS_sdio_int_status_END    (16)


/*****************************************************************************
 结构名    : SOC_MMC_STATUS_UNION
 结构说明  : STATUS 寄存器结构定义。地址偏移量:0x0048，初值:0x00000506，宽度:32
 寄存器说明: MMC状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  fifo_rx_watermark  : 1;  /* bit[0]    : FIFO是否到达Receive watermark level。
                                                              0：未达到水线；
                                                              1：达到水线。 */
        unsigned int  fifo_tx_watermark  : 1;  /* bit[1]    : FIFO是否到达Transmit watermark level。
                                                              0：未达到水线；
                                                              1：达到水线。 */
        unsigned int  fifo_empty         : 1;  /* bit[2]    : FIFO空状态标志位。
                                                              0：FIFO非空；
                                                              1：FIFO空。 */
        unsigned int  fifo_full          : 1;  /* bit[3]    : FIFO满状态标志位。
                                                              0：FIFO不满；
                                                              1：FIFO满。 */
        unsigned int  command_fsm_states : 4;  /* bit[4-7]  : 控制器命令状态机状态。
                                                              0x0：Idle；
                                                              0x1：Send init sequence；
                                                              0x2：Tx cmd start bit；
                                                              0x3：Tx cmd tx bit；
                                                              0x4：Tx cmd index +arg；
                                                              0x5：Tx cmd crc7；
                                                              0x6：Tx cmd end bit；
                                                              0x7：Rx resp start bit；
                                                              0x8：Rx resp IRQ response；
                                                              0x9：Rx resp tx bit；
                                                              0xA：Rx resp cmd idx；
                                                              0xB：Rx resp data；
                                                              0xC：Rx resp crc7；
                                                              0xD：Rx resp end bit；
                                                              0xE：Cmd path wait NCC；
                                                              0xF：Wait；CMD-to-response turnaround。 */
        unsigned int  data_3_status      : 1;  /* bit[8]    : 该位直接反映卡数据线data[3]信号状态。 */
        unsigned int  data_busy          : 1;  /* bit[9]    : 忙状态指示。
                                                              0：卡处于闲置状态；
                                                              1：执行写或擦除操作后卡处于繁忙状态。
                                                              该状态位直接反映卡数据线dat[0]信号的取反。执行写或擦除等操作后需由软件查询该位，直至该位由1变为0后，才能对卡进行下一步的操作。 */
        unsigned int  data_state_mc_busy : 1;  /* bit[10]   : 数据发送/接收状态机的状态。
                                                              0：数据发送/接收状态机处于闲置状态；
                                                              1：数据发送/接收状态机处于繁忙状态。 */
        unsigned int  response_index     : 6;  /* bit[11-16]: 上一个指令响应的序号。包括auto-stop指令的响应。 */
        unsigned int  fifo_count         : 13; /* bit[17-29]: FIFO已有数据数量，以word为单位。 */
        unsigned int  dma_ack            : 1;  /* bit[30]   : DMA确认状态位。
                                                              0：DMAC未清除MMC/SD控制器请求；
                                                              1：DMAC清除MMC/SD控制器请求。 */
        unsigned int  dma_req            : 1;  /* bit[31]   : DMA请求状态位。
                                                              0：控制器未请求DMA传输；
                                                              1：控制器正请求DMA传输。 */
    } reg;
} SOC_MMC_STATUS_UNION;
#endif
#define SOC_MMC_STATUS_fifo_rx_watermark_START   (0)
#define SOC_MMC_STATUS_fifo_rx_watermark_END     (0)
#define SOC_MMC_STATUS_fifo_tx_watermark_START   (1)
#define SOC_MMC_STATUS_fifo_tx_watermark_END     (1)
#define SOC_MMC_STATUS_fifo_empty_START          (2)
#define SOC_MMC_STATUS_fifo_empty_END            (2)
#define SOC_MMC_STATUS_fifo_full_START           (3)
#define SOC_MMC_STATUS_fifo_full_END             (3)
#define SOC_MMC_STATUS_command_fsm_states_START  (4)
#define SOC_MMC_STATUS_command_fsm_states_END    (7)
#define SOC_MMC_STATUS_data_3_status_START       (8)
#define SOC_MMC_STATUS_data_3_status_END         (8)
#define SOC_MMC_STATUS_data_busy_START           (9)
#define SOC_MMC_STATUS_data_busy_END             (9)
#define SOC_MMC_STATUS_data_state_mc_busy_START  (10)
#define SOC_MMC_STATUS_data_state_mc_busy_END    (10)
#define SOC_MMC_STATUS_response_index_START      (11)
#define SOC_MMC_STATUS_response_index_END        (16)
#define SOC_MMC_STATUS_fifo_count_START          (17)
#define SOC_MMC_STATUS_fifo_count_END            (29)
#define SOC_MMC_STATUS_dma_ack_START             (30)
#define SOC_MMC_STATUS_dma_ack_END               (30)
#define SOC_MMC_STATUS_dma_req_START             (31)
#define SOC_MMC_STATUS_dma_req_END               (31)


/*****************************************************************************
 结构名    : SOC_MMC_FIFOTH_UNION
 结构说明  : FIFOTH 寄存器结构定义。地址偏移量:0x004C，初值:0x007F0000，宽度:32
 寄存器说明: MMC FIFO参数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_wmark   : 12; /* bit[0-11] : 发送数据时的FIFO阈值。当FIFO已有数据个数小于或等于该值时，产生DMA请求；若中断使能，则产生中断请求。
                                                      非DMA模式下transmit FIFO data request（TXDR）中断会被使能并产生中断请求。在数据传输末尾，中断产生后由软件完成剩余字节的传送。
                                                      DMA模式下，在数据传输末尾，若剩余的数据小于burst size，DMA仍通过burst方式完成剩余数据的传输。
                                                      限制：Tx_wmark ≤ 1
                                                      建议值：FIFO_DEPTH/2，即小于或等于FIFO_DEPTH/2时发出请求。 */
        unsigned int  resevered  : 4;  /* bit[12-15]: 保留。 */
        unsigned int  rx_wmark   : 9;  /* bit[16-24]: 读数据时的FIFO阈值。当FIFO已有数据个数大于该值时，产生DMA请求；若中断使能，则产生中断请求。
                                                      非DMA模式下receive FIFO data request（RXDR）中断会被使能并产生中断请求。在数据传输末尾，如果FIFO计数没有大于该值，不会产生中断。在Data Transfer Done中断产生后由软件完成剩余数据的读取。
                                                      DMA模式下，在数据传输末尾，当剩余的数据比阈值低，DMA仍会在Data Transfer Done中断产生之前通过burst方式将数据读取。
                                                      限制：Rx_wmark ≥ FIFO_DEPTH - 2
                                                      建议值：(FIFO_DEPTH/2) – 1，即多于(FIFO_DEPTH / 2) - 1)时发出请求。
                                                      对MMC0来说,rx_wmark的范围为24：16。对MMC1，MMC2的范围为23：16。 */
        unsigned int  reserved_0 : 3;  /* bit[25-27]:  */
        unsigned int  burst_size : 3;  /* bit[28-30]: DMA一次burst传输数据量，以word为单位。应该被设置为跟DMAC相同的大小。
                                                      000：1；
                                                      001：4；
                                                      010：8。
                                                      其他：保留。
                                                      允许使用的burst_size和tx_wmark组合为：
                                                      Burst_size = 1，Tx_wmark = 1～15
                                                      Burst_size = 4，Tx_wmark = 4
                                                      Burst_size = 4，Tx_wmark = 4
                                                      Burst_size = 4，Tx_wmark = 12
                                                      Burst_size = 8，Tx_wmark = 8
                                                      Burst_size = 8，Tx_wmark = 4
                                                      允许使用的burst_size和rx_wmark组合为：
                                                      Burst_size = 1，Rx_wmark = 0～14
                                                      Burst_size = 4，Rx_wmark = 3
                                                      Burst_size = 4，Rx_wmark = 7
                                                      Burst_size = 4，Rx_wmark = 11
                                                      Burst_size = 8，Rx_wmark = 7
                                                      Burst_size = 8，Rx_wmark = 11 */
        unsigned int  reserved_1 : 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_MMC_FIFOTH_UNION;
#endif
#define SOC_MMC_FIFOTH_tx_wmark_START    (0)
#define SOC_MMC_FIFOTH_tx_wmark_END      (11)
#define SOC_MMC_FIFOTH_resevered_START   (12)
#define SOC_MMC_FIFOTH_resevered_END     (15)
#define SOC_MMC_FIFOTH_rx_wmark_START    (16)
#define SOC_MMC_FIFOTH_rx_wmark_END      (24)
#define SOC_MMC_FIFOTH_burst_size_START  (28)
#define SOC_MMC_FIFOTH_burst_size_END    (30)


/*****************************************************************************
 结构名    : SOC_MMC_TCBCNT_UNION
 结构说明  : TCBCNT 寄存器结构定义。地址偏移量:0x005C，初值:0x00000000，宽度:32
 寄存器说明: 接口传输计数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trans_card_byte_count : 32; /* bit[0-31]: 控制器与卡之间数据传输字节数。用于统计完成一次数据传输指令后接口传输的数据字节数。在数据传输过程中，本寄存器返回值为0；数据传输结束后本寄存器反映控制器与卡之间数据传输字节数。本寄存器在启动一次新的数据传输指令时清0。 */
    } reg;
} SOC_MMC_TCBCNT_UNION;
#endif
#define SOC_MMC_TCBCNT_trans_card_byte_count_START  (0)
#define SOC_MMC_TCBCNT_trans_card_byte_count_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_TBBCNT_UNION
 结构说明  : TBBCNT 寄存器结构定义。地址偏移量:0x0060，初值:0x00000000，宽度:32
 寄存器说明: FIFO传输计数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trans_fifo_byte_count : 32; /* bit[0-31]: CPU/DMA与控制器FIFO之间传送数据的字节数。 */
    } reg;
} SOC_MMC_TBBCNT_UNION;
#endif
#define SOC_MMC_TBBCNT_trans_fifo_byte_count_START  (0)
#define SOC_MMC_TBBCNT_trans_fifo_byte_count_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_DEBNCE_UNION
 结构说明  : DEBNCE 寄存器结构定义。地址偏移量:0x0064，初值:0x00FFFFFF，宽度:32
 寄存器说明: 去抖计数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  debounce_count : 24; /* bit[0-23] : 去抖动需要的Clk周期数，典型值为5~25ns。 */
        unsigned int  reserved       : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_MMC_DEBNCE_UNION;
#endif
#define SOC_MMC_DEBNCE_debounce_count_START  (0)
#define SOC_MMC_DEBNCE_debounce_count_END    (23)


/*****************************************************************************
 结构名    : SOC_MMC_VERID_UNION
 结构说明  : VERID 寄存器结构定义。地址偏移量:0x006C，初值:0x5342250A，宽度:32
 寄存器说明: 版本寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  version : 32; /* bit[0-31]: Synopsys IP 版本号 */
    } reg;
} SOC_MMC_VERID_UNION;
#endif
#define SOC_MMC_VERID_version_START  (0)
#define SOC_MMC_VERID_version_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_UHSREG_UNION
 结构说明  : UHSREG 寄存器结构定义。地址偏移量:0x0074，初值:0x00000000，宽度:32
 寄存器说明: UHS-1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  VOLT_REG : 1;  /* bit[0]    : High voltag mode，决定外部的输入电压。
                                                    0：输入3.3v电压
                                                    1：输入1.8v电压 */
        unsigned int  reserved_0: 15; /* bit[1-15] : 保留 */
        unsigned int  DDR_REG  : 1;  /* bit[16]   : DDR模式，决定内存的反馈电压。
                                                    0：非DDR模式
                                                    1：DDR模式 */
        unsigned int  reserved_1: 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_MMC_UHSREG_UNION;
#endif
#define SOC_MMC_UHSREG_VOLT_REG_START  (0)
#define SOC_MMC_UHSREG_VOLT_REG_END    (0)
#define SOC_MMC_UHSREG_DDR_REG_START   (16)
#define SOC_MMC_UHSREG_DDR_REG_END     (16)


/*****************************************************************************
 结构名    : SOC_MMC_RST_n_UNION
 结构说明  : RST_n 寄存器结构定义。地址偏移量:0x0078，初值:0x00000001，宽度:32
 寄存器说明: 硬件复位寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  card_reset : 1;  /* bit[0]   : 硬件复位。
                                                     0： 复位模式。
                                                     1： 解复位。 */
        unsigned int  reserved   : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_MMC_RST_n_UNION;
#endif
#define SOC_MMC_RST_n_card_reset_START  (0)
#define SOC_MMC_RST_n_card_reset_END    (0)


/*****************************************************************************
 结构名    : SOC_MMC_BMOD_UNION
 结构说明  : BMOD 寄存器结构定义。地址偏移量:0x0080，初值:0x00000000，宽度:32
 寄存器说明: 总线模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  SWR      : 1;  /* bit[0]    : 软复位寄存器。
                                                    1：复位所有IDMAC 内部寄存器。一个周期后自动清零。 */
        unsigned int  FB       : 1;  /* bit[1]    : 定长传输。控制AHB master 接口是否使用定长模式传输。1： burst传输将使用single、incr4、incr8、incr16模式传输。
                                                    0：将使用Single 和incr 方式传输。 */
        unsigned int  DSL      : 5;  /* bit[2-6]  : Discriptor Skip 长度。 */
        unsigned int  DE       : 1;  /* bit[7]    : IDMAC使能。当置位时为使能IDMAC。 */
        unsigned int  PBL      : 3;  /* bit[8-10] : 可编程burst 长度。寄存器中的内容是FIFOTH寄存器中的Misze内容。 */
        unsigned int  reserved : 21; /* bit[11-31]: 保留。 */
    } reg;
} SOC_MMC_BMOD_UNION;
#endif
#define SOC_MMC_BMOD_SWR_START       (0)
#define SOC_MMC_BMOD_SWR_END         (0)
#define SOC_MMC_BMOD_FB_START        (1)
#define SOC_MMC_BMOD_FB_END          (1)
#define SOC_MMC_BMOD_DSL_START       (2)
#define SOC_MMC_BMOD_DSL_END         (6)
#define SOC_MMC_BMOD_DE_START        (7)
#define SOC_MMC_BMOD_DE_END          (7)
#define SOC_MMC_BMOD_PBL_START       (8)
#define SOC_MMC_BMOD_PBL_END         (10)


/*****************************************************************************
 结构名    : SOC_MMC_PLDMND_UNION
 结构说明  : PLDMND 寄存器结构定义。地址偏移量:0x0084，初值:0x00000000，宽度:32
 寄存器说明: 投票寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  poll_demand : 32; /* bit[0-31]: 如果Discriptor中的OWN没有置位，这FSM将进入suspend状态。这时host向本寄存器写入任何值将会使FSM重新进入读取Discriptor的状态。本寄存器为只写寄存器。 */
    } reg;
} SOC_MMC_PLDMND_UNION;
#endif
#define SOC_MMC_PLDMND_poll_demand_START  (0)
#define SOC_MMC_PLDMND_poll_demand_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_DBADDR_UNION
 结构说明  : DBADDR 寄存器结构定义。地址偏移量:0x0088，初值:0x00000000，宽度:32
 寄存器说明: 链表地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  SDL : 32; /* bit[0-31]: Discriptor 链表的起始地址，包含第一个Discriptor的基地址。低两bit只读。 */
    } reg;
} SOC_MMC_DBADDR_UNION;
#endif
#define SOC_MMC_DBADDR_SDL_START  (0)
#define SOC_MMC_DBADDR_SDL_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_IDSTS_UNION
 结构说明  : IDSTS 寄存器结构定义。地址偏移量:0x008C，初值:0x00000000，宽度:32
 寄存器说明: IDMAC状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  TI       : 1;  /* bit[0]    : 发送中断。指示一次完整的数据发送完成。写1清零。 */
        unsigned int  RI       : 1;  /* bit[1]    : 接收中断。指示一次完整的数据接收完成。写1清零。 */
        unsigned int  FBE      : 1;  /* bit[2]    : 致命总线错误中断。指示由总线错误产生(IDSTS[12:10])。当此bit置位，DMA停止所有总线通道。写1清零。 */
        unsigned int  reserved_0: 1;  /* bit[3]    : 保留。 */
        unsigned int  DU       : 1;  /* bit[4]    : Discriptor不可获取中断。当由于OWN bit为0导致Discriptor不能获取将会将此bit置位。
                                                    写1清除此寄存器。 */
        unsigned int  CES      : 1;  /* bit[5]    : 卡错误组合。指示向卡发送数据或读取数据的错误状态。是以下几个错误的组合。
                                                    EBE – End Bit Error 
                                                    RTO – Response Timeout/Boot Ack Timeout 
                                                    RCRC – Response CRC 
                                                    SBE – Start Bit Error 
                                                    DRTO – Data Read Timeout/BDS timeout 
                                                    DCRC – Data CRC for Receive 
                                                    RE – Response Error
                                                    写1清除此寄存器。 */
        unsigned int  reserved_1: 2;  /* bit[6-7]  : 保留。 */
        unsigned int  NIS      : 1;  /* bit[8]    : 一般中断组合。此寄存器是以下几个中断组合。
                                                    IDSTS[0] – Transmit Interrupt 
                                                    IDSTS[1] – Receive Interrupt
                                                    此bit必须手动清除。写1清除此寄存器。 */
        unsigned int  AIS      : 1;  /* bit[9]    : 异常中断组合。此寄存器是以下几个中断组合。
                                                    IDSTS[2] – Fatal Bus Interrupt 
                                                    IDSTS[4] – DU bit Interrupt 
                                                    IDSTS[5] – Card Error Summary Interrupt
                                                    此bit必须手动清除。写1清除此寄存器。 */
        unsigned int  EB       : 3;  /* bit[10-12]: 错误bit指示。指示总线错误的类型。只有当Fatal Bus Error bit (IDSTS[2])置位时有效。
                                                    3’b001 – Host Abort received during transmission 
                                                    3’b010 – Host Abort received during reception 
                                                    Others: Reserved */
        unsigned int  FSM      : 4;  /* bit[13-16]: DMAC FSM现在的状态。
                                                    0：DMA_IDLE
                                                    1：DMA_SUSPEND
                                                    2：DESC_RD
                                                    3：DESC_CHK
                                                    4：DMA_RD_REQ_WAIT
                                                    5：DMA_WR_REQ_WAIT
                                                    6：DMA_RD
                                                    7：DMA_WR
                                                    8：DESC_CLOSE
                                                    此寄存器只读。 */
        unsigned int  reserved_2: 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_MMC_IDSTS_UNION;
#endif
#define SOC_MMC_IDSTS_TI_START        (0)
#define SOC_MMC_IDSTS_TI_END          (0)
#define SOC_MMC_IDSTS_RI_START        (1)
#define SOC_MMC_IDSTS_RI_END          (1)
#define SOC_MMC_IDSTS_FBE_START       (2)
#define SOC_MMC_IDSTS_FBE_END         (2)
#define SOC_MMC_IDSTS_DU_START        (4)
#define SOC_MMC_IDSTS_DU_END          (4)
#define SOC_MMC_IDSTS_CES_START       (5)
#define SOC_MMC_IDSTS_CES_END         (5)
#define SOC_MMC_IDSTS_NIS_START       (8)
#define SOC_MMC_IDSTS_NIS_END         (8)
#define SOC_MMC_IDSTS_AIS_START       (9)
#define SOC_MMC_IDSTS_AIS_END         (9)
#define SOC_MMC_IDSTS_EB_START        (10)
#define SOC_MMC_IDSTS_EB_END          (12)
#define SOC_MMC_IDSTS_FSM_START       (13)
#define SOC_MMC_IDSTS_FSM_END         (16)


/*****************************************************************************
 结构名    : SOC_MMC_IDINTEN_UNION
 结构说明  : IDINTEN 寄存器结构定义。地址偏移量:0x0090，初值:0x00000000，宽度:32
 寄存器说明: IDMAC中断使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  TI       : 1;  /* bit[0]    : 发送中断使能。此bit当连同NI置位后，TI中断使能。 */
        unsigned int  RI       : 1;  /* bit[1]    : 接收中断使能。此bit当连同NI置位后，RI中断使能。 */
        unsigned int  FBE      : 1;  /* bit[2]    : 致命总线错误中断使能。此bit当连同AI置位后，FBE中断使能。 */
        unsigned int  reserved_0: 1;  /* bit[3]    : 保留。 */
        unsigned int  DU       : 1;  /* bit[4]    : Discriptor不可获取中断使能。此bit当连同AI置位后，DU中断使能。 */
        unsigned int  CES      : 1;  /* bit[5]    : 卡错误组合中断使能。写1使能卡错误组合中断。 */
        unsigned int  reserved_1: 2;  /* bit[6-7]  : 保留。 */
        unsigned int  NI       : 1;  /* bit[8]    : 一般组合中断。写1使能一般组合中断。此bit使能以下几个bit。
                                                    IDINTEN[0] – Transmit Interrupt 
                                                    IDINTEN[1] – Receive Interrupt */
        unsigned int  AI       : 1;  /* bit[9]    : 异常组合中断使能。写1使能异常组合中断。此bit使能以下几个bit。
                                                    IDINTEN[2] – Fatal Bus Interrupt 
                                                    IDINTEN[4] – DU bit Interrupt 
                                                    IDINTEN[5] – Card Error Summary Interrupt */
        unsigned int  reserved_2: 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_MMC_IDINTEN_UNION;
#endif
#define SOC_MMC_IDINTEN_TI_START        (0)
#define SOC_MMC_IDINTEN_TI_END          (0)
#define SOC_MMC_IDINTEN_RI_START        (1)
#define SOC_MMC_IDINTEN_RI_END          (1)
#define SOC_MMC_IDINTEN_FBE_START       (2)
#define SOC_MMC_IDINTEN_FBE_END         (2)
#define SOC_MMC_IDINTEN_DU_START        (4)
#define SOC_MMC_IDINTEN_DU_END          (4)
#define SOC_MMC_IDINTEN_CES_START       (5)
#define SOC_MMC_IDINTEN_CES_END         (5)
#define SOC_MMC_IDINTEN_NI_START        (8)
#define SOC_MMC_IDINTEN_NI_END          (8)
#define SOC_MMC_IDINTEN_AI_START        (9)
#define SOC_MMC_IDINTEN_AI_END          (9)


/*****************************************************************************
 结构名    : SOC_MMC_DSCADDR_UNION
 结构说明  : DSCADDR 寄存器结构定义。地址偏移量:0x0094，初值:0x00000000，宽度:32
 寄存器说明: 当前Discriptor地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  HAD : 32; /* bit[0-31]: 当前Discriptor地址指针。复位后此寄存器清零。本寄存器指示DMA当前读取的Discriptor的起始地址。 */
    } reg;
} SOC_MMC_DSCADDR_UNION;
#endif
#define SOC_MMC_DSCADDR_HAD_START  (0)
#define SOC_MMC_DSCADDR_HAD_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_BUFADDR_UNION
 结构说明  : BUFADDR 寄存器结构定义。地址偏移量:0x0098，初值:0x00000000，宽度:32
 寄存器说明: 当前Buffer地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  HBA : 32; /* bit[0-31]: 当前Buffer 地址指针。复位后此寄存器清零。本寄存器指示DMA当前读取的Buffer的地址 */
    } reg;
} SOC_MMC_BUFADDR_UNION;
#endif
#define SOC_MMC_BUFADDR_HBA_START  (0)
#define SOC_MMC_BUFADDR_HBA_END    (31)


/*****************************************************************************
 结构名    : SOC_MMC_CARDTHRCTL_UNION
 结构说明  : CARDTHRCTL 寄存器结构定义。地址偏移量:0x0100，初值:0x00000000，宽度:32
 寄存器说明: 读卡FIFO门限寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  card_rd_thr_en    : 1;  /* bit[0]    : 读卡FIFO门限使能。
                                                             0：不使能读卡FIFO门限；
                                                             1：使能读卡FIFO门限。
                                                             当使能读卡FIFO门限时，只有在读FIFO留有足够空间时才会发出读命令。 */
        unsigned int  bsy_clr_int_en    : 1;  /* bit[1]    : 总线忙状态清除中断使能。
                                                             0：不使能总线忙状态清除中断；
                                                             1：使能总线忙状态清除中断。 */
        unsigned int  reserved_0        : 14; /* bit[2-15] : 保留。 */
        unsigned int  card_rd_threshold : 12; /* bit[16-27]: 读卡FIFO门限值，最大为512字节。 */
        unsigned int  reserved_1        : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_MMC_CARDTHRCTL_UNION;
#endif
#define SOC_MMC_CARDTHRCTL_card_rd_thr_en_START     (0)
#define SOC_MMC_CARDTHRCTL_card_rd_thr_en_END       (0)
#define SOC_MMC_CARDTHRCTL_bsy_clr_int_en_START     (1)
#define SOC_MMC_CARDTHRCTL_bsy_clr_int_en_END       (1)
#define SOC_MMC_CARDTHRCTL_card_rd_threshold_START  (16)
#define SOC_MMC_CARDTHRCTL_card_rd_threshold_END    (27)


/*****************************************************************************
 结构名    : SOC_MMC_UHS_REG_EXT_UNION
 结构说明  : UHS_REG_EXT 寄存器结构定义。地址偏移量:0x0108，初值:0x00000000，宽度:32
 寄存器说明: UHS寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0          : 16; /* bit[0-15] : 保留。 */
        unsigned int  clk_smpl_phase_ctrl : 7;  /* bit[16-22]: cclk_in_sample时钟移相控制。
                                                               其中bit[21]控制插是否插入delay buffer，用于移相细调；bit[19:16]控制移相相位，用于移相粗调。 */
        unsigned int  clk_drv_phase_ctrl  : 7;  /* bit[23-29]: driver时钟不支持移相 */
        unsigned int  reserved_1          : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_MMC_UHS_REG_EXT_UNION;
#endif
#define SOC_MMC_UHS_REG_EXT_clk_smpl_phase_ctrl_START  (16)
#define SOC_MMC_UHS_REG_EXT_clk_smpl_phase_ctrl_END    (22)
#define SOC_MMC_UHS_REG_EXT_clk_drv_phase_ctrl_START   (23)
#define SOC_MMC_UHS_REG_EXT_clk_drv_phase_ctrl_END     (29)


/*****************************************************************************
 结构名    : SOC_MMC_EMMC_DDR_REG_UNION
 结构说明  : EMMC_DDR_REG 寄存器结构定义。地址偏移量:0x010C，初值:0x00000000，宽度:32
 寄存器说明: eMMC4.5 DDR模式起始位控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  half_start_bit : 1;  /* bit[0]   : DDR50模式下起始位控制。
                                                         0：起始位为一个时钟周期；
                                                         1：起始位为半个时钟周期。 */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_MMC_EMMC_DDR_REG_UNION;
#endif
#define SOC_MMC_EMMC_DDR_REG_half_start_bit_START  (0)
#define SOC_MMC_EMMC_DDR_REG_half_start_bit_END    (0)


/*****************************************************************************
 结构名    : SOC_MMC_ENABLE_SHIFT_UNION
 结构说明  : ENABLE_SHIFT 寄存器结构定义。地址偏移量:0x0110，初值:0x00000000，宽度:32
 寄存器说明: 移相使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enable_shift : 2;  /* bit[0-1] : 移相使能控制。
                                                       00：默认移相；
                                                       01：移相到下一个上升沿；
                                                       10：移相到下一个下降沿；
                                                       11：reserved。 */
        unsigned int  reserved     : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_MMC_ENABLE_SHIFT_UNION;
#endif
#define SOC_MMC_ENABLE_SHIFT_enable_shift_START  (0)
#define SOC_MMC_ENABLE_SHIFT_enable_shift_END    (1)






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

#endif /* end of soc_mmc_interface.h */
