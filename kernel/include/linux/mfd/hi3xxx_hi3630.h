/*
 * Header file for device driver hi3630 power manger
 *
 * Copyright (c) 2013 Linaro Ltd.
 * Copyright (C) 2011 Hisilicon.
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

#ifndef	__HISI_HI3630_REGULATOR_H
#define	__HISI_HI3630_REGULATOR_H
/*soft reset enable register of isp,venc,vdec,vpp,dss2,dss1,dss0*/
#define SCPERRSTEN1_REG	0x084
/*soft reset disable register of isp,venc,vdec,vpp,dss2,dss1,dss0*/
#define SCPERRSTDIS1_REG	0x088
/*open and close mtmos */
#define PWR_EN_REG			0x150
#define PWR_DIS_REG		0x154
/*enable and disable iso*/
#define EN_ISO_REG 			0x144
#define DIS_ISO_REG 		0x148

#define hi3630_ISP		0
#define hi3630_VENC		1
#define hi3630_VDEC		2
#define hi3630_VPP		3
#define hi3630_DSS0		4
#define hi3630_DSS1 	5
#define hi3630_DSS2 	6
#define hi3630_G3D		7

struct hisi_regulator_hi3630_core {
	struct resource	*res;
	struct device *dev;
	void __iomem *regs;
	spinlock_t lock;
};

/* Register Access Helpers */
static inline u32 hisi_regulator_hi3630_core_read(struct hisi_regulator_hi3630_core *pmic, int reg)
{
	return readl(pmic->regs + reg);
}

static inline void hisi_regulator_hi3630_core_write(struct hisi_regulator_hi3630_core *pmic, int reg, u32 val)
{
	writel(val, pmic->regs + reg);
}

#endif		/* __HISI_HI3630_REGULATOR_H */
