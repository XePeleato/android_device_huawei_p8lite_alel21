/*
 * Device driver for PMU DRIVER	 in HI6xxx SOC
 *
 * Copyright (c) 2011 Hisilicon Co. Ltd
 *
 * information about chip-hi6551 which not in hixxx_pmic dts
 *
 */

#ifndef __PMIC_CHIP_HI6551_H__
#define __PMIC_CHIP_HI6551_H__


#define HI6XXX_BITS			            (8)

/*numb of sub-interrupt*/
#define HI6XXX_NR_IRQ			        (40)

#define HI6XXX_IRQ_STAT_BASE	        (0x003)
#define HI6XXX_IRQ_MASK_BASE	        (0x008)
#define HI6XXX_IRQ_ARRAY			    (5)
#define HI6XXX_IRQ_MASK                 (0xFF)
#define HI6XXX_IRQ_CLR                  (0xFF)

#define HI6XXX_VER_REG                  (0x115)
#define HI6XXX_REG_WIDTH                (0xFF)

#define OTMP_D1R_TYPE                   (0)
#define VSYS_2P5_F_TYPE                 (1)
#define VSYS_UV_D2F_TYPE                (2)
#define VSYS_6P0_D200UR_TYPE            (3)
#define POWERON_D4sR_TYPE               (4)
#define POWERON_D20F_TYPE               (5)
#define POWERON_D20R_TYPE               (6)
#define RESERVED_IRQ1_TYPE              (7)

#define OCP_SCP_R_TYPE                  (8)
#define VBUS_DET_1P375_D90UR_TYPE       (9)
#define IRQ_VBUS_COMP_VBAT_FALLING      (10)
#define IRQ_VBUS_COMP_VBAT_RISING       (11)
#define RESERVED_IRQ2_1_TYPE            (12)
#define RESERVED_IRQ2_2_TYPE            (13)
#define RESERVED_IRQ2_3_TYPE            (14)
#define RESERVED_IRQ2_4_TYPE            (15)

#define ALARM_ON_A_TYPE                 (16)
#define ALARM_ON_B_TYPE                 (17)
#define ALARM_ON_C_TYPE                 (18)
#define ALARM_ON_D_TYPE                 (19)
#define ALARM_ON_E_TYPE                 (20)
#define ALARM_ON_F_TYPE                 (21)
#define RESERVED_IRQ3_1_TYPE            (22)
#define RESERVED_IRQ3_2_TYPE            (23)

#define SIM1_HPD_OUT_F_TYPE             (24)
#define SIM1_HPD_OUT_R_TYPE             (25)
#define SIM1_HPD_IN_F_TYPE              (26)
#define SIM1_HPD_IN_R_TYPE              (27)
#define SIM2_HPD_OUT_F_TYPE             (28)
#define SIM2_HPD_OUT_R_TYPE             (29)
#define SIM2_HPD_IN_F_TYPE              (30)
#define SIM2_HPD_IN_R_TYPE              (31)

#define CL_INT_I_TYPE                   (32)
#define CL_OUT_I_TYPE                   (33)
#define CL_IN_I_TYPE                    (34)
#define CHG_TIMER_I_TYPE                (35)
#define LOAD_TIMER_I_TYPE               (36)
#define VBAT_INT_I_TYPE                 (37)
#define RESERVED_IRQ5_1_TYPE            (38)
#define RESERVED_IRQ5_2_TYPE            (39)

#endif /* end of __PMIC_CHIP_HI6552_H__ */
