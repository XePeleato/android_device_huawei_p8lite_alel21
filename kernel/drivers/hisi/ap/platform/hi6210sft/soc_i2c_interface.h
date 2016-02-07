

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_I2C_INTERFACE_H__
#define __SOC_I2C_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) Register_I2C
 ****************************************************************************/
/* 寄存器说明：I2C控制寄存器。
            (CH)I2C_CON只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_CON_UNION */
#define SOC_I2C_CON_ADDR(base)                        ((base) + (0x0000))

/* 寄存器说明：I2C访问Slave地址寄存器。
            (CH)I2C_TAR只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_TAR_UNION */
#define SOC_I2C_TAR_ADDR(base)                        ((base) + (0x0004))

/* 寄存器说明：I2C作为Slave时自身的地址寄存器。
            (CH)I2C_SAR只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_SAR_UNION */
#define SOC_I2C_SAR_ADDR(base)                        ((base) + (0x0008))

/* 寄存器说明：I2C高速模式主机码地址寄存器。(CH)I2C_HS_MADDR只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_HS_MADDR_UNION */
#define SOC_I2C_HS_MADDR_ADDR(base)                   ((base) + (0x000C))

/* 寄存器说明：I2C数据通道寄存器。
   位域定义UNION结构:  SOC_I2C_DATA_CMD_UNION */
#define SOC_I2C_DATA_CMD_ADDR(base)                   ((base) + (0x0010))

/* 寄存器说明：标准速度下的SCL时钟高电平时间配置寄存器。
            (CH)I2C_SS_SCL_HCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_SS_SCL_HCNT_UNION */
#define SOC_I2C_SS_SCL_HCNT_ADDR(base)                ((base) + (0x0014))

/* 寄存器说明：标准速度下的SCL时钟低电平时间配置寄存器。
            (CH)I2C_SS_SCL_LCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_SS_SCL_LCNT_UNION */
#define SOC_I2C_SS_SCL_LCNT_ADDR(base)                ((base) + (0x0018))

/* 寄存器说明：快速速度下的SCL时钟高电平时间配置寄存器。
            (CH)I2C_FS_SCL_HCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_FS_SCL_HCNT_UNION */
#define SOC_I2C_FS_SCL_HCNT_ADDR(base)                ((base) + (0x001C))

/* 寄存器说明：快速速度下的SCL时钟低电平时间配置寄存器。
            (CH)I2C_FS_SCL_LCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_FS_SCL_LCNT_UNION */
#define SOC_I2C_FS_SCL_LCNT_ADDR(base)                ((base) + (0x0020))

/* 寄存器说明：高速速速度下的SCL时钟高电平时间配置寄存器。
            (CH)I2C_HS_SCL_HCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_HS_SCL_HCNT_UNION */
#define SOC_I2C_HS_SCL_HCNT_ADDR(base)                ((base) + (0x0024))

/* 寄存器说明：高速速度下的SCL时钟高电平时间配置寄存器。
            (CH)I2C_HS_SCL_HCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
   位域定义UNION结构:  SOC_I2C_HS_SCL_LCNT_UNION */
#define SOC_I2C_HS_SCL_LCNT_ADDR(base)                ((base) + (0x0028))

/* 寄存器说明：中断状态寄存器。
   位域定义UNION结构:  SOC_I2C_INTR_STAT_UNION */
#define SOC_I2C_INTR_STAT_ADDR(base)                  ((base) + (0x002C))

/* 寄存器说明：中断屏蔽寄存器。Bit值为0表示屏蔽，为1表示不屏蔽。
   位域定义UNION结构:  SOC_I2C_INTR_MASK_UNION */
#define SOC_I2C_INTR_MASK_ADDR(base)                  ((base) + (0x0030))

/* 寄存器说明：原始中断状态寄存器。Bit值为0表示无中断，为1表示有中断。
   位域定义UNION结构:  SOC_I2C_RAW_INTR_STAT_UNION */
#define SOC_I2C_RAW_INTR_STAT_ADDR(base)              ((base) + (0x0034))

/* 寄存器说明：RX_FIFO的水线配置寄存器。
   位域定义UNION结构:  SOC_I2C_RX_TL_UNION */
#define SOC_I2C_RX_TL_ADDR(base)                      ((base) + (0x0038))

/* 寄存器说明：TX_FIFO的水线配置寄存器。
   位域定义UNION结构:  SOC_I2C_TX_TL_UNION */
#define SOC_I2C_TX_TL_ADDR(base)                      ((base) + (0x003C))

/* 寄存器说明：组合及独立中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_INTR_UNION */
#define SOC_I2C_CLR_INTR_ADDR(base)                   ((base) + (0x0040))

/* 寄存器说明：RX_UNDER中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_RX_UNDER_UNION */
#define SOC_I2C_CLR_RX_UNDER_ADDR(base)               ((base) + (0x0044))

/* 寄存器说明：RX_OVER中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_RX_OVER_UNION */
#define SOC_I2C_CLR_RX_OVER_ADDR(base)                ((base) + (0x0048))

/* 寄存器说明：TX_OVER中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_TX_OVER_UNION */
#define SOC_I2C_CLR_TX_OVER_ADDR(base)                ((base) + (0x004C))

/* 寄存器说明：RD_REQ中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_RD_REQ_UNION */
#define SOC_I2C_CLR_RD_REQ_ADDR(base)                 ((base) + (0x0050))

/* 寄存器说明：ABRT中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_TX_ABRT_UNION */
#define SOC_I2C_CLR_TX_ABRT_ADDR(base)                ((base) + (0x0054))

/* 寄存器说明：RX_DONE中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_RX_DONE_UNION */
#define SOC_I2C_CLR_RX_DONE_ADDR(base)                ((base) + (0x0058))

/* 寄存器说明：ACTIVITY状态寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_ACTIVITY_UNION */
#define SOC_I2C_CLR_ACTIVITY_ADDR(base)               ((base) + (0x005C))

/* 寄存器说明：STOP_DET中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_STOP_DET_UNION */
#define SOC_I2C_CLR_STOP_DET_ADDR(base)               ((base) + (0x0060))

/* 寄存器说明：START_DET中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_START_DET_UNION */
#define SOC_I2C_CLR_START_DET_ADDR(base)              ((base) + (0x0064))

/* 寄存器说明：GEN_CALL中断清除寄存器。
   位域定义UNION结构:  SOC_I2C_CLR_GEN_CALL_UNION */
#define SOC_I2C_CLR_GEN_CALL_ADDR(base)               ((base) + (0x0068))

/* 寄存器说明：I2C使能寄存器。用于关闭或使能I2C。
            当I2C处于数据传输状态时（即I2C_STATUS[activity]为1），I2C可以被关闭，但需要注意：
            (I)如果I2C处于发送状态时关闭，则I2C在完成当前字节发送后停止继续传输，同时清除TX_FIFO的数据。
            (I)如果I2C处于接收状态时关闭，则I2C在接收完当前字节后不响应这次传输，即发出NACK。
   位域定义UNION结构:  SOC_I2C_ENABLE_UNION */
#define SOC_I2C_ENABLE_ADDR(base)                     ((base) + (0x006C))

/* 寄存器说明：I2C状态寄存器。
   位域定义UNION结构:  SOC_I2C_STATUS_UNION */
#define SOC_I2C_STATUS_ADDR(base)                     ((base) + (0x0070))

/* 寄存器说明：TX_FIFO中的数据个数指示寄存器。
   位域定义UNION结构:  SOC_I2C_TXFLR_UNION */
#define SOC_I2C_TXFLR_ADDR(base)                      ((base) + (0x0074))

/* 寄存器说明：RX_FIFO中的数据个数指示寄存器。
   位域定义UNION结构:  SOC_I2C_RXFLR_UNION */
#define SOC_I2C_RXFLR_ADDR(base)                      ((base) + (0x0078))

/* 寄存器说明：SDA保持时间长度寄存器
   位域定义UNION结构:  SOC_I2C_SDA_HOLD_UNION */
#define SOC_I2C_SDA_HOLD_ADDR(base)                   ((base) + (0x007C))

/* 寄存器说明：TX_ABRT的源头中断寄存器。
   位域定义UNION结构:  SOC_I2C_TX_ABRT_SOURCE_UNION */
#define SOC_I2C_TX_ABRT_SOURCE_ADDR(base)             ((base) + (0x0080))

/* 寄存器说明：I2C DMA通道开启控制寄存器。
   位域定义UNION结构:  SOC_I2C_DMA_CR_UNION */
#define SOC_I2C_DMA_CR_ADDR(base)                     ((base) + (0x0088))

/* 寄存器说明：TX_FIFO进行DMA操作时的水线配置寄存器。
   位域定义UNION结构:  SOC_I2C_DMA_TDLR_UNION */
#define SOC_I2C_DMA_TDLR_ADDR(base)                   ((base) + (0x008C))

/* 寄存器说明：RX_FIFO进行DMA操作时的水线配置寄存器
   位域定义UNION结构:  SOC_I2C_DMA_RDLR_UNION */
#define SOC_I2C_DMA_RDLR_ADDR(base)                   ((base) + (0x0090))

/* 寄存器说明：SDA建立时间长度寄存器
   位域定义UNION结构:  SOC_I2C_SDA_SETUP_UNION */
#define SOC_I2C_SDA_SETUP_ADDR(base)                  ((base) + (0x0094))

/* 寄存器说明：全呼叫响应寄存器
   位域定义UNION结构:  SOC_I2C_ACK_GENERAL_CALL_UNION */
#define SOC_I2C_ACK_GENERAL_CALL_ADDR(base)           ((base) + (0x0098))

/* 寄存器说明：I2C使能状态寄存器
   位域定义UNION结构:  SOC_I2C_ENABLE_STATUS_UNION */
#define SOC_I2C_ENABLE_STATUS_ADDR(base)              ((base) + (0x009C))

/* 寄存器说明：标速及全速模式下毛刺抑制长度寄存器
   位域定义UNION结构:  SOC_I2C_FS_SPKLEN_UNION */
#define SOC_I2C_FS_SPKLEN_ADDR(base)                  ((base) + (0x00A0))

/* 寄存器说明：高速模式下毛刺抑制长度寄存器
   位域定义UNION结构:  SOC_I2C_HS_SPKLEN_UNION */
#define SOC_I2C_HS_SPKLEN_ADDR(base)                  ((base) + (0x00A4))

/* 寄存器说明：参数寄存器
   位域定义UNION结构:  SOC_I2C_COMP_PARAM_1_UNION */
#define SOC_I2C_COMP_PARAM_1_ADDR(base)               ((base) + (0x00F4))

/* 寄存器说明：版本寄存器
   位域定义UNION结构:  SOC_I2C_COMP_VERSION_UNION */
#define SOC_I2C_COMP_VERSION_ADDR(base)               ((base) + (0x00F8))

/* 寄存器说明：DesignWareIP类型寄存器
   位域定义UNION结构:  SOC_I2C_COMP_TYPE_UNION */
#define SOC_I2C_COMP_TYPE_ADDR(base)                  ((base) + (0x00FC))





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
                     (1/1) Register_I2C
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_I2C_CON_UNION
 结构说明  : CON 寄存器结构定义。地址偏移量:0x0000，初值:0x007F，宽度:16
 寄存器说明: I2C控制寄存器。
            (CH)I2C_CON只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  master_mode          : 1;  /* bit[0]   : Master功能使能。
                                                                 0：禁止；
                                                                 1：使能。 */
        unsigned short  speed                : 2;  /* bit[1-2] : I2C操作速度选择。
                                                                 00：非法；
                                                                 01：标准速度（0~100kbit/s）；
                                                                 10：快速速度（<=400kbit/s）；
                                                                 11：高速速度（<=3.4Mbit/s）。&#13; */
        unsigned short  i2c_10bitaddr_slave  : 1;  /* bit[3]   : 保留。 */
        unsigned short  i2c_10bitaddr_master : 1;  /* bit[4]   : 发送7bit地址/10bit地址选择。
                                                                 0：7bit地址；
                                                                 1：10bit地址。 */
        unsigned short  i2c_restart_en       : 1;  /* bit[5]   : Master产生“重新开始”条件使能。
                                                                 0：禁止，此时无法实现“重新开始”条件的功能；
                                                                 1：使能。 */
        unsigned short  i2c_slave_disable    : 1;  /* bit[6]   : 保留。
                                                                 注意：此位只能设置为1。 */
        unsigned short  reserved             : 9;  /* bit[7-15]: 保留。 */
    } reg;
} SOC_I2C_CON_UNION;
#endif
#define SOC_I2C_CON_master_mode_START           (0)
#define SOC_I2C_CON_master_mode_END             (0)
#define SOC_I2C_CON_speed_START                 (1)
#define SOC_I2C_CON_speed_END                   (2)
#define SOC_I2C_CON_i2c_10bitaddr_slave_START   (3)
#define SOC_I2C_CON_i2c_10bitaddr_slave_END     (3)
#define SOC_I2C_CON_i2c_10bitaddr_master_START  (4)
#define SOC_I2C_CON_i2c_10bitaddr_master_END    (4)
#define SOC_I2C_CON_i2c_restart_en_START        (5)
#define SOC_I2C_CON_i2c_restart_en_END          (5)
#define SOC_I2C_CON_i2c_slave_disable_START     (6)
#define SOC_I2C_CON_i2c_slave_disable_END       (6)


/*****************************************************************************
 结构名    : SOC_I2C_TAR_UNION
 结构说明  : TAR 寄存器结构定义。地址偏移量:0x0004，初值:0x0055，宽度:16
 寄存器说明: I2C访问Slave地址寄存器。
            (CH)I2C_TAR只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_tar               : 10; /* bit[0-9]  : I2C作为Master时要访问的Slave的地址。 */
        unsigned short  gc_or_start           : 1;  /* bit[10]   : bit[11]为1时此位有效，决定执行功能是general call还是start byte。
                                                                   0：general call；
                                                                   1：start byte。 */
        unsigned short  special               : 1;  /* bit[11]   : General call和start byte功能使能。
                                                                   0：禁止；
                                                                   1：使能。 */
        unsigned short  i2c_10bit_addr_master : 1;  /* bit[12]   : 控制当DW_apb_i2c做为主机时处于7-bit寻址或是10-bit寻址模式。
                                                                   0：7-bit寻址；
                                                                   1：10-bit寻址 */
        unsigned short  reserved              : 3;  /* bit[13-15]: 保留。 */
    } reg;
} SOC_I2C_TAR_UNION;
#endif
#define SOC_I2C_TAR_i2c_tar_START                (0)
#define SOC_I2C_TAR_i2c_tar_END                  (9)
#define SOC_I2C_TAR_gc_or_start_START            (10)
#define SOC_I2C_TAR_gc_or_start_END              (10)
#define SOC_I2C_TAR_special_START                (11)
#define SOC_I2C_TAR_special_END                  (11)
#define SOC_I2C_TAR_i2c_10bit_addr_master_START  (12)
#define SOC_I2C_TAR_i2c_10bit_addr_master_END    (12)


/*****************************************************************************
 结构名    : SOC_I2C_SAR_UNION
 结构说明  : SAR 寄存器结构定义。地址偏移量:0x0008，初值:0x0055，宽度:16
 寄存器说明: I2C作为Slave时自身的地址寄存器。
            (CH)I2C_SAR只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_sar  : 10; /* bit[0-9]  : I2C作为Slave时自身的地址。对于7bit寻址，只有i2c-sar[6:0]会被使用 */
        unsigned short  reserved : 6;  /* bit[10-15]: 保留。 */
    } reg;
} SOC_I2C_SAR_UNION;
#endif
#define SOC_I2C_SAR_i2c_sar_START   (0)
#define SOC_I2C_SAR_i2c_sar_END     (9)


/*****************************************************************************
 结构名    : SOC_I2C_HS_MADDR_UNION
 结构说明  : HS_MADDR 寄存器结构定义。地址偏移量:0x000C，初值:0x0001，宽度:16
 寄存器说明: I2C高速模式主机码地址寄存器。(CH)I2C_HS_MADDR只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_hs_mar : 3;  /* bit[0-2] : I2C高速模式下主机代码。每一个master有自己独立的master code，在同一个I2C系统中，最大支持到8个高速模式的master。 */
        unsigned short  reserved   : 13; /* bit[3-15]: 保留。 */
    } reg;
} SOC_I2C_HS_MADDR_UNION;
#endif
#define SOC_I2C_HS_MADDR_i2c_hs_mar_START  (0)
#define SOC_I2C_HS_MADDR_i2c_hs_mar_END    (2)


/*****************************************************************************
 结构名    : SOC_I2C_DATA_CMD_UNION
 结构说明  : DATA_CMD 寄存器结构定义。地址偏移量:0x0010，初值:0x0000，宽度:16
 寄存器说明: I2C数据通道寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  dat      : 8;  /* bit[0-7]  : 将要在I2C总线上发送/接收的数据。
                                                      读此8bit会读出在I2C总线上接收的数据；
                                                      写此8bit会把写入的数据发送到I2C总线上。 */
        unsigned short  cmd      : 1;  /* bit[8]    : 读/写控制位。
                                                      
                                                      0：写操作，表示I2C控制器将要向I2C总线发送数据，此时低8位（DAT）是I2C要向I2C总线发送的数据；
                                                      1：读操作，表示I2C控制器将要从I2C总线读回数据。 */
        unsigned short  stop     : 1;  /* bit[9]    : 1：当前字节数据传输完成后，均产生STOP，不管发送FIFO是否为空。如果发送FIFO不为空，则主机立刻产生一个START及请求总线仲裁来尝试新的传输；
                                                      
                                                      0：当前字节数据传输完成后，不产生STOP，不管发送FIFO是否为空。如果发送FIFO不为空，则主机继续当前传输，根据方向命令字的配置发送或接收数据。如果发送FIFO为空，则主机将SCL拉低，直到发送FIFO中有新的命令。 */
        unsigned short  restart  : 1;  /* bit[10]   : 1：不管传输方向与之前是否相同，在下一次传输前均产生RESTART。
                                                      0：只在传输方向与之前不相同时，在下一次传输前产生RESTART。 */
        unsigned short  reserved : 5;  /* bit[11-15]: 保留。 */
    } reg;
} SOC_I2C_DATA_CMD_UNION;
#endif
#define SOC_I2C_DATA_CMD_dat_START       (0)
#define SOC_I2C_DATA_CMD_dat_END         (7)
#define SOC_I2C_DATA_CMD_cmd_START       (8)
#define SOC_I2C_DATA_CMD_cmd_END         (8)
#define SOC_I2C_DATA_CMD_stop_START      (9)
#define SOC_I2C_DATA_CMD_stop_END        (9)
#define SOC_I2C_DATA_CMD_restart_START   (10)
#define SOC_I2C_DATA_CMD_restart_END     (10)


/*****************************************************************************
 结构名    : SOC_I2C_SS_SCL_HCNT_UNION
 结构说明  : SS_SCL_HCNT 寄存器结构定义。地址偏移量:0x0014，初值:0x0190，宽度:16
 寄存器说明: 标准速度下的SCL时钟高电平时间配置寄存器。
            (CH)I2C_SS_SCL_HCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_ss_scl_hcnt : 16; /* bit[0-15]: 标准速度下的SCL信号高电平的时钟周期数（ic_clk周期）。
                                                            
                                                            注意：配置的最小值为6，写入小于6的值时会被认为是6。 */
    } reg;
} SOC_I2C_SS_SCL_HCNT_UNION;
#endif
#define SOC_I2C_SS_SCL_HCNT_i2c_ss_scl_hcnt_START  (0)
#define SOC_I2C_SS_SCL_HCNT_i2c_ss_scl_hcnt_END    (15)


/*****************************************************************************
 结构名    : SOC_I2C_SS_SCL_LCNT_UNION
 结构说明  : SS_SCL_LCNT 寄存器结构定义。地址偏移量:0x0018，初值:0x01D6，宽度:16
 寄存器说明: 标准速度下的SCL时钟低电平时间配置寄存器。
            (CH)I2C_SS_SCL_LCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_ss_scl_lcnt : 16; /* bit[0-15]: 标准速度下的SCL信号低电平的时钟周期数（ic_clk周期）。
                                                            
                                                            注意：配置的最小值为8，写入小于8的值时会被认为是8。 */
    } reg;
} SOC_I2C_SS_SCL_LCNT_UNION;
#endif
#define SOC_I2C_SS_SCL_LCNT_i2c_ss_scl_lcnt_START  (0)
#define SOC_I2C_SS_SCL_LCNT_i2c_ss_scl_lcnt_END    (15)


/*****************************************************************************
 结构名    : SOC_I2C_FS_SCL_HCNT_UNION
 结构说明  : FS_SCL_HCNT 寄存器结构定义。地址偏移量:0x001C，初值:0x003C，宽度:16
 寄存器说明: 快速速度下的SCL时钟高电平时间配置寄存器。
            (CH)I2C_FS_SCL_HCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_fs_scl_hcnt : 16; /* bit[0-15]: 快速速度下的SCL信号高电平的时钟周期数（ic_clk周期）。
                                                            
                                                            注意：配置的最小值为6，写入小于6的值时会被认为是6。 */
    } reg;
} SOC_I2C_FS_SCL_HCNT_UNION;
#endif
#define SOC_I2C_FS_SCL_HCNT_i2c_fs_scl_hcnt_START  (0)
#define SOC_I2C_FS_SCL_HCNT_i2c_fs_scl_hcnt_END    (15)


/*****************************************************************************
 结构名    : SOC_I2C_FS_SCL_LCNT_UNION
 结构说明  : FS_SCL_LCNT 寄存器结构定义。地址偏移量:0x0020，初值:0x0082，宽度:16
 寄存器说明: 快速速度下的SCL时钟低电平时间配置寄存器。
            (CH)I2C_FS_SCL_LCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_fs_scl_lcnt : 16; /* bit[0-15]: 快速速度下的SCL信号低电平的时钟周期数（ic_clk周期）。
                                                            
                                                            注意：配置的最小值为8，写入小于8的值时会被认为是8。 */
    } reg;
} SOC_I2C_FS_SCL_LCNT_UNION;
#endif
#define SOC_I2C_FS_SCL_LCNT_i2c_fs_scl_lcnt_START  (0)
#define SOC_I2C_FS_SCL_LCNT_i2c_fs_scl_lcnt_END    (15)


/*****************************************************************************
 结构名    : SOC_I2C_HS_SCL_HCNT_UNION
 结构说明  : HS_SCL_HCNT 寄存器结构定义。地址偏移量:0x0024，初值:0x0006，宽度:16
 寄存器说明: 高速速速度下的SCL时钟高电平时间配置寄存器。
            (CH)I2C_HS_SCL_HCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_hs_scl_hcnt : 16; /* bit[0-15]: 高速速度下的SCL信号高电平的时钟周期数（ic_clk周期）。
                                                            
                                                            注意：配置的最小值为6，写入小于6的值时会被认为是6。 */
    } reg;
} SOC_I2C_HS_SCL_HCNT_UNION;
#endif
#define SOC_I2C_HS_SCL_HCNT_i2c_hs_scl_hcnt_START  (0)
#define SOC_I2C_HS_SCL_HCNT_i2c_hs_scl_hcnt_END    (15)


/*****************************************************************************
 结构名    : SOC_I2C_HS_SCL_LCNT_UNION
 结构说明  : HS_SCL_LCNT 寄存器结构定义。地址偏移量:0x0028，初值:0x0010，宽度:16
 寄存器说明: 高速速度下的SCL时钟高电平时间配置寄存器。
            (CH)I2C_HS_SCL_HCNT只在I2C被禁止（即I2C_ENABLE[enable]为0）时才可配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_hs_scl_lcnt : 16; /* bit[0-15]: 高速速度下的SCL信号低电平的时钟周期数（ic_clk周期）。
                                                            
                                                            注意：配置的最小值为8，写入小于8的值时会被认为是8。 */
    } reg;
} SOC_I2C_HS_SCL_LCNT_UNION;
#endif
#define SOC_I2C_HS_SCL_LCNT_i2c_hs_scl_lcnt_START  (0)
#define SOC_I2C_HS_SCL_LCNT_i2c_hs_scl_lcnt_END    (15)


/*****************************************************************************
 结构名    : SOC_I2C_INTR_STAT_UNION
 结构说明  : INTR_STAT 寄存器结构定义。地址偏移量:0x002C，初值:0x0000，宽度:16
 寄存器说明: 中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  rx_under  : 1;  /* bit[0]    : RX_UNDER中断，当接收FIFO为空时，内部总线接口发起了读I2C_DATA_CMD的请求。
                                                       0：无意义；
                                                       1：RX_FIFO为空时，CPU读I2C_DATA_CMD。 */
        unsigned short  rx_over   : 1;  /* bit[1]    : RX_OVER中断，RX_FIFO溢出标志。
                                                       0：未溢出；
                                                       1：溢出。 */
        unsigned short  rx_full   : 1;  /* bit[2]    : RX_FULL中断，指示RX_FIFO中数据是否到达或大于水线值。
                                                       0：小于水线；
                                                       1：等于或大于水线。 */
        unsigned short  tx_over   : 1;  /* bit[3]    : TX_OVER中断，TX_FIFO溢出标志。
                                                       0：未溢出；
                                                       1：溢出。 */
        unsigned short  tx_empty  : 1;  /* bit[4]    : TX_EMPTY中断，指示TX_FIFO中数据是否到达或低于水线值。
                                                       0：大于水线；
                                                       1：等于或低于水线。 */
        unsigned short  rd_req    : 1;  /* bit[5]    : RD_REQ中断，当I2C作为Slave时，该数据位指示是否有Master设备发起读数据请求。
                                                       0：无请求；
                                                       1：有请求。 */
        unsigned short  tx_abrt   : 1;  /* bit[6]    : TX_ABRT中断，有多种情况可以触发此位（详细描述请参见“I2C_TX_ABRT_SOURCE”）。 */
        unsigned short  rx_done   : 1;  /* bit[7]    : RX_DONE中断，当I2C作为Slave时，该数据位指示数据接收是否完成。
                                                       0：未完成；
                                                       1：完成。 */
        unsigned short  activity  : 1;  /* bit[8]    : ACTIVITY中断，指示I2C的Activity状态。
                                                       0：空闲；
                                                       1：忙。 */
        unsigned short  stop_det  : 1;  /* bit[9]    : STOP_DET中断，指示在I2C总线上是否发生了停止条件。
                                                       0：未发生；
                                                       1：已发生。 */
        unsigned short  start_det : 1;  /* bit[10]   : START_DET中断，指示在I2C总线上是否发生了开始条件。
                                                       0：未发生；
                                                       1：已发生。 */
        unsigned short  gen_call  : 1;  /* bit[11]   : GEN_CALL中断，一次general call请求被接收状态。
                                                       0：未被接收；
                                                       1：已被接收。&#13;I2C将接收到的数据放在RX Buffer中。 */
        unsigned short  reserved  : 4;  /* bit[12-15]: 保留。 */
    } reg;
} SOC_I2C_INTR_STAT_UNION;
#endif
#define SOC_I2C_INTR_STAT_rx_under_START   (0)
#define SOC_I2C_INTR_STAT_rx_under_END     (0)
#define SOC_I2C_INTR_STAT_rx_over_START    (1)
#define SOC_I2C_INTR_STAT_rx_over_END      (1)
#define SOC_I2C_INTR_STAT_rx_full_START    (2)
#define SOC_I2C_INTR_STAT_rx_full_END      (2)
#define SOC_I2C_INTR_STAT_tx_over_START    (3)
#define SOC_I2C_INTR_STAT_tx_over_END      (3)
#define SOC_I2C_INTR_STAT_tx_empty_START   (4)
#define SOC_I2C_INTR_STAT_tx_empty_END     (4)
#define SOC_I2C_INTR_STAT_rd_req_START     (5)
#define SOC_I2C_INTR_STAT_rd_req_END       (5)
#define SOC_I2C_INTR_STAT_tx_abrt_START    (6)
#define SOC_I2C_INTR_STAT_tx_abrt_END      (6)
#define SOC_I2C_INTR_STAT_rx_done_START    (7)
#define SOC_I2C_INTR_STAT_rx_done_END      (7)
#define SOC_I2C_INTR_STAT_activity_START   (8)
#define SOC_I2C_INTR_STAT_activity_END     (8)
#define SOC_I2C_INTR_STAT_stop_det_START   (9)
#define SOC_I2C_INTR_STAT_stop_det_END     (9)
#define SOC_I2C_INTR_STAT_start_det_START  (10)
#define SOC_I2C_INTR_STAT_start_det_END    (10)
#define SOC_I2C_INTR_STAT_gen_call_START   (11)
#define SOC_I2C_INTR_STAT_gen_call_END     (11)


/*****************************************************************************
 结构名    : SOC_I2C_INTR_MASK_UNION
 结构说明  : INTR_MASK 寄存器结构定义。地址偏移量:0x0030，初值:0x08FF，宽度:16
 寄存器说明: 中断屏蔽寄存器。Bit值为0表示屏蔽，为1表示不屏蔽。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  m_rx_under  : 1;  /* bit[0]    : RX_UNDER中断屏蔽。 */
        unsigned short  m_rx_over   : 1;  /* bit[1]    : RX_OVER中断屏蔽。 */
        unsigned short  m_rx_full   : 1;  /* bit[2]    : RX_FULL中断屏蔽。 */
        unsigned short  m_tx_over   : 1;  /* bit[3]    : TX_OVER中断屏蔽。 */
        unsigned short  m_tx_empty  : 1;  /* bit[4]    : TX_EMPTY中断屏蔽。 */
        unsigned short  m_rd_req    : 1;  /* bit[5]    : RD_REQ中断屏蔽。 */
        unsigned short  m_tx_abrt   : 1;  /* bit[6]    : TX_ABRT中断屏蔽。 */
        unsigned short  m_rx_done   : 1;  /* bit[7]    : RX_DONE中断屏蔽。 */
        unsigned short  m_activity  : 1;  /* bit[8]    : ACTIVITY中断屏蔽。 */
        unsigned short  m_stop_det  : 1;  /* bit[9]    : STOP_DET中断屏蔽。 */
        unsigned short  m_start_det : 1;  /* bit[10]   : START_DET中断屏蔽。 */
        unsigned short  m_gen_call  : 1;  /* bit[11]   : GEN_CALL中断屏蔽。 */
        unsigned short  reserved    : 4;  /* bit[12-15]: 保留。 */
    } reg;
} SOC_I2C_INTR_MASK_UNION;
#endif
#define SOC_I2C_INTR_MASK_m_rx_under_START   (0)
#define SOC_I2C_INTR_MASK_m_rx_under_END     (0)
#define SOC_I2C_INTR_MASK_m_rx_over_START    (1)
#define SOC_I2C_INTR_MASK_m_rx_over_END      (1)
#define SOC_I2C_INTR_MASK_m_rx_full_START    (2)
#define SOC_I2C_INTR_MASK_m_rx_full_END      (2)
#define SOC_I2C_INTR_MASK_m_tx_over_START    (3)
#define SOC_I2C_INTR_MASK_m_tx_over_END      (3)
#define SOC_I2C_INTR_MASK_m_tx_empty_START   (4)
#define SOC_I2C_INTR_MASK_m_tx_empty_END     (4)
#define SOC_I2C_INTR_MASK_m_rd_req_START     (5)
#define SOC_I2C_INTR_MASK_m_rd_req_END       (5)
#define SOC_I2C_INTR_MASK_m_tx_abrt_START    (6)
#define SOC_I2C_INTR_MASK_m_tx_abrt_END      (6)
#define SOC_I2C_INTR_MASK_m_rx_done_START    (7)
#define SOC_I2C_INTR_MASK_m_rx_done_END      (7)
#define SOC_I2C_INTR_MASK_m_activity_START   (8)
#define SOC_I2C_INTR_MASK_m_activity_END     (8)
#define SOC_I2C_INTR_MASK_m_stop_det_START   (9)
#define SOC_I2C_INTR_MASK_m_stop_det_END     (9)
#define SOC_I2C_INTR_MASK_m_start_det_START  (10)
#define SOC_I2C_INTR_MASK_m_start_det_END    (10)
#define SOC_I2C_INTR_MASK_m_gen_call_START   (11)
#define SOC_I2C_INTR_MASK_m_gen_call_END     (11)


/*****************************************************************************
 结构名    : SOC_I2C_RAW_INTR_STAT_UNION
 结构说明  : RAW_INTR_STAT 寄存器结构定义。地址偏移量:0x0034，初值:0x0000，宽度:16
 寄存器说明: 原始中断状态寄存器。Bit值为0表示无中断，为1表示有中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  r_rx_under  : 1;  /* bit[0]    : RX_UNDER原始中断状态。 */
        unsigned short  r_rx_over   : 1;  /* bit[1]    : RX_OVER原始中断状态。 */
        unsigned short  r_rx_full   : 1;  /* bit[2]    : RX_FULL原始中断状态。 */
        unsigned short  r_tx_over   : 1;  /* bit[3]    : TX_OVER原始中断状态。 */
        unsigned short  r_tx_empty  : 1;  /* bit[4]    : R_TX_EMPTY原始中断状态。 */
        unsigned short  r_rd_req    : 1;  /* bit[5]    : RD_REQ原始中断状态。 */
        unsigned short  r_tx_abrt   : 1;  /* bit[6]    : 原始中断状态。 */
        unsigned short  r_rx_done   : 1;  /* bit[7]    : RX_DONE原始中断状态。 */
        unsigned short  r_activity  : 1;  /* bit[8]    : ACTIVITY原始中断状态。 */
        unsigned short  r_stop_det  : 1;  /* bit[9]    : STOP_DET原始中断状态。 */
        unsigned short  r_start_det : 1;  /* bit[10]   : START_DET原始中断状态。 */
        unsigned short  r_gen_call  : 1;  /* bit[11]   : GEN_CALL原始中断状态。 */
        unsigned short  reserved    : 4;  /* bit[12-15]: 保留。 */
    } reg;
} SOC_I2C_RAW_INTR_STAT_UNION;
#endif
#define SOC_I2C_RAW_INTR_STAT_r_rx_under_START   (0)
#define SOC_I2C_RAW_INTR_STAT_r_rx_under_END     (0)
#define SOC_I2C_RAW_INTR_STAT_r_rx_over_START    (1)
#define SOC_I2C_RAW_INTR_STAT_r_rx_over_END      (1)
#define SOC_I2C_RAW_INTR_STAT_r_rx_full_START    (2)
#define SOC_I2C_RAW_INTR_STAT_r_rx_full_END      (2)
#define SOC_I2C_RAW_INTR_STAT_r_tx_over_START    (3)
#define SOC_I2C_RAW_INTR_STAT_r_tx_over_END      (3)
#define SOC_I2C_RAW_INTR_STAT_r_tx_empty_START   (4)
#define SOC_I2C_RAW_INTR_STAT_r_tx_empty_END     (4)
#define SOC_I2C_RAW_INTR_STAT_r_rd_req_START     (5)
#define SOC_I2C_RAW_INTR_STAT_r_rd_req_END       (5)
#define SOC_I2C_RAW_INTR_STAT_r_tx_abrt_START    (6)
#define SOC_I2C_RAW_INTR_STAT_r_tx_abrt_END      (6)
#define SOC_I2C_RAW_INTR_STAT_r_rx_done_START    (7)
#define SOC_I2C_RAW_INTR_STAT_r_rx_done_END      (7)
#define SOC_I2C_RAW_INTR_STAT_r_activity_START   (8)
#define SOC_I2C_RAW_INTR_STAT_r_activity_END     (8)
#define SOC_I2C_RAW_INTR_STAT_r_stop_det_START   (9)
#define SOC_I2C_RAW_INTR_STAT_r_stop_det_END     (9)
#define SOC_I2C_RAW_INTR_STAT_r_start_det_START  (10)
#define SOC_I2C_RAW_INTR_STAT_r_start_det_END    (10)
#define SOC_I2C_RAW_INTR_STAT_r_gen_call_START   (11)
#define SOC_I2C_RAW_INTR_STAT_r_gen_call_END     (11)


/*****************************************************************************
 结构名    : SOC_I2C_RX_TL_UNION
 结构说明  : RX_TL 寄存器结构定义。地址偏移量:0x0038，初值:0x0000，宽度:16
 寄存器说明: RX_FIFO的水线配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  rx_tl    : 8;  /* bit[0-7] : RX_FIFO的水线值，实际值等于配置值加1。
                                                     注意：当配置值超过FIFO深度（8）时被认为是8。 */
        unsigned short  reserved : 8;  /* bit[8-15]: 保留。 */
    } reg;
} SOC_I2C_RX_TL_UNION;
#endif
#define SOC_I2C_RX_TL_rx_tl_START     (0)
#define SOC_I2C_RX_TL_rx_tl_END       (7)


/*****************************************************************************
 结构名    : SOC_I2C_TX_TL_UNION
 结构说明  : TX_TL 寄存器结构定义。地址偏移量:0x003C，初值:0x0000，宽度:16
 寄存器说明: TX_FIFO的水线配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  tx_tl    : 8;  /* bit[0-7] : TX_FIFO的水线值。
                                                     注意：当配置值超过FIFO深度（8）时被认为是8。 */
        unsigned short  reserved : 8;  /* bit[8-15]: 保留。 */
    } reg;
} SOC_I2C_TX_TL_UNION;
#endif
#define SOC_I2C_TX_TL_tx_tl_START     (0)
#define SOC_I2C_TX_TL_tx_tl_END       (7)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_INTR_UNION
 结构说明  : CLR_INTR 寄存器结构定义。地址偏移量:0x0040，初值:0x0000，宽度:16
 寄存器说明: 组合及独立中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_intr : 1;  /* bit[0]   : 读此寄存器，清所有组合中断和独立中断，以及I2C_TX_ABRT_SOURCE寄存器。
                                                     注意：I2C_TX_ABRT_SOURCE[abrt_sbyte_norstrt]及其引发的组合中断无法被清除。 */
        unsigned short  reserved : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_INTR_UNION;
#endif
#define SOC_I2C_CLR_INTR_clr_intr_START  (0)
#define SOC_I2C_CLR_INTR_clr_intr_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_RX_UNDER_UNION
 结构说明  : CLR_RX_UNDER 寄存器结构定义。地址偏移量:0x0044，初值:0x0000，宽度:16
 寄存器说明: RX_UNDER中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_rx_under : 1;  /* bit[0]   : 读此寄存器，清RX_UNDER中断。 */
        unsigned short  reserved     : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_RX_UNDER_UNION;
#endif
#define SOC_I2C_CLR_RX_UNDER_clr_rx_under_START  (0)
#define SOC_I2C_CLR_RX_UNDER_clr_rx_under_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_RX_OVER_UNION
 结构说明  : CLR_RX_OVER 寄存器结构定义。地址偏移量:0x0048，初值:0x0000，宽度:16
 寄存器说明: RX_OVER中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_rx_over : 1;  /* bit[0]   : 读此寄存器，清RX_OVER中断。 */
        unsigned short  reserved    : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_RX_OVER_UNION;
#endif
#define SOC_I2C_CLR_RX_OVER_clr_rx_over_START  (0)
#define SOC_I2C_CLR_RX_OVER_clr_rx_over_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_TX_OVER_UNION
 结构说明  : CLR_TX_OVER 寄存器结构定义。地址偏移量:0x004C，初值:0x0000，宽度:16
 寄存器说明: TX_OVER中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_tx_over : 1;  /* bit[0]   : 读此寄存器，清TX_OVER中断。 */
        unsigned short  reserved    : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_TX_OVER_UNION;
#endif
#define SOC_I2C_CLR_TX_OVER_clr_tx_over_START  (0)
#define SOC_I2C_CLR_TX_OVER_clr_tx_over_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_RD_REQ_UNION
 结构说明  : CLR_RD_REQ 寄存器结构定义。地址偏移量:0x0050，初值:0x0000，宽度:16
 寄存器说明: RD_REQ中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_rd_req : 1;  /* bit[0]   : 读此寄存器，清RD_REQ中断 */
        unsigned short  reserved   : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_RD_REQ_UNION;
#endif
#define SOC_I2C_CLR_RD_REQ_clr_rd_req_START  (0)
#define SOC_I2C_CLR_RD_REQ_clr_rd_req_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_TX_ABRT_UNION
 结构说明  : CLR_TX_ABRT 寄存器结构定义。地址偏移量:0x0054，初值:0x0000，宽度:16
 寄存器说明: ABRT中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_tx_abrt : 1;  /* bit[0]   : 读此寄存器，清TX_ABRT中断和I2C_TX_ABRT_SOURCE寄存器。 */
        unsigned short  reserved    : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_TX_ABRT_UNION;
#endif
#define SOC_I2C_CLR_TX_ABRT_clr_tx_abrt_START  (0)
#define SOC_I2C_CLR_TX_ABRT_clr_tx_abrt_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_RX_DONE_UNION
 结构说明  : CLR_RX_DONE 寄存器结构定义。地址偏移量:0x0058，初值:0x0000，宽度:16
 寄存器说明: RX_DONE中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_rx_done : 1;  /* bit[0]   : 读此寄存器，清RX_DONE中断。 */
        unsigned short  reserved    : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_RX_DONE_UNION;
#endif
#define SOC_I2C_CLR_RX_DONE_clr_rx_done_START  (0)
#define SOC_I2C_CLR_RX_DONE_clr_rx_done_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_ACTIVITY_UNION
 结构说明  : CLR_ACTIVITY 寄存器结构定义。地址偏移量:0x005C，初值:0x0000，宽度:16
 寄存器说明: ACTIVITY状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_activity : 1;  /* bit[0]   : 读此寄存器，可获得ACTIVITY中断状态，硬件自动清0。 */
        unsigned short  reserved     : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_ACTIVITY_UNION;
#endif
#define SOC_I2C_CLR_ACTIVITY_clr_activity_START  (0)
#define SOC_I2C_CLR_ACTIVITY_clr_activity_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_STOP_DET_UNION
 结构说明  : CLR_STOP_DET 寄存器结构定义。地址偏移量:0x0060，初值:0x0000，宽度:16
 寄存器说明: STOP_DET中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_stop_det : 1;  /* bit[0]   : 读此寄存器，清STOP_DET中断。 */
        unsigned short  reserved     : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_STOP_DET_UNION;
#endif
#define SOC_I2C_CLR_STOP_DET_clr_stop_det_START  (0)
#define SOC_I2C_CLR_STOP_DET_clr_stop_det_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_START_DET_UNION
 结构说明  : CLR_START_DET 寄存器结构定义。地址偏移量:0x0064，初值:0x0000，宽度:16
 寄存器说明: START_DET中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_start_det : 1;  /* bit[0]   : 读此寄存器，清START_DET中断。 */
        unsigned short  reserved      : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_START_DET_UNION;
#endif
#define SOC_I2C_CLR_START_DET_clr_start_det_START  (0)
#define SOC_I2C_CLR_START_DET_clr_start_det_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_CLR_GEN_CALL_UNION
 结构说明  : CLR_GEN_CALL 寄存器结构定义。地址偏移量:0x0068，初值:0x0000，宽度:16
 寄存器说明: GEN_CALL中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  clr_gen_call : 1;  /* bit[0]   : 读此寄存器，清GEN_CALL中断。 */
        unsigned short  reserved     : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_CLR_GEN_CALL_UNION;
#endif
#define SOC_I2C_CLR_GEN_CALL_clr_gen_call_START  (0)
#define SOC_I2C_CLR_GEN_CALL_clr_gen_call_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_ENABLE_UNION
 结构说明  : ENABLE 寄存器结构定义。地址偏移量:0x006C，初值:0x0000，宽度:16
 寄存器说明: I2C使能寄存器。用于关闭或使能I2C。
            当I2C处于数据传输状态时（即I2C_STATUS[activity]为1），I2C可以被关闭，但需要注意：
            (I)如果I2C处于发送状态时关闭，则I2C在完成当前字节发送后停止继续传输，同时清除TX_FIFO的数据。
            (I)如果I2C处于接收状态时关闭，则I2C在接收完当前字节后不响应这次传输，即发出NACK。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  i2c_enable : 1;  /* bit[0]   : I2C的使能寄存器。
                                                       0：禁止；
                                                       1：使能。 */
        unsigned short  reserved   : 15; /* bit[1-15]: 保留。 */
    } reg;
} SOC_I2C_ENABLE_UNION;
#endif
#define SOC_I2C_ENABLE_i2c_enable_START  (0)
#define SOC_I2C_ENABLE_i2c_enable_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_STATUS_UNION
 结构说明  : STATUS 寄存器结构定义。地址偏移量:0x0070，初值:0x0006，宽度:16
 寄存器说明: I2C状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  activity     : 1;  /* bit[0]   : I2C总线状态。
                                                         0：空闲；
                                                         1：忙。 */
        unsigned short  tfnf         : 1;  /* bit[1]   : TX_FIFO是否已满。
                                                         0：已满；
                                                         1：未满。 */
        unsigned short  tfe          : 1;  /* bit[2]   : TX_FIFO是否已空。
                                                         0：未空；
                                                         1：已空。 */
        unsigned short  rfne         : 1;  /* bit[3]   : RX_FIFO是否已空。
                                                         0：已空；
                                                         1：未空。 */
        unsigned short  rff          : 1;  /* bit[4]   : RX_FIFO是否已满。
                                                         0：未满；
                                                         1：已满。 */
        unsigned short  mst_activity : 1;  /* bit[5]   : 主机状态机活动状态。
                                                         0：主机状态机处于IDLE态，DW_apb_i2c主机部分为非活跃态；
                                                         1：主机状态机处于非IDLE态，DW_apb_i2c主机部分为活跃态。 */
        unsigned short  slv_activity : 1;  /* bit[6]   : 从机状态机活动状态。
                                                         0：从机状态机处于IDLE态，DW_apb_i2c从机部分为非活跃态；
                                                         1：从机状态机处于非IDLE态，DW_apb_i2c从机部分为活跃态。 */
        unsigned short  reserved     : 9;  /* bit[7-15]: 保留。 */
    } reg;
} SOC_I2C_STATUS_UNION;
#endif
#define SOC_I2C_STATUS_activity_START      (0)
#define SOC_I2C_STATUS_activity_END        (0)
#define SOC_I2C_STATUS_tfnf_START          (1)
#define SOC_I2C_STATUS_tfnf_END            (1)
#define SOC_I2C_STATUS_tfe_START           (2)
#define SOC_I2C_STATUS_tfe_END             (2)
#define SOC_I2C_STATUS_rfne_START          (3)
#define SOC_I2C_STATUS_rfne_END            (3)
#define SOC_I2C_STATUS_rff_START           (4)
#define SOC_I2C_STATUS_rff_END             (4)
#define SOC_I2C_STATUS_mst_activity_START  (5)
#define SOC_I2C_STATUS_mst_activity_END    (5)
#define SOC_I2C_STATUS_slv_activity_START  (6)
#define SOC_I2C_STATUS_slv_activity_END    (6)


/*****************************************************************************
 结构名    : SOC_I2C_TXFLR_UNION
 结构说明  : TXFLR 寄存器结构定义。地址偏移量:0x0074，初值:0x0000，宽度:16
 寄存器说明: TX_FIFO中的数据个数指示寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  txflr    : 7;  /* bit[0-6] : 指示TX_FIFO中的数据个数。 */
        unsigned short  reserved : 9;  /* bit[7-15]: 保留。 */
    } reg;
} SOC_I2C_TXFLR_UNION;
#endif
#define SOC_I2C_TXFLR_txflr_START     (0)
#define SOC_I2C_TXFLR_txflr_END       (6)


/*****************************************************************************
 结构名    : SOC_I2C_RXFLR_UNION
 结构说明  : RXFLR 寄存器结构定义。地址偏移量:0x0078，初值:0x0000，宽度:16
 寄存器说明: RX_FIFO中的数据个数指示寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  rxflr    : 7;  /* bit[0-6] : 指示RX_FIFO中的数据个数。 */
        unsigned short  reserved : 9;  /* bit[7-15]: 保留。 */
    } reg;
} SOC_I2C_RXFLR_UNION;
#endif
#define SOC_I2C_RXFLR_rxflr_START     (0)
#define SOC_I2C_RXFLR_rxflr_END       (6)


/*****************************************************************************
 结构名    : SOC_I2C_SDA_HOLD_UNION
 结构说明  : SDA_HOLD 寄存器结构定义。地址偏移量:0x007C，初值:0x00000001，宽度:32
 寄存器说明: SDA保持时间长度寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  i2c_sda_hold : 16; /* bit[0-15] : SDA 保持时间（以ic_clk为单位） */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_I2C_SDA_HOLD_UNION;
#endif
#define SOC_I2C_SDA_HOLD_i2c_sda_hold_START  (0)
#define SOC_I2C_SDA_HOLD_i2c_sda_hold_END    (15)


/*****************************************************************************
 结构名    : SOC_I2C_TX_ABRT_SOURCE_UNION
 结构说明  : TX_ABRT_SOURCE 寄存器结构定义。地址偏移量:0x0080，初值:0x00000000，宽度:32
 寄存器说明: TX_ABRT的源头中断寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  abrt_7b_addr_noack   : 1;  /* bit[0]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备时发送的7bit地址没被响应。 */
        unsigned int  abrt_10addr1_noack   : 1;  /* bit[1]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备时发送的10bit地址的第1字节没被响应。 */
        unsigned int  abrt_10addr2_noack   : 1;  /* bit[2]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备时发送的10bit地址的第2字节没被响应。 */
        unsigned int  abrt_txdata_noack    : 1;  /* bit[3]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备时发送的地址被从设备响应，而发送的数据没被响应。 */
        unsigned int  abrt_gcall_noack     : 1;  /* bit[4]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备时发出General call，但没被响应。 */
        unsigned int  abrt_gcall_read      : 1;  /* bit[5]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备时发出General call，而CPU向I2C发出读命令。 */
        unsigned int  abrt_hs_ackdet       : 1;  /* bit[6]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备进行高速传输时，高速主机码被响应。 */
        unsigned int  abrt_sbyte_ackdet    : 1;  /* bit[7]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备时发出Start byte而被响应。 */
        unsigned int  abrt_hs_norstrt      : 1;  /* bit[8]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备不支持Restart功能时，进行高速操作。 */
        unsigned int  abrt_sbyte_norstrt   : 1;  /* bit[9]    : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备不支持Restart功能时，发送Start byte。 */
        unsigned int  abrt_10b_rd_norstrt  : 1;  /* bit[10]   : 错误中断原因指示。
                                                                0：复位值；
                                                                1：作为主设备不支持Restart功能时，对10bit地址的从设备发出了读命令。 */
        unsigned int  abrt_master_dis      : 1;  /* bit[11]   : 错误中断原因指示。
                                                                0：复位值；
                                                                1：Master功能禁用时，发起Master操作。 */
        unsigned int  abrt_lost            : 1;  /* bit[12]   : 总线仲裁丢失中断指示。
                                                                0：复位值；
                                                                1：主机仲裁丢失，或当I2C_TX_ABRT_SOURCE[14]被设置时，表示从机的传输失去仲裁。 */
        unsigned int  abrt_slvflush_txfifo : 1;  /* bit[13]   : 0：复位值；
                                                                1：在从机收到一个读命令，并且有相同的数据存在于发送FIFO中时，从机会发一个TX_ARBT中断，来冲掉发送FIFO中的原有数据。 */
        unsigned int  abrt_slv_arblost     : 1;  /* bit[14]   : 0：复位值；
                                                                1：从机在向远端主机传输数据时丢失总线仲裁，此时I2C_TX_ABRT_SOURCE[14]同样被设置。 */
        unsigned int  abrt_slvrd_intx      : 1;  /* bit[15]   : 0：复位值；
                                                                1：处理器响应从机请求传送数据到远端主机，而用户向命令寄存器写1时产生的中断。 */
        unsigned int  reserved             : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_I2C_TX_ABRT_SOURCE_UNION;
#endif
#define SOC_I2C_TX_ABRT_SOURCE_abrt_7b_addr_noack_START    (0)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_7b_addr_noack_END      (0)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_10addr1_noack_START    (1)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_10addr1_noack_END      (1)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_10addr2_noack_START    (2)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_10addr2_noack_END      (2)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_txdata_noack_START     (3)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_txdata_noack_END       (3)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_gcall_noack_START      (4)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_gcall_noack_END        (4)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_gcall_read_START       (5)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_gcall_read_END         (5)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_hs_ackdet_START        (6)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_hs_ackdet_END          (6)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_sbyte_ackdet_START     (7)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_sbyte_ackdet_END       (7)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_hs_norstrt_START       (8)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_hs_norstrt_END         (8)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_sbyte_norstrt_START    (9)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_sbyte_norstrt_END      (9)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_10b_rd_norstrt_START   (10)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_10b_rd_norstrt_END     (10)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_master_dis_START       (11)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_master_dis_END         (11)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_lost_START             (12)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_lost_END               (12)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_slvflush_txfifo_START  (13)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_slvflush_txfifo_END    (13)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_slv_arblost_START      (14)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_slv_arblost_END        (14)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_slvrd_intx_START       (15)
#define SOC_I2C_TX_ABRT_SOURCE_abrt_slvrd_intx_END         (15)


/*****************************************************************************
 结构名    : SOC_I2C_DMA_CR_UNION
 结构说明  : DMA_CR 寄存器结构定义。地址偏移量:0x0088，初值:0x0000，宽度:16
 寄存器说明: I2C DMA通道开启控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  rdmae    : 1;  /* bit[0]   : 是否打开RX_FIFO的DMA通道。
                                                     0：不打开；
                                                     1：打开。 */
        unsigned short  tdmae    : 1;  /* bit[1]   : 是否打开TX_FIFO的DMA通道。
                                                     0：不打开；
                                                     1：打开。 */
        unsigned short  reserved : 14; /* bit[2-15]: 保留。 */
    } reg;
} SOC_I2C_DMA_CR_UNION;
#endif
#define SOC_I2C_DMA_CR_rdmae_START     (0)
#define SOC_I2C_DMA_CR_rdmae_END       (0)
#define SOC_I2C_DMA_CR_tdmae_START     (1)
#define SOC_I2C_DMA_CR_tdmae_END       (1)


/*****************************************************************************
 结构名    : SOC_I2C_DMA_TDLR_UNION
 结构说明  : DMA_TDLR 寄存器结构定义。地址偏移量:0x008C，初值:0x0000，宽度:16
 寄存器说明: TX_FIFO进行DMA操作时的水线配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  dmatdl   : 6;  /* bit[0-5] : TX_FIFO DMA操作时的水线值。 */
        unsigned short  reserved : 10; /* bit[6-15]: 保留。 */
    } reg;
} SOC_I2C_DMA_TDLR_UNION;
#endif
#define SOC_I2C_DMA_TDLR_dmatdl_START    (0)
#define SOC_I2C_DMA_TDLR_dmatdl_END      (5)


/*****************************************************************************
 结构名    : SOC_I2C_DMA_RDLR_UNION
 结构说明  : DMA_RDLR 寄存器结构定义。地址偏移量:0x0090，初值:0x0000，宽度:16
 寄存器说明: RX_FIFO进行DMA操作时的水线配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  dmardl   : 6;  /* bit[0-5] : RX_FIFO DMA操作时的水线值，实际值等于配置值加1。 */
        unsigned short  reserved : 10; /* bit[6-15]: 保留。 */
    } reg;
} SOC_I2C_DMA_RDLR_UNION;
#endif
#define SOC_I2C_DMA_RDLR_dmardl_START    (0)
#define SOC_I2C_DMA_RDLR_dmardl_END      (5)


/*****************************************************************************
 结构名    : SOC_I2C_SDA_SETUP_UNION
 结构说明  : SDA_SETUP 寄存器结构定义。地址偏移量:0x0094，初值:0x00000064，宽度:32
 寄存器说明: SDA建立时间长度寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  i2c_sda_setup : 8;  /* bit[0-7] : SDA保持时间寄存器。 */
        unsigned int  reserved      : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_I2C_SDA_SETUP_UNION;
#endif
#define SOC_I2C_SDA_SETUP_i2c_sda_setup_START  (0)
#define SOC_I2C_SDA_SETUP_i2c_sda_setup_END    (7)


/*****************************************************************************
 结构名    : SOC_I2C_ACK_GENERAL_CALL_UNION
 结构说明  : ACK_GENERAL_CALL 寄存器结构定义。地址偏移量:0x0098，初值:0x00000001，宽度:32
 寄存器说明: 全呼叫响应寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ack_gen_call : 1;  /* bit[0]   : 全呼叫响应寄存器。
                                                       0：收到全呼叫时不产生全呼叫中断；
                                                       1：收到全呼叫时产生一个ACK。 */
        unsigned int  reserved     : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_I2C_ACK_GENERAL_CALL_UNION;
#endif
#define SOC_I2C_ACK_GENERAL_CALL_ack_gen_call_START  (0)
#define SOC_I2C_ACK_GENERAL_CALL_ack_gen_call_END    (0)


/*****************************************************************************
 结构名    : SOC_I2C_ENABLE_STATUS_UNION
 结构说明  : ENABLE_STATUS 寄存器结构定义。地址偏移量:0x009C，初值:0x00000000，宽度:32
 寄存器说明: I2C使能状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  i2c_en                  : 1;  /* bit[0]   : DW_apb_i2c使能状态 */
        unsigned int  slv_disabled_while_busy : 1;  /* bit[1]   : 指示从机操作是否因I2C_ENABlE由1变0而被中止 */
        unsigned int  slv_rx_data_lost        : 1;  /* bit[2]   : 指示从机接收器操作在至少收到1字节数据时，是否因I2C_ENABlE由1变0而被中止 */
        unsigned int  reserved                : 29; /* bit[3-31]: 保留。 */
    } reg;
} SOC_I2C_ENABLE_STATUS_UNION;
#endif
#define SOC_I2C_ENABLE_STATUS_i2c_en_START                   (0)
#define SOC_I2C_ENABLE_STATUS_i2c_en_END                     (0)
#define SOC_I2C_ENABLE_STATUS_slv_disabled_while_busy_START  (1)
#define SOC_I2C_ENABLE_STATUS_slv_disabled_while_busy_END    (1)
#define SOC_I2C_ENABLE_STATUS_slv_rx_data_lost_START         (2)
#define SOC_I2C_ENABLE_STATUS_slv_rx_data_lost_END           (2)


/*****************************************************************************
 结构名    : SOC_I2C_FS_SPKLEN_UNION
 结构说明  : FS_SPKLEN 寄存器结构定义。地址偏移量:0x00A0，初值:0x00000005，宽度:32
 寄存器说明: 标速及全速模式下毛刺抑制长度寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  i2c_fs_spklen : 8;  /* bit[0-7] : 指定标速及全速模式下毛刺抑制长度 */
        unsigned int  reserved      : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_I2C_FS_SPKLEN_UNION;
#endif
#define SOC_I2C_FS_SPKLEN_i2c_fs_spklen_START  (0)
#define SOC_I2C_FS_SPKLEN_i2c_fs_spklen_END    (7)


/*****************************************************************************
 结构名    : SOC_I2C_HS_SPKLEN_UNION
 结构说明  : HS_SPKLEN 寄存器结构定义。地址偏移量:0x00A4，初值:0x00000002，宽度:32
 寄存器说明: 高速模式下毛刺抑制长度寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  i2c_hs_spklen : 8;  /* bit[0-7] : 指定高速模式下毛刺抑制长度 */
        unsigned int  reserved      : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_I2C_HS_SPKLEN_UNION;
#endif
#define SOC_I2C_HS_SPKLEN_i2c_hs_spklen_START  (0)
#define SOC_I2C_HS_SPKLEN_i2c_hs_spklen_END    (7)


/*****************************************************************************
 结构名    : SOC_I2C_COMP_PARAM_1_UNION
 结构说明  : COMP_PARAM_1 寄存器结构定义。地址偏移量:0x00F4，初值:0x003F3FEE，宽度:32
 寄存器说明: 参数寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  apb_data_width     : 2;  /* bit[0-1]  : apb数据线位宽 */
        unsigned int  max_speed_mode     : 2;  /* bit[2-3]  : 最大速度模式 */
        unsigned int  hc_count_values    : 1;  /* bit[4]    : *CNT 寄存器是否置写属性 */
        unsigned int  intr_io            : 1;  /* bit[5]    : 中断组合方式 */
        unsigned int  has_dma            : 1;  /* bit[6]    : 是否包含DMA信号 */
        unsigned int  add_encoded_params : 1;  /* bit[7]    : 增加编码参数 */
        unsigned int  rx_buffer_depth    : 8;  /* bit[8-15] : 接收FIFO深度 */
        unsigned int  tx_buffer_depth    : 8;  /* bit[16-23]: 发送FIFO深度 */
        unsigned int  reserved           : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_I2C_COMP_PARAM_1_UNION;
#endif
#define SOC_I2C_COMP_PARAM_1_apb_data_width_START      (0)
#define SOC_I2C_COMP_PARAM_1_apb_data_width_END        (1)
#define SOC_I2C_COMP_PARAM_1_max_speed_mode_START      (2)
#define SOC_I2C_COMP_PARAM_1_max_speed_mode_END        (3)
#define SOC_I2C_COMP_PARAM_1_hc_count_values_START     (4)
#define SOC_I2C_COMP_PARAM_1_hc_count_values_END       (4)
#define SOC_I2C_COMP_PARAM_1_intr_io_START             (5)
#define SOC_I2C_COMP_PARAM_1_intr_io_END               (5)
#define SOC_I2C_COMP_PARAM_1_has_dma_START             (6)
#define SOC_I2C_COMP_PARAM_1_has_dma_END               (6)
#define SOC_I2C_COMP_PARAM_1_add_encoded_params_START  (7)
#define SOC_I2C_COMP_PARAM_1_add_encoded_params_END    (7)
#define SOC_I2C_COMP_PARAM_1_rx_buffer_depth_START     (8)
#define SOC_I2C_COMP_PARAM_1_rx_buffer_depth_END       (15)
#define SOC_I2C_COMP_PARAM_1_tx_buffer_depth_START     (16)
#define SOC_I2C_COMP_PARAM_1_tx_buffer_depth_END       (23)


/*****************************************************************************
 结构名    : SOC_I2C_COMP_VERSION_UNION
 结构说明  : COMP_VERSION 寄存器结构定义。地址偏移量:0x00F8，初值:0x3131352A，宽度:32
 寄存器说明: 版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  i2c_comp_version : 32; /* bit[0-31]: DW_apb_i2c版本信息 */
    } reg;
} SOC_I2C_COMP_VERSION_UNION;
#endif
#define SOC_I2C_COMP_VERSION_i2c_comp_version_START  (0)
#define SOC_I2C_COMP_VERSION_i2c_comp_version_END    (31)


/*****************************************************************************
 结构名    : SOC_I2C_COMP_TYPE_UNION
 结构说明  : COMP_TYPE 寄存器结构定义。地址偏移量:0x00FC，初值:0x44570140，宽度:32
 寄存器说明: DesignWareIP类型寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  i2c_comp_type : 32; /* bit[0-31]: DesignWareIP类型 */
    } reg;
} SOC_I2C_COMP_TYPE_UNION;
#endif
#define SOC_I2C_COMP_TYPE_i2c_comp_type_START  (0)
#define SOC_I2C_COMP_TYPE_i2c_comp_type_END    (31)






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

#endif /* end of soc_i2c_interface.h */
