/*
 * linux/drivers/power/bq_bci_battery.c
 *
 * K3:battery driver for Linux
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
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/power_supply.h>
#include <linux/wakelock.h>
#include <linux/notifier.h>
//#include <bq27510_battery.h>
//#include <bq2419x_charger.h>
#include <hi6xxx_scharger_v200.h>
#include <bq_bci_battery.h>
#include <linux/interrupt.h>
//#include <linux/irq.h>
//#include <mach/irqs.h>
//#include <linux/usb/hiusb_android.h>
//#include <mach/boardid.h>
#include <hisi_coul_drv.h>
#include <huawei_platform/log/log_exception.h>
#include <linux/timer.h>
#include <linux/rtc.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
/*#include "board_diff.h"*/
#include <huawei_platform/log/hw_log.h>

#define HWLOG_TAG bq_bci
HWLOG_REGIST();

#if defined (CONFIG_HUAWEI_DSM)
#include <huawei_platform/dsm/dsm_pub.h>
#endif

#define WINDOW_LEN                   (10)
#define LOG_ARCH_DELAY_TIME          (60000)
static struct wake_lock low_power_lock;
static int is_low_power_locked = 0;
static unsigned int    capacity_filter[WINDOW_LEN];
static unsigned int    capacity_sum;
#ifdef CONFIG_PM
static unsigned int    suspend_capacity;
#endif
extern void hisi_smartstar_print_cc_reg(int c);
extern int is_smartstar_fcc_debounce(void) ;
extern int hi6521_get_factory_flag(void);
static int bq_bci_battery_resume(struct platform_device *pdev);
static int bq_bci_battery_suspend(struct platform_device *pdev,pm_message_t state);
struct class *hw_power_get_class(void);
static int removable_batt_flag = 0;
static int is_fake_battery = 0;
module_param(is_fake_battery, int, 0644);

#if defined (CONFIG_HUAWEI_DSM)
static struct dsm_dev dsm_battery = {
	.name = "dsm_battery",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 1024,
};
static struct dsm_client *battery_dclient = NULL;
#endif

struct bq_bci_device_info {
    int    bat_voltage;
    int    bat_temperature;
    int    bat_exist;
    int    bat_health;
    int    bat_capacity;
    int    bat_capacity_level;
    int    bat_technolog;
    int    bat_rm;
    int    bat_fcc;
    short           bat_current;
    unsigned int    bat_err;
    int    bk_voltage_mV;
    int    charge_status;
    int    power_supply_status;
    u8     usb_online;
    u8     ac_online;
    u8     chargedone_stat;
    u16    monitoring_interval;
    unsigned long   event;
    unsigned int    capacity;
    unsigned int    capacity_filter_count;
    unsigned int    prev_capacity;
    unsigned int    charge_full_count;
    unsigned int    wakelock_enabled;
    struct power_supply    bat;
    struct power_supply    usb;
    struct power_supply    ac;
    struct power_supply    bk_bat;
    struct device   *dev;
    struct notifier_block    nb;
    struct delayed_work bq_bci_monitor_work;
};

struct bq_bci_device_info *g_bq_bci_dev = NULL;

BLOCKING_NOTIFIER_HEAD(notifier_list);

#define CHG_ODD_CAPACITY                (2)
#define CHG_CANT_FULL_THRESHOLD         (95) /*bad gasgauge ic can't report capacity correctly
                                              *when charging schedule near full-state.   */
#define REACH_FULL_RESAMPLE_THRESHOLD   (90)
#define REACH_EMPTY_RESAMPLE_THRESHOLD  (10)
#define NORMAL_SAMPLE_INTERVAL          (10)
#define REACH_FULL_SAMPLE_INTERVAL      (30)
#define REACH_EMPTY_SAMPLE_INTERVAL     (5)
#define CHARGE_FULL_TIME                ((40*60)/REACH_FULL_SAMPLE_INTERVAL)   /*40min = 2400s/INTERVAL*/
#define BATT_OVERVOLTAGE_THRES          (4550)
#define BATT_LOWVOLTAGE_THRES           (3100)
#define ARCH_NAME_MAX                   (256)

static enum power_supply_property bq_bci_battery_props[] = {
    POWER_SUPPLY_PROP_STATUS,
    POWER_SUPPLY_PROP_HEALTH,
    POWER_SUPPLY_PROP_PRESENT,
    POWER_SUPPLY_PROP_ONLINE,
    POWER_SUPPLY_PROP_TECHNOLOGY,
    POWER_SUPPLY_PROP_VOLTAGE_NOW,
    POWER_SUPPLY_PROP_CURRENT_NOW,
    POWER_SUPPLY_PROP_CAPACITY,
    POWER_SUPPLY_PROP_CAPACITY_LEVEL,
    POWER_SUPPLY_PROP_TEMP,
    POWER_SUPPLY_PROP_CAPACITY_RM,
    POWER_SUPPLY_PROP_CAPACITY_FCC,
    POWER_SUPPLY_PROP_VOLTAGE_MAX,
    POWER_SUPPLY_PROP_ID_VOLTAGE,
    POWER_SUPPLY_PROP_BRAND,
};

static enum power_supply_property bq_usb_props[] = {
    POWER_SUPPLY_PROP_ONLINE,
    POWER_SUPPLY_PROP_HEALTH,
};

static enum power_supply_property bq_ac_props[] = {
    POWER_SUPPLY_PROP_ONLINE,
    POWER_SUPPLY_PROP_HEALTH,
};

static enum power_supply_property bq_bk_bci_battery_props[] = {
    POWER_SUPPLY_PROP_VOLTAGE_NOW,
};

static int modem_off = false;

static void get_time_stamp(char* timestamp_buf,unsigned int len)
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

static void bq_log_exception_archive(unsigned int bat_err)
{
    /*struct bq_bci_device_info *di = g_bq_bci_dev;*/
    static unsigned int pre_bat_err = 0;
    char cmd[ARCH_NAME_MAX];
    char time_buf[16];
    int ret = 0;

    if(pre_bat_err != bat_err) {
        pre_bat_err = bat_err;
        get_time_stamp(time_buf,16);
        snprintf(cmd, ARCH_NAME_MAX, "%s%s%s%s%s%x%s",
                 "archive -i /data/android_logs/charge-log -i /data/android_logs/charge-log.1",
                        " -i /data/android_logs/kmsgcat-log -i /data/android_logs/kmsgcat-log.1",
                        " -o ", time_buf, "_BattErr0x", bat_err, " -z zip");

        ret = log_to_exception("charge",cmd);
        hwlog_info("bq_log, log_to_exception, ret:%d\n", ret);
        if(ret < 0 ){
            pre_bat_err = 0;
        }
    }
}

static struct platform_device *g_pdev = NULL;//for debug

static int bq_bci_running = 1;	// 1 is running, 0 is suspend
static int bq_bci_state_ops_set(const char *buffer,
							  const struct kernel_param *kp)
{
/*	struct smartstar_coul_device *di = g_smartstar_coul_dev;  --no use  */
	int run = 0;
	run = buffer[0] - '0';
	if (run && !bq_bci_running)
	{
		bq_bci_running = 1;
		bq_bci_battery_resume(g_pdev);
	}
	else if (!run && bq_bci_running)
	{
		pm_message_t pm = { 0 };
		bq_bci_running = 0;
		bq_bci_battery_suspend(g_pdev, pm);
	}
	return 0;
}

static int bq_bci_state_ops_get(char *buffer, const struct kernel_param *kp)
{
	sprintf(buffer, "%d", bq_bci_running);
	return strlen(buffer);
}

static struct kernel_param_ops bq_bci_state_ops = {
	.set = bq_bci_state_ops_set,
	.get = bq_bci_state_ops_get,
};

module_param_cb(bq_bci_running, &bq_bci_state_ops, &bq_bci_running, 0644);

static int get_factory_flag(void)
{
	int factory_flag = 0;
	factory_flag = hi6521_get_factory_flag();

	return factory_flag;
}

/*
 * Judge the bit which is set to 1
 */
#if defined (CONFIG_HUAWEI_DSM)
static int get_bit(unsigned int reg)
{
	int bit = 0, ret_val = -1;

	for (bit = 0; bit < 32; bit++) {
		if ((1UL << bit) & reg) {
			ret_val = bit;
			break;
		}
	}

    return ret_val;
}
#endif

int notify_mdm_off_to_pm(void)
{
    if (system_state != SYSTEM_RUNNING)
        return 0;
//    if ((hisi_battery_voltage() < 3400)&&
//        (get_charger_name() == CHARGER_REMOVED)){
    if (hisi_battery_voltage() < 3400){
        modem_off = true;
        return 1;
    }
    return 0;
}

static int calc_capacity_from_voltage(void)
{
    int data  = 50;
    int battery_voltage = 0;

    battery_voltage = hisi_battery_voltage();//bq27510_battery_voltage(&dev27510);
    if (battery_voltage < BAT_VOL_3500)
        data = 2;
    else if (battery_voltage < BAT_VOL_3550 && battery_voltage >= BAT_VOL_3500)
        data = 10;
    else if (battery_voltage < BAT_VOL_3600 && battery_voltage >= BAT_VOL_3550)
        data = 20;
    else if (battery_voltage < BAT_VOL_3700 && battery_voltage >= BAT_VOL_3600)
        data = 30;
    else if (battery_voltage < BAT_VOL_3800 && battery_voltage >= BAT_VOL_3700)
        data = 40;    
     else if (battery_voltage < BAT_VOL_3850 && battery_voltage >= BAT_VOL_3800)
        data = 50;    
    else if (battery_voltage < BAT_VOL_3900 && battery_voltage >= BAT_VOL_3850)
        data = 60;
    else if (battery_voltage < BAT_VOL_3950 && battery_voltage >= BAT_VOL_3900)
        data = 65;
    else if (battery_voltage < BAT_VOL_4000 && battery_voltage >= BAT_VOL_3950)
        data = 70;
    else if (battery_voltage < BAT_VOL_4250 && battery_voltage >= BAT_VOL_4000)
        data = 85;
    else if (battery_voltage >= BAT_VOL_4250)
       data = 100;
    return data;
}


/*only charge-work can not reach full(95%).Change capacity to full after 40min.*/
static int bq_force_full_timer(int curr_capacity, struct bq_bci_device_info *di)
{
    if(curr_capacity > CHG_CANT_FULL_THRESHOLD) {
        di->charge_full_count++;
        if(di->charge_full_count >= CHARGE_FULL_TIME){
            hwlog_info("FORCE_CHARGE_FULL = %d\n",curr_capacity);
            di->charge_full_count = CHARGE_FULL_TIME;
            curr_capacity = 100;
        }
    } else {
        di->charge_full_count = 0;
    }

    return curr_capacity;
}

static int bq_capacity_pulling_filter(int curr_capacity, struct bq_bci_device_info *di)
{
    int index = 0;
    int i,cnt;

    index = di->capacity_filter_count%WINDOW_LEN;
/*
    capacity_sum -= capacity_filter[index];
    capacity_filter[index] = curr_capacity;
    capacity_sum += capacity_filter[index];

    if (++di->capacity_filter_count >= WINDOW_LEN) {
        di->capacity_filter_count = 0;
    }
	
    curr_capacity = (capacity_sum*10)/WINDOW_LEN;
    curr_capacity = (curr_capacity+5)/10;
*/
    capacity_filter[index] = curr_capacity;

    di->capacity_filter_count++;

    if (di->capacity_filter_count > WINDOW_LEN) {
        cnt = WINDOW_LEN;
    }
    else {
        cnt = di->capacity_filter_count;
    }

    capacity_sum = 0;
    for (i=0; i<cnt; i++) {
        capacity_sum += capacity_filter[i];
    }

    /*rounding-off 0.5 method*/
    curr_capacity = (capacity_sum*10)/cnt;
    curr_capacity = (curr_capacity+5)/10;
    return curr_capacity;
}

static int capacity_changed(struct bq_bci_device_info *di)
{
    int curr_capacity = 0;
    int low_bat_flag = is_hisi_battery_reach_threshold();
    int batt_old_exist;
    int battery_volt = 0, max_charge_volt = 0, vrechg_hys =0;

    batt_old_exist = di->bat_exist;
    di->bat_exist = is_hisi_battery_exist();
    if (batt_old_exist^di->bat_exist){
        hwlog_info("Battery Present changed = %d!\n", di->bat_exist);
        return 1;
    }

    if(!di->bat_exist){
        di->capacity_filter_count = 0;
    }

    /* if battery is not present we assume it is on battery simulator
    *  if we are in factory mode, BAT FW is not updated yet, we use volt2Capacity
    */
    if ((!di->bat_exist) || (is_fake_battery) ||
            (strstr(saved_command_line, "androidboot.swtype=factory") && (COUL_BQ27510 == hisi_coulometer_type()))
         ){
        curr_capacity = calc_capacity_from_voltage();
    } else {
        curr_capacity = hisi_battery_capacity();
    }

    if((low_bat_flag & BQ_FLAG_LOCK) != BQ_FLAG_LOCK && is_low_power_locked){
        wake_unlock(&low_power_lock);
        is_low_power_locked = 0;
    }

    /* Debouncing of power on init. */
    if (di->capacity == -1) {
        di->capacity = curr_capacity;
        di->prev_capacity = curr_capacity;
        return 1;
    }

    if (modem_off) {
        hwlog_info(" modem off so shut down AP and curr_capacity = %d \n", curr_capacity);
        curr_capacity = 0;
        return 1;
    }
    /*Only availability if the capacity changed*/
    if (curr_capacity != di->prev_capacity) {
        if (abs(di->prev_capacity -curr_capacity) >= CHG_ODD_CAPACITY){
            hwlog_info("prev_capacity = %d \n"
            "curr_capacity = %d \n" "curr_voltage = %d \n",
            di->prev_capacity, curr_capacity,hisi_battery_voltage());
        }
    }

    if (curr_capacity < 2 ){
        di->capacity = curr_capacity;
        return 1;
    }

    switch(di->charge_status) {
    case POWER_SUPPLY_STATUS_CHARGING:
        curr_capacity = bq_force_full_timer(curr_capacity,di);
        break;

    case POWER_SUPPLY_STATUS_FULL:
        battery_volt = hisi_battery_voltage();
        max_charge_volt = hi6521_get_max_voltagemV();
        vrechg_hys = hi6521_get_vrechg_hys();
        if((max_charge_volt > vrechg_hys) && (battery_volt >= (max_charge_volt - vrechg_hys)))
        {
            curr_capacity = 100;
            di->charge_full_count = 0;
        }
        break;

    case POWER_SUPPLY_STATUS_DISCHARGING:
    case POWER_SUPPLY_STATUS_NOT_CHARGING:
        /*capacity-count will always decrease when discharging || notcharging*/
        if(di->prev_capacity < curr_capacity)
            return 0;
        di->charge_full_count = 0;
        break;

    default:
        hwlog_err( "%s defualt run.\n", __func__);
        break;
    }

    /*change monitoring interval from 10s to 30s when capacity greater than 90%*/
    if(curr_capacity > REACH_FULL_RESAMPLE_THRESHOLD) {
        di->monitoring_interval = REACH_FULL_SAMPLE_INTERVAL;
    } else if(curr_capacity < REACH_EMPTY_RESAMPLE_THRESHOLD) {
        di->monitoring_interval = REACH_EMPTY_SAMPLE_INTERVAL;
    } else {
        di->monitoring_interval = NORMAL_SAMPLE_INTERVAL;
    }
    /*filter*/
    curr_capacity = bq_capacity_pulling_filter(curr_capacity,di);

    if(di->prev_capacity == curr_capacity)
        return 0;

    hwlog_info("Capacity Updated = %d, charge_full_count = %d, charge_status = %d\n",
                         curr_capacity, di->charge_full_count, di->charge_status);

    di->capacity = curr_capacity;
    di->prev_capacity = curr_capacity;
    return 1;
}

static int bq_charger_event(struct notifier_block *nb, unsigned long event,
            void *_data)
{
    struct bq_bci_device_info *di;
    int ret = 0;
    di = container_of(nb, struct bq_bci_device_info, nb);

    switch (event) {
    case VCHRG_START_USB_CHARGING_EVENT:
        di->usb_online = 1;
        di->ac_online = 0;
        di->charge_status = POWER_SUPPLY_STATUS_CHARGING;
        di->power_supply_status = POWER_SUPPLY_HEALTH_GOOD;
        di->charge_full_count = 0;
        break;

    case VCHRG_START_AC_CHARGING_EVENT:
        di->ac_online = 1;
        di->usb_online = 0;
        di->charge_status = POWER_SUPPLY_STATUS_CHARGING;
        di->power_supply_status = POWER_SUPPLY_HEALTH_GOOD;
        di->charge_full_count = 0;
        break;

    case VCHRG_STOP_CHARGING_EVENT:
        di->usb_online = 0;
        di->ac_online = 0;
        di->charge_status = POWER_SUPPLY_STATUS_DISCHARGING;
        di->power_supply_status = POWER_SUPPLY_HEALTH_UNKNOWN;
        di->charge_full_count = 0;
        break;

    case VCHRG_START_CHARGING_EVENT:
        di->charge_status = POWER_SUPPLY_STATUS_CHARGING;
        di->power_supply_status = POWER_SUPPLY_HEALTH_GOOD;
        break;

    case VCHRG_NOT_CHARGING_EVENT:
        di->charge_status = POWER_SUPPLY_STATUS_NOT_CHARGING;
        di->power_supply_status = POWER_SUPPLY_HEALTH_GOOD;
        break;
    case VCHRG_CHARGE_DONE_EVENT:
   /****Do not need charge status change to full when bq24192 chargedone.
    because bq27510 will insure the charge status to full when capacity is 100.
        di->charge_status = POWER_SUPPLY_STATUS_FULL;
        di->power_supply_status = POWER_SUPPLY_HEALTH_GOOD;
    *****************************************************************/
        break;
    case VCHRG_POWER_SUPPLY_OVERVOLTAGE:
        di->charge_status = POWER_SUPPLY_STATUS_NOT_CHARGING;
        di->power_supply_status = POWER_SUPPLY_HEALTH_OVERVOLTAGE;
        break;

	case VCHRG_POWER_SUPPLY_WEAKSOURCE:
        di->charge_status = POWER_SUPPLY_STATUS_NOT_CHARGING;
	    di->power_supply_status = POWER_SUPPLY_HEALTH_UNSPEC_FAILURE;
	break;

    case BATTERY_LOW_WARNING:
        break;
    case BATTERY_LOW_SHUTDOWN:
        wake_lock(&low_power_lock);
        is_low_power_locked = 1;
        break;
    case BATTERY_MOVE:
        di->bat_exist = is_hisi_battery_exist();
        break;
    default:
        hwlog_err( "%s defualt run.\n", __func__);
        break;
    }

    if ((di->usb_online || di->ac_online) && di->capacity == 100)
        di->charge_status = POWER_SUPPLY_STATUS_FULL;
   /*in case charger can not get the report of charger removed, so
    * update the status of charger.*/
//   if(get_charger_name() == CHARGER_REMOVED){
//        di->usb_online = 0;
//        di->ac_online = 0;
//        di->charge_status = POWER_SUPPLY_STATUS_DISCHARGING;
//        di->power_supply_status = POWER_SUPPLY_HEALTH_UNKNOWN;
//        di->charge_full_count = 0;
//   }

    if(event == VCHRG_CHARGE_DONE_EVENT)
        di->chargedone_stat = 1;
    else
        di->chargedone_stat = 0;
    hwlog_info("received event = %lx, charge_status = %d\n",event,di->charge_status);
    power_supply_changed(&di->bat);
    return ret;
}

static unsigned int batt_error_mask = ~0;//set corresponding bit to 0 to stop monitoring the error type.

int bq_get_error_info(struct bq_bci_device_info *di)
{
    static int pre_bat_err =0, archive_state = 1;
    static unsigned long timeout_jiffies = 0;
    static int first_in = 1;
    static int pre_uf_capacity = 0;
    static int pre_capacity = 0;
    static int capacity_stay_count = 0;
    static int online_delay_count = 0;
    int bat_uf_capacity = 0;
    int curr_by_coultype = 1;
#if defined (CONFIG_HUAWEI_DSM)
	int dsm_bci_battery_err_offset = 0;
#endif

    if (COUL_SMARTSTAR == hisi_coulometer_type())
        curr_by_coultype = -1;

    di->bat_err = 0;

    if(!is_hisi_battery_exist())
        di->bat_err |= (ERROR_BATT_NOT_EXIST & batt_error_mask);

    if(hisi_battery_temperature()< HI6521_COLD_BATTERY_THRESHOLD
           || hisi_battery_temperature()>= HI6521_HOT_BATTERY_THRESHOLD)
    {
        if(di->charge_status == POWER_SUPPLY_STATUS_NOT_CHARGING)
            di->bat_err |= (ERROR_BATT_TEMP_STOP & batt_error_mask);
        if(di->charge_status == POWER_SUPPLY_STATUS_CHARGING)
            di->bat_err |= (ERROR_BATT_TEMP_CHARGE & batt_error_mask);

        di->bat_err |= (ERROR_BATT_TEMP_OUT & batt_error_mask);
        hwlog_info("batt temp = %d\n ", hisi_battery_temperature());
    }

    if(di->charge_status == POWER_SUPPLY_STATUS_NOT_CHARGING
        && !(get_factory_flag())) {
        if(di->power_supply_status == POWER_SUPPLY_HEALTH_OVERVOLTAGE)
            di->bat_err |= (ERROR_VBUS_OVERVOLTAGE & batt_error_mask);
        if(hisi_battery_voltage() > BATT_OVERVOLTAGE_THRES)
            di->bat_err |= (ERROR_BATT_OVERVOLTAGE & batt_error_mask);

        di->bat_err |= (ERROR_BATT_NOT_CHARGING & batt_error_mask);
    } else {
        if(hisi_battery_voltage() > BATT_OVERVOLTAGE_THRES
           || hisi_battery_voltage() < BATT_LOWVOLTAGE_THRES) {
            di->bat_err |= (ERROR_BATT_VOLTAGE & batt_error_mask);
            hwlog_info("batt volt = %d\n ", hisi_battery_voltage());
        }
    }

    if(di->chargedone_stat && hisi_battery_capacity() <= CHG_CANT_FULL_THRESHOLD){
        di->bat_err |= (ERROR_PRE_CHARGEDONE & batt_error_mask);
        hwlog_info("batt capacity = %d\n ", hisi_battery_capacity());
    }

    if(di->charge_status == POWER_SUPPLY_STATUS_CHARGING
       && hisi_battery_current_avg() <= 50
       && hisi_battery_current_avg() >= 10
       && (curr_by_coultype*hisi_battery_current()) <= 50
       && (curr_by_coultype*hisi_battery_current()) >= 10
       && di->capacity == 100) {
        di->bat_err |= (ERROR_NO_CHARGEDONE & batt_error_mask);
        hwlog_info("batt curr = %d batt curr_avg = %d\n ",
                         (curr_by_coultype*hisi_battery_current()), hisi_battery_current_avg());
    }

    if((!di->usb_online) && (!di->ac_online)){
        if((-hisi_battery_current() > 0) && (online_delay_count++ == 1)){
             di->bat_err |= (ERROR_BAD_CURR_SENSOR & batt_error_mask);
             online_delay_count = 0;
             hwlog_info("batt curr = %d\n ", (-hisi_battery_current()));
        }
    } else {
        online_delay_count = 0;
    }

    bat_uf_capacity = hisi_battery_unfiltered_capacity();
    if(first_in){
        pre_uf_capacity = bat_uf_capacity;
        pre_capacity = di->capacity;
        first_in = 0;
    } else {
        if (abs(pre_uf_capacity - bat_uf_capacity) >= 3) {
            if (100 != bat_uf_capacity) {
                di->bat_err |= (ERROR_UFCAPCITY_DEBOUNCE_OTHER & batt_error_mask);
            }
            else {
                if (abs(pre_uf_capacity - bat_uf_capacity) > 10)
                    di->bat_err |= (ERROR_UFCAPCITY_DEBOUNCE_100 & batt_error_mask);
            }
            hisi_smartstar_print_cc_reg(2); //debug
        }
        pre_uf_capacity = bat_uf_capacity;
    }

    if(di->charge_status == POWER_SUPPLY_STATUS_NOT_CHARGING
       || di->charge_status == POWER_SUPPLY_STATUS_DISCHARGING) {
        if(pre_capacity > 0 && di->capacity != pre_capacity) {
            if(di->capacity > REACH_FULL_RESAMPLE_THRESHOLD) {
                if(capacity_stay_count <= 2) {
                    di->bat_err |= (ERROR_CAPACITY_CHANGE_FAST & batt_error_mask);
                    capacity_stay_count = 0;
                }
            } else if (di->capacity < REACH_EMPTY_RESAMPLE_THRESHOLD) {
                if(capacity_stay_count <= 12) {
                    di->bat_err |= (ERROR_CAPACITY_CHANGE_FAST & batt_error_mask);
                    capacity_stay_count = 0;
                }
            } else {
                if(capacity_stay_count <= 6) {
                    di->bat_err |= (ERROR_CAPACITY_CHANGE_FAST & batt_error_mask);
                    capacity_stay_count = 0;
                }
            }
            capacity_stay_count = 0;
        } else {
            capacity_stay_count++;
        }
    }
    pre_capacity = di->capacity;

    if (is_smartstar_fcc_debounce()) {
        di->bat_err |= (ERROR_FCC_DEBOUNCE & batt_error_mask);
    }

    if(di->bat_err != pre_bat_err && archive_state == 1){
        timeout_jiffies = jiffies + msecs_to_jiffies(LOG_ARCH_DELAY_TIME);
        archive_state = 0;
        hwlog_info("(%s) BATT ERR = %x\n", hisi_battery_brand(), di->bat_err);
    }
    pre_bat_err = di->bat_err;

    if(time_is_before_jiffies(timeout_jiffies) && archive_state == 0){
#ifndef  FINAL_RELEASE_MODE
        bq_log_exception_archive(di->bat_err);
#endif

#if defined (CONFIG_HUAWEI_DSM)
		if (dsm_client_ocuppy(battery_dclient) && (di->bat_err != 0x0)) {
			dsm_client_record(battery_dclient, "battery error = %x.\n", di->bat_err);
			dsm_bci_battery_err_offset = get_bit(di->bat_err);
			dsm_client_notify(battery_dclient, DSM_BATTERY_ERROR_NO + dsm_bci_battery_err_offset);
		}
#endif

        archive_state = 1;
    }

    return di->bat_err;
}

static void bq_get_battery_info(struct bq_bci_device_info *di)
{
    di->bat_rm = hisi_battery_rm();
    di->bat_fcc = hisi_battery_fcc();

    if (!(di->bat_exist)) {
        di->bat_health         = POWER_SUPPLY_HEALTH_UNKNOWN;
        di->bat_capacity_level = POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN;
        di->bat_technolog      = POWER_SUPPLY_TECHNOLOGY_UNKNOWN;
        di->bat_temperature    = 0;
    } else {
        di->bat_health         = hisi_battery_health();
        di->bat_capacity_level = hisi_battery_capacity_level();
        di->bat_technolog      = hisi_battery_technology();
        di->bat_temperature    = hisi_battery_temperature() ;
   }
}

static void bq_bci_battery_work(struct work_struct *work)
{
    struct bq_bci_device_info *di = container_of(work,
               struct bq_bci_device_info, bq_bci_monitor_work.work);

    bq_get_battery_info(di);

    bq_get_error_info(di);

    schedule_delayed_work(&di->bq_bci_monitor_work,
                 msecs_to_jiffies(1000 * di->monitoring_interval));

    if (capacity_changed(di)){
        power_supply_changed(&di->bat);
    }
}

#define to_bq_ac_device_info(x) container_of((x), \
         struct bq_bci_device_info, ac);

static int bq_ac_get_property(struct power_supply *psy,
                    enum power_supply_property psp,
                    union power_supply_propval *val)
{
    struct bq_bci_device_info *di = to_bq_ac_device_info(psy);

    switch (psp) {
    case POWER_SUPPLY_PROP_ONLINE:
        val->intval = di->ac_online;
        break;
    case POWER_SUPPLY_PROP_HEALTH:
        val->intval = di->power_supply_status;
        break;
     default:
        return -EINVAL;
    }

    return 0;
}

#define to_bq_usb_device_info(x) container_of((x), \
         struct bq_bci_device_info, usb);

static int bq_usb_get_property(struct power_supply *psy,
                    enum power_supply_property psp,
                    union power_supply_propval *val)
{
    struct bq_bci_device_info *di = to_bq_usb_device_info(psy);

    switch (psp) {
    case POWER_SUPPLY_PROP_ONLINE:
        val->intval = di->usb_online;
        break;
    case POWER_SUPPLY_PROP_HEALTH:
        val->intval = di->power_supply_status;
        break;
     default:
        return -EINVAL;
    }

    return 0;
}

/*#define to_bq_bk_bci_device_info(x) container_of((x), \
         struct bq_bci_device_info, bk_bat);*/

static int bq_bk_bci_battery_get_property(struct power_supply *psy,
                    enum power_supply_property psp,
                    union power_supply_propval *val)
{
    //struct bq_bci_device_info *di = to_bq_bk_bci_device_info(psy);

    switch (psp) {
    case POWER_SUPPLY_PROP_VOLTAGE_NOW:
        /*Use gpadc channel for measuring bk battery voltage*/
        val->intval = 0;//bq_get_gpadc_conversion(di, ADC_VBATMON);
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

#define to_bq_bci_device_info(x) container_of((x), \
                 struct bq_bci_device_info, bat);

static int bq_bci_battery_get_property(struct power_supply *psy,
                    enum power_supply_property psp,
                    union power_supply_propval *val)
{
    struct bq_bci_device_info *di;
	
    di = to_bq_bci_device_info(psy);

    switch (psp) {
    case POWER_SUPPLY_PROP_STATUS:
        val->intval = di->charge_status;
        break;
    case POWER_SUPPLY_PROP_VOLTAGE_NOW:
        di->bat_voltage = hisi_battery_voltage();
        if (COUL_SMARTSTAR == hisi_coulometer_type()){
            val->intval = hisi_battery_voltage_uv();
        }
        else{
            val->intval = di->bat_voltage * 1000;
        }
        break;
    case POWER_SUPPLY_PROP_CURRENT_NOW:
        di->bat_current = hisi_battery_current();
        if(COUL_SMARTSTAR == hisi_coulometer_type()){
            di->bat_current = -(di->bat_current);
        }
        val->intval = di->bat_current;
        break;
    case POWER_SUPPLY_PROP_TEMP:
#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
        if(hisi_coul_reg_read())
         {
          val->intval =250;
         }
        else
        {
          val->intval = di->bat_temperature * 10;
        }
#else
        val->intval = di->bat_temperature * 10;
#endif
        break;
    case POWER_SUPPLY_PROP_PRESENT:
		val->intval = di->bat_exist;
		break;
    case POWER_SUPPLY_PROP_ONLINE:
        val->intval = di->bat_exist;
        break;
    case POWER_SUPPLY_PROP_HEALTH:
        val->intval = di->bat_health;
        break;
    case POWER_SUPPLY_PROP_CAPACITY:
        if (modem_off) {
            val->intval = 0;
        }else{
            val->intval = di->capacity;
        }
        break;
    case POWER_SUPPLY_PROP_CAPACITY_LEVEL:
        val->intval = di->bat_capacity_level;
        break;
    case POWER_SUPPLY_PROP_TECHNOLOGY:
        val->intval = di->bat_technolog;
        break;
    case POWER_SUPPLY_PROP_CAPACITY_RM:
        val->intval = di->bat_rm;
        break;
    case POWER_SUPPLY_PROP_CAPACITY_FCC:
        val->intval = di->bat_fcc;
        break;
    case POWER_SUPPLY_PROP_VOLTAGE_MAX:
         val->intval = 4200;//di->limited_charge_voltagemV; need to update p00240364
    case POWER_SUPPLY_PROP_ID_VOLTAGE:
        val->intval = hisi_battery_id_voltage();
        break;
    case POWER_SUPPLY_PROP_BRAND:
        val->strval = hisi_battery_brand();
        break;
    default:
        return -EINVAL;
    }
    return 0;
}
int bq_bci_show_capacity(void)
{
    struct bq_bci_device_info *di = g_bq_bci_dev;

    if (di)
        return di->capacity;
    else
        return 0;
}
int bq_register_notifier(struct notifier_block *nb,
                         unsigned int events)
{
    return blocking_notifier_chain_register(&notifier_list, nb);
}
EXPORT_SYMBOL_GPL(bq_register_notifier);

int bq_unregister_notifier(struct notifier_block *nb,
                           unsigned int events)
{
    return blocking_notifier_chain_unregister(&notifier_list, nb);
}
EXPORT_SYMBOL_GPL(bq_unregister_notifier);

static ssize_t bq_bci_show_batt_removable(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", removable_batt_flag);
}
static ssize_t bq_bci_set_batt_error_mask(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned long val = 0;
    unsigned int tmp = 0;
    size_t status = count;

    if(strict_strtoul(buf, 16, &val) < 0)
        return -EINVAL;

    tmp = val;
    batt_error_mask &= (~tmp);

    return status;
}

static ssize_t bq_bci_show_batt_error_mask(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "0x%x\n", batt_error_mask);
}

static DEVICE_ATTR(batt_error_mask, (S_IWUSR | S_IRUGO),
                bq_bci_show_batt_error_mask,
                bq_bci_set_batt_error_mask);
static DEVICE_ATTR(batt_removable, (S_IWUSR | S_IRUGO),
                bq_bci_show_batt_removable,
                NULL);

static struct attribute *bq_bci_attributes[] = {
    &dev_attr_batt_error_mask.attr,
    &dev_attr_batt_removable.attr,
    NULL,
};

static const struct attribute_group bq_bci_attr_group = {
    .attrs = bq_bci_attributes,
};

static char *bq_bci_supplied_to[] = {
    "bq_bci_battery",
};

static int bq_bci_battery_probe(struct platform_device *pdev)
{
    struct bq_bci_device_info *di;
    int low_bat_flag = 0;
    int ret = 0;
    unsigned int i = 0;
   struct device_node* np;

    struct class *power_class = NULL;
    struct device *new_dev = NULL;

    np = pdev->dev.of_node;
    if(NULL == np)
    {
        return -1;
    }
		
    di = kzalloc(sizeof(*di), GFP_KERNEL);
    if (!di)
        return -ENOMEM;
    g_bq_bci_dev = di;

    di->monitoring_interval = NORMAL_SAMPLE_INTERVAL;
    di->dev = &pdev->dev;
    di->bat.name = "Battery";
    di->bat.supplied_to = bq_bci_supplied_to;
    di->bat.num_supplicants = ARRAY_SIZE(bq_bci_supplied_to);
    di->bat.type = POWER_SUPPLY_TYPE_BATTERY;
    di->bat.properties = bq_bci_battery_props;
    di->bat.num_properties = ARRAY_SIZE(bq_bci_battery_props);
    di->bat.get_property = bq_bci_battery_get_property;
    di->bat_health = POWER_SUPPLY_HEALTH_GOOD;
    di->bat_exist = is_hisi_battery_exist();
    di->bat_err = 0;

    di->usb.name = "USB";
    di->usb.type = POWER_SUPPLY_TYPE_USB;
    di->usb.properties = bq_usb_props;
    di->usb.num_properties = ARRAY_SIZE(bq_usb_props);
    di->usb.get_property = bq_usb_get_property;
    di->power_supply_status = POWER_SUPPLY_HEALTH_GOOD;

    di->ac.name = "Mains";
    di->ac.type = POWER_SUPPLY_TYPE_MAINS;
    di->ac.properties = bq_ac_props;
    di->ac.num_properties = ARRAY_SIZE(bq_ac_props);
    di->ac.get_property = bq_ac_get_property;

    di->charge_status = POWER_SUPPLY_STATUS_DISCHARGING;

    di->bk_bat.name = "bq_bk_battery";
    di->bk_bat.type = POWER_SUPPLY_TYPE_UPS;
    di->bk_bat.properties = bq_bk_bci_battery_props;
    di->bk_bat.num_properties = ARRAY_SIZE(bq_bk_bci_battery_props);
    di->bk_bat.get_property = bq_bk_bci_battery_get_property;

    di->capacity = -1;
    di->capacity_filter_count = 0;
    di->charge_full_count = 0;

    for(i=0;i<WINDOW_LEN;i++) {
            capacity_filter[i] = hisi_battery_capacity();
            capacity_sum += capacity_filter[i];
    }

    bq_get_battery_info(di);

    platform_set_drvdata(pdev, di);

    wake_lock_init(&low_power_lock, WAKE_LOCK_SUSPEND, "low_power_wake_lock");

    low_bat_flag = is_hisi_battery_reach_threshold();
    if(( low_bat_flag & BQ_FLAG_LOCK ) == BQ_FLAG_LOCK) {
    	wake_lock(&low_power_lock);
    	is_low_power_locked = 1;
	}

    ret = power_supply_register(&pdev->dev, &di->bat);
    if (ret) {
        hwlog_debug("failed to register main battery\n");
        goto batt_failed;
    }

    ret = power_supply_register(&pdev->dev, &di->usb);
    if (ret) {
        hwlog_debug("failed to register usb power supply\n");
        goto usb_failed;
    }

    ret = power_supply_register(&pdev->dev, &di->ac);
    if (ret) {
        hwlog_debug("failed to register ac power supply\n");
        goto ac_failed;
    }

    ret = power_supply_register(&pdev->dev, &di->bk_bat);
    if (ret) {
        hwlog_debug("failed to register backup battery\n");
        goto bk_batt_failed;
    }

    power_class = hw_power_get_class();
    if(power_class)
    {
        new_dev = device_create(power_class, NULL, 0, "%s", "bq_bci");
        if(new_dev)
        {
            ret = sysfs_create_group(&new_dev->kobj, &bq_bci_attr_group);
            if(ret)
            {
                hwlog_err("%s, could not create sysfs files bq_bci!\n", __FUNCTION__);
            }
        }
        else
        {
            hwlog_err("%s, could not create dev bq_bci!\n", __FUNCTION__);
        }
    }
    if (of_property_read_u32(np, "battery_is_removable",&removable_batt_flag)){
        removable_batt_flag = 0;
        hwlog_err("error:get removable_batt_flag value failed!\n");
    }
    INIT_DELAYED_WORK(&di->bq_bci_monitor_work,
                bq_bci_battery_work);
    schedule_delayed_work(&di->bq_bci_monitor_work, 0);

    di->nb.notifier_call = bq_charger_event;
    bq_register_notifier(&di->nb, 1);
    hwlog_info( "bq_bci probe ok!\n");
    
#ifdef _DRV_LLT_
#else
    g_pdev = pdev;/* for debug */
#endif

#if defined (CONFIG_HUAWEI_DSM)
    if (!battery_dclient) {
       battery_dclient = dsm_register_client(&dsm_battery);
    }
#endif

    return 0;

bk_batt_failed:
    cancel_delayed_work(&di->bq_bci_monitor_work);
    power_supply_unregister(&di->ac);
ac_failed:
    power_supply_unregister(&di->usb);
usb_failed:
    power_supply_unregister(&di->bat);
batt_failed:
    wake_lock_destroy(&low_power_lock);
    platform_set_drvdata(pdev, NULL);
    kfree(di);
    di = NULL;
    return ret;
}

static int bq_bci_battery_remove(struct platform_device *pdev)
{
    struct bq_bci_device_info *di = platform_get_drvdata(pdev);

    if (di == NULL) {
        hwlog_err( "di is NULL!\n");
        return -ENODEV;
    }

    bq_unregister_notifier(&di->nb, 1);
    cancel_delayed_work(&di->bq_bci_monitor_work);

    flush_scheduled_work();

    power_supply_unregister(&di->bat);
    power_supply_unregister(&di->usb);
    power_supply_unregister(&di->ac);
    power_supply_unregister(&di->bk_bat);
    wake_lock_destroy(&low_power_lock);
    platform_set_drvdata(pdev, NULL);
    kfree(di);
    di = NULL;

    return 0;
}

static void bq_bci_battery_shutdown(struct platform_device *pdev)
{
    struct bq_bci_device_info *di = platform_get_drvdata(pdev);

    if (di == NULL) {
        hwlog_err( "di is NULL!\n");
        return;
    }

    cancel_delayed_work(&di->bq_bci_monitor_work);
    wake_lock_destroy(&low_power_lock);

    return;
}

#ifdef CONFIG_PM
static int bq_bci_battery_suspend(struct platform_device *pdev,pm_message_t state)
{
    struct bq_bci_device_info *di = platform_get_drvdata(pdev);

    if (di == NULL) {
        hwlog_err( "di is NULL!\n");
        return -ENODEV;
    }
    suspend_capacity = hisi_battery_capacity();

    cancel_delayed_work(&di->bq_bci_monitor_work);

    return 0;
}

static int bq_bci_battery_resume(struct platform_device *pdev)
{
    struct bq_bci_device_info *di = platform_get_drvdata(pdev);
    int i = 0, resume_capacity = 0;

    if (di == NULL) {
        hwlog_err( "di is NULL!\n");
        return -ENODEV;
    }
    resume_capacity = hisi_battery_capacity();
    if(di->charge_status == POWER_SUPPLY_STATUS_DISCHARGING
        || di->charge_status == POWER_SUPPLY_STATUS_NOT_CHARGING) {
        if ((suspend_capacity - resume_capacity)>=2) {
            capacity_sum = 0;
            for(i=0;i<WINDOW_LEN;i++) {
                    capacity_filter[i] = resume_capacity;
                    capacity_sum += capacity_filter[i];
            }
        }
    }
    schedule_delayed_work(&di->bq_bci_monitor_work, 0);

    return 0;
}
#endif /* CONFIG_PM */

static struct class *hw_power_class = NULL;
static int __init hw_power_class_init(void)
{
    hw_power_class = class_create(THIS_MODULE, "hw_power");

    if(IS_ERR(hw_power_class))
    {
        hwlog_err("hw_power_class create fail!!!\n");
        return PTR_ERR(hw_power_class);
    }

    return 0;
}
struct class *hw_power_get_class(void)
{
    if(IS_ERR(hw_power_class))
    {
        return NULL;
    }
    return hw_power_class;
}
EXPORT_SYMBOL_GPL(hw_power_get_class);
subsys_initcall(hw_power_class_init);

static struct of_device_id bq_bci_battery_match_table[] =
{
    {
	.compatible = "huawei,bq_bci_battery",
	.data = NULL,
    },
    {
    },
};
static struct platform_driver bq_bci_battery_driver = {
    .probe = bq_bci_battery_probe,
    .remove = bq_bci_battery_remove,
#ifdef CONFIG_PM
    .suspend = bq_bci_battery_suspend,
    .resume = bq_bci_battery_resume,
#endif
    .shutdown = bq_bci_battery_shutdown,
    .driver = {
        .name = "huawei,bq_bci_battery",
	.owner = THIS_MODULE,
	.of_match_table = of_match_ptr(bq_bci_battery_match_table),
    },
};

static int __init bq_battery_init(void)
{
    return platform_driver_register(&bq_bci_battery_driver);
}

module_init(bq_battery_init);

static void __exit bq_battery_exit(void)
{
    platform_driver_unregister(&bq_bci_battery_driver);
}

module_exit(bq_battery_exit);

MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:bq_bci");
MODULE_AUTHOR("HUAWEI Inc");
