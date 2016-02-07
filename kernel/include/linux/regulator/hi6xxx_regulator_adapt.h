/*
 * Device driver for regulators in HI6620 IC
 *
 * Copyright (c) 2011 Hisilicon.
 *
 * Dongbin Yu <yudongbin@huawei.com>
 *
 * this regulator's probe function will be called lots of times,,
 * because of there are lots of regulator nodes in dtb.
 * so,that's say, the driver must be inited before the regulator nodes
 * registor to system.
 *
 * Makefile have proved my guess, please refor to the makefile.
 * when the code is rebuild i hope we can build pmu sub_system.
 * init order can not base on compile
 */

#ifndef __HISI_HI6XXX_REGULATOR_ADAPT_H__
#define __HISI_HI6XXX_REGULATOR_ADAPT_H__

enum hi6xxx_regulator_type{
    PMIC_BUCK_TYPE = 0,
    PMIC_LDO_TYPE  = 1,
    PMIC_LVS_TYPE  = 2,
    PMIC_BOOST_TYPE= 3,
    MTCMOS_SC_ON_TYPE      = 4,
    MTCMOS_ACPU_ON_TYPE    = 5,
    SCHARGE_TYPE           = 6,
};

struct hi6xxx_regulator_ctrl_regs{
	unsigned int  enable_reg;
	unsigned int  disable_reg;
	unsigned int  status_reg;
};

struct hi6xxx_regulator_vset_regs{
	unsigned int vset_reg;
};

struct hi6xxx_regulator_ctrl_data{
	int          shift;
	unsigned int mask;
};

struct hi6xxx_regulator_vset_data{
	int          shift;
	unsigned int mask;
};

void hi6xxx_regulator_pmic_adapt_init(void);
int hi6xxx_pmic_get_status(struct hi6xxx_regulator_ctrl_regs *ctrl_regs,
                                             unsigned int mask,int shift);
int hi6xxx_pmic_on(struct hi6xxx_regulator_ctrl_regs *ctrl_regs,
                   unsigned int mask,int shift,unsigned int delay_us);
int hi6xxx_pmic_off(struct hi6xxx_regulator_ctrl_regs *ctrl_regs,unsigned int mask,int shift,int flag);
int hi6xxx_pmic_list_vol_tab( unsigned int *vset_table,unsigned int vol_numb,unsigned int selector );
unsigned int hi6xxx_pmic_get_voltage_index(struct hi6xxx_regulator_vset_regs *vset_regs,
                                                             unsigned int mask,int shift);
int  hi6xxx_pmic_set_voltage(struct hi6xxx_regulator_vset_regs *vset_regs,
                             unsigned int mask,int shift,int index);


#endif
