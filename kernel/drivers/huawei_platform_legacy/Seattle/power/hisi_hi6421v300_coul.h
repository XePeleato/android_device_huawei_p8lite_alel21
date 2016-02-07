/*****************************************************************************************
* Filename:	hisi_hi6421v300_coul.h
*
* Discription:  hisi_hi6421v300 coulometer functions.
* Copyright:	(C) 2013 huawei.
*
* revision history:
* 
*
******************************************************************************************/
#ifndef __HISI_HI6421V300_COUL
#define __HISI_HI6421V300_COUL
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <asm/irq.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <huawei_platform_legacy/Seattle/power/hisi_battery_data.h>
#include <huawei_platform_legacy/Seattle/power/hisi_coul_drv.h>
#include <huawei_platform_legacy/Seattle/power/bq_bci_battery.h>
#include <linux/mtd/hisi_nve_interface.h>
#include <linux/mfd/hi3xxx_hi6421v300.h>
#include <asm/bug.h>
#include <linux/huawei/hisi_adc.h>
#include <huawei_platform/log/hw_log.h>
#include <linux/raid/pq.h>
#include <huawei_platform/dsm/dsm_pub.h>
#include <linux/reboot.h>
#include "coulomb_test_interface.h"

#define HISI_HI6421V300_DEBUG
#define RBATT_ADJ 1
#define DEFAULT_V_OFF_A 1000000
#define DEFAULT_V_OFF_B 0
#define DEFAULT_C_OFF_A 1000000
#define DEFAULT_C_OFF_B 0
#define DEFAULT_RPCB 0 /*uohm*/
#define ABNORMAL_BATT_TEMPERATURE_LOW   (-40)
#define ABNORMAL_BATT_TEMPERATURE_HIGH  (80)
#define TEMP_TOO_HOT            (60)
#define TEMP_TOO_COLD          (-20)
#define BATTERY_MOVE_MAGIC_NUM        0xc2
#define NVE_HISI_HI6421V300_COUL_NUM         316
#define HISI_HI6421V300_COUL_NV_NAME         "HICOUL"
#define HISI_BAT_CALIBRATION_NV_NAME         "BATCALI"
#define HISI_HI6421V300_COUL_NV_NUM          NVE_HISI_HI6421V300_COUL_NUM
#define HISI_BAT_CALIBRATION_NV_NUM          317

#define CHARGING_STATE_CHARGE_START         (1)
#define CHARGING_STATE_CHARGE_DONE          (2)
#define CHARGING_STATE_CHARGE_STOP          (3)
#define CHARGING_STATE_CHARGE_RECHARGE      (4)
#define CHARGING_STATE_CHARGE_NOT_CHARGE    (5)
#define CHARGING_STATE_CHARGE_UNKNOW        (0x0)

#define IAVG_SAMPLES 10
#define CHARGING_IAVG_MA 250
#define R_COUL_MOHM     10		/* resisitance mohm */

#define DELTA_FCC_PERCENT 20
#define CALCULATE_SOC_MS    (20*1000)
#define READ_TEMPERATURE_MS (5*1000)
#define TEMPERATURE_INIT_STATUS 1
#define TEMPERATURE_UPDATE_STATUS 0
#define TEMPERATURE_CHANGE_LIMIT 50

#define CALIBRATE_INTERVAL (5*60*1000)	/* 5 min */
#define CHARGING_CURRENT_OFFSET (-10*1000)

#define SS_COUL_NV_NAME  "SS_COUL"
#define SS_COUL_NV_NUM   NVE_SS_COUL_NUM
#define MIN_BEGIN_PERCENT_FOR_LEARNING 40
#define MAX_DELTA_RC_PC 1
#define HISI_ADC_CHANNEL_BAT_ID (6)
#define HISI_ADC_CHANNEL_BAT_TEMP (7)
#define INVALID_TO_UPDATE_FCC    (0x8000)
/************************************************************
    coul register of hisi_hi6421v300
************************************************************/
#define HISI_HI6421V300_REG_READ(regAddr)             hisi_hi6421v300_reg_read(regAddr)
#define HISI_HI6421V300_REG_WRITE(regAddr,regval)     hisi_hi6421v300_reg_write((unsigned int)(regAddr),( unsigned char)regval)

#define HISI_HI6421V300_REGS_READ(regAddr,buf,size)   hisi_hi6421v300_regs_read((unsigned int)(regAddr),(unsigned char*)(buf),(int)(size))
#define HISI_HI6421V300_REGS_WRITE(regAddr,buf,size)  hisi_hi6421v300_regs_write((unsigned int)(regAddr),(unsigned char*)(buf),(int)(size))
#define HISI_HI6421V300_COUL_ERR(fmt,args...) do { hwlog_err(fmt, ## args); } while (0)
#define HISI_HI6421V300_COUL_EVT(fmt,args...) do { hwlog_info(fmt, ## args); } while (0)
#define HISI_HI6421V300_COUL_INF(fmt,args...) do { hwlog_info(fmt, ## args);} while(0)
#define HISI_HI6421V300_COUL_DEBUG(fmt,args...) do { hwlog_debug(fmt, ## args);} while(0)

#define DI_LOCK()    do {mutex_lock(&di->soc_mutex);} while (0)
#define DI_UNLOCK()  do {mutex_unlock(&di->soc_mutex);} while (0)

#define COUL_CTRL_ENABLE     (1<<7)
#define COUL_CALI_ENABLE     (1<<6)
#define COUL_ECO_FLT_20MS    (0)
#define COUL_ECO_FLT_25MS    (0x1<<4)
#define COUL_ECO_FLT_30MS    (0x2<<4)
#define COUL_ECO_FLT_35MS    (0x3<<4)
#define COUL_ALL_REFLASH    (0x0)
#define COUL_ECO_REFLASH    (0x1<<3)
#define COUL_ECO_ENABLE     (0x1<<2)
#define COUL_ECO_PMU_EN     (0x3)   /*enable ECO with PMU, and enable signal filter*/
#define COUL_ECO_DISABLE    (0x0)

#define DEFAULT_COUL_CTRL_VAL   (COUL_CTRL_ENABLE | COUL_ECO_FLT_20MS  \
                                | COUL_ALL_REFLASH |COUL_ECO_DISABLE)

#define  COUL_LOW_CUR_CNT_INT   (0x1<<6)
#define  COUL_VBAT_LOW_INT      (0x1<<5)
#define  COUL_LOAD_TIMER_INT    (0x1<<4)
#define  COUL_CHG_TIMER_INT     (0x1<<3)
#define  COUL_CCIN_CNT_INT      (0x1<<2)
#define  COUL_CCOUT_CNT_INT     (0x1<<1)
#define  COUL_CCOUT_BIG_INT     (0x1<<0)

#define DEFAULT_INT_MASK_VAL    (0x01<<6)
#define COUL_CALI_ING     (0x33)

#define HISI_HI6421V300_COUL_VERSION_ADDR    (0x0)
#define HISI_HI6421V300_CALI_PARAMS_BASE 0x110
#define HISI_HI6421V300_COUL_STATE_REG       0x1C0
#define HISI_HI6421V300_COUL_IRQ_REG      0x1c9
#define HISI_HI6421V300_COUL_IRQ_MASK_REG 0x1ca
#define HISI_HI6421V300_COUL_CTRL_REG 0x14d
#define HISI_HI6421V300_CL_OUT_BASE   0x14f
#define HISI_HI6421V300_CL_IN_BASE    0x153
#define HISI_HI6421V300_CHG_TIMER_BASE 0x157
#define HISI_HI6421V300_LOAD_TIMER_BASE 0x15b
#define HISI_HI6421V300_CL_INT_BASE 0x163
#define HISI_HI6421V300_VOL_INT_BASE    0x167
#define HISI_HI6421V300_OFFSET_CURRENT 0x169
#define HISI_HI6421V300_OFFSET_VOLTAGE 0x16b
#define HISI_HI6421V300_OCV_REG_BASE  0x16d
#define HISI_HI6421V300_VOL_FIFO_BASE 0x16f
#define HISI_HI6421V300_CUR_FIFO_BASE 0x197
#define HISI_HI6421V300_OCV_CHOOSE_FLAG 0x1c5      //use bit 7
#define HISI_HI6421V300_NV_SAVE_SUCCESS 0x1c5      //use bit 5
#define HISI_HI6421V300_NV_READ_SUCCESS 0x1c5      //use bit 6
#define HISI_HI6421V300_BATTERY_MOVE_ADDR 0x1c1  //use bit[7:1] to record the battery move magic number
#define HISI_HI6421V300_SAVE_OCV_ADDR 0x1c2
#define HISI_HI6421V300_DEBUG_REG 0x1bf
#define HISI_HI6421V300_COUL_VERSION_ADDR (0x0)
#define HISI_HI6421V300_OFFSET_REG_BASE   (0x1CC)

#define DSM_COUL_CLOSE_OFFSET 1
#define VERSION_READ_RETYR_TIMES             (5)
#define HISI_HI6421V300_COUL_VERSION         (0x31)

#define USE_SAVED_OCV_FLAG                   (0x01 << 7)
#define USE_PMU_OCV_FLAG                     (~USE_SAVED_OCV_FLAG)
#define NV_SAVE_SUCCESS                      (0x01 << 5)          //use bit5 to record NV save flag
#define NV_READ_SUCCESS                      (0x01 << 6)          //use bit6 to record NV read flag
#define HISI_HI6421V300_LOW_VOL_INT_LVL1_SOC      (15)
#define HISI_HI6421V300_LOW_VOL_INT_LVL2_SOC      (2)
#define HISI_HI6421V300_LOW_VOL_INT_LVL1_FLAG    (1<<0)
#define HISI_HI6421V300_LOW_VOL_INT_LVL2_FLAG    (1<<1)
#define HISI_HI6421V300_LOW_VOL_INT_MASK   (HISI_HI6421V300_LOW_VOL_INT_LVL1_FLAG | HISI_HI6421V300_LOW_VOL_INT_LVL2_FLAG)

#ifdef HISI_HI6421V300_DEBUG
#define DBG_CNT_INC(xxx)     di->xxx++
#else
#define DBG_CNT_INC(xxx)
#endif

#define BATTERY_CC_WARNING_LEV  10
#define BATTERY_CC_LOW_LEV      3

#define BATTERY_VOL_WARNING 3550
#define BATTERY_VOL_LOW 3350
#define BATTERY_SWITCH_ON_VOLTAGE        (3250)

#define LOW_INT_NOT_SET 0
#define LOW_INT_PROMPT 1
#define LOW_INT_ALARM 2
#define LOW_INT_ZERO 3

#define ZERO_VOLTAGE_PLUS_5 3200
#define ZERO_VOLTAGE_MINUS_10 3300

#define MAX_TEMPS 10

#define INVALID_BATT_ID_VOL  -999
#define IAVG_TIME_2MIN   6 //6*20s

#define SR_ARR_LEN             100
#define SR_MAX_RESUME_TIME     90         // 90 s
#define SR_DELTA_SLEEP_TIME    (4 * 60)   // 4 min
#define SR_DELTA_WAKEUP_TIME   30         // 30 s
#define SR_TOTAL_TIME          (30 * 60)  // 30 min
#define SR_DUTY_RATIO          95
#define SR_DEVICE_WAKEUP       1
#define SR_DEVICE_SLEEP        2

unsigned char hisi_hi6421v300_reg_read(unsigned int regaddr);
void hisi_hi6421v300_reg_write(unsigned int regaddr,unsigned char val);
void hisi_hi6421v300_regs_read(unsigned int regaddr,unsigned char* buf,int size);
void hisi_hi6421v300_regs_write(unsigned int regaddr,unsigned char* buf,int size);
void hisi_hi6421v300_print_cc_reg(int c); //debug
int is_hisi_hi6421v300_fcc_debounce(void);
#endif
