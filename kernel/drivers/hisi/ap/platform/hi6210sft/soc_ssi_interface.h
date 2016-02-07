/******************************************************************************

                 版权所有 (C), 2001-2012, 华为技术有限公司

 ******************************************************************************
  文 件 名   : soc_ssi_interface.h
  版 本 号   : 初稿
  作    者   : Excel2Code
  生成日期   : 2012-06-05 19:58:59
  最近修改   :
  功能描述   : 接口头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月5日
    作    者   : m53479
    修改内容   : 从《Hi6620V100SOC寄存器手册_SSI.xml》自动生成

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_SSI_INTERFACE_H__
#define __SOC_SSI_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) SSI
 ****************************************************************************/
/* 寄存器说明：控制寄存器，用于配置地址位宽与数据位宽。
   位域定义UNION结构:  SOC_SSI_SSICR0_UNION */
#define SOC_SSI_SSICR0_ADDR(base)                     ((base) + (0x0000))

/* 寄存器说明：控制寄存器，主要控制SSI中的使能信号。该信号是用来对SSI接收发送逻辑进行软复位，使其不能够发送或接收数据。
   位域定义UNION结构:  SOC_SSI_SSICR1_UNION */
#define SOC_SSI_SSICR1_ADDR(base)                     ((base) + (0x0004))

/* 寄存器说明：控制寄存器，主要控制SSI中的读写判断信号。该信号是用来对SSI进行读写判断的操作。
   位域定义UNION结构:  SOC_SSI_SSICR2_UNION */
#define SOC_SSI_SSICR2_ADDR(base)                     ((base) + (0x0008))

/* 寄存器说明：控制寄存器，用于配置SSI中的接收FIFO以及发送FIFO的水线。
   位域定义UNION结构:  SOC_SSI_SSICR3_UNION */
#define SOC_SSI_SSICR3_ADDR(base)                     ((base) + (0x000C))

/* 寄存器说明：控制寄存器，用于配置SSI中超时等待时间WAIT_TIME以及master check读同步位的时间。
   位域定义UNION结构:  SOC_SSI_SSICR4_UNION */
#define SOC_SSI_SSICR4_ADDR(base)                     ((base) + (0x0010))

/* 寄存器说明：32位宽的数据寄存器。
            当SSIDR被读时，接收FIFO被访问（被当前FIFO读指针所指）。当数据值被SSI接收逻辑从输入的数据帧移出，它们被放到接收FIFO中（被当前FIFO写指针所指）。
            当SSIDR被写时，发送FIFO被写入（被当前FIFO的写指针所指）。数据值从发送FIFO中被发送逻辑读出。它们被装载到发送串行的移位器，然后在单线串行移出。
            其中，地址信息在低位，数据信息在高位。
   详      述：对发送/接收FIFO读写数据。
               读操作时：先向发送FIFO中写地址信息，等待一段时间，然后从接收FIFO中读数据。
               写操作时：向发送FIFO中写地址与数据信息。
               其中低位存放的是地址信息，高位存放的是数据信息。
   UNION 结构：无 */
#define SOC_SSI_SSIDR_ADDR(base)                      ((base) + (0x0014))

/* 寄存器说明：SSI状态寄存器，只读。该寄存器中反映了当前FIFO的空满状态，以及SSI是否处于忙状态。
   位域定义UNION结构:  SOC_SSI_SSISR_UNION */
#define SOC_SSI_SSISR_ADDR(base)                      ((base) + (0x0018))

/* 寄存器说明：SSI中断屏蔽置位清零寄存器。
            当读寄存器给出相关中断屏蔽的当前值。
   位域定义UNION结构:  SOC_SSI_SSIIMSC_UNION */
#define SOC_SSI_SSIIMSC_ADDR(base)                    ((base) + (0x001C))

/* 寄存器说明：SSI原始中断状态寄存器，只读。存储屏蔽之前的初始中断的中断状态。
   位域定义UNION结构:  SOC_SSI_SSIRIS_UNION */
#define SOC_SSI_SSIRIS_ADDR(base)                     ((base) + (0x0020))

/* 寄存器说明：SSI屏蔽中断状态寄存器，只读。存储经过屏蔽后的中断状态。
   位域定义UNION结构:  SOC_SSI_SSIMIS_UNION */
#define SOC_SSI_SSIMIS_ADDR(base)                     ((base) + (0x0024))

/* 寄存器说明：SSI中断清除寄存器，只写。
   位域定义UNION结构:  SOC_SSI_SSIICR_UNION */
#define SOC_SSI_SSIICR_ADDR(base)                     ((base) + (0x0028))





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
                     (1/1) SSI
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_SSI_SSICR0_UNION
 结构说明  : SSICR0 寄存器结构定义。地址偏移量:0x0000，初值:0x00000000，宽度:32
 寄存器说明: 控制寄存器，用于配置地址位宽与数据位宽。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ass      : 3;  /* bit[0-2] : 设置地址位宽。
                                                   支持1bit～8bit
                                                   000：1bit； 001：2bit；
                                                   010：3bit； 011：4bit；
                                                   100：5bit； 101：6bit；
                                                   110：7bit； 111：8bit。 */
        unsigned int  dss      : 5;  /* bit[3-7] : 设置数据位宽。
                                                   支持2bit～24bit。
                                                   00001：2bit； 00010：3bit；
                                                   00011：4bit； 00100：5bit；
                                                   00101：6bit； 00110：7bit；
                                                   00111：8bit； 01000：9bit；
                                                   01001：10bit；01010：11bit；
                                                   01011：12bit；01100：13bit；
                                                   01101：14bit；01110：15bit；
                                                   01111：16bit；10000：17bit；
                                                   10001：18bit；10010：19bit；
                                                   10011：20bit；10100：21bit；
                                                   10101：22bit；10110：23bit；
                                                   10111：24bit；
                                                   其他：保留。 */
        unsigned int  reserved : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_SSI_SSICR0_UNION;
#define SOC_SSI_SSICR0_ass_START       (0)
#define SOC_SSI_SSICR0_ass_END         (2)
#define SOC_SSI_SSICR0_dss_START       (3)
#define SOC_SSI_SSICR0_dss_END         (7)


/*****************************************************************************
 结构名    : SOC_SSI_SSICR1_UNION
 结构说明  : SSICR1 寄存器结构定义。地址偏移量:0x0004，初值:0x00000000，宽度:32
 寄存器说明: 控制寄存器，主要控制SSI中的使能信号。该信号是用来对SSI接收发送逻辑进行软复位，使其不能够发送或接收数据。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ssi_en   : 1;  /* bit[0]   : 设置SSI使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_SSI_SSICR1_UNION;
#define SOC_SSI_SSICR1_ssi_en_START    (0)
#define SOC_SSI_SSICR1_ssi_en_END      (0)


/*****************************************************************************
 结构名    : SOC_SSI_SSICR2_UNION
 结构说明  : SSICR2 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: 控制寄存器，主要控制SSI中的读写判断信号。该信号是用来对SSI进行读写判断的操作。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ssi_rw   : 1;  /* bit[0]   : 读写操作判断位。
                                                   0：写操作；
                                                   1：读操作。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_SSI_SSICR2_UNION;
#define SOC_SSI_SSICR2_ssi_rw_START    (0)
#define SOC_SSI_SSICR2_ssi_rw_END      (0)


/*****************************************************************************
 结构名    : SOC_SSI_SSICR3_UNION
 结构说明  : SSICR3 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: 控制寄存器，用于配置SSI中的接收FIFO以及发送FIFO的水线。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  txfifo_wl : 3;  /* bit[0-2] : 设置发送FIFO水线。
                                                    000：1； 001：2；
                                                    010：3； 011：4；
                                                    100：5； 101：6；
                                                    110：7； 111：8。 */
        unsigned int  rxfifo_wl : 3;  /* bit[3-5] : 设置接收FIFO水线。
                                                    000：1； 001：2；
                                                    010：3； 011：4；
                                                    100：5； 101：6；
                                                    110：7； 111：8。 */
        unsigned int  reserved  : 26; /* bit[6-31]: 保留。 */
    } reg;
} SOC_SSI_SSICR3_UNION;
#define SOC_SSI_SSICR3_txfifo_wl_START  (0)
#define SOC_SSI_SSICR3_txfifo_wl_END    (2)
#define SOC_SSI_SSICR3_rxfifo_wl_START  (3)
#define SOC_SSI_SSICR3_rxfifo_wl_END    (5)


/*****************************************************************************
 结构名    : SOC_SSI_SSICR4_UNION
 结构说明  : SSICR4 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: 控制寄存器，用于配置SSI中超时等待时间WAIT_TIME以及master check读同步位的时间。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wait_time  : 4;  /* bit[0-3] : 设置超时等待时间，单位为cycle。
                                                     0000：1； 0001：2；
                                                     0010：3； 0011：4；
                                                     0100：5； 0101：6；
                                                     0110：7； 0111：8；
                                                     1000：9； 1001：10；
                                                     1010：11；1011：12；
                                                     1100：13；1101：14；
                                                     1110：15；1111：16。 */
        unsigned int  mst_ck_cfg : 3;  /* bit[4-6] : 该值用来配置读操作时,master check读同步位的时间。
                                                     000:1.5 clk 001:2.0 clk
                                                     010:2.5 clk 011:3.0 clk
                                                     100:3.5 clk 101:4.0 clk
                                                     110:4.5 clk 111:5.0 clk */
        unsigned int  reserved   : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_SSI_SSICR4_UNION;
#define SOC_SSI_SSICR4_wait_time_START   (0)
#define SOC_SSI_SSICR4_wait_time_END     (3)
#define SOC_SSI_SSICR4_mst_ck_cfg_START  (4)
#define SOC_SSI_SSICR4_mst_ck_cfg_END    (6)


/*****************************************************************************
 结构名    : SOC_SSI_SSISR_UNION
 结构说明  : SSISR 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: SSI状态寄存器，只读。该寄存器中反映了当前FIFO的空满状态，以及SSI是否处于忙状态。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tfe       : 1;  /* bit[0]    : 发送FIFO 是否已空。
                                                     0：未空；
                                                     1：已空。 */
        unsigned int  tnf       : 1;  /* bit[1]    : 发送FIFO 是否未满。
                                                     0：已满；
                                                     1：未满。 */
        unsigned int  rne       : 1;  /* bit[2]    : 接收FIFO 是否未空。
                                                     0：已空；
                                                     1：未空。 */
        unsigned int  rff       : 1;  /* bit[3]    : 接收FIFO是否已满。
                                                     0：未满；
                                                     1：已满。 */
        unsigned int  bsy       : 1;  /* bit[4]    : SSI 忙标记。
                                                     0：空闲；
                                                     1：忙碌。 */
        unsigned int  rxfifl_lv : 3;  /* bit[5-7]  : 接收FIFO深度（深度最大为8）。
                                                     000：0； 001：1；
                                                     010：2； 011：3；
                                                     100：4； 101：5；
                                                     110：6； 111：7。 */
        unsigned int  txfifo_lv : 3;  /* bit[8-10] : 发送FIFO深度（深度最大为8）。
                                                     000：0； 001：1；
                                                     010：2； 011：3；
                                                     100：4； 101：5；
                                                     110：6； 111：7。 */
        unsigned int  reserved  : 21; /* bit[11-31]: 保留。 */
    } reg;
} SOC_SSI_SSISR_UNION;
#define SOC_SSI_SSISR_tfe_START        (0)
#define SOC_SSI_SSISR_tfe_END          (0)
#define SOC_SSI_SSISR_tnf_START        (1)
#define SOC_SSI_SSISR_tnf_END          (1)
#define SOC_SSI_SSISR_rne_START        (2)
#define SOC_SSI_SSISR_rne_END          (2)
#define SOC_SSI_SSISR_rff_START        (3)
#define SOC_SSI_SSISR_rff_END          (3)
#define SOC_SSI_SSISR_bsy_START        (4)
#define SOC_SSI_SSISR_bsy_END          (4)
#define SOC_SSI_SSISR_rxfifl_lv_START  (5)
#define SOC_SSI_SSISR_rxfifl_lv_END    (7)
#define SOC_SSI_SSISR_txfifo_lv_START  (8)
#define SOC_SSI_SSISR_txfifo_lv_END    (10)


/*****************************************************************************
 结构名    : SOC_SSI_SSIIMSC_UNION
 结构说明  : SSIIMSC 寄存器结构定义。地址偏移量:0x001C，初值:0x00000000，宽度:32
 寄存器说明: SSI中断屏蔽置位清零寄存器。
            当读寄存器给出相关中断屏蔽的当前值。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rorim    : 1;  /* bit[0]   : 接收FIFO溢出中断屏蔽。
                                                   0:屏蔽。
                                                   1:不屏蔽。 */
        unsigned int  rtim     : 1;  /* bit[1]   : 接收超时中断屏蔽。
                                                   0:屏蔽。
                                                   1:不屏蔽。 */
        unsigned int  rxim     : 1;  /* bit[2]   : 接收FIFO中断屏蔽。
                                                   0:屏蔽。
                                                   1:不屏蔽。 */
        unsigned int  txim     : 1;  /* bit[3]   : 发送FIFO中断屏蔽。
                                                   0:屏蔽。
                                                   1:不屏蔽。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_SSI_SSIIMSC_UNION;
#define SOC_SSI_SSIIMSC_rorim_START     (0)
#define SOC_SSI_SSIIMSC_rorim_END       (0)
#define SOC_SSI_SSIIMSC_rtim_START      (1)
#define SOC_SSI_SSIIMSC_rtim_END        (1)
#define SOC_SSI_SSIIMSC_rxim_START      (2)
#define SOC_SSI_SSIIMSC_rxim_END        (2)
#define SOC_SSI_SSIIMSC_txim_START      (3)
#define SOC_SSI_SSIIMSC_txim_END        (3)


/*****************************************************************************
 结构名    : SOC_SSI_SSIRIS_UNION
 结构说明  : SSIRIS 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: SSI原始中断状态寄存器，只读。存储屏蔽之前的初始中断的中断状态。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rorris   : 1;  /* bit[0]   : 接收溢出中断的原始中断状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  rtris    : 1;  /* bit[1]   : 接收超时中断的原始中断状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  rxris    : 1;  /* bit[2]   : 接收FIFO中断的原始中断状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  txris    : 1;  /* bit[3]   : 发送FIFO中断的原始中断状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_SSI_SSIRIS_UNION;
#define SOC_SSI_SSIRIS_rorris_START    (0)
#define SOC_SSI_SSIRIS_rorris_END      (0)
#define SOC_SSI_SSIRIS_rtris_START     (1)
#define SOC_SSI_SSIRIS_rtris_END       (1)
#define SOC_SSI_SSIRIS_rxris_START     (2)
#define SOC_SSI_SSIRIS_rxris_END       (2)
#define SOC_SSI_SSIRIS_txris_START     (3)
#define SOC_SSI_SSIRIS_txris_END       (3)


/*****************************************************************************
 结构名    : SOC_SSI_SSIMIS_UNION
 结构说明  : SSIMIS 寄存器结构定义。地址偏移量:0x0024，初值:0x00000000，宽度:32
 寄存器说明: SSI屏蔽中断状态寄存器，只读。存储经过屏蔽后的中断状态。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rormis   : 1;  /* bit[0]   : 接收溢出中断屏蔽后的状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  rtmis    : 1;  /* bit[1]   : 接收超时中断屏蔽后的状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  rxmis    : 1;  /* bit[2]   : 接收FIFO中断屏蔽后的状态。
                                                   0：无中断；
                                                   1：有中断 */
        unsigned int  txmis    : 1;  /* bit[3]   : 发送FIFO中断屏蔽后的状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_SSI_SSIMIS_UNION;
#define SOC_SSI_SSIMIS_rormis_START    (0)
#define SOC_SSI_SSIMIS_rormis_END      (0)
#define SOC_SSI_SSIMIS_rtmis_START     (1)
#define SOC_SSI_SSIMIS_rtmis_END       (1)
#define SOC_SSI_SSIMIS_rxmis_START     (2)
#define SOC_SSI_SSIMIS_rxmis_END       (2)
#define SOC_SSI_SSIMIS_txmis_START     (3)
#define SOC_SSI_SSIMIS_txmis_END       (3)


/*****************************************************************************
 结构名    : SOC_SSI_SSIICR_UNION
 结构说明  : SSIICR 寄存器结构定义。地址偏移量:0x0028，初值:0x00000000，宽度:32
 寄存器说明: SSI中断清除寄存器，只写。
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rorim    : 1;  /* bit[0]   : 清除接收溢出中断。
                                                   0：无影响；
                                                   1：清除中断。 */
        unsigned int  rtim     : 1;  /* bit[1]   : 清除接收超时中断。
                                                   0：无影响；
                                                   1：清除中断。 */
        unsigned int  rxim     : 1;  /* bit[2]   : 清除接收中断。
                                                   0：无影响；
                                                   1：清除中断。 */
        unsigned int  txim     : 1;  /* bit[3]   : 清除发送中断。
                                                   0：无影响；
                                                   1：清除中断。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_SSI_SSIICR_UNION;
#define SOC_SSI_SSIICR_rorim_START     (0)
#define SOC_SSI_SSIICR_rorim_END       (0)
#define SOC_SSI_SSIICR_rtim_START      (1)
#define SOC_SSI_SSIICR_rtim_END        (1)
#define SOC_SSI_SSIICR_rxim_START      (2)
#define SOC_SSI_SSIICR_rxim_END        (2)
#define SOC_SSI_SSIICR_txim_START      (3)
#define SOC_SSI_SSIICR_txim_END        (3)






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

#endif /* end of soc_ssi_interface.h */
