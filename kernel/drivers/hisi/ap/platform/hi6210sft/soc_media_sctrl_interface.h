

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_MEDIA_SCTRL_INTERFACE_H__
#define __SOC_MEDIA_SCTRL_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) media_sc_reg
 ****************************************************************************/
/* 寄存器说明：MEDIA控制寄存器。功能包含CODEC的RFS/RAS/RAD类型memory 控制
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_ADDR(base) ((base) + (0x00EC))

/* 寄存器说明：MEDIA控制寄存器。功能包含G3D的RFS类型memory控制
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_ADDR(base) ((base) + (0x00F0))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含ADE和SMMU的RAS/RFS类型memory控制
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ADDR(base) ((base) + (0x518))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含G3D控制以及RFD类型memory控制
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_ADDR(base) ((base) + (0x500))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含ISP控制(I2C0和I2C1)
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_ADDR(base) ((base) + (0x504))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含CODEC_VPU控制以及CODEC的RFD类型memory控制
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_ADDR(base) ((base) + (0x508))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含CODEC JPEG控制
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL3_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL3_ADDR(base) ((base) + (0x50C))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含ADE的RFD类型memory控制
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ADDR(base) ((base) + (0x514))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含对G3D，ADE,CODEC时钟源的选择，及MMU和ISP的控制，MEMORY SHARE选择
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(base) ((base) + (0x51C))

/* 寄存器说明：MEDIA子系统状态寄存器，功能包含G3D状态
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT0_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT0_ADDR(base) ((base) + (0x540))

/* 寄存器说明：MEDIA子系统状态寄存器，功能包含ISP状态
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT1_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT1_ADDR(base) ((base) + (0x544))

/* 寄存器说明：MEDIA子系统状态寄存器，功能包含CODEC VPU状态
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT2_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT2_ADDR(base) ((base) + (0x548))

/* 寄存器说明：MEDIA子系统状态寄存器，功能包含CODEC JPEG状态
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT3_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT3_ADDR(base) ((base) + (0x54C))

/* 寄存器说明：MEDIA子系统状态寄存器，功能包含ADE/NOC的状态
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_ADDR(base) ((base) + (0x550))

/* 寄存器说明：MEDIA子系统NOC自动调频参数配置寄存器
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_ADDR(base)   ((base) + (0x510))

/* 寄存器说明：MEDIA时钟参数配置寄存器0(ISP和NOC)
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_ADDR(base)   ((base) + (0xCBC))

/* 寄存器说明：MEDIA时钟参数配置寄存器1（ADE）
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_ADDR(base)   ((base) + (0xCC0))

/* 寄存器说明：MEDIA时钟参数配置寄存器2(CODEC和GPU)
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_ADDR(base)   ((base) + (0xCC4))

/* 寄存器说明：MEDIA子系统时钟使能寄存器，对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_ADDR(base)     ((base) + (0x520))

/* 寄存器说明：MEDIA子系统时钟禁止寄存器，对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_ADDR(base)    ((base) + (0x524))

/* 寄存器说明：MEDIA子系统时钟状态寄存器，由SC_MEDIA_CLKEN和SC_MEDIA_CLKDIS配置进行状态修改。
            媒体时钟控制信号需经过在MEDIA_CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
            每个Bit含义：
            0：时钟关闭；
            1：时钟打开。
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_ADDR(base)  ((base) + (0x528))

/* 寄存器说明：MEDIA子系统复位使能寄存器，用于使能复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(base)     ((base) + (0x52C))

/* 寄存器说明：MEDIA子系统解复位寄存器，用于禁止复位。对比特写1，对应IP解除复位；写0不影响复位使能的状态。
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(base)    ((base) + (0x530))

/* 寄存器说明：MEDIA子系统复位使能状态寄存器。由SC_MEDIA_RSTEN和SC_MEDIA_RSTDIS配置进行状态修改。解复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
            每个Bit含义：
            0：复位不使能；
            1：复位使能。
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_ADDR(base)  ((base) + (0x534))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含TEST_MUX控制
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL6_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL6_ADDR(base) ((base) + (0x538))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含hurry/press配置，详细请参见NOC设计说明书
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_ADDR(base) ((base) + (0x53C))

/* 寄存器说明：MEDIA子系统控制寄存器，功能包含hurry/press/qos配置，详细请参见NOC设计说明书
   位域定义UNION结构:  SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_UNION */
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_ADDR(base) ((base) + (0x554))





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
                     (1/1) media_sc_reg
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_UNION
 结构说明  : SC_MEDIA_PERIPH_CTRL0 寄存器结构定义。地址偏移量:0x00EC，初值:0x0C9B0C9B，宽度:32
 寄存器说明: MEDIA控制寄存器。功能包含CODEC的RFS/RAS/RAD类型memory 控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  codec_rad_ramctrl_ema      : 3;  /* bit[0-2]  : memory读速率控制，随此值的增大，memory读延时增大 */
        unsigned int  reserved_0                 : 1;  /* bit[3]    : reserved */
        unsigned int  codec_rad_ramctrl_emaw     : 2;  /* bit[4-5]  : memory写速率控制，随此值的增大，memory写延时增大 */
        unsigned int  codec_rad_ramctrl_emas     : 1;  /* bit[6]    : 对HSmemory有作用；
                                                                      对HDmemory为保留bit，无意义； */
        unsigned int  codec_rad_ramctrl_colldisn : 1;  /* bit[7]    : 同时读写一个地址时，memory的行为
                                                                      0：读写都不保证正确
                                                                      1：写保证正确，读不保证正确 */
        unsigned int  reserved_1                 : 2;  /* bit[8-9]  : reserved */
        unsigned int  codec_rad_ramctrl_s_ret1n  : 1;  /* bit[10]   : retention控制1
                                                                      0：retention模式1；
                                                                      1：正常工作模式。 */
        unsigned int  codec_rad_ramctrl_s_ret2n  : 1;  /* bit[11]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                      retention控制2
                                                                      0：retention模式2；
                                                                      1：正常工作模式。 */
        unsigned int  codec_rad_ramctrl_s_pgen   : 1;  /* bit[12]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                      0：正常工作模式；
                                                                      1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_2                 : 3;  /* bit[13-15]: reserved */
        unsigned int  codec_rfs_ramctrl_ema      : 3;  /* bit[16-18]: memory读速率控制，随此值的增大，memory读延时增大
                                                                      A口和B口共享此配置。 */
        unsigned int  reserved_3                 : 1;  /* bit[19]   : reserved */
        unsigned int  codec_rfs_ramctrl_emaw     : 2;  /* bit[20-21]: memory写速率控制，随此值的增大，memory写延时增大，
                                                                      A口和B口共享此配置。 */
        unsigned int  codec_rfs_ramctrl_emas     : 1;  /* bit[22]   : A口和B口共享此配置。
                                                                      对HSmemory有作用；
                                                                      对HDmemory为保留bit，无意义； */
        unsigned int  reserved_4                 : 3;  /* bit[23-25]: reserved */
        unsigned int  codec_rfs_ramctrl_s_ret1n  : 1;  /* bit[26]   : retention控制1
                                                                      0：retention模式1；
                                                                      1：正常工作模式。 */
        unsigned int  codec_rfs_ramctrl_s_ret2n  : 1;  /* bit[27]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                      retention控制2
                                                                      0：retention模式2；
                                                                      1：正常工作模式。 */
        unsigned int  codec_rfs_ramctrl_s_pgen   : 1;  /* bit[28]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                      0：正常工作模式；
                                                                      1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_5                 : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_ema_START       (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_ema_END         (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_emaw_START      (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_emaw_END        (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_emas_START      (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_emas_END        (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_colldisn_START  (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_colldisn_END    (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_s_ret1n_START   (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_s_ret1n_END     (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_s_ret2n_START   (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_s_ret2n_END     (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_s_pgen_START    (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rad_ramctrl_s_pgen_END      (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_ema_START       (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_ema_END         (18)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_emaw_START      (20)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_emaw_END        (21)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_emas_START      (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_emas_END        (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_s_ret1n_START   (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_s_ret1n_END     (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_s_ret2n_START   (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_s_ret2n_END     (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_s_pgen_START    (28)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL0_codec_rfs_ramctrl_s_pgen_END      (28)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_UNION
 结构说明  : SC_MEDIA_PERIPH_CTRL1 寄存器结构定义。地址偏移量:0x00F0，初值:0x0C9B0C9B，宽度:32
 寄存器说明: MEDIA控制寄存器。功能包含G3D的RFS类型memory控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0              : 16; /* bit[0-15] : 保留 */
        unsigned int  g3d_rfs_ramctrl_ema     : 3;  /* bit[16-18]: memery写速率控制，随此值的增大，memery写延时增大 */
        unsigned int  reserved_1              : 1;  /* bit[19]   : 保留 */
        unsigned int  g3d_rfs_ramctrl_emaw    : 2;  /* bit[20-21]: memery写速率控制，随此值的增大，memery写延时增大 */
        unsigned int  g3d_rfs_ramctrl_emas    : 1;  /* bit[22]   : 对HSmemory有作用；
                                                                   对HDmemory为保留bit，无意义； */
        unsigned int  reserved_2              : 3;  /* bit[23-25]: 保留 */
        unsigned int  g3d_rfs_ramctrl_s_ret1n : 1;  /* bit[26]   : retention控制1
                                                                   0：retention模式1；
                                                                   1：正常工作模式。 */
        unsigned int  g3d_rfs_ramctrl_s_ret2n : 1;  /* bit[27]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                   retention控制2
                                                                   0：retention模式2；
                                                                   1：正常工作模式。 */
        unsigned int  g3d_rfs_ramctrl_s_pgen  : 1;  /* bit[28]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                   0：正常工作模式；
                                                                   1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_3              : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_ema_START      (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_ema_END        (18)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_emaw_START     (20)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_emaw_END       (21)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_emas_START     (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_emas_END       (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_s_ret1n_START  (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_s_ret1n_END    (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_s_ret2n_START  (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_s_ret2n_END    (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_s_pgen_START   (28)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL1_g3d_rfs_ramctrl_s_pgen_END     (28)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_UNION
 结构说明  : SC_MEDIA_PERIPH_CTRL2 寄存器结构定义。地址偏移量:0x518，初值:0x0C9B0C9B，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含ADE和SMMU的RAS/RFS类型memory控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  smmu_rfs_ramctrl_ema     : 3;  /* bit[0-2]  : memery写速率控制，随此值的增大，memery写延时增大 */
        unsigned int  reserved_0               : 1;  /* bit[3]    : 保留 */
        unsigned int  smmu_rfs_ramctrl_emaw    : 2;  /* bit[4-5]  : memery写速率控制，随此值的增大，memery写延时增大 */
        unsigned int  smmu_rfs_ramctrl_emas    : 1;  /* bit[6]    : 对HSmemory有作用；
                                                                    对HDmemory为保留bit，无意义； */
        unsigned int  reserved_1               : 3;  /* bit[7-9]  : reserved */
        unsigned int  smmu_rfs_ramctrl_s_ret1n : 1;  /* bit[10]   : retention控制1
                                                                    0：retention模式1；
                                                                    1：正常工作模式。 */
        unsigned int  smmu_rfs_ramctrl_s_ret2n : 1;  /* bit[11]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    retention控制2
                                                                    0：retention模式2；
                                                                    1：正常工作模式。 */
        unsigned int  smmu_rfs_ramctrl_s_pgen  : 1;  /* bit[12]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    0：正常工作模式；
                                                                    1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_2               : 3;  /* bit[13-15]: reserved */
        unsigned int  ade_rfs_ramctrl_ema      : 3;  /* bit[16-18]: memery写速率控制，随此值的增大，memery写延时增大 */
        unsigned int  reserved_3               : 1;  /* bit[19]   : 保留 */
        unsigned int  ade_rfs_ramctrl_emaw     : 2;  /* bit[20-21]: memery写速率控制，随此值的增大，memery写延时增大 */
        unsigned int  ade_rfs_ramctrl_emas     : 1;  /* bit[22]   : 对HSmemory有作用；
                                                                    对HDmemory为保留bit，无意义； */
        unsigned int  reserved_4               : 3;  /* bit[23-25]: reserved */
        unsigned int  ade_rfs_ramctrl_s_ret1n  : 1;  /* bit[26]   : retention控制1
                                                                    0：retention模式1；
                                                                    1：正常工作模式。 */
        unsigned int  ade_rfs_ramctrl_s_ret2n  : 1;  /* bit[27]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    retention控制2
                                                                    0：retention模式2；
                                                                    1：正常工作模式。 */
        unsigned int  ade_rfs_ramctrl_s_pgen   : 1;  /* bit[28]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    0：正常工作模式；
                                                                    1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_5               : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_ema_START      (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_ema_END        (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_emaw_START     (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_emaw_END       (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_emas_START     (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_emas_END       (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_s_ret1n_START  (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_s_ret1n_END    (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_s_ret2n_START  (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_s_ret2n_END    (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_s_pgen_START   (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_smmu_rfs_ramctrl_s_pgen_END     (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_ema_START       (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_ema_END         (18)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_emaw_START      (20)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_emaw_END        (21)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_emas_START      (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_emas_END        (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_s_ret1n_START   (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_s_ret1n_END     (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_s_ret2n_START   (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_s_ret2n_END     (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_s_pgen_START    (28)
#define SOC_MEDIA_SCTRL_SC_MEDIA_PERIPH_CTRL2_ade_rfs_ramctrl_s_pgen_END      (28)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_UNION
 结构说明  : SC_MEDIA_SUBSYS_CTRL0 寄存器结构定义。地址偏移量:0x500，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含G3D控制以及RFD类型memory控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0               : 16; /* bit[0-15] : 保留 */
        unsigned int  g3d_rfd_ramctrl_emaa     : 3;  /* bit[16-18]: memory读速率控制，随此值的增大，memory读延时增大 */
        unsigned int  g3d_rfd_ramctrl_emab     : 3;  /* bit[19-21]: 含义不明确 */
        unsigned int  g3d_rfd_ramctrl_emasa    : 1;  /* bit[22]   : 含义不明确 */
        unsigned int  g3d_rfd_ramctrl_colldisn : 1;  /* bit[23]   : 同时读写一个地址时，memory的行为
                                                                    0：读写都不保证正确
                                                                    1：写保证正确，读不保证正确 */
        unsigned int  reserved_1               : 2;  /* bit[24-25]: reserved */
        unsigned int  g3d_rfd_ramctrl_s_ret1n  : 1;  /* bit[26]   : retention控制1
                                                                    0：retention模式1；
                                                                    1：正常工作模式。 */
        unsigned int  g3d_rfd_ramctrl_s_ret2n  : 1;  /* bit[27]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    retention控制2
                                                                    0：retention模式2；
                                                                    1：正常工作模式。 */
        unsigned int  g3d_rfd_ramctrl_s_pgen   : 1;  /* bit[28]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    0：正常工作模式；
                                                                    1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_2               : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_emaa_START      (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_emaa_END        (18)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_emab_START      (19)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_emab_END        (21)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_emasa_START     (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_emasa_END       (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_colldisn_START  (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_colldisn_END    (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_s_ret1n_START   (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_s_ret1n_END     (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_s_ret2n_START   (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_s_ret2n_END     (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_s_pgen_START    (28)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL0_g3d_rfd_ramctrl_s_pgen_END      (28)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_UNION
 结构说明  : SC_MEDIA_SUBSYS_CTRL1 寄存器结构定义。地址偏移量:0x504，初值:0x0002B2B0，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含ISP控制(I2C0和I2C1)
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_subsys_ctrl1_lp_ctrl       : 2;  /* bit[0-1]  : sc_ctrl_isp[1]:表示isp的shutdown控制，高有效
                                                                            sc_ctrl_isp[0]:表示对isp的dfs请求 */
        unsigned int  media_subsys_ctrl1_i2c0_oden     : 1;  /* bit[2]    : ISP的I2C0的OD和非OD模式选择,
                                                                            1：非OD模式
                                                                            0：OD模式 */
        unsigned int  media_subsys_ctrl1_i2c0_dly_en   : 1;  /* bit[3]    : ISP的I2C0的延迟功能时能,
                                                                            1：表示时能
                                                                            0：表示禁止 */
        unsigned int  media_subsys_ctrl1_i2c0_dly_time : 6;  /* bit[4-9]  : 在I2C0的延迟功能使能时，延迟时间配置 */
        unsigned int  media_subsys_ctrl1_i2c1_oden     : 1;  /* bit[10]   : ISP的I2C1的OD和非OD模式选择,
                                                                            1：非OD模式
                                                                            0：OD模式 */
        unsigned int  media_subsys_ctrl1_i2c1_dly_en   : 1;  /* bit[11]   : ISP的I2C1的延迟功能时能,
                                                                            1：表示时能
                                                                            0：表示禁止 */
        unsigned int  media_subsys_ctrl1_i2c1_dly_time : 6;  /* bit[12-17]: 在I2C1的延迟功能使能时，延迟clock数 */
        unsigned int  media_subsys_ctrl1               : 14; /* bit[18-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_lp_ctrl_START        (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_lp_ctrl_END          (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c0_oden_START      (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c0_oden_END        (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c0_dly_en_START    (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c0_dly_en_END      (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c0_dly_time_START  (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c0_dly_time_END    (9)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c1_oden_START      (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c1_oden_END        (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c1_dly_en_START    (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c1_dly_en_END      (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c1_dly_time_START  (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_i2c1_dly_time_END    (17)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_START                (18)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL1_media_subsys_ctrl1_END                  (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_UNION
 结构说明  : SC_MEDIA_SUBSYS_CTRL2 寄存器结构定义。地址偏移量:0x508，初值:0x0C9B0000，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含CODEC_VPU控制以及CODEC的RFD类型memory控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  auto_codec_vpu_clken       : 1;  /* bit[0]    : codec vpu的cclk的自动调频使能信号：
                                                                      1：表示使能codec的cclk的自动时钟调频
                                                                      0：表示禁止codec的cclk的自动时钟调频 */
        unsigned int  reserved_0                 : 15; /* bit[1-15] : 保留 */
        unsigned int  codec_rfd_ramctrl_emaa     : 3;  /* bit[16-18]: memory读速率控制，随此值的增大，memory读延时增大 */
        unsigned int  codec_rfd_ramctrl_emab     : 3;  /* bit[19-21]: 含义不明确 */
        unsigned int  codec_rfd_ramctrl_emasa    : 1;  /* bit[22]   : 含义不明确 */
        unsigned int  codec_rfd_ramctrl_colldisn : 1;  /* bit[23]   : 同时读写一个地址时，memory的行为
                                                                      0：读写都不保证正确
                                                                      1：写保证正确，读不保证正确 */
        unsigned int  reserved_1                 : 2;  /* bit[24-25]: reserved */
        unsigned int  codec_rfd_ramctrl_s_ret1n  : 1;  /* bit[26]   : retention控制1
                                                                      0：retention模式1；
                                                                      1：正常工作模式。 */
        unsigned int  codec_rfd_ramctrl_s_ret2n  : 1;  /* bit[27]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                      retention控制2
                                                                      0：retention模式2；
                                                                      1：正常工作模式。 */
        unsigned int  codec_rfd_ramctrl_s_pgen   : 1;  /* bit[28]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                      0：正常工作模式；
                                                                      1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_2                 : 3;  /* bit[29-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_auto_codec_vpu_clken_START        (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_auto_codec_vpu_clken_END          (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_emaa_START      (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_emaa_END        (18)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_emab_START      (19)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_emab_END        (21)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_emasa_START     (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_emasa_END       (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_colldisn_START  (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_colldisn_END    (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_s_ret1n_START   (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_s_ret1n_END     (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_s_ret2n_START   (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_s_ret2n_END     (27)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_s_pgen_START    (28)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL2_codec_rfd_ramctrl_s_pgen_END      (28)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL3_UNION
 结构说明  : SC_MEDIA_SUBSYS_CTRL3 寄存器结构定义。地址偏移量:0x50C，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含CODEC JPEG控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_subsys_ctrl3 : 32; /* bit[0-31]: codec_jpeg subsyscrtl[31:0] */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL3_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL3_media_subsys_ctrl3_START  (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL3_media_subsys_ctrl3_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_UNION
 结构说明  : SC_MEDIA_SUBSYS_CTRL4 寄存器结构定义。地址偏移量:0x514，初值:0x00000C9B，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含ADE的RFD类型memory控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ade_rfd_ramctrl_ema      : 3;  /* bit[0-2]  : memery写速率控制，随此值的增大，memery写延时增大 */
        unsigned int  ade_rfd_ramctrl_emab     : 3;  /* bit[3-5]  : 含义不明确 */
        unsigned int  ade_rfd_ramctrl_emasa    : 1;  /* bit[6]    : 含义不明确 */
        unsigned int  ade_rfd_ramctrl_colldisn : 1;  /* bit[7]    : 同时读写一个地址时，memory的行为
                                                                    0：读写都不保证正确
                                                                    1：写保证正确，读不保证正确 */
        unsigned int  reserved_0               : 2;  /* bit[8-9]  : reserved */
        unsigned int  ade_rfd_ramctrl_s_ret1n  : 1;  /* bit[10]   : retention控制1
                                                                    0：retention模式1；
                                                                    1：正常工作模式。 */
        unsigned int  ade_rfd_ramctrl_s_ret2n  : 1;  /* bit[11]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    retention控制2
                                                                    0：retention模式2；
                                                                    1：正常工作模式。 */
        unsigned int  ade_rfd_ramctrl_s_pgen   : 1;  /* bit[12]   : 只有当打开powergating选项时有此功能，对没有powergating功能的memory，此端口为保留，无意义。
                                                                    0：正常工作模式；
                                                                    1：配合ret1n/ret2n完成retention。 */
        unsigned int  reserved_1               : 3;  /* bit[13-15]: reserved */
        unsigned int  media_subsys_ctrl4       : 16; /* bit[16-31]: ade subsyscrtl[31:16] */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_ema_START       (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_ema_END         (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_emab_START      (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_emab_END        (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_emasa_START     (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_emasa_END       (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_colldisn_START  (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_colldisn_END    (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_s_ret1n_START   (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_s_ret1n_END     (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_s_ret2n_START   (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_s_ret2n_END     (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_s_pgen_START    (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_ade_rfd_ramctrl_s_pgen_END      (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_media_subsys_ctrl4_START        (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL4_media_subsys_ctrl4_END          (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_UNION
 结构说明  : SC_MEDIA_SUBSYS_CTRL5 寄存器结构定义。地址偏移量:0x51C，初值:0x00000084，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含对G3D，ADE,CODEC时钟源的选择，及MMU和ISP的控制，MEMORY SHARE选择
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_clk_sel_ade     : 1;  /* bit[0]   : ADE PIXEL时钟源的选择：
                                                                1：表示选择媒体PLL 1.44GHz
                                                                0：表示选择系统PLL 1.2GHz */
        unsigned int  media_clk_sel_g3d     : 1;  /* bit[1]   : G3D的时钟源选择：
                                                                1：表示选择系统PLL 1.2GHz
                                                                0：表示选择G3D PLL 1.0GHz */
        unsigned int  media_mmu_sel         : 1;  /* bit[2]   : MMU bypass信号:
                                                                1:表示bypass MMU逻辑，axi总线直接输出
                                                                0:表示非bypass模式 */
        unsigned int  isp_fsin0_poen        : 1;  /* bit[3]   : 控制isp的FSIN0
                                                                1：帧同步1
                                                                0：帧同步0 */
        unsigned int  isp_fsin1_poen        : 1;  /* bit[4]   : 控制isp的FSIN1
                                                                1：帧同步1
                                                                0：帧同步0 */
        unsigned int  isp_dw_auto_clken     : 1;  /* bit[5]   : ISP的dw axi总线桥自动门控使能
                                                                0：自动门控功能bypass
                                                                1：自动门控功能开启 */
        unsigned int  media_clk_sel_vpu     : 1;  /* bit[6]   : VPU的时钟源选择（V8方案固定选择0）：
                                                                1：表示选择媒体PLL 1.44GHz
                                                                0：表示选择G3D PLL 1.0GHz */
        unsigned int  media_noc_time_out_en : 1;  /* bit[7]   : MEDIA_NOC总线在time out挂死状态下返回ERROR信号的功能开关
                                                                0：此功能bypass
                                                                1：此功能开启 */
        unsigned int  vpu_mem_share_sel     : 1;  /* bit[8]   : CODEC VPU的MEMORY share目标RAM选择
                                                                0：CODEC VPU共享HARQ RAM
                                                                1：CODEC VPU共享ADE RAM */
        unsigned int  reserved              : 23; /* bit[9-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_clk_sel_ade_START      (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_clk_sel_ade_END        (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_clk_sel_g3d_START      (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_clk_sel_g3d_END        (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_mmu_sel_START          (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_mmu_sel_END            (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_isp_fsin0_poen_START         (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_isp_fsin0_poen_END           (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_isp_fsin1_poen_START         (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_isp_fsin1_poen_END           (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_isp_dw_auto_clken_START      (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_isp_dw_auto_clken_END        (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_clk_sel_vpu_START      (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_clk_sel_vpu_END        (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_noc_time_out_en_START  (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_media_noc_time_out_en_END    (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_vpu_mem_share_sel_START      (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_vpu_mem_share_sel_END        (8)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT0_UNION
 结构说明  : SC_MEDIA_SUBSYS_STAT0 寄存器结构定义。地址偏移量:0x540，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统状态寄存器，功能包含G3D状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_subsys_g3d_state0 : 7;  /* bit[0-6] : G3D内部工作状态输出，
                                                                  sc_g3d_stat[6:0]={idle_pp0,idle_ppmmu0,idle_pp1,idle_ppmmu1,idle_gp,idle_gpmmu,idle_l2c}
                                                                  idle_pp0:pp0的处于IDLE
                                                                  idle_ppmmu0:ppmmu0处于IDLE
                                                                  idle_pp1:pp1处于IDLE
                                                                  idle_ppmmu1:ppmmu1处于IDLE
                                                                  idle_gp:gp处于IDLE
                                                                  idle_gpmmu:gpmmu处于IDLE
                                                                  idle_l2c:L2C处于IDLE */
        unsigned int  media_subsys_state0     : 25; /* bit[7-31]: g3dsubsysstat[31:0] */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT0_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT0_media_subsys_g3d_state0_START  (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT0_media_subsys_g3d_state0_END    (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT0_media_subsys_state0_START      (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT0_media_subsys_state0_END        (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT1_UNION
 结构说明  : SC_MEDIA_SUBSYS_STAT1 寄存器结构定义。地址偏移量:0x544，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统状态寄存器，功能包含ISP状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_subsys_isp_state1 : 1;  /* bit[0]   : ISP的dfs响应状态,为1表示响应了dfs */
        unsigned int  media_subsys_state1     : 31; /* bit[1-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT1_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT1_media_subsys_isp_state1_START  (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT1_media_subsys_isp_state1_END    (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT1_media_subsys_state1_START      (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT1_media_subsys_state1_END        (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT2_UNION
 结构说明  : SC_MEDIA_SUBSYS_STAT2 寄存器结构定义。地址偏移量:0x548，初值:0x00000001，宽度:32
 寄存器说明: MEDIA子系统状态寄存器，功能包含CODEC VPU状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_codec_vpu_idle          : 1;  /* bit[0]   : codec的vpu的IDLE信号 */
        unsigned int  media_subsys_codec_vpu_state2 : 31; /* bit[1-31]: codec_vpusubsysstat[31:0] */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT2_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT2_media_codec_vpu_idle_START           (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT2_media_codec_vpu_idle_END             (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT2_media_subsys_codec_vpu_state2_START  (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT2_media_subsys_codec_vpu_state2_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT3_UNION
 结构说明  : SC_MEDIA_SUBSYS_STAT3 寄存器结构定义。地址偏移量:0x54C，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统状态寄存器，功能包含CODEC JPEG状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_subsys_codec_jpeg_state3 : 32; /* bit[0-31]: codec_jpegsubsysstat[31:0] */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT3_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT3_media_subsys_codec_jpeg_state3_START  (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT3_media_subsys_codec_jpeg_state3_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_UNION
 结构说明  : SC_MEDIA_SUBSYS_STAT4 寄存器结构定义。地址偏移量:0x550，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统状态寄存器，功能包含ADE/NOC的状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  smmu_timeout            : 1;  /* bit[0]   : smmu_ram_timeout状态，表示NOC的SMMU通路响应超时，超时门限为4096个32KHz时钟cycle，通过SC_MEDIA_SUBSYS_CTRL5[7]来bypass */
        unsigned int  harq_ram_timeout        : 1;  /* bit[1]   : harq_ram_timeout状态，表示NOC的XG2RAM通路响应超时，超时门限为4096个32KHz时钟cycle，通过SC_MEDIA_SUBSYS_CTRL5[7]来bypass */
        unsigned int  media_subsys_ade_state4 : 30; /* bit[2-31]: ade_subsysstat[31:0] */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_smmu_timeout_START             (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_smmu_timeout_END               (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_harq_ram_timeout_START         (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_harq_ram_timeout_END           (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_media_subsys_ade_state4_START  (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_STAT4_media_subsys_ade_state4_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_UNION
 结构说明  : SC_MEDIA_NOC_DFS 寄存器结构定义。地址偏移量:0x510，初值:0x00040040，宽度:32
 寄存器说明: MEDIA子系统NOC自动调频参数配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_noc_lp_value0  : 4;  /* bit[0-3]  : [3:1]:保留
                                                                [0]:MEDIA NOC总线自动调频使能
                                                                 1：表示使能NOC的自动时钟调频
                                                                 0：表示禁止NOC的自动时钟调频 */
        unsigned int  media_noc_lp_num_in  : 12; /* bit[4-15] : [15:14]:保留
                                                                [13:4]:MEDIA NOC总线进入调频模式的脉冲计数值，计数时钟周期为19.2MHz */
        unsigned int  media_noc_lp_num_out : 12; /* bit[16-27]: [27:26]:保留
                                                                [25:16]:MEDIA NOC总线退出调频模式的脉冲计数值，计数时钟周期为19.2MHz */
        unsigned int  reserved             : 4;  /* bit[28-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_media_noc_lp_value0_START   (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_media_noc_lp_value0_END     (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_media_noc_lp_num_in_START   (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_media_noc_lp_num_in_END     (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_media_noc_lp_num_out_START  (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_NOC_DFS_media_noc_lp_num_out_END    (27)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_UNION
 结构说明  : SC_MEDIA_CLKCFG0 寄存器结构定义。地址偏移量:0xCBC，初值:0xFFFFFFFF，宽度:32
 寄存器说明: MEDIA时钟参数配置寄存器0(ISP和NOC)
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_clkcfg0_noc_value0       : 7;  /* bit[0-6]  : 时钟参数。
                                                                          [6:4]:保留
                                                                          [3:0]:NOC总线工作源时钟分频参数配置； */
        unsigned int  media_clkcfg0_noc_vld0         : 1;  /* bit[7]    : 时钟参数有效指示。 */
        unsigned int  media_clkcfg0_isp_sck_value1   : 7;  /* bit[8-14] : 时钟参数。
                                                                          [14:12]:保留
                                                                          [11:8]:ISP工作源时钟分频参数配置； */
        unsigned int  media_clkcfg0_isp_sclk_vld1    : 1;  /* bit[15]   : 时钟参数有效指示。 */
        unsigned int  media_clkcfg0_noc_lp_value0    : 7;  /* bit[16-22]: 时钟参数。
                                                                          [22:20]:保留
                                                                          [19:16]:NOC总线低功耗状态下的调频时钟分频参数配置； */
        unsigned int  media_clkcfg0_noc_lp_vld0      : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  media_clkcfg0_isp_sclk2_value3 : 7;  /* bit[24-30]: 时钟参数。
                                                                          [30:28]:保留
                                                                          [27:24]:ISP的SCLK2（MIPI）时钟分频参数配置； */
        unsigned int  media_clkcfg0_isp_sclk2_vld3   : 1;  /* bit[31]   : 时钟参数有效指示。 */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_noc_value0_START        (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_noc_value0_END          (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_noc_vld0_START          (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_noc_vld0_END            (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_isp_sck_value1_START    (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_isp_sck_value1_END      (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_isp_sclk_vld1_START     (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_isp_sclk_vld1_END       (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_noc_lp_value0_START     (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_noc_lp_value0_END       (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_noc_lp_vld0_START       (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_noc_lp_vld0_END         (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_isp_sclk2_value3_START  (24)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_isp_sclk2_value3_END    (30)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_isp_sclk2_vld3_START    (31)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_media_clkcfg0_isp_sclk2_vld3_END      (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_UNION
 结构说明  : SC_MEDIA_CLKCFG1 寄存器结构定义。地址偏移量:0xCC0，初值:0xFFFFFFFF，宽度:32
 寄存器说明: MEDIA时钟参数配置寄存器1（ADE）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_clkcfg1_value0           : 7;  /* bit[0-6]  : 时钟参数。
                                                                          保留 */
        unsigned int  media_clkcfg1_vld0             : 1;  /* bit[7]    : 时钟参数有效指示。 */
        unsigned int  media_clkcfg1_value1           : 7;  /* bit[8-14] : 时钟参数。
                                                                          保留 */
        unsigned int  media_clkcfg1_cfg_vld1         : 1;  /* bit[15]   : 时钟参数有效指示。 */
        unsigned int  media_clkcfg1_ade_core_value2  : 7;  /* bit[16-22]: 时钟参数。
                                                                          [22:19]:保留
                                                                          [18:16]:ADE的core时钟分频参数配置； */
        unsigned int  media_clkcfg1_ade_core_vld2    : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  media_clkcfg1_ade_pixel_value3 : 7;  /* bit[24-30]: 时钟参数。
                                                                          [30]:保留
                                                                          [29:24]:ADE的pixel时钟分频参数配置； */
        unsigned int  media_clkcfg1_ade_pixel_vld3   : 1;  /* bit[31]   : 时钟参数有效指示。 */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_value0_START            (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_value0_END              (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_vld0_START              (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_vld0_END                (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_value1_START            (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_value1_END              (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_cfg_vld1_START          (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_cfg_vld1_END            (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_ade_core_value2_START   (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_ade_core_value2_END     (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_ade_core_vld2_START     (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_ade_core_vld2_END       (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_ade_pixel_value3_START  (24)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_ade_pixel_value3_END    (30)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_ade_pixel_vld3_START    (31)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG1_media_clkcfg1_ade_pixel_vld3_END      (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_UNION
 结构说明  : SC_MEDIA_CLKCFG2 寄存器结构定义。地址偏移量:0xCC4，初值:0xFFFFFFFF，宽度:32
 寄存器说明: MEDIA时钟参数配置寄存器2(CODEC和GPU)
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clk_8bit_cfg8_value0            : 7;  /* bit[0-6]  : 时钟参数。保留 */
        unsigned int  clk_8bit_cfg8_vld0              : 1;  /* bit[7]    : 时钟参数有效指示。 */
        unsigned int  media_clkcfg2_g3d_value1        : 7;  /* bit[8-14] : 时钟参数。
                                                                           [14:12]:保留
                                                                           [11:8]:G3D时钟分频参数配置； */
        unsigned int  media_clkcfg2_g3d_vld1          : 1;  /* bit[15]   : 时钟参数有效指示。 */
        unsigned int  media_clkcfg2_codec_jpeg_value2 : 7;  /* bit[16-22]: 时钟参数。
                                                                           [22:20]:保留
                                                                           [19:16]:CODEC JPEG时钟分频参数配置； */
        unsigned int  media_clkcfg2_codec_jpeg_vld2   : 1;  /* bit[23-23]: 时钟参数有效指示。 */
        unsigned int  media_clkcfg2_codec_vpu_value3  : 7;  /* bit[24-30]: 时钟参数。
                                                                           [30:30]:保留
                                                                           [29:24]:CODEC VPU时钟分频参数配置； */
        unsigned int  media_clkcfg2_codec_vpu_vld3    : 1;  /* bit[31]   : 时钟参数有效指示。 */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_clk_8bit_cfg8_value0_START             (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_clk_8bit_cfg8_value0_END               (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_clk_8bit_cfg8_vld0_START               (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_clk_8bit_cfg8_vld0_END                 (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_g3d_value1_START         (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_g3d_value1_END           (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_g3d_vld1_START           (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_g3d_vld1_END             (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_codec_jpeg_value2_START  (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_codec_jpeg_value2_END    (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_codec_jpeg_vld2_START    (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_codec_jpeg_vld2_END      (23)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_codec_vpu_value3_START   (24)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_codec_vpu_value3_END     (30)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_codec_vpu_vld3_START     (31)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG2_media_clkcfg2_codec_vpu_vld3_END       (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_UNION
 结构说明  : SC_MEDIA_CLKEN 寄存器结构定义。地址偏移量:0x520，初值:0x0003FE3A，宽度:32
 寄存器说明: MEDIA子系统时钟使能寄存器，对比特写1，使能对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_clken_dsi_clk        : 1;  /* bit[0]    : dsi的时钟软门控信号 */
        unsigned int  media_clken_g3d_clk        : 1;  /* bit[1]    : G3D的core工作时钟软门控信号 */
        unsigned int  reserved_0                 : 1;  /* bit[2]    : reserved */
        unsigned int  media_clken_codec_vpu_clk  : 1;  /* bit[3]    : CODEC_VIDEO的编解码器工作时钟软门控信号 */
        unsigned int  media_clken_codec_jpeg_clk : 1;  /* bit[4]    : CODE_JPEG的编解码器工作时钟软门控信号 */
        unsigned int  media_clken_isp_clk        : 1;  /* bit[5]    : ISP的时钟软门控信号 */
        unsigned int  media_clken_ade_clk        : 1;  /* bit[6]    : ADE的时钟软门控信号 */
        unsigned int  media_clken_x2rm_hq_clk    : 1;  /* bit[7]    : 系统CRG输入的xg2ram时钟对应HARQ MEMORY SHARE时钟软门控信号 */
        unsigned int  media_clken_mmu            : 1;  /* bit[8]    : 系统CRG输入的mmu时钟软门控信号 */
        unsigned int  media_clken_csi4phy        : 1;  /* bit[9]    : 4lane csi phy配置时钟软门控信号 */
        unsigned int  media_clken_csi2phy        : 1;  /* bit[10]   : 2lane csi phy配置时钟软门控信号 */
        unsigned int  media_gating_sclk_isp      : 1;  /* bit[11]   : ISP SCLK时钟分频前时钟软门控信号 */
        unsigned int  media_gating_sclk2_isp     : 1;  /* bit[12]   : ISP SCLK2时钟分频前时钟软门控信号 */
        unsigned int  reserved_1                 : 1;  /* bit[13]   : reserved */
        unsigned int  media_gating_ade_clk       : 1;  /* bit[14]   : ade时钟分频前时钟软门控信号 */
        unsigned int  media_gating_codec_vpu     : 1;  /* bit[15]   : codec video编解码器工作时钟分频前时钟软门控信号 */
        unsigned int  media_gating_codec_jpeg    : 1;  /* bit[16]   : codec jpeg编解码器工作时钟分频前时钟软门控信号 */
        unsigned int  media_gating_sys_dsp       : 1;  /* bit[17]   : g3d工作时钟分频前对于system pll的时钟软门控信号 */
        unsigned int  reserved_2                 : 1;  /* bit[18]   : reserved */
        unsigned int  reserved_3                 : 1;  /* bit[19]   : reserved */
        unsigned int  reserved_4                 : 12; /* bit[20-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_dsi_clk_START         (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_dsi_clk_END           (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_g3d_clk_START         (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_g3d_clk_END           (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_codec_vpu_clk_START   (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_codec_vpu_clk_END     (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_codec_jpeg_clk_START  (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_codec_jpeg_clk_END    (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_isp_clk_START         (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_isp_clk_END           (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_ade_clk_START         (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_ade_clk_END           (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_x2rm_hq_clk_START     (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_x2rm_hq_clk_END       (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_mmu_START             (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_mmu_END               (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_csi4phy_START         (9)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_csi4phy_END           (9)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_csi2phy_START         (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_clken_csi2phy_END           (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_sclk_isp_START       (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_sclk_isp_END         (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_sclk2_isp_START      (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_sclk2_isp_END        (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_ade_clk_START        (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_ade_clk_END          (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_codec_vpu_START      (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_codec_vpu_END        (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_codec_jpeg_START     (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_codec_jpeg_END       (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_sys_dsp_START        (17)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_media_gating_sys_dsp_END          (17)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_UNION
 结构说明  : SC_MEDIA_CLKDIS 寄存器结构定义。地址偏移量:0x524，初值:0x000000，宽度:32
 寄存器说明: MEDIA子系统时钟禁止寄存器，对比特写1，禁止对应的时钟；写0不影响时钟使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_clken_dis_dsi_clk        : 1;  /* bit[0]    : dsi的时钟软门控信号 */
        unsigned int  media_clken_dis_g3d_clk        : 1;  /* bit[1]    : G3D的core工作时钟软门控信号 */
        unsigned int  reserved_0                     : 1;  /* bit[2]    : reserved */
        unsigned int  media_clken_dis_codec_vpu_clk  : 1;  /* bit[3]    : CODEC_VIDEO的编解码器工作时钟软门控信号 */
        unsigned int  media_clken_dis_codec_jpeg_clk : 1;  /* bit[4]    : CODE_JPEG的编解码器工作时钟软门控信号 */
        unsigned int  media_clken_dis_isp_clk        : 1;  /* bit[5]    : ISP的时钟软门控信号 */
        unsigned int  media_clken_dis_ade_clk        : 1;  /* bit[6]    : ADE的时钟软门控信号 */
        unsigned int  media_clken_dis_x2rm_hq_clk    : 1;  /* bit[7]    : 系统CRG输入的xg2ram时钟对应HARQ MEMORY SHARE时钟软门控信号 */
        unsigned int  media_clken_dis_mmu            : 1;  /* bit[8]    : 系统CRG输入的mmu时钟软门控信号 */
        unsigned int  media_clken_dis_csi4phy        : 1;  /* bit[9]    : 4lane csi phy配置时钟软门控信号 */
        unsigned int  media_clken_dis_csi2phy        : 1;  /* bit[10]   : 2lane csi phy配置时钟软门控信号 */
        unsigned int  media_gating_dis_sclk_isp      : 1;  /* bit[11]   : ISP SCLK时钟分频前时钟软门控信号 */
        unsigned int  media_gating_dis_sclk2_isp     : 1;  /* bit[12]   : ISP SCLK2时钟分频前时钟软门控信号 */
        unsigned int  reserved_1                     : 1;  /* bit[13]   : reserved */
        unsigned int  media_gating_dis_ade_clk       : 1;  /* bit[14]   : ade时钟分频前时钟软门控信号 */
        unsigned int  media_gating_dis_codec_vpu     : 1;  /* bit[15]   : codec video编解码器工作时钟分频前时钟软门控信号 */
        unsigned int  media_gating_dis_codec_jpeg    : 1;  /* bit[16]   : codec jpeg编解码器工作时钟分频前时钟软门控信号 */
        unsigned int  media_gating_dis_sys_dsp       : 1;  /* bit[17]   : g3d工作时钟分频前对于system pll的时钟软门控信号 */
        unsigned int  reserved_2                     : 1;  /* bit[18]   : reserved */
        unsigned int  reserved_3                     : 1;  /* bit[19]   : reserved */
        unsigned int  reserved_4                     : 12; /* bit[20-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_dsi_clk_START         (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_dsi_clk_END           (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_g3d_clk_START         (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_g3d_clk_END           (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_codec_vpu_clk_START   (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_codec_vpu_clk_END     (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_codec_jpeg_clk_START  (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_codec_jpeg_clk_END    (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_isp_clk_START         (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_isp_clk_END           (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_ade_clk_START         (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_ade_clk_END           (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_x2rm_hq_clk_START     (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_x2rm_hq_clk_END       (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_mmu_START             (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_mmu_END               (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_csi4phy_START         (9)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_csi4phy_END           (9)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_csi2phy_START         (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_clken_dis_csi2phy_END           (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_sclk_isp_START       (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_sclk_isp_END         (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_sclk2_isp_START      (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_sclk2_isp_END        (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_ade_clk_START        (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_ade_clk_END          (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_codec_vpu_START      (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_codec_vpu_END        (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_codec_jpeg_START     (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_codec_jpeg_END       (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_sys_dsp_START        (17)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_media_gating_dis_sys_dsp_END          (17)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_UNION
 结构说明  : SC_MEDIA_CLK_STAT 寄存器结构定义。地址偏移量:0x528，初值:0x0003F81A，宽度:32
 寄存器说明: MEDIA子系统时钟状态寄存器，由SC_MEDIA_CLKEN和SC_MEDIA_CLKDIS配置进行状态修改。
            媒体时钟控制信号需经过在MEDIA_CRG中进行同步处理。因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应时钟状态。
            每个Bit含义：
            0：时钟关闭；
            1：时钟打开。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_clken_dsi_clk_stat        : 1;  /* bit[0]    : dsi的时钟软门控信号 */
        unsigned int  media_clken_g3d_clk_stat        : 1;  /* bit[1]    : G3D的core工作时钟软门控信号 */
        unsigned int  reserved_0                      : 1;  /* bit[2]    : reserved */
        unsigned int  media_clken_codec_vpu_clk_stat  : 1;  /* bit[3]    : CODEC_VIDEO的编解码器工作时钟软门控信号 */
        unsigned int  media_clken_codec_jpeg_clk_stat : 1;  /* bit[4]    : CODE_JPEG的编解码器工作时钟软门控信号 */
        unsigned int  media_clken_isp_clk_stat        : 1;  /* bit[5]    : ISP的时钟软门控信号 */
        unsigned int  media_clken_ade_clk_stat        : 1;  /* bit[6]    : ADE的时钟软门控信号 */
        unsigned int  media_clken_x2rm_hq_clk_stat    : 1;  /* bit[7]    : 系统CRG输入的xg2ram时钟对应HARQ MEMORY SHARE时钟软门控信号 */
        unsigned int  media_clken_mmu_stat            : 1;  /* bit[8]    : 系统CRG输入的mmu时钟软门控信号 */
        unsigned int  media_clken_csi4phy_stat        : 1;  /* bit[9]    : 4lane csi phy配置时钟软门控信号 */
        unsigned int  media_clken_csi2phy_stat        : 1;  /* bit[10]   : 2lane csi phy配置时钟软门控信号 */
        unsigned int  media_gating_sclk_isp_stat      : 1;  /* bit[11]   : ISP SCLK时钟分频前时钟软门控信号 */
        unsigned int  media_gating_sclk2_isp_stat     : 1;  /* bit[12]   : ISP SCLK2时钟分频前时钟软门控信号 */
        unsigned int  reserved_1                      : 1;  /* bit[13]   : reserved */
        unsigned int  media_gating_ade_clk_stat       : 1;  /* bit[14]   : ade时钟分频前时钟软门控信号 */
        unsigned int  media_gating_codec_vpu_stat     : 1;  /* bit[15]   : codec video编解码器工作时钟分频前时钟软门控信号 */
        unsigned int  media_gating_codec_jpeg_stat    : 1;  /* bit[16]   : codec jpeg编解码器工作时钟分频前时钟软门控信号 */
        unsigned int  media_gating_sys_dsp_stat       : 1;  /* bit[17]   : g3d工作时钟分频前对于system pll的时钟软门控信号 */
        unsigned int  reserved_2                      : 1;  /* bit[18]   : reserved */
        unsigned int  reserved_3                      : 1;  /* bit[19]   : reserved */
        unsigned int  reserved_4                      : 12; /* bit[20-31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_dsi_clk_stat_START         (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_dsi_clk_stat_END           (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_g3d_clk_stat_START         (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_g3d_clk_stat_END           (1)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_codec_vpu_clk_stat_START   (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_codec_vpu_clk_stat_END     (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_codec_jpeg_clk_stat_START  (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_codec_jpeg_clk_stat_END    (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_isp_clk_stat_START         (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_isp_clk_stat_END           (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_ade_clk_stat_START         (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_ade_clk_stat_END           (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_x2rm_hq_clk_stat_START     (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_x2rm_hq_clk_stat_END       (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_mmu_stat_START             (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_mmu_stat_END               (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_csi4phy_stat_START         (9)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_csi4phy_stat_END           (9)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_csi2phy_stat_START         (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_clken_csi2phy_stat_END           (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_sclk_isp_stat_START       (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_sclk_isp_stat_END         (11)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_sclk2_isp_stat_START      (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_sclk2_isp_stat_END        (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_ade_clk_stat_START        (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_ade_clk_stat_END          (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_codec_vpu_stat_START      (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_codec_vpu_stat_END        (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_codec_jpeg_stat_START     (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_codec_jpeg_stat_END       (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_sys_dsp_stat_START        (17)
#define SOC_MEDIA_SCTRL_SC_MEDIA_CLK_STAT_media_gating_sys_dsp_stat_END          (17)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_UNION
 结构说明  : SC_MEDIA_RSTEN 寄存器结构定义。地址偏移量:0x52C，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统复位使能寄存器，用于使能复位。对比特写1，对应IP进入复位状态；写0不影响复位状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_rsten_g3d        : 1;  /* bit[0] : G3D的core和配置口软复位使能 */
        unsigned int  reserved_0             : 1;  /* bit[1] : reserved */
        unsigned int  media_rsten_codec_vpu  : 1;  /* bit[2] : CODEC VPU软复位使能 */
        unsigned int  media_rsten_codec_jpeg : 1;  /* bit[3] : CODEC JPEG软复位使能 */
        unsigned int  media_rsten_isp        : 1;  /* bit[4] : ISP的软复位使能 */
        unsigned int  media_rsten_ade        : 1;  /* bit[5] : ADE的软复位使能 */
        unsigned int  reset_med_mmu_en       : 1;  /* bit[6] : media mmu软复位使能 */
        unsigned int  reset_med_x2rm_en      : 1;  /* bit[7] : media harq memory share xg2ram1软复位使能 */
        unsigned int  reserved_1             : 1;  /* bit[8] : reserved */
        unsigned int  reserved_2             : 1;  /* bit[9] : reserved */
        unsigned int  reserved_3             : 1;  /* bit[10]: reserved */
        unsigned int  reserved_4             : 1;  /* bit[11]: reserved */
        unsigned int  reserved_5             : 1;  /* bit[12]: reserved */
        unsigned int  reserved_6             : 1;  /* bit[13]: reserved */
        unsigned int  reserved_7             : 1;  /* bit[14]: reserved */
        unsigned int  reserved_8             : 1;  /* bit[15]: reserved */
        unsigned int  reserved_9             : 1;  /* bit[16]: reserved */
        unsigned int  reserved_10            : 1;  /* bit[17]: reserved */
        unsigned int  reserved_11            : 1;  /* bit[18]: reserved */
        unsigned int  reserved_12            : 1;  /* bit[19]: reserved */
        unsigned int  reserved_13            : 1;  /* bit[20]: reserved */
        unsigned int  reserved_14            : 1;  /* bit[21]: reserved */
        unsigned int  reserved_15            : 1;  /* bit[22]: reserved */
        unsigned int  reserved_16            : 1;  /* bit[23]: reserved */
        unsigned int  reserved_17            : 1;  /* bit[24]: reserved */
        unsigned int  reserved_18            : 1;  /* bit[25]: reserved */
        unsigned int  reserved_19            : 1;  /* bit[26]: reserved */
        unsigned int  reserved_20            : 1;  /* bit[27]: reserved */
        unsigned int  reserved_21            : 1;  /* bit[28]: reserved */
        unsigned int  reserved_22            : 1;  /* bit[29]: reserved */
        unsigned int  reserved_23            : 1;  /* bit[30]: reserved */
        unsigned int  reserved_24            : 1;  /* bit[31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_g3d_START         (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_g3d_END           (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_codec_vpu_START   (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_codec_vpu_END     (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_codec_jpeg_START  (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_codec_jpeg_END    (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_isp_START         (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_isp_END           (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_ade_START         (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_media_rsten_ade_END           (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_reset_med_mmu_en_START        (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_reset_med_mmu_en_END          (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_reset_med_x2rm_en_START       (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_reset_med_x2rm_en_END         (7)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_UNION
 结构说明  : SC_MEDIA_RSTDIS 寄存器结构定义。地址偏移量:0x530，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统解复位寄存器，用于禁止复位。对比特写1，对应IP解除复位；写0不影响复位使能的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_rstdis_g3d        : 1;  /* bit[0] : G3D的core和配置口软复位禁止 */
        unsigned int  reserved_0              : 1;  /* bit[1] : reserved */
        unsigned int  media_rstdis_codec_vpu  : 1;  /* bit[2] : CODEC VPU软复位禁止 */
        unsigned int  media_rstdis_codec_jpeg : 1;  /* bit[3] : CODEC JPEG软复位禁止 */
        unsigned int  media_rstdis_isp        : 1;  /* bit[4] : ISP的软复位禁止 */
        unsigned int  media_rstdis_ade        : 1;  /* bit[5] : ADE的软复位禁止 */
        unsigned int  reset_med_mmu_dis       : 1;  /* bit[6] : media mmu软复位禁止 */
        unsigned int  reset_med_x2rm_dis      : 1;  /* bit[7] : media harq memory share xg2ram1软复位禁止 */
        unsigned int  reserved_1              : 1;  /* bit[8] : reserved */
        unsigned int  reserved_2              : 1;  /* bit[9] : reserved */
        unsigned int  reserved_3              : 1;  /* bit[10]: reserved */
        unsigned int  reserved_4              : 1;  /* bit[11]: reserved */
        unsigned int  reserved_5              : 1;  /* bit[12]: reserved */
        unsigned int  reserved_6              : 1;  /* bit[13]: reserved */
        unsigned int  reserved_7              : 1;  /* bit[14]: reserved */
        unsigned int  reserved_8              : 1;  /* bit[15]: reserved */
        unsigned int  reserved_9              : 1;  /* bit[16]: reserved */
        unsigned int  reserved_10             : 1;  /* bit[17]: reserved */
        unsigned int  reserved_11             : 1;  /* bit[18]: reserved */
        unsigned int  reserved_12             : 1;  /* bit[19]: reserved */
        unsigned int  reserved_13             : 1;  /* bit[20]: reserved */
        unsigned int  reserved_14             : 1;  /* bit[21]: reserved */
        unsigned int  reserved_15             : 1;  /* bit[22]: reserved */
        unsigned int  reserved_16             : 1;  /* bit[23]: reserved */
        unsigned int  reserved_17             : 1;  /* bit[24]: reserved */
        unsigned int  reserved_18             : 1;  /* bit[25]: reserved */
        unsigned int  reserved_19             : 1;  /* bit[26]: reserved */
        unsigned int  reserved_20             : 1;  /* bit[27]: reserved */
        unsigned int  reserved_21             : 1;  /* bit[28]: reserved */
        unsigned int  reserved_22             : 1;  /* bit[29]: reserved */
        unsigned int  reserved_23             : 1;  /* bit[30]: reserved */
        unsigned int  reserved_24             : 1;  /* bit[31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_g3d_START         (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_g3d_END           (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_codec_vpu_START   (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_codec_vpu_END     (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_codec_jpeg_START  (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_codec_jpeg_END    (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_isp_START         (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_isp_END           (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_ade_START         (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_media_rstdis_ade_END           (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_reset_med_mmu_dis_START        (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_reset_med_mmu_dis_END          (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_reset_med_x2rm_dis_START       (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_reset_med_x2rm_dis_END         (7)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_UNION
 结构说明  : SC_MEDIA_RST_STAT 寄存器结构定义。地址偏移量:0x534，初值:0x000000FD，宽度:32
 寄存器说明: MEDIA子系统复位使能状态寄存器。由SC_MEDIA_RSTEN和SC_MEDIA_RSTDIS配置进行状态修改。解复位控制信号需经过同步处理，因此，这里的寄存器值在延迟相应时钟域的几个周期后才真正反应解复位状态。
            每个Bit含义：
            0：复位不使能；
            1：复位使能。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  media_rst_stat_g3d        : 1;  /* bit[0] : G3D的core和配置口软复位使能状态 */
        unsigned int  reserved_0                : 1;  /* bit[1] : reserved */
        unsigned int  media_rst_stat_codec_vpu  : 1;  /* bit[2] : CODEC VPU软复位使能状态 */
        unsigned int  media_rst_stat_codec_jpeg : 1;  /* bit[3] : CODEC JPEG软复位使能状态 */
        unsigned int  media_rst_stat_isp        : 1;  /* bit[4] : ISP的软复位使能状态 */
        unsigned int  media_rst_stat_ade        : 1;  /* bit[5] : ADE的软复位使能状态 */
        unsigned int  reset_med_mmu_state       : 1;  /* bit[6] : media mmu软复位使能状态 */
        unsigned int  reset_med_x2rm_state      : 1;  /* bit[7] : media memory share xg2ram软复位使能状态 */
        unsigned int  reserved_1                : 1;  /* bit[8] : reserved */
        unsigned int  reserved_2                : 1;  /* bit[9] : reserved */
        unsigned int  reserved_3                : 1;  /* bit[10]: reserved */
        unsigned int  reserved_4                : 1;  /* bit[11]: reserved */
        unsigned int  reserved_5                : 1;  /* bit[12]: reserved */
        unsigned int  reserved_6                : 1;  /* bit[13]: reserved */
        unsigned int  reserved_7                : 1;  /* bit[14]: reserved */
        unsigned int  reserved_8                : 1;  /* bit[15]: reserved */
        unsigned int  reserved_9                : 1;  /* bit[16]: reserved */
        unsigned int  reserved_10               : 1;  /* bit[17]: reserved */
        unsigned int  reserved_11               : 1;  /* bit[18]: reserved */
        unsigned int  reserved_12               : 1;  /* bit[19]: reserved */
        unsigned int  reserved_13               : 1;  /* bit[20]: reserved */
        unsigned int  reserved_14               : 1;  /* bit[21]: reserved */
        unsigned int  reserved_15               : 1;  /* bit[22]: reserved */
        unsigned int  reserved_16               : 1;  /* bit[23]: reserved */
        unsigned int  reserved_17               : 1;  /* bit[24]: reserved */
        unsigned int  reserved_18               : 1;  /* bit[25]: reserved */
        unsigned int  reserved_19               : 1;  /* bit[26]: reserved */
        unsigned int  reserved_20               : 1;  /* bit[27]: reserved */
        unsigned int  reserved_21               : 1;  /* bit[28]: reserved */
        unsigned int  reserved_22               : 1;  /* bit[29]: reserved */
        unsigned int  reserved_23               : 1;  /* bit[30]: reserved */
        unsigned int  reserved_24               : 1;  /* bit[31]: reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_g3d_START         (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_g3d_END           (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_codec_vpu_START   (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_codec_vpu_END     (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_codec_jpeg_START  (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_codec_jpeg_END    (3)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_isp_START         (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_isp_END           (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_ade_START         (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_media_rst_stat_ade_END           (5)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_reset_med_mmu_state_START        (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_reset_med_mmu_state_END          (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_reset_med_x2rm_state_START       (7)
#define SOC_MEDIA_SCTRL_SC_MEDIA_RST_STAT_reset_med_x2rm_state_END         (7)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL6_UNION
 结构说明  : SC_MEDIA__SUBSYS_CTRL6 寄存器结构定义。地址偏移量:0x538，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含TEST_MUX控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  testpin_grp_sel    : 16; /* bit[0-15] : 媒体各模块内部的test group选择信号。
                                                              详细请参见\TC_CHIPSET_DOC\Hi6210V100\03. SOC\03.Media\01.HLD\MEDIA TESTMUX路径下媒体各模块testmux说明书 */
        unsigned int  testpin_module_sel : 16; /* bit[16-31]: 选择通过testmux观测的媒体模块。
                                                              0x0000:选择g3d的test group
                                                              0x0001:选择codec的test group
                                                              0x0002:选择isp的test group
                                                              0x0003:选择ade的test group
                                                              0x0004:选择mipi tx phy的test group
                                                              0x0005:选择mipi rx 4lane phy的test group
                                                              0x0006:选择mipi rx 2lane phy的test group */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL6_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL6_testpin_grp_sel_START     (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL6_testpin_grp_sel_END       (15)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL6_testpin_module_sel_START  (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL6_testpin_module_sel_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_UNION
 结构说明  : SC_MEDIA__SUBSYS_CTRL7 寄存器结构定义。地址偏移量:0x53C，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含hurry/press配置，详细请参见NOC设计说明书
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vpu_m0_press : 3;  /* bit[0-2]  : VPU MASTER0的press配置，press值影响传输通道优先级 */
        unsigned int  reserved_0   : 1;  /* bit[3]    : reserved */
        unsigned int  vpu_m0_hurry : 3;  /* bit[4-6]  : VPU MASTER0的hurry配置，hurry值影响pending transaction优先级 */
        unsigned int  reserved_1   : 1;  /* bit[7]    : reserved */
        unsigned int  vpu_m1_press : 3;  /* bit[8-10] : VPU MASTER1的press配置，press值影响传输通道优先级 */
        unsigned int  reserved_2   : 1;  /* bit[11]   : reserved */
        unsigned int  vpu_m1_hurry : 3;  /* bit[12-14]: VPU MASTER1的hurry配置，hurry值影响pending transaction优先级 */
        unsigned int  reserved_3   : 1;  /* bit[15]   : reserved */
        unsigned int  jpu_m_press  : 3;  /* bit[16-18]: JPU MASTER的press配置，press值影响传输通道优先级 */
        unsigned int  reserved_4   : 1;  /* bit[19]   : reserved */
        unsigned int  jpu_m_hurry  : 3;  /* bit[20-22]: JPU MASTER的hurry配置，hurry值影响pending transaction优先级 */
        unsigned int  reserved_5   : 1;  /* bit[23]   : reserved */
        unsigned int  isp_m_press  : 3;  /* bit[24-26]: ISP MASTER的press配置，press值影响传输通道优先级 */
        unsigned int  reserved_6   : 1;  /* bit[27]   : reserved */
        unsigned int  isp_m_hurry  : 3;  /* bit[28-30]: ISP MASTER的hurry配置，hurry值影响pending transaction优先级 */
        unsigned int  reserved_7   : 1;  /* bit[31]   : reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_vpu_m0_press_START  (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_vpu_m0_press_END    (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_vpu_m0_hurry_START  (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_vpu_m0_hurry_END    (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_vpu_m1_press_START  (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_vpu_m1_press_END    (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_vpu_m1_hurry_START  (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_vpu_m1_hurry_END    (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_jpu_m_press_START   (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_jpu_m_press_END     (18)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_jpu_m_hurry_START   (20)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_jpu_m_hurry_END     (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_isp_m_press_START   (24)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_isp_m_press_END     (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_isp_m_hurry_START   (28)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL7_isp_m_hurry_END     (30)


/*****************************************************************************
 结构名    : SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_UNION
 结构说明  : SC_MEDIA__SUBSYS_CTRL8 寄存器结构定义。地址偏移量:0x554，初值:0x00000000，宽度:32
 寄存器说明: MEDIA子系统控制寄存器，功能包含hurry/press/qos配置，详细请参见NOC设计说明书
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ade_m0_press : 3;  /* bit[0-2]  : ADE MASTER0的press配置，press值影响传输通道优先级 */
        unsigned int  reserved_0   : 1;  /* bit[3]    : reserved */
        unsigned int  ade_m0_hurry : 3;  /* bit[4-6]  : ADE MASTER0的hurry配置，hurry值影响pending transaction优先级 */
        unsigned int  reserved_1   : 1;  /* bit[7]    : reserved */
        unsigned int  ade_m1_press : 3;  /* bit[8-10] : ADE MASTER1的press配置，press值影响传输通道优先级 */
        unsigned int  reserved_2   : 1;  /* bit[11]   : reserved */
        unsigned int  ade_m1_hurry : 3;  /* bit[12-14]: ADE MASTER1的hurry配置，hurry值影响pending transaction优先级 */
        unsigned int  reserved_3   : 1;  /* bit[15]   : reserved */
        unsigned int  g3d_m_press  : 3;  /* bit[16-18]: G3D MASTER的press配置，press值影响传输通道优先级 */
        unsigned int  reserved_4   : 1;  /* bit[19]   : reserved */
        unsigned int  g3d_m_hurry  : 3;  /* bit[20-22]: G3D MASTER的hurry配置，hurry值影响pending transaction优先级 */
        unsigned int  reserved_5   : 1;  /* bit[23]   : reserved */
        unsigned int  g3d_m_wr_qos : 3;  /* bit[24-26]: G3D MASTER的写QOS配置 */
        unsigned int  reserved_6   : 1;  /* bit[27]   : reserved */
        unsigned int  g3d_m_rd_qos : 3;  /* bit[28-30]: G3D MASTER的读QOS配置 */
        unsigned int  reserved_7   : 1;  /* bit[31]   : reserved */
    } reg;
} SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_UNION;
#endif
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_ade_m0_press_START  (0)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_ade_m0_press_END    (2)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_ade_m0_hurry_START  (4)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_ade_m0_hurry_END    (6)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_ade_m1_press_START  (8)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_ade_m1_press_END    (10)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_ade_m1_hurry_START  (12)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_ade_m1_hurry_END    (14)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_g3d_m_press_START   (16)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_g3d_m_press_END     (18)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_g3d_m_hurry_START   (20)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_g3d_m_hurry_END     (22)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_g3d_m_wr_qos_START  (24)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_g3d_m_wr_qos_END    (26)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_g3d_m_rd_qos_START  (28)
#define SOC_MEDIA_SCTRL_SC_MEDIA__SUBSYS_CTRL8_g3d_m_rd_qos_END    (30)






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

#endif /* end of soc_media_sctrl_interface.h */
