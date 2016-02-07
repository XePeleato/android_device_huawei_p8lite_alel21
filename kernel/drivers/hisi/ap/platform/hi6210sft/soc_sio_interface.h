

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_SIO_INTERFACE_H__
#define __SOC_SIO_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) SIO
 ****************************************************************************/
/* 寄存器说明：版本寄存器，用来记录SIO的版本号和进行SIO的自测试。
   位域定义UNION结构:  SOC_SIO_VERSION_UNION */
#define SOC_SIO_VERSION_ADDR(base)                    ((base) + (0x03C))

/* 寄存器说明：模式寄存器，用来对SIO工作的基本模式进行选择：
            (I)主模式下，CRG送时钟和同步信号给CODEC，同时给SIO。
            (I)从模式下，时钟和同步信号由外部CODEC送给SIO。
            I2S或PCM的主从模式选择由系统控制寄存器SC_PERICLK_CTRL0设定。
   位域定义UNION结构:  SOC_SIO_MODE_UNION */
#define SOC_SIO_MODE_ADDR(base)                       ((base) + (0x040))

/* 寄存器说明：SIO的中断状态指示寄存器。
            对于接收中断，当接收FIFO的数据深度大于FIFO阈值时，会一直把高电平锁存到中断状态寄存器中，一直产生中断（即使CPU清一次中断，但中断状态寄存器会在下一个时钟周期再次置位）。因此，建议CPU的处理步骤为：
            (S)向SIO_CT_CLR[intr_en]写1，关闭全局中断使能。
            (S)读中断状态寄存器SIO_INTSTATUS。
            (S)根据中断源进行相应处理。
            (S)向SIO_INTCLR的相应位写1，清除中断。
            (S)向写寄存器SIO_CT_SET[intr_en]写1，打开全局中断使能。
            (SE)
            发送中断的产生方式与接收中断产生方式相同，因此对于发送中断的处理，建议也采用相同的方式。
            本寄存器是原始中断状态寄存器。在相应中断位屏蔽的情况下，中断条件满足时，对应中断状态位仍然会置位，但不会触发中断。
   位域定义UNION结构:  SOC_SIO_INTSTATUS_UNION */
#define SOC_SIO_INTSTATUS_ADDR(base)                  ((base) + (0x044))

/* 寄存器说明：中断清除寄存器，可以按位清除。Bit值为0表示不清除中断，为1表示清除中断。
   位域定义UNION结构:  SOC_SIO_INTCLR_UNION */
#define SOC_SIO_INTCLR_ADDR(base)                     ((base) + (0x048))

/* 寄存器说明：I2S模式下的左声道数据发送寄存器。
            向寄存器写数据时，有效数据需放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
   位域定义UNION结构:  SOC_SIO_I2S_LEFT_XD_UNION */
#define SOC_SIO_I2S_LEFT_XD_ADDR(base)                ((base) + (0x04C))

/* 寄存器说明：I2S模式下的右声道数据发送寄存器。PCM数据发送寄存器与I2S右声道数据发送寄存器是复用的。
            向寄存器写数据时，有效数据需放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
   位域定义UNION结构:  SOC_SIO_I2S_RIGHT_XD_UNION */
#define SOC_SIO_I2S_RIGHT_XD_ADDR(base)               ((base) + (0x050))

/* 寄存器说明：PCM模式下的数据发送寄存器。PCM数据发送寄存器与I2S右声道数据发送寄存器是复用的。
            向寄存器写有效数据时，有效数据需放在寄存器的低bit区域。例如，8bit宽度时，bit[7:0]为有效数据，bit[31:8]为无效数据。16bit宽度时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
   位域定义UNION结构:  SOC_SIO_PCM_XD_UNION */
#define SOC_SIO_PCM_XD_ADDR(base)                     ((base) + (0x050))

/* 寄存器说明：I2S左声道数据接收寄存器。
            SIO模块把接收到的有效数据放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
   位域定义UNION结构:  SOC_SIO_I2S_LEFT_RD_UNION */
#define SOC_SIO_I2S_LEFT_RD_ADDR(base)                ((base) + (0x054))

/* 寄存器说明：I2S右声道数据接收寄存器。
            SIO模块把接收到的有效数据放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
   位域定义UNION结构:  SOC_SIO_I2S_RIGHT_RD_UNION */
#define SOC_SIO_I2S_RIGHT_RD_ADDR(base)               ((base) + (0x058))

/* 寄存器说明：PCM接收数据寄存器。它与I2S右声道接收寄存器是复用的。
            SIO模块把接收到的有效数据放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
   位域定义UNION结构:  SOC_SIO_PCM_RD_UNION */
#define SOC_SIO_PCM_RD_ADDR(base)                     ((base) + (0x058))

/* 寄存器说明：设置寄存器。
            为了能够方便的对SIO控制寄存器进行位操作，在SIO中，为SIO控制寄存器设置了2个地址：0x05C（SIO_CT_SET）和0x060（SIO_CT_CLR），其中：
            0x05C为设置寄存器地址，当向0x05C寄存器中相应位写入1时，对应位被设为1，写0无效；该寄存器属性为读写。
   位域定义UNION结构:  SOC_SIO_CT_SET_UNION */
#define SOC_SIO_CT_SET_ADDR(base)                     ((base) + (0x05C))

/* 寄存器说明：清除寄存器。
            为了能够方便的对SIO控制寄存器进行位操作，在SIO中，为SIO控制寄存器设置了2个地址：0x05C（SIO_CT_SET）和0x060（SIO_CT_CLR），其中：
            0x060为清除寄存器地址，当向寄存器中相应位写入1时，对应位被清除，写0无效。该寄存器属性为只写。
   位域定义UNION结构:  SOC_SIO_CT_CLR_UNION */
#define SOC_SIO_CT_CLR_ADDR(base)                     ((base) + (0x060))

/* 寄存器说明：SIO接收状态寄存器。
   位域定义UNION结构:  SOC_SIO_RX_STA_UNION */
#define SOC_SIO_RX_STA_ADDR(base)                     ((base) + (0x068))

/* 寄存器说明：SIO发送状态寄存器。
   位域定义UNION结构:  SOC_SIO_TX_STA_UNION */
#define SOC_SIO_TX_STA_ADDR(base)                     ((base) + (0x06C))

/* 寄存器说明：I2S/PCM模式下的数据宽度配置寄存器。
   位域定义UNION结构:  SOC_SIO_DATA_WIDTH_SET_UNION */
#define SOC_SIO_DATA_WIDTH_SET_ADDR(base)             ((base) + (0x078))

/* 寄存器说明：I2S左右声道起始位置配置控制寄存器。
            在I2S模式下，左右声道数据操作地址合并使能后，控制起始访问是从左声道开始还是从右声道开始。
   位域定义UNION结构:  SOC_SIO_I2S_START_POS_UNION */
#define SOC_SIO_I2S_START_POS_ADDR(base)              ((base) + (0x07C))

/* 寄存器说明：I2S左右声道操作当前位置状态寄存器。
            在I2S模式下，左右声道数据操作地址合并使能后，指示下一次访问寄存器是从左声道开始还是从右声道开始。
   位域定义UNION结构:  SOC_SIO_I2S_POS_FLAG_UNION */
#define SOC_SIO_I2S_POS_FLAG_ADDR(base)               ((base) + (0x080))

/* 寄存器说明：高位数据符号扩展使能寄存器。该标志只对接收数据有效，对发送数据无效。PCM模式和I2S模式下接收到的数据都支持符号扩展。
            在接收有效数据位宽为8/16/18/20/24 bit时，如果该标志使能，把接收到的数据转换为32bit数据时，把32bit数据的高位无效比特设置为接收数据最高有效bit对应的值，然后再写入接收FIFO。
            以16bit位宽为例：
            if (data_rx[15] == 1 )
            data_rx[31:16]=0xffff;
            else
            data_rx[31:16]=0x0000;
   位域定义UNION结构:  SOC_SIO_SIGNED_EXT_UNION */
#define SOC_SIO_SIGNED_EXT_ADDR(base)                 ((base) + (0x084))

/* 寄存器说明：I2S模式左右声道数据操作地址合并使能寄存器。
            在I2S模式下，用DMA方式读写SIO的FIFO数据时，因为左右声道数据地址不同，需要CPU不断配置DMA操作的地址，导致CPU效率低。为了提高CPU的效率，提供左右声道数据的读写的统一地址使能控制。
            使能情况下，读左右声道数据统一使用SIO_I2S_DUAL_RX_CHN寄存器，写左右声道数据统一使用SIO_I2S_DUAL_TX_CHN寄存器。
   位域定义UNION结构:  SOC_SIO_I2S_POS_MERGE_EN_UNION */
#define SOC_SIO_I2S_POS_MERGE_EN_ADDR(base)           ((base) + (0x088))

/* 寄存器说明：中断屏蔽寄存器。
   位域定义UNION结构:  SOC_SIO_INTMASK_UNION */
#define SOC_SIO_INTMASK_ADDR(base)                    ((base) + (0x08C))

/* 寄存器说明：读取接收数据寄存器。I2S左右声道操作地址合并使能后，读取接收数据。
   位域定义UNION结构:  SOC_SIO_I2S_DUAL_RX_CHN_UNION */
#define SOC_SIO_I2S_DUAL_RX_CHN_ADDR(base)            ((base) + (0x0A0))

/* 寄存器说明：写发送数据寄存器。I2S左右声道操作地址合并使能后，写发送数据。
   位域定义UNION结构:  SOC_SIO_I2S_DUAL_TX_CHN_UNION */
#define SOC_SIO_I2S_DUAL_TX_CHN_ADDR(base)            ((base) + (0x0C0))





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
                     (1/1) SIO
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_SIO_VERSION_UNION
 结构说明  : VERSION 寄存器结构定义。地址偏移量:0x03C，初值:0x00000013，宽度:32
 寄存器说明: 版本寄存器，用来记录SIO的版本号和进行SIO的自测试。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vesion   : 8;  /* bit[0-7] : SIO的版本号。 */
        unsigned int  sio_loop : 1;  /* bit[8]   : 0：正常模式；
                                                   1：SIO发送和接收数据环回模式，用于SIO的自测试。在该模式下，在SIO的对外接口处，SIO接收串行数据线与SIO发送串行数据线直接相连。 */
        unsigned int  reserved : 23; /* bit[9-31]: 保留。 */
    } reg;
} SOC_SIO_VERSION_UNION;
#endif
#define SOC_SIO_VERSION_vesion_START    (0)
#define SOC_SIO_VERSION_vesion_END      (7)
#define SOC_SIO_VERSION_sio_loop_START  (8)
#define SOC_SIO_VERSION_sio_loop_END    (8)


/*****************************************************************************
 结构名    : SOC_SIO_MODE_UNION
 结构说明  : MODE 寄存器结构定义。地址偏移量:0x040，初值:0x00000000，宽度:32
 寄存器说明: 模式寄存器，用来对SIO工作的基本模式进行选择：
            (I)主模式下，CRG送时钟和同步信号给CODEC，同时给SIO。
            (I)从模式下，时钟和同步信号由外部CODEC送给SIO。
            I2S或PCM的主从模式选择由系统控制寄存器SC_PERICLK_CTRL0设定。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sio_mode   : 1;  /* bit[0]   : PCM/I2S模式选择。
                                                     0：I2S模式；
                                                     &#13;1：PCM模式。 */
        unsigned int  pcm_mode   : 1;  /* bit[1]   : PCM时序模式。
                                                     0：标准模式；
                                                     &#13;1：自定义模式。 */
        unsigned int  reserved_0 : 1;  /* bit[2]   : 保留，必须配置为0 */
        unsigned int  ext_rec_en : 1;  /* bit[3]   : 标准模式下，I2S接收两个声道的数据，即左声道和右声道。PCM只接收一个声道的数据。
                                                     多路接收模式下，对于I2S或PCM，接收通道数可以配置。该模式下，通道的数据宽度必须为8bit或16bit。
                                                     0：标准的I2S或PCM接收模式；
                                                     1：扩展的I2S或PCM多路接收模式。 */
        unsigned int  chn_num    : 2;  /* bit[4-5] : 多路接收的通道数选择。
                                                     00：2 chn；
                                                     01：4 chn；
                                                     10：8 chn；
                                                     11：16 chn。 */
        unsigned int  clk_edge   : 1;  /* bit[6]   : PCM多路接收模式下，采样数据时钟边沿选择。
                                                     0：下降沿有效；
                                                     1：上升沿有效。 */
        unsigned int  reserved_1 : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_SIO_MODE_UNION;
#endif
#define SOC_SIO_MODE_sio_mode_START    (0)
#define SOC_SIO_MODE_sio_mode_END      (0)
#define SOC_SIO_MODE_pcm_mode_START    (1)
#define SOC_SIO_MODE_pcm_mode_END      (1)
#define SOC_SIO_MODE_ext_rec_en_START  (3)
#define SOC_SIO_MODE_ext_rec_en_END    (3)
#define SOC_SIO_MODE_chn_num_START     (4)
#define SOC_SIO_MODE_chn_num_END       (5)
#define SOC_SIO_MODE_clk_edge_START    (6)
#define SOC_SIO_MODE_clk_edge_END      (6)


/*****************************************************************************
 结构名    : SOC_SIO_INTSTATUS_UNION
 结构说明  : INTSTATUS 寄存器结构定义。地址偏移量:0x044，初值:0x00000000，宽度:32
 寄存器说明: SIO的中断状态指示寄存器。
            对于接收中断，当接收FIFO的数据深度大于FIFO阈值时，会一直把高电平锁存到中断状态寄存器中，一直产生中断（即使CPU清一次中断，但中断状态寄存器会在下一个时钟周期再次置位）。因此，建议CPU的处理步骤为：
            (S)向SIO_CT_CLR[intr_en]写1，关闭全局中断使能。
            (S)读中断状态寄存器SIO_INTSTATUS。
            (S)根据中断源进行相应处理。
            (S)向SIO_INTCLR的相应位写1，清除中断。
            (S)向写寄存器SIO_CT_SET[intr_en]写1，打开全局中断使能。
            (SE)
            发送中断的产生方式与接收中断产生方式相同，因此对于发送中断的处理，建议也采用相同的方式。
            本寄存器是原始中断状态寄存器。在相应中断位屏蔽的情况下，中断条件满足时，对应中断状态位仍然会置位，但不会触发中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rx_intr             : 1;  /* bit[0]   : 接收FIFO高于阈值中断状态。
                                                              0：未产生中断；
                                                              &#13;1：已产生中断。 */
        unsigned int  tx_intr             : 1;  /* bit[1]   : 发送FIFO低于阈值中断状态。
                                                              0：未产生中断；
                                                              &#13;1：已产生中断。 */
        unsigned int  rx_right_fifo_over  : 1;  /* bit[2]   : I2S模式下，右声道接收FIFO上溢中断状态。PCM模式下，PCM接收FIFO上溢标志。
                                                              0：未产生中断；
                                                              1：已产生中断。 */
        unsigned int  rx_left_fifo_over   : 1;  /* bit[3]   : I2S模式下，左声道接收FIFO上溢中断状态。PCM模式下无效。
                                                              0：未产生中断；
                                                              &#13;1：已产生中断。 */
        unsigned int  tx_right_fifo_under : 1;  /* bit[4]   : I2S模式下，右声道发送FIFO下溢中断状态。PCM模式下，PCM发送FIFO下溢标志。
                                                              0：未产生中断；
                                                              &#13;1：已产生中断。 */
        unsigned int  tx_left_fifo_under  : 1;  /* bit[5]   : I2S模式下，左声道发送FIFO下溢中断状态。PCM模式下无效。
                                                              0：未产生中断；
                                                              &#13;1：已产生中断。 */
        unsigned int  reserved            : 26; /* bit[6-31]: 保留。 */
    } reg;
} SOC_SIO_INTSTATUS_UNION;
#endif
#define SOC_SIO_INTSTATUS_rx_intr_START              (0)
#define SOC_SIO_INTSTATUS_rx_intr_END                (0)
#define SOC_SIO_INTSTATUS_tx_intr_START              (1)
#define SOC_SIO_INTSTATUS_tx_intr_END                (1)
#define SOC_SIO_INTSTATUS_rx_right_fifo_over_START   (2)
#define SOC_SIO_INTSTATUS_rx_right_fifo_over_END     (2)
#define SOC_SIO_INTSTATUS_rx_left_fifo_over_START    (3)
#define SOC_SIO_INTSTATUS_rx_left_fifo_over_END      (3)
#define SOC_SIO_INTSTATUS_tx_right_fifo_under_START  (4)
#define SOC_SIO_INTSTATUS_tx_right_fifo_under_END    (4)
#define SOC_SIO_INTSTATUS_tx_left_fifo_under_START   (5)
#define SOC_SIO_INTSTATUS_tx_left_fifo_under_END     (5)


/*****************************************************************************
 结构名    : SOC_SIO_INTCLR_UNION
 结构说明  : INTCLR 寄存器结构定义。地址偏移量:0x048，初值:0x00000000，宽度:32
 寄存器说明: 中断清除寄存器，可以按位清除。Bit值为0表示不清除中断，为1表示清除中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rx_intr             : 1;  /* bit[0]   : 接收FIFO高于阈值中断清除。 */
        unsigned int  tx_intr             : 1;  /* bit[1]   : 发送FIFO低于阈值中断清除。 */
        unsigned int  rx_right_fifo_over  : 1;  /* bit[2]   : I2S模式下，右声道接收FIFO上溢中断清除。
                                                              PCM模式下，PCM接收FIFO上溢中断清除。 */
        unsigned int  rx_left_fifo_over   : 1;  /* bit[3]   : I2S模式下，左声道接收FIFO上溢中断清除。
                                                              PCM模式下无效。 */
        unsigned int  tx_right_fifo_under : 1;  /* bit[4]   : I2S模式下，右声道发送FIFO下溢中断清除。
                                                              PCM模式下，PCM发送FIFO下溢中断清除。 */
        unsigned int  tx_left_fifo_under  : 1;  /* bit[5]   : I2S模式下，左声道发送FIFO下溢中断清除。
                                                              PCM模式下无效。 */
        unsigned int  reserved            : 26; /* bit[6-31]: 保留。 */
    } reg;
} SOC_SIO_INTCLR_UNION;
#endif
#define SOC_SIO_INTCLR_rx_intr_START              (0)
#define SOC_SIO_INTCLR_rx_intr_END                (0)
#define SOC_SIO_INTCLR_tx_intr_START              (1)
#define SOC_SIO_INTCLR_tx_intr_END                (1)
#define SOC_SIO_INTCLR_rx_right_fifo_over_START   (2)
#define SOC_SIO_INTCLR_rx_right_fifo_over_END     (2)
#define SOC_SIO_INTCLR_rx_left_fifo_over_START    (3)
#define SOC_SIO_INTCLR_rx_left_fifo_over_END      (3)
#define SOC_SIO_INTCLR_tx_right_fifo_under_START  (4)
#define SOC_SIO_INTCLR_tx_right_fifo_under_END    (4)
#define SOC_SIO_INTCLR_tx_left_fifo_under_START   (5)
#define SOC_SIO_INTCLR_tx_left_fifo_under_END     (5)


/*****************************************************************************
 结构名    : SOC_SIO_I2S_LEFT_XD_UNION
 结构说明  : I2S_LEFT_XD 寄存器结构定义。地址偏移量:0x04C，初值:0x00000000，宽度:32
 寄存器说明: I2S模式下的左声道数据发送寄存器。
            向寄存器写数据时，有效数据需放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_left_data : 32; /* bit[0-31]: 左声道发送数据。 */
    } reg;
} SOC_SIO_I2S_LEFT_XD_UNION;
#endif
#define SOC_SIO_I2S_LEFT_XD_tx_left_data_START  (0)
#define SOC_SIO_I2S_LEFT_XD_tx_left_data_END    (31)


/*****************************************************************************
 结构名    : SOC_SIO_I2S_RIGHT_XD_UNION
 结构说明  : I2S_RIGHT_XD 寄存器结构定义。地址偏移量:0x050，初值:0x00000000，宽度:32
 寄存器说明: I2S模式下的右声道数据发送寄存器。PCM数据发送寄存器与I2S右声道数据发送寄存器是复用的。
            向寄存器写数据时，有效数据需放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_right_data : 32; /* bit[0-31]: 右声道发送数据。 */
    } reg;
} SOC_SIO_I2S_RIGHT_XD_UNION;
#endif
#define SOC_SIO_I2S_RIGHT_XD_tx_right_data_START  (0)
#define SOC_SIO_I2S_RIGHT_XD_tx_right_data_END    (31)


/*****************************************************************************
 结构名    : SOC_SIO_PCM_XD_UNION
 结构说明  : PCM_XD 寄存器结构定义。地址偏移量:0x050，初值:0x00000000，宽度:32
 寄存器说明: PCM模式下的数据发送寄存器。PCM数据发送寄存器与I2S右声道数据发送寄存器是复用的。
            向寄存器写有效数据时，有效数据需放在寄存器的低bit区域。例如，8bit宽度时，bit[7:0]为有效数据，bit[31:8]为无效数据。16bit宽度时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_data  : 16; /* bit[0-15] : PCM发送数据。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SIO_PCM_XD_UNION;
#endif
#define SOC_SIO_PCM_XD_tx_data_START   (0)
#define SOC_SIO_PCM_XD_tx_data_END     (15)


/*****************************************************************************
 结构名    : SOC_SIO_I2S_LEFT_RD_UNION
 结构说明  : I2S_LEFT_RD 寄存器结构定义。地址偏移量:0x054，初值:0x00000000，宽度:32
 寄存器说明: I2S左声道数据接收寄存器。
            SIO模块把接收到的有效数据放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rx_left_data : 32; /* bit[0-31]: I2S左声道接收数据。 */
    } reg;
} SOC_SIO_I2S_LEFT_RD_UNION;
#endif
#define SOC_SIO_I2S_LEFT_RD_rx_left_data_START  (0)
#define SOC_SIO_I2S_LEFT_RD_rx_left_data_END    (31)


/*****************************************************************************
 结构名    : SOC_SIO_I2S_RIGHT_RD_UNION
 结构说明  : I2S_RIGHT_RD 寄存器结构定义。地址偏移量:0x058，初值:0x00000000，宽度:32
 寄存器说明: I2S右声道数据接收寄存器。
            SIO模块把接收到的有效数据放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rx_right_data : 32; /* bit[0-31]: I2S右声道接收数据。 */
    } reg;
} SOC_SIO_I2S_RIGHT_RD_UNION;
#endif
#define SOC_SIO_I2S_RIGHT_RD_rx_right_data_START  (0)
#define SOC_SIO_I2S_RIGHT_RD_rx_right_data_END    (31)


/*****************************************************************************
 结构名    : SOC_SIO_PCM_RD_UNION
 结构说明  : PCM_RD 寄存器结构定义。地址偏移量:0x058，初值:0x00000000，宽度:32
 寄存器说明: PCM接收数据寄存器。它与I2S右声道接收寄存器是复用的。
            SIO模块把接收到的有效数据放在寄存器的低bit区域。例如，数据宽度为8bit时，bit[7:0]为有效数据，bit[31:8]为无效数据；数据宽度为16bit时，bit[15:0]为有效数据，bit[31:16]为无效数据。超出有效数据宽度的bit位由SIO模块自动置0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rx__data : 16; /* bit[0-15] : PCM接收数据。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SIO_PCM_RD_UNION;
#endif
#define SOC_SIO_PCM_RD_rx__data_START  (0)
#define SOC_SIO_PCM_RD_rx__data_END    (15)


/*****************************************************************************
 结构名    : SOC_SIO_CT_SET_UNION
 结构说明  : CT_SET 寄存器结构定义。地址偏移量:0x05C，初值:0x00008000，宽度:32
 寄存器说明: 设置寄存器。
            为了能够方便的对SIO控制寄存器进行位操作，在SIO中，为SIO控制寄存器设置了2个地址：0x05C（SIO_CT_SET）和0x060（SIO_CT_CLR），其中：
            0x05C为设置寄存器地址，当向0x05C寄存器中相应位写入1时，对应位被设为1，写0无效；该寄存器属性为读写。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_fifo_threshold : 4;  /* bit[0-3]  : 发送FIFO阈值。
                                                             当tx_right_depth < (tx_fifo_threshold + 1)时，报发送中断和DMA请求。 */
        unsigned int  rx_fifo_threshold : 4;  /* bit[4-7]  : 接收FIFO阈值。
                                                             当rx_right_depth 3(rx_fifo_threshold + 1)时，报接收中断和DMA请求。 */
        unsigned int  tx_data_merge_en  : 1;  /* bit[8]    : 数据发送合并使能，只在I2S 模式下且数据位宽为16bit时有效。
                                                             0：不使能；
                                                             1：使能。
                                                             &#13;该位为1时，左右声道数据拼成一个32bit数据在FIFO中进行存储，左声道16bit数据占据高16位，右声道16bit数据占据低16位。这样可以提高FIFO的利用率，以提供更长时间的缓冲能力。&#13;在这种情况下，CPU向发送FIFO中写入数据时，按照如下顺序：向左声道FIFO写入一个32bit数据（左右声道各16bit合并成的一个32bit数据），再向右声道FIFO写入一个32bit数据，如此反复。 */
        unsigned int  rx_data_merge_en  : 1;  /* bit[9]    : 数据接收合并使能，只在I2S模式下且数据位宽为16bit时有效。
                                                             0：不使能；
                                                             &#13;1：使能。
                                                             该位为1时，左右声道数据拼成一个32bit数据在FIFO中进行存储，左声道16bit数据占据高16位，右声道16bit数据占据低16位。这样可以提高FIFO的利用率，以提供更长时间的缓冲能力。&#13;CPU从接收FIFO中读数据时，按照如下顺序：从左声道FIFO读出一个32bit数据（左右声道各16bit合并成的一个32bit数据），再从右声道FIFO读出一个32bit数据，如此反复。 */
        unsigned int  tx_fifo_disable   : 1;  /* bit[10]   : 发送FIFO禁止。
                                                             0：使能；
                                                             &#13;1：禁止。 */
        unsigned int  rx_fifo_disable   : 1;  /* bit[11]   : 接收FIFO禁止。
                                                             0：使能；
                                                             &#13;1：禁止。 */
        unsigned int  tx_enable         : 1;  /* bit[12]   : 发送通道使能。
                                                             0：禁止；
                                                             &#13;1：使能。 */
        unsigned int  rx_enable         : 1;  /* bit[13]   : 接收通道使能。
                                                             0：禁止；
                                                             &#13;1：使能。 */
        unsigned int  intr_en           : 1;  /* bit[14]   : 中断全局使能。
                                                             0：禁止；
                                                             &#13;1：使能。 */
        unsigned int  rst_n             : 1;  /* bit[15]   : I2S/PCM通道复位，低电平有效。
                                                             它对I2S/PCM接收和发送模块（包括FIFO）进行复位，因此发送和接收FIFO状态寄存器会变为0。但不会复位CPU接口寄存器模块。 */
        unsigned int  reserved          : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SIO_CT_SET_UNION;
#endif
#define SOC_SIO_CT_SET_tx_fifo_threshold_START  (0)
#define SOC_SIO_CT_SET_tx_fifo_threshold_END    (3)
#define SOC_SIO_CT_SET_rx_fifo_threshold_START  (4)
#define SOC_SIO_CT_SET_rx_fifo_threshold_END    (7)
#define SOC_SIO_CT_SET_tx_data_merge_en_START   (8)
#define SOC_SIO_CT_SET_tx_data_merge_en_END     (8)
#define SOC_SIO_CT_SET_rx_data_merge_en_START   (9)
#define SOC_SIO_CT_SET_rx_data_merge_en_END     (9)
#define SOC_SIO_CT_SET_tx_fifo_disable_START    (10)
#define SOC_SIO_CT_SET_tx_fifo_disable_END      (10)
#define SOC_SIO_CT_SET_rx_fifo_disable_START    (11)
#define SOC_SIO_CT_SET_rx_fifo_disable_END      (11)
#define SOC_SIO_CT_SET_tx_enable_START          (12)
#define SOC_SIO_CT_SET_tx_enable_END            (12)
#define SOC_SIO_CT_SET_rx_enable_START          (13)
#define SOC_SIO_CT_SET_rx_enable_END            (13)
#define SOC_SIO_CT_SET_intr_en_START            (14)
#define SOC_SIO_CT_SET_intr_en_END              (14)
#define SOC_SIO_CT_SET_rst_n_START              (15)
#define SOC_SIO_CT_SET_rst_n_END                (15)


/*****************************************************************************
 结构名    : SOC_SIO_CT_CLR_UNION
 结构说明  : CT_CLR 寄存器结构定义。地址偏移量:0x060，初值:0x00008000，宽度:32
 寄存器说明: 清除寄存器。
            为了能够方便的对SIO控制寄存器进行位操作，在SIO中，为SIO控制寄存器设置了2个地址：0x05C（SIO_CT_SET）和0x060（SIO_CT_CLR），其中：
            0x060为清除寄存器地址，当向寄存器中相应位写入1时，对应位被清除，写0无效。该寄存器属性为只写。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_fifo_threshold : 4;  /* bit[0-3]  : 发送FIFO阈值。
                                                             当tx_right_depth < (tx_fifo_threshold + 1)时，报发送中断和DMA请求。 */
        unsigned int  rx_fifo_threshold : 4;  /* bit[4-7]  : 接收FIFO阈值。
                                                             当rx_right_depth 3(rx_fifo_threshold + 1)时，报接收中断和DMA请求。 */
        unsigned int  tx_data_merge_en  : 1;  /* bit[8]    : 数据发送合并使能，只在I2S 模式下且数据位宽为16bit时有效。
                                                             0：不使能；
                                                             1：使能。
                                                             &#13;该位为1时，左右声道数据拼成一个32bit数据在FIFO中进行存储，左声道16bit数据占据高16位，右声道16bit数据占据低16位。这样可以提高FIFO的利用率，以提供更长时间的缓冲能力。&#13;在这种情况下，CPU向发送FIFO中写入数据时，按照如下顺序：向左声道FIFO写入一个32bit数据（左右声道各16bit合并成的一个32bit数据），再向右声道FIFO写入一个32bit数据，如此反复。 */
        unsigned int  rx_data_merge_en  : 1;  /* bit[9]    : 数据接收合并使能，只在I2S模式下且数据位宽为16bit时有效。
                                                             0：不使能；
                                                             1：使能。
                                                             该位为1时，左右声道数据拼成一个32bit数据在FIFO中进行存储，左声道16bit数据占据高16位，右声道16bit数据占据低16位。这样可以提高FIFO的利用率，以提供更长时间的缓冲能力。&#13;CPU从接收FIFO中读数据时，按照如下顺序：从左声道FIFO读出一个32bit数据（左右声道各16bit合并成的一个32bit数据），再从右声道FIFO读出一个32bit数据，如此反复。 */
        unsigned int  tx_fifo_disable   : 1;  /* bit[10]   : 发送FIFO禁止。
                                                             0：使能；
                                                             &#13;1：禁止。 */
        unsigned int  rx_fifo_disable   : 1;  /* bit[11]   : 接收FIFO禁止。
                                                             0：使能；
                                                             1：禁止。 */
        unsigned int  tx_enable         : 1;  /* bit[12]   : 发送通道使能。
                                                             0：禁止；
                                                             &#13;1：使能。 */
        unsigned int  rx_enable         : 1;  /* bit[13]   : 接收通道使能。
                                                             0：禁止；
                                                             &#13;1：使能。 */
        unsigned int  intr_en           : 1;  /* bit[14]   : 中断全局使能。
                                                             0：禁止；
                                                             &#13;1：使能。 */
        unsigned int  rst_n             : 1;  /* bit[15]   : I2S/PCM通道复位，低电平有效。
                                                             它对I2S/PCM接收和发送模块（包括FIFO）进行复位，因此发送和接收FIFO状态寄存器会变为0。但不会复位CPU接口寄存器模块。 */
        unsigned int  reserved          : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SIO_CT_CLR_UNION;
#endif
#define SOC_SIO_CT_CLR_tx_fifo_threshold_START  (0)
#define SOC_SIO_CT_CLR_tx_fifo_threshold_END    (3)
#define SOC_SIO_CT_CLR_rx_fifo_threshold_START  (4)
#define SOC_SIO_CT_CLR_rx_fifo_threshold_END    (7)
#define SOC_SIO_CT_CLR_tx_data_merge_en_START   (8)
#define SOC_SIO_CT_CLR_tx_data_merge_en_END     (8)
#define SOC_SIO_CT_CLR_rx_data_merge_en_START   (9)
#define SOC_SIO_CT_CLR_rx_data_merge_en_END     (9)
#define SOC_SIO_CT_CLR_tx_fifo_disable_START    (10)
#define SOC_SIO_CT_CLR_tx_fifo_disable_END      (10)
#define SOC_SIO_CT_CLR_rx_fifo_disable_START    (11)
#define SOC_SIO_CT_CLR_rx_fifo_disable_END      (11)
#define SOC_SIO_CT_CLR_tx_enable_START          (12)
#define SOC_SIO_CT_CLR_tx_enable_END            (12)
#define SOC_SIO_CT_CLR_rx_enable_START          (13)
#define SOC_SIO_CT_CLR_rx_enable_END            (13)
#define SOC_SIO_CT_CLR_intr_en_START            (14)
#define SOC_SIO_CT_CLR_intr_en_END              (14)
#define SOC_SIO_CT_CLR_rst_n_START              (15)
#define SOC_SIO_CT_CLR_rst_n_END                (15)


/*****************************************************************************
 结构名    : SOC_SIO_RX_STA_UNION
 结构说明  : RX_STA 寄存器结构定义。地址偏移量:0x068，初值:0x00000000，宽度:32
 寄存器说明: SIO接收状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rx_right_depth : 5;  /* bit[0-4]  : I2S模式下，为右声道接收FIFO深度指示。
                                                          PCM模式下，为PCM接收FIFO深度指示。 */
        unsigned int  rx_left_depth  : 5;  /* bit[5-9]  : 左声道接收FIFO深度指示。
                                                          只在I2S模式下有效。 */
        unsigned int  reserved       : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_SIO_RX_STA_UNION;
#endif
#define SOC_SIO_RX_STA_rx_right_depth_START  (0)
#define SOC_SIO_RX_STA_rx_right_depth_END    (4)
#define SOC_SIO_RX_STA_rx_left_depth_START   (5)
#define SOC_SIO_RX_STA_rx_left_depth_END     (9)


/*****************************************************************************
 结构名    : SOC_SIO_TX_STA_UNION
 结构说明  : TX_STA 寄存器结构定义。地址偏移量:0x06C，初值:0x00000000，宽度:32
 寄存器说明: SIO发送状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_right_depth : 5;  /* bit[0-4]  : I2S模式下，为右声道发送FIFO深度指示。
                                                          PCM模式下，为PCM发送FIFO深度指示。 */
        unsigned int  tx_left_depth  : 5;  /* bit[5-9]  : 左声道发送FIFO深度指示。
                                                          只在I2S模式下有效。 */
        unsigned int  reserved       : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_SIO_TX_STA_UNION;
#endif
#define SOC_SIO_TX_STA_tx_right_depth_START  (0)
#define SOC_SIO_TX_STA_tx_right_depth_END    (4)
#define SOC_SIO_TX_STA_tx_left_depth_START   (5)
#define SOC_SIO_TX_STA_tx_left_depth_END     (9)


/*****************************************************************************
 结构名    : SOC_SIO_DATA_WIDTH_SET_UNION
 结构说明  : DATA_WIDTH_SET 寄存器结构定义。地址偏移量:0x078，初值:0x00000009，宽度:32
 寄存器说明: I2S/PCM模式下的数据宽度配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_mode  : 3;  /* bit[0-2] : 发送数据长度配置位。
                                                   对于I2S模式
                                                   000：保留；
                                                   001：16bit；
                                                   010：18bit；
                                                   011：20bit；
                                                   100：24bit；
                                                   101：32bit；
                                                   110～111：保留。
                                                   000：8bit
                                                   001：16bit；
                                                   010：32bit；
                                                   011：64bit；
                                                   100：128bit；
                                                   101～111：保留 */
        unsigned int  rx_mode  : 3;  /* bit[3-5] : 接收数据长度配置位。
                                                   000：8bit；
                                                   &#13;001：16bit；
                                                   010：18bit；
                                                   &#13;011：20bit；
                                                   &#13;100：24bit；
                                                   &#13;101：32bit；
                                                   &#13;110～111：保留。
                                                   &#13;对于I2S模式，支持16/18/20/24/32 bit；&#13;对于PCM模式，支持8/16 bit。
                                                   对于多路接收，两种模式只支持8/16bit。 */
        unsigned int  reserved : 26; /* bit[6-31]: 保留。 */
    } reg;
} SOC_SIO_DATA_WIDTH_SET_UNION;
#endif
#define SOC_SIO_DATA_WIDTH_SET_tx_mode_START   (0)
#define SOC_SIO_DATA_WIDTH_SET_tx_mode_END     (2)
#define SOC_SIO_DATA_WIDTH_SET_rx_mode_START   (3)
#define SOC_SIO_DATA_WIDTH_SET_rx_mode_END     (5)


/*****************************************************************************
 结构名    : SOC_SIO_I2S_START_POS_UNION
 结构说明  : I2S_START_POS 寄存器结构定义。地址偏移量:0x07C，初值:0x00000000，宽度:32
 寄存器说明: I2S左右声道起始位置配置控制寄存器。
            在I2S模式下，左右声道数据操作地址合并使能后，控制起始访问是从左声道开始还是从右声道开始。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  start_pos_read  : 1;  /* bit[0]   : 读接收FIFO时：
                                                          0：从左声道开始访问；
                                                          &#13;1：从右声道开始访问。 */
        unsigned int  start_pos_write : 1;  /* bit[1]   : 写发送FIFO时：
                                                          0：从左声道开始访问；
                                                          &#13;1：从右声道开始访问。 */
        unsigned int  reserved        : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_SIO_I2S_START_POS_UNION;
#endif
#define SOC_SIO_I2S_START_POS_start_pos_read_START   (0)
#define SOC_SIO_I2S_START_POS_start_pos_read_END     (0)
#define SOC_SIO_I2S_START_POS_start_pos_write_START  (1)
#define SOC_SIO_I2S_START_POS_start_pos_write_END    (1)


/*****************************************************************************
 结构名    : SOC_SIO_I2S_POS_FLAG_UNION
 结构说明  : I2S_POS_FLAG 寄存器结构定义。地址偏移量:0x080，初值:0x00000000，宽度:32
 寄存器说明: I2S左右声道操作当前位置状态寄存器。
            在I2S模式下，左右声道数据操作地址合并使能后，指示下一次访问寄存器是从左声道开始还是从右声道开始。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  start_pos_read  : 1;  /* bit[0]   : 读接收FIFO时：
                                                          0：下一次从左声道开始访问；
                                                          &#13;1：下一次从右声道开始访问。 */
        unsigned int  start_pos_write : 1;  /* bit[1]   : 写发送FIFO时：
                                                          0：下一次从左声道开始访问；
                                                          &#13;1：下一次从右声道开始访问。 */
        unsigned int  reserved        : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_SIO_I2S_POS_FLAG_UNION;
#endif
#define SOC_SIO_I2S_POS_FLAG_start_pos_read_START   (0)
#define SOC_SIO_I2S_POS_FLAG_start_pos_read_END     (0)
#define SOC_SIO_I2S_POS_FLAG_start_pos_write_START  (1)
#define SOC_SIO_I2S_POS_FLAG_start_pos_write_END    (1)


/*****************************************************************************
 结构名    : SOC_SIO_SIGNED_EXT_UNION
 结构说明  : SIGNED_EXT 寄存器结构定义。地址偏移量:0x084，初值:0x00000000，宽度:32
 寄存器说明: 高位数据符号扩展使能寄存器。该标志只对接收数据有效，对发送数据无效。PCM模式和I2S模式下接收到的数据都支持符号扩展。
            在接收有效数据位宽为8/16/18/20/24 bit时，如果该标志使能，把接收到的数据转换为32bit数据时，把32bit数据的高位无效比特设置为接收数据最高有效bit对应的值，然后再写入接收FIFO。
            以16bit位宽为例：
            if (data_rx[15] == 1 )
            data_rx[31:16]=0xffff;
            else
            data_rx[31:16]=0x0000;
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  signed_ext_en : 1;  /* bit[0]   : 高位数据符号扩展使能。
                                                        0：不使能；
                                                        &#13;1：使能。 */
        unsigned int  reserved      : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_SIO_SIGNED_EXT_UNION;
#endif
#define SOC_SIO_SIGNED_EXT_signed_ext_en_START  (0)
#define SOC_SIO_SIGNED_EXT_signed_ext_en_END    (0)


/*****************************************************************************
 结构名    : SOC_SIO_I2S_POS_MERGE_EN_UNION
 结构说明  : I2S_POS_MERGE_EN 寄存器结构定义。地址偏移量:0x088，初值:0x00000000，宽度:32
 寄存器说明: I2S模式左右声道数据操作地址合并使能寄存器。
            在I2S模式下，用DMA方式读写SIO的FIFO数据时，因为左右声道数据地址不同，需要CPU不断配置DMA操作的地址，导致CPU效率低。为了提高CPU的效率，提供左右声道数据的读写的统一地址使能控制。
            使能情况下，读左右声道数据统一使用SIO_I2S_DUAL_RX_CHN寄存器，写左右声道数据统一使用SIO_I2S_DUAL_TX_CHN寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  merge_en : 1;  /* bit[0]   : 在I2S模式下，对左右声道数据的操作地址合并使能。
                                                   0：不使能；
                                                   &#13;1：使能。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_SIO_I2S_POS_MERGE_EN_UNION;
#endif
#define SOC_SIO_I2S_POS_MERGE_EN_merge_en_START  (0)
#define SOC_SIO_I2S_POS_MERGE_EN_merge_en_END    (0)


/*****************************************************************************
 结构名    : SOC_SIO_INTMASK_UNION
 结构说明  : INTMASK 寄存器结构定义。地址偏移量:0x08C，初值:0x00000000，宽度:32
 寄存器说明: 中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rx_intr             : 1;  /* bit[0]   : 接收FIFO高于阈值中断屏蔽。
                                                              0：不屏蔽该中断；
                                                              1：屏蔽该中断。 */
        unsigned int  tx_intr             : 1;  /* bit[1]   : 发送FIFO低于阈值中断屏蔽。
                                                              0：不屏蔽该中断；
                                                              1：屏蔽该中断。  */
        unsigned int  rx_right_fifo_over  : 1;  /* bit[2]   : I2S模式下，右声道接收FIFO上溢中断屏蔽。PCM模式下，PCM接收FIFO下溢中断屏蔽。
                                                              0：不屏蔽该中断；
                                                              &#13;1：屏蔽该中断。 */
        unsigned int  rx_left_fifo_over   : 1;  /* bit[3]   : I2S模式下，左声道接收FIFO上溢中断屏蔽。PCM模式下无效。
                                                              0：不屏蔽该中断；
                                                              &#13;1：屏蔽该中断。 */
        unsigned int  tx_right_fifo_under : 1;  /* bit[4]   : I2S模式下，右声道发送FIFO下溢中断屏蔽。PCM模式下，PCM发送FIFO下溢中断屏蔽。
                                                              0：不屏蔽该中断；
                                                              &#13;1：屏蔽该中断。 */
        unsigned int  tx_left_fifo_under  : 1;  /* bit[5]   : I2S模式下，左声道发送FIFO下溢中断屏蔽。PCM模式下无效。
                                                              0：不屏蔽该中断；
                                                              &#13;1：屏蔽该中断。 */
        unsigned int  reserved            : 26; /* bit[6-31]: 保留。 */
    } reg;
} SOC_SIO_INTMASK_UNION;
#endif
#define SOC_SIO_INTMASK_rx_intr_START              (0)
#define SOC_SIO_INTMASK_rx_intr_END                (0)
#define SOC_SIO_INTMASK_tx_intr_START              (1)
#define SOC_SIO_INTMASK_tx_intr_END                (1)
#define SOC_SIO_INTMASK_rx_right_fifo_over_START   (2)
#define SOC_SIO_INTMASK_rx_right_fifo_over_END     (2)
#define SOC_SIO_INTMASK_rx_left_fifo_over_START    (3)
#define SOC_SIO_INTMASK_rx_left_fifo_over_END      (3)
#define SOC_SIO_INTMASK_tx_right_fifo_under_START  (4)
#define SOC_SIO_INTMASK_tx_right_fifo_under_END    (4)
#define SOC_SIO_INTMASK_tx_left_fifo_under_START   (5)
#define SOC_SIO_INTMASK_tx_left_fifo_under_END     (5)


/*****************************************************************************
 结构名    : SOC_SIO_I2S_DUAL_RX_CHN_UNION
 结构说明  : I2S_DUAL_RX_CHN 寄存器结构定义。地址偏移量:0x0A0，初值:0x00000000，宽度:32
 寄存器说明: 读取接收数据寄存器。I2S左右声道操作地址合并使能后，读取接收数据。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rx_data : 32; /* bit[0-31]: 接收到的数据。 */
    } reg;
} SOC_SIO_I2S_DUAL_RX_CHN_UNION;
#endif
#define SOC_SIO_I2S_DUAL_RX_CHN_rx_data_START  (0)
#define SOC_SIO_I2S_DUAL_RX_CHN_rx_data_END    (31)


/*****************************************************************************
 结构名    : SOC_SIO_I2S_DUAL_TX_CHN_UNION
 结构说明  : I2S_DUAL_TX_CHN 寄存器结构定义。地址偏移量:0x0C0，初值:0x00000000，宽度:32
 寄存器说明: 写发送数据寄存器。I2S左右声道操作地址合并使能后，写发送数据。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_data : 32; /* bit[0-31]: 发送的数据。 */
    } reg;
} SOC_SIO_I2S_DUAL_TX_CHN_UNION;
#endif
#define SOC_SIO_I2S_DUAL_TX_CHN_tx_data_START  (0)
#define SOC_SIO_I2S_DUAL_TX_CHN_tx_data_END    (31)






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

#endif /* end of soc_sio_interface.h */

