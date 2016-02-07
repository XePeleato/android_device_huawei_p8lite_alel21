/*
 * Copyright (C) 2012 Hisilicon Co. Ltd.
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __PMUSSI_DRV_H
#define __PMUSSI_DRV_H

#define PMU_VER_START  0x10
#define PMU_VER_END    0x38

#define SSI_DEVICE_OK  1
#define SSI_DEVICE_ERR 0

#define PMUSSI_REG_EX(pmu_base, reg_addr)        (((reg_addr)<<2) + (char *)pmu_base)

unsigned char hi6xxx_pmic_reg_read (unsigned int reg_addr);
void hi6xxx_pmic_reg_write (unsigned int reg_addr, unsigned char reg_val);
unsigned char hi6xxx_pmic_reg_read_ex (void *pmu_base, unsigned int reg_addr);
void hi6xxx_pmic_reg_write_ex (void *pmu_base, unsigned int reg_addr, unsigned char reg_val);
int hi6xxx_pmic_recv_data (unsigned int reg_addr, char *buf, int len);
int hi6xxx_pmic_send_data (unsigned int reg_addr, char *buf, int len);
int *hi6xxx_pmic_get_buck_vol_table(int id);
unsigned int hi6xxx_pmic_get_version(void);
int hi6xxx_pmic_device_stat_notify(char *dev_name, int dev_stat);

#endif
