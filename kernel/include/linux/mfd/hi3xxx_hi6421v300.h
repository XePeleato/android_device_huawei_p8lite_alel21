/*
 * Header file for device driver Hi6421 PMIC
 *
 * Copyright (c) 2013 Linaro Ltd.
 * Copyright (C) 2011 Hisilicon.
 *
 * Guodong Xu <guodong.xu@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef	__HISI_HI6421V300_H
#define	__HISI_HI6421V300_H

#include <linux/irqdomain.h>

#define HI6421_REGS_ENA_PROTECT_TIME	(0) 	/* in microseconds */
#define HI6421_ECO_MODE_ENABLE		(1)
#define HI6421_ECO_MODE_DISABLE		(0)

#define HI6421_NR_IRQ			40
#define HI6421_MASK_FIELD		0xFF
#define HI6421_BITS			8

#define HI6421_IRQ_ALARM					0	/* RTC Alarm */
#define HI6421_IRQ_OTMP					1	/* Temperature too high */
#define HI6421_IRQ_RESET_IN				2	/* RESETIN_N signal */
#define HI6421_IRQ_ONKEY_10S				3	/* PWRON key hold for 10s */
#define HI6421_IRQ_ONKEY_8S				4	/* PWRON key hold for 8s */
#define HI6421_IRQ_ONKEY_1S				5	/* PWRON key hold for 1s */
#define HI6421_IRQ_ONKEY_UP				6	/* PWRON key released */
#define HI6421_IRQ_ONKEY_DOWN			7	/* PWRON key pressed */
#define HI6421_IRQ_OCP						8	/* BUCK/LDO overload */
#define HI6421_IRQ_COULOMB				9	/* Coulomb Counter */
#define HI6421_IRQ_BAT_LITTLE_FALL			10	/* battery little fall off */
#define HI6421_IRQ_VBUS_UP					11	/* VBUS rising */
#define HI6421_IRQ_VBUS_DOWN				12	/* VBUS falling */
#define HI6421_IRQ_VBUS_LESS				13	/* VBUS Less than 4.3V */
#define HI6421_IRQ_VBUS_BIG				14	/* VBUS big shot 5.5V */
#define HI6421_IRQ_CHARGE_IN1				15	/* Charger plugged in check out 1*/
#define HI6421_IRQ_CHARGE_IN3				16	/* Charger plugged in check out 2*/
#define HI6421_IRQ_CHARGE_IN2				17	/* Charger plugged in check out 3*/
#define HI6421_IRQ_RESET					18	/* Reset */
#define HI6421_IRQ_BATTERY_OK				19	/* battery plugged in ok */
#define HI6421_IRQ_VSYS_LOW_VOL			20	/* Vsys low voltage*/
#define HI6421_IRQ_VSYS_HI_VOL				21	/* Vsys high voltage */
#define HI6421_IRQ_BST_LOW_VOL			22	/* BST low voltage*/
#define HI6421_IRQ_BST_HI_VOL				23	/* BST high voltage */
#define HI6421_IRQ_SIM_HPD1_UP			24	/* Sim hpd1 rising */
#define HI6421_IRQ_SIM_HPD1_DOWN			25	/* Sim hpd1 falling */
#define HI6421_IRQ_SIM_HPD1_HI_LEVEL		26	/* Sim hpd1 high level */
#define HI6421_IRQ_SIM_HPD1_LOW_LEVEL	27	/* Sim hpd1 low level */
#define HI6421_IRQ_SIM_HPD2_UP			28	/* Sim hpd1 rising */
#define HI6421_IRQ_SIM_HPD2_DOWN			29	/* Sim hpd1 falling */
#define HI6421_IRQ_SIM_HPD2_HI_LEVEL		30	/* Sim hpd1 high level */
#define HI6421_IRQ_SIM_HPD2_LOW_LEVEL	31	/* Sim hpd1 low level */
#define HI6421_IRQ_FLASH_LED_SHORT		32	/* flash led short*/
#define HI6421_IRQ_FLASH_LED_CARVE		33	/* flash led varve out a way */
#define HI6421_IRQ_LOW_VOLTAGE1			34	/* low voltage 2.3V */
#define HI6421_IRQ_LOW_VOLTAGE2			35	/* low voltage 2.8V */
#define HI6421_IRQ_OVER_TEMP				36	/* over temperature 150*/

#define PMU_IRQ_REG0_ERROR_OTMP_R                0x02
#define PMU_IRQ_REG0_ERROR_HRESETN_F             0x04
#define PMU_IRQ_REG1_ERROR_BOI_R                 0x04
#define PMU_IRQ_REG1_ERROR_VBUS_DET_4P3_F        0x20
#define PMU_IRQ_REG1_ERROR_VBUS_DET_5P5_R        0x40
#define PMU_IRQ_REG2_ERROR_VSYS_OV               0x10
#define PMU_IRQ_REG2_ERROR_VSYS_UV               0x20
#define PMU_IRQ_REG2_ERROR_BST_UVP               0x40
#define PMU_IRQ_REG2_ERROR_BST_OVP               0x80
#define PMU_IRQ_REG4_ERROR_LED_SHORT_R           0x01
#define PMU_IRQ_REG4_ERROR_LED_OPEN_R            0x02
#define PMU_IRQ_REG4_ERROR_VBAT2P3_F             0x04
#define PMU_IRQ_REG4_ERROR_VBAT2P8_F             0x08
#define PMU_IRQ_REG4_ERROR_OTMP150_R             0x10

struct hi6421_pmic {
	struct resource		*res;
	struct device		*dev;
	void __iomem		*regs;
	spinlock_t		lock;
	struct irq_domain	*domain;
	int			irq;
	int			gpio;
	int			irqs[HI6421_NR_IRQ];
};

/* Register Access Helpers */
u32 hi6421_pmic_read(struct hi6421_pmic *pmic, int reg);
void hi6421_pmic_write(struct hi6421_pmic *pmic, int reg, u32 val);
void hi6421_pmic_rmw(struct hi6421_pmic *pmic, int reg, u32 mask, u32 bits);

#endif		/* __HISI_HI6421V300_H */
