

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_TZPC_INTERFACE_H__
#define __SOC_TZPC_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) TZPC
 ****************************************************************************/
/* 寄存器说明：TZMA安全size设置
   位域定义UNION结构:  SOC_TZPC_TZPCR0SIZE_UNION */
#define SOC_TZPC_TZPCR0SIZE_ADDR(base)                ((base) + (0x0000))

/* 寄存器说明：安全设置状态寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT0STAT_UNION */
#define SOC_TZPC_TZPCDECPROT0STAT_ADDR(base)          ((base) + (0x0800))

/* 寄存器说明：非安全设置寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT0SET_UNION */
#define SOC_TZPC_TZPCDECPROT0SET_ADDR(base)           ((base) + (0x0804))

/* 寄存器说明：安全设置寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT0CLR_UNION */
#define SOC_TZPC_TZPCDECPROT0CLR_ADDR(base)           ((base) + (0x0808))

/* 寄存器说明：安全设置状态寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT1STAT_UNION */
#define SOC_TZPC_TZPCDECPROT1STAT_ADDR(base)          ((base) + (0x080C))

/* 寄存器说明：非安全设置寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT1SET_UNION */
#define SOC_TZPC_TZPCDECPROT1SET_ADDR(base)           ((base) + (0x0810))

/* 寄存器说明：安全设置寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT1CLR_UNION */
#define SOC_TZPC_TZPCDECPROT1CLR_ADDR(base)           ((base) + (0x0814))

/* 寄存器说明：安全设置状态寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT2STAT_UNION */
#define SOC_TZPC_TZPCDECPROT2STAT_ADDR(base)          ((base) + (0x0818))

/* 寄存器说明：非安全设置寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT2SET_UNION */
#define SOC_TZPC_TZPCDECPROT2SET_ADDR(base)           ((base) + (0x081C))

/* 寄存器说明：安全设置寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT2CLR_UNION */
#define SOC_TZPC_TZPCDECPROT2CLR_ADDR(base)           ((base) + (0x0820))

/* 寄存器说明：安全设置状态寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT3STAT_UNION */
#define SOC_TZPC_TZPCDECPROT3STAT_ADDR(base)          ((base) + (0x0824))

/* 寄存器说明：非安全设置寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT3SET_UNION */
#define SOC_TZPC_TZPCDECPROT3SET_ADDR(base)           ((base) + (0x0828))

/* 寄存器说明：安全设置寄存器
   位域定义UNION结构:  SOC_TZPC_TZPCDECPROT3CLR_UNION */
#define SOC_TZPC_TZPCDECPROT3CLR_ADDR(base)           ((base) + (0x082C))





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
                     (1/1) TZPC
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_TZPC_TZPCR0SIZE_UNION
 结构说明  : TZPCR0SIZE 寄存器结构定义。地址偏移量:0x0000，初值:0x00000000，宽度:32
 寄存器说明: TZMA安全size设置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tzpcr0size : 10; /* bit[0-9]  : 0x00000000 = no secure region
                                                      0x00000001 = 4KB secure region
                                                      0x00000002 = 8KB secure region
                                                      …
                                                      0x000001FF = 2044KB secure region.
                                                      0x00000200 or above sets the entire RAM to secure regardless of size */
        unsigned int  reserved   : 22; /* bit[10-31]: reserved */
    } reg;
} SOC_TZPC_TZPCR0SIZE_UNION;
#endif
#define SOC_TZPC_TZPCR0SIZE_tzpcr0size_START  (0)
#define SOC_TZPC_TZPCR0SIZE_tzpcr0size_END    (9)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT0STAT_UNION
 结构说明  : TZPCDECPROT0STAT 寄存器结构定义。地址偏移量:0x0800，初值:0xFFFFFFFF，宽度:32
 寄存器说明: 安全设置状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  g2d_cfg_port        : 1;  /* bit[0] : G2D配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  sio0_cfg_port       : 1;  /* bit[1] : SIO0配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  sio1_cfg_port       : 1;  /* bit[2] : SIO1配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  sio2_cfg_port       : 1;  /* bit[3] : SIO2配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  sio3_cfg_port       : 1;  /* bit[4] : SIO3配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  mmc0_cfg_port       : 1;  /* bit[5] : MMC0配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  mmc1_cfg_port       : 1;  /* bit[6] : MMC1配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  mmc2_cfg_port       : 1;  /* bit[7] : MMC2配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  dsi_cfg_port        : 1;  /* bit[8] : DSI配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  vdec_cfg_port       : 1;  /* bit[9] : VDEC配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  venc_cfg_port       : 1;  /* bit[10]: VENC配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  vpp_cfg_port        : 1;  /* bit[11]: VPP配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  g3d_cfg_port        : 1;  /* bit[12]: G3D配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  pwm0_cfg_port       : 1;  /* bit[13]: PWM0配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  pwm1_cfg_port       : 1;  /* bit[14]: PWM1配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off4_cfg_port  : 1;  /* bit[15]: GPIO_OFF4 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off5_cfg_port  : 1;  /* bit[16]: GPIO_OFF5 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off6_cfg_port  : 1;  /* bit[17]: GPIO_OFF6 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off7_cfg_port  : 1;  /* bit[18]: GPIO_OFF7 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off8_cfg_port  : 1;  /* bit[19]: GPIO_OFF8 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off9_cfg_port  : 1;  /* bit[20]: GPIO_OFF9 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off10_cfg_port : 1;  /* bit[21]: GPIO_OFF10 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off11_cfg_port : 1;  /* bit[22]: GPIO_OFF11 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off12_cfg_port : 1;  /* bit[23]: GPIO_OFF12 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off13_cfg_port : 1;  /* bit[24]: GPIO_OFF13 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off14_cfg_port : 1;  /* bit[25]: GPIO_OFF14 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off15_cfg_port : 1;  /* bit[26]: GPIO_OFF15 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off16_cfg_port : 1;  /* bit[27]: GPIO_OFF16 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off17_cfg_port : 1;  /* bit[28]: GPIO_OFF17 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off18_cfg_port : 1;  /* bit[29]: GPIO_OFF18 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  gpio_off19_cfg_port : 1;  /* bit[30]: GPIO_OFF19 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
        unsigned int  ioc_off_cfg_port    : 1;  /* bit[31]: IOC OFF 配置接口安全设置状态：
                                                            0：安全
                                                            1：非安全 */
    } reg;
} SOC_TZPC_TZPCDECPROT0STAT_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT0STAT_g2d_cfg_port_START         (0)
#define SOC_TZPC_TZPCDECPROT0STAT_g2d_cfg_port_END           (0)
#define SOC_TZPC_TZPCDECPROT0STAT_sio0_cfg_port_START        (1)
#define SOC_TZPC_TZPCDECPROT0STAT_sio0_cfg_port_END          (1)
#define SOC_TZPC_TZPCDECPROT0STAT_sio1_cfg_port_START        (2)
#define SOC_TZPC_TZPCDECPROT0STAT_sio1_cfg_port_END          (2)
#define SOC_TZPC_TZPCDECPROT0STAT_sio2_cfg_port_START        (3)
#define SOC_TZPC_TZPCDECPROT0STAT_sio2_cfg_port_END          (3)
#define SOC_TZPC_TZPCDECPROT0STAT_sio3_cfg_port_START        (4)
#define SOC_TZPC_TZPCDECPROT0STAT_sio3_cfg_port_END          (4)
#define SOC_TZPC_TZPCDECPROT0STAT_mmc0_cfg_port_START        (5)
#define SOC_TZPC_TZPCDECPROT0STAT_mmc0_cfg_port_END          (5)
#define SOC_TZPC_TZPCDECPROT0STAT_mmc1_cfg_port_START        (6)
#define SOC_TZPC_TZPCDECPROT0STAT_mmc1_cfg_port_END          (6)
#define SOC_TZPC_TZPCDECPROT0STAT_mmc2_cfg_port_START        (7)
#define SOC_TZPC_TZPCDECPROT0STAT_mmc2_cfg_port_END          (7)
#define SOC_TZPC_TZPCDECPROT0STAT_dsi_cfg_port_START         (8)
#define SOC_TZPC_TZPCDECPROT0STAT_dsi_cfg_port_END           (8)
#define SOC_TZPC_TZPCDECPROT0STAT_vdec_cfg_port_START        (9)
#define SOC_TZPC_TZPCDECPROT0STAT_vdec_cfg_port_END          (9)
#define SOC_TZPC_TZPCDECPROT0STAT_venc_cfg_port_START        (10)
#define SOC_TZPC_TZPCDECPROT0STAT_venc_cfg_port_END          (10)
#define SOC_TZPC_TZPCDECPROT0STAT_vpp_cfg_port_START         (11)
#define SOC_TZPC_TZPCDECPROT0STAT_vpp_cfg_port_END           (11)
#define SOC_TZPC_TZPCDECPROT0STAT_g3d_cfg_port_START         (12)
#define SOC_TZPC_TZPCDECPROT0STAT_g3d_cfg_port_END           (12)
#define SOC_TZPC_TZPCDECPROT0STAT_pwm0_cfg_port_START        (13)
#define SOC_TZPC_TZPCDECPROT0STAT_pwm0_cfg_port_END          (13)
#define SOC_TZPC_TZPCDECPROT0STAT_pwm1_cfg_port_START        (14)
#define SOC_TZPC_TZPCDECPROT0STAT_pwm1_cfg_port_END          (14)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off4_cfg_port_START   (15)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off4_cfg_port_END     (15)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off5_cfg_port_START   (16)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off5_cfg_port_END     (16)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off6_cfg_port_START   (17)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off6_cfg_port_END     (17)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off7_cfg_port_START   (18)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off7_cfg_port_END     (18)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off8_cfg_port_START   (19)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off8_cfg_port_END     (19)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off9_cfg_port_START   (20)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off9_cfg_port_END     (20)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off10_cfg_port_START  (21)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off10_cfg_port_END    (21)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off11_cfg_port_START  (22)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off11_cfg_port_END    (22)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off12_cfg_port_START  (23)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off12_cfg_port_END    (23)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off13_cfg_port_START  (24)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off13_cfg_port_END    (24)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off14_cfg_port_START  (25)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off14_cfg_port_END    (25)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off15_cfg_port_START  (26)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off15_cfg_port_END    (26)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off16_cfg_port_START  (27)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off16_cfg_port_END    (27)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off17_cfg_port_START  (28)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off17_cfg_port_END    (28)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off18_cfg_port_START  (29)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off18_cfg_port_END    (29)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off19_cfg_port_START  (30)
#define SOC_TZPC_TZPCDECPROT0STAT_gpio_off19_cfg_port_END    (30)
#define SOC_TZPC_TZPCDECPROT0STAT_ioc_off_cfg_port_START     (31)
#define SOC_TZPC_TZPCDECPROT0STAT_ioc_off_cfg_port_END       (31)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT0SET_UNION
 结构说明  : TZPCDECPROT0SET 寄存器结构定义。地址偏移量:0x0804，初值:0x00000000，宽度:32
 寄存器说明: 非安全设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  g2d_cfg_port        : 1;  /* bit[0] : G2D配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  sio0_cfg_port       : 1;  /* bit[1] : SIO0配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  sio1_cfg_port       : 1;  /* bit[2] : SIO1配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  sio2_cfg_port       : 1;  /* bit[3] : SIO2配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  sio3_cfg_port       : 1;  /* bit[4] : SIO3配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  mmc0_cfg_port       : 1;  /* bit[5] : MMC0配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  mmc1_cfg_port       : 1;  /* bit[6] : MMC1配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  mmc2_cfg_port       : 1;  /* bit[7] : MMC2配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  dsi_cfg_port        : 1;  /* bit[8] : DSI配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  vdec_cfg_port       : 1;  /* bit[9] : VDEC配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  venc_cfg_port       : 1;  /* bit[10]: VENC配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  vpp_cfg_port        : 1;  /* bit[11]: VPP配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  g3d_cfg_port        : 1;  /* bit[12]: G3D配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  pwm0_cfg_port       : 1;  /* bit[13]: PWM0配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  pwm1_cfg_port       : 1;  /* bit[14]: PWM1配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off4_cfg_port  : 1;  /* bit[15]: GPIO_OFF4 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off5_cfg_port  : 1;  /* bit[16]: GPIO_OFF5 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off6_cfg_port  : 1;  /* bit[17]: GPIO_OFF6 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off7_cfg_port  : 1;  /* bit[18]: GPIO_OFF7 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off8_cfg_port  : 1;  /* bit[19]: GPIO_OFF8 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off9_cfg_port  : 1;  /* bit[20]: GPIO_OFF9 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off10_cfg_port : 1;  /* bit[21]: GPIO_OFF10 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off11_cfg_port : 1;  /* bit[22]: GPIO_OFF11 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off12_cfg_port : 1;  /* bit[23]: GPIO_OFF12 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off13_cfg_port : 1;  /* bit[24]: GPIO_OFF13 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off14_cfg_port : 1;  /* bit[25]: GPIO_OFF14 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off15_cfg_port : 1;  /* bit[26]: GPIO_OFF15 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off16_cfg_port : 1;  /* bit[27]: GPIO_OFF16 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off17_cfg_port : 1;  /* bit[28]: GPIO_OFF17 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off18_cfg_port : 1;  /* bit[29]: GPIO_OFF18 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  gpio_off19_cfg_port : 1;  /* bit[30]: GPIO_OFF19 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
        unsigned int  ioc_off_cfg_port    : 1;  /* bit[31]: IOC OFF 配置接口非安全设置：
                                                            0：无效
                                                            1：设置为非安全 */
    } reg;
} SOC_TZPC_TZPCDECPROT0SET_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT0SET_g2d_cfg_port_START         (0)
#define SOC_TZPC_TZPCDECPROT0SET_g2d_cfg_port_END           (0)
#define SOC_TZPC_TZPCDECPROT0SET_sio0_cfg_port_START        (1)
#define SOC_TZPC_TZPCDECPROT0SET_sio0_cfg_port_END          (1)
#define SOC_TZPC_TZPCDECPROT0SET_sio1_cfg_port_START        (2)
#define SOC_TZPC_TZPCDECPROT0SET_sio1_cfg_port_END          (2)
#define SOC_TZPC_TZPCDECPROT0SET_sio2_cfg_port_START        (3)
#define SOC_TZPC_TZPCDECPROT0SET_sio2_cfg_port_END          (3)
#define SOC_TZPC_TZPCDECPROT0SET_sio3_cfg_port_START        (4)
#define SOC_TZPC_TZPCDECPROT0SET_sio3_cfg_port_END          (4)
#define SOC_TZPC_TZPCDECPROT0SET_mmc0_cfg_port_START        (5)
#define SOC_TZPC_TZPCDECPROT0SET_mmc0_cfg_port_END          (5)
#define SOC_TZPC_TZPCDECPROT0SET_mmc1_cfg_port_START        (6)
#define SOC_TZPC_TZPCDECPROT0SET_mmc1_cfg_port_END          (6)
#define SOC_TZPC_TZPCDECPROT0SET_mmc2_cfg_port_START        (7)
#define SOC_TZPC_TZPCDECPROT0SET_mmc2_cfg_port_END          (7)
#define SOC_TZPC_TZPCDECPROT0SET_dsi_cfg_port_START         (8)
#define SOC_TZPC_TZPCDECPROT0SET_dsi_cfg_port_END           (8)
#define SOC_TZPC_TZPCDECPROT0SET_vdec_cfg_port_START        (9)
#define SOC_TZPC_TZPCDECPROT0SET_vdec_cfg_port_END          (9)
#define SOC_TZPC_TZPCDECPROT0SET_venc_cfg_port_START        (10)
#define SOC_TZPC_TZPCDECPROT0SET_venc_cfg_port_END          (10)
#define SOC_TZPC_TZPCDECPROT0SET_vpp_cfg_port_START         (11)
#define SOC_TZPC_TZPCDECPROT0SET_vpp_cfg_port_END           (11)
#define SOC_TZPC_TZPCDECPROT0SET_g3d_cfg_port_START         (12)
#define SOC_TZPC_TZPCDECPROT0SET_g3d_cfg_port_END           (12)
#define SOC_TZPC_TZPCDECPROT0SET_pwm0_cfg_port_START        (13)
#define SOC_TZPC_TZPCDECPROT0SET_pwm0_cfg_port_END          (13)
#define SOC_TZPC_TZPCDECPROT0SET_pwm1_cfg_port_START        (14)
#define SOC_TZPC_TZPCDECPROT0SET_pwm1_cfg_port_END          (14)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off4_cfg_port_START   (15)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off4_cfg_port_END     (15)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off5_cfg_port_START   (16)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off5_cfg_port_END     (16)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off6_cfg_port_START   (17)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off6_cfg_port_END     (17)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off7_cfg_port_START   (18)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off7_cfg_port_END     (18)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off8_cfg_port_START   (19)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off8_cfg_port_END     (19)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off9_cfg_port_START   (20)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off9_cfg_port_END     (20)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off10_cfg_port_START  (21)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off10_cfg_port_END    (21)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off11_cfg_port_START  (22)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off11_cfg_port_END    (22)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off12_cfg_port_START  (23)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off12_cfg_port_END    (23)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off13_cfg_port_START  (24)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off13_cfg_port_END    (24)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off14_cfg_port_START  (25)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off14_cfg_port_END    (25)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off15_cfg_port_START  (26)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off15_cfg_port_END    (26)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off16_cfg_port_START  (27)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off16_cfg_port_END    (27)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off17_cfg_port_START  (28)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off17_cfg_port_END    (28)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off18_cfg_port_START  (29)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off18_cfg_port_END    (29)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off19_cfg_port_START  (30)
#define SOC_TZPC_TZPCDECPROT0SET_gpio_off19_cfg_port_END    (30)
#define SOC_TZPC_TZPCDECPROT0SET_ioc_off_cfg_port_START     (31)
#define SOC_TZPC_TZPCDECPROT0SET_ioc_off_cfg_port_END       (31)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT0CLR_UNION
 结构说明  : TZPCDECPROT0CLR 寄存器结构定义。地址偏移量:0x0808，初值:0x00000000，宽度:32
 寄存器说明: 安全设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  g2d_cfg_port        : 1;  /* bit[0] : G2D配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  sio0_cfg_port       : 1;  /* bit[1] : SIO0配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  sio1_cfg_port       : 1;  /* bit[2] : SIO1配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  sio2_cfg_port       : 1;  /* bit[3] : SIO2配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  sio3_cfg_port       : 1;  /* bit[4] : SIO3配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  mmc0_cfg_port       : 1;  /* bit[5] : MMC0配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  mmc1_cfg_port       : 1;  /* bit[6] : MMC1配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  mmc2_cfg_port       : 1;  /* bit[7] : MMC2配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  dsi_cfg_port        : 1;  /* bit[8] : DSI配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  vdec_cfg_port       : 1;  /* bit[9] : VDEC配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  venc_cfg_port       : 1;  /* bit[10]: VENC配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  vpp_cfg_port        : 1;  /* bit[11]: VPP配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  g3d_cfg_port        : 1;  /* bit[12]: G3D配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  pwm0_cfg_port       : 1;  /* bit[13]: PWM0配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  pwm1_cfg_port       : 1;  /* bit[14]: PWM1配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off4_cfg_port  : 1;  /* bit[15]: GPIO_OFF4 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off5_cfg_port  : 1;  /* bit[16]: GPIO_OFF5 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off6_cfg_port  : 1;  /* bit[17]: GPIO_OFF6 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off7_cfg_port  : 1;  /* bit[18]: GPIO_OFF7 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off8_cfg_port  : 1;  /* bit[19]: GPIO_OFF8 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off9_cfg_port  : 1;  /* bit[20]: GPIO_OFF9 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off10_cfg_port : 1;  /* bit[21]: GPIO_OFF10 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off11_cfg_port : 1;  /* bit[22]: GPIO_OFF11 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off12_cfg_port : 1;  /* bit[23]: GPIO_OFF12 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off13_cfg_port : 1;  /* bit[24]: GPIO_OFF13 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off14_cfg_port : 1;  /* bit[25]: GPIO_OFF14 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off15_cfg_port : 1;  /* bit[26]: GPIO_OFF15 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off16_cfg_port : 1;  /* bit[27]: GPIO_OFF16 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off17_cfg_port : 1;  /* bit[28]: GPIO_OFF17 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off18_cfg_port : 1;  /* bit[29]: GPIO_OFF18 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  gpio_off19_cfg_port : 1;  /* bit[30]: GPIO_OFF19 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
        unsigned int  ioc_off_cfg_port    : 1;  /* bit[31]: IOC OFF 配置接口安全设置：
                                                            0：无效
                                                            1：设置为安全 */
    } reg;
} SOC_TZPC_TZPCDECPROT0CLR_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT0CLR_g2d_cfg_port_START         (0)
#define SOC_TZPC_TZPCDECPROT0CLR_g2d_cfg_port_END           (0)
#define SOC_TZPC_TZPCDECPROT0CLR_sio0_cfg_port_START        (1)
#define SOC_TZPC_TZPCDECPROT0CLR_sio0_cfg_port_END          (1)
#define SOC_TZPC_TZPCDECPROT0CLR_sio1_cfg_port_START        (2)
#define SOC_TZPC_TZPCDECPROT0CLR_sio1_cfg_port_END          (2)
#define SOC_TZPC_TZPCDECPROT0CLR_sio2_cfg_port_START        (3)
#define SOC_TZPC_TZPCDECPROT0CLR_sio2_cfg_port_END          (3)
#define SOC_TZPC_TZPCDECPROT0CLR_sio3_cfg_port_START        (4)
#define SOC_TZPC_TZPCDECPROT0CLR_sio3_cfg_port_END          (4)
#define SOC_TZPC_TZPCDECPROT0CLR_mmc0_cfg_port_START        (5)
#define SOC_TZPC_TZPCDECPROT0CLR_mmc0_cfg_port_END          (5)
#define SOC_TZPC_TZPCDECPROT0CLR_mmc1_cfg_port_START        (6)
#define SOC_TZPC_TZPCDECPROT0CLR_mmc1_cfg_port_END          (6)
#define SOC_TZPC_TZPCDECPROT0CLR_mmc2_cfg_port_START        (7)
#define SOC_TZPC_TZPCDECPROT0CLR_mmc2_cfg_port_END          (7)
#define SOC_TZPC_TZPCDECPROT0CLR_dsi_cfg_port_START         (8)
#define SOC_TZPC_TZPCDECPROT0CLR_dsi_cfg_port_END           (8)
#define SOC_TZPC_TZPCDECPROT0CLR_vdec_cfg_port_START        (9)
#define SOC_TZPC_TZPCDECPROT0CLR_vdec_cfg_port_END          (9)
#define SOC_TZPC_TZPCDECPROT0CLR_venc_cfg_port_START        (10)
#define SOC_TZPC_TZPCDECPROT0CLR_venc_cfg_port_END          (10)
#define SOC_TZPC_TZPCDECPROT0CLR_vpp_cfg_port_START         (11)
#define SOC_TZPC_TZPCDECPROT0CLR_vpp_cfg_port_END           (11)
#define SOC_TZPC_TZPCDECPROT0CLR_g3d_cfg_port_START         (12)
#define SOC_TZPC_TZPCDECPROT0CLR_g3d_cfg_port_END           (12)
#define SOC_TZPC_TZPCDECPROT0CLR_pwm0_cfg_port_START        (13)
#define SOC_TZPC_TZPCDECPROT0CLR_pwm0_cfg_port_END          (13)
#define SOC_TZPC_TZPCDECPROT0CLR_pwm1_cfg_port_START        (14)
#define SOC_TZPC_TZPCDECPROT0CLR_pwm1_cfg_port_END          (14)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off4_cfg_port_START   (15)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off4_cfg_port_END     (15)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off5_cfg_port_START   (16)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off5_cfg_port_END     (16)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off6_cfg_port_START   (17)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off6_cfg_port_END     (17)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off7_cfg_port_START   (18)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off7_cfg_port_END     (18)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off8_cfg_port_START   (19)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off8_cfg_port_END     (19)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off9_cfg_port_START   (20)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off9_cfg_port_END     (20)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off10_cfg_port_START  (21)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off10_cfg_port_END    (21)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off11_cfg_port_START  (22)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off11_cfg_port_END    (22)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off12_cfg_port_START  (23)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off12_cfg_port_END    (23)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off13_cfg_port_START  (24)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off13_cfg_port_END    (24)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off14_cfg_port_START  (25)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off14_cfg_port_END    (25)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off15_cfg_port_START  (26)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off15_cfg_port_END    (26)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off16_cfg_port_START  (27)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off16_cfg_port_END    (27)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off17_cfg_port_START  (28)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off17_cfg_port_END    (28)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off18_cfg_port_START  (29)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off18_cfg_port_END    (29)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off19_cfg_port_START  (30)
#define SOC_TZPC_TZPCDECPROT0CLR_gpio_off19_cfg_port_END    (30)
#define SOC_TZPC_TZPCDECPROT0CLR_ioc_off_cfg_port_START     (31)
#define SOC_TZPC_TZPCDECPROT0CLR_ioc_off_cfg_port_END       (31)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT1STAT_UNION
 结构说明  : TZPCDECPROT1STAT 寄存器结构定义。地址偏移量:0x080C，初值:0x3FFFFFFF，宽度:32
 寄存器说明: 安全设置状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tzasc_cfg_port         : 1;  /* bit[0]    : TZASC 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  ddrc_cfg_port          : 1;  /* bit[1]    : DDRC 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  i2c0_cfg_port          : 1;  /* bit[2]    : I2C0 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  i2c1_cfg_port          : 1;  /* bit[3]    : I2C1 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  i2c2_cfg_port          : 1;  /* bit[4]    : I2C2 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  i2c3_cfg_port          : 1;  /* bit[5]    : I2C3 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  i2c4_cfg_port          : 1;  /* bit[6]    : I2C4 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  i2c5_cfg_port          : 1;  /* bit[7]    : I2C5 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  uart0_cfg_port         : 1;  /* bit[8]    : UART0 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  uart1_cfg_port         : 1;  /* bit[9]    : UART1 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  uart2_cfg_port         : 1;  /* bit[10]   : UART2 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  uart3_cfg_port         : 1;  /* bit[11]   : UART3 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  uart4_cfg_port         : 1;  /* bit[12]   : UART4 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  uart5_cfg_port         : 1;  /* bit[13]   : UART5 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  uart6_cfg_port         : 1;  /* bit[14]   : UART6 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  ssp0_cfg_port          : 1;  /* bit[15]   : SSP0 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  ssp1_cfg_port          : 1;  /* bit[16]   : SSP1 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  asp_cfg_port           : 1;  /* bit[17]   : ASP 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  hifiitcm               : 1;  /* bit[18]   : HIFIITCM 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  hifidtcm               : 1;  /* bit[19]   : HIFIDTCM 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  pmctrl_cfg_port        : 1;  /* bit[20]   : PMCTRL低3K空间配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全
                                                                  （高1K空间始终非安全） */
        unsigned int  scoff_sec_ctrl         : 1;  /* bit[21]   : SCOFF_SEC_CTRL 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  scoff_sec_ctrl_rd0     : 1;  /* bit[22]   : SCOFF_SEC_CTRL_RD0 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  scoff_sec_ctrl_rd1     : 1;  /* bit[23]   : SCOFF_SEC_CTRL_RD1 配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  ap2mdm_tzasc_boot_lock : 1;  /* bit[24]   : AP2MDM_TZASC_BOOT_LOCK安全设置状态：
                                                                  0：LOCK
                                                                  1：非LOCK */
        unsigned int  ddrc_tzasc_boot_lock   : 1;  /* bit[25]   : DDRC_TZASC_BOOT_LOCK安全设置状态：
                                                                  0：LOCK
                                                                  1：非LOCK */
        unsigned int  seceng_cfg_port        : 2;  /* bit[26-27]: SECENG配置口安全设置状态：
                                                                  2'b11：将非安全转为安全访问；
                                                                  2'b10：操作透传，不做处理；
                                                                  2'b0x：只有安全可访问。 */
        unsigned int  nandc_cfg_port         : 1;  /* bit[28]   : NANDC配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  coresight_cfg_port     : 1;  /* bit[29]   : CoreSight配置接口安全设置状态：
                                                                  0：安全
                                                                  1：非安全 */
        unsigned int  reserved               : 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_TZPC_TZPCDECPROT1STAT_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT1STAT_tzasc_cfg_port_START          (0)
#define SOC_TZPC_TZPCDECPROT1STAT_tzasc_cfg_port_END            (0)
#define SOC_TZPC_TZPCDECPROT1STAT_ddrc_cfg_port_START           (1)
#define SOC_TZPC_TZPCDECPROT1STAT_ddrc_cfg_port_END             (1)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c0_cfg_port_START           (2)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c0_cfg_port_END             (2)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c1_cfg_port_START           (3)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c1_cfg_port_END             (3)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c2_cfg_port_START           (4)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c2_cfg_port_END             (4)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c3_cfg_port_START           (5)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c3_cfg_port_END             (5)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c4_cfg_port_START           (6)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c4_cfg_port_END             (6)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c5_cfg_port_START           (7)
#define SOC_TZPC_TZPCDECPROT1STAT_i2c5_cfg_port_END             (7)
#define SOC_TZPC_TZPCDECPROT1STAT_uart0_cfg_port_START          (8)
#define SOC_TZPC_TZPCDECPROT1STAT_uart0_cfg_port_END            (8)
#define SOC_TZPC_TZPCDECPROT1STAT_uart1_cfg_port_START          (9)
#define SOC_TZPC_TZPCDECPROT1STAT_uart1_cfg_port_END            (9)
#define SOC_TZPC_TZPCDECPROT1STAT_uart2_cfg_port_START          (10)
#define SOC_TZPC_TZPCDECPROT1STAT_uart2_cfg_port_END            (10)
#define SOC_TZPC_TZPCDECPROT1STAT_uart3_cfg_port_START          (11)
#define SOC_TZPC_TZPCDECPROT1STAT_uart3_cfg_port_END            (11)
#define SOC_TZPC_TZPCDECPROT1STAT_uart4_cfg_port_START          (12)
#define SOC_TZPC_TZPCDECPROT1STAT_uart4_cfg_port_END            (12)
#define SOC_TZPC_TZPCDECPROT1STAT_uart5_cfg_port_START          (13)
#define SOC_TZPC_TZPCDECPROT1STAT_uart5_cfg_port_END            (13)
#define SOC_TZPC_TZPCDECPROT1STAT_uart6_cfg_port_START          (14)
#define SOC_TZPC_TZPCDECPROT1STAT_uart6_cfg_port_END            (14)
#define SOC_TZPC_TZPCDECPROT1STAT_ssp0_cfg_port_START           (15)
#define SOC_TZPC_TZPCDECPROT1STAT_ssp0_cfg_port_END             (15)
#define SOC_TZPC_TZPCDECPROT1STAT_ssp1_cfg_port_START           (16)
#define SOC_TZPC_TZPCDECPROT1STAT_ssp1_cfg_port_END             (16)
#define SOC_TZPC_TZPCDECPROT1STAT_asp_cfg_port_START            (17)
#define SOC_TZPC_TZPCDECPROT1STAT_asp_cfg_port_END              (17)
#define SOC_TZPC_TZPCDECPROT1STAT_hifiitcm_START                (18)
#define SOC_TZPC_TZPCDECPROT1STAT_hifiitcm_END                  (18)
#define SOC_TZPC_TZPCDECPROT1STAT_hifidtcm_START                (19)
#define SOC_TZPC_TZPCDECPROT1STAT_hifidtcm_END                  (19)
#define SOC_TZPC_TZPCDECPROT1STAT_pmctrl_cfg_port_START         (20)
#define SOC_TZPC_TZPCDECPROT1STAT_pmctrl_cfg_port_END           (20)
#define SOC_TZPC_TZPCDECPROT1STAT_scoff_sec_ctrl_START          (21)
#define SOC_TZPC_TZPCDECPROT1STAT_scoff_sec_ctrl_END            (21)
#define SOC_TZPC_TZPCDECPROT1STAT_scoff_sec_ctrl_rd0_START      (22)
#define SOC_TZPC_TZPCDECPROT1STAT_scoff_sec_ctrl_rd0_END        (22)
#define SOC_TZPC_TZPCDECPROT1STAT_scoff_sec_ctrl_rd1_START      (23)
#define SOC_TZPC_TZPCDECPROT1STAT_scoff_sec_ctrl_rd1_END        (23)
#define SOC_TZPC_TZPCDECPROT1STAT_ap2mdm_tzasc_boot_lock_START  (24)
#define SOC_TZPC_TZPCDECPROT1STAT_ap2mdm_tzasc_boot_lock_END    (24)
#define SOC_TZPC_TZPCDECPROT1STAT_ddrc_tzasc_boot_lock_START    (25)
#define SOC_TZPC_TZPCDECPROT1STAT_ddrc_tzasc_boot_lock_END      (25)
#define SOC_TZPC_TZPCDECPROT1STAT_seceng_cfg_port_START         (26)
#define SOC_TZPC_TZPCDECPROT1STAT_seceng_cfg_port_END           (27)
#define SOC_TZPC_TZPCDECPROT1STAT_nandc_cfg_port_START          (28)
#define SOC_TZPC_TZPCDECPROT1STAT_nandc_cfg_port_END            (28)
#define SOC_TZPC_TZPCDECPROT1STAT_coresight_cfg_port_START      (29)
#define SOC_TZPC_TZPCDECPROT1STAT_coresight_cfg_port_END        (29)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT1SET_UNION
 结构说明  : TZPCDECPROT1SET 寄存器结构定义。地址偏移量:0x0810，初值:0x00000000，宽度:32
 寄存器说明: 非安全设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tzasc_cfg_port         : 1;  /* bit[0]    : TZASC 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  ddrc_cfg_port          : 1;  /* bit[1]    : DDRC 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  i2c0_cfg_port          : 1;  /* bit[2]    : I2C0 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  i2c1_cfg_port          : 1;  /* bit[3]    : I2C1 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  i2c2_cfg_port          : 1;  /* bit[4]    : I2C2 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  i2c3_cfg_port          : 1;  /* bit[5]    : I2C3 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  i2c4_cfg_port          : 1;  /* bit[6]    : I2C4 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  i2c5_cfg_port          : 1;  /* bit[7]    : I2C5 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  uart0_cfg_port         : 1;  /* bit[8]    : UART0 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  uart1_cfg_port         : 1;  /* bit[9]    : UART1 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  uart2_cfg_port         : 1;  /* bit[10]   : UART2 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  uart3_cfg_port         : 1;  /* bit[11]   : UART3 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  uart4_cfg_port         : 1;  /* bit[12]   : UART4 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  uart5_cfg_port         : 1;  /* bit[13]   : UART5 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  uart6_cfg_port         : 1;  /* bit[14]   : UART6 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  ssp0_cfg_port          : 1;  /* bit[15]   : SSP0 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  ssp1_cfg_port          : 1;  /* bit[16]   : SSP1 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  asp_cfg_port           : 1;  /* bit[17]   : ASP 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  hifiitcm               : 1;  /* bit[18]   : HIFIITCM 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  hifidtcm               : 1;  /* bit[19]   : HIFIDTCM 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  pmctrl_cfg_port        : 1;  /* bit[20]   : PMCTRL低3K空间配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全
                                                                  （高1K空间始终非安全） */
        unsigned int  scoff_sec_ctrl         : 1;  /* bit[21]   : SCOFF_SEC_CTRL 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  scoff_sec_ctrl_rd0     : 1;  /* bit[22]   : SCOFF_SEC_CTRL_RD0 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  scoff_sec_ctrl_rd1     : 1;  /* bit[23]   : SCOFF_SEC_CTRL_RD1 配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  ap2mdm_tzasc_boot_lock : 1;  /* bit[24]   : AP2MDM_TZASC_BOOT_LOCK非安全设置：
                                                                  0：无效
                                                                  1：非LOCK */
        unsigned int  ddrc_tzasc_boot_lock   : 1;  /* bit[25]   : DDRC_TZASC_BOOT_LOCK非安全设置：
                                                                  0：无效
                                                                  1：非LOCK */
        unsigned int  seceng_cfg_port        : 2;  /* bit[26-27]: SECENG配置口非安全设置：
                                                                  相应bit位配置1时，对应安全设置状态寄存器的对应bit变为1，对应含义根据TZPCDECPROT1STAT[27:26]而定 */
        unsigned int  nandc_cfg_port         : 1;  /* bit[28]   : NANDC配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  coresight_cfg_port     : 1;  /* bit[29]   : CoreSight配置接口非安全设置：
                                                                  0：无效
                                                                  1：非安全 */
        unsigned int  reserved               : 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_TZPC_TZPCDECPROT1SET_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT1SET_tzasc_cfg_port_START          (0)
#define SOC_TZPC_TZPCDECPROT1SET_tzasc_cfg_port_END            (0)
#define SOC_TZPC_TZPCDECPROT1SET_ddrc_cfg_port_START           (1)
#define SOC_TZPC_TZPCDECPROT1SET_ddrc_cfg_port_END             (1)
#define SOC_TZPC_TZPCDECPROT1SET_i2c0_cfg_port_START           (2)
#define SOC_TZPC_TZPCDECPROT1SET_i2c0_cfg_port_END             (2)
#define SOC_TZPC_TZPCDECPROT1SET_i2c1_cfg_port_START           (3)
#define SOC_TZPC_TZPCDECPROT1SET_i2c1_cfg_port_END             (3)
#define SOC_TZPC_TZPCDECPROT1SET_i2c2_cfg_port_START           (4)
#define SOC_TZPC_TZPCDECPROT1SET_i2c2_cfg_port_END             (4)
#define SOC_TZPC_TZPCDECPROT1SET_i2c3_cfg_port_START           (5)
#define SOC_TZPC_TZPCDECPROT1SET_i2c3_cfg_port_END             (5)
#define SOC_TZPC_TZPCDECPROT1SET_i2c4_cfg_port_START           (6)
#define SOC_TZPC_TZPCDECPROT1SET_i2c4_cfg_port_END             (6)
#define SOC_TZPC_TZPCDECPROT1SET_i2c5_cfg_port_START           (7)
#define SOC_TZPC_TZPCDECPROT1SET_i2c5_cfg_port_END             (7)
#define SOC_TZPC_TZPCDECPROT1SET_uart0_cfg_port_START          (8)
#define SOC_TZPC_TZPCDECPROT1SET_uart0_cfg_port_END            (8)
#define SOC_TZPC_TZPCDECPROT1SET_uart1_cfg_port_START          (9)
#define SOC_TZPC_TZPCDECPROT1SET_uart1_cfg_port_END            (9)
#define SOC_TZPC_TZPCDECPROT1SET_uart2_cfg_port_START          (10)
#define SOC_TZPC_TZPCDECPROT1SET_uart2_cfg_port_END            (10)
#define SOC_TZPC_TZPCDECPROT1SET_uart3_cfg_port_START          (11)
#define SOC_TZPC_TZPCDECPROT1SET_uart3_cfg_port_END            (11)
#define SOC_TZPC_TZPCDECPROT1SET_uart4_cfg_port_START          (12)
#define SOC_TZPC_TZPCDECPROT1SET_uart4_cfg_port_END            (12)
#define SOC_TZPC_TZPCDECPROT1SET_uart5_cfg_port_START          (13)
#define SOC_TZPC_TZPCDECPROT1SET_uart5_cfg_port_END            (13)
#define SOC_TZPC_TZPCDECPROT1SET_uart6_cfg_port_START          (14)
#define SOC_TZPC_TZPCDECPROT1SET_uart6_cfg_port_END            (14)
#define SOC_TZPC_TZPCDECPROT1SET_ssp0_cfg_port_START           (15)
#define SOC_TZPC_TZPCDECPROT1SET_ssp0_cfg_port_END             (15)
#define SOC_TZPC_TZPCDECPROT1SET_ssp1_cfg_port_START           (16)
#define SOC_TZPC_TZPCDECPROT1SET_ssp1_cfg_port_END             (16)
#define SOC_TZPC_TZPCDECPROT1SET_asp_cfg_port_START            (17)
#define SOC_TZPC_TZPCDECPROT1SET_asp_cfg_port_END              (17)
#define SOC_TZPC_TZPCDECPROT1SET_hifiitcm_START                (18)
#define SOC_TZPC_TZPCDECPROT1SET_hifiitcm_END                  (18)
#define SOC_TZPC_TZPCDECPROT1SET_hifidtcm_START                (19)
#define SOC_TZPC_TZPCDECPROT1SET_hifidtcm_END                  (19)
#define SOC_TZPC_TZPCDECPROT1SET_pmctrl_cfg_port_START         (20)
#define SOC_TZPC_TZPCDECPROT1SET_pmctrl_cfg_port_END           (20)
#define SOC_TZPC_TZPCDECPROT1SET_scoff_sec_ctrl_START          (21)
#define SOC_TZPC_TZPCDECPROT1SET_scoff_sec_ctrl_END            (21)
#define SOC_TZPC_TZPCDECPROT1SET_scoff_sec_ctrl_rd0_START      (22)
#define SOC_TZPC_TZPCDECPROT1SET_scoff_sec_ctrl_rd0_END        (22)
#define SOC_TZPC_TZPCDECPROT1SET_scoff_sec_ctrl_rd1_START      (23)
#define SOC_TZPC_TZPCDECPROT1SET_scoff_sec_ctrl_rd1_END        (23)
#define SOC_TZPC_TZPCDECPROT1SET_ap2mdm_tzasc_boot_lock_START  (24)
#define SOC_TZPC_TZPCDECPROT1SET_ap2mdm_tzasc_boot_lock_END    (24)
#define SOC_TZPC_TZPCDECPROT1SET_ddrc_tzasc_boot_lock_START    (25)
#define SOC_TZPC_TZPCDECPROT1SET_ddrc_tzasc_boot_lock_END      (25)
#define SOC_TZPC_TZPCDECPROT1SET_seceng_cfg_port_START         (26)
#define SOC_TZPC_TZPCDECPROT1SET_seceng_cfg_port_END           (27)
#define SOC_TZPC_TZPCDECPROT1SET_nandc_cfg_port_START          (28)
#define SOC_TZPC_TZPCDECPROT1SET_nandc_cfg_port_END            (28)
#define SOC_TZPC_TZPCDECPROT1SET_coresight_cfg_port_START      (29)
#define SOC_TZPC_TZPCDECPROT1SET_coresight_cfg_port_END        (29)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT1CLR_UNION
 结构说明  : TZPCDECPROT1CLR 寄存器结构定义。地址偏移量:0x0814，初值:0x00000000，宽度:32
 寄存器说明: 安全设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tzasc_cfg_port         : 1;  /* bit[0]    : TZASC 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  ddrc_cfg_port          : 1;  /* bit[1]    : DDRC 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  i2c0_cfg_port          : 1;  /* bit[2]    : I2C0 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  i2c1_cfg_port          : 1;  /* bit[3]    : I2C1 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  i2c2_cfg_port          : 1;  /* bit[4]    : I2C2 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  i2c3_cfg_port          : 1;  /* bit[5]    : I2C3 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  i2c4_cfg_port          : 1;  /* bit[6]    : I2C4 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  i2c5_cfg_port          : 1;  /* bit[7]    : I2C5 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  uart0_cfg_port         : 1;  /* bit[8]    : UART0 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  uart1_cfg_port         : 1;  /* bit[9]    : UART1 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  uart2_cfg_port         : 1;  /* bit[10]   : UART2 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  uart3_cfg_port         : 1;  /* bit[11]   : UART3 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  uart4_cfg_port         : 1;  /* bit[12]   : UART4 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  uart5_cfg_port         : 1;  /* bit[13]   : UART5 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  uart6_cfg_port         : 1;  /* bit[14]   : UART6 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  ssp0_cfg_port          : 1;  /* bit[15]   : SSP0 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  ssp1_cfg_port          : 1;  /* bit[16]   : SSP1 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  asp_cfg_port           : 1;  /* bit[17]   : ASP 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  hifiitcm               : 1;  /* bit[18]   : HIFIITCM 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  hifidtcm               : 1;  /* bit[19]   : HIFIDTCM 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  pmctrl_cfg_port        : 1;  /* bit[20]   : PMCTRL低3K空间配置接口安全设置：
                                                                  0：无效
                                                                  1：安全
                                                                  （高1K空间始终非安全） */
        unsigned int  scoff_sec_ctrl         : 1;  /* bit[21]   : SCOFF_SEC_CTRL 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  scoff_sec_ctrl_rd0     : 1;  /* bit[22]   : SCOFF_SEC_CTRL_RD0 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  scoff_sec_ctrl_rd1     : 1;  /* bit[23]   : SCOFF_SEC_CTRL_RD1 配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  ap2mdm_tzasc_boot_lock : 1;  /* bit[24]   : AP2MDM_TZASC_BOOT_LOCK安全设置：
                                                                  0：无效
                                                                  1：LOCK */
        unsigned int  ddrc_tzasc_boot_lock   : 1;  /* bit[25]   : DDRC_TZASC_BOOT_LOCK安全设置：
                                                                  0：无效
                                                                  1：LOCK */
        unsigned int  seceng_cfg_port        : 2;  /* bit[26-27]: SECENG配置口安全设置：
                                                                  相应bit位配置1时，对应安全设置状态寄存器的对应bit变为0，对应含义根据TZPCDECPROT1STAT[27:26]而定 */
        unsigned int  nandc_cfg_port         : 1;  /* bit[28]   : NANDC配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  coresight_cfg_port     : 1;  /* bit[29]   : CoreSight配置接口安全设置：
                                                                  0：无效
                                                                  1：安全 */
        unsigned int  reserved               : 2;  /* bit[30-31]: reserved */
    } reg;
} SOC_TZPC_TZPCDECPROT1CLR_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT1CLR_tzasc_cfg_port_START          (0)
#define SOC_TZPC_TZPCDECPROT1CLR_tzasc_cfg_port_END            (0)
#define SOC_TZPC_TZPCDECPROT1CLR_ddrc_cfg_port_START           (1)
#define SOC_TZPC_TZPCDECPROT1CLR_ddrc_cfg_port_END             (1)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c0_cfg_port_START           (2)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c0_cfg_port_END             (2)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c1_cfg_port_START           (3)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c1_cfg_port_END             (3)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c2_cfg_port_START           (4)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c2_cfg_port_END             (4)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c3_cfg_port_START           (5)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c3_cfg_port_END             (5)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c4_cfg_port_START           (6)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c4_cfg_port_END             (6)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c5_cfg_port_START           (7)
#define SOC_TZPC_TZPCDECPROT1CLR_i2c5_cfg_port_END             (7)
#define SOC_TZPC_TZPCDECPROT1CLR_uart0_cfg_port_START          (8)
#define SOC_TZPC_TZPCDECPROT1CLR_uart0_cfg_port_END            (8)
#define SOC_TZPC_TZPCDECPROT1CLR_uart1_cfg_port_START          (9)
#define SOC_TZPC_TZPCDECPROT1CLR_uart1_cfg_port_END            (9)
#define SOC_TZPC_TZPCDECPROT1CLR_uart2_cfg_port_START          (10)
#define SOC_TZPC_TZPCDECPROT1CLR_uart2_cfg_port_END            (10)
#define SOC_TZPC_TZPCDECPROT1CLR_uart3_cfg_port_START          (11)
#define SOC_TZPC_TZPCDECPROT1CLR_uart3_cfg_port_END            (11)
#define SOC_TZPC_TZPCDECPROT1CLR_uart4_cfg_port_START          (12)
#define SOC_TZPC_TZPCDECPROT1CLR_uart4_cfg_port_END            (12)
#define SOC_TZPC_TZPCDECPROT1CLR_uart5_cfg_port_START          (13)
#define SOC_TZPC_TZPCDECPROT1CLR_uart5_cfg_port_END            (13)
#define SOC_TZPC_TZPCDECPROT1CLR_uart6_cfg_port_START          (14)
#define SOC_TZPC_TZPCDECPROT1CLR_uart6_cfg_port_END            (14)
#define SOC_TZPC_TZPCDECPROT1CLR_ssp0_cfg_port_START           (15)
#define SOC_TZPC_TZPCDECPROT1CLR_ssp0_cfg_port_END             (15)
#define SOC_TZPC_TZPCDECPROT1CLR_ssp1_cfg_port_START           (16)
#define SOC_TZPC_TZPCDECPROT1CLR_ssp1_cfg_port_END             (16)
#define SOC_TZPC_TZPCDECPROT1CLR_asp_cfg_port_START            (17)
#define SOC_TZPC_TZPCDECPROT1CLR_asp_cfg_port_END              (17)
#define SOC_TZPC_TZPCDECPROT1CLR_hifiitcm_START                (18)
#define SOC_TZPC_TZPCDECPROT1CLR_hifiitcm_END                  (18)
#define SOC_TZPC_TZPCDECPROT1CLR_hifidtcm_START                (19)
#define SOC_TZPC_TZPCDECPROT1CLR_hifidtcm_END                  (19)
#define SOC_TZPC_TZPCDECPROT1CLR_pmctrl_cfg_port_START         (20)
#define SOC_TZPC_TZPCDECPROT1CLR_pmctrl_cfg_port_END           (20)
#define SOC_TZPC_TZPCDECPROT1CLR_scoff_sec_ctrl_START          (21)
#define SOC_TZPC_TZPCDECPROT1CLR_scoff_sec_ctrl_END            (21)
#define SOC_TZPC_TZPCDECPROT1CLR_scoff_sec_ctrl_rd0_START      (22)
#define SOC_TZPC_TZPCDECPROT1CLR_scoff_sec_ctrl_rd0_END        (22)
#define SOC_TZPC_TZPCDECPROT1CLR_scoff_sec_ctrl_rd1_START      (23)
#define SOC_TZPC_TZPCDECPROT1CLR_scoff_sec_ctrl_rd1_END        (23)
#define SOC_TZPC_TZPCDECPROT1CLR_ap2mdm_tzasc_boot_lock_START  (24)
#define SOC_TZPC_TZPCDECPROT1CLR_ap2mdm_tzasc_boot_lock_END    (24)
#define SOC_TZPC_TZPCDECPROT1CLR_ddrc_tzasc_boot_lock_START    (25)
#define SOC_TZPC_TZPCDECPROT1CLR_ddrc_tzasc_boot_lock_END      (25)
#define SOC_TZPC_TZPCDECPROT1CLR_seceng_cfg_port_START         (26)
#define SOC_TZPC_TZPCDECPROT1CLR_seceng_cfg_port_END           (27)
#define SOC_TZPC_TZPCDECPROT1CLR_nandc_cfg_port_START          (28)
#define SOC_TZPC_TZPCDECPROT1CLR_nandc_cfg_port_END            (28)
#define SOC_TZPC_TZPCDECPROT1CLR_coresight_cfg_port_START      (29)
#define SOC_TZPC_TZPCDECPROT1CLR_coresight_cfg_port_END        (29)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT2STAT_UNION
 结构说明  : TZPCDECPROT2STAT 寄存器结构定义。地址偏移量:0x0818，初值:0x00000000，宽度:32
 寄存器说明: 安全设置状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  watch_dog_0_cfg_port : 1;  /* bit[0] : Watch_Dog_0_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  watch_dog_1_cfg_port : 1;  /* bit[1] : Watch_Dog_1_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  watch_dog_2_cfg_port : 1;  /* bit[2] : Watch_Dog_2_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  watch_dog_3_cfg_port : 1;  /* bit[3] : Watch_Dog_3_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  timer0_cfg_port      : 1;  /* bit[4] : Timer0_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  timer1_cfg_port      : 1;  /* bit[5] : Timer1_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  timer2_cfg_port      : 1;  /* bit[6] : Timer2_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  timer3_cfg_port      : 1;  /* bit[7] : Timer3_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  timer4_cfg_port      : 1;  /* bit[8] : Timer4_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  timer5_cfg_port      : 1;  /* bit[9] : Timer5_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  timer6_cfg_port      : 1;  /* bit[10]: Timer6_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  timer7_cfg_port      : 1;  /* bit[11]: Timer7_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  timer8_cfg_port      : 1;  /* bit[12]: Timer8_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  rtc_cfg_port         : 1;  /* bit[13]: RTC_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  bbp_on_cfg_port      : 1;  /* bit[14]: BBP_ON_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer0_cfg_port     : 1;  /* bit[15]: Mtimer0_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer1_cfg_port     : 1;  /* bit[16]: Mtimer1_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer2_cfg_port     : 1;  /* bit[17]: Mtimer2_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer3_cfg_port     : 1;  /* bit[18]: Mtimer3_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer4_cfg_port     : 1;  /* bit[19]: Mtimer4_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer5_cfg_port     : 1;  /* bit[20]: Mtimer5_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer6_cfg_port     : 1;  /* bit[21]: Mtimer6_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer7_cfg_port     : 1;  /* bit[22]: Mtimer7_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer8_cfg_port     : 1;  /* bit[23]: Mtimer8_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mtimer9_cfg_port     : 1;  /* bit[24]: Mtimer9_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  pmussi_cfg_port      : 1;  /* bit[25]: PMUSSI_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  ioc_on_cfg_port      : 1;  /* bit[26]: IOC_ON_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  gpio_on0_cfg_port    : 1;  /* bit[27]: GPIO_ON0_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  gpio_on1_cfg_port    : 1;  /* bit[28]: GPIO_ON1_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  gpio_on2_cfg_port    : 1;  /* bit[29]: GPIO_ON2_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  gpio_on3_cfg_port    : 1;  /* bit[30]: GPIO_ON3_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
        unsigned int  mcu_subsys_cfg_port  : 1;  /* bit[31]: MCU_Subsys_CFG_Port安全状态：
                                                             0：安全
                                                             1：非安全 */
    } reg;
} SOC_TZPC_TZPCDECPROT2STAT_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT2STAT_watch_dog_0_cfg_port_START  (0)
#define SOC_TZPC_TZPCDECPROT2STAT_watch_dog_0_cfg_port_END    (0)
#define SOC_TZPC_TZPCDECPROT2STAT_watch_dog_1_cfg_port_START  (1)
#define SOC_TZPC_TZPCDECPROT2STAT_watch_dog_1_cfg_port_END    (1)
#define SOC_TZPC_TZPCDECPROT2STAT_watch_dog_2_cfg_port_START  (2)
#define SOC_TZPC_TZPCDECPROT2STAT_watch_dog_2_cfg_port_END    (2)
#define SOC_TZPC_TZPCDECPROT2STAT_watch_dog_3_cfg_port_START  (3)
#define SOC_TZPC_TZPCDECPROT2STAT_watch_dog_3_cfg_port_END    (3)
#define SOC_TZPC_TZPCDECPROT2STAT_timer0_cfg_port_START       (4)
#define SOC_TZPC_TZPCDECPROT2STAT_timer0_cfg_port_END         (4)
#define SOC_TZPC_TZPCDECPROT2STAT_timer1_cfg_port_START       (5)
#define SOC_TZPC_TZPCDECPROT2STAT_timer1_cfg_port_END         (5)
#define SOC_TZPC_TZPCDECPROT2STAT_timer2_cfg_port_START       (6)
#define SOC_TZPC_TZPCDECPROT2STAT_timer2_cfg_port_END         (6)
#define SOC_TZPC_TZPCDECPROT2STAT_timer3_cfg_port_START       (7)
#define SOC_TZPC_TZPCDECPROT2STAT_timer3_cfg_port_END         (7)
#define SOC_TZPC_TZPCDECPROT2STAT_timer4_cfg_port_START       (8)
#define SOC_TZPC_TZPCDECPROT2STAT_timer4_cfg_port_END         (8)
#define SOC_TZPC_TZPCDECPROT2STAT_timer5_cfg_port_START       (9)
#define SOC_TZPC_TZPCDECPROT2STAT_timer5_cfg_port_END         (9)
#define SOC_TZPC_TZPCDECPROT2STAT_timer6_cfg_port_START       (10)
#define SOC_TZPC_TZPCDECPROT2STAT_timer6_cfg_port_END         (10)
#define SOC_TZPC_TZPCDECPROT2STAT_timer7_cfg_port_START       (11)
#define SOC_TZPC_TZPCDECPROT2STAT_timer7_cfg_port_END         (11)
#define SOC_TZPC_TZPCDECPROT2STAT_timer8_cfg_port_START       (12)
#define SOC_TZPC_TZPCDECPROT2STAT_timer8_cfg_port_END         (12)
#define SOC_TZPC_TZPCDECPROT2STAT_rtc_cfg_port_START          (13)
#define SOC_TZPC_TZPCDECPROT2STAT_rtc_cfg_port_END            (13)
#define SOC_TZPC_TZPCDECPROT2STAT_bbp_on_cfg_port_START       (14)
#define SOC_TZPC_TZPCDECPROT2STAT_bbp_on_cfg_port_END         (14)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer0_cfg_port_START      (15)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer0_cfg_port_END        (15)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer1_cfg_port_START      (16)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer1_cfg_port_END        (16)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer2_cfg_port_START      (17)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer2_cfg_port_END        (17)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer3_cfg_port_START      (18)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer3_cfg_port_END        (18)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer4_cfg_port_START      (19)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer4_cfg_port_END        (19)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer5_cfg_port_START      (20)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer5_cfg_port_END        (20)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer6_cfg_port_START      (21)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer6_cfg_port_END        (21)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer7_cfg_port_START      (22)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer7_cfg_port_END        (22)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer8_cfg_port_START      (23)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer8_cfg_port_END        (23)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer9_cfg_port_START      (24)
#define SOC_TZPC_TZPCDECPROT2STAT_mtimer9_cfg_port_END        (24)
#define SOC_TZPC_TZPCDECPROT2STAT_pmussi_cfg_port_START       (25)
#define SOC_TZPC_TZPCDECPROT2STAT_pmussi_cfg_port_END         (25)
#define SOC_TZPC_TZPCDECPROT2STAT_ioc_on_cfg_port_START       (26)
#define SOC_TZPC_TZPCDECPROT2STAT_ioc_on_cfg_port_END         (26)
#define SOC_TZPC_TZPCDECPROT2STAT_gpio_on0_cfg_port_START     (27)
#define SOC_TZPC_TZPCDECPROT2STAT_gpio_on0_cfg_port_END       (27)
#define SOC_TZPC_TZPCDECPROT2STAT_gpio_on1_cfg_port_START     (28)
#define SOC_TZPC_TZPCDECPROT2STAT_gpio_on1_cfg_port_END       (28)
#define SOC_TZPC_TZPCDECPROT2STAT_gpio_on2_cfg_port_START     (29)
#define SOC_TZPC_TZPCDECPROT2STAT_gpio_on2_cfg_port_END       (29)
#define SOC_TZPC_TZPCDECPROT2STAT_gpio_on3_cfg_port_START     (30)
#define SOC_TZPC_TZPCDECPROT2STAT_gpio_on3_cfg_port_END       (30)
#define SOC_TZPC_TZPCDECPROT2STAT_mcu_subsys_cfg_port_START   (31)
#define SOC_TZPC_TZPCDECPROT2STAT_mcu_subsys_cfg_port_END     (31)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT2SET_UNION
 结构说明  : TZPCDECPROT2SET 寄存器结构定义。地址偏移量:0x081C，初值:0x00000000，宽度:32
 寄存器说明: 非安全设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  watch_dog_0_cfg_port : 1;  /* bit[0] : Watch_Dog_0_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  watch_dog_1_cfg_port : 1;  /* bit[1] : Watch_Dog_1_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  watch_dog_2_cfg_port : 1;  /* bit[2] : Watch_Dog_2_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  watch_dog_3_cfg_port : 1;  /* bit[3] : Watch_Dog_3_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  timer0_cfg_port      : 1;  /* bit[4] : Timer0_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  timer1_cfg_port      : 1;  /* bit[5] : Timer1_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  timer2_cfg_port      : 1;  /* bit[6] : Timer2_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  timer3_cfg_port      : 1;  /* bit[7] : Timer3_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  timer4_cfg_port      : 1;  /* bit[8] : Timer4_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  timer5_cfg_port      : 1;  /* bit[9] : Timer5_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  timer6_cfg_port      : 1;  /* bit[10]: Timer6_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  timer7_cfg_port      : 1;  /* bit[11]: Timer7_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  timer8_cfg_port      : 1;  /* bit[12]: Timer8_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  rtc_cfg_port         : 1;  /* bit[13]: RTC_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  bbp_on_cfg_port      : 1;  /* bit[14]: BBP_ON_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer0_cfg_port     : 1;  /* bit[15]: Mtimer0_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer1_cfg_port     : 1;  /* bit[16]: Mtimer1_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer2_cfg_port     : 1;  /* bit[17]: Mtimer2_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer3_cfg_port     : 1;  /* bit[18]: Mtimer3_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer4_cfg_port     : 1;  /* bit[19]: Mtimer4_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer5_cfg_port     : 1;  /* bit[20]: Mtimer5_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer6_cfg_port     : 1;  /* bit[21]: Mtimer6_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer7_cfg_port     : 1;  /* bit[22]: Mtimer7_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer8_cfg_port     : 1;  /* bit[23]: Mtimer8_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mtimer9_cfg_port     : 1;  /* bit[24]: Mtimer9_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  pmussi_cfg_port      : 1;  /* bit[25]: PMUSSI_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  ioc_on_cfg_port      : 1;  /* bit[26]: IOC_ON_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  gpio_on0_cfg_port    : 1;  /* bit[27]: GPIO_ON0_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  gpio_on1_cfg_port    : 1;  /* bit[28]: GPIO_ON1_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  gpio_on2_cfg_port    : 1;  /* bit[29]: GPIO_ON2_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  gpio_on3_cfg_port    : 1;  /* bit[30]: GPIO_ON3_CFG_Port非安全设置：
                                                             0：无效
                                                             1：非安全 */
        unsigned int  mcu_subsys_cfg_port  : 1;  /* bit[31]: MCU_Subsys_CFG_Port非安全设置：
                                                             0：安全
                                                             1：非安全 */
    } reg;
} SOC_TZPC_TZPCDECPROT2SET_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT2SET_watch_dog_0_cfg_port_START  (0)
#define SOC_TZPC_TZPCDECPROT2SET_watch_dog_0_cfg_port_END    (0)
#define SOC_TZPC_TZPCDECPROT2SET_watch_dog_1_cfg_port_START  (1)
#define SOC_TZPC_TZPCDECPROT2SET_watch_dog_1_cfg_port_END    (1)
#define SOC_TZPC_TZPCDECPROT2SET_watch_dog_2_cfg_port_START  (2)
#define SOC_TZPC_TZPCDECPROT2SET_watch_dog_2_cfg_port_END    (2)
#define SOC_TZPC_TZPCDECPROT2SET_watch_dog_3_cfg_port_START  (3)
#define SOC_TZPC_TZPCDECPROT2SET_watch_dog_3_cfg_port_END    (3)
#define SOC_TZPC_TZPCDECPROT2SET_timer0_cfg_port_START       (4)
#define SOC_TZPC_TZPCDECPROT2SET_timer0_cfg_port_END         (4)
#define SOC_TZPC_TZPCDECPROT2SET_timer1_cfg_port_START       (5)
#define SOC_TZPC_TZPCDECPROT2SET_timer1_cfg_port_END         (5)
#define SOC_TZPC_TZPCDECPROT2SET_timer2_cfg_port_START       (6)
#define SOC_TZPC_TZPCDECPROT2SET_timer2_cfg_port_END         (6)
#define SOC_TZPC_TZPCDECPROT2SET_timer3_cfg_port_START       (7)
#define SOC_TZPC_TZPCDECPROT2SET_timer3_cfg_port_END         (7)
#define SOC_TZPC_TZPCDECPROT2SET_timer4_cfg_port_START       (8)
#define SOC_TZPC_TZPCDECPROT2SET_timer4_cfg_port_END         (8)
#define SOC_TZPC_TZPCDECPROT2SET_timer5_cfg_port_START       (9)
#define SOC_TZPC_TZPCDECPROT2SET_timer5_cfg_port_END         (9)
#define SOC_TZPC_TZPCDECPROT2SET_timer6_cfg_port_START       (10)
#define SOC_TZPC_TZPCDECPROT2SET_timer6_cfg_port_END         (10)
#define SOC_TZPC_TZPCDECPROT2SET_timer7_cfg_port_START       (11)
#define SOC_TZPC_TZPCDECPROT2SET_timer7_cfg_port_END         (11)
#define SOC_TZPC_TZPCDECPROT2SET_timer8_cfg_port_START       (12)
#define SOC_TZPC_TZPCDECPROT2SET_timer8_cfg_port_END         (12)
#define SOC_TZPC_TZPCDECPROT2SET_rtc_cfg_port_START          (13)
#define SOC_TZPC_TZPCDECPROT2SET_rtc_cfg_port_END            (13)
#define SOC_TZPC_TZPCDECPROT2SET_bbp_on_cfg_port_START       (14)
#define SOC_TZPC_TZPCDECPROT2SET_bbp_on_cfg_port_END         (14)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer0_cfg_port_START      (15)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer0_cfg_port_END        (15)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer1_cfg_port_START      (16)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer1_cfg_port_END        (16)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer2_cfg_port_START      (17)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer2_cfg_port_END        (17)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer3_cfg_port_START      (18)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer3_cfg_port_END        (18)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer4_cfg_port_START      (19)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer4_cfg_port_END        (19)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer5_cfg_port_START      (20)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer5_cfg_port_END        (20)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer6_cfg_port_START      (21)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer6_cfg_port_END        (21)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer7_cfg_port_START      (22)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer7_cfg_port_END        (22)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer8_cfg_port_START      (23)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer8_cfg_port_END        (23)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer9_cfg_port_START      (24)
#define SOC_TZPC_TZPCDECPROT2SET_mtimer9_cfg_port_END        (24)
#define SOC_TZPC_TZPCDECPROT2SET_pmussi_cfg_port_START       (25)
#define SOC_TZPC_TZPCDECPROT2SET_pmussi_cfg_port_END         (25)
#define SOC_TZPC_TZPCDECPROT2SET_ioc_on_cfg_port_START       (26)
#define SOC_TZPC_TZPCDECPROT2SET_ioc_on_cfg_port_END         (26)
#define SOC_TZPC_TZPCDECPROT2SET_gpio_on0_cfg_port_START     (27)
#define SOC_TZPC_TZPCDECPROT2SET_gpio_on0_cfg_port_END       (27)
#define SOC_TZPC_TZPCDECPROT2SET_gpio_on1_cfg_port_START     (28)
#define SOC_TZPC_TZPCDECPROT2SET_gpio_on1_cfg_port_END       (28)
#define SOC_TZPC_TZPCDECPROT2SET_gpio_on2_cfg_port_START     (29)
#define SOC_TZPC_TZPCDECPROT2SET_gpio_on2_cfg_port_END       (29)
#define SOC_TZPC_TZPCDECPROT2SET_gpio_on3_cfg_port_START     (30)
#define SOC_TZPC_TZPCDECPROT2SET_gpio_on3_cfg_port_END       (30)
#define SOC_TZPC_TZPCDECPROT2SET_mcu_subsys_cfg_port_START   (31)
#define SOC_TZPC_TZPCDECPROT2SET_mcu_subsys_cfg_port_END     (31)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT2CLR_UNION
 结构说明  : TZPCDECPROT2CLR 寄存器结构定义。地址偏移量:0x0820，初值:0x00000000，宽度:32
 寄存器说明: 安全设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  watch_dog_0_cfg_port : 1;  /* bit[0] : Watch_Dog_0_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  watch_dog_1_cfg_port : 1;  /* bit[1] : Watch_Dog_1_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  watch_dog_2_cfg_port : 1;  /* bit[2] : Watch_Dog_2_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  watch_dog_3_cfg_port : 1;  /* bit[3] : Watch_Dog_3_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  timer0_cfg_port      : 1;  /* bit[4] : Timer0_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  timer1_cfg_port      : 1;  /* bit[5] : Timer1_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  timer2_cfg_port      : 1;  /* bit[6] : Timer2_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  timer3_cfg_port      : 1;  /* bit[7] : Timer3_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  timer4_cfg_port      : 1;  /* bit[8] : Timer4_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  timer5_cfg_port      : 1;  /* bit[9] : Timer5_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  timer6_cfg_port      : 1;  /* bit[10]: Timer6_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  timer7_cfg_port      : 1;  /* bit[11]: Timer7_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  timer8_cfg_port      : 1;  /* bit[12]: Timer8_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  rtc_cfg_port         : 1;  /* bit[13]: RTC_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  bbp_on_cfg_port      : 1;  /* bit[14]: BBP_ON_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer0_cfg_port     : 1;  /* bit[15]: Mtimer0_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer1_cfg_port     : 1;  /* bit[16]: Mtimer1_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer2_cfg_port     : 1;  /* bit[17]: Mtimer2_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer3_cfg_port     : 1;  /* bit[18]: Mtimer3_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer4_cfg_port     : 1;  /* bit[19]: Mtimer4_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer5_cfg_port     : 1;  /* bit[20]: Mtimer5_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer6_cfg_port     : 1;  /* bit[21]: Mtimer6_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer7_cfg_port     : 1;  /* bit[22]: Mtimer7_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer8_cfg_port     : 1;  /* bit[23]: Mtimer8_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mtimer9_cfg_port     : 1;  /* bit[24]: Mtimer9_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  pmussi_cfg_port      : 1;  /* bit[25]: PMUSSI_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  ioc_on_cfg_port      : 1;  /* bit[26]: IOC_ON_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  gpio_on0_cfg_port    : 1;  /* bit[27]: GPIO_ON0_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  gpio_on1_cfg_port    : 1;  /* bit[28]: GPIO_ON1_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  gpio_on2_cfg_port    : 1;  /* bit[29]: GPIO_ON2_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  gpio_on3_cfg_port    : 1;  /* bit[30]: GPIO_ON3_CFG_Port安全设置：
                                                             0：无效
                                                             1：安全 */
        unsigned int  mcu_subsys_cfg_port  : 1;  /* bit[31]: MCU_Subsys_CFG_Port安全设置：
                                                             0：安全
                                                             1：安全 */
    } reg;
} SOC_TZPC_TZPCDECPROT2CLR_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT2CLR_watch_dog_0_cfg_port_START  (0)
#define SOC_TZPC_TZPCDECPROT2CLR_watch_dog_0_cfg_port_END    (0)
#define SOC_TZPC_TZPCDECPROT2CLR_watch_dog_1_cfg_port_START  (1)
#define SOC_TZPC_TZPCDECPROT2CLR_watch_dog_1_cfg_port_END    (1)
#define SOC_TZPC_TZPCDECPROT2CLR_watch_dog_2_cfg_port_START  (2)
#define SOC_TZPC_TZPCDECPROT2CLR_watch_dog_2_cfg_port_END    (2)
#define SOC_TZPC_TZPCDECPROT2CLR_watch_dog_3_cfg_port_START  (3)
#define SOC_TZPC_TZPCDECPROT2CLR_watch_dog_3_cfg_port_END    (3)
#define SOC_TZPC_TZPCDECPROT2CLR_timer0_cfg_port_START       (4)
#define SOC_TZPC_TZPCDECPROT2CLR_timer0_cfg_port_END         (4)
#define SOC_TZPC_TZPCDECPROT2CLR_timer1_cfg_port_START       (5)
#define SOC_TZPC_TZPCDECPROT2CLR_timer1_cfg_port_END         (5)
#define SOC_TZPC_TZPCDECPROT2CLR_timer2_cfg_port_START       (6)
#define SOC_TZPC_TZPCDECPROT2CLR_timer2_cfg_port_END         (6)
#define SOC_TZPC_TZPCDECPROT2CLR_timer3_cfg_port_START       (7)
#define SOC_TZPC_TZPCDECPROT2CLR_timer3_cfg_port_END         (7)
#define SOC_TZPC_TZPCDECPROT2CLR_timer4_cfg_port_START       (8)
#define SOC_TZPC_TZPCDECPROT2CLR_timer4_cfg_port_END         (8)
#define SOC_TZPC_TZPCDECPROT2CLR_timer5_cfg_port_START       (9)
#define SOC_TZPC_TZPCDECPROT2CLR_timer5_cfg_port_END         (9)
#define SOC_TZPC_TZPCDECPROT2CLR_timer6_cfg_port_START       (10)
#define SOC_TZPC_TZPCDECPROT2CLR_timer6_cfg_port_END         (10)
#define SOC_TZPC_TZPCDECPROT2CLR_timer7_cfg_port_START       (11)
#define SOC_TZPC_TZPCDECPROT2CLR_timer7_cfg_port_END         (11)
#define SOC_TZPC_TZPCDECPROT2CLR_timer8_cfg_port_START       (12)
#define SOC_TZPC_TZPCDECPROT2CLR_timer8_cfg_port_END         (12)
#define SOC_TZPC_TZPCDECPROT2CLR_rtc_cfg_port_START          (13)
#define SOC_TZPC_TZPCDECPROT2CLR_rtc_cfg_port_END            (13)
#define SOC_TZPC_TZPCDECPROT2CLR_bbp_on_cfg_port_START       (14)
#define SOC_TZPC_TZPCDECPROT2CLR_bbp_on_cfg_port_END         (14)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer0_cfg_port_START      (15)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer0_cfg_port_END        (15)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer1_cfg_port_START      (16)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer1_cfg_port_END        (16)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer2_cfg_port_START      (17)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer2_cfg_port_END        (17)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer3_cfg_port_START      (18)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer3_cfg_port_END        (18)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer4_cfg_port_START      (19)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer4_cfg_port_END        (19)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer5_cfg_port_START      (20)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer5_cfg_port_END        (20)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer6_cfg_port_START      (21)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer6_cfg_port_END        (21)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer7_cfg_port_START      (22)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer7_cfg_port_END        (22)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer8_cfg_port_START      (23)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer8_cfg_port_END        (23)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer9_cfg_port_START      (24)
#define SOC_TZPC_TZPCDECPROT2CLR_mtimer9_cfg_port_END        (24)
#define SOC_TZPC_TZPCDECPROT2CLR_pmussi_cfg_port_START       (25)
#define SOC_TZPC_TZPCDECPROT2CLR_pmussi_cfg_port_END         (25)
#define SOC_TZPC_TZPCDECPROT2CLR_ioc_on_cfg_port_START       (26)
#define SOC_TZPC_TZPCDECPROT2CLR_ioc_on_cfg_port_END         (26)
#define SOC_TZPC_TZPCDECPROT2CLR_gpio_on0_cfg_port_START     (27)
#define SOC_TZPC_TZPCDECPROT2CLR_gpio_on0_cfg_port_END       (27)
#define SOC_TZPC_TZPCDECPROT2CLR_gpio_on1_cfg_port_START     (28)
#define SOC_TZPC_TZPCDECPROT2CLR_gpio_on1_cfg_port_END       (28)
#define SOC_TZPC_TZPCDECPROT2CLR_gpio_on2_cfg_port_START     (29)
#define SOC_TZPC_TZPCDECPROT2CLR_gpio_on2_cfg_port_END       (29)
#define SOC_TZPC_TZPCDECPROT2CLR_gpio_on3_cfg_port_START     (30)
#define SOC_TZPC_TZPCDECPROT2CLR_gpio_on3_cfg_port_END       (30)
#define SOC_TZPC_TZPCDECPROT2CLR_mcu_subsys_cfg_port_START   (31)
#define SOC_TZPC_TZPCDECPROT2CLR_mcu_subsys_cfg_port_END     (31)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT3STAT_UNION
 结构说明  : TZPCDECPROT3STAT 寄存器结构定义。地址偏移量:0x0824，初值:0x0BFFFFFF，宽度:32
 寄存器说明: 安全设置状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sci0_cfg_port      : 1;  /* bit[0]    : SCI0_CFG_Port安全状态：
                                                              0：安全
                                                              1：非安全 */
        unsigned int  sci1_cfg_port      : 1;  /* bit[1]    : SCI1_CFG_Port安全状态：
                                                              0：安全
                                                              1：非安全 */
        unsigned int  sram_on_slave_port : 1;  /* bit[2]    : SRAM_ON_Slave_Port安全状态：
                                                              0：安全
                                                              1：非安全 */
        unsigned int  scon_sec_ctrl      : 1;  /* bit[3]    : SCON_SEC_CTRL安全状态：
                                                              0：安全
                                                              1：非安全 */
        unsigned int  scon_sec_ctrl_rd0  : 1;  /* bit[4]    : SCON_SEC_CTRL_RD0安全状态：
                                                              0：安全
                                                              1：非安全 */
        unsigned int  scon_sec_ctrl_rd1  : 1;  /* bit[5]    : SCON_SEC_CTRL_RD1安全状态：
                                                              0：安全
                                                              1：非安全 */
        unsigned int  sec_cfg_edc0       : 8;  /* bit[6-13] : SEC_CFG_EDC0[7:0]安全状态：
                                                              0：安全
                                                              1：非安全
                                                              具体含义参考EDC0接口说明 */
        unsigned int  sec_cfg_dsi        : 1;  /* bit[14]   : SEC_CFG_DSI安全状态：
                                                              0：安全
                                                              1：非安全
                                                              具体含义参考DSI接口说明 */
        unsigned int  sec_cfg_hdmi       : 5;  /* bit[15-19]: SEC_CFG_HDMI[4:0]安全状态：
                                                              0：安全
                                                              1：非安全
                                                              具体含义参考HDMI接口说明 */
        unsigned int  sec_cfg_edc1       : 7;  /* bit[20-26]: SEC_CFG_EDC1[6:0]安全控制状态：
                                                              0：安全
                                                              1：非安全
                                                              具体含义参考EDC1接口说明 */
        unsigned int  ipc_sec_cfg_port   : 1;  /* bit[27]   : IPC_SEC_CFG_Port安全状态：
                                                              0：安全
                                                              1：非安全 */
        unsigned int  reserved           : 4;  /* bit[28-31]: reserved */
    } reg;
} SOC_TZPC_TZPCDECPROT3STAT_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT3STAT_sci0_cfg_port_START       (0)
#define SOC_TZPC_TZPCDECPROT3STAT_sci0_cfg_port_END         (0)
#define SOC_TZPC_TZPCDECPROT3STAT_sci1_cfg_port_START       (1)
#define SOC_TZPC_TZPCDECPROT3STAT_sci1_cfg_port_END         (1)
#define SOC_TZPC_TZPCDECPROT3STAT_sram_on_slave_port_START  (2)
#define SOC_TZPC_TZPCDECPROT3STAT_sram_on_slave_port_END    (2)
#define SOC_TZPC_TZPCDECPROT3STAT_scon_sec_ctrl_START       (3)
#define SOC_TZPC_TZPCDECPROT3STAT_scon_sec_ctrl_END         (3)
#define SOC_TZPC_TZPCDECPROT3STAT_scon_sec_ctrl_rd0_START   (4)
#define SOC_TZPC_TZPCDECPROT3STAT_scon_sec_ctrl_rd0_END     (4)
#define SOC_TZPC_TZPCDECPROT3STAT_scon_sec_ctrl_rd1_START   (5)
#define SOC_TZPC_TZPCDECPROT3STAT_scon_sec_ctrl_rd1_END     (5)
#define SOC_TZPC_TZPCDECPROT3STAT_sec_cfg_edc0_START        (6)
#define SOC_TZPC_TZPCDECPROT3STAT_sec_cfg_edc0_END          (13)
#define SOC_TZPC_TZPCDECPROT3STAT_sec_cfg_dsi_START         (14)
#define SOC_TZPC_TZPCDECPROT3STAT_sec_cfg_dsi_END           (14)
#define SOC_TZPC_TZPCDECPROT3STAT_sec_cfg_hdmi_START        (15)
#define SOC_TZPC_TZPCDECPROT3STAT_sec_cfg_hdmi_END          (19)
#define SOC_TZPC_TZPCDECPROT3STAT_sec_cfg_edc1_START        (20)
#define SOC_TZPC_TZPCDECPROT3STAT_sec_cfg_edc1_END          (26)
#define SOC_TZPC_TZPCDECPROT3STAT_ipc_sec_cfg_port_START    (27)
#define SOC_TZPC_TZPCDECPROT3STAT_ipc_sec_cfg_port_END      (27)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT3SET_UNION
 结构说明  : TZPCDECPROT3SET 寄存器结构定义。地址偏移量:0x0828，初值:0x00000000，宽度:32
 寄存器说明: 非安全设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sci0_cfg_port      : 1;  /* bit[0]    : SCI0_CFG_Port非安全配置：
                                                              0：无效
                                                              1：非安全 */
        unsigned int  sci1_cfg_port      : 1;  /* bit[1]    : SCI1_CFG_Port非安全配置：
                                                              0：无效
                                                              1：非安全 */
        unsigned int  sram_on_slave_port : 1;  /* bit[2]    : SRAM_ON_Slave_Port非安全配置：
                                                              0：无效
                                                              1：非安全 */
        unsigned int  scon_sec_ctrl      : 1;  /* bit[3]    : SCON_SEC_CTRL非安全配置：
                                                              0：无效
                                                              1：非安全 */
        unsigned int  scon_sec_ctrl_rd0  : 1;  /* bit[4]    : SCON_SEC_CTRL_RD0非安全配置：
                                                              0：无效
                                                              1：非安全 */
        unsigned int  scon_sec_ctrl_rd1  : 1;  /* bit[5]    : SCON_SEC_CTRL_RD1非安全配置：
                                                              0：无效
                                                              1：非安全 */
        unsigned int  sec_cfg_edc0       : 8;  /* bit[6-13] : SEC_CFG_EDC0[7:0]非安全配置：
                                                              0：无效
                                                              1：非安全
                                                              具体含义参考EDC0接口说明 */
        unsigned int  sec_cfg_dsi        : 1;  /* bit[14]   : SEC_CFG_DSI非安全配置：
                                                              0：无效
                                                              1：非安全
                                                              具体含义参考DSI接口说明 */
        unsigned int  sec_cfg_hdmi       : 5;  /* bit[15-19]: SEC_CFG_HDMI[4:0]非安全配置：
                                                              0：无效
                                                              1：非安全
                                                              具体含义参考HDMI接口说明 */
        unsigned int  sec_cfg_edc1       : 7;  /* bit[20-26]: SEC_CFG_EDC1[6:0]非安全配置：
                                                              0：无效
                                                              1：非安全
                                                              具体含义参考EDC1接口说明 */
        unsigned int  ipc_sec_cfg_port   : 1;  /* bit[27]   : IPC_SEC_CFG_Port非安全配置：
                                                              0：无效
                                                              1：非安全 */
        unsigned int  reserved           : 4;  /* bit[28-31]: reserved */
    } reg;
} SOC_TZPC_TZPCDECPROT3SET_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT3SET_sci0_cfg_port_START       (0)
#define SOC_TZPC_TZPCDECPROT3SET_sci0_cfg_port_END         (0)
#define SOC_TZPC_TZPCDECPROT3SET_sci1_cfg_port_START       (1)
#define SOC_TZPC_TZPCDECPROT3SET_sci1_cfg_port_END         (1)
#define SOC_TZPC_TZPCDECPROT3SET_sram_on_slave_port_START  (2)
#define SOC_TZPC_TZPCDECPROT3SET_sram_on_slave_port_END    (2)
#define SOC_TZPC_TZPCDECPROT3SET_scon_sec_ctrl_START       (3)
#define SOC_TZPC_TZPCDECPROT3SET_scon_sec_ctrl_END         (3)
#define SOC_TZPC_TZPCDECPROT3SET_scon_sec_ctrl_rd0_START   (4)
#define SOC_TZPC_TZPCDECPROT3SET_scon_sec_ctrl_rd0_END     (4)
#define SOC_TZPC_TZPCDECPROT3SET_scon_sec_ctrl_rd1_START   (5)
#define SOC_TZPC_TZPCDECPROT3SET_scon_sec_ctrl_rd1_END     (5)
#define SOC_TZPC_TZPCDECPROT3SET_sec_cfg_edc0_START        (6)
#define SOC_TZPC_TZPCDECPROT3SET_sec_cfg_edc0_END          (13)
#define SOC_TZPC_TZPCDECPROT3SET_sec_cfg_dsi_START         (14)
#define SOC_TZPC_TZPCDECPROT3SET_sec_cfg_dsi_END           (14)
#define SOC_TZPC_TZPCDECPROT3SET_sec_cfg_hdmi_START        (15)
#define SOC_TZPC_TZPCDECPROT3SET_sec_cfg_hdmi_END          (19)
#define SOC_TZPC_TZPCDECPROT3SET_sec_cfg_edc1_START        (20)
#define SOC_TZPC_TZPCDECPROT3SET_sec_cfg_edc1_END          (26)
#define SOC_TZPC_TZPCDECPROT3SET_ipc_sec_cfg_port_START    (27)
#define SOC_TZPC_TZPCDECPROT3SET_ipc_sec_cfg_port_END      (27)


/*****************************************************************************
 结构名    : SOC_TZPC_TZPCDECPROT3CLR_UNION
 结构说明  : TZPCDECPROT3CLR 寄存器结构定义。地址偏移量:0x082C，初值:0x00000000，宽度:32
 寄存器说明: 安全设置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sci0_cfg_port      : 1;  /* bit[0]    : SCI0_CFG_Port安全配置：
                                                              0：无效
                                                              1：安全 */
        unsigned int  sci1_cfg_port      : 1;  /* bit[1]    : SCI1_CFG_Port安全配置：
                                                              0：无效
                                                              1：安全 */
        unsigned int  sram_on_slave_port : 1;  /* bit[2]    : SRAM_ON_Slave_Port安全配置：
                                                              0：无效
                                                              1：安全 */
        unsigned int  scon_sec_ctrl      : 1;  /* bit[3]    : SCON_SEC_CTRL安全配置：
                                                              0：无效
                                                              1：安全 */
        unsigned int  scon_sec_ctrl_rd0  : 1;  /* bit[4]    : SCON_SEC_CTRL_RD0安全配置：
                                                              0：无效
                                                              1：安全 */
        unsigned int  scon_sec_ctrl_rd1  : 1;  /* bit[5]    : SCON_SEC_CTRL_RD1安全配置：
                                                              0：无效
                                                              1：安全 */
        unsigned int  sec_cfg_edc0       : 8;  /* bit[6-13] : SEC_CFG_EDC0[7:0]安全配置：
                                                              0：无效
                                                              1：安全
                                                              具体含义参考EDC0接口说明 */
        unsigned int  sec_cfg_dsi        : 1;  /* bit[14]   : SEC_CFG_DSI安全配置：
                                                              0：无效
                                                              1：安全
                                                              具体含义参考DSI接口说明 */
        unsigned int  sec_cfg_hdmi       : 5;  /* bit[15-19]: SEC_CFG_HDMI[4:0]安全配置：
                                                              0：无效
                                                              1：安全
                                                              具体含义参考HDMI接口说明 */
        unsigned int  sec_cfg_edc1       : 7;  /* bit[20-26]: SEC_CFG_EDC1[6:0]安全配置：
                                                              0：无效
                                                              1：安全
                                                              具体含义参考EDC1接口说明 */
        unsigned int  ipc_sec_cfg_port   : 1;  /* bit[27]   : IPC_SEC_CFG_Port安全配置：
                                                              0：无效
                                                              1：安全 */
        unsigned int  reserved           : 4;  /* bit[28-31]: reserved */
    } reg;
} SOC_TZPC_TZPCDECPROT3CLR_UNION;
#endif
#define SOC_TZPC_TZPCDECPROT3CLR_sci0_cfg_port_START       (0)
#define SOC_TZPC_TZPCDECPROT3CLR_sci0_cfg_port_END         (0)
#define SOC_TZPC_TZPCDECPROT3CLR_sci1_cfg_port_START       (1)
#define SOC_TZPC_TZPCDECPROT3CLR_sci1_cfg_port_END         (1)
#define SOC_TZPC_TZPCDECPROT3CLR_sram_on_slave_port_START  (2)
#define SOC_TZPC_TZPCDECPROT3CLR_sram_on_slave_port_END    (2)
#define SOC_TZPC_TZPCDECPROT3CLR_scon_sec_ctrl_START       (3)
#define SOC_TZPC_TZPCDECPROT3CLR_scon_sec_ctrl_END         (3)
#define SOC_TZPC_TZPCDECPROT3CLR_scon_sec_ctrl_rd0_START   (4)
#define SOC_TZPC_TZPCDECPROT3CLR_scon_sec_ctrl_rd0_END     (4)
#define SOC_TZPC_TZPCDECPROT3CLR_scon_sec_ctrl_rd1_START   (5)
#define SOC_TZPC_TZPCDECPROT3CLR_scon_sec_ctrl_rd1_END     (5)
#define SOC_TZPC_TZPCDECPROT3CLR_sec_cfg_edc0_START        (6)
#define SOC_TZPC_TZPCDECPROT3CLR_sec_cfg_edc0_END          (13)
#define SOC_TZPC_TZPCDECPROT3CLR_sec_cfg_dsi_START         (14)
#define SOC_TZPC_TZPCDECPROT3CLR_sec_cfg_dsi_END           (14)
#define SOC_TZPC_TZPCDECPROT3CLR_sec_cfg_hdmi_START        (15)
#define SOC_TZPC_TZPCDECPROT3CLR_sec_cfg_hdmi_END          (19)
#define SOC_TZPC_TZPCDECPROT3CLR_sec_cfg_edc1_START        (20)
#define SOC_TZPC_TZPCDECPROT3CLR_sec_cfg_edc1_END          (26)
#define SOC_TZPC_TZPCDECPROT3CLR_ipc_sec_cfg_port_START    (27)
#define SOC_TZPC_TZPCDECPROT3CLR_ipc_sec_cfg_port_END      (27)






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

#endif /* end of soc_tzpc_interface.h */
