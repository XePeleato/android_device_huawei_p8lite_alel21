

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_SSP_INTERFACE_H__
#define __SOC_SSP_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) register_SPI
 ****************************************************************************/
/* 寄存器说明：控制寄存器。
   位域定义UNION结构:  SOC_SSP_SPICR0_UNION */
#define SOC_SSP_SPICR0_ADDR(base)                     ((base) + (0x000))

/* 寄存器说明：控制寄存器。
   位域定义UNION结构:  SOC_SSP_SPICR1_UNION */
#define SOC_SSP_SPICR1_ADDR(base)                     ((base) + (0x004))

/* 寄存器说明：数据寄存器。
   位域定义UNION结构:  SOC_SSP_SPIDR_UNION */
#define SOC_SSP_SPIDR_ADDR(base)                      ((base) + (0x008))

/* 寄存器说明：状态寄存器。
   位域定义UNION结构:  SOC_SSP_SPISR_UNION */
#define SOC_SSP_SPISR_ADDR(base)                      ((base) + (0x00C))

/* 寄存器说明：时钟分频寄存器。
   位域定义UNION结构:  SOC_SSP_SPICPSR_UNION */
#define SOC_SSP_SPICPSR_ADDR(base)                    ((base) + (0x010))

/* 寄存器说明：中断屏蔽设置或清除寄存器。
   位域定义UNION结构:  SOC_SSP_SPIMISC_UNION */
#define SOC_SSP_SPIMISC_ADDR(base)                    ((base) + (0x014))

/* 寄存器说明：原始中断状态寄存器。
   位域定义UNION结构:  SOC_SSP_SPIRIS_UNION */
#define SOC_SSP_SPIRIS_ADDR(base)                     ((base) + (0x018))

/* 寄存器说明：中断清除寄存器。
   位域定义UNION结构:  SOC_SSP_SPIICR_UNION */
#define SOC_SSP_SPIICR_ADDR(base)                     ((base) + (0x020))

/* 寄存器说明：DMA控制寄存器。
   位域定义UNION结构:  SOC_SSP_SPIDMACR_UNION */
#define SOC_SSP_SPIDMACR_ADDR(base)                   ((base) + (0x024))

/* 寄存器说明：发送FIFO控制寄存器。该寄存器只使用于SPI2。
   位域定义UNION结构:  SOC_SSP_SPITXFIFOCR_UNION */
#define SOC_SSP_SPITXFIFOCR_ADDR(base)                ((base) + (0x028))

/* 寄存器说明：接收FIFO控制寄存器。该寄存器只使用于SPI2。
   位域定义UNION结构:  SOC_SSP_SPIRXFIFOCR_UNION */
#define SOC_SSP_SPIRXFIFOCR_ADDR(base)                ((base) + (0x02C))





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
                     (1/1) register_SPI
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_SSP_SPICR0_UNION
 结构说明  : SPICR0 寄存器结构定义。地址偏移量:0x000，初值:0x0000，宽度:16
 寄存器说明: 控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  dss : 4;  /* bit[0-3] : 设置数据大小。
                                                0011：4bit；
                                                0100：5bit；
                                                0101：6bit；
                                                0110：7bit；
                                                0111：8bit；
                                                1000：9bit；
                                                1001：10bit；
                                                1010：11bit；
                                                1011：12bit；
                                                1100：13bit；
                                                1101：14bit；
                                                1110：15bit；
                                                1111：16bit；
                                                其他：保留。 */
        unsigned short  frf : 2;  /* bit[4-5] : 帧格式。
                                                00：SPI帧格式；
                                                其他：保留。 */
        unsigned short  spo : 1;  /* bit[6]   : SPICLKOUT极性。 */
        unsigned short  sph : 1;  /* bit[7]   : SPICLKOUT相位。 */
        unsigned short  scr : 8;  /* bit[8-15]: 串行时钟率，取值范围0～255。SCR的值用于产生SPI发送和接收的比特率，公式为Fspiclk/(CPSDVSR x(1+SCR))。
                                                CPSDVSR是一个2～254之间的偶数，由寄存器SPICPSR配置。 */
    } reg;
} SOC_SSP_SPICR0_UNION;
#endif
#define SOC_SSP_SPICR0_dss_START  (0)
#define SOC_SSP_SPICR0_dss_END    (3)
#define SOC_SSP_SPICR0_frf_START  (4)
#define SOC_SSP_SPICR0_frf_END    (5)
#define SOC_SSP_SPICR0_spo_START  (6)
#define SOC_SSP_SPICR0_spo_END    (6)
#define SOC_SSP_SPICR0_sph_START  (7)
#define SOC_SSP_SPICR0_sph_END    (7)
#define SOC_SSP_SPICR0_scr_START  (8)
#define SOC_SSP_SPICR0_scr_END    (15)


/*****************************************************************************
 结构名    : SOC_SSP_SPICR1_UNION
 结构说明  : SPICR1 寄存器结构定义。地址偏移量:0x004，初值:0x0000，宽度:16
 寄存器说明: 控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  lbm      : 1;  /* bit[0]   : 设置环回模式。
                                                     0：正常的串行接口操作使能；
                                                     1：发送串行移位寄存器的输出在内部连接到接收串行移位寄存器的输入上。 */
        unsigned short  sse      : 1;  /* bit[1]   : 设置SPI使能。
                                                     0：禁止；
                                                     1：使能。 */
        unsigned short  ms       : 1;  /* bit[2]   : 设置Master或者Slave模式，此位只能在SPI处于非使能状态时改变。
                                                     0：Master模式（默认）；
                                                     1：Slave模式（仅SPI2支持）。
                                                     注意：SSP2在高速模式下，只做SLAVE，配置该位为0无效。 */
        unsigned short  reserved : 13; /* bit[3-15]: 保留。 */
    } reg;
} SOC_SSP_SPICR1_UNION;
#endif
#define SOC_SSP_SPICR1_lbm_START       (0)
#define SOC_SSP_SPICR1_lbm_END         (0)
#define SOC_SSP_SPICR1_sse_START       (1)
#define SOC_SSP_SPICR1_sse_END         (1)
#define SOC_SSP_SPICR1_ms_START        (2)
#define SOC_SSP_SPICR1_ms_END          (2)


/*****************************************************************************
 结构名    : SOC_SSP_SPIDR_UNION
 结构说明  : SPIDR 寄存器结构定义。地址偏移量:0x008，初值:0x0000，宽度:16
 寄存器说明: 数据寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  data : 16; /* bit[0-15]: 发送/接收FIFO。
                                                 读：接收FIFO；
                                                 写：发送FIFO。
                                                 如果数据比特数少于16则必须右对齐。发送逻辑将忽略高位未使用的比特位，接收逻辑则自动将数据右对齐。 */
    } reg;
} SOC_SSP_SPIDR_UNION;
#endif
#define SOC_SSP_SPIDR_data_START  (0)
#define SOC_SSP_SPIDR_data_END    (15)


/*****************************************************************************
 结构名    : SOC_SSP_SPISR_UNION
 结构说明  : SPISR 寄存器结构定义。地址偏移量:0x00C，初值:0x0000，宽度:16
 寄存器说明: 状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  tfe      : 1;  /* bit[0]   : 发送FIFO是否已空。
                                                     0：未空；
                                                     1：已空。 */
        unsigned short  tnf      : 1;  /* bit[1]   : 发送FIFO是否未满。
                                                     0：已满；
                                                     1：未满。 */
        unsigned short  rne      : 1;  /* bit[2]   : 接收FIFO是否未空。
                                                     0：已空；
                                                     1：未空。 */
        unsigned short  rff      : 1;  /* bit[3]   : 接收FIFO是否已满。
                                                     0：未满；
                                                     1：已满。 */
        unsigned short  bsy      : 1;  /* bit[4]   : SPI忙标记。
                                                     0：空闲；
                                                     1：忙。 */
        unsigned short  reserved : 11; /* bit[5-15]: 保留。 */
    } reg;
} SOC_SSP_SPISR_UNION;
#endif
#define SOC_SSP_SPISR_tfe_START       (0)
#define SOC_SSP_SPISR_tfe_END         (0)
#define SOC_SSP_SPISR_tnf_START       (1)
#define SOC_SSP_SPISR_tnf_END         (1)
#define SOC_SSP_SPISR_rne_START       (2)
#define SOC_SSP_SPISR_rne_END         (2)
#define SOC_SSP_SPISR_rff_START       (3)
#define SOC_SSP_SPISR_rff_END         (3)
#define SOC_SSP_SPISR_bsy_START       (4)
#define SOC_SSP_SPISR_bsy_END         (4)


/*****************************************************************************
 结构名    : SOC_SSP_SPICPSR_UNION
 结构说明  : SPICPSR 寄存器结构定义。地址偏移量:0x010，初值:0x0000，宽度:16
 寄存器说明: 时钟分频寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  cpsdvsr  : 8;  /* bit[0-7] : 时钟分频因子。此值必须是2～254之间的偶数，取决于输入时钟SPICLK的频率。最低位读作0。 */
        unsigned short  reserved : 8;  /* bit[8-15]: 保留。 */
    } reg;
} SOC_SSP_SPICPSR_UNION;
#endif
#define SOC_SSP_SPICPSR_cpsdvsr_START   (0)
#define SOC_SSP_SPICPSR_cpsdvsr_END     (7)


/*****************************************************************************
 结构名    : SOC_SSP_SPIMISC_UNION
 结构说明  : SPIMISC 寄存器结构定义。地址偏移量:0x014，初值:0x0000，宽度:16
 寄存器说明: 中断屏蔽设置或清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  rorim    : 1;  /* bit[0]   : 接收溢出中断屏蔽。
                                                     0：接收FIFO溢出中断屏蔽；
                                                     1：接收FIFO溢出中断不屏蔽。 */
        unsigned short  rtim     : 1;  /* bit[1]   : 接收超时中断。
                                                     0：接收超时中断屏蔽；
                                                     1：接收超时中断不屏蔽。 */
        unsigned short  rxim     : 1;  /* bit[2]   : 接收FIFO中断屏蔽。
                                                     0：半空或更少情况下中断被屏蔽；
                                                     1：半空或更少情况下中断未被屏蔽。 */
        unsigned short  txim     : 1;  /* bit[3]   : 发送FIFO中断屏蔽。
                                                     0：半空或更少情况下中断被屏蔽；
                                                     1：半空或更少情况下中断未被屏蔽。 */
        unsigned short  reserved : 12; /* bit[4-15]: 域说明。 */
    } reg;
} SOC_SSP_SPIMISC_UNION;
#endif
#define SOC_SSP_SPIMISC_rorim_START     (0)
#define SOC_SSP_SPIMISC_rorim_END       (0)
#define SOC_SSP_SPIMISC_rtim_START      (1)
#define SOC_SSP_SPIMISC_rtim_END        (1)
#define SOC_SSP_SPIMISC_rxim_START      (2)
#define SOC_SSP_SPIMISC_rxim_END        (2)
#define SOC_SSP_SPIMISC_txim_START      (3)
#define SOC_SSP_SPIMISC_txim_END        (3)


/*****************************************************************************
 结构名    : SOC_SSP_SPIRIS_UNION
 结构说明  : SPIRIS 寄存器结构定义。地址偏移量:0x018，初值:0x0000，宽度:16
 寄存器说明: 原始中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  rorros   : 1;  /* bit[0]   : 接收溢出中断的原始中断状态。
                                                     0：无中断；
                                                     1：有中断。 */
        unsigned short  rttis    : 1;  /* bit[1]   : 接收超时中断的原始中断状态。
                                                     0：无中断；
                                                     1：有中断。 */
        unsigned short  rxris    : 1;  /* bit[2]   : 接收FIFO中断的原始中断状态。
                                                     0：无中断；
                                                     1：有中断。 */
        unsigned short  txris    : 1;  /* bit[3]   : 发送FIFO中断的原始中断状态。
                                                     0：无中断；
                                                     1：有中断。 */
        unsigned short  reserved : 12; /* bit[4-15]: 保留。 */
    } reg;
} SOC_SSP_SPIRIS_UNION;
#endif
#define SOC_SSP_SPIRIS_rorros_START    (0)
#define SOC_SSP_SPIRIS_rorros_END      (0)
#define SOC_SSP_SPIRIS_rttis_START     (1)
#define SOC_SSP_SPIRIS_rttis_END       (1)
#define SOC_SSP_SPIRIS_rxris_START     (2)
#define SOC_SSP_SPIRIS_rxris_END       (2)
#define SOC_SSP_SPIRIS_txris_START     (3)
#define SOC_SSP_SPIRIS_txris_END       (3)


/*****************************************************************************
 结构名    : SOC_SSP_SPIICR_UNION
 结构说明  : SPIICR 寄存器结构定义。地址偏移量:0x020，初值:0x0000，宽度:16
 寄存器说明: 中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  roric    : 1;  /* bit[0]   : 清除接收溢出中断。
                                                     0：无影响；
                                                     1：清除中断。 */
        unsigned short  rtic     : 1;  /* bit[1]   : 清除接收超时中断。
                                                     0：无影响；
                                                     1：清除中断。 */
        unsigned short  reserved : 14; /* bit[2-15]: 保留。 */
    } reg;
} SOC_SSP_SPIICR_UNION;
#endif
#define SOC_SSP_SPIICR_roric_START     (0)
#define SOC_SSP_SPIICR_roric_END       (0)
#define SOC_SSP_SPIICR_rtic_START      (1)
#define SOC_SSP_SPIICR_rtic_END        (1)


/*****************************************************************************
 结构名    : SOC_SSP_SPIDMACR_UNION
 结构说明  : SPIDMACR 寄存器结构定义。地址偏移量:0x024，初值:0x0000，宽度:16
 寄存器说明: DMA控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  rxdmae   : 1;  /* bit[0]   : DMA接收FIFO使能位。
                                                     0：禁止；
                                                     1：使能DMA接收FIFO。 */
        unsigned short  txdmae   : 1;  /* bit[1]   : DMA发送FIFO使能位。
                                                     0：禁止；
                                                     1：使能DMA发送FIFO。 */
        unsigned short  reserved : 14; /* bit[2-15]:  */
    } reg;
} SOC_SSP_SPIDMACR_UNION;
#endif
#define SOC_SSP_SPIDMACR_rxdmae_START    (0)
#define SOC_SSP_SPIDMACR_rxdmae_END      (0)
#define SOC_SSP_SPIDMACR_txdmae_START    (1)
#define SOC_SSP_SPIDMACR_txdmae_END      (1)


/*****************************************************************************
 结构名    : SOC_SSP_SPITXFIFOCR_UNION
 结构说明  : SPITXFIFOCR 寄存器结构定义。地址偏移量:0x028，初值:0x0009，宽度:16
 寄存器说明: 发送FIFO控制寄存器。该寄存器只使用于SPI2。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  dmatxbrsize : 3;  /* bit[0-2] : 配置发送FIFO请求DMA进行burst传输的水线。即，发送FIFO中数据数目小于或等于（256-DMATXBRSize）所配置的字数时，DMATXBREQ有效，此处字长是16位。
                                                        000：1；
                                                        001：4；
                                                        010：8；
                                                        011：16；
                                                        100：32；
                                                        101：64；
                                                        110：64；
                                                        111：64。 */
        unsigned short  txintsize   : 3;  /* bit[3-5] : 配置发送FIFO请求中断的水线。即，发送FIFO中数据数目小于或等于TXINTSize所配置的字数时，TXRIS有效。
                                                        000：1；
                                                        001：4；
                                                        010：8；
                                                        011：16；
                                                        100：32；
                                                        101：64；
                                                        110：128；
                                                        111：128。 */
        unsigned short  reserved    : 10; /* bit[6-15]: 保留。 */
    } reg;
} SOC_SSP_SPITXFIFOCR_UNION;
#endif
#define SOC_SSP_SPITXFIFOCR_dmatxbrsize_START  (0)
#define SOC_SSP_SPITXFIFOCR_dmatxbrsize_END    (2)
#define SOC_SSP_SPITXFIFOCR_txintsize_START    (3)
#define SOC_SSP_SPITXFIFOCR_txintsize_END      (5)


/*****************************************************************************
 结构名    : SOC_SSP_SPIRXFIFOCR_UNION
 结构说明  : SPIRXFIFOCR 寄存器结构定义。地址偏移量:0x02C，初值:0x0000，宽度:16
 寄存器说明: 接收FIFO控制寄存器。该寄存器只使用于SPI2。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  dmarxbrsize : 3;  /* bit[0-2] : 配置接收FIFO请求DMA进行burst传输的水线。即，接收FIFO中数据数目大于或等于DMARXBRSize所配置的字数时，DMARXBREQ有效。
                                                        000：1；
                                                        001：4；
                                                        010：8；
                                                        011：16；
                                                        100：32；
                                                        101：64；
                                                        110：64；
                                                        111：64。 */
        unsigned short  rxintsize   : 3;  /* bit[3-5] : 配置接收FIFO请求中断的水线。即，接收FIFO中数据数目大于或等于（256-RXINTSize）所配置的字数时，RXRIS有效，此处字长是16位。
                                                        000：1；
                                                        001：4；
                                                        010：8；
                                                        011：16；
                                                        100：32；
                                                        101：64；
                                                        110：128；
                                                        111：224。 */
        unsigned short  reserved    : 10; /* bit[6-15]: 保留。 */
    } reg;
} SOC_SSP_SPIRXFIFOCR_UNION;
#endif
#define SOC_SSP_SPIRXFIFOCR_dmarxbrsize_START  (0)
#define SOC_SSP_SPIRXFIFOCR_dmarxbrsize_END    (2)
#define SOC_SSP_SPIRXFIFOCR_rxintsize_START    (3)
#define SOC_SSP_SPIRXFIFOCR_rxintsize_END      (5)






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

#endif /* end of soc_ssp_interface.h */
