

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_GPIO_INTERFACE_H__
#define __SOC_GPIO_INTERFACE_H__

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
/* 寄存器说明：GPIO数据寄存器，用于对输入或输出数据进行缓存。
            当配置GPIO_DIR中对应位为输出时，写入GPIO_DATA寄存器的值将会输出到相应的管脚（注意：需要配置正确的管脚复用）；如果配置为输入时，将会读取相应输入管脚的值。
            （CH）当GPIO_DIR相应的比特配置为输入时，有效读取的结果将返回管脚的值；当配置为输出的时候，有效读取的结果将返回写入的值。
            GPIO_DATA寄存器利用PADDR[9:2]实现了读写寄存器比特的屏蔽操作。该寄存器对应256个地址空间。PADDR[9：2]分别对应GPIO_DATA[7：0]，当相应的bit为高时，则可以对相应的位进行读写操作；反之，若对应bit为低则不能进行操作。例如：
            （I）若地址为0x3FC（0b11_1111_1100）,则对GPIO_DATA[7：0]这8bit操作全部有效。
            （I）若地址为0x200（0b10_0000_0000）,则仅对GPIO_DATA[7]的操作有效。
   位域定义UNION结构:  SOC_GPIO_GPIODATA_0_UNION */
#define SOC_GPIO_GPIODATA_0_ADDR(base)                ((base) + (0x0004))

/* 寄存器说明：同GPIODATA_0
   位域定义UNION结构:  SOC_GPIO_GPIODATA_1_UNION */
#define SOC_GPIO_GPIODATA_1_ADDR(base)                ((base) + (0x0008))

/* 寄存器说明：同GPIODATA_0
   位域定义UNION结构:  SOC_GPIO_GPIODATA_2_UNION */
#define SOC_GPIO_GPIODATA_2_ADDR(base)                ((base) + (0x0010))

/* 寄存器说明：同GPIODATA_0
   位域定义UNION结构:  SOC_GPIO_GPIODATA_3_UNION */
#define SOC_GPIO_GPIODATA_3_ADDR(base)                ((base) + (0x0020))

/* 寄存器说明：同GPIODATA_0
   位域定义UNION结构:  SOC_GPIO_GPIODATA_4_UNION */
#define SOC_GPIO_GPIODATA_4_ADDR(base)                ((base) + (0x0040))

/* 寄存器说明：同GPIODATA_0
   位域定义UNION结构:  SOC_GPIO_GPIODATA_5_UNION */
#define SOC_GPIO_GPIODATA_5_ADDR(base)                ((base) + (0x0080))

/* 寄存器说明：同GPIODATA_0
   位域定义UNION结构:  SOC_GPIO_GPIODATA_6_UNION */
#define SOC_GPIO_GPIODATA_6_ADDR(base)                ((base) + (0x0100))

/* 寄存器说明：同GPIODATA_0
   位域定义UNION结构:  SOC_GPIO_GPIODATA_7_UNION */
#define SOC_GPIO_GPIODATA_7_ADDR(base)                ((base) + (0x0200))

/* 寄存器说明：方向控制寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIODIR_UNION */
#define SOC_GPIO_GPIODIR_ADDR(base)                   ((base) + (0x400))

/* 寄存器说明：边沿或电平触发方式选择寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOIS_UNION */
#define SOC_GPIO_GPIOIS_ADDR(base)                    ((base) + (0x404))

/* 寄存器说明：单沿或双沿触发方式寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOIBE_UNION */
#define SOC_GPIO_GPIOIBE_ADDR(base)                   ((base) + (0x408))

/* 寄存器说明：升沿/降沿或高电平/低电平选择寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOIEV_UNION */
#define SOC_GPIO_GPIOIEV_ADDR(base)                   ((base) + (0x40C))

/* 寄存器说明：中断屏蔽寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOIE_UNION */
#define SOC_GPIO_GPIOIE_ADDR(base)                    ((base) + (0x410))

/* 寄存器说明：中断屏蔽寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOIE2_UNION */
#define SOC_GPIO_GPIOIE2_ADDR(base)                   ((base) + (0x500))

/* 寄存器说明：中断屏蔽寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOIE3_UNION */
#define SOC_GPIO_GPIOIE3_ADDR(base)                   ((base) + (0x504))

/* 寄存器说明：原始中断状态寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIORIS_UNION */
#define SOC_GPIO_GPIORIS_ADDR(base)                   ((base) + (0x414))

/* 寄存器说明：屏蔽后中断状态寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOMIS_UNION */
#define SOC_GPIO_GPIOMIS_ADDR(base)                   ((base) + (0x418))

/* 寄存器说明：屏蔽后中断状态寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOMIS2_UNION */
#define SOC_GPIO_GPIOMIS2_ADDR(base)                  ((base) + (0x530))

/* 寄存器说明：屏蔽后中断状态寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOMIS3_UNION */
#define SOC_GPIO_GPIOMIS3_ADDR(base)                  ((base) + (0x534))

/* 寄存器说明：中断清除寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOIC_UNION */
#define SOC_GPIO_GPIOIC_ADDR(base)                    ((base) + (0x41C))

/* 寄存器说明：硬/软件模式控制寄存器。
   位域定义UNION结构:  SOC_GPIO_GPIOAFSEL_UNION */
#define SOC_GPIO_GPIOAFSEL_ADDR(base)                 ((base) + (0x420))

/* 寄存器说明：滤毛刺电路使能寄存器
   位域定义UNION结构:  SOC_GPIO_GPIODEBBYP_UNION */
#define SOC_GPIO_GPIODEBBYP_ADDR(base)                ((base) + (0x618))





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
 结构名    : SOC_GPIO_GPIODATA_0_UNION
 结构说明  : GPIODATA_0 寄存器结构定义。地址偏移量:0x0004，初值:0x00000000，宽度:32
 寄存器说明: GPIO数据寄存器，用于对输入或输出数据进行缓存。
            当配置GPIO_DIR中对应位为输出时，写入GPIO_DATA寄存器的值将会输出到相应的管脚（注意：需要配置正确的管脚复用）；如果配置为输入时，将会读取相应输入管脚的值。
            （CH）当GPIO_DIR相应的比特配置为输入时，有效读取的结果将返回管脚的值；当配置为输出的时候，有效读取的结果将返回写入的值。
            GPIO_DATA寄存器利用PADDR[9:2]实现了读写寄存器比特的屏蔽操作。该寄存器对应256个地址空间。PADDR[9：2]分别对应GPIO_DATA[7：0]，当相应的bit为高时，则可以对相应的位进行读写操作；反之，若对应bit为低则不能进行操作。例如：
            （I）若地址为0x3FC（0b11_1111_1100）,则对GPIO_DATA[7：0]这8bit操作全部有效。
            （I）若地址为0x200（0b10_0000_0000）,则仅对GPIO_DATA[7]的操作有效。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_register : 8;  /* bit[0-7] :  */
        unsigned int  reserved      : 24; /* bit[8-31]:  */
    } reg;
} SOC_GPIO_GPIODATA_0_UNION;
#endif
#define SOC_GPIO_GPIODATA_0_data_register_START  (0)
#define SOC_GPIO_GPIODATA_0_data_register_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIODATA_1_UNION
 结构说明  : GPIODATA_1 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: 同GPIODATA_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_register : 2;  /* bit[0-1] :  */
        unsigned int  reserved      : 30; /* bit[2-31]:  */
    } reg;
} SOC_GPIO_GPIODATA_1_UNION;
#endif
#define SOC_GPIO_GPIODATA_1_data_register_START  (0)
#define SOC_GPIO_GPIODATA_1_data_register_END    (1)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIODATA_2_UNION
 结构说明  : GPIODATA_2 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: 同GPIODATA_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_register : 8;  /* bit[0-7] :  */
        unsigned int  reserved      : 24; /* bit[8-31]:  */
    } reg;
} SOC_GPIO_GPIODATA_2_UNION;
#endif
#define SOC_GPIO_GPIODATA_2_data_register_START  (0)
#define SOC_GPIO_GPIODATA_2_data_register_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIODATA_3_UNION
 结构说明  : GPIODATA_3 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: 同GPIODATA_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_register : 8;  /* bit[0-7] :  */
        unsigned int  reserved      : 24; /* bit[8-31]:  */
    } reg;
} SOC_GPIO_GPIODATA_3_UNION;
#endif
#define SOC_GPIO_GPIODATA_3_data_register_START  (0)
#define SOC_GPIO_GPIODATA_3_data_register_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIODATA_4_UNION
 结构说明  : GPIODATA_4 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: 同GPIODATA_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_register : 8;  /* bit[0-7] :  */
        unsigned int  reserved      : 24; /* bit[8-31]:  */
    } reg;
} SOC_GPIO_GPIODATA_4_UNION;
#endif
#define SOC_GPIO_GPIODATA_4_data_register_START  (0)
#define SOC_GPIO_GPIODATA_4_data_register_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIODATA_5_UNION
 结构说明  : GPIODATA_5 寄存器结构定义。地址偏移量:0x0080，初值:0x00000000，宽度:32
 寄存器说明: 同GPIODATA_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_register : 8;  /* bit[0-7] :  */
        unsigned int  reserved      : 24; /* bit[8-31]:  */
    } reg;
} SOC_GPIO_GPIODATA_5_UNION;
#endif
#define SOC_GPIO_GPIODATA_5_data_register_START  (0)
#define SOC_GPIO_GPIODATA_5_data_register_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIODATA_6_UNION
 结构说明  : GPIODATA_6 寄存器结构定义。地址偏移量:0x0100，初值:0x00000000，宽度:32
 寄存器说明: 同GPIODATA_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_register : 8;  /* bit[0-7] :  */
        unsigned int  reserved      : 24; /* bit[8-31]:  */
    } reg;
} SOC_GPIO_GPIODATA_6_UNION;
#endif
#define SOC_GPIO_GPIODATA_6_data_register_START  (0)
#define SOC_GPIO_GPIODATA_6_data_register_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIODATA_7_UNION
 结构说明  : GPIODATA_7 寄存器结构定义。地址偏移量:0x0200，初值:0x00000000，宽度:32
 寄存器说明: 同GPIODATA_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_register : 8;  /* bit[0-7] :  */
        unsigned int  reserved      : 24; /* bit[8-31]:  */
    } reg;
} SOC_GPIO_GPIODATA_7_UNION;
#endif
#define SOC_GPIO_GPIODATA_7_data_register_START  (0)
#define SOC_GPIO_GPIODATA_7_data_register_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIODIR_UNION
 结构说明  : GPIODIR 寄存器结构定义。地址偏移量:0x400，初值:0x00000000，宽度:32
 寄存器说明: 方向控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_direct : 8;  /* bit[0-7] : 数据方向。
                                                      0：输入；
                                                      1：输出。 */
        unsigned int  reserved    : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIODIR_UNION;
#endif
#define SOC_GPIO_GPIODIR_data_direct_START  (0)
#define SOC_GPIO_GPIODIR_data_direct_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOIS_UNION
 结构说明  : GPIOIS 寄存器结构定义。地址偏移量:0x404，初值:0x00000000，宽度:32
 寄存器说明: 边沿或电平触发方式选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  interrupt_sense : 8;  /* bit[0-7] : 边沿或点评触发方式选择。
                                                          0：边沿触发中断；
                                                          1：电平触发中断。 */
        unsigned int  reserved        : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOIS_UNION;
#endif
#define SOC_GPIO_GPIOIS_interrupt_sense_START  (0)
#define SOC_GPIO_GPIOIS_interrupt_sense_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOIBE_UNION
 结构说明  : GPIOIBE 寄存器结构定义。地址偏移量:0x408，初值:0x00000000，宽度:32
 寄存器说明: 单沿或双沿触发方式寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  interrupt_sense : 8;  /* bit[0-7] : 单沿或双沿触发方式选择。
                                                          0：单沿触发。上升沿触发还是下降沿触发由寄存器GPIOIEV决定；
                                                          1：上升沿下降沿都可以触发中断。 */
        unsigned int  reserved        : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOIBE_UNION;
#endif
#define SOC_GPIO_GPIOIBE_interrupt_sense_START  (0)
#define SOC_GPIO_GPIOIBE_interrupt_sense_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOIEV_UNION
 结构说明  : GPIOIEV 寄存器结构定义。地址偏移量:0x40C，初值:0x00000000，宽度:32
 寄存器说明: 升沿/降沿或高电平/低电平选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  interrupt_event : 8;  /* bit[0-7] : 升沿/降沿或高电平/低电平选择。
                                                          0：下降沿或低电平触发中断；
                                                          1：上升沿或高电平触发中断。 */
        unsigned int  reserved        : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOIEV_UNION;
#endif
#define SOC_GPIO_GPIOIEV_interrupt_event_START  (0)
#define SOC_GPIO_GPIOIEV_interrupt_event_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOIE_UNION
 结构说明  : GPIOIE 寄存器结构定义。地址偏移量:0x410，初值:0x00000000，宽度:32
 寄存器说明: 中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  interrupt_mask : 8;  /* bit[0-7] : 中断屏蔽。
                                                         0：屏蔽相应管脚的中断；
                                                         1：使能相应管脚的中断。 */
        unsigned int  reserved       : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOIE_UNION;
#endif
#define SOC_GPIO_GPIOIE_interrupt_mask_START  (0)
#define SOC_GPIO_GPIOIE_interrupt_mask_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOIE2_UNION
 结构说明  : GPIOIE2 寄存器结构定义。地址偏移量:0x500，初值:0x00000000，宽度:32
 寄存器说明: 中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  interrupt2_mask : 8;  /* bit[0-7] : 中断屏蔽。
                                                          0：屏蔽相应管脚的中断；
                                                          1：使能相应管脚的中断。 */
        unsigned int  reserved        : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOIE2_UNION;
#endif
#define SOC_GPIO_GPIOIE2_interrupt2_mask_START  (0)
#define SOC_GPIO_GPIOIE2_interrupt2_mask_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOIE3_UNION
 结构说明  : GPIOIE3 寄存器结构定义。地址偏移量:0x504，初值:0x00000000，宽度:32
 寄存器说明: 中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  interrupt3_mask : 8;  /* bit[0-7] : 中断屏蔽。
                                                          0：屏蔽相应管脚的中断；
                                                          1：使能相应管脚的中断。 */
        unsigned int  reserved        : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOIE3_UNION;
#endif
#define SOC_GPIO_GPIOIE3_interrupt3_mask_START  (0)
#define SOC_GPIO_GPIOIE3_interrupt3_mask_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIORIS_UNION
 结构说明  : GPIORIS 寄存器结构定义。地址偏移量:0x414，初值:0x00000000，宽度:32
 寄存器说明: 原始中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  raw_interrupt_status : 8;  /* bit[0-7] : 原始中断状态。
                                                               0：该位对应的管脚没有触发中断；
                                                               1：该位对应的管脚已经触发中断。 */
        unsigned int  reserved             : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIORIS_UNION;
#endif
#define SOC_GPIO_GPIORIS_raw_interrupt_status_START  (0)
#define SOC_GPIO_GPIORIS_raw_interrupt_status_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOMIS_UNION
 结构说明  : GPIOMIS 寄存器结构定义。地址偏移量:0x418，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  masked_interrupt_status : 8;  /* bit[0-7] : 屏蔽后中断状态。
                                                                  0：该位对应的管脚没有触发中断；
                                                                  1：该位对应的管脚已经触发中断。 */
        unsigned int  reserved                : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOMIS_UNION;
#endif
#define SOC_GPIO_GPIOMIS_masked_interrupt_status_START  (0)
#define SOC_GPIO_GPIOMIS_masked_interrupt_status_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOMIS2_UNION
 结构说明  : GPIOMIS2 寄存器结构定义。地址偏移量:0x530，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  masked_interrupt2_status : 8;  /* bit[0-7] : 屏蔽后中断状态。
                                                                   0：该位对应的管脚没有触发中断；
                                                                   1：该位对应的管脚已经触发中断。 */
        unsigned int  reserved                 : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOMIS2_UNION;
#endif
#define SOC_GPIO_GPIOMIS2_masked_interrupt2_status_START  (0)
#define SOC_GPIO_GPIOMIS2_masked_interrupt2_status_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOMIS3_UNION
 结构说明  : GPIOMIS3 寄存器结构定义。地址偏移量:0x534，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  masked_interrupt3_status : 8;  /* bit[0-7] : 屏蔽后中断状态。
                                                                   0：该位对应的管脚没有触发中断；
                                                                   1：该位对应的管脚已经触发中断。 */
        unsigned int  reserved                 : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOMIS3_UNION;
#endif
#define SOC_GPIO_GPIOMIS3_masked_interrupt3_status_START  (0)
#define SOC_GPIO_GPIOMIS3_masked_interrupt3_status_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOIC_UNION
 结构说明  : GPIOIC 寄存器结构定义。地址偏移量:0x41C，初值:0x00000000，宽度:32
 寄存器说明: 中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  interrupt_status_clear : 8;  /* bit[0-7] : 中断清除。
                                                                 0：对中断无影响；
                                                                 1：清除相应中断。 */
        unsigned int  reserved               : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOIC_UNION;
#endif
#define SOC_GPIO_GPIOIC_interrupt_status_clear_START  (0)
#define SOC_GPIO_GPIOIC_interrupt_status_clear_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIOAFSEL_UNION
 结构说明  : GPIOAFSEL 寄存器结构定义。地址偏移量:0x420，初值:0x00000000，宽度:32
 寄存器说明: 硬/软件模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode_control_select_register : 8;  /* bit[0-7] : 控制模式选择。
                                                                       0：软件控制模式；
                                                                       1：硬件控制模式。 */
        unsigned int  reserved                     : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIOAFSEL_UNION;
#endif
#define SOC_GPIO_GPIOAFSEL_mode_control_select_register_START  (0)
#define SOC_GPIO_GPIOAFSEL_mode_control_select_register_END    (7)


/*****************************************************************************
 结构名    : SOC_GPIO_GPIODEBBYP_UNION
 结构说明  : GPIODEBBYP 寄存器结构定义。地址偏移量:0x618，初值:0x000000FF，宽度:32
 寄存器说明: 滤毛刺电路使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  input_filter : 8;  /* bit[0-7] : 滤毛刺电路使能：(各比特分别控制各个对应通路)
                                                       0：使能；
                                                       1：不使能。 */
        unsigned int  reserved     : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_GPIO_GPIODEBBYP_UNION;
#endif
#define SOC_GPIO_GPIODEBBYP_input_filter_START  (0)
#define SOC_GPIO_GPIODEBBYP_input_filter_END    (7)






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

#endif /* end of soc_gpio_interface.h */
