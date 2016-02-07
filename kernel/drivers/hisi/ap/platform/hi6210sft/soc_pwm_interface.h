

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_PWM_INTERFACE_H__
#define __SOC_PWM_INTERFACE_H__

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
/* 寄存器说明：pwm lock寄存器
   位域定义UNION结构:  SOC_PWM_LOCK_UNION */
#define SOC_PWM_LOCK_ADDR(base)                       ((base) + (0x000))

/* 寄存器说明：pwm控制寄存器
   位域定义UNION结构:  SOC_PWM_CTL_UNION */
#define SOC_PWM_CTL_ADDR(base)                        ((base) + (0x004))

/* 寄存器说明：PWM配置寄存器。
   位域定义UNION结构:  SOC_PWM_CFG_UNION */
#define SOC_PWM_CFG_ADDR(base)                        ((base) + (0x008))

/* 寄存器说明：PWM低功耗状态机寄存器
   位域定义UNION结构:  SOC_PWM_LPIF_STATE_UNION */
#define SOC_PWM_LPIF_STATE_ADDR(base)                 ((base) + (0x020))

/* 寄存器说明：PWM工作状态寄存器
   位域定义UNION结构:  SOC_PWM_STATUS_UNION */
#define SOC_PWM_STATUS_ADDR(base)                     ((base) + (0x0024))

/* 寄存器说明：PWM预分频0寄存器
   位域定义UNION结构:  SOC_PWM_PR0_UNION */
#define SOC_PWM_PR0_ADDR(base)                        ((base) + (0x0100))

/* 寄存器说明：PWM预分频1寄存器
   位域定义UNION结构:  SOC_PWM_PR1_UNION */
#define SOC_PWM_PR1_ADDR(base)                        ((base) + (0x0104))

/* 寄存器说明：PWM预分频0计数寄存器
   位域定义UNION结构:  SOC_PWM_PCNT0_UNION */
#define SOC_PWM_PCNT0_ADDR(base)                      ((base) + (0x0108))

/* 寄存器说明：PWM预分频1计数寄存器
   位域定义UNION结构:  SOC_PWM_PCNT1_UNION */
#define SOC_PWM_PCNT1_ADDR(base)                      ((base) + (0x010C))

/* 寄存器说明：PWM原始中断寄存器。
   位域定义UNION结构:  SOC_PWM_RIS_UNION */
#define SOC_PWM_RIS_ADDR(base)                        ((base) + (0x200))

/* 寄存器说明：PWM中断屏蔽寄存器。
   位域定义UNION结构:  SOC_PWM_MSK_UNION */
#define SOC_PWM_MSK_ADDR(base)                        ((base) + (0x204))

/* 寄存器说明：PWM中断清除寄存器。
   位域定义UNION结构:  SOC_PWM_EOI_UNION */
#define SOC_PWM_EOI_ADDR(base)                        ((base) + (0x208))

/* 寄存器说明：PWM屏蔽后中断寄存器。
   位域定义UNION结构:  SOC_PWM_INT_UNION */
#define SOC_PWM_INT_ADDR(base)                        ((base) + (0x20C))

/* 寄存器说明：PWM周期匹配寄存器
   位域定义UNION结构:  SOC_PWM_C0_MR_UNION */
#define SOC_PWM_C0_MR_ADDR(base)                      ((base) + (0x0300))

/* 寄存器说明：PWM周期匹配寄存器
   位域定义UNION结构:  SOC_PWM_C1_MR_UNION */
#define SOC_PWM_C1_MR_ADDR(base)                      ((base) + (0x0400))

/* 寄存器说明：PWM周期匹配寄存器
   位域定义UNION结构:  SOC_PWM_C0_MR0_UNION */
#define SOC_PWM_C0_MR0_ADDR(base)                     ((base) + (0x0304))

/* 寄存器说明：PWM周期匹配寄存器
   位域定义UNION结构:  SOC_PWM_C1_MR0_UNION */
#define SOC_PWM_C1_MR0_ADDR(base)                     ((base) + (0x0404))

/* 寄存器说明：PWM周期匹配寄存器
   位域定义UNION结构:  SOC_PWM_C0_MR1_UNION */
#define SOC_PWM_C0_MR1_ADDR(base)                     ((base) + (0x0308))

/* 寄存器说明：PWM周期匹配寄存器
   位域定义UNION结构:  SOC_PWM_C1_MR1_UNION */
#define SOC_PWM_C1_MR1_ADDR(base)                     ((base) + (0x0408))

/* 寄存器说明：PWM通道0的定时器寄存器
   位域定义UNION结构:  SOC_PWM_C0_TCNT_UNION */
#define SOC_PWM_C0_TCNT_ADDR(base)                    ((base) + (0x030C))

/* 寄存器说明：PWM通道1的定时器寄存器
   位域定义UNION结构:  SOC_PWM_C1_TCNT_UNION */
#define SOC_PWM_C1_TCNT_ADDR(base)                    ((base) + (0x040C))

/* 寄存器说明：PWM脉冲相位设置寄存器
   位域定义UNION结构:  SOC_PWM_C0_PHA_UNION */
#define SOC_PWM_C0_PHA_ADDR(base)                     ((base) + (0x0310))

/* 寄存器说明：PWM脉冲相位设置寄存器
   位域定义UNION结构:  SOC_PWM_C1_PHA_UNION */
#define SOC_PWM_C1_PHA_ADDR(base)                     ((base) + (0x0410))





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
 结构名    : SOC_PWM_LOCK_UNION
 结构说明  : LOCK 寄存器结构定义。地址偏移量:0x000，初值:0x00000001，宽度:32
 寄存器说明: pwm lock寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_lock : 32; /* bit[0-31]: pwm_lock寄存器，当向该地址写入0x1ACC_E551(该值宏定义可配，配置范围为32'h00000000~32'hffffffff)时，pwm_lock为1'b0,解锁可以配置所有寄存器；当向该寄存器写入其他值时，pwm_lock为1'b1，只能配置lock寄存器。 */
    } reg;
} SOC_PWM_LOCK_UNION;
#endif
#define SOC_PWM_LOCK_pwm_lock_START  (0)
#define SOC_PWM_LOCK_pwm_lock_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_CTL_UNION
 结构说明  : CTL 寄存器结构定义。地址偏移量:0x004，初值:0x00000000，宽度:32
 寄存器说明: pwm控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_en   : 1;  /* bit[0]   : 模块使能信号，高有效，使能和模块计数功能有关，输出pwm_out保持为关闭时状态。
                                                   0：PWM未使能；
                                                   1：PWM使能； */
        unsigned int  pwm_rst  : 1;  /* bit[1]   : 软复位，高有效
                                                   0：软复位无效；
                                                   1：软复位有效； */
        unsigned int  reserved : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_PWM_CTL_UNION;
#endif
#define SOC_PWM_CTL_pwm_en_START    (0)
#define SOC_PWM_CTL_pwm_en_END      (0)
#define SOC_PWM_CTL_pwm_rst_START   (1)
#define SOC_PWM_CTL_pwm_rst_END     (1)


/*****************************************************************************
 结构名    : SOC_PWM_CFG_UNION
 结构说明  : CFG 寄存器结构定义。地址偏移量:0x008，初值:0x00000000，宽度:32
 寄存器说明: PWM配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_mode_0    : 1;  /* bit[0] : 通道0的边沿方式选择信号
                                                      0：单边沿方式
                                                      1：双边沿方式 */
        unsigned int  pwm_oen_0     : 1;  /* bit[1] : 通道0的PWM波形输出控制信号
                                                      0：输出三态
                                                      1：输出PWM波形 */
        unsigned int  pwm_mode_1    : 1;  /* bit[2] : 根据宏定义`PWM_CH_NUM以及`PWM_CH1可配,通道1的边沿方式选择信号
                                                      0：单边沿方式
                                                      1：双边沿方式 */
        unsigned int  pwm_oen_1     : 1;  /* bit[3] : 根据宏定义`PWM_CH_NUM以及`PWM_CH1可配,通道1的PWM波形输出控制信号
                                                      0：输出三态
                                                      1：输出PWM波形 */
        unsigned int  pwm_mode_2    : 1;  /* bit[4] : 根据宏定义`PWM_CH_NUM以及`PWM_CH2可配,通道2的边沿方式选择信号
                                                      0：单边沿方式
                                                      1：双边沿方式 */
        unsigned int  pwm_oen_2     : 1;  /* bit[5] : 根据宏定义`PWM_CH_NUM以及`PWM_CH2可配,通道2的PWM波形输出控制信号
                                                      0：输出三态
                                                      1：输出PWM波形 */
        unsigned int  pwm_mode_3    : 1;  /* bit[6] : 根据宏定义`PWM_CH_NUM以及`PWM_CH3可配,通道3的边沿方式选择信号
                                                      0：单边沿方式
                                                      1：双边沿方式 */
        unsigned int  pwm_oen_3     : 1;  /* bit[7] : 根据宏定义`PWM_CH_NUM以及`PWM_CH3可配,通道3的PWM波形输出控制信号
                                                      0：输出三态
                                                      1：输出PWM波形 */
        unsigned int  pwm_mode_4    : 1;  /* bit[8] : 根据宏定义`PWM_CH_NUM以及`PWM_CH4可配,通道4的边沿方式选择信号
                                                      0：单边沿方式
                                                      1：双边沿方式 */
        unsigned int  pwm_oen_4     : 1;  /* bit[9] : 根据宏定义`PWM_CH_NUM以及`PWM_CH4可配,通道4的PWM波形输出控制信号
                                                      0：输出三态
                                                      1：输出PWM波形 */
        unsigned int  pwm_mode_5    : 1;  /* bit[10]: 根据宏定义`PWM_CH_NUM以及`PWM_CH5可配,通道5的边沿方式选择信号
                                                      0：单边沿方式
                                                      1：双边沿方式 */
        unsigned int  pwm_oen_5     : 1;  /* bit[11]: 根据宏定义`PWM_CH_NUM以及`PWM_CH5可配,通道5的PWM波形输出控制信号
                                                      0：输出三态
                                                      1：输出PWM波形 */
        unsigned int  pwm_mode_6    : 1;  /* bit[12]: 根据宏定义`PWM_CH_NUM以及`PWM_CH6可配,通道6的边沿方式选择信号
                                                      0：单边沿方式
                                                      1：双边沿方式 */
        unsigned int  pwm_oen_6     : 1;  /* bit[13]: 根据宏定义`PWM_CH_NUM以及`PWM_CH6可配,通道6的PWM波形输出控制信号
                                                      0：输出三态
                                                      1：输出PWM波形 */
        unsigned int  pwm_mode_7    : 1;  /* bit[14]: 根据宏定义`PWM_CH_NUM以及`PWM_CH7可配,通道7的边沿方式选择信号
                                                      0：单边沿方式
                                                      1：双边沿方式 */
        unsigned int  pwm_oen_7     : 1;  /* bit[15]: 根据宏定义`PWM_CH_NUM以及`PWM_CH7可配,通道7的PWM波形输出控制信号
                                                      0：输出三态
                                                      1：输出PWM波形 */
        unsigned int  pwm_int_en_0  : 1;  /* bit[16]: 使能pwm_ris_0原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT可关闭 */
        unsigned int  pwm_int_en_1  : 1;  /* bit[17]: 使能pwm_ris_1原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT可关闭 */
        unsigned int  pwm_int_en_2  : 1;  /* bit[18]: 使能pwm_ris_2原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_3  : 1;  /* bit[19]: 使能pwm_ris_3原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_4  : 1;  /* bit[20]: 使能pwm_ris_4原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_5  : 1;  /* bit[21]: 使能pwm_ris_5原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_6  : 1;  /* bit[22]: 使能pwm_ris_6原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_7  : 1;  /* bit[23]: 使能pwm_ris_7原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_8  : 1;  /* bit[24]: 使能pwm_ris_8原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_9  : 1;  /* bit[25]: 使能pwm_ris_9原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_10 : 1;  /* bit[26]: 使能pwm_ris_10原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_11 : 1;  /* bit[27]: 使能pwm_ris_11原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_12 : 1;  /* bit[28]: 使能pwm_ris_12原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_13 : 1;  /* bit[29]: 使能pwm_ris_13原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_14 : 1;  /* bit[30]: 使能pwm_ris_14原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_int_en_15 : 1;  /* bit[31]: 使能pwm_ris_15原始中断信号
                                                      0：使能无效
                                                      1：使能有效
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
    } reg;
} SOC_PWM_CFG_UNION;
#endif
#define SOC_PWM_CFG_pwm_mode_0_START     (0)
#define SOC_PWM_CFG_pwm_mode_0_END       (0)
#define SOC_PWM_CFG_pwm_oen_0_START      (1)
#define SOC_PWM_CFG_pwm_oen_0_END        (1)
#define SOC_PWM_CFG_pwm_mode_1_START     (2)
#define SOC_PWM_CFG_pwm_mode_1_END       (2)
#define SOC_PWM_CFG_pwm_oen_1_START      (3)
#define SOC_PWM_CFG_pwm_oen_1_END        (3)
#define SOC_PWM_CFG_pwm_mode_2_START     (4)
#define SOC_PWM_CFG_pwm_mode_2_END       (4)
#define SOC_PWM_CFG_pwm_oen_2_START      (5)
#define SOC_PWM_CFG_pwm_oen_2_END        (5)
#define SOC_PWM_CFG_pwm_mode_3_START     (6)
#define SOC_PWM_CFG_pwm_mode_3_END       (6)
#define SOC_PWM_CFG_pwm_oen_3_START      (7)
#define SOC_PWM_CFG_pwm_oen_3_END        (7)
#define SOC_PWM_CFG_pwm_mode_4_START     (8)
#define SOC_PWM_CFG_pwm_mode_4_END       (8)
#define SOC_PWM_CFG_pwm_oen_4_START      (9)
#define SOC_PWM_CFG_pwm_oen_4_END        (9)
#define SOC_PWM_CFG_pwm_mode_5_START     (10)
#define SOC_PWM_CFG_pwm_mode_5_END       (10)
#define SOC_PWM_CFG_pwm_oen_5_START      (11)
#define SOC_PWM_CFG_pwm_oen_5_END        (11)
#define SOC_PWM_CFG_pwm_mode_6_START     (12)
#define SOC_PWM_CFG_pwm_mode_6_END       (12)
#define SOC_PWM_CFG_pwm_oen_6_START      (13)
#define SOC_PWM_CFG_pwm_oen_6_END        (13)
#define SOC_PWM_CFG_pwm_mode_7_START     (14)
#define SOC_PWM_CFG_pwm_mode_7_END       (14)
#define SOC_PWM_CFG_pwm_oen_7_START      (15)
#define SOC_PWM_CFG_pwm_oen_7_END        (15)
#define SOC_PWM_CFG_pwm_int_en_0_START   (16)
#define SOC_PWM_CFG_pwm_int_en_0_END     (16)
#define SOC_PWM_CFG_pwm_int_en_1_START   (17)
#define SOC_PWM_CFG_pwm_int_en_1_END     (17)
#define SOC_PWM_CFG_pwm_int_en_2_START   (18)
#define SOC_PWM_CFG_pwm_int_en_2_END     (18)
#define SOC_PWM_CFG_pwm_int_en_3_START   (19)
#define SOC_PWM_CFG_pwm_int_en_3_END     (19)
#define SOC_PWM_CFG_pwm_int_en_4_START   (20)
#define SOC_PWM_CFG_pwm_int_en_4_END     (20)
#define SOC_PWM_CFG_pwm_int_en_5_START   (21)
#define SOC_PWM_CFG_pwm_int_en_5_END     (21)
#define SOC_PWM_CFG_pwm_int_en_6_START   (22)
#define SOC_PWM_CFG_pwm_int_en_6_END     (22)
#define SOC_PWM_CFG_pwm_int_en_7_START   (23)
#define SOC_PWM_CFG_pwm_int_en_7_END     (23)
#define SOC_PWM_CFG_pwm_int_en_8_START   (24)
#define SOC_PWM_CFG_pwm_int_en_8_END     (24)
#define SOC_PWM_CFG_pwm_int_en_9_START   (25)
#define SOC_PWM_CFG_pwm_int_en_9_END     (25)
#define SOC_PWM_CFG_pwm_int_en_10_START  (26)
#define SOC_PWM_CFG_pwm_int_en_10_END    (26)
#define SOC_PWM_CFG_pwm_int_en_11_START  (27)
#define SOC_PWM_CFG_pwm_int_en_11_END    (27)
#define SOC_PWM_CFG_pwm_int_en_12_START  (28)
#define SOC_PWM_CFG_pwm_int_en_12_END    (28)
#define SOC_PWM_CFG_pwm_int_en_13_START  (29)
#define SOC_PWM_CFG_pwm_int_en_13_END    (29)
#define SOC_PWM_CFG_pwm_int_en_14_START  (30)
#define SOC_PWM_CFG_pwm_int_en_14_END    (30)
#define SOC_PWM_CFG_pwm_int_en_15_START  (31)
#define SOC_PWM_CFG_pwm_int_en_15_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_LPIF_STATE_UNION
 结构说明  : LPIF_STATE 寄存器结构定义。地址偏移量:0x020，初值:0x00000000，宽度:32
 寄存器说明: PWM低功耗状态机寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_lpif_state : 3;  /* bit[0-2] : 低功耗状态指示信息
                                                         000：IDLE状态；
                                                         001：正常工作状态；
                                                         010：接收低功耗请求状态；
                                                         011：进入低功耗确认状态；
                                                         100：拒绝进入低功耗状态；
                                                         101：等待退出低功耗请求状态；
                                                         110：等待确认退出低功耗状态。 */
        unsigned int  reserved       : 29; /* bit[3-31]: 保留。 */
    } reg;
} SOC_PWM_LPIF_STATE_UNION;
#endif
#define SOC_PWM_LPIF_STATE_pwm_lpif_state_START  (0)
#define SOC_PWM_LPIF_STATE_pwm_lpif_state_END    (2)


/*****************************************************************************
 结构名    : SOC_PWM_STATUS_UNION
 结构说明  : STATUS 寄存器结构定义。地址偏移量:0x0024，初值:0x00000000，宽度:32
 寄存器说明: PWM工作状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_status : 1;  /* bit[0]   : pwm工作状态寄存器
                                                     0：pwm空闲；
                                                     1：pwm忙； */
        unsigned int  reserved   : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_PWM_STATUS_UNION;
#endif
#define SOC_PWM_STATUS_pwm_status_START  (0)
#define SOC_PWM_STATUS_pwm_status_END    (0)


/*****************************************************************************
 结构名    : SOC_PWM_PR0_UNION
 结构说明  : PR0 寄存器结构定义。地址偏移量:0x0100，初值:0x00000000，宽度:32
 寄存器说明: PWM预分频0寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_pr0  : 16; /* bit[0-15] : 用来记录脉冲调制的预分频0值得大小 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_PWM_PR0_UNION;
#endif
#define SOC_PWM_PR0_pwm_pr0_START   (0)
#define SOC_PWM_PR0_pwm_pr0_END     (15)


/*****************************************************************************
 结构名    : SOC_PWM_PR1_UNION
 结构说明  : PR1 寄存器结构定义。地址偏移量:0x0104，初值:0x00000000，宽度:32
 寄存器说明: PWM预分频1寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_pr1  : 16; /* bit[0-15] : 用来记录脉冲调制的预分频1值得大小 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_PWM_PR1_UNION;
#endif
#define SOC_PWM_PR1_pwm_pr1_START   (0)
#define SOC_PWM_PR1_pwm_pr1_END     (15)


/*****************************************************************************
 结构名    : SOC_PWM_PCNT0_UNION
 结构说明  : PCNT0 寄存器结构定义。地址偏移量:0x0108，初值:0x00000000，宽度:32
 寄存器说明: PWM预分频0计数寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_pcnt1 : 16; /* bit[0-15] : 全局复位或者软复位时，寄存器初始化；当PWM模块使能信号有效时，计数器自加1；当计数器匹配预分频0值时，赋值0，此寄存器位宽1~32宏定义`PWM_PR_WIDTH可配，可通过宏定义`PWM_INCLUDE_DE进行关闭 */
        unsigned int  reserved  : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_PWM_PCNT0_UNION;
#endif
#define SOC_PWM_PCNT0_pwm_pcnt1_START  (0)
#define SOC_PWM_PCNT0_pwm_pcnt1_END    (15)


/*****************************************************************************
 结构名    : SOC_PWM_PCNT1_UNION
 结构说明  : PCNT1 寄存器结构定义。地址偏移量:0x010C，初值:0x00000000，宽度:32
 寄存器说明: PWM预分频1计数寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_pcnt1 : 16; /* bit[0-15] : 全局复位或者软复位时，寄存器初始化；当PWM模块使能信号有效时，计数器自加1；当计数器匹配预分频0值时，赋值0，此寄存器位宽1~32宏定义`PWM_PR_WIDTH可配，可通过宏定义`PWM_INCLUDE_DE进行关闭 */
        unsigned int  reserved  : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_PWM_PCNT1_UNION;
#endif
#define SOC_PWM_PCNT1_pwm_pcnt1_START  (0)
#define SOC_PWM_PCNT1_pwm_pcnt1_END    (15)


/*****************************************************************************
 结构名    : SOC_PWM_RIS_UNION
 结构说明  : RIS 寄存器结构定义。地址偏移量:0x200，初值:0x00000000，宽度:32
 寄存器说明: PWM原始中断寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_ris_0  : 1;  /* bit[0]    : 当通道0匹配寄存器0匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_1  : 1;  /* bit[1]    : 当通道0匹配寄存器1匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_2  : 1;  /* bit[2]    : 当通道1匹配寄存器0匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_3  : 1;  /* bit[3]    : 当通道1匹配寄存器1匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_4  : 1;  /* bit[4]    : 当通道2匹配寄存器0匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_5  : 1;  /* bit[5]    : 当通道2匹配寄存器1匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_6  : 1;  /* bit[6]    : 当通道3匹配寄存器0匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_7  : 1;  /* bit[7]    : 当通道3匹配寄存器1匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_8  : 1;  /* bit[8]    : 当通道4匹配寄存器0匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_9  : 1;  /* bit[9]    : 当通道4匹配寄存器1匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_10 : 1;  /* bit[10]   : 当通道5匹配寄存器0匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_11 : 1;  /* bit[11]   : 当通道5匹配寄存器1匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_12 : 1;  /* bit[12]   : 当通道6匹配寄存器0匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_13 : 1;  /* bit[13]   : 当通道6匹配寄存器1匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_14 : 1;  /* bit[14]   : 当通道7匹配寄存器0匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  pwm_ris_15 : 1;  /* bit[15]   : 当通道7匹配寄存器1匹配时，该信号有效
                                                      0：无中断
                                                      1：产生中断
                                                      根据宏定义`PWM_CH_NUM以及`PWM_INCLUDE_INT可配 */
        unsigned int  reserved   : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_PWM_RIS_UNION;
#endif
#define SOC_PWM_RIS_pwm_ris_0_START   (0)
#define SOC_PWM_RIS_pwm_ris_0_END     (0)
#define SOC_PWM_RIS_pwm_ris_1_START   (1)
#define SOC_PWM_RIS_pwm_ris_1_END     (1)
#define SOC_PWM_RIS_pwm_ris_2_START   (2)
#define SOC_PWM_RIS_pwm_ris_2_END     (2)
#define SOC_PWM_RIS_pwm_ris_3_START   (3)
#define SOC_PWM_RIS_pwm_ris_3_END     (3)
#define SOC_PWM_RIS_pwm_ris_4_START   (4)
#define SOC_PWM_RIS_pwm_ris_4_END     (4)
#define SOC_PWM_RIS_pwm_ris_5_START   (5)
#define SOC_PWM_RIS_pwm_ris_5_END     (5)
#define SOC_PWM_RIS_pwm_ris_6_START   (6)
#define SOC_PWM_RIS_pwm_ris_6_END     (6)
#define SOC_PWM_RIS_pwm_ris_7_START   (7)
#define SOC_PWM_RIS_pwm_ris_7_END     (7)
#define SOC_PWM_RIS_pwm_ris_8_START   (8)
#define SOC_PWM_RIS_pwm_ris_8_END     (8)
#define SOC_PWM_RIS_pwm_ris_9_START   (9)
#define SOC_PWM_RIS_pwm_ris_9_END     (9)
#define SOC_PWM_RIS_pwm_ris_10_START  (10)
#define SOC_PWM_RIS_pwm_ris_10_END    (10)
#define SOC_PWM_RIS_pwm_ris_11_START  (11)
#define SOC_PWM_RIS_pwm_ris_11_END    (11)
#define SOC_PWM_RIS_pwm_ris_12_START  (12)
#define SOC_PWM_RIS_pwm_ris_12_END    (12)
#define SOC_PWM_RIS_pwm_ris_13_START  (13)
#define SOC_PWM_RIS_pwm_ris_13_END    (13)
#define SOC_PWM_RIS_pwm_ris_14_START  (14)
#define SOC_PWM_RIS_pwm_ris_14_END    (14)
#define SOC_PWM_RIS_pwm_ris_15_START  (15)
#define SOC_PWM_RIS_pwm_ris_15_END    (15)


/*****************************************************************************
 结构名    : SOC_PWM_MSK_UNION
 结构说明  : MSK 寄存器结构定义。地址偏移量:0x204，初值:0x00000000，宽度:32
 寄存器说明: PWM中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_msk0  : 1;  /* bit[0]    : 屏蔽pwm_ris_0原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT可关闭 */
        unsigned int  pwm_msk1  : 1;  /* bit[1]    : 屏蔽pwm_ris_1原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk2  : 1;  /* bit[2]    : 屏蔽pwm_ris_2原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk3  : 1;  /* bit[3]    : 屏蔽pwm_ris_3原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk4  : 1;  /* bit[4]    : 屏蔽pwm_ris_4原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk5  : 1;  /* bit[5]    : 屏蔽pwm_ris_5原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk6  : 1;  /* bit[6]    : 屏蔽pwm_ris_6原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk7  : 1;  /* bit[7]    : 屏蔽pwm_ris_7原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk8  : 1;  /* bit[8]    : 屏蔽pwm_ris_8原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk9  : 1;  /* bit[9]    : 屏蔽pwm_ris_9原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk10 : 1;  /* bit[10]   : 屏蔽pwm_ris_10原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk11 : 1;  /* bit[11]   : 屏蔽pwm_ris_11原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk12 : 1;  /* bit[12]   : 屏蔽pwm_ris_12原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk13 : 1;  /* bit[13]   : 屏蔽pwm_ris_13原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk14 : 1;  /* bit[14]   : 屏蔽pwm_ris_14原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_msk15 : 1;  /* bit[15]   : 屏蔽pwm_ris_15原始中断信号
                                                     1：屏蔽中断
                                                     0：未屏蔽
                                                     通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  reserved  : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_PWM_MSK_UNION;
#endif
#define SOC_PWM_MSK_pwm_msk0_START   (0)
#define SOC_PWM_MSK_pwm_msk0_END     (0)
#define SOC_PWM_MSK_pwm_msk1_START   (1)
#define SOC_PWM_MSK_pwm_msk1_END     (1)
#define SOC_PWM_MSK_pwm_msk2_START   (2)
#define SOC_PWM_MSK_pwm_msk2_END     (2)
#define SOC_PWM_MSK_pwm_msk3_START   (3)
#define SOC_PWM_MSK_pwm_msk3_END     (3)
#define SOC_PWM_MSK_pwm_msk4_START   (4)
#define SOC_PWM_MSK_pwm_msk4_END     (4)
#define SOC_PWM_MSK_pwm_msk5_START   (5)
#define SOC_PWM_MSK_pwm_msk5_END     (5)
#define SOC_PWM_MSK_pwm_msk6_START   (6)
#define SOC_PWM_MSK_pwm_msk6_END     (6)
#define SOC_PWM_MSK_pwm_msk7_START   (7)
#define SOC_PWM_MSK_pwm_msk7_END     (7)
#define SOC_PWM_MSK_pwm_msk8_START   (8)
#define SOC_PWM_MSK_pwm_msk8_END     (8)
#define SOC_PWM_MSK_pwm_msk9_START   (9)
#define SOC_PWM_MSK_pwm_msk9_END     (9)
#define SOC_PWM_MSK_pwm_msk10_START  (10)
#define SOC_PWM_MSK_pwm_msk10_END    (10)
#define SOC_PWM_MSK_pwm_msk11_START  (11)
#define SOC_PWM_MSK_pwm_msk11_END    (11)
#define SOC_PWM_MSK_pwm_msk12_START  (12)
#define SOC_PWM_MSK_pwm_msk12_END    (12)
#define SOC_PWM_MSK_pwm_msk13_START  (13)
#define SOC_PWM_MSK_pwm_msk13_END    (13)
#define SOC_PWM_MSK_pwm_msk14_START  (14)
#define SOC_PWM_MSK_pwm_msk14_END    (14)
#define SOC_PWM_MSK_pwm_msk15_START  (15)
#define SOC_PWM_MSK_pwm_msk15_END    (15)


/*****************************************************************************
 结构名    : SOC_PWM_EOI_UNION
 结构说明  : EOI 寄存器结构定义。地址偏移量:0x208，初值:0x00000000，宽度:32
 寄存器说明: PWM中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_eoi_0  : 1;  /* bit[0]    : 清除pwm_ris_0原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT可关闭 */
        unsigned int  pwm_eoi_1  : 1;  /* bit[1]    : 清除pwm_ris_1原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT可关闭 */
        unsigned int  pwm_eoi_2  : 1;  /* bit[2]    : 清除pwm_ris_2原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_3  : 1;  /* bit[3]    : 清除pwm_ris_3原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_4  : 1;  /* bit[4]    : 清除pwm_ris_4原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_5  : 1;  /* bit[5]    : 清除pwm_ris_5原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_6  : 1;  /* bit[6]    : 清除pwm_ris_6原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_7  : 1;  /* bit[7]    : 清除pwm_ris_7原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_8  : 1;  /* bit[8]    : 清除pwm_ris_8原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_9  : 1;  /* bit[9]    : 清除pwm_ris_9原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_10 : 1;  /* bit[10]   : 清除pwm_ris_10原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_11 : 1;  /* bit[11]   : 清除pwm_ris_11原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_12 : 1;  /* bit[12]   : 清除pwm_ris_12原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_13 : 1;  /* bit[13]   : 清除pwm_ris_13原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_14 : 1;  /* bit[14]   : 清除pwm_ris_14原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_eoi_15 : 1;  /* bit[15]   : 清除pwm_ris_15原始中断信号
                                                      0：未屏蔽
                                                      1：清除中断
                                                      通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  reserved   : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_PWM_EOI_UNION;
#endif
#define SOC_PWM_EOI_pwm_eoi_0_START   (0)
#define SOC_PWM_EOI_pwm_eoi_0_END     (0)
#define SOC_PWM_EOI_pwm_eoi_1_START   (1)
#define SOC_PWM_EOI_pwm_eoi_1_END     (1)
#define SOC_PWM_EOI_pwm_eoi_2_START   (2)
#define SOC_PWM_EOI_pwm_eoi_2_END     (2)
#define SOC_PWM_EOI_pwm_eoi_3_START   (3)
#define SOC_PWM_EOI_pwm_eoi_3_END     (3)
#define SOC_PWM_EOI_pwm_eoi_4_START   (4)
#define SOC_PWM_EOI_pwm_eoi_4_END     (4)
#define SOC_PWM_EOI_pwm_eoi_5_START   (5)
#define SOC_PWM_EOI_pwm_eoi_5_END     (5)
#define SOC_PWM_EOI_pwm_eoi_6_START   (6)
#define SOC_PWM_EOI_pwm_eoi_6_END     (6)
#define SOC_PWM_EOI_pwm_eoi_7_START   (7)
#define SOC_PWM_EOI_pwm_eoi_7_END     (7)
#define SOC_PWM_EOI_pwm_eoi_8_START   (8)
#define SOC_PWM_EOI_pwm_eoi_8_END     (8)
#define SOC_PWM_EOI_pwm_eoi_9_START   (9)
#define SOC_PWM_EOI_pwm_eoi_9_END     (9)
#define SOC_PWM_EOI_pwm_eoi_10_START  (10)
#define SOC_PWM_EOI_pwm_eoi_10_END    (10)
#define SOC_PWM_EOI_pwm_eoi_11_START  (11)
#define SOC_PWM_EOI_pwm_eoi_11_END    (11)
#define SOC_PWM_EOI_pwm_eoi_12_START  (12)
#define SOC_PWM_EOI_pwm_eoi_12_END    (12)
#define SOC_PWM_EOI_pwm_eoi_13_START  (13)
#define SOC_PWM_EOI_pwm_eoi_13_END    (13)
#define SOC_PWM_EOI_pwm_eoi_14_START  (14)
#define SOC_PWM_EOI_pwm_eoi_14_END    (14)
#define SOC_PWM_EOI_pwm_eoi_15_START  (15)
#define SOC_PWM_EOI_pwm_eoi_15_END    (15)


/*****************************************************************************
 结构名    : SOC_PWM_INT_UNION
 结构说明  : INT 寄存器结构定义。地址偏移量:0x20C，初值:0x00000000，宽度:32
 寄存器说明: PWM屏蔽后中断寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_imsk_0  : 1;  /* bit[0]    : 屏蔽pwm_imsk_0屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_1  : 1;  /* bit[1]    : 屏蔽pwm_imsk_1屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_2  : 1;  /* bit[2]    : 屏蔽pwm_imsk_2屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_3  : 1;  /* bit[3]    : 屏蔽pwm_imsk_3屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_4  : 1;  /* bit[4]    : 屏蔽pwm_imsk_4屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_5  : 1;  /* bit[5]    : 屏蔽pwm_imsk_5屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_6  : 1;  /* bit[6]    : 屏蔽pwm_imsk_6屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_7  : 1;  /* bit[7]    : 屏蔽pwm_imsk_7屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_8  : 1;  /* bit[8]    : 屏蔽pwm_imsk_8屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_9  : 1;  /* bit[9]    : 屏蔽pwm_imsk_9屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_10 : 1;  /* bit[10]   : 屏蔽pwm_imsk_10屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_11 : 1;  /* bit[11]   : 屏蔽pwm_imsk_11屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_12 : 1;  /* bit[12]   : 屏蔽pwm_imsk_12屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_13 : 1;  /* bit[13]   : 屏蔽pwm_imsk_13屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_14 : 1;  /* bit[14]   : 屏蔽pwm_imsk_14屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  pwm_imsk_15 : 1;  /* bit[15]   : 屏蔽pwm_imsk_15屏蔽后中断信号
                                                       1：发生中断
                                                       0：未发生中断
                                                       通过宏定义`PWM_INCLUDE_INT和`PWM_CH_NUM可关闭 */
        unsigned int  reserved    : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_PWM_INT_UNION;
#endif
#define SOC_PWM_INT_pwm_imsk_0_START   (0)
#define SOC_PWM_INT_pwm_imsk_0_END     (0)
#define SOC_PWM_INT_pwm_imsk_1_START   (1)
#define SOC_PWM_INT_pwm_imsk_1_END     (1)
#define SOC_PWM_INT_pwm_imsk_2_START   (2)
#define SOC_PWM_INT_pwm_imsk_2_END     (2)
#define SOC_PWM_INT_pwm_imsk_3_START   (3)
#define SOC_PWM_INT_pwm_imsk_3_END     (3)
#define SOC_PWM_INT_pwm_imsk_4_START   (4)
#define SOC_PWM_INT_pwm_imsk_4_END     (4)
#define SOC_PWM_INT_pwm_imsk_5_START   (5)
#define SOC_PWM_INT_pwm_imsk_5_END     (5)
#define SOC_PWM_INT_pwm_imsk_6_START   (6)
#define SOC_PWM_INT_pwm_imsk_6_END     (6)
#define SOC_PWM_INT_pwm_imsk_7_START   (7)
#define SOC_PWM_INT_pwm_imsk_7_END     (7)
#define SOC_PWM_INT_pwm_imsk_8_START   (8)
#define SOC_PWM_INT_pwm_imsk_8_END     (8)
#define SOC_PWM_INT_pwm_imsk_9_START   (9)
#define SOC_PWM_INT_pwm_imsk_9_END     (9)
#define SOC_PWM_INT_pwm_imsk_10_START  (10)
#define SOC_PWM_INT_pwm_imsk_10_END    (10)
#define SOC_PWM_INT_pwm_imsk_11_START  (11)
#define SOC_PWM_INT_pwm_imsk_11_END    (11)
#define SOC_PWM_INT_pwm_imsk_12_START  (12)
#define SOC_PWM_INT_pwm_imsk_12_END    (12)
#define SOC_PWM_INT_pwm_imsk_13_START  (13)
#define SOC_PWM_INT_pwm_imsk_13_END    (13)
#define SOC_PWM_INT_pwm_imsk_14_START  (14)
#define SOC_PWM_INT_pwm_imsk_14_END    (14)
#define SOC_PWM_INT_pwm_imsk_15_START  (15)
#define SOC_PWM_INT_pwm_imsk_15_END    (15)


/*****************************************************************************
 结构名    : SOC_PWM_C0_MR_UNION
 结构说明  : C0_MR 寄存器结构定义。地址偏移量:0x0300，初值:0x00000000，宽度:32
 寄存器说明: PWM周期匹配寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_c0_mr : 32; /* bit[0-31]: 用来记录通道0的匹配寄存器的值。 */
    } reg;
} SOC_PWM_C0_MR_UNION;
#endif
#define SOC_PWM_C0_MR_pwm_c0_mr_START  (0)
#define SOC_PWM_C0_MR_pwm_c0_mr_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_C1_MR_UNION
 结构说明  : C1_MR 寄存器结构定义。地址偏移量:0x0400，初值:0x00000000，宽度:32
 寄存器说明: PWM周期匹配寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_c1_mr : 32; /* bit[0-31]: 用来记录通道1的匹配寄存器的值。 */
    } reg;
} SOC_PWM_C1_MR_UNION;
#endif
#define SOC_PWM_C1_MR_pwm_c1_mr_START  (0)
#define SOC_PWM_C1_MR_pwm_c1_mr_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_C0_MR0_UNION
 结构说明  : C0_MR0 寄存器结构定义。地址偏移量:0x0304，初值:0x00000000，宽度:32
 寄存器说明: PWM周期匹配寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_c0_mr0 : 32; /* bit[0-31]: 用来记录通道0的匹配寄存器0的值。 */
    } reg;
} SOC_PWM_C0_MR0_UNION;
#endif
#define SOC_PWM_C0_MR0_pwm_c0_mr0_START  (0)
#define SOC_PWM_C0_MR0_pwm_c0_mr0_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_C1_MR0_UNION
 结构说明  : C1_MR0 寄存器结构定义。地址偏移量:0x0404，初值:0x00000000，宽度:32
 寄存器说明: PWM周期匹配寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_c1_mr0 : 32; /* bit[0-31]: 用来记录通道1的匹配寄存器0的值。 */
    } reg;
} SOC_PWM_C1_MR0_UNION;
#endif
#define SOC_PWM_C1_MR0_pwm_c1_mr0_START  (0)
#define SOC_PWM_C1_MR0_pwm_c1_mr0_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_C0_MR1_UNION
 结构说明  : C0_MR1 寄存器结构定义。地址偏移量:0x0308，初值:0x00000000，宽度:32
 寄存器说明: PWM周期匹配寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_c0_mr1 : 32; /* bit[0-31]: 用来记录通道0的匹配寄存器1的值。 */
    } reg;
} SOC_PWM_C0_MR1_UNION;
#endif
#define SOC_PWM_C0_MR1_pwm_c0_mr1_START  (0)
#define SOC_PWM_C0_MR1_pwm_c0_mr1_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_C1_MR1_UNION
 结构说明  : C1_MR1 寄存器结构定义。地址偏移量:0x0408，初值:0x00000000，宽度:32
 寄存器说明: PWM周期匹配寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_c1_mr1 : 32; /* bit[0-31]: 用来记录通道1的匹配寄存器1的值。 */
    } reg;
} SOC_PWM_C1_MR1_UNION;
#endif
#define SOC_PWM_C1_MR1_pwm_c1_mr1_START  (0)
#define SOC_PWM_C1_MR1_pwm_c1_mr1_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_C0_TCNT_UNION
 结构说明  : C0_TCNT 寄存器结构定义。地址偏移量:0x030C，初值:0x00000000，宽度:32
 寄存器说明: PWM通道0的定时器寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_c0_tcnt : 32; /* bit[0-31]: 全局复位或者软复位时，寄存器初始化；当定时器使能信号有效时，定时器自加1；当技术匹配预分频0值时，赋值0. */
    } reg;
} SOC_PWM_C0_TCNT_UNION;
#endif
#define SOC_PWM_C0_TCNT_pwm_c0_tcnt_START  (0)
#define SOC_PWM_C0_TCNT_pwm_c0_tcnt_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_C1_TCNT_UNION
 结构说明  : C1_TCNT 寄存器结构定义。地址偏移量:0x040C，初值:0x00000000，宽度:32
 寄存器说明: PWM通道1的定时器寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_c1_tcnt : 32; /* bit[0-31]: 全局复位或者软复位时，寄存器初始化；当定时器使能信号有效时，定时器自加1；当技术匹配预分频0值时，赋值0. */
    } reg;
} SOC_PWM_C1_TCNT_UNION;
#endif
#define SOC_PWM_C1_TCNT_pwm_c1_tcnt_START  (0)
#define SOC_PWM_C1_TCNT_pwm_c1_tcnt_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_C0_PHA_UNION
 结构说明  : C0_PHA 寄存器结构定义。地址偏移量:0x0310，初值:0x00000000，宽度:32
 寄存器说明: PWM脉冲相位设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_ch0_pha : 32; /* bit[0-31]: 用来记录通道0输出脉冲的相位值。 */
    } reg;
} SOC_PWM_C0_PHA_UNION;
#endif
#define SOC_PWM_C0_PHA_pwm_ch0_pha_START  (0)
#define SOC_PWM_C0_PHA_pwm_ch0_pha_END    (31)


/*****************************************************************************
 结构名    : SOC_PWM_C1_PHA_UNION
 结构说明  : C1_PHA 寄存器结构定义。地址偏移量:0x0410，初值:0x00000000，宽度:32
 寄存器说明: PWM脉冲相位设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pwm_ch1_pha : 32; /* bit[0-31]: 用来记录通道1输出脉冲的相位值。 */
    } reg;
} SOC_PWM_C1_PHA_UNION;
#endif
#define SOC_PWM_C1_PHA_pwm_ch1_pha_START  (0)
#define SOC_PWM_C1_PHA_pwm_ch1_pha_END    (31)






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

#endif /* end of soc_pwm_interface.h */
