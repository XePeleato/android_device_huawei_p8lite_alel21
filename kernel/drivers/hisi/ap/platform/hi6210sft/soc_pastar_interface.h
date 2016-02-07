/******************************************************************************

                 版权所有 (C), 2001-2013, 华为技术有限公司

 ******************************************************************************
  文 件 名   : soc_pastar_interface.h
  版 本 号   : 初稿
  作    者   : Excel2Code
  生成日期   : 2013-04-08 19:04:14
  最近修改   :
  功能描述   : 接口头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年4月8日
    作    者   : wwx168173
    修改内容   : 从《HI6561 PA Star nManager寄存器描述_PASTAR.xml》自动生成

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_PASTAR_INTERFACE_H__
#define __SOC_PASTAR_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) LTE_PMU
 ****************************************************************************/
/* 寄存器说明：版本寄存器。
   位域定义UNION结构:  SOC_PASTAR_VERSION_UNION */
#define SOC_PASTAR_VERSION_ADDR(base)                 ((base) + (0x20))

/* 寄存器说明：状态标志1寄存器。
   位域定义UNION结构:  SOC_PASTAR_STATUS1_UNION */
#define SOC_PASTAR_STATUS1_ADDR(base)                 ((base) + (0x21))

/* 寄存器说明：其他控制寄存器1。
   位域定义UNION结构:  SOC_PASTAR_PERIPH_CTRL1_UNION */
#define SOC_PASTAR_PERIPH_CTRL1_ADDR(base)            ((base) + (0x22))

/* 寄存器说明：开关控制1寄存器。
   位域定义UNION结构:  SOC_PASTAR_ONOFF1_UNION */
#define SOC_PASTAR_ONOFF1_ADDR(base)                  ((base) + (0x23))

/* 寄存器说明：开关控制2寄存器。
   位域定义UNION结构:  SOC_PASTAR_ONOFF2_UNION */
#define SOC_PASTAR_ONOFF2_ADDR(base)                  ((base) + (0x24))

/* 寄存器说明：BUCK2电压调节寄存器。
   位域定义UNION结构:  SOC_PASTAR_SET_BUCK2_UNION */
#define SOC_PASTAR_SET_BUCK2_ADDR(base)               ((base) + (0x26))

/* 寄存器说明：BUCK1电压调节寄存器。
   位域定义UNION结构:  SOC_PASTAR_SET_BUCK1_UNION */
#define SOC_PASTAR_SET_BUCK1_ADDR(base)               ((base) + (0x27))

/* 寄存器说明：BUCK0电压调节寄存器。
   位域定义UNION结构:  SOC_PASTAR_SET_BUCK0_UNION */
#define SOC_PASTAR_SET_BUCK0_ADDR(base)               ((base) + (0x28))

/* 寄存器说明：BUCK0调节1寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK0_ADJ1_UNION */
#define SOC_PASTAR_BUCK0_ADJ1_ADDR(base)              ((base) + (0x29))

/* 寄存器说明：BUCK0调节2寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK0_ADJ2_UNION */
#define SOC_PASTAR_BUCK0_ADJ2_ADDR(base)              ((base) + (0x2A))

/* 寄存器说明：BUCK0调节3寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK0_ADJ3_UNION */
#define SOC_PASTAR_BUCK0_ADJ3_ADDR(base)              ((base) + (0x2B))

/* 寄存器说明：BUCK0调节4寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK0_ADJ4_UNION */
#define SOC_PASTAR_BUCK0_ADJ4_ADDR(base)              ((base) + (0x2C))

/* 寄存器说明：BUCK0调节5寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK0_ADJ5_UNION */
#define SOC_PASTAR_BUCK0_ADJ5_ADDR(base)              ((base) + (0x2D))

/* 寄存器说明：BUCK1调节1寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK1_ADJ1_UNION */
#define SOC_PASTAR_BUCK1_ADJ1_ADDR(base)              ((base) + (0x30))

/* 寄存器说明：BUCK1调节2寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK1_ADJ2_UNION */
#define SOC_PASTAR_BUCK1_ADJ2_ADDR(base)              ((base) + (0x31))

/* 寄存器说明：BUCK1调节3寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK1_ADJ3_UNION */
#define SOC_PASTAR_BUCK1_ADJ3_ADDR(base)              ((base) + (0x32))

/* 寄存器说明：BUCK2调节1寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK2_ADJ1_UNION */
#define SOC_PASTAR_BUCK2_ADJ1_ADDR(base)              ((base) + (0x36))

/* 寄存器说明：BUCK2调节2寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK2_ADJ2_UNION */
#define SOC_PASTAR_BUCK2_ADJ2_ADDR(base)              ((base) + (0x37))

/* 寄存器说明：BUCK2调节3寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK2_ADJ3_UNION */
#define SOC_PASTAR_BUCK2_ADJ3_ADDR(base)              ((base) + (0x38))

/* 寄存器说明：'BUCK2调节4寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK2_ADJ4_UNION */
#define SOC_PASTAR_BUCK2_ADJ4_ADDR(base)              ((base) + (0x39))

/* 寄存器说明：BUCK频率控制1寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK_CTRL1_UNION */
#define SOC_PASTAR_BUCK_CTRL1_ADDR(base)              ((base) + (0x45))

/* 寄存器说明：BUCK扩频控制2寄存器。
   位域定义UNION结构:  SOC_PASTAR_BUCK_CTRL2_UNION */
#define SOC_PASTAR_BUCK_CTRL2_ADDR(base)              ((base) + (0x46))

/* 寄存器说明：LDO1电压调节寄存器。
   位域定义UNION结构:  SOC_PASTAR_SET_LDO1_UNION */
#define SOC_PASTAR_SET_LDO1_ADDR(base)                ((base) + (0x48))

/* 寄存器说明：LDO2电压调节寄存器。
   位域定义UNION结构:  SOC_PASTAR_SET_LDO2_UNION */
#define SOC_PASTAR_SET_LDO2_ADDR(base)                ((base) + (0x49))

/* 寄存器说明：LDO控制寄存器。
   位域定义UNION结构:  SOC_PASTAR_SET_LDO_CTRL_UNION */
#define SOC_PASTAR_SET_LDO_CTRL_ADDR(base)            ((base) + (0x4A))

/* 寄存器说明：PA_AMPLIFIER寄存器。
   位域定义UNION结构:  SOC_PASTAR_PA_AMP_CTRL_UNION */
#define SOC_PASTAR_PA_AMP_CTRL_ADDR(base)             ((base) + (0x4B))

/* 寄存器说明：CLASSAB控制1寄存器。
   位域定义UNION结构:  SOC_PASTAR_CLASSAB_CTRL_UNION */
#define SOC_PASTAR_CLASSAB_CTRL_ADDR(base)            ((base) + (0x4C))

/* 寄存器说明：ILIM_CTRL控制1寄存器。
   位域定义UNION结构:  SOC_PASTAR_ILIM_CTRL1_UNION */
#define SOC_PASTAR_ILIM_CTRL1_ADDR(base)              ((base) + (0x4E))

/* 寄存器说明：ILIM_CTRL控制1寄存器。
   位域定义UNION结构:  SOC_PASTAR_ILIM_CTRL2_UNION */
#define SOC_PASTAR_ILIM_CTRL2_ADDR(base)              ((base) + (0x4F))

/* 寄存器说明：BANDGAP控制寄存器。
   位域定义UNION结构:  SOC_PASTAR_BANDGAP_UNION */
#define SOC_PASTAR_BANDGAP_ADDR(base)                 ((base) + (0x55))

/* 寄存器说明：PMU模拟部分输入寄存器。
   位域定义UNION结构:  SOC_PASTAR_ANA_IN_UNION */
#define SOC_PASTAR_ANA_IN_ADDR(base)                  ((base) + (0x56))

/* 寄存器说明：保留1寄存器。
   位域定义UNION结构:  SOC_PASTAR_RESERVED1_UNION */
#define SOC_PASTAR_RESERVED1_ADDR(base)               ((base) + (0x57))

/* 寄存器说明：保留2寄存器。
   位域定义UNION结构:  SOC_PASTAR_RESERVED2_UNION */
#define SOC_PASTAR_RESERVED2_ADDR(base)               ((base) + (0x58))

/* 寄存器说明：保留0寄存器。
   位域定义UNION结构:  SOC_PASTAR_RESERVED0_UNION */
#define SOC_PASTAR_RESERVED0_ADDR(base)               ((base) + (0x59))

/* 寄存器说明：OTP设置寄存器。
   位域定义UNION结构:  SOC_PASTAR_SET_OTP_UNION */
#define SOC_PASTAR_SET_OTP_ADDR(base)                 ((base) + (0x5A))

/* 寄存器说明：OTP控制信号寄存器。
   位域定义UNION结构:  SOC_PASTAR_OTP_CTRL_UNION */
#define SOC_PASTAR_OTP_CTRL_ADDR(base)                ((base) + (0x5B))

/* 寄存器说明：OTP写入值寄存器。
   位域定义UNION结构:  SOC_PASTAR_OTP_PDIN_UNION */
#define SOC_PASTAR_OTP_PDIN_ADDR(base)                ((base) + (0x5C))

/* 寄存器说明：OTP读出值0寄存器。
   位域定义UNION结构:  SOC_PASTAR_OTP_PDOB0_UNION */
#define SOC_PASTAR_OTP_PDOB0_ADDR(base)               ((base) + (0x5D))

/* 寄存器说明：OTP读出值1寄存器。
   位域定义UNION结构:  SOC_PASTAR_OTP_PDOB1_UNION */
#define SOC_PASTAR_OTP_PDOB1_ADDR(base)               ((base) + (0x5E))

/* 寄存器说明：OTP读出值2寄存器。
   位域定义UNION结构:  SOC_PASTAR_OTP_PDOB2_UNION */
#define SOC_PASTAR_OTP_PDOB2_ADDR(base)               ((base) + (0x5F))

/* 寄存器说明：OTP读出值3寄存器。
   位域定义UNION结构:  SOC_PASTAR_OTP_PDOB3_UNION */
#define SOC_PASTAR_OTP_PDOB3_ADDR(base)               ((base) + (0x60))

/* 寄存器说明：非下电信息保存寄存器。
   位域定义UNION结构:  SOC_PASTAR_NO_PWR_REG_RO_UNION */
#define SOC_PASTAR_NO_PWR_REG_RO_ADDR(base)           ((base) + (0x6A))





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
                     (1/1) LTE_PMU
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_PASTAR_VERSION_UNION
 结构说明  : VERSION 寄存器结构定义。地址偏移量:0x20，初值:0x10，宽度:8
 寄存器说明: 版本寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  version : 8;  /* bit[0-7]: 版本寄存器，表示版本为V100。 */
    } reg;
} SOC_PASTAR_VERSION_UNION;
#endif
#define SOC_PASTAR_VERSION_version_START  (0)
#define SOC_PASTAR_VERSION_version_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_STATUS1_UNION
 结构说明  : STATUS1 寄存器结构定义。地址偏移量:0x21，初值:0x00，宽度:8
 寄存器说明: 状态标志1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ldo1    : 1;  /* bit[0]: 0：LDO1输出不过载；
                                                    1: LDO1输出过载 。  */
        unsigned char  ocp_ldo2    : 1;  /* bit[1]: 0：LDO2输出不过载；
                                                    1: LDO2输出过载。  */
        unsigned char  ocp_buck0   : 1;  /* bit[2]: 0：BUCK0输出不过载；
                                                    1: BUCK0输出过载。  */
        unsigned char  ocp_buck1   : 1;  /* bit[3]: 0：BUCK1输出不过载；
                                                    1: BUCK1输出过载。  */
        unsigned char  ocp_buck2   : 1;  /* bit[4]: 0：BUCK2输出不过载；
                                                    1: BUCK2输出过载。  */
        unsigned char  otmp150_d3r : 1;  /* bit[5]: 0: 温度没有超过150°；
                                                    1： 温度超过150° 3ms ； */
        unsigned char  otmp125_d3r : 1;  /* bit[6]: 0: 温度没有超过125°；
                                                    1： 温度超过125° 3ms ； */
        unsigned char  vdd_en_flag : 1;  /* bit[7]: VDD_EN flag信号  */
    } reg;
} SOC_PASTAR_STATUS1_UNION;
#endif
#define SOC_PASTAR_STATUS1_ocp_ldo1_START     (0)
#define SOC_PASTAR_STATUS1_ocp_ldo1_END       (0)
#define SOC_PASTAR_STATUS1_ocp_ldo2_START     (1)
#define SOC_PASTAR_STATUS1_ocp_ldo2_END       (1)
#define SOC_PASTAR_STATUS1_ocp_buck0_START    (2)
#define SOC_PASTAR_STATUS1_ocp_buck0_END      (2)
#define SOC_PASTAR_STATUS1_ocp_buck1_START    (3)
#define SOC_PASTAR_STATUS1_ocp_buck1_END      (3)
#define SOC_PASTAR_STATUS1_ocp_buck2_START    (4)
#define SOC_PASTAR_STATUS1_ocp_buck2_END      (4)
#define SOC_PASTAR_STATUS1_otmp150_d3r_START  (5)
#define SOC_PASTAR_STATUS1_otmp150_d3r_END    (5)
#define SOC_PASTAR_STATUS1_otmp125_d3r_START  (6)
#define SOC_PASTAR_STATUS1_otmp125_d3r_END    (6)
#define SOC_PASTAR_STATUS1_vdd_en_flag_START  (7)
#define SOC_PASTAR_STATUS1_vdd_en_flag_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_PERIPH_CTRL1_UNION
 结构说明  : PERIPH_CTRL1 寄存器结构定义。地址偏移量:0x22，初值:0x23，宽度:8
 寄存器说明: 其他控制寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_auto_off         : 1;  /* bit[0]  : 0：不支持OCP模块（LDO|BUCK）自动关闭；
                                                               1：支持OCP模块（LDO|BUCK）自动关闭；  */
        unsigned char  otmp_off_ctrl        : 1;  /* bit[1]  : 0：高温（150℃）不关机；
                                                               1：高温(150℃)关机。 */
        unsigned char  ocp_deb_sel          : 2;  /* bit[2-3]: ocp去抖时间：
                                                               00：500us
                                                               01：1ms
                                                               10：2ms
                                                               11：4ms */
        unsigned char  en_buck_ocp_debounce : 1;  /* bit[4]  : 0：BUCK OCP上升沿不去抖
                                                               1：BUCK OCP上升沿去抖； */
        unsigned char  en_ldo_ocp_debounce  : 1;  /* bit[5]  : 0：LDO OCP上升沿不去抖
                                                               1：LDO OCP上升沿去抖； */
        unsigned char  reserved             : 2;  /* bit[6-7]: 保留 */
    } reg;
} SOC_PASTAR_PERIPH_CTRL1_UNION;
#endif
#define SOC_PASTAR_PERIPH_CTRL1_ocp_auto_off_START          (0)
#define SOC_PASTAR_PERIPH_CTRL1_ocp_auto_off_END            (0)
#define SOC_PASTAR_PERIPH_CTRL1_otmp_off_ctrl_START         (1)
#define SOC_PASTAR_PERIPH_CTRL1_otmp_off_ctrl_END           (1)
#define SOC_PASTAR_PERIPH_CTRL1_ocp_deb_sel_START           (2)
#define SOC_PASTAR_PERIPH_CTRL1_ocp_deb_sel_END             (3)
#define SOC_PASTAR_PERIPH_CTRL1_en_buck_ocp_debounce_START  (4)
#define SOC_PASTAR_PERIPH_CTRL1_en_buck_ocp_debounce_END    (4)
#define SOC_PASTAR_PERIPH_CTRL1_en_ldo_ocp_debounce_START   (5)
#define SOC_PASTAR_PERIPH_CTRL1_en_ldo_ocp_debounce_END     (5)


/*****************************************************************************
 结构名    : SOC_PASTAR_ONOFF1_UNION
 结构说明  : ONOFF1 寄存器结构定义。地址偏移量:0x23，初值:0x00，宽度:8
 寄存器说明: 开关控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved_0   : 1;  /* bit[0]  : 保留。 */
        unsigned char  en_buck0_int : 1;  /* bit[1]  : 0：关闭BUCK0；
                                                       1: 开启BUCK0；  */
        unsigned char  reserved_1   : 1;  /* bit[2]  : 保留。 */
        unsigned char  en_buck1_int : 1;  /* bit[3]  : 0：关闭BUCK1；
                                                       1: 开启BUCK1；  */
        unsigned char  reserved_2   : 1;  /* bit[4]  : 保留。 */
        unsigned char  en_buck2_int : 1;  /* bit[5]  : 0：关闭BUCK2； 
                                                       1：开启BUCK2 。 */
        unsigned char  reserved_3   : 2;  /* bit[6-7]: 保留  */
    } reg;
} SOC_PASTAR_ONOFF1_UNION;
#endif
#define SOC_PASTAR_ONOFF1_en_buck0_int_START  (1)
#define SOC_PASTAR_ONOFF1_en_buck0_int_END    (1)
#define SOC_PASTAR_ONOFF1_en_buck1_int_START  (3)
#define SOC_PASTAR_ONOFF1_en_buck1_int_END    (3)
#define SOC_PASTAR_ONOFF1_en_buck2_int_START  (5)
#define SOC_PASTAR_ONOFF1_en_buck2_int_END    (5)


/*****************************************************************************
 结构名    : SOC_PASTAR_ONOFF2_UNION
 结构说明  : ONOFF2 寄存器结构定义。地址偏移量:0x24，初值:0x00，宽度:8
 寄存器说明: 开关控制2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_eco_ldo1_int : 1;  /* bit[0]  : 0：LDO1正常模式输出；
                                                          1：LDO1节能模式输出。  */
        unsigned char  en_ldo1_int     : 1;  /* bit[1]  : 0：关闭LDO1； 
                                                          1:开启LDO1 。  */
        unsigned char  en_eco_ldo2_int : 1;  /* bit[2]  : 0：LDO2正常模式输出；
                                                          1：LDO2节能模式输出。  */
        unsigned char  en_ldo2_int     : 1;  /* bit[3]  : 0：关闭LDO2；
                                                          1:开启LDO2。  */
        unsigned char  reserved        : 4;  /* bit[4-7]: 保留 */
    } reg;
} SOC_PASTAR_ONOFF2_UNION;
#endif
#define SOC_PASTAR_ONOFF2_en_eco_ldo1_int_START  (0)
#define SOC_PASTAR_ONOFF2_en_eco_ldo1_int_END    (0)
#define SOC_PASTAR_ONOFF2_en_ldo1_int_START      (1)
#define SOC_PASTAR_ONOFF2_en_ldo1_int_END        (1)
#define SOC_PASTAR_ONOFF2_en_eco_ldo2_int_START  (2)
#define SOC_PASTAR_ONOFF2_en_eco_ldo2_int_END    (2)
#define SOC_PASTAR_ONOFF2_en_ldo2_int_START      (3)
#define SOC_PASTAR_ONOFF2_en_ldo2_int_END        (3)


/*****************************************************************************
 结构名    : SOC_PASTAR_SET_BUCK2_UNION
 结构说明  : SET_BUCK2 寄存器结构定义。地址偏移量:0x26，初值:0x1E，宽度:8
 寄存器说明: BUCK2电压调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_buck2        : 4;  /* bit[0-3]: BUCK2 电压调节。
                                                            0x0: 1.00 V;
                                                            0x1: 1.10 V;
                                                            0x2: 1.15 V;
                                                            0x3: 1.20 V; 
                                                            0x4: 1.25 V;
                                                            0x5: 1.30 V; 
                                                            0x6: 1.35 V;
                                                            0x7: 1.40 V;
                                                            0x8: 1.45 V;
                                                            0x9: 1.50 V;
                                                            0xA: 1.55 V;
                                                            0xB: 1.60 V;
                                                            0xC: 1.70 V;
                                                            0xD: 1.75 V;
                                                            0xE:1.80 V; 
                                                            0xF: 1.85 V; */
        unsigned char  buck2_mod_sel_int : 2;  /* bit[4-5]: BUCK2的模式选择：
                                                            00：自由切换
                                                            X1：PWM;
                                                            10：PFM; */
        unsigned char  reserved          : 2;  /* bit[6-7]: 保留。 */
    } reg;
} SOC_PASTAR_SET_BUCK2_UNION;
#endif
#define SOC_PASTAR_SET_BUCK2_vset_buck2_START         (0)
#define SOC_PASTAR_SET_BUCK2_vset_buck2_END           (3)
#define SOC_PASTAR_SET_BUCK2_buck2_mod_sel_int_START  (4)
#define SOC_PASTAR_SET_BUCK2_buck2_mod_sel_int_END    (5)


/*****************************************************************************
 结构名    : SOC_PASTAR_SET_BUCK1_UNION
 结构说明  : SET_BUCK1 寄存器结构定义。地址偏移量:0x27，初值:0x18，宽度:8
 寄存器说明: BUCK1电压调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_buck1        : 4;  /* bit[0-3]: BUCK1 电压调节。
                                                            0x0: 1.50 V ；
                                                            0x1: 1.60 V ；
                                                            0x2: 1.70 V ；
                                                            0x3: 1.80 V ； 
                                                            0x4: 1.85 V ；
                                                            0x5: 1.90 V ； 
                                                            0x6: 2.00 V ；
                                                            0x7: 2.10 V ；
                                                            0x8: 2.20V ；
                                                            0x9: 2.25 V;
                                                            0xA: 2.30 V;
                                                            0xB: 2.35 V;
                                                            0xC: 2.50 V;
                                                            0xD: 2.80 V;
                                                            0xE: 2.85 V; 
                                                            0xF: 3.20 V; */
        unsigned char  buck1_mod_sel_int : 2;  /* bit[4-5]: BUCK1的模式选择：
                                                            00：自由切换
                                                            X1：PWM;
                                                            10：PFM; */
        unsigned char  reserved          : 2;  /* bit[6-7]: 保留。 */
    } reg;
} SOC_PASTAR_SET_BUCK1_UNION;
#endif
#define SOC_PASTAR_SET_BUCK1_vset_buck1_START         (0)
#define SOC_PASTAR_SET_BUCK1_vset_buck1_END           (3)
#define SOC_PASTAR_SET_BUCK1_buck1_mod_sel_int_START  (4)
#define SOC_PASTAR_SET_BUCK1_buck1_mod_sel_int_END    (5)


/*****************************************************************************
 结构名    : SOC_PASTAR_SET_BUCK0_UNION
 结构说明  : SET_BUCK0 寄存器结构定义。地址偏移量:0x28，初值:0x23，宽度:8
 寄存器说明: BUCK0电压调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_buck0         : 4;  /* bit[0-3]: BUCK0 电压调节。
                                                             0x0: 0.90 V;
                                                             0x1: 1.10 V;
                                                             0x2: 1.30 V;
                                                             0x3: 1.50 V; 
                                                             0x4: 1.70 V;
                                                             0x5: 1.90 V; 
                                                             0x6: 2.10 V;
                                                             0x7: 2.30 V;
                                                             0x8: 2.50 V;
                                                             0x9: 2.70 V;
                                                             0xA: 2.90 V;
                                                             0xB: 3.10 V;
                                                             0xC: 3.30 V;
                                                             0xD: 3.50 V;
                                                             0xE: 3.70 V; 
                                                             0xF: 3.90 V; */
        unsigned char  pdm_sel            : 1;  /* bit[4]  : 使能APT功能信号。
                                                             0：APT功能无效；
                                                             1：APT功能有效。 */
        unsigned char  buck0_mode_sel_int : 2;  /* bit[5-6]: BUCK0工作模式选择。 
                                                             00:pwm与pfm自动切换；
                                                             01：工作在pwm模式；
                                                             10：工作在pfm模式； 
                                                             默认设置为PWM模式。 */
        unsigned char  reserved           : 1;  /* bit[7]  : 保留。 */
    } reg;
} SOC_PASTAR_SET_BUCK0_UNION;
#endif
#define SOC_PASTAR_SET_BUCK0_vset_buck0_START          (0)
#define SOC_PASTAR_SET_BUCK0_vset_buck0_END            (3)
#define SOC_PASTAR_SET_BUCK0_pdm_sel_START             (4)
#define SOC_PASTAR_SET_BUCK0_pdm_sel_END               (4)
#define SOC_PASTAR_SET_BUCK0_buck0_mode_sel_int_START  (5)
#define SOC_PASTAR_SET_BUCK0_buck0_mode_sel_int_END    (6)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK0_ADJ1_UNION
 结构说明  : BUCK0_ADJ1 寄存器结构定义。地址偏移量:0x29，初值:0xF6，宽度:8
 寄存器说明: BUCK0调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_comp_adj : 8;  /* bit[0-7]: BUCK0稳定型调节寄存器
                                                          <7>: 控制PFM工作模式电感的峰值电流点
                                                         0：增加峰值电流
                                                         1：默认值
                                                         
                                                          
                                                         <6：0>：用于调整EA的跨导及调零点的电阻 */
    } reg;
} SOC_PASTAR_BUCK0_ADJ1_UNION;
#endif
#define SOC_PASTAR_BUCK0_ADJ1_buck0_comp_adj_START  (0)
#define SOC_PASTAR_BUCK0_ADJ1_buck0_comp_adj_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK0_ADJ2_UNION
 结构说明  : BUCK0_ADJ2 寄存器结构定义。地址偏移量:0x2A，初值:0x69，宽度:8
 寄存器说明: BUCK0调节2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sg0_sel       : 2;  /* bit[0-1]: 斜坡产生量调节 */
        unsigned char  sc0_sel       : 2;  /* bit[2-3]: 斜坡补偿电阻调节 */
        unsigned char  shortn_buck0  : 1;  /* bit[4]  : buck0的短路保护功能信号
                                                        0：有短路保护
                                                        1：无短路保护 */
        unsigned char  ccm0_ctr      : 1;  /* bit[5]  : buck0的强制连续模式功能
                                                        1：强制连续模式功能关闭
                                                        0：强制连续模式功能开启 */
        unsigned char  buck0_dt_sel  : 1;  /* bit[6]  : BUCK0死区时间调节
                                                        0：增加死区时间
                                                        1：减小死区时间 */
        unsigned char  clampen_buck0 : 1;  /* bit[7]  : VZ箝位功能开关
                                                        0：关闭箝位功能
                                                        1：开启箝位功能 */
    } reg;
} SOC_PASTAR_BUCK0_ADJ2_UNION;
#endif
#define SOC_PASTAR_BUCK0_ADJ2_sg0_sel_START        (0)
#define SOC_PASTAR_BUCK0_ADJ2_sg0_sel_END          (1)
#define SOC_PASTAR_BUCK0_ADJ2_sc0_sel_START        (2)
#define SOC_PASTAR_BUCK0_ADJ2_sc0_sel_END          (3)
#define SOC_PASTAR_BUCK0_ADJ2_shortn_buck0_START   (4)
#define SOC_PASTAR_BUCK0_ADJ2_shortn_buck0_END     (4)
#define SOC_PASTAR_BUCK0_ADJ2_ccm0_ctr_START       (5)
#define SOC_PASTAR_BUCK0_ADJ2_ccm0_ctr_END         (5)
#define SOC_PASTAR_BUCK0_ADJ2_buck0_dt_sel_START   (6)
#define SOC_PASTAR_BUCK0_ADJ2_buck0_dt_sel_END     (6)
#define SOC_PASTAR_BUCK0_ADJ2_clampen_buck0_START  (7)
#define SOC_PASTAR_BUCK0_ADJ2_clampen_buck0_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK0_ADJ3_UNION
 结构说明  : BUCK0_ADJ3 寄存器结构定义。地址偏移量:0x2B，初值:0x20，宽度:8
 寄存器说明: BUCK0调节3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp0_shield   : 1;  /* bit[0]  : BUCK0过流保护功能设置;
                                                        0：有过流保护功能;
                                                        1：无过流保护功能; */
        unsigned char  ocp0_3a_p     : 1;  /* bit[1]  : BUCK0过流点设置;
                                                        0：限流点减低;
                                                        1：限流点提高; */
        unsigned char  buck0_hyctrl  : 1;  /* bit[2]  : APT工作时的上限3.8V的迟滞
                                                        0：默认
                                                        1：加大迟滞 */
        unsigned char  ramp_sel      : 1;  /* bit[3]  : 斜坡补偿的有无；
                                                        0：默认，有斜坡；
                                                        1：关闭斜坡。 */
        unsigned char  ton_sel       : 2;  /* bit[4-5]: ton,min的时间调节；
                                                        00：5ns；
                                                        01：15ns；
                                                        10：25ns default；
                                                        11：35ns； */
        unsigned char  buck0_dmd_sel : 2;  /* bit[6-7]: BUCK0 dmd值调节@5Vin,3.8Vo
                                                        00:350mA;
                                                        10>350mA;
                                                        01<11<0mA; */
    } reg;
} SOC_PASTAR_BUCK0_ADJ3_UNION;
#endif
#define SOC_PASTAR_BUCK0_ADJ3_ocp0_shield_START    (0)
#define SOC_PASTAR_BUCK0_ADJ3_ocp0_shield_END      (0)
#define SOC_PASTAR_BUCK0_ADJ3_ocp0_3a_p_START      (1)
#define SOC_PASTAR_BUCK0_ADJ3_ocp0_3a_p_END        (1)
#define SOC_PASTAR_BUCK0_ADJ3_buck0_hyctrl_START   (2)
#define SOC_PASTAR_BUCK0_ADJ3_buck0_hyctrl_END     (2)
#define SOC_PASTAR_BUCK0_ADJ3_ramp_sel_START       (3)
#define SOC_PASTAR_BUCK0_ADJ3_ramp_sel_END         (3)
#define SOC_PASTAR_BUCK0_ADJ3_ton_sel_START        (4)
#define SOC_PASTAR_BUCK0_ADJ3_ton_sel_END          (5)
#define SOC_PASTAR_BUCK0_ADJ3_buck0_dmd_sel_START  (6)
#define SOC_PASTAR_BUCK0_ADJ3_buck0_dmd_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK0_ADJ4_UNION
 结构说明  : BUCK0_ADJ4 寄存器结构定义。地址偏移量:0x2C，初值:0x3A，宽度:8
 寄存器说明: BUCK0调节4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_reserv : 8;  /* bit[0-7]: <7:6>:设置APT工作时输入参考电压的RC常数
                                                       00：默认
                                                       <5>:电流检测的delay值cs_sel
                                                       1：默认
                                                       0：加大delay
                                                       <4>:APT工作时的驱动分块device_sel
                                                       1：默认，不分块
                                                       0：分块
                                                       <3>:PMOS驱动上升时间调节PR
                                                       1:默认
                                                       0：加大上升时间
                                                       <2>:PMOS驱动下降时间调节PF
                                                       0:默认
                                                       1：加大下降时间
                                                       <1>:NMOS驱动上升时间调节NR
                                                       1:默认
                                                       0：加大上升时间
                                                       <0>:NMOS驱动下降时间调节NF
                                                       0:默认
                                                       1：加大下降时间 */
    } reg;
} SOC_PASTAR_BUCK0_ADJ4_UNION;
#endif
#define SOC_PASTAR_BUCK0_ADJ4_buck0_reserv_START  (0)
#define SOC_PASTAR_BUCK0_ADJ4_buck0_reserv_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK0_ADJ5_UNION
 结构说明  : BUCK0_ADJ5 寄存器结构定义。地址偏移量:0x2D，初值:0x82，宽度:8
 寄存器说明: BUCK0调节5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_reserv2 : 7;  /* bit[0-6]: <6>:设置关闭DMD功能。
                                                        0：默认，不关闭；
                                                        1：关闭。
                                                        <5:4>:调节sense管处的电容CSC_SEL
                                                        00default 0pF
                                                        <3:2>:调节isns处的电容CCC_SEL
                                                        00default 0pF
                                                        <1:0>:调节EA输入对管处的电容EACAP_SEL
                                                        00default 1.8pF */
        unsigned char  en_vodet      : 1;  /* bit[7]  : vo检测功能；
                                                        0x：有vo检测；
                                                        1：默认，无vo检测； */
    } reg;
} SOC_PASTAR_BUCK0_ADJ5_UNION;
#endif
#define SOC_PASTAR_BUCK0_ADJ5_buck0_reserv2_START  (0)
#define SOC_PASTAR_BUCK0_ADJ5_buck0_reserv2_END    (6)
#define SOC_PASTAR_BUCK0_ADJ5_en_vodet_START       (7)
#define SOC_PASTAR_BUCK0_ADJ5_en_vodet_END         (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK1_ADJ1_UNION
 结构说明  : BUCK1_ADJ1 寄存器结构定义。地址偏移量:0x30，初值:0x00，宽度:8
 寄存器说明: BUCK1调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_comp_adj : 8;  /* bit[0-7]: BUCK1稳定型调节寄存器
                                                          <7>: 设置设置GATE_DRV
                                                         1：减弱
                                                         0：加强
                                                         <6：5>: 调整EA内部环路补偿电容的大小
                                                         00:3pF 01:0pF 10：4pF 11:1pF
                                                         <4：0>：用于调整EA的跨导及调零点的电阻 */
    } reg;
} SOC_PASTAR_BUCK1_ADJ1_UNION;
#endif
#define SOC_PASTAR_BUCK1_ADJ1_buck1_comp_adj_START  (0)
#define SOC_PASTAR_BUCK1_ADJ1_buck1_comp_adj_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK1_ADJ2_UNION
 结构说明  : BUCK1_ADJ2 寄存器结构定义。地址偏移量:0x31，初值:0x29，宽度:8
 寄存器说明: BUCK1调节2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_sg_sel  : 2;  /* bit[0-1]: 斜坡产生量调节 */
        unsigned char  buck1_sc_sel  : 2;  /* bit[2-3]: 斜坡补偿电阻调节 */
        unsigned char  buck1_shortn  : 1;  /* bit[4]  : buck1的短路保护功能信号
                                                        0：有短路保护
                                                        1：无短路保护 */
        unsigned char  buck1_ccm_ctr : 1;  /* bit[5]  : buck1的强制连续模式功能
                                                        0：强制连续模式功能开启
                                                        1：强制连续模式功能关闭 */
        unsigned char  buck1_dt_sel  : 1;  /* bit[6]  : BUCK1死区时间调节
                                                        0：默认
                                                        1：减小死区时间 */
        unsigned char  buck1_clampen : 1;  /* bit[7]  : VZ箝位功能开关
                                                        0：关闭箝位功能
                                                        1：开启箝位功能 */
    } reg;
} SOC_PASTAR_BUCK1_ADJ2_UNION;
#endif
#define SOC_PASTAR_BUCK1_ADJ2_buck1_sg_sel_START   (0)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_sg_sel_END     (1)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_sc_sel_START   (2)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_sc_sel_END     (3)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_shortn_START   (4)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_shortn_END     (4)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_ccm_ctr_START  (5)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_ccm_ctr_END    (5)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_dt_sel_START   (6)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_dt_sel_END     (6)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_clampen_START  (7)
#define SOC_PASTAR_BUCK1_ADJ2_buck1_clampen_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK1_ADJ3_UNION
 结构说明  : BUCK1_ADJ3 寄存器结构定义。地址偏移量:0x32，初值:0x00，宽度:8
 寄存器说明: BUCK1调节3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_ocp_shield : 1;  /* bit[0]  : BUCK1过流保护功能设置
                                                           0：有过流保护功能
                                                           1：无过流保护功能 */
        unsigned char  buck1_nmos_shut  : 1;  /* bit[1]  : 关闭NMOS管 */
        unsigned char  buck1_dmd_sel    : 2;  /* bit[2-3]: 设置DMD的检测电流值 */
        unsigned char  buck1_reserv     : 3;  /* bit[4-6]: 用于sensor电路
                                                           buck1_reserv<0> 控制采样检测时间
                                                           buck1_reserv<1> 控制采样环路环路带宽
                                                           buck1_reserv<2> 控制isn过冲 */
        unsigned char  reserved         : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_PASTAR_BUCK1_ADJ3_UNION;
#endif
#define SOC_PASTAR_BUCK1_ADJ3_buck1_ocp_shield_START  (0)
#define SOC_PASTAR_BUCK1_ADJ3_buck1_ocp_shield_END    (0)
#define SOC_PASTAR_BUCK1_ADJ3_buck1_nmos_shut_START   (1)
#define SOC_PASTAR_BUCK1_ADJ3_buck1_nmos_shut_END     (1)
#define SOC_PASTAR_BUCK1_ADJ3_buck1_dmd_sel_START     (2)
#define SOC_PASTAR_BUCK1_ADJ3_buck1_dmd_sel_END       (3)
#define SOC_PASTAR_BUCK1_ADJ3_buck1_reserv_START      (4)
#define SOC_PASTAR_BUCK1_ADJ3_buck1_reserv_END        (6)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK2_ADJ1_UNION
 结构说明  : BUCK2_ADJ1 寄存器结构定义。地址偏移量:0x36，初值:0x00，宽度:8
 寄存器说明: BUCK2调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_comp_adj : 8;  /* bit[0-7]: BUCK2稳定型调节寄存器
                                                          <7>: 设置EA输出高电位箝位点
                                                         1：1.8V
                                                         0：2.6V
                                                         <6>: 设置PFM工作的比较器的偏值电流
                                                         1：1uA;
                                                         0：0.5uA
                                                         <5>: 控制PFM工作模式电感的峰值电流点
                                                         0：增加峰值电流
                                                         1：默认值
                                                         <4：0>：用于调整EA的跨导及调零点的电阻 */
    } reg;
} SOC_PASTAR_BUCK2_ADJ1_UNION;
#endif
#define SOC_PASTAR_BUCK2_ADJ1_buck2_comp_adj_START  (0)
#define SOC_PASTAR_BUCK2_ADJ1_buck2_comp_adj_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK2_ADJ2_UNION
 结构说明  : BUCK2_ADJ2 寄存器结构定义。地址偏移量:0x37，初值:0x29，宽度:8
 寄存器说明: BUCK2调节2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_sg_sel  : 2;  /* bit[0-1]: 斜坡产生量调节 */
        unsigned char  buck2_sc_sel  : 2;  /* bit[2-3]: 斜坡补偿电阻调节 */
        unsigned char  buck2_shortn  : 1;  /* bit[4]  : buck2的短路保护功能信号
                                                        0：有短路保护
                                                        1：无短路保护 */
        unsigned char  buck2_ccm_ctr : 1;  /* bit[5]  : buck2的强制连续模式功能
                                                        0：强制连续模式功能开启
                                                        1：强制连续模式功能关闭 */
        unsigned char  buck2_dt_sel  : 1;  /* bit[6]  : BUCK2死区时间调节
                                                        0：默认
                                                        1：减小死区时间 */
        unsigned char  buck2_clampen : 1;  /* bit[7]  : VZ箝位功能开关
                                                        0：关闭箝位功能
                                                        1：开启箝位功能 */
    } reg;
} SOC_PASTAR_BUCK2_ADJ2_UNION;
#endif
#define SOC_PASTAR_BUCK2_ADJ2_buck2_sg_sel_START   (0)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_sg_sel_END     (1)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_sc_sel_START   (2)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_sc_sel_END     (3)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_shortn_START   (4)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_shortn_END     (4)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_ccm_ctr_START  (5)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_ccm_ctr_END    (5)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_dt_sel_START   (6)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_dt_sel_END     (6)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_clampen_START  (7)
#define SOC_PASTAR_BUCK2_ADJ2_buck2_clampen_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK2_ADJ3_UNION
 结构说明  : BUCK2_ADJ3 寄存器结构定义。地址偏移量:0x38，初值:0x00，宽度:8
 寄存器说明: BUCK2调节3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_ocp_shield : 1;  /* bit[0]  : BUCK2过流保护功能设置
                                                           0：有过流保护功能
                                                           1：无过流保护功能 */
        unsigned char  buck2_nmos_shut  : 1;  /* bit[1]  : 关闭NMOS管 */
        unsigned char  buck2_dmd_sel    : 2;  /* bit[2-3]: 设置DMD的检测电流值 */
        unsigned char  buck2_reserv     : 3;  /* bit[4-6]: 用于sensor电路
                                                           buck1_reserv<0> 控制采样检测时间
                                                           buck1_reserv<1> 控制采样环路环路带宽
                                                           buck1_reserv<2> 控制isn过冲 */
        unsigned char  reserved         : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_PASTAR_BUCK2_ADJ3_UNION;
#endif
#define SOC_PASTAR_BUCK2_ADJ3_buck2_ocp_shield_START  (0)
#define SOC_PASTAR_BUCK2_ADJ3_buck2_ocp_shield_END    (0)
#define SOC_PASTAR_BUCK2_ADJ3_buck2_nmos_shut_START   (1)
#define SOC_PASTAR_BUCK2_ADJ3_buck2_nmos_shut_END     (1)
#define SOC_PASTAR_BUCK2_ADJ3_buck2_dmd_sel_START     (2)
#define SOC_PASTAR_BUCK2_ADJ3_buck2_dmd_sel_END       (3)
#define SOC_PASTAR_BUCK2_ADJ3_buck2_reserv_START      (4)
#define SOC_PASTAR_BUCK2_ADJ3_buck2_reserv_END        (6)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK2_ADJ4_UNION
 结构说明  : BUCK2_ADJ4 寄存器结构定义。地址偏移量:0x39，初值:0x00，宽度:8
 寄存器说明: 'BUCK2调节4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_slope_gen : 2;  /* bit[0-1]: 调节斜坡补偿量 */
        unsigned char  reserved        : 6;  /* bit[2-7]: 保留 */
    } reg;
} SOC_PASTAR_BUCK2_ADJ4_UNION;
#endif
#define SOC_PASTAR_BUCK2_ADJ4_buck2_slope_gen_START  (0)
#define SOC_PASTAR_BUCK2_ADJ4_buck2_slope_gen_END    (1)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK_CTRL1_UNION
 结构说明  : BUCK_CTRL1 寄存器结构定义。地址偏移量:0x45，初值:0x08，宽度:8
 寄存器说明: BUCK频率控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck_osc_f : 5;  /* bit[0-4]: 控制buck频率,默认频率为2MHz */
        unsigned char  reserved   : 3;  /* bit[5-7]: 保留 */
    } reg;
} SOC_PASTAR_BUCK_CTRL1_UNION;
#endif
#define SOC_PASTAR_BUCK_CTRL1_buck_osc_f_START  (0)
#define SOC_PASTAR_BUCK_CTRL1_buck_osc_f_END    (4)


/*****************************************************************************
 结构名    : SOC_PASTAR_BUCK_CTRL2_UNION
 结构说明  : BUCK_CTRL2 寄存器结构定义。地址偏移量:0x46，初值:0x00，宽度:8
 寄存器说明: BUCK扩频控制2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck_osc_random : 8;  /* bit[0-7]: buck扩频控制 */
    } reg;
} SOC_PASTAR_BUCK_CTRL2_UNION;
#endif
#define SOC_PASTAR_BUCK_CTRL2_buck_osc_random_START  (0)
#define SOC_PASTAR_BUCK_CTRL2_buck_osc_random_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_SET_LDO1_UNION
 结构说明  : SET_LDO1 寄存器结构定义。地址偏移量:0x48，初值:0x08，宽度:8
 寄存器说明: LDO1电压调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo1 : 4;  /* bit[0-3]: LDO1 电压调节。 
                                                    0x0: 1.50 V;
                                                    0x1: 1.60 V;
                                                    0x2: 1.70 V;
                                                    0x3: 1.80 V; 
                                                    0x4: 2.20 V;
                                                    0x5: 2.50 V; 
                                                    0x6: 2.60 V;
                                                    0x7: 2.70 V;
                                                    0x8: 2.80 V;
                                                    0x9: 2.90 V;
                                                    0xA: 3.00 V;
                                                    0xB: 3.10 V;
                                                    0xC: 3.20 V;
                                                    0xD: 3.30 V;
                                                    0xE: 3.40 V; 
                                                    0xF: 3.50 V;  */
        unsigned char  reserved  : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_PASTAR_SET_LDO1_UNION;
#endif
#define SOC_PASTAR_SET_LDO1_vset_ldo1_START  (0)
#define SOC_PASTAR_SET_LDO1_vset_ldo1_END    (3)


/*****************************************************************************
 结构名    : SOC_PASTAR_SET_LDO2_UNION
 结构说明  : SET_LDO2 寄存器结构定义。地址偏移量:0x49，初值:0x0D，宽度:8
 寄存器说明: LDO2电压调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo2 : 4;  /* bit[0-3]: LDO2 电压调节。 
                                                    0x0: 1.50 V;
                                                    0x1: 1.60 V;
                                                    0x2: 1.70 V;
                                                    0x3: 1.80 V; 
                                                    0x4: 2.20 V;
                                                    0x5: 2.50 V; 
                                                    0x6: 2.60 V;
                                                    0x7: 2.70 V;
                                                    0x8: 2.80 V;
                                                    0x9: 2.90 V;
                                                    0xA: 3.00 V;
                                                    0xB: 3.10 V;
                                                    0xC: 3.20 V;
                                                    0xD: 3.30 V;
                                                    0xE: 3.40 V; 
                                                    0xF: 3.50 V;
                                                      */
        unsigned char  reserved  : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_PASTAR_SET_LDO2_UNION;
#endif
#define SOC_PASTAR_SET_LDO2_vset_ldo2_START  (0)
#define SOC_PASTAR_SET_LDO2_vset_ldo2_END    (3)


/*****************************************************************************
 结构名    : SOC_PASTAR_SET_LDO_CTRL_UNION
 结构说明  : SET_LDO_CTRL 寄存器结构定义。地址偏移量:0x4A，初值:0x01，宽度:8
 寄存器说明: LDO控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vs_delay_ldo_int    : 2;  /* bit[0-1]: 00:ECO-NORMAL切换时间交叠0us
                                                              01:ECO-NORMAL切换时间交叠30us
                                                              10:ECO-NORMAL切换时间交叠60us
                                                              11:ECO-NORMAL切换时间交叠90us */
        unsigned char  vs_en_ldo_delay_int : 2;  /* bit[2-3]: 00:从关闭到NORMAL切换时en_ldo的delay时间可选:
                                                              00:90us;
                                                              01:120us;
                                                              10:150us;
                                                              11:180us; */
        unsigned char  vs_ldo              : 3;  /* bit[4-6]: 300mA LDO 输出电压微调信号  */
        unsigned char  reserved            : 1;  /* bit[7]  : 保留。 */
    } reg;
} SOC_PASTAR_SET_LDO_CTRL_UNION;
#endif
#define SOC_PASTAR_SET_LDO_CTRL_vs_delay_ldo_int_START     (0)
#define SOC_PASTAR_SET_LDO_CTRL_vs_delay_ldo_int_END       (1)
#define SOC_PASTAR_SET_LDO_CTRL_vs_en_ldo_delay_int_START  (2)
#define SOC_PASTAR_SET_LDO_CTRL_vs_en_ldo_delay_int_END    (3)
#define SOC_PASTAR_SET_LDO_CTRL_vs_ldo_START               (4)
#define SOC_PASTAR_SET_LDO_CTRL_vs_ldo_END                 (6)


/*****************************************************************************
 结构名    : SOC_PASTAR_PA_AMP_CTRL_UNION
 结构说明  : PA_AMP_CTRL 寄存器结构定义。地址偏移量:0x4B，初值:0x10，宽度:8
 寄存器说明: PA_AMPLIFIER寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reg_cap    : 4;  /* bit[0-3]: 0000：接入电阻两端的电容值为0fp 0001：接入电阻两端的电容值为50fp 0011：接入电阻两端的电容值为100fp 0111：接入电阻两端的电容值为150fp 1111：接入电阻两端的电容值为200fp 
                                                     
                                                       */
        unsigned char  set_ib_amp : 1;  /* bit[4]  : 0:IB_AMP=0.25uA in APT mode 1:IB_AMP=0.50uA in APT mode */
        unsigned char  reserved   : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_PASTAR_PA_AMP_CTRL_UNION;
#endif
#define SOC_PASTAR_PA_AMP_CTRL_reg_cap_START     (0)
#define SOC_PASTAR_PA_AMP_CTRL_reg_cap_END       (3)
#define SOC_PASTAR_PA_AMP_CTRL_set_ib_amp_START  (4)
#define SOC_PASTAR_PA_AMP_CTRL_set_ib_amp_END    (4)


/*****************************************************************************
 结构名    : SOC_PASTAR_CLASSAB_CTRL_UNION
 结构说明  : CLASSAB_CTRL 寄存器结构定义。地址偏移量:0x4C，初值:0x03，宽度:8
 寄存器说明: CLASSAB控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ibd      : 3;  /* bit[0-2]: 011：接入3u
                                                   101：接入2u 
                                                   110：接入1u
                                                   001：接入5u
                                                   010：接入4u */
        unsigned char  reserved : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_PASTAR_CLASSAB_CTRL_UNION;
#endif
#define SOC_PASTAR_CLASSAB_CTRL_ibd_START       (0)
#define SOC_PASTAR_CLASSAB_CTRL_ibd_END         (2)


/*****************************************************************************
 结构名    : SOC_PASTAR_ILIM_CTRL1_UNION
 结构说明  : ILIM_CTRL1 寄存器结构定义。地址偏移量:0x4E，初值:0x00，宽度:8
 寄存器说明: ILIM_CTRL控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  lit_cur_set  : 3;  /* bit[0-2]: 000：选择500mA档位;
                                                       001：选择300mA档位(用于ATE TRIM);
                                                       010：选择200mA档位(用于ATE TRIM);
                                                       011：选择不限流档位;
                                                       100：选择1800mA档位;
                                                       101：选择1500mA档位;
                                                       110：选择1200mA档位;
                                                       111：选择900mA档位 ; 
                                                       
                                                         */
        unsigned char  clim_op_isel : 1;  /* bit[3]  : 0:运放偏置电流选择2.5uA;
                                                       1：运放偏置电流选择1.5uA; */
        unsigned char  pro_mode     : 1;  /* bit[4]  : 0:开启大电流保护模式; 
                                                       1:关闭大电流保护模式; */
        unsigned char  reserved     : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_PASTAR_ILIM_CTRL1_UNION;
#endif
#define SOC_PASTAR_ILIM_CTRL1_lit_cur_set_START   (0)
#define SOC_PASTAR_ILIM_CTRL1_lit_cur_set_END     (2)
#define SOC_PASTAR_ILIM_CTRL1_clim_op_isel_START  (3)
#define SOC_PASTAR_ILIM_CTRL1_clim_op_isel_END    (3)
#define SOC_PASTAR_ILIM_CTRL1_pro_mode_START      (4)
#define SOC_PASTAR_ILIM_CTRL1_pro_mode_END        (4)


/*****************************************************************************
 结构名    : SOC_PASTAR_ILIM_CTRL2_UNION
 结构说明  : ILIM_CTRL2 寄存器结构定义。地址偏移量:0x4F，初值:0x00，宽度:8
 寄存器说明: ILIM_CTRL控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  clim_vos : 4;  /* bit[0-3]: 限流精度trim信号;
                                                   
                                                     */
        unsigned char  clim_ctl : 4;  /* bit[4-7]: 限流精度trim信号; */
    } reg;
} SOC_PASTAR_ILIM_CTRL2_UNION;
#endif
#define SOC_PASTAR_ILIM_CTRL2_clim_vos_START  (0)
#define SOC_PASTAR_ILIM_CTRL2_clim_vos_END    (3)
#define SOC_PASTAR_ILIM_CTRL2_clim_ctl_START  (4)
#define SOC_PASTAR_ILIM_CTRL2_clim_ctl_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_BANDGAP_UNION
 结构说明  : BANDGAP 寄存器结构定义。地址偏移量:0x55，初值:0x04，宽度:8
 寄存器说明: BANDGAP控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  test_bg  : 2;  /* bit[0-1]: TEST_BG<1>:
                                                   1:表示current switch开
                                                   0：表示current switch关
                                                   TEST_BG<0>：
                                                   1：表示进入测试模式
                                                   0：表示进入正常模式
                                                   
                                                     */
        unsigned char  en_tmp   : 1;  /* bit[2]  : 0:THSD过温保护模块关闭
                                                   1:THSD过温保护模块开启 */
        unsigned char  reserved : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_PASTAR_BANDGAP_UNION;
#endif
#define SOC_PASTAR_BANDGAP_test_bg_START   (0)
#define SOC_PASTAR_BANDGAP_test_bg_END     (1)
#define SOC_PASTAR_BANDGAP_en_tmp_START    (2)
#define SOC_PASTAR_BANDGAP_en_tmp_END      (2)


/*****************************************************************************
 结构名    : SOC_PASTAR_ANA_IN_UNION
 结构说明  : ANA_IN 寄存器结构定义。地址偏移量:0x56，初值:0x00，宽度:8
 寄存器说明: PMU模拟部分输入寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ana_in_2d : 8;  /* bit[0-7]: PMU模拟部分输入，8bits可通过寄存器读出，保留使用。 */
    } reg;
} SOC_PASTAR_ANA_IN_UNION;
#endif
#define SOC_PASTAR_ANA_IN_ana_in_2d_START  (0)
#define SOC_PASTAR_ANA_IN_ana_in_2d_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_RESERVED1_UNION
 结构说明  : RESERVED1 寄存器结构定义。地址偏移量:0x57，初值:0x00，宽度:8
 寄存器说明: 保留1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved  : 8;  /* bit[0-7]: 测试用及备用未定。 */
    } reg;
} SOC_PASTAR_RESERVED1_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PASTAR_RESERVED2_UNION
 结构说明  : RESERVED2 寄存器结构定义。地址偏移量:0x58，初值:0x00，宽度:8
 寄存器说明: 保留2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved  : 8;  /* bit[0-7]: 测试用及备用未定。 */
    } reg;
} SOC_PASTAR_RESERVED2_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PASTAR_RESERVED0_UNION
 结构说明  : RESERVED0 寄存器结构定义。地址偏移量:0x59，初值:0x00，宽度:8
 寄存器说明: 保留0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved  : 8;  /* bit[0-7]: 测试用及备用未定。 */
    } reg;
} SOC_PASTAR_RESERVED0_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_PASTAR_SET_OTP_UNION
 结构说明  : SET_OTP 寄存器结构定义。地址偏移量:0x5A，初值:0x00，宽度:8
 寄存器说明: OTP设置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_por_pulse  : 1;  /* bit[0]  : 0：不发起脉冲；
                                                         1：当otp_read_mask=1时，发起一个31000ns的高电平脉冲。 */
        unsigned char  otp_read_mask  : 1;  /* bit[1]  : 0：由otp_por信号控制OTP的读操作；
                                                         1:由otp_por_pulse信号控制OTP的读操作。 */
        unsigned char  otp_pwe_pulse  : 1;  /* bit[2]  : 0：不发起脉冲；
                                                         1：当otp_write_mask=1时，发起一个310us的高电平脉冲。 */
        unsigned char  otp_write_mask : 1;  /* bit[3]  : 0:由otp_pwe信号控制OTP的写操作。
                                                         1:由otp_pwe_pulse信号控制OTP的写操作； */
        unsigned char  reserved       : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_PASTAR_SET_OTP_UNION;
#endif
#define SOC_PASTAR_SET_OTP_otp_por_pulse_START   (0)
#define SOC_PASTAR_SET_OTP_otp_por_pulse_END     (0)
#define SOC_PASTAR_SET_OTP_otp_read_mask_START   (1)
#define SOC_PASTAR_SET_OTP_otp_read_mask_END     (1)
#define SOC_PASTAR_SET_OTP_otp_pwe_pulse_START   (2)
#define SOC_PASTAR_SET_OTP_otp_pwe_pulse_END     (2)
#define SOC_PASTAR_SET_OTP_otp_write_mask_START  (3)
#define SOC_PASTAR_SET_OTP_otp_write_mask_END    (3)


/*****************************************************************************
 结构名    : SOC_PASTAR_OTP_CTRL_UNION
 结构说明  : OTP_CTRL 寄存器结构定义。地址偏移量:0x5B，初值:0x00，宽度:8
 寄存器说明: OTP控制信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pwe_int : 1;  /* bit[0]  : OTP写控制信号。 */
        unsigned char  otp_por_int : 1;  /* bit[1]  : OTP读控制信号。 */
        unsigned char  otp_pa      : 2;  /* bit[2-3]: OTP的地址信号。 */
        unsigned char  otp_ptm     : 2;  /* bit[4-5]: 测试模式使能信号。 */
        unsigned char  otp_pprog   : 1;  /* bit[6]  : 编程模式使能信号。 */
        unsigned char  reserved    : 1;  /* bit[7]  : 保留。 */
    } reg;
} SOC_PASTAR_OTP_CTRL_UNION;
#endif
#define SOC_PASTAR_OTP_CTRL_otp_pwe_int_START  (0)
#define SOC_PASTAR_OTP_CTRL_otp_pwe_int_END    (0)
#define SOC_PASTAR_OTP_CTRL_otp_por_int_START  (1)
#define SOC_PASTAR_OTP_CTRL_otp_por_int_END    (1)
#define SOC_PASTAR_OTP_CTRL_otp_pa_START       (2)
#define SOC_PASTAR_OTP_CTRL_otp_pa_END         (3)
#define SOC_PASTAR_OTP_CTRL_otp_ptm_START      (4)
#define SOC_PASTAR_OTP_CTRL_otp_ptm_END        (5)
#define SOC_PASTAR_OTP_CTRL_otp_pprog_START    (6)
#define SOC_PASTAR_OTP_CTRL_otp_pprog_END      (6)


/*****************************************************************************
 结构名    : SOC_PASTAR_OTP_PDIN_UNION
 结构说明  : OTP_PDIN 寄存器结构定义。地址偏移量:0x5C，初值:0x00，宽度:8
 寄存器说明: OTP写入值寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pdin : 8;  /* bit[0-7]: OTP的输入值寄存器。 */
    } reg;
} SOC_PASTAR_OTP_PDIN_UNION;
#endif
#define SOC_PASTAR_OTP_PDIN_otp_pdin_START  (0)
#define SOC_PASTAR_OTP_PDIN_otp_pdin_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_OTP_PDOB0_UNION
 结构说明  : OTP_PDOB0 寄存器结构定义。地址偏移量:0x5D，初值:0x00，宽度:8
 寄存器说明: OTP读出值0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pdob0 : 8;  /* bit[0-7]: OTP_PDOB[7:0]。 */
    } reg;
} SOC_PASTAR_OTP_PDOB0_UNION;
#endif
#define SOC_PASTAR_OTP_PDOB0_otp_pdob0_START  (0)
#define SOC_PASTAR_OTP_PDOB0_otp_pdob0_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_OTP_PDOB1_UNION
 结构说明  : OTP_PDOB1 寄存器结构定义。地址偏移量:0x5E，初值:0x00，宽度:8
 寄存器说明: OTP读出值1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pdob1 : 8;  /* bit[0-7]: OTP_PDOB[15:8]。 */
    } reg;
} SOC_PASTAR_OTP_PDOB1_UNION;
#endif
#define SOC_PASTAR_OTP_PDOB1_otp_pdob1_START  (0)
#define SOC_PASTAR_OTP_PDOB1_otp_pdob1_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_OTP_PDOB2_UNION
 结构说明  : OTP_PDOB2 寄存器结构定义。地址偏移量:0x5F，初值:0x00，宽度:8
 寄存器说明: OTP读出值2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pdob2 : 8;  /* bit[0-7]: OTP_PDOB[23:16]。 */
    } reg;
} SOC_PASTAR_OTP_PDOB2_UNION;
#endif
#define SOC_PASTAR_OTP_PDOB2_otp_pdob2_START  (0)
#define SOC_PASTAR_OTP_PDOB2_otp_pdob2_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_OTP_PDOB3_UNION
 结构说明  : OTP_PDOB3 寄存器结构定义。地址偏移量:0x60，初值:0x00，宽度:8
 寄存器说明: OTP读出值3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pdob3 : 8;  /* bit[0-7]: OTP_PDOB[31:24]。 */
    } reg;
} SOC_PASTAR_OTP_PDOB3_UNION;
#endif
#define SOC_PASTAR_OTP_PDOB3_otp_pdob3_START  (0)
#define SOC_PASTAR_OTP_PDOB3_otp_pdob3_END    (7)


/*****************************************************************************
 结构名    : SOC_PASTAR_NO_PWR_REG_RO_UNION
 结构说明  : NO_PWR_REG_RO 寄存器结构定义。地址偏移量:0x6A，初值:0x80，宽度:8
 寄存器说明: 非下电信息保存寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ldo1_masked  : 1;  /* bit[0]: 0: LDO1没有过流；
                                                         1：LDO1过流； */
        unsigned char  ocp_ldo2_masked  : 1;  /* bit[1]: 0: LDO2没有过流；
                                                         1：LDO2过流； */
        unsigned char  ocp_buck0_masked : 1;  /* bit[2]: 0: buck0没有过流；
                                                         1：buck0过流； */
        unsigned char  ocp_buck1_masked : 1;  /* bit[3]: 0: buck1没有过流；
                                                         1：buck1过流； */
        unsigned char  ocp_buck2_masked : 1;  /* bit[4]: 0: buck2没有过流；
                                                         1：buck2过流； */
        unsigned char  otmp150_d3r      : 1;  /* bit[5]: 0: 温度没有超过150°；
                                                         1： 温度超过150° 3ms ； */
        unsigned char  otmp125_d3r      : 1;  /* bit[6]: 0: 温度没有超过125°；
                                                         1： 温度超过125° 3ms ； */
        unsigned char  vdd_en_flag      : 1;  /* bit[7]: VDD_EN flag信号  */
    } reg;
} SOC_PASTAR_NO_PWR_REG_RO_UNION;
#endif
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_ldo1_masked_START   (0)
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_ldo1_masked_END     (0)
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_ldo2_masked_START   (1)
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_ldo2_masked_END     (1)
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_buck0_masked_START  (2)
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_buck0_masked_END    (2)
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_buck1_masked_START  (3)
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_buck1_masked_END    (3)
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_buck2_masked_START  (4)
#define SOC_PASTAR_NO_PWR_REG_RO_ocp_buck2_masked_END    (4)
#define SOC_PASTAR_NO_PWR_REG_RO_otmp150_d3r_START       (5)
#define SOC_PASTAR_NO_PWR_REG_RO_otmp150_d3r_END         (5)
#define SOC_PASTAR_NO_PWR_REG_RO_otmp125_d3r_START       (6)
#define SOC_PASTAR_NO_PWR_REG_RO_otmp125_d3r_END         (6)
#define SOC_PASTAR_NO_PWR_REG_RO_vdd_en_flag_START       (7)
#define SOC_PASTAR_NO_PWR_REG_RO_vdd_en_flag_END         (7)






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

#endif /* end of soc_pastar_interface.h */
