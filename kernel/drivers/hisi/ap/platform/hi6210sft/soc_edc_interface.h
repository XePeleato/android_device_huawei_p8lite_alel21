

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_EDC_INTERFACE_H__
#define __SOC_EDC_INTERFACE_H__

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
/* 寄存器说明：EDC版本信息
   位域定义UNION结构:  SOC_EDC_ID_UNION */
#define SOC_EDC_ID_ADDR(base)                         ((base) + (0x000))

/* 寄存器说明：Channel1左通道地址
   位域定义UNION结构:  SOC_EDC_CH1L_ADDR_UNION */
#define SOC_EDC_CH1L_ADDR_ADDR(base)                  ((base) + (0x004))

/* 寄存器说明：Channel1右通道地址
   位域定义UNION结构:  SOC_EDC_CH1R_ADDR_UNION */
#define SOC_EDC_CH1R_ADDR_ADDR(base)                  ((base) + (0x008))

/* 寄存器说明：Channel1行地址偏移
   位域定义UNION结构:  SOC_EDC_CH1_STRIDE_UNION */
#define SOC_EDC_CH1_STRIDE_ADDR(base)                 ((base) + (0x00C))

/* 寄存器说明：Channel1显示坐标寄存器
   位域定义UNION结构:  SOC_EDC_CH1_XY_UNION */
#define SOC_EDC_CH1_XY_ADDR(base)                     ((base) + (0x010))

/* 寄存器说明：Channel1图像尺寸
   位域定义UNION结构:  SOC_EDC_CH1_SIZE_UNION */
#define SOC_EDC_CH1_SIZE_ADDR(base)                   ((base) + (0x014))

/* 寄存器说明：Channel1控制
   位域定义UNION结构:  SOC_EDC_CH1_CTL_UNION */
#define SOC_EDC_CH1_CTL_ADDR(base)                    ((base) + (0x018))

/* 寄存器说明：Channel1关键色下限
   位域定义UNION结构:  SOC_EDC_CH1_COLORK_MIN_UNION */
#define SOC_EDC_CH1_COLORK_MIN_ADDR(base)             ((base) + (0x01C))

/* 寄存器说明：Channel1关键色上限
   位域定义UNION结构:  SOC_EDC_CH1_COLORK_MAX_UNION */
#define SOC_EDC_CH1_COLORK_MAX_ADDR(base)             ((base) + (0x020))

/* 寄存器说明：Channel2 Left地址
   位域定义UNION结构:  SOC_EDC_CH2L_ADDR_UNION */
#define SOC_EDC_CH2L_ADDR_ADDR(base)                  ((base) + (0x024))

/* 寄存器说明：Channel2 Right地址
   位域定义UNION结构:  SOC_EDC_CH2R_ADDR_UNION */
#define SOC_EDC_CH2R_ADDR_ADDR(base)                  ((base) + (0x028))

/* 寄存器说明：Channel2 Stride
   位域定义UNION结构:  SOC_EDC_CH2_STRIDE_UNION */
#define SOC_EDC_CH2_STRIDE_ADDR(base)                 ((base) + (0x02C))

/* 寄存器说明：Channel2坐标寄存器
   位域定义UNION结构:  SOC_EDC_CH2_XY_UNION */
#define SOC_EDC_CH2_XY_ADDR(base)                     ((base) + (0x030))

/* 寄存器说明：Channel2图像尺寸
   位域定义UNION结构:  SOC_EDC_CH2_SIZE_UNION */
#define SOC_EDC_CH2_SIZE_ADDR(base)                   ((base) + (0x034))

/* 寄存器说明：EDC_CH2_CTL
   位域定义UNION结构:  SOC_EDC_CH2_CTL_UNION */
#define SOC_EDC_CH2_CTL_ADDR(base)                    ((base) + (0x0038))

/* 寄存器说明：Channel2关键色下限
   位域定义UNION结构:  SOC_EDC_CH2_COLORK_MIN_UNION */
#define SOC_EDC_CH2_COLORK_MIN_ADDR(base)             ((base) + (0x003C))

/* 寄存器说明：Channel2关键色上限
   位域定义UNION结构:  SOC_EDC_CH2_COLORK_MAX_UNION */
#define SOC_EDC_CH2_COLORK_MAX_ADDR(base)             ((base) + (0x0040))

/* 寄存器说明：叠加控制
   位域定义UNION结构:  SOC_EDC_CH12_OVLY_UNION */
#define SOC_EDC_CH12_OVLY_ADDR(base)                  ((base) + (0x0044))

/* 寄存器说明：Channe1和Channel2进行Alpha Blending时，全局Alpha配置值
   位域定义UNION结构:  SOC_EDC_CH12_GLB_ALP_VAL_UNION */
#define SOC_EDC_CH12_GLB_ALP_VAL_ADDR(base)           ((base) + (0x0048))

/* 寄存器说明：Channel1与Channel2进行Blending时的屏蔽色0下限
   位域定义UNION结构:  SOC_EDC_CH12_MSK_COLOR0_MIN_UNION */
#define SOC_EDC_CH12_MSK_COLOR0_MIN_ADDR(base)        ((base) + (0x004C))

/* 寄存器说明：Channel1与Channel2进行Blending时的屏蔽色0上限
   位域定义UNION结构:  SOC_EDC_CH12_MSK_COLOR0_MAX_UNION */
#define SOC_EDC_CH12_MSK_COLOR0_MAX_ADDR(base)        ((base) + (0x0050))

/* 寄存器说明：Channel1与Channel2进行Blending时的屏蔽色1下限
   位域定义UNION结构:  SOC_EDC_CH12_MSK_COLOR1_MIN_UNION */
#define SOC_EDC_CH12_MSK_COLOR1_MIN_ADDR(base)        ((base) + (0x0054))

/* 寄存器说明：Channel1与Channel2进行Blending时的屏蔽色1上限
   位域定义UNION结构:  SOC_EDC_CH12_MSK_COLOR1_MAX_UNION */
#define SOC_EDC_CH12_MSK_COLOR1_MAX_ADDR(base)        ((base) + (0x0058))

/* 寄存器说明：Channel1与Channel2进行Blending时的屏蔽色2下限
   位域定义UNION结构:  SOC_EDC_CH12_MSK_COLOR2_MIN_UNION */
#define SOC_EDC_CH12_MSK_COLOR2_MIN_ADDR(base)        ((base) + (0x005C))

/* 寄存器说明：Channel1与Channel2进行Blending时的屏蔽色2上限
   位域定义UNION结构:  SOC_EDC_CH12_MSK_COLOR2_MAX_UNION */
#define SOC_EDC_CH12_MSK_COLOR2_MAX_ADDR(base)        ((base) + (0x0060))

/* 寄存器说明：Channel1与Channel2进行Blending时的屏蔽色3下限
   位域定义UNION结构:  SOC_EDC_CH12_MSK_COLOR3_MIN_UNION */
#define SOC_EDC_CH12_MSK_COLOR3_MIN_ADDR(base)        ((base) + (0x0064))

/* 寄存器说明：Channel1与Channel2进行Blending时的屏蔽色3上限
   位域定义UNION结构:  SOC_EDC_CH12_MSK_COLOR3_MAX_UNION */
#define SOC_EDC_CH12_MSK_COLOR3_MAX_ADDR(base)        ((base) + (0x0068))

/* 寄存器说明：Cursor左通道地址
   位域定义UNION结构:  SOC_EDC_CRSL_ADDR_UNION */
#define SOC_EDC_CRSL_ADDR_ADDR(base)                  ((base) + (0x006C))

/* 寄存器说明：Cursor右通道地址
   位域定义UNION结构:  SOC_EDC_CRSR_ADDR_UNION */
#define SOC_EDC_CRSR_ADDR_ADDR(base)                  ((base) + (0x0070))

/* 寄存器说明：Cursor Stride
   位域定义UNION结构:  SOC_EDC_CRS_STRIDE_UNION */
#define SOC_EDC_CRS_STRIDE_ADDR(base)                 ((base) + (0x0074))

/* 寄存器说明：Cursor坐标寄存器
   位域定义UNION结构:  SOC_EDC_CRS_XY_UNION */
#define SOC_EDC_CRS_XY_ADDR(base)                     ((base) + (0x0078))

/* 寄存器说明：Cursor图像尺寸
   位域定义UNION结构:  SOC_EDC_CRS_SIZE_UNION */
#define SOC_EDC_CRS_SIZE_ADDR(base)                   ((base) + (0x07C))

/* 寄存器说明：Cursor控制
   位域定义UNION结构:  SOC_EDC_CRS_CTL_UNION */
#define SOC_EDC_CRS_CTL_ADDR(base)                    ((base) + (0x0080))

/* 寄存器说明：Cursor关键色下限
   位域定义UNION结构:  SOC_EDC_CRS_COLORK_MIN_UNION */
#define SOC_EDC_CRS_COLORK_MIN_ADDR(base)             ((base) + (0x0084))

/* 寄存器说明：Cursor关键色上限
   位域定义UNION结构:  SOC_EDC_CRS_COLORK_MAX_UNION */
#define SOC_EDC_CRS_COLORK_MAX_ADDR(base)             ((base) + (0x0088))

/* 寄存器说明：Cursor和下层进行Alpha Blending时，全局Alpha配置值
   位域定义UNION结构:  SOC_EDC_CRS_GLB_ALP_VAL_UNION */
#define SOC_EDC_CRS_GLB_ALP_VAL_ADDR(base)            ((base) + (0x008C))

/* 寄存器说明：显示尺寸
   位域定义UNION结构:  SOC_EDC_DISP_SIZE_UNION */
#define SOC_EDC_DISP_SIZE_ADDR(base)                  ((base) + (0x090))

/* 寄存器说明：显示控制
   位域定义UNION结构:  SOC_EDC_DISP_CTL_UNION */
#define SOC_EDC_DISP_CTL_ADDR(base)                   ((base) + (0x094))

/* 寄存器说明：显示背景色
   位域定义UNION结构:  SOC_EDC_DISP_DPD_UNION */
#define SOC_EDC_DISP_DPD_ADDR(base)                   ((base) + (0x098))

/* 寄存器说明：显示状态
   位域定义UNION结构:  SOC_EDC_STS_UNION */
#define SOC_EDC_STS_ADDR(base)                        ((base) + (0x09C))

/* 寄存器说明：中断状态
   位域定义UNION结构:  SOC_EDC_INTS_UNION */
#define SOC_EDC_INTS_ADDR(base)                       ((base) + (0x0A0))

/* 寄存器说明：中断使能
   位域定义UNION结构:  SOC_EDC_INTE_UNION */
#define SOC_EDC_INTE_ADDR(base)                       ((base) + (0x0A4))

/* 寄存器说明：Gamma参数计数器清零
   位域定义UNION结构:  SOC_EDC_GAMCNT_CLR_UNION */
#define SOC_EDC_GAMCNT_CLR_ADDR(base)                 ((base) + (0x0A8))

/* 寄存器说明：Channel1 Tile存储图像的垂直高度
   位域定义UNION结构:  SOC_EDC_TILE_SIZE_VRT_UNION */
#define SOC_EDC_TILE_SIZE_VRT_ADDR(base)              ((base) + (0x0AC))

/* 寄存器说明：Channel1 Tile格式下的Crop起点坐标
   位域定义UNION结构:  SOC_EDC_TILE_CROP_UNION */
#define SOC_EDC_TILE_CROP_ADDR(base)                  ((base) + (0x0B0))

/* 寄存器说明：Tile Channel左通道Engine0地址
   位域定义UNION结构:  SOC_EDC_TL_ENGIN0_ADDR_UNION */
#define SOC_EDC_TL_ENGIN0_ADDR_ADDR(base)             ((base) + (0x0B4))

/* 寄存器说明：Tile Channel左通道Engine1地址
   位域定义UNION结构:  SOC_EDC_TL_ENGIN1_ADDR_UNION */
#define SOC_EDC_TL_ENGIN1_ADDR_ADDR(base)             ((base) + (0x0B8))

/* 寄存器说明：Tile Channel右通道Engine0地址
   位域定义UNION结构:  SOC_EDC_TR_ENGIN0_ADDR_UNION */
#define SOC_EDC_TR_ENGIN0_ADDR_ADDR(base)             ((base) + (0x0BC))

/* 寄存器说明：Tile Channel右通道Engine1地址
   位域定义UNION结构:  SOC_EDC_TR_ENGIN1_ADDR_UNION */
#define SOC_EDC_TR_ENGIN1_ADDR_ADDR(base)             ((base) + (0x0C0))

/* 寄存器说明：安全层中断状态寄存器
   位域定义UNION结构:  SOC_EDC_TRUST_INTS_UNION */
#define SOC_EDC_TRUST_INTS_ADDR(base)                 ((base) + (0x0D0))

/* 寄存器说明：安全层中断屏蔽寄存器
   位域定义UNION结构:  SOC_EDC_TRUST_MASK_UNION */
#define SOC_EDC_TRUST_MASK_ADDR(base)                 ((base) + (0x00D4))

/* 寄存器说明：各图层总线安全位配置寄存器
   位域定义UNION结构:  SOC_EDC_APROT_CFG_UNION */
#define SOC_EDC_APROT_CFG_ADDR(base)                  ((base) + (0x00D8))

/* 寄存器说明：Cursor层Clip配置寄存器
   位域定义UNION结构:  SOC_EDC_CRS_CLIP_UNION */
#define SOC_EDC_CRS_CLIP_ADDR(base)                   ((base) + (0x00DC))

/* 寄存器说明：Channel1图层Clip配置寄存器
   位域定义UNION结构:  SOC_EDC_CH1_CLIP_UNION */
#define SOC_EDC_CH1_CLIP_ADDR(base)                   ((base) + (0x00F0))

/* 寄存器说明：Channel2图层Clip配置寄存器
   位域定义UNION结构:  SOC_EDC_CH2_CLIP_UNION */
#define SOC_EDC_CH2_CLIP_ADDR(base)                   ((base) + (0x00F4))

/* 寄存器说明：Gamma参数寄存器(k=0~39)
   位域定义UNION结构:  SOC_EDC_GAMMA_UNION */
#define SOC_EDC_GAMMA_ADDR(base, k)                   ((base) + (0x100+(k)*0x4))

/* 寄存器说明：Channel1 CSC输入直流偏量寄存器
   位域定义UNION结构:  SOC_EDC_CH1_CSCIDC_UNION */
#define SOC_EDC_CH1_CSCIDC_ADDR(base)                 ((base) + (0x200))

/* 寄存器说明：Channel1 CSC输出直流偏量寄存器
   位域定义UNION结构:  SOC_EDC_CH1_CSCODC_UNION */
#define SOC_EDC_CH1_CSCODC_ADDR(base)                 ((base) + (0x204))

/* 寄存器说明：Channel1 CSC参数寄存器0
   位域定义UNION结构:  SOC_EDC_CH1_CSCP0_UNION */
#define SOC_EDC_CH1_CSCP0_ADDR(base)                  ((base) + (0x208))

/* 寄存器说明：Channel1 CSC参数寄存器1
   位域定义UNION结构:  SOC_EDC_CH1_CSCP1_UNION */
#define SOC_EDC_CH1_CSCP1_ADDR(base)                  ((base) + (0x20C))

/* 寄存器说明：Channel1 CSC参数寄存器2
   位域定义UNION结构:  SOC_EDC_CH1_CSCP2_UNION */
#define SOC_EDC_CH1_CSCP2_ADDR(base)                  ((base) + (0x210))

/* 寄存器说明：Channel1 CSC参数寄存器3
   位域定义UNION结构:  SOC_EDC_CH1_CSCP3_UNION */
#define SOC_EDC_CH1_CSCP3_ADDR(base)                  ((base) + (0x214))

/* 寄存器说明：Channel1 CSC参数寄存器4
   位域定义UNION结构:  SOC_EDC_CH1_CSCP4_UNION */
#define SOC_EDC_CH1_CSCP4_ADDR(base)                  ((base) + (0x218))

/* 寄存器说明：Channel2 CSC输入直流偏量寄存器
   位域定义UNION结构:  SOC_EDC_CH2_CSCIDC_UNION */
#define SOC_EDC_CH2_CSCIDC_ADDR(base)                 ((base) + (0x300))

/* 寄存器说明：Channel2 CSC输出直流偏量寄存器
   位域定义UNION结构:  SOC_EDC_CH2_CSCODC_UNION */
#define SOC_EDC_CH2_CSCODC_ADDR(base)                 ((base) + (0x304))

/* 寄存器说明：Channel2 CSC参数寄存器0
   位域定义UNION结构:  SOC_EDC_CH2_CSCP0_UNION */
#define SOC_EDC_CH2_CSCP0_ADDR(base)                  ((base) + (0x308))

/* 寄存器说明：Channel2 CSC参数寄存器1
   位域定义UNION结构:  SOC_EDC_CH2_CSCP1_UNION */
#define SOC_EDC_CH2_CSCP1_ADDR(base)                  ((base) + (0x30C))

/* 寄存器说明：Channel2 CSC参数寄存器2
   位域定义UNION结构:  SOC_EDC_CH2_CSCP2_UNION */
#define SOC_EDC_CH2_CSCP2_ADDR(base)                  ((base) + (0x310))

/* 寄存器说明：Channel2 CSC参数寄存器3
   位域定义UNION结构:  SOC_EDC_CH2_CSCP3_UNION */
#define SOC_EDC_CH2_CSCP3_ADDR(base)                  ((base) + (0x314))

/* 寄存器说明：Channel2 CSC参数寄存器4
   位域定义UNION结构:  SOC_EDC_CH2_CSCP4_UNION */
#define SOC_EDC_CH2_CSCP4_ADDR(base)                  ((base) + (0x318))

/* 寄存器说明：输出CSC输入直流偏量寄存器
   位域定义UNION结构:  SOC_EDC_OUT_CSCIDC_UNION */
#define SOC_EDC_OUT_CSCIDC_ADDR(base)                 ((base) + (0x400))

/* 寄存器说明：输出CSC输出直流偏量寄存器
   位域定义UNION结构:  SOC_EDC_OUT_CSCODC_UNION */
#define SOC_EDC_OUT_CSCODC_ADDR(base)                 ((base) + (0x404))

/* 寄存器说明：输出CSC参数寄存器0
   位域定义UNION结构:  SOC_EDC_OUT_CSCP0_UNION */
#define SOC_EDC_OUT_CSCP0_ADDR(base)                  ((base) + (0x408))

/* 寄存器说明：输出CSC参数寄存器1
   位域定义UNION结构:  SOC_EDC_OUT_CSCP1_UNION */
#define SOC_EDC_OUT_CSCP1_ADDR(base)                  ((base) + (0x40C))

/* 寄存器说明：输出CSC参数寄存器2
   位域定义UNION结构:  SOC_EDC_OUT_CSCP2_UNION */
#define SOC_EDC_OUT_CSCP2_ADDR(base)                  ((base) + (0x410))

/* 寄存器说明：输出CSC参数寄存器3
   位域定义UNION结构:  SOC_EDC_OUT_CSCP3_UNION */
#define SOC_EDC_OUT_CSCP3_ADDR(base)                  ((base) + (0x414))

/* 寄存器说明：输出CSC参数寄存器4
   位域定义UNION结构:  SOC_EDC_OUT_CSCP4_UNION */
#define SOC_EDC_OUT_CSCP4_ADDR(base)                  ((base) + (0x418))

/* 寄存器说明：Scaler水平缩放控制寄存器
   位域定义UNION结构:  SOC_EDC_CH1_SCL_HSP_UNION */
#define SOC_EDC_CH1_SCL_HSP_ADDR(base)                ((base) + (0x500))

/* 寄存器说明：Scaler水平初始相移寄存器
   位域定义UNION结构:  SOC_EDC_CH1_SCL_HOFFSET_UNION */
#define SOC_EDC_CH1_SCL_HOFFSET_ADDR(base)            ((base) + (0x504))

/* 寄存器说明：Scaler垂直缩放控制寄存器
   位域定义UNION结构:  SOC_EDC_CH1_SCL_VSP_UNION */
#define SOC_EDC_CH1_SCL_VSP_ADDR(base)                ((base) + (0x508))

/* 寄存器说明：Scaler垂直缩放比例寄存器
   位域定义UNION结构:  SOC_EDC_CH1_SCL_VSR_UNION */
#define SOC_EDC_CH1_SCL_VSR_ADDR(base)                ((base) + (0x50C))

/* 寄存器说明：Scaler垂直初始相移寄存器
   位域定义UNION结构:  SOC_EDC_CH1_SCL_VOFFSET_UNION */
#define SOC_EDC_CH1_SCL_VOFFSET_ADDR(base)            ((base) + (0x510))

/* 寄存器说明：Scaler输出图像尺寸寄存器
   位域定义UNION结构:  SOC_EDC_CH1_SCL_ORES_UNION */
#define SOC_EDC_CH1_SCL_ORES_ADDR(base)               ((base) + (0x514))

/* 寄存器说明：Scaler输入图像尺寸寄存器
   位域定义UNION结构:  SOC_EDC_CH1_SCL_IRES_UNION */
#define SOC_EDC_CH1_SCL_IRES_ADDR(base)               ((base) + (0x518))

/* 寄存器说明：Scaler水平滤波参数寄存器（m=1~8;i=1~4）
   位域定义UNION结构:  SOC_EDC_CH1_SCL_HPC_UNION */
#define SOC_EDC_CH1_SCL_HPC_ADDR(base, m, i)          ((base) + (0x600+((m)-1)*0x10+((i)-1)*0x4))

/* 寄存器说明：Scaler垂直滤波参数寄存器（n=1~16;i=1~2）
   位域定义UNION结构:  SOC_EDC_CH1_SCL_VPC_UNION */
#define SOC_EDC_CH1_SCL_VPC_ADDR(base, n, j)          ((base) + (0x700+((n)-1)*0x8+((j)-1)*0x4))

/* 寄存器说明：LDI水平扫描控制寄存器0
   位域定义UNION结构:  SOC_EDC_LDI_HRZ_CTRL0_UNION */
#define SOC_EDC_LDI_HRZ_CTRL0_ADDR(base)              ((base) + (0x1000))

/* 寄存器说明：LDI水平扫描寄存器1
   位域定义UNION结构:  SOC_EDC_LDI_HRZ_CTRL1_UNION */
#define SOC_EDC_LDI_HRZ_CTRL1_ADDR(base)              ((base) + (0x1004))

/* 寄存器说明：LDI垂直扫描控制寄存器0
   位域定义UNION结构:  SOC_EDC_LDI_VRT_CTRL0_UNION */
#define SOC_EDC_LDI_VRT_CTRL0_ADDR(base)              ((base) + (0x1008))

/* 寄存器说明：LDI垂直扫描控制寄存器1
   位域定义UNION结构:  SOC_EDC_LDI_VRT_CTRL1_UNION */
#define SOC_EDC_LDI_VRT_CTRL1_ADDR(base)              ((base) + (0x100C))

/* 寄存器说明：LDI信号极性控制寄存器
   位域定义UNION结构:  SOC_EDC_LDI_PLR_CTRL_UNION */
#define SOC_EDC_LDI_PLR_CTRL_ADDR(base)               ((base) + (0x1010))

/* 寄存器说明：LDI显示屏尺寸寄存器
   位域定义UNION结构:  SOC_EDC_LDI_DSP_SIZE_UNION */
#define SOC_EDC_LDI_DSP_SIZE_ADDR(base)               ((base) + (0x1014))

/* 寄存器说明：LDI 3D显示控制寄存器
   位域定义UNION结构:  SOC_EDC_LDI_3D_CTRL_UNION */
#define SOC_EDC_LDI_3D_CTRL_ADDR(base)                ((base) + (0x1018))

/* 寄存器说明：LDI中断屏蔽寄存器
   位域定义UNION结构:  SOC_EDC_LDI_INT_EN_UNION */
#define SOC_EDC_LDI_INT_EN_ADDR(base)                 ((base) + (0x101C))

/* 寄存器说明：LDI控制寄存器
   位域定义UNION结构:  SOC_EDC_LDI_CTRL_UNION */
#define SOC_EDC_LDI_CTRL_ADDR(base)                   ((base) + (0x1020))

/* 寄存器说明：LDI原始中断状态寄存器
   位域定义UNION结构:  SOC_EDC_LDI_ORG_INT_UNION */
#define SOC_EDC_LDI_ORG_INT_ADDR(base)                ((base) + (0x1024))

/* 寄存器说明：LDI屏蔽后的中断状态寄存器
   位域定义UNION结构:  SOC_EDC_LDI_MSK_INT_UNION */
#define SOC_EDC_LDI_MSK_INT_ADDR(base)                ((base) + (0x1028))

/* 寄存器说明：LDI中断清除寄存器
   位域定义UNION结构:  SOC_EDC_LDI_INT_CLR_UNION */
#define SOC_EDC_LDI_INT_CLR_ADDR(base)                ((base) + (0x102C))

/* 寄存器说明：LDI回写控制寄存器
   位域定义UNION结构:  SOC_EDC_LDI_WORK_MODE_UNION */
#define SOC_EDC_LDI_WORK_MODE_ADDR(base)              ((base) + (0x1030))

/* 寄存器说明：HDMI/DSI像素时钟门控寄存器
   位域定义UNION结构:  SOC_EDC_LDI_HDMI_DSI_GT_UNION */
#define SOC_EDC_LDI_HDMI_DSI_GT_ADDR(base)            ((base) + (0x1034))

/* 寄存器说明：3D frame by frame格式下active space阶段DE信号有效控制寄存器
   位域定义UNION结构:  SOC_EDC_LDI_DE_SPACE_LOW_UNION */
#define SOC_EDC_LDI_DE_SPACE_LOW_ADDR(base)           ((base) + (0x1038))

/* 寄存器说明：DSI Command(CMD)模式控制寄存器
   位域定义UNION结构:  SOC_EDC_DSI_CMD_MOD_CTRL_UNION */
#define SOC_EDC_DSI_CMD_MOD_CTRL_ADDR(base)           ((base) + (0x103C))

/* 寄存器说明：DSI Command(CMD)模式TE控制寄存器
   位域定义UNION结构:  SOC_EDC_DSI_TE_CTRL_UNION */
#define SOC_EDC_DSI_TE_CTRL_ADDR(base)                ((base) + (0x1040))

/* 寄存器说明：DSI响应行配置寄存器控制寄存器
   位域定义UNION结构:  SOC_EDC_DSI_TE_HS_NUM_UNION */
#define SOC_EDC_DSI_TE_HS_NUM_ADDR(base)              ((base) + (0x1044))

/* 寄存器说明：DSI TE PIN模式检测Hsync宽度控制寄存器
   位域定义UNION结构:  SOC_EDC_DSI_TE_HS_WD_UNION */
#define SOC_EDC_DSI_TE_HS_WD_ADDR(base)               ((base) + (0x1048))

/* 寄存器说明：DSI TE PIN模式检测Vsync宽度控制寄存器
   位域定义UNION结构:  SOC_EDC_DSI_TE_VS_WD_UNION */
#define SOC_EDC_DSI_TE_VS_WD_ADDR(base)               ((base) + (0x104C))

/* 寄存器说明：给MCU的中断状态寄存器
   位域定义UNION结构:  SOC_EDC_LDI_MCU_INTS_UNION */
#define SOC_EDC_LDI_MCU_INTS_ADDR(base)               ((base) + (0x1050))

/* 寄存器说明：给MCU的中断屏蔽寄存器
   位域定义UNION结构:  SOC_EDC_LDI_MCU_INTE_UNION */
#define SOC_EDC_LDI_MCU_INTE_ADDR(base)               ((base) + (0x1054))

/* 寄存器说明：给MCU的中断清零寄存器
   位域定义UNION结构:  SOC_EDC_LDI_MCU_INTC_UNION */
#define SOC_EDC_LDI_MCU_INTC_ADDR(base)               ((base) + (0x1058))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_CONTROL_REG0_UNION */
#define SOC_EDC_CONTROL_REG0_ADDR(base)               ((base) + (0x2000))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_CONTROL_REG1_UNION */
#define SOC_EDC_CONTROL_REG1_ADDR(base)               ((base) + (0x2004))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_HS_POS0_UNION */
#define SOC_EDC_HS_POS0_ADDR(base)                    ((base) + (0x2008))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_HS_POS1_UNION */
#define SOC_EDC_HS_POS1_ADDR(base)                    ((base) + (0x200C))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_FRAME_WIDTH0_UNION */
#define SOC_EDC_FRAME_WIDTH0_ADDR(base)               ((base) + (0x2010))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_FRAME_WIDTH1_UNION */
#define SOC_EDC_FRAME_WIDTH1_ADDR(base)               ((base) + (0x2014))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_FRAME_HEIGHT0_UNION */
#define SOC_EDC_FRAME_HEIGHT0_ADDR(base)              ((base) + (0x2018))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_FRAME_HEIGHT1_UNION */
#define SOC_EDC_FRAME_HEIGHT1_ADDR(base)              ((base) + (0x201C))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_VS_POS0_UNION */
#define SOC_EDC_VS_POS0_ADDR(base)                    ((base) + (0x2020))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_VS_POS1_UNION */
#define SOC_EDC_VS_POS1_ADDR(base)                    ((base) + (0x2024))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_IRIDIX_CONTROL_0_UNION */
#define SOC_EDC_IRIDIX_CONTROL_0_ADDR(base)           ((base) + (0x2400))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_VARIANCE_UNION */
#define SOC_EDC_VARIANCE_ADDR(base)                   ((base) + (0x240C))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_SLOPE_MAX_UNION */
#define SOC_EDC_SLOPE_MAX_ADDR(base)                  ((base) + (0x2410))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_SLOPE_MIN_UNION */
#define SOC_EDC_SLOPE_MIN_ADDR(base)                  ((base) + (0x2414))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_BLACK_LEVEL0_UNION */
#define SOC_EDC_BLACK_LEVEL0_ADDR(base)               ((base) + (0x2418))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_BLACK_LEVEL1_UNION */
#define SOC_EDC_BLACK_LEVEL1_ADDR(base)               ((base) + (0x241C))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_WHITE_LEVEL0_UNION */
#define SOC_EDC_WHITE_LEVEL0_ADDR(base)               ((base) + (0x2420))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_WHITE_LEVEL1_UNION */
#define SOC_EDC_WHITE_LEVEL1_ADDR(base)               ((base) + (0x2424))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_LIMIT_AMP_UNION */
#define SOC_EDC_LIMIT_AMP_ADDR(base)                  ((base) + (0x2428))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_DITHER_UNION */
#define SOC_EDC_DITHER_ADDR(base)                     ((base) + (0x242C))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_LOGO_LEFT_UNION */
#define SOC_EDC_LOGO_LEFT_ADDR(base)                  ((base) + (0x2800))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_LOGO_RIGHT_UNION */
#define SOC_EDC_LOGO_RIGHT_ADDR(base)                 ((base) + (0x2804))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_DITHER_CONTROL_UNION */
#define SOC_EDC_DITHER_CONTROL_ADDR(base)             ((base) + (0x2840))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_STRENGTH_SEL_UNION */
#define SOC_EDC_STRENGTH_SEL_ADDR(base)               ((base) + (0x2C00))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_STRENGTH_MANUAL_UNION */
#define SOC_EDC_STRENGTH_MANUAL_ADDR(base)            ((base) + (0x2C04))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_OPTION_SELECT_UNION */
#define SOC_EDC_OPTION_SELECT_ADDR(base)              ((base) + (0x2C08))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_AMBIENT_LIGHT0_UNION */
#define SOC_EDC_AMBIENT_LIGHT0_ADDR(base)             ((base) + (0x2C10))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_AMBIENT_LIGHT1_UNION */
#define SOC_EDC_AMBIENT_LIGHT1_ADDR(base)             ((base) + (0x2C14))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_BACKLIGHT_LEVEL0_UNION */
#define SOC_EDC_BACKLIGHT_LEVEL0_ADDR(base)           ((base) + (0x2C18))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_BACKLIGHT_LEVEL1_UNION */
#define SOC_EDC_BACKLIGHT_LEVEL1_ADDR(base)           ((base) + (0x2C1C))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_BACKLIGHT_MAX0_UNION */
#define SOC_EDC_BACKLIGHT_MAX0_ADDR(base)             ((base) + (0x2C20))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_BACKLIGHT_MAX1_UNION */
#define SOC_EDC_BACKLIGHT_MAX1_ADDR(base)             ((base) + (0x2C24))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_STRENGTH_LIMIT_UNION */
#define SOC_EDC_STRENGTH_LIMIT_ADDR(base)             ((base) + (0x2C28))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_CALIBRATION_A_UNION */
#define SOC_EDC_CALIBRATION_A_ADDR(base)              ((base) + (0x2C2C))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_CALIBRATION_B_UNION */
#define SOC_EDC_CALIBRATION_B_ADDR(base)              ((base) + (0x2C30))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_DRC_IN0_UNION */
#define SOC_EDC_DRC_IN0_ADDR(base)                    ((base) + (0x2C38))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_DRC_IN1_UNION */
#define SOC_EDC_DRC_IN1_ADDR(base)                    ((base) + (0x2C3C))

/* 寄存器说明：
   位域定义UNION结构:  SOC_EDC_T_FILT_CONTROL_UNION */
#define SOC_EDC_T_FILT_CONTROL_ADDR(base)             ((base) + (0x2C34))





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
 结构名    : SOC_EDC_ID_UNION
 结构说明  : ID 寄存器结构定义。地址偏移量:0x000，初值:0x00000300，宽度:32
 寄存器说明: EDC版本信息
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  id : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_EDC_ID_UNION;
#endif
#define SOC_EDC_ID_id_START  (0)
#define SOC_EDC_ID_id_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH1L_ADDR_UNION
 结构说明  : CH1L_ADDR 寄存器结构定义。地址偏移量:0x004，初值:0x00000000，宽度:32
 寄存器说明: Channel1左通道地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1l_addr : 32; /* bit[0-31]: Channel1(左眼)图像在内存中的存储地址(128bit对齐，最低4位恒为0)：
                                                    0度90度旋转： 首行首像素的存储地址；
                                                    180度270度旋转： 图像左下角像素的存储地址
                                                    注：当Channel1读入2D图像时，那么只需配置CH1L_ADDR，
                                                    CH1R_ADDR不起作用 */
    } reg;
} SOC_EDC_CH1L_ADDR_UNION;
#endif
#define SOC_EDC_CH1L_ADDR_ch1l_addr_START  (0)
#define SOC_EDC_CH1L_ADDR_ch1l_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH1R_ADDR_UNION
 结构说明  : CH1R_ADDR 寄存器结构定义。地址偏移量:0x008，初值:0x00000000，宽度:32
 寄存器说明: Channel1右通道地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1r_addr : 32; /* bit[0-31]: Channel1(右眼)图像在内存中的存储地址(128bit对齐，最低4位恒为0)：
                                                    0度90度旋转： 首行首像素的存储地址；
                                                    180度270度旋转： 图像左下角像素的存储地址
                                                    注：当Channel1读入2D图像时，那么只需配置CH1L_ADDR，
                                                    CH1R_ADDR不起作用 */
    } reg;
} SOC_EDC_CH1R_ADDR_UNION;
#endif
#define SOC_EDC_CH1R_ADDR_ch1r_addr_START  (0)
#define SOC_EDC_CH1R_ADDR_ch1r_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_STRIDE_UNION
 结构说明  : CH1_STRIDE 寄存器结构定义。地址偏移量:0x00C，初值:0x00000000，宽度:32
 寄存器说明: Channel1行地址偏移
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_ln_stride : 14; /* bit[0-13] : Channel1行地址偏移量(128bit对齐，最低4位恒为0)。
                                                         行地址偏移量为图像在内存中相邻两行首像素存储地址之差,Stride的步长为8（以像素为单位） */
        unsigned int  reserved      : 18; /* bit[14-31]: 保留 */
    } reg;
} SOC_EDC_CH1_STRIDE_UNION;
#endif
#define SOC_EDC_CH1_STRIDE_ch1_ln_stride_START  (0)
#define SOC_EDC_CH1_STRIDE_ch1_ln_stride_END    (13)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_XY_UNION
 结构说明  : CH1_XY 寄存器结构定义。地址偏移量:0x010，初值:0x00000000，宽度:32
 寄存器说明: Channel1显示坐标寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_y    : 12; /* bit[0-11] : Channel1图像左上角像素的Y坐标 (以像素为单位)
                                                    注： Column By Column/Line By Line格式时指的是拼接后图像的左上角,3D Top To Bottom格式时指的是左/右图像在上半部分/下半部分的坐标，3D Side By Side格式时指的是左/右图像在左半部分/右半部分的坐标 */
        unsigned int  reserved_0: 4;  /* bit[12-15]: 保留 */
        unsigned int  ch1_x    : 12; /* bit[16-27]: Channel1图像左上角像素的X坐标 (以像素为单位)
                                                    注：3D Column By Column/Line By Line格式时指的是拼接后图像的左上角,3D Top To Bottom格式时指的是左/右图像在上半部分/下半部分的坐标，3D Side By Side格式时指的是左/右图像在左半部分/右半部分的坐标 */
        unsigned int  reserved_1: 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_CH1_XY_UNION;
#endif
#define SOC_EDC_CH1_XY_ch1_y_START     (0)
#define SOC_EDC_CH1_XY_ch1_y_END       (11)
#define SOC_EDC_CH1_XY_ch1_x_START     (16)
#define SOC_EDC_CH1_XY_ch1_x_END       (27)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SIZE_UNION
 结构说明  : CH1_SIZE 寄存器结构定义。地址偏移量:0x014，初值:0x00000000，宽度:32
 寄存器说明: Channel1图像尺寸
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_size_vrt : 12; /* bit[0-11] : Channel1读入左/右眼图像的像素高度(垂平方向)-1(以像素为单位)
                                                        注： 
                                                         1，CH1_SIZE_VRT的步长为2；
                                                         2，在左眼图像超出显示下边界的时候，该字段应配置为：显示分辨率高度与左眼图像起点Y坐标的差值对2向上取整-1；
                                                         3，旋转下该值仍应配置为旋转前原始图像的读入像素高度。 */
        unsigned int  reserved_0   : 4;  /* bit[12-15]: 保留 */
        unsigned int  ch1_size_hrz : 12; /* bit[16-27]: Channel1读入左/右眼图像的像素宽度(水平方向)-1(以像素为单位)
                                                        注： 
                                                         1，CH1_SIZE_HRZ的步长为8；
                                                         2，在左眼图像超出显示右边界的时候，该字段应配置为：显示分辨率宽度与左眼图像起点X坐标的差值对8向上取整-1；
                                                         3，旋转下该值仍应配置为旋转前原始图像的读入像素宽度。 */
        unsigned int  reserved_1   : 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_CH1_SIZE_UNION;
#endif
#define SOC_EDC_CH1_SIZE_ch1_size_vrt_START  (0)
#define SOC_EDC_CH1_SIZE_ch1_size_vrt_END    (11)
#define SOC_EDC_CH1_SIZE_ch1_size_hrz_START  (16)
#define SOC_EDC_CH1_SIZE_ch1_size_hrz_END    (27)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_CTL_UNION
 结构说明  : CH1_CTL 寄存器结构定义。地址偏移量:0x018，初值:0x00000000，宽度:32
 寄存器说明: Channel1控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_secu_line : 12; /* bit[0-11] : Channel1的刷新安全区起始行号-1：
                                                         当EDC开始读完该寄存器指定的行，发起CH1_LINE_INT中断 */
        unsigned int  ch1_y_flip    : 1;  /* bit[12]   : Y坐标是否反转：(仅Channel1为Tile格式时有效)
                                                         0： Y坐标不反转
                                                         1： Y坐标反转 */
        unsigned int  ch1_tile_fmt  : 2;  /* bit[13-14]: Channel1图像Tile格式类型：
                                                         00： tile
                                                         01： super tile 
                                                         10： multi tile
                                                         11： super+multi tile */
        unsigned int  ch1_tile_en   : 1;  /* bit[15]   : Channel1输入图像存储格式：
                                                         0： Linear格式
                                                         1： Tile格式 */
        unsigned int  ch1_pix_fmt   : 3;  /* bit[16-18]: Channel1的输入像素格式：
                                                         000： ARGB1555
                                                         001： RGB565
                                                         010： RGB0888
                                                         011： ARGB8888
                                                         100： YUYV Packet
                                                         101： UYVY Packet 
                                                         110： YVYU Packet
                                                         111： VYUY Packet */
        unsigned int  ch_bgr        : 1;  /* bit[19]   : 0： Channel1输入像素为RGB格式
                                                         1： Channel1输入像素为BGR格式 */
        unsigned int  ch1_colork_en : 1;  /* bit[20]   : Channel1 Color Key使能
                                                         0： Channel1 Color Key关闭
                                                         1： Channel1 Color Key使能 */
        unsigned int  ch1_rot       : 2;  /* bit[21-22]: Channel1旋转角度：
                                                         00： 无旋转；
                                                         01： 180度旋转；
                                                         10： 90度旋转；
                                                         11： 270度旋转； */
        unsigned int  reserved_0    : 1;  /* bit[23]   : 保留 */
        unsigned int  ch1_en        : 1;  /* bit[24]   : Channel1使能：
                                                         0： Channel1关闭
                                                         1： Channel1开启 */
        unsigned int  reserved_1    : 1;  /* bit[25]   : 保留 */
        unsigned int  ch1_clk_en    : 1;  /* bit[26]   : 通道切换Channel1 时钟使能：
                                                         0： Channel1时钟关闭
                                                         1： Channel1时钟开启 */
        unsigned int  reserved_2    : 5;  /* bit[27-31]: 保留 */
    } reg;
} SOC_EDC_CH1_CTL_UNION;
#endif
#define SOC_EDC_CH1_CTL_ch1_secu_line_START  (0)
#define SOC_EDC_CH1_CTL_ch1_secu_line_END    (11)
#define SOC_EDC_CH1_CTL_ch1_y_flip_START     (12)
#define SOC_EDC_CH1_CTL_ch1_y_flip_END       (12)
#define SOC_EDC_CH1_CTL_ch1_tile_fmt_START   (13)
#define SOC_EDC_CH1_CTL_ch1_tile_fmt_END     (14)
#define SOC_EDC_CH1_CTL_ch1_tile_en_START    (15)
#define SOC_EDC_CH1_CTL_ch1_tile_en_END      (15)
#define SOC_EDC_CH1_CTL_ch1_pix_fmt_START    (16)
#define SOC_EDC_CH1_CTL_ch1_pix_fmt_END      (18)
#define SOC_EDC_CH1_CTL_ch_bgr_START         (19)
#define SOC_EDC_CH1_CTL_ch_bgr_END           (19)
#define SOC_EDC_CH1_CTL_ch1_colork_en_START  (20)
#define SOC_EDC_CH1_CTL_ch1_colork_en_END    (20)
#define SOC_EDC_CH1_CTL_ch1_rot_START        (21)
#define SOC_EDC_CH1_CTL_ch1_rot_END          (22)
#define SOC_EDC_CH1_CTL_ch1_en_START         (24)
#define SOC_EDC_CH1_CTL_ch1_en_END           (24)
#define SOC_EDC_CH1_CTL_ch1_clk_en_START     (26)
#define SOC_EDC_CH1_CTL_ch1_clk_en_END       (26)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_COLORK_MIN_UNION
 结构说明  : CH1_COLORK_MIN 寄存器结构定义。地址偏移量:0x01C，初值:0x00000000，宽度:32
 寄存器说明: Channel1关键色下限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_colork_min : 24; /* bit[0-23] : Channel1读入图像关键色的下限，该字段的RGB格式与Channel1的输入RGB格式一致 */
        unsigned int  reserved       : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH1_COLORK_MIN_UNION;
#endif
#define SOC_EDC_CH1_COLORK_MIN_ch1_colork_min_START  (0)
#define SOC_EDC_CH1_COLORK_MIN_ch1_colork_min_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_COLORK_MAX_UNION
 结构说明  : CH1_COLORK_MAX 寄存器结构定义。地址偏移量:0x020，初值:0x00000000，宽度:32
 寄存器说明: Channel1关键色上限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_colork_max : 24; /* bit[0-23] : Channel1读入图像关键色的上限，该字段的RGB格式与Channel1的输入RGB格式一致 */
        unsigned int  reserved       : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH1_COLORK_MAX_UNION;
#endif
#define SOC_EDC_CH1_COLORK_MAX_ch1_colork_max_START  (0)
#define SOC_EDC_CH1_COLORK_MAX_ch1_colork_max_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH2L_ADDR_UNION
 结构说明  : CH2L_ADDR 寄存器结构定义。地址偏移量:0x024，初值:0x00000000，宽度:32
 寄存器说明: Channel2 Left地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2l_addr : 32; /* bit[0-31]: Channel2(左眼)图像在内存中的存储地址(128bit对齐，最低4位恒为0)：
                                                    首行首像素的存储地址。
                                                    注：当Channel2读入2D图像时，那么只需配置CH2L_ADDR，
                                                    CH2R_ADDR不起作用 */
    } reg;
} SOC_EDC_CH2L_ADDR_UNION;
#endif
#define SOC_EDC_CH2L_ADDR_ch2l_addr_START  (0)
#define SOC_EDC_CH2L_ADDR_ch2l_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH2R_ADDR_UNION
 结构说明  : CH2R_ADDR 寄存器结构定义。地址偏移量:0x028，初值:0x00000000，宽度:32
 寄存器说明: Channel2 Right地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2r_addr : 32; /* bit[0-31]: Channel2(右眼)图像在内存中的存储地址(128bit对齐，最低4位恒为0)：
                                                    首行首像素的存储地址。
                                                    注：当Channel2读入2D图像时，那么只需配置CH2L_ADDR，
                                                    CH2R_ADDR不起作用 */
    } reg;
} SOC_EDC_CH2R_ADDR_UNION;
#endif
#define SOC_EDC_CH2R_ADDR_ch2r_addr_START  (0)
#define SOC_EDC_CH2R_ADDR_ch2r_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_STRIDE_UNION
 结构说明  : CH2_STRIDE 寄存器结构定义。地址偏移量:0x02C，初值:0x00000000，宽度:32
 寄存器说明: Channel2 Stride
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_ln_stride : 14; /* bit[0-13] : Channel2行地址偏移量(128bit对齐，最低两位恒为0)。
                                                         行地址偏移量为图像在内存中相邻两行首像素存储地址之差,Stride的步长为4（以像素为单位） */
        unsigned int  reserved      : 18; /* bit[14-31]: 保留 */
    } reg;
} SOC_EDC_CH2_STRIDE_UNION;
#endif
#define SOC_EDC_CH2_STRIDE_ch2_ln_stride_START  (0)
#define SOC_EDC_CH2_STRIDE_ch2_ln_stride_END    (13)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_XY_UNION
 结构说明  : CH2_XY 寄存器结构定义。地址偏移量:0x030，初值:0x00000000，宽度:32
 寄存器说明: Channel2坐标寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_y    : 12; /* bit[0-11] : Channel2图像左上角像素的Y坐标 (以像素为单位)
                                                    注： Column By Column/Line By Line格式时指的是拼接后图像的左上角,3D Top To Bottom格式时指的是左/右图像在上半部分/下半部分的坐标，3D Side By Side格式时指的是左/右图像在左半部分/右半部分的坐标 */
        unsigned int  reserved_0: 4;  /* bit[12-15]: 保留 */
        unsigned int  ch2_x    : 12; /* bit[16-27]: Channel2图像左上角像素的X坐标 (以像素为单位)
                                                    注：3D Column By Column/Line By Line格式时指的是拼接后图像的左上角,3D Top To Bottom格式时指的是左/右图像在上半部分/下半部分的坐标，3D Side By Side格式时指的是左/右图像在左半部分/右半部分的坐标 */
        unsigned int  reserved_1: 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_CH2_XY_UNION;
#endif
#define SOC_EDC_CH2_XY_ch2_y_START     (0)
#define SOC_EDC_CH2_XY_ch2_y_END       (11)
#define SOC_EDC_CH2_XY_ch2_x_START     (16)
#define SOC_EDC_CH2_XY_ch2_x_END       (27)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_SIZE_UNION
 结构说明  : CH2_SIZE 寄存器结构定义。地址偏移量:0x034，初值:0x00000000，宽度:32
 寄存器说明: Channel2图像尺寸
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_size_vrt : 12; /* bit[0-11] : Channel2读入左/右眼图像的像素高度(垂平方向)-1(以像素为单位)
                                                        注： 
                                                         1，CH2_SIZE_VRT的步长为2；
                                                         2，在左眼图像超出显示下边界的时候，该字段应配置为：显示分辨率高度与左眼图像起点Y坐标的差值对2向上取整-1；
                                                         3，旋转下该值仍应配置为旋转前原始图像的读入像素高度。 */
        unsigned int  reserved_0   : 4;  /* bit[12-15]: 保留 */
        unsigned int  ch2_size_hrz : 12; /* bit[16-27]: Channel2读入左/右眼图像的像素宽度(水平方向)-1(以像素为单位)
                                                        注： 
                                                         1，CH2_SIZE_HRZ的步长为4；
                                                         2，在左眼图像超出显示右边界的时候，该字段应配置为：显示分辨率宽度与左眼图像起点X坐标的差值对4向上取整-1；
                                                         3，旋转下该值仍应配置为旋转前原始图像的读入像素宽度。 */
        unsigned int  reserved_1   : 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_CH2_SIZE_UNION;
#endif
#define SOC_EDC_CH2_SIZE_ch2_size_vrt_START  (0)
#define SOC_EDC_CH2_SIZE_ch2_size_vrt_END    (11)
#define SOC_EDC_CH2_SIZE_ch2_size_hrz_START  (16)
#define SOC_EDC_CH2_SIZE_ch2_size_hrz_END    (27)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_CTL_UNION
 结构说明  : CH2_CTL 寄存器结构定义。地址偏移量:0x0038，初值:0x00000000，宽度:32
 寄存器说明: EDC_CH2_CTL
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_secu_line : 12; /* bit[0-11] : Channel2的刷新安全区起始行号-1：
                                                         当EDC开始读由该寄存器指定的行，发起CH2_LINE_INT中断，通知Channel2更新进程可以刷新一帧。该机制可避免显示碎裂现象 */
        unsigned int  reserved_0    : 4;  /* bit[12-15]: 保留 */
        unsigned int  ch2_fmtin     : 3;  /* bit[16-18]: Channel2的输入像素格式：
                                                         000： ARGB1555
                                                         001： RGB565
                                                         010： RGB0888
                                                         011： ARGB8888
                                                         100： YUYV Packet
                                                         101： UYVY Packet 
                                                         110： YVYU Packet
                                                         111： VYUY Packet */
        unsigned int  ch2_bgr       : 1;  /* bit[19]   : 0： Channel2输入像素为RGB格式
                                                         1： Channel2输入像素为BGR格式 */
        unsigned int  ch2_colork_en : 1;  /* bit[20]   : Channel2 Color Key使能：
                                                         0： Channel2 Color Key关闭
                                                         1： Channel2 Color Key使能 */
        unsigned int  ch2_en        : 1;  /* bit[21]   : Channel2使能：
                                                         0： Channel2关闭
                                                         1： Channel2开启 */
        unsigned int  reserved_1    : 10; /* bit[22-31]: 保留 */
    } reg;
} SOC_EDC_CH2_CTL_UNION;
#endif
#define SOC_EDC_CH2_CTL_ch2_secu_line_START  (0)
#define SOC_EDC_CH2_CTL_ch2_secu_line_END    (11)
#define SOC_EDC_CH2_CTL_ch2_fmtin_START      (16)
#define SOC_EDC_CH2_CTL_ch2_fmtin_END        (18)
#define SOC_EDC_CH2_CTL_ch2_bgr_START        (19)
#define SOC_EDC_CH2_CTL_ch2_bgr_END          (19)
#define SOC_EDC_CH2_CTL_ch2_colork_en_START  (20)
#define SOC_EDC_CH2_CTL_ch2_colork_en_END    (20)
#define SOC_EDC_CH2_CTL_ch2_en_START         (21)
#define SOC_EDC_CH2_CTL_ch2_en_END           (21)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_COLORK_MIN_UNION
 结构说明  : CH2_COLORK_MIN 寄存器结构定义。地址偏移量:0x003C，初值:0x00000000，宽度:32
 寄存器说明: Channel2关键色下限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_colork_min : 24; /* bit[0-23] : Channel2读入图像关键色的下限，该字段的RGB格式与Channel2的输入RGB格式一致 */
        unsigned int  reserved       : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH2_COLORK_MIN_UNION;
#endif
#define SOC_EDC_CH2_COLORK_MIN_ch2_colork_min_START  (0)
#define SOC_EDC_CH2_COLORK_MIN_ch2_colork_min_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_COLORK_MAX_UNION
 结构说明  : CH2_COLORK_MAX 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: Channel2关键色上限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_colork_max : 24; /* bit[0-23] : Channel2读入图像关键色的上限，该字段的RGB格式与Channel2的输入RGB格式一致 */
        unsigned int  reserved       : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH2_COLORK_MAX_UNION;
#endif
#define SOC_EDC_CH2_COLORK_MAX_ch2_colork_max_START  (0)
#define SOC_EDC_CH2_COLORK_MAX_ch2_colork_max_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_OVLY_UNION
 结构说明  : CH12_OVLY 寄存器结构定义。地址偏移量:0x0044，初值:0x00000000，宽度:32
 寄存器说明: 叠加控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  blend_mskc3_en : 1;  /* bit[0]    : Alpha Blending Mask color3使能
                                                          0： 关闭
                                                          4： 使能 */
        unsigned int  blend_mskc2_en : 1;  /* bit[1]    : Alpha Blending Mask color2使能
                                                          0： 关闭
                                                          3： 使能 */
        unsigned int  blend_mskc1_en : 1;  /* bit[2]    : Alpha Blending Mask color1使能
                                                          0： 关闭
                                                          2： 使能 */
        unsigned int  blend_mskc0_en : 1;  /* bit[3]    : Alpha Blending Mask color0使能
                                                          0： 关闭
                                                          1： 使能 */
        unsigned int  blend_mskc_src : 1;  /* bit[4]    : Alpha Blending Mask Color选择：
                                                          0： 对Channel1进行Mask Color；
                                                          1： 对Channel2进行Mask Color； */
        unsigned int  alp_ch2_sel    : 2;  /* bit[5-6]  : Channel1与Channel2进行Alpha Blending时，Channel2所取的相乘系数：
                                                          00： Alpha 
                                                          01： 1-Alpha
                                                          10： 0
                                                          11： 1 */
        unsigned int  alp_ch1_sel    : 2;  /* bit[7-8]  : Channel1与Channel2进行Alpha Blending时，Channel1所取的相乘系数：
                                                          00： Alpha 
                                                          01： 1-Alpha
                                                          10： 0
                                                          11： 1 */
        unsigned int  pix_alp_src    : 1;  /* bit[9]    : Pixel Alpha的来源(当CH2_ALP_SRC==1时该位有效)：
                                                          0： Pixel Alpha来源于Channel2读入的ARGB图像
                                                          1： Pixel Alpha来源于Channel1读入的ARGB图像 */
        unsigned int  alp_mode       : 1;  /* bit[10]   : Alpha Source选择：
                                                          0： Global Alpha，Alpha来自Global Alpha值寄存器
                                                          1： Pixel Alpha，Alpha来自ARGB格式的像素 */
        unsigned int  blend_en       : 1;  /* bit[11]   : Channel2图层的Source Alpha Blending功能使能：
                                                          0： Channel2的Source Alpha Blending被关闭
                                                          1： Channel2的Source Alpha Blending被开启 */
        unsigned int  ch2_top        : 1;  /* bit[12]   : Channel2图层位置选择：
                                                          0： Channel2图像位于Channel1图像之下
                                                          1： Channel2图像位于Channel1图像之上 */
        unsigned int  reserved       : 19; /* bit[13-31]: 保留 */
    } reg;
} SOC_EDC_CH12_OVLY_UNION;
#endif
#define SOC_EDC_CH12_OVLY_blend_mskc3_en_START  (0)
#define SOC_EDC_CH12_OVLY_blend_mskc3_en_END    (0)
#define SOC_EDC_CH12_OVLY_blend_mskc2_en_START  (1)
#define SOC_EDC_CH12_OVLY_blend_mskc2_en_END    (1)
#define SOC_EDC_CH12_OVLY_blend_mskc1_en_START  (2)
#define SOC_EDC_CH12_OVLY_blend_mskc1_en_END    (2)
#define SOC_EDC_CH12_OVLY_blend_mskc0_en_START  (3)
#define SOC_EDC_CH12_OVLY_blend_mskc0_en_END    (3)
#define SOC_EDC_CH12_OVLY_blend_mskc_src_START  (4)
#define SOC_EDC_CH12_OVLY_blend_mskc_src_END    (4)
#define SOC_EDC_CH12_OVLY_alp_ch2_sel_START     (5)
#define SOC_EDC_CH12_OVLY_alp_ch2_sel_END       (6)
#define SOC_EDC_CH12_OVLY_alp_ch1_sel_START     (7)
#define SOC_EDC_CH12_OVLY_alp_ch1_sel_END       (8)
#define SOC_EDC_CH12_OVLY_pix_alp_src_START     (9)
#define SOC_EDC_CH12_OVLY_pix_alp_src_END       (9)
#define SOC_EDC_CH12_OVLY_alp_mode_START        (10)
#define SOC_EDC_CH12_OVLY_alp_mode_END          (10)
#define SOC_EDC_CH12_OVLY_blend_en_START        (11)
#define SOC_EDC_CH12_OVLY_blend_en_END          (11)
#define SOC_EDC_CH12_OVLY_ch2_top_START         (12)
#define SOC_EDC_CH12_OVLY_ch2_top_END           (12)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_GLB_ALP_VAL_UNION
 结构说明  : CH12_GLB_ALP_VAL 寄存器结构定义。地址偏移量:0x0048，初值:0x00000000，宽度:32
 寄存器说明: Channe1和Channel2进行Alpha Blending时，全局Alpha配置值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch12_alp_value0 : 8;  /* bit[0-7]  : 1，当Alpha取自ARGB1555像素时，该字段表示A=0时所对应的Alpha
                                                           2，当Alpha取自Global Alpha值时，该字段表示Global Alpha */
        unsigned int  ch12_alp_value1 : 8;  /* bit[8-15] : 当Alpha取自ARGB1555像素时，该字段表示A=1时所对应的Alpha，其他情况该字段无效 */
        unsigned int  reserved        : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_EDC_CH12_GLB_ALP_VAL_UNION;
#endif
#define SOC_EDC_CH12_GLB_ALP_VAL_ch12_alp_value0_START  (0)
#define SOC_EDC_CH12_GLB_ALP_VAL_ch12_alp_value0_END    (7)
#define SOC_EDC_CH12_GLB_ALP_VAL_ch12_alp_value1_START  (8)
#define SOC_EDC_CH12_GLB_ALP_VAL_ch12_alp_value1_END    (15)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_MSK_COLOR0_MIN_UNION
 结构说明  : CH12_MSK_COLOR0_MIN 寄存器结构定义。地址偏移量:0x004C，初值:0x00000000，宽度:32
 寄存器说明: Channel1与Channel2进行Blending时的屏蔽色0下限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_msk_color0_min : 24; /* bit[0-23] : Alpha Blending Mask的颜色范围0下限：
                                                              Channel1与Channel2进行Alpha Blending时，如果Channel2像素落入该Mask范围，那么就不进行Alpha Blending，按Channel2的RGB格式写入 */
        unsigned int  reserved           : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH12_MSK_COLOR0_MIN_UNION;
#endif
#define SOC_EDC_CH12_MSK_COLOR0_MIN_ch2_msk_color0_min_START  (0)
#define SOC_EDC_CH12_MSK_COLOR0_MIN_ch2_msk_color0_min_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_MSK_COLOR0_MAX_UNION
 结构说明  : CH12_MSK_COLOR0_MAX 寄存器结构定义。地址偏移量:0x0050，初值:0x00000000，宽度:32
 寄存器说明: Channel1与Channel2进行Blending时的屏蔽色0上限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_msk_color0_max : 24; /* bit[0-23] : Alpha Blending Mask的颜色范围0上限：
                                                              Channel1与Channel2进行Alpha Blending时，如果Channel2像素落入该Mask范围，那么就不进行Alpha Blending，按Channel2的RGB格式写入 */
        unsigned int  reserved           : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH12_MSK_COLOR0_MAX_UNION;
#endif
#define SOC_EDC_CH12_MSK_COLOR0_MAX_ch2_msk_color0_max_START  (0)
#define SOC_EDC_CH12_MSK_COLOR0_MAX_ch2_msk_color0_max_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_MSK_COLOR1_MIN_UNION
 结构说明  : CH12_MSK_COLOR1_MIN 寄存器结构定义。地址偏移量:0x0054，初值:0x00000000，宽度:32
 寄存器说明: Channel1与Channel2进行Blending时的屏蔽色1下限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_msk_color1_min : 24; /* bit[0-23] : Alpha Blending Mask的颜色范围1下限：
                                                              Channel1与Channel2进行Alpha Blending时，如果Channel2像素落入该Mask范围，那么就不进行Alpha Blending，按Channel2的RGB格式写入 */
        unsigned int  reserved           : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH12_MSK_COLOR1_MIN_UNION;
#endif
#define SOC_EDC_CH12_MSK_COLOR1_MIN_ch2_msk_color1_min_START  (0)
#define SOC_EDC_CH12_MSK_COLOR1_MIN_ch2_msk_color1_min_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_MSK_COLOR1_MAX_UNION
 结构说明  : CH12_MSK_COLOR1_MAX 寄存器结构定义。地址偏移量:0x0058，初值:0x00000000，宽度:32
 寄存器说明: Channel1与Channel2进行Blending时的屏蔽色1上限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_msk_color1_max : 24; /* bit[0-23] : Alpha Blending Mask的颜色范围1上限：
                                                              Channel1与Channel2进行Alpha Blending时，如果Channel2像素落入该Mask范围，那么就不进行Alpha Blending，按Channel2的RGB格式写入 */
        unsigned int  reserved           : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH12_MSK_COLOR1_MAX_UNION;
#endif
#define SOC_EDC_CH12_MSK_COLOR1_MAX_ch2_msk_color1_max_START  (0)
#define SOC_EDC_CH12_MSK_COLOR1_MAX_ch2_msk_color1_max_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_MSK_COLOR2_MIN_UNION
 结构说明  : CH12_MSK_COLOR2_MIN 寄存器结构定义。地址偏移量:0x005C，初值:0x00000000，宽度:32
 寄存器说明: Channel1与Channel2进行Blending时的屏蔽色2下限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_msk_color2_min : 24; /* bit[0-23] : Alpha Blending Mask的颜色范围2下限：
                                                              Channel1与Channel2进行Alpha Blending时，如果Channel2像素落入该Mask范围，那么就不进行Alpha Blending，按Channel2的RGB格式写入 */
        unsigned int  reserved           : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH12_MSK_COLOR2_MIN_UNION;
#endif
#define SOC_EDC_CH12_MSK_COLOR2_MIN_ch2_msk_color2_min_START  (0)
#define SOC_EDC_CH12_MSK_COLOR2_MIN_ch2_msk_color2_min_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_MSK_COLOR2_MAX_UNION
 结构说明  : CH12_MSK_COLOR2_MAX 寄存器结构定义。地址偏移量:0x0060，初值:0x00000000，宽度:32
 寄存器说明: Channel1与Channel2进行Blending时的屏蔽色2上限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_msk_color2_max : 24; /* bit[0-23] : Alpha Blending Mask的颜色范围2上限：
                                                              Channel1与Channel2进行Alpha Blending时，如果Channel2像素落入该Mask范围，那么就不进行Alpha Blending，按Channel2的RGB格式写入 */
        unsigned int  reserved           : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH12_MSK_COLOR2_MAX_UNION;
#endif
#define SOC_EDC_CH12_MSK_COLOR2_MAX_ch2_msk_color2_max_START  (0)
#define SOC_EDC_CH12_MSK_COLOR2_MAX_ch2_msk_color2_max_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_MSK_COLOR3_MIN_UNION
 结构说明  : CH12_MSK_COLOR3_MIN 寄存器结构定义。地址偏移量:0x0064，初值:0x00000000，宽度:32
 寄存器说明: Channel1与Channel2进行Blending时的屏蔽色3下限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_msk_color3_min : 24; /* bit[0-23] : Alpha Blending Mask的颜色范围3下限：
                                                              Channel1与Channel2进行Alpha Blending时，如果Channel2像素落入该Mask范围，那么就不进行Alpha Blending，按Channel2的RGB格式写入 */
        unsigned int  reserved           : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH12_MSK_COLOR3_MIN_UNION;
#endif
#define SOC_EDC_CH12_MSK_COLOR3_MIN_ch2_msk_color3_min_START  (0)
#define SOC_EDC_CH12_MSK_COLOR3_MIN_ch2_msk_color3_min_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH12_MSK_COLOR3_MAX_UNION
 结构说明  : CH12_MSK_COLOR3_MAX 寄存器结构定义。地址偏移量:0x0068，初值:0x00000000，宽度:32
 寄存器说明: Channel1与Channel2进行Blending时的屏蔽色3上限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_msk_color3_max : 24; /* bit[0-23] : Alpha Blending Mask的颜色范围3上限：
                                                              Channel1与Channel2进行Alpha Blending时，如果Channel2像素落入该Mask范围，那么就不进行Alpha Blending，按Channel2的RGB格式写入 */
        unsigned int  reserved           : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CH12_MSK_COLOR3_MAX_UNION;
#endif
#define SOC_EDC_CH12_MSK_COLOR3_MAX_ch2_msk_color3_max_START  (0)
#define SOC_EDC_CH12_MSK_COLOR3_MAX_ch2_msk_color3_max_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CRSL_ADDR_UNION
 结构说明  : CRSL_ADDR 寄存器结构定义。地址偏移量:0x006C，初值:0x00000000，宽度:32
 寄存器说明: Cursor左通道地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crsl_addr : 32; /* bit[0-31]: Cursor(左眼)图像在内存中的存储地址(128bit对齐，最低4位恒为0)：
                                                    首行首像素的存储地址。
                                                    注：当Cursor通道读入2D图像时，那么只需配置CRSL_ADDR，
                                                    CRSR_ADDR不起作用 */
    } reg;
} SOC_EDC_CRSL_ADDR_UNION;
#endif
#define SOC_EDC_CRSL_ADDR_crsl_addr_START  (0)
#define SOC_EDC_CRSL_ADDR_crsl_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_CRSR_ADDR_UNION
 结构说明  : CRSR_ADDR 寄存器结构定义。地址偏移量:0x0070，初值:0x00000000，宽度:32
 寄存器说明: Cursor右通道地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crsr_addr : 32; /* bit[0-31]: Cursor(右眼)图像在内存中的存储地址(128bit对齐，最低4位恒为0)：
                                                    首行首像素的存储地址。
                                                    注：当Cursor通道读入2D图像时，那么只需配置CRSL_ADDR，
                                                    CRSR_ADDR不起作用 */
    } reg;
} SOC_EDC_CRSR_ADDR_UNION;
#endif
#define SOC_EDC_CRSR_ADDR_crsr_addr_START  (0)
#define SOC_EDC_CRSR_ADDR_crsr_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_CRS_STRIDE_UNION
 结构说明  : CRS_STRIDE 寄存器结构定义。地址偏移量:0x0074，初值:0x00000000，宽度:32
 寄存器说明: Cursor Stride
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crs_ln_stride : 14; /* bit[0-13] : Cursor行地址偏移量(128bit对齐，最低两位恒为0)。
                                                         行地址偏移量为图像在内存中相邻两行首像素存储地址之差,Stride的步长为4（以像素为单位） */
        unsigned int  reserved      : 18; /* bit[14-31]: 保留 */
    } reg;
} SOC_EDC_CRS_STRIDE_UNION;
#endif
#define SOC_EDC_CRS_STRIDE_crs_ln_stride_START  (0)
#define SOC_EDC_CRS_STRIDE_crs_ln_stride_END    (13)


/*****************************************************************************
 结构名    : SOC_EDC_CRS_XY_UNION
 结构说明  : CRS_XY 寄存器结构定义。地址偏移量:0x0078，初值:0x00000000，宽度:32
 寄存器说明: Cursor坐标寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crs_y    : 12; /* bit[0-11] : Cursor图像左上角像素的Y坐标 (以像素为单位)
                                                    注： Column By Column/Line By Line格式时指的是拼接后图像的左上角,3D Top To Bottom格式时指的是左/右图像在上半部分/下半部分的坐标，3D Side By Side格式时指的是左/右图像在左半部分/右半部分的坐标 */
        unsigned int  reserved_0: 4;  /* bit[12-15]: 保留 */
        unsigned int  crs_x    : 12; /* bit[16-27]: Cursor图像左上角像素的X坐标 (以像素为单位)
                                                    注：3D Column By Column/Line By Line格式时指的是拼接后图像的左上角,3D Top To Bottom格式时指的是左/右图像在上半部分/下半部分的坐标，3D Side By Side格式时指的是左/右图像在左半部分/右半部分的坐标 */
        unsigned int  reserved_1: 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_CRS_XY_UNION;
#endif
#define SOC_EDC_CRS_XY_crs_y_START     (0)
#define SOC_EDC_CRS_XY_crs_y_END       (11)
#define SOC_EDC_CRS_XY_crs_x_START     (16)
#define SOC_EDC_CRS_XY_crs_x_END       (27)


/*****************************************************************************
 结构名    : SOC_EDC_CRS_SIZE_UNION
 结构说明  : CRS_SIZE 寄存器结构定义。地址偏移量:0x07C，初值:0x00000000，宽度:32
 寄存器说明: Cursor图像尺寸
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crs_size_vrt : 12; /* bit[0-11] : Cursor读入左/右眼图像的像素高度(垂平方向)-1(以像素为单位)
                                                        注： 
                                                         1，CRS_SIZE_VRT的步长为2；
                                                         2，在左眼图像超出显示下边界的时候，该字段应配置为：显示分辨率高度与左眼图像起点Y坐标的差值对2向上取整-1；
                                                         3，旋转下该值仍应配置为旋转前原始图像的读入像素高度。 */
        unsigned int  reserved_0   : 4;  /* bit[12-15]: 保留 */
        unsigned int  crs_size_hrz : 12; /* bit[16-27]: Cursor读入左/右眼图像的像素宽度(水平方向)-1(以像素为单位)
                                                        注： 
                                                         1，CRS_SIZE_HRZ的步长为4；
                                                         2，在左眼图像超出显示右边界的时候，该字段应配置为：显示分辨率宽度与左眼图像起点X坐标的差值对4向上取整-1；
                                                         3，旋转下该值仍应配置为旋转前原始图像的读入像素宽度。 */
        unsigned int  reserved_1   : 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_CRS_SIZE_UNION;
#endif
#define SOC_EDC_CRS_SIZE_crs_size_vrt_START  (0)
#define SOC_EDC_CRS_SIZE_crs_size_vrt_END    (11)
#define SOC_EDC_CRS_SIZE_crs_size_hrz_START  (16)
#define SOC_EDC_CRS_SIZE_crs_size_hrz_END    (27)


/*****************************************************************************
 结构名    : SOC_EDC_CRS_CTL_UNION
 结构说明  : CRS_CTL 寄存器结构定义。地址偏移量:0x0080，初值:0x00000000，宽度:32
 寄存器说明: Cursor控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crs_secu_line     : 12; /* bit[0-11] : Cursor的刷新安全区起始行号-1：
                                                             当EDC开始读由该寄存器指定的行，发起CRS_LINE_INT中断，通知Cursor更新进程可以刷新一帧Graphic。该机制可避免Cursor显示碎裂现象 */
        unsigned int  alp_crs_sel       : 2;  /* bit[12-13]: Underlayer与Cursor进行Alpha Blending时，Cursor所取的相乘系数：
                                                             00： Alpha 
                                                             01： 1-Alpha
                                                             10： 0
                                                             11： 1
                                                             注：Underlayer为Base,Channel1和Channel2叠加后的图像 */
        unsigned int  alp_under_sel     : 2;  /* bit[14-15]: Underlayer与Cursor进行Alpha Blending时，Underlayer所取的相乘系数：
                                                             00： Alpha 
                                                             01： 1-Alpha
                                                             10： 0
                                                             11： 1
                                                             注：Underlayer为Base,Channel1和Channel2叠加后的图像 */
        unsigned int  reserved_0        : 1;  /* bit[16]   : 保留 */
        unsigned int  crs_fmtin         : 2;  /* bit[17-18]: Cursor的输入像素格式：
                                                             000： ARGB1555
                                                             001： RGB565
                                                             010： RGB0888
                                                             011： ARGB8888 */
        unsigned int  crs_bgr           : 1;  /* bit[19]   : 0： Cursor输入像素为RGB格式
                                                             1： Cursor输入像素为BGR格式 */
        unsigned int  crs_des_colork_en : 1;  /* bit[20]   : Cursor as Destination Color Key使能：
                                                             0： Cursor as Destination Color Key关闭
                                                             1： Cursor as Destination Color Key使能 */
        unsigned int  reserved_1        : 1;  /* bit[21]   : 保留 */
        unsigned int  crs_alp_mode      : 1;  /* bit[22]   : Alpha Source选择：
                                                             0： Global Alpha，Alpha来自Global Alpha值寄存器
                                                             1： Pixel Alpha，Alpha来自Cursor ARGB格式的像素 */
        unsigned int  crs_src_blend_en  : 1;  /* bit[23]   : Cursor图层的Source Alpha Blending功能使能：
                                                             0： Cursor的Source Alpha Blending被关闭
                                                             1： Cursor的Source Alpha Blending被开启 */
        unsigned int  reserved_2        : 3;  /* bit[24-26]: 保留 */
        unsigned int  crs_en            : 1;  /* bit[27]   : Cursor使能：
                                                             0： Cursor关闭
                                                             1： Cursor开启 */
        unsigned int  crs_cfg_ok        : 1;  /* bit[28]   : 当crs_cfg_ok_sel为1时，该位有效。当CPU配置完所有Cursor寄存器后置该位为1，当EDC检测到帧开始时硬件自动清零。 */
        unsigned int  crs_cfg_ok_sel    : 1;  /* bit[29]   : crs_cfg_ok来源选择：
                                                             0： crs_cfg_ok来自edc_cfg_ok(DISP_CTL.bit1)
                                                             1： crs_cfg_ok来自crs_cfg_ok(CRS_CTL.bit28) */
        unsigned int  reserved_3        : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_EDC_CRS_CTL_UNION;
#endif
#define SOC_EDC_CRS_CTL_crs_secu_line_START      (0)
#define SOC_EDC_CRS_CTL_crs_secu_line_END        (11)
#define SOC_EDC_CRS_CTL_alp_crs_sel_START        (12)
#define SOC_EDC_CRS_CTL_alp_crs_sel_END          (13)
#define SOC_EDC_CRS_CTL_alp_under_sel_START      (14)
#define SOC_EDC_CRS_CTL_alp_under_sel_END        (15)
#define SOC_EDC_CRS_CTL_crs_fmtin_START          (17)
#define SOC_EDC_CRS_CTL_crs_fmtin_END            (18)
#define SOC_EDC_CRS_CTL_crs_bgr_START            (19)
#define SOC_EDC_CRS_CTL_crs_bgr_END              (19)
#define SOC_EDC_CRS_CTL_crs_des_colork_en_START  (20)
#define SOC_EDC_CRS_CTL_crs_des_colork_en_END    (20)
#define SOC_EDC_CRS_CTL_crs_alp_mode_START       (22)
#define SOC_EDC_CRS_CTL_crs_alp_mode_END         (22)
#define SOC_EDC_CRS_CTL_crs_src_blend_en_START   (23)
#define SOC_EDC_CRS_CTL_crs_src_blend_en_END     (23)
#define SOC_EDC_CRS_CTL_crs_en_START             (27)
#define SOC_EDC_CRS_CTL_crs_en_END               (27)
#define SOC_EDC_CRS_CTL_crs_cfg_ok_START         (28)
#define SOC_EDC_CRS_CTL_crs_cfg_ok_END           (28)
#define SOC_EDC_CRS_CTL_crs_cfg_ok_sel_START     (29)
#define SOC_EDC_CRS_CTL_crs_cfg_ok_sel_END       (29)


/*****************************************************************************
 结构名    : SOC_EDC_CRS_COLORK_MIN_UNION
 结构说明  : CRS_COLORK_MIN 寄存器结构定义。地址偏移量:0x0084，初值:0x00000000，宽度:32
 寄存器说明: Cursor关键色下限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crs_colork_min : 24; /* bit[0-23] : Cursor读入图像关键色的下限，该字段的RGB格式与Cursor的输入RGB格式一致 */
        unsigned int  reserved       : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CRS_COLORK_MIN_UNION;
#endif
#define SOC_EDC_CRS_COLORK_MIN_crs_colork_min_START  (0)
#define SOC_EDC_CRS_COLORK_MIN_crs_colork_min_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CRS_COLORK_MAX_UNION
 结构说明  : CRS_COLORK_MAX 寄存器结构定义。地址偏移量:0x0088，初值:0x00000000，宽度:32
 寄存器说明: Cursor关键色上限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crs_colork_max : 24; /* bit[0-23] : Cursor读入图像关键色的上限，该字段的RGB格式与Cursor的输入RGB格式一致 */
        unsigned int  reserved       : 8;  /* bit[24-31]: 保留 */
    } reg;
} SOC_EDC_CRS_COLORK_MAX_UNION;
#endif
#define SOC_EDC_CRS_COLORK_MAX_crs_colork_max_START  (0)
#define SOC_EDC_CRS_COLORK_MAX_crs_colork_max_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_CRS_GLB_ALP_VAL_UNION
 结构说明  : CRS_GLB_ALP_VAL 寄存器结构定义。地址偏移量:0x008C，初值:0x00000000，宽度:32
 寄存器说明: Cursor和下层进行Alpha Blending时，全局Alpha配置值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crs_alp_value0 : 8;  /* bit[0-7]  : 1，当Alpha取自ARGB1555像素时，该字段表示A=0时所对应的Alpha
                                                          2，当Alpha取自Global Alpha值时，该字段表示Global Alpha */
        unsigned int  crs_alp_value1 : 8;  /* bit[8-15] : 当Alpha取自ARGB1555像素时，该字段表示A=1时所对应的Alpha，其他情况该字段无效 */
        unsigned int  reserved       : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_EDC_CRS_GLB_ALP_VAL_UNION;
#endif
#define SOC_EDC_CRS_GLB_ALP_VAL_crs_alp_value0_START  (0)
#define SOC_EDC_CRS_GLB_ALP_VAL_crs_alp_value0_END    (7)
#define SOC_EDC_CRS_GLB_ALP_VAL_crs_alp_value1_START  (8)
#define SOC_EDC_CRS_GLB_ALP_VAL_crs_alp_value1_END    (15)


/*****************************************************************************
 结构名    : SOC_EDC_DISP_SIZE_UNION
 结构说明  : DISP_SIZE 寄存器结构定义。地址偏移量:0x090，初值:0x00000000，宽度:32
 寄存器说明: 显示尺寸
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  disp_size_vrt : 12; /* bit[0-11] : 显示图像的高度-1 */
        unsigned int  reserved_0    : 4;  /* bit[12-15]: 保留 */
        unsigned int  disp_size_hrz : 12; /* bit[16-27]: 显示图像的宽度-1 */
        unsigned int  reserved_1    : 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_DISP_SIZE_UNION;
#endif
#define SOC_EDC_DISP_SIZE_disp_size_vrt_START  (0)
#define SOC_EDC_DISP_SIZE_disp_size_vrt_END    (11)
#define SOC_EDC_DISP_SIZE_disp_size_hrz_START  (16)
#define SOC_EDC_DISP_SIZE_disp_size_hrz_END    (27)


/*****************************************************************************
 结构名    : SOC_EDC_DISP_CTL_UNION
 结构说明  : DISP_CTL 寄存器结构定义。地址偏移量:0x094，初值:0x3C000000，宽度:32
 寄存器说明: 显示控制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  edc_cfg_ok_sel  : 1;  /* bit[0]    : EDC_CFG_OK选择：
                                                           0： 选择本EDC(EDC2)的EDC_CFG_OK作为本EDC的配置完成位
                                                           1： 选择EDC1的EDC_CFG_OK作为本EDC的配置完成位
                                                           注：该位只有在EDC2中有效，使用于相同分辨率的双屏进行扩展显示的场合和通道更替的场合。 */
        unsigned int  edc_cfg_ok      : 1;  /* bit[1]    : EDC配置完成：
                                                           当配置完所有EDC寄存器后，应将该位写1再写0。 */
        unsigned int  d3d_right_first : 1;  /* bit[2]    : 3D非Frame By Frame格式下RIGHT在前标志位：
                                                           0： Left在前
                                                            Side by Side： Left在左，Right在右
                                                            Top to Bottom： Left在上，Right在下
                                                            Column By Column：从左到右，先左后右
                                                            Line By Line： 从上到下，先左后右
                                                            Quincunx： 偶数行从左到右，先左后右；奇数行从左到右， 
                                                            先右后左(行数从0计数)
                                                           1： Right在前
                                                            Side by Side： Right在左，Left在右
                                                            Top to Bottom： right在上，Right在下
                                                            Column By Column：从左到右，先左后右
                                                            Line By Line： 从上到下，先左后右
                                                            Quincunx： 偶数行从左到右，先左后右；奇数行从左到右， 
                                                            先右后左(行数从0计数) */
        unsigned int  disp_frm_fmt    : 3;  /* bit[3-5]  : 显示帧格式：
                                                           000： 2D格式
                                                           001： 3D Side By Side格式
                                                           010： 3D Top To Bottom格式
                                                           011： 3D Column By Column格式
                                                           100： 3D Line By Line格式
                                                           101： 3D Quincunx格式
                                                           110： 3D Frame By Frame格式 */
        unsigned int  disp_pix_fmt    : 2;  /* bit[6-7]  : 输出像素格式：
                                                           00： RGB565
                                                           01： RGB666
                                                           10： RGB888 */
        unsigned int  dither_en       : 1;  /* bit[8]    : Dithering使能：
                                                           0： Dithering关闭，EDC采用四舍五入法将输出像素统一到输出格式
                                                           1： Dithering开启，EDC采用Dithering算法将输出像素统一到输出格式 */
        unsigned int  disp_endian     : 1;  /* bit[9]    : 图层输入大小端：
                                                           0： 小端模式
                                                           1： 64bit数据按8bit大端模式  */
        unsigned int  edc_en          : 1;  /* bit[10]   : EDC使能控制：
                                                           0： EDC被关闭
                                                           1： EDC被开启 */
        unsigned int  unflow_lev      : 12; /* bit[11-22]: DFS下溢预警配置水线 */
        unsigned int  gamma_en        : 1;  /* bit[23]   : 输出GAMMA使能：
                                                           0： 关闭GAMMA校正
                                                           1： 使能GAMMA校正 */
        unsigned int  ch_chg          : 1;  /* bit[24]   : EDC2的Channel1替换EDC1的Channel2控制位：
                                                           0： EDC2的Channel1不替换EDC1的Channel2 
                                                           1： EDC2的Channel1替换EDC1的Channel2
                                                           注：该寄存器位只在EDC1内，同步于LDI1 */
        unsigned int  nrot_burst16    : 1;  /* bit[25]   : 0度旋转下Burst长度定义：
                                                           0： Burst4
                                                           1： Burst8 */
        unsigned int  outstding_dep   : 4;  /* bit[26-29]: outstanding深度配置，范围：1~15，不允许配置为0 */
        unsigned int  frm_end_start   : 1;  /* bit[30]   : EDC帧开始传输时刻选择：
                                                           0：EDC在帧同步信号有效时，开始新一帧的传输；
                                                           1：EDC在帧结束后，就开始新一帧的传输 */
        unsigned int  crg_gt_en       : 1;  /* bit[31]   : 模块级Clock门控使能 */
    } reg;
} SOC_EDC_DISP_CTL_UNION;
#endif
#define SOC_EDC_DISP_CTL_edc_cfg_ok_sel_START   (0)
#define SOC_EDC_DISP_CTL_edc_cfg_ok_sel_END     (0)
#define SOC_EDC_DISP_CTL_edc_cfg_ok_START       (1)
#define SOC_EDC_DISP_CTL_edc_cfg_ok_END         (1)
#define SOC_EDC_DISP_CTL_d3d_right_first_START  (2)
#define SOC_EDC_DISP_CTL_d3d_right_first_END    (2)
#define SOC_EDC_DISP_CTL_disp_frm_fmt_START     (3)
#define SOC_EDC_DISP_CTL_disp_frm_fmt_END       (5)
#define SOC_EDC_DISP_CTL_disp_pix_fmt_START     (6)
#define SOC_EDC_DISP_CTL_disp_pix_fmt_END       (7)
#define SOC_EDC_DISP_CTL_dither_en_START        (8)
#define SOC_EDC_DISP_CTL_dither_en_END          (8)
#define SOC_EDC_DISP_CTL_disp_endian_START      (9)
#define SOC_EDC_DISP_CTL_disp_endian_END        (9)
#define SOC_EDC_DISP_CTL_edc_en_START           (10)
#define SOC_EDC_DISP_CTL_edc_en_END             (10)
#define SOC_EDC_DISP_CTL_unflow_lev_START       (11)
#define SOC_EDC_DISP_CTL_unflow_lev_END         (22)
#define SOC_EDC_DISP_CTL_gamma_en_START         (23)
#define SOC_EDC_DISP_CTL_gamma_en_END           (23)
#define SOC_EDC_DISP_CTL_ch_chg_START           (24)
#define SOC_EDC_DISP_CTL_ch_chg_END             (24)
#define SOC_EDC_DISP_CTL_nrot_burst16_START     (25)
#define SOC_EDC_DISP_CTL_nrot_burst16_END       (25)
#define SOC_EDC_DISP_CTL_outstding_dep_START    (26)
#define SOC_EDC_DISP_CTL_outstding_dep_END      (29)
#define SOC_EDC_DISP_CTL_frm_end_start_START    (30)
#define SOC_EDC_DISP_CTL_frm_end_start_END      (30)
#define SOC_EDC_DISP_CTL_crg_gt_en_START        (31)
#define SOC_EDC_DISP_CTL_crg_gt_en_END          (31)


/*****************************************************************************
 结构名    : SOC_EDC_DISP_DPD_UNION
 结构说明  : DISP_DPD 寄存器结构定义。地址偏移量:0x098，初值:0x00000000，宽度:32
 寄存器说明: 显示背景色
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  disp_dpd           : 24; /* bit[0-23] : Default Pixel Data(DPD)，用于配置Base的显示颜色 */
        unsigned int  reserved           : 4;  /* bit[24-27]: 保留 */
        unsigned int  sbl_recover_bypass : 1;  /* bit[28]   : 0： SBL underflow自恢复功能打开
                                                              1： SBL underflow自恢复功能关闭 */
        unsigned int  sbl_clk_gt         : 1;  /* bit[29]   : SBL时钟门控使能：
                                                              0： 不门控SBL时钟
                                                              1： 门控SBL时钟 */
        unsigned int  sbl_en             : 1;  /* bit[30]   : SBL使能：
                                                              0： 不使能SBL
                                                              1： 使能SBL */
        unsigned int  outstd_ctl_disable : 1;  /* bit[31]   : Outstanding深度控制关闭：
                                                              0： 打开Outstanding深度控制；
                                                              1： 关闭Outstanding深度控制 */
    } reg;
} SOC_EDC_DISP_DPD_UNION;
#endif
#define SOC_EDC_DISP_DPD_disp_dpd_START            (0)
#define SOC_EDC_DISP_DPD_disp_dpd_END              (23)
#define SOC_EDC_DISP_DPD_sbl_recover_bypass_START  (28)
#define SOC_EDC_DISP_DPD_sbl_recover_bypass_END    (28)
#define SOC_EDC_DISP_DPD_sbl_clk_gt_START          (29)
#define SOC_EDC_DISP_DPD_sbl_clk_gt_END            (29)
#define SOC_EDC_DISP_DPD_sbl_en_START              (30)
#define SOC_EDC_DISP_DPD_sbl_en_END                (30)
#define SOC_EDC_DISP_DPD_outstd_ctl_disable_START  (31)
#define SOC_EDC_DISP_DPD_outstd_ctl_disable_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_STS_UNION
 结构说明  : STS 寄存器结构定义。地址偏移量:0x09C，初值:0x80000000，宽度:32
 寄存器说明: 显示状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ovly_fifo_fill_lev : 12; /* bit[0-11] : Overlay fifo填充水线 */
        unsigned int  reserved           : 18; /* bit[12-29]: 保留 */
        unsigned int  bas_vsync          : 1;  /* bit[30]   : 帧后消隐状态位：
                                                              0： EDC正在向LCD控制器组合传输一帧数据；
                                                              1： EDC已完成向LCD控制器的一帧数据的组合和传输；
                                                              该位在帧起始时自动清零，EDC完成一帧数据的组合和传输后自动置位 */
        unsigned int  outstding_zero     : 1;  /* bit[31]   : Master接口的outstanding计数器为0 */
    } reg;
} SOC_EDC_STS_UNION;
#endif
#define SOC_EDC_STS_ovly_fifo_fill_lev_START  (0)
#define SOC_EDC_STS_ovly_fifo_fill_lev_END    (11)
#define SOC_EDC_STS_bas_vsync_START           (30)
#define SOC_EDC_STS_bas_vsync_END             (30)
#define SOC_EDC_STS_outstding_zero_START      (31)
#define SOC_EDC_STS_outstding_zero_END        (31)


/*****************************************************************************
 结构名    : SOC_EDC_INTS_UNION
 结构说明  : INTS 寄存器结构定义。地址偏移量:0x0A0，初值:0x00000000，宽度:32
 寄存器说明: 中断状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crsr_end_int : 1;  /* bit[0]    : Cursor Right通道读入完成中断指示：
                                                        Cursor Right通道读入完成后该位置1，软件写0时该位被情零 */
        unsigned int  crsl_end_int : 1;  /* bit[1]    : Cursor Left通道读入完成中断指示：
                                                        Cursor Left通道读入完成后该位置1，软件写0时该位被情零 */
        unsigned int  ch2r_end_int : 1;  /* bit[2]    : Channel2 Right通道读入完成中断指示：
                                                        Channel2 Right通道读入完成后该位置1，软件写0时该位被情零 */
        unsigned int  ch2l_end_int : 1;  /* bit[3]    : Channel2 Left通道读入完成中断指示：
                                                        Channel2 Left通道读入完成后该位置1，软件写0时该位被情零 */
        unsigned int  ch1r_end_int : 1;  /* bit[4]    : Channel1 Right通道读入完成中断指示：
                                                        Channel1 Right通道读入完成后该位置1，软件写0时该位被情零 */
        unsigned int  ch1l_end_int : 1;  /* bit[5]    : Channel1 Left通道读入完成中断指示：
                                                        Channel1 Left通道读入完成后该位置1，软件写0时该位被情零 */
        unsigned int  bas_end_int  : 1;  /* bit[6]    : 帧更新结束中断位指示：
                                                        当EDC完成向LCD控制器传一帧时发起该中断，软件写0时该位被清零 */
        unsigned int  bas_stat_int : 1;  /* bit[7]    : 帧开始更新中断位：
                                                        当EDC开始向LCD控制器传一帧时发起该中断，软件写0时该位被清零 */
        unsigned int  crs_err_int  : 1;  /* bit[8]    : Cursor安全区起始行中断指示：
                                                        EDC读完Cursor安全区起始行后该位置1，软件写0时该位被清零 */
        unsigned int  ch2_line_int : 1;  /* bit[9]    : Channel2安全区起始行中断指示：
                                                        EDC读完Channel2安全区起始行后该位置1，软件写0时该位被清零 */
        unsigned int  ch1_line_int : 1;  /* bit[10]   : Channel1安全区起始行中断指示：
                                                        EDC读完Channel1安全区起始行后该位置1，软件写0时该位被清零 */
        unsigned int  crsr_err_int : 1;  /* bit[11]   : Cursor Rightt通道传输Bus Error中断指示，软件写0时该位被清零 */
        unsigned int  crsl_err_int : 1;  /* bit[12]   : Cursor Left通道传输Bus Error中断指示，软件写0时该位被清零 */
        unsigned int  ch2r_err_int : 1;  /* bit[13]   : Channel2 Rightt通道传输Bus Error中断指示，软件写0时该位被清零 */
        unsigned int  ch2l_err_int : 1;  /* bit[14]   : Channel2 Left通道传输Bus Error中断指示，软件写0时该位被清零 */
        unsigned int  ch1r_err_int : 1;  /* bit[15]   : Channel1 Rightt通道传输Bus Error中断指示，软件写0时该位被清零 */
        unsigned int  ch1l_err_int : 1;  /* bit[16]   : Channel1 Left通道传输Bus Error中断指示，软件写0时该位被清零 */
        unsigned int  unflow_int   : 1;  /* bit[17]   : Overlay FIFO下溢预警中断指示，软件写0时该位被清零 */
        unsigned int  reserved     : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_EDC_INTS_UNION;
#endif
#define SOC_EDC_INTS_crsr_end_int_START  (0)
#define SOC_EDC_INTS_crsr_end_int_END    (0)
#define SOC_EDC_INTS_crsl_end_int_START  (1)
#define SOC_EDC_INTS_crsl_end_int_END    (1)
#define SOC_EDC_INTS_ch2r_end_int_START  (2)
#define SOC_EDC_INTS_ch2r_end_int_END    (2)
#define SOC_EDC_INTS_ch2l_end_int_START  (3)
#define SOC_EDC_INTS_ch2l_end_int_END    (3)
#define SOC_EDC_INTS_ch1r_end_int_START  (4)
#define SOC_EDC_INTS_ch1r_end_int_END    (4)
#define SOC_EDC_INTS_ch1l_end_int_START  (5)
#define SOC_EDC_INTS_ch1l_end_int_END    (5)
#define SOC_EDC_INTS_bas_end_int_START   (6)
#define SOC_EDC_INTS_bas_end_int_END     (6)
#define SOC_EDC_INTS_bas_stat_int_START  (7)
#define SOC_EDC_INTS_bas_stat_int_END    (7)
#define SOC_EDC_INTS_crs_err_int_START   (8)
#define SOC_EDC_INTS_crs_err_int_END     (8)
#define SOC_EDC_INTS_ch2_line_int_START  (9)
#define SOC_EDC_INTS_ch2_line_int_END    (9)
#define SOC_EDC_INTS_ch1_line_int_START  (10)
#define SOC_EDC_INTS_ch1_line_int_END    (10)
#define SOC_EDC_INTS_crsr_err_int_START  (11)
#define SOC_EDC_INTS_crsr_err_int_END    (11)
#define SOC_EDC_INTS_crsl_err_int_START  (12)
#define SOC_EDC_INTS_crsl_err_int_END    (12)
#define SOC_EDC_INTS_ch2r_err_int_START  (13)
#define SOC_EDC_INTS_ch2r_err_int_END    (13)
#define SOC_EDC_INTS_ch2l_err_int_START  (14)
#define SOC_EDC_INTS_ch2l_err_int_END    (14)
#define SOC_EDC_INTS_ch1r_err_int_START  (15)
#define SOC_EDC_INTS_ch1r_err_int_END    (15)
#define SOC_EDC_INTS_ch1l_err_int_START  (16)
#define SOC_EDC_INTS_ch1l_err_int_END    (16)
#define SOC_EDC_INTS_unflow_int_START    (17)
#define SOC_EDC_INTS_unflow_int_END      (17)


/*****************************************************************************
 结构名    : SOC_EDC_INTE_UNION
 结构说明  : INTE 寄存器结构定义。地址偏移量:0x0A4，初值:0x00000000，宽度:32
 寄存器说明: 中断使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crsr_end_int_msk : 1;  /* bit[0]    : Cursor Right通道读入完成中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  crsl_end_int_msk : 1;  /* bit[1]    : Cursor Left通道读入完成中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch2r_end_int_msk : 1;  /* bit[2]    : Channel2 Right通道读入完成中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch2l_end_int_msk : 1;  /* bit[3]    : Channel2 Left通道读入完成中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch1r_end_int_msk : 1;  /* bit[4]    : Channel1 Right通道读入完成中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch1l_end_int_msk : 1;  /* bit[5]    : Channel1 Left通道读入完成中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  bas_end_int_msk  : 1;  /* bit[6]    : 帧更新结束中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  bas_stat_int_msk : 1;  /* bit[7]    : 帧开始更新中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  crs_err_int_msk  : 1;  /* bit[8]    : Cursor安全区起始行中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch2_line_int_msk : 1;  /* bit[9]    : Channel2安全区起始行中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch1_line_int_msk : 1;  /* bit[10]   : Channel1安全区起始行中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  crsr_err_int_msk : 1;  /* bit[11]   : Cursor Rightt通道传输Bus Error中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  crsl_err_int_msk : 1;  /* bit[12]   : Cursor Left通道传输Bus Error中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch2r_err_int_msk : 1;  /* bit[13]   : Channel2 Rightt通道传输Bus Error中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch2l_err_int_msk : 1;  /* bit[14]   : Channel2 Left通道传输Bus Error中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch1r_err_int_msk : 1;  /* bit[15]   : Channel1 Rightt通道传输Bus Error中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  ch1l_err_int_msk : 1;  /* bit[16]   : Channel1 Left通道传输Bus Error中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  unflow_int_msk   : 1;  /* bit[17]   : Overlay FIFO下溢预警中断屏蔽
                                                            0： 使能中断
                                                            1： 屏蔽中断 */
        unsigned int  reserved         : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_EDC_INTE_UNION;
#endif
#define SOC_EDC_INTE_crsr_end_int_msk_START  (0)
#define SOC_EDC_INTE_crsr_end_int_msk_END    (0)
#define SOC_EDC_INTE_crsl_end_int_msk_START  (1)
#define SOC_EDC_INTE_crsl_end_int_msk_END    (1)
#define SOC_EDC_INTE_ch2r_end_int_msk_START  (2)
#define SOC_EDC_INTE_ch2r_end_int_msk_END    (2)
#define SOC_EDC_INTE_ch2l_end_int_msk_START  (3)
#define SOC_EDC_INTE_ch2l_end_int_msk_END    (3)
#define SOC_EDC_INTE_ch1r_end_int_msk_START  (4)
#define SOC_EDC_INTE_ch1r_end_int_msk_END    (4)
#define SOC_EDC_INTE_ch1l_end_int_msk_START  (5)
#define SOC_EDC_INTE_ch1l_end_int_msk_END    (5)
#define SOC_EDC_INTE_bas_end_int_msk_START   (6)
#define SOC_EDC_INTE_bas_end_int_msk_END     (6)
#define SOC_EDC_INTE_bas_stat_int_msk_START  (7)
#define SOC_EDC_INTE_bas_stat_int_msk_END    (7)
#define SOC_EDC_INTE_crs_err_int_msk_START   (8)
#define SOC_EDC_INTE_crs_err_int_msk_END     (8)
#define SOC_EDC_INTE_ch2_line_int_msk_START  (9)
#define SOC_EDC_INTE_ch2_line_int_msk_END    (9)
#define SOC_EDC_INTE_ch1_line_int_msk_START  (10)
#define SOC_EDC_INTE_ch1_line_int_msk_END    (10)
#define SOC_EDC_INTE_crsr_err_int_msk_START  (11)
#define SOC_EDC_INTE_crsr_err_int_msk_END    (11)
#define SOC_EDC_INTE_crsl_err_int_msk_START  (12)
#define SOC_EDC_INTE_crsl_err_int_msk_END    (12)
#define SOC_EDC_INTE_ch2r_err_int_msk_START  (13)
#define SOC_EDC_INTE_ch2r_err_int_msk_END    (13)
#define SOC_EDC_INTE_ch2l_err_int_msk_START  (14)
#define SOC_EDC_INTE_ch2l_err_int_msk_END    (14)
#define SOC_EDC_INTE_ch1r_err_int_msk_START  (15)
#define SOC_EDC_INTE_ch1r_err_int_msk_END    (15)
#define SOC_EDC_INTE_ch1l_err_int_msk_START  (16)
#define SOC_EDC_INTE_ch1l_err_int_msk_END    (16)
#define SOC_EDC_INTE_unflow_int_msk_START    (17)
#define SOC_EDC_INTE_unflow_int_msk_END      (17)


/*****************************************************************************
 结构名    : SOC_EDC_GAMCNT_CLR_UNION
 结构说明  : GAMCNT_CLR 寄存器结构定义。地址偏移量:0x0A8，初值:0x00000000，宽度:32
 寄存器说明: Gamma参数计数器清零
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gamcnt_clr : 32; /* bit[0-31]: 写1将Gamma系数计数寄存器清零。在每次对EDC_GAMMA进行配置或读出之前需先置位该位。 */
    } reg;
} SOC_EDC_GAMCNT_CLR_UNION;
#endif
#define SOC_EDC_GAMCNT_CLR_gamcnt_clr_START  (0)
#define SOC_EDC_GAMCNT_CLR_gamcnt_clr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_TILE_SIZE_VRT_UNION
 结构说明  : TILE_SIZE_VRT 寄存器结构定义。地址偏移量:0x0AC，初值:0x00000000，宽度:32
 寄存器说明: Channel1 Tile存储图像的垂直高度
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tile_size_vrt : 14; /* bit[0-13] : Tile图像的有效总高度-1。读出值最低两位为0，需软件补2'b11 */
        unsigned int  reserved      : 18; /* bit[14-31]: 保留。 */
    } reg;
} SOC_EDC_TILE_SIZE_VRT_UNION;
#endif
#define SOC_EDC_TILE_SIZE_VRT_tile_size_vrt_START  (0)
#define SOC_EDC_TILE_SIZE_VRT_tile_size_vrt_END    (13)


/*****************************************************************************
 结构名    : SOC_EDC_TILE_CROP_UNION
 结构说明  : TILE_CROP 寄存器结构定义。地址偏移量:0x0B0，初值:0x00000000，宽度:32
 寄存器说明: Channel1 Tile格式下的Crop起点坐标
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tile_crop_x0 : 14; /* bit[0-13] : Tile格式Channel1图像Crop起点X坐标 */
        unsigned int  reserved_0   : 2;  /* bit[14-15]: 保留。 */
        unsigned int  tile_crop_y0 : 14; /* bit[16-29]: Tile格式Channel1图像Crop起点Y坐标 */
        unsigned int  reserved_1   : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_EDC_TILE_CROP_UNION;
#endif
#define SOC_EDC_TILE_CROP_tile_crop_x0_START  (0)
#define SOC_EDC_TILE_CROP_tile_crop_x0_END    (13)
#define SOC_EDC_TILE_CROP_tile_crop_y0_START  (16)
#define SOC_EDC_TILE_CROP_tile_crop_y0_END    (29)


/*****************************************************************************
 结构名    : SOC_EDC_TL_ENGIN0_ADDR_UNION
 结构说明  : TL_ENGIN0_ADDR 寄存器结构定义。地址偏移量:0x0B4，初值:0x00000000，宽度:32
 寄存器说明: Tile Channel左通道Engine0地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tl_engin0_addr : 32; /* bit[0-31]: Tile Channel左通道Engine0地址，低4位固定为0（128bit对齐） */
    } reg;
} SOC_EDC_TL_ENGIN0_ADDR_UNION;
#endif
#define SOC_EDC_TL_ENGIN0_ADDR_tl_engin0_addr_START  (0)
#define SOC_EDC_TL_ENGIN0_ADDR_tl_engin0_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_TL_ENGIN1_ADDR_UNION
 结构说明  : TL_ENGIN1_ADDR 寄存器结构定义。地址偏移量:0x0B8，初值:0x00000000，宽度:32
 寄存器说明: Tile Channel左通道Engine1地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tl_engin1_addr : 32; /* bit[0-31]: Tile Channel左通道Engine1地址，低4位固定为0（128bit对齐） */
    } reg;
} SOC_EDC_TL_ENGIN1_ADDR_UNION;
#endif
#define SOC_EDC_TL_ENGIN1_ADDR_tl_engin1_addr_START  (0)
#define SOC_EDC_TL_ENGIN1_ADDR_tl_engin1_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_TR_ENGIN0_ADDR_UNION
 结构说明  : TR_ENGIN0_ADDR 寄存器结构定义。地址偏移量:0x0BC，初值:0x00000000，宽度:32
 寄存器说明: Tile Channel右通道Engine0地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tr_engin0_addr : 32; /* bit[0-31]: Tile Channel右通道Engine0地址，低4位固定为0（128bit对齐） */
    } reg;
} SOC_EDC_TR_ENGIN0_ADDR_UNION;
#endif
#define SOC_EDC_TR_ENGIN0_ADDR_tr_engin0_addr_START  (0)
#define SOC_EDC_TR_ENGIN0_ADDR_tr_engin0_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_TR_ENGIN1_ADDR_UNION
 结构说明  : TR_ENGIN1_ADDR 寄存器结构定义。地址偏移量:0x0C0，初值:0x00000000，宽度:32
 寄存器说明: Tile Channel右通道Engine1地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tr_engin1_addr : 32; /* bit[0-31]: Tile Channel右通道Engine0地址，低4位固定为0（128bit对齐） */
    } reg;
} SOC_EDC_TR_ENGIN1_ADDR_UNION;
#endif
#define SOC_EDC_TR_ENGIN1_ADDR_tr_engin1_addr_START  (0)
#define SOC_EDC_TR_ENGIN1_ADDR_tr_engin1_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_TRUST_INTS_UNION
 结构说明  : TRUST_INTS 寄存器结构定义。地址偏移量:0x0D0，初值:0x00000000，宽度:32
 寄存器说明: 安全层中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trust_bas_start_int : 1;  /* bit[0]   : 帧开始中断位：
                                                              当EDC开始向LCD控制器传一帧时发起该中断，软件写0时该位被清零 */
        unsigned int  reserved            : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_EDC_TRUST_INTS_UNION;
#endif
#define SOC_EDC_TRUST_INTS_trust_bas_start_int_START  (0)
#define SOC_EDC_TRUST_INTS_trust_bas_start_int_END    (0)


/*****************************************************************************
 结构名    : SOC_EDC_TRUST_MASK_UNION
 结构说明  : TRUST_MASK 寄存器结构定义。地址偏移量:0x00D4，初值:0x00000001，宽度:32
 寄存器说明: 安全层中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trust_bas_start_int_msk : 1;  /* bit[0]   : 帧开始中断屏蔽位：
                                                                  0：使能帧开始中断；
                                                                  1：屏蔽帧开始中断 */
        unsigned int  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_EDC_TRUST_MASK_UNION;
#endif
#define SOC_EDC_TRUST_MASK_trust_bas_start_int_msk_START  (0)
#define SOC_EDC_TRUST_MASK_trust_bas_start_int_msk_END    (0)


/*****************************************************************************
 结构名    : SOC_EDC_APROT_CFG_UNION
 结构说明  : APROT_CFG 寄存器结构定义。地址偏移量:0x00D8，初值:0x00000007，宽度:32
 寄存器说明: 各图层总线安全位配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crs_aprot1 : 1;  /* bit[0]   : Cursor的AXI总线APROT[1] */
        unsigned int  ch2_aprot1 : 1;  /* bit[1]   : Channel2的AXI总线APROT[1] */
        unsigned int  ch1_aprot1 : 1;  /* bit[2]   : Channel1的AXI总线APROT[1] */
        unsigned int  reserved   : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_EDC_APROT_CFG_UNION;
#endif
#define SOC_EDC_APROT_CFG_crs_aprot1_START  (0)
#define SOC_EDC_APROT_CFG_crs_aprot1_END    (0)
#define SOC_EDC_APROT_CFG_ch2_aprot1_START  (1)
#define SOC_EDC_APROT_CFG_ch2_aprot1_END    (1)
#define SOC_EDC_APROT_CFG_ch1_aprot1_START  (2)
#define SOC_EDC_APROT_CFG_ch1_aprot1_END    (2)


/*****************************************************************************
 结构名    : SOC_EDC_CRS_CLIP_UNION
 结构说明  : CRS_CLIP 寄存器结构定义。地址偏移量:0x00DC，初值:0x00000000，宽度:32
 寄存器说明: Cursor层Clip配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  crsr_bot_clip   : 4;  /* bit[0-3]  : cursor右通道读入图像下边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  crsr_top_clip   : 4;  /* bit[4-7]  : cursor右通道读入图像上边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  crsr_right_clip : 4;  /* bit[8-11] : cursor右通道读入图像右边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  crsr_left_clip  : 4;  /* bit[12-15]: cursor右通道读入图像左边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  crsl_bot_clip   : 4;  /* bit[16-19]: cursor左通道读入图像下边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  crsl_top_clip   : 4;  /* bit[20-23]: cursor左通道读入图像上边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  crsl_right_clip : 4;  /* bit[24-27]: cursor左通道读入图像右边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  crsl_left_clip  : 4;  /* bit[28-31]: cursor左通道读入图像左边界Clip的像素数：0~15，为0表示不进行Clip */
    } reg;
} SOC_EDC_CRS_CLIP_UNION;
#endif
#define SOC_EDC_CRS_CLIP_crsr_bot_clip_START    (0)
#define SOC_EDC_CRS_CLIP_crsr_bot_clip_END      (3)
#define SOC_EDC_CRS_CLIP_crsr_top_clip_START    (4)
#define SOC_EDC_CRS_CLIP_crsr_top_clip_END      (7)
#define SOC_EDC_CRS_CLIP_crsr_right_clip_START  (8)
#define SOC_EDC_CRS_CLIP_crsr_right_clip_END    (11)
#define SOC_EDC_CRS_CLIP_crsr_left_clip_START   (12)
#define SOC_EDC_CRS_CLIP_crsr_left_clip_END     (15)
#define SOC_EDC_CRS_CLIP_crsl_bot_clip_START    (16)
#define SOC_EDC_CRS_CLIP_crsl_bot_clip_END      (19)
#define SOC_EDC_CRS_CLIP_crsl_top_clip_START    (20)
#define SOC_EDC_CRS_CLIP_crsl_top_clip_END      (23)
#define SOC_EDC_CRS_CLIP_crsl_right_clip_START  (24)
#define SOC_EDC_CRS_CLIP_crsl_right_clip_END    (27)
#define SOC_EDC_CRS_CLIP_crsl_left_clip_START   (28)
#define SOC_EDC_CRS_CLIP_crsl_left_clip_END     (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_CLIP_UNION
 结构说明  : CH1_CLIP 寄存器结构定义。地址偏移量:0x00F0，初值:0x00000000，宽度:32
 寄存器说明: Channel1图层Clip配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1r_bot_clip   : 4;  /* bit[0-3]  : CH1右通道读入图像下边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch1r_top_clip   : 4;  /* bit[4-7]  : CH1右通道读入图像上边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch1r_right_clip : 4;  /* bit[8-11] : CH1右通道读入图像右边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch1r_left_clip  : 4;  /* bit[12-15]: CH1右通道读入图像左边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch1l_bot_clip   : 4;  /* bit[16-19]: CH1左通道读入图像下边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch1l_top_clip   : 4;  /* bit[20-23]: CH1左通道读入图像上边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch1l_right_clip : 4;  /* bit[24-27]: CH1左通道读入图像右边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch1l_left_clip  : 4;  /* bit[28-31]: CH1左通道读入图像左边界Clip的像素数：0~15，为0表示不进行Clip */
    } reg;
} SOC_EDC_CH1_CLIP_UNION;
#endif
#define SOC_EDC_CH1_CLIP_ch1r_bot_clip_START    (0)
#define SOC_EDC_CH1_CLIP_ch1r_bot_clip_END      (3)
#define SOC_EDC_CH1_CLIP_ch1r_top_clip_START    (4)
#define SOC_EDC_CH1_CLIP_ch1r_top_clip_END      (7)
#define SOC_EDC_CH1_CLIP_ch1r_right_clip_START  (8)
#define SOC_EDC_CH1_CLIP_ch1r_right_clip_END    (11)
#define SOC_EDC_CH1_CLIP_ch1r_left_clip_START   (12)
#define SOC_EDC_CH1_CLIP_ch1r_left_clip_END     (15)
#define SOC_EDC_CH1_CLIP_ch1l_bot_clip_START    (16)
#define SOC_EDC_CH1_CLIP_ch1l_bot_clip_END      (19)
#define SOC_EDC_CH1_CLIP_ch1l_top_clip_START    (20)
#define SOC_EDC_CH1_CLIP_ch1l_top_clip_END      (23)
#define SOC_EDC_CH1_CLIP_ch1l_right_clip_START  (24)
#define SOC_EDC_CH1_CLIP_ch1l_right_clip_END    (27)
#define SOC_EDC_CH1_CLIP_ch1l_left_clip_START   (28)
#define SOC_EDC_CH1_CLIP_ch1l_left_clip_END     (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_CLIP_UNION
 结构说明  : CH2_CLIP 寄存器结构定义。地址偏移量:0x00F4，初值:0x00000000，宽度:32
 寄存器说明: Channel2图层Clip配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2r_bot_clip   : 4;  /* bit[0-3]  : CH2右通道读入图像下边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch2r_top_clip   : 4;  /* bit[4-7]  : CH2右通道读入图像上边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch2r_right_clip : 4;  /* bit[8-11] : CH2右通道读入图像右边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch2r_left_clip  : 4;  /* bit[12-15]: CH2右通道读入图像左边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch2l_bot_clip   : 4;  /* bit[16-19]: CH2左通道读入图像下边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch2l_top_clip   : 4;  /* bit[20-23]: CH2左通道读入图像上边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch2l_right_clip : 4;  /* bit[24-27]: CH2左通道读入图像右边界Clip的像素数：0~15，为0表示不进行Clip */
        unsigned int  ch2l_left_clip  : 4;  /* bit[28-31]: CH2左通道读入图像左边界Clip的像素数：0~15，为0表示不进行Clip */
    } reg;
} SOC_EDC_CH2_CLIP_UNION;
#endif
#define SOC_EDC_CH2_CLIP_ch2r_bot_clip_START    (0)
#define SOC_EDC_CH2_CLIP_ch2r_bot_clip_END      (3)
#define SOC_EDC_CH2_CLIP_ch2r_top_clip_START    (4)
#define SOC_EDC_CH2_CLIP_ch2r_top_clip_END      (7)
#define SOC_EDC_CH2_CLIP_ch2r_right_clip_START  (8)
#define SOC_EDC_CH2_CLIP_ch2r_right_clip_END    (11)
#define SOC_EDC_CH2_CLIP_ch2r_left_clip_START   (12)
#define SOC_EDC_CH2_CLIP_ch2r_left_clip_END     (15)
#define SOC_EDC_CH2_CLIP_ch2l_bot_clip_START    (16)
#define SOC_EDC_CH2_CLIP_ch2l_bot_clip_END      (19)
#define SOC_EDC_CH2_CLIP_ch2l_top_clip_START    (20)
#define SOC_EDC_CH2_CLIP_ch2l_top_clip_END      (23)
#define SOC_EDC_CH2_CLIP_ch2l_right_clip_START  (24)
#define SOC_EDC_CH2_CLIP_ch2l_right_clip_END    (27)
#define SOC_EDC_CH2_CLIP_ch2l_left_clip_START   (28)
#define SOC_EDC_CH2_CLIP_ch2l_left_clip_END     (31)


/*****************************************************************************
 结构名    : SOC_EDC_GAMMA_UNION
 结构说明  : GAMMA 寄存器结构定义。地址偏移量:0x100+(k)*0x4，初值:0x00000000，宽度:32
 寄存器说明: Gamma参数寄存器(k=0~39)
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 2;  /* bit[0-1]  : 保留 */
        unsigned int  gamma_b  : 8;  /* bit[2-9]  : Gamma B曲线的第k拐点值(高8位有效) */
        unsigned int  reserved_1: 2;  /* bit[10-11]: 保留 */
        unsigned int  gamma_g  : 8;  /* bit[12-19]: Gamma G曲线的第k拐点值(高8位有效) */
        unsigned int  reserved_2: 2;  /* bit[20-21]: 保留 */
        unsigned int  gamma_r  : 8;  /* bit[22-29]: Gamma R曲线的第k拐点值 */
        unsigned int  reserved_3: 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_EDC_GAMMA_UNION;
#endif
#define SOC_EDC_GAMMA_gamma_b_START   (2)
#define SOC_EDC_GAMMA_gamma_b_END     (9)
#define SOC_EDC_GAMMA_gamma_g_START   (12)
#define SOC_EDC_GAMMA_gamma_g_END     (19)
#define SOC_EDC_GAMMA_gamma_r_START   (22)
#define SOC_EDC_GAMMA_gamma_r_END     (29)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_CSCIDC_UNION
 结构说明  : CH1_CSCIDC 寄存器结构定义。地址偏移量:0x200，初值:0x00000000，宽度:32
 寄存器说明: Channel1 CSC输入直流偏量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_cscidc_v : 9;  /* bit[0-8]  : in_dc0 */
        unsigned int  ch1_cscidc_u : 9;  /* bit[9-17] : in_dc1 */
        unsigned int  ch1_cscidc_y : 9;  /* bit[18-26]: in_dc2，MSB为符号位。补码表示 */
        unsigned int  ch1_csc_en   : 1;  /* bit[27]   : Channel1 YUV2RGB使能 */
        unsigned int  reserved     : 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_CH1_CSCIDC_UNION;
#endif
#define SOC_EDC_CH1_CSCIDC_ch1_cscidc_v_START  (0)
#define SOC_EDC_CH1_CSCIDC_ch1_cscidc_v_END    (8)
#define SOC_EDC_CH1_CSCIDC_ch1_cscidc_u_START  (9)
#define SOC_EDC_CH1_CSCIDC_ch1_cscidc_u_END    (17)
#define SOC_EDC_CH1_CSCIDC_ch1_cscidc_y_START  (18)
#define SOC_EDC_CH1_CSCIDC_ch1_cscidc_y_END    (26)
#define SOC_EDC_CH1_CSCIDC_ch1_csc_en_START    (27)
#define SOC_EDC_CH1_CSCIDC_ch1_csc_en_END      (27)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_CSCODC_UNION
 结构说明  : CH1_CSCODC 寄存器结构定义。地址偏移量:0x204，初值:0x00000000，宽度:32
 寄存器说明: Channel1 CSC输出直流偏量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_cscodc_b : 9;  /* bit[0-8]  : out_dc0 */
        unsigned int  ch1_cscodc_g : 9;  /* bit[9-17] : out_dc1 */
        unsigned int  ch1_cscodc_r : 9;  /* bit[18-26]: out_dc2，MSB为符号位。补码表示 */
        unsigned int  reserved     : 5;  /* bit[27-31]: 保留 */
    } reg;
} SOC_EDC_CH1_CSCODC_UNION;
#endif
#define SOC_EDC_CH1_CSCODC_ch1_cscodc_b_START  (0)
#define SOC_EDC_CH1_CSCODC_ch1_cscodc_b_END    (8)
#define SOC_EDC_CH1_CSCODC_ch1_cscodc_g_START  (9)
#define SOC_EDC_CH1_CSCODC_ch1_cscodc_g_END    (17)
#define SOC_EDC_CH1_CSCODC_ch1_cscodc_r_START  (18)
#define SOC_EDC_CH1_CSCODC_ch1_cscodc_r_END    (26)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_CSCP0_UNION
 结构说明  : CH1_CSCP0 寄存器结构定义。地址偏移量:0x208，初值:0x00000000，宽度:32
 寄存器说明: Channel1 CSC参数寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_csc_00p : 13; /* bit[0-12] : csc00 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  ch1_csc_01p : 13; /* bit[16-28]: csc01，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_CH1_CSCP0_UNION;
#endif
#define SOC_EDC_CH1_CSCP0_ch1_csc_00p_START  (0)
#define SOC_EDC_CH1_CSCP0_ch1_csc_00p_END    (12)
#define SOC_EDC_CH1_CSCP0_ch1_csc_01p_START  (16)
#define SOC_EDC_CH1_CSCP0_ch1_csc_01p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_CSCP1_UNION
 结构说明  : CH1_CSCP1 寄存器结构定义。地址偏移量:0x20C，初值:0x00000000，宽度:32
 寄存器说明: Channel1 CSC参数寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_csc_02p : 13; /* bit[0-12] : csc02 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  ch1_csc_10p : 13; /* bit[16-28]: csc10，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_CH1_CSCP1_UNION;
#endif
#define SOC_EDC_CH1_CSCP1_ch1_csc_02p_START  (0)
#define SOC_EDC_CH1_CSCP1_ch1_csc_02p_END    (12)
#define SOC_EDC_CH1_CSCP1_ch1_csc_10p_START  (16)
#define SOC_EDC_CH1_CSCP1_ch1_csc_10p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_CSCP2_UNION
 结构说明  : CH1_CSCP2 寄存器结构定义。地址偏移量:0x210，初值:0x00000000，宽度:32
 寄存器说明: Channel1 CSC参数寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_csc_11p : 13; /* bit[0-12] : csc11 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  ch1_csc_12p : 13; /* bit[16-28]: csc12，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_CH1_CSCP2_UNION;
#endif
#define SOC_EDC_CH1_CSCP2_ch1_csc_11p_START  (0)
#define SOC_EDC_CH1_CSCP2_ch1_csc_11p_END    (12)
#define SOC_EDC_CH1_CSCP2_ch1_csc_12p_START  (16)
#define SOC_EDC_CH1_CSCP2_ch1_csc_12p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_CSCP3_UNION
 结构说明  : CH1_CSCP3 寄存器结构定义。地址偏移量:0x214，初值:0x00000000，宽度:32
 寄存器说明: Channel1 CSC参数寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_csc_20p : 13; /* bit[0-12] : csc20 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  ch1_csc_21p : 13; /* bit[16-28]: csc21，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_CH1_CSCP3_UNION;
#endif
#define SOC_EDC_CH1_CSCP3_ch1_csc_20p_START  (0)
#define SOC_EDC_CH1_CSCP3_ch1_csc_20p_END    (12)
#define SOC_EDC_CH1_CSCP3_ch1_csc_21p_START  (16)
#define SOC_EDC_CH1_CSCP3_ch1_csc_21p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_CSCP4_UNION
 结构说明  : CH1_CSCP4 寄存器结构定义。地址偏移量:0x218，初值:0x00000000，宽度:32
 寄存器说明: Channel1 CSC参数寄存器4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_csc_22p : 13; /* bit[0-12] : csc22 */
        unsigned int  reserved    : 19; /* bit[13-31]: 保留 */
    } reg;
} SOC_EDC_CH1_CSCP4_UNION;
#endif
#define SOC_EDC_CH1_CSCP4_ch1_csc_22p_START  (0)
#define SOC_EDC_CH1_CSCP4_ch1_csc_22p_END    (12)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_CSCIDC_UNION
 结构说明  : CH2_CSCIDC 寄存器结构定义。地址偏移量:0x300，初值:0x00000000，宽度:32
 寄存器说明: Channel2 CSC输入直流偏量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_cscidc_v : 9;  /* bit[0-8]  : in_dc0 */
        unsigned int  ch2_cscidc_u : 9;  /* bit[9-17] : in_dc1 */
        unsigned int  ch2_cscidc_y : 9;  /* bit[18-26]: in_dc2，MSB为符号位。补码表示 */
        unsigned int  ch2_csc_en   : 1;  /* bit[27]   : Channel2 YUV2RGB使能 */
        unsigned int  reserved     : 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_CH2_CSCIDC_UNION;
#endif
#define SOC_EDC_CH2_CSCIDC_ch2_cscidc_v_START  (0)
#define SOC_EDC_CH2_CSCIDC_ch2_cscidc_v_END    (8)
#define SOC_EDC_CH2_CSCIDC_ch2_cscidc_u_START  (9)
#define SOC_EDC_CH2_CSCIDC_ch2_cscidc_u_END    (17)
#define SOC_EDC_CH2_CSCIDC_ch2_cscidc_y_START  (18)
#define SOC_EDC_CH2_CSCIDC_ch2_cscidc_y_END    (26)
#define SOC_EDC_CH2_CSCIDC_ch2_csc_en_START    (27)
#define SOC_EDC_CH2_CSCIDC_ch2_csc_en_END      (27)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_CSCODC_UNION
 结构说明  : CH2_CSCODC 寄存器结构定义。地址偏移量:0x304，初值:0x00000000，宽度:32
 寄存器说明: Channel2 CSC输出直流偏量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_cscodc_b : 9;  /* bit[0-8]  : out_dc0 */
        unsigned int  ch2_cscodc_g : 9;  /* bit[9-17] : out_dc1 */
        unsigned int  ch2_cscodc_r : 9;  /* bit[18-26]: out_dc2，MSB为符号位。补码表示 */
        unsigned int  reserved     : 5;  /* bit[27-31]: 保留 */
    } reg;
} SOC_EDC_CH2_CSCODC_UNION;
#endif
#define SOC_EDC_CH2_CSCODC_ch2_cscodc_b_START  (0)
#define SOC_EDC_CH2_CSCODC_ch2_cscodc_b_END    (8)
#define SOC_EDC_CH2_CSCODC_ch2_cscodc_g_START  (9)
#define SOC_EDC_CH2_CSCODC_ch2_cscodc_g_END    (17)
#define SOC_EDC_CH2_CSCODC_ch2_cscodc_r_START  (18)
#define SOC_EDC_CH2_CSCODC_ch2_cscodc_r_END    (26)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_CSCP0_UNION
 结构说明  : CH2_CSCP0 寄存器结构定义。地址偏移量:0x308，初值:0x00000000，宽度:32
 寄存器说明: Channel2 CSC参数寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_csc_00p : 13; /* bit[0-12] : csc00 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  ch2_csc_01p : 13; /* bit[16-28]: csc01，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_CH2_CSCP0_UNION;
#endif
#define SOC_EDC_CH2_CSCP0_ch2_csc_00p_START  (0)
#define SOC_EDC_CH2_CSCP0_ch2_csc_00p_END    (12)
#define SOC_EDC_CH2_CSCP0_ch2_csc_01p_START  (16)
#define SOC_EDC_CH2_CSCP0_ch2_csc_01p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_CSCP1_UNION
 结构说明  : CH2_CSCP1 寄存器结构定义。地址偏移量:0x30C，初值:0x00000000，宽度:32
 寄存器说明: Channel2 CSC参数寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_csc_02p : 13; /* bit[0-12] : csc02 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  ch2_csc_10p : 13; /* bit[16-28]: csc10，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_CH2_CSCP1_UNION;
#endif
#define SOC_EDC_CH2_CSCP1_ch2_csc_02p_START  (0)
#define SOC_EDC_CH2_CSCP1_ch2_csc_02p_END    (12)
#define SOC_EDC_CH2_CSCP1_ch2_csc_10p_START  (16)
#define SOC_EDC_CH2_CSCP1_ch2_csc_10p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_CSCP2_UNION
 结构说明  : CH2_CSCP2 寄存器结构定义。地址偏移量:0x310，初值:0x00000000，宽度:32
 寄存器说明: Channel2 CSC参数寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_csc_11p : 13; /* bit[0-12] : csc11 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  ch2_csc_12p : 13; /* bit[16-28]: csc12，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_CH2_CSCP2_UNION;
#endif
#define SOC_EDC_CH2_CSCP2_ch2_csc_11p_START  (0)
#define SOC_EDC_CH2_CSCP2_ch2_csc_11p_END    (12)
#define SOC_EDC_CH2_CSCP2_ch2_csc_12p_START  (16)
#define SOC_EDC_CH2_CSCP2_ch2_csc_12p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_CSCP3_UNION
 结构说明  : CH2_CSCP3 寄存器结构定义。地址偏移量:0x314，初值:0x00000000，宽度:32
 寄存器说明: Channel2 CSC参数寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_csc_20p : 13; /* bit[0-12] : csc20 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  ch2_csc_21p : 13; /* bit[16-28]: csc21，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_CH2_CSCP3_UNION;
#endif
#define SOC_EDC_CH2_CSCP3_ch2_csc_20p_START  (0)
#define SOC_EDC_CH2_CSCP3_ch2_csc_20p_END    (12)
#define SOC_EDC_CH2_CSCP3_ch2_csc_21p_START  (16)
#define SOC_EDC_CH2_CSCP3_ch2_csc_21p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_CH2_CSCP4_UNION
 结构说明  : CH2_CSCP4 寄存器结构定义。地址偏移量:0x318，初值:0x00000000，宽度:32
 寄存器说明: Channel2 CSC参数寄存器4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_csc_22p : 13; /* bit[0-12] : csc22 */
        unsigned int  reserved    : 19; /* bit[13-31]: 保留 */
    } reg;
} SOC_EDC_CH2_CSCP4_UNION;
#endif
#define SOC_EDC_CH2_CSCP4_ch2_csc_22p_START  (0)
#define SOC_EDC_CH2_CSCP4_ch2_csc_22p_END    (12)


/*****************************************************************************
 结构名    : SOC_EDC_OUT_CSCIDC_UNION
 结构说明  : OUT_CSCIDC 寄存器结构定义。地址偏移量:0x400，初值:0x00000000，宽度:32
 寄存器说明: 输出CSC输入直流偏量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  out_cscidc_v : 9;  /* bit[0-8]  : in_dc0 */
        unsigned int  out_cscidc_u : 9;  /* bit[9-17] : in_dc1 */
        unsigned int  out_cscidc_y : 9;  /* bit[18-26]: in_dc2，MSB为符号位。补码表示 */
        unsigned int  out_csc_en   : 1;  /* bit[27]   : Overlay输出CSC使能 */
        unsigned int  reserved     : 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_EDC_OUT_CSCIDC_UNION;
#endif
#define SOC_EDC_OUT_CSCIDC_out_cscidc_v_START  (0)
#define SOC_EDC_OUT_CSCIDC_out_cscidc_v_END    (8)
#define SOC_EDC_OUT_CSCIDC_out_cscidc_u_START  (9)
#define SOC_EDC_OUT_CSCIDC_out_cscidc_u_END    (17)
#define SOC_EDC_OUT_CSCIDC_out_cscidc_y_START  (18)
#define SOC_EDC_OUT_CSCIDC_out_cscidc_y_END    (26)
#define SOC_EDC_OUT_CSCIDC_out_csc_en_START    (27)
#define SOC_EDC_OUT_CSCIDC_out_csc_en_END      (27)


/*****************************************************************************
 结构名    : SOC_EDC_OUT_CSCODC_UNION
 结构说明  : OUT_CSCODC 寄存器结构定义。地址偏移量:0x404，初值:0x00000000，宽度:32
 寄存器说明: 输出CSC输出直流偏量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  out_cscodc_b : 9;  /* bit[0-8]  : out_dc0 */
        unsigned int  out_cscodc_g : 9;  /* bit[9-17] : out_dc1 */
        unsigned int  out_cscodc_r : 9;  /* bit[18-26]: out_dc2，MSB为符号位。补码表示 */
        unsigned int  reserved     : 5;  /* bit[27-31]: 保留 */
    } reg;
} SOC_EDC_OUT_CSCODC_UNION;
#endif
#define SOC_EDC_OUT_CSCODC_out_cscodc_b_START  (0)
#define SOC_EDC_OUT_CSCODC_out_cscodc_b_END    (8)
#define SOC_EDC_OUT_CSCODC_out_cscodc_g_START  (9)
#define SOC_EDC_OUT_CSCODC_out_cscodc_g_END    (17)
#define SOC_EDC_OUT_CSCODC_out_cscodc_r_START  (18)
#define SOC_EDC_OUT_CSCODC_out_cscodc_r_END    (26)


/*****************************************************************************
 结构名    : SOC_EDC_OUT_CSCP0_UNION
 结构说明  : OUT_CSCP0 寄存器结构定义。地址偏移量:0x408，初值:0x00000000，宽度:32
 寄存器说明: 输出CSC参数寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  out_csc_00p : 13; /* bit[0-12] : csc00 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  out_csc_01p : 13; /* bit[16-28]: csc01，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_OUT_CSCP0_UNION;
#endif
#define SOC_EDC_OUT_CSCP0_out_csc_00p_START  (0)
#define SOC_EDC_OUT_CSCP0_out_csc_00p_END    (12)
#define SOC_EDC_OUT_CSCP0_out_csc_01p_START  (16)
#define SOC_EDC_OUT_CSCP0_out_csc_01p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_OUT_CSCP1_UNION
 结构说明  : OUT_CSCP1 寄存器结构定义。地址偏移量:0x40C，初值:0x00000000，宽度:32
 寄存器说明: 输出CSC参数寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  out_csc_02p : 13; /* bit[0-12] : csc02 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  out_csc_10p : 13; /* bit[16-28]: csc10，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_OUT_CSCP1_UNION;
#endif
#define SOC_EDC_OUT_CSCP1_out_csc_02p_START  (0)
#define SOC_EDC_OUT_CSCP1_out_csc_02p_END    (12)
#define SOC_EDC_OUT_CSCP1_out_csc_10p_START  (16)
#define SOC_EDC_OUT_CSCP1_out_csc_10p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_OUT_CSCP2_UNION
 结构说明  : OUT_CSCP2 寄存器结构定义。地址偏移量:0x410，初值:0x00000000，宽度:32
 寄存器说明: 输出CSC参数寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  out_csc_11p : 13; /* bit[0-12] : csc11 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  out_csc_12p : 13; /* bit[16-28]: csc12，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_OUT_CSCP2_UNION;
#endif
#define SOC_EDC_OUT_CSCP2_out_csc_11p_START  (0)
#define SOC_EDC_OUT_CSCP2_out_csc_11p_END    (12)
#define SOC_EDC_OUT_CSCP2_out_csc_12p_START  (16)
#define SOC_EDC_OUT_CSCP2_out_csc_12p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_OUT_CSCP3_UNION
 结构说明  : OUT_CSCP3 寄存器结构定义。地址偏移量:0x414，初值:0x00000000，宽度:32
 寄存器说明: 输出CSC参数寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  out_csc_20p : 13; /* bit[0-12] : csc20 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留 */
        unsigned int  out_csc_21p : 13; /* bit[16-28]: csc21，1bit符号位、4bit整数位、8bit小数位。补码表示 */
        unsigned int  reserved_1  : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_EDC_OUT_CSCP3_UNION;
#endif
#define SOC_EDC_OUT_CSCP3_out_csc_20p_START  (0)
#define SOC_EDC_OUT_CSCP3_out_csc_20p_END    (12)
#define SOC_EDC_OUT_CSCP3_out_csc_21p_START  (16)
#define SOC_EDC_OUT_CSCP3_out_csc_21p_END    (28)


/*****************************************************************************
 结构名    : SOC_EDC_OUT_CSCP4_UNION
 结构说明  : OUT_CSCP4 寄存器结构定义。地址偏移量:0x418，初值:0x00000000，宽度:32
 寄存器说明: 输出CSC参数寄存器4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  out_csc_22p : 13; /* bit[0-12] : csc22 */
        unsigned int  reserved    : 19; /* bit[13-31]: 保留 */
    } reg;
} SOC_EDC_OUT_CSCP4_UNION;
#endif
#define SOC_EDC_OUT_CSCP4_out_csc_22p_START  (0)
#define SOC_EDC_OUT_CSCP4_out_csc_22p_END    (12)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SCL_HSP_UNION
 结构说明  : CH1_SCL_HSP 寄存器结构定义。地址偏移量:0x500，初值:0x00000000，宽度:32
 寄存器说明: Scaler水平缩放控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hratio     : 16; /* bit[0-15] : 水平缩放倍数，(u,4,12)格式。 */
        unsigned int  reserved_0 : 3;  /* bit[16-18]: 保留。 */
        unsigned int  hfir_order : 1;  /* bit[19]   : 水平缩放位置。
                                                      
                                                      0：水平缩放在垂直缩放前面
                                                      1：水平缩放在垂直缩放后面 */
        unsigned int  reserved_1 : 3;  /* bit[20-22]: 保留。 */
        unsigned int  hafir_en   : 1;  /* bit[23]   : 水平α缩放模式。
                                                      
                                                      0：复制模式（滤波不使能）；
                                                      1：滤波模式（滤波使能），与Y分量采用同一滤波模式。 */
        unsigned int  hfir_en    : 1;  /* bit[24]   : 亮度和色度水平缩放模式。
                                                      
                                                      0：复制模式（滤波不使能）；
                                                      1：滤波模式（滤波使能）。 */
        unsigned int  reserved_2 : 3;  /* bit[25-27]: 保留。 */
        unsigned int  hchmid_en  : 1;  /* bit[28]   : 水平缩放中值滤波使能（当hfir_en无效时，该比特不起作用）。
                                                      
                                                      0：禁止；
                                                      1：使能。 */
        unsigned int  hlmid_en   : 1;  /* bit[29]   : 水平缩放中值滤波使能（当hfir_en无效时，该比特不起作用）。
                                                      
                                                      0：禁止；
                                                      1：使能。 */
        unsigned int  hamid_en   : 1;  /* bit[30]   : α值水平缩放中值滤波使能（当hfir_en无效时，该比特不起作用）。
                                                      
                                                      0：禁止；
                                                      1：使能 */
        unsigned int  hsc_en     : 1;  /* bit[31]   : 水平缩放使能。
                                                      0：禁止；1：使能 */
    } reg;
} SOC_EDC_CH1_SCL_HSP_UNION;
#endif
#define SOC_EDC_CH1_SCL_HSP_hratio_START      (0)
#define SOC_EDC_CH1_SCL_HSP_hratio_END        (15)
#define SOC_EDC_CH1_SCL_HSP_hfir_order_START  (19)
#define SOC_EDC_CH1_SCL_HSP_hfir_order_END    (19)
#define SOC_EDC_CH1_SCL_HSP_hafir_en_START    (23)
#define SOC_EDC_CH1_SCL_HSP_hafir_en_END      (23)
#define SOC_EDC_CH1_SCL_HSP_hfir_en_START     (24)
#define SOC_EDC_CH1_SCL_HSP_hfir_en_END       (24)
#define SOC_EDC_CH1_SCL_HSP_hchmid_en_START   (28)
#define SOC_EDC_CH1_SCL_HSP_hchmid_en_END     (28)
#define SOC_EDC_CH1_SCL_HSP_hlmid_en_START    (29)
#define SOC_EDC_CH1_SCL_HSP_hlmid_en_END      (29)
#define SOC_EDC_CH1_SCL_HSP_hamid_en_START    (30)
#define SOC_EDC_CH1_SCL_HSP_hamid_en_END      (30)
#define SOC_EDC_CH1_SCL_HSP_hsc_en_START      (31)
#define SOC_EDC_CH1_SCL_HSP_hsc_en_END        (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SCL_HOFFSET_UNION
 结构说明  : CH1_SCL_HOFFSET 寄存器结构定义。地址偏移量:0x504，初值:0x00000000，宽度:32
 寄存器说明: Scaler水平初始相移寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hor_coffset : 16; /* bit[0-15] : 水平色度位置偏移，(s,4,12)格式。补码表示。 */
        unsigned int  hor_loffset : 16; /* bit[16-31]: 水平亮度位置偏移，(s,4,12)格式。补码表示。 */
    } reg;
} SOC_EDC_CH1_SCL_HOFFSET_UNION;
#endif
#define SOC_EDC_CH1_SCL_HOFFSET_hor_coffset_START  (0)
#define SOC_EDC_CH1_SCL_HOFFSET_hor_coffset_END    (15)
#define SOC_EDC_CH1_SCL_HOFFSET_hor_loffset_START  (16)
#define SOC_EDC_CH1_SCL_HOFFSET_hor_loffset_END    (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SCL_VSP_UNION
 结构说明  : CH1_SCL_VSP 寄存器结构定义。地址偏移量:0x508，初值:0x00000000，宽度:32
 寄存器说明: Scaler垂直缩放控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0   : 23; /* bit[0-22] : 保留。 */
        unsigned int  vafir_en     : 1;  /* bit[23]   : 垂直α缩放模式。
                                                        
                                                        0：复制模式（滤波不使能）；
                                                        1：滤波模式（滤波使能），与亮度分量一样滤波。 */
        unsigned int  vfir_en      : 1;  /* bit[24]   : 亮度和色度垂直缩放模式。
                                                        
                                                        0：复制模式（滤波不使能）；
                                                        1：滤波模式（滤波使能）。 */
        unsigned int  reserved_1   : 2;  /* bit[25-26]: 保留。 */
        unsigned int  vsc_luma_tap : 1;  /* bit[27]   : 垂直缩放阶数。
                                                        
                                                        0：4阶FIR；
                                                        1：2阶FIR */
        unsigned int  vchmid_en    : 1;  /* bit[28]   : 垂直色度缩放中值滤波使能（当vlfir_en无效时，该比特不起作用）。
                                                        
                                                        0：禁止；
                                                        1：使能。 */
        unsigned int  vlmid_en     : 1;  /* bit[29]   : 垂直亮度缩放中值滤波使能（当vlfir_en无效时，该比特不起作用）。
                                                        
                                                        0：禁止；
                                                        1：使能。 */
        unsigned int  vamid_en     : 1;  /* bit[30]   : 垂直α缩放中值滤波使能（当vlfir_en无效时，该比特不起作用）。
                                                        
                                                        0：禁止；
                                                        1：使能。 */
        unsigned int  vsc_en       : 1;  /* bit[31]   : 垂直缩放使能。
                                                        0：禁止；1：使能 */
    } reg;
} SOC_EDC_CH1_SCL_VSP_UNION;
#endif
#define SOC_EDC_CH1_SCL_VSP_vafir_en_START      (23)
#define SOC_EDC_CH1_SCL_VSP_vafir_en_END        (23)
#define SOC_EDC_CH1_SCL_VSP_vfir_en_START       (24)
#define SOC_EDC_CH1_SCL_VSP_vfir_en_END         (24)
#define SOC_EDC_CH1_SCL_VSP_vsc_luma_tap_START  (27)
#define SOC_EDC_CH1_SCL_VSP_vsc_luma_tap_END    (27)
#define SOC_EDC_CH1_SCL_VSP_vchmid_en_START     (28)
#define SOC_EDC_CH1_SCL_VSP_vchmid_en_END       (28)
#define SOC_EDC_CH1_SCL_VSP_vlmid_en_START      (29)
#define SOC_EDC_CH1_SCL_VSP_vlmid_en_END        (29)
#define SOC_EDC_CH1_SCL_VSP_vamid_en_START      (30)
#define SOC_EDC_CH1_SCL_VSP_vamid_en_END        (30)
#define SOC_EDC_CH1_SCL_VSP_vsc_en_START        (31)
#define SOC_EDC_CH1_SCL_VSP_vsc_en_END          (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SCL_VSR_UNION
 结构说明  : CH1_SCL_VSR 寄存器结构定义。地址偏移量:0x50C，初值:0x00000000，宽度:32
 寄存器说明: Scaler垂直缩放比例寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vratio   : 16; /* bit[0-15] : 垂直缩放倍数，(u,4,12)格式。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_EDC_CH1_SCL_VSR_UNION;
#endif
#define SOC_EDC_CH1_SCL_VSR_vratio_START    (0)
#define SOC_EDC_CH1_SCL_VSR_vratio_END      (15)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SCL_VOFFSET_UNION
 结构说明  : CH1_SCL_VOFFSET 寄存器结构定义。地址偏移量:0x510，初值:0x00000000，宽度:32
 寄存器说明: Scaler垂直初始相移寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vbtm_offset : 16; /* bit[0-15] : 垂直底场偏移，补码表示，(s,4,12)格式。 */
        unsigned int  vtp_offset  : 16; /* bit[16-31]: 隔行视频：垂直顶场偏移，补码表示，(s,4,12)格式。 */
    } reg;
} SOC_EDC_CH1_SCL_VOFFSET_UNION;
#endif
#define SOC_EDC_CH1_SCL_VOFFSET_vbtm_offset_START  (0)
#define SOC_EDC_CH1_SCL_VOFFSET_vbtm_offset_END    (15)
#define SOC_EDC_CH1_SCL_VOFFSET_vtp_offset_START   (16)
#define SOC_EDC_CH1_SCL_VOFFSET_vtp_offset_END     (31)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SCL_ORES_UNION
 结构说明  : CH1_SCL_ORES 寄存器结构定义。地址偏移量:0x514，初值:0x00000000，宽度:32
 寄存器说明: Scaler输出图像尺寸寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ow       : 12; /* bit[0-11] : 宽度，像素单位。实际宽度减1。
                                                    注意：层的实际宽度必须是偶数。
                                                    注：如果个方向不使能缩放，该方向的输入输出尺寸也需要配置正确。 */
        unsigned int  oh       : 12; /* bit[12-23]: 高度，行为单位。实际高度减1。
                                                    以帧高度为参考。
                                                    注：如果个方向不使能缩放，该方向的输入输出尺寸也需要配置正确。 */
        unsigned int  reserved : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_EDC_CH1_SCL_ORES_UNION;
#endif
#define SOC_EDC_CH1_SCL_ORES_ow_START        (0)
#define SOC_EDC_CH1_SCL_ORES_ow_END          (11)
#define SOC_EDC_CH1_SCL_ORES_oh_START        (12)
#define SOC_EDC_CH1_SCL_ORES_oh_END          (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SCL_IRES_UNION
 结构说明  : CH1_SCL_IRES 寄存器结构定义。地址偏移量:0x518，初值:0x00000000，宽度:32
 寄存器说明: Scaler输入图像尺寸寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  iw       : 12; /* bit[0-11] : 宽度，像素单位。实际宽度减1。
                                                    注意：层的实际宽度必须是偶数。
                                                    注：如果个方向不使能缩放，该方向的输入输出尺寸也需要配置正确。 */
        unsigned int  ih       : 12; /* bit[12-23]: 高度，行为单位。实际高度减1。
                                                    以帧高度为参考。
                                                    注：如果个方向不使能缩放，该方向的输入输出尺寸也需要配置正确。 */
        unsigned int  reserved : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_EDC_CH1_SCL_IRES_UNION;
#endif
#define SOC_EDC_CH1_SCL_IRES_iw_START        (0)
#define SOC_EDC_CH1_SCL_IRES_iw_END          (11)
#define SOC_EDC_CH1_SCL_IRES_ih_START        (12)
#define SOC_EDC_CH1_SCL_IRES_ih_END          (23)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SCL_HPC_UNION
 结构说明  : CH1_SCL_HPC 寄存器结构定义。地址偏移量:0x600+((m)-1)*0x10+((i)-1)*0x4，初值:0x00000000，宽度:32
 寄存器说明: Scaler水平滤波参数寄存器（m=1~8;i=1~4）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hlcoefn1 : 10; /* bit[0-9]  : 相位m水平亮度缩放滤波第2i-1阶系数。
                                                    
                                                    每个滤波器系数为10bit，由符号位和小数位组成，最高位为符号位，低9bit为小数位的绝对值。由于系数存放在片上Memory中，因此默认值为不定态 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留。 */
        unsigned int  hlcoefn2 : 10; /* bit[16-25]: 相位m水平亮度缩放滤波第2i阶系数。
                                                    
                                                    每个滤波器系数为10bit，由符号位和小数位组成，最高位为符号位，低9bit为小数位的绝对值。由于系数存放在片上Memory中，因此默认值为不定态 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_EDC_CH1_SCL_HPC_UNION;
#endif
#define SOC_EDC_CH1_SCL_HPC_hlcoefn1_START  (0)
#define SOC_EDC_CH1_SCL_HPC_hlcoefn1_END    (9)
#define SOC_EDC_CH1_SCL_HPC_hlcoefn2_START  (16)
#define SOC_EDC_CH1_SCL_HPC_hlcoefn2_END    (25)


/*****************************************************************************
 结构名    : SOC_EDC_CH1_SCL_VPC_UNION
 结构说明  : CH1_SCL_VPC 寄存器结构定义。地址偏移量:0x700+((n)-1)*0x8+((j)-1)*0x4，初值:0x00000000，宽度:32
 寄存器说明: Scaler垂直滤波参数寄存器（n=1~16;i=1~2）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vlcoefn1 : 10; /* bit[0-9]  : 相位n垂直亮度缩放滤波第2j-1阶系数。
                                                    
                                                    每个滤波器系数为10bit，由符号位和小数位组成，最高位为符号位，低9bit为小数位的绝对值。由于系数存放在片上Memory中，因此默认值为不定态 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留。 */
        unsigned int  vlcoefn2 : 10; /* bit[16-25]: 相位n垂直亮度缩放滤波第2j阶系数。
                                                    
                                                    每个滤波器系数为10bit，由符号位和小数位组成，最高位为符号位，低9bit为小数位的绝对值。由于系数存放在片上Memory中，因此默认值为不定态 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_EDC_CH1_SCL_VPC_UNION;
#endif
#define SOC_EDC_CH1_SCL_VPC_vlcoefn1_START  (0)
#define SOC_EDC_CH1_SCL_VPC_vlcoefn1_END    (9)
#define SOC_EDC_CH1_SCL_VPC_vlcoefn2_START  (16)
#define SOC_EDC_CH1_SCL_VPC_vlcoefn2_END    (25)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_HRZ_CTRL0_UNION
 结构说明  : LDI_HRZ_CTRL0 寄存器结构定义。地址偏移量:0x1000，初值:0x00000000，宽度:32
 寄存器说明: LDI水平扫描控制寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hfp      : 12; /* bit[0-11] : 行前消隐时间配置，写入的是实际值。行后消隐取值范围0~4095;
                                                    行后消隐为一行有效数据结束到水平同步信号ldi_hsync有效这段时间内的像素时钟个数 */
        unsigned int  reserved : 8;  /* bit[12-19]:  */
        unsigned int  hbp      : 12; /* bit[20-31]: 行后消隐时间配置，写入的是实际值。行后消隐的取值范围为0~4095 */
    } reg;
} SOC_EDC_LDI_HRZ_CTRL0_UNION;
#endif
#define SOC_EDC_LDI_HRZ_CTRL0_hfp_START       (0)
#define SOC_EDC_LDI_HRZ_CTRL0_hfp_END         (11)
#define SOC_EDC_LDI_HRZ_CTRL0_hbp_START       (20)
#define SOC_EDC_LDI_HRZ_CTRL0_hbp_END         (31)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_HRZ_CTRL1_UNION
 结构说明  : LDI_HRZ_CTRL1 寄存器结构定义。地址偏移量:0x1004，初值:0x00000000，宽度:32
 寄存器说明: LDI水平扫描寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hsw      : 12; /* bit[0-11] : 水平同步脉宽配置，写入的值为实际值减1后的结果。水平同步脉冲的宽度以像素时钟为单位，取值范围为1~4096 */
        unsigned int  reserved : 20; /* bit[12-31]:  */
    } reg;
} SOC_EDC_LDI_HRZ_CTRL1_UNION;
#endif
#define SOC_EDC_LDI_HRZ_CTRL1_hsw_START       (0)
#define SOC_EDC_LDI_HRZ_CTRL1_hsw_END         (11)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_VRT_CTRL0_UNION
 结构说明  : LDI_VRT_CTRL0 寄存器结构定义。地址偏移量:0x1008，初值:0x00000000，宽度:32
 寄存器说明: LDI垂直扫描控制寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vfp      : 12; /* bit[0-11] : 帧前消隐时间配置，写入的是实际值，取值范围为0~4095;
                                                    帧前消隐为有效数据结束到帧同步信号ldi_vsync有效这段时间内的消隐扫描行数 */
        unsigned int  reserved : 8;  /* bit[12-19]:  */
        unsigned int  vbp      : 12; /* bit[20-31]: 帧后消隐时间配置，写入的是实际值，取值范围为0~4095.
                                                    帧后消隐为帧同步信号ldi_vsync无效到一帧有效数据开始这段时间内的消隐扫描行数. */
    } reg;
} SOC_EDC_LDI_VRT_CTRL0_UNION;
#endif
#define SOC_EDC_LDI_VRT_CTRL0_vfp_START       (0)
#define SOC_EDC_LDI_VRT_CTRL0_vfp_END         (11)
#define SOC_EDC_LDI_VRT_CTRL0_vbp_START       (20)
#define SOC_EDC_LDI_VRT_CTRL0_vbp_END         (31)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_VRT_CTRL1_UNION
 结构说明  : LDI_VRT_CTRL1 寄存器结构定义。地址偏移量:0x100C，初值:0x00000000，宽度:32
 寄存器说明: LDI垂直扫描控制寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vsw      : 12; /* bit[0-11] : 帧同步脉冲宽度配置，写入的值为实际值减1的结果。帧同步脉冲宽度是以扫描行数为单位,取值范围为1~4096 */
        unsigned int  reserved : 20; /* bit[12-31]:  */
    } reg;
} SOC_EDC_LDI_VRT_CTRL1_UNION;
#endif
#define SOC_EDC_LDI_VRT_CTRL1_vsw_START       (0)
#define SOC_EDC_LDI_VRT_CTRL1_vsw_END         (11)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_PLR_CTRL_UNION
 结构说明  : LDI_PLR_CTRL 寄存器结构定义。地址偏移量:0x1010，初值:0x00000000，宽度:32
 寄存器说明: LDI信号极性控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vsync_plr     : 1;  /* bit[0]   : 帧同步信号ldi_vsync_o有效极性：
                                                        0：ldi_vsync_o高有效；
                                                        1：ldi_hsync_o低有效 */
        unsigned int  hsync_plr     : 1;  /* bit[1]   : 行同步信号ldi_hsync_o有效极性：
                                                        0：ldi_hsync_o高有效；
                                                        1：ldi_hsync_o低有效 */
        unsigned int  pixel_clk_plr : 1;  /* bit[2]   : 像素时钟ldi_pixel_clk有效极性：
                                                        0：ldi_pixel_clk上升沿有效；
                                                        1：ldi_pixel_clk下降沿有效 */
        unsigned int  data_en_plr   : 1;  /* bit[3]   : 数据有效信号ldi_data_en_o有效极性:
                                                        0：ldi_data_en_o高有效；
                                                        1：ldi_data_en_o低有效 */
        unsigned int  reserved      : 28; /* bit[4-31]:  */
    } reg;
} SOC_EDC_LDI_PLR_CTRL_UNION;
#endif
#define SOC_EDC_LDI_PLR_CTRL_vsync_plr_START      (0)
#define SOC_EDC_LDI_PLR_CTRL_vsync_plr_END        (0)
#define SOC_EDC_LDI_PLR_CTRL_hsync_plr_START      (1)
#define SOC_EDC_LDI_PLR_CTRL_hsync_plr_END        (1)
#define SOC_EDC_LDI_PLR_CTRL_pixel_clk_plr_START  (2)
#define SOC_EDC_LDI_PLR_CTRL_pixel_clk_plr_END    (2)
#define SOC_EDC_LDI_PLR_CTRL_data_en_plr_START    (3)
#define SOC_EDC_LDI_PLR_CTRL_data_en_plr_END      (3)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_DSP_SIZE_UNION
 结构说明  : LDI_DSP_SIZE 寄存器结构定义。地址偏移量:0x1014，初值:0x00000000，宽度:32
 寄存器说明: LDI显示屏尺寸寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hsize    : 12; /* bit[0-11] : 显示屏水平方向像素个数，写入值为实际值减1，取值范围为1~4096 */
        unsigned int  reserved : 8;  /* bit[12-19]:  */
        unsigned int  vsize    : 12; /* bit[20-31]: 显示屏垂直方向像素个数，写入值为实际值减1，取值范围为1~4096 */
    } reg;
} SOC_EDC_LDI_DSP_SIZE_UNION;
#endif
#define SOC_EDC_LDI_DSP_SIZE_hsize_START     (0)
#define SOC_EDC_LDI_DSP_SIZE_hsize_END       (11)
#define SOC_EDC_LDI_DSP_SIZE_vsize_START     (20)
#define SOC_EDC_LDI_DSP_SIZE_vsize_END       (31)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_3D_CTRL_UNION
 结构说明  : LDI_3D_CTRL 寄存器结构定义。地址偏移量:0x1018，初值:0x00000000，宽度:32
 寄存器说明: LDI 3D显示控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  active_space : 12; /* bit[0-11] : 帧与帧之间的active space时间配置，写入为实际值，取值范围为0~4095。（只用于3D 显示frame by frame格式）;Active space是以扫描行数为单位 */
        unsigned int  reserved     : 20; /* bit[12-31]:  */
    } reg;
} SOC_EDC_LDI_3D_CTRL_UNION;
#endif
#define SOC_EDC_LDI_3D_CTRL_active_space_START  (0)
#define SOC_EDC_LDI_3D_CTRL_active_space_END    (11)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_INT_EN_UNION
 结构说明  : LDI_INT_EN 寄存器结构定义。地址偏移量:0x101C，初值:0x00000000，宽度:32
 寄存器说明: LDI中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  frame_start_int_en         : 1;  /* bit[0]    : 帧开始中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  frame_end_int_en           : 1;  /* bit[1]    : 帧结束中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  edc_afifo_underflow_int_en : 1;  /* bit[2]    : 比较中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  vsync_int_en               : 1;  /* bit[3]    : 帧同步开始中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  vfrontporch_int_en         : 1;  /* bit[4]    : 帧前消隐开始中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  vfp_last_int_en            : 1;  /* bit[5]    : 帧后消隐开始中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  vactive0_start_int_en      : 1;  /* bit[6]    : 帧有效数据开始中断使能（2D模式下）或者
                                                                      左眼帧有效数据开始中断使能（3D模式下）：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  vactive0_end_int_en        : 1;  /* bit[7]    : 帧有效数据结束中断使能（2D模式下）或者
                                                                      左眼帧有效数据结束中断使能（3D模式下）：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  vactive1_start_int_en      : 1;  /* bit[8]    : 右眼帧有效数据开始中断使能，仅适用于3D模式：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  vactive1_end_int_en        : 1;  /* bit[9]    : 右眼帧有效数据结束中断使能，仅适用于3D模式：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  vfrontporch_end_int_en     : 1;  /* bit[10]   : 帧后消隐结束中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  dsi_te_tri_int_en          : 1;  /* bit[11]   : Tearing Effect Trigger中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  dsi_te0_pin_int_en         : 1;  /* bit[12]   : Tearing Effect管脚TE0中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  dsi_te1_pin_int_en         : 1;  /* bit[13]   : Tearing Effect管脚TE1中断使能：
                                                                      0：中断屏蔽；
                                                                      1：中断使能； */
        unsigned int  reserved                   : 18; /* bit[14-31]:  */
    } reg;
} SOC_EDC_LDI_INT_EN_UNION;
#endif
#define SOC_EDC_LDI_INT_EN_frame_start_int_en_START          (0)
#define SOC_EDC_LDI_INT_EN_frame_start_int_en_END            (0)
#define SOC_EDC_LDI_INT_EN_frame_end_int_en_START            (1)
#define SOC_EDC_LDI_INT_EN_frame_end_int_en_END              (1)
#define SOC_EDC_LDI_INT_EN_edc_afifo_underflow_int_en_START  (2)
#define SOC_EDC_LDI_INT_EN_edc_afifo_underflow_int_en_END    (2)
#define SOC_EDC_LDI_INT_EN_vsync_int_en_START                (3)
#define SOC_EDC_LDI_INT_EN_vsync_int_en_END                  (3)
#define SOC_EDC_LDI_INT_EN_vfrontporch_int_en_START          (4)
#define SOC_EDC_LDI_INT_EN_vfrontporch_int_en_END            (4)
#define SOC_EDC_LDI_INT_EN_vfp_last_int_en_START             (5)
#define SOC_EDC_LDI_INT_EN_vfp_last_int_en_END               (5)
#define SOC_EDC_LDI_INT_EN_vactive0_start_int_en_START       (6)
#define SOC_EDC_LDI_INT_EN_vactive0_start_int_en_END         (6)
#define SOC_EDC_LDI_INT_EN_vactive0_end_int_en_START         (7)
#define SOC_EDC_LDI_INT_EN_vactive0_end_int_en_END           (7)
#define SOC_EDC_LDI_INT_EN_vactive1_start_int_en_START       (8)
#define SOC_EDC_LDI_INT_EN_vactive1_start_int_en_END         (8)
#define SOC_EDC_LDI_INT_EN_vactive1_end_int_en_START         (9)
#define SOC_EDC_LDI_INT_EN_vactive1_end_int_en_END           (9)
#define SOC_EDC_LDI_INT_EN_vfrontporch_end_int_en_START      (10)
#define SOC_EDC_LDI_INT_EN_vfrontporch_end_int_en_END        (10)
#define SOC_EDC_LDI_INT_EN_dsi_te_tri_int_en_START           (11)
#define SOC_EDC_LDI_INT_EN_dsi_te_tri_int_en_END             (11)
#define SOC_EDC_LDI_INT_EN_dsi_te0_pin_int_en_START          (12)
#define SOC_EDC_LDI_INT_EN_dsi_te0_pin_int_en_END            (12)
#define SOC_EDC_LDI_INT_EN_dsi_te1_pin_int_en_START          (13)
#define SOC_EDC_LDI_INT_EN_dsi_te1_pin_int_en_END            (13)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_CTRL_UNION
 结构说明  : LDI_CTRL 寄存器结构定义。地址偏移量:0x1020，初值:0x00000000，宽度:32
 寄存器说明: LDI控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ldi_en          : 1;  /* bit[0]    : LDI使能 */
        unsigned int  disp_mode_buf   : 1;  /* bit[1]    : 显示模式。
                                                           0：2D 显示模式或者3D显示时非frame by frame模式，默认配置；
                                                           1：3D frame by frame模式； */
        unsigned int  date_gate_en    : 1;  /* bit[2]    : 数据有效信号门控功能使能。
                                                           0：不使能；
                                                           1：使能； */
        unsigned int  bpp             : 2;  /* bit[3-4]  : 输入像素深度配置。
                                                           00：输入RGB565像素格式；
                                                           01：输入RGB666像素格式；
                                                           10：输入RGB888像素格式；
                                                           11：保留 */
        unsigned int  wait_vsync_en   : 1;  /* bit[5]    : 等待ldi_vsync_other使能；
                                                           0：不使能；
                                                           1：使能； */
        unsigned int  corlorbar_width : 7;  /* bit[6-12] : corlorbar RGB彩条宽度，写入值为实际值减1，取值范围为1~128 */
        unsigned int  bgr             : 1;  /* bit[13]   : RGB输出配置:
                                                           0：普通的RGB输出格式；
                                                           1：BGR输出格式（Blue与Red分量对调） */
        unsigned int  color_mode      : 1;  /* bit[14]   : 给DSI DPI接口的Color Mode信号：
                                                           0： Full color Mode;
                                                           1： Reduced Color Mode */
        unsigned int  shutdown        : 1;  /* bit[15]   : 给DSI DPI接口的shutdown信号：
                                                           0： LCD正常显示;
                                                           1： LCD进入Sleep模式 */
        unsigned int  reserved        : 16; /* bit[16-31]:  */
    } reg;
} SOC_EDC_LDI_CTRL_UNION;
#endif
#define SOC_EDC_LDI_CTRL_ldi_en_START           (0)
#define SOC_EDC_LDI_CTRL_ldi_en_END             (0)
#define SOC_EDC_LDI_CTRL_disp_mode_buf_START    (1)
#define SOC_EDC_LDI_CTRL_disp_mode_buf_END      (1)
#define SOC_EDC_LDI_CTRL_date_gate_en_START     (2)
#define SOC_EDC_LDI_CTRL_date_gate_en_END       (2)
#define SOC_EDC_LDI_CTRL_bpp_START              (3)
#define SOC_EDC_LDI_CTRL_bpp_END                (4)
#define SOC_EDC_LDI_CTRL_wait_vsync_en_START    (5)
#define SOC_EDC_LDI_CTRL_wait_vsync_en_END      (5)
#define SOC_EDC_LDI_CTRL_corlorbar_width_START  (6)
#define SOC_EDC_LDI_CTRL_corlorbar_width_END    (12)
#define SOC_EDC_LDI_CTRL_bgr_START              (13)
#define SOC_EDC_LDI_CTRL_bgr_END                (13)
#define SOC_EDC_LDI_CTRL_color_mode_START       (14)
#define SOC_EDC_LDI_CTRL_color_mode_END         (14)
#define SOC_EDC_LDI_CTRL_shutdown_START         (15)
#define SOC_EDC_LDI_CTRL_shutdown_END           (15)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_ORG_INT_UNION
 结构说明  : LDI_ORG_INT 寄存器结构定义。地址偏移量:0x1024，初值:0x00000000，宽度:32
 寄存器说明: LDI原始中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  frame_start_int         : 1;  /* bit[0]    : 帧开始中断原始状态。
                                                                   该位不受中断使能位屏蔽，当帧开始时该位置1 */
        unsigned int  frame_end_int           : 1;  /* bit[1]    : 帧结束中断原始状态。
                                                                   该位不受中断使能位屏蔽，当帧结束时该位置1 */
        unsigned int  edc_afifo_underflow_int : 1;  /* bit[2]    : 异步FIFO下溢出中断原始状态。
                                                                   该位不受中断使能位屏蔽，当异步FIFO下溢出时该位置1。 */
        unsigned int  vsync_int               : 1;  /* bit[3]    : 帧同步开始中断原始状态。
                                                                   该位不受中断使能位屏蔽，当帧同步开始时该位置1； */
        unsigned int  vfrontporch_int         : 1;  /* bit[4]    : 帧后消隐开始中断原始状态。
                                                                   该位不受中断使能位屏蔽，当帧后消隐开始时该位置1 */
        unsigned int  vbackporch_int          : 1;  /* bit[5]    : 帧前消隐最后行开始中断原始状态，
                                                                   该位不受中断使能位屏蔽，当帧前消隐开始时该位置1 */
        unsigned int  vactive0_start_int      : 1;  /* bit[6]    : 帧有效数据开始中断原始状态（2D模式下）或者帧左眼帧有效数据开始中断原始状态（3D模式下）,
                                                                   该位不受中断使能位屏蔽，当帧有效数据开始时该位置1 */
        unsigned int  vactive0_end_int        : 1;  /* bit[7]    : 帧有效数据结束中断原始状态（2D模式下）或者帧左眼帧有效数据结束中断原始状态（3D模式下）;
                                                                   该位不受中断使能位屏蔽，当帧有效数据结束时该位置1 */
        unsigned int  vactive1_start_int      : 1;  /* bit[8]    : 右眼帧有效数据开始中断原始状态，仅适用于3D模式
                                                                   该位不受中断使能位屏蔽，当右眼帧有效数据开始时该位置1. */
        unsigned int  vactive1_end_int        : 1;  /* bit[9]    : 右眼帧有效数据结束中断原始状态，仅适用于3D模式
                                                                   该位不受中断使能位屏蔽，当右眼帧有效数据结束时该位置1. */
        unsigned int  vfrontporch_end_int     : 1;  /* bit[10]   : 帧后消隐结束中断原始状态。
                                                                   该位不受中断使能位屏蔽，当帧后消隐结束时该位置1 */
        unsigned int  dsi_te_tri_int          : 1;  /* bit[11]   : Tearing Effect Trigger中断原始状态。
                                                                   该位不受中断使能位屏蔽，当DSI Host收到Tearing Effect Trigger信号时给LDI指示信号时该位置1 */
        unsigned int  dsi_te0_pin_int         : 1;  /* bit[12]   : Tearing Effect管脚TE0中断原始状态。
                                                                   该位不受中断使能位屏蔽，当管脚TE0检测到有效Tearing Effect信号时该位置1 */
        unsigned int  dsi_te1_pin_int         : 1;  /* bit[13]   : Tearing Effect管脚TE1中断原始状态。
                                                                   该位不受中断使能位屏蔽，当管脚TE1检测到有效Tearing Effect信号时该位置1 */
        unsigned int  reserved                : 18; /* bit[14-31]:  */
    } reg;
} SOC_EDC_LDI_ORG_INT_UNION;
#endif
#define SOC_EDC_LDI_ORG_INT_frame_start_int_START          (0)
#define SOC_EDC_LDI_ORG_INT_frame_start_int_END            (0)
#define SOC_EDC_LDI_ORG_INT_frame_end_int_START            (1)
#define SOC_EDC_LDI_ORG_INT_frame_end_int_END              (1)
#define SOC_EDC_LDI_ORG_INT_edc_afifo_underflow_int_START  (2)
#define SOC_EDC_LDI_ORG_INT_edc_afifo_underflow_int_END    (2)
#define SOC_EDC_LDI_ORG_INT_vsync_int_START                (3)
#define SOC_EDC_LDI_ORG_INT_vsync_int_END                  (3)
#define SOC_EDC_LDI_ORG_INT_vfrontporch_int_START          (4)
#define SOC_EDC_LDI_ORG_INT_vfrontporch_int_END            (4)
#define SOC_EDC_LDI_ORG_INT_vbackporch_int_START           (5)
#define SOC_EDC_LDI_ORG_INT_vbackporch_int_END             (5)
#define SOC_EDC_LDI_ORG_INT_vactive0_start_int_START       (6)
#define SOC_EDC_LDI_ORG_INT_vactive0_start_int_END         (6)
#define SOC_EDC_LDI_ORG_INT_vactive0_end_int_START         (7)
#define SOC_EDC_LDI_ORG_INT_vactive0_end_int_END           (7)
#define SOC_EDC_LDI_ORG_INT_vactive1_start_int_START       (8)
#define SOC_EDC_LDI_ORG_INT_vactive1_start_int_END         (8)
#define SOC_EDC_LDI_ORG_INT_vactive1_end_int_START         (9)
#define SOC_EDC_LDI_ORG_INT_vactive1_end_int_END           (9)
#define SOC_EDC_LDI_ORG_INT_vfrontporch_end_int_START      (10)
#define SOC_EDC_LDI_ORG_INT_vfrontporch_end_int_END        (10)
#define SOC_EDC_LDI_ORG_INT_dsi_te_tri_int_START           (11)
#define SOC_EDC_LDI_ORG_INT_dsi_te_tri_int_END             (11)
#define SOC_EDC_LDI_ORG_INT_dsi_te0_pin_int_START          (12)
#define SOC_EDC_LDI_ORG_INT_dsi_te0_pin_int_END            (12)
#define SOC_EDC_LDI_ORG_INT_dsi_te1_pin_int_START          (13)
#define SOC_EDC_LDI_ORG_INT_dsi_te1_pin_int_END            (13)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_MSK_INT_UNION
 结构说明  : LDI_MSK_INT 寄存器结构定义。地址偏移量:0x1028，初值:0x00000000，宽度:32
 寄存器说明: LDI屏蔽后的中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  frame_start_int         : 1;  /* bit[0]    : 帧开始中断屏蔽后状态 */
        unsigned int  frame_end_int           : 1;  /* bit[1]    : 帧结束中断屏蔽后状态 */
        unsigned int  edc_afifo_underflow_int : 1;  /* bit[2]    : edc异步FIFO下溢出中断屏蔽后状态 */
        unsigned int  vsync_int               : 1;  /* bit[3]    : 帧同步开始中断屏蔽后状态 */
        unsigned int  vfrontporch_int         : 1;  /* bit[4]    : 帧后消隐开始中断屏蔽后状态 */
        unsigned int  vbackport_int           : 1;  /* bit[5]    : 帧前消隐最后行开始中断屏蔽后状态 */
        unsigned int  vactive0_start_int      : 1;  /* bit[6]    : 帧有效数据开始中断屏蔽后状态（2D模式下）或者左眼帧有效数据开始中断屏蔽后状态（3D模式下） */
        unsigned int  vactive0_end_int        : 1;  /* bit[7]    : 帧有效数据结束中断屏蔽后状态（2D模式下）或者左眼帧有效数据结束中断屏蔽后状态（3D模式下） */
        unsigned int  vactive1_start_int      : 1;  /* bit[8]    : 右眼帧有效数据开始中断屏蔽后状态，仅适用于3D模式 */
        unsigned int  vactive1_end_int        : 1;  /* bit[9]    : 右眼帧有效数据结束中断屏蔽后状态，仅适用于3D模式 */
        unsigned int  vfrontporch_end_int     : 1;  /* bit[10]   : 帧后消隐结束中断原始状态。 */
        unsigned int  dsi_te_tri_int          : 1;  /* bit[11]   : Tearing Effect Trigger中断原始状态。 */
        unsigned int  dsi_te0_pin_int         : 1;  /* bit[12]   : Tearing Effect管脚TE0中断原始状态。 */
        unsigned int  dsi_te1_pin_int         : 1;  /* bit[13]   : Tearing Effect管脚TE1中断原始状态。 */
        unsigned int  reserved                : 18; /* bit[14-31]:  */
    } reg;
} SOC_EDC_LDI_MSK_INT_UNION;
#endif
#define SOC_EDC_LDI_MSK_INT_frame_start_int_START          (0)
#define SOC_EDC_LDI_MSK_INT_frame_start_int_END            (0)
#define SOC_EDC_LDI_MSK_INT_frame_end_int_START            (1)
#define SOC_EDC_LDI_MSK_INT_frame_end_int_END              (1)
#define SOC_EDC_LDI_MSK_INT_edc_afifo_underflow_int_START  (2)
#define SOC_EDC_LDI_MSK_INT_edc_afifo_underflow_int_END    (2)
#define SOC_EDC_LDI_MSK_INT_vsync_int_START                (3)
#define SOC_EDC_LDI_MSK_INT_vsync_int_END                  (3)
#define SOC_EDC_LDI_MSK_INT_vfrontporch_int_START          (4)
#define SOC_EDC_LDI_MSK_INT_vfrontporch_int_END            (4)
#define SOC_EDC_LDI_MSK_INT_vbackport_int_START            (5)
#define SOC_EDC_LDI_MSK_INT_vbackport_int_END              (5)
#define SOC_EDC_LDI_MSK_INT_vactive0_start_int_START       (6)
#define SOC_EDC_LDI_MSK_INT_vactive0_start_int_END         (6)
#define SOC_EDC_LDI_MSK_INT_vactive0_end_int_START         (7)
#define SOC_EDC_LDI_MSK_INT_vactive0_end_int_END           (7)
#define SOC_EDC_LDI_MSK_INT_vactive1_start_int_START       (8)
#define SOC_EDC_LDI_MSK_INT_vactive1_start_int_END         (8)
#define SOC_EDC_LDI_MSK_INT_vactive1_end_int_START         (9)
#define SOC_EDC_LDI_MSK_INT_vactive1_end_int_END           (9)
#define SOC_EDC_LDI_MSK_INT_vfrontporch_end_int_START      (10)
#define SOC_EDC_LDI_MSK_INT_vfrontporch_end_int_END        (10)
#define SOC_EDC_LDI_MSK_INT_dsi_te_tri_int_START           (11)
#define SOC_EDC_LDI_MSK_INT_dsi_te_tri_int_END             (11)
#define SOC_EDC_LDI_MSK_INT_dsi_te0_pin_int_START          (12)
#define SOC_EDC_LDI_MSK_INT_dsi_te0_pin_int_END            (12)
#define SOC_EDC_LDI_MSK_INT_dsi_te1_pin_int_START          (13)
#define SOC_EDC_LDI_MSK_INT_dsi_te1_pin_int_END            (13)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_INT_CLR_UNION
 结构说明  : LDI_INT_CLR 寄存器结构定义。地址偏移量:0x102C，初值:0x00000000，宽度:32
 寄存器说明: LDI中断清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  frame_start_int_clr         : 1;  /* bit[0]    : 帧开始中断清除：
                                                                       写1，清除帧开始中断 */
        unsigned int  frame_end_int_clr           : 1;  /* bit[1]    : 帧结束中断清除：
                                                                       写1，清除帧结束中断 */
        unsigned int  edc_afifo_underflow_int_clr : 1;  /* bit[2]    : edc异步FIFO下溢出中断清除：
                                                                       写1，清除异步FIFO下溢出中断 */
        unsigned int  vsync_int_clr               : 1;  /* bit[3]    : 帧同步开始中断清除：
                                                                       写1，清除帧同步开始中断 */
        unsigned int  vbackporch_int_clr          : 1;  /* bit[4]    : 帧后消隐开始中断清除：
                                                                       写1，清除帧后消隐开始中断 */
        unsigned int  vfrontporch_int_clr         : 1;  /* bit[5]    : 帧前消隐开始中断清除：
                                                                       写1，清除帧前消隐开始中断 */
        unsigned int  vactive0_start_int_clr      : 1;  /* bit[6]    : 帧有效数据开始中断清除（2D模式下）或者左眼帧有效数据开始中断清除：
                                                                       写1，清除帧有效数据开始中断（2D模式下），左眼帧数据开始中断（3D模式下） */
        unsigned int  vactive0_end_int_clr        : 1;  /* bit[7]    : 帧有效数据结束中断清除（2D模式下）或者左眼帧有效数据结束中断清除：
                                                                       写1，清除帧有效数据结束中断（2D模式下），左眼帧数据结束中断（3D模式下） */
        unsigned int  vactive1_start_int_clr      : 1;  /* bit[8]    : 右眼帧有效数据开始中断清除：
                                                                       写1，清除右眼帧有效数据开始中断 */
        unsigned int  vactive1_end_int_clr        : 1;  /* bit[9]    : 右眼帧有效数据结束中断清除：
                                                                       写1，清除右眼帧有效数据结束中断 */
        unsigned int  vfrontporch_end_int_clr     : 1;  /* bit[10]   : 帧后消隐结束中断清除：
                                                                       写1，清除右眼帧有效数据结束中断 */
        unsigned int  dsi_te_tri_int_clr          : 1;  /* bit[11]   : Tearing Effect Trigger中断清除：
                                                                       写1，清除右眼帧有效数据结束中断 */
        unsigned int  dsi_te0_pin_int_clr         : 1;  /* bit[12]   : Tearing Effect管脚TE0中断清除：
                                                                       写1，清除右眼帧有效数据结束中断 */
        unsigned int  dsi_te1_pin_int_clr         : 1;  /* bit[13]   : Tearing Effect管脚TE1中断清除：
                                                                       写1，清除右眼帧有效数据结束中断 */
        unsigned int  reserved                    : 18; /* bit[14-31]:  */
    } reg;
} SOC_EDC_LDI_INT_CLR_UNION;
#endif
#define SOC_EDC_LDI_INT_CLR_frame_start_int_clr_START          (0)
#define SOC_EDC_LDI_INT_CLR_frame_start_int_clr_END            (0)
#define SOC_EDC_LDI_INT_CLR_frame_end_int_clr_START            (1)
#define SOC_EDC_LDI_INT_CLR_frame_end_int_clr_END              (1)
#define SOC_EDC_LDI_INT_CLR_edc_afifo_underflow_int_clr_START  (2)
#define SOC_EDC_LDI_INT_CLR_edc_afifo_underflow_int_clr_END    (2)
#define SOC_EDC_LDI_INT_CLR_vsync_int_clr_START                (3)
#define SOC_EDC_LDI_INT_CLR_vsync_int_clr_END                  (3)
#define SOC_EDC_LDI_INT_CLR_vbackporch_int_clr_START           (4)
#define SOC_EDC_LDI_INT_CLR_vbackporch_int_clr_END             (4)
#define SOC_EDC_LDI_INT_CLR_vfrontporch_int_clr_START          (5)
#define SOC_EDC_LDI_INT_CLR_vfrontporch_int_clr_END            (5)
#define SOC_EDC_LDI_INT_CLR_vactive0_start_int_clr_START       (6)
#define SOC_EDC_LDI_INT_CLR_vactive0_start_int_clr_END         (6)
#define SOC_EDC_LDI_INT_CLR_vactive0_end_int_clr_START         (7)
#define SOC_EDC_LDI_INT_CLR_vactive0_end_int_clr_END           (7)
#define SOC_EDC_LDI_INT_CLR_vactive1_start_int_clr_START       (8)
#define SOC_EDC_LDI_INT_CLR_vactive1_start_int_clr_END         (8)
#define SOC_EDC_LDI_INT_CLR_vactive1_end_int_clr_START         (9)
#define SOC_EDC_LDI_INT_CLR_vactive1_end_int_clr_END           (9)
#define SOC_EDC_LDI_INT_CLR_vfrontporch_end_int_clr_START      (10)
#define SOC_EDC_LDI_INT_CLR_vfrontporch_end_int_clr_END        (10)
#define SOC_EDC_LDI_INT_CLR_dsi_te_tri_int_clr_START           (11)
#define SOC_EDC_LDI_INT_CLR_dsi_te_tri_int_clr_END             (11)
#define SOC_EDC_LDI_INT_CLR_dsi_te0_pin_int_clr_START          (12)
#define SOC_EDC_LDI_INT_CLR_dsi_te0_pin_int_clr_END            (12)
#define SOC_EDC_LDI_INT_CLR_dsi_te1_pin_int_clr_START          (13)
#define SOC_EDC_LDI_INT_CLR_dsi_te1_pin_int_clr_END            (13)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_WORK_MODE_UNION
 结构说明  : LDI_WORK_MODE 寄存器结构定义。地址偏移量:0x1030，初值:0x00000000，宽度:32
 寄存器说明: LDI回写控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  work_mode   : 1;  /* bit[0]   : 工作模式：
                                                      0：测试模式；
                                                      1：正常工作模式 */
        unsigned int  wback_en    : 1;  /* bit[1]   : 回写使能 */
        unsigned int  colorbar_en : 1;  /* bit[2]   : colorbar使能信号。为高时，LDI内部自动产生红、绿、蓝彩带， 用于LCD屏调试：
                                                      只有在测试模式下有效 */
        unsigned int  reserved    : 29; /* bit[3-31]:  */
    } reg;
} SOC_EDC_LDI_WORK_MODE_UNION;
#endif
#define SOC_EDC_LDI_WORK_MODE_work_mode_START    (0)
#define SOC_EDC_LDI_WORK_MODE_work_mode_END      (0)
#define SOC_EDC_LDI_WORK_MODE_wback_en_START     (1)
#define SOC_EDC_LDI_WORK_MODE_wback_en_END       (1)
#define SOC_EDC_LDI_WORK_MODE_colorbar_en_START  (2)
#define SOC_EDC_LDI_WORK_MODE_colorbar_en_END    (2)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_HDMI_DSI_GT_UNION
 结构说明  : LDI_HDMI_DSI_GT 寄存器结构定义。地址偏移量:0x1034，初值:0x00000000，宽度:32
 寄存器说明: HDMI/DSI像素时钟门控寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dsi_pclk_off  : 1;  /* bit[0]   : DSI像素时钟关闭：
                                                        0： DSI像素时钟打开；
                                                        1： DSI像素时钟关闭 */
        unsigned int  hdmi_pclk_off : 1;  /* bit[1]   : HDMI像素时钟关闭：
                                                        0： HDMI像素时钟打开；
                                                        1： HDMI像素时钟关闭 */
        unsigned int  reserved      : 30; /* bit[2-31]:  */
    } reg;
} SOC_EDC_LDI_HDMI_DSI_GT_UNION;
#endif
#define SOC_EDC_LDI_HDMI_DSI_GT_dsi_pclk_off_START   (0)
#define SOC_EDC_LDI_HDMI_DSI_GT_dsi_pclk_off_END     (0)
#define SOC_EDC_LDI_HDMI_DSI_GT_hdmi_pclk_off_START  (1)
#define SOC_EDC_LDI_HDMI_DSI_GT_hdmi_pclk_off_END    (1)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_DE_SPACE_LOW_UNION
 结构说明  : LDI_DE_SPACE_LOW 寄存器结构定义。地址偏移量:0x1038，初值:0x00000000，宽度:32
 寄存器说明: 3D frame by frame格式下active space阶段DE信号有效控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  de_space_low      : 1;  /* bit[0]   : DE信号在active space阶段是否有效：
                                                            0：DE信号在active space阶段为高；
                                                            1：DE信号在active space阶段为低；
                                                            该寄存器只在3D frame by frame格式下有效，2D格式下无效 */
        unsigned int  clr_ldi_en_bypass : 1;  /* bit[1]   : 硬件自清零LDI_EN旁路使能：
                                                            0： 使能硬件自清零LDI_EN功能：当vfp_last_int_en有效，且VFP开始时将LDI_EN自动清零；
                                                            1： 关闭硬件自清零功能 */
        unsigned int  reserved          : 30; /* bit[2-31]:  */
    } reg;
} SOC_EDC_LDI_DE_SPACE_LOW_UNION;
#endif
#define SOC_EDC_LDI_DE_SPACE_LOW_de_space_low_START       (0)
#define SOC_EDC_LDI_DE_SPACE_LOW_de_space_low_END         (0)
#define SOC_EDC_LDI_DE_SPACE_LOW_clr_ldi_en_bypass_START  (1)
#define SOC_EDC_LDI_DE_SPACE_LOW_clr_ldi_en_bypass_END    (1)


/*****************************************************************************
 结构名    : SOC_EDC_DSI_CMD_MOD_CTRL_UNION
 结构说明  : DSI_CMD_MOD_CTRL 寄存器结构定义。地址偏移量:0x103C，初值:0x00000000，宽度:32
 寄存器说明: DSI Command(CMD)模式控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dsi_halt_cmd_en   : 1;  /* bit[0]   : 在CMD模式下HALT信号的使能
                                                            0：不使能Halt信号，在内部不看这个信号
                                                            1：使能Halt信号 */
        unsigned int  dsi_halt_video_en : 1;  /* bit[1]   : 在Video模式下HALT信号的使能,用于last line拉长使能
                                                            0：不使能Halt信号，在内部不看这个信号
                                                            1：使能Halt信号 */
        unsigned int  dsi_wms_3d_mode   : 1;  /* bit[2]   : 在3D模式下WMS信号的模式（frame by frame）
                                                            0 分离WMS模式
                                                            1 连续WMS模式 */
        unsigned int  reserved          : 29; /* bit[3-31]:  */
    } reg;
} SOC_EDC_DSI_CMD_MOD_CTRL_UNION;
#endif
#define SOC_EDC_DSI_CMD_MOD_CTRL_dsi_halt_cmd_en_START    (0)
#define SOC_EDC_DSI_CMD_MOD_CTRL_dsi_halt_cmd_en_END      (0)
#define SOC_EDC_DSI_CMD_MOD_CTRL_dsi_halt_video_en_START  (1)
#define SOC_EDC_DSI_CMD_MOD_CTRL_dsi_halt_video_en_END    (1)
#define SOC_EDC_DSI_CMD_MOD_CTRL_dsi_wms_3d_mode_START    (2)
#define SOC_EDC_DSI_CMD_MOD_CTRL_dsi_wms_3d_mode_END      (2)


/*****************************************************************************
 结构名    : SOC_EDC_DSI_TE_CTRL_UNION
 结构说明  : DSI_TE_CTRL 寄存器结构定义。地址偏移量:0x1040，初值:0x00000020，宽度:32
 寄存器说明: DSI Command(CMD)模式TE控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dsi_te_hard_en    : 1;  /* bit[0]    : TE硬件模式使能信号
                                                             0：不使能TE硬件模式
                                                             1：使能TE硬件模式
                                                             每当在TE Mask模式并且是按比例做FRC（帧率切换）的模式开和关时需要先Disable这个寄存器。 */
        unsigned int  dsi_te0_pin_p     : 1;  /* bit[1]    : TE0 极性选择
                                                             0：保持输入极性
                                                             1：输入极性取反 */
        unsigned int  dsi_te1_pin_p     : 1;  /* bit[2]    : TE1 极性选择
                                                             0：保持输入极性
                                                             1：输入极性取反 */
        unsigned int  dsi_te_hard_sel   : 1;  /* bit[3]    : TE硬件模式源选择
                                                             0：从管脚输入的TE PIN源
                                                             1：从MIPI接口上读的TE Trigger源 */
        unsigned int  dsi_te_pin_hd_sel : 1;  /* bit[4]    : TE硬件模式下，TE PIN选择
                                                             0：TE0 PIN
                                                             1：TE1 PIN */
        unsigned int  dsi_te_mask_en    : 1;  /* bit[5]    : TE屏蔽使能（只用于Hard模式，soft模式只需忽略中断即可，不需此功能）
                                                             0：屏蔽TE信号关闭
                                                             1：屏蔽TE信号功能打开 */
        unsigned int  dsi_te_mask_dis   : 4;  /* bit[6-9]  : TE屏蔽时显示的TE个数
                                                             例如等于6时，开始的6个TE信号显示数据到屏，default 1 */
        unsigned int  dsi_te_mask_und   : 4;  /* bit[10-13]: TE屏蔽时屏蔽的TE个数
                                                             例如等于6时，接着显示的TE个数后面的6个TE信号被，default 1 */
        unsigned int  dsi_te_pin_en     : 1;  /* bit[14]   : 在CMD模式下用TE PIN（管脚）检测使能
                                                             0 关闭TE PIN检测逻辑，用于Video模式或CMD模式不用TE PIN场景
                                                             1 启动TE PIN检测逻辑，用于CMD模式并且用TE PIN场景 */
        unsigned int  reserved          : 17; /* bit[15-31]:  */
    } reg;
} SOC_EDC_DSI_TE_CTRL_UNION;
#endif
#define SOC_EDC_DSI_TE_CTRL_dsi_te_hard_en_START     (0)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_hard_en_END       (0)
#define SOC_EDC_DSI_TE_CTRL_dsi_te0_pin_p_START      (1)
#define SOC_EDC_DSI_TE_CTRL_dsi_te0_pin_p_END        (1)
#define SOC_EDC_DSI_TE_CTRL_dsi_te1_pin_p_START      (2)
#define SOC_EDC_DSI_TE_CTRL_dsi_te1_pin_p_END        (2)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_hard_sel_START    (3)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_hard_sel_END      (3)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_pin_hd_sel_START  (4)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_pin_hd_sel_END    (4)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_mask_en_START     (5)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_mask_en_END       (5)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_mask_dis_START    (6)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_mask_dis_END      (9)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_mask_und_START    (10)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_mask_und_END      (13)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_pin_en_START      (14)
#define SOC_EDC_DSI_TE_CTRL_dsi_te_pin_en_END        (14)


/*****************************************************************************
 结构名    : SOC_EDC_DSI_TE_HS_NUM_UNION
 结构说明  : DSI_TE_HS_NUM 寄存器结构定义。地址偏移量:0x1044，初值:0x00000000，宽度:32
 寄存器说明: DSI响应行配置寄存器控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dsi_te0_hs_num : 13; /* bit[0-12] : 用于在TE0信号在Mode2(VS+HS)模式下，设置在哪个HS时开始发送数据。
                                                          设置为0有特殊含义，即此模式用VS发送数据不看HS。
                                                          实际发送值为DSI_TE1_HS_NUM + 1行发送。 */
        unsigned int  dsi_te1_hs_num : 13; /* bit[13-25]: 用于在TE1信号在Mode2(VS+HS)模式下，设置在哪个HS时开始发送数据。
                                                          设置为0有特殊含义，即此模式用VS发送数据不看HS。
                                                          实际发送值为DSI_TE1_HS_NUM + 1行发送。 */
        unsigned int  reserved       : 6;  /* bit[26-31]:  */
    } reg;
} SOC_EDC_DSI_TE_HS_NUM_UNION;
#endif
#define SOC_EDC_DSI_TE_HS_NUM_dsi_te0_hs_num_START  (0)
#define SOC_EDC_DSI_TE_HS_NUM_dsi_te0_hs_num_END    (12)
#define SOC_EDC_DSI_TE_HS_NUM_dsi_te1_hs_num_START  (13)
#define SOC_EDC_DSI_TE_HS_NUM_dsi_te1_hs_num_END    (25)


/*****************************************************************************
 结构名    : SOC_EDC_DSI_TE_HS_WD_UNION
 结构说明  : DSI_TE_HS_WD 寄存器结构定义。地址偏移量:0x1048，初值:0x00003003，宽度:32
 寄存器说明: DSI TE PIN模式检测Hsync宽度控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dsi_te0_hs_wd : 12; /* bit[0-11] : 用于检测TE0信号在Mode2(VS+HS)模式下，HS信号的判别，当进来的TE信号有效区域大于这个寄存器设置是认为这个HS信号。
                                                         这个寄存器的设置值必须小于DSI_TE1_VS_WD的值。 */
        unsigned int  dsi_te1_hs_wd : 12; /* bit[12-23]: 用于检测TE1信号在Mode2(VS+HS)模式下，HS信号的判别，当进来的TE信号有效区域大于这个寄存器设置是认为这个HS信号。
                                                         这个寄存器的设置值必须小于DSI_TE0_VS_WD的值。 */
        unsigned int  reserved      : 8;  /* bit[24-31]:  */
    } reg;
} SOC_EDC_DSI_TE_HS_WD_UNION;
#endif
#define SOC_EDC_DSI_TE_HS_WD_dsi_te0_hs_wd_START  (0)
#define SOC_EDC_DSI_TE_HS_WD_dsi_te0_hs_wd_END    (11)
#define SOC_EDC_DSI_TE_HS_WD_dsi_te1_hs_wd_START  (12)
#define SOC_EDC_DSI_TE_HS_WD_dsi_te1_hs_wd_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_DSI_TE_VS_WD_UNION
 结构说明  : DSI_TE_VS_WD 寄存器结构定义。地址偏移量:0x104C，初值:0x00080080，宽度:32
 寄存器说明: DSI TE PIN模式检测Vsync宽度控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dsi_te0_vs_wd : 12; /* bit[0-11] : 用于检测TE0信号在Mode2(VS+HS)模式下，VS信号的判别，当进来的TE信号有效区域大于这个寄存器设置是认为这个VS信号 */
        unsigned int  dsi_te1_vs_wd : 12; /* bit[12-23]: 用于检测TE1信号在Mode2(VS+HS)模式下，VS信号的判别，当进来的TE信号有效区域大于这个寄存器设置是认为这个VS信号，此寄存器设置必须大于DSI_TE1_HS_WD， */
        unsigned int  reserved      : 8;  /* bit[24-31]:  */
    } reg;
} SOC_EDC_DSI_TE_VS_WD_UNION;
#endif
#define SOC_EDC_DSI_TE_VS_WD_dsi_te0_vs_wd_START  (0)
#define SOC_EDC_DSI_TE_VS_WD_dsi_te0_vs_wd_END    (11)
#define SOC_EDC_DSI_TE_VS_WD_dsi_te1_vs_wd_START  (12)
#define SOC_EDC_DSI_TE_VS_WD_dsi_te1_vs_wd_END    (23)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_MCU_INTS_UNION
 结构说明  : LDI_MCU_INTS 寄存器结构定义。地址偏移量:0x1050，初值:0x00000000，宽度:32
 寄存器说明: 给MCU的中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_frm_end_ints : 1;  /* bit[0]   : 帧结束中断状态位 */
        unsigned int  reserved         : 31; /* bit[1-31]:  */
    } reg;
} SOC_EDC_LDI_MCU_INTS_UNION;
#endif
#define SOC_EDC_LDI_MCU_INTS_mcu_frm_end_ints_START  (0)
#define SOC_EDC_LDI_MCU_INTS_mcu_frm_end_ints_END    (0)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_MCU_INTE_UNION
 结构说明  : LDI_MCU_INTE 寄存器结构定义。地址偏移量:0x1054，初值:0x00000000，宽度:32
 寄存器说明: 给MCU的中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_frm_end_int_en : 1;  /* bit[0]   : 帧结束中断使能：
                                                             0：中断屏蔽；
                                                             1：中断使能； */
        unsigned int  reserved           : 31; /* bit[1-31]:  */
    } reg;
} SOC_EDC_LDI_MCU_INTE_UNION;
#endif
#define SOC_EDC_LDI_MCU_INTE_mcu_frm_end_int_en_START  (0)
#define SOC_EDC_LDI_MCU_INTE_mcu_frm_end_int_en_END    (0)


/*****************************************************************************
 结构名    : SOC_EDC_LDI_MCU_INTC_UNION
 结构说明  : LDI_MCU_INTC 寄存器结构定义。地址偏移量:0x1058，初值:0x00000000，宽度:32
 寄存器说明: 给MCU的中断清零寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mcu_frm_end_clr : 1;  /* bit[0]   : 帧结束中断清除位，写1清中断 */
        unsigned int  reserved        : 31; /* bit[1-31]:  */
    } reg;
} SOC_EDC_LDI_MCU_INTC_UNION;
#endif
#define SOC_EDC_LDI_MCU_INTC_mcu_frm_end_clr_START  (0)
#define SOC_EDC_LDI_MCU_INTC_mcu_frm_end_clr_END    (0)


/*****************************************************************************
 结构名    : SOC_EDC_CONTROL_REG0_UNION
 结构说明  : CONTROL_REG0 寄存器结构定义。地址偏移量:0x2000，初值:0x0000000B，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_format : 2;  /* bit[0-1] :  */
        unsigned int  reserved    : 30; /* bit[2-31]:  */
    } reg;
} SOC_EDC_CONTROL_REG0_UNION;
#endif
#define SOC_EDC_CONTROL_REG0_data_format_START  (0)
#define SOC_EDC_CONTROL_REG0_data_format_END    (1)


/*****************************************************************************
 结构名    : SOC_EDC_CONTROL_REG1_UNION
 结构说明  : CONTROL_REG1 寄存器结构定义。地址偏移量:0x2004，初值:0x00000094，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 6;  /* bit[0-5] :  */
        unsigned int  auto_pos  : 1;  /* bit[6]   :  */
        unsigned int  auto_size : 1;  /* bit[7]   :  */
        unsigned int  reserved_1: 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_CONTROL_REG1_UNION;
#endif
#define SOC_EDC_CONTROL_REG1_auto_pos_START   (6)
#define SOC_EDC_CONTROL_REG1_auto_pos_END     (6)
#define SOC_EDC_CONTROL_REG1_auto_size_START  (7)
#define SOC_EDC_CONTROL_REG1_auto_size_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_HS_POS0_UNION
 结构说明  : HS_POS0 寄存器结构定义。地址偏移量:0x2008，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pos_7_0_ : 8;  /* bit[0-7] :  */
        unsigned int  reserved : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_HS_POS0_UNION;
#endif
#define SOC_EDC_HS_POS0_pos_7_0__START  (0)
#define SOC_EDC_HS_POS0_pos_7_0__END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_HS_POS1_UNION
 结构说明  : HS_POS1 寄存器结构定义。地址偏移量:0x200C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pos_15_8_ : 8;  /* bit[0-7] :  */
        unsigned int  reserved  : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_HS_POS1_UNION;
#endif
#define SOC_EDC_HS_POS1_pos_15_8__START  (0)
#define SOC_EDC_HS_POS1_pos_15_8__END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_FRAME_WIDTH0_UNION
 结构说明  : FRAME_WIDTH0 寄存器结构定义。地址偏移量:0x2010，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  width_7_0 : 8;  /* bit[0-7] :  */
        unsigned int  reserved  : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_FRAME_WIDTH0_UNION;
#endif
#define SOC_EDC_FRAME_WIDTH0_width_7_0_START  (0)
#define SOC_EDC_FRAME_WIDTH0_width_7_0_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_FRAME_WIDTH1_UNION
 结构说明  : FRAME_WIDTH1 寄存器结构定义。地址偏移量:0x2014，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  width_15_8 : 8;  /* bit[0-7] :  */
        unsigned int  reserved   : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_FRAME_WIDTH1_UNION;
#endif
#define SOC_EDC_FRAME_WIDTH1_width_15_8_START  (0)
#define SOC_EDC_FRAME_WIDTH1_width_15_8_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_FRAME_HEIGHT0_UNION
 结构说明  : FRAME_HEIGHT0 寄存器结构定义。地址偏移量:0x2018，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  height_7_0 : 8;  /* bit[0-7] :  */
        unsigned int  reserved   : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_FRAME_HEIGHT0_UNION;
#endif
#define SOC_EDC_FRAME_HEIGHT0_height_7_0_START  (0)
#define SOC_EDC_FRAME_HEIGHT0_height_7_0_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_FRAME_HEIGHT1_UNION
 结构说明  : FRAME_HEIGHT1 寄存器结构定义。地址偏移量:0x201C，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  height_15_8 : 8;  /* bit[0-7] :  */
        unsigned int  reserved    : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_FRAME_HEIGHT1_UNION;
#endif
#define SOC_EDC_FRAME_HEIGHT1_height_15_8_START  (0)
#define SOC_EDC_FRAME_HEIGHT1_height_15_8_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_VS_POS0_UNION
 结构说明  : VS_POS0 寄存器结构定义。地址偏移量:0x2020，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pos_7_0  : 8;  /* bit[0-7] :  */
        unsigned int  reserved : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_VS_POS0_UNION;
#endif
#define SOC_EDC_VS_POS0_pos_7_0_START   (0)
#define SOC_EDC_VS_POS0_pos_7_0_END     (7)


/*****************************************************************************
 结构名    : SOC_EDC_VS_POS1_UNION
 结构说明  : VS_POS1 寄存器结构定义。地址偏移量:0x2024，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pos_15_8 : 8;  /* bit[0-7] :  */
        unsigned int  reserved : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_VS_POS1_UNION;
#endif
#define SOC_EDC_VS_POS1_pos_15_8_START  (0)
#define SOC_EDC_VS_POS1_pos_15_8_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_IRIDIX_CONTROL_0_UNION
 结构说明  : IRIDIX_CONTROL_0 寄存器结构定义。地址偏移量:0x2400，初值:0x00000013，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enable           : 1;  /* bit[0]   :  */
        unsigned int  local_contrast   : 2;  /* bit[1-2] :  */
        unsigned int  reserved_0       : 1;  /* bit[3]   :  */
        unsigned int  color_correction : 1;  /* bit[4]   :  */
        unsigned int  reserved_1       : 27; /* bit[5-31]:  */
    } reg;
} SOC_EDC_IRIDIX_CONTROL_0_UNION;
#endif
#define SOC_EDC_IRIDIX_CONTROL_0_enable_START            (0)
#define SOC_EDC_IRIDIX_CONTROL_0_enable_END              (0)
#define SOC_EDC_IRIDIX_CONTROL_0_local_contrast_START    (1)
#define SOC_EDC_IRIDIX_CONTROL_0_local_contrast_END      (2)
#define SOC_EDC_IRIDIX_CONTROL_0_color_correction_START  (4)
#define SOC_EDC_IRIDIX_CONTROL_0_color_correction_END    (4)


/*****************************************************************************
 结构名    : SOC_EDC_VARIANCE_UNION
 结构说明  : VARIANCE 寄存器结构定义。地址偏移量:0x240C，初值:0x00000022，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  variance_space     : 4;  /* bit[0-3] :  */
        unsigned int  variance_intensity : 4;  /* bit[4-7] :  */
        unsigned int  reserved           : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_VARIANCE_UNION;
#endif
#define SOC_EDC_VARIANCE_variance_space_START      (0)
#define SOC_EDC_VARIANCE_variance_space_END        (3)
#define SOC_EDC_VARIANCE_variance_intensity_START  (4)
#define SOC_EDC_VARIANCE_variance_intensity_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_SLOPE_MAX_UNION
 结构说明  : SLOPE_MAX 寄存器结构定义。地址偏移量:0x2410，初值:0x00000030，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  slope_max : 8;  /* bit[0-7] :  */
        unsigned int  reserved  : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_SLOPE_MAX_UNION;
#endif
#define SOC_EDC_SLOPE_MAX_slope_max_START  (0)
#define SOC_EDC_SLOPE_MAX_slope_max_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_SLOPE_MIN_UNION
 结构说明  : SLOPE_MIN 寄存器结构定义。地址偏移量:0x2414，初值:0x00000080，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  slope_min : 8;  /* bit[0-7] :  */
        unsigned int  reserved  : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_SLOPE_MIN_UNION;
#endif
#define SOC_EDC_SLOPE_MIN_slope_min_START  (0)
#define SOC_EDC_SLOPE_MIN_slope_min_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_BLACK_LEVEL0_UNION
 结构说明  : BLACK_LEVEL0 寄存器结构定义。地址偏移量:0x2418，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  black_level0 : 8;  /* bit[0-7] :  */
        unsigned int  reserved     : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_BLACK_LEVEL0_UNION;
#endif
#define SOC_EDC_BLACK_LEVEL0_black_level0_START  (0)
#define SOC_EDC_BLACK_LEVEL0_black_level0_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_BLACK_LEVEL1_UNION
 结构说明  : BLACK_LEVEL1 寄存器结构定义。地址偏移量:0x241C，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  black_level1 : 8;  /* bit[0-7] :  */
        unsigned int  reserved     : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_BLACK_LEVEL1_UNION;
#endif
#define SOC_EDC_BLACK_LEVEL1_black_level1_START  (0)
#define SOC_EDC_BLACK_LEVEL1_black_level1_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_WHITE_LEVEL0_UNION
 结构说明  : WHITE_LEVEL0 寄存器结构定义。地址偏移量:0x2420，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  white_level0 : 8;  /* bit[0-7] :  */
        unsigned int  reserved     : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_WHITE_LEVEL0_UNION;
#endif
#define SOC_EDC_WHITE_LEVEL0_white_level0_START  (0)
#define SOC_EDC_WHITE_LEVEL0_white_level0_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_WHITE_LEVEL1_UNION
 结构说明  : WHITE_LEVEL1 寄存器结构定义。地址偏移量:0x2424，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  white_level1 : 8;  /* bit[0-7] :  */
        unsigned int  reserved     : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_WHITE_LEVEL1_UNION;
#endif
#define SOC_EDC_WHITE_LEVEL1_white_level1_START  (0)
#define SOC_EDC_WHITE_LEVEL1_white_level1_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_LIMIT_AMP_UNION
 结构说明  : LIMIT_AMP 寄存器结构定义。地址偏移量:0x2428，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  drak_amp_light   : 4;  /* bit[0-3] :  */
        unsigned int  bright_amp_limit : 4;  /* bit[4-7] :  */
        unsigned int  reserved         : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_LIMIT_AMP_UNION;
#endif
#define SOC_EDC_LIMIT_AMP_drak_amp_light_START    (0)
#define SOC_EDC_LIMIT_AMP_drak_amp_light_END      (3)
#define SOC_EDC_LIMIT_AMP_bright_amp_limit_START  (4)
#define SOC_EDC_LIMIT_AMP_bright_amp_limit_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_DITHER_UNION
 结构说明  : DITHER 寄存器结构定义。地址偏移量:0x242C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dither_mode : 3;  /* bit[0-2] :  */
        unsigned int  reserved    : 29; /* bit[3-31]:  */
    } reg;
} SOC_EDC_DITHER_UNION;
#endif
#define SOC_EDC_DITHER_dither_mode_START  (0)
#define SOC_EDC_DITHER_dither_mode_END    (2)


/*****************************************************************************
 结构名    : SOC_EDC_LOGO_LEFT_UNION
 结构说明  : LOGO_LEFT 寄存器结构定义。地址偏移量:0x2800，初值:0x00000002，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  logo_left : 8;  /* bit[0-7] :  */
        unsigned int  reserved  : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_LOGO_LEFT_UNION;
#endif
#define SOC_EDC_LOGO_LEFT_logo_left_START  (0)
#define SOC_EDC_LOGO_LEFT_logo_left_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_LOGO_RIGHT_UNION
 结构说明  : LOGO_RIGHT 寄存器结构定义。地址偏移量:0x2804，初值:0x00000002，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  logo_right : 8;  /* bit[0-7] :  */
        unsigned int  reserved   : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_LOGO_RIGHT_UNION;
#endif
#define SOC_EDC_LOGO_RIGHT_logo_right_START  (0)
#define SOC_EDC_LOGO_RIGHT_logo_right_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_DITHER_CONTROL_UNION
 结构说明  : DITHER_CONTROL 寄存器结构定义。地址偏移量:0x2840，初值:0x00000003，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enable_dither : 1;  /* bit[0]   :  */
        unsigned int  dither_amount : 1;  /* bit[1]   :  */
        unsigned int  shift_mode    : 1;  /* bit[2]   :  */
        unsigned int  reserved_0    : 4;  /* bit[3-6] :  */
        unsigned int  dither_bypass : 1;  /* bit[7]   :  */
        unsigned int  reserved_1    : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_DITHER_CONTROL_UNION;
#endif
#define SOC_EDC_DITHER_CONTROL_enable_dither_START  (0)
#define SOC_EDC_DITHER_CONTROL_enable_dither_END    (0)
#define SOC_EDC_DITHER_CONTROL_dither_amount_START  (1)
#define SOC_EDC_DITHER_CONTROL_dither_amount_END    (1)
#define SOC_EDC_DITHER_CONTROL_shift_mode_START     (2)
#define SOC_EDC_DITHER_CONTROL_shift_mode_END       (2)
#define SOC_EDC_DITHER_CONTROL_dither_bypass_START  (7)
#define SOC_EDC_DITHER_CONTROL_dither_bypass_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_STRENGTH_SEL_UNION
 结构说明  : STRENGTH_SEL 寄存器结构定义。地址偏移量:0x2C00，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  strength_sel : 1;  /* bit[0]   :  */
        unsigned int  reserved     : 31; /* bit[1-31]:  */
    } reg;
} SOC_EDC_STRENGTH_SEL_UNION;
#endif
#define SOC_EDC_STRENGTH_SEL_strength_sel_START  (0)
#define SOC_EDC_STRENGTH_SEL_strength_sel_END    (0)


/*****************************************************************************
 结构名    : SOC_EDC_STRENGTH_MANUAL_UNION
 结构说明  : STRENGTH_MANUAL 寄存器结构定义。地址偏移量:0x2C04，初值:0x0000001F，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  strength_manual : 8;  /* bit[0-7] :  */
        unsigned int  reserved        : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_STRENGTH_MANUAL_UNION;
#endif
#define SOC_EDC_STRENGTH_MANUAL_strength_manual_START  (0)
#define SOC_EDC_STRENGTH_MANUAL_strength_manual_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_OPTION_SELECT_UNION
 结构说明  : OPTION_SELECT 寄存器结构定义。地址偏移量:0x2C08，初值:0x00000002，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  option_select : 2;  /* bit[0-1] :  */
        unsigned int  reserved      : 30; /* bit[2-31]:  */
    } reg;
} SOC_EDC_OPTION_SELECT_UNION;
#endif
#define SOC_EDC_OPTION_SELECT_option_select_START  (0)
#define SOC_EDC_OPTION_SELECT_option_select_END    (1)


/*****************************************************************************
 结构名    : SOC_EDC_AMBIENT_LIGHT0_UNION
 结构说明  : AMBIENT_LIGHT0 寄存器结构定义。地址偏移量:0x2C10，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ambient_light_7_0_ : 8;  /* bit[0-7] :  */
        unsigned int  reserved           : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_AMBIENT_LIGHT0_UNION;
#endif
#define SOC_EDC_AMBIENT_LIGHT0_ambient_light_7_0__START  (0)
#define SOC_EDC_AMBIENT_LIGHT0_ambient_light_7_0__END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_AMBIENT_LIGHT1_UNION
 结构说明  : AMBIENT_LIGHT1 寄存器结构定义。地址偏移量:0x2C14，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ambient_light_15_8_ : 8;  /* bit[0-7] :  */
        unsigned int  reserved            : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_AMBIENT_LIGHT1_UNION;
#endif
#define SOC_EDC_AMBIENT_LIGHT1_ambient_light_15_8__START  (0)
#define SOC_EDC_AMBIENT_LIGHT1_ambient_light_15_8__END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_BACKLIGHT_LEVEL0_UNION
 结构说明  : BACKLIGHT_LEVEL0 寄存器结构定义。地址偏移量:0x2C18，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  backlight_level_7_0_ : 8;  /* bit[0-7] :  */
        unsigned int  reserved             : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_BACKLIGHT_LEVEL0_UNION;
#endif
#define SOC_EDC_BACKLIGHT_LEVEL0_backlight_level_7_0__START  (0)
#define SOC_EDC_BACKLIGHT_LEVEL0_backlight_level_7_0__END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_BACKLIGHT_LEVEL1_UNION
 结构说明  : BACKLIGHT_LEVEL1 寄存器结构定义。地址偏移量:0x2C1C，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  backlight_level_15_8_ : 8;  /* bit[0-7] :  */
        unsigned int  reserved              : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_BACKLIGHT_LEVEL1_UNION;
#endif
#define SOC_EDC_BACKLIGHT_LEVEL1_backlight_level_15_8__START  (0)
#define SOC_EDC_BACKLIGHT_LEVEL1_backlight_level_15_8__END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_BACKLIGHT_MAX0_UNION
 结构说明  : BACKLIGHT_MAX0 寄存器结构定义。地址偏移量:0x2C20，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  backlight_max_7_0_ : 8;  /* bit[0-7] :  */
        unsigned int  reserved           : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_BACKLIGHT_MAX0_UNION;
#endif
#define SOC_EDC_BACKLIGHT_MAX0_backlight_max_7_0__START  (0)
#define SOC_EDC_BACKLIGHT_MAX0_backlight_max_7_0__END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_BACKLIGHT_MAX1_UNION
 结构说明  : BACKLIGHT_MAX1 寄存器结构定义。地址偏移量:0x2C24，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  backlight_max1_15_8_ : 8;  /* bit[0-7] :  */
        unsigned int  reserved             : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_BACKLIGHT_MAX1_UNION;
#endif
#define SOC_EDC_BACKLIGHT_MAX1_backlight_max1_15_8__START  (0)
#define SOC_EDC_BACKLIGHT_MAX1_backlight_max1_15_8__END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_STRENGTH_LIMIT_UNION
 结构说明  : STRENGTH_LIMIT 寄存器结构定义。地址偏移量:0x2C28，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  strength_limit : 8;  /* bit[0-7] :  */
        unsigned int  reserved       : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_STRENGTH_LIMIT_UNION;
#endif
#define SOC_EDC_STRENGTH_LIMIT_strength_limit_START  (0)
#define SOC_EDC_STRENGTH_LIMIT_strength_limit_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_CALIBRATION_A_UNION
 结构说明  : CALIBRATION_A 寄存器结构定义。地址偏移量:0x2C2C，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  calibration_a : 8;  /* bit[0-7] :  */
        unsigned int  reserved      : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_CALIBRATION_A_UNION;
#endif
#define SOC_EDC_CALIBRATION_A_calibration_a_START  (0)
#define SOC_EDC_CALIBRATION_A_calibration_a_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_CALIBRATION_B_UNION
 结构说明  : CALIBRATION_B 寄存器结构定义。地址偏移量:0x2C30，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  calibration_b : 8;  /* bit[0-7] :  */
        unsigned int  reserved      : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_CALIBRATION_B_UNION;
#endif
#define SOC_EDC_CALIBRATION_B_calibration_b_START  (0)
#define SOC_EDC_CALIBRATION_B_calibration_b_END    (7)


/*****************************************************************************
 结构名    : SOC_EDC_DRC_IN0_UNION
 结构说明  : DRC_IN0 寄存器结构定义。地址偏移量:0x2C38，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  drc_in0  : 8;  /* bit[0-7] :  */
        unsigned int  reserved : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_DRC_IN0_UNION;
#endif
#define SOC_EDC_DRC_IN0_drc_in0_START   (0)
#define SOC_EDC_DRC_IN0_drc_in0_END     (7)


/*****************************************************************************
 结构名    : SOC_EDC_DRC_IN1_UNION
 结构说明  : DRC_IN1 寄存器结构定义。地址偏移量:0x2C3C，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  drc_in1  : 8;  /* bit[0-7] :  */
        unsigned int  reserved : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_DRC_IN1_UNION;
#endif
#define SOC_EDC_DRC_IN1_drc_in1_START   (0)
#define SOC_EDC_DRC_IN1_drc_in1_END     (7)


/*****************************************************************************
 结构名    : SOC_EDC_T_FILT_CONTROL_UNION
 结构说明  : T_FILT_CONTROL 寄存器结构定义。地址偏移量:0x2C34，初值:0x000000FF，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  t_filt_control : 8;  /* bit[0-7] :  */
        unsigned int  reserved       : 24; /* bit[8-31]:  */
    } reg;
} SOC_EDC_T_FILT_CONTROL_UNION;
#endif
#define SOC_EDC_T_FILT_CONTROL_t_filt_control_START  (0)
#define SOC_EDC_T_FILT_CONTROL_t_filt_control_END    (7)






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

#endif /* end of soc_edc_interface.h */
