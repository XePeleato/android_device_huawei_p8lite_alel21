

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_SOCP_INTERFACE_H__
#define __SOC_SOCP_INTERFACE_H__

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
/* 寄存器说明：SOCP全局软复位控制寄存器
   位域定义UNION结构:  SOC_SOCP_GLOBAL_CTRL_UNION */
#define SOC_SOCP_GLOBAL_CTRL_ADDR(base)               ((base) + (0x0000))

/* 寄存器说明：编码通道软复位控制寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_SRST_CTRL_UNION */
#define SOC_SOCP_ENC_SRST_CTRL_ADDR(base)             ((base) + (0x0004))

/* 寄存器说明：解码通道软复位控制寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_SRST_CTRL_UNION */
#define SOC_SOCP_DEC_SRST_CTRL_ADDR(base)             ((base) + (0x0008))

/* 寄存器说明：编码通道状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CH_STATUS_UNION */
#define SOC_SOCP_ENC_CH_STATUS_ADDR(base)             ((base) + (0x000C))

/* 寄存器说明：解码通道状态寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CH_STATUS_UNION */
#define SOC_SOCP_DEC_CH_STATUS_ADDR(base)             ((base) + (0x0010))

/* 寄存器说明：自动时钟门控控制寄存器
   位域定义UNION结构:  SOC_SOCP_CLK_CTRL_UNION */
#define SOC_SOCP_CLK_CTRL_ADDR(base)                  ((base) + (0x0014))

/* 寄存器说明：优先级配置寄存器
   位域定义UNION结构:  SOC_SOCP_PRIOR_CFG_UNION */
#define SOC_SOCP_PRIOR_CFG_ADDR(base)                 ((base) + (0x0018))

#define SOC_SOCP_DEC_INT_TIMEOUT_ADDR(base)		((base) + (0x0020))

/* 寄存器说明：中断超时配置寄存器
   位域定义UNION结构:  SOC_SOCP_INT_TIMEOUT_UNION */
#define SOC_SOCP_INT_TIMEOUT_ADDR(base)               ((base) + (0x0024))

/* 寄存器说明：BUFFER溢出超时配置寄存器
   位域定义UNION结构:  SOC_SOCP_BUFFER_TIMEOUT_UNION */
#define SOC_SOCP_BUFFER_TIMEOUT_ADDR(base)            ((base) + (0x0028))

/* 寄存器说明：解码通路包长度配置寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_PKT_LEN_CFG_UNION */
#define SOC_SOCP_DEC_PKT_LEN_CFG_ADDR(base)           ((base) + (0x002C))

#define SOC_SOCP_ENC_SRCCH_SECCTRL_ADDR(base)		((base) + (0x0030))

#define SOC_SOCP_ENC_DSTCH_SECCTRL_ADDR(base)		((base) + (0x0034))

#define SOC_SOCP_DEC_SRCCH_SECCTRL_ADDR(base)		((base) + (0x0038))

#define SOC_SOCP_DEC_DSTCH_SECCTRL_ADDR(base)		((base) + (0x003C))

#define SOC_SOCP_GLB_REG_SECCTRL_ADDR(base)		((base) + (0x0040))

/* 寄存器说明：全局中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_GLOBAL_INT_STATUS_UNION */
#define SOC_SOCP_GLOBAL_INT_STATUS_ADDR(base)         ((base) + (0x0050))

/* 寄存器说明：CORE0编码传输中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_MASK0_UNION */
#define SOC_SOCP_ENC_CORE0_MASK0_ADDR(base)           ((base) + (0x0054))

/* 寄存器说明：CORE0编码传输中断原始状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_RAWINT0_UNION */
#define SOC_SOCP_ENC_CORE0_RAWINT0_ADDR(base)         ((base) + (0x0058))

/* 寄存器说明：CORE0编码传输中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_INT0_UNION */
#define SOC_SOCP_ENC_CORE0_INT0_ADDR(base)            ((base) + (0x005C))

/* 寄存器说明：CORE0编码头标志错中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_MASK1_UNION */
#define SOC_SOCP_ENC_CORE0_MASK1_ADDR(base)           ((base) + (0x0060))

/* 寄存器说明：CORE1编码头标志错中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE1_MASK1_UNION */
#define SOC_SOCP_ENC_CORE1_MASK1_ADDR(base)           ((base) + (0x0064))

/* 寄存器说明：编码通道头标志错中断原始状态寄存器1
   位域定义UNION结构:  SOC_SOCP_ENC_RAWINT1_UNION */
#define SOC_SOCP_ENC_RAWINT1_ADDR(base)               ((base) + (0x0068))

/* 寄存器说明：CORE0编码通道头标志错中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_INT1_UNION */
#define SOC_SOCP_ENC_CORE0_INT1_ADDR(base)            ((base) + (0x006C))

/* 寄存器说明：CORE1编码通道头标志错中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE1_INT1_UNION */
#define SOC_SOCP_ENC_CORE1_INT1_ADDR(base)            ((base) + (0x0070))

/* 寄存器说明：CORE0编码目的buffer上溢中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_MASK2_UNION */
#define SOC_SOCP_ENC_CORE0_MASK2_ADDR(base)           ((base) + (0x0074))

/* 寄存器说明：编码目的buffer上溢原始中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_RAWINT2_UNION */
#define SOC_SOCP_ENC_CORE0_RAWINT2_ADDR(base)         ((base) + (0x0078))

/* 寄存器说明：CORE0编码目的buffer上溢中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_INT2_UNION */
#define SOC_SOCP_ENC_CORE0_INT2_ADDR(base)            ((base) + (0x007C))

/* 寄存器说明：CORE0编码RD中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_MASK3_UNION */
#define SOC_SOCP_ENC_CORE0_MASK3_ADDR(base)           ((base) + (0x0080))

/* 寄存器说明：CORE1编码RD中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE1_MASK3_UNION */
#define SOC_SOCP_ENC_CORE1_MASK3_ADDR(base)           ((base) + (0x0084))

/* 寄存器说明：编码通道RD原始中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_RAWINT3_UNION */
#define SOC_SOCP_ENC_RAWINT3_ADDR(base)               ((base) + (0x0088))

/* 寄存器说明：CORE0编码RD中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE0_INT3_UNION */
#define SOC_SOCP_ENC_CORE0_INT3_ADDR(base)            ((base) + (0x008C))

/* 寄存器说明：CORE1编码RD中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE1_INT3_UNION */
#define SOC_SOCP_ENC_CORE1_INT3_ADDR(base)            ((base) + (0x0090))

/* 寄存器说明：CORE0解码通道传输中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE0_MASK0_UNION */
#define SOC_SOCP_DEC_CORE0_MASK0_ADDR(base)           ((base) + (0x00A8))

/* 寄存器说明：CORE1解码通道传输中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE1_MASK0_UNION */
#define SOC_SOCP_DEC_CORE1_MASK0_ADDR(base)           ((base) + (0x00AC))

/* 寄存器说明：解码通道中断原始状态寄存器0
   位域定义UNION结构:  SOC_SOCP_DEC_RAWINT0_UNION */
#define SOC_SOCP_DEC_RAWINT0_ADDR(base)               ((base) + (0x00B0))

/* 寄存器说明：CORE0解码通道传输中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE0_INT0_UNION */
#define SOC_SOCP_DEC_CORE0_INT0_ADDR(base)            ((base) + (0x00B4))

/* 寄存器说明：CORE1解码通道传输中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE1_INT0_UNION */
#define SOC_SOCP_DEC_CORE1_INT0_ADDR(base)            ((base) + (0x00B8))

/* 寄存器说明：CORE0解码通道异常中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE0_MASK1_UNION */
#define SOC_SOCP_DEC_CORE0_MASK1_ADDR(base)           ((base) + (0x00BC))

/* 寄存器说明：解码通道异常中断原始状态寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE0_RAWINT1_UNION */
#define SOC_SOCP_DEC_CORE0_RAWINT1_ADDR(base)         ((base) + (0x00C0))

/* 寄存器说明：CORE0解码通道异常中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE0_INT1_UNION */
#define SOC_SOCP_DEC_CORE0_INT1_ADDR(base)            ((base) + (0x00C4))

/* 寄存器说明：CORE0解码通道目的buffer溢出中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE0_MASK2_UNION */
#define SOC_SOCP_DEC_CORE0_MASK2_ADDR(base)           ((base) + (0x00C8))

/* 寄存器说明：CORE1解码通道目的buffer溢出中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE1Note_MASK2_UNION */
#define SOC_SOCP_DEC_CORE1Note_MASK2_ADDR(base)       ((base) + (0x00CC))

/* 寄存器说明：解码通道目的buffer溢出中断原始状态寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_RAWINT2_UNION */
#define SOC_SOCP_DEC_RAWINT2_ADDR(base)               ((base) + (0x00D0))

/* 寄存器说明：CORE0解码通道目的buffer溢出中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE0Note_NT2_UNION */
#define SOC_SOCP_DEC_CORE0Note_NT2_ADDR(base)         ((base) + (0x00D4))

/* 寄存器说明：CORE1解码通道目的buffer溢出中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE1Note_INT2_UNION */
#define SOC_SOCP_DEC_CORE1Note_INT2_ADDR(base)        ((base) + (0x00D8))

/* 寄存器说明：CORE1编码传输中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE1_MASK0_UNION */
#define SOC_SOCP_ENC_CORE1_MASK0_ADDR(base)           ((base) + (0x00DC))

/* 寄存器说明：CORE1编码传输中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE1_INT0_UNION */
#define SOC_SOCP_ENC_CORE1_INT0_ADDR(base)            ((base) + (0x00E0))

/* 寄存器说明：CORE1编码目的buffer上溢中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE1_MASK2_UNION */
#define SOC_SOCP_ENC_CORE1_MASK2_ADDR(base)           ((base) + (0x00E4))

/* 寄存器说明：CORE1编码目的buffer上溢中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_CORE1_INT2_UNION */
#define SOC_SOCP_ENC_CORE1_INT2_ADDR(base)            ((base) + (0x00E8))

/* 寄存器说明：CORE1解码通道异常中断屏蔽寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE1_MASK1_UNION */
#define SOC_SOCP_DEC_CORE1_MASK1_ADDR(base)           ((base) + (0x00EC))

/* 寄存器说明：CORE1解码通道异常中断状态寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_CORE1_INT1_UNION */
#define SOC_SOCP_DEC_CORE1_INT1_ADDR(base)            ((base) + (0x00F0))

#define SOC_SOCP_BUS_ERROR_MASK_ADDR(base)            ((base) + (0x00F4))

#define SOC_SOCP_BUS_ERROR_RAWINT_ADDR(base)            ((base) + (0x00F8))


/* 寄存器说明：编码通路源bufferm写指针寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_SRC_BUFm_WPTR_UNION */
#define SOC_SOCP_ENC_SRC_BUFm_WPTR_ADDR(base, m)      ((base) + (0x0100+0x40*(m)))

/* 寄存器说明：编码通路源bufferm读指针寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_SRC_BUFm_RPTR_UNION */
#define SOC_SOCP_ENC_SRC_BUFm_RPTR_ADDR(base, m)      ((base) + (0x0104+0x40*(m)))

/* 寄存器说明：编码通路源bufferm起始地址寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_SRC_BUFm_ADDR_UNION */
#define SOC_SOCP_ENC_SRC_BUFm_ADDR_ADDR(base, m)      ((base) + (0x0108+0x40*(m)))

/* 寄存器说明：编码通路源bufferm配置寄存器0
   位域定义UNION结构:  SOC_SOCP_ENC_SRC_BUFm_CFG0_UNION */
#define SOC_SOCP_ENC_SRC_BUFm_CFG0_ADDR(base, m)      ((base) + (0x010C+0x40*(m)))

/* 寄存器说明：编码通路源bufferm配置寄存器1
   位域定义UNION结构:  SOC_SOCP_ENC_SRC_BUFm_CFG1_UNION */
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_ADDR(base, m)      ((base) + (0x0110+0x40*(m)))

/* 寄存器说明：编码通路RD环形buffer写指针寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_SRC_RDQ_WPTR_UNION */
#define SOC_SOCP_ENC_SRC_RDQ_WPTR_ADDR(base, m)       ((base) + (0x0114+0x40*(m)))

/* 寄存器说明：编码通路RD环形buffer写指针寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_SRC_RDQ_RPTR_UNION */
#define SOC_SOCP_ENC_SRC_RDQ_RPTR_ADDR(base, m)       ((base) + (0x0118+0x40*(m)))

/* 寄存器说明：编码通路RD环形buffer起始地址寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_SRC_RDQ_BADDR_UNION */
#define SOC_SOCP_ENC_SRC_RDQ_BADDR_ADDR(base, m)      ((base) + (0x011C+0x40*(m)))

/* 寄存器说明：编码通路RD环形buffer配置寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_SRC_RDQ_CFG_UNION */
#define SOC_SOCP_ENC_SRC_RDQ_CFG_ADDR(base, m)        ((base) + (0x0120+0x40*(m)))

/* 寄存器说明：编码通路目的buffer写指针寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_DEST_BUFn_WPTR_UNION */
#define SOC_SOCP_ENC_DEST_BUFn_WPTR_ADDR(base, n)     ((base) + (0x0900+0x20*(n)))

/* 寄存器说明：编码通路目的buffer读指针寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_DEST_BUFn_RPTR_UNION */
#define SOC_SOCP_ENC_DEST_BUFn_RPTR_ADDR(base, n)     ((base) + (0x0904+0x20*(n)))

/* 寄存器说明：编码通路目的buffer起始地址寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_DEST_BUFn_ADDR_UNION */
#define SOC_SOCP_ENC_DEST_BUFn_ADDR_ADDR(base, n)     ((base) + (0x0908+0x20*(n)))

/* 寄存器说明：编码通路目的buffer配置寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_DEST_BUFn_CFG_UNION */
#define SOC_SOCP_ENC_DEST_BUFn_CFG_ADDR(base, n)      ((base) + (0x090C+0x20*(n)))

/* 寄存器说明：编码通路目的buffer仲裁阈值寄存器
   位域定义UNION结构:  SOC_SOCP_ENC_DEST_BUFn_THRH_UNION */
#define SOC_SOCP_ENC_DEST_BUFn_THRH_ADDR(base, n)     ((base) + (0x0910+0x20*(n)))

/* 寄存器说明：解码通路源bufferx写指针寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_SRC_BUFx_WPTR_UNION */
#define SOC_SOCP_DEC_SRC_BUFx_WPTR_ADDR(base, x)      ((base) + (0X0A00+0X40*(x)))

/* 寄存器说明：解码通路源bufferx读指针寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_SRC_BUFx_RPTR_UNION */
#define SOC_SOCP_DEC_SRC_BUFx_RPTR_ADDR(base, x)      ((base) + (0X0A04+0X40*(x)))

/* 寄存器说明：解码通路源bufferx起始地址寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_SRC_BUFx_ADDR_UNION */
#define SOC_SOCP_DEC_SRC_BUFx_ADDR_ADDR(base, x)      ((base) + (0X0A08+0X40*(x)))

/* 寄存器说明：解码通路源bufferx配置寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_SRC_BUFx_CFG0_UNION */
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_ADDR(base, x)      ((base) + (0X0A0C+0X40*(x)))

/* 寄存器说明：解码通路状态寄存器0
   位域定义UNION结构:  SOC_SOCP_DEC_BUFx_STATUS0_UNION */
#define SOC_SOCP_DEC_BUFx_STATUS0_ADDR(base, x)       ((base) + (0X0A20+0X40*(x)))

/* 寄存器说明：解码通路状态寄存器1
   位域定义UNION结构:  SOC_SOCP_DEC_BUFx_STATUS1_UNION */
#define SOC_SOCP_DEC_BUFx_STATUS1_ADDR(base, x)       ((base) + (0X0A24+0X40*(x)))

/* 寄存器说明：解码通路目的buffery写指针寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_DEST_BUFy_WPTR_UNION */
#define SOC_SOCP_DEC_DEST_BUFy_WPTR_ADDR(base, y)        ((base) + (0x0C00+0x10*y))

/* 寄存器说明：解码通路目的buffery读指针寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_DEST_BUFy_RPTR_UNION */
#define SOC_SOCP_DEC_DEST_BUFy_RPTR_ADDR(base, y)        ((base) + (0x0C04+0x10*y))

/* 寄存器说明：解码通路目的buffery起始地址寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_DEST_BUFy_ADDR_UNION */
#define SOC_SOCP_DEC_DEST_BUFy_ADDR_ADDR(base, y)        ((base) + (0x0C08+0x10*y))

/* 寄存器说明：解码通路目的buffery配置寄存器
   位域定义UNION结构:  SOC_SOCP_DEC_DEST_BUFy_CFG0_UNION */
#define SOC_SOCP_DEC_DEST_BUFy_CFG0_ADDR(base, y)        ((base) + (0x0C0C+0x10*y))

/* 寄存器说明：编码CD debug寄存器0
   位域定义UNION结构:  SOC_SOCP_ENC_CD_DBG0_UNION */
#define SOC_SOCP_ENC_CD_DBG0_ADDR(base)               ((base) + (0x0E00))

/* 寄存器说明：编码CD debug寄存器1
   位域定义UNION结构:  SOC_SOCP_ENC_CD_DBG1_UNION */
#define SOC_SOCP_ENC_CD_DBG1_ADDR(base)               ((base) + (0x0E04))

/* 寄存器说明：编码源端debug寄存器0
   位域定义UNION结构:  SOC_SOCP_ENC_IBUF_DBG0_UNION */
#define SOC_SOCP_ENC_IBUF_DBG0_ADDR(base)             ((base) + (0x0E08))

/* 寄存器说明：编码源端debug寄存器1
   位域定义UNION结构:  SOC_SOCP_ENC_IBUF_DBG1_UNION */
#define SOC_SOCP_ENC_IBUF_DBG1_ADDR(base)             ((base) + (0x0E0C))

/* 寄存器说明：编码源端debug寄存器2
   位域定义UNION结构:  SOC_SOCP_ENC_IBUF_DBG2_UNION */
#define SOC_SOCP_ENC_IBUF_DBG2_ADDR(base)             ((base) + (0x0E10))

/* 寄存器说明：编码源端debug寄存器3
   位域定义UNION结构:  SOC_SOCP_ENC_IBUF_DBG3_UNION */
#define SOC_SOCP_ENC_IBUF_DBG3_ADDR(base)             ((base) + (0x0E14))

/* 寄存器说明：编码目的端debug寄存器0
   位域定义UNION结构:  SOC_SOCP_ENC_OBUF_DBG0_UNION */
#define SOC_SOCP_ENC_OBUF_DBG0_ADDR(base)             ((base) + (0x0E18))

/* 寄存器说明：编码目的端debug寄存器1
   位域定义UNION结构:  SOC_SOCP_ENC_OBUF_DBG1_UNION */
#define SOC_SOCP_ENC_OBUF_DBG1_ADDR(base)             ((base) + (0x0E1C))

/* 寄存器说明：编码目的端debug寄存器2
   位域定义UNION结构:  SOC_SOCP_ENC_OBUF_DBG2_UNION */
#define SOC_SOCP_ENC_OBUF_DBG2_ADDR(base)             ((base) + (0x0E20))

/* 寄存器说明：编码目的端debug寄存器3
   位域定义UNION结构:  SOC_SOCP_ENC_OBUF_DBG3_UNION */
#define SOC_SOCP_ENC_OBUF_DBG3_ADDR(base)             ((base) + (0x0E24))

/* 寄存器说明：解码源端debug寄存器0
   位域定义UNION结构:  SOC_SOCP_DEC_IBUF_DBG0_UNION */
#define SOC_SOCP_DEC_IBUF_DBG0_ADDR(base)             ((base) + (0x0E28))

/* 寄存器说明：解码源端debug寄存器1
   位域定义UNION结构:  SOC_SOCP_DEC_IBUF_DBG1_UNION */
#define SOC_SOCP_DEC_IBUF_DBG1_ADDR(base)             ((base) + (0x0E2C))

/* 寄存器说明：解码源端debug寄存器2
   位域定义UNION结构:  SOC_SOCP_DEC_IBUF_DBG2_UNION */
#define SOC_SOCP_DEC_IBUF_DBG2_ADDR(base)             ((base) + (0x0E30))

/* 寄存器说明：解码源端debug寄存器3
   位域定义UNION结构:  SOC_SOCP_DEC_IBUF_DBG3_UNION */
#define SOC_SOCP_DEC_IBUF_DBG3_ADDR(base)             ((base) + (0x0E34))

/* 寄存器说明：解码目的端debug寄存器0
   位域定义UNION结构:  SOC_SOCP_DEC_OBUF_DBG0_UNION */
#define SOC_SOCP_DEC_OBUF_DBG0_ADDR(base)             ((base) + (0x0E38))

/* 寄存器说明：解码目的端debug寄存器1
   位域定义UNION结构:  SOC_SOCP_DEC_OBUF_DBG1_UNION */
#define SOC_SOCP_DEC_OBUF_DBG1_ADDR(base)             ((base) + (0x0E3C))

/* 寄存器说明：解码目的端debug寄存器2
   位域定义UNION结构:  SOC_SOCP_DEC_OBUF_DBG2_UNION */
#define SOC_SOCP_DEC_OBUF_DBG2_ADDR(base)             ((base) + (0x0E40))

/* 寄存器说明：解码目的端debug寄存器3
   位域定义UNION结构:  SOC_SOCP_DEC_OBUF_DBG3_UNION */
#define SOC_SOCP_DEC_OBUF_DBG3_ADDR(base)             ((base) + (0x0E44))





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
 结构名    : SOC_SOCP_GLOBAL_CTRL_UNION
 结构说明  : GLOBAL_CTRL 寄存器结构定义。地址偏移量:0x0000，初值:0x00000000，宽度:32
 寄存器说明: SOCP全局软复位控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  global_srst : 1;  /* bit[0]    : SOCP模块全局软复位控制，自动清零。
                                                       1：请求复位
                                                       0：不请求复位 */
        unsigned int  reserved_0  : 15; /* bit[1-15] : 保留 */
        unsigned int  ch_halt     : 1;  /* bit[16]   : SOCP编解码通道停止控制位。
                                                       1 - 编解码通道停止工作
                                                       0 - 编解码通道正常工作
                                                       该bit主要用于低功耗模式。 */
        unsigned int  reserved_1  : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_SOCP_GLOBAL_CTRL_UNION;
#endif
#define SOC_SOCP_GLOBAL_CTRL_global_srst_START  (0)
#define SOC_SOCP_GLOBAL_CTRL_global_srst_END    (0)
#define SOC_SOCP_GLOBAL_CTRL_ch_halt_START      (16)
#define SOC_SOCP_GLOBAL_CTRL_ch_halt_END        (16)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRST_CTRL_UNION
 结构说明  : ENC_SRST_CTRL 寄存器结构定义。地址偏移量:0x0004，初值:0x00000000，宽度:32
 寄存器说明: 编码通道软复位控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_chm_srst : 32; /* bit[0-31]: 编码通路通道软复位控制，自动清零。
                                                       1：请求复位
                                                       0：不请求复位 */
    } reg;
} SOC_SOCP_ENC_SRST_CTRL_UNION;
#endif
#define SOC_SOCP_ENC_SRST_CTRL_enc_chm_srst_START  (0)
#define SOC_SOCP_ENC_SRST_CTRL_enc_chm_srst_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_SRST_CTRL_UNION
 结构说明  : DEC_SRST_CTRL 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: 解码通道软复位控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_chx_srst : 4;  /* bit[0-3] : 解码通路通道软复位控制，自动清零。
                                                       1：请求复位
                                                       0：不请求复位 */
        unsigned int  reserved     : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_SRST_CTRL_UNION;
#endif
#define SOC_SOCP_DEC_SRST_CTRL_dec_chx_srst_START  (0)
#define SOC_SOCP_DEC_SRST_CTRL_dec_chx_srst_END    (3)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CH_STATUS_UNION
 结构说明  : ENC_CH_STATUS 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: 编码通道状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_chm_busy : 32; /* bit[0-31]: 编码通路通道忙指示。
                                                       1：忙
                                                       0：空闲 */
    } reg;
} SOC_SOCP_ENC_CH_STATUS_UNION;
#endif
#define SOC_SOCP_ENC_CH_STATUS_enc_chm_busy_START  (0)
#define SOC_SOCP_ENC_CH_STATUS_enc_chm_busy_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CH_STATUS_UNION
 结构说明  : DEC_CH_STATUS 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: 解码通道状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_chx_busy : 4;  /* bit[0-3] : 解码通路通道忙指示。
                                                       1：忙
                                                       0：空闲 */
        unsigned int  reserved     : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CH_STATUS_UNION;
#endif
#define SOC_SOCP_DEC_CH_STATUS_dec_chx_busy_START  (0)
#define SOC_SOCP_DEC_CH_STATUS_dec_chx_busy_END    (3)


/*****************************************************************************
 结构名    : SOC_SOCP_CLK_CTRL_UNION
 结构说明  : CLK_CTRL 寄存器结构定义。地址偏移量:0x0014，初值:0x00000000，宽度:32
 寄存器说明: 自动时钟门控控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_gclk_en : 1;  /* bit[0]   : 编码通道自动门控控制。
                                                      1：空闲时，时钟自动关闭
                                                      0：时钟一直打开 */
        unsigned int  dec_gclk_en : 1;  /* bit[1]   : 解码通道自动门控控制。
                                                      1：空闲时，时钟自动关闭
                                                      0：时钟一直打开 */
        unsigned int  reg_clk_en  : 1;  /* bit[2]   : 寄存器自动门控控制。
                                                      1：空闲时，时钟自动关闭
                                                      0：时钟一直打开 */
        unsigned int  reserved_0  : 1;  /* bit[3]   : 保留。 */
        unsigned int  ram_gclk_en : 1;  /* bit[4]   : RAM自动门控控制。
                                                      1：空闲时，时钟自动关闭
                                                      0：时钟一直打开 */
        unsigned int  reserved_1  : 27; /* bit[5-31]: 保留。 */
    } reg;
} SOC_SOCP_CLK_CTRL_UNION;
#endif
#define SOC_SOCP_CLK_CTRL_enc_gclk_en_START  (0)
#define SOC_SOCP_CLK_CTRL_enc_gclk_en_END    (0)
#define SOC_SOCP_CLK_CTRL_dec_gclk_en_START  (1)
#define SOC_SOCP_CLK_CTRL_dec_gclk_en_END    (1)
#define SOC_SOCP_CLK_CTRL_reg_clk_en_START   (2)
#define SOC_SOCP_CLK_CTRL_reg_clk_en_END     (2)
#define SOC_SOCP_CLK_CTRL_ram_gclk_en_START  (4)
#define SOC_SOCP_CLK_CTRL_ram_gclk_en_END    (4)


/*****************************************************************************
 结构名    : SOC_SOCP_PRIOR_CFG_UNION
 结构说明  : PRIOR_CFG 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: 优先级配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_prior_cfg : 1;  /* bit[0]   : 编码通道优先级配置
                                                        1：高优先级
                                                        0：低优先级 */
        unsigned int  dec_prior_cfg : 1;  /* bit[1]   : 解码通道优先级配置（与编码通道配置相同时优先级高于编码通道）
                                                        1：高优先级
                                                        0：低优先级 */
        unsigned int  reserved      : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_SOCP_PRIOR_CFG_UNION;
#endif
#define SOC_SOCP_PRIOR_CFG_enc_prior_cfg_START  (0)
#define SOC_SOCP_PRIOR_CFG_enc_prior_cfg_END    (0)
#define SOC_SOCP_PRIOR_CFG_dec_prior_cfg_START  (1)
#define SOC_SOCP_PRIOR_CFG_dec_prior_cfg_END    (1)


/*****************************************************************************
 结构名    : SOC_SOCP_INT_TIMEOUT_UNION
 结构说明  : INT_TIMEOUT 寄存器结构定义。地址偏移量:0x0024，初值:0x00000000，宽度:32
 寄存器说明: 中断超时配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_timeout0_cfg : 8;  /* bit[0-7] : 超时传输中断超时配置。
                                                           达到超时计数，上报超时中断。
                                                           配置全0时，表示无超时传输中断。
                                                           （计数单位为0xFFFF个时钟周期，在150Mhz频率下约为436.9us） */
        unsigned int  reserved         : 24; /* bit[8-31]: 保留 */
    } reg;
} SOC_SOCP_INT_TIMEOUT_UNION;
#endif
#define SOC_SOCP_INT_TIMEOUT_int_timeout0_cfg_START  (0)
#define SOC_SOCP_INT_TIMEOUT_int_timeout0_cfg_END    (7)


/*****************************************************************************
 结构名    : SOC_SOCP_BUFFER_TIMEOUT_UNION
 结构说明  : BUFFER_TIMEOUT 寄存器结构定义。地址偏移量:0x0028，初值:0x00000000，宽度:32
 寄存器说明: BUFFER溢出超时配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  buffer_timeout_cfg : 16; /* bit[0-15] : 源目的buffer溢出超时配置。
                                                              Buffer溢出，并且到达超时计数，停止处理，上报中断。（计数单位为0xFFFF个时钟周期，在150Mhz频率下约为436.9us） */
        unsigned int  reserved           : 15; /* bit[16-30]: 保留 */
        unsigned int  buffer_timeout_en  : 1;  /* bit[31]   : 源目的buffer溢出超时中断丢包使能。
                                                              1：源/目的buffer溢出，达到超时计数后，丢弃当前包
                                                              0：源/目的buffer溢出，达到超时计数后，等待 */
    } reg;
} SOC_SOCP_BUFFER_TIMEOUT_UNION;
#endif
#define SOC_SOCP_BUFFER_TIMEOUT_buffer_timeout_cfg_START  (0)
#define SOC_SOCP_BUFFER_TIMEOUT_buffer_timeout_cfg_END    (15)
#define SOC_SOCP_BUFFER_TIMEOUT_buffer_timeout_en_START   (31)
#define SOC_SOCP_BUFFER_TIMEOUT_buffer_timeout_en_END     (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_PKT_LEN_CFG_UNION
 结构说明  : DEC_PKT_LEN_CFG 寄存器结构定义。地址偏移量:0x002C，初值:0x00000800，宽度:32
 寄存器说明: 解码通路包长度配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_max_pkt_len : 12; /* bit[0-11] : 解码后最大数据包大小，以KB为单位。
                                                           默认为2KB。 */
        unsigned int  dec_min_pkt_len : 5;  /* bit[12-16]: 解码后最小数据包大小，字节为单位。 */
        unsigned int  reserved        : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_PKT_LEN_CFG_UNION;
#endif
#define SOC_SOCP_DEC_PKT_LEN_CFG_dec_max_pkt_len_START  (0)
#define SOC_SOCP_DEC_PKT_LEN_CFG_dec_max_pkt_len_END    (11)
#define SOC_SOCP_DEC_PKT_LEN_CFG_dec_min_pkt_len_START  (12)
#define SOC_SOCP_DEC_PKT_LEN_CFG_dec_min_pkt_len_END    (16)


/*****************************************************************************
 结构名    : SOC_SOCP_GLOBAL_INT_STATUS_UNION
 结构说明  : GLOBAL_INT_STATUS 寄存器结构定义。地址偏移量:0x0050，初值:0x00000000，宽度:32
 寄存器说明: 全局中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_dec_obuf_ovf_int : 1;  /* bit[0]    : core0解码通道目的buffer溢出全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core0_dec_err_int      : 1;  /* bit[1]    : core0解码通道异常全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core0_dec_tfr_int      : 1;  /* bit[2]    : core0解码通道传输全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core1_dec_tfr_int      : 1;  /* bit[3]    : core1解码通道传输全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core1_dec_obuf_ovf_int : 1;  /* bit[4]    : core1解码通道目的buffer溢出全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core1_dec_err_int      : 1;  /* bit[5]    : core1解码通道异常全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  reserved_0             : 4;  /* bit[6-9]  : 保留。 */
        unsigned int  core1_enc_rdq_int      : 1;  /* bit[10]   : 供MODEM-CPU查询编码通道Rd完成全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core0_enc_rdq_int      : 1;  /* bit[11]   : 供APP-CPU查询编码通道Rd完成全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core0_enc_obuf_ovf_int : 1;  /* bit[12]   : core0编码通道目的buffer溢出全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core1_enc_flag_err_int : 1;  /* bit[13]   : 供MODEM-CPU查询包头错误全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core0_enc_flag_err_int : 1;  /* bit[14]   : 供APP-CPU查询包头错误全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core0_enc_tfr_int      : 1;  /* bit[15]   : core0编码通道传输全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core1_enc_obuf_ovf_int : 1;  /* bit[16]   : core1编码通道目的buffer溢出全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  core1_enc_tfr_int      : 1;  /* bit[17]   : core1编码通道传输全局中断状态
                                                                  1：有中断
                                                                  0：无中断 */
        unsigned int  reserved_1             : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_SOCP_GLOBAL_INT_STATUS_UNION;
#endif
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_dec_obuf_ovf_int_START  (0)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_dec_obuf_ovf_int_END    (0)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_dec_err_int_START       (1)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_dec_err_int_END         (1)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_dec_tfr_int_START       (2)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_dec_tfr_int_END         (2)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_dec_tfr_int_START       (3)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_dec_tfr_int_END         (3)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_dec_obuf_ovf_int_START  (4)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_dec_obuf_ovf_int_END    (4)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_dec_err_int_START       (5)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_dec_err_int_END         (5)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_enc_rdq_int_START       (10)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_enc_rdq_int_END         (10)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_enc_rdq_int_START       (11)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_enc_rdq_int_END         (11)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_enc_obuf_ovf_int_START  (12)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_enc_obuf_ovf_int_END    (12)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_enc_flag_err_int_START  (13)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_enc_flag_err_int_END    (13)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_enc_flag_err_int_START  (14)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_enc_flag_err_int_END    (14)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_enc_tfr_int_START       (15)
#define SOC_SOCP_GLOBAL_INT_STATUS_core0_enc_tfr_int_END         (15)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_enc_obuf_ovf_int_START  (16)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_enc_obuf_ovf_int_END    (16)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_enc_tfr_int_START       (17)
#define SOC_SOCP_GLOBAL_INT_STATUS_core1_enc_tfr_int_END         (17)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_MASK0_UNION
 结构说明  : ENC_CORE0_MASK0 寄存器结构定义。地址偏移量:0x0054，初值:0x0000007F，宽度:32
 寄存器说明: CORE0编码传输中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_enc_tfr_mask : 7;  /* bit[0-6] : core0编码通路传输中断屏蔽控制位。每个bit对应一个目的buffer，Bit0对应目的buffer0。 
                                                             1：屏蔽中断
                                                             0：不屏蔽中断 */
        unsigned int  reserved           : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE0_MASK0_UNION;
#endif
#define SOC_SOCP_ENC_CORE0_MASK0_core0_enc_tfr_mask_START  (0)
#define SOC_SOCP_ENC_CORE0_MASK0_core0_enc_tfr_mask_END    (6)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_RAWINT0_UNION
 结构说明  : ENC_CORE0_RAWINT0 寄存器结构定义。地址偏移量:0x0058，初值:0x00000000，宽度:32
 寄存器说明: CORE0编码传输中断原始状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_tfr_rawint : 7;  /* bit[0-6] : 编码通路原始传输中断状态,写1清。每个bit对应一个目的buffer，Bit0对应目的buffer0。
                                                         1：有中断
                                                         0：无中断 */
        unsigned int  reserved       : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE0_RAWINT0_UNION;
#endif
#define SOC_SOCP_ENC_CORE0_RAWINT0_enc_tfr_rawint_START  (0)
#define SOC_SOCP_ENC_CORE0_RAWINT0_enc_tfr_rawint_END    (6)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_INT0_UNION
 结构说明  : ENC_CORE0_INT0 寄存器结构定义。地址偏移量:0x005C，初值:0x00000000，宽度:32
 寄存器说明: CORE0编码传输中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_enc_tfr_maskint : 7;  /* bit[0-6] : core0编码通路屏蔽后传输中断状态。每个bit对应一个目的buffer，Bit0对应目的buffer0。
                                                                1：有中断
                                                                0：无中断 */
        unsigned int  reserved              : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE0_INT0_UNION;
#endif
#define SOC_SOCP_ENC_CORE0_INT0_core0_enc_tfr_maskint_START  (0)
#define SOC_SOCP_ENC_CORE0_INT0_core0_enc_tfr_maskint_END    (6)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_MASK1_UNION
 结构说明  : ENC_CORE0_MASK1 寄存器结构定义。地址偏移量:0x0060，初值:0xFFFFFFFF，宽度:32
 寄存器说明: CORE0编码头标志错中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 编码通路源数据包头出错中断屏蔽控制位（APP-cpu配置）每个bit对应一个逻辑通道，Bit0对应逻辑通道0
                                                   1：屏蔽中断
                                                   0：不屏蔽中断 */
    } reg;
} SOC_SOCP_ENC_CORE0_MASK1_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE1_MASK1_UNION
 结构说明  : ENC_CORE1_MASK1 寄存器结构定义。地址偏移量:0x0064，初值:0xFFFFFFFF，宽度:32
 寄存器说明: CORE1编码头标志错中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 编码通路源数据包头出错中断屏蔽控制位（MODEM-cpu配置）每个bit对应一个逻辑通道，Bit0对应逻辑通道0
                                                   1：屏蔽中断
                                                   0：不屏蔽中断 */
    } reg;
} SOC_SOCP_ENC_CORE1_MASK1_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_RAWINT1_UNION
 结构说明  : ENC_RAWINT1 寄存器结构定义。地址偏移量:0x0068，初值:0x00000000，宽度:32
 寄存器说明: 编码通道头标志错中断原始状态寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_flag_err_rawint : 32; /* bit[0-31]: 编码通路源数据包头错原始中断状态位,写1清，每个bit对应一个逻辑通道
                                                              1：有中断
                                                              0：无中断 */
    } reg;
} SOC_SOCP_ENC_RAWINT1_UNION;
#endif
#define SOC_SOCP_ENC_RAWINT1_enc_flag_err_rawint_START  (0)
#define SOC_SOCP_ENC_RAWINT1_enc_flag_err_rawint_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_INT1_UNION
 结构说明  : ENC_CORE0_INT1 寄存器结构定义。地址偏移量:0x006C，初值:0x00000000，宽度:32
 寄存器说明: CORE0编码通道头标志错中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_flag_err_app_maskint : 32; /* bit[0-31]: 编码通路源数据包头错屏蔽中断状态位(供APP-cpu查询)每个bit对应一个逻辑通道
                                                                   1：有中断
                                                                   0：无中断 */
    } reg;
} SOC_SOCP_ENC_CORE0_INT1_UNION;
#endif
#define SOC_SOCP_ENC_CORE0_INT1_enc_flag_err_app_maskint_START  (0)
#define SOC_SOCP_ENC_CORE0_INT1_enc_flag_err_app_maskint_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE1_INT1_UNION
 结构说明  : ENC_CORE1_INT1 寄存器结构定义。地址偏移量:0x0070，初值:0x00000000，宽度:32
 寄存器说明: CORE1编码通道头标志错中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_flag_err_modem_maskint : 32; /* bit[0-31]: 编码通路源数据包头错屏蔽中断状态位(供MODEM-cpu查询)每个bit对应一个逻辑通道
                                                                     1：有中断
                                                                     0：无中断 */
    } reg;
} SOC_SOCP_ENC_CORE1_INT1_UNION;
#endif
#define SOC_SOCP_ENC_CORE1_INT1_enc_flag_err_modem_maskint_START  (0)
#define SOC_SOCP_ENC_CORE1_INT1_enc_flag_err_modem_maskint_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_MASK2_UNION
 结构说明  : ENC_CORE0_MASK2 寄存器结构定义。地址偏移量:0x0074，初值:0x007F007F，宽度:32
 寄存器说明: CORE0编码目的buffer上溢中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_enc_obuf_ovf_mask      : 7;  /* bit[0-6]  : core0编码目的buffer剩余空间不够一次dma传输时上溢中断屏蔽控制，每个bit对应一个目的buffer
                                                                        1：屏蔽中断
                                                                        0：不屏蔽中断 */
        unsigned int  reserved_0                   : 9;  /* bit[7-15] : 保留。 */
        unsigned int  core0_enc_obuf_thrh_ovf_mask : 7;  /* bit[16-22]: core0当目的buffer剩余空间小于配置的阈值时上溢中断屏蔽控制，每个bit对应一个目的buffer
                                                                        1：屏蔽中断
                                                                        0：不屏蔽中断 */
        unsigned int  reserved_1                   : 9;  /* bit[23-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE0_MASK2_UNION;
#endif
#define SOC_SOCP_ENC_CORE0_MASK2_core0_enc_obuf_ovf_mask_START       (0)
#define SOC_SOCP_ENC_CORE0_MASK2_core0_enc_obuf_ovf_mask_END         (6)
#define SOC_SOCP_ENC_CORE0_MASK2_core0_enc_obuf_thrh_ovf_mask_START  (16)
#define SOC_SOCP_ENC_CORE0_MASK2_core0_enc_obuf_thrh_ovf_mask_END    (22)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_RAWINT2_UNION
 结构说明  : ENC_CORE0_RAWINT2 寄存器结构定义。地址偏移量:0x0078，初值:0x00000000，宽度:32
 寄存器说明: 编码目的buffer上溢原始中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_obuf_ovf_rawint      : 7;  /* bit[0-6]  : 编码目的buffer剩余空间不够一次dma传输时上溢原始中断状态,写1清，每个bit对应一个目的buffer
                                                                    1：有中断
                                                                    0：无中断 */
        unsigned int  reserved_0               : 9;  /* bit[7-15] : 保留。 */
        unsigned int  enc_obuf_thrh_ovf_rawint : 7;  /* bit[16-22]: 当目的buffer剩余空间小于配置的阈值时上溢原始中断状态，写1清，每个bit对应一个目的buffer
                                                                    1：有中断
                                                                    0：无中断 */
        unsigned int  reserved_1               : 9;  /* bit[23-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE0_RAWINT2_UNION;
#endif
#define SOC_SOCP_ENC_CORE0_RAWINT2_enc_obuf_ovf_rawint_START       (0)
#define SOC_SOCP_ENC_CORE0_RAWINT2_enc_obuf_ovf_rawint_END         (6)
#define SOC_SOCP_ENC_CORE0_RAWINT2_enc_obuf_thrh_ovf_rawint_START  (16)
#define SOC_SOCP_ENC_CORE0_RAWINT2_enc_obuf_thrh_ovf_rawint_END    (22)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_INT2_UNION
 结构说明  : ENC_CORE0_INT2 寄存器结构定义。地址偏移量:0x007C，初值:0x00000000，宽度:32
 寄存器说明: CORE0编码目的buffer上溢中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_enc_obuf_ovf_maskint      : 7;  /* bit[0-6]  : core0编码目的buffer剩余空间不够一次dma传输时上溢屏蔽后中断状态，每个bit对应一个目的buffer
                                                                           1：有中断
                                                                           0：无中断 */
        unsigned int  reserved_0                      : 9;  /* bit[7-15] : 保留。 */
        unsigned int  core0_enc_obuf_thrh_ovf_maskint : 7;  /* bit[16-22]: core0编码目的buffer剩余空间小于配置的阈值时上溢屏蔽后中断状态，每个bit对应一个目的buffer
                                                                           1：有中断
                                                                           0：无中断 */
        unsigned int  reserved_1                      : 9;  /* bit[23-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE0_INT2_UNION;
#endif
#define SOC_SOCP_ENC_CORE0_INT2_core0_enc_obuf_ovf_maskint_START       (0)
#define SOC_SOCP_ENC_CORE0_INT2_core0_enc_obuf_ovf_maskint_END         (6)
#define SOC_SOCP_ENC_CORE0_INT2_core0_enc_obuf_thrh_ovf_maskint_START  (16)
#define SOC_SOCP_ENC_CORE0_INT2_core0_enc_obuf_thrh_ovf_maskint_END    (22)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_MASK3_UNION
 结构说明  : ENC_CORE0_MASK3 寄存器结构定义。地址偏移量:0x0080，初值:0xFFFFFFFF，宽度:32
 寄存器说明: CORE0编码RD中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_enc_rdq_done_mask : 32; /* bit[0-31]: CORE0编码通路RD环形完成中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                  1：屏蔽中断
                                                                  0：不屏蔽中断 */
    } reg;
} SOC_SOCP_ENC_CORE0_MASK3_UNION;
#endif
#define SOC_SOCP_ENC_CORE0_MASK3_core0_enc_rdq_done_mask_START  (0)
#define SOC_SOCP_ENC_CORE0_MASK3_core0_enc_rdq_done_mask_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE1_MASK3_UNION
 结构说明  : ENC_CORE1_MASK3 寄存器结构定义。地址偏移量:0x0084，初值:0xFFFFFFFF，宽度:32
 寄存器说明: CORE1编码RD中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_enc_rdq_done_mask : 32; /* bit[0-31]: CORE1编码通路RD环形完成中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                  1：屏蔽中断
                                                                  0：不屏蔽中断 */
    } reg;
} SOC_SOCP_ENC_CORE1_MASK3_UNION;
#endif
#define SOC_SOCP_ENC_CORE1_MASK3_core0_enc_rdq_done_mask_START  (0)
#define SOC_SOCP_ENC_CORE1_MASK3_core0_enc_rdq_done_mask_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_RAWINT3_UNION
 结构说明  : ENC_RAWINT3 寄存器结构定义。地址偏移量:0x0088，初值:0x00000000，宽度:32
 寄存器说明: 编码通道RD原始中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_rdq_done_rawint : 32; /* bit[0-31]: 编码通路RD环形完成原始中断状态,写1清，每个bit对应一个逻辑通道
                                                              1：有中断
                                                              0：无中断 */
    } reg;
} SOC_SOCP_ENC_RAWINT3_UNION;
#endif
#define SOC_SOCP_ENC_RAWINT3_enc_rdq_done_rawint_START  (0)
#define SOC_SOCP_ENC_RAWINT3_enc_rdq_done_rawint_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE0_INT3_UNION
 结构说明  : ENC_CORE0_INT3 寄存器结构定义。地址偏移量:0x008C，初值:0x00000000，宽度:32
 寄存器说明: CORE0编码RD中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_enc_rdq_done_int : 32; /* bit[0-31]: CORE0编码通路RD环形完成屏蔽后中断状态，每个bit对应一个逻辑通道
                                                                 1：有中断
                                                                 0：无中断 */
    } reg;
} SOC_SOCP_ENC_CORE0_INT3_UNION;
#endif
#define SOC_SOCP_ENC_CORE0_INT3_core0_enc_rdq_done_int_START  (0)
#define SOC_SOCP_ENC_CORE0_INT3_core0_enc_rdq_done_int_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE1_INT3_UNION
 结构说明  : ENC_CORE1_INT3 寄存器结构定义。地址偏移量:0x0090，初值:0x00000000，宽度:32
 寄存器说明: CORE1编码RD中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core1_enc_rdq_done_int : 32; /* bit[0-31]: CORE1编码通路RD环形完成屏蔽后中断状态，每个bit对应一个逻辑通道
                                                                 1：有中断
                                                                 0：无中断 */
    } reg;
} SOC_SOCP_ENC_CORE1_INT3_UNION;
#endif
#define SOC_SOCP_ENC_CORE1_INT3_core1_enc_rdq_done_int_START  (0)
#define SOC_SOCP_ENC_CORE1_INT3_core1_enc_rdq_done_int_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE0_MASK0_UNION
 结构说明  : DEC_CORE0_MASK0 寄存器结构定义。地址偏移量:0x00A8，初值:0x0000FFFF，宽度:32
 寄存器说明: CORE0解码通道传输中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_dtype0_tfr_int_mask0 : 4;  /* bit[0-3]  : core0解码通路DATA-TYPE0传输中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                      1：屏蔽中断
                                                                      0：不屏蔽中断 */
        unsigned int  core0_dtype1_tfr_int_mask1 : 4;  /* bit[4-7]  : core0解码通路DATA-TYPE1传输中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                      1：屏蔽中断
                                                                      0：不屏蔽中断 */
        unsigned int  core0_dtype2_tfr_int_mask2 : 4;  /* bit[8-11] : core0解码通路DATA-TYPE2传输中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                      1：屏蔽中断
                                                                      0：不屏蔽中断 */
        unsigned int  core0_dtype3_tfr_int_mask3 : 4;  /* bit[12-15]: core0解码通路DATA-TYPE3传输中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                      1：屏蔽中断
                                                                      0：不屏蔽中断 */
        unsigned int  reserved                   : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE0_MASK0_UNION;
#endif
#define SOC_SOCP_DEC_CORE0_MASK0_core0_dtype0_tfr_int_mask0_START  (0)
#define SOC_SOCP_DEC_CORE0_MASK0_core0_dtype0_tfr_int_mask0_END    (3)
#define SOC_SOCP_DEC_CORE0_MASK0_core0_dtype1_tfr_int_mask1_START  (4)
#define SOC_SOCP_DEC_CORE0_MASK0_core0_dtype1_tfr_int_mask1_END    (7)
#define SOC_SOCP_DEC_CORE0_MASK0_core0_dtype2_tfr_int_mask2_START  (8)
#define SOC_SOCP_DEC_CORE0_MASK0_core0_dtype2_tfr_int_mask2_END    (11)
#define SOC_SOCP_DEC_CORE0_MASK0_core0_dtype3_tfr_int_mask3_START  (12)
#define SOC_SOCP_DEC_CORE0_MASK0_core0_dtype3_tfr_int_mask3_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE1_MASK0_UNION
 结构说明  : DEC_CORE1_MASK0 寄存器结构定义。地址偏移量:0x00AC，初值:0x0000FFFF，宽度:32
 寄存器说明: CORE1解码通道传输中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core1_dtype0_tfr_int_mask0 : 4;  /* bit[0-3]  : core1解码通路DATA-TYPE0传输中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                      1：屏蔽中断
                                                                      0：不屏蔽中断 */
        unsigned int  core1_dtype1_tfr_int_mask1 : 4;  /* bit[4-7]  : core1解码通路DATA-TYPE1传输中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                      1：屏蔽中断
                                                                      0：不屏蔽中断 */
        unsigned int  core1_dtype2_tfr_int_mask2 : 4;  /* bit[8-11] : core1解码通路DATA-TYPE2传输中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                      1：屏蔽中断
                                                                      0：不屏蔽中断 */
        unsigned int  core1_dtype3_tfr_int_mask3 : 4;  /* bit[12-15]: core1解码通路DATA-TYPE3传输中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                      1：屏蔽中断
                                                                      0：不屏蔽中断 */
        unsigned int  reserved                   : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_SOCP_DEC_CORE1_MASK0_UNION;
#endif
#define SOC_SOCP_DEC_CORE1_MASK0_core1_dtype0_tfr_int_mask0_START  (0)
#define SOC_SOCP_DEC_CORE1_MASK0_core1_dtype0_tfr_int_mask0_END    (3)
#define SOC_SOCP_DEC_CORE1_MASK0_core1_dtype1_tfr_int_mask1_START  (4)
#define SOC_SOCP_DEC_CORE1_MASK0_core1_dtype1_tfr_int_mask1_END    (7)
#define SOC_SOCP_DEC_CORE1_MASK0_core1_dtype2_tfr_int_mask2_START  (8)
#define SOC_SOCP_DEC_CORE1_MASK0_core1_dtype2_tfr_int_mask2_END    (11)
#define SOC_SOCP_DEC_CORE1_MASK0_core1_dtype3_tfr_int_mask3_START  (12)
#define SOC_SOCP_DEC_CORE1_MASK0_core1_dtype3_tfr_int_mask3_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_RAWINT0_UNION
 结构说明  : DEC_RAWINT0 寄存器结构定义。地址偏移量:0x00B0，初值:0x00000000，宽度:32
 寄存器说明: 解码通道中断原始状态寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dtype0_tfr_rawint0 : 4;  /* bit[0-3]  : 解码通路DATA-TYPE0原始传输中断状态,写1清，每个bit对应一个逻辑通道
                                                              1：有中断
                                                              0：无中断 */
        unsigned int  dtype1_tfr_rawint1 : 4;  /* bit[4-7]  : 解码通路DATA-TYPE1原始传输中断状态,写1清，每个bit对应一个逻辑通道
                                                              1：有中断
                                                              0：无中断 */
        unsigned int  dtype2_tfr_rawint2 : 4;  /* bit[8-11] : 解码通路DATA-TYPE2原始传输中断状态,写1清，每个bit对应一个逻辑通道
                                                              1：有中断
                                                              0：无中断 */
        unsigned int  dtype3_tfr_rawint3 : 4;  /* bit[12-15]: 解码通路DATA-TYPE3原始传输中断状态,写1清，每个bit对应一个逻辑通道
                                                              1：有中断
                                                              0：无中断 */
        unsigned int  reserved           : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_RAWINT0_UNION;
#endif
#define SOC_SOCP_DEC_RAWINT0_dtype0_tfr_rawint0_START  (0)
#define SOC_SOCP_DEC_RAWINT0_dtype0_tfr_rawint0_END    (3)
#define SOC_SOCP_DEC_RAWINT0_dtype1_tfr_rawint1_START  (4)
#define SOC_SOCP_DEC_RAWINT0_dtype1_tfr_rawint1_END    (7)
#define SOC_SOCP_DEC_RAWINT0_dtype2_tfr_rawint2_START  (8)
#define SOC_SOCP_DEC_RAWINT0_dtype2_tfr_rawint2_END    (11)
#define SOC_SOCP_DEC_RAWINT0_dtype3_tfr_rawint3_START  (12)
#define SOC_SOCP_DEC_RAWINT0_dtype3_tfr_rawint3_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE0_INT0_UNION
 结构说明  : DEC_CORE0_INT0 寄存器结构定义。地址偏移量:0x00B4，初值:0x00000000，宽度:32
 寄存器说明: CORE0解码通道传输中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_dtype0_tfr_maskint0 : 4;  /* bit[0-3]  : core0解码通路DATA-TYPE0屏蔽后传输中断状态，每个bit对应一个逻辑通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core0_dtype1_tfr_maskint1 : 4;  /* bit[4-7]  : core0解码通路DATA-TYPE1屏蔽后传输中断状态，每个bit对应一个逻辑通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core0_dtype2_tfr_maskint2 : 4;  /* bit[8-11] : core0解码通路DATA-TYPE2屏蔽后传输中断状态，每个bit对应一个逻辑通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core0_dtype3_tfr_maskint3 : 4;  /* bit[12-15]: core0解码通路DATA-TYPE3屏蔽后传输中断状态，每个bit对应一个逻辑通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  reserved                  : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE0_INT0_UNION;
#endif
#define SOC_SOCP_DEC_CORE0_INT0_core0_dtype0_tfr_maskint0_START  (0)
#define SOC_SOCP_DEC_CORE0_INT0_core0_dtype0_tfr_maskint0_END    (3)
#define SOC_SOCP_DEC_CORE0_INT0_core0_dtype1_tfr_maskint1_START  (4)
#define SOC_SOCP_DEC_CORE0_INT0_core0_dtype1_tfr_maskint1_END    (7)
#define SOC_SOCP_DEC_CORE0_INT0_core0_dtype2_tfr_maskint2_START  (8)
#define SOC_SOCP_DEC_CORE0_INT0_core0_dtype2_tfr_maskint2_END    (11)
#define SOC_SOCP_DEC_CORE0_INT0_core0_dtype3_tfr_maskint3_START  (12)
#define SOC_SOCP_DEC_CORE0_INT0_core0_dtype3_tfr_maskint3_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE1_INT0_UNION
 结构说明  : DEC_CORE1_INT0 寄存器结构定义。地址偏移量:0x00B8，初值:0x00000000，宽度:32
 寄存器说明: CORE1解码通道传输中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core1_dtype0_tfr_maskint0 : 4;  /* bit[0-3]  : core1解码通路DATA-TYPE0屏蔽后传输中断状态，每个bit对应一个逻辑通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core1_dtype1_tfr_maskint1 : 4;  /* bit[4-7]  : core1解码通路DATA-TYPE1屏蔽后传输中断状态，每个bit对应一个逻辑通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core1_dtype2_tfr_maskint2 : 4;  /* bit[8-11] : core1解码通路DATA-TYPE2屏蔽后传输中断状态，每个bit对应一个逻辑通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core1_dtype3_tfr_maskint3 : 4;  /* bit[12-15]: core1解码通路DATA-TYPE3屏蔽后传输中断状态，每个bit对应一个逻辑通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  reserved                  : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE1_INT0_UNION;
#endif
#define SOC_SOCP_DEC_CORE1_INT0_core1_dtype0_tfr_maskint0_START  (0)
#define SOC_SOCP_DEC_CORE1_INT0_core1_dtype0_tfr_maskint0_END    (3)
#define SOC_SOCP_DEC_CORE1_INT0_core1_dtype1_tfr_maskint1_START  (4)
#define SOC_SOCP_DEC_CORE1_INT0_core1_dtype1_tfr_maskint1_END    (7)
#define SOC_SOCP_DEC_CORE1_INT0_core1_dtype2_tfr_maskint2_START  (8)
#define SOC_SOCP_DEC_CORE1_INT0_core1_dtype2_tfr_maskint2_END    (11)
#define SOC_SOCP_DEC_CORE1_INT0_core1_dtype3_tfr_maskint3_START  (12)
#define SOC_SOCP_DEC_CORE1_INT0_core1_dtype3_tfr_maskint3_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE0_MASK1_UNION
 结构说明  : DEC_CORE0_MASK1 寄存器结构定义。地址偏移量:0x00BC，初值:0xFFFFFFFF，宽度:32
 寄存器说明: CORE0解码通道异常中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0                    : 4;  /* bit[0-3]  : 保留 */
        unsigned int  core0_dec_ibuf_underflow_mask : 4;  /* bit[4-7]  : core0解码源buffer下溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core0_dec_flag_unmatch_mask   : 4;  /* bit[8-11] : core0解码标志(0x7E)不匹配中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core0_dec_dtype_unmatch_mask  : 4;  /* bit[12-15]: core0解码目的bufferID不匹配中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core0_dec_crc_err_mask        : 4;  /* bit[16-19]: core0解码通路CRC错中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core0_dec_len_err_mask        : 4;  /* bit[20-23]: core0解码通路包长度错中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core0_dec_7d_err_mask         : 4;  /* bit[24-27]: core0解码通路7D出错中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  reserved_1                    : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE0_MASK1_UNION;
#endif
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_ibuf_underflow_mask_START  (4)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_ibuf_underflow_mask_END    (7)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_flag_unmatch_mask_START    (8)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_flag_unmatch_mask_END      (11)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_dtype_unmatch_mask_START   (12)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_dtype_unmatch_mask_END     (15)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_crc_err_mask_START         (16)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_crc_err_mask_END           (19)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_len_err_mask_START         (20)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_len_err_mask_END           (23)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_7d_err_mask_START          (24)
#define SOC_SOCP_DEC_CORE0_MASK1_core0_dec_7d_err_mask_END            (27)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE0_RAWINT1_UNION
 结构说明  : DEC_CORE0_RAWINT1 寄存器结构定义。地址偏移量:0x00C0，初值:0x00000000，宽度:32
 寄存器说明: 解码通道异常中断原始状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0                 : 4;  /* bit[0-3]  : 保留 */
        unsigned int  dec_ibuf_underflow_rawint  : 4;  /* bit[4-7]  : 解码源buffer下溢原始中断状态,写1清，每个bit对应一个逻辑通道
                                                                      1：有中断
                                                                      0：无中断 */
        unsigned int  dec_flag_unmatch_rawint    : 4;  /* bit[8-11] : 解码标志(0x7E)不匹配原始中断状态,写1清，每个bit对应一个逻辑通道
                                                                      1：有中断
                                                                      0：无中断 */
        unsigned int  dec_obuf_id_unmatch_rawint : 4;  /* bit[12-15]: 解码目的bufferID不匹配原始中断状态,写1清，每个bit对应一个逻辑通道
                                                                      1：有中断
                                                                      0：无中断 */
        unsigned int  dec_crc_err_rawint         : 4;  /* bit[16-19]: 解码通路CRC错原始中断状态,写1清，每个bit对应一个逻辑通道
                                                                      1：有中断
                                                                      0：无中断 */
        unsigned int  dec_len_err_rawint         : 4;  /* bit[20-23]: 解码通路包长度错原始中断状态,写1清，每个bit对应一个逻辑通道
                                                                      1：有中断
                                                                      0：无中断 */
        unsigned int  dec_7d_err_rawint          : 4;  /* bit[24-27]: 解码通路7D出错原始中断状态，写1清，每个bit对应一个逻辑通道
                                                                      1：有中断
                                                                      0：无中断 */
        unsigned int  reserved_1                 : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE0_RAWINT1_UNION;
#endif
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_ibuf_underflow_rawint_START   (4)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_ibuf_underflow_rawint_END     (7)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_flag_unmatch_rawint_START     (8)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_flag_unmatch_rawint_END       (11)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_obuf_id_unmatch_rawint_START  (12)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_obuf_id_unmatch_rawint_END    (15)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_crc_err_rawint_START          (16)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_crc_err_rawint_END            (19)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_len_err_rawint_START          (20)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_len_err_rawint_END            (23)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_7d_err_rawint_START           (24)
#define SOC_SOCP_DEC_CORE0_RAWINT1_dec_7d_err_rawint_END             (27)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE0_INT1_UNION
 结构说明  : DEC_CORE0_INT1 寄存器结构定义。地址偏移量:0x00C4，初值:0x00000000，宽度:32
 寄存器说明: CORE0解码通道异常中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0                        : 4;  /* bit[0-3]  : 保留 */
        unsigned int  core0_dec_ibuf_underflow_maskint  : 4;  /* bit[4-7]  : core0解码源buffer下溢中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core0_dec_flag_unmatch_maskint    : 4;  /* bit[8-11] : core0解码标志(0x7E)不匹配中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core0_dec_obuf_id_unmatch_maskint : 4;  /* bit[12-15]: core0解码ID不匹配中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core0_dec_crc_err_maskint         : 4;  /* bit[16-19]: core0解码通路CRC错中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core0_dec_len_err_maskint         : 4;  /* bit[20-23]: core0解码通路包长度错中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core0_dec_7d_err_maskint          : 4;  /* bit[24-27]: core0解码通路7D出错中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  reserved_1                        : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE0_INT1_UNION;
#endif
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_ibuf_underflow_maskint_START   (4)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_ibuf_underflow_maskint_END     (7)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_flag_unmatch_maskint_START     (8)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_flag_unmatch_maskint_END       (11)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_obuf_id_unmatch_maskint_START  (12)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_obuf_id_unmatch_maskint_END    (15)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_crc_err_maskint_START          (16)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_crc_err_maskint_END            (19)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_len_err_maskint_START          (20)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_len_err_maskint_END            (23)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_7d_err_maskint_START           (24)
#define SOC_SOCP_DEC_CORE0_INT1_core0_dec_7d_err_maskint_END             (27)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE0_MASK2_UNION
 结构说明  : DEC_CORE0_MASK2 寄存器结构定义。地址偏移量:0x00C8，初值:0x0000FFFF，宽度:32
 寄存器说明: CORE0解码通道目的buffer溢出中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_dtype0_obuf_ovf_mask0 : 4;  /* bit[0-3]  : core0解码DATA-TYPE0目的buffer上溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                       1：屏蔽中断
                                                                       0：不屏蔽中断 */
        unsigned int  core0_dtype1_obuf_ovf_mask1 : 4;  /* bit[4-7]  : core0解码DATA-TYPE1目的buffer上溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                       1：屏蔽中断
                                                                       0：不屏蔽中断 */
        unsigned int  core0_dtype2_obuf_ovf_mask2 : 4;  /* bit[8-11] : core0解码DATA-TYPE2目的buffer上溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                       1：屏蔽中断
                                                                       0：不屏蔽中断 */
        unsigned int  core0_dtype3_obuf_ovf_mask3 : 4;  /* bit[12-15]: core0解码DATA-TYPE3目的buffer上溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                       1：屏蔽中断
                                                                       0：不屏蔽中断 */
        unsigned int  reserved                    : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE0_MASK2_UNION;
#endif
#define SOC_SOCP_DEC_CORE0_MASK2_core0_dtype0_obuf_ovf_mask0_START  (0)
#define SOC_SOCP_DEC_CORE0_MASK2_core0_dtype0_obuf_ovf_mask0_END    (3)
#define SOC_SOCP_DEC_CORE0_MASK2_core0_dtype1_obuf_ovf_mask1_START  (4)
#define SOC_SOCP_DEC_CORE0_MASK2_core0_dtype1_obuf_ovf_mask1_END    (7)
#define SOC_SOCP_DEC_CORE0_MASK2_core0_dtype2_obuf_ovf_mask2_START  (8)
#define SOC_SOCP_DEC_CORE0_MASK2_core0_dtype2_obuf_ovf_mask2_END    (11)
#define SOC_SOCP_DEC_CORE0_MASK2_core0_dtype3_obuf_ovf_mask3_START  (12)
#define SOC_SOCP_DEC_CORE0_MASK2_core0_dtype3_obuf_ovf_mask3_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE1Note_MASK2_UNION
 结构说明  : DEC_CORE1Note_MASK2 寄存器结构定义。地址偏移量:0x00CC，初值:0x0000FFFF，宽度:32
 寄存器说明: CORE1解码通道目的buffer溢出中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core1_dtype0_obuf_ovf_mask0 : 4;  /* bit[0-3]  : core1解码DATA-TYPE0目的buffer上溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                       1：屏蔽中断
                                                                       0：不屏蔽中断 */
        unsigned int  core1_dtype1_obuf_ovf_mask1 : 4;  /* bit[4-7]  : core1解码DATA-TYPE1目的buffer上溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                       1：屏蔽中断
                                                                       0：不屏蔽中断 */
        unsigned int  core1_dtype2_obuf_ovf_mask2 : 4;  /* bit[8-11] : core1解码DATA-TYPE2目的buffer上溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                       1：屏蔽中断
                                                                       0：不屏蔽中断 */
        unsigned int  core1_dtype3_obuf_ovf_mask3 : 4;  /* bit[12-15]: core1解码DATA-TYPE3目的buffer上溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                       1：屏蔽中断
                                                                       0：不屏蔽中断 */
        unsigned int  reserved                    : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE1Note_MASK2_UNION;
#endif
#define SOC_SOCP_DEC_CORE1Note_MASK2_core1_dtype0_obuf_ovf_mask0_START  (0)
#define SOC_SOCP_DEC_CORE1Note_MASK2_core1_dtype0_obuf_ovf_mask0_END    (3)
#define SOC_SOCP_DEC_CORE1Note_MASK2_core1_dtype1_obuf_ovf_mask1_START  (4)
#define SOC_SOCP_DEC_CORE1Note_MASK2_core1_dtype1_obuf_ovf_mask1_END    (7)
#define SOC_SOCP_DEC_CORE1Note_MASK2_core1_dtype2_obuf_ovf_mask2_START  (8)
#define SOC_SOCP_DEC_CORE1Note_MASK2_core1_dtype2_obuf_ovf_mask2_END    (11)
#define SOC_SOCP_DEC_CORE1Note_MASK2_core1_dtype3_obuf_ovf_mask3_START  (12)
#define SOC_SOCP_DEC_CORE1Note_MASK2_core1_dtype3_obuf_ovf_mask3_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_RAWINT2_UNION
 结构说明  : DEC_RAWINT2 寄存器结构定义。地址偏移量:0x00D0，初值:0x00000000，宽度:32
 寄存器说明: 解码通道目的buffer溢出中断原始状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dtype0_obuf_ovf_rawint0 : 4;  /* bit[0-3]  : 解码DATA-TYPE0目的buffer上溢原始中断状态,写1清，每个bit对应一个逻辑通道
                                                                   1：有中断
                                                                   0：无中断 */
        unsigned int  dtype1_obuf_ovf_rawint1 : 4;  /* bit[4-7]  : 解码DATA-TYPE1目的buffer上溢原始中断状态,写1清，每个bit对应一个逻辑通道
                                                                   1：有中断
                                                                   0：无中断 */
        unsigned int  dtype2_obuf_ovf_rawint2 : 4;  /* bit[8-11] : 解码DATA-TYPE2目的buffer上溢原始中断状态,写1清，每个bit对应一个逻辑通道
                                                                   1：有中断
                                                                   0：无中断 */
        unsigned int  dtype3_obuf_ovf_rawint3 : 4;  /* bit[12-15]: 解码DATA-TYPE3目的buffer上溢原始中断状态,写1清，每个bit对应一个逻辑通道
                                                                   1：有中断
                                                                   0：无中断 */
        unsigned int  reserved                : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_RAWINT2_UNION;
#endif
#define SOC_SOCP_DEC_RAWINT2_dtype0_obuf_ovf_rawint0_START  (0)
#define SOC_SOCP_DEC_RAWINT2_dtype0_obuf_ovf_rawint0_END    (3)
#define SOC_SOCP_DEC_RAWINT2_dtype1_obuf_ovf_rawint1_START  (4)
#define SOC_SOCP_DEC_RAWINT2_dtype1_obuf_ovf_rawint1_END    (7)
#define SOC_SOCP_DEC_RAWINT2_dtype2_obuf_ovf_rawint2_START  (8)
#define SOC_SOCP_DEC_RAWINT2_dtype2_obuf_ovf_rawint2_END    (11)
#define SOC_SOCP_DEC_RAWINT2_dtype3_obuf_ovf_rawint3_START  (12)
#define SOC_SOCP_DEC_RAWINT2_dtype3_obuf_ovf_rawint3_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE0Note_NT2_UNION
 结构说明  : DEC_CORE0Note_NT2 寄存器结构定义。地址偏移量:0x00D4，初值:0x00000000，宽度:32
 寄存器说明: CORE0解码通道目的buffer溢出中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core0_dtype0_obuf_ovf_int : 4;  /* bit[0-3]  : core0解码DATA-TYPE0目的buffer上溢屏蔽后中断状态
                                                                     每个bit对应一个解码通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core0_dtype1_obuf_ovf_int : 4;  /* bit[4-7]  : core0解码DATA-TYPE1目的buffer上溢屏蔽后中断状态
                                                                     每个bit对应一个解码通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core0_dtype2_obuf_ovf_int : 4;  /* bit[8-11] : core0解码DATA-TYPE2目的buffer上溢屏蔽后中断状态
                                                                     每个bit对应一个解码通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core0_dtype3_obuf_ovf_int : 4;  /* bit[12-15]: core0解码DATA-TYPE3目的buffer上溢屏蔽后中断状态
                                                                     每个bit对应一个解码通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  reserved                  : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE0Note_NT2_UNION;
#endif
#define SOC_SOCP_DEC_CORE0Note_NT2_core0_dtype0_obuf_ovf_int_START  (0)
#define SOC_SOCP_DEC_CORE0Note_NT2_core0_dtype0_obuf_ovf_int_END    (3)
#define SOC_SOCP_DEC_CORE0Note_NT2_core0_dtype1_obuf_ovf_int_START  (4)
#define SOC_SOCP_DEC_CORE0Note_NT2_core0_dtype1_obuf_ovf_int_END    (7)
#define SOC_SOCP_DEC_CORE0Note_NT2_core0_dtype2_obuf_ovf_int_START  (8)
#define SOC_SOCP_DEC_CORE0Note_NT2_core0_dtype2_obuf_ovf_int_END    (11)
#define SOC_SOCP_DEC_CORE0Note_NT2_core0_dtype3_obuf_ovf_int_START  (12)
#define SOC_SOCP_DEC_CORE0Note_NT2_core0_dtype3_obuf_ovf_int_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE1Note_INT2_UNION
 结构说明  : DEC_CORE1Note_INT2 寄存器结构定义。地址偏移量:0x00D8，初值:0x00000000，宽度:32
 寄存器说明: CORE1解码通道目的buffer溢出中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core1_dtype0_obuf_ovf_int : 4;  /* bit[0-3]  : core1解码DATA-TYPE0目的buffer上溢屏蔽后中断状态
                                                                     每个bit对应一个解码通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core1_dtype1_obuf_ovf_int : 4;  /* bit[4-7]  : core1解码DATA-TYPE1目的buffer上溢屏蔽后中断状态
                                                                     每个bit对应一个解码通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core1_dtype2_obuf_ovf_int : 4;  /* bit[8-11] : core1解码DATA-TYPE2目的buffer上溢屏蔽后中断状态
                                                                     每个bit对应一个解码通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  core1_dtype3_obuf_ovf_int : 4;  /* bit[12-15]: core1解码DATA-TYPE3目的buffer上溢屏蔽后中断状态
                                                                     每个bit对应一个解码通道
                                                                     1：有中断
                                                                     0：无中断 */
        unsigned int  reserved                  : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE1Note_INT2_UNION;
#endif
#define SOC_SOCP_DEC_CORE1Note_INT2_core1_dtype0_obuf_ovf_int_START  (0)
#define SOC_SOCP_DEC_CORE1Note_INT2_core1_dtype0_obuf_ovf_int_END    (3)
#define SOC_SOCP_DEC_CORE1Note_INT2_core1_dtype1_obuf_ovf_int_START  (4)
#define SOC_SOCP_DEC_CORE1Note_INT2_core1_dtype1_obuf_ovf_int_END    (7)
#define SOC_SOCP_DEC_CORE1Note_INT2_core1_dtype2_obuf_ovf_int_START  (8)
#define SOC_SOCP_DEC_CORE1Note_INT2_core1_dtype2_obuf_ovf_int_END    (11)
#define SOC_SOCP_DEC_CORE1Note_INT2_core1_dtype3_obuf_ovf_int_START  (12)
#define SOC_SOCP_DEC_CORE1Note_INT2_core1_dtype3_obuf_ovf_int_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE1_MASK0_UNION
 结构说明  : ENC_CORE1_MASK0 寄存器结构定义。地址偏移量:0x00DC，初值:0x0000007F，宽度:32
 寄存器说明: CORE1编码传输中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core1_enc_tfr_mask : 7;  /* bit[0-6] : core1编码通路传输中断屏蔽控制位。每个bit对应一个目的buffer，Bit0对应目的buffer0。 
                                                             1：屏蔽中断
                                                             0：不屏蔽中断 */
        unsigned int  reserved           : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE1_MASK0_UNION;
#endif
#define SOC_SOCP_ENC_CORE1_MASK0_core1_enc_tfr_mask_START  (0)
#define SOC_SOCP_ENC_CORE1_MASK0_core1_enc_tfr_mask_END    (6)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE1_INT0_UNION
 结构说明  : ENC_CORE1_INT0 寄存器结构定义。地址偏移量:0x00E0，初值:0x00000000，宽度:32
 寄存器说明: CORE1编码传输中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core1_enc_tfr_maskint : 7;  /* bit[0-6] : core1编码通路屏蔽后传输中断状态。每个bit对应一个目的buffer，Bit0对应目的buffer0。
                                                                1：有中断
                                                                0：无中断 */
        unsigned int  reserved              : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE1_INT0_UNION;
#endif
#define SOC_SOCP_ENC_CORE1_INT0_core1_enc_tfr_maskint_START  (0)
#define SOC_SOCP_ENC_CORE1_INT0_core1_enc_tfr_maskint_END    (6)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE1_MASK2_UNION
 结构说明  : ENC_CORE1_MASK2 寄存器结构定义。地址偏移量:0x00E4，初值:0x007F007F，宽度:32
 寄存器说明: CORE1编码目的buffer上溢中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core1_enc_obuf_ovf_mask      : 7;  /* bit[0-6]  : core1编码目的buffer剩余空间不够一次dma传输时上溢中断屏蔽控制，每个bit对应一个目的buffer
                                                                        1：屏蔽中断
                                                                        0：不屏蔽中断 */
        unsigned int  reserved_0                   : 9;  /* bit[7-15] : 保留。 */
        unsigned int  core1_enc_obuf_thrh_ovf_mask : 7;  /* bit[16-22]: core1编码目的buffer剩余空间小于配置的阈值时上溢中断屏蔽控制，每个bit对应一个目的buffer
                                                                        1：屏蔽中断
                                                                        0：不屏蔽中断 */
        unsigned int  reserved_1                   : 9;  /* bit[23-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE1_MASK2_UNION;
#endif
#define SOC_SOCP_ENC_CORE1_MASK2_core1_enc_obuf_ovf_mask_START       (0)
#define SOC_SOCP_ENC_CORE1_MASK2_core1_enc_obuf_ovf_mask_END         (6)
#define SOC_SOCP_ENC_CORE1_MASK2_core1_enc_obuf_thrh_ovf_mask_START  (16)
#define SOC_SOCP_ENC_CORE1_MASK2_core1_enc_obuf_thrh_ovf_mask_END    (22)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CORE1_INT2_UNION
 结构说明  : ENC_CORE1_INT2 寄存器结构定义。地址偏移量:0x00E8，初值:0x00000000，宽度:32
 寄存器说明: CORE1编码目的buffer上溢中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core1_enc_obuf_ovf_maskint      : 7;  /* bit[0-6]  : core1编码目的buffer剩余空间不够一次dma传输时上溢屏蔽后中断状态，每个bit对应一个目的buffer
                                                                           1：有中断
                                                                           0：无中断 */
        unsigned int  reserved_0                      : 9;  /* bit[7-15] : 保留。 */
        unsigned int  core1_enc_obuf_thrh_ovf_maskint : 7;  /* bit[16-22]: core1编码目的buffer剩余空间小于配置的阈值时上溢屏蔽后中断状态，每个bit对应一个目的buffer
                                                                           1：有中断
                                                                           0：无中断 */
        unsigned int  reserved_1                      : 9;  /* bit[23-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_CORE1_INT2_UNION;
#endif
#define SOC_SOCP_ENC_CORE1_INT2_core1_enc_obuf_ovf_maskint_START       (0)
#define SOC_SOCP_ENC_CORE1_INT2_core1_enc_obuf_ovf_maskint_END         (6)
#define SOC_SOCP_ENC_CORE1_INT2_core1_enc_obuf_thrh_ovf_maskint_START  (16)
#define SOC_SOCP_ENC_CORE1_INT2_core1_enc_obuf_thrh_ovf_maskint_END    (22)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE1_MASK1_UNION
 结构说明  : DEC_CORE1_MASK1 寄存器结构定义。地址偏移量:0x00EC，初值:0xFFFFFFFF，宽度:32
 寄存器说明: CORE1解码通道异常中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0                    : 4;  /* bit[0-3]  : 保留 */
        unsigned int  core1_dec_ibuf_underflow_mask : 4;  /* bit[4-7]  : core1解码源buffer下溢中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core1_dec_flag_unmatch_mask   : 4;  /* bit[8-11] : core1解码标志(0x7E)不匹配中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core1_dec_dtype_unmatch_mask  : 4;  /* bit[12-15]: core1解码目的bufferID不匹配中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core1_dec_crc_err_mask        : 4;  /* bit[16-19]: core1解码通路CRC错中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core1_dec_len_err_mask        : 4;  /* bit[20-23]: core1解码通路包长度错中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  core1_dec_7d_err_mask         : 4;  /* bit[24-27]: core1解码通路7D出错中断屏蔽控制位，每个bit对应一个逻辑通道
                                                                         1：屏蔽中断
                                                                         0：不屏蔽中断 */
        unsigned int  reserved_1                    : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE1_MASK1_UNION;
#endif
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_ibuf_underflow_mask_START  (4)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_ibuf_underflow_mask_END    (7)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_flag_unmatch_mask_START    (8)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_flag_unmatch_mask_END      (11)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_dtype_unmatch_mask_START   (12)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_dtype_unmatch_mask_END     (15)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_crc_err_mask_START         (16)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_crc_err_mask_END           (19)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_len_err_mask_START         (20)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_len_err_mask_END           (23)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_7d_err_mask_START          (24)
#define SOC_SOCP_DEC_CORE1_MASK1_core1_dec_7d_err_mask_END            (27)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_CORE1_INT1_UNION
 结构说明  : DEC_CORE1_INT1 寄存器结构定义。地址偏移量:0x00F0，初值:0x00000000，宽度:32
 寄存器说明: CORE1解码通道异常中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0                        : 4;  /* bit[0-3]  : 保留 */
        unsigned int  core1_dec_ibuf_underflow_maskint  : 4;  /* bit[4-7]  : core1解码源buffer下溢中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core1_dec_flag_unmatch_maskint    : 4;  /* bit[8-11] : core1解码标志(0x7E)不匹配中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core1_dec_obuf_id_unmatch_maskint : 4;  /* bit[12-15]: core1解码ID不匹配中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core1_dec_crc_err_maskint         : 4;  /* bit[16-19]: core1解码通路CRC错中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core1_dec_len_err_maskint         : 4;  /* bit[20-23]: core1解码通路包长度错中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  core1_dec_7d_err_maskint          : 4;  /* bit[24-27]: core1解码通路7D出错中断状态，每个bit对应一个逻辑通道
                                                                             1：有中断
                                                                             0：无中断 */
        unsigned int  reserved_1                        : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_SOCP_DEC_CORE1_INT1_UNION;
#endif
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_ibuf_underflow_maskint_START   (4)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_ibuf_underflow_maskint_END     (7)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_flag_unmatch_maskint_START     (8)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_flag_unmatch_maskint_END       (11)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_obuf_id_unmatch_maskint_START  (12)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_obuf_id_unmatch_maskint_END    (15)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_crc_err_maskint_START          (16)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_crc_err_maskint_END            (19)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_len_err_maskint_START          (20)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_len_err_maskint_END            (23)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_7d_err_maskint_START           (24)
#define SOC_SOCP_DEC_CORE1_INT1_core1_dec_7d_err_maskint_END             (27)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRC_BUFm_WPTR_UNION
 结构说明  : ENC_SRC_BUFm_WPTR 寄存器结构定义。地址偏移量:0x0100+0x40*(m)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路源bufferm写指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_src_bufm_wptr : 32; /* bit[0-31]: 编码通道源bufferm写指针，绝对地址，以字节为单位 */
    } reg;
} SOC_SOCP_ENC_SRC_BUFm_WPTR_UNION;
#endif
#define SOC_SOCP_ENC_SRC_BUFm_WPTR_enc_src_bufm_wptr_START  (0)
#define SOC_SOCP_ENC_SRC_BUFm_WPTR_enc_src_bufm_wptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRC_BUFm_RPTR_UNION
 结构说明  : ENC_SRC_BUFm_RPTR 寄存器结构定义。地址偏移量:0x0104+0x40*(m)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路源bufferm读指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_src_bufm_rptr : 32; /* bit[0-31]: 编码通道源bufferm读指针，绝对地址，以字节为单位 */
    } reg;
} SOC_SOCP_ENC_SRC_BUFm_RPTR_UNION;
#endif
#define SOC_SOCP_ENC_SRC_BUFm_RPTR_enc_src_bufm_rptr_START  (0)
#define SOC_SOCP_ENC_SRC_BUFm_RPTR_enc_src_bufm_rptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRC_BUFm_ADDR_UNION
 结构说明  : ENC_SRC_BUFm_ADDR 寄存器结构定义。地址偏移量:0x0108+0x40*(m)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路源bufferm起始地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_src_bufm_addr : 32; /* bit[0-31]: fs */
    } reg;
} SOC_SOCP_ENC_SRC_BUFm_ADDR_UNION;
#endif
#define SOC_SOCP_ENC_SRC_BUFm_ADDR_enc_src_bufm_addr_START  (0)
#define SOC_SOCP_ENC_SRC_BUFm_ADDR_enc_src_bufm_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRC_BUFm_CFG0_UNION
 结构说明  : ENC_SRC_BUFm_CFG0 寄存器结构定义。地址偏移量:0x010C+0x40*(m)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路源bufferm配置寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_src_bufm_size : 27; /* bit[0-26] : 编码通路源bufferm buffer大小，以字节为单位
                                                             BBP数采通路buffer空间为20.2MB以上,bit[26:0]最大支持128MB */
        unsigned int  reserved          : 5;  /* bit[27-31]: 保留。 */
    } reg;
} SOC_SOCP_ENC_SRC_BUFm_CFG0_UNION;
#endif
#define SOC_SOCP_ENC_SRC_BUFm_CFG0_enc_src_bufm_size_START  (0)
#define SOC_SOCP_ENC_SRC_BUFm_CFG0_enc_src_bufm_size_END    (26)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRC_BUFm_CFG1_UNION
 结构说明  : ENC_SRC_BUFm_CFG1 寄存器结构定义。地址偏移量:0x0110+0x40*(m)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路源bufferm配置寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_chm_en          : 1;  /* bit[0]    : 通道使能控制。
                                                               1：通道使能；
                                                               0 ：通道不使能 */
        unsigned int  enc_chm_mode        : 2;  /* bit[1-2]  : 源buffer数据结构。
                                                               1?：链表结构；
                                                               01：数采通路固定长度数据包结构
                                                               00：连续存储数据包结构 */
        unsigned int  reserved_0          : 1;  /* bit[3]    : 保留。 */
        unsigned int  enc_chm_dest_buf_id : 4;  /* bit[4-7]  : 目的bufferID。
                                                               目前支持7个目的buffer，配置范围0~6。 */
        unsigned int  enc_chm_prior_id    : 2;  /* bit[8-9]  : 高低优先级队列配置。
                                                               11：最高优先级队列；
                                                               10：次高优先级队列；
                                                               01：次低优先级队列； 
                                                               00：最低优先级队列。 */
        unsigned int  bypass_en           : 1;  /* bit[10]   : 编码通路bypass使能信号
                                                               1：通道bypass使能
                                                               0：通道bypass不使能 */
        unsigned int  no_data_type        : 1;  /* bit[11]   : 编码通路data_type字段配置
                                                               1：传输不带有data_type字段
                                                               0：传输带有data_type字段 */
        unsigned int  reserved_1          : 4;  /* bit[12-15]: 保留 */
        unsigned int  data_type           : 8;  /* bit[16-23]: 多模类型指示。目前使用3个模，配0表示LTE模或TDS模，配1表示GU模。 */
        unsigned int  reserved_2          : 7;  /* bit[24-30]: 保留 */
        unsigned int  enc_debug           : 1;  /* bit[31]   : 当debug模式有效时，若出现“HISI”包头校验错误，则不再更新源buffer的读指针，供软件调试用。注意：软件调试时，建议屏蔽包头错中断后不要清除中断，这样错误通道便不再参与仲裁；否则再次仲裁到错误通道时会继续上报包头错误中断。
                                                               需复位该通道重新使用。
                                                               1：debug 模式
                                                               0：正常模式 */
    } reg;
} SOC_SOCP_ENC_SRC_BUFm_CFG1_UNION;
#endif
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_chm_en_START           (0)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_chm_en_END             (0)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_chm_mode_START         (1)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_chm_mode_END           (2)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_chm_dest_buf_id_START  (4)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_chm_dest_buf_id_END    (7)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_chm_prior_id_START     (8)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_chm_prior_id_END       (9)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_bypass_en_START            (10)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_bypass_en_END              (10)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_no_data_type_START         (11)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_no_data_type_END           (11)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_data_type_START            (16)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_data_type_END              (23)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_debug_START            (31)
#define SOC_SOCP_ENC_SRC_BUFm_CFG1_enc_debug_END              (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRC_RDQ_WPTR_UNION
 结构说明  : ENC_SRC_RDQ_WPTR 寄存器结构定义。地址偏移量:0x0114+0x40*(m)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路RD环形buffer写指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_bufm_rdq_wptr : 32; /* bit[0-31]: 编码通路Rd环形buffer写指针，绝对地址，以字节为单位 */
    } reg;
} SOC_SOCP_ENC_SRC_RDQ_WPTR_UNION;
#endif
#define SOC_SOCP_ENC_SRC_RDQ_WPTR_enc_bufm_rdq_wptr_START  (0)
#define SOC_SOCP_ENC_SRC_RDQ_WPTR_enc_bufm_rdq_wptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRC_RDQ_RPTR_UNION
 结构说明  : ENC_SRC_RDQ_RPTR 寄存器结构定义。地址偏移量:0x0118+0x40*(m)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路RD环形buffer写指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_bufm_rdq_rptr : 32; /* bit[0-31]: 编码通路Rd环形buffer读指针，绝对地址，以字节为单位 */
    } reg;
} SOC_SOCP_ENC_SRC_RDQ_RPTR_UNION;
#endif
#define SOC_SOCP_ENC_SRC_RDQ_RPTR_enc_bufm_rdq_rptr_START  (0)
#define SOC_SOCP_ENC_SRC_RDQ_RPTR_enc_bufm_rdq_rptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRC_RDQ_BADDR_UNION
 结构说明  : ENC_SRC_RDQ_BADDR 寄存器结构定义。地址偏移量:0x011C+0x40*(m)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路RD环形buffer起始地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_bufm_rdq_baddr : 32; /* bit[0-31]: 编码通路Rd环形buffer起始地址，8字节对齐 */
    } reg;
} SOC_SOCP_ENC_SRC_RDQ_BADDR_UNION;
#endif
#define SOC_SOCP_ENC_SRC_RDQ_BADDR_enc_bufm_rdq_baddr_START  (0)
#define SOC_SOCP_ENC_SRC_RDQ_BADDR_enc_bufm_rdq_baddr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_SRC_RDQ_CFG_UNION
 结构说明  : ENC_SRC_RDQ_CFG 寄存器结构定义。地址偏移量:0x0120+0x40*(m)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路RD环形buffer配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_bufm_rdq_size : 16; /* bit[0-15] : Rd环形buffer大小，以字节为单位 */
        unsigned int  reserved          : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_SOCP_ENC_SRC_RDQ_CFG_UNION;
#endif
#define SOC_SOCP_ENC_SRC_RDQ_CFG_enc_bufm_rdq_size_START  (0)
#define SOC_SOCP_ENC_SRC_RDQ_CFG_enc_bufm_rdq_size_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_DEST_BUFn_WPTR_UNION
 结构说明  : ENC_DEST_BUFn_WPTR 寄存器结构定义。地址偏移量:0x0900+0x20*(n)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路目的buffer写指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_dest_bufn_wptr : 32; /* bit[0-31]: 编码通路目的buffern写指针，绝对地址，以字节为单位 */
    } reg;
} SOC_SOCP_ENC_DEST_BUFn_WPTR_UNION;
#endif
#define SOC_SOCP_ENC_DEST_BUFn_WPTR_enc_dest_bufn_wptr_START  (0)
#define SOC_SOCP_ENC_DEST_BUFn_WPTR_enc_dest_bufn_wptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_DEST_BUFn_RPTR_UNION
 结构说明  : ENC_DEST_BUFn_RPTR 寄存器结构定义。地址偏移量:0x0904+0x20*(n)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路目的buffer读指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_dest_bufn_rptr : 32; /* bit[0-31]: 编码通路目的buffern读指针，绝对地址，以字节为单位 */
    } reg;
} SOC_SOCP_ENC_DEST_BUFn_RPTR_UNION;
#endif
#define SOC_SOCP_ENC_DEST_BUFn_RPTR_enc_dest_bufn_rptr_START  (0)
#define SOC_SOCP_ENC_DEST_BUFn_RPTR_enc_dest_bufn_rptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_DEST_BUFn_ADDR_UNION
 结构说明  : ENC_DEST_BUFn_ADDR 寄存器结构定义。地址偏移量:0x0908+0x20*(n)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路目的buffer起始地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_dest_bufn_addr : 32; /* bit[0-31]: 编码通路目的buffern起始地址，8字节对齐 */
    } reg;
} SOC_SOCP_ENC_DEST_BUFn_ADDR_UNION;
#endif
#define SOC_SOCP_ENC_DEST_BUFn_ADDR_enc_dest_bufn_addr_START  (0)
#define SOC_SOCP_ENC_DEST_BUFn_ADDR_enc_dest_bufn_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_DEST_BUFn_CFG_UNION
 结构说明  : ENC_DEST_BUFn_CFG 寄存器结构定义。地址偏移量:0x090C+0x20*(n)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路目的buffer配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_dest_bufn_size : 21; /* bit[0-20] : 编码通路目的buffern大小，以字节为单位
                                                              支持buffer空间最大为2M字节 */
        unsigned int  enc_int_threshold  : 11; /* bit[21-31]: 编码通路中断上报阈值门限，以KB字节为单位 */
    } reg;
} SOC_SOCP_ENC_DEST_BUFn_CFG_UNION;
#endif
#define SOC_SOCP_ENC_DEST_BUFn_CFG_enc_dest_bufn_size_START  (0)
#define SOC_SOCP_ENC_DEST_BUFn_CFG_enc_dest_bufn_size_END    (20)
#define SOC_SOCP_ENC_DEST_BUFn_CFG_enc_int_threshold_START   (21)
#define SOC_SOCP_ENC_DEST_BUFn_CFG_enc_int_threshold_END     (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_DEST_BUFn_THRH_UNION
 结构说明  : ENC_DEST_BUFn_THRH 寄存器结构定义。地址偏移量:0x0910+0x20*(n)，初值:0x00000000，宽度:32
 寄存器说明: 编码通路目的buffer仲裁阈值寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enc_dest_bufn_thrh : 32; /* bit[0-31]: 编码目的buffer阈值配置。若剩余空间大于此阈值，此目的buffer对应的编码通道参与仲裁；否则不参与仲裁。 */
    } reg;
} SOC_SOCP_ENC_DEST_BUFn_THRH_UNION;
#endif
#define SOC_SOCP_ENC_DEST_BUFn_THRH_enc_dest_bufn_thrh_START  (0)
#define SOC_SOCP_ENC_DEST_BUFn_THRH_enc_dest_bufn_thrh_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_SRC_BUFx_WPTR_UNION
 结构说明  : DEC_SRC_BUFx_WPTR 寄存器结构定义。地址偏移量:0(x)0A00+0(x)40*(x)，初值:0x00000000，宽度:32
 寄存器说明: 解码通路源bufferx写指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_src_bufx_wptr : 32; /* bit[0-31]: 解码通路源bufferx写指针，以字节为单位.
                                                            目前支持4个解码通路 */
    } reg;
} SOC_SOCP_DEC_SRC_BUFx_WPTR_UNION;
#endif
#define SOC_SOCP_DEC_SRC_BUFx_WPTR_dec_src_bufx_wptr_START  (0)
#define SOC_SOCP_DEC_SRC_BUFx_WPTR_dec_src_bufx_wptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_SRC_BUFx_RPTR_UNION
 结构说明  : DEC_SRC_BUFx_RPTR 寄存器结构定义。地址偏移量:0(x)0A04+0(x)40*(x)，初值:0x00000000，宽度:32
 寄存器说明: 解码通路源bufferx读指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_src_bufx_rptr : 32; /* bit[0-31]: 解码通路源bufferx读指针，以字节为单位.
                                                            目前支持4个解码通路 */
    } reg;
} SOC_SOCP_DEC_SRC_BUFx_RPTR_UNION;
#endif
#define SOC_SOCP_DEC_SRC_BUFx_RPTR_dec_src_bufx_rptr_START  (0)
#define SOC_SOCP_DEC_SRC_BUFx_RPTR_dec_src_bufx_rptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_SRC_BUFx_ADDR_UNION
 结构说明  : DEC_SRC_BUFx_ADDR 寄存器结构定义。地址偏移量:0(x)0A08+0(x)40*(x)，初值:0x00000000，宽度:32
 寄存器说明: 解码通路源bufferx起始地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_src_bufx_addr : 32; /* bit[0-31]: 解码通路源buffer起始地址。
                                                            8字节对齐 */
    } reg;
} SOC_SOCP_DEC_SRC_BUFx_ADDR_UNION;
#endif
#define SOC_SOCP_DEC_SRC_BUFx_ADDR_dec_src_bufx_addr_START  (0)
#define SOC_SOCP_DEC_SRC_BUFx_ADDR_dec_src_bufx_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_SRC_BUFx_CFG0_UNION
 结构说明  : DEC_SRC_BUFx_CFG0 寄存器结构定义。地址偏移量:0(x)0A0C+0(x)40*(x)，初值:0x00000000，宽度:32
 寄存器说明: 解码通路源bufferx配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_src_bufx_size : 16; /* bit[0-15] : 解码通路源buffer大小，以字节为单位。 */
        unsigned int  reserved_0        : 10; /* bit[16-25]: 保留 */
        unsigned int  no_data_type_sel  : 2;  /* bit[26-27]: 解码源数据不含data-type字段时对应的目的buffer配置
                                                             11：解码后数据存入data-type为3（保留模）的目的buffer。
                                                             10：解码后数据存入data-type为2（保留模）的目的buffer。
                                                             01：解码后数据存入data-type为1（GU模）的目的buffer。
                                                             00：解码后数据存入data-type为0（LTE模或TDS模）的目的buffer。 */
        unsigned int  reserved_1        : 1;  /* bit[28]   : 保留 */
        unsigned int  dec_debug         : 1;  /* bit[29]   : debug测试配置。
                                                             1：调试使能。若通道异常，上报异常中断，则停止处理该通道，直到异常恢复或者复位。
                                                             0：调试不使能。通道异常时，上报异常中断，继续处理该通道。 */
        unsigned int  dec_chx_en        : 1;  /* bit[30]   : 通道使能配置。
                                                             1：使能；
                                                             0：不使能 */
        unsigned int  no_data_type      : 1;  /* bit[31]   : 解码通路data_type字段配置
                                                             1：解码数据不带有data_type字段
                                                             0：解码数据带有data_type字段
                                                             注：若解码数据不带有data_type字段，则将解码后数据放入no_data_type_sel配置对应的目的buffer */
    } reg;
} SOC_SOCP_DEC_SRC_BUFx_CFG0_UNION;
#endif
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_dec_src_bufx_size_START  (0)
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_dec_src_bufx_size_END    (15)
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_no_data_type_sel_START   (26)
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_no_data_type_sel_END     (27)
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_dec_debug_START          (29)
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_dec_debug_END            (29)
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_dec_chx_en_START         (30)
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_dec_chx_en_END           (30)
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_no_data_type_START       (31)
#define SOC_SOCP_DEC_SRC_BUFx_CFG0_no_data_type_END         (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_BUFx_STATUS0_UNION
 结构说明  : DEC_BUFx_STATUS0 寄存器结构定义。地址偏移量:0(x)0A20+0(x)40*(x)，初值:0x00000000，宽度:32
 寄存器说明: 解码通路状态寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_crc_err_cnt : 16; /* bit[0-15] : 解码通路x CRC错数据包个数 */
        unsigned int  dec_len_err_cnt : 16; /* bit[16-31]: 解码通路包长度错数据包个数。
                                                           解码后的包长>最大包长，<最小包长
                                                           查找了最大包长个字节未发现包头7E。 */
    } reg;
} SOC_SOCP_DEC_BUFx_STATUS0_UNION;
#endif
#define SOC_SOCP_DEC_BUFx_STATUS0_dec_crc_err_cnt_START  (0)
#define SOC_SOCP_DEC_BUFx_STATUS0_dec_crc_err_cnt_END    (15)
#define SOC_SOCP_DEC_BUFx_STATUS0_dec_len_err_cnt_START  (16)
#define SOC_SOCP_DEC_BUFx_STATUS0_dec_len_err_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_BUFx_STATUS1_UNION
 结构说明  : DEC_BUFx_STATUS1 寄存器结构定义。地址偏移量:0(x)0A24+0(x)40*(x)，初值:0x00000000，宽度:32
 寄存器说明: 解码通路状态寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_start_err_cnt : 16; /* bit[0-15] : 启动异常计数（上一包包尾7E和当前包包头7E不匹配） */
        unsigned int  data_type_err_cnt : 16; /* bit[16-31]: data_type 错误统计计数 */
    } reg;
} SOC_SOCP_DEC_BUFx_STATUS1_UNION;
#endif
#define SOC_SOCP_DEC_BUFx_STATUS1_dec_start_err_cnt_START  (0)
#define SOC_SOCP_DEC_BUFx_STATUS1_dec_start_err_cnt_END    (15)
#define SOC_SOCP_DEC_BUFx_STATUS1_data_type_err_cnt_START  (16)
#define SOC_SOCP_DEC_BUFx_STATUS1_data_type_err_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_DEST_BUFy_WPTR_UNION
 结构说明  : DEC_DEST_BUFy_WPTR 寄存器结构定义。地址偏移量:0x0C00+0x10*y，初值:0x00000000，宽度:32
 寄存器说明: 解码通路目的buffery写指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_dest_bufy_wptr : 32; /* bit[0-31]: 解码通路目的buffery写指针，绝对地址，以字节为单位.
                                                             目前支持4*4个解码通路 */
    } reg;
} SOC_SOCP_DEC_DEST_BUFy_WPTR_UNION;
#endif
#define SOC_SOCP_DEC_DEST_BUFy_WPTR_dec_dest_bufy_wptr_START  (0)
#define SOC_SOCP_DEC_DEST_BUFy_WPTR_dec_dest_bufy_wptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_DEST_BUFy_RPTR_UNION
 结构说明  : DEC_DEST_BUFy_RPTR 寄存器结构定义。地址偏移量:0x0C04+0x10*y，初值:0x00000000，宽度:32
 寄存器说明: 解码通路目的buffery读指针寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_dest_bufy_rptr : 32; /* bit[0-31]: 解码通路目的buffery读指针，绝对地址，以字节为单位.
                                                             目前支持4*4个解码通路 */
    } reg;
} SOC_SOCP_DEC_DEST_BUFy_RPTR_UNION;
#endif
#define SOC_SOCP_DEC_DEST_BUFy_RPTR_dec_dest_bufy_rptr_START  (0)
#define SOC_SOCP_DEC_DEST_BUFy_RPTR_dec_dest_bufy_rptr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_DEST_BUFy_ADDR_UNION
 结构说明  : DEC_DEST_BUFy_ADDR 寄存器结构定义。地址偏移量:0x0C08+0x10*y，初值:0x00000000，宽度:32
 寄存器说明: 解码通路目的buffery起始地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_dest_bufy_addr : 32; /* bit[0-31]: 解码通路目的buffery起始地址，8字节对齐 */
    } reg;
} SOC_SOCP_DEC_DEST_BUFy_ADDR_UNION;
#endif
#define SOC_SOCP_DEC_DEST_BUFy_ADDR_dec_dest_bufy_addr_START  (0)
#define SOC_SOCP_DEC_DEST_BUFy_ADDR_dec_dest_bufy_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_DEST_BUFy_CFG0_UNION
 结构说明  : DEC_DEST_BUFy_CFG0 寄存器结构定义。地址偏移量:0x0C0C+0x10*y，初值:0xFF010000，宽度:32
 寄存器说明: 解码通路目的buffery配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dec_dest_bufy_size : 16; /* bit[0-15] : 解码通路目的buffery大小寄存器，字节为单位，8字节的整数倍 */
        unsigned int  dec_int_threshold  : 8;  /* bit[16-23]: 解码通路中断上报阈值门限，以数据包为单位。 */
        unsigned int  data_type          : 8;  /* bit[24-31]: 配置目的buffer的多模类型指示。目前使用3个模，配0表示LTE或TDS模对应的目的buffer，配1表示GU模对应的目的buffer。 */
    } reg;
} SOC_SOCP_DEC_DEST_BUFy_CFG0_UNION;
#endif
#define SOC_SOCP_DEC_DEST_BUFy_CFG0_dec_dest_bufy_size_START  (0)
#define SOC_SOCP_DEC_DEST_BUFy_CFG0_dec_dest_bufy_size_END    (15)
#define SOC_SOCP_DEC_DEST_BUFy_CFG0_dec_int_threshold_START   (16)
#define SOC_SOCP_DEC_DEST_BUFy_CFG0_dec_int_threshold_END     (23)
#define SOC_SOCP_DEC_DEST_BUFy_CFG0_data_type_START           (24)
#define SOC_SOCP_DEC_DEST_BUFy_CFG0_data_type_END             (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CD_DBG0_UNION
 结构说明  : ENC_CD_DBG0 寄存器结构定义。地址偏移量:0x0E00，初值:0x00000000，宽度:32
 寄存器说明: 编码CD debug寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cd_buf_baddr : 32; /* bit[0-31]: CD消息片buffer起始地址 */
    } reg;
} SOC_SOCP_ENC_CD_DBG0_UNION;
#endif
#define SOC_SOCP_ENC_CD_DBG0_cd_buf_baddr_START  (0)
#define SOC_SOCP_ENC_CD_DBG0_cd_buf_baddr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_CD_DBG1_UNION
 结构说明  : ENC_CD_DBG1 寄存器结构定义。地址偏移量:0x0E04，初值:0x00020000，宽度:32
 寄存器说明: 编码CD debug寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cd_blk_len   : 16; /* bit[0-15] : CD消息片数据包大小 */
        unsigned int  cd_last_flag : 1;  /* bit[16]   : 最后一个CD消息片
                                                        1：最后一个
                                                        0：不是最后一个 */
        unsigned int  cd_idle      : 1;  /* bit[17]   : cd_ctrl模块处于空闲状态
                                                        1：空闲
                                                        0：不空闲 */
        unsigned int  reserved     : 14; /* bit[18-31]: 保留 */
    } reg;
} SOC_SOCP_ENC_CD_DBG1_UNION;
#endif
#define SOC_SOCP_ENC_CD_DBG1_cd_blk_len_START    (0)
#define SOC_SOCP_ENC_CD_DBG1_cd_blk_len_END      (15)
#define SOC_SOCP_ENC_CD_DBG1_cd_last_flag_START  (16)
#define SOC_SOCP_ENC_CD_DBG1_cd_last_flag_END    (16)
#define SOC_SOCP_ENC_CD_DBG1_cd_idle_START       (17)
#define SOC_SOCP_ENC_CD_DBG1_cd_idle_END         (17)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_IBUF_DBG0_UNION
 结构说明  : ENC_IBUF_DBG0 寄存器结构定义。地址偏移量:0x0E08，初值:0x00000080，宽度:32
 寄存器说明: 编码源端debug寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ibuf_fsm_state : 3;  /* bit[0-2]  : ibuf_ctrl状态机状态
                                                          3'b000：IDLE
                                                          3'b001：REQ_HEAD
                                                          3'b011：XFER_HEAD
                                                          3;b100：REQ_DATA
                                                          3'b110：XFER_DATA
                                                          3'b111：WAIT_IFIFO_RDY */
        unsigned int  reserved_0     : 1;  /* bit[3]    : 保留 */
        unsigned int  eop            : 1;  /* bit[4]    : 包尾指示信号
                                                          1：包尾
                                                          0：非包尾 */
        unsigned int  sop            : 1;  /* bit[5]    : 包头指示信号
                                                          1：包头
                                                          0：非包头 */
        unsigned int  fifo_full      : 1;  /* bit[6]    : fifo满指示
                                                          1：fifo满
                                                          0：fifo不满 */
        unsigned int  fifo_empty     : 1;  /* bit[7]    : fifo空指示
                                                          1：fifo空
                                                          0：fifo不空 */
        unsigned int  reserved_1     : 8;  /* bit[8-15] : 保留 */
        unsigned int  remain_blk_cnt : 16; /* bit[16-31]: 数据包待传输的字节数 */
    } reg;
} SOC_SOCP_ENC_IBUF_DBG0_UNION;
#endif
#define SOC_SOCP_ENC_IBUF_DBG0_ibuf_fsm_state_START  (0)
#define SOC_SOCP_ENC_IBUF_DBG0_ibuf_fsm_state_END    (2)
#define SOC_SOCP_ENC_IBUF_DBG0_eop_START             (4)
#define SOC_SOCP_ENC_IBUF_DBG0_eop_END               (4)
#define SOC_SOCP_ENC_IBUF_DBG0_sop_START             (5)
#define SOC_SOCP_ENC_IBUF_DBG0_sop_END               (5)
#define SOC_SOCP_ENC_IBUF_DBG0_fifo_full_START       (6)
#define SOC_SOCP_ENC_IBUF_DBG0_fifo_full_END         (6)
#define SOC_SOCP_ENC_IBUF_DBG0_fifo_empty_START      (7)
#define SOC_SOCP_ENC_IBUF_DBG0_fifo_empty_END        (7)
#define SOC_SOCP_ENC_IBUF_DBG0_remain_blk_cnt_START  (16)
#define SOC_SOCP_ENC_IBUF_DBG0_remain_blk_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_IBUF_DBG1_UNION
 结构说明  : ENC_IBUF_DBG1 寄存器结构定义。地址偏移量:0x0E0C，初值:0x00000000，宽度:32
 寄存器说明: 编码源端debug寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dma_addr : 32; /* bit[0-31]: 当前传输的地址 */
    } reg;
} SOC_SOCP_ENC_IBUF_DBG1_UNION;
#endif
#define SOC_SOCP_ENC_IBUF_DBG1_dma_addr_START  (0)
#define SOC_SOCP_ENC_IBUF_DBG1_dma_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_IBUF_DBG2_UNION
 结构说明  : ENC_IBUF_DBG2 寄存器结构定义。地址偏移量:0x0E10，初值:0x00000000，宽度:32
 寄存器说明: 编码源端debug寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ibuf_fifo_dout_lo : 32; /* bit[0-31]: FIFO读出的当前数据bit[31:0] */
    } reg;
} SOC_SOCP_ENC_IBUF_DBG2_UNION;
#endif
#define SOC_SOCP_ENC_IBUF_DBG2_ibuf_fifo_dout_lo_START  (0)
#define SOC_SOCP_ENC_IBUF_DBG2_ibuf_fifo_dout_lo_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_IBUF_DBG3_UNION
 结构说明  : ENC_IBUF_DBG3 寄存器结构定义。地址偏移量:0x0E14，初值:0x00000000，宽度:32
 寄存器说明: 编码源端debug寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ibuf_fifo_dout_hi : 32; /* bit[0-31]: FIFO读出的当前数据bit[63:32] */
    } reg;
} SOC_SOCP_ENC_IBUF_DBG3_UNION;
#endif
#define SOC_SOCP_ENC_IBUF_DBG3_ibuf_fifo_dout_hi_START  (0)
#define SOC_SOCP_ENC_IBUF_DBG3_ibuf_fifo_dout_hi_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_OBUF_DBG0_UNION
 结构说明  : ENC_OBUF_DBG0 寄存器结构定义。地址偏移量:0x0E18，初值:0x00000080，宽度:32
 寄存器说明: 编码目的端debug寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  obuf_fsm_state : 3;  /* bit[0-2]  : obuf_ctrl状态机状态
                                                          3'b000:IDLE
                                                          3'b001:REQ_DATA
                                                          3'b011:XFER_DATA
                                                          3'b111:REQ_FLAG
                                                          3'b101:XFER_FLAG
                                                          3'b010:WAIT_DMA_RDY
                                                          3'b100:WAIT_RESP_DONE */
        unsigned int  reserved_0     : 2;  /* bit[3-4]  : 保留 */
        unsigned int  fifo_afull     : 1;  /* bit[5]    : fifo将满指示
                                                          1：fifo将满
                                                          0：fifo非将满 */
        unsigned int  fifo_full      : 1;  /* bit[6]    : fifo满指示
                                                          1：fifo满
                                                          0：fifo不满 */
        unsigned int  fifo_empty     : 1;  /* bit[7]    : fifo空指示
                                                          1：fifo空
                                                          0：fifo不空 */
        unsigned int  eop            : 1;  /* bit[8]    : 包尾指示信号
                                                          1：包尾
                                                          0：非包尾 */
        unsigned int  sop            : 1;  /* bit[9]    : 包头指示信号
                                                          1：包尾
                                                          0：非包尾 */
        unsigned int  reserved_1     : 6;  /* bit[10-15]: 保留 */
        unsigned int  pkt_in_cnt     : 16; /* bit[16-31]: 编码引擎输出的字节数 */
    } reg;
} SOC_SOCP_ENC_OBUF_DBG0_UNION;
#endif
#define SOC_SOCP_ENC_OBUF_DBG0_obuf_fsm_state_START  (0)
#define SOC_SOCP_ENC_OBUF_DBG0_obuf_fsm_state_END    (2)
#define SOC_SOCP_ENC_OBUF_DBG0_fifo_afull_START      (5)
#define SOC_SOCP_ENC_OBUF_DBG0_fifo_afull_END        (5)
#define SOC_SOCP_ENC_OBUF_DBG0_fifo_full_START       (6)
#define SOC_SOCP_ENC_OBUF_DBG0_fifo_full_END         (6)
#define SOC_SOCP_ENC_OBUF_DBG0_fifo_empty_START      (7)
#define SOC_SOCP_ENC_OBUF_DBG0_fifo_empty_END        (7)
#define SOC_SOCP_ENC_OBUF_DBG0_eop_START             (8)
#define SOC_SOCP_ENC_OBUF_DBG0_eop_END               (8)
#define SOC_SOCP_ENC_OBUF_DBG0_sop_START             (9)
#define SOC_SOCP_ENC_OBUF_DBG0_sop_END               (9)
#define SOC_SOCP_ENC_OBUF_DBG0_pkt_in_cnt_START      (16)
#define SOC_SOCP_ENC_OBUF_DBG0_pkt_in_cnt_END        (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_OBUF_DBG1_UNION
 结构说明  : ENC_OBUF_DBG1 寄存器结构定义。地址偏移量:0x0E1C，初值:0x00000000，宽度:32
 寄存器说明: 编码目的端debug寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pkt_out_cnt : 16; /* bit[0-15] : 当前输出的字节数 */
        unsigned int  reserved    : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_SOCP_ENC_OBUF_DBG1_UNION;
#endif
#define SOC_SOCP_ENC_OBUF_DBG1_pkt_out_cnt_START  (0)
#define SOC_SOCP_ENC_OBUF_DBG1_pkt_out_cnt_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_OBUF_DBG2_UNION
 结构说明  : ENC_OBUF_DBG2 寄存器结构定义。地址偏移量:0x0E20，初值:0x00000000，宽度:32
 寄存器说明: 编码目的端debug寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nxt_dma_addr : 32; /* bit[0-31]: 输出地址 */
    } reg;
} SOC_SOCP_ENC_OBUF_DBG2_UNION;
#endif
#define SOC_SOCP_ENC_OBUF_DBG2_nxt_dma_addr_START  (0)
#define SOC_SOCP_ENC_OBUF_DBG2_nxt_dma_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_ENC_OBUF_DBG3_UNION
 结构说明  : ENC_OBUF_DBG3 寄存器结构定义。地址偏移量:0x0E24，初值:0x00000000，宽度:32
 寄存器说明: 编码目的端debug寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  obuf_fifo_dout_lo : 32; /* bit[0-31]: FIFO读出的当前数据bit[31:0] */
    } reg;
} SOC_SOCP_ENC_OBUF_DBG3_UNION;
#endif
#define SOC_SOCP_ENC_OBUF_DBG3_obuf_fifo_dout_lo_START  (0)
#define SOC_SOCP_ENC_OBUF_DBG3_obuf_fifo_dout_lo_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_IBUF_DBG0_UNION
 结构说明  : DEC_IBUF_DBG0 寄存器结构定义。地址偏移量:0x0E28，初值:0x00000080，宽度:32
 寄存器说明: 解码源端debug寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ibuf_fsm_state : 3;  /* bit[0-2]  : ibuf_ctrl状态机状态
                                                          3'b000：IDLE
                                                          3;b100：REQ_DATA
                                                          3'b110：XFER_DATA
                                                          3'b010：WAIT_IFIFO_RDY
                                                          3'b011：WAIT_BUF_RDY */
        unsigned int  reserved_0     : 1;  /* bit[3]    : 保留 */
        unsigned int  eop            : 1;  /* bit[4]    : 包尾指示信号
                                                          1：包尾
                                                          0：非包尾 */
        unsigned int  sop            : 1;  /* bit[5]    : 包头指示信号
                                                          1：包头
                                                          0：非包头 */
        unsigned int  fifo_full      : 1;  /* bit[6]    : fifo满指示
                                                          1：fifo满
                                                          0：fifo不满 */
        unsigned int  fifo_empty     : 1;  /* bit[7]    : fifo空指示
                                                          1：fifo空
                                                          0：fifo不空 */
        unsigned int  reserved_1     : 8;  /* bit[8-15] : 保留 */
        unsigned int  remain_blk_cnt : 16; /* bit[16-31]: 数据包待传输的字节数 */
    } reg;
} SOC_SOCP_DEC_IBUF_DBG0_UNION;
#endif
#define SOC_SOCP_DEC_IBUF_DBG0_ibuf_fsm_state_START  (0)
#define SOC_SOCP_DEC_IBUF_DBG0_ibuf_fsm_state_END    (2)
#define SOC_SOCP_DEC_IBUF_DBG0_eop_START             (4)
#define SOC_SOCP_DEC_IBUF_DBG0_eop_END               (4)
#define SOC_SOCP_DEC_IBUF_DBG0_sop_START             (5)
#define SOC_SOCP_DEC_IBUF_DBG0_sop_END               (5)
#define SOC_SOCP_DEC_IBUF_DBG0_fifo_full_START       (6)
#define SOC_SOCP_DEC_IBUF_DBG0_fifo_full_END         (6)
#define SOC_SOCP_DEC_IBUF_DBG0_fifo_empty_START      (7)
#define SOC_SOCP_DEC_IBUF_DBG0_fifo_empty_END        (7)
#define SOC_SOCP_DEC_IBUF_DBG0_remain_blk_cnt_START  (16)
#define SOC_SOCP_DEC_IBUF_DBG0_remain_blk_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_IBUF_DBG1_UNION
 结构说明  : DEC_IBUF_DBG1 寄存器结构定义。地址偏移量:0x0E2C，初值:0x00000000，宽度:32
 寄存器说明: 解码源端debug寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dma_addr : 32; /* bit[0-31]: 当前传输的地址 */
    } reg;
} SOC_SOCP_DEC_IBUF_DBG1_UNION;
#endif
#define SOC_SOCP_DEC_IBUF_DBG1_dma_addr_START  (0)
#define SOC_SOCP_DEC_IBUF_DBG1_dma_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_IBUF_DBG2_UNION
 结构说明  : DEC_IBUF_DBG2 寄存器结构定义。地址偏移量:0x0E30，初值:0x00000000，宽度:32
 寄存器说明: 解码源端debug寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ibuf_fifo_dout_lo : 32; /* bit[0-31]: FIFO读出的当前数据bit[31:0] */
    } reg;
} SOC_SOCP_DEC_IBUF_DBG2_UNION;
#endif
#define SOC_SOCP_DEC_IBUF_DBG2_ibuf_fifo_dout_lo_START  (0)
#define SOC_SOCP_DEC_IBUF_DBG2_ibuf_fifo_dout_lo_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_IBUF_DBG3_UNION
 结构说明  : DEC_IBUF_DBG3 寄存器结构定义。地址偏移量:0x0E34，初值:0x00000000，宽度:32
 寄存器说明: 解码源端debug寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ibuf_fifo_dout_hi : 32; /* bit[0-31]: FIFO读出的当前数据bit[63:32] */
    } reg;
} SOC_SOCP_DEC_IBUF_DBG3_UNION;
#endif
#define SOC_SOCP_DEC_IBUF_DBG3_ibuf_fifo_dout_hi_START  (0)
#define SOC_SOCP_DEC_IBUF_DBG3_ibuf_fifo_dout_hi_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_OBUF_DBG0_UNION
 结构说明  : DEC_OBUF_DBG0 寄存器结构定义。地址偏移量:0x0E38，初值:0x00000080，宽度:32
 寄存器说明: 解码目的端debug寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  obuf_fsm_state : 3;  /* bit[0-2]  : obuf_ctrl状态机状态
                                                          3'b000:IDLE
                                                          3'b001:REQ_DATA
                                                          3'b011:XFER_DATA
                                                          3'b111:REQ_FLAG
                                                          3'b101:XFER_FLAG
                                                          3'b010:WAIT_DMA_RDY
                                                          3'b100:WAIT_RESP_DONE */
        unsigned int  reserved_0     : 2;  /* bit[3-4]  : 保留 */
        unsigned int  fifo_afull     : 1;  /* bit[5]    : fifo将满指示
                                                          1：fifo将满
                                                          0：fifo非将满 */
        unsigned int  fifo_full      : 1;  /* bit[6]    : fifo满指示
                                                          1：fifo满
                                                          0：fifo不满 */
        unsigned int  fifo_empty     : 1;  /* bit[7]    : fifo空指示
                                                          1：fifo空
                                                          0：fifo不空 */
        unsigned int  eop            : 1;  /* bit[8]    : 包尾指示信号
                                                          1：包尾
                                                          0：非包尾 */
        unsigned int  sop            : 1;  /* bit[9]    : 包头指示信号
                                                          1：包尾
                                                          0：非包尾 */
        unsigned int  reserved_1     : 6;  /* bit[10-15]: 保留 */
        unsigned int  pkt_in_cnt     : 16; /* bit[16-31]: 编码引擎输出的字节数 */
    } reg;
} SOC_SOCP_DEC_OBUF_DBG0_UNION;
#endif
#define SOC_SOCP_DEC_OBUF_DBG0_obuf_fsm_state_START  (0)
#define SOC_SOCP_DEC_OBUF_DBG0_obuf_fsm_state_END    (2)
#define SOC_SOCP_DEC_OBUF_DBG0_fifo_afull_START      (5)
#define SOC_SOCP_DEC_OBUF_DBG0_fifo_afull_END        (5)
#define SOC_SOCP_DEC_OBUF_DBG0_fifo_full_START       (6)
#define SOC_SOCP_DEC_OBUF_DBG0_fifo_full_END         (6)
#define SOC_SOCP_DEC_OBUF_DBG0_fifo_empty_START      (7)
#define SOC_SOCP_DEC_OBUF_DBG0_fifo_empty_END        (7)
#define SOC_SOCP_DEC_OBUF_DBG0_eop_START             (8)
#define SOC_SOCP_DEC_OBUF_DBG0_eop_END               (8)
#define SOC_SOCP_DEC_OBUF_DBG0_sop_START             (9)
#define SOC_SOCP_DEC_OBUF_DBG0_sop_END               (9)
#define SOC_SOCP_DEC_OBUF_DBG0_pkt_in_cnt_START      (16)
#define SOC_SOCP_DEC_OBUF_DBG0_pkt_in_cnt_END        (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_OBUF_DBG1_UNION
 结构说明  : DEC_OBUF_DBG1 寄存器结构定义。地址偏移量:0x0E3C，初值:0x00000000，宽度:32
 寄存器说明: 解码目的端debug寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pkt_out_cnt : 16; /* bit[0-15] : 当前输出的字节数 */
        unsigned int  reserved    : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_SOCP_DEC_OBUF_DBG1_UNION;
#endif
#define SOC_SOCP_DEC_OBUF_DBG1_pkt_out_cnt_START  (0)
#define SOC_SOCP_DEC_OBUF_DBG1_pkt_out_cnt_END    (15)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_OBUF_DBG2_UNION
 结构说明  : DEC_OBUF_DBG2 寄存器结构定义。地址偏移量:0x0E40，初值:0x00000000，宽度:32
 寄存器说明: 解码目的端debug寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nxt_dma_addr : 32; /* bit[0-31]: dma输出地址 */
    } reg;
} SOC_SOCP_DEC_OBUF_DBG2_UNION;
#endif
#define SOC_SOCP_DEC_OBUF_DBG2_nxt_dma_addr_START  (0)
#define SOC_SOCP_DEC_OBUF_DBG2_nxt_dma_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_SOCP_DEC_OBUF_DBG3_UNION
 结构说明  : DEC_OBUF_DBG3 寄存器结构定义。地址偏移量:0x0E44，初值:0x00000000，宽度:32
 寄存器说明: 解码目的端debug寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  obuf_fifo_dout_lo : 32; /* bit[0-31]: FIFO读出的当前数据bit[31:0] */
    } reg;
} SOC_SOCP_DEC_OBUF_DBG3_UNION;
#endif
#define SOC_SOCP_DEC_OBUF_DBG3_obuf_fifo_dout_lo_START  (0)
#define SOC_SOCP_DEC_OBUF_DBG3_obuf_fifo_dout_lo_END    (31)






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

#endif /* end of soc_socp_interface.h */
