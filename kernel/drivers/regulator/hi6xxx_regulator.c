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
#include <linux/power/hi6xxx_mtcmos.h>
#include <linux/power/hisi_hi6521_charger_power.h>

#ifdef _DRV_LLT_
#define static 
static inline int of_property_read_string_index(struct device_node *np,
                        const char *propname, int index,
                        const char **out_string)
{
    return -ENOSYS;
}
#endif

/* when the Linux is runing,
 * in system there are a lot of regulator,
 * based on the nodes in dtb
 */
struct hi6xxx_regulator {
	int status;                             /*this property in child  node*/
	unsigned int off_on_delay;              /*this property in parent node*/
	enum hi6xxx_regulator_type type;        /*this property in child  node*/
	int regulator_id;

	/*this property must be unify which is in child node*/
    struct hi6xxx_regulator_ctrl_regs   ctrl_regs;
    struct hi6xxx_regulator_ctrl_data   ctrl_data;

    struct hi6xxx_regulator_vset_regs   vset_regs;
    struct hi6xxx_regulator_vset_data   vset_data;
    unsigned int                        vol_numb;
    unsigned int                        *vset_table;
	struct regulator_desc rdesc;
	int (*dt_parse)(struct hi6xxx_regulator*, struct platform_device*);
};

static int regulator_test_debug = 0;

#ifdef _DRV_LLT_
void regulator_has_full_constraints(void)
{

}
#endif

static int hi6xxx_regulator_pmic_is_enabled(struct regulator_dev *rdev)
{
    int ret = 0;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
    struct hi6xxx_regulator_ctrl_regs  *ctrl_regs = &(sreg->ctrl_regs);
    struct hi6xxx_regulator_ctrl_data  *ctrl_data = &(sreg->ctrl_data);
    /*
     * regulator is all set,but the pmu is only subset.
     * maybe this "buck"/"ldo"/"lvs" is not contrl by a core.
     * and in regulator have a "status" member ("okey" or "disable").
     */
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ ctrl_reg->enable %x+++++++\n",(unsigned int)ctrl_regs->enable_reg);
	    printk("++++++++ ctrl_reg->disable %x+++++++\n",(unsigned int)ctrl_regs->disable_reg);
	    printk("++++++++ ctrl_reg->status %x+++++++\n",(unsigned int)ctrl_regs->status_reg);
	    printk("++++++++ ctrl_data->shift %x+++++++\n",(unsigned int)ctrl_data->shift);
	    printk("++++++++ ctrl_data->mask %x+++++++\n",(unsigned int)ctrl_data->mask);
    }
    ret = hi6xxx_pmic_get_status(ctrl_regs,ctrl_data->mask,ctrl_data->shift);

    return ret;
}

static int hi6xxx_regulator_pmic_enable(struct regulator_dev *rdev)
{
    int ret = 0;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
    struct hi6xxx_regulator_ctrl_regs  *ctrl_regs = &(sreg->ctrl_regs);
    struct hi6xxx_regulator_ctrl_data  *ctrl_data = &(sreg->ctrl_data);
    /*
     * regulator is all set,but the pmu is only subset.
     * maybe this "buck"/"ldo"/"lvs" is not contrl by a core.
     * and in regulator have a "status" member (okey or disable).
     * maybe we can del some regulator which is not contrl by core.
     */
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ ctrl_reg->enable %x+++++++\n",(unsigned int)ctrl_regs->enable_reg);
	    printk("++++++++ ctrl_reg->disable %x+++++++\n",(unsigned int)ctrl_regs->disable_reg);
	    printk("++++++++ ctrl_reg->status %x+++++++\n",(unsigned int)ctrl_regs->status_reg);
	    printk("++++++++ ctrl_data->shift %x+++++++\n",(unsigned int)ctrl_data->shift);
	    printk("++++++++ ctrl_data->mask %x+++++++\n",(unsigned int)ctrl_data->mask);
	    printk("++++++++ sreg->off_on_delay %x+++++++\n",(unsigned int)sreg->off_on_delay);
	}
    ret = hi6xxx_pmic_on(ctrl_regs,ctrl_data->mask,ctrl_data->shift,sreg->off_on_delay);

    return ret;
}

static int hi6xxx_regulator_pmic_disable(struct regulator_dev *rdev)
{
    int ret = 0;
    int flag = 1;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
    struct hi6xxx_regulator_ctrl_regs  *ctrl_regs = &(sreg->ctrl_regs);
    struct hi6xxx_regulator_ctrl_data  *ctrl_data = &(sreg->ctrl_data);

    /*
     * regulator is all set,but the pmu is only subset.
     * maybe this "buck"/"ldo"/"lvs" is not contrl by a core.
     * and in regulator have a "status" member (okey or disable).
     * maybe we can del some regulator which is not contrl by core.
     */
    if(sreg->type == PMIC_BOOST_TYPE){
		flag = 0;
    }

    /*
	 * for flag init value = 1;
     */
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ ctrl_reg->enable %x+++++++\n",(unsigned int)ctrl_regs->enable_reg);
	    printk("++++++++ ctrl_reg->disable %x+++++++\n",(unsigned int)ctrl_regs->disable_reg);
	    printk("++++++++ ctrl_reg->status %x+++++++\n",(unsigned int)ctrl_regs->status_reg);
	    printk("++++++++ ctrl_data->shift %x+++++++\n",(unsigned int)ctrl_data->shift);
	    printk("++++++++ ctrl_data->mask %x+++++++\n",(unsigned int)ctrl_data->mask);
	    printk("++++++++ flag %x+++++++\n",(unsigned int)flag);
	}
    ret = hi6xxx_pmic_off(ctrl_regs,ctrl_data->mask,ctrl_data->shift,flag);

    return ret;
}

static int hi6xxx_regulator_pmic_list_voltage_linear(struct regulator_dev *rdev,
				  unsigned int selector)
{

    int i;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
    /*
     * regulator is all set,but the pmu is only subset.
     * maybe this "buck"/"ldo"/"lvs" is not contrl by a core.
     * and in regulator have a "status" member (okey or disable).
     * maybe we can del some regulator which is not contrl by core.
     * we will return min_uV
     */
    if(sreg->type == PMIC_LVS_TYPE){
		return 900000;
    }

    if (selector >= sreg->vol_numb){
        printk("selector err %s %d  \n",__func__, __LINE__);
        return -1;
    }

	if (regulator_test_debug){
	    for (i=0;i<sreg->vol_numb;i++){
			printk("%d ",(int)sreg->vset_table[i]);
	    }
	    printk("+++++++++++++++++++++++++++++++++++\n");
    }
    return sreg->vset_table[selector];
}

static int hi6xxx_regulator_pmic_get_voltage(struct regulator_dev *rdev)
{
    int i;
    int index = 0;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
    struct hi6xxx_regulator_vset_regs  *vset_regs = &(sreg->vset_regs);
    struct hi6xxx_regulator_vset_data  *vset_data = &(sreg->vset_data);

    if(sreg->type == PMIC_LVS_TYPE){
		return 900000;
    }
    
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ vset_regs->vset_reg %x+++++++\n",(unsigned int)vset_regs->vset_reg);
	    printk("++++++++ vset_data->shift %x+++++++\n",(unsigned int)vset_data->shift);
	    printk("++++++++ vset_data->mask %x+++++++\n",(unsigned int)vset_data->mask);
	}

    index = hi6xxx_pmic_get_voltage_index(vset_regs,vset_data->mask,vset_data->shift);

    if (regulator_test_debug){
	    printk("++++++++ index %x+++++++\n",(unsigned int)index);
	    printk("+++++++++++++++++++++++++++++++++++\n");
	    for (i=0;i<sreg->vol_numb;i++){
			printk("%d ",(int)sreg->vset_table[i]);
	    }
	    printk("+++++++++++++++++++++++++++++++++++\n");
    }

    return sreg->vset_table[index];
}

static int hi6xxx_regulator_pmic_set_voltage(struct regulator_dev *rdev,
				int min_uV, int max_uV, unsigned *selector)
{
    int i = 0;
    int ret = 0;
    int vol = 0;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
    struct hi6xxx_regulator_vset_regs  *vset_regs = &(sreg->vset_regs);
    struct hi6xxx_regulator_vset_data  *vset_data = &(sreg->vset_data);

    if(sreg->type == PMIC_LVS_TYPE){
		return 0;
    }
    
    /*
     * search the matched vol and get its index
     */
    for(i = 0;i<sreg->vol_numb;i++){
        vol = sreg->vset_table[i];

        if((vol >= min_uV)&&(vol <= max_uV))
        {
            break;
        }
    }

    if (i == sreg->vol_numb){
		return -1;
    }
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ min_uV %d max_uV %d++++++++++++++\n",min_uV,max_uV);
	    printk("++++++++ vset_regs->vset_reg %x+++++++\n",(unsigned int)vset_regs->vset_reg);
	    printk("++++++++ vset_data->shift %x+++++++\n",(unsigned int)vset_data->shift);
	    printk("++++++++ vset_data->mask %x+++++++\n",(unsigned int)vset_data->mask);
	    printk("++++++++ i %x+++++++\n",(unsigned int)i);
	}

    ret = hi6xxx_pmic_set_voltage(vset_regs,vset_data->mask,vset_data->shift,i);
    *selector = i;
    if (regulator_test_debug){
	    printk("+++++++++++++++++++++++++++++++++++\n");
	    for (i=0;i<sreg->vol_numb;i++){
			printk("%d ",(int)sreg->vset_table[i]);
	    }
	    printk("+++++++++++++++++++++++++++++++++++\n");
	}
    return ret;
}

static unsigned int hi6xxx_regulator_pmic_get_mode(struct regulator_dev *rdev)
{
    printk(KERN_INFO"+++++++++in [%s]++++++++",__func__);
    return REGULATOR_MODE_NORMAL;
}

static int hi6xxx_regulator_pmic_set_mode(struct regulator_dev *rdev,
						unsigned int mode)

{
    printk(KERN_INFO"+++++++++in [%s]++++++++",__func__);
    return 0;
}

static unsigned int hi6xxx_regulator_pmic_get_optimum_mode(struct regulator_dev *rdev,
			int input_uV, int output_uV, int load_uA)

{
    printk(KERN_INFO"+++++++++in [%s]++++++++",__func__);
    return REGULATOR_MODE_NORMAL;
}

static struct regulator_ops hi6xxx_regulator_pmic_rops = {
	.is_enabled = hi6xxx_regulator_pmic_is_enabled,
	.enable = hi6xxx_regulator_pmic_enable,
	.disable = hi6xxx_regulator_pmic_disable,
	.list_voltage = hi6xxx_regulator_pmic_list_voltage_linear,
	.get_voltage = hi6xxx_regulator_pmic_get_voltage,
	.set_voltage = hi6xxx_regulator_pmic_set_voltage,
	.get_mode = hi6xxx_regulator_pmic_get_mode,
	.set_mode = hi6xxx_regulator_pmic_set_mode,
	.get_optimum_mode = hi6xxx_regulator_pmic_get_optimum_mode,
};

static int hi6xxx_regulator_mtcmos_is_enabled(struct regulator_dev *rdev)
{
    int ret;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
    struct hi6xxx_regulator_ctrl_regs  *ctrl_regs = &(sreg->ctrl_regs);
    struct hi6xxx_regulator_ctrl_data  *ctrl_data = &(sreg->ctrl_data);

    /*TODO:build regulator dev*/
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ ctrl_reg->enable %x+++++++\n",(unsigned int)ctrl_regs->enable_reg);
	    printk("++++++++ ctrl_reg->disable %x+++++++\n",(unsigned int)ctrl_regs->disable_reg);
	    printk("++++++++ ctrl_reg->status %x+++++++\n",(unsigned int)ctrl_regs->status_reg);
	    printk("++++++++ ctrl_data->mask %x+++++++\n",(unsigned int)ctrl_data->mask);
	    printk("++++++++ ctrl_data->shift %x+++++++\n",(unsigned int)ctrl_data->shift);
    }

    ret = hi6xxx_mtcmos_get_status(ctrl_regs,ctrl_data->mask,ctrl_data->shift,sreg->type);

    return ret;
}

/*
 * return value: success 0   failed -1
 */
static int hi6xxx_regulator_mtcmos_enabled(struct regulator_dev *rdev)
{
    int ret;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
    struct hi6xxx_regulator_ctrl_regs  *ctrl_regs = &(sreg->ctrl_regs);
    struct hi6xxx_regulator_ctrl_data  *ctrl_data = &(sreg->ctrl_data);

    /*TODO:build regulator dev*/
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ ctrl_reg->enable %x+++++++\n",(unsigned int)ctrl_regs->enable_reg);
	    printk("++++++++ ctrl_reg->disable %x+++++++\n",(unsigned int)ctrl_regs->disable_reg);
	    printk("++++++++ ctrl_reg->status %x+++++++\n",(unsigned int)ctrl_regs->status_reg);
	    printk("++++++++ ctrl_data->mask %x+++++++\n",(unsigned int)ctrl_data->mask);
	    printk("++++++++ ctrl_data->shift %x+++++++\n",(unsigned int)ctrl_data->shift);
	}
    ret = hi6xxx_mtcmos_on(ctrl_regs,ctrl_data->mask,ctrl_data->shift,sreg->type);

    return ret;
}

/*
 * there is no two return value, only 0
 */
static int hi6xxx_regulator_mtcmos_disabled(struct regulator_dev *rdev)
{
    int ret;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
    struct hi6xxx_regulator_ctrl_regs  *ctrl_regs = &(sreg->ctrl_regs);
    struct hi6xxx_regulator_ctrl_data  *ctrl_data = &(sreg->ctrl_data);

    /*TODO:build regulator dev*/
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ ctrl_reg->enable %x+++++++\n",(unsigned int)ctrl_regs->enable_reg);
	    printk("++++++++ ctrl_reg->disable %x+++++++\n",(unsigned int)ctrl_regs->disable_reg);
	    printk("++++++++ ctrl_reg->status %x+++++++\n",(unsigned int)ctrl_regs->status_reg);
	    printk("++++++++ ctrl_data->mask %x+++++++\n",(unsigned int)ctrl_data->mask);
	    printk("++++++++ ctrl_data->shift %x+++++++\n",(unsigned int)ctrl_data->shift);
    }
    ret = hi6xxx_mtcmos_off(ctrl_regs,ctrl_data->mask,ctrl_data->shift,sreg->type);

    return ret;
}

static struct regulator_ops hi6xxx_regulator_mtcmos_rops = {
	.is_enabled = hi6xxx_regulator_mtcmos_is_enabled,
	.enable = hi6xxx_regulator_mtcmos_enabled,
	.disable = hi6xxx_regulator_mtcmos_disabled,
};

static int hi6xxx_regulator_scharge_is_enabled(struct regulator_dev *dev)
{
	int ret = 0;
	int regulator_id = 0;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(dev);
    struct hi6xxx_regulator_ctrl_regs  *ctrl_regs = &(sreg->ctrl_regs);
    struct hi6xxx_regulator_ctrl_data  *ctrl_data = &(sreg->ctrl_data);
	regulator_id =  rdev_get_id(dev);
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ ctrl_reg->enable %x+++++++\n",(unsigned int)ctrl_regs->enable_reg);
	    printk("++++++++ ctrl_reg->disable %x+++++++\n",(unsigned int)ctrl_regs->disable_reg);
	    printk("++++++++ ctrl_reg->status %x+++++++\n",(unsigned int)ctrl_regs->status_reg);
	    printk("++++++++ ctrl_data->shift %x+++++++\n",(unsigned int)ctrl_data->shift);
	    printk("++++++++ ctrl_data->mask %x+++++++\n",(unsigned int)ctrl_data->mask);
	    printk("++++++++ sreg->off_on_delay %x+++++++\n",(unsigned int)sreg->off_on_delay);
		printk("++++++++ sreg->id %x+++++++\n",(unsigned int)regulator_id);
	}
	ret = scharger_power_status(regulator_id);
    if (1 == ret) {
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}
static int hi6xxx_regulator_scharge_enable(struct regulator_dev *dev)
{
	int ret = 0;
	int regulator_id = 0;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(dev);
    struct hi6xxx_regulator_ctrl_regs  *ctrl_regs = &(sreg->ctrl_regs);
    struct hi6xxx_regulator_ctrl_data  *ctrl_data = &(sreg->ctrl_data);
	regulator_id =  rdev_get_id(dev);	

    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ ctrl_reg->enable %x+++++++\n",(unsigned int)ctrl_regs->enable_reg);
	    printk("++++++++ ctrl_reg->disable %x+++++++\n",(unsigned int)ctrl_regs->disable_reg);
	    printk("++++++++ ctrl_reg->status %x+++++++\n",(unsigned int)ctrl_regs->status_reg);
	    printk("++++++++ ctrl_data->shift %x+++++++\n",(unsigned int)ctrl_data->shift);
	    printk("++++++++ ctrl_data->mask %x+++++++\n",(unsigned int)ctrl_data->mask);
	    printk("++++++++ sreg->off_on_delay %x+++++++\n",(unsigned int)sreg->off_on_delay);
		printk("++++++++ sreg->id %x+++++++\n",(unsigned int)regulator_id);
	}
    ret = scharger_power_on(regulator_id);
    if (0 == ret) {
        ret = 0;
    } else {
        ret = -1;
    }

    return ret;
}
static int hi6xxx_regulator_scharge_disable(struct regulator_dev *dev)
{
   	int ret = 0;
	int regulator_id = 0;
	regulator_id =  rdev_get_id(dev);
    ret = scharger_power_off(regulator_id);
    if (0 == ret) {
        ret = 0;
    } else {
        ret = -1;
    }

    return ret;
	
}
static int hi6xxx_regulator_scharge_list_voltage_linear(struct regulator_dev *rdev,
				  unsigned int selector)
{
    int i;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(rdev);
 
	//regulator_list_voltage_table

    if (selector >= sreg->vol_numb){
        printk("selector err %s %d  \n",__func__, __LINE__);
        return -1;
    }

	if (regulator_test_debug){
	    for (i=0;i<sreg->vol_numb;i++){
			printk("%d ",(int)sreg->vset_table[i]);
	    }
	    printk("+++++++++++++++++++++++++++++++++++\n");
    }
    return sreg->vset_table[selector];
}
static int hi6xxx_regulator_scharge_get_voltage(struct regulator_dev *dev)
{
	int i;
    int index = 0;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(dev);
    struct hi6xxx_regulator_vset_regs  *vset_regs = &(sreg->vset_regs);
    struct hi6xxx_regulator_vset_data  *vset_data = &(sreg->vset_data);

    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ sreg->vol_numb %x+++++++\n",(unsigned int)sreg->vol_numb);
		printk("++++++++ vset_regs->vset_reg %x+++++++\n",(unsigned int)vset_regs->vset_reg);
	    printk("++++++++ vset_data->shift %x+++++++\n",(unsigned int)vset_data->shift);
	    printk("++++++++ vset_data->mask %x+++++++\n",(unsigned int)vset_data->mask);
		
	}

    index = scharger_power_get_voltage_index(vset_regs->vset_reg,vset_data->mask,vset_data->shift);

	if (index < 0 || index >=sreg->vol_numb)
	{
		printk("++++++++ in [%s] index  %d outside +++++++\n",__func__,index);
		return -1;
	}

    if (regulator_test_debug){
	    printk("++++++++ index %x+++++++\n",(unsigned int)index);
	    printk("+++++++++++++++++++++++++++++++++++\n");
	    for (i=0;i<sreg->vol_numb;i++){
			printk("%d ",(int)sreg->vset_table[i]);
	    }
	    printk("+++++++++++++++++++++++++++++++++++\n");
    }

    return sreg->vset_table[index];
}
static int hi6xxx_regulator_scharge_set_voltage(struct regulator_dev *dev,
				int min_uV, int max_uV, unsigned *selector)
{	
	struct hi6xxx_regulator *sreg = rdev_get_drvdata(dev);
    struct hi6xxx_regulator_vset_regs  *vset_regs = &(sreg->vset_regs);
    struct hi6xxx_regulator_vset_data  *vset_data = &(sreg->vset_data);
	u32 vsel;
	int ret = 0;
	int uV = 0;

	for (vsel = 0; vsel < sreg->rdesc.n_voltages; vsel++) {
		uV = sreg->vset_table[vsel];		
		/* Break at the first in-range value */
		if (min_uV <= uV && uV <= max_uV)
			break;
	}

	/* unlikely to happen. sanity test done by regulator core */
	if (unlikely(vsel == sreg->rdesc.n_voltages))
		return -EINVAL;

	*selector = vsel;
	/* set voltage selector */
	ret = scharger_power_set_voltage(vset_regs->vset_reg,vset_data->mask,vset_data->shift,vsel);
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
		printk("++++++++ vset_regs->vset_reg 0x%x+++++++\n",(unsigned int)vset_regs->vset_reg);
	    printk("++++++++ vset_data->shift 0x%x+++++++\n",(unsigned int)vset_data->shift);
	    printk("++++++++ vset_data->mask 0x%x+++++++\n",(unsigned int)vset_data->mask);
		printk("++++++++ sel 0x%x+++++++\n",(unsigned int)vsel);
		
	}
	return ret;
    //return 0;
}
static int hi6xxx_regulator_scharge_set_current_limit(struct regulator_dev *dev,
				int min_uA, int max_uA)
{
	struct hi6xxx_regulator *sreg = rdev_get_drvdata(dev);
    struct hi6xxx_regulator_vset_regs  *vset_regs = &(sreg->vset_regs);
    struct hi6xxx_regulator_vset_data  *vset_data = &(sreg->vset_data);
	u32 vsel;
	int ret = 0;
	int uA = 0;

	for (vsel = 0; vsel < sreg->rdesc.n_voltages; vsel++) {
		uA = sreg->vset_table[vsel];		
		/* Break at the first in-range value */
		if (min_uA <= uA && uA <= max_uA)
			break;
	}

	/* unlikely to happen. sanity test done by regulator core */
	if (unlikely(vsel == sreg->rdesc.n_voltages))
		return -EINVAL;

	/* set voltage selector */
	ret = scharger_power_set_current_limit(vset_regs->vset_reg,vset_data->mask,vset_data->shift,vsel);
    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
		printk("++++++++ vset_regs->vset_reg 0x%x+++++++\n",(unsigned int)vset_regs->vset_reg);
	    printk("++++++++ vset_data->shift 0x%x+++++++\n",(unsigned int)vset_data->shift);
	    printk("++++++++ vset_data->mask 0x%x+++++++\n",(unsigned int)vset_data->mask);
		printk("++++++++ sel 0x%x+++++++\n",(unsigned int)vsel);
		
	}
	return ret;
}
static int hi6xxx_regulator_scharge_get_current_limit(struct regulator_dev *dev)
				
{
	int i;
    int index = 0;
    struct hi6xxx_regulator *sreg = rdev_get_drvdata(dev);
    struct hi6xxx_regulator_vset_regs  *vset_regs = &(sreg->vset_regs);
    struct hi6xxx_regulator_vset_data  *vset_data = &(sreg->vset_data);

    if (regulator_test_debug){
	    printk("++++++++ in [%s] regulator name %s+++++++\n",__func__,sreg->rdesc.name);
	    printk("++++++++ sreg->vol_numb %x+++++++\n",(unsigned int)sreg->vol_numb);
		printk("++++++++ vset_regs->vset_reg %x+++++++\n",(unsigned int)vset_regs->vset_reg);
	    printk("++++++++ vset_data->shift %x+++++++\n",(unsigned int)vset_data->shift);
	    printk("++++++++ vset_data->mask %x+++++++\n",(unsigned int)vset_data->mask);
		
	}

    index = scharger_power_get_current_limit_index(vset_regs->vset_reg,vset_data->mask,vset_data->shift);

	if (index < 0 || index >=sreg->vol_numb)
	{
		printk("++++++++ in [%s] index  %d outside +++++++\n",__func__,index);
		return -1;
	}

    if (regulator_test_debug){
	    printk("++++++++ index %x+++++++\n",(unsigned int)index);
	    printk("+++++++++++++++++++++++++++++++++++\n");
	    for (i=0;i<sreg->vol_numb;i++){
			printk("%d ",(int)sreg->vset_table[i]);
	    }
	    printk("+++++++++++++++++++++++++++++++++++\n");
    }

    return sreg->vset_table[index];
}
static unsigned int hi6xxx_regulator_scharge_get_mode(struct regulator_dev *dev)
{
    return 0;
}
static int hi6xxx_regulator_scharge_set_mode(struct regulator_dev *dev,unsigned int mode)

{
    return 0;
}
static unsigned int hi6xxx_regulator_scharge_get_optimum_mode(struct regulator_dev *dev,
			int input_uV, int output_uV, int load_uA)

{
    return 0;
}


static struct regulator_ops hi6xxx_regulator_scharge_rops = {
	.is_enabled = hi6xxx_regulator_scharge_is_enabled,
	.enable = hi6xxx_regulator_scharge_enable,
	.disable = hi6xxx_regulator_scharge_disable,
	.list_voltage = hi6xxx_regulator_scharge_list_voltage_linear,
	.get_voltage = hi6xxx_regulator_scharge_get_voltage,
	.set_voltage = hi6xxx_regulator_scharge_set_voltage,
	.get_current_limit = hi6xxx_regulator_scharge_get_current_limit,
	.set_current_limit = hi6xxx_regulator_scharge_set_current_limit,
	.get_mode = hi6xxx_regulator_scharge_get_mode,
	.set_mode = hi6xxx_regulator_scharge_set_mode,
	.get_optimum_mode = hi6xxx_regulator_scharge_get_optimum_mode,
};

static int hi6xxx_regualtor_dt_parse_common(struct hi6xxx_regulator *sreg,
				struct platform_device *pdev)
{
    printk(KERN_INFO"for stub\n\r");
	return 0;
}
static int hi6xxx_regualtor_scharge_dt_parse(struct hi6xxx_regulator *sreg,
					struct platform_device *pdev)
{
    int ret;
    ret = hi6xxx_regualtor_dt_parse_common(sreg,pdev);
    printk(KERN_INFO"for stub\n\r");
	return ret;
}

static int hi6xxx_regualtor_pmic_dt_parse(struct hi6xxx_regulator *sreg,
					struct platform_device *pdev)
{
    int ret;
    ret = hi6xxx_regualtor_dt_parse_common(sreg,pdev);
    printk(KERN_INFO"for stub\n\r");
	return ret;
}

static int hi6xxx_regualtor_mtcmos_dt_parse(struct hi6xxx_regulator *sreg,
					struct platform_device *pdev)
{
    int ret;
    ret = hi6xxx_regualtor_dt_parse_common(sreg,pdev);
    printk(KERN_INFO"for stub\n\r");
	return ret;
}


static const struct hi6xxx_regulator hi6xxx_regulator_pmic = {
	.rdesc = {
		.ops = &hi6xxx_regulator_pmic_rops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	.dt_parse = hi6xxx_regualtor_pmic_dt_parse,
};

static const struct hi6xxx_regulator hi6xxx_regulator_mtcmos = {
	.rdesc = {
		.ops = &hi6xxx_regulator_mtcmos_rops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	.dt_parse = hi6xxx_regualtor_mtcmos_dt_parse,
};

static const struct hi6xxx_regulator hi6xxx_regulator_scharge = {
	.rdesc = {
		.ops = &hi6xxx_regulator_scharge_rops,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	.dt_parse = hi6xxx_regualtor_scharge_dt_parse,
};

static struct of_device_id of_hi6xxx_regulator_match_tbl[] = {
	{
		.compatible = "hisilicon,hi6xxx-regulator-pmic",
		.data = &hi6xxx_regulator_pmic,
	},
	{
		.compatible = "hisilicon,hi6xxx-regulator-mtcmos",
		.data = &hi6xxx_regulator_mtcmos,
	},
	/*in future,in V8-PRJ there is scharge which is also power supply*/
	/*so we can do base on below*/
    {
		.compatible = "hisilicon,hi6xxx-regulator-scharger",
		.data = &hi6xxx_regulator_scharge,
    },
	{ /* end */ }
};

#if 0 //def CONFIG_DEBUG_FS
extern void get_current_regulator_dev(void);
extern void set_regulator_state(char *ldo_name, int value);
extern void get_regulator_state(char *ldo_name);
static int dbg_hi6xxx_regulator_show(struct seq_file *s, void *data)
{

	printk("%-13s %-15s %-15s %-15s %-15s\n",
			"LDO_NAME", "ON/OFF", "Use_count", "Open_count", "Always_on");
	printk("-----------------------------------------"
			"-----------------------------------------------\n");
	get_current_regulator_dev();
	return 0;
}

static int dbg_hi6xxx_regulator_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_hi6xxx_regulator_show, inode->i_private);
}

static const struct file_operations debug_regulator_state_fops = {
	.open		= dbg_hi6xxx_regulator_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int dbg_control_regulator_show(struct seq_file *s, void *data)
{
	printk("usage1: echo [r]/[w] LDO_Name > control_regulator\n\r \
		    enable and disable LDO; get LDO current voltage;\n\r \
		    eg: echo W LDO16 0 > control_regulator		:disable LDO16\n\r \
		    echo W LDO16 1 > control_regulator	:enable LDO16\n\r \
		    echo R LDO16 > control_regulator	:get LDO16 voltage\n\r\n\r");
	return 0;
}
static ssize_t dbg_control_regulator_set_value(struct file *filp, const char __user *buffer,
	size_t count, loff_t *ppos)
{
	char tmp[128] = {0};
	char ptr[128] = {0};
	char num = 0;
	int i;
	int next_flag = 1;

	if (count > 128) {
		pr_info("error! buffer size big than internal buffer\n");
		return -EFAULT;
	}

	if (copy_from_user(tmp, buffer, count)) {
		pr_info("error!\n");
		return -EFAULT;
	}

	if (tmp[0] == 'R' || tmp[0] == 'r') {
		for (i = 2; i < (count - 1); i++) {
			ptr[i - 2] = tmp[i];
		}
		ptr[i - 2] = '\0';
		get_regulator_state(ptr);
	} else if (tmp[0] == 'W' || tmp[0] == 'w') {
		for (i = 2; i < (count - 1); i++) {
			if (tmp[i] == ' ') {
				next_flag = 0;
				ptr[i - 2] = '\0';
				continue;
			}
			if (next_flag) {
				ptr[i - 2] = tmp[i];
			} else {
				num = tmp[i] - 48;
			}
		}
		set_regulator_state(ptr, num);
	}

	*ppos += count;

	return count;
}

static int dbg_control_regulator_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return single_open(file, dbg_control_regulator_show, &inode->i_private);
}

static const struct file_operations set_control_regulator_fops = {
	.open		= dbg_control_regulator_open,
	.read		= seq_read,
	.write		= dbg_control_regulator_set_value,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

static struct regulator_init_data *fake_of_get_regulator_init_data(struct device *dev,
                        struct device_node *np)
{
    struct regulator_init_data *init_data = NULL;
    const __be32 *num_consumer_supplies = NULL;
    struct regulator_consumer_supply *consumer_supplies = NULL;
    int consumer_id = 0;

    init_data = devm_kzalloc(dev, sizeof(*init_data), GFP_KERNEL);
    if (!init_data)
        return NULL;

    init_data->supply_regulator = of_get_property(np,"hisilicon,regulator-supply",NULL);

    num_consumer_supplies = of_get_property(np, "hisilicon,num_consumer_supplies", NULL);
    if((NULL == num_consumer_supplies)||(0 == *num_consumer_supplies))
    {
       dev_warn(dev,"%s no consumer_supplies \n",__func__);
       return init_data;
    }

    init_data->num_consumer_supplies = be32_to_cpu(*num_consumer_supplies);
    init_data->consumer_supplies = (struct regulator_consumer_supply *)devm_kzalloc(dev,
    init_data->num_consumer_supplies *sizeof(struct regulator_consumer_supply),GFP_KERNEL);
    if(NULL == init_data->consumer_supplies)
    {
        dev_err(dev,"%s devm_kzalloc consumer_supplies err\n",__func__);
        return NULL;
    }

    consumer_supplies = init_data->consumer_supplies ;

    for(consumer_id = 0;consumer_id < init_data->num_consumer_supplies;consumer_id++,consumer_supplies++)
    {
        int ret = of_property_read_string_index(np,"hisilicon,consumer-supplies",consumer_id,&consumer_supplies->supply);
        if(ret)
        {
            dev_err(dev,"\n %s %s of_property_read_string_index consumer-supplies err\n",__func__,np->name);
        }
    }

	return init_data;
}

static int fake_of_get_regulator_constraint(struct regulation_constraints *constraints,
						struct device_node *np)
{
	const __be32 *min_uV, *max_uV;
	unsigned int *valid_modes_mask;
	unsigned int *valid_ops_mask;
	unsigned int *initial_mode;

	if (!np)
		return -1;

	if (!constraints)
		return -1;

    (constraints)->name = of_get_property(np, "regulator-name", NULL);

	min_uV = of_get_property(np, "regulator-min-microvolt", NULL);
	if (min_uV)
	{
		(constraints)->min_uV = be32_to_cpu(*min_uV);
		(constraints)->min_uA = be32_to_cpu(*min_uV);
	}		

	max_uV = of_get_property(np, "regulator-max-microvolt", NULL);
	if (max_uV)
	{
		(constraints)->max_uV = be32_to_cpu(*max_uV);
		(constraints)->max_uA = be32_to_cpu(*max_uV);
	}		

	valid_modes_mask = (unsigned int *)of_get_property(np, "hisilicon,valid-modes-mask", NULL);
	if (valid_modes_mask)
		(constraints)->valid_modes_mask = be32_to_cpu(*valid_modes_mask);

	valid_ops_mask = (unsigned int *)of_get_property(np, "hisilicon,valid-ops-mask", NULL);
	if (valid_ops_mask)
		(constraints)->valid_ops_mask = be32_to_cpu(*valid_ops_mask);

    initial_mode = (unsigned int *)of_get_property(np, "hisilicon,initial-mode", NULL);
	if (initial_mode)
		(constraints)->initial_mode = be32_to_cpu(*initial_mode);

	(constraints)->always_on = !!(of_find_property(np, "regulator-always-on", NULL));

    (constraints)->boot_on = !!(of_find_property(np, "regulator-boot-on", NULL));
	return 0;

}

static int fake_of_get_regulator_sreg(struct hi6xxx_regulator * sreg,struct device *dev,
						struct device_node *np)
{
	int *vol_numb;
    unsigned int *off_on_delay;
    enum hi6xxx_regulator_type *regulator_type;
    const char *status = NULL;
	unsigned int *vset_table = NULL;
	int *regulator_id;

    status = of_get_property(np, "hisilicon,regulator-status", NULL);
    if (status)
		sreg->status = !(strcmp(status,"okey"));

    regulator_type = (enum hi6xxx_regulator_type *)of_get_property(np, "hisilicon,regulator-type", NULL);
    if (regulator_type)
		sreg->type = be32_to_cpu(*regulator_type);

    off_on_delay = (unsigned int *)of_get_property(np, "hisilicon,off-on-delay", NULL);
	if (off_on_delay)
		sreg->off_on_delay = be32_to_cpu(*off_on_delay);

    (void)of_property_read_u32_array(np,"hisilicon,ctrl-regs", (unsigned int *)(&sreg->ctrl_regs),0x3);

    (void)of_property_read_u32_array(np,"hisilicon,ctrl-data", (unsigned int *)(&sreg->ctrl_data),0x2);

    (void)of_property_read_u32_array(np,"hisilicon,vset-regs", (unsigned int *)(&sreg->vset_regs),0x1);

    (void)of_property_read_u32_array(np,"hisilicon,vset-data", (unsigned int *)(&sreg->vset_data),0x2);

    vol_numb = (int *)of_get_property(np, "hisilicon,regulator-n-vol", NULL);
	if (vol_numb)
		sreg->vol_numb = be32_to_cpu(*vol_numb);
	
	regulator_id =(int *)of_get_property(np, "hisilicon,hisi-scharger-regulator-id",NULL);
	if (regulator_id)
		sreg->regulator_id =  be32_to_cpu(*regulator_id);

    /*
     * for the mem bug,if vset number > 8,stack will over
     * actual only buck2.return ok.

	if (sreg->vol_numb == (int)64){
        sreg->vset_table = hi6xxx_pmic_get_buck_vol_table(2);
        return 0;
	}
    */
    vset_table = devm_kzalloc(dev, sreg->vol_numb * sizeof(int), GFP_KERNEL);
	if (!vset_table){
		return -1;
	}

    (void)of_property_read_u32_array(np,"hisilicon,vset-table", (unsigned int *)vset_table,sreg->vol_numb);
	sreg->vset_table = vset_table;

	return 0;

}

static int hi6xxx_regulator_probe(struct platform_device *pdev)
{

	int ret = 0;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct regulator_dev *rdev = NULL;
	struct regulator_desc *rdesc = NULL;
	struct hi6xxx_regulator *sreg = NULL;
	struct regulator_init_data *initdata = NULL;
	const struct of_device_id *match = NULL;
	const struct hi6xxx_regulator *template = NULL;
	struct regulator_config config = { };

#if 0 //def CONFIG_DEBUG_FS
	struct dentry *d;
	static int debugfs_flag;
#endif

	//printk("+++++++++++++[in %s : 1]++++++++++++++\n",__func__);

    /*
     * build hi6xxx_regulator device
     */

	/* to check which type of regulator this is */
	match = of_match_device(of_hi6xxx_regulator_match_tbl, &pdev->dev);
	if (NULL == match) {
		pr_err("of match hi6xxx regulator fail!\n\r");
		return -EINVAL;
	}
	/*tempdev is regulator device*/
	template = match->data;

    /*
     *initdata mem will release auto;
     *this is kernel 3.10 import.
     */

    /*just for getting "std regulator node" value-key about constraint*/
    initdata = fake_of_get_regulator_init_data(dev,np);
	if (!initdata){
		pr_err("get regulator init data error !\n");
		return -EINVAL;
	}

	ret = fake_of_get_regulator_constraint(&initdata->constraints,np);
	if (!!ret){
		pr_err("get regulator constraint error !\n");
		return -EINVAL;
	}
	//printk("+++++++++++++[in %s : 2]++++++++++++++\n",__func__);

	/* TODO:hi6xxx regulator supports two modes */
	sreg = kmemdup(template, sizeof(*sreg), GFP_KERNEL);
	if (!sreg)
		return -ENOMEM;
	//printk("+++++++++++++[in %s : 3]++++++++++++++\n",__func__);

    if(0 != fake_of_get_regulator_sreg(sreg,dev,np)){
	    kfree(sreg);
		return -EINVAL;
    }

	rdesc = &sreg->rdesc;
	rdesc->n_voltages = sreg->vol_numb;
	rdesc->name = initdata->constraints.name;
	rdesc->id = sreg->regulator_id;
	rdesc->min_uV = initdata->constraints.min_uV;

	/*just for skeleton for future*/
#if 0
	ret = sreg->dt_parse(sreg, pdev);
	if (ret) {
		pr_err("device tree parameter parse error!\n");
		goto hi6xxx_probe_end;
	}
#endif
	/* to parse device tree data for regulator specific */
	config.dev = &pdev->dev;
	config.init_data = initdata;
	config.driver_data = sreg;
	config.of_node = pdev->dev.of_node;

	//printk("+++++++++++++[in %s : 4]++++++++++++++\n",__func__);

	/* register regulator */
	rdev = regulator_register(rdesc, &config);
	if (IS_ERR(rdev)) {
		pr_err("regulator failed to register %s\n",
			rdesc->name);
		ret = PTR_ERR(rdev);
		//goto hi6xxx_probe_end;
		return -EINVAL;
	}
	//printk("+++++++++++++[in %s : 5]++++++++++++++\n",__func__);

	platform_set_drvdata(pdev, rdev);
    regulator_has_full_constraints();

#if 0 //def CONFIG_DEBUG_FS
	if (debugfs_flag == 0) {
		d = debugfs_create_dir("hi6xxx_regulator_debugfs", NULL);
		if (!d) {
			pr_err("failed to create hi6xxx regulator debugfs dir !\n");
			ret = -ENOMEM;
			goto hi6xxx_probe_fail;
		}
		(void) debugfs_create_file("regulator_state", S_IRUSR,
						d, NULL, &debug_regulator_state_fops);

		(void) debugfs_create_file("control_regulator", S_IRUSR,
						d, NULL, &set_control_regulator_fops);
		debugfs_flag = 1;
	}

hi6xxx_probe_fail:
	if (ret)
		regulator_unregister(rdev);

hi6xxx_probe_end:
	if (ret)
		kfree(sreg);
#endif
	//printk("+++++++++++++[in %s : 6]++++++++++++++\n",__func__);

	return ret;
}

static int hi6xxx_regulator_remove(struct platform_device *pdev)
{
	return 0;
}


static struct platform_driver hi6xxx_regulator_driver = {
	.driver = {
		.name	= "hi6xxx_regulator",
		.owner  = THIS_MODULE,
		.of_match_table = of_hi6xxx_regulator_match_tbl,
	},
	.probe	= hi6xxx_regulator_probe,
	.remove	= hi6xxx_regulator_remove,
};

static int __init hi6xxx_regulator_init(void)
{
    /*if there is some adapt layers so must be add its init-function*/
    printk(KERN_INFO"++++++++++++++[%s]++++++++++++\n",__func__);
    hi6xxx_regulator_pmic_adapt_init();
	return platform_driver_register(&hi6xxx_regulator_driver);
}

static void __exit hi6xxx_regulator_exit(void)
{
	platform_driver_unregister(&hi6xxx_regulator_driver);
}

fs_initcall(hi6xxx_regulator_init);
module_exit(hi6xxx_regulator_exit);

MODULE_AUTHOR("Dongbin Yu <yudongbin@huawei.com>");
MODULE_DESCRIPTION("Hisi hi6xxx regulator driver");
MODULE_LICENSE("GPL v2");
