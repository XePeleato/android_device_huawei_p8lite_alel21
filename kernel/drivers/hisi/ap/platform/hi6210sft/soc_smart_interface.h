

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_SMART_INTERFACE_H__
#define __SOC_SMART_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/2) PMU_CTRL
 ****************************************************************************/
/* 寄存器说明：版本寄存器。
   位域定义UNION结构:  SOC_SMART_VERSION_UNION */
#define SOC_SMART_VERSION_ADDR(base)                  ((base) + (0x000))

/* 寄存器说明：状态标志1寄存器。
   位域定义UNION结构:  SOC_SMART_STATUS1_UNION */
#define SOC_SMART_STATUS1_ADDR(base)                  ((base) + (0x001))

/* 寄存器说明：状态标志2寄存器。
   位域定义UNION结构:  SOC_SMART_STATUS2_UNION */
#define SOC_SMART_STATUS2_ADDR(base)                  ((base) + (0x002))

/* 寄存器说明：中断1寄存器。
   位域定义UNION结构:  SOC_SMART_IRQ1_UNION */
#define SOC_SMART_IRQ1_ADDR(base)                     ((base) + (0x003))

/* 寄存器说明：中断2寄存器。
   位域定义UNION结构:  SOC_SMART_IRQ2_UNION */
#define SOC_SMART_IRQ2_ADDR(base)                     ((base) + (0x004))

/* 寄存器说明：中断3寄存器。
   位域定义UNION结构:  SOC_SMART_IRQ3_UNION */
#define SOC_SMART_IRQ3_ADDR(base)                     ((base) + (0x005))

/* 寄存器说明：库仑计中断寄存器。
   位域定义UNION结构:  SOC_SMART_COUL_IRQ_UNION */
#define SOC_SMART_COUL_IRQ_ADDR(base)                 ((base) + (0x006))

/* 寄存器说明：中断屏蔽1寄存器。
   位域定义UNION结构:  SOC_SMART_IRQ1_MASK_UNION */
#define SOC_SMART_IRQ1_MASK_ADDR(base)                ((base) + (0x007))

/* 寄存器说明：中断屏蔽2寄存器。
   位域定义UNION结构:  SOC_SMART_IRQ2_MASK_UNION */
#define SOC_SMART_IRQ2_MASK_ADDR(base)                ((base) + (0x008))

/* 寄存器说明：中断屏蔽3寄存器。
   位域定义UNION结构:  SOC_SMART_IRQ3_MASK_UNION */
#define SOC_SMART_IRQ3_MASK_ADDR(base)                ((base) + (0x009))

/* 寄存器说明：库仑计中断屏蔽寄存器。
   位域定义UNION结构:  SOC_SMART_COUL_IRQ_MASK_UNION */
#define SOC_SMART_COUL_IRQ_MASK_ADDR(base)            ((base) + (0x00A))

/* 寄存器说明：短路保护记录1寄存器。
   位域定义UNION结构:  SOC_SMART_SCP_BUCK_RECORD1_UNION */
#define SOC_SMART_SCP_BUCK_RECORD1_ADDR(base)         ((base) + (0x00B))

/* 寄存器说明：过流保护记录1寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_BUCK_RECORD1_UNION */
#define SOC_SMART_OCP_BUCK_RECORD1_ADDR(base)         ((base) + (0x00C))

/* 寄存器说明：过流保护记录2寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_LDO1_8_RECORD2_UNION */
#define SOC_SMART_OCP_LDO1_8_RECORD2_ADDR(base)       ((base) + (0x00D))

/* 寄存器说明：过流保护记录3寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_LDO16_9_RECORD3_UNION */
#define SOC_SMART_OCP_LDO16_9_RECORD3_ADDR(base)      ((base) + (0x00E))

/* 寄存器说明：过流保护记录4寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_LDO22_17_RECORD4_UNION */
#define SOC_SMART_OCP_LDO22_17_RECORD4_ADDR(base)     ((base) + (0x00F))

/* 寄存器说明：过流保护记录5寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_CLASSD_LVS2_RECORD5_UNION */
#define SOC_SMART_OCP_CLASSD_LVS2_RECORD5_ADDR(base)  ((base) + (0x010))

/* 寄存器说明：OCP去抖动调节0寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_DEB0_UNION */
#define SOC_SMART_OCP_DEB0_ADDR(base)                 ((base) + (0x01A))

/* 寄存器说明：OCP去抖动调节1寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_DEB1_UNION */
#define SOC_SMART_OCP_DEB1_ADDR(base)                 ((base) + (0x01B))

/* 寄存器说明：开关控制8寄存器。
   位域定义UNION结构:  SOC_SMART_ONOFF8_UNION */
#define SOC_SMART_ONOFF8_ADDR(base)                   ((base) + (0x01C))

/* 寄存器说明：BUCK1-4过流和短路保护模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_SCP_MOD_CTRL1_UNION */
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ADDR(base)        ((base) + (0x01D))

/* 寄存器说明：LDO1-4过流保护模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_MOD_CTRL3_UNION */
#define SOC_SMART_OCP_MOD_CTRL3_ADDR(base)            ((base) + (0x01E))

/* 寄存器说明：LDO5-8过流保护模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_MOD_CTRL4_UNION */
#define SOC_SMART_OCP_MOD_CTRL4_ADDR(base)            ((base) + (0x01F))

/* 寄存器说明：LDO9-12过流保护模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_MOD_CTRL5_UNION */
#define SOC_SMART_OCP_MOD_CTRL5_ADDR(base)            ((base) + (0x020))

/* 寄存器说明：LDO13-16过流保护模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_MOD_CTRL6_UNION */
#define SOC_SMART_OCP_MOD_CTRL6_ADDR(base)            ((base) + (0x021))

/* 寄存器说明：LDO17-20过流保护模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_MOD_CTRL7_UNION */
#define SOC_SMART_OCP_MOD_CTRL7_ADDR(base)            ((base) + (0x022))

/* 寄存器说明：LDO21-ldo23过流保护模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_MOD_CTRL8_UNION */
#define SOC_SMART_OCP_MOD_CTRL8_ADDR(base)            ((base) + (0x023))

/* 寄存器说明：LVS2过流保护模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_MOD_CTRL9_UNION */
#define SOC_SMART_OCP_MOD_CTRL9_ADDR(base)            ((base) + (0x024))

/* 寄存器说明：CLASSD过流保护模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_OCP_MOD_CTRL11_UNION */
#define SOC_SMART_OCP_MOD_CTRL11_ADDR(base)           ((base) + (0x025))

/* 寄存器说明：电源模块使能1寄存器。
   位域定义UNION结构:  SOC_SMART_ENABLE1_BUCK_UNION */
#define SOC_SMART_ENABLE1_BUCK_ADDR(base)             ((base) + (0x026))

/* 寄存器说明：电源模块关闭1寄存器。
   位域定义UNION结构:  SOC_SMART_DISABLE1_BUCK_UNION */
#define SOC_SMART_DISABLE1_BUCK_ADDR(base)            ((base) + (0x027))

/* 寄存器说明：电源模块开关状态1寄存器。
   位域定义UNION结构:  SOC_SMART_ONOFF_STATUS1_BUCK_UNION */
#define SOC_SMART_ONOFF_STATUS1_BUCK_ADDR(base)       ((base) + (0x028))

/* 寄存器说明：电源模块使能2寄存器。
   位域定义UNION结构:  SOC_SMART_ENABLE2_LDO1_8_UNION */
#define SOC_SMART_ENABLE2_LDO1_8_ADDR(base)           ((base) + (0x029))

/* 寄存器说明：电源模块关闭2寄存器。
   位域定义UNION结构:  SOC_SMART_DISABLE2_LDO1_8_UNION */
#define SOC_SMART_DISABLE2_LDO1_8_ADDR(base)          ((base) + (0x02A))

/* 寄存器说明：电源模块开关状态2寄存器。
   位域定义UNION结构:  SOC_SMART_ONOFF_STATUS2_LDO1_8_UNION */
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_ADDR(base)     ((base) + (0x02B))

/* 寄存器说明：电源模块使能3寄存器。
   位域定义UNION结构:  SOC_SMART_ENABLE3_LDO9_16_UNION */
#define SOC_SMART_ENABLE3_LDO9_16_ADDR(base)          ((base) + (0x02C))

/* 寄存器说明：电源模块关闭3寄存器。
   位域定义UNION结构:  SOC_SMART_DISABLE3_LDO9_16_UNION */
#define SOC_SMART_DISABLE3_LDO9_16_ADDR(base)         ((base) + (0x02D))

/* 寄存器说明：电源模块开关状态3寄存器。
   位域定义UNION结构:  SOC_SMART_ONOFF_STATUS3_LDO9_16_UNION */
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_ADDR(base)    ((base) + (0x02E))

/* 寄存器说明：电源模块使能4寄存器。
   位域定义UNION结构:  SOC_SMART_ENABLE4_LDO17_22_UNION */
#define SOC_SMART_ENABLE4_LDO17_22_ADDR(base)         ((base) + (0x02F))

/* 寄存器说明：电源模块关闭4寄存器。
   位域定义UNION结构:  SOC_SMART_DISABLE4_LDO17_22_UNION */
#define SOC_SMART_DISABLE4_LDO17_22_ADDR(base)        ((base) + (0x030))

/* 寄存器说明：电源模块开关状态4寄存器。
   位域定义UNION结构:  SOC_SMART_ONOFF_STATUS4_LDO17_22_UNION */
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_ADDR(base)   ((base) + (0x031))

/* 寄存器说明：电源模块使能5寄存器。
   位域定义UNION结构:  SOC_SMART_ENABLE5_LVS2_UNION */
#define SOC_SMART_ENABLE5_LVS2_ADDR(base)             ((base) + (0x032))

/* 寄存器说明：电源模块关闭5寄存器。
   位域定义UNION结构:  SOC_SMART_DISABLE5_LVS2_UNION */
#define SOC_SMART_DISABLE5_LVS2_ADDR(base)            ((base) + (0x033))

/* 寄存器说明：电源模块开关状态5寄存器。
   位域定义UNION结构:  SOC_SMART_ONOFF_STATUS5_LVS2_UNION */
#define SOC_SMART_ONOFF_STATUS5_LVS2_ADDR(base)       ((base) + (0x034))

/* 寄存器说明：电源模块使能6寄存器。
   位域定义UNION结构:  SOC_SMART_ENABLE6_XO_CLK_UNION */
#define SOC_SMART_ENABLE6_XO_CLK_ADDR(base)           ((base) + (0x035))

/* 寄存器说明：电源模块关闭6寄存器。
   位域定义UNION结构:  SOC_SMART_DISABLE6_XO_CLK_UNION */
#define SOC_SMART_DISABLE6_XO_CLK_ADDR(base)          ((base) + (0x036))

/* 寄存器说明：电源模块开关状态6寄存器。
   位域定义UNION结构:  SOC_SMART_ONOFF_STATUS6_XO_CLK_UNION */
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_ADDR(base)     ((base) + (0x037))

/* 寄存器说明：电源模块是否跟随PMU进出ECO模式配置1寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_MOD_CFG1_UNION */
#define SOC_SMART_ECO_MOD_CFG1_ADDR(base)             ((base) + (0x038))

/* 寄存器说明：电源模块是否跟随PMU进出ECO模式配置2寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_MOD_CFG2_UNION */
#define SOC_SMART_ECO_MOD_CFG2_ADDR(base)             ((base) + (0x039))

/* 寄存器说明：电源模块是否跟随PMU进出ECO模式配置3寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_MOD_CFG3_UNION */
#define SOC_SMART_ECO_MOD_CFG3_ADDR(base)             ((base) + (0x03A))

/* 寄存器说明：电源模块是否跟随PMU进出ECO模式配置4寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_MOD_CFG4_UNION */
#define SOC_SMART_ECO_MOD_CFG4_ADDR(base)             ((base) + (0x03B))

/* 寄存器说明：电源模块是否强制进入ECO模式配置5寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_MOD_CFG5_UNION */
#define SOC_SMART_ECO_MOD_CFG5_ADDR(base)             ((base) + (0x03C))

/* 寄存器说明：电源模块是否强制进入ECO模式配置6寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_MOD_CFG6_UNION */
#define SOC_SMART_ECO_MOD_CFG6_ADDR(base)             ((base) + (0x03D))

/* 寄存器说明：电源模块是否强制进入ECO模式配置7寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_MOD_CFG7_UNION */
#define SOC_SMART_ECO_MOD_CFG7_ADDR(base)             ((base) + (0x03E))

/* 寄存器说明：电源模块是否强制进入ECO模式配置8寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_MOD_CFG8_UNION */
#define SOC_SMART_ECO_MOD_CFG8_ADDR(base)             ((base) + (0x03F))

/* 寄存器说明：与PERI_EN硬线和ECO功能相关的屏蔽信号寄存器。
   位域定义UNION结构:  SOC_SMART_PERI_EN_MARK_UNION */
#define SOC_SMART_PERI_EN_MARK_ADDR(base)             ((base) + (0x040))

/* 寄存器说明：BUCK2控制0寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK2_REG0_UNION */
#define SOC_SMART_BUCK2_REG0_ADDR(base)               ((base) + (0x049))

/* 寄存器说明：BUCK2控制1寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK2_REG1_UNION */
#define SOC_SMART_BUCK2_REG1_ADDR(base)               ((base) + (0x04A))

/* 寄存器说明：BUCK2控制2寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK2_REG2_UNION */
#define SOC_SMART_BUCK2_REG2_ADDR(base)               ((base) + (0x04B))

/* 寄存器说明：BUCK2控制3寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK2_REG3_UNION */
#define SOC_SMART_BUCK2_REG3_ADDR(base)               ((base) + (0x04C))

/* 寄存器说明：BUCK2控制4寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK2_REG4_UNION */
#define SOC_SMART_BUCK2_REG4_ADDR(base)               ((base) + (0x04D))

/* 寄存器说明：BUCK2控制5寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK2_REG5_UNION */
#define SOC_SMART_BUCK2_REG5_ADDR(base)               ((base) + (0x04E))

/* 寄存器说明：BUCK2控制6寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK2_REG6_UNION */
#define SOC_SMART_BUCK2_REG6_ADDR(base)               ((base) + (0x04F))

/* 寄存器说明：BUCK2控制7寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK2_REG7_UNION */
#define SOC_SMART_BUCK2_REG7_ADDR(base)               ((base) + (0x050))

/* 寄存器说明：BUCK3控制0寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK3_REG0_UNION */
#define SOC_SMART_BUCK3_REG0_ADDR(base)               ((base) + (0x051))

/* 寄存器说明：BUCK3控制1寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK3_REG1_UNION */
#define SOC_SMART_BUCK3_REG1_ADDR(base)               ((base) + (0x052))

/* 寄存器说明：BUCK3控制2寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK3_REG2_UNION */
#define SOC_SMART_BUCK3_REG2_ADDR(base)               ((base) + (0x053))

/* 寄存器说明：BUCK3控制3寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK3_REG3_UNION */
#define SOC_SMART_BUCK3_REG3_ADDR(base)               ((base) + (0x054))

/* 寄存器说明：BUCK3控制4寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK3_REG4_UNION */
#define SOC_SMART_BUCK3_REG4_ADDR(base)               ((base) + (0x055))

/* 寄存器说明：BUCK3控制5寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK3_REG5_UNION */
#define SOC_SMART_BUCK3_REG5_ADDR(base)               ((base) + (0x056))

/* 寄存器说明：BUCK3控制6寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK3_REG6_UNION */
#define SOC_SMART_BUCK3_REG6_ADDR(base)               ((base) + (0x057))

/* 寄存器说明：BUCK3控制7寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK3_REG7_UNION */
#define SOC_SMART_BUCK3_REG7_ADDR(base)               ((base) + (0x058))

/* 寄存器说明：BUCK4控制0寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK4_REG0_UNION */
#define SOC_SMART_BUCK4_REG0_ADDR(base)               ((base) + (0x059))

/* 寄存器说明：BUCK4控制1寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK4_REG1_UNION */
#define SOC_SMART_BUCK4_REG1_ADDR(base)               ((base) + (0x05A))

/* 寄存器说明：BUCK4控制2寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK4_REG2_UNION */
#define SOC_SMART_BUCK4_REG2_ADDR(base)               ((base) + (0x05B))

/* 寄存器说明：BUCK4控制3寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK4_REG3_UNION */
#define SOC_SMART_BUCK4_REG3_ADDR(base)               ((base) + (0x05C))

/* 寄存器说明：BUCK4控制4寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK4_REG4_UNION */
#define SOC_SMART_BUCK4_REG4_ADDR(base)               ((base) + (0x05D))

/* 寄存器说明：BUCK4控制5寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK4_REG5_UNION */
#define SOC_SMART_BUCK4_REG5_ADDR(base)               ((base) + (0x05E))

/* 寄存器说明：BUCK4控制6寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK4_REG6_UNION */
#define SOC_SMART_BUCK4_REG6_ADDR(base)               ((base) + (0x05F))

/* 寄存器说明：BUCK4控制7寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK4_REG7_UNION */
#define SOC_SMART_BUCK4_REG7_ADDR(base)               ((base) + (0x060))

/* 寄存器说明：BUCK预留0寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK_RESERVE0_UNION */
#define SOC_SMART_BUCK_RESERVE0_ADDR(base)            ((base) + (0x061))

/* 寄存器说明：BUCK预留1寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK_RESERVE1_UNION */
#define SOC_SMART_BUCK_RESERVE1_ADDR(base)            ((base) + (0x062))

/* 寄存器说明：CLK_TOP控制0寄存器。
   位域定义UNION结构:  SOC_SMART_CLK_TOP0_UNION */
#define SOC_SMART_CLK_TOP0_ADDR(base)                 ((base) + (0x063))

/* 寄存器说明：CLK_TOP控制1寄存器。
   位域定义UNION结构:  SOC_SMART_CLK_TOP1_UNION */
#define SOC_SMART_CLK_TOP1_ADDR(base)                 ((base) + (0x064))

/* 寄存器说明：CLK_TOP控制3寄存器。
   位域定义UNION结构:  SOC_SMART_CLK_TOP3_UNION */
#define SOC_SMART_CLK_TOP3_ADDR(base)                 ((base) + (0x066))

/* 寄存器说明：CLK_TOP控制4寄存器。
   位域定义UNION结构:  SOC_SMART_CLK_TOP4_UNION */
#define SOC_SMART_CLK_TOP4_ADDR(base)                 ((base) + (0x067))

/* 寄存器说明：CLK_TOP控制5寄存器。
   位域定义UNION结构:  SOC_SMART_CLK_TOP5_UNION */
#define SOC_SMART_CLK_TOP5_ADDR(base)                 ((base) + (0x068))

/* 寄存器说明：CLASS_D控制1寄存器。
   位域定义UNION结构:  SOC_SMART_CLASSD_CTRL1_UNION */
#define SOC_SMART_CLASSD_CTRL1_ADDR(base)             ((base) + (0x069))

/* 寄存器说明：CLASS_D控制2寄存器。
   位域定义UNION结构:  SOC_SMART_CLASSD_CTRL2_UNION */
#define SOC_SMART_CLASSD_CTRL2_ADDR(base)             ((base) + (0x06A))

/* 寄存器说明：CLASS_D控制3寄存器。
   位域定义UNION结构:  SOC_SMART_CLASSD_CTRL3_UNION */
#define SOC_SMART_CLASSD_CTRL3_ADDR(base)             ((base) + (0x06B))

/* 寄存器说明：BUCK2电压调节寄存器。
   位域定义UNION结构:  SOC_SMART_VSET_BUCK2_ADJ_UNION */
#define SOC_SMART_VSET_BUCK2_ADJ_ADDR(base)           ((base) + (0x06D))

/* 寄存器说明：BUCK3电压调节寄存器。
   位域定义UNION结构:  SOC_SMART_VSET_BUCK3_ADJ_UNION */
#define SOC_SMART_VSET_BUCK3_ADJ_ADDR(base)           ((base) + (0x06E))

/* 寄存器说明：BUCK4电压调节0寄存器。
   位域定义UNION结构:  SOC_SMART_VSET_BUCK4_ADJ0_UNION */
#define SOC_SMART_VSET_BUCK4_ADJ0_ADDR(base)          ((base) + (0x06F))

/* 寄存器说明：BUCK4电压调节1寄存器。
   位域定义UNION结构:  SOC_SMART_VSET_BUCK4_ADJ1_UNION */
#define SOC_SMART_VSET_BUCK4_ADJ1_ADDR(base)          ((base) + (0x070))

/* 寄存器说明：LDO1调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO1_REG_ADJ_UNION */
#define SOC_SMART_LDO1_REG_ADJ_ADDR(base)             ((base) + (0x071))

/* 寄存器说明：LDO2调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO2_REG_ADJ_UNION */
#define SOC_SMART_LDO2_REG_ADJ_ADDR(base)             ((base) + (0x072))

/* 寄存器说明：LDO3调节0寄存器。
   位域定义UNION结构:  SOC_SMART_LDO3_REG_ADJ0_UNION */
#define SOC_SMART_LDO3_REG_ADJ0_ADDR(base)            ((base) + (0x073))

/* 寄存器说明：LDO3调节1寄存器。
   位域定义UNION结构:  SOC_SMART_LDO3_REG_ADJ1_UNION */
#define SOC_SMART_LDO3_REG_ADJ1_ADDR(base)            ((base) + (0x074))

/* 寄存器说明：LDO4调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO4_REG_ADJ_UNION */
#define SOC_SMART_LDO4_REG_ADJ_ADDR(base)             ((base) + (0x075))

/* 寄存器说明：LDO5调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO5_REG_ADJ_UNION */
#define SOC_SMART_LDO5_REG_ADJ_ADDR(base)             ((base) + (0x076))

/* 寄存器说明：LDO6调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO6_REG_ADJ_UNION */
#define SOC_SMART_LDO6_REG_ADJ_ADDR(base)             ((base) + (0x077))

/* 寄存器说明：LDO7调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO7_REG_ADJ_UNION */
#define SOC_SMART_LDO7_REG_ADJ_ADDR(base)             ((base) + (0x078))

/* 寄存器说明：LDO8调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO8_REG_ADJ_UNION */
#define SOC_SMART_LDO8_REG_ADJ_ADDR(base)             ((base) + (0x079))

/* 寄存器说明：LDO9调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO9_REG_ADJ_UNION */
#define SOC_SMART_LDO9_REG_ADJ_ADDR(base)             ((base) + (0x07A))

/* 寄存器说明：LDO10调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO10_REG_ADJ_UNION */
#define SOC_SMART_LDO10_REG_ADJ_ADDR(base)            ((base) + (0x07B))

/* 寄存器说明：LDO11调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO11_REG_ADJ_UNION */
#define SOC_SMART_LDO11_REG_ADJ_ADDR(base)            ((base) + (0x07C))

/* 寄存器说明：LDO12调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO12_REG_ADJ_UNION */
#define SOC_SMART_LDO12_REG_ADJ_ADDR(base)            ((base) + (0x07D))

/* 寄存器说明：LDO13调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO13_REG_ADJ_UNION */
#define SOC_SMART_LDO13_REG_ADJ_ADDR(base)            ((base) + (0x07E))

/* 寄存器说明：LDO14调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO14_REG_ADJ_UNION */
#define SOC_SMART_LDO14_REG_ADJ_ADDR(base)            ((base) + (0x07F))

/* 寄存器说明：LDO15调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO15_REG_ADJ_UNION */
#define SOC_SMART_LDO15_REG_ADJ_ADDR(base)            ((base) + (0x080))

/* 寄存器说明：LDO16调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO16_REG_ADJ_UNION */
#define SOC_SMART_LDO16_REG_ADJ_ADDR(base)            ((base) + (0x081))

/* 寄存器说明：LDO17调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO17_REG_ADJ_UNION */
#define SOC_SMART_LDO17_REG_ADJ_ADDR(base)            ((base) + (0x082))

/* 寄存器说明：LDO18调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO18_REG_ADJ_UNION */
#define SOC_SMART_LDO18_REG_ADJ_ADDR(base)            ((base) + (0x083))

/* 寄存器说明：LDO19调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO19_REG_ADJ_UNION */
#define SOC_SMART_LDO19_REG_ADJ_ADDR(base)            ((base) + (0x084))

/* 寄存器说明：LDO20调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO20_REG_ADJ_UNION */
#define SOC_SMART_LDO20_REG_ADJ_ADDR(base)            ((base) + (0x085))

/* 寄存器说明：LDO21调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO21_REG_ADJ_UNION */
#define SOC_SMART_LDO21_REG_ADJ_ADDR(base)            ((base) + (0x086))

/* 寄存器说明：LDO22调节寄存器。
   位域定义UNION结构:  SOC_SMART_LDO22_REG_ADJ_UNION */
#define SOC_SMART_LDO22_REG_ADJ_ADDR(base)            ((base) + (0x087))

/* 寄存器说明：PMUA调节寄存器。
   位域定义UNION结构:  SOC_SMART_PMUA_REG_ADJ_UNION */
#define SOC_SMART_PMUA_REG_ADJ_ADDR(base)             ((base) + (0x088))

/* 寄存器说明：LVS调节1寄存器。
   位域定义UNION结构:  SOC_SMART_LVS_ADJ1_UNION */
#define SOC_SMART_LVS_ADJ1_ADDR(base)                 ((base) + (0x089))

/* 寄存器说明：BANDGAP和THSD调节1寄存器。
   位域定义UNION结构:  SOC_SMART_BANDGAP_THSD_ADJ1_UNION */
#define SOC_SMART_BANDGAP_THSD_ADJ1_ADDR(base)        ((base) + (0x08A))

/* 寄存器说明：BANDGAP和THSD调节2寄存器。
   位域定义UNION结构:  SOC_SMART_BANDGAP_THSD_ADJ2_UNION */
#define SOC_SMART_BANDGAP_THSD_ADJ2_ADDR(base)        ((base) + (0x08B))

/* 寄存器说明：DR闪烁模式控制1寄存器。
   位域定义UNION结构:  SOC_SMART_DR_FLA_CTRL1_UNION */
#define SOC_SMART_DR_FLA_CTRL1_ADDR(base)             ((base) + (0x08C))

/* 寄存器说明：DR闪烁模式控制2寄存器。
   位域定义UNION结构:  SOC_SMART_DR_FLA_CTRL2_UNION */
#define SOC_SMART_DR_FLA_CTRL2_ADDR(base)             ((base) + (0x08D))

/* 寄存器说明：DR闪烁模式周期调节0寄存器。
   位域定义UNION结构:  SOC_SMART_FLASH_PERIOD_DR12_UNION */
#define SOC_SMART_FLASH_PERIOD_DR12_ADDR(base)        ((base) + (0x08E))

/* 寄存器说明：DR闪烁模式点亮时间调节0寄存器。
   位域定义UNION结构:  SOC_SMART_FLASH_ON_DR12_UNION */
#define SOC_SMART_FLASH_ON_DR12_ADDR(base)            ((base) + (0x08F))

/* 寄存器说明：DR闪烁模式周期调节1寄存器。
   位域定义UNION结构:  SOC_SMART_FLASH_PERIOD_DR345_UNION */
#define SOC_SMART_FLASH_PERIOD_DR345_ADDR(base)       ((base) + (0x090))

/* 寄存器说明：DR闪烁模式点亮时间调节1寄存器。
   位域定义UNION结构:  SOC_SMART_FLASH_ON_DR345_UNION */
#define SOC_SMART_FLASH_ON_DR345_ADDR(base)           ((base) + (0x091))

/* 寄存器说明：DR灯模式选择寄存器。
   位域定义UNION结构:  SOC_SMART_DR_MODE_SEL_UNION */
#define SOC_SMART_DR_MODE_SEL_ADDR(base)              ((base) + (0x092))

/* 寄存器说明：DR1/2呼吸模式控制寄存器。
   位域定义UNION结构:  SOC_SMART_DR_BRE_CTRL_UNION */
#define SOC_SMART_DR_BRE_CTRL_ADDR(base)              ((base) + (0x093))

/* 寄存器说明：DR1和DR2常亮常暗时间配置寄存器。
   位域定义UNION结构:  SOC_SMART_DR12_TIM_CONF0_UNION */
#define SOC_SMART_DR12_TIM_CONF0_ADDR(base)           ((base) + (0x094))

/* 寄存器说明：DR1和DR2渐亮渐暗时间配置寄存器。
   位域定义UNION结构:  SOC_SMART_DR12_TIM_CONF1_UNION */
#define SOC_SMART_DR12_TIM_CONF1_ADDR(base)           ((base) + (0x095))

/* 寄存器说明：DR1电流选择寄存器。
   位域定义UNION结构:  SOC_SMART_DR1_ISET_UNION */
#define SOC_SMART_DR1_ISET_ADDR(base)                 ((base) + (0x096))

/* 寄存器说明：DR2电流选择寄存器。
   位域定义UNION结构:  SOC_SMART_DR2_ISET_UNION */
#define SOC_SMART_DR2_ISET_ADDR(base)                 ((base) + (0x097))

/* 寄存器说明：DR3/4/5控制寄存器。
   位域定义UNION结构:  SOC_SMART_DR_LED_CTRL_UNION */
#define SOC_SMART_DR_LED_CTRL_ADDR(base)              ((base) + (0x098))

/* 寄存器说明：DR3/4/5输出控制寄存器。
   位域定义UNION结构:  SOC_SMART_DR_OUT_CTRL_UNION */
#define SOC_SMART_DR_OUT_CTRL_ADDR(base)              ((base) + (0x099))

/* 寄存器说明：DR3电流选择寄存器。
   位域定义UNION结构:  SOC_SMART_DR3_ISET_UNION */
#define SOC_SMART_DR3_ISET_ADDR(base)                 ((base) + (0x09A))

/* 寄存器说明：DR3启动延时配置寄存器。
   位域定义UNION结构:  SOC_SMART_DR3_START_DEL_UNION */
#define SOC_SMART_DR3_START_DEL_ADDR(base)            ((base) + (0x09B))

/* 寄存器说明：DR4电流选择寄存器。
   位域定义UNION结构:  SOC_SMART_DR4_ISET_UNION */
#define SOC_SMART_DR4_ISET_ADDR(base)                 ((base) + (0x09C))

/* 寄存器说明：DR4启动延时配置寄存器。
   位域定义UNION结构:  SOC_SMART_DR4_START_DEL_UNION */
#define SOC_SMART_DR4_START_DEL_ADDR(base)            ((base) + (0x09D))

/* 寄存器说明：DR5电流选择寄存器。
   位域定义UNION结构:  SOC_SMART_DR5_ISET_UNION */
#define SOC_SMART_DR5_ISET_ADDR(base)                 ((base) + (0x09E))

/* 寄存器说明：DR5启动延时配置寄存器。
   位域定义UNION结构:  SOC_SMART_DR5_START_DEL_UNION */
#define SOC_SMART_DR5_START_DEL_ADDR(base)            ((base) + (0x09F))


/* 寄存器说明：DR3常亮常暗时间配置寄存器。
   位域定义UNION结构:  SOC_SMART_DR3_TIM_CONF0_UNION */
#define SOC_SMART_DR3_TIM_CONF0_ADDR(base)            ((base) + (0x0A0))

/* 寄存器说明：DR3渐亮渐暗时间配置寄存器。
   位域定义UNION结构:  SOC_SMART_DR3_TIM_CONF1_UNION */
#define SOC_SMART_DR3_TIM_CONF1_ADDR(base)            ((base) + (0x0A1))

/* 寄存器说明：OTP控制1信号寄存器。
   位域定义UNION结构:  SOC_SMART_OTP_CTRL1_UNION */
#define SOC_SMART_OTP_CTRL1_ADDR(base)                ((base) + (0x0A2))

/* 寄存器说明：OTP控制2信号寄存器。
   位域定义UNION结构:  SOC_SMART_OTP_CTRL2_UNION */
#define SOC_SMART_OTP_CTRL2_ADDR(base)                ((base) + (0x0A3))

/* 寄存器说明：OTP写入值寄存器。
   位域定义UNION结构:  SOC_SMART_OTP_PDIN_UNION */
#define SOC_SMART_OTP_PDIN_ADDR(base)                 ((base) + (0x0A4))

/* 寄存器说明：模拟RO预留寄存器。
   位域定义UNION结构:  SOC_SMART_ANA_IN_UNION */
#define SOC_SMART_ANA_IN_ADDR(base)                   ((base) + (0x0B6))

/* 寄存器说明：保留0寄存器。
   位域定义UNION结构:  SOC_SMART_RESERVED0_UNION */
#define SOC_SMART_RESERVED0_ADDR(base)                ((base) + (0x0B7))

/* 寄存器说明：保留1寄存器。
   位域定义UNION结构:  SOC_SMART_RESERVED1_UNION */
#define SOC_SMART_RESERVED1_ADDR(base)                ((base) + (0x0B8))

/* 寄存器说明：保留2寄存器。
   位域定义UNION结构:  SOC_SMART_RESERVED2_UNION */
#define SOC_SMART_RESERVED2_ADDR(base)                ((base) + (0x0B9))

/* 寄存器说明：DAC控制寄存器。
   位域定义UNION结构:  SOC_SMART_DAC_CTRL_UNION */
#define SOC_SMART_DAC_CTRL_ADDR(base)                 ((base) + (0x0BA))

/* 寄存器说明：BUCK01控制0信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK01_CTRL0_UNION */
#define SOC_SMART_BUCK01_CTRL0_ADDR(base)             ((base) + (0x0D7))

/* 寄存器说明：BUCK01控制1信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK01_CTRL1_UNION */
#define SOC_SMART_BUCK01_CTRL1_ADDR(base)             ((base) + (0x0D8))

/* 寄存器说明：BUCK01控制2信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK01_CTRL2_UNION */
#define SOC_SMART_BUCK01_CTRL2_ADDR(base)             ((base) + (0xD9))

/* 寄存器说明：BUCK01控制3信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK01_CTRL3_UNION */
#define SOC_SMART_BUCK01_CTRL3_ADDR(base)             ((base) + (0xDA))

/* 寄存器说明：BUCK01控制4信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK01_CTRL4_UNION */
#define SOC_SMART_BUCK01_CTRL4_ADDR(base)             ((base) + (0x0DB))

/* 寄存器说明：BUCK0控制0信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL0_UNION */
#define SOC_SMART_BUCK0_CTRL0_ADDR(base)              ((base) + (0x0DC))

/* 寄存器说明：BUCK0控制1信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL1_UNION */
#define SOC_SMART_BUCK0_CTRL1_ADDR(base)              ((base) + (0x0DD))

/* 寄存器说明：BUCK0控制2信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL2_UNION */
#define SOC_SMART_BUCK0_CTRL2_ADDR(base)              ((base) + (0x0DE))

/* 寄存器说明：BUCK0控制3信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL3_UNION */
#define SOC_SMART_BUCK0_CTRL3_ADDR(base)              ((base) + (0x0DF))

/* 寄存器说明：BUCK0控制4信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL4_UNION */
#define SOC_SMART_BUCK0_CTRL4_ADDR(base)              ((base) + (0x0E0))

/* 寄存器说明：BUCK0控制5信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL5_UNION */
#define SOC_SMART_BUCK0_CTRL5_ADDR(base)              ((base) + (0x0E1))

/* 寄存器说明：BUCK0控制6信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL6_UNION */
#define SOC_SMART_BUCK0_CTRL6_ADDR(base)              ((base) + (0x0E2))

/* 寄存器说明：BUCK0控制7信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL7_UNION */
#define SOC_SMART_BUCK0_CTRL7_ADDR(base)              ((base) + (0x0E3))

/* 寄存器说明：BUCK0控制8信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL8_UNION */
#define SOC_SMART_BUCK0_CTRL8_ADDR(base)              ((base) + (0x0E4))

/* 寄存器说明：BUCK0控制9信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL9_UNION */
#define SOC_SMART_BUCK0_CTRL9_ADDR(base)              ((base) + (0x0E5))

/* 寄存器说明：BUCK0控制10信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK0_CTRL10_UNION */
#define SOC_SMART_BUCK0_CTRL10_ADDR(base)             ((base) + (0x0E6))

/* 寄存器说明：BUCK1控制0信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL0_UNION */
#define SOC_SMART_BUCK1_CTRL0_ADDR(base)              ((base) + (0x0E7))

/* 寄存器说明：BUCK1控制1信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL1_UNION */
#define SOC_SMART_BUCK1_CTRL1_ADDR(base)              ((base) + (0x0E8))

/* 寄存器说明：BUCK1控制2信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL2_UNION */
#define SOC_SMART_BUCK1_CTRL2_ADDR(base)              ((base) + (0x0E9))

/* 寄存器说明：BUCK1控制3信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL3_UNION */
#define SOC_SMART_BUCK1_CTRL3_ADDR(base)              ((base) + (0x0EA))

/* 寄存器说明：BUCK1控制4信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL4_UNION */
#define SOC_SMART_BUCK1_CTRL4_ADDR(base)              ((base) + (0x0EB))

/* 寄存器说明：BUCK1控制5信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL5_UNION */
#define SOC_SMART_BUCK1_CTRL5_ADDR(base)              ((base) + (0x0EC))

/* 寄存器说明：BUCK1控制6信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL6_UNION */
#define SOC_SMART_BUCK1_CTRL6_ADDR(base)              ((base) + (0x0ED))

/* 寄存器说明：BUCK1控制7信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL7_UNION */
#define SOC_SMART_BUCK1_CTRL7_ADDR(base)              ((base) + (0x0EF))

/* 寄存器说明：BUCK1控制8信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL8_UNION */
#define SOC_SMART_BUCK1_CTRL8_ADDR(base)              ((base) + (0x0F0))

/* 寄存器说明：BUCK1控制9信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL9_UNION */
#define SOC_SMART_BUCK1_CTRL9_ADDR(base)              ((base) + (0x0F1))

/* 寄存器说明：BUCK1控制10信号寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK1_CTRL10_UNION */
#define SOC_SMART_BUCK1_CTRL10_ADDR(base)             ((base) + (0x0F2))

/* 寄存器说明：LDO22过流比较器配置寄存器。
   位域定义UNION结构:  SOC_SMART_LDO22_OCP_VOS_UNION */
#define SOC_SMART_LDO22_OCP_VOS_ADDR(base)            ((base) + (0x0F3))

/* 寄存器说明：LDO_BUCK输出电压配置寄存器。
   位域定义UNION结构:  SOC_SMART_VSET_LDO_BUCK_UNION */
#define SOC_SMART_VSET_LDO_BUCK_ADDR(base)            ((base) + (0x0F4))

/* 寄存器说明：XO_LDO控制寄存器。
   位域定义UNION结构:  SOC_SMART_XO_LDO_CTRL_UNION */
#define SOC_SMART_XO_LDO_CTRL_ADDR(base)              ((base) + (0x0F5))

/* 寄存器说明：BUCKO1模式选择寄存器。
   位域定义UNION结构:  SOC_SMART_BUCK01_MOD_CTRL_UNION */
#define SOC_SMART_BUCK01_MOD_CTRL_ADDR(base)          ((base) + (0x0F7))

/* 寄存器说明：sensor时钟控制寄存器。
   位域定义UNION结构:  SOC_SMART_SENSOR_CLK_CTRL_UNION */
#define SOC_SMART_SENSOR_CLK_CTRL_ADDR(base)          ((base) + (0x0F9))

/* 寄存器说明：CK_19M2 pin脚输出时钟控制寄存器。
   位域定义UNION结构:  SOC_SMART_CK_19M2_CTRL_UNION */
#define SOC_SMART_CK_19M2_CTRL_ADDR(base)             ((base) + (0x0FA))

/* 寄存器说明：EXTBUCK控制模式选择寄存器。
   位域定义UNION结构:  SOC_SMART_EXTBUCK_MOD_SEL_UNION */
#define SOC_SMART_EXTBUCK_MOD_SEL_ADDR(base)          ((base) + (0x0FB))

/* 寄存器说明：XO时钟输出波形控制寄存器。
   位域定义UNION结构:  SOC_SMART_XO_WAVE_CTRL_UNION */
#define SOC_SMART_XO_WAVE_CTRL_ADDR(base)             ((base) + (0x0FD))



/****************************************************************************
                     (2/2) NP_PMU_CTRL
 ****************************************************************************/
/* 寄存器说明：中断1非下电记录寄存器。
   位域定义UNION结构:  SOC_SMART_NP_IRQ1_RECORD_UNION */
#define SOC_SMART_NP_IRQ1_RECORD_ADDR(base)           ((base) + (0x011))

/* 寄存器说明：中断2非下电记录寄存器。
   位域定义UNION结构:  SOC_SMART_NP_IRQ2_RECORD_UNION */
#define SOC_SMART_NP_IRQ2_RECORD_ADDR(base)           ((base) + (0x012))

/* 寄存器说明：中断3非下电记录寄存器。
   位域定义UNION结构:  SOC_SMART_NP_IRQ3_RECORD_UNION */
#define SOC_SMART_NP_IRQ3_RECORD_ADDR(base)           ((base) + (0x013))

/* 寄存器说明：短路保护非下电记录1寄存器。
   位域定义UNION结构:  SOC_SMART_NP_SCP_RECORD1_UNION */
#define SOC_SMART_NP_SCP_RECORD1_ADDR(base)           ((base) + (0x014))

/* 寄存器说明：过流保护非下电记录1寄存器。
   位域定义UNION结构:  SOC_SMART_NP_OCP_RECORD1_UNION */
#define SOC_SMART_NP_OCP_RECORD1_ADDR(base)           ((base) + (0x015))

/* 寄存器说明：过流保护非下电记录2寄存器。
   位域定义UNION结构:  SOC_SMART_NP_OCP_RECORD2_UNION */
#define SOC_SMART_NP_OCP_RECORD2_ADDR(base)           ((base) + (0x016))

/* 寄存器说明：过流保护非下电记录3寄存器。
   位域定义UNION结构:  SOC_SMART_NP_OCP_RECORD3_UNION */
#define SOC_SMART_NP_OCP_RECORD3_ADDR(base)           ((base) + (0x017))

/* 寄存器说明：过流保护非下电记录4寄存器。
   位域定义UNION结构:  SOC_SMART_NP_OCP_RECORD4_UNION */
#define SOC_SMART_NP_OCP_RECORD4_ADDR(base)           ((base) + (0x018))

/* 寄存器说明：过流保护非下电记录5寄存器。
   位域定义UNION结构:  SOC_SMART_NP_OCP_RECORD5_UNION */
#define SOC_SMART_NP_OCP_RECORD5_ADDR(base)           ((base) + (0x019))

/* 寄存器说明：CLK_TOP控制2寄存器。
   位域定义UNION结构:  SOC_SMART_CLK_TOP2_UNION */
#define SOC_SMART_CLK_TOP2_ADDR(base)                 ((base) + (0x065))

/* 寄存器说明：RTC当前时间bit[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCDR0_UNION */
#define SOC_SMART_RTCDR0_ADDR(base)                   ((base) + (0x0A9))

/* 寄存器说明：RTC当前时间bit[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCDR1_UNION */
#define SOC_SMART_RTCDR1_ADDR(base)                   ((base) + (0x0AA))

/* 寄存器说明：RTC当前时间bit[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCDR2_UNION */
#define SOC_SMART_RTCDR2_ADDR(base)                   ((base) + (0x0AB))

/* 寄存器说明：RTC当前时间bit[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCDR3_UNION */
#define SOC_SMART_RTCDR3_ADDR(base)                   ((base) + (0x0AC))

/* 寄存器说明：RTC比较bit[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCMR0_UNION */
#define SOC_SMART_RTCMR0_ADDR(base)                   ((base) + (0x0AD))

/* 寄存器说明：RTC比较bit[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCMR1_UNION */
#define SOC_SMART_RTCMR1_ADDR(base)                   ((base) + (0x0AE))

/* 寄存器说明：RTC比较bit[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCMR2_UNION */
#define SOC_SMART_RTCMR2_ADDR(base)                   ((base) + (0x0AF))

/* 寄存器说明：RTC比较bit[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCMR3_UNION */
#define SOC_SMART_RTCMR3_ADDR(base)                   ((base) + (0x0B0))

/* 寄存器说明：RTC加载bit[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCLR0_UNION */
#define SOC_SMART_RTCLR0_ADDR(base)                   ((base) + (0x0B1))

/* 寄存器说明：RTC加载bit[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCLR1_UNION */
#define SOC_SMART_RTCLR1_ADDR(base)                   ((base) + (0x0B2))

/* 寄存器说明：RTC加载bit[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCLR2_UNION */
#define SOC_SMART_RTCLR2_ADDR(base)                   ((base) + (0x0B3))

/* 寄存器说明：RTC加载bit[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_RTCLR3_UNION */
#define SOC_SMART_RTCLR3_ADDR(base)                   ((base) + (0x0B4))

/* 寄存器说明：RTC控制寄存器。
   位域定义UNION结构:  SOC_SMART_RTCCTRL_UNION */
#define SOC_SMART_RTCCTRL_ADDR(base)                  ((base) + (0x0B5))

/* 寄存器说明：OTP读出值0寄存器。
   位域定义UNION结构:  SOC_SMART_OTP_PDOB0_UNION */
#define SOC_SMART_OTP_PDOB0_ADDR(base)                ((base) + (0x0A5))

/* 寄存器说明：OTP读出值1寄存器。
   位域定义UNION结构:  SOC_SMART_OTP_PDOB1_UNION */
#define SOC_SMART_OTP_PDOB1_ADDR(base)                ((base) + (0x0A6))

/* 寄存器说明：OTP读出值2寄存器。
   位域定义UNION结构:  SOC_SMART_OTP_PDOB2_UNION */
#define SOC_SMART_OTP_PDOB2_ADDR(base)                ((base) + (0x0A7))

/* 寄存器说明：OTP读出值3寄存器。
   位域定义UNION结构:  SOC_SMART_OTP_PDOB3_UNION */
#define SOC_SMART_OTP_PDOB3_ADDR(base)                ((base) + (0x0A8))

/* 寄存器说明：CRC_VALUE[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CRC_VALUE0_UNION */
#define SOC_SMART_CRC_VALUE0_ADDR(base)               ((base) + (0x0BB))

/* 寄存器说明：CRC_VALUE[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CRC_VALUE1_UNION */
#define SOC_SMART_CRC_VALUE1_ADDR(base)               ((base) + (0x0BC))

/* 寄存器说明：CRC_VALUE[20:16]寄存器。
   位域定义UNION结构:  SOC_SMART_CRC_VALUE2_UNION */
#define SOC_SMART_CRC_VALUE2_ADDR(base)               ((base) + (0x0BD))

/* 寄存器说明：非下电调节1寄存器。
   位域定义UNION结构:  SOC_SMART_NP_REG_ADJ1_UNION */
#define SOC_SMART_NP_REG_ADJ1_ADDR(base)              ((base) + (0x0BE))

/* 寄存器说明：非下电调节2寄存器。
   位域定义UNION结构:  SOC_SMART_NP_REG_ADJ2_UNION */
#define SOC_SMART_NP_REG_ADJ2_ADDR(base)              ((base) + (0x0BF))

/* 寄存器说明：备用电池非下电调节寄存器。
   位域定义UNION结构:  SOC_SMART_NP_REG_CHG_UNION */
#define SOC_SMART_NP_REG_CHG_ADDR(base)               ((base) + (0x0C0))

/* 寄存器说明：rtc_adj[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_ADJ1_UNION */
#define SOC_SMART_RTC_ADJ1_ADDR(base)                 ((base) + (0x0C1))

/* 寄存器说明：rtc_adj[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_ADJ2_UNION */
#define SOC_SMART_RTC_ADJ2_ADDR(base)                 ((base) + (0x0C2))

/* 寄存器说明：XO_THRESOLD2[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_XO_THRESOLD1_UNION */
#define SOC_SMART_XO_THRESOLD1_ADDR(base)             ((base) + (0x0C3))

/* 寄存器说明：XO_THRESOLD2[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_XO_THRESOLD2_UNION */
#define SOC_SMART_XO_THRESOLD2_ADDR(base)             ((base) + (0x0C4))

/* 寄存器说明：非下电模拟保留0寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED_ANA0_UNION */
#define SOC_SMART_NP_RSVED_ANA0_ADDR(base)            ((base) + (0x0C5))

/* 寄存器说明：非下电模拟保留1寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED_ANA1_UNION */
#define SOC_SMART_NP_RSVED_ANA1_ADDR(base)            ((base) + (0x0C6))

/* 寄存器说明：非下电保留0寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED0_UNION */
#define SOC_SMART_NP_RSVED0_ADDR(base)                ((base) + (0x0C7))

/* 寄存器说明：非下电保留1寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED1_UNION */
#define SOC_SMART_NP_RSVED1_ADDR(base)                ((base) + (0x0C8))

/* 寄存器说明：非下电保留2寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED2_UNION */
#define SOC_SMART_NP_RSVED2_ADDR(base)                ((base) + (0x0C9))

/* 寄存器说明：非下电保留3寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED3_UNION */
#define SOC_SMART_NP_RSVED3_ADDR(base)                ((base) + (0x0CA))

/* 寄存器说明：非下电保留4寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED4_UNION */
#define SOC_SMART_NP_RSVED4_ADDR(base)                ((base) + (0x0CB))

/* 寄存器说明：非下电保留5寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED5_UNION */
#define SOC_SMART_NP_RSVED5_ADDR(base)                ((base) + (0x0CC))

/* 寄存器说明：非下电保留6寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED6_UNION */
#define SOC_SMART_NP_RSVED6_ADDR(base)                ((base) + (0x0CD))

/* 寄存器说明：非下电保留7寄存器。
   位域定义UNION结构:  SOC_SMART_NP_RSVED7_UNION */
#define SOC_SMART_NP_RSVED7_ADDR(base)                ((base) + (0x0CE))

/* 寄存器说明：RTC上电拍照[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_PWRUP_TIMER0_UNION */
#define SOC_SMART_RTC_PWRUP_TIMER0_ADDR(base)         ((base) + (0x0CF))

/* 寄存器说明：RTC上电拍照[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_PWRUP_TIMER1_UNION */
#define SOC_SMART_RTC_PWRUP_TIMER1_ADDR(base)         ((base) + (0x0D0))

/* 寄存器说明：RTC上电拍照[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_PWRUP_TIMER2_UNION */
#define SOC_SMART_RTC_PWRUP_TIMER2_ADDR(base)         ((base) + (0x0D1))

/* 寄存器说明：RTC上电拍照[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_PWRUP_TIMER3_UNION */
#define SOC_SMART_RTC_PWRUP_TIMER3_ADDR(base)         ((base) + (0x0D2))

/* 寄存器说明：RTC下电拍照[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_PWRDOWN_TIMER0_UNION */
#define SOC_SMART_RTC_PWRDOWN_TIMER0_ADDR(base)       ((base) + (0x0D3))

/* 寄存器说明：RTC下电拍照[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_PWRDOWN_TIMER1_UNION */
#define SOC_SMART_RTC_PWRDOWN_TIMER1_ADDR(base)       ((base) + (0x0D4))

/* 寄存器说明：RTC下电拍照[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_PWRDOWN_TIMER2_UNION */
#define SOC_SMART_RTC_PWRDOWN_TIMER2_ADDR(base)       ((base) + (0x0D5))

/* 寄存器说明：RTC下电拍照[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_RTC_PWRDOWN_TIMER3_UNION */
#define SOC_SMART_RTC_PWRDOWN_TIMER3_ADDR(base)       ((base) + (0x0D6))

/* 寄存器说明：XO_DIG_CLK_EN控制器配置寄存器。
   位域定义UNION结构:  SOC_SMART_NOPWR_RC_OFF_EN_UNION */
#define SOC_SMART_NOPWR_RC_OFF_EN_ADDR(base)          ((base) + (0x0EE))

/* 寄存器说明：XO_DIG_CLK控制器配置寄存器。
   位域定义UNION结构:  SOC_SMART_XO_DIG_CLK_UNION */
#define SOC_SMART_XO_DIG_CLK_ADDR(base)               ((base) + (0x0F6))

/* 寄存器说明：SMPL功能开关寄存器。
   位域定义UNION结构:  SOC_SMART_SMPL_OPEN_EN_UNION */
#define SOC_SMART_SMPL_OPEN_EN_ADDR(base)             ((base) + (0x0F8))

/* 寄存器说明：PWR_HOLD_2D滤波配置寄存器。
   位域定义UNION结构:  SOC_SMART_PWR_HOLD_2D_DEB_UNION */
#define SOC_SMART_PWR_HOLD_2D_DEB_ADDR(base)          ((base) + (0x0FC))

/* 寄存器说明：19.2M时钟600与586分频切换使能寄存器。
   位域定义UNION结构:  SOC_SMART_CK19M2_600_586_EN_UNION */
#define SOC_SMART_CK19M2_600_586_EN_ADDR(base)        ((base) + (0x0FE))

/* 寄存器说明：SMPL触发重新上电事件配置寄存器。
   位域定义UNION结构:  SOC_SMART_NP_SMPL_IRQ_UNION */
#define SOC_SMART_NP_SMPL_IRQ_ADDR(base)              ((base) + (0x0FF))

/* 寄存器说明：CLK_TOP控制6寄存器。
   位域定义UNION结构:  SOC_SMART_CLK_TOP6_UNION */
#define SOC_SMART_CLK_TOP6_ADDR(base)                 ((base) + (0x100))

/* 寄存器说明：当前电流[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT_0_UNION */
#define SOC_SMART_CURRENT_0_ADDR(base)                ((base) + (0x14D))

/* 寄存器说明：当前电流[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT_1_UNION */
#define SOC_SMART_CURRENT_1_ADDR(base)                ((base) + (0x14E))

/* 寄存器说明：当前电压[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT_0_UNION */
#define SOC_SMART_V_OUT_0_ADDR(base)                  ((base) + (0x14F))

/* 寄存器说明：当前电压[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT_1_UNION */
#define SOC_SMART_V_OUT_1_ADDR(base)                  ((base) + (0x150))

/* 寄存器说明：库仑计控制寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_CTRL_REG_UNION */
#define SOC_SMART_CLJ_CTRL_REG_ADDR(base)             ((base) + (0x151))

/* 寄存器说明：ECO_REFLASH工作时间寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_REFALSH_TIME_UNION */
#define SOC_SMART_ECO_REFALSH_TIME_ADDR(base)         ((base) + (0x152))

/* 寄存器说明：cl_out[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_OUT0_UNION */
#define SOC_SMART_CL_OUT0_ADDR(base)                  ((base) + (0x153))

/* 寄存器说明：cl_out[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_OUT1_UNION */
#define SOC_SMART_CL_OUT1_ADDR(base)                  ((base) + (0x154))

/* 寄存器说明：cl_out[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_OUT2_UNION */
#define SOC_SMART_CL_OUT2_ADDR(base)                  ((base) + (0x155))

/* 寄存器说明：cl_out[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_OUT3_UNION */
#define SOC_SMART_CL_OUT3_ADDR(base)                  ((base) + (0x156))

/* 寄存器说明：cl_in[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_IN0_UNION */
#define SOC_SMART_CL_IN0_ADDR(base)                   ((base) + (0x157))

/* 寄存器说明：cl_in[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_IN1_UNION */
#define SOC_SMART_CL_IN1_ADDR(base)                   ((base) + (0x158))

/* 寄存器说明：cl_in[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_IN2_UNION */
#define SOC_SMART_CL_IN2_ADDR(base)                   ((base) + (0x159))

/* 寄存器说明：cl_in[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_IN3_UNION */
#define SOC_SMART_CL_IN3_ADDR(base)                   ((base) + (0x15A))

/* 寄存器说明：chg_timer[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CHG_TIMER0_UNION */
#define SOC_SMART_CHG_TIMER0_ADDR(base)               ((base) + (0x15B))

/* 寄存器说明：chg_timer[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CHG_TIMER1_UNION */
#define SOC_SMART_CHG_TIMER1_ADDR(base)               ((base) + (0x15C))

/* 寄存器说明：chg_timer[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_CHG_TIMER2_UNION */
#define SOC_SMART_CHG_TIMER2_ADDR(base)               ((base) + (0x15D))

/* 寄存器说明：chg_timer[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_CHG_TIMER3_UNION */
#define SOC_SMART_CHG_TIMER3_ADDR(base)               ((base) + (0x15E))

/* 寄存器说明：load_timer[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_LOAD_TIMER0_UNION */
#define SOC_SMART_LOAD_TIMER0_ADDR(base)              ((base) + (0x15F))

/* 寄存器说明：load_timer[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_LOAD_TIMER1_UNION */
#define SOC_SMART_LOAD_TIMER1_ADDR(base)              ((base) + (0x160))

/* 寄存器说明：load_timer[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_LOAD_TIMER2_UNION */
#define SOC_SMART_LOAD_TIMER2_ADDR(base)              ((base) + (0x161))

/* 寄存器说明：load_timer[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_LOAD_TIMER3_UNION */
#define SOC_SMART_LOAD_TIMER3_ADDR(base)              ((base) + (0x162))

/* 寄存器说明：cl_int[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_INT0_UNION */
#define SOC_SMART_CL_INT0_ADDR(base)                  ((base) + (0x163))

/* 寄存器说明：cl_int[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_INT1_UNION */
#define SOC_SMART_CL_INT1_ADDR(base)                  ((base) + (0x164))

/* 寄存器说明：cl_int[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_INT2_UNION */
#define SOC_SMART_CL_INT2_ADDR(base)                  ((base) + (0x165))

/* 寄存器说明：cl_int[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_CL_INT3_UNION */
#define SOC_SMART_CL_INT3_ADDR(base)                  ((base) + (0x166))

/* 寄存器说明：v_int[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_INT0_UNION */
#define SOC_SMART_V_INT0_ADDR(base)                   ((base) + (0x167))

/* 寄存器说明：v_int[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_INT1_UNION */
#define SOC_SMART_V_INT1_ADDR(base)                   ((base) + (0x168))

/* 寄存器说明：offset_current[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_OFFSET_CURRENT0_UNION */
#define SOC_SMART_OFFSET_CURRENT0_ADDR(base)          ((base) + (0x169))

/* 寄存器说明：offset_current[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_OFFSET_CURRENT1_UNION */
#define SOC_SMART_OFFSET_CURRENT1_ADDR(base)          ((base) + (0x16A))

/* 寄存器说明：offset_voltage[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_OFFSET_VOLTAGE0_UNION */
#define SOC_SMART_OFFSET_VOLTAGE0_ADDR(base)          ((base) + (0x16B))

/* 寄存器说明：offset_voltage[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_OFFSET_VOLTAGE1_UNION */
#define SOC_SMART_OFFSET_VOLTAGE1_ADDR(base)          ((base) + (0x16C))

/* 寄存器说明：v_ocv_data[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_OCV_VOLTAGE0_UNION */
#define SOC_SMART_OCV_VOLTAGE0_ADDR(base)             ((base) + (0x16D))

/* 寄存器说明：v_ocv_data[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_OCV_VOLTAGE1_UNION */
#define SOC_SMART_OCV_VOLTAGE1_ADDR(base)             ((base) + (0x16E))

/* 寄存器说明：i_ocv_data[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_OCV_CURRENT0_UNION */
#define SOC_SMART_OCV_CURRENT0_ADDR(base)             ((base) + (0x16F))

/* 寄存器说明：i_ocv_data[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_OCV_CURRENT1_UNION */
#define SOC_SMART_OCV_CURRENT1_ADDR(base)             ((base) + (0x170))

/* 寄存器说明：eco_out_clin[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_OUT_CLIN_0_UNION */
#define SOC_SMART_ECO_OUT_CLIN_0_ADDR(base)           ((base) + (0x171))

/* 寄存器说明：eco_out_clin[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_OUT_CLIN_1_UNION */
#define SOC_SMART_ECO_OUT_CLIN_1_ADDR(base)           ((base) + (0x172))

/* 寄存器说明：eco_out_clin[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_OUT_CLIN_2_UNION */
#define SOC_SMART_ECO_OUT_CLIN_2_ADDR(base)           ((base) + (0x173))

/* 寄存器说明：eco_out_clin[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_OUT_CLIN_3_UNION */
#define SOC_SMART_ECO_OUT_CLIN_3_ADDR(base)           ((base) + (0x174))

/* 寄存器说明：eco_out_clout[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_OUT_CLOUT_0_UNION */
#define SOC_SMART_ECO_OUT_CLOUT_0_ADDR(base)          ((base) + (0x175))

/* 寄存器说明：eco_out_clout[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_OUT_CLOUT_1_UNION */
#define SOC_SMART_ECO_OUT_CLOUT_1_ADDR(base)          ((base) + (0x176))

/* 寄存器说明：eco_out_clout[23:16]寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_OUT_CLOUT_2_UNION */
#define SOC_SMART_ECO_OUT_CLOUT_2_ADDR(base)          ((base) + (0x177))

/* 寄存器说明：eco_out_clout[31:24]寄存器。
   位域定义UNION结构:  SOC_SMART_ECO_OUT_CLOUT_3_UNION */
#define SOC_SMART_ECO_OUT_CLOUT_3_ADDR(base)          ((base) + (0x178))

/* 寄存器说明：v_out_pre0[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE0_UNION */
#define SOC_SMART_V_OUT0_PRE0_ADDR(base)              ((base) + (0x179))

/* 寄存器说明：v_out_pre0[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE0_UNION */
#define SOC_SMART_V_OUT1_PRE0_ADDR(base)              ((base) + (0x17A))

/* 寄存器说明：v_out_pre1[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE1_UNION */
#define SOC_SMART_V_OUT0_PRE1_ADDR(base)              ((base) + (0x17B))

/* 寄存器说明：v_out_pre1[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE1_UNION */
#define SOC_SMART_V_OUT1_PRE1_ADDR(base)              ((base) + (0x17C))

/* 寄存器说明：v_out_pre2[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE2_UNION */
#define SOC_SMART_V_OUT0_PRE2_ADDR(base)              ((base) + (0x17D))

/* 寄存器说明：v_out_pre2[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE2_UNION */
#define SOC_SMART_V_OUT1_PRE2_ADDR(base)              ((base) + (0x17E))

/* 寄存器说明：v_out_pre3[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE3_UNION */
#define SOC_SMART_V_OUT0_PRE3_ADDR(base)              ((base) + (0x17F))

/* 寄存器说明：v_out_pre3[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE3_UNION */
#define SOC_SMART_V_OUT1_PRE3_ADDR(base)              ((base) + (0x180))

/* 寄存器说明：v_out_pre4[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE4_UNION */
#define SOC_SMART_V_OUT0_PRE4_ADDR(base)              ((base) + (0x181))

/* 寄存器说明：v_out_pre4[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE4_UNION */
#define SOC_SMART_V_OUT1_PRE4_ADDR(base)              ((base) + (0x182))

/* 寄存器说明：v_out_pre5[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE5_UNION */
#define SOC_SMART_V_OUT0_PRE5_ADDR(base)              ((base) + (0x183))

/* 寄存器说明：v_out_pre5[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE5_UNION */
#define SOC_SMART_V_OUT1_PRE5_ADDR(base)              ((base) + (0x184))

/* 寄存器说明：v_out_pre6[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE6_UNION */
#define SOC_SMART_V_OUT0_PRE6_ADDR(base)              ((base) + (0x185))

/* 寄存器说明：v_out_pre6[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE6_UNION */
#define SOC_SMART_V_OUT1_PRE6_ADDR(base)              ((base) + (0x186))

/* 寄存器说明：v_out_pre7[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE7_UNION */
#define SOC_SMART_V_OUT0_PRE7_ADDR(base)              ((base) + (0x187))

/* 寄存器说明：v_out_pre7[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE7_UNION */
#define SOC_SMART_V_OUT1_PRE7_ADDR(base)              ((base) + (0x188))

/* 寄存器说明：v_out_pre8[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE8_UNION */
#define SOC_SMART_V_OUT0_PRE8_ADDR(base)              ((base) + (0x189))

/* 寄存器说明：v_out_pre8[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE8_UNION */
#define SOC_SMART_V_OUT1_PRE8_ADDR(base)              ((base) + (0x18A))

/* 寄存器说明：v_out_pre9[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT0_PRE9_UNION */
#define SOC_SMART_V_OUT0_PRE9_ADDR(base)              ((base) + (0x18B))

/* 寄存器说明：v_out_pre9[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_V_OUT1_PRE9_UNION */
#define SOC_SMART_V_OUT1_PRE9_ADDR(base)              ((base) + (0x18C))

/* 寄存器说明：current_pre0[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE0_UNION */
#define SOC_SMART_CURRENT0_PRE0_ADDR(base)            ((base) + (0x18D))

/* 寄存器说明：current_pre0[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE0_UNION */
#define SOC_SMART_CURRENT1_PRE0_ADDR(base)            ((base) + (0x18E))

/* 寄存器说明：current_pre1[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE1_UNION */
#define SOC_SMART_CURRENT0_PRE1_ADDR(base)            ((base) + (0x18F))

/* 寄存器说明：current_pre1[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE1_UNION */
#define SOC_SMART_CURRENT1_PRE1_ADDR(base)            ((base) + (0x190))

/* 寄存器说明：current_pre2[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE2_UNION */
#define SOC_SMART_CURRENT0_PRE2_ADDR(base)            ((base) + (0x191))

/* 寄存器说明：current_pre2[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE2_UNION */
#define SOC_SMART_CURRENT1_PRE2_ADDR(base)            ((base) + (0x192))

/* 寄存器说明：current_pre3[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE3_UNION */
#define SOC_SMART_CURRENT0_PRE3_ADDR(base)            ((base) + (0x193))

/* 寄存器说明：current_pre3[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE3_UNION */
#define SOC_SMART_CURRENT1_PRE3_ADDR(base)            ((base) + (0x194))

/* 寄存器说明：current_pre4[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE4_UNION */
#define SOC_SMART_CURRENT0_PRE4_ADDR(base)            ((base) + (0x195))

/* 寄存器说明：current_pre4[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE4_UNION */
#define SOC_SMART_CURRENT1_PRE4_ADDR(base)            ((base) + (0x196))

/* 寄存器说明：current_pre5[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE5_UNION */
#define SOC_SMART_CURRENT0_PRE5_ADDR(base)            ((base) + (0x197))

/* 寄存器说明：current_pre5[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE5_UNION */
#define SOC_SMART_CURRENT1_PRE5_ADDR(base)            ((base) + (0x198))

/* 寄存器说明：current_pre6[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE6_UNION */
#define SOC_SMART_CURRENT0_PRE6_ADDR(base)            ((base) + (0x199))

/* 寄存器说明：current_pre6[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE6_UNION */
#define SOC_SMART_CURRENT1_PRE6_ADDR(base)            ((base) + (0x19A))

/* 寄存器说明：current_pre7[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE7_UNION */
#define SOC_SMART_CURRENT0_PRE7_ADDR(base)            ((base) + (0x19B))

/* 寄存器说明：current_pre7[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE7_UNION */
#define SOC_SMART_CURRENT1_PRE7_ADDR(base)            ((base) + (0x19C))

/* 寄存器说明：current_pre8[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE8_UNION */
#define SOC_SMART_CURRENT0_PRE8_ADDR(base)            ((base) + (0x19D))

/* 寄存器说明：current_pre8[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE8_UNION */
#define SOC_SMART_CURRENT1_PRE8_ADDR(base)            ((base) + (0x19E))

/* 寄存器说明：current_pre9[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT0_PRE9_UNION */
#define SOC_SMART_CURRENT0_PRE9_ADDR(base)            ((base) + (0x19F))

/* 寄存器说明：current_pre9[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_CURRENT1_PRE9_UNION */
#define SOC_SMART_CURRENT1_PRE9_ADDR(base)            ((base) + (0x1A0))

/* 寄存器说明：电流偏置调整[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_OFFSET_CURRENT_MOD_0_UNION */
#define SOC_SMART_OFFSET_CURRENT_MOD_0_ADDR(base)     ((base) + (0x1A1))

/* 寄存器说明：电流偏置调整[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_OFFSET_CURRENT_MOD_1_UNION */
#define SOC_SMART_OFFSET_CURRENT_MOD_1_ADDR(base)     ((base) + (0x1A2))

/* 寄存器说明：电压偏置调整[7:0]寄存器。
   位域定义UNION结构:  SOC_SMART_OFFSET_VOLTAGE_MOD_0_UNION */
#define SOC_SMART_OFFSET_VOLTAGE_MOD_0_ADDR(base)     ((base) + (0x1A3))

/* 寄存器说明：电压偏置调整[15:8]寄存器。
   位域定义UNION结构:  SOC_SMART_OFFSET_VOLTAGE_MOD_1_UNION */
#define SOC_SMART_OFFSET_VOLTAGE_MOD_1_ADDR(base)     ((base) + (0x1A4))

/* 寄存器说明：库仑计保留1寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_RESERVED1_UNION */
#define SOC_SMART_CLJ_RESERVED1_ADDR(base)            ((base) + (0x1A5))

/* 寄存器说明：库仑计保留2寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_RESERVED2_UNION */
#define SOC_SMART_CLJ_RESERVED2_ADDR(base)            ((base) + (0x1A6))

/* 寄存器说明：库仑计保留3寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_RESERVED3_UNION */
#define SOC_SMART_CLJ_RESERVED3_ADDR(base)            ((base) + (0x1A7))

/* 寄存器说明：库仑计保留4寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_RESERVED4_UNION */
#define SOC_SMART_CLJ_RESERVED4_ADDR(base)            ((base) + (0x1A8))

/* 寄存器说明：库仑计保留5寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_RESERVED5_UNION */
#define SOC_SMART_CLJ_RESERVED5_ADDR(base)            ((base) + (0x1A9))

/* 寄存器说明：库仑计保留6寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_RESERVED6_UNION */
#define SOC_SMART_CLJ_RESERVED6_ADDR(base)            ((base) + (0x1AA))

/* 寄存器说明：库仑计保留7寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_RESERVED7_UNION */
#define SOC_SMART_CLJ_RESERVED7_ADDR(base)            ((base) + (0x1AB))

/* 寄存器说明：PMU软复位寄存器。
   位域定义UNION结构:  SOC_SMART_PMU_SOFT_RST_UNION */
#define SOC_SMART_PMU_SOFT_RST_ADDR(base)             ((base) + (0x1AC))

/* 寄存器说明：库仑计DEBUG专用1寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_DEBUG_UNION */
#define SOC_SMART_CLJ_DEBUG_ADDR(base)                ((base) + (0x1AD))

/* 寄存器说明：库仑计DEBUG专用2寄存器。
   位域定义UNION结构:  SOC_SMART_CLJ_DEBUG_2_UNION */
#define SOC_SMART_CLJ_DEBUG_2_ADDR(base)              ((base) + (0x1AE))

/* 寄存器说明：库仑计DEBUG专用3寄存器。
   位域定义UNION结构:  SOC_SMART_STATE_TEST_UNION */
#define SOC_SMART_STATE_TEST_ADDR(base)               ((base) + (0x1AF))





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
                     (1/2) PMU_CTRL
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_SMART_VERSION_UNION
 结构说明  : VERSION 寄存器结构定义。地址偏移量:0x000，初值:0x10，宽度:8
 寄存器说明: 版本寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chip_id : 8;  /* bit[0-7]: 版本寄存器，表示版本为V100。 */
    } reg;
} SOC_SMART_VERSION_UNION;
#endif
#define SOC_SMART_VERSION_chip_id_START  (0)
#define SOC_SMART_VERSION_chip_id_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_STATUS1_UNION
 结构说明  : STATUS1 寄存器结构定义。地址偏移量:0x001，初值:0x00，宽度:8
 寄存器说明: 状态标志1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otmp_d1r           : 1;  /* bit[0]  : 0：当前温度低于设定阈值（默认125℃）；
                                                             1：当前温度高于设定阈值。 */
        unsigned char  otmp150_d1r        : 1;  /* bit[1]  : 0：当前温度低于150℃；
                                                             1：当前温度高于150℃。 */
        unsigned char  vsys_uv_d3r        : 1;  /* bit[2]  : 当前vsys电压：
                                                             0：vsys>3.0V；
                                                             1：vsys<3.0V； */
        unsigned char  vsys_6p0_d200ur    : 1;  /* bit[3]  : 当前vsys电压：
                                                             0：vsys<6.0V；
                                                             1：vsys>6.0V； */
        unsigned char  pwron_d20r         : 1;  /* bit[4]  : 0：PWRON键当前没有被按下；
                                                             1:PWRON键当前被按下(pwron为pwron_n的反向)。 */
        unsigned char  vbat_vcoin_sel_ff2 : 1;  /* bit[5]  : VBAT与VCION切换信号
                                                             0：VCOIN；
                                                             1：VBAT； */
        unsigned char  reserved           : 2;  /* bit[6-7]: 保留。 */
    } reg;
} SOC_SMART_STATUS1_UNION;
#endif
#define SOC_SMART_STATUS1_otmp_d1r_START            (0)
#define SOC_SMART_STATUS1_otmp_d1r_END              (0)
#define SOC_SMART_STATUS1_otmp150_d1r_START         (1)
#define SOC_SMART_STATUS1_otmp150_d1r_END           (1)
#define SOC_SMART_STATUS1_vsys_uv_d3r_START         (2)
#define SOC_SMART_STATUS1_vsys_uv_d3r_END           (2)
#define SOC_SMART_STATUS1_vsys_6p0_d200ur_START     (3)
#define SOC_SMART_STATUS1_vsys_6p0_d200ur_END       (3)
#define SOC_SMART_STATUS1_pwron_d20r_START          (4)
#define SOC_SMART_STATUS1_pwron_d20r_END            (4)
#define SOC_SMART_STATUS1_vbat_vcoin_sel_ff2_START  (5)
#define SOC_SMART_STATUS1_vbat_vcoin_sel_ff2_END    (5)


/*****************************************************************************
 结构名    : SOC_SMART_STATUS2_UNION
 结构说明  : STATUS2 寄存器结构定义。地址偏移量:0x002，初值:0x00，宽度:8
 寄存器说明: 状态标志2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vbus_det_3p6_d20r : 1;  /* bit[0]  : vbus_det_3p6：
                                                            0: 没有插入；
                                                            1：插入。 */
        unsigned char  dcxo_clk_sel      : 1;  /* bit[1]  : DCXO模式当前工作时钟源：
                                                            0：32K RC ；
                                                            1：19.2MHz/600 ； */
        unsigned char  tcxo_clk_sel      : 1;  /* bit[2]  : TCXO模式当前工作时钟源：
                                                            0：32K RC ；
                                                            1：32K Crystal OSC ； */
        unsigned char  xo_mode_a2d_ff2   : 1;  /* bit[3]  : 芯片当前工作模式：
                                                            0：DCXO
                                                            1：TCXO */
        unsigned char  reserved          : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_SMART_STATUS2_UNION;
#endif
#define SOC_SMART_STATUS2_vbus_det_3p6_d20r_START  (0)
#define SOC_SMART_STATUS2_vbus_det_3p6_d20r_END    (0)
#define SOC_SMART_STATUS2_dcxo_clk_sel_START       (1)
#define SOC_SMART_STATUS2_dcxo_clk_sel_END         (1)
#define SOC_SMART_STATUS2_tcxo_clk_sel_START       (2)
#define SOC_SMART_STATUS2_tcxo_clk_sel_END         (2)
#define SOC_SMART_STATUS2_xo_mode_a2d_ff2_START    (3)
#define SOC_SMART_STATUS2_xo_mode_a2d_ff2_END      (3)


/*****************************************************************************
 结构名    : SOC_SMART_IRQ1_UNION
 结构说明  : IRQ1 寄存器结构定义。地址偏移量:0x003，初值:0x00，宽度:8
 寄存器说明: 中断1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otmp_d1r        : 1;  /* bit[0]: 0:无此中断；
                                                        1：otmp 报温度超过设定阈值（默认125度）中断。 */
        unsigned char  vsys_2p5_r      : 1;  /* bit[1]: 0:无此中断；
                                                        1:vsys电压低于2.5V中断。 */
        unsigned char  vsys_uv_d3r     : 1;  /* bit[2]: 0:无此中断；
                                                        1:vsys电压低于3.0V 3ms中断。(默认3V，非下电寄存器vsys_uv_adj可调) */
        unsigned char  vsys_6p0_d200ur : 1;  /* bit[3]: 0：无此中断；
                                                        1:vsys报电压高于6.0V 200us中断。 */
        unsigned char  pwron_d4sr      : 1;  /* bit[4]: 0:无此中断；
                                                        1:PWRON按键按下4s中断(该中断只在正常工作时按下才上报) 。 */
        unsigned char  pwron_d20f      : 1;  /* bit[5]: 0:无此中断；
                                                        1:PWRON按键释放20ms中断。 */
        unsigned char  pwron_d20r      : 1;  /* bit[6]: 0:无此中断；
                                                        1:PWRON按键按下20ms中断。 */
        unsigned char  reserved        : 1;  /* bit[7]: 保留。 */
    } reg;
} SOC_SMART_IRQ1_UNION;
#endif
#define SOC_SMART_IRQ1_otmp_d1r_START         (0)
#define SOC_SMART_IRQ1_otmp_d1r_END           (0)
#define SOC_SMART_IRQ1_vsys_2p5_r_START       (1)
#define SOC_SMART_IRQ1_vsys_2p5_r_END         (1)
#define SOC_SMART_IRQ1_vsys_uv_d3r_START      (2)
#define SOC_SMART_IRQ1_vsys_uv_d3r_END        (2)
#define SOC_SMART_IRQ1_vsys_6p0_d200ur_START  (3)
#define SOC_SMART_IRQ1_vsys_6p0_d200ur_END    (3)
#define SOC_SMART_IRQ1_pwron_d4sr_START       (4)
#define SOC_SMART_IRQ1_pwron_d4sr_END         (4)
#define SOC_SMART_IRQ1_pwron_d20f_START       (5)
#define SOC_SMART_IRQ1_pwron_d20f_END         (5)
#define SOC_SMART_IRQ1_pwron_d20r_START       (6)
#define SOC_SMART_IRQ1_pwron_d20r_END         (6)


/*****************************************************************************
 结构名    : SOC_SMART_IRQ2_UNION
 结构说明  : IRQ2 寄存器结构定义。地址偏移量:0x004，初值:0x00，宽度:8
 寄存器说明: 中断2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vbus_det_3p6_d20f : 1;  /* bit[0]  : 0:无此中断；
                                                            1:vbus_det<3.6 V 20ms中断。 */
        unsigned char  vbus_det_3p6_d20r : 1;  /* bit[1]  : 0:无此中断；
                                                            1:vbus_det>3.6 V 20ms中断。 */
        unsigned char  ocp_scp_r         : 1;  /* bit[2]  : 0:无此中断；
                                                            1:表示至少有一个BUCK发生过流保护或者短路保护，或者CLASSD/LDO/LVS/发生过流保护，CPU查询到后要去查看下面的过流保存寄存器查看是哪一个发生过流，并将其写1清后，中断消除。 */
        unsigned char  reserved          : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_SMART_IRQ2_UNION;
#endif
#define SOC_SMART_IRQ2_vbus_det_3p6_d20f_START  (0)
#define SOC_SMART_IRQ2_vbus_det_3p6_d20f_END    (0)
#define SOC_SMART_IRQ2_vbus_det_3p6_d20r_START  (1)
#define SOC_SMART_IRQ2_vbus_det_3p6_d20r_END    (1)
#define SOC_SMART_IRQ2_ocp_scp_r_START          (2)
#define SOC_SMART_IRQ2_ocp_scp_r_END            (2)


/*****************************************************************************
 结构名    : SOC_SMART_IRQ3_UNION
 结构说明  : IRQ3 寄存器结构定义。地址偏移量:0x005，初值:0x00，宽度:8
 寄存器说明: 中断3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tcxo_clk_sel_r : 1;  /* bit[0]  : 0：无此中断；
                                                         1：32kRC切换到32K Crystal OSC 时钟； */
        unsigned char  tcxo_clk_sel_f : 1;  /* bit[1]  : 0：无此中断；
                                                         1：32K Crystal OSC 切换到32kRC时钟； */
        unsigned char  dcxo_clk_sel_r : 1;  /* bit[2]  : 0：无此中断；
                                                         1：32K RC 切换到19.2MHz/600 时钟； */
        unsigned char  dcxo_clk_sel_f : 1;  /* bit[3]  : 0：无此中断；
                                                         1：19.2MHz/600 切换到32K RC时钟； */
        unsigned char  alarm_on       : 1;  /* bit[4]  : ALARM_ON中断：
                                                         0:无此中断；
                                                         1：当RTC时间与alarm时间相同时,上报中断。 */
        unsigned char  reserved       : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_IRQ3_UNION;
#endif
#define SOC_SMART_IRQ3_tcxo_clk_sel_r_START  (0)
#define SOC_SMART_IRQ3_tcxo_clk_sel_r_END    (0)
#define SOC_SMART_IRQ3_tcxo_clk_sel_f_START  (1)
#define SOC_SMART_IRQ3_tcxo_clk_sel_f_END    (1)
#define SOC_SMART_IRQ3_dcxo_clk_sel_r_START  (2)
#define SOC_SMART_IRQ3_dcxo_clk_sel_r_END    (2)
#define SOC_SMART_IRQ3_dcxo_clk_sel_f_START  (3)
#define SOC_SMART_IRQ3_dcxo_clk_sel_f_END    (3)
#define SOC_SMART_IRQ3_alarm_on_START        (4)
#define SOC_SMART_IRQ3_alarm_on_END          (4)


/*****************************************************************************
 结构名    : SOC_SMART_COUL_IRQ_UNION
 结构说明  : COUL_IRQ 寄存器结构定义。地址偏移量:0x006，初值:0x00，宽度:8
 寄存器说明: 库仑计中断寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_int_i   : 1;  /* bit[0]  : cl_out>cl_int时上报中断。
                                                     0：无此中断；
                                                     1：上报中断。 */
        unsigned char  cl_out_i   : 1;  /* bit[1]  : cl_out计数到81%上报中断。
                                                     0：无此中断；
                                                     1：上报中断。 */
        unsigned char  cl_in_i    : 1;  /* bit[2]  : cl_in计数到81%上报中断。
                                                     0：无此中断；
                                                     1：上报中断。 */
        unsigned char  vbat_int_i : 1;  /* bit[3]  : vbat电压<设定的vbat_int值。
                                                     0：无此中断；
                                                     1：上报中断。 */
        unsigned char  reserved   : 4;  /* bit[4-7]:  */
    } reg;
} SOC_SMART_COUL_IRQ_UNION;
#endif
#define SOC_SMART_COUL_IRQ_cl_int_i_START    (0)
#define SOC_SMART_COUL_IRQ_cl_int_i_END      (0)
#define SOC_SMART_COUL_IRQ_cl_out_i_START    (1)
#define SOC_SMART_COUL_IRQ_cl_out_i_END      (1)
#define SOC_SMART_COUL_IRQ_cl_in_i_START     (2)
#define SOC_SMART_COUL_IRQ_cl_in_i_END       (2)
#define SOC_SMART_COUL_IRQ_vbat_int_i_START  (3)
#define SOC_SMART_COUL_IRQ_vbat_int_i_END    (3)


/*****************************************************************************
 结构名    : SOC_SMART_IRQ1_MASK_UNION
 结构说明  : IRQ1_MASK 寄存器结构定义。地址偏移量:0x007，初值:0x00，宽度:8
 寄存器说明: 中断屏蔽1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otmp_d1r_mk        : 1;  /* bit[0]: 0:使能该中断；
                                                           1：屏蔽该中断。 */
        unsigned char  vsys_2p5_r_mk      : 1;  /* bit[1]: 0:使能该中断；
                                                           1：屏蔽该中断。 */
        unsigned char  vsys_uv_d3r_mk     : 1;  /* bit[2]: 0:使能该中断；
                                                           1：屏蔽该中断。 */
        unsigned char  vsys_6p0_d200ur_mk : 1;  /* bit[3]: 0:使能该中断；
                                                           1：屏蔽该中断。 */
        unsigned char  pwron_d4sr_mk      : 1;  /* bit[4]: 0:使能该中断；
                                                           1：屏蔽该中断。 */
        unsigned char  pwron_d20f_mk      : 1;  /* bit[5]: 0:使能该中断；
                                                           1：屏蔽该中断。 */
        unsigned char  pwron_d20r_mk      : 1;  /* bit[6]: 0:使能该中断；
                                                           1：屏蔽该中断。 */
        unsigned char  reserved           : 1;  /* bit[7]: 保留。 */
    } reg;
} SOC_SMART_IRQ1_MASK_UNION;
#endif
#define SOC_SMART_IRQ1_MASK_otmp_d1r_mk_START         (0)
#define SOC_SMART_IRQ1_MASK_otmp_d1r_mk_END           (0)
#define SOC_SMART_IRQ1_MASK_vsys_2p5_r_mk_START       (1)
#define SOC_SMART_IRQ1_MASK_vsys_2p5_r_mk_END         (1)
#define SOC_SMART_IRQ1_MASK_vsys_uv_d3r_mk_START      (2)
#define SOC_SMART_IRQ1_MASK_vsys_uv_d3r_mk_END        (2)
#define SOC_SMART_IRQ1_MASK_vsys_6p0_d200ur_mk_START  (3)
#define SOC_SMART_IRQ1_MASK_vsys_6p0_d200ur_mk_END    (3)
#define SOC_SMART_IRQ1_MASK_pwron_d4sr_mk_START       (4)
#define SOC_SMART_IRQ1_MASK_pwron_d4sr_mk_END         (4)
#define SOC_SMART_IRQ1_MASK_pwron_d20f_mk_START       (5)
#define SOC_SMART_IRQ1_MASK_pwron_d20f_mk_END         (5)
#define SOC_SMART_IRQ1_MASK_pwron_d20r_mk_START       (6)
#define SOC_SMART_IRQ1_MASK_pwron_d20r_mk_END         (6)


/*****************************************************************************
 结构名    : SOC_SMART_IRQ2_MASK_UNION
 结构说明  : IRQ2_MASK 寄存器结构定义。地址偏移量:0x008，初值:0x00，宽度:8
 寄存器说明: 中断屏蔽2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vbus_det_3p6_d20f_mk : 1;  /* bit[0]  : 0:使能该中断；
                                                               1：屏蔽该中断。 */
        unsigned char  vbus_det_3p6_d20r_mk : 1;  /* bit[1]  : 0:使能该中断；
                                                               1：屏蔽该中断。 */
        unsigned char  ocp_scp_r_mk         : 1;  /* bit[2]  : 0:使能该中断；
                                                               1：屏蔽该中断。 */
        unsigned char  reserved             : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_SMART_IRQ2_MASK_UNION;
#endif
#define SOC_SMART_IRQ2_MASK_vbus_det_3p6_d20f_mk_START  (0)
#define SOC_SMART_IRQ2_MASK_vbus_det_3p6_d20f_mk_END    (0)
#define SOC_SMART_IRQ2_MASK_vbus_det_3p6_d20r_mk_START  (1)
#define SOC_SMART_IRQ2_MASK_vbus_det_3p6_d20r_mk_END    (1)
#define SOC_SMART_IRQ2_MASK_ocp_scp_r_mk_START          (2)
#define SOC_SMART_IRQ2_MASK_ocp_scp_r_mk_END            (2)


/*****************************************************************************
 结构名    : SOC_SMART_IRQ3_MASK_UNION
 结构说明  : IRQ3_MASK 寄存器结构定义。地址偏移量:0x009，初值:0x00，宽度:8
 寄存器说明: 中断屏蔽3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tcxo_clk_sel_r_mk : 1;  /* bit[0]  : 0:使能该中断；
                                                            1：屏蔽该中断。 */
        unsigned char  tcxo_clk_sel_f_mk : 1;  /* bit[1]  : 0:使能该中断；
                                                            1：屏蔽该中断。 */
        unsigned char  dcxo_clk_sel_r_mk : 1;  /* bit[2]  : 0:使能该中断；
                                                            1：屏蔽该中断。 */
        unsigned char  dcxo_clk_sel_f_mk : 1;  /* bit[3]  : 0:使能该中断；
                                                            1：屏蔽该中断。 */
        unsigned char  alarm_on_mk       : 1;  /* bit[4]  : ALARM_ON中断：
                                                            0:使能该中断；
                                                            1：屏蔽该中断。 */
        unsigned char  reserved          : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_IRQ3_MASK_UNION;
#endif
#define SOC_SMART_IRQ3_MASK_tcxo_clk_sel_r_mk_START  (0)
#define SOC_SMART_IRQ3_MASK_tcxo_clk_sel_r_mk_END    (0)
#define SOC_SMART_IRQ3_MASK_tcxo_clk_sel_f_mk_START  (1)
#define SOC_SMART_IRQ3_MASK_tcxo_clk_sel_f_mk_END    (1)
#define SOC_SMART_IRQ3_MASK_dcxo_clk_sel_r_mk_START  (2)
#define SOC_SMART_IRQ3_MASK_dcxo_clk_sel_r_mk_END    (2)
#define SOC_SMART_IRQ3_MASK_dcxo_clk_sel_f_mk_START  (3)
#define SOC_SMART_IRQ3_MASK_dcxo_clk_sel_f_mk_END    (3)
#define SOC_SMART_IRQ3_MASK_alarm_on_mk_START        (4)
#define SOC_SMART_IRQ3_MASK_alarm_on_mk_END          (4)


/*****************************************************************************
 结构名    : SOC_SMART_COUL_IRQ_MASK_UNION
 结构说明  : COUL_IRQ_MASK 寄存器结构定义。地址偏移量:0x00A，初值:0x00，宽度:8
 寄存器说明: 库仑计中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_int_i_mk   : 1;  /* bit[0]  : cl_int_i中断屏蔽位。
                                                        0：不屏蔽；
                                                        1：cl_int_i中断屏蔽。 */
        unsigned char  cl_out_i_mk   : 1;  /* bit[1]  : cl_out_i中断屏蔽位。
                                                        0：不屏蔽；
                                                        1：cl_out_i中断屏蔽。 */
        unsigned char  cl_in_i_mk    : 1;  /* bit[2]  : cl_in_i中断屏蔽位。
                                                        0：不屏蔽；
                                                        1：cl_in_i中断屏蔽。 */
        unsigned char  vbat_int_i_mk : 1;  /* bit[3]  : vbat_int_i中断屏蔽位。
                                                        0：不屏蔽；
                                                        1：vbat_int_i中断屏蔽。 */
        unsigned char  reserved      : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_SMART_COUL_IRQ_MASK_UNION;
#endif
#define SOC_SMART_COUL_IRQ_MASK_cl_int_i_mk_START    (0)
#define SOC_SMART_COUL_IRQ_MASK_cl_int_i_mk_END      (0)
#define SOC_SMART_COUL_IRQ_MASK_cl_out_i_mk_START    (1)
#define SOC_SMART_COUL_IRQ_MASK_cl_out_i_mk_END      (1)
#define SOC_SMART_COUL_IRQ_MASK_cl_in_i_mk_START     (2)
#define SOC_SMART_COUL_IRQ_MASK_cl_in_i_mk_END       (2)
#define SOC_SMART_COUL_IRQ_MASK_vbat_int_i_mk_START  (3)
#define SOC_SMART_COUL_IRQ_MASK_vbat_int_i_mk_END    (3)


/*****************************************************************************
 结构名    : SOC_SMART_SCP_BUCK_RECORD1_UNION
 结构说明  : SCP_BUCK_RECORD1 寄存器结构定义。地址偏移量:0x00B，初值:0x00，宽度:8
 寄存器说明: 短路保护记录1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  scp_buck01 : 1;  /* bit[0]  : 0：BUCK01输出不短路；
                                                     1: BUCK01输出短路。 */
        unsigned char  scp_buck2  : 1;  /* bit[1]  : 0：BUCK2输出不短路；
                                                     1: BUCK2输出短路。 */
        unsigned char  scp_buck3  : 1;  /* bit[2]  : 0：BUCK3输出不短路；
                                                     1: BUCK3输出短路。 */
        unsigned char  scp_buck4  : 1;  /* bit[3]  : 0：BUCK4输出不短路；
                                                     1: BUCK4输出短路。 */
        unsigned char  reserved   : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_SMART_SCP_BUCK_RECORD1_UNION;
#endif
#define SOC_SMART_SCP_BUCK_RECORD1_scp_buck01_START  (0)
#define SOC_SMART_SCP_BUCK_RECORD1_scp_buck01_END    (0)
#define SOC_SMART_SCP_BUCK_RECORD1_scp_buck2_START   (1)
#define SOC_SMART_SCP_BUCK_RECORD1_scp_buck2_END     (1)
#define SOC_SMART_SCP_BUCK_RECORD1_scp_buck3_START   (2)
#define SOC_SMART_SCP_BUCK_RECORD1_scp_buck3_END     (2)
#define SOC_SMART_SCP_BUCK_RECORD1_scp_buck4_START   (3)
#define SOC_SMART_SCP_BUCK_RECORD1_scp_buck4_END     (3)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_BUCK_RECORD1_UNION
 结构说明  : OCP_BUCK_RECORD1 寄存器结构定义。地址偏移量:0x00C，初值:0x00，宽度:8
 寄存器说明: 过流保护记录1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_buck1 : 1;  /* bit[0]  : 0：BUCK1输出不过载；
                                                    1: BUCK1输出过载。 */
        unsigned char  ocp_buck2 : 1;  /* bit[1]  : 0：BUCK2输出不过载；
                                                    1: BUCK2输出过载。 */
        unsigned char  ocp_buck3 : 1;  /* bit[2]  : 0：BUCK3输出不过载；
                                                    1: BUCK3输出过载。 */
        unsigned char  ocp_buck4 : 1;  /* bit[3]  : 0：BUCK4输出不过载；
                                                    1: BUCK4输出过载。 */
        unsigned char  ocp_buck0 : 1;  /* bit[4]  : 0：BUCK0输出不过载；
                                                    1: BUCK0输出过载。 */
        unsigned char  reserved  : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_OCP_BUCK_RECORD1_UNION;
#endif
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck1_START  (0)
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck1_END    (0)
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck2_START  (1)
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck2_END    (1)
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck3_START  (2)
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck3_END    (2)
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck4_START  (3)
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck4_END    (3)
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck0_START  (4)
#define SOC_SMART_OCP_BUCK_RECORD1_ocp_buck0_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_LDO1_8_RECORD2_UNION
 结构说明  : OCP_LDO1_8_RECORD2 寄存器结构定义。地址偏移量:0x00D，初值:0x00，宽度:8
 寄存器说明: 过流保护记录2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ldo1 : 1;  /* bit[0]: 0：LDO1输出不过载；
                                                 1: LDO1输出过载。 */
        unsigned char  ocp_ldo2 : 1;  /* bit[1]: 0：LDO2输出不过载；
                                                 1: LDO2输出过载。 */
        unsigned char  ocp_ldo3 : 1;  /* bit[2]: 0：LDO3输出不过载；
                                                 1: LDO3输出过载。 */
        unsigned char  ocp_ldo4 : 1;  /* bit[3]: 0：LDO4输出不过载；
                                                 1: LDO4输出过载。 */
        unsigned char  ocp_ldo5 : 1;  /* bit[4]: 0：LDO5输出不过载；
                                                 1: LDO5输出过载。 */
        unsigned char  ocp_ldo6 : 1;  /* bit[5]: 0：LDO6输出不过载；
                                                 1: LDO6输出过载。 */
        unsigned char  ocp_ldo7 : 1;  /* bit[6]: 0：LDO7输出不过载；
                                                 1: LDO7输出过载。 */
        unsigned char  ocp_ldo8 : 1;  /* bit[7]: 0：LDO8输出不过载；
                                                 1: LDO8输出过载。 */
    } reg;
} SOC_SMART_OCP_LDO1_8_RECORD2_UNION;
#endif
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo1_START  (0)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo1_END    (0)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo2_START  (1)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo2_END    (1)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo3_START  (2)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo3_END    (2)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo4_START  (3)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo4_END    (3)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo5_START  (4)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo5_END    (4)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo6_START  (5)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo6_END    (5)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo7_START  (6)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo7_END    (6)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo8_START  (7)
#define SOC_SMART_OCP_LDO1_8_RECORD2_ocp_ldo8_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_LDO16_9_RECORD3_UNION
 结构说明  : OCP_LDO16_9_RECORD3 寄存器结构定义。地址偏移量:0x00E，初值:0x00，宽度:8
 寄存器说明: 过流保护记录3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ldo9  : 1;  /* bit[0]: 0：LDO9输出不过载；
                                                  1: LDO9输出过载。 */
        unsigned char  ocp_ldo10 : 1;  /* bit[1]: 0：LDO10输出不过载；
                                                  1: LDO10输出过载。 */
        unsigned char  ocp_ldo11 : 1;  /* bit[2]: 0：LDO11输出不过载；
                                                  1: LDO11输出过载。 */
        unsigned char  ocp_ldo12 : 1;  /* bit[3]: 0：LDO12输出不过载；
                                                  1: LDO12输出过载。 */
        unsigned char  ocp_ldo13 : 1;  /* bit[4]: 0：LDO13输出不过载；
                                                  1: LDO13输出过载。 */
        unsigned char  ocp_ldo14 : 1;  /* bit[5]: 0：LDO14输出不过载；
                                                  1: LDO14输出过载。 */
        unsigned char  ocp_ldo15 : 1;  /* bit[6]: 0：LDO15输出不过载；
                                                  1: LDO15输出过载。 */
        unsigned char  ocp_ldo16 : 1;  /* bit[7]: 0：LDO16输出不过载；
                                                  1: LDO16输出过载。 */
    } reg;
} SOC_SMART_OCP_LDO16_9_RECORD3_UNION;
#endif
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo9_START   (0)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo9_END     (0)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo10_START  (1)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo10_END    (1)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo11_START  (2)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo11_END    (2)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo12_START  (3)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo12_END    (3)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo13_START  (4)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo13_END    (4)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo14_START  (5)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo14_END    (5)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo15_START  (6)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo15_END    (6)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo16_START  (7)
#define SOC_SMART_OCP_LDO16_9_RECORD3_ocp_ldo16_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_LDO22_17_RECORD4_UNION
 结构说明  : OCP_LDO22_17_RECORD4 寄存器结构定义。地址偏移量:0x00F，初值:0x00，宽度:8
 寄存器说明: 过流保护记录4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ldo17 : 1;  /* bit[0]  : 0：LDO17输出不过载；
                                                    1: LDO17输出过载。 */
        unsigned char  ocp_ldo18 : 1;  /* bit[1]  : 0：LDO18输出不过载；
                                                    1: LDO18输出过载。 */
        unsigned char  ocp_ldo19 : 1;  /* bit[2]  : 0：LDO19输出不过载；
                                                    1: LDO19输出过载。 */
        unsigned char  ocp_ldo20 : 1;  /* bit[3]  : 0：LDO20输出不过载；
                                                    1: LDO20输出过载。 */
        unsigned char  ocp_ldo21 : 1;  /* bit[4]  : 0：LDO21输出不过载；
                                                    1: LDO21输出过载。 */
        unsigned char  ocp_ldo22 : 1;  /* bit[5]  : 0：LDO22输出不过载；
                                                    1: LDO22输出过载。 */
        unsigned char  reserved  : 2;  /* bit[6-7]: 保留。 */
    } reg;
} SOC_SMART_OCP_LDO22_17_RECORD4_UNION;
#endif
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo17_START  (0)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo17_END    (0)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo18_START  (1)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo18_END    (1)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo19_START  (2)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo19_END    (2)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo20_START  (3)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo20_END    (3)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo21_START  (4)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo21_END    (4)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo22_START  (5)
#define SOC_SMART_OCP_LDO22_17_RECORD4_ocp_ldo22_END    (5)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_CLASSD_LVS2_RECORD5_UNION
 结构说明  : OCP_CLASSD_LVS2_RECORD5 寄存器结构定义。地址偏移量:0x010，初值:0x00，宽度:8
 寄存器说明: 过流保护记录5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_lvs2   : 1;  /* bit[0]  : 0：LVS2输出不过载；
                                                     1: LVS2输出过载。 */
        unsigned char  ocp_classd : 1;  /* bit[1]  : 0：CLASSD输出不过流；
                                                     1: CLASSD输出过流。 */
        unsigned char  scp_pmua   : 1;  /* bit[2]  : 0：PMUA输出不短路；
                                                     1: PMUA输出不短路； */
        unsigned char  reserved   : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_SMART_OCP_CLASSD_LVS2_RECORD5_UNION;
#endif
#define SOC_SMART_OCP_CLASSD_LVS2_RECORD5_ocp_lvs2_START    (0)
#define SOC_SMART_OCP_CLASSD_LVS2_RECORD5_ocp_lvs2_END      (0)
#define SOC_SMART_OCP_CLASSD_LVS2_RECORD5_ocp_classd_START  (1)
#define SOC_SMART_OCP_CLASSD_LVS2_RECORD5_ocp_classd_END    (1)
#define SOC_SMART_OCP_CLASSD_LVS2_RECORD5_scp_pmua_START    (2)
#define SOC_SMART_OCP_CLASSD_LVS2_RECORD5_scp_pmua_END      (2)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_DEB0_UNION
 结构说明  : OCP_DEB0 寄存器结构定义。地址偏移量:0x01A，初值:0x00，宽度:8
 寄存器说明: OCP去抖动调节0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_classd_ocp_deb : 1;  /* bit[0]  : 0：CLASSD OCP上升沿不去抖；
                                                            1：CLASSD OCP上升沿去抖。 */
        unsigned char  ocp_buck_deb_sel  : 2;  /* bit[1-2]: BUCK 的ocp去抖时间：
                                                            00：500us；
                                                            01：1ms；
                                                            10：2ms；
                                                            11：4ms。 */
        unsigned char  en_buck_ocp_deb   : 1;  /* bit[3]  : 0：BUCK OCP上升沿不去抖；
                                                            1：BUCK OCP上升沿去抖。 */
        unsigned char  scp_buck_deb_sel  : 2;  /* bit[4-5]: BUCK 的scp去抖时间：
                                                            00：500us；
                                                            01：1ms；
                                                            10：2ms；
                                                            11：4ms。 */
        unsigned char  en_buck_scp_deb   : 1;  /* bit[6]  : 0：BUCK SCP上升沿不去抖；
                                                            1：BUCK SCP上升沿去抖；
                                                            注：SCP表示短路保护。 */
        unsigned char  reserved          : 1;  /* bit[7]  : 保留。 */
    } reg;
} SOC_SMART_OCP_DEB0_UNION;
#endif
#define SOC_SMART_OCP_DEB0_en_classd_ocp_deb_START  (0)
#define SOC_SMART_OCP_DEB0_en_classd_ocp_deb_END    (0)
#define SOC_SMART_OCP_DEB0_ocp_buck_deb_sel_START   (1)
#define SOC_SMART_OCP_DEB0_ocp_buck_deb_sel_END     (2)
#define SOC_SMART_OCP_DEB0_en_buck_ocp_deb_START    (3)
#define SOC_SMART_OCP_DEB0_en_buck_ocp_deb_END      (3)
#define SOC_SMART_OCP_DEB0_scp_buck_deb_sel_START   (4)
#define SOC_SMART_OCP_DEB0_scp_buck_deb_sel_END     (5)
#define SOC_SMART_OCP_DEB0_en_buck_scp_deb_START    (6)
#define SOC_SMART_OCP_DEB0_en_buck_scp_deb_END      (6)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_DEB1_UNION
 结构说明  : OCP_DEB1 寄存器结构定义。地址偏移量:0x01B，初值:0x7D，宽度:8
 寄存器说明: OCP去抖动调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ldo_deb_sel    : 2;  /* bit[0-1]: ocp去抖时间：
                                                             00：500us；
                                                             01：1ms；
                                                             10：2ms；
                                                             11：4ms。 */
        unsigned char  en_ldo_ocp_deb     : 1;  /* bit[2]  : 0：LDO OCP上升沿不去抖；
                                                             1：LDO OCP上升沿去抖； */
        unsigned char  ocp_lvs_deb_sel    : 2;  /* bit[3-4]: LVS 的ocp去抖时间：
                                                             00：500us；
                                                             01：1ms；
                                                             10：2ms；
                                                             11：4ms。 */
        unsigned char  en_lvs_ocp_deb     : 1;  /* bit[5]  : 0：LVS OCP上升不去抖；
                                                             1：LVS OCP上升去抖。 */
        unsigned char  ocp_classd_deb_sel : 2;  /* bit[6-7]: CLASSD的ocp去抖时间：：
                                                             00：500us；
                                                             01：1ms；
                                                             10：2ms；
                                                             11：4ms。 */
    } reg;
} SOC_SMART_OCP_DEB1_UNION;
#endif
#define SOC_SMART_OCP_DEB1_ocp_ldo_deb_sel_START     (0)
#define SOC_SMART_OCP_DEB1_ocp_ldo_deb_sel_END       (1)
#define SOC_SMART_OCP_DEB1_en_ldo_ocp_deb_START      (2)
#define SOC_SMART_OCP_DEB1_en_ldo_ocp_deb_END        (2)
#define SOC_SMART_OCP_DEB1_ocp_lvs_deb_sel_START     (3)
#define SOC_SMART_OCP_DEB1_ocp_lvs_deb_sel_END       (4)
#define SOC_SMART_OCP_DEB1_en_lvs_ocp_deb_START      (5)
#define SOC_SMART_OCP_DEB1_en_lvs_ocp_deb_END        (5)
#define SOC_SMART_OCP_DEB1_ocp_classd_deb_sel_START  (6)
#define SOC_SMART_OCP_DEB1_ocp_classd_deb_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ONOFF8_UNION
 结构说明  : ONOFF8 寄存器结构定义。地址偏移量:0x01C，初值:0x9F，宽度:8
 寄存器说明: 开关控制8寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vbus_det_3p6_ctrl : 1;  /* bit[0]: 0：屏蔽vbus_det<3.6V检测；
                                                          1:不屏蔽vbus_det<3.6V检测。 */
        unsigned char  vsys_2p5_ctrl     : 1;  /* bit[1]: 0：屏蔽vsys< 2.5V检测；
                                                          1:不屏蔽vsys<2.5V检测。 */
        unsigned char  vsys_uv_ctrl      : 1;  /* bit[2]: 0：屏蔽vsys< 3.0V检测；
                                                          1:不屏蔽vsys<3.0V检测。 */
        unsigned char  vsys_6p0_ctrl     : 1;  /* bit[3]: 0：屏蔽vsys>6.0V检测；
                                                          1:不屏蔽vsys>6.0检测。 */
        unsigned char  otmp150_ctrl      : 1;  /* bit[4]: 0：高温（150℃）不关机；
                                                          1：高温(150℃)关机。 */
        unsigned char  en_32kc           : 1;  /* bit[5]: 0：关闭CLK_32C时钟输出；
                                                          1：CLK_32C 时钟输出使能。 */
        unsigned char  en_32kb           : 1;  /* bit[6]: 0：关闭CLK_32B 时钟输出；
                                                          1：CLK_32B 时钟输出使能。 */
        unsigned char  en_32ka           : 1;  /* bit[7]: 0：关闭CLK_32A 时钟输出；
                                                          1：CLK_32A 时钟输出使能。 */
    } reg;
} SOC_SMART_ONOFF8_UNION;
#endif
#define SOC_SMART_ONOFF8_vbus_det_3p6_ctrl_START  (0)
#define SOC_SMART_ONOFF8_vbus_det_3p6_ctrl_END    (0)
#define SOC_SMART_ONOFF8_vsys_2p5_ctrl_START      (1)
#define SOC_SMART_ONOFF8_vsys_2p5_ctrl_END        (1)
#define SOC_SMART_ONOFF8_vsys_uv_ctrl_START       (2)
#define SOC_SMART_ONOFF8_vsys_uv_ctrl_END         (2)
#define SOC_SMART_ONOFF8_vsys_6p0_ctrl_START      (3)
#define SOC_SMART_ONOFF8_vsys_6p0_ctrl_END        (3)
#define SOC_SMART_ONOFF8_otmp150_ctrl_START       (4)
#define SOC_SMART_ONOFF8_otmp150_ctrl_END         (4)
#define SOC_SMART_ONOFF8_en_32kc_START            (5)
#define SOC_SMART_ONOFF8_en_32kc_END              (5)
#define SOC_SMART_ONOFF8_en_32kb_START            (6)
#define SOC_SMART_ONOFF8_en_32kb_END              (6)
#define SOC_SMART_ONOFF8_en_32ka_START            (7)
#define SOC_SMART_ONOFF8_en_32ka_END              (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_SCP_MOD_CTRL1_UNION
 结构说明  : OCP_SCP_MOD_CTRL1 寄存器结构定义。地址偏移量:0x01D，初值:0xFF，宽度:8
 寄存器说明: BUCK1-4过流和短路保护模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_scp_ctrl_buck01 : 1;  /* bit[0]: ocp_scp_off_buck01配置为1时：
                                                            ocp_scp_ctrl_buck01:
                                                            0:工作在过流模式2；
                                                            1:工作在过流模式3； */
        unsigned char  ocp_scp_off_buck01  : 1;  /* bit[1]: 0：过流后或短路后不支持BUCK01模块自动关闭；
                                                            1：过流或短路后支持BUCK01模块自动关闭。 */
        unsigned char  ocp_scp_ctrl_buck2  : 1;  /* bit[2]: ocp_scp_off_buck2配置为1时：
                                                            ocp_scp_ctrl_buck2:
                                                            0:工作在过流模式2；
                                                            1:工作在过流模式3； */
        unsigned char  ocp_scp_off_buck2   : 1;  /* bit[3]: 0：过流或短路后不支持BUCK2模块自动关闭；
                                                            1：过流或短路后支持BUCK2模块自动关闭。 */
        unsigned char  ocp_scp_ctrl_buck3  : 1;  /* bit[4]: ocp_scp_off_buck3配置为1时：
                                                            ocp_scp_ctrl_buck3:
                                                            0:工作在过流模式2；
                                                            1:工作在过流模式3； */
        unsigned char  ocp_scp_off_buck3   : 1;  /* bit[5]: 0：过流或短路后不支持BUCK3模块自动关闭；
                                                            1：过流或短路后支持BUCK3模块自动关闭。 */
        unsigned char  ocp_scp_ctrl_buck4  : 1;  /* bit[6]: ocp_scp_off_buck4配置为1时：
                                                            ocp_scp_ctrl_buck4:
                                                            0:工作在过流模式2；
                                                            1:工作在过流模式3； */
        unsigned char  ocp_scp_off_buck4   : 1;  /* bit[7]: 0：过流或短路后不支持BUCK4模块自动关闭；
                                                            1：过流或短路后支持BUCK4模块自动关闭。 */
    } reg;
} SOC_SMART_OCP_SCP_MOD_CTRL1_UNION;
#endif
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_ctrl_buck01_START  (0)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_ctrl_buck01_END    (0)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_off_buck01_START   (1)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_off_buck01_END     (1)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_ctrl_buck2_START   (2)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_ctrl_buck2_END     (2)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_off_buck2_START    (3)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_off_buck2_END      (3)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_ctrl_buck3_START   (4)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_ctrl_buck3_END     (4)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_off_buck3_START    (5)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_off_buck3_END      (5)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_ctrl_buck4_START   (6)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_ctrl_buck4_END     (6)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_off_buck4_START    (7)
#define SOC_SMART_OCP_SCP_MOD_CTRL1_ocp_scp_off_buck4_END      (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_MOD_CTRL3_UNION
 结构说明  : OCP_MOD_CTRL3 寄存器结构定义。地址偏移量:0x01E，初值:0xFB，宽度:8
 寄存器说明: LDO1-4过流保护模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ctrl_ldo1 : 1;  /* bit[0]: ocp_off_ldo1配置为1时：
                                                      ocp_ctrl_ldo1:
                                                      0：过流模式为模式2；
                                                      1：过流模式为模式3。 */
        unsigned char  ocp_off_ldo1  : 1;  /* bit[1]: 0：过流后不支持LDO1模块自动关闭；
                                                      1：过流后支持LDO1模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo2 : 1;  /* bit[2]: ocp_off_ldo2配置为1时：
                                                      ocp_ctrl_ldo2:
                                                      0：过流模式为模式2；
                                                      1：过流模式为模式3。 */
        unsigned char  ocp_off_ldo2  : 1;  /* bit[3]: 0：过流后不支持LDO2模块自动关闭；
                                                      1：过流后支持LDO2模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo3 : 1;  /* bit[4]: ocp_off_ldo3配置为1时：
                                                      ocp_ctrl_ldo3:
                                                      0：过流模式为模式2；
                                                      1：过流模式为模式3。 */
        unsigned char  ocp_off_ldo3  : 1;  /* bit[5]: 0：过流后不支持LDO3模块自动关闭；
                                                      1：过流后支持LDO3模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo4 : 1;  /* bit[6]: ocp_off_ldo4配置为1时：
                                                      ocp_ctrl_ldo4:
                                                      0:工作在过流模式2；
                                                      1:工作在过流模式3； */
        unsigned char  ocp_off_ldo4  : 1;  /* bit[7]: 0：过流后不支持LDO4模块自动关闭；
                                                      1：过流后支持LDO4模块自动关闭。 */
    } reg;
} SOC_SMART_OCP_MOD_CTRL3_UNION;
#endif
#define SOC_SMART_OCP_MOD_CTRL3_ocp_ctrl_ldo1_START  (0)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_ctrl_ldo1_END    (0)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_off_ldo1_START   (1)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_off_ldo1_END     (1)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_ctrl_ldo2_START  (2)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_ctrl_ldo2_END    (2)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_off_ldo2_START   (3)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_off_ldo2_END     (3)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_ctrl_ldo3_START  (4)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_ctrl_ldo3_END    (4)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_off_ldo3_START   (5)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_off_ldo3_END     (5)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_ctrl_ldo4_START  (6)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_ctrl_ldo4_END    (6)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_off_ldo4_START   (7)
#define SOC_SMART_OCP_MOD_CTRL3_ocp_off_ldo4_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_MOD_CTRL4_UNION
 结构说明  : OCP_MOD_CTRL4 寄存器结构定义。地址偏移量:0x01F，初值:0xEF，宽度:8
 寄存器说明: LDO5-8过流保护模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ctrl_ldo5 : 1;  /* bit[0]: ocp_off_ldo5配置为1时：
                                                      ocp_ctrl_ldo5:
                                                      0:工作在过流模式2；
                                                      1:工作在过流模式3； */
        unsigned char  ocp_off_ldo5  : 1;  /* bit[1]: 0：过流后不支持LDO5模块自动关闭；
                                                      1：过流后支持LDO5模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo6 : 1;  /* bit[2]: ocp_off_ldo6配置为1时：
                                                      ocp_ctrl_ldo6:
                                                      0:工作在过流模式2；
                                                      1:工作在过流模式3； */
        unsigned char  ocp_off_ldo6  : 1;  /* bit[3]: 0：过流后不支持LDO6模块自动关闭；
                                                      1：过流后支持LDO6模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo7 : 1;  /* bit[4]: ocp_off_ldo7配置为1时：
                                                      ocp_ctrl_ldo7:
                                                      0:工作在过流模式2；
                                                      1:工作在过流模式3； */
        unsigned char  ocp_off_ldo7  : 1;  /* bit[5]: 0：过流后不支持LDO7模块自动关闭；
                                                      1：过流后支持LDO7模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo8 : 1;  /* bit[6]: ocp_off_ldo8配置为1时：
                                                      ocp_ctrl_ldo8:
                                                      0:工作在过流模式2；
                                                      1:工作在过流模式3； */
        unsigned char  ocp_off_ldo8  : 1;  /* bit[7]: 0：过流后不支持LDO8模块自动关闭；
                                                      1：过流后支持LDO8模块自动关闭。 */
    } reg;
} SOC_SMART_OCP_MOD_CTRL4_UNION;
#endif
#define SOC_SMART_OCP_MOD_CTRL4_ocp_ctrl_ldo5_START  (0)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_ctrl_ldo5_END    (0)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_off_ldo5_START   (1)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_off_ldo5_END     (1)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_ctrl_ldo6_START  (2)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_ctrl_ldo6_END    (2)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_off_ldo6_START   (3)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_off_ldo6_END     (3)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_ctrl_ldo7_START  (4)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_ctrl_ldo7_END    (4)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_off_ldo7_START   (5)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_off_ldo7_END     (5)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_ctrl_ldo8_START  (6)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_ctrl_ldo8_END    (6)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_off_ldo8_START   (7)
#define SOC_SMART_OCP_MOD_CTRL4_ocp_off_ldo8_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_MOD_CTRL5_UNION
 结构说明  : OCP_MOD_CTRL5 寄存器结构定义。地址偏移量:0x020，初值:0xEA，宽度:8
 寄存器说明: LDO9-12过流保护模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ctrl_ldo9  : 1;  /* bit[0]: ocp_off_ldo9配置为1时：
                                                       ocp_ctrl_ldo9:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo9   : 1;  /* bit[1]: 0：过流后不支持LDO9模块自动关闭；
                                                       1：过流后支持LDO9模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo10 : 1;  /* bit[2]: ocp_off_ldo10配置为1时：
                                                       ocp_ctrl_ldo10:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo10  : 1;  /* bit[3]: 0：过流后不支持LDO10模块自动关闭；
                                                       1：过流后支持LDO10模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo11 : 1;  /* bit[4]: ocp_off_ldo11配置为1时：
                                                       ocp_ctrl_ldo11:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo11  : 1;  /* bit[5]: 0：过流后不支持LDO11模块自动关闭；
                                                       1：过流后支持LDO11模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo12 : 1;  /* bit[6]: ocp_off_ldo12配置为1时：
                                                       ocp_ctrl_ldo12:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo12  : 1;  /* bit[7]: 0：过流后不支持LDO12模块自动关闭；
                                                       1：过流后支持LDO12模块自动关闭。 */
    } reg;
} SOC_SMART_OCP_MOD_CTRL5_UNION;
#endif
#define SOC_SMART_OCP_MOD_CTRL5_ocp_ctrl_ldo9_START   (0)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_ctrl_ldo9_END     (0)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_off_ldo9_START    (1)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_off_ldo9_END      (1)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_ctrl_ldo10_START  (2)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_ctrl_ldo10_END    (2)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_off_ldo10_START   (3)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_off_ldo10_END     (3)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_ctrl_ldo11_START  (4)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_ctrl_ldo11_END    (4)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_off_ldo11_START   (5)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_off_ldo11_END     (5)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_ctrl_ldo12_START  (6)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_ctrl_ldo12_END    (6)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_off_ldo12_START   (7)
#define SOC_SMART_OCP_MOD_CTRL5_ocp_off_ldo12_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_MOD_CTRL6_UNION
 结构说明  : OCP_MOD_CTRL6 寄存器结构定义。地址偏移量:0x021，初值:0xAA，宽度:8
 寄存器说明: LDO13-16过流保护模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ctrl_ldo13 : 1;  /* bit[0]: ocp_off_ldo13配置为1时：
                                                       ocp_ctrl_ldo13:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo13  : 1;  /* bit[1]: 0：过流后不支持LDO13模块自动关闭；
                                                       1：过流后支持LDO13模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo14 : 1;  /* bit[2]: ocp_off_ldo14配置为1时：
                                                       ocp_ctrl_ldo14:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo14  : 1;  /* bit[3]: 0：过流后不支持LDO14模块自动关闭；
                                                       1：过流后支持LDO14模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo15 : 1;  /* bit[4]: ocp_off_ldo15配置为1时：
                                                       ocp_ctrl_ldo15:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo15  : 1;  /* bit[5]: 0：过流后不支持LDO15模块自动关闭；
                                                       1：过流后支持LDO15模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo16 : 1;  /* bit[6]: ocp_off_ldo16配置为1时：
                                                       ocp_ctrl_ldo16:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo16  : 1;  /* bit[7]: 0：过流后不支持LDO16模块自动关闭；
                                                       1：过流后支持LDO16模块自动关闭。 */
    } reg;
} SOC_SMART_OCP_MOD_CTRL6_UNION;
#endif
#define SOC_SMART_OCP_MOD_CTRL6_ocp_ctrl_ldo13_START  (0)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_ctrl_ldo13_END    (0)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_off_ldo13_START   (1)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_off_ldo13_END     (1)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_ctrl_ldo14_START  (2)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_ctrl_ldo14_END    (2)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_off_ldo14_START   (3)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_off_ldo14_END     (3)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_ctrl_ldo15_START  (4)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_ctrl_ldo15_END    (4)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_off_ldo15_START   (5)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_off_ldo15_END     (5)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_ctrl_ldo16_START  (6)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_ctrl_ldo16_END    (6)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_off_ldo16_START   (7)
#define SOC_SMART_OCP_MOD_CTRL6_ocp_off_ldo16_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_MOD_CTRL7_UNION
 结构说明  : OCP_MOD_CTRL7 寄存器结构定义。地址偏移量:0x022，初值:0xBE，宽度:8
 寄存器说明: LDO17-20过流保护模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ctrl_ldo17 : 1;  /* bit[0]: ocp_off_ldo17配置为1时：
                                                       ocp_ctrl_ldo17:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo17  : 1;  /* bit[1]: 0：过流后不支持LDO17模块自动关闭；
                                                       1：过流后支持LDO17模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo18 : 1;  /* bit[2]: ocp_off_ldo18配置为1时：
                                                       ocp_ctrl_ldo18:
                                                       0:LDO1和LDO18工作在过流模式2；
                                                       1:LDO1和LDO18工作在过流模式3； */
        unsigned char  ocp_off_ldo18  : 1;  /* bit[3]: 0：过流后不支持LDO1和LDO18模块自动关闭；
                                                       1：过流后支持LDO1和LDO18模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo19 : 1;  /* bit[4]: ocp_off_ldo19配置为1时：
                                                       ocp_ctrl_ldo19:
                                                       0:工作在过流模式2；
                                                       1:工作在过流模式3； */
        unsigned char  ocp_off_ldo19  : 1;  /* bit[5]: 0：过流后不支持LDO19模块自动关闭；
                                                       1：过流后支持LDO19模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo20 : 1;  /* bit[6]: ocp_off_ldo20配置为1时：
                                                       ocp_ctrl_ldo20:
                                                       0：过流模式为模式2；
                                                       1：过流模式为模式3。 */
        unsigned char  ocp_off_ldo20  : 1;  /* bit[7]: 0：过流后不支持LDO20模块自动关闭；
                                                       1：过流后支持LDO20模块自动关闭。 */
    } reg;
} SOC_SMART_OCP_MOD_CTRL7_UNION;
#endif
#define SOC_SMART_OCP_MOD_CTRL7_ocp_ctrl_ldo17_START  (0)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_ctrl_ldo17_END    (0)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_off_ldo17_START   (1)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_off_ldo17_END     (1)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_ctrl_ldo18_START  (2)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_ctrl_ldo18_END    (2)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_off_ldo18_START   (3)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_off_ldo18_END     (3)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_ctrl_ldo19_START  (4)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_ctrl_ldo19_END    (4)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_off_ldo19_START   (5)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_off_ldo19_END     (5)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_ctrl_ldo20_START  (6)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_ctrl_ldo20_END    (6)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_off_ldo20_START   (7)
#define SOC_SMART_OCP_MOD_CTRL7_ocp_off_ldo20_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_MOD_CTRL8_UNION
 结构说明  : OCP_MOD_CTRL8 寄存器结构定义。地址偏移量:0x023，初值:0x0A，宽度:8
 寄存器说明: LDO21-ldo23过流保护模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ctrl_ldo21 : 1;  /* bit[0]  : ocp_off_ldo21配置为1时：
                                                         ocp_ctrl_ldo21:
                                                         0：过流模式为模式2；
                                                         1：过流模式为模式3。 */
        unsigned char  ocp_off_ldo21  : 1;  /* bit[1]  : 0：过流后不支持LDO21模块自动关闭；
                                                         1：过流后支持LDO21模块自动关闭。 */
        unsigned char  ocp_ctrl_ldo22 : 1;  /* bit[2]  : ocp_off_ldo22配置为1时：
                                                         ocp_ctrl_ldo22:
                                                         0:工作在过流模式2；
                                                         1:工作在过流模式3； */
        unsigned char  ocp_off_ldo22  : 1;  /* bit[3]  : 0：过流后不支持LDO22模块自动关闭；
                                                         1：过流后支持LDO22模块自动关闭。 */
        unsigned char  reserved       : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_SMART_OCP_MOD_CTRL8_UNION;
#endif
#define SOC_SMART_OCP_MOD_CTRL8_ocp_ctrl_ldo21_START  (0)
#define SOC_SMART_OCP_MOD_CTRL8_ocp_ctrl_ldo21_END    (0)
#define SOC_SMART_OCP_MOD_CTRL8_ocp_off_ldo21_START   (1)
#define SOC_SMART_OCP_MOD_CTRL8_ocp_off_ldo21_END     (1)
#define SOC_SMART_OCP_MOD_CTRL8_ocp_ctrl_ldo22_START  (2)
#define SOC_SMART_OCP_MOD_CTRL8_ocp_ctrl_ldo22_END    (2)
#define SOC_SMART_OCP_MOD_CTRL8_ocp_off_ldo22_START   (3)
#define SOC_SMART_OCP_MOD_CTRL8_ocp_off_ldo22_END     (3)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_MOD_CTRL9_UNION
 结构说明  : OCP_MOD_CTRL9 寄存器结构定义。地址偏移量:0x024，初值:0x02，宽度:8
 寄存器说明: LVS2过流保护模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ctrl_lvs2 : 1;  /* bit[0]  : ocp_off_lvs2配置为1时：
                                                        ocp_ctrl_lvs2:
                                                        0：过流模式为模式2；
                                                        1：过流模式为模式3。 */
        unsigned char  ocp_off_lvs2  : 1;  /* bit[1]  : 0：过流后不支持LVS2模块自动关闭；
                                                        1：过流后支持LVS2模块自动关闭。 */
        unsigned char  reserved      : 6;  /* bit[2-7]: 保留。 */
    } reg;
} SOC_SMART_OCP_MOD_CTRL9_UNION;
#endif
#define SOC_SMART_OCP_MOD_CTRL9_ocp_ctrl_lvs2_START  (0)
#define SOC_SMART_OCP_MOD_CTRL9_ocp_ctrl_lvs2_END    (0)
#define SOC_SMART_OCP_MOD_CTRL9_ocp_off_lvs2_START   (1)
#define SOC_SMART_OCP_MOD_CTRL9_ocp_off_lvs2_END     (1)


/*****************************************************************************
 结构名    : SOC_SMART_OCP_MOD_CTRL11_UNION
 结构说明  : OCP_MOD_CTRL11 寄存器结构定义。地址偏移量:0x025，初值:0x02，宽度:8
 寄存器说明: CLASSD过流保护模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ocp_ctrl_classd : 1;  /* bit[0]  : ocp_off_classd配置为1时：
                                                          ocp_ctrl_classd:
                                                          0：保护模式为模式2；
                                                          1：保护模式为模式3。 */
        unsigned char  ocp_off_classd  : 1;  /* bit[1]  : 0：过流后不支持CLASSD模块自动关闭；
                                                          1：过流后支持CLASSD模块自动关闭。 */
        unsigned char  reserved        : 6;  /* bit[2-7]: 保留。 */
    } reg;
} SOC_SMART_OCP_MOD_CTRL11_UNION;
#endif
#define SOC_SMART_OCP_MOD_CTRL11_ocp_ctrl_classd_START  (0)
#define SOC_SMART_OCP_MOD_CTRL11_ocp_ctrl_classd_END    (0)
#define SOC_SMART_OCP_MOD_CTRL11_ocp_off_classd_START   (1)
#define SOC_SMART_OCP_MOD_CTRL11_ocp_off_classd_END     (1)


/*****************************************************************************
 结构名    : SOC_SMART_ENABLE1_BUCK_UNION
 结构说明  : ENABLE1_BUCK 寄存器结构定义。地址偏移量:0x026，初值:0x00，宽度:8
 寄存器说明: 电源模块使能1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_buck01_int  : 1;  /* bit[0]  : BUCK01开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_buck2_int   : 1;  /* bit[1]  : BUCK2开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_buck3_int   : 1;  /* bit[2]  : BUCK3开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_buck4_int   : 1;  /* bit[3]  : BUCK4开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_extbuck_int : 1;  /* bit[4]  : EXTBUCK开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  reserved       : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_ENABLE1_BUCK_UNION;
#endif
#define SOC_SMART_ENABLE1_BUCK_en_buck01_int_START   (0)
#define SOC_SMART_ENABLE1_BUCK_en_buck01_int_END     (0)
#define SOC_SMART_ENABLE1_BUCK_en_buck2_int_START    (1)
#define SOC_SMART_ENABLE1_BUCK_en_buck2_int_END      (1)
#define SOC_SMART_ENABLE1_BUCK_en_buck3_int_START    (2)
#define SOC_SMART_ENABLE1_BUCK_en_buck3_int_END      (2)
#define SOC_SMART_ENABLE1_BUCK_en_buck4_int_START    (3)
#define SOC_SMART_ENABLE1_BUCK_en_buck4_int_END      (3)
#define SOC_SMART_ENABLE1_BUCK_en_extbuck_int_START  (4)
#define SOC_SMART_ENABLE1_BUCK_en_extbuck_int_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_DISABLE1_BUCK_UNION
 结构说明  : DISABLE1_BUCK 寄存器结构定义。地址偏移量:0x027，初值:0x00，宽度:8
 寄存器说明: 电源模块关闭1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dis_buck01_int  : 1;  /* bit[0]  : BUCK01开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_buck2_int   : 1;  /* bit[1]  : BUCK2开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_buck3_int   : 1;  /* bit[2]  : BUCK3开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_buck4_int   : 1;  /* bit[3]  : BUCK4开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_extbuck_int : 1;  /* bit[4]  : EXTBUCK开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  reserved        : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_DISABLE1_BUCK_UNION;
#endif
#define SOC_SMART_DISABLE1_BUCK_dis_buck01_int_START   (0)
#define SOC_SMART_DISABLE1_BUCK_dis_buck01_int_END     (0)
#define SOC_SMART_DISABLE1_BUCK_dis_buck2_int_START    (1)
#define SOC_SMART_DISABLE1_BUCK_dis_buck2_int_END      (1)
#define SOC_SMART_DISABLE1_BUCK_dis_buck3_int_START    (2)
#define SOC_SMART_DISABLE1_BUCK_dis_buck3_int_END      (2)
#define SOC_SMART_DISABLE1_BUCK_dis_buck4_int_START    (3)
#define SOC_SMART_DISABLE1_BUCK_dis_buck4_int_END      (3)
#define SOC_SMART_DISABLE1_BUCK_dis_extbuck_int_START  (4)
#define SOC_SMART_DISABLE1_BUCK_dis_extbuck_int_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_ONOFF_STATUS1_BUCK_UNION
 结构说明  : ONOFF_STATUS1_BUCK 寄存器结构定义。地址偏移量:0x028，初值:0x1F，宽度:8
 寄存器说明: 电源模块开关状态1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  st_buck01_int  : 1;  /* bit[0]  : BUCK01开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_buck2_int   : 1;  /* bit[1]  : BUCK2开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_buck3_int   : 1;  /* bit[2]  : BUCK3开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_buck4_int   : 1;  /* bit[3]  : BUCK4开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_extbuck_int : 1;  /* bit[4]  : EXTBUCK开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  reserved       : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_ONOFF_STATUS1_BUCK_UNION;
#endif
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_buck01_int_START   (0)
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_buck01_int_END     (0)
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_buck2_int_START    (1)
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_buck2_int_END      (1)
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_buck3_int_START    (2)
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_buck3_int_END      (2)
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_buck4_int_START    (3)
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_buck4_int_END      (3)
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_extbuck_int_START  (4)
#define SOC_SMART_ONOFF_STATUS1_BUCK_st_extbuck_int_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_ENABLE2_LDO1_8_UNION
 结构说明  : ENABLE2_LDO1_8 寄存器结构定义。地址偏移量:0x029，初值:0x00，宽度:8
 寄存器说明: 电源模块使能2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_ldo1_int      : 1;  /* bit[0]: 功能作废 */
        unsigned char  en_ldo2_int      : 1;  /* bit[1]: LDO2开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo3_buck_int : 1;  /* bit[2]: BUCK下LDO3使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo4_int      : 1;  /* bit[3]: LDO4开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo5_int      : 1;  /* bit[4]: LDO5开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo6_int      : 1;  /* bit[5]: LDO6开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo7_int      : 1;  /* bit[6]: LDO7开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo8_int      : 1;  /* bit[7]: LDO8开关使能：
                                                         写1开启，自动清零。 */
    } reg;
} SOC_SMART_ENABLE2_LDO1_8_UNION;
#endif
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo1_int_START       (0)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo1_int_END         (0)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo2_int_START       (1)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo2_int_END         (1)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo3_buck_int_START  (2)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo3_buck_int_END    (2)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo4_int_START       (3)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo4_int_END         (3)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo5_int_START       (4)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo5_int_END         (4)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo6_int_START       (5)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo6_int_END         (5)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo7_int_START       (6)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo7_int_END         (6)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo8_int_START       (7)
#define SOC_SMART_ENABLE2_LDO1_8_en_ldo8_int_END         (7)


/*****************************************************************************
 结构名    : SOC_SMART_DISABLE2_LDO1_8_UNION
 结构说明  : DISABLE2_LDO1_8 寄存器结构定义。地址偏移量:0x02A，初值:0x00，宽度:8
 寄存器说明: 电源模块关闭2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dis_ldo1_int      : 1;  /* bit[0]: 功能作废 */
        unsigned char  dis_ldo2_int      : 1;  /* bit[1]: LDO2开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo3_buck_int : 1;  /* bit[2]: BUCK下LDO3禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo4_int      : 1;  /* bit[3]: LDO4开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo5_int      : 1;  /* bit[4]: LDO5开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo6_int      : 1;  /* bit[5]: LDO6开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo7_int      : 1;  /* bit[6]: LDO7开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo8_int      : 1;  /* bit[7]: LDO8开关禁止：
                                                          写1关闭，自动清零。 */
    } reg;
} SOC_SMART_DISABLE2_LDO1_8_UNION;
#endif
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo1_int_START       (0)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo1_int_END         (0)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo2_int_START       (1)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo2_int_END         (1)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo3_buck_int_START  (2)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo3_buck_int_END    (2)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo4_int_START       (3)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo4_int_END         (3)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo5_int_START       (4)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo5_int_END         (4)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo6_int_START       (5)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo6_int_END         (5)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo7_int_START       (6)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo7_int_END         (6)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo8_int_START       (7)
#define SOC_SMART_DISABLE2_LDO1_8_dis_ldo8_int_END         (7)


/*****************************************************************************
 结构名    : SOC_SMART_ONOFF_STATUS2_LDO1_8_UNION
 结构说明  : ONOFF_STATUS2_LDO1_8 寄存器结构定义。地址偏移量:0x02B，初值:0x94，宽度:8
 寄存器说明: 电源模块开关状态2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  st_ldo1_int      : 1;  /* bit[0]: 功能作废 */
        unsigned char  st_ldo2_int      : 1;  /* bit[1]: LDO2开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo3_buck_int : 1;  /* bit[2]: BUCK下LDO3时能开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo4_int      : 1;  /* bit[3]: LDO4开关状态(仅支持寄存器方式开启)：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo5_int      : 1;  /* bit[4]: LDO5开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo6_int      : 1;  /* bit[5]: LDO6开关状态(仅支持寄存器方式开启)：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo7_int      : 1;  /* bit[6]: LDO7开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo8_int      : 1;  /* bit[7]: LDO8开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
    } reg;
} SOC_SMART_ONOFF_STATUS2_LDO1_8_UNION;
#endif
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo1_int_START       (0)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo1_int_END         (0)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo2_int_START       (1)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo2_int_END         (1)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo3_buck_int_START  (2)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo3_buck_int_END    (2)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo4_int_START       (3)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo4_int_END         (3)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo5_int_START       (4)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo5_int_END         (4)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo6_int_START       (5)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo6_int_END         (5)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo7_int_START       (6)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo7_int_END         (6)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo8_int_START       (7)
#define SOC_SMART_ONOFF_STATUS2_LDO1_8_st_ldo8_int_END         (7)


/*****************************************************************************
 结构名    : SOC_SMART_ENABLE3_LDO9_16_UNION
 结构说明  : ENABLE3_LDO9_16 寄存器结构定义。地址偏移量:0x02C，初值:0x00，宽度:8
 寄存器说明: 电源模块使能3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_ldo9_int  : 1;  /* bit[0]: LDO9开关使能：
                                                     写1开启，自动清零。 */
        unsigned char  en_ldo10_int : 1;  /* bit[1]: LDO10开关使能：
                                                     写1开启，自动清零。 */
        unsigned char  en_ldo11_int : 1;  /* bit[2]: LDO11开关使能：
                                                     写1开启，自动清零。 */
        unsigned char  en_ldo12_int : 1;  /* bit[3]: LDO12开关使能：
                                                     写1开启，自动清零。 */
        unsigned char  en_ldo13_int : 1;  /* bit[4]: LDO13开关使能：
                                                     写1开启，自动清零。 */
        unsigned char  en_ldo14_int : 1;  /* bit[5]: LDO14开关使能：
                                                     写1开启，自动清零。 */
        unsigned char  en_ldo15_int : 1;  /* bit[6]: LDO15开关使能：
                                                     写1开启，自动清零。 */
        unsigned char  en_ldo16_int : 1;  /* bit[7]: LDO16开关使能：
                                                     写1开启，自动清零。 */
    } reg;
} SOC_SMART_ENABLE3_LDO9_16_UNION;
#endif
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo9_int_START   (0)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo9_int_END     (0)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo10_int_START  (1)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo10_int_END    (1)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo11_int_START  (2)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo11_int_END    (2)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo12_int_START  (3)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo12_int_END    (3)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo13_int_START  (4)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo13_int_END    (4)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo14_int_START  (5)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo14_int_END    (5)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo15_int_START  (6)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo15_int_END    (6)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo16_int_START  (7)
#define SOC_SMART_ENABLE3_LDO9_16_en_ldo16_int_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_DISABLE3_LDO9_16_UNION
 结构说明  : DISABLE3_LDO9_16 寄存器结构定义。地址偏移量:0x02D，初值:0x00，宽度:8
 寄存器说明: 电源模块关闭3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dis_ldo9_int  : 1;  /* bit[0]: LDO9开关禁止：
                                                      写1关闭，自动清零。 */
        unsigned char  dis_ldo10_int : 1;  /* bit[1]: LDO10开关禁止：
                                                      写1关闭，自动清零。 */
        unsigned char  dis_ldo11_int : 1;  /* bit[2]: LDO11开关禁止：
                                                      写1关闭，自动清零。 */
        unsigned char  dis_ldo12_int : 1;  /* bit[3]: LDO12开关禁止：
                                                      写1关闭，自动清零。 */
        unsigned char  dis_ldo13_int : 1;  /* bit[4]: LDO13开关禁止：
                                                      写1关闭，自动清零。 */
        unsigned char  dis_ldo14_int : 1;  /* bit[5]: LDO14开关禁止：
                                                      写1关闭，自动清零。 */
        unsigned char  dis_ldo15_int : 1;  /* bit[6]: LDO15开关禁止：
                                                      写1关闭，自动清零。 */
        unsigned char  dis_ldo16_int : 1;  /* bit[7]: LDO16开关禁止：
                                                      写1关闭，自动清零。 */
    } reg;
} SOC_SMART_DISABLE3_LDO9_16_UNION;
#endif
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo9_int_START   (0)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo9_int_END     (0)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo10_int_START  (1)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo10_int_END    (1)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo11_int_START  (2)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo11_int_END    (2)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo12_int_START  (3)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo12_int_END    (3)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo13_int_START  (4)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo13_int_END    (4)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo14_int_START  (5)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo14_int_END    (5)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo15_int_START  (6)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo15_int_END    (6)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo16_int_START  (7)
#define SOC_SMART_DISABLE3_LDO9_16_dis_ldo16_int_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ONOFF_STATUS3_LDO9_16_UNION
 结构说明  : ONOFF_STATUS3_LDO9_16 寄存器结构定义。地址偏移量:0x02E，初值:0x08，宽度:8
 寄存器说明: 电源模块开关状态3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  st_ldo9_int  : 1;  /* bit[0]: LDO9开关状态：
                                                     0：通过寄存器关闭；
                                                     1：通过寄存器开启。 */
        unsigned char  st_ldo10_int : 1;  /* bit[1]: LDO10开关状态：
                                                     0：通过寄存器关闭；
                                                     1：通过寄存器开启。 */
        unsigned char  st_ldo11_int : 1;  /* bit[2]: LDO11开关状态：
                                                     0：通过寄存器关闭；
                                                     1：通过寄存器开启。 */
        unsigned char  st_ldo12_int : 1;  /* bit[3]: LDO12开关状态：
                                                     0：通过寄存器关闭；
                                                     1：通过寄存器开启。 */
        unsigned char  st_ldo13_int : 1;  /* bit[4]: LDO13开关状态：
                                                     0：通过寄存器关闭；
                                                     1：通过寄存器开启。 */
        unsigned char  st_ldo14_int : 1;  /* bit[5]: LDO14开关状态：
                                                     0：通过寄存器关闭；
                                                     1：通过寄存器开启。 */
        unsigned char  st_ldo15_int : 1;  /* bit[6]: LDO15开关状态：
                                                     0：通过寄存器关闭；
                                                     1：通过寄存器开启。 */
        unsigned char  st_ldo16_int : 1;  /* bit[7]: LDO16开关状态：
                                                     0：通过寄存器关闭；
                                                     1：通过寄存器开启。 */
    } reg;
} SOC_SMART_ONOFF_STATUS3_LDO9_16_UNION;
#endif
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo9_int_START   (0)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo9_int_END     (0)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo10_int_START  (1)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo10_int_END    (1)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo11_int_START  (2)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo11_int_END    (2)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo12_int_START  (3)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo12_int_END    (3)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo13_int_START  (4)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo13_int_END    (4)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo14_int_START  (5)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo14_int_END    (5)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo15_int_START  (6)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo15_int_END    (6)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo16_int_START  (7)
#define SOC_SMART_ONOFF_STATUS3_LDO9_16_st_ldo16_int_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ENABLE4_LDO17_22_UNION
 结构说明  : ENABLE4_LDO17_22 寄存器结构定义。地址偏移量:0x02F，初值:0x00，宽度:8
 寄存器说明: 电源模块使能4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_ldo17_int     : 1;  /* bit[0]: LDO17开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo18_int     : 1;  /* bit[1]: LDO1和LDO18开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo19_int     : 1;  /* bit[2]: LDO19开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo20_int     : 1;  /* bit[3]: LDO20开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo21_int     : 1;  /* bit[4]: LDO21开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo22_int     : 1;  /* bit[5]: LDO22开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_ldo3_batt_int : 1;  /* bit[6]: BATT下LDO3使能：
                                                         写1开启，自动清零。 */
        unsigned char  reserved         : 1;  /* bit[7]: 保留。 */
    } reg;
} SOC_SMART_ENABLE4_LDO17_22_UNION;
#endif
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo17_int_START      (0)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo17_int_END        (0)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo18_int_START      (1)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo18_int_END        (1)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo19_int_START      (2)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo19_int_END        (2)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo20_int_START      (3)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo20_int_END        (3)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo21_int_START      (4)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo21_int_END        (4)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo22_int_START      (5)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo22_int_END        (5)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo3_batt_int_START  (6)
#define SOC_SMART_ENABLE4_LDO17_22_en_ldo3_batt_int_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_DISABLE4_LDO17_22_UNION
 结构说明  : DISABLE4_LDO17_22 寄存器结构定义。地址偏移量:0x030，初值:0x00，宽度:8
 寄存器说明: 电源模块关闭4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dis_ldo17_int     : 1;  /* bit[0]: LDO17开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo18_int     : 1;  /* bit[1]: LDO1和LDO18开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo19_int     : 1;  /* bit[2]: LDO19开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo20_int     : 1;  /* bit[3]: LDO20开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo21_int     : 1;  /* bit[4]: LDO21开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo22_int     : 1;  /* bit[5]: LDO22开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_ldo3_batt_int : 1;  /* bit[6]: BATT下LDO3禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  reserved          : 1;  /* bit[7]: 保留。 */
    } reg;
} SOC_SMART_DISABLE4_LDO17_22_UNION;
#endif
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo17_int_START      (0)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo17_int_END        (0)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo18_int_START      (1)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo18_int_END        (1)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo19_int_START      (2)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo19_int_END        (2)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo20_int_START      (3)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo20_int_END        (3)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo21_int_START      (4)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo21_int_END        (4)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo22_int_START      (5)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo22_int_END        (5)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo3_batt_int_START  (6)
#define SOC_SMART_DISABLE4_LDO17_22_dis_ldo3_batt_int_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_ONOFF_STATUS4_LDO17_22_UNION
 结构说明  : ONOFF_STATUS4_LDO17_22 寄存器结构定义。地址偏移量:0x031，初值:0x06，宽度:8
 寄存器说明: 电源模块开关状态4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  st_ldo17_int     : 1;  /* bit[0]: LDO17开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo18_int     : 1;  /* bit[1]: LDO1和LDO18开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo19_int     : 1;  /* bit[2]: LDO19开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo20_int     : 1;  /* bit[3]: LDO20开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo21_int     : 1;  /* bit[4]: LDO21开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo22_int     : 1;  /* bit[5]: LDO22开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_ldo3_batt_int : 1;  /* bit[6]: BATT下LDO3开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  reserved         : 1;  /* bit[7]: 保留。 */
    } reg;
} SOC_SMART_ONOFF_STATUS4_LDO17_22_UNION;
#endif
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo17_int_START      (0)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo17_int_END        (0)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo18_int_START      (1)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo18_int_END        (1)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo19_int_START      (2)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo19_int_END        (2)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo20_int_START      (3)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo20_int_END        (3)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo21_int_START      (4)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo21_int_END        (4)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo22_int_START      (5)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo22_int_END        (5)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo3_batt_int_START  (6)
#define SOC_SMART_ONOFF_STATUS4_LDO17_22_st_ldo3_batt_int_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_ENABLE5_LVS2_UNION
 结构说明  : ENABLE5_LVS2 寄存器结构定义。地址偏移量:0x032，初值:0x00，宽度:8
 寄存器说明: 电源模块使能5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_lvs2_int : 1;  /* bit[0]  : LVS2开关使能：
                                                      写1开启，自动清零。 */
        unsigned char  reserved    : 7;  /* bit[1-7]: 保留。 */
    } reg;
} SOC_SMART_ENABLE5_LVS2_UNION;
#endif
#define SOC_SMART_ENABLE5_LVS2_en_lvs2_int_START  (0)
#define SOC_SMART_ENABLE5_LVS2_en_lvs2_int_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_DISABLE5_LVS2_UNION
 结构说明  : DISABLE5_LVS2 寄存器结构定义。地址偏移量:0x033，初值:0x00，宽度:8
 寄存器说明: 电源模块关闭5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dis_lvs2_int : 1;  /* bit[0]  : LVS2开关禁止：
                                                       写1关闭，自动清零。 */
        unsigned char  reserved     : 7;  /* bit[1-7]: 保留。 */
    } reg;
} SOC_SMART_DISABLE5_LVS2_UNION;
#endif
#define SOC_SMART_DISABLE5_LVS2_dis_lvs2_int_START  (0)
#define SOC_SMART_DISABLE5_LVS2_dis_lvs2_int_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_ONOFF_STATUS5_LVS2_UNION
 结构说明  : ONOFF_STATUS5_LVS2 寄存器结构定义。地址偏移量:0x034，初值:0x00，宽度:8
 寄存器说明: 电源模块开关状态5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  st_lvs2_int : 1;  /* bit[0]  : LVS2开关状态：
                                                      0：通过寄存器关闭；
                                                      1：通过寄存器开启。 */
        unsigned char  reserved    : 7;  /* bit[1-7]: 保留。 */
    } reg;
} SOC_SMART_ONOFF_STATUS5_LVS2_UNION;
#endif
#define SOC_SMART_ONOFF_STATUS5_LVS2_st_lvs2_int_START  (0)
#define SOC_SMART_ONOFF_STATUS5_LVS2_st_lvs2_int_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_ENABLE6_XO_CLK_UNION
 结构说明  : ENABLE6_XO_CLK 寄存器结构定义。地址偏移量:0x035，初值:0x00，宽度:8
 寄存器说明: 电源模块使能6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_xo_clk0_int : 1;  /* bit[0]  : BB buf开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_xo_clk1_int : 1;  /* bit[1]  : CONN buf开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_xo_clk2_int : 1;  /* bit[2]  : NFC buf开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_xo_clk3_int : 1;  /* bit[3]  : RF1 buf开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  en_xo_clk4_int : 1;  /* bit[4]  : RF2 buf开关使能：
                                                         写1开启，自动清零。 */
        unsigned char  reserved       : 3;  /* bit[5-7]:  */
    } reg;
} SOC_SMART_ENABLE6_XO_CLK_UNION;
#endif
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk0_int_START  (0)
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk0_int_END    (0)
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk1_int_START  (1)
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk1_int_END    (1)
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk2_int_START  (2)
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk2_int_END    (2)
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk3_int_START  (3)
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk3_int_END    (3)
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk4_int_START  (4)
#define SOC_SMART_ENABLE6_XO_CLK_en_xo_clk4_int_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_DISABLE6_XO_CLK_UNION
 结构说明  : DISABLE6_XO_CLK 寄存器结构定义。地址偏移量:0x036，初值:0x00，宽度:8
 寄存器说明: 电源模块关闭6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dis_xo_clk0_int : 1;  /* bit[0]  : BB buf开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_xo_clk1_int : 1;  /* bit[1]  : CONN buf开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_xo_clk2_int : 1;  /* bit[2]  : NFC buf开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_xo_clk3_int : 1;  /* bit[3]  : RF1 buf开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  dis_xo_clk4_int : 1;  /* bit[4]  : RF2 buf开关禁止：
                                                          写1关闭，自动清零。 */
        unsigned char  reserved        : 3;  /* bit[5-7]:  */
    } reg;
} SOC_SMART_DISABLE6_XO_CLK_UNION;
#endif
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk0_int_START  (0)
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk0_int_END    (0)
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk1_int_START  (1)
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk1_int_END    (1)
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk2_int_START  (2)
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk2_int_END    (2)
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk3_int_START  (3)
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk3_int_END    (3)
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk4_int_START  (4)
#define SOC_SMART_DISABLE6_XO_CLK_dis_xo_clk4_int_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_ONOFF_STATUS6_XO_CLK_UNION
 结构说明  : ONOFF_STATUS6_XO_CLK 寄存器结构定义。地址偏移量:0x037，初值:0x00，宽度:8
 寄存器说明: 电源模块开关状态6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  st_xo_clk0_int : 1;  /* bit[0]  : BB buf开关状态(仅支持寄存器方式开启)：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_xo_clk1_int : 1;  /* bit[1]  : CONN buf开关状态(仅支持寄存器方式开启)：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_xo_clk2_int : 1;  /* bit[2]  : NFC buf开关状态(仅支持寄存器方式开启)：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_xo_clk3_int : 1;  /* bit[3]  : RF1 buf开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  st_xo_clk4_int : 1;  /* bit[4]  : RF2 buf开关状态：
                                                         0：通过寄存器关闭；
                                                         1：通过寄存器开启。 */
        unsigned char  reserved       : 3;  /* bit[5-7]:  */
    } reg;
} SOC_SMART_ONOFF_STATUS6_XO_CLK_UNION;
#endif
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk0_int_START  (0)
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk0_int_END    (0)
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk1_int_START  (1)
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk1_int_END    (1)
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk2_int_START  (2)
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk2_int_END    (2)
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk3_int_START  (3)
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk3_int_END    (3)
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk4_int_START  (4)
#define SOC_SMART_ONOFF_STATUS6_XO_CLK_st_xo_clk4_int_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_MOD_CFG1_UNION
 结构说明  : ECO_MOD_CFG1 寄存器结构定义。地址偏移量:0x038，初值:0x07，宽度:8
 寄存器说明: 电源模块是否跟随PMU进出ECO模式配置1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_eco_buck2_int : 1;  /* bit[0]  : 设置BUCK2是否随PMU进出ECO模式:
                                                           0:不跟随；
                                                           1：跟随。 */
        unsigned char  en_eco_buck3_int : 1;  /* bit[1]  : 设置BUCK3是否随PMU进出ECO模式:
                                                           0:不跟随；
                                                           1：跟随。 */
        unsigned char  en_eco_buck4_int : 1;  /* bit[2]  : 设置BUCK4是否随PMU进出ECO模式:
                                                           0:不跟随；
                                                           1：跟随。 */
        unsigned char  reserved         : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_SMART_ECO_MOD_CFG1_UNION;
#endif
#define SOC_SMART_ECO_MOD_CFG1_en_eco_buck2_int_START  (0)
#define SOC_SMART_ECO_MOD_CFG1_en_eco_buck2_int_END    (0)
#define SOC_SMART_ECO_MOD_CFG1_en_eco_buck3_int_START  (1)
#define SOC_SMART_ECO_MOD_CFG1_en_eco_buck3_int_END    (1)
#define SOC_SMART_ECO_MOD_CFG1_en_eco_buck4_int_START  (2)
#define SOC_SMART_ECO_MOD_CFG1_en_eco_buck4_int_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_MOD_CFG2_UNION
 结构说明  : ECO_MOD_CFG2 寄存器结构定义。地址偏移量:0x039，初值:0xFF，宽度:8
 寄存器说明: 电源模块是否跟随PMU进出ECO模式配置2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_eco_ldo1_int : 1;  /* bit[0]: 设置LDO1是否随PMU进出ECO模式:
                                                        0:不跟随；
                                                        1：跟随。 */
        unsigned char  en_eco_ldo3_int : 1;  /* bit[1]: 设置LDO3是否随PMU进出ECO模式:
                                                        0:不跟随；
                                                        1：跟随。 */
        unsigned char  en_eco_ldo4_int : 1;  /* bit[2]: 设置LDO4是否随PMU进出ECO模式:
                                                        0:不跟随；
                                                        1：跟随。 */
        unsigned char  en_eco_ldo5_int : 1;  /* bit[3]: 设置LDO5是否随PMU进出ECO模式:
                                                        0:不跟随；
                                                        1：跟随。 */
        unsigned char  en_eco_ldo6_int : 1;  /* bit[4]: 设置LDO6是否随PMU进出ECO模式:
                                                        0:不跟随；
                                                        1：跟随。 */
        unsigned char  en_eco_ldo7_int : 1;  /* bit[5]: 设置LDO7是否随PMU进出ECO模式:
                                                        0:不跟随；
                                                        1：跟随。 */
        unsigned char  en_eco_ldo8_int : 1;  /* bit[6]: 设置LDO8是否随PMU进出ECO模式:
                                                        0:不跟随；
                                                        1：跟随。 */
        unsigned char  en_eco_ldo9_int : 1;  /* bit[7]: 设置LDO9是否随PMU进出ECO模式:
                                                        0:不跟随；
                                                        1：跟随。 */
    } reg;
} SOC_SMART_ECO_MOD_CFG2_UNION;
#endif
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo1_int_START  (0)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo1_int_END    (0)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo3_int_START  (1)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo3_int_END    (1)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo4_int_START  (2)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo4_int_END    (2)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo5_int_START  (3)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo5_int_END    (3)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo6_int_START  (4)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo6_int_END    (4)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo7_int_START  (5)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo7_int_END    (5)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo8_int_START  (6)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo8_int_END    (6)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo9_int_START  (7)
#define SOC_SMART_ECO_MOD_CFG2_en_eco_ldo9_int_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_MOD_CFG3_UNION
 结构说明  : ECO_MOD_CFG3 寄存器结构定义。地址偏移量:0x03A，初值:0x7F，宽度:8
 寄存器说明: 电源模块是否跟随PMU进出ECO模式配置3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_eco_ldo10_int : 1;  /* bit[0]: 设置LDO10是否随PMU进出ECO模式:
                                                         0:不跟随；
                                                         1：跟随。 */
        unsigned char  en_eco_ldo11_int : 1;  /* bit[1]: 设置LDO11是否随PMU进出ECO模式:
                                                         0:不跟随；
                                                         1：跟随。 */
        unsigned char  en_eco_ldo12_int : 1;  /* bit[2]: 设置LDO12是否随PMU进出ECO模式:
                                                         0:不跟随；
                                                         1：跟随。 */
        unsigned char  en_eco_ldo13_int : 1;  /* bit[3]: 设置LDO13是否随PMU进出ECO模式:
                                                         0:不跟随；
                                                         1：跟随。 */
        unsigned char  en_eco_ldo15_int : 1;  /* bit[4]: 设置LDO15是否随PMU进出ECO模式:
                                                         0:不跟随；
                                                         1：跟随。 */
        unsigned char  en_eco_ldo16_int : 1;  /* bit[5]: 设置LDO16是否随PMU进出ECO模式:
                                                         0:不跟随；
                                                         1：跟随。 */
        unsigned char  en_eco_ldo18_int : 1;  /* bit[6]: 设置LDO18是否随PMU进出ECO模式:
                                                         0:不跟随；
                                                         1：跟随。 */
        unsigned char  reserved         : 1;  /* bit[7]: 保留。 */
    } reg;
} SOC_SMART_ECO_MOD_CFG3_UNION;
#endif
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo10_int_START  (0)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo10_int_END    (0)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo11_int_START  (1)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo11_int_END    (1)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo12_int_START  (2)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo12_int_END    (2)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo13_int_START  (3)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo13_int_END    (3)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo15_int_START  (4)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo15_int_END    (4)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo16_int_START  (5)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo16_int_END    (5)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo18_int_START  (6)
#define SOC_SMART_ECO_MOD_CFG3_en_eco_ldo18_int_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_MOD_CFG4_UNION
 结构说明  : ECO_MOD_CFG4 寄存器结构定义。地址偏移量:0x03B，初值:0x1F，宽度:8
 寄存器说明: 电源模块是否跟随PMU进出ECO模式配置4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_eco_ldo19_int   : 1;  /* bit[0]  : 设置LDO19是否随PMU进出ECO模式:
                                                             0:不跟随；
                                                             1：跟随。 */
        unsigned char  en_eco_ldo21_int   : 1;  /* bit[1]  : 设置LDO21是否随PMU进出ECO模式:
                                                             0:不跟随；
                                                             1：跟随。 */
        unsigned char  en_eco_pmua_int    : 1;  /* bit[2]  : 设置PMUA是否随PMU进出ECO模式:
                                                             0:不跟随；
                                                             1：跟随。 */
        unsigned char  en_eco_ldo20_int   : 1;  /* bit[3]  : 设置LDO20是否随PMU进出ECO模式:
                                                             0:不跟随；
                                                             1：跟随。 */
        unsigned char  en_eco_ldobuck_int : 1;  /* bit[4]  : 设置LDOBUCK是否随PMU进出ECO模式:
                                                             0:不跟随；
                                                             1：跟随。 */
        unsigned char  reserved           : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_ECO_MOD_CFG4_UNION;
#endif
#define SOC_SMART_ECO_MOD_CFG4_en_eco_ldo19_int_START    (0)
#define SOC_SMART_ECO_MOD_CFG4_en_eco_ldo19_int_END      (0)
#define SOC_SMART_ECO_MOD_CFG4_en_eco_ldo21_int_START    (1)
#define SOC_SMART_ECO_MOD_CFG4_en_eco_ldo21_int_END      (1)
#define SOC_SMART_ECO_MOD_CFG4_en_eco_pmua_int_START     (2)
#define SOC_SMART_ECO_MOD_CFG4_en_eco_pmua_int_END       (2)
#define SOC_SMART_ECO_MOD_CFG4_en_eco_ldo20_int_START    (3)
#define SOC_SMART_ECO_MOD_CFG4_en_eco_ldo20_int_END      (3)
#define SOC_SMART_ECO_MOD_CFG4_en_eco_ldobuck_int_START  (4)
#define SOC_SMART_ECO_MOD_CFG4_en_eco_ldobuck_int_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_MOD_CFG5_UNION
 结构说明  : ECO_MOD_CFG5 寄存器结构定义。地址偏移量:0x03C，初值:0x00，宽度:8
 寄存器说明: 电源模块是否强制进入ECO模式配置5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  force_eco_buck2_int : 1;  /* bit[0]  : 设置是否强行使BUCK2进入ECO模式：
                                                              0:不进入；
                                                              1：进入。 */
        unsigned char  force_eco_buck3_int : 1;  /* bit[1]  : 设置是否强行使BUCK3进入ECO模式：
                                                              0:不进入；
                                                              1：进入。 */
        unsigned char  force_eco_buck4_int : 1;  /* bit[2]  : 设置是否强行使BUCK4进入ECO模式：
                                                              0:不进入；
                                                              1：进入。 */
        unsigned char  reserved            : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_SMART_ECO_MOD_CFG5_UNION;
#endif
#define SOC_SMART_ECO_MOD_CFG5_force_eco_buck2_int_START  (0)
#define SOC_SMART_ECO_MOD_CFG5_force_eco_buck2_int_END    (0)
#define SOC_SMART_ECO_MOD_CFG5_force_eco_buck3_int_START  (1)
#define SOC_SMART_ECO_MOD_CFG5_force_eco_buck3_int_END    (1)
#define SOC_SMART_ECO_MOD_CFG5_force_eco_buck4_int_START  (2)
#define SOC_SMART_ECO_MOD_CFG5_force_eco_buck4_int_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_MOD_CFG6_UNION
 结构说明  : ECO_MOD_CFG6 寄存器结构定义。地址偏移量:0x03D，初值:0x00，宽度:8
 寄存器说明: 电源模块是否强制进入ECO模式配置6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  force_eco_ldo1_int : 1;  /* bit[0]: 设置是否强行使LDO1进入ECO模式：
                                                           0:不进入；
                                                           1：进入。 */
        unsigned char  force_eco_ldo3_int : 1;  /* bit[1]: 设置是否强行使LDO3进入ECO模式：
                                                           0:不进入；
                                                           1：进入。 */
        unsigned char  force_eco_ldo4_int : 1;  /* bit[2]: 设置是否强行使LDO4进入ECO模式：
                                                           0:不进入；
                                                           1：进入。 */
        unsigned char  force_eco_ldo5_int : 1;  /* bit[3]: 设置是否强行使LDO5进入ECO模式：
                                                           0:不进入；
                                                           1：进入。 */
        unsigned char  force_eco_ldo6_int : 1;  /* bit[4]: 设置是否强行使LDO6进入ECO模式：
                                                           0:不进入；
                                                           1：进入。 */
        unsigned char  force_eco_ldo7_int : 1;  /* bit[5]: 设置是否强行使LDO7进入ECO模式：
                                                           0:不进入；
                                                           1：进入。 */
        unsigned char  force_eco_ldo8_int : 1;  /* bit[6]: 设置是否强行使LDO8进入ECO模式：
                                                           0:不进入；
                                                           1：进入。 */
        unsigned char  force_eco_ldo9_int : 1;  /* bit[7]: 设置是否强行使LDO9进入ECO模式：
                                                           0:不进入；
                                                           1：进入。 */
    } reg;
} SOC_SMART_ECO_MOD_CFG6_UNION;
#endif
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo1_int_START  (0)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo1_int_END    (0)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo3_int_START  (1)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo3_int_END    (1)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo4_int_START  (2)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo4_int_END    (2)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo5_int_START  (3)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo5_int_END    (3)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo6_int_START  (4)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo6_int_END    (4)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo7_int_START  (5)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo7_int_END    (5)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo8_int_START  (6)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo8_int_END    (6)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo9_int_START  (7)
#define SOC_SMART_ECO_MOD_CFG6_force_eco_ldo9_int_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_MOD_CFG7_UNION
 结构说明  : ECO_MOD_CFG7 寄存器结构定义。地址偏移量:0x03E，初值:0x00，宽度:8
 寄存器说明: 电源模块是否强制进入ECO模式配置7寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  force_eco_ldo10_int : 1;  /* bit[0]: 设置是否强行使LDO10进入ECO模式：
                                                            0:不进入；
                                                            1：进入。 */
        unsigned char  force_eco_ldo11_int : 1;  /* bit[1]: 设置是否强行使LDO11进入ECO模式：
                                                            0:不进入；
                                                            1：进入。 */
        unsigned char  force_eco_ldo12_int : 1;  /* bit[2]: 设置是否强行使LDO12进入ECO模式：
                                                            0:不进入；
                                                            1：进入。 */
        unsigned char  force_eco_ldo13_int : 1;  /* bit[3]: 设置是否强行使LDO13进入ECO模式：
                                                            0:不进入；
                                                            1：进入。 */
        unsigned char  force_eco_ldo15_int : 1;  /* bit[4]: 设置是否强行使LDO15进入ECO模式：
                                                            0:不进入；
                                                            1：进入。 */
        unsigned char  force_eco_ldo16_int : 1;  /* bit[5]: 设置是否强行使LDO16进入ECO模式：
                                                            0:不进入；
                                                            1：进入。 */
        unsigned char  force_eco_ldo18_int : 1;  /* bit[6]: 设置是否强行使LDO18进入ECO模式：
                                                            0:不进入；
                                                            1：进入。 */
        unsigned char  reserved            : 1;  /* bit[7]: 保留。 */
    } reg;
} SOC_SMART_ECO_MOD_CFG7_UNION;
#endif
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo10_int_START  (0)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo10_int_END    (0)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo11_int_START  (1)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo11_int_END    (1)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo12_int_START  (2)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo12_int_END    (2)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo13_int_START  (3)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo13_int_END    (3)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo15_int_START  (4)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo15_int_END    (4)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo16_int_START  (5)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo16_int_END    (5)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo18_int_START  (6)
#define SOC_SMART_ECO_MOD_CFG7_force_eco_ldo18_int_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_MOD_CFG8_UNION
 结构说明  : ECO_MOD_CFG8 寄存器结构定义。地址偏移量:0x03F，初值:0x00，宽度:8
 寄存器说明: 电源模块是否强制进入ECO模式配置8寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  force_eco_ldo19_int   : 1;  /* bit[0]  : 设置是否强行使LDO19进入ECO模式：
                                                                0:不进入；
                                                                1：进入。 */
        unsigned char  force_eco_ldo21_int   : 1;  /* bit[1]  : 设置是否强行使LDO21进入ECO模式：
                                                                0:不进入；
                                                                1：进入。 */
        unsigned char  force_eco_pmua_int    : 1;  /* bit[2]  : 设置是否强行使PMUA进入ECO模式：
                                                                0:不进入；
                                                                1：进入。 */
        unsigned char  force_eco_ldo20_int   : 1;  /* bit[3]  : 设置是否强行使LDO20进出ECO模式:
                                                                0:不跟随；
                                                                1：跟随。 */
        unsigned char  force_eco_ldobuck_int : 1;  /* bit[4]  : 设置是否强行使ldobuck进出ECO模式:
                                                                0:不跟随；
                                                                1：跟随。 */
        unsigned char  reserved              : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_ECO_MOD_CFG8_UNION;
#endif
#define SOC_SMART_ECO_MOD_CFG8_force_eco_ldo19_int_START    (0)
#define SOC_SMART_ECO_MOD_CFG8_force_eco_ldo19_int_END      (0)
#define SOC_SMART_ECO_MOD_CFG8_force_eco_ldo21_int_START    (1)
#define SOC_SMART_ECO_MOD_CFG8_force_eco_ldo21_int_END      (1)
#define SOC_SMART_ECO_MOD_CFG8_force_eco_pmua_int_START     (2)
#define SOC_SMART_ECO_MOD_CFG8_force_eco_pmua_int_END       (2)
#define SOC_SMART_ECO_MOD_CFG8_force_eco_ldo20_int_START    (3)
#define SOC_SMART_ECO_MOD_CFG8_force_eco_ldo20_int_END      (3)
#define SOC_SMART_ECO_MOD_CFG8_force_eco_ldobuck_int_START  (4)
#define SOC_SMART_ECO_MOD_CFG8_force_eco_ldobuck_int_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_PERI_EN_MARK_UNION
 结构说明  : PERI_EN_MARK 寄存器结构定义。地址偏移量:0x040，初值:0x1F，宽度:8
 寄存器说明: 与PERI_EN硬线和ECO功能相关的屏蔽信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  peri_en_ctrl_lvs2  : 1;  /* bit[0]  : LVS2是否受PERI_EN控制上下电:
                                                             0：不受控；
                                                             1：受控。 */
        unsigned char  peri_en_ctrl_ldo12 : 1;  /* bit[1]  : LDO12是否受PERI_EN控制上下电:
                                                             0：不受控；
                                                             1：受控。 */
        unsigned char  peri_en_ctrl_ldo18 : 1;  /* bit[2]  : LDO18是否受PERI_EN控制上下电:
                                                             0：不受控；
                                                             1：受控。 */
        unsigned char  peri_en_ctrl_ldo8  : 1;  /* bit[3]  : LDO8是否受PERI_EN控制上下电:
                                                             0：不受控；
                                                             1：受控。 */
        unsigned char  peri_en_ctrl_buck2 : 1;  /* bit[4]  : BUCK2是否受PERI_EN控制上下电:
                                                             0：不受控；
                                                             1：受控。 */
        unsigned char  reserved           : 3;  /* bit[5-7]: 保留 */
    } reg;
} SOC_SMART_PERI_EN_MARK_UNION;
#endif
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_lvs2_START   (0)
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_lvs2_END     (0)
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_ldo12_START  (1)
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_ldo12_END    (1)
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_ldo18_START  (2)
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_ldo18_END    (2)
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_ldo8_START   (3)
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_ldo8_END     (3)
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_buck2_START  (4)
#define SOC_SMART_PERI_EN_MARK_peri_en_ctrl_buck2_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK2_REG0_UNION
 结构说明  : BUCK2_REG0 寄存器结构定义。地址偏移量:0x049，初值:0x01，宽度:8
 寄存器说明: BUCK2控制0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_nmos_switch : 1;  /* bit[0]  : buck2 sleep到normal切换时NMOS开启切换方式：（0：和dmd同时切换，1：在dmd之后切换） */
        unsigned char  reserved          : 7;  /* bit[1-7]: 保留 */
    } reg;
} SOC_SMART_BUCK2_REG0_UNION;
#endif
#define SOC_SMART_BUCK2_REG0_buck2_nmos_switch_START  (0)
#define SOC_SMART_BUCK2_REG0_buck2_nmos_switch_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK2_REG1_UNION
 结构说明  : BUCK2_REG1 寄存器结构定义。地址偏移量:0x04A，初值:0x45，宽度:8
 寄存器说明: BUCK2控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_dbias     : 4;  /* bit[0-3]: buck2 <1:0>比较器偏置电流调节（00~11增大）
                                                          buck2<3:2> 比较器主偏置电流调节（00~11增大） */
        unsigned char  buck2_sleep_ng  : 1;  /* bit[4]  : buck2在sleep状态时用于关闭NMOS power管。1：强制关闭，0：不强制关闭 */
        unsigned char  buck2_sleep_dmd : 1;  /* bit[5]  : 在正常模式下强制NMOS管关闭（0：不强制，1：强制） */
        unsigned char  buck2_reg_r     : 2;  /* bit[6-7]: buck2内部regulator电阻，用于调节regulator带宽 */
    } reg;
} SOC_SMART_BUCK2_REG1_UNION;
#endif
#define SOC_SMART_BUCK2_REG1_buck2_dbias_START      (0)
#define SOC_SMART_BUCK2_REG1_buck2_dbias_END        (3)
#define SOC_SMART_BUCK2_REG1_buck2_sleep_ng_START   (4)
#define SOC_SMART_BUCK2_REG1_buck2_sleep_ng_END     (4)
#define SOC_SMART_BUCK2_REG1_buck2_sleep_dmd_START  (5)
#define SOC_SMART_BUCK2_REG1_buck2_sleep_dmd_END    (5)
#define SOC_SMART_BUCK2_REG1_buck2_reg_r_START      (6)
#define SOC_SMART_BUCK2_REG1_buck2_reg_r_END        (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK2_REG2_UNION
 结构说明  : BUCK2_REG2 寄存器结构定义。地址偏移量:0x04B，初值:0x7D，宽度:8
 寄存器说明: BUCK2控制2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_adj_rlx : 4;  /* bit[0-3]: buck2 LX反馈滤波电阻调节
                                                        （1111：电阻全不接入，0000：电阻全接入） */
        unsigned char  buck2_adj_clx : 2;  /* bit[4-5]: buck2 LX反馈滤波电容调节
                                                        （11：电容全接入，00：电容全不接入） */
        unsigned char  buck2_ocp_sel : 2;  /* bit[6-7]: buck2 ocp点调整（00~11增大） */
    } reg;
} SOC_SMART_BUCK2_REG2_UNION;
#endif
#define SOC_SMART_BUCK2_REG2_buck2_adj_rlx_START  (0)
#define SOC_SMART_BUCK2_REG2_buck2_adj_rlx_END    (3)
#define SOC_SMART_BUCK2_REG2_buck2_adj_clx_START  (4)
#define SOC_SMART_BUCK2_REG2_buck2_adj_clx_END    (5)
#define SOC_SMART_BUCK2_REG2_buck2_ocp_sel_START  (6)
#define SOC_SMART_BUCK2_REG2_buck2_ocp_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK2_REG3_UNION
 结构说明  : BUCK2_REG3 寄存器结构定义。地址偏移量:0x04C，初值:0x04，宽度:8
 寄存器说明: BUCK2控制3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_dmd_sel   : 3;  /* bit[0-2]: buck2 dmd点选择（000~111增大） */
        unsigned char  buck2_bias_reg  : 1;  /* bit[3]  : buck2内部regulator电流调节（0:0.5uA，1:1uA） */
        unsigned char  buck2_en_reg    : 1;  /* bit[4]  : buck2内部精度调整器使能信号。0：使能，1：不使能 */
        unsigned char  buck2_dmd_clamp : 1;  /* bit[5]  : dmd嵌位使能信号（0：加上；1：不加上） */
        unsigned char  buck2_ocp_dis   : 1;  /* bit[6]  : buck2 内部ocp环路屏蔽信号。0：不屏蔽，1：屏蔽 */
        unsigned char  buck2_ocp_delay : 1;  /* bit[7]  : ocp屏蔽时间延长20ns信号（0：不延长，1：延长） */
    } reg;
} SOC_SMART_BUCK2_REG3_UNION;
#endif
#define SOC_SMART_BUCK2_REG3_buck2_dmd_sel_START    (0)
#define SOC_SMART_BUCK2_REG3_buck2_dmd_sel_END      (2)
#define SOC_SMART_BUCK2_REG3_buck2_bias_reg_START   (3)
#define SOC_SMART_BUCK2_REG3_buck2_bias_reg_END     (3)
#define SOC_SMART_BUCK2_REG3_buck2_en_reg_START     (4)
#define SOC_SMART_BUCK2_REG3_buck2_en_reg_END       (4)
#define SOC_SMART_BUCK2_REG3_buck2_dmd_clamp_START  (5)
#define SOC_SMART_BUCK2_REG3_buck2_dmd_clamp_END    (5)
#define SOC_SMART_BUCK2_REG3_buck2_ocp_dis_START    (6)
#define SOC_SMART_BUCK2_REG3_buck2_ocp_dis_END      (6)
#define SOC_SMART_BUCK2_REG3_buck2_ocp_delay_START  (7)
#define SOC_SMART_BUCK2_REG3_buck2_ocp_delay_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK2_REG4_UNION
 结构说明  : BUCK2_REG4 寄存器结构定义。地址偏移量:0x04D，初值:0x0B，宽度:8
 寄存器说明: BUCK2控制4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_reg_dr      : 3;  /* bit[0-2]: buck2内部regulator电阻，用于调节regulator调整精度范围 */
        unsigned char  buck2_dt_sel      : 2;  /* bit[3-4]: buck2死区时间调节。(1：不额外延迟，0：额外延迟5ns）
                                                            <1>:死区时间调节（包括NTP和PTN）
                                                            <0>:采用PWM信号新方案的NTP的死区时间调节 */
        unsigned char  buck2_regop_clamp : 1;  /* bit[5]  : regulator 嵌位电路使能（1：使能。0：不使能） */
        unsigned char  buck2_reg_ss      : 1;  /* bit[6]  : buck2软启动时内部regulator状态。
                                                            0：正常状态，1：单位增益反馈 */
        unsigned char  buck2_regop_c     : 1;  /* bit[7]  : buck2内部regulator OP输出电容，用于调节OP的slew-rate。 */
    } reg;
} SOC_SMART_BUCK2_REG4_UNION;
#endif
#define SOC_SMART_BUCK2_REG4_buck2_reg_dr_START       (0)
#define SOC_SMART_BUCK2_REG4_buck2_reg_dr_END         (2)
#define SOC_SMART_BUCK2_REG4_buck2_dt_sel_START       (3)
#define SOC_SMART_BUCK2_REG4_buck2_dt_sel_END         (4)
#define SOC_SMART_BUCK2_REG4_buck2_regop_clamp_START  (5)
#define SOC_SMART_BUCK2_REG4_buck2_regop_clamp_END    (5)
#define SOC_SMART_BUCK2_REG4_buck2_reg_ss_START       (6)
#define SOC_SMART_BUCK2_REG4_buck2_reg_ss_END         (6)
#define SOC_SMART_BUCK2_REG4_buck2_regop_c_START      (7)
#define SOC_SMART_BUCK2_REG4_buck2_regop_c_END        (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK2_REG5_UNION
 结构说明  : BUCK2_REG5 寄存器结构定义。地址偏移量:0x04E，初值:0x3C，宽度:8
 寄存器说明: BUCK2控制5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_ng_n_sel : 2;  /* bit[0-1]: buck2 N power管驱动对应NMOS驱动能力调节（00~11增大驱动能力） */
        unsigned char  buck2_ng_p_sel : 2;  /* bit[2-3]: buck2 N power管驱动对应PMOS驱动能力调节（00~11减小驱动能力） */
        unsigned char  buck2_pg_n_sel : 2;  /* bit[4-5]: buck2 P power管驱动对应NMOS驱动能力调节（00~11减小驱动能力） */
        unsigned char  buck2_pg_p_sel : 2;  /* bit[6-7]: buck2 P power管驱动对应PMOS驱动能力调节（00~11增加驱动能力） */
    } reg;
} SOC_SMART_BUCK2_REG5_UNION;
#endif
#define SOC_SMART_BUCK2_REG5_buck2_ng_n_sel_START  (0)
#define SOC_SMART_BUCK2_REG5_buck2_ng_n_sel_END    (1)
#define SOC_SMART_BUCK2_REG5_buck2_ng_p_sel_START  (2)
#define SOC_SMART_BUCK2_REG5_buck2_ng_p_sel_END    (3)
#define SOC_SMART_BUCK2_REG5_buck2_pg_n_sel_START  (4)
#define SOC_SMART_BUCK2_REG5_buck2_pg_n_sel_END    (5)
#define SOC_SMART_BUCK2_REG5_buck2_pg_p_sel_START  (6)
#define SOC_SMART_BUCK2_REG5_buck2_pg_p_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK2_REG6_UNION
 结构说明  : BUCK2_REG6 寄存器结构定义。地址偏移量:0x04F，初值:0x05，宽度:8
 寄存器说明: BUCK2控制6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_ocp_toff   : 2;  /* bit[0-1]: buck2发生ocp时最小关断时间量（00~11增大） */
        unsigned char  buck2_ton        : 2;  /* bit[2-3]: buck2最小导通时间（00~11增大） */
        unsigned char  buck2_filter_ton : 2;  /* bit[4-5]: 比较器滤毛刺（00~11增大） */
        unsigned char  buck2_ng_dt_sel  : 1;  /* bit[6]  : buck2 P管到N管死区模式选择（1：并入新死区控制模式，0：不并入新死区控制模式） */
        unsigned char  buck2_pg_dt_sel  : 1;  /* bit[7]  : buck2 N管到P管死区模式选择（1：并入新死区控制模式，0：不并入新死区控制模式） */
    } reg;
} SOC_SMART_BUCK2_REG6_UNION;
#endif
#define SOC_SMART_BUCK2_REG6_buck2_ocp_toff_START    (0)
#define SOC_SMART_BUCK2_REG6_buck2_ocp_toff_END      (1)
#define SOC_SMART_BUCK2_REG6_buck2_ton_START         (2)
#define SOC_SMART_BUCK2_REG6_buck2_ton_END           (3)
#define SOC_SMART_BUCK2_REG6_buck2_filter_ton_START  (4)
#define SOC_SMART_BUCK2_REG6_buck2_filter_ton_END    (5)
#define SOC_SMART_BUCK2_REG6_buck2_ng_dt_sel_START   (6)
#define SOC_SMART_BUCK2_REG6_buck2_ng_dt_sel_END     (6)
#define SOC_SMART_BUCK2_REG6_buck2_pg_dt_sel_START   (7)
#define SOC_SMART_BUCK2_REG6_buck2_pg_dt_sel_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK2_REG7_UNION
 结构说明  : BUCK2_REG7 寄存器结构定义。地址偏移量:0x050，初值:0x03，宽度:8
 寄存器说明: BUCK2控制7寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_dmd_ton    : 3;  /* bit[0-2]: buck2发生dmd时的加入导通时间量（000~111增大） */
        unsigned char  buck2_short_pdp  : 1;  /* bit[3]  : 短路保护屏蔽寄存器.0:使能短路保护;1:不使能短路保护 */
        unsigned char  buck2_cmp_filter : 1;  /* bit[4]  : 比较器滤毛刺功能（0：不虑毛刺；1：虑毛刺） */
        unsigned char  buck2_ton_dmd    : 1;  /* bit[5]  : buck2发生dmd时屏蔽控制最小导通时间（0：不屏蔽） */
        unsigned char  reserved         : 2;  /* bit[6-7]:  */
    } reg;
} SOC_SMART_BUCK2_REG7_UNION;
#endif
#define SOC_SMART_BUCK2_REG7_buck2_dmd_ton_START     (0)
#define SOC_SMART_BUCK2_REG7_buck2_dmd_ton_END       (2)
#define SOC_SMART_BUCK2_REG7_buck2_short_pdp_START   (3)
#define SOC_SMART_BUCK2_REG7_buck2_short_pdp_END     (3)
#define SOC_SMART_BUCK2_REG7_buck2_cmp_filter_START  (4)
#define SOC_SMART_BUCK2_REG7_buck2_cmp_filter_END    (4)
#define SOC_SMART_BUCK2_REG7_buck2_ton_dmd_START     (5)
#define SOC_SMART_BUCK2_REG7_buck2_ton_dmd_END       (5)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK3_REG0_UNION
 结构说明  : BUCK3_REG0 寄存器结构定义。地址偏移量:0x051，初值:0x01，宽度:8
 寄存器说明: BUCK3控制0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck3_nmos_switch : 1;  /* bit[0]  : buck3 sleep到normal切换时NMOS开启切换方式：（0：和dmd同时切换，1：在dmd之后切换） */
        unsigned char  reserved          : 7;  /* bit[1-7]: 保留 */
    } reg;
} SOC_SMART_BUCK3_REG0_UNION;
#endif
#define SOC_SMART_BUCK3_REG0_buck3_nmos_switch_START  (0)
#define SOC_SMART_BUCK3_REG0_buck3_nmos_switch_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK3_REG1_UNION
 结构说明  : BUCK3_REG1 寄存器结构定义。地址偏移量:0x052，初值:0x45，宽度:8
 寄存器说明: BUCK3控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck3_dbias     : 4;  /* bit[0-3]: buck3 <1:0>比较器偏置电流调节（00~11增大）
                                                          buck3<3:2> 比较器主偏置电流调节（00~11增大） */
        unsigned char  buck3_sleep_ng  : 1;  /* bit[4]  : buck3在sleep状态时用于关闭NMOS power管。1：强制关闭，0：不强制关闭 */
        unsigned char  buck3_sleep_dmd : 1;  /* bit[5]  : 在正常模式下强制NMOS管关闭（0：不强制，1：强制） */
        unsigned char  buck3_reg_r     : 2;  /* bit[6-7]: buck3内部regulator电阻，用于调节regulator带宽 */
    } reg;
} SOC_SMART_BUCK3_REG1_UNION;
#endif
#define SOC_SMART_BUCK3_REG1_buck3_dbias_START      (0)
#define SOC_SMART_BUCK3_REG1_buck3_dbias_END        (3)
#define SOC_SMART_BUCK3_REG1_buck3_sleep_ng_START   (4)
#define SOC_SMART_BUCK3_REG1_buck3_sleep_ng_END     (4)
#define SOC_SMART_BUCK3_REG1_buck3_sleep_dmd_START  (5)
#define SOC_SMART_BUCK3_REG1_buck3_sleep_dmd_END    (5)
#define SOC_SMART_BUCK3_REG1_buck3_reg_r_START      (6)
#define SOC_SMART_BUCK3_REG1_buck3_reg_r_END        (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK3_REG2_UNION
 结构说明  : BUCK3_REG2 寄存器结构定义。地址偏移量:0x053，初值:0x5A，宽度:8
 寄存器说明: BUCK3控制2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck3_adj_rlx : 4;  /* bit[0-3]: buck3 LX反馈滤波电阻调节
                                                        （1111：电阻全不接入，0000：电阻全接入） */
        unsigned char  buck3_adj_clx : 2;  /* bit[4-5]: buck3 LX反馈滤波电容调节
                                                        （11：电容全接入，00：电容全不接入） */
        unsigned char  buck3_ocp_sel : 2;  /* bit[6-7]: buck3 ocp点调整（00~11增大） */
    } reg;
} SOC_SMART_BUCK3_REG2_UNION;
#endif
#define SOC_SMART_BUCK3_REG2_buck3_adj_rlx_START  (0)
#define SOC_SMART_BUCK3_REG2_buck3_adj_rlx_END    (3)
#define SOC_SMART_BUCK3_REG2_buck3_adj_clx_START  (4)
#define SOC_SMART_BUCK3_REG2_buck3_adj_clx_END    (5)
#define SOC_SMART_BUCK3_REG2_buck3_ocp_sel_START  (6)
#define SOC_SMART_BUCK3_REG2_buck3_ocp_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK3_REG3_UNION
 结构说明  : BUCK3_REG3 寄存器结构定义。地址偏移量:0x054，初值:0x03，宽度:8
 寄存器说明: BUCK3控制3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck3_dmd_sel   : 3;  /* bit[0-2]: buck3 dmd点选择（000~111增大） */
        unsigned char  buck3_bias_reg  : 1;  /* bit[3]  : buck3内部regulator电流调节（0:0.5uA，1:1uA） */
        unsigned char  buck3_en_reg    : 1;  /* bit[4]  : buck3内部精度调整器使能信号。0：使能，1：不使能 */
        unsigned char  buck3_dmd_clamp : 1;  /* bit[5]  : dmd嵌位使能信号（0：加上；1：不加上） */
        unsigned char  buck3_ocp_dis   : 1;  /* bit[6]  : buck3 内部ocp环路屏蔽信号。0：不屏蔽，1：屏蔽 */
        unsigned char  buck3_ocp_delay : 1;  /* bit[7]  : ocp屏蔽时间延长20ns信号（0：不延长，1：延长） */
    } reg;
} SOC_SMART_BUCK3_REG3_UNION;
#endif
#define SOC_SMART_BUCK3_REG3_buck3_dmd_sel_START    (0)
#define SOC_SMART_BUCK3_REG3_buck3_dmd_sel_END      (2)
#define SOC_SMART_BUCK3_REG3_buck3_bias_reg_START   (3)
#define SOC_SMART_BUCK3_REG3_buck3_bias_reg_END     (3)
#define SOC_SMART_BUCK3_REG3_buck3_en_reg_START     (4)
#define SOC_SMART_BUCK3_REG3_buck3_en_reg_END       (4)
#define SOC_SMART_BUCK3_REG3_buck3_dmd_clamp_START  (5)
#define SOC_SMART_BUCK3_REG3_buck3_dmd_clamp_END    (5)
#define SOC_SMART_BUCK3_REG3_buck3_ocp_dis_START    (6)
#define SOC_SMART_BUCK3_REG3_buck3_ocp_dis_END      (6)
#define SOC_SMART_BUCK3_REG3_buck3_ocp_delay_START  (7)
#define SOC_SMART_BUCK3_REG3_buck3_ocp_delay_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK3_REG4_UNION
 结构说明  : BUCK3_REG4 寄存器结构定义。地址偏移量:0x055，初值:0x0B，宽度:8
 寄存器说明: BUCK3控制4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck3_reg_dr      : 3;  /* bit[0-2]: buck3内部regulator电阻，用于调节regulator调整精度范围 */
        unsigned char  buck3_dt_sel      : 2;  /* bit[3-4]: buck3死区时间调节。(1：不额外延迟，0：额外延迟5ns）
                                                            <1>:死区时间调节（包括NTP和PTN）
                                                            <0>:采用PWM信号新方案的NTP的死区时间调节 */
        unsigned char  buck3_regop_clamp : 1;  /* bit[5]  : regulator 嵌位电路使能（1：使能。0：不使能） */
        unsigned char  buck3_reg_ss      : 1;  /* bit[6]  : buck3软启动时内部regulator状态。
                                                            0：正常状态，1：单位增益反馈 */
        unsigned char  buck3_regop_c     : 1;  /* bit[7]  : buck3内部regulator OP输出电容，用于调节OP的slew-rate。 */
    } reg;
} SOC_SMART_BUCK3_REG4_UNION;
#endif
#define SOC_SMART_BUCK3_REG4_buck3_reg_dr_START       (0)
#define SOC_SMART_BUCK3_REG4_buck3_reg_dr_END         (2)
#define SOC_SMART_BUCK3_REG4_buck3_dt_sel_START       (3)
#define SOC_SMART_BUCK3_REG4_buck3_dt_sel_END         (4)
#define SOC_SMART_BUCK3_REG4_buck3_regop_clamp_START  (5)
#define SOC_SMART_BUCK3_REG4_buck3_regop_clamp_END    (5)
#define SOC_SMART_BUCK3_REG4_buck3_reg_ss_START       (6)
#define SOC_SMART_BUCK3_REG4_buck3_reg_ss_END         (6)
#define SOC_SMART_BUCK3_REG4_buck3_regop_c_START      (7)
#define SOC_SMART_BUCK3_REG4_buck3_regop_c_END        (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK3_REG5_UNION
 结构说明  : BUCK3_REG5 寄存器结构定义。地址偏移量:0x056，初值:0x3C，宽度:8
 寄存器说明: BUCK3控制5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck3_ng_n_sel : 2;  /* bit[0-1]: buck3 N power管驱动对应NMOS驱动能力调节（00~11增大驱动能力） */
        unsigned char  buck3_ng_p_sel : 2;  /* bit[2-3]: buck3 N power管驱动对应PMOS驱动能力调节（00~11减小驱动能力） */
        unsigned char  buck3_pg_n_sel : 2;  /* bit[4-5]: buck3 P power管驱动对应NMOS驱动能力调节（00~11减小驱动能力） */
        unsigned char  buck3_pg_p_sel : 2;  /* bit[6-7]: buck3 P power管驱动对应PMOS驱动能力调节（00~11增加驱动能力） */
    } reg;
} SOC_SMART_BUCK3_REG5_UNION;
#endif
#define SOC_SMART_BUCK3_REG5_buck3_ng_n_sel_START  (0)
#define SOC_SMART_BUCK3_REG5_buck3_ng_n_sel_END    (1)
#define SOC_SMART_BUCK3_REG5_buck3_ng_p_sel_START  (2)
#define SOC_SMART_BUCK3_REG5_buck3_ng_p_sel_END    (3)
#define SOC_SMART_BUCK3_REG5_buck3_pg_n_sel_START  (4)
#define SOC_SMART_BUCK3_REG5_buck3_pg_n_sel_END    (5)
#define SOC_SMART_BUCK3_REG5_buck3_pg_p_sel_START  (6)
#define SOC_SMART_BUCK3_REG5_buck3_pg_p_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK3_REG6_UNION
 结构说明  : BUCK3_REG6 寄存器结构定义。地址偏移量:0x057，初值:0x01，宽度:8
 寄存器说明: BUCK3控制6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck3_ocp_toff   : 2;  /* bit[0-1]: buck3发生ocp时最小关断时间量（00~11增大） */
        unsigned char  buck3_ton        : 2;  /* bit[2-3]: buck3最小导通时间（00~11增大） */
        unsigned char  buck3_filter_ton : 2;  /* bit[4-5]: 比较器滤毛刺（00~11增大） */
        unsigned char  buck3_ng_dt_sel  : 1;  /* bit[6]  : buck3 P管到N管死区模式选择（1：并入新死区控制模式，0：不并入新死区控制模式） */
        unsigned char  buck3_pg_dt_sel  : 1;  /* bit[7]  : buck3 N管到P管死区模式选择（1：并入新死区控制模式，0：不并入新死区控制模式） */
    } reg;
} SOC_SMART_BUCK3_REG6_UNION;
#endif
#define SOC_SMART_BUCK3_REG6_buck3_ocp_toff_START    (0)
#define SOC_SMART_BUCK3_REG6_buck3_ocp_toff_END      (1)
#define SOC_SMART_BUCK3_REG6_buck3_ton_START         (2)
#define SOC_SMART_BUCK3_REG6_buck3_ton_END           (3)
#define SOC_SMART_BUCK3_REG6_buck3_filter_ton_START  (4)
#define SOC_SMART_BUCK3_REG6_buck3_filter_ton_END    (5)
#define SOC_SMART_BUCK3_REG6_buck3_ng_dt_sel_START   (6)
#define SOC_SMART_BUCK3_REG6_buck3_ng_dt_sel_END     (6)
#define SOC_SMART_BUCK3_REG6_buck3_pg_dt_sel_START   (7)
#define SOC_SMART_BUCK3_REG6_buck3_pg_dt_sel_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK3_REG7_UNION
 结构说明  : BUCK3_REG7 寄存器结构定义。地址偏移量:0x058，初值:0x03，宽度:8
 寄存器说明: BUCK3控制7寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck3_dmd_ton    : 3;  /* bit[0-2]: buck3发生dmd时的加入导通时间量（000~111增大） */
        unsigned char  buck3_short_pdp  : 1;  /* bit[3]  : 短路保护屏蔽寄存器.0:使能短路保护;1:不使能短路保护 */
        unsigned char  buck3_cmp_filter : 1;  /* bit[4]  : 比较器滤毛刺功能（0：不虑毛刺；1：虑毛刺） */
        unsigned char  buck3_ton_dmd    : 1;  /* bit[5]  : buck3发生dmd时屏蔽控制最小导通时间（0：不屏蔽） */
        unsigned char  reserved         : 2;  /* bit[6-7]: 保留 */
    } reg;
} SOC_SMART_BUCK3_REG7_UNION;
#endif
#define SOC_SMART_BUCK3_REG7_buck3_dmd_ton_START     (0)
#define SOC_SMART_BUCK3_REG7_buck3_dmd_ton_END       (2)
#define SOC_SMART_BUCK3_REG7_buck3_short_pdp_START   (3)
#define SOC_SMART_BUCK3_REG7_buck3_short_pdp_END     (3)
#define SOC_SMART_BUCK3_REG7_buck3_cmp_filter_START  (4)
#define SOC_SMART_BUCK3_REG7_buck3_cmp_filter_END    (4)
#define SOC_SMART_BUCK3_REG7_buck3_ton_dmd_START     (5)
#define SOC_SMART_BUCK3_REG7_buck3_ton_dmd_END       (5)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK4_REG0_UNION
 结构说明  : BUCK4_REG0 寄存器结构定义。地址偏移量:0x059，初值:0x01，宽度:8
 寄存器说明: BUCK4控制0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_nmos_switch : 1;  /* bit[0]  : buck4 sleep到normal切换时NMOS开启切换方式：（0：和dmd同时切换，1：在dmd之后切换） */
        unsigned char  reserved          : 7;  /* bit[1-7]: 保留 */
    } reg;
} SOC_SMART_BUCK4_REG0_UNION;
#endif
#define SOC_SMART_BUCK4_REG0_buck4_nmos_switch_START  (0)
#define SOC_SMART_BUCK4_REG0_buck4_nmos_switch_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK4_REG1_UNION
 结构说明  : BUCK4_REG1 寄存器结构定义。地址偏移量:0x05A，初值:0x45，宽度:8
 寄存器说明: BUCK4控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_dbias     : 4;  /* bit[0-3]: buck4 <1:0>比较器偏置电流调节（00~11增大）
                                                          buck4<3:2> 比较器主偏置电流调节（00~11增大） */
        unsigned char  buck4_sleep_ng  : 1;  /* bit[4]  : buck4在sleep状态时用于关闭NMOS power管。1：强制关闭，0：不强制关闭 */
        unsigned char  buck4_sleep_dmd : 1;  /* bit[5]  : 在正常模式下强制NMOS管关闭（0：不强制，1：强制） */
        unsigned char  buck4_reg_r     : 2;  /* bit[6-7]: buck4内部regulator电阻，用于调节regulator带宽 */
    } reg;
} SOC_SMART_BUCK4_REG1_UNION;
#endif
#define SOC_SMART_BUCK4_REG1_buck4_dbias_START      (0)
#define SOC_SMART_BUCK4_REG1_buck4_dbias_END        (3)
#define SOC_SMART_BUCK4_REG1_buck4_sleep_ng_START   (4)
#define SOC_SMART_BUCK4_REG1_buck4_sleep_ng_END     (4)
#define SOC_SMART_BUCK4_REG1_buck4_sleep_dmd_START  (5)
#define SOC_SMART_BUCK4_REG1_buck4_sleep_dmd_END    (5)
#define SOC_SMART_BUCK4_REG1_buck4_reg_r_START      (6)
#define SOC_SMART_BUCK4_REG1_buck4_reg_r_END        (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK4_REG2_UNION
 结构说明  : BUCK4_REG2 寄存器结构定义。地址偏移量:0x05B，初值:0x5A，宽度:8
 寄存器说明: BUCK4控制2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_adj_rlx : 4;  /* bit[0-3]: buck4 LX反馈滤波电阻调节
                                                        （1111：电阻全不接入，0000：电阻全接入） */
        unsigned char  buck4_adj_clx : 2;  /* bit[4-5]: buck4 LX反馈滤波电容调节
                                                        （11：电容全接入，00：电容全不接入） */
        unsigned char  buck4_ocp_sel : 2;  /* bit[6-7]: buck4 ocp点调整（00~11增大） */
    } reg;
} SOC_SMART_BUCK4_REG2_UNION;
#endif
#define SOC_SMART_BUCK4_REG2_buck4_adj_rlx_START  (0)
#define SOC_SMART_BUCK4_REG2_buck4_adj_rlx_END    (3)
#define SOC_SMART_BUCK4_REG2_buck4_adj_clx_START  (4)
#define SOC_SMART_BUCK4_REG2_buck4_adj_clx_END    (5)
#define SOC_SMART_BUCK4_REG2_buck4_ocp_sel_START  (6)
#define SOC_SMART_BUCK4_REG2_buck4_ocp_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK4_REG3_UNION
 结构说明  : BUCK4_REG3 寄存器结构定义。地址偏移量:0x05C，初值:0x03，宽度:8
 寄存器说明: BUCK4控制3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_dmd_sel   : 3;  /* bit[0-2]: buck4 dmd点选择（000~111增大） */
        unsigned char  buck4_bias_reg  : 1;  /* bit[3]  : buck4内部regulator电流调节（0:0.5uA，1:1uA） */
        unsigned char  buck4_en_reg    : 1;  /* bit[4]  : buck4内部精度调整器使能信号。0：使能，1：不使能 */
        unsigned char  buck4_dmd_clamp : 1;  /* bit[5]  : dmd嵌位使能信号（0：加上；1：不加上） */
        unsigned char  buck4_ocp_dis   : 1;  /* bit[6]  : buck4 内部ocp环路屏蔽信号。0：不屏蔽，1：屏蔽 */
        unsigned char  buck4_ocp_delay : 1;  /* bit[7]  : ocp屏蔽时间延长20ns信号（0：不延长，1：延长） */
    } reg;
} SOC_SMART_BUCK4_REG3_UNION;
#endif
#define SOC_SMART_BUCK4_REG3_buck4_dmd_sel_START    (0)
#define SOC_SMART_BUCK4_REG3_buck4_dmd_sel_END      (2)
#define SOC_SMART_BUCK4_REG3_buck4_bias_reg_START   (3)
#define SOC_SMART_BUCK4_REG3_buck4_bias_reg_END     (3)
#define SOC_SMART_BUCK4_REG3_buck4_en_reg_START     (4)
#define SOC_SMART_BUCK4_REG3_buck4_en_reg_END       (4)
#define SOC_SMART_BUCK4_REG3_buck4_dmd_clamp_START  (5)
#define SOC_SMART_BUCK4_REG3_buck4_dmd_clamp_END    (5)
#define SOC_SMART_BUCK4_REG3_buck4_ocp_dis_START    (6)
#define SOC_SMART_BUCK4_REG3_buck4_ocp_dis_END      (6)
#define SOC_SMART_BUCK4_REG3_buck4_ocp_delay_START  (7)
#define SOC_SMART_BUCK4_REG3_buck4_ocp_delay_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK4_REG4_UNION
 结构说明  : BUCK4_REG4 寄存器结构定义。地址偏移量:0x05D，初值:0x0B，宽度:8
 寄存器说明: BUCK4控制4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_reg_dr      : 3;  /* bit[0-2]: buck4内部regulator电阻，用于调节regulator调整精度范围 */
        unsigned char  buck4_dt_sel      : 2;  /* bit[3-4]: buck4死区时间调节。(1：不额外延迟，0：额外延迟5ns）
                                                            <1>:死区时间调节（包括NTP和PTN）
                                                            <0>:采用PWM信号新方案的NTP的死区时间调节 */
        unsigned char  buck4_regop_clamp : 1;  /* bit[5]  : regulator 嵌位电路使能（1：使能。0：不使能） */
        unsigned char  buck4_reg_ss      : 1;  /* bit[6]  : buck4软启动时内部regulator状态。
                                                            0：正常状态，1：单位增益反馈 */
        unsigned char  buck4_regop_c     : 1;  /* bit[7]  : buck4内部regulator OP输出电容，用于调节OP的slew-rate。 */
    } reg;
} SOC_SMART_BUCK4_REG4_UNION;
#endif
#define SOC_SMART_BUCK4_REG4_buck4_reg_dr_START       (0)
#define SOC_SMART_BUCK4_REG4_buck4_reg_dr_END         (2)
#define SOC_SMART_BUCK4_REG4_buck4_dt_sel_START       (3)
#define SOC_SMART_BUCK4_REG4_buck4_dt_sel_END         (4)
#define SOC_SMART_BUCK4_REG4_buck4_regop_clamp_START  (5)
#define SOC_SMART_BUCK4_REG4_buck4_regop_clamp_END    (5)
#define SOC_SMART_BUCK4_REG4_buck4_reg_ss_START       (6)
#define SOC_SMART_BUCK4_REG4_buck4_reg_ss_END         (6)
#define SOC_SMART_BUCK4_REG4_buck4_regop_c_START      (7)
#define SOC_SMART_BUCK4_REG4_buck4_regop_c_END        (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK4_REG5_UNION
 结构说明  : BUCK4_REG5 寄存器结构定义。地址偏移量:0x05E，初值:0x3C，宽度:8
 寄存器说明: BUCK4控制5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_ng_n_sel : 2;  /* bit[0-1]: buck4 N power管驱动对应NMOS驱动能力调节（00~11增大驱动能力） */
        unsigned char  buck4_ng_p_sel : 2;  /* bit[2-3]: buck4 N power管驱动对应PMOS驱动能力调节（00~11减小驱动能力） */
        unsigned char  buck4_pg_n_sel : 2;  /* bit[4-5]: buck4 P power管驱动对应NMOS驱动能力调节（00~11减小驱动能力） */
        unsigned char  buck4_pg_p_sel : 2;  /* bit[6-7]: buck4 P power管驱动对应PMOS驱动能力调节（00~11增加驱动能力） */
    } reg;
} SOC_SMART_BUCK4_REG5_UNION;
#endif
#define SOC_SMART_BUCK4_REG5_buck4_ng_n_sel_START  (0)
#define SOC_SMART_BUCK4_REG5_buck4_ng_n_sel_END    (1)
#define SOC_SMART_BUCK4_REG5_buck4_ng_p_sel_START  (2)
#define SOC_SMART_BUCK4_REG5_buck4_ng_p_sel_END    (3)
#define SOC_SMART_BUCK4_REG5_buck4_pg_n_sel_START  (4)
#define SOC_SMART_BUCK4_REG5_buck4_pg_n_sel_END    (5)
#define SOC_SMART_BUCK4_REG5_buck4_pg_p_sel_START  (6)
#define SOC_SMART_BUCK4_REG5_buck4_pg_p_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK4_REG6_UNION
 结构说明  : BUCK4_REG6 寄存器结构定义。地址偏移量:0x05F，初值:0x05，宽度:8
 寄存器说明: BUCK4控制6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_ocp_toff   : 2;  /* bit[0-1]: buck4发生ocp时最小关断时间量（00~11增大） */
        unsigned char  buck4_ton        : 2;  /* bit[2-3]: buck4最小导通时间（00~11增大） */
        unsigned char  buck4_filter_ton : 2;  /* bit[4-5]: 比较器滤毛刺（00~11增大） */
        unsigned char  buck4_ng_dt_sel  : 1;  /* bit[6]  : buck4 P管到N管死区模式选择（1：并入新死区控制模式，0：不并入新死区控制模式） */
        unsigned char  buck4_pg_dt_sel  : 1;  /* bit[7]  : buck4 N管到P管死区模式选择（1：并入新死区控制模式，0：不并入新死区控制模式） */
    } reg;
} SOC_SMART_BUCK4_REG6_UNION;
#endif
#define SOC_SMART_BUCK4_REG6_buck4_ocp_toff_START    (0)
#define SOC_SMART_BUCK4_REG6_buck4_ocp_toff_END      (1)
#define SOC_SMART_BUCK4_REG6_buck4_ton_START         (2)
#define SOC_SMART_BUCK4_REG6_buck4_ton_END           (3)
#define SOC_SMART_BUCK4_REG6_buck4_filter_ton_START  (4)
#define SOC_SMART_BUCK4_REG6_buck4_filter_ton_END    (5)
#define SOC_SMART_BUCK4_REG6_buck4_ng_dt_sel_START   (6)
#define SOC_SMART_BUCK4_REG6_buck4_ng_dt_sel_END     (6)
#define SOC_SMART_BUCK4_REG6_buck4_pg_dt_sel_START   (7)
#define SOC_SMART_BUCK4_REG6_buck4_pg_dt_sel_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK4_REG7_UNION
 结构说明  : BUCK4_REG7 寄存器结构定义。地址偏移量:0x060，初值:0x02，宽度:8
 寄存器说明: BUCK4控制7寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_dmd_ton    : 3;  /* bit[0-2]: buck4发生dmd时的加入导通时间量（000~111增大） */
        unsigned char  buck4_short_pdp  : 1;  /* bit[3]  : 短路保护屏蔽寄存器.0:使能短路保护;1:不使能短路保护 */
        unsigned char  buck4_cmp_filter : 1;  /* bit[4]  : 比较器滤毛刺功能（0：不虑毛刺；1：虑毛刺） */
        unsigned char  buck4_ton_dmd    : 1;  /* bit[5]  : buck4发生dmd时屏蔽控制最小导通时间（0：不屏蔽） */
        unsigned char  reserved         : 2;  /* bit[6-7]: 保留。 */
    } reg;
} SOC_SMART_BUCK4_REG7_UNION;
#endif
#define SOC_SMART_BUCK4_REG7_buck4_dmd_ton_START     (0)
#define SOC_SMART_BUCK4_REG7_buck4_dmd_ton_END       (2)
#define SOC_SMART_BUCK4_REG7_buck4_short_pdp_START   (3)
#define SOC_SMART_BUCK4_REG7_buck4_short_pdp_END     (3)
#define SOC_SMART_BUCK4_REG7_buck4_cmp_filter_START  (4)
#define SOC_SMART_BUCK4_REG7_buck4_cmp_filter_END    (4)
#define SOC_SMART_BUCK4_REG7_buck4_ton_dmd_START     (5)
#define SOC_SMART_BUCK4_REG7_buck4_ton_dmd_END       (5)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK_RESERVE0_UNION
 结构说明  : BUCK_RESERVE0 寄存器结构定义。地址偏移量:0x061，初值:0x00，宽度:8
 寄存器说明: BUCK预留0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck_reserve1 : 8;  /* bit[0-7]: buck备用寄存器 */
    } reg;
} SOC_SMART_BUCK_RESERVE0_UNION;
#endif
#define SOC_SMART_BUCK_RESERVE0_buck_reserve1_START  (0)
#define SOC_SMART_BUCK_RESERVE0_buck_reserve1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK_RESERVE1_UNION
 结构说明  : BUCK_RESERVE1 寄存器结构定义。地址偏移量:0x062，初值:0x00，宽度:8
 寄存器说明: BUCK预留1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck_reserve2 : 8;  /* bit[0-7]: buck备用寄存器 */
    } reg;
} SOC_SMART_BUCK_RESERVE1_UNION;
#endif
#define SOC_SMART_BUCK_RESERVE1_buck_reserve2_START  (0)
#define SOC_SMART_BUCK_RESERVE1_buck_reserve2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLK_TOP0_UNION
 结构说明  : CLK_TOP0 寄存器结构定义。地址偏移量:0x063，初值:0x00，宽度:8
 寄存器说明: CLK_TOP控制0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  xo_otp_reg  : 2;  /* bit[0-1]: XO core的电流配置 */
        unsigned char  xo_ldo_curr : 2;  /* bit[2-3]: 内部LDO偏置电阻控制，00表示电流300uA */
        unsigned char  xo_ldo_res  : 3;  /* bit[4-6]: 内部LDO的反馈电阻，000:1.8V;001：1.4V;010：1.5V;011：1.6V;100：1.7V;101：1.9V;110：2.0V;111：2.1V */
        unsigned char  reserved    : 1;  /* bit[7]  : 保留。 */
    } reg;
} SOC_SMART_CLK_TOP0_UNION;
#endif
#define SOC_SMART_CLK_TOP0_xo_otp_reg_START   (0)
#define SOC_SMART_CLK_TOP0_xo_otp_reg_END     (1)
#define SOC_SMART_CLK_TOP0_xo_ldo_curr_START  (2)
#define SOC_SMART_CLK_TOP0_xo_ldo_curr_END    (3)
#define SOC_SMART_CLK_TOP0_xo_ldo_res_START   (4)
#define SOC_SMART_CLK_TOP0_xo_ldo_res_END     (6)


/*****************************************************************************
 结构名    : SOC_SMART_CLK_TOP1_UNION
 结构说明  : CLK_TOP1 寄存器结构定义。地址偏移量:0x064，初值:0x00，宽度:8
 寄存器说明: CLK_TOP控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved_0       : 4;  /* bit[0-3]: 保留 */
        unsigned char  xo_buff_curr_reg : 1;  /* bit[4]  : XO core input buffer的电流配置  */
        unsigned char  xo_otp_reg_sel1  : 1;  /* bit[5]  : XO core input buffer的电流配置选择0：OTP控制1：寄存器控制 */
        unsigned char  xo_otp_reg_sel0  : 1;  /* bit[6]  : XO core的电流配置选择0：OTP控制1：寄存器控制 */
        unsigned char  reserved_1       : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_CLK_TOP1_UNION;
#endif
#define SOC_SMART_CLK_TOP1_xo_buff_curr_reg_START  (4)
#define SOC_SMART_CLK_TOP1_xo_buff_curr_reg_END    (4)
#define SOC_SMART_CLK_TOP1_xo_otp_reg_sel1_START   (5)
#define SOC_SMART_CLK_TOP1_xo_otp_reg_sel1_END     (5)
#define SOC_SMART_CLK_TOP1_xo_otp_reg_sel0_START   (6)
#define SOC_SMART_CLK_TOP1_xo_otp_reg_sel0_END     (6)


/*****************************************************************************
 结构名    : SOC_SMART_CLK_TOP3_UNION
 结构说明  : CLK_TOP3 寄存器结构定义。地址偏移量:0x066，初值:0xFD，宽度:8
 寄存器说明: CLK_TOP控制3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  trim_freq_clk3 : 2;  /* bit[0-1]: clk_buf3(RF1_BUF)驱动能力控制，00:1x;01:2x;10:3x;11:4x, */
        unsigned char  trim_freq_clk2 : 2;  /* bit[2-3]: clk_buf2（NFC_BUF）驱动能力控制，00:1x;01:2x;10:3x;11:4x, */
        unsigned char  trim_freq_clk1 : 2;  /* bit[4-5]: clk_buf1（CONN_BUF）驱动能力控制，00:1x;01:2x;10:3x;11:4x, */
        unsigned char  trim_freq_clk0 : 2;  /* bit[6-7]: clk_buf0（BB_BUF）驱动能力控制，00:1x;01:2x;10:3x;11:4x, */
    } reg;
} SOC_SMART_CLK_TOP3_UNION;
#endif
#define SOC_SMART_CLK_TOP3_trim_freq_clk3_START  (0)
#define SOC_SMART_CLK_TOP3_trim_freq_clk3_END    (1)
#define SOC_SMART_CLK_TOP3_trim_freq_clk2_START  (2)
#define SOC_SMART_CLK_TOP3_trim_freq_clk2_END    (3)
#define SOC_SMART_CLK_TOP3_trim_freq_clk1_START  (4)
#define SOC_SMART_CLK_TOP3_trim_freq_clk1_END    (5)
#define SOC_SMART_CLK_TOP3_trim_freq_clk0_START  (6)
#define SOC_SMART_CLK_TOP3_trim_freq_clk0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLK_TOP4_UNION
 结构说明  : CLK_TOP4 寄存器结构定义。地址偏移量:0x067，初值:0x01，宽度:8
 寄存器说明: CLK_TOP控制4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  trim_freq_clk4 : 2;  /* bit[0-1]: clk_buf3(RF2_BUF)驱动能力控制，00:1x;01:2x;10:3x;11:4x, */
        unsigned char  reserved       : 6;  /* bit[2-7]: 保留 */
    } reg;
} SOC_SMART_CLK_TOP4_UNION;
#endif
#define SOC_SMART_CLK_TOP4_trim_freq_clk4_START  (0)
#define SOC_SMART_CLK_TOP4_trim_freq_clk4_END    (1)


/*****************************************************************************
 结构名    : SOC_SMART_CLK_TOP5_UNION
 结构说明  : CLK_TOP5 寄存器结构定义。地址偏移量:0x068，初值:0x00，宽度:8
 寄存器说明: CLK_TOP控制5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  xo_reserve : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SMART_CLK_TOP5_UNION;
#endif
#define SOC_SMART_CLK_TOP5_xo_reserve_START  (0)
#define SOC_SMART_CLK_TOP5_xo_reserve_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLASSD_CTRL1_UNION
 结构说明  : CLASSD_CTRL1 寄存器结构定义。地址偏移量:0x069，初值:0x04，宽度:8
 寄存器说明: CLASS_D控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  classd_gain     : 2;  /* bit[0-1]: classD增益控制。
                                                          00:12db
                                                          01:15db
                                                          10:18db
                                                          11:21db
                                                          其它：mute。 */
        unsigned char  classd_i_ocp    : 2;  /* bit[2-3]: classd 过流阈值调节:
                                                          00 1.2A ；
                                                          01 1.59A ；
                                                          10 2A ；
                                                          11 2.39A 。 */
        unsigned char  classd_mute_sel : 1;  /* bit[4]  : classd_mute_sel控制。
                                                          0：延时mute模式；
                                                          1：正常mute模式。 */
        unsigned char  classd_mute     : 1;  /* bit[5]  : classd_mute控制。
                                                          0：正常工作；
                                                          1：Mute。 */
        unsigned char  reserved        : 2;  /* bit[6-7]: 保留 */
    } reg;
} SOC_SMART_CLASSD_CTRL1_UNION;
#endif
#define SOC_SMART_CLASSD_CTRL1_classd_gain_START      (0)
#define SOC_SMART_CLASSD_CTRL1_classd_gain_END        (1)
#define SOC_SMART_CLASSD_CTRL1_classd_i_ocp_START     (2)
#define SOC_SMART_CLASSD_CTRL1_classd_i_ocp_END       (3)
#define SOC_SMART_CLASSD_CTRL1_classd_mute_sel_START  (4)
#define SOC_SMART_CLASSD_CTRL1_classd_mute_sel_END    (4)
#define SOC_SMART_CLASSD_CTRL1_classd_mute_START      (5)
#define SOC_SMART_CLASSD_CTRL1_classd_mute_END        (5)


/*****************************************************************************
 结构名    : SOC_SMART_CLASSD_CTRL2_UNION
 结构说明  : CLASSD_CTRL2 寄存器结构定义。地址偏移量:0x06A，初值:0x05，宽度:8
 寄存器说明: CLASS_D控制2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  classd_n_sel  : 2;  /* bit[0-1]: 驱动级nmos控制。
                                                        00：减半驱动；
                                                        01：默认驱动；
                                                        10：两倍驱动；
                                                        11：三倍驱动。 */
        unsigned char  classd_p_sel  : 2;  /* bit[2-3]: 驱动级pmos控制。
                                                        00：减半驱动；
                                                        01：默认驱动；
                                                        10：两倍驱动；
                                                        11：三倍驱动。 */
        unsigned char  classd_i_pump : 2;  /* bit[4-5]: Ramp波中，pump电流控制。
                                                        00：6μA；
                                                        01：7μA；
                                                        10：7μA；
                                                        11：8μA。 */
        unsigned char  reserved      : 2;  /* bit[6-7]: 保留 */
    } reg;
} SOC_SMART_CLASSD_CTRL2_UNION;
#endif
#define SOC_SMART_CLASSD_CTRL2_classd_n_sel_START   (0)
#define SOC_SMART_CLASSD_CTRL2_classd_n_sel_END     (1)
#define SOC_SMART_CLASSD_CTRL2_classd_p_sel_START   (2)
#define SOC_SMART_CLASSD_CTRL2_classd_p_sel_END     (3)
#define SOC_SMART_CLASSD_CTRL2_classd_i_pump_START  (4)
#define SOC_SMART_CLASSD_CTRL2_classd_i_pump_END    (5)


/*****************************************************************************
 结构名    : SOC_SMART_CLASSD_CTRL3_UNION
 结构说明  : CLASSD_CTRL3 寄存器结构定义。地址偏移量:0x06B，初值:0x00，宽度:8
 寄存器说明: CLASS_D控制3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  classd_ocp_bps : 1;  /* bit[0]  : 过流保护控制。
                                                         0：开启过流保护；
                                                         1：bypass过流保护电路。 */
        unsigned char  classd_dt_sel  : 1;  /* bit[1]  : dead time控制。
                                                         0：3ns；
                                                         1：6ns。 */
        unsigned char  classd_pls_byp : 1;  /* bit[2]  : 最小脉宽门限：
                                                         0：正常工作
                                                         1：bypass最小脉宽限制 */
        unsigned char  en_classd_int  : 1;  /* bit[3]  : classD使能控制。
                                                         1：开启；
                                                         0：关闭。 */
        unsigned char  classd_drv_en  : 1;  /* bit[4]  : 驱动电路的使能信号。1：使能0：不使能 */
        unsigned char  reserved       : 3;  /* bit[5-7]: 保留 */
    } reg;
} SOC_SMART_CLASSD_CTRL3_UNION;
#endif
#define SOC_SMART_CLASSD_CTRL3_classd_ocp_bps_START  (0)
#define SOC_SMART_CLASSD_CTRL3_classd_ocp_bps_END    (0)
#define SOC_SMART_CLASSD_CTRL3_classd_dt_sel_START   (1)
#define SOC_SMART_CLASSD_CTRL3_classd_dt_sel_END     (1)
#define SOC_SMART_CLASSD_CTRL3_classd_pls_byp_START  (2)
#define SOC_SMART_CLASSD_CTRL3_classd_pls_byp_END    (2)
#define SOC_SMART_CLASSD_CTRL3_en_classd_int_START   (3)
#define SOC_SMART_CLASSD_CTRL3_en_classd_int_END     (3)
#define SOC_SMART_CLASSD_CTRL3_classd_drv_en_START   (4)
#define SOC_SMART_CLASSD_CTRL3_classd_drv_en_END     (4)


/*****************************************************************************
 结构名    : SOC_SMART_VSET_BUCK2_ADJ_UNION
 结构说明  : VSET_BUCK2_ADJ 寄存器结构定义。地址偏移量:0x06D，初值:0x20，宽度:8
 寄存器说明: BUCK2电压调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck2_dv : 6;
        unsigned char  reserved : 2;  /* bit[6-7]: 保留 */
    } reg;
} SOC_SMART_VSET_BUCK2_ADJ_UNION;
#endif
#define SOC_SMART_VSET_BUCK2_ADJ_buck2_dv_START  (0)
#define SOC_SMART_VSET_BUCK2_ADJ_buck2_dv_END    (5)


/*****************************************************************************
 结构名    : SOC_SMART_VSET_BUCK3_ADJ_UNION
 结构说明  : VSET_BUCK3_ADJ 寄存器结构定义。地址偏移量:0x06E，初值:0x02，宽度:8
 寄存器说明: BUCK3电压调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck3_dv : 4;  /* bit[0-3]: buck3输出电压调节:
                                                   0000 1.2 V；
                                                   0001 1.225 V；
                                                   0010 1.25 V；
                                                   0011 1.275 V；
                                                   0100 1.3 V；
                                                   0101 1.325 V；
                                                   0110 1.35 V；
                                                   0111 1.375 V；
                                                   1000 1.4 V；
                                                   1001 1.425 V；
                                                   1010 1.45 V；
                                                   1011 1.475 V；
                                                   1100 1.5 V；
                                                   1101 1.525 V；
                                                   1110 1.55 V；
                                                   1111 1.575 V。 */
        unsigned char  reserved : 4;  /* bit[4-7]: 保留 */
    } reg;
} SOC_SMART_VSET_BUCK3_ADJ_UNION;
#endif
#define SOC_SMART_VSET_BUCK3_ADJ_buck3_dv_START  (0)
#define SOC_SMART_VSET_BUCK3_ADJ_buck3_dv_END    (3)


/*****************************************************************************
 结构名    : SOC_SMART_VSET_BUCK4_ADJ0_UNION
 结构说明  : VSET_BUCK4_ADJ0 寄存器结构定义。地址偏移量:0x06F，初值:0x08，宽度:8
 寄存器说明: BUCK4电压调节0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_dv : 4;  /* bit[0-3]: buck4输出电压调节:
                                                   0000 1.9 V；
                                                   0001 1.925 V；
                                                   0010 1.95 V；
                                                   0011 1.975 V；
                                                   0100 2 V；
                                                   0101 2.025 V；
                                                   0110 2.05 V；
                                                   0111 2.075 V；
                                                   1000 2.1 V；
                                                   1001 2.125 V；
                                                   1010 2.15 V；
                                                   1011 2.175 V；
                                                   1100 2.2 V；
                                                   1101 2.225 V；
                                                   1110 2.25 V；
                                                   1111 2.275 V。 */
        unsigned char  reserved : 4;  /* bit[4-7]: 保留 */
    } reg;
} SOC_SMART_VSET_BUCK4_ADJ0_UNION;
#endif
#define SOC_SMART_VSET_BUCK4_ADJ0_buck4_dv_START  (0)
#define SOC_SMART_VSET_BUCK4_ADJ0_buck4_dv_END    (3)


/*****************************************************************************
 结构名    : SOC_SMART_VSET_BUCK4_ADJ1_UNION
 结构说明  : VSET_BUCK4_ADJ1 寄存器结构定义。地址偏移量:0x070，初值:0x08，宽度:8
 寄存器说明: BUCK4电压调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck4_dv_adj : 4;  /* bit[0-3]: 当PMU进入ECO模式后，Buck4的电压调低，目标电压寄存器配置（寄存器默认值为2.1V） */
        unsigned char  reserved     : 4;  /* bit[4-7]: 保留 */
    } reg;
} SOC_SMART_VSET_BUCK4_ADJ1_UNION;
#endif
#define SOC_SMART_VSET_BUCK4_ADJ1_buck4_dv_adj_START  (0)
#define SOC_SMART_VSET_BUCK4_ADJ1_buck4_dv_adj_END    (3)


/*****************************************************************************
 结构名    : SOC_SMART_LDO1_REG_ADJ_UNION
 结构说明  : LDO1_REG_ADJ 寄存器结构定义。地址偏移量:0x071，初值:0x14，宽度:8
 寄存器说明: LDO1调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo1  : 3;  /* bit[0-2]: LDO1电压调节：
                                                     0x0: 1.90V；
                                                     0x1: 1.95V ；
                                                     0x2: 2.00V ；
                                                     0x3: 2.05V ；
                                                     0x4: 2.10V ；
                                                     0x5: 2.15V ；
                                                     0x6: 2.20V ；
                                                     0x7: 2.25V。 */
        unsigned char  reserved_0 : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo1 : 3;  /* bit[4-6]: LDO1负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1 : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO1_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO1_REG_ADJ_vset_ldo1_START   (0)
#define SOC_SMART_LDO1_REG_ADJ_vset_ldo1_END     (2)
#define SOC_SMART_LDO1_REG_ADJ_vrset_ldo1_START  (4)
#define SOC_SMART_LDO1_REG_ADJ_vrset_ldo1_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO2_REG_ADJ_UNION
 结构说明  : LDO2_REG_ADJ 寄存器结构定义。地址偏移量:0x072，初值:0x10，宽度:8
 寄存器说明: LDO2调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo2  : 3;  /* bit[0-2]: LDO2电压调节：
                                                     0x0: 2.50V ；
                                                     0x1: 2.60V ；
                                                     0x2: 2.70V ；
                                                     0x3: 2.80V ；
                                                     0x4: 2.90V ；
                                                     0x5: 3.00V ；
                                                     0x6: 3.10V ；
                                                     0x7: 3.20V 。 */
        unsigned char  reserved_0 : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo2 : 3;  /* bit[4-6]: LDO2负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1 : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO2_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO2_REG_ADJ_vset_ldo2_START   (0)
#define SOC_SMART_LDO2_REG_ADJ_vset_ldo2_END     (2)
#define SOC_SMART_LDO2_REG_ADJ_vrset_ldo2_START  (4)
#define SOC_SMART_LDO2_REG_ADJ_vrset_ldo2_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO3_REG_ADJ0_UNION
 结构说明  : LDO3_REG_ADJ0 寄存器结构定义。地址偏移量:0x073，初值:0x25，宽度:8
 寄存器说明: LDO3调节0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo3  : 3;  /* bit[0-2]: LDO3电压调节：
                                                     0x0:0.80V ；
                                                     0x1: 0.85V ；
                                                     0x2: 0.90V ；
                                                     0x3: 0.95V ；
                                                     0x4: 1.00V ；
                                                     0x5: 1.05V；
                                                     0x6: 1.10V；
                                                     0x7：1.15V。
                                                     OTP可编程默认电压：
                                                     otp_vset:
                                                     0x0 : 1.05V ；
                                                     0x1 : 0.9V 。
                                                     注：OTP没有编程前Pmu上电时ldo3_batt为1.0V，编程后电压为0.85V，当ldo3_buck上电时vset_ldo3 otp没烧写为1.05V，编程后为0.9V。 */
        unsigned char  reserved_0 : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo3 : 3;  /* bit[4-6]: LDO3负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1 : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO3_REG_ADJ0_UNION;
#endif
#define SOC_SMART_LDO3_REG_ADJ0_vset_ldo3_START   (0)
#define SOC_SMART_LDO3_REG_ADJ0_vset_ldo3_END     (2)
#define SOC_SMART_LDO3_REG_ADJ0_vrset_ldo3_START  (4)
#define SOC_SMART_LDO3_REG_ADJ0_vrset_ldo3_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO3_REG_ADJ1_UNION
 结构说明  : LDO3_REG_ADJ1 寄存器结构定义。地址偏移量:0x074，初值:0x05，宽度:8
 寄存器说明: LDO3调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo3_adj : 3;  /* bit[0-2]: 关闭BUCK2的时候，LDO3电压切换调节寄存器：
                                                        0x0:0.80V ；
                                                        0x1: 0.85V ；
                                                        0x2: 0.90V ；
                                                        0x3: 0.95V ；
                                                        0x4: 1.00V ；
                                                        0x5: 1.05V；
                                                        0x6: 1.10V；
                                                        0x7：1.15V。
                                                        注：LDO3在Buck2关闭后切换电压，切换目标电压档位由该寄存器配置（默认为1.05V）。在Buck2打开前恢复电压 */
        unsigned char  reserved      : 5;  /* bit[3-7]: 保留 */
    } reg;
} SOC_SMART_LDO3_REG_ADJ1_UNION;
#endif
#define SOC_SMART_LDO3_REG_ADJ1_vset_ldo3_adj_START  (0)
#define SOC_SMART_LDO3_REG_ADJ1_vset_ldo3_adj_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_LDO4_REG_ADJ_UNION
 结构说明  : LDO4_REG_ADJ 寄存器结构定义。地址偏移量:0x075，初值:0x00，宽度:8
 寄存器说明: LDO4调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  i_sst_ldo4 : 1;  /* bit[0]  : LDO4的缓启电流调整，1表示增大缓启电流，0表示缓启电流为默认值 */
        unsigned char  reserved   : 7;  /* bit[1-7]: 保留 */
    } reg;
} SOC_SMART_LDO4_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO4_REG_ADJ_i_sst_ldo4_START  (0)
#define SOC_SMART_LDO4_REG_ADJ_i_sst_ldo4_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_LDO5_REG_ADJ_UNION
 结构说明  : LDO5_REG_ADJ 寄存器结构定义。地址偏移量:0x076，初值:0x24，宽度:8
 寄存器说明: LDO5调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo5  : 3;  /* bit[0-2]: LDO5电压调节：
                                                     0x0: 1.60V ；
                                                     0x1: 1.65V ；
                                                     0x2:1.70V ；
                                                     0x3: 1.75V ；
                                                     0x4: 1.80V ；
                                                     0x5: 1.85V ；
                                                     0x6: 1.90V ；
                                                     0x7: 1.95V 。 */
        unsigned char  reserved_0 : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo5 : 3;  /* bit[4-6]: LDO5负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1 : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO5_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO5_REG_ADJ_vset_ldo5_START   (0)
#define SOC_SMART_LDO5_REG_ADJ_vset_ldo5_END     (2)
#define SOC_SMART_LDO5_REG_ADJ_vrset_ldo5_START  (4)
#define SOC_SMART_LDO5_REG_ADJ_vrset_ldo5_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO6_REG_ADJ_UNION
 结构说明  : LDO6_REG_ADJ 寄存器结构定义。地址偏移量:0x077，初值:0x14，宽度:8
 寄存器说明: LDO6调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo6  : 3;  /* bit[0-2]: LDO6电压调节：
                                                     0x0: 2.60V ；
                                                     0x1: 2.65V ；
                                                     0x2: 2.70V ；
                                                     0x3: 2.750V ；
                                                     0x4: 2.80V ；
                                                     0x5: 2.85V ；
                                                     0x6: 2.90V；
                                                     0x7：2.95V。 */
        unsigned char  reserved_0 : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo6 : 3;  /* bit[4-6]: LDO6负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1 : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO6_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO6_REG_ADJ_vset_ldo6_START   (0)
#define SOC_SMART_LDO6_REG_ADJ_vset_ldo6_END     (2)
#define SOC_SMART_LDO6_REG_ADJ_vrset_ldo6_START  (4)
#define SOC_SMART_LDO6_REG_ADJ_vrset_ldo6_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO7_REG_ADJ_UNION
 结构说明  : LDO7_REG_ADJ 寄存器结构定义。地址偏移量:0x078，初值:0x12，宽度:8
 寄存器说明: LDO7调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo7  : 3;  /* bit[0-2]: LDO7电压调节：
                                                     0x0: 1.80V ；
                                                     0x1: 1.85V ；
                                                     0x2: 2.85V ；
                                                     0x3: 2.90V ；
                                                     0x4: 3.00V ；
                                                     0x5: 3.10V ；
                                                     0x6: 3.20V ；
                                                     0x7: 3.30V 。 */
        unsigned char  reserved_0 : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo7 : 3;  /* bit[4-6]: LDO7负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1 : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO7_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO7_REG_ADJ_vset_ldo7_START   (0)
#define SOC_SMART_LDO7_REG_ADJ_vset_ldo7_END     (2)
#define SOC_SMART_LDO7_REG_ADJ_vrset_ldo7_START  (4)
#define SOC_SMART_LDO7_REG_ADJ_vrset_ldo7_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO8_REG_ADJ_UNION
 结构说明  : LDO8_REG_ADJ 寄存器结构定义。地址偏移量:0x079，初值:0x14，宽度:8
 寄存器说明: LDO8调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo8  : 3;  /* bit[0-2]: LDO8电压调节：
                                                     0x0: 1.000V ；
                                                     0x1: 1.025V ；
                                                     0x2: 1.05V ；
                                                     0x3: 1.075V ；
                                                     0x4: 1.100V ；
                                                     0x5: 1.125V ；
                                                     0x6: 1.15V ；
                                                     0x7：1.175V 。 */
        unsigned char  reserved_0 : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo8 : 3;  /* bit[4-6]: LDO8负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1 : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO8_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO8_REG_ADJ_vset_ldo8_START   (0)
#define SOC_SMART_LDO8_REG_ADJ_vset_ldo8_END     (2)
#define SOC_SMART_LDO8_REG_ADJ_vrset_ldo8_START  (4)
#define SOC_SMART_LDO8_REG_ADJ_vrset_ldo8_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO9_REG_ADJ_UNION
 结构说明  : LDO9_REG_ADJ 寄存器结构定义。地址偏移量:0x07A，初值:0x10，宽度:8
 寄存器说明: LDO9调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo9  : 3;  /* bit[0-2]: LDO9电压调节：
                                                     0x0: 1.80V ；
                                                     0x1: 1.85V ；
                                                     0x2: 2.85V ；
                                                     0x3: 2.90V ；
                                                     0x4: 3.00V ；
                                                     0x5: 3.10V ；
                                                     0x6: 3.20V ；
                                                     0x7: 3.30V 。 */
        unsigned char  reserved_0 : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo9 : 3;  /* bit[4-6]: LDO9负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1 : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO9_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO9_REG_ADJ_vset_ldo9_START   (0)
#define SOC_SMART_LDO9_REG_ADJ_vset_ldo9_END     (2)
#define SOC_SMART_LDO9_REG_ADJ_vrset_ldo9_START  (4)
#define SOC_SMART_LDO9_REG_ADJ_vrset_ldo9_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO10_REG_ADJ_UNION
 结构说明  : LDO10_REG_ADJ 寄存器结构定义。地址偏移量:0x07B，初值:0x25，宽度:8
 寄存器说明: LDO10调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo10  : 3;  /* bit[0-2]: LDO10电压调节：
                                                      0x0: 1.80V ；
                                                      0x1: 1.85V ；
                                                      0x2: 1.90V ；
                                                      0x3: 2.75V ；
                                                      0x4: 2.80V ；
                                                      0x5: 2.85V ；
                                                      0x6: 2.90V ；
                                                      0x7: 3.00V 。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo10 : 3;  /* bit[4-6]: LDO10负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO10_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO10_REG_ADJ_vset_ldo10_START   (0)
#define SOC_SMART_LDO10_REG_ADJ_vset_ldo10_END     (2)
#define SOC_SMART_LDO10_REG_ADJ_vrset_ldo10_START  (4)
#define SOC_SMART_LDO10_REG_ADJ_vrset_ldo10_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO11_REG_ADJ_UNION
 结构说明  : LDO11_REG_ADJ 寄存器结构定义。地址偏移量:0x07C，初值:0x10，宽度:8
 寄存器说明: LDO11调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo11  : 3;  /* bit[0-2]: LDO11电压调节：
                                                      0x0: 1.80V ；
                                                      0x1: 1.85V ；
                                                      0x2: 2.85V ；
                                                      0x3: 2.90V ；
                                                      0x4: 3.00V ；
                                                      0x5: 3.10V ；
                                                      0x6: 3.20V ；
                                                      0x7: 3.30V 。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo11 : 3;  /* bit[4-6]: LDO11负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO11_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO11_REG_ADJ_vset_ldo11_START   (0)
#define SOC_SMART_LDO11_REG_ADJ_vset_ldo11_END     (2)
#define SOC_SMART_LDO11_REG_ADJ_vrset_ldo11_START  (4)
#define SOC_SMART_LDO11_REG_ADJ_vrset_ldo11_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO12_REG_ADJ_UNION
 结构说明  : LDO12_REG_ADJ 寄存器结构定义。地址偏移量:0x07D，初值:0x16，宽度:8
 寄存器说明: LDO12调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo12  : 3;  /* bit[0-2]: LDO12电压调节：
                                                      0x0: 1.80V ；
                                                      0x1: 1.85V ；
                                                      0x2: 2.85V ；
                                                      0x3: 2.90V ；
                                                      0x4: 3.00V ；
                                                      0x5: 3.10V ；
                                                      0x6: 3.20V ；
                                                      0x7: 3.30V 。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo12 : 3;  /* bit[4-6]: LDO12负载调整率补偿调整。 */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO12_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO12_REG_ADJ_vset_ldo12_START   (0)
#define SOC_SMART_LDO12_REG_ADJ_vset_ldo12_END     (2)
#define SOC_SMART_LDO12_REG_ADJ_vrset_ldo12_START  (4)
#define SOC_SMART_LDO12_REG_ADJ_vrset_ldo12_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO13_REG_ADJ_UNION
 结构说明  : LDO13_REG_ADJ 寄存器结构定义。地址偏移量:0x07E，初值:0x24，宽度:8
 寄存器说明: LDO13调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo13  : 3;  /* bit[0-2]: LDO13电压调节：
                                                      0x0: 1.60V ；
                                                      0x1: 1.65V ；
                                                      0x2: 1.70V ；
                                                      0x3: 1.75V ；
                                                      0x4: 1.80V ；
                                                      0x5: 1.85V ；
                                                      0x6: 1.90V ；
                                                      0x7: 1.95V。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo13 : 3;  /* bit[4-6]: LDO13负载调整率补偿调整。 */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO13_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO13_REG_ADJ_vset_ldo13_START   (0)
#define SOC_SMART_LDO13_REG_ADJ_vset_ldo13_END     (2)
#define SOC_SMART_LDO13_REG_ADJ_vrset_ldo13_START  (4)
#define SOC_SMART_LDO13_REG_ADJ_vrset_ldo13_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO14_REG_ADJ_UNION
 结构说明  : LDO14_REG_ADJ 寄存器结构定义。地址偏移量:0x07F，初值:0x23，宽度:8
 寄存器说明: LDO14调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo14  : 3;  /* bit[0-2]: LDO14电压调节：
                                                      0x0: 2.50V ；
                                                      0x1: 2.60V ；
                                                      0x2: 2.70V ；
                                                      0x3: 2.80V ；
                                                      0x4: 2.90V ；
                                                      0x5: 3.00V ；
                                                      0x6: 3.10V ；
                                                      0x7: 3.20V。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo14 : 3;  /* bit[4-6]: LDO14负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO14_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO14_REG_ADJ_vset_ldo14_START   (0)
#define SOC_SMART_LDO14_REG_ADJ_vset_ldo14_END     (2)
#define SOC_SMART_LDO14_REG_ADJ_vrset_ldo14_START  (4)
#define SOC_SMART_LDO14_REG_ADJ_vrset_ldo14_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO15_REG_ADJ_UNION
 结构说明  : LDO15_REG_ADJ 寄存器结构定义。地址偏移量:0x080，初值:0x24，宽度:8
 寄存器说明: LDO15调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo15  : 3;  /* bit[0-2]: LDO15电压调节：
                                                      0x0: 1.60V ；
                                                      0x1: 1.65V ；
                                                      0x2: 1.70V ；
                                                      0x3: 1.75V ；
                                                      0x4: 1.80V ；
                                                      0x5: 1.85V ；
                                                      0x6: 1.90V ；
                                                      0x7: 1.95V。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo15 : 3;  /* bit[4-6]: LDO15负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO15_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO15_REG_ADJ_vset_ldo15_START   (0)
#define SOC_SMART_LDO15_REG_ADJ_vset_ldo15_END     (2)
#define SOC_SMART_LDO15_REG_ADJ_vrset_ldo15_START  (4)
#define SOC_SMART_LDO15_REG_ADJ_vrset_ldo15_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO16_REG_ADJ_UNION
 结构说明  : LDO16_REG_ADJ 寄存器结构定义。地址偏移量:0x081，初值:0x23，宽度:8
 寄存器说明: LDO16调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo16  : 3;  /* bit[0-2]: LDO16电压调节：
                                                      0x0: 1.65V ；
                                                      0x1: 1.70V ；
                                                      0x2: 1.75V ；
                                                      0x3: 1.80V ；
                                                      0x4: 1.85V ；
                                                      0x5: 1.90V ；
                                                      0x6: 1.95V ；
                                                      0x7：2.00V 。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo16 : 3;  /* bit[4-6]: LDO16负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO16_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO16_REG_ADJ_vset_ldo16_START   (0)
#define SOC_SMART_LDO16_REG_ADJ_vset_ldo16_END     (2)
#define SOC_SMART_LDO16_REG_ADJ_vrset_ldo16_START  (4)
#define SOC_SMART_LDO16_REG_ADJ_vrset_ldo16_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO17_REG_ADJ_UNION
 结构说明  : LDO17_REG_ADJ 寄存器结构定义。地址偏移量:0x082，初值:0x10，宽度:8
 寄存器说明: LDO17调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo17  : 3;  /* bit[0-2]: LDO17电压调节：
                                                      0x0: 2.50V ；
                                                      0x1: 2.60V ；
                                                      0x2: 2.70V ；
                                                      0x3: 2.80V ；
                                                      0x4: 2.90V ；
                                                      0x5: 3.00V ；
                                                      0x6: 3.10V ；
                                                      0x7: 3.20V。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo17 : 3;  /* bit[4-6]: LDO17负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO17_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO17_REG_ADJ_vset_ldo17_START   (0)
#define SOC_SMART_LDO17_REG_ADJ_vset_ldo17_END     (2)
#define SOC_SMART_LDO17_REG_ADJ_vrset_ldo17_START  (4)
#define SOC_SMART_LDO17_REG_ADJ_vrset_ldo17_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO18_REG_ADJ_UNION
 结构说明  : LDO18_REG_ADJ 寄存器结构定义。地址偏移量:0x083，初值:0x24，宽度:8
 寄存器说明: LDO18调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo18  : 3;  /* bit[0-2]: LDO18电压调节：
                                                      0x0: 1.60V ；
                                                      0x1: 1.70V ；
                                                      0x2: 1.75V ；
                                                      0x3: 1.8V ；
                                                      0x4: 1.825V ；
                                                      0x5: 1.85V ；
                                                      0x6: 1.90V ；
                                                      0x7: 1.95V 。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo18 : 3;  /* bit[4-6]: LDO18负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO18_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO18_REG_ADJ_vset_ldo18_START   (0)
#define SOC_SMART_LDO18_REG_ADJ_vset_ldo18_END     (2)
#define SOC_SMART_LDO18_REG_ADJ_vrset_ldo18_START  (4)
#define SOC_SMART_LDO18_REG_ADJ_vrset_ldo18_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO19_REG_ADJ_UNION
 结构说明  : LDO19_REG_ADJ 寄存器结构定义。地址偏移量:0x084，初值:0x25，宽度:8
 寄存器说明: LDO19调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo19  : 3;  /* bit[0-2]: LDO19电压调节：
                                                      0x0: 1.80V ；
                                                      0x1: 1.85V ；
                                                      0x2: 1.90V ；
                                                      0x3: 2.75V ；
                                                      0x4: 2.80V ；
                                                      0x5: 2.85V ；
                                                      0x6: 2.90V ；
                                                      0x7: 3.00V 。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo19 : 3;  /* bit[4-6]: LDO19负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO19_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO19_REG_ADJ_vset_ldo19_START   (0)
#define SOC_SMART_LDO19_REG_ADJ_vset_ldo19_END     (2)
#define SOC_SMART_LDO19_REG_ADJ_vrset_ldo19_START  (4)
#define SOC_SMART_LDO19_REG_ADJ_vrset_ldo19_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO20_REG_ADJ_UNION
 结构说明  : LDO20_REG_ADJ 寄存器结构定义。地址偏移量:0x085，初值:0x10，宽度:8
 寄存器说明: LDO20调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo20  : 3;  /* bit[0-2]: LDO20电压调节：
                                                      0x0:1.80V ；
                                                      0x1: 1.85V ；
                                                      0x2: 2.85V ；
                                                      0x3: 2.9V ；
                                                      0x4: 3.0V ；
                                                      0x5: 3.1V ；
                                                      0x6: 3.20V；
                                                      0x7: 3.3V。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo20 : 3;  /* bit[4-6]: LDO20负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO20_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO20_REG_ADJ_vset_ldo20_START   (0)
#define SOC_SMART_LDO20_REG_ADJ_vset_ldo20_END     (2)
#define SOC_SMART_LDO20_REG_ADJ_vrset_ldo20_START  (4)
#define SOC_SMART_LDO20_REG_ADJ_vrset_ldo20_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO21_REG_ADJ_UNION
 结构说明  : LDO21_REG_ADJ 寄存器结构定义。地址偏移量:0x086，初值:0x23，宽度:8
 寄存器说明: LDO21调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo21  : 3;  /* bit[0-2]: LDO21电压调节：
                                                      0x0: 1.65V ；
                                                      0x1: 1.70V ；
                                                      0x2: 1.75V ；
                                                      0x3: 1.80V ；
                                                      0x4: 1.85V ；
                                                      0x5: 1.90V ；
                                                      0x6: 1.95V ；
                                                      0x7：2.00V 。 */
        unsigned char  reserved_0  : 1;  /* bit[3]  : 保留 */
        unsigned char  vrset_ldo21 : 3;  /* bit[4-6]: LDO21负载调整率补偿调整，5mV / Step */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO21_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO21_REG_ADJ_vset_ldo21_START   (0)
#define SOC_SMART_LDO21_REG_ADJ_vset_ldo21_END     (2)
#define SOC_SMART_LDO21_REG_ADJ_vrset_ldo21_START  (4)
#define SOC_SMART_LDO21_REG_ADJ_vrset_ldo21_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_LDO22_REG_ADJ_UNION
 结构说明  : LDO22_REG_ADJ 寄存器结构定义。地址偏移量:0x087，初值:0x20，宽度:8
 寄存器说明: LDO22调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo22  : 3;  /* bit[0-2]: LDO22电压调节：
                                                      0x0: 0.90V ；
                                                      0x1: 1.00V ；
                                                      0x2: 1.05V ；
                                                      0x3: 1.10V ；
                                                      0x4: 1.15V ；
                                                      0x5: 1.175V；
                                                      0x6：1.1875V；
                                                      0x7：1.2V */
        unsigned char  en_dc_ldo22 : 1;  /* bit[3]  : LDO22 强制常通模式，0表示非常通模式，1表示常通模式 */
        unsigned char  vrset_ldo22 : 3;  /* bit[4-6]: LDO22负载调整率补偿调整，5mV / Step */
        unsigned char  reserved    : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_LDO22_REG_ADJ_UNION;
#endif
#define SOC_SMART_LDO22_REG_ADJ_vset_ldo22_START   (0)
#define SOC_SMART_LDO22_REG_ADJ_vset_ldo22_END     (2)
#define SOC_SMART_LDO22_REG_ADJ_en_dc_ldo22_START  (3)
#define SOC_SMART_LDO22_REG_ADJ_en_dc_ldo22_END    (3)
#define SOC_SMART_LDO22_REG_ADJ_vrset_ldo22_START  (4)
#define SOC_SMART_LDO22_REG_ADJ_vrset_ldo22_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_PMUA_REG_ADJ_UNION
 结构说明  : PMUA_REG_ADJ 寄存器结构定义。地址偏移量:0x088，初值:0x01，宽度:8
 寄存器说明: PMUA调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_pmua : 2;  /* bit[0-1]: PMUA电压调节：
                                                    0x0: 1.70V ；
                                                    0x1: 1.80V ；
                                                    0x2: 1.90V ；
                                                    0x3: 2.00V ； */
        unsigned char  reserved  : 6;  /* bit[2-7]: 保留 */
    } reg;
} SOC_SMART_PMUA_REG_ADJ_UNION;
#endif
#define SOC_SMART_PMUA_REG_ADJ_vset_pmua_START  (0)
#define SOC_SMART_PMUA_REG_ADJ_vset_pmua_END    (1)


/*****************************************************************************
 结构名    : SOC_SMART_LVS_ADJ1_UNION
 结构说明  : LVS_ADJ1 寄存器结构定义。地址偏移量:0x089，初值:0x01，宽度:8
 寄存器说明: LVS调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sstset_lvs2 : 2;  /* bit[0-1]: LVS2缓启速度调节，11最快，00最慢 */
        unsigned char  reserved    : 6;  /* bit[2-7]: 保留 */
    } reg;
} SOC_SMART_LVS_ADJ1_UNION;
#endif
#define SOC_SMART_LVS_ADJ1_sstset_lvs2_START  (0)
#define SOC_SMART_LVS_ADJ1_sstset_lvs2_END    (1)


/*****************************************************************************
 结构名    : SOC_SMART_BANDGAP_THSD_ADJ1_UNION
 结构说明  : BANDGAP_THSD_ADJ1 寄存器结构定义。地址偏移量:0x08A，初值:0x00，宽度:8
 寄存器说明: BANDGAP和THSD调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ibias_trim_en : 2;  /* bit[0-1]: 基准电流trim使能信号 */
        unsigned char  ref_reserve   : 2;  /* bit[2-3]: 基准备用寄存器 */
        unsigned char  reserved      : 4;  /* bit[4-7]: 保留 */
    } reg;
} SOC_SMART_BANDGAP_THSD_ADJ1_UNION;
#endif
#define SOC_SMART_BANDGAP_THSD_ADJ1_ibias_trim_en_START  (0)
#define SOC_SMART_BANDGAP_THSD_ADJ1_ibias_trim_en_END    (1)
#define SOC_SMART_BANDGAP_THSD_ADJ1_ref_reserve_START    (2)
#define SOC_SMART_BANDGAP_THSD_ADJ1_ref_reserve_END      (3)


/*****************************************************************************
 结构名    : SOC_SMART_BANDGAP_THSD_ADJ2_UNION
 结构说明  : BANDGAP_THSD_ADJ2 寄存器结构定义。地址偏移量:0x08B，初值:0x26，宽度:8
 寄存器说明: BANDGAP和THSD调节2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_tmp_int      : 2;  /* bit[0-1]: 0x:THSD过温保护模块关闭；
                                                          10:THSD过温保护模块开启。
                                                          11:THSD的低功耗模式。高电平为10ms,周期为1s的周期信号 */
        unsigned char  en_eco_thsd_int : 1;  /* bit[2]  : 设置TSHD是否随PMU进出ECO模式:
                                                          0:不跟随；
                                                          1：跟随。 */
        unsigned char  reserved_0      : 1;  /* bit[3]  : 保留 */
        unsigned char  thsd_set_tmp    : 2;  /* bit[4-5]: 温度保护模块报警温度设置。
                                                          00: 105℃；
                                                          01: 115℃；
                                                          10: 125℃；
                                                          默认设置为125℃。 */
        unsigned char  reserved_1      : 2;  /* bit[6-7]: 保留 */
    } reg;
} SOC_SMART_BANDGAP_THSD_ADJ2_UNION;
#endif
#define SOC_SMART_BANDGAP_THSD_ADJ2_en_tmp_int_START       (0)
#define SOC_SMART_BANDGAP_THSD_ADJ2_en_tmp_int_END         (1)
#define SOC_SMART_BANDGAP_THSD_ADJ2_en_eco_thsd_int_START  (2)
#define SOC_SMART_BANDGAP_THSD_ADJ2_en_eco_thsd_int_END    (2)
#define SOC_SMART_BANDGAP_THSD_ADJ2_thsd_set_tmp_START     (4)
#define SOC_SMART_BANDGAP_THSD_ADJ2_thsd_set_tmp_END       (5)


/*****************************************************************************
 结构名    : SOC_SMART_DR_FLA_CTRL1_UNION
 结构说明  : DR_FLA_CTRL1 寄存器结构定义。地址偏移量:0x08C，初值:0x00，宽度:8
 寄存器说明: DR闪烁模式控制1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_dr3_int : 1;  /* bit[0]  : 0: 关闭DR3；
                                                     1: 开启DR3。 */
        unsigned char  dr3_mode   : 1;  /* bit[1]  : 0: 关闭闪动模式(此时表现为持续输出)；
                                                     1: 开启DR3闪动模式。 */
        unsigned char  en_dr4_int : 1;  /* bit[2]  : 0: 关闭DR4；
                                                     1: 开启DR4。 */
        unsigned char  dr4_mode   : 1;  /* bit[3]  : 0: 关闭闪动模式(此时表现为持续输出)；
                                                     1: 开启DR4闪动模式。 */
        unsigned char  en_dr5_int : 1;  /* bit[4]  : 0: 关闭DR5；
                                                     1: 开启DR5。 */
        unsigned char  dr5_mode   : 1;  /* bit[5]  : 0: 关闭闪动模式(此时表现为持续输出)；
                                                     1: 开启DR5闪动模式。 */
        unsigned char  reserved   : 2;  /* bit[6-7]: 保留。 */
    } reg;
} SOC_SMART_DR_FLA_CTRL1_UNION;
#endif
#define SOC_SMART_DR_FLA_CTRL1_en_dr3_int_START  (0)
#define SOC_SMART_DR_FLA_CTRL1_en_dr3_int_END    (0)
#define SOC_SMART_DR_FLA_CTRL1_dr3_mode_START    (1)
#define SOC_SMART_DR_FLA_CTRL1_dr3_mode_END      (1)
#define SOC_SMART_DR_FLA_CTRL1_en_dr4_int_START  (2)
#define SOC_SMART_DR_FLA_CTRL1_en_dr4_int_END    (2)
#define SOC_SMART_DR_FLA_CTRL1_dr4_mode_START    (3)
#define SOC_SMART_DR_FLA_CTRL1_dr4_mode_END      (3)
#define SOC_SMART_DR_FLA_CTRL1_en_dr5_int_START  (4)
#define SOC_SMART_DR_FLA_CTRL1_en_dr5_int_END    (4)
#define SOC_SMART_DR_FLA_CTRL1_dr5_mode_START    (5)
#define SOC_SMART_DR_FLA_CTRL1_dr5_mode_END      (5)


/*****************************************************************************
 结构名    : SOC_SMART_DR_FLA_CTRL2_UNION
 结构说明  : DR_FLA_CTRL2 寄存器结构定义。地址偏移量:0x08D，初值:0x00，宽度:8
 寄存器说明: DR闪烁模式控制2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  en_dr1_int : 1;  /* bit[0]  : 0: 关闭DR1；
                                                     1: 开启DR1 。 */
        unsigned char  dr1_mode   : 1;  /* bit[1]  : 0: 关闭闪动模式(此时表现为持续输出)；
                                                     1: 开启DR1闪动模式。 */
        unsigned char  reserved_0 : 2;  /* bit[2-3]: 保留 */
        unsigned char  en_dr2_int : 1;  /* bit[4]  : 0: 关闭DR2；
                                                     1: 开启DR2。 */
        unsigned char  dr2_mode   : 1;  /* bit[5]  : 0: 关闭闪动模式(此时表现为持续输出)；
                                                     1: 开启DR2闪动模式。 */
        unsigned char  reserved_1 : 2;  /* bit[6-7]: 保留。 */
    } reg;
} SOC_SMART_DR_FLA_CTRL2_UNION;
#endif
#define SOC_SMART_DR_FLA_CTRL2_en_dr1_int_START  (0)
#define SOC_SMART_DR_FLA_CTRL2_en_dr1_int_END    (0)
#define SOC_SMART_DR_FLA_CTRL2_dr1_mode_START    (1)
#define SOC_SMART_DR_FLA_CTRL2_dr1_mode_END      (1)
#define SOC_SMART_DR_FLA_CTRL2_en_dr2_int_START  (4)
#define SOC_SMART_DR_FLA_CTRL2_en_dr2_int_END    (4)
#define SOC_SMART_DR_FLA_CTRL2_dr2_mode_START    (5)
#define SOC_SMART_DR_FLA_CTRL2_dr2_mode_END      (5)


/*****************************************************************************
 结构名    : SOC_SMART_FLASH_PERIOD_DR12_UNION
 结构说明  : FLASH_PERIOD_DR12 寄存器结构定义。地址偏移量:0x08E，初值:0x3F，宽度:8
 寄存器说明: DR闪烁模式周期调节0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  flash_period_dr12 : 8;  /* bit[0-7]: 闪烁周期T = 配置值*31.25ms。 */
    } reg;
} SOC_SMART_FLASH_PERIOD_DR12_UNION;
#endif
#define SOC_SMART_FLASH_PERIOD_DR12_flash_period_dr12_START  (0)
#define SOC_SMART_FLASH_PERIOD_DR12_flash_period_dr12_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_FLASH_ON_DR12_UNION
 结构说明  : FLASH_ON_DR12 寄存器结构定义。地址偏移量:0x08F，初值:0x1F，宽度:8
 寄存器说明: DR闪烁模式点亮时间调节0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  flash_on_dr12 : 8;  /* bit[0-7]: 点亮时间t =配置值*7.8125ms。 */
    } reg;
} SOC_SMART_FLASH_ON_DR12_UNION;
#endif
#define SOC_SMART_FLASH_ON_DR12_flash_on_dr12_START  (0)
#define SOC_SMART_FLASH_ON_DR12_flash_on_dr12_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_FLASH_PERIOD_DR345_UNION
 结构说明  : FLASH_PERIOD_DR345 寄存器结构定义。地址偏移量:0x090，初值:0x3F，宽度:8
 寄存器说明: DR闪烁模式周期调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  flash_period_dr345 : 8;  /* bit[0-7]: 闪烁周期T = 配置值*31.25ms。 */
    } reg;
} SOC_SMART_FLASH_PERIOD_DR345_UNION;
#endif
#define SOC_SMART_FLASH_PERIOD_DR345_flash_period_dr345_START  (0)
#define SOC_SMART_FLASH_PERIOD_DR345_flash_period_dr345_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_FLASH_ON_DR345_UNION
 结构说明  : FLASH_ON_DR345 寄存器结构定义。地址偏移量:0x091，初值:0x1F，宽度:8
 寄存器说明: DR闪烁模式点亮时间调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  flash_on_dr345 : 8;  /* bit[0-7]: 点亮时间t =配置值*7.8125ms。 */
    } reg;
} SOC_SMART_FLASH_ON_DR345_UNION;
#endif
#define SOC_SMART_FLASH_ON_DR345_flash_on_dr345_START  (0)
#define SOC_SMART_FLASH_ON_DR345_flash_on_dr345_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_DR_MODE_SEL_UNION
 结构说明  : DR_MODE_SEL 寄存器结构定义。地址偏移量:0x092，初值:0x00，宽度:8
 寄存器说明: DR灯模式选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr1_mode_sel : 1;  /* bit[0]  : 0: DR1工作在呼吸功能模式；
                                                       1：DR1工作在闪烁模式； */
        unsigned char  dr2_mode_sel : 1;  /* bit[1]  : 0: DR2工作在呼吸功能模式；
                                                       1：DR2工作在闪烁模式； */
        unsigned char  dr3_mode_sel : 1;  /* bit[2]  : 0: DR3工作在呼吸功能模式；
                                                       1：DR3工作在闪烁模式； */
        unsigned char  dr4_mode_sel : 1;  /* bit[3]  : 0: DR4工作在呼吸功能模式；
                                                       1：DR4工作在闪烁模式； */
        unsigned char  dr5_mode_sel : 1;  /* bit[4]  : 0: DR5工作在呼吸功能模式；
                                                       1：DR5工作在闪烁模式； */
        unsigned char  reserved     : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_DR_MODE_SEL_UNION;
#endif
#define SOC_SMART_DR_MODE_SEL_dr1_mode_sel_START  (0)
#define SOC_SMART_DR_MODE_SEL_dr1_mode_sel_END    (0)
#define SOC_SMART_DR_MODE_SEL_dr2_mode_sel_START  (1)
#define SOC_SMART_DR_MODE_SEL_dr2_mode_sel_END    (1)
#define SOC_SMART_DR_MODE_SEL_dr3_mode_sel_START  (2)
#define SOC_SMART_DR_MODE_SEL_dr3_mode_sel_END    (2)
#define SOC_SMART_DR_MODE_SEL_dr4_mode_sel_START  (3)
#define SOC_SMART_DR_MODE_SEL_dr4_mode_sel_END    (3)
#define SOC_SMART_DR_MODE_SEL_dr5_mode_sel_START  (4)
#define SOC_SMART_DR_MODE_SEL_dr5_mode_sel_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_DR_BRE_CTRL_UNION
 结构说明  : DR_BRE_CTRL 寄存器结构定义。地址偏移量:0x093，初值:0x00，宽度:8
 寄存器说明: DR1/2呼吸模式控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr1_en       : 1;  /* bit[0]  : 0：dr1关闭；
                                                       1：dr1使能：
                                                       dr1_flash_en = 1,闪烁；
                                                       dr1_flash_en = 0,常亮。 */
        unsigned char  dr1_flash_en : 1;  /* bit[1]  : 0：dr1不闪烁。
                                                       1：dr1闪烁 */
        unsigned char  reserved_0   : 2;  /* bit[2-3]: 保留 */
        unsigned char  dr2_en       : 1;  /* bit[4]  : 0：dr2关闭；
                                                       1：dr2使能：
                                                       dr2_flash_en = 1,闪烁；
                                                       dr2_flash_en = 0,常亮。 */
        unsigned char  dr2_flash_en : 1;  /* bit[5]  : 0：dr2不闪烁。
                                                       1：dr2闪烁 */
        unsigned char  reserved_1   : 2;  /* bit[6-7]: 保留 */
    } reg;
} SOC_SMART_DR_BRE_CTRL_UNION;
#endif
#define SOC_SMART_DR_BRE_CTRL_dr1_en_START        (0)
#define SOC_SMART_DR_BRE_CTRL_dr1_en_END          (0)
#define SOC_SMART_DR_BRE_CTRL_dr1_flash_en_START  (1)
#define SOC_SMART_DR_BRE_CTRL_dr1_flash_en_END    (1)
#define SOC_SMART_DR_BRE_CTRL_dr2_en_START        (4)
#define SOC_SMART_DR_BRE_CTRL_dr2_en_END          (4)
#define SOC_SMART_DR_BRE_CTRL_dr2_flash_en_START  (5)
#define SOC_SMART_DR_BRE_CTRL_dr2_flash_en_END    (5)


/*****************************************************************************
 结构名    : SOC_SMART_DR12_TIM_CONF0_UNION
 结构说明  : DR12_TIM_CONF0 寄存器结构定义。地址偏移量:0x094，初值:0x33，宽度:8
 寄存器说明: DR1和DR2常亮常暗时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr12_t_off : 3;  /* bit[0-2]: 000：1ms
                                                     001：0.25s
                                                     010：0.5s
                                                     011：1s
                                                     100：2s
                                                     101：4s
                                                     110：4s
                                                     111：长暗 */
        unsigned char  reserved_0 : 1;  /* bit[3]  : 保留 */
        unsigned char  dr12_t_on  : 3;  /* bit[4-6]: 000：1ms
                                                     001：0.25s
                                                     010：0.5s
                                                     011：1s
                                                     100：2s
                                                     101：4s
                                                     110：4s
                                                     111：长亮 */
        unsigned char  reserved_1 : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_DR12_TIM_CONF0_UNION;
#endif
#define SOC_SMART_DR12_TIM_CONF0_dr12_t_off_START  (0)
#define SOC_SMART_DR12_TIM_CONF0_dr12_t_off_END    (2)
#define SOC_SMART_DR12_TIM_CONF0_dr12_t_on_START   (4)
#define SOC_SMART_DR12_TIM_CONF0_dr12_t_on_END     (6)


/*****************************************************************************
 结构名    : SOC_SMART_DR12_TIM_CONF1_UNION
 结构说明  : DR12_TIM_CONF1 寄存器结构定义。地址偏移量:0x095，初值:0x33，宽度:8
 寄存器说明: DR1和DR2渐亮渐暗时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr12_t_rise : 3;  /* bit[0-2]: 000：0s
                                                      001：0.25s
                                                      010：0.5s
                                                      011：1s
                                                      100：2s
                                                      101：2.5s
                                                      110：3s
                                                      111：4s */
        unsigned char  reserved_0  : 1;  /* bit[3]  :  */
        unsigned char  dr12_t_fall : 3;  /* bit[4-6]: 000：0s
                                                      001：0.25s
                                                      010：0.5s
                                                      011：1s
                                                      100：2s
                                                      101：2.5s
                                                      110：3s
                                                      111：4s */
        unsigned char  reserved_1  : 1;  /* bit[7]  : 保留 */
    } reg;
} SOC_SMART_DR12_TIM_CONF1_UNION;
#endif
#define SOC_SMART_DR12_TIM_CONF1_dr12_t_rise_START  (0)
#define SOC_SMART_DR12_TIM_CONF1_dr12_t_rise_END    (2)
#define SOC_SMART_DR12_TIM_CONF1_dr12_t_fall_START  (4)
#define SOC_SMART_DR12_TIM_CONF1_dr12_t_fall_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_DR1_ISET_UNION
 结构说明  : DR1_ISET 寄存器结构定义。地址偏移量:0x096，初值:0x00，宽度:8
 寄存器说明: DR1电流选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  iset_dr1 : 3;  /* bit[0-2]: DR1电流调节。000: 1 mA；
                                                   001: 3 mA；
                                                   010: 5 mA；
                                                   011: 7 mA；
                                                   100: 9 mA；
                                                   101: 11 mA；
                                                   110: 13 mA；
                                                   111: 15 mA； */
        unsigned char  reserved : 5;  /* bit[3-7]: 保留 */
    } reg;
} SOC_SMART_DR1_ISET_UNION;
#endif
#define SOC_SMART_DR1_ISET_iset_dr1_START  (0)
#define SOC_SMART_DR1_ISET_iset_dr1_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_DR2_ISET_UNION
 结构说明  : DR2_ISET 寄存器结构定义。地址偏移量:0x097，初值:0x00，宽度:8
 寄存器说明: DR2电流选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  iset_dr2 : 3;  /* bit[0-2]: DR2电流调节。000: 1 mA；
                                                   001: 3 mA；
                                                   010: 5 mA；
                                                   011: 7 mA；
                                                   100: 9 mA；
                                                   101: 11 mA；
                                                   110: 13 mA；
                                                   111: 15 mA； */
        unsigned char  reserved : 5;  /* bit[3-7]: 保留 */
    } reg;
} SOC_SMART_DR2_ISET_UNION;
#endif
#define SOC_SMART_DR2_ISET_iset_dr2_START  (0)
#define SOC_SMART_DR2_ISET_iset_dr2_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_DR_LED_CTRL_UNION
 结构说明  : DR_LED_CTRL 寄存器结构定义。地址偏移量:0x098，初值:0x00，宽度:8
 寄存器说明: DR3/4/5控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr3_en   : 1;  /* bit[0]  : DR3使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned char  dr4_en   : 1;  /* bit[1]  : DR4使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned char  dr5_en   : 1;  /* bit[2]  : DR5使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned char  reserved : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_SMART_DR_LED_CTRL_UNION;
#endif
#define SOC_SMART_DR_LED_CTRL_dr3_en_START    (0)
#define SOC_SMART_DR_LED_CTRL_dr3_en_END      (0)
#define SOC_SMART_DR_LED_CTRL_dr4_en_START    (1)
#define SOC_SMART_DR_LED_CTRL_dr4_en_END      (1)
#define SOC_SMART_DR_LED_CTRL_dr5_en_START    (2)
#define SOC_SMART_DR_LED_CTRL_dr5_en_END      (2)


/*****************************************************************************
 结构名    : SOC_SMART_DR_OUT_CTRL_UNION
 结构说明  : DR_OUT_CTRL 寄存器结构定义。地址偏移量:0x099，初值:0x00，宽度:8
 寄存器说明: DR3/4/5输出控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr3_out_ctrl : 2;  /* bit[0-1]: DR3输出控制。
                                                       00：DR3的配置项输出到DR3管脚；
                                                       01：DR3与DR4相或后的配置项输出到DR3管脚；
                                                       10：DR3与DR4、DR5相或后的配置项输出到DR3管脚；
                                                       11：DR4与DR5相或后的配置项输出到DR3管脚。 */
        unsigned char  dr4_out_ctrl : 2;  /* bit[2-3]: DR4输出控制。
                                                       00：DR4的配置项输出到DR4管脚；
                                                       01：DR3与DR4相或后的配置项输出到DR4管脚；
                                                       10：DR3与DR4、DR5相或后的配置项输出到DR4管脚；
                                                       11：DR4与DR5相或后的配置项输出到DR4管脚。 */
        unsigned char  dr5_out_ctrl : 2;  /* bit[4-5]: DR5输出控制。
                                                       00：DR5的配置项输出到DR5管脚；
                                                       01：DR3与DR4相或后的配置项输出到DR5管脚；
                                                       10：DR3与DR4、DR5相或后的配置项输出到DR5管脚；
                                                       11：DR4与DR5相或后的配置项输出到DR5管脚。 */
        unsigned char  reserved     : 2;  /* bit[6-7]: 保留。 */
    } reg;
} SOC_SMART_DR_OUT_CTRL_UNION;
#endif
#define SOC_SMART_DR_OUT_CTRL_dr3_out_ctrl_START  (0)
#define SOC_SMART_DR_OUT_CTRL_dr3_out_ctrl_END    (1)
#define SOC_SMART_DR_OUT_CTRL_dr4_out_ctrl_START  (2)
#define SOC_SMART_DR_OUT_CTRL_dr4_out_ctrl_END    (3)
#define SOC_SMART_DR_OUT_CTRL_dr5_out_ctrl_START  (4)
#define SOC_SMART_DR_OUT_CTRL_dr5_out_ctrl_END    (5)


/*****************************************************************************
 结构名    : SOC_SMART_DR3_ISET_UNION
 结构说明  : DR3_ISET 寄存器结构定义。地址偏移量:0x09A，初值:0x00，宽度:8
 寄存器说明: DR3电流选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  iset_dr3 : 3;  /* bit[0-2]: R3输出电流调节（mA）：
                                                   0x0 : 1.0；
                                                   0x1 : 1.5；
                                                   0x2 : 2.0；
                                                   0x3 : 2.5；
                                                   0x4 : 3.0；
                                                   0x5 : 3.5；
                                                   0x6 : 4.0；
                                                   0x7 : 4.5。 */
        unsigned char  reserved : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_SMART_DR3_ISET_UNION;
#endif
#define SOC_SMART_DR3_ISET_iset_dr3_START  (0)
#define SOC_SMART_DR3_ISET_iset_dr3_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_DR3_START_DEL_UNION
 结构说明  : DR3_START_DEL 寄存器结构定义。地址偏移量:0x09B，初值:0x10，宽度:8
 寄存器说明: DR3启动延时配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr3_start_delay : 8;  /* bit[0-7]: DR3启动延时范围：0～32768 ms，step：125ms。 */
    } reg;
} SOC_SMART_DR3_START_DEL_UNION;
#endif
#define SOC_SMART_DR3_START_DEL_dr3_start_delay_START  (0)
#define SOC_SMART_DR3_START_DEL_dr3_start_delay_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_DR4_ISET_UNION
 结构说明  : DR4_ISET 寄存器结构定义。地址偏移量:0x09C，初值:0x00，宽度:8
 寄存器说明: DR4电流选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  iset_dr4 : 3;  /* bit[0-2]: DR4输出电流调节（mA）：
                                                   0x0 : 1.0；
                                                   0x1 : 1.5；
                                                   0x2 : 2.0；
                                                   0x3 : 2.5；
                                                   0x4 : 3.0；
                                                   0x5 : 3.5；
                                                   0x6 : 4.0；
                                                   0x7 : 4.5。 */
        unsigned char  reserved : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_SMART_DR4_ISET_UNION;
#endif
#define SOC_SMART_DR4_ISET_iset_dr4_START  (0)
#define SOC_SMART_DR4_ISET_iset_dr4_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_DR4_START_DEL_UNION
 结构说明  : DR4_START_DEL 寄存器结构定义。地址偏移量:0x09D，初值:0x10，宽度:8
 寄存器说明: DR4启动延时配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr4_start_delay : 8;  /* bit[0-7]: DR4启动延时范围：0～32768 ms，step：125ms。 */
    } reg;
} SOC_SMART_DR4_START_DEL_UNION;
#endif
#define SOC_SMART_DR4_START_DEL_dr4_start_delay_START  (0)
#define SOC_SMART_DR4_START_DEL_dr4_start_delay_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_DR5_ISET_UNION
 结构说明  : DR5_ISET 寄存器结构定义。地址偏移量:0x09E，初值:0x00，宽度:8
 寄存器说明: DR5电流选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  iset_dr5 : 3;  /* bit[0-2]: DR5输出电流调节（mA）：
                                                   0x0 : 1.0；
                                                   0x1 : 1.5；
                                                   0x2 : 2.0；
                                                   0x3 : 2.5；
                                                   0x4 : 3.0；
                                                   0x5 : 3.5；
                                                   0x6 : 4.0；
                                                   0x7 : 4.5。 */
        unsigned char  reserved : 5;  /* bit[3-7]: 保留。 */
    } reg;
} SOC_SMART_DR5_ISET_UNION;
#endif
#define SOC_SMART_DR5_ISET_iset_dr5_START  (0)
#define SOC_SMART_DR5_ISET_iset_dr5_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_DR5_START_DEL_UNION
 结构说明  : DR5_START_DEL 寄存器结构定义。地址偏移量:0x09F，初值:0x10，宽度:8
 寄存器说明: DR5启动延时配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr5_start_delay : 8;  /* bit[0-7]: DR5启动延时范围：0～32768 ms，step：125ms。 */
    } reg;
} SOC_SMART_DR5_START_DEL_UNION;
#endif
#define SOC_SMART_DR5_START_DEL_dr5_start_delay_START  (0)
#define SOC_SMART_DR5_START_DEL_dr5_start_delay_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_DR3_TIM_CONF0_UNION
 结构说明  : DR3_TIM_CONF0 寄存器结构定义。地址偏移量:0x0A0，初值:0x44，宽度:8
 寄存器说明: DR3常亮常暗时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr345_t_off : 4;  /* bit[0-3]: DR345常亮时间配置。
                                                      000：0s；
                                                      001：0.5s；
                                                      010：1s；
                                                      011：2s；
                                                      100：4s；
                                                      101：6s；
                                                      110：8s；
                                                      111：12s；
                                                      1000：14；
                                                      1001：16；
                                                      其他：常暗。 */
        unsigned char  dr345_t_on  : 4;  /* bit[4-7]: DR345常亮时间配置。
                                                      0000：0s；
                                                      0001：0.5s；
                                                      0010：1s；
                                                      0011：2s；
                                                      0100：4s；
                                                      0101：6s；
                                                      0110：8s；
                                                      0111：12s；
                                                      1000：14；
                                                      1001：16；
                                                      其他：常亮。 */
    } reg;
} SOC_SMART_DR3_TIM_CONF0_UNION;
#endif
#define SOC_SMART_DR3_TIM_CONF0_dr345_t_off_START  (0)
#define SOC_SMART_DR3_TIM_CONF0_dr345_t_off_END    (3)
#define SOC_SMART_DR3_TIM_CONF0_dr345_t_on_START   (4)
#define SOC_SMART_DR3_TIM_CONF0_dr345_t_on_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_DR3_TIM_CONF1_UNION
 结构说明  : DR3_TIM_CONF1 寄存器结构定义。地址偏移量:0x0A1，初值:0x33，宽度:8
 寄存器说明: DR3渐亮渐暗时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  dr345_t_rise : 3;  /* bit[0-2]: DR345渐暗时间配置。
                                                       000：0s；
                                                       001：0.25s；
                                                       010：0.5s；
                                                       011：1s；
                                                       100：2s；
                                                       101：2.5s；
                                                       110:3s；
                                                       111：4s； */
        unsigned char  reserved_0   : 1;  /* bit[3]  : 保留。 */
        unsigned char  dr345_t_fall : 3;  /* bit[4-6]: DR345渐暗时间配置。
                                                       000：0s；
                                                       001：0.25s；
                                                       010：0.5s；
                                                       011：1s；
                                                       100：2s；
                                                       101：2.5s；
                                                       110:3s；
                                                       111：4s； */
        unsigned char  reserved_1   : 1;  /* bit[7]  : 保留。 */
    } reg;
} SOC_SMART_DR3_TIM_CONF1_UNION;
#endif
#define SOC_SMART_DR3_TIM_CONF1_dr345_t_rise_START  (0)
#define SOC_SMART_DR3_TIM_CONF1_dr345_t_rise_END    (2)
#define SOC_SMART_DR3_TIM_CONF1_dr345_t_fall_START  (4)
#define SOC_SMART_DR3_TIM_CONF1_dr345_t_fall_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_OTP_CTRL1_UNION
 结构说明  : OTP_CTRL1 寄存器结构定义。地址偏移量:0x0A2，初值:0x00，宽度:8
 寄存器说明: OTP控制1信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pwe_int    : 1;  /* bit[0]  : OTP写控制信号。 */
        unsigned char  otp_pwe_pulse  : 1;  /* bit[1]  : 0：不发起脉冲；
                                                         1：当otp_write_mask=1时，发起一个310us的高电平脉冲。 */
        unsigned char  otp_write_mask : 1;  /* bit[2]  : 0:由otp_pwe信号控制OTP的写操作。
                                                         1:由otp_pwe_pulse信号控制OTP的写操作； */
        unsigned char  otp_por_int    : 1;  /* bit[3]  : OTP读控制信号。
                                                         0: otp不读；
                                                         1：自动将32bits otp值读回到非下电寄存器保存。 */
        unsigned char  reserved       : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_SMART_OTP_CTRL1_UNION;
#endif
#define SOC_SMART_OTP_CTRL1_otp_pwe_int_START     (0)
#define SOC_SMART_OTP_CTRL1_otp_pwe_int_END       (0)
#define SOC_SMART_OTP_CTRL1_otp_pwe_pulse_START   (1)
#define SOC_SMART_OTP_CTRL1_otp_pwe_pulse_END     (1)
#define SOC_SMART_OTP_CTRL1_otp_write_mask_START  (2)
#define SOC_SMART_OTP_CTRL1_otp_write_mask_END    (2)
#define SOC_SMART_OTP_CTRL1_otp_por_int_START     (3)
#define SOC_SMART_OTP_CTRL1_otp_por_int_END       (3)


/*****************************************************************************
 结构名    : SOC_SMART_OTP_CTRL2_UNION
 结构说明  : OTP_CTRL2 寄存器结构定义。地址偏移量:0x0A3，初值:0x00，宽度:8
 寄存器说明: OTP控制2信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pa    : 2;  /* bit[0-1]: OTP的地址信号。 */
        unsigned char  otp_ptm   : 2;  /* bit[2-3]: 测试模式使能信号。 */
        unsigned char  otp_pprog : 1;  /* bit[4]  : 编程模式使能信号。 */
        unsigned char  reserved  : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_OTP_CTRL2_UNION;
#endif
#define SOC_SMART_OTP_CTRL2_otp_pa_START     (0)
#define SOC_SMART_OTP_CTRL2_otp_pa_END       (1)
#define SOC_SMART_OTP_CTRL2_otp_ptm_START    (2)
#define SOC_SMART_OTP_CTRL2_otp_ptm_END      (3)
#define SOC_SMART_OTP_CTRL2_otp_pprog_START  (4)
#define SOC_SMART_OTP_CTRL2_otp_pprog_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_OTP_PDIN_UNION
 结构说明  : OTP_PDIN 寄存器结构定义。地址偏移量:0x0A4，初值:0x00，宽度:8
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
} SOC_SMART_OTP_PDIN_UNION;
#endif
#define SOC_SMART_OTP_PDIN_otp_pdin_START  (0)
#define SOC_SMART_OTP_PDIN_otp_pdin_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ANA_IN_UNION
 结构说明  : ANA_IN 寄存器结构定义。地址偏移量:0x0B6，初值:0x00，宽度:8
 寄存器说明: 模拟RO预留寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ana_in : 8;  /* bit[0-7]: PMU模拟部分输入，8bits可通过寄存器读出，保留使用。 */
    } reg;
} SOC_SMART_ANA_IN_UNION;
#endif
#define SOC_SMART_ANA_IN_ana_in_START  (0)
#define SOC_SMART_ANA_IN_ana_in_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RESERVED0_UNION
 结构说明  : RESERVED0 寄存器结构定义。地址偏移量:0x0B7，初值:0x00，宽度:8
 寄存器说明: 保留0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved  : 8;  /* bit[0-7]: bit[7:4]：测试用及备用未定；
                                                    bit[3:0]: lvs的offset调整寄存器。 */
    } reg;
} SOC_SMART_RESERVED0_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_SMART_RESERVED1_UNION
 结构说明  : RESERVED1 寄存器结构定义。地址偏移量:0x0B8，初值:0x00，宽度:8
 寄存器说明: 保留1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved  : 8;  /* bit[0-7]: bit[7:2]:测试用及备用未定；
                                                    bit[1:0]:配置REF进入ECO模式。 */
    } reg;
} SOC_SMART_RESERVED1_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_SMART_RESERVED2_UNION
 结构说明  : RESERVED2 寄存器结构定义。地址偏移量:0x0B9，初值:0x00，宽度:8
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
} SOC_SMART_RESERVED2_UNION;
#endif


/*****************************************************************************
 结构名    : SOC_SMART_DAC_CTRL_UNION
 结构说明  : DAC_CTRL 寄存器结构定义。地址偏移量:0x0BA，初值:0xAA，宽度:8
 寄存器说明: DAC控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  aux_offset_cfg1 : 2;  /* bit[0-1]: AUXDAC OFFSET调节
                                                          00：+7.5mV；
                                                          01：0mV；
                                                          10：0mV；
                                                          11：-7.5mV。 */
        unsigned char  aux_ibias_cfg1  : 2;  /* bit[2-3]: AUXDAC偏置电流调节
                                                          00：2.5uA；
                                                          01：5.0uA；
                                                          10：5.0uA；
                                                          11：7.5uA。 */
        unsigned char  aux_offset_cfg2 : 2;  /* bit[4-5]: AUXDAC OFFSET调节
                                                          00：+7.5mV；
                                                          01：0mV；
                                                          10：0mV；
                                                          11：-7.5mV。 */
        unsigned char  aux_ibias_cfg2  : 2;  /* bit[6-7]: AUXDAC偏置电流调节
                                                          00：2.5uA；
                                                          01：5.0uA；
                                                          10：5.0uA；
                                                          11：7.5uA。 */
    } reg;
} SOC_SMART_DAC_CTRL_UNION;
#endif
#define SOC_SMART_DAC_CTRL_aux_offset_cfg1_START  (0)
#define SOC_SMART_DAC_CTRL_aux_offset_cfg1_END    (1)
#define SOC_SMART_DAC_CTRL_aux_ibias_cfg1_START   (2)
#define SOC_SMART_DAC_CTRL_aux_ibias_cfg1_END     (3)
#define SOC_SMART_DAC_CTRL_aux_offset_cfg2_START  (4)
#define SOC_SMART_DAC_CTRL_aux_offset_cfg2_END    (5)
#define SOC_SMART_DAC_CTRL_aux_ibias_cfg2_START   (6)
#define SOC_SMART_DAC_CTRL_aux_ibias_cfg2_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK01_CTRL0_UNION
 结构说明  : BUCK01_CTRL0 寄存器结构定义。地址偏移量:0x0D7，初值:0x72，宽度:8
 寄存器说明: BUCK01控制0信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck01_pfm_solution_sel  : 1;  /* bit[0]  : buck01自由切换方案控制：
                                                                   0：有ITH钳位的自由切换；
                                                                   1：没有ITH钳位的自由切换； */
        unsigned char  buck01_mode_sel          : 1;  /* bit[1]  : buck01的模式选择：
                                                                   0：自由切换；
                                                                   1：强制PWM； */
        unsigned char  buck01_z_trim            : 1;  /* bit[2]  : 设置运放高频噪声抑制电容的大小。
                                                                   0：减小补偿量；
                                                                   1：增大补偿量； */
        unsigned char  buck01_interleaving_ctrl : 1;  /* bit[3]  : 控制BUCK0/1时钟相位。
                                                                   0：异相工作（interleaving operation）；
                                                                   1：同相工作(non-interleaving operation)。 */
        unsigned char  buck01_osc_frequency     : 4;  /* bit[4-7]: 控制buck01时钟频率,默认频率为2MHz
                                                                   0000:3.1MHz；
                                                                   0001:3.0MHz；
                                                                   0010:2.9MHz；
                                                                   0011:2.8MHz；
                                                                   0100:2.7MHz；
                                                                   0101:2.6MHz；
                                                                   0110:2.5MHz；
                                                                   0111:2.4MHz；
                                                                   1000:2.2MHz；
                                                                   1001:2.0MHz；
                                                                   1010:1.8MHz；
                                                                   1011:1.6MHz；
                                                                   1100:1.4MHz；
                                                                   1101:1.2MHz；
                                                                   1110:1.0MHz；
                                                                   1111:0.8MHz。 */
    } reg;
} SOC_SMART_BUCK01_CTRL0_UNION;
#endif
#define SOC_SMART_BUCK01_CTRL0_buck01_pfm_solution_sel_START   (0)
#define SOC_SMART_BUCK01_CTRL0_buck01_pfm_solution_sel_END     (0)
#define SOC_SMART_BUCK01_CTRL0_buck01_mode_sel_START           (1)
#define SOC_SMART_BUCK01_CTRL0_buck01_mode_sel_END             (1)
#define SOC_SMART_BUCK01_CTRL0_buck01_z_trim_START             (2)
#define SOC_SMART_BUCK01_CTRL0_buck01_z_trim_END               (2)
#define SOC_SMART_BUCK01_CTRL0_buck01_interleaving_ctrl_START  (3)
#define SOC_SMART_BUCK01_CTRL0_buck01_interleaving_ctrl_END    (3)
#define SOC_SMART_BUCK01_CTRL0_buck01_osc_frequency_START      (4)
#define SOC_SMART_BUCK01_CTRL0_buck01_osc_frequency_END        (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK01_CTRL1_UNION
 结构说明  : BUCK01_CTRL1 寄存器结构定义。地址偏移量:0x0D8，初值:0x16，宽度:8
 寄存器说明: BUCK01控制1信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck01_ea_comp_cap_sel : 2;  /* bit[0-1]: buck01 EA内部环路补偿电容调节：
                                                                 00到11补偿电容从小到大；默认2.8pF； */
        unsigned char  buck01_vc_sel          : 2;  /* bit[2-3]: buck01自由切换PFM EA 阈值电压设置电阻调节：
                                                                 00:45k；
                                                                 01:60k；
                                                                 10:75k；
                                                                 11:90k； */
        unsigned char  buck01_burst_drv_ctr   : 1;  /* bit[4]  : buck01 进入自由切换PFM时，选择burst信号对功率管的控制：
                                                                 1：进入自由切换PFM时，选择burst只控制P功率管；
                                                                 0：进入自由切换PFM时，选择burst控制P/N功率管； */
        unsigned char  buck01_pfm_vth_sel     : 1;  /* bit[5]  : buck01 EA与EA_OUT是否通过传输门连接控制信号：
                                                                 1：传输门开启；
                                                                 0：传输门关闭； */
        unsigned char  buck01_pfm_ctrl        : 2;  /* bit[6-7]: buck01自由切换PFM辅助控制：
                                                                 Bit 7
                                                                 0：自由切换模式时EA电容有电流充电；
                                                                 1：自由切换模式时EA电容屏蔽电流充电； */
    } reg;
} SOC_SMART_BUCK01_CTRL1_UNION;
#endif
#define SOC_SMART_BUCK01_CTRL1_buck01_ea_comp_cap_sel_START  (0)
#define SOC_SMART_BUCK01_CTRL1_buck01_ea_comp_cap_sel_END    (1)
#define SOC_SMART_BUCK01_CTRL1_buck01_vc_sel_START           (2)
#define SOC_SMART_BUCK01_CTRL1_buck01_vc_sel_END             (3)
#define SOC_SMART_BUCK01_CTRL1_buck01_burst_drv_ctr_START    (4)
#define SOC_SMART_BUCK01_CTRL1_buck01_burst_drv_ctr_END      (4)
#define SOC_SMART_BUCK01_CTRL1_buck01_pfm_vth_sel_START      (5)
#define SOC_SMART_BUCK01_CTRL1_buck01_pfm_vth_sel_END        (5)
#define SOC_SMART_BUCK01_CTRL1_buck01_pfm_ctrl_START         (6)
#define SOC_SMART_BUCK01_CTRL1_buck01_pfm_ctrl_END           (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK01_CTRL2_UNION
 结构说明  : BUCK01_CTRL2 寄存器结构定义。地址偏移量:0xD9，初值:0x64，宽度:8
 寄存器说明: BUCK01控制2信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck01_comp_adj : 8;  /* bit[0-7]: 
                                                          buck01稳定性调节寄存器：
                                                          bit<7>：设置运放高频噪声抑制电容的大小。
                                                          0：1.5p；
                                                          1：4p。
                                                          
                                                          bit<6:3>：与输出相关的补偿零点调节电阻选择位。
                                                          0000：400k；
                                                          0001：375k；
                                                          0010：350k；
                                                          0011：325k；
                                                          0100：300k；
                                                          0101：275k；
                                                          0110：250k；
                                                          0111：225k；
                                                          1000：200k；
                                                          1001：175k；
                                                          1010：150k；
                                                          1011：125k；
                                                          1100：100k；
                                                          1101：75k；
                                                          1110：50k；
                                                          1111：25k。
                                                          
                                                          bit<2:0>：误差放大器跨导调整。
                                                          000：40k；
                                                          001：35k；
                                                          010：30k；
                                                          011：25k；
                                                          100：20k；
                                                          101：15k；
                                                          110：10k；
                                                          111：5k。 */
    } reg;
} SOC_SMART_BUCK01_CTRL2_UNION;
#endif
#define SOC_SMART_BUCK01_CTRL2_buck01_comp_adj_START  (0)
#define SOC_SMART_BUCK01_CTRL2_buck01_comp_adj_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK01_CTRL3_UNION
 结构说明  : BUCK01_CTRL3 寄存器结构定义。地址偏移量:0xDA，初值:0x4A，宽度:8
 寄存器说明: BUCK01控制3信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck01_vout_set : 7;  /* bit[0-6]: buck01输出控制：
                                                          默认输出1.05V
                                                           （0.6V-1.375V，127Steps
                                                          6.1mV/step）
                                                          要求电压曲线必须单调 */
        unsigned char  reserved        : 1;  /* bit[7]  : 保留。 */
    } reg;
} SOC_SMART_BUCK01_CTRL3_UNION;
#endif
#define SOC_SMART_BUCK01_CTRL3_buck01_vout_set_START  (0)
#define SOC_SMART_BUCK01_CTRL3_buck01_vout_set_END    (6)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK01_CTRL4_UNION
 结构说明  : BUCK01_CTRL4 寄存器结构定义。地址偏移量:0x0DB，初值:0xB9，宽度:8
 寄存器说明: BUCK01控制4信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck01_ib_pwm_sel       : 1;  /* bit[0]  : PWM比较器偏置电流配置。
                                                                  0：1uA偏置；
                                                                  1：2uA偏置。 */
        unsigned char  buck01_isns_rc_sel      : 1;  /* bit[1]  : ISNS滤波选择信号。
                                                                  0：增加RC滤波；
                                                                  1：不加RC滤波。
                                                                  指Sense信号的滤波。 */
        unsigned char  buck01_scp_shield       : 1;  /* bit[2]  : 短路保护屏蔽寄存器.
                                                                  0:使能短路保护;1:不使能短路保护 */
        unsigned char  buck01_sft_sel          : 1;  /* bit[3]  : BUCK0/1软启动方式选择。
                                                                  0：软启动时间为480us左右；
                                                                  1：软启动时间为240us左右。 */
        unsigned char  buck01_remote_sense_os  : 3;  /* bit[4-6]: BUCK01远端反馈offset调节； */
        unsigned char  buck01_remote_sense_sel : 1;  /* bit[7]  : buck01选择远端反馈或者近端反馈：
                                                                  0：近端反馈；
                                                                  1：远端反馈； */
    } reg;
} SOC_SMART_BUCK01_CTRL4_UNION;
#endif
#define SOC_SMART_BUCK01_CTRL4_buck01_ib_pwm_sel_START        (0)
#define SOC_SMART_BUCK01_CTRL4_buck01_ib_pwm_sel_END          (0)
#define SOC_SMART_BUCK01_CTRL4_buck01_isns_rc_sel_START       (1)
#define SOC_SMART_BUCK01_CTRL4_buck01_isns_rc_sel_END         (1)
#define SOC_SMART_BUCK01_CTRL4_buck01_scp_shield_START        (2)
#define SOC_SMART_BUCK01_CTRL4_buck01_scp_shield_END          (2)
#define SOC_SMART_BUCK01_CTRL4_buck01_sft_sel_START           (3)
#define SOC_SMART_BUCK01_CTRL4_buck01_sft_sel_END             (3)
#define SOC_SMART_BUCK01_CTRL4_buck01_remote_sense_os_START   (4)
#define SOC_SMART_BUCK01_CTRL4_buck01_remote_sense_os_END     (6)
#define SOC_SMART_BUCK01_CTRL4_buck01_remote_sense_sel_START  (7)
#define SOC_SMART_BUCK01_CTRL4_buck01_remote_sense_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL0_UNION
 结构说明  : BUCK0_CTRL0 寄存器结构定义。地址偏移量:0x0DC，初值:0x99，宽度:8
 寄存器说明: BUCK0控制0信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_ng_n_sel : 2;  /* bit[0-1]: buck0 N power管驱动对应NMOS驱动能力调节； */
        unsigned char  buck0_ng_p_sel : 2;  /* bit[2-3]: buck0 N power管驱动对应PMOS驱动能力调节； */
        unsigned char  buck0_pg_n_sel : 2;  /* bit[4-5]: buck0 P power管驱动对应NMOS驱动能力调节； */
        unsigned char  buck0_pg_p_sel : 2;  /* bit[6-7]: buck0 P power管驱动对应PMOS驱动能力调节； */
    } reg;
} SOC_SMART_BUCK0_CTRL0_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL0_buck0_ng_n_sel_START  (0)
#define SOC_SMART_BUCK0_CTRL0_buck0_ng_n_sel_END    (1)
#define SOC_SMART_BUCK0_CTRL0_buck0_ng_p_sel_START  (2)
#define SOC_SMART_BUCK0_CTRL0_buck0_ng_p_sel_END    (3)
#define SOC_SMART_BUCK0_CTRL0_buck0_pg_n_sel_START  (4)
#define SOC_SMART_BUCK0_CTRL0_buck0_pg_n_sel_END    (5)
#define SOC_SMART_BUCK0_CTRL0_buck0_pg_p_sel_START  (6)
#define SOC_SMART_BUCK0_CTRL0_buck0_pg_p_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL1_UNION
 结构说明  : BUCK0_CTRL1 寄存器结构定义。地址偏移量:0x0DD，初值:0x80，宽度:8
 寄存器说明: BUCK0控制1信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved          : 1;  /* bit[0]  : 保留。 */
        unsigned char  buck0_cs_mode_sel : 1;  /* bit[1]  : buck0电流检测电路模式选择：
                                                            0：低压sense电路工作；
                                                            1：快速sense电路工作； */
        unsigned char  buck0_pdn_lx_det  : 1;  /* bit[2]  : buck0 使能LX检测的死区时间控制电路pd：
                                                            0：不使能；
                                                            1：使能； */
        unsigned char  buck0_pdn_lx_sel  : 1;  /* bit[3]  : buck0的SLEEPN信号，控制LX检测死区方案；
                                                            0：不选择；
                                                            1：选择； */
        unsigned char  buck0_ng_dt_sel   : 1;  /* bit[4]  : buck0 P管到N管死区模式选择：
                                                            1：并入新死区控制模式；
                                                            0：不并入新死区控制模式； */
        unsigned char  buck0_pg_dt_sel   : 1;  /* bit[5]  : buck0 N管到P管死区模式选择：
                                                            1：并入新死区控制模式；
                                                            0：不并入新死区控制模式； */
        unsigned char  buck0_dt_sel      : 2;  /* bit[6-7]: bit[7]：传统N、P开启5ns延时。
                                                            0：N MOS关闭到P MOS开启加5ns延时（P MOS关闭到N MOS开启加5ns延时）；
                                                            1：N MOS关闭到P MOS开启无延时（P MOS关闭到N MOS开启无延时）。
                                                            
                                                            bit[6]：N管关闭时立刻开启P管死区模式延时选择。
                                                            0：P mos开启无5ns延时；
                                                            1：P mos开启加5ns延时。 */
    } reg;
} SOC_SMART_BUCK0_CTRL1_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL1_buck0_cs_mode_sel_START  (1)
#define SOC_SMART_BUCK0_CTRL1_buck0_cs_mode_sel_END    (1)
#define SOC_SMART_BUCK0_CTRL1_buck0_pdn_lx_det_START   (2)
#define SOC_SMART_BUCK0_CTRL1_buck0_pdn_lx_det_END     (2)
#define SOC_SMART_BUCK0_CTRL1_buck0_pdn_lx_sel_START   (3)
#define SOC_SMART_BUCK0_CTRL1_buck0_pdn_lx_sel_END     (3)
#define SOC_SMART_BUCK0_CTRL1_buck0_ng_dt_sel_START    (4)
#define SOC_SMART_BUCK0_CTRL1_buck0_ng_dt_sel_END      (4)
#define SOC_SMART_BUCK0_CTRL1_buck0_pg_dt_sel_START    (5)
#define SOC_SMART_BUCK0_CTRL1_buck0_pg_dt_sel_END      (5)
#define SOC_SMART_BUCK0_CTRL1_buck0_dt_sel_START       (6)
#define SOC_SMART_BUCK0_CTRL1_buck0_dt_sel_END         (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL2_UNION
 结构说明  : BUCK0_CTRL2 寄存器结构定义。地址偏移量:0x0DE，初值:0x00，宽度:8
 寄存器说明: BUCK0控制2信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_bt_sel     : 2;  /* bit[0-1]: buck0开关管blanking time调节：
                                                           00：0ns延时；
                                                           01，10：5ns延时；
                                                           11：10ns延时； */
        unsigned char  buck0_cs_sel     : 2;  /* bit[2-3]: buck0调节sense管处的电容：
                                                           00：0p；
                                                           01：0.2p；
                                                           10：0.4p；
                                                           11：0.6p； */
        unsigned char  buck0_cs_os_sel  : 1;  /* bit[4]  : buck0电流检测电路os选择：
                                                           0：无os；
                                                           1：有os； */
        unsigned char  buck0_cs_dt_sel  : 1;  /* bit[5]  : buck0电流检测DT选择：
                                                           0:0ns;
                                                           1:5ns; */
        unsigned char  buck0_cs_mos_sel : 2;  /* bit[6-7]: 选择sense管工作状态。
                                                           00：sense管受PG控制；
                                                           01：低压sense电路sense管常开；
                                                           10：快速sense电路sense管常开；
                                                           11: 低压快速sense管均关闭（禁止该配置）。 */
    } reg;
} SOC_SMART_BUCK0_CTRL2_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL2_buck0_bt_sel_START      (0)
#define SOC_SMART_BUCK0_CTRL2_buck0_bt_sel_END        (1)
#define SOC_SMART_BUCK0_CTRL2_buck0_cs_sel_START      (2)
#define SOC_SMART_BUCK0_CTRL2_buck0_cs_sel_END        (3)
#define SOC_SMART_BUCK0_CTRL2_buck0_cs_os_sel_START   (4)
#define SOC_SMART_BUCK0_CTRL2_buck0_cs_os_sel_END     (4)
#define SOC_SMART_BUCK0_CTRL2_buck0_cs_dt_sel_START   (5)
#define SOC_SMART_BUCK0_CTRL2_buck0_cs_dt_sel_END     (5)
#define SOC_SMART_BUCK0_CTRL2_buck0_cs_mos_sel_START  (6)
#define SOC_SMART_BUCK0_CTRL2_buck0_cs_mos_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL3_UNION
 结构说明  : BUCK0_CTRL3 寄存器结构定义。地址偏移量:0x0DF，初值:0x02，宽度:8
 寄存器说明: BUCK0控制3信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved              : 1;  /* bit[0]  : 保留。 */
        unsigned char  buck0_ocp_n_sel       : 2;  /* bit[1-2]: buck0 nmos ocp电路ocp点调节； */
        unsigned char  buck0_ocp_new_old_sel : 1;  /* bit[3]  : buck0 ocp电路选择：
                                                                0：new ocp；
                                                                1：old ocp； */
        unsigned char  buck0_ccc_sel         : 2;  /* bit[4-5]: buck0调节isns处防开关误动作的电容； */
        unsigned char  buck0_ccm_sel         : 2;  /* bit[6-7]: buck0调节并联均流电流大小；
                                                                00：0；
                                                                01：2%；
                                                                10：5%；
                                                                11：10%。 */
    } reg;
} SOC_SMART_BUCK0_CTRL3_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL3_buck0_ocp_n_sel_START        (1)
#define SOC_SMART_BUCK0_CTRL3_buck0_ocp_n_sel_END          (2)
#define SOC_SMART_BUCK0_CTRL3_buck0_ocp_new_old_sel_START  (3)
#define SOC_SMART_BUCK0_CTRL3_buck0_ocp_new_old_sel_END    (3)
#define SOC_SMART_BUCK0_CTRL3_buck0_ccc_sel_START          (4)
#define SOC_SMART_BUCK0_CTRL3_buck0_ccc_sel_END            (5)
#define SOC_SMART_BUCK0_CTRL3_buck0_ccm_sel_START          (6)
#define SOC_SMART_BUCK0_CTRL3_buck0_ccm_sel_END            (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL4_UNION
 结构说明  : BUCK0_CTRL4 寄存器结构定义。地址偏移量:0x0E0，初值:0x48，宽度:8
 寄存器说明: BUCK0控制4信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_ocp_sel           : 4;  /* bit[0-3]: buck0 OCP 触发点选择位； */
        unsigned char  buck0_ocp_tran_enhanced : 1;  /* bit[4]  : buck0 ocp瞬态提升方案； */
        unsigned char  buck0_nmos_ocp          : 1;  /* bit[5]  : buck0 nmos ocp电路选择； */
        unsigned char  buck0_ocp_p_sel         : 2;  /* bit[6-7]: buck0 pmos ocp电路ocp点调节；
                                                                  00：3.9A；
                                                                  01：4.5A；
                                                                  10：5.1A；
                                                                  11：5.6A。 */
    } reg;
} SOC_SMART_BUCK0_CTRL4_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL4_buck0_ocp_sel_START            (0)
#define SOC_SMART_BUCK0_CTRL4_buck0_ocp_sel_END              (3)
#define SOC_SMART_BUCK0_CTRL4_buck0_ocp_tran_enhanced_START  (4)
#define SOC_SMART_BUCK0_CTRL4_buck0_ocp_tran_enhanced_END    (4)
#define SOC_SMART_BUCK0_CTRL4_buck0_nmos_ocp_START           (5)
#define SOC_SMART_BUCK0_CTRL4_buck0_nmos_ocp_END             (5)
#define SOC_SMART_BUCK0_CTRL4_buck0_ocp_p_sel_START          (6)
#define SOC_SMART_BUCK0_CTRL4_buck0_ocp_p_sel_END            (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL5_UNION
 结构说明  : BUCK0_CTRL5 寄存器结构定义。地址偏移量:0x0E1，初值:0x00，宽度:8
 寄存器说明: BUCK0控制5信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved            : 1;  /* bit[0]  : buck0新老dmd结构dmd电流调节； */
        unsigned char  buck0_new_dmd_sel   : 4;  /* bit[1-4]: buck0新dmd结构dmd电流调节； */
        unsigned char  buck0_dmd_en        : 1;  /* bit[5]  : buck0选择DMD功能的开关； */
        unsigned char  buck0_ocp_shield    : 1;  /* bit[6]  : buck0 OCP信号对PMOS的控制：
                                                              0：屏蔽OCP对PMOS的控制，模拟不限流；
                                                              1：不屏蔽OCP对PMOS的控制，模拟限流； */
        unsigned char  buck0_ocp_delay_sel : 1;  /* bit[7]  : buck0 ocp屏蔽时间延长30ns信号：
                                                              0：不延长,15ns；
                                                              1：延长,30ns； */
    } reg;
} SOC_SMART_BUCK0_CTRL5_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL5_buck0_new_dmd_sel_START    (1)
#define SOC_SMART_BUCK0_CTRL5_buck0_new_dmd_sel_END      (4)
#define SOC_SMART_BUCK0_CTRL5_buck0_dmd_en_START         (5)
#define SOC_SMART_BUCK0_CTRL5_buck0_dmd_en_END           (5)
#define SOC_SMART_BUCK0_CTRL5_buck0_ocp_shield_START     (6)
#define SOC_SMART_BUCK0_CTRL5_buck0_ocp_shield_END       (6)
#define SOC_SMART_BUCK0_CTRL5_buck0_ocp_delay_sel_START  (7)
#define SOC_SMART_BUCK0_CTRL5_buck0_ocp_delay_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL6_UNION
 结构说明  : BUCK0_CTRL6 寄存器结构定义。地址偏移量:0x0E2，初值:0x52，宽度:8
 寄存器说明: BUCK0控制6信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_dmd_sel : 2;  /* bit[0-1]: buck0 DMD点设置； */
        unsigned char  buck0_mos_sel : 2;  /* bit[2-3]: buck0新老dmd结构dmd电流调节； */
        unsigned char  buck0_isc_sel : 2;  /* bit[4-5]: buck0 自由切换pfm时电感峰值电流控制增加量(00~11增大)； */
        unsigned char  buck0_ipk_sel : 2;  /* bit[6-7]: buck0 自由切换pfm时电感峰值电流控制电阻增加量：
                                                        00:21k；
                                                        01:28k；
                                                        10:35k；
                                                        11:41k； */
    } reg;
} SOC_SMART_BUCK0_CTRL6_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL6_buck0_dmd_sel_START  (0)
#define SOC_SMART_BUCK0_CTRL6_buck0_dmd_sel_END    (1)
#define SOC_SMART_BUCK0_CTRL6_buck0_mos_sel_START  (2)
#define SOC_SMART_BUCK0_CTRL6_buck0_mos_sel_END    (3)
#define SOC_SMART_BUCK0_CTRL6_buck0_isc_sel_START  (4)
#define SOC_SMART_BUCK0_CTRL6_buck0_isc_sel_END    (5)
#define SOC_SMART_BUCK0_CTRL6_buck0_ipk_sel_START  (6)
#define SOC_SMART_BUCK0_CTRL6_buck0_ipk_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL7_UNION
 结构说明  : BUCK0_CTRL7 寄存器结构定义。地址偏移量:0x0E3，初值:0x0C，宽度:8
 寄存器说明: BUCK0控制7信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_l_sel        : 1;  /* bit[0]  : buck0斜坡补偿电容调节：
                                                             0：电容1倍，兼容0.47uH电感；
                                                             1：电容2倍，兼容1.0uH电感； */
        unsigned char  buck0_slope_gen    : 3;  /* bit[1-3]: buck0斜坡补偿变化（000-111逐渐增加）； */
        unsigned char  buck0_sc_sel       : 2;  /* bit[4-5]: buck0斜坡补偿电阻，SENSE电阻，斜坡补偿电容调节； */
        unsigned char  buck0_dmd_shield_n : 1;  /* bit[6]  : buck0 大电流下dmd屏蔽信号：
                                                             0：屏蔽；
                                                             1：不屏蔽； */
        unsigned char  buck0_dmd_clamp    : 1;  /* bit[7]  : buck0 dmd比较强clamp选择：
                                                             0：使用clamp电路；
                                                             1：不使用clamp电路； */
    } reg;
} SOC_SMART_BUCK0_CTRL7_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL7_buck0_l_sel_START         (0)
#define SOC_SMART_BUCK0_CTRL7_buck0_l_sel_END           (0)
#define SOC_SMART_BUCK0_CTRL7_buck0_slope_gen_START     (1)
#define SOC_SMART_BUCK0_CTRL7_buck0_slope_gen_END       (3)
#define SOC_SMART_BUCK0_CTRL7_buck0_sc_sel_START        (4)
#define SOC_SMART_BUCK0_CTRL7_buck0_sc_sel_END          (5)
#define SOC_SMART_BUCK0_CTRL7_buck0_dmd_shield_n_START  (6)
#define SOC_SMART_BUCK0_CTRL7_buck0_dmd_shield_n_END    (6)
#define SOC_SMART_BUCK0_CTRL7_buck0_dmd_clamp_START     (7)
#define SOC_SMART_BUCK0_CTRL7_buck0_dmd_clamp_END       (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL8_UNION
 结构说明  : BUCK0_CTRL8 寄存器结构定义。地址偏移量:0x0E4，初值:0x40，宽度:8
 寄存器说明: BUCK0控制8信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved       : 1;  /* bit[0]  : 保留。 */
        unsigned char  buck0_ton_on   : 2;  /* bit[1-2]: buck0最小占空比调节位(00~11增大)； */
        unsigned char  buck0_ton_off  : 1;  /* bit[3]  : buck0最小占空比调节控制位：
                                                         0：关闭调节功能；
                                                         1：开启调节功能； */
        unsigned char  buck0_clk_sel  : 1;  /* bit[4]  : buck0并联时钟选在信号：
                                                         1：共用buck0 EA；
                                                         0：共用buck0 PWM； */
        unsigned char  buck0_ea_sel   : 1;  /* bit[5]  : buck0并联模式控制信号：
                                                         1：共用buck0 PWM信号；
                                                         0：分别用各自PWM信号； */
        unsigned char  buck0_slope_dc : 2;  /* bit[6-7]: buck0 slope dc值设定选择
                                                         00：670mv；
                                                         01：550mv；
                                                         10：860mv；
                                                         11：770mv。 */
    } reg;
} SOC_SMART_BUCK0_CTRL8_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL8_buck0_ton_on_START    (1)
#define SOC_SMART_BUCK0_CTRL8_buck0_ton_on_END      (2)
#define SOC_SMART_BUCK0_CTRL8_buck0_ton_off_START   (3)
#define SOC_SMART_BUCK0_CTRL8_buck0_ton_off_END     (3)
#define SOC_SMART_BUCK0_CTRL8_buck0_clk_sel_START   (4)
#define SOC_SMART_BUCK0_CTRL8_buck0_clk_sel_END     (4)
#define SOC_SMART_BUCK0_CTRL8_buck0_ea_sel_START    (5)
#define SOC_SMART_BUCK0_CTRL8_buck0_ea_sel_END      (5)
#define SOC_SMART_BUCK0_CTRL8_buck0_slope_dc_START  (6)
#define SOC_SMART_BUCK0_CTRL8_buck0_slope_dc_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL9_UNION
 结构说明  : BUCK0_CTRL9 寄存器结构定义。地址偏移量:0x0E5，初值:0x00，宽度:8
 寄存器说明: BUCK0控制9信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_reserved0 : 8;  /* bit[0-7]: 预留寄存器 */
    } reg;
} SOC_SMART_BUCK0_CTRL9_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL9_buck0_reserved0_START  (0)
#define SOC_SMART_BUCK0_CTRL9_buck0_reserved0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK0_CTRL10_UNION
 结构说明  : BUCK0_CTRL10 寄存器结构定义。地址偏移量:0x0E6，初值:0x00，宽度:8
 寄存器说明: BUCK0控制10信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck0_reserved1 : 8;  /* bit[0-7]: 预留寄存器 */
    } reg;
} SOC_SMART_BUCK0_CTRL10_UNION;
#endif
#define SOC_SMART_BUCK0_CTRL10_buck0_reserved1_START  (0)
#define SOC_SMART_BUCK0_CTRL10_buck0_reserved1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL0_UNION
 结构说明  : BUCK1_CTRL0 寄存器结构定义。地址偏移量:0x0E7，初值:0x99，宽度:8
 寄存器说明: BUCK1控制0信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_ng_n_sel : 2;  /* bit[0-1]: buck1 N power管驱动对应NMOS驱动能力调节； */
        unsigned char  buck1_ng_p_sel : 2;  /* bit[2-3]: buck1 N power管驱动对应PMOS驱动能力调节； */
        unsigned char  buck1_pg_n_sel : 2;  /* bit[4-5]: buck1 P power管驱动对应NMOS驱动能力调节； */
        unsigned char  buck1_pg_p_sel : 2;  /* bit[6-7]: buck1 P power管驱动对应PMOS驱动能力调节； */
    } reg;
} SOC_SMART_BUCK1_CTRL0_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL0_buck1_ng_n_sel_START  (0)
#define SOC_SMART_BUCK1_CTRL0_buck1_ng_n_sel_END    (1)
#define SOC_SMART_BUCK1_CTRL0_buck1_ng_p_sel_START  (2)
#define SOC_SMART_BUCK1_CTRL0_buck1_ng_p_sel_END    (3)
#define SOC_SMART_BUCK1_CTRL0_buck1_pg_n_sel_START  (4)
#define SOC_SMART_BUCK1_CTRL0_buck1_pg_n_sel_END    (5)
#define SOC_SMART_BUCK1_CTRL0_buck1_pg_p_sel_START  (6)
#define SOC_SMART_BUCK1_CTRL0_buck1_pg_p_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL1_UNION
 结构说明  : BUCK1_CTRL1 寄存器结构定义。地址偏移量:0x0E8，初值:0x80，宽度:8
 寄存器说明: BUCK1控制1信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved          : 1;  /* bit[0]  : 保留。 */
        unsigned char  buck1_cs_mode_sel : 1;  /* bit[1]  : buck0电流检测电路模式选择：
                                                            0：低压sense电路工作；
                                                            1：快速sense电路工作； */
        unsigned char  buck1_pdn_lx_det  : 1;  /* bit[2]  : buck0 使能LX检测的死区时间控制电路pd：
                                                            0：不使能；
                                                            1：使能； */
        unsigned char  buck1_pdn_lx_sel  : 1;  /* bit[3]  : buck0的SLEEPN信号，控制LX检测死区方案；
                                                            0：不选择；
                                                            1：选择； */
        unsigned char  buck1_ng_dt_sel   : 1;  /* bit[4]  : buck1 P管到N管死区模式选择：
                                                            1：并入新死区控制模式；
                                                            0：不并入新死区控制模式； */
        unsigned char  buck1_pg_dt_sel   : 1;  /* bit[5]  : buck1 N管到P管死区模式选择：
                                                            1：并入新死区控制模式；
                                                            0：不并入新死区控制模式； */
        unsigned char  buck1_dt_sel      : 2;  /* bit[6-7]: bit[7]：传统N、P开启5ns延时。
                                                            0：N MOS关闭到P MOS开启加5ns延时（P MOS关闭到N MOS开启加5ns延时）；
                                                            1：N MOS关闭到P MOS开启无延时（P MOS关闭到N MOS开启无延时）。
                                                            
                                                            bit[6]：N管关闭时立刻开启P管死区模式延时选择。
                                                            0：P mos开启无5ns延时；
                                                            1：P mos开启加5ns延时。 */
    } reg;
} SOC_SMART_BUCK1_CTRL1_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL1_buck1_cs_mode_sel_START  (1)
#define SOC_SMART_BUCK1_CTRL1_buck1_cs_mode_sel_END    (1)
#define SOC_SMART_BUCK1_CTRL1_buck1_pdn_lx_det_START   (2)
#define SOC_SMART_BUCK1_CTRL1_buck1_pdn_lx_det_END     (2)
#define SOC_SMART_BUCK1_CTRL1_buck1_pdn_lx_sel_START   (3)
#define SOC_SMART_BUCK1_CTRL1_buck1_pdn_lx_sel_END     (3)
#define SOC_SMART_BUCK1_CTRL1_buck1_ng_dt_sel_START    (4)
#define SOC_SMART_BUCK1_CTRL1_buck1_ng_dt_sel_END      (4)
#define SOC_SMART_BUCK1_CTRL1_buck1_pg_dt_sel_START    (5)
#define SOC_SMART_BUCK1_CTRL1_buck1_pg_dt_sel_END      (5)
#define SOC_SMART_BUCK1_CTRL1_buck1_dt_sel_START       (6)
#define SOC_SMART_BUCK1_CTRL1_buck1_dt_sel_END         (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL2_UNION
 结构说明  : BUCK1_CTRL2 寄存器结构定义。地址偏移量:0x0E9，初值:0x00，宽度:8
 寄存器说明: BUCK1控制2信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_bt_sel     : 2;  /* bit[0-1]: buck1开关管blanking time调节：
                                                           00：0ns延时；
                                                           01，10：5ns延时；
                                                           11：10ns延时； */
        unsigned char  buck1_cs_sel     : 2;  /* bit[2-3]: buck1调节sense管处的电容：
                                                           00：0p；
                                                           01：0.2p；
                                                           10：0.4p；
                                                           11：0.6p； */
        unsigned char  buck1_cs_os_sel  : 1;  /* bit[4]  : buck1电流检测电路os选择：
                                                           0：无os；
                                                           1：有os； */
        unsigned char  buck1_cs_dt_sel  : 1;  /* bit[5]  : buck1电流检测DT选择：
                                                           0:0ns;
                                                           1:5ns; */
        unsigned char  buck1_cs_mos_sel : 2;  /* bit[6-7]: 选择sense管工作状态。
                                                           00：sense管受PG控制；
                                                           01：低压sense电路sense管常开；
                                                           10：快速sense电路sense管常开；
                                                           11: 低压快速sense管均关闭（禁止该配置）。 */
    } reg;
} SOC_SMART_BUCK1_CTRL2_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL2_buck1_bt_sel_START      (0)
#define SOC_SMART_BUCK1_CTRL2_buck1_bt_sel_END        (1)
#define SOC_SMART_BUCK1_CTRL2_buck1_cs_sel_START      (2)
#define SOC_SMART_BUCK1_CTRL2_buck1_cs_sel_END        (3)
#define SOC_SMART_BUCK1_CTRL2_buck1_cs_os_sel_START   (4)
#define SOC_SMART_BUCK1_CTRL2_buck1_cs_os_sel_END     (4)
#define SOC_SMART_BUCK1_CTRL2_buck1_cs_dt_sel_START   (5)
#define SOC_SMART_BUCK1_CTRL2_buck1_cs_dt_sel_END     (5)
#define SOC_SMART_BUCK1_CTRL2_buck1_cs_mos_sel_START  (6)
#define SOC_SMART_BUCK1_CTRL2_buck1_cs_mos_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL3_UNION
 结构说明  : BUCK1_CTRL3 寄存器结构定义。地址偏移量:0x0EA，初值:0x02，宽度:8
 寄存器说明: BUCK1控制3信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved              : 1;  /* bit[0]  : 保留。 */
        unsigned char  buck1_ocp_n_sel       : 2;  /* bit[1-2]: buck1 nmos ocp电路ocp点调节； */
        unsigned char  buck1_ocp_new_old_sel : 1;  /* bit[3]  : buck1 ocp电路选择：
                                                                0：new ocp；
                                                                1：old ocp； */
        unsigned char  buck1_ccc_sel         : 2;  /* bit[4-5]: buck1调节isns处防开关误动作的电容； */
        unsigned char  buck1_ccm_sel         : 2;  /* bit[6-7]: buck1调节并联均流电流大小；
                                                                00：0；
                                                                01：2%；
                                                                10：5%；
                                                                11：10%。 */
    } reg;
} SOC_SMART_BUCK1_CTRL3_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL3_buck1_ocp_n_sel_START        (1)
#define SOC_SMART_BUCK1_CTRL3_buck1_ocp_n_sel_END          (2)
#define SOC_SMART_BUCK1_CTRL3_buck1_ocp_new_old_sel_START  (3)
#define SOC_SMART_BUCK1_CTRL3_buck1_ocp_new_old_sel_END    (3)
#define SOC_SMART_BUCK1_CTRL3_buck1_ccc_sel_START          (4)
#define SOC_SMART_BUCK1_CTRL3_buck1_ccc_sel_END            (5)
#define SOC_SMART_BUCK1_CTRL3_buck1_ccm_sel_START          (6)
#define SOC_SMART_BUCK1_CTRL3_buck1_ccm_sel_END            (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL4_UNION
 结构说明  : BUCK1_CTRL4 寄存器结构定义。地址偏移量:0x0EB，初值:0x48，宽度:8
 寄存器说明: BUCK1控制4信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_ocp_sel           : 4;  /* bit[0-3]: buck1 OCP 触发点选择位； */
        unsigned char  buck1_ocp_tran_enhanced : 1;  /* bit[4]  : buck1 ocp瞬态提升方案； */
        unsigned char  buck1_nmos_ocp          : 1;  /* bit[5]  : buck1 nmos ocp电路选择； */
        unsigned char  buck1_ocp_p_sel         : 2;  /* bit[6-7]: buck1 pmos ocp电路ocp点调节；
                                                                  00：3.9A；
                                                                  01：4.5A；
                                                                  10：5.1A；
                                                                  11：5.6A。 */
    } reg;
} SOC_SMART_BUCK1_CTRL4_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL4_buck1_ocp_sel_START            (0)
#define SOC_SMART_BUCK1_CTRL4_buck1_ocp_sel_END              (3)
#define SOC_SMART_BUCK1_CTRL4_buck1_ocp_tran_enhanced_START  (4)
#define SOC_SMART_BUCK1_CTRL4_buck1_ocp_tran_enhanced_END    (4)
#define SOC_SMART_BUCK1_CTRL4_buck1_nmos_ocp_START           (5)
#define SOC_SMART_BUCK1_CTRL4_buck1_nmos_ocp_END             (5)
#define SOC_SMART_BUCK1_CTRL4_buck1_ocp_p_sel_START          (6)
#define SOC_SMART_BUCK1_CTRL4_buck1_ocp_p_sel_END            (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL5_UNION
 结构说明  : BUCK1_CTRL5 寄存器结构定义。地址偏移量:0x0EC，初值:0x00，宽度:8
 寄存器说明: BUCK1控制5信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved            : 1;  /* bit[0]  : buck1新老dmd结构dmd电流调节； */
        unsigned char  buck1_new_dmd_sel   : 4;  /* bit[1-4]: buck1新dmd结构dmd电流调节； */
        unsigned char  buck1_dmd_en        : 1;  /* bit[5]  : buck1选择DMD功能的开关； */
        unsigned char  buck1_ocp_shield    : 1;  /* bit[6]  : buck1 OCP信号对PMOS的控制：
                                                              0：屏蔽OCP对PMOS的控制，模拟不限流；
                                                              1：不屏蔽OCP对PMOS的控制，模拟限流； */
        unsigned char  buck1_ocp_delay_sel : 1;  /* bit[7]  : buck1 ocp屏蔽时间延长30ns信号：
                                                              0：15ns；
                                                              1：30ns。 */
    } reg;
} SOC_SMART_BUCK1_CTRL5_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL5_buck1_new_dmd_sel_START    (1)
#define SOC_SMART_BUCK1_CTRL5_buck1_new_dmd_sel_END      (4)
#define SOC_SMART_BUCK1_CTRL5_buck1_dmd_en_START         (5)
#define SOC_SMART_BUCK1_CTRL5_buck1_dmd_en_END           (5)
#define SOC_SMART_BUCK1_CTRL5_buck1_ocp_shield_START     (6)
#define SOC_SMART_BUCK1_CTRL5_buck1_ocp_shield_END       (6)
#define SOC_SMART_BUCK1_CTRL5_buck1_ocp_delay_sel_START  (7)
#define SOC_SMART_BUCK1_CTRL5_buck1_ocp_delay_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL6_UNION
 结构说明  : BUCK1_CTRL6 寄存器结构定义。地址偏移量:0x0ED，初值:0x52，宽度:8
 寄存器说明: BUCK1控制6信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_dmd_sel : 2;  /* bit[0-1]: buck1 DMD点设置； */
        unsigned char  buck1_mos_sel : 2;  /* bit[2-3]: buck1新老dmd结构dmd电流调节； */
        unsigned char  buck1_isc_sel : 2;  /* bit[4-5]: buck1 自由切换pfm时电感峰值电流控制增加量(00~11增大)； */
        unsigned char  buck1_ipk_sel : 2;  /* bit[6-7]: buck1 自由切换pfm时电感峰值电流控制电阻增加量：
                                                        00:21k；
                                                        01:28k；
                                                        10:35k；
                                                        11:41k； */
    } reg;
} SOC_SMART_BUCK1_CTRL6_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL6_buck1_dmd_sel_START  (0)
#define SOC_SMART_BUCK1_CTRL6_buck1_dmd_sel_END    (1)
#define SOC_SMART_BUCK1_CTRL6_buck1_mos_sel_START  (2)
#define SOC_SMART_BUCK1_CTRL6_buck1_mos_sel_END    (3)
#define SOC_SMART_BUCK1_CTRL6_buck1_isc_sel_START  (4)
#define SOC_SMART_BUCK1_CTRL6_buck1_isc_sel_END    (5)
#define SOC_SMART_BUCK1_CTRL6_buck1_ipk_sel_START  (6)
#define SOC_SMART_BUCK1_CTRL6_buck1_ipk_sel_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL7_UNION
 结构说明  : BUCK1_CTRL7 寄存器结构定义。地址偏移量:0x0EF，初值:0x0C，宽度:8
 寄存器说明: BUCK1控制7信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_l_sel        : 1;  /* bit[0]  : buck1斜坡补偿电容调节：
                                                             0：电容1倍，兼容0.47uH电感；
                                                             1：电容2倍，兼容1.0uH电感； */
        unsigned char  buck1_slope_gen    : 3;  /* bit[1-3]: buck1斜坡补偿变化（000-111逐渐增加）； */
        unsigned char  buck1_sc_sel       : 2;  /* bit[4-5]: buck1斜坡补偿电阻，SENSE电阻，斜坡补偿电容调节； */
        unsigned char  buck1_dmd_shield_n : 1;  /* bit[6]  : buck1 大电流下dmd屏蔽信号：
                                                             0：屏蔽；
                                                             1：不屏蔽； */
        unsigned char  buck1_dmd_clamp    : 1;  /* bit[7]  : buck1 dmd比较强clamp选择：
                                                             0：使用clamp电路；
                                                             1：不使用clamp电路； */
    } reg;
} SOC_SMART_BUCK1_CTRL7_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL7_buck1_l_sel_START         (0)
#define SOC_SMART_BUCK1_CTRL7_buck1_l_sel_END           (0)
#define SOC_SMART_BUCK1_CTRL7_buck1_slope_gen_START     (1)
#define SOC_SMART_BUCK1_CTRL7_buck1_slope_gen_END       (3)
#define SOC_SMART_BUCK1_CTRL7_buck1_sc_sel_START        (4)
#define SOC_SMART_BUCK1_CTRL7_buck1_sc_sel_END          (5)
#define SOC_SMART_BUCK1_CTRL7_buck1_dmd_shield_n_START  (6)
#define SOC_SMART_BUCK1_CTRL7_buck1_dmd_shield_n_END    (6)
#define SOC_SMART_BUCK1_CTRL7_buck1_dmd_clamp_START     (7)
#define SOC_SMART_BUCK1_CTRL7_buck1_dmd_clamp_END       (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL8_UNION
 结构说明  : BUCK1_CTRL8 寄存器结构定义。地址偏移量:0x0F0，初值:0x40，宽度:8
 寄存器说明: BUCK1控制8信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved       : 1;  /* bit[0]  : 保留。 */
        unsigned char  buck1_ton_on   : 2;  /* bit[1-2]: buck1最小占空比调节位(00~11增大)； */
        unsigned char  buck1_ton_off  : 1;  /* bit[3]  : buck1最小占空比调节控制位：
                                                         0：关闭调节功能；
                                                         1：开启调节功能； */
        unsigned char  buck1_clk_sel  : 1;  /* bit[4]  : buck1并联时钟选在信号：
                                                         1：共用buck0 EA；
                                                         0：共用buck0 PWM； */
        unsigned char  buck1_ea_sel   : 1;  /* bit[5]  : buck1并联模式控制信号：
                                                         1：共用buck0 PWM信号；
                                                         0：分别用各自PWM信号； */
        unsigned char  buck1_slope_dc : 2;  /* bit[6-7]: buck1 slope dc值设定选择
                                                         00：670mv；
                                                         01：550mv；
                                                         10：860mv；
                                                         11：770mv。 */
    } reg;
} SOC_SMART_BUCK1_CTRL8_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL8_buck1_ton_on_START    (1)
#define SOC_SMART_BUCK1_CTRL8_buck1_ton_on_END      (2)
#define SOC_SMART_BUCK1_CTRL8_buck1_ton_off_START   (3)
#define SOC_SMART_BUCK1_CTRL8_buck1_ton_off_END     (3)
#define SOC_SMART_BUCK1_CTRL8_buck1_clk_sel_START   (4)
#define SOC_SMART_BUCK1_CTRL8_buck1_clk_sel_END     (4)
#define SOC_SMART_BUCK1_CTRL8_buck1_ea_sel_START    (5)
#define SOC_SMART_BUCK1_CTRL8_buck1_ea_sel_END      (5)
#define SOC_SMART_BUCK1_CTRL8_buck1_slope_dc_START  (6)
#define SOC_SMART_BUCK1_CTRL8_buck1_slope_dc_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL9_UNION
 结构说明  : BUCK1_CTRL9 寄存器结构定义。地址偏移量:0x0F1，初值:0x00，宽度:8
 寄存器说明: BUCK1控制9信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_reserved0 : 8;  /* bit[0-7]: 预留寄存器 */
    } reg;
} SOC_SMART_BUCK1_CTRL9_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL9_buck1_reserved0_START  (0)
#define SOC_SMART_BUCK1_CTRL9_buck1_reserved0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK1_CTRL10_UNION
 结构说明  : BUCK1_CTRL10 寄存器结构定义。地址偏移量:0x0F2，初值:0x00，宽度:8
 寄存器说明: BUCK1控制10信号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck1_reserved1 : 8;  /* bit[0-7]: 预留寄存器 */
    } reg;
} SOC_SMART_BUCK1_CTRL10_UNION;
#endif
#define SOC_SMART_BUCK1_CTRL10_buck1_reserved1_START  (0)
#define SOC_SMART_BUCK1_CTRL10_buck1_reserved1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_LDO22_OCP_VOS_UNION
 结构说明  : LDO22_OCP_VOS 寄存器结构定义。地址偏移量:0x0F3，初值:0x01，宽度:8
 寄存器说明: LDO22过流比较器配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ldo22_ocp_vos : 2;  /* bit[0-1]: LDO22过流的比较器失调设置，8mV/14mV/20mV/26mV */
        unsigned char  reserved      : 6;  /* bit[2-7]: 保留。 */
    } reg;
} SOC_SMART_LDO22_OCP_VOS_UNION;
#endif
#define SOC_SMART_LDO22_OCP_VOS_ldo22_ocp_vos_START  (0)
#define SOC_SMART_LDO22_OCP_VOS_ldo22_ocp_vos_END    (1)


/*****************************************************************************
 结构名    : SOC_SMART_VSET_LDO_BUCK_UNION
 结构说明  : VSET_LDO_BUCK 寄存器结构定义。地址偏移量:0x0F4，初值:0x01，宽度:8
 寄存器说明: LDO_BUCK输出电压配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vset_ldo_buck : 2;  /* bit[0-1]: LDO_BUCK输出电压设置
                                                        2.7/2.9/3.0/3.15 */
        unsigned char  reserved      : 6;  /* bit[2-7]: 保留。 */
    } reg;
} SOC_SMART_VSET_LDO_BUCK_UNION;
#endif
#define SOC_SMART_VSET_LDO_BUCK_vset_ldo_buck_START  (0)
#define SOC_SMART_VSET_LDO_BUCK_vset_ldo_buck_END    (1)


/*****************************************************************************
 结构名    : SOC_SMART_XO_LDO_CTRL_UNION
 结构说明  : XO_LDO_CTRL 寄存器结构定义。地址偏移量:0x0F5，初值:0x00，宽度:8
 寄存器说明: XO_LDO控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  xo_ldo_en_cfg  : 1;  /* bit[0]  : 内部buf的LDO的使能，
                                                         1'b0表示关闭；
                                                         1'b1表示打开。 */
        unsigned char  xo_ldo_en_sel  : 1;  /* bit[1]  : xo_ldo_en控制来源选择：
                                                         1'b1:寄存器控制
                                                         1'b0:内部逻辑控制 */
        unsigned char  xo_ldo_res_reg : 1;  /* bit[2]  : 模拟内部没使用 */
        unsigned char  reserved       : 5;  /* bit[3-7]: 保留 */
    } reg;
} SOC_SMART_XO_LDO_CTRL_UNION;
#endif
#define SOC_SMART_XO_LDO_CTRL_xo_ldo_en_cfg_START   (0)
#define SOC_SMART_XO_LDO_CTRL_xo_ldo_en_cfg_END     (0)
#define SOC_SMART_XO_LDO_CTRL_xo_ldo_en_sel_START   (1)
#define SOC_SMART_XO_LDO_CTRL_xo_ldo_en_sel_END     (1)
#define SOC_SMART_XO_LDO_CTRL_xo_ldo_res_reg_START  (2)
#define SOC_SMART_XO_LDO_CTRL_xo_ldo_res_reg_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_BUCK01_MOD_CTRL_UNION
 结构说明  : BUCK01_MOD_CTRL 寄存器结构定义。地址偏移量:0x0F7，初值:0x03，宽度:8
 寄存器说明: BUCKO1模式选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck01_mode_ctrl : 1;  /* bit[0]  : BUCK0/1模式选择。
                                                           0：增减功率管；
                                                           1：增减BUCK。 */
        unsigned char  buck01_phase_sel : 1;  /* bit[1]  : BUCK0/1相位控制。
                                                           0：单相；
                                                           1：双相。 */
        unsigned char  reserved         : 6;  /* bit[2-7]: 保留 */
    } reg;
} SOC_SMART_BUCK01_MOD_CTRL_UNION;
#endif
#define SOC_SMART_BUCK01_MOD_CTRL_buck01_mode_ctrl_START  (0)
#define SOC_SMART_BUCK01_MOD_CTRL_buck01_mode_ctrl_END    (0)
#define SOC_SMART_BUCK01_MOD_CTRL_buck01_phase_sel_START  (1)
#define SOC_SMART_BUCK01_MOD_CTRL_buck01_phase_sel_END    (1)


/*****************************************************************************
 结构名    : SOC_SMART_SENSOR_CLK_CTRL_UNION
 结构说明  : SENSOR_CLK_CTRL 寄存器结构定义。地址偏移量:0x0F9，初值:0x00，宽度:8
 寄存器说明: sensor时钟控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sensor_clk_en      : 1;  /* bit[0]  : sensor时钟分频器输出使能：
                                                             1'b0:关闭；
                                                             1'b1:打开。 */
        unsigned char  sensor_clk_div_num : 2;  /* bit[1-2]: sensor时钟分频配置：
                                                             2'b00:1分频；
                                                             2'b01:2分频；
                                                             2'b10:4分频；
                                                             2'b11:8分频； */
        unsigned char  reserved           : 5;  /* bit[3-7]: 保留 */
    } reg;
} SOC_SMART_SENSOR_CLK_CTRL_UNION;
#endif
#define SOC_SMART_SENSOR_CLK_CTRL_sensor_clk_en_START       (0)
#define SOC_SMART_SENSOR_CLK_CTRL_sensor_clk_en_END         (0)
#define SOC_SMART_SENSOR_CLK_CTRL_sensor_clk_div_num_START  (1)
#define SOC_SMART_SENSOR_CLK_CTRL_sensor_clk_div_num_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_CK_19M2_CTRL_UNION
 结构说明  : CK_19M2_CTRL 寄存器结构定义。地址偏移量:0x0FA，初值:0x00，宽度:8
 寄存器说明: CK_19M2 pin脚输出时钟控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ck_19m2_clk_en : 1;  /* bit[0]  : ck_19m2pin脚时钟输出使能：
                                                         1'b0:关闭；
                                                         1'b1:打开。 */
        unsigned char  reserved       : 7;  /* bit[1-7]: 保留 */
    } reg;
} SOC_SMART_CK_19M2_CTRL_UNION;
#endif
#define SOC_SMART_CK_19M2_CTRL_ck_19m2_clk_en_START  (0)
#define SOC_SMART_CK_19M2_CTRL_ck_19m2_clk_en_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_EXTBUCK_MOD_SEL_UNION
 结构说明  : EXTBUCK_MOD_SEL 寄存器结构定义。地址偏移量:0x0FB，初值:0x00，宽度:8
 寄存器说明: EXTBUCK控制模式选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  extbuck_mod_sel : 1;  /* bit[0]  : EXTBUCK控制模式选择：
                                                          1'b0:受控于数模接口；
                                                          1'b1:受控于软件配置。
                                                          (见0x26,27,28描述) */
        unsigned char  reserved        : 7;  /* bit[1-7]: 保留 */
    } reg;
} SOC_SMART_EXTBUCK_MOD_SEL_UNION;
#endif
#define SOC_SMART_EXTBUCK_MOD_SEL_extbuck_mod_sel_START  (0)
#define SOC_SMART_EXTBUCK_MOD_SEL_extbuck_mod_sel_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_XO_WAVE_CTRL_UNION
 结构说明  : XO_WAVE_CTRL 寄存器结构定义。地址偏移量:0x0FD，初值:0x00，宽度:8
 寄存器说明: XO时钟输出波形控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  xo_tri_cap   : 3;  /* bit[0-2]: 三角波产生电路的电容控制 */
        unsigned char  xo_sys_phase : 1;  /* bit[3]  : 0表示时钟异相，1表示时钟同相 */
        unsigned char  xo_c1_cap    : 1;  /* bit[4]  : 0表示不增加电容，1表示新增2pF电容 */
        unsigned char  reserved     : 3;  /* bit[5-7]: 保留 */
    } reg;
} SOC_SMART_XO_WAVE_CTRL_UNION;
#endif
#define SOC_SMART_XO_WAVE_CTRL_xo_tri_cap_START    (0)
#define SOC_SMART_XO_WAVE_CTRL_xo_tri_cap_END      (2)
#define SOC_SMART_XO_WAVE_CTRL_xo_sys_phase_START  (3)
#define SOC_SMART_XO_WAVE_CTRL_xo_sys_phase_END    (3)
#define SOC_SMART_XO_WAVE_CTRL_xo_c1_cap_START     (4)
#define SOC_SMART_XO_WAVE_CTRL_xo_c1_cap_END       (4)




/****************************************************************************
                     (2/2) NP_PMU_CTRL
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_SMART_NP_IRQ1_RECORD_UNION
 结构说明  : NP_IRQ1_RECORD 寄存器结构定义。地址偏移量:0x011，初值:0x00，宽度:8
 寄存器说明: 中断1非下电记录寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_otmp_d1r        : 1;  /* bit[0]: 温度报警(默认125°)：
                                                           0:没有发生过该事件；
                                                           1:发生该事件。 */
        unsigned char  np_hreset_n_f      : 1;  /* bit[1]: 热复位：
                                                           0:没有发生过该事件；
                                                           1:发生该事件。 */
        unsigned char  np_vsys_2p5_r      : 1;  /* bit[2]: vsys < 2.5V：
                                                           0:没有发生过该事件；
                                                           1:发生该事件。 */
        unsigned char  np_vsys_uv_d3r     : 1;  /* bit[3]: vsys < 3.0V 3ms:
                                                           0:没有发生过该事件；
                                                           1:发生该事件。 */
        unsigned char  np_vsys_6p0_d200ur : 1;  /* bit[4]: vsys> 6.0V 200us:
                                                           0:没有发生过该事件；
                                                           1:发生该事件。 */
        unsigned char  np_pwron_d4sr      : 1;  /* bit[5]: 在正常工作状态下PWRON按下4s:
                                                           0:没有发生过该事件；
                                                           1:发生该事件。 */
        unsigned char  np_pwron_d20f      : 1;  /* bit[6]: PWRON按键释放20ms：
                                                           0:没有发生过该事件；
                                                           1:发生该事件。 */
        unsigned char  np_pwron_d20r      : 1;  /* bit[7]: PWRON按键按下20ms：
                                                           0:没有发生过该事件；
                                                           1:发生该事件。 */
    } reg;
} SOC_SMART_NP_IRQ1_RECORD_UNION;
#endif
#define SOC_SMART_NP_IRQ1_RECORD_np_otmp_d1r_START         (0)
#define SOC_SMART_NP_IRQ1_RECORD_np_otmp_d1r_END           (0)
#define SOC_SMART_NP_IRQ1_RECORD_np_hreset_n_f_START       (1)
#define SOC_SMART_NP_IRQ1_RECORD_np_hreset_n_f_END         (1)
#define SOC_SMART_NP_IRQ1_RECORD_np_vsys_2p5_r_START       (2)
#define SOC_SMART_NP_IRQ1_RECORD_np_vsys_2p5_r_END         (2)
#define SOC_SMART_NP_IRQ1_RECORD_np_vsys_uv_d3r_START      (3)
#define SOC_SMART_NP_IRQ1_RECORD_np_vsys_uv_d3r_END        (3)
#define SOC_SMART_NP_IRQ1_RECORD_np_vsys_6p0_d200ur_START  (4)
#define SOC_SMART_NP_IRQ1_RECORD_np_vsys_6p0_d200ur_END    (4)
#define SOC_SMART_NP_IRQ1_RECORD_np_pwron_d4sr_START       (5)
#define SOC_SMART_NP_IRQ1_RECORD_np_pwron_d4sr_END         (5)
#define SOC_SMART_NP_IRQ1_RECORD_np_pwron_d20f_START       (6)
#define SOC_SMART_NP_IRQ1_RECORD_np_pwron_d20f_END         (6)
#define SOC_SMART_NP_IRQ1_RECORD_np_pwron_d20r_START       (7)
#define SOC_SMART_NP_IRQ1_RECORD_np_pwron_d20r_END         (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_IRQ2_RECORD_UNION
 结构说明  : NP_IRQ2_RECORD 寄存器结构定义。地址偏移量:0x012，初值:0x01，宽度:8
 寄存器说明: 中断2非下电记录寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_vbat_vcoin_sel     : 1;  /* bit[0]: VBAT与VCION切换信号：
                                                              0:没有发生过该事件；
                                                              1:发生该事件，VBAT。 */
        unsigned char  np_vbus_det_3p6_d150r : 1;  /* bit[1]: vbus_det> 3.6V 150ms：
                                                              0:没有发生过该事件；
                                                              1:发生该事件。 */
        unsigned char  np_vbus_det_3p6_d20f  : 1;  /* bit[2]: vbus_det< 3.6V 20ms：
                                                              0:没有发生过该事件；
                                                              1:发生该事件。 */
        unsigned char  np_vbus_det_3p6_d20r  : 1;  /* bit[3]: vbus_det> 3.6V 20ms：
                                                              0:没有发生过该事件；
                                                              1:发生该事件。 */
        unsigned char  np_pwron_restart      : 1;  /* bit[4]: 通过pwron长按键重启:
                                                              0:没有发生过该事件；
                                                              1:发生该事件。 */
        unsigned char  np_pwron_dnsr         : 1;  /* bit[5]: 在正常工作下pwron按下Ns关机:
                                                              0:没有发生过该事件；
                                                              1:发生该事件。 */
        unsigned char  np_pwron_d500r        : 1;  /* bit[6]: 关机状态下PWRON按下500ms:
                                                              0:没有发生过该事件；
                                                              1:发生该事件。 */
        unsigned char  np_otmp150_d1r        : 1;  /* bit[7]: 温度> 150℃：
                                                              0:没有发生过该事件；
                                                              1:发生该事件。 */
    } reg;
} SOC_SMART_NP_IRQ2_RECORD_UNION;
#endif
#define SOC_SMART_NP_IRQ2_RECORD_np_vbat_vcoin_sel_START      (0)
#define SOC_SMART_NP_IRQ2_RECORD_np_vbat_vcoin_sel_END        (0)
#define SOC_SMART_NP_IRQ2_RECORD_np_vbus_det_3p6_d150r_START  (1)
#define SOC_SMART_NP_IRQ2_RECORD_np_vbus_det_3p6_d150r_END    (1)
#define SOC_SMART_NP_IRQ2_RECORD_np_vbus_det_3p6_d20f_START   (2)
#define SOC_SMART_NP_IRQ2_RECORD_np_vbus_det_3p6_d20f_END     (2)
#define SOC_SMART_NP_IRQ2_RECORD_np_vbus_det_3p6_d20r_START   (3)
#define SOC_SMART_NP_IRQ2_RECORD_np_vbus_det_3p6_d20r_END     (3)
#define SOC_SMART_NP_IRQ2_RECORD_np_pwron_restart_START       (4)
#define SOC_SMART_NP_IRQ2_RECORD_np_pwron_restart_END         (4)
#define SOC_SMART_NP_IRQ2_RECORD_np_pwron_dnsr_START          (5)
#define SOC_SMART_NP_IRQ2_RECORD_np_pwron_dnsr_END            (5)
#define SOC_SMART_NP_IRQ2_RECORD_np_pwron_d500r_START         (6)
#define SOC_SMART_NP_IRQ2_RECORD_np_pwron_d500r_END           (6)
#define SOC_SMART_NP_IRQ2_RECORD_np_otmp150_d1r_START         (7)
#define SOC_SMART_NP_IRQ2_RECORD_np_otmp150_d1r_END           (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_IRQ3_RECORD_UNION
 结构说明  : NP_IRQ3_RECORD 寄存器结构定义。地址偏移量:0x013，初值:0x00，宽度:8
 寄存器说明: 中断3非下电记录寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_tcxo_clk_sel_r   : 1;  /* bit[0]: 32kRC切换到32K Crystal OSC 时钟：
                                                            0:没有发生过该事件；
                                                            1:发生该事件。 */
        unsigned char  np_tcxo_clk_sel_f   : 1;  /* bit[1]: 32K Crystal OSC 切换到32kRC时钟：
                                                            0:没有发生过该事件；
                                                            1:发生该事件。 */
        unsigned char  np_dcxo_clk_sel_r   : 1;  /* bit[2]: 32K RC 切换到19.2MHz/600 时钟：
                                                            0:没有发生过该事件；
                                                            1:发生该事件。 */
        unsigned char  np_dcxo_clk_sel_f   : 1;  /* bit[3]: 19.2MHz/600 切换到32K RC时钟：
                                                            0:没有发生过该事件；
                                                            1:发生该事件。 */
        unsigned char  np_alarm_on         : 1;  /* bit[4]: 触发闹钟：
                                                            0:没有发生过该事件；
                                                            1:发生该事件。 */
        unsigned char  np_pwr_hold_2d_pwrn : 1;  /* bit[5]: pwr_hold开机：
                                                            0:没有发生过该事件；
                                                            1:发生该事件。 */
        unsigned char  np_pwr_hold_2d_pwrd : 1;  /* bit[6]: pwr_hold关机：
                                                            0:没有发生过该事件；
                                                            1:发生该事件。 */
        unsigned char  np_pdns_rst_sel     : 1;  /* bit[7]: 长按键NS复位：
                                                            0:没有发生过该事件；
                                                            1:发生该事件。 */
    } reg;
} SOC_SMART_NP_IRQ3_RECORD_UNION;
#endif
#define SOC_SMART_NP_IRQ3_RECORD_np_tcxo_clk_sel_r_START    (0)
#define SOC_SMART_NP_IRQ3_RECORD_np_tcxo_clk_sel_r_END      (0)
#define SOC_SMART_NP_IRQ3_RECORD_np_tcxo_clk_sel_f_START    (1)
#define SOC_SMART_NP_IRQ3_RECORD_np_tcxo_clk_sel_f_END      (1)
#define SOC_SMART_NP_IRQ3_RECORD_np_dcxo_clk_sel_r_START    (2)
#define SOC_SMART_NP_IRQ3_RECORD_np_dcxo_clk_sel_r_END      (2)
#define SOC_SMART_NP_IRQ3_RECORD_np_dcxo_clk_sel_f_START    (3)
#define SOC_SMART_NP_IRQ3_RECORD_np_dcxo_clk_sel_f_END      (3)
#define SOC_SMART_NP_IRQ3_RECORD_np_alarm_on_START          (4)
#define SOC_SMART_NP_IRQ3_RECORD_np_alarm_on_END            (4)
#define SOC_SMART_NP_IRQ3_RECORD_np_pwr_hold_2d_pwrn_START  (5)
#define SOC_SMART_NP_IRQ3_RECORD_np_pwr_hold_2d_pwrn_END    (5)
#define SOC_SMART_NP_IRQ3_RECORD_np_pwr_hold_2d_pwrd_START  (6)
#define SOC_SMART_NP_IRQ3_RECORD_np_pwr_hold_2d_pwrd_END    (6)
#define SOC_SMART_NP_IRQ3_RECORD_np_pdns_rst_sel_START      (7)
#define SOC_SMART_NP_IRQ3_RECORD_np_pdns_rst_sel_END        (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_SCP_RECORD1_UNION
 结构说明  : NP_SCP_RECORD1 寄存器结构定义。地址偏移量:0x014，初值:0x00，宽度:8
 寄存器说明: 短路保护非下电记录1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_scp_buck01 : 1;  /* bit[0]  : 0：仅记录BUCK01输出不短路；
                                                        1: 仅记录BUCK01输出短路。 */
        unsigned char  np_scp_buck2  : 1;  /* bit[1]  : 0：仅记录BUCK2输出不短路；
                                                        1: 仅记录BUCK2输出短路。 */
        unsigned char  np_scp_buck3  : 1;  /* bit[2]  : 0：仅记录BUCK3输出不短路；
                                                        1: 仅记录BUCK3输出短路。 */
        unsigned char  np_scp_buck4  : 1;  /* bit[3]  : 0：仅记录BUCK4输出不短路；
                                                        1: 仅记录BUCK4输出短路。 */
        unsigned char  reserved      : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_SMART_NP_SCP_RECORD1_UNION;
#endif
#define SOC_SMART_NP_SCP_RECORD1_np_scp_buck01_START  (0)
#define SOC_SMART_NP_SCP_RECORD1_np_scp_buck01_END    (0)
#define SOC_SMART_NP_SCP_RECORD1_np_scp_buck2_START   (1)
#define SOC_SMART_NP_SCP_RECORD1_np_scp_buck2_END     (1)
#define SOC_SMART_NP_SCP_RECORD1_np_scp_buck3_START   (2)
#define SOC_SMART_NP_SCP_RECORD1_np_scp_buck3_END     (2)
#define SOC_SMART_NP_SCP_RECORD1_np_scp_buck4_START   (3)
#define SOC_SMART_NP_SCP_RECORD1_np_scp_buck4_END     (3)


/*****************************************************************************
 结构名    : SOC_SMART_NP_OCP_RECORD1_UNION
 结构说明  : NP_OCP_RECORD1 寄存器结构定义。地址偏移量:0x015，初值:0x00，宽度:8
 寄存器说明: 过流保护非下电记录1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_ocp_buck1 : 1;  /* bit[0]  : 0：仅记录BUCK1输出不过载；
                                                       1: 仅记录BUCK1输出过载。 */
        unsigned char  np_ocp_buck2 : 1;  /* bit[1]  : 0：仅记录BUCK2输出不过载；
                                                       1: 仅记录BUCK2输出过载。 */
        unsigned char  np_ocp_buck3 : 1;  /* bit[2]  : 0：仅记录BUCK3输出不过载；
                                                       1: 仅记录BUCK3输出过载。 */
        unsigned char  np_ocp_buck4 : 1;  /* bit[3]  : 0：仅记录BUCK4输出不过载；
                                                       1: 仅记录BUCK4输出过载。 */
        unsigned char  np_ocp_buck0 : 1;  /* bit[4]  : 0：仅记录BUCK0输出不过载；
                                                       1: 仅记录BUCK0输出过载。 */
        unsigned char  reserved     : 3;  /* bit[5-7]: 保留。 */
    } reg;
} SOC_SMART_NP_OCP_RECORD1_UNION;
#endif
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck1_START  (0)
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck1_END    (0)
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck2_START  (1)
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck2_END    (1)
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck3_START  (2)
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck3_END    (2)
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck4_START  (3)
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck4_END    (3)
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck0_START  (4)
#define SOC_SMART_NP_OCP_RECORD1_np_ocp_buck0_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_NP_OCP_RECORD2_UNION
 结构说明  : NP_OCP_RECORD2 寄存器结构定义。地址偏移量:0x016，初值:0x00，宽度:8
 寄存器说明: 过流保护非下电记录2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_ocp_ldo1 : 1;  /* bit[0]: 0：仅记录LDO1输出不过载；
                                                    1: 仅记录LDO1输出过载。 */
        unsigned char  np_ocp_ldo2 : 1;  /* bit[1]: 0：仅记录LDO2输出不过载；
                                                    1: 仅记录LDO2输出过载。 */
        unsigned char  np_ocp_ldo3 : 1;  /* bit[2]: 0：仅记录LDO3输出不过载；
                                                    1: 仅记录LDO3输出过载。 */
        unsigned char  np_ocp_ldo4 : 1;  /* bit[3]: 0：仅记录LDO4输出不过载；
                                                    1: 仅记录LDO4输出过载。 */
        unsigned char  np_ocp_ldo5 : 1;  /* bit[4]: 0：仅记录LDO5输出不过载；
                                                    1: 仅记录LDO5输出过载。 */
        unsigned char  np_ocp_ldo6 : 1;  /* bit[5]: 0：仅记录LDO6输出不过载；
                                                    1: 仅记录LDO6输出过载。 */
        unsigned char  np_ocp_ldo7 : 1;  /* bit[6]: 0：仅记录LDO7输出不过载；
                                                    1: 仅记录LDO7输出过载。 */
        unsigned char  np_ocp_ldo8 : 1;  /* bit[7]: 0：仅记录LDO8输出不过载；
                                                    1: 仅记录LDO8输出过载。 */
    } reg;
} SOC_SMART_NP_OCP_RECORD2_UNION;
#endif
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo1_START  (0)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo1_END    (0)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo2_START  (1)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo2_END    (1)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo3_START  (2)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo3_END    (2)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo4_START  (3)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo4_END    (3)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo5_START  (4)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo5_END    (4)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo6_START  (5)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo6_END    (5)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo7_START  (6)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo7_END    (6)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo8_START  (7)
#define SOC_SMART_NP_OCP_RECORD2_np_ocp_ldo8_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_OCP_RECORD3_UNION
 结构说明  : NP_OCP_RECORD3 寄存器结构定义。地址偏移量:0x017，初值:0x00，宽度:8
 寄存器说明: 过流保护非下电记录3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_ocp_ldo9  : 1;  /* bit[0]: 0：仅记录LDO9输出不过载；
                                                     1: 仅记录LDO9输出过载。 */
        unsigned char  np_ocp_ldo10 : 1;  /* bit[1]: 0：仅记录LDO10输出不过载；
                                                     1: 仅记录LDO10输出过载。 */
        unsigned char  np_ocp_ldo11 : 1;  /* bit[2]: 0：仅记录LDO11输出不过载；
                                                     1: 仅记录LDO11输出过载。 */
        unsigned char  np_ocp_ldo12 : 1;  /* bit[3]: 0：仅记录LDO12输出不过载；
                                                     1: 仅记录LDO12输出过载。 */
        unsigned char  np_ocp_ldo13 : 1;  /* bit[4]: 0：仅记录LDO13输出不过载；
                                                     1: 仅记录LDO13输出过载。 */
        unsigned char  np_ocp_ldo14 : 1;  /* bit[5]: 0：仅记录LDO14输出不过载；
                                                     1: 仅记录LDO14输出过载。 */
        unsigned char  np_ocp_ldo15 : 1;  /* bit[6]: 0：仅记录LDO15输出不过载；
                                                     1: 仅记录LDO15输出过载。 */
        unsigned char  np_ocp_ldo16 : 1;  /* bit[7]: 0：仅记录LDO16输出不过载；
                                                     1: 仅记录LDO16输出过载。 */
    } reg;
} SOC_SMART_NP_OCP_RECORD3_UNION;
#endif
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo9_START   (0)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo9_END     (0)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo10_START  (1)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo10_END    (1)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo11_START  (2)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo11_END    (2)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo12_START  (3)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo12_END    (3)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo13_START  (4)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo13_END    (4)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo14_START  (5)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo14_END    (5)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo15_START  (6)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo15_END    (6)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo16_START  (7)
#define SOC_SMART_NP_OCP_RECORD3_np_ocp_ldo16_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_OCP_RECORD4_UNION
 结构说明  : NP_OCP_RECORD4 寄存器结构定义。地址偏移量:0x018，初值:0x00，宽度:8
 寄存器说明: 过流保护非下电记录4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_ocp_ldo17 : 1;  /* bit[0]  : 0：仅记录LDO17输出不过载；
                                                       1: 仅记录LDO17输出过载。 */
        unsigned char  np_ocp_ldo18 : 1;  /* bit[1]  : 0：仅记录LDO18输出不过载；
                                                       1: 仅记录LDO18输出过载。 */
        unsigned char  np_ocp_ldo19 : 1;  /* bit[2]  : 0：仅记录LDO19输出不过载；
                                                       1: 仅记录LDO19输出过载。 */
        unsigned char  np_ocp_ldo20 : 1;  /* bit[3]  : 0：仅记录LDO20输出不过载；
                                                       1: 仅记录LDO20输出过载。 */
        unsigned char  np_ocp_ldo21 : 1;  /* bit[4]  : 0：仅记录LDO21输出不过载；
                                                       1: 仅记录LDO21输出过载。 */
        unsigned char  np_ocp_ldo22 : 1;  /* bit[5]  : 0：仅记录LDO22输出不过载；
                                                       1: 仅记录LDO22输出过载。 */
        unsigned char  reserved     : 2;  /* bit[6-7]: 保留 */
    } reg;
} SOC_SMART_NP_OCP_RECORD4_UNION;
#endif
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo17_START  (0)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo17_END    (0)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo18_START  (1)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo18_END    (1)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo19_START  (2)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo19_END    (2)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo20_START  (3)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo20_END    (3)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo21_START  (4)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo21_END    (4)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo22_START  (5)
#define SOC_SMART_NP_OCP_RECORD4_np_ocp_ldo22_END    (5)


/*****************************************************************************
 结构名    : SOC_SMART_NP_OCP_RECORD5_UNION
 结构说明  : NP_OCP_RECORD5 寄存器结构定义。地址偏移量:0x019，初值:0x00，宽度:8
 寄存器说明: 过流保护非下电记录5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_ocp_lvs2   : 1;  /* bit[0]  : 0：仅记录LVS2输出不过载；
                                                        1: 仅记录LVS2输出过载。 */
        unsigned char  np_ocp_classd : 1;  /* bit[1]  : 0：仅记录CLASSD输出不过流；
                                                        1: 仅记录CLASSD输出过流。 */
        unsigned char  np_scp_pmua   : 1;  /* bit[2]  : 0：仅记录PMUA输出不短路；
                                                        1: 仅记录PMUA输出不短路； */
        unsigned char  reserved      : 5;  /* bit[3-7]: 保留 */
    } reg;
} SOC_SMART_NP_OCP_RECORD5_UNION;
#endif
#define SOC_SMART_NP_OCP_RECORD5_np_ocp_lvs2_START    (0)
#define SOC_SMART_NP_OCP_RECORD5_np_ocp_lvs2_END      (0)
#define SOC_SMART_NP_OCP_RECORD5_np_ocp_classd_START  (1)
#define SOC_SMART_NP_OCP_RECORD5_np_ocp_classd_END    (1)
#define SOC_SMART_NP_OCP_RECORD5_np_scp_pmua_START    (2)
#define SOC_SMART_NP_OCP_RECORD5_np_scp_pmua_END      (2)


/*****************************************************************************
 结构名    : SOC_SMART_CLK_TOP2_UNION
 结构说明  : CLK_TOP2 寄存器结构定义。地址偏移量:0x065，初值:0x50，宽度:8
 寄存器说明: CLK_TOP控制2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  trim_c1fix : 8;  /* bit[0-7]: C1电容大小trim */
    } reg;
} SOC_SMART_CLK_TOP2_UNION;
#endif
#define SOC_SMART_CLK_TOP2_trim_c1fix_START  (0)
#define SOC_SMART_CLK_TOP2_trim_c1fix_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCDR0_UNION
 结构说明  : RTCDR0 寄存器结构定义。地址偏移量:0x0A9，初值:0x00，宽度:8
 寄存器说明: RTC当前时间bit[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcdr0 : 8;  /* bit[0-7]: 读此寄存器，返回当前RTC值（32bit）中的bit[7:0]。 */
    } reg;
} SOC_SMART_RTCDR0_UNION;
#endif
#define SOC_SMART_RTCDR0_rtcdr0_START  (0)
#define SOC_SMART_RTCDR0_rtcdr0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCDR1_UNION
 结构说明  : RTCDR1 寄存器结构定义。地址偏移量:0x0AA，初值:0x00，宽度:8
 寄存器说明: RTC当前时间bit[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcdr1 : 8;  /* bit[0-7]: 读此寄存器，返回当前RTC值（32bit）中的bit[15:8]。 */
    } reg;
} SOC_SMART_RTCDR1_UNION;
#endif
#define SOC_SMART_RTCDR1_rtcdr1_START  (0)
#define SOC_SMART_RTCDR1_rtcdr1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCDR2_UNION
 结构说明  : RTCDR2 寄存器结构定义。地址偏移量:0x0AB，初值:0x00，宽度:8
 寄存器说明: RTC当前时间bit[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcdr2 : 8;  /* bit[0-7]: 读此寄存器，返回当前RTC值（32bit）中的bit[23:16]。 */
    } reg;
} SOC_SMART_RTCDR2_UNION;
#endif
#define SOC_SMART_RTCDR2_rtcdr2_START  (0)
#define SOC_SMART_RTCDR2_rtcdr2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCDR3_UNION
 结构说明  : RTCDR3 寄存器结构定义。地址偏移量:0x0AC，初值:0x00，宽度:8
 寄存器说明: RTC当前时间bit[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcdr3 : 8;  /* bit[0-7]: 读此寄存器，返回当前RTC值（32bit）中的bit[31:24]。 */
    } reg;
} SOC_SMART_RTCDR3_UNION;
#endif
#define SOC_SMART_RTCDR3_rtcdr3_START  (0)
#define SOC_SMART_RTCDR3_rtcdr3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCMR0_UNION
 结构说明  : RTCMR0 寄存器结构定义。地址偏移量:0x0AD，初值:0x00，宽度:8
 寄存器说明: RTC比较bit[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcmr0 : 8;  /* bit[0-7]: 可设置闹铃时间值（32bit）中的bit[7:0]。
                                                 读此寄存器，返回最后一次闹铃时间设置值的bit[7:0]。 */
    } reg;
} SOC_SMART_RTCMR0_UNION;
#endif
#define SOC_SMART_RTCMR0_rtcmr0_START  (0)
#define SOC_SMART_RTCMR0_rtcmr0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCMR1_UNION
 结构说明  : RTCMR1 寄存器结构定义。地址偏移量:0x0AE，初值:0x00，宽度:8
 寄存器说明: RTC比较bit[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcmr1 : 8;  /* bit[0-7]: 可设置闹铃时间值（32bit）中的bit[15:8]。
                                                 读此寄存器，返回最后一次闹铃时间设置值的bit[15:8]。 */
    } reg;
} SOC_SMART_RTCMR1_UNION;
#endif
#define SOC_SMART_RTCMR1_rtcmr1_START  (0)
#define SOC_SMART_RTCMR1_rtcmr1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCMR2_UNION
 结构说明  : RTCMR2 寄存器结构定义。地址偏移量:0x0AF，初值:0x00，宽度:8
 寄存器说明: RTC比较bit[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcmr2 : 8;  /* bit[0-7]: 可设置闹铃时间值（32bit）中的bit[23:16]。
                                                 读此寄存器，返回最后一次闹铃时间设置值的bit[23:16]。 */
    } reg;
} SOC_SMART_RTCMR2_UNION;
#endif
#define SOC_SMART_RTCMR2_rtcmr2_START  (0)
#define SOC_SMART_RTCMR2_rtcmr2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCMR3_UNION
 结构说明  : RTCMR3 寄存器结构定义。地址偏移量:0x0B0，初值:0x00，宽度:8
 寄存器说明: RTC比较bit[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcmr3 : 8;  /* bit[0-7]: 可设置闹铃时间值（32bit）中的bit[31:24]。
                                                 读此寄存器，返回最后一次闹铃时间设置值的bit[31:24]。 */
    } reg;
} SOC_SMART_RTCMR3_UNION;
#endif
#define SOC_SMART_RTCMR3_rtcmr3_START  (0)
#define SOC_SMART_RTCMR3_rtcmr3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCLR0_UNION
 结构说明  : RTCLR0 寄存器结构定义。地址偏移量:0x0B1，初值:0x00，宽度:8
 寄存器说明: RTC加载bit[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcclr0 : 8;  /* bit[0-7]: 可设置当前时间值（32bit）中的bit[7:0]。
                                                  读此寄存器，返回最后一次当前时间设置值的bit[7:0]。 */
    } reg;
} SOC_SMART_RTCLR0_UNION;
#endif
#define SOC_SMART_RTCLR0_rtcclr0_START  (0)
#define SOC_SMART_RTCLR0_rtcclr0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCLR1_UNION
 结构说明  : RTCLR1 寄存器结构定义。地址偏移量:0x0B2，初值:0x00，宽度:8
 寄存器说明: RTC加载bit[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcclr1 : 8;  /* bit[0-7]: 可设置当前时间值（32bit）中的bit[15:8]。
                                                  读此寄存器，返回最后一次当前时间设置值的bit[15:8]。 */
    } reg;
} SOC_SMART_RTCLR1_UNION;
#endif
#define SOC_SMART_RTCLR1_rtcclr1_START  (0)
#define SOC_SMART_RTCLR1_rtcclr1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCLR2_UNION
 结构说明  : RTCLR2 寄存器结构定义。地址偏移量:0x0B3，初值:0x00，宽度:8
 寄存器说明: RTC加载bit[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcclr2 : 8;  /* bit[0-7]: 可设置当前时间值（32bit）中的bit[23:16]。
                                                  读此寄存器，返回最后一次当前时间设置值的bit[23:16]。 */
    } reg;
} SOC_SMART_RTCLR2_UNION;
#endif
#define SOC_SMART_RTCLR2_rtcclr2_START  (0)
#define SOC_SMART_RTCLR2_rtcclr2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCLR3_UNION
 结构说明  : RTCLR3 寄存器结构定义。地址偏移量:0x0B4，初值:0x00，宽度:8
 寄存器说明: RTC加载bit[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtcclr3 : 8;  /* bit[0-7]: 可设置当前时间值（32bit）中的bit[31:24]。
                                                  读此寄存器，返回最后一次当前时间设置值的bit[31:24]。 */
    } reg;
} SOC_SMART_RTCLR3_UNION;
#endif
#define SOC_SMART_RTCLR3_rtcclr3_START  (0)
#define SOC_SMART_RTCLR3_rtcclr3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTCCTRL_UNION
 结构说明  : RTCCTRL 寄存器结构定义。地址偏移量:0x0B5，初值:0x01，宽度:8
 寄存器说明: RTC控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtccr    : 1;  /* bit[0]  : 读RTC控制位
                                                   0：读RTC当前值，返回值全部为0；
                                                   1：读RTC当前值，返回值为实际当前值； */
        unsigned char  reserved : 7;  /* bit[1-7]: 保留。 */
    } reg;
} SOC_SMART_RTCCTRL_UNION;
#endif
#define SOC_SMART_RTCCTRL_rtccr_START     (0)
#define SOC_SMART_RTCCTRL_rtccr_END       (0)


/*****************************************************************************
 结构名    : SOC_SMART_OTP_PDOB0_UNION
 结构说明  : OTP_PDOB0 寄存器结构定义。地址偏移量:0x0A5，初值:0x00，宽度:8
 寄存器说明: OTP读出值0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pdob0 : 8;  /* bit[0-7]: OTP_PDOB[7:0] */
    } reg;
} SOC_SMART_OTP_PDOB0_UNION;
#endif
#define SOC_SMART_OTP_PDOB0_otp_pdob0_START  (0)
#define SOC_SMART_OTP_PDOB0_otp_pdob0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OTP_PDOB1_UNION
 结构说明  : OTP_PDOB1 寄存器结构定义。地址偏移量:0x0A6，初值:0x00，宽度:8
 寄存器说明: OTP读出值1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pdob1 : 8;  /* bit[0-7]: OTP_PDOB[15:8] */
    } reg;
} SOC_SMART_OTP_PDOB1_UNION;
#endif
#define SOC_SMART_OTP_PDOB1_otp_pdob1_START  (0)
#define SOC_SMART_OTP_PDOB1_otp_pdob1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OTP_PDOB2_UNION
 结构说明  : OTP_PDOB2 寄存器结构定义。地址偏移量:0x0A7，初值:0x00，宽度:8
 寄存器说明: OTP读出值2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pdob2 : 8;  /* bit[0-7]: OTP_PDOB[23:16] */
    } reg;
} SOC_SMART_OTP_PDOB2_UNION;
#endif
#define SOC_SMART_OTP_PDOB2_otp_pdob2_START  (0)
#define SOC_SMART_OTP_PDOB2_otp_pdob2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OTP_PDOB3_UNION
 结构说明  : OTP_PDOB3 寄存器结构定义。地址偏移量:0x0A8，初值:0x00，宽度:8
 寄存器说明: OTP读出值3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otp_pdob3 : 8;  /* bit[0-7]: OTP_PDOB[31:24] */
    } reg;
} SOC_SMART_OTP_PDOB3_UNION;
#endif
#define SOC_SMART_OTP_PDOB3_otp_pdob3_START  (0)
#define SOC_SMART_OTP_PDOB3_otp_pdob3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CRC_VALUE0_UNION
 结构说明  : CRC_VALUE0 寄存器结构定义。地址偏移量:0x0BB，初值:0xC0，宽度:8
 寄存器说明: CRC_VALUE[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  crc_value0 : 8;  /* bit[0-7]: DCXO模式下，在RTC时钟校准状态下得到的RC时钟校准值[7:0]。 */
    } reg;
} SOC_SMART_CRC_VALUE0_UNION;
#endif
#define SOC_SMART_CRC_VALUE0_crc_value0_START  (0)
#define SOC_SMART_CRC_VALUE0_crc_value0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CRC_VALUE1_UNION
 结构说明  : CRC_VALUE1 寄存器结构定义。地址偏移量:0x0BC，初值:0x27，宽度:8
 寄存器说明: CRC_VALUE[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  crc_value1 : 8;  /* bit[0-7]: DCXO模式下，在RTC时钟校准状态下得到的RC时钟校准值[15:8]。 */
    } reg;
} SOC_SMART_CRC_VALUE1_UNION;
#endif
#define SOC_SMART_CRC_VALUE1_crc_value1_START  (0)
#define SOC_SMART_CRC_VALUE1_crc_value1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CRC_VALUE2_UNION
 结构说明  : CRC_VALUE2 寄存器结构定义。地址偏移量:0x0BD，初值:0x09，宽度:8
 寄存器说明: CRC_VALUE[20:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  crc_value2 : 5;  /* bit[0-4]: DCXO模式下，在RTC时钟校准状态下得到的RC时钟校准值[20:16]。 */
        unsigned char  reserved   : 3;  /* bit[5-7]: 保留 */
    } reg;
} SOC_SMART_CRC_VALUE2_UNION;
#endif
#define SOC_SMART_CRC_VALUE2_crc_value2_START  (0)
#define SOC_SMART_CRC_VALUE2_crc_value2_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_NP_REG_ADJ1_UNION
 结构说明  : NP_REG_ADJ1 寄存器结构定义。地址偏移量:0x0BE，初值:0x02，宽度:8
 寄存器说明: 非下电调节1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pdns_time_sel : 3;  /* bit[0-2]: 长按键Ns关机可配寄存器：
                                                        000:6s;
                                                        001:7s;
                                                        010:8s;
                                                        011:9s;
                                                        100:10s;
                                                        101:11s;
                                                        其他：8s。 */
        unsigned char  pdns_mk       : 1;  /* bit[3]  : 长按键关机屏蔽寄存器：
                                                        0：不屏蔽长按键关机；
                                                        1：屏蔽长按键关机；
                                                        注：在正常工作中，一直按下pwron_n ，此时才可以通过SSI将长按键关机屏蔽位置一，当pwron_n松开20ms后该位自动清除。 */
        unsigned char  pdns_pd_sel   : 1;  /* bit[4]  : 长按键Ns关机或重启选择：
                                                        0：关机；
                                                        1：重启。 */
        unsigned char  vsys_uv_adj   : 1;  /* bit[5]  : 0：
                                                        vsys上电阈值3.2V；
                                                        vsys下电阈值3.0V；
                                                        1：
                                                        vsys上电阈值3.2V；
                                                        vsys下电阈值3.0V； */
        unsigned char  nopwr_rc_off  : 1;  /* bit[6]  : 关闭内部256K RC时钟:
                                                        0:使能;
                                                        1:关闭。
                                                        与0xEE[0]共同有效时，才能关闭rc256时钟。 */
        unsigned char  pdns_rst_sel  : 1;  /* bit[7]  : 长按键Ns复位选择：
                                                        0：不复位,选择pdns_pd_sel；
                                                        1：复位。 */
    } reg;
} SOC_SMART_NP_REG_ADJ1_UNION;
#endif
#define SOC_SMART_NP_REG_ADJ1_pdns_time_sel_START  (0)
#define SOC_SMART_NP_REG_ADJ1_pdns_time_sel_END    (2)
#define SOC_SMART_NP_REG_ADJ1_pdns_mk_START        (3)
#define SOC_SMART_NP_REG_ADJ1_pdns_mk_END          (3)
#define SOC_SMART_NP_REG_ADJ1_pdns_pd_sel_START    (4)
#define SOC_SMART_NP_REG_ADJ1_pdns_pd_sel_END      (4)
#define SOC_SMART_NP_REG_ADJ1_vsys_uv_adj_START    (5)
#define SOC_SMART_NP_REG_ADJ1_vsys_uv_adj_END      (5)
#define SOC_SMART_NP_REG_ADJ1_nopwr_rc_off_START   (6)
#define SOC_SMART_NP_REG_ADJ1_nopwr_rc_off_END     (6)
#define SOC_SMART_NP_REG_ADJ1_pdns_rst_sel_START   (7)
#define SOC_SMART_NP_REG_ADJ1_pdns_rst_sel_END     (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_REG_ADJ2_UNION
 结构说明  : NP_REG_ADJ2 寄存器结构定义。地址偏移量:0x0BF，初值:0x40，宽度:8
 寄存器说明: 非下电调节2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  lvs_offset      : 2;  /* bit[0-1]: LVS公用offset调节寄存器 */
        unsigned char  buffer_vos_trim : 3;  /* bit[2-4]: TRIM 供给LDO的0.7V基准电压 */
        unsigned char  dac_buffer_en   : 1;  /* bit[5]  : 基准VREF管脚滤波电阻选择ref_res_sel_int&amp;dac_buffer_en:
                                                          00:2K;
                                                          01:2K;
                                                          10:20K;
                                                          11:70K。 */
        unsigned char  ref_res_sel_int : 1;  /* bit[6]  : 基准VREF管脚滤波电阻选择 */
        unsigned char  pmua_short_mk   : 1;  /* bit[7]  : pmua_short为pmua短路信号：
                                                          0:响应pmua_short；
                                                          1：不响应pmua_short。 */
    } reg;
} SOC_SMART_NP_REG_ADJ2_UNION;
#endif
#define SOC_SMART_NP_REG_ADJ2_lvs_offset_START       (0)
#define SOC_SMART_NP_REG_ADJ2_lvs_offset_END         (1)
#define SOC_SMART_NP_REG_ADJ2_buffer_vos_trim_START  (2)
#define SOC_SMART_NP_REG_ADJ2_buffer_vos_trim_END    (4)
#define SOC_SMART_NP_REG_ADJ2_dac_buffer_en_START    (5)
#define SOC_SMART_NP_REG_ADJ2_dac_buffer_en_END      (5)
#define SOC_SMART_NP_REG_ADJ2_ref_res_sel_int_START  (6)
#define SOC_SMART_NP_REG_ADJ2_ref_res_sel_int_END    (6)
#define SOC_SMART_NP_REG_ADJ2_pmua_short_mk_START    (7)
#define SOC_SMART_NP_REG_ADJ2_pmua_short_mk_END      (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_REG_CHG_UNION
 结构说明  : NP_REG_CHG 寄存器结构定义。地址偏移量:0x0C0，初值:0x0C，宽度:8
 寄存器说明: 备用电池非下电调节寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  nopwr_vcoinsl        : 2;  /* bit[0-1]: 备用电池充电电压选择：
                                                               00：2.5V；
                                                               01：3.0V；
                                                               1X：3.3V。 */
        unsigned char  nopwr_en_backup_chg  : 1;  /* bit[2]  : 备用电池充电使能：
                                                               0：不使能；
                                                               1：使能。 */
        unsigned char  nopwr_backup_chg_sel : 1;  /* bit[3]  : 备用电池充电方式选择
                                                               1：限流模式（cion和超级电容）
                                                               0：直通模式（普通电容）。 */
        unsigned char  codec_reg_mask       : 1;  /* bit[4]  : PMU_SSI响应0x000到0x1FF的所有地址：
                                                               0：PMU_SSI响应0x1B0~0x1FF之间的地址操作；
                                                               1：PMU_SSI不响应0x1B0~0x1FF之间的地址操作。 */
        unsigned char  reserved             : 3;  /* bit[5-7]: 保留 */
    } reg;
} SOC_SMART_NP_REG_CHG_UNION;
#endif
#define SOC_SMART_NP_REG_CHG_nopwr_vcoinsl_START         (0)
#define SOC_SMART_NP_REG_CHG_nopwr_vcoinsl_END           (1)
#define SOC_SMART_NP_REG_CHG_nopwr_en_backup_chg_START   (2)
#define SOC_SMART_NP_REG_CHG_nopwr_en_backup_chg_END     (2)
#define SOC_SMART_NP_REG_CHG_nopwr_backup_chg_sel_START  (3)
#define SOC_SMART_NP_REG_CHG_nopwr_backup_chg_sel_END    (3)
#define SOC_SMART_NP_REG_CHG_codec_reg_mask_START        (4)
#define SOC_SMART_NP_REG_CHG_codec_reg_mask_END          (4)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_ADJ1_UNION
 结构说明  : RTC_ADJ1 寄存器结构定义。地址偏移量:0x0C1，初值:0x00，宽度:8
 寄存器说明: rtc_adj[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_clk_step_adj1 : 8;  /* bit[0-7]: TCXO模式下，计时精度可以调节，调节范围+/-192ppm（63拍时钟），调节步进3.05ppm（1拍时钟），默认为0。 */
    } reg;
} SOC_SMART_RTC_ADJ1_UNION;
#endif
#define SOC_SMART_RTC_ADJ1_rtc_clk_step_adj1_START  (0)
#define SOC_SMART_RTC_ADJ1_rtc_clk_step_adj1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_ADJ2_UNION
 结构说明  : RTC_ADJ2 寄存器结构定义。地址偏移量:0x0C2，初值:0x80，宽度:8
 寄存器说明: rtc_adj[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_clk_step_adj2 : 8;  /* bit[0-7]: TCXO模式下，计时精度可以调节，调节范围+/-192ppm（63拍时钟），调节步进3.05ppm（1拍时钟），默认为0。 */
    } reg;
} SOC_SMART_RTC_ADJ2_UNION;
#endif
#define SOC_SMART_RTC_ADJ2_rtc_clk_step_adj2_START  (0)
#define SOC_SMART_RTC_ADJ2_rtc_clk_step_adj2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_XO_THRESOLD1_UNION
 结构说明  : XO_THRESOLD1 寄存器结构定义。地址偏移量:0x0C3，初值:0xB8，宽度:8
 寄存器说明: XO_THRESOLD2[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  xo_cali_thresold1 : 8;  /* bit[0-7]: 校准偏差阈值设置寄存器。 */
    } reg;
} SOC_SMART_XO_THRESOLD1_UNION;
#endif
#define SOC_SMART_XO_THRESOLD1_xo_cali_thresold1_START  (0)
#define SOC_SMART_XO_THRESOLD1_xo_cali_thresold1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_XO_THRESOLD2_UNION
 结构说明  : XO_THRESOLD2 寄存器结构定义。地址偏移量:0x0C4，初值:0x00，宽度:8
 寄存器说明: XO_THRESOLD2[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  xo_cali_thresold2 : 8;  /* bit[0-7]: 校准偏差阈值设置寄存器。 */
    } reg;
} SOC_SMART_XO_THRESOLD2_UNION;
#endif
#define SOC_SMART_XO_THRESOLD2_xo_cali_thresold2_START  (0)
#define SOC_SMART_XO_THRESOLD2_xo_cali_thresold2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED_ANA0_UNION
 结构说明  : NP_RSVED_ANA0 寄存器结构定义。地址偏移量:0x0C5，初值:0x00，宽度:8
 寄存器说明: 非下电模拟保留0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved0 : 8;  /* bit[0-7]: 预留非下电寄存器 */
    } reg;
} SOC_SMART_NP_RSVED_ANA0_UNION;
#endif
#define SOC_SMART_NP_RSVED_ANA0_no_pwr_resved0_START  (0)
#define SOC_SMART_NP_RSVED_ANA0_no_pwr_resved0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED_ANA1_UNION
 结构说明  : NP_RSVED_ANA1 寄存器结构定义。地址偏移量:0x0C6，初值:0x00，宽度:8
 寄存器说明: 非下电模拟保留1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved1 : 8;  /* bit[0-7]: 预留非下电寄存器 */
    } reg;
} SOC_SMART_NP_RSVED_ANA1_UNION;
#endif
#define SOC_SMART_NP_RSVED_ANA1_no_pwr_resved1_START  (0)
#define SOC_SMART_NP_RSVED_ANA1_no_pwr_resved1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED0_UNION
 结构说明  : NP_RSVED0 寄存器结构定义。地址偏移量:0x0C7，初值:0x00，宽度:8
 寄存器说明: 非下电保留0寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved2 : 8;  /* bit[0-7]: 产品预留：测试用及备用，不对PMU内部工作状态产生任何作用，仅用于记录信息。 */
    } reg;
} SOC_SMART_NP_RSVED0_UNION;
#endif
#define SOC_SMART_NP_RSVED0_no_pwr_resved2_START  (0)
#define SOC_SMART_NP_RSVED0_no_pwr_resved2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED1_UNION
 结构说明  : NP_RSVED1 寄存器结构定义。地址偏移量:0x0C8，初值:0x00，宽度:8
 寄存器说明: 非下电保留1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved3 : 8;  /* bit[0-7]: 产品预留：测试用及备用，不对PMU内部工作状态产生任何作用，仅用于记录信息。 */
    } reg;
} SOC_SMART_NP_RSVED1_UNION;
#endif
#define SOC_SMART_NP_RSVED1_no_pwr_resved3_START  (0)
#define SOC_SMART_NP_RSVED1_no_pwr_resved3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED2_UNION
 结构说明  : NP_RSVED2 寄存器结构定义。地址偏移量:0x0C9，初值:0x00，宽度:8
 寄存器说明: 非下电保留2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved4 : 8;  /* bit[0-7]: 产品预留：测试用及备用，不对PMU内部工作状态产生任何作用，仅用于记录信息。 */
    } reg;
} SOC_SMART_NP_RSVED2_UNION;
#endif
#define SOC_SMART_NP_RSVED2_no_pwr_resved4_START  (0)
#define SOC_SMART_NP_RSVED2_no_pwr_resved4_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED3_UNION
 结构说明  : NP_RSVED3 寄存器结构定义。地址偏移量:0x0CA，初值:0x00，宽度:8
 寄存器说明: 非下电保留3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved5 : 8;  /* bit[0-7]: 产品预留：测试用及备用，不对PMU内部工作状态产生任何作用，仅用于记录信息。 */
    } reg;
} SOC_SMART_NP_RSVED3_UNION;
#endif
#define SOC_SMART_NP_RSVED3_no_pwr_resved5_START  (0)
#define SOC_SMART_NP_RSVED3_no_pwr_resved5_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED4_UNION
 结构说明  : NP_RSVED4 寄存器结构定义。地址偏移量:0x0CB，初值:0x00，宽度:8
 寄存器说明: 非下电保留4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved6 : 8;  /* bit[0-7]: 产品预留：测试用及备用，不对PMU内部工作状态产生任何作用，仅用于记录信息。 */
    } reg;
} SOC_SMART_NP_RSVED4_UNION;
#endif
#define SOC_SMART_NP_RSVED4_no_pwr_resved6_START  (0)
#define SOC_SMART_NP_RSVED4_no_pwr_resved6_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED5_UNION
 结构说明  : NP_RSVED5 寄存器结构定义。地址偏移量:0x0CC，初值:0x00，宽度:8
 寄存器说明: 非下电保留5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved7 : 8;  /* bit[0-7]: 产品预留：测试用及备用，不对PMU内部工作状态产生任何作用，仅用于记录信息。 */
    } reg;
} SOC_SMART_NP_RSVED5_UNION;
#endif
#define SOC_SMART_NP_RSVED5_no_pwr_resved7_START  (0)
#define SOC_SMART_NP_RSVED5_no_pwr_resved7_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED6_UNION
 结构说明  : NP_RSVED6 寄存器结构定义。地址偏移量:0x0CD，初值:0x00，宽度:8
 寄存器说明: 非下电保留6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved8 : 8;  /* bit[0-7]: 产品预留：测试用及备用，不对PMU内部工作状态产生任何作用，仅用于记录信息。 */
    } reg;
} SOC_SMART_NP_RSVED6_UNION;
#endif
#define SOC_SMART_NP_RSVED6_no_pwr_resved8_START  (0)
#define SOC_SMART_NP_RSVED6_no_pwr_resved8_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NP_RSVED7_UNION
 结构说明  : NP_RSVED7 寄存器结构定义。地址偏移量:0x0CE，初值:0x00，宽度:8
 寄存器说明: 非下电保留7寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  no_pwr_resved9 : 8;  /* bit[0-7]: 产品预留：测试用及备用，不对PMU内部工作状态产生任何作用，仅用于记录信息。 */
    } reg;
} SOC_SMART_NP_RSVED7_UNION;
#endif
#define SOC_SMART_NP_RSVED7_no_pwr_resved9_START  (0)
#define SOC_SMART_NP_RSVED7_no_pwr_resved9_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_PWRUP_TIMER0_UNION
 结构说明  : RTC_PWRUP_TIMER0 寄存器结构定义。地址偏移量:0x0CF，初值:0x00，宽度:8
 寄存器说明: RTC上电拍照[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_pwrup_timer0 : 8;  /* bit[0-7]: PMU上电时刻RTC计数拍照值[7:0]。 */
    } reg;
} SOC_SMART_RTC_PWRUP_TIMER0_UNION;
#endif
#define SOC_SMART_RTC_PWRUP_TIMER0_rtc_pwrup_timer0_START  (0)
#define SOC_SMART_RTC_PWRUP_TIMER0_rtc_pwrup_timer0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_PWRUP_TIMER1_UNION
 结构说明  : RTC_PWRUP_TIMER1 寄存器结构定义。地址偏移量:0x0D0，初值:0x00，宽度:8
 寄存器说明: RTC上电拍照[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_pwrup_timer1 : 8;  /* bit[0-7]: PMU上电时刻RTC计数拍照值[15:8]。 */
    } reg;
} SOC_SMART_RTC_PWRUP_TIMER1_UNION;
#endif
#define SOC_SMART_RTC_PWRUP_TIMER1_rtc_pwrup_timer1_START  (0)
#define SOC_SMART_RTC_PWRUP_TIMER1_rtc_pwrup_timer1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_PWRUP_TIMER2_UNION
 结构说明  : RTC_PWRUP_TIMER2 寄存器结构定义。地址偏移量:0x0D1，初值:0x00，宽度:8
 寄存器说明: RTC上电拍照[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_pwrup_timer2 : 8;  /* bit[0-7]: PMU上电时刻RTC计数拍照值[23:16]。 */
    } reg;
} SOC_SMART_RTC_PWRUP_TIMER2_UNION;
#endif
#define SOC_SMART_RTC_PWRUP_TIMER2_rtc_pwrup_timer2_START  (0)
#define SOC_SMART_RTC_PWRUP_TIMER2_rtc_pwrup_timer2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_PWRUP_TIMER3_UNION
 结构说明  : RTC_PWRUP_TIMER3 寄存器结构定义。地址偏移量:0x0D2，初值:0x00，宽度:8
 寄存器说明: RTC上电拍照[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_pwrup_timer3 : 8;  /* bit[0-7]: PMU上电时刻RTC计数拍照值[31:24]。 */
    } reg;
} SOC_SMART_RTC_PWRUP_TIMER3_UNION;
#endif
#define SOC_SMART_RTC_PWRUP_TIMER3_rtc_pwrup_timer3_START  (0)
#define SOC_SMART_RTC_PWRUP_TIMER3_rtc_pwrup_timer3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_PWRDOWN_TIMER0_UNION
 结构说明  : RTC_PWRDOWN_TIMER0 寄存器结构定义。地址偏移量:0x0D3，初值:0x00，宽度:8
 寄存器说明: RTC下电拍照[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_pwrdown_timer0 : 8;  /* bit[0-7]: PMU下电时刻RTC计数拍照值[7:0]。 */
    } reg;
} SOC_SMART_RTC_PWRDOWN_TIMER0_UNION;
#endif
#define SOC_SMART_RTC_PWRDOWN_TIMER0_rtc_pwrdown_timer0_START  (0)
#define SOC_SMART_RTC_PWRDOWN_TIMER0_rtc_pwrdown_timer0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_PWRDOWN_TIMER1_UNION
 结构说明  : RTC_PWRDOWN_TIMER1 寄存器结构定义。地址偏移量:0x0D4，初值:0x00，宽度:8
 寄存器说明: RTC下电拍照[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_pwrdown_timer1 : 8;  /* bit[0-7]: PMU下电时刻RTC计数拍照值[15:8]。 */
    } reg;
} SOC_SMART_RTC_PWRDOWN_TIMER1_UNION;
#endif
#define SOC_SMART_RTC_PWRDOWN_TIMER1_rtc_pwrdown_timer1_START  (0)
#define SOC_SMART_RTC_PWRDOWN_TIMER1_rtc_pwrdown_timer1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_PWRDOWN_TIMER2_UNION
 结构说明  : RTC_PWRDOWN_TIMER2 寄存器结构定义。地址偏移量:0x0D5，初值:0x00，宽度:8
 寄存器说明: RTC下电拍照[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_pwrdown_timer2 : 8;  /* bit[0-7]: PMU下电时刻RTC计数拍照值[23:16]。 */
    } reg;
} SOC_SMART_RTC_PWRDOWN_TIMER2_UNION;
#endif
#define SOC_SMART_RTC_PWRDOWN_TIMER2_rtc_pwrdown_timer2_START  (0)
#define SOC_SMART_RTC_PWRDOWN_TIMER2_rtc_pwrdown_timer2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_RTC_PWRDOWN_TIMER3_UNION
 结构说明  : RTC_PWRDOWN_TIMER3 寄存器结构定义。地址偏移量:0x0D6，初值:0x00，宽度:8
 寄存器说明: RTC下电拍照[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rtc_pwrdown_timer3 : 8;  /* bit[0-7]: PMU下电时刻RTC计数拍照值[31:24]。 */
    } reg;
} SOC_SMART_RTC_PWRDOWN_TIMER3_UNION;
#endif
#define SOC_SMART_RTC_PWRDOWN_TIMER3_rtc_pwrdown_timer3_START  (0)
#define SOC_SMART_RTC_PWRDOWN_TIMER3_rtc_pwrdown_timer3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_NOPWR_RC_OFF_EN_UNION
 结构说明  : NOPWR_RC_OFF_EN 寄存器结构定义。地址偏移量:0x0EE，初值:0x00，宽度:8
 寄存器说明: XO_DIG_CLK_EN控制器配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  nopwr_rc_off_en : 1;  /* bit[0]  : RC时钟控制信号，
                                                          1’b1关闭;
                                                          1'b0使能。
                                                          与0xBE[6]共同有效时，才能关闭rc256时钟。 */
        unsigned char  reserved        : 7;  /* bit[1-7]: 保留 */
    } reg;
} SOC_SMART_NOPWR_RC_OFF_EN_UNION;
#endif
#define SOC_SMART_NOPWR_RC_OFF_EN_nopwr_rc_off_en_START  (0)
#define SOC_SMART_NOPWR_RC_OFF_EN_nopwr_rc_off_en_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_XO_DIG_CLK_UNION
 结构说明  : XO_DIG_CLK 寄存器结构定义。地址偏移量:0x0F6，初值:0x1F，宽度:8
 寄存器说明: XO_DIG_CLK控制器配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  xo_dig_clk0 : 1;  /* bit[0]  : clk_buf0选择：
                                                      OTP_BUF=1，xo_dig_clk0配置模拟BUF和数字BUF才有效：0对应模拟BUF,1对应数字BUF。
                                                      OTP_BUF=0，xo_dig_clk0固定模拟BUF */
        unsigned char  xo_dig_clk1 : 1;  /* bit[1]  : clk_buf1选择：
                                                      OTP_BUF=1，xo_dig_clk1配置模拟BUF和数字BUF才有效：0对应模拟BUF,1对应数字BUF。
                                                      OTP_BUF=0，xo_dig_clk1固定模拟BUF */
        unsigned char  xo_dig_clk2 : 1;  /* bit[2]  : clk_buf2选择：
                                                      OTP_BUF=1，xo_dig_clk2配置模拟BUF和数字BUF才有效：0对应模拟BUF,1对应数字BUF。
                                                      OTP_BUF=0，xo_dig_clk2固定模拟BUF */
        unsigned char  xo_dig_clk3 : 1;  /* bit[3]  : clk_buf3选择：
                                                      OTP_BUF=1，xo_dig_clk3配置模拟BUF和数字BUF才有效：0对应模拟BUF,1对应数字BUF。
                                                      OTP_BUF=0，xo_dig_clk3固定模拟BUF */
        unsigned char  xo_dig_clk4 : 1;  /* bit[4]  : clk_buf4选择：
                                                      OTP_BUF=1，xo_dig_clk4配置模拟BUF和数字BUF才有效：0对应模拟BUF,1对应数字BUF。
                                                      OTP_BUF=0，xo_dig_clk4固定模拟BUF */
        unsigned char  reserved    : 3;  /* bit[5-7]: 保留 */
    } reg;
} SOC_SMART_XO_DIG_CLK_UNION;
#endif
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk0_START  (0)
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk0_END    (0)
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk1_START  (1)
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk1_END    (1)
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk2_START  (2)
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk2_END    (2)
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk3_START  (3)
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk3_END    (3)
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk4_START  (4)
#define SOC_SMART_XO_DIG_CLK_xo_dig_clk4_END    (4)


/*****************************************************************************
 结构名    : SOC_SMART_SMPL_OPEN_EN_UNION
 结构说明  : SMPL_OPEN_EN 寄存器结构定义。地址偏移量:0x0F8，初值:0x00，宽度:8
 寄存器说明: SMPL功能开关寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  smpl_open_en  : 1;  /* bit[0]  : SMPL功能开关：
                                                        1'b0:关闭
                                                        1'b1:开启 */
        unsigned char  smpl_time_sel : 2;  /* bit[1-2]: SMPL时间配置：
                                                        2'b00:0.5s
                                                        2'b01:1s
                                                        2'b10:1.5s
                                                        2'b11:2s */
        unsigned char  reserved      : 5;  /* bit[3-7]: 保留 */
    } reg;
} SOC_SMART_SMPL_OPEN_EN_UNION;
#endif
#define SOC_SMART_SMPL_OPEN_EN_smpl_open_en_START   (0)
#define SOC_SMART_SMPL_OPEN_EN_smpl_open_en_END     (0)
#define SOC_SMART_SMPL_OPEN_EN_smpl_time_sel_START  (1)
#define SOC_SMART_SMPL_OPEN_EN_smpl_time_sel_END    (2)


/*****************************************************************************
 结构名    : SOC_SMART_PWR_HOLD_2D_DEB_UNION
 结构说明  : PWR_HOLD_2D_DEB 寄存器结构定义。地址偏移量:0x0FC，初值:0x03，宽度:8
 寄存器说明: PWR_HOLD_2D滤波配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pwr_hold_2d_deb : 2;  /* bit[0-1]: pwr_hold_2d_deb滤波配置：
                                                          2'b00:0ms；
                                                          2'b01:1ms；
                                                          2'b10:2ms；
                                                          2'b11:4ms；默认4ms */
        unsigned char  reserved        : 6;  /* bit[2-7]: 保留 */
    } reg;
} SOC_SMART_PWR_HOLD_2D_DEB_UNION;
#endif
#define SOC_SMART_PWR_HOLD_2D_DEB_pwr_hold_2d_deb_START  (0)
#define SOC_SMART_PWR_HOLD_2D_DEB_pwr_hold_2d_deb_END    (1)


/*****************************************************************************
 结构名    : SOC_SMART_CK19M2_600_586_EN_UNION
 结构说明  : CK19M2_600_586_EN 寄存器结构定义。地址偏移量:0x0FE，初值:0x00，宽度:8
 寄存器说明: 19.2M时钟600与586分频切换使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ck19m_600_586_en : 1;  /* bit[0]  : 1'b1：DCXO模式采用586分频
                                                           1'b0：DCXO模式采样600分频；
                                                           默认600分频 */
        unsigned char  reserved         : 7;  /* bit[1-7]: 保留 */
    } reg;
} SOC_SMART_CK19M2_600_586_EN_UNION;
#endif
#define SOC_SMART_CK19M2_600_586_EN_ck19m_600_586_en_START  (0)
#define SOC_SMART_CK19M2_600_586_EN_ck19m_600_586_en_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_NP_SMPL_IRQ_UNION
 结构说明  : NP_SMPL_IRQ 寄存器结构定义。地址偏移量:0x0FF，初值:0x00，宽度:8
 寄存器说明: SMPL触发重新上电事件配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  np_smpl_irq : 1;  /* bit[0]  : 发生SMPL事件：
                                                      0:没有发生过该事件；
                                                      1:发生该事件。 */
        unsigned char  reserved    : 7;  /* bit[1-7]: 保留 */
    } reg;
} SOC_SMART_NP_SMPL_IRQ_UNION;
#endif
#define SOC_SMART_NP_SMPL_IRQ_np_smpl_irq_START  (0)
#define SOC_SMART_NP_SMPL_IRQ_np_smpl_irq_END    (0)


/*****************************************************************************
 结构名    : SOC_SMART_CLK_TOP6_UNION
 结构说明  : CLK_TOP6 寄存器结构定义。地址偏移量:0x100，初值:0x04，宽度:8
 寄存器说明: CLK_TOP控制6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  trim_c2fix : 4;  /* bit[0-3]: C2电容大小trim */
        unsigned char  reserved   : 4;  /* bit[4-7]: 保留 */
    } reg;
} SOC_SMART_CLK_TOP6_UNION;
#endif
#define SOC_SMART_CLK_TOP6_trim_c2fix_START  (0)
#define SOC_SMART_CLK_TOP6_trim_c2fix_END    (3)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT_0_UNION
 结构说明  : CURRENT_0 寄存器结构定义。地址偏移量:0x14D，初值:0x00，宽度:8
 寄存器说明: 当前电流[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current_0 : 8;  /* bit[0-7]: 当前电流低8位，此值是经过校准后的值。 */
    } reg;
} SOC_SMART_CURRENT_0_UNION;
#endif
#define SOC_SMART_CURRENT_0_current_0_START  (0)
#define SOC_SMART_CURRENT_0_current_0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT_1_UNION
 结构说明  : CURRENT_1 寄存器结构定义。地址偏移量:0x14E，初值:0x00，宽度:8
 寄存器说明: 当前电流[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current_1 : 8;  /* bit[0-7]: 当前电流高8位，此值是经过校准后的值。 */
    } reg;
} SOC_SMART_CURRENT_1_UNION;
#endif
#define SOC_SMART_CURRENT_1_current_1_START  (0)
#define SOC_SMART_CURRENT_1_current_1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT_0_UNION
 结构说明  : V_OUT_0 寄存器结构定义。地址偏移量:0x14F，初值:0xFF，宽度:8
 寄存器说明: 当前电压[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out_0 : 8;  /* bit[0-7]: 当前电压低8位，此值是经过校准后的值。 */
    } reg;
} SOC_SMART_V_OUT_0_UNION;
#endif
#define SOC_SMART_V_OUT_0_v_out_0_START  (0)
#define SOC_SMART_V_OUT_0_v_out_0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT_1_UNION
 结构说明  : V_OUT_1 寄存器结构定义。地址偏移量:0x150，初值:0xFF，宽度:8
 寄存器说明: 当前电压[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out_1 : 8;  /* bit[0-7]: 当前电压高8位，此值是经过校准后的值。 */
    } reg;
} SOC_SMART_V_OUT_1_UNION;
#endif
#define SOC_SMART_V_OUT_1_v_out_1_START  (0)
#define SOC_SMART_V_OUT_1_v_out_1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_CTRL_REG_UNION
 结构说明  : CLJ_CTRL_REG 寄存器结构定义。地址偏移量:0x151，初值:0x80，宽度:8
 寄存器说明: 库仑计控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_ctrl            : 3;  /* bit[0-2]: bit[2]：是否强制进入ECO模式控制。
                                                              0：不强制进入ECO模式；
                                                              1：强制进入ECO模式。
                                                              
                                                              bit[1]：库仑计是否跟随PMU进入ECO模式控制。
                                                              0：库仑计不跟随PMU进入ECO模式；
                                                              1：库仑计跟随PMU进入ECO模式。
                                                              仅发生在bit[2] == 0 时有效。
                                                              
                                                              bit[0]：进入ECO模式时，使能信号是否去抖控制。
                                                              0：进入ECO模式使能信号不去抖；
                                                              1：进入ECO模式使能信号去抖。
                                                              仅发生在bit[1] ==1 时有效。 */
        unsigned char  reflash_value_ctrl  : 1;  /* bit[3]  : 库仑计更新数据方式选择：
                                                              0：所有状态都更新数据；
                                                              1：仅ECO模式更新数据。 */
        unsigned char  eco_filter_time     : 2;  /* bit[4-5]: 库仑计进入ECO模式使能信号去抖时间选择：
                                                              00：20ms；
                                                              01：25ms；
                                                              10：30ms；
                                                              11：35ms。
                                                              用于配置en_eco_mode去抖时长，默认为00。 */
        unsigned char  calibration_ctrl    : 1;  /* bit[6]  : 库仑计强制校准控制位。
                                                              0：不校准；
                                                              1：强制进入校准状态。 */
        unsigned char  coul_ctrl_onoff_reg : 1;  /* bit[7]  : 库仑计开关控制寄存器。
                                                              0：库仑计关闭；
                                                              1：库仑计开启。
                                                              注：该寄存器不能配置为库仑计关闭。 */
    } reg;
} SOC_SMART_CLJ_CTRL_REG_UNION;
#endif
#define SOC_SMART_CLJ_CTRL_REG_eco_ctrl_START             (0)
#define SOC_SMART_CLJ_CTRL_REG_eco_ctrl_END               (2)
#define SOC_SMART_CLJ_CTRL_REG_reflash_value_ctrl_START   (3)
#define SOC_SMART_CLJ_CTRL_REG_reflash_value_ctrl_END     (3)
#define SOC_SMART_CLJ_CTRL_REG_eco_filter_time_START      (4)
#define SOC_SMART_CLJ_CTRL_REG_eco_filter_time_END        (5)
#define SOC_SMART_CLJ_CTRL_REG_calibration_ctrl_START     (6)
#define SOC_SMART_CLJ_CTRL_REG_calibration_ctrl_END       (6)
#define SOC_SMART_CLJ_CTRL_REG_coul_ctrl_onoff_reg_START  (7)
#define SOC_SMART_CLJ_CTRL_REG_coul_ctrl_onoff_reg_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_REFALSH_TIME_UNION
 结构说明  : ECO_REFALSH_TIME 寄存器结构定义。地址偏移量:0x152，初值:0x01，宽度:8
 寄存器说明: ECO_REFLASH工作时间寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_reflash_time : 8;  /* bit[0-7]: 此工作时间以一个单位代表110ms，为ECO_REFLASH时间；有效配置值为1~255，配置值大于255时，硬件直接饱和到255。默认为1。 */
    } reg;
} SOC_SMART_ECO_REFALSH_TIME_UNION;
#endif
#define SOC_SMART_ECO_REFALSH_TIME_eco_reflash_time_START  (0)
#define SOC_SMART_ECO_REFALSH_TIME_eco_reflash_time_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_OUT0_UNION
 结构说明  : CL_OUT0 寄存器结构定义。地址偏移量:0x153，初值:0x00，宽度:8
 寄存器说明: cl_out[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_out0 : 8;  /* bit[0-7]: 流出电量计数器[7:0]。 */
    } reg;
} SOC_SMART_CL_OUT0_UNION;
#endif
#define SOC_SMART_CL_OUT0_cl_out0_START  (0)
#define SOC_SMART_CL_OUT0_cl_out0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_OUT1_UNION
 结构说明  : CL_OUT1 寄存器结构定义。地址偏移量:0x154，初值:0x00，宽度:8
 寄存器说明: cl_out[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_out1 : 8;  /* bit[0-7]: 流出电量计数器[15:8]。 */
    } reg;
} SOC_SMART_CL_OUT1_UNION;
#endif
#define SOC_SMART_CL_OUT1_cl_out1_START  (0)
#define SOC_SMART_CL_OUT1_cl_out1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_OUT2_UNION
 结构说明  : CL_OUT2 寄存器结构定义。地址偏移量:0x155，初值:0x00，宽度:8
 寄存器说明: cl_out[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_out2 : 8;  /* bit[0-7]: 流出电量计数器[23:16]。 */
    } reg;
} SOC_SMART_CL_OUT2_UNION;
#endif
#define SOC_SMART_CL_OUT2_cl_out2_START  (0)
#define SOC_SMART_CL_OUT2_cl_out2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_OUT3_UNION
 结构说明  : CL_OUT3 寄存器结构定义。地址偏移量:0x156，初值:0x00，宽度:8
 寄存器说明: cl_out[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_out3 : 8;  /* bit[0-7]: 流出电量计数器[31:24]。 */
    } reg;
} SOC_SMART_CL_OUT3_UNION;
#endif
#define SOC_SMART_CL_OUT3_cl_out3_START  (0)
#define SOC_SMART_CL_OUT3_cl_out3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_IN0_UNION
 结构说明  : CL_IN0 寄存器结构定义。地址偏移量:0x157，初值:0x00，宽度:8
 寄存器说明: cl_in[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_in0 : 8;  /* bit[0-7]: 流入电量计数器[7:0]。 */
    } reg;
} SOC_SMART_CL_IN0_UNION;
#endif
#define SOC_SMART_CL_IN0_cl_in0_START  (0)
#define SOC_SMART_CL_IN0_cl_in0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_IN1_UNION
 结构说明  : CL_IN1 寄存器结构定义。地址偏移量:0x158，初值:0x00，宽度:8
 寄存器说明: cl_in[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_in1 : 8;  /* bit[0-7]: 流入电量计数器[15:8]。 */
    } reg;
} SOC_SMART_CL_IN1_UNION;
#endif
#define SOC_SMART_CL_IN1_cl_in1_START  (0)
#define SOC_SMART_CL_IN1_cl_in1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_IN2_UNION
 结构说明  : CL_IN2 寄存器结构定义。地址偏移量:0x159，初值:0x00，宽度:8
 寄存器说明: cl_in[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_in2 : 8;  /* bit[0-7]: 流入电量计数器[23:16]。 */
    } reg;
} SOC_SMART_CL_IN2_UNION;
#endif
#define SOC_SMART_CL_IN2_cl_in2_START  (0)
#define SOC_SMART_CL_IN2_cl_in2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_IN3_UNION
 结构说明  : CL_IN3 寄存器结构定义。地址偏移量:0x15A，初值:0x00，宽度:8
 寄存器说明: cl_in[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_in3 : 8;  /* bit[0-7]: 流入电量计数器[31:24]。 */
    } reg;
} SOC_SMART_CL_IN3_UNION;
#endif
#define SOC_SMART_CL_IN3_cl_in3_START  (0)
#define SOC_SMART_CL_IN3_cl_in3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CHG_TIMER0_UNION
 结构说明  : CHG_TIMER0 寄存器结构定义。地址偏移量:0x15B，初值:0x00，宽度:8
 寄存器说明: chg_timer[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chg_timer0 : 8;  /* bit[0-7]: 充电时间计数器[7:0]。 */
    } reg;
} SOC_SMART_CHG_TIMER0_UNION;
#endif
#define SOC_SMART_CHG_TIMER0_chg_timer0_START  (0)
#define SOC_SMART_CHG_TIMER0_chg_timer0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CHG_TIMER1_UNION
 结构说明  : CHG_TIMER1 寄存器结构定义。地址偏移量:0x15C，初值:0x00，宽度:8
 寄存器说明: chg_timer[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chg_timer1 : 8;  /* bit[0-7]: 充电时间计数器[15:8]。 */
    } reg;
} SOC_SMART_CHG_TIMER1_UNION;
#endif
#define SOC_SMART_CHG_TIMER1_chg_timer1_START  (0)
#define SOC_SMART_CHG_TIMER1_chg_timer1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CHG_TIMER2_UNION
 结构说明  : CHG_TIMER2 寄存器结构定义。地址偏移量:0x15D，初值:0x00，宽度:8
 寄存器说明: chg_timer[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chg_timer2 : 8;  /* bit[0-7]: 充电时间计数器[23:16]。 */
    } reg;
} SOC_SMART_CHG_TIMER2_UNION;
#endif
#define SOC_SMART_CHG_TIMER2_chg_timer2_START  (0)
#define SOC_SMART_CHG_TIMER2_chg_timer2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CHG_TIMER3_UNION
 结构说明  : CHG_TIMER3 寄存器结构定义。地址偏移量:0x15E，初值:0x00，宽度:8
 寄存器说明: chg_timer[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chg_timer3 : 8;  /* bit[0-7]: 充电时间计数器[31:24]。 */
    } reg;
} SOC_SMART_CHG_TIMER3_UNION;
#endif
#define SOC_SMART_CHG_TIMER3_chg_timer3_START  (0)
#define SOC_SMART_CHG_TIMER3_chg_timer3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_LOAD_TIMER0_UNION
 结构说明  : LOAD_TIMER0 寄存器结构定义。地址偏移量:0x15F，初值:0x00，宽度:8
 寄存器说明: load_timer[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  load_timer0 : 8;  /* bit[0-7]: 放电时间计数器[7:0]。 */
    } reg;
} SOC_SMART_LOAD_TIMER0_UNION;
#endif
#define SOC_SMART_LOAD_TIMER0_load_timer0_START  (0)
#define SOC_SMART_LOAD_TIMER0_load_timer0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_LOAD_TIMER1_UNION
 结构说明  : LOAD_TIMER1 寄存器结构定义。地址偏移量:0x160，初值:0x00，宽度:8
 寄存器说明: load_timer[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  load_timer1 : 8;  /* bit[0-7]: 放电时间计数器[15:8]。 */
    } reg;
} SOC_SMART_LOAD_TIMER1_UNION;
#endif
#define SOC_SMART_LOAD_TIMER1_load_timer1_START  (0)
#define SOC_SMART_LOAD_TIMER1_load_timer1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_LOAD_TIMER2_UNION
 结构说明  : LOAD_TIMER2 寄存器结构定义。地址偏移量:0x161，初值:0x00，宽度:8
 寄存器说明: load_timer[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  load_timer2 : 8;  /* bit[0-7]: 放电时间计数器[23:16]。 */
    } reg;
} SOC_SMART_LOAD_TIMER2_UNION;
#endif
#define SOC_SMART_LOAD_TIMER2_load_timer2_START  (0)
#define SOC_SMART_LOAD_TIMER2_load_timer2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_LOAD_TIMER3_UNION
 结构说明  : LOAD_TIMER3 寄存器结构定义。地址偏移量:0x162，初值:0x00，宽度:8
 寄存器说明: load_timer[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  load_timer3 : 8;  /* bit[0-7]: 放电时间计数器[31:24]。 */
    } reg;
} SOC_SMART_LOAD_TIMER3_UNION;
#endif
#define SOC_SMART_LOAD_TIMER3_load_timer3_START  (0)
#define SOC_SMART_LOAD_TIMER3_load_timer3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_INT0_UNION
 结构说明  : CL_INT0 寄存器结构定义。地址偏移量:0x163，初值:0x00，宽度:8
 寄存器说明: cl_int[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_int0 : 8;  /* bit[0-7]: 流出电量中断比较寄存器[7:0]：
                                                  当cl_out>cl_int时上报中断。 */
    } reg;
} SOC_SMART_CL_INT0_UNION;
#endif
#define SOC_SMART_CL_INT0_cl_int0_START  (0)
#define SOC_SMART_CL_INT0_cl_int0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_INT1_UNION
 结构说明  : CL_INT1 寄存器结构定义。地址偏移量:0x164，初值:0x00，宽度:8
 寄存器说明: cl_int[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_int1 : 8;  /* bit[0-7]: 流出电量中断比较寄存器[15:8]：
                                                  当cl_out>cl_int时上报中断。 */
    } reg;
} SOC_SMART_CL_INT1_UNION;
#endif
#define SOC_SMART_CL_INT1_cl_int1_START  (0)
#define SOC_SMART_CL_INT1_cl_int1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_INT2_UNION
 结构说明  : CL_INT2 寄存器结构定义。地址偏移量:0x165，初值:0x00，宽度:8
 寄存器说明: cl_int[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_int2 : 8;  /* bit[0-7]: 流出电量中断比较寄存器[23:16]：
                                                  当cl_out>cl_int时上报中断。 */
    } reg;
} SOC_SMART_CL_INT2_UNION;
#endif
#define SOC_SMART_CL_INT2_cl_int2_START  (0)
#define SOC_SMART_CL_INT2_cl_int2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CL_INT3_UNION
 结构说明  : CL_INT3 寄存器结构定义。地址偏移量:0x166，初值:0x80，宽度:8
 寄存器说明: cl_int[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cl_int3 : 8;  /* bit[0-7]: 流出电量中断比较寄存器[31:24]：
                                                  当cl_out>cl_int时上报中断。 */
    } reg;
} SOC_SMART_CL_INT3_UNION;
#endif
#define SOC_SMART_CL_INT3_cl_int3_START  (0)
#define SOC_SMART_CL_INT3_cl_int3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_INT0_UNION
 结构说明  : V_INT0 寄存器结构定义。地址偏移量:0x167，初值:0x00，宽度:8
 寄存器说明: v_int[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_int0 : 8;  /* bit[0-7]: 电压中断比较值寄存器[7:0]。 */
    } reg;
} SOC_SMART_V_INT0_UNION;
#endif
#define SOC_SMART_V_INT0_v_int0_START  (0)
#define SOC_SMART_V_INT0_v_int0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_INT1_UNION
 结构说明  : V_INT1 寄存器结构定义。地址偏移量:0x168，初值:0x00，宽度:8
 寄存器说明: v_int[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_int1 : 8;  /* bit[0-7]: 电压中断比较值寄存器[15:8]。 */
    } reg;
} SOC_SMART_V_INT1_UNION;
#endif
#define SOC_SMART_V_INT1_v_int1_START  (0)
#define SOC_SMART_V_INT1_v_int1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OFFSET_CURRENT0_UNION
 结构说明  : OFFSET_CURRENT0 寄存器结构定义。地址偏移量:0x169，初值:0x00，宽度:8
 寄存器说明: offset_current[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  offset_current0 : 8;  /* bit[0-7]: 电流校准值[7:0]。 */
    } reg;
} SOC_SMART_OFFSET_CURRENT0_UNION;
#endif
#define SOC_SMART_OFFSET_CURRENT0_offset_current0_START  (0)
#define SOC_SMART_OFFSET_CURRENT0_offset_current0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OFFSET_CURRENT1_UNION
 结构说明  : OFFSET_CURRENT1 寄存器结构定义。地址偏移量:0x16A，初值:0x00，宽度:8
 寄存器说明: offset_current[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  offset_current1 : 8;  /* bit[0-7]: 电流校准值[15:8]。 */
    } reg;
} SOC_SMART_OFFSET_CURRENT1_UNION;
#endif
#define SOC_SMART_OFFSET_CURRENT1_offset_current1_START  (0)
#define SOC_SMART_OFFSET_CURRENT1_offset_current1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OFFSET_VOLTAGE0_UNION
 结构说明  : OFFSET_VOLTAGE0 寄存器结构定义。地址偏移量:0x16B，初值:0x00，宽度:8
 寄存器说明: offset_voltage[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  offset_voltage0 : 8;  /* bit[0-7]: 电压校准值[7:0]。 */
    } reg;
} SOC_SMART_OFFSET_VOLTAGE0_UNION;
#endif
#define SOC_SMART_OFFSET_VOLTAGE0_offset_voltage0_START  (0)
#define SOC_SMART_OFFSET_VOLTAGE0_offset_voltage0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OFFSET_VOLTAGE1_UNION
 结构说明  : OFFSET_VOLTAGE1 寄存器结构定义。地址偏移量:0x16C，初值:0x00，宽度:8
 寄存器说明: offset_voltage[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  offset_voltage1 : 8;  /* bit[0-7]: 电压校准值[15:8]。 */
    } reg;
} SOC_SMART_OFFSET_VOLTAGE1_UNION;
#endif
#define SOC_SMART_OFFSET_VOLTAGE1_offset_voltage1_START  (0)
#define SOC_SMART_OFFSET_VOLTAGE1_offset_voltage1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCV_VOLTAGE0_UNION
 结构说明  : OCV_VOLTAGE0 寄存器结构定义。地址偏移量:0x16D，初值:0x00，宽度:8
 寄存器说明: v_ocv_data[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_ocv_data0 : 8;  /* bit[0-7]: 开机时OCV电压采样值[7:0]。此值是原始值，未经过校准。 */
    } reg;
} SOC_SMART_OCV_VOLTAGE0_UNION;
#endif
#define SOC_SMART_OCV_VOLTAGE0_v_ocv_data0_START  (0)
#define SOC_SMART_OCV_VOLTAGE0_v_ocv_data0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCV_VOLTAGE1_UNION
 结构说明  : OCV_VOLTAGE1 寄存器结构定义。地址偏移量:0x16E，初值:0x00，宽度:8
 寄存器说明: v_ocv_data[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_ocv_data1 : 8;  /* bit[0-7]: 开机时OCV电压采样值[15:8]。此值是原始值，未经过校准。 */
    } reg;
} SOC_SMART_OCV_VOLTAGE1_UNION;
#endif
#define SOC_SMART_OCV_VOLTAGE1_v_ocv_data1_START  (0)
#define SOC_SMART_OCV_VOLTAGE1_v_ocv_data1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCV_CURRENT0_UNION
 结构说明  : OCV_CURRENT0 寄存器结构定义。地址偏移量:0x16F，初值:0x00，宽度:8
 寄存器说明: i_ocv_data[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  i_ocv_data0 : 8;  /* bit[0-7]: 开机时OCV电流采样值[7:0]。此值是原始值，未经过校准。 */
    } reg;
} SOC_SMART_OCV_CURRENT0_UNION;
#endif
#define SOC_SMART_OCV_CURRENT0_i_ocv_data0_START  (0)
#define SOC_SMART_OCV_CURRENT0_i_ocv_data0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OCV_CURRENT1_UNION
 结构说明  : OCV_CURRENT1 寄存器结构定义。地址偏移量:0x170，初值:0x00，宽度:8
 寄存器说明: i_ocv_data[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  i_ocv_data1 : 8;  /* bit[0-7]: 开机时OCV电流采样值[15:8]。此值是原始值，未经过校准。 */
    } reg;
} SOC_SMART_OCV_CURRENT1_UNION;
#endif
#define SOC_SMART_OCV_CURRENT1_i_ocv_data1_START  (0)
#define SOC_SMART_OCV_CURRENT1_i_ocv_data1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_OUT_CLIN_0_UNION
 结构说明  : ECO_OUT_CLIN_0 寄存器结构定义。地址偏移量:0x171，初值:0x00，宽度:8
 寄存器说明: eco_out_clin[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_out_clin0 : 8;  /* bit[0-7]: 退出ECO模式流入电量计数器[7:0]。每次退出ECO模式自动记录当前流入电量，其余时刻保持。 */
    } reg;
} SOC_SMART_ECO_OUT_CLIN_0_UNION;
#endif
#define SOC_SMART_ECO_OUT_CLIN_0_eco_out_clin0_START  (0)
#define SOC_SMART_ECO_OUT_CLIN_0_eco_out_clin0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_OUT_CLIN_1_UNION
 结构说明  : ECO_OUT_CLIN_1 寄存器结构定义。地址偏移量:0x172，初值:0x00，宽度:8
 寄存器说明: eco_out_clin[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_out_clin1 : 8;  /* bit[0-7]: 退出ECO模式流入电量计数器[15:8]。每次退出ECO模式自动记录当前流入电量，其余时刻保持。 */
    } reg;
} SOC_SMART_ECO_OUT_CLIN_1_UNION;
#endif
#define SOC_SMART_ECO_OUT_CLIN_1_eco_out_clin1_START  (0)
#define SOC_SMART_ECO_OUT_CLIN_1_eco_out_clin1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_OUT_CLIN_2_UNION
 结构说明  : ECO_OUT_CLIN_2 寄存器结构定义。地址偏移量:0x173，初值:0x00，宽度:8
 寄存器说明: eco_out_clin[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_out_clin2 : 8;  /* bit[0-7]: 退出ECO模式流入电量计数器[23:16]。每次退出ECO模式自动记录当前流入电量，其余时刻保持。 */
    } reg;
} SOC_SMART_ECO_OUT_CLIN_2_UNION;
#endif
#define SOC_SMART_ECO_OUT_CLIN_2_eco_out_clin2_START  (0)
#define SOC_SMART_ECO_OUT_CLIN_2_eco_out_clin2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_OUT_CLIN_3_UNION
 结构说明  : ECO_OUT_CLIN_3 寄存器结构定义。地址偏移量:0x174，初值:0x00，宽度:8
 寄存器说明: eco_out_clin[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_out_clin3 : 8;  /* bit[0-7]: 退出ECO模式流入电量计数器[31:24]。每次退出ECO模式自动记录当前流入电量，其余时刻保持。 */
    } reg;
} SOC_SMART_ECO_OUT_CLIN_3_UNION;
#endif
#define SOC_SMART_ECO_OUT_CLIN_3_eco_out_clin3_START  (0)
#define SOC_SMART_ECO_OUT_CLIN_3_eco_out_clin3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_OUT_CLOUT_0_UNION
 结构说明  : ECO_OUT_CLOUT_0 寄存器结构定义。地址偏移量:0x175，初值:0x00，宽度:8
 寄存器说明: eco_out_clout[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_out_clout0 : 8;  /* bit[0-7]: 退出ECO模式流出电量计数器[7:0]。每次退出ECO模式自动记录当前流出电量，其余时刻保持。 */
    } reg;
} SOC_SMART_ECO_OUT_CLOUT_0_UNION;
#endif
#define SOC_SMART_ECO_OUT_CLOUT_0_eco_out_clout0_START  (0)
#define SOC_SMART_ECO_OUT_CLOUT_0_eco_out_clout0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_OUT_CLOUT_1_UNION
 结构说明  : ECO_OUT_CLOUT_1 寄存器结构定义。地址偏移量:0x176，初值:0x00，宽度:8
 寄存器说明: eco_out_clout[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_out_clout1 : 8;  /* bit[0-7]: 退出ECO模式流出电量计数器[15:8]。每次退出ECO模式自动记录当前流出电量，其余时刻保持。 */
    } reg;
} SOC_SMART_ECO_OUT_CLOUT_1_UNION;
#endif
#define SOC_SMART_ECO_OUT_CLOUT_1_eco_out_clout1_START  (0)
#define SOC_SMART_ECO_OUT_CLOUT_1_eco_out_clout1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_OUT_CLOUT_2_UNION
 结构说明  : ECO_OUT_CLOUT_2 寄存器结构定义。地址偏移量:0x177，初值:0x00，宽度:8
 寄存器说明: eco_out_clout[23:16]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_out_clout2 : 8;  /* bit[0-7]: 退出ECO模式流出电量计数器[23:16]。每次退出ECO模式自动记录当前流出电量，其余时刻保持。 */
    } reg;
} SOC_SMART_ECO_OUT_CLOUT_2_UNION;
#endif
#define SOC_SMART_ECO_OUT_CLOUT_2_eco_out_clout2_START  (0)
#define SOC_SMART_ECO_OUT_CLOUT_2_eco_out_clout2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_ECO_OUT_CLOUT_3_UNION
 结构说明  : ECO_OUT_CLOUT_3 寄存器结构定义。地址偏移量:0x178，初值:0x00，宽度:8
 寄存器说明: eco_out_clout[31:24]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  eco_out_clout3 : 8;  /* bit[0-7]: 退出ECO模式流出电量计数器[31:24]。每次退出ECO模式自动记录当前流出电量，其余时刻保持。 */
    } reg;
} SOC_SMART_ECO_OUT_CLOUT_3_UNION;
#endif
#define SOC_SMART_ECO_OUT_CLOUT_3_eco_out_clout3_START  (0)
#define SOC_SMART_ECO_OUT_CLOUT_3_eco_out_clout3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE0_UNION
 结构说明  : V_OUT0_PRE0 寄存器结构定义。地址偏移量:0x179，初值:0xFF，宽度:8
 寄存器说明: v_out_pre0[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre0 : 8;  /* bit[0-7]: 当前电池电压指示[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE0_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE0_v_out0_pre0_START  (0)
#define SOC_SMART_V_OUT0_PRE0_v_out0_pre0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE0_UNION
 结构说明  : V_OUT1_PRE0 寄存器结构定义。地址偏移量:0x17A，初值:0xFF，宽度:8
 寄存器说明: v_out_pre0[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre0 : 8;  /* bit[0-7]: 当前电池电压指示[15:8]，最高位代表符号位。 */
    } reg;
} SOC_SMART_V_OUT1_PRE0_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE0_v_out1_pre0_START  (0)
#define SOC_SMART_V_OUT1_PRE0_v_out1_pre0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE1_UNION
 结构说明  : V_OUT0_PRE1 寄存器结构定义。地址偏移量:0x17B，初值:0xFF，宽度:8
 寄存器说明: v_out_pre1[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre1 : 8;  /* bit[0-7]: 当前电压前一次值[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE1_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE1_v_out0_pre1_START  (0)
#define SOC_SMART_V_OUT0_PRE1_v_out0_pre1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE1_UNION
 结构说明  : V_OUT1_PRE1 寄存器结构定义。地址偏移量:0x17C，初值:0xFF，宽度:8
 寄存器说明: v_out_pre1[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre1 : 8;  /* bit[0-7]: 当前电压前一次值[15:8]。 */
    } reg;
} SOC_SMART_V_OUT1_PRE1_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE1_v_out1_pre1_START  (0)
#define SOC_SMART_V_OUT1_PRE1_v_out1_pre1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE2_UNION
 结构说明  : V_OUT0_PRE2 寄存器结构定义。地址偏移量:0x17D，初值:0xFF，宽度:8
 寄存器说明: v_out_pre2[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre2 : 8;  /* bit[0-7]: 当前电压前二次值[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE2_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE2_v_out0_pre2_START  (0)
#define SOC_SMART_V_OUT0_PRE2_v_out0_pre2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE2_UNION
 结构说明  : V_OUT1_PRE2 寄存器结构定义。地址偏移量:0x17E，初值:0xFF，宽度:8
 寄存器说明: v_out_pre2[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre2 : 8;  /* bit[0-7]: 当前电压前二次值[15:8]。 */
    } reg;
} SOC_SMART_V_OUT1_PRE2_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE2_v_out1_pre2_START  (0)
#define SOC_SMART_V_OUT1_PRE2_v_out1_pre2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE3_UNION
 结构说明  : V_OUT0_PRE3 寄存器结构定义。地址偏移量:0x17F，初值:0xFF，宽度:8
 寄存器说明: v_out_pre3[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre3 : 8;  /* bit[0-7]: 当前电压前三次值[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE3_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE3_v_out0_pre3_START  (0)
#define SOC_SMART_V_OUT0_PRE3_v_out0_pre3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE3_UNION
 结构说明  : V_OUT1_PRE3 寄存器结构定义。地址偏移量:0x180，初值:0xFF，宽度:8
 寄存器说明: v_out_pre3[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre3 : 8;  /* bit[0-7]: 当前电压前三次值[15:8]。 */
    } reg;
} SOC_SMART_V_OUT1_PRE3_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE3_v_out1_pre3_START  (0)
#define SOC_SMART_V_OUT1_PRE3_v_out1_pre3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE4_UNION
 结构说明  : V_OUT0_PRE4 寄存器结构定义。地址偏移量:0x181，初值:0xFF，宽度:8
 寄存器说明: v_out_pre4[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre4 : 8;  /* bit[0-7]: 当前电压前四次值[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE4_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE4_v_out0_pre4_START  (0)
#define SOC_SMART_V_OUT0_PRE4_v_out0_pre4_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE4_UNION
 结构说明  : V_OUT1_PRE4 寄存器结构定义。地址偏移量:0x182，初值:0xFF，宽度:8
 寄存器说明: v_out_pre4[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre4 : 8;  /* bit[0-7]: 当前电压前四次值[15:8]。 */
    } reg;
} SOC_SMART_V_OUT1_PRE4_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE4_v_out1_pre4_START  (0)
#define SOC_SMART_V_OUT1_PRE4_v_out1_pre4_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE5_UNION
 结构说明  : V_OUT0_PRE5 寄存器结构定义。地址偏移量:0x183，初值:0xFF，宽度:8
 寄存器说明: v_out_pre5[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre5 : 8;  /* bit[0-7]: 当前电压前五次值[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE5_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE5_v_out0_pre5_START  (0)
#define SOC_SMART_V_OUT0_PRE5_v_out0_pre5_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE5_UNION
 结构说明  : V_OUT1_PRE5 寄存器结构定义。地址偏移量:0x184，初值:0xFF，宽度:8
 寄存器说明: v_out_pre5[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre5 : 8;  /* bit[0-7]: 当前电压前五次值[15:8]。 */
    } reg;
} SOC_SMART_V_OUT1_PRE5_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE5_v_out1_pre5_START  (0)
#define SOC_SMART_V_OUT1_PRE5_v_out1_pre5_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE6_UNION
 结构说明  : V_OUT0_PRE6 寄存器结构定义。地址偏移量:0x185，初值:0xFF，宽度:8
 寄存器说明: v_out_pre6[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre6 : 8;  /* bit[0-7]: 当前电压前六次值[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE6_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE6_v_out0_pre6_START  (0)
#define SOC_SMART_V_OUT0_PRE6_v_out0_pre6_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE6_UNION
 结构说明  : V_OUT1_PRE6 寄存器结构定义。地址偏移量:0x186，初值:0xFF，宽度:8
 寄存器说明: v_out_pre6[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre6 : 8;  /* bit[0-7]: 当前电压前六次值[15:8]。 */
    } reg;
} SOC_SMART_V_OUT1_PRE6_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE6_v_out1_pre6_START  (0)
#define SOC_SMART_V_OUT1_PRE6_v_out1_pre6_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE7_UNION
 结构说明  : V_OUT0_PRE7 寄存器结构定义。地址偏移量:0x187，初值:0xFF，宽度:8
 寄存器说明: v_out_pre7[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre7 : 8;  /* bit[0-7]: 当前电压前七次值[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE7_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE7_v_out0_pre7_START  (0)
#define SOC_SMART_V_OUT0_PRE7_v_out0_pre7_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE7_UNION
 结构说明  : V_OUT1_PRE7 寄存器结构定义。地址偏移量:0x188，初值:0xFF，宽度:8
 寄存器说明: v_out_pre7[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre7 : 8;  /* bit[0-7]: 当前电压前七次值[15:8]。 */
    } reg;
} SOC_SMART_V_OUT1_PRE7_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE7_v_out1_pre7_START  (0)
#define SOC_SMART_V_OUT1_PRE7_v_out1_pre7_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE8_UNION
 结构说明  : V_OUT0_PRE8 寄存器结构定义。地址偏移量:0x189，初值:0xFF，宽度:8
 寄存器说明: v_out_pre8[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre8 : 8;  /* bit[0-7]: 当前电压前八次值[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE8_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE8_v_out0_pre8_START  (0)
#define SOC_SMART_V_OUT0_PRE8_v_out0_pre8_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE8_UNION
 结构说明  : V_OUT1_PRE8 寄存器结构定义。地址偏移量:0x18A，初值:0xFF，宽度:8
 寄存器说明: v_out_pre8[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre8 : 8;  /* bit[0-7]: 当前电压前八次值[15:8]。 */
    } reg;
} SOC_SMART_V_OUT1_PRE8_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE8_v_out1_pre8_START  (0)
#define SOC_SMART_V_OUT1_PRE8_v_out1_pre8_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT0_PRE9_UNION
 结构说明  : V_OUT0_PRE9 寄存器结构定义。地址偏移量:0x18B，初值:0xFF，宽度:8
 寄存器说明: v_out_pre9[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out0_pre9 : 8;  /* bit[0-7]: 当前电压前九次值[7:0]。 */
    } reg;
} SOC_SMART_V_OUT0_PRE9_UNION;
#endif
#define SOC_SMART_V_OUT0_PRE9_v_out0_pre9_START  (0)
#define SOC_SMART_V_OUT0_PRE9_v_out0_pre9_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_V_OUT1_PRE9_UNION
 结构说明  : V_OUT1_PRE9 寄存器结构定义。地址偏移量:0x18C，初值:0xFF，宽度:8
 寄存器说明: v_out_pre9[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_out1_pre9 : 8;  /* bit[0-7]: 当前电压前九次值[15:8]。 */
    } reg;
} SOC_SMART_V_OUT1_PRE9_UNION;
#endif
#define SOC_SMART_V_OUT1_PRE9_v_out1_pre9_START  (0)
#define SOC_SMART_V_OUT1_PRE9_v_out1_pre9_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE0_UNION
 结构说明  : CURRENT0_PRE0 寄存器结构定义。地址偏移量:0x18D，初值:0x00，宽度:8
 寄存器说明: current_pre0[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre0 : 8;  /* bit[0-7]: 当前电流指示[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE0_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE0_current0_pre0_START  (0)
#define SOC_SMART_CURRENT0_PRE0_current0_pre0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE0_UNION
 结构说明  : CURRENT1_PRE0 寄存器结构定义。地址偏移量:0x18E，初值:0x00，宽度:8
 寄存器说明: current_pre0[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre0 : 8;  /* bit[0-7]: 当前电流指示[15:8]，最高位代表符号位。 */
    } reg;
} SOC_SMART_CURRENT1_PRE0_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE0_current1_pre0_START  (0)
#define SOC_SMART_CURRENT1_PRE0_current1_pre0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE1_UNION
 结构说明  : CURRENT0_PRE1 寄存器结构定义。地址偏移量:0x18F，初值:0x00，宽度:8
 寄存器说明: current_pre1[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre1 : 8;  /* bit[0-7]: 当前电流前一次值[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE1_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE1_current0_pre1_START  (0)
#define SOC_SMART_CURRENT0_PRE1_current0_pre1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE1_UNION
 结构说明  : CURRENT1_PRE1 寄存器结构定义。地址偏移量:0x190，初值:0x00，宽度:8
 寄存器说明: current_pre1[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre1 : 8;  /* bit[0-7]: 当前电流前一次值[15:8]。 */
    } reg;
} SOC_SMART_CURRENT1_PRE1_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE1_current1_pre1_START  (0)
#define SOC_SMART_CURRENT1_PRE1_current1_pre1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE2_UNION
 结构说明  : CURRENT0_PRE2 寄存器结构定义。地址偏移量:0x191，初值:0x00，宽度:8
 寄存器说明: current_pre2[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre2 : 8;  /* bit[0-7]: 当前电流前二次值[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE2_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE2_current0_pre2_START  (0)
#define SOC_SMART_CURRENT0_PRE2_current0_pre2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE2_UNION
 结构说明  : CURRENT1_PRE2 寄存器结构定义。地址偏移量:0x192，初值:0x00，宽度:8
 寄存器说明: current_pre2[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre2 : 8;  /* bit[0-7]: 当前电流前二次值[15:8]。 */
    } reg;
} SOC_SMART_CURRENT1_PRE2_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE2_current1_pre2_START  (0)
#define SOC_SMART_CURRENT1_PRE2_current1_pre2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE3_UNION
 结构说明  : CURRENT0_PRE3 寄存器结构定义。地址偏移量:0x193，初值:0x00，宽度:8
 寄存器说明: current_pre3[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre3 : 8;  /* bit[0-7]: 当前电流前三次值[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE3_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE3_current0_pre3_START  (0)
#define SOC_SMART_CURRENT0_PRE3_current0_pre3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE3_UNION
 结构说明  : CURRENT1_PRE3 寄存器结构定义。地址偏移量:0x194，初值:0x00，宽度:8
 寄存器说明: current_pre3[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre3 : 8;  /* bit[0-7]: 当前电流前三次值[15:8]。 */
    } reg;
} SOC_SMART_CURRENT1_PRE3_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE3_current1_pre3_START  (0)
#define SOC_SMART_CURRENT1_PRE3_current1_pre3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE4_UNION
 结构说明  : CURRENT0_PRE4 寄存器结构定义。地址偏移量:0x195，初值:0x00，宽度:8
 寄存器说明: current_pre4[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre4 : 8;  /* bit[0-7]: 当前电流前四次值[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE4_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE4_current0_pre4_START  (0)
#define SOC_SMART_CURRENT0_PRE4_current0_pre4_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE4_UNION
 结构说明  : CURRENT1_PRE4 寄存器结构定义。地址偏移量:0x196，初值:0x00，宽度:8
 寄存器说明: current_pre4[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre4 : 8;  /* bit[0-7]: 当前电流前四次值[15:8]。 */
    } reg;
} SOC_SMART_CURRENT1_PRE4_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE4_current1_pre4_START  (0)
#define SOC_SMART_CURRENT1_PRE4_current1_pre4_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE5_UNION
 结构说明  : CURRENT0_PRE5 寄存器结构定义。地址偏移量:0x197，初值:0x00，宽度:8
 寄存器说明: current_pre5[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre5 : 8;  /* bit[0-7]: 当前电流前五次值[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE5_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE5_current0_pre5_START  (0)
#define SOC_SMART_CURRENT0_PRE5_current0_pre5_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE5_UNION
 结构说明  : CURRENT1_PRE5 寄存器结构定义。地址偏移量:0x198，初值:0x00，宽度:8
 寄存器说明: current_pre5[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre5 : 8;  /* bit[0-7]: 当前电流前五次值[15:8]。 */
    } reg;
} SOC_SMART_CURRENT1_PRE5_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE5_current1_pre5_START  (0)
#define SOC_SMART_CURRENT1_PRE5_current1_pre5_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE6_UNION
 结构说明  : CURRENT0_PRE6 寄存器结构定义。地址偏移量:0x199，初值:0x00，宽度:8
 寄存器说明: current_pre6[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre6 : 8;  /* bit[0-7]: 当前电流前六次值[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE6_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE6_current0_pre6_START  (0)
#define SOC_SMART_CURRENT0_PRE6_current0_pre6_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE6_UNION
 结构说明  : CURRENT1_PRE6 寄存器结构定义。地址偏移量:0x19A，初值:0x00，宽度:8
 寄存器说明: current_pre6[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre6 : 8;  /* bit[0-7]: 当前电流前六次值[15:8]。 */
    } reg;
} SOC_SMART_CURRENT1_PRE6_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE6_current1_pre6_START  (0)
#define SOC_SMART_CURRENT1_PRE6_current1_pre6_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE7_UNION
 结构说明  : CURRENT0_PRE7 寄存器结构定义。地址偏移量:0x19B，初值:0x00，宽度:8
 寄存器说明: current_pre7[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre7 : 8;  /* bit[0-7]: 当前电流前七次值[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE7_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE7_current0_pre7_START  (0)
#define SOC_SMART_CURRENT0_PRE7_current0_pre7_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE7_UNION
 结构说明  : CURRENT1_PRE7 寄存器结构定义。地址偏移量:0x19C，初值:0x00，宽度:8
 寄存器说明: current_pre7[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre7 : 8;  /* bit[0-7]: 当前电流前七次值[15:8]。 */
    } reg;
} SOC_SMART_CURRENT1_PRE7_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE7_current1_pre7_START  (0)
#define SOC_SMART_CURRENT1_PRE7_current1_pre7_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE8_UNION
 结构说明  : CURRENT0_PRE8 寄存器结构定义。地址偏移量:0x19D，初值:0x00，宽度:8
 寄存器说明: current_pre8[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre8 : 8;  /* bit[0-7]: 当前电流前八次值[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE8_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE8_current0_pre8_START  (0)
#define SOC_SMART_CURRENT0_PRE8_current0_pre8_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE8_UNION
 结构说明  : CURRENT1_PRE8 寄存器结构定义。地址偏移量:0x19E，初值:0x00，宽度:8
 寄存器说明: current_pre8[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre8 : 8;  /* bit[0-7]: 当前电流前八次值[15:8]。 */
    } reg;
} SOC_SMART_CURRENT1_PRE8_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE8_current1_pre8_START  (0)
#define SOC_SMART_CURRENT1_PRE8_current1_pre8_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT0_PRE9_UNION
 结构说明  : CURRENT0_PRE9 寄存器结构定义。地址偏移量:0x19F，初值:0x00，宽度:8
 寄存器说明: current_pre9[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current0_pre9 : 8;  /* bit[0-7]: 当前电流前九次值[7:0]。 */
    } reg;
} SOC_SMART_CURRENT0_PRE9_UNION;
#endif
#define SOC_SMART_CURRENT0_PRE9_current0_pre9_START  (0)
#define SOC_SMART_CURRENT0_PRE9_current0_pre9_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CURRENT1_PRE9_UNION
 结构说明  : CURRENT1_PRE9 寄存器结构定义。地址偏移量:0x1A0，初值:0x00，宽度:8
 寄存器说明: current_pre9[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current1_pre9 : 8;  /* bit[0-7]: 当前电流前九次值[15:8]。 */
    } reg;
} SOC_SMART_CURRENT1_PRE9_UNION;
#endif
#define SOC_SMART_CURRENT1_PRE9_current1_pre9_START  (0)
#define SOC_SMART_CURRENT1_PRE9_current1_pre9_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OFFSET_CURRENT_MOD_0_UNION
 结构说明  : OFFSET_CURRENT_MOD_0 寄存器结构定义。地址偏移量:0x1A1，初值:0x00，宽度:8
 寄存器说明: 电流偏置调整[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  offset_current_mod_0 : 8;  /* bit[0-7]: 电流偏置调整低8位。 */
    } reg;
} SOC_SMART_OFFSET_CURRENT_MOD_0_UNION;
#endif
#define SOC_SMART_OFFSET_CURRENT_MOD_0_offset_current_mod_0_START  (0)
#define SOC_SMART_OFFSET_CURRENT_MOD_0_offset_current_mod_0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OFFSET_CURRENT_MOD_1_UNION
 结构说明  : OFFSET_CURRENT_MOD_1 寄存器结构定义。地址偏移量:0x1A2，初值:0x00，宽度:8
 寄存器说明: 电流偏置调整[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  offset_current_mod_1 : 8;  /* bit[0-7]: 电流偏置调整高8位。 */
    } reg;
} SOC_SMART_OFFSET_CURRENT_MOD_1_UNION;
#endif
#define SOC_SMART_OFFSET_CURRENT_MOD_1_offset_current_mod_1_START  (0)
#define SOC_SMART_OFFSET_CURRENT_MOD_1_offset_current_mod_1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OFFSET_VOLTAGE_MOD_0_UNION
 结构说明  : OFFSET_VOLTAGE_MOD_0 寄存器结构定义。地址偏移量:0x1A3，初值:0x00，宽度:8
 寄存器说明: 电压偏置调整[7:0]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  offset_voltage_mod_0 : 8;  /* bit[0-7]: 电压偏置调整低8位。 */
    } reg;
} SOC_SMART_OFFSET_VOLTAGE_MOD_0_UNION;
#endif
#define SOC_SMART_OFFSET_VOLTAGE_MOD_0_offset_voltage_mod_0_START  (0)
#define SOC_SMART_OFFSET_VOLTAGE_MOD_0_offset_voltage_mod_0_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_OFFSET_VOLTAGE_MOD_1_UNION
 结构说明  : OFFSET_VOLTAGE_MOD_1 寄存器结构定义。地址偏移量:0x1A4，初值:0x00，宽度:8
 寄存器说明: 电压偏置调整[15:8]寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  offset_voltage_mod_1 : 8;  /* bit[0-7]: 电压偏置调整高8位。 */
    } reg;
} SOC_SMART_OFFSET_VOLTAGE_MOD_1_UNION;
#endif
#define SOC_SMART_OFFSET_VOLTAGE_MOD_1_offset_voltage_mod_1_START  (0)
#define SOC_SMART_OFFSET_VOLTAGE_MOD_1_offset_voltage_mod_1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_RESERVED1_UNION
 结构说明  : CLJ_RESERVED1 寄存器结构定义。地址偏移量:0x1A5，初值:0x00，宽度:8
 寄存器说明: 库仑计保留1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  clj_rw_inf1 : 8;  /* bit[0-7]: 产品软件专用，对库仑计可维可测，仅用于记录数据，不对库仑计状态产生任何作用。 */
    } reg;
} SOC_SMART_CLJ_RESERVED1_UNION;
#endif
#define SOC_SMART_CLJ_RESERVED1_clj_rw_inf1_START  (0)
#define SOC_SMART_CLJ_RESERVED1_clj_rw_inf1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_RESERVED2_UNION
 结构说明  : CLJ_RESERVED2 寄存器结构定义。地址偏移量:0x1A6，初值:0x00，宽度:8
 寄存器说明: 库仑计保留2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  clj_rw_inf2 : 8;  /* bit[0-7]: 产品软件专用，对库仑计可维可测，仅用于记录数据，不对库仑计状态产生任何作用。 */
    } reg;
} SOC_SMART_CLJ_RESERVED2_UNION;
#endif
#define SOC_SMART_CLJ_RESERVED2_clj_rw_inf2_START  (0)
#define SOC_SMART_CLJ_RESERVED2_clj_rw_inf2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_RESERVED3_UNION
 结构说明  : CLJ_RESERVED3 寄存器结构定义。地址偏移量:0x1A7，初值:0x00，宽度:8
 寄存器说明: 库仑计保留3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  clj_rw_inf3 : 8;  /* bit[0-7]: 产品软件专用，对库仑计可维可测，仅用于记录数据，不对库仑计状态产生任何作用。 */
    } reg;
} SOC_SMART_CLJ_RESERVED3_UNION;
#endif
#define SOC_SMART_CLJ_RESERVED3_clj_rw_inf3_START  (0)
#define SOC_SMART_CLJ_RESERVED3_clj_rw_inf3_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_RESERVED4_UNION
 结构说明  : CLJ_RESERVED4 寄存器结构定义。地址偏移量:0x1A8，初值:0x00，宽度:8
 寄存器说明: 库仑计保留4寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  clj_rw_inf4 : 8;  /* bit[0-7]: 产品软件专用，对库仑计可维可测，仅用于记录数据，不对库仑计状态产生任何作用。 */
    } reg;
} SOC_SMART_CLJ_RESERVED4_UNION;
#endif
#define SOC_SMART_CLJ_RESERVED4_clj_rw_inf4_START  (0)
#define SOC_SMART_CLJ_RESERVED4_clj_rw_inf4_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_RESERVED5_UNION
 结构说明  : CLJ_RESERVED5 寄存器结构定义。地址偏移量:0x1A9，初值:0x00，宽度:8
 寄存器说明: 库仑计保留5寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  i_reserve_1 : 8;  /* bit[0-7]: 库仑计模拟寄存器保留。 */
    } reg;
} SOC_SMART_CLJ_RESERVED5_UNION;
#endif
#define SOC_SMART_CLJ_RESERVED5_i_reserve_1_START  (0)
#define SOC_SMART_CLJ_RESERVED5_i_reserve_1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_RESERVED6_UNION
 结构说明  : CLJ_RESERVED6 寄存器结构定义。地址偏移量:0x1AA，初值:0x00，宽度:8
 寄存器说明: 库仑计保留6寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  i_reserve_2 : 8;  /* bit[0-7]: 库仑计模拟寄存器保留。 */
    } reg;
} SOC_SMART_CLJ_RESERVED6_UNION;
#endif
#define SOC_SMART_CLJ_RESERVED6_i_reserve_2_START  (0)
#define SOC_SMART_CLJ_RESERVED6_i_reserve_2_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_RESERVED7_UNION
 结构说明  : CLJ_RESERVED7 寄存器结构定义。地址偏移量:0x1AB，初值:0x00，宽度:8
 寄存器说明: 库仑计保留7寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  v_reserve_1 : 8;  /* bit[0-7]: 库仑计模拟寄存器保留。 */
    } reg;
} SOC_SMART_CLJ_RESERVED7_UNION;
#endif
#define SOC_SMART_CLJ_RESERVED7_v_reserve_1_START  (0)
#define SOC_SMART_CLJ_RESERVED7_v_reserve_1_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_PMU_SOFT_RST_UNION
 结构说明  : PMU_SOFT_RST 寄存器结构定义。地址偏移量:0x1AC，初值:0x00，宽度:8
 寄存器说明: PMU软复位寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  soft_rst_n : 8;  /* bit[0-7]: 库仑计软复位：
                                                     0x11:复位库仑计模块；
                                                     0x1F:解除库仑计模块复位。
                                                     写入其他无效。
                                                     
                                                     CODEC模块软复位：
                                                     0x22:复位CODEC模块；
                                                     0x2F:解除CODEC模块复位。
                                                     写入其他无效。
                                                     
                                                     DCXO模式时钟校准软复位。
                                                     0x33:复位DCXO模式；
                                                     0x3F:解除DCXO模式复位。
                                                     写入其他无效。 */
    } reg;
} SOC_SMART_PMU_SOFT_RST_UNION;
#endif
#define SOC_SMART_PMU_SOFT_RST_soft_rst_n_START  (0)
#define SOC_SMART_PMU_SOFT_RST_soft_rst_n_END    (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_DEBUG_UNION
 结构说明  : CLJ_DEBUG 寄存器结构定义。地址偏移量:0x1AD，初值:0x00，宽度:8
 寄存器说明: 库仑计DEBUG专用1寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cic_clk_inv_i    : 1;  /* bit[0]: 控制库仑计电流滤波电路第一级取反时钟。
                                                         0：无效；
                                                         1：有效。 */
        unsigned char  cic_clk_inv_v    : 1;  /* bit[1]: 控制库仑计电压滤波电路第一级取反时钟。
                                                         0：无效；
                                                         1：有效。 */
        unsigned char  adc_ana_v_output : 1;  /* bit[2]: 复用CK32BC管脚，电压ADC模拟输出。 */
        unsigned char  adc_ana_i_output : 1;  /* bit[3]: 复用CK32C管脚，电流ADC模拟输出。 */
        unsigned char  cali_en_i        : 1;  /* bit[4]: 电流检测是否校准选择。
                                                         0：保持正常；
                                                         1：电流始终不进行检测校准。 */
        unsigned char  cali_en_i_force  : 1;  /* bit[5]: 电流校准选择。
                                                         0：电流不强制校准；
                                                         1：电流强制校准。
                                                         以上功能均在clj_debug[4]=0时候作用。 */
        unsigned char  cali_en_v_force  : 1;  /* bit[6]: 电压校准选择。
                                                         0：电压不强制校准；
                                                         1：电压强制校准。
                                                         以上功能均在clj_debug[7]=0时候作用。 */
        unsigned char  cali_en_v        : 1;  /* bit[7]: 电压检测是否校准选择。
                                                         0：保持正常；
                                                         1：电压检测始终不进行校准。 */
    } reg;
} SOC_SMART_CLJ_DEBUG_UNION;
#endif
#define SOC_SMART_CLJ_DEBUG_cic_clk_inv_i_START     (0)
#define SOC_SMART_CLJ_DEBUG_cic_clk_inv_i_END       (0)
#define SOC_SMART_CLJ_DEBUG_cic_clk_inv_v_START     (1)
#define SOC_SMART_CLJ_DEBUG_cic_clk_inv_v_END       (1)
#define SOC_SMART_CLJ_DEBUG_adc_ana_v_output_START  (2)
#define SOC_SMART_CLJ_DEBUG_adc_ana_v_output_END    (2)
#define SOC_SMART_CLJ_DEBUG_adc_ana_i_output_START  (3)
#define SOC_SMART_CLJ_DEBUG_adc_ana_i_output_END    (3)
#define SOC_SMART_CLJ_DEBUG_cali_en_i_START         (4)
#define SOC_SMART_CLJ_DEBUG_cali_en_i_END           (4)
#define SOC_SMART_CLJ_DEBUG_cali_en_i_force_START   (5)
#define SOC_SMART_CLJ_DEBUG_cali_en_i_force_END     (5)
#define SOC_SMART_CLJ_DEBUG_cali_en_v_force_START   (6)
#define SOC_SMART_CLJ_DEBUG_cali_en_v_force_END     (6)
#define SOC_SMART_CLJ_DEBUG_cali_en_v_START         (7)
#define SOC_SMART_CLJ_DEBUG_cali_en_v_END           (7)


/*****************************************************************************
 结构名    : SOC_SMART_CLJ_DEBUG_2_UNION
 结构说明  : CLJ_DEBUG_2 寄存器结构定义。地址偏移量:0x1AE，初值:0x00，宽度:8
 寄存器说明: 库仑计DEBUG专用2寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  current_coul_always_off : 1;  /* bit[0]  : 电流计常关控制。
                                                                  0：正常功能；
                                                                  1：电流计常关。
                                                                  默认为1'b0。 */
        unsigned char  voltage_coul_always_off : 1;  /* bit[1]  : 电压计常关控制。
                                                                  0：正常功能；
                                                                  1：电压计常关。
                                                                  默认为1'b0。 */
        unsigned char  coul_gate_clk_en        : 1;  /* bit[2]  : 库仑计门控时钟使能。
                                                                  0：开启门控使能；
                                                                  1：关闭门控使能。
                                                                  默认为1'b0。 */
        unsigned char  reg_data_clr            : 1;  /* bit[3]  : 20组寄存器（电流，电压）检测值恢复初始值设置。
                                                                  0：不恢复初始值；
                                                                  1：恢复初始值。(写1自动清零) */
        unsigned char  reserved                : 4;  /* bit[4-7]: 保留。 */
    } reg;
} SOC_SMART_CLJ_DEBUG_2_UNION;
#endif
#define SOC_SMART_CLJ_DEBUG_2_current_coul_always_off_START  (0)
#define SOC_SMART_CLJ_DEBUG_2_current_coul_always_off_END    (0)
#define SOC_SMART_CLJ_DEBUG_2_voltage_coul_always_off_START  (1)
#define SOC_SMART_CLJ_DEBUG_2_voltage_coul_always_off_END    (1)
#define SOC_SMART_CLJ_DEBUG_2_coul_gate_clk_en_START         (2)
#define SOC_SMART_CLJ_DEBUG_2_coul_gate_clk_en_END           (2)
#define SOC_SMART_CLJ_DEBUG_2_reg_data_clr_START             (3)
#define SOC_SMART_CLJ_DEBUG_2_reg_data_clr_END               (3)


/*****************************************************************************
 结构名    : SOC_SMART_STATE_TEST_UNION
 结构说明  : STATE_TEST 寄存器结构定义。地址偏移量:0x1AF，初值:0x00，宽度:8
 寄存器说明: 库仑计DEBUG专用3寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  state_test : 3;  /* bit[0-2]: bit[2:0]表示电压电流跳转状态。
                                                     000：CLJ_OFF(库仑计关闭状态)；
                                                     001：CLJ_ON(库仑计开启状态)；
                                                     010：OCV_DETE(库仑计ocv电压采样状态)；
                                                     011：CALIBRATION(库仑计自动校准状态)；
                                                     100：NORMAL_FUNC(库仑计正常工作状态)；
                                                     101：ECO_REFLASH(库仑计进入低功耗前一拍状态)；
                                                     110：ECO_POWER_OFF(库仑计进入低功耗状态)；
                                                     111：WAITTING(库仑计退出低功耗后一拍状态)。” */
        unsigned char  reserved   : 5;  /* bit[3-7]: 保留 */
    } reg;
} SOC_SMART_STATE_TEST_UNION;
#endif
#define SOC_SMART_STATE_TEST_state_test_START  (0)
#define SOC_SMART_STATE_TEST_state_test_END    (2)






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

#endif /* end of soc_smart_interface.h */
