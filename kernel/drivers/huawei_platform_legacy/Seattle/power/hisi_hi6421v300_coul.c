/*****************************************************************************************
* Filename:    hisi_hi6421v300_coul.c
*
* Discription:  hisi_hi6421v300 coulometer functions.
* Copyright:    (C) 2013 huawei.
*
* revision history:
*
*
******************************************************************************************/
#include "hisi_hi6421v300_coul.h"

#define HWLOG_TAG hisi_hi6421v300_coul
HWLOG_REGIST();
/*************************************struct type definitipn area*************************/
struct dsm_dev dsm_pmu_coul = {
    .name = "dsm_pmu_coul",
    .fops = NULL,
    .buff_size = 1024,
};

struct dsm_client *pmu_coul_dclient = NULL;

struct hisi_hi6421v300_coul_nv_info{
    int charge_cycles;
    int r_pcb; // uohm
    int v_offset_a;
    int v_offset_b;
    int c_offset_a;
    int c_offset_b;
    int start_cc;
    int limit_fcc;
    short temp[MAX_TEMPS];
    short real_fcc[MAX_TEMPS];
    short calc_ocv_reg_v;
    short calc_ocv_reg_c;
    short hkadc_batt_temp;
    short hkadc_batt_id_voltage;
    short ocv_temp;
};

struct hisi_hi6421_coul_cali_info
{
    int v_offset_a;
    int v_offset_b;
    int c_offset_a;
    int c_offset_b;
};

struct hisi_hi6421v300_coul_gauge_log
{
    int uuc_pc;
    int soc_est;
    int rbat_calc;
    int ocv_for_delrc;
    int vavg;
    int cavg;
    int delta_ocv;
    int delta_rc_mid;
    int delta_rc_100;
    int iavg_ua;
    int soc_before_limit;
};

struct hisi_hi6421v300_coul_device
{
    unsigned char irq_mask;
    int batt_exist;
    int prev_pc_unusable;
    int irq;
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
    int uuc_err;
    int batt_delta_rc;
    int batt_pre_delta_rc;
    int rbatt;
    int rbatt_ratio;
    int r_pcb;
    int soc_work_interval;
    int charging_begin_soc;
    int charging_state;
    int charging_start_time;
    int charging_stop_time;
    int batt_soc;
    int batt_soc_real;
    int batt_soc_with_uuc;
    int batt_soc_est;
    int last_cali_temp; // temperature in degree*10
    int cc_end_value;
    int cc_start_value;
    int sr_suspend_time;  // sr time when suspend
    int sr_resume_time;   // sr time when resume
    int charging_begin_cc;  /*the unit is uah*/
    int suspend_cc; // cc when suspend
    int last_cc;
    int last_iavg_ma;
    int last_fifo_iavg_ma;
    int fcc_real_mah;
    int is_nv_read;
    int is_nv_need_save;
    unsigned int batt_id_vol;
    unsigned int batt_chargecycles; //chargecycle in percent
    unsigned int v_cutoff;
    unsigned int v_low_int_value;
    unsigned int get_cc_start_time;
    unsigned int get_cc_end_time;
    unsigned int suspend_time; // time when suspend
    unsigned int last_time;
    struct mutex soc_mutex;
    struct hisi_hi6421v300_coul_battery_data *batt_data;
    struct single_row_lut *adjusted_fcc_temp_lut;
    struct single_row_lut adjusted_fcc_temp_lut_tbl1;
    struct single_row_lut adjusted_fcc_temp_lut_tbl2;
    struct delayed_work notifier_work;
    struct delayed_work calculate_soc_delayed_work;
    struct delayed_work read_temperature_delayed_work;
    struct hisi_coul_ops *ops;
    struct hisi_hi6421v300_coul_nv_info nv_info;
    struct notifier_block nb;

#ifdef HISI_HI6421V300_DEBUG
    unsigned int dbg_ocv_cng_0; /*ocv change count by wake up*/
    unsigned int dbg_ocv_cng_1; /*ocv change count by full charged*/
    unsigned int dbg_valid_vol; /*vaild voltage from FIFO vol registers*/
    unsigned int dbg_invalid_vol; /*invaild voltage from FIFO vol registers*/
    unsigned int dbg_ocv_fc_failed; /*full charged can't update OCV*/
#endif
};

/*************************************static variable definitipn area*************************/
static int hisi_saved_abs_cc_mah = 0;
static int disable_temperature_debounce = 1;
static int do_save_offset_ret;
static int sr_time_sleep[SR_ARR_LEN];
static int sr_time_wakeup[SR_ARR_LEN];
static int sr_index_sleep = 0;
static int sr_index_wakeup = 0;
static int sr_cur_state = 0;    // 1:wakeup  2:sleep
static bool udp_charge = false;
static unsigned int hand_chg_capacity_flag = 0;
static unsigned int input_capacity = 50;
static struct hi6421_pmic *pmic = NULL;
static struct hisi_hi6421v300_coul_gauge_log gauglog;
static struct hisi_hi6421v300_coul_gauge_log* glog = &gauglog;
static struct hisi_hi6421v300_coul_device *g_hisi_hi6421v300_coul_dev = NULL;
extern struct blocking_notifier_head notifier_list;

/*********************************non-static variable definitipn area*************************/
int delta_sleep_time = 10*60; // sleep time bigger could update ocv, in s
int delta_sleep_current = 50; // sleep current less could updat eocv, in mA
int v_offset_a = 1000000;     // 995729; /* need div 1000000 */
int v_offset_b = 0;           // 3258; /* uv */
int c_offset_a = 1000000;     // 977704; /* need div 1000000 */
int c_offset_b = 0;           // 6071; /* ua */

module_param(delta_sleep_time, int, 0644);
module_param(delta_sleep_current, int, 0644);

/******************************variable and function declarationn area*************************/
static int hisi_hi6421v300_battery_voltage_uv(void);
void fcc_overstep(struct hisi_hi6421v300_coul_device *di);
static int save_nv_info(struct hisi_hi6421v300_coul_device *di);

unsigned char hisi_hi6421v300_reg_read(unsigned int regaddr)
{
    unsigned char val = hi6421_pmic_read(pmic,regaddr);
    return val;
}
void hisi_hi6421v300_reg_write(unsigned int regaddr,unsigned char val)
{
    hi6421_pmic_write(pmic,regaddr,val);
}

void hisi_hi6421v300_regs_read(unsigned int regaddr,unsigned char* buf,int size)
{
    int i = 0;
    unsigned int addr = regaddr;
    while(i < size)
    {
        *buf++ = hi6421_pmic_read(pmic,addr++);
        ++i;
    }
}
void hisi_hi6421v300_regs_write(unsigned int regaddr,unsigned char* buf,int size)
{
    int i = 0;
    unsigned int addr = regaddr;
    while(i < size)
    {
        hi6421_pmic_write(pmic,addr++,*buf++);
        ++i;
    }
}

static int save_cali_param(void)
{
    int ret;
    struct hisi_nve_info_user nve;
    struct hisi_hi6421_coul_cali_info *pinfo = (struct hisi_hi6421_coul_cali_info* ) (&(nve.nv_data[0]));

    memset(&nve, 0, sizeof(nve));
    strncpy(nve.nv_name, HISI_BAT_CALIBRATION_NV_NAME, sizeof(HISI_BAT_CALIBRATION_NV_NAME));
    nve.nv_number = HISI_BAT_CALIBRATION_NV_NUM;
    nve.valid_size = sizeof(*pinfo);
    nve.nv_operation = NV_WRITE;
    pinfo->v_offset_a = v_offset_a;
    pinfo->v_offset_b = v_offset_b;

    ret = hisi_nve_direct_access(&nve);
    if (ret)
    {
        HISI_HI6421V300_COUL_ERR("save cali param failed, ret=%d\n", ret);
    }
    else
    {
        HISI_HI6421V300_COUL_INF("save cali param success\n");
    }
    return ret;
}


static int ro_ops_set(const char *buffer, const struct kernel_param *kp)
{
    return -EINVAL;
}

static int convert_regval2uv(short reg_val);
static int batt_init_ocv;        // in uv
static int batt_init_ocv_ops_get(char *buffer, const struct kernel_param *kp)
{
    short ocvreg = 0;
    short volreg_offset = 0;
    int ocv;
    /* read ocv and offset */
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_OCV_REG_BASE, &ocvreg, 2);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_OFFSET_VOLTAGE, &volreg_offset, 2);
    ocv = convert_regval2uv(ocvreg - volreg_offset);
    snprintf(buffer, PAGE_SIZE, "%d uv", ocv);
    return strlen(buffer);
}

static struct kernel_param_ops batt_init_ocv_ops = {
    .set = ro_ops_set,
    .get = batt_init_ocv_ops_get,
};

module_param_cb(batt_init_ocv, &batt_init_ocv_ops, &batt_init_ocv, 0444);
static struct platform_device *g_pdev = NULL;
static int hisi_hi6421v300_coul_suspend(struct platform_device *pdev, pm_message_t state);
static int hisi_hi6421v300_coul_resume(struct platform_device *pdev);
static int coul_running = 1;    // 1 is running, 0 is suspend
static int coul_state_ops_set(const char *buffer,const struct kernel_param *kp)
{
    int run;
    run = buffer[0] - '0';
    if (run && !coul_running)
    {
        coul_running = 1;
        hisi_hi6421v300_coul_resume(g_pdev);
    }
    else if (!run && coul_running)
    {
        pm_message_t pm = {0};
        coul_running = 0;
        hisi_hi6421v300_coul_suspend(g_pdev, pm);
    }
    return 0;
}

static int coul_state_ops_get(char *buffer, const struct kernel_param *kp)
{
    snprintf(buffer, PAGE_SIZE, "%d", coul_running);
    return strlen(buffer);
}

static struct kernel_param_ops coul_state_ops = {
    .set = coul_state_ops_set,
    .get = coul_state_ops_get,
};

module_param_cb(coul_running, &coul_state_ops, &coul_running, 0644);
static int batt_soc_with_uuc;    // in uv
static int batt_soc_with_uuc_ops_get(char *buffer, const struct kernel_param *kp)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    snprintf(buffer, PAGE_SIZE, "%d", di->batt_soc_with_uuc);
    return strlen(buffer);
}

static struct kernel_param_ops batt_soc_with_uuc_ops = {
    .set = ro_ops_set,
    .get = batt_soc_with_uuc_ops_get,
};

module_param_cb(batt_soc_with_uuc, &batt_soc_with_uuc_ops, &batt_soc_with_uuc, 0444);

static int calculate_cc_uah(void);
/**
 * clear_cc_register
 *
 * clear the cc register
 */
static void clear_cc_register(void)
{
    int ccregval = 0;

    hisi_saved_abs_cc_mah += (calculate_cc_uah() / 1000);
    HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_CL_IN_BASE,&ccregval,4);
    HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_CL_OUT_BASE,&ccregval,4);
}

/**
 * convert_regval2uah
 * 1bit = 1bit current * 0.11 c = 5/10661 * 11/100 c = 5/10661 * 11/100 * 1000/3600 mAh
 *       = 11 / (10661*2*36) mAh = 11 * 1000/ (10661 *2 *36) uAh
 *       = 11 * 125/ (10661* 9) uAh
 * convert battery voltage to uah
 */
static int convert_regval2uah(unsigned int reg_val)
{
    int ret;
    s64 temp;

    temp = reg_val;
    temp = temp * 11 *125;
    temp = div_s64(temp, (10661*9));

    ret = temp / (R_COUL_MOHM/10);

    return ret;
}

#if 1 /*debug*/
static unsigned int g_cc_in = 0;
static unsigned int g_cc_out = 0;
void hisi_hi6421v300_print_cc_reg(int c)
{
    static int cnt = 0;
    if (c==0 && cnt>0)
        cnt --;
    else
        cnt = c;

    if (cnt)
        HISI_HI6421V300_COUL_INF("reg: cc_in = 0x%x, cc_out = 0x%x\n", g_cc_in, g_cc_out);
}
#endif
/**
 * hisi_hi6421v300_battery_voltage
 *
 * battery voltage in milivolts
 */
static int calculate_cc_uah(void)
{
    unsigned int cc_in = 0;
    unsigned int cc_out = 0;
    unsigned int cl_in_time = 0;
    unsigned int cl_out_time = 0;
    int cc_uah_in = 0;
    int cc_uah_out = 0;
    int cc_uah = 0;
    s64 temp = 0;
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CL_IN_BASE,&cc_in,4);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CL_OUT_BASE,&cc_out,4);
#if 1 /*debug*/
    g_cc_in = cc_in;
    g_cc_out = cc_out;
    hisi_hi6421v300_print_cc_reg(0);
#endif
    cc_uah_out = convert_regval2uah(cc_out);
    cc_uah_in = convert_regval2uah(cc_in);

    temp = (s64) c_offset_a *cc_uah_in;
    cc_uah_in = div_s64(temp, 1000000);
    temp = (s64) c_offset_a *cc_uah_out;
    cc_uah_out = div_s64(temp, 1000000);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CHG_TIMER_BASE, &cl_in_time, 4);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_LOAD_TIMER_BASE, &cl_out_time, 4);
    /* uah = uas/3600 = ua*s/3600 */
    cc_uah_in = cc_uah_in - div_s64((s64) cl_in_time * c_offset_b, 3600);
    cc_uah_out =
        cc_uah_out + div_s64((s64) cl_out_time * c_offset_b, 3600);
    cc_uah = cc_uah_out - cc_uah_in;

    return cc_uah;
}

/*
 *    from coul get battery cl_in or cl_out.
 */
static unsigned int get_coul_time(void)
{
    unsigned int cl_in_time = 0;
    unsigned int cl_out_time = 0;

    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CHG_TIMER_BASE,&cl_in_time,4);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_LOAD_TIMER_BASE,&cl_out_time,4);

    return (cl_in_time + cl_out_time);
}

/*
 * clear sleep and wakeup global variable
 */
static void clear_sr_time_array(void)
{
    memset(&sr_time_sleep, 0, sizeof(sr_time_sleep));
    memset(&sr_time_wakeup, 0, sizeof(sr_time_wakeup));
    sr_index_sleep = 0;
    sr_index_wakeup = 0;
}
/*
 *    from coul get battery cl_in or cl_out.
 */
static void clear_coul_time(void)
{
    unsigned int cl_time = 0;
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    unsigned int time_now = get_coul_time();

    if (di)
    {
        if (SR_DEVICE_WAKEUP == sr_cur_state)
        {
            di->sr_resume_time -= time_now;
            if (di->sr_resume_time > 0)
            {
                HISI_HI6421V300_COUL_ERR("[SR]%s(%d): di->sr_resume_time = %d\n", __func__, __LINE__, di->sr_resume_time);
                di->sr_resume_time = 0;
            }
            di->sr_suspend_time = 0;
        }
        else
        {
            di->sr_resume_time = 0;
            di->sr_suspend_time = 0;
            HISI_HI6421V300_COUL_ERR("[SR]%s(%d): sr_cur_state = %d\n", __func__, __LINE__, sr_cur_state);
        }
        di->charging_start_time -= time_now;
        di->charging_stop_time -= time_now;
    }
    else
    {
        HISI_HI6421V300_COUL_ERR("[SR]%s(%d): di is NULL\n", __func__, __LINE__);
    }

    HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_CHG_TIMER_BASE,&cl_time ,4);
    HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_LOAD_TIMER_BASE,&cl_time ,4);
    clear_sr_time_array();
}

/**************************************************************************
 * In case the coul-offset-reg is writen in some conditions,
 * so clear these regs: 0x1cc 0x1cd 0x1ce 0x1cf
 * Clear condition:clear it when found these four regs are not ZERO.
 **************************************************************************/
static void clear_coul_offset_reg(void)
{
    unsigned int regval;
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_OFFSET_REG_BASE,&regval,4);

    if (regval)
    {
        HISI_HI6421V300_COUL_ERR("Coul offset is not ZERO! reg:0x%x\n",regval);

		if (!dsm_client_ocuppy(pmu_coul_dclient)) {
			dsm_client_record(pmu_coul_dclient, "Coul offset is not ZERO! reg:0x%lx\n", regval);
			dsm_client_notify(pmu_coul_dclient, DSM_PMU_COUL_ERROR_NO);
		}

		regval = 0;
        HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_OFFSET_REG_BASE,&regval,4);
        mdelay(150);
    }
}

static void check_coul_close_register(void)
{
    static int dmd_notify_enable = 1;
    unsigned char regval = HISI_HI6421V300_REG_READ(HISI_HI6421V300_COUL_CTRL_REG);

    if ((1 == dmd_notify_enable) && (!(regval & COUL_CTRL_ENABLE)))
    {
        if (!dsm_client_ocuppy(pmu_coul_dclient))
        {
            dsm_client_record(pmu_coul_dclient, "Coul is closed! reg:0x%x\n", regval);
            dsm_client_notify(pmu_coul_dclient, DSM_PMU_COUL_ERROR_NO + DSM_COUL_CLOSE_OFFSET);
            dmd_notify_enable = 0;
        }
    }
}
/**
 * convert_regval2uv
 * 1 bit = 225.1196 uv = 4.8/21322 V = 4.8/21322 * 1000 * 1000 uV = 24 * 1000 * 100/ 10661 uV
 * convert regval to uv
 */
static int convert_regval2uv(short reg_val)
{
    s64 temp;

    if (reg_val & 0x8000) {
        return -1;
    }

    temp = (s64)((s64)(reg_val)  * (s64)(24 * 1000 * 100));

    temp = div_s64(temp, 10661);

    temp = (s64) v_offset_a *temp;
    temp = div_s64(temp, 1000000);
    temp += v_offset_b;

    return (int)temp;
}

/**
 * convert_uv2regval
 * 1 uv = 10661 / 24 * 1000 * 100 bit
 * convert uv to regval
 */
static short convert_uv2regval(int uv_val)
{
    short ret;
    s64 temp;

    temp = uv_val - v_offset_b;
    temp = temp * 1000000;
    uv_val = div_s64(temp, v_offset_a);

    temp = (s64)((s64)uv_val * (s64)10661);
    temp = div_s64(temp,2400000);

    ret = (short)temp;

    return ret;
}

/**
 * hisi_hi6421v300_battery_voltage_uv
 *
 * battery voltage in uv
 */
int hisi_hi6421v300_battery_voltage_uv(void)
{
    short regval = 0;

    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_VOL_FIFO_BASE, &regval, 2);
    return(convert_regval2uv(regval));
}

/**
 * convert_regval2ua
 * 10 mohm resistance: 1 bit = 5/10661 A = 5*1000*1000 / 10661 uA
 * 20 mohm resistance: 1 bit = 10 mohm / 2
 * 30 mohm resistance: 1 bit = 10 mohm / 3
 * ...
 * high bit = 0 is in, 1 is out
 * convert regval to ua
 */
static  int convert_regval2ua(short reg_val)
{
    int ret;
    s64 temp;

    ret = reg_val;
    temp = (s64)(ret) * (s64)(1000 * 1000 * 5);
    temp = div_s64(temp, 10661);

    ret = temp / (R_COUL_MOHM/10);

    ret = -ret;

    temp = (s64) c_offset_a *ret;
    ret = div_s64(temp, 1000000);
    ret += c_offset_b;

    return ret;
}
/**
 * convert_ua2regval
 *
 * 10 mohm resistance: 1 bit = 5/10661 A = 5*1000*1000 / 10661 uA
 * 20 mohm resistance: 1 bit = 10 mohm / 2
 * 30 mohm resistance: 1 bit = 10 mohm / 3
 */
static  short convert_ua2regval(int ua)
{
    s64 temp;
    short regval;

    temp = ((s64)(ua) * (s64)(10661 * (R_COUL_MOHM/10)));
    temp = div_s64(temp,5000000);

    regval = (short)(-temp);

    return regval;
}

/**
 * hisi_hi6421v300_battery_current_ua - return the current of battery.
 *
 * return the battery current in uA.
 */
int hisi_hi6421v300_battery_current_ua(void)
{
    short regval = 0;

    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CUR_FIFO_BASE, &regval, 2);

    return (0 - convert_regval2ua(regval));
}


static int is_between(int left, int right, int value)
{
    if ((left >= right) && (left >= value) && (value >= right))
        return 1;
    if ((left <= right) && (left <= value) && (value <= right))
        return 1;

    return 0;
}

static int linear_interpolate(int y0, int x0, int y1, int x1, int x)
{
    if ((y0 == y1) || (x == x0))
        return y0;
    if ((x1 == x0) || (x == x1))
        return y1;

    return y0 + ((y1 - y0) * (x - x0) / (x1 - x0));
}

static int interpolate_single_lut(struct single_row_lut *lut, int x)
{
    int i, result;

    if (x < lut->x[0]) {
        return lut->y[0];
    }
    if (x > lut->x[lut->cols - 1]) {
        return lut->y[lut->cols - 1];
    }

    for (i = 0; i < lut->cols; i++)
        if (x <= lut->x[i])
            break;
    if (x == lut->x[i]) {
        result = lut->y[i];
    } else {
        result = linear_interpolate(
            lut->y[i - 1],
            lut->x[i - 1],
            lut->y[i],
            lut->x[i],
            x);
    }
    return result;
}

static int interpolate_single_y_lut(struct single_row_lut *lut, int y)
{
    int i, result;

    if (y < lut->y[0]) {
        return lut->x[0];
    }
    if (y > lut->y[lut->cols - 1]) {
        return lut->x[lut->cols - 1];
    }

    for (i = 0; i < lut->cols; i++)
        if (y <= lut->y[i])
            break;
    if (y == lut->y[i]) {
        result = lut->x[i];
    } else {
        result = linear_interpolate(
            lut->x[i - 1],
            lut->y[i - 1],
            lut->x[i],
            lut->y[i],
            y);
    }
    return result;
}

static int interpolate_scalingfactor(struct sf_lut *sf_lut, int row_entry, int pc)
{
    int i, scalefactorrow1, scalefactorrow2, scalefactor;
    int rows, cols;
    int row1 = 0;
    int row2 = 0;

    /*
     * sf table could be null when no battery aging data is available, in
     * that case return 100%
     */
    if (!sf_lut)
        return 100;

    rows = sf_lut->rows;
    cols = sf_lut->cols;
    if (pc > sf_lut->percent[0]) {
        //HISI_HI6421V300_COUL_EVT("pc %d greater than known pc ranges for sfd\n", pc);
        row1 = 0;
        row2 = 0;
    }
    if (pc < sf_lut->percent[rows - 1]) {
        //HISI_HI6421V300_COUL_EVT("pc %d less than known pc ranges for sf\n", pc);
        row1 = rows - 1;
        row2 = rows - 1;
    }
    for (i = 0; i < rows; i++) {
        if (pc == sf_lut->percent[i]) {
            row1 = i;
            row2 = i;
            break;
        }
        if (pc > sf_lut->percent[i]) {
            row1 = i - 1;
            row2 = i;
            break;
        }
    }

    if (row_entry < sf_lut->row_entries[0])
        row_entry = sf_lut->row_entries[0];
    if (row_entry > sf_lut->row_entries[cols - 1])
        row_entry = sf_lut->row_entries[cols - 1];

    for (i = 0; i < cols; i++)
        if (row_entry <= sf_lut->row_entries[i])
            break;
    if (row_entry == sf_lut->row_entries[i]) {
        scalefactor = linear_interpolate(
                sf_lut->sf[row1][i],
                sf_lut->percent[row1],
                sf_lut->sf[row2][i],
                sf_lut->percent[row2],
                pc);
        return scalefactor;
    }

    scalefactorrow1 = linear_interpolate(
                sf_lut->sf[row1][i - 1],
                sf_lut->row_entries[i - 1],
                sf_lut->sf[row1][i],
                sf_lut->row_entries[i],
                row_entry);

    scalefactorrow2 = linear_interpolate(
                sf_lut->sf[row2][i - 1],
                sf_lut->row_entries[i - 1],
                sf_lut->sf[row2][i],
                sf_lut->row_entries[i],
                row_entry);

    scalefactor = linear_interpolate(
                scalefactorrow1,
                sf_lut->percent[row1],
                scalefactorrow2,
                sf_lut->percent[row2],
                pc);

    return scalefactor;
}

static void get_simultaneous_battery_voltage_and_current(struct hisi_hi6421v300_coul_device *di, int *ibat_ua, int *vbat_uv)
{
    int vol[3], cur[3];
    int i;
    for (i=0; i<3; i++){
        vol[i] = hisi_hi6421v300_battery_voltage_uv();
        cur[i] = hisi_hi6421v300_battery_current_ua();
    }

    if (vol[0]==vol[1] && cur[0]==cur[1]){
        *ibat_ua = cur[0];
        *vbat_uv = vol[0];
    } else if (vol[1]==vol[2] && cur[1]==cur[2]){
        *ibat_ua = cur[1];
        *vbat_uv = vol[1];
    } else {
        *ibat_ua = cur[2];
        *vbat_uv = vol[2];
    }

    *vbat_uv += (*ibat_ua/1000)*(di->r_pcb/1000);

}



/**
 * interpolate_fcc
 *
 * look for fcc value by battery temperature
 */
static int interpolate_fcc(struct hisi_hi6421v300_coul_device *di, int batt_temp)
{
    /* batt_temp is in tenths of degC - convert it to degC for lookups */
    batt_temp = batt_temp/10;
    return interpolate_single_lut(di->batt_data->fcc_temp_lut, batt_temp);
}

/**
 * interpolate_scalingfactor_fcc
 *
 * look for fcc scaling factory value by battery charge cycles
 */
static int interpolate_scalingfactor_fcc(struct hisi_hi6421v300_coul_device *di, int cycles)
{
    /*
     * sf table could be null when no battery aging data is available, in
     * that case return 100%
     */
    if (di->batt_data->fcc_sf_lut)
        return interpolate_single_lut(di->batt_data->fcc_sf_lut, cycles);
    else
        return 100;
}

static int interpolate_fcc_adjusted(struct hisi_hi6421v300_coul_device *di, int batt_temp)
{
    /* batt_temp is in tenths of degC - convert it to degC for lookups */
    batt_temp = batt_temp/10;
    return interpolate_single_lut(di->adjusted_fcc_temp_lut, batt_temp);
}

static int calculate_fcc_uah(struct hisi_hi6421v300_coul_device *di, int batt_temp, int chargecycles)
{
    int initfcc, result, scalefactor = 0;

    if (di->adjusted_fcc_temp_lut == NULL) {
        initfcc = interpolate_fcc(di, batt_temp);
        scalefactor = interpolate_scalingfactor_fcc(di, chargecycles);

        /* Multiply the initial FCC value by the scale factor. */
        result = (initfcc * scalefactor * 1000) / 100;

    } else {
        return 1000 * interpolate_fcc_adjusted(di, batt_temp);
    }

    return result;
}

static int interpolate_pc(struct pc_temp_ocv_lut *lut, int batt_temp, int ocv)
{
    int i, j, pcj, pcj_minus_one, pc;
    int rows = lut->rows;
    int cols = lut->cols;

    /* batt_temp is in tenths of degC - convert it to degC for lookups */
    batt_temp = batt_temp/10;

    if (batt_temp < lut->temp[0]) {
    HISI_HI6421V300_COUL_ERR("batt_temp %d < known temp range for pc\n", batt_temp);
    batt_temp = lut->temp[0];
    }
    if (batt_temp > lut->temp[cols - 1]) {
    HISI_HI6421V300_COUL_ERR("batt_temp %d > known temp range for pc\n", batt_temp);
    batt_temp = lut->temp[cols - 1];
    }

    for (j = 0; j < cols; j++)
    if (batt_temp <= lut->temp[j])
        break;
    if (batt_temp == lut->temp[j]) {
    /* found an exact match for temp in the table */
    if (ocv >= lut->ocv[0][j])
        return lut->percent[0]*10;
    if (ocv <= lut->ocv[rows - 1][j])
        return lut->percent[rows - 1]*10;
    for (i = 0; i < rows; i++) {
        if (ocv >= lut->ocv[i][j]) {
            if (ocv == lut->ocv[i][j])
                return lut->percent[i]*10;
            pc = linear_interpolate(
                lut->percent[i]*10,
                lut->ocv[i][j],
                lut->percent[i - 1]*10,
                lut->ocv[i - 1][j],
                ocv);
            return pc;
        }
    }
    }

    /*
     * batt_temp is within temperature for
     * column j-1 and j
     */
    if (ocv >= lut->ocv[0][j])
    return lut->percent[0]*10;
    if (ocv <= lut->ocv[rows - 1][j - 1])
    return lut->percent[rows - 1]*10;

    pcj_minus_one = 0;
    pcj = 0;
    for (i = 0; i < rows-1; i++) {
    if (pcj == 0
        && is_between(lut->ocv[i][j],
            lut->ocv[i+1][j], ocv)) {
        pcj = linear_interpolate(
            lut->percent[i]*10,
            lut->ocv[i][j],
            lut->percent[i + 1]*10,
            lut->ocv[i+1][j],
            ocv);
    }

    if (pcj_minus_one == 0
        && is_between(lut->ocv[i][j-1],
            lut->ocv[i+1][j-1], ocv)) {

        pcj_minus_one = linear_interpolate(
            lut->percent[i]*10,
            lut->ocv[i][j-1],
            lut->percent[i + 1]*10,
            lut->ocv[i+1][j-1],
            ocv);
    }

    if (pcj && pcj_minus_one) {
        pc = linear_interpolate(
            pcj_minus_one,
            lut->temp[j-1],
            pcj,
            lut->temp[j],
            batt_temp);
        return pc;
    }
    }

    if (pcj)
    return pcj;

    if (pcj_minus_one)
    return pcj_minus_one;

    HISI_HI6421V300_COUL_ERR("%d ocv wasn't found for temp %d in the LUT returning 100%%\n",
                                            ocv, batt_temp);
    return 1000;
}


static int calculate_pc(struct hisi_hi6421v300_coul_device *di, int ocv_uv, int batt_temp, int chargecycles)
{
    int pc, scalefactor;

    pc = interpolate_pc(di->batt_data->pc_temp_ocv_lut, batt_temp, ocv_uv / 1000);

    scalefactor = interpolate_scalingfactor(di->batt_data->pc_sf_lut, chargecycles, pc/10);

    /* Multiply the initial FCC value by the scale factor. */
    pc = (pc * scalefactor) / 100;
    return pc;
}

static int calculate_remaining_charge_uah(struct hisi_hi6421v300_coul_device *di, int fcc_uah, int chargecycles)
{
    int  ocv, pc;
    int fcc_mah = fcc_uah/1000;
    int temp;
    temp = di->batt_ocv_temp;

    ocv = di->batt_ocv;
    pc = calculate_pc(di, ocv, temp, chargecycles);

    return fcc_mah * pc;
}


static int get_rbatt(struct hisi_hi6421v300_coul_device *di, int soc_rbatt, int batt_temp)
{
    int rbatt, scalefactor;

    rbatt = di->batt_data->default_rbatt_mohm;
    if (di->batt_data->rbatt_sf_lut == NULL)  {
        return rbatt;
    }
    /* Convert the batt_temp to DegC from deciDegC */
    batt_temp = batt_temp / 10;
    scalefactor = interpolate_scalingfactor(di->batt_data->rbatt_sf_lut,
                            batt_temp, soc_rbatt);
    rbatt = (rbatt * scalefactor) / 100;

    if (is_between(20, 10, soc_rbatt))
        rbatt = rbatt
            + ((20 - soc_rbatt) * di->batt_data->delta_rbatt_mohm) / 10;
    else
        if (is_between(10, 0, soc_rbatt))
            rbatt = rbatt + di->batt_data->delta_rbatt_mohm;

    return rbatt;
}

/* get ocv given a soc  -- reverse lookup */
static int interpolate_ocv(struct pc_temp_ocv_lut *lut, int batt_temp_degc, int pc)
{
    int i, ocvrow1, ocvrow2, ocv;
    int rows, cols;
    int row1 = 0;
    int row2 = 0;
    if (pc > 1000)
    {
        pc = 1000;
    }
    else if (pc < 0)
    {
        pc = 0;
    }

    rows = lut->rows;
    cols = lut->cols;
    if (pc > lut->percent[0]*10)
    {
        //HISI_HI6421V300_COUL_EVT("pc %d greater than known pc ranges for sfd\n", pc);
        row1 = 0;
        row2 = 0;
    }
    if (pc < lut->percent[rows - 1]*10)
    {
        //HISI_HI6421V300_COUL_EVT("pc %d less than known pc ranges for sf\n", pc);
        row1 = rows - 1;
        row2 = rows - 1;
    }
    for (i = 0; i < rows; i++)
    {
        if (pc == lut->percent[i]*10)
        {
            row1 = i;
            row2 = i;
            break;
        }
        if (pc > lut->percent[i]*10)
        {
            row1 = i - 1;
            row2 = i;
            break;
        }
    }

    if (batt_temp_degc < lut->temp[0])
        batt_temp_degc = lut->temp[0];
    if (batt_temp_degc > lut->temp[cols - 1])
        batt_temp_degc = lut->temp[cols - 1];

    for (i = 0; i < cols; i++)
        if (batt_temp_degc <= lut->temp[i])
            break;
    if (batt_temp_degc == lut->temp[i])
    {
        ocv = linear_interpolate(
        lut->ocv[row1][i],
        lut->percent[row1]*10,
        lut->ocv[row2][i],
        lut->percent[row2]*10,
        pc);
        return ocv;
    }

    ocvrow1 = linear_interpolate(
                lut->ocv[row1][i - 1],
                lut->temp[i - 1],
                lut->ocv[row1][i],
                lut->temp[i],
                batt_temp_degc);

    ocvrow2 = linear_interpolate(
                lut->ocv[row2][i - 1],
                lut->temp[i - 1],
                lut->ocv[row2][i],
                lut->temp[i],
                batt_temp_degc);

    ocv = linear_interpolate(
                ocvrow1,
                lut->percent[row1]*10,
                ocvrow2,
                lut->percent[row2]*10,
                pc);

    return ocv;
}


static int calculate_termination_uuc(struct hisi_hi6421v300_coul_device *di, int batt_temp, int chargecycles, int fcc_uah, int i_ma, int *ret_pc_unusable)
{
    int unusable_uv, pc_unusable, uuc;
    int batt_temp_degc = batt_temp / 10;
    int rbatt_mohm = 0;
    int fcc_mah = fcc_uah / 1000;
    int zero_voltage = 3200;

    i_ma = di->last_fifo_iavg_ma;

    if((batt_temp_degc < 5) && (batt_temp_degc > -10))
    {
        zero_voltage =(((5 - batt_temp_degc)*(ZERO_VOLTAGE_MINUS_10 - ZERO_VOLTAGE_PLUS_5)
                                /15)+ZERO_VOLTAGE_PLUS_5);
    }
    else if(batt_temp_degc <= -10)
    {
        zero_voltage = ZERO_VOLTAGE_MINUS_10;
    }
    else
    {
        zero_voltage =  ZERO_VOLTAGE_PLUS_5;
    }
    HISI_HI6421V300_COUL_DEBUG("%s,batt_temp_degc = %d,zero_voltage = %d\n",__func__,batt_temp_degc,zero_voltage);

    rbatt_mohm = get_rbatt(di, (di->batt_soc_real / 10), batt_temp);
    unusable_uv = (rbatt_mohm * i_ma) + (zero_voltage * 1000);

    pc_unusable = calculate_pc(di, unusable_uv, batt_temp, chargecycles);
    uuc =  fcc_mah * pc_unusable;
    *ret_pc_unusable = pc_unusable;
    return uuc;
}

static int adjust_uuc(struct hisi_hi6421v300_coul_device *di, int fcc_uah,
            int new_pc_unusable,
            int new_uuc,
            int batt_temp,
            int rbatt,
            int *iavg_ma)
{
    int fcc_mah = fcc_uah / 1000;

    if (di->prev_pc_unusable == -EINVAL
        || abs(di->prev_pc_unusable - new_pc_unusable) <= 10)
    {
        di->prev_pc_unusable = new_pc_unusable;
        return new_uuc;
    }

    /* the uuc is trying to change more than 2% restrict it */
    if (new_pc_unusable > di->prev_pc_unusable)
        di->prev_pc_unusable += 10;
    else
        di->prev_pc_unusable -= 10;

    new_uuc = fcc_mah * di->prev_pc_unusable;

    return new_uuc;
}

static int calculate_unusable_charge_uah(struct hisi_hi6421v300_coul_device *di,
                int rbatt, int fcc_uah, int cc_uah,
                 int batt_temp, int chargecycles,
                int iavg_ua)
{
    int uuc_uah_iavg;
    int i;
    int iavg_ma = iavg_ua / 1000;
    static int iavg_samples[IAVG_SAMPLES];
    static int iavg_index;
    static int iavg_num_samples;
    int pc_unusable;

    /*
     * if we are charging use a nominal avg current so that we keep
     * a reasonable UUC while charging
     */
    if (iavg_ma < 0)
        iavg_ma = CHARGING_IAVG_MA;
    iavg_samples[iavg_index] = iavg_ma;
    iavg_index = (iavg_index + 1) % IAVG_SAMPLES;
    iavg_num_samples++;
    if (iavg_num_samples >= IAVG_SAMPLES)
        iavg_num_samples = IAVG_SAMPLES;

    /* now that this sample is added calcualte the average */
    iavg_ma = 0;
    if (iavg_num_samples != 0) {
        for (i = 0; i < iavg_num_samples; i++) {
            iavg_ma += iavg_samples[i];
        }

        iavg_ma = DIV_ROUND_CLOSEST(iavg_ma, iavg_num_samples);
    }

    uuc_uah_iavg = calculate_termination_uuc(di,
                    batt_temp, chargecycles,
                    fcc_uah, iavg_ma,
                    &pc_unusable);

    HISI_HI6421V300_COUL_DEBUG("RBATT_ADJ:UUC =%d uAh, pc=%d.%d\n",
        uuc_uah_iavg, pc_unusable/10, pc_unusable%10);

    glog->uuc_pc = pc_unusable;

    /* restrict the uuc such that it can increase only by one percent */
    uuc_uah_iavg = adjust_uuc(di, fcc_uah, pc_unusable, uuc_uah_iavg,
                    batt_temp, rbatt, &iavg_ma);
    uuc_uah_iavg += fcc_uah/1000*10;

    di->batt_uuc = uuc_uah_iavg;

    return uuc_uah_iavg;
}

/**
 * recalc_chargecycles - recalculate the chargecycle
 * @di: stmartstar coul device
 *
 */
static unsigned int recalc_chargecycles(struct hisi_hi6421v300_coul_device *di)
{
    int cc_end, real_fcc, fcc, temp, pc, new_chargecycles;
    unsigned int retval = 0;

    if (di->batt_soc==100 && di->charging_begin_soc/10<MIN_BEGIN_PERCENT_FOR_LEARNING) {
        cc_end = calculate_cc_uah();
        temp = di->batt_temp;
        real_fcc = (cc_end - di->charging_begin_cc)*1000/(1000 - di->charging_begin_soc);
        fcc = interpolate_fcc(di,temp);
        pc = real_fcc *100 / fcc;
        new_chargecycles = interpolate_single_y_lut(di->batt_data->fcc_sf_lut, pc);
        new_chargecycles -= 40*100;
        retval = (unsigned int)(new_chargecycles>0?new_chargecycles:0);

        HISI_HI6421V300_COUL_EVT("trigger battery charge cycle reclac, val = %d!\n", new_chargecycles);
    }

    return retval;
}

static struct hisi_hi6421v300_coul_nv_info my_nv_info;
static unsigned long nv_info_addr = 0;

static int __init early_parse_pmu_nv_addr_cmdline(char *p)
{
    char buf[30] = {0};

    char *endptr = NULL;

    memcpy(buf, p, strlen(p)+1);

    nv_info_addr = (unsigned long)simple_strtoul(buf, &endptr, 16);

    return 0;
}

early_param("pmu_nv_addr", early_parse_pmu_nv_addr_cmdline);
static int __init move_pmu_nv_info(void)
{
    struct hisi_hi6421v300_coul_nv_info *pmu_nv_addr;

    if (nv_info_addr == 0){
        return 0;
    }

    pmu_nv_addr = (struct hisi_hi6421v300_coul_nv_info*)ioremap(nv_info_addr, sizeof (struct hisi_hi6421v300_coul_nv_info));

    HISI_HI6421V300_COUL_EVT("pmu_nv_addr=0x%p\n", pmu_nv_addr);

    memcpy(&my_nv_info, pmu_nv_addr, sizeof(my_nv_info));
    return 0;
}
arch_initcall(move_pmu_nv_info);

static int get_initial_params(struct hisi_hi6421v300_coul_device *di)
{
    int i;
    struct hisi_hi6421v300_coul_nv_info *pinfo = &di->nv_info;
    struct single_row_lut *preal_fcc_lut = &di->adjusted_fcc_temp_lut_tbl1;

    memcpy(&di->nv_info, &my_nv_info, sizeof(my_nv_info));
    di->batt_chargecycles = pinfo->charge_cycles;
    v_offset_a = (0 == pinfo->v_offset_a) ? DEFAULT_V_OFF_A : pinfo->v_offset_a;
    v_offset_b = (0 == pinfo->v_offset_b) ? DEFAULT_V_OFF_B : pinfo->v_offset_b;

    g_coulomb_test_interface_info->v_offset_a = v_offset_a;
    g_coulomb_test_interface_info->v_offset_b = v_offset_b;
    di->batt_id_vol = pinfo->hkadc_batt_id_voltage;
    HISI_HI6421V300_COUL_INF("%s: batt ID is %d\n",__FUNCTION__,di->batt_id_vol);
    HISI_HI6421V300_COUL_INF("v_offset_a = %d , v_offset_b = %d\n", v_offset_a, v_offset_b);
    HISI_HI6421V300_COUL_INF("cycles=%d,reg_c=%d, reg_v=%d, start_cc=%d\n"
        , pinfo->charge_cycles, pinfo->calc_ocv_reg_c, pinfo->calc_ocv_reg_v, pinfo->start_cc);

    for (i=0; i<MAX_TEMPS; i++)
    {
        if (pinfo->real_fcc[i] == 0){
            break;
        }

        if (pinfo->real_fcc[i] < 100)
        {
            HISI_HI6421V300_COUL_INF("real fcc in nv is not currect!\n");
            return 0;
        }

        preal_fcc_lut->x[i] = pinfo->temp[i];
        preal_fcc_lut->y[i] = pinfo->real_fcc[i];
    }

    if (i == 0){
        HISI_HI6421V300_COUL_INF("no real fcc data in nv\n");
        return 0;
    }

    preal_fcc_lut->cols = i;

    di->adjusted_fcc_temp_lut = preal_fcc_lut;

    HISI_HI6421V300_COUL_INF("temp:real_fcc %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d\n"
        ,pinfo->temp[0], pinfo->real_fcc[0]
        ,pinfo->temp[1], pinfo->real_fcc[1]
        ,pinfo->temp[2], pinfo->real_fcc[2]
        ,pinfo->temp[3], pinfo->real_fcc[3]
        ,pinfo->temp[4], pinfo->real_fcc[4]
        ,pinfo->temp[5], pinfo->real_fcc[5]
        ,pinfo->temp[6], pinfo->real_fcc[6]
        );
    return 0;
}

static void set_need_save_nv_flag(struct hisi_hi6421v300_coul_device *di)
{
    char val;

    di->is_nv_need_save = 1;
    HISI_HI6421V300_COUL_INF("set need save nv flag\n");
    //we will do nv save operation in calculate soc work
    //so before nv is really saved, we should set nv save
    //success to invalid
    val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_NV_SAVE_SUCCESS);
    val = val&(~NV_SAVE_SUCCESS);
    HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_NV_SAVE_SUCCESS, val);
}

static int save_nv_info(struct hisi_hi6421v300_coul_device *di)
{
    int ret, i;
    char val;
    int refresh_fcc_success = 1;
    struct hisi_nve_info_user nve;
    struct hisi_hi6421v300_coul_nv_info *pinfo = &di->nv_info;

    if (!di->is_nv_read){
        HISI_HI6421V300_COUL_ERR("save nv before read, end\n");
        val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_NV_READ_SUCCESS);
        val = val&(~(NV_SAVE_SUCCESS|NV_READ_SUCCESS));
        HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_NV_SAVE_SUCCESS, val);
        return -1;
    }

    memset(&nve, 0, sizeof(nve));
    strncpy(nve.nv_name, HISI_HI6421V300_COUL_NV_NAME, sizeof(HISI_HI6421V300_COUL_NV_NAME));
    nve.nv_number = HISI_HI6421V300_COUL_NV_NUM;
    nve.valid_size = sizeof(*pinfo);
    nve.nv_operation = NV_WRITE;

    pinfo->charge_cycles = di->batt_chargecycles;
    pinfo->start_cc = di->cc_start_value;
    pinfo->ocv_temp = di->batt_ocv_temp;
    pinfo->limit_fcc = di->batt_limit_fcc;

    if (di->adjusted_fcc_temp_lut){
        for(i=0; i<di->adjusted_fcc_temp_lut->cols; i++)
        {
            if(di->adjusted_fcc_temp_lut->y[i] < 100)
            {
                refresh_fcc_success = 0;
                break;
            }
        }
        if(refresh_fcc_success){
            for(i=0; i<di->adjusted_fcc_temp_lut->cols; i++){
                pinfo->temp[i] = di->adjusted_fcc_temp_lut->x[i];
                pinfo->real_fcc[i] = di->adjusted_fcc_temp_lut->y[i];
            }
        }
    }
    else{
        for(i=0; i<MAX_TEMPS; i++){
            pinfo->temp[i] = 0;
            pinfo->real_fcc[i] = 0;
        }
    }

    memcpy(nve.nv_data, pinfo, sizeof(*pinfo));

    ret = hisi_nve_direct_access(&nve);
    if (ret) {
        HISI_HI6421V300_COUL_ERR("save nv partion failed, ret=%d\n", ret);
        val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_NV_SAVE_SUCCESS);
        val = val&(~(NV_SAVE_SUCCESS|NV_READ_SUCCESS));
        HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_NV_SAVE_SUCCESS, val);
    }
    else
    {
        val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_NV_SAVE_SUCCESS);
        val = val | NV_SAVE_SUCCESS;
        HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_NV_SAVE_SUCCESS, val);
    }
    return ret;
}

static int update_chargecycles(struct hisi_hi6421v300_coul_device *di)
{
    if (di->batt_soc_real - di->charging_begin_soc >= 10)
    {
        di->batt_chargecycles += (di->batt_soc_real- di->charging_begin_soc) / 10;
        HISI_HI6421V300_COUL_INF("new chargecycle=%d, added=%d\n", di->batt_chargecycles, (di->batt_soc_real - di->charging_begin_soc)/10);
        di->charging_begin_soc = 1000;
        return 1; //return 1 means need to update chargecycle
    }
    else
    {
        HISI_HI6421V300_COUL_INF("chargecycle not updated, soc_begin=%d, soc_current=%d\n", di->charging_begin_soc/10, di->batt_soc_real/10);
        di->charging_begin_soc = 1000;
        return 0;
    }
}

/**
 * is_hisi_hi6421v300_coul_ready -
 *
 * 0:not ready, 1: ready
 */
int is_hisi_hi6421v300_coul_ready(void)
{
    // TODO: do something here ?
    return 1;
}

/**********************************************************
*  Function:    volt_to_temp
*  Discription:    convert adc sampling voltage to battery temperature
*  Parameters:
*               voltage: adc sampling voltage
*  return value:
*               battery temperature
**********************************************************/
#define T_V_ARRAY_LENGTH 31
#define BAT_ID_V_GUANGYU_MIN 583
#define BAT_ID_V_GUANGYU_MAX 640
static int volt_to_temp(int temp_volt,int id_volt)
{
    int T_V_ATL_LG[][2] =
    {
        {-273, 2500}, {-40, 2298}, {-36, 2252}, {-32, 2199}, {-28, 2137},
        {-24, 2068}, {-20, 1991}, {-16, 1906}, {-12, 1815}, {-8, 1718},
        {-4, 1618}, {0, 1514}, {4, 1409}, {8, 1304}, {12, 1201},
        {16, 1101}, {20, 1005}, {24, 914}, {28, 830}, {32, 751},
        {36, 678}, {40, 611}, {44, 551}, {48, 495}, {52, 445},
        {56, 400}, {60, 360}, {64, 324}, {74, 248}, {84, 191},
        {125, 67}, {0, 0},
    };
    int T_V_GUANGYU[][2] =
    {
        {-273, 2500}, {-40, 2289}, {-36, 2242}, {-32, 2188}, {-28, 2125},
        {-24, 2056}, {-20, 1978}, {-16, 1894}, {-12, 1804}, {-8, 1708},
        {-4, 1607}, {0, 1504}, {4, 1401}, {8, 1298}, {12, 1197},
        {16, 1098}, {20, 1004}, {24, 914}, {28, 830}, {32, 751},
        {36, 678}, {40, 612}, {44, 551}, {48, 495}, {52, 445},
        {56, 399}, {60, 359}, {64, 322}, {74, 248}, {84, 191},
        {125, 71}, {0, 0},
    };
    int temprature = 0;
    int i = 0;
    int (* T_V_TABLE)[2];
    if ((id_volt >= BAT_ID_V_GUANGYU_MIN)&&(id_volt <= BAT_ID_V_GUANGYU_MAX))
    {
        T_V_TABLE = T_V_GUANGYU;
    }
    else
    {
        T_V_TABLE = T_V_ATL_LG;
    }

    if(temp_volt >= T_V_TABLE[0][1])
    {
        return T_V_TABLE[0][0];
    }
    else if(temp_volt <= T_V_TABLE[T_V_ARRAY_LENGTH-1][1])
    {
        return T_V_TABLE[T_V_ARRAY_LENGTH-1][0];
    }
    else
    {
        for(i = 0; i < T_V_ARRAY_LENGTH; i++)
        {
            if(temp_volt == T_V_TABLE[i][1])
                return T_V_TABLE[i][0];
            if(temp_volt > T_V_TABLE[i][1])
            {
                break;
            }
        }
        temprature = T_V_TABLE[i-1][0] + (temp_volt - T_V_TABLE[i-1][1])*(T_V_TABLE[i][0] - T_V_TABLE[i-1][0])/(T_V_TABLE[i][1] - T_V_TABLE[i-1][1]);
    }
    return temprature;
}
/**
 * hisi_hi6421v300_temperature
 *
 * return the battery temperature in centigrade.
 */
 #define DEFAULT_TEMP 30
int hisi_hi6421v300_battery_temperature_tenth_degree(void)
{
    int retry_times = 3;
    int cnt = 0;
    int T_volt;
    int temperature;

    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if(udp_charge)
    {
        return DEFAULT_TEMP*10;
    }
    while(cnt++ < retry_times)
    {
        T_volt = hisi_adc_get_value(HISI_ADC_CHANNEL_BAT_TEMP);
        if(T_volt < 0)
        {
            HISI_HI6421V300_COUL_ERR("Bat temp read fail!,retry_cnt = %d\n",cnt);
        }
        else
        {
            temperature = volt_to_temp(T_volt, di->batt_id_vol);
            HISI_HI6421V300_COUL_DEBUG("Bat temp read success!,T_volt = %d temp = %d\n",T_volt,temperature);
            return 10*temperature;
        }
    }
    HISI_HI6421V300_COUL_ERR("Bat temp read retry 3 times,error!\n");
    return DEFAULT_TEMP*10;
}

static void update_battery_temperature(struct hisi_hi6421v300_coul_device *di, int status)
{
    int temp = hisi_hi6421v300_battery_temperature_tenth_degree();
    if (TEMPERATURE_INIT_STATUS == status)
    {
        HISI_HI6421V300_COUL_INF("init temp = %d\n", temp);
        di->batt_temp = temp;
    }
    else
    {
        if (temp - di->batt_temp > TEMPERATURE_CHANGE_LIMIT)
        {
            HISI_HI6421V300_COUL_ERR("temperature change too fast, pre = %d, current = %d\n", di->batt_temp, temp);
            di->batt_temp = di->batt_temp + TEMPERATURE_CHANGE_LIMIT;
        }
        else if (di->batt_temp - temp > TEMPERATURE_CHANGE_LIMIT)
        {
            HISI_HI6421V300_COUL_ERR("temperature change too fast, pre = %d, current = %d\n", di->batt_temp, temp);
            di->batt_temp = di->batt_temp - TEMPERATURE_CHANGE_LIMIT;
        }
        else if(di->batt_temp != temp)
        {
            HISI_HI6421V300_COUL_INF("temperature changed, pre = %d, current = %d\n", di->batt_temp, temp);
            di->batt_temp = temp;
        }
    }
}
/**
 * hisi_hi6421v300_temperature
 *
 * return the battery temperature in centigrade.
 */
int hisi_hi6421v300_battery_temperature(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    if (NULL == di)
    {
        HISI_HI6421V300_COUL_ERR("error, di is NULL, return default temp\n");
        return DEFAULT_TEMP;
    }
    return (di->batt_temp / 10);
}

/**
 * is_hi6421v300_battery_exist -
 *
 * 0:battery isn't exist, 1: exist
 */
int is_hi6421v300_battery_exist(void)
{
    int temp;

    temp = hisi_hi6421v300_battery_temperature();
    if ((temp <= ABNORMAL_BATT_TEMPERATURE_LOW)
        || (temp >= ABNORMAL_BATT_TEMPERATURE_HIGH)) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * is_hisi_hi6421v300_battery_reach_threshold
 */
int is_hisi_hi6421v300_battery_reach_threshold(void)
{

    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    if (udp_charge)
    {
        //do not add log here, this will be called frequently
        return 0;
    }
    if (di->batt_soc > BATTERY_CC_WARNING_LEV)
        return 0;
    else if (di->batt_soc > BATTERY_CC_LOW_LEV)
        return (1<<2); // BQ_FLAG_SOC1
    else
        return (3<<1); // BQ_FLAG_LOCK
}

/**
 * hisi_hi6421v300_battery_voltage
 *
 * battery voltage in milivolts
 */
int hisi_hi6421v300_battery_voltage(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    int ibat_ua = 0, vbat_uv = 0;
    get_simultaneous_battery_voltage_and_current(di, &ibat_ua, &vbat_uv);

    return vbat_uv/1000;
}

/**
 * hisi_hi6421v300_battery_current - return the current of battery.
 *
 * return the battery current in mA.
 */
int hisi_hi6421v300_battery_current(void)
{
    int cur = hisi_hi6421v300_battery_current_ua();
    return (cur / 1000);
}

/**
 * hisi_hi6421v300_battery_brand
 *
 *
 * return the battery brand in string.
 */
char* hisi_hi6421v300_battery_brand(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    return di->batt_data->batt_brand;
}

/**
 * hisi_hi6421v300_battery_id_voltage - get voltage on ID pin by HKADC.
 * @di: stmartstar coul device
 * called in module initalization
 */
#define DEFAULT_BAT_ID_VOL 0
int hisi_hi6421v300_battery_id_voltage(void)
{
    int retry_times = 3;
    int cnt = 0;
    int Id_volt;

    if(udp_charge)
    {
        return DEFAULT_BAT_ID_VOL;
    }
    while(cnt++ < retry_times)
    {
        Id_volt = hisi_adc_get_value(HISI_ADC_CHANNEL_BAT_ID);// return voltage in mv
        if(Id_volt < 0)
        {
            HISI_HI6421V300_COUL_ERR("Bat Id_volt read fail!,retry_cnt = %d\n",cnt);
        }
        else
        {
            HISI_HI6421V300_COUL_DEBUG("Bat Id_volt read success!,Id_volt = %d\n",Id_volt);
            return Id_volt;
        }
    }
    HISI_HI6421V300_COUL_ERR("Bat Id_volt read retry 3 times,error!\n");
    return DEFAULT_BAT_ID_VOL;
}

/**
 * hisi_hi6421v300_battery_current_avg - return the avg_current of battery
 *   in at least 2MIN.
 * return the battery avg_current in mA.
 */
int hisi_hi6421v300_battery_current_avg(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    return di->last_iavg_ma;
}

/**
 * hisi_hi6421v300_battery_unfilter_capacity - return the unfilter capacity of battery.
 *
 * return the battery unfiltered capacity in percent.
 */
int hisi_hi6421v300_battery_unfiltered_capacity(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    return di->batt_soc_real/10;
}

/**
 * hisi_hi6421v300_battery_capacity - return the capacity of battery.
 *
 * return the battery capacity in percent.
 */
int hisi_hi6421v300_battery_capacity(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist())
    {
        HISI_HI6421V300_COUL_ERR("read capacity error, battery not exist\n");
        return 0;
    }

    if (hand_chg_capacity_flag == 1)
        return input_capacity;

    if (udp_charge && (di->batt_soc <= 2))
    {
        return 3;
    }
    return di->batt_soc;
}


/**
 * hisi_hi6421v300_battery_rm
 *
 * return the remaining capacity in mAh.
 */
int hisi_hi6421v300_battery_rm(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    return di->batt_ruc/1000;
}

/**
 * hisi_hi6421v300_battery_fcc
 *
 * return battery FullChargeCapacity,the reture value is mAh Or < 0 if something fails.
 */
int hisi_hi6421v300_battery_fcc (void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    return di->batt_fcc/1000;
}

int hisi_hi6421v300_battery_uuc (void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    return di->batt_uuc/1000;
}

int hisi_hi6421v300_battery_cc (void)
{
    int cc;
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    cc = calculate_cc_uah();
    cc -= di->cc_start_value;

    return cc/1000;
}

int hisi_hi6421v300_battery_delta_rc (void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    return di->batt_delta_rc/1000;
}

int hisi_hi6421v300_battery_ocv (void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    return di->batt_ocv;
}

int hisi_hi6421v300_battery_resistance (void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    return di->rbatt;
}

/**
 * hisi_hi6421v300_battery_tte
 *
 * return time to empty, the reture value is min, or -1 in charging;
 */
int hisi_hi6421v300_battery_tte (void)
{
    int cc, cur;
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    cur = hisi_hi6421v300_battery_current_ua();

    if (cur < 0){
        return -1; /* charging now */
    }

    cc = di->batt_ruc - di->batt_uuc;

    if (cc < 0) {
        return -1;
    }

    return cc * 60 / cur;
}

/**
 * hisi_hi6421v300_battery_ttf
 *
 * return time to full, the reture value is min, or -1 in discharging.
 */
int hisi_hi6421v300_battery_ttf (void)
{
    int cc, cur;
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    cur = hisi_hi6421v300_battery_current_ua();

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    if (cur >= 0){
        return -1; /* discharging now */
    }

    cc = di->batt_fcc - di->batt_ruc;

    return cc * 60 / (-cur); /* cur is < 0 */
}

/**
 * hisi_hi6421v300_battery_health
 *
 * return 0->"Unknown", 1->"Good", 2->"Overheat", 3->"Dead", 4->"Over voltage",
 *                5->"Unspecified failure", 6->"Cold",
 */
int hisi_hi6421v300_battery_health (void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    int status = POWER_SUPPLY_HEALTH_GOOD;
    int temp = di->batt_temp/10;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    if (temp < TEMP_TOO_COLD)
        status = POWER_SUPPLY_HEALTH_COLD;
    else if (temp > TEMP_TOO_HOT)
        status = POWER_SUPPLY_HEALTH_OVERHEAT;

    return status;
}

/**
 * hisi_hi6421v300_battery_capacity_level
 *
 * return battery FullChargeCapacity,the reture value is mAh Or < 0 if something fails.
 */
int hisi_hi6421v300_battery_capacity_level (void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    int data_capacity = di->batt_soc;
    int status;

    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    if(data_capacity > 100 || data_capacity < 0)
        return POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN;
    else if ((data_capacity >= 0) && (data_capacity <= 5))
        status = POWER_SUPPLY_CAPACITY_LEVEL_CRITICAL;
    else if ((data_capacity > 5) && (data_capacity <= 15))
        status = POWER_SUPPLY_CAPACITY_LEVEL_LOW;
    else if ((data_capacity >= 95) && (data_capacity < 100))
        status = POWER_SUPPLY_CAPACITY_LEVEL_HIGH;
    else if (100 == data_capacity)
        status = POWER_SUPPLY_CAPACITY_LEVEL_FULL;
    else
        status = POWER_SUPPLY_CAPACITY_LEVEL_NORMAL;

    return status;
}

/**
 * hisi_hi6421v300_battery_technology
 */
int hisi_hi6421v300_battery_technology (void)
{
    /*Default technology is "Li-poly"*/
    return POWER_SUPPLY_TECHNOLOGY_LIPO;
}

int hisi_hi6421v300_battery_vbat_max(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    return di->batt_data->vbatt_max;
}

int hisi_hi6421v300_battery_cycle_count(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    return di->batt_chargecycles / 100;
}

int hisi_hi6421v300_battery_fcc_design(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    return di->batt_data->fcc;
}

struct chrg_para_lut *hisi_hi6421v300_battery_charge_params(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    return di->batt_data->chrg_para;
}

int is_hisi_hi6421v300_fcc_debounce(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    int ret = 0;
    int batt_fcc = 0;
    int fcc = 0;

    if (!di) {
        HISI_HI6421V300_COUL_ERR("%s, di is NULL\n", __func__);
        return ret;
    }

    if (!is_hi6421v300_battery_exist()) {
        return ret;
    }

    batt_fcc = hisi_hi6421v300_battery_fcc();
    fcc = interpolate_fcc(di, di->batt_temp);
    if (batt_fcc < (fcc * 85 / 100)  || batt_fcc > (fcc * 115 / 100)) {
        ret = 1;
        HISI_HI6421V300_COUL_ERR("%s, fcc_from_temp=%d, batt_fcc=%d, ret=%d\n", __func__, fcc, batt_fcc, ret);
    }

    return ret;
}

void save_ocv(int ocv)
{
    short ocvreg = convert_uv2regval(ocv);
    HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_SAVE_OCV_ADDR,&ocvreg,2);
    HISI_HI6421V300_COUL_ERR("save ocv, ocv=%d,reg=%d", ocv, ocvreg);
}

/**
 * get_ocv_by_fcc - interpolate ocv value by full charge capacity.
 * @di: stmartstar coul device
 * @batt_temp: temperature
 * called when receive charging complete event
 */
static void get_ocv_by_fcc(struct hisi_hi6421v300_coul_device *di,int batt_temp)
{
    unsigned int new_ocv;
    int batt_temp_degc = batt_temp/10;

    /*looking for ocv value in the OCV-FCC table*/
    new_ocv = interpolate_ocv(di->batt_data->pc_temp_ocv_lut, batt_temp_degc, 1000);

    new_ocv *=1000;

    if ((new_ocv - di->batt_ocv) > 0) {
        DBG_CNT_INC(dbg_ocv_cng_1);
        HISI_HI6421V300_COUL_EVT("full charged, and OCV change, "
                            "new_ocv = %d, old_ocv = %d \n",new_ocv,di->batt_ocv);
        di->batt_ocv = new_ocv;
        di->batt_ocv_temp = di->batt_temp;
        di->batt_ocv_valid_to_refresh_fcc = 1;
        save_ocv(new_ocv);
        /*clear cc register*/
        clear_cc_register();
        di->batt_pre_delta_rc = 0;
        di->uuc_err = 0;
        clear_coul_time();
        di->get_cc_start_time = 0;
        if (di->cc_start_value != 0){
            di->cc_start_value = 0;
            set_need_save_nv_flag(di);
        }

    } else {
        DBG_CNT_INC(dbg_ocv_fc_failed);
        HISI_HI6421V300_COUL_ERR("full charged, but OCV don't change,\
                            new_ocv = %d, old_ocv = %d \n",new_ocv,di->batt_ocv);
    }
}

#define CURRENT_LIMIT (25*1000)
#define DEFAULT_BATTERY_OHMIC_RESISTANCE 100 /* mohm */
/**
 * get_ocv_by_vol - update the ocv from the value of vol FIFO.
 * @di: stmartstar coul device
 *
 * should be called when AP exit from deep sleep
 */
static void get_ocv_by_vol(struct hisi_hi6421v300_coul_device *di)
{
    int i, used, current_ua, voltage_uv;
    short regval = 0;
    int totalvol, totalcur;

    totalcur = 0;
    totalvol = 0;
    used = 0;

    for (i = 0; i < 20; i++)
    {
        HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CUR_FIFO_BASE + i * 2, &regval, 2);
        current_ua = convert_regval2ua(regval);
        if (current_ua >= CURRENT_LIMIT
            || current_ua < CHARGING_CURRENT_OFFSET)
        {
            DBG_CNT_INC(dbg_invalid_vol);
            HISI_HI6421V300_COUL_INF("invalid current = %d ua\n", current_ua);
            continue;
        }
        DBG_CNT_INC(dbg_valid_vol);
        HISI_HI6421V300_COUL_INF("valid current = %d ua\n", current_ua);
        HISI_HI6421V300_REGS_READ(HISI_HI6421V300_VOL_FIFO_BASE + i * 2, &regval, 2);
        totalvol += convert_regval2uv(regval);
        totalcur += current_ua;
        used++;
    }
    HISI_HI6421V300_COUL_INF("used = %d\n", used);
    if (used > 0)
    {
        voltage_uv = totalvol / used;
        current_ua = totalcur / used;
        voltage_uv += current_ua/1000*(di->r_pcb/1000 + DEFAULT_BATTERY_OHMIC_RESISTANCE);

      if( voltage_uv<3200000
            || (voltage_uv>3670000 && voltage_uv<3690000)
            || (voltage_uv>3730000 && voltage_uv<3800000))
        {
            HISI_HI6421V300_COUL_EVT("do not update OCV(%d)\n",voltage_uv);
            return;
        }

        HISI_HI6421V300_COUL_EVT("awake from deep sleep, old OCV = %d \n",di->batt_ocv);
        di->batt_ocv = voltage_uv;
        di->batt_ocv_temp = di->batt_temp;
        di->batt_ocv_valid_to_refresh_fcc = 1;
        clear_cc_register();
        di->batt_pre_delta_rc = 0;
        di->uuc_err = 0;
        clear_coul_time();
        save_ocv(voltage_uv);
        di->get_cc_start_time = 0;
        if (di->cc_start_value != 0)
        {
            di->cc_start_value = 0;
            set_need_save_nv_flag(di);
        }
        HISI_HI6421V300_COUL_EVT("awake from deep sleep, new OCV = %d \n",di->batt_ocv);
        DBG_CNT_INC(dbg_ocv_cng_0);
    }
}

/**
 * get_initial_ocv - get first ocv from register, hardware record it during system reset.
 */
static void get_initial_ocv(struct hisi_hi6421v300_coul_device *di)
{
    unsigned short ocvreg = 0;
    unsigned char val;
    int ocv;

    struct hisi_hi6421v300_coul_nv_info *pinfo = &di->nv_info;

    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_SAVE_OCV_ADDR,&ocvreg,2);

    HISI_HI6421V300_COUL_INF("ocvreg = 0x%4x\n", ocvreg);
    di->batt_ocv_valid_to_refresh_fcc = 1;

    //when the highest bit of ocv reg is 1, it means the ocv is not accurate enough to update fcc
    if (ocvreg & INVALID_TO_UPDATE_FCC)
    {
        ocvreg = ocvreg & (~INVALID_TO_UPDATE_FCC);
        di->batt_ocv_valid_to_refresh_fcc = 0;
    }
    ocv = convert_regval2uv(ocvreg);
    //when we use saved ocv, we do not need to save nv.
    //when we use pmu ocv, we should save nv to upadte ocv_temp and start_cc.
    val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_OCV_CHOOSE_FLAG);
    if (val & USE_SAVED_OCV_FLAG)
    {
        //when use saved ocv, we do not need to save nv at all;
        di->is_nv_need_save = 0;
        HISI_HI6421V300_COUL_INF("use saved ocv\n");
    }
    else
    {
        //when use pmu ocv, we should save nv to update ocv_temp and start_cc.
        di->is_nv_need_save = 1;
        HISI_HI6421V300_COUL_INF("use pmu ocv\n");
    }

    di->cc_start_value = pinfo->start_cc;
    di->batt_ocv_temp = pinfo->ocv_temp;
    di->batt_limit_fcc = pinfo->limit_fcc;
    di->batt_ocv = ocv;

    HISI_HI6421V300_COUL_INF("initial ocv = %d uv, start_cc = %d uAh\n", ocv, di->cc_start_value);
}

#define INT_OFFSET 10 /* mv */
#define LOW_INT_STATE_RUNNING 1
#define LOW_INT_STATE_SLEEP 0
#define BATTERY_VOL_2_PERCENT 3350
static void set_low_vol_int_reg(struct hisi_hi6421v300_coul_device *di, int state)
{
    int vol;
    short regval;

    di->v_low_int_value = state;

    if (state == LOW_INT_STATE_RUNNING){
        vol = di->v_cutoff;
    }
    else{
        vol = BATTERY_VOL_2_PERCENT;
    }

    g_coulomb_test_interface_info->low_int = vol;
    regval = convert_uv2regval(vol*1000);

    HISI_HI6421V300_COUL_INF("set low power vol is %d mv, reg=%d, and soc = %d\n",vol, regval, di->batt_soc);

    HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_VOL_INT_BASE, &regval, 2);
}

/**
 * is_hisi_hi6421v300_battery_moved - check whether user have changed a battery
 *
 * 0:not moved, 1: moved
 */
int is_hisi_hi6421v300_battery_moved(void)
{
    unsigned char val;
    val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_BATTERY_MOVE_ADDR);

    if (val == BATTERY_MOVE_MAGIC_NUM)
    {
        return 0;
    }
    else
    {
        val = BATTERY_MOVE_MAGIC_NUM|(val&0x01);// we should not change bit 0
        HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_BATTERY_MOVE_ADDR, val);
        return 1;
    }

}

static int bound_soc(int soc)
{
    soc = max(0, soc);
    soc = min(100, soc);
    return soc;
}

struct vcdata{
int avg_v; //mv
int avg_c; //ma
int min_c; //ma
int max_c; //ma
};

#define FIFO_DEPTH 20
static void get_fifo_data(struct vcdata *vc)
{
    int i;
    static short vol_fifo[FIFO_DEPTH];
    static short cur_fifo[FIFO_DEPTH];
    int vol,cur;
    int max_cur, min_cur;
    int vols, curs;

    for (i=0; i<FIFO_DEPTH; i++) {
        HISI_HI6421V300_REGS_READ(HISI_HI6421V300_VOL_FIFO_BASE+i*2, &vol_fifo[i], 2);
        HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CUR_FIFO_BASE+i*2, &cur_fifo[i], 2);
    }

    vol=convert_regval2uv(vol_fifo[0])/1000;
    cur= 0 - convert_regval2ua(cur_fifo[0])/1000;

    vols=vol;
    curs=cur;

    max_cur = min_cur = cur;

    for (i=1; i<FIFO_DEPTH; i++){
        vol = convert_regval2uv(vol_fifo[i])/1000;
        cur = 0 - convert_regval2ua(cur_fifo[i])/1000;

        vols += vol;
        curs += cur;

        if (cur > max_cur){
            max_cur = cur;
        }
        else if (cur < min_cur){
            min_cur = cur;
        }
    }

    vol = vols/FIFO_DEPTH;
    cur = curs/FIFO_DEPTH;

    vc->avg_v = vol;
    vc->avg_c = cur;
    vc->max_c = max_cur;
    vc->min_c = min_cur;
}

static int calculate_delta_rc(struct hisi_hi6421v300_coul_device *di, int soc,
        int batt_temp, int rbatt_tbl, int fcc_uah)
{
    int ibat_ua = 0, vbat_uv = 0;
    int pc_new = 0, delta_pc = 0, pc_new_100 = 0, delta_pc_100 = 0;
    int rc_new_uah = 0, delta_rc_uah = 0, delta_rc_uah_100 = 0, delta_rc_final = 0;
    int soc_new = -EINVAL;
    int ocv = 0, delta_ocv = 0, delta_ocv_100 = 0, ocv_new = 0;
    int rbatt_calc = 0, delta_rbatt = 0;
    int batt_temp_degc = batt_temp/10;
    int ratio = 0;
    int ocv_for_uuc_err = 0;
    int rbatt_calc_for_uuc_err = 0;
    int ratio_for_uuc_err = 0;
    struct vcdata vc = {0};
    static int first_in = 1;

    if (first_in)
    {
        di->batt_soc = di->batt_soc_real / 10;//first in, batt_soc is 0 ,so use batt_soc_real
        first_in = 0;
    }

    get_simultaneous_battery_voltage_and_current(di, &ibat_ua, &vbat_uv);

    get_fifo_data(&vc);

    vc.avg_v += (di->r_pcb/1000)*(vc.avg_c)/1000;
    di->last_fifo_iavg_ma = vc.avg_c;

    if (vc.avg_c < 10) {
    goto out;
    }

    if (di->batt_soc_real > 10 * di->batt_soc)
    {
        ocv = interpolate_ocv(di->batt_data->pc_temp_ocv_lut, batt_temp_degc, di->batt_soc_real);
    }
    else
    {
        ocv = interpolate_ocv(di->batt_data->pc_temp_ocv_lut, batt_temp_degc, 10 * di->batt_soc);
    }
    ocv_for_uuc_err = interpolate_ocv(di->batt_data->pc_temp_ocv_lut, batt_temp_degc, 10 * di->batt_soc);

    rbatt_calc = (ocv - vc.avg_v)*1000/vc.avg_c;
    rbatt_calc_for_uuc_err = (ocv_for_uuc_err - vc.avg_v) * 1000 / vc.avg_c;

    ratio = rbatt_calc*100/rbatt_tbl;
    ratio_for_uuc_err = rbatt_calc_for_uuc_err * 100 /rbatt_tbl;

    di->rbatt_ratio = ratio_for_uuc_err;

    delta_rbatt = rbatt_calc - rbatt_tbl;

    delta_ocv = delta_rbatt*vc.avg_c/1000;

    ocv_new = ocv - delta_ocv;

    pc_new = interpolate_pc(di->batt_data->pc_temp_ocv_lut, batt_temp, ocv_new);

    rc_new_uah = di->batt_fcc/1000 * pc_new;

    delta_pc = pc_new - (10 * di->batt_soc);

    delta_rc_uah = di->batt_fcc/1000 * delta_pc;

    if (ratio <= 0)
    {
        delta_ocv_100 = -rbatt_tbl*vc.avg_c/1000;
        pc_new_100 = interpolate_pc(di->batt_data->pc_temp_ocv_lut, batt_temp, ocv-delta_ocv_100);
        delta_pc_100 = pc_new_100 - (10 * di->batt_soc);
        delta_rc_uah_100 = di->batt_fcc/1000 * delta_pc_100;

        delta_rc_final = delta_rc_uah - delta_rc_uah_100;
    }

    soc_new = (rc_new_uah)*100 / (fcc_uah);

    soc_new = bound_soc(soc_new);

out:
    HISI_HI6421V300_COUL_DEBUG("RBATT_ADJ: soc_new=%d rbat_calc=%d rbat_btl=%d ratio=%d "
                       "soc=%d.%d, ocv=%d "
                       "cmin=%d cmax=%d cavg=%d vavg=%d "
                       "c=%d u=%d last_ocv=%d ocv_temp=%d "
                       "delta_ocv=%d delta_pc=%d.%ld delta_rc_uah=%d "
                       "delta_ocv_100=%d delta_pc_100=%d.%ld delta_rc_uah_100=%d "
                       "delta_rc_final=%d \n",
                       soc_new, rbatt_calc, rbatt_tbl, ratio,
                       di->batt_soc_real/10, di->batt_soc_real%10, ocv,
                       vc.min_c, vc.max_c, vc.avg_c, vc.avg_v,
                       ibat_ua, vbat_uv, di->batt_ocv, di->batt_ocv_temp,
                       delta_ocv, delta_pc/10, abs(delta_pc%10), delta_rc_uah,
                       delta_ocv_100, delta_pc_100/10, abs(delta_pc_100%10), delta_rc_uah_100,
                       delta_rc_final
                       );

    glog->soc_est = soc_new;
    glog->rbat_calc = rbatt_calc;
    glog->ocv_for_delrc = ocv;
    glog->vavg = vc.avg_v;
    glog->cavg = vc.avg_c;
    glog->delta_ocv = delta_ocv;
    glog->delta_rc_mid = delta_rc_uah;
    glog->delta_rc_100 = delta_rc_uah_100;

    di->batt_soc_est = soc_new;
    return delta_rc_final;

}

static int adjust_soc(struct hisi_hi6421v300_coul_device *di, int soc,
        int batt_temp, int chargecycles,
        int rbatt, int fcc_uah, int uuc_uah, int cc_uah)
{
    int ibat_ua = 0, vbat_uv = 0;
    int delta_soc = 0, n = 0;
    int soc_new = soc;
    int soc_est_avg = 0;
    short ocvreg = 0;
    static int soc_ests[3] = {100,100,100};
    static int i = 0;

    get_simultaneous_battery_voltage_and_current(di,
                                                &ibat_ua, &vbat_uv);

    if (ibat_ua < -CHARGING_CURRENT_OFFSET) {
        goto out;
    }

    if (di->batt_soc_est<0){
        goto out;
    }

    soc_ests[i%3] = di->batt_soc_est;
    i++;

    soc_est_avg = DIV_ROUND_CLOSEST(soc_ests[0]+soc_ests[1]+soc_ests[2], 3);

    if (soc_est_avg>2){
        goto out;
    }

    delta_soc = soc - soc_est_avg;

    if (delta_soc <= 0){
        goto out;
    }

    n = 3 - soc_est_avg;

    soc_new = soc - delta_soc*n/3;
    HISI_HI6421V300_COUL_EVT("delta_soc=%d, mark save ocv is invalid\n", delta_soc);
    HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_SAVE_OCV_ADDR,&ocvreg,2);

out:
    HISI_HI6421V300_COUL_DEBUG("soc_est_avg=%d delta_soc=%d n=%d\n",
                       soc_est_avg, delta_soc, n);
    soc_new = bound_soc(soc_new);
    return soc_new;
}

static int limit_soc(struct hisi_hi6421v300_coul_device *di,int input_soc)
{
    int output_soc = input_soc;
    static int power_on_cnt = 0;

    int last_soc = di->batt_soc;
    int current_ua, voltage_uv;
    get_simultaneous_battery_voltage_and_current(di, &current_ua, &voltage_uv);

    if (di->soc_limit_flag == 1){
        if(current_ua >= CHARGING_CURRENT_OFFSET) {
        if(last_soc - input_soc >= 1)
            output_soc = last_soc - 1;
        else
            output_soc = last_soc;
        }
        else {
        if(input_soc - last_soc >= 1)
            output_soc = last_soc + 1;
        else
            output_soc = last_soc;
        }
    }
    else if (di->soc_limit_flag == 2){
        if(current_ua >= CHARGING_CURRENT_OFFSET) {
        if(last_soc < input_soc)
            output_soc = last_soc;
        }
        else {
        if(last_soc > input_soc)
            output_soc = last_soc;
        }
    }

    if (di->charging_state == CHARGING_STATE_CHARGE_DONE){
        HISI_HI6421V300_COUL_INF("pre_chargedone output_soc = %d\n", output_soc);
        output_soc = 100;
    }
    if (di->charging_state == CHARGING_STATE_CHARGE_START &&
        voltage_uv/1000>BATTERY_SWITCH_ON_VOLTAGE &&
        output_soc==0 &&
        (current_ua<-CHARGING_CURRENT_OFFSET || power_on_cnt < 3))
    {
        output_soc = 1;
    }

    power_on_cnt ++;
    return output_soc;
}

static void calculate_iavg_ma(struct hisi_hi6421v300_coul_device *di, int iavg_ua)
{
    int iavg_ma = iavg_ua / 1000;
    int i;
    static int iavg_samples[IAVG_SAMPLES];
    static int iavg_index;
    static int iavg_num_samples;

    iavg_samples[iavg_index] = iavg_ma;
    iavg_index = (iavg_index + 1) % IAVG_SAMPLES;
    iavg_num_samples++;
    if (iavg_num_samples >= IAVG_SAMPLES)
        iavg_num_samples = IAVG_SAMPLES;

    iavg_ma = 0;
    for (i = 0; i < iavg_num_samples; i++) {
        iavg_ma += iavg_samples[i];
    }

    iavg_ma = DIV_ROUND_CLOSEST(iavg_ma, iavg_num_samples);

    if (iavg_num_samples > IAVG_TIME_2MIN)
        di->last_iavg_ma = -iavg_ma;
}

static int adjust_delta_rc(struct hisi_hi6421v300_coul_device *di, int delt_rc, int fcc_uah)
{
    int max_changeable_delta_rc = fcc_uah * MAX_DELTA_RC_PC /100;

    if (abs(di->batt_pre_delta_rc - delt_rc) <= max_changeable_delta_rc)
    {
        di->batt_pre_delta_rc = delt_rc;
        return delt_rc;
    }
    HISI_HI6421V300_COUL_INF("delta_rc change exceed 5 percents, pre = %d, current = %d\n", di->batt_pre_delta_rc, delt_rc);
    if (di->batt_pre_delta_rc > delt_rc)
    {
        di->batt_pre_delta_rc -= max_changeable_delta_rc;
    }
    else
    {
        di->batt_pre_delta_rc += max_changeable_delta_rc;
    }
    return di->batt_pre_delta_rc;
}
static void calculate_soc_params(struct hisi_hi6421v300_coul_device *di,
                        int *fcc_uah,
                        int *unusable_charge_uah,
                        int *remaining_charge_uah,
                        int *cc_uah,
                        int *delta_rc_uah,
                        int *rbatt)
{
    int soc_rbatt, iavg_ua;
    int batt_temp = di->batt_temp;
    int chargecycles = di->batt_chargecycles/100;
    int delt_rc;
    static int first_in = 1;

    *fcc_uah = calculate_fcc_uah(di, batt_temp, chargecycles);

    di->batt_fcc = *fcc_uah;

    /* calculate remainging charge */
    *remaining_charge_uah = calculate_remaining_charge_uah(di,
                *fcc_uah, chargecycles);

    di->batt_rm = *remaining_charge_uah;

    /* calculate cc micro_volt_hour */
    di->cc_end_value = calculate_cc_uah();
    *cc_uah = di->cc_end_value - di->cc_start_value;

    di->batt_ruc = *remaining_charge_uah - *cc_uah;

    di->get_cc_end_time = get_coul_time();

    di->batt_soc_real = DIV_ROUND_CLOSEST((*remaining_charge_uah - *cc_uah), *fcc_uah/1000);

    HISI_HI6421V300_COUL_DEBUG("SOC real = %d\n", di->batt_soc_real);

    soc_rbatt = di->batt_soc_real/10;
    if (soc_rbatt < 0)
    soc_rbatt = 0;

    *rbatt = get_rbatt(di, soc_rbatt, batt_temp);

#if RBATT_ADJ
    delt_rc = calculate_delta_rc(di, di->batt_soc_real, di->batt_temp, *rbatt, *fcc_uah);
    *delta_rc_uah = adjust_delta_rc(di, delt_rc, di->batt_fcc);
    di->batt_delta_rc = *delta_rc_uah;
#endif

    if (first_in){
        di->last_cc = di->cc_end_value;
        di->last_time = di->get_cc_end_time;
        iavg_ua = hisi_hi6421v300_battery_current_ua();
        first_in = 0;
    }
    else{
        int delta_cc = di->cc_end_value - di->last_cc;
        int delta_time = di->get_cc_end_time - di->last_time;
        di->last_cc = di->cc_end_value;
        di->last_time = di->get_cc_end_time;

        if(delta_time > 0)
            iavg_ua = div_s64((s64)delta_cc * 3600, delta_time);
        else
            iavg_ua = hisi_hi6421v300_battery_current_ua();

        HISI_HI6421V300_COUL_DEBUG("delta_time=%ds, iavg_ua=%d\n", delta_time, iavg_ua);
    }
    glog->iavg_ua = iavg_ua;

    calculate_iavg_ma(di,iavg_ua);

    *unusable_charge_uah = calculate_unusable_charge_uah(di, *rbatt,
                *fcc_uah, *cc_uah,
                batt_temp, chargecycles, iavg_ua);
}

/*******************************************************
  Function:        get_rm
  Description:     get remain capacity
  Input:            struct hisi_hi6421v300_coul_device *di
  Output:          NULL
  Return:          NULL
********************************************************/
static void get_rm(struct hisi_hi6421v300_coul_device *di, int *rm)
{
    int fcc_uah = 0, unusable_charge_uah = 0, delta_rc_uah = 0;
    int remaining_charge_uah = 0;
    int cc_uah = 0;
    int rbatt = 0;

    if( NULL == di )
    {
        hwlog_err("NULL point in [%s]\n", __func__);
        return;
    }

    calculate_soc_params(di, &fcc_uah, &unusable_charge_uah, &remaining_charge_uah, &cc_uah, &delta_rc_uah, &rbatt);
    *rm = remaining_charge_uah - cc_uah;
}
static int calculate_state_of_charge(struct hisi_hi6421v300_coul_device *di)
{
    int remaining_usable_charge_uah, fcc_uah, unusable_charge_uah, delta_rc_uah;
    int remaining_charge_uah, soc;
    int cc_uah;
    int rbatt;

    if (!di->batt_exist){
        return 0;
    }

    calculate_soc_params(di,
                                    &fcc_uah,
                                    &unusable_charge_uah,
                                    &remaining_charge_uah,
                                    &cc_uah,
                                    &delta_rc_uah,
                                    &rbatt);

    HISI_HI6421V300_COUL_DEBUG("FCC=%duAh, UUC=%duAh, RC=%duAh, CC=%duAh, delta_RC=%duAh, Rbatt=%dmOhm\n",
                       fcc_uah, unusable_charge_uah, remaining_charge_uah, cc_uah, delta_rc_uah, rbatt);

    di->rbatt = rbatt;

    if (di->batt_limit_fcc && di->batt_limit_fcc < fcc_uah)
    {
        fcc_uah = di->batt_limit_fcc;
        hwlog_info("use limit_FCC! %duAh\n", fcc_uah);
    }
    soc = DIV_ROUND_CLOSEST((remaining_charge_uah - cc_uah), fcc_uah/100);
    HISI_HI6421V300_COUL_DEBUG("SOC without UUC = %d\n", soc);
    di->batt_soc_with_uuc = soc;

    /* calculate remaining usable charge */
    remaining_usable_charge_uah = remaining_charge_uah - cc_uah - unusable_charge_uah - di->uuc_err + delta_rc_uah;

    if (fcc_uah - unusable_charge_uah <= 0) {
    soc = 0;
    } else {
    soc = DIV_ROUND_CLOSEST((remaining_usable_charge_uah),
                (fcc_uah - unusable_charge_uah)/100);
    }
    if (soc > 100)
    soc = 100;

    HISI_HI6421V300_COUL_DEBUG("SOC before adjust = %d\n", soc);
    soc= adjust_soc(di, soc, di->batt_temp, di->batt_chargecycles/100, rbatt, fcc_uah, unusable_charge_uah, cc_uah);

    HISI_HI6421V300_COUL_DEBUG("SOC before limit = %d\n", soc);
    glog->soc_before_limit = soc;

    soc = limit_soc(di, soc);
    HISI_HI6421V300_COUL_DEBUG("SOC_NEW = %d\n", soc);

    di->batt_soc = soc;

    return soc;
}

void cali_adc(void)
{
    unsigned char reg_val = 0;
    reg_val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_COUL_STATE_REG);
    if (COUL_CALI_ING == reg_val)
    {
        HISI_HI6421V300_COUL_EVT("cali ing, don't do it again!\n");
        return;
    }
    HISI_HI6421V300_COUL_INF("calibrate!\n");
    reg_val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_COUL_CTRL_REG);
    /* Mode */
    reg_val = reg_val | COUL_CALI_ENABLE;
    HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_COUL_CTRL_REG, reg_val);
}

void print_offset(void)
{
    unsigned short volreg_offset = 0;
    unsigned short curreg_offset = 0;
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_OFFSET_VOLTAGE, &volreg_offset, 2);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_OFFSET_CURRENT, &curreg_offset, 2);
    HISI_HI6421V300_COUL_INF("calibrated, volreg_offset=%d, curreg_offset=%d\n",
                       volreg_offset, curreg_offset);
}

static void update_uuc_err(struct hisi_hi6421v300_coul_device * di)
{
    int uuc_err = di->uuc_err;

    if (di->last_fifo_iavg_ma < 10)
    {
        uuc_err -= di->batt_fcc / 1000;
    }
    else if (di->rbatt_ratio < 0)
    {
        uuc_err -= di->batt_fcc / 1000;
    }
    else if (di->rbatt_ratio > 100)
    {
        if (di->batt_soc_real <= 50) //uuc_err is only used when soc_real is below 5%
        {
            uuc_err += di->batt_fcc / 1000;
        }
    }
    else
    {
        //do nothing;
    }
    if (uuc_err < 0)
        uuc_err = 0;
    di->uuc_err = uuc_err;
    di->rbatt_ratio = 0;
}
#define CHARGED_OCV_UPDATE_INTERVAL (10*60*1000)
/**
 * calculate_soc_work - schedule every CALCULATE_SOC_MS.
 */
 static void calculate_soc_work(struct work_struct *work)
{
    struct hisi_hi6421v300_coul_device *di = container_of(work,
                struct hisi_hi6421v300_coul_device,
                calculate_soc_delayed_work.work);

    static int cali_cnt = 0;
    static int charged_cnt = 0;
    static int last_cc=0;
    static int last_time=0;
    int ret = -1;

    check_coul_close_register();
    clear_coul_offset_reg();

    DI_LOCK();
    /* calc soc */
    di->batt_soc = calculate_state_of_charge(di);
    update_uuc_err(di);


   if (cali_cnt % (CALIBRATE_INTERVAL / di->soc_work_interval) == 0)
   {
           cali_adc();
   }
   else if (cali_cnt % (CALIBRATE_INTERVAL / di->soc_work_interval) == 1)
   {
           print_offset();
   }
    cali_cnt++;/*Here coul must calibrate! when first*/
    if (di->charging_state == CHARGING_STATE_CHARGE_DONE){
        if(charged_cnt == 0){
            last_cc = calculate_cc_uah();
            last_time = get_coul_time();
        }
        charged_cnt++;

        if (charged_cnt%(CHARGED_OCV_UPDATE_INTERVAL/CALCULATE_SOC_MS) == 0){
        int sleep_cc, sleep_current;
        int sleep_time, time_now;

            sleep_cc = calculate_cc_uah();
            sleep_cc = sleep_cc - last_cc;
            time_now = get_coul_time();
            sleep_time = time_now - last_time;

        HISI_HI6421V300_COUL_INF("sleep_cc=%d, sleep_time=%d\n", sleep_cc, sleep_time);

            if (sleep_time <= 0){
                charged_cnt --;
            }
            else {
        sleep_current = (sleep_cc * 18) / (sleep_time * 5);

                if(sleep_current<0){
                    sleep_current = -sleep_current;
                }

        HISI_HI6421V300_COUL_INF("sleep_current=%d\n", sleep_current);

                if (sleep_current < 20){
                    int rm;

                    get_ocv_by_vol(di);
                    get_rm(di, &rm);
                    if (0 == di->batt_limit_fcc || rm < di->batt_limit_fcc) {
                        di->batt_limit_fcc = rm * 100 / 101;
                        set_need_save_nv_flag(di);
                    }
                }
            }
            last_cc = calculate_cc_uah();
            last_time = get_coul_time();
        }

    }
    else{
        charged_cnt = 0;
    }

    DI_UNLOCK();
    if(di->is_nv_need_save){
        ret = save_nv_info(di);
        if(!ret)
        {
            di->is_nv_need_save = 0;
        }
    }

    if (di->batt_soc>30){
        di->soc_work_interval = CALCULATE_SOC_MS;
    }
    else{
        di->soc_work_interval = CALCULATE_SOC_MS/2;
    }
    schedule_delayed_work(&di->calculate_soc_delayed_work,
        round_jiffies_relative(msecs_to_jiffies(di->soc_work_interval)) );
}

static void read_temperature_work(struct work_struct *work)
{
    struct hisi_hi6421v300_coul_device *di = container_of(work, struct hisi_hi6421v300_coul_device,
                read_temperature_delayed_work.work);
    update_battery_temperature(di, TEMPERATURE_UPDATE_STATUS);
    schedule_delayed_work(&di->read_temperature_delayed_work, round_jiffies_relative(msecs_to_jiffies(READ_TEMPERATURE_MS)) );
}
static void make_cc_no_overload(struct hisi_hi6421v300_coul_device *di)
{
    int cc;
    unsigned int time;
    cc = calculate_cc_uah();
    cc = cc - di->cc_start_value;
    di->cc_start_value -= cc;
    time = get_coul_time();
    time = time - di->get_cc_start_time;
    di->get_cc_start_time = -time;
    clear_cc_register();
    clear_coul_time();
    set_need_save_nv_flag(di);
}

/**
 * interrupt_notifier_work - send a notifier event to battery monitor.
 */
 static void interrupt_notifier_work(struct work_struct *work)
{
    struct hisi_hi6421v300_coul_device *di = container_of(work,
                struct hisi_hi6421v300_coul_device,
                notifier_work.work);
    unsigned char intstat = di->irq_mask;
    int ibat_ua = 0, vbat_uv = 0;
    int delta_soc = 0;
    get_simultaneous_battery_voltage_and_current(di, &ibat_ua, &vbat_uv);
        if (intstat & COUL_LOW_CUR_CNT_INT)
    {
            HISI_HI6421V300_COUL_EVT("IRQ: COUL_LOW_CUR_CNT_INT\n");
    }
    if (intstat & COUL_VBAT_LOW_INT)
    {
        HISI_HI6421V300_COUL_EVT("IRQ: COUL_VBAT_LOW_INT, vbat=%duv, last vbat_int=%d\n", vbat_uv, di->v_low_int_value);

        vbat_uv /= 1000;
        vbat_uv -= INT_OFFSET;

        if (di->v_low_int_value == LOW_INT_STATE_SLEEP){
            if (vbat_uv < BATTERY_VOL_2_PERCENT){
                delta_soc = di->batt_soc - 2;
                di->batt_soc = 1;
            }
        }
        else if (di->v_low_int_value == LOW_INT_STATE_RUNNING){
            if (vbat_uv < di->v_cutoff){
                delta_soc = di->batt_soc;
                di->batt_soc = 0;
            }
        }

        if (delta_soc > 1){
            short ocvreg = 0;
            HISI_HI6421V300_COUL_EVT("delta_soc=%d, mark save ocv is invalid\n", delta_soc);
            HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_SAVE_OCV_ADDR,&ocvreg,2);
        }
        if(!udp_charge)
        {
            blocking_notifier_call_chain(&notifier_list, BATTERY_LOW_SHUTDOWN, NULL);//hisi_coul_charger_event_rcv(evt);
        }
        di->irq_mask &= ~COUL_VBAT_LOW_INT;
    }
    if (intstat & COUL_LOAD_TIMER_INT)
    {
        HISI_HI6421V300_COUL_INF("IRQ: COUL_LOAD_TIMER_INT\n");
              di->irq_mask &= ~COUL_LOAD_TIMER_INT;

    }
    if (intstat & COUL_CHG_TIMER_INT)
    {
        HISI_HI6421V300_COUL_INF("IRQ: COUL_CHG_TIMER_INT\n");
              di->irq_mask &= ~COUL_CHG_TIMER_INT;
    }
    if (intstat & COUL_CCIN_CNT_INT)
    {
        HISI_HI6421V300_COUL_INF("IRQ: COUL_CCIN_CNT_INT\n");
        make_cc_no_overload(di);
              di->irq_mask &= ~COUL_CCIN_CNT_INT;
    }
    if (intstat & COUL_CCOUT_CNT_INT)
    {
        HISI_HI6421V300_COUL_INF("IRQ: COUL_CCOUT_CNT_INT\n");
        make_cc_no_overload(di);
              di->irq_mask &= ~COUL_CCOUT_CNT_INT;
    }
    if (intstat & COUL_CCOUT_BIG_INT)
    {
        HISI_HI6421V300_COUL_INF("IRQ: COUL_CCOUT_BIG_INT\n");
              di->irq_mask &= ~COUL_CCOUT_BIG_INT;
        // TODO: something ?
    }
}
/**
 * coul_irq_cb - coul irq handler
 */
static irqreturn_t coul_irq_cb(int irq,  void *_di)
{
    struct hisi_hi6421v300_coul_device *di = _di;
    unsigned char val;

    val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_COUL_IRQ_REG);
    HISI_HI6421V300_COUL_INF("hisi_hi6421v300 irq reg=%d\n", val);
    HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_COUL_IRQ_REG,val);
    di->irq_mask |= val;
    schedule_delayed_work(&di->notifier_work, msecs_to_jiffies(0));

    return IRQ_HANDLED;
}


/**
 * aging_update_begin - begin aging update when charging beging.
 * @di: stmartstar coul device
 *
 * should be called after charging beging, and this can't be called
 * in atomic context
 */
static void hisi_hi6421v300_charging_begin (struct hisi_hi6421v300_coul_device *di)
{
    if (di->charging_state == CHARGING_STATE_CHARGE_START)
        return;

    di->charging_start_time = get_coul_time();
    di->charging_state = CHARGING_STATE_CHARGE_START;

    /*calculate soc again*/
    di->batt_soc = calculate_state_of_charge(di);

    /*record soc of charging begin*/
    di->charging_begin_soc = di->batt_soc_real;

    /*record cc value*/
    di->charging_begin_cc = calculate_cc_uah();

    HISI_HI6421V300_COUL_INF("hisi_hi6421v300_charging_begin -\n");

}


static void hisi_hi6421v300_charging_stop (struct hisi_hi6421v300_coul_device *di)
{
    int rm, cc;
    int fcc_101 = di->batt_fcc*101/100;
    int need_save = 0;

    if (CHARGING_STATE_CHARGE_UNKNOW == di->charging_state)
    {
        HISI_HI6421V300_COUL_INF("hisi_hi6421v300_charging_stop return by unknow charge state\n");
        return;
    }
    di->charging_stop_time = get_coul_time();

    // if charge time is less than 5 seconds, in order to avoid SR(nv operation) problem
    // we should not update chargecycle
    if (di->charging_stop_time - di->charging_start_time < 5)
    {
        HISI_HI6421V300_COUL_INF("hisi_hi6421v300_charging_stop return by charge less than 5 seconds\n");
        return;
    }
    di->batt_soc = calculate_state_of_charge(di);

    if (CHARGING_STATE_CHARGE_START == di->charging_state)
    {
        if (update_chargecycles(di))
        {
            HISI_HI6421V300_COUL_INF("hisi_hi6421v300_charging_stop chargecycle update\n");
            need_save = 1;
        }
    }

    cc = calculate_cc_uah();

    cc = cc - di->cc_start_value;

    rm = di->batt_rm - cc;

    if (rm > fcc_101)
    {
        di->cc_start_value -= rm-fcc_101;
        need_save = 1;
        di->batt_limit_fcc = 0;
    }
    else if (di->batt_soc == 100 && di->batt_soc_real > 950)
    {
        need_save = 1;
        di->batt_limit_fcc = rm*100/101;
    }

    if(need_save)
    {
        di->is_nv_need_save = save_nv_info(di);
    }

    di->charging_state = CHARGING_STATE_CHARGE_STOP;
    set_low_vol_int_reg(di, LOW_INT_STATE_RUNNING);
}

static int calculate_real_fcc_uah(struct hisi_hi6421v300_coul_device *di,
                                  int *ret_fcc_uah)
{
    int fcc_uah, unusable_charge_uah, delta_rc;
    int remaining_charge_uah;
    int cc_uah;
    int real_fcc_uah;
    int rbatt;
    int terminate_soc_real;

    terminate_soc_real = di->batt_soc_real;
    calculate_soc_params(di,
                     &fcc_uah,
                     &unusable_charge_uah,
                     &remaining_charge_uah, &cc_uah, &delta_rc, &rbatt);
    real_fcc_uah = (-(cc_uah - di->charging_begin_cc))/(terminate_soc_real - di->charging_begin_soc)*terminate_soc_real;
    //real_fcc_uah = remaining_charge_uah - cc_uah;
    //real_fcc_uah = real_fcc_uah*100/101;
    *ret_fcc_uah = fcc_uah;
    HISI_HI6421V300_COUL_INF("real_fcc=%d, RC=%d CC=%d fcc=%d charging_begin_soc=%d.%d\n",
                   real_fcc_uah, remaining_charge_uah, cc_uah, fcc_uah, di->charging_begin_soc/10, di->charging_begin_soc);
    return real_fcc_uah;
}

static void readjust_fcc_table(struct hisi_hi6421v300_coul_device *di)
{
    struct single_row_lut *temp, *now;
    int i, fcc, ratio;
    int real_fcc_mah = di->fcc_real_mah;
    if (!di->batt_data->fcc_temp_lut)
    {
        HISI_HI6421V300_COUL_ERR("%s The static fcc lut table is NULL\n", __func__);
        return;
    }
    if (di->adjusted_fcc_temp_lut == NULL){
    temp = &di->adjusted_fcc_temp_lut_tbl1;
        now = di->batt_data->fcc_temp_lut;
    }
    else if (di->adjusted_fcc_temp_lut == &di->adjusted_fcc_temp_lut_tbl1){
    temp = &di->adjusted_fcc_temp_lut_tbl2;
        now = di->batt_data->fcc_temp_lut;
    }
    else{
    temp = &di->adjusted_fcc_temp_lut_tbl1;
        now = di->batt_data->fcc_temp_lut;
    }

    fcc = interpolate_fcc(di, di->batt_temp);
    temp->cols = now->cols;
    for (i = 0; i < now->cols; i++)
    {
        u64 rs = 0;

        temp->x[i] = now->x[i];
        rs = now->y[i] * 1000;
        ratio = div_u64(rs, fcc);
        temp->y[i] = (ratio * real_fcc_mah);
        temp->y[i] /= 1000;
        HISI_HI6421V300_COUL_INF("temp=%d, staticfcc=%d, adjfcc=%d, ratio=%d\n",
                           temp->x[i], now->y[i],
                           temp->y[i], ratio);
    }
    di->adjusted_fcc_temp_lut = temp;
}

void refresh_fcc(struct hisi_hi6421v300_coul_device *di)
{
    if (di->charging_begin_soc/10 < MIN_BEGIN_PERCENT_FOR_LEARNING
        && di->batt_ocv_valid_to_refresh_fcc
        && ((di->batt_ocv>3200000 && di->batt_ocv<3670000)
            || (di->batt_ocv>3690000 && di->batt_ocv <3730000)
            || (di->batt_ocv>3800000 && di->batt_ocv <3900000)
            )
        )
    {
        int fcc_uah, new_fcc_uah, delta_fcc_uah,max_delta_fcc_uah;
        new_fcc_uah = calculate_real_fcc_uah(di, &fcc_uah);
        max_delta_fcc_uah = interpolate_fcc(di, di->batt_temp)*DELTA_FCC_PERCENT*10;
        delta_fcc_uah = new_fcc_uah - fcc_uah;
        if (delta_fcc_uah < 0)
            delta_fcc_uah = -delta_fcc_uah;
        if (delta_fcc_uah > max_delta_fcc_uah)
        {
            /* new_fcc_uah is outside the scope limit it */
            if (new_fcc_uah > fcc_uah)
                new_fcc_uah = (fcc_uah + max_delta_fcc_uah);
            else
                new_fcc_uah = (fcc_uah - max_delta_fcc_uah);
            HISI_HI6421V300_COUL_INF("delta_fcc=%d > %d percent of fcc=%d"
                               "restring it to %d\n",
                               delta_fcc_uah, DELTA_FCC_PERCENT,
                               fcc_uah, new_fcc_uah);
        }
        di->fcc_real_mah = new_fcc_uah / 1000;
        HISI_HI6421V300_COUL_EVT("refresh_fcc, start soc=%d, new fcc=%d \n",
            di->charging_begin_soc, di->fcc_real_mah);

        readjust_fcc_table(di);
    }
}

static void hisi_hi6421v300_charging_done (struct hisi_hi6421v300_coul_device *di)
{
    if (CHARGING_STATE_CHARGE_START != di->charging_state) {
        HISI_HI6421V300_COUL_ERR("hisi_hi6421v300_charging_done, but pre charge state is %d \n", di->charging_state);
        return;
    }

    refresh_fcc(di);

    HISI_HI6421V300_COUL_EVT("hisi_hi6421v300_charging_done, adjust soc from %d to 100\n",di->batt_soc);

    di->batt_soc = 100;

    if (di->batt_changed_flag) {
        /*recalculate charge cycles*/
        recalc_chargecycles(di);
    }

    get_ocv_by_fcc(di, di->batt_temp);

    di->batt_fcc  = calculate_fcc_uah(di, di->batt_temp, di->batt_chargecycles/100);
    //di->cc_start_value = -di->batt_fcc/100;

    if (update_chargecycles(di))
    {
        HISI_HI6421V300_COUL_INF("hisi_hi6421v300_charging_done chargecycle update\n");
    }

    set_need_save_nv_flag(di);

    di->charging_state = CHARGING_STATE_CHARGE_DONE;

    HISI_HI6421V300_COUL_INF("new charging cycles = %d%%\n", di->batt_chargecycles);
}

static void charger_event_process(struct hisi_hi6421v300_coul_device *di,unsigned long event)
{
    switch (event) {
    case VCHRG_START_USB_CHARGING_EVENT:
    case VCHRG_START_AC_CHARGING_EVENT:
            HISI_HI6421V300_COUL_EVT("receive charge start event = 0x%x\n",(int)event);
            HISI_HI6421V300_COUL_INF("hisi_hi6421v300_charging_begin +\n");
            HISI_HI6421V300_COUL_INF("pre charging state is %d \n",di->charging_state);
    case VCHRG_START_CHARGING_EVENT:
            /*record soc and cc value*/
            DI_LOCK();
            hisi_hi6421v300_charging_begin(di);
            DI_UNLOCK();
        break;

    case VCHRG_STOP_CHARGING_EVENT:
            HISI_HI6421V300_COUL_EVT("receive charge stop event = 0x%x\n",(int)event);
            DI_LOCK();
        hisi_hi6421v300_charging_stop(di);
        DI_UNLOCK();
        break;

    case VCHRG_CHARGE_DONE_EVENT:
            HISI_HI6421V300_COUL_EVT("receive charge done event = 0x%x\n",(int)event);
            DI_LOCK();
        hisi_hi6421v300_charging_done(di);
        DI_UNLOCK();
        break;

    case VCHRG_NOT_CHARGING_EVENT:
        di->charging_state = CHARGING_STATE_CHARGE_NOT_CHARGE;
            HISI_HI6421V300_COUL_ERR("charging is stop by fault\n");
        break;

    case VCHRG_POWER_SUPPLY_OVERVOLTAGE:
        di->charging_state = CHARGING_STATE_CHARGE_NOT_CHARGE;
            HISI_HI6421V300_COUL_ERR("charging is stop by overvoltage\n");
        break;

    case VCHRG_POWER_SUPPLY_WEAKSOURCE:
        di->charging_state = CHARGING_STATE_CHARGE_NOT_CHARGE;
            HISI_HI6421V300_COUL_ERR("charging is stop by weaksource\n");
        break;

    default:
            di->charging_state = CHARGING_STATE_CHARGE_NOT_CHARGE;
        HISI_HI6421V300_COUL_ERR("unknow event %d\n",(int)event);
        break;
    }
}

/**
 * hisi_hi6421v300_battery_charger_event_rcv
 */
int hisi_hi6421v300_battery_charger_event_rcv (unsigned long evt)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (!di || !di->batt_exist){
        return 0;
    }
    if (!is_hi6421v300_battery_exist()){
        return 0;
    }

    charger_event_process(di,evt);

    return 0;
}



/**
 * hisi_hi6421v300_coul_config_power_on_reg - config hisi_hi6421v300 registers.
 */
static void hisi_hi6421v300_coul_config_power_on_reg(struct hisi_hi6421v300_coul_device *di)
{
    /* Mode */
    HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_COUL_CTRL_REG,DEFAULT_COUL_CTRL_VAL);
}

static void hisi_hi6421v300_enable_coul_irq(void)
{
    /* coulometer interrupt mask */
    HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_COUL_IRQ_MASK_REG,DEFAULT_INT_MASK_VAL);
}


#ifdef HISI_HI6421V300_DEBUG
void ss_cur_show(void)
{
    int i,ua;
    short regval = 0;

    for (i=0; i<20; i++) {
        HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CUR_FIFO_BASE+i*2, &regval, 2);
        ua = convert_regval2ua(regval);
        printk(KERN_ERR"[%02d]:regval=0x%04x,cur = %d ua \n",i,regval,ua);
    }
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CUR_FIFO_BASE, &regval, 2);
    ua = convert_regval2ua(regval);
    printk(KERN_ERR "***CLJ ST*** CURRENT = %d ua ***CLJ ST***\n", ua);
}

void ss_vol_show(void)
{
    int i,uv;
    short regval = 0;

    for (i=0; i<20; i++) {
        HISI_HI6421V300_REGS_READ(HISI_HI6421V300_VOL_FIFO_BASE+i*2, &regval, 2);
        uv = convert_regval2uv(regval);
        printk(KERN_ERR"[%02d]:regval=0x%04x,vol = %d uv \n",i,regval,uv);
    }
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_VOL_FIFO_BASE, &regval, 2);
    uv = convert_regval2uv(regval);
    printk(KERN_ERR "***CLJ ST*** VOLTAGE = %d uv ***CLJ ST***\n", uv);
}

void ss_ocv_show(void)
{
    short ocvreg = 0;
    short volreg_offset = 0;
    int ocv;

    /*read ocv and offset */
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_OCV_REG_BASE,&ocvreg,2);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_OFFSET_VOLTAGE,&volreg_offset,2);

    ocv = convert_regval2uv(ocvreg - volreg_offset);

    printk(KERN_ERR"regval=0x%04x,offset = 0x%04x,ocv = %d uv \n",\
            ocvreg,volreg_offset,ocv);
    printk(KERN_ERR "***CLJ ST*** OCV = %d uv ***CLJ ST***\n", ocv);
}

void ss_get_ocv(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    if (di)
        get_initial_ocv(di);
}

void ss_cc_show(void)
{
    unsigned int cc_in = 0;
    unsigned int cc_out= 0;
    int cc_uah_in = 0;
    int cc_uah_out = 0;
    int cc_uah = 0;
    int cc_uah_adjust = 0;
    s64 temp = 0;
    unsigned int cl_in_time = 0;
    unsigned int cl_out_time = 0;

    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CL_IN_BASE,&cc_in,4);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CL_OUT_BASE,&cc_out,4);
    cc_uah_out = convert_regval2uah(cc_out);
    cc_uah_in = convert_regval2uah(cc_in);
    cc_uah = cc_uah - cc_uah_in;

    temp = (s64) c_offset_a *cc_uah_in;
    cc_uah_in = div_s64(temp, 1000000);
    temp = (s64) c_offset_a *cc_uah_out;
    cc_uah_out = div_s64(temp, 1000000);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CHG_TIMER_BASE, &cl_in_time, 4);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_LOAD_TIMER_BASE, &cl_out_time, 4);
    /* uah = uas/3600 = ua*s/3600 */
    cc_uah_in = cc_uah_in - div_s64((s64) cl_in_time * c_offset_b, 3600);
    cc_uah_out =
        cc_uah_out + div_s64((s64) cl_out_time * c_offset_b, 3600);
    cc_uah_adjust = cc_uah_out - cc_uah_in;

    printk(KERN_ERR "ccout_reg=0x%x,ccin_reg = 0x%x,cc = %d uah \n"
           "cl_out_time=%d, cl_in_time=%d, cc_adjust=%d uah\n",
           cc_out, cc_in, cc_uah,
           cl_out_time, cl_in_time, cc_uah_adjust);
}

void ss_time_show(void)
{
    int cl_in_time = 0;
    int cl_out_time = 0;

    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_CHG_TIMER_BASE,&cl_in_time,4);
    HISI_HI6421V300_REGS_READ(HISI_HI6421V300_LOAD_TIMER_BASE,&cl_out_time,4);

    printk(KERN_ERR"cl_in_time=%d,cl_out_time = %d,total = %d\n", cl_in_time, cl_out_time, (cl_in_time+cl_out_time));
}

void ss_soc_update(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    unsigned int old_soc;
    old_soc = di->batt_soc;
    di->batt_soc = calculate_state_of_charge(di);
    printk(KERN_ERR"batt_soc %d ---> %d \n",old_soc,di->batt_soc);
}

void ss_soc_show(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    printk(KERN_ERR "***CLJ ST*** SOC = %d ***CLJ ST***\n", di->batt_soc);
}

void ss_di_show(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    printk(KERN_ERR"batt_exist = 0x%x\n", di->batt_exist);
    printk(KERN_ERR"prev_pc_unusable = %d\n", di->prev_pc_unusable);
    printk(KERN_ERR"irq[i] = %d\n", di->irq);
    printk(KERN_ERR"irq_mask = 0x%x\n", di->irq_mask);
    printk(KERN_ERR"batt_ocv_cur_limit = %d\n", di->batt_ocv_cur_limit);
    printk(KERN_ERR"batt_ocv_curreglimit = %d\n", di->batt_ocv_curreglimit);
    printk(KERN_ERR"batt_ocv = %d\n", di->batt_ocv);
    printk(KERN_ERR"batt_changed_flag = %d\n", di->batt_changed_flag);
    printk(KERN_ERR"batt_vol_low_flag = %d\n", di->batt_vol_low_flag);
    printk(KERN_ERR"soc_limit_flag = %d\n", di->soc_limit_flag);
    printk(KERN_ERR"batt_temp = %d\n", di->batt_temp);
    printk(KERN_ERR"batt_fcc = %d\n", di->batt_fcc);
    printk(KERN_ERR"batt_limit_fcc = %d\n", di->batt_limit_fcc);
    printk(KERN_ERR"batt_rm = %d\n", di->batt_rm);
    printk(KERN_ERR"batt_ruc = %d\n", di->batt_ruc);
    printk(KERN_ERR"batt_uuc = %d\n", di->batt_uuc);
    printk(KERN_ERR"rbatt = %d\n", di->rbatt);
    printk(KERN_ERR"r_pcb = %d\n", di->r_pcb);
    printk(KERN_ERR"soc_work_interval = %d\n", di->soc_work_interval);
    printk(KERN_ERR"charging_begin_soc = %d\n", di->charging_begin_soc);
    printk(KERN_ERR"charging_state = %d\n", di->charging_state);
    printk(KERN_ERR"batt_soc = %d\n", di->batt_soc);
    printk(KERN_ERR"batt_soc_real = %d\n", di->batt_soc_real);
    printk(KERN_ERR"batt_soc_with_uuc = %d\n", di->batt_soc_with_uuc);
    printk(KERN_ERR"batt_soc_est = %d\n", di->batt_soc_est);
    printk(KERN_ERR"batt_id_vol = %d\n", di->batt_id_vol);
    printk(KERN_ERR"batt_chargecycles = %d\n", di->batt_chargecycles);
    printk(KERN_ERR"last_cali_temp = %d\n", di->last_cali_temp);
    printk(KERN_ERR"cc_end_value = %d\n", di->cc_end_value);
    printk(KERN_ERR"cc_start_value = %d\n", di->cc_start_value);
    printk(KERN_ERR"v_cutoff = %d\n", di->v_cutoff);
    printk(KERN_ERR"v_low_int_value = %d\n", di->v_low_int_value);
    printk(KERN_ERR"get_cc_start_time = %d\n", di->get_cc_start_time);
    printk(KERN_ERR"get_cc_end_time = %d\n", di->get_cc_end_time);
    printk(KERN_ERR"suspend_time = %d\n", di->suspend_time);
    printk(KERN_ERR"charging_begin_cc = %d\n", di->charging_begin_cc);
    printk(KERN_ERR"suspend_cc = %d\n", di->suspend_cc);
    printk(KERN_ERR"last_time = %d\n", di->last_time);
    printk(KERN_ERR"last_cc = %d\n", di->last_cc);
    printk(KERN_ERR"last_iavg_ma = %d\n", di->last_iavg_ma);
    printk(KERN_ERR"fcc_real_mah = %d\n", di->fcc_real_mah);
    printk(KERN_ERR"is_nv_read = %d\n", di->is_nv_read);
    printk(KERN_ERR"is_nv_need_save = %d\n", di->is_nv_need_save);
    printk(KERN_ERR"dbg_ocv_cng_0 = %d\n",di->dbg_ocv_cng_0);
    printk(KERN_ERR"dbg_ocv_cng_1 = %d\n",di->dbg_ocv_cng_1);
    printk(KERN_ERR"dbg_valid_vol = %d\n",di->dbg_valid_vol);
    printk(KERN_ERR"dbg_invalid_vol = %d\n",di->dbg_invalid_vol);
    printk(KERN_ERR"dbg_ocv_fc_failed = %d\n",di->dbg_ocv_fc_failed);

}

void ss_offset_show(void)
{
    struct hisi_nve_info_user nve;
    struct hisi_hi6421v300_coul_nv_info *pinfo = (struct hisi_hi6421v300_coul_nv_info *)&nve.nv_data[0];
    int ret = 0;

    memset(&nve, 0, sizeof(nve));
    strncpy(nve.nv_name, HISI_HI6421V300_COUL_NV_NAME, sizeof(HISI_HI6421V300_COUL_NV_NAME));
    nve.nv_number = HISI_HI6421V300_COUL_NV_NUM;
    nve.valid_size = sizeof(*pinfo);
    nve.nv_operation = NV_READ;

    ret = hisi_nve_direct_access(&nve);
    if (ret) {
        HISI_HI6421V300_COUL_INF("read nv partion failed, ret=%d\n", ret);
        return;
    }

    HISI_HI6421V300_COUL_ERR("C_offset: a=%d, b=%d uA V_offset: a=%d, b=%d uV, r_pcb=%d uohm\n",
         pinfo->c_offset_a, pinfo->c_offset_b, pinfo->v_offset_a, pinfo->v_offset_b, pinfo->r_pcb);
}

void ss_offset_set(int c_a, int c_b, int v_a, int v_b, int r_pcb)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    HISI_HI6421V300_COUL_ERR("C_offset: a=%d, b=%d uA V_offset: a=%d, b=%d uV, r_pcb=%d uohm\n",
         c_offset_a, c_offset_b, v_offset_a, v_offset_b, di->r_pcb);

    save_nv_info(di);
}

int ss_realfcc_show(void)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    int i;
    struct single_row_lut *preal_fcc_lut = di->adjusted_fcc_temp_lut;

    if (preal_fcc_lut == NULL){
        HISI_HI6421V300_COUL_INF("no real fcc data in nv\n");
        return 0;
    }

    for (i=0; i<preal_fcc_lut->cols; i++)
    {
        HISI_HI6421V300_COUL_INF("temp=%d real_fcc=%d\n", preal_fcc_lut->x[i], preal_fcc_lut->y[i]);
    }

    return di->fcc_real_mah;
}

void ss_realfcc_set(int fcc_mah)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if (fcc_mah == 0)
    {
        di->adjusted_fcc_temp_lut = NULL;
        di->batt_limit_fcc = 0;
    }
    else
    {
        di->fcc_real_mah = fcc_mah;
        readjust_fcc_table(di);
    }
    save_nv_info(di);
}

#endif

static ssize_t hisi_coul_show_pl_v_offset_a(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", v_offset_a);
}
static ssize_t hisi_coul_set_pl_v_offset_a(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if(strict_strtol(buf, 10, &val) < 0)
        return -EINVAL;

    v_offset_a = (int)val;//clear it before calibration and make it effective realtime.

    return status;
}

static ssize_t hisi_coul_show_pl_v_offset_b(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", v_offset_b);
}
static ssize_t hisi_coul_set_pl_v_offset_b(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if(strict_strtol(buf, 10, &val) < 0)
        return -EINVAL;

    v_offset_b = (int)val;//clear it before calibration and make it effective realtime.

    return status;
}

static ssize_t hisi_coul_show_pl_c_offset_a(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", c_offset_a);
}
static ssize_t hisi_coul_set_pl_c_offset_a(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if(strict_strtol(buf, 10, &val) < 0)
        return -EINVAL;

    c_offset_a = (int)val;//clear it before calibration and make it effective realtime.

    return status;
}

static ssize_t hisi_coul_show_pl_c_offset_b(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", c_offset_b);
}
static ssize_t hisi_coul_set_pl_c_offset_b(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if(strict_strtol(buf, 10, &val) < 0)
        return -EINVAL;

    c_offset_b = (int)val;//clear it before calibration and make it effective realtime.

    return status;
}

static ssize_t hisi_coul_show_ate_v_offset(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "0x%x\n", HISI_HI6421V300_REG_READ(HISI_HI6421V300_CALI_PARAMS_BASE));
}
static ssize_t hisi_coul_show_do_save_offset_ret(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", do_save_offset_ret);
}

static ssize_t hisi_coul_do_save_offset(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;
    int status = count;

    if (!di)
    {
        do_save_offset_ret = -EINVAL;
        HISI_HI6421V300_COUL_ERR("%s: di is NULL\n", __func__);
        return do_save_offset_ret;
    }
    do_save_offset_ret = save_cali_param();
    return status;
}

static ssize_t hisi_hi6421v300_show_gaugelog_head(struct device *dev, struct device_attribute *attr, char *buf)
{
    snprintf(buf, PAGE_SIZE, "ss_VOL  ss_CUR  ss_ufSOC  ss_SOC  SOC  ss_RM  ss_FCC  ss_UUC  UUC_ERR  ss_CC  ss_dRC  Temp  ss_OCV   rbatt  fcc    uuc_pc  soc_est  rbat_calc  ocv_delrc  vavg  cavg  delta_ocv  delta_rc_mid  delta_rc_100  iavg_ua  soc_before  ");
    return strlen(buf);
}

static ssize_t hisi_hi6421v300_show_gaugelog(struct device *dev, struct device_attribute *attr, char *buf)
{
    int temp = 0, voltage = 0, ufcapacity = 0, capacity = 100, afcapacity = 0, rm = 0, fcc = 0, delta_rc = 0;
    int cur = 0,uuc = 0,cc = 0, ocv=0, rbatt;
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    if(NULL == buf){
        return -1;
    }

    if (di == NULL)
        return snprintf(buf, PAGE_SIZE, "%s", "hisi_hi6421v300 coulometer probe failed!");

    temp =  hisi_hi6421v300_battery_temperature();
    voltage = hisi_hi6421v300_battery_voltage();
    cur = -(hisi_hi6421v300_battery_current());
    ufcapacity = hisi_hi6421v300_battery_unfiltered_capacity();
    capacity = hisi_hi6421v300_battery_capacity();
    afcapacity = bq_bci_show_capacity();
    rm =   hisi_hi6421v300_battery_rm();
    fcc =  hisi_hi6421v300_battery_fcc();
    uuc = hisi_hi6421v300_battery_uuc();
    cc = hisi_hi6421v300_battery_cc();
    delta_rc = hisi_hi6421v300_battery_delta_rc();
    ocv = hisi_hi6421v300_battery_ocv();
    rbatt = hisi_hi6421v300_battery_resistance();

    snprintf(buf, PAGE_SIZE, "%-6d  %-6d  %-8d  %-6d  %-3d  %-5d  %-6d  %-6d  %-6d  %-5d  %-6d  %-4d  %-7d  %-5d  %-5d  %-6d  %-6d  %-6d  %-6d  %-6d  %-6d  %-6d  %-6d  %-6d  %-6d  %-6d  ", voltage,  (signed short)cur, ufcapacity, capacity, afcapacity, rm, fcc, uuc, di->uuc_err/1000, cc, delta_rc, temp, ocv, rbatt, di->batt_limit_fcc/1000, glog->uuc_pc, glog->soc_est, glog->rbat_calc, glog->ocv_for_delrc, glog->vavg, glog->cavg, glog->delta_ocv, glog->delta_rc_mid, glog->delta_rc_100, glog->iavg_ua, glog->soc_before_limit);

    return strlen(buf);
}

static ssize_t hisi_hi6421v300_set_hand_chg_capacity_flag(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;
    hand_chg_capacity_flag = (unsigned int)val;
    return status;
}

static ssize_t hisi_hi6421v300_show_hand_chg_capacity_flag(struct device *dev, struct device_attribute *attr, char *buf)
{
    unsigned int val;

    val = hand_chg_capacity_flag;
    return snprintf(buf, PAGE_SIZE, "%u\n", val);
}

static ssize_t hisi_hi6421v300_set_input_capacity(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 100))
        return -EINVAL;
    input_capacity = (unsigned int) val;
    return status;
}

static ssize_t hisi_hi6421v300_show_input_capacity(struct device *dev, struct device_attribute *attr, char *buf)
{
    unsigned int val;

    val = input_capacity;
    return snprintf(buf, PAGE_SIZE, "%u\n", val);
}

static ssize_t hisi_hi6421v300_show_abs_cc(struct device *dev, struct device_attribute *attr, char *buf)
{
    int val = 0;

    val = hisi_saved_abs_cc_mah + (calculate_cc_uah() / 1000);
    return snprintf(buf, PAGE_SIZE, "%d\n", val);
}

static ssize_t hisi_hi6421v300_show_id_voltage(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct hisi_hi6421v300_coul_device *di = g_hisi_hi6421v300_coul_dev;

    return snprintf(buf, PAGE_SIZE, "%d\n", di->batt_id_vol);
}
static DEVICE_ATTR(pl_v_offset_a, (S_IWUSR | S_IRUGO),
                hisi_coul_show_pl_v_offset_a,
                hisi_coul_set_pl_v_offset_a);
static DEVICE_ATTR(pl_v_offset_b, (S_IWUSR | S_IRUGO),
                hisi_coul_show_pl_v_offset_b,
                hisi_coul_set_pl_v_offset_b);
static DEVICE_ATTR(pl_c_offset_a, (S_IWUSR | S_IRUGO),
                hisi_coul_show_pl_c_offset_a,
                hisi_coul_set_pl_c_offset_a);
static DEVICE_ATTR(pl_c_offset_b, (S_IWUSR | S_IRUGO),
                hisi_coul_show_pl_c_offset_b,
                hisi_coul_set_pl_c_offset_b);
static DEVICE_ATTR(ate_v_offset, (S_IRUGO),
                hisi_coul_show_ate_v_offset,
                NULL);
static DEVICE_ATTR(do_save_offset, (S_IWUSR | S_IRUGO),
                hisi_coul_show_do_save_offset_ret,
                hisi_coul_do_save_offset);
static DEVICE_ATTR(gaugelog_head, S_IWUSR | S_IWGRP | S_IRUGO,
                hisi_hi6421v300_show_gaugelog_head,
                NULL);
static DEVICE_ATTR(gaugelog, S_IWUSR | S_IWGRP | S_IRUGO,
                hisi_hi6421v300_show_gaugelog,
                NULL);
static DEVICE_ATTR(hand_chg_capacity_flag, S_IWUSR | S_IRUGO,
                hisi_hi6421v300_show_hand_chg_capacity_flag,
                hisi_hi6421v300_set_hand_chg_capacity_flag);
static DEVICE_ATTR(input_capacity, S_IWUSR | S_IRUGO,
                hisi_hi6421v300_show_input_capacity,
                hisi_hi6421v300_set_input_capacity);
static DEVICE_ATTR(abs_cc, S_IWUSR | S_IRUGO,
                hisi_hi6421v300_show_abs_cc,
                NULL);
static DEVICE_ATTR(id_voltage, S_IWUSR | S_IRUGO,
                hisi_hi6421v300_show_id_voltage,
                NULL);
static struct attribute *coul_attributes[] = {
    &dev_attr_pl_v_offset_a.attr,
    &dev_attr_pl_v_offset_b.attr,
    &dev_attr_pl_c_offset_a.attr,
    &dev_attr_pl_c_offset_b.attr,
    &dev_attr_ate_v_offset.attr,
    &dev_attr_do_save_offset.attr,
    &dev_attr_gaugelog_head.attr,
    &dev_attr_gaugelog.attr,
    &dev_attr_hand_chg_capacity_flag.attr,
    &dev_attr_input_capacity.attr,
    &dev_attr_abs_cc.attr,
    &dev_attr_id_voltage.attr,
    NULL,
};
static const struct attribute_group coul_attr_group = {
    .attrs = coul_attributes,
};

static int coul_shutdown_prepare(struct notifier_block *nb, unsigned long event, void *_data)
{
    struct hisi_hi6421v300_coul_device *di = container_of(nb, struct hisi_hi6421v300_coul_device, nb);

    switch (event)
    {
    case SYS_DOWN:
    case SYS_HALT:
    case SYS_POWER_OFF:
        HISI_HI6421V300_COUL_INF("coul prepare to shutdown, event = %ld\n",event);
        cancel_delayed_work_sync(&di->calculate_soc_delayed_work);
        break;
    default:
        HISI_HI6421V300_COUL_ERR("error event, coul ignore, event = %ld\n",event);
        break;
    }
    return 0;
}

static struct platform_driver hisi_hi6421v300_coul_driver;
static int hisi_hi6421v300_coul_probe(struct platform_device *pdev)
{
    struct hisi_coul_ops *coul_ops = NULL;
    struct hisi_hi6421v300_coul_device *di = NULL;
    int val,retval,coul_irq;
    int tryloop = 0;
    struct device_node* np;
    struct class *power_class = NULL;
    struct device *new_dev = NULL;

    pmic = dev_get_drvdata(pdev->dev.parent);
    if(NULL == pmic)
    {
        HISI_HI6421V300_COUL_ERR("failed to get parent resource!\n");
        return -ENOENT;
    }
    np = pdev->dev.of_node;
    if(NULL == np)
    {
        return -ENOENT;
    }
    coul_irq = platform_get_irq_byname(pdev,"coulirq");
    if(coul_irq < 0)
    {
        HISI_HI6421V300_COUL_ERR("failed to get irq resource!\n");
        return -ENOENT;
    }
    udp_charge = of_property_read_bool(np,"udp_charge");
    HISI_HI6421V300_COUL_EVT("udp_charge = %d\n",(int)udp_charge);
    do
    {
        retval = HISI_HI6421V300_REG_READ(HISI_HI6421V300_COUL_VERSION_ADDR);
        HISI_HI6421V300_COUL_INF("do a dummy read, hisi_hi6421v300 version is 0x%x\n",retval);
        udelay(500);
        if ((tryloop++) > VERSION_READ_RETYR_TIMES)
        {
            HISI_HI6421V300_COUL_ERR("hisi_hi6421v300 version is not correct!\n");
            goto coul_failed;
        }
    } while(retval != HISI_HI6421V300_COUL_VERSION);

    val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_DEBUG_REG);
    //to do: need to check why not use state register to detect coul is ready
    udelay(500);
    if(val != 0x0)
    {
        short ocvreg = 0x0;
        HISI_HI6421V300_COUL_ERR("hisi_hi6421v300 debug reg is not 0x0,debug_reg = 0x%x\n", val);
        HISI_HI6421V300_REG_WRITE(HISI_HI6421V300_DEBUG_REG, 0x0);
        udelay(500);
        HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_SAVE_OCV_ADDR, &ocvreg, 2);
        udelay(500);
        BUG();
    }
    clear_coul_offset_reg();

    retval = sysfs_create_group(&pdev->dev.kobj, &coul_attr_group);
    if (retval) {
        hwlog_err("%s failed to create sysfs group!!!\n", __FUNCTION__);
        return -1;
    }
    power_class = hw_power_get_class();
    if(power_class)
    {
        new_dev = device_create(power_class, NULL, 0, NULL, "coul");
        if(new_dev)
        {
            retval = sysfs_create_link(&new_dev->kobj, &pdev->dev.kobj, "coul_data");
            if(0 != retval)
                hwlog_err("%s failed to create sysfs link!!!\n", __FUNCTION__);
        }
        else
        {
            hwlog_err("%s failed to create new_dev!!!\n", __FUNCTION__);
        }
    }
    di = (struct hisi_hi6421v300_coul_device *)kzalloc(sizeof(*di), GFP_KERNEL);
    if (!di)
    {
        HISI_HI6421V300_COUL_ERR("%s failed to alloc di struct\n",__FUNCTION__);
        retval = -ENOMEM;
        goto coul_failed_0;
    }

    g_hisi_hi6421v300_coul_dev = di;
    di->irq = coul_irq;
    platform_set_drvdata(pdev, di);

    /*set di element */
    di->prev_pc_unusable = -EINVAL;
    di->batt_pre_delta_rc = 0;
    di->r_pcb = 0;
    di->v_cutoff = 3150;//3200;
    di->batt_ocv_cur_limit = 25000; /*ua*/
    di->batt_ocv_curreglimit = (short)convert_ua2regval(di->batt_ocv_cur_limit);
    di->v_low_int_value = LOW_INT_NOT_SET;
    di->last_cali_temp = -990; /* invalid temperature */
    di->soc_work_interval = CALCULATE_SOC_MS;
    di->last_iavg_ma = IMPOSSIBLE_IAVG;
    /* read nv info */
    di->is_nv_read = !get_initial_params(di);
    val = HISI_HI6421V300_REG_READ(HISI_HI6421V300_NV_READ_SUCCESS);
    val = val & NV_READ_SUCCESS;
    di->is_nv_read = !!val;

    di->is_nv_need_save = 0;
    mutex_init(&di->soc_mutex);
    di->sr_resume_time = get_coul_time();
    sr_cur_state = SR_DEVICE_WAKEUP;

    update_battery_temperature(di, TEMPERATURE_INIT_STATUS);
    /*check battery is exist*/
    if (!is_hi6421v300_battery_exist()) {
        HISI_HI6421V300_COUL_ERR("%s: no battery, just registe callback\n",__FUNCTION__);
        //retval = -EPERM;
        di->batt_data = get_battery_data(di->batt_id_vol);
        HISI_HI6421V300_COUL_EVT("battery type is %s\n", di->batt_data->batt_brand);
        di->batt_exist = 0;
        goto coul_no_battery;
    }

    di->batt_exist = 1;

    /*set battery data*/
    di->batt_data = get_battery_data(di->batt_id_vol);
    HISI_HI6421V300_COUL_EVT("battery type is %s\n", di->batt_data->batt_brand);
    if (NULL == di->batt_data) {
        HISI_HI6421V300_COUL_ERR("%s: batt ID(0x%x) is invalid\n",__FUNCTION__,di->batt_id_vol);
        retval = -EINVAL;
        goto coul_failed_1;
    }

    INIT_DELAYED_WORK(&di->calculate_soc_delayed_work, calculate_soc_work);
    INIT_DELAYED_WORK(&di->read_temperature_delayed_work, read_temperature_work);
    di->nb.notifier_call = coul_shutdown_prepare;
    register_reboot_notifier(&(di->nb));

    /*calculate first soc */
    get_initial_ocv(di);
    di->charging_stop_time = get_coul_time();

    /*config coul reg */
    hisi_hi6421v300_coul_config_power_on_reg(di);

    if(is_hisi_hi6421v300_battery_moved()){
        di->batt_chargecycles = 0;
        di->batt_changed_flag = 1;
        di->batt_limit_fcc = 0;
        di->adjusted_fcc_temp_lut = NULL; /* enable it when test ok */
        set_need_save_nv_flag(di);
        HISI_HI6421V300_COUL_INF("battery changed, reset chargecycles!\n");
    } else {
        HISI_HI6421V300_COUL_INF("battery not changed, chargecycles = %d%%\n", di->batt_chargecycles);
    }

    /*get the first soc value*/
    DI_LOCK();
    di->soc_limit_flag = 0;
    di->batt_soc = calculate_state_of_charge(di);
    di->soc_limit_flag = 1;
    DI_UNLOCK();

    /*schedule calculate_soc_work*/
    schedule_delayed_work(&di->calculate_soc_delayed_work, round_jiffies_relative(msecs_to_jiffies(di->soc_work_interval)));
    schedule_delayed_work(&di->read_temperature_delayed_work, round_jiffies_relative(msecs_to_jiffies(READ_TEMPERATURE_MS)) );

    INIT_DELAYED_WORK(&di->notifier_work,
                      interrupt_notifier_work);


    set_low_vol_int_reg(di, LOW_INT_STATE_RUNNING);


    retval = request_irq(di->irq, coul_irq_cb, IRQF_NO_SUSPEND, pdev->name, di);
    if (retval)
    {
        HISI_HI6421V300_COUL_ERR("failed to request irq\n");
        BUG();
        //goto coul_failed_2;
    }
    hisi_hi6421v300_enable_coul_irq();

coul_no_battery:
    coul_ops = (struct hisi_coul_ops*) kzalloc(sizeof (*coul_ops), GFP_KERNEL);
    if (!coul_ops) {
        HISI_HI6421V300_COUL_ERR("failed to alloc coul_ops struct\n");
        retval = -ENOMEM;
        goto coul_failed_3;
    }

    coul_ops->is_coul_ready = is_hisi_hi6421v300_coul_ready;
    coul_ops->is_battery_exist = is_hi6421v300_battery_exist;
    coul_ops->is_battery_reach_threshold = is_hisi_hi6421v300_battery_reach_threshold;
    coul_ops->battery_brand = hisi_hi6421v300_battery_brand;
    coul_ops->battery_id_voltage = hisi_hi6421v300_battery_id_voltage;
    coul_ops->battery_voltage = hisi_hi6421v300_battery_voltage;
    coul_ops->battery_voltage_uv= hisi_hi6421v300_battery_voltage_uv;
    coul_ops->battery_current = hisi_hi6421v300_battery_current;
    coul_ops->battery_current_avg = hisi_hi6421v300_battery_current_avg;
    coul_ops->battery_unfiltered_capacity = hisi_hi6421v300_battery_unfiltered_capacity;
    coul_ops->battery_capacity = hisi_hi6421v300_battery_capacity;
    coul_ops->battery_temperature = hisi_hi6421v300_battery_temperature;
    coul_ops->battery_rm = hisi_hi6421v300_battery_rm;
    coul_ops->battery_fcc = hisi_hi6421v300_battery_fcc;
    coul_ops->battery_tte = hisi_hi6421v300_battery_tte;
    coul_ops->battery_ttf = hisi_hi6421v300_battery_ttf;
    coul_ops->battery_health = hisi_hi6421v300_battery_health;
    coul_ops->battery_capacity_level = hisi_hi6421v300_battery_capacity_level;
    coul_ops->battery_technology = hisi_hi6421v300_battery_technology;
    coul_ops->battery_charge_params = hisi_hi6421v300_battery_charge_params;
    coul_ops->battery_vbat_max = hisi_hi6421v300_battery_vbat_max;
    coul_ops->charger_event_rcv = hisi_hi6421v300_battery_charger_event_rcv;
    coul_ops->battery_cycle_count = hisi_hi6421v300_battery_cycle_count;
    coul_ops->battery_fcc_design = hisi_hi6421v300_battery_fcc_design;

    di->ops = coul_ops;
    retval = hisi_coul_ops_register(coul_ops,COUL_HISI_HI6421V300);
    if (retval) {
        HISI_HI6421V300_COUL_ERR("failed to register coul ops\n");
        goto coul_failed_4;
    }

    HISI_HI6421V300_COUL_EVT("coul probe ok!\n");
    g_pdev = pdev;                /* for debug */

    if (!pmu_coul_dclient) {
        pmu_coul_dclient = dsm_register_client(&dsm_pmu_coul);
    }

    return retval;


coul_failed_4:
    kfree(coul_ops);
    di->ops = NULL;
coul_failed_3:
    free_irq(di->irq, di);
    cancel_delayed_work(&di->calculate_soc_delayed_work);
    cancel_delayed_work(&di->read_temperature_delayed_work);
//coul_failed_2:
    di->irq = 0;
coul_failed_1:
    platform_set_drvdata(pdev, NULL);
    g_hisi_hi6421v300_coul_dev = NULL;
coul_failed_0:
    kfree(di);
coul_failed:
    HISI_HI6421V300_COUL_ERR("hisi_hi6421v300 porbe failed!\n");
    return retval;
}

static void hisi_hi6421v300_coul_shutdown(struct platform_device *pdev)
{

    struct hisi_hi6421v300_coul_device *di = platform_get_drvdata(pdev);

    HISI_HI6421V300_COUL_INF("...shutdown...\n");
    if (NULL == di)
    {
        HISI_HI6421V300_COUL_ERR("[coul_shutdown]:di is NULL\n");
        return;
    }

    disable_irq(di->irq);
    cancel_delayed_work(&di->notifier_work);
    cancel_delayed_work(&di->read_temperature_delayed_work);

#if RBATT_ADJ
    if (di->batt_soc <= 2)
    {
        int ocv_invalid = 0;
        short ocvreg = 0;
        if (di->rbatt_ratio >= 400)
        {
            ocv_invalid = 1;
            HISI_HI6421V300_COUL_EVT("rbatt_ratio(%d) > 400, mark save ocv is invalid\n", di->rbatt_ratio);
        }
        if (di->batt_delta_rc)
        {
            ocv_invalid = 1;
            HISI_HI6421V300_COUL_EVT("batt_delta_rc=%d, mark save ocv is invalid\n", di->batt_delta_rc);
        }

        if (ocv_invalid)
        {
            HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_SAVE_OCV_ADDR,&ocvreg,2);
        }
    }
#endif
}
static int hisi_hi6421v300_coul_remove(struct platform_device *pdev)
{
    struct hisi_hi6421v300_coul_device *di = platform_get_drvdata(pdev);

    if (NULL == di)
    {
        HISI_HI6421V300_COUL_ERR("[coul_remove]:di is NULL\n");
        return 0;
    }

    cancel_delayed_work(&di->calculate_soc_delayed_work);
    disable_irq(di->irq);
    cancel_delayed_work(&di->notifier_work);
    cancel_delayed_work(&di->read_temperature_delayed_work);

    return 0;
}

#ifdef CONFIG_PM
static int hisi_hi6421v300_coul_suspend(struct platform_device *pdev,
    pm_message_t state)
{
    struct hisi_hi6421v300_coul_device *di = platform_get_drvdata(pdev);
    int current_sec;
    int wakeup_time = 0;

    printk("%s+.\n",__func__);
    if (NULL == di)
    {
        HISI_HI6421V300_COUL_ERR("[coul_suspend]:di is NULL\n");
        return 0;
    }
    current_sec = get_coul_time();
    DI_LOCK();
    di->suspend_cc = calculate_cc_uah();
    di->suspend_time = current_sec;
    di->sr_suspend_time = current_sec;
    wakeup_time = current_sec - di->sr_resume_time;
    if (wakeup_time > SR_MAX_RESUME_TIME)
    {
        clear_sr_time_array();
        HISI_HI6421V300_COUL_INF("[SR]%s(%d): wakeup_time(%d) > SR_MAX_RESUME_TIME(%d)\n", __func__, __LINE__, wakeup_time, SR_MAX_RESUME_TIME);
    }
    else if (wakeup_time >= 0)
    {
        sr_time_wakeup[sr_index_wakeup] = wakeup_time;
        sr_index_wakeup++;
        sr_index_wakeup = sr_index_wakeup % SR_ARR_LEN;
    }
    else
    {
        HISI_HI6421V300_COUL_ERR("[SR]%s(%d): wakeup_time=%d, di->sr_suspend_time=%d, di->sr_resume_time=%d\n",
            __func__, __LINE__, wakeup_time, di->sr_suspend_time, di->sr_resume_time);
    }

    sr_cur_state = SR_DEVICE_SLEEP;
    DI_UNLOCK();
    HISI_HI6421V300_COUL_INF("SUSPEND! cc=%d, time=%d\n", di->suspend_cc,
                       di->suspend_time);
    cancel_delayed_work(&di->read_temperature_delayed_work);
    if (di->batt_exist)
    {
        cancel_delayed_work(&di->calculate_soc_delayed_work);
    }
    cali_adc();
    set_low_vol_int_reg(di, LOW_INT_STATE_SLEEP);
    printk("%s-.\n",__func__);

    return 0;
}

/* calculate last SR_TOTAL_TIME seconds duty ratio */
static int sr_get_duty_ratio(void) {
    int total_sleep_time = 0;
    int total_wakeup_time = 0;
    int last_sleep_idx  = (sr_index_sleep - 1 < 0) ? SR_ARR_LEN - 1 : sr_index_sleep - 1;
    int last_wakeup_idx = (sr_index_wakeup - 1 < 0) ? SR_ARR_LEN - 1 : sr_index_wakeup - 1;
    int cnt = 0;
    int duty_ratio = 0;

    do
    {
        total_sleep_time += sr_time_sleep[last_sleep_idx];
        total_wakeup_time += sr_time_wakeup[last_wakeup_idx];

        last_sleep_idx = (last_sleep_idx - 1 < 0) ? SR_ARR_LEN - 1 : last_sleep_idx - 1;
        last_wakeup_idx = (last_wakeup_idx - 1 < 0) ? SR_ARR_LEN - 1 : last_wakeup_idx - 1;

        cnt++;
        if (cnt >= SR_ARR_LEN)
        {
            break;
        }
    } while (total_sleep_time + total_wakeup_time < SR_TOTAL_TIME);

    /* calculate duty ratio */
    if (total_sleep_time + total_wakeup_time >= SR_TOTAL_TIME)
    {
        duty_ratio = total_sleep_time * 100 / (total_sleep_time + total_wakeup_time);
        HISI_HI6421V300_COUL_INF("[SR]%s(%d): total_wakeup=%ds, total_sleep=%ds, duty_ratio=%d\n",
            __func__, __LINE__, total_wakeup_time, total_sleep_time, duty_ratio);
    }

    return duty_ratio;
}

static int sr_need_update_ocv(struct hisi_hi6421v300_coul_device *di)
{
    int last_wakeup_time = 0;
    int last_sleep_time = 0;
    int duty_ratio = 0;

    /* get last wakeup time */
    if (sr_index_wakeup >= 0 && sr_index_wakeup < SR_ARR_LEN)
    {
        last_wakeup_time = (sr_index_wakeup - 1 < 0) ? sr_time_wakeup[SR_ARR_LEN - 1]: sr_time_wakeup[sr_index_wakeup - 1];
    }

    /* get last sleep time */
    if (sr_index_sleep >= 0 && sr_index_sleep < SR_ARR_LEN)
    {
        last_sleep_time = (sr_index_sleep - 1 < 0) ? sr_time_sleep[SR_ARR_LEN - 1]: sr_time_sleep[sr_index_sleep - 1];
    }

    /* get last SR_TOTAL_TIME seconds duty ratio */
    duty_ratio = sr_get_duty_ratio();

    HISI_HI6421V300_COUL_INF("[SR]%s(%d): need_update, last_sleep=%ds, last_wakeup=%ds, duty_ratio=%d\n",
            __func__, __LINE__, last_sleep_time, last_wakeup_time, duty_ratio);
    /* judge if need update ocv */
    if (last_sleep_time > SR_DELTA_SLEEP_TIME &&
        last_wakeup_time < SR_DELTA_WAKEUP_TIME &&
        duty_ratio > SR_DUTY_RATIO )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int ocv_could_update(struct hisi_hi6421v300_coul_device *di)
{
    int sleep_cc;
    int sleep_current = 0;
    int sleep_time, time_now;

    sleep_cc = calculate_cc_uah();
    sleep_cc = sleep_cc - di->suspend_cc;
    time_now = get_coul_time();
    sleep_time = time_now - di->suspend_time;

    if (sleep_time < delta_sleep_time && !sr_need_update_ocv(di))
    {
        HISI_HI6421V300_COUL_INF("[SR]Can't update ocv, sleep_current=%d ma, sleep_time=%d s\n", sleep_current, sleep_time);
        return 0;
    }

    /* ma = ua/1000 = uas/s/1000 = uah*3600/s/1000 = uah*18/(s*5) */
    if (sleep_time > 0)
    {
        sleep_current = (sleep_cc * 18) / (sleep_time * 5);
        if (sleep_current > delta_sleep_current)
        {
            HISI_HI6421V300_COUL_INF("[SR]Can't update ocv, sleep_current=%d ma, sleep_time=%d s\n", sleep_current, sleep_time);
            return 0;
        }
    }
    else
    {
        HISI_HI6421V300_COUL_ERR("[SR]%s(%d): sleep_time = %d\n",  __func__, __LINE__, sleep_time);
        return 0;
    }

    HISI_HI6421V300_COUL_INF("[SR]going to update ocv, sleep_time=%ds, sleep_current=%d ma\n", sleep_time, sleep_current);
    return 1;
}

static int hisi_hi6421v300_coul_resume(struct platform_device *pdev)
{
    struct hisi_hi6421v300_coul_device *di = platform_get_drvdata(pdev);
    int current_sec = get_coul_time();
    int sr_sleep_time = current_sec - di->sr_suspend_time;

    printk("%s+.\n",__func__);
    if (NULL == di)
    {
        HISI_HI6421V300_COUL_ERR("[coul_resume]:di is NULL\n");
        return 0;
    }
    update_battery_temperature(di, TEMPERATURE_INIT_STATUS);
    disable_temperature_debounce = 0;
    set_low_vol_int_reg(di, LOW_INT_STATE_RUNNING);
    DI_LOCK();
    sr_cur_state = SR_DEVICE_WAKEUP;
    di->sr_resume_time = current_sec;
    if (sr_sleep_time >= 0)
    {
        sr_time_sleep[sr_index_sleep] = sr_sleep_time;
        sr_index_sleep++;
        sr_index_sleep = sr_index_sleep % SR_ARR_LEN;
    }
    else
    {
        HISI_HI6421V300_COUL_ERR("[SR]%s(%d): sr_sleep_time = %d\n", __func__, __LINE__, sr_sleep_time);
    }

    if (ocv_could_update(di)
        && (current_sec - di->charging_stop_time > 30*60))
    {
        get_ocv_by_vol(di);
    }
    else if(di->batt_delta_rc > di->batt_data->fcc*5*10
        && di->charging_state != CHARGING_STATE_CHARGE_START
        && (current_sec - di->charging_stop_time > 30*60))
    {
        int old_ocv = di->batt_ocv;
        HISI_HI6421V300_COUL_EVT("Update ocv for delta_rc(%d)!\n", di->batt_delta_rc);
        get_ocv_by_vol(di);
        if (old_ocv != di->batt_ocv)
        {
            short ocvreg = convert_uv2regval(di->batt_ocv);
            ocvreg = ocvreg | INVALID_TO_UPDATE_FCC;
            udelay(100);
            HISI_HI6421V300_REGS_WRITE(HISI_HI6421V300_SAVE_OCV_ADDR,&ocvreg,2);
            di->batt_ocv_valid_to_refresh_fcc = 0;
        }
    }

    di->soc_limit_flag = 2;
    di->batt_soc = calculate_state_of_charge(di);
    di->soc_limit_flag = 1;

    DI_UNLOCK();

    if (di->batt_exist)
    {
        schedule_delayed_work(&di->read_temperature_delayed_work, round_jiffies_relative(msecs_to_jiffies(READ_TEMPERATURE_MS)));
        schedule_delayed_work(&di->calculate_soc_delayed_work,round_jiffies_relative(msecs_to_jiffies(CALCULATE_SOC_MS/2)));
    }
    printk("%s-.\n",__func__);

    return 0;
}
#endif
static struct of_device_id hisi_hi6421v300_coul_match_table[] =
{
    {
    .compatible = "hisilicon,hi6421-coul",
    .data = NULL,
    },
    {
    },
};
static struct platform_driver hisi_hi6421v300_coul_driver = {
    .probe = hisi_hi6421v300_coul_probe,
    .remove = hisi_hi6421v300_coul_remove,
    .shutdown = hisi_hi6421v300_coul_shutdown,
#ifdef CONFIG_PM
    .suspend = hisi_hi6421v300_coul_suspend,
    .resume = hisi_hi6421v300_coul_resume,
#endif
    .driver = {
        .name = "hisi_hi6421v300_coul",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(hisi_hi6421v300_coul_match_table),
    },
};

int __init hisi_hi6421v300_coul_init(void)
{
    return (platform_driver_register(&hisi_hi6421v300_coul_driver));
}

void __exit hisi_hi6421v300_coul_exit(void)
{
    platform_driver_unregister(&hisi_hi6421v300_coul_driver);
}


fs_initcall_sync(hisi_hi6421v300_coul_init);
module_exit(hisi_hi6421v300_coul_exit);

MODULE_AUTHOR("HUAWEI");
MODULE_DESCRIPTION("hisi coul module driver");
MODULE_LICENSE("GPL");
