

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_LCIPHER_INTERFACE_H__
#define __SOC_LCIPHER_INTERFACE_H__

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
/* 寄存器说明：CIPHER模块控制寄存器
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_CTRL_UNION */
#define SOC_LCIPHER_CIPHER_CTRL_ADDR(base)            ((base) + (0x000))

/* 寄存器说明：CIPHER优先级控制寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_PRI_UNION */
#define SOC_LCIPHER_CIPHER_PRI_ADDR(base)             ((base) + (0x004))

/* 寄存器说明：总BD个数
   位域定义UNION结构:  SOC_LCIPHER_TOTAL_BD_NUM_UNION */
#define SOC_LCIPHER_TOTAL_BD_NUM_ADDR(base)           ((base) + (0x008))

/* 寄存器说明：非法BD个数
   位域定义UNION结构:  SOC_LCIPHER_INVALID_BD_NUM_UNION */
#define SOC_LCIPHER_INVALID_BD_NUM_ADDR(base)         ((base) + (0x00C))

/* 寄存器说明：dma配置寄存器
   位域定义UNION结构:  SOC_LCIPHER_DMA_CFG_UNION */
#define SOC_LCIPHER_DMA_CFG_ADDR(base)                ((base) + (0x014))

/* 寄存器说明：调试寄存器
   位域定义UNION结构:  SOC_LCIPHER_TEST_REG0_UNION */
#define SOC_LCIPHER_TEST_REG0_ADDR(base)              ((base) + (0x020))

/* 寄存器说明：调试寄存器
   位域定义UNION结构:  SOC_LCIPHER_TEST_REG1_UNION */
#define SOC_LCIPHER_TEST_REG1_ADDR(base)              ((base) + (0x024))

/* 寄存器说明：调试寄存器
   位域定义UNION结构:  SOC_LCIPHER_TEST_REG2_UNION */
#define SOC_LCIPHER_TEST_REG2_ADDR(base)              ((base) + (0x028))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug20_UNION */
#define SOC_LCIPHER_cipher_debug20_ADDR(base)         ((base) + (0x02C))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug0_UNION */
#define SOC_LCIPHER_cipher_debug0_ADDR(base)          ((base) + (0x030))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug1_UNION */
#define SOC_LCIPHER_cipher_debug1_ADDR(base)          ((base) + (0x034))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug2_UNION */
#define SOC_LCIPHER_cipher_debug2_ADDR(base)          ((base) + (0x038))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug3_UNION */
#define SOC_LCIPHER_cipher_debug3_ADDR(base)          ((base) + (0x03C))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug4_UNION */
#define SOC_LCIPHER_cipher_debug4_ADDR(base)          ((base) + (0x040))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug5_UNION */
#define SOC_LCIPHER_cipher_debug5_ADDR(base)          ((base) + (0x044))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug6_UNION */
#define SOC_LCIPHER_cipher_debug6_ADDR(base)          ((base) + (0x048))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug7_UNION */
#define SOC_LCIPHER_cipher_debug7_ADDR(base)          ((base) + (0x04C))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug8_UNION */
#define SOC_LCIPHER_cipher_debug8_ADDR(base)          ((base) + (0x050))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug9_UNION */
#define SOC_LCIPHER_cipher_debug9_ADDR(base)          ((base) + (0x054))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug10_UNION */
#define SOC_LCIPHER_cipher_debug10_ADDR(base)         ((base) + (0x058))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug11_UNION */
#define SOC_LCIPHER_cipher_debug11_ADDR(base)         ((base) + (0x05C))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug12_UNION */
#define SOC_LCIPHER_cipher_debug12_ADDR(base)         ((base) + (0x060))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug13_UNION */
#define SOC_LCIPHER_cipher_debug13_ADDR(base)         ((base) + (0x064))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug14_UNION */
#define SOC_LCIPHER_cipher_debug14_ADDR(base)         ((base) + (0x068))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug15_UNION */
#define SOC_LCIPHER_cipher_debug15_ADDR(base)         ((base) + (0x06C))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug16_UNION */
#define SOC_LCIPHER_cipher_debug16_ADDR(base)         ((base) + (0x070))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug17_UNION */
#define SOC_LCIPHER_cipher_debug17_ADDR(base)         ((base) + (0x074))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug18_UNION */
#define SOC_LCIPHER_cipher_debug18_ADDR(base)         ((base) + (0x078))

/* 寄存器说明：debug寄存器
   位域定义UNION结构:  SOC_LCIPHER_cipher_debug19_UNION */
#define SOC_LCIPHER_cipher_debug19_ADDR(base)         ((base) + (0x07C))

/* 寄存器说明：通道0软复位寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH0_SOFTRESET_UNION */
#define SOC_LCIPHER_CH0_SOFTRESET_ADDR(base)          ((base) + (0x80))

/* 寄存器说明：通道0使能寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH0_EN_UNION */
#define SOC_LCIPHER_CH0_EN_ADDR(base)                 ((base) + (0x84))

/* 寄存器说明：通道0配置寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH0_CONFIG_UNION */
#define SOC_LCIPHER_CH0_CONFIG_ADDR(base)             ((base) + (0x88))

/* 寄存器说明：通道0 BDQ基地址寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH0_BDQ_BADDR_UNION */
#define SOC_LCIPHER_CH0_BDQ_BADDR_ADDR(base)          ((base) + (0x8C))

/* 寄存器说明：通道0 BDQ大小寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH0_BDQ_SIZE_UNION */
#define SOC_LCIPHER_CH0_BDQ_SIZE_ADDR(base)           ((base) + (0x90))

/* 寄存器说明：通道0 BDQ读写指针寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH0_BDQ_PTR_UNION */
#define SOC_LCIPHER_CH0_BDQ_PTR_ADDR(base)            ((base) + (0x94))

/* 寄存器说明：通道0 BDQ读地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH0_BDQ_RADDR_UNION */
#define SOC_LCIPHER_CH0_BDQ_RADDR_ADDR(base)          ((base) + (0x98))

/* 寄存器说明：通道0 BDQ写地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH0_BDQ_WADDR_UNION */
#define SOC_LCIPHER_CH0_BDQ_WADDR_ADDR(base)          ((base) + (0x9C))

/* 寄存器说明：表示通道0处理的BD总数
   位域定义UNION结构:  SOC_LCIPHER_CH0_BD_NUM_UNION */
#define SOC_LCIPHER_CH0_BD_NUM_ADDR(base)             ((base) + (0xA0))

/* 寄存器说明：表示通道0处理的无效BD数
   位域定义UNION结构:  SOC_LCIPHER_CH0_INVALID_BD_NUM_UNION */
#define SOC_LCIPHER_CH0_INVALID_BD_NUM_ADDR(base)     ((base) + (0xA4))

/* 寄存器说明：通道1软复位寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH1_SOFTRESET_UNION */
#define SOC_LCIPHER_CH1_SOFTRESET_ADDR(base)          ((base) + (0x100))

/* 寄存器说明：通道1使能寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH1_EN_UNION */
#define SOC_LCIPHER_CH1_EN_ADDR(base)                 ((base) + (0x104))

/* 寄存器说明：通道1配置寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH1_CONFIG_UNION */
#define SOC_LCIPHER_CH1_CONFIG_ADDR(base)             ((base) + (0x108))

/* 寄存器说明：通道1 BDQ基地址寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH1_BDQ_BADDR_UNION */
#define SOC_LCIPHER_CH1_BDQ_BADDR_ADDR(base)          ((base) + (0x10C))

/* 寄存器说明：通道1 BDQ大小寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH1_BDQ_SIZE_UNION */
#define SOC_LCIPHER_CH1_BDQ_SIZE_ADDR(base)           ((base) + (0x110))

/* 寄存器说明：通道1 BDQ读写指针寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH1_BDQ_PTR_UNION */
#define SOC_LCIPHER_CH1_BDQ_PTR_ADDR(base)            ((base) + (0x114))

/* 寄存器说明：通道1 BDQ读地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH1_BDQ_RADDR_UNION */
#define SOC_LCIPHER_CH1_BDQ_RADDR_ADDR(base)          ((base) + (0x118))

/* 寄存器说明：通道1 BDQ写地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH1_BDQ_WADDR_UNION */
#define SOC_LCIPHER_CH1_BDQ_WADDR_ADDR(base)          ((base) + (0x11C))

/* 寄存器说明：表示通道1处理的BD总数
   位域定义UNION结构:  SOC_LCIPHER_CH1_BD_NUM_UNION */
#define SOC_LCIPHER_CH1_BD_NUM_ADDR(base)             ((base) + (0x120))

/* 寄存器说明：表示通道1处理的无效BD数
   位域定义UNION结构:  SOC_LCIPHER_CH1_INVALID_BD_NUM_UNION */
#define SOC_LCIPHER_CH1_INVALID_BD_NUM_ADDR(base)     ((base) + (0x124))

/* 寄存器说明：通道2软复位寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH2_SOFTRESET_UNION */
#define SOC_LCIPHER_CH2_SOFTRESET_ADDR(base)          ((base) + (0x180))

/* 寄存器说明：通道2使能寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH2_EN_UNION */
#define SOC_LCIPHER_CH2_EN_ADDR(base)                 ((base) + (0x184))

/* 寄存器说明：通道2配置寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH2_CONFIG_UNION */
#define SOC_LCIPHER_CH2_CONFIG_ADDR(base)             ((base) + (0x188))

/* 寄存器说明：通道2 BDQ基地址寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH2_BDQ_BADDR_UNION */
#define SOC_LCIPHER_CH2_BDQ_BADDR_ADDR(base)          ((base) + (0x18C))

/* 寄存器说明：通道2 BDQ大小寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH2_BDQ_SIZE_UNION */
#define SOC_LCIPHER_CH2_BDQ_SIZE_ADDR(base)           ((base) + (0x190))

/* 寄存器说明：通道2 BDQ读写指针寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH2_BDQ_PTR_UNION */
#define SOC_LCIPHER_CH2_BDQ_PTR_ADDR(base)            ((base) + (0x194))

/* 寄存器说明：通道2 BDQ读地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH2_BDQ_RADDR_UNION */
#define SOC_LCIPHER_CH2_BDQ_RADDR_ADDR(base)          ((base) + (0x198))

/* 寄存器说明：通道2 BDQ写地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH2_BDQ_WADDR_UNION */
#define SOC_LCIPHER_CH2_BDQ_WADDR_ADDR(base)          ((base) + (0x19C))

/* 寄存器说明：表示通道1处理的BD总数
   位域定义UNION结构:  SOC_LCIPHER_CH2_BD_NUM_UNION */
#define SOC_LCIPHER_CH2_BD_NUM_ADDR(base)             ((base) + (0x1A0))

/* 寄存器说明：表示通道1处理的无效BD数
   位域定义UNION结构:  SOC_LCIPHER_CH2_INVALID_BD_NUM_UNION */
#define SOC_LCIPHER_CH2_INVALID_BD_NUM_ADDR(base)     ((base) + (0x1A4))

/* 寄存器说明：通道3软复位寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH3_SOFTRESET_UNION */
#define SOC_LCIPHER_CH3_SOFTRESET_ADDR(base)          ((base) + (0x200))

/* 寄存器说明：通道3使能寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH3_EN_UNION */
#define SOC_LCIPHER_CH3_EN_ADDR(base)                 ((base) + (0x204))

/* 寄存器说明：通道3配置寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH3_CONFIG_UNION */
#define SOC_LCIPHER_CH3_CONFIG_ADDR(base)             ((base) + (0x208))

/* 寄存器说明：通道3 BDQ基地址寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH3_BDQ_BADDR_UNION */
#define SOC_LCIPHER_CH3_BDQ_BADDR_ADDR(base)          ((base) + (0x20C))

/* 寄存器说明：通道3 BDQ大小寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH3_BDQ_SIZE_UNION */
#define SOC_LCIPHER_CH3_BDQ_SIZE_ADDR(base)           ((base) + (0x210))

/* 寄存器说明：通道3 BDQ读写指针寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH3_BDQ_PTR_UNION */
#define SOC_LCIPHER_CH3_BDQ_PTR_ADDR(base)            ((base) + (0x214))

/* 寄存器说明：通道3 BDQ读地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH3_BDQ_RADDR_UNION */
#define SOC_LCIPHER_CH3_BDQ_RADDR_ADDR(base)          ((base) + (0x218))

/* 寄存器说明：通道3 BDQ写地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH3_BDQ_WADDR_UNION */
#define SOC_LCIPHER_CH3_BDQ_WADDR_ADDR(base)          ((base) + (0x21C))

/* 寄存器说明：表示通道3处理的BD总数
   位域定义UNION结构:  SOC_LCIPHER_CH3_BD_NUM_UNION */
#define SOC_LCIPHER_CH3_BD_NUM_ADDR(base)             ((base) + (0x220))

/* 寄存器说明：表示通道3处理的无效BD数
   位域定义UNION结构:  SOC_LCIPHER_CH3_INVALID_BD_NUM_UNION */
#define SOC_LCIPHER_CH3_INVALID_BD_NUM_ADDR(base)     ((base) + (0x224))

/* 寄存器说明：通道4软复位寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH4_SOFTRESET_UNION */
#define SOC_LCIPHER_CH4_SOFTRESET_ADDR(base)          ((base) + (0x280))

/* 寄存器说明：通道4使能寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH4_EN_UNION */
#define SOC_LCIPHER_CH4_EN_ADDR(base)                 ((base) + (0x284))

/* 寄存器说明：通道4配置寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH4_CONFIG_UNION */
#define SOC_LCIPHER_CH4_CONFIG_ADDR(base)             ((base) + (0x288))

/* 寄存器说明：通道4 BDQ基地址寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH4_BDQ_BADDR_UNION */
#define SOC_LCIPHER_CH4_BDQ_BADDR_ADDR(base)          ((base) + (0x28C))

/* 寄存器说明：通道4 BDQ大小寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH4_BDQ_SIZE_UNION */
#define SOC_LCIPHER_CH4_BDQ_SIZE_ADDR(base)           ((base) + (0x290))

/* 寄存器说明：通道4 BDQ读写指针寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH4_BDQ_PTR_UNION */
#define SOC_LCIPHER_CH4_BDQ_PTR_ADDR(base)            ((base) + (0x294))

/* 寄存器说明：通道4 BDQ读地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH4_BDQ_RADDR_UNION */
#define SOC_LCIPHER_CH4_BDQ_RADDR_ADDR(base)          ((base) + (0x298))

/* 寄存器说明：通道4 BDQ写地址寄存器。
   位域定义UNION结构:  SOC_LCIPHER_CH4_BDQ_WADDR_UNION */
#define SOC_LCIPHER_CH4_BDQ_WADDR_ADDR(base)          ((base) + (0x29C))

/* 寄存器说明：表示通道4处理的BD总数
   位域定义UNION结构:  SOC_LCIPHER_CH4_BD_NUM_UNION */
#define SOC_LCIPHER_CH4_BD_NUM_ADDR(base)             ((base) + (0x2A0))

/* 寄存器说明：表示通道4处理的无效BD数
   位域定义UNION结构:  SOC_LCIPHER_CH4_INVALID_BD_NUM_UNION */
#define SOC_LCIPHER_CH4_INVALID_BD_NUM_ADDR(base)     ((base) + (0x2A4))

/* 寄存器说明：中断包间隔寄存器
   位域定义UNION结构:  SOC_LCIPHER_CH_INT_PKT_INTERVAL_UNION */
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ADDR(base)    ((base) + (0x400))

/* 寄存器说明：cipher中断状态寄存器
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT_STATUS_UNION */
#define SOC_LCIPHER_CIPHER_INT_STATUS_ADDR(base)      ((base) + (0x404))

/* 寄存器说明：cipher中断寄存器0
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT0_STATUS_UNION */
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ADDR(base)     ((base) + (0x408))

/* 寄存器说明：cipher中断屏蔽寄存器0
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT0_MASK_UNION */
#define SOC_LCIPHER_CIPHER_INT0_MASK_ADDR(base)       ((base) + (0x40C))

/* 寄存器说明：cipher经过屏蔽之后的中断状态寄存器0
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT0_MSTATUS_UNION */
#define SOC_LCIPHER_CIPHER_INT0_MSTATUS_ADDR(base)    ((base) + (0x410))

/* 寄存器说明：cipher中断寄存器1
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT1_STATUS_UNION */
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ADDR(base)     ((base) + (0x414))

/* 寄存器说明：cipher中断屏蔽寄存器1
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT1_MASK_UNION */
#define SOC_LCIPHER_CIPHER_INT1_MASK_ADDR(base)       ((base) + (0x418))

/* 寄存器说明：cipher经过屏蔽之后的中断状态寄存器1
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT1_MSTATUS_UNION */
#define SOC_LCIPHER_CIPHER_INT1_MSTATUS_ADDR(base)    ((base) + (0x41C))

/* 寄存器说明：cipher中断寄存器2
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT2_STATUS_UNION */
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ADDR(base)     ((base) + (0x420))

/* 寄存器说明：cipher中断屏蔽寄存器2
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT2_MASK_UNION */
#define SOC_LCIPHER_CIPHER_INT2_MASK_ADDR(base)       ((base) + (0x424))

/* 寄存器说明：cipher经过屏蔽之后的中断状态寄存器2
   位域定义UNION结构:  SOC_LCIPHER_CIPHER_INT2_MSTATUS_UNION */
#define SOC_LCIPHER_CIPHER_INT2_MSTATUS_ADDR(base)    ((base) + (0x428))

/* 寄存器说明：通道0 RD队列基地址
   位域定义UNION结构:  SOC_LCIPHER_CH0_RDQ_BADDR_UNION */
#define SOC_LCIPHER_CH0_RDQ_BADDR_ADDR(base)          ((base) + (0x480))

/* 寄存器说明：通道0 RD队列大小
   位域定义UNION结构:  SOC_LCIPHER_CH0_RDQ_SIZE_UNION */
#define SOC_LCIPHER_CH0_RDQ_SIZE_ADDR(base)           ((base) + (0x484))

/* 寄存器说明：通道0 RD队列的读写指针
   位域定义UNION结构:  SOC_LCIPHER_CH0_RDQ_PTR_UNION */
#define SOC_LCIPHER_CH0_RDQ_PTR_ADDR(base)            ((base) + (0x488))

/* 寄存器说明：通道0 RD队列的读地址
   位域定义UNION结构:  SOC_LCIPHER_CH0_RDQ_RADDR_UNION */
#define SOC_LCIPHER_CH0_RDQ_RADDR_ADDR(base)          ((base) + (0x48C))

/* 寄存器说明：通道0 RD队列的写地址
   位域定义UNION结构:  SOC_LCIPHER_CH0_RDQ_WADDR_UNION */
#define SOC_LCIPHER_CH0_RDQ_WADDR_ADDR(base)          ((base) + (0x490))

/* 寄存器说明：通道1 RD队列基地址
   位域定义UNION结构:  SOC_LCIPHER_CH1_RDQ_BADDR_UNION */
#define SOC_LCIPHER_CH1_RDQ_BADDR_ADDR(base)          ((base) + (0x500))

/* 寄存器说明：通道1 RD队列大小
   位域定义UNION结构:  SOC_LCIPHER_CH1_RDQ_SIZE_UNION */
#define SOC_LCIPHER_CH1_RDQ_SIZE_ADDR(base)           ((base) + (0x504))

/* 寄存器说明：通道1 RD队列的读写指针
   位域定义UNION结构:  SOC_LCIPHER_CH1_RDQ_PTR_UNION */
#define SOC_LCIPHER_CH1_RDQ_PTR_ADDR(base)            ((base) + (0x508))

/* 寄存器说明：通道1 RD队列的读地址
   位域定义UNION结构:  SOC_LCIPHER_CH1_RDQ_RADDR_UNION */
#define SOC_LCIPHER_CH1_RDQ_RADDR_ADDR(base)          ((base) + (0x50C))

/* 寄存器说明：通道1 RD队列的写地址
   位域定义UNION结构:  SOC_LCIPHER_CH1_RDQ_WADDR_UNION */
#define SOC_LCIPHER_CH1_RDQ_WADDR_ADDR(base)          ((base) + (0x510))

/* 寄存器说明：通道2 RD队列基地址
   位域定义UNION结构:  SOC_LCIPHER_CH2_RDQ_BADDR_UNION */
#define SOC_LCIPHER_CH2_RDQ_BADDR_ADDR(base)          ((base) + (0x580))

/* 寄存器说明：通道2 RD队列大小
   位域定义UNION结构:  SOC_LCIPHER_CH2_RDQ_SIZE_UNION */
#define SOC_LCIPHER_CH2_RDQ_SIZE_ADDR(base)           ((base) + (0x584))

/* 寄存器说明：通道2 RD队列的读写指针
   位域定义UNION结构:  SOC_LCIPHER_CH2_RDQ_PTR_UNION */
#define SOC_LCIPHER_CH2_RDQ_PTR_ADDR(base)            ((base) + (0x588))

/* 寄存器说明：通道2 RD队列的读地址
   位域定义UNION结构:  SOC_LCIPHER_CH2_RDQ_RADDR_UNION */
#define SOC_LCIPHER_CH2_RDQ_RADDR_ADDR(base)          ((base) + (0x58C))

/* 寄存器说明：通道2 RD队列的写地址
   位域定义UNION结构:  SOC_LCIPHER_CH2_RDQ_WADDR_UNION */
#define SOC_LCIPHER_CH2_RDQ_WADDR_ADDR(base)          ((base) + (0x590))

/* 寄存器说明：通道3 RD队列基地址
   位域定义UNION结构:  SOC_LCIPHER_CH3_RDQ_BADDR_UNION */
#define SOC_LCIPHER_CH3_RDQ_BADDR_ADDR(base)          ((base) + (0x600))

/* 寄存器说明：通道3 RD队列大小
   位域定义UNION结构:  SOC_LCIPHER_CH3_RDQ_SIZE_UNION */
#define SOC_LCIPHER_CH3_RDQ_SIZE_ADDR(base)           ((base) + (0x604))

/* 寄存器说明：通道3 RD队列的读写指针
   位域定义UNION结构:  SOC_LCIPHER_CH3_RDQ_PTR_UNION */
#define SOC_LCIPHER_CH3_RDQ_PTR_ADDR(base)            ((base) + (0x608))

/* 寄存器说明：通道3 RD队列的读地址
   位域定义UNION结构:  SOC_LCIPHER_CH3_RDQ_RADDR_UNION */
#define SOC_LCIPHER_CH3_RDQ_RADDR_ADDR(base)          ((base) + (0x60C))

/* 寄存器说明：通道3 RD队列的写地址
   位域定义UNION结构:  SOC_LCIPHER_CH3_RDQ_WADDR_UNION */
#define SOC_LCIPHER_CH3_RDQ_WADDR_ADDR(base)          ((base) + (0x610))

/* 寄存器说明：通道4 RD队列基地址
   位域定义UNION结构:  SOC_LCIPHER_CH4_RDQ_BADDR_UNION */
#define SOC_LCIPHER_CH4_RDQ_BADDR_ADDR(base)          ((base) + (0x680))

/* 寄存器说明：通道4 RD队列大小
   位域定义UNION结构:  SOC_LCIPHER_CH4_RDQ_SIZE_UNION */
#define SOC_LCIPHER_CH4_RDQ_SIZE_ADDR(base)           ((base) + (0x684))

/* 寄存器说明：通道4 RD队列的读写指针
   位域定义UNION结构:  SOC_LCIPHER_CH4_RDQ_PTR_UNION */
#define SOC_LCIPHER_CH4_RDQ_PTR_ADDR(base)            ((base) + (0x688))

/* 寄存器说明：通道4 RD队列的读地址
   位域定义UNION结构:  SOC_LCIPHER_CH4_RDQ_RADDR_UNION */
#define SOC_LCIPHER_CH4_RDQ_RADDR_ADDR(base)          ((base) + (0x68C))

/* 寄存器说明：通道4 RD队列的写地址
   位域定义UNION结构:  SOC_LCIPHER_CH4_RDQ_WADDR_UNION */
#define SOC_LCIPHER_CH4_RDQ_WADDR_ADDR(base)          ((base) + (0x690))





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
 结构名    : SOC_LCIPHER_CIPHER_CTRL_UNION
 结构说明  : CIPHER_CTRL 寄存器结构定义。地址偏移量:0x000，初值:0x0，宽度:32
 寄存器说明: CIPHER模块控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cipher_srst  : 1;  /* bit[0]   : cipher模块复位。
                                                       写1复位cipher模块，若此时cipher占有总线，不能立即执行复位，直到cipher释放总线以后才开始复位，复位结束此寄存器自动归0。 */
        unsigned int  cipher_cg_en : 1;  /* bit[1]   : cipher自动时钟门控使能 */
        unsigned int  reserved     : 29; /* bit[2-30]: 保留。 */
        unsigned int  cipher_busy  : 1;  /* bit[31]  : cipher模块忙标志。
                                                       当5个命令通道的任何一个控制寄存器的忙标志为1，这个标志就为1。 */
    } reg;
} SOC_LCIPHER_CIPHER_CTRL_UNION;
#endif
#define SOC_LCIPHER_CIPHER_CTRL_cipher_srst_START   (0)
#define SOC_LCIPHER_CIPHER_CTRL_cipher_srst_END     (0)
#define SOC_LCIPHER_CIPHER_CTRL_cipher_cg_en_START  (1)
#define SOC_LCIPHER_CIPHER_CTRL_cipher_cg_en_END    (1)
#define SOC_LCIPHER_CIPHER_CTRL_cipher_busy_START   (31)
#define SOC_LCIPHER_CIPHER_CTRL_cipher_busy_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_PRI_UNION
 结构说明  : CIPHER_PRI 寄存器结构定义。地址偏移量:0x004，初值:0x0，宽度:32
 寄存器说明: CIPHER优先级控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  idata_threshold : 28; /* bit[0-27] : 通道流量控制阈值，单位是字节，计数对象是输入数据。
                                                           如果其它低优先级通道有命令请求，当前通道输入数据流量开始计数，如果输入流量超过阈值或操作结束，当前通道暂停，优先级降为最低，其它通道开始工作。通道2和通道3支持输入流量控制功能。 */
        unsigned int  ch_pri          : 1;  /* bit[28]   : 通道0和通道1优先级。
                                                           0：通道0为最高优先级；
                                                           1：通道1为最高优先级；
                                                           通道2、通道3的优先级总是低于通道0和通道1，且根据流量控制寄存器来执行。 */
        unsigned int  reserved        : 3;  /* bit[29-31]:  */
    } reg;
} SOC_LCIPHER_CIPHER_PRI_UNION;
#endif
#define SOC_LCIPHER_CIPHER_PRI_idata_threshold_START  (0)
#define SOC_LCIPHER_CIPHER_PRI_idata_threshold_END    (27)
#define SOC_LCIPHER_CIPHER_PRI_ch_pri_START           (28)
#define SOC_LCIPHER_CIPHER_PRI_ch_pri_END             (28)


/*****************************************************************************
 结构名    : SOC_LCIPHER_TOTAL_BD_NUM_UNION
 结构说明  : TOTAL_BD_NUM 寄存器结构定义。地址偏移量:0x008，初值:0x0，宽度:32
 寄存器说明: 总BD个数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  total_bd_num : 32; /* bit[0-31]: 表示cipher已经处理的BD总数（5个通道的总和）
                                                       只读，
                                                       可写清0 */
    } reg;
} SOC_LCIPHER_TOTAL_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_TOTAL_BD_NUM_total_bd_num_START  (0)
#define SOC_LCIPHER_TOTAL_BD_NUM_total_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_INVALID_BD_NUM_UNION
 结构说明  : INVALID_BD_NUM 寄存器结构定义。地址偏移量:0x00C，初值:0x0，宽度:32
 寄存器说明: 非法BD个数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  invalid_bd_num : 32; /* bit[0-31]: 表示cipher收到的无效BD总数（5个通道的总和）
                                                         只读，
                                                         可写清0 */
    } reg;
} SOC_LCIPHER_INVALID_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_INVALID_BD_NUM_invalid_bd_num_START  (0)
#define SOC_LCIPHER_INVALID_BD_NUM_invalid_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_DMA_CFG_UNION
 结构说明  : DMA_CFG 寄存器结构定义。地址偏移量:0x014，初值:0x2713469A，宽度:32
 寄存器说明: dma配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_id    : 3;  /* bit[0-2]  : RD回写通道的ID */
        unsigned int  obuf_id  : 3;  /* bit[3-5]  : OBUF写通道的ID */
        unsigned int  ocd_id   : 3;  /* bit[6-8]  : OCD读通道的ID */
        unsigned int  icd_id   : 3;  /* bit[9-11] : ICD读通道的ID */
        unsigned int  ibuf_id  : 3;  /* bit[12-14]: IBUF读通道的ID */
        unsigned int  reserved_0: 1;  /* bit[15]   : 保留 */
        unsigned int  rd_pri   : 3;  /* bit[16-18]: RD回写通道的优先级 */
        unsigned int  obuf_pri : 3;  /* bit[19-21]: OBUF写通道的优先级 */
        unsigned int  ocd_pri  : 3;  /* bit[22-24]: OCD读通道的优先级 */
        unsigned int  icd_pri  : 3;  /* bit[25-27]: ICD读通道的优先级 */
        unsigned int  ibuf_pri : 3;  /* bit[28-30]: IBUF读通道的优先级 */
        unsigned int  reserved_1: 1;  /* bit[31]   : 保留 */
    } reg;
} SOC_LCIPHER_DMA_CFG_UNION;
#endif
#define SOC_LCIPHER_DMA_CFG_rd_id_START     (0)
#define SOC_LCIPHER_DMA_CFG_rd_id_END       (2)
#define SOC_LCIPHER_DMA_CFG_obuf_id_START   (3)
#define SOC_LCIPHER_DMA_CFG_obuf_id_END     (5)
#define SOC_LCIPHER_DMA_CFG_ocd_id_START    (6)
#define SOC_LCIPHER_DMA_CFG_ocd_id_END      (8)
#define SOC_LCIPHER_DMA_CFG_icd_id_START    (9)
#define SOC_LCIPHER_DMA_CFG_icd_id_END      (11)
#define SOC_LCIPHER_DMA_CFG_ibuf_id_START   (12)
#define SOC_LCIPHER_DMA_CFG_ibuf_id_END     (14)
#define SOC_LCIPHER_DMA_CFG_rd_pri_START    (16)
#define SOC_LCIPHER_DMA_CFG_rd_pri_END      (18)
#define SOC_LCIPHER_DMA_CFG_obuf_pri_START  (19)
#define SOC_LCIPHER_DMA_CFG_obuf_pri_END    (21)
#define SOC_LCIPHER_DMA_CFG_ocd_pri_START   (22)
#define SOC_LCIPHER_DMA_CFG_ocd_pri_END     (24)
#define SOC_LCIPHER_DMA_CFG_icd_pri_START   (25)
#define SOC_LCIPHER_DMA_CFG_icd_pri_END     (27)
#define SOC_LCIPHER_DMA_CFG_ibuf_pri_START  (28)
#define SOC_LCIPHER_DMA_CFG_ibuf_pri_END    (30)


/*****************************************************************************
 结构名    : SOC_LCIPHER_TEST_REG0_UNION
 结构说明  : TEST_REG0 寄存器结构定义。地址偏移量:0x020，初值:0x0C9B0C9B，宽度:32
 寄存器说明: 调试寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  spram_mem_ctrl  : 16; /* bit[0-15] : cipher内部spram的控制信号 */
        unsigned int  atpram_mem_ctrl : 16; /* bit[16-31]: cipher内部atpram的控制信号  */
    } reg;
} SOC_LCIPHER_TEST_REG0_UNION;
#endif
#define SOC_LCIPHER_TEST_REG0_spram_mem_ctrl_START   (0)
#define SOC_LCIPHER_TEST_REG0_spram_mem_ctrl_END     (15)
#define SOC_LCIPHER_TEST_REG0_atpram_mem_ctrl_START  (16)
#define SOC_LCIPHER_TEST_REG0_atpram_mem_ctrl_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_TEST_REG1_UNION
 结构说明  : TEST_REG1 寄存器结构定义。地址偏移量:0x024，初值:0x4FE，宽度:32
 寄存器说明: 调试寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  aes_key_cov     : 1;  /* bit[0]    : aes运算时，是否需要颠倒key的字序
                                                           1：表示需要颠倒字序；
                                                           0：表示不需要颠倒字序。 */
        unsigned int  snow3g_key_cov  : 1;  /* bit[1]    : snow3g运算时，是否需要颠倒key的字序
                                                           1：表示需要颠倒字序；
                                                           0：表示不需要颠倒字序。 */
        unsigned int  cipher_dma_ctrl : 6;  /* bit[2-7]  : 对于dma的控制信号：
                                                           [5:4]：表示GM写方向的outstanding深度
                                                           [3:2]：表示GM读方向的outstanding深度
                                                           [1]： 1表示读outstanding使能，0表示读outstanding不使能 ；
                                                           [0]： 1表示写outstanding使能，0表示写outstanding不使能。 */
        unsigned int  zuc_key_cov     : 1;  /* bit[8]    : zuc运算时，是否需要颠倒key的字序
                                                           1：表示需要颠倒字序；
                                                           0：表示不需要颠倒字序。 */
        unsigned int  aes_iv_cov      : 1;  /* bit[9]    : aes运算时，是否需要颠倒iv的字序
                                                           1：表示需要颠倒字序；
                                                           0：表示不需要颠倒字序。 */
        unsigned int  snow3g_iv_cov   : 1;  /* bit[10]   : snow3g运算时，是否需要颠倒iv的字序
                                                           1：表示需要颠倒字序；
                                                           0：表示不需要颠倒字序。 */
        unsigned int  zuc_iv_cov      : 1;  /* bit[11]   : zuc运算时，是否需要颠倒iv的字序
                                                           1：表示需要颠倒字序；
                                                           0：表示不需要颠倒字序。 */
        unsigned int  reserved        : 20; /* bit[12-31]: 保留 */
    } reg;
} SOC_LCIPHER_TEST_REG1_UNION;
#endif
#define SOC_LCIPHER_TEST_REG1_aes_key_cov_START      (0)
#define SOC_LCIPHER_TEST_REG1_aes_key_cov_END        (0)
#define SOC_LCIPHER_TEST_REG1_snow3g_key_cov_START   (1)
#define SOC_LCIPHER_TEST_REG1_snow3g_key_cov_END     (1)
#define SOC_LCIPHER_TEST_REG1_cipher_dma_ctrl_START  (2)
#define SOC_LCIPHER_TEST_REG1_cipher_dma_ctrl_END    (7)
#define SOC_LCIPHER_TEST_REG1_zuc_key_cov_START      (8)
#define SOC_LCIPHER_TEST_REG1_zuc_key_cov_END        (8)
#define SOC_LCIPHER_TEST_REG1_aes_iv_cov_START       (9)
#define SOC_LCIPHER_TEST_REG1_aes_iv_cov_END         (9)
#define SOC_LCIPHER_TEST_REG1_snow3g_iv_cov_START    (10)
#define SOC_LCIPHER_TEST_REG1_snow3g_iv_cov_END      (10)
#define SOC_LCIPHER_TEST_REG1_zuc_iv_cov_START       (11)
#define SOC_LCIPHER_TEST_REG1_zuc_iv_cov_END         (11)


/*****************************************************************************
 结构名    : SOC_LCIPHER_TEST_REG2_UNION
 结构说明  : TEST_REG2 寄存器结构定义。地址偏移量:0x028，初值:0x0，宽度:32
 寄存器说明: 调试寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 调试寄存器。 */
    } reg;
} SOC_LCIPHER_TEST_REG2_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug20_UNION
 结构说明  : cipher_debug20 寄存器结构定义。地址偏移量:0x02C，初值:0x1F0000，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dma_rd_gnt         : 1;  /* bit[0]    : rd的dma请求被响应 */
        unsigned int  dma_ocd_gnt        : 1;  /* bit[1]    : ocd的dma请求被响应 */
        unsigned int  dma_icd_gnt        : 1;  /* bit[2]    : icd的dma请求被响应 */
        unsigned int  dma_obuf_gnt       : 1;  /* bit[3]    : obuf的dma请求被响应 */
        unsigned int  dma_ibuf_gnt       : 1;  /* bit[4]    : ibuf的dma请求被响应 */
        unsigned int  reserved_0         : 3;  /* bit[5-7]  :  */
        unsigned int  dma_rd_req         : 1;  /* bit[8]    : rd进行dma请求 */
        unsigned int  dma_ocd_req        : 1;  /* bit[9]    : ocd进行dma请求 */
        unsigned int  dma_icd_req        : 1;  /* bit[10]   : icd进行dma请求 */
        unsigned int  dma_obuf_req       : 1;  /* bit[11]   : obuf进行dma请求 */
        unsigned int  dma_ibuf_req       : 1;  /* bit[12]   : ibuf进行dma请求 */
        unsigned int  reserved_1         : 3;  /* bit[13-15]:  */
        unsigned int  dma_rd_idle        : 1;  /* bit[16]   : rd没有进行dma操作，现在处于空闲状态 */
        unsigned int  dma_ocd_idle       : 1;  /* bit[17]   : ocd没有进行dma操作，现在处于空闲状态 */
        unsigned int  dma_icd_idle       : 1;  /* bit[18]   : icd没有进行dma操作，现在处于空闲状态 */
        unsigned int  dma_obuf_idle      : 1;  /* bit[19]   : obuf没有进行dma操作，现在处于空闲状态 */
        unsigned int  dma_ibuf_idle      : 1;  /* bit[20]   : ibuf没有进行dma操作，现在处于空闲状态 */
        unsigned int  reserved_2         : 3;  /* bit[21-23]:  */
        unsigned int  dma_rd_trans_end   : 1;  /* bit[24]   : rd每次dma交易的最后一个数据 */
        unsigned int  dma_ocd_last_read  : 1;  /* bit[25]   : ocd每次dma交易的最后一个数据 */
        unsigned int  dma_icd_last_read  : 1;  /* bit[26]   : icd每次dma交易的最后一个数据 */
        unsigned int  dma_obuf_trans_end : 1;  /* bit[27]   : obuf每次dma交易的最后一个数据 */
        unsigned int  dma_ibuf_last_read : 1;  /* bit[28]   : ibuf每次dma交易的最后一个数据 */
        unsigned int  reserved_3         : 3;  /* bit[29-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug20_UNION;
#endif
#define SOC_LCIPHER_cipher_debug20_dma_rd_gnt_START          (0)
#define SOC_LCIPHER_cipher_debug20_dma_rd_gnt_END            (0)
#define SOC_LCIPHER_cipher_debug20_dma_ocd_gnt_START         (1)
#define SOC_LCIPHER_cipher_debug20_dma_ocd_gnt_END           (1)
#define SOC_LCIPHER_cipher_debug20_dma_icd_gnt_START         (2)
#define SOC_LCIPHER_cipher_debug20_dma_icd_gnt_END           (2)
#define SOC_LCIPHER_cipher_debug20_dma_obuf_gnt_START        (3)
#define SOC_LCIPHER_cipher_debug20_dma_obuf_gnt_END          (3)
#define SOC_LCIPHER_cipher_debug20_dma_ibuf_gnt_START        (4)
#define SOC_LCIPHER_cipher_debug20_dma_ibuf_gnt_END          (4)
#define SOC_LCIPHER_cipher_debug20_dma_rd_req_START          (8)
#define SOC_LCIPHER_cipher_debug20_dma_rd_req_END            (8)
#define SOC_LCIPHER_cipher_debug20_dma_ocd_req_START         (9)
#define SOC_LCIPHER_cipher_debug20_dma_ocd_req_END           (9)
#define SOC_LCIPHER_cipher_debug20_dma_icd_req_START         (10)
#define SOC_LCIPHER_cipher_debug20_dma_icd_req_END           (10)
#define SOC_LCIPHER_cipher_debug20_dma_obuf_req_START        (11)
#define SOC_LCIPHER_cipher_debug20_dma_obuf_req_END          (11)
#define SOC_LCIPHER_cipher_debug20_dma_ibuf_req_START        (12)
#define SOC_LCIPHER_cipher_debug20_dma_ibuf_req_END          (12)
#define SOC_LCIPHER_cipher_debug20_dma_rd_idle_START         (16)
#define SOC_LCIPHER_cipher_debug20_dma_rd_idle_END           (16)
#define SOC_LCIPHER_cipher_debug20_dma_ocd_idle_START        (17)
#define SOC_LCIPHER_cipher_debug20_dma_ocd_idle_END          (17)
#define SOC_LCIPHER_cipher_debug20_dma_icd_idle_START        (18)
#define SOC_LCIPHER_cipher_debug20_dma_icd_idle_END          (18)
#define SOC_LCIPHER_cipher_debug20_dma_obuf_idle_START       (19)
#define SOC_LCIPHER_cipher_debug20_dma_obuf_idle_END         (19)
#define SOC_LCIPHER_cipher_debug20_dma_ibuf_idle_START       (20)
#define SOC_LCIPHER_cipher_debug20_dma_ibuf_idle_END         (20)
#define SOC_LCIPHER_cipher_debug20_dma_rd_trans_end_START    (24)
#define SOC_LCIPHER_cipher_debug20_dma_rd_trans_end_END      (24)
#define SOC_LCIPHER_cipher_debug20_dma_ocd_last_read_START   (25)
#define SOC_LCIPHER_cipher_debug20_dma_ocd_last_read_END     (25)
#define SOC_LCIPHER_cipher_debug20_dma_icd_last_read_START   (26)
#define SOC_LCIPHER_cipher_debug20_dma_icd_last_read_END     (26)
#define SOC_LCIPHER_cipher_debug20_dma_obuf_trans_end_START  (27)
#define SOC_LCIPHER_cipher_debug20_dma_obuf_trans_end_END    (27)
#define SOC_LCIPHER_cipher_debug20_dma_ibuf_last_read_START  (28)
#define SOC_LCIPHER_cipher_debug20_dma_ibuf_last_read_END    (28)


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug0_UNION
 结构说明  : cipher_debug0 寄存器结构定义。地址偏移量:0x030，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug0_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug1_UNION
 结构说明  : cipher_debug1 寄存器结构定义。地址偏移量:0x034，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug1_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug2_UNION
 结构说明  : cipher_debug2 寄存器结构定义。地址偏移量:0x038，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug2_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug3_UNION
 结构说明  : cipher_debug3 寄存器结构定义。地址偏移量:0x03C，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug3_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug4_UNION
 结构说明  : cipher_debug4 寄存器结构定义。地址偏移量:0x040，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug4_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug5_UNION
 结构说明  : cipher_debug5 寄存器结构定义。地址偏移量:0x044，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug5_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug6_UNION
 结构说明  : cipher_debug6 寄存器结构定义。地址偏移量:0x048，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug6_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug7_UNION
 结构说明  : cipher_debug7 寄存器结构定义。地址偏移量:0x04C，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug7_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug8_UNION
 结构说明  : cipher_debug8 寄存器结构定义。地址偏移量:0x050，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug8_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug9_UNION
 结构说明  : cipher_debug9 寄存器结构定义。地址偏移量:0x054，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug9_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug10_UNION
 结构说明  : cipher_debug10 寄存器结构定义。地址偏移量:0x058，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug10_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug11_UNION
 结构说明  : cipher_debug11 寄存器结构定义。地址偏移量:0x05C，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug11_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug12_UNION
 结构说明  : cipher_debug12 寄存器结构定义。地址偏移量:0x060，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug12_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug13_UNION
 结构说明  : cipher_debug13 寄存器结构定义。地址偏移量:0x064，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug13_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug14_UNION
 结构说明  : cipher_debug14 寄存器结构定义。地址偏移量:0x068，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug14_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug15_UNION
 结构说明  : cipher_debug15 寄存器结构定义。地址偏移量:0x06C，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug15_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug16_UNION
 结构说明  : cipher_debug16 寄存器结构定义。地址偏移量:0x070，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug16_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug17_UNION
 结构说明  : cipher_debug17 寄存器结构定义。地址偏移量:0x074，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug17_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug18_UNION
 结构说明  : cipher_debug18 寄存器结构定义。地址偏移量:0x078，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug18_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_cipher_debug19_UNION
 结构说明  : cipher_debug19 寄存器结构定义。地址偏移量:0x07C，初值:0x0，宽度:32
 寄存器说明: debug寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 保留 */
    } reg;
} SOC_LCIPHER_cipher_debug19_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_SOFTRESET_UNION
 结构说明  : CH0_SOFTRESET 寄存器结构定义。地址偏移量:0x80，初值:0x0，宽度:32
 寄存器说明: 通道0软复位寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_srst      : 1;  /* bit[0]   : 写1复位通道，若此时通道占有总线，不能立即执行复位，直到本通道释放总线以后才开始复位，复位结束此寄存器自动归0。 */
        unsigned int  ch_bdq_clear : 1;  /* bit[1]   : 写1请求复位BD对列，若此时通道正busy，则等到当前BD处理完毕后，清空BD对列，同时该位自动归零。 */
        unsigned int  ch_rdq_clear : 1;  /* bit[2]   : 写1请求复位RD对列，若此时通道正busy，则等到当前BD处理完毕后，清空RD对列，同时该位自动归零。 */
        unsigned int  reserved     : 29; /* bit[3-31]: 域说明。 */
    } reg;
} SOC_LCIPHER_CH0_SOFTRESET_UNION;
#endif
#define SOC_LCIPHER_CH0_SOFTRESET_ch_srst_START       (0)
#define SOC_LCIPHER_CH0_SOFTRESET_ch_srst_END         (0)
#define SOC_LCIPHER_CH0_SOFTRESET_ch_bdq_clear_START  (1)
#define SOC_LCIPHER_CH0_SOFTRESET_ch_bdq_clear_END    (1)
#define SOC_LCIPHER_CH0_SOFTRESET_ch_rdq_clear_START  (2)
#define SOC_LCIPHER_CH0_SOFTRESET_ch_rdq_clear_END    (2)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_EN_UNION
 结构说明  : CH0_EN 寄存器结构定义。地址偏移量:0x84，初值:0x0，宽度:32
 寄存器说明: 通道0使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_en    : 1;  /* bit[0]   : 1表示通道使能，0表示通道禁止。 */
        unsigned int  reserved : 29; /* bit[1-29]:  */
        unsigned int  pack_en  : 1;  /* bit[30]  : 此寄存器初始值为1，当为1时，表示本次组包任务完成，可以开始下一次，否则表示本次组包任务正在进行。与配合软件配合过程如下：
                                                   当cipher配合进行上行组包时，首先查询此寄存器，若为1，表示可以开始组包，软件将此bit置0，且软件把本次组包的最后一个BD中的gen_bd_int置为有效，当cipher完成这最后一个BD任务后，把此寄存器置为1，通知软件可以开始下一次组包任务。 */
        unsigned int  ch_busy  : 1;  /* bit[31]  : 1表示通道工作，0表示通道暂停。 */
    } reg;
} SOC_LCIPHER_CH0_EN_UNION;
#endif
#define SOC_LCIPHER_CH0_EN_ch_en_START     (0)
#define SOC_LCIPHER_CH0_EN_ch_en_END       (0)
#define SOC_LCIPHER_CH0_EN_pack_en_START   (30)
#define SOC_LCIPHER_CH0_EN_pack_en_END     (30)
#define SOC_LCIPHER_CH0_EN_ch_busy_START   (31)
#define SOC_LCIPHER_CH0_EN_ch_busy_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_CONFIG_UNION
 结构说明  : CH0_CONFIG 寄存器结构定义。地址偏移量:0x88，初值:0x0，宽度:32
 寄存器说明: 通道0配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  iv_sel     : 1;  /* bit[0]   : 本通道完整性参数、IV随BD还是都由芯片自动产生。
                                                     0：完整性参数、IV由芯片根据参数自动产生；
                                                     1：完整性参数、IV随包或随BD。 */
        unsigned int  iv_num     : 1;  /* bit[1]   : 本通道BD中含有的IV个数。只有当iv_sel为1时有意义。
                                                     0：一个IV，此时BD长度固定是；
                                                     1：两个IV，其中一个IV可以是无效IV，但仍需占用内存。 */
        unsigned int  rdq_ctrl   : 2;  /* bit[2-3] : RDQ对列维护方式。当RDQ满且有需要回写的RD时，Cipher等待直到RDQ不满；
                                                     00：不回写RD；
                                                     01：每个BD都对应回写RD。
                                                     others：RD是否回写由BD配置决定，只在BD中的gen_rd_en有效时回写RD。 */
        unsigned int  reserved_0 : 1;  /* bit[4]   : 保留。 */
        unsigned int  ufield_len : 2;  /* bit[5-6] : BD中userfield字段的长度，单位是32bit。 */
        unsigned int  reserved_1 : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH0_CONFIG_UNION;
#endif
#define SOC_LCIPHER_CH0_CONFIG_iv_sel_START      (0)
#define SOC_LCIPHER_CH0_CONFIG_iv_sel_END        (0)
#define SOC_LCIPHER_CH0_CONFIG_iv_num_START      (1)
#define SOC_LCIPHER_CH0_CONFIG_iv_num_END        (1)
#define SOC_LCIPHER_CH0_CONFIG_rdq_ctrl_START    (2)
#define SOC_LCIPHER_CH0_CONFIG_rdq_ctrl_END      (3)
#define SOC_LCIPHER_CH0_CONFIG_ufield_len_START  (5)
#define SOC_LCIPHER_CH0_CONFIG_ufield_len_END    (6)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_BDQ_BADDR_UNION
 结构说明  : CH0_BDQ_BADDR 寄存器结构定义。地址偏移量:0x8C，初值:0x0，宽度:32
 寄存器说明: 通道0 BDQ基地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 BD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道0 BD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH0_BDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH0_BDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH0_BDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH0_BDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH0_BDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_BDQ_SIZE_UNION
 结构说明  : CH0_BDQ_SIZE 寄存器结构定义。地址偏移量:0x90，初值:0x0，宽度:32
 寄存器说明: 通道0 BDQ大小寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 BD Queue大小，以描述符个数为单位。
                                                    0：表示队列大小为1
                                                    1023：表示队列大小为1024 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH0_BDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH0_BDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH0_BDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_BDQ_PTR_UNION
 结构说明  : CH0_BDQ_PTR 寄存器结构定义。地址偏移量:0x94，初值:0x0，宽度:32
 寄存器说明: 通道0 BDQ读写指针寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr         : 10; /* bit[0-9]  : BD队列写指针。可被软复位 */
        unsigned int  reserved_0   : 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr         : 10; /* bit[16-25]: BD队列读指针。可被软复位。 */
        unsigned int  reserved_1   : 4;  /* bit[26-29]: 保留 */
        unsigned int  ch_bdq_empty : 1;  /* bit[30]   : BD队列空 */
        unsigned int  ch_bdq_full  : 1;  /* bit[31]   : BD队列满 */
    } reg;
} SOC_LCIPHER_CH0_BDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH0_BDQ_PTR_wptr_START          (0)
#define SOC_LCIPHER_CH0_BDQ_PTR_wptr_END            (9)
#define SOC_LCIPHER_CH0_BDQ_PTR_rptr_START          (16)
#define SOC_LCIPHER_CH0_BDQ_PTR_rptr_END            (25)
#define SOC_LCIPHER_CH0_BDQ_PTR_ch_bdq_empty_START  (30)
#define SOC_LCIPHER_CH0_BDQ_PTR_ch_bdq_empty_END    (30)
#define SOC_LCIPHER_CH0_BDQ_PTR_ch_bdq_full_START   (31)
#define SOC_LCIPHER_CH0_BDQ_PTR_ch_bdq_full_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_BDQ_RADDR_UNION
 结构说明  : CH0_BDQ_RADDR 寄存器结构定义。地址偏移量:0x98，初值:0x0，宽度:32
 寄存器说明: 通道0 BDQ读地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道0 BD队列读地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH0_BDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH0_BDQ_RADDR_addr_START  (0)
#define SOC_LCIPHER_CH0_BDQ_RADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_BDQ_WADDR_UNION
 结构说明  : CH0_BDQ_WADDR 寄存器结构定义。地址偏移量:0x9C，初值:0x0，宽度:32
 寄存器说明: 通道0 BDQ写地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道0 BD队列写地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH0_BDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH0_BDQ_WADDR_addr_START  (0)
#define SOC_LCIPHER_CH0_BDQ_WADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_BD_NUM_UNION
 结构说明  : CH0_BD_NUM 寄存器结构定义。地址偏移量:0xA0，初值:0x0，宽度:32
 寄存器说明: 表示通道0处理的BD总数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch0_bd_num : 32; /* bit[0-31]: 表示通道0已经处理的BD总数
                                                     可以写清零 */
    } reg;
} SOC_LCIPHER_CH0_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH0_BD_NUM_ch0_bd_num_START  (0)
#define SOC_LCIPHER_CH0_BD_NUM_ch0_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_INVALID_BD_NUM_UNION
 结构说明  : CH0_INVALID_BD_NUM 寄存器结构定义。地址偏移量:0xA4，初值:0x0，宽度:32
 寄存器说明: 表示通道0处理的无效BD数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch0_invalid_bd_num : 32; /* bit[0-31]: 表示通道0已经处理的无效BD总数
                                                             可以写清零 */
    } reg;
} SOC_LCIPHER_CH0_INVALID_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH0_INVALID_BD_NUM_ch0_invalid_bd_num_START  (0)
#define SOC_LCIPHER_CH0_INVALID_BD_NUM_ch0_invalid_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_SOFTRESET_UNION
 结构说明  : CH1_SOFTRESET 寄存器结构定义。地址偏移量:0x100，初值:0x0，宽度:32
 寄存器说明: 通道1软复位寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_srst      : 1;  /* bit[0]   : 写1复位通道，若此时通道占有总线，不能立即执行复位，直到本通道释放总线以后才开始复位，复位结束此寄存器自动归0。 */
        unsigned int  ch_bdq_clear : 1;  /* bit[1]   : 写1请求复位BD对列，若此时通道正busy，则等到当前BD处理完毕后，清空BD对列，同时该位自动归零。 */
        unsigned int  ch_rdq_clear : 1;  /* bit[2]   : 写1请求复位RD对列，若此时通道正busy，则等到当前BD处理完毕后，清空RD对列，同时该位自动归零。 */
        unsigned int  reserved     : 29; /* bit[3-31]: 域说明。 */
    } reg;
} SOC_LCIPHER_CH1_SOFTRESET_UNION;
#endif
#define SOC_LCIPHER_CH1_SOFTRESET_ch_srst_START       (0)
#define SOC_LCIPHER_CH1_SOFTRESET_ch_srst_END         (0)
#define SOC_LCIPHER_CH1_SOFTRESET_ch_bdq_clear_START  (1)
#define SOC_LCIPHER_CH1_SOFTRESET_ch_bdq_clear_END    (1)
#define SOC_LCIPHER_CH1_SOFTRESET_ch_rdq_clear_START  (2)
#define SOC_LCIPHER_CH1_SOFTRESET_ch_rdq_clear_END    (2)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_EN_UNION
 结构说明  : CH1_EN 寄存器结构定义。地址偏移量:0x104，初值:0x0，宽度:32
 寄存器说明: 通道1使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_en    : 1;  /* bit[0]   : 1表示通道使能，0表示通道禁止。 */
        unsigned int  reserved : 29; /* bit[1-29]:  */
        unsigned int  pack_en  : 1;  /* bit[30]  : 此寄存器初始值为1，当为1时，表示本次组包任务完成，可以开始下一次，否则表示本次组包任务正在进行。与配合软件配合过程如下：
                                                   当cipher配合进行上行组包时，首先查询此寄存器，若为1，表示可以开始组包，软件将此bit置0，且软件把本次组包的最后一个BD中的gen_bd_int置为有效，当cipher完成这最后一个BD任务后，把此寄存器置为1，通知软件可以开始下一次组包任务。 */
        unsigned int  ch_busy  : 1;  /* bit[31]  : 1表示通道工作，0表示通道暂停。 */
    } reg;
} SOC_LCIPHER_CH1_EN_UNION;
#endif
#define SOC_LCIPHER_CH1_EN_ch_en_START     (0)
#define SOC_LCIPHER_CH1_EN_ch_en_END       (0)
#define SOC_LCIPHER_CH1_EN_pack_en_START   (30)
#define SOC_LCIPHER_CH1_EN_pack_en_END     (30)
#define SOC_LCIPHER_CH1_EN_ch_busy_START   (31)
#define SOC_LCIPHER_CH1_EN_ch_busy_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_CONFIG_UNION
 结构说明  : CH1_CONFIG 寄存器结构定义。地址偏移量:0x108，初值:0x0，宽度:32
 寄存器说明: 通道1配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  iv_sel     : 1;  /* bit[0]   : 本通道完整性参数、IV随BD还是都由芯片自动产生。
                                                     0：完整性参数、IV由芯片根据参数自动产生；
                                                     1：完整性参数、IV随包或随BD。 */
        unsigned int  iv_num     : 1;  /* bit[1]   : 本通道BD中含有的IV个数。只有当iv_sel为1时有意义。
                                                     0：一个IV，此时BD长度固定是；
                                                     1：两个IV，其中一个IV可以是无效IV，但仍需占用内存。 */
        unsigned int  rdq_ctrl   : 2;  /* bit[2-3] : RDQ对列维护方式。当RDQ满且有需要回写的RD时，Cipher等待直到RDQ不满；
                                                     00：不回写RD；
                                                     01：每个BD都对应回写RD。
                                                     others：RD是否回写由BD配置决定，只在BD中的gen_rd_en有效时回写RD。 */
        unsigned int  reserved_0 : 1;  /* bit[4]   : 保留。 */
        unsigned int  ufield_len : 2;  /* bit[5-6] : BD中userfield字段的长度，单位是32bit。 */
        unsigned int  reserved_1 : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH1_CONFIG_UNION;
#endif
#define SOC_LCIPHER_CH1_CONFIG_iv_sel_START      (0)
#define SOC_LCIPHER_CH1_CONFIG_iv_sel_END        (0)
#define SOC_LCIPHER_CH1_CONFIG_iv_num_START      (1)
#define SOC_LCIPHER_CH1_CONFIG_iv_num_END        (1)
#define SOC_LCIPHER_CH1_CONFIG_rdq_ctrl_START    (2)
#define SOC_LCIPHER_CH1_CONFIG_rdq_ctrl_END      (3)
#define SOC_LCIPHER_CH1_CONFIG_ufield_len_START  (5)
#define SOC_LCIPHER_CH1_CONFIG_ufield_len_END    (6)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_BDQ_BADDR_UNION
 结构说明  : CH1_BDQ_BADDR 寄存器结构定义。地址偏移量:0x10C，初值:0x0，宽度:32
 寄存器说明: 通道1 BDQ基地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 BD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道1 BD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH1_BDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH1_BDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH1_BDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH1_BDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH1_BDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_BDQ_SIZE_UNION
 结构说明  : CH1_BDQ_SIZE 寄存器结构定义。地址偏移量:0x110，初值:0x0，宽度:32
 寄存器说明: 通道1 BDQ大小寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 BD Queue大小，以描述符个数为单位。
                                                    0：表示队列大小为1
                                                    1023：表示队列大小为1024 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH1_BDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH1_BDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH1_BDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_BDQ_PTR_UNION
 结构说明  : CH1_BDQ_PTR 寄存器结构定义。地址偏移量:0x114，初值:0x0，宽度:32
 寄存器说明: 通道1 BDQ读写指针寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr          : 10; /* bit[0-9]  : BD队列写指针。可被软复位 */
        unsigned int  reserved_0    : 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr          : 10; /* bit[16-25]: BD队列读指针。可被软复位。 */
        unsigned int  reserved_1    : 4;  /* bit[26-29]: 保留 */
        unsigned int  ch1_bdq_empty : 1;  /* bit[30]   : B队列空 */
        unsigned int  ch1_bdq_full  : 1;  /* bit[31]   : BD队列满 */
    } reg;
} SOC_LCIPHER_CH1_BDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH1_BDQ_PTR_wptr_START           (0)
#define SOC_LCIPHER_CH1_BDQ_PTR_wptr_END             (9)
#define SOC_LCIPHER_CH1_BDQ_PTR_rptr_START           (16)
#define SOC_LCIPHER_CH1_BDQ_PTR_rptr_END             (25)
#define SOC_LCIPHER_CH1_BDQ_PTR_ch1_bdq_empty_START  (30)
#define SOC_LCIPHER_CH1_BDQ_PTR_ch1_bdq_empty_END    (30)
#define SOC_LCIPHER_CH1_BDQ_PTR_ch1_bdq_full_START   (31)
#define SOC_LCIPHER_CH1_BDQ_PTR_ch1_bdq_full_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_BDQ_RADDR_UNION
 结构说明  : CH1_BDQ_RADDR 寄存器结构定义。地址偏移量:0x118，初值:0x0，宽度:32
 寄存器说明: 通道1 BDQ读地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道1 BD队列读地址。可被软复位。 */
    } reg;
} SOC_LCIPHER_CH1_BDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH1_BDQ_RADDR_addr_START  (0)
#define SOC_LCIPHER_CH1_BDQ_RADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_BDQ_WADDR_UNION
 结构说明  : CH1_BDQ_WADDR 寄存器结构定义。地址偏移量:0x11C，初值:0x0，宽度:32
 寄存器说明: 通道1 BDQ写地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道1 BD队列写地址。可被软复位。 */
    } reg;
} SOC_LCIPHER_CH1_BDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH1_BDQ_WADDR_addr_START  (0)
#define SOC_LCIPHER_CH1_BDQ_WADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_BD_NUM_UNION
 结构说明  : CH1_BD_NUM 寄存器结构定义。地址偏移量:0x120，初值:0x0，宽度:32
 寄存器说明: 表示通道1处理的BD总数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_bd_num : 32; /* bit[0-31]: 表示通道1已经处理的BD总数
                                                     只读，
                                                     可以写清零 */
    } reg;
} SOC_LCIPHER_CH1_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH1_BD_NUM_ch1_bd_num_START  (0)
#define SOC_LCIPHER_CH1_BD_NUM_ch1_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_INVALID_BD_NUM_UNION
 结构说明  : CH1_INVALID_BD_NUM 寄存器结构定义。地址偏移量:0x124，初值:0x0，宽度:32
 寄存器说明: 表示通道1处理的无效BD数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch1_invalid_bd_num : 32; /* bit[0-31]: 表示通道1已经处理的无效BD总数
                                                             只读，
                                                             可以写清零 */
    } reg;
} SOC_LCIPHER_CH1_INVALID_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH1_INVALID_BD_NUM_ch1_invalid_bd_num_START  (0)
#define SOC_LCIPHER_CH1_INVALID_BD_NUM_ch1_invalid_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_SOFTRESET_UNION
 结构说明  : CH2_SOFTRESET 寄存器结构定义。地址偏移量:0x180，初值:0x0，宽度:32
 寄存器说明: 通道2软复位寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_srst      : 1;  /* bit[0]   : 写1复位通道，若此时通道占有总线，不能立即执行复位，直到本通道释放总线以后才开始复位，复位结束此寄存器自动归0。 */
        unsigned int  ch_bdq_clear : 1;  /* bit[1]   : 写1请求复位BD对列，若此时通道正busy，则等到当前BD处理完毕后，清空BD对列，同时该位自动归零。 */
        unsigned int  ch_rdq_clear : 1;  /* bit[2]   : 写1请求复位RD对列，若此时通道正busy，则等到当前BD处理完毕后，清空RD对列，同时该位自动归零。 */
        unsigned int  reserved     : 29; /* bit[3-31]: 域说明。 */
    } reg;
} SOC_LCIPHER_CH2_SOFTRESET_UNION;
#endif
#define SOC_LCIPHER_CH2_SOFTRESET_ch_srst_START       (0)
#define SOC_LCIPHER_CH2_SOFTRESET_ch_srst_END         (0)
#define SOC_LCIPHER_CH2_SOFTRESET_ch_bdq_clear_START  (1)
#define SOC_LCIPHER_CH2_SOFTRESET_ch_bdq_clear_END    (1)
#define SOC_LCIPHER_CH2_SOFTRESET_ch_rdq_clear_START  (2)
#define SOC_LCIPHER_CH2_SOFTRESET_ch_rdq_clear_END    (2)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_EN_UNION
 结构说明  : CH2_EN 寄存器结构定义。地址偏移量:0x184，初值:0x0，宽度:32
 寄存器说明: 通道2使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_en    : 1;  /* bit[0]   : 1表示通道使能，0表示通道禁止。 */
        unsigned int  reserved : 29; /* bit[1-29]:  */
        unsigned int  pack_en  : 1;  /* bit[30]  : 此寄存器初始值为1，当为1时，表示本次组包任务完成，可以开始下一次，否则表示本次组包任务正在进行。与配合软件配合过程如下：
                                                   当cipher配合进行上行组包时，首先查询此寄存器，若为1，表示可以开始组包，软件将此bit置0，且软件把本次组包的最后一个BD中的gen_bd_int置为有效，当cipher完成这最后一个BD任务后，把此寄存器置为1，通知软件可以开始下一次组包任务。 */
        unsigned int  ch_busy  : 1;  /* bit[31]  : 1表示通道工作，0表示通道暂停。 */
    } reg;
} SOC_LCIPHER_CH2_EN_UNION;
#endif
#define SOC_LCIPHER_CH2_EN_ch_en_START     (0)
#define SOC_LCIPHER_CH2_EN_ch_en_END       (0)
#define SOC_LCIPHER_CH2_EN_pack_en_START   (30)
#define SOC_LCIPHER_CH2_EN_pack_en_END     (30)
#define SOC_LCIPHER_CH2_EN_ch_busy_START   (31)
#define SOC_LCIPHER_CH2_EN_ch_busy_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_CONFIG_UNION
 结构说明  : CH2_CONFIG 寄存器结构定义。地址偏移量:0x188，初值:0x0，宽度:32
 寄存器说明: 通道2配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  iv_sel     : 1;  /* bit[0]   : 本通道完整性参数、IV随BD还是都由芯片自动产生。
                                                     0：完整性参数、IV由芯片根据参数自动产生；
                                                     1：完整性参数、IV随包或随BD。 */
        unsigned int  iv_num     : 1;  /* bit[1]   : 本通道BD中含有的IV个数。只有当iv_sel为1时有意义。
                                                     0：一个IV，此时BD长度固定是；
                                                     1：两个IV，其中一个IV可以是无效IV，但仍需占用内存。 */
        unsigned int  rdq_ctrl   : 2;  /* bit[2-3] : RDQ对列维护方式。当RDQ满且有需要回写的RD时，Cipher等待直到RDQ不满；
                                                     00：不回写RD；
                                                     01：每个BD都对应回写RD。
                                                     others：RD是否回写由BD配置决定，只在BD中的gen_rd_en有效时回写RD。 */
        unsigned int  reserved_0 : 1;  /* bit[4]   : 保留。 */
        unsigned int  ufield_len : 2;  /* bit[5-6] : BD中userfield字段的长度，单位是32bit。 */
        unsigned int  reserved_1 : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH2_CONFIG_UNION;
#endif
#define SOC_LCIPHER_CH2_CONFIG_iv_sel_START      (0)
#define SOC_LCIPHER_CH2_CONFIG_iv_sel_END        (0)
#define SOC_LCIPHER_CH2_CONFIG_iv_num_START      (1)
#define SOC_LCIPHER_CH2_CONFIG_iv_num_END        (1)
#define SOC_LCIPHER_CH2_CONFIG_rdq_ctrl_START    (2)
#define SOC_LCIPHER_CH2_CONFIG_rdq_ctrl_END      (3)
#define SOC_LCIPHER_CH2_CONFIG_ufield_len_START  (5)
#define SOC_LCIPHER_CH2_CONFIG_ufield_len_END    (6)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_BDQ_BADDR_UNION
 结构说明  : CH2_BDQ_BADDR 寄存器结构定义。地址偏移量:0x18C，初值:0x0，宽度:32
 寄存器说明: 通道2 BDQ基地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 BD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道2 BD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH2_BDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH2_BDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH2_BDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH2_BDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH2_BDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_BDQ_SIZE_UNION
 结构说明  : CH2_BDQ_SIZE 寄存器结构定义。地址偏移量:0x190，初值:0x0，宽度:32
 寄存器说明: 通道2 BDQ大小寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 BD Queue大小，以描述符个数为单位。
                                                    0：表示队列大小为1
                                                    1023：表示队列大小为1024 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH2_BDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH2_BDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH2_BDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_BDQ_PTR_UNION
 结构说明  : CH2_BDQ_PTR 寄存器结构定义。地址偏移量:0x194，初值:0x0，宽度:32
 寄存器说明: 通道2 BDQ读写指针寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr          : 10; /* bit[0-9]  : BD队列写指针。可被软复位 */
        unsigned int  reserved_0    : 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr          : 10; /* bit[16-25]: BD队列读指针。可被软复位。 */
        unsigned int  reserved_1    : 4;  /* bit[26-29]: 保留 */
        unsigned int  ch2_bdq_empty : 1;  /* bit[30]   : BD队列空 */
        unsigned int  ch2_bdq_full  : 1;  /* bit[31]   : BD队列满 */
    } reg;
} SOC_LCIPHER_CH2_BDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH2_BDQ_PTR_wptr_START           (0)
#define SOC_LCIPHER_CH2_BDQ_PTR_wptr_END             (9)
#define SOC_LCIPHER_CH2_BDQ_PTR_rptr_START           (16)
#define SOC_LCIPHER_CH2_BDQ_PTR_rptr_END             (25)
#define SOC_LCIPHER_CH2_BDQ_PTR_ch2_bdq_empty_START  (30)
#define SOC_LCIPHER_CH2_BDQ_PTR_ch2_bdq_empty_END    (30)
#define SOC_LCIPHER_CH2_BDQ_PTR_ch2_bdq_full_START   (31)
#define SOC_LCIPHER_CH2_BDQ_PTR_ch2_bdq_full_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_BDQ_RADDR_UNION
 结构说明  : CH2_BDQ_RADDR 寄存器结构定义。地址偏移量:0x198，初值:0x0，宽度:32
 寄存器说明: 通道2 BDQ读地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道2 BD队列读地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH2_BDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH2_BDQ_RADDR_addr_START  (0)
#define SOC_LCIPHER_CH2_BDQ_RADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_BDQ_WADDR_UNION
 结构说明  : CH2_BDQ_WADDR 寄存器结构定义。地址偏移量:0x19C，初值:0x0，宽度:32
 寄存器说明: 通道2 BDQ写地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道2 BD队列写地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH2_BDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH2_BDQ_WADDR_addr_START  (0)
#define SOC_LCIPHER_CH2_BDQ_WADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_BD_NUM_UNION
 结构说明  : CH2_BD_NUM 寄存器结构定义。地址偏移量:0x1A0，初值:0x0，宽度:32
 寄存器说明: 表示通道1处理的BD总数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_bd_num : 32; /* bit[0-31]: 表示通道2已经处理的BD总数
                                                     只读，
                                                     可以写清零 */
    } reg;
} SOC_LCIPHER_CH2_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH2_BD_NUM_ch2_bd_num_START  (0)
#define SOC_LCIPHER_CH2_BD_NUM_ch2_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_INVALID_BD_NUM_UNION
 结构说明  : CH2_INVALID_BD_NUM 寄存器结构定义。地址偏移量:0x1A4，初值:0x0，宽度:32
 寄存器说明: 表示通道1处理的无效BD数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch2_invalid_bd_num : 32; /* bit[0-31]: 表示通道2已经处理的无效BD总数
                                                             只读，
                                                             可以写清零 */
    } reg;
} SOC_LCIPHER_CH2_INVALID_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH2_INVALID_BD_NUM_ch2_invalid_bd_num_START  (0)
#define SOC_LCIPHER_CH2_INVALID_BD_NUM_ch2_invalid_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_SOFTRESET_UNION
 结构说明  : CH3_SOFTRESET 寄存器结构定义。地址偏移量:0x200，初值:0x0，宽度:32
 寄存器说明: 通道3软复位寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_srst      : 1;  /* bit[0]   : 写1复位通道，若此时通道占有总线，不能立即执行复位，直到本通道释放总线以后才开始复位，复位结束此寄存器自动归0。 */
        unsigned int  ch_bdq_clear : 1;  /* bit[1]   : 写1请求复位BD对列，若此时通道正busy，则等到当前BD处理完毕后，清空BD对列，同时该位自动归零。 */
        unsigned int  ch_rdq_clear : 1;  /* bit[2]   : 写1请求复位RD对列，若此时通道正busy，则等到当前BD处理完毕后，清空RD对列，同时该位自动归零。 */
        unsigned int  reserved     : 29; /* bit[3-31]: 域说明。 */
    } reg;
} SOC_LCIPHER_CH3_SOFTRESET_UNION;
#endif
#define SOC_LCIPHER_CH3_SOFTRESET_ch_srst_START       (0)
#define SOC_LCIPHER_CH3_SOFTRESET_ch_srst_END         (0)
#define SOC_LCIPHER_CH3_SOFTRESET_ch_bdq_clear_START  (1)
#define SOC_LCIPHER_CH3_SOFTRESET_ch_bdq_clear_END    (1)
#define SOC_LCIPHER_CH3_SOFTRESET_ch_rdq_clear_START  (2)
#define SOC_LCIPHER_CH3_SOFTRESET_ch_rdq_clear_END    (2)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_EN_UNION
 结构说明  : CH3_EN 寄存器结构定义。地址偏移量:0x204，初值:0x0，宽度:32
 寄存器说明: 通道3使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_en    : 1;  /* bit[0]   : 1表示通道使能，0表示通道禁止。 */
        unsigned int  reserved : 29; /* bit[1-29]:  */
        unsigned int  pack_en  : 1;  /* bit[30]  : 此寄存器初始值为1，当为1时，表示本次组包任务完成，可以开始下一次，否则表示本次组包任务正在进行。与配合软件配合过程如下：
                                                   当cipher配合进行上行组包时，首先查询此寄存器，若为1，表示可以开始组包，软件将此bit置0，且软件把本次组包的最后一个BD中的gen_bd_int置为有效，当cipher完成这最后一个BD任务后，把此寄存器置为1，通知软件可以开始下一次组包任务。 */
        unsigned int  ch_busy  : 1;  /* bit[31]  : 1表示通道工作，0表示通道暂停。 */
    } reg;
} SOC_LCIPHER_CH3_EN_UNION;
#endif
#define SOC_LCIPHER_CH3_EN_ch_en_START     (0)
#define SOC_LCIPHER_CH3_EN_ch_en_END       (0)
#define SOC_LCIPHER_CH3_EN_pack_en_START   (30)
#define SOC_LCIPHER_CH3_EN_pack_en_END     (30)
#define SOC_LCIPHER_CH3_EN_ch_busy_START   (31)
#define SOC_LCIPHER_CH3_EN_ch_busy_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_CONFIG_UNION
 结构说明  : CH3_CONFIG 寄存器结构定义。地址偏移量:0x208，初值:0x0，宽度:32
 寄存器说明: 通道3配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  iv_sel     : 1;  /* bit[0]   : 本通道完整性参数、IV随BD还是都由芯片自动产生。
                                                     0：完整性参数、IV由芯片根据参数自动产生；
                                                     1：完整性参数、IV随包或随BD。 */
        unsigned int  iv_num     : 1;  /* bit[1]   : 本通道BD中含有的IV个数。只有当iv_sel为1时有意义。
                                                     0：一个IV，此时BD长度固定是；
                                                     1：两个IV，其中一个IV可以是无效IV，但仍需占用内存。 */
        unsigned int  rdq_ctrl   : 2;  /* bit[2-3] : RDQ对列维护方式。当RDQ满且有需要回写的RD时，Cipher等待直到RDQ不满；
                                                     00：不回写RD；
                                                     01：每个BD都对应回写RD。
                                                     others：RD是否回写由BD配置决定，只在BD中的gen_rd_en有效时回写RD。 */
        unsigned int  reserved_0 : 1;  /* bit[4]   : 保留。 */
        unsigned int  ufield_len : 2;  /* bit[5-6] : BD中userfield字段的长度，单位是32bit。 */
        unsigned int  reserved_1 : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH3_CONFIG_UNION;
#endif
#define SOC_LCIPHER_CH3_CONFIG_iv_sel_START      (0)
#define SOC_LCIPHER_CH3_CONFIG_iv_sel_END        (0)
#define SOC_LCIPHER_CH3_CONFIG_iv_num_START      (1)
#define SOC_LCIPHER_CH3_CONFIG_iv_num_END        (1)
#define SOC_LCIPHER_CH3_CONFIG_rdq_ctrl_START    (2)
#define SOC_LCIPHER_CH3_CONFIG_rdq_ctrl_END      (3)
#define SOC_LCIPHER_CH3_CONFIG_ufield_len_START  (5)
#define SOC_LCIPHER_CH3_CONFIG_ufield_len_END    (6)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_BDQ_BADDR_UNION
 结构说明  : CH3_BDQ_BADDR 寄存器结构定义。地址偏移量:0x20C，初值:0x0，宽度:32
 寄存器说明: 通道3 BDQ基地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 BD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道3 BD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH3_BDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH3_BDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH3_BDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH3_BDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH3_BDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_BDQ_SIZE_UNION
 结构说明  : CH3_BDQ_SIZE 寄存器结构定义。地址偏移量:0x210，初值:0x0，宽度:32
 寄存器说明: 通道3 BDQ大小寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 BD Queue大小，以描述符个数为单位。
                                                    0：表示队列大小为1
                                                    1023：表示队列大小为1024 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH3_BDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH3_BDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH3_BDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_BDQ_PTR_UNION
 结构说明  : CH3_BDQ_PTR 寄存器结构定义。地址偏移量:0x214，初值:0x0，宽度:32
 寄存器说明: 通道3 BDQ读写指针寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr          : 10; /* bit[0-9]  : BD队列写指针。可被软复位 */
        unsigned int  reserved_0    : 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr          : 10; /* bit[16-25]: BD队列读指针。可被软复位。 */
        unsigned int  reserved_1    : 4;  /* bit[26-29]: 保留 */
        unsigned int  ch3_bdq_empty : 1;  /* bit[30]   : BD队列空 */
        unsigned int  ch3_bdq_full  : 1;  /* bit[31]   : BD队列满 */
    } reg;
} SOC_LCIPHER_CH3_BDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH3_BDQ_PTR_wptr_START           (0)
#define SOC_LCIPHER_CH3_BDQ_PTR_wptr_END             (9)
#define SOC_LCIPHER_CH3_BDQ_PTR_rptr_START           (16)
#define SOC_LCIPHER_CH3_BDQ_PTR_rptr_END             (25)
#define SOC_LCIPHER_CH3_BDQ_PTR_ch3_bdq_empty_START  (30)
#define SOC_LCIPHER_CH3_BDQ_PTR_ch3_bdq_empty_END    (30)
#define SOC_LCIPHER_CH3_BDQ_PTR_ch3_bdq_full_START   (31)
#define SOC_LCIPHER_CH3_BDQ_PTR_ch3_bdq_full_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_BDQ_RADDR_UNION
 结构说明  : CH3_BDQ_RADDR 寄存器结构定义。地址偏移量:0x218，初值:0x0，宽度:32
 寄存器说明: 通道3 BDQ读地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道3 BD队列读地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH3_BDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH3_BDQ_RADDR_addr_START  (0)
#define SOC_LCIPHER_CH3_BDQ_RADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_BDQ_WADDR_UNION
 结构说明  : CH3_BDQ_WADDR 寄存器结构定义。地址偏移量:0x21C，初值:0x0，宽度:32
 寄存器说明: 通道3 BDQ写地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道3 BD队列写地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH3_BDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH3_BDQ_WADDR_addr_START  (0)
#define SOC_LCIPHER_CH3_BDQ_WADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_BD_NUM_UNION
 结构说明  : CH3_BD_NUM 寄存器结构定义。地址偏移量:0x220，初值:0x0，宽度:32
 寄存器说明: 表示通道3处理的BD总数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch3_bd_num : 32; /* bit[0-31]: 表示通道3已经处理的BD总数
                                                     只读，
                                                     可以写清零 */
    } reg;
} SOC_LCIPHER_CH3_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH3_BD_NUM_ch3_bd_num_START  (0)
#define SOC_LCIPHER_CH3_BD_NUM_ch3_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_INVALID_BD_NUM_UNION
 结构说明  : CH3_INVALID_BD_NUM 寄存器结构定义。地址偏移量:0x224，初值:0x0，宽度:32
 寄存器说明: 表示通道3处理的无效BD数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch3_invalid_bd_num : 32; /* bit[0-31]: 表示通道3已经处理的无效BD总数
                                                             只读，
                                                             可以写清零 */
    } reg;
} SOC_LCIPHER_CH3_INVALID_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH3_INVALID_BD_NUM_ch3_invalid_bd_num_START  (0)
#define SOC_LCIPHER_CH3_INVALID_BD_NUM_ch3_invalid_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_SOFTRESET_UNION
 结构说明  : CH4_SOFTRESET 寄存器结构定义。地址偏移量:0x280，初值:0x0，宽度:32
 寄存器说明: 通道4软复位寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_srst      : 1;  /* bit[0]   : 写1复位通道，若此时通道占有总线，不能立即执行复位，直到本通道释放总线以后才开始复位，复位结束此寄存器自动归0。 */
        unsigned int  ch_bdq_clear : 1;  /* bit[1]   : 写1请求复位BD对列，若此时通道正busy，则等到当前BD处理完毕后，清空BD对列，同时该位自动归零。 */
        unsigned int  ch_rdq_clear : 1;  /* bit[2]   : 写1请求复位RD对列，若此时通道正busy，则等到当前BD处理完毕后，清空RD对列，同时该位自动归零。 */
        unsigned int  reserved     : 29; /* bit[3-31]: 域说明。 */
    } reg;
} SOC_LCIPHER_CH4_SOFTRESET_UNION;
#endif
#define SOC_LCIPHER_CH4_SOFTRESET_ch_srst_START       (0)
#define SOC_LCIPHER_CH4_SOFTRESET_ch_srst_END         (0)
#define SOC_LCIPHER_CH4_SOFTRESET_ch_bdq_clear_START  (1)
#define SOC_LCIPHER_CH4_SOFTRESET_ch_bdq_clear_END    (1)
#define SOC_LCIPHER_CH4_SOFTRESET_ch_rdq_clear_START  (2)
#define SOC_LCIPHER_CH4_SOFTRESET_ch_rdq_clear_END    (2)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_EN_UNION
 结构说明  : CH4_EN 寄存器结构定义。地址偏移量:0x284，初值:0x0，宽度:32
 寄存器说明: 通道4使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_en    : 1;  /* bit[0]   : 1表示通道使能，0表示通道禁止。 */
        unsigned int  reserved : 29; /* bit[1-29]:  */
        unsigned int  pack_en  : 1;  /* bit[30]  : 此寄存器初始值为1，当为1时，表示本次组包任务完成，可以开始下一次，否则表示本次组包任务正在进行。与配合软件配合过程如下：
                                                   当cipher配合进行上行组包时，首先查询此寄存器，若为1，表示可以开始组包，软件将此bit置0，且软件把本次组包的最后一个BD中的gen_bd_int置为有效，当cipher完成这最后一个BD任务后，把此寄存器置为1，通知软件可以开始下一次组包任务。 */
        unsigned int  ch_busy  : 1;  /* bit[31]  : 1表示通道工作，0表示通道暂停。 */
    } reg;
} SOC_LCIPHER_CH4_EN_UNION;
#endif
#define SOC_LCIPHER_CH4_EN_ch_en_START     (0)
#define SOC_LCIPHER_CH4_EN_ch_en_END       (0)
#define SOC_LCIPHER_CH4_EN_pack_en_START   (30)
#define SOC_LCIPHER_CH4_EN_pack_en_END     (30)
#define SOC_LCIPHER_CH4_EN_ch_busy_START   (31)
#define SOC_LCIPHER_CH4_EN_ch_busy_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_CONFIG_UNION
 结构说明  : CH4_CONFIG 寄存器结构定义。地址偏移量:0x288，初值:0x0，宽度:32
 寄存器说明: 通道4配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  iv_sel     : 1;  /* bit[0]   : 本通道完整性参数、IV随BD还是都由芯片自动产生。
                                                     0：完整性参数、IV由芯片根据参数自动产生；
                                                     1：完整性参数、IV随包或随BD。 */
        unsigned int  iv_num     : 1;  /* bit[1]   : 本通道BD中含有的IV个数。只有当iv_sel为1时有意义。
                                                     0：一个IV，此时BD长度固定是；
                                                     1：两个IV，其中一个IV可以是无效IV，但仍需占用内存。 */
        unsigned int  rdq_ctrl   : 2;  /* bit[2-3] : RDQ对列维护方式。当RDQ满且有需要回写的RD时，Cipher等待直到RDQ不满；
                                                     00：不回写RD；
                                                     01：每个BD都对应回写RD。
                                                     others：RD是否回写由BD配置决定，只在BD中的gen_rd_en有效时回写RD。 */
        unsigned int  reserved_0 : 1;  /* bit[4]   : 保留。 */
        unsigned int  ufield_len : 2;  /* bit[5-6] : BD中userfield字段的长度，单位是32bit。 */
        unsigned int  reserved_1 : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH4_CONFIG_UNION;
#endif
#define SOC_LCIPHER_CH4_CONFIG_iv_sel_START      (0)
#define SOC_LCIPHER_CH4_CONFIG_iv_sel_END        (0)
#define SOC_LCIPHER_CH4_CONFIG_iv_num_START      (1)
#define SOC_LCIPHER_CH4_CONFIG_iv_num_END        (1)
#define SOC_LCIPHER_CH4_CONFIG_rdq_ctrl_START    (2)
#define SOC_LCIPHER_CH4_CONFIG_rdq_ctrl_END      (3)
#define SOC_LCIPHER_CH4_CONFIG_ufield_len_START  (5)
#define SOC_LCIPHER_CH4_CONFIG_ufield_len_END    (6)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_BDQ_BADDR_UNION
 结构说明  : CH4_BDQ_BADDR 寄存器结构定义。地址偏移量:0x28C，初值:0x0，宽度:32
 寄存器说明: 通道4 BDQ基地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 BD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道4 BD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH4_BDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH4_BDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH4_BDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH4_BDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH4_BDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_BDQ_SIZE_UNION
 结构说明  : CH4_BDQ_SIZE 寄存器结构定义。地址偏移量:0x290，初值:0x0，宽度:32
 寄存器说明: 通道4 BDQ大小寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 BD Queue大小，以描述符个数为单位。
                                                    0：表示队列大小为1
                                                    255：表示队列大小为256 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH4_BDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH4_BDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH4_BDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_BDQ_PTR_UNION
 结构说明  : CH4_BDQ_PTR 寄存器结构定义。地址偏移量:0x294，初值:0x0，宽度:32
 寄存器说明: 通道4 BDQ读写指针寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr          : 10; /* bit[0-9]  : BD队列写指针。可被软复位 */
        unsigned int  reserved_0    : 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr          : 10; /* bit[16-25]: BD队列读指针。可被软复位。 */
        unsigned int  reserved_1    : 4;  /* bit[26-29]: 保留 */
        unsigned int  ch4_bdq_empty : 1;  /* bit[30]   : BD队列空 */
        unsigned int  ch4_bdq_full  : 1;  /* bit[31]   : BD队列满 */
    } reg;
} SOC_LCIPHER_CH4_BDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH4_BDQ_PTR_wptr_START           (0)
#define SOC_LCIPHER_CH4_BDQ_PTR_wptr_END             (9)
#define SOC_LCIPHER_CH4_BDQ_PTR_rptr_START           (16)
#define SOC_LCIPHER_CH4_BDQ_PTR_rptr_END             (25)
#define SOC_LCIPHER_CH4_BDQ_PTR_ch4_bdq_empty_START  (30)
#define SOC_LCIPHER_CH4_BDQ_PTR_ch4_bdq_empty_END    (30)
#define SOC_LCIPHER_CH4_BDQ_PTR_ch4_bdq_full_START   (31)
#define SOC_LCIPHER_CH4_BDQ_PTR_ch4_bdq_full_END     (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_BDQ_RADDR_UNION
 结构说明  : CH4_BDQ_RADDR 寄存器结构定义。地址偏移量:0x298，初值:0x0，宽度:32
 寄存器说明: 通道4 BDQ读地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道4 BD队列读地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH4_BDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH4_BDQ_RADDR_addr_START  (0)
#define SOC_LCIPHER_CH4_BDQ_RADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_BDQ_WADDR_UNION
 结构说明  : CH4_BDQ_WADDR 寄存器结构定义。地址偏移量:0x29C，初值:0x0，宽度:32
 寄存器说明: 通道4 BDQ写地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr : 32; /* bit[0-31]: 通道4 BD队列写地址。可被软复位。 */
    } reg;
} SOC_LCIPHER_CH4_BDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH4_BDQ_WADDR_addr_START  (0)
#define SOC_LCIPHER_CH4_BDQ_WADDR_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_BD_NUM_UNION
 结构说明  : CH4_BD_NUM 寄存器结构定义。地址偏移量:0x2A0，初值:0x0，宽度:32
 寄存器说明: 表示通道4处理的BD总数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch4_bd_num : 32; /* bit[0-31]: 表示通道4已经处理的BD总数
                                                     只读，
                                                     可以写清零 */
    } reg;
} SOC_LCIPHER_CH4_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH4_BD_NUM_ch4_bd_num_START  (0)
#define SOC_LCIPHER_CH4_BD_NUM_ch4_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_INVALID_BD_NUM_UNION
 结构说明  : CH4_INVALID_BD_NUM 寄存器结构定义。地址偏移量:0x2A4，初值:0x0，宽度:32
 寄存器说明: 表示通道4处理的无效BD数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch4_invalid_bd_num : 32; /* bit[0-31]: 表示通道4已经处理的无效BD总数
                                                             只读，
                                                             可以写清零 */
    } reg;
} SOC_LCIPHER_CH4_INVALID_BD_NUM_UNION;
#endif
#define SOC_LCIPHER_CH4_INVALID_BD_NUM_ch4_invalid_bd_num_START  (0)
#define SOC_LCIPHER_CH4_INVALID_BD_NUM_ch4_invalid_bd_num_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH_INT_PKT_INTERVAL_UNION
 结构说明  : CH_INT_PKT_INTERVAL 寄存器结构定义。地址偏移量:0x400，初值:0xFF0000，宽度:32
 寄存器说明: 中断包间隔寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch0_pkt_num  : 4;  /* bit[0-3]  : 通道0 BD DONE中断包间隔数。
                                                        0：5个数据包；
                                                        1：10个数据包；
                                                        n：10*（n+1）个数据包 */
        unsigned int  ch1_pkt_num  : 4;  /* bit[4-7]  : 通道1 BD DONE中断包间隔数。
                                                        0：5个数据包；
                                                        1：10个数据包；
                                                        n：10*（n+1）个数据包 */
        unsigned int  ch2_pkt_num  : 4;  /* bit[8-11] : 通道2 BD DONE中断包间隔数。
                                                        0：5个数据包；
                                                        1：10个数据包；
                                                        n：10*（n+1）个数据包 */
        unsigned int  ch3_pkt_num  : 4;  /* bit[12-15]: 通道3 BD DONE中断包间隔数。
                                                        0：5个数据包；
                                                        1：10个数据包；
                                                        n：10*（n+1）个数据包 */
        unsigned int  timeout_time : 9;  /* bit[16-24]: 超时中断阈值，4个通道共用。单位是20us。 */
        unsigned int  ch4_pkt_num  : 4;  /* bit[25-28]: 通道4 BD DONE中断包间隔数。
                                                        0：5个数据包；
                                                        1：10个数据包；
                                                        n：10*（n+1）个数据包 */
        unsigned int  reserved     : 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH_INT_PKT_INTERVAL_UNION;
#endif
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch0_pkt_num_START   (0)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch0_pkt_num_END     (3)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch1_pkt_num_START   (4)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch1_pkt_num_END     (7)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch2_pkt_num_START   (8)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch2_pkt_num_END     (11)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch3_pkt_num_START   (12)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch3_pkt_num_END     (15)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_timeout_time_START  (16)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_timeout_time_END    (24)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch4_pkt_num_START   (25)
#define SOC_LCIPHER_CH_INT_PKT_INTERVAL_ch4_pkt_num_END     (28)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT_STATUS_UNION
 结构说明  : CIPHER_INT_STATUS 寄存器结构定义。地址偏移量:0x404，初值:0x0，宽度:32
 寄存器说明: cipher中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cipher_int0 : 1;  /* bit[0]   : CIPHER_INT0_STATUS中断寄存器中的中断是否有中断侵请求。可被软复位 */
        unsigned int  cipher_int1 : 1;  /* bit[1]   : CIPHER_INT1_STATUS中断寄存器中的中断是否有中断侵请求。可被软复位 */
        unsigned int  cipher_int2 : 1;  /* bit[2]   : CIPHER_INT2_STATUS中断寄存器中的中断是否有中断侵请求。可被软复位 */
        unsigned int  reserved    : 29; /* bit[3-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CIPHER_INT_STATUS_UNION;
#endif
#define SOC_LCIPHER_CIPHER_INT_STATUS_cipher_int0_START  (0)
#define SOC_LCIPHER_CIPHER_INT_STATUS_cipher_int0_END    (0)
#define SOC_LCIPHER_CIPHER_INT_STATUS_cipher_int1_START  (1)
#define SOC_LCIPHER_CIPHER_INT_STATUS_cipher_int1_END    (1)
#define SOC_LCIPHER_CIPHER_INT_STATUS_cipher_int2_START  (2)
#define SOC_LCIPHER_CIPHER_INT_STATUS_cipher_int2_END    (2)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT0_STATUS_UNION
 结构说明  : CIPHER_INT0_STATUS 寄存器结构定义。地址偏移量:0x408，初值:0x0，宽度:32
 寄存器说明: cipher中断寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch0_bdq_full      : 1;  /* bit[0]    : 通道0 BDQ满中断。当BDQ满时发起此中断。 */
        unsigned int  ch0_rdq_full      : 1;  /* bit[1]    : 通道0 RDQ满中断。当RDQ满时发起此中断。 */
        unsigned int  ch0_nbd_done      : 1;  /* bit[2]    : 通道0处理完给定数目N个BD之后产生此中断。N的值见CH0_INT_PKT_INTERVAL。 */
        unsigned int  ch0_bd_done       : 1;  /* bit[3]    : 通道0当BD中gen_int_en为1时，在当前BD的操作完成后产生此中断。 */
        unsigned int  ch0_allbd_done    : 1;  /* bit[4]    : 通道0在BDQ中最后一个BD操作完成后产生此中断。 */
        unsigned int  ch0_integrity_err : 1;  /* bit[5]    : 通道0完整性检查错误中断。 */
        unsigned int  reserved_0        : 2;  /* bit[6-7]  :  */
        unsigned int  ch1_bdq_full      : 1;  /* bit[8]    : 通道1 BDQ满中断。当BDQ满时发起此中断。 */
        unsigned int  ch1_rdq_full      : 1;  /* bit[9]    : 通道1 RDQ满中断。当RDQ满时发起此中断。 */
        unsigned int  ch1_nbd_done      : 1;  /* bit[10]   : 通道1处理完给定数目N个BD之后产生此中断。N的值见CH0_INT_PKT_INTERVAL。 */
        unsigned int  ch1_bd_done       : 1;  /* bit[11]   : 通道1当BD中gen_int_en为1时，在当前BD的操作完成后产生此中断。 */
        unsigned int  ch1_allbd_done    : 1;  /* bit[12]   : 通道1在BDQ中最后一个BD操作完成后产生此中断。 */
        unsigned int  ch1_integrity_err : 1;  /* bit[13]   : 通道1完整性检查错误中断。 */
        unsigned int  reserved_1        : 2;  /* bit[14-15]:  */
        unsigned int  ch2_bdq_full      : 1;  /* bit[16]   : 通道2 BDQ满中断。当BDQ满时发起此中断。 */
        unsigned int  ch2_rdq_full      : 1;  /* bit[17]   : 通道2 RDQ满中断。当RDQ满时发起此中断。 */
        unsigned int  ch2_nbd_done      : 1;  /* bit[18]   : 通道2处理完给定数目N个BD之后产生此中断。N的值见CH0_INT_PKT_INTERVAL。 */
        unsigned int  ch2_bd_done       : 1;  /* bit[19]   : 通道2当BD中gen_int_en为1时，在当前BD的操作完成后产生此中断。 */
        unsigned int  ch2_allbd_done    : 1;  /* bit[20]   : 通道2在BDQ中最后一个BD操作完成后产生此中断。 */
        unsigned int  ch2_integrity_err : 1;  /* bit[21]   : 通道2完整性检查错误中断。 */
        unsigned int  reserved_2        : 2;  /* bit[22-23]:  */
        unsigned int  ch3_bdq_full      : 1;  /* bit[24]   : 通道3 BDQ满中断。当BDQ满时发起此中断。 */
        unsigned int  ch3_rdq_full      : 1;  /* bit[25]   : 通道3 RDQ满中断。当RDQ满时发起此中断。 */
        unsigned int  ch3_nbd_done      : 1;  /* bit[26]   : 通道3处理完给定数目N个BD之后产生此中断。N的值见CH0_INT_PKT_INTERVAL。 */
        unsigned int  ch3_bd_done       : 1;  /* bit[27]   : 通道3当BD中gen_int_en为1时，在当前BD的操作完成后产生此中断。
                                                             写1清中断 */
        unsigned int  ch3_allbd_done    : 1;  /* bit[28]   : 通道3在BDQ中最后一个BD操作完成后产生此中断。
                                                             写1清中断 */
        unsigned int  ch3_integrity_err : 1;  /* bit[29]   : 通道3完整性检查错误中断。
                                                             写1清中断。 */
        unsigned int  reserved_3        : 2;  /* bit[30-31]: 保留。该状态寄存器可被软复位 */
    } reg;
} SOC_LCIPHER_CIPHER_INT0_STATUS_UNION;
#endif
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_bdq_full_START       (0)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_bdq_full_END         (0)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_rdq_full_START       (1)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_rdq_full_END         (1)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_nbd_done_START       (2)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_nbd_done_END         (2)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_bd_done_START        (3)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_bd_done_END          (3)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_allbd_done_START     (4)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_allbd_done_END       (4)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_integrity_err_START  (5)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch0_integrity_err_END    (5)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_bdq_full_START       (8)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_bdq_full_END         (8)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_rdq_full_START       (9)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_rdq_full_END         (9)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_nbd_done_START       (10)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_nbd_done_END         (10)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_bd_done_START        (11)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_bd_done_END          (11)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_allbd_done_START     (12)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_allbd_done_END       (12)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_integrity_err_START  (13)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch1_integrity_err_END    (13)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_bdq_full_START       (16)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_bdq_full_END         (16)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_rdq_full_START       (17)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_rdq_full_END         (17)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_nbd_done_START       (18)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_nbd_done_END         (18)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_bd_done_START        (19)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_bd_done_END          (19)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_allbd_done_START     (20)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_allbd_done_END       (20)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_integrity_err_START  (21)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch2_integrity_err_END    (21)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_bdq_full_START       (24)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_bdq_full_END         (24)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_rdq_full_START       (25)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_rdq_full_END         (25)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_nbd_done_START       (26)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_nbd_done_END         (26)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_bd_done_START        (27)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_bd_done_END          (27)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_allbd_done_START     (28)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_allbd_done_END       (28)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_integrity_err_START  (29)
#define SOC_LCIPHER_CIPHER_INT0_STATUS_ch3_integrity_err_END    (29)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT0_MASK_UNION
 结构说明  : CIPHER_INT0_MASK 寄存器结构定义。地址偏移量:0x40C，初值:0x0，宽度:32
 寄存器说明: cipher中断屏蔽寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int0_mask : 32; /* bit[0-31]: 与cipher_int0_status各位对应的中断屏蔽寄存器 */
    } reg;
} SOC_LCIPHER_CIPHER_INT0_MASK_UNION;
#endif
#define SOC_LCIPHER_CIPHER_INT0_MASK_int0_mask_START  (0)
#define SOC_LCIPHER_CIPHER_INT0_MASK_int0_mask_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT0_MSTATUS_UNION
 结构说明  : CIPHER_INT0_MSTATUS 寄存器结构定义。地址偏移量:0x410，初值:0x0，宽度:32
 寄存器说明: cipher经过屏蔽之后的中断状态寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int0_mstatus : 32; /* bit[0-31]: 经过屏蔽之后的中断状态，写1清除对应bit。可被软复位。 */
    } reg;
} SOC_LCIPHER_CIPHER_INT0_MSTATUS_UNION;
#endif
#define SOC_LCIPHER_CIPHER_INT0_MSTATUS_int0_mstatus_START  (0)
#define SOC_LCIPHER_CIPHER_INT0_MSTATUS_int0_mstatus_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT1_STATUS_UNION
 结构说明  : CIPHER_INT1_STATUS 寄存器结构定义。地址偏移量:0x414，初值:0xFFFFFFFF，宽度:32
 寄存器说明: cipher中断寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch0_bdq_empty : 1;  /* bit[0]    : 通道0 BDQ空中断。当BDQ空且所有BD已处理完时发起此中断。 */
        unsigned int  ch0_rdq_empty : 1;  /* bit[1]    : 通道0 RDQ空中断。当RDQ空时发起此中断。 */
        unsigned int  ch0_timeout   : 1;  /* bit[2]    : cipher一直忙且一直没有BD处理完成。该状态表明cipher出现故障，需要软件处理（复位）。 */
        unsigned int  reserved_0    : 5;  /* bit[3-7]  :  */
        unsigned int  ch1_bdq_empty : 1;  /* bit[8]    : 通道1 BDQ空中断。当BDQ空且所有BD已处理完时发起此中断。 */
        unsigned int  ch1_rdq_empty : 1;  /* bit[9]    : 通道1 RDQ空中断。当RDQ空时发起此中断。 */
        unsigned int  ch1_timeout   : 1;  /* bit[10]   : cipher一直忙且一直没有BD处理完成。该状态表明cipher出现故障，需要软件处理（复位）。 */
        unsigned int  reserved_1    : 5;  /* bit[11-15]:  */
        unsigned int  ch2_bdq_empty : 1;  /* bit[16]   : 通道2 BDQ空中断。当BDQ空且所有BD已处理完时发起此中断。 */
        unsigned int  ch2_rdq_empty : 1;  /* bit[17]   : 通道2 RDQ空中断。当RDQ空时发起此中断。 */
        unsigned int  ch2_timeout   : 1;  /* bit[18]   : cipher一直忙且一直没有BD处理完成。该状态表明cipher出现故障，需要软件处理（复位）。 */
        unsigned int  reserved_2    : 5;  /* bit[19-23]:  */
        unsigned int  ch3_bdq_empty : 1;  /* bit[24]   : 通道3 BDQ空中断。当BDQ空且所有BD已处理完时发起此中断。 */
        unsigned int  ch3_rdq_empty : 1;  /* bit[25]   : 通道3 RDQ空中断。当RDQ空时发起此中断。 */
        unsigned int  ch3_timeout   : 1;  /* bit[26]   : cipher一直忙且一直没有BD处理完成。该状态表明cipher出现故障，需要软件处理（复位）。 */
        unsigned int  reserved_3    : 5;  /* bit[27-31]: 保留。该状态寄存器可被软复位 */
    } reg;
} SOC_LCIPHER_CIPHER_INT1_STATUS_UNION;
#endif
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch0_bdq_empty_START  (0)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch0_bdq_empty_END    (0)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch0_rdq_empty_START  (1)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch0_rdq_empty_END    (1)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch0_timeout_START    (2)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch0_timeout_END      (2)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch1_bdq_empty_START  (8)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch1_bdq_empty_END    (8)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch1_rdq_empty_START  (9)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch1_rdq_empty_END    (9)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch1_timeout_START    (10)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch1_timeout_END      (10)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch2_bdq_empty_START  (16)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch2_bdq_empty_END    (16)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch2_rdq_empty_START  (17)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch2_rdq_empty_END    (17)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch2_timeout_START    (18)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch2_timeout_END      (18)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch3_bdq_empty_START  (24)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch3_bdq_empty_END    (24)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch3_rdq_empty_START  (25)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch3_rdq_empty_END    (25)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch3_timeout_START    (26)
#define SOC_LCIPHER_CIPHER_INT1_STATUS_ch3_timeout_END      (26)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT1_MASK_UNION
 结构说明  : CIPHER_INT1_MASK 寄存器结构定义。地址偏移量:0x418，初值:0xFFFFFFFF，宽度:32
 寄存器说明: cipher中断屏蔽寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int1_mask : 32; /* bit[0-31]: 与cipher_int1_status各位对应的中断屏蔽寄存器 */
    } reg;
} SOC_LCIPHER_CIPHER_INT1_MASK_UNION;
#endif
#define SOC_LCIPHER_CIPHER_INT1_MASK_int1_mask_START  (0)
#define SOC_LCIPHER_CIPHER_INT1_MASK_int1_mask_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT1_MSTATUS_UNION
 结构说明  : CIPHER_INT1_MSTATUS 寄存器结构定义。地址偏移量:0x41C，初值:0x0，宽度:32
 寄存器说明: cipher经过屏蔽之后的中断状态寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int1_mstatus : 32; /* bit[0-31]: 经过屏蔽之后的中断状态，写1清除对应bit。可被软复位。 */
    } reg;
} SOC_LCIPHER_CIPHER_INT1_MSTATUS_UNION;
#endif
#define SOC_LCIPHER_CIPHER_INT1_MSTATUS_int1_mstatus_START  (0)
#define SOC_LCIPHER_CIPHER_INT1_MSTATUS_int1_mstatus_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT2_STATUS_UNION
 结构说明  : CIPHER_INT2_STATUS 寄存器结构定义。地址偏移量:0x420，初值:0x60，宽度:32
 寄存器说明: cipher中断寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch4_bdq_full   : 1;  /* bit[0]   : 通道4 BDQ满中断。当BDQ满时发起此中断。 */
        unsigned int  ch4_rdq_full   : 1;  /* bit[1]   : 通道4 RDQ满中断。当RDQ满时发起此中断。 */
        unsigned int  ch4_nbd_done   : 1;  /* bit[2]   : 通道4处理完给定数目N个BD之后产生此中断。N的值见CH0_INT_PKT_INTERVAL。 */
        unsigned int  ch4_bd_done    : 1;  /* bit[3]   : 通道4当BD中gen_int_en为1时，在当前BD的操作完成后产生此中断。 */
        unsigned int  ch4_allbd_done : 1;  /* bit[4]   : 通道4在BDQ中最后一个BD操作完成后产生此中断。 */
        unsigned int  ch4_bdq_empty  : 1;  /* bit[5]   : 通道4 BDQ空中断。当BDQ空且所有BD已处理完时发起此中断。 */
        unsigned int  ch4_rdq_empty  : 1;  /* bit[6]   : 通道4 RDQ空中断。当RDQ空时发起此中断。 */
        unsigned int  ch4_timeout    : 1;  /* bit[7]   : cipher一直忙且一直没有BD处理完成。该状态表明cipher出现故障，需要软件处理（复位）。 */
        unsigned int  reserved       : 24; /* bit[8-31]: 保留。该状态寄存器可被软复位 */
    } reg;
} SOC_LCIPHER_CIPHER_INT2_STATUS_UNION;
#endif
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_bdq_full_START    (0)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_bdq_full_END      (0)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_rdq_full_START    (1)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_rdq_full_END      (1)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_nbd_done_START    (2)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_nbd_done_END      (2)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_bd_done_START     (3)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_bd_done_END       (3)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_allbd_done_START  (4)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_allbd_done_END    (4)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_bdq_empty_START   (5)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_bdq_empty_END     (5)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_rdq_empty_START   (6)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_rdq_empty_END     (6)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_timeout_START     (7)
#define SOC_LCIPHER_CIPHER_INT2_STATUS_ch4_timeout_END       (7)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT2_MASK_UNION
 结构说明  : CIPHER_INT2_MASK 寄存器结构定义。地址偏移量:0x424，初值:0x60，宽度:32
 寄存器说明: cipher中断屏蔽寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 与cipher_int2_status各位对应的中断屏蔽寄存器 */
    } reg;
} SOC_LCIPHER_CIPHER_INT2_MASK_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_CIPHER_INT2_MSTATUS_UNION
 结构说明  : CIPHER_INT2_MSTATUS 寄存器结构定义。地址偏移量:0x428，初值:0x0，宽度:32
 寄存器说明: cipher经过屏蔽之后的中断状态寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 32; /* bit[0-31]: 经过屏蔽之后的中断状态，写1清除对应bit。可被软复位。 */
    } reg;
} SOC_LCIPHER_CIPHER_INT2_MSTATUS_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_RDQ_BADDR_UNION
 结构说明  : CH0_RDQ_BADDR 寄存器结构定义。地址偏移量:0x480，初值:0x0，宽度:32
 寄存器说明: 通道0 RD队列基地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 RD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道0 RD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH0_RDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH0_RDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH0_RDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH0_RDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH0_RDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_RDQ_SIZE_UNION
 结构说明  : CH0_RDQ_SIZE 寄存器结构定义。地址偏移量:0x484，初值:0x0，宽度:32
 寄存器说明: 通道0 RD队列大小
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 RD Queue大小，以描述符个数为单位。其中
                                                    0：表示队列大小为1
                                                    1023：表示队列大小为1024 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH0_RDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH0_RDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH0_RDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_RDQ_PTR_UNION
 结构说明  : CH0_RDQ_PTR 寄存器结构定义。地址偏移量:0x488，初值:0x0，宽度:32
 寄存器说明: 通道0 RD队列的读写指针
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr     : 10; /* bit[0-9]  : RD队列写指针。可被软复位 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr     : 10; /* bit[16-25]: RD队列读指针。可被软复位 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH0_RDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH0_RDQ_PTR_wptr_START      (0)
#define SOC_LCIPHER_CH0_RDQ_PTR_wptr_END        (9)
#define SOC_LCIPHER_CH0_RDQ_PTR_rptr_START      (16)
#define SOC_LCIPHER_CH0_RDQ_PTR_rptr_END        (25)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_RDQ_RADDR_UNION
 结构说明  : CH0_RDQ_RADDR 寄存器结构定义。地址偏移量:0x48C，初值:0x0，宽度:32
 寄存器说明: 通道0 RD队列的读地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  raddr : 32; /* bit[0-31]: 通道0 RD队列读地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH0_RDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH0_RDQ_RADDR_raddr_START  (0)
#define SOC_LCIPHER_CH0_RDQ_RADDR_raddr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH0_RDQ_WADDR_UNION
 结构说明  : CH0_RDQ_WADDR 寄存器结构定义。地址偏移量:0x490，初值:0x0，宽度:32
 寄存器说明: 通道0 RD队列的写地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  waddr : 32; /* bit[0-31]: 通道0 RD队列写地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH0_RDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH0_RDQ_WADDR_waddr_START  (0)
#define SOC_LCIPHER_CH0_RDQ_WADDR_waddr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_RDQ_BADDR_UNION
 结构说明  : CH1_RDQ_BADDR 寄存器结构定义。地址偏移量:0x500，初值:0x0，宽度:32
 寄存器说明: 通道1 RD队列基地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 RD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道1 RD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH1_RDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH1_RDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH1_RDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH1_RDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH1_RDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_RDQ_SIZE_UNION
 结构说明  : CH1_RDQ_SIZE 寄存器结构定义。地址偏移量:0x504，初值:0x0，宽度:32
 寄存器说明: 通道1 RD队列大小
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 RD Queue大小，以描述符个数为单位。其中
                                                    0：表示队列大小为1
                                                    1023：表示队列大小为1024 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH1_RDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH1_RDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH1_RDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_RDQ_PTR_UNION
 结构说明  : CH1_RDQ_PTR 寄存器结构定义。地址偏移量:0x508，初值:0x0，宽度:32
 寄存器说明: 通道1 RD队列的读写指针
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr     : 10; /* bit[0-9]  : RD队列写指针。可被软复位 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr     : 10; /* bit[16-25]: RD队列读指针。可被软复位 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH1_RDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH1_RDQ_PTR_wptr_START      (0)
#define SOC_LCIPHER_CH1_RDQ_PTR_wptr_END        (9)
#define SOC_LCIPHER_CH1_RDQ_PTR_rptr_START      (16)
#define SOC_LCIPHER_CH1_RDQ_PTR_rptr_END        (25)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_RDQ_RADDR_UNION
 结构说明  : CH1_RDQ_RADDR 寄存器结构定义。地址偏移量:0x50C，初值:0x0，宽度:32
 寄存器说明: 通道1 RD队列的读地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  raddr : 32; /* bit[0-31]: 通道1 RD队列读地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH1_RDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH1_RDQ_RADDR_raddr_START  (0)
#define SOC_LCIPHER_CH1_RDQ_RADDR_raddr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH1_RDQ_WADDR_UNION
 结构说明  : CH1_RDQ_WADDR 寄存器结构定义。地址偏移量:0x510，初值:0x0，宽度:32
 寄存器说明: 通道1 RD队列的写地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  waddr : 32; /* bit[0-31]: 通道1 RD队列写地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH1_RDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH1_RDQ_WADDR_waddr_START  (0)
#define SOC_LCIPHER_CH1_RDQ_WADDR_waddr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_RDQ_BADDR_UNION
 结构说明  : CH2_RDQ_BADDR 寄存器结构定义。地址偏移量:0x580，初值:0x0，宽度:32
 寄存器说明: 通道2 RD队列基地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 RD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道2 RD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH2_RDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH2_RDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH2_RDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH2_RDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH2_RDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_RDQ_SIZE_UNION
 结构说明  : CH2_RDQ_SIZE 寄存器结构定义。地址偏移量:0x584，初值:0x0，宽度:32
 寄存器说明: 通道2 RD队列大小
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 RD Queue大小，以描述符个数为单位。其中
                                                    0：表示队列大小为1
                                                    1023：表示队列大小为1024 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH2_RDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH2_RDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH2_RDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_RDQ_PTR_UNION
 结构说明  : CH2_RDQ_PTR 寄存器结构定义。地址偏移量:0x588，初值:0x0，宽度:32
 寄存器说明: 通道2 RD队列的读写指针
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr     : 10; /* bit[0-9]  : RD队列写指针。可被软复位 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr     : 10; /* bit[16-25]: RD队列读指针。可被软复位 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH2_RDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH2_RDQ_PTR_wptr_START      (0)
#define SOC_LCIPHER_CH2_RDQ_PTR_wptr_END        (9)
#define SOC_LCIPHER_CH2_RDQ_PTR_rptr_START      (16)
#define SOC_LCIPHER_CH2_RDQ_PTR_rptr_END        (25)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_RDQ_RADDR_UNION
 结构说明  : CH2_RDQ_RADDR 寄存器结构定义。地址偏移量:0x58C，初值:0x0，宽度:32
 寄存器说明: 通道2 RD队列的读地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  raddr : 32; /* bit[0-31]: 通道2 RD队列读地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH2_RDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH2_RDQ_RADDR_raddr_START  (0)
#define SOC_LCIPHER_CH2_RDQ_RADDR_raddr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH2_RDQ_WADDR_UNION
 结构说明  : CH2_RDQ_WADDR 寄存器结构定义。地址偏移量:0x590，初值:0x0，宽度:32
 寄存器说明: 通道2 RD队列的写地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  waddr : 32; /* bit[0-31]: 通道2 RD队列写地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH2_RDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH2_RDQ_WADDR_waddr_START  (0)
#define SOC_LCIPHER_CH2_RDQ_WADDR_waddr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_RDQ_BADDR_UNION
 结构说明  : CH3_RDQ_BADDR 寄存器结构定义。地址偏移量:0x600，初值:0x0，宽度:32
 寄存器说明: 通道3 RD队列基地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 RD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道3 RD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH3_RDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH3_RDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH3_RDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH3_RDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH3_RDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_RDQ_SIZE_UNION
 结构说明  : CH3_RDQ_SIZE 寄存器结构定义。地址偏移量:0x604，初值:0x0，宽度:32
 寄存器说明: 通道3 RD队列大小
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 RD Queue大小，以描述符个数为单位。其中
                                                    0：表示队列大小为1
                                                    1023：表示队列大小为1024 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH3_RDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH3_RDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH3_RDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_RDQ_PTR_UNION
 结构说明  : CH3_RDQ_PTR 寄存器结构定义。地址偏移量:0x608，初值:0x0，宽度:32
 寄存器说明: 通道3 RD队列的读写指针
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr     : 10; /* bit[0-9]  : RD队列写指针。可被软复位 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr     : 10; /* bit[16-25]: RD队列读指针。可被软复位 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH3_RDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH3_RDQ_PTR_wptr_START      (0)
#define SOC_LCIPHER_CH3_RDQ_PTR_wptr_END        (9)
#define SOC_LCIPHER_CH3_RDQ_PTR_rptr_START      (16)
#define SOC_LCIPHER_CH3_RDQ_PTR_rptr_END        (25)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_RDQ_RADDR_UNION
 结构说明  : CH3_RDQ_RADDR 寄存器结构定义。地址偏移量:0x60C，初值:0x0，宽度:32
 寄存器说明: 通道3 RD队列的读地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  raddr : 32; /* bit[0-31]: 通道3 RD队列读地址。可被软复位。 */
    } reg;
} SOC_LCIPHER_CH3_RDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH3_RDQ_RADDR_raddr_START  (0)
#define SOC_LCIPHER_CH3_RDQ_RADDR_raddr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH3_RDQ_WADDR_UNION
 结构说明  : CH3_RDQ_WADDR 寄存器结构定义。地址偏移量:0x610，初值:0x0，宽度:32
 寄存器说明: 通道3 RD队列的写地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  waddr : 32; /* bit[0-31]: 通道3 RD队列写地址。可被软复位。 */
    } reg;
} SOC_LCIPHER_CH3_RDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH3_RDQ_WADDR_waddr_START  (0)
#define SOC_LCIPHER_CH3_RDQ_WADDR_waddr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_RDQ_BADDR_UNION
 结构说明  : CH4_RDQ_BADDR 寄存器结构定义。地址偏移量:0x680，初值:0x0，宽度:32
 寄存器说明: 通道4 RD队列基地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  base_addr_l : 3;  /* bit[0-2] : 通道0 RD队列基地址低3bit，固定为0。即要求基址64位对齐。 */
        unsigned int  base_addr   : 29; /* bit[3-31]: 通道4 RD队列基地址高29位 */
    } reg;
} SOC_LCIPHER_CH4_RDQ_BADDR_UNION;
#endif
#define SOC_LCIPHER_CH4_RDQ_BADDR_base_addr_l_START  (0)
#define SOC_LCIPHER_CH4_RDQ_BADDR_base_addr_l_END    (2)
#define SOC_LCIPHER_CH4_RDQ_BADDR_base_addr_START    (3)
#define SOC_LCIPHER_CH4_RDQ_BADDR_base_addr_END      (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_RDQ_SIZE_UNION
 结构说明  : CH4_RDQ_SIZE 寄存器结构定义。地址偏移量:0x684，初值:0x0，宽度:32
 寄存器说明: 通道4 RD队列大小
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  size     : 10; /* bit[0-9]  : 通道0 RD Queue大小，以描述符个数为单位。其中
                                                    0：表示队列大小为1
                                                    1023：表示队列大小为1024 */
        unsigned int  reserved : 22; /* bit[10-31]: 成员说明。 */
    } reg;
} SOC_LCIPHER_CH4_RDQ_SIZE_UNION;
#endif
#define SOC_LCIPHER_CH4_RDQ_SIZE_size_START      (0)
#define SOC_LCIPHER_CH4_RDQ_SIZE_size_END        (9)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_RDQ_PTR_UNION
 结构说明  : CH4_RDQ_PTR 寄存器结构定义。地址偏移量:0x688，初值:0x0，宽度:32
 寄存器说明: 通道4 RD队列的读写指针
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wptr     : 10; /* bit[0-9]  : RD队列写指针。可被软复位 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留 */
        unsigned int  rptr     : 10; /* bit[16-25]: RD队列读指针。可被软复位 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_LCIPHER_CH4_RDQ_PTR_UNION;
#endif
#define SOC_LCIPHER_CH4_RDQ_PTR_wptr_START      (0)
#define SOC_LCIPHER_CH4_RDQ_PTR_wptr_END        (9)
#define SOC_LCIPHER_CH4_RDQ_PTR_rptr_START      (16)
#define SOC_LCIPHER_CH4_RDQ_PTR_rptr_END        (25)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_RDQ_RADDR_UNION
 结构说明  : CH4_RDQ_RADDR 寄存器结构定义。地址偏移量:0x68C，初值:0x0，宽度:32
 寄存器说明: 通道4 RD队列的读地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  raddr : 32; /* bit[0-31]: 通道4 RD队列读地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH4_RDQ_RADDR_UNION;
#endif
#define SOC_LCIPHER_CH4_RDQ_RADDR_raddr_START  (0)
#define SOC_LCIPHER_CH4_RDQ_RADDR_raddr_END    (31)


/*****************************************************************************
 结构名    : SOC_LCIPHER_CH4_RDQ_WADDR_UNION
 结构说明  : CH4_RDQ_WADDR 寄存器结构定义。地址偏移量:0x690，初值:0x0，宽度:32
 寄存器说明: 通道4 RD队列的写地址
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  waddr : 32; /* bit[0-31]: 通道4 RD队列写地址。可被软复位 */
    } reg;
} SOC_LCIPHER_CH4_RDQ_WADDR_UNION;
#endif
#define SOC_LCIPHER_CH4_RDQ_WADDR_waddr_START  (0)
#define SOC_LCIPHER_CH4_RDQ_WADDR_waddr_END    (31)






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

#endif /* end of soc_lcipher_interface.h */
