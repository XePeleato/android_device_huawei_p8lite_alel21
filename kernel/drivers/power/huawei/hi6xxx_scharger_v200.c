/*
 * drivers/power/scharger_V200.c
 *
 * HI6521 charging driver
 *
 * Copyright (C) 2012-2015 HUAWEI, Inc.
 * Author: HUAWEI, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/wakelock.h>
#include <linux/usb/otg.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/semaphore.h>
#include <linux/power_supply.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <huawei_platform/log/hw_log.h>
#include <hi6xxx_scharger_v200.h>
//#include <dev_scharger_v200.h>
//#include <regulator_info.h>
//#include <bq27510_battery.h>
#include <bq_bci_battery.h>
//#include <mach/gpio.h>
//#include <mach/irqs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/notifier.h>
//#include <linux/mux.h>
#include <linux/huawei/usb/usb_charger.h>
#include <linux/huawei/usb/hisi_susb.h>
//#include <linux/usb/hiusb_android.h>
#include <linux/mutex.h>
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
#include <linux/hw_dev_dec.h>
#endif
#include <hisi_coul_drv.h>
#include "../../hisi/mntn/excDrv.h"
#include <linux/power/hisi_hi6521_charger_power.h>
#if defined (CONFIG_HUAWEI_DSM)
#include <huawei_platform/dsm/dsm_pub.h>
#endif
#include <huawei_platform/log/log_exception.h>
#include <linux/timer.h>
#include <linux/rtc.h>

#ifdef _DRV_LLT_
#define STATIC
#else
#define STATIC static
#endif
/*
#undef dev_info
#define dev_info dev_err
*/
#define HWLOG_TAG scharger_v200
HWLOG_REGIST();

#define DPM_CHECK_FOR_V204 1

#if defined (CONFIG_HUAWEI_DSM)
static struct dsm_dev dsm_charger = {
	.name = "dsm_charger",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 1024,
};
static struct dsm_client *charger_dclient = NULL;
#endif

struct charge_params {
    unsigned int       currentmA;
    unsigned int       voltagemV;
    unsigned int       term_currentmA;
    unsigned int       enable_iterm;
    bool                enable;
};

struct hi6521_device_info {
    struct device        *dev;
    struct i2c_client    *client;
    struct charge_params  params;
    struct delayed_work   hi6521_charger_work;
    struct delayed_work   hi6521_usb_otg_work;
#if DPM_CHECK_FOR_V204
    struct delayed_work   hi6521_dpm_check_work;
#endif
	struct work_struct    hi6521_irq_work;
    struct work_struct    usb_work;
	struct workqueue_struct *scharger_int_workqueue;
	struct semaphore      charger_data_busy_lock;
	unsigned int          wakelock_enabled;

	SOC_SCHARGER_STATUS0_UNION      charge_status_reg;
	SOC_SCHARGER_BUCK_REG3_UNION    buck_sysmin_sel_reg;
	SOC_SCHARGER_BUCK_REG4_UNION    input_source_reg;
	SOC_SCHARGER_ONFF0_UNION		power_on_config_reg00;
	SOC_SCHARGER_ONFF1_UNION        power_on_config_reg01;
	SOC_SCHARGER_CHG_REG0_UNION     charge_pre_current_vol_reg;
	SOC_SCHARGER_CHG_REG1_UNION     charge_fast_current_reg;
	SOC_SCHARGER_CHG_REG2_UNION     charge_fast_vol_term_cur_reg;
	SOC_SCHARGER_CHG_REG3_UNION		charge_pre_timer_rechg_timer_vol_reg;
	SOC_SCHARGER_CHG_REG4_UNION 	charge_fast_safe_timer_term_ctrl_reg;
	SOC_SCHARGER_IRQM0_UNION	    charge_irq_mask0_reg;
	SOC_SCHARGER_IRQM1_UNION	    charge_irq_mask1_reg;
	SOC_SCHARGER_IRQM2_UNION	    charge_irq_mask2_reg;
	SOC_SCHARGER_IRQM3_UNION	    charge_irq_mask3_reg;
	SOC_SCHARGER_IRQM4_UNION	    charge_irq_mask4_reg;
	SOC_SCHARGER_SYS_SET_REG_UNION  charge_set_chgwdt_timer_reg;
	SOC_SCHARGER_CTRL_REG_UNION     charge_chgwdt_rst_reg;
	SOC_SCHARGER_OTG_REG0_UNION     charge_otg_reg0;
	SOC_SCHARGER_REF_REG_UNION      charge_ref_reg;
    SOC_SCHARGER_FLASH_LED_REG4_UNION flash_led_reg4;
	SOC_SCHARGER_FLASH_LED_REG6_UNION flash_led_reg6;
    SOC_SCHARGER_FLASH_LED_REG7_UNION flash_led_reg7;

    SOC_SCHARGER_BUCK_REG5_UNION buck_reg5;

    unsigned short    bqchip_version;

	/*power_on_config_reg00*/
	unsigned char 	  flash_led_torch_en;
	unsigned char 	  flash_led_flash_en;
	unsigned char 	  flash_bst_en;
	unsigned char 	  lcd_bst_en;
	unsigned char 	  lcd_ldo_en;
	unsigned char 	  lcd_ncp_en;

	/*power_on_config_reg01*/
	unsigned char 	  chg_en;
	unsigned char 	  otg_en;
	unsigned char 	  wled_en;
	unsigned char 	  ldo1_200ma_en;
	unsigned char 	  ldo2_200ma_en;

	/*charge_fast_current_reg*/
	unsigned int    chg_fast_ichg;

	/*charge_pre_current_vol_reg*/
	unsigned int    chg_pre_ichg;
	unsigned int    chg_pre_vchg;
	unsigned char   batfet_ctrl;

	/*charge_fast_vol_term_cur_reg*/
	unsigned int    chg_fast_vchg;
	unsigned int    chg_term_ichg;

	/*charge_pre_timer_rechg_timer_vol_reg*/
	unsigned int 	chg_prechg_timer;
	unsigned int 	chg_rechg_time;
	unsigned int 	chg_vrechg_hys;

	/*charge_fast_safe_timer_term_ctrl_reg*/
	unsigned char	chg_termination_ctrl;
	unsigned int 	chg_fastchg_safe_timer;
	unsigned int	chg_vbatir_adj;

	/*charge_ref_reg*/
	unsigned int    chg_thsd_tstp_set;
	unsigned int    chg_thsd_treg_set;

	/*flash_led_timeout_reg*/
	unsigned int    chg_flash_led_timeout;
	unsigned int    chg_flash_torch_timeout;

	/*flash led timerout enable*/
	unsigned char    chg_flash_led_timeout_en;
	/*charge_chgwdt_rst_reg*/
	unsigned char   flash_led_wd_rst;
	unsigned char   wd_rst_n;
	unsigned char   soft_rst_n;

	/*charge_set_chgwdt_timer_reg*/
	unsigned int    watchdog_timer;

	/*otg reg0*/
	unsigned char   otg_force_en;

    unsigned int    max_currentmA;
    unsigned int    max_voltagemV;
    unsigned int    max_cin_currentmA;

	/*input_source_reg*/
    unsigned int    cin_dpmmV;
    unsigned int    cin_limit;
    unsigned int    sys_minmV;

    unsigned int    voltagemV;
    unsigned int    bat_compohm;
    unsigned int    comp_vclampmV;


    /*buck_reg5*/
    unsigned int    buck_dpm_mode;

    bool    		factory_flag;
    //bool    		hot_enable_charge_flag;
    bool    		calling_limit;
    bool    		battery_present;
	bool    		backup_battery_exist;
    //int     		charge_status;
    int     		charger_source;
    unsigned int    battery_temp_status;
    unsigned long           event;
    struct notifier_block   nb;

    int     battery_voltage;
    int     temperature_cold;
    int     temperature_cool;
    int     temperature_warm;
    int     temperature_hot;
    bool    not_limit_chrg_flag;
    bool    not_stop_chrg_flag;
    bool    battery_full;
    int     temperature_5;
    int     temperature_10;
    unsigned int  charge_full_count;

    /* these parameters are for charging between 0C-5C & 5C-10C, 1-0.1*capacity...
       charge_in_temp_5 means the parameter for charging between 0C-5C. */
    unsigned int design_capacity;
    unsigned int charge_in_temp_5;
    unsigned int charge_in_temp_10;

    int  japan_charger;
    int  is_two_stage_charger;
    int  two_stage_charger_status;
    int  first_stage_voltage;
    int  second_stage_voltage;
    int  is_disable_cool_temperature_charger;
    int  high_temp_para;
	int  irq;
	int  interrput_gpio;
	int  gpio_ce;
    int usb_shell_gpio;
    int usb_shell_gpio_valid;
};

struct hi6521_device_info *g_scharger_dev = NULL;
static unsigned long lcd_ldo_ocp_time = 0;
static unsigned long lcd_ldo_open_time = 0;
static void lcd_ldo_ocp_notify_prepare(void);
/*extern a notifier list for charging notification*/
extern struct blocking_notifier_head notifier_list;

extern struct bq27510_device_info *g_battery_measure_by_bq27510_device;

extern struct blocking_notifier_head reboot_notifier_list;

STATIC struct wake_lock chrg_lock;
STATIC struct wake_lock stop_chrg_lock;
STATIC u32 wakeup_timer_seconds;//h00121290 remove extern temply

/* backup the last cin_limit */
//STATIC unsigned int pre_cin_limit;
#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
STATIC unsigned int charge_temp_protected = 0;
#endif
STATIC unsigned int mhl_limit_cin_flag = 1;

STATIC unsigned int input_current_iin;
STATIC unsigned int input_current_ichg;
STATIC struct hi6521_device_info *scharger_di;

STATIC unsigned int dbg_current_iin = 0;
STATIC unsigned int dbg_current_ichg = 0;
STATIC unsigned int iin_temp = 0;
STATIC unsigned int ichg_temp = 0;

STATIC unsigned int cin_limit_temp = 1;
STATIC unsigned int charge_current_temp = 1;

STATIC u32 is_board_type = 0;

STATIC u8 s_scharger_version = 0;
STATIC u32 dpm_switch_enable = 0;
STATIC unsigned int vbus_ovp_flag = 0;

#ifdef CONFIG_CHARGER_TYPE_RECHECK
static int work_first_in_since_vbus_intr = 1;
#endif
enum{
    BATTERY_HEALTH_TEMPERATURE_NORMAL = 0,
    BATTERY_HEALTH_TEMPERATURE_OVERLOW,
    BATTERY_HEALTH_TEMPERATURE_LOW,
    BATTERY_HEALTH_TEMPERATURE_NORMAL_HIGH,
    BATTERY_HEALTH_TEMPERATURE_HIGH,
    BATTERY_HEALTH_TEMPERATURE_OVERHIGH,
    BATTERY_HEALTH_TEMPERATURE_10,
    BATTERY_HEALTH_TEMPERATURE_5,
    BATTERY_HEALTH_TEMPERATURE_HIGH_CP1,
    BATTERY_HEALTH_TEMPERATURE_HIGH_CP2,
    BATTERY_HEALTH_TEMPERATURE_HIGH_CP3,
    BATTERY_HEALTH_TEMPERATURE_HIGH_CP4,
    BATTERY_HEALTH_TEMPERATURE_HIGH_CP5,
};
enum{
    NORMAL_TEMP_CONFIG = 0,  // (BATTERY_HEALTH_TEMPERATURE_NORMAL)
    NORMAL_HIGH_TEMP_CONFIG, // (BATTERY_HEALTH_TEMPERATURE_NORMAL_HIGH)
    HIGH_TEMP_CONFIG,        // (BATTERY_HEALTH_TEMPERATURE_HIGH)
    TEMP_CONFIG_10,
    TEMP_CONFIG_5,
    NORMAL_SECOND_STAGE,//for two charging stage
    NORMAL_HIGH_TEMP_CONFIG_CP1,
    NORMAL_HIGH_TEMP_CONFIG_CP2,
    NORMAL_HIGH_TEMP_CONFIG_CP3,
    NORMAL_HIGH_TEMP_CONFIG_CP4,
};

//tempture checkpoint
struct hi6521_high_temp_cp{
    int cp1;
    int cp2;
    int cp3;
    int cp4;
    int cp5;
    int cp6;
};

enum{
    HIGH_TEMP_CP_U9701L = 0,
    HIGH_TEMP_CP_U9700L = 1,
};

STATIC struct hi6521_high_temp_cp japan_temp_cp[] ={
    {40, 42, 43, 45, 53, 55}, //HIGH_TEMP_CP_U9701L
    {37, 39, 40, 42, 43, 45},//HIGH_TEMP_CP_U9700L
    {39, 41, 42, 45, 53, 55},//HIGH_TEMP_CP_U9700GVC China area
    {0},
};

#define CHG_REG_SETBITS(reg_val, shift, mask, set_val) \
                          ((reg_val  = (reg_val & (~((((unsigned int)1 << (mask)) - 1) << (shift)))) \
                           | ((unsigned int)((set_val) & (((unsigned int)1 << (mask)) - 1)) << (shift))))

#define CHG_REG_GETBITS(reg_val, shift, mask) \
                          (((reg_val >> (shift)) \
                           &(((unsigned int)1 << (mask)) - 1)))

extern struct class *hw_power_get_class(void);
#define CHARGER_DSM(error_found,dsm_info,func,reg_value) do{ \
        if (error_found >= 0){\
            dsm_client_record(charger_dclient, dsm_info,\
				func, reg_value);\
            error_found++;\
        }\
}while(0)

STATIC void hi6521_get_boardid_japan_charge_parameter(struct hi6521_device_info *di)
{
	int ret;
    struct device_node* np;

    np = di->dev->of_node;
    ret = of_property_read_u32(np,"japan_charger",&di->japan_charger);
    ret |= of_property_read_u32(np,"is_two_stage_charger",&di->is_two_stage_charger);
    ret |= of_property_read_u32(np,"first_stage_voltage",&di->first_stage_voltage);
    ret |= of_property_read_u32(np,"second_stage_voltage",&di->second_stage_voltage);
    ret |= of_property_read_u32(np,"is_disable_cool_temperature_charger",&di->is_disable_cool_temperature_charger);
    ret |= of_property_read_u32(np,"high_temperature_checkpoint",&di->high_temp_para);
     if(ret)
        hwlog_err( " hi6521 get japan charge parameter from boardid dts fail \n");

     hwlog_info("%s, \n"
                         "japan_charger = %d \n"
                         "is_two_stage_charger = %d\n"
                         "first_stage_voltage = %d \n"
                         "second_stage_voltage = %d \n"
                         "is_disable_cool_temperature_charger = %d \n"
                         "high_temp_para = %d \n"
                         , __func__, di->japan_charger, di->is_two_stage_charger,di->first_stage_voltage,
                         di->second_stage_voltage, di->is_disable_cool_temperature_charger,di->high_temp_para);
     hwlog_info("\ncp1 = %d \n"
                         "cp2 = %d \n"
                         "cp3 = %d \n"
                         "cp4 = %d \n"
                         "cp5 = %d \n"
                         "cp6 = %d \n"
                         ,japan_temp_cp[di->high_temp_para].cp1,japan_temp_cp[di->high_temp_para].cp2,
                         japan_temp_cp[di->high_temp_para].cp3,japan_temp_cp[di->high_temp_para].cp4,
                         japan_temp_cp[di->high_temp_para].cp5,japan_temp_cp[di->high_temp_para].cp6);
     return;
}


STATIC int hi6521_get_boardid_charge_parameter(struct hi6521_device_info *di)
{
    bool ret = 0;

	struct battery_charge_param_s charge_param = {0};
	ret = hisi_battery_charge_param(&charge_param);
	if (ret){
		if (CHG_V210 == s_scharger_version){
			di->max_voltagemV = charge_param.max_voltagemV - 50;/*scharger(210/211/212) need sub 50mv*/
		}
		else{
			di->max_voltagemV = charge_param.max_voltagemV;
		}
	    di->max_currentmA = charge_param.max_currentmA;
	    di->max_cin_currentmA = charge_param.max_cin_currentmA;
        di->design_capacity = charge_param.design_capacity;
        di->charge_in_temp_5 = charge_param.charge_in_temp_5;
        di->charge_in_temp_10 = charge_param.charge_in_temp_10;
	    hwlog_info("%s: capacity is %d, charge_in_temp_5 is %d,"
           "charge_in_temp_10 is %d.\n",__FUNCTION__,di->design_capacity,
           di->charge_in_temp_5,di->charge_in_temp_10);
		return true;
	}
    else{
        hwlog_err( " hi6521 get charge parameter from boardid fail \n");
        di->max_voltagemV = 4208;
        di->max_currentmA = 1000;
        di->max_cin_currentmA = 1000;
	    /* Configuration parameters for 0C-10C, set the default to the parameters,
       	and get the values from boardid files */
	    di->design_capacity   = HI6521_DEFAULT_CAPACITY;
	    di->charge_in_temp_5  = DEFAULT_CHARGE_PARAM_LOW_TEMP;
	    di->charge_in_temp_10 = DEFAULT_CHARGE_PARAM_LOW_TEMP;
	    hwlog_info("%s: capacity is %d, charge_in_temp_5 is %d,"
           "charge_in_temp_10 is %d.\n",__FUNCTION__,di->design_capacity,
           di->charge_in_temp_5,di->charge_in_temp_10);
        return false;
    }

}


STATIC int hi6521_get_max_charge_voltage(struct hi6521_device_info *di)
{
    bool ret = 0;

    struct battery_charge_param_s charge_param = {0};
    ret = hisi_battery_charge_param(&charge_param);
    if (ret){
		if (CHG_V210 == s_scharger_version){
			di->max_voltagemV = charge_param.max_voltagemV - 50;
		}
		else{
			di->max_voltagemV = charge_param.max_voltagemV;
		}
		return true;
    }
	else{
        di->max_voltagemV = 4208;
        hwlog_err( "boardid does not set limited charge voltage\n");
        return false;
    }
}

STATIC int hi6521_write_block(struct hi6521_device_info *di, u8 *value,
                               u8 reg, unsigned num_bytes)
{
    struct i2c_msg msg[1];
    int ret = 0;

    *value = reg;

    msg[0].addr = di->client->addr;
    msg[0].flags = 0;
    msg[0].buf = value;
    msg[0].len = num_bytes + 1;

   ret = i2c_transfer(di->client->adapter, msg, 1);

    /* i2c_transfer returns number of messages transferred */
    if (ret != 1) {
        hwlog_err(
                "i2c_write failed to transfer all messages\n");
        if (ret < 0)
            return ret;
        else
            return -EIO;
     } else {
        return 0;
   }
}

STATIC int hi6521_read_block(struct hi6521_device_info *di, u8 *value,
                            u8 reg, unsigned num_bytes)
{
    struct i2c_msg msg[2];
    u8 buf = 0;
    int ret = 0;

    buf = reg;

    msg[0].addr = di->client->addr;
    msg[0].flags = 0;
    msg[0].buf = &buf;
    msg[0].len = 1;

    msg[1].addr = di->client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = value;
    msg[1].len = num_bytes;

    ret = i2c_transfer(di->client->adapter, msg, 2);

    /* i2c_transfer returns number of messages transferred */
    if (ret != 2) {
         hwlog_err(
              "i2c_read failed to transfer all messages\n");
        if (ret < 0)
            return ret;
        else
            return -EIO;
    } else {
        return 0;
    }
}

STATIC int hi6521_write_byte(struct hi6521_device_info *di, u8 value, u8 reg)
{
    /* 2 bytes offset 1 contains the data offset 0 is used by i2c_write */
    u8 temp_buffer[2] = { 0 };

    /* offset 1 contains the data */
    temp_buffer[1] = value;
    return hi6521_write_block(di, temp_buffer, reg, 1);
}

STATIC int hi6521_read_byte(struct hi6521_device_info *di, u8 *value, u8 reg)
{
    return hi6521_read_block(di, value, reg, 1);
}

int scharger_write_regs(u8 value, u8 reg)
{
    u8 temp_buffer[2] = { 0 };

    if (NULL == scharger_di){
        return -1;
    }

    /* offset 1 contains the data */
    temp_buffer[1] = value;
    return hi6521_write_block(scharger_di, temp_buffer, reg, 1);

}
EXPORT_SYMBOL_GPL(scharger_write_regs);

int scharger_read_regs(u8 *value, u8 reg)
{
    if (NULL == scharger_di){
        return -1;
    }
    return hi6521_read_block(scharger_di, value, reg, 1);
}
EXPORT_SYMBOL_GPL(scharger_read_regs);

/****************************************************************************
  Function:     hi6521_config_input_source_reg
  Description:  onfig BUCK_REG4 to setup input current limit  and input dpm voltage
  Input:        struct hi6521_device_info
  Output:       NA
  Return:       NA
****************************************************************************/

STATIC void hi6521_config_input_source_reg(struct hi6521_device_info *di)
{
      unsigned int vindpm     = 0;
      unsigned char Vdpm      = 0;
      unsigned char Iin_limit = 0;

      vindpm = di->cin_dpmmV;

      if(vindpm < VINDPM_MIN_3880)
          vindpm = VINDPM_MIN_3880;
      else if (vindpm > VINDPM_MAX_5080)
          vindpm = VINDPM_MAX_5080;
	  Vdpm = (unsigned char)((vindpm -VINDPM_MIN_3880)/VINDPM_STEP_80);

	  if (dbg_current_iin){
		  di->cin_limit = dbg_current_iin;
	  }

      if (di->cin_limit <= IINLIM_100)
          Iin_limit = 0;
      else if (di->cin_limit > IINLIM_100 && di->cin_limit <= IINLIM_150)
          Iin_limit = 1;
      else if (di->cin_limit > IINLIM_150 && di->cin_limit <= IINLIM_500)
          Iin_limit = 2;
      else if (di->cin_limit > IINLIM_500 && di->cin_limit <= IINLIM_900)
          Iin_limit = 3;
	  else if (di->cin_limit > IINLIM_900 && di->cin_limit <= IINLIM_1000)
		  Iin_limit = 4;
      else if (di->cin_limit > IINLIM_1000 && di->cin_limit <= IINLIM_1200)
          Iin_limit = 5;
      else if (di->cin_limit > IINLIM_1200 && di->cin_limit <= IINLIM_1500)
          Iin_limit = 6;
      else if (di->cin_limit > IINLIM_1500 && di->cin_limit <= IINLIM_1600)
          Iin_limit = 7;
      else if (di->cin_limit > IINLIM_1600 && di->cin_limit <= IINLIM_1800)
          Iin_limit = 8;
      else if (di->cin_limit > IINLIM_1800 && di->cin_limit <= IINLIM_2000)
          Iin_limit = 9;
      else if (di->cin_limit > IINLIM_2000 && di->cin_limit <= IINLIM_2200)
          Iin_limit = 10;
	  else if (di->cin_limit > IINLIM_2200 && di->cin_limit <= IINLIM_2500)
          Iin_limit = 11;
      else
          Iin_limit = 4;

      di->input_source_reg.reg.buck_int_lim_sel = Iin_limit;
	  di->input_source_reg.reg.buck_dpm_sel     = Vdpm;

      hi6521_write_byte(di, di->input_source_reg.value, INPUT_SOURCE_REG);
      return;
}

/****************************************************************************
  Function:     hi6521_config_sys_min_vol_reg
  Description:  onfig BUCK_REG3 to setup buck_sysmin_sel and buck_dmd_offset
  Input:        struct hi6521_device_info
  Output:       NA
  Return:       NA
****************************************************************************/
STATIC void hi6521_config_sys_min_vol_reg(struct hi6521_device_info *di)
{
	unsigned int  sys_min = 0;
	unsigned char Sysmin  = 0;

	sys_min = di->sys_minmV;
	if(sys_min < SYS_MIN_MIN_3550)
	  sys_min = SYS_MIN_MIN_3550;
	else if (sys_min > SYS_MIN_MAX_3850)
	  sys_min = SYS_MIN_MAX_3850;
	Sysmin = (unsigned char)((sys_min -SYS_MIN_MIN_3550)/SYS_MIN_STEP_100);
	di->buck_sysmin_sel_reg.reg.buck_sysmin_sel = Sysmin;
	di->buck_sysmin_sel_reg.reg.buck_dmd_offset = BUCK_DMD_OFFSET;
	hi6521_write_byte(di, di->buck_sysmin_sel_reg.value, VSYS_MIN_CONFIG_REG);

}

/****************************************************************************
  Function:     hi6521_config_power_on_reg01
  Description:  config onff1 to disable/enable chg/otg/wled/ldo_200ma.
  Input:        struct hi6521_device_info
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int hi6521_config_power_on_reg01(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_power_on_reg01 sema fail! \n");
		return -EINTR;
	}
	di->power_on_config_reg01.reg.chg_en_int        = di->chg_en;
	di->power_on_config_reg01.reg.otg_en_int        = di->otg_en;
	di->power_on_config_reg01.reg.wled_en_int       = di->wled_en;
	di->power_on_config_reg01.reg.ldo1_200ma_en_int = di->ldo1_200ma_en;
	di->power_on_config_reg01.reg.ldo2_200ma_en_int = di->ldo2_200ma_en;

	ret = hi6521_write_byte(di, di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_charger_enable
  Description:  config chg enable/disable.
  Input:        struct hi6521_device_info,di->chg_en need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_charger_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_charger_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->power_on_config_reg01.value,POWER_ON_CONFIG_REG01);
	di->power_on_config_reg01.reg.chg_en_int = di->chg_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_charger_enable_val
  Description:  config chg enable/disable.
  Input:        struct hi6521_device_info,di->chg_en need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_charger_enable_val(struct hi6521_device_info *di,int val)
{
    int ret = 0;
    if (down_interruptible(&di->charger_data_busy_lock)){
        hwlog_err("hi6521_config_charger_enable sema fail! \n");
        return -EINTR;
    }
    di->chg_en=(unsigned char)val;
    ret = hi6521_read_byte(di,&di->power_on_config_reg01.value,POWER_ON_CONFIG_REG01);
    di->power_on_config_reg01.reg.chg_en_int = di->chg_en;
    ret |= hi6521_write_byte(di,di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);
    up(&di->charger_data_busy_lock);
    return ret;
}

/****************************************************************************
  Function:     hi6521_config_charger_nolock_enable
  Description:  config chg enable/disable.
  Input:        struct hi6521_device_info,di->chg_en need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_charger_nolock_enable(struct hi6521_device_info *di)
{

  int ret = 0;
  ret = hi6521_read_byte(di,&di->power_on_config_reg01.value,POWER_ON_CONFIG_REG01);
  di->power_on_config_reg01.reg.chg_en_int = di->chg_en;
  ret |= hi6521_write_byte(di,di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);
  return ret;
}
/****************************************************************************
  Function:     hi6521_config_otg_enable
  Description:  config otg enable/disable.
  Input:        struct hi6521_device_info,di->otg_en and di->otg_force_en need setup
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_otg_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_otg_enable sema fail! \n");
		return -EINTR;
	}
	/*config otg en*/
	ret = hi6521_read_byte(di,&di->power_on_config_reg01.value,POWER_ON_CONFIG_REG01);
	di->power_on_config_reg01.reg.otg_en_int = di->otg_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);
	/*config otg force en*/
	/*ret = hi6521_read_byte(di,&di->charge_otg_reg0.value,CHG_OTG_REG0);
	di->charge_otg_reg0.reg.otg_force_en_int = di->otg_force_en;
	ret |= hi6521_write_byte(di,di->charge_otg_reg0.value, CHG_OTG_REG0);*/
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_wled_enable
  Description:  config wled enable/disable.
  Input:        struct hi6521_device_info,di->wled need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
static int  hi6521_config_wled_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_wled_enable sema fail! \n");
		return -EINTR;
	}
	/*opt para for temporary*/
#if 0
	hi6521_write_byte(di, 0x7C, 0x15);
	hi6521_write_byte(di, 0x97, 0x19);
#endif
	ret = hi6521_read_byte(di,&di->power_on_config_reg01.value,POWER_ON_CONFIG_REG01);
	di->power_on_config_reg01.reg.wled_en_int = di->wled_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);

	/*opt para for temporary*/
#if 0
	mdelay(20);
	hi6521_write_byte(di, 0x7D, 0x15);
	hi6521_write_byte(di, 0x9F, 0x19);
#endif
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_ldo1_200ma_enable
  Description:  config ldo1_200ma_en enable/disable,NFC use.
  Input:        struct hi6521_device_info,di->ldo1_200ma_en need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_ldo1_200ma_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_ldo1_200ma_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->power_on_config_reg01.value,POWER_ON_CONFIG_REG01);
	di->power_on_config_reg01.reg.ldo1_200ma_en_int = di->ldo1_200ma_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_ldo2_200ma_enable
  Description:  config ldo1_200ma_en enable/disable,NFC use.
  Input:        struct hi6521_device_info,di->ldo1_200ma_en need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_ldo2_200ma_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_ldo2_200ma_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->power_on_config_reg01.value,POWER_ON_CONFIG_REG01);
	di->power_on_config_reg01.reg.ldo2_200ma_en_int = di->ldo2_200ma_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_lcd_boost_enable
  Description:  config lcd_bst enable/disable.
  Input:        struct hi6521_device_info,di->lcd_bst_en need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_lcd_boost_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_lcd_boost_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->power_on_config_reg00.value,POWER_ON_CONFIG_REG00);
	di->power_on_config_reg00.reg.lcd_bst_en_int = di->lcd_bst_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg00.value, POWER_ON_CONFIG_REG00);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_lcd_ldo_enable
  Description:  config lcd_ldo enable/disable,LCD+5v.
  Input:        struct hi6521_device_info,di->lcd_ldo need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_lcd_ldo_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_lcd_ldo_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->power_on_config_reg00.value,POWER_ON_CONFIG_REG00);
	di->power_on_config_reg00.reg.lcd_ldo_en_int = di->lcd_ldo_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg00.value, POWER_ON_CONFIG_REG00);
    lcd_ldo_open_time = get_jiffies_64();
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_lcd_ncp_enable
  Description:  config lcd_ncp enable/disable,LCD-5V.
  Input:        struct hi6521_device_info:di->lcd_ncp need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_lcd_ncp_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_lcd_ncp_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->power_on_config_reg00.value,POWER_ON_CONFIG_REG00);
	di->power_on_config_reg00.reg.lcd_ncp_en_int = di->lcd_ncp_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg00.value, POWER_ON_CONFIG_REG00);
	up(&di->charger_data_busy_lock);
	return ret;
}
/****************************************************************************
  Function:     hi6521_config_lcd_ldo_ncp_enable
  Description:  config lcd_ldo_ncp enable/disable,LCD+5v-5v simultaneouly.
  Input:        struct hi6521_device_info,di->lcd_ldo & di->lcd_ncp need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_lcd_ldo_ncp_enable(struct hi6521_device_info *di)
{
    int ret = 0;
    if (down_interruptible(&di->charger_data_busy_lock)){
        hwlog_err("hi6521_config_lcd_ldo_enable sema fail! \n");
        return -EINTR;
    }
    ret = hi6521_read_byte(di,&di->power_on_config_reg00.value,POWER_ON_CONFIG_REG00);
    di->power_on_config_reg00.reg.lcd_ldo_en_int = di->lcd_ldo_en;
    di->power_on_config_reg00.reg.lcd_ncp_en_int = di->lcd_ncp_en;
    ret |= hi6521_write_byte(di,di->power_on_config_reg00.value, POWER_ON_CONFIG_REG00);
    up(&di->charger_data_busy_lock);
    return ret;
}

/****************************************************************************
  Function:     hi6521_config_flash_led_timeout_enable
  Description:  config flash_led_torch current enable/disable.
  Input:        struct hi6521_device_info:di->di->flash_led_flash_en need setup.
                lightness
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_flash_led_timeout_enable(struct hi6521_device_info *di)
{
	int ret = 0;

	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_flash_led_timeout_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->flash_led_reg7.value, FLASH_LED_REG7_REG);
	di->flash_led_reg7.reg.flash_led_timeout_en = di->chg_flash_led_timeout_en;
	ret |= hi6521_write_byte(di,di->flash_led_reg7.value, FLASH_LED_REG7_REG);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_flash_led_timeout
  Description:  config flash_led_timeout.
  Input:        struct hi6521_device_info:di->chg_flash_led_timeout need setup.
                lightness
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_flash_led_timeout(struct hi6521_device_info *di)
{
	int ret = 0;
	unsigned int flash_led_timeout = 0;
	unsigned char tout = 0;

	flash_led_timeout = di->chg_flash_led_timeout;
    if(flash_led_timeout <= FLASH_LED_TIMEOUT_400)
        tout = 0;
    else if ((flash_led_timeout > FLASH_LED_TIMEOUT_400) && (flash_led_timeout <= FLASH_LED_TIMEOUT_600))
        tout = 0x1;
	else if ((flash_led_timeout > FLASH_LED_TIMEOUT_600) && (flash_led_timeout <= FLASH_LED_TIMEOUT_800))
        tout = 0x2;
    else
		tout = 0x3;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_flash_led_timeout sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->flash_led_reg6.value, FLASH_LED_REG6_REG);
	di->flash_led_reg6.reg.flash_led_timeout = tout;
	ret |= hi6521_write_byte(di,di->flash_led_reg6.value, FLASH_LED_REG6_REG);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_flash_torch_timeout
  Description:  config flash_torch timeout.
  Input:        struct hi6521_device_info:di->chg_flash_torch_timeout need setup.
                lightness
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_flash_torch_timeout(struct hi6521_device_info *di)
{
	int ret = 0;
	unsigned int flash_torch_timeout = 0;
	unsigned char tout = 0;

	flash_torch_timeout = di->chg_flash_torch_timeout;
	/*config torch timeout*/
    if (flash_torch_timeout < CHG_FLASH_TORCH_TIMOUT_MIN)
        flash_torch_timeout = CHG_FLASH_TORCH_TIMOUT_MIN;
    else if(flash_torch_timeout > CHG_FLASH_TORCH_TIMOUT_MAX)
        flash_torch_timeout = CHG_FLASH_TORCH_TIMOUT_MAX;
    tout = (unsigned char)((flash_torch_timeout - CHG_FLASH_TORCH_TIMOUT_MIN)/CHG_FLASH_TORCH_TIMOUT_STEP_2);

	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_flash_torch_timeout sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->flash_led_reg6.value, FLASH_LED_REG6_REG);
	di->flash_led_reg6.reg.flash_led_watchdog_timer = tout;
	ret |= hi6521_write_byte(di,di->flash_led_reg6.value, FLASH_LED_REG6_REG);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_flash_boost_enable
  Description:  config flash boost enable/disable.
  Input:        struct hi6521_device_info:di di->flash_bst_en need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_flash_boost_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_flash_boost_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->power_on_config_reg00.value,POWER_ON_CONFIG_REG00);
	di->power_on_config_reg00.reg.flash_bst_en_int = di->flash_bst_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg00.value, POWER_ON_CONFIG_REG00);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     hi6521_config_flash_led_flash_enable
  Description:  config flash_led_flash enable/disable.
  Input:        struct hi6521_device_info:di->di->flash_led_flash_en need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_flash_led_flash_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_flash_led_flash_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->power_on_config_reg00.value,POWER_ON_CONFIG_REG00);
	di->power_on_config_reg00.reg.flash_led_flash_en = di->flash_led_flash_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg00.value, POWER_ON_CONFIG_REG00);
	up(&di->charger_data_busy_lock);
	return ret;
}
/****************************************************************************
  Function:     hi6521_config_flash_led_torch_enable
  Description:  config falsh_led_torch enable/disable.
  Input:        struct hi6521_device_info:di->di->flash_led_torch_en need setup.
  Output:       NA
  Return:       0:success others:fail
****************************************************************************/
STATIC int  hi6521_config_flash_led_torch_enable(struct hi6521_device_info *di)
{
	int ret = 0;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("hi6521_config_flash_led_torch_enable sema fail! \n");
		return -EINTR;
	}
	ret = hi6521_read_byte(di,&di->power_on_config_reg00.value,POWER_ON_CONFIG_REG00);
	di->power_on_config_reg00.reg.flash_led_torch_en = di->flash_led_torch_en;
	ret |= hi6521_write_byte(di,di->power_on_config_reg00.value, POWER_ON_CONFIG_REG00);
	up(&di->charger_data_busy_lock);
	return ret;
}
/****************************************************************************
  Function:     hi6521_config_fast_current_reg
  Description:  config fast charger current limit.
  Input:        struct hi6521_device_info:di->chg_fast_ichg
  Output:       NA
  Return:       NA
****************************************************************************/
STATIC void hi6521_config_fast_current_reg(struct hi6521_device_info *di)
{
    unsigned int fast_currentmA = 0;
    unsigned char Vichrg = 0;

    fast_currentmA = di->chg_fast_ichg;

	if (dbg_current_ichg){
		fast_currentmA = dbg_current_ichg;
	}

    if (fast_currentmA < CHG_FAST_ICHG_MIN)
        fast_currentmA = CHG_FAST_ICHG_MIN;
    else if(fast_currentmA > CHG_FAST_ICHG_MAX)
        fast_currentmA = CHG_FAST_ICHG_MAX;
    Vichrg = (unsigned char)((fast_currentmA - CHG_FAST_ICHG_MIN)/CHG_FAST_ICHG_STEP_100);
    di->charge_fast_current_reg.reg.chg_fast_ichg = Vichrg;

	hi6521_write_byte(di, di->charge_fast_current_reg.value, CHG_FAST_CURRENT_REG);
	return;
}

/****************************************************************************
  Function:     hi6521_config_fast_vol_and_term_current_reg
  Description:  config fast charge vol threhold and term current threhold.
  Input:        struct hi6521_device_info: di->chg_fast_vchg/di->chg_term_ichg.
  Output:       NA
  Return:       NA
****************************************************************************/
STATIC void hi6521_config_fast_vol_and_term_current_reg(struct hi6521_device_info *di)
{
    unsigned int fast_voltagemv = 0;
	unsigned int term_currentma = 0;
    unsigned char vfast = 0;
	unsigned char iterm = 0;
	/*config fast charge vol*/
    fast_voltagemv = di->chg_fast_vchg;
	if (fast_voltagemv < CHG_FAST_VCHG_MIN)
	    fast_voltagemv = CHG_FAST_VCHG_MIN;
	else if(fast_voltagemv > CHG_FAST_VCHG_MAX)
    	fast_voltagemv = CHG_FAST_VCHG_MAX;
	vfast = (unsigned char)((fast_voltagemv - CHG_FAST_VCHG_MIN)/CHG_FAST_VCHG_STEP_50);
	di->charge_fast_vol_term_cur_reg.reg.chg_fast_vchg = vfast;
	/*config term current threhold*/
    term_currentma = di->chg_term_ichg;
    if (term_currentma < CHG_TERM_ICHG_MIN)
        term_currentma = CHG_TERM_ICHG_MIN;
    else if(term_currentma > CHG_TERM_ICHG_MAX)
        term_currentma = CHG_TERM_ICHG_MAX;
    iterm = (unsigned char)((term_currentma - CHG_TERM_ICHG_MIN)/CHG_TERM_ICHG_STEP_50);
    di->charge_fast_vol_term_cur_reg.reg.chg_term_ichg = iterm;

	hi6521_write_byte(di, di->charge_fast_vol_term_cur_reg.value, CHG_FAST_VOL_TERM_CUR_REG);
	return;
}
/****************************************************************************
  Function:     hi6521_config_prechg_current_vot_reg
  Description:  config prechg charger current limit and voltage threshold ,control batlet diable/enbale,
  Input:        struct hi6521_device_info :di->chg_pre_ichg,di->chg_pre_vchg,di->batfet_ctrl
  Output:       NA
  Return:       NA
****************************************************************************/
STATIC void hi6521_config_prechg_current_vot_and_batfet_ctrl_reg(struct hi6521_device_info *di)
{
    unsigned int precurrentmA  = 0;
    unsigned int prevoltage    = 0;
	unsigned char batfetCtrl   = 0;
    unsigned char iprechg      = 0;
    unsigned char vprechg      = 0;

    precurrentmA = di->chg_pre_ichg;
	prevoltage   = di->chg_pre_vchg;
    batfetCtrl   = di->batfet_ctrl;
	/*config prechg current */
    if(precurrentmA < CHG_PRG_ICHG_MIN)
		precurrentmA = CHG_PRG_ICHG_MIN;
	if (precurrentmA > CHG_PRG_ICHG_MAX)
		precurrentmA = CHG_PRG_ICHG_MAX;
	iprechg = (unsigned char)((precurrentmA - CHG_PRG_ICHG_MIN )/CHG_PRG_ICHG_STEP_100);

	/*config prechg vol */
    if(prevoltage <= CHG_PRE_VCHG_MIN)
		vprechg = 0;
	else if ((prevoltage > CHG_PRE_VCHG_MIN) && (prevoltage <= CHG_PRE_VCHG_3000MV))
		vprechg = 0x1;
	else if  ((prevoltage > CHG_PRE_VCHG_3000MV) && (prevoltage <= CHG_PRE_VCHG_3100MV))
		vprechg = 0x2;
	else
		vprechg = 0x3;

    di->charge_pre_current_vol_reg.reg.chg_pre_vchg = vprechg;
	di->charge_pre_current_vol_reg.reg.chg_pre_ichg = iprechg;
	di->charge_pre_current_vol_reg.reg.batfet_ctrl  = batfetCtrl;
    hi6521_write_byte(di, di->charge_pre_current_vol_reg.value, CHG_PRE_VOL_CURRENT_BATFET_REG);
    return;
}


/****************************************************************************
  Function:     hi6521_config_prechg_timer_and_rechg_vol_timer_reg
  Description:  config prechg timer and recharge voltage/timer.
  Input:        struct hi6521_device_info:di->chg_prechg_timer,di->chg_rechg_time,di->chg_vrechg_hys
  Output:       NA
  Return:       NA
****************************************************************************/
STATIC void hi6521_config_prechg_timer_and_rechg_vol_timer_reg(struct hi6521_device_info *di)
{
    unsigned int prechgtimer = 0;
    unsigned int rechgtimer  = 0;
	unsigned int rechgvol    = 0;
    unsigned char tpre       = 0;
    unsigned char vre        = 0;
	unsigned char tre        = 0;

	prechgtimer = di->chg_prechg_timer;
	rechgtimer  = di->chg_rechg_time;
	rechgvol    = di->chg_vrechg_hys;
	/*config pre charge timer*/
    if(prechgtimer < CHG_PRE_TIMER_MIN)
        prechgtimer = CHG_PRE_TIMER_MIN;
    if(prechgtimer > CHG_PRE_TIMER_MAX)
        prechgtimer = CHG_PRE_TIMER_MAX;
	tpre = (unsigned char)((prechgtimer - CHG_PRE_TIMER_MIN)/CHG_PRE_TIMER_STEP_15MIN);
	/*config re charge time*/
    if(rechgtimer <= CHG_RECHG_TIMER_MIN)
        tre = 0;
    else if ((rechgtimer > CHG_RECHG_TIMER_MIN) && (rechgtimer <= CHG_RECHG_TIMER_1000MS))
        tre = 0x1;
	else if ((rechgtimer > CHG_RECHG_TIMER_1000MS) && (rechgtimer <= CHG_RECHG_TIMER_2000MS))
        tre = 0x2;
    else
		tre = 0x3;
	/*config re charge vol*/
    if(rechgvol < CHG_RECHG_VOL_HYS_MIN)
        rechgvol = CHG_RECHG_VOL_HYS_MIN;
    if(rechgvol > CHG_RECHG_VOL_HYS_MAX)
        rechgvol = CHG_RECHG_VOL_HYS_MAX;
	vre = (unsigned char)((rechgvol - CHG_RECHG_VOL_HYS_MIN)/CHG_RECHG_VOL_STEP_50MV);

    di->charge_pre_timer_rechg_timer_vol_reg.reg.chg_prechg_timer = tpre;
	di->charge_pre_timer_rechg_timer_vol_reg.reg.chg_rechg_time   = tre;
	di->charge_pre_timer_rechg_timer_vol_reg.reg.chg_vrechg_hys   = vre;
    hi6521_write_byte(di, di->charge_pre_timer_rechg_timer_vol_reg.value, CHG_PRE_TIMER_RECHG_VOL_TIMER_REG);
    return;
}

/****************************************************************************
  Function:     hi6521_config_fast_safe_timer_and_term_ctrl_reg
  Description:  config fast safe timer and charge termination enable/disable ctrl and vbatir adj.
  Input:        struct hi6521_device_info:di->chg_fastchg_safe_timer,di->chg_vbatir_adj,di->chg_termination_ctrl
  Output:       NA
  Return:       NA
****************************************************************************/
STATIC void hi6521_config_fast_safe_timer_and_term_ctrl_reg(struct hi6521_device_info *di)
{
    unsigned int fasttimer   = 0;
    unsigned int vbatir      = 0;
	unsigned char termctrl   = 0;
    unsigned char tfast      = 0;
    unsigned char vbat       = 0;

	fasttimer = di->chg_fastchg_safe_timer;
	vbatir    = di->chg_vbatir_adj;
	termctrl  = di->chg_termination_ctrl;

	/*config fast charge safe timer*/
    if(fasttimer <= CHG_FAST_SAFE_TIMER_MIN)
        tfast = 0;
    else if ((fasttimer > CHG_FAST_SAFE_TIMER_MIN) && (fasttimer <= CHG_FAST_SAFE_TIMER_8H))
        tfast = 0x1;
	else if ((fasttimer > CHG_FAST_SAFE_TIMER_8H) && (fasttimer <= CHG_FAST_SAFE_TIMER_12H))
        tfast = 0x2;
    else
		tfast = 0x3;
	/*config vbat ir*/
    if(vbatir < CHG_VBATIR_ADJ_MIN)
        vbatir = CHG_VBATIR_ADJ_MIN;
    if(vbatir > CHG_VBATIR_ADJ_MAX)
        vbatir = CHG_VBATIR_ADJ_MAX;
	vbat = (unsigned char)((vbatir - CHG_VBATIR_ADJ_MIN)/CHG_VBATIR_ADJ_STEP_10MV);

    di->charge_fast_safe_timer_term_ctrl_reg.reg.chg_fastchg_safe_timer = tfast;
	di->charge_fast_safe_timer_term_ctrl_reg.reg.chg_termination_ctrl   = termctrl;
	di->charge_fast_safe_timer_term_ctrl_reg.reg.chg_vbatir_adj         = vbat;
    hi6521_write_byte(di, di->charge_fast_safe_timer_term_ctrl_reg.value, CHG_FAST_SAFE_TIMER_TERM_CTRL_REG);
    return;
}

/****************************************************************************
  Function:     hi6521_config_chgwdt_soft_torchwdt_rst_reg
  Description:  config ctrl reg to restart chgwdt/torchwdt/charge register .
  Input:        struct hi6521_device_info:di->wd_rst_n,di->soft_rst_n,di->flash_led_wd_rst.
  Output:       NA
  Return:       NA
***************************************************************************/
STATIC void hi6521_config_chgwdt_soft_torchwdt_rst_reg(struct hi6521_device_info *di)
{
	di->charge_chgwdt_rst_reg.reg.wd_rst_n         = di->wd_rst_n;
	di->charge_chgwdt_rst_reg.reg.soft_rst_n       = di->soft_rst_n;
	di->charge_chgwdt_rst_reg.reg.flash_led_wd_rst = di->flash_led_wd_rst;

	hi6521_write_byte(di, di->charge_chgwdt_rst_reg.value, CHG_WDT_RST_REG);
	return;

}

/****************************************************************************
  Function:     hi6521_config_chgwdt_timer_reg
  Description:  config chg wdt timer .
  Input:        struct hi6521_device_info:di->watchdog_timer.
  Output:       NA
  Return:       NA
***************************************************************************/
STATIC void hi6521_config_chgwdt_timer_reg(struct hi6521_device_info *di)
{
	di->charge_set_chgwdt_timer_reg.reg.watchdog_timer = di->watchdog_timer;

	hi6521_write_byte(di, di->charge_set_chgwdt_timer_reg.value, CHG_WDT_TIMER_REG);
	return;

}

/****************************************************************************
  Function:     hi6521_get_chgwdt_status
  Description:  get chg wdt status
  Input:        struct hi6521_device_info:di->charge_set_chgwdt_timer_reg.
  Output:       NA
  Return:       NA
***************************************************************************/
STATIC unsigned char hi6521_get_chgwdt_status(struct hi6521_device_info *di)
{
	hi6521_read_byte(di, &di->charge_set_chgwdt_timer_reg.value, CHG_WDT_TIMER_REG);
	return di->charge_set_chgwdt_timer_reg.reg.watchdog_timer;
}

/****************************************************************************
  Function:     hi6521_config_thernal_regulation_reg
  Description:  config thernal alarm threshold.
  Input:        struct hi6521_device_info:di->chg_thsd_treg_set,di->chg_thsd_tstp_set.
  Output:       NA
  Return:       NA
***************************************************************************/
STATIC void hi6521_config_thernal_regulation_reg(struct hi6521_device_info *di)
{
    unsigned int treg_temperature = 0;
    unsigned char treg            = 0;
	unsigned char tstp            = 0;

	/*config thsd_treg*/
    treg_temperature = di->chg_thsd_treg_set;
	if (treg_temperature < CHG_THSD_TREG_SET_MIN)
	    treg_temperature = CHG_THSD_TREG_SET_MIN;
	else if(treg_temperature > CHG_THSD_TREG_SET_MAX)
    	treg_temperature = CHG_THSD_TREG_SET_MAX;
	treg = (unsigned char)((treg_temperature - CHG_THSD_TREG_SET_MIN)/CHG_THSD_TREG_STEP_20);
	di->charge_ref_reg.reg.thsd_treg_set = treg;
	/*config thsd_tstp*/
	if (CHG_THSD_TSTP_SET_120C == di->chg_thsd_tstp_set)
		tstp = 0;
	else if (CHG_THSD_TSTP_SET_140C == di->chg_thsd_tstp_set)
		tstp = 1;
	else
		tstp = 0;
	di->charge_ref_reg.reg.thsd_tstp_set = tstp;

	hi6521_write_byte(di, di->charge_ref_reg.value, CHG_REF_REG);

}

/****************************************************************************
  Function:     hi6521_config_thernal_regulation_reg
  Description:  check charger present by buck_ok bit.
  Input:        struct hi6521_device_info:di->chg_thsd_treg_set,di->chg_thsd_tstp_set.
  Output:       NA
  Return:       NA
***************************************************************************/
STATIC int  hi6521_check_charger_present(struct hi6521_device_info *di)
{
    unsigned char data = 0;
	hi6521_read_byte(di, &data, CHG_REG6);
	if (CHG_BUCK_OK != (data & CHG_BUCK_OK)){
		return HI6521_VBUS_STAT_UNKNOWM;
	}
    return data;
}

/****************************************************************************
  Function:     hi6521_config_opt_param
  Description:  scharger optimization config.
  Input:        struct hi6521_device_info:di.
  Output:       NA
  Return:       NA
***************************************************************************/

STATIC void hi6521_config_opt_param(struct hi6521_device_info *di)
{
	unsigned char chg_version = 0;
	unsigned char v210_fx = 0;
	hi6521_read_byte(di,&chg_version, CHG_V210_VERSION);
	hi6521_read_byte(di, &v210_fx, CHG_V210_CMBOX);

	if ((CHG_V200 == chg_version) && (CHG_V200 == v210_fx))
	{
		hi6521_write_byte(di, 0x0D, 0x54);
	    hi6521_write_byte(di, 0xB6, 0x3A);
		hi6521_write_byte(di, 0x12, 0x21);
		hi6521_write_byte(di, 0x7B, 0x25);
	    hi6521_write_byte(di, 0x07, 0x27);
	    hi6521_write_byte(di, 0x7F, 0x1C);
	    hi6521_write_byte(di, 0x78, 0x17);
	    hi6521_write_byte(di, 0xC8, 0x16);
	    hi6521_write_byte(di, 0x6B, 0x1B);
	    hi6521_write_byte(di, 0x99, 0x18);
	    hi6521_write_byte(di, 0x26, 0x64);
	    hi6521_write_byte(di, 0x0F, 0x69);
	    hi6521_write_byte(di, 0x1A, 0x56);
	    hi6521_write_byte(di, 0x1D, 0x5E);
	    hi6521_write_byte(di, 0x2F, 0x58);
	    hi6521_write_byte(di, 0x6C, 0x59);
	    hi6521_write_byte(di, 0xF9, 0x5B);
	    hi6521_write_byte(di, 0x37, 0x5D);
	    hi6521_write_byte(di, 0x18, 0x2F);
		hi6521_write_byte(di, 0x3C, 0x31);

	    hi6521_write_byte(di, 0x10, 0x0C);
	    hi6521_write_byte(di, 0x03, 0x0D);
	    hi6521_write_byte(di, 0x80, 0x0F);
	    hi6521_write_byte(di, 0x82, 0x10);
	    hi6521_write_byte(di, 0x01, 0x45);
	    hi6521_write_byte(di, 0x2e, 0x4A);
	    hi6521_write_byte(di, 0x90, 0x3B);
		hi6521_write_byte(di, 0x12, 0x65);
		hi6521_write_byte(di, 0x08, 0x33);
		hi6521_write_byte(di, 0x6B, 0x1D);
	}
	else
	{
 		hi6521_write_byte(di, 0x14, 0x48);
	    hi6521_write_byte(di, 0x8A, 0x40);

		hi6521_write_byte(di, 0x2A, 0x42);
		hi6521_write_byte(di, 0x11, 0x3B);
	    hi6521_write_byte(di, 0xF3, 0x37);
	    hi6521_write_byte(di, 0x82, 0x45);
	    hi6521_write_byte(di, 0x25, 0x41);
	    hi6521_write_byte(di, 0x11, 0x3e);

	    hi6521_write_byte(di, 0x7F, 0x1C);
	    hi6521_write_byte(di, 0x78, 0x17);
	    hi6521_write_byte(di, 0xC8, 0x16);
	    hi6521_write_byte(di, 0x6B, 0x1D);
	    hi6521_write_byte(di, 0x99, 0x18);
	    hi6521_write_byte(di, 0x18, 0x2F);
	    hi6521_write_byte(di, 0x2C, 0x31);
		/*PHONE*/
	    hi6521_write_byte(di, 0x0A, 0x21);
	    hi6521_write_byte(di, 0x7B, 0x25);
	    hi6521_write_byte(di, 0x07, 0x27);
	#if 0 /*PAD*/
	    hi6521_write_byte(di, 0x0A, 0x21);
		hi6521_write_byte(di, 0xA5, 0x24);
	    hi6521_write_byte(di, 0x54, 0x25);
	    hi6521_write_byte(di, 0x07, 0x27);
	#endif
	    hi6521_write_byte(di, 0x26, 0x64);
	    hi6521_write_byte(di, 0x0F, 0x69);
	    hi6521_write_byte(di, 0x1A, 0x56);
	    hi6521_write_byte(di, 0x1D, 0x5E);
	    hi6521_write_byte(di, 0x2F, 0x58);
	    hi6521_write_byte(di, 0x6C, 0x59);
	    hi6521_write_byte(di, 0xF9, 0x5B);
	    hi6521_write_byte(di, 0x33, 0x5D);
	    hi6521_write_byte(di, 0x12, 0x65);
		hi6521_write_byte(di, 0x08, 0x33);
		hi6521_write_byte(di, 0x85, 0x23);

	}

	if (((0 == chg_version) && (1 == v210_fx)) ||
		((0 == chg_version) && (2 == v210_fx)) ||
		((0 == chg_version) && (3 == v210_fx)) ||
		((2 == chg_version) && (3 == v210_fx))) { /*215-220,开启反向放电的ocp*/

		hi6521_write_byte(di, 0x18, 0x63);
	}

}


/****************************************************************************
  Function:     scharger_power_status
  Description:  get scharger power on status .
  Input:        di :regulator id
  Output:       NA
  Return:       1:enable 0:disable other:read fail.
***************************************************************************/

int scharger_power_status(int id)
{
	int ret = 0;
	if (NULL == scharger_di){
		return -1;
	}
	ret = hi6521_read_byte(scharger_di, &scharger_di->power_on_config_reg00.value, POWER_ON_CONFIG_REG00);
	if (ret){
		hwlog_err("read power_on reg00  fail ! \n");
		return ret;
	}
	ret = hi6521_read_byte(scharger_di, &scharger_di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);
	if (ret){
		hwlog_err("read power_on reg01  fail ! \n");
		return ret;
	}
	switch(id){
	case SCHG_BOOST1_ID://otg:
		ret = scharger_di->power_on_config_reg01.reg.otg_en_int;
		break;
	case SCHG_BOOST2_ID://wled :
		ret = scharger_di->power_on_config_reg01.reg.wled_en_int;
		break;
	case SCHG_BOOST3_ID://flash_bst:
		ret = scharger_di->power_on_config_reg00.reg.flash_bst_en_int;
		break;
	case SCHG_SOURCE1_ID://flash_led_flash:
		ret = scharger_di->power_on_config_reg00.reg.flash_led_flash_en;
		break;
	case SCHG_SOURCE2_ID://flash_led_torch:
		ret = scharger_di->power_on_config_reg00.reg.flash_led_torch_en;
		break;
	case SCHG_BOOST4_ID://lcd_boost:
		ret = scharger_di->power_on_config_reg00.reg.lcd_bst_en_int;
		break;
	case SCHG_LDO3_ID://lcd_ldo:
		ret = scharger_di->power_on_config_reg00.reg.lcd_ldo_en_int;
		break;
	case SCHG_CPN1_ID://lcd_ncp:
		ret = scharger_di->power_on_config_reg00.reg.lcd_ncp_en_int;
		break;
	case SCHG_LDO1_ID://ldo1:
		ret = scharger_di->power_on_config_reg01.reg.ldo1_200ma_en_int;
		break;
	case SCHG_LDO2_ID://ldo2:
		ret = scharger_di->power_on_config_reg01.reg.ldo2_200ma_en_int;
		break;
    case SCHG_LDO3_CPN1_ID://vsp && vsn:
        ret = (scharger_di->power_on_config_reg00.reg.lcd_ldo_en_int && scharger_di->power_on_config_reg00.reg.lcd_ncp_en_int);
        break;
	default:
		break;

	}
	return ret;

}
/****************************************************************************
  Function:     scharger_power_on
  Description:  enable scharger power .
  Input:        di :regulator id
  Output:       NA
  Return:       0:enable success other: fail.
***************************************************************************/

int scharger_power_on(int id)
{

	int ret = 0;
	if (NULL == scharger_di){
		return -1;
	}
	switch(id){
	case SCHG_BOOST1_ID://otg:
		scharger_di->otg_en = CHG_POWER_EN;
		//scharger_di->otg_force_en= CHG_POWER_EN;
		ret = hi6521_config_otg_enable(scharger_di);
		break;
	case SCHG_BOOST2_ID://wled :
		scharger_di->wled_en = CHG_POWER_EN;
		ret = hi6521_config_wled_enable(scharger_di);
		break;
	case SCHG_BOOST3_ID://flash_bst:
        scharger_di->flash_bst_en = CHG_POWER_EN;
		ret = hi6521_config_flash_boost_enable(scharger_di);
        udelay(500);
		break;
	case SCHG_SOURCE1_ID://flash_led_flash:
		scharger_di->flash_led_flash_en = CHG_POWER_EN;
		ret = hi6521_config_flash_led_flash_enable(scharger_di);
		break;
	case SCHG_SOURCE2_ID://flash_led_torch:
		scharger_di->flash_led_torch_en = CHG_POWER_EN;
		ret = hi6521_config_flash_led_torch_enable(scharger_di);
		break;
	case SCHG_BOOST4_ID://lcd_boost:
		scharger_di->lcd_bst_en = CHG_POWER_EN;
		ret = hi6521_config_lcd_boost_enable(scharger_di);
		break;
	case SCHG_LDO3_ID://lcd_ldo:
		scharger_di->lcd_ldo_en = CHG_POWER_EN;
		ret = hi6521_config_lcd_ldo_enable(scharger_di);
		break;
	case SCHG_CPN1_ID://lcd_ncp:
		scharger_di->lcd_ncp_en = CHG_POWER_EN;
		ret = hi6521_config_lcd_ncp_enable(scharger_di);
		break;
	case SCHG_LDO1_ID://ldo1:
		scharger_di->ldo1_200ma_en = CHG_POWER_EN;
		ret = hi6521_config_ldo1_200ma_enable(scharger_di);
		break;
	case SCHG_LDO2_ID://ldo2:
		scharger_di->ldo2_200ma_en = CHG_POWER_EN;
		ret = hi6521_config_ldo2_200ma_enable(scharger_di);;
		break;
	case SCHG_LDO3_CPN1_ID://vsp && vsn
		scharger_di->lcd_ldo_en = CHG_POWER_EN;
		scharger_di->lcd_ncp_en = CHG_POWER_EN;
		ret = hi6521_config_lcd_ldo_ncp_enable(scharger_di);
		break;

	default:
		break;

	}
	return ret;

}
/****************************************************************************
  Function:     scharer_power_off
  Description:  disable scharger power .
  Input:        di :regulator id
  Output:       NA
  Return:       0:disable success other: fail.
***************************************************************************/

int scharger_power_off(int id)
{
	int ret = 0;
	if (NULL == scharger_di){
		return -1;
	}
	switch(id){
	case SCHG_BOOST1_ID://otg:
		scharger_di->otg_en = CHG_POWER_DIS;
		//scharger_di->otg_force_en= CHG_POWER_DIS;
		//ret = hi6521_config_otg_enable(scharger_di);
		break;
	case SCHG_BOOST2_ID://wled :
		scharger_di->wled_en = CHG_POWER_DIS;
		ret = hi6521_config_wled_enable(scharger_di);
		break;
	case SCHG_BOOST3_ID://flash_bst:
	    mdelay(3);
        scharger_di->flash_bst_en = CHG_POWER_DIS;
		ret = hi6521_config_flash_boost_enable(scharger_di);
		break;
	case SCHG_SOURCE1_ID://flash_led_flash:
		scharger_di->flash_led_flash_en = CHG_POWER_DIS;
		ret = hi6521_config_flash_led_flash_enable(scharger_di);
		break;
	case SCHG_SOURCE2_ID://flash_led_torch:
		scharger_di->flash_led_torch_en = CHG_POWER_DIS;
		ret = hi6521_config_flash_led_torch_enable(scharger_di);
		break;
	case SCHG_BOOST4_ID://lcd_boost:
		scharger_di->lcd_bst_en = CHG_POWER_DIS;
		ret = hi6521_config_lcd_boost_enable(scharger_di);
		break;
	case SCHG_LDO3_ID://lcd_ldo:
		scharger_di->lcd_ldo_en = CHG_POWER_DIS;
		ret = hi6521_config_lcd_ldo_enable(scharger_di);
		break;
	case SCHG_CPN1_ID://lcd_ncp:
		scharger_di->lcd_ncp_en = CHG_POWER_DIS;
		ret = hi6521_config_lcd_ncp_enable(scharger_di);
		break;
	case SCHG_LDO1_ID://ldo1:
		scharger_di->ldo1_200ma_en = CHG_POWER_DIS;
		ret = hi6521_config_ldo1_200ma_enable(scharger_di);
		break;
	case SCHG_LDO2_ID://ldo2:
		scharger_di->ldo2_200ma_en = CHG_POWER_DIS;
		ret = hi6521_config_ldo2_200ma_enable(scharger_di);
		break;
	case SCHG_LDO3_CPN1_ID://vsp && vsn
		scharger_di->lcd_ncp_en = CHG_POWER_DIS;
		ret = hi6521_config_lcd_ncp_enable(scharger_di);
		mdelay(5);
		scharger_di->lcd_ldo_en = CHG_POWER_DIS;
		ret = hi6521_config_lcd_ldo_enable(scharger_di);
		break;
	default:
		break;

	}

	return ret;

}

/****************************************************************************
  Function:     scharger_power_set_voltage
  Description:  set scharger power voltage .
  Input:        vset_regs,mask,shift,index
  Output:       NA
  Return:       0:disable success other: fail.
***************************************************************************/

int scharger_power_set_voltage(unsigned int vset_regs,unsigned int mask,int shift,int index)
{
	int ret = 0;
	struct hi6521_device_info *di;
	unsigned char reg_value = 0;
#if 0
	reg_value = 0xff;
	CHG_REG_SETBITS(reg_value ,shift, mask, index);
	hwlog_err("***** syb reg_value = %d******* \n",reg_value);
#endif

	if (NULL == scharger_di){
		hwlog_err("hi6521 scharger_id is null! \n");
		return -1;
	}
	di = scharger_di;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("scharger_power_set_voltage get sema fail! \n");
		return -EINTR;
	}
	/*set voltage*/
	ret = hi6521_read_byte(di,&reg_value, (u8)vset_regs);
	CHG_REG_SETBITS(reg_value ,shift, mask, index);
	ret |= hi6521_write_byte(di, reg_value, (u8)vset_regs);
	up(&di->charger_data_busy_lock);
	return ret;
}

/****************************************************************************
  Function:     scharger_power_get_voltage_index
  Description:  get scharger power voltage index.
  Input:        vget_regs,mask,shift
  Output:       NA
  Return:       voltage index.
***************************************************************************/
int scharger_power_get_voltage_index(unsigned int vget_regs,unsigned int mask,int shift)
{
	int ret = 0;
	struct hi6521_device_info *di;
	unsigned char reg_value = 0;
	int index = 0;
#if 0
	reg_value = 0xff;
	index = CHG_REG_GETBITS(reg_value, shift, mask);
	hwlog_info("***** syb index = %d******* \n",index);
#endif
	if (NULL == scharger_di){
		hwlog_err("hi6521 scharger_id is null! \n");
		return -1;
	}
	di = scharger_di;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("scharger_power_get_current_limit get sema fail! \n");
		return -EINTR;
	}
	/*get voltage*/
	ret = hi6521_read_byte(di,&reg_value, (u8)vget_regs);
	index = CHG_REG_GETBITS(reg_value, shift, mask);
	up(&di->charger_data_busy_lock);
	return index;

}

/****************************************************************************
  Function:     scharger_power_set_current_limit
  Description:  set scharger power current .
  Input:        vset_regs:reg,mask:mask bits,shift:shift value
  Output:       NA
  Return:       0:disable success other: fail.
***************************************************************************/
int scharger_power_set_current_limit(unsigned int vset_regs,unsigned int mask,int shift,int index)
{
	int ret = 0;
	struct hi6521_device_info *di;
	unsigned char reg_value = 0;
#if 0
	reg_value = 0xff;
	CHG_REG_SETBITS(reg_value ,shift, mask, index);
	hwlog_info("***** syb reg_value = %d******* \n",reg_value);
#endif

	if (NULL == scharger_di){
		hwlog_err("hi6521 scharger_id is null! \n");
		return -1;
	}
	di = scharger_di;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("scharger_power_set_current_limit get sema fail! \n");
		return -EINTR;
	}
	/*set current*/
	ret = hi6521_read_byte(di,&reg_value, (u8)vset_regs);
	CHG_REG_SETBITS(reg_value ,shift, mask, index);
	ret |= hi6521_write_byte(di, reg_value, (u8)vset_regs);
	up(&di->charger_data_busy_lock);
	return ret;
}
/****************************************************************************
  Function:     scharger_power_get_current_limit_index
  Description:  get scharger power current index.
  Input:        vget_regs,mask,shift
  Output:       NA
  Return:       voltage index
***************************************************************************/
int scharger_power_get_current_limit_index(unsigned int vget_regs,unsigned int mask,int shift)
{
	int ret = 0;
	struct hi6521_device_info *di;
	unsigned char reg_value = 0;
	int index = 0;
#if 0
	reg_value = 0xf7;
	index = CHG_REG_GETBITS(reg_value, shift, mask);
	hwlog_err("***** syb index = %d******* \n",index);
#endif
	if (NULL == scharger_di){
		hwlog_err("hi6521 scharger_id is null! \n");
		return -1;
	}
	di = scharger_di;
	if (down_interruptible(&di->charger_data_busy_lock)){
		hwlog_err("scharger_power_get_current_limit get sema fail! \n");
		return -EINTR;
	}
	/*get current*/
	ret = hi6521_read_byte(di,&reg_value, (u8)vget_regs);
	index = CHG_REG_GETBITS(reg_value, shift, mask);
	up(&di->charger_data_busy_lock);
	return index;
}


/****************************************************************************
  Function:     scharger_flash_led_timeout_enable
  Description:  enable flash led timeout.
  Input:        lightness
  Output:       NA
  Return:       0:enable success other: fail.
***************************************************************************/
int scharger_flash_led_timeout_enable(void)
{
	if (NULL == scharger_di){
		return -1;
	}
	scharger_di->chg_flash_led_timeout_en = 1;
    return hi6521_config_flash_led_timeout_enable(scharger_di);
}

/****************************************************************************
  Function:     scharger_flash_led_timeout_disable
  Description:  disable flash led timeout.
  Input:        lightness
  Output:       NA
  Return:       0:enable success other: fail.
***************************************************************************/
int scharger_flash_led_timeout_disable(void)
{
	if (NULL == scharger_di){
		return -1;
	}
	scharger_di->chg_flash_led_timeout_en = 0;
    return hi6521_config_flash_led_timeout_enable(scharger_di);
}

/****************************************************************************
  Function:     scharger_flash_led_timeout_config
  Description:  config flash led timeout.
  Input:        timeOutMs
  Output:       NA
  Return:       0:config success other: fail.
***************************************************************************/
int scharger_flash_led_timeout_config(unsigned int timeoutMs)
{
	if (NULL == scharger_di){
		return -1;
	}

	scharger_di->chg_flash_led_timeout = timeoutMs;

    return hi6521_config_flash_led_timeout(scharger_di);
}

/****************************************************************************
  Function:     scharger_flash_torch_timeout_config
  Description:  config flash torch timeout.
  Input:        timeoutSec
  Output:       NA
  Return:       0:config success other: fail.
***************************************************************************/
int scharger_flash_torch_timeout_config(unsigned int timeoutSec)
{
	if (NULL == scharger_di){
		return -1;
	}

	scharger_di->chg_flash_torch_timeout = timeoutSec;

    return hi6521_config_flash_torch_timeout(scharger_di);
}


/*deal with poor charger when capacity is more than 90% ,if hardware does not
 use REGN for usb_int,pls delete the follow fuction*/
STATIC void hi6521_reset_vindpm(struct hi6521_device_info *di)
{
	int cur_battery_vol = 0;
	static int pre_battery_vol;

	if (POWER_SUPPLY_TYPE_MAINS != di->charger_source)
		return;

	cur_battery_vol = hisi_battery_voltage_uv();
	if (((cur_battery_vol > pre_battery_vol) && (cur_battery_vol < 3900000)) ||
		 ((cur_battery_vol < pre_battery_vol) && (cur_battery_vol < 3850000))) {
		di->cin_dpmmV = VINDPM_4280;
	} else if (((cur_battery_vol > pre_battery_vol) && (cur_battery_vol < 4000000)) ||
		 ((cur_battery_vol < pre_battery_vol) && (cur_battery_vol < 3950000))) {
		di->cin_dpmmV = VINDPM_4440;
	} else if (((cur_battery_vol > pre_battery_vol) && (cur_battery_vol < 4250000)) ||
		 ((cur_battery_vol < pre_battery_vol) && (cur_battery_vol < 4200000))) {
		di->cin_dpmmV = VINDPM_4520;
	} else if (((cur_battery_vol > pre_battery_vol) && (cur_battery_vol > 4250000)) ||
		 ((cur_battery_vol < pre_battery_vol) && (cur_battery_vol > 4200000))) {
		di->cin_dpmmV = VINDPM_4600;
	}
	hi6521_config_input_source_reg(di);
	pre_battery_vol = cur_battery_vol;
	hwlog_info("dpmmv = %d! \n", di->cin_dpmmV);
}

/*0 = temperature less than 42,1 = temperature more than 42 and less 45,2 more than 45 */
STATIC void hi6521_calling_limit_ac_input_current(struct hi6521_device_info *di,int flag)
{
    if (di->charger_source == POWER_SUPPLY_TYPE_MAINS)
    {
       switch(flag){
       case NORMAL_TEMP_CONFIG:
           if(di->calling_limit){
               iin_temp = IINLIM_900;
               ichg_temp = CHG_FAST_ICHG_VALUE(800);
           } else {
               iin_temp = di->max_cin_currentmA;
               ichg_temp = di->max_currentmA;
           }
           break;
       case NORMAL_HIGH_TEMP_CONFIG:
           if(di->calling_limit){
               iin_temp = IINLIM_900;
               ichg_temp = CHG_FAST_ICHG_VALUE(800);
           } else {
               if(iin_temp == di->max_cin_currentmA)
                   ichg_temp =  di->max_currentmA;
               iin_temp = di->cin_limit;
               ichg_temp = di->chg_fast_ichg;
           }

           break;
       case HIGH_TEMP_CONFIG:
           iin_temp = IINLIM_900;
           ichg_temp = CHG_FAST_ICHG_VALUE(800);
           break;
       case TEMP_CONFIG_5:
           if(di->calling_limit){
               iin_temp = IINLIM_900;
               ichg_temp = di->design_capacity / 10 * di->charge_in_temp_5;
           } else {
               iin_temp = di->max_cin_currentmA;
               /* battery whose max_voltage is above 4.35V is easy to broken
                  when the temperature is below 10℃.
                  So we need set the Current below 0.x * Capacity. */
               ichg_temp = di->design_capacity / 10 * di->charge_in_temp_5;
           }
           break;
       case TEMP_CONFIG_10:
           if(di->calling_limit){
               iin_temp = IINLIM_900;
               ichg_temp = di->design_capacity / 10 * di->charge_in_temp_10;
           } else {
               iin_temp = di->max_cin_currentmA;
               ichg_temp = di->design_capacity / 10 * di->charge_in_temp_10;
           }
           break;
        case NORMAL_SECOND_STAGE:
            if(di->calling_limit){
               iin_temp = IINLIM_900;
               ichg_temp = CHG_FAST_ICHG_VALUE(800);//ICHG_820;
           } else {
               iin_temp = IINLIM_1200;
               ichg_temp = CHG_FAST_ICHG_VALUE(1000);//ICHG_1024;
           }
           break;
       case NORMAL_HIGH_TEMP_CONFIG_CP2:
           iin_temp = IINLIM_900;
           ichg_temp = CHG_FAST_ICHG_VALUE(800);
           break;
       case NORMAL_HIGH_TEMP_CONFIG_CP4:
           iin_temp = IINLIM_500;
           ichg_temp = CHG_FAST_ICHG_MIN;//ICHG_MIN;
           break;
	   default:
	   		break;

        }
    }else{
        iin_temp = IINLIM_500;
        ichg_temp = CHG_FAST_ICHG_VALUE(500);
    }

    if(iin_temp > input_current_iin)
        iin_temp = input_current_iin;
    if(ichg_temp > input_current_ichg)
        ichg_temp = input_current_ichg;
    if(!mhl_limit_cin_flag)
        iin_temp = IINLIM_100;
    return;
}

STATIC void hi6521_config_limit_temperature_parameter(struct hi6521_device_info *di)
{
    //di->coldhot_charging_flag = 0;//DIS_CHARGER;
    //di->limit_charging_flag = 1;//EN_CHARGER;
    //di->hot_limit_charging_flag = 1;//enable charge limit at first
    di->temperature_cold = HI6521_COLD_BATTERY_THRESHOLD;
    di->temperature_cool = HI6521_COOL_BATTERY_THRESHOLD;
    di->temperature_warm = HI6521_WARM_BATTERY_THRESHOLD;
    di->temperature_hot  = HI6521_HOT_BATTERY_THRESHOLD;
    di->temperature_5    = HI6521_BATTERY_THRESHOLD_5;
    di->temperature_10   = HI6521_BATTERY_THRESHOLD_10;
    return;
}

STATIC int hi6521_check_battery_temperature_japan_threshold(struct hi6521_device_info *di)
{
    int battery_temperature = 0;

    battery_temperature = hisi_battery_temperature();

    if (battery_temperature < HI6521_COLD_BATTERY_THRESHOLD) {
        return BATTERY_HEALTH_TEMPERATURE_OVERLOW;

    } else if(battery_temperature <  HI6521_COOL_BATTERY_THRESHOLD) {
        return BATTERY_HEALTH_TEMPERATURE_LOW;

    } else if (battery_temperature < HI6521_BATTERY_THRESHOLD_5) {
       return BATTERY_HEALTH_TEMPERATURE_5;

    } else if (battery_temperature < HI6521_BATTERY_THRESHOLD_10) {
       return BATTERY_HEALTH_TEMPERATURE_10;

    } else if (battery_temperature < japan_temp_cp[di->high_temp_para].cp1) {
       return BATTERY_HEALTH_TEMPERATURE_NORMAL;

    } else if (battery_temperature < japan_temp_cp[di->high_temp_para].cp2) {
        return BATTERY_HEALTH_TEMPERATURE_HIGH_CP1;

    } else if (battery_temperature < japan_temp_cp[di->high_temp_para].cp3) {
        return BATTERY_HEALTH_TEMPERATURE_HIGH_CP2;

    } else if (battery_temperature < japan_temp_cp[di->high_temp_para].cp4) {
        return BATTERY_HEALTH_TEMPERATURE_HIGH_CP3;

    } else if (battery_temperature < japan_temp_cp[di->high_temp_para].cp5) {
        return BATTERY_HEALTH_TEMPERATURE_HIGH_CP4;

    } else if (battery_temperature < japan_temp_cp[di->high_temp_para].cp6) {
        return BATTERY_HEALTH_TEMPERATURE_HIGH_CP5;

    }else {
       return BATTERY_HEALTH_TEMPERATURE_OVERHIGH;

    }
}

STATIC int hi6521_check_battery_temperature_threshold(struct hi6521_device_info *di)
{
    int battery_temperature = 0;

    battery_temperature = hisi_battery_temperature();

    if (battery_temperature < di->temperature_cold) {
        return BATTERY_HEALTH_TEMPERATURE_OVERLOW;

    } else if((battery_temperature >= di->temperature_cold)
        && (battery_temperature <  di->temperature_cool)){
        return BATTERY_HEALTH_TEMPERATURE_LOW;

    } else if ((battery_temperature >= di->temperature_cool)
        && (battery_temperature < di->temperature_5)){
       return BATTERY_HEALTH_TEMPERATURE_5;

    } else if ((battery_temperature >= di->temperature_5)
        && (battery_temperature < di->temperature_10)){
       return BATTERY_HEALTH_TEMPERATURE_10;

    } else if ((battery_temperature >= di->temperature_10)
        && (battery_temperature < (di->temperature_warm - HI6521_TEMPERATURE_OFFSET))){
       return BATTERY_HEALTH_TEMPERATURE_NORMAL;

    } else if ((battery_temperature >= (di->temperature_warm - HI6521_TEMPERATURE_OFFSET))
        && (battery_temperature < di->temperature_warm)){
        return BATTERY_HEALTH_TEMPERATURE_NORMAL_HIGH;

    } else if ((battery_temperature >= di->temperature_warm)
        && (battery_temperature < di->temperature_hot)){
        return BATTERY_HEALTH_TEMPERATURE_HIGH;

    } else if (battery_temperature >= di->temperature_hot){
       return BATTERY_HEALTH_TEMPERATURE_OVERHIGH;

    } else {
       return BATTERY_HEALTH_TEMPERATURE_NORMAL;
    }
}

/* get hot area temperature by nct203. */
STATIC int get_hot_temperature(void)
{
    //extern int nct203_temp_report(void);
    return -1;//nct203_temp_report();
}

STATIC void hi6521_monitor_battery_ntc_japan_charging(struct hi6521_device_info *di)
{
    int battery_status = 0;
    unsigned int events = VCHRG_START_CHARGING_EVENT;
    u8 read_reg = 0;
    int batt_temp = hisi_battery_temperature();
    int chg_en = 0;

    //hi6521_read_byte(di, &read_reg, CHG_STATUS0);
	if (!di->battery_present){
		/*battry is not present ,notify scharger ic.*/
		hi6521_write_byte(di, CHG_BATTRY_NOT_PRESENT, CHG_REG10);
		return;
	}
	else{
		hi6521_write_byte(di, CHG_BATTRY_PRESENT, CHG_REG10);
	}
    //battery_temperature = hisi_battery_temperature();
    //di->battery_voltage = hisi_battery_voltage();
    hi6521_reset_vindpm(di);
    if(di->not_limit_chrg_flag){
        hi6521_config_power_on_reg01(di);
        return;
    }

    hwlog_info("%s: japan Status:batt_temp is %d , ntc_temp is %d , batt_volt is %d",
	        __FUNCTION__, batt_temp, get_hot_temperature(), di->battery_voltage);

    di->battery_voltage = hisi_battery_voltage();
    battery_status = hi6521_check_battery_temperature_japan_threshold(di);


    switch (battery_status) {
    case BATTERY_HEALTH_TEMPERATURE_OVERLOW:
        chg_en = CHG_POWER_DIS;
        break;

    case BATTERY_HEALTH_TEMPERATURE_LOW:
        if (di->battery_voltage <= HI6521_LOW_TEMP_TERM_VOLTAGE)
            chg_en = CHG_POWER_EN;
        else if (di->battery_voltage > HI6521_LOW_TEMP_NOT_CHARGING_VOLTAGE)
            chg_en = CHG_POWER_DIS;

        if(di->battery_temp_status != battery_status){
            ichg_temp = CHG_FAST_ICHG_MIN;

        }
        if (di->is_disable_cool_temperature_charger == 1){
            chg_en = CHG_POWER_DIS;
        }
        break;

    case BATTERY_HEALTH_TEMPERATURE_5:
        chg_en = CHG_POWER_EN;
        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,TEMP_CONFIG_5);
        }
        break;

    case BATTERY_HEALTH_TEMPERATURE_10:
        chg_en = CHG_POWER_EN;
        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,TEMP_CONFIG_10);
        }
        break;

    case BATTERY_HEALTH_TEMPERATURE_NORMAL:
        chg_en = CHG_POWER_EN;
            if((di->is_two_stage_charger== 1)
                &&( (di->battery_voltage >= di->first_stage_voltage)
                        || ((di->battery_voltage >= di->second_stage_voltage)
                        &&(di->two_stage_charger_status  == TWO_STAGE_CHARGE_SECOND_STAGE)))){
                 hi6521_calling_limit_ac_input_current(di,NORMAL_SECOND_STAGE);
                 di->two_stage_charger_status  = TWO_STAGE_CHARGE_SECOND_STAGE;
            }else if(di->battery_voltage > HI6521_NORNAL_ICHRG_VOLTAGE){
                 hi6521_calling_limit_ac_input_current(di,NORMAL_TEMP_CONFIG);
                 di->two_stage_charger_status  = TWO_STAGE_CHARGE_FIRST_STAGE;
            } else {
                 ichg_temp = CHG_FAST_ICHG_VALUE(800);
            }
        break;

    case BATTERY_HEALTH_TEMPERATURE_HIGH_CP1:
        chg_en = CHG_POWER_EN;
        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,NORMAL_HIGH_TEMP_CONFIG_CP1);
        }
        break;

     case BATTERY_HEALTH_TEMPERATURE_HIGH_CP2:
        chg_en = CHG_POWER_EN;
        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,NORMAL_HIGH_TEMP_CONFIG_CP2);
        }
        break;

      case BATTERY_HEALTH_TEMPERATURE_HIGH_CP3:
        chg_en = CHG_POWER_EN;
        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,NORMAL_HIGH_TEMP_CONFIG_CP3);
        }
        break;

     case BATTERY_HEALTH_TEMPERATURE_HIGH_CP4:
        chg_en = CHG_POWER_EN;
        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,NORMAL_HIGH_TEMP_CONFIG_CP4);
        }
        break;

     case BATTERY_HEALTH_TEMPERATURE_HIGH_CP5:
        chg_en = di->chg_en;
        if ((di->battery_temp_status != battery_status) && (di->chg_en == CHG_POWER_EN)){
            hi6521_calling_limit_ac_input_current(di,NORMAL_HIGH_TEMP_CONFIG_CP4);
        }
        break;

    case BATTERY_HEALTH_TEMPERATURE_OVERHIGH:
        chg_en = CHG_POWER_DIS;
        break;
    default:
        break;
    }
    di->battery_temp_status = battery_status;

    if(cin_limit_temp == 1){
        if(di->cin_limit != iin_temp){
            di->cin_limit = iin_temp;
        }
    }else{
        di->cin_limit = cin_limit_temp;
    }
    hi6521_config_input_source_reg(di);

    if(charge_current_temp == 1){
        if(di->chg_fast_ichg != ichg_temp){
            di->chg_fast_ichg = ichg_temp;
        }
    }else{
        di->chg_fast_ichg = charge_current_temp;
    }
    hi6521_config_fast_current_reg(di);

    chg_en = (chg_en & di->factory_flag) | di->not_stop_chrg_flag;

    hi6521_config_charger_enable_val(di,chg_en);

    hwlog_info("japan iin = %d, ichg = %d\n", di->cin_limit, di->chg_fast_ichg);
    hi6521_read_byte(di, &read_reg, CHG_STATUS0);
    if((read_reg & CHG_STATUS0_STAT_CHAEGE_DONE) == CHG_STATUS0_STAT_CHAEGE_DONE)
        return;

    if(chg_en){
        events = VCHRG_START_CHARGING_EVENT;
    } else {
        events = VCHRG_NOT_CHARGING_EVENT;
    }
    hisi_coul_charger_event_rcv(events);//blocking_notifier_call_chain(&notifier_list, events, NULL);
    return;
}

STATIC void hi6521_monitor_battery_ntc_charging(struct hi6521_device_info *di)
{
    int battery_status = 0;
    unsigned int events = VCHRG_START_CHARGING_EVENT;
    u8 read_reg = 0;
    int batt_temp = 0;
    int chg_en = 0;

    if(di->japan_charger){
        return hi6521_monitor_battery_ntc_japan_charging(di);
    }

    if (!di->battery_present){
        /*battry is not present ,notify scharger ic.*/
        hi6521_write_byte(di, CHG_BATTRY_NOT_PRESENT, CHG_REG10);
        hwlog_info("ntc_charging battery is not present!\n");
        return;
    }
    else{
        hi6521_write_byte(di, CHG_BATTRY_PRESENT, CHG_REG10);
    }

    if(di->not_limit_chrg_flag){
        hi6521_config_charger_enable(di);
        hwlog_info("ntc_charging not_limit_chrg_flag is set!\n");
        return;
    }

    di->battery_voltage = hisi_battery_voltage();
    battery_status = hi6521_check_battery_temperature_threshold(di);
#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
    charge_temp_protected= hisi_coul_reg_read();
    if(charge_temp_protected==1)
    {
      battery_status=BATTERY_HEALTH_TEMPERATURE_NORMAL;
    }
#endif
   switch (battery_status) {
    case BATTERY_HEALTH_TEMPERATURE_OVERLOW:
        chg_en = CHG_POWER_DIS;
        break;
    case BATTERY_HEALTH_TEMPERATURE_LOW:
        /*
        if (di->battery_voltage <= HI6521_LOW_TEMP_TERM_VOLTAGE)
            di->chg_en = CHG_POWER_EN;
        else if (di->battery_voltage > HI6521_LOW_TEMP_NOT_CHARGING_VOLTAGE)
            di->chg_en = CHG_POWER_DIS;
        //else
            //di->chg_en =di->chrg_config;

        if(di->battery_temp_status != battery_status){
            di->chg_fast_ichg = CHG_FAST_ICHG_MIN;
            hi6521_config_fast_current_reg(di);
        }*/
        chg_en = CHG_POWER_DIS;
        break;
    case BATTERY_HEALTH_TEMPERATURE_5:
        chg_en = CHG_POWER_EN;
        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,TEMP_CONFIG_5);
        }
        break;
    case BATTERY_HEALTH_TEMPERATURE_10:
        chg_en = CHG_POWER_EN;
        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,TEMP_CONFIG_10);
        }
        break;
    case BATTERY_HEALTH_TEMPERATURE_NORMAL:
        chg_en = CHG_POWER_EN;
        if(di->battery_voltage > HI6521_NORNAL_ICHRG_VOLTAGE){
            hi6521_calling_limit_ac_input_current(di,NORMAL_TEMP_CONFIG);
        } else {
            if(di->charger_source == POWER_SUPPLY_TYPE_MAINS){
                ichg_temp = CHG_FAST_ICHG_VALUE(800);
            }
        }
        break;
    case BATTERY_HEALTH_TEMPERATURE_NORMAL_HIGH:
        chg_en = CHG_POWER_EN;
        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,NORMAL_HIGH_TEMP_CONFIG);
        }
        break;
    case BATTERY_HEALTH_TEMPERATURE_HIGH:
        chg_en = CHG_POWER_EN;
#if 0
        if (di->battery_voltage <= HI6521_HIGH_TEMP_RECHARGE_VOLTAGE)
            di->chg_en = CHG_POWER_EN;
        else if (di->battery_voltage > HI6521_LOW_TEMP_NOT_CHARGING_VOLTAGE)
            di->chg_en = CHG_POWER_DIS;
        else
            di->chg_en = di->chg_en;
#endif

        if (di->battery_temp_status != battery_status){
            hi6521_calling_limit_ac_input_current(di,HIGH_TEMP_CONFIG);
        }
        break;
    case BATTERY_HEALTH_TEMPERATURE_OVERHIGH:
        chg_en = CHG_POWER_DIS;
        break;
    default:
        break;
    }

    di->battery_temp_status = battery_status;

   if(cin_limit_temp == 1){
        if(di->cin_limit != iin_temp){
            di->cin_limit = iin_temp;
        }
    }else{
        di->cin_limit = cin_limit_temp;
    }

    if(charge_current_temp == 1){
        if(di->chg_fast_ichg != ichg_temp){
            di->chg_fast_ichg = ichg_temp;
        }
    }else{
        di->chg_fast_ichg = charge_current_temp;
    }

    chg_en = (chg_en & di->factory_flag) | di->not_stop_chrg_flag;
#ifdef CONFIG_CHARGER_TYPE_RECHECK
   if((CHARGER_TYPE_NON_STANDARD == di->event))
    {
	    if(1 == work_first_in_since_vbus_intr)
		{
            di->cin_limit = IINLIM_500;
            di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(500);
		}
		else
		{
            di->cin_limit = IINLIM_1000;
            di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(900);
		}
    }
#endif

    if(CHARGER_TYPE_BC_USB == di->event)
    {
            di->cin_limit = IINLIM_1000;
            di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(900);
    }

    hi6521_config_input_source_reg(di);
    hi6521_config_fast_current_reg(di);
    hi6521_config_charger_enable_val(di,chg_en);
    batt_temp = hisi_battery_temperature();
    hi6521_read_byte(di, &read_reg, CHG_STATUS0);
    hwlog_info("ntc_charging batt_temp:%d, batt_volt:%d, iin:%d, ichg:%d, chg_en:%d, status0:0x%x\n", \
        batt_temp, di->battery_voltage, di->cin_limit, di->chg_fast_ichg, chg_en, read_reg);
    if((read_reg & CHG_STATUS0_STAT_CHAEGE_DONE) == CHG_STATUS0_STAT_CHAEGE_DONE)
        return;

    if(chg_en){
        events = VCHRG_START_CHARGING_EVENT;
    } else {
        events = VCHRG_NOT_CHARGING_EVENT;
    }
    hisi_coul_charger_event_rcv(events);//blocking_notifier_call_chain(&notifier_list, events, NULL);
    return;
}

#if 1 /*V8打桩*/
STATIC int hi6521_check_backup_battery_exist(struct hi6521_device_info *di)
{
	return 0;
}
#else
extern int hkadc_debug_init(int chanid);
extern int hkadc_debug_uninit(int chanid);
extern int hkadc_get_shared_value(int chanid, unsigned short* volt);

STATIC int hi6521_check_backup_battery_exist(struct hi6521_device_info *di)
{
    unsigned short volt = -1;
    int channel = HKADC9;

    hkadc_debug_init(channel);
    mdelay(1500);
    if (hkadc_get_shared_value(channel, &volt) != 0){
        hkadc_debug_uninit(channel);
        hwlog_err( "HKADC9 read error\n");
        return -1;
    }
    hkadc_debug_uninit(channel);
    if((volt >= BACKUP_EXIST_MIN_MV)&&(volt <= BACKUP_EXIST_MAX_MV))
        return 1;
    else
        return 0;
}

#endif
STATIC void hi6521_start_usb_charger(struct hi6521_device_info *di)
{
    unsigned int  events = VCHRG_START_USB_CHARGING_EVENT;
    int  timeout = 100;
    int chg_en=0;

    dbg_current_iin = 0;
    input_current_iin = di->max_cin_currentmA;
    
    di->wakelock_enabled = 1;
    if (di->wakelock_enabled){
        wake_lock(&chrg_lock);
    }

    if(1 == hi6521_check_backup_battery_exist(di)) {
        di->backup_battery_exist = true;
        hwlog_info("backup battery exist!");
    }

    while(HI6521_VBUS_STAT_UNKNOWM == hi6521_check_charger_present(di)) {
        if (!(timeout--)){
            hwlog_err("ERROR:charger buck isn't ok!\n");
            events = VCHRG_NOT_CHARGING_EVENT;
            break;
        }
        msleep(5);
    }

    //hisi_coul_charger_event_rcv(events);//blocking_notifier_call_chain(&notifier_list, events, NULL);
    di->charger_source = POWER_SUPPLY_TYPE_USB;
    //di->charge_status = POWER_SUPPLY_STATUS_CHARGING;
    hi6521_get_max_charge_voltage(di);
    di->cin_dpmmV = VINDPM_4520;
    if(di->backup_battery_exist) {
        di->cin_limit = IINLIM_1200;
        //pre_cin_limit = IINLIM_1200;
        di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(1000);
        di->charger_source = POWER_SUPPLY_TYPE_MAINS;
    } else {
        if(CHARGER_TYPE_BC_USB == di->event)
        {
            di->cin_limit = IINLIM_1000;
            di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(900);
        }
        else
        {
            di->cin_limit = IINLIM_500;
            di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(500);
        }
    }

    iin_temp = di->cin_limit;
    ichg_temp = di->chg_fast_ichg;
    di->chg_fast_vchg = di->max_voltagemV;
    di->chg_term_ichg = CHG_TERM_ICHG_150MA;
    di->chg_fastchg_safe_timer = CHG_FAST_SAFE_TIMER_12H;
    chg_en = CHG_POWER_EN;
    di->otg_en = CHG_POWER_DIS;
    di->chg_termination_ctrl = CHG_TERMINATION_DIS;
    di->batfet_ctrl = CHG_BATFET_EN;
    di->factory_flag = CHG_POWER_EN;
    di->calling_limit = 0;
    di->battery_temp_status = -1;
    di->charge_full_count = 0;

    msleep(500);
    hi6521_config_input_source_reg(di);
    hi6521_config_fast_current_reg(di);
    hi6521_config_fast_vol_and_term_current_reg(di);
    hi6521_config_fast_safe_timer_and_term_ctrl_reg(di);
    hi6521_config_otg_enable(di);
    hi6521_config_charger_enable_val(di,chg_en);

    schedule_delayed_work(&di->hi6521_charger_work, msecs_to_jiffies(0));
#if DPM_CHECK_FOR_V204
    schedule_delayed_work(&di->hi6521_dpm_check_work, msecs_to_jiffies(0));
#endif
    hwlog_info("%s, ---->START USB CHARGING, \n"
        "battery current = %d mA\n"
        "battery voltage = %d mV\n"
        "cin_limit_current = %d mA\n"
        , __func__, di->chg_fast_ichg, di->chg_fast_vchg,di->cin_limit);

    di->battery_present = is_hisi_battery_exist();
    if (!di->battery_present) {
        dev_dbg(di->dev, "BATTERY NOT DETECTED!\n");
        /*battry is not present ,notify scharger ic.*/
        hi6521_write_byte(di, CHG_BATTRY_NOT_PRESENT, CHG_REG10);
        events = VCHRG_NOT_CHARGING_EVENT;
    }
    else{
        hi6521_write_byte(di, CHG_BATTRY_PRESENT, CHG_REG10);
    }
    hisi_coul_charger_event_rcv(events);//blocking_notifier_call_chain(&notifier_list, events, NULL);

    return;
}

STATIC void hi6521_start_ac_charger(struct hi6521_device_info *di)
{
    unsigned int  events = VCHRG_START_AC_CHARGING_EVENT;
	int timeout= 100;
    int chg_en=0;

    dbg_current_iin = 0;
    input_current_iin = di->max_cin_currentmA;
    
    di->wakelock_enabled = 1;
    if (di->wakelock_enabled){
        wake_lock(&chrg_lock);
    }
	while(HI6521_VBUS_STAT_UNKNOWM == hi6521_check_charger_present(di)) {
		if (!(timeout--)){
			hwlog_err("ERROR:charger buck isn't ok!\n");
                    events = VCHRG_NOT_CHARGING_EVENT;
			break;
		}
	    msleep(5);
	}
#ifdef CONFIG_CHARGER_TYPE_RECHECK
   if(di->event == CHARGER_TYPE_NON_STANDARD)
    {
        di->cin_limit = IINLIM_500;
        di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(500);
    }
    else
    {
        di->cin_limit = di->max_cin_currentmA;
        di->chg_fast_ichg = di->max_currentmA;
    }
#else
    di->cin_limit = di->max_cin_currentmA;
    di->chg_fast_ichg = di->max_currentmA;
#endif
    di->charger_source = POWER_SUPPLY_TYPE_MAINS;
    hi6521_get_max_charge_voltage(di);
    di->cin_dpmmV = VINDPM_4520;
    di->chg_fast_vchg = di->max_voltagemV;
    iin_temp = di->cin_limit;
    ichg_temp = di->chg_fast_ichg;
    di->chg_term_ichg = CHG_TERM_ICHG_150MA;
    di->chg_fastchg_safe_timer = CHG_FAST_SAFE_TIMER_8H;

    chg_en = CHG_POWER_EN;
	di->otg_en = CHG_POWER_DIS;
    di->chg_termination_ctrl = CHG_TERMINATION_DIS;
    di->batfet_ctrl = CHG_BATFET_EN;
    di->factory_flag = CHG_POWER_EN;
    di->calling_limit = 0;
    di->battery_temp_status = -1;
    di->charge_full_count = 0;

    msleep(500);
    hi6521_config_input_source_reg(di);
    hi6521_config_fast_current_reg(di);
    hi6521_config_fast_vol_and_term_current_reg(di);
    hi6521_config_fast_safe_timer_and_term_ctrl_reg(di);
	hi6521_config_otg_enable(di);
    hi6521_config_charger_enable_val(di,chg_en);


    schedule_delayed_work(&di->hi6521_charger_work, msecs_to_jiffies(0));
#if DPM_CHECK_FOR_V204
    schedule_delayed_work(&di->hi6521_dpm_check_work, msecs_to_jiffies(0));
#endif
    hwlog_info("%s, ---->START AC CHARGING, \n"
                         "battery current = %d mA\n"
                         "battery voltage = %d mV\n"
                         "cin_limit_current = %d mA\n"
						 "charge_type = %lu\n"
                         , __func__, di->chg_fast_ichg, di->chg_fast_vchg,di->cin_limit,di->event);

    di->battery_present = is_hisi_battery_exist();
    if (!di->battery_present) {
            dev_dbg(di->dev, "BATTERY NOT DETECTED!\n");
			/*battry is not present ,notify scharger ic.*/
			hi6521_write_byte(di, CHG_BATTRY_NOT_PRESENT, CHG_REG10);
            events = VCHRG_NOT_CHARGING_EVENT;
    }
	else{
		hi6521_write_byte(di, CHG_BATTRY_PRESENT, CHG_REG10);
	}
    hisi_coul_charger_event_rcv(events);//blocking_notifier_call_chain(&notifier_list, events, NULL);

    return;
}
/****************************************************************************
  Function:     get_time_tick
  Description:  get the current time.
  Input:        timestamp_buf
  Output:       NA
  Return:       NA.
***************************************************************************/
static void get_time_tick(char* timestamp_buf,unsigned int len)
{
   struct timeval tv;
   struct rtc_time tm;

   if(NULL == timestamp_buf) {
       return;
   }
   memset(&tv, 0, sizeof(struct timeval));
   memset(&tm, 0, sizeof(struct rtc_time));
   do_gettimeofday(&tv);
   tv.tv_sec -= sys_tz.tz_minuteswest * 60;
   rtc_time_to_tm(tv.tv_sec, &tm);
   snprintf(timestamp_buf,len, "%04d%02d%02d%02d%02d%02d",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}
/****************************************************************************
  Function:     record_nff_for_otg_setup
  Description:  record the charge type which is otg  in nff log
                just record one times otg setup to nff log for this boot
  Input:        NA
  Output:       NA
  Return:       NA.
***************************************************************************/
static void record_nff_for_otg_setup(void)
{
    int ret = 0;
    static int nff_record_time = 0;
    char cmd[40]={0};
    char time_buf[16]={0};
    if(nff_record_time == 0)
    {
     get_time_tick(time_buf,16);
     snprintf(cmd, 40, "%s %s\n",time_buf,"chg type:otg-mode ");
     ret=log_to_exception("messagestorage",cmd);
     nff_record_time=1;
    }


  }
STATIC void hi6521_start_usb_otg(struct hi6521_device_info *di)
{
    hwlog_info("%s,---->USB_EVENT_OTG_ID<----\n", __func__);

    int chg_en=0;

    di->wakelock_enabled = 1;
    wake_lock(&chrg_lock);

    di->otg_en = CHG_POWER_EN;
	//di->otg_force_en = CHG_OTG_FORCE_DIS;
    chg_en = CHG_POWER_DIS;
	hi6521_config_charger_enable_val(di,chg_en);
    hi6521_config_otg_enable(di);
    record_nff_for_otg_setup();

    schedule_delayed_work(&di->hi6521_usb_otg_work, msecs_to_jiffies(0));
    return;
}

STATIC void hi6521_stop_charger(struct hi6521_device_info *di)
{
    unsigned int  events  = VCHRG_STOP_CHARGING_EVENT;
    int chg_en=0;

    if (!wake_lock_active(&chrg_lock)){
        wake_lock(&chrg_lock);
    }

	#if DPM_CHECK_FOR_V204
    cancel_delayed_work_sync(&di->hi6521_dpm_check_work);
    #endif
    cancel_delayed_work_sync(&di->hi6521_charger_work);
    hisi_coul_charger_event_rcv(events);//blocking_notifier_call_chain(&notifier_list, events, NULL);

    hwlog_info("%s,---->STOP CHARGING\n", __func__);
    di->charger_source = POWER_SUPPLY_TYPE_BATTERY;

    di->batfet_ctrl = CHG_BATFET_EN;
    di->calling_limit = 0;
    di->factory_flag = 0;
    di->battery_temp_status = -1;
    chg_en = CHG_POWER_DIS;
	di->otg_en = CHG_POWER_DIS;
    di->charge_full_count = 0;
	di->backup_battery_exist = false;

    hi6521_config_charger_enable_val(di,chg_en);
	hi6521_config_otg_enable(di);
    hi6521_config_input_source_reg(di);

    cancel_delayed_work_sync(&di->hi6521_usb_otg_work);
    msleep(1000);

    di->wakelock_enabled = 1;
    if (di->wakelock_enabled){
        wake_lock_timeout(&stop_chrg_lock, HZ);
        wake_unlock(&chrg_lock);
    }

    wakeup_timer_seconds = 0;
#ifdef CONFIG_CHARGER_TYPE_RECHECK
    work_first_in_since_vbus_intr = 1;
#endif
	return;
}

STATIC void hi6521_check_bq27510_charge_full(struct hi6521_device_info *di)
{
    if(di->battery_present){

        if(di->battery_full){
            di->chg_termination_ctrl = CHG_TERMINATION_EN;
            di->charge_full_count++;
            if(di->charge_full_count >= 20){
               di->charge_full_count = 20;
            }
        }else{
            di->chg_termination_ctrl = CHG_TERMINATION_DIS;
        }
		hi6521_write_byte(di, CHG_BATTRY_PRESENT, CHG_REG10);
    }else{
        di->chg_termination_ctrl = CHG_TERMINATION_EN;
		hi6521_write_byte(di, CHG_BATTRY_NOT_PRESENT, CHG_REG10);
    }

    hi6521_config_fast_safe_timer_and_term_ctrl_reg(di);
    return;
}



STATIC void hi6521_charger_done_release_wakelock(struct hi6521_device_info *di)
{
    if(di->charger_source == POWER_SUPPLY_TYPE_MAINS){
		if (!di->battery_present){
			/*battry is not present ,notify scharger ic.*/
			hi6521_write_byte(di, CHG_BATTRY_NOT_PRESENT, CHG_REG10);
			return;
		}
		else{
			hi6521_write_byte(di, CHG_BATTRY_PRESENT, CHG_REG10);
		}
        if(di->battery_full){
            if (wake_lock_active(&chrg_lock)){
                if(di->charge_full_count >= 20){
                    wake_unlock(&chrg_lock);
                    hwlog_err( "ac charge done wakelock release\n");
                }
            }
        }else{
            if (!wake_lock_active(&chrg_lock)){
                wake_lock(&chrg_lock);
                hwlog_err( "ac recharge wakelock add again\n");
            }
        }
    }
    return;
}

STATIC void hi6521_charger_update_status(struct hi6521_device_info *di)
{
    unsigned int  events  = 0;
    int chg_en=0;
    u8 read_reg[3] = {0};
#if defined (CONFIG_HUAWEI_DSM)
    int dsm_charger_error_found = -1;
#endif

    //hi6521_read_block(di, &read_reg[0], 0, 11);
    hi6521_read_byte(di,&read_reg[0],CHG_STATUS0);
	hi6521_read_byte(di,&read_reg[1],CHG_STATUS1);
	hi6521_read_byte(di,&read_reg[2],CHG_STATUS5);

       /*l00279044 scharger mntn add start*/
    if ((read_reg[0] & CHG_STATUS0_STAT_MASK_BIT) == CHG_STATUS0_STAT_CHAEGE_DONE){
        hwlog_info( "Charge Done\n");
        events  = VCHRG_CHARGE_DONE_EVENT;
        hisi_coul_charger_event_rcv(events);
    }
    if ((read_reg[0] & CHG_STATUS0_STAT_MASK_BIT) == CHG_STATUS0_STAT_NOT_CHARGING)
        hwlog_info( "Not_Charging \n");

    if ((read_reg[0] & CHG_STATUS0_STAT_MASK_BIT) == CHG_STATUS0_STAT_PRE_CHARGING)
        hwlog_info( "Pre_charge\n");

    if ((read_reg[0] & CHG_STATUS0_STAT_MASK_BIT) == CHG_STATUS0_STAT_FAST_CHARGING)
        hwlog_info( "Fast_charge\n");

    if ((read_reg[0] & CHG_STATUS0_BUCK_VSYS_STAT) == CHG_STATUS0_BUCK_VSYS_STAT)
        hwlog_info( "IN VSYSMIN regulation\n");

       /*l00279044 scharger mntn add end*/

#if defined (CONFIG_HUAWEI_DSM)
	if (!dsm_client_ocuppy(charger_dclient)) {
		dsm_charger_error_found++;
	}
#endif
    if ((1 == vbus_ovp_flag) && (!(read_reg[1] & ~HI6521_VBUS_OVP_BITMASK)))
    {
        hi6521_config_fast_current_reg(di);
        chg_en = CHG_POWER_EN;
        hi6521_config_charger_enable_val(di,chg_en);
        events  = VCHRG_START_CHARGING_EVENT;
        hisi_coul_charger_event_rcv(events);
        vbus_ovp_flag = 0;
        hwlog_info("....................The vbus voltage recover!\n");
    }

    if ((read_reg[1] & CHG_STATUS1_VBUS_OVP_6P5V) == CHG_STATUS1_VBUS_OVP_6P5V){
        hwlog_err( "VBUS_OVP_6.5v\n");
        events  = VCHRG_POWER_SUPPLY_OVERVOLTAGE;
        hisi_coul_charger_event_rcv(events);
        vbus_ovp_flag = 1;

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]POWER_SUPPLY_VBUS_OVP_6.5v,reg[0x02]:0x%x.\n",
                            __func__,read_reg[1] );
#endif
    }

       /*l00279044 scharger mntn add start*/
    if ((read_reg[1] & CHG_STATUS1_OTMP_JREG) == CHG_STATUS1_OTMP_JREG){
        hwlog_err( "POWER_SUPPLY_OVER_TEMP_Jreg,reg[0x02] = 0x%x\n", read_reg[1]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]POWER_SUPPLY_OVER_TEMP_Jreg,reg[0x02]:0x%x.\n",
                            __func__,read_reg[1] );
#endif
    }

    if ((read_reg[1] & CHG_STATUS1_OTMP_150) == CHG_STATUS1_OTMP_150){
        hwlog_err( "POWER_SUPPLY_OVER_TEMP_150C,reg[0x02] = 0x%x\n", read_reg[1]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]POWER_SUPPLY_OVER_TEMP_150C,reg[0x02]:0x%x.\n",
                            __func__,read_reg[1] );
#endif
    }
       /*l00279044 scharger mntn add end*/

#if defined (CONFIG_HUAWEI_DSM)
    if (dsm_charger_error_found > 0) {
        dsm_client_notify(charger_dclient, DSM_CHARGER_ERROR_NO);
    } else if (!dsm_charger_error_found) {
    	dsm_client_unocuppy(charger_dclient);
    }
#endif

	/*before feed wdt,judge wdt status*/
	if (0 == hi6521_get_chgwdt_status(di))
	{
		di->watchdog_timer = CHG_WDT_TIMER_40S;
		hi6521_config_chgwdt_timer_reg(di);
		hwlog_err("chgwdt is timeout,but status is no change!\n");
	}

    /* reset 30 second timer */
	di->wd_rst_n         = CHG_WDT_RST;
	di->flash_led_wd_rst = CHG_FLASH_LED_NO_RST;
	di->soft_rst_n       = CHG_SOFT_NO_RST;
    hi6521_config_chgwdt_soft_torchwdt_rst_reg(di);
}

#ifdef CONFIG_CHARGER_TYPE_RECHECK
extern int hiusb_charger_detect_charger_type(void);
STATIC void hi6521_update_charge_current(struct hi6521_device_info *di)
{
    unsigned int  events = VCHRG_START_CHARGING_EVENT;

    if(NULL == di)
    {
        hwlog_err("slowinsertusb update_charge_current, di is NULL!!!\n");
        return;
    }

    switch(di->event)
    {
        case CHARGER_TYPE_USB:
            events = VCHRG_START_USB_CHARGING_EVENT;
            di->charger_source = POWER_SUPPLY_TYPE_USB;
            if(di->backup_battery_exist)
            {
                di->charger_source = POWER_SUPPLY_TYPE_MAINS;
                di->cin_limit = IINLIM_1200;
                di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(1000);
                hwlog_info("slowinsertusb attention backup_battery_exist!\n");
            }else
            {
                di->cin_limit = IINLIM_500;
                di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(500);
            }
            break;
        case CHARGER_TYPE_NON_STANDARD:
            events = VCHRG_START_AC_CHARGING_EVENT;
            di->charger_source = POWER_SUPPLY_TYPE_MAINS;
            if(di->backup_battery_exist)
            {
                di->charger_source = POWER_SUPPLY_TYPE_MAINS;
                di->cin_limit = IINLIM_1200;
                di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(1000);
                hwlog_info("slowinsertusb attention backup_battery_exist!\n");
            }else
            {
                di->cin_limit = IINLIM_1000;
                di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(900);
            }
            break;
        case CHARGER_TYPE_BC_USB:
        case CHARGER_TYPE_STANDARD:
            events = VCHRG_START_AC_CHARGING_EVENT;
            di->charger_source = POWER_SUPPLY_TYPE_MAINS;
            di->cin_limit = di->max_cin_currentmA;
            di->chg_fast_vchg = di->max_voltagemV;
            break;
        default:
            hwlog_info("slowinsertusb update_charge_current, unknown charger type:%lu!\n", di->event);
            break;
    }

    iin_temp = di->cin_limit;
    ichg_temp = di->chg_fast_ichg;
    hi6521_config_input_source_reg(di);
    hi6521_config_fast_current_reg(di);
    if(VCHRG_START_CHARGING_EVENT != events)
    {
        hisi_coul_charger_event_rcv(events);
    }
    return;
}
#endif
STATIC void hi6521_charger_work(struct work_struct *work)
{
    struct hi6521_device_info *di = container_of(work,
                 struct hi6521_device_info, hi6521_charger_work.work);
#ifdef CONFIG_CHARGER_TYPE_RECHECK
    int charger_type = CHARGER_TYPE_NON_STANDARD;
    int usb_enum = 0;
#endif

    di->battery_present = is_hisi_battery_exist();
	if (!di->battery_present){
		/*battry is not present ,notify scharger ic.*/
		hi6521_write_byte(di, CHG_BATTRY_NOT_PRESENT, CHG_REG10);
	}
	else{
		hi6521_write_byte(di, CHG_BATTRY_PRESENT, CHG_REG10);
	}

    di->battery_full = is_hisi_battery_full();

#ifdef CONFIG_CHARGER_TYPE_RECHECK
    usb_enum = getChargerUsbState();
    //hwlog_info( "slowinsertusb work_first_in_since_vbus_intr:%d, charger type:%d, usb_enum:%d\n", work_first_in_since_vbus_intr, di->event, usb_enum);
    if(!work_first_in_since_vbus_intr && (CHARGER_TYPE_NON_STANDARD == di->event) && !usb_enum)
    {
        charger_type = hiusb_charger_detect_charger_type();
        if(charger_type != di->event)
        {
            hwlog_info( "slowinsertusb new charger type:%d, old charger type:%lu\n", charger_type, di->event);
            di->event = charger_type;
        }
        hi6521_update_charge_current(di);

    }
	else if((CHARGER_TYPE_NON_STANDARD == di->event) && usb_enum)
    {
         di->event = CHARGER_TYPE_USB;
         hi6521_update_charge_current(di);
    }
#endif

    hi6521_monitor_battery_ntc_charging(di);
#ifdef CONFIG_CHARGER_TYPE_RECHECK
    work_first_in_since_vbus_intr = 0;
#endif

    hi6521_reset_vindpm(di);

    hi6521_check_bq27510_charge_full(di);

    hi6521_charger_update_status(di);

    hi6521_charger_done_release_wakelock(di);

    schedule_delayed_work(&di->hi6521_charger_work,
                 msecs_to_jiffies(CHG_TASK_PERIOD_30S));
}
#if DPM_CHECK_FOR_V204

static int dpm_check_current_threshold_ma = 40;
module_param(dpm_check_current_threshold_ma, int, 0644);

static int dpm_switch_delay_time_ms = 200;
module_param(dpm_switch_delay_time_ms, int, 0644);

static int dpm_check_delay_time_ms = 1000;
module_param(dpm_check_delay_time_ms, int, 0644);

static int dpm_switch_with_charge_stop = 1;
module_param(dpm_switch_with_charge_stop, int, 0644);


STATIC void hi6521_dpm_check_work(struct work_struct *work)
{
    int current_ma;
	unsigned char chg_en_status = 0;
    struct hi6521_device_info *di = container_of(work,
                 struct hi6521_device_info, hi6521_dpm_check_work.work);

    current_ma = hisi_battery_current();
    if ((current_ma > dpm_check_current_threshold_ma) || (dpm_switch_enable)){
        unsigned char reg;
        hwlog_info("hi6521_dpm_check_work cur=%dmA do dpm switch\n", current_ma);

        if (dpm_switch_with_charge_stop){
            down_interruptible(&di->charger_data_busy_lock);
			chg_en_status = di->chg_en;
            di->chg_en = CHG_POWER_DIS;
            hi6521_config_charger_nolock_enable(di);
            hi6521_read_byte(di,&reg, BUCK_REG5_REG);
            reg = reg|0x08;
    		hi6521_write_byte(di, reg, BUCK_REG5_REG);
            mdelay(dpm_switch_delay_time_ms);
            reg = reg&(~0x08);
    		hi6521_write_byte(di, reg, BUCK_REG5_REG);
			di->chg_en = chg_en_status;
            hi6521_config_charger_nolock_enable(di);
            up(&di->charger_data_busy_lock);
        }
        else{
            hi6521_read_byte(di,&reg, BUCK_REG5_REG);
            reg = reg|0x08;
    		hi6521_write_byte(di, reg, BUCK_REG5_REG);
            mdelay(dpm_switch_delay_time_ms);
            reg = reg&(~0x08);
    		hi6521_write_byte(di, reg, BUCK_REG5_REG);
        }
    }

    schedule_delayed_work(&di->hi6521_dpm_check_work,
                 msecs_to_jiffies(dpm_check_delay_time_ms));
}
#endif
void hi6521_lcd_ldo_clear_int(void)
{
      u8 reg =0;
      struct hi6521_device_info *di;
      di = g_scharger_dev;

      hi6521_read_byte(di,&reg, CHG_IRQ_REG3);
      reg = reg | HI6521_LCD_LDO_OCP_CLEAR_BIT;
      hi6521_write_byte(di, reg, CHG_IRQ_REG3);
      hwlog_info("clear lcd ldo ocp reg is %d\n", reg);
      return;
}
EXPORT_SYSMOL_GPL(hi6521_lcd_ldo_clear_int);

STATIC void hi6521_usb_otg_work(struct work_struct *work)
{
	u8 read_reg =0;
	struct hi6521_device_info *di = container_of(work,
		struct hi6521_device_info, hi6521_usb_otg_work.work);

#if defined (CONFIG_HUAWEI_DSM)
    int dsm_charger_error_found = -1;
    if (!dsm_client_ocuppy(charger_dclient))
        dsm_charger_error_found++;
#endif

    hi6521_read_byte(di,&read_reg,CHG_STATUS0);
    if ((read_reg & CHG_STATUS0_OTG_ON) != CHG_STATUS0_OTG_ON)
        hwlog_info( "OTG_off \n");

    hi6521_read_byte(di,&read_reg,CHG_STATUS5);
    if ((read_reg & CHG_STATUS5_OTG_OVP_5P3V) == CHG_STATUS5_OTG_OVP_5P3V){
        hwlog_err( "OTG_OVP_5P3V,reg[0x06]:0x%x.\n",read_reg);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]OTG_OVP_5P3V,reg[0x06]:0x%x.\n",
                    __func__,read_reg);
#endif
    }

#if defined (CONFIG_HUAWEI_DSM)
    if (dsm_charger_error_found > 0) {
        dsm_client_notify(charger_dclient, DSM_CHARGER_ERROR_NO);
    } else if (!dsm_charger_error_found) {
		dsm_client_unocuppy(charger_dclient);
	}
#endif
    /* reset 30 second timer */
	di->wd_rst_n         = CHG_WDT_RST;
	di->flash_led_wd_rst = CHG_FLASH_LED_NO_RST;
	di->soft_rst_n       = CHG_SOFT_NO_RST;
    hi6521_config_chgwdt_soft_torchwdt_rst_reg(di);

    schedule_delayed_work(&di->hi6521_usb_otg_work,
                 msecs_to_jiffies(CHG_TASK_PERIOD_30S));
}

STATIC void hi6521_usb_charger_work(struct work_struct *work)
{
    struct hi6521_device_info	*di =
              container_of(work, struct hi6521_device_info, usb_work);

    if(di->usb_shell_gpio_valid)
    {
        switch(di->event)
        {
            int ret = 0;

            case CHARGER_TYPE_USB:
            case CHARGER_TYPE_NON_STANDARD:
            case CHARGER_TYPE_BC_USB:
            case CHARGER_TYPE_STANDARD:
                ret = gpio_request(di->usb_shell_gpio, "usb_shell_gpio");
                if(ret)
                {
                    hwlog_err("usbshellgpio could not request usb shell to gnd gpio!\n");
                }
                else
                {
                    gpio_direction_output(di->usb_shell_gpio, 1);
                    gpio_set_value(di->usb_shell_gpio, 1);
                    hwlog_info("usbshellgpio set usb_shell_gpio to high!\n");
                }
                gpio_free(di->usb_shell_gpio);
                break;
            case CHARGER_REMOVED:
                ret = gpio_request(di->usb_shell_gpio, "usb_shell_gpio");
                if(ret)
                {
                    hwlog_err("usbshellgpio could not request usb shell to gnd gpio!\n");
                }
                else
                {
                    gpio_direction_output(di->usb_shell_gpio, 0);
                    gpio_set_value(di->usb_shell_gpio, 0);
                    hwlog_info("usbshellgpio set usb_shell_gpio to low!\n");
                }
                gpio_free(di->usb_shell_gpio);
                break;
            default:
                hwlog_info("usbshellgpio unkown charger type!\n");
                break;
        }
    }

    switch (di->event) {
    case CHARGER_TYPE_USB: /*SDP*/
        hwlog_info("case = CHARGER_TYPE_USB-> \n");
        hi6521_start_usb_charger(di);
        break;
    case CHARGER_TYPE_NON_STANDARD:/*SDP*/
        hwlog_info("case = CHARGER_TYPE_NON_STANDARD -> \n");
#ifdef CONFIG_CHARGER_TYPE_RECHECK
        hi6521_start_ac_charger(di);
#else
        hi6521_start_usb_charger(di);
#endif
        break;
    case CHARGER_TYPE_BC_USB:/*CDP*/
        hwlog_info("case = CHARGER_TYPE_BC_USB -> \n");
        hi6521_start_usb_charger(di);
        break;
    case CHARGER_TYPE_STANDARD:/*DCP*/
        hwlog_info("case = CHARGER_TYPE_STANDARD\n");
        hi6521_start_ac_charger(di);
        break;
    case CHARGER_REMOVED:
        hwlog_info("case = USB_EVENT_NONE\n");
        hi6521_stop_charger(di);
        break;
    case USB_EVENT_OTG_ID:
        hwlog_info("case = USB_EVENT_OTG_ID\n");
        hi6521_start_usb_otg(di);
        break;
    default:
        break;
    }
}


STATIC irqreturn_t hi6521_irq_handle(int irq, void *data)
{
	struct hi6521_device_info *di = (struct hi6521_device_info *)data;
    lcd_ldo_ocp_time = get_jiffies_64();
	queue_work(di->scharger_int_workqueue,&di->hi6521_irq_work);
	disable_irq_nosync(di->irq);
	return IRQ_HANDLED;

}
STATIC void hi6521_irq_work_handle(struct work_struct *work)
{
    struct hi6521_device_info *di = container_of(work, struct hi6521_device_info, hi6521_irq_work);
    unsigned char irq_status[5] = {0};
    STATIC u8 flash_bst_scp_cnt = 0;
    STATIC u8 otg_scp_cnt = 0;
    STATIC u8 otg_uvp_cnt = 0;
    STATIC u8 reg_read = 0;

    static int lcd_ldo_ocp_count = 0;
#if defined (CONFIG_HUAWEI_DSM)
		int dsm_charger_error_found = -1;
#endif


	/*if (WARN(di != scharger_di,
		 "hi6521_irq_work_handle di is error di =0x%x,scharger_di=0x%x\n",di, scharger_di))
	{
		return 0;
	}*/
	/*save irq status*/
	hi6521_read_byte(di,&irq_status[0], CHG_IRQ_REG0);
	hi6521_read_byte(di,&irq_status[1], CHG_IRQ_REG1);
	hi6521_read_byte(di,&irq_status[2], CHG_IRQ_REG2);
	hi6521_read_byte(di,&irq_status[3], CHG_IRQ_REG3);
	hi6521_read_byte(di,&irq_status[4], CHG_IRQ_REG4);

#if defined (CONFIG_HUAWEI_DSM)
    if (!dsm_client_ocuppy(charger_dclient))
        dsm_charger_error_found++;
#endif


	    /*handle irq*/
       /*l00279044 scharger mntn add start*/
    if (irq_status[0] & CHG_IRQ0_VBUS_FAST_CHG_FAULT){
        hwlog_err("SCHARGER ERROR:FAST_CHG_FAULT!,reg[0x07]:0x%x.\n",irq_status[0]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]FAST_CHG_FAULT,reg[0x07]:0x%x.\n",
                      __func__,irq_status[0] );
#endif
    }

    if (irq_status[0] & CHG_IRQ0_VBUS_PRE_CHG_FAULT){
        hwlog_err("SCHARGER ERROR:PRE_CHG_FAULT!,reg[0x07]:0x%x.\n",irq_status[0]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]PRE_CHG_FAULT,reg[0x07]:0x%x.\n",
                      __func__,irq_status[0] );
#endif
    }

    if (irq_status[0] & CHG_IRQ0_VBUS_TRICKLE_CHG_FAULT){
        hwlog_err("SCHARGER ERROR:TRICKLE_CHG_FAULT!,reg[0x07]:0x%x.\n",irq_status[0]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]TRICKLE_CHG_FAULT,reg[0x07]:0x%x.\n",
                      __func__,irq_status[0] );
#endif
    }

    if (irq_status[1] & CHG_IRQ1_BUCK_SCP){
        hwlog_err("SCHARGER ERROR:BUCK_SCP,reg[0x08]:0x%x.\n",irq_status[1]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]BUCK_SCP,reg[0x07]:0x%x.",
                      __func__,irq_status[1] );
#endif
    }

    if (irq_status[1] & CHG_IRQ1_WLED_OVP_38V){
        di->wled_en = CHG_POWER_DIS;
        hi6521_config_wled_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ1_WLED_OVP_38V,reg[0x08]:0x%x.\n",irq_status[1]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]WLED_OVP_38V,reg[0x08]:0x%x.\n",
                      __func__,irq_status[1] );
#endif
    }

    if (irq_status[1] & CHG_IRQ1_WLED_OVP_4P5V){
        di->wled_en = CHG_POWER_DIS;
        hi6521_config_wled_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ1_WLED_OVP_4P5V,reg[0x08]:0x%x.\n",irq_status[1]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]WLED_OVP_4P5V,reg[0x08]:0x%x.\n",
                      __func__,irq_status[1] );
#endif
    }

    if (irq_status[1] & CHG_IRQ1_WLED_UVP){
        di->wled_en = CHG_POWER_DIS;
        hi6521_config_wled_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ1_WLED_UVP,reg[0x08]:0x%x.\n",irq_status[1]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]WLED_UVP,reg[0x08]:0x%x.\n",
                      __func__,irq_status[1] );
#endif
    }

    if (irq_status[1] & CHG_IRQ1_WLED_SCP){
        di->wled_en = CHG_POWER_DIS;
        hi6521_config_wled_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ1_WLED_SCP,reg[0x08]:0x%x.\n",irq_status[1]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]WLED_SCP,reg[0x08]:0x%x.\n",
                      __func__,irq_status[1] );
#endif
    }

    if (irq_status[2] & CHG_IRQ2_FLASH_BST_UVP){
        di->flash_bst_en = CHG_POWER_DIS;
        hi6521_config_flash_boost_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ2_FLASH_BST_UVP,reg[0x09]:0x%x.\n",irq_status[2]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]FLASH_BST_UVP,reg[0x09]:0x%x.\n",
                      __func__,irq_status[2] );
#endif
    }

    if (irq_status[2] & CHG_IRQ2_FLASH_BST_OVP){
        di->flash_bst_en = CHG_POWER_DIS;
        hi6521_config_flash_boost_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ2_FLASH_BST_OVP,reg[0x09]:0x%x.\n",irq_status[2]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]FLASH_BST_OVP,reg[0x09]:0x%x.\n",
                      __func__,irq_status[2] );
#endif
    }

    if (irq_status[2] & CHG_IRQ2_FLASH_BST_SCP){
    /*To avoid Scharger v210 chip problem*/
        flash_bst_scp_cnt++;
        if (flash_bst_scp_cnt > 3){
            di->flash_bst_en = CHG_POWER_DIS;
            hi6521_config_flash_boost_enable(di);
            hwlog_err("SCHARGER ERROR:CHG_IRQ2_FLASH_BST_SCP,reg[0x09]:0x%x.\n",irq_status[2]);
            flash_bst_scp_cnt = 0;

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]FLASH_BST_SCP,reg[0x09]:0x%x.\n",
                      __func__,irq_status[2] );
#endif
        }
    }

    if (irq_status[2] & CHG_IRQ2_FLASH_LED_SHORT){
        di->flash_led_flash_en = CHG_POWER_DIS;
        di->flash_led_torch_en = CHG_POWER_DIS;
        hi6521_config_flash_led_flash_enable(di);
        hi6521_config_flash_led_torch_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ2_FLASH_LED_SHORT,reg[0x09]:0x%x.\n",irq_status[2]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]FLASH_LED_SHORT,reg[0x09]:0x%x.\n",
                      __func__,irq_status[2] );
#endif
    }

    if (irq_status[2] & CHG_IRQ2_FLASH_LED_OPEN){
        di->flash_led_flash_en = CHG_POWER_DIS;
        di->flash_led_torch_en = CHG_POWER_DIS;
        hi6521_config_flash_led_flash_enable(di);
        hi6521_config_flash_led_torch_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ2_FLASH_LED_OPEN,reg[0x09]:0x%x.\n",irq_status[2]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]FLASH_LED_OPEN,reg[0x09]:0x%x.\n",
                      __func__,irq_status[2] );
#endif
    }

    if (irq_status[3] & CHG_IRQ3_LCD_NCP_SCP){
        di->lcd_bst_en = CHG_POWER_DIS;
        di->lcd_ncp_en = CHG_POWER_DIS;
        di->lcd_ldo_en = CHG_POWER_DIS;
        hi6521_config_lcd_ncp_enable(di);
        hi6521_config_lcd_ldo_enable(di);
        hi6521_config_lcd_boost_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ3_LCD_NCP_SCP,reg[0x0A]:0x%x.\n",irq_status[3]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]LCD_NCP_SCP,reg[0x0A]:0x%x.\n",
                      __func__,irq_status[3] );
#endif
    }

    if (irq_status[3] & CHG_IRQ3_LCD_BST_UVP){
        di->lcd_bst_en = CHG_POWER_DIS;
        di->lcd_ncp_en = CHG_POWER_DIS;
        di->lcd_ldo_en = CHG_POWER_DIS;
        hi6521_config_lcd_ncp_enable(di);
        hi6521_config_lcd_ldo_enable(di);
        hi6521_config_lcd_boost_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ3_LCD_BST_UVP,reg[0x0A]:0x%x.\n",irq_status[3]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]LCD_BST_UVP,reg[0x0A]:0x%x.\n",
                      __func__,irq_status[3] );
#endif
    }

    if (irq_status[3] & CHG_IRQ3_LCD_BST_OVP){
        di->lcd_bst_en = CHG_POWER_DIS;
        di->lcd_ncp_en = CHG_POWER_DIS;
        di->lcd_ldo_en = CHG_POWER_DIS;
        hi6521_config_lcd_ncp_enable(di);
        hi6521_config_lcd_ldo_enable(di);
        hi6521_config_lcd_boost_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ3_LCD_BST_OVP,reg[0x0A]:0x%x.\n",irq_status[3]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]LCD_BST_OVP,reg[0x0A]:0x%x.\n",
                      __func__,irq_status[3] );
#endif
    }

    if (irq_status[3] & CHG_IRQ3_LCD_BST_SCP){
        hi6521_read_byte(di, &reg_read, SOC_SCHARGER_LCD_BOOST_REG10_ADDR(0));
        if (0x55 == reg_read){
            hi6521_write_byte(di, 0x54, SOC_SCHARGER_LCD_BOOST_REG10_ADDR(0));
            di->lcd_bst_en = CHG_POWER_DIS;
            di->lcd_ncp_en = CHG_POWER_DIS;
            di->lcd_ldo_en = CHG_POWER_DIS;
            hi6521_config_lcd_ncp_enable(di);
            hi6521_config_lcd_ldo_enable(di);
            hi6521_config_lcd_boost_enable(di);
            hi6521_write_byte(di, 0x55, SOC_SCHARGER_LCD_BOOST_REG10_ADDR(0));
        }
        hwlog_err("SCHARGER ERROR:CHG_IRQ3_LCD_BST_SCP,reg[0x0A]:0x%x.\n",irq_status[3]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]LCD_BST_SCP,reg[0x0A]:0x%x.\n",
                      __func__,irq_status[3] );
#endif
    }

    if (irq_status[3] & CHG_IRQ3_LCD_LDO_OCP){
        hwlog_err("SCHARGER ERROR:CHG_IRQ3_LDO_OCP,reg[0x0A]:0x%x;lcd ocp count %d;open lcd ldo time %lu;lcd ldo ocp time %lu;open to ocp time %u\n",\
                    irq_status[3],lcd_ldo_ocp_count,lcd_ldo_open_time,lcd_ldo_ocp_time,jiffies_to_msecs(lcd_ldo_ocp_time - lcd_ldo_open_time));

        if(jiffies_to_msecs(lcd_ldo_ocp_time - lcd_ldo_open_time)< HI6521_OPEN_LDO_TO_OCP_TIME)
        {
            lcd_ldo_ocp_count++;
        }
        else
        {
            if(lcd_ldo_ocp_count <= HI6521_LCD_LDO_OCP_LIMIT_NUM)
            {
                lcd_ldo_ocp_count = 0;
            }
        }
        lcd_ldo_ocp_time = 0;
        lcd_ldo_open_time = 0;

        if(lcd_ldo_ocp_count <= HI6521_LCD_LDO_OCP_LIMIT_NUM)
        {
            lcd_ldo_ocp_notify_prepare();
        }
        else
        {
            di->lcd_ncp_en = CHG_POWER_DIS;
            di->lcd_ldo_en = CHG_POWER_DIS;
            hi6521_config_lcd_ncp_enable(di);
            hi6521_config_lcd_ldo_enable(di);
            hi6521_config_lcd_boost_enable(di);
        }

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]LDO_OCP,reg[0x0A]:0x%x.\n",
                      __func__,irq_status[3] );
#endif
    }

    if (irq_status[3] & CHG_IRQ3_LDO1_200MA_OCP){
        di->ldo1_200ma_en= CHG_POWER_DIS;
        hi6521_config_ldo1_200ma_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ3_LDO1_200MA_OCP,reg[0x0A]:0x%x.\n",irq_status[3]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]LDO1_200MA_OCP,reg[0x0A]:0x%x.\n",
                      __func__,irq_status[3] );
#endif
    }

    if (irq_status[3] & CHG_IRQ3_LDO2_200MA_OCP){
        di->ldo2_200ma_en= CHG_POWER_DIS;
        hi6521_config_ldo2_200ma_enable(di);
        hwlog_err("SCHARGER ERROR:CHG_IRQ3_LDO2_200MA_OCP,reg[0x0A]:0x%x.\n",irq_status[3]);

#if defined (CONFIG_HUAWEI_DSM)
    CHARGER_DSM(dsm_charger_error_found, "[%s]LDO2_200MA_OCP,reg[0x0A]:0x%x.\n",
                      __func__,irq_status[3] );
#endif
    }

    if (irq_status[4] & CHG_IRQ4_OTG_SCP){
        if (CHG_V222 == s_scharger_version){
            otg_scp_cnt++;
	        hi6521_write_byte(di,irq_status[4] & CHG_IRQ4_OTG_SCP,CHG_IRQ_REG4);
            msleep(600);
            hi6521_read_byte(di,&reg_read,CHG_STATUS0);
            if ((reg_read & CHG_STATUS0_OTG_ON) == CHG_STATUS0_OTG_ON){
                otg_scp_cnt = 0;
                }
            if (otg_scp_cnt > 3){
                otg_scp_cnt = 0;
                di->otg_en = CHG_POWER_DIS;
                hi6521_config_otg_enable(di);
                hwlog_err("SCHARGER ERROR:CHG_IRQ4_OTG_SCP,reg[0x0B]:0x%x.\n",irq_status[4]);

#if defined (CONFIG_HUAWEI_DSM)
        CHARGER_DSM(dsm_charger_error_found, "[%s]OTG_SCP,reg[0x0B]:0x%x.\n",
                          __func__,irq_status[4] );
#endif
            }
        }else{
            di->otg_en = CHG_POWER_DIS;
            hi6521_config_otg_enable(di);
            hwlog_err("SCHARGER ERROR:CHG_IRQ4_OTG_SCP,reg[0x0B]:0x%x.\n",irq_status[4]);

#if defined (CONFIG_HUAWEI_DSM)
        CHARGER_DSM(dsm_charger_error_found, "[%s]OTG_SCP,reg[0x0B]:0x%x.\n",
                      __func__,irq_status[4] );
#endif
        }
    }

    if (irq_status[4] & CHG_IRQ4_OTG_UVP){
        if (CHG_V222 == s_scharger_version){
            otg_uvp_cnt++;
            hi6521_write_byte(di,irq_status[4] & CHG_IRQ4_OTG_UVP,CHG_IRQ_REG4);
            msleep(600);
            hi6521_read_byte(di,&reg_read,CHG_STATUS0);
            if ((reg_read & CHG_STATUS0_OTG_ON) == CHG_STATUS0_OTG_ON){
                otg_uvp_cnt = 0;
                }
            if (otg_uvp_cnt > 3){
                otg_uvp_cnt = 0;
                di->otg_en = CHG_POWER_DIS;
                hi6521_config_otg_enable(di);
                hwlog_err("SCHARGER ERROR:CHG_IRQ4_OTG_UVP,reg[0x0B]:0x%x.\n",irq_status[4]);

#if defined (CONFIG_HUAWEI_DSM)
        CHARGER_DSM(dsm_charger_error_found, "[%s]OTG_UVP,reg[0x0B]:0x%x.\n",
                          __func__,irq_status[4] );
#endif
            }
        }else{
            di->otg_en = CHG_POWER_DIS;
            hi6521_config_otg_enable(di);
            hwlog_err("SCHARGER ERROR:CHG_IRQ4_OTG_UVP,reg[0x0B]:0x%x.\n",irq_status[4]);

#if defined (CONFIG_HUAWEI_DSM)
        CHARGER_DSM(dsm_charger_error_found, "[%s]OTG_UVP,reg[0x0B]:0x%x.\n",
                          __func__,irq_status[4] );
#endif
        }
    }
       /*l00279044 scharger mntn add end*/
#if defined (CONFIG_HUAWEI_DSM)
    if (dsm_charger_error_found > 0) {
        dsm_client_notify(charger_dclient, DSM_CHARGER_ERROR_NO);
    } else if (!dsm_charger_error_found) {
        dsm_client_unocuppy(charger_dclient);
    }
#endif

    irq_status[3] = irq_status[3] & HI6521_NO_CLR_LDO_OCP;
	/*clean irq*/
	hi6521_write_byte(di,irq_status[0],CHG_IRQ_REG0);
	hi6521_write_byte(di,irq_status[1],CHG_IRQ_REG1);
	hi6521_write_byte(di,irq_status[2],CHG_IRQ_REG2);
	hi6521_write_byte(di,irq_status[3],CHG_IRQ_REG3);
	hi6521_write_byte(di,irq_status[4],CHG_IRQ_REG4);

	//blocking_notifier_call_chain(&reboot_notifier_list, NULL, NULL);
	//systemError(0x25, EXCH_S_SCHARGER, 0,0,0);
	enable_irq(di->irq);
}

STATIC struct hi6521_device_info *g_di;

int hi6521_get_max_voltagemV(void)
{
    struct hi6521_device_info *di = g_di;

    if(!di)
    {
        hwlog_err("error:hi6521_get_max_voltagemV di is NULL!!!");
        return 0;
    }
    return di->chg_fast_vchg;
}

int hi6521_get_vrechg_hys(void)
{
    struct hi6521_device_info *di = g_di;

    if(!di)
    {
        hwlog_err("error:hi6521_get_vrechg_hys di is NULL!!!");
        return 0;
    }

    return di->chg_vrechg_hys;
}

#if 0
STATIC void hi6521_charger_usb_stub(int type)
{
    g_di->event = type;

    hi6521_usb_charger_work(&g_di->usb_work);
}
#endif
STATIC int hi6521_usb_notifier_call(struct notifier_block *nb,
                        unsigned long event, void *data)
{
    struct hi6521_device_info *di =
          container_of(nb, struct hi6521_device_info, nb);

    di->event = event;
    switch (event) {
    case CHARGER_TYPE_USB:
        break;
    case CHARGER_TYPE_NON_STANDARD:
        break;
    case CHARGER_TYPE_BC_USB:
        break;
    case CHARGER_TYPE_STANDARD:
        break;
    case CHARGER_REMOVED:
        break;
    case USB_EVENT_OTG_ID:
        break;
    default:
        return NOTIFY_OK;
    }

    schedule_work(&di->usb_work);
    return NOTIFY_OK;
}
int hi6521_set_charge_state(int state)
{
    int old_state;
    int chg_en=0;
    struct hi6521_device_info *di = g_di;

    if ((di == NULL) || ((state!=0)&&(state!=1))){
        return -1;
    }

    old_state = di->chg_en;
    chg_en = state;
    di->factory_flag = state;
    hi6521_config_charger_enable_val(di,chg_en);

    return old_state;
}

STATIC ssize_t hi6521_set_dppm_voltage(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < VINDPM_MIN_3880)
                      || (val > VINDPM_MAX_5080))
         return -EINVAL;

    di->cin_dpmmV = (unsigned int)val;
    hi6521_config_input_source_reg(di);

    return status;
}

STATIC ssize_t hi6521_show_dppm_voltage(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val = 0;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    //val = di->cin_dpmmV;
	ret = hi6521_read_byte(di,&di->input_source_reg.value, INPUT_SOURCE_REG);
	if(ret)	{
		val = ret;
	} else {
		val = (int)di->input_source_reg.reg.buck_dpm_sel;
	}
    return sprintf(buf, "%d\n", val);
}

STATIC ssize_t hi6521_set_iin_runningtest(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < IINLIM_100)
                || (val > IINLIM_2500))
        return -EINVAL;

	dbg_current_iin = val;

    di->cin_limit = (unsigned int)val;
    hi6521_config_input_source_reg(di);

    return status;
}

STATIC ssize_t hi6521_show_iin_runningtest(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

	ret = hi6521_read_byte(di,&di->input_source_reg.value, INPUT_SOURCE_REG);
	if(ret)	{
		val = ret;
	} else {
		val = (int)di->input_source_reg.reg.buck_int_lim_sel;
	}
    return sprintf(buf, "%d\n", val);
}

STATIC ssize_t hi6521_set_fast_voltage(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < CHG_FAST_VCHG_MIN)
                     || (val > CHG_FAST_VCHG_MAX))
        return -EINVAL;

    di->chg_fast_vchg = (unsigned int)val;
    hi6521_config_fast_vol_and_term_current_reg(di);
    return status;
}

STATIC ssize_t hi6521_show_fast_voltage(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);
	ret = hi6521_read_byte(di,&di->charge_fast_vol_term_cur_reg.value, CHG_FAST_VOL_TERM_CUR_REG);
	if(ret)	{
		val = ret;
	} else {
		val = (int)(di->charge_fast_vol_term_cur_reg.reg.chg_fast_vchg * CHG_FAST_VCHG_STEP_50 + CHG_FAST_VCHG_MIN);
	}
    //val = di->chg_fast_vchg;
    return sprintf(buf, "%d\n", val);
}

STATIC ssize_t hi6521_set_ichg_runningtest(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < CHG_FAST_ICHG_MIN)
                   || (val > CHG_FAST_ICHG_MAX))
        return -EINVAL;

	dbg_current_ichg = val;
    di->chg_fast_ichg = (unsigned int)val;
    hi6521_config_fast_current_reg(di);

    return status;
}

STATIC ssize_t hi6521_show_ichg_runningtest(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

	ret = hi6521_read_byte(di,&di->charge_fast_current_reg.value, CHG_FAST_CURRENT_REG);
	if(ret)	{
		val = ret;
	} else {
		val = (int)(di->charge_fast_current_reg.reg.chg_fast_ichg * CHG_FAST_ICHG_STEP_100 + CHG_FAST_ICHG_MIN);
	}
    //val = di->chg_fast_ichg;
    return sprintf(buf, "%d\n", val);
}

STATIC ssize_t hi6521_set_precharge_current(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < CHG_PRG_ICHG_MIN) || (val > CHG_PRG_ICHG_MAX))
    return -EINVAL;

    di->chg_pre_ichg = (unsigned int)val;
    hi6521_config_prechg_current_vot_and_batfet_ctrl_reg(di);

    return status;
}

STATIC ssize_t hi6521_show_precharge_current(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    //val = di->chg_pre_ichg;
    ret = hi6521_read_byte(di,&di->charge_pre_current_vol_reg.value, CHG_PRE_VOL_CURRENT_BATFET_REG);
	if(ret)	{
		val = ret;
	} else {
		val = (int)(di->charge_pre_current_vol_reg.reg.chg_pre_ichg * CHG_PRG_ICHG_STEP_100 + CHG_PRG_ICHG_MIN);
	}
    return sprintf(buf, "%d\n", val);
}


STATIC ssize_t hi6521_set_precharge_voltage(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > CHG_PRE_VCHG_MAX))
    return -EINVAL;
    if(val == 1){
        charge_current_temp = val;
        hi6521_monitor_battery_ntc_charging(di);
        return status;
    }else{
        if((val < IINLIM_500) || (val > IINLIM_2500))
            return -EINVAL;
    }
    di->chg_pre_vchg = (unsigned int)val;
    di->cin_limit = di->chg_pre_vchg;
    hi6521_config_prechg_current_vot_and_batfet_ctrl_reg(di);

    return status;
}

STATIC ssize_t hi6521_show_precharge_voltage(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    //val = di->chg_pre_ichg;
    ret = hi6521_read_byte(di,&di->charge_pre_current_vol_reg.value, CHG_PRE_VOL_CURRENT_BATFET_REG);
	if(ret)	{
		val = ret;
	} else {
		val = (int)di->charge_pre_current_vol_reg.reg.chg_pre_vchg;
	    if(0 == val)
			val = CHG_PRE_VCHG_MIN;
		else if (0x1 == val)
			val = CHG_PRE_VCHG_3000MV;
		else if  (0x2 == val)
			val = CHG_PRE_VCHG_3100MV;
		else
			val = CHG_PRE_VCHG_MAX;
	}
    return sprintf(buf, "%d\n", val);
}


STATIC ssize_t hi6521_set_termination_current(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < CHG_TERM_ICHG_MIN) || (val > CHG_TERM_ICHG_MAX))
        return -EINVAL;

    di->chg_term_ichg = (unsigned int)val;
    hi6521_config_fast_vol_and_term_current_reg(di);

    return status;
}

STATIC ssize_t hi6521_show_termination_current(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    //val = di->chg_term_ichg;
    ret = hi6521_read_byte(di,&di->charge_fast_vol_term_cur_reg.value, CHG_FAST_VOL_TERM_CUR_REG);
	if(ret)	{
		val = ret;
	} else {
		val = (int)(di->charge_fast_vol_term_cur_reg.reg.chg_term_ichg * CHG_TERM_ICHG_STEP_50 + CHG_TERM_ICHG_MIN);
	}
    return sprintf(buf, "%d\n", val);
}

STATIC ssize_t hi6521_set_enable_itermination(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    di->chg_termination_ctrl = (unsigned int)val;
    hi6521_config_fast_safe_timer_and_term_ctrl_reg(di);

    return status;
}

STATIC ssize_t hi6521_show_enable_itermination(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    //val = di->chg_termination_ctrl;
    ret = hi6521_read_byte(di,&di->charge_fast_safe_timer_term_ctrl_reg.value, CHG_FAST_SAFE_TIMER_TERM_CTRL_REG);
	if(ret)	{
		val = ret;
	} else {
		val = (int)di->charge_fast_safe_timer_term_ctrl_reg.reg.chg_termination_ctrl;
	}
    return sprintf(buf, "%d\n", val);
}

/*
 * Add a sysfs to set the parameter hot_enable_charge_flag.
 * If framework call to disable charge because ntc(hot area) temperature is over 53,
 * set hot_enable_charge_flag to 0. Then schedule hi6521_monitor_battery_ntc_charging,
 * it will set input source to 100 and disable charge by setting chrg_config to disable.
 * If set hot_enable_charge_flag to 0, it means the temperature is too high.
 * Just stop charging can't make the temperature falling down,
 * so it limits input source to 100mA together.
 */
int hi6521_get_factory_flag(void)
{
    struct hi6521_device_info *di = g_di;

    if (g_di)
        return di->factory_flag;
    else
        return 0;
}
/* set 1 --- enable_charger; 0 --- disable charger */
STATIC ssize_t hi6521_set_enable_charger(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int chg_en=0;
    size_t status = count;
    unsigned int events = VCHRG_START_CHARGING_EVENT;
    struct hi6521_device_info *di = dev_get_drvdata(dev);
    int charger_type = CHARGER_TYPE_MAX;

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    if(val)
    {
        charger_type = get_charger_name();
        if((charger_type <= CHARGER_TYPE_MIN) || (charger_type >= CHARGER_REMOVED))
        {
            hwlog_err("Set Charger event by device node, charger_type:%d, error, charger is not present!\n", charger_type);
            return -EINVAL;
        }
    }

    chg_en = val;
    di->factory_flag = val;
    di->not_stop_chrg_flag = 0;
    di->not_limit_chrg_flag = 0;
    if(di->factory_flag){
        events = VCHRG_START_CHARGING_EVENT;
    } else {
        events = VCHRG_NOT_CHARGING_EVENT;
    }
    hi6521_config_charger_enable_val(di, chg_en);
    hwlog_err("Set Charger event: 0x%x by device node\n",events);
    hisi_coul_charger_event_rcv(events);//blocking_notifier_call_chain(&notifier_list, events, NULL);
    return status;
}

STATIC ssize_t hi6521_show_enable_charger(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    //val = di->chg_en ;
    ret = hi6521_read_byte(di,&di->power_on_config_reg01.value, POWER_ON_CONFIG_REG01);
	if(ret)	{
		val = ret;
	} else {
		val = (int)di->power_on_config_reg01.reg.chg_en_int;
	}
    return sprintf(buf, "%d\n", val);
}

/* set 1 --- enable_batfet; 0 --- disable batfet */
STATIC ssize_t hi6521_set_enable_batfet(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    di->batfet_ctrl = (unsigned char)val;
    hi6521_config_prechg_current_vot_and_batfet_ctrl_reg(di);//hi6521_config_misc_operation_reg(di);

    return status;
}

STATIC ssize_t hi6521_show_enable_batfet(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val;
	int ret = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    //val = di->batfet_ctrl;
    ret = hi6521_read_byte(di,&di->charge_pre_current_vol_reg.value, CHG_PRE_VOL_CURRENT_BATFET_REG);
	if(ret)	{
		val = ret;
	} else {
		val = (int)di->charge_pre_current_vol_reg.reg.batfet_ctrl;
	}
    return sprintf(buf, "%d\n", val);
}

/*
* set 0 --- enable charger ; 1 --- disable charger
*
*/
STATIC ssize_t hi6521_set_gpio_enable_charger(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;
	if (BAT_BOARD_SFT == is_board_type)
    	gpio_set_value(di->gpio_ce, (int)val);
    return status;
}

STATIC ssize_t hi6521_show_gpio_enable_charger(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val = 0;
    struct hi6521_device_info *di = dev_get_drvdata(dev);
	if (BAT_BOARD_SFT == is_board_type){
		val = gpio_get_value(di->gpio_ce);
	}

    return sprintf(buf, "%d\n", val);
}

STATIC ssize_t hi6521_set_charging(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    int status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if (strncmp(buf, "startac", 7) == 0) {
        if (di->charger_source == POWER_SUPPLY_TYPE_USB)
            hi6521_stop_charger(di);
        hi6521_start_ac_charger(di);
    } else if (strncmp(buf, "startusb", 8) == 0) {
        if (di->charger_source == POWER_SUPPLY_TYPE_MAINS)
            hi6521_stop_charger(di);
        hi6521_start_usb_charger(di);
    } else if (strncmp(buf, "stop" , 4) == 0) {
        hi6521_stop_charger(di);
    } else if (strncmp(buf, "otg" , 3) == 0) {
        if (di->charger_source == POWER_SUPPLY_TYPE_BATTERY){
            hi6521_stop_charger(di);
            hi6521_start_usb_otg(di);
        } else{
            return -EINVAL;
        }
    } else
        return -EINVAL;

    return status;
}

STATIC ssize_t hi6521_set_wakelock_enable(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    if ((val) && (di->charger_source != POWER_SUPPLY_TYPE_BATTERY))
        wake_lock(&chrg_lock);
    else
        wake_unlock(&chrg_lock);

    di->wakelock_enabled = val;
    return status;
}

STATIC ssize_t hi6521_show_wakelock_enable(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    val = di->wakelock_enabled;
    return sprintf(buf, "%u\n", val);
}


STATIC long g_reg_addr = 0;
STATIC ssize_t hi6521_set_reg_sel(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    int status = count;
	g_reg_addr = 0;
    /*struct hi6521_device_info *di = dev_get_drvdata(dev);*/

    if (strict_strtol(buf, 0, &g_reg_addr) < 0)
        return -EINVAL;
	return status;
}

STATIC ssize_t hi6521_set_reg_value(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val = 0;
	size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if (strict_strtol(buf, 0, &val) < 0)
        return -EINVAL;
	hi6521_write_byte(di, (u8)val, (u8)g_reg_addr);
	return status;

}

STATIC ssize_t hi6521_show_reg_info(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
	int ret = 0;
    u8 val = 0;

    struct hi6521_device_info *di = dev_get_drvdata(dev);

    ret = hi6521_read_byte(di, &val,(u8)g_reg_addr);
	if(ret)	{
		val = ret;
	}
	return sprintf(buf,"reg[0x%x]=0x%x\n",(u32)g_reg_addr,val);
}
STATIC ssize_t hi6521_show_chargelog_head(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int i = 0;
    u8 temp_buf[20] = {0};
    for(i=0;i<=107;i++)
    {
        sprintf(temp_buf,"Reg[0x%2x]    ",i);
        strcat(buf,temp_buf);
    }
    strcat(buf,"\n");
    return strlen(buf);
}
STATIC ssize_t hi6521_show_chargelog(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int i = 0;
    u8 read_reg[108] = {0};
    u8 buf_temp[26] = {0};
    struct hi6521_device_info *di = dev_get_drvdata(dev);
	/*i2c driver don't support >64byte operate*/
    hi6521_read_block(di, &read_reg[0], 0, 64);
	hi6521_read_block(di, &read_reg[64], 64, 44);
    for(i=0;i<=107;i++)
    {
        snprintf(buf_temp,26,"0x%-11.2x",read_reg[i]);
        strcat(buf,buf_temp);
    }
    strcat(buf,"\n");
   return strlen(buf);
}

STATIC ssize_t hi6521_show_lcd_state(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int val = 0;
    int ret = 0;
    SOC_SCHARGER_ONFF1_UNION   scharger_lcd_state_reg;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    ret = hi6521_read_byte(di,&scharger_lcd_state_reg.value, POWER_ON_CONFIG_REG01);
    if(ret) {
        val = ret;
    } else {
        val = (int)scharger_lcd_state_reg.reg.wled_en_int;
    }
    return sprintf(buf, "%d\n", val);
}

STATIC ssize_t hi6521_set_calling_limit(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
       return -EINVAL;

    di->calling_limit = val;
    if (di->charger_source == POWER_SUPPLY_TYPE_MAINS)
    {
        if(di->calling_limit){
            di->cin_limit = IINLIM_900;
            di->chg_fast_ichg = CHG_FAST_ICHG_VALUE(800);
            hi6521_config_input_source_reg(di);
            hi6521_config_fast_current_reg(di);
            hwlog_info("calling_limit_current = %d\n", di->cin_limit);
        } else {
            di->battery_temp_status = -1;
            di->cin_limit = di->max_cin_currentmA;
            di->chg_fast_ichg = di->max_currentmA ;
            hi6521_config_input_source_reg(di);
            hi6521_config_fast_current_reg(di);
        }
    } else {
        di->calling_limit = 0;
    }

    return status;
}

STATIC ssize_t hi6521_show_calling_limit(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    val = di->calling_limit;

    return sprintf(buf, "%u\n", val);
}

/*
 * Add a sysfs to set the parameter hot_limit_charging_flag.
 * If framework call to limit current because ntc(hot area) temperature is over 48C,
 * set hot_limit_charging_flag to 1.
 * Then schedule hi6521_monitor_battery_ntc_charging to set current to no limit status.
 */
 /*
STATIC ssize_t hi6521_set_hot_limit_temp_charging(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    di->not_limit_chrg_flag = val;

    di->chg_fast_ichg = di->max_currentmA;
    di->cin_limit = di->max_cin_currentmA;
    hi6521_config_fast_current_reg(di);
    hi6521_config_charger_enable(di);

    return status;
}
*/
static ssize_t hi6521_show_limit_charging(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    val = di->not_limit_chrg_flag;
    return snprintf(buf, PAGE_SIZE, "%lu\n", val);
}

static ssize_t hi6521_set_limit_charging(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    int status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    di->not_limit_chrg_flag = val;

    di->chg_fast_ichg = di->max_currentmA;
    di->cin_limit = di->max_cin_currentmA;
    hi6521_config_fast_current_reg(di);
    hi6521_config_charger_enable(di);

    return status;
}

static ssize_t hi6521_set_not_stop_chrg_flag(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    di->not_stop_chrg_flag = val;
    hi6521_monitor_battery_ntc_charging(di);
    return status;
}

static ssize_t hi6521_show_not_stop_chrg_flag(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    val = di->not_stop_chrg_flag;
    return snprintf(buf, PAGE_SIZE, "%lu\n", val);
}
static ssize_t hi6521_set_dpm_switch_enable(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;

    dpm_switch_enable = val;
    hwlog_info("dpm_switch_enable = %d\n", dpm_switch_enable);
    return status;
}

static ssize_t hi6521_show_dpm_switch_enable(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned long val;
    val = dpm_switch_enable;
    return sprintf(buf, "%lu\n", val);
}

STATIC ssize_t hi6521_set_temperature_parameter(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 40)
                   || (val > 70))
        return -EINVAL;

	di->temperature_warm = (int)val;
	di->temperature_hot  = val + 5;

	hi6521_monitor_battery_ntc_charging(di);

    return status;
}

STATIC ssize_t hi6521_show_temperature_parameter(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int read_reg[6] = {0};
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    read_reg[0] = di->temperature_cold;
    read_reg[1] = di->temperature_cool;
    read_reg[2] = di->temperature_5;
    read_reg[3] = di->temperature_10;
    read_reg[4] = di->temperature_warm;
    read_reg[5] = di->temperature_hot;

    sprintf(buf,"%-9d  %-9d  %-9d  %-9d  %-9d  %-9d",
    read_reg[0],read_reg[1],read_reg[2],read_reg[3],read_reg[4],read_reg[5]);

    return strlen(buf);
}

STATIC ssize_t hi6521_set_current(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < CHG_FAST_ICHG_MIN)
                   || (val > CHG_FAST_ICHG_MAX))
        return -EINVAL;

    di->max_currentmA = val;
    di->max_cin_currentmA = val;

    return status;
}
/****************************************************************************
  Function:     bq2419x_mhl_set_iin
  Description:  MHL certification
  Input:        flag
                0:cin_limit = 500
                1:return normal charge cycle
  Output:       NA
  Return:       0:OK.
                Negtive:someting is wrong.
****************************************************************************/
int hi6521_mhl_set_iin(int flag)
{
    struct hi6521_device_info *di = g_di;
    static unsigned int mhl_iin_temp = 0;

    if (!di)
        return -EPERM;

    if(flag != 0 && flag != 1)
        return -EINVAL;

    hwlog_info("mhl_limit_flag = %d\n", flag);

    mhl_limit_cin_flag = flag;

    if(flag == 0 && di->cin_limit >= IINLIM_100) {
        mhl_iin_temp = di->cin_limit;
        di->cin_limit = IINLIM_100;
        hi6521_config_input_source_reg(di);
        return 0;
    }

    if(flag == 1 && mhl_iin_temp != 0){
        di->cin_limit = mhl_iin_temp;
        hi6521_config_input_source_reg(di);
        mhl_iin_temp = 0;
        return 0;
    }

    return -EINVAL;
}
STATIC ssize_t hi6521_set_iin_thermal(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0)
                   || (val > IINLIM_2500))
        return -EINVAL;
    if((0 == val) || (1 == val))
	{
        input_current_iin = di->max_cin_currentmA;
    }
    else
	{
	   input_current_iin = val;
	}

    if(di->cin_limit > input_current_iin){
        di->cin_limit = input_current_iin;
        hi6521_config_input_source_reg(di);
    }
    hwlog_info("set temp_current_iin = %d\n", input_current_iin);
    return status;
}

STATIC ssize_t hi6521_show_iin_thermal(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val;
    val = input_current_iin;
    return sprintf(buf, "%u\n", val);
}

STATIC ssize_t hi6521_set_ichg_thermal(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val;
    size_t status = count;
    struct hi6521_device_info *di = dev_get_drvdata(dev);

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0)
                   || (val > CHG_FAST_ICHG_MAX))
        return -EINVAL;
    if(val == 1)
        input_current_ichg = di->max_currentmA;
    else
	input_current_ichg = val;
    hi6521_monitor_battery_ntc_charging(di);
    hwlog_info("set temp_current_ichg = %d\n", input_current_ichg);
    return status;
}

STATIC ssize_t hi6521_show_ichg_thermal(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val;
    val = input_current_ichg;
    return sprintf(buf, "%u\n", val);
}

#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
STATIC ssize_t hi6521_store_charge_temp_protect(struct device *dev,
                  struct device_attribute *attr,
                  const char *buf, size_t count)
{
    long val=0;
    size_t status = count;
    if (strict_strtol(buf, 10, &val) < 0)
        return -EINVAL;
    hisi_coul_reg_write(val);
    return status;
}

STATIC ssize_t hi6521_show_charge_temp_protect(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    unsigned int val;
      val = hisi_coul_reg_read();
    return sprintf(buf, "%d\n", val);
}
#endif

STATIC DEVICE_ATTR(dppm_voltage, (S_IWUSR | S_IRUGO),
                hi6521_show_dppm_voltage,
                hi6521_set_dppm_voltage);
STATIC DEVICE_ATTR(iin_runningtest, (S_IWUSR | S_IRUGO),
                hi6521_show_iin_runningtest,
                hi6521_set_iin_runningtest);
STATIC DEVICE_ATTR(fast_voltage, (S_IWUSR | S_IRUGO),
                hi6521_show_fast_voltage,
                hi6521_set_fast_voltage);
STATIC DEVICE_ATTR(ichg_runningtest, (S_IWUSR | S_IRUGO),
                hi6521_show_ichg_runningtest,
                hi6521_set_ichg_runningtest);
STATIC DEVICE_ATTR(termination_current, (S_IWUSR | S_IRUGO),
                hi6521_show_termination_current,
                hi6521_set_termination_current);
STATIC DEVICE_ATTR(precharge_current, (S_IWUSR | S_IRUGO),
                hi6521_show_precharge_current,
                hi6521_set_precharge_current);
STATIC DEVICE_ATTR(precharge_voltage, (S_IWUSR | S_IRUGO),
                hi6521_show_precharge_voltage,
                hi6521_set_precharge_voltage);
STATIC DEVICE_ATTR(enable_itermination, (S_IWUSR | S_IRUGO),
                hi6521_show_enable_itermination,
                hi6521_set_enable_itermination);
STATIC DEVICE_ATTR(enable_charger, (S_IWUSR | S_IRUGO),
                hi6521_show_enable_charger,
                hi6521_set_enable_charger);
STATIC DEVICE_ATTR(enable_batfet, (S_IWUSR | S_IRUGO),
                hi6521_show_enable_batfet,
                hi6521_set_enable_batfet);
STATIC DEVICE_ATTR(gpio_enable_charger, (S_IWUSR | S_IRUGO),
                hi6521_show_gpio_enable_charger,
                hi6521_set_gpio_enable_charger);
STATIC DEVICE_ATTR(charging_mode_sel, (S_IWUSR | S_IRUGO),
                NULL,
                hi6521_set_charging);
STATIC DEVICE_ATTR(wakelock_enable, (S_IWUSR | S_IRUGO),
                hi6521_show_wakelock_enable,
                hi6521_set_wakelock_enable);
STATIC DEVICE_ATTR(chargelog_head, (S_IWUSR | S_IRUGO),
                hi6521_show_chargelog_head,
                NULL);
STATIC DEVICE_ATTR(chargelog, (S_IWUSR | S_IRUGO),
                hi6521_show_chargelog,
                NULL);
STATIC DEVICE_ATTR(select_reg, (S_IWUSR | S_IRUGO),
                NULL,
                hi6521_set_reg_sel);
STATIC DEVICE_ATTR(config_reg, (S_IWUSR | S_IRUGO),
                hi6521_show_reg_info,
                hi6521_set_reg_value);
STATIC DEVICE_ATTR(calling_limit, (S_IWUSR | S_IRUGO),
                hi6521_show_calling_limit,
                hi6521_set_calling_limit);
STATIC DEVICE_ATTR(temperature_parameter, (S_IWUSR | S_IRUGO),
                hi6521_show_temperature_parameter,
                hi6521_set_temperature_parameter);
STATIC DEVICE_ATTR(set_current, (S_IWUSR | S_IRUGO),
                NULL,
                hi6521_set_current);
STATIC DEVICE_ATTR(iin_thermal, (S_IWUSR | S_IRUGO),
                hi6521_show_iin_thermal,
                hi6521_set_iin_thermal);
STATIC DEVICE_ATTR(ichg_thermal, (S_IWUSR | S_IRUGO),
                hi6521_show_ichg_thermal,
                hi6521_set_ichg_thermal);
static DEVICE_ATTR(limit_charging, (S_IWUSR | S_IRUGO),
                hi6521_show_limit_charging,
                hi6521_set_limit_charging);
static DEVICE_ATTR(not_stop_chrg_flag, (S_IWUSR | S_IRUGO),
                hi6521_show_not_stop_chrg_flag,
                hi6521_set_not_stop_chrg_flag);
static DEVICE_ATTR(dpm_switch_enable, (S_IWUSR | S_IRUGO),
                hi6521_show_dpm_switch_enable,
                hi6521_set_dpm_switch_enable);
STATIC DEVICE_ATTR(lcd_state, (S_IWUSR | S_IRUGO),
                hi6521_show_lcd_state,
                NULL);
#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
STATIC DEVICE_ATTR(charge_temp_protect, (S_IWUSR | S_IRUGO),
                hi6521_show_charge_temp_protect,
                hi6521_store_charge_temp_protect);
#endif


STATIC struct attribute *hi6521_attributes[] = {
    &dev_attr_dppm_voltage.attr,
    &dev_attr_iin_runningtest.attr,
    &dev_attr_ichg_runningtest.attr,
    &dev_attr_fast_voltage.attr,
    &dev_attr_precharge_current.attr,
    &dev_attr_precharge_voltage.attr,
    &dev_attr_termination_current.attr,
    &dev_attr_enable_itermination.attr,
    &dev_attr_enable_charger.attr,
    &dev_attr_enable_batfet.attr,
    &dev_attr_gpio_enable_charger.attr,
    &dev_attr_charging_mode_sel.attr,
    &dev_attr_wakelock_enable.attr,
    &dev_attr_chargelog_head.attr,
    &dev_attr_chargelog.attr,
    &dev_attr_select_reg.attr,
    &dev_attr_config_reg.attr,
    &dev_attr_calling_limit.attr,
    &dev_attr_temperature_parameter.attr,
    &dev_attr_set_current.attr,
    &dev_attr_iin_thermal.attr,
    &dev_attr_ichg_thermal.attr,
    &dev_attr_limit_charging.attr,
    &dev_attr_not_stop_chrg_flag.attr,
    &dev_attr_dpm_switch_enable.attr,
    &dev_attr_lcd_state.attr,
#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
    &dev_attr_charge_temp_protect.attr,
#endif
    NULL,
};

STATIC const struct attribute_group hi6521_attr_group = {
    .attrs = hi6521_attributes,
};

STATIC BLOCKING_NOTIFIER_HEAD(scharger_init_notifier_list);

int scharger_register_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&scharger_init_notifier_list, nb);
}

EXPORT_SYMBOL_GPL(scharger_register_notifier);

int scharger_unregister_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&scharger_init_notifier_list, nb);
}

EXPORT_SYMBOL_GPL(scharger_unregister_notifier);


STATIC void scharger_notify_prepare(void)
{
    blocking_notifier_call_chain(&scharger_init_notifier_list,0,NULL);
}

STATIC BLOCKING_NOTIFIER_HEAD(lcd_ldo_ocp_init_notifier_list);

int lcd_ldo_ocp_register_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&lcd_ldo_ocp_init_notifier_list, nb);
}

EXPORT_SYMBOL_GPL(lcd_ldo_ocp_register_notifier);

int lcd_ldo_ocp_unregister_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&lcd_ldo_ocp_init_notifier_list, nb);
}
EXPORT_SYMBOL_GPL(lcd_ldo_ocp_unregister_notifier);

static void lcd_ldo_ocp_notify_prepare(void)
{
    blocking_notifier_call_chain(&lcd_ldo_ocp_init_notifier_list,HI6521_LCD_LDO_OCP_EVENT,NULL);
}
STATIC int  hi6521_charger_probe(struct i2c_client *client,
                           const struct i2c_device_id *id)
{
    struct hi6521_device_info *di;
    //struct hi6521_platform_data *pdata = client->dev.platform_data;
    enum usb_charger_type plugin_stat = CHARGER_REMOVED;
	enum of_gpio_flags gpio_flags;
	struct device_node *batt_node;
    int ret = 0;
    u8 read_reg = 0;

	struct class *power_class = NULL;
	struct device * new_dev = NULL;
	struct device_node* np;

    di = kzalloc(sizeof(*di), GFP_KERNEL);
    if (!di) {
        hwlog_err("hi6521_device_info is NULL!\n");
        return -ENOMEM;
    }

	sema_init(&di->charger_data_busy_lock, 1);
    di->dev = &client->dev;
	np = di->dev->of_node;
    di->client = client;

    i2c_set_clientdata(client, di);


    ret = hi6521_read_byte(di, &read_reg, SOC_SCHARGER_CHIP_VERSION_ADDR(0));

    if (ret < 0) {
        hwlog_err("chip not present at address %x\n",
                     client->addr);
        ret = -EINVAL;
        goto err_kfree;
    }

    if ((SCHARGER_VERSION_HI6521_VALUE  == read_reg)/*&& (client->addr == I2C_ADDR_HI6521)*/) {
		di->bqchip_version = SCHARGER_VERSION_HI6521_VALUE;
	}
    else {
        hwlog_err("unknown bq chip\n");
        hwlog_err("Chip address %x", client->addr);
        hwlog_err("scharger chip version reg value %x", read_reg);
        ret = -EINVAL;
        goto err_kfree;
    }

	ret = hi6521_read_byte(di, &s_scharger_version, CHG_V210_VERSION);
    if (ret < 0) {
        hwlog_err("[%s]scharger_version read error.\n",__FUNCTION__);
    }

     ret = hi6521_get_boardid_charge_parameter(di);

     if(!ret){
        if (of_property_read_u32(np, "max_charger_voltagemV",
            &di->max_voltagemV) < 0) {
            hwlog_err("%s failed %d\n", __func__, __LINE__);
            return -EFAULT;
        }
		if (CHG_V210 == s_scharger_version){
			di->max_voltagemV -= 50;
		}

        if (of_property_read_u32(np, "max_charger_currentmA",
            &di->max_currentmA) < 0) {
            hwlog_err("%s failed %d\n", __func__, __LINE__);
            return -EFAULT;
        }
        if (of_property_read_u32(np, "max_cin_limit_currentmA",
            &di->max_cin_currentmA) < 0) {
            hwlog_err("%s failed %d\n", __func__, __LINE__);
            return -EFAULT;
        }
    }
    //of_property_read_u32(np, "charger_board_type",&g_is_sft);
    batt_node = of_find_compatible_node(NULL, NULL, "huawei,bq_bci_battery");
    if (batt_node) {
        of_property_read_u32(batt_node, "battery_board_type",&is_board_type);
    } else {
		is_board_type = BAT_BOARD_SFT;
	}
	hwlog_err( "get board type is %d !\n",is_board_type );

	if (BAT_BOARD_SFT == is_board_type)
	{
		di->gpio_ce = of_get_gpio_by_prop(np,"gpio_ce",0,0,&gpio_flags);
		if(!gpio_is_valid(di->gpio_ce)){
			hwlog_err("%s failed %d\n", __func__, __LINE__);
			return -EFAULT;
		}

		/*set GPIO_4_0 to control CD pin to disable/enable hi6521 IC,0:enable 1:disable*/
		ret = gpio_request(di->gpio_ce, "charger_cd");
		if (ret) {
			hwlog_err("could not request scharger enable gpio!\n");
			ret = -ENOMEM;
			goto err_io;
		}
		gpio_direction_output(di->gpio_ce, 0);
		gpio_set_value(di->gpio_ce,0);
	}


    di->cin_dpmmV              = VINDPM_4520;
    di->cin_limit              = IINLIM_500;
    //pre_cin_limit              = IINLIM_500;

    di->chg_fast_vchg          = di->max_voltagemV;
    di->chg_fast_ichg          = CHG_FAST_ICHG_MIN ;

    di->chg_fastchg_safe_timer = CHG_FAST_SAFE_TIMER_8H;
    di->chg_termination_ctrl   = CHG_TERMINATION_EN;
    di->chg_vbatir_adj         = CHG_VBATIR_ADJ_VALUE_MV(80);

    di->chg_pre_ichg           = CHG_PRG_ICHG_200MA;
	di->chg_pre_vchg           = CHG_PRE_VCHG_MIN;
    di->chg_term_ichg          = CHG_TERM_ICHG_150MA;

	di->chg_prechg_timer       = CHG_PRE_TIMER_MIN;
	di->chg_rechg_time         = CHG_RECHG_TIMER_2000MS;
	di->chg_vrechg_hys         = CHG_RECHG_VOL_HYS_VALUE_MV(100);

	di->chg_thsd_treg_set      = CHG_THSD_TREG_SET_MAX;
	di->chg_thsd_tstp_set      = CHG_THSD_TSTP_SET_140C;


	di->watchdog_timer         = CHG_WDT_TIMER_40S;
	di->sys_minmV              = SYS_MIN_3650;

    di->chg_en                 = CHG_POWER_DIS;
	di->otg_en                 = CHG_POWER_DIS;
	di->wled_en                = CHG_POWER_EN;//CHG_POWER_DIS;
	di->ldo1_200ma_en          = CHG_POWER_DIS;
	di->ldo2_200ma_en          = CHG_POWER_DIS;
    di->batfet_ctrl            = CHG_BATFET_EN;
	di->backup_battery_exist   = false;
    //di->hot_enable_charge_flag = 1;
    di->battery_full           = 0;
    di->charge_full_count      = 0;

    di->japan_charger          = 0;
    di->is_two_stage_charger   = 0;
    di->two_stage_charger_status = TWO_STAGE_CHARGE_FIRST_STAGE;
    di->first_stage_voltage    = 4200;
    di->second_stage_voltage   = 4100;
    di->is_disable_cool_temperature_charger = 0;
    di->high_temp_para =HIGH_TEMP_CP_U9701L;
    input_current_iin = di->max_cin_currentmA;
    input_current_ichg = di->max_currentmA;
	di->not_limit_chrg_flag = 0;
    di->not_stop_chrg_flag = 0;
    di->factory_flag = 0;

	/*set irq init*/
	di->charge_irq_mask0_reg.value = 0xFF;
	di->charge_irq_mask1_reg.value = 0xFF;
	di->charge_irq_mask2_reg.value = 0xFF;
	di->charge_irq_mask3_reg.value = 0xFF;
	di->charge_irq_mask4_reg.value = 0xFF;
    di->charge_irq_mask0_reg.reg.trickle_chg_fault_rm = CHG_IRQ_EN;
    di->charge_irq_mask0_reg.reg.pre_chg_fault_rm     = CHG_IRQ_EN;
    di->charge_irq_mask0_reg.reg.fast_chg_fault_rm    = CHG_IRQ_EN;
    di->charge_irq_mask1_reg.reg.wled_scp_rm          = CHG_IRQ_EN;
    di->charge_irq_mask1_reg.reg.buck_scp_rm          = CHG_IRQ_EN;
    di->charge_irq_mask1_reg.reg.wled_uvp_rm          = CHG_IRQ_EN;
    di->charge_irq_mask1_reg.reg.wled_ovp_4p5v_rm     = CHG_IRQ_EN;
    di->charge_irq_mask1_reg.reg.wled_ovp_38v_rm      = CHG_IRQ_EN;
    di->charge_irq_mask2_reg.reg.flash_bst_uvp_rm     = CHG_IRQ_EN;
    di->charge_irq_mask2_reg.reg.flash_bst_ovp_rm     = CHG_IRQ_EN;
    di->charge_irq_mask2_reg.reg.flash_bst_scp_rm     = CHG_IRQ_EN;
    di->charge_irq_mask2_reg.reg.flash_led_short_rm   = CHG_IRQ_EN;
    di->charge_irq_mask2_reg.reg.flash_led_open_rm    = CHG_IRQ_EN;
    di->charge_irq_mask3_reg.reg.lcd_ncp_scp_rm       = CHG_IRQ_EN;
    di->charge_irq_mask3_reg.reg.lcd_bst_uvp_rm       = CHG_IRQ_EN;
    di->charge_irq_mask3_reg.reg.lcd_bst_ovp_rm       = CHG_IRQ_EN;
    di->charge_irq_mask3_reg.reg.lcd_bst_scp_rm       = CHG_IRQ_EN;
    di->charge_irq_mask3_reg.reg.lcd_ldo_ocp_rm       = CHG_IRQ_EN;
    di->charge_irq_mask3_reg.reg.ldo1_200ma_ocp_rm    = CHG_IRQ_EN;
    di->charge_irq_mask3_reg.reg.ldo2_200ma_ocp_rm    = CHG_IRQ_EN;

    di->charge_irq_mask4_reg.reg.otg_uvp_rm     	  = CHG_IRQ_EN;
    di->charge_irq_mask4_reg.reg.otg_scp_rm     	  = CHG_IRQ_EN;

    g_scharger_dev = di;
    hi6521_get_boardid_japan_charge_parameter(di);
    hi6521_config_fast_current_reg(di);
    hi6521_config_fast_vol_and_term_current_reg(di);
    hi6521_config_fast_safe_timer_and_term_ctrl_reg(di);
    hi6521_config_prechg_current_vot_and_batfet_ctrl_reg(di);
	hi6521_config_prechg_timer_and_rechg_vol_timer_reg(di);
    hi6521_config_thernal_regulation_reg(di);
    hi6521_config_chgwdt_timer_reg(di);
	hi6521_config_sys_min_vol_reg(di);
	hi6521_config_power_on_reg01(di);
	hi6521_config_limit_temperature_parameter(di);
	hi6521_config_opt_param(di);
    wake_lock_init(&chrg_lock, WAKE_LOCK_SUSPEND, "hi6521_chrg_wakelock");
    wake_lock_init(&stop_chrg_lock, WAKE_LOCK_SUSPEND, "hi6521_stop_chrg_wakelock");
	/*interrput init*/
	hi6521_write_byte(di, di->charge_irq_mask0_reg.value, CHG_IRQ_MASK_REG0);
	hi6521_write_byte(di, di->charge_irq_mask1_reg.value, CHG_IRQ_MASK_REG1);
	hi6521_write_byte(di, di->charge_irq_mask2_reg.value, CHG_IRQ_MASK_REG2);
	hi6521_write_byte(di, di->charge_irq_mask3_reg.value, CHG_IRQ_MASK_REG3);
	hi6521_write_byte(di, di->charge_irq_mask4_reg.value, CHG_IRQ_MASK_REG4);

	/*create scharger irq workqueue*/
	if (!di->scharger_int_workqueue)
    {
        /* Initialize works */
        di->scharger_int_workqueue = create_singlethread_workqueue("balong_scharger_workqueue");
        if (!di->scharger_int_workqueue) {
            hwlog_err( "Create scharger workqueue failed\n");
            ret = -1;
            goto err_int_work;
        }
        else
        {
            dev_dbg(di->dev,"Initialization of scharger workqueue succeed\n");
        }
    }

    di->usb_shell_gpio = of_get_gpio_by_prop(np,"gpio_usb_shell",0,0,&gpio_flags);
    if(di->usb_shell_gpio < 0)
    {
        hwlog_err("usbshellgpio gpio_usb_shell is not exist:%d\n", di->usb_shell_gpio);
        di->usb_shell_gpio_valid = 0;
    }
    else
    {
        if(!gpio_is_valid(di->usb_shell_gpio))
        {
            hwlog_err("usbshellgpio usb_shell_gpio get failed!!!\n");
            di->usb_shell_gpio_valid = 0;
        }
        else
        {
            di->usb_shell_gpio_valid = 1;
            hwlog_info("usbshellgpio usb_shell_gpio:%d\n", di->usb_shell_gpio);
        }
    }

    INIT_WORK(&di->hi6521_irq_work, hi6521_irq_work_handle);
    //di->interrput_gpio = of_get_named_gpio(np,"gpio_int",0);
    di->interrput_gpio = of_get_gpio_by_prop(np,"gpio_int",0,0,&gpio_flags);
    if(!gpio_is_valid(di->interrput_gpio)){
        hwlog_err("%s failed %d\n", __func__, __LINE__);
        return -EFAULT;
    }
	di->irq = gpio_to_irq(di->interrput_gpio);
	ret = gpio_request_one(di->interrput_gpio, GPIOF_IN, "Scharger_int");
	if (ret) {
		hwlog_err("no card-detect pin available!\n");
		goto err_irq_gpio;
	}
	ret = request_irq(di->irq, hi6521_irq_handle,
		 IRQF_TRIGGER_LOW | IRQF_DISABLED | IRQF_NO_SUSPEND , di->client->name, di);
	if (ret) {
		hwlog_err("request Scharger irq error\n");
		goto err_irq_request;
	}

    INIT_DELAYED_WORK(&di->hi6521_charger_work,
        hi6521_charger_work);

#if DPM_CHECK_FOR_V204
    INIT_DELAYED_WORK(&di->hi6521_dpm_check_work,
        hi6521_dpm_check_work);
#endif
    INIT_DELAYED_WORK(&di->hi6521_usb_otg_work,
        hi6521_usb_otg_work);

    INIT_WORK(&di->usb_work, hi6521_usb_charger_work);

    ret = sysfs_create_group(&client->dev.kobj, &hi6521_attr_group);
    if (ret) {
        dev_dbg(&client->dev, "could not create sysfs files\n");
        goto err_sysfs;
    }

#ifndef _DRV_LLT_
    power_class = hw_power_get_class();
    //charge_class = class_create(THIS_MODULE, "hw_power");
    if(power_class)
    {
        new_dev = device_create(power_class, NULL, 0, "%s", "charger");
        if(new_dev)
        {
            ret = sysfs_create_link(&new_dev->kobj, &client->dev.kobj, "charge_data");
            if(0 != ret)
                hwlog_err("%s, create sysfs link failed!!!\n", __FUNCTION__);
        }
        else
        {
            hwlog_err("%s, create new_dev failed!!!\n", __FUNCTION__);
        }
    }
#endif
    /*ret = sysfs_create_group(&client->dev.kobj, &hi6521_attr_group);*/

    di->nb.notifier_call = hi6521_usb_notifier_call;

    ret = hiusb_charger_registe_notifier(&di->nb);
    if (ret < 0) {
       hwlog_err("hiusb_charger_registe_notifier failed\n");
       goto err_hiusb;
    }

    plugin_stat = get_charger_name();
    if (CHARGER_TYPE_MIN < plugin_stat && plugin_stat < CHARGER_TYPE_MAX) {
        di->event = plugin_stat;
    }
    else {
        di->event = CHARGER_REMOVED;
    }
    schedule_work(&di->usb_work);

#ifdef CONFIG_HUAWEI_HW_DEV_DCT
    /* detect current device successful, set the flag as present */
    set_hw_dev_flag(DEV_I2C_CHARGER);
#endif

#if defined (CONFIG_HUAWEI_DSM)
	if (!charger_dclient) {
		charger_dclient = dsm_register_client(&dsm_charger);
	}
#endif

    g_di = di; //h00121290 add for usb stub
    scharger_di = di;
	scharger_notify_prepare();/*notify backright open*/
    return 0;

err_hiusb:
    sysfs_remove_group(&client->dev.kobj, &hi6521_attr_group);
err_sysfs:
	free_irq(di->irq,di);
err_irq_request:
	gpio_free(di->interrput_gpio);
err_irq_gpio:
	destroy_workqueue(di->scharger_int_workqueue);
    di->scharger_int_workqueue = NULL;
err_int_work:
	if (BAT_BOARD_SFT == is_board_type){
		gpio_free(di->gpio_ce);
	}
    wake_lock_destroy(&chrg_lock);
    wake_lock_destroy(&stop_chrg_lock);
err_io:
err_kfree:
    kfree(di);
    di = NULL;

    return ret;
}

STATIC int  hi6521_charger_remove(struct i2c_client *client)
{
    struct hi6521_device_info *di = i2c_get_clientdata(client);

    hiusb_charger_unregiste_notifier(&di->nb);
    sysfs_remove_group(&client->dev.kobj, &hi6521_attr_group);
    wake_lock_destroy(&chrg_lock);
#if DPM_CHECK_FOR_V204
    cancel_delayed_work(&di->hi6521_dpm_check_work);
#endif
    wake_lock_destroy(&stop_chrg_lock);

    cancel_delayed_work(&di->hi6521_charger_work);
    flush_scheduled_work();
    cancel_delayed_work_sync(&di->hi6521_usb_otg_work);
    //gpio_set_value(di->gpio, 1);
    if (BAT_BOARD_SFT == is_board_type){
		gpio_free(di->gpio_ce);
	}

	free_irq(di->irq,di);
	gpio_free(di->interrput_gpio);
	destroy_workqueue(di->scharger_int_workqueue);
	di->scharger_int_workqueue = NULL;
    kfree(di);

    return 0;
}

STATIC void hi6521_charger_shutdown(struct i2c_client *client)
{
    struct hi6521_device_info *di = i2c_get_clientdata(client);

#if DPM_CHECK_FOR_V204
    cancel_delayed_work(&di->hi6521_dpm_check_work);
#endif
    cancel_delayed_work(&di->hi6521_charger_work);
    cancel_delayed_work_sync(&di->hi6521_usb_otg_work);
	//cancel_work_sync(&di->hi6521_irq_work);
    return;
}

STATIC const struct i2c_device_id hi6521_id[] = {
    { "hi6521_charger", 0 },
    {},
};

#ifdef CONFIG_PM
STATIC int hi6521_charger_suspend(struct i2c_client *client,
              pm_message_t state)
{
    struct hi6521_device_info *di = i2c_get_clientdata(client);

    if(di->charger_source == POWER_SUPPLY_TYPE_MAINS){
        if(di->battery_full){
            if (!wake_lock_active(&chrg_lock)){
                cancel_delayed_work(&di->hi6521_charger_work);
                if((wakeup_timer_seconds > 300) || !wakeup_timer_seconds)
                    wakeup_timer_seconds = 300;
            }
        }
    }

    hi6521_config_power_on_reg01(di);
    return 0;
}

STATIC int hi6521_charger_resume(struct i2c_client *client)
{
    struct hi6521_device_info *di = i2c_get_clientdata(client);

    if(di->charger_source == POWER_SUPPLY_TYPE_MAINS){
        hi6521_config_fast_vol_and_term_current_reg(di);
        schedule_delayed_work(&di->hi6521_charger_work, msecs_to_jiffies(0));
    }

    hi6521_config_power_on_reg01(di);
   return 0;
}
#else
#define hi6521_charger_suspend       NULL
#define hi6521_charger_resume        NULL
#endif /* CONFIG_PM */

MODULE_DEVICE_TABLE(i2c, hi6521_id);
static struct of_device_id hi6521_charger_match_table[] =
{
    {
	.compatible = "huawei,hi6521_charger",
	.data = NULL,
    },
    {
    },
};
MODULE_DEVICE_TABLE(of, hi6521_charger_match_table);
STATIC struct i2c_driver hi6521_charger_driver = {
    .probe = hi6521_charger_probe,
    .remove = hi6521_charger_remove,
    .suspend = hi6521_charger_suspend,
    .resume = hi6521_charger_resume,
    .shutdown = hi6521_charger_shutdown,
    .id_table = hi6521_id,
    .driver = {
         .owner = THIS_MODULE,
         .name = "hi6521_charger",
    .of_match_table = of_match_ptr(hi6521_charger_match_table),
    },
};

STATIC int __init hi6521_charger_init(void)
{
    return i2c_add_driver(&hi6521_charger_driver);
}
//device_initcall_sync(hi6521_charger_init);

module_init(hi6521_charger_init);

STATIC void __exit hi6521_charger_exit(void)
{
    i2c_del_driver(&hi6521_charger_driver);
}
module_exit(hi6521_charger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HW Inc");

