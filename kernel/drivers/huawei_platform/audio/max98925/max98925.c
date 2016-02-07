/*
 * max98925-mono.c -- ALSA SoC Mono MAX98925 driver
 *
 * Copyright 2013-2014 Maxim Integrated Products
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <huawei_platform/dsm/dsm_pub.h>

#include "max98925.h"
#define SUPPORT_DEVICE_TREE
#ifdef SUPPORT_DEVICE_TREE
#include <linux/regulator/consumer.h>
#endif

#define SMARTPA_I2C_ERR (20251)

struct max98925_priv *max98925_data;
static struct dsm_dev dsm_maxim_smartpa = {
    .name = "dsm_maxim_smartpa",
    .fops = NULL,
    .buff_size = 1024,
};
struct dsm_client * max98925_dclient = NULL;

static struct reg_default max98925_reg[] = {
	{ 0x00, 0x00 }, /* Battery Voltage Data */
	{ 0x01, 0x00 }, /* Boost Voltage Data */
	{ 0x02, 0x00 }, /* Live Status0 */
	{ 0x03, 0x00 }, /* Live Status1 */
	{ 0x04, 0x00 }, /* Live Status2 */
	{ 0x05, 0x00 }, /* State0 */
	{ 0x06, 0x00 }, /* State1 */
	{ 0x07, 0x00 }, /* State2 */
	{ 0x08, 0x00 }, /* Flag0 */
	{ 0x09, 0x00 }, /* Flag1 */
	{ 0x0A, 0x00 }, /* Flag2 */
	{ 0x0B, 0x00 }, /* IRQ Enable0 */
	{ 0x0C, 0x00 }, /* IRQ Enable1 */
	{ 0x0D, 0x00 }, /* IRQ Enable2 */
	{ 0x0E, 0x00 }, /* IRQ Clear0 */
	{ 0x0F, 0x00 }, /* IRQ Clear1 */
	{ 0x10, 0x00 }, /* IRQ Clear2 */
	{ 0x11, 0xC0 }, /* Map0 */
	{ 0x12, 0x00 }, /* Map1 */
	{ 0x13, 0x00 }, /* Map2 */
	{ 0x14, 0xF0 }, /* Map3 */
	{ 0x15, 0x00 }, /* Map4 */
	{ 0x16, 0xAB }, /* Map5 */
	{ 0x17, 0x89 }, /* Map6 */
	{ 0x18, 0x00 }, /* Map7 */
	{ 0x19, 0x00 }, /* Map8 */
	{ 0x1A, 0x06 }, /* DAI Clock Mode 1 */
	{ 0x1B, 0xC0 }, /* DAI Clock Mode 2 */
	{ 0x1C, 0x00 }, /* DAI Clock Divider Denominator MSBs */
	{ 0x1D, 0x00 }, /* DAI Clock Divider Denominator LSBs */
	{ 0x1E, 0xF0 }, /* DAI Clock Divider Numerator MSBs */
	{ 0x1F, 0x00 }, /* DAI Clock Divider Numerator LSBs */
	{ 0x20, 0x50 }, /* Format */
	{ 0x21, 0x00 }, /* TDM Slot Select */
	{ 0x22, 0x00 }, /* DOUT Configuration VMON */
	{ 0x23, 0x00 }, /* DOUT Configuration IMON */
	{ 0x24, 0x00 }, /* DOUT Configuration VBAT */
	{ 0x25, 0x00 }, /* DOUT Configuration VBST */
	{ 0x26, 0x00 }, /* DOUT Configuration FLAG */
	{ 0x27, 0xFF }, /* DOUT HiZ Configuration 1 */
	{ 0x28, 0xFF }, /* DOUT HiZ Configuration 2 */
	{ 0x29, 0xFF }, /* DOUT HiZ Configuration 3 */
	{ 0x2A, 0xFF }, /* DOUT HiZ Configuration 4 */
	{ 0x2B, 0x02 }, /* DOUT Drive Strength */
	{ 0x2C, 0x90 }, /* Filters */
	{ 0x2D, 0x00 }, /* Gain */
	{ 0x2E, 0x02 }, /* Gain Ramping */
	{ 0x2F, 0x00 }, /* Speaker Amplifier */
	{ 0x30, 0x0A }, /* Threshold */
	{ 0x31, 0x00 }, /* ALC Attack */
	{ 0x32, 0x80 }, /* ALC Atten and Release */
	{ 0x33, 0x00 }, /* ALC Infinite Hold Release */
	{ 0x34, 0x92 }, /* ALC Configuration */
	{ 0x35, 0x01 }, /* Boost Converter */
	{ 0x36, 0x00 }, /* Block Enable */
	{ 0x37, 0x00 }, /* Configuration */
	{ 0x38, 0x00 }, /* Global Enable */
	{ 0x3A, 0x00 }, /* Boost Limiter */
	{ 0xFF, 0x50 }, /* Revision ID */
};

static bool max98925_volatile_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case MAX98925_R000_VBAT_DATA:
	case MAX98925_R001_VBST_DATA:
	case MAX98925_R002_LIVE_STATUS0:
	case MAX98925_R003_LIVE_STATUS1:
	case MAX98925_R004_LIVE_STATUS2:
	case MAX98925_R005_STATE0:
	case MAX98925_R006_STATE1:
	case MAX98925_R007_STATE2:
	case MAX98925_R008_FLAG0:
	case MAX98925_R009_FLAG1:
	case MAX98925_R00A_FLAG2:
	case MAX98925_R0FF_VERSION:
		return true;
	default:
		return false;
	}
}

static bool max98925_readable_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case MAX98925_R00E_IRQ_CLEAR0:
	case MAX98925_R00F_IRQ_CLEAR1:
	case MAX98925_R010_IRQ_CLEAR2:
	case MAX98925_R033_ALC_HOLD_RLS:
		return false;
	default:
		return true;
	}
};

#if 0
static int max98925_regulator_config(struct i2c_client *i2c, bool pullup, bool on)
{
	struct regulator *max98925_vcc_i2c;
	int rc;
    #define VCC_I2C_MIN_UV	1800000
    #define VCC_I2C_MAX_UV	1800000
	#define I2C_LOAD_UA		300000

	pr_info("%s: enter\n", __func__);

	if (pullup) {
		pr_info("%s: I2C PULL UP.\n", __func__);

		max98925_vcc_i2c = regulator_get(&i2c->dev, "vcc_i2c");
		if (IS_ERR(max98925_vcc_i2c)) {
			rc = PTR_ERR(max98925_vcc_i2c);
			pr_info("%s: regulator get failed rc=%d\n", __func__, rc);
			goto error_get_vtg_i2c;
		}
		if (regulator_count_voltages(max98925_vcc_i2c) > 0) {
			rc = regulator_set_voltage(max98925_vcc_i2c,
				VCC_I2C_MIN_UV, VCC_I2C_MAX_UV);
			if (rc) {
				pr_info("%s: regulator set_vtg failed rc=%d\n", __func__, rc);
				goto error_set_vtg_i2c;
			}
		}

		rc = reg_set_optimum_mode_check(max98925_vcc_i2c, I2C_LOAD_UA);
		if (rc < 0) {
			pr_info("%s: regulator vcc_i2c set_opt failed rc=%d\n", __func__, rc);
			goto error_reg_opt_i2c;
		}

		rc = regulator_enable(max98925_vcc_i2c);
		if (rc) {
			pr_info("%s: regulator vcc_i2c enable failed rc=%d\n", __func__, rc);
			goto error_reg_en_vcc_i2c;
		}

	}

	return 0;

	error_set_vtg_i2c:
		regulator_put(max98925_vcc_i2c);
	error_get_vtg_i2c:
	if (regulator_count_voltages(max98925_vcc_i2c) > 0)
		regulator_set_voltage(max98925_vcc_i2c, 0,
			VCC_I2C_MAX_UV);
	error_reg_en_vcc_i2c:
			if(pullup) 	reg_set_optimum_mode_check(max98925_vcc_i2c, 0);
	error_reg_opt_i2c:
			regulator_disable(max98925_vcc_i2c);

	return rc;
}
#endif

static ssize_t max98925_register_show(struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	int value, i;
	char valStr[20];

	buf[0] = '\0';
	for (i = 0; i <= 0x38; i++){
		regmap_read(max98925_data->regmapL, i, &value);
		sprintf(valStr, "0x%02x = 0x%02x\n", i, value);

		strcat(buf, valStr);
	}

	return strlen(buf);
}

static DEVICE_ATTR(register_list, S_IRUGO | S_IWUSR, max98925_register_show, NULL);

static int max98925_digital_mute(struct max98925_priv *max98925, int mute)
{
	if (mute) {
		pr_info("%s: maxim smartpa mute\n",__func__);
		regmap_update_bits(max98925->regmapL, MAX98925_R02D_GAIN,
			M98925_SPK_GAIN_MASK, 0x00);

		regmap_update_bits(max98925->regmapL, MAX98925_R038_GLOBAL_ENABLE,
			M98925_EN_MASK, 0x0);
	}
	else {
		pr_info("%s: maxim smartpa unmute\n",__func__);
		regmap_update_bits(max98925->regmapL, MAX98925_R02D_GAIN,
			M98925_SPK_GAIN_MASK, max98925->spk_gain);

		regmap_update_bits(max98925->regmapL, MAX98925_R036_BLOCK_ENABLE,
			M98925_BST_EN_MASK | M98925_SPK_EN_MASK |
				M98925_ADC_IMON_EN_MASK | M98925_ADC_VMON_EN_MASK,
			M98925_BST_EN_MASK | M98925_SPK_EN_MASK |
				M98925_ADC_IMON_EN_MASK | M98925_ADC_VMON_EN_MASK);
		regmap_write(max98925->regmapL, MAX98925_R038_GLOBAL_ENABLE,
			M98925_EN_MASK);
	}

	return 0;
}

/* codec sample rate and n/m dividers parameter table */
static const struct {
	u32 rate;
	u8  sr;
	u32 divisors[3][2];
} rate_table[] = {
	{ 8000, 0, {{  1,   375}, {5, 1764}, {  1,   384}}},
	{11025,	1, {{147, 40000}, {1,  256}, {147, 40960}}},
	{12000, 2, {{  1,   250}, {5, 1176}, {  1,   256}}},
	{16000, 3, {{  2,   375}, {5,  882}, {  1,   192}}},
	{22050, 4, {{147, 20000}, {1,  128}, {147, 20480}}},
	{24000, 5, {{  1,   125}, {5,  588}, {  1,   128}}},
	{32000, 6, {{  4,   375}, {5,  441}, {  1,    96}}},
	{44100, 7, {{147, 10000}, {1,   64}, {147, 10240}}},
	{48000, 8, {{  2,   125}, {5,  294}, {  1,    64}}},
};

static inline int max98925_rate_value(int rate, int clock, u8 *value, int *n, int *m)
{
	int ret = -EINVAL;
	int i;
	for (i = 0; i < ARRAY_SIZE(rate_table); i++) {
		if (rate_table[i].rate >= rate) {
			*value = rate_table[i].sr;
			*n = rate_table[i].divisors[clock][0];
			*m = rate_table[i].divisors[clock][1];
			ret = 0;
			break;
		}
	}

	pr_info("%s: sample rate is %d, returning %d\n", __func__, rate_table[i].rate, *value);

	return ret;
}

static int max98925_set_slave(struct max98925_priv *max98925)
{
	pr_info("%s: ENTER\n", __func__);
	/*
	 * 1. use BCLK instead of MCLK
	 */
	regmap_update_bits(max98925->regmapL, MAX98925_R01A_DAI_CLK_MODE1,
			M98925_DAI_CLK_SOURCE_MASK, M98925_DAI_CLK_SOURCE_MASK);
	/*
	 * 2. set DAI to slave mode
	 */
	regmap_update_bits(max98925->regmapL, MAX98925_R01B_DAI_CLK_MODE2,
			M98925_DAI_MAS_MASK, 0);
	/*
	 * 3. set BLCKs to LRCLKs to 64
	 */
	regmap_update_bits(max98925->regmapL, MAX98925_R01B_DAI_CLK_MODE2,
			M98925_DAI_BSEL_MASK, M98925_DAI_BSEL_64);
	/*
	 * 4. set VMON slots
	 */
	regmap_update_bits(max98925->regmapL, MAX98925_R022_DOUT_CFG_VMON,
			M98925_DAI_VMON_EN_MASK, M98925_DAI_VMON_EN_MASK);
	regmap_update_bits(max98925->regmapL, MAX98925_R022_DOUT_CFG_VMON,
			M98925_DAI_VMON_SLOT_MASK, M98925_DAI_VMON_SLOT_00_01);
	/*
	 * 5. set IMON slots
	 */
	regmap_update_bits(max98925->regmapL, MAX98925_R023_DOUT_CFG_IMON,
			M98925_DAI_IMON_EN_MASK, M98925_DAI_IMON_EN_MASK);
	regmap_update_bits(max98925->regmapL, MAX98925_R023_DOUT_CFG_IMON,
			M98925_DAI_IMON_SLOT_MASK, M98925_DAI_IMON_SLOT_04_05);
	return 0;
}

static void max98925_set_master(struct max98925_priv *max98925)
{
	pr_info("%s: ENTER\n", __func__);

	/*
	 * 1. use MCLK for Left channel, right channel always BCLK
	 */
	regmap_update_bits(max98925->regmapL, MAX98925_R01A_DAI_CLK_MODE1,
			M98925_DAI_CLK_SOURCE_MASK, 0);
	/*
	 * 2. set left channel DAI to master mode, right channel always slave
	 */
	regmap_update_bits(max98925->regmapL, MAX98925_R01B_DAI_CLK_MODE2,
			M98925_DAI_MAS_MASK, M98925_DAI_MAS_MASK);
}

static int max98925_set_clock(struct max98925_priv *max98925, unsigned int rate)
{
	unsigned int clock;
	unsigned int mdll;
	unsigned int n;
	unsigned int m;
	u8 dai_sr;
	switch (max98925->sysclk) {
	case 6000000:
		clock = 0;
		mdll  = M98925_MDLL_MULT_MCLKx16;
		break;
	case 11289600:
		clock = 1;
		mdll  = M98925_MDLL_MULT_MCLKx8;
		break;
	case 12000000:
		clock = 0;
		mdll  = M98925_MDLL_MULT_MCLKx8;
		break;
	case 12288000:
		clock = 2;
		mdll  = M98925_MDLL_MULT_MCLKx8;
		break;
	default:
		pr_err("%s: unsupported sysclk %u\n", __func__, max98925->sysclk);
		return -EINVAL;
	}

	if (max98925_rate_value(rate, clock, &dai_sr, &n, &m))
		return -EINVAL;

	/*
	 * 1. set DAI_SR to correct LRCLK frequency
	 */
	regmap_update_bits(max98925->regmapL, MAX98925_R01B_DAI_CLK_MODE2,
			M98925_DAI_SR_MASK, dai_sr << M98925_DAI_SR_SHIFT);
	/*
	 * 2. set DAI m divider
	 */
	regmap_write(max98925->regmapL, MAX98925_R01C_DAI_CLK_DIV_M_MSBS,
			m >> 8);
	regmap_write(max98925->regmapL, MAX98925_R01D_DAI_CLK_DIV_M_LSBS,
			m & 0xFF);
	/*
	 * 3. set DAI n divider
	 */
	regmap_write(max98925->regmapL, MAX98925_R01E_DAI_CLK_DIV_N_MSBS,
			n >> 8);
	regmap_write(max98925->regmapL, MAX98925_R01F_DAI_CLK_DIV_N_LSBS,
			n & 0xFF);
	/*
	 * 4. set MDLL
	 */
	regmap_update_bits(max98925->regmapL, MAX98925_R01A_DAI_CLK_MODE1,
			M98925_MDLL_MULT_MASK, mdll << M98925_MDLL_MULT_SHIFT);

	return 0;
}

#ifdef CONFIG_PM
static int max98925_suspend(struct device *dev)
{
	regcache_cache_only(max98925_data->regmapL, true);
	return 0;
}

static int max98925_resume(struct device *dev)
{
	regcache_cache_only(max98925_data->regmapL, false);
	return regcache_sync(max98925_data->regmapL);
}
#else
#define max98925_suspend NULL
#define max98925_resume NULL
#endif

static int max98925_open(struct inode *inode, struct file *filp)
{
	int ret = 0;
	int reg = 0;

	max98925_data->sysclk = 12288000;
	max98925_data->spk_gain = 0x14;

	ret = regmap_read(max98925_data->regmapL, MAX98925_R0FF_VERSION, &reg);
	if ((ret < 0) || (reg == 0x00)) {
		pr_err("max98925 initialization error (%d 0x%02X)\n",
			ret, reg);
		goto err_access;
	}

	pr_info("max98925 version 0x%02X\n", reg);

	regmap_write(max98925_data->regmapL, MAX98925_R038_GLOBAL_ENABLE, 0x00);

	/* It's not the default but we need to set DAI_DLY */
	regmap_write(max98925_data->regmapL, MAX98925_R020_FORMAT, M98925_DAI_DLY_MASK | M98925_DAI_CHANSZ_32);
	regmap_write(max98925_data->regmapL, MAX98925_R021_TDM_SLOT_SELECT, 0xC0);

	regmap_write(max98925_data->regmapL, MAX98925_R027_DOUT_HIZ_CFG1, 0x00);
	regmap_write(max98925_data->regmapL, MAX98925_R028_DOUT_HIZ_CFG2, 0x00);
	regmap_write(max98925_data->regmapL, MAX98925_R029_DOUT_HIZ_CFG3, 0x00);
	regmap_write(max98925_data->regmapL, MAX98925_R02A_DOUT_HIZ_CFG4, 0x00);

	regmap_write(max98925_data->regmapL, MAX98925_R02C_FILTERS, 0x49);
	regmap_write(max98925_data->regmapL, MAX98925_R034_ALC_CONFIGURATION, 0x12);

	/*****************************************************************/
	/* Set boost output to minimum until DSM is implemented          */
	regmap_write(max98925_data->regmapL, MAX98925_R037_CONFIGURATION, 0x00);
	/*****************************************************************/

	// Disable ALC muting
	regmap_write(max98925_data->regmapL, MAX98925_R03A_BOOST_LIMITER, 0xF8);
	regmap_update_bits(max98925_data->regmapL, MAX98925_R02D_GAIN,
			M98925_DAC_IN_SEL_MASK, M98925_DAC_IN_SEL_DIV2_SUMMED_DAI);

	max98925_set_clock(max98925_data, 48000);
	max98925_set_slave(max98925_data);

	ret = nonseekable_open(inode, filp);
	if (ret)
		return ret;

	filp->private_data = (void*)max98925_data;

err_access:

	pr_info("%s: exit %d\n", __func__, ret);

	return ret;
}

static int max98925_release(struct inode *inode, struct file *filp)
{
	pr_info("%s: enter\n", __func__);
	return 0;
}

static int max98925_do_ioctl(struct file *file, unsigned int cmd,
			   void __user *p, int compat_mode)
{
	int ret = 0;
	unsigned int value;
	struct max98925_reg_ops reg_val;
	unsigned int __user *pUser = (unsigned int __user *) p;
	struct max98925_priv *max98925 = (struct max98925_priv *)file->private_data;
	switch (cmd) {
	case M98925_GET_VERSION:
		ret = regmap_read(max98925->regmapL, MAX98925_R0FF_VERSION, &value);
		ret |= put_user(value, pUser);
		break;

    case M98925_GET_REG_VAL:
        if(copy_from_user(&reg_val,(void*) pUser, sizeof(struct max98925_reg_ops)))
		{
		    pr_err("%s: set reg copy_from_user fail", __func__);
			return -1;
		}
		ret = regmap_read(max98925->regmapL, reg_val.reg_addr, &reg_val.reg_val);
		if(copy_to_user((void*) pUser, &reg_val, sizeof(struct max98925_reg_ops)))
		{
		    pr_err("%s: send reg value to user fail", __func__);
			return -1;
		}
                break;
	case M98925_SET_REG_VAL:
	    if(copy_from_user(&reg_val,(void*) pUser, sizeof(struct max98925_reg_ops)))
		{
		    pr_err("%s: set reg copy_from_user fail", __func__);
			return -1;
		}
		ret = regmap_write(max98925->regmapL, reg_val.reg_addr, reg_val.reg_val);
		pr_info("%s:  maxim smartpa set reg val: addr = 0x%x, val = 0x%x\n", __func__, reg_val.reg_addr, reg_val.reg_val);
		break;

	case M98925_POWER_ON:
		ret = max98925_digital_mute(max98925, 0);
		break;

	case M98925_POWER_OFF:
		ret = max98925_digital_mute(max98925, 1);
		break;

	case M98925_GET_VOLUME:
		ret = regmap_read(max98925->regmapL, MAX98925_R02D_GAIN, &value);
		ret |= put_user(value, pUser);
		break;

	case M98925_SET_VOLUME:
		ret = get_user(value, pUser);
		ret |= regmap_write(max98925->regmapL, MAX98925_R02D_GAIN, value);
		pr_info("%s:  maxim smartpa set volume: 0x%x\n", __func__, value);
		break;

	case M98925_GET_DAICLOCK:
		ret = regmap_read(max98925->regmapL, MAX98925_R01B_DAI_CLK_MODE2, &value);
		ret |= put_user(value, pUser);
		break;

	case M98925_SET_DAICLOCK:
		ret = get_user(value, pUser);
		pr_info("%s: maxim smartpa set daiclock: %d\n", __func__, value);
		ret |= max98925_set_clock(max98925_data, value);
		ret |= max98925_set_slave(max98925_data);
		break;

	case M98925_GET_DAIFORMAT:
		ret = regmap_read(max98925->regmapL, MAX98925_R020_FORMAT, &value);
		ret |= put_user(value, pUser);
		break;

	case M98925_SET_DAIFORMAT:
		ret = get_user(value, pUser);
		pr_info("%s: maxim smartpa set daiformat: 0x%x\n",__func__, value);
		ret |= regmap_write(max98925->regmapL, MAX98925_R020_FORMAT, value);
		break;

	case M98925_GET_BOOSTVOLT:
		ret = regmap_read(max98925->regmapL, MAX98925_R037_CONFIGURATION, &value);
		ret |= put_user(value, pUser);
		break;

	case M98925_SET_BOOSTVOLT:
		ret = get_user(value, pUser);
		pr_info("%s: maxim smartpa set boost voltage: 0x%x\n",__func__, value);
		ret |= regmap_write(max98925->regmapL, MAX98925_R037_CONFIGURATION, value);
		break;

	case M98925_GET_ALCTHRESHOLD:
		ret = regmap_read(max98925->regmapL, MAX98925_R030_THRESHOLD, &value);
		ret |= put_user(value, pUser);
		break;

	case M98925_SET_ALCTHRESHOLD:
		ret = get_user(value, pUser);
		pr_info("%s: maxim smartpa set alc threshold: 0x%x\n",__func__, value);
		ret |= regmap_write(max98925->regmapL, MAX98925_R030_THRESHOLD, value);
		break;

	case M98925_GET_FILTERS:
		ret = regmap_read(max98925->regmapL, MAX98925_R02C_FILTERS, &value);
		ret |= put_user(value, pUser);
		break;

	case M98925_SET_FILTERS:
		ret = get_user(value, pUser);
		pr_info("%s: maxim smartpa set fliters: 0x%x\n",__func__, value);
		ret |= regmap_write(max98925->regmapL, MAX98925_R02C_FILTERS, value);
		break;

	case M98925_GET_GAINRAMP:
		ret = regmap_read(max98925->regmapL, MAX98925_R02E_GAIN_RAMPING, &value);
		ret |= put_user(value, pUser);
		break;

	case M98925_SET_GAINRAMP:
		ret = get_user(value, pUser);
		pr_info("%s: maxim smartpa set gainramp: 0x%x\n",__func__, value);
		ret |= regmap_write(max98925->regmapL, MAX98925_R02E_GAIN_RAMPING, value);
		break;
	}
	if(ret && !dsm_client_ocuppy(max98925_dclient))
	{
		dsm_client_record(max98925_dclient, "%s: ioctl error %d", __func__, ret);
		dsm_client_record(max98925_dclient, SMARTPA_I2C_ERR);
	}
	return ret;
}

static long max98925_ioctl(struct file *file, unsigned int command,
				unsigned long arg)
{
	return max98925_do_ioctl(file, command, (void __user *)arg, 0);
}

#ifdef CONFIG_COMPAT
static long max98925_ioctl_compat(struct file *file, unsigned int command,
				unsigned long arg)
{
	return max98925_do_ioctl(file, command, compat_ptr(arg), 1);
}
#else
#define max98925_ioctl_compat NULL
#endif

static const struct file_operations max98925_ctrl_fops = {
	.owner			= THIS_MODULE,
	.open			= max98925_open,
	.release		= max98925_release,
	.unlocked_ioctl = max98925_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= max98925_ioctl_compat,
#endif
};
/*
static irqreturn_t max98925_irq_handler(int irq, void *data)
{
	return IRQ_HANDLED;
}
*/
static struct miscdevice max98925_ctrl_miscdev = {
	.minor =    MISC_DYNAMIC_MINOR,
	.name =     "maxim_smartpa_dev",
	.fops =     &max98925_ctrl_fops,
};

static const struct regmap_config max98925_regmap = {
	.reg_bits         = 8,
	.val_bits         = 8,
	.max_register     = MAX98925_R0FF_VERSION,
	.reg_defaults     = max98925_reg,
	.num_reg_defaults = ARRAY_SIZE(max98925_reg),
	.volatile_reg     = max98925_volatile_register,
	.readable_reg     = max98925_readable_register,
	.cache_type       = REGCACHE_RBTREE,
};

static int max98925_i2c_probe(struct i2c_client *i2c_l,
			     const struct i2c_device_id *id)
{
	int ret;

	pr_err("%s: enter, device '%s'\n", __func__, id->name);

	max98925_data = kzalloc(sizeof(struct max98925_priv), GFP_KERNEL);
	if (max98925_data == NULL)
		return -ENOMEM;

	max98925_data->devtype = id->driver_data;
	max98925_data->control_data = i2c_l;
	//max98925_data->pdata = i2c_l->dev.platform_data;

	max98925_data->regmapL = regmap_init_i2c(i2c_l, &max98925_regmap);
	if (IS_ERR(max98925_data->regmapL)) {
		ret = PTR_ERR(max98925_data->regmapL);
		dev_err(&i2c_l->dev, "Failed to allocate regmapL: %d\n", ret);
		goto err_out;
	}
/*
	ret = of_property_read_u32(i2c_l->dev.of_node,
				"irq-gpio", &max98925_data->irq_gpio);
	if (ret) {
		dev_err(&i2c_l->dev, "DT has no property named irq_gpio\n");
		goto err_out;
	}

	ret = request_threaded_irq(gpio_to_irq(max98925_data->irq_gpio), NULL, max98925_irq_handler,
			IRQF_TRIGGER_HIGH | IRQF_ONESHOT, "MAX98925", max98925_data);
	if (ret < 0) {
		dev_err(&i2c_l->dev, "Failed to register IRQ for GPIO %d, error = %d\n",
			max98925_data->irq_gpio, ret);
		goto err_out;
	}
*/
	ret = sysfs_create_file(&i2c_l->dev.kobj, &dev_attr_register_list.attr);
	ret |= misc_register(&max98925_ctrl_miscdev);

	pr_info("%s: ret %d\n", __func__, ret);

	if(!max98925_dclient){
		max98925_dclient = dsm_register_client(&dsm_maxim_smartpa);
	}
err_out:

	if (ret < 0) {
		if (max98925_data->regmapL)
			regmap_exit(max98925_data->regmapL);
		kfree(max98925_data);
	}

	return ret;
}

static int max98925_i2c_remove(struct i2c_client *client)
{
	misc_deregister(&max98925_ctrl_miscdev);
	if (max98925_data && max98925_data->regmapL)
		regmap_exit(max98925_data->regmapL);

	kfree(max98925_data);
	max98925_data = NULL;

	pr_info("%s: exit\n", __func__);

	return 0;
}

static const struct dev_pm_ops max98925_pm_ops = {
	.suspend		= max98925_suspend,
	.resume			= max98925_resume,
};
static const struct of_device_id max98925_match[] = {
	{ .compatible = "huawei,max98925", },
	{},
};
MODULE_DEVICE_TABLE(of, max98925_of_ids);

static const struct i2c_device_id max98925_i2c_id[] = {
	{ "max98925", MAX98925 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, max98925_i2c_id);

static struct i2c_driver max98925_i2c_driver = {
	.driver = {
		.name = "max98925",
		.owner = THIS_MODULE,
		.pm = &max98925_pm_ops,
		.of_match_table = of_match_ptr(max98925_match),
	},
	.probe  = max98925_i2c_probe,
	.remove = max98925_i2c_remove,
	.id_table = max98925_i2c_id,
};

module_i2c_driver(max98925_i2c_driver);

MODULE_DESCRIPTION("MAX98925 misc device driver");
MODULE_AUTHOR("Bo Yang<bo.yang@maximintegrated.com>");
MODULE_LICENSE("GPL");
