#ifndef __MTCMOS_DRIVER_H__
#define __MTCMOS_DRIVER_H__

#include <linux/regulator/hi6xxx_regulator_adapt.h>

int hi6xxx_mtcmos_on(struct hi6xxx_regulator_ctrl_regs  *regs,
                               unsigned int mask,int shift,int type);
int hi6xxx_mtcmos_off(struct hi6xxx_regulator_ctrl_regs  *regs,
                               unsigned int mask,int shift,int type);
int hi6xxx_mtcmos_get_status(struct hi6xxx_regulator_ctrl_regs  *regs,
                               unsigned int mask,int shift,int type);
#endif /* end of mtcmos_driver.h */
