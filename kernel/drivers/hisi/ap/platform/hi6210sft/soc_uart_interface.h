

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_UART_INTERFACE_H__
#define __SOC_UART_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) register_UART
 ****************************************************************************/
/* 寄存器说明：UART数据寄存器，存放接收数据和发送的数据，同时可以从该寄存器中读出接收状态。
   位域定义UNION结构:  SOC_UART_DR_UNION */
#define SOC_UART_DR_ADDR(base)                        ((base) + (0x000))

/* 寄存器说明：接收状态寄存器/错误清除寄存器。
            (I)寄存器读时作为接收状态寄存器。
            (I)寄存器写时作为错误清除寄存器。
            接收状态也可以从UART_DR中读出。从UART_DR中读出的break、frame、parity的状态信息要比从UART_RSR读出的信息优先级高（即UART_DR中的状态变化比UART_RSR更快）。
            对UART_RSR寄存器的任何写操作都会对UART_RSR寄存器进行复位。
   位域定义UNION结构:  SOC_UART_RSR_UNION */
#define SOC_UART_RSR_ADDR(base)                       ((base) + (0x004))

/* 寄存器说明：UART标志寄存器。
   位域定义UNION结构:  SOC_UART_FR_UNION */
#define SOC_UART_FR_ADDR(base)                        ((base) + (0x018))

/* 寄存器说明：IrDA（Infrared Data Association）低功耗计数器寄存器。
   位域定义UNION结构:  SOC_UART_UARTLPR_UNION */
#define SOC_UART_UARTLPR_ADDR(base)                   ((base) + (0x020))

/* 寄存器说明：整数波特率寄存器。
   位域定义UNION结构:  SOC_UART_IBRD_UNION */
#define SOC_UART_IBRD_ADDR(base)                      ((base) + (0x024))

/* 寄存器说明：小数波特率寄存器。
            (CH)
            (CL)整数波特率寄存器和小数波特率寄存器的值必须等到当前数据发送和接收完毕才能更新。
            (CL)最小的分频值为1，最大的分频值为65535（216-1）。即UART_IBRD=0是无效的，而此时UART_FBRD将被忽略。同样，如果UART_IBRD=65535（0xFFFF），UART_FBRD就只能是0，如果比0大，则会导致发送和接收的失败
            (CL)假设UART_FBRD=0x1E、UART_IBRD=0x01，这就表示分频系数的整数部分为30，小数部分为0.015625，整个分频系数为30.015625。
            (CL)UART的波特率=内部总线频率/(16*分频系数)=内部总线频率/(16*30.015625)。
   位域定义UNION结构:  SOC_UART_FBRD_UNION */
#define SOC_UART_FBRD_ADDR(base)                      ((base) + (0x028))

/* 寄存器说明：传输模式控制寄存器。UART_LCR_H、UART_IBRD、UART_FBRD组成一个30bit宽的寄存器。如果更新UART_IBRD和UART_FBRD的内容，必须同时更新UART_LCR_H。
   位域定义UNION结构:  SOC_UART_LCR_H_UNION */
#define SOC_UART_LCR_H_ADDR(base)                     ((base) + (0x02C))

/* 寄存器说明：UART控制寄存器。
            配置UART_CR遵循以下步骤：
            (S)向UART_CR[0]写0，禁止UART。
            (S)等待当前数据发送或接收结束。
            (S)将UART_LCR_H[fen]清0。
            (S)配置UART_CR。
            (S)向UART_CR[0]写1，使能UART。
            (SE)
   位域定义UNION结构:  SOC_UART_CR_UNION */
#define SOC_UART_CR_ADDR(base)                        ((base) + (0x030))

/* 寄存器说明：中断FIFO阈值选择寄存器，用于设置FIFO的中断（UART_TXINTR或UART_RXINTR）触发线。
   位域定义UNION结构:  SOC_UART_IFLS_UNION */
#define SOC_UART_IFLS_ADDR(base)                      ((base) + (0x034))

/* 寄存器说明：中断屏蔽寄存器，用于屏蔽中断。Bit值为0表示屏蔽中断，为1表示不屏蔽中断。
   位域定义UNION结构:  SOC_UART_IMSC_UNION */
#define SOC_UART_IMSC_ADDR(base)                      ((base) + (0x038))

/* 寄存器说明：原始中断状态寄存器，其内容不受中断屏蔽寄存器的影响。Bit值为0表示无中断，为1表示有中断。
   位域定义UNION结构:  SOC_UART_RIS_UNION */
#define SOC_UART_RIS_ADDR(base)                       ((base) + (0x03C))

/* 寄存器说明：屏蔽后中断状态寄存器，其内容为原始中断状态和中断屏蔽进行“与”操作后的结果。Bit值为0表示无中断，为1表示有中断。
   位域定义UNION结构:  SOC_UART_MIS_UNION */
#define SOC_UART_MIS_ADDR(base)                       ((base) + (0x040))

/* 寄存器说明：中断清除寄存器。Bit值写1时相应的中断被清除，写0则不起作用。
   位域定义UNION结构:  SOC_UART_ICR_UNION */
#define SOC_UART_ICR_ADDR(base)                       ((base) + (0x044))

/* 寄存器说明：DMA控制寄存器，用于配置发送FIFO和接收FIFO的DMA使能。
   位域定义UNION结构:  SOC_UART_DMACR_UNION */
#define SOC_UART_DMACR_ADDR(base)                     ((base) + (0x048))





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
                     (1/1) register_UART
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_UART_DR_UNION
 结构说明  : DR 寄存器结构定义。地址偏移量:0x000，初值:0x0000，宽度:16
 寄存器说明: UART数据寄存器，存放接收数据和发送的数据，同时可以从该寄存器中读出接收状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  data     : 8;  /* bit[0-7]  : 接收数据和发送数据。 */
        unsigned short  fe       : 1;  /* bit[8]    : 帧错误。
                                                      0：无帧错误；
                                                      1：接收到的数据的帧错误（错误的停止位）。 */
        unsigned short  pe       : 1;  /* bit[9]    : 校验错误。
                                                      0：无校验错误；
                                                      1：接收数据的校验错误。 */
        unsigned short  be       : 1;  /* bit[10]   : break错误。
                                                      0：无break错误；
                                                      1：检测到break错误，即接收数据的输入保持低的时间比一个全字传输（包括start、data、parity、stop bit）还要长。 */
        unsigned short  oe       : 1;  /* bit[11]   : 溢出错误。
                                                      0：无溢出错误；
                                                      1：溢出错误，接收FIFO满且接收了一个数据。 */
        unsigned short  reserved : 4;  /* bit[12-15]: 保留。 */
    } reg;
} SOC_UART_DR_UNION;
#endif
#define SOC_UART_DR_data_START      (0)
#define SOC_UART_DR_data_END        (7)
#define SOC_UART_DR_fe_START        (8)
#define SOC_UART_DR_fe_END          (8)
#define SOC_UART_DR_pe_START        (9)
#define SOC_UART_DR_pe_END          (9)
#define SOC_UART_DR_be_START        (10)
#define SOC_UART_DR_be_END          (10)
#define SOC_UART_DR_oe_START        (11)
#define SOC_UART_DR_oe_END          (11)


/*****************************************************************************
 结构名    : SOC_UART_RSR_UNION
 结构说明  : RSR 寄存器结构定义。地址偏移量:0x004，初值:0x00，宽度:8
 寄存器说明: 接收状态寄存器/错误清除寄存器。
            (I)寄存器读时作为接收状态寄存器。
            (I)寄存器写时作为错误清除寄存器。
            接收状态也可以从UART_DR中读出。从UART_DR中读出的break、frame、parity的状态信息要比从UART_RSR读出的信息优先级高（即UART_DR中的状态变化比UART_RSR更快）。
            对UART_RSR寄存器的任何写操作都会对UART_RSR寄存器进行复位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fe       : 1;  /* bit[0]  : 帧错误。
                                                   0：无帧错误；
                                                   1：接收到的数据的停止位错误（有效的停止位为1）。 */
        unsigned char  pe       : 1;  /* bit[1]  : 校验错误。
                                                   0：无校验错误；
                                                   1：接收数据的校验错误。&#13;FIFO模式下，该错误与FIFO顶部的数据相关联。 */
        unsigned char  be       : 1;  /* bit[2]  : Break错误。
                                                   0：无break错误；
                                                   1：break错误。&#13;Break的条件：接收数据的输入保持低的时间比一个全字传输（定义了start、data、parity、stop bit）还要长。 */
        unsigned char  oe       : 1;  /* bit[3]  : 溢出错误。
                                                   0：无溢出错误；
                                                   1：溢出错误。&#13;当FIFO满时，FIFO中的内容保持有效，因为不会有下一个数据写到FIFO中，只是移位寄存器会溢出。CPU必须立刻读数据以腾空FIFO。 */
        unsigned char  reserved : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_UART_RSR_UNION;
#endif
#define SOC_UART_RSR_fe_START        (0)
#define SOC_UART_RSR_fe_END          (0)
#define SOC_UART_RSR_pe_START        (1)
#define SOC_UART_RSR_pe_END          (1)
#define SOC_UART_RSR_be_START        (2)
#define SOC_UART_RSR_be_END          (2)
#define SOC_UART_RSR_oe_START        (3)
#define SOC_UART_RSR_oe_END          (3)


/*****************************************************************************
 结构名    : SOC_UART_FR_UNION
 结构说明  : FR 寄存器结构定义。地址偏移量:0x018，初值:0x0010，宽度:16
 寄存器说明: UART标志寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  cts      : 1;  /* bit[0]   : 该位是nUARTCTS（即UARTx_CTS_N）的取反。如modem状态的输入为0，该位为1。 */
        unsigned short  reserved_0: 2;  /* bit[1-2] : 保留。 */
        unsigned short  busy     : 1;  /* bit[3]   : UART忙闲状态位。
                                                     0：UART空闲或者完成发送数据；
                                                     1：UART正忙于发送数据。
                                                     该位一旦置位，该状态一直保持到整个字节（包括所有的停止位）完全从移位寄存器中发送出去。&#13;一旦发送FIFO非空该位就置位，不管UART使能与否。 */
        unsigned short  rxfe     : 1;  /* bit[4]   : 该位的含义由UART_LCR_H[FEN]的状态决定。
                                                     如果UART_LCR_H[fen]为0，则当接收holding register空时该bit置1。&#13;如果UART_LCR_H[fen]为1，则当接收FIFO为空时该位就置1。 */
        unsigned short  txff     : 1;  /* bit[5]   : 该位的含义由UART_LCR_H[FEN]的状态决定。
                                                     如果UART_LCR_H[fen]为0，则当发送holding register满时该位置1。&#13;如果UART_LCR_H[fen]为1，当发送FIFO为满时该位置1。 */
        unsigned short  rxff     : 1;  /* bit[6]   : 该位的含义由UART_LCR_H[FEN]的状态决定。
                                                     如果UART_LCR_H[fen]为0，则当接收holding register满时该位置1。&#13;如果UART_LCR_H[fen]为1，则当接收FIFO为满时该位置1。 */
        unsigned short  txfe     : 1;  /* bit[7]   : 该位的含义由UART_LCR_H[fen]的状态决定。
                                                     如果UART_LCR_H[fen]为0，则当发送holding register空时该位置1。&#13;如果UART_LCR_H[fen]为1，则当发送FIFO为空时该位置1。 */
        unsigned short  reserved_1: 8;  /* bit[8-15]: 保留。 */
    } reg;
} SOC_UART_FR_UNION;
#endif
#define SOC_UART_FR_cts_START       (0)
#define SOC_UART_FR_cts_END         (0)
#define SOC_UART_FR_busy_START      (3)
#define SOC_UART_FR_busy_END        (3)
#define SOC_UART_FR_rxfe_START      (4)
#define SOC_UART_FR_rxfe_END        (4)
#define SOC_UART_FR_txff_START      (5)
#define SOC_UART_FR_txff_END        (5)
#define SOC_UART_FR_rxff_START      (6)
#define SOC_UART_FR_rxff_END        (6)
#define SOC_UART_FR_txfe_START      (7)
#define SOC_UART_FR_txfe_END        (7)


/*****************************************************************************
 结构名    : SOC_UART_UARTLPR_UNION
 结构说明  : UARTLPR 寄存器结构定义。地址偏移量:0x020，初值:0x0000，宽度:16
 寄存器说明: IrDA（Infrared Data Association）低功耗计数器寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  irda     : 8;  /* bit[0-7] : 8位低功耗分频值。 */
        unsigned short  reserved : 8;  /* bit[8-15]: 保留。 */
    } reg;
} SOC_UART_UARTLPR_UNION;
#endif
#define SOC_UART_UARTLPR_irda_START      (0)
#define SOC_UART_UARTLPR_irda_END        (7)


/*****************************************************************************
 结构名    : SOC_UART_IBRD_UNION
 结构说明  : IBRD 寄存器结构定义。地址偏移量:0x024，初值:0x0000，宽度:16
 寄存器说明: 整数波特率寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  bauddivint : 16; /* bit[0-15]: 整数波特率分频值。复位时全部清0。 */
    } reg;
} SOC_UART_IBRD_UNION;
#endif
#define SOC_UART_IBRD_bauddivint_START  (0)
#define SOC_UART_IBRD_bauddivint_END    (15)


/*****************************************************************************
 结构名    : SOC_UART_FBRD_UNION
 结构说明  : FBRD 寄存器结构定义。地址偏移量:0x028，初值:0x0000，宽度:16
 寄存器说明: 小数波特率寄存器。
            (CH)
            (CL)整数波特率寄存器和小数波特率寄存器的值必须等到当前数据发送和接收完毕才能更新。
            (CL)最小的分频值为1，最大的分频值为65535（216-1）。即UART_IBRD=0是无效的，而此时UART_FBRD将被忽略。同样，如果UART_IBRD=65535（0xFFFF），UART_FBRD就只能是0，如果比0大，则会导致发送和接收的失败
            (CL)假设UART_FBRD=0x1E、UART_IBRD=0x01，这就表示分频系数的整数部分为30，小数部分为0.015625，整个分频系数为30.015625。
            (CL)UART的波特率=内部总线频率/(16*分频系数)=内部总线频率/(16*30.015625)。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  banddivfrac : 16; /* bit[0-15]: 小数波特率分频值。 */
    } reg;
} SOC_UART_FBRD_UNION;
#endif
#define SOC_UART_FBRD_banddivfrac_START  (0)
#define SOC_UART_FBRD_banddivfrac_END    (15)


/*****************************************************************************
 结构名    : SOC_UART_LCR_H_UNION
 结构说明  : LCR_H 寄存器结构定义。地址偏移量:0x02C，初值:0x0000，宽度:16
 寄存器说明: 传输模式控制寄存器。UART_LCR_H、UART_IBRD、UART_FBRD组成一个30bit宽的寄存器。如果更新UART_IBRD和UART_FBRD的内容，必须同时更新UART_LCR_H。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  brk      : 1;  /* bit[0]   : 发送break。
                                                     0：无效；
                                                     1：在完成当前数据的发送后，UTXD连续输出低电平。
                                                     <B>注意：要正确的执行break命令，软件将该位置1的时间必须超过2个完整帧；在正常使用中，该位必须清0。</B> */
        unsigned short  pen      : 1;  /* bit[1]   : 校验选择位。
                                                     0：不作校验；
                                                     1：发送方向产生校验，接收方向作校验检查。 */
        unsigned short  eps      : 1;  /* bit[2]   : 发送和接收过程中的奇偶校验选择。
                                                     0：在发送和接收过程中生成奇校验或检查奇校验；
                                                     1：在发送和接收过程中生成偶校验或检查偶校验。&#13;当UART_LCR_H[fen]为0时，该位不起作用。 */
        unsigned short  stp2     : 1;  /* bit[3]   : 发送帧尾2bit停止位判断。
                                                     0：发送的帧尾没有2bit停止位；
                                                     1：发送的帧尾有2bit停止位。&#13;接收逻辑在接收时不检查2bit的停止位。 */
        unsigned short  fen      : 1;  /* bit[4]   : 发送和接收FIFO使能控制。
                                                     0：发送和接收FIFO禁止；
                                                     1：发送和接收FIFO使能。 */
        unsigned short  wlen     : 2;  /* bit[5-6] : 指示发送和接收一个帧里数据比特的数目。
                                                     00：5bit；
                                                     01：6bit；
                                                     10：7bit；
                                                     11：8bit。 */
        unsigned short  sps      : 1;  /* bit[7]   : 校验选择。
                                                     当本寄存器的bit[1]、bit[2]、bit[7]被置位时，校验位就会作为0发送和检测。
                                                     当本寄存器的bit[1]、bit[7]被置位，bit[2]为0时，校验位就会作为1发送和检测。
                                                     当bit[1]、bit[2]、bit[7]都清0，那么stick parity禁止。 */
        unsigned short  reserved : 8;  /* bit[8-15]: 保留。 */
    } reg;
} SOC_UART_LCR_H_UNION;
#endif
#define SOC_UART_LCR_H_brk_START       (0)
#define SOC_UART_LCR_H_brk_END         (0)
#define SOC_UART_LCR_H_pen_START       (1)
#define SOC_UART_LCR_H_pen_END         (1)
#define SOC_UART_LCR_H_eps_START       (2)
#define SOC_UART_LCR_H_eps_END         (2)
#define SOC_UART_LCR_H_stp2_START      (3)
#define SOC_UART_LCR_H_stp2_END        (3)
#define SOC_UART_LCR_H_fen_START       (4)
#define SOC_UART_LCR_H_fen_END         (4)
#define SOC_UART_LCR_H_wlen_START      (5)
#define SOC_UART_LCR_H_wlen_END        (6)
#define SOC_UART_LCR_H_sps_START       (7)
#define SOC_UART_LCR_H_sps_END         (7)


/*****************************************************************************
 结构名    : SOC_UART_CR_UNION
 结构说明  : CR 寄存器结构定义。地址偏移量:0x030，初值:0x0300，宽度:16
 寄存器说明: UART控制寄存器。
            配置UART_CR遵循以下步骤：
            (S)向UART_CR[0]写0，禁止UART。
            (S)等待当前数据发送或接收结束。
            (S)将UART_LCR_H[fen]清0。
            (S)配置UART_CR。
            (S)向UART_CR[0]写1，使能UART。
            (SE)
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  uarten   : 1;  /* bit[0]    : UART使能。
                                                      0：禁止；
                                                      1：使能。&#13;如果在发送和接收过程中将UART禁止，则会在正常停止之前结束当前数据的传送。 */
        unsigned short  reserved_0: 6;  /* bit[1-6]  : 保留。 */
        unsigned short  lbe      : 1;  /* bit[7]    : 环回使能。
                                                      0：禁止；
                                                      1：UARTTXD输出环回到UARTRXD。 */
        unsigned short  txe      : 1;  /* bit[8]    : UART发送使能。
                                                      0：禁止；
                                                      1：使能。&#13;在发送的过程中如果UART被禁止，那么当前数据的发送就会在正常停止之前结束。 */
        unsigned short  rxe      : 1;  /* bit[9]    : UART接收使能。
                                                      0：禁止；
                                                      1：使能。&#13;在接收的过程中如果UART被禁止，那么当前数据的接收就会在正常停止之前结束。 */
        unsigned short  dtr      : 1;  /* bit[10]   : 数据发送准备。
                                                      该bit为UART modem状态输出信号nUARTDTR的取反。
                                                      0：输出信号不变；
                                                      1：即该bit配置为1，则输出信号为0。 */
        unsigned short  rts      : 1;  /* bit[11]   : 请求发送。
                                                      该bit为UART modem状态输出信号nUARTRTS的取反。
                                                      0：输出信号不变；
                                                      1：即该bit配置为1，则输出信号为0。 */
        unsigned short  reserved_1: 2;  /* bit[12-13]: 保留。 */
        unsigned short  rtsen    : 1;  /* bit[14]   : RTS硬件流控使能。
                                                      0：不使能RTS硬件流控；
                                                      1：使能RTS硬件流控，只有当接收FIFO有空间时才请求接收数据。 */
        unsigned short  ctsen    : 1;  /* bit[15]   : CTS硬件流控使能。
                                                      0：不使能CTS硬件流控；
                                                      1：使能CTS硬件流控，只有当nUARTCTS信号有效时才发送数据。 */
    } reg;
} SOC_UART_CR_UNION;
#endif
#define SOC_UART_CR_uarten_START    (0)
#define SOC_UART_CR_uarten_END      (0)
#define SOC_UART_CR_lbe_START       (7)
#define SOC_UART_CR_lbe_END         (7)
#define SOC_UART_CR_txe_START       (8)
#define SOC_UART_CR_txe_END         (8)
#define SOC_UART_CR_rxe_START       (9)
#define SOC_UART_CR_rxe_END         (9)
#define SOC_UART_CR_dtr_START       (10)
#define SOC_UART_CR_dtr_END         (10)
#define SOC_UART_CR_rts_START       (11)
#define SOC_UART_CR_rts_END         (11)
#define SOC_UART_CR_rtsen_START     (14)
#define SOC_UART_CR_rtsen_END       (14)
#define SOC_UART_CR_ctsen_START     (15)
#define SOC_UART_CR_ctsen_END       (15)


/*****************************************************************************
 结构名    : SOC_UART_IFLS_UNION
 结构说明  : IFLS 寄存器结构定义。地址偏移量:0x034，初值:0x0012，宽度:16
 寄存器说明: 中断FIFO阈值选择寄存器，用于设置FIFO的中断（UART_TXINTR或UART_RXINTR）触发线。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  txiflsel : 3;  /* bit[0-2] : 发送中断FIFO的阈值选择，发送中断的触发点如下。
                                                     000：发送FIFO≤1/8full；
                                                     001：发送FIFO≤1/4full；
                                                     011：发送FIFO≤3/4full；
                                                     010：发送FIFO≤1/2full；
                                                     100：发送FIFO≤7/8full；
                                                     101～111：保留。 */
        unsigned short  rxiflsel : 3;  /* bit[3-5] : 接收中断FIFO的阈值选择，接收中断的触发点如下。
                                                     000：接收FIFO≥1/8full；
                                                     001：接收FIFO≥1/4full；
                                                     010：接收FIFO≥1/2full；
                                                     011：接收FIFO≥3/4full；
                                                     100：接收FIFO≥7/8full；
                                                     101～111：保留。 */
        unsigned short  rtsflsel : 3;  /* bit[6-8] : RTS流控对应的接收FIFO的level选择，RTS的触发点如下：
                                                     000：接收FIFO开始>=1/8 full
                                                     001：接收FIFO开始>=1/4 full
                                                     010：接收FIFO开始>=1/2 full
                                                     011：接收FIFO开始>=3/4 full
                                                     100：接收FIFO开始>=7/8 full
                                                     101：接收FIFO开始>=full-2
                                                     110~111：保留
                                                     注意：RTS流控水线配置不能低于接收中断水线；推荐配置为“101”
                                                     （该位域只针对系统中的HI6210V100中断UART1/2） */
        unsigned short  reserved : 7;  /* bit[9-15]: 保留。 */
    } reg;
} SOC_UART_IFLS_UNION;
#endif
#define SOC_UART_IFLS_txiflsel_START  (0)
#define SOC_UART_IFLS_txiflsel_END    (2)
#define SOC_UART_IFLS_rxiflsel_START  (3)
#define SOC_UART_IFLS_rxiflsel_END    (5)
#define SOC_UART_IFLS_rtsflsel_START  (6)
#define SOC_UART_IFLS_rtsflsel_END    (8)


/*****************************************************************************
 结构名    : SOC_UART_IMSC_UNION
 结构说明  : IMSC 寄存器结构定义。地址偏移量:0x038，初值:0x0000，宽度:16
 寄存器说明: 中断屏蔽寄存器，用于屏蔽中断。Bit值为0表示屏蔽中断，为1表示不屏蔽中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  reserved_0: 4;  /* bit[0-3]  : 保留。 */
        unsigned short  rxim     : 1;  /* bit[4]    : 接收中断的屏蔽状态。 */
        unsigned short  txim     : 1;  /* bit[5]    : 发送中断的屏蔽状态。 */
        unsigned short  rtim     : 1;  /* bit[6]    : 接收超时中断的屏蔽状态。 */
        unsigned short  feim     : 1;  /* bit[7]    : 帧错误中断的屏蔽状态。 */
        unsigned short  peim     : 1;  /* bit[8]    : 校验中断的屏蔽状态。 */
        unsigned short  beim     : 1;  /* bit[9]    : break错误中断的屏蔽状态。 */
        unsigned short  oeim     : 1;  /* bit[10]   : 溢出错误中断的屏蔽状态。 */
        unsigned short  reserved_1: 5;  /* bit[11-15]: 保留。 */
    } reg;
} SOC_UART_IMSC_UNION;
#endif
#define SOC_UART_IMSC_rxim_START      (4)
#define SOC_UART_IMSC_rxim_END        (4)
#define SOC_UART_IMSC_txim_START      (5)
#define SOC_UART_IMSC_txim_END        (5)
#define SOC_UART_IMSC_rtim_START      (6)
#define SOC_UART_IMSC_rtim_END        (6)
#define SOC_UART_IMSC_feim_START      (7)
#define SOC_UART_IMSC_feim_END        (7)
#define SOC_UART_IMSC_peim_START      (8)
#define SOC_UART_IMSC_peim_END        (8)
#define SOC_UART_IMSC_beim_START      (9)
#define SOC_UART_IMSC_beim_END        (9)
#define SOC_UART_IMSC_oeim_START      (10)
#define SOC_UART_IMSC_oeim_END        (10)


/*****************************************************************************
 结构名    : SOC_UART_RIS_UNION
 结构说明  : RIS 寄存器结构定义。地址偏移量:0x03C，初值:0x0000，宽度:16
 寄存器说明: 原始中断状态寄存器，其内容不受中断屏蔽寄存器的影响。Bit值为0表示无中断，为1表示有中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  reserved_0: 4;  /* bit[0-3]  : 保留。 */
        unsigned short  rxris    : 1;  /* bit[4]    : 原始的接收中断状态。 */
        unsigned short  txris    : 1;  /* bit[5]    : 原始的发送中断状态。 */
        unsigned short  rtris    : 1;  /* bit[6]    : 原始的接收超时中断状态。 */
        unsigned short  feris    : 1;  /* bit[7]    : 原始的错误中断状态。 */
        unsigned short  peris    : 1;  /* bit[8]    : 原始的校验中断状态。 */
        unsigned short  beris    : 1;  /* bit[9]    : 原始的break错误中断状态。 */
        unsigned short  oeris    : 1;  /* bit[10]   : 原始的溢出错误中断状态。 */
        unsigned short  reserved_1: 5;  /* bit[11-15]: 保留。 */
    } reg;
} SOC_UART_RIS_UNION;
#endif
#define SOC_UART_RIS_rxris_START     (4)
#define SOC_UART_RIS_rxris_END       (4)
#define SOC_UART_RIS_txris_START     (5)
#define SOC_UART_RIS_txris_END       (5)
#define SOC_UART_RIS_rtris_START     (6)
#define SOC_UART_RIS_rtris_END       (6)
#define SOC_UART_RIS_feris_START     (7)
#define SOC_UART_RIS_feris_END       (7)
#define SOC_UART_RIS_peris_START     (8)
#define SOC_UART_RIS_peris_END       (8)
#define SOC_UART_RIS_beris_START     (9)
#define SOC_UART_RIS_beris_END       (9)
#define SOC_UART_RIS_oeris_START     (10)
#define SOC_UART_RIS_oeris_END       (10)


/*****************************************************************************
 结构名    : SOC_UART_MIS_UNION
 结构说明  : MIS 寄存器结构定义。地址偏移量:0x040，初值:0x0000，宽度:16
 寄存器说明: 屏蔽后中断状态寄存器，其内容为原始中断状态和中断屏蔽进行“与”操作后的结果。Bit值为0表示无中断，为1表示有中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  reserved_0: 4;  /* bit[0-3]  : 保留。 */
        unsigned short  rxmis    : 1;  /* bit[4]    : 屏蔽后的接收中断状态。 */
        unsigned short  txmis    : 1;  /* bit[5]    : 屏蔽后的发送中断状态。 */
        unsigned short  rtmis    : 1;  /* bit[6]    : 屏蔽后的接收超时中断状态。 */
        unsigned short  femis    : 1;  /* bit[7]    : 屏蔽后的错误中断状态。 */
        unsigned short  pemis    : 1;  /* bit[8]    : 屏蔽后的校验中断状态。 */
        unsigned short  bemis    : 1;  /* bit[9]    : 屏蔽后的break错误中断状态。 */
        unsigned short  oemis    : 1;  /* bit[10]   : 屏蔽后的溢出错误中断状态。 */
        unsigned short  reserved_1: 5;  /* bit[11-15]: 保留。 */
    } reg;
} SOC_UART_MIS_UNION;
#endif
#define SOC_UART_MIS_rxmis_START     (4)
#define SOC_UART_MIS_rxmis_END       (4)
#define SOC_UART_MIS_txmis_START     (5)
#define SOC_UART_MIS_txmis_END       (5)
#define SOC_UART_MIS_rtmis_START     (6)
#define SOC_UART_MIS_rtmis_END       (6)
#define SOC_UART_MIS_femis_START     (7)
#define SOC_UART_MIS_femis_END       (7)
#define SOC_UART_MIS_pemis_START     (8)
#define SOC_UART_MIS_pemis_END       (8)
#define SOC_UART_MIS_bemis_START     (9)
#define SOC_UART_MIS_bemis_END       (9)
#define SOC_UART_MIS_oemis_START     (10)
#define SOC_UART_MIS_oemis_END       (10)


/*****************************************************************************
 结构名    : SOC_UART_ICR_UNION
 结构说明  : ICR 寄存器结构定义。地址偏移量:0x044，初值:0x0000，宽度:16
 寄存器说明: 中断清除寄存器。Bit值写1时相应的中断被清除，写0则不起作用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  reserved_0: 4;  /* bit[0-3]  : 保留。 */
        unsigned short  rxic     : 1;  /* bit[4]    : 清除接收中断。 */
        unsigned short  txic     : 1;  /* bit[5]    : 清除发送中断。 */
        unsigned short  rtic     : 1;  /* bit[6]    : 清除接收超时中断。 */
        unsigned short  feic     : 1;  /* bit[7]    : 清除错误中断。 */
        unsigned short  peic     : 1;  /* bit[8]    : 清除校验中断。 */
        unsigned short  beic     : 1;  /* bit[9]    : 清除break错误中断。 */
        unsigned short  oeic     : 1;  /* bit[10]   : 清除溢出错误中断。 */
        unsigned short  reserved_1: 5;  /* bit[11-15]: 保留。 */
    } reg;
} SOC_UART_ICR_UNION;
#endif
#define SOC_UART_ICR_rxic_START      (4)
#define SOC_UART_ICR_rxic_END        (4)
#define SOC_UART_ICR_txic_START      (5)
#define SOC_UART_ICR_txic_END        (5)
#define SOC_UART_ICR_rtic_START      (6)
#define SOC_UART_ICR_rtic_END        (6)
#define SOC_UART_ICR_feic_START      (7)
#define SOC_UART_ICR_feic_END        (7)
#define SOC_UART_ICR_peic_START      (8)
#define SOC_UART_ICR_peic_END        (8)
#define SOC_UART_ICR_beic_START      (9)
#define SOC_UART_ICR_beic_END        (9)
#define SOC_UART_ICR_oeic_START      (10)
#define SOC_UART_ICR_oeic_END        (10)


/*****************************************************************************
 结构名    : SOC_UART_DMACR_UNION
 结构说明  : DMACR 寄存器结构定义。地址偏移量:0x048，初值:0x0000，宽度:16
 寄存器说明: DMA控制寄存器，用于配置发送FIFO和接收FIFO的DMA使能。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned short      value;
    struct
    {
        unsigned short  rxdmae   : 1;  /* bit[0]   : 接收FIFO的DMA使能控制。
                                                     0：禁止；
                                                     1：使能。 */
        unsigned short  txdmae   : 1;  /* bit[1]   : 发送FIFO的DMA使能控制。
                                                     0：禁止；
                                                     1：使能。 */
        unsigned short  dmaonerr : 1;  /* bit[2]   : UART错误中断（UARTEINTR）出现时的接收通道DMA使能控制。
                                                     0：当UART错误中断（UARTEINTR）有效时，接收通道DMA的请求输出（UARTRXDMASREQ或UARRTXDMABREQ）有效；
                                                     1：当UART错误中断（UARTEINTR）有效时，接收通道DMA的请求输出（UARTRXDMASREQ或UARRTXDMABREQ）无效。 */
        unsigned short  reserved : 13; /* bit[3-15]: 保留。 */
    } reg;
} SOC_UART_DMACR_UNION;
#endif
#define SOC_UART_DMACR_rxdmae_START    (0)
#define SOC_UART_DMACR_rxdmae_END      (0)
#define SOC_UART_DMACR_txdmae_START    (1)
#define SOC_UART_DMACR_txdmae_END      (1)
#define SOC_UART_DMACR_dmaonerr_START  (2)
#define SOC_UART_DMACR_dmaonerr_END    (2)






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

#endif /* end of soc_uart_interface.h */
