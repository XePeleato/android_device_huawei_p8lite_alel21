

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_CSI_INTERFACE_H__
#define __SOC_CSI_INTERFACE_H__

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
/* 寄存器说明：Controller版本寄存器。
   位域定义UNION结构:  SOC_CSI_VERSION_UNION */
#define SOC_CSI_VERSION_ADDR(base)                    ((base) + (0x0000))

/* 寄存器说明：Data Lane数量配置寄存器。
   位域定义UNION结构:  SOC_CSI_N_LANES_UNION */
#define SOC_CSI_N_LANES_ADDR(base)                    ((base) + (0x0004))

/* 寄存器说明：PHY关闭控制寄存器。
   位域定义UNION结构:  SOC_CSI_PHY_SHUTDOWNZ_UNION */
#define SOC_CSI_PHY_SHUTDOWNZ_ADDR(base)              ((base) + (0x0008))

/* 寄存器说明：PHY复位控制寄存器。
   位域定义UNION结构:  SOC_CSI_DPHY_RSTZ_UNION */
#define SOC_CSI_DPHY_RSTZ_ADDR(base)                  ((base) + (0x000C))

/* 寄存器说明：Controller软复位控制寄存器。
   位域定义UNION结构:  SOC_CSI_CSI2_RESETN_UNION */
#define SOC_CSI_CSI2_RESETN_ADDR(base)                ((base) + (0x0010))

/* 寄存器说明：PHY状态指示寄存器。
   位域定义UNION结构:  SOC_CSI_PHY_STATE_UNION */
#define SOC_CSI_PHY_STATE_ADDR(base)                  ((base) + (0x0014))

/* 寄存器说明：行边界匹配错误Data ID寄存器1。
            注意，如果硬件没有配置Data ID，则此寄存器不存在。
   位域定义UNION结构:  SOC_CSI_DATA_IDS_1_UNION */
#define SOC_CSI_DATA_IDS_1_ADDR(base)                 ((base) + (0x0018))

/* 寄存器说明：行边界匹配错误Data ID寄存器2。
            注意，如果硬件没有配置Data ID，或者配置的Data ID的数量为4，则此寄存器不存在。
   位域定义UNION结构:  SOC_CSI_DATA_IDS_2_UNION */
#define SOC_CSI_DATA_IDS_2_ADDR(base)                 ((base) + (0x001C))

/* 寄存器说明：错误状态指示寄存器1。
   位域定义UNION结构:  SOC_CSI_ERR1_UNION */
#define SOC_CSI_ERR1_ADDR(base)                       ((base) + (0x0020))

/* 寄存器说明：错误状态指示寄存器2。
   位域定义UNION结构:  SOC_CSI_ERR2_UNION */
#define SOC_CSI_ERR2_ADDR(base)                       ((base) + (0x0024))

/* 寄存器说明：ERR1中断屏蔽寄存器。
   位域定义UNION结构:  SOC_CSI_MASK1_UNION */
#define SOC_CSI_MASK1_ADDR(base)                      ((base) + (0x0028))

/* 寄存器说明：ERR2中断屏蔽寄存器。
   位域定义UNION结构:  SOC_CSI_MASK2_UNION */
#define SOC_CSI_MASK2_ADDR(base)                      ((base) + (0x002C))

/* 寄存器说明：PHY测试接口控制寄存器0。
   位域定义UNION结构:  SOC_CSI_PHY_TST_CTRL0_UNION */
#define SOC_CSI_PHY_TST_CTRL0_ADDR(base)              ((base) + (0x0030))

/* 寄存器说明：PHY测试接口控制寄存器1。
   位域定义UNION结构:  SOC_CSI_PHY_TST_CTRL1_UNION */
#define SOC_CSI_PHY_TST_CTRL1_ADDR(base)              ((base) + (0x0034))





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
 结构名    : SOC_CSI_VERSION_UNION
 结构说明  : VERSION 寄存器结构定义。地址偏移量:0x0000，初值:0x3130322A，宽度:32
 寄存器说明: Controller版本寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  version : 32; /* bit[0-31]: 指示CSI-2 Host Controller的版本。 */
    } reg;
} SOC_CSI_VERSION_UNION;
#endif
#define SOC_CSI_VERSION_version_START  (0)
#define SOC_CSI_VERSION_version_END    (31)


/*****************************************************************************
 结构名    : SOC_CSI_N_LANES_UNION
 结构说明  : N_LANES 寄存器结构定义。地址偏移量:0x0004，初值:0x00000003，宽度:32
 寄存器说明: Data Lane数量配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  n_lanes  : 2;  /* bit[0-1] : 配置活动的Data Lane的数量。
                                                   00：1（Lane 0）；
                                                   01：2（Lane 0、1）；
                                                   10：3（Lane 0、1、2）；
                                                   11：4（Lane 0、1、2、3）。
                                                   注意，这个寄存器的复位值与硬件配置的Data Lane数量CSI_N_LANES有关，只能当PHY处于Stop状态时才能进行配置。如果CSI_N_LANES=为1，则此寄存器不存在。 */
        unsigned int  reserved : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_CSI_N_LANES_UNION;
#endif
#define SOC_CSI_N_LANES_n_lanes_START   (0)
#define SOC_CSI_N_LANES_n_lanes_END     (1)


/*****************************************************************************
 结构名    : SOC_CSI_PHY_SHUTDOWNZ_UNION
 结构说明  : PHY_SHUTDOWNZ 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: PHY关闭控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_shutdownz : 1;  /* bit[0]   : PHY关闭控制。
                                                        0：关闭PHY；
                                                        1：开启PHY。
                                                        关闭PHY将使PHY进入下电状态，PHY的模拟部分将被下电，数字逻辑将被复位。 */
        unsigned int  reserved      : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_CSI_PHY_SHUTDOWNZ_UNION;
#endif
#define SOC_CSI_PHY_SHUTDOWNZ_phy_shutdownz_START  (0)
#define SOC_CSI_PHY_SHUTDOWNZ_phy_shutdownz_END    (0)


/*****************************************************************************
 结构名    : SOC_CSI_DPHY_RSTZ_UNION
 结构说明  : DPHY_RSTZ 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: PHY复位控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dphy_rstz : 1;  /* bit[0]   : PHY复位控制。
                                                    0：复位PHY；
                                                    1：解复位PHY。
                                                    此寄存器使PHY的数字逻辑被复位。 */
        unsigned int  reserved  : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_CSI_DPHY_RSTZ_UNION;
#endif
#define SOC_CSI_DPHY_RSTZ_dphy_rstz_START  (0)
#define SOC_CSI_DPHY_RSTZ_dphy_rstz_END    (0)


/*****************************************************************************
 结构名    : SOC_CSI_CSI2_RESETN_UNION
 结构说明  : CSI2_RESETN 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: Controller软复位控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  csi2_resetn : 1;  /* bit[0]   : CSI-2 Controller软复位控制。
                                                      0：复位；
                                                      1：解复位。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_CSI_CSI2_RESETN_UNION;
#endif
#define SOC_CSI_CSI2_RESETN_csi2_resetn_START  (0)
#define SOC_CSI_CSI2_RESETN_csi2_resetn_END    (0)


/*****************************************************************************
 结构名    : SOC_CSI_PHY_STATE_UNION
 结构说明  : PHY_STATE 寄存器结构定义。地址偏移量:0x0014，初值:0x00000200，宽度:32
 寄存器说明: PHY状态指示寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_rxulpsesc_0     : 1;  /* bit[0]    : 指示Lane 0处于ULP状态。
                                                               0：非ULP状态；
                                                               1：ULP状态。 */
        unsigned int  phy_rxulpsesc_1     : 1;  /* bit[1]    : 指示Lane 1处于ULP状态。
                                                               0：非ULP状态；
                                                               1：ULP状态。 */
        unsigned int  phy_rxulpsesc_2     : 1;  /* bit[2]    : 指示Lane 2处于ULP状态。
                                                               0：非ULP状态；
                                                               1：ULP状态。 */
        unsigned int  phy_rxulpsesc_3     : 1;  /* bit[3]    : 指示Lane 3处于ULP状态。
                                                               0：非ULP状态；
                                                               1：ULP状态。 */
        unsigned int  phy_stopstatedata_0 : 1;  /* bit[4]    : 指示Lane 0处于STOP状态。
                                                               0：非STOP状态；
                                                               1：STOP状态。 */
        unsigned int  phy_stopstatedata_1 : 1;  /* bit[5]    : 指示Lane 1处于STOP状态。
                                                               0：非STOP状态；
                                                               1：STOP状态。 */
        unsigned int  phy_stopstatedata_2 : 1;  /* bit[6]    : 指示Lane 2处于STOP状态。
                                                               0：非STOP状态；
                                                               1：STOP状态。 */
        unsigned int  phy_stopstatedata_3 : 1;  /* bit[7]    : 指示Lane 3处于STOP状态。
                                                               0：非STOP状态；
                                                               1：STOP状态。 */
        unsigned int  phy_rxclkactivehs   : 1;  /* bit[8]    : 指示Clock Lane正在接收DDR时钟。
                                                               0：没有接收；
                                                               1：正在接收。 */
        unsigned int  phy_rxulpsclknot    : 1;  /* bit[9]    : 指示Clock Lane处于ULP状态。
                                                               0：ULP状态；
                                                               1：非ULP状态。 */
        unsigned int  phy_stopstateclk    : 1;  /* bit[10]   : 指示Clock Lane处于STOP状态。
                                                               0：非STOP状态；
                                                               1：STOP状态。 */
        unsigned int  bypass_2ecc_tst     : 1;  /* bit[11]   : Double ECC错误下的载荷旁路测试模式控制。 */
        unsigned int  reserved            : 20; /* bit[12-31]: 保留。 */
    } reg;
} SOC_CSI_PHY_STATE_UNION;
#endif
#define SOC_CSI_PHY_STATE_phy_rxulpsesc_0_START      (0)
#define SOC_CSI_PHY_STATE_phy_rxulpsesc_0_END        (0)
#define SOC_CSI_PHY_STATE_phy_rxulpsesc_1_START      (1)
#define SOC_CSI_PHY_STATE_phy_rxulpsesc_1_END        (1)
#define SOC_CSI_PHY_STATE_phy_rxulpsesc_2_START      (2)
#define SOC_CSI_PHY_STATE_phy_rxulpsesc_2_END        (2)
#define SOC_CSI_PHY_STATE_phy_rxulpsesc_3_START      (3)
#define SOC_CSI_PHY_STATE_phy_rxulpsesc_3_END        (3)
#define SOC_CSI_PHY_STATE_phy_stopstatedata_0_START  (4)
#define SOC_CSI_PHY_STATE_phy_stopstatedata_0_END    (4)
#define SOC_CSI_PHY_STATE_phy_stopstatedata_1_START  (5)
#define SOC_CSI_PHY_STATE_phy_stopstatedata_1_END    (5)
#define SOC_CSI_PHY_STATE_phy_stopstatedata_2_START  (6)
#define SOC_CSI_PHY_STATE_phy_stopstatedata_2_END    (6)
#define SOC_CSI_PHY_STATE_phy_stopstatedata_3_START  (7)
#define SOC_CSI_PHY_STATE_phy_stopstatedata_3_END    (7)
#define SOC_CSI_PHY_STATE_phy_rxclkactivehs_START    (8)
#define SOC_CSI_PHY_STATE_phy_rxclkactivehs_END      (8)
#define SOC_CSI_PHY_STATE_phy_rxulpsclknot_START     (9)
#define SOC_CSI_PHY_STATE_phy_rxulpsclknot_END       (9)
#define SOC_CSI_PHY_STATE_phy_stopstateclk_START     (10)
#define SOC_CSI_PHY_STATE_phy_stopstateclk_END       (10)
#define SOC_CSI_PHY_STATE_bypass_2ecc_tst_START      (11)
#define SOC_CSI_PHY_STATE_bypass_2ecc_tst_END        (11)


/*****************************************************************************
 结构名    : SOC_CSI_DATA_IDS_1_UNION
 结构说明  : DATA_IDS_1 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: 行边界匹配错误Data ID寄存器1。
            注意，如果硬件没有配置Data ID，则此寄存器不存在。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  di0_dt : 6;  /* bit[0-5]  : Data ID 0 Data Type */
        unsigned int  di0_vc : 2;  /* bit[6-7]  : Data ID 0 Virtual channel */
        unsigned int  di1_dt : 6;  /* bit[8-13] : Data ID 1 Data Type */
        unsigned int  di1_vc : 2;  /* bit[14-15]: Data ID 1 Virtual channel */
        unsigned int  di2_dt : 6;  /* bit[16-21]: Data ID 2 Data Type */
        unsigned int  di2_vc : 2;  /* bit[22-23]: Data ID 2 Virtual channel */
        unsigned int  di3_dt : 6;  /* bit[24-29]: Data ID 3 Data Type */
        unsigned int  di3_vc : 2;  /* bit[30-31]: Data ID 3 Virtual channel */
    } reg;
} SOC_CSI_DATA_IDS_1_UNION;
#endif
#define SOC_CSI_DATA_IDS_1_di0_dt_START  (0)
#define SOC_CSI_DATA_IDS_1_di0_dt_END    (5)
#define SOC_CSI_DATA_IDS_1_di0_vc_START  (6)
#define SOC_CSI_DATA_IDS_1_di0_vc_END    (7)
#define SOC_CSI_DATA_IDS_1_di1_dt_START  (8)
#define SOC_CSI_DATA_IDS_1_di1_dt_END    (13)
#define SOC_CSI_DATA_IDS_1_di1_vc_START  (14)
#define SOC_CSI_DATA_IDS_1_di1_vc_END    (15)
#define SOC_CSI_DATA_IDS_1_di2_dt_START  (16)
#define SOC_CSI_DATA_IDS_1_di2_dt_END    (21)
#define SOC_CSI_DATA_IDS_1_di2_vc_START  (22)
#define SOC_CSI_DATA_IDS_1_di2_vc_END    (23)
#define SOC_CSI_DATA_IDS_1_di3_dt_START  (24)
#define SOC_CSI_DATA_IDS_1_di3_dt_END    (29)
#define SOC_CSI_DATA_IDS_1_di3_vc_START  (30)
#define SOC_CSI_DATA_IDS_1_di3_vc_END    (31)


/*****************************************************************************
 结构名    : SOC_CSI_DATA_IDS_2_UNION
 结构说明  : DATA_IDS_2 寄存器结构定义。地址偏移量:0x001C，初值:0x00000000，宽度:32
 寄存器说明: 行边界匹配错误Data ID寄存器2。
            注意，如果硬件没有配置Data ID，或者配置的Data ID的数量为4，则此寄存器不存在。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  di4_dt : 6;  /* bit[0-5]  : Data ID 4 Data Type */
        unsigned int  di4_vc : 2;  /* bit[6-7]  : Data ID 4 Virtual channel */
        unsigned int  di5_dt : 6;  /* bit[8-13] : Data ID 5 Data Type */
        unsigned int  di5_vc : 2;  /* bit[14-15]: Data ID 5 Virtual channel */
        unsigned int  di6_dt : 6;  /* bit[16-21]: Data ID 6 Data Type */
        unsigned int  di6_vc : 2;  /* bit[22-23]: Data ID 6 Virtual channel */
        unsigned int  di7_dt : 6;  /* bit[24-29]: Data ID 7 Data Type */
        unsigned int  di7_vc : 2;  /* bit[30-31]: Data ID 7 Virtual channel */
    } reg;
} SOC_CSI_DATA_IDS_2_UNION;
#endif
#define SOC_CSI_DATA_IDS_2_di4_dt_START  (0)
#define SOC_CSI_DATA_IDS_2_di4_dt_END    (5)
#define SOC_CSI_DATA_IDS_2_di4_vc_START  (6)
#define SOC_CSI_DATA_IDS_2_di4_vc_END    (7)
#define SOC_CSI_DATA_IDS_2_di5_dt_START  (8)
#define SOC_CSI_DATA_IDS_2_di5_dt_END    (13)
#define SOC_CSI_DATA_IDS_2_di5_vc_START  (14)
#define SOC_CSI_DATA_IDS_2_di5_vc_END    (15)
#define SOC_CSI_DATA_IDS_2_di6_dt_START  (16)
#define SOC_CSI_DATA_IDS_2_di6_dt_END    (21)
#define SOC_CSI_DATA_IDS_2_di6_vc_START  (22)
#define SOC_CSI_DATA_IDS_2_di6_vc_END    (23)
#define SOC_CSI_DATA_IDS_2_di7_dt_START  (24)
#define SOC_CSI_DATA_IDS_2_di7_dt_END    (29)
#define SOC_CSI_DATA_IDS_2_di7_vc_START  (30)
#define SOC_CSI_DATA_IDS_2_di7_vc_END    (31)


/*****************************************************************************
 结构名    : SOC_CSI_ERR1_UNION
 结构说明  : ERR1 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: 错误状态指示寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_errsotsynchs_0    : 1;  /* bit[0]    : Data Lane 0 SoT错误指示（同步失败）。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_errsotsynchs_1    : 1;  /* bit[1]    : Data Lane 1 SoT错误指示（同步失败）。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_errsotsynchs_2    : 1;  /* bit[2]    : Data Lane 2 SoT错误指示（同步失败）。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_errsotsynchs_3    : 1;  /* bit[3]    : Data Lane 3 SoT错误指示（同步失败）。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_f_bndry_match_vc0 : 1;  /* bit[4]    : Virtual Channel 0 FS与FE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_f_bndry_match_vc1 : 1;  /* bit[5]    : Virtual Channel 1 FS与FE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_f_bndry_match_vc2 : 1;  /* bit[6]    : Virtual Channel 2 FS与FE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_f_bndry_match_vc3 : 1;  /* bit[7]    : Virtual Channel 3 FS与FE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_f_seq_vc0         : 1;  /* bit[8]    : Virtual Channel 0 帧序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_f_seq_vc1         : 1;  /* bit[9]    : Virtual Channel 1 帧序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_f_seq_vc2         : 1;  /* bit[10]   : Virtual Channel 2 帧序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_f_seq_vc3         : 1;  /* bit[11]   : Virtual Channel 3 帧序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_frame_data_vc0    : 1;  /* bit[12]   : Virtual Channel 0 最近一帧发生CRC错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_frame_data_vc1    : 1;  /* bit[13]   : Virtual Channel 1 最近一帧发生CRC错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_frame_data_vc2    : 1;  /* bit[14]   : Virtual Channel 2 最近一帧发生CRC错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_frame_data_vc3    : 1;  /* bit[15]   : Virtual Channel 3 最近一帧发生CRC错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_bndry_match_di0 : 1;  /* bit[16]   : Data ID 0 LS与LE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_bndry_match_di1 : 1;  /* bit[17]   : Data ID 1 LS与LE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_bndry_match_di2 : 1;  /* bit[18]   : Data ID 2 LS与LE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_bndry_match_di3 : 1;  /* bit[19]   : Data ID 3 LS与LE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_seq_di0         : 1;  /* bit[20]   : Data ID 0 行序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_seq_di1         : 1;  /* bit[21]   : Data ID 1 行序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_seq_di2         : 1;  /* bit[22]   : Data ID 2 行序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_seq_di3         : 1;  /* bit[23]   : Data ID 3 行序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  vc0_err_crc           : 1;  /* bit[24]   : Virtual Channel 0 CRC错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  vc1_err_crc           : 1;  /* bit[25]   : Virtual Channel 1 CRC错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  vc2_err_crc           : 1;  /* bit[26]   : Virtual Channel 2 CRC错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  vc3_err_crc           : 1;  /* bit[27]   : Virtual Channel 3 CRC错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_ecc_double        : 1;  /* bit[28]   : 包头ECC 2-bit错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  reserved              : 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_CSI_ERR1_UNION;
#endif
#define SOC_CSI_ERR1_phy_errsotsynchs_0_START     (0)
#define SOC_CSI_ERR1_phy_errsotsynchs_0_END       (0)
#define SOC_CSI_ERR1_phy_errsotsynchs_1_START     (1)
#define SOC_CSI_ERR1_phy_errsotsynchs_1_END       (1)
#define SOC_CSI_ERR1_phy_errsotsynchs_2_START     (2)
#define SOC_CSI_ERR1_phy_errsotsynchs_2_END       (2)
#define SOC_CSI_ERR1_phy_errsotsynchs_3_START     (3)
#define SOC_CSI_ERR1_phy_errsotsynchs_3_END       (3)
#define SOC_CSI_ERR1_err_f_bndry_match_vc0_START  (4)
#define SOC_CSI_ERR1_err_f_bndry_match_vc0_END    (4)
#define SOC_CSI_ERR1_err_f_bndry_match_vc1_START  (5)
#define SOC_CSI_ERR1_err_f_bndry_match_vc1_END    (5)
#define SOC_CSI_ERR1_err_f_bndry_match_vc2_START  (6)
#define SOC_CSI_ERR1_err_f_bndry_match_vc2_END    (6)
#define SOC_CSI_ERR1_err_f_bndry_match_vc3_START  (7)
#define SOC_CSI_ERR1_err_f_bndry_match_vc3_END    (7)
#define SOC_CSI_ERR1_err_f_seq_vc0_START          (8)
#define SOC_CSI_ERR1_err_f_seq_vc0_END            (8)
#define SOC_CSI_ERR1_err_f_seq_vc1_START          (9)
#define SOC_CSI_ERR1_err_f_seq_vc1_END            (9)
#define SOC_CSI_ERR1_err_f_seq_vc2_START          (10)
#define SOC_CSI_ERR1_err_f_seq_vc2_END            (10)
#define SOC_CSI_ERR1_err_f_seq_vc3_START          (11)
#define SOC_CSI_ERR1_err_f_seq_vc3_END            (11)
#define SOC_CSI_ERR1_err_frame_data_vc0_START     (12)
#define SOC_CSI_ERR1_err_frame_data_vc0_END       (12)
#define SOC_CSI_ERR1_err_frame_data_vc1_START     (13)
#define SOC_CSI_ERR1_err_frame_data_vc1_END       (13)
#define SOC_CSI_ERR1_err_frame_data_vc2_START     (14)
#define SOC_CSI_ERR1_err_frame_data_vc2_END       (14)
#define SOC_CSI_ERR1_err_frame_data_vc3_START     (15)
#define SOC_CSI_ERR1_err_frame_data_vc3_END       (15)
#define SOC_CSI_ERR1_err_l_bndry_match_di0_START  (16)
#define SOC_CSI_ERR1_err_l_bndry_match_di0_END    (16)
#define SOC_CSI_ERR1_err_l_bndry_match_di1_START  (17)
#define SOC_CSI_ERR1_err_l_bndry_match_di1_END    (17)
#define SOC_CSI_ERR1_err_l_bndry_match_di2_START  (18)
#define SOC_CSI_ERR1_err_l_bndry_match_di2_END    (18)
#define SOC_CSI_ERR1_err_l_bndry_match_di3_START  (19)
#define SOC_CSI_ERR1_err_l_bndry_match_di3_END    (19)
#define SOC_CSI_ERR1_err_l_seq_di0_START          (20)
#define SOC_CSI_ERR1_err_l_seq_di0_END            (20)
#define SOC_CSI_ERR1_err_l_seq_di1_START          (21)
#define SOC_CSI_ERR1_err_l_seq_di1_END            (21)
#define SOC_CSI_ERR1_err_l_seq_di2_START          (22)
#define SOC_CSI_ERR1_err_l_seq_di2_END            (22)
#define SOC_CSI_ERR1_err_l_seq_di3_START          (23)
#define SOC_CSI_ERR1_err_l_seq_di3_END            (23)
#define SOC_CSI_ERR1_vc0_err_crc_START            (24)
#define SOC_CSI_ERR1_vc0_err_crc_END              (24)
#define SOC_CSI_ERR1_vc1_err_crc_START            (25)
#define SOC_CSI_ERR1_vc1_err_crc_END              (25)
#define SOC_CSI_ERR1_vc2_err_crc_START            (26)
#define SOC_CSI_ERR1_vc2_err_crc_END              (26)
#define SOC_CSI_ERR1_vc3_err_crc_START            (27)
#define SOC_CSI_ERR1_vc3_err_crc_END              (27)
#define SOC_CSI_ERR1_err_ecc_double_START         (28)
#define SOC_CSI_ERR1_err_ecc_double_END           (28)


/*****************************************************************************
 结构名    : SOC_CSI_ERR2_UNION
 结构说明  : ERR2 寄存器结构定义。地址偏移量:0x0024，初值:0x00000000，宽度:32
 寄存器说明: 错误状态指示寄存器2。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_erresc_0          : 1;  /* bit[0]    : Data Lane 0 Escape入口错误（ULPM）指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_erresc_1          : 1;  /* bit[1]    : Data Lane 1 Escape入口错误（ULPM）指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_erresc_2          : 1;  /* bit[2]    : Data Lane 2 Escape入口错误（ULPM）指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_erresc_3          : 1;  /* bit[3]    : Data Lane 3 Escape入口错误（ULPM）指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_errsoths_0        : 1;  /* bit[4]    : Data Lane 0 SoT错误指示（同步成功）。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_errsoths_1        : 1;  /* bit[5]    : Data Lane 1 SoT错误指示（同步成功）。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_errsoths_2        : 1;  /* bit[6]    : Data Lane 2 SoT错误指示（同步成功）。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  phy_errsoths_3        : 1;  /* bit[7]    : Data Lane 3 SoT错误指示（同步成功）。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  vc0_err_ecc_corrected : 1;  /* bit[8]    : Virtual Channel 0 包头ECC 1-bit错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  vc1_err_ecc_corrected : 1;  /* bit[9]    : Virtual Channel 1 包头ECC 1-bit错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  vc2_err_ecc_corrected : 1;  /* bit[10]   : Virtual Channel 2 包头ECC 1-bit错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  vc3_err_ecc_corrected : 1;  /* bit[11]   : Virtual Channel 3 包头ECC 1-bit错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_id_vc0            : 1;  /* bit[12]   : Virtual Channel 0 未知数据类型错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_id_vc1            : 1;  /* bit[13]   : Virtual Channel 1 未知数据类型错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_id_vc2            : 1;  /* bit[14]   : Virtual Channel 2 未知数据类型错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_id_vc3            : 1;  /* bit[15]   : Virtual Channel 3 未知数据类型错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_bndry_match_di4 : 1;  /* bit[16]   : Data ID 4 LS与LE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_bndry_match_di5 : 1;  /* bit[17]   : Data ID 5 LS与LE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_bndry_match_di6 : 1;  /* bit[18]   : Data ID 6 LS与LE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_bndry_match_di7 : 1;  /* bit[19]   : Data ID 7 LS与LE匹配错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_seq_di4         : 1;  /* bit[20]   : Data ID 4 行序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_seq_di5         : 1;  /* bit[21]   : Data ID 5 行序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_seq_di6         : 1;  /* bit[22]   : Data ID 6 行序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  err_l_seq_di7         : 1;  /* bit[23]   : Data ID 7 行序列错误指示。
                                                                 0：无错误；
                                                                 1：错误。 */
        unsigned int  reserved              : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_CSI_ERR2_UNION;
#endif
#define SOC_CSI_ERR2_phy_erresc_0_START           (0)
#define SOC_CSI_ERR2_phy_erresc_0_END             (0)
#define SOC_CSI_ERR2_phy_erresc_1_START           (1)
#define SOC_CSI_ERR2_phy_erresc_1_END             (1)
#define SOC_CSI_ERR2_phy_erresc_2_START           (2)
#define SOC_CSI_ERR2_phy_erresc_2_END             (2)
#define SOC_CSI_ERR2_phy_erresc_3_START           (3)
#define SOC_CSI_ERR2_phy_erresc_3_END             (3)
#define SOC_CSI_ERR2_phy_errsoths_0_START         (4)
#define SOC_CSI_ERR2_phy_errsoths_0_END           (4)
#define SOC_CSI_ERR2_phy_errsoths_1_START         (5)
#define SOC_CSI_ERR2_phy_errsoths_1_END           (5)
#define SOC_CSI_ERR2_phy_errsoths_2_START         (6)
#define SOC_CSI_ERR2_phy_errsoths_2_END           (6)
#define SOC_CSI_ERR2_phy_errsoths_3_START         (7)
#define SOC_CSI_ERR2_phy_errsoths_3_END           (7)
#define SOC_CSI_ERR2_vc0_err_ecc_corrected_START  (8)
#define SOC_CSI_ERR2_vc0_err_ecc_corrected_END    (8)
#define SOC_CSI_ERR2_vc1_err_ecc_corrected_START  (9)
#define SOC_CSI_ERR2_vc1_err_ecc_corrected_END    (9)
#define SOC_CSI_ERR2_vc2_err_ecc_corrected_START  (10)
#define SOC_CSI_ERR2_vc2_err_ecc_corrected_END    (10)
#define SOC_CSI_ERR2_vc3_err_ecc_corrected_START  (11)
#define SOC_CSI_ERR2_vc3_err_ecc_corrected_END    (11)
#define SOC_CSI_ERR2_err_id_vc0_START             (12)
#define SOC_CSI_ERR2_err_id_vc0_END               (12)
#define SOC_CSI_ERR2_err_id_vc1_START             (13)
#define SOC_CSI_ERR2_err_id_vc1_END               (13)
#define SOC_CSI_ERR2_err_id_vc2_START             (14)
#define SOC_CSI_ERR2_err_id_vc2_END               (14)
#define SOC_CSI_ERR2_err_id_vc3_START             (15)
#define SOC_CSI_ERR2_err_id_vc3_END               (15)
#define SOC_CSI_ERR2_err_l_bndry_match_di4_START  (16)
#define SOC_CSI_ERR2_err_l_bndry_match_di4_END    (16)
#define SOC_CSI_ERR2_err_l_bndry_match_di5_START  (17)
#define SOC_CSI_ERR2_err_l_bndry_match_di5_END    (17)
#define SOC_CSI_ERR2_err_l_bndry_match_di6_START  (18)
#define SOC_CSI_ERR2_err_l_bndry_match_di6_END    (18)
#define SOC_CSI_ERR2_err_l_bndry_match_di7_START  (19)
#define SOC_CSI_ERR2_err_l_bndry_match_di7_END    (19)
#define SOC_CSI_ERR2_err_l_seq_di4_START          (20)
#define SOC_CSI_ERR2_err_l_seq_di4_END            (20)
#define SOC_CSI_ERR2_err_l_seq_di5_START          (21)
#define SOC_CSI_ERR2_err_l_seq_di5_END            (21)
#define SOC_CSI_ERR2_err_l_seq_di6_START          (22)
#define SOC_CSI_ERR2_err_l_seq_di6_END            (22)
#define SOC_CSI_ERR2_err_l_seq_di7_START          (23)
#define SOC_CSI_ERR2_err_l_seq_di7_END            (23)


/*****************************************************************************
 结构名    : SOC_CSI_MASK1_UNION
 结构说明  : MASK1 寄存器结构定义。地址偏移量:0x0028，初值:0x00000000，宽度:32
 寄存器说明: ERR1中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mask_phy_errsotsynchs_0    : 1;  /* bit[0]    : phy_errsotsynchs_0中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_errsotsynchs_1    : 1;  /* bit[1]    : phy_errsotsynchs_1中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_errsotsynchs_2    : 1;  /* bit[2]    : phy_errsotsynchs_2中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_errsotsynchs_3    : 1;  /* bit[3]    : phy_errsotsynchs_3中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_f_bndry_match_vc0 : 1;  /* bit[4]    : err_f_bndry_match_vc0中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_f_bndry_match_vc1 : 1;  /* bit[5]    : err_f_bndry_match_vc1中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_f_bndry_match_vc2 : 1;  /* bit[6]    : err_f_bndry_match_vc2中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_f_bndry_match_vc3 : 1;  /* bit[7]    : err_f_bndry_match_vc3中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_f_seq_vc0         : 1;  /* bit[8]    : err_f_seq_vc0中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_f_seq_vc1         : 1;  /* bit[9]    : err_f_seq_vc1中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_f_seq_vc2         : 1;  /* bit[10]   : err_f_seq_vc2中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_f_seq_vc3         : 1;  /* bit[11]   : err_f_seq_vc3中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_frame_data_vc0    : 1;  /* bit[12]   : err_frame_data_vc0中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_frame_data_vc1    : 1;  /* bit[13]   : err_frame_data_vc1中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_frame_data_vc2    : 1;  /* bit[14]   : err_frame_data_vc2中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_frame_data_vc3    : 1;  /* bit[15]   : err_frame_data_vc3中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_bndry_match_di0 : 1;  /* bit[16]   : err_l_bndry_match_di0中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_bndry_match_di1 : 1;  /* bit[17]   : err_l_bndry_match_di1中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_bndry_match_di2 : 1;  /* bit[18]   : err_l_bndry_match_di2中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_bndry_match_di3 : 1;  /* bit[19]   : err_l_bndry_match_di3中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_seq_di0         : 1;  /* bit[20]   : err_l_seq_di0中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_seq_di1         : 1;  /* bit[21]   : err_l_seq_di1中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_seq_di2         : 1;  /* bit[22]   : err_l_seq_di2中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_seq_di3         : 1;  /* bit[23]   : err_l_seq_di3中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_vc0_err_crc           : 1;  /* bit[24]   : vc0_err_crc中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_vc1_err_crc           : 1;  /* bit[25]   : vc1_err_crc中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_vc2_err_crc           : 1;  /* bit[26]   : vc2_err_crc中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_vc3_err_crc           : 1;  /* bit[27]   : vc3_err_crc中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_ecc_double        : 1;  /* bit[28]   : err_ecc_double中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  reserved                   : 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_CSI_MASK1_UNION;
#endif
#define SOC_CSI_MASK1_mask_phy_errsotsynchs_0_START     (0)
#define SOC_CSI_MASK1_mask_phy_errsotsynchs_0_END       (0)
#define SOC_CSI_MASK1_mask_phy_errsotsynchs_1_START     (1)
#define SOC_CSI_MASK1_mask_phy_errsotsynchs_1_END       (1)
#define SOC_CSI_MASK1_mask_phy_errsotsynchs_2_START     (2)
#define SOC_CSI_MASK1_mask_phy_errsotsynchs_2_END       (2)
#define SOC_CSI_MASK1_mask_phy_errsotsynchs_3_START     (3)
#define SOC_CSI_MASK1_mask_phy_errsotsynchs_3_END       (3)
#define SOC_CSI_MASK1_mask_err_f_bndry_match_vc0_START  (4)
#define SOC_CSI_MASK1_mask_err_f_bndry_match_vc0_END    (4)
#define SOC_CSI_MASK1_mask_err_f_bndry_match_vc1_START  (5)
#define SOC_CSI_MASK1_mask_err_f_bndry_match_vc1_END    (5)
#define SOC_CSI_MASK1_mask_err_f_bndry_match_vc2_START  (6)
#define SOC_CSI_MASK1_mask_err_f_bndry_match_vc2_END    (6)
#define SOC_CSI_MASK1_mask_err_f_bndry_match_vc3_START  (7)
#define SOC_CSI_MASK1_mask_err_f_bndry_match_vc3_END    (7)
#define SOC_CSI_MASK1_mask_err_f_seq_vc0_START          (8)
#define SOC_CSI_MASK1_mask_err_f_seq_vc0_END            (8)
#define SOC_CSI_MASK1_mask_err_f_seq_vc1_START          (9)
#define SOC_CSI_MASK1_mask_err_f_seq_vc1_END            (9)
#define SOC_CSI_MASK1_mask_err_f_seq_vc2_START          (10)
#define SOC_CSI_MASK1_mask_err_f_seq_vc2_END            (10)
#define SOC_CSI_MASK1_mask_err_f_seq_vc3_START          (11)
#define SOC_CSI_MASK1_mask_err_f_seq_vc3_END            (11)
#define SOC_CSI_MASK1_mask_err_frame_data_vc0_START     (12)
#define SOC_CSI_MASK1_mask_err_frame_data_vc0_END       (12)
#define SOC_CSI_MASK1_mask_err_frame_data_vc1_START     (13)
#define SOC_CSI_MASK1_mask_err_frame_data_vc1_END       (13)
#define SOC_CSI_MASK1_mask_err_frame_data_vc2_START     (14)
#define SOC_CSI_MASK1_mask_err_frame_data_vc2_END       (14)
#define SOC_CSI_MASK1_mask_err_frame_data_vc3_START     (15)
#define SOC_CSI_MASK1_mask_err_frame_data_vc3_END       (15)
#define SOC_CSI_MASK1_mask_err_l_bndry_match_di0_START  (16)
#define SOC_CSI_MASK1_mask_err_l_bndry_match_di0_END    (16)
#define SOC_CSI_MASK1_mask_err_l_bndry_match_di1_START  (17)
#define SOC_CSI_MASK1_mask_err_l_bndry_match_di1_END    (17)
#define SOC_CSI_MASK1_mask_err_l_bndry_match_di2_START  (18)
#define SOC_CSI_MASK1_mask_err_l_bndry_match_di2_END    (18)
#define SOC_CSI_MASK1_mask_err_l_bndry_match_di3_START  (19)
#define SOC_CSI_MASK1_mask_err_l_bndry_match_di3_END    (19)
#define SOC_CSI_MASK1_mask_err_l_seq_di0_START          (20)
#define SOC_CSI_MASK1_mask_err_l_seq_di0_END            (20)
#define SOC_CSI_MASK1_mask_err_l_seq_di1_START          (21)
#define SOC_CSI_MASK1_mask_err_l_seq_di1_END            (21)
#define SOC_CSI_MASK1_mask_err_l_seq_di2_START          (22)
#define SOC_CSI_MASK1_mask_err_l_seq_di2_END            (22)
#define SOC_CSI_MASK1_mask_err_l_seq_di3_START          (23)
#define SOC_CSI_MASK1_mask_err_l_seq_di3_END            (23)
#define SOC_CSI_MASK1_mask_vc0_err_crc_START            (24)
#define SOC_CSI_MASK1_mask_vc0_err_crc_END              (24)
#define SOC_CSI_MASK1_mask_vc1_err_crc_START            (25)
#define SOC_CSI_MASK1_mask_vc1_err_crc_END              (25)
#define SOC_CSI_MASK1_mask_vc2_err_crc_START            (26)
#define SOC_CSI_MASK1_mask_vc2_err_crc_END              (26)
#define SOC_CSI_MASK1_mask_vc3_err_crc_START            (27)
#define SOC_CSI_MASK1_mask_vc3_err_crc_END              (27)
#define SOC_CSI_MASK1_mask_err_ecc_double_START         (28)
#define SOC_CSI_MASK1_mask_err_ecc_double_END           (28)


/*****************************************************************************
 结构名    : SOC_CSI_MASK2_UNION
 结构说明  : MASK2 寄存器结构定义。地址偏移量:0x002C，初值:0x00000000，宽度:32
 寄存器说明: ERR2中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mask_phy_erresc_0          : 1;  /* bit[0]    : phy_erresc_0中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_erresc_1          : 1;  /* bit[1]    : phy_erresc_1中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_erresc_2          : 1;  /* bit[2]    : phy_erresc_2中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_erresc_3          : 1;  /* bit[3]    : phy_erresc_3中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_errsoths_0        : 1;  /* bit[4]    : phy_errsoths_0中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_errsoths_1        : 1;  /* bit[5]    : phy_errsoths_1中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_errsoths_2        : 1;  /* bit[6]    : phy_errsoths_2中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_phy_errsoths_3        : 1;  /* bit[7]    : phy_errsoths_3中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_vc0_err_ecc_corrected : 1;  /* bit[8]    : vc0_err_ecc_corrected中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_vc1_err_ecc_corrected : 1;  /* bit[9]    : vc1_err_ecc_corrected中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_vc2_err_ecc_corrected : 1;  /* bit[10]   : vc2_err_ecc_corrected中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_vc3_err_ecc_corrected : 1;  /* bit[11]   : vc3_err_ecc_corrected中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_id_vc0            : 1;  /* bit[12]   : err_id_vc0中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_id_vc1            : 1;  /* bit[13]   : err_id_vc1中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_id_vc2            : 1;  /* bit[14]   : err_id_vc2中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_id_vc3            : 1;  /* bit[15]   : err_id_vc3中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_bndry_match_di4 : 1;  /* bit[16]   : err_l_bndry_match_di4中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_bndry_match_di5 : 1;  /* bit[17]   : err_l_bndry_match_di5中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_bndry_match_di6 : 1;  /* bit[18]   : err_l_bndry_match_di6中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_bndry_match_di7 : 1;  /* bit[19]   : err_l_bndry_match_di7中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_seq_di4         : 1;  /* bit[20]   : err_l_seq_di4中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_seq_di5         : 1;  /* bit[21]   : err_l_seq_di5中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_seq_di6         : 1;  /* bit[22]   : err_l_seq_di6中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  mask_err_l_seq_di7         : 1;  /* bit[23]   : err_l_seq_di7中断屏蔽。
                                                                      0：不屏蔽；
                                                                      1：屏蔽。 */
        unsigned int  reserved                   : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_CSI_MASK2_UNION;
#endif
#define SOC_CSI_MASK2_mask_phy_erresc_0_START           (0)
#define SOC_CSI_MASK2_mask_phy_erresc_0_END             (0)
#define SOC_CSI_MASK2_mask_phy_erresc_1_START           (1)
#define SOC_CSI_MASK2_mask_phy_erresc_1_END             (1)
#define SOC_CSI_MASK2_mask_phy_erresc_2_START           (2)
#define SOC_CSI_MASK2_mask_phy_erresc_2_END             (2)
#define SOC_CSI_MASK2_mask_phy_erresc_3_START           (3)
#define SOC_CSI_MASK2_mask_phy_erresc_3_END             (3)
#define SOC_CSI_MASK2_mask_phy_errsoths_0_START         (4)
#define SOC_CSI_MASK2_mask_phy_errsoths_0_END           (4)
#define SOC_CSI_MASK2_mask_phy_errsoths_1_START         (5)
#define SOC_CSI_MASK2_mask_phy_errsoths_1_END           (5)
#define SOC_CSI_MASK2_mask_phy_errsoths_2_START         (6)
#define SOC_CSI_MASK2_mask_phy_errsoths_2_END           (6)
#define SOC_CSI_MASK2_mask_phy_errsoths_3_START         (7)
#define SOC_CSI_MASK2_mask_phy_errsoths_3_END           (7)
#define SOC_CSI_MASK2_mask_vc0_err_ecc_corrected_START  (8)
#define SOC_CSI_MASK2_mask_vc0_err_ecc_corrected_END    (8)
#define SOC_CSI_MASK2_mask_vc1_err_ecc_corrected_START  (9)
#define SOC_CSI_MASK2_mask_vc1_err_ecc_corrected_END    (9)
#define SOC_CSI_MASK2_mask_vc2_err_ecc_corrected_START  (10)
#define SOC_CSI_MASK2_mask_vc2_err_ecc_corrected_END    (10)
#define SOC_CSI_MASK2_mask_vc3_err_ecc_corrected_START  (11)
#define SOC_CSI_MASK2_mask_vc3_err_ecc_corrected_END    (11)
#define SOC_CSI_MASK2_mask_err_id_vc0_START             (12)
#define SOC_CSI_MASK2_mask_err_id_vc0_END               (12)
#define SOC_CSI_MASK2_mask_err_id_vc1_START             (13)
#define SOC_CSI_MASK2_mask_err_id_vc1_END               (13)
#define SOC_CSI_MASK2_mask_err_id_vc2_START             (14)
#define SOC_CSI_MASK2_mask_err_id_vc2_END               (14)
#define SOC_CSI_MASK2_mask_err_id_vc3_START             (15)
#define SOC_CSI_MASK2_mask_err_id_vc3_END               (15)
#define SOC_CSI_MASK2_mask_err_l_bndry_match_di4_START  (16)
#define SOC_CSI_MASK2_mask_err_l_bndry_match_di4_END    (16)
#define SOC_CSI_MASK2_mask_err_l_bndry_match_di5_START  (17)
#define SOC_CSI_MASK2_mask_err_l_bndry_match_di5_END    (17)
#define SOC_CSI_MASK2_mask_err_l_bndry_match_di6_START  (18)
#define SOC_CSI_MASK2_mask_err_l_bndry_match_di6_END    (18)
#define SOC_CSI_MASK2_mask_err_l_bndry_match_di7_START  (19)
#define SOC_CSI_MASK2_mask_err_l_bndry_match_di7_END    (19)
#define SOC_CSI_MASK2_mask_err_l_seq_di4_START          (20)
#define SOC_CSI_MASK2_mask_err_l_seq_di4_END            (20)
#define SOC_CSI_MASK2_mask_err_l_seq_di5_START          (21)
#define SOC_CSI_MASK2_mask_err_l_seq_di5_END            (21)
#define SOC_CSI_MASK2_mask_err_l_seq_di6_START          (22)
#define SOC_CSI_MASK2_mask_err_l_seq_di6_END            (22)
#define SOC_CSI_MASK2_mask_err_l_seq_di7_START          (23)
#define SOC_CSI_MASK2_mask_err_l_seq_di7_END            (23)


/*****************************************************************************
 结构名    : SOC_CSI_PHY_TST_CTRL0_UNION
 结构说明  : PHY_TST_CTRL0 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: PHY测试接口控制寄存器0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_testclr : 1;  /* bit[0]   : PHY测试接口复位。
                                                      0：解复位；
                                                      1：复位。 */
        unsigned int  phy_testclk : 1;  /* bit[1]   : PHY测试接口时钟，与phy_testen共同进行操作选择。 */
        unsigned int  reserved    : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_CSI_PHY_TST_CTRL0_UNION;
#endif
#define SOC_CSI_PHY_TST_CTRL0_phy_testclr_START  (0)
#define SOC_CSI_PHY_TST_CTRL0_phy_testclr_END    (0)
#define SOC_CSI_PHY_TST_CTRL0_phy_testclk_START  (1)
#define SOC_CSI_PHY_TST_CTRL0_phy_testclk_END    (1)


/*****************************************************************************
 结构名    : SOC_CSI_PHY_TST_CTRL1_UNION
 结构说明  : PHY_TST_CTRL1 寄存器结构定义。地址偏移量:0x0034，初值:0x00000000，宽度:32
 寄存器说明: PHY测试接口控制寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_testdin  : 8;  /* bit[0-7]  : PHY测试接口8-bit数据输入。 */
        unsigned int  phy_testdout : 8;  /* bit[8-15] : PHY测试接口8-bit数据输出。 */
        unsigned int  phy_testen   : 1;  /* bit[16]   : PHY测试接口操作模式选择。
                                                        0：phy_testclk上升沿写数据；
                                                        1：phy_testclk下降沿写地址。 */
        unsigned int  reserved     : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_CSI_PHY_TST_CTRL1_UNION;
#endif
#define SOC_CSI_PHY_TST_CTRL1_phy_testdin_START   (0)
#define SOC_CSI_PHY_TST_CTRL1_phy_testdin_END     (7)
#define SOC_CSI_PHY_TST_CTRL1_phy_testdout_START  (8)
#define SOC_CSI_PHY_TST_CTRL1_phy_testdout_END    (15)
#define SOC_CSI_PHY_TST_CTRL1_phy_testen_START    (16)
#define SOC_CSI_PHY_TST_CTRL1_phy_testen_END      (16)






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

#endif /* end of soc_csi_interface.h */
