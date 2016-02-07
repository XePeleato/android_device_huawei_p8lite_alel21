#ifndef _HISI_SMARTSTAR_V300_H_
#define _HISI_SMARTSTAR_V300_H_

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
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <bq_bci_battery.h>
#include <asm/bug.h>
#include <linux/huawei/hisi_adc.h>
#include <huawei_platform/log/hw_log.h>
#include <linux/hisi/hi6xxx_temp_cfg.h>
#include <linux/mtd/hisi_nve_interface.h>
#include <soc_smart_interface.h>
#include "hisi_battery_data.h"
#include <hisi_coul_drv.h>

#define RBATT_ADJ 1

#define GPIO_4_1    33
#define GPIO_4_3    35

#define SMARTSTAR_COUL_IRQ_REG      SOC_SMART_COUL_IRQ_ADDR(0)
#define SMARTSTAR_COUL_IRQ_MASK_REG SOC_SMART_COUL_IRQ_MASK_ADDR(0)
#define SMARTSTAR_VOL_OFFSET_ADDR   SOC_SMART_OTP_PDOB3_ADDR(0)
#define COUL_WORK_MODE_ADDR   SOC_SMART_STATUS2_ADDR(0)
#define SMARTSTAR_COUL_VERSION_ADDR  SOC_SMART_VERSION_ADDR(0)
#define SMARTSTAR_CURRENT SOC_SMART_CURRENT_0_ADDR(0)
#define SMARTSTAR_V_OUT     SOC_SMART_V_OUT_0_ADDR(0)
#define SMARTSTAR_COUL_CTRL_REG SOC_SMART_CLJ_CTRL_REG_ADDR(0)
#define SMARTSTAR_CL_OUT_BASE   SOC_SMART_CL_OUT0_ADDR(0)
#define SMARTSTAR_CL_IN_BASE    SOC_SMART_CL_IN0_ADDR(0)
#define SMARTSTAR_CHG_TIMER_BASE SOC_SMART_CHG_TIMER0_ADDR(0)
#define SMARTSTAR_LOAD_TIMER_BASE SOC_SMART_LOAD_TIMER0_ADDR(0)
#define SMARTSTAR_CL_INT_BASE SOC_SMART_CL_INT0_ADDR(0)
#define SMARTSTAR_VOL_INT_BASE    SOC_SMART_V_INT0_ADDR(0)
#define SMARTSTAR_OFFSET_CURRENT SOC_SMART_OFFSET_CURRENT0_ADDR(0)
#define SMARTSTAR_OFFSET_VOLTAGE SOC_SMART_OFFSET_VOLTAGE0_ADDR(0)
#define SMARTSTAR_OCV_VOLTAGE_BASE  SOC_SMART_OCV_VOLTAGE0_ADDR(0)
#define SMARTSTAR_OCV_CURRENT_BASE  SOC_SMART_OCV_CURRENT0_ADDR(0)
#define SMARTSTAR_ECO_OUT_CLIN_REG_BASE  SOC_SMART_ECO_OUT_CLIN_0_ADDR(0)
#define SMARTSTAR_ECO_OUT_CLOUT_REG_BASE  SOC_SMART_ECO_OUT_CLOUT_0_ADDR(0)
#define SMARTSTAR_VOL_FIFO_BASE SOC_SMART_V_OUT0_PRE0_ADDR(0)
#define SMARTSTAR_CUR_FIFO_BASE SOC_SMART_CURRENT0_PRE0_ADDR(0)
#define SMARTSTAR_COUL_STATE_REG SOC_SMART_STATE_TEST_ADDR(0)   //Warning: bit change
#define SMARTSTAR_FIFO_CLEAR SOC_SMART_CLJ_DEBUG_2_ADDR(0)         //use bit 3
#define SMARTSTAR_BATTERY_MOVE_ADDR  SOC_SMART_CLJ_RESERVED1_ADDR(0)
#define NV_READ_BITMASK (1 << 1)
#define NV_SAVE_BITMASK (1 << 0)
#define SMARTSTAR_NV_SAVE_SUCCESS  SOC_SMART_CLJ_RESERVED2_ADDR(0)  //use bit 0
#define SMARTSTAR_NV_READ_SUCCESS SOC_SMART_CLJ_RESERVED2_ADDR(0) //use bit 1
#define IGNORE_DELTA_RC_SCENE     (1 << 3)
#define USE_DELTA_RC_SCENE        (0 << 3)
#define DELTA_RC_SCENE_BITMASK    (1 << 3)
#define SMARTSTAR_DELTA_RC_SCENE  SOC_SMART_CLJ_RESERVED2_ADDR(0)/*use bit 3*/
#define SMARTSTAR_SAVE_OCV_ADDR SOC_SMART_CLJ_RESERVED3_ADDR(0) /*use 2byte*/
#define SMARTSTAR_DEBUG_REG SOC_SMART_CLJ_DEBUG_ADDR(0)
#define SMARTSTAR_CLK19M2_600_586_EN  (0xFE)
#define SMARTSTAR_COUL_OFFSET_CUR_MODIFY_BASE SOC_SMART_OFFSET_CURRENT_MOD_0_ADDR(0)
#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
#define SMARTSTAR_COUL_SET_TEMP_PROTECT_FLAG  SOC_SMART_CLJ_RESERVED2_ADDR(0)
#define MASK_BIT4 0XEF
#define SET_BIT4  0X10
#endif

#ifdef _DRV_LLT_
//struct smartstar_coul_device ut_smartstar_coul;
struct smartstar_coul_device *di;
extern char g_ut_register[500];
extern struct smartstar_coul_device ut_smartstar_coul;
extern struct smartstar_coul_device * ut_platform_get_drvdata(struct platform_device *pdev);
extern struct hisi_coul_ops* ut_malloc_ops(int size, int flags);
extern struct smartstar_coul_device * ut_malloc_device(int size, int flags);
extern char ut_pmussi_reg_read(unsigned int reg);
extern void ut_pmussi_reg_write(unsigned int reg,char regval);
extern void ut_pmussi_recv_data(unsigned int reg,char *buf, int len);
extern void ut_pmussi_send_data(unsigned int reg,char *buf, int len);
extern int printf (__const char *__restrict __format, ...);
#define SMARTSTAR_COUL_ERR(fmt,args...) do { printf("[smartstar]" fmt, ## args); } while (0)
#define SMARTSTAR_COUL_EVT(fmt,args...) do { printf("[smartstar]" fmt, ## args); } while (0)
#define SMARTSTAR_COUL_INF(fmt,args...) do { printf("[smartstar]" fmt, ## args); } while (0)

#define SMARTSTAR_REG_READ(regAddr)             ut_pmussi_reg_read(regAddr)
#define SMARTSTAR_REG_WRITE(regAddr,regval)     ut_pmussi_reg_write((unsigned int)(regAddr),(char)regval)

#define SMARTSTAR_REGS_READ(regAddr,buf,size)   ut_pmussi_recv_data((unsigned int)(regAddr),(char*)(buf),(int)(size))
#define SMARTSTAR_REGS_WRITE(regAddr,buf,size)  ut_pmussi_send_data((unsigned int)(regAddr),(char*)(buf),(int)(size))

#else
extern unsigned char hi6xxx_pmic_reg_read (unsigned int reg_addr);
extern unsigned char hi6xxx_pmic_reg_write (unsigned int reg_addr, char regval);
extern int hi6xxx_pmic_recv_data (unsigned int reg_addr, char *buffer, int len);
extern int hi6xxx_pmic_send_data (unsigned int reg_addr, char *buffer, int len);

#define SMARTSTAR_REG_READ(regAddr)             hi6xxx_pmic_reg_read(regAddr)
#define SMARTSTAR_REG_WRITE(regAddr,regval)     hi6xxx_pmic_reg_write((unsigned int)(regAddr),(char)regval)
#define SMARTSTAR_REGS_READ(regAddr,buf,size)   hi6xxx_pmic_recv_data((unsigned int)(regAddr),(char*)(buf),(int)(size))
#define SMARTSTAR_REGS_WRITE(regAddr,buf,size)  hi6xxx_pmic_send_data((unsigned int)(regAddr),(char*)(buf),(int)(size))


#define SMARTSTAR_COUL_ERR(fmt,args...) do { printk(KERN_ERR "[smartstar]" fmt, ## args); } while (0)
#define SMARTSTAR_COUL_EVT(fmt,args...) do { printk(KERN_ERR "[smartstar]" fmt, ## args); } while (0)
#define SMARTSTAR_COUL_INF(fmt,args...) do { printk(KERN_ERR "[smartstar]" fmt, ## args); } while (0)


#endif

#define DI_LOCK()    do {mutex_lock(&di->soc_mutex);} while (0)
#define DI_UNLOCK()  do {mutex_unlock(&di->soc_mutex);} while (0)
#define NV_WRITE                0           /*NV write operation*/
#define NV_READ                 1           /*NV read  operation*/
#define ABNORMAL_BATT_TEMPERATURE_LOW   (-40)
#define ABNORMAL_BATT_TEMPERATURE_HIGH  (80)

#define DEFAULT_TEMP 25         /* SFT  UDP */

#define BATTERY_EXIST_FLAG  0x1
#define BATTERY_MOVE_MAGIC_NUM        0xc3

#define TEMP_TOO_HOT            (60)
#define TEMP_TOO_COLD          (-20)


#define CHARGING_STATE_CHARGE_START         (1)
#define CHARGING_STATE_CHARGE_DONE          (2)
#define CHARGING_STATE_CHARGE_STOP          (3)
#define CHARGING_STATE_CHARGE_RECHARGE      (4)
#define CHARGING_STATE_CHARGE_NOT_CHARGE    (5)
#define CHARGING_STATE_CHARGE_UNKNOW        (0x0)

#define SMARTSTAR_DEBUG

#define IAVG_SAMPLES 10
#define CHARGING_IAVG_MA 250
#define R_COUL_MOHM     10		/* resisitance mohm */

#define DELTA_FCC_PERCENT 20
#define CALCULATE_SOC_MS    (20*1000)

#define CALIBRATE_INTERVAL (5*60*1000)	/* 5 min */

#define MIN_CHARGING_CURRENT_OFFSET (-10) 
/************************************************************
    coul register of smartstar
************************************************************/
#define COUL_CTRL_ENABLE     (1<<7)
#define COUL_CALI_ENABLE     (1<<6)
#define COUL_ECO_FLT_20MS    (0)
#define COUL_ECO_FLT_25MS    (0x1<<4)
#define COUL_ECO_FLT_30MS    (0x2<<4)
#define COUL_ECO_FLT_35MS    (0x3<<4)
#define COUL_ALL_REFLASH    (0x0)
#define COUL_ECO_REFLASH    (0x1<<3)
#define COUL_ECO_ENABLE     (0x1<<2)
#define COUL_ECO_PMU_EN     (0x3)   /*enable enter ECO with PMU, and enable signal filter*/
#define COUL_ECO_DISABLE    (0x0)
#define COUL_FIFO_CLEAR     (1 << 0x3)


#define SMARTSTAR_LOW_VOL_INT_LVL1_SOC      (15)
#define SMARTSTAR_LOW_VOL_INT_LVL2_SOC      (2)
#define SMARTSTAR_LOW_VOL_INT_LVL1_FLAG    (1<<0)
#define SMARTSTAR_LOW_VOL_INT_LVL2_FLAG    (1<<1)
#define SMARTSTAR_LOW_VOL_INT_MASK   (SMARTSTAR_LOW_VOL_INT_LVL1_FLAG | SMARTSTAR_LOW_VOL_INT_LVL2_FLAG)

#ifdef SMARTSTAR_DEBUG
#define DBG_CNT_INC(xxx)     di->xxx++
#else
#define DBG_CNT_INC(xxx)
#endif

#define BATTERY_CC_FULL_LEV  100
#define BATTERY_CC_WARNING_LEV  10
#define BATTERY_CC_LOW_LEV      3
#define BATTERY_VOL_WARNING 3550
#define BATTERY_VOL_LOW 3350
#define BATTERY_SWITCH_ON_VOLTAGE        (3250)
#define BATTERY_VOL_LOW_ERR 2900
#define BATTERY_NORMAL_CUTOFF_VOL 3150

#define LOW_INT_NOT_SET 0
#define LOW_INT_PROMPT 1
#define LOW_INT_ALARM 2
#define LOW_INT_ZERO 3

#define ZERO_VOLTAGE 3200

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

#define SS_COUL_NV_NAME  "SS_COUL"
#define SS_COUL_NV_NUM   298


#define MIN_BEGIN_PERCENT_FOR_LEARNING 20
#define LOW_INT_STATE_RUNNING 1
#define LOW_INT_STATE_SLEEP 0
#define BATTERY_VOL_2_PERCENT 3350
#define BATTERY_CAPACITY_WARNING 10
#define BATTERY_CAPACITY_SHUNDONW 2

#define CURRENT_LIMIT (20*1000)
#define DEFAULT_BATTERY_OHMIC_RESISTANCE 100 /* mohm */
#define FLAG_USE_CLAC_OCV 0xABCD
#define FLAG_USE_PMU_OCV 0xCDEF
#define IRQ_MAX 4

static unsigned int hand_chg_capacity_flag = 0;
static unsigned int input_capacity = 50;
static int hisi_saved_abs_cc_mah = 0;
static int disable_temperature_debounce = 1;

static int sr_time_sleep[SR_ARR_LEN];
static int sr_time_wakeup[SR_ARR_LEN];
static int sr_index_sleep = 0;
static int sr_index_wakeup = 0;
static int sr_cur_state = 0;    // 1:wakeup  2:sleep

#define DEFAULT_V_OFF_A 1014000
#define DEFAULT_V_OFF_B 0
#define DEFAULT_C_OFF_A 1000000
#define DEFAULT_C_OFF_B 0
#define DEFAULT_RPCB 10000 /*uohm*/

static int v_offset_a = DEFAULT_V_OFF_A;		
static int v_offset_b = 0;			            
static int c_offset_a = DEFAULT_C_OFF_A;		
static int c_offset_b = 0;			            

#define CONFIG_PM    1
#define COUL_REGDATA_TIME  10986
#define BIT_FOR_UA  4689990
#define BIT_FOR_UAH_TCXO  143127
#define BIT_FOR_UAH_DCXO_600 146562
#define BIT_FOR_UAH_DCXO_586 143142
#define BIT_FOR_UV    2400000
#define FIFO_DATA_RECOVERY    0x8
#define COUL_VERSION_MIN      0x10
#define COUL_VERSION_MAX  	  0x38
#define HI6553_COUL_VERSION_MIN   0x10
#define HI6553_COUL_VERSION_MAX   0x20
#define DCXO_600          0
#define DCXO_586          1
#define DEFAULT_COUL_CTRL_VAL   (COUL_CTRL_ENABLE | COUL_ECO_FLT_20MS  \
                                | COUL_ALL_REFLASH |COUL_ECO_DISABLE )

#define ECO_COUL_CTRL_VAL   	(COUL_CTRL_ENABLE | COUL_ECO_FLT_35MS  \
                                | COUL_ECO_REFLASH |COUL_ECO_PMU_EN )
#define  COUL_VBAT_LOW_INT      (0x1<<3)
#define  COUL_CCIN_CNT_INT      (0x1<<2)
#define  COUL_CCOUT_CNT_INT     (0x1<<1)
#define  COUL_CCOUT_BIG_INT     (0x1<<0)
#define DEFAULT_INT_MASK_VAL    (0x0)

#define FIFO_DEPTH 10
#define BATTERY_MOVED_FLAG 0x1
#define FLAG_NV_READ  0X20
#define FLAG_NV_WRITE 0x10
#define FLAG_NV_FIFO  0x08
#define COUL_CALI_ING     (0x3)
#define TCXO_MODE 0x8
#define DCXO_MODE 0x0
#define PMU_WORK_MODE_MSK  0x8                     // bit 3 tells DCXO or TCXO
#define CHARGING_CURRENT_OFFSET (-10*1000)
#define BATTERY_CHECK_TIME_MS (1*1000)
struct ss_coul_nv_info{
    int charge_cycles;
    int r_pcb; // uohm
    int v_offset_a;
    int v_offset_b;
    int c_offset_a;
    int c_offset_b;
    short temp[MAX_TEMPS];
    short real_fcc[MAX_TEMPS];
    short calc_ocv_reg_v;
    short calc_ocv_reg_c;
    short hkadc_batt_temp;
    short hkadc_batt_id_voltage;
    int start_cc;
    short ocv_temp;
    int limit_fcc;
};

struct smartstar_coul_device
{
    int batt_exist;
    int prev_pc_unusable;
    int irqs[IRQ_MAX];
    unsigned char irq_mask;
    int batt_ocv_cur_limit;
    int batt_ocv_curreglimit;
    int batt_ocv; // ocv in uv
    int batt_ocv_temp;
    int batt_ocv_valid_to_refresh_fcc;
    int batt_changed_flag;
    int batt_vol_low_flag;
    int soc_limit_flag;
    int batt_uf_temp;
    int batt_temp; // temperature in degree*10
    int batt_fcc;
    int batt_limit_fcc;
    int batt_rm;
    int batt_ruc;
    int batt_uuc;
    int batt_delta_rc;
    int rbatt;
    int rbatt_ratio;
    int r_pcb;
    int soc_work_interval;
    int charging_begin_soc;
    int charging_state;
    int charging_stop_time;
    int batt_soc;
    int batt_soc_real;
    int batt_soc_with_uuc;
    int batt_soc_est;
    int product_index;
    unsigned int batt_id_vol;
    unsigned int batt_chargecycles; //chargecycle in percent
    int last_cali_temp; // temperature in degree*10
    int cc_end_value;
    //int cc_start_value;
    unsigned int v_cutoff;
    unsigned int v_low_int_value;
    unsigned int	get_cc_start_time;
    unsigned int 	get_cc_end_time;
    unsigned int   suspend_time; // time when suspend
    int   sr_suspend_time;  // sr time when suspend
    int   sr_resume_time;   // sr time when resume
    int charging_begin_cc;  /*the unit is uah*/
    int suspend_cc; // cc when suspend
    unsigned int last_time;
    int last_cc;
    int last_iavg_ma;
    int last_fifo_iavg_ma;
    struct mutex soc_mutex;
    struct hisi_smartstar_coul_battery_data *batt_data;
    struct single_row_lut	*adjusted_fcc_temp_lut;
    struct single_row_lut	adjusted_fcc_temp_lut_tbl1;
    struct single_row_lut	adjusted_fcc_temp_lut_tbl2;
    int fcc_real_mah;
    struct delayed_work	notifier_work;
    struct delayed_work	calculate_soc_delayed_work;
    struct delayed_work	battery_check_delayed_work;
    struct hisi_coul_ops *   ops;
    struct ss_coul_nv_info nv_info;
    int is_nv_read;
    int is_nv_need_save;

#ifdef SMARTSTAR_DEBUG
    unsigned int dbg_ocv_cng_0; /*ocv change count by wake up*/
    unsigned int dbg_ocv_cng_1; /*ocv change count by full charged*/
    unsigned int dbg_valid_vol; /*vaild voltage from FIFO vol registers*/
    unsigned int dbg_invalid_vol; /*invaild voltage from FIFO vol registers*/
    unsigned int dbg_ocv_fc_failed; /*full charged can't update OCV*/
#endif
};

struct vcdata{
int avg_v; //mv
int avg_c; //ma
int min_c; //ma
int max_c; //ma
};

#endif


