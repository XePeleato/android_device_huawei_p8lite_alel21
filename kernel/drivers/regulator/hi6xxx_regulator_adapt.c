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
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/regulator/hi6xxx_regulator_adapt.h>
#include <linux/mfd/hisi_hi6xxx_pmic.h>

#ifdef _DRV_LLT_
#define static
#define udelay(n)  (n = n)
#endif

#define FORMAT_SSI_PMUREG(base, reg) (*(volatile unsigned int *)((char *)base + (reg)))
#define BSP_REG_SETBITS(base, reg, pos, bits, val) \
                          ((FORMAT_SSI_PMUREG(base, reg) = (FORMAT_SSI_PMUREG(base, reg) \
                           & (~((((unsigned int)1 << (bits)) - 1) << (pos)))) \
                           | ((unsigned int)((val) & (((unsigned int)1 << (bits)) - 1)) << (pos))))

#define BSP_REG_GETBITS(base, reg, pos, bits) \
                          (((FORMAT_SSI_PMUREG(base, reg) >> (pos)) \
                           &(((unsigned int)1 << (bits)) - 1)))

static spinlock_t regulator_pmic_spinlock;

void hi6xxx_regulator_pmic_adapt_init(void)
{
    spin_lock_init(&regulator_pmic_spinlock);
}
EXPORT_SYMBOL_GPL(hi6xxx_regulator_pmic_adapt_init);


int hi6xxx_pmic_get_status(struct hi6xxx_regulator_ctrl_regs *ctrl_regs,
                                             unsigned int mask,int shift)
{
    int value_s32 = 0;
    unsigned char value_u8 = 0;  
	value_u8 = hi6xxx_pmic_reg_read(ctrl_regs->status_reg);	
	/*
	 *the 32 bits only low 8 bits is valid.
	 */
    value_s32 = ((int)BSP_REG_GETBITS(&value_u8,0,shift,mask) & 0xff);
	return !!value_s32;
}
EXPORT_SYMBOL(hi6xxx_pmic_get_status);

int hi6xxx_pmic_on(struct hi6xxx_regulator_ctrl_regs *ctrl_regs,
                   unsigned int mask,int shift,unsigned int delay_us)
{
    unsigned char value_u8 = 0;
    unsigned int value_u32 = 0;
    unsigned long regulator_spin_flag = 0;
    /*
     * the spin-lock just for actomic option
     */
    spin_lock_irqsave(&regulator_pmic_spinlock, regulator_spin_flag);
    /*
     * though value_u32 is 0,and high 24 bits is 0;
     * we also only get low 8 bits.
     */
    BSP_REG_SETBITS(&value_u32,0,shift,mask,0x1);
    value_u8  = (unsigned char)value_u32;
    hi6xxx_pmic_reg_write(ctrl_regs->enable_reg,value_u8);
    udelay(delay_us);
    
    spin_unlock_irqrestore(&regulator_pmic_spinlock, regulator_spin_flag);

    return 0;
}
EXPORT_SYMBOL(hi6xxx_pmic_on);

int hi6xxx_pmic_off(struct hi6xxx_regulator_ctrl_regs *ctrl_regs,unsigned int mask,int shift,int flag)
{
    unsigned char value_u8 = 0;
    unsigned int value_u32 = 0;
    unsigned long regulator_spin_flag = 0;

    /*
     * the spin-lock just for actomic option
     */    
    spin_lock_irqsave(&regulator_pmic_spinlock, regulator_spin_flag);
    /*
     * though value_u32 is 0,and high 24 bits is 0;
     * we also only get low 8 bits.
     */
    BSP_REG_SETBITS(&value_u32,0,shift,mask,flag);
    value_u8  = (unsigned char)value_u32;
    hi6xxx_pmic_reg_write(ctrl_regs->disable_reg,value_u8);
    
    spin_unlock_irqrestore(&regulator_pmic_spinlock, regulator_spin_flag);

    return 0;
}
EXPORT_SYMBOL(hi6xxx_pmic_off);

int hi6xxx_pmic_list_vol_tab( unsigned int *vset_table,unsigned int vol_numb,unsigned int selector )
{
   
    if (selector >= vol_numb){
        printk("selector err %s %d  \n",__func__, __LINE__);
        return -1;
    }
    return vset_table[selector];

}
EXPORT_SYMBOL(hi6xxx_pmic_list_vol_tab);

unsigned int hi6xxx_pmic_get_voltage_index(struct hi6xxx_regulator_vset_regs *vset_regs,unsigned int mask,int shift)
{
    unsigned int value_u32 = 0;
    unsigned int ret = 0;
    
    /*
     * though value_u32 is 0,and high 24 bits is 0;
     * we also only get low 8 bits.
     */
    value_u32 = (unsigned int)hi6xxx_pmic_reg_read(vset_regs->vset_reg)&0xff;
    ret = ((unsigned int)BSP_REG_GETBITS(&value_u32,0,shift,mask) & 0xff);
    
    return ret;
}
EXPORT_SYMBOL_GPL(hi6xxx_pmic_get_voltage_index);

int  hi6xxx_pmic_set_voltage(struct hi6xxx_regulator_vset_regs *vset_regs,
                             unsigned int mask,int shift,int index)
{

    unsigned char value_u8 = 0;
    unsigned int value_u32 = 0;
    unsigned long regulator_spin_flag = 0;
    
    /*
     * the spin-lock just for actomic option
     */    
    spin_lock_irqsave(&regulator_pmic_spinlock, regulator_spin_flag);   
    /*
     * though value_u32 is 0,and high 24 bits is 0;
     * we also only get low 8 bits.
     */
    value_u32 = (unsigned int)hi6xxx_pmic_reg_read(vset_regs->vset_reg)&0xff;
    BSP_REG_SETBITS(&value_u32,0,shift,mask,index);
    value_u8  = (unsigned char)value_u32;
    hi6xxx_pmic_reg_write(vset_regs->vset_reg,value_u8);

    spin_unlock_irqrestore(&regulator_pmic_spinlock, regulator_spin_flag);

    return 0;
}
EXPORT_SYMBOL_GPL(hi6xxx_pmic_set_voltage);
