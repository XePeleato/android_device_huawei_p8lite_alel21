

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_ADE_INTERFACE_H__
#define __SOC_ADE_INTERFACE_H__

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
/* 寄存器说明：ADE版本指示寄存器。
   位域定义UNION结构:  SOC_ADE_VERSION_UNION */
#define SOC_ADE_VERSION_ADDR(base)                    ((base) + (0x0000))

/* 寄存器说明：ADE全局控制寄存器0。
   位域定义UNION结构:  SOC_ADE_CTRL_UNION */
#define SOC_ADE_CTRL_ADDR(base)                       ((base) + (0x0004))

/* 寄存器说明：SCL3复用控制寄存器。
   位域定义UNION结构:  SOC_ADE_SCL3_MUX_CFG_UNION */
#define SOC_ADE_SCL3_MUX_CFG_ADDR(base)               ((base) + (0x0008))

/* 寄存器说明：SCL1复用控制寄存器。
   位域定义UNION结构:  SOC_ADE_SCL1_MUX_CFG_UNION */
#define SOC_ADE_SCL1_MUX_CFG_ADDR(base)               ((base) + (0x000C))

/* 寄存器说明：ROT数据源选择寄存器。
   位域定义UNION结构:  SOC_ADE_ROT_SRC_CFG_UNION */
#define SOC_ADE_ROT_SRC_CFG_ADDR(base)                ((base) + (0x0010))

/* 寄存器说明：SCL2数据源选择寄存器。
   位域定义UNION结构:  SOC_ADE_SCL2_SRC_CFG_UNION */
#define SOC_ADE_SCL2_SRC_CFG_ADDR(base)               ((base) + (0x0014))

/* 寄存器说明：在线显示通道数据源选择寄存器。
   位域定义UNION结构:  SOC_ADE_DISP_SRC_CFG_UNION */
#define SOC_ADE_DISP_SRC_CFG_ADDR(base)               ((base) + (0x0018))

/* 寄存器说明：在线显示回写通道数据源选择寄存器。
   位域定义UNION结构:  SOC_ADE_WDMA2_SRC_CFG_UNION */
#define SOC_ADE_WDMA2_SRC_CFG_ADDR(base)              ((base) + (0x001C))

/* 寄存器说明：二次叠加通道数据源选择寄存器。
   位域定义UNION结构:  SOC_ADE_SEC_OVLY_SRC_CFG_UNION */
#define SOC_ADE_SEC_OVLY_SRC_CFG_ADDR(base)           ((base) + (0x0020))

/* 寄存器说明：Wifi显示回写通道数据源选择寄存器。
   位域定义UNION结构:  SOC_ADE_WDMA3_SRC_CFG_UNION */
#define SOC_ADE_WDMA3_SRC_CFG_ADDR(base)              ((base) + (0x0024))

/* 寄存器说明：OVLY1下级通道数据传输使能配置寄存器。
   位域定义UNION结构:  SOC_ADE_OVLY1_TRANS_CFG_UNION */
#define SOC_ADE_OVLY1_TRANS_CFG_ADDR(base)            ((base) + (0x002C))

/* 寄存器说明：CTRAN5下级通道数据传输使能配置寄存器。
   位域定义UNION结构:  SOC_ADE_CTRAN5_TRANS_CFG_UNION */
#define SOC_ADE_CTRAN5_TRANS_CFG_ADDR(base)           ((base) + (0x0040))

/* 寄存器说明：DMA通道AXI分配控制寄存器。
   位域定义UNION结构:  SOC_ADE_DMA_AXI_MUX_UNION */
#define SOC_ADE_DMA_AXI_MUX_ADDR(base)                ((base) + (0x0050))

/* 寄存器说明：DMA通道AXI安全配置寄存器。此寄存器只能通过安全方式配置。
   位域定义UNION结构:  SOC_ADE_DMA_SEC_CFG_UNION */
#define SOC_ADE_DMA_SEC_CFG_ADDR(base)                ((base) + (0x0054))

/* 寄存器说明：DMA AXI接口Outstanding配置寄存器。
   位域定义UNION结构:  SOC_ADE_DMA_AXI_OSD_UNION */
#define SOC_ADE_DMA_AXI_OSD_ADDR(base)                ((base) + (0x0058))

/* 寄存器说明：子模块软复位寄存器。
   位域定义UNION结构:  SOC_ADE_SOFT_RST0_UNION */
#define SOC_ADE_SOFT_RST0_ADDR(base)                  ((base) + (0x0070))

/* 寄存器说明：子模块软复位寄存器。
   位域定义UNION结构:  SOC_ADE_SOFT_RST1_UNION */
#define SOC_ADE_SOFT_RST1_ADDR(base)                  ((base) + (0x0074))

/* 寄存器说明：子模块软复位源选择寄存器。
   位域定义UNION结构:  SOC_ADE_SOFT_RST_SEL0_UNION */
#define SOC_ADE_SOFT_RST_SEL0_ADDR(base)              ((base) + (0x0078))

/* 寄存器说明：子模块软复位源选择寄存器。
   位域定义UNION结构:  SOC_ADE_SOFT_RST_SEL1_UNION */
#define SOC_ADE_SOFT_RST_SEL1_ADDR(base)              ((base) + (0x007C))

/* 寄存器说明：子模块工作状态寄存器。
   位域定义UNION结构:  SOC_ADE_STAT0_UNION */
#define SOC_ADE_STAT0_ADDR(base)                      ((base) + (0x0080))

/* 寄存器说明：子模块工作状态寄存器。
   位域定义UNION结构:  SOC_ADE_STAT1_UNION */
#define SOC_ADE_STAT1_ADDR(base)                      ((base) + (0x0084))

/* 寄存器说明：DMA错误状态寄存器。
   位域定义UNION结构:  SOC_ADE_DMA_ERR_STAT_UNION */
#define SOC_ADE_DMA_ERR_STAT_ADDR(base)               ((base) + (0x0088))

/* 寄存器说明：ADE全局控制寄存器1。
   位域定义UNION结构:  SOC_ADE_CTRL1_UNION */
#define SOC_ADE_CTRL1_ADDR(base)                      ((base) + (0x008C))

/* 寄存器说明：ADE安全配置寄存器0。此寄存器用于控制子模块的寄存器是否安全可配，寄存器本身只能通过安全方式配置。
   位域定义UNION结构:  SOC_ADE_SEC_CFG0_UNION */
#define SOC_ADE_SEC_CFG0_ADDR(base)                   ((base) + (0x0090))

/* 寄存器说明：ADE安全配置寄存器1。此寄存器用于控制子模块的寄存器是否安全可配，寄存器本身只能通过安全方式配置。
   位域定义UNION结构:  SOC_ADE_SEC_CFG1_UNION */
#define SOC_ADE_SEC_CFG1_ADDR(base)                   ((base) + (0x0094))

/* 寄存器说明：OVLY通道控制寄存器。属于静态配置寄存器，禁止在ADE工作过程中动态配置。
   位域定义UNION结构:  SOC_ADE_OVLY_CTL_UNION */
#define SOC_ADE_OVLY_CTL_ADDR(base)                   ((base) + (0x0098))

/* 寄存器说明：ADE子模块自动时钟门控控制寄存器0。
   位域定义UNION结构:  SOC_ADE_AUTO_CLK_GT_EN0_UNION */
#define SOC_ADE_AUTO_CLK_GT_EN0_ADDR(base)            ((base) + (0x009C))

/* 寄存器说明：ADE子模块自动时钟门控控制寄存器1。
   位域定义UNION结构:  SOC_ADE_AUTO_CLK_GT_EN1_UNION */
#define SOC_ADE_AUTO_CLK_GT_EN1_ADDR(base)            ((base) + (0x00A0))

/* 寄存器说明：ADE丢帧控制寄存器。
   位域定义UNION结构:  SOC_ADE_FRM_DISGARD_CTRL_UNION */
#define SOC_ADE_FRM_DISGARD_CTRL_ADDR(base)           ((base) + (0x00A4))

/* 寄存器说明：ADE丢帧统计计数器。
   位域定义UNION结构:  SOC_ADE_FRM_DISGARD_CNT_UNION */
#define SOC_ADE_FRM_DISGARD_CNT_ADDR(base)            ((base) + (0x00A8))

/* 寄存器说明：子模块硬件同步屏蔽寄存器0。
   位域定义UNION结构:  SOC_ADE_RELOAD_DIS0_UNION */
#define SOC_ADE_RELOAD_DIS0_ADDR(base)                ((base) + (0x00AC))

/* 寄存器说明：子模块硬件同步屏蔽寄存器1。
   位域定义UNION结构:  SOC_ADE_RELOAD_DIS1_UNION */
#define SOC_ADE_RELOAD_DIS1_ADDR(base)                ((base) + (0x00B0))

/* 寄存器说明：ADE全局使能寄存器。
   位域定义UNION结构:  SOC_ADE_EN_UNION */
#define SOC_ADE_EN_ADDR(base)                         ((base) + (0x0100))





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
 结构名    : SOC_ADE_VERSION_UNION
 结构说明  : VERSION 寄存器结构定义。地址偏移量:0x0000，初值:0x00000100，宽度:32
 寄存器说明: ADE版本指示寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  version : 32; /* bit[0-31]: 指示ADE的版本。 */
    } reg;
} SOC_ADE_VERSION_UNION;
#endif
#define SOC_ADE_VERSION_version_START  (0)
#define SOC_ADE_VERSION_version_END    (31)


/*****************************************************************************
 结构名    : SOC_ADE_CTRL_UNION
 结构说明  : CTRL 寄存器结构定义。地址偏移量:0x0004，初值:0x4005000D，宽度:32
 寄存器说明: ADE全局控制寄存器0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  frm_end_start       : 2;  /* bit[0-1]  : ADE帧开始时刻选择，表示ADE的资源分配、通路配置等寄存器刷新的时刻：
                                                               0：每一帧Vsync同步信号时生效；
                                                               1：第一帧配置完ade_en后生效，后续帧帧结束后生效（推荐）；
                                                               2：配置ade_en寄存器时马上生效；
                                                               3：第一帧Vsync同步信号生效，后续帧帧结束后生效。 */
        unsigned int  dfs_buf_cfg         : 1;  /* bit[2]    : DFS buffer分配控制，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                               0：DFS buffer深度为6400，此时SCL2的垂直缩放功能不可用；
                                                               1：DFS buffer深度为3840，此时SCL2的垂直缩放功能可用；
                                                               注意：DFS buffer下溢水线的配置不能超过分配的buffer深度。 */
        unsigned int  rot_buf_cfg         : 3;  /* bit[3-5]  : 通道ROT buffer分配控制，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                               0：CH1=NA;CH5=NA;CH6=16KB;ROT=153.6KB；
                                                               1：CH1=16KB;CH5=NA;CH6=153.6KB;ROT=NA；
                                                               2：CH1=NA;CH5=153.6KB;CH6=16KB;ROT=NA；
                                                               3：CH1=16KB;CH5=76.8KB;CH6=76.8KB;ROT=NA；
                                                               4：CH1=16KB;CH5=NA;CH6=76.8KB;ROT=76.8KB；
                                                               5：保留。
                                                               注意：
                                                               1、ROT未分配buffer时，必须disable；
                                                               2、Buffer大小为乒乓buffer的总和，配置的RDMA的图像块大小不能超过单个乒buffer的大小； */
        unsigned int  rd_ch5_nv           : 1;  /* bit[6]    : ROT buffer分配控制附加属性，需要和rot_buf_cfg配合使用，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                               0：Channel 5当前不是NV12/NV21格式；
                                                               1：Channel 5当前是NV12/NV21格式； */
        unsigned int  rd_ch6_nv           : 1;  /* bit[7]    : ROT buffer分配控制附加属性，需要和rot_buf_cfg配合使用，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                               0：Channel 6当前不是NV12/NV21格式；
                                                               1：Channel 6当前是NV12/NV21格式； */
        unsigned int  dfs_buf_unflow_lev1 : 13; /* bit[8-20] : DFS buffer下溢预警水线，默认值为1280，最高不超过分配的DFS buffer大小。当DFS buffer的填充深度低于此水线时，上报underflow中断，并调节RDMA的QOS。 */
        unsigned int  dfs_buf_unflow_lev2 : 11; /* bit[21-31]: DFS buffer下溢预警水线增量，默认值为512，下溢预警水线+增量最高不超过分配的DFS buffer大小。当DFS buffer的填充深度低于水线+增量时，调节RDMA的QOS。 */
    } reg;
} SOC_ADE_CTRL_UNION;
#endif
#define SOC_ADE_CTRL_frm_end_start_START        (0)
#define SOC_ADE_CTRL_frm_end_start_END          (1)
#define SOC_ADE_CTRL_dfs_buf_cfg_START          (2)
#define SOC_ADE_CTRL_dfs_buf_cfg_END            (2)
#define SOC_ADE_CTRL_rot_buf_cfg_START          (3)
#define SOC_ADE_CTRL_rot_buf_cfg_END            (5)
#define SOC_ADE_CTRL_rd_ch5_nv_START            (6)
#define SOC_ADE_CTRL_rd_ch5_nv_END              (6)
#define SOC_ADE_CTRL_rd_ch6_nv_START            (7)
#define SOC_ADE_CTRL_rd_ch6_nv_END              (7)
#define SOC_ADE_CTRL_dfs_buf_unflow_lev1_START  (8)
#define SOC_ADE_CTRL_dfs_buf_unflow_lev1_END    (20)
#define SOC_ADE_CTRL_dfs_buf_unflow_lev2_START  (21)
#define SOC_ADE_CTRL_dfs_buf_unflow_lev2_END    (31)


/*****************************************************************************
 结构名    : SOC_ADE_SCL3_MUX_CFG_UNION
 结构说明  : SCL3_MUX_CFG 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: SCL3复用控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  scl3_mux_cfg         : 1;  /* bit[0]   : SCL3复用控制，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                               0：SCL3位于CH5之后；
                                                               1：SCL3位于Wifi显示回写通道（WDMA3）之前。 */
        unsigned int  clip6_wdma3_trans_en : 1;  /* bit[1]   : 控制CH6到WDMA3之间的直通通路，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                               0：禁用通路；
                                                               1：开启通路，此时SCL3不能复用在WDMA3前（scl3_mux_cfg必须=0），CLIP6到SCL1和CTRAN2之间的通路也自动断开。 */
        unsigned int  reserved             : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_ADE_SCL3_MUX_CFG_UNION;
#endif
#define SOC_ADE_SCL3_MUX_CFG_scl3_mux_cfg_START          (0)
#define SOC_ADE_SCL3_MUX_CFG_scl3_mux_cfg_END            (0)
#define SOC_ADE_SCL3_MUX_CFG_clip6_wdma3_trans_en_START  (1)
#define SOC_ADE_SCL3_MUX_CFG_clip6_wdma3_trans_en_END    (1)


/*****************************************************************************
 结构名    : SOC_ADE_SCL1_MUX_CFG_UNION
 结构说明  : SCL1_MUX_CFG 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: SCL1复用控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  scl1_mux_cfg : 2;  /* bit[0-1] : SCL1复用控制，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                       0：SCL1位于CH6之后；
                                                       1：SCL1位于CH1之后；
                                                       2：SCL1位于CH5之后；
                                                       3：未定义。 */
        unsigned int  reserved     : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_ADE_SCL1_MUX_CFG_UNION;
#endif
#define SOC_ADE_SCL1_MUX_CFG_scl1_mux_cfg_START  (0)
#define SOC_ADE_SCL1_MUX_CFG_scl1_mux_cfg_END    (1)


/*****************************************************************************
 结构名    : SOC_ADE_ROT_SRC_CFG_UNION
 结构说明  : ROT_SRC_CFG 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: ROT数据源选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rot_src_cfg : 2;  /* bit[0-1] : Ovly后的ROT数据源选择，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                      0：断开；
                                                      1：Ovly2；
                                                      2：Ovly3；
                                                      3：自刷新回读通道（DISP_RDMA）。 */
        unsigned int  reserved    : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_ADE_ROT_SRC_CFG_UNION;
#endif
#define SOC_ADE_ROT_SRC_CFG_rot_src_cfg_START  (0)
#define SOC_ADE_ROT_SRC_CFG_rot_src_cfg_END    (1)


/*****************************************************************************
 结构名    : SOC_ADE_SCL2_SRC_CFG_UNION
 结构说明  : SCL2_SRC_CFG 寄存器结构定义。地址偏移量:0x0014，初值:0x00000000，宽度:32
 寄存器说明: SCL2数据源选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  scl2_src_cfg : 3;  /* bit[0-2] : Ovly后SCL2的数据源选择，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                       0：断开；
                                                       1：Ovly2；
                                                       2：Ovly3；
                                                       3：自刷新回读通道（DISP_RDMA）；
                                                       4：Ovly后的ROT；
                                                       5~7：断开。 */
        unsigned int  reserved     : 29; /* bit[3-31]: 保留。 */
    } reg;
} SOC_ADE_SCL2_SRC_CFG_UNION;
#endif
#define SOC_ADE_SCL2_SRC_CFG_scl2_src_cfg_START  (0)
#define SOC_ADE_SCL2_SRC_CFG_scl2_src_cfg_END    (2)


/*****************************************************************************
 结构名    : SOC_ADE_DISP_SRC_CFG_UNION
 结构说明  : DISP_SRC_CFG 寄存器结构定义。地址偏移量:0x0018，初值:0x00000001，宽度:32
 寄存器说明: 在线显示通道数据源选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  disp_src_cfg : 3;  /* bit[0-2] : 在线显示通道的数据源选择，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                       0：断开；
                                                       1：Ovly2；
                                                       2：自刷新回读通道（DISP_RDMA）；
                                                       3：Ovly后的ROT；
                                                       4：Ovly后的SCL2；
                                                       5~7：断开。 */
        unsigned int  reserved     : 29; /* bit[3-31]: 保留。 */
    } reg;
} SOC_ADE_DISP_SRC_CFG_UNION;
#endif
#define SOC_ADE_DISP_SRC_CFG_disp_src_cfg_START  (0)
#define SOC_ADE_DISP_SRC_CFG_disp_src_cfg_END    (2)


/*****************************************************************************
 结构名    : SOC_ADE_WDMA2_SRC_CFG_UNION
 结构说明  : WDMA2_SRC_CFG 寄存器结构定义。地址偏移量:0x001C，初值:0x00000000，宽度:32
 寄存器说明: 在线显示回写通道数据源选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdma2_src_cfg : 3;  /* bit[0-2] : 在线显示回写通道（WDMA2）的数据源选择，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                        0：断开；
                                                        1：自刷新回读通道（DISP_RDMA）；
                                                        2：Ovly后的ROT；
                                                        3：Ovly后的SCL2；
                                                        4：Ovly2；
                                                        5：Ovly3；
                                                        6~7：断开。 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留。 */
    } reg;
} SOC_ADE_WDMA2_SRC_CFG_UNION;
#endif
#define SOC_ADE_WDMA2_SRC_CFG_wdma2_src_cfg_START  (0)
#define SOC_ADE_WDMA2_SRC_CFG_wdma2_src_cfg_END    (2)


/*****************************************************************************
 结构名    : SOC_ADE_SEC_OVLY_SRC_CFG_UNION
 结构说明  : SEC_OVLY_SRC_CFG 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: 二次叠加通道数据源选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sec_ovly_src_cfg : 2;  /* bit[0-1] : 二次叠加通道的数据源选择，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                           0：断开；
                                                           1：Ovly3；
                                                           2：Ovly后的ROT；
                                                           3：Ovly后的SCL2。 */
        unsigned int  reserved         : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_ADE_SEC_OVLY_SRC_CFG_UNION;
#endif
#define SOC_ADE_SEC_OVLY_SRC_CFG_sec_ovly_src_cfg_START  (0)
#define SOC_ADE_SEC_OVLY_SRC_CFG_sec_ovly_src_cfg_END    (1)


/*****************************************************************************
 结构名    : SOC_ADE_WDMA3_SRC_CFG_UNION
 结构说明  : WDMA3_SRC_CFG 寄存器结构定义。地址偏移量:0x0024，初值:0x00000000，宽度:32
 寄存器说明: Wifi显示回写通道数据源选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdma3_src_cfg : 3;  /* bit[0-2] : Wifi显示通道（WDMA3）的数据源选择，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                        0：断开；
                                                        1：自刷新回读通道（DISP_RDMA）；
                                                        2：Ovly后的ROT；
                                                        3：Ovly后的SCL2；
                                                        4：Ovly1；
                                                        5：Ovly2；
                                                        6~7：断开。 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留。 */
    } reg;
} SOC_ADE_WDMA3_SRC_CFG_UNION;
#endif
#define SOC_ADE_WDMA3_SRC_CFG_wdma3_src_cfg_START  (0)
#define SOC_ADE_WDMA3_SRC_CFG_wdma3_src_cfg_END    (2)


/*****************************************************************************
 结构名    : SOC_ADE_OVLY1_TRANS_CFG_UNION
 结构说明  : OVLY1_TRANS_CFG 寄存器结构定义。地址偏移量:0x002C，初值:0x00000001，宽度:32
 寄存器说明: OVLY1下级通道数据传输使能配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ovly1_wdma1_trans_en : 1;  /* bit[0]   : OVLY1向离线回写通道（WDMA1）数据传输使能，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                               0：传输关闭；
                                                               1：传输使能； */
        unsigned int  reserved             : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ADE_OVLY1_TRANS_CFG_UNION;
#endif
#define SOC_ADE_OVLY1_TRANS_CFG_ovly1_wdma1_trans_en_START  (0)
#define SOC_ADE_OVLY1_TRANS_CFG_ovly1_wdma1_trans_en_END    (0)


/*****************************************************************************
 结构名    : SOC_ADE_CTRAN5_TRANS_CFG_UNION
 结构说明  : CTRAN5_TRANS_CFG 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: CTRAN5下级通道数据传输使能配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ctran5_ovly_trans_en : 1;  /* bit[0]   : CTRAN5向OVLY（OVLY1/OVLY2）数据传输使能，属于静态配置寄存器，禁止在ADE工作过程中动态配置。
                                                               0：传输关闭；
                                                               1：传输使能； */
        unsigned int  reserved             : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ADE_CTRAN5_TRANS_CFG_UNION;
#endif
#define SOC_ADE_CTRAN5_TRANS_CFG_ctran5_ovly_trans_en_START  (0)
#define SOC_ADE_CTRAN5_TRANS_CFG_ctran5_ovly_trans_en_END    (0)


/*****************************************************************************
 结构名    : SOC_ADE_DMA_AXI_MUX_UNION
 结构说明  : DMA_AXI_MUX 寄存器结构定义。地址偏移量:0x0050，初值:0x00007780，宽度:32
 寄存器说明: DMA通道AXI分配控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_dma_ch1_axi_cfg   : 1;  /* bit[0]    : dma读通道1AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  rd_dma_ch2_axi_cfg   : 1;  /* bit[1]    : dma读通道2AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  rd_dma_ch3_axi_cfg   : 1;  /* bit[2]    : dma读通道3AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  rd_dma_ch4_axi_cfg   : 1;  /* bit[3]    : dma读通道4AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  rd_dma_ch5_axi_cfg   : 1;  /* bit[4]    : dma读通道5AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  rd_dma_ch6_axi_cfg   : 1;  /* bit[5]    : dma读通道6AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  rd_dma_disp_axi_cfg  : 1;  /* bit[6]    : dma display读通道AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  rd_dma_cmdq1_axi_cfg : 1;  /* bit[7]    : dma CMDQ1读通道AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  rd_dma_cmdq2_axi_cfg : 1;  /* bit[8]    : dma CMDQ2读通道AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  reserved_0           : 1;  /* bit[9]    : 保留。 */
        unsigned int  wr_dma_ch1_axi_cfg   : 1;  /* bit[10]   : dma 写通道1AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  wr_dma_ch2_axi_cfg   : 1;  /* bit[11]   : dma 写通道2AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  wr_dma_ch3_axi_cfg   : 1;  /* bit[12]   : dma 写通道3AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  reserved_1           : 1;  /* bit[13]   : 保留。 */
        unsigned int  wr_dma_cmdq_axi_cfg  : 1;  /* bit[14]   : dma CMDQ写通道AXI接口配置：
                                                                0：使用AXI0接口；
                                                                1：使用AXI1接口。 */
        unsigned int  reserved_2           : 17; /* bit[15-31]: 保留。 */
    } reg;
} SOC_ADE_DMA_AXI_MUX_UNION;
#endif
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch1_axi_cfg_START    (0)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch1_axi_cfg_END      (0)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch2_axi_cfg_START    (1)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch2_axi_cfg_END      (1)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch3_axi_cfg_START    (2)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch3_axi_cfg_END      (2)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch4_axi_cfg_START    (3)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch4_axi_cfg_END      (3)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch5_axi_cfg_START    (4)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch5_axi_cfg_END      (4)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch6_axi_cfg_START    (5)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_ch6_axi_cfg_END      (5)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_disp_axi_cfg_START   (6)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_disp_axi_cfg_END     (6)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_cmdq1_axi_cfg_START  (7)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_cmdq1_axi_cfg_END    (7)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_cmdq2_axi_cfg_START  (8)
#define SOC_ADE_DMA_AXI_MUX_rd_dma_cmdq2_axi_cfg_END    (8)
#define SOC_ADE_DMA_AXI_MUX_wr_dma_ch1_axi_cfg_START    (10)
#define SOC_ADE_DMA_AXI_MUX_wr_dma_ch1_axi_cfg_END      (10)
#define SOC_ADE_DMA_AXI_MUX_wr_dma_ch2_axi_cfg_START    (11)
#define SOC_ADE_DMA_AXI_MUX_wr_dma_ch2_axi_cfg_END      (11)
#define SOC_ADE_DMA_AXI_MUX_wr_dma_ch3_axi_cfg_START    (12)
#define SOC_ADE_DMA_AXI_MUX_wr_dma_ch3_axi_cfg_END      (12)
#define SOC_ADE_DMA_AXI_MUX_wr_dma_cmdq_axi_cfg_START   (14)
#define SOC_ADE_DMA_AXI_MUX_wr_dma_cmdq_axi_cfg_END     (14)


/*****************************************************************************
 结构名    : SOC_ADE_DMA_SEC_CFG_UNION
 结构说明  : DMA_SEC_CFG 寄存器结构定义。地址偏移量:0x0054，初值:0x00007FFF，宽度:32
 寄存器说明: DMA通道AXI安全配置寄存器。此寄存器只能通过安全方式配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_dma_ch1_sec   : 1;  /* bit[0]    : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  rd_dma_ch2_sec   : 1;  /* bit[1]    : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  rd_dma_ch3_sec   : 1;  /* bit[2]    : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  rd_dma_ch4_sec   : 1;  /* bit[3]    : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  rd_dma_ch5_sec   : 1;  /* bit[4]    : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  rd_dma_ch6_sec   : 1;  /* bit[5]    : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  rd_dma_disp_sec  : 1;  /* bit[6]    : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  rd_dma_cmdq1_sec : 1;  /* bit[7]    : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  rd_dma_cmdq2_sec : 1;  /* bit[8]    : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  reserved_0       : 1;  /* bit[9]    : 保留。 */
        unsigned int  wr_dma_ch1_sec   : 1;  /* bit[10]   : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  wr_dma_ch2_sec   : 1;  /* bit[11]   : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  wr_dma_ch3_sec   : 1;  /* bit[12]   : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  reserved_1       : 1;  /* bit[13]   : 保留。 */
        unsigned int  wr_dma_cmdq_sec  : 1;  /* bit[14]   : 通道安全配置，控制DMA发出的AX操作的PROT[1]属性：
                                                            0：安全通道；
                                                            1：非安全通道。 */
        unsigned int  reserved_2       : 17; /* bit[15-31]: 保留。 */
    } reg;
} SOC_ADE_DMA_SEC_CFG_UNION;
#endif
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch1_sec_START    (0)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch1_sec_END      (0)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch2_sec_START    (1)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch2_sec_END      (1)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch3_sec_START    (2)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch3_sec_END      (2)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch4_sec_START    (3)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch4_sec_END      (3)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch5_sec_START    (4)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch5_sec_END      (4)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch6_sec_START    (5)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_ch6_sec_END      (5)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_disp_sec_START   (6)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_disp_sec_END     (6)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_cmdq1_sec_START  (7)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_cmdq1_sec_END    (7)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_cmdq2_sec_START  (8)
#define SOC_ADE_DMA_SEC_CFG_rd_dma_cmdq2_sec_END    (8)
#define SOC_ADE_DMA_SEC_CFG_wr_dma_ch1_sec_START    (10)
#define SOC_ADE_DMA_SEC_CFG_wr_dma_ch1_sec_END      (10)
#define SOC_ADE_DMA_SEC_CFG_wr_dma_ch2_sec_START    (11)
#define SOC_ADE_DMA_SEC_CFG_wr_dma_ch2_sec_END      (11)
#define SOC_ADE_DMA_SEC_CFG_wr_dma_ch3_sec_START    (12)
#define SOC_ADE_DMA_SEC_CFG_wr_dma_ch3_sec_END      (12)
#define SOC_ADE_DMA_SEC_CFG_wr_dma_cmdq_sec_START   (14)
#define SOC_ADE_DMA_SEC_CFG_wr_dma_cmdq_sec_END     (14)


/*****************************************************************************
 结构名    : SOC_ADE_DMA_AXI_OSD_UNION
 结构说明  : DMA_AXI_OSD 寄存器结构定义。地址偏移量:0x0058，初值:0x00BB00BB，宽度:32
 寄存器说明: DMA AXI接口Outstanding配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ade_axi0_rd_os : 4;  /* bit[0-3]  : AXI0接口读Outstanding总深度，最大值为11，不能配置为0 */
        unsigned int  ade_axi0_wr_os : 4;  /* bit[4-7]  : AXI0接口写Outstanding总深度，最大值为11，不能配置为0 */
        unsigned int  reserved_0     : 8;  /* bit[8-15] : 保留。 */
        unsigned int  ade_axi1_rd_os : 4;  /* bit[16-19]: AXI1接口读Outstanding总深度，最大值为11，不能配置为0 */
        unsigned int  ade_axi1_wr_os : 4;  /* bit[20-23]: AXI1接口写Outstanding总深度，最大值为11，不能配置为0 */
        unsigned int  reserved_1     : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_ADE_DMA_AXI_OSD_UNION;
#endif
#define SOC_ADE_DMA_AXI_OSD_ade_axi0_rd_os_START  (0)
#define SOC_ADE_DMA_AXI_OSD_ade_axi0_rd_os_END    (3)
#define SOC_ADE_DMA_AXI_OSD_ade_axi0_wr_os_START  (4)
#define SOC_ADE_DMA_AXI_OSD_ade_axi0_wr_os_END    (7)
#define SOC_ADE_DMA_AXI_OSD_ade_axi1_rd_os_START  (16)
#define SOC_ADE_DMA_AXI_OSD_ade_axi1_rd_os_END    (19)
#define SOC_ADE_DMA_AXI_OSD_ade_axi1_wr_os_START  (20)
#define SOC_ADE_DMA_AXI_OSD_ade_axi1_wr_os_END    (23)


/*****************************************************************************
 结构名    : SOC_ADE_SOFT_RST0_UNION
 结构说明  : SOFT_RST0 寄存器结构定义。地址偏移量:0x0070，初值:0x00000000，宽度:32
 寄存器说明: 子模块软复位寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_rdma_srst   : 1;  /* bit[0] : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ch2_rdma_srst   : 1;  /* bit[1] : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ch3_rdma_srst   : 1;  /* bit[2] : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ch4_rdma_srst   : 1;  /* bit[3] : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ch5_rdma_srst   : 1;  /* bit[4] : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ch6_rdma_srst   : 1;  /* bit[5] : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  disp_rdma_srst  : 1;  /* bit[6] : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  cmdq1_rdma_srst : 1;  /* bit[7] : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  cmdq2_rdma_srst : 1;  /* bit[8] : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  reserved_0      : 1;  /* bit[9] : 保留。 */
        unsigned int  ch1_wdma_srst   : 1;  /* bit[10]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ch2_wdma_srst   : 1;  /* bit[11]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ch3_wdma_srst   : 1;  /* bit[12]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  reserved_1      : 1;  /* bit[13]: 保留。 */
        unsigned int  cmdq_wdma_srst  : 1;  /* bit[14]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  clip1_srst      : 1;  /* bit[15]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  clip2_srst      : 1;  /* bit[16]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  clip3_srst      : 1;  /* bit[17]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  clip4_srst      : 1;  /* bit[18]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  clip5_srst      : 1;  /* bit[19]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  clip6_srst      : 1;  /* bit[20]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  scl1_srst       : 1;  /* bit[21]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  scl2_srst       : 1;  /* bit[22]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  scl3_srst       : 1;  /* bit[23]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ctran1_srst     : 1;  /* bit[24]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ctran2_srst     : 1;  /* bit[25]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ctran3_srst     : 1;  /* bit[26]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ctran4_srst     : 1;  /* bit[27]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ctran5_srst     : 1;  /* bit[28]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ctran6_srst     : 1;  /* bit[29]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  rot_srst        : 1;  /* bit[30]: 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  reserved_2      : 1;  /* bit[31]: 保留。 */
    } reg;
} SOC_ADE_SOFT_RST0_UNION;
#endif
#define SOC_ADE_SOFT_RST0_ch1_rdma_srst_START    (0)
#define SOC_ADE_SOFT_RST0_ch1_rdma_srst_END      (0)
#define SOC_ADE_SOFT_RST0_ch2_rdma_srst_START    (1)
#define SOC_ADE_SOFT_RST0_ch2_rdma_srst_END      (1)
#define SOC_ADE_SOFT_RST0_ch3_rdma_srst_START    (2)
#define SOC_ADE_SOFT_RST0_ch3_rdma_srst_END      (2)
#define SOC_ADE_SOFT_RST0_ch4_rdma_srst_START    (3)
#define SOC_ADE_SOFT_RST0_ch4_rdma_srst_END      (3)
#define SOC_ADE_SOFT_RST0_ch5_rdma_srst_START    (4)
#define SOC_ADE_SOFT_RST0_ch5_rdma_srst_END      (4)
#define SOC_ADE_SOFT_RST0_ch6_rdma_srst_START    (5)
#define SOC_ADE_SOFT_RST0_ch6_rdma_srst_END      (5)
#define SOC_ADE_SOFT_RST0_disp_rdma_srst_START   (6)
#define SOC_ADE_SOFT_RST0_disp_rdma_srst_END     (6)
#define SOC_ADE_SOFT_RST0_cmdq1_rdma_srst_START  (7)
#define SOC_ADE_SOFT_RST0_cmdq1_rdma_srst_END    (7)
#define SOC_ADE_SOFT_RST0_cmdq2_rdma_srst_START  (8)
#define SOC_ADE_SOFT_RST0_cmdq2_rdma_srst_END    (8)
#define SOC_ADE_SOFT_RST0_ch1_wdma_srst_START    (10)
#define SOC_ADE_SOFT_RST0_ch1_wdma_srst_END      (10)
#define SOC_ADE_SOFT_RST0_ch2_wdma_srst_START    (11)
#define SOC_ADE_SOFT_RST0_ch2_wdma_srst_END      (11)
#define SOC_ADE_SOFT_RST0_ch3_wdma_srst_START    (12)
#define SOC_ADE_SOFT_RST0_ch3_wdma_srst_END      (12)
#define SOC_ADE_SOFT_RST0_cmdq_wdma_srst_START   (14)
#define SOC_ADE_SOFT_RST0_cmdq_wdma_srst_END     (14)
#define SOC_ADE_SOFT_RST0_clip1_srst_START       (15)
#define SOC_ADE_SOFT_RST0_clip1_srst_END         (15)
#define SOC_ADE_SOFT_RST0_clip2_srst_START       (16)
#define SOC_ADE_SOFT_RST0_clip2_srst_END         (16)
#define SOC_ADE_SOFT_RST0_clip3_srst_START       (17)
#define SOC_ADE_SOFT_RST0_clip3_srst_END         (17)
#define SOC_ADE_SOFT_RST0_clip4_srst_START       (18)
#define SOC_ADE_SOFT_RST0_clip4_srst_END         (18)
#define SOC_ADE_SOFT_RST0_clip5_srst_START       (19)
#define SOC_ADE_SOFT_RST0_clip5_srst_END         (19)
#define SOC_ADE_SOFT_RST0_clip6_srst_START       (20)
#define SOC_ADE_SOFT_RST0_clip6_srst_END         (20)
#define SOC_ADE_SOFT_RST0_scl1_srst_START        (21)
#define SOC_ADE_SOFT_RST0_scl1_srst_END          (21)
#define SOC_ADE_SOFT_RST0_scl2_srst_START        (22)
#define SOC_ADE_SOFT_RST0_scl2_srst_END          (22)
#define SOC_ADE_SOFT_RST0_scl3_srst_START        (23)
#define SOC_ADE_SOFT_RST0_scl3_srst_END          (23)
#define SOC_ADE_SOFT_RST0_ctran1_srst_START      (24)
#define SOC_ADE_SOFT_RST0_ctran1_srst_END        (24)
#define SOC_ADE_SOFT_RST0_ctran2_srst_START      (25)
#define SOC_ADE_SOFT_RST0_ctran2_srst_END        (25)
#define SOC_ADE_SOFT_RST0_ctran3_srst_START      (26)
#define SOC_ADE_SOFT_RST0_ctran3_srst_END        (26)
#define SOC_ADE_SOFT_RST0_ctran4_srst_START      (27)
#define SOC_ADE_SOFT_RST0_ctran4_srst_END        (27)
#define SOC_ADE_SOFT_RST0_ctran5_srst_START      (28)
#define SOC_ADE_SOFT_RST0_ctran5_srst_END        (28)
#define SOC_ADE_SOFT_RST0_ctran6_srst_START      (29)
#define SOC_ADE_SOFT_RST0_ctran6_srst_END        (29)
#define SOC_ADE_SOFT_RST0_rot_srst_START         (30)
#define SOC_ADE_SOFT_RST0_rot_srst_END           (30)


/*****************************************************************************
 结构名    : SOC_ADE_SOFT_RST1_UNION
 结构说明  : SOFT_RST1 寄存器结构定义。地址偏移量:0x0074，初值:0x00000000，宽度:32
 寄存器说明: 子模块软复位寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cmdq1_srst  : 1;  /* bit[0]   : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  cmdq2_srst  : 1;  /* bit[1]   : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  reserved_0  : 1;  /* bit[2]   : 保留。 */
        unsigned int  gamma_srst  : 1;  /* bit[3]   : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  dither_srst : 1;  /* bit[4]   : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ovly1_srst  : 1;  /* bit[5]   : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ovly2_srst  : 1;  /* bit[6]   : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  ovly3_srst  : 1;  /* bit[7]   : 子模块软复位，写0无效，写1产生软复位，且自动清零。 */
        unsigned int  reserved_1  : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_ADE_SOFT_RST1_UNION;
#endif
#define SOC_ADE_SOFT_RST1_cmdq1_srst_START   (0)
#define SOC_ADE_SOFT_RST1_cmdq1_srst_END     (0)
#define SOC_ADE_SOFT_RST1_cmdq2_srst_START   (1)
#define SOC_ADE_SOFT_RST1_cmdq2_srst_END     (1)
#define SOC_ADE_SOFT_RST1_gamma_srst_START   (3)
#define SOC_ADE_SOFT_RST1_gamma_srst_END     (3)
#define SOC_ADE_SOFT_RST1_dither_srst_START  (4)
#define SOC_ADE_SOFT_RST1_dither_srst_END    (4)
#define SOC_ADE_SOFT_RST1_ovly1_srst_START   (5)
#define SOC_ADE_SOFT_RST1_ovly1_srst_END     (5)
#define SOC_ADE_SOFT_RST1_ovly2_srst_START   (6)
#define SOC_ADE_SOFT_RST1_ovly2_srst_END     (6)
#define SOC_ADE_SOFT_RST1_ovly3_srst_START   (7)
#define SOC_ADE_SOFT_RST1_ovly3_srst_END     (7)


/*****************************************************************************
 结构名    : SOC_ADE_SOFT_RST_SEL0_UNION
 结构说明  : SOFT_RST_SEL0 寄存器结构定义。地址偏移量:0x0078，初值:0x00000000，宽度:32
 寄存器说明: 子模块软复位源选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_rdma_srst_sel   : 1;  /* bit[0] : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ch2_rdma_srst_sel   : 1;  /* bit[1] : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ch3_rdma_srst_sel   : 1;  /* bit[2] : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ch4_rdma_srst_sel   : 1;  /* bit[3] : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ch5_rdma_srst_sel   : 1;  /* bit[4] : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ch6_rdma_srst_sel   : 1;  /* bit[5] : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  disp_rdma_srst_sel  : 1;  /* bit[6] : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  cmdq1_rdma_srst_sel : 1;  /* bit[7] : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  cmdq2_rdma_srst_sel : 1;  /* bit[8] : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  reserved_0          : 1;  /* bit[9] : 保留。 */
        unsigned int  ch1_wdma_srst_sel   : 1;  /* bit[10]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ch2_wdma_srst_sel   : 1;  /* bit[11]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ch3_wdma_srst_sel   : 1;  /* bit[12]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  reserved_1          : 1;  /* bit[13]: 保留。 */
        unsigned int  cmdq_wdma_srst_sel  : 1;  /* bit[14]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  clip1_srst_sel      : 1;  /* bit[15]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  clip2_srst_sel      : 1;  /* bit[16]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  clip3_srst_sel      : 1;  /* bit[17]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  clip4_srst_sel      : 1;  /* bit[18]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  clip5_srst_sel      : 1;  /* bit[19]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  clip6_srst_sel      : 1;  /* bit[20]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  scl1_srst_sel       : 1;  /* bit[21]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  scl2_srst_sel       : 1;  /* bit[22]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  scl3_srst_sel       : 1;  /* bit[23]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ctran1_srst_sel     : 1;  /* bit[24]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ctran2_srst_sel     : 1;  /* bit[25]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ctran3_srst_sel     : 1;  /* bit[26]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ctran4_srst_sel     : 1;  /* bit[27]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ctran5_srst_sel     : 1;  /* bit[28]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ctran6_srst_sel     : 1;  /* bit[29]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  rot_srst_sel        : 1;  /* bit[30]: 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                            0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                            1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  reserved_2          : 1;  /* bit[31]: 保留。 */
    } reg;
} SOC_ADE_SOFT_RST_SEL0_UNION;
#endif
#define SOC_ADE_SOFT_RST_SEL0_ch1_rdma_srst_sel_START    (0)
#define SOC_ADE_SOFT_RST_SEL0_ch1_rdma_srst_sel_END      (0)
#define SOC_ADE_SOFT_RST_SEL0_ch2_rdma_srst_sel_START    (1)
#define SOC_ADE_SOFT_RST_SEL0_ch2_rdma_srst_sel_END      (1)
#define SOC_ADE_SOFT_RST_SEL0_ch3_rdma_srst_sel_START    (2)
#define SOC_ADE_SOFT_RST_SEL0_ch3_rdma_srst_sel_END      (2)
#define SOC_ADE_SOFT_RST_SEL0_ch4_rdma_srst_sel_START    (3)
#define SOC_ADE_SOFT_RST_SEL0_ch4_rdma_srst_sel_END      (3)
#define SOC_ADE_SOFT_RST_SEL0_ch5_rdma_srst_sel_START    (4)
#define SOC_ADE_SOFT_RST_SEL0_ch5_rdma_srst_sel_END      (4)
#define SOC_ADE_SOFT_RST_SEL0_ch6_rdma_srst_sel_START    (5)
#define SOC_ADE_SOFT_RST_SEL0_ch6_rdma_srst_sel_END      (5)
#define SOC_ADE_SOFT_RST_SEL0_disp_rdma_srst_sel_START   (6)
#define SOC_ADE_SOFT_RST_SEL0_disp_rdma_srst_sel_END     (6)
#define SOC_ADE_SOFT_RST_SEL0_cmdq1_rdma_srst_sel_START  (7)
#define SOC_ADE_SOFT_RST_SEL0_cmdq1_rdma_srst_sel_END    (7)
#define SOC_ADE_SOFT_RST_SEL0_cmdq2_rdma_srst_sel_START  (8)
#define SOC_ADE_SOFT_RST_SEL0_cmdq2_rdma_srst_sel_END    (8)
#define SOC_ADE_SOFT_RST_SEL0_ch1_wdma_srst_sel_START    (10)
#define SOC_ADE_SOFT_RST_SEL0_ch1_wdma_srst_sel_END      (10)
#define SOC_ADE_SOFT_RST_SEL0_ch2_wdma_srst_sel_START    (11)
#define SOC_ADE_SOFT_RST_SEL0_ch2_wdma_srst_sel_END      (11)
#define SOC_ADE_SOFT_RST_SEL0_ch3_wdma_srst_sel_START    (12)
#define SOC_ADE_SOFT_RST_SEL0_ch3_wdma_srst_sel_END      (12)
#define SOC_ADE_SOFT_RST_SEL0_cmdq_wdma_srst_sel_START   (14)
#define SOC_ADE_SOFT_RST_SEL0_cmdq_wdma_srst_sel_END     (14)
#define SOC_ADE_SOFT_RST_SEL0_clip1_srst_sel_START       (15)
#define SOC_ADE_SOFT_RST_SEL0_clip1_srst_sel_END         (15)
#define SOC_ADE_SOFT_RST_SEL0_clip2_srst_sel_START       (16)
#define SOC_ADE_SOFT_RST_SEL0_clip2_srst_sel_END         (16)
#define SOC_ADE_SOFT_RST_SEL0_clip3_srst_sel_START       (17)
#define SOC_ADE_SOFT_RST_SEL0_clip3_srst_sel_END         (17)
#define SOC_ADE_SOFT_RST_SEL0_clip4_srst_sel_START       (18)
#define SOC_ADE_SOFT_RST_SEL0_clip4_srst_sel_END         (18)
#define SOC_ADE_SOFT_RST_SEL0_clip5_srst_sel_START       (19)
#define SOC_ADE_SOFT_RST_SEL0_clip5_srst_sel_END         (19)
#define SOC_ADE_SOFT_RST_SEL0_clip6_srst_sel_START       (20)
#define SOC_ADE_SOFT_RST_SEL0_clip6_srst_sel_END         (20)
#define SOC_ADE_SOFT_RST_SEL0_scl1_srst_sel_START        (21)
#define SOC_ADE_SOFT_RST_SEL0_scl1_srst_sel_END          (21)
#define SOC_ADE_SOFT_RST_SEL0_scl2_srst_sel_START        (22)
#define SOC_ADE_SOFT_RST_SEL0_scl2_srst_sel_END          (22)
#define SOC_ADE_SOFT_RST_SEL0_scl3_srst_sel_START        (23)
#define SOC_ADE_SOFT_RST_SEL0_scl3_srst_sel_END          (23)
#define SOC_ADE_SOFT_RST_SEL0_ctran1_srst_sel_START      (24)
#define SOC_ADE_SOFT_RST_SEL0_ctran1_srst_sel_END        (24)
#define SOC_ADE_SOFT_RST_SEL0_ctran2_srst_sel_START      (25)
#define SOC_ADE_SOFT_RST_SEL0_ctran2_srst_sel_END        (25)
#define SOC_ADE_SOFT_RST_SEL0_ctran3_srst_sel_START      (26)
#define SOC_ADE_SOFT_RST_SEL0_ctran3_srst_sel_END        (26)
#define SOC_ADE_SOFT_RST_SEL0_ctran4_srst_sel_START      (27)
#define SOC_ADE_SOFT_RST_SEL0_ctran4_srst_sel_END        (27)
#define SOC_ADE_SOFT_RST_SEL0_ctran5_srst_sel_START      (28)
#define SOC_ADE_SOFT_RST_SEL0_ctran5_srst_sel_END        (28)
#define SOC_ADE_SOFT_RST_SEL0_ctran6_srst_sel_START      (29)
#define SOC_ADE_SOFT_RST_SEL0_ctran6_srst_sel_END        (29)
#define SOC_ADE_SOFT_RST_SEL0_rot_srst_sel_START         (30)
#define SOC_ADE_SOFT_RST_SEL0_rot_srst_sel_END           (30)


/*****************************************************************************
 结构名    : SOC_ADE_SOFT_RST_SEL1_UNION
 结构说明  : SOFT_RST_SEL1 寄存器结构定义。地址偏移量:0x007C，初值:0x00000018，宽度:32
 寄存器说明: 子模块软复位源选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cmdq1_srst_sel  : 1;  /* bit[0]   : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                          0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                          1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  cmdq2_srst_sel  : 1;  /* bit[1]   : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                          0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                          1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  reserved_0      : 1;  /* bit[2]   : 保留。 */
        unsigned int  gamma_srst_sel  : 1;  /* bit[3]   : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                          0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                          1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  dither_srst_sel : 1;  /* bit[4]   : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                          0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                          1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ovly1_srst_sel  : 1;  /* bit[5]   : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                          0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                          1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ovly2_srst_sel  : 1;  /* bit[6]   : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                          0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                          1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  ovly3_srst_sel  : 1;  /* bit[7]   : 子模块软复位源选择，需要与frm_end_start选择配合使用：
                                                          0：软件配置SOFT_RST寄存器对应的bit位时产生软复位，子模块不会重新开始工作；
                                                          1：硬件资源同步更新信号产生软复位，子模块不会重新开始工作； */
        unsigned int  reserved_1      : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_ADE_SOFT_RST_SEL1_UNION;
#endif
#define SOC_ADE_SOFT_RST_SEL1_cmdq1_srst_sel_START   (0)
#define SOC_ADE_SOFT_RST_SEL1_cmdq1_srst_sel_END     (0)
#define SOC_ADE_SOFT_RST_SEL1_cmdq2_srst_sel_START   (1)
#define SOC_ADE_SOFT_RST_SEL1_cmdq2_srst_sel_END     (1)
#define SOC_ADE_SOFT_RST_SEL1_gamma_srst_sel_START   (3)
#define SOC_ADE_SOFT_RST_SEL1_gamma_srst_sel_END     (3)
#define SOC_ADE_SOFT_RST_SEL1_dither_srst_sel_START  (4)
#define SOC_ADE_SOFT_RST_SEL1_dither_srst_sel_END    (4)
#define SOC_ADE_SOFT_RST_SEL1_ovly1_srst_sel_START   (5)
#define SOC_ADE_SOFT_RST_SEL1_ovly1_srst_sel_END     (5)
#define SOC_ADE_SOFT_RST_SEL1_ovly2_srst_sel_START   (6)
#define SOC_ADE_SOFT_RST_SEL1_ovly2_srst_sel_END     (6)
#define SOC_ADE_SOFT_RST_SEL1_ovly3_srst_sel_START   (7)
#define SOC_ADE_SOFT_RST_SEL1_ovly3_srst_sel_END     (7)


/*****************************************************************************
 结构名    : SOC_ADE_STAT0_UNION
 结构说明  : STAT0 寄存器结构定义。地址偏移量:0x0080，初值:0x00000000，宽度:32
 寄存器说明: 子模块工作状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_rdma_status   : 1;  /* bit[0] : 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ch2_rdma_status   : 1;  /* bit[1] : 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ch3_rdma_status   : 1;  /* bit[2] : 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ch4_rdma_status   : 1;  /* bit[3] : 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ch5_rdma_status   : 1;  /* bit[4] : 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ch6_rdma_status   : 1;  /* bit[5] : 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  disp_rdma_status  : 1;  /* bit[6] : 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  cmdq1_rdma_status : 1;  /* bit[7] : 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  cmdq2_rdma_status : 1;  /* bit[8] : 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  reserved_0        : 1;  /* bit[9] : 保留。 */
        unsigned int  ch1_wdma_status   : 1;  /* bit[10]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ch2_wdma_status   : 1;  /* bit[11]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ch3_wdma_status   : 1;  /* bit[12]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  reserved_1        : 1;  /* bit[13]: 保留。 */
        unsigned int  cmdq_wdma_status  : 1;  /* bit[14]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  clip1_status      : 1;  /* bit[15]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  clip2_status      : 1;  /* bit[16]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  clip3_status      : 1;  /* bit[17]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  clip4_status      : 1;  /* bit[18]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  clip5_status      : 1;  /* bit[19]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  clip6_status      : 1;  /* bit[20]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  scl1_status       : 1;  /* bit[21]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  scl2_status       : 1;  /* bit[22]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  scl3_status       : 1;  /* bit[23]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ctran1_status     : 1;  /* bit[24]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ctran2_status     : 1;  /* bit[25]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ctran3_status     : 1;  /* bit[26]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ctran4_status     : 1;  /* bit[27]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ctran5_status     : 1;  /* bit[28]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  ctran6_status     : 1;  /* bit[29]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  rot_status        : 1;  /* bit[30]: 子模块工作状态：
                                                          0：空闲；
                                                          1：工作； */
        unsigned int  reserved_2        : 1;  /* bit[31]: 保留。 */
    } reg;
} SOC_ADE_STAT0_UNION;
#endif
#define SOC_ADE_STAT0_ch1_rdma_status_START    (0)
#define SOC_ADE_STAT0_ch1_rdma_status_END      (0)
#define SOC_ADE_STAT0_ch2_rdma_status_START    (1)
#define SOC_ADE_STAT0_ch2_rdma_status_END      (1)
#define SOC_ADE_STAT0_ch3_rdma_status_START    (2)
#define SOC_ADE_STAT0_ch3_rdma_status_END      (2)
#define SOC_ADE_STAT0_ch4_rdma_status_START    (3)
#define SOC_ADE_STAT0_ch4_rdma_status_END      (3)
#define SOC_ADE_STAT0_ch5_rdma_status_START    (4)
#define SOC_ADE_STAT0_ch5_rdma_status_END      (4)
#define SOC_ADE_STAT0_ch6_rdma_status_START    (5)
#define SOC_ADE_STAT0_ch6_rdma_status_END      (5)
#define SOC_ADE_STAT0_disp_rdma_status_START   (6)
#define SOC_ADE_STAT0_disp_rdma_status_END     (6)
#define SOC_ADE_STAT0_cmdq1_rdma_status_START  (7)
#define SOC_ADE_STAT0_cmdq1_rdma_status_END    (7)
#define SOC_ADE_STAT0_cmdq2_rdma_status_START  (8)
#define SOC_ADE_STAT0_cmdq2_rdma_status_END    (8)
#define SOC_ADE_STAT0_ch1_wdma_status_START    (10)
#define SOC_ADE_STAT0_ch1_wdma_status_END      (10)
#define SOC_ADE_STAT0_ch2_wdma_status_START    (11)
#define SOC_ADE_STAT0_ch2_wdma_status_END      (11)
#define SOC_ADE_STAT0_ch3_wdma_status_START    (12)
#define SOC_ADE_STAT0_ch3_wdma_status_END      (12)
#define SOC_ADE_STAT0_cmdq_wdma_status_START   (14)
#define SOC_ADE_STAT0_cmdq_wdma_status_END     (14)
#define SOC_ADE_STAT0_clip1_status_START       (15)
#define SOC_ADE_STAT0_clip1_status_END         (15)
#define SOC_ADE_STAT0_clip2_status_START       (16)
#define SOC_ADE_STAT0_clip2_status_END         (16)
#define SOC_ADE_STAT0_clip3_status_START       (17)
#define SOC_ADE_STAT0_clip3_status_END         (17)
#define SOC_ADE_STAT0_clip4_status_START       (18)
#define SOC_ADE_STAT0_clip4_status_END         (18)
#define SOC_ADE_STAT0_clip5_status_START       (19)
#define SOC_ADE_STAT0_clip5_status_END         (19)
#define SOC_ADE_STAT0_clip6_status_START       (20)
#define SOC_ADE_STAT0_clip6_status_END         (20)
#define SOC_ADE_STAT0_scl1_status_START        (21)
#define SOC_ADE_STAT0_scl1_status_END          (21)
#define SOC_ADE_STAT0_scl2_status_START        (22)
#define SOC_ADE_STAT0_scl2_status_END          (22)
#define SOC_ADE_STAT0_scl3_status_START        (23)
#define SOC_ADE_STAT0_scl3_status_END          (23)
#define SOC_ADE_STAT0_ctran1_status_START      (24)
#define SOC_ADE_STAT0_ctran1_status_END        (24)
#define SOC_ADE_STAT0_ctran2_status_START      (25)
#define SOC_ADE_STAT0_ctran2_status_END        (25)
#define SOC_ADE_STAT0_ctran3_status_START      (26)
#define SOC_ADE_STAT0_ctran3_status_END        (26)
#define SOC_ADE_STAT0_ctran4_status_START      (27)
#define SOC_ADE_STAT0_ctran4_status_END        (27)
#define SOC_ADE_STAT0_ctran5_status_START      (28)
#define SOC_ADE_STAT0_ctran5_status_END        (28)
#define SOC_ADE_STAT0_ctran6_status_START      (29)
#define SOC_ADE_STAT0_ctran6_status_END        (29)
#define SOC_ADE_STAT0_rot_status_START         (30)
#define SOC_ADE_STAT0_rot_status_END           (30)


/*****************************************************************************
 结构名    : SOC_ADE_STAT1_UNION
 结构说明  : STAT1 寄存器结构定义。地址偏移量:0x0084，初值:0x00000000，宽度:32
 寄存器说明: 子模块工作状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cmdq1_status     : 1;  /* bit[0]    : 子模块工作状态：
                                                            0：空闲；
                                                            1：工作； */
        unsigned int  cmdq2_status     : 1;  /* bit[1]    : 子模块工作状态：
                                                            0：空闲；
                                                            1：工作； */
        unsigned int  reserved_0       : 1;  /* bit[2]    : 保留。 */
        unsigned int  gamma_status     : 1;  /* bit[3]    : 子模块工作状态：
                                                            0：空闲；
                                                            1：工作； */
        unsigned int  dither_status    : 1;  /* bit[4]    : 子模块工作状态：
                                                            0：空闲；
                                                            1：工作； */
        unsigned int  ovly1_status     : 1;  /* bit[5]    : 子模块工作状态：
                                                            0：空闲；
                                                            1：工作； */
        unsigned int  ovly2_status     : 1;  /* bit[6]    : 子模块工作状态：
                                                            0：空闲；
                                                            1：工作； */
        unsigned int  ovly3_status     : 1;  /* bit[7]    : 子模块工作状态：
                                                            0：空闲；
                                                            1：工作； */
        unsigned int  reserved_1       : 8;  /* bit[8-15] : 保留。 */
        unsigned int  dfs_buf_fill_lev : 13; /* bit[16-28]: DFS buffer填充水线 */
        unsigned int  reserved_2       : 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_ADE_STAT1_UNION;
#endif
#define SOC_ADE_STAT1_cmdq1_status_START      (0)
#define SOC_ADE_STAT1_cmdq1_status_END        (0)
#define SOC_ADE_STAT1_cmdq2_status_START      (1)
#define SOC_ADE_STAT1_cmdq2_status_END        (1)
#define SOC_ADE_STAT1_gamma_status_START      (3)
#define SOC_ADE_STAT1_gamma_status_END        (3)
#define SOC_ADE_STAT1_dither_status_START     (4)
#define SOC_ADE_STAT1_dither_status_END       (4)
#define SOC_ADE_STAT1_ovly1_status_START      (5)
#define SOC_ADE_STAT1_ovly1_status_END        (5)
#define SOC_ADE_STAT1_ovly2_status_START      (6)
#define SOC_ADE_STAT1_ovly2_status_END        (6)
#define SOC_ADE_STAT1_ovly3_status_START      (7)
#define SOC_ADE_STAT1_ovly3_status_END        (7)
#define SOC_ADE_STAT1_dfs_buf_fill_lev_START  (16)
#define SOC_ADE_STAT1_dfs_buf_fill_lev_END    (28)


/*****************************************************************************
 结构名    : SOC_ADE_DMA_ERR_STAT_UNION
 结构说明  : DMA_ERR_STAT 寄存器结构定义。地址偏移量:0x0088，初值:0x00000000，宽度:32
 寄存器说明: DMA错误状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_rdma_err_status   : 1;  /* bit[0]    : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  ch2_rdma_err_status   : 1;  /* bit[1]    : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  ch3_rdma_err_status   : 1;  /* bit[2]    : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  ch4_rdma_err_status   : 1;  /* bit[3]    : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  ch5_rdma_err_status   : 1;  /* bit[4]    : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  ch6_rdma_err_status   : 1;  /* bit[5]    : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  disp_rdma_err_status  : 1;  /* bit[6]    : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  cmdq1_rdma_err_status : 1;  /* bit[7]    : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  cmdq2_rdma_err_status : 1;  /* bit[8]    : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  reserved_0            : 1;  /* bit[9]    : 保留。 */
        unsigned int  ch1_wdma_err_status   : 1;  /* bit[10]   : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  ch2_wdma_err_status   : 1;  /* bit[11]   : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  ch3_wdma_err_status   : 1;  /* bit[12]   : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  reserved_1            : 1;  /* bit[13]   : 保留。 */
        unsigned int  cmdq_wdma_err_status  : 1;  /* bit[14]   : DMA错误指示，读清除：
                                                                 0：无错误；
                                                                 1：AXI发生错误； */
        unsigned int  reserved_2            : 17; /* bit[15-31]: 保留。 */
    } reg;
} SOC_ADE_DMA_ERR_STAT_UNION;
#endif
#define SOC_ADE_DMA_ERR_STAT_ch1_rdma_err_status_START    (0)
#define SOC_ADE_DMA_ERR_STAT_ch1_rdma_err_status_END      (0)
#define SOC_ADE_DMA_ERR_STAT_ch2_rdma_err_status_START    (1)
#define SOC_ADE_DMA_ERR_STAT_ch2_rdma_err_status_END      (1)
#define SOC_ADE_DMA_ERR_STAT_ch3_rdma_err_status_START    (2)
#define SOC_ADE_DMA_ERR_STAT_ch3_rdma_err_status_END      (2)
#define SOC_ADE_DMA_ERR_STAT_ch4_rdma_err_status_START    (3)
#define SOC_ADE_DMA_ERR_STAT_ch4_rdma_err_status_END      (3)
#define SOC_ADE_DMA_ERR_STAT_ch5_rdma_err_status_START    (4)
#define SOC_ADE_DMA_ERR_STAT_ch5_rdma_err_status_END      (4)
#define SOC_ADE_DMA_ERR_STAT_ch6_rdma_err_status_START    (5)
#define SOC_ADE_DMA_ERR_STAT_ch6_rdma_err_status_END      (5)
#define SOC_ADE_DMA_ERR_STAT_disp_rdma_err_status_START   (6)
#define SOC_ADE_DMA_ERR_STAT_disp_rdma_err_status_END     (6)
#define SOC_ADE_DMA_ERR_STAT_cmdq1_rdma_err_status_START  (7)
#define SOC_ADE_DMA_ERR_STAT_cmdq1_rdma_err_status_END    (7)
#define SOC_ADE_DMA_ERR_STAT_cmdq2_rdma_err_status_START  (8)
#define SOC_ADE_DMA_ERR_STAT_cmdq2_rdma_err_status_END    (8)
#define SOC_ADE_DMA_ERR_STAT_ch1_wdma_err_status_START    (10)
#define SOC_ADE_DMA_ERR_STAT_ch1_wdma_err_status_END      (10)
#define SOC_ADE_DMA_ERR_STAT_ch2_wdma_err_status_START    (11)
#define SOC_ADE_DMA_ERR_STAT_ch2_wdma_err_status_END      (11)
#define SOC_ADE_DMA_ERR_STAT_ch3_wdma_err_status_START    (12)
#define SOC_ADE_DMA_ERR_STAT_ch3_wdma_err_status_END      (12)
#define SOC_ADE_DMA_ERR_STAT_cmdq_wdma_err_status_START   (14)
#define SOC_ADE_DMA_ERR_STAT_cmdq_wdma_err_status_END     (14)


/*****************************************************************************
 结构名    : SOC_ADE_CTRL1_UNION
 结构说明  : CTRL1 寄存器结构定义。地址偏移量:0x008C，初值:0x00000001，宽度:32
 寄存器说明: ADE全局控制寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  auto_clk_gate_en : 1;  /* bit[0]   : ADE全局自动时钟门控使能。
                                                           0：禁用；
                                                           1：使能。 */
        unsigned int  rot_buf_shr_out  : 1;  /* bit[1]   : 旋转buffer外部共享使能，用于控制channel5的旋转buffer是否共享给codec。如果共享使能，则rd_ch5_nv必须配置为0，rot_buf_cfg必须配置为2或3。
                                                           0：禁用；
                                                           1：使能。 */
        unsigned int  reserved         : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_ADE_CTRL1_UNION;
#endif
#define SOC_ADE_CTRL1_auto_clk_gate_en_START  (0)
#define SOC_ADE_CTRL1_auto_clk_gate_en_END    (0)
#define SOC_ADE_CTRL1_rot_buf_shr_out_START   (1)
#define SOC_ADE_CTRL1_rot_buf_shr_out_END     (1)


/*****************************************************************************
 结构名    : SOC_ADE_SEC_CFG0_UNION
 结构说明  : SEC_CFG0 寄存器结构定义。地址偏移量:0x0090，初值:0x00000000，宽度:32
 寄存器说明: ADE安全配置寄存器0。此寄存器用于控制子模块的寄存器是否安全可配，寄存器本身只能通过安全方式配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_rdma_sec   : 1;  /* bit[0] : 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ch2_rdma_sec   : 1;  /* bit[1] : 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ch3_rdma_sec   : 1;  /* bit[2] : 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ch4_rdma_sec   : 1;  /* bit[3] : 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ch5_rdma_sec   : 1;  /* bit[4] : 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ch6_rdma_sec   : 1;  /* bit[5] : 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  disp_rdma_sec  : 1;  /* bit[6] : 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  cmdq1_rdma_sec : 1;  /* bit[7] : 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  cmdq2_rdma_sec : 1;  /* bit[8] : 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  reserved_0     : 1;  /* bit[9] : 保留。 */
        unsigned int  ch1_wdma_sec   : 1;  /* bit[10]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ch2_wdma_sec   : 1;  /* bit[11]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ch3_wdma_sec   : 1;  /* bit[12]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  reserved_1     : 1;  /* bit[13]: 保留。 */
        unsigned int  cmdq_wdma_sec  : 1;  /* bit[14]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  clip1_sec      : 1;  /* bit[15]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  clip2_sec      : 1;  /* bit[16]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  clip3_sec      : 1;  /* bit[17]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  clip4_sec      : 1;  /* bit[18]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  clip5_sec      : 1;  /* bit[19]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  clip6_sec      : 1;  /* bit[20]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  scl1_sec       : 1;  /* bit[21]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  scl2_sec       : 1;  /* bit[22]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  scl3_sec       : 1;  /* bit[23]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ctran1_sec     : 1;  /* bit[24]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ctran2_sec     : 1;  /* bit[25]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ctran3_sec     : 1;  /* bit[26]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ctran4_sec     : 1;  /* bit[27]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ctran5_sec     : 1;  /* bit[28]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  ctran6_sec     : 1;  /* bit[29]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  rot_sec        : 1;  /* bit[30]: 子模块安全属性：
                                                       0：安全/非安全可配；
                                                       1：只有安全可配； */
        unsigned int  reserved_2     : 1;  /* bit[31]: 保留。 */
    } reg;
} SOC_ADE_SEC_CFG0_UNION;
#endif
#define SOC_ADE_SEC_CFG0_ch1_rdma_sec_START    (0)
#define SOC_ADE_SEC_CFG0_ch1_rdma_sec_END      (0)
#define SOC_ADE_SEC_CFG0_ch2_rdma_sec_START    (1)
#define SOC_ADE_SEC_CFG0_ch2_rdma_sec_END      (1)
#define SOC_ADE_SEC_CFG0_ch3_rdma_sec_START    (2)
#define SOC_ADE_SEC_CFG0_ch3_rdma_sec_END      (2)
#define SOC_ADE_SEC_CFG0_ch4_rdma_sec_START    (3)
#define SOC_ADE_SEC_CFG0_ch4_rdma_sec_END      (3)
#define SOC_ADE_SEC_CFG0_ch5_rdma_sec_START    (4)
#define SOC_ADE_SEC_CFG0_ch5_rdma_sec_END      (4)
#define SOC_ADE_SEC_CFG0_ch6_rdma_sec_START    (5)
#define SOC_ADE_SEC_CFG0_ch6_rdma_sec_END      (5)
#define SOC_ADE_SEC_CFG0_disp_rdma_sec_START   (6)
#define SOC_ADE_SEC_CFG0_disp_rdma_sec_END     (6)
#define SOC_ADE_SEC_CFG0_cmdq1_rdma_sec_START  (7)
#define SOC_ADE_SEC_CFG0_cmdq1_rdma_sec_END    (7)
#define SOC_ADE_SEC_CFG0_cmdq2_rdma_sec_START  (8)
#define SOC_ADE_SEC_CFG0_cmdq2_rdma_sec_END    (8)
#define SOC_ADE_SEC_CFG0_ch1_wdma_sec_START    (10)
#define SOC_ADE_SEC_CFG0_ch1_wdma_sec_END      (10)
#define SOC_ADE_SEC_CFG0_ch2_wdma_sec_START    (11)
#define SOC_ADE_SEC_CFG0_ch2_wdma_sec_END      (11)
#define SOC_ADE_SEC_CFG0_ch3_wdma_sec_START    (12)
#define SOC_ADE_SEC_CFG0_ch3_wdma_sec_END      (12)
#define SOC_ADE_SEC_CFG0_cmdq_wdma_sec_START   (14)
#define SOC_ADE_SEC_CFG0_cmdq_wdma_sec_END     (14)
#define SOC_ADE_SEC_CFG0_clip1_sec_START       (15)
#define SOC_ADE_SEC_CFG0_clip1_sec_END         (15)
#define SOC_ADE_SEC_CFG0_clip2_sec_START       (16)
#define SOC_ADE_SEC_CFG0_clip2_sec_END         (16)
#define SOC_ADE_SEC_CFG0_clip3_sec_START       (17)
#define SOC_ADE_SEC_CFG0_clip3_sec_END         (17)
#define SOC_ADE_SEC_CFG0_clip4_sec_START       (18)
#define SOC_ADE_SEC_CFG0_clip4_sec_END         (18)
#define SOC_ADE_SEC_CFG0_clip5_sec_START       (19)
#define SOC_ADE_SEC_CFG0_clip5_sec_END         (19)
#define SOC_ADE_SEC_CFG0_clip6_sec_START       (20)
#define SOC_ADE_SEC_CFG0_clip6_sec_END         (20)
#define SOC_ADE_SEC_CFG0_scl1_sec_START        (21)
#define SOC_ADE_SEC_CFG0_scl1_sec_END          (21)
#define SOC_ADE_SEC_CFG0_scl2_sec_START        (22)
#define SOC_ADE_SEC_CFG0_scl2_sec_END          (22)
#define SOC_ADE_SEC_CFG0_scl3_sec_START        (23)
#define SOC_ADE_SEC_CFG0_scl3_sec_END          (23)
#define SOC_ADE_SEC_CFG0_ctran1_sec_START      (24)
#define SOC_ADE_SEC_CFG0_ctran1_sec_END        (24)
#define SOC_ADE_SEC_CFG0_ctran2_sec_START      (25)
#define SOC_ADE_SEC_CFG0_ctran2_sec_END        (25)
#define SOC_ADE_SEC_CFG0_ctran3_sec_START      (26)
#define SOC_ADE_SEC_CFG0_ctran3_sec_END        (26)
#define SOC_ADE_SEC_CFG0_ctran4_sec_START      (27)
#define SOC_ADE_SEC_CFG0_ctran4_sec_END        (27)
#define SOC_ADE_SEC_CFG0_ctran5_sec_START      (28)
#define SOC_ADE_SEC_CFG0_ctran5_sec_END        (28)
#define SOC_ADE_SEC_CFG0_ctran6_sec_START      (29)
#define SOC_ADE_SEC_CFG0_ctran6_sec_END        (29)
#define SOC_ADE_SEC_CFG0_rot_sec_START         (30)
#define SOC_ADE_SEC_CFG0_rot_sec_END           (30)


/*****************************************************************************
 结构名    : SOC_ADE_SEC_CFG1_UNION
 结构说明  : SEC_CFG1 寄存器结构定义。地址偏移量:0x0094，初值:0x00000000，宽度:32
 寄存器说明: ADE安全配置寄存器1。此寄存器用于控制子模块的寄存器是否安全可配，寄存器本身只能通过安全方式配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cmdq1_sec  : 1;  /* bit[0]   : 子模块安全属性，只控制安全中断相关寄存器：
                                                     0：安全/非安全可配；
                                                     1：只有安全可配； */
        unsigned int  cmdq2_sec  : 1;  /* bit[1]   : 子模块安全属性：
                                                     0：安全/非安全可配；
                                                     1：只有安全可配； */
        unsigned int  reserved_0 : 1;  /* bit[2]   : 保留。 */
        unsigned int  gamma_sec  : 1;  /* bit[3]   : 子模块安全属性：
                                                     0：安全/非安全可配；
                                                     1：只有安全可配； */
        unsigned int  dither_sec : 1;  /* bit[4]   : 子模块安全属性：
                                                     0：安全/非安全可配；
                                                     1：只有安全可配； */
        unsigned int  ovly1_sec  : 1;  /* bit[5]   : 子模块安全属性：
                                                     0：安全/非安全可配；
                                                     1：只有安全可配； */
        unsigned int  ovly2_sec  : 1;  /* bit[6]   : 子模块安全属性：
                                                     0：安全/非安全可配；
                                                     1：只有安全可配； */
        unsigned int  ovly3_sec  : 1;  /* bit[7]   : 子模块安全属性：
                                                     0：安全/非安全可配；
                                                     1：只有安全可配； */
        unsigned int  top_sec    : 1;  /* bit[8]   : 子模块安全属性：
                                                     0：安全/非安全可配；
                                                     1：只有安全可配； */
        unsigned int  reserved_1 : 23; /* bit[9-31]: 保留。 */
    } reg;
} SOC_ADE_SEC_CFG1_UNION;
#endif
#define SOC_ADE_SEC_CFG1_cmdq1_sec_START   (0)
#define SOC_ADE_SEC_CFG1_cmdq1_sec_END     (0)
#define SOC_ADE_SEC_CFG1_cmdq2_sec_START   (1)
#define SOC_ADE_SEC_CFG1_cmdq2_sec_END     (1)
#define SOC_ADE_SEC_CFG1_gamma_sec_START   (3)
#define SOC_ADE_SEC_CFG1_gamma_sec_END     (3)
#define SOC_ADE_SEC_CFG1_dither_sec_START  (4)
#define SOC_ADE_SEC_CFG1_dither_sec_END    (4)
#define SOC_ADE_SEC_CFG1_ovly1_sec_START   (5)
#define SOC_ADE_SEC_CFG1_ovly1_sec_END     (5)
#define SOC_ADE_SEC_CFG1_ovly2_sec_START   (6)
#define SOC_ADE_SEC_CFG1_ovly2_sec_END     (6)
#define SOC_ADE_SEC_CFG1_ovly3_sec_START   (7)
#define SOC_ADE_SEC_CFG1_ovly3_sec_END     (7)
#define SOC_ADE_SEC_CFG1_top_sec_START     (8)
#define SOC_ADE_SEC_CFG1_top_sec_END       (8)


/*****************************************************************************
 结构名    : SOC_ADE_OVLY_CTL_UNION
 结构说明  : OVLY_CTL 寄存器结构定义。地址偏移量:0x0098，初值:0x22222222，宽度:32
 寄存器说明: OVLY通道控制寄存器。属于静态配置寄存器，禁止在ADE工作过程中动态配置。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_ovly_sel : 2;  /* bit[0-1]  : 2'b00:该通道不属于任何OVLY
                                                        2'b01:该通道属于OVLY1
                                                        2'b10:该通道属于OVLY2
                                                        2'b11:该通道属于OVLY3 */
        unsigned int  reserved_0   : 2;  /* bit[2-3]  :  */
        unsigned int  ch2_ovly_sel : 2;  /* bit[4-5]  : 2'b00:该通道不属于任何OVLY
                                                        2'b01:该通道属于OVLY1
                                                        2'b10:该通道属于OVLY2
                                                        2'b11:该通道属于OVLY3 */
        unsigned int  reserved_1   : 2;  /* bit[6-7]  :  */
        unsigned int  ch3_ovly_sel : 2;  /* bit[8-9]  : 2'b00:该通道不属于任何OVLY
                                                        2'b01:该通道属于OVLY
                                                        2'b10:该通道属于OVLY2
                                                        2'b11:该通道属于OVLY3 */
        unsigned int  reserved_2   : 2;  /* bit[10-11]:  */
        unsigned int  ch4_ovly_sel : 2;  /* bit[12-13]: 2'b00:该通道不属于任何OVLY
                                                        2'b01:该通道属于OVLY1
                                                        2'b10:该通道属于OVLY2
                                                        2'b11:该通道属于OVLY3 */
        unsigned int  reserved_3   : 2;  /* bit[14-15]:  */
        unsigned int  ch5_ovly_sel : 2;  /* bit[16-17]: 2'b00:该通道不属于任何OVLY
                                                        2'b01:该通道属于OVLY1
                                                        2'b10:该通道属于OVLY2
                                                        2'b11:该通道属于OVLY3 */
        unsigned int  reserved_4   : 2;  /* bit[18-19]:  */
        unsigned int  ch6_ovly_sel : 2;  /* bit[20-21]: 2'b00:该通道不属于任何OVLY
                                                        2'b01:该通道属于OVLY1
                                                        2'b10:该通道属于OVLY2
                                                        2'b11:该通道属于OVLY3 */
        unsigned int  reserved_5   : 2;  /* bit[22-23]:  */
        unsigned int  ch7_ovly_sel : 2;  /* bit[24-25]: 2'b00:该通道不属于任何OVLY
                                                        2'b01:该通道属于OVLY1
                                                        2'b10:该通道属于OVLY2
                                                        2'b11:该通道属于OVLY3 */
        unsigned int  reserved_6   : 2;  /* bit[26-27]:  */
        unsigned int  ch8_ovly_sel : 2;  /* bit[28-29]: 2'b00:该通道不属于任何OVLY
                                                        2'b01:该通道属于OVLY1
                                                        2'b10:该通道属于OVLY2
                                                        2'b11:该通道属于OVLY3 */
        unsigned int  reserved_7   : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_ADE_OVLY_CTL_UNION;
#endif
#define SOC_ADE_OVLY_CTL_ch1_ovly_sel_START  (0)
#define SOC_ADE_OVLY_CTL_ch1_ovly_sel_END    (1)
#define SOC_ADE_OVLY_CTL_ch2_ovly_sel_START  (4)
#define SOC_ADE_OVLY_CTL_ch2_ovly_sel_END    (5)
#define SOC_ADE_OVLY_CTL_ch3_ovly_sel_START  (8)
#define SOC_ADE_OVLY_CTL_ch3_ovly_sel_END    (9)
#define SOC_ADE_OVLY_CTL_ch4_ovly_sel_START  (12)
#define SOC_ADE_OVLY_CTL_ch4_ovly_sel_END    (13)
#define SOC_ADE_OVLY_CTL_ch5_ovly_sel_START  (16)
#define SOC_ADE_OVLY_CTL_ch5_ovly_sel_END    (17)
#define SOC_ADE_OVLY_CTL_ch6_ovly_sel_START  (20)
#define SOC_ADE_OVLY_CTL_ch6_ovly_sel_END    (21)
#define SOC_ADE_OVLY_CTL_ch7_ovly_sel_START  (24)
#define SOC_ADE_OVLY_CTL_ch7_ovly_sel_END    (25)
#define SOC_ADE_OVLY_CTL_ch8_ovly_sel_START  (28)
#define SOC_ADE_OVLY_CTL_ch8_ovly_sel_END    (29)


/*****************************************************************************
 结构名    : SOC_ADE_AUTO_CLK_GT_EN0_UNION
 结构说明  : AUTO_CLK_GT_EN0 寄存器结构定义。地址偏移量:0x009C，初值:0x7FFFFFFF，宽度:32
 寄存器说明: ADE子模块自动时钟门控控制寄存器0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_rdma_auto_clk_gt_en   : 1;  /* bit[0] : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ch2_rdma_auto_clk_gt_en   : 1;  /* bit[1] : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ch3_rdma_auto_clk_gt_en   : 1;  /* bit[2] : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ch4_rdma_auto_clk_gt_en   : 1;  /* bit[3] : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ch5_rdma_auto_clk_gt_en   : 1;  /* bit[4] : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ch6_rdma_auto_clk_gt_en   : 1;  /* bit[5] : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  disp_rdma_auto_clk_gt_en  : 1;  /* bit[6] : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  cmdq1_rdma_auto_clk_gt_en : 1;  /* bit[7] : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  cmdq2_rdma_auto_clk_gt_en : 1;  /* bit[8] : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  reserved_0                : 1;  /* bit[9] : 保留。 */
        unsigned int  ch1_wdma_auto_clk_gt_en   : 1;  /* bit[10]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ch2_wdma_auto_clk_gt_en   : 1;  /* bit[11]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ch3_wdma_auto_clk_gt_en   : 1;  /* bit[12]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  reserved_1                : 1;  /* bit[13]: 保留。 */
        unsigned int  cmdq_wdma_auto_clk_gt_en  : 1;  /* bit[14]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  clip1_auto_clk_gt_en      : 1;  /* bit[15]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  clip2_auto_clk_gt_en      : 1;  /* bit[16]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  clip3_auto_clk_gt_en      : 1;  /* bit[17]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  clip4_auto_clk_gt_en      : 1;  /* bit[18]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  clip5_auto_clk_gt_en      : 1;  /* bit[19]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  clip6_auto_clk_gt_en      : 1;  /* bit[20]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  scl1_auto_clk_gt_en       : 1;  /* bit[21]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  scl2_auto_clk_gt_en       : 1;  /* bit[22]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  scl3_auto_clk_gt_en       : 1;  /* bit[23]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ctran1_auto_clk_gt_en     : 1;  /* bit[24]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ctran2_auto_clk_gt_en     : 1;  /* bit[25]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ctran3_auto_clk_gt_en     : 1;  /* bit[26]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ctran4_auto_clk_gt_en     : 1;  /* bit[27]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ctran5_auto_clk_gt_en     : 1;  /* bit[28]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  ctran6_auto_clk_gt_en     : 1;  /* bit[29]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  rot_auto_clk_gt_en        : 1;  /* bit[30]: 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                  0：自动时钟门控禁用；
                                                                  1：自动时钟门控使能； */
        unsigned int  reserved_2                : 1;  /* bit[31]: 保留。 */
    } reg;
} SOC_ADE_AUTO_CLK_GT_EN0_UNION;
#endif
#define SOC_ADE_AUTO_CLK_GT_EN0_ch1_rdma_auto_clk_gt_en_START    (0)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch1_rdma_auto_clk_gt_en_END      (0)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch2_rdma_auto_clk_gt_en_START    (1)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch2_rdma_auto_clk_gt_en_END      (1)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch3_rdma_auto_clk_gt_en_START    (2)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch3_rdma_auto_clk_gt_en_END      (2)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch4_rdma_auto_clk_gt_en_START    (3)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch4_rdma_auto_clk_gt_en_END      (3)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch5_rdma_auto_clk_gt_en_START    (4)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch5_rdma_auto_clk_gt_en_END      (4)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch6_rdma_auto_clk_gt_en_START    (5)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch6_rdma_auto_clk_gt_en_END      (5)
#define SOC_ADE_AUTO_CLK_GT_EN0_disp_rdma_auto_clk_gt_en_START   (6)
#define SOC_ADE_AUTO_CLK_GT_EN0_disp_rdma_auto_clk_gt_en_END     (6)
#define SOC_ADE_AUTO_CLK_GT_EN0_cmdq1_rdma_auto_clk_gt_en_START  (7)
#define SOC_ADE_AUTO_CLK_GT_EN0_cmdq1_rdma_auto_clk_gt_en_END    (7)
#define SOC_ADE_AUTO_CLK_GT_EN0_cmdq2_rdma_auto_clk_gt_en_START  (8)
#define SOC_ADE_AUTO_CLK_GT_EN0_cmdq2_rdma_auto_clk_gt_en_END    (8)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch1_wdma_auto_clk_gt_en_START    (10)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch1_wdma_auto_clk_gt_en_END      (10)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch2_wdma_auto_clk_gt_en_START    (11)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch2_wdma_auto_clk_gt_en_END      (11)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch3_wdma_auto_clk_gt_en_START    (12)
#define SOC_ADE_AUTO_CLK_GT_EN0_ch3_wdma_auto_clk_gt_en_END      (12)
#define SOC_ADE_AUTO_CLK_GT_EN0_cmdq_wdma_auto_clk_gt_en_START   (14)
#define SOC_ADE_AUTO_CLK_GT_EN0_cmdq_wdma_auto_clk_gt_en_END     (14)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip1_auto_clk_gt_en_START       (15)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip1_auto_clk_gt_en_END         (15)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip2_auto_clk_gt_en_START       (16)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip2_auto_clk_gt_en_END         (16)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip3_auto_clk_gt_en_START       (17)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip3_auto_clk_gt_en_END         (17)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip4_auto_clk_gt_en_START       (18)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip4_auto_clk_gt_en_END         (18)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip5_auto_clk_gt_en_START       (19)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip5_auto_clk_gt_en_END         (19)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip6_auto_clk_gt_en_START       (20)
#define SOC_ADE_AUTO_CLK_GT_EN0_clip6_auto_clk_gt_en_END         (20)
#define SOC_ADE_AUTO_CLK_GT_EN0_scl1_auto_clk_gt_en_START        (21)
#define SOC_ADE_AUTO_CLK_GT_EN0_scl1_auto_clk_gt_en_END          (21)
#define SOC_ADE_AUTO_CLK_GT_EN0_scl2_auto_clk_gt_en_START        (22)
#define SOC_ADE_AUTO_CLK_GT_EN0_scl2_auto_clk_gt_en_END          (22)
#define SOC_ADE_AUTO_CLK_GT_EN0_scl3_auto_clk_gt_en_START        (23)
#define SOC_ADE_AUTO_CLK_GT_EN0_scl3_auto_clk_gt_en_END          (23)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran1_auto_clk_gt_en_START      (24)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran1_auto_clk_gt_en_END        (24)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran2_auto_clk_gt_en_START      (25)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran2_auto_clk_gt_en_END        (25)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran3_auto_clk_gt_en_START      (26)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran3_auto_clk_gt_en_END        (26)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran4_auto_clk_gt_en_START      (27)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran4_auto_clk_gt_en_END        (27)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran5_auto_clk_gt_en_START      (28)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran5_auto_clk_gt_en_END        (28)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran6_auto_clk_gt_en_START      (29)
#define SOC_ADE_AUTO_CLK_GT_EN0_ctran6_auto_clk_gt_en_END        (29)
#define SOC_ADE_AUTO_CLK_GT_EN0_rot_auto_clk_gt_en_START         (30)
#define SOC_ADE_AUTO_CLK_GT_EN0_rot_auto_clk_gt_en_END           (30)


/*****************************************************************************
 结构名    : SOC_ADE_AUTO_CLK_GT_EN1_UNION
 结构说明  : AUTO_CLK_GT_EN1 寄存器结构定义。地址偏移量:0x00A0，初值:0x000001FF，宽度:32
 寄存器说明: ADE子模块自动时钟门控控制寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cmdq1_auto_clk_gt_en  : 1;  /* bit[0]   : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                0：自动时钟门控禁用；
                                                                1：自动时钟门控使能； */
        unsigned int  cmdq2_auto_clk_gt_en  : 1;  /* bit[1]   : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                0：自动时钟门控禁用；
                                                                1：自动时钟门控使能； */
        unsigned int  reserved_0            : 1;  /* bit[2]   : 保留。 */
        unsigned int  gamma_auto_clk_gt_en  : 1;  /* bit[3]   : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                0：自动时钟门控禁用；
                                                                1：自动时钟门控使能； */
        unsigned int  dither_auto_clk_gt_en : 1;  /* bit[4]   : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                0：自动时钟门控禁用；
                                                                1：自动时钟门控使能； */
        unsigned int  ovly1_auto_clk_gt_en  : 1;  /* bit[5]   : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                0：自动时钟门控禁用；
                                                                1：自动时钟门控使能； */
        unsigned int  ovly2_auto_clk_gt_en  : 1;  /* bit[6]   : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                0：自动时钟门控禁用；
                                                                1：自动时钟门控使能； */
        unsigned int  ovly3_auto_clk_gt_en  : 1;  /* bit[7]   : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                0：自动时钟门控禁用；
                                                                1：自动时钟门控使能； */
        unsigned int  top_auto_clk_gt_en    : 1;  /* bit[8]   : 子模块自动时钟门控使能，只有全局自动时钟门控使能时才有效：
                                                                0：自动时钟门控禁用；
                                                                1：自动时钟门控使能； */
        unsigned int  reserved_1            : 23; /* bit[9-31]: 保留。 */
    } reg;
} SOC_ADE_AUTO_CLK_GT_EN1_UNION;
#endif
#define SOC_ADE_AUTO_CLK_GT_EN1_cmdq1_auto_clk_gt_en_START   (0)
#define SOC_ADE_AUTO_CLK_GT_EN1_cmdq1_auto_clk_gt_en_END     (0)
#define SOC_ADE_AUTO_CLK_GT_EN1_cmdq2_auto_clk_gt_en_START   (1)
#define SOC_ADE_AUTO_CLK_GT_EN1_cmdq2_auto_clk_gt_en_END     (1)
#define SOC_ADE_AUTO_CLK_GT_EN1_gamma_auto_clk_gt_en_START   (3)
#define SOC_ADE_AUTO_CLK_GT_EN1_gamma_auto_clk_gt_en_END     (3)
#define SOC_ADE_AUTO_CLK_GT_EN1_dither_auto_clk_gt_en_START  (4)
#define SOC_ADE_AUTO_CLK_GT_EN1_dither_auto_clk_gt_en_END    (4)
#define SOC_ADE_AUTO_CLK_GT_EN1_ovly1_auto_clk_gt_en_START   (5)
#define SOC_ADE_AUTO_CLK_GT_EN1_ovly1_auto_clk_gt_en_END     (5)
#define SOC_ADE_AUTO_CLK_GT_EN1_ovly2_auto_clk_gt_en_START   (6)
#define SOC_ADE_AUTO_CLK_GT_EN1_ovly2_auto_clk_gt_en_END     (6)
#define SOC_ADE_AUTO_CLK_GT_EN1_ovly3_auto_clk_gt_en_START   (7)
#define SOC_ADE_AUTO_CLK_GT_EN1_ovly3_auto_clk_gt_en_END     (7)
#define SOC_ADE_AUTO_CLK_GT_EN1_top_auto_clk_gt_en_START     (8)
#define SOC_ADE_AUTO_CLK_GT_EN1_top_auto_clk_gt_en_END       (8)


/*****************************************************************************
 结构名    : SOC_ADE_FRM_DISGARD_CTRL_UNION
 结构说明  : FRM_DISGARD_CTRL 寄存器结构定义。地址偏移量:0x00A4，初值:0x00000040，宽度:32
 寄存器说明: ADE丢帧控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dfs_buf_unflow_lev0 : 12; /* bit[0-11] : DFS buffer下溢预警水线0，默认值为64，最高不超过分配的DFS buffer大小。当送往LCD显示的数据同时回写至DDR，且DFS buffer的填充深度低于此水线，而指定WDMA的FIFO满时，为了防止WDMA堵塞显示，需要丢帧。 */
        unsigned int  reserved_0          : 4;  /* bit[12-15]: 保留。 */
        unsigned int  wdma2_disgard_en    : 1;  /* bit[16]   : WDMA2丢帧使能，当显示数据同时通过此WDMA写回DDR时，需要使能。
                                                               0：禁用；
                                                               1：使能。 */
        unsigned int  wdma3_disgard_en    : 1;  /* bit[17]   : WDMA3丢帧使能，当显示数据同时通过此WDMA写回DDR时，需要使能。
                                                               0：禁用；
                                                               1：使能。 */
        unsigned int  reserved_1          : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_ADE_FRM_DISGARD_CTRL_UNION;
#endif
#define SOC_ADE_FRM_DISGARD_CTRL_dfs_buf_unflow_lev0_START  (0)
#define SOC_ADE_FRM_DISGARD_CTRL_dfs_buf_unflow_lev0_END    (11)
#define SOC_ADE_FRM_DISGARD_CTRL_wdma2_disgard_en_START     (16)
#define SOC_ADE_FRM_DISGARD_CTRL_wdma2_disgard_en_END       (16)
#define SOC_ADE_FRM_DISGARD_CTRL_wdma3_disgard_en_START     (17)
#define SOC_ADE_FRM_DISGARD_CTRL_wdma3_disgard_en_END       (17)


/*****************************************************************************
 结构名    : SOC_ADE_FRM_DISGARD_CNT_UNION
 结构说明  : FRM_DISGARD_CNT 寄存器结构定义。地址偏移量:0x00A8，初值:0x00000000，宽度:32
 寄存器说明: ADE丢帧统计计数器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wdma2_disgard_cnt : 16; /* bit[0-15] : WDMA2丢帧统计计数器，每次发生丢帧时，计数器加1，软件写清零。 */
        unsigned int  wdma3_disgard_cnt : 16; /* bit[16-31]: WDMA3丢帧统计计数器，每次发生丢帧时，计数器加1，软件写清零。 */
    } reg;
} SOC_ADE_FRM_DISGARD_CNT_UNION;
#endif
#define SOC_ADE_FRM_DISGARD_CNT_wdma2_disgard_cnt_START  (0)
#define SOC_ADE_FRM_DISGARD_CNT_wdma2_disgard_cnt_END    (15)
#define SOC_ADE_FRM_DISGARD_CNT_wdma3_disgard_cnt_START  (16)
#define SOC_ADE_FRM_DISGARD_CNT_wdma3_disgard_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_ADE_RELOAD_DIS0_UNION
 结构说明  : RELOAD_DIS0 寄存器结构定义。地址偏移量:0x00AC，初值:0x00000000，宽度:32
 寄存器说明: 子模块硬件同步屏蔽寄存器0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_rdma_reload_dis   : 1;  /* bit[0] : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ch2_rdma_reload_dis   : 1;  /* bit[1] : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ch3_rdma_reload_dis   : 1;  /* bit[2] : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ch4_rdma_reload_dis   : 1;  /* bit[3] : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ch5_rdma_reload_dis   : 1;  /* bit[4] : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ch6_rdma_reload_dis   : 1;  /* bit[5] : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  disp_rdma_reload_dis  : 1;  /* bit[6] : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  cmdq1_rdma_reload_dis : 1;  /* bit[7] : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  cmdq2_rdma_reload_dis : 1;  /* bit[8] : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  reserved_0            : 1;  /* bit[9] : 保留。 */
        unsigned int  ch1_wdma_reload_dis   : 1;  /* bit[10]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ch2_wdma_reload_dis   : 1;  /* bit[11]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ch3_wdma_reload_dis   : 1;  /* bit[12]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  reserved_1            : 1;  /* bit[13]: 保留。 */
        unsigned int  cmdq_wdma_reload_dis  : 1;  /* bit[14]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  clip1_reload_dis      : 1;  /* bit[15]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  clip2_reload_dis      : 1;  /* bit[16]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  clip3_reload_dis      : 1;  /* bit[17]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  clip4_reload_dis      : 1;  /* bit[18]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  clip5_reload_dis      : 1;  /* bit[19]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  clip6_reload_dis      : 1;  /* bit[20]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  scl1_reload_dis       : 1;  /* bit[21]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  scl2_reload_dis       : 1;  /* bit[22]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  scl3_reload_dis       : 1;  /* bit[23]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ctran1_reload_dis     : 1;  /* bit[24]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ctran2_reload_dis     : 1;  /* bit[25]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ctran3_reload_dis     : 1;  /* bit[26]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ctran4_reload_dis     : 1;  /* bit[27]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ctran5_reload_dis     : 1;  /* bit[28]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ctran6_reload_dis     : 1;  /* bit[29]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  rot_reload_dis        : 1;  /* bit[30]: 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                              0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                              1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  reserved_2            : 1;  /* bit[31]: 保留。 */
    } reg;
} SOC_ADE_RELOAD_DIS0_UNION;
#endif
#define SOC_ADE_RELOAD_DIS0_ch1_rdma_reload_dis_START    (0)
#define SOC_ADE_RELOAD_DIS0_ch1_rdma_reload_dis_END      (0)
#define SOC_ADE_RELOAD_DIS0_ch2_rdma_reload_dis_START    (1)
#define SOC_ADE_RELOAD_DIS0_ch2_rdma_reload_dis_END      (1)
#define SOC_ADE_RELOAD_DIS0_ch3_rdma_reload_dis_START    (2)
#define SOC_ADE_RELOAD_DIS0_ch3_rdma_reload_dis_END      (2)
#define SOC_ADE_RELOAD_DIS0_ch4_rdma_reload_dis_START    (3)
#define SOC_ADE_RELOAD_DIS0_ch4_rdma_reload_dis_END      (3)
#define SOC_ADE_RELOAD_DIS0_ch5_rdma_reload_dis_START    (4)
#define SOC_ADE_RELOAD_DIS0_ch5_rdma_reload_dis_END      (4)
#define SOC_ADE_RELOAD_DIS0_ch6_rdma_reload_dis_START    (5)
#define SOC_ADE_RELOAD_DIS0_ch6_rdma_reload_dis_END      (5)
#define SOC_ADE_RELOAD_DIS0_disp_rdma_reload_dis_START   (6)
#define SOC_ADE_RELOAD_DIS0_disp_rdma_reload_dis_END     (6)
#define SOC_ADE_RELOAD_DIS0_cmdq1_rdma_reload_dis_START  (7)
#define SOC_ADE_RELOAD_DIS0_cmdq1_rdma_reload_dis_END    (7)
#define SOC_ADE_RELOAD_DIS0_cmdq2_rdma_reload_dis_START  (8)
#define SOC_ADE_RELOAD_DIS0_cmdq2_rdma_reload_dis_END    (8)
#define SOC_ADE_RELOAD_DIS0_ch1_wdma_reload_dis_START    (10)
#define SOC_ADE_RELOAD_DIS0_ch1_wdma_reload_dis_END      (10)
#define SOC_ADE_RELOAD_DIS0_ch2_wdma_reload_dis_START    (11)
#define SOC_ADE_RELOAD_DIS0_ch2_wdma_reload_dis_END      (11)
#define SOC_ADE_RELOAD_DIS0_ch3_wdma_reload_dis_START    (12)
#define SOC_ADE_RELOAD_DIS0_ch3_wdma_reload_dis_END      (12)
#define SOC_ADE_RELOAD_DIS0_cmdq_wdma_reload_dis_START   (14)
#define SOC_ADE_RELOAD_DIS0_cmdq_wdma_reload_dis_END     (14)
#define SOC_ADE_RELOAD_DIS0_clip1_reload_dis_START       (15)
#define SOC_ADE_RELOAD_DIS0_clip1_reload_dis_END         (15)
#define SOC_ADE_RELOAD_DIS0_clip2_reload_dis_START       (16)
#define SOC_ADE_RELOAD_DIS0_clip2_reload_dis_END         (16)
#define SOC_ADE_RELOAD_DIS0_clip3_reload_dis_START       (17)
#define SOC_ADE_RELOAD_DIS0_clip3_reload_dis_END         (17)
#define SOC_ADE_RELOAD_DIS0_clip4_reload_dis_START       (18)
#define SOC_ADE_RELOAD_DIS0_clip4_reload_dis_END         (18)
#define SOC_ADE_RELOAD_DIS0_clip5_reload_dis_START       (19)
#define SOC_ADE_RELOAD_DIS0_clip5_reload_dis_END         (19)
#define SOC_ADE_RELOAD_DIS0_clip6_reload_dis_START       (20)
#define SOC_ADE_RELOAD_DIS0_clip6_reload_dis_END         (20)
#define SOC_ADE_RELOAD_DIS0_scl1_reload_dis_START        (21)
#define SOC_ADE_RELOAD_DIS0_scl1_reload_dis_END          (21)
#define SOC_ADE_RELOAD_DIS0_scl2_reload_dis_START        (22)
#define SOC_ADE_RELOAD_DIS0_scl2_reload_dis_END          (22)
#define SOC_ADE_RELOAD_DIS0_scl3_reload_dis_START        (23)
#define SOC_ADE_RELOAD_DIS0_scl3_reload_dis_END          (23)
#define SOC_ADE_RELOAD_DIS0_ctran1_reload_dis_START      (24)
#define SOC_ADE_RELOAD_DIS0_ctran1_reload_dis_END        (24)
#define SOC_ADE_RELOAD_DIS0_ctran2_reload_dis_START      (25)
#define SOC_ADE_RELOAD_DIS0_ctran2_reload_dis_END        (25)
#define SOC_ADE_RELOAD_DIS0_ctran3_reload_dis_START      (26)
#define SOC_ADE_RELOAD_DIS0_ctran3_reload_dis_END        (26)
#define SOC_ADE_RELOAD_DIS0_ctran4_reload_dis_START      (27)
#define SOC_ADE_RELOAD_DIS0_ctran4_reload_dis_END        (27)
#define SOC_ADE_RELOAD_DIS0_ctran5_reload_dis_START      (28)
#define SOC_ADE_RELOAD_DIS0_ctran5_reload_dis_END        (28)
#define SOC_ADE_RELOAD_DIS0_ctran6_reload_dis_START      (29)
#define SOC_ADE_RELOAD_DIS0_ctran6_reload_dis_END        (29)
#define SOC_ADE_RELOAD_DIS0_rot_reload_dis_START         (30)
#define SOC_ADE_RELOAD_DIS0_rot_reload_dis_END           (30)


/*****************************************************************************
 结构名    : SOC_ADE_RELOAD_DIS1_UNION
 结构说明  : RELOAD_DIS1 寄存器结构定义。地址偏移量:0x00B0，初值:0x00000000，宽度:32
 寄存器说明: 子模块硬件同步屏蔽寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cmdq1_reload_dis  : 1;  /* bit[0]   : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                            0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                            1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  cmdq2_reload_dis  : 1;  /* bit[1]   : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                            0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                            1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  reserved_0        : 1;  /* bit[2]   : 保留。 */
        unsigned int  gamma_reload_dis  : 1;  /* bit[3]   : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                            0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                            1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  dither_reload_dis : 1;  /* bit[4]   : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                            0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                            1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ovly1_reload_dis  : 1;  /* bit[5]   : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                            0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                            1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ovly2_reload_dis  : 1;  /* bit[6]   : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                            0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                            1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  ovly3_reload_dis  : 1;  /* bit[7]   : 子模块硬件同步屏蔽，需要与ADE_SOFT_RST_SEL选择配合使用：
                                                            0：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后会重新reload，开始工作；
                                                            1：ADE_SOFT_RST_SEL对应的bit=1时，子模块软复位后不会重新reload和开始工作； */
        unsigned int  reserved_1        : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_ADE_RELOAD_DIS1_UNION;
#endif
#define SOC_ADE_RELOAD_DIS1_cmdq1_reload_dis_START   (0)
#define SOC_ADE_RELOAD_DIS1_cmdq1_reload_dis_END     (0)
#define SOC_ADE_RELOAD_DIS1_cmdq2_reload_dis_START   (1)
#define SOC_ADE_RELOAD_DIS1_cmdq2_reload_dis_END     (1)
#define SOC_ADE_RELOAD_DIS1_gamma_reload_dis_START   (3)
#define SOC_ADE_RELOAD_DIS1_gamma_reload_dis_END     (3)
#define SOC_ADE_RELOAD_DIS1_dither_reload_dis_START  (4)
#define SOC_ADE_RELOAD_DIS1_dither_reload_dis_END    (4)
#define SOC_ADE_RELOAD_DIS1_ovly1_reload_dis_START   (5)
#define SOC_ADE_RELOAD_DIS1_ovly1_reload_dis_END     (5)
#define SOC_ADE_RELOAD_DIS1_ovly2_reload_dis_START   (6)
#define SOC_ADE_RELOAD_DIS1_ovly2_reload_dis_END     (6)
#define SOC_ADE_RELOAD_DIS1_ovly3_reload_dis_START   (7)
#define SOC_ADE_RELOAD_DIS1_ovly3_reload_dis_END     (7)


/*****************************************************************************
 结构名    : SOC_ADE_EN_UNION
 结构说明  : EN 寄存器结构定义。地址偏移量:0x0100，初值:0x00000000，宽度:32
 寄存器说明: ADE全局使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ade_en   : 1;  /* bit[0]   : ADE全局使能，向此寄存器写入任何值都将触发一次Config OK：
                                                   0：禁用；
                                                   1：使能。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ADE_EN_UNION;
#endif
#define SOC_ADE_EN_ade_en_START    (0)
#define SOC_ADE_EN_ade_en_END      (0)






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

#endif /* end of soc_ade_interface.h */
