/*
 * Device driver for PMU DRIVER	 in HI6xxx SOC
 *
 * Copyright (c) 2011 Hisilicon Co. Ltd
 *
 * information about chip-hi6552 which not in hixxx_pmic dts
 *
 */

#ifndef __PMIC_CHIP_HI6552_H__
#define __PMIC_CHIP_HI6552_H__

#define HI6XXX_BITS			            (8)

/*numb of sub-interrupt*/
#define HI6XXX_NR_IRQ			        (32)

#define HI6XXX_IRQ_STAT_BASE	        (0x003)
#define HI6XXX_IRQ_MASK_BASE	        (0x007)
#define HI6XXX_IRQ_ARRAY			    (4)
#define HI6XXX_IRQ_MASK                 (0xFF)
#define HI6XXX_IRQ_CLR                  (0xFF)

#define HI6XXX_VER_REG                  (0x000)
#define HI6XXX_REG_WIDTH                (0xFF)

#define OTMP_D1R_TYPE                   (0)
#define VSYS_2P5_F_TYPE                 (1)
#define VSYS_UV_D3R_TYPE                (2)
#define VSYS_6P0_D200UR_TYPE            (3)
#define POWERON_D4sR_TYPE               (4)
#define POWERON_D20F_TYPE               (5)
#define POWERON_D20R_TYPE               (6)
#define RESERVED_IRQ1_TYPE              (7)

#define IRQ_VBUS_COMP_VBAT_FALLING      (8)
#define IRQ_VBUS_COMP_VBAT_RISING       (9)
#define OCP_SCP_R_TYPE                  (10)
#define RESERVED_IRQ2_0_TYPE            (11)
#define RESERVED_IRQ2_1_TYPE            (12)
#define RESERVED_IRQ2_2_TYPE            (13)
#define RESERVED_IRQ2_3_TYPE            (14)
#define RESERVED_IRQ2_4_TYPE            (15)

#define TCXO_CLK_SEL_R_TYPE             (16)
#define TCXO_CLK_SEL_F_TYPE             (17)
#define DCXO_CLK_SEL_R_TYPE             (18)
#define DCXO_CLK_SEL_F_TYPE             (19)
#define ALARM_ON_TYPE                   (20)
#define RESERVED_IRQ3_0_TYPE            (21)
#define RESERVED_IRQ3_1_TYPE            (22)
#define RESERVED_IRQ3_2_TYPE            (23)

#define CL_INT_I_TYPE                   (24)
#define CL_OUT_I_TYPE                   (25)
#define CL_IN_I_TYPE                    (26)
#define VBAT_INT_I_TYPE                 (27)
#define RESERVED_IRQ5_0_TYPE            (28)
#define RESERVED_IRQ5_1_TYPE            (29)
/*stubs for coul,V9 use follow interrupts,however,they do not exist in V8 pmu*/
#define CHG_TIMER_I_TYPE                (30)
#define LOAD_TIMER_I_TYPE               (31)
/*
#define RESERVED_IRQ5_2_TYPE    30
#define RESERVED_IRQ5_3_TYPE    31
*/

#endif /* end of __PMIC_CHIP_HI6552_H__ */
