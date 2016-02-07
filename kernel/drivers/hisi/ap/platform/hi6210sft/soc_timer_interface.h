

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_TIMER_INTERFACE_H__
#define __SOC_TIMER_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) register_TIMER
 ****************************************************************************/
/* 寄存器说明：TIMERn计数初值寄存器，保存着定时器的计数初值。
            当定时器处于周期模式且计数值递减到0时，将TIMERn_LOAD寄存器的值重新载入计数器。当直接写本寄存器时，在被TIMCLKENn使能的下一个TIMCLK的上升沿，定时器当前的计数器将更新为写入值。
            TIMERn_LOAD的最小有效值为1，如果往TIMERn_LOAD寄存器写0，将立即产生定时中断。
            当向TIMERn_BGLOAD寄存器写入值时，TIMERn_LOAD寄存器的值也会被覆盖，但定时器计数的当前值不会受到影响。
            若在被TIMCLKENn使能的TIMCLK信号的上升沿到来之前，向TIMERn_BGLOAD寄存器和TIMERn_LOAD寄存器都写入值，则在被TIMCLKENn使能的TIMCLK信号的下一个上升沿，定时计数器的值首先更新为TIMERn_LOAD寄存器的写入值。此后，每当计数器递减到0时，其值重载为TIMERn_BGLOAD寄存器与TIMERn_LOAD寄存器中最后被写入时的写入值。
            在分别对TIMERn_BGLOAD寄存器和TIMERn_LOAD寄存器进行了两次写入之后，读TIMERn_LOAD寄存器返回的值为TIMERn_BGLOAD寄存器的写入值，即为周期模式下计数器第2次递减到0时载入的有效值。
   位域定义UNION结构:  SOC_TIMER_TIMERN_LOAD_UNION */
#define SOC_TIMER_TIMERN_LOAD_ADDR(base, n)           ((base) + (0x000+(0x20*(n))))

/* 寄存器说明：TIMERn当前计数值寄存器。
            当向TIMERn_LOAD寄存器的写操作发生后，TIMERn_VALUE寄存器在PCLK时钟域立刻反映出计数器的新载入值，无须等到下一个被TIMCLKENn使能的TIMCLK时钟沿。
   位域定义UNION结构:  SOC_TIMER_TIMERN_VALUE_UNION */
#define SOC_TIMER_TIMERN_VALUE_ADDR(base, n)          ((base) + (0x004+(0x20*(n))))

/* 寄存器说明：定时器控制寄存器，用于控制定时器的操作模式及中断的产生。
   位域定义UNION结构:  SOC_TIMER_TIMERN_CONTROL_UNION */
#define SOC_TIMER_TIMERN_CONTROL_ADDR(base, n)        ((base) + (0x008+(0x20*(n))))

/* 寄存器说明：中断清除寄存器，对本寄存器的任何写操作都会清除Timer定时器的中断。
   位域定义UNION结构:  SOC_TIMER_TIMERN_INTCLR_UNION */
#define SOC_TIMER_TIMERN_INTCLR_ADDR(base, n)         ((base) + (0x00C+(0x20*(n))))

/* 寄存器说明：原始中断状态寄存器。
   位域定义UNION结构:  SOC_TIMER_TIMERN_RIS_UNION */
#define SOC_TIMER_TIMERN_RIS_ADDR(base, n)            ((base) + (0x010+(0x20*(n))))

/* 寄存器说明：屏蔽后中断状态寄存器。
   位域定义UNION结构:  SOC_TIMER_TIMERN_MIS_UNION */
#define SOC_TIMER_TIMERN_MIS_ADDR(base, n)            ((base) + (0x014+(0x20*(n))))

/* 寄存器说明：周期模式计数初值寄存器。
            在周期模式下，当本寄存器的值更新时，TIMERn_LOAD的值也随之更新为本寄存器的值。但是Timer定时器不会重新计数，而是等到计数器递减到零之后，将TIMERn_LOAD寄存器的值（即本寄存器的值）重新载入计数器。
            本寄存器提供了访问TIMERn_LOAD寄存器的另一种方法。不同之处在于写入本寄存器不会导致定时器立即装载TIMERn_LOAD的值并重新计数。
   位域定义UNION结构:  SOC_TIMER_TIMERN_BGLOAD_UNION */
#define SOC_TIMER_TIMERN_BGLOAD_ADDR(base)            ((base) + (0x018))





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
                     (1/1) register_TIMER
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_TIMER_TIMERN_LOAD_UNION
 结构说明  : TIMERN_LOAD 寄存器结构定义。地址偏移量:0x000+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: TIMERn计数初值寄存器，保存着定时器的计数初值。
            当定时器处于周期模式且计数值递减到0时，将TIMERn_LOAD寄存器的值重新载入计数器。当直接写本寄存器时，在被TIMCLKENn使能的下一个TIMCLK的上升沿，定时器当前的计数器将更新为写入值。
            TIMERn_LOAD的最小有效值为1，如果往TIMERn_LOAD寄存器写0，将立即产生定时中断。
            当向TIMERn_BGLOAD寄存器写入值时，TIMERn_LOAD寄存器的值也会被覆盖，但定时器计数的当前值不会受到影响。
            若在被TIMCLKENn使能的TIMCLK信号的上升沿到来之前，向TIMERn_BGLOAD寄存器和TIMERn_LOAD寄存器都写入值，则在被TIMCLKENn使能的TIMCLK信号的下一个上升沿，定时计数器的值首先更新为TIMERn_LOAD寄存器的写入值。此后，每当计数器递减到0时，其值重载为TIMERn_BGLOAD寄存器与TIMERn_LOAD寄存器中最后被写入时的写入值。
            在分别对TIMERn_BGLOAD寄存器和TIMERn_LOAD寄存器进行了两次写入之后，读TIMERn_LOAD寄存器返回的值为TIMERn_BGLOAD寄存器的写入值，即为周期模式下计数器第2次递减到0时载入的有效值。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer0_load : 32; /* bit[0-31]: Timern的计数初值。 */
    } reg;
} SOC_TIMER_TIMERN_LOAD_UNION;
#endif
#define SOC_TIMER_TIMERN_LOAD_timer0_load_START  (0)
#define SOC_TIMER_TIMERN_LOAD_timer0_load_END    (31)


/*****************************************************************************
 结构名    : SOC_TIMER_TIMERN_VALUE_UNION
 结构说明  : TIMERN_VALUE 寄存器结构定义。地址偏移量:0x004+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: TIMERn当前计数值寄存器。
            当向TIMERn_LOAD寄存器的写操作发生后，TIMERn_VALUE寄存器在PCLK时钟域立刻反映出计数器的新载入值，无须等到下一个被TIMCLKENn使能的TIMCLK时钟沿。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer0_value : 32; /* bit[0-31]: 正在递减的TIMERn的当前值。
                                                       <B>注意：当定时器处于16位模式时，32位的TIMERnValue寄存器的高16位并未被自动设为0。若该定时器之前处于32位模式，并且自进入16位模式后TIMERnLoad寄存器从未被写过，则TIMERnValue寄存器的高16位可能具有非零值。</B> */
    } reg;
} SOC_TIMER_TIMERN_VALUE_UNION;
#endif
#define SOC_TIMER_TIMERN_VALUE_timer0_value_START  (0)
#define SOC_TIMER_TIMERN_VALUE_timer0_value_END    (31)


/*****************************************************************************
 结构名    : SOC_TIMER_TIMERN_CONTROL_UNION
 结构说明  : TIMERN_CONTROL 寄存器结构定义。地址偏移量:0x008+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: 定时器控制寄存器，用于控制定时器的操作模式及中断的产生。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  oneshot   : 1;  /* bit[0]   : 选择计数模式为单次计数模式还是周期计数模式。
                                                    0：周期计数模式或自由运行模式；
                                                    1：单次计数模式。 */
        unsigned int  timersize : 1;  /* bit[1]   : 选择16bit/32bit计数器操作模式。
                                                    0：16bit计数器；
                                                    1：32bit计数器。 */
        unsigned int  timerpre  : 2;  /* bit[2-3] : 设置Timer的预分频因子。
                                                    00：不经过预分频，时钟频率除以1；
                                                    01：4级预分频，将Timer时钟频率除以16；
                                                    10：8级预分频，将Timer时钟频率除以256；
                                                    11：未定义，若设为该值，相当于8级预分频。 */
        unsigned int  reserved_0: 1;  /* bit[4]   : 保留。 */
        unsigned int  intenable : 1;  /* bit[5]   : TIMERx_RIS中断屏蔽。
                                                    0：屏蔽；
                                                    1：不屏蔽。 */
        unsigned int  timermode : 1;  /* bit[6]   : 定时器的计数模式。
                                                    0：自由运行模式；
                                                    1：周期模式。 */
        unsigned int  timeren   : 1;  /* bit[7]   : 定时器使能。
                                                    0：禁止；
                                                    1：使能。 */
        unsigned int  reserved_1: 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_TIMER_TIMERN_CONTROL_UNION;
#endif
#define SOC_TIMER_TIMERN_CONTROL_oneshot_START    (0)
#define SOC_TIMER_TIMERN_CONTROL_oneshot_END      (0)
#define SOC_TIMER_TIMERN_CONTROL_timersize_START  (1)
#define SOC_TIMER_TIMERN_CONTROL_timersize_END    (1)
#define SOC_TIMER_TIMERN_CONTROL_timerpre_START   (2)
#define SOC_TIMER_TIMERN_CONTROL_timerpre_END     (3)
#define SOC_TIMER_TIMERN_CONTROL_intenable_START  (5)
#define SOC_TIMER_TIMERN_CONTROL_intenable_END    (5)
#define SOC_TIMER_TIMERN_CONTROL_timermode_START  (6)
#define SOC_TIMER_TIMERN_CONTROL_timermode_END    (6)
#define SOC_TIMER_TIMERN_CONTROL_timeren_START    (7)
#define SOC_TIMER_TIMERN_CONTROL_timeren_END      (7)


/*****************************************************************************
 结构名    : SOC_TIMER_TIMERN_INTCLR_UNION
 结构说明  : TIMERN_INTCLR 寄存器结构定义。地址偏移量:0x00C+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: 中断清除寄存器，对本寄存器的任何写操作都会清除Timer定时器的中断。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timern_intclr : 1;  /* bit[0]   : 中断清除寄存器位。
                                                        写1清除中断，写0无影响。 */
        unsigned int  timer0_intclr : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_TIMER_TIMERN_INTCLR_UNION;
#endif
#define SOC_TIMER_TIMERN_INTCLR_timern_intclr_START  (0)
#define SOC_TIMER_TIMERN_INTCLR_timern_intclr_END    (0)
#define SOC_TIMER_TIMERN_INTCLR_timer0_intclr_START  (1)
#define SOC_TIMER_TIMERN_INTCLR_timer0_intclr_END    (31)


/*****************************************************************************
 结构名    : SOC_TIMER_TIMERN_RIS_UNION
 结构说明  : TIMERN_RIS 寄存器结构定义。地址偏移量:0x010+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: 原始中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timernris : 1;  /* bit[0]   : 读时返回计数器的原始中断状态。
                                                    0：未产生原始中断；
                                                    1：已产生原始中断。 */
        unsigned int  reserved  : 31; /* bit[1-31]: 保留。写入无效，读时返回0。 */
    } reg;
} SOC_TIMER_TIMERN_RIS_UNION;
#endif
#define SOC_TIMER_TIMERN_RIS_timernris_START  (0)
#define SOC_TIMER_TIMERN_RIS_timernris_END    (0)


/*****************************************************************************
 结构名    : SOC_TIMER_TIMERN_MIS_UNION
 结构说明  : TIMERN_MIS 寄存器结构定义。地址偏移量:0x014+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timernmis : 1;  /* bit[0]   : 读时返回屏蔽后的计数器中断状态。
                                                    0：未产生中断；
                                                    1：已产生中断。 */
        unsigned int  reserved  : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_TIMER_TIMERN_MIS_UNION;
#endif
#define SOC_TIMER_TIMERN_MIS_timernmis_START  (0)
#define SOC_TIMER_TIMERN_MIS_timernmis_END    (0)


/*****************************************************************************
 结构名    : SOC_TIMER_TIMERN_BGLOAD_UNION
 结构说明  : TIMERN_BGLOAD 寄存器结构定义。地址偏移量:0x018，初值:0x00000000，宽度:32
 寄存器说明: 周期模式计数初值寄存器。
            在周期模式下，当本寄存器的值更新时，TIMERn_LOAD的值也随之更新为本寄存器的值。但是Timer定时器不会重新计数，而是等到计数器递减到零之后，将TIMERn_LOAD寄存器的值（即本寄存器的值）重新载入计数器。
            本寄存器提供了访问TIMERn_LOAD寄存器的另一种方法。不同之处在于写入本寄存器不会导致定时器立即装载TIMERn_LOAD的值并重新计数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  timer0bgload : 32; /* bit[0-31]: TIMERn的计数初值。
                                                       <B>注意：与TIMERn_LOAD寄存器有区别，具体请参见TIMERn_LOAD寄存器的描述。</B> */
    } reg;
} SOC_TIMER_TIMERN_BGLOAD_UNION;
#endif
#define SOC_TIMER_TIMERN_BGLOAD_timer0bgload_START  (0)
#define SOC_TIMER_TIMERN_BGLOAD_timer0bgload_END    (31)






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

#endif /* end of soc_timer_interface.h */
