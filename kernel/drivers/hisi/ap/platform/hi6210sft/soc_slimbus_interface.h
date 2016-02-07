/******************************************************************************

                 版权所有 (C), 2001-2012, 华为技术有限公司

 ******************************************************************************
  文 件 名   : soc_slimbus_interface.h
  版 本 号   : 初稿
  作    者   : Excel2Code
  生成日期   : 2012-08-09 20:06:19
  最近修改   :
  功能描述   : 接口头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年8月9日
    作    者   : m53479
    修改内容   : 从《Hi6620V100SOC寄存器手册_SLIMBUS.xml》自动生成

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_SLIMBUS_INTERFACE_H__
#define __SOC_SLIMBUS_INTERFACE_H__

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
/* 寄存器说明：中断使能寄存器
   位域定义UNION结构:  SOC_SLIMBUS_INTEN_UNION */
#define SOC_SLIMBUS_INTEN_ADDR(base)                  ((base) + (0x0000))

/* 寄存器说明：中断状态寄存器
   位域定义UNION结构:  SOC_SLIMBUS_INTSTS_UNION */
#define SOC_SLIMBUS_INTSTS_ADDR(base)                 ((base) + (0x0004))

/* 寄存器说明：传输仲裁寄存器1
   位域定义UNION结构:  SOC_SLIMBUS_TXARBI1_UNION */
#define SOC_SLIMBUS_TXARBI1_ADDR(base)                ((base) + (0x0008))

/* 寄存器说明：传输仲裁寄存器2
   位域定义UNION结构:  SOC_SLIMBUS_TXARBI2_UNION */
#define SOC_SLIMBUS_TXARBI2_ADDR(base)                ((base) + (0x000C))

/* 寄存器说明：传输消息头1
   位域定义UNION结构:  SOC_SLIMBUS_TXMSGHDR1_UNION */
#define SOC_SLIMBUS_TXMSGHDR1_ADDR(base)              ((base) + (0x0010))

/* 寄存器说明：传输消息头2
   位域定义UNION结构:  SOC_SLIMBUS_TXMSGHDR2_UNION */
#define SOC_SLIMBUS_TXMSGHDR2_ADDR(base)              ((base) + (0x0014))

/* 寄存器说明：传输消息头3
   位域定义UNION结构:  SOC_SLIMBUS_TXMSGHDR3_UNION */
#define SOC_SLIMBUS_TXMSGHDR3_ADDR(base)              ((base) + (0x0018))

/* 寄存器说明：传输消息负载1
   详      述：包含Message Payload data(31:0)，数据源为SLIMbus Active Manager
   UNION 结构：无 */
#define SOC_SLIMBUS_TXMSGPLD1_ADDR(base)              ((base) + (0x001C))

/* 寄存器说明：传输消息负载2
   详      述：包含Message Payload data(63:32)，数据源为SLIMbus Active Manager
   UNION 结构：无 */
#define SOC_SLIMBUS_TXMSGPLD2_ADDR(base)              ((base) + (0x0020))

/* 寄存器说明：传输消息负载3
   位域定义UNION结构:  SOC_SLIMBUS_TXMSGPLD3_UNION */
#define SOC_SLIMBUS_TXMSGPLD3_ADDR(base)              ((base) + (0x0024))

/* 寄存器说明：接收仲裁寄存器1
   位域定义UNION结构:  SOC_SLIMBUS_RXARBI1_UNION */
#define SOC_SLIMBUS_RXARBI1_ADDR(base)                ((base) + (0x0038))

/* 寄存器说明：接收仲裁寄存器2
   位域定义UNION结构:  SOC_SLIMBUS_RXARBI2_UNION */
#define SOC_SLIMBUS_RXARBI2_ADDR(base)                ((base) + (0x003C))

/* 寄存器说明：接收消息头1
   位域定义UNION结构:  SOC_SLIMBUS_RXMSGHDR1_UNION */
#define SOC_SLIMBUS_RXMSGHDR1_ADDR(base)              ((base) + (0x0040))

/* 寄存器说明：接收消息头2
   位域定义UNION结构:  SOC_SLIMBUS_RXMSGHDR2_UNION */
#define SOC_SLIMBUS_RXMSGHDR2_ADDR(base)              ((base) + (0x0044))

/* 寄存器说明：接收消息头3
   位域定义UNION结构:  SOC_SLIMBUS_RXMSGHDR3_UNION */
#define SOC_SLIMBUS_RXMSGHDR3_ADDR(base)              ((base) + (0x0048))

/* 寄存器说明：接收消息负载1
   详      述：包含Message Payload data(31:0)，数据源为SLIMbus Active Manager
   UNION 结构：无 */
#define SOC_SLIMBUS_RXMSGPLD1_ADDR(base)              ((base) + (0x004C))

/* 寄存器说明：接收消息负载2
   详      述：包含Message Payload data(63:32)，数据源为SLIMbus Active Manager
   UNION 结构：无 */
#define SOC_SLIMBUS_RXMSGPLD2_ADDR(base)              ((base) + (0x0050))

/* 寄存器说明：接口设备枚举地址
   详      述：interface device的Enumeration Address [31:0]
   UNION 结构：无 */
#define SOC_SLIMBUS_IFEA310_ADDR(base)                ((base) + (0x0068))

/* 寄存器说明：接口设备枚举地址
   位域定义UNION结构:  SOC_SLIMBUS_IFEA4732_UNION */
#define SOC_SLIMBUS_IFEA4732_ADDR(base)               ((base) + (0x006C))

/* 寄存器说明：Framer设备枚举地址
   详      述：Framer的Enumeration Address [31:0]
   UNION 结构：无 */
#define SOC_SLIMBUS_FEA310_ADDR(base)                 ((base) + (0x0074))

/* 寄存器说明：Framer设备枚举地址
   位域定义UNION结构:  SOC_SLIMBUS_FEA4732_UNION */
#define SOC_SLIMBUS_FEA4732_ADDR(base)                ((base) + (0x0078))

/* 寄存器说明：配置寄存器
   位域定义UNION结构:  SOC_SLIMBUS_CNFG_UNION */
#define SOC_SLIMBUS_CNFG_ADDR(base)                   ((base) + (0x0080))

/* 寄存器说明：中断寄存器
   位域定义UNION结构:  SOC_SLIMBUS_INTR_UNION */
#define SOC_SLIMBUS_INTR_ADDR(base)                   ((base) + (0x0084))

/* 寄存器说明：Report Present Enable
   位域定义UNION结构:  SOC_SLIMBUS_RPPREEN_UNION */
#define SOC_SLIMBUS_RPPREEN_ADDR(base)                ((base) + (0x0088))

/* 寄存器说明：通用设备0枚举地址
   详      述：GD0的Enumeration Address [31:0]
   UNION 结构：无 */
#define SOC_SLIMBUS_GD0EA310_ADDR(base)               ((base) + (0x0150))

/* 寄存器说明：通用设备0枚举地址
   位域定义UNION结构:  SOC_SLIMBUS_GD0EA4732_UNION */
#define SOC_SLIMBUS_GD0EA4732_ADDR(base)              ((base) + (0x0154))

/* 寄存器说明：通用设备0逻辑地址
   位域定义UNION结构:  SOC_SLIMBUS_GD0LA_UNION */
#define SOC_SLIMBUS_GD0LA_ADDR(base)                  ((base) + (0x0158))

/* 寄存器说明：通用设备0中断使能寄存器
   位域定义UNION结构:  SOC_SLIMBUS_GD0INTEN_UNION */
#define SOC_SLIMBUS_GD0INTEN_ADDR(base)               ((base) + (0x015C))

/* 寄存器说明：通用设备0中断状态寄存器
   位域定义UNION结构:  SOC_SLIMBUS_GD0INTSTS_UNION */
#define SOC_SLIMBUS_GD0INTSTS_ADDR(base)              ((base) + (0x0160))

/* 寄存器说明：通用设备0端口i(0-15)中断使能寄存器
   位域定义UNION结构:  SOC_SLIMBUS_D0PiINTEN_UNION */
#define SOC_SLIMBUS_D0PiINTEN_ADDR(base)              ((base) + (0x0164+0x10*i))

/* 寄存器说明：通用设备0端口i(0-15)中断状态寄存器
   位域定义UNION结构:  SOC_SLIMBUS_D0PiINTSTS_UNION */
#define SOC_SLIMBUS_D0PiINTSTS_ADDR(base)             ((base) + (0x0168+0x10*i))

/* 寄存器说明：通用设备0端口i(0-15)配置寄存器
   位域定义UNION结构:  SOC_SLIMBUS_D0PiCNFG_UNION */
#define SOC_SLIMBUS_D0PiCNFG_ADDR(base)               ((base) + (0x016C+0x10*i))

/* 寄存器说明：通用设备0端口i(0-15)数据端口寄存器
   详      述：由于访问端口FIFO区的寄存器。在packed mode下，alignment不重要。unpacked mode下，数据在FIFO中是left aligned。
   UNION 结构：无 */
#define SOC_SLIMBUS_D0PiDP_ADDR(base)                 ((base) + (0x0170+0x10*i))

/* 寄存器说明：SDMAi(0-15)配置寄存器
   位域定义UNION结构:  SOC_SLIMBUS_SDMAiCNFG_UNION */
#define SOC_SLIMBUS_SDMAiCNFG_ADDR(base)              ((base) + (0x0400+0x4*i))

/* 寄存器说明：音频端口i(0-15)寄存器
   位域定义UNION结构:  SOC_SLIMBUS_ADPRTi_UNION */
#define SOC_SLIMBUS_ADPRTi_ADDR(base)                 ((base) + (0x0500+0x4*i))

/* 寄存器说明：FL上下文保存寄存器
   位域定义UNION结构:  SOC_SLIMBUS_FLCONSAV1_UNION */
#define SOC_SLIMBUS_FLCONSAV1_ADDR(base)              ((base) + (0x0900))

/* 寄存器说明：MH上下文保存寄存器
   位域定义UNION结构:  SOC_SLIMBUS_MHCONSAV1_UNION */
#define SOC_SLIMBUS_MHCONSAV1_ADDR(base)              ((base) + (0x0934))

/* 寄存器说明：AF上下文保存寄存器1
   位域定义UNION结构:  SOC_SLIMBUS_AFCONSAV1_UNION */
#define SOC_SLIMBUS_AFCONSAV1_ADDR(base)              ((base) + (0x0948))

/* 寄存器说明：AF上下文保存寄存器2
   位域定义UNION结构:  SOC_SLIMBUS_AFCONSAV2_UNION */
#define SOC_SLIMBUS_AFCONSAV2_ADDR(base)              ((base) + (0x094C))

/* 寄存器说明：IF上下文保存寄存器
   位域定义UNION结构:  SOC_SLIMBUS_IFCONSAV1_UNION */
#define SOC_SLIMBUS_IFCONSAV1_ADDR(base)              ((base) + (0x0968))

/* 寄存器说明：GD0消息上下文保存寄存器1
   位域定义UNION结构:  SOC_SLIMBUS_GD0MSGCS1_UNION */
#define SOC_SLIMBUS_GD0MSGCS1_ADDR(base)              ((base) + (0x0978))

/* 寄存器说明：GD0消息上下文保存寄存器2
   位域定义UNION结构:  SOC_SLIMBUS_GD0MSGCS2_UNION */
#define SOC_SLIMBUS_GD0MSGCS2_ADDR(base)              ((base) + (0x097C))

/* 寄存器说明：GD0端口i(0-15)上下文保存寄存器1
   位域定义UNION结构:  SOC_SLIMBUS_GD0PRTiCS1_UNION */
#define SOC_SLIMBUS_GD0PRTiCS1_ADDR(base)             ((base) + (0x0980+0x4*i))

/* 寄存器说明：Channel i(1-16)上下文保存寄存器2
   位域定义UNION结构:  SOC_SLIMBUS_CHCONSAVi_UNION */
#define SOC_SLIMBUS_CHCONSAVi_ADDR(base)              ((base) + (0x0A00+0x8*(i-1)))

/* 寄存器说明：GD0端口i(0-15)上下文保存寄存器2
   位域定义UNION结构:  SOC_SLIMBUS_GD0PRTiCS2_UNION */
#define SOC_SLIMBUS_GD0PRTiCS2_ADDR(base)             ((base) + (0x0A04+0x8*i))

/* 寄存器说明：Channel映射上下文保存寄存器i(1-16)
   位域定义UNION结构:  SOC_SLIMBUS_CHMAPCSi_UNION */
#define SOC_SLIMBUS_CHMAPCSi_ADDR(base)               ((base) + (0x0A80+0x4*(i-1)))





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
 结构名    : SOC_SLIMBUS_INTEN_UNION
 结构说明  : INTEN 寄存器结构定义。地址偏移量:0x0000，初值:0x0000，宽度:32
 寄存器说明: 中断使能寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_msg_pack_en    : 1;  /* bit[0]    : 1'b0:禁止Transmit Message PACK中断
                                                             1'b1:使能Transmit Message PACK中断 */
        unsigned int  tx_msg_nack_en    : 1;  /* bit[1]    : 1'b0:禁止Transmit Message NACK中断
                                                             1'b1:使能Transmit Message NACK中断 */
        unsigned int  tx_msg_nore_en    : 1;  /* bit[2]    : 1'b0:禁止Transmit Message No Response中断
                                                             1'b1:使能Transmit Message No Response中断 */
        unsigned int  tx_msg_udef_en    : 1;  /* bit[3]    : 1'b0:禁止Transmit Message Undefined中断
                                                             1'b1:使能Transmit Message Undefined中断 */
        unsigned int  rx_msg_pack_en    : 1;  /* bit[4]    : 1'b0:禁止Receive Message PACK中断
                                                             1'b1:使能Receive Message PACK中断 */
        unsigned int  rx_msg_nack_en    : 1;  /* bit[5]    : 1'b0:禁止Receive Message NACK中断
                                                             1'b1:使能Receive Message NACK中断 */
        unsigned int  rx_msg_nore_en    : 1;  /* bit[6]    : 1'b0:禁止Receive Message No Response中断
                                                             1'b1:使能Receive Message No Response中断 */
        unsigned int  rx_msg_udef_en    : 1;  /* bit[7]    : 1'b0:禁止Transmit Message Undefined中断
                                                             1'b1:使能Transmit Message Undefined中断 */
        unsigned int  reconfig_set_en   : 1;  /* bit[8]    : 1'b0:禁止Reconfiguration set中断
                                                             1'b1:使能Reconfiguration set中断 */
        unsigned int  reconfig_clear_en : 1;  /* bit[9]    : 1'b0:禁止Reconfiguration clear中断
                                                             1'b1:使能Reconfiguration clear中断 */
        unsigned int  ex_error_en       : 1;  /* bit[10]   : 1'b0:禁止ex error中断
                                                             1'b1:使能ex error中断 */
        unsigned int  data_tx_col_en    : 1;  /* bit[11]   : 1'b0:禁止data tx collision中断
                                                             1'b1:使能data tx collision中断 */
        unsigned int  unsprtd_msg_en    : 1;  /* bit[12]   : 1'b0:禁止unsupported message中断
                                                             1'b1:使能unsupported message中断 */
        unsigned int  am_h_inta_n       : 1;  /* bit[13]   : 1'b0:禁止Active manager device中断
                                                             1'b1:使能Active manager device中断 */
        unsigned int  gd0_h_inta_n      : 1;  /* bit[14]   : 1'b0:禁止Generic device0中断
                                                             1'b1:使能Generic device0中断 */
        unsigned int  reserved_0        : 4;  /* bit[15-18]: Reserved for Future Use */
        unsigned int  prep_cont_sts_en  : 1;  /* bit[19]   : 1'b0:禁止NEXT_PREPARE_CONTEXT消息接收中断
                                                             1'b1:使能NEXT_PREPARE_CONTEXT消息接收中断 */
        unsigned int  Rx_Msg_OFL_en     : 1;  /* bit[20]   : 1'b0:禁止Rx_Msg_ofl中断
                                                             1'b1:使能Rx_Msg_ofl中断 */
        unsigned int  clk_pause_sts_en  : 1;  /* bit[21]   : 1'b0:禁止clk_pause_sts中断
                                                             1'b1:使能clk_pause_sts中断 */
        unsigned int  reserved_1        : 10; /* bit[22-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_INTEN_UNION;
#define SOC_SLIMBUS_INTEN_tx_msg_pack_en_START     (0)
#define SOC_SLIMBUS_INTEN_tx_msg_pack_en_END       (0)
#define SOC_SLIMBUS_INTEN_tx_msg_nack_en_START     (1)
#define SOC_SLIMBUS_INTEN_tx_msg_nack_en_END       (1)
#define SOC_SLIMBUS_INTEN_tx_msg_nore_en_START     (2)
#define SOC_SLIMBUS_INTEN_tx_msg_nore_en_END       (2)
#define SOC_SLIMBUS_INTEN_tx_msg_udef_en_START     (3)
#define SOC_SLIMBUS_INTEN_tx_msg_udef_en_END       (3)
#define SOC_SLIMBUS_INTEN_rx_msg_pack_en_START     (4)
#define SOC_SLIMBUS_INTEN_rx_msg_pack_en_END       (4)
#define SOC_SLIMBUS_INTEN_rx_msg_nack_en_START     (5)
#define SOC_SLIMBUS_INTEN_rx_msg_nack_en_END       (5)
#define SOC_SLIMBUS_INTEN_rx_msg_nore_en_START     (6)
#define SOC_SLIMBUS_INTEN_rx_msg_nore_en_END       (6)
#define SOC_SLIMBUS_INTEN_rx_msg_udef_en_START     (7)
#define SOC_SLIMBUS_INTEN_rx_msg_udef_en_END       (7)
#define SOC_SLIMBUS_INTEN_reconfig_set_en_START    (8)
#define SOC_SLIMBUS_INTEN_reconfig_set_en_END      (8)
#define SOC_SLIMBUS_INTEN_reconfig_clear_en_START  (9)
#define SOC_SLIMBUS_INTEN_reconfig_clear_en_END    (9)
#define SOC_SLIMBUS_INTEN_ex_error_en_START        (10)
#define SOC_SLIMBUS_INTEN_ex_error_en_END          (10)
#define SOC_SLIMBUS_INTEN_data_tx_col_en_START     (11)
#define SOC_SLIMBUS_INTEN_data_tx_col_en_END       (11)
#define SOC_SLIMBUS_INTEN_unsprtd_msg_en_START     (12)
#define SOC_SLIMBUS_INTEN_unsprtd_msg_en_END       (12)
#define SOC_SLIMBUS_INTEN_am_h_inta_n_START        (13)
#define SOC_SLIMBUS_INTEN_am_h_inta_n_END          (13)
#define SOC_SLIMBUS_INTEN_gd0_h_inta_n_START       (14)
#define SOC_SLIMBUS_INTEN_gd0_h_inta_n_END         (14)
#define SOC_SLIMBUS_INTEN_prep_cont_sts_en_START   (19)
#define SOC_SLIMBUS_INTEN_prep_cont_sts_en_END     (19)
#define SOC_SLIMBUS_INTEN_Rx_Msg_OFL_en_START      (20)
#define SOC_SLIMBUS_INTEN_Rx_Msg_OFL_en_END        (20)
#define SOC_SLIMBUS_INTEN_clk_pause_sts_en_START   (21)
#define SOC_SLIMBUS_INTEN_clk_pause_sts_en_END     (21)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_INTSTS_UNION
 结构说明  : INTSTS 寄存器结构定义。地址偏移量:0x0004，初值:0x0000，宽度:32
 寄存器说明: 中断状态寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_msg_pack    : 1;  /* bit[0]    : 1'b0:无中断
                                                          1'b1:有中断，当Transmit message收到positive ack */
        unsigned int  tx_msg_nack    : 1;  /* bit[1]    : 1'b0:无中断
                                                          1'b1:有中断，当Transmit message收到negative ack */
        unsigned int  tx_msg_nore    : 1;  /* bit[2]    : 1'b0:无中断
                                                          1'b1:有中断，当Transmit message收不到response */
        unsigned int  tx_msg_udef    : 1;  /* bit[3]    : 1'b0:无中断
                                                          1'b1:有中断，当Transmit message收到undefined response */
        unsigned int  rx_msg_pack    : 1;  /* bit[4]    : 1'b0:无中断
                                                          1'b1:有中断，当Received message收到positive ack */
        unsigned int  rx_msg_nack    : 1;  /* bit[5]    : 1'b0:无中断
                                                          1'b1:有中断，当Received message收到negative ack */
        unsigned int  rx_msg_nore    : 1;  /* bit[6]    : 1'b0:无中断
                                                          1'b1:有中断，当Received message收不到response */
        unsigned int  rx_msg_udef    : 1;  /* bit[7]    : 1'b0:无中断
                                                          1'b1:有中断，当Received message收到undefined response */
        unsigned int  reconfig_set   : 1;  /* bit[8]    : 1'b0:无中断
                                                          1'b1:有中断，当Bus reconfiguration开始时(&quot;Reconfiguration Pending&quot;位已设置) */
        unsigned int  reconfig_clear : 1;  /* bit[9]    : 1'b0:无中断
                                                          1'b1:有中断，当Bus reconfiguration完成时(&quot;Reconfiguration Pending&quot;位已清除) */
        unsigned int  ex_error       : 1;  /* bit[10]   : 1'b0:无中断
                                                          1'b1:有中断，当发生Message execution error */
        unsigned int  data_tx_col    : 1;  /* bit[11]   : 1'b0:无中断
                                                          1'b1:有中断，当数据通道发生Collision */
        unsigned int  unsprtd_msg    : 1;  /* bit[12]   : 1'b0:无中断
                                                          1'b1:有中断，当收到unsupported message */
        unsigned int  prep_cont_sts  : 1;  /* bit[13]   : 1'b0:无中断
                                                          1'b1:有中断，当Context registers已准备好 */
        unsigned int  Rx_Msg_OFL_sts : 1;  /* bit[14]   : 1'b0:无中断
                                                          1'b1:有中断，当Rx message overflow */
        unsigned int  clk_pause_sts  : 1;  /* bit[15]   : 1'b0:无中断
                                                          1'b1:有中断，当总线从high power state变化到low power state（clock pause or shut down） */
        unsigned int  reserved       : 16; /* bit[16-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_INTSTS_UNION;
#define SOC_SLIMBUS_INTSTS_tx_msg_pack_START     (0)
#define SOC_SLIMBUS_INTSTS_tx_msg_pack_END       (0)
#define SOC_SLIMBUS_INTSTS_tx_msg_nack_START     (1)
#define SOC_SLIMBUS_INTSTS_tx_msg_nack_END       (1)
#define SOC_SLIMBUS_INTSTS_tx_msg_nore_START     (2)
#define SOC_SLIMBUS_INTSTS_tx_msg_nore_END       (2)
#define SOC_SLIMBUS_INTSTS_tx_msg_udef_START     (3)
#define SOC_SLIMBUS_INTSTS_tx_msg_udef_END       (3)
#define SOC_SLIMBUS_INTSTS_rx_msg_pack_START     (4)
#define SOC_SLIMBUS_INTSTS_rx_msg_pack_END       (4)
#define SOC_SLIMBUS_INTSTS_rx_msg_nack_START     (5)
#define SOC_SLIMBUS_INTSTS_rx_msg_nack_END       (5)
#define SOC_SLIMBUS_INTSTS_rx_msg_nore_START     (6)
#define SOC_SLIMBUS_INTSTS_rx_msg_nore_END       (6)
#define SOC_SLIMBUS_INTSTS_rx_msg_udef_START     (7)
#define SOC_SLIMBUS_INTSTS_rx_msg_udef_END       (7)
#define SOC_SLIMBUS_INTSTS_reconfig_set_START    (8)
#define SOC_SLIMBUS_INTSTS_reconfig_set_END      (8)
#define SOC_SLIMBUS_INTSTS_reconfig_clear_START  (9)
#define SOC_SLIMBUS_INTSTS_reconfig_clear_END    (9)
#define SOC_SLIMBUS_INTSTS_ex_error_START        (10)
#define SOC_SLIMBUS_INTSTS_ex_error_END          (10)
#define SOC_SLIMBUS_INTSTS_data_tx_col_START     (11)
#define SOC_SLIMBUS_INTSTS_data_tx_col_END       (11)
#define SOC_SLIMBUS_INTSTS_unsprtd_msg_START     (12)
#define SOC_SLIMBUS_INTSTS_unsprtd_msg_END       (12)
#define SOC_SLIMBUS_INTSTS_prep_cont_sts_START   (13)
#define SOC_SLIMBUS_INTSTS_prep_cont_sts_END     (13)
#define SOC_SLIMBUS_INTSTS_Rx_Msg_OFL_sts_START  (14)
#define SOC_SLIMBUS_INTSTS_Rx_Msg_OFL_sts_END    (14)
#define SOC_SLIMBUS_INTSTS_clk_pause_sts_START   (15)
#define SOC_SLIMBUS_INTSTS_clk_pause_sts_END     (15)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_TXARBI1_UNION
 结构说明  : TXARBI1 寄存器结构定义。地址偏移量:0x0008，初值:0x0000，宽度:32
 寄存器说明: 传输仲裁寄存器1
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ArbitrationPriority : 3;  /* bit[0-2]  : Transmitted Message仲裁优先级
                                                               3'b000:Reserved
                                                               3'b001:低优先级消息
                                                               3'b010:默认优先级消息
                                                               3'b011:高优先级消息
                                                               3'b100:Manager assigned only */
        unsigned int  AE                  : 1;  /* bit[3]    : 1'b0:禁止AE
                                                               1'b1:使能Arbitration Extension */
        unsigned int  ArbitrationType     : 4;  /* bit[4-7]  : 仲裁类型
                                                               4'b0000:无仲裁
                                                               b0001-b0100:Reserved
                                                               4'b0101:长仲裁
                                                               b0110-b1110:Reserved
                                                               3'b1111:短仲裁 */
        unsigned int  Source_LA_EA        : 8;  /* bit[8-15] : Source Logical Address[7:0] 或
                                                               Enumeration Address[47:40] */
        unsigned int  Source_EA           : 16; /* bit[16-31]: Source Enumeration Address[39:24] */
    } reg;
} SOC_SLIMBUS_TXARBI1_UNION;
#define SOC_SLIMBUS_TXARBI1_ArbitrationPriority_START  (0)
#define SOC_SLIMBUS_TXARBI1_ArbitrationPriority_END    (2)
#define SOC_SLIMBUS_TXARBI1_AE_START                   (3)
#define SOC_SLIMBUS_TXARBI1_AE_END                     (3)
#define SOC_SLIMBUS_TXARBI1_ArbitrationType_START      (4)
#define SOC_SLIMBUS_TXARBI1_ArbitrationType_END        (7)
#define SOC_SLIMBUS_TXARBI1_Source_LA_EA_START         (8)
#define SOC_SLIMBUS_TXARBI1_Source_LA_EA_END           (15)
#define SOC_SLIMBUS_TXARBI1_Source_EA_START            (16)
#define SOC_SLIMBUS_TXARBI1_Source_EA_END              (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_TXARBI2_UNION
 结构说明  : TXARBI2 寄存器结构定义。地址偏移量:0x000C，初值:0x0000，宽度:32
 寄存器说明: 传输仲裁寄存器2
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Source_EA : 16; /* bit[0-15] : Source Enumeration Address[23:8] */
        unsigned int  Source_EA : 8;  /* bit[16-23]: Source Enumeration Address[7:0] */
        unsigned int  reserved  : 8;  /* bit[24-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_TXARBI2_UNION;
#define SOC_SLIMBUS_TXARBI2_Source_EA_START  (0)
#define SOC_SLIMBUS_TXARBI2_Source_EA_END    (15)
#define SOC_SLIMBUS_TXARBI2_Source_EA_START  (16)
#define SOC_SLIMBUS_TXARBI2_Source_EA_END    (23)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_TXMSGHDR1_UNION
 结构说明  : TXMSGHDR1 寄存器结构定义。地址偏移量:0x0010，初值:0x0000，宽度:32
 寄存器说明: 传输消息头1
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Remaininglength  : 5;  /* bit[0-4]  : Message中剩余的字节数
                                                            5'b00000:剩余'0'
                                                            5'b00001:剩余'1'
                                                            ..
                                                            ..
                                                            5'b11111:剩余'31' */
        unsigned int  Messagetype      : 3;  /* bit[5-7]  : Message类型
                                                            3'b000:Core Msg
                                                            3'b001:Destination-referred Device Class-specific Msg
                                                            3'b010:Destination-referred User Msg
                                                            3'b011:非法的
                                                            3'b100:Reserved
                                                            3'b101:Source-referred Device Class-specific Msg
                                                            3'b110:Source-referred User Msg
                                                            3'b111:Escape */
        unsigned int  Messagecode      : 7;  /* bit[8-14] : The nature of Message
                                                            例如：
                                                            7'h01:Report Present Msg code
                                                            7'h02:分配logical Address */
        unsigned int  PrimaryIntegrity : 5;  /* bit[15-19]: These 4 bits are invalid because primary integrity is calculated inside the Component. */
        unsigned int  DestinationType  : 2;  /* bit[20-21]: Destination Address interpreted方式
                                                            2'b00:Destination是Logical Address
                                                            2'b01:Destination是Enumeration Address
                                                            2'b10:Reserved
                                                            2'b11:Header中无Destination Address，所有Devices都是Destinations */
        unsigned int  reserved         : 2;  /* bit[22-23]: Reserved */
        unsigned int  Dest_EA          : 8;  /* bit[24-31]: Enumeration Address[47:40] */
    } reg;
} SOC_SLIMBUS_TXMSGHDR1_UNION;
#define SOC_SLIMBUS_TXMSGHDR1_Remaininglength_START   (0)
#define SOC_SLIMBUS_TXMSGHDR1_Remaininglength_END     (4)
#define SOC_SLIMBUS_TXMSGHDR1_Messagetype_START       (5)
#define SOC_SLIMBUS_TXMSGHDR1_Messagetype_END         (7)
#define SOC_SLIMBUS_TXMSGHDR1_Messagecode_START       (8)
#define SOC_SLIMBUS_TXMSGHDR1_Messagecode_END         (14)
#define SOC_SLIMBUS_TXMSGHDR1_PrimaryIntegrity_START  (15)
#define SOC_SLIMBUS_TXMSGHDR1_PrimaryIntegrity_END    (19)
#define SOC_SLIMBUS_TXMSGHDR1_DestinationType_START   (20)
#define SOC_SLIMBUS_TXMSGHDR1_DestinationType_END     (21)
#define SOC_SLIMBUS_TXMSGHDR1_Dest_EA_START           (24)
#define SOC_SLIMBUS_TXMSGHDR1_Dest_EA_END             (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_TXMSGHDR2_UNION
 结构说明  : TXMSGHDR2 寄存器结构定义。地址偏移量:0x0014，初值:0x0000，宽度:32
 寄存器说明: 传输消息头2
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Dest_EA : 8;  /* bit[0-7]  : Destination Enumeration Address[39:32] */
        unsigned int  Dest_EA : 8;  /* bit[8-15] : Destination Enumeration Address[31:24] */
        unsigned int  Dest_EA : 8;  /* bit[16-23]: Destination Enumeration Address[23:16] */
        unsigned int  Dest_EA : 8;  /* bit[24-31]: Destination Enumeration Address[15:8] */
    } reg;
} SOC_SLIMBUS_TXMSGHDR2_UNION;
#define SOC_SLIMBUS_TXMSGHDR2_Dest_EA_START  (0)
#define SOC_SLIMBUS_TXMSGHDR2_Dest_EA_END    (7)
#define SOC_SLIMBUS_TXMSGHDR2_Dest_EA_START  (8)
#define SOC_SLIMBUS_TXMSGHDR2_Dest_EA_END    (15)
#define SOC_SLIMBUS_TXMSGHDR2_Dest_EA_START  (16)
#define SOC_SLIMBUS_TXMSGHDR2_Dest_EA_END    (23)
#define SOC_SLIMBUS_TXMSGHDR2_Dest_EA_START  (24)
#define SOC_SLIMBUS_TXMSGHDR2_Dest_EA_END    (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_TXMSGHDR3_UNION
 结构说明  : TXMSGHDR3 寄存器结构定义。地址偏移量:0x0018，初值:0x0000，宽度:32
 寄存器说明: 传输消息头3
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Dest_LA_EA : 8;  /* bit[0-7] : Destination Logical Address[7:0] 或 Destination Enumeration Address[7:0] */
        unsigned int  reserved   : 24; /* bit[8-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_TXMSGHDR3_UNION;
#define SOC_SLIMBUS_TXMSGHDR3_Dest_LA_EA_START  (0)
#define SOC_SLIMBUS_TXMSGHDR3_Dest_LA_EA_END    (7)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_TXMSGPLD3_UNION
 结构说明  : TXMSGPLD3 寄存器结构定义。地址偏移量:0x0024，初值:0x0000，宽度:32
 寄存器说明: 传输消息负载3
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_msg_payload3 : 8;  /* bit[0-7] : 包含Message Payload data(71:64)，数据源为SLIMbus Active Manager */
        unsigned int  msg_rdy         : 1;  /* bit[8]   : 1'b0:message未准备好
                                                          1'b1:message准备好，置1触发Message传送到总线上，set ready前保证所有register已配置，此位由硬件自动清除。 */
        unsigned int                  : 23; /* bit[9-31]: 无效，only [8:0] */
    } reg;
} SOC_SLIMBUS_TXMSGPLD3_UNION;
#define SOC_SLIMBUS_TXMSGPLD3_tx_msg_payload3_START  (0)
#define SOC_SLIMBUS_TXMSGPLD3_tx_msg_payload3_END    (7)
#define SOC_SLIMBUS_TXMSGPLD3_msg_rdy_START          (8)
#define SOC_SLIMBUS_TXMSGPLD3_msg_rdy_END            (8)
#define SOC_SLIMBUS_TXMSGPLD3__START                 (9)
#define SOC_SLIMBUS_TXMSGPLD3__END                   (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_RXARBI1_UNION
 结构说明  : RXARBI1 寄存器结构定义。地址偏移量:0x0038，初值:0x0000，宽度:32
 寄存器说明: 接收仲裁寄存器1
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ArbitrationPriority : 3;  /* bit[0-2]  : Transmitted Message仲裁优先级
                                                               3'b000:Reserved
                                                               3'b001:低优先级消息
                                                               3'b010:默认优先级消息
                                                               3'b011:高优先级消息
                                                               3'b100:Manager assigned only */
        unsigned int  AE                  : 1;  /* bit[3]    : 1'b0:禁止AE
                                                               1'b1:使能Arbitration Extension */
        unsigned int  ArbitrationType     : 4;  /* bit[4-7]  : 仲裁类型
                                                               4'b0000:无仲裁
                                                               b0001-b0100:Reserved
                                                               4'b0101:长仲裁
                                                               b0110-b1110:Reserved
                                                               3'b1111:短仲裁 */
        unsigned int  Source_LA_EA        : 8;  /* bit[8-15] : Source Logical Address[7:0] 或 
                                                               Enumeration Address[47:40] */
        unsigned int  Source_EA           : 8;  /* bit[16-23]: Source Enumeration Address[39:32] */
        unsigned int  Source_EA           : 8;  /* bit[24-31]: Source Enumeration Address[31:24] */
    } reg;
} SOC_SLIMBUS_RXARBI1_UNION;
#define SOC_SLIMBUS_RXARBI1_ArbitrationPriority_START  (0)
#define SOC_SLIMBUS_RXARBI1_ArbitrationPriority_END    (2)
#define SOC_SLIMBUS_RXARBI1_AE_START                   (3)
#define SOC_SLIMBUS_RXARBI1_AE_END                     (3)
#define SOC_SLIMBUS_RXARBI1_ArbitrationType_START      (4)
#define SOC_SLIMBUS_RXARBI1_ArbitrationType_END        (7)
#define SOC_SLIMBUS_RXARBI1_Source_LA_EA_START         (8)
#define SOC_SLIMBUS_RXARBI1_Source_LA_EA_END           (15)
#define SOC_SLIMBUS_RXARBI1_Source_EA_START            (16)
#define SOC_SLIMBUS_RXARBI1_Source_EA_END              (23)
#define SOC_SLIMBUS_RXARBI1_Source_EA_START            (24)
#define SOC_SLIMBUS_RXARBI1_Source_EA_END              (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_RXARBI2_UNION
 结构说明  : RXARBI2 寄存器结构定义。地址偏移量:0x003C，初值:0x0000，宽度:32
 寄存器说明: 接收仲裁寄存器2
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Source_EA : 8;  /* bit[0-7]  : Source Enumeration Address[23:16] */
        unsigned int  Source_EA : 8;  /* bit[8-15] : Source Enumeration Address[15:8] */
        unsigned int  Source_EA : 8;  /* bit[16-23]: Source Enumeration Address[7:0] */
        unsigned int  msg_resp  : 4;  /* bit[24-27]: 响应类型
                                                     4'b0000:无响应
                                                     4'b1010:PACK
                                                     4'b1111:NACK
                                                     b0001-b1001:未定义
                                                     b1011-b1110:未定义 */
        unsigned int  reserved  : 4;  /* bit[28-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_RXARBI2_UNION;
#define SOC_SLIMBUS_RXARBI2_Source_EA_START  (0)
#define SOC_SLIMBUS_RXARBI2_Source_EA_END    (7)
#define SOC_SLIMBUS_RXARBI2_Source_EA_START  (8)
#define SOC_SLIMBUS_RXARBI2_Source_EA_END    (15)
#define SOC_SLIMBUS_RXARBI2_Source_EA_START  (16)
#define SOC_SLIMBUS_RXARBI2_Source_EA_END    (23)
#define SOC_SLIMBUS_RXARBI2_msg_resp_START   (24)
#define SOC_SLIMBUS_RXARBI2_msg_resp_END     (27)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_RXMSGHDR1_UNION
 结构说明  : RXMSGHDR1 寄存器结构定义。地址偏移量:0x0040，初值:0x0000，宽度:32
 寄存器说明: 接收消息头1
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Remaininglength  : 5;  /* bit[0-4]  : Message中剩余的字节数
                                                            5'b00000:剩余'0'
                                                            5'b00001:剩余'1'
                                                            ..
                                                            ..
                                                            5'b11111:剩余'31' */
        unsigned int  Messagetype      : 3;  /* bit[5-7]  : Message类型
                                                            3'b000:Core Msg
                                                            3'b001:Destination-referred Device Class-specific Msg
                                                            3'b010:Destination-referred User Msg
                                                            3'b011:非法的
                                                            3'b100:Reserved
                                                            3'b101:Source-referred Device Class-specific Msg
                                                            3'b110:Source-referred User Msg
                                                            3'b111:Escape */
        unsigned int  Messagecode      : 7;  /* bit[8-14] : The nature of Message
                                                            例如：
                                                            7'h01:Report Present Msg code
                                                            7'h02:分配logical Address */
        unsigned int  reserved_0       : 1;  /* bit[15]   : Reserved */
        unsigned int  PrimaryIntegrity : 4;  /* bit[16-19]: Received primary integrity */
        unsigned int  DestinationType  : 2;  /* bit[20-21]: Destination Address interpreted方式
                                                            2'b00:Destination是Logical Address
                                                            2'b01:Destination是Enumeration Address
                                                            2'b10:Reserved
                                                            2'b11:Header中无Destination Address，所有Devices都是Destinations */
        unsigned int  reserved_1       : 2;  /* bit[22-23]: Reserved */
        unsigned int  Dest_EA          : 8;  /* bit[24-31]: Enumeration Address[47:40] */
    } reg;
} SOC_SLIMBUS_RXMSGHDR1_UNION;
#define SOC_SLIMBUS_RXMSGHDR1_Remaininglength_START   (0)
#define SOC_SLIMBUS_RXMSGHDR1_Remaininglength_END     (4)
#define SOC_SLIMBUS_RXMSGHDR1_Messagetype_START       (5)
#define SOC_SLIMBUS_RXMSGHDR1_Messagetype_END         (7)
#define SOC_SLIMBUS_RXMSGHDR1_Messagecode_START       (8)
#define SOC_SLIMBUS_RXMSGHDR1_Messagecode_END         (14)
#define SOC_SLIMBUS_RXMSGHDR1_PrimaryIntegrity_START  (16)
#define SOC_SLIMBUS_RXMSGHDR1_PrimaryIntegrity_END    (19)
#define SOC_SLIMBUS_RXMSGHDR1_DestinationType_START   (20)
#define SOC_SLIMBUS_RXMSGHDR1_DestinationType_END     (21)
#define SOC_SLIMBUS_RXMSGHDR1_Dest_EA_START           (24)
#define SOC_SLIMBUS_RXMSGHDR1_Dest_EA_END             (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_RXMSGHDR2_UNION
 结构说明  : RXMSGHDR2 寄存器结构定义。地址偏移量:0x0044，初值:0x0000，宽度:32
 寄存器说明: 接收消息头2
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Dest_EA : 8;  /* bit[0-7]  : Destination Enumeration Address[15:8] */
        unsigned int  Dest_EA : 8;  /* bit[8-15] : Destination Enumeration Address[23:16] */
        unsigned int  Dest_EA : 8;  /* bit[16-23]: Destination Enumeration Address[31:24] */
        unsigned int  Dest_EA : 8;  /* bit[24-31]: Destination Enumeration Address[39:32] */
    } reg;
} SOC_SLIMBUS_RXMSGHDR2_UNION;
#define SOC_SLIMBUS_RXMSGHDR2_Dest_EA_START  (0)
#define SOC_SLIMBUS_RXMSGHDR2_Dest_EA_END    (7)
#define SOC_SLIMBUS_RXMSGHDR2_Dest_EA_START  (8)
#define SOC_SLIMBUS_RXMSGHDR2_Dest_EA_END    (15)
#define SOC_SLIMBUS_RXMSGHDR2_Dest_EA_START  (16)
#define SOC_SLIMBUS_RXMSGHDR2_Dest_EA_END    (23)
#define SOC_SLIMBUS_RXMSGHDR2_Dest_EA_START  (24)
#define SOC_SLIMBUS_RXMSGHDR2_Dest_EA_END    (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_RXMSGHDR3_UNION
 结构说明  : RXMSGHDR3 寄存器结构定义。地址偏移量:0x0048，初值:0x0000，宽度:32
 寄存器说明: 接收消息头3
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Dest_LA_EA : 8;  /* bit[0-7] : Destination Logical Address[7:0] 或 Destination Enumeration Address[7:0] */
        unsigned int  reserved   : 24; /* bit[8-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_RXMSGHDR3_UNION;
#define SOC_SLIMBUS_RXMSGHDR3_Dest_LA_EA_START  (0)
#define SOC_SLIMBUS_RXMSGHDR3_Dest_LA_EA_END    (7)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_IFEA4732_UNION
 结构说明  : IFEA4732 寄存器结构定义。地址偏移量:0x006C，初值:0x0000，宽度:32
 寄存器说明: 接口设备枚举地址
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ifea_47_32 : 16; /* bit[0-15] : interface device的Enumeration Address [47:32] */
        unsigned int  reserved   : 16; /* bit[16-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_IFEA4732_UNION;
#define SOC_SLIMBUS_IFEA4732_ifea_47_32_START  (0)
#define SOC_SLIMBUS_IFEA4732_ifea_47_32_END    (15)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_FEA4732_UNION
 结构说明  : FEA4732 寄存器结构定义。地址偏移量:0x0078，初值:0x0000，宽度:32
 寄存器说明: Framer设备枚举地址
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ifea_47_32 : 16; /* bit[0-15] : Framer的Enumeration Address [47:32] */
        unsigned int  reserved   : 16; /* bit[16-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_FEA4732_UNION;
#define SOC_SLIMBUS_FEA4732_ifea_47_32_START  (0)
#define SOC_SLIMBUS_FEA4732_ifea_47_32_END    (15)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_CNFG_UNION
 结构说明  : CNFG 寄存器结构定义。地址偏移量:0x0080，初值:0x0000，宽度:32
 寄存器说明: 配置寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  fr_pri_sec        : 1;  /* bit[0]    : 1'b0:framer device作为次设备(inactive Framer)
                                                             1'b1:framer device作为主设备(active Framer) */
        unsigned int  am_en             : 1;  /* bit[1]    : 1'b0:关闭AM
                                                             1'b1:开启AM(Active Manager) */
        unsigned int  fr_en             : 1;  /* bit[2]    : 1'b0:关闭Framer
                                                             1'b1:开启Framer */
        unsigned int  gd0_en            : 1;  /* bit[3]    : 1'b0:关闭GD0
                                                             1'b1:开启GD0(Generic Device 0) */
        unsigned int  fr_clk_freq       : 4;  /* bit[4-7]  : 时钟输入的root frequency
                                                             例如：
                                                             4'b1000:Root Frequency 26 Mhz */
        unsigned int  fr_clk_gear       : 5;  /* bit[8-12] : 时钟输入的gear
                                                             例如：
                                                             4'b1010:Clock Gear 10 */
        unsigned int  fr_boot_mode      : 5;  /* bit[13-17]: 作为主设备，framer device启动总线的mode */
        unsigned int  fr_boot_gear      : 4;  /* bit[18-21]: 启动时，framer device的gear */
        unsigned int  fr_clk_pause      : 1;  /* bit[22]   : 常用于唤醒slimbus clock
                                                             1'b1:framer进入clock pause
                                                             1'b1:framer退出clock pause */
        unsigned int  if_en             : 1;  /* bit[23]   : 1'b0:disable IF
                                                             1'b1:enable IF(Interface Device) */
        unsigned int  framer_clk_on_off : 1;  /* bit[24]   : 转换到'Active Framer'前设置为1，注意同时要提供slim_base_clk */
        unsigned int  reserved          : 7;  /* bit[25-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_CNFG_UNION;
#define SOC_SLIMBUS_CNFG_fr_pri_sec_START         (0)
#define SOC_SLIMBUS_CNFG_fr_pri_sec_END           (0)
#define SOC_SLIMBUS_CNFG_am_en_START              (1)
#define SOC_SLIMBUS_CNFG_am_en_END                (1)
#define SOC_SLIMBUS_CNFG_fr_en_START              (2)
#define SOC_SLIMBUS_CNFG_fr_en_END                (2)
#define SOC_SLIMBUS_CNFG_gd0_en_START             (3)
#define SOC_SLIMBUS_CNFG_gd0_en_END               (3)
#define SOC_SLIMBUS_CNFG_fr_clk_freq_START        (4)
#define SOC_SLIMBUS_CNFG_fr_clk_freq_END          (7)
#define SOC_SLIMBUS_CNFG_fr_clk_gear_START        (8)
#define SOC_SLIMBUS_CNFG_fr_clk_gear_END          (12)
#define SOC_SLIMBUS_CNFG_fr_boot_mode_START       (13)
#define SOC_SLIMBUS_CNFG_fr_boot_mode_END         (17)
#define SOC_SLIMBUS_CNFG_fr_boot_gear_START       (18)
#define SOC_SLIMBUS_CNFG_fr_boot_gear_END         (21)
#define SOC_SLIMBUS_CNFG_fr_clk_pause_START       (22)
#define SOC_SLIMBUS_CNFG_fr_clk_pause_END         (22)
#define SOC_SLIMBUS_CNFG_if_en_START              (23)
#define SOC_SLIMBUS_CNFG_if_en_END                (23)
#define SOC_SLIMBUS_CNFG_framer_clk_on_off_START  (24)
#define SOC_SLIMBUS_CNFG_framer_clk_on_off_END    (24)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_INTR_UNION
 结构说明  : INTR 寄存器结构定义。地址偏移量:0x0084，初值:0x0000，宽度:32
 寄存器说明: 中断寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  am_if_int : 1;  /* bit[0]   : 中断状态位:
                                                    1'b0:无中断
                                                    1'b1:有中断来自AM或IF */
        unsigned int  gd0_int   : 1;  /* bit[1]   : 中断状态位:
                                                    1'b0:无中断
                                                    1'b1:有中断来自GD0 */
        unsigned int  reserved  : 30; /* bit[2-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_INTR_UNION;
#define SOC_SLIMBUS_INTR_am_if_int_START  (0)
#define SOC_SLIMBUS_INTR_am_if_int_END    (0)
#define SOC_SLIMBUS_INTR_gd0_int_START    (1)
#define SOC_SLIMBUS_INTR_gd0_int_END      (1)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_RPPREEN_UNION
 结构说明  : RPPREEN 寄存器结构定义。地址偏移量:0x0088，初值:0x0000，宽度:32
 寄存器说明: Report Present Enable
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  af_rp_pre_en   : 1;  /* bit[0]    : 设1'b1使Framer device切换到waiting_LA状态 */
        unsigned int  if_rp_pre_en   : 1;  /* bit[1]    : 设1'b1使IF切换到waiting_LA状态 */
        unsigned int  gd0_rp_pre_en  : 1;  /* bit[2]    : 设1'b1使GD0切换到waiting_LA状态 */
        unsigned int  reserved_0     : 5;  /* bit[3-7]  : Reserved for future use */
        unsigned int  fr_enumerated  : 1;  /* bit[8]    : 设1'b1表示Framer device已进入enumerated状态 */
        unsigned int  if_enumerated  : 1;  /* bit[9]    : 设1'b1表示IF已进入enumerated状态 */
        unsigned int  gd0_enumerated : 1;  /* bit[10]   : 设1'b1表示GD0已进入enumerated状态 */
        unsigned int  reserved_1     : 21; /* bit[11-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_RPPREEN_UNION;
#define SOC_SLIMBUS_RPPREEN_af_rp_pre_en_START    (0)
#define SOC_SLIMBUS_RPPREEN_af_rp_pre_en_END      (0)
#define SOC_SLIMBUS_RPPREEN_if_rp_pre_en_START    (1)
#define SOC_SLIMBUS_RPPREEN_if_rp_pre_en_END      (1)
#define SOC_SLIMBUS_RPPREEN_gd0_rp_pre_en_START   (2)
#define SOC_SLIMBUS_RPPREEN_gd0_rp_pre_en_END     (2)
#define SOC_SLIMBUS_RPPREEN_fr_enumerated_START   (8)
#define SOC_SLIMBUS_RPPREEN_fr_enumerated_END     (8)
#define SOC_SLIMBUS_RPPREEN_if_enumerated_START   (9)
#define SOC_SLIMBUS_RPPREEN_if_enumerated_END     (9)
#define SOC_SLIMBUS_RPPREEN_gd0_enumerated_START  (10)
#define SOC_SLIMBUS_RPPREEN_gd0_enumerated_END    (10)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_GD0EA4732_UNION
 结构说明  : GD0EA4732 寄存器结构定义。地址偏移量:0x0154，初值:0x0000，宽度:32
 寄存器说明: 通用设备0枚举地址
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gd0ea_47_32 : 16; /* bit[0-15] : GD0的Enumeration Address [47:32] */
        unsigned int  reserved    : 16; /* bit[16-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_GD0EA4732_UNION;
#define SOC_SLIMBUS_GD0EA4732_gd0ea_47_32_START  (0)
#define SOC_SLIMBUS_GD0EA4732_gd0ea_47_32_END    (15)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_GD0LA_UNION
 结构说明  : GD0LA 寄存器结构定义。地址偏移量:0x0158，初值:0x0000，宽度:32
 寄存器说明: 通用设备0逻辑地址
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gd0la    : 8;  /* bit[0-7] : Logical Address of generic device0 */
        unsigned int  reserved : 24; /* bit[8-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_GD0LA_UNION;
#define SOC_SLIMBUS_GD0LA_gd0la_START     (0)
#define SOC_SLIMBUS_GD0LA_gd0la_END       (7)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_GD0INTEN_UNION
 结构说明  : GD0INTEN 寄存器结构定义。地址偏移量:0x015C，初值:0x0000，宽度:32
 寄存器说明: 通用设备0中断使能寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gd0_p0_int_en  : 1;  /* bit[0]    : 1'b0:禁止port 0中断
                                                          1'b1:使能port 0中断 */
        unsigned int  gd0_p1_int_en  : 1;  /* bit[1]    : 1'b0:禁止port 1中断
                                                          1'b1:使能port 1中断 */
        unsigned int  gd0_p2_int_en  : 1;  /* bit[2]    : 1'b0:禁止port 2中断
                                                          1'b1:使能port 2中断 */
        unsigned int  gd0_p3_int_en  : 1;  /* bit[3]    : 1'b0:禁止port 3中断
                                                          1'b1:使能port 3中断 */
        unsigned int  gd0_p4_int_en  : 1;  /* bit[4]    : 1'b0:禁止port 4中断
                                                          1'b1:使能port 4中断 */
        unsigned int  gd0_p5_int_en  : 1;  /* bit[5]    : 1'b0:禁止port 5中断
                                                          1'b1:使能port 5中断 */
        unsigned int  gd0_p6_int_en  : 1;  /* bit[6]    : 1'b0:禁止port 6中断
                                                          1'b1:使能port 6中断 */
        unsigned int  gd0_p7_int_en  : 1;  /* bit[7]    : 1'b0:禁止port 7中断
                                                          1'b1:使能port 7中断 */
        unsigned int  gd0_p8_int_en  : 1;  /* bit[8]    : 1'b0:禁止port 8中断
                                                          1'b1:使能port 8中断 */
        unsigned int  gd0_p9_int_en  : 1;  /* bit[9]    : 1'b0:禁止port 9中断
                                                          1'b1:使能port 9中断 */
        unsigned int  gd0_p10_int_en : 1;  /* bit[10]   : 1'b0:禁止port 10中断
                                                          1'b1:使能port 10中断 */
        unsigned int  gd0_p11_int_en : 1;  /* bit[11]   : 1'b0:禁止port 11中断
                                                          1'b1:使能port 11中断 */
        unsigned int  gd0_p12_int_en : 1;  /* bit[12]   : 1'b0:禁止port 12中断
                                                          1'b1:使能port 12中断 */
        unsigned int  gd0_p13_int_en : 1;  /* bit[13]   : 1'b0:禁止port 13中断
                                                          1'b1:使能port 13中断 */
        unsigned int  gd0_p14_int_en : 1;  /* bit[14]   : 1'b0:禁止port 14中断
                                                          1'b1:使能port 14中断 */
        unsigned int  gd0_p15_int_en : 1;  /* bit[15]   : 1'b0:禁止port 15中断
                                                          1'b1:使能port 15中断 */
        unsigned int  reserved       : 16; /* bit[16-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_GD0INTEN_UNION;
#define SOC_SLIMBUS_GD0INTEN_gd0_p0_int_en_START   (0)
#define SOC_SLIMBUS_GD0INTEN_gd0_p0_int_en_END     (0)
#define SOC_SLIMBUS_GD0INTEN_gd0_p1_int_en_START   (1)
#define SOC_SLIMBUS_GD0INTEN_gd0_p1_int_en_END     (1)
#define SOC_SLIMBUS_GD0INTEN_gd0_p2_int_en_START   (2)
#define SOC_SLIMBUS_GD0INTEN_gd0_p2_int_en_END     (2)
#define SOC_SLIMBUS_GD0INTEN_gd0_p3_int_en_START   (3)
#define SOC_SLIMBUS_GD0INTEN_gd0_p3_int_en_END     (3)
#define SOC_SLIMBUS_GD0INTEN_gd0_p4_int_en_START   (4)
#define SOC_SLIMBUS_GD0INTEN_gd0_p4_int_en_END     (4)
#define SOC_SLIMBUS_GD0INTEN_gd0_p5_int_en_START   (5)
#define SOC_SLIMBUS_GD0INTEN_gd0_p5_int_en_END     (5)
#define SOC_SLIMBUS_GD0INTEN_gd0_p6_int_en_START   (6)
#define SOC_SLIMBUS_GD0INTEN_gd0_p6_int_en_END     (6)
#define SOC_SLIMBUS_GD0INTEN_gd0_p7_int_en_START   (7)
#define SOC_SLIMBUS_GD0INTEN_gd0_p7_int_en_END     (7)
#define SOC_SLIMBUS_GD0INTEN_gd0_p8_int_en_START   (8)
#define SOC_SLIMBUS_GD0INTEN_gd0_p8_int_en_END     (8)
#define SOC_SLIMBUS_GD0INTEN_gd0_p9_int_en_START   (9)
#define SOC_SLIMBUS_GD0INTEN_gd0_p9_int_en_END     (9)
#define SOC_SLIMBUS_GD0INTEN_gd0_p10_int_en_START  (10)
#define SOC_SLIMBUS_GD0INTEN_gd0_p10_int_en_END    (10)
#define SOC_SLIMBUS_GD0INTEN_gd0_p11_int_en_START  (11)
#define SOC_SLIMBUS_GD0INTEN_gd0_p11_int_en_END    (11)
#define SOC_SLIMBUS_GD0INTEN_gd0_p12_int_en_START  (12)
#define SOC_SLIMBUS_GD0INTEN_gd0_p12_int_en_END    (12)
#define SOC_SLIMBUS_GD0INTEN_gd0_p13_int_en_START  (13)
#define SOC_SLIMBUS_GD0INTEN_gd0_p13_int_en_END    (13)
#define SOC_SLIMBUS_GD0INTEN_gd0_p14_int_en_START  (14)
#define SOC_SLIMBUS_GD0INTEN_gd0_p14_int_en_END    (14)
#define SOC_SLIMBUS_GD0INTEN_gd0_p15_int_en_START  (15)
#define SOC_SLIMBUS_GD0INTEN_gd0_p15_int_en_END    (15)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_GD0INTSTS_UNION
 结构说明  : GD0INTSTS 寄存器结构定义。地址偏移量:0x0160，初值:0x0000，宽度:32
 寄存器说明: 通用设备0中断状态寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gd0_p0_int_sts  : 1;  /* bit[0]    : Port 0中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p1_int_sts  : 1;  /* bit[1]    : Port 1中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p2_int_sts  : 1;  /* bit[2]    : Port 2中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p3_int_sts  : 1;  /* bit[3]    : Port 3中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p4_int_sts  : 1;  /* bit[4]    : Port 4中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p5_int_sts  : 1;  /* bit[5]    : Port 5中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p6_int_sts  : 1;  /* bit[6]    : Port 6中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p7_int_sts  : 1;  /* bit[7]    : Port 7中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p8_int_sts  : 1;  /* bit[8]    : Port 8中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p9_int_sts  : 1;  /* bit[9]    : Port 9中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p10_int_sts : 1;  /* bit[10]   : Port 10中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p11_int_sts : 1;  /* bit[11]   : Port 11中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p12_int_sts : 1;  /* bit[12]   : Port 12中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p13_int_sts : 1;  /* bit[13]   : Port 13中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p14_int_sts : 1;  /* bit[14]   : Port 14中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  gd0_p15_int_sts : 1;  /* bit[15]   : Port 15中断状态
                                                           1'b0:无中断
                                                           1'b1:有中断 */
        unsigned int  reserved        : 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_SLIMBUS_GD0INTSTS_UNION;
#define SOC_SLIMBUS_GD0INTSTS_gd0_p0_int_sts_START   (0)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p0_int_sts_END     (0)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p1_int_sts_START   (1)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p1_int_sts_END     (1)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p2_int_sts_START   (2)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p2_int_sts_END     (2)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p3_int_sts_START   (3)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p3_int_sts_END     (3)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p4_int_sts_START   (4)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p4_int_sts_END     (4)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p5_int_sts_START   (5)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p5_int_sts_END     (5)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p6_int_sts_START   (6)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p6_int_sts_END     (6)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p7_int_sts_START   (7)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p7_int_sts_END     (7)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p8_int_sts_START   (8)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p8_int_sts_END     (8)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p9_int_sts_START   (9)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p9_int_sts_END     (9)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p10_int_sts_START  (10)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p10_int_sts_END    (10)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p11_int_sts_START  (11)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p11_int_sts_END    (11)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p12_int_sts_START  (12)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p12_int_sts_END    (12)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p13_int_sts_START  (13)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p13_int_sts_END    (13)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p14_int_sts_START  (14)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p14_int_sts_END    (14)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p15_int_sts_START  (15)
#define SOC_SLIMBUS_GD0INTSTS_gd0_p15_int_sts_END    (15)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_D0PiINTEN_UNION
 结构说明  : D0PiINTEN 寄存器结构定义。地址偏移量:0x0164+0x10*i，初值:0x0000，宽度:32
 寄存器说明: 通用设备0端口i(0-15)中断使能寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  src_threshold     : 1;  /* bit[0]    : 1'b0:禁止Source Threshold reached中断
                                                             1'b1:使能Source Threshold reached中断 */
        unsigned int  src_fifo_empty    : 1;  /* bit[1]    : 1'b0:禁止source FIFO empty中断
                                                             1'b1:使能source FIFO empty中断 */
        unsigned int  src_fifo_full     : 1;  /* bit[2]    : 1'b0:禁止source FIFO full中断
                                                             1'b1:使能source FIFO full中断 */
        unsigned int  src_fifo_underrun : 1;  /* bit[3]    : 1'b0:禁止source FIFO underrun中断
                                                             1'b1:使能source FIFO underrun中断 */
        unsigned int  reserved_0        : 12; /* bit[4-15] : Reserved for Future Use */
        unsigned int  sink_threshold    : 1;  /* bit[16]   : 1'b0:禁止Sink Threshold reached中断
                                                             1'b1:使能Sink Threshold reached中断 */
        unsigned int  sink_fifo_empty   : 1;  /* bit[17]   : 1'b0:禁止Sink FIFO empty中断
                                                             1'b1:使能Sink FIFO empty中断 */
        unsigned int  sink_fifo_full    : 1;  /* bit[18]   : 1'b0:禁止Sink FIFO full中断
                                                             1'b1:使能Sink FIFO full中断 */
        unsigned int  sink_fifo_overrun : 1;  /* bit[19]   : 1'b0:禁止Sink FIFO overrun中断
                                                             1'b1:使能Sink FIFO overrun中断 */
        unsigned int  reserved_1        : 11; /* bit[20-30]: Reserved for Future Use */
        unsigned int  Timeout_en        : 1;  /* bit[31]   : 1'b0:禁止timeout中断
                                                             1'b1:使能timeout中断 */
    } reg;
} SOC_SLIMBUS_D0PiINTEN_UNION;
#define SOC_SLIMBUS_D0PiINTEN_src_threshold_START      (0)
#define SOC_SLIMBUS_D0PiINTEN_src_threshold_END        (0)
#define SOC_SLIMBUS_D0PiINTEN_src_fifo_empty_START     (1)
#define SOC_SLIMBUS_D0PiINTEN_src_fifo_empty_END       (1)
#define SOC_SLIMBUS_D0PiINTEN_src_fifo_full_START      (2)
#define SOC_SLIMBUS_D0PiINTEN_src_fifo_full_END        (2)
#define SOC_SLIMBUS_D0PiINTEN_src_fifo_underrun_START  (3)
#define SOC_SLIMBUS_D0PiINTEN_src_fifo_underrun_END    (3)
#define SOC_SLIMBUS_D0PiINTEN_sink_threshold_START     (16)
#define SOC_SLIMBUS_D0PiINTEN_sink_threshold_END       (16)
#define SOC_SLIMBUS_D0PiINTEN_sink_fifo_empty_START    (17)
#define SOC_SLIMBUS_D0PiINTEN_sink_fifo_empty_END      (17)
#define SOC_SLIMBUS_D0PiINTEN_sink_fifo_full_START     (18)
#define SOC_SLIMBUS_D0PiINTEN_sink_fifo_full_END       (18)
#define SOC_SLIMBUS_D0PiINTEN_sink_fifo_overrun_START  (19)
#define SOC_SLIMBUS_D0PiINTEN_sink_fifo_overrun_END    (19)
#define SOC_SLIMBUS_D0PiINTEN_Timeout_en_START         (31)
#define SOC_SLIMBUS_D0PiINTEN_Timeout_en_END           (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_D0PiINTSTS_UNION
 结构说明  : D0PiINTSTS 寄存器结构定义。地址偏移量:0x0168+0x10*i，初值:0x0000，宽度:32
 寄存器说明: 通用设备0端口i(0-15)中断状态寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Src_Thresholdreached  : 1;  /* bit[0]    : 1'b0:无中断
                                                                 1'b1:有中断，当write transaction时Threshold到达（AHB clock需处于active状态） */
        unsigned int  src_fifo_empty        : 1;  /* bit[1]    : 1'b0:无中断
                                                                 1'b1:有中断，当Source FIFO中无数据，FIFO非空时清0 */
        unsigned int  src_fifo_full         : 1;  /* bit[2]    : 1'b0:无中断
                                                                 1'b1:有中断，当Source FIFO full时（AHB clock需处于active状态） */
        unsigned int  src_fifo_underrun     : 1;  /* bit[3]    : 1'b0:无中断
                                                                 1'b1:有中断，当source port尝试从空FIFO中读数据（AHB clock需处于active状态） */
        unsigned int  src_fifo_empty_poll   : 1;  /* bit[4]    : 1'b0:无中断
                                                                 1'b1:有中断，当Source FIFO中无数据此标志位置1，FIFO非空时清0 */
        unsigned int  reserved_0            : 11; /* bit[5-15] : Reserved for Future Use */
        unsigned int  sink_Thresholdreached : 1;  /* bit[16]   : 1'b0:无中断
                                                                 1'b1:有中断，当read transaction时Threshold到达，AHB clock必须处于active状态 */
        unsigned int  sink_fifo_empty       : 1;  /* bit[17]   : 1'b0:无中断
                                                                 1'b1:有中断，当Sink FIFO中无数据，FIFO非空时清0 */
        unsigned int  sink_fifo_full        : 1;  /* bit[18]   : 1'b0:无中断
                                                                 1'b1:有中断，当Sink FIFO full时（AHB clock需处于active状态） */
        unsigned int  sink_fifo_overrun     : 1;  /* bit[19]   : 1'b0:无中断
                                                                 1'b1:有中断，当sink port尝试写数据到Full FIFO中（AHB clock需处于active状态） */
        unsigned int  sink_fifo_empty_poll  : 1;  /* bit[20]   : 1'b0:无中断
                                                                 1'b1:有中断，当Sink FIFO中无数据，FIFO非空时清0 */
        unsigned int  reserved_1            : 10; /* bit[21-30]: Reserved for Future Use */
        unsigned int  Timeout_sts           : 1;  /* bit[31]   : 1'b0:无中断
                                                                 1'b1:有中断，当计数值到达0时 */
    } reg;
} SOC_SLIMBUS_D0PiINTSTS_UNION;
#define SOC_SLIMBUS_D0PiINTSTS_Src_Thresholdreached_START   (0)
#define SOC_SLIMBUS_D0PiINTSTS_Src_Thresholdreached_END     (0)
#define SOC_SLIMBUS_D0PiINTSTS_src_fifo_empty_START         (1)
#define SOC_SLIMBUS_D0PiINTSTS_src_fifo_empty_END           (1)
#define SOC_SLIMBUS_D0PiINTSTS_src_fifo_full_START          (2)
#define SOC_SLIMBUS_D0PiINTSTS_src_fifo_full_END            (2)
#define SOC_SLIMBUS_D0PiINTSTS_src_fifo_underrun_START      (3)
#define SOC_SLIMBUS_D0PiINTSTS_src_fifo_underrun_END        (3)
#define SOC_SLIMBUS_D0PiINTSTS_src_fifo_empty_poll_START    (4)
#define SOC_SLIMBUS_D0PiINTSTS_src_fifo_empty_poll_END      (4)
#define SOC_SLIMBUS_D0PiINTSTS_sink_Thresholdreached_START  (16)
#define SOC_SLIMBUS_D0PiINTSTS_sink_Thresholdreached_END    (16)
#define SOC_SLIMBUS_D0PiINTSTS_sink_fifo_empty_START        (17)
#define SOC_SLIMBUS_D0PiINTSTS_sink_fifo_empty_END          (17)
#define SOC_SLIMBUS_D0PiINTSTS_sink_fifo_full_START         (18)
#define SOC_SLIMBUS_D0PiINTSTS_sink_fifo_full_END           (18)
#define SOC_SLIMBUS_D0PiINTSTS_sink_fifo_overrun_START      (19)
#define SOC_SLIMBUS_D0PiINTSTS_sink_fifo_overrun_END        (19)
#define SOC_SLIMBUS_D0PiINTSTS_sink_fifo_empty_poll_START   (20)
#define SOC_SLIMBUS_D0PiINTSTS_sink_fifo_empty_poll_END     (20)
#define SOC_SLIMBUS_D0PiINTSTS_Timeout_sts_START            (31)
#define SOC_SLIMBUS_D0PiINTSTS_Timeout_sts_END              (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_D0PiCNFG_UNION
 结构说明  : D0PiCNFG 寄存器结构定义。地址偏移量:0x016C+0x10*i，初值:0x0000，宽度:32
 寄存器说明: 通用设备0端口i(0-15)配置寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  FIFOWatermark : 6;  /* bit[0-5]  : 门限值，用于产生threshold reached interrupt。当port配置为source port时，FIFO中数据空间达到门限值时产生中断；配置为sink port时，则FIFO数据达到门限值时产生中断。
                                                         6'b000000:1Byte
                                                         6'b000001:2Bytes
                                                         6'b000010:3Bytes
                                                         6'b000011:4Bytes
                                                         ..
                                                         6'b111111:1Bytes */
        unsigned int  reserved_0    : 10; /* bit[6-15] : Reserved for Future Use */
        unsigned int  dma_en        : 1;  /* bit[16]   : 1'b1:Data Transaction通过Slave DMA Interface */
        unsigned int  packed_en     : 1;  /* bit[17]   : 1'b0:zeros are stuffing，且处于unpacked mode
                                                         1'b1:no zeros are stuffing，且处于packed mode */
        unsigned int  Timeout       : 8;  /* bit[18-25]: 计数值，当从FIFO读或写数据至FIFO时加载。减至0时，timeout_sts置1。 */
        unsigned int  AHB_8_16_32   : 2;  /* bit[26-27]: port的AHB访问位宽
                                                         2'b00:8 bit AHB access
                                                         2'b01:16 bit AHB access
                                                         2'b10:32 bit AHB access */
        unsigned int  wake_up_ena   : 1;  /* bit[28]   : 1'b0:disable system wakeup
                                                         1'b1:当sink mode下port level大于threshold时，使能port i的system wakeup */
        unsigned int  wake_up_sts   : 1;  /* bit[29]   : 1'b0:端口i的wakeup signal处于inactive
                                                         1'b1:端口i的wakeup signal处于active */
        unsigned int  reserved_1    : 2;  /* bit[30-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_D0PiCNFG_UNION;
#define SOC_SLIMBUS_D0PiCNFG_FIFOWatermark_START  (0)
#define SOC_SLIMBUS_D0PiCNFG_FIFOWatermark_END    (5)
#define SOC_SLIMBUS_D0PiCNFG_dma_en_START         (16)
#define SOC_SLIMBUS_D0PiCNFG_dma_en_END           (16)
#define SOC_SLIMBUS_D0PiCNFG_packed_en_START      (17)
#define SOC_SLIMBUS_D0PiCNFG_packed_en_END        (17)
#define SOC_SLIMBUS_D0PiCNFG_Timeout_START        (18)
#define SOC_SLIMBUS_D0PiCNFG_Timeout_END          (25)
#define SOC_SLIMBUS_D0PiCNFG_AHB_8_16_32_START    (26)
#define SOC_SLIMBUS_D0PiCNFG_AHB_8_16_32_END      (27)
#define SOC_SLIMBUS_D0PiCNFG_wake_up_ena_START    (28)
#define SOC_SLIMBUS_D0PiCNFG_wake_up_ena_END      (28)
#define SOC_SLIMBUS_D0PiCNFG_wake_up_sts_START    (29)
#define SOC_SLIMBUS_D0PiCNFG_wake_up_sts_END      (29)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_SDMAiCNFG_UNION
 结构说明  : SDMAiCNFG 寄存器结构定义。地址偏移量:0x0400+0x4*i，初值:0x0000，宽度:32
 寄存器说明: SDMAi(0-15)配置寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0       : 4;  /* bit[0-3]  : Reserved for Future Use */
        unsigned int  DMATransferCount : 20; /* bit[4-23] : Total DMA Transfer Count，每次数据传输时递减，此值必须大于或等于DMA burst size。此值非零时Slave DMA才会initiate the data transaction。
                                                            20'h01:1 (byte/halfword/word) to transfer
                                                            20'h02:2 (bytes/halfwords/words) to transfer
                                                            ...
                                                            20'hfffff:1048575 (bytes/halfwords/words) to transfer */
        unsigned int  DMAburstsize     : 2;  /* bit[24-25]: 单次数据传输的DMA burst size
                                                            2'b00:1 (byte/halfword/word)
                                                            2'b01:4 (bytes/halfwords/words)
                                                            2'b10:8 (bytes/halfwords/words)
                                                            2'b11:Reserved for future use
                                                            DMA Transfer Count如小于DMA burst size，则dma single产生assert。 */
        unsigned int  reserved_1       : 6;  /* bit[26-31]: Reserved for Future Use */
    } reg;
} SOC_SLIMBUS_SDMAiCNFG_UNION;
#define SOC_SLIMBUS_SDMAiCNFG_DMATransferCount_START  (4)
#define SOC_SLIMBUS_SDMAiCNFG_DMATransferCount_END    (23)
#define SOC_SLIMBUS_SDMAiCNFG_DMAburstsize_START      (24)
#define SOC_SLIMBUS_SDMAiCNFG_DMAburstsize_END        (25)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_ADPRTi_UNION
 结构说明  : ADPRTi 寄存器结构定义。地址偏移量:0x0500+0x4*i，初值:0x0000，宽度:32
 寄存器说明: 音频端口i(0-15)寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Period_value   : 13; /* bit[0-12] : 端口i的period value */
        unsigned int  reserved       : 17; /* bit[13-29]: Reserved for future use */
        unsigned int  Smoother_en    : 1;  /* bit[30]   : 1'b0:禁止Smoother bit
                                                          1'b1:使能Smoother bit */
        unsigned int  Period_load_en : 1;  /* bit[31]   : 1'b0:禁止Period load bit
                                                          1'b1:使能Period load bit */
    } reg;
} SOC_SLIMBUS_ADPRTi_UNION;
#define SOC_SLIMBUS_ADPRTi_Period_value_START    (0)
#define SOC_SLIMBUS_ADPRTi_Period_value_END      (12)
#define SOC_SLIMBUS_ADPRTi_Smoother_en_START     (30)
#define SOC_SLIMBUS_ADPRTi_Smoother_en_END       (30)
#define SOC_SLIMBUS_ADPRTi_Period_load_en_START  (31)
#define SOC_SLIMBUS_ADPRTi_Period_load_en_END    (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_FLCONSAV1_UNION
 结构说明  : FLCONSAV1 寄存器结构定义。地址偏移量:0x0900，初值:0x0000，宽度:32
 寄存器说明: FL上下文保存寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  next_frame_state     : 5;  /* bit[0-4]  : FL(Frame Logic)的Next Frame state */
        unsigned int  reserved_0           : 5;  /* bit[5-9]  : Reserved for future use */
        unsigned int  cell1_count_10_0     : 11; /* bit[10-20]: FL的cell count value */
        unsigned int  cell1_count_6_0      : 7;  /* bit[21-27]: FL的cell count value */
        unsigned int  current_clk_rx_state : 3;  /* bit[28-30]: FL的Current Clock Rx state */
        unsigned int  reserved_1           : 1;  /* bit[31]   : Reserved for future use */
    } reg;
} SOC_SLIMBUS_FLCONSAV1_UNION;
#define SOC_SLIMBUS_FLCONSAV1_next_frame_state_START      (0)
#define SOC_SLIMBUS_FLCONSAV1_next_frame_state_END        (4)
#define SOC_SLIMBUS_FLCONSAV1_cell1_count_10_0_START      (10)
#define SOC_SLIMBUS_FLCONSAV1_cell1_count_10_0_END        (20)
#define SOC_SLIMBUS_FLCONSAV1_cell1_count_6_0_START       (21)
#define SOC_SLIMBUS_FLCONSAV1_cell1_count_6_0_END         (27)
#define SOC_SLIMBUS_FLCONSAV1_current_clk_rx_state_START  (28)
#define SOC_SLIMBUS_FLCONSAV1_current_clk_rx_state_END    (30)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_MHCONSAV1_UNION
 结构说明  : MHCONSAV1 寄存器结构定义。地址偏移量:0x0934，初值:0x0000，宽度:32
 寄存器说明: MH上下文保存寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sig_guide_val  : 5;  /* bit[0-4]  : Guide Value */
        unsigned int  sig_clk_gear   : 4;  /* bit[5-8]  : Clock Gear value */
        unsigned int  sig_root_freq  : 4;  /* bit[9-12] : MH的root freq */
        unsigned int  sig_subfr_mode : 5;  /* bit[13-17]: MH(Message Handler)的subframe mode */
        unsigned int  reserved       : 14; /* bit[18-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_MHCONSAV1_UNION;
#define SOC_SLIMBUS_MHCONSAV1_sig_guide_val_START   (0)
#define SOC_SLIMBUS_MHCONSAV1_sig_guide_val_END     (4)
#define SOC_SLIMBUS_MHCONSAV1_sig_clk_gear_START    (5)
#define SOC_SLIMBUS_MHCONSAV1_sig_clk_gear_END      (8)
#define SOC_SLIMBUS_MHCONSAV1_sig_root_freq_START   (9)
#define SOC_SLIMBUS_MHCONSAV1_sig_root_freq_END     (12)
#define SOC_SLIMBUS_MHCONSAV1_sig_subfr_mode_START  (13)
#define SOC_SLIMBUS_MHCONSAV1_sig_subfr_mode_END    (17)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_AFCONSAV1_UNION
 结构说明  : AFCONSAV1 寄存器结构定义。地址偏移量:0x0948，初值:0x0000，宽度:32
 寄存器说明: AF上下文保存寄存器1
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  slim_fr_fl_info_cnt : 3;  /* bit[0-2]  : AF的Frame info count */
        unsigned int  sig_slim_fr_fl_info : 4;  /* bit[3-6]  : AF的Frame info */
        unsigned int  cur_boot_st         : 4;  /* bit[7-10] : AF的current boot state */
        unsigned int  fr_log_addr         : 7;  /* bit[11-17]: AF的logical address */
        unsigned int  root_supr_fr_cnt    : 14; /* bit[18-31]: AF的Root Super Frame Count */
    } reg;
} SOC_SLIMBUS_AFCONSAV1_UNION;
#define SOC_SLIMBUS_AFCONSAV1_slim_fr_fl_info_cnt_START  (0)
#define SOC_SLIMBUS_AFCONSAV1_slim_fr_fl_info_cnt_END    (2)
#define SOC_SLIMBUS_AFCONSAV1_sig_slim_fr_fl_info_START  (3)
#define SOC_SLIMBUS_AFCONSAV1_sig_slim_fr_fl_info_END    (6)
#define SOC_SLIMBUS_AFCONSAV1_cur_boot_st_START          (7)
#define SOC_SLIMBUS_AFCONSAV1_cur_boot_st_END            (10)
#define SOC_SLIMBUS_AFCONSAV1_fr_log_addr_START          (11)
#define SOC_SLIMBUS_AFCONSAV1_fr_log_addr_END            (17)
#define SOC_SLIMBUS_AFCONSAV1_root_supr_fr_cnt_START     (18)
#define SOC_SLIMBUS_AFCONSAV1_root_supr_fr_cnt_END       (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_AFCONSAV2_UNION
 结构说明  : AFCONSAV2 寄存器结构定义。地址偏移量:0x094C，初值:0x0000，宽度:32
 寄存器说明: AF上下文保存寄存器2
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  curr_enum_st : 2;  /* bit[0-1]  : AF的Current Enumeration State */
        unsigned int  cur_fr_st    : 2;  /* bit[2-3]  : AF的Current Frame State */
        unsigned int  core_info    : 3;  /* bit[4-6]  : AF的Core Information Element */
        unsigned int  dev_info     : 3;  /* bit[7-9]  : AF的Device Information Element */
        unsigned int  reserved     : 22; /* bit[10-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_AFCONSAV2_UNION;
#define SOC_SLIMBUS_AFCONSAV2_curr_enum_st_START  (0)
#define SOC_SLIMBUS_AFCONSAV2_curr_enum_st_END    (1)
#define SOC_SLIMBUS_AFCONSAV2_cur_fr_st_START     (2)
#define SOC_SLIMBUS_AFCONSAV2_cur_fr_st_END       (3)
#define SOC_SLIMBUS_AFCONSAV2_core_info_START     (4)
#define SOC_SLIMBUS_AFCONSAV2_core_info_END       (6)
#define SOC_SLIMBUS_AFCONSAV2_dev_info_START      (7)
#define SOC_SLIMBUS_AFCONSAV2_dev_info_END        (9)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_IFCONSAV1_UNION
 结构说明  : IFCONSAV1 寄存器结构定义。地址偏移量:0x0968，初值:0x0000，宽度:32
 寄存器说明: IF上下文保存寄存器
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  logical_addr   : 8;  /* bit[0-7]  : IF的Logical Address value */
        unsigned int  receive_ala    : 1;  /* bit[8]    : IF的receive ala bit value */
        unsigned int  arb_pri        : 3;  /* bit[9-11] : IF的Arbitration Priority */
        unsigned int  curr_enu_state : 2;  /* bit[12-13]: IF的Current Enumeration State */
        unsigned int  reserved       : 18; /* bit[14-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_IFCONSAV1_UNION;
#define SOC_SLIMBUS_IFCONSAV1_logical_addr_START    (0)
#define SOC_SLIMBUS_IFCONSAV1_logical_addr_END      (7)
#define SOC_SLIMBUS_IFCONSAV1_receive_ala_START     (8)
#define SOC_SLIMBUS_IFCONSAV1_receive_ala_END       (8)
#define SOC_SLIMBUS_IFCONSAV1_arb_pri_START         (9)
#define SOC_SLIMBUS_IFCONSAV1_arb_pri_END           (11)
#define SOC_SLIMBUS_IFCONSAV1_curr_enu_state_START  (12)
#define SOC_SLIMBUS_IFCONSAV1_curr_enu_state_END    (13)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_GD0MSGCS1_UNION
 结构说明  : GD0MSGCS1 寄存器结构定义。地址偏移量:0x0978，初值:0x0000，宽度:32
 寄存器说明: GD0消息上下文保存寄存器1
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sig_channel_1      : 1;  /* bit[0] : GD0的sig_channel_1 value */
        unsigned int  sig_channel_2      : 1;  /* bit[1] : GD0的sig_channel_2 value */
        unsigned int  sig_channel_3      : 1;  /* bit[2] : GD0的sig_channel_3 value */
        unsigned int  sig_channel_4      : 1;  /* bit[3] : GD0的sig_channel_4 value */
        unsigned int  sig_channel_5      : 1;  /* bit[4] : GD0的sig_channel_5 value */
        unsigned int  sig_channel_6      : 1;  /* bit[5] : GD0的sig_channel_6 value */
        unsigned int  sig_channel_7      : 1;  /* bit[6] : GD0的sig_channel_7 value */
        unsigned int  sig_channel_8      : 1;  /* bit[7] : GD0的sig_channel_8 value */
        unsigned int  sig_channel_9      : 1;  /* bit[8] : GD0的sig_channel_9 value */
        unsigned int  sig_channel_10     : 1;  /* bit[9] : GD0的sig_channel_10 value */
        unsigned int  sig_channel_11     : 1;  /* bit[10]: GD0的sig_channel_11 value */
        unsigned int  sig_channel_12     : 1;  /* bit[11]: GD0的sig_channel_12 value */
        unsigned int  sig_channel_13     : 1;  /* bit[12]: GD0的sig_channel_13 value */
        unsigned int  sig_channel_14     : 1;  /* bit[13]: GD0的sig_channel_14 value */
        unsigned int  sig_channel_15     : 1;  /* bit[14]: GD0的sig_channel_15 value */
        unsigned int  sig_channel_16     : 1;  /* bit[15]: GD0的sig_channel_16 value */
        unsigned int  sig_act_channel_1  : 1;  /* bit[16]: GD0的sig_act_channel_1 value */
        unsigned int  sig_act_channel_2  : 1;  /* bit[17]: GD0的sig_act_channel_2 value */
        unsigned int  sig_act_channel_3  : 1;  /* bit[18]: GD0的sig_act_channel_3 value */
        unsigned int  sig_act_channel_4  : 1;  /* bit[19]: GD0的sig_act_channel_4 value */
        unsigned int  sig_act_channel_5  : 1;  /* bit[20]: GD0的sig_act_channel_5 value */
        unsigned int  sig_act_channel_6  : 1;  /* bit[21]: GD0的sig_act_channel_6 value */
        unsigned int  sig_act_channel_7  : 1;  /* bit[22]: GD0的sig_act_channel_7 value */
        unsigned int  sig_act_channel_8  : 1;  /* bit[23]: GD0的sig_act_channel_8 value */
        unsigned int  sig_act_channel_9  : 1;  /* bit[24]: GD0的sig_act_channel_9 value */
        unsigned int  sig_act_channel_10 : 1;  /* bit[25]: GD0的sig_act_channel_10 value */
        unsigned int  sig_act_channel_11 : 1;  /* bit[26]: GD0的sig_act_channel_11 value */
        unsigned int  sig_act_channel_12 : 1;  /* bit[27]: GD0的sig_act_channel_12 value */
        unsigned int  sig_act_channel_13 : 1;  /* bit[28]: GD0的sig_act_channel_13 value */
        unsigned int  sig_act_channel_14 : 1;  /* bit[29]: GD0的sig_act_channel_14 value */
        unsigned int  sig_act_channel_15 : 1;  /* bit[30]: GD0的sig_act_channel_15 value */
        unsigned int  sig_act_channel_16 : 1;  /* bit[31]: GD0的sig_act_channel_16 value */
    } reg;
} SOC_SLIMBUS_GD0MSGCS1_UNION;
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_1_START       (0)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_1_END         (0)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_2_START       (1)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_2_END         (1)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_3_START       (2)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_3_END         (2)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_4_START       (3)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_4_END         (3)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_5_START       (4)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_5_END         (4)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_6_START       (5)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_6_END         (5)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_7_START       (6)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_7_END         (6)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_8_START       (7)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_8_END         (7)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_9_START       (8)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_9_END         (8)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_10_START      (9)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_10_END        (9)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_11_START      (10)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_11_END        (10)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_12_START      (11)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_12_END        (11)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_13_START      (12)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_13_END        (12)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_14_START      (13)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_14_END        (13)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_15_START      (14)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_15_END        (14)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_16_START      (15)
#define SOC_SLIMBUS_GD0MSGCS1_sig_channel_16_END        (15)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_1_START   (16)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_1_END     (16)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_2_START   (17)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_2_END     (17)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_3_START   (18)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_3_END     (18)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_4_START   (19)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_4_END     (19)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_5_START   (20)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_5_END     (20)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_6_START   (21)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_6_END     (21)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_7_START   (22)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_7_END     (22)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_8_START   (23)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_8_END     (23)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_9_START   (24)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_9_END     (24)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_10_START  (25)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_10_END    (25)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_11_START  (26)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_11_END    (26)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_12_START  (27)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_12_END    (27)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_13_START  (28)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_13_END    (28)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_14_START  (29)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_14_END    (29)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_15_START  (30)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_15_END    (30)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_16_START  (31)
#define SOC_SLIMBUS_GD0MSGCS1_sig_act_channel_16_END    (31)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_GD0MSGCS2_UNION
 结构说明  : GD0MSGCS2 寄存器结构定义。地址偏移量:0x097C，初值:0x0000，宽度:32
 寄存器说明: GD0消息上下文保存寄存器2
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sig_gd_log_addr : 8;  /* bit[0-7]  : GD0的Logical Address value */
        unsigned int  receive_ala     : 1;  /* bit[8]    : GD0的receive ala bit value */
        unsigned int  arb_pri         : 3;  /* bit[9-11] : GD0的Arbitration Priority */
        unsigned int  curr_enu_state  : 2;  /* bit[12-13]: GD0的Current Enumeration State */
        unsigned int  reserved        : 18; /* bit[14-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_GD0MSGCS2_UNION;
#define SOC_SLIMBUS_GD0MSGCS2_sig_gd_log_addr_START  (0)
#define SOC_SLIMBUS_GD0MSGCS2_sig_gd_log_addr_END    (7)
#define SOC_SLIMBUS_GD0MSGCS2_receive_ala_START      (8)
#define SOC_SLIMBUS_GD0MSGCS2_receive_ala_END        (8)
#define SOC_SLIMBUS_GD0MSGCS2_arb_pri_START          (9)
#define SOC_SLIMBUS_GD0MSGCS2_arb_pri_END            (11)
#define SOC_SLIMBUS_GD0MSGCS2_curr_enu_state_START   (12)
#define SOC_SLIMBUS_GD0MSGCS2_curr_enu_state_END     (13)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_GD0PRTiCS1_UNION
 结构说明  : GD0PRTiCS1 寄存器结构定义。地址偏移量:0x0980+0x4*i，初值:0x0000，宽度:32
 寄存器说明: GD0端口i(0-15)上下文保存寄存器1
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sig_gd_pi_seg_length : 5;  /* bit[0-4]  : GD0的端口i(0-15)的Segment Length value；pi中i=0-15 */
        unsigned int  sig_gd_pi_dat_len    : 5;  /* bit[5-9]  : GD0的端口i(0-15)的Data Length value；pi中i=0-15 */
        unsigned int  sig_gd_pi_protocol   : 4;  /* bit[10-13]: GD0的端口i(0-15)的Protocol value；pi中i=0-15 */
        unsigned int  sig_gd_pi_src        : 1;  /* bit[14]   : GD0的端口i(0-15)的source bit；pi中i=0-15 */
        unsigned int  sig_gd_pi_sink       : 1;  /* bit[15]   : GD0的端口i(0-15)的sink bit；pi中i=0-15 */
        unsigned int  sig_gd_pi_src_sink   : 1;  /* bit[16]   : GD0的端口i(0-15)的source_sink bit；pi中i=0-15 */
        unsigned int  sig_gd_pi_rec_bound  : 1;  /* bit[17]   : GD0的端口i(0-15)的reconfiguration boundary bit；pi中i=0-15 */
        unsigned int  sig_gd_pi_ch         : 4;  /* bit[18-21]: GD0的端口i(0-15)的Channel number；pi中i=0-15 */
        unsigned int  sig_gd_pi_freq_lock  : 1;  /* bit[22]   : GD0的端口i(0-15)的Frequency lock bit；pi中i=0-15 */
        unsigned int  reserved             : 9;  /* bit[23-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_GD0PRTiCS1_UNION;
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_seg_length_START  (0)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_seg_length_END    (4)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_dat_len_START     (5)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_dat_len_END       (9)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_protocol_START    (10)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_protocol_END      (13)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_src_START         (14)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_src_END           (14)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_sink_START        (15)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_sink_END          (15)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_src_sink_START    (16)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_src_sink_END      (16)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_rec_bound_START   (17)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_rec_bound_END     (17)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_ch_START          (18)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_ch_END            (21)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_freq_lock_START   (22)
#define SOC_SLIMBUS_GD0PRTiCS1_sig_gd_pi_freq_lock_END     (22)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_CHCONSAVi_UNION
 结构说明  : CHCONSAVi 寄存器结构定义。地址偏移量:0x0A00+0x8*(i-1)，初值:0x0000，宽度:32
 寄存器说明: Channel i(1-16)上下文保存寄存器2
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sig_slim_x_fl_seg_len_chi : 5;  /* bit[0-4]  : Channel i的Segment length value，其中chi，i=1-16，比如ch1 */
        unsigned int  sig_slim_x_fl_seg_off_chi : 11; /* bit[5-15] : Channel i的Segment Interval value，其中chi，i=1-16，比如ch1 */
        unsigned int  sig_slim_x_fl_seg_int_chi : 11; /* bit[16-26]: Channel i的Segment Offset value，其中chi，i=1-16，比如ch1 */
        unsigned int  reserved                  : 5;  /* bit[27-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_CHCONSAVi_UNION;
#define SOC_SLIMBUS_CHCONSAVi_sig_slim_x_fl_seg_len_chi_START  (0)
#define SOC_SLIMBUS_CHCONSAVi_sig_slim_x_fl_seg_len_chi_END    (4)
#define SOC_SLIMBUS_CHCONSAVi_sig_slim_x_fl_seg_off_chi_START  (5)
#define SOC_SLIMBUS_CHCONSAVi_sig_slim_x_fl_seg_off_chi_END    (15)
#define SOC_SLIMBUS_CHCONSAVi_sig_slim_x_fl_seg_int_chi_START  (16)
#define SOC_SLIMBUS_CHCONSAVi_sig_slim_x_fl_seg_int_chi_END    (26)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_GD0PRTiCS2_UNION
 结构说明  : GD0PRTiCS2 寄存器结构定义。地址偏移量:0x0A04+0x8*i，初值:0x0000，宽度:32
 寄存器说明: GD0端口i(0-15)上下文保存寄存器2
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sig_gd_presence_rate_d0_pi : 7;  /* bit[0-6] : Channel (i+1)的Presence Rate value，其中pi，i=0-15，比如p0，对应为channel 1。 */
        unsigned int  reserved                   : 25; /* bit[7-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_GD0PRTiCS2_UNION;
#define SOC_SLIMBUS_GD0PRTiCS2_sig_gd_presence_rate_d0_pi_START  (0)
#define SOC_SLIMBUS_GD0PRTiCS2_sig_gd_presence_rate_d0_pi_END    (6)


/*****************************************************************************
 结构名    : SOC_SLIMBUS_CHMAPCSi_UNION
 结构说明  : CHMAPCSi 寄存器结构定义。地址偏移量:0x0A80+0x4*(i-1)，初值:0x0000，宽度:32
 寄存器说明: Channel映射上下文保存寄存器i(1-16)
*****************************************************************************/
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  channel_number  : 8;  /* bit[0-7] : Channel number */
        unsigned int  entry_valid_reg : 1;  /* bit[8]   : entry valid bit */
        unsigned int  reserved        : 23; /* bit[9-31]: Reserved for future use */
    } reg;
} SOC_SLIMBUS_CHMAPCSi_UNION;
#define SOC_SLIMBUS_CHMAPCSi_channel_number_START   (0)
#define SOC_SLIMBUS_CHMAPCSi_channel_number_END     (7)
#define SOC_SLIMBUS_CHMAPCSi_entry_valid_reg_START  (8)
#define SOC_SLIMBUS_CHMAPCSi_entry_valid_reg_END    (8)






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

#endif /* end of soc_slimbus_interface.h */
