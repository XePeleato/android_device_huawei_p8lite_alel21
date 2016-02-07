

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_UPACC_INTERFACE_H__
#define __SOC_UPACC_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) UPACC
 ****************************************************************************/
/* 寄存器说明：封装启动寄存器。
   位域定义UNION结构:  SOC_UPACC_MAC_ENCAP_START_UNION */
#define SOC_UPACC_MAC_ENCAP_START_ADDR(base)          ((base) + (0x0000))

/* 寄存器说明：逻辑信道个数寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH_NUM_UNION */
#define SOC_UPACC_LOCH_NUM_ADDR(base)                 ((base) + (0x0004))

/* 寄存器说明：DDI0使能寄存器。
   位域定义UNION结构:  SOC_UPACC_DDI0_EN_UNION */
#define SOC_UPACC_DDI0_EN_ADDR(base)                  ((base) + (0x0008))

/* 寄存器说明：SI使能寄存器。
   位域定义UNION结构:  SOC_UPACC_SI_EN_UNION */
#define SOC_UPACC_SI_EN_ADDR(base)                    ((base) + (0x000C))

/* 寄存器说明：SI数据寄存器。
   位域定义UNION结构:  SOC_UPACC_SI_DATA_UNION */
#define SOC_UPACC_SI_DATA_ADDR(base)                  ((base) + (0x0010))

/* 寄存器说明：padding长度寄存器。
   位域定义UNION结构:  SOC_UPACC_PADDING_LENGTH_UNION */
#define SOC_UPACC_PADDING_LENGTH_ADDR(base)           ((base) + (0x0014))

/* 寄存器说明：封装目标地址及搬运起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_ENCAP_TRANS_ADDR_UNION */
#define SOC_UPACC_ENCAP_TRANS_ADDR_ADDR(base)         ((base) + (0x0018))

/* 寄存器说明：搬运个数配置寄存器。
   位域定义UNION结构:  SOC_UPACC_TRANS_NUM_UNION */
#define SOC_UPACC_TRANS_NUM_ADDR(base)                ((base) + (0x001C))

/* 寄存器说明：搬运目标地址寄存器。
   位域定义UNION结构:  SOC_UPACC_BBP_DEST_ADDR_UNION */
#define SOC_UPACC_BBP_DEST_ADDR_ADDR(base)            ((base) + (0x0020))

/* 寄存器说明：中断使能寄存器。Bit值为0表示中断禁止；值为1表示中断使能。
   位域定义UNION结构:  SOC_UPACC_INT_EN_UNION */
#define SOC_UPACC_INT_EN_ADDR(base)                   ((base) + (0x0024))

/* 寄存器说明：原始中断状态寄存器。
   位域定义UNION结构:  SOC_UPACC_INT_RAW_UNION */
#define SOC_UPACC_INT_RAW_ADDR(base)                  ((base) + (0x0028))

/* 寄存器说明：屏蔽后中断状态寄存器。
   位域定义UNION结构:  SOC_UPACC_INT_MASK_UNION */
#define SOC_UPACC_INT_MASK_ADDR(base)                 ((base) + (0x002C))

/* 寄存器说明：中断清除寄存器。
   位域定义UNION结构:  SOC_UPACC_INT_CLR_UNION */
#define SOC_UPACC_INT_CLR_ADDR(base)                  ((base) + (0x0030))

/* 寄存器说明：总线ERROR地址寄存器。
   位域定义UNION结构:  SOC_UPACC_BUS_ERROR_ADDR_UNION */
#define SOC_UPACC_BUS_ERROR_ADDR_ADDR(base)           ((base) + (0x0034))

/* 寄存器说明：逻辑信道1参数配置寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH1_PARAM_UNION */
#define SOC_UPACC_LOCH1_PARAM_ADDR(base)              ((base) + (0x0038))

/* 寄存器说明：逻辑信道2参数配置寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH2_PARAM_UNION */
#define SOC_UPACC_LOCH2_PARAM_ADDR(base)              ((base) + (0x003C))

/* 寄存器说明：逻辑信道1参数配置寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH3_PARAM_UNION */
#define SOC_UPACC_LOCH3_PARAM_ADDR(base)              ((base) + (0x0040))

/* 寄存器说明：逻辑信道1参数配置寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH4_PARAM_UNION */
#define SOC_UPACC_LOCH4_PARAM_ADDR(base)              ((base) + (0x0044))

/* 寄存器说明：逻辑信道1参数配置寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH5_PARAM_UNION */
#define SOC_UPACC_LOCH5_PARAM_ADDR(base)              ((base) + (0x0048))

/* 寄存器说明：逻辑信道1参数配置寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH6_PARAM_UNION */
#define SOC_UPACC_LOCH6_PARAM_ADDR(base)              ((base) + (0x004C))

/* 寄存器说明：逻辑信道1参数配置寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH7_PARAM_UNION */
#define SOC_UPACC_LOCH7_PARAM_ADDR(base)              ((base) + (0x0050))

/* 寄存器说明：逻辑信道1参数配置寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH8_PARAM_UNION */
#define SOC_UPACC_LOCH8_PARAM_ADDR(base)              ((base) + (0x0054))

/* 寄存器说明：逻辑信道1起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH1_START_ADDR_UNION */
#define SOC_UPACC_LOCH1_START_ADDR_ADDR(base)         ((base) + (0x0058))

/* 寄存器说明：逻辑信道2起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH2_START_ADDR_UNION */
#define SOC_UPACC_LOCH2_START_ADDR_ADDR(base)         ((base) + (0x005C))

/* 寄存器说明：逻辑信道3起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH3_START_ADDR_UNION */
#define SOC_UPACC_LOCH3_START_ADDR_ADDR(base)         ((base) + (0x0060))

/* 寄存器说明：逻辑信道4起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH4_START_ADDR_UNION */
#define SOC_UPACC_LOCH4_START_ADDR_ADDR(base)         ((base) + (0x0064))

/* 寄存器说明：逻辑信道5起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH5_START_ADDR_UNION */
#define SOC_UPACC_LOCH5_START_ADDR_ADDR(base)         ((base) + (0x0068))

/* 寄存器说明：逻辑信道6起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH6_START_ADDR_UNION */
#define SOC_UPACC_LOCH6_START_ADDR_ADDR(base)         ((base) + (0x006C))

/* 寄存器说明：逻辑信道7起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH7_START_ADDR_UNION */
#define SOC_UPACC_LOCH7_START_ADDR_ADDR(base)         ((base) + (0x0070))

/* 寄存器说明：逻辑信道8起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH8_START_ADDR_UNION */
#define SOC_UPACC_LOCH8_START_ADDR_ADDR(base)         ((base) + (0x0074))

/* 寄存器说明：逻辑信道1结束地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH1_END_ADDR_UNION */
#define SOC_UPACC_LOCH1_END_ADDR_ADDR(base)           ((base) + (0x0078))

/* 寄存器说明：逻辑信道2结束地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH2_END_ADDR_UNION */
#define SOC_UPACC_LOCH2_END_ADDR_ADDR(base)           ((base) + (0x007C))

/* 寄存器说明：逻辑信道3结束地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH3_END_ADDR_UNION */
#define SOC_UPACC_LOCH3_END_ADDR_ADDR(base)           ((base) + (0x0080))

/* 寄存器说明：逻辑信道4结束地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH4_END_ADDR_UNION */
#define SOC_UPACC_LOCH4_END_ADDR_ADDR(base)           ((base) + (0x0084))

/* 寄存器说明：逻辑信道5结束地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH5_END_ADDR_UNION */
#define SOC_UPACC_LOCH5_END_ADDR_ADDR(base)           ((base) + (0x0088))

/* 寄存器说明：逻辑信道6结束地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH6_END_ADDR_UNION */
#define SOC_UPACC_LOCH6_END_ADDR_ADDR(base)           ((base) + (0x008C))

/* 寄存器说明：逻辑信道7结束地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH7_END_ADDR_UNION */
#define SOC_UPACC_LOCH7_END_ADDR_ADDR(base)           ((base) + (0x0090))

/* 寄存器说明：逻辑信道8结束地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH8_END_ADDR_UNION */
#define SOC_UPACC_LOCH8_END_ADDR_ADDR(base)           ((base) + (0x0094))

/* 寄存器说明：逻辑信道1读起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH1_READ_ADDR_UNION */
#define SOC_UPACC_LOCH1_READ_ADDR_ADDR(base)          ((base) + (0x0098))

/* 寄存器说明：逻辑信道2读起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH2_READ_ADDR_UNION */
#define SOC_UPACC_LOCH2_READ_ADDR_ADDR(base)          ((base) + (0x009C))

/* 寄存器说明：逻辑信道3读起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH3_READ_ADDR_UNION */
#define SOC_UPACC_LOCH3_READ_ADDR_ADDR(base)          ((base) + (0x00A0))

/* 寄存器说明：逻辑信道4读起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH4_READ_ADDR_UNION */
#define SOC_UPACC_LOCH4_READ_ADDR_ADDR(base)          ((base) + (0x00A4))

/* 寄存器说明：逻辑信道5读起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH5_READ_ADDR_UNION */
#define SOC_UPACC_LOCH5_READ_ADDR_ADDR(base)          ((base) + (0x00A8))

/* 寄存器说明：逻辑信道6读起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH6_READ_ADDR_UNION */
#define SOC_UPACC_LOCH6_READ_ADDR_ADDR(base)          ((base) + (0x00AC))

/* 寄存器说明：逻辑信道7读起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH7_READ_ADDR_UNION */
#define SOC_UPACC_LOCH7_READ_ADDR_ADDR(base)          ((base) + (0x00B0))

/* 寄存器说明：逻辑信道8读起始地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH8_READ_ADDR_UNION */
#define SOC_UPACC_LOCH8_READ_ADDR_ADDR(base)          ((base) + (0x00B4))

/* 寄存器说明：逻辑信道1读数据相关个数寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH1_NUM_CFG_UNION */
#define SOC_UPACC_LOCH1_NUM_CFG_ADDR(base)            ((base) + (0x00B8))

/* 寄存器说明：逻辑信道2读数据相关个数寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH2_NUM_CFG_UNION */
#define SOC_UPACC_LOCH2_NUM_CFG_ADDR(base)            ((base) + (0x00BC))

/* 寄存器说明：逻辑信道3读数据相关个数寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH3_NUM_CFG_UNION */
#define SOC_UPACC_LOCH3_NUM_CFG_ADDR(base)            ((base) + (0x00C0))

/* 寄存器说明：逻辑信道4读数据相关个数寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH4_NUM_CFG_UNION */
#define SOC_UPACC_LOCH4_NUM_CFG_ADDR(base)            ((base) + (0x00C4))

/* 寄存器说明：逻辑信道5读数据相关个数寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH5_NUM_CFG_UNION */
#define SOC_UPACC_LOCH5_NUM_CFG_ADDR(base)            ((base) + (0x00C8))

/* 寄存器说明：逻辑信道6读数据相关个数寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH6_NUM_CFG_UNION */
#define SOC_UPACC_LOCH6_NUM_CFG_ADDR(base)            ((base) + (0x00CC))

/* 寄存器说明：逻辑信道7读数据相关个数寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH7_NUM_CFG_UNION */
#define SOC_UPACC_LOCH7_NUM_CFG_ADDR(base)            ((base) + (0x00D0))

/* 寄存器说明：逻辑信道8读数据相关个数寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH8_NUM_CFG_UNION */
#define SOC_UPACC_LOCH8_NUM_CFG_ADDR(base)            ((base) + (0x00D4))

/* 寄存器说明：模块使能寄存器。
   位域定义UNION结构:  SOC_UPACC_EN_UNION */
#define SOC_UPACC_EN_ADDR(base)                       ((base) + (0x00D8))

/* 寄存器说明：Mac-e PDU源数据数组方式配置使能寄存器。
   位域定义UNION结构:  SOC_UPACC_PDUARY_EN_E_UNION */
#define SOC_UPACC_PDUARY_EN_E_ADDR(base)              ((base) + (0x00DC))

/* 寄存器说明：逻辑信道1中待读取Mac-e PDU源数据数组首地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH1_PDUARY_ADDR_UNION */
#define SOC_UPACC_LOCH1_PDUARY_ADDR_ADDR(base)        ((base) + (0x00E0))

/* 寄存器说明：逻辑信道2中待读取Mac-e PDU源数据数组首地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH2_PDUARY_ADDR_UNION */
#define SOC_UPACC_LOCH2_PDUARY_ADDR_ADDR(base)        ((base) + (0x00E4))

/* 寄存器说明：逻辑信道3中待读取Mac-e PDU源数据数组首地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH3_PDUARY_ADDR_UNION */
#define SOC_UPACC_LOCH3_PDUARY_ADDR_ADDR(base)        ((base) + (0x00E8))

/* 寄存器说明：逻辑信道4中待读取Mac-e PDU源数据数组首地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH4_PDUARY_ADDR_UNION */
#define SOC_UPACC_LOCH4_PDUARY_ADDR_ADDR(base)        ((base) + (0x00EC))

/* 寄存器说明：逻辑信道5中待读取Mac-e PDU源数据数组首地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH5_PDUARY_ADDR_UNION */
#define SOC_UPACC_LOCH5_PDUARY_ADDR_ADDR(base)        ((base) + (0x00F0))

/* 寄存器说明：逻辑信道6中待读取Mac-e PDU源数据数组首地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH6_PDUARY_ADDR_UNION */
#define SOC_UPACC_LOCH6_PDUARY_ADDR_ADDR(base)        ((base) + (0x00F4))

/* 寄存器说明：逻辑信道7中待读取Mac-e PDU源数据数组首地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH7_PDUARY_ADDR_UNION */
#define SOC_UPACC_LOCH7_PDUARY_ADDR_ADDR(base)        ((base) + (0x00F8))

/* 寄存器说明：逻辑信道8中待读取Mac-e PDU源数据数组首地址寄存器。
   位域定义UNION结构:  SOC_UPACC_LOCH8_PDUARY_ADDR_UNION */
#define SOC_UPACC_LOCH8_PDUARY_ADDR_ADDR(base)        ((base) + (0x00FC))

/* 寄存器说明：启动方式配置寄存器。用于配置MAC_i/is PDU封装的启动方式。
            UPACCV200支持对MAC-i/is PDU 进行不封装只搬移和封装且搬移两种启动方式。
   位域定义UNION结构:  SOC_UPACC_MAC_ENCAP_START_I1_UNION */
#define SOC_UPACC_MAC_ENCAP_START_I1_ADDR(base)       ((base) + (0x0100))

/* 寄存器说明：逻辑信道个数寄存器。用于指示当次封装MAC-i/is PDU共需从几个逻辑信道中读取数据。若该寄存器内容配置为0，表示当前MAC-i/is PDU仅由SI组成，没有其他数据和参数，即为SI单独发送。
   位域定义UNION结构:  SOC_UPACC_LOCH_NUM_I1_UNION */
#define SOC_UPACC_LOCH_NUM_I1_ADDR(base)              ((base) + (0x0104))

/* 寄存器说明：MAC-i header0使能寄存器。用于指示当次封装的MAC-i/is PDU中是否包含header0参数。
   位域定义UNION结构:  SOC_UPACC_HEAD0_EN_I1_UNION */
#define SOC_UPACC_HEAD0_EN_I1_ADDR(base)              ((base) + (0x0108))

/* 寄存器说明：E_RNTI配置寄存器。用于配置MAC-i/is PDU header0中的E-RNTI参数。
   位域定义UNION结构:  SOC_UPACC_E_RNTI_I1_UNION */
#define SOC_UPACC_E_RNTI_I1_ADDR(base)                ((base) + (0x010C))

/* 寄存器说明：SI使能寄存器。用于指示当次封装的MAC-i/is PDU中是否包含SI参数。
   位域定义UNION结构:  SOC_UPACC_SI_EN_I1_UNION */
#define SOC_UPACC_SI_EN_I1_ADDR(base)                 ((base) + (0x0110))

/* 寄存器说明：SI数据配置寄存器。用于配置MAC-i/is PDU中的SI参数。
   位域定义UNION结构:  SOC_UPACC_SI_DATA_I1_UNION */
#define SOC_UPACC_SI_DATA_I1_ADDR(base)               ((base) + (0x0114))

/* 寄存器说明：padding长度配置寄存器。用于配置MAC-i/is PDU中的padding参数的长度。
   位域定义UNION结构:  SOC_UPACC_PADDING_LEN_I1_UNION */
#define SOC_UPACC_PADDING_LEN_I1_ADDR(base)           ((base) + (0x0118))

/* 寄存器说明：TSN长度配置寄存器。
   位域定义UNION结构:  SOC_UPACC_TSN_LEN_I1_UNION */
#define SOC_UPACC_TSN_LEN_I1_ADDR(base)               ((base) + (0x011C))

/* 寄存器说明：参数链表起始地址寄存器。用于配置参数链表在存储器中存放的起始地址。
   位域定义UNION结构:  SOC_UPACC_PARAM_ADDR_I1_UNION */
#define SOC_UPACC_PARAM_ADDR_I1_ADDR(base)            ((base) + (0x0120))

/* 寄存器说明：封装目标地址与搬移起始地址寄存器。用于配置当次MAC-i/is PDU封装的目的地址或搬运的起始地址。
   位域定义UNION结构:  SOC_UPACC_HARQ_BUF_ADDR_I1_UNION */
#define SOC_UPACC_HARQ_BUF_ADDR_I1_ADDR(base)         ((base) + (0x0124))

/* 寄存器说明：搬移目标地址寄存器。用于配置MAC-i/is PDU搬移到BBP编码缓存的目标地址。
   位域定义UNION结构:  SOC_UPACC_BBP_DEST_ADDR_I1_UNION */
#define SOC_UPACC_BBP_DEST_ADDR_I1_ADDR(base)         ((base) + (0x0128))

/* 寄存器说明：搬移数据大小寄存器。用于配置需要搬移到BBP的MAC-i/is PDU数据字节数。
   位域定义UNION结构:  SOC_UPACC_TRANS_NUM_I1_UNION */
#define SOC_UPACC_TRANS_NUM_I1_ADDR(base)             ((base) + (0x012C))

/* 寄存器说明：中断使能寄存器。用于配置各中断信号使能与否。
   位域定义UNION结构:  SOC_UPACC_INT_EN_I1_UNION */
#define SOC_UPACC_INT_EN_I1_ADDR(base)                ((base) + (0x0130))

/* 寄存器说明：原始中断状态寄存器。用于供CPU查询各中断信号在屏蔽前的状态。
   位域定义UNION结构:  SOC_UPACC_INT_RAW_I1_UNION */
#define SOC_UPACC_INT_RAW_I1_ADDR(base)               ((base) + (0x0134))

/* 寄存器说明：屏蔽后中断状态寄存器。用于供CPU查询各中断信号在屏蔽后的状态。
   位域定义UNION结构:  SOC_UPACC_INT_MASK_I1_UNION */
#define SOC_UPACC_INT_MASK_I1_ADDR(base)              ((base) + (0x0138))

/* 寄存器说明：中断状态清除寄存器。用于供CPU将各中断信号状态清零。
   位域定义UNION结构:  SOC_UPACC_INT_CLR_I1_UNION */
#define SOC_UPACC_INT_CLR_I1_ADDR(base)               ((base) + (0x013C))

/* 寄存器说明：总线ERROR地址寄存器。用于寄存Slave侧返回ERROR响应时的当前操作地址。
   位域定义UNION结构:  SOC_UPACC_BUS_ERROR_ADDR_I1_UNION */
#define SOC_UPACC_BUS_ERROR_ADDR_I1_ADDR(base)        ((base) + (0x0140))

/* 寄存器说明：MAC-i/is PDU封装及搬移模块使能寄存器。用于配置MAC-i/is PDU封装及搬移模块使能与否。
            UPACC_EN_I1为MAC-i/is PDU封装及搬移模块启动寄存器。用于配置MAC-i/is PDU封装及搬移模块使能与否。
            DSP配置完其他封装参数后，通过设置该寄存器的bit0为1启动MAC-i/is PDU封装及搬移操作。封装及搬移操作完成后，硬件可自动对该寄存器清零。DSP在不需启用MAC-i/is PDU封装及搬移模块时，也应将该寄存器的bit0置为0，以降低功耗。
            当软件参数配置出错，即INT_RAW_I1中的addri_error_raw_i1、loch_loch_num_error_raw_i1、dest_addr_error_raw_i1等任意一个有效时，硬件逻辑会对该寄存器的bit0自动清零，同时对寄存器MAC_ENCAP_START_I1的bit[1:0]进行清零，以复位UPACC内部的MAC-i/is  PDU封装及搬运模块，使其内部状态机返回IDLE态。
            当软件发现UPACC封装及搬运超时或异常时，可对该寄存器的bit0写0，将UPACC内部MAC-i/is PDU封装及搬运模块的状态机复位，使其返回到IDLE态。此时，软件还需对寄存器MAC_ENCAP_START_I1的bit[1:0]进行清零，以防止异常启动。
   位域定义UNION结构:  SOC_UPACC_EN_I1_UNION */
#define SOC_UPACC_EN_I1_ADDR(base)                    ((base) + (0x0144))

/* 寄存器说明：HARQ buffer写地址寄存器。
   位域定义UNION结构:  SOC_UPACC_HARQ_WRITE_ADDR_I1_UNION */
#define SOC_UPACC_HARQ_WRITE_ADDR_I1_ADDR(base)       ((base) + (0x0148))

/* 寄存器说明：BBP 编码缓存写地址寄存器。
   位域定义UNION结构:  SOC_UPACC_BBP_WRITE_ADDR_I1_UNION */
#define SOC_UPACC_BBP_WRITE_ADDR_I1_ADDR(base)        ((base) + (0x014C))

/* 寄存器说明：Mac-i PDU载波1源数据数组方式配置使能寄存器。
   位域定义UNION结构:  SOC_UPACC_PDUARY_EN_I1_UNION */
#define SOC_UPACC_PDUARY_EN_I1_ADDR(base)             ((base) + (0x0150))

/* 寄存器说明：启动方式配置寄存器。用于配置MAC_i/is PDU封装的启动方式。
            UPACCV200支持对MAC-i/is PDU 进行不封装只搬移和封装且搬移两种启动方式。
   位域定义UNION结构:  SOC_UPACC_MAC_ENCAP_START_I2_UNION */
#define SOC_UPACC_MAC_ENCAP_START_I2_ADDR(base)       ((base) + (0x0200))

/* 寄存器说明：逻辑信道个数寄存器。用于指示当次封装MAC-i/is PDU共需从几个逻辑信道中读取数据。若该寄存器内容配置为0，表示当前MAC-i/is PDU仅由SI组成，没有其他数据和参数，即为SI单独发送。
   位域定义UNION结构:  SOC_UPACC_LOCH_NUM_I2_UNION */
#define SOC_UPACC_LOCH_NUM_I2_ADDR(base)              ((base) + (0x0204))

/* 寄存器说明：MAC-i header0使能寄存器。用于指示当次封装的MAC-i/is PDU中是否包含header0参数。
   位域定义UNION结构:  SOC_UPACC_HEAD0_EN_I2_UNION */
#define SOC_UPACC_HEAD0_EN_I2_ADDR(base)              ((base) + (0x0208))

/* 寄存器说明：E_RNTI配置寄存器。用于配置MAC-i/is PDU header0中的E-RNTI参数。
   位域定义UNION结构:  SOC_UPACC_E_RNTI_I2_UNION */
#define SOC_UPACC_E_RNTI_I2_ADDR(base)                ((base) + (0x020C))

/* 寄存器说明：SI使能寄存器。用于指示当次封装的MAC-i/is PDU中是否包含SI参数。
   位域定义UNION结构:  SOC_UPACC_SI_EN_I2_UNION */
#define SOC_UPACC_SI_EN_I2_ADDR(base)                 ((base) + (0x0210))

/* 寄存器说明：SI数据配置寄存器。用于配置MAC-i/is PDU中的SI参数。
   位域定义UNION结构:  SOC_UPACC_SI_DATA_I2_UNION */
#define SOC_UPACC_SI_DATA_I2_ADDR(base)               ((base) + (0x0214))

/* 寄存器说明：padding长度配置寄存器。用于配置MAC-i/is PDU中的padding参数的长度。
   位域定义UNION结构:  SOC_UPACC_PADDING_LEN_I2_UNION */
#define SOC_UPACC_PADDING_LEN_I2_ADDR(base)           ((base) + (0x0218))

/* 寄存器说明：TSN长度配置寄存器。
   位域定义UNION结构:  SOC_UPACC_TSN_LEN_I2_UNION */
#define SOC_UPACC_TSN_LEN_I2_ADDR(base)               ((base) + (0x021C))

/* 寄存器说明：参数链表起始地址寄存器。用于配置参数链表在存储器中存放的起始地址。
   位域定义UNION结构:  SOC_UPACC_PARAM_ADDR_I2_UNION */
#define SOC_UPACC_PARAM_ADDR_I2_ADDR(base)            ((base) + (0x0220))

/* 寄存器说明：封装目标地址与搬移起始地址寄存器。用于配置当次MAC-i/is PDU封装的目的地址或搬运的起始地址。
   位域定义UNION结构:  SOC_UPACC_HARQ_BUF_ADDR_I2_UNION */
#define SOC_UPACC_HARQ_BUF_ADDR_I2_ADDR(base)         ((base) + (0x0224))

/* 寄存器说明：搬移目标地址寄存器。用于配置MAC-i/is PDU搬移到BBP编码缓存的目标地址。
   位域定义UNION结构:  SOC_UPACC_BBP_DEST_ADDR_I2_UNION */
#define SOC_UPACC_BBP_DEST_ADDR_I2_ADDR(base)         ((base) + (0x0228))

/* 寄存器说明：搬移数据大小寄存器。用于配置需要搬移到BBP的MAC-i/is PDU数据字节数。
   位域定义UNION结构:  SOC_UPACC_TRANS_NUM_I2_UNION */
#define SOC_UPACC_TRANS_NUM_I2_ADDR(base)             ((base) + (0x022C))

/* 寄存器说明：中断使能寄存器。用于配置各中断信号使能与否。
   位域定义UNION结构:  SOC_UPACC_INT_EN_I2_UNION */
#define SOC_UPACC_INT_EN_I2_ADDR(base)                ((base) + (0x0230))

/* 寄存器说明：原始中断状态寄存器。用于供CPU查询各中断信号在屏蔽前的状态。
   位域定义UNION结构:  SOC_UPACC_INT_RAW_I2_UNION */
#define SOC_UPACC_INT_RAW_I2_ADDR(base)               ((base) + (0x0234))

/* 寄存器说明：屏蔽后中断状态寄存器。用于供CPU查询各中断信号在屏蔽后的状态。
   位域定义UNION结构:  SOC_UPACC_INT_MASK_I2_UNION */
#define SOC_UPACC_INT_MASK_I2_ADDR(base)              ((base) + (0x0238))

/* 寄存器说明：中断状态清除寄存器。用于供CPU将各中断信号状态清零。
   位域定义UNION结构:  SOC_UPACC_INT_CLR_I2_UNION */
#define SOC_UPACC_INT_CLR_I2_ADDR(base)               ((base) + (0x023C))

/* 寄存器说明：总线ERROR地址寄存器。用于寄存Slave侧返回ERROR响应时的当前操作地址。
   位域定义UNION结构:  SOC_UPACC_BUS_ERROR_ADDR_I2_UNION */
#define SOC_UPACC_BUS_ERROR_ADDR_I2_ADDR(base)        ((base) + (0x0240))

/* 寄存器说明：MAC-i/is PDU封装及搬移模块使能寄存器。用于配置MAC-i/is PDU封装及搬移模块使能与否。
            UPACC_EN_I2为MAC-i/is PDU封装及搬移模块启动寄存器。用于配置MAC-i/is PDU封装及搬移模块使能与否。
            DSP配置完其他封装参数后，通过设置该寄存器的bit0为1启动MAC-i/is PDU封装及搬移操作。封装及搬移操作完成后，硬件可自动对该寄存器清零。DSP在不需启用MAC-i/is PDU封装及搬移模块时，也应将该寄存器的bit0置为0，以降低功耗。
            当软件参数配置出错，即INT_RAW_I2中的addri_error_raw_I2、loch_loch_num_error_raw_I2、dest_addr_error_raw_I2等任意一个有效时，硬件逻辑会对该寄存器的bit0自动清零，同时对寄存器MAC_ENCAP_START_I2的bit[1:0]进行清零，以复位UPACC内部的MAC-i/is  PDU封装及搬运模块，使其内部状态机返回IDLE态。
            当软件发现UPACC封装及搬运超时或异常时，可对该寄存器的bit0写0，将UPACC内部MAC-i/is PDU封装及搬运模块的状态机复位，使其返回到IDLE态。此时，软件还需对寄存器MAC_ENCAP_START_I2的bit[1:0]进行清零，以防止异常启动。
   位域定义UNION结构:  SOC_UPACC_EN_I2_UNION */
#define SOC_UPACC_EN_I2_ADDR(base)                    ((base) + (0x0244))

/* 寄存器说明：HARQ buffer写地址寄存器。
   位域定义UNION结构:  SOC_UPACC_HARQ_WRITE_ADDR_I2_UNION */
#define SOC_UPACC_HARQ_WRITE_ADDR_I2_ADDR(base)       ((base) + (0x0248))

/* 寄存器说明：BBP 编码缓存写地址寄存器。
   位域定义UNION结构:  SOC_UPACC_BBP_WRITE_ADDR_I2_UNION */
#define SOC_UPACC_BBP_WRITE_ADDR_I2_ADDR(base)        ((base) + (0x024C))

/* 寄存器说明：Mac-i PDU载波2源数据数组方式配置使能寄存器。
   位域定义UNION结构:  SOC_UPACC_PDUARY_EN_I2_UNION */
#define SOC_UPACC_PDUARY_EN_I2_ADDR(base)             ((base) + (0x0250))





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
                     (1/1) UPACC
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_UPACC_MAC_ENCAP_START_UNION
 结构说明  : MAC_ENCAP_START 寄存器结构定义。地址偏移量:0x0000，初值:0x00000000，宽度:32
 寄存器说明: 封装启动寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mac_encap_start : 2;  /* bit[0-1] : 封装启动寄存器。
                                                          00：不启动MAC-e PDU封装，也不启动BBP搬运； 
                                                          01：启动MAC-e PDU封装，但是封装完成后不启动BBP搬运；
                                                          10：不启动MAC-e PDU封装，只启动BBP搬运；
                                                          11：启动MAC-e PDU封装，并在封装完成后启动BBP搬运。
                                                          当MAC-e PDU封装完成后，即INT_RAW寄存器中encap_end_raw产生后，硬件逻辑会对mac_encap_start信号的bit0进行自清零。
                                                          当BBP搬运完成后，即INT_RAW寄存器中trans_end_raw产生后，硬件逻辑会对mac_encap_start信号中的bit1进行自清零。 */
        unsigned int  reserved        : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_UPACC_MAC_ENCAP_START_UNION;
#endif
#define SOC_UPACC_MAC_ENCAP_START_mac_encap_start_START  (0)
#define SOC_UPACC_MAC_ENCAP_START_mac_encap_start_END    (1)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH_NUM_UNION
 结构说明  : LOCH_NUM 寄存器结构定义。地址偏移量:0x0004，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道个数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch_num : 4;  /* bit[0-3] : 逻辑信道个数寄存器。
                                                   用于指示当前待封装MAC-e PDU从几个逻辑信道中提取数据。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_UPACC_LOCH_NUM_UNION;
#endif
#define SOC_UPACC_LOCH_NUM_loch_num_START  (0)
#define SOC_UPACC_LOCH_NUM_loch_num_END    (3)


/*****************************************************************************
 结构名    : SOC_UPACC_DDI0_EN_UNION
 结构说明  : DDI0_EN 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: DDI0使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ddi0_en  : 1;  /* bit[0]   : DDI0使能寄存器。
                                                   0：在MAC-e PDU中DDI0处无需写入6’b111111；
                                                   1：在MAC-e PDU中DDI0处需要写入6’b111111。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_UPACC_DDI0_EN_UNION;
#endif
#define SOC_UPACC_DDI0_EN_ddi0_en_START   (0)
#define SOC_UPACC_DDI0_EN_ddi0_en_END     (0)


/*****************************************************************************
 结构名    : SOC_UPACC_SI_EN_UNION
 结构说明  : SI_EN 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: SI使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  si_en    : 1;  /* bit[0]   : SI使能寄存器。
                                                   0：在MAC-e PDU中无需写入SI参数； 
                                                   1：在MAC-e PDU中需要写入SI参数。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_UPACC_SI_EN_UNION;
#endif
#define SOC_UPACC_SI_EN_si_en_START     (0)
#define SOC_UPACC_SI_EN_si_en_END       (0)


/*****************************************************************************
 结构名    : SOC_UPACC_SI_DATA_UNION
 结构说明  : SI_DATA 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: SI数据寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  si_hlid_data : 4;  /* bit[0-3]  : 配置MAC-e PDU中SI HLID参数。 */
        unsigned int  si_hlbs_data : 4;  /* bit[4-7]  : 配置MAC-e PDU中SI HLBS参数。 */
        unsigned int  si_tebs_data : 5;  /* bit[8-12] : 配置MAC-e PDU中SI TEBS参数。 */
        unsigned int  si_uph_data  : 5;  /* bit[13-17]: 配置MAC-e PDU中SI UPH参数。 */
        unsigned int  reserved     : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_UPACC_SI_DATA_UNION;
#endif
#define SOC_UPACC_SI_DATA_si_hlid_data_START  (0)
#define SOC_UPACC_SI_DATA_si_hlid_data_END    (3)
#define SOC_UPACC_SI_DATA_si_hlbs_data_START  (4)
#define SOC_UPACC_SI_DATA_si_hlbs_data_END    (7)
#define SOC_UPACC_SI_DATA_si_tebs_data_START  (8)
#define SOC_UPACC_SI_DATA_si_tebs_data_END    (12)
#define SOC_UPACC_SI_DATA_si_uph_data_START   (13)
#define SOC_UPACC_SI_DATA_si_uph_data_END     (17)


/*****************************************************************************
 结构名    : SOC_UPACC_PADDING_LENGTH_UNION
 结构说明  : PADDING_LENGTH 寄存器结构定义。地址偏移量:0x0014，初值:0x00000000，宽度:32
 寄存器说明: padding长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  padding_length : 16; /* bit[0-15] : padding长度寄存器。
                                                          用于配置在MAC-e PDU中padding参数的长度，以bit为单位。 */
        unsigned int  reserved       : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_UPACC_PADDING_LENGTH_UNION;
#endif
#define SOC_UPACC_PADDING_LENGTH_padding_length_START  (0)
#define SOC_UPACC_PADDING_LENGTH_padding_length_END    (15)


/*****************************************************************************
 结构名    : SOC_UPACC_ENCAP_TRANS_ADDR_UNION
 结构说明  : ENCAP_TRANS_ADDR 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: 封装目标地址及搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_trans_addr : 32; /* bit[0-31]: 封装目标地址寄存器。
                                                           用于配置MAC-e PDU的目标地址。以byte为单位，但是该地址低三bit必须为0，即[2:0]=3’b000，即以double word地址进行配置。 */
    } reg;
} SOC_UPACC_ENCAP_TRANS_ADDR_UNION;
#endif
#define SOC_UPACC_ENCAP_TRANS_ADDR_encap_trans_addr_START  (0)
#define SOC_UPACC_ENCAP_TRANS_ADDR_encap_trans_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_TRANS_NUM_UNION
 结构说明  : TRANS_NUM 寄存器结构定义。地址偏移量:0x001C，初值:0x00000000，宽度:32
 寄存器说明: 搬运个数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trans_num : 16; /* bit[0-15] : 搬运个数配置寄存器。
                                                     用于配置MAC-e PDU需要搬运到BBP的数据的byte数。
                                                     最大允许配置值为16'hFFFC。 */
        unsigned int  reserved  : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_UPACC_TRANS_NUM_UNION;
#endif
#define SOC_UPACC_TRANS_NUM_trans_num_START  (0)
#define SOC_UPACC_TRANS_NUM_trans_num_END    (15)


/*****************************************************************************
 结构名    : SOC_UPACC_BBP_DEST_ADDR_UNION
 结构说明  : BBP_DEST_ADDR 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: 搬运目标地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bbp_dest_addr : 32; /* bit[0-31]: 搬运目标地址寄存器。
                                                        用于配置MAC-e PDU需要搬运到BBP的数据的目标地址。以byte为单位，但是该地址低三bit必须为0，即[2:0]=3’b000，即以double word地址进行配置。 */
    } reg;
} SOC_UPACC_BBP_DEST_ADDR_UNION;
#endif
#define SOC_UPACC_BBP_DEST_ADDR_bbp_dest_addr_START  (0)
#define SOC_UPACC_BBP_DEST_ADDR_bbp_dest_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_EN_UNION
 结构说明  : INT_EN 寄存器结构定义。地址偏移量:0x0024，初值:0x00000000，宽度:32
 寄存器说明: 中断使能寄存器。Bit值为0表示中断禁止；值为1表示中断使能。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_en          : 1;  /* bit[0]    : MAC-e PDU封装完成中断使能寄存器。 */
        unsigned int  trans_end_en          : 1;  /* bit[1]    : MAC-e PDU搬运到BBP完成中断使能寄存器。 */
        unsigned int  addr1_error_en        : 1;  /* bit[2]    : MAC-e PDU封装逻辑通道1读地址配置错误中断使能寄存器。 */
        unsigned int  addr2_error_en        : 1;  /* bit[3]    : MAC-e PDU封装逻辑通道2读地址配置错误中断使能寄存器。 */
        unsigned int  addr3_error_en        : 1;  /* bit[4]    : MAC-e PDU封装逻辑通道3读地址配置错误中断使能寄存器。 */
        unsigned int  addr4_error_en        : 1;  /* bit[5]    : MAC-e PDU封装逻辑通道4读地址配置错误中断使能寄存器。 */
        unsigned int  addr5_error_en        : 1;  /* bit[6]    : MAC-e PDU封装逻辑通道5读地址配置错误中断使能寄存器。 */
        unsigned int  addr6_error_en        : 1;  /* bit[7]    : MAC-e PDU封装逻辑通道6读地址配置错误中断使能寄存器。 */
        unsigned int  addr7_error_en        : 1;  /* bit[8]    : MAC-e PDU封装逻辑通道7读地址配置错误中断使能寄存器。 */
        unsigned int  addr8_error_en        : 1;  /* bit[9]    : MAC-e PDU封装逻辑通道8读地址配置错误中断使能寄存器。 */
        unsigned int  num_error_en          : 1;  /* bit[10]   : MAC-e PDU封装逻辑通道个数配置错误中断使能寄存器。 */
        unsigned int  encap_addr_error_en   : 1;  /* bit[11]   : MAC-e PDU封装目标地址及搬运起始地址配置错误中断使能位。 */
        unsigned int  dest_addr_error_en    : 1;  /* bit[12]   : MAC-e PDU搬运目标地址配置错误中断使能位。 */
        unsigned int  bus_error_en          : 1;  /* bit[13]   : 模块所访问的Slave侧返回ERROR RESP中断使能寄存器。 */
        unsigned int  dsp_int_en            : 1;  /* bit[14]   : 模块各组合中断发送给DSP使能寄存器。 */
        unsigned int  arm_int_en            : 1;  /* bit[15]   : 模块各组合中断发送给ARM使能寄存器。 */
        unsigned int  pduary_addr1_error_en : 1;  /* bit[16]   : MAC-e PDU封装逻辑通道1源数据数组首地址配置错误中断使能位。 */
        unsigned int  pduary_addr2_error_en : 1;  /* bit[17]   : MAC-e PDU封装逻辑通道2源数据数组首地址配置错误中断使能位。 */
        unsigned int  pduary_addr3_error_en : 1;  /* bit[18]   : MAC-e PDU封装逻辑通道3源数据数组首地址配置错误中断使能位。 */
        unsigned int  pduary_addr4_error_en : 1;  /* bit[19]   : MAC-e PDU封装逻辑通道4源数据数组首地址配置错误中断使能位。 */
        unsigned int  pduary_addr5_error_en : 1;  /* bit[20]   : MAC-e PDU封装逻辑通道5源数据数组首地址配置错误中断使能位。 */
        unsigned int  pduary_addr6_error_en : 1;  /* bit[21]   : MAC-e PDU封装逻辑通道6源数据数组首地址配置错误中断使能位。 */
        unsigned int  pduary_addr7_error_en : 1;  /* bit[22]   : MAC-e PDU封装逻辑通道7源数据数组首地址配置错误中断使能位。 */
        unsigned int  pduary_addr8_error_en : 1;  /* bit[23]   : MAC-e PDU封装逻辑通道8源数据数组首地址配置错误中断使能位。 */
        unsigned int  reserved              : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_UPACC_INT_EN_UNION;
#endif
#define SOC_UPACC_INT_EN_encap_end_en_START           (0)
#define SOC_UPACC_INT_EN_encap_end_en_END             (0)
#define SOC_UPACC_INT_EN_trans_end_en_START           (1)
#define SOC_UPACC_INT_EN_trans_end_en_END             (1)
#define SOC_UPACC_INT_EN_addr1_error_en_START         (2)
#define SOC_UPACC_INT_EN_addr1_error_en_END           (2)
#define SOC_UPACC_INT_EN_addr2_error_en_START         (3)
#define SOC_UPACC_INT_EN_addr2_error_en_END           (3)
#define SOC_UPACC_INT_EN_addr3_error_en_START         (4)
#define SOC_UPACC_INT_EN_addr3_error_en_END           (4)
#define SOC_UPACC_INT_EN_addr4_error_en_START         (5)
#define SOC_UPACC_INT_EN_addr4_error_en_END           (5)
#define SOC_UPACC_INT_EN_addr5_error_en_START         (6)
#define SOC_UPACC_INT_EN_addr5_error_en_END           (6)
#define SOC_UPACC_INT_EN_addr6_error_en_START         (7)
#define SOC_UPACC_INT_EN_addr6_error_en_END           (7)
#define SOC_UPACC_INT_EN_addr7_error_en_START         (8)
#define SOC_UPACC_INT_EN_addr7_error_en_END           (8)
#define SOC_UPACC_INT_EN_addr8_error_en_START         (9)
#define SOC_UPACC_INT_EN_addr8_error_en_END           (9)
#define SOC_UPACC_INT_EN_num_error_en_START           (10)
#define SOC_UPACC_INT_EN_num_error_en_END             (10)
#define SOC_UPACC_INT_EN_encap_addr_error_en_START    (11)
#define SOC_UPACC_INT_EN_encap_addr_error_en_END      (11)
#define SOC_UPACC_INT_EN_dest_addr_error_en_START     (12)
#define SOC_UPACC_INT_EN_dest_addr_error_en_END       (12)
#define SOC_UPACC_INT_EN_bus_error_en_START           (13)
#define SOC_UPACC_INT_EN_bus_error_en_END             (13)
#define SOC_UPACC_INT_EN_dsp_int_en_START             (14)
#define SOC_UPACC_INT_EN_dsp_int_en_END               (14)
#define SOC_UPACC_INT_EN_arm_int_en_START             (15)
#define SOC_UPACC_INT_EN_arm_int_en_END               (15)
#define SOC_UPACC_INT_EN_pduary_addr1_error_en_START  (16)
#define SOC_UPACC_INT_EN_pduary_addr1_error_en_END    (16)
#define SOC_UPACC_INT_EN_pduary_addr2_error_en_START  (17)
#define SOC_UPACC_INT_EN_pduary_addr2_error_en_END    (17)
#define SOC_UPACC_INT_EN_pduary_addr3_error_en_START  (18)
#define SOC_UPACC_INT_EN_pduary_addr3_error_en_END    (18)
#define SOC_UPACC_INT_EN_pduary_addr4_error_en_START  (19)
#define SOC_UPACC_INT_EN_pduary_addr4_error_en_END    (19)
#define SOC_UPACC_INT_EN_pduary_addr5_error_en_START  (20)
#define SOC_UPACC_INT_EN_pduary_addr5_error_en_END    (20)
#define SOC_UPACC_INT_EN_pduary_addr6_error_en_START  (21)
#define SOC_UPACC_INT_EN_pduary_addr6_error_en_END    (21)
#define SOC_UPACC_INT_EN_pduary_addr7_error_en_START  (22)
#define SOC_UPACC_INT_EN_pduary_addr7_error_en_END    (22)
#define SOC_UPACC_INT_EN_pduary_addr8_error_en_START  (23)
#define SOC_UPACC_INT_EN_pduary_addr8_error_en_END    (23)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_RAW_UNION
 结构说明  : INT_RAW 寄存器结构定义。地址偏移量:0x0028，初值:0x00000000，宽度:32
 寄存器说明: 原始中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_raw          : 1;  /* bit[0]    : MAC-e PDU封装完成原始中断寄存器。高有效。 */
        unsigned int  trans_end_raw          : 1;  /* bit[1]    : MAC-e PDU搬运到BBP完成原始中断寄存器。高有效。 */
        unsigned int  addr1_error_raw        : 1;  /* bit[2]    : MAC-e PDU封装逻辑通道1读地址配置错误原始中断寄存器。高有效。 */
        unsigned int  addr2_error_raw        : 1;  /* bit[3]    : MAC-e PDU封装逻辑通道2读地址配置错误原始中断寄存器。高有效。 */
        unsigned int  addr3_error_raw        : 1;  /* bit[4]    : MAC-e PDU封装逻辑通道3读地址配置错误原始中断寄存器。高有效。 */
        unsigned int  addr4_error_raw        : 1;  /* bit[5]    : MAC-e PDU封装逻辑通道4读地址配置错误原始中断寄存器。高有效。 */
        unsigned int  addr5_error_raw        : 1;  /* bit[6]    : MAC-e PDU封装逻辑通道5读地址配置错误原始中断寄存器。高有效。 */
        unsigned int  addr6_error_raw        : 1;  /* bit[7]    : MAC-e PDU封装逻辑通道6读地址配置错误原始中断寄存器。高有效。 */
        unsigned int  addr7_error_raw        : 1;  /* bit[8]    : MAC-e PDU封装逻辑通道7读地址配置错误原始中断寄存器。高有效。 */
        unsigned int  addr8_error_raw        : 1;  /* bit[9]    : MAC-e PDU封装逻辑通道8读地址配置错误原始中断寄存器。高有效。 */
        unsigned int  num_error_raw          : 1;  /* bit[10]   : MAC-e PDU封装逻辑通道个数配置错误原始中断寄存器。高有效。 */
        unsigned int  encap_addr_error_raw   : 1;  /* bit[11]   : MAC-e PDU封装目标地址及搬运起始地址配置错误原始中断寄存器。高有效。 */
        unsigned int  dest_addr_error_raw    : 1;  /* bit[12]   : MAC-e PDU搬运目标地址配置错误原始中断寄存器。高有效。 */
        unsigned int  bus_error_raw          : 1;  /* bit[13]   : 模块所访问的Slave侧返回ERROR RESP原始中断寄存器。高有效。 */
        unsigned int  dsp_int_raw            : 1;  /* bit[14]   : 模块各组合中断发送给DSP原始中断寄存器。高有效。 */
        unsigned int  arm_int_raw            : 1;  /* bit[15]   : 模块各组合中断发送给ARM原始中断寄存器。高有效。 */
        unsigned int  pduary_addr1_error_raw : 1;  /* bit[16]   : MAC-e PDU封装逻辑通道1源数据数组首地址配置错误原始中断寄存器。高有效。 */
        unsigned int  pduary_addr2_error_raw : 1;  /* bit[17]   : MAC-e PDU封装逻辑通道2源数据数组首地址配置错误原始中断寄存器。高有效。 */
        unsigned int  pduary_addr3_error_raw : 1;  /* bit[18]   : MAC-e PDU封装逻辑通道3源数据数组首地址配置错误原始中断寄存器。高有效。 */
        unsigned int  pduary_addr4_error_raw : 1;  /* bit[19]   : MAC-e PDU封装逻辑通道4源数据数组首地址配置错误原始中断寄存器。高有效。 */
        unsigned int  pduary_addr5_error_raw : 1;  /* bit[20]   : MAC-e PDU封装逻辑通道5源数据数组首地址配置错误原始中断寄存器。高有效。 */
        unsigned int  pduary_addr6_error_raw : 1;  /* bit[21]   : MAC-e PDU封装逻辑通道6源数据数组首地址配置错误原始中断寄存器。高有效。 */
        unsigned int  pduary_addr7_error_raw : 1;  /* bit[22]   : MAC-e PDU封装逻辑通道7源数据数组首地址配置错误原始中断寄存器。高有效。 */
        unsigned int  pduary_addr8_error_raw : 1;  /* bit[23]   : MAC-e PDU封装逻辑通道8源数据数组首地址配置错误原始中断寄存器。高有效。 */
        unsigned int  reserved               : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_UPACC_INT_RAW_UNION;
#endif
#define SOC_UPACC_INT_RAW_encap_end_raw_START           (0)
#define SOC_UPACC_INT_RAW_encap_end_raw_END             (0)
#define SOC_UPACC_INT_RAW_trans_end_raw_START           (1)
#define SOC_UPACC_INT_RAW_trans_end_raw_END             (1)
#define SOC_UPACC_INT_RAW_addr1_error_raw_START         (2)
#define SOC_UPACC_INT_RAW_addr1_error_raw_END           (2)
#define SOC_UPACC_INT_RAW_addr2_error_raw_START         (3)
#define SOC_UPACC_INT_RAW_addr2_error_raw_END           (3)
#define SOC_UPACC_INT_RAW_addr3_error_raw_START         (4)
#define SOC_UPACC_INT_RAW_addr3_error_raw_END           (4)
#define SOC_UPACC_INT_RAW_addr4_error_raw_START         (5)
#define SOC_UPACC_INT_RAW_addr4_error_raw_END           (5)
#define SOC_UPACC_INT_RAW_addr5_error_raw_START         (6)
#define SOC_UPACC_INT_RAW_addr5_error_raw_END           (6)
#define SOC_UPACC_INT_RAW_addr6_error_raw_START         (7)
#define SOC_UPACC_INT_RAW_addr6_error_raw_END           (7)
#define SOC_UPACC_INT_RAW_addr7_error_raw_START         (8)
#define SOC_UPACC_INT_RAW_addr7_error_raw_END           (8)
#define SOC_UPACC_INT_RAW_addr8_error_raw_START         (9)
#define SOC_UPACC_INT_RAW_addr8_error_raw_END           (9)
#define SOC_UPACC_INT_RAW_num_error_raw_START           (10)
#define SOC_UPACC_INT_RAW_num_error_raw_END             (10)
#define SOC_UPACC_INT_RAW_encap_addr_error_raw_START    (11)
#define SOC_UPACC_INT_RAW_encap_addr_error_raw_END      (11)
#define SOC_UPACC_INT_RAW_dest_addr_error_raw_START     (12)
#define SOC_UPACC_INT_RAW_dest_addr_error_raw_END       (12)
#define SOC_UPACC_INT_RAW_bus_error_raw_START           (13)
#define SOC_UPACC_INT_RAW_bus_error_raw_END             (13)
#define SOC_UPACC_INT_RAW_dsp_int_raw_START             (14)
#define SOC_UPACC_INT_RAW_dsp_int_raw_END               (14)
#define SOC_UPACC_INT_RAW_arm_int_raw_START             (15)
#define SOC_UPACC_INT_RAW_arm_int_raw_END               (15)
#define SOC_UPACC_INT_RAW_pduary_addr1_error_raw_START  (16)
#define SOC_UPACC_INT_RAW_pduary_addr1_error_raw_END    (16)
#define SOC_UPACC_INT_RAW_pduary_addr2_error_raw_START  (17)
#define SOC_UPACC_INT_RAW_pduary_addr2_error_raw_END    (17)
#define SOC_UPACC_INT_RAW_pduary_addr3_error_raw_START  (18)
#define SOC_UPACC_INT_RAW_pduary_addr3_error_raw_END    (18)
#define SOC_UPACC_INT_RAW_pduary_addr4_error_raw_START  (19)
#define SOC_UPACC_INT_RAW_pduary_addr4_error_raw_END    (19)
#define SOC_UPACC_INT_RAW_pduary_addr5_error_raw_START  (20)
#define SOC_UPACC_INT_RAW_pduary_addr5_error_raw_END    (20)
#define SOC_UPACC_INT_RAW_pduary_addr6_error_raw_START  (21)
#define SOC_UPACC_INT_RAW_pduary_addr6_error_raw_END    (21)
#define SOC_UPACC_INT_RAW_pduary_addr7_error_raw_START  (22)
#define SOC_UPACC_INT_RAW_pduary_addr7_error_raw_END    (22)
#define SOC_UPACC_INT_RAW_pduary_addr8_error_raw_START  (23)
#define SOC_UPACC_INT_RAW_pduary_addr8_error_raw_END    (23)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_MASK_UNION
 结构说明  : INT_MASK 寄存器结构定义。地址偏移量:0x002C，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_mask          : 1;  /* bit[0]    : MAC-e PDU封装完成屏蔽后中断寄存器。高有效。 */
        unsigned int  trans_end_mask          : 1;  /* bit[1]    : MAC-e PDU搬运到BBP完成屏蔽后中断寄存器。高有效。 */
        unsigned int  addr1_error_mask        : 1;  /* bit[2]    : MAC-e PDU封装逻辑通道1读地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  addr2_error_mask        : 1;  /* bit[3]    : MAC-e PDU封装逻辑通道2读地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  addr3_error_mask        : 1;  /* bit[4]    : MAC-e PDU封装逻辑通道3读地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  addr4_error_mask        : 1;  /* bit[5]    : MAC-e PDU封装逻辑通道4读地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  addr5_error_mask        : 1;  /* bit[6]    : MAC-e PDU封装逻辑通道5读地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  addr6_error_mask        : 1;  /* bit[7]    : MAC-e PDU封装逻辑通道6读地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  addr7_error_mask        : 1;  /* bit[8]    : MAC-e PDU封装逻辑通道7读地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  addr8_error_mask        : 1;  /* bit[9]    : MAC-e PDU封装逻辑通道8读地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  num_error_mask          : 1;  /* bit[10]   : MAC-e PDU封装逻辑通道个数配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  encap_addr_error_mask   : 1;  /* bit[11]   : MAC-e PDU封装目标地址及搬运起始地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  dest_addr_error_mask    : 1;  /* bit[12]   : MAC-e PDU搬运目标地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  bus_error_mask          : 1;  /* bit[13]   : 模块所访问的Slave侧返回ERROR RESP屏蔽后中断寄存器。高有效。 */
        unsigned int  dsp_int_mask            : 1;  /* bit[14]   : 模块各组合中断发送给DSP屏蔽后中断寄存器。高有效。 */
        unsigned int  arm_int_mask            : 1;  /* bit[15]   : 模块各组合中断发送给ARM屏蔽后中断寄存器。高有效。 */
        unsigned int  pduary_addr1_error_mask : 1;  /* bit[16]   : MAC-e PDU封装逻辑通道1源数据数组首地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  pduary_addr2_error_mask : 1;  /* bit[17]   : MAC-e PDU封装逻辑通道2源数据数组首地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  pduary_addr3_error_mask : 1;  /* bit[18]   : MAC-e PDU封装逻辑通道3源数据数组首地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  pduary_addr4_error_mask : 1;  /* bit[19]   : MAC-e PDU封装逻辑通道4源数据数组首地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  pduary_addr5_error_mask : 1;  /* bit[20]   : MAC-e PDU封装逻辑通道5源数据数组首地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  pduary_addr6_error_mask : 1;  /* bit[21]   : MAC-e PDU封装逻辑通道6源数据数组首地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  pduary_addr7_error_mask : 1;  /* bit[22]   : MAC-e PDU封装逻辑通道7源数据数组首地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  pduary_addr8_error_mask : 1;  /* bit[23]   : MAC-e PDU封装逻辑通道8源数据数组首地址配置错误屏蔽后中断寄存器。高有效。 */
        unsigned int  reserved                : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_UPACC_INT_MASK_UNION;
#endif
#define SOC_UPACC_INT_MASK_encap_end_mask_START           (0)
#define SOC_UPACC_INT_MASK_encap_end_mask_END             (0)
#define SOC_UPACC_INT_MASK_trans_end_mask_START           (1)
#define SOC_UPACC_INT_MASK_trans_end_mask_END             (1)
#define SOC_UPACC_INT_MASK_addr1_error_mask_START         (2)
#define SOC_UPACC_INT_MASK_addr1_error_mask_END           (2)
#define SOC_UPACC_INT_MASK_addr2_error_mask_START         (3)
#define SOC_UPACC_INT_MASK_addr2_error_mask_END           (3)
#define SOC_UPACC_INT_MASK_addr3_error_mask_START         (4)
#define SOC_UPACC_INT_MASK_addr3_error_mask_END           (4)
#define SOC_UPACC_INT_MASK_addr4_error_mask_START         (5)
#define SOC_UPACC_INT_MASK_addr4_error_mask_END           (5)
#define SOC_UPACC_INT_MASK_addr5_error_mask_START         (6)
#define SOC_UPACC_INT_MASK_addr5_error_mask_END           (6)
#define SOC_UPACC_INT_MASK_addr6_error_mask_START         (7)
#define SOC_UPACC_INT_MASK_addr6_error_mask_END           (7)
#define SOC_UPACC_INT_MASK_addr7_error_mask_START         (8)
#define SOC_UPACC_INT_MASK_addr7_error_mask_END           (8)
#define SOC_UPACC_INT_MASK_addr8_error_mask_START         (9)
#define SOC_UPACC_INT_MASK_addr8_error_mask_END           (9)
#define SOC_UPACC_INT_MASK_num_error_mask_START           (10)
#define SOC_UPACC_INT_MASK_num_error_mask_END             (10)
#define SOC_UPACC_INT_MASK_encap_addr_error_mask_START    (11)
#define SOC_UPACC_INT_MASK_encap_addr_error_mask_END      (11)
#define SOC_UPACC_INT_MASK_dest_addr_error_mask_START     (12)
#define SOC_UPACC_INT_MASK_dest_addr_error_mask_END       (12)
#define SOC_UPACC_INT_MASK_bus_error_mask_START           (13)
#define SOC_UPACC_INT_MASK_bus_error_mask_END             (13)
#define SOC_UPACC_INT_MASK_dsp_int_mask_START             (14)
#define SOC_UPACC_INT_MASK_dsp_int_mask_END               (14)
#define SOC_UPACC_INT_MASK_arm_int_mask_START             (15)
#define SOC_UPACC_INT_MASK_arm_int_mask_END               (15)
#define SOC_UPACC_INT_MASK_pduary_addr1_error_mask_START  (16)
#define SOC_UPACC_INT_MASK_pduary_addr1_error_mask_END    (16)
#define SOC_UPACC_INT_MASK_pduary_addr2_error_mask_START  (17)
#define SOC_UPACC_INT_MASK_pduary_addr2_error_mask_END    (17)
#define SOC_UPACC_INT_MASK_pduary_addr3_error_mask_START  (18)
#define SOC_UPACC_INT_MASK_pduary_addr3_error_mask_END    (18)
#define SOC_UPACC_INT_MASK_pduary_addr4_error_mask_START  (19)
#define SOC_UPACC_INT_MASK_pduary_addr4_error_mask_END    (19)
#define SOC_UPACC_INT_MASK_pduary_addr5_error_mask_START  (20)
#define SOC_UPACC_INT_MASK_pduary_addr5_error_mask_END    (20)
#define SOC_UPACC_INT_MASK_pduary_addr6_error_mask_START  (21)
#define SOC_UPACC_INT_MASK_pduary_addr6_error_mask_END    (21)
#define SOC_UPACC_INT_MASK_pduary_addr7_error_mask_START  (22)
#define SOC_UPACC_INT_MASK_pduary_addr7_error_mask_END    (22)
#define SOC_UPACC_INT_MASK_pduary_addr8_error_mask_START  (23)
#define SOC_UPACC_INT_MASK_pduary_addr8_error_mask_END    (23)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_CLR_UNION
 结构说明  : INT_CLR 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: 中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_clr          : 1;  /* bit[0]    : MAC-e PDU封装完成中断清除寄存器。CPU写1清零。 */
        unsigned int  trans_end_clr          : 1;  /* bit[1]    : MAC-e PDU搬运到BBP完成中断清除寄存器。CPU写1清零。 */
        unsigned int  addr1_error_clr        : 1;  /* bit[2]    : MAC-e PDU封装逻辑通道1读地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  addr2_error_clr        : 1;  /* bit[3]    : MAC-e PDU封装逻辑通道2读地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  addr3_error_clr        : 1;  /* bit[4]    : MAC-e PDU封装逻辑通道3读地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  addr4_error_clr        : 1;  /* bit[5]    : MAC-e PDU封装逻辑通道4读地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  addr5_error_clr        : 1;  /* bit[6]    : MAC-e PDU封装逻辑通道5读地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  addr6_error_clr        : 1;  /* bit[7]    : MAC-e PDU封装逻辑通道6读地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  addr7_error_clr        : 1;  /* bit[8]    : MAC-e PDU封装逻辑通道7读地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  addr8_error_clr        : 1;  /* bit[9]    : MAC-e PDU封装逻辑通道8读地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  num_error_clr          : 1;  /* bit[10]   : MAC-e PDU封装逻辑通道个数配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  encap_addr_error_clr   : 1;  /* bit[11]   : MAC-e PDU封装目标地址及搬运起始地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  dest_addr_error_clr    : 1;  /* bit[12]   : MAC-e PDU搬运目标地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  bus_error_clr          : 1;  /* bit[13]   : 模块所访问的Slave侧返回ERROR RESP中断清除寄存器。CPU写1清零。 */
        unsigned int  reserved_0             : 1;  /* bit[14]   : 保留。 */
        unsigned int  reserved_1             : 1;  /* bit[15]   : 保留。 */
        unsigned int  pduary_addr1_error_clr : 1;  /* bit[16]   : MAC-e PDU封装逻辑通道1源数据数组首地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  pduary_addr2_error_clr : 1;  /* bit[17]   : MAC-e PDU封装逻辑通道2源数据数组首地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  pduary_addr3_error_clr : 1;  /* bit[18]   : MAC-e PDU封装逻辑通道3源数据数组首地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  pduary_addr4_error_clr : 1;  /* bit[19]   : MAC-e PDU封装逻辑通道4源数据数组首地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  pduary_addr5_error_clr : 1;  /* bit[20]   : MAC-e PDU封装逻辑通道5源数据数组首地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  pduary_addr6_error_clr : 1;  /* bit[21]   : MAC-e PDU封装逻辑通道6源数据数组首地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  pduary_addr7_error_clr : 1;  /* bit[22]   : MAC-e PDU封装逻辑通道7源数据数组首地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  pduary_addr8_error_clr : 1;  /* bit[23]   : MAC-e PDU封装逻辑通道8源数据数组首地址配置错误中断清除寄存器。CPU写1清零。 */
        unsigned int  reserved_2             : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_UPACC_INT_CLR_UNION;
#endif
#define SOC_UPACC_INT_CLR_encap_end_clr_START           (0)
#define SOC_UPACC_INT_CLR_encap_end_clr_END             (0)
#define SOC_UPACC_INT_CLR_trans_end_clr_START           (1)
#define SOC_UPACC_INT_CLR_trans_end_clr_END             (1)
#define SOC_UPACC_INT_CLR_addr1_error_clr_START         (2)
#define SOC_UPACC_INT_CLR_addr1_error_clr_END           (2)
#define SOC_UPACC_INT_CLR_addr2_error_clr_START         (3)
#define SOC_UPACC_INT_CLR_addr2_error_clr_END           (3)
#define SOC_UPACC_INT_CLR_addr3_error_clr_START         (4)
#define SOC_UPACC_INT_CLR_addr3_error_clr_END           (4)
#define SOC_UPACC_INT_CLR_addr4_error_clr_START         (5)
#define SOC_UPACC_INT_CLR_addr4_error_clr_END           (5)
#define SOC_UPACC_INT_CLR_addr5_error_clr_START         (6)
#define SOC_UPACC_INT_CLR_addr5_error_clr_END           (6)
#define SOC_UPACC_INT_CLR_addr6_error_clr_START         (7)
#define SOC_UPACC_INT_CLR_addr6_error_clr_END           (7)
#define SOC_UPACC_INT_CLR_addr7_error_clr_START         (8)
#define SOC_UPACC_INT_CLR_addr7_error_clr_END           (8)
#define SOC_UPACC_INT_CLR_addr8_error_clr_START         (9)
#define SOC_UPACC_INT_CLR_addr8_error_clr_END           (9)
#define SOC_UPACC_INT_CLR_num_error_clr_START           (10)
#define SOC_UPACC_INT_CLR_num_error_clr_END             (10)
#define SOC_UPACC_INT_CLR_encap_addr_error_clr_START    (11)
#define SOC_UPACC_INT_CLR_encap_addr_error_clr_END      (11)
#define SOC_UPACC_INT_CLR_dest_addr_error_clr_START     (12)
#define SOC_UPACC_INT_CLR_dest_addr_error_clr_END       (12)
#define SOC_UPACC_INT_CLR_bus_error_clr_START           (13)
#define SOC_UPACC_INT_CLR_bus_error_clr_END             (13)
#define SOC_UPACC_INT_CLR_pduary_addr1_error_clr_START  (16)
#define SOC_UPACC_INT_CLR_pduary_addr1_error_clr_END    (16)
#define SOC_UPACC_INT_CLR_pduary_addr2_error_clr_START  (17)
#define SOC_UPACC_INT_CLR_pduary_addr2_error_clr_END    (17)
#define SOC_UPACC_INT_CLR_pduary_addr3_error_clr_START  (18)
#define SOC_UPACC_INT_CLR_pduary_addr3_error_clr_END    (18)
#define SOC_UPACC_INT_CLR_pduary_addr4_error_clr_START  (19)
#define SOC_UPACC_INT_CLR_pduary_addr4_error_clr_END    (19)
#define SOC_UPACC_INT_CLR_pduary_addr5_error_clr_START  (20)
#define SOC_UPACC_INT_CLR_pduary_addr5_error_clr_END    (20)
#define SOC_UPACC_INT_CLR_pduary_addr6_error_clr_START  (21)
#define SOC_UPACC_INT_CLR_pduary_addr6_error_clr_END    (21)
#define SOC_UPACC_INT_CLR_pduary_addr7_error_clr_START  (22)
#define SOC_UPACC_INT_CLR_pduary_addr7_error_clr_END    (22)
#define SOC_UPACC_INT_CLR_pduary_addr8_error_clr_START  (23)
#define SOC_UPACC_INT_CLR_pduary_addr8_error_clr_END    (23)


/*****************************************************************************
 结构名    : SOC_UPACC_BUS_ERROR_ADDR_UNION
 结构说明  : BUS_ERROR_ADDR 寄存器结构定义。地址偏移量:0x0034，初值:0x00000000，宽度:32
 寄存器说明: 总线ERROR地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bus_error_addr : 32; /* bit[0-31]: 总线ERROR地址寄存器。
                                                         用于寄存当模块所访问的Slave侧返回ERROR时的当前操作的地址。 */
    } reg;
} SOC_UPACC_BUS_ERROR_ADDR_UNION;
#endif
#define SOC_UPACC_BUS_ERROR_ADDR_bus_error_addr_START  (0)
#define SOC_UPACC_BUS_ERROR_ADDR_bus_error_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH1_PARAM_UNION
 结构说明  : LOCH1_PARAM 寄存器结构定义。地址偏移量:0x0038，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1参数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn1     : 6;  /* bit[0-5]  : 配置MAC-e PDU参数TSN1。 */
        unsigned int  n1       : 6;  /* bit[6-11] : 配置MAC-e PDU参数N1。 */
        unsigned int  ddi1     : 6;  /* bit[12-17]: 配置MAC-e PDU参数DDI1。 */
        unsigned int  reserved : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_UPACC_LOCH1_PARAM_UNION;
#endif
#define SOC_UPACC_LOCH1_PARAM_tsn1_START      (0)
#define SOC_UPACC_LOCH1_PARAM_tsn1_END        (5)
#define SOC_UPACC_LOCH1_PARAM_n1_START        (6)
#define SOC_UPACC_LOCH1_PARAM_n1_END          (11)
#define SOC_UPACC_LOCH1_PARAM_ddi1_START      (12)
#define SOC_UPACC_LOCH1_PARAM_ddi1_END        (17)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH2_PARAM_UNION
 结构说明  : LOCH2_PARAM 寄存器结构定义。地址偏移量:0x003C，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道2参数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn2     : 6;  /* bit[0-5]  : 配置MAC-e PDU参数TSN2。 */
        unsigned int  n2       : 6;  /* bit[6-11] : 配置MAC-e PDU参数N2。 */
        unsigned int  ddi2     : 6;  /* bit[12-17]: 配置MAC-e PDU参数DDI2。 */
        unsigned int  reserved : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_UPACC_LOCH2_PARAM_UNION;
#endif
#define SOC_UPACC_LOCH2_PARAM_tsn2_START      (0)
#define SOC_UPACC_LOCH2_PARAM_tsn2_END        (5)
#define SOC_UPACC_LOCH2_PARAM_n2_START        (6)
#define SOC_UPACC_LOCH2_PARAM_n2_END          (11)
#define SOC_UPACC_LOCH2_PARAM_ddi2_START      (12)
#define SOC_UPACC_LOCH2_PARAM_ddi2_END        (17)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH3_PARAM_UNION
 结构说明  : LOCH3_PARAM 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1参数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn3     : 6;  /* bit[0-5]  : 配置MAC-e PDU参数TSN3。 */
        unsigned int  n3       : 6;  /* bit[6-11] : 配置MAC-e PDU参数N3。 */
        unsigned int  ddi3     : 6;  /* bit[12-17]: 配置MAC-e PDU参数DDI3。 */
        unsigned int  reserved : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_UPACC_LOCH3_PARAM_UNION;
#endif
#define SOC_UPACC_LOCH3_PARAM_tsn3_START      (0)
#define SOC_UPACC_LOCH3_PARAM_tsn3_END        (5)
#define SOC_UPACC_LOCH3_PARAM_n3_START        (6)
#define SOC_UPACC_LOCH3_PARAM_n3_END          (11)
#define SOC_UPACC_LOCH3_PARAM_ddi3_START      (12)
#define SOC_UPACC_LOCH3_PARAM_ddi3_END        (17)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH4_PARAM_UNION
 结构说明  : LOCH4_PARAM 寄存器结构定义。地址偏移量:0x0044，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1参数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn4     : 6;  /* bit[0-5]  : 配置MAC-e PDU参数TSN4。 */
        unsigned int  n4       : 6;  /* bit[6-11] : 配置MAC-e PDU参数N4。 */
        unsigned int  ddi4     : 6;  /* bit[12-17]: 配置MAC-e PDU参数DDI4。 */
        unsigned int  reserved : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_UPACC_LOCH4_PARAM_UNION;
#endif
#define SOC_UPACC_LOCH4_PARAM_tsn4_START      (0)
#define SOC_UPACC_LOCH4_PARAM_tsn4_END        (5)
#define SOC_UPACC_LOCH4_PARAM_n4_START        (6)
#define SOC_UPACC_LOCH4_PARAM_n4_END          (11)
#define SOC_UPACC_LOCH4_PARAM_ddi4_START      (12)
#define SOC_UPACC_LOCH4_PARAM_ddi4_END        (17)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH5_PARAM_UNION
 结构说明  : LOCH5_PARAM 寄存器结构定义。地址偏移量:0x0048，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1参数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn5     : 6;  /* bit[0-5]  : 配置MAC-e PDU参数TSN5。 */
        unsigned int  n5       : 6;  /* bit[6-11] : 配置MAC-e PDU参数N5。 */
        unsigned int  ddi5     : 6;  /* bit[12-17]: 配置MAC-e PDU参数DDI5。 */
        unsigned int  reserved : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_UPACC_LOCH5_PARAM_UNION;
#endif
#define SOC_UPACC_LOCH5_PARAM_tsn5_START      (0)
#define SOC_UPACC_LOCH5_PARAM_tsn5_END        (5)
#define SOC_UPACC_LOCH5_PARAM_n5_START        (6)
#define SOC_UPACC_LOCH5_PARAM_n5_END          (11)
#define SOC_UPACC_LOCH5_PARAM_ddi5_START      (12)
#define SOC_UPACC_LOCH5_PARAM_ddi5_END        (17)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH6_PARAM_UNION
 结构说明  : LOCH6_PARAM 寄存器结构定义。地址偏移量:0x004C，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1参数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn6     : 6;  /* bit[0-5]  : 配置MAC-e PDU参数TSN6。 */
        unsigned int  n6       : 6;  /* bit[6-11] : 配置MAC-e PDU参数N6。 */
        unsigned int  ddi6     : 6;  /* bit[12-17]: 配置MAC-e PDU参数DDI6。 */
        unsigned int  reserved : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_UPACC_LOCH6_PARAM_UNION;
#endif
#define SOC_UPACC_LOCH6_PARAM_tsn6_START      (0)
#define SOC_UPACC_LOCH6_PARAM_tsn6_END        (5)
#define SOC_UPACC_LOCH6_PARAM_n6_START        (6)
#define SOC_UPACC_LOCH6_PARAM_n6_END          (11)
#define SOC_UPACC_LOCH6_PARAM_ddi6_START      (12)
#define SOC_UPACC_LOCH6_PARAM_ddi6_END        (17)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH7_PARAM_UNION
 结构说明  : LOCH7_PARAM 寄存器结构定义。地址偏移量:0x0050，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1参数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn7     : 6;  /* bit[0-5]  : 配置MAC-e PDU参数TSN7。 */
        unsigned int  n7       : 6;  /* bit[6-11] : 配置MAC-e PDU参数N7。 */
        unsigned int  ddi7     : 6;  /* bit[12-17]: 配置MAC-e PDU参数DDI7。 */
        unsigned int  reserved : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_UPACC_LOCH7_PARAM_UNION;
#endif
#define SOC_UPACC_LOCH7_PARAM_tsn7_START      (0)
#define SOC_UPACC_LOCH7_PARAM_tsn7_END        (5)
#define SOC_UPACC_LOCH7_PARAM_n7_START        (6)
#define SOC_UPACC_LOCH7_PARAM_n7_END          (11)
#define SOC_UPACC_LOCH7_PARAM_ddi7_START      (12)
#define SOC_UPACC_LOCH7_PARAM_ddi7_END        (17)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH8_PARAM_UNION
 结构说明  : LOCH8_PARAM 寄存器结构定义。地址偏移量:0x0054，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1参数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn8     : 6;  /* bit[0-5]  : 配置MAC-e PDU参数TSN8。 */
        unsigned int  n8       : 6;  /* bit[6-11] : 配置MAC-e PDU参数N8。 */
        unsigned int  ddi8     : 6;  /* bit[12-17]: 配置MAC-e PDU参数DDI8。 */
        unsigned int  reserved : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_UPACC_LOCH8_PARAM_UNION;
#endif
#define SOC_UPACC_LOCH8_PARAM_tsn8_START      (0)
#define SOC_UPACC_LOCH8_PARAM_tsn8_END        (5)
#define SOC_UPACC_LOCH8_PARAM_n8_START        (6)
#define SOC_UPACC_LOCH8_PARAM_n8_END          (11)
#define SOC_UPACC_LOCH8_PARAM_ddi8_START      (12)
#define SOC_UPACC_LOCH8_PARAM_ddi8_END        (17)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH1_START_ADDR_UNION
 结构说明  : LOCH1_START_ADDR 寄存器结构定义。地址偏移量:0x0058，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch1_start_addr : 32; /* bit[0-31]: 配置存放逻辑信道1的数据的起始地址，为byte地址。
                                                           要求该地址Word对齐。
                                                           源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH1_START_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH1_START_ADDR_loch1_start_addr_START  (0)
#define SOC_UPACC_LOCH1_START_ADDR_loch1_start_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH2_START_ADDR_UNION
 结构说明  : LOCH2_START_ADDR 寄存器结构定义。地址偏移量:0x005C，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道2起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch2_start_addr : 32; /* bit[0-31]: 配置存放逻辑信道2的数据的起始地址。
                                                           要求该地址Double Word对齐。
                                                           源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH2_START_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH2_START_ADDR_loch2_start_addr_START  (0)
#define SOC_UPACC_LOCH2_START_ADDR_loch2_start_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH3_START_ADDR_UNION
 结构说明  : LOCH3_START_ADDR 寄存器结构定义。地址偏移量:0x0060，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道3起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch3_start_addr : 32; /* bit[0-31]: 配置存放逻辑信道3的数据的起始地址。
                                                           要求该地址Double Word对齐。
                                                           源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH3_START_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH3_START_ADDR_loch3_start_addr_START  (0)
#define SOC_UPACC_LOCH3_START_ADDR_loch3_start_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH4_START_ADDR_UNION
 结构说明  : LOCH4_START_ADDR 寄存器结构定义。地址偏移量:0x0064，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道4起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch4_start_addr : 32; /* bit[0-31]: 配置存放逻辑信道4的数据的起始地址。
                                                           要求该地址Double Word对齐。
                                                           源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH4_START_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH4_START_ADDR_loch4_start_addr_START  (0)
#define SOC_UPACC_LOCH4_START_ADDR_loch4_start_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH5_START_ADDR_UNION
 结构说明  : LOCH5_START_ADDR 寄存器结构定义。地址偏移量:0x0068，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道5起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch5_start_addr : 32; /* bit[0-31]: 配置存放逻辑信道5的数据的起始地址。
                                                           要求该地址Double Word对齐。
                                                           源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH5_START_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH5_START_ADDR_loch5_start_addr_START  (0)
#define SOC_UPACC_LOCH5_START_ADDR_loch5_start_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH6_START_ADDR_UNION
 结构说明  : LOCH6_START_ADDR 寄存器结构定义。地址偏移量:0x006C，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道6起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch6_start_addr : 32; /* bit[0-31]: 配置存放逻辑信道6的数据的起始地址。
                                                           要求该地址Double Word对齐。
                                                           源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH6_START_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH6_START_ADDR_loch6_start_addr_START  (0)
#define SOC_UPACC_LOCH6_START_ADDR_loch6_start_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH7_START_ADDR_UNION
 结构说明  : LOCH7_START_ADDR 寄存器结构定义。地址偏移量:0x0070，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道7起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch7_start_addr : 32; /* bit[0-31]: 配置存放逻辑信道7的数据的起始地址。
                                                           要求该地址Double Word对齐。
                                                           源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH7_START_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH7_START_ADDR_loch7_start_addr_START  (0)
#define SOC_UPACC_LOCH7_START_ADDR_loch7_start_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH8_START_ADDR_UNION
 结构说明  : LOCH8_START_ADDR 寄存器结构定义。地址偏移量:0x0074，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道8起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch8_start_addr : 32; /* bit[0-31]: 配置存放逻辑信道8的数据的起始地址。
                                                           要求该地址Double Word对齐。
                                                           源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH8_START_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH8_START_ADDR_loch8_start_addr_START  (0)
#define SOC_UPACC_LOCH8_START_ADDR_loch8_start_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH1_END_ADDR_UNION
 结构说明  : LOCH1_END_ADDR 寄存器结构定义。地址偏移量:0x0078，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1结束地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch1_end_addr : 32; /* bit[0-31]: 配置存放逻辑信道1的数据的结束地址。
                                                         要求该地址Double Word对齐。
                                                         源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH1_END_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH1_END_ADDR_loch1_end_addr_START  (0)
#define SOC_UPACC_LOCH1_END_ADDR_loch1_end_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH2_END_ADDR_UNION
 结构说明  : LOCH2_END_ADDR 寄存器结构定义。地址偏移量:0x007C，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道2结束地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch2_end_addr : 32; /* bit[0-31]: 配置存放逻辑信道2的数据的结束地址。
                                                         要求该地址Double Word对齐。
                                                         源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH2_END_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH2_END_ADDR_loch2_end_addr_START  (0)
#define SOC_UPACC_LOCH2_END_ADDR_loch2_end_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH3_END_ADDR_UNION
 结构说明  : LOCH3_END_ADDR 寄存器结构定义。地址偏移量:0x0080，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道3结束地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch3_end_addr : 32; /* bit[0-31]: 配置存放逻辑信道3的数据的结束地址。
                                                         要求该地址Double Word对齐。
                                                         源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH3_END_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH3_END_ADDR_loch3_end_addr_START  (0)
#define SOC_UPACC_LOCH3_END_ADDR_loch3_end_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH4_END_ADDR_UNION
 结构说明  : LOCH4_END_ADDR 寄存器结构定义。地址偏移量:0x0084，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道4结束地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch4_end_addr : 32; /* bit[0-31]: 配置存放逻辑信道4的数据的结束地址。
                                                         要求该地址Double Word对齐。
                                                         源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH4_END_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH4_END_ADDR_loch4_end_addr_START  (0)
#define SOC_UPACC_LOCH4_END_ADDR_loch4_end_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH5_END_ADDR_UNION
 结构说明  : LOCH5_END_ADDR 寄存器结构定义。地址偏移量:0x0088，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道5结束地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch5_end_addr : 32; /* bit[0-31]: 配置存放逻辑信道5的数据的结束地址。
                                                         要求该地址Double Word对齐。
                                                         源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH5_END_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH5_END_ADDR_loch5_end_addr_START  (0)
#define SOC_UPACC_LOCH5_END_ADDR_loch5_end_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH6_END_ADDR_UNION
 结构说明  : LOCH6_END_ADDR 寄存器结构定义。地址偏移量:0x008C，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道6结束地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch6_end_addr : 32; /* bit[0-31]: 配置存放逻辑信道6的数据的结束地址。
                                                         要求该地址Double Word对齐。
                                                         源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH6_END_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH6_END_ADDR_loch6_end_addr_START  (0)
#define SOC_UPACC_LOCH6_END_ADDR_loch6_end_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH7_END_ADDR_UNION
 结构说明  : LOCH7_END_ADDR 寄存器结构定义。地址偏移量:0x0090，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道7结束地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch7_end_addr : 32; /* bit[0-31]: 配置存放逻辑信道7的数据的结束地址。
                                                         要求该地址Double Word对齐。
                                                         源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH7_END_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH7_END_ADDR_loch7_end_addr_START  (0)
#define SOC_UPACC_LOCH7_END_ADDR_loch7_end_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH8_END_ADDR_UNION
 结构说明  : LOCH8_END_ADDR 寄存器结构定义。地址偏移量:0x0094，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道8结束地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch8_end_addr : 32; /* bit[0-31]: 配置存放逻辑信道8的数据的结束地址。
                                                         要求该地址Double Word对齐。
                                                         源数据数组方式时，该寄存器无意义，建议直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH8_END_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH8_END_ADDR_loch8_end_addr_START  (0)
#define SOC_UPACC_LOCH8_END_ADDR_loch8_end_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH1_READ_ADDR_UNION
 结构说明  : LOCH1_READ_ADDR 寄存器结构定义。地址偏移量:0x0098，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1读起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch1_read_addr : 32; /* bit[0-31]: 配置逻辑信道1的读起始地址，为byte地址。
                                                          源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件从该地址访问连续的数据空间，读入源数据。
                                                          源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的源数据读地址更新到此寄存器，便于调试。 */
    } reg;
} SOC_UPACC_LOCH1_READ_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH1_READ_ADDR_loch1_read_addr_START  (0)
#define SOC_UPACC_LOCH1_READ_ADDR_loch1_read_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH2_READ_ADDR_UNION
 结构说明  : LOCH2_READ_ADDR 寄存器结构定义。地址偏移量:0x009C，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道2读起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch2_read_addr : 32; /* bit[0-31]: 配置逻辑信道2的读起始地址，为byte地址。
                                                          源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件从该地址访问连续的数据空间，读入源数据。
                                                          源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的源数据读地址更新到此寄存器，便于调试。 */
    } reg;
} SOC_UPACC_LOCH2_READ_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH2_READ_ADDR_loch2_read_addr_START  (0)
#define SOC_UPACC_LOCH2_READ_ADDR_loch2_read_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH3_READ_ADDR_UNION
 结构说明  : LOCH3_READ_ADDR 寄存器结构定义。地址偏移量:0x00A0，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道3读起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch3_read_addr : 32; /* bit[0-31]: 配置逻辑信道3的读起始地址，为byte地址。
                                                          源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件从该地址访问连续的数据空间，读入源数据。
                                                          源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的源数据读地址更新到此寄存器，便于调试。 */
    } reg;
} SOC_UPACC_LOCH3_READ_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH3_READ_ADDR_loch3_read_addr_START  (0)
#define SOC_UPACC_LOCH3_READ_ADDR_loch3_read_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH4_READ_ADDR_UNION
 结构说明  : LOCH4_READ_ADDR 寄存器结构定义。地址偏移量:0x00A4，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道4读起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch4_read_addr : 32; /* bit[0-31]: 配置逻辑信道4的读起始地址，为byte地址。
                                                          源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件从该地址访问连续的数据空间，读入源数据。
                                                          源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的源数据读地址更新到此寄存器，便于调试。 */
    } reg;
} SOC_UPACC_LOCH4_READ_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH4_READ_ADDR_loch4_read_addr_START  (0)
#define SOC_UPACC_LOCH4_READ_ADDR_loch4_read_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH5_READ_ADDR_UNION
 结构说明  : LOCH5_READ_ADDR 寄存器结构定义。地址偏移量:0x00A8，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道5读起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch5_read_addr : 32; /* bit[0-31]: 配置逻辑信道5的读起始地址，为byte地址。
                                                          源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件从该地址访问连续的数据空间，读入源数据。
                                                          源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的源数据读地址更新到此寄存器，便于调试。 */
    } reg;
} SOC_UPACC_LOCH5_READ_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH5_READ_ADDR_loch5_read_addr_START  (0)
#define SOC_UPACC_LOCH5_READ_ADDR_loch5_read_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH6_READ_ADDR_UNION
 结构说明  : LOCH6_READ_ADDR 寄存器结构定义。地址偏移量:0x00AC，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道6读起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch6_read_addr : 32; /* bit[0-31]: 配置逻辑信道6的读起始地址，为byte地址。
                                                          源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件从该地址访问连续的数据空间，读入源数据。
                                                          源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的源数据读地址更新到此寄存器，便于调试。 */
    } reg;
} SOC_UPACC_LOCH6_READ_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH6_READ_ADDR_loch6_read_addr_START  (0)
#define SOC_UPACC_LOCH6_READ_ADDR_loch6_read_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH7_READ_ADDR_UNION
 结构说明  : LOCH7_READ_ADDR 寄存器结构定义。地址偏移量:0x00B0，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道7读起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch7_read_addr : 32; /* bit[0-31]: 配置逻辑信道7的读起始地址，为byte地址。
                                                          源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件从该地址访问连续的数据空间，读入源数据。
                                                          源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的源数据读地址更新到此寄存器，便于调试。 */
    } reg;
} SOC_UPACC_LOCH7_READ_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH7_READ_ADDR_loch7_read_addr_START  (0)
#define SOC_UPACC_LOCH7_READ_ADDR_loch7_read_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH8_READ_ADDR_UNION
 结构说明  : LOCH8_READ_ADDR 寄存器结构定义。地址偏移量:0x00B4，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道8读起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch8_read_addr : 32; /* bit[0-31]: 配置逻辑信道8的读起始地址，为byte地址。
                                                          源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件从该地址访问连续的数据空间，读入源数据。
                                                          源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的源数据读地址更新到此寄存器，便于调试。 */
    } reg;
} SOC_UPACC_LOCH8_READ_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH8_READ_ADDR_loch8_read_addr_START  (0)
#define SOC_UPACC_LOCH8_READ_ADDR_loch8_read_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH1_NUM_CFG_UNION
 结构说明  : LOCH1_NUM_CFG 寄存器结构定义。地址偏移量:0x00B8，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1读数据相关个数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch1_read_num   : 16; /* bit[0-15] : 配置逻辑信道1的读数据个数，以byte为单位。
                                                            源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件根据该长度从读地址读入源数据。
                                                            源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的pdu段长度更新到此寄存器，便于调试。 */
        unsigned int  loch1_pduary_num : 16; /* bit[16-31]: 配置逻辑信道1的源数据数组长度，以word为单位。（该寄存器的值相当于待读取pdu段的个数*2）
                                                            该寄存器仅当源数据采用数组方式配置时有意义。
                                                            当源数据不采用数组方式配置时无意义，直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH1_NUM_CFG_UNION;
#endif
#define SOC_UPACC_LOCH1_NUM_CFG_loch1_read_num_START    (0)
#define SOC_UPACC_LOCH1_NUM_CFG_loch1_read_num_END      (15)
#define SOC_UPACC_LOCH1_NUM_CFG_loch1_pduary_num_START  (16)
#define SOC_UPACC_LOCH1_NUM_CFG_loch1_pduary_num_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH2_NUM_CFG_UNION
 结构说明  : LOCH2_NUM_CFG 寄存器结构定义。地址偏移量:0x00BC，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道2读数据相关个数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch2_read_num   : 16; /* bit[0-15] : 配置逻辑信道2的读数据个数，以byte为单位。
                                                            源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件根据该长度从读地址读入源数据。
                                                            源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的pdu段长度更新到此寄存器，便于调试 */
        unsigned int  loch2_pduary_num : 16; /* bit[16-31]: 配置逻辑信道2的源数据数组长度，以word为单位。（该寄存器的值相当于待读取pdu段的个数*2）
                                                            该寄存器仅当源数据采用数组方式配置时有意义。
                                                            当源数据不采用数组方式配置时无意义，直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH2_NUM_CFG_UNION;
#endif
#define SOC_UPACC_LOCH2_NUM_CFG_loch2_read_num_START    (0)
#define SOC_UPACC_LOCH2_NUM_CFG_loch2_read_num_END      (15)
#define SOC_UPACC_LOCH2_NUM_CFG_loch2_pduary_num_START  (16)
#define SOC_UPACC_LOCH2_NUM_CFG_loch2_pduary_num_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH3_NUM_CFG_UNION
 结构说明  : LOCH3_NUM_CFG 寄存器结构定义。地址偏移量:0x00C0，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道3读数据相关个数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch3_read_num   : 16; /* bit[0-15] : 配置逻辑信道3的读数据个数，以byte为单位。
                                                            源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件根据该长度从读地址读入源数据。
                                                            源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的pdu段长度更新到此寄存器，便于调试 */
        unsigned int  loch3_pduary_num : 16; /* bit[16-31]: 配置逻辑信道3的源数据数组长度，以word为单位。（该寄存器的值相当于待读取pdu段的个数*2）
                                                            该寄存器仅当源数据采用数组方式配置时有意义。
                                                            当源数据不采用数组方式配置时无意义，直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH3_NUM_CFG_UNION;
#endif
#define SOC_UPACC_LOCH3_NUM_CFG_loch3_read_num_START    (0)
#define SOC_UPACC_LOCH3_NUM_CFG_loch3_read_num_END      (15)
#define SOC_UPACC_LOCH3_NUM_CFG_loch3_pduary_num_START  (16)
#define SOC_UPACC_LOCH3_NUM_CFG_loch3_pduary_num_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH4_NUM_CFG_UNION
 结构说明  : LOCH4_NUM_CFG 寄存器结构定义。地址偏移量:0x00C4，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道4读数据相关个数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch4_read_num   : 16; /* bit[0-15] : 配置逻辑信道4的读数据个数，以byte为单位。
                                                            源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件根据该长度从读地址读入源数据。
                                                            源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的pdu段长度更新到此寄存器，便于调试 */
        unsigned int  loch4_pduary_num : 16; /* bit[16-31]: 配置逻辑信道4的源数据数组长度，以word为单位。（该寄存器的值相当于待读取pdu段的个数*2）
                                                            该寄存器仅当源数据采用数组方式配置时有意义。
                                                            当源数据不采用数组方式配置时无意义，直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH4_NUM_CFG_UNION;
#endif
#define SOC_UPACC_LOCH4_NUM_CFG_loch4_read_num_START    (0)
#define SOC_UPACC_LOCH4_NUM_CFG_loch4_read_num_END      (15)
#define SOC_UPACC_LOCH4_NUM_CFG_loch4_pduary_num_START  (16)
#define SOC_UPACC_LOCH4_NUM_CFG_loch4_pduary_num_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH5_NUM_CFG_UNION
 结构说明  : LOCH5_NUM_CFG 寄存器结构定义。地址偏移量:0x00C8，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道5读数据相关个数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch5_read_num   : 16; /* bit[0-15] : 配置逻辑信道5的读数据个数，以byte为单位。
                                                            源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件根据该长度从读地址读入源数据。
                                                            源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的pdu段长度更新到此寄存器，便于调试 */
        unsigned int  loch5_pduary_num : 16; /* bit[16-31]: 配置逻辑信道5的源数据数组长度，以word为单位。（该寄存器的值相当于待读取pdu段的个数*2）
                                                            该寄存器仅当源数据采用数组方式配置时有意义。
                                                            当源数据不采用数组方式配置时无意义，直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH5_NUM_CFG_UNION;
#endif
#define SOC_UPACC_LOCH5_NUM_CFG_loch5_read_num_START    (0)
#define SOC_UPACC_LOCH5_NUM_CFG_loch5_read_num_END      (15)
#define SOC_UPACC_LOCH5_NUM_CFG_loch5_pduary_num_START  (16)
#define SOC_UPACC_LOCH5_NUM_CFG_loch5_pduary_num_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH6_NUM_CFG_UNION
 结构说明  : LOCH6_NUM_CFG 寄存器结构定义。地址偏移量:0x00CC，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道6读数据相关个数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch6_read_num   : 16; /* bit[0-15] : 配置逻辑信道6的读数据个数，以byte为单位。
                                                            源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件根据该长度从读地址读入源数据。
                                                            源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的pdu段长度更新到此寄存器，便于调试 */
        unsigned int  loch6_pduary_num : 16; /* bit[16-31]: 配置逻辑信道6的源数据数组长度，以word为单位。（该寄存器的值相当于待读取pdu段的个数*2）
                                                            该寄存器仅当源数据采用数组方式配置时有意义。
                                                            当源数据不采用数组方式配置时无意义，直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH6_NUM_CFG_UNION;
#endif
#define SOC_UPACC_LOCH6_NUM_CFG_loch6_read_num_START    (0)
#define SOC_UPACC_LOCH6_NUM_CFG_loch6_read_num_END      (15)
#define SOC_UPACC_LOCH6_NUM_CFG_loch6_pduary_num_START  (16)
#define SOC_UPACC_LOCH6_NUM_CFG_loch6_pduary_num_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH7_NUM_CFG_UNION
 结构说明  : LOCH7_NUM_CFG 寄存器结构定义。地址偏移量:0x00D0，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道7读数据相关个数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch7_read_num   : 16; /* bit[0-15] : 配置逻辑信道7的读数据个数，以byte为单位。
                                                            源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件根据该长度从读地址读入源数据。
                                                            源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的pdu段长度更新到此寄存器，便于调试 */
        unsigned int  loch7_pduary_num : 16; /* bit[16-31]: 配置逻辑信道7的源数据数组长度，以word为单位。（该寄存器的值相当于待读取pdu段的个数*2）
                                                            该寄存器仅当源数据采用数组方式配置时有意义。
                                                            当源数据不采用数组方式配置时无意义，直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH7_NUM_CFG_UNION;
#endif
#define SOC_UPACC_LOCH7_NUM_CFG_loch7_read_num_START    (0)
#define SOC_UPACC_LOCH7_NUM_CFG_loch7_read_num_END      (15)
#define SOC_UPACC_LOCH7_NUM_CFG_loch7_pduary_num_START  (16)
#define SOC_UPACC_LOCH7_NUM_CFG_loch7_pduary_num_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH8_NUM_CFG_UNION
 结构说明  : LOCH8_NUM_CFG 寄存器结构定义。地址偏移量:0x00D4，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道8读数据相关个数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch8_read_num   : 16; /* bit[0-15] : 配置逻辑信道8的读数据个数，以byte为单位。
                                                            源数据不采用数组方式配置时，该寄存器由软件一次性配置，硬件根据该长度从读地址读入源数据。
                                                            源数据采用数组方式配置时，将该寄存器配置为0。UPACC处理时，会将从数组中读到的pdu段长度更新到此寄存器，便于调试 */
        unsigned int  loch8_pduary_num : 16; /* bit[16-31]: 配置逻辑信道8的源数据数组长度，以word为单位。（该寄存器的值相当于待读取pdu段的个数*2）
                                                            该寄存器仅当源数据采用数组方式配置时有意义。
                                                            当源数据不采用数组方式配置时无意义，直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH8_NUM_CFG_UNION;
#endif
#define SOC_UPACC_LOCH8_NUM_CFG_loch8_read_num_START    (0)
#define SOC_UPACC_LOCH8_NUM_CFG_loch8_read_num_END      (15)
#define SOC_UPACC_LOCH8_NUM_CFG_loch8_pduary_num_START  (16)
#define SOC_UPACC_LOCH8_NUM_CFG_loch8_pduary_num_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_EN_UNION
 结构说明  : EN 寄存器结构定义。地址偏移量:0x00D8，初值:0x00000000，宽度:32
 寄存器说明: 模块使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  upacc_en : 1;  /* bit[0]   : UPACC模块的使能信号。
                                                   0：禁止，UPACC模块不工作，内部状态归0；
                                                   1：使能，UPACC模块处于工作状态。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_UPACC_EN_UNION;
#endif
#define SOC_UPACC_EN_upacc_en_START  (0)
#define SOC_UPACC_EN_upacc_en_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_PDUARY_EN_E_UNION
 结构说明  : PDUARY_EN_E 寄存器结构定义。地址偏移量:0x00DC，初值:0x00000000，宽度:32
 寄存器说明: Mac-e PDU源数据数组方式配置使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pduary_en_e : 1;  /* bit[0]   : Mac-e PDU源数据数组方式配置使能信号。
                                                      0：禁止，源数据不采用数组方式配置，兼容UPACCV200；
                                                      1：使能，源数据采用数组方式配置。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_UPACC_PDUARY_EN_E_UNION;
#endif
#define SOC_UPACC_PDUARY_EN_E_pduary_en_e_START  (0)
#define SOC_UPACC_PDUARY_EN_E_pduary_en_e_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH1_PDUARY_ADDR_UNION
 结构说明  : LOCH1_PDUARY_ADDR 寄存器结构定义。地址偏移量:0x00E0，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道1中待读取Mac-e PDU源数据数组首地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch1_pduary_addr : 32; /* bit[0-31]: 配置逻辑信道1中待读取Mac-e PDU源数据数组存放的首地址。
                                                            要求Double Word对齐。
                                                            只有当源数据采用数组方式配置时，该寄存器有意义；当源数据不采用数组方式配置时，该寄存器直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH1_PDUARY_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH1_PDUARY_ADDR_loch1_pduary_addr_START  (0)
#define SOC_UPACC_LOCH1_PDUARY_ADDR_loch1_pduary_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH2_PDUARY_ADDR_UNION
 结构说明  : LOCH2_PDUARY_ADDR 寄存器结构定义。地址偏移量:0x00E4，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道2中待读取Mac-e PDU源数据数组首地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch2_pduary_addr : 32; /* bit[0-31]: 配置逻辑信道2中待读取Mac-e PDU源数据数组存放的首地址。
                                                            要求Double Word对齐。
                                                            只有当源数据采用数组方式配置时，该寄存器有意义；当源数据不采用数组方式配置时，该寄存器直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH2_PDUARY_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH2_PDUARY_ADDR_loch2_pduary_addr_START  (0)
#define SOC_UPACC_LOCH2_PDUARY_ADDR_loch2_pduary_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH3_PDUARY_ADDR_UNION
 结构说明  : LOCH3_PDUARY_ADDR 寄存器结构定义。地址偏移量:0x00E8，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道3中待读取Mac-e PDU源数据数组首地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch3_pduary_addr : 32; /* bit[0-31]: 配置逻辑信道3中待读取Mac-e PDU源数据数组存放的首地址。
                                                            要求Double Word对齐。
                                                            只有当源数据采用数组方式配置时，该寄存器有意义；当源数据不采用数组方式配置时，该寄存器直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH3_PDUARY_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH3_PDUARY_ADDR_loch3_pduary_addr_START  (0)
#define SOC_UPACC_LOCH3_PDUARY_ADDR_loch3_pduary_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH4_PDUARY_ADDR_UNION
 结构说明  : LOCH4_PDUARY_ADDR 寄存器结构定义。地址偏移量:0x00EC，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道4中待读取Mac-e PDU源数据数组首地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch4_pduary_addr : 32; /* bit[0-31]: 配置逻辑信道4中待读取Mac-e PDU源数据数组存放的首地址。
                                                            要求Double Word对齐。
                                                            只有当源数据采用数组方式配置时，该寄存器有意义；当源数据不采用数组方式配置时，该寄存器直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH4_PDUARY_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH4_PDUARY_ADDR_loch4_pduary_addr_START  (0)
#define SOC_UPACC_LOCH4_PDUARY_ADDR_loch4_pduary_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH5_PDUARY_ADDR_UNION
 结构说明  : LOCH5_PDUARY_ADDR 寄存器结构定义。地址偏移量:0x00F0，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道5中待读取Mac-e PDU源数据数组首地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch5_pduary_addr : 32; /* bit[0-31]: 配置逻辑信道5中待读取Mac-e PDU源数据数组存放的首地址。
                                                            要求Double Word对齐。
                                                            只有当源数据采用数组方式配置时，该寄存器有意义；当源数据不采用数组方式配置时，该寄存器直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH5_PDUARY_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH5_PDUARY_ADDR_loch5_pduary_addr_START  (0)
#define SOC_UPACC_LOCH5_PDUARY_ADDR_loch5_pduary_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH6_PDUARY_ADDR_UNION
 结构说明  : LOCH6_PDUARY_ADDR 寄存器结构定义。地址偏移量:0x00F4，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道6中待读取Mac-e PDU源数据数组首地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch6_pduary_addr : 32; /* bit[0-31]: 配置逻辑信道6中待读取Mac-e PDU源数据数组存放的首地址。
                                                            要求Double Word对齐。
                                                            只有当源数据采用数组方式配置时，该寄存器有意义；当源数据不采用数组方式配置时，该寄存器直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH6_PDUARY_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH6_PDUARY_ADDR_loch6_pduary_addr_START  (0)
#define SOC_UPACC_LOCH6_PDUARY_ADDR_loch6_pduary_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH7_PDUARY_ADDR_UNION
 结构说明  : LOCH7_PDUARY_ADDR 寄存器结构定义。地址偏移量:0x00F8，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道7中待读取Mac-e PDU源数据数组首地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch7_pduary_addr : 32; /* bit[0-31]: 配置逻辑信道7中待读取Mac-e PDU源数据数组存放的首地址。
                                                            要求Double Word对齐。
                                                            只有当源数据采用数组方式配置时，该寄存器有意义；当源数据不采用数组方式配置时，该寄存器直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH7_PDUARY_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH7_PDUARY_ADDR_loch7_pduary_addr_START  (0)
#define SOC_UPACC_LOCH7_PDUARY_ADDR_loch7_pduary_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH8_PDUARY_ADDR_UNION
 结构说明  : LOCH8_PDUARY_ADDR 寄存器结构定义。地址偏移量:0x00FC，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道8中待读取Mac-e PDU源数据数组首地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch8_pduary_addr : 32; /* bit[0-31]: 配置逻辑信道8中待读取Mac-e PDU源数据数组存放的首地址。
                                                            要求Double Word对齐。
                                                            只有当源数据采用数组方式配置时，该寄存器有意义；当源数据不采用数组方式配置时，该寄存器直接配置为0。 */
    } reg;
} SOC_UPACC_LOCH8_PDUARY_ADDR_UNION;
#endif
#define SOC_UPACC_LOCH8_PDUARY_ADDR_loch8_pduary_addr_START  (0)
#define SOC_UPACC_LOCH8_PDUARY_ADDR_loch8_pduary_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_MAC_ENCAP_START_I1_UNION
 结构说明  : MAC_ENCAP_START_I1 寄存器结构定义。地址偏移量:0x0100，初值:0x00000000，宽度:32
 寄存器说明: 启动方式配置寄存器。用于配置MAC_i/is PDU封装的启动方式。
            UPACCV200支持对MAC-i/is PDU 进行不封装只搬移和封装且搬移两种启动方式。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mac_encap_start_i1 : 2;  /* bit[0-1] : 00：不启动MAC-i/is PDU封装，也不启动MAC-i/is PDU搬移；
                                                             01：仅启动MAC-i/is PDU封装，封装好的数据仅存放到HARQ Buffer中，不存放到BBP编码缓存中。不启动MAC-i/is PDU搬移；这种启动方式仅用于调试。
                                                             10：不启动MAC-i/is PDU封装，仅启动MAC-i/is PDU搬移；将HARQ Buffer中指定位置处的数据搬移到BBP编码缓存中。这种启动方式应用于重传MAC-i/is PDU的场景。
                                                             11：启动MAC-i/is PDU封装，并将封装好的数据同时分别存放到HARQ Buffer和BBP编码缓存中。这种启动方式应用于新传MAC-i/is PDU的场景。
                                                             当MAC-i/is PDU封装完成后，即INT_RAW_I1寄存器中的encap_end_raw_i1有效后，硬件逻辑会对该寄存器的bit0自动清零；
                                                             当MAC-i/is PDU搬移完成后，即INT_RAW_I1寄存器中的trans_end_raw_i1有效后，硬件逻辑会对该寄存器的bit1自动清零。
                                                             当软件参数配置出错，即INT_RAW_I中的addrn_error_raw_i1、loch_num_error_raw_i1、dest_addr_error_raw_i1等任意一个有效时，硬件逻辑会对该寄存器的bit0和bit1同时清零。 */
        unsigned int  reserved           : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_UPACC_MAC_ENCAP_START_I1_UNION;
#endif
#define SOC_UPACC_MAC_ENCAP_START_I1_mac_encap_start_i1_START  (0)
#define SOC_UPACC_MAC_ENCAP_START_I1_mac_encap_start_i1_END    (1)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH_NUM_I1_UNION
 结构说明  : LOCH_NUM_I1 寄存器结构定义。地址偏移量:0x0104，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道个数寄存器。用于指示当次封装MAC-i/is PDU共需从几个逻辑信道中读取数据。若该寄存器内容配置为0，表示当前MAC-i/is PDU仅由SI组成，没有其他数据和参数，即为SI单独发送。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch_num_i1 : 4;  /* bit[0-3] : 用于指示当次封装MAC-i/is PDU共需从几个信道中提取数据。
                                                      若该参数配置为0，则表示当次为SI单独发送，无其他数据和参数。 */
        unsigned int  reserved    : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_UPACC_LOCH_NUM_I1_UNION;
#endif
#define SOC_UPACC_LOCH_NUM_I1_loch_num_i1_START  (0)
#define SOC_UPACC_LOCH_NUM_I1_loch_num_i1_END    (3)


/*****************************************************************************
 结构名    : SOC_UPACC_HEAD0_EN_I1_UNION
 结构说明  : HEAD0_EN_I1 寄存器结构定义。地址偏移量:0x0108，初值:0x00000000，宽度:32
 寄存器说明: MAC-i header0使能寄存器。用于指示当次封装的MAC-i/is PDU中是否包含header0参数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  head0_en_i1 : 1;  /* bit[0]   : 0：MAC-i/is PDU中不包含header0参数。
                                                      1：MAC-i/is PDU中包含header0参数。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_UPACC_HEAD0_EN_I1_UNION;
#endif
#define SOC_UPACC_HEAD0_EN_I1_head0_en_i1_START  (0)
#define SOC_UPACC_HEAD0_EN_I1_head0_en_i1_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_E_RNTI_I1_UNION
 结构说明  : E_RNTI_I1 寄存器结构定义。地址偏移量:0x010C，初值:0x00000000，宽度:32
 寄存器说明: E_RNTI配置寄存器。用于配置MAC-i/is PDU header0中的E-RNTI参数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  e_rnti_i1 : 16; /* bit[0-15] : 当head0_en_i1为1时，用于配置MAC-i/is PDU header0中的E-RNTI参数。 */
        unsigned int  reserved  : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_UPACC_E_RNTI_I1_UNION;
#endif
#define SOC_UPACC_E_RNTI_I1_e_rnti_i1_START  (0)
#define SOC_UPACC_E_RNTI_I1_e_rnti_i1_END    (15)


/*****************************************************************************
 结构名    : SOC_UPACC_SI_EN_I1_UNION
 结构说明  : SI_EN_I1 寄存器结构定义。地址偏移量:0x0110，初值:0x00000000，宽度:32
 寄存器说明: SI使能寄存器。用于指示当次封装的MAC-i/is PDU中是否包含SI参数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  si_en_i1 : 1;  /* bit[0]   : 0：MAC-i/is PDU中不包含SI参数。
                                                   1：MAC-i/is PDU中包含SI参数。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_UPACC_SI_EN_I1_UNION;
#endif
#define SOC_UPACC_SI_EN_I1_si_en_i1_START  (0)
#define SOC_UPACC_SI_EN_I1_si_en_i1_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_SI_DATA_I1_UNION
 结构说明  : SI_DATA_I1 寄存器结构定义。地址偏移量:0x0114，初值:0x00000000，宽度:32
 寄存器说明: SI数据配置寄存器。用于配置MAC-i/is PDU中的SI参数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  si_hlid_i1 : 4;  /* bit[0-3]  : 当si_en_i1为1时，用于配置MAC-i/is PDU中SI参数中的HLID数据段。 */
        unsigned int  si_hlbs_i1 : 4;  /* bit[4-7]  : 当si_en_i1为1时，用于配置MAC-i/is PDU中SI参数中的HLBS数据段。 */
        unsigned int  si_tebs_i1 : 5;  /* bit[8-12] : 当si_en_i1为1时，用于配置MAC-i/is PDU中SI参数中的TEBS数据段。 */
        unsigned int  reserved_0 : 3;  /* bit[13-15]: 保留 */
        unsigned int  si_uph_i1  : 5;  /* bit[16-20]: 当si_en_i1为1时，用于配置MAC-i/is PDU中SI参数中的UPH数据段。 */
        unsigned int  reserved_1 : 11; /* bit[21-31]: 保留 */
    } reg;
} SOC_UPACC_SI_DATA_I1_UNION;
#endif
#define SOC_UPACC_SI_DATA_I1_si_hlid_i1_START  (0)
#define SOC_UPACC_SI_DATA_I1_si_hlid_i1_END    (3)
#define SOC_UPACC_SI_DATA_I1_si_hlbs_i1_START  (4)
#define SOC_UPACC_SI_DATA_I1_si_hlbs_i1_END    (7)
#define SOC_UPACC_SI_DATA_I1_si_tebs_i1_START  (8)
#define SOC_UPACC_SI_DATA_I1_si_tebs_i1_END    (12)
#define SOC_UPACC_SI_DATA_I1_si_uph_i1_START   (16)
#define SOC_UPACC_SI_DATA_I1_si_uph_i1_END     (20)


/*****************************************************************************
 结构名    : SOC_UPACC_PADDING_LEN_I1_UNION
 结构说明  : PADDING_LEN_I1 寄存器结构定义。地址偏移量:0x0118，初值:0x00000000，宽度:32
 寄存器说明: padding长度配置寄存器。用于配置MAC-i/is PDU中的padding参数的长度。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  padding_len_i1 : 16; /* bit[0-15] : 用于配置MAC-i/is PDU中padding参数的长度。以bit为单位。
                                                          最大允许配置值为16'hFFCD。
                                                          当padding_len配置为0，表示当次封装的MAC-i/is PDU中没有padding参数。 */
        unsigned int  reserved       : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_UPACC_PADDING_LEN_I1_UNION;
#endif
#define SOC_UPACC_PADDING_LEN_I1_padding_len_i1_START  (0)
#define SOC_UPACC_PADDING_LEN_I1_padding_len_i1_END    (15)


/*****************************************************************************
 结构名    : SOC_UPACC_TSN_LEN_I1_UNION
 结构说明  : TSN_LEN_I1 寄存器结构定义。地址偏移量:0x011C，初值:0x00000000，宽度:32
 寄存器说明: TSN长度配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn_len_i1 : 1;  /* bit[0]   : 用于配置MAC-i/is PDU中TSN参数的长度。
                                                     1：TSN的长度为14bits。
                                                     0：TSN的长度为6bits。 */
        unsigned int  reserved   : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_UPACC_TSN_LEN_I1_UNION;
#endif
#define SOC_UPACC_TSN_LEN_I1_tsn_len_i1_START  (0)
#define SOC_UPACC_TSN_LEN_I1_tsn_len_i1_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_PARAM_ADDR_I1_UNION
 结构说明  : PARAM_ADDR_I1 寄存器结构定义。地址偏移量:0x0120，初值:0x00000000，宽度:32
 寄存器说明: 参数链表起始地址寄存器。用于配置参数链表在存储器中存放的起始地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  param_addr_i1 : 32; /* bit[0-31]: 用于配置MAC-i/is PDU封装时所需的参数链表在存储器中存放的起始地址。
                                                        以byte为单位。但是必须以double word地址进行配置，即该地址的低三位bit[2:0]必须为3'b000。 */
    } reg;
} SOC_UPACC_PARAM_ADDR_I1_UNION;
#endif
#define SOC_UPACC_PARAM_ADDR_I1_param_addr_i1_START  (0)
#define SOC_UPACC_PARAM_ADDR_I1_param_addr_i1_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_HARQ_BUF_ADDR_I1_UNION
 结构说明  : HARQ_BUF_ADDR_I1 寄存器结构定义。地址偏移量:0x0124，初值:0x00000000，宽度:32
 寄存器说明: 封装目标地址与搬移起始地址寄存器。用于配置当次MAC-i/is PDU封装的目的地址或搬运的起始地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hraq_buf_addr_i1 : 32; /* bit[0-31]: 用于配置当次MAC-i/is PDU封装的目的地址或搬运的起始地址。MAC-i/is PDU的封装目标地址与搬移起始地址是相同的，都由该寄存器配置。
                                                           以byte为单位。但是必须以double word地址进行配置，即该地址的低三位bit[2:0]必须为2'b000。 */
    } reg;
} SOC_UPACC_HARQ_BUF_ADDR_I1_UNION;
#endif
#define SOC_UPACC_HARQ_BUF_ADDR_I1_hraq_buf_addr_i1_START  (0)
#define SOC_UPACC_HARQ_BUF_ADDR_I1_hraq_buf_addr_i1_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_BBP_DEST_ADDR_I1_UNION
 结构说明  : BBP_DEST_ADDR_I1 寄存器结构定义。地址偏移量:0x0128，初值:0x00000000，宽度:32
 寄存器说明: 搬移目标地址寄存器。用于配置MAC-i/is PDU搬移到BBP编码缓存的目标地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bbp_dest_addr_i1 : 32; /* bit[0-31]: 用于配置MAC-i/is PDU搬移到BBP编码缓存的目标地址。
                                                           以byte为单位。但是必须以double word地址进行配置，即该地址的低三位bit[2:0]必须为3'b000。 */
    } reg;
} SOC_UPACC_BBP_DEST_ADDR_I1_UNION;
#endif
#define SOC_UPACC_BBP_DEST_ADDR_I1_bbp_dest_addr_i1_START  (0)
#define SOC_UPACC_BBP_DEST_ADDR_I1_bbp_dest_addr_i1_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_TRANS_NUM_I1_UNION
 结构说明  : TRANS_NUM_I1 寄存器结构定义。地址偏移量:0x012C，初值:0x00000000，宽度:32
 寄存器说明: 搬移数据大小寄存器。用于配置需要搬移到BBP的MAC-i/is PDU数据字节数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trans_num_i1 : 16; /* bit[0-15] : 用于配置当次需搬运到BBP编码缓存中的MAC-i/is PDU数据大小。以byte为单位。但是由于BBP仅支持word访问，因此实际搬运时会按照整word进行搬移操作。
                                                        最大允许配置值为16'hFFFC。 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_UPACC_TRANS_NUM_I1_UNION;
#endif
#define SOC_UPACC_TRANS_NUM_I1_trans_num_i1_START  (0)
#define SOC_UPACC_TRANS_NUM_I1_trans_num_i1_END    (15)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_EN_I1_UNION
 结构说明  : INT_EN_I1 寄存器结构定义。地址偏移量:0x0130，初值:0x00000000，宽度:32
 寄存器说明: 中断使能寄存器。用于配置各中断信号使能与否。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_en_i1        : 1;  /* bit[0]    : MAC-i/is PDU封装完成中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  trans_end_en_i1        : 1;  /* bit[1]    : MAC-i/is PDU搬移完成中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr1_error_en_i1      : 1;  /* bit[2]    : MAC-i/is PDU逻辑信道1读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr2_error_en_i1      : 1;  /* bit[3]    : MAC-i/is PDU逻辑信道2读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr3_error_en_i1      : 1;  /* bit[4]    : MAC-i/is PDU逻辑信道3读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr4_error_en_i1      : 1;  /* bit[5]    : MAC-i/is PDU逻辑信道4读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr5_error_en_i1      : 1;  /* bit[6]    : MAC-i/is PDU逻辑信道5读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr6_error_en_i1      : 1;  /* bit[7]    : MAC-i/is PDU逻辑信道6读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr7_error_en_i1      : 1;  /* bit[8]    : MAC-i/is PDU逻辑信道7读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr8_error_en_i1      : 1;  /* bit[9]    : MAC-i/is PDU逻辑信道8读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr9_error_en_i1      : 1;  /* bit[10]   : MAC-i/is PDU逻辑信道9读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr10_error_en_i1     : 1;  /* bit[11]   : MAC-i/is PDU逻辑信道10读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr11_error_en_i1     : 1;  /* bit[12]   : MAC-i/is PDU逻辑信道11读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr12_error_en_i1     : 1;  /* bit[13]   : MAC-i/is PDU逻辑信道12读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr13_error_en_i1     : 1;  /* bit[14]   : MAC-i/is PDU逻辑信道13读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr14_error_en_i1     : 1;  /* bit[15]   : MAC-i/is PDU逻辑信道14读地址配置错误中断。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  param_addr_error_en_i1 : 1;  /* bit[16]   : MAC-i/is PDU参数链表地址配置错误中断。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  harq_addr_error_en_i1  : 1;  /* bit[17]   : MAC-i/is PDU封装目标地址也即搬移起始地址配置错误中断。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  dest_addr_eorr_en_i1   : 1;  /* bit[18]   : MAC-i/is PDU搬移目标地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  loch_num_error_en_i1   : 1;  /* bit[19]   : 逻辑信道个数配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  bus_error_en_i1        : 1;  /* bit[20]   : UPACC所访问的Slave侧返回ERROR响应中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  dsp_int_en_i1          : 1;  /* bit[21]   : 载波1的各中断组合发给DSP使能位。
                                                                  0：不使能该组合中断。
                                                                  1：使能该组合中断。 */
        unsigned int  arm_int_en_i1          : 1;  /* bit[22]   : 载波1的各中断组合发给ARM使能位。
                                                                  0：不使能该组合中断。
                                                                  1：使能该组合中断。 */
        unsigned int  reserved               : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_UPACC_INT_EN_I1_UNION;
#endif
#define SOC_UPACC_INT_EN_I1_encap_end_en_i1_START         (0)
#define SOC_UPACC_INT_EN_I1_encap_end_en_i1_END           (0)
#define SOC_UPACC_INT_EN_I1_trans_end_en_i1_START         (1)
#define SOC_UPACC_INT_EN_I1_trans_end_en_i1_END           (1)
#define SOC_UPACC_INT_EN_I1_addr1_error_en_i1_START       (2)
#define SOC_UPACC_INT_EN_I1_addr1_error_en_i1_END         (2)
#define SOC_UPACC_INT_EN_I1_addr2_error_en_i1_START       (3)
#define SOC_UPACC_INT_EN_I1_addr2_error_en_i1_END         (3)
#define SOC_UPACC_INT_EN_I1_addr3_error_en_i1_START       (4)
#define SOC_UPACC_INT_EN_I1_addr3_error_en_i1_END         (4)
#define SOC_UPACC_INT_EN_I1_addr4_error_en_i1_START       (5)
#define SOC_UPACC_INT_EN_I1_addr4_error_en_i1_END         (5)
#define SOC_UPACC_INT_EN_I1_addr5_error_en_i1_START       (6)
#define SOC_UPACC_INT_EN_I1_addr5_error_en_i1_END         (6)
#define SOC_UPACC_INT_EN_I1_addr6_error_en_i1_START       (7)
#define SOC_UPACC_INT_EN_I1_addr6_error_en_i1_END         (7)
#define SOC_UPACC_INT_EN_I1_addr7_error_en_i1_START       (8)
#define SOC_UPACC_INT_EN_I1_addr7_error_en_i1_END         (8)
#define SOC_UPACC_INT_EN_I1_addr8_error_en_i1_START       (9)
#define SOC_UPACC_INT_EN_I1_addr8_error_en_i1_END         (9)
#define SOC_UPACC_INT_EN_I1_addr9_error_en_i1_START       (10)
#define SOC_UPACC_INT_EN_I1_addr9_error_en_i1_END         (10)
#define SOC_UPACC_INT_EN_I1_addr10_error_en_i1_START      (11)
#define SOC_UPACC_INT_EN_I1_addr10_error_en_i1_END        (11)
#define SOC_UPACC_INT_EN_I1_addr11_error_en_i1_START      (12)
#define SOC_UPACC_INT_EN_I1_addr11_error_en_i1_END        (12)
#define SOC_UPACC_INT_EN_I1_addr12_error_en_i1_START      (13)
#define SOC_UPACC_INT_EN_I1_addr12_error_en_i1_END        (13)
#define SOC_UPACC_INT_EN_I1_addr13_error_en_i1_START      (14)
#define SOC_UPACC_INT_EN_I1_addr13_error_en_i1_END        (14)
#define SOC_UPACC_INT_EN_I1_addr14_error_en_i1_START      (15)
#define SOC_UPACC_INT_EN_I1_addr14_error_en_i1_END        (15)
#define SOC_UPACC_INT_EN_I1_param_addr_error_en_i1_START  (16)
#define SOC_UPACC_INT_EN_I1_param_addr_error_en_i1_END    (16)
#define SOC_UPACC_INT_EN_I1_harq_addr_error_en_i1_START   (17)
#define SOC_UPACC_INT_EN_I1_harq_addr_error_en_i1_END     (17)
#define SOC_UPACC_INT_EN_I1_dest_addr_eorr_en_i1_START    (18)
#define SOC_UPACC_INT_EN_I1_dest_addr_eorr_en_i1_END      (18)
#define SOC_UPACC_INT_EN_I1_loch_num_error_en_i1_START    (19)
#define SOC_UPACC_INT_EN_I1_loch_num_error_en_i1_END      (19)
#define SOC_UPACC_INT_EN_I1_bus_error_en_i1_START         (20)
#define SOC_UPACC_INT_EN_I1_bus_error_en_i1_END           (20)
#define SOC_UPACC_INT_EN_I1_dsp_int_en_i1_START           (21)
#define SOC_UPACC_INT_EN_I1_dsp_int_en_i1_END             (21)
#define SOC_UPACC_INT_EN_I1_arm_int_en_i1_START           (22)
#define SOC_UPACC_INT_EN_I1_arm_int_en_i1_END             (22)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_RAW_I1_UNION
 结构说明  : INT_RAW_I1 寄存器结构定义。地址偏移量:0x0134，初值:0x00000000，宽度:32
 寄存器说明: 原始中断状态寄存器。用于供CPU查询各中断信号在屏蔽前的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_raw_i1        : 1;  /* bit[0]    : MAC-i/is PDU封装完成原始中断位。封装操作完成时有效。
                                                                   高有效。 */
        unsigned int  trans_end_raw_i1        : 1;  /* bit[1]    : MAC-i/is PDU搬移装完成原始中断位。搬移操作完成时有效。
                                                                   高有效。 */
        unsigned int  addr1_error_raw_i1      : 1;  /* bit[2]    : MAC-i/is PDU逻辑信道2读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。 
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr2_error_raw_i1      : 1;  /* bit[3]    : MAC-i/is PDU逻辑信道1读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr3_error_raw_i1      : 1;  /* bit[4]    : MAC-i/is PDU逻辑信道3读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr4_error_raw_i1      : 1;  /* bit[5]    : MAC-i/is PDU逻辑信道4读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr5_error_raw_i1      : 1;  /* bit[6]    : MAC-i/is PDU逻辑信道5读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr6_error_raw_i1      : 1;  /* bit[7]    : MAC-i/is PDU逻辑信道6读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr7_error_raw_i1      : 1;  /* bit[8]    : MAC-i/is PDU逻辑信道7读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr8_error_raw_i1      : 1;  /* bit[9]    : MAC-i/is PDU逻辑信道8读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。 
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr9_error_raw_i1      : 1;  /* bit[10]   : MAC-i/is PDU逻辑信道9读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。 
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr10_error_raw_i1     : 1;  /* bit[11]   : MAC-i/is PDU逻辑信道10读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr11_error_raw_i1     : 1;  /* bit[12]   : MAC-i/is PDU逻辑信道11读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr12_error_raw_i1     : 1;  /* bit[13]   : MAC-i/is PDU逻辑信道12读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr13_error_raw_i1     : 1;  /* bit[14]   : MAC-i/is PDU逻辑信道13读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  addr14_error_raw_i1     : 1;  /* bit[15]   : MAC-i/is PDU逻辑信道14读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   源数据数组方式时，读入的源数据数组首地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  param_addr_error_raw_i1 : 1;  /* bit[16]   : MAC-i/is PDU参数链表地址配置错误原始中断位。参数链表地址最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  harq_addr_error_raw_i1  : 1;  /* bit[17]   : MAC-i/is PDU封装目标地址也即搬移起始地址配置错误原始中断位。Harq buufer起始地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  dest_addr_eorr_raw_i1   : 1;  /* bit[18]   : MAC-i/is PDU搬移目标地址配置错误原始中断位。Bbp编码缓存起始地址最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  loch_num_error_raw_i1   : 1;  /* bit[19]   : 逻辑信道个数配置错误原始中断位。逻辑信道个数不在[0,14]范围之内时有效。
                                                                   高有效。 */
        unsigned int  bus_error_raw_i1        : 1;  /* bit[20]   : UPACC所访问的Slave侧返回ERROR响应原始中断位。Slave侧返回ERROR响应时有效。
                                                                   高有效。 */
        unsigned int  dsp_int_raw_i1          : 1;  /* bit[21]   : 载波1的各组合中断发生给DSP原始中断位。
                                                                   高有效。 */
        unsigned int  arm_int_raw_i1          : 1;  /* bit[22]   : 载波1的各组合中断发生给ARM原始中断位。
                                                                   高有效。 */
        unsigned int  reserved                : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_UPACC_INT_RAW_I1_UNION;
#endif
#define SOC_UPACC_INT_RAW_I1_encap_end_raw_i1_START         (0)
#define SOC_UPACC_INT_RAW_I1_encap_end_raw_i1_END           (0)
#define SOC_UPACC_INT_RAW_I1_trans_end_raw_i1_START         (1)
#define SOC_UPACC_INT_RAW_I1_trans_end_raw_i1_END           (1)
#define SOC_UPACC_INT_RAW_I1_addr1_error_raw_i1_START       (2)
#define SOC_UPACC_INT_RAW_I1_addr1_error_raw_i1_END         (2)
#define SOC_UPACC_INT_RAW_I1_addr2_error_raw_i1_START       (3)
#define SOC_UPACC_INT_RAW_I1_addr2_error_raw_i1_END         (3)
#define SOC_UPACC_INT_RAW_I1_addr3_error_raw_i1_START       (4)
#define SOC_UPACC_INT_RAW_I1_addr3_error_raw_i1_END         (4)
#define SOC_UPACC_INT_RAW_I1_addr4_error_raw_i1_START       (5)
#define SOC_UPACC_INT_RAW_I1_addr4_error_raw_i1_END         (5)
#define SOC_UPACC_INT_RAW_I1_addr5_error_raw_i1_START       (6)
#define SOC_UPACC_INT_RAW_I1_addr5_error_raw_i1_END         (6)
#define SOC_UPACC_INT_RAW_I1_addr6_error_raw_i1_START       (7)
#define SOC_UPACC_INT_RAW_I1_addr6_error_raw_i1_END         (7)
#define SOC_UPACC_INT_RAW_I1_addr7_error_raw_i1_START       (8)
#define SOC_UPACC_INT_RAW_I1_addr7_error_raw_i1_END         (8)
#define SOC_UPACC_INT_RAW_I1_addr8_error_raw_i1_START       (9)
#define SOC_UPACC_INT_RAW_I1_addr8_error_raw_i1_END         (9)
#define SOC_UPACC_INT_RAW_I1_addr9_error_raw_i1_START       (10)
#define SOC_UPACC_INT_RAW_I1_addr9_error_raw_i1_END         (10)
#define SOC_UPACC_INT_RAW_I1_addr10_error_raw_i1_START      (11)
#define SOC_UPACC_INT_RAW_I1_addr10_error_raw_i1_END        (11)
#define SOC_UPACC_INT_RAW_I1_addr11_error_raw_i1_START      (12)
#define SOC_UPACC_INT_RAW_I1_addr11_error_raw_i1_END        (12)
#define SOC_UPACC_INT_RAW_I1_addr12_error_raw_i1_START      (13)
#define SOC_UPACC_INT_RAW_I1_addr12_error_raw_i1_END        (13)
#define SOC_UPACC_INT_RAW_I1_addr13_error_raw_i1_START      (14)
#define SOC_UPACC_INT_RAW_I1_addr13_error_raw_i1_END        (14)
#define SOC_UPACC_INT_RAW_I1_addr14_error_raw_i1_START      (15)
#define SOC_UPACC_INT_RAW_I1_addr14_error_raw_i1_END        (15)
#define SOC_UPACC_INT_RAW_I1_param_addr_error_raw_i1_START  (16)
#define SOC_UPACC_INT_RAW_I1_param_addr_error_raw_i1_END    (16)
#define SOC_UPACC_INT_RAW_I1_harq_addr_error_raw_i1_START   (17)
#define SOC_UPACC_INT_RAW_I1_harq_addr_error_raw_i1_END     (17)
#define SOC_UPACC_INT_RAW_I1_dest_addr_eorr_raw_i1_START    (18)
#define SOC_UPACC_INT_RAW_I1_dest_addr_eorr_raw_i1_END      (18)
#define SOC_UPACC_INT_RAW_I1_loch_num_error_raw_i1_START    (19)
#define SOC_UPACC_INT_RAW_I1_loch_num_error_raw_i1_END      (19)
#define SOC_UPACC_INT_RAW_I1_bus_error_raw_i1_START         (20)
#define SOC_UPACC_INT_RAW_I1_bus_error_raw_i1_END           (20)
#define SOC_UPACC_INT_RAW_I1_dsp_int_raw_i1_START           (21)
#define SOC_UPACC_INT_RAW_I1_dsp_int_raw_i1_END             (21)
#define SOC_UPACC_INT_RAW_I1_arm_int_raw_i1_START           (22)
#define SOC_UPACC_INT_RAW_I1_arm_int_raw_i1_END             (22)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_MASK_I1_UNION
 结构说明  : INT_MASK_I1 寄存器结构定义。地址偏移量:0x0138，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后中断状态寄存器。用于供CPU查询各中断信号在屏蔽后的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_msk_i1        : 1;  /* bit[0]    : MAC-i/is PDU封装完成屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  trans_end_msk_i1        : 1;  /* bit[1]    : MAC-i/is PDU搬移装完成屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr1_error_msk_i1      : 1;  /* bit[2]    : MAC-i/is PDU逻辑信道2读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr2_error_msk_i1      : 1;  /* bit[3]    : MAC-i/is PDU逻辑信道1读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr3_error_msk_i1      : 1;  /* bit[4]    : MAC-i/is PDU逻辑信道3读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr4_error_msk_i1      : 1;  /* bit[5]    : MAC-i/is PDU逻辑信道4读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr5_error_msk_i1      : 1;  /* bit[6]    : MAC-i/is PDU逻辑信道5读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr6_error_msk_i1      : 1;  /* bit[7]    : MAC-i/is PDU逻辑信道6读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr7_error_msk_i1      : 1;  /* bit[8]    : MAC-i/is PDU逻辑信道7读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr8_error_msk_i1      : 1;  /* bit[9]    : MAC-i/is PDU逻辑信道8读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr9_error_msk_i1      : 1;  /* bit[10]   : MAC-i/is PDU逻辑信道9读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr10_error_msk_i1     : 1;  /* bit[11]   : MAC-i/is PDU逻辑信道10读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr11_error_msk_i1     : 1;  /* bit[12]   : MAC-i/is PDU逻辑信道11读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr12_error_msk_i1     : 1;  /* bit[13]   : MAC-i/is PDU逻辑信道12读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr13_error_msk_i1     : 1;  /* bit[14]   : MAC-i/is PDU逻辑信道13读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr14_error_msk_i1     : 1;  /* bit[15]   : MAC-i/is PDU逻辑信道14读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  param_addr_error_msk_i1 : 1;  /* bit[16]   : MAC-i/is PDU参数链表地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  harq_addr_error_msk_i1  : 1;  /* bit[17]   : MAC-i/is PDU封装目标地址也即搬移起始地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  dest_addr_eorr_msk_i1   : 1;  /* bit[18]   : MAC-i/is PDU搬移目标地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  loch_num_error_msk_i1   : 1;  /* bit[19]   : 逻辑信道个数配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  bus_error_msk_i1        : 1;  /* bit[20]   : UPACC所访问的Slave侧返回ERROR响应屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  dsp_int_msk_i1          : 1;  /* bit[21]   : 载波1的各组合中断发生给DSP屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  arm_int_msk_i1          : 1;  /* bit[22]   : 载波1的各组合中断发生给ARM屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  reserved                : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_UPACC_INT_MASK_I1_UNION;
#endif
#define SOC_UPACC_INT_MASK_I1_encap_end_msk_i1_START         (0)
#define SOC_UPACC_INT_MASK_I1_encap_end_msk_i1_END           (0)
#define SOC_UPACC_INT_MASK_I1_trans_end_msk_i1_START         (1)
#define SOC_UPACC_INT_MASK_I1_trans_end_msk_i1_END           (1)
#define SOC_UPACC_INT_MASK_I1_addr1_error_msk_i1_START       (2)
#define SOC_UPACC_INT_MASK_I1_addr1_error_msk_i1_END         (2)
#define SOC_UPACC_INT_MASK_I1_addr2_error_msk_i1_START       (3)
#define SOC_UPACC_INT_MASK_I1_addr2_error_msk_i1_END         (3)
#define SOC_UPACC_INT_MASK_I1_addr3_error_msk_i1_START       (4)
#define SOC_UPACC_INT_MASK_I1_addr3_error_msk_i1_END         (4)
#define SOC_UPACC_INT_MASK_I1_addr4_error_msk_i1_START       (5)
#define SOC_UPACC_INT_MASK_I1_addr4_error_msk_i1_END         (5)
#define SOC_UPACC_INT_MASK_I1_addr5_error_msk_i1_START       (6)
#define SOC_UPACC_INT_MASK_I1_addr5_error_msk_i1_END         (6)
#define SOC_UPACC_INT_MASK_I1_addr6_error_msk_i1_START       (7)
#define SOC_UPACC_INT_MASK_I1_addr6_error_msk_i1_END         (7)
#define SOC_UPACC_INT_MASK_I1_addr7_error_msk_i1_START       (8)
#define SOC_UPACC_INT_MASK_I1_addr7_error_msk_i1_END         (8)
#define SOC_UPACC_INT_MASK_I1_addr8_error_msk_i1_START       (9)
#define SOC_UPACC_INT_MASK_I1_addr8_error_msk_i1_END         (9)
#define SOC_UPACC_INT_MASK_I1_addr9_error_msk_i1_START       (10)
#define SOC_UPACC_INT_MASK_I1_addr9_error_msk_i1_END         (10)
#define SOC_UPACC_INT_MASK_I1_addr10_error_msk_i1_START      (11)
#define SOC_UPACC_INT_MASK_I1_addr10_error_msk_i1_END        (11)
#define SOC_UPACC_INT_MASK_I1_addr11_error_msk_i1_START      (12)
#define SOC_UPACC_INT_MASK_I1_addr11_error_msk_i1_END        (12)
#define SOC_UPACC_INT_MASK_I1_addr12_error_msk_i1_START      (13)
#define SOC_UPACC_INT_MASK_I1_addr12_error_msk_i1_END        (13)
#define SOC_UPACC_INT_MASK_I1_addr13_error_msk_i1_START      (14)
#define SOC_UPACC_INT_MASK_I1_addr13_error_msk_i1_END        (14)
#define SOC_UPACC_INT_MASK_I1_addr14_error_msk_i1_START      (15)
#define SOC_UPACC_INT_MASK_I1_addr14_error_msk_i1_END        (15)
#define SOC_UPACC_INT_MASK_I1_param_addr_error_msk_i1_START  (16)
#define SOC_UPACC_INT_MASK_I1_param_addr_error_msk_i1_END    (16)
#define SOC_UPACC_INT_MASK_I1_harq_addr_error_msk_i1_START   (17)
#define SOC_UPACC_INT_MASK_I1_harq_addr_error_msk_i1_END     (17)
#define SOC_UPACC_INT_MASK_I1_dest_addr_eorr_msk_i1_START    (18)
#define SOC_UPACC_INT_MASK_I1_dest_addr_eorr_msk_i1_END      (18)
#define SOC_UPACC_INT_MASK_I1_loch_num_error_msk_i1_START    (19)
#define SOC_UPACC_INT_MASK_I1_loch_num_error_msk_i1_END      (19)
#define SOC_UPACC_INT_MASK_I1_bus_error_msk_i1_START         (20)
#define SOC_UPACC_INT_MASK_I1_bus_error_msk_i1_END           (20)
#define SOC_UPACC_INT_MASK_I1_dsp_int_msk_i1_START           (21)
#define SOC_UPACC_INT_MASK_I1_dsp_int_msk_i1_END             (21)
#define SOC_UPACC_INT_MASK_I1_arm_int_msk_i1_START           (22)
#define SOC_UPACC_INT_MASK_I1_arm_int_msk_i1_END             (22)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_CLR_I1_UNION
 结构说明  : INT_CLR_I1 寄存器结构定义。地址偏移量:0x013C，初值:0x00000000，宽度:32
 寄存器说明: 中断状态清除寄存器。用于供CPU将各中断信号状态清零。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_clr_i1        : 1;  /* bit[0]    : MAC-i/is PDU封装完成中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  trans_end_clr_i1        : 1;  /* bit[1]    : MAC-i/is PDU搬移装完成中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr1_error_clr_i1      : 1;  /* bit[2]    : MAC-i/is PDU逻辑信道2读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr2_error_clr_i1      : 1;  /* bit[3]    : MAC-i/is PDU逻辑信道1读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr3_error_clr_i1      : 1;  /* bit[4]    : MAC-i/is PDU逻辑信道3读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr4_error_clr_i1      : 1;  /* bit[5]    : MAC-i/is PDU逻辑信道4读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr5_error_clr_i1      : 1;  /* bit[6]    : MAC-i/is PDU逻辑信道5读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr6_error_clr_i1      : 1;  /* bit[7]    : MAC-i/is PDU逻辑信道6读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr7_error_clr_i1      : 1;  /* bit[8]    : MAC-i/is PDU逻辑信道7读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr8_error_clr_i1      : 1;  /* bit[9]    : MAC-i/is PDU逻辑信道8读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr9_error_clr_i1      : 1;  /* bit[10]   : MAC-i/is PDU逻辑信道9读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr10_error_clr_i1     : 1;  /* bit[11]   : MAC-i/is PDU逻辑信道10读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr11_error_clr_i1     : 1;  /* bit[12]   : MAC-i/is PDU逻辑信道11读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr12_error_clr_i1     : 1;  /* bit[13]   : MAC-i/is PDU逻辑信道12读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr13_error_clr_i1     : 1;  /* bit[14]   : MAC-i/is PDU逻辑信道13读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr14_error_clr_i1     : 1;  /* bit[15]   : MAC-i/is PDU逻辑信道14读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  param_addr_error_clr_i1 : 1;  /* bit[16]   : 参数链表地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  harq_addr_error_clr_i1  : 1;  /* bit[17]   : MAC-i/is PDU封装目标地址也即搬移起始地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  dest_addr_eorr_clr_i1   : 1;  /* bit[18]   : MAC-i/is PDU搬移目标地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  loch_num_error_clr_i1   : 1;  /* bit[19]   : 逻辑信道个数配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  bus_error_clr_i1        : 1;  /* bit[20]   : UPACC所访问的Slave侧返回ERROR响应中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  dsp_int_clr_i1          : 1;  /* bit[21]   : 载波1的各组合中断发生给DSP中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  arm_int_clr_i1          : 1;  /* bit[22]   : 载波1的各组合中断发生给ARM中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  reserved                : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_UPACC_INT_CLR_I1_UNION;
#endif
#define SOC_UPACC_INT_CLR_I1_encap_end_clr_i1_START         (0)
#define SOC_UPACC_INT_CLR_I1_encap_end_clr_i1_END           (0)
#define SOC_UPACC_INT_CLR_I1_trans_end_clr_i1_START         (1)
#define SOC_UPACC_INT_CLR_I1_trans_end_clr_i1_END           (1)
#define SOC_UPACC_INT_CLR_I1_addr1_error_clr_i1_START       (2)
#define SOC_UPACC_INT_CLR_I1_addr1_error_clr_i1_END         (2)
#define SOC_UPACC_INT_CLR_I1_addr2_error_clr_i1_START       (3)
#define SOC_UPACC_INT_CLR_I1_addr2_error_clr_i1_END         (3)
#define SOC_UPACC_INT_CLR_I1_addr3_error_clr_i1_START       (4)
#define SOC_UPACC_INT_CLR_I1_addr3_error_clr_i1_END         (4)
#define SOC_UPACC_INT_CLR_I1_addr4_error_clr_i1_START       (5)
#define SOC_UPACC_INT_CLR_I1_addr4_error_clr_i1_END         (5)
#define SOC_UPACC_INT_CLR_I1_addr5_error_clr_i1_START       (6)
#define SOC_UPACC_INT_CLR_I1_addr5_error_clr_i1_END         (6)
#define SOC_UPACC_INT_CLR_I1_addr6_error_clr_i1_START       (7)
#define SOC_UPACC_INT_CLR_I1_addr6_error_clr_i1_END         (7)
#define SOC_UPACC_INT_CLR_I1_addr7_error_clr_i1_START       (8)
#define SOC_UPACC_INT_CLR_I1_addr7_error_clr_i1_END         (8)
#define SOC_UPACC_INT_CLR_I1_addr8_error_clr_i1_START       (9)
#define SOC_UPACC_INT_CLR_I1_addr8_error_clr_i1_END         (9)
#define SOC_UPACC_INT_CLR_I1_addr9_error_clr_i1_START       (10)
#define SOC_UPACC_INT_CLR_I1_addr9_error_clr_i1_END         (10)
#define SOC_UPACC_INT_CLR_I1_addr10_error_clr_i1_START      (11)
#define SOC_UPACC_INT_CLR_I1_addr10_error_clr_i1_END        (11)
#define SOC_UPACC_INT_CLR_I1_addr11_error_clr_i1_START      (12)
#define SOC_UPACC_INT_CLR_I1_addr11_error_clr_i1_END        (12)
#define SOC_UPACC_INT_CLR_I1_addr12_error_clr_i1_START      (13)
#define SOC_UPACC_INT_CLR_I1_addr12_error_clr_i1_END        (13)
#define SOC_UPACC_INT_CLR_I1_addr13_error_clr_i1_START      (14)
#define SOC_UPACC_INT_CLR_I1_addr13_error_clr_i1_END        (14)
#define SOC_UPACC_INT_CLR_I1_addr14_error_clr_i1_START      (15)
#define SOC_UPACC_INT_CLR_I1_addr14_error_clr_i1_END        (15)
#define SOC_UPACC_INT_CLR_I1_param_addr_error_clr_i1_START  (16)
#define SOC_UPACC_INT_CLR_I1_param_addr_error_clr_i1_END    (16)
#define SOC_UPACC_INT_CLR_I1_harq_addr_error_clr_i1_START   (17)
#define SOC_UPACC_INT_CLR_I1_harq_addr_error_clr_i1_END     (17)
#define SOC_UPACC_INT_CLR_I1_dest_addr_eorr_clr_i1_START    (18)
#define SOC_UPACC_INT_CLR_I1_dest_addr_eorr_clr_i1_END      (18)
#define SOC_UPACC_INT_CLR_I1_loch_num_error_clr_i1_START    (19)
#define SOC_UPACC_INT_CLR_I1_loch_num_error_clr_i1_END      (19)
#define SOC_UPACC_INT_CLR_I1_bus_error_clr_i1_START         (20)
#define SOC_UPACC_INT_CLR_I1_bus_error_clr_i1_END           (20)
#define SOC_UPACC_INT_CLR_I1_dsp_int_clr_i1_START           (21)
#define SOC_UPACC_INT_CLR_I1_dsp_int_clr_i1_END             (21)
#define SOC_UPACC_INT_CLR_I1_arm_int_clr_i1_START           (22)
#define SOC_UPACC_INT_CLR_I1_arm_int_clr_i1_END             (22)


/*****************************************************************************
 结构名    : SOC_UPACC_BUS_ERROR_ADDR_I1_UNION
 结构说明  : BUS_ERROR_ADDR_I1 寄存器结构定义。地址偏移量:0x0140，初值:0x00000000，宽度:32
 寄存器说明: 总线ERROR地址寄存器。用于寄存Slave侧返回ERROR响应时的当前操作地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bus_error_addr_i1 : 32; /* bit[0-31]: 用于寄存UPACC所访问的Slave侧放回ERROR响应时的当前操作地址。 */
    } reg;
} SOC_UPACC_BUS_ERROR_ADDR_I1_UNION;
#endif
#define SOC_UPACC_BUS_ERROR_ADDR_I1_bus_error_addr_i1_START  (0)
#define SOC_UPACC_BUS_ERROR_ADDR_I1_bus_error_addr_i1_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_EN_I1_UNION
 结构说明  : EN_I1 寄存器结构定义。地址偏移量:0x0144，初值:0x00000000，宽度:32
 寄存器说明: MAC-i/is PDU封装及搬移模块使能寄存器。用于配置MAC-i/is PDU封装及搬移模块使能与否。
            UPACC_EN_I1为MAC-i/is PDU封装及搬移模块启动寄存器。用于配置MAC-i/is PDU封装及搬移模块使能与否。
            DSP配置完其他封装参数后，通过设置该寄存器的bit0为1启动MAC-i/is PDU封装及搬移操作。封装及搬移操作完成后，硬件可自动对该寄存器清零。DSP在不需启用MAC-i/is PDU封装及搬移模块时，也应将该寄存器的bit0置为0，以降低功耗。
            当软件参数配置出错，即INT_RAW_I1中的addri_error_raw_i1、loch_loch_num_error_raw_i1、dest_addr_error_raw_i1等任意一个有效时，硬件逻辑会对该寄存器的bit0自动清零，同时对寄存器MAC_ENCAP_START_I1的bit[1:0]进行清零，以复位UPACC内部的MAC-i/is  PDU封装及搬运模块，使其内部状态机返回IDLE态。
            当软件发现UPACC封装及搬运超时或异常时，可对该寄存器的bit0写0，将UPACC内部MAC-i/is PDU封装及搬运模块的状态机复位，使其返回到IDLE态。此时，软件还需对寄存器MAC_ENCAP_START_I1的bit[1:0]进行清零，以防止异常启动。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  upacc_en_i1 : 1;  /* bit[0]   : 用于配置MAC-I PDU封装及搬移模块使能与否。
                                                      0：禁止。UPACC内部MAC-I PDU封装及搬运模块UPACC_ACC_I不工作，内部状态回到IDLE状态。
                                                      1：使能。UPACC内部MAC-I PDU封装及搬运模块UPACC_ACC_I处于工作状态。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_UPACC_EN_I1_UNION;
#endif
#define SOC_UPACC_EN_I1_upacc_en_i1_START  (0)
#define SOC_UPACC_EN_I1_upacc_en_i1_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_HARQ_WRITE_ADDR_I1_UNION
 结构说明  : HARQ_WRITE_ADDR_I1 寄存器结构定义。地址偏移量:0x0148，初值:0x00000000，宽度:32
 寄存器说明: HARQ buffer写地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  harq_write_addr_i1 : 32; /* bit[0-31]: 用于调试时计算当次封装处理完成后，UPACC实际写入HARQ buffer的数据Byte个数。
                                                             若启动方式为封装且搬移或者仅搬移，则当封装处理结束，即INT_RAW_I1的bit[0]封装完成原始中断位encap_end_raw_i1有效之后，寄存器HARQ_WRITE_ADDR_I1的值与寄存器HARQ_BUF_ADDR_I1的值相减的结果，就是当次封装处理过程中，UPACC实际写入HARQ buffer中的数据Byte个数。
                                                             值得注意的是，由于BBP仅支持Word访问，因此UPACC内部逻辑设计中对不足整Word的数据，通过在高位补零凑成整Word后，写回外部Memory。所以，两个寄存器的值直接相减的结果，会比当次封装处理需要搬移到HARQ buffer中的数据量要略大一些，误差值最大为7Byte。具体误差值与si_en_i1、padding_len_i1等封装参数有关。不同配置下的误差值计算方法请参见《实际封装数据个数与所需封装数据个数在不同配置下的误差列表.xlsx》。 */
    } reg;
} SOC_UPACC_HARQ_WRITE_ADDR_I1_UNION;
#endif
#define SOC_UPACC_HARQ_WRITE_ADDR_I1_harq_write_addr_i1_START  (0)
#define SOC_UPACC_HARQ_WRITE_ADDR_I1_harq_write_addr_i1_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_BBP_WRITE_ADDR_I1_UNION
 结构说明  : BBP_WRITE_ADDR_I1 寄存器结构定义。地址偏移量:0x014C，初值:0x00000000，宽度:32
 寄存器说明: BBP 编码缓存写地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bbp_write_addr_i1 : 32; /* bit[0-31]: 用于调试时计算当次封装处理完成后，UPACC实际写入BBP buffer的数据Byte个数。
                                                            若启动方式为封装且搬移或者仅搬移，则当次封装处理结束，即INT_RAW_I1的bit[1]搬移完成原始中断位trans_end_raw_i1有效后，寄存器BBP_WRITE_ADDR_I1的值与寄存器BBP_DEST_ADDR_I1的值相减的结果，就是当次封装处理过程中，UPACC实际写入BBP编码缓存中的数据Byte个数。
                                                            值得注意的是，由于BBP仅支持Word访问，因此UPACC内部逻辑设计中对不足整Word的数据，通过在高位补零凑成整Word后，写回外部Memory。所以，两个寄存器的值直接相减的结果，会比当次封装处理需要搬移到BBP编码缓存中的数据量要略大一些，误差值最大为7Byte。具体误差值与si_en_i1、padding_len_i1等封装参数或者trans_num_i1有关。不同配置下的误差值计算方法请参见《实际封装数据个数与所需封装数据个数在不同配置下的误差列表.xlsx》。 */
    } reg;
} SOC_UPACC_BBP_WRITE_ADDR_I1_UNION;
#endif
#define SOC_UPACC_BBP_WRITE_ADDR_I1_bbp_write_addr_i1_START  (0)
#define SOC_UPACC_BBP_WRITE_ADDR_I1_bbp_write_addr_i1_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_PDUARY_EN_I1_UNION
 结构说明  : PDUARY_EN_I1 寄存器结构定义。地址偏移量:0x0150，初值:0x00000000，宽度:32
 寄存器说明: Mac-i PDU载波1源数据数组方式配置使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pduary_en_i1 : 1;  /* bit[0]   : Mac-i PDU载波1的源数据数组方式配置使能信号。
                                                       0：禁止，源数据不采用数组方式配置，兼容UPACCV200；
                                                       1：使能，源数据采用数组方式配置。 */
        unsigned int  reserved     : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_UPACC_PDUARY_EN_I1_UNION;
#endif
#define SOC_UPACC_PDUARY_EN_I1_pduary_en_i1_START  (0)
#define SOC_UPACC_PDUARY_EN_I1_pduary_en_i1_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_MAC_ENCAP_START_I2_UNION
 结构说明  : MAC_ENCAP_START_I2 寄存器结构定义。地址偏移量:0x0200，初值:0x00000000，宽度:32
 寄存器说明: 启动方式配置寄存器。用于配置MAC_i/is PDU封装的启动方式。
            UPACCV200支持对MAC-i/is PDU 进行不封装只搬移和封装且搬移两种启动方式。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mac_encap_start_i2 : 2;  /* bit[0-1] : 00：不启动MAC-i/is PDU封装，也不启动MAC-i/is PDU搬移；
                                                             01：仅启动MAC-i/is PDU封装，封装好的数据仅存放到HARQ Buffer中，不存放到BBP编码缓存中。不启动MAC-i/is PDU搬移；这种启动方式仅用于调试。
                                                             10：不启动MAC-i/is PDU封装，仅启动MAC-i/is PDU搬移；将HARQ Buffer中指定位置处的数据搬移到BBP编码缓存中。这种启动方式应用于重传MAC-i/is PDU的场景。
                                                             11：启动MAC-i/is PDU封装，并将封装好的数据同时分别存放到HARQ Buffer和BBP编码缓存中。这种启动方式应用于新传MAC-i/is PDU的场景。
                                                             当MAC-i/is PDU封装完成后，即INT_RAW_I1寄存器中的encap_end_raw_i1有效后，硬件逻辑会对该寄存器的bit0自动清零；
                                                             当MAC-i/is PDU搬移完成后，即INT_RAW_I1寄存器中的trans_end_raw_i1有效后，硬件逻辑会对该寄存器的bit1自动清零。
                                                             当软件参数配置出错，即INT_RAW_I1中的addrn_error_raw_i1、loch_num_error_raw_i1、dest_addr_error_raw_i1等任意一个有效时，硬件逻辑会对该寄存器的bit0和bit1同时清零。 */
        unsigned int  reserved           : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_UPACC_MAC_ENCAP_START_I2_UNION;
#endif
#define SOC_UPACC_MAC_ENCAP_START_I2_mac_encap_start_i2_START  (0)
#define SOC_UPACC_MAC_ENCAP_START_I2_mac_encap_start_i2_END    (1)


/*****************************************************************************
 结构名    : SOC_UPACC_LOCH_NUM_I2_UNION
 结构说明  : LOCH_NUM_I2 寄存器结构定义。地址偏移量:0x0204，初值:0x00000000，宽度:32
 寄存器说明: 逻辑信道个数寄存器。用于指示当次封装MAC-i/is PDU共需从几个逻辑信道中读取数据。若该寄存器内容配置为0，表示当前MAC-i/is PDU仅由SI组成，没有其他数据和参数，即为SI单独发送。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  loch_num_i2 : 4;  /* bit[0-3] : 用于指示当次封装MAC-i/is PDU共需从几个信道中提取数据。
                                                      若该参数配置为0，则表示当次为SI单独发送，无其他数据和参数。 */
        unsigned int  reserved    : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_UPACC_LOCH_NUM_I2_UNION;
#endif
#define SOC_UPACC_LOCH_NUM_I2_loch_num_i2_START  (0)
#define SOC_UPACC_LOCH_NUM_I2_loch_num_i2_END    (3)


/*****************************************************************************
 结构名    : SOC_UPACC_HEAD0_EN_I2_UNION
 结构说明  : HEAD0_EN_I2 寄存器结构定义。地址偏移量:0x0208，初值:0x00000000，宽度:32
 寄存器说明: MAC-i header0使能寄存器。用于指示当次封装的MAC-i/is PDU中是否包含header0参数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  head0_en_i2 : 1;  /* bit[0]   : 0：MAC-i/is PDU中不包含header0参数。
                                                      1：MAC-i/is PDU中包含header0参数。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_UPACC_HEAD0_EN_I2_UNION;
#endif
#define SOC_UPACC_HEAD0_EN_I2_head0_en_i2_START  (0)
#define SOC_UPACC_HEAD0_EN_I2_head0_en_i2_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_E_RNTI_I2_UNION
 结构说明  : E_RNTI_I2 寄存器结构定义。地址偏移量:0x020C，初值:0x00000000，宽度:32
 寄存器说明: E_RNTI配置寄存器。用于配置MAC-i/is PDU header0中的E-RNTI参数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  e_rnti_i2 : 16; /* bit[0-15] : 当head0_en_i2为1时，用于配置MAC-i/is PDU header0中的E-RNTI参数。 */
        unsigned int  reserved  : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_UPACC_E_RNTI_I2_UNION;
#endif
#define SOC_UPACC_E_RNTI_I2_e_rnti_i2_START  (0)
#define SOC_UPACC_E_RNTI_I2_e_rnti_i2_END    (15)


/*****************************************************************************
 结构名    : SOC_UPACC_SI_EN_I2_UNION
 结构说明  : SI_EN_I2 寄存器结构定义。地址偏移量:0x0210，初值:0x00000000，宽度:32
 寄存器说明: SI使能寄存器。用于指示当次封装的MAC-i/is PDU中是否包含SI参数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  si_en_i2 : 1;  /* bit[0]   : 0：MAC-i/is PDU中不包含SI参数。
                                                   1：MAC-i/is PDU中包含SI参数。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_UPACC_SI_EN_I2_UNION;
#endif
#define SOC_UPACC_SI_EN_I2_si_en_i2_START  (0)
#define SOC_UPACC_SI_EN_I2_si_en_i2_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_SI_DATA_I2_UNION
 结构说明  : SI_DATA_I2 寄存器结构定义。地址偏移量:0x0214，初值:0x00000000，宽度:32
 寄存器说明: SI数据配置寄存器。用于配置MAC-i/is PDU中的SI参数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  si_hlid_i2 : 4;  /* bit[0-3]  : 当si_en_i2为1时，用于配置MAC-i/is PDU中SI参数中的HLID数据段。 */
        unsigned int  si_hlbs_i2 : 4;  /* bit[4-7]  : 当si_en_i2为1时，用于配置MAC-i/is PDU中SI参数中的HLBS数据段。 */
        unsigned int  si_tebs_i2 : 5;  /* bit[8-12] : 当si_en_i2为1时，用于配置MAC-i/is PDU中SI参数中的TEBS数据段。 */
        unsigned int  reserved_0 : 3;  /* bit[13-15]: 保留 */
        unsigned int  si_uph_i2  : 5;  /* bit[16-20]: 当si_en_i2为1时，用于配置MAC-i/is PDU中SI参数中的UPH数据段。 */
        unsigned int  reserved_1 : 11; /* bit[21-31]: 保留 */
    } reg;
} SOC_UPACC_SI_DATA_I2_UNION;
#endif
#define SOC_UPACC_SI_DATA_I2_si_hlid_i2_START  (0)
#define SOC_UPACC_SI_DATA_I2_si_hlid_i2_END    (3)
#define SOC_UPACC_SI_DATA_I2_si_hlbs_i2_START  (4)
#define SOC_UPACC_SI_DATA_I2_si_hlbs_i2_END    (7)
#define SOC_UPACC_SI_DATA_I2_si_tebs_i2_START  (8)
#define SOC_UPACC_SI_DATA_I2_si_tebs_i2_END    (12)
#define SOC_UPACC_SI_DATA_I2_si_uph_i2_START   (16)
#define SOC_UPACC_SI_DATA_I2_si_uph_i2_END     (20)


/*****************************************************************************
 结构名    : SOC_UPACC_PADDING_LEN_I2_UNION
 结构说明  : PADDING_LEN_I2 寄存器结构定义。地址偏移量:0x0218，初值:0x00000000，宽度:32
 寄存器说明: padding长度配置寄存器。用于配置MAC-i/is PDU中的padding参数的长度。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  padding_len_i2 : 16; /* bit[0-15] : 用于配置MAC-i/is PDU中padding参数的长度。以bit为单位。
                                                          最大允许配置值为16'hFFCD。
                                                          当padding_len_i2配置为0，表示当次封装的MAC-i/is PDU中没有padding参数。 */
        unsigned int  reserved       : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_UPACC_PADDING_LEN_I2_UNION;
#endif
#define SOC_UPACC_PADDING_LEN_I2_padding_len_i2_START  (0)
#define SOC_UPACC_PADDING_LEN_I2_padding_len_i2_END    (15)


/*****************************************************************************
 结构名    : SOC_UPACC_TSN_LEN_I2_UNION
 结构说明  : TSN_LEN_I2 寄存器结构定义。地址偏移量:0x021C，初值:0x00000000，宽度:32
 寄存器说明: TSN长度配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tsn_len_i2 : 1;  /* bit[0]   : 用于配置MAC-i/is PDU中TSN参数的长度。
                                                     1：TSN的长度为14bits。
                                                     0：TSN的长度为6bits。 */
        unsigned int  reserved   : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_UPACC_TSN_LEN_I2_UNION;
#endif
#define SOC_UPACC_TSN_LEN_I2_tsn_len_i2_START  (0)
#define SOC_UPACC_TSN_LEN_I2_tsn_len_i2_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_PARAM_ADDR_I2_UNION
 结构说明  : PARAM_ADDR_I2 寄存器结构定义。地址偏移量:0x0220，初值:0x00000000，宽度:32
 寄存器说明: 参数链表起始地址寄存器。用于配置参数链表在存储器中存放的起始地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  param_addr_i2 : 32; /* bit[0-31]: 用于配置MAC-i/is PDU封装时所需的参数链表在存储器中存放的起始地址。
                                                        以byte为单位。但是必须以double word地址进行配置，即该地址的低三位bit[2:0]必须为3'b000。 */
    } reg;
} SOC_UPACC_PARAM_ADDR_I2_UNION;
#endif
#define SOC_UPACC_PARAM_ADDR_I2_param_addr_i2_START  (0)
#define SOC_UPACC_PARAM_ADDR_I2_param_addr_i2_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_HARQ_BUF_ADDR_I2_UNION
 结构说明  : HARQ_BUF_ADDR_I2 寄存器结构定义。地址偏移量:0x0224，初值:0x00000000，宽度:32
 寄存器说明: 封装目标地址与搬移起始地址寄存器。用于配置当次MAC-i/is PDU封装的目的地址或搬运的起始地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hraq_buf_addr_i2 : 32; /* bit[0-31]: 用于配置当次MAC-i/is PDU封装的目的地址或搬运的起始地址。MAC-i/is PDU的封装目标地址与搬移起始地址是相同的，都由该寄存器配置。
                                                           以byte为单位。但是必须以double word地址进行配置，即该地址的低三位bit[2:0]必须为3'b000。 */
    } reg;
} SOC_UPACC_HARQ_BUF_ADDR_I2_UNION;
#endif
#define SOC_UPACC_HARQ_BUF_ADDR_I2_hraq_buf_addr_i2_START  (0)
#define SOC_UPACC_HARQ_BUF_ADDR_I2_hraq_buf_addr_i2_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_BBP_DEST_ADDR_I2_UNION
 结构说明  : BBP_DEST_ADDR_I2 寄存器结构定义。地址偏移量:0x0228，初值:0x00000000，宽度:32
 寄存器说明: 搬移目标地址寄存器。用于配置MAC-i/is PDU搬移到BBP编码缓存的目标地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bbp_dest_addr_i2 : 32; /* bit[0-31]: 用于配置MAC-i/is PDU搬移到BBP编码缓存的目标地址。
                                                           以byte为单位。但是必须以double word地址进行配置，即该地址的低三位bit[2:0]必须为3'b000。 */
    } reg;
} SOC_UPACC_BBP_DEST_ADDR_I2_UNION;
#endif
#define SOC_UPACC_BBP_DEST_ADDR_I2_bbp_dest_addr_i2_START  (0)
#define SOC_UPACC_BBP_DEST_ADDR_I2_bbp_dest_addr_i2_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_TRANS_NUM_I2_UNION
 结构说明  : TRANS_NUM_I2 寄存器结构定义。地址偏移量:0x022C，初值:0x00000000，宽度:32
 寄存器说明: 搬移数据大小寄存器。用于配置需要搬移到BBP的MAC-i/is PDU数据字节数。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trans_num_i2 : 16; /* bit[0-15] : 用于配置当次需搬运到BBP编码缓存中的MAC-i/is PDU数据大小。以byte为单位。但是由于BBP仅支持word访问，因此实际搬运时会按照整word进行搬移操作。
                                                        最大允许配置值为16'hFFFC。 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_UPACC_TRANS_NUM_I2_UNION;
#endif
#define SOC_UPACC_TRANS_NUM_I2_trans_num_i2_START  (0)
#define SOC_UPACC_TRANS_NUM_I2_trans_num_i2_END    (15)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_EN_I2_UNION
 结构说明  : INT_EN_I2 寄存器结构定义。地址偏移量:0x0230，初值:0x00000000，宽度:32
 寄存器说明: 中断使能寄存器。用于配置各中断信号使能与否。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_en_i2        : 1;  /* bit[0]    : MAC-i/is PDU封装完成中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  trans_end_en_i2        : 1;  /* bit[1]    : MAC-i/is PDU搬移完成中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr1_error_en_i2      : 1;  /* bit[2]    : MAC-i/is PDU逻辑信道1读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr2_error_en_i2      : 1;  /* bit[3]    : MAC-i/is PDU逻辑信道2读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr3_error_en_i2      : 1;  /* bit[4]    : MAC-i/is PDU逻辑信道3读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr4_error_en_i2      : 1;  /* bit[5]    : MAC-i/is PDU逻辑信道4读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr5_error_en_i2      : 1;  /* bit[6]    : MAC-i/is PDU逻辑信道5读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr6_error_en_i2      : 1;  /* bit[7]    : MAC-i/is PDU逻辑信道6读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr7_error_en_i2      : 1;  /* bit[8]    : MAC-i/is PDU逻辑信道7读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr8_error_en_i2      : 1;  /* bit[9]    : MAC-i/is PDU逻辑信道8读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr9_error_en_i2      : 1;  /* bit[10]   : MAC-i/is PDU逻辑信道9读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr10_error_en_i2     : 1;  /* bit[11]   : MAC-i/is PDU逻辑信道10读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr11_error_en_i2     : 1;  /* bit[12]   : MAC-i/is PDU逻辑信道11读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr12_error_en_i2     : 1;  /* bit[13]   : MAC-i/is PDU逻辑信道12读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr13_error_en_i2     : 1;  /* bit[14]   : MAC-i/is PDU逻辑信道13读地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  addr14_error_en_i2     : 1;  /* bit[15]   : MAC-i/is PDU逻辑信道14读地址配置错误中断。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  param_addr_error_en_i2 : 1;  /* bit[16]   : MAC-i/is PDU参数链表地址配置错误中断。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  harq_addr_error_en_i2  : 1;  /* bit[17]   : MAC-i/is PDU封装目标地址也即搬移起始地址配置错误中断。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  dest_addr_eorr_en_i2   : 1;  /* bit[18]   : MAC-i/is PDU搬移目标地址配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  loch_num_error_en_i2   : 1;  /* bit[19]   : 逻辑信道个数配置错误中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  bus_error_en_i2        : 1;  /* bit[20]   : UPACC所访问的Slave侧返回ERROR响应中断使能位。
                                                                  0：不使能该中断。
                                                                  1：使能该中断。 */
        unsigned int  dsp_int_en_i2          : 1;  /* bit[21]   : 载波2的各中断组合发给DSP使能位。
                                                                  0：不使能该组合中断。
                                                                  1：使能该组合中断。 */
        unsigned int  arm_int_en_12          : 1;  /* bit[22]   : 载波2的各中断组合发给ARM使能位。
                                                                  0：不使能该组合中断。
                                                                  1：使能该组合中断。 */
        unsigned int  reserved               : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_UPACC_INT_EN_I2_UNION;
#endif
#define SOC_UPACC_INT_EN_I2_encap_end_en_i2_START         (0)
#define SOC_UPACC_INT_EN_I2_encap_end_en_i2_END           (0)
#define SOC_UPACC_INT_EN_I2_trans_end_en_i2_START         (1)
#define SOC_UPACC_INT_EN_I2_trans_end_en_i2_END           (1)
#define SOC_UPACC_INT_EN_I2_addr1_error_en_i2_START       (2)
#define SOC_UPACC_INT_EN_I2_addr1_error_en_i2_END         (2)
#define SOC_UPACC_INT_EN_I2_addr2_error_en_i2_START       (3)
#define SOC_UPACC_INT_EN_I2_addr2_error_en_i2_END         (3)
#define SOC_UPACC_INT_EN_I2_addr3_error_en_i2_START       (4)
#define SOC_UPACC_INT_EN_I2_addr3_error_en_i2_END         (4)
#define SOC_UPACC_INT_EN_I2_addr4_error_en_i2_START       (5)
#define SOC_UPACC_INT_EN_I2_addr4_error_en_i2_END         (5)
#define SOC_UPACC_INT_EN_I2_addr5_error_en_i2_START       (6)
#define SOC_UPACC_INT_EN_I2_addr5_error_en_i2_END         (6)
#define SOC_UPACC_INT_EN_I2_addr6_error_en_i2_START       (7)
#define SOC_UPACC_INT_EN_I2_addr6_error_en_i2_END         (7)
#define SOC_UPACC_INT_EN_I2_addr7_error_en_i2_START       (8)
#define SOC_UPACC_INT_EN_I2_addr7_error_en_i2_END         (8)
#define SOC_UPACC_INT_EN_I2_addr8_error_en_i2_START       (9)
#define SOC_UPACC_INT_EN_I2_addr8_error_en_i2_END         (9)
#define SOC_UPACC_INT_EN_I2_addr9_error_en_i2_START       (10)
#define SOC_UPACC_INT_EN_I2_addr9_error_en_i2_END         (10)
#define SOC_UPACC_INT_EN_I2_addr10_error_en_i2_START      (11)
#define SOC_UPACC_INT_EN_I2_addr10_error_en_i2_END        (11)
#define SOC_UPACC_INT_EN_I2_addr11_error_en_i2_START      (12)
#define SOC_UPACC_INT_EN_I2_addr11_error_en_i2_END        (12)
#define SOC_UPACC_INT_EN_I2_addr12_error_en_i2_START      (13)
#define SOC_UPACC_INT_EN_I2_addr12_error_en_i2_END        (13)
#define SOC_UPACC_INT_EN_I2_addr13_error_en_i2_START      (14)
#define SOC_UPACC_INT_EN_I2_addr13_error_en_i2_END        (14)
#define SOC_UPACC_INT_EN_I2_addr14_error_en_i2_START      (15)
#define SOC_UPACC_INT_EN_I2_addr14_error_en_i2_END        (15)
#define SOC_UPACC_INT_EN_I2_param_addr_error_en_i2_START  (16)
#define SOC_UPACC_INT_EN_I2_param_addr_error_en_i2_END    (16)
#define SOC_UPACC_INT_EN_I2_harq_addr_error_en_i2_START   (17)
#define SOC_UPACC_INT_EN_I2_harq_addr_error_en_i2_END     (17)
#define SOC_UPACC_INT_EN_I2_dest_addr_eorr_en_i2_START    (18)
#define SOC_UPACC_INT_EN_I2_dest_addr_eorr_en_i2_END      (18)
#define SOC_UPACC_INT_EN_I2_loch_num_error_en_i2_START    (19)
#define SOC_UPACC_INT_EN_I2_loch_num_error_en_i2_END      (19)
#define SOC_UPACC_INT_EN_I2_bus_error_en_i2_START         (20)
#define SOC_UPACC_INT_EN_I2_bus_error_en_i2_END           (20)
#define SOC_UPACC_INT_EN_I2_dsp_int_en_i2_START           (21)
#define SOC_UPACC_INT_EN_I2_dsp_int_en_i2_END             (21)
#define SOC_UPACC_INT_EN_I2_arm_int_en_12_START           (22)
#define SOC_UPACC_INT_EN_I2_arm_int_en_12_END             (22)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_RAW_I2_UNION
 结构说明  : INT_RAW_I2 寄存器结构定义。地址偏移量:0x0234，初值:0x00000000，宽度:32
 寄存器说明: 原始中断状态寄存器。用于供CPU查询各中断信号在屏蔽前的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_raw_i2        : 1;  /* bit[0]    : MAC-i/is PDU封装完成原始中断位。封装操作完成时有效。
                                                                   高有效。 */
        unsigned int  trans_end_raw_i2        : 1;  /* bit[1]    : MAC-i/is PDU搬移装完成原始中断位。搬移操作完成时有效。
                                                                   高有效。 */
        unsigned int  addr1_error_raw_i2      : 1;  /* bit[2]    : MAC-i/is PDU逻辑信道2读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr2_error_raw_i2      : 1;  /* bit[3]    : MAC-i/is PDU逻辑信道1读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr3_error_raw_i2      : 1;  /* bit[4]    : MAC-i/is PDU逻辑信道3读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr4_error_raw_i2      : 1;  /* bit[5]    : MAC-i/is PDU逻辑信道4读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr5_error_raw_i2      : 1;  /* bit[6]    : MAC-i/is PDU逻辑信道5读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr6_error_raw_i2      : 1;  /* bit[7]    : MAC-i/is PDU逻辑信道6读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr7_error_raw_i2      : 1;  /* bit[8]    : MAC-i/is PDU逻辑信道7读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr8_error_raw_i2      : 1;  /* bit[9]    : MAC-i/is PDU逻辑信道8读地址配置错误原始中断位。读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr9_error_raw_i2      : 1;  /* bit[10]   : MAC-i/is PDU逻辑信道9读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr10_error_raw_i2     : 1;  /* bit[11]   : MAC-i/is PDU逻辑信道10读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr11_error_raw_i2     : 1;  /* bit[12]   : MAC-i/is PDU逻辑信道11读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr12_error_raw_i2     : 1;  /* bit[13]   : MAC-i/is PDU逻辑信道12读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr13_error_raw_i2     : 1;  /* bit[14]   : MAC-i/is PDU逻辑信道13读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  addr14_error_raw_i2     : 1;  /* bit[15]   : MAC-i/is PDU逻辑信道14读地址配置错误原始中断位。
                                                                   源数据非数组方式时，读地址不在起始地址与终止地址之间时有效。
                                                                   高有效。 */
        unsigned int  param_addr_error_raw_i2 : 1;  /* bit[16]   : MAC-i/is PDU参数链表地址配置错误原始中断位。参数链表地址最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  harq_addr_error_raw_i2  : 1;  /* bit[17]   : MAC-i/is PDU封装目标地址也即搬移起始地址配置错误原始中断位。Harq buufer起始地址的最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  dest_addr_eorr_raw_i2   : 1;  /* bit[18]   : MAC-i/is PDU搬移目标地址配置错误原始中断位。Bbp编码缓存起始地址最低三位bit[2:0]不为3'b000时有效。
                                                                   高有效。 */
        unsigned int  loch_num_error_raw_i2   : 1;  /* bit[19]   : 逻辑信道个数配置错误原始中断位。逻辑信道个数不在[0,14]范围之内时有效。
                                                                   高有效。 */
        unsigned int  bus_error_raw_i2        : 1;  /* bit[20]   : UPACC所访问的Slave侧返回ERROR响应原始中断位。Slave侧返回ERROR响应时有效。
                                                                   高有效。 */
        unsigned int  dsp_int_raw_i2          : 1;  /* bit[21]   : 载波2的各组合中断发生给DSP原始中断位。
                                                                   高有效。 */
        unsigned int  arm_int_raw_i2          : 1;  /* bit[22]   : 载波2的各组合中断发生给ARM原始中断位。
                                                                   高有效。 */
        unsigned int  reserved                : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_UPACC_INT_RAW_I2_UNION;
#endif
#define SOC_UPACC_INT_RAW_I2_encap_end_raw_i2_START         (0)
#define SOC_UPACC_INT_RAW_I2_encap_end_raw_i2_END           (0)
#define SOC_UPACC_INT_RAW_I2_trans_end_raw_i2_START         (1)
#define SOC_UPACC_INT_RAW_I2_trans_end_raw_i2_END           (1)
#define SOC_UPACC_INT_RAW_I2_addr1_error_raw_i2_START       (2)
#define SOC_UPACC_INT_RAW_I2_addr1_error_raw_i2_END         (2)
#define SOC_UPACC_INT_RAW_I2_addr2_error_raw_i2_START       (3)
#define SOC_UPACC_INT_RAW_I2_addr2_error_raw_i2_END         (3)
#define SOC_UPACC_INT_RAW_I2_addr3_error_raw_i2_START       (4)
#define SOC_UPACC_INT_RAW_I2_addr3_error_raw_i2_END         (4)
#define SOC_UPACC_INT_RAW_I2_addr4_error_raw_i2_START       (5)
#define SOC_UPACC_INT_RAW_I2_addr4_error_raw_i2_END         (5)
#define SOC_UPACC_INT_RAW_I2_addr5_error_raw_i2_START       (6)
#define SOC_UPACC_INT_RAW_I2_addr5_error_raw_i2_END         (6)
#define SOC_UPACC_INT_RAW_I2_addr6_error_raw_i2_START       (7)
#define SOC_UPACC_INT_RAW_I2_addr6_error_raw_i2_END         (7)
#define SOC_UPACC_INT_RAW_I2_addr7_error_raw_i2_START       (8)
#define SOC_UPACC_INT_RAW_I2_addr7_error_raw_i2_END         (8)
#define SOC_UPACC_INT_RAW_I2_addr8_error_raw_i2_START       (9)
#define SOC_UPACC_INT_RAW_I2_addr8_error_raw_i2_END         (9)
#define SOC_UPACC_INT_RAW_I2_addr9_error_raw_i2_START       (10)
#define SOC_UPACC_INT_RAW_I2_addr9_error_raw_i2_END         (10)
#define SOC_UPACC_INT_RAW_I2_addr10_error_raw_i2_START      (11)
#define SOC_UPACC_INT_RAW_I2_addr10_error_raw_i2_END        (11)
#define SOC_UPACC_INT_RAW_I2_addr11_error_raw_i2_START      (12)
#define SOC_UPACC_INT_RAW_I2_addr11_error_raw_i2_END        (12)
#define SOC_UPACC_INT_RAW_I2_addr12_error_raw_i2_START      (13)
#define SOC_UPACC_INT_RAW_I2_addr12_error_raw_i2_END        (13)
#define SOC_UPACC_INT_RAW_I2_addr13_error_raw_i2_START      (14)
#define SOC_UPACC_INT_RAW_I2_addr13_error_raw_i2_END        (14)
#define SOC_UPACC_INT_RAW_I2_addr14_error_raw_i2_START      (15)
#define SOC_UPACC_INT_RAW_I2_addr14_error_raw_i2_END        (15)
#define SOC_UPACC_INT_RAW_I2_param_addr_error_raw_i2_START  (16)
#define SOC_UPACC_INT_RAW_I2_param_addr_error_raw_i2_END    (16)
#define SOC_UPACC_INT_RAW_I2_harq_addr_error_raw_i2_START   (17)
#define SOC_UPACC_INT_RAW_I2_harq_addr_error_raw_i2_END     (17)
#define SOC_UPACC_INT_RAW_I2_dest_addr_eorr_raw_i2_START    (18)
#define SOC_UPACC_INT_RAW_I2_dest_addr_eorr_raw_i2_END      (18)
#define SOC_UPACC_INT_RAW_I2_loch_num_error_raw_i2_START    (19)
#define SOC_UPACC_INT_RAW_I2_loch_num_error_raw_i2_END      (19)
#define SOC_UPACC_INT_RAW_I2_bus_error_raw_i2_START         (20)
#define SOC_UPACC_INT_RAW_I2_bus_error_raw_i2_END           (20)
#define SOC_UPACC_INT_RAW_I2_dsp_int_raw_i2_START           (21)
#define SOC_UPACC_INT_RAW_I2_dsp_int_raw_i2_END             (21)
#define SOC_UPACC_INT_RAW_I2_arm_int_raw_i2_START           (22)
#define SOC_UPACC_INT_RAW_I2_arm_int_raw_i2_END             (22)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_MASK_I2_UNION
 结构说明  : INT_MASK_I2 寄存器结构定义。地址偏移量:0x0238，初值:0x00000000，宽度:32
 寄存器说明: 屏蔽后中断状态寄存器。用于供CPU查询各中断信号在屏蔽后的状态。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_msk_i2        : 1;  /* bit[0]    : MAC-i/is PDU封装完成屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  trans_end_msk_i2        : 1;  /* bit[1]    : MAC-i/is PDU搬移装完成屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr1_error_msk_i2      : 1;  /* bit[2]    : MAC-i/is PDU逻辑信道2读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr2_error_msk_i2      : 1;  /* bit[3]    : MAC-i/is PDU逻辑信道1读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr3_error_msk_i2      : 1;  /* bit[4]    : MAC-i/is PDU逻辑信道3读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr4_error_msk_i2      : 1;  /* bit[5]    : MAC-i/is PDU逻辑信道4读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr5_error_msk_i2      : 1;  /* bit[6]    : MAC-i/is PDU逻辑信道5读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr6_error_msk_i2      : 1;  /* bit[7]    : MAC-i/is PDU逻辑信道6读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr7_error_msk_i2      : 1;  /* bit[8]    : MAC-i/is PDU逻辑信道7读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr8_error_msk_i2      : 1;  /* bit[9]    : MAC-i/is PDU逻辑信道8读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr9_error_msk_i2      : 1;  /* bit[10]   : MAC-i/is PDU逻辑信道9读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr10_error_msk_i2     : 1;  /* bit[11]   : MAC-i/is PDU逻辑信道10读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr11_error_msk_i2     : 1;  /* bit[12]   : MAC-i/is PDU逻辑信道11读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr12_error_msk_i2     : 1;  /* bit[13]   : MAC-i/is PDU逻辑信道12读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr13_error_msk_i2     : 1;  /* bit[14]   : MAC-i/is PDU逻辑信道13读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  addr14_error_msk_i2     : 1;  /* bit[15]   : MAC-i/is PDU逻辑信道14读地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  param_addr_error_msk_i2 : 1;  /* bit[16]   : MAC-i/is PDU参数链表地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  harq_addr_error_msk_i2  : 1;  /* bit[17]   : MAC-i/is PDU封装目标地址也即搬移起始地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  dest_addr_eorr_msk_i2   : 1;  /* bit[18]   : MAC-i/is PDU搬移目标地址配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  loch_num_error_msk_i2   : 1;  /* bit[19]   : 逻辑信道个数配置错误屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  bus_error_msk_i2        : 1;  /* bit[20]   : UPACC所访问的Slave侧返回ERROR响应屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  dsp_int_msk_i2          : 1;  /* bit[21]   : 载波2的各组合中断发生给DSP屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  arm_int_msk_i2          : 1;  /* bit[22]   : 载波2的各组合中断发生给ARM屏蔽后中断位。
                                                                   高有效。 */
        unsigned int  reserved                : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_UPACC_INT_MASK_I2_UNION;
#endif
#define SOC_UPACC_INT_MASK_I2_encap_end_msk_i2_START         (0)
#define SOC_UPACC_INT_MASK_I2_encap_end_msk_i2_END           (0)
#define SOC_UPACC_INT_MASK_I2_trans_end_msk_i2_START         (1)
#define SOC_UPACC_INT_MASK_I2_trans_end_msk_i2_END           (1)
#define SOC_UPACC_INT_MASK_I2_addr1_error_msk_i2_START       (2)
#define SOC_UPACC_INT_MASK_I2_addr1_error_msk_i2_END         (2)
#define SOC_UPACC_INT_MASK_I2_addr2_error_msk_i2_START       (3)
#define SOC_UPACC_INT_MASK_I2_addr2_error_msk_i2_END         (3)
#define SOC_UPACC_INT_MASK_I2_addr3_error_msk_i2_START       (4)
#define SOC_UPACC_INT_MASK_I2_addr3_error_msk_i2_END         (4)
#define SOC_UPACC_INT_MASK_I2_addr4_error_msk_i2_START       (5)
#define SOC_UPACC_INT_MASK_I2_addr4_error_msk_i2_END         (5)
#define SOC_UPACC_INT_MASK_I2_addr5_error_msk_i2_START       (6)
#define SOC_UPACC_INT_MASK_I2_addr5_error_msk_i2_END         (6)
#define SOC_UPACC_INT_MASK_I2_addr6_error_msk_i2_START       (7)
#define SOC_UPACC_INT_MASK_I2_addr6_error_msk_i2_END         (7)
#define SOC_UPACC_INT_MASK_I2_addr7_error_msk_i2_START       (8)
#define SOC_UPACC_INT_MASK_I2_addr7_error_msk_i2_END         (8)
#define SOC_UPACC_INT_MASK_I2_addr8_error_msk_i2_START       (9)
#define SOC_UPACC_INT_MASK_I2_addr8_error_msk_i2_END         (9)
#define SOC_UPACC_INT_MASK_I2_addr9_error_msk_i2_START       (10)
#define SOC_UPACC_INT_MASK_I2_addr9_error_msk_i2_END         (10)
#define SOC_UPACC_INT_MASK_I2_addr10_error_msk_i2_START      (11)
#define SOC_UPACC_INT_MASK_I2_addr10_error_msk_i2_END        (11)
#define SOC_UPACC_INT_MASK_I2_addr11_error_msk_i2_START      (12)
#define SOC_UPACC_INT_MASK_I2_addr11_error_msk_i2_END        (12)
#define SOC_UPACC_INT_MASK_I2_addr12_error_msk_i2_START      (13)
#define SOC_UPACC_INT_MASK_I2_addr12_error_msk_i2_END        (13)
#define SOC_UPACC_INT_MASK_I2_addr13_error_msk_i2_START      (14)
#define SOC_UPACC_INT_MASK_I2_addr13_error_msk_i2_END        (14)
#define SOC_UPACC_INT_MASK_I2_addr14_error_msk_i2_START      (15)
#define SOC_UPACC_INT_MASK_I2_addr14_error_msk_i2_END        (15)
#define SOC_UPACC_INT_MASK_I2_param_addr_error_msk_i2_START  (16)
#define SOC_UPACC_INT_MASK_I2_param_addr_error_msk_i2_END    (16)
#define SOC_UPACC_INT_MASK_I2_harq_addr_error_msk_i2_START   (17)
#define SOC_UPACC_INT_MASK_I2_harq_addr_error_msk_i2_END     (17)
#define SOC_UPACC_INT_MASK_I2_dest_addr_eorr_msk_i2_START    (18)
#define SOC_UPACC_INT_MASK_I2_dest_addr_eorr_msk_i2_END      (18)
#define SOC_UPACC_INT_MASK_I2_loch_num_error_msk_i2_START    (19)
#define SOC_UPACC_INT_MASK_I2_loch_num_error_msk_i2_END      (19)
#define SOC_UPACC_INT_MASK_I2_bus_error_msk_i2_START         (20)
#define SOC_UPACC_INT_MASK_I2_bus_error_msk_i2_END           (20)
#define SOC_UPACC_INT_MASK_I2_dsp_int_msk_i2_START           (21)
#define SOC_UPACC_INT_MASK_I2_dsp_int_msk_i2_END             (21)
#define SOC_UPACC_INT_MASK_I2_arm_int_msk_i2_START           (22)
#define SOC_UPACC_INT_MASK_I2_arm_int_msk_i2_END             (22)


/*****************************************************************************
 结构名    : SOC_UPACC_INT_CLR_I2_UNION
 结构说明  : INT_CLR_I2 寄存器结构定义。地址偏移量:0x023C，初值:0x00000000，宽度:32
 寄存器说明: 中断状态清除寄存器。用于供CPU将各中断信号状态清零。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  encap_end_clr_i2        : 1;  /* bit[0]    : MAC-i/is PDU封装完成中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  trans_end_clr_i2        : 1;  /* bit[1]    : MAC-i/is PDU搬移装完成中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr1_error_clr_i2      : 1;  /* bit[2]    : MAC-i/is PDU逻辑信道2读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr2_error_clr_i2      : 1;  /* bit[3]    : MAC-i/is PDU逻辑信道1读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr3_error_clr_i2      : 1;  /* bit[4]    : MAC-i/is PDU逻辑信道3读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr4_error_clr_i2      : 1;  /* bit[5]    : MAC-i/is PDU逻辑信道4读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr5_error_clr_i2      : 1;  /* bit[6]    : MAC-i/is PDU逻辑信道5读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr6_error_clr_i2      : 1;  /* bit[7]    : MAC-i/is PDU逻辑信道6读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr7_error_clr_i2      : 1;  /* bit[8]    : MAC-i/is PDU逻辑信道7读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr8_error_clr_i2      : 1;  /* bit[9]    : MAC-i/is PDU逻辑信道8读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr9_error_clr_i2      : 1;  /* bit[10]   : MAC-i/is PDU逻辑信道9读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr10_error_clr_i2     : 1;  /* bit[11]   : MAC-i/is PDU逻辑信道10读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr11_error_clr_i2     : 1;  /* bit[12]   : MAC-i/is PDU逻辑信道11读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr12_error_clr_i2     : 1;  /* bit[13]   : MAC-i/is PDU逻辑信道12读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr13_error_clr_i2     : 1;  /* bit[14]   : MAC-i/is PDU逻辑信道13读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  addr14_error_clr_i2     : 1;  /* bit[15]   : MAC-i/is PDU逻辑信道14读地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  param_addr_error_clr_i2 : 1;  /* bit[16]   : 参数链表地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  harq_addr_error_clr_i2  : 1;  /* bit[17]   : MAC-i/is PDU封装目标地址也即搬移起始地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  dest_addr_eorr_clr_i2   : 1;  /* bit[18]   : MAC-i/is PDU搬移目标地址配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  loch_num_error_clr_i2   : 1;  /* bit[19]   : 逻辑信道个数配置错误中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  bus_error_clr_i2        : 1;  /* bit[20]   : UPACC所访问的Slave侧返回ERROR响应中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  dsp_int_clr_i2          : 1;  /* bit[21]   : 载波2的各组合中断发生给DSP中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  arm_int_clr_i2          : 1;  /* bit[22]   : 载波2的各组合中断发生给ARM中断清零位。
                                                                   CPU写1清零。 */
        unsigned int  reserved                : 9;  /* bit[23-31]: 保留 */
    } reg;
} SOC_UPACC_INT_CLR_I2_UNION;
#endif
#define SOC_UPACC_INT_CLR_I2_encap_end_clr_i2_START         (0)
#define SOC_UPACC_INT_CLR_I2_encap_end_clr_i2_END           (0)
#define SOC_UPACC_INT_CLR_I2_trans_end_clr_i2_START         (1)
#define SOC_UPACC_INT_CLR_I2_trans_end_clr_i2_END           (1)
#define SOC_UPACC_INT_CLR_I2_addr1_error_clr_i2_START       (2)
#define SOC_UPACC_INT_CLR_I2_addr1_error_clr_i2_END         (2)
#define SOC_UPACC_INT_CLR_I2_addr2_error_clr_i2_START       (3)
#define SOC_UPACC_INT_CLR_I2_addr2_error_clr_i2_END         (3)
#define SOC_UPACC_INT_CLR_I2_addr3_error_clr_i2_START       (4)
#define SOC_UPACC_INT_CLR_I2_addr3_error_clr_i2_END         (4)
#define SOC_UPACC_INT_CLR_I2_addr4_error_clr_i2_START       (5)
#define SOC_UPACC_INT_CLR_I2_addr4_error_clr_i2_END         (5)
#define SOC_UPACC_INT_CLR_I2_addr5_error_clr_i2_START       (6)
#define SOC_UPACC_INT_CLR_I2_addr5_error_clr_i2_END         (6)
#define SOC_UPACC_INT_CLR_I2_addr6_error_clr_i2_START       (7)
#define SOC_UPACC_INT_CLR_I2_addr6_error_clr_i2_END         (7)
#define SOC_UPACC_INT_CLR_I2_addr7_error_clr_i2_START       (8)
#define SOC_UPACC_INT_CLR_I2_addr7_error_clr_i2_END         (8)
#define SOC_UPACC_INT_CLR_I2_addr8_error_clr_i2_START       (9)
#define SOC_UPACC_INT_CLR_I2_addr8_error_clr_i2_END         (9)
#define SOC_UPACC_INT_CLR_I2_addr9_error_clr_i2_START       (10)
#define SOC_UPACC_INT_CLR_I2_addr9_error_clr_i2_END         (10)
#define SOC_UPACC_INT_CLR_I2_addr10_error_clr_i2_START      (11)
#define SOC_UPACC_INT_CLR_I2_addr10_error_clr_i2_END        (11)
#define SOC_UPACC_INT_CLR_I2_addr11_error_clr_i2_START      (12)
#define SOC_UPACC_INT_CLR_I2_addr11_error_clr_i2_END        (12)
#define SOC_UPACC_INT_CLR_I2_addr12_error_clr_i2_START      (13)
#define SOC_UPACC_INT_CLR_I2_addr12_error_clr_i2_END        (13)
#define SOC_UPACC_INT_CLR_I2_addr13_error_clr_i2_START      (14)
#define SOC_UPACC_INT_CLR_I2_addr13_error_clr_i2_END        (14)
#define SOC_UPACC_INT_CLR_I2_addr14_error_clr_i2_START      (15)
#define SOC_UPACC_INT_CLR_I2_addr14_error_clr_i2_END        (15)
#define SOC_UPACC_INT_CLR_I2_param_addr_error_clr_i2_START  (16)
#define SOC_UPACC_INT_CLR_I2_param_addr_error_clr_i2_END    (16)
#define SOC_UPACC_INT_CLR_I2_harq_addr_error_clr_i2_START   (17)
#define SOC_UPACC_INT_CLR_I2_harq_addr_error_clr_i2_END     (17)
#define SOC_UPACC_INT_CLR_I2_dest_addr_eorr_clr_i2_START    (18)
#define SOC_UPACC_INT_CLR_I2_dest_addr_eorr_clr_i2_END      (18)
#define SOC_UPACC_INT_CLR_I2_loch_num_error_clr_i2_START    (19)
#define SOC_UPACC_INT_CLR_I2_loch_num_error_clr_i2_END      (19)
#define SOC_UPACC_INT_CLR_I2_bus_error_clr_i2_START         (20)
#define SOC_UPACC_INT_CLR_I2_bus_error_clr_i2_END           (20)
#define SOC_UPACC_INT_CLR_I2_dsp_int_clr_i2_START           (21)
#define SOC_UPACC_INT_CLR_I2_dsp_int_clr_i2_END             (21)
#define SOC_UPACC_INT_CLR_I2_arm_int_clr_i2_START           (22)
#define SOC_UPACC_INT_CLR_I2_arm_int_clr_i2_END             (22)


/*****************************************************************************
 结构名    : SOC_UPACC_BUS_ERROR_ADDR_I2_UNION
 结构说明  : BUS_ERROR_ADDR_I2 寄存器结构定义。地址偏移量:0x0240，初值:0x00000000，宽度:32
 寄存器说明: 总线ERROR地址寄存器。用于寄存Slave侧返回ERROR响应时的当前操作地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bus_error_addr_i2 : 32; /* bit[0-31]: 用于寄存UPACC所访问的Slave侧放回ERROR响应时的当前操作地址。 */
    } reg;
} SOC_UPACC_BUS_ERROR_ADDR_I2_UNION;
#endif
#define SOC_UPACC_BUS_ERROR_ADDR_I2_bus_error_addr_i2_START  (0)
#define SOC_UPACC_BUS_ERROR_ADDR_I2_bus_error_addr_i2_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_EN_I2_UNION
 结构说明  : EN_I2 寄存器结构定义。地址偏移量:0x0244，初值:0x00000000，宽度:32
 寄存器说明: MAC-i/is PDU封装及搬移模块使能寄存器。用于配置MAC-i/is PDU封装及搬移模块使能与否。
            UPACC_EN_I2为MAC-i/is PDU封装及搬移模块启动寄存器。用于配置MAC-i/is PDU封装及搬移模块使能与否。
            DSP配置完其他封装参数后，通过设置该寄存器的bit0为1启动MAC-i/is PDU封装及搬移操作。封装及搬移操作完成后，硬件可自动对该寄存器清零。DSP在不需启用MAC-i/is PDU封装及搬移模块时，也应将该寄存器的bit0置为0，以降低功耗。
            当软件参数配置出错，即INT_RAW_I2中的addri_error_raw_I2、loch_loch_num_error_raw_I2、dest_addr_error_raw_I2等任意一个有效时，硬件逻辑会对该寄存器的bit0自动清零，同时对寄存器MAC_ENCAP_START_I2的bit[1:0]进行清零，以复位UPACC内部的MAC-i/is  PDU封装及搬运模块，使其内部状态机返回IDLE态。
            当软件发现UPACC封装及搬运超时或异常时，可对该寄存器的bit0写0，将UPACC内部MAC-i/is PDU封装及搬运模块的状态机复位，使其返回到IDLE态。此时，软件还需对寄存器MAC_ENCAP_START_I2的bit[1:0]进行清零，以防止异常启动。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  upacc_en_i2 : 1;  /* bit[0]   : 用于配置MAC-I PDU封装及搬移模块使能与否。
                                                      0：禁止。UPACC内部MAC-I PDU封装及搬运模块UPACC_ACC_I不工作，内部状态回到IDLE状态。
                                                      1：使能。UPACC内部MAC-I PDU封装及搬运模块UPACC_ACC_I处于工作状态。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_UPACC_EN_I2_UNION;
#endif
#define SOC_UPACC_EN_I2_upacc_en_i2_START  (0)
#define SOC_UPACC_EN_I2_upacc_en_i2_END    (0)


/*****************************************************************************
 结构名    : SOC_UPACC_HARQ_WRITE_ADDR_I2_UNION
 结构说明  : HARQ_WRITE_ADDR_I2 寄存器结构定义。地址偏移量:0x0248，初值:0x00000000，宽度:32
 寄存器说明: HARQ buffer写地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  harq_write_addr_i2 : 32; /* bit[0-31]: 用于调试时计算当次封装处理完成后，UPACC实际写入HARQ buffer的数据Byte个数。
                                                             若启动方式为封装且搬移或者仅搬移，则当封装处理结束，即INT_RAW_I2的bit[0]封装完成原始中断位encap_end_raw_i2有效之后，寄存器HARQ_WRITE_ADDR_I2的值与寄存器HARQ_BUF_ADDR_I2的值相减的结果，就是当次封装处理过程中，UPACC实际写入HARQ buffer中的数据Byte个数。
                                                             值得注意的是，由于BBP仅支持Word访问，因此UPACC内部逻辑设计中对不足整Word的数据，通过在高位补零凑成整Word后，写回外部Memory。所以，两个寄存器的值直接相减的结果，会比当次封装处理需要搬移到HARQ buffer中的数据量要略大一些，误差值最大为7Byte。具体误差值与si_en_i2、padding_len_i2等封装参数有关。不同配置下的误差值计算方法请参见《实际封装数据个数与所需封装数据个数在不同配置下的误差列表.xlsx》。 */
    } reg;
} SOC_UPACC_HARQ_WRITE_ADDR_I2_UNION;
#endif
#define SOC_UPACC_HARQ_WRITE_ADDR_I2_harq_write_addr_i2_START  (0)
#define SOC_UPACC_HARQ_WRITE_ADDR_I2_harq_write_addr_i2_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_BBP_WRITE_ADDR_I2_UNION
 结构说明  : BBP_WRITE_ADDR_I2 寄存器结构定义。地址偏移量:0x024C，初值:0x00000000，宽度:32
 寄存器说明: BBP 编码缓存写地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bbp_write_addr_i2 : 32; /* bit[0-31]: 用于调试时计算当次封装处理完成后，UPACC实际写入BBP buffer的数据Byte个数。
                                                            若启动方式为封装且搬移或者仅搬移，则当次封装处理结束，即INT_RAW_I2的bit[1]搬移完成原始中断位trans_end_raw_i2有效后，寄存器BBP_WRITE_ADDR_I2的值与寄存器BBP_DEST_ADDR_I2的值相减的结果，就是当次封装处理过程中，UPACC实际写入BBP编码缓存中的数据Byte个数。
                                                            值得注意的是，由于BBP仅支持Word访问，因此UPACC内部逻辑设计中对不足整Word的数据，通过在高位补零凑成整Word后，写回外部Memory。所以，两个寄存器的值直接相减的结果，会比当次封装处理需要搬移到BBP编码缓存中的数据量要略大一些，误差值最大为7Byte。具体误差值与si_en_i2、padding_len_i2等封装参数或者trans_num_i2有关。不同配置下的误差值计算方法请参见《实际封装数据个数与所需封装数据个数在不同配置下的误差列表.xlsx》。 */
    } reg;
} SOC_UPACC_BBP_WRITE_ADDR_I2_UNION;
#endif
#define SOC_UPACC_BBP_WRITE_ADDR_I2_bbp_write_addr_i2_START  (0)
#define SOC_UPACC_BBP_WRITE_ADDR_I2_bbp_write_addr_i2_END    (31)


/*****************************************************************************
 结构名    : SOC_UPACC_PDUARY_EN_I2_UNION
 结构说明  : PDUARY_EN_I2 寄存器结构定义。地址偏移量:0x0250，初值:0x00000000，宽度:32
 寄存器说明: Mac-i PDU载波2源数据数组方式配置使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pduary_en_i2 : 1;  /* bit[0]   : Mac-i PDU载波2的源数据数组方式配置使能信号。
                                                       0：禁止，源数据不采用数组方式配置，兼容UPACCV200；
                                                       1：使能，源数据采用数组方式配置。 */
        unsigned int  reserved     : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_UPACC_PDUARY_EN_I2_UNION;
#endif
#define SOC_UPACC_PDUARY_EN_I2_pduary_en_i2_START  (0)
#define SOC_UPACC_PDUARY_EN_I2_pduary_en_i2_END    (0)






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

#endif /* end of soc_upacc_interface.h */
