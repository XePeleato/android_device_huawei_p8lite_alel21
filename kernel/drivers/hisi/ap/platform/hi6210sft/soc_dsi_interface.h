

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_DSI_INTERFACE_H__
#define __SOC_DSI_INTERFACE_H__

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
/* 寄存器说明：版本指示寄存器。
   位域定义UNION结构:  SOC_DSI_VERSION_UNION */
#define SOC_DSI_VERSION_ADDR(base)                    ((base) + (0x0000))

/* 寄存器说明：复位控制寄存器。
   位域定义UNION结构:  SOC_DSI_PWR_UP_UNION */
#define SOC_DSI_PWR_UP_ADDR(base)                     ((base) + (0x0004))

/* 寄存器说明：内部时钟分频控制寄存器。
   位域定义UNION结构:  SOC_DSI_CLKMGR_CFG_UNION */
#define SOC_DSI_CLKMGR_CFG_ADDR(base)                 ((base) + (0x0008))

/* 寄存器说明：DPI Virtual Channel ID配置寄存器。
   位域定义UNION结构:  SOC_DSI_DPI_VCID_UNION */
#define SOC_DSI_DPI_VCID_ADDR(base)                   ((base) + (0x000C))

/* 寄存器说明：DPI颜色编码配置寄存器。
   位域定义UNION结构:  SOC_DSI_DPI_COLOR_CODING_UNION */
#define SOC_DSI_DPI_COLOR_CODING_ADDR(base)           ((base) + (0x0010))

/* 寄存器说明：DPI接口信号极性配置寄存器。
   位域定义UNION结构:  SOC_DSI_DPI_CFG_POL_UNION */
#define SOC_DSI_DPI_CFG_POL_ADDR(base)                ((base) + (0x0014))

/* 寄存器说明：DPI LP Command传输时间配置寄存器。
   位域定义UNION结构:  SOC_DSI_DPI_LP_CMD_TIM_UNION */
#define SOC_DSI_DPI_LP_CMD_TIM_ADDR(base)             ((base) + (0x0018))

/* 寄存器说明：DSI数据包配置寄存器。
   位域定义UNION结构:  SOC_DSI_PCKHDL_CFG_UNION */
#define SOC_DSI_PCKHDL_CFG_ADDR(base)                 ((base) + (0x002C))

/* 寄存器说明：Generic接口 Virtual Channel ID配置寄存器。
   位域定义UNION结构:  SOC_DSI_GEN_VCID_UNION */
#define SOC_DSI_GEN_VCID_ADDR(base)                   ((base) + (0x0030))

/* 寄存器说明：DSI工作模式配置寄存器。
   位域定义UNION结构:  SOC_DSI_MODE_CFG_UNION */
#define SOC_DSI_MODE_CFG_ADDR(base)                   ((base) + (0x0034))

/* 寄存器说明：Video Mode配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_MODE_CFG_UNION */
#define SOC_DSI_VID_MODE_CFG_ADDR(base)               ((base) + (0x0038))

/* 寄存器说明：Video包尺寸配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_PKT_SIZE_UNION */
#define SOC_DSI_VID_PKT_SIZE_ADDR(base)               ((base) + (0x003C))

/* 寄存器说明：Chunk数量配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_NUM_CHUNKS_UNION */
#define SOC_DSI_VID_NUM_CHUNKS_ADDR(base)             ((base) + (0x0040))

/* 寄存器说明：Null包尺寸配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_NULL_SIZE_UNION */
#define SOC_DSI_VID_NULL_SIZE_ADDR(base)              ((base) + (0x0044))

/* 寄存器说明：HSA时间配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_HSA_TIME_UNION */
#define SOC_DSI_VID_HSA_TIME_ADDR(base)               ((base) + (0x0048))

/* 寄存器说明：HBP时间配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_HBP_TIME_UNION */
#define SOC_DSI_VID_HBP_TIME_ADDR(base)               ((base) + (0x004C))

/* 寄存器说明：Video行时间配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_HLINE_TIME_UNION */
#define SOC_DSI_VID_HLINE_TIME_ADDR(base)             ((base) + (0x0050))

/* 寄存器说明：VSA时间配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_VSA_LINES_UNION */
#define SOC_DSI_VID_VSA_LINES_ADDR(base)              ((base) + (0x0054))

/* 寄存器说明：VBP时间配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_VBP_LINES_UNION */
#define SOC_DSI_VID_VBP_LINES_ADDR(base)              ((base) + (0x0058))

/* 寄存器说明：VFP时间配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_VFP_LINES_UNION */
#define SOC_DSI_VID_VFP_LINES_ADDR(base)              ((base) + (0x005C))

/* 寄存器说明：VACT时间配置寄存器。
   位域定义UNION结构:  SOC_DSI_VID_VACTIVE_LINES_UNION */
#define SOC_DSI_VID_VACTIVE_LINES_ADDR(base)          ((base) + (0x0060))

/* 寄存器说明：eDPI包尺寸配置寄存器。
   位域定义UNION结构:  SOC_DSI_EDPI_CMD_SIZE_UNION */
#define SOC_DSI_EDPI_CMD_SIZE_ADDR(base)              ((base) + (0x0064))

/* 寄存器说明：Command Mode配置寄存器。
   位域定义UNION结构:  SOC_DSI_CMD_MODE_CFG_UNION */
#define SOC_DSI_CMD_MODE_CFG_ADDR(base)               ((base) + (0x0068))

/* 寄存器说明：Generic包头配置寄存器。
   位域定义UNION结构:  SOC_DSI_GEN_HDR_UNION */
#define SOC_DSI_GEN_HDR_ADDR(base)                    ((base) + (0x006C))

/* 寄存器说明：Generic载荷数据寄存器。
   位域定义UNION结构:  SOC_DSI_GEN_PLD_DATA_UNION */
#define SOC_DSI_GEN_PLD_DATA_ADDR(base)               ((base) + (0x0070))

/* 寄存器说明：Command Mode包状态指示寄存器。
   位域定义UNION结构:  SOC_DSI_CMD_PKT_STATUS_UNION */
#define SOC_DSI_CMD_PKT_STATUS_ADDR(base)             ((base) + (0x0074))

/* 寄存器说明：超时计数器配置寄存器。
   位域定义UNION结构:  SOC_DSI_TO_CNT_CFG_UNION */
#define SOC_DSI_TO_CNT_CFG_ADDR(base)                 ((base) + (0x0078))

/* 寄存器说明：HS读操作外设响应超时配置寄存器。
   位域定义UNION结构:  SOC_DSI_HS_RD_TO_CNT_UNION */
#define SOC_DSI_HS_RD_TO_CNT_ADDR(base)               ((base) + (0x007C))

/* 寄存器说明：LP读操作外设响应超时配置寄存器。
   位域定义UNION结构:  SOC_DSI_LP_RD_TO_CNT_UNION */
#define SOC_DSI_LP_RD_TO_CNT_ADDR(base)               ((base) + (0x0080))

/* 寄存器说明：HS写操作外设响应超时配置寄存器。
   位域定义UNION结构:  SOC_DSI_HS_WR_TO_CNT_UNION */
#define SOC_DSI_HS_WR_TO_CNT_ADDR(base)               ((base) + (0x0084))

/* 寄存器说明：LP写操作外设响应超时配置寄存器。
   位域定义UNION结构:  SOC_DSI_LP_WR_TO_CNT_UNION */
#define SOC_DSI_LP_WR_TO_CNT_ADDR(base)               ((base) + (0x0088))

/* 寄存器说明：BTA操作外设响应超时配置寄存器。
   位域定义UNION结构:  SOC_DSI_BTA_TO_CNT_UNION */
#define SOC_DSI_BTA_TO_CNT_ADDR(base)                 ((base) + (0x008C))

/* 寄存器说明：Video Mode 3D模式配置寄存器。
   位域定义UNION结构:  SOC_DSI_SDF_3D_UNION */
#define SOC_DSI_SDF_3D_ADDR(base)                     ((base) + (0x0090))

/* 寄存器说明：Clock Lane配置寄存器。
   位域定义UNION结构:  SOC_DSI_LPCLK_CTRL_UNION */
#define SOC_DSI_LPCLK_CTRL_ADDR(base)                 ((base) + (0x0094))

/* 寄存器说明：Clock Lane时序配置寄存器。
   位域定义UNION结构:  SOC_DSI_PHY_TMR_LPCLK_CFG_UNION */
#define SOC_DSI_PHY_TMR_LPCLK_CFG_ADDR(base)          ((base) + (0x0098))

/* 寄存器说明：Data Lane时序配置寄存器。
   位域定义UNION结构:  SOC_DSI_PHY_TMR_CFG_UNION */
#define SOC_DSI_PHY_TMR_CFG_ADDR(base)                ((base) + (0x009C))

/* 寄存器说明：DPHY复位和PLL控制寄存器。
   位域定义UNION结构:  SOC_DSI_PHY_RSTZ_UNION */
#define SOC_DSI_PHY_RSTZ_ADDR(base)                   ((base) + (0x00A0))

/* 寄存器说明：DPHY接口配置寄存器。
   位域定义UNION结构:  SOC_DSI_PHY_IF_CFG_UNION */
#define SOC_DSI_PHY_IF_CFG_ADDR(base)                 ((base) + (0x00A4))

/* 寄存器说明：DPHY ULPS模式控制寄存器。
   位域定义UNION结构:  SOC_DSI_PHY_ULPS_CTRL_UNION */
#define SOC_DSI_PHY_ULPS_CTRL_ADDR(base)              ((base) + (0x00A8))

/* 寄存器说明：DPHY TRIGGERS配置寄存器。
   位域定义UNION结构:  SOC_DSI_PHY_TX_TRIGGERS_UNION */
#define SOC_DSI_PHY_TX_TRIGGERS_ADDR(base)            ((base) + (0x00AC))

/* 寄存器说明：DPHY状态指示寄存器。
   位域定义UNION结构:  SOC_DSI_PHY_STATUS_UNION */
#define SOC_DSI_PHY_STATUS_ADDR(base)                 ((base) + (0x00B0))

/* 寄存器说明：DPHY配置接口控制寄存器0。
   位域定义UNION结构:  SOC_DSI_PHY_TST_CTRL0_UNION */
#define SOC_DSI_PHY_TST_CTRL0_ADDR(base)              ((base) + (0x00B4))

/* 寄存器说明：DPHY配置接口控制寄存器1。
   位域定义UNION结构:  SOC_DSI_PHY_TST_CTRL1_UNION */
#define SOC_DSI_PHY_TST_CTRL1_ADDR(base)              ((base) + (0x00B8))

/* 寄存器说明：中断状态寄存器0。
   位域定义UNION结构:  SOC_DSI_INT_ST0_UNION */
#define SOC_DSI_INT_ST0_ADDR(base)                    ((base) + (0x00BC))

/* 寄存器说明：中断状态寄存器1。
   位域定义UNION结构:  SOC_DSI_INT_ST1_UNION */
#define SOC_DSI_INT_ST1_ADDR(base)                    ((base) + (0x00C0))

/* 寄存器说明：中断屏蔽寄存器0。
   位域定义UNION结构:  SOC_DSI_INT_MSK0_UNION */
#define SOC_DSI_INT_MSK0_ADDR(base)                   ((base) + (0x00C4))

/* 寄存器说明：中断屏蔽寄存器1。
   位域定义UNION结构:  SOC_DSI_INT_MSK1_UNION */
#define SOC_DSI_INT_MSK1_ADDR(base)                   ((base) + (0x00C8))





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
 结构名    : SOC_DSI_VERSION_UNION
 结构说明  : VERSION 寄存器结构定义。地址偏移量:0x0000，初值:0x3132302A，宽度:32
 寄存器说明: 版本指示寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  version : 32; /* bit[0-31]: 指示DSI Controller的版本。 */
    } reg;
} SOC_DSI_VERSION_UNION;
#endif
#define SOC_DSI_VERSION_version_START  (0)
#define SOC_DSI_VERSION_version_END    (31)


/*****************************************************************************
 结构名    : SOC_DSI_PWR_UP_UNION
 结构说明  : PWR_UP 寄存器结构定义。地址偏移量:0x0004，初值:0x00000000，宽度:32
 寄存器说明: 复位控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  shutdownz : 1;  /* bit[0]   : DSI Controller复位控制。
                                                    0：复位；
                                                    1：解复位。 */
        unsigned int  reserved  : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_DSI_PWR_UP_UNION;
#endif
#define SOC_DSI_PWR_UP_shutdownz_START  (0)
#define SOC_DSI_PWR_UP_shutdownz_END    (0)


/*****************************************************************************
 结构名    : SOC_DSI_CLKMGR_CFG_UNION
 结构说明  : CLKMGR_CFG 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: 内部时钟分频控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx_esc_clk_division : 8;  /* bit[0-7]  : TX Escape时钟分频比，用于对DPHY产生的lanebyteclk时钟进行分频，产生低速TX_ESC时钟。
                                                               注意，配置为0或1时将停止产生TX_ESC时钟。 */
        unsigned int  to_clk_division     : 8;  /* bit[8-15] : Timeout时钟分频比，用于对lanebyteclk时钟进行分频，产生的时钟将作为Timeout计数器的时钟源，用于检测HS to LP（或LP to HS）模式转换超时。 */
        unsigned int  reserved            : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_DSI_CLKMGR_CFG_UNION;
#endif
#define SOC_DSI_CLKMGR_CFG_tx_esc_clk_division_START  (0)
#define SOC_DSI_CLKMGR_CFG_tx_esc_clk_division_END    (7)
#define SOC_DSI_CLKMGR_CFG_to_clk_division_START      (8)
#define SOC_DSI_CLKMGR_CFG_to_clk_division_END        (15)


/*****************************************************************************
 结构名    : SOC_DSI_DPI_VCID_UNION
 结构说明  : DPI_VCID 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: DPI Virtual Channel ID配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dpi_vcid : 2;  /* bit[0-1] : Video Mode下DPI数据包的Virtual Channel ID号。 */
        unsigned int  reserved : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_DSI_DPI_VCID_UNION;
#endif
#define SOC_DSI_DPI_VCID_dpi_vcid_START  (0)
#define SOC_DSI_DPI_VCID_dpi_vcid_END    (1)


/*****************************************************************************
 结构名    : SOC_DSI_DPI_COLOR_CODING_UNION
 结构说明  : DPI_COLOR_CODING 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: DPI颜色编码配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dpi_color_coding : 4;  /* bit[0-3] : DPI颜色编码。
                                                           0000：16-bit configuration 1；
                                                           0001：16-bit configuration 2；
                                                           0010：16-bit configuration 3；
                                                           0011：18-bit configuration 1；
                                                           0100：18-bit configuration 2；
                                                           0101：24-bit；
                                                           0110：20-bit YCbCr 4:2:2 loosely packed；
                                                           0111：24-bit YCbCr 4:2:2；
                                                           1000：16-bit YCbCr 4:2:2；
                                                           1001：30-bit；
                                                           1010：36-bit；
                                                           1011~1111：12-bit YCbCr 4:2:0。 */
        unsigned int  reserved_0       : 4;  /* bit[4-7] : 保留。 */
        unsigned int  loosely18_en     : 1;  /* bit[8]   : 18-bit loosely packet使能。
                                                           0：不使能；
                                                           1：使能。 */
        unsigned int  reserved_1       : 23; /* bit[9-31]: 保留。 */
    } reg;
} SOC_DSI_DPI_COLOR_CODING_UNION;
#endif
#define SOC_DSI_DPI_COLOR_CODING_dpi_color_coding_START  (0)
#define SOC_DSI_DPI_COLOR_CODING_dpi_color_coding_END    (3)
#define SOC_DSI_DPI_COLOR_CODING_loosely18_en_START      (8)
#define SOC_DSI_DPI_COLOR_CODING_loosely18_en_END        (8)


/*****************************************************************************
 结构名    : SOC_DSI_DPI_CFG_POL_UNION
 结构说明  : DPI_CFG_POL 寄存器结构定义。地址偏移量:0x0014，初值:0x00000000，宽度:32
 寄存器说明: DPI接口信号极性配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dataen_active_low : 1;  /* bit[0]   : dpidaten信号有效电平。
                                                            0：高有效；
                                                            1：低有效。 */
        unsigned int  vsync_active_low  : 1;  /* bit[1]   : dpivsync信号有效电平。
                                                            0：高有效；
                                                            1：低有效。 */
        unsigned int  hsync_active_low  : 1;  /* bit[2]   : dpihsync信号有效电平。
                                                            0：高有效；
                                                            1：低有效。 */
        unsigned int  shutd_active_low  : 1;  /* bit[3]   : dpishutdn信号有效电平。
                                                            0：高有效；
                                                            1：低有效。 */
        unsigned int  colorm_active_low : 1;  /* bit[4]   : dpicolcorm信号有效电平。
                                                            0：高有效；
                                                            1：低有效。 */
        unsigned int  reserved          : 27; /* bit[5-31]: 保留。 */
    } reg;
} SOC_DSI_DPI_CFG_POL_UNION;
#endif
#define SOC_DSI_DPI_CFG_POL_dataen_active_low_START  (0)
#define SOC_DSI_DPI_CFG_POL_dataen_active_low_END    (0)
#define SOC_DSI_DPI_CFG_POL_vsync_active_low_START   (1)
#define SOC_DSI_DPI_CFG_POL_vsync_active_low_END     (1)
#define SOC_DSI_DPI_CFG_POL_hsync_active_low_START   (2)
#define SOC_DSI_DPI_CFG_POL_hsync_active_low_END     (2)
#define SOC_DSI_DPI_CFG_POL_shutd_active_low_START   (3)
#define SOC_DSI_DPI_CFG_POL_shutd_active_low_END     (3)
#define SOC_DSI_DPI_CFG_POL_colorm_active_low_START  (4)
#define SOC_DSI_DPI_CFG_POL_colorm_active_low_END    (4)


/*****************************************************************************
 结构名    : SOC_DSI_DPI_LP_CMD_TIM_UNION
 结构说明  : DPI_LP_CMD_TIM 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: DPI LP Command传输时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  invact_lpcmd_time  : 8;  /* bit[0-7]  : 在VACT周期内的LP Command传输时间阈值。代表在LP模式下，VACT周期中可以用来进行LP Command传输的最大byte数。 */
        unsigned int  reserved_0         : 8;  /* bit[8-15] : 保留。 */
        unsigned int  outvact_lpcmd_time : 8;  /* bit[16-23]: 在VACT周期外的LP Command传输时间阈值。代表在LP模式下，VSA/VBF/VFP周期中可以用来进行LP Command传输的最大byte数。 */
        unsigned int  reserved_1         : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_DSI_DPI_LP_CMD_TIM_UNION;
#endif
#define SOC_DSI_DPI_LP_CMD_TIM_invact_lpcmd_time_START   (0)
#define SOC_DSI_DPI_LP_CMD_TIM_invact_lpcmd_time_END     (7)
#define SOC_DSI_DPI_LP_CMD_TIM_outvact_lpcmd_time_START  (16)
#define SOC_DSI_DPI_LP_CMD_TIM_outvact_lpcmd_time_END    (23)


/*****************************************************************************
 结构名    : SOC_DSI_PCKHDL_CFG_UNION
 结构说明  : PCKHDL_CFG 寄存器结构定义。地址偏移量:0x002C，初值:0x00000000，宽度:32
 寄存器说明: DSI数据包配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  eotp_tx_en : 1;  /* bit[0]   : EoT包发送使能。
                                                     0：不使能；
                                                     1：使能。 */
        unsigned int  eotn_rx_en : 1;  /* bit[1]   : EoT包接收使能。
                                                     0：不使能；
                                                     1：使能。 */
        unsigned int  bta_en     : 1;  /* bit[2]   : BTA请求使能。
                                                     0：不使能；
                                                     1：使能。 */
        unsigned int  ecc_rx_en  : 1;  /* bit[3]   : ECC接收、错误纠正和上报使能。
                                                     0：不使能；
                                                     1：使能。 */
        unsigned int  crc_rx_en  : 1;  /* bit[4]   : CRC接收和错误上报使能。
                                                     0：不使能；
                                                     1：使能。 */
        unsigned int  reserved   : 27; /* bit[5-31]: 保留。 */
    } reg;
} SOC_DSI_PCKHDL_CFG_UNION;
#endif
#define SOC_DSI_PCKHDL_CFG_eotp_tx_en_START  (0)
#define SOC_DSI_PCKHDL_CFG_eotp_tx_en_END    (0)
#define SOC_DSI_PCKHDL_CFG_eotn_rx_en_START  (1)
#define SOC_DSI_PCKHDL_CFG_eotn_rx_en_END    (1)
#define SOC_DSI_PCKHDL_CFG_bta_en_START      (2)
#define SOC_DSI_PCKHDL_CFG_bta_en_END        (2)
#define SOC_DSI_PCKHDL_CFG_ecc_rx_en_START   (3)
#define SOC_DSI_PCKHDL_CFG_ecc_rx_en_END     (3)
#define SOC_DSI_PCKHDL_CFG_crc_rx_en_START   (4)
#define SOC_DSI_PCKHDL_CFG_crc_rx_en_END     (4)


/*****************************************************************************
 结构名    : SOC_DSI_GEN_VCID_UNION
 结构说明  : GEN_VCID 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: Generic接口 Virtual Channel ID配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gen_vid_rx : 2;  /* bit[0-1] : Generic接口回读数据的Virtual Channel ID。 */
        unsigned int  reserved   : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_DSI_GEN_VCID_UNION;
#endif
#define SOC_DSI_GEN_VCID_gen_vid_rx_START  (0)
#define SOC_DSI_GEN_VCID_gen_vid_rx_END    (1)


/*****************************************************************************
 结构名    : SOC_DSI_MODE_CFG_UNION
 结构说明  : MODE_CFG 寄存器结构定义。地址偏移量:0x0034，初值:0x00000001，宽度:32
 寄存器说明: DSI工作模式配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cmd_video_mode : 1;  /* bit[0]   : DSI工作模式选择。
                                                         0：Video Mode；
                                                         1：Command Mode。 */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_DSI_MODE_CFG_UNION;
#endif
#define SOC_DSI_MODE_CFG_cmd_video_mode_START  (0)
#define SOC_DSI_MODE_CFG_cmd_video_mode_END    (0)


/*****************************************************************************
 结构名    : SOC_DSI_VID_MODE_CFG_UNION
 结构说明  : VID_MODE_CFG 寄存器结构定义。地址偏移量:0x0038，初值:0x00000000，宽度:32
 寄存器说明: Video Mode配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vid_mode_type    : 2;  /* bit[0-1]  : Video Mode传输类型选择。
                                                            00：Non-burst with Sync pulses；
                                                            01：Non-burst with Sync events；
                                                            10~11：Burst with Sync pulses。 */
        unsigned int  reserved_0       : 6;  /* bit[2-7]  : 保留。 */
        unsigned int  lp_vsa_en        : 1;  /* bit[8]    : VSA周期允许进入LP状态。
                                                            0：禁止；
                                                            1：允许。 */
        unsigned int  lp_vbp_en        : 1;  /* bit[9]    : VBP周期允许进入LP状态。
                                                            0：禁止；
                                                            1：允许。 */
        unsigned int  lp_vfp_en        : 1;  /* bit[10]   : VFP周期允许进入LP状态。
                                                            0：禁止；
                                                            1：允许。 */
        unsigned int  lp_vact_en       : 1;  /* bit[11]   : VACT周期允许进入LP状态。
                                                            0：禁止；
                                                            1：允许。 */
        unsigned int  lp_hbp_en        : 1;  /* bit[12]   : HBP周期允许进入LP状态。
                                                            0：禁止；
                                                            1：允许。 */
        unsigned int  lp_hfp_en        : 1;  /* bit[13]   : HFP周期允许进入LP状态。
                                                            0：禁止；
                                                            1：允许。 */
        unsigned int  frame_bta_ack_en : 1;  /* bit[14]   : 允许帧结束时发送BTA请求。
                                                            0：禁止；
                                                            1：允许。 */
        unsigned int  lp_cmd_en        : 1;  /* bit[15]   : LP Command的传输使能。
                                                            0：不使能；
                                                            1：使能。 */
        unsigned int  reserved_1       : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_DSI_VID_MODE_CFG_UNION;
#endif
#define SOC_DSI_VID_MODE_CFG_vid_mode_type_START     (0)
#define SOC_DSI_VID_MODE_CFG_vid_mode_type_END       (1)
#define SOC_DSI_VID_MODE_CFG_lp_vsa_en_START         (8)
#define SOC_DSI_VID_MODE_CFG_lp_vsa_en_END           (8)
#define SOC_DSI_VID_MODE_CFG_lp_vbp_en_START         (9)
#define SOC_DSI_VID_MODE_CFG_lp_vbp_en_END           (9)
#define SOC_DSI_VID_MODE_CFG_lp_vfp_en_START         (10)
#define SOC_DSI_VID_MODE_CFG_lp_vfp_en_END           (10)
#define SOC_DSI_VID_MODE_CFG_lp_vact_en_START        (11)
#define SOC_DSI_VID_MODE_CFG_lp_vact_en_END          (11)
#define SOC_DSI_VID_MODE_CFG_lp_hbp_en_START         (12)
#define SOC_DSI_VID_MODE_CFG_lp_hbp_en_END           (12)
#define SOC_DSI_VID_MODE_CFG_lp_hfp_en_START         (13)
#define SOC_DSI_VID_MODE_CFG_lp_hfp_en_END           (13)
#define SOC_DSI_VID_MODE_CFG_frame_bta_ack_en_START  (14)
#define SOC_DSI_VID_MODE_CFG_frame_bta_ack_en_END    (14)
#define SOC_DSI_VID_MODE_CFG_lp_cmd_en_START         (15)
#define SOC_DSI_VID_MODE_CFG_lp_cmd_en_END           (15)


/*****************************************************************************
 结构名    : SOC_DSI_VID_PKT_SIZE_UNION
 结构说明  : VID_PKT_SIZE 寄存器结构定义。地址偏移量:0x003C，初值:0x00000000，宽度:32
 寄存器说明: Video包尺寸配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vid_pkt_size : 14; /* bit[0-13] : 单个Video包里的像素数量。
                                                        注意，如果像素为18-bit非loosely packed格式，则这个数量必须是4的整数倍。如果像素为YCbCr格式，则这个数量必须是2的整数倍。 */
        unsigned int  reserved     : 18; /* bit[14-31]: 保留。 */
    } reg;
} SOC_DSI_VID_PKT_SIZE_UNION;
#endif
#define SOC_DSI_VID_PKT_SIZE_vid_pkt_size_START  (0)
#define SOC_DSI_VID_PKT_SIZE_vid_pkt_size_END    (13)


/*****************************************************************************
 结构名    : SOC_DSI_VID_NUM_CHUNKS_UNION
 结构说明  : VID_NUM_CHUNKS 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: Chunk数量配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vid_num_chunks : 13; /* bit[0-12] : 一个行周期里要传输的Chunk的数量（Chunk包括Video包和Null包）。
                                                          注意，如果配置为0或1，则整个Video行将放在一个Video包中传输，不做拆分。如果配置为1，且vid_null_size > 0，则一个Chunk将包含一个Video包和一个Null包。其他情况下，一个Video行将被拆分成若干个Chunk传输。 */
        unsigned int  reserved       : 19; /* bit[13-31]: 保留。 */
    } reg;
} SOC_DSI_VID_NUM_CHUNKS_UNION;
#endif
#define SOC_DSI_VID_NUM_CHUNKS_vid_num_chunks_START  (0)
#define SOC_DSI_VID_NUM_CHUNKS_vid_num_chunks_END    (12)


/*****************************************************************************
 结构名    : SOC_DSI_VID_NULL_SIZE_UNION
 结构说明  : VID_NULL_SIZE 寄存器结构定义。地址偏移量:0x0044，初值:0x00000000，宽度:32
 寄存器说明: Null包尺寸配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vid_null_size : 13; /* bit[0-12] : Null包的长度，单位为字节。
                                                         注意，配置为0时将禁止Null包的产生。 */
        unsigned int  reserved      : 19; /* bit[13-31]: 保留。 */
    } reg;
} SOC_DSI_VID_NULL_SIZE_UNION;
#endif
#define SOC_DSI_VID_NULL_SIZE_vid_null_size_START  (0)
#define SOC_DSI_VID_NULL_SIZE_vid_null_size_END    (12)


/*****************************************************************************
 结构名    : SOC_DSI_VID_HSA_TIME_UNION
 结构说明  : VID_HSA_TIME 寄存器结构定义。地址偏移量:0x0048，初值:0x00000000，宽度:32
 寄存器说明: HSA时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vid_hsa_time : 12; /* bit[0-11] : HSA周期的长度，单位为lanebyteclk。 */
        unsigned int  reserved     : 20; /* bit[12-31]: 保留。 */
    } reg;
} SOC_DSI_VID_HSA_TIME_UNION;
#endif
#define SOC_DSI_VID_HSA_TIME_vid_hsa_time_START  (0)
#define SOC_DSI_VID_HSA_TIME_vid_hsa_time_END    (11)


/*****************************************************************************
 结构名    : SOC_DSI_VID_HBP_TIME_UNION
 结构说明  : VID_HBP_TIME 寄存器结构定义。地址偏移量:0x004C，初值:0x00000000，宽度:32
 寄存器说明: HBP时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vid_hbp_time : 12; /* bit[0-11] : HBP周期的长度，单位为lanebyteclk。 */
        unsigned int  reserved     : 20; /* bit[12-31]: 保留。 */
    } reg;
} SOC_DSI_VID_HBP_TIME_UNION;
#endif
#define SOC_DSI_VID_HBP_TIME_vid_hbp_time_START  (0)
#define SOC_DSI_VID_HBP_TIME_vid_hbp_time_END    (11)


/*****************************************************************************
 结构名    : SOC_DSI_VID_HLINE_TIME_UNION
 结构说明  : VID_HLINE_TIME 寄存器结构定义。地址偏移量:0x0050，初值:0x00000000，宽度:32
 寄存器说明: Video行时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vid_hline_time : 15; /* bit[0-14] : 整个Video行周期的长度（HAS+HBP+HACT+HFP），单位为lanebyteclk。 */
        unsigned int  reserved       : 17; /* bit[15-31]: 保留。 */
    } reg;
} SOC_DSI_VID_HLINE_TIME_UNION;
#endif
#define SOC_DSI_VID_HLINE_TIME_vid_hline_time_START  (0)
#define SOC_DSI_VID_HLINE_TIME_vid_hline_time_END    (14)


/*****************************************************************************
 结构名    : SOC_DSI_VID_VSA_LINES_UNION
 结构说明  : VID_VSA_LINES 寄存器结构定义。地址偏移量:0x0054，初值:0x00000000，宽度:32
 寄存器说明: VSA时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vsa_lines : 10; /* bit[0-9]  : VSA周期的长度，单位为行。 */
        unsigned int  reserved  : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_DSI_VID_VSA_LINES_UNION;
#endif
#define SOC_DSI_VID_VSA_LINES_vsa_lines_START  (0)
#define SOC_DSI_VID_VSA_LINES_vsa_lines_END    (9)


/*****************************************************************************
 结构名    : SOC_DSI_VID_VBP_LINES_UNION
 结构说明  : VID_VBP_LINES 寄存器结构定义。地址偏移量:0x0058，初值:0x00000000，宽度:32
 寄存器说明: VBP时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vbp_lines : 10; /* bit[0-9]  : VBP周期的长度，单位为行。 */
        unsigned int  reserved  : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_DSI_VID_VBP_LINES_UNION;
#endif
#define SOC_DSI_VID_VBP_LINES_vbp_lines_START  (0)
#define SOC_DSI_VID_VBP_LINES_vbp_lines_END    (9)


/*****************************************************************************
 结构名    : SOC_DSI_VID_VFP_LINES_UNION
 结构说明  : VID_VFP_LINES 寄存器结构定义。地址偏移量:0x005C，初值:0x00000000，宽度:32
 寄存器说明: VFP时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vfp_lines : 10; /* bit[0-9]  : VFP周期的长度，单位为行。 */
        unsigned int  reserved  : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_DSI_VID_VFP_LINES_UNION;
#endif
#define SOC_DSI_VID_VFP_LINES_vfp_lines_START  (0)
#define SOC_DSI_VID_VFP_LINES_vfp_lines_END    (9)


/*****************************************************************************
 结构名    : SOC_DSI_VID_VACTIVE_LINES_UNION
 结构说明  : VID_VACTIVE_LINES 寄存器结构定义。地址偏移量:0x0060，初值:0x00000000，宽度:32
 寄存器说明: VACT时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  v_active_lines : 14; /* bit[0-13] : VACT周期的长度，单位为行。 */
        unsigned int  reserved       : 18; /* bit[14-31]: 保留。 */
    } reg;
} SOC_DSI_VID_VACTIVE_LINES_UNION;
#endif
#define SOC_DSI_VID_VACTIVE_LINES_v_active_lines_START  (0)
#define SOC_DSI_VID_VACTIVE_LINES_v_active_lines_END    (13)


/*****************************************************************************
 结构名    : SOC_DSI_EDPI_CMD_SIZE_UNION
 结构说明  : EDPI_CMD_SIZE 寄存器结构定义。地址偏移量:0x0064，初值:0x00000000，宽度:32
 寄存器说明: eDPI包尺寸配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  edpi_allowed_cmd_size : 16; /* bit[0-15] : eDPI Write Memory Command允许的最大传输数据量，以像素为单位。eDPI数据包的自动拆分功能永久使能。 */
        unsigned int  reserved              : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_DSI_EDPI_CMD_SIZE_UNION;
#endif
#define SOC_DSI_EDPI_CMD_SIZE_edpi_allowed_cmd_size_START  (0)
#define SOC_DSI_EDPI_CMD_SIZE_edpi_allowed_cmd_size_END    (15)


/*****************************************************************************
 结构名    : SOC_DSI_CMD_MODE_CFG_UNION
 结构说明  : CMD_MODE_CFG 寄存器结构定义。地址偏移量:0x0068，初值:0x00000000，宽度:32
 寄存器说明: Command Mode配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tear_fx_en      : 1;  /* bit[0]    : Tearing Effect响应请求使能。
                                                           0：不使能；
                                                           1：使能。 */
        unsigned int  ack_rqst_en     : 1;  /* bit[1]    : 包传输结束请求外设响应使能。
                                                           0：不使能；
                                                           1：使能。 */
        unsigned int  reserved_0      : 6;  /* bit[2-7]  : 保留。 */
        unsigned int  gen_sw_0p_tx    : 1;  /* bit[8]    : Generic short write包（0个参数）的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  gen_sw_1p_tx    : 1;  /* bit[9]    : Generic short write包（1个参数）的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  gen_sw_2p_tx    : 1;  /* bit[10]   : Generic short write包（2个参数）的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  gen_sr_0p_tx    : 1;  /* bit[11]   : Generic short read包（0个参数）的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  gen_sr_1p_tx    : 1;  /* bit[12]   : Generic读短包（1个参数）的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  gen_sr_2p_tx    : 1;  /* bit[13]   : Generic short read包（2个参数）的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  gen_lw_tx       : 1;  /* bit[14]   : Generic long write包的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  reserved_1      : 1;  /* bit[15]   : 保留。 */
        unsigned int  dcs_sw_0p_tx    : 1;  /* bit[16]   : DCS short write包（0个参数）的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  dcs_sw_1p_tx    : 1;  /* bit[17]   : DCS short write包（1个参数）的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  dcs_sr_0p_tx    : 1;  /* bit[18]   : DCS short read包（0个参数）的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  dcs_lw_tx       : 1;  /* bit[19]   : DCS long write包的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  reserved_2      : 4;  /* bit[20-23]: 保留。 */
        unsigned int  max_rd_pkt_size : 1;  /* bit[24]   : Maximum Read Packet Size命令的传输模式。
                                                           0：HS；
                                                           1：LP。 */
        unsigned int  reserved_3      : 7;  /* bit[25-31]: 保留。 */
    } reg;
} SOC_DSI_CMD_MODE_CFG_UNION;
#endif
#define SOC_DSI_CMD_MODE_CFG_tear_fx_en_START       (0)
#define SOC_DSI_CMD_MODE_CFG_tear_fx_en_END         (0)
#define SOC_DSI_CMD_MODE_CFG_ack_rqst_en_START      (1)
#define SOC_DSI_CMD_MODE_CFG_ack_rqst_en_END        (1)
#define SOC_DSI_CMD_MODE_CFG_gen_sw_0p_tx_START     (8)
#define SOC_DSI_CMD_MODE_CFG_gen_sw_0p_tx_END       (8)
#define SOC_DSI_CMD_MODE_CFG_gen_sw_1p_tx_START     (9)
#define SOC_DSI_CMD_MODE_CFG_gen_sw_1p_tx_END       (9)
#define SOC_DSI_CMD_MODE_CFG_gen_sw_2p_tx_START     (10)
#define SOC_DSI_CMD_MODE_CFG_gen_sw_2p_tx_END       (10)
#define SOC_DSI_CMD_MODE_CFG_gen_sr_0p_tx_START     (11)
#define SOC_DSI_CMD_MODE_CFG_gen_sr_0p_tx_END       (11)
#define SOC_DSI_CMD_MODE_CFG_gen_sr_1p_tx_START     (12)
#define SOC_DSI_CMD_MODE_CFG_gen_sr_1p_tx_END       (12)
#define SOC_DSI_CMD_MODE_CFG_gen_sr_2p_tx_START     (13)
#define SOC_DSI_CMD_MODE_CFG_gen_sr_2p_tx_END       (13)
#define SOC_DSI_CMD_MODE_CFG_gen_lw_tx_START        (14)
#define SOC_DSI_CMD_MODE_CFG_gen_lw_tx_END          (14)
#define SOC_DSI_CMD_MODE_CFG_dcs_sw_0p_tx_START     (16)
#define SOC_DSI_CMD_MODE_CFG_dcs_sw_0p_tx_END       (16)
#define SOC_DSI_CMD_MODE_CFG_dcs_sw_1p_tx_START     (17)
#define SOC_DSI_CMD_MODE_CFG_dcs_sw_1p_tx_END       (17)
#define SOC_DSI_CMD_MODE_CFG_dcs_sr_0p_tx_START     (18)
#define SOC_DSI_CMD_MODE_CFG_dcs_sr_0p_tx_END       (18)
#define SOC_DSI_CMD_MODE_CFG_dcs_lw_tx_START        (19)
#define SOC_DSI_CMD_MODE_CFG_dcs_lw_tx_END          (19)
#define SOC_DSI_CMD_MODE_CFG_max_rd_pkt_size_START  (24)
#define SOC_DSI_CMD_MODE_CFG_max_rd_pkt_size_END    (24)


/*****************************************************************************
 结构名    : SOC_DSI_GEN_HDR_UNION
 结构说明  : GEN_HDR 寄存器结构定义。地址偏移量:0x006C，初值:0x00000000，宽度:32
 寄存器说明: Generic包头配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gen_dt        : 6;  /* bit[0-5]  : 包头数据Data Type。 */
        unsigned int  gen_vc        : 2;  /* bit[6-7]  : 包头数据Virtual Channel ID。 */
        unsigned int  gen_wc_lsbyte : 8;  /* bit[8-15] : 包头数据低位字节。 */
        unsigned int  gen_wc_msbyte : 8;  /* bit[16-23]: 包头数据高位字节。 */
        unsigned int  reserved      : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_DSI_GEN_HDR_UNION;
#endif
#define SOC_DSI_GEN_HDR_gen_dt_START         (0)
#define SOC_DSI_GEN_HDR_gen_dt_END           (5)
#define SOC_DSI_GEN_HDR_gen_vc_START         (6)
#define SOC_DSI_GEN_HDR_gen_vc_END           (7)
#define SOC_DSI_GEN_HDR_gen_wc_lsbyte_START  (8)
#define SOC_DSI_GEN_HDR_gen_wc_lsbyte_END    (15)
#define SOC_DSI_GEN_HDR_gen_wc_msbyte_START  (16)
#define SOC_DSI_GEN_HDR_gen_wc_msbyte_END    (23)


/*****************************************************************************
 结构名    : SOC_DSI_GEN_PLD_DATA_UNION
 结构说明  : GEN_PLD_DATA 寄存器结构定义。地址偏移量:0x0070，初值:0x00000000，宽度:32
 寄存器说明: Generic载荷数据寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gen_pld_data : 32; /* bit[0-31]: 写操作时保存要发送的Generic包的载荷数据，读操作时保存外设返回的Generic包的载荷数据。 */
    } reg;
} SOC_DSI_GEN_PLD_DATA_UNION;
#endif
#define SOC_DSI_GEN_PLD_DATA_gen_pld_data_START  (0)
#define SOC_DSI_GEN_PLD_DATA_gen_pld_data_END    (31)


/*****************************************************************************
 结构名    : SOC_DSI_CMD_PKT_STATUS_UNION
 结构说明  : CMD_PKT_STATUS 寄存器结构定义。地址偏移量:0x0074，初值:0x00000015，宽度:32
 寄存器说明: Command Mode包状态指示寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gen_cmd_empty   : 1;  /* bit[0]   : Generic命令FIFO空状态指示。
                                                          0：未空；
                                                          1：空。 */
        unsigned int  gen_cmd_full    : 1;  /* bit[1]   : Generic命令FIFO满状态指示。
                                                          0：未满；
                                                          1：满。 */
        unsigned int  gen_pld_w_empty : 1;  /* bit[2]   : Generic写载荷FIFO空状态指示。
                                                          0：未空；
                                                          1：空。 */
        unsigned int  gen_pld_w_full  : 1;  /* bit[3]   : Generic写载荷FIFO满状态指示。
                                                          0：未满；
                                                          1：满。 */
        unsigned int  gen_pld_r_empty : 1;  /* bit[4]   : Generic读载荷FIFO空状态指示。
                                                          0：未空；
                                                          1：空。 */
        unsigned int  gen_pld_r_full  : 1;  /* bit[5]   : Generic读载荷FIFO满状态指示。
                                                          0：未满；
                                                          1：满。 */
        unsigned int  gen_rd_cmd_busy : 1;  /* bit[6]   : Generic读命令忙指示。
                                                          0：空闲；
                                                          1：忙。 */
        unsigned int  reserved        : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_DSI_CMD_PKT_STATUS_UNION;
#endif
#define SOC_DSI_CMD_PKT_STATUS_gen_cmd_empty_START    (0)
#define SOC_DSI_CMD_PKT_STATUS_gen_cmd_empty_END      (0)
#define SOC_DSI_CMD_PKT_STATUS_gen_cmd_full_START     (1)
#define SOC_DSI_CMD_PKT_STATUS_gen_cmd_full_END       (1)
#define SOC_DSI_CMD_PKT_STATUS_gen_pld_w_empty_START  (2)
#define SOC_DSI_CMD_PKT_STATUS_gen_pld_w_empty_END    (2)
#define SOC_DSI_CMD_PKT_STATUS_gen_pld_w_full_START   (3)
#define SOC_DSI_CMD_PKT_STATUS_gen_pld_w_full_END     (3)
#define SOC_DSI_CMD_PKT_STATUS_gen_pld_r_empty_START  (4)
#define SOC_DSI_CMD_PKT_STATUS_gen_pld_r_empty_END    (4)
#define SOC_DSI_CMD_PKT_STATUS_gen_pld_r_full_START   (5)
#define SOC_DSI_CMD_PKT_STATUS_gen_pld_r_full_END     (5)
#define SOC_DSI_CMD_PKT_STATUS_gen_rd_cmd_busy_START  (6)
#define SOC_DSI_CMD_PKT_STATUS_gen_rd_cmd_busy_END    (6)


/*****************************************************************************
 结构名    : SOC_DSI_TO_CNT_CFG_UNION
 结构说明  : TO_CNT_CFG 寄存器结构定义。地址偏移量:0x0078，初值:0x00000000，宽度:32
 寄存器说明: 超时计数器配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lprx_to_cnt : 16; /* bit[0-15] : LP接收超时计数器，单位为TO_CLK_DIVISION时钟。 */
        unsigned int  hstx_to_cnt : 16; /* bit[16-31]: HS传输超时计数器，单位为TO_CLK_DIVISION时钟。
                                                       如果Video包使用non-burst模式传输，且在两行之间没有足够的时间从HS切换到LP（或反之），则DSI链路只会在帧结束以后回到LP状态，这时TO_CLK_DIVISION和hstx_to_cnt的配置应满足以下条件：
                                                       hstx_to_cnt * lanebyteclkperiod * TO_CLK_DIVISION >= 一帧数据传输时间 * (1 + 10%)。
                                                       如果Video包使用burst模式传输，一行的RGB像素数据只使用一个长包传输，两行之间就可能有足够的时间从HS切换到LP（或反之），则DSI链路在两行像素的传输间隙就可以回到LP状态，以降低功耗，这时TO_CLK_DIVISION和hstx_to_cnt的配置应满足以下条件：
                                                       hstx_to_cnt * lanebyteclkperiod * TO_CLK_DIVISION >= 一行数据传输时间 * (1 + 10%)。 */
    } reg;
} SOC_DSI_TO_CNT_CFG_UNION;
#endif
#define SOC_DSI_TO_CNT_CFG_lprx_to_cnt_START  (0)
#define SOC_DSI_TO_CNT_CFG_lprx_to_cnt_END    (15)
#define SOC_DSI_TO_CNT_CFG_hstx_to_cnt_START  (16)
#define SOC_DSI_TO_CNT_CFG_hstx_to_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_DSI_HS_RD_TO_CNT_UNION
 结构说明  : HS_RD_TO_CNT 寄存器结构定义。地址偏移量:0x007C，初值:0x00000000，宽度:32
 寄存器说明: HS读操作外设响应超时配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hs_rd_to_cnt : 16; /* bit[0-15] : DSI controller发出HS读操作后的最长等待时间，单位为lanebyteclk时钟。计数器从读操作发出后且DPHY进入STOP状态时开始计数。 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_DSI_HS_RD_TO_CNT_UNION;
#endif
#define SOC_DSI_HS_RD_TO_CNT_hs_rd_to_cnt_START  (0)
#define SOC_DSI_HS_RD_TO_CNT_hs_rd_to_cnt_END    (15)


/*****************************************************************************
 结构名    : SOC_DSI_LP_RD_TO_CNT_UNION
 结构说明  : LP_RD_TO_CNT 寄存器结构定义。地址偏移量:0x0080，初值:0x00000000，宽度:32
 寄存器说明: LP读操作外设响应超时配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lp_rd_to_cnt : 16; /* bit[0-15] : DSI controller发出LP读操作后的最长等待时间，单位为lanebyteclk时钟。计数器从读操作发出后且DPHY进入STOP状态时开始计数。 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_DSI_LP_RD_TO_CNT_UNION;
#endif
#define SOC_DSI_LP_RD_TO_CNT_lp_rd_to_cnt_START  (0)
#define SOC_DSI_LP_RD_TO_CNT_lp_rd_to_cnt_END    (15)


/*****************************************************************************
 结构名    : SOC_DSI_HS_WR_TO_CNT_UNION
 结构说明  : HS_WR_TO_CNT 寄存器结构定义。地址偏移量:0x0084，初值:0x00000000，宽度:32
 寄存器说明: HS写操作外设响应超时配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hs_wr_to_cnt  : 16; /* bit[0-15] : DSI controller发出HS写操作后的最长等待时间，单位为lanebyteclk时钟。计数器从写操作发出后且DPHY进入STOP状态时开始计数。 */
        unsigned int  reserved_0    : 8;  /* bit[16-23]: 保留。 */
        unsigned int  presp_to_mode : 1;  /* bit[24]   : 外设响应超时模式。当配置为1且满足以下条件时，由hs_wr_to_cnt导致的超时事件每个eDPI帧只产生一次：
                                                         1、收到dpivsync_edpiwms脉冲信号；
                                                         2、eDPI包发送完成，FIFO为空。
                                                         在这个场景下，非eDPI包将不会发送给DPHY，即使Generic接口有发包需求。DPHY将进入STOP状态，PRESP_TO计数器开始计数。只有等计数结束以后，非eDPI包才回被发送。 */
        unsigned int  reserved_1    : 7;  /* bit[25-31]: 保留。 */
    } reg;
} SOC_DSI_HS_WR_TO_CNT_UNION;
#endif
#define SOC_DSI_HS_WR_TO_CNT_hs_wr_to_cnt_START   (0)
#define SOC_DSI_HS_WR_TO_CNT_hs_wr_to_cnt_END     (15)
#define SOC_DSI_HS_WR_TO_CNT_presp_to_mode_START  (24)
#define SOC_DSI_HS_WR_TO_CNT_presp_to_mode_END    (24)


/*****************************************************************************
 结构名    : SOC_DSI_LP_WR_TO_CNT_UNION
 结构说明  : LP_WR_TO_CNT 寄存器结构定义。地址偏移量:0x0088，初值:0x00000000，宽度:32
 寄存器说明: LP写操作外设响应超时配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lp_wr_to_cnt : 16; /* bit[0-15] : DSI controller发出LP写操作后的最长等待时间，单位为lanebyteclk时钟。计数器从写操作发出后且DPHY进入STOP状态时开始计数。 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_DSI_LP_WR_TO_CNT_UNION;
#endif
#define SOC_DSI_LP_WR_TO_CNT_lp_wr_to_cnt_START  (0)
#define SOC_DSI_LP_WR_TO_CNT_lp_wr_to_cnt_END    (15)


/*****************************************************************************
 结构名    : SOC_DSI_BTA_TO_CNT_UNION
 结构说明  : BTA_TO_CNT 寄存器结构定义。地址偏移量:0x008C，初值:0x00000000，宽度:32
 寄存器说明: BTA操作外设响应超时配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bta_to_cnt : 16; /* bit[0-15] : DSI controller发出BTA操作后的最长等待时间，单位为lanebyteclk时钟。计数器从BTA操作发出后且DPHY进入STOP状态时开始计数。 */
        unsigned int  reserved   : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_DSI_BTA_TO_CNT_UNION;
#endif
#define SOC_DSI_BTA_TO_CNT_bta_to_cnt_START  (0)
#define SOC_DSI_BTA_TO_CNT_bta_to_cnt_END    (15)


/*****************************************************************************
 结构名    : SOC_DSI_SDF_3D_UNION
 结构说明  : SDF_3D 寄存器结构定义。地址偏移量:0x0090，初值:0x00000000，宽度:32
 寄存器说明: Video Mode 3D模式配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode_3d      : 2;  /* bit[0-1]  : 3D模式开关和显示方向控制。
                                                        00：2D模式；
                                                        01：3D模式，纵向显示；
                                                        10：3D模式，横向显示；
                                                        11：保留。 */
        unsigned int  format_3d    : 2;  /* bit[2-3]  : 3D图像格式。
                                                        00：Line by Line；
                                                        01：Frame by Frame；
                                                        10：Pixel by Pixel；
                                                        11：保留。 */
        unsigned int  second_vsync : 1;  /* bit[4]    : Frame by Frame 3D格式时左眼帧和右眼帧之间是否有VSYNC脉冲信号。
                                                        0：没有；
                                                        1：有。 */
        unsigned int  right_first  : 1;  /* bit[5]    : 左右眼显示顺序。
                                                        0：左眼在前；
                                                        1：右眼在前。 */
        unsigned int  reserved_0   : 10; /* bit[6-15] : 保留。 */
        unsigned int  send_3d_cfg  : 1;  /* bit[16]   : VSS包是否包括含3D控制信息。
                                                        0：不包含；
                                                        1：包含。 */
        unsigned int  reserved_1   : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_DSI_SDF_3D_UNION;
#endif
#define SOC_DSI_SDF_3D_mode_3d_START       (0)
#define SOC_DSI_SDF_3D_mode_3d_END         (1)
#define SOC_DSI_SDF_3D_format_3d_START     (2)
#define SOC_DSI_SDF_3D_format_3d_END       (3)
#define SOC_DSI_SDF_3D_second_vsync_START  (4)
#define SOC_DSI_SDF_3D_second_vsync_END    (4)
#define SOC_DSI_SDF_3D_right_first_START   (5)
#define SOC_DSI_SDF_3D_right_first_END     (5)
#define SOC_DSI_SDF_3D_send_3d_cfg_START   (16)
#define SOC_DSI_SDF_3D_send_3d_cfg_END     (16)


/*****************************************************************************
 结构名    : SOC_DSI_LPCLK_CTRL_UNION
 结构说明  : LPCLK_CTRL 寄存器结构定义。地址偏移量:0x0094，初值:0x00000000，宽度:32
 寄存器说明: Clock Lane配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_txrequestclkhs : 1;  /* bit[0]   : 向Clock Lane发起HS时钟传输请求。
                                                             0：不发起；
                                                             1：发起。 */
        unsigned int  auto_clklane_ctrl  : 1;  /* bit[1]   : non-continues clock使能：
                                                             0：不使能；
                                                             1：使能。 */
        unsigned int  reserved           : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_DSI_LPCLK_CTRL_UNION;
#endif
#define SOC_DSI_LPCLK_CTRL_phy_txrequestclkhs_START  (0)
#define SOC_DSI_LPCLK_CTRL_phy_txrequestclkhs_END    (0)
#define SOC_DSI_LPCLK_CTRL_auto_clklane_ctrl_START   (1)
#define SOC_DSI_LPCLK_CTRL_auto_clklane_ctrl_END     (1)


/*****************************************************************************
 结构名    : SOC_DSI_PHY_TMR_LPCLK_CFG_UNION
 结构说明  : PHY_TMR_LPCLK_CFG 寄存器结构定义。地址偏移量:0x0098，初值:0x00000000，宽度:32
 寄存器说明: Clock Lane时序配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_clklp2hs_time : 10; /* bit[0-9]  : Clock Lane从LP模式切换到HS模式需要的最长时间，单位为lanebyteclk时钟。 */
        unsigned int  reserved_0        : 6;  /* bit[10-15]: 保留。 */
        unsigned int  phy_clkhs2lp_time : 10; /* bit[16-25]: Clock Lane从HS模式切换到LP模式需要的最长时间，单位为lanebyteclk时钟。 */
        unsigned int  reserved_1        : 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_DSI_PHY_TMR_LPCLK_CFG_UNION;
#endif
#define SOC_DSI_PHY_TMR_LPCLK_CFG_phy_clklp2hs_time_START  (0)
#define SOC_DSI_PHY_TMR_LPCLK_CFG_phy_clklp2hs_time_END    (9)
#define SOC_DSI_PHY_TMR_LPCLK_CFG_phy_clkhs2lp_time_START  (16)
#define SOC_DSI_PHY_TMR_LPCLK_CFG_phy_clkhs2lp_time_END    (25)


/*****************************************************************************
 结构名    : SOC_DSI_PHY_TMR_CFG_UNION
 结构说明  : PHY_TMR_CFG 寄存器结构定义。地址偏移量:0x009C，初值:0x00000000，宽度:32
 寄存器说明: Data Lane时序配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  max_rd_time    : 15; /* bit[0-14] : Data Lane进行读操作需要的最长时间，单位为lanebyteclk时钟。
                                                          注意，读操作发出以后禁止配置此寄存器。 */
        unsigned int  reserved       : 1;  /* bit[15]   : 保留。 */
        unsigned int  phy_lp2hs_time : 8;  /* bit[16-23]: Data Lane从LP模式切换到HS模式需要的最长时间，单位为lanebyteclk时钟。 */
        unsigned int  phy_hs2lp_time : 8;  /* bit[24-31]: Data Lane从HS模式切换到LP模式需要的最长时间，单位为lanebyteclk时钟。 */
    } reg;
} SOC_DSI_PHY_TMR_CFG_UNION;
#endif
#define SOC_DSI_PHY_TMR_CFG_max_rd_time_START     (0)
#define SOC_DSI_PHY_TMR_CFG_max_rd_time_END       (14)
#define SOC_DSI_PHY_TMR_CFG_phy_lp2hs_time_START  (16)
#define SOC_DSI_PHY_TMR_CFG_phy_lp2hs_time_END    (23)
#define SOC_DSI_PHY_TMR_CFG_phy_hs2lp_time_START  (24)
#define SOC_DSI_PHY_TMR_CFG_phy_hs2lp_time_END    (31)


/*****************************************************************************
 结构名    : SOC_DSI_PHY_RSTZ_UNION
 结构说明  : PHY_RSTZ 寄存器结构定义。地址偏移量:0x00A0，初值:0x00000000，宽度:32
 寄存器说明: DPHY复位和PLL控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_shutdownz : 1;  /* bit[0]   : DPHY关闭控制。
                                                        0：关闭；
                                                        1：打开。
                                                        配置为0时将使DPHY进入Shutdown状态。 */
        unsigned int  phy_rstz      : 1;  /* bit[1]   : DPHY复位控制。
                                                        0：复位；
                                                        1：解复位。
                                                        配置为0时将复位DPHY的数字逻辑。 */
        unsigned int  phy_enableclk : 1;  /* bit[2]   : DPHY Clock Lane使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  phy_forcepll  : 1;  /* bit[3]   : 当DPHY处于ULPS模式，选择是否关闭PLL。
                                                        0：关闭PLL；
                                                        1：打开PLL。 */
        unsigned int  reserved      : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_DSI_PHY_RSTZ_UNION;
#endif
#define SOC_DSI_PHY_RSTZ_phy_shutdownz_START  (0)
#define SOC_DSI_PHY_RSTZ_phy_shutdownz_END    (0)
#define SOC_DSI_PHY_RSTZ_phy_rstz_START       (1)
#define SOC_DSI_PHY_RSTZ_phy_rstz_END         (1)
#define SOC_DSI_PHY_RSTZ_phy_enableclk_START  (2)
#define SOC_DSI_PHY_RSTZ_phy_enableclk_END    (2)
#define SOC_DSI_PHY_RSTZ_phy_forcepll_START   (3)
#define SOC_DSI_PHY_RSTZ_phy_forcepll_END     (3)


/*****************************************************************************
 结构名    : SOC_DSI_PHY_IF_CFG_UNION
 结构说明  : PHY_IF_CFG 寄存器结构定义。地址偏移量:0x00A4，初值:0x00000003，宽度:32
 寄存器说明: DPHY接口配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  n_lanes            : 2;  /* bit[0-1]  : 活动的Data Lane的数量。
                                                              00：1 Data Lane (Lane 0)；
                                                              01：2 Data Lanes (Lane 0、1)；
                                                              10：3 Data Lanes (Lane 0、1、2)；
                                                              11：4 Data Lanes (Lane 0、1、2、3)。 */
        unsigned int  reserved_0         : 6;  /* bit[2-7]  : 保留。 */
        unsigned int  phy_stop_wait_time : 8;  /* bit[8-15] : DPHY从Stop状态到发起HS传输请求的最短时间，单位为lanebyteclk周期。 */
        unsigned int  reserved_1         : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_DSI_PHY_IF_CFG_UNION;
#endif
#define SOC_DSI_PHY_IF_CFG_n_lanes_START             (0)
#define SOC_DSI_PHY_IF_CFG_n_lanes_END               (1)
#define SOC_DSI_PHY_IF_CFG_phy_stop_wait_time_START  (8)
#define SOC_DSI_PHY_IF_CFG_phy_stop_wait_time_END    (15)


/*****************************************************************************
 结构名    : SOC_DSI_PHY_ULPS_CTRL_UNION
 结构说明  : PHY_ULPS_CTRL 寄存器结构定义。地址偏移量:0x00A8，初值:0x00000000，宽度:32
 寄存器说明: DPHY ULPS模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_txrequlpsclk  : 1;  /* bit[0]   : 向Clock Lane发起ULPS进入请求。
                                                            0：不发起；
                                                            1：发起。 */
        unsigned int  phy_txexitulpsclk : 1;  /* bit[1]   : 向Clock Lane发起ULPS退出请求。
                                                            0：不发起；
                                                            1：发起。 */
        unsigned int  phy_txrequlpslan  : 1;  /* bit[2]   : 向所有Data Lane发起ULPS进入请求。
                                                            0：不发起；
                                                            1：发起。 */
        unsigned int  phy_txexitulpslan : 1;  /* bit[3]   : 向所有Data Lane发起ULPS退出请求。
                                                            0：不发起；
                                                            1：发起。 */
        unsigned int  reserved          : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_DSI_PHY_ULPS_CTRL_UNION;
#endif
#define SOC_DSI_PHY_ULPS_CTRL_phy_txrequlpsclk_START   (0)
#define SOC_DSI_PHY_ULPS_CTRL_phy_txrequlpsclk_END     (0)
#define SOC_DSI_PHY_ULPS_CTRL_phy_txexitulpsclk_START  (1)
#define SOC_DSI_PHY_ULPS_CTRL_phy_txexitulpsclk_END    (1)
#define SOC_DSI_PHY_ULPS_CTRL_phy_txrequlpslan_START   (2)
#define SOC_DSI_PHY_ULPS_CTRL_phy_txrequlpslan_END     (2)
#define SOC_DSI_PHY_ULPS_CTRL_phy_txexitulpslan_START  (3)
#define SOC_DSI_PHY_ULPS_CTRL_phy_txexitulpslan_END    (3)


/*****************************************************************************
 结构名    : SOC_DSI_PHY_TX_TRIGGERS_UNION
 结构说明  : PHY_TX_TRIGGERS 寄存器结构定义。地址偏移量:0x00AC，初值:0x00000000，宽度:32
 寄存器说明: DPHY TRIGGERS配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_tx_triggers : 4;  /* bit[0-3] : DPHY进行Trigger传输时的值。 */
        unsigned int  reserved        : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_DSI_PHY_TX_TRIGGERS_UNION;
#endif
#define SOC_DSI_PHY_TX_TRIGGERS_phy_tx_triggers_START  (0)
#define SOC_DSI_PHY_TX_TRIGGERS_phy_tx_triggers_END    (3)


/*****************************************************************************
 结构名    : SOC_DSI_PHY_STATUS_UNION
 结构说明  : PHY_STATUS 寄存器结构定义。地址偏移量:0x00B0，初值:0x00001528，宽度:32
 寄存器说明: DPHY状态指示寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_lock               : 1;  /* bit[0]    : 指示DPHY PPI接口信号phylock的状态。 */
        unsigned int  phy_direction          : 1;  /* bit[1]    : 指示DPHY PPI接口信号phydirection的状态。 */
        unsigned int  phy_stopstateclklane   : 1;  /* bit[2]    : 指示DPHY PPI接口信号phystopstateclklane的状态。 */
        unsigned int  phy_ulpsactivenotclk   : 1;  /* bit[3]    : 指示DPHY PPI接口信号phyulpsactivenotclk的状态。 */
        unsigned int  phy_stopstate0lane     : 1;  /* bit[4]    : 指示DPHY PPI接口信号phystopstate0lane的状态。 */
        unsigned int  phy_ulpsactivenot0lane : 1;  /* bit[5]    : 指示DPHY PPI接口信号ulpsactivenot0lane的状态。 */
        unsigned int  phy_rxulpsesc0lane     : 1;  /* bit[6]    : 指示DPHY PPI接口信号rxulpsesc0lane的状态。 */
        unsigned int  phy_stopstate1lane     : 1;  /* bit[7]    : 指示DPHY PPI接口信号phystopstate1lane的状态。 */
        unsigned int  phy_ulpsactivenot1lane : 1;  /* bit[8]    : 指示DPHY PPI接口信号ulpsactivenot1lane的状态。 */
        unsigned int  phy_stopstate2lane     : 1;  /* bit[9]    : 指示DPHY PPI接口信号phystopstate2lane的状态。 */
        unsigned int  phy_ulpsactivenot2lane : 1;  /* bit[10]   : 指示DPHY PPI接口信号ulpsactivenot2lane的状态。 */
        unsigned int  phy_stopstate3lane     : 1;  /* bit[11]   : 指示DPHY PPI接口信号phystopstate3lane的状态。 */
        unsigned int  phy_ulpsactivenot3lane : 1;  /* bit[12]   : 指示DPHY PPI接口信号ulpsactivenot3lane的状态。 */
        unsigned int  reserved               : 19; /* bit[13-31]: 保留。 */
    } reg;
} SOC_DSI_PHY_STATUS_UNION;
#endif
#define SOC_DSI_PHY_STATUS_phy_lock_START                (0)
#define SOC_DSI_PHY_STATUS_phy_lock_END                  (0)
#define SOC_DSI_PHY_STATUS_phy_direction_START           (1)
#define SOC_DSI_PHY_STATUS_phy_direction_END             (1)
#define SOC_DSI_PHY_STATUS_phy_stopstateclklane_START    (2)
#define SOC_DSI_PHY_STATUS_phy_stopstateclklane_END      (2)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenotclk_START    (3)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenotclk_END      (3)
#define SOC_DSI_PHY_STATUS_phy_stopstate0lane_START      (4)
#define SOC_DSI_PHY_STATUS_phy_stopstate0lane_END        (4)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenot0lane_START  (5)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenot0lane_END    (5)
#define SOC_DSI_PHY_STATUS_phy_rxulpsesc0lane_START      (6)
#define SOC_DSI_PHY_STATUS_phy_rxulpsesc0lane_END        (6)
#define SOC_DSI_PHY_STATUS_phy_stopstate1lane_START      (7)
#define SOC_DSI_PHY_STATUS_phy_stopstate1lane_END        (7)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenot1lane_START  (8)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenot1lane_END    (8)
#define SOC_DSI_PHY_STATUS_phy_stopstate2lane_START      (9)
#define SOC_DSI_PHY_STATUS_phy_stopstate2lane_END        (9)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenot2lane_START  (10)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenot2lane_END    (10)
#define SOC_DSI_PHY_STATUS_phy_stopstate3lane_START      (11)
#define SOC_DSI_PHY_STATUS_phy_stopstate3lane_END        (11)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenot3lane_START  (12)
#define SOC_DSI_PHY_STATUS_phy_ulpsactivenot3lane_END    (12)


/*****************************************************************************
 结构名    : SOC_DSI_PHY_TST_CTRL0_UNION
 结构说明  : PHY_TST_CTRL0 寄存器结构定义。地址偏移量:0x00B4，初值:0x00000001，宽度:32
 寄存器说明: DPHY配置接口控制寄存器0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_testclr : 1;  /* bit[0]   : DPHY测试接口复位。
                                                      0：解复位；
                                                      1：复位。 */
        unsigned int  phy_testclk : 1;  /* bit[1]   : DPHY测试接口时钟，与phy_testen共同进行操作选择。 */
        unsigned int  reserved    : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_DSI_PHY_TST_CTRL0_UNION;
#endif
#define SOC_DSI_PHY_TST_CTRL0_phy_testclr_START  (0)
#define SOC_DSI_PHY_TST_CTRL0_phy_testclr_END    (0)
#define SOC_DSI_PHY_TST_CTRL0_phy_testclk_START  (1)
#define SOC_DSI_PHY_TST_CTRL0_phy_testclk_END    (1)


/*****************************************************************************
 结构名    : SOC_DSI_PHY_TST_CTRL1_UNION
 结构说明  : PHY_TST_CTRL1 寄存器结构定义。地址偏移量:0x00B8，初值:0x00000000，宽度:32
 寄存器说明: DPHY配置接口控制寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  phy_testdin  : 8;  /* bit[0-7]  : DPHY配置接口8-bit数据输入。 */
        unsigned int  phy_testdout : 8;  /* bit[8-15] : DPHY配置接口8-bit数据输出。 */
        unsigned int  phy_testen   : 1;  /* bit[16]   : DPHY配置接口操作模式选择。
                                                        0：phy_testclk上升沿写数据；
                                                        1：phy_testclk下降沿写地址。 */
        unsigned int  reserved     : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_DSI_PHY_TST_CTRL1_UNION;
#endif
#define SOC_DSI_PHY_TST_CTRL1_phy_testdin_START   (0)
#define SOC_DSI_PHY_TST_CTRL1_phy_testdin_END     (7)
#define SOC_DSI_PHY_TST_CTRL1_phy_testdout_START  (8)
#define SOC_DSI_PHY_TST_CTRL1_phy_testdout_END    (15)
#define SOC_DSI_PHY_TST_CTRL1_phy_testen_START    (16)
#define SOC_DSI_PHY_TST_CTRL1_phy_testen_END      (16)


/*****************************************************************************
 结构名    : SOC_DSI_INT_ST0_UNION
 结构说明  : INT_ST0 寄存器结构定义。地址偏移量:0x00BC，初值:0x00000000，宽度:32
 寄存器说明: 中断状态寄存器0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ack_with_err_0  : 1;  /* bit[0]    : SoT错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_1  : 1;  /* bit[1]    : SoT同步错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_2  : 1;  /* bit[2]    : EoT同步错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_3  : 1;  /* bit[3]    : Escape模式入口命令错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_4  : 1;  /* bit[4]    : LP传输同步错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_5  : 1;  /* bit[5]    : 外设超时错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_6  : 1;  /* bit[6]    : 假控制错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_7  : 1;  /* bit[7]    : 保留。 */
        unsigned int  ack_with_err_8  : 1;  /* bit[8]    : 单比特ECC错误（可纠正）指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_9  : 1;  /* bit[9]    : 多比特ECC错误（不可纠正）指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_10 : 1;  /* bit[10]   : 长包Checksum错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_11 : 1;  /* bit[11]   : DSI未知数据类型错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_12 : 1;  /* bit[12]   : DSI VC ID无效错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_13 : 1;  /* bit[13]   : 传输长度无效错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  ack_with_err_14 : 1;  /* bit[14]   : 保留。 */
        unsigned int  ack_with_err_15 : 1;  /* bit[15]   : DSI协议违规错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  dphy_errors_0   : 1;  /* bit[16]   : Lane 0 Escape入口错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  dphy_errors_1   : 1;  /* bit[17]   : Lane 0 LP数据传输同步错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  dphy_errors_2   : 1;  /* bit[18]   : Lane 0 控制错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  dphy_errors_3   : 1;  /* bit[19]   : Lane 0 LP0争用错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  dphy_errors_4   : 1;  /* bit[20]   : Lane 0 LP1争用错误指示。
                                                           0：无错误；
                                                           1：错误。 */
        unsigned int  reserved        : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_DSI_INT_ST0_UNION;
#endif
#define SOC_DSI_INT_ST0_ack_with_err_0_START   (0)
#define SOC_DSI_INT_ST0_ack_with_err_0_END     (0)
#define SOC_DSI_INT_ST0_ack_with_err_1_START   (1)
#define SOC_DSI_INT_ST0_ack_with_err_1_END     (1)
#define SOC_DSI_INT_ST0_ack_with_err_2_START   (2)
#define SOC_DSI_INT_ST0_ack_with_err_2_END     (2)
#define SOC_DSI_INT_ST0_ack_with_err_3_START   (3)
#define SOC_DSI_INT_ST0_ack_with_err_3_END     (3)
#define SOC_DSI_INT_ST0_ack_with_err_4_START   (4)
#define SOC_DSI_INT_ST0_ack_with_err_4_END     (4)
#define SOC_DSI_INT_ST0_ack_with_err_5_START   (5)
#define SOC_DSI_INT_ST0_ack_with_err_5_END     (5)
#define SOC_DSI_INT_ST0_ack_with_err_6_START   (6)
#define SOC_DSI_INT_ST0_ack_with_err_6_END     (6)
#define SOC_DSI_INT_ST0_ack_with_err_7_START   (7)
#define SOC_DSI_INT_ST0_ack_with_err_7_END     (7)
#define SOC_DSI_INT_ST0_ack_with_err_8_START   (8)
#define SOC_DSI_INT_ST0_ack_with_err_8_END     (8)
#define SOC_DSI_INT_ST0_ack_with_err_9_START   (9)
#define SOC_DSI_INT_ST0_ack_with_err_9_END     (9)
#define SOC_DSI_INT_ST0_ack_with_err_10_START  (10)
#define SOC_DSI_INT_ST0_ack_with_err_10_END    (10)
#define SOC_DSI_INT_ST0_ack_with_err_11_START  (11)
#define SOC_DSI_INT_ST0_ack_with_err_11_END    (11)
#define SOC_DSI_INT_ST0_ack_with_err_12_START  (12)
#define SOC_DSI_INT_ST0_ack_with_err_12_END    (12)
#define SOC_DSI_INT_ST0_ack_with_err_13_START  (13)
#define SOC_DSI_INT_ST0_ack_with_err_13_END    (13)
#define SOC_DSI_INT_ST0_ack_with_err_14_START  (14)
#define SOC_DSI_INT_ST0_ack_with_err_14_END    (14)
#define SOC_DSI_INT_ST0_ack_with_err_15_START  (15)
#define SOC_DSI_INT_ST0_ack_with_err_15_END    (15)
#define SOC_DSI_INT_ST0_dphy_errors_0_START    (16)
#define SOC_DSI_INT_ST0_dphy_errors_0_END      (16)
#define SOC_DSI_INT_ST0_dphy_errors_1_START    (17)
#define SOC_DSI_INT_ST0_dphy_errors_1_END      (17)
#define SOC_DSI_INT_ST0_dphy_errors_2_START    (18)
#define SOC_DSI_INT_ST0_dphy_errors_2_END      (18)
#define SOC_DSI_INT_ST0_dphy_errors_3_START    (19)
#define SOC_DSI_INT_ST0_dphy_errors_3_END      (19)
#define SOC_DSI_INT_ST0_dphy_errors_4_START    (20)
#define SOC_DSI_INT_ST0_dphy_errors_4_END      (20)


/*****************************************************************************
 结构名    : SOC_DSI_INT_ST1_UNION
 结构说明  : INT_ST1 寄存器结构定义。地址偏移量:0x00C0，初值:0x00000000，宽度:32
 寄存器说明: 中断状态寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  to_hs_tx         : 1;  /* bit[0]    : HS传输超时错误（检测到争用）指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  to_lp_rx         : 1;  /* bit[1]    : LP接收超时错误（检测到争用）指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  ecc_single_err   : 1;  /* bit[2]    : 接收的包单比特ECC错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  ecc_multi_err    : 1;  /* bit[3]    : 接收的包多比特ECC错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  crc_err          : 1;  /* bit[4]    : 接收的包CRC错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  pkt_size_err     : 1;  /* bit[5]    : 接收过程中包长错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  eopt_err         : 1;  /* bit[6]    : 外设传输完成后未收到外设传输的EoT包错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  dpi_pld_wr_err   : 1;  /* bit[7]    : DPI像素数据FIFO上溢错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  gen_cmd_wr_err   : 1;  /* bit[8]    : Generic写命令FIFO上溢错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  gen_pld_wr_err   : 1;  /* bit[9]    : Generic写数据FIFO上溢错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  gen_pld_send_err : 1;  /* bit[10]   : Generic数据发送FIFO下溢错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  gen_pld_rd_err   : 1;  /* bit[11]   : Generic数据回读FIFO下溢错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  gen_pld_recv_err : 1;  /* bit[12]   : Generic数据回读FIFO上溢错误指示。
                                                            0：无错误；
                                                            1：错误。 */
        unsigned int  reserved         : 19; /* bit[13-31]: 保留。 */
    } reg;
} SOC_DSI_INT_ST1_UNION;
#endif
#define SOC_DSI_INT_ST1_to_hs_tx_START          (0)
#define SOC_DSI_INT_ST1_to_hs_tx_END            (0)
#define SOC_DSI_INT_ST1_to_lp_rx_START          (1)
#define SOC_DSI_INT_ST1_to_lp_rx_END            (1)
#define SOC_DSI_INT_ST1_ecc_single_err_START    (2)
#define SOC_DSI_INT_ST1_ecc_single_err_END      (2)
#define SOC_DSI_INT_ST1_ecc_multi_err_START     (3)
#define SOC_DSI_INT_ST1_ecc_multi_err_END       (3)
#define SOC_DSI_INT_ST1_crc_err_START           (4)
#define SOC_DSI_INT_ST1_crc_err_END             (4)
#define SOC_DSI_INT_ST1_pkt_size_err_START      (5)
#define SOC_DSI_INT_ST1_pkt_size_err_END        (5)
#define SOC_DSI_INT_ST1_eopt_err_START          (6)
#define SOC_DSI_INT_ST1_eopt_err_END            (6)
#define SOC_DSI_INT_ST1_dpi_pld_wr_err_START    (7)
#define SOC_DSI_INT_ST1_dpi_pld_wr_err_END      (7)
#define SOC_DSI_INT_ST1_gen_cmd_wr_err_START    (8)
#define SOC_DSI_INT_ST1_gen_cmd_wr_err_END      (8)
#define SOC_DSI_INT_ST1_gen_pld_wr_err_START    (9)
#define SOC_DSI_INT_ST1_gen_pld_wr_err_END      (9)
#define SOC_DSI_INT_ST1_gen_pld_send_err_START  (10)
#define SOC_DSI_INT_ST1_gen_pld_send_err_END    (10)
#define SOC_DSI_INT_ST1_gen_pld_rd_err_START    (11)
#define SOC_DSI_INT_ST1_gen_pld_rd_err_END      (11)
#define SOC_DSI_INT_ST1_gen_pld_recv_err_START  (12)
#define SOC_DSI_INT_ST1_gen_pld_recv_err_END    (12)


/*****************************************************************************
 结构名    : SOC_DSI_INT_MSK0_UNION
 结构说明  : INT_MSK0 寄存器结构定义。地址偏移量:0x00C4，初值:0x001FFFFF，宽度:32
 寄存器说明: 中断屏蔽寄存器0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mask_ack_with_err_0  : 1;  /* bit[0]    : ack_with_err_0中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_1  : 1;  /* bit[1]    : ack_with_err_1中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_2  : 1;  /* bit[2]    : ack_with_err_2中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_3  : 1;  /* bit[3]    : ack_with_err_3中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_4  : 1;  /* bit[4]    : ack_with_err_4中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_5  : 1;  /* bit[5]    : ack_with_err_5中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_6  : 1;  /* bit[6]    : ack_with_err_6中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_7  : 1;  /* bit[7]    : ack_with_err_7中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_8  : 1;  /* bit[8]    : ack_with_err_8中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_9  : 1;  /* bit[9]    : ack_with_err_9中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_10 : 1;  /* bit[10]   : ack_with_err_10中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_11 : 1;  /* bit[11]   : ack_with_err_11中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_12 : 1;  /* bit[12]   : ack_with_err_12中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_13 : 1;  /* bit[13]   : ack_with_err_13中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_14 : 1;  /* bit[14]   : ack_with_err_14中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_ack_with_err_15 : 1;  /* bit[15]   : ack_with_err_15中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_dphy_errors_0   : 1;  /* bit[16]   : dphy_errors_0中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_dphy_errors_1   : 1;  /* bit[17]   : dphy_errors_1中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_dphy_errors_2   : 1;  /* bit[18]   : dphy_errors_2中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_dphy_errors_3   : 1;  /* bit[19]   : dphy_errors_3中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  mask_dphy_errors_4   : 1;  /* bit[20]   : dphy_errors_4中断屏蔽。
                                                                0：不屏蔽；
                                                                1：屏蔽。 */
        unsigned int  reserved             : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_DSI_INT_MSK0_UNION;
#endif
#define SOC_DSI_INT_MSK0_mask_ack_with_err_0_START   (0)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_0_END     (0)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_1_START   (1)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_1_END     (1)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_2_START   (2)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_2_END     (2)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_3_START   (3)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_3_END     (3)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_4_START   (4)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_4_END     (4)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_5_START   (5)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_5_END     (5)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_6_START   (6)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_6_END     (6)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_7_START   (7)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_7_END     (7)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_8_START   (8)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_8_END     (8)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_9_START   (9)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_9_END     (9)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_10_START  (10)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_10_END    (10)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_11_START  (11)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_11_END    (11)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_12_START  (12)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_12_END    (12)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_13_START  (13)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_13_END    (13)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_14_START  (14)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_14_END    (14)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_15_START  (15)
#define SOC_DSI_INT_MSK0_mask_ack_with_err_15_END    (15)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_0_START    (16)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_0_END      (16)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_1_START    (17)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_1_END      (17)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_2_START    (18)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_2_END      (18)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_3_START    (19)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_3_END      (19)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_4_START    (20)
#define SOC_DSI_INT_MSK0_mask_dphy_errors_4_END      (20)


/*****************************************************************************
 结构名    : SOC_DSI_INT_MSK1_UNION
 结构说明  : INT_MSK1 寄存器结构定义。地址偏移量:0x00C8，初值:0x00001FFF，宽度:32
 寄存器说明: 中断屏蔽寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mask_to_hs_tx         : 1;  /* bit[0]    : to_hs_tx中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_to_lp_rx         : 1;  /* bit[1]    : to_lp_rx中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_ecc_single_err   : 1;  /* bit[2]    : ecc_single_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_ecc_multi_err    : 1;  /* bit[3]    : ecc_multi_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_crc_err          : 1;  /* bit[4]    : crc_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_pkt_size_err     : 1;  /* bit[5]    : pkt_size_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_eopt_err         : 1;  /* bit[6]    : eopt_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_dpi_pld_wr_err   : 1;  /* bit[7]    : dpi_pld_wr_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_gen_cmd_wr_err   : 1;  /* bit[8]    : gen_cmd_wr_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_gen_pld_wr_err   : 1;  /* bit[9]    : gen_pld_wr_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_gen_pld_send_err : 1;  /* bit[10]   : gen_pld_send_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_gen_pld_rd_err   : 1;  /* bit[11]   : gen_pld_rd_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  mask_gen_pld_recv_err : 1;  /* bit[12]   : gen_pld_recv_err中断屏蔽。
                                                                 0：不屏蔽；
                                                                 1：屏蔽。 */
        unsigned int  reserved              : 19; /* bit[13-31]: 保留。 */
    } reg;
} SOC_DSI_INT_MSK1_UNION;
#endif
#define SOC_DSI_INT_MSK1_mask_to_hs_tx_START          (0)
#define SOC_DSI_INT_MSK1_mask_to_hs_tx_END            (0)
#define SOC_DSI_INT_MSK1_mask_to_lp_rx_START          (1)
#define SOC_DSI_INT_MSK1_mask_to_lp_rx_END            (1)
#define SOC_DSI_INT_MSK1_mask_ecc_single_err_START    (2)
#define SOC_DSI_INT_MSK1_mask_ecc_single_err_END      (2)
#define SOC_DSI_INT_MSK1_mask_ecc_multi_err_START     (3)
#define SOC_DSI_INT_MSK1_mask_ecc_multi_err_END       (3)
#define SOC_DSI_INT_MSK1_mask_crc_err_START           (4)
#define SOC_DSI_INT_MSK1_mask_crc_err_END             (4)
#define SOC_DSI_INT_MSK1_mask_pkt_size_err_START      (5)
#define SOC_DSI_INT_MSK1_mask_pkt_size_err_END        (5)
#define SOC_DSI_INT_MSK1_mask_eopt_err_START          (6)
#define SOC_DSI_INT_MSK1_mask_eopt_err_END            (6)
#define SOC_DSI_INT_MSK1_mask_dpi_pld_wr_err_START    (7)
#define SOC_DSI_INT_MSK1_mask_dpi_pld_wr_err_END      (7)
#define SOC_DSI_INT_MSK1_mask_gen_cmd_wr_err_START    (8)
#define SOC_DSI_INT_MSK1_mask_gen_cmd_wr_err_END      (8)
#define SOC_DSI_INT_MSK1_mask_gen_pld_wr_err_START    (9)
#define SOC_DSI_INT_MSK1_mask_gen_pld_wr_err_END      (9)
#define SOC_DSI_INT_MSK1_mask_gen_pld_send_err_START  (10)
#define SOC_DSI_INT_MSK1_mask_gen_pld_send_err_END    (10)
#define SOC_DSI_INT_MSK1_mask_gen_pld_rd_err_START    (11)
#define SOC_DSI_INT_MSK1_mask_gen_pld_rd_err_END      (11)
#define SOC_DSI_INT_MSK1_mask_gen_pld_recv_err_START  (12)
#define SOC_DSI_INT_MSK1_mask_gen_pld_recv_err_END    (12)






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

#endif /* end of soc_dsi_interface.h */
