/*****************************************************************************************
* Filename:	hisi_smartstar_coul.c
*
* Discription:  smartstar coulometer functions.
* Copyright: 	(C) 2013 huawei.
*
* revision history:
* 03/25/13 yuanqinshun -v1.0
*
******************************************************************************************/

#include "hi6xxx_smartstar_v300.h"

#ifdef _DRV_LLT_
#define static
#define __init
#define __devexit
#define inline
#endif

#define HWLOG_TAG smartstar
HWLOG_REGIST();

extern struct blocking_notifier_head notifier_list;
extern int hisi_coul_ops_register (struct hisi_coul_ops *coul_ops,
                               enum HISI_COULOMETER_TYPE coul_type);
static struct smartstar_coul_device *g_smartstar_coul_dev = NULL;
static struct ss_coul_nv_info my_nv_info;
static unsigned long nv_info_addr = 0;
static char g_coul_work_mode;
static u32 is_board_type = 0;
static u32 battery_is_removable = 0;
static u32 s_smartstar_version = 0;
static u32 s_dcxo_tpye = DCXO_600;
static int vol_offset_table[3][256]={
{/*hi6552*/
1002000, 1002100, 1002200, 1002300, 1002400, 1002500, 1002600, 1002700, 1002800,
1002900, 1003000, 1003100, 1003200, 1003300, 1003400, 1003500, 1003600, 1003700,
1003800, 1003900, 1004000, 1004100, 1004200, 1004300, 1004400, 1004500, 1004600,
1004700, 1004800, 1004900, 1005000, 1005100, 1005200, 1005300, 1005400, 1005500,
1005600, 1005700, 1005800, 1005900, 1006000, 1006100, 1006200, 1006300, 1006400,
1006500, 1006600, 1006700, 1006800, 1006900, 1007000, 1007100, 1007200, 1007300,
1007400, 1007500, 1007600, 1007700, 1007800, 1007900, 1008000, 1008100, 1008200,
1008300, 1008400, 1008500, 1008600, 1008700, 1008800, 1008900, 1009000, 1009100,
1009200, 1009300, 1009400, 1009500, 1009600, 1009700, 1009800, 1009900, 1010000,
1010100, 1010200, 1010300, 1010400, 1010500, 1010600, 1010700, 1010800, 1010900,
1011000, 1011100, 1011200, 1011300, 1011400, 1011500, 1011600, 1011700, 1011800,
1011900, 1012000, 1012100, 1012200, 1012300, 1012400, 1012500, 1012600, 1012700,
1012800, 1012900, 1013000, 1013100, 1013200, 1013300, 1013400, 1013500, 1013600,
1013700, 1013800, 1013900, 1014000, 1014100, 1014200, 1014300, 1014400, 1014500,
1014600, 1014700, 1014800, 1014900, 1015000, 1015100, 1015200, 1015300, 1015400,
1015500, 1015600, 1015700, 1015800, 1015900, 1016000, 1016100, 1016200, 1016300,
1016400, 1016500, 1016600, 1016700, 1016800, 1016900, 1017000, 1017100, 1017200,
1017300, 1017400, 1017500, 1017600, 1017700, 1017800, 1017900, 1018000, 1018100,
1018200, 1018300, 1018400, 1018500, 1018600, 1018700, 1018800, 1018900, 1019000,
1019100, 1019200, 1019300, 1019400, 1019500, 1019600, 1019700, 1019800, 1019900,
1020000, 1020100, 1020200, 1020300, 1020400, 1020500, 1020600, 1020700, 1020800,
1020900, 1021000, 1021100, 1021200, 1021300, 1021400, 1021500, 1021600, 1021700,
1021800, 1021900, 1022000, 1022100, 1022200, 1022300, 1022400, 1022500, 1022600,
1022700, 1022800, 1022900, 1023000, 1023100, 1023200, 1023300, 1023400, 1023500,
1023600, 1023700, 1023800, 1023900, 1024000, 1024100, 1024200, 1024300, 1024400,
1024500, 1024600, 1024700, 1024800, 1024900, 1025000, 1025100, 1025200, 1025300,
1025400, 1025500, 1025600, 1025700, 1025800, 1025900, 1026000, 1026100, 1026200,
1026300, 1026400, 1026500, 1026600, 1026700, 1026800, 1026900, 1027000, 1027100,
1027200, 1027300, 1027400, 1027500},

{/*hi6553:0x10*/
990000,   990100,  990200,  990300,  990400,  990500,  990600,  990700,  990800,
990900,   991000,  991100,  991200,  991300,  991400,  991500,  991600,  991700,
991800,   991900,  992000,  992100,  992200,  992300,  992400,  992500,  992600,
992700,   992800,  992900,  993000,  993100,  993200,  993300,  993400,  993500,
993600,   993700,  993800,  993900,  994000,  994100,  994200,  994300,  994400,
994500,   994600,  994700,  994800,  994900,  995000,  995100,  995200,  995300,
995400,   995500,  995600,  995700,  995800,  995900,  996000,  996100,  996200,
996300,   996400,  996500,  996600,  996700,  996800,  996900,  997000,  997100,
997200,   997300,  997400,  997500,  997600,  997700,  997800,  997900,  998000,
998100,   998200,  998300,  998400,  998500,  998600,  998700,  998800,  998900,
999000,   999100,  999200,  999300,  999400,  999500,  999600,  999700,  999800,
999900,  1000000, 1000100, 1000200, 1000300, 1000400, 1000500, 1000600, 1000700,
1000800, 1000900, 1001000, 1001100, 1001200, 1001300, 1001400, 1001500, 1001600,
1001700, 1001800, 1001900, 1002000, 1002100, 1002200, 1002300, 1002400, 1002500,
1002600, 1002700, 1002800, 1002900, 1003000, 1003100, 1003200, 1003300, 1003400,
1003500, 1003600, 1003700, 1003800, 1003900, 1004000, 1004100, 1004200, 1004300,
1004400, 1004500, 1004600, 1004700, 1004800, 1004900, 1005000, 1005100, 1005200,
1005300, 1005400, 1005500, 1005600, 1005700, 1005800, 1005900, 1006000, 1006100,
1006200, 1006300, 1006400, 1006500, 1006600, 1006700, 1006800, 1006900, 1007000,
1007100, 1007200, 1007300, 1007400, 1007500, 1007600, 1007700, 1007800, 1007900,
1008000, 1008100, 1008200, 1008300, 1008400, 1008500, 1008600, 1008700, 1008800,
1008900, 1009000, 1009100, 1009200, 1009300, 1009400, 1009500, 1009600, 1009700,
1009800, 1009900, 1010000, 1010100, 1010200, 1010300, 1010400, 1010500, 1010600,
1010700, 1010800, 1010900, 1011000, 1011100, 1011200, 1011300, 1011400, 1011500,
1011600, 1011700, 1011800, 1011900, 1012000, 1012100, 1012200, 1012300, 1012400,
1012500, 1012600, 1012700, 1012800, 1012900, 1013000, 1013100, 1013200, 1013300,
1013400, 1013500, 1013600, 1013700, 1013800, 1013900, 1014000, 1014100, 1014200,
1014300, 1014400, 1014500, 1014600, 1014700, 1014800, 1014900, 1015000, 1015100,
1015200, 1015300, 1015400, 1015500},

{/*hi6553>=0x11*/
984500,  984600,  984700,  984800,  984900,  985000,  985100,  985200,  985300,
985400,  985500,  985600,  985700,  985800,  985900,  986000,  986100,  986200,
986300,  986400,  986500,  986600,  986700,  986800,  986900,  987000,  987100,
987200,  987300,  987400,  987500,  987600,  987700,  987800,  987900,  988000,
988100,  988200,  988300,  988400,  988500,  988600,  988700,  988800,  988900,
989000,  989100,  989200,  989300,  989400,  989500,  989600,  989700,  989800,
989900,  990000,  990100,  990200,  990300,  990400,  990500,  990600,  990700,
990800,  990900,  991000,  991100,  991200,  991300,  991400,  991500,  991600,
991700,  991800,  991900,  992000,  992100,  992200,  992300,  992400,  992500,
992600,  992700,  992800,  992900,  993000,  993100,  993200,  993300,  993400,
993500,  993600,  993700,  993800,  993900,  994000,  994100,  994200,  994300,
994400,  994500,  994600,  994700,  994800,  994900,  995000,  995100,  995200,
995300,  995400,  995500,  995600,  995700,  995800,  995900,  996000,  996100,
996200,  996300,  996400,  996500,  996600,  996700,  996800,  996900,  997000,
997100,  997200,  997300,  997400,  997500,  997600,  997700,  997800,  997900,
998000,  998100,  998200,  998300,  998400,  998500,  998600,  998700,  998800,
998900,  999000,  999100,  999200,  999300,  999400,  999500,  999600,  999700,
999800,  999900,  1000000, 1000100, 1000200, 1000300, 1000400, 1000500, 1000600,
1000700, 1000800, 1000900, 1001000, 1001100, 1001200, 1001300, 1001400, 1001500,
1001600, 1001700, 1001800, 1001900, 1002000, 1002100, 1002200, 1002300, 1002400,
1002500, 1002600, 1002700, 1002800, 1002900, 1003000, 1003100, 1003200, 1003300,
1003400, 1003500, 1003600, 1003700, 1003800, 1003900, 1004000, 1004100, 1004200,
1004300, 1004400, 1004500, 1004600, 1004700, 1004800, 1004900, 1005000, 1005100,
1005200, 1005300, 1005400, 1005500, 1005600, 1005700, 1005800, 1005900, 1006000,
1006100, 1006200, 1006300, 1006400, 1006500, 1006600, 1006700, 1006800, 1006900,
1007000, 1007100, 1007200, 1007300, 1007400, 1007500, 1007600, 1007700, 1007800,
1007900, 1008000, 1008100, 1008200, 1008300, 1008400, 1008500, 1008600, 1008700,
1008800, 1008900, 1009000, 1009100, 1009200, 1009300, 1009400, 1009500, 1009600,
1009700, 1009800, 1009900, 1010000}
};

static int pl_v_offset_a = 0;//product line v_offset_a
static int pl_v_offset_b = 0;
static int pl_c_offset_a = 0;
static int pl_c_offset_b = 0;

static int g_eco_leak_uah = 0;
static int saved_vol_fifo0_out_eco = 0;
static int saved_vol_fifo0_in_eco = 0;
static unsigned int last_eco_in = 0;
static unsigned int last_eco_out = 0;
static unsigned int is_cali_flag = 0;
static struct platform_driver hisi_smartstar_coul_driver;
//static struct wake_lock hisi_smartstar_wakelock;
static int smartstar_read_avg_reg (unsigned int reg_addr,int len);
static int smartstar_battery_voltage_uv(void);
extern int hisi_nve_direct_access(struct hisi_nve_info_user *user_info);
extern struct class *hw_power_get_class(void);
int delta_sleep_time = 10*60; // sleep time bigger could update ocv, in s
int delta_sleep_current = 50; // sleep current less could updat eocv, in mA
//int delta_ocv_update_time = 2*60; // min time between ocv update, in s

#ifndef _DRV_LLT_
module_param(delta_sleep_time, int, 0644);
//module_param(delta_ocv_update_time, int, 0644);
module_param(delta_sleep_current, int, 0644);

static int batt_id_voltage; // in mV

static int ro_ops_set(const char *buffer, const struct kernel_param *kp)
{
    return -EINVAL;
}

static int batt_id_voltage_ops_get(char *buffer, const struct kernel_param *kp)
{
    // TODO: HKADC batt_id_voltage

    sprintf(buffer, "%d mV", batt_id_voltage);
	return strlen(buffer);
}
static struct kernel_param_ops batt_id_voltage_ops={
    .set = ro_ops_set,
    .get = batt_id_voltage_ops_get,
};

module_param_cb(batt_id_voltage, &batt_id_voltage_ops,&batt_id_voltage, 0444);
static int batt_voltage_uv;			// in uv
static int batt_voltage_uv_ops_get(char *buffer, const struct kernel_param *kp)
{
    int vol = smartstar_battery_voltage_uv();
	sprintf(buffer, "%d", vol);
	return strlen(buffer);
}

static struct kernel_param_ops batt_voltage_uv_ops = {
	.set = ro_ops_set,
	.get = batt_voltage_uv_ops_get,
};

module_param_cb(batt_voltage_uv, &batt_voltage_uv_ops, &batt_voltage_uv, 0444);
static int convert_regval2uv(short reg_val);
static int batt_init_ocv;		// in uv
static int batt_init_ocv_ops_get(char *buffer, const struct kernel_param *kp)
{
	short ocvreg = 0;
    short volreg_offset = 0;
	int ocv = 0;
	/* read ocv and offset */
	SMARTSTAR_REGS_READ(SMARTSTAR_OCV_VOLTAGE_BASE, &ocvreg, 2);
	SMARTSTAR_REGS_READ(SMARTSTAR_OFFSET_VOLTAGE, &volreg_offset, 2);

	ocv = convert_regval2uv(ocvreg - volreg_offset);

	sprintf(buffer, "%d uv", ocv);
	return strlen(buffer);
}

static struct kernel_param_ops batt_init_ocv_ops = {
	.set = ro_ops_set,
	.get = batt_init_ocv_ops_get,
};

module_param_cb(batt_init_ocv, &batt_init_ocv_ops, &batt_init_ocv, 0444);
static struct platform_device *g_pdev = NULL;
static int hisi_smartstar_coul_suspend(struct platform_device *pdev,
									   pm_message_t state);
static int hisi_smartstar_coul_resume(struct platform_device *pdev);
static int coul_running = 1;	// 1 is running, 0 is suspend
static int coul_state_ops_set(const char *buffer,
							  const struct kernel_param *kp)
{
/*	struct smartstar_coul_device *di = g_smartstar_coul_dev;  --no use  */
	int run = 0;
	run = buffer[0] - '0';
	if (run && !coul_running)
	{
		coul_running = 1;
		hisi_smartstar_coul_resume(g_pdev);
	}
	else if (!run && coul_running)
	{
		pm_message_t pm = { 0 };
		coul_running = 0;
		hisi_smartstar_coul_suspend(g_pdev, pm);
	}
	return 0;
}

static int coul_state_ops_get(char *buffer, const struct kernel_param *kp)
{
	sprintf(buffer, "%d", coul_running);
	return strlen(buffer);
}

static struct kernel_param_ops coul_state_ops = {
	.set = coul_state_ops_set,
	.get = coul_state_ops_get,
};

module_param_cb(coul_running, &coul_state_ops, &coul_running, 0644);
static int batt_soc_with_uuc;	// in uv
static int batt_soc_with_uuc_ops_get(char *buffer,
										const struct kernel_param *kp)
{
	struct smartstar_coul_device *di = g_smartstar_coul_dev;
	sprintf(buffer, "%d", di->batt_soc_with_uuc);
	return strlen(buffer);
}

static struct kernel_param_ops batt_soc_with_uuc_ops = {
	.set = ro_ops_set,
	.get = batt_soc_with_uuc_ops_get,
};

module_param_cb(batt_soc_with_uuc, &batt_soc_with_uuc_ops,
				&batt_soc_with_uuc, 0444);
#endif

static int calculate_cc_uah(void);
static int calculate_eco_leak_uah( void );
static void calculate_soc_params(struct smartstar_coul_device *di,
						int *fcc_uah,
						int *unusable_charge_uah,
						int *remaining_charge_uah,
						int *cc_uah,
						int *delta_rc_uah,
						int *rbatt);


/*******************************************************
  Function:      get_offset
  Description:  get voltage offset_a
  Input:           NULL
  Output:         NULL
  Return:         NULL
********************************************************/
static int get_vol_offset(unsigned char regval)
{
	if ((s_smartstar_version >= HI6553_COUL_VERSION_MIN) && (s_smartstar_version <= HI6553_COUL_VERSION_MAX))
	{
		if ( HI6553_COUL_VERSION_MIN == s_smartstar_version){
			return vol_offset_table[1][regval];/*HI6553:0x10*/
		} else {
			return vol_offset_table[2][regval];/*HI6553 >=0x11*/
		}
	}
	else
	{
		return vol_offset_table[0][regval];  //uV /*HI6552*/
	}

}
/**********************************************************
*  Function:	is_nv_cali_param_valid()
*  Discription:
*               judge whether cali param is valid
*  Parameters:
*               NULL
*  return value:
*               0 means invalid
*               1 means valid
**********************************************************/
static int is_nv_cali_param_valid(void)
{
    if (0 == v_offset_a)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
/**********************************************************
*  Function:	get_otp_calibration_params()
*  Discription:
*               get voltage calibration params a and b
*  Parameters:
*               NULL
*  return value:
*               NULL
**********************************************************/
static void get_otp_calibration_params(void)
{
        unsigned char v_offset_regval;
        v_offset_regval = SMARTSTAR_REG_READ(SMARTSTAR_VOL_OFFSET_ADDR);
        v_offset_a = get_vol_offset(v_offset_regval)==0? DEFAULT_V_OFF_A:get_vol_offset(v_offset_regval);
        v_offset_b = 0;
        return;
}

/**********************************************************
*  Function:	get_nv_calibration_params()
*  Discription:
*               get nv voltage calibration params a and b
*  Parameters:
*               struct smartstar_coul_device *di
*  return value:
*               0 means get nv cali params failed;
*               1 means get nv cali params success;
**********************************************************/
static int  get_nv_calibration_params(struct smartstar_coul_device *di)
{
    struct ss_coul_nv_info *pinfo = &di->nv_info;

    v_offset_a = pinfo->v_offset_a;
    v_offset_b = pinfo->v_offset_b;
    if(is_nv_cali_param_valid())
    {
        return 1;
    }
    hwlog_info("nv calibration param is invalid\n");
    return 0;
}

/**********************************************************
*  Function:	get_calibration_params()
*  Discription:
*               get voltage calibration params a and b
*  Parameters:
*               NULL
*  return value:
*               NULL
**********************************************************/
static void get_calibration_params(struct smartstar_coul_device *di)
{
    if (get_nv_calibration_params(di))
    {
        hwlog_info("use nv calibration params \n");
    }
    else
    {
        hwlog_info("use OTP calibration params \n");
        get_otp_calibration_params();
    }
}

/*******************************************************
  Function:      clear_cc_register
  Description:  clear coulomb uah record
  Input:           NULL
  Output:         NULL
  Return:         NULL
********************************************************/
static void clear_cc_register(void)
{
    int ccregval = 0;

    hisi_saved_abs_cc_mah += (calculate_cc_uah() / 1000);
    udelay(110); /* when operator the same register of coul must delay 35us,the CMOS chip BUG */
    SMARTSTAR_REGS_WRITE(SMARTSTAR_CL_IN_BASE,&ccregval,4);
    SMARTSTAR_REGS_WRITE(SMARTSTAR_CL_OUT_BASE,&ccregval,4);
}

/*******************************************************
  Function:      convert_regval2uah
  Description:  convert register value to uah
  Input:           NULL
  Output:         NULL
  Return:         value of register in uah
  Remark:       1bit = 1bit current * 0.11 c = 5/10661 * 11/100 c = 5/10661 * 11/100 * 1000/3600 mAh
                     = 11 / (10661*2*36) mAh = 11 * 1000/ (10661 *2 *36) uAh
                     = 11 * 125/ (10661* 9) uAh
********************************************************/
static int convert_regval2uah(unsigned int reg_val)
{
    int ret;
    s64 temp;
    temp = reg_val;

    if( (g_coul_work_mode & PMU_WORK_MODE_MSK) == TCXO_MODE )
    {
        temp = temp * BIT_FOR_UAH_TCXO;
    }
    else
    {
    	if (DCXO_586 == s_dcxo_tpye){
			temp = temp * BIT_FOR_UAH_DCXO_586;
		}
		else{
			temp = temp * BIT_FOR_UAH_DCXO_600;
		}

    }
    temp = (s64)div_s64(temp, 10000000);

    temp = (s64) c_offset_a * temp;
    ret = div_s64(temp, 1000000);

    return ret;
}
static int convert_uah2regval(unsigned int reg_val)
{
    int ret = 0;
    s64 temp;

    temp = reg_val;

    temp = temp * 1000000;
    temp = div_s64(temp, c_offset_a);

    temp = temp * 10000000;
    if( (g_coul_work_mode & PMU_WORK_MODE_MSK) == TCXO_MODE  )
    {
        temp =div_s64( temp, BIT_FOR_UAH_TCXO);
    }
    else
    {
    	if (DCXO_586 == s_dcxo_tpye){
			temp = div_s64(temp, BIT_FOR_UAH_DCXO_586);
		}
		else{
			temp = div_s64(temp, BIT_FOR_UAH_DCXO_600);
		}
    }

    ret = (int)temp;
    return ret;
}

#if 1 /*debug*/
static unsigned int g_cc_in = 0;
static unsigned int g_cc_out = 0;
void hisi_smartstar_print_cc_reg(int c)
{
    static int cnt = 0;
    if (c==0 && cnt>0)
        cnt --;
    else
        cnt = c;

    if (cnt)
        hwlog_err("reg: cc_in = 0x%x, cc_out = 0x%x\n", g_cc_in, g_cc_out);
}
#endif

void coul_reg_show(void)
{
    int reg0=0;
    int reg1=0;
    int reg2=0;
    int reg3=0;
    int reg4=0;
    int reg5=0;
    int reg6=0;
    int reg7=0;

    udelay(50);
    SMARTSTAR_REGS_READ(0x1af,&reg0,1);
    SMARTSTAR_REGS_READ(0x179,&reg1,2);
    SMARTSTAR_REGS_READ(0x17b,&reg2,2);
    SMARTSTAR_REGS_READ(0x16b,&reg3,2);
    SMARTSTAR_REGS_READ(0x151,&reg4,1);
    SMARTSTAR_REGS_READ(0x152,&reg5,1);
    SMARTSTAR_REGS_READ(0x14f,&reg6,2);
    SMARTSTAR_REGS_READ(0x1a3,&reg7,2);

    hwlog_err("0x1af(state) = 0x%x, 0x179-0x17a(fifo0) = 0x%x, 0x17b-0x17c(fifo1) = 0x%x\n"
              "0x16b-0x16c(offset) = 0x%x,0x151(ctrl)=0x%x,0x152(reflash)=0x%x\n"
              "0x14f(cur vol)=0x%x, 0x1a3-0x1a4(offset vol mod)=0x%x\n",
               reg0,reg1,reg2,reg3,reg4,reg5,reg6,reg7);

}
/*******************************************************
  Function:      smartstar_read_avg_reg
  Description:  get average reg value in 3 samples,if 2 samples equal with each other,
                        return it,else return the first sample avg_tmp[0]
  Input:           unsigned int reg_addr, int len
  Output:         NULL
  Return:         average reg value
********************************************************/
static int smartstar_read_avg_reg (unsigned int reg_addr, int len)
{
    int i;
    int avg_tmp[3] = {0};

    if (len > 4) {
        len = 4;
        hwlog_err("read avg reg length error\n");
    }

/* read reg_addr for 3 times and calculate the average */
    for (i = 0; i < 3; i++) {
        SMARTSTAR_REGS_READ(reg_addr,(avg_tmp + i),len);
    }
        if (avg_tmp[0] == avg_tmp[1] ){
            return avg_tmp[0];
            }else if(avg_tmp[1] == avg_tmp[2] ){
                        return avg_tmp[1];
            }else{
            return avg_tmp[0];
            }
}

 /*******************************************************
  Function:      calculate_cc_uah
  Description:  value of out_uah - in_uah recorded by  coulomb
  Input:           NULL
  Output:         NULL
  Return:         value of  uah through coulomb
  Remark:       adjusted by offset integrated on time
********************************************************/
static int calculate_cc_uah(void)
{
    unsigned int cc_in = 0;
    unsigned int cc_out = 0;
    unsigned int cl_in_time, cl_out_time;
    int cc_uah_in = 0;
    int cc_uah_out = 0;
    int cc_uah = 0;
    cc_in = (unsigned int)smartstar_read_avg_reg(SMARTSTAR_CL_IN_BASE, 4);
    cc_out = (unsigned int)smartstar_read_avg_reg(SMARTSTAR_CL_OUT_BASE, 4);
#if 1 /*debug*/
    g_cc_in = cc_in;
    g_cc_out = cc_out;
    hisi_smartstar_print_cc_reg(0);
#endif
    cc_uah_out = convert_regval2uah(cc_out);
    cc_uah_in = convert_regval2uah(cc_in);

    SMARTSTAR_REGS_READ(SMARTSTAR_CHG_TIMER_BASE,&cl_in_time,4);
    SMARTSTAR_REGS_READ(SMARTSTAR_LOAD_TIMER_BASE,&cl_out_time,4);
    /* uah = uas/3600 = ua*s/3600 */
    cc_uah_in = cc_uah_in - div_s64((s64) cl_in_time * c_offset_b, 3600);
    cc_uah_out = cc_uah_out + div_s64((s64) cl_out_time * c_offset_b, 3600);
    cc_uah = cc_uah_out - cc_uah_in;

    hwlog_info("ccout_reg=0x%x,ccin_reg = 0x%x, cl_out_time=%d, cl_in_time=%d, cc_adjust=%d uah\n",
		   (unsigned int) cc_out, (unsigned int)cc_in, cl_out_time, cl_in_time, cc_uah);

    return cc_uah;
}


 /*******************************************************
  Function:      get_coul_time
  Description:  get coulomb in and out time
  Input:           NULL
  Output:         NULL
  Return:         sum of in and out time
********************************************************/
static unsigned int get_coul_time(void)
{
    unsigned int cl_in_time = 0;
    unsigned int cl_out_time = 0;

    SMARTSTAR_REGS_READ(SMARTSTAR_CHG_TIMER_BASE,&cl_in_time,4);
    SMARTSTAR_REGS_READ(SMARTSTAR_LOAD_TIMER_BASE,&cl_out_time,4);

    return (cl_in_time + cl_out_time);
}


  /*******************************************************
  Function:      clear_sr_time_array
  Description:  clear sleep and wakeup global variable
  Input:           NULL
  Output:         NULL
  Return:         NULL
********************************************************/
static void clear_sr_time_array(void)
{
    memset(&sr_time_sleep, 0, sizeof(sr_time_sleep));
    memset(&sr_time_wakeup, 0, sizeof(sr_time_wakeup));
    sr_index_sleep = 0;
    sr_index_wakeup = 0;
}


 /*******************************************************
  Function:      clear_coul_time
  Description:  clear sleep/wakeup/cl_in/cl_out time
  Input:           NULL
  Output:         NULL
  Return:         NULL
********************************************************/
static void clear_coul_time(void)
{
    unsigned int cl_time = 0;
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    unsigned int time_now = get_coul_time();

    if (di) {
        if (SR_DEVICE_WAKEUP == sr_cur_state) {
            di->sr_resume_time -= time_now;
            if (di->sr_resume_time > 0) {
                hwlog_err("[SR]%s(%d): di->sr_resume_time = %d\n", __func__, __LINE__, di->sr_resume_time);
                di->sr_resume_time = 0;
            }
            di->sr_suspend_time = 0;
        }
        else {
            di->sr_resume_time = 0;
            di->sr_suspend_time = 0;
            hwlog_err("[SR]%s(%d): sr_cur_state = %d\n", __func__, __LINE__, sr_cur_state);
        }
        di->charging_stop_time -= time_now;
    }
    else {
        hwlog_err("[SR]%s(%d): di is NULL\n", __func__, __LINE__);
    }

    udelay(110); /* when operator the same register of coul must delay 35us,the CMOS chip BUG */
    SMARTSTAR_REGS_WRITE(SMARTSTAR_CHG_TIMER_BASE,&cl_time ,4);
    SMARTSTAR_REGS_WRITE(SMARTSTAR_LOAD_TIMER_BASE,&cl_time ,4);

    clear_sr_time_array();
}

static void save_cc_uah(long cc_uah)
{
    unsigned long reg;
	if (cc_uah > 0){
        reg = convert_uah2regval(cc_uah);
        udelay(110); /* when operator the same register of coul must delay 35us,the CMOS chip BUG */
        SMARTSTAR_REGS_WRITE(SMARTSTAR_CL_OUT_BASE, &reg, 4);
        reg = 0;
        SMARTSTAR_REGS_WRITE(SMARTSTAR_CL_IN_BASE, &reg, 4);
    }
    else {
        reg = convert_uah2regval(-cc_uah);
        udelay(110); /* when operator the same register of coul must delay 35us,the CMOS chip BUG */
        SMARTSTAR_REGS_WRITE(SMARTSTAR_CL_IN_BASE, &reg, 4);
        reg = 0;
        SMARTSTAR_REGS_WRITE(SMARTSTAR_CL_OUT_BASE, &reg, 4);

    }
	clear_coul_time();
}

  /*******************************************************
  Function:      convert_regval2uv
  Description:  convert register value to uah
  Input:           NULL
  Output:         NULL
  Return:         value of register in uV
  Remark:       1 bit = 225.1196 uv = 4.8/21322 V = 4.8/21322 * 1000 * 1000 uV = 24 * 1000 * 100/ 10661 uV
********************************************************/
 /* V8R1 V9R1 have the same uv ---- xuxinhu */
static  int convert_regval2uv(short reg_val)
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


/*******************************************************
  Function:      convert_uv2regval
  Description:  convert uah value to Bit for register
  Input:           NULL
  Output:         NULL
  Return:         value of register in uV
  Remark:       1 bit = 225.1196 uv = 4.8/21322 V = 4.8/21322 * 1000 * 1000 uV =10661 /  24 * 1000 * 100 uV
                   V8R1 V9R1 have the same uv ---- xuxinhu
********************************************************/
static  short convert_uv2regval(int uv_val)
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

 /*******************************************************
  Function:      smartstar_battery_voltage_uv
  Description:  get battery voltage in uV
  Input:           NULL
  Output:         NULL
  Return:         battery voltage in uV
********************************************************/
int smartstar_battery_voltage_uv(void)
{
    short regval;
    regval = (short)smartstar_read_avg_reg(SMARTSTAR_V_OUT, 2);
    return(convert_regval2uv(regval));
}

 /*******************************************************
  Function:      convert_regval2uah
  Description:  convert register value to uah
  Input:           NULL
  Output:         NULL
  Return:         value of register in uA
  Remark:       10 mohm resistance: 1 bit = 5/10661 A = 5*1000*1000 / 10661 uA
                      20 mohm resistance: 1 bit = 10 mohm / 2
                      30 mohm resistance: 1 bit = 10 mohm / 3
  						high bit = 1 is in, 0 is out
********************************************************/
static  int convert_regval2ua(short reg_val)
{
    int ret;
    s64 temp;

    ret = reg_val;
    temp = (s64)(ret) * (s64)(1000 * 1000 * 5);
    temp = div_s64(temp, 10661);

    ret = temp / (R_COUL_MOHM/10);
    temp = (s64) c_offset_a *ret;
    ret = div_s64(temp, 1000000);
    ret += c_offset_b;

    return ret;

}

/*******************************************************
  Function:      convert_regval2uv
  Description:  convert uA value to BIT for register
  Input:           NULL
  Output:         NULL
  Return:         value of register in uV
  Remark:       10 mohm resistance: 1 bit = 5/10661 A = 5*1000*1000 / 10661 uA
                     20 mohm resistance: 1 bit = 10 mohm / 2
                     30 mohm resistance: 1 bit = 10 mohm / 3
********************************************************/
static  short convert_ua2regval(int ua)
{
    s64 temp;
    short regval;

    temp = ((s64)(ua) * (s64)(10661 * (R_COUL_MOHM/10)));
    temp = div_s64(temp,5000000);
    regval = (short)(temp);
    return regval;
}

 /*******************************************************
  Function:      smartstar_battery_current_ua
  Description:  get battery current in uA
  Input:           NULL
  Output:         NULL
  Return:         battery current in uA
********************************************************/
int smartstar_battery_current_ua(void)
{
    short regval;
    regval = (short)smartstar_read_avg_reg(SMARTSTAR_CURRENT, 2);
    return convert_regval2ua(regval);
}

 /*******************************************************
  Function:       is_between
  Description:   whether value locates in zone [left, right] or [right, left]
  Input:           NULL
  Output:         NULL
  Return:         1: yes
                      0: no
********************************************************/
static int is_between(int left, int right, int value)
{
	if ((left >= right) && (left >= value) && (value >= right))
		return 1;
	if ((left <= right) && (left <= value) && (value <= right))
		return 1;

	return 0;
}

 /*******************************************************
  Function:       linear_interpolate
  Description:   get y according to : y = ax +b
  Input:           (x0,y0) (x1,y1) x
  Output:         NULL
  Return:         y conresponding x
  Remark:       a = (y1 - y0) / (x1 - x0)
********************************************************/
static int linear_interpolate(int y0, int x0, int y1, int x1, int x)
{
	if ((y0 == y1) || (x == x0))
		return y0;
	if ((x1 == x0) || (x == x1))
		return y1;

	return y0 + ((y1 - y0) * (x - x0) / (x1 - x0));
}

 /*******************************************************
  Function:       interpolate_single_lut
  Description:   get y according to : y = ax +b
  Input:           struct single_row_lut *lut   lookup table
  Output:         NULL
  Return:         y conresponding x
  Remark:       a = (y1 - y0) / (x1 - x0)
                      x0 <x  and x1 > x
********************************************************/
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

 /*******************************************************
  Function:       interpolate_scalingfactor
  Description:   get y according to : y = ax +b
  Input:
                      struct sf_lut *sf_lut ---- lookup table
                      row_entry              ---- battery temperature
                      pc                         ---- percent of uah
  Output:         NULL
  Return:         scalefactor of pc
********************************************************/
static int interpolate_scalingfactor(struct sf_lut *sf_lut,
				int row_entry, int pc)
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
		//hwlog_info("pc %d greater than known pc ranges for sfd\n", pc);
		row1 = 0;
		row2 = 0;
	}
	if (pc < sf_lut->percent[rows - 1]) {
		//hwlog_info("pc %d less than known pc ranges for sf\n", pc);
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

/*******************************************************
  Function:       get_simultaneous_battery_voltage_and_current
  Description:   get battery current in mA and voltage in mV
  Input:
                      struct smartstar_coul_device *di   ---- coul device
  Output:
                      int *ibat_ua                                 ----  battery current
                      int *vbat_uv                                ---- battery voltage
  Return:         NULL
********************************************************/
static void get_simultaneous_battery_voltage_and_current(struct smartstar_coul_device *di, int *ibat_ua, int *vbat_uv)
{
    int vol[3], cur[3];
    int i;
    if( NULL == di || NULL==ibat_ua || NULL==vbat_uv)
    {
        hwlog_info("NULL point in [%s]\n", __func__);
	 return;
    }
    for (i=0; i<3; i++){
        vol[i] = smartstar_battery_voltage_uv();
        cur[i] = smartstar_battery_current_ua();
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


/*******************************************************
  Function:       interpolate_fcc
  Description:   look for fcc value by battery temperature
  Input:
                      struct smartstar_coul_device *di   ---- coul device
                      int batt_temp                              ---- battery temperature
  Output:          NULL
  Return:         battery fcc
  Remark:       this function is called  before fcc self_study
********************************************************/
static int interpolate_fcc(struct smartstar_coul_device *di, int batt_temp)
{
       if( NULL == di )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
	    return -1;
       }
	/* batt_temp is in tenths of degC - convert it to degC for lookups */
	batt_temp = batt_temp/10;
	return interpolate_single_lut(di->batt_data->fcc_temp_lut, batt_temp);
}

/*
* look for fcc scaling factory value by battery charge cycles
* used to adjust fcc before fcc self_study
*/
static int interpolate_scalingfactor_fcc(struct smartstar_coul_device *di,
								int cycles)
{
	/*
	 * sf table could be null when no battery aging data is available, in
	 * that case return 100%
	 */
	if( NULL == di )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
	    return -1;
       }
	if (di->batt_data->fcc_sf_lut)
		return interpolate_single_lut(di->batt_data->fcc_sf_lut, cycles);
	else
		return 100;
}

/*******************************************************
  Function:       interpolate_fcc_adjusted
  Description:   look for fcc value by battery temperature
  Input:
                      struct smartstar_coul_device *di   ---- coul device
                      int batt_temp                              ---- battery temperature
  Output:          NULL
  Return:         looked up battery fcc
  Remark:       this function is called  after fcc self_study
********************************************************/
static int interpolate_fcc_adjusted(struct smartstar_coul_device *di, int batt_temp)
{
       if( NULL == di )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
	    return -1;
       }
	/* batt_temp is in tenths of degC - convert it to degC for lookups */
	batt_temp = batt_temp/10;
	return interpolate_single_lut(di->adjusted_fcc_temp_lut, batt_temp);
}

/*******************************************************
  Function:       calculate_fcc_uah
  Description:   calculate fcc value by battery temperature and chargecycles
  Input:
                      struct smartstar_coul_device *di   ---- coul device
                      int batt_temp                              ---- battery temperature
                      int chargecycles                          ---- charge chcycles for scalefactor
  Output:          NULL
  Return:         battery fcc
  Remark:       lookup table is different, seperated by self_study
********************************************************/
static int calculate_fcc_uah(struct smartstar_coul_device *di, int batt_temp,
							int chargecycles)
{
    int initfcc, result, scalefactor = 0;
    if( NULL == di )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
	    return -1;
       }
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

/*******************************************************
  Function:       interpolate_pc
  Description:   look for pc
  Input:
                      struct pc_temp_ocv_lut *lut          ---- lookup table
                      int batt_temp                              ---- battery temperature
  Output:          NULL
  Return:         percent of uah
********************************************************/
static int interpolate_pc(struct pc_temp_ocv_lut *lut,
				int batt_temp, int ocv)
{
    int i, j, pcj, pcj_minus_one, pc;
    int rows = 0;
    int cols = 0;

    /* batt_temp is in tenths of degC - convert it to degC for lookups */
    batt_temp = batt_temp/10;
    if( NULL == lut )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
	    return -1;
    }

    rows = lut->rows;
    cols = lut->cols;

    if (batt_temp < lut->temp[0]) {
    	hwlog_err("batt_temp %d < known temp range for pc\n", batt_temp);
    	batt_temp = lut->temp[0];
    }
    if (batt_temp > lut->temp[cols - 1]) {
    	hwlog_err("batt_temp %d > known temp range for pc\n", batt_temp);
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

    hwlog_err("%d ocv wasn't found for temp %d in the LUT returning 100%%\n",
                                            ocv, batt_temp);
    return 1000;
}

/*******************************************************
  Function:       calculate_pc
  Description:   calculate and adjust pc
  Input:
                      int ocv_uv                                  ---- voltage
                      int batt_temp                              ----battery temperature
                      struct smartstar_coul_device *di   ---- coul device
                      int chargecycles                              ---- charge cycles
  Output:          NULL
  Return:         percent of uah
********************************************************/
static int calculate_pc(struct smartstar_coul_device *di, int ocv_uv, int batt_temp,
							int chargecycles)
{
    int pc, scalefactor;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
	 return -1;
    }
    pc = interpolate_pc(di->batt_data->pc_temp_ocv_lut, batt_temp, ocv_uv / 1000);

    scalefactor = interpolate_scalingfactor(di->batt_data->pc_sf_lut, chargecycles, pc/10);

    pc = (pc * scalefactor) / 100;
    return pc;
}

/*******************************************************
  Function:       calculate_remaining_charge_uah
  Description:   calculate remaining uah
  Input:
                      struct smartstar_coul_device *di   ---- coul device
                      int fcc_uah                                  ---- full  charge uah
                      int chargecycles                          ---- charge cycles
  Output:          NULL
  Return:         remaining uah
********************************************************/
static int calculate_remaining_charge_uah(struct smartstar_coul_device *di,
						int fcc_uah, int chargecycles)
{
    int  ocv, pc;
    int fcc_mah = fcc_uah/1000;
    int temp;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
	 return -1;
    }
    temp = di->batt_ocv_temp;

    ocv = di->batt_ocv;
    pc = calculate_pc(di, ocv, temp, chargecycles);

    return fcc_mah * pc;
}

/*******************************************************
  Function:       get_rbatt
  Description:   calculate battery resistence
  Input:
                      struct smartstar_coul_device *di   ---- coul device
                      int soc_rbatt                                  ---- soc
                      int batt_temp                              ---- battery tempture
  Output:          NULL
  Return:         battery resistence
********************************************************/
static int get_rbatt(struct smartstar_coul_device *di, int soc_rbatt, int batt_temp)
{
	int rbatt, scalefactor;
       if( NULL == di )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
	    return -1;
       }
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

/*******************************************************
  Function:       interpolate_ocv
  Description:   look for ocv according to temp, lookup table and pc
  Input:
                      struct pc_temp_ocv_lut *lut          ---- lookup table
                      int batt_temp_degc                      ---- battery temperature
                      int pc                                         ---- percent of uah
  Output:          NULL
  Return:         percent of uah
********************************************************/
static int interpolate_ocv(struct pc_temp_ocv_lut *lut,
				int batt_temp_degc, int pc)
{
	int i, ocvrow1, ocvrow2, ocv;
	int rows, cols;
	int row1 = 0;
	int row2 = 0;
       if( NULL == lut )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
	    return -1;
       }
       if (pc > 1000){
           pc = 1000;
       }
       else if (pc < 0){
           pc = 0;
       }

	rows = lut->rows;
	cols = lut->cols;
	if (pc > lut->percent[0]*10) {
		//hwlog_info("pc %d greater than known pc ranges for sfd\n", pc);
		row1 = 0;
		row2 = 0;
	}
	if (pc < lut->percent[rows - 1]*10) {
		//hwlog_info("pc %d less than known pc ranges for sf\n", pc);
		row1 = rows - 1;
		row2 = rows - 1;
	}
	for (i = 0; i < rows; i++) {
		if (pc == lut->percent[i]*10) {
			row1 = i;
			row2 = i;
			break;
		}
		if (pc > lut->percent[i]*10) {
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
	if (batt_temp_degc == lut->temp[i]) {
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

/*******************************************************
  Function:       calculate_termination_uuc
  Description:   calculate unuse uah
  Return:         unuse uah without adjust
********************************************************/
static int calculate_termination_uuc(struct smartstar_coul_device *di,
				 int batt_temp, int chargecycles,
				int fcc_uah, int i_ma,
				int *ret_pc_unusable)
{
	int unusable_uv, pc_unusable, uuc;
	int i = 0;
	int ocv_mv;
	int batt_temp_degc = batt_temp / 10;
	int rbatt_mohm;
	int delta_uv;
	int prev_delta_uv = 0;
	int prev_rbatt_mohm = 0;
	int prev_ocv_mv = 0;
	int uuc_rbatt_uv;
    int fcc_mah = fcc_uah / 1000;
    int ratio = 100;

    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
	 return -1;
    }

#if RBATT_ADJ
    if (di->rbatt_ratio){
        ratio = di->rbatt_ratio;
        i_ma = di->last_fifo_iavg_ma;
    }
#endif
#ifdef _DRV_LLT
    i = 0;
#else
	for (i = 0; i <= 100; i++)
#endif
	{
		ocv_mv = interpolate_ocv(di->batt_data->pc_temp_ocv_lut, batt_temp_degc, i*10);
		rbatt_mohm = get_rbatt(di, i, batt_temp);
        rbatt_mohm = rbatt_mohm*ratio/100;
		unusable_uv = (rbatt_mohm * i_ma) + (ZERO_VOLTAGE * 1000);
		delta_uv = ocv_mv * 1000 - unusable_uv;

		if (delta_uv > 0)
#ifdef _DRV_LLT
            ;
#else
			break;
#endif
		prev_delta_uv = delta_uv;
		prev_rbatt_mohm = rbatt_mohm;
		prev_ocv_mv = ocv_mv;
	}

	uuc_rbatt_uv = linear_interpolate(rbatt_mohm, delta_uv,
					prev_rbatt_mohm, prev_delta_uv,
					0);

	unusable_uv = (uuc_rbatt_uv * i_ma) + (ZERO_VOLTAGE * 1000);

	pc_unusable = calculate_pc(di, unusable_uv, batt_temp, chargecycles);
        uuc =  fcc_mah * pc_unusable;
	*ret_pc_unusable = pc_unusable;
	return uuc;
}

/*******************************************************
  Function:       adjust_uuc
  Description:   adjust unuse uah, changes no more than 2%
  Return:         adjusted uuc
********************************************************/

static int adjust_uuc(struct smartstar_coul_device *di, int fcc_uah,
			int new_pc_unusable,
			int new_uuc,
			int batt_temp,
			int rbatt,
			int *iavg_ma)
{
       int fcc_mah = fcc_uah / 1000;
       if( NULL == di )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
       }
	if (di->prev_pc_unusable == -EINVAL
		|| abs(di->prev_pc_unusable - new_pc_unusable) <= 50) {
		di->prev_pc_unusable = new_pc_unusable;
		return new_uuc;
	}

	/* the uuc is trying to change more than 2% restrict it */
	if (new_pc_unusable > di->prev_pc_unusable)
		di->prev_pc_unusable += 20;
	else
		di->prev_pc_unusable -= 20;

	new_uuc = fcc_mah * di->prev_pc_unusable;

	return new_uuc;
}


/*******************************************************
  Function:       calculate_unusable_charge_uah
  Description:   calculate unuse uah
  Return:         unuse uah without adjust and changes less than 2%
********************************************************/
static int calculate_unusable_charge_uah(struct smartstar_coul_device *di,
				int rbatt, int fcc_uah, int cc_uah,
				 int batt_temp, int chargecycles,
				int iavg_ua)
{
	int uuc_uah_iavg;
	int i;
	int iavg_ma = iavg_ua / 1000;
	static int iavg_samples[IAVG_SAMPLES] = {0};
	static int iavg_index = 0;
	static int iavg_num_samples = 0;
	int pc_unusable;

	/*
	 * if we are charging use a nominal avg current so that we keep
	 * a reasonable UUC while charging
	 */
	if( NULL == di )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
       }
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

    hwlog_info("RBATT_ADJ:UUC =%d uAh, pc=%d.%d\n",
        uuc_uah_iavg, pc_unusable/10, pc_unusable%10);

    di->rbatt_ratio = 0;

	/* restrict the uuc such that it can increase only by one percent */
	uuc_uah_iavg = adjust_uuc(di, fcc_uah, pc_unusable, uuc_uah_iavg,
					batt_temp, rbatt, &iavg_ma);

    uuc_uah_iavg += fcc_uah/1000*10;

	di->batt_uuc = uuc_uah_iavg;

	return uuc_uah_iavg;
}

 /*******************************************************
  Function:       recalc_chargecycles
  Description:    recalculate the chargecycle after charging done
  Input:
                      struct smartstar_coul_device *di   ---- coul device
  Output:          NULL
  Return:         new chargecycles
********************************************************/
static unsigned int recalc_chargecycles(struct smartstar_coul_device *di)
{
    int cc_end, real_fcc, fcc, temp, pc, new_chargecycles;
    unsigned int retval = 0;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
        return -1;
    }
    if (di->batt_soc==100 && di->charging_begin_soc/10<MIN_BEGIN_PERCENT_FOR_LEARNING) {
        cc_end = calculate_cc_uah();
        temp = di->batt_temp;
        real_fcc = (cc_end - di->charging_begin_cc)*1000/(1000 - di->charging_begin_soc);
        fcc = interpolate_fcc(di,temp);
        pc = real_fcc *100 / fcc;
        new_chargecycles = interpolate_single_y_lut(di->batt_data->fcc_sf_lut, pc);
        new_chargecycles -= 40*100;
        retval = (unsigned int)(new_chargecycles>0?new_chargecycles:0);

        hwlog_info("trigger battery charge cycle reclac, val = %d!\n", new_chargecycles);
    }

    return retval;
}

static int __init early_parse_pmu_nv_addr_cmdline(char *p)
{
    char buf[30] = {0};

    char *endptr = NULL;
    if( NULL == p )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
    }
    memcpy(buf, p, strlen(p)+1);

    nv_info_addr = simple_strtoul(buf, &endptr, 16);

    return 0;
}

#ifdef _DRV_LLT_
#else
early_param("pmu_nv_addr", early_parse_pmu_nv_addr_cmdline);
#endif
static int __init move_pmu_nv_info(void)
{
    struct ss_coul_nv_info *pmu_nv_addr;

    if (nv_info_addr == 0){
        return 0;
    }
     #ifdef CONFIG_ARM64
    pmu_nv_addr = (struct ss_coul_nv_info*)ioremap(nv_info_addr, sizeof (struct ss_coul_nv_info));
    #else
    pmu_nv_addr = (struct ss_coul_nv_info*)phys_to_virt(nv_info_addr);
    #endif
    hwlog_info("pmu_nv_addr=0x%p\n", pmu_nv_addr);
	if (NULL == pmu_nv_addr)
	{
		hwlog_err("nv add err,pmu_nv_addr=0x%p\n", pmu_nv_addr);
		return 0;
	}
    memcpy(&my_nv_info, pmu_nv_addr, sizeof(my_nv_info));
	hwlog_info("v_offset_b=%d,c_offset_b=%d\n", my_nv_info.v_offset_b,my_nv_info.c_offset_b);

    return 0;
}



arch_initcall(move_pmu_nv_info);
int fastboot_save_nv_info_error(void)
{
    unsigned short ocvreg = 0;
    SMARTSTAR_REGS_READ(SMARTSTAR_SAVE_OCV_ADDR,&ocvreg,2);
    if(ocvreg == 0){
        return 1;
    }
    else{
        return 0;
    }
}


 /*******************************************************
  Function:       read_nv_info
  Description:    get info from NV
  Input:
                      struct smartstar_coul_device *di   ---- coul device
  Output:          NULL
  Return:         0
********************************************************/
static int read_nv_info(struct smartstar_coul_device *di)
{
    int i;
    struct ss_coul_nv_info *pinfo = &di->nv_info;

    struct single_row_lut *preal_fcc_lut = &di->adjusted_fcc_temp_lut_tbl1;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
    }
    memcpy(&di->nv_info, &my_nv_info, sizeof(my_nv_info));

    di->batt_chargecycles = pinfo->charge_cycles;
#if 0 /*it's impossible to read NV for geting a /b value after,so reserved。*/

    v_offset_a = pinfo->v_offset_a==0?DEFAULT_V_OFF_A:pinfo->v_offset_a;
    v_offset_b = pinfo->v_offset_b==0?DEFAULT_V_OFF_B:pinfo->v_offset_b;
    c_offset_a = pinfo->c_offset_a==0?DEFAULT_C_OFF_A:pinfo->c_offset_a;
    c_offset_b = pinfo->c_offset_b==0?DEFAULT_C_OFF_B:pinfo->c_offset_b;
    di->r_pcb   = pinfo->r_pcb == 0? DEFAULT_RPCB: pinfo->r_pcb;
    hwlog_info("read nv partion ok\n");
    hwlog_info("v_a=%d,v_b=%d,c_a=%d,c_b=%d, r_pcb=%d, cycles=%d, reg_c=%d, reg_v=%d, start_cc=%d\n",
        v_offset_a, v_offset_b, c_offset_a, c_offset_b, pinfo->r_pcb,
        pinfo->charge_cycles, pinfo->calc_ocv_reg_c, pinfo->calc_ocv_reg_v, pinfo->start_cc);
#endif
    pl_v_offset_a = pinfo->v_offset_a;
    pl_v_offset_b = pinfo->v_offset_b;
    pl_c_offset_a = pinfo->c_offset_a;
    pl_c_offset_b = pinfo->c_offset_b;
    get_calibration_params(di);
    hwlog_info("pl_v_a=%d,pl_v_b=%d,pl_c_a=%d,pl_c_b=%d, r_pcb=%d, cycles=%d, reg_c=%d, reg_v=%d, start_cc=%d\n",
        pl_v_offset_a, pl_v_offset_b, pl_c_offset_a, pl_c_offset_b, pinfo->r_pcb,
        pinfo->charge_cycles, pinfo->calc_ocv_reg_c, pinfo->calc_ocv_reg_v, pinfo->start_cc);

    for (i=0; i<MAX_TEMPS; i++)
    {
        if (pinfo->real_fcc[i] == 0){
            break;
        }

        if (pinfo->real_fcc[i] < 100)
        {
            hwlog_info("real fcc in nv is not currect!\n");
            return 0;
        }

        preal_fcc_lut->x[i] = pinfo->temp[i];
        preal_fcc_lut->y[i] = pinfo->real_fcc[i];
    }

    if (i == 0){
        hwlog_info("no real fcc data in nv\n");
        return 0;
    }

    preal_fcc_lut->cols = i;

    di->adjusted_fcc_temp_lut = preal_fcc_lut;

    hwlog_info("temp:real_fcc %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d\n"
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

static int read_nv_info_real(struct smartstar_coul_device *di)
{
    int ret, i;
    struct hisi_nve_info_user nve;
    struct ss_coul_nv_info *pinfo = &di->nv_info;
    struct single_row_lut *preal_fcc_lut = &di->adjusted_fcc_temp_lut_tbl1;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
    }
    memset(&nve, 0, sizeof(nve));
    strcpy(nve.nv_name, SS_COUL_NV_NAME);
    nve.nv_number = SS_COUL_NV_NUM;
    nve.valid_size = sizeof(*pinfo);
    nve.nv_operation = NV_READ;

    ret = hisi_nve_direct_access(&nve);
    if (ret) {
        pinfo->charge_cycles = 0;
        pinfo->r_pcb = 0;
#if 1 /*it's impossible to read NV for geting a /b value after,so reserved。*/
        pinfo->v_offset_a = DEFAULT_V_OFF_A;
        pinfo->v_offset_b = DEFAULT_V_OFF_B;
        pinfo->c_offset_a = DEFAULT_C_OFF_A;
        pinfo->c_offset_b = DEFAULT_C_OFF_B;
#endif
        memset(pinfo->temp,  0, sizeof(pinfo->temp));
        memset(pinfo->real_fcc,  0, sizeof(pinfo->real_fcc));
        di->is_nv_read = 0;
        hwlog_info("read nv partion failed, ret=%d\n", ret);
        return -1;
    }
    memcpy(pinfo, nve.nv_data, sizeof(*pinfo));


    di->batt_chargecycles = pinfo->charge_cycles;
#if 1 /*it's impossible to read NV for geting a /b value after,so reserved。*/
    v_offset_a = pinfo->v_offset_a==0?DEFAULT_V_OFF_A:pinfo->v_offset_a;
    v_offset_b = pinfo->v_offset_b==0?DEFAULT_V_OFF_B:pinfo->v_offset_b;
    c_offset_a = pinfo->c_offset_a==0?DEFAULT_C_OFF_A:pinfo->c_offset_a;
    c_offset_b = pinfo->c_offset_b==0?DEFAULT_C_OFF_B:pinfo->c_offset_b;
#endif
    pl_v_offset_a = pinfo->v_offset_a;
    pl_v_offset_b = pinfo->v_offset_b;
    pl_c_offset_a = pinfo->c_offset_a;
    pl_c_offset_b = pinfo->c_offset_b;
    hwlog_info("read nv partion ok\n");
    hwlog_info("pl_v_a=%d,pl_v_b=%d,pl_c_a=%d,pl_c_b=%d,cycles=%d, reg_c=%d, reg_v=%d, start_cc=%d\n"
        ,pinfo->v_offset_a, pinfo->v_offset_b, pinfo->c_offset_a, pinfo->c_offset_b,
        pinfo->charge_cycles,pinfo->calc_ocv_reg_c, pinfo->calc_ocv_reg_v, pinfo->start_cc);
    for (i=0; i<MAX_TEMPS; i++)
    {
        if (pinfo->real_fcc[i] == 0){
            break;
        }

        if (pinfo->real_fcc[i] < 100)
        {
            hwlog_info("real fcc in nv is not currect!\n");
            return 0;
        }

        preal_fcc_lut->x[i] = pinfo->temp[i];
        preal_fcc_lut->y[i] = pinfo->real_fcc[i];
    }

    if (i == 0){
        hwlog_info("no real fcc data in nv\n");
        return 0;
    }

    preal_fcc_lut->cols = i;

    di->adjusted_fcc_temp_lut = preal_fcc_lut;

    hwlog_info("temp:real_fcc %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d\n"
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

 /*******************************************************
  Function:       save_nv_info
  Description:    save info to NV
  Input:
                      struct smartstar_coul_device *di   ---- coul device
  Output:          NULL
  Return:         -1: failed     0:success
********************************************************/
static int save_nv_info(struct smartstar_coul_device *di)
{
    int ret, i;
    char val;
    int refresh_fcc_success = 1;
    struct hisi_nve_info_user nve;
    struct ss_coul_nv_info *pinfo = &di->nv_info;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
    }
    if (!di->is_nv_read){ /* here failed */
        hwlog_err("save nv before read, error\n");
        val = SMARTSTAR_REG_READ(SMARTSTAR_NV_SAVE_SUCCESS);
        udelay(110);
        SMARTSTAR_REG_WRITE(SMARTSTAR_NV_SAVE_SUCCESS, (val & (~NV_SAVE_BITMASK)));
        return -1;
    }

    memset(&nve, 0, sizeof(nve));
    strcpy(nve.nv_name, SS_COUL_NV_NAME);
    nve.nv_number = SS_COUL_NV_NUM;
    nve.valid_size = sizeof(*pinfo);
    nve.nv_operation = NV_WRITE;

    pinfo->charge_cycles = di->batt_chargecycles;

    pinfo->v_offset_a = pl_v_offset_a;
    pinfo->v_offset_b = pl_v_offset_b;
    pinfo->c_offset_a = pl_c_offset_a;
    pinfo->c_offset_b = pl_c_offset_b;

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

    /* here save info in register */
    ret = hisi_nve_direct_access(&nve);
    if (ret) {
        hwlog_info("save nv partion failed, ret=%d\n", ret);
        val = SMARTSTAR_REG_READ(SMARTSTAR_NV_SAVE_SUCCESS);
        udelay(110);
        SMARTSTAR_REG_WRITE(SMARTSTAR_NV_SAVE_SUCCESS, (val & (~NV_SAVE_BITMASK)));
    }
    else
    {
        val = SMARTSTAR_REG_READ(SMARTSTAR_NV_SAVE_SUCCESS);
        udelay(110);
        SMARTSTAR_REG_WRITE(SMARTSTAR_NV_SAVE_SUCCESS, (val | NV_SAVE_BITMASK));
    }

    return ret;
}

static void update_chargecycles(struct smartstar_coul_device *di)
{
    if( 0 == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return;
    }
    if (di->batt_soc - di->charging_begin_soc/10 > 0) {
        di->batt_chargecycles += di->batt_soc - di->charging_begin_soc/10;
        hwlog_info("new chargecycle=%d, added=%d\n", di->batt_chargecycles, di->batt_soc - di->charging_begin_soc/10);
    }
    else{
        hwlog_info("chargecycle not updated, soc_begin=%d, soc_current=%d\n", di->charging_begin_soc/10, di->batt_soc);
    }

	di->charging_begin_soc = 1000;
}

/**
 * is_smartstar_coul_ready -
 *
 * 0:not ready, 1: ready
 */
int is_smartstar_coul_ready(void)
{
    // TODO: do something here ?
    return 1;
}

/**
 * hisi_smartstar_temperature
 *
 * return the battery temperature in centigrade.
 */
 extern int DRV_HKADC_GET_TEMP(HKADC_TEMP_PROTECT_E log_chan,
              unsigned short* volt,
              short* temp, HKADC_CONV_MODE_E ulMode);
int smartstar_battery_temperature_tenth_degree(void)
{
    int ret = 0;
    short temperature = 0;
    unsigned short volt = 0;
    static int once_read_ok = 0;
    static int pre_temperature = -999;
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

	/*default is no battery in sft and udp, so here temp is fixed 25c to prevent low power reset*/
	if ( BAT_BOARD_ASIC != is_board_type) {
		return DEFAULT_TEMP*10;
	}
    ret = DRV_HKADC_GET_TEMP(HKADC_TEMP_BATTERY,&volt,&temperature, 0);
    if(ret != 0){ // 0 == HKADC_OK
        if (once_read_ok){
            hwlog_err("read battery temperature from nv. temp = %d\n", di->nv_info.hkadc_batt_temp);
            return di->batt_temp;
        }else{
            return di->nv_info.hkadc_batt_temp*10;
        }
    }
    if(once_read_ok == 0){
        once_read_ok = 1;
        hwlog_info("read battery temperature success, battery temperature = %d\n", temperature);
    }
    di->batt_uf_temp = temperature;
    if (pre_temperature == -999){
        if(temperature >= -40 && temperature <= 80){
            pre_temperature = temperature;
        }else if (temperature > 80){
            hwlog_err("read battery temperature error, temperature over 80 degree, return pre temperatrue: %d.\n",pre_temperature);
            return di->nv_info.hkadc_batt_temp*10;    //get an invalid temperatrue at the first time.
        }
    }else{
        if(temperature >= -40 && temperature <= 80){
            if(((pre_temperature - temperature) > 10 ||(pre_temperature - temperature) < -10) && disable_temperature_debounce){
                if(pre_temperature > temperature){
                    temperature = pre_temperature - 10;
                }else{
                    temperature = pre_temperature + 10;
                }
            }
            pre_temperature = temperature;

            disable_temperature_debounce = 1;
        }else if (temperature > 80){
            temperature = pre_temperature;
        }
    }

    if(temperature < -40){
        pre_temperature = -999;
    }

    return temperature*10;
}

#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
void is_coul_reg_write(int temp)
{
     int val=0;
     val = SMARTSTAR_REG_READ(SMARTSTAR_COUL_SET_TEMP_PROTECT_FLAG);
     val =val&MASK_BIT4;
     if(temp==1)
     {
       val = val | SET_BIT4;
     }
     else
     {
        val =val&MASK_BIT4;
     }
     hwlog_info("hisi  get val  =%d\n",val);
        SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_SET_TEMP_PROTECT_FLAG, val);
}
int is_coul_reg_read(void)
{
     int val=0;
     val = SMARTSTAR_REG_READ(SMARTSTAR_COUL_SET_TEMP_PROTECT_FLAG);
     if(val&SET_BIT4)
     {
       val=1;
     }
     else
     {
       val=0;
     }
     return val;
}
#endif

/**
 * hisi_smartstar_temperature
 *
 * return the battery temperature in centigrade.
 */

int smartstar_battery_uf_temperature(void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    return di->batt_uf_temp;
}


  /*******************************************************
  Function:        smartstar_battery_temperature
  Description:    return the battery temperature in centigrade.
  Input:            NULL
  Output:          NULL
  Return:         battery temperature in centigrade.
********************************************************/
int smartstar_battery_temperature(void)
{
    int temp = smartstar_battery_temperature_tenth_degree();
    return temp/10;
}


static int is_battery_exist(void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
	return di->batt_exist;
}
/*******************************************************
  Function:        is_smartstar_battery_exist
  Description:    check whether battery exist
  Input:            NULL
  Output:          NULL
  Return:         0:battery isn't exist, 1: exist
********************************************************/
int is_smartstar_battery_exist(void)
{
/*    struct smartstar_coul_device *di = g_smartstar_coul_dev;  --no use */
    int temp;
    temp = smartstar_battery_temperature();
#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
    if(is_coul_reg_read())
    {
     return 1;
    }
#endif
    if ((temp <= ABNORMAL_BATT_TEMPERATURE_LOW)
        || (temp >= ABNORMAL_BATT_TEMPERATURE_HIGH)) {
        return 0;
    } else {
        return 1;
    }
}

/*******************************************************
  Function:        is_smartstar_battery_reach_threshold
  Description:    check whether battery uah reach threshold
  Input:            NULL
  Output:          NULL
  Return:         0:not, 4: lower than warning_lev, 8: lower than Low_Lev
********************************************************/
int is_smartstar_battery_reach_threshold(void)
{

    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
    }
    if (!is_smartstar_battery_exist()){
        return 0;
    }

    if (di->batt_soc > BATTERY_CC_WARNING_LEV)
        return 0;
    else if (di->batt_soc > BATTERY_CC_LOW_LEV)
        return BQ_FLAG_SOC1;
    else
        return BQ_FLAG_LOCK;
}


 /*******************************************************
  Function:        smartstar_battery_voltage
  Description:    get battery voltage in uV
  Input:            NULL
  Output:          NULL
  Return:         battery voltage in uV
********************************************************/
int smartstar_battery_voltage(void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    int ibat_ua = 0, vbat_uv = 0;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
    }

    get_simultaneous_battery_voltage_and_current(di, &ibat_ua, &vbat_uv);

    return vbat_uv/1000;
}

 /*******************************************************
  Function:        smartstar_battery_voltage
  Description:    get battery current in uA
  Input:            NULL
  Output:          NULL
  Return:         current voltage in uA
********************************************************/
int smartstar_battery_current(void)
{
    int cur = smartstar_battery_current_ua();
    return (cur / 1000);
}

 /*******************************************************
  Function:        is_smartstar_battery_full
  Description:    is battery charged to full
  Input:            NULL
  Output:          NULL
  Return:         0: not full, 1: full
********************************************************/
int is_smartstar_battery_full(void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    int ibat_ma = 0;
    int iterm_ma = 0;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return 0;
    }
    if (!is_smartstar_battery_exist()){
        return 0;
    }

    iterm_ma = di->batt_data->terminal_currentmA;

    ibat_ma = -smartstar_battery_current();

	if (di->last_iavg_ma > MIN_CHARGING_CURRENT_OFFSET
        && di->last_iavg_ma < iterm_ma
        && ibat_ma > MIN_CHARGING_CURRENT_OFFSET
        && ibat_ma < iterm_ma)
	{
		hwlog_info("battery_full:ibat_ma=%d, when soc_real=%d, last_iavg_ma=%d\n",
						   ibat_ma, di->batt_soc_real, di->last_iavg_ma);
		return 1;
	}
    else
        return 0;
}
/**
 * smartstar_battery_brand
 *
 *
 * return the battery brand in string.
 */
char* smartstar_battery_brand(void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    return di->batt_data->batt_brand;
}
/**
 * smartstar_battery_current_avg - return the avg_current of battery
 *   in at least 2MIN.
 * return the battery avg_current in mA.
 */
int smartstar_battery_current_avg(void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    return di->last_iavg_ma;
}

/**
 * smartstar_battery_unfilter_capacity - return the unfilter capacity of battery.
 *
 * return the battery unfiltered capacity in percent.
 */
int smartstar_battery_unfiltered_capacity(void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    return di->batt_soc_real/10;
}

/*******************************************************
  Function:        smartstar_battery_capacity
  Description:    get the capacity of battery
  Input:            NULL
  Output:          NULL
  Return:         the capacity of battery
********************************************************/
int smartstar_battery_capacity(void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (battery_is_removable) {
         if (!is_smartstar_battery_exist()){
             return 0;
         }
   }

   if(hand_chg_capacity_flag == 1)
        return input_capacity;

    return di->batt_soc;
}

 /*******************************************************
  Function:        smartstar_battery_rm
  Description:    battery remaining uah with uuh
  Input:            NULL
  Output:          NULL
  Return:         remaining uah
********************************************************/
int smartstar_battery_rm(void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    return di->batt_ruc/1000;
}

 /*******************************************************
  Function:        smartstar_battery_fcc
  Description:    battery full charge capacity
  Input:            NULL
  Output:          NULL
  Return:          fcc
********************************************************/
int smartstar_battery_fcc (void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    return di->batt_fcc/1000;
}

 /*******************************************************
  Function:        smartstar_battery_uuc
  Description:    battery unused capacity
  Input:            NULL
  Output:          NULL
  Return:          uuc
********************************************************/
int smartstar_battery_uuc (void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    return di->batt_uuc/1000;
}

 /*******************************************************
  Function:        smartstar_battery_cc
  Description:    capacity recorded by coulomb
  Input:            NULL
  Output:          NULL
  Return:          battery_cc
********************************************************/
int smartstar_battery_cc (void)
{
    int cc;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    cc = calculate_cc_uah();

    return cc/1000;
}

int smartstar_battery_delta_rc (void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    return di->batt_delta_rc/1000;
}

 /*******************************************************
  Function:        smartstar_battery_ocv
  Description:    battery open circult voltage
  Input:            NULL
  Output:          NULL
  Return:          ocv in uV
********************************************************/
int smartstar_battery_ocv (void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    return di->batt_ocv;
}
 /*******************************************************
  Function:        smartstar_battery_resistance
  Description:    battery_resistance
  Input:            NULL
  Output:          NULL
  Return:          battery_resistance
********************************************************/
int smartstar_battery_resistance (void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    return di->rbatt;
}

/*******************************************************
  Function:        smartstar_battery_tte
  Description:    time to empty
  Input:            NULL
  Output:          NULL
  Return:          value in min or -1----charging
********************************************************/
int smartstar_battery_tte (void)
{
    int cc, cur;
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    cur = smartstar_battery_current_ua();

    if (cur < 0){
        return -1; /* charging now */
    }

    cc = di->batt_ruc - di->batt_uuc;

	if (cc <= 0) {
        return -1;
    }

    return cc * 60 / cur;
}

 /*******************************************************
  Function:        smartstar_battery_ttf
  Description:    time to full
  Input:            NULL
  Output:          NULL
  Return:          value in min or -1----discharging
********************************************************/
int smartstar_battery_ttf (void)
{
    int cc, cur;
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    cur = smartstar_battery_current_ua();

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    if (cur >= 0){
        return -1; /* discharging now */
    }

    cc = di->batt_fcc - di->batt_ruc;

    return cc * 60 / (-cur); /* cur is < 0 */
}

/*******************************************************
  Function:        smartstar_battery_health
  Description:    battery health l
  Input:            NULL
  Output:          NULL
  Return:          0: Unknown, 1:Good, 2:Overheat, 3:Dead, 4:Over voltage,
        	         5:Unspecified failure, 6:Cold
********************************************************/
int smartstar_battery_health (void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    int status = POWER_SUPPLY_HEALTH_GOOD;
    int temp = di->batt_temp/10;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    if (temp < TEMP_TOO_COLD)
        status = POWER_SUPPLY_HEALTH_COLD;
    else if (temp > TEMP_TOO_HOT)
        status = POWER_SUPPLY_HEALTH_OVERHEAT;

    return status;
}

/*******************************************************
  Function:        smartstar_battery_health
  Description:    battery health level
  Input:            NULL
  Output:          NULL
  Return:          0: Unknown, 1:CRITICAL, 2:LOW, 3:NORMAL, 4:HIGH,
        	         5:FULL
********************************************************/
int smartstar_battery_capacity_level (void)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    int data_capacity = di->batt_soc;
    int status;

    if (!is_smartstar_battery_exist()){
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
 * smartstar_battery_technology
 */
int smartstar_battery_technology (void)
{
    /*Default technology is "Li-poly"*/
    return POWER_SUPPLY_TECHNOLOGY_LIPO;
}

/*******************************************************
  Function:        smartstar_battery_charge_param
  Description:    battery data param
  Input:            NULL
  Output:          NULL
  Return:          0: invalid battery, 1: successed
********************************************************/
int smartstar_battery_charge_param(struct battery_charge_param_s *param)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if (!is_smartstar_battery_exist()){
        return 0;
    }

    if (di->batt_data == NULL)
        return 0;

    param->max_cin_currentmA = di->batt_data->max_cin_currentmA;
    param->max_currentmA= di->batt_data->max_currentmA;
    param->max_voltagemV= di->batt_data->max_voltagemV;
    param->charge_in_temp_5 = di->batt_data->charge_in_temp_5;
    param->charge_in_temp_10 = di->batt_data->charge_in_temp_10;
    param->design_capacity = di->batt_data->fcc;

    return 1;
}
/*  */
int is_smartstar_fcc_debounce(void) {
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    int ret = 0;
    int batt_fcc = 0;
    int fcc = 0;

    if (!di) {
        //hwlog_err("%s, di is NULL\n", __func__);
        return ret;
    }

    if (!is_smartstar_battery_exist()) {
        return ret;
    }

    batt_fcc = smartstar_battery_fcc();
    fcc = interpolate_fcc(di, di->batt_temp);
    if (batt_fcc < (fcc * 85 / 100)  || batt_fcc > (fcc * 115 / 100)) {
        ret = 1;
        hwlog_err("%s, fcc_from_temp=%d, batt_fcc=%d, ret=%d\n", __func__, fcc, batt_fcc, ret);
    }

    return ret;
}

void save_ocv(int ocv)
{
    short ocvreg = convert_uv2regval(ocv);
    udelay(110);
    SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR,&ocvreg,2);
    hwlog_err("save ocv, ocv=%d,reg=%d", ocv, ocvreg);
}


/*******************************************************
  Function:        get_ocv_by_fcc
  Description:    interpolate ocv value by full charge capacity when charging done
  Input:
                       struct smartstar_coul_device *di      ---- coul device
                       int batt_temp                                 ---- battery temperature
  Output:          NULL
  Return:          NULL
********************************************************/
static void get_ocv_by_fcc(struct smartstar_coul_device *di,int batt_temp)
{
    unsigned int new_ocv;
    int batt_temp_degc = batt_temp/10;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return;
    }
    /*looking for ocv value in the OCV-FCC table*/
    new_ocv = interpolate_ocv(di->batt_data->pc_temp_ocv_lut, batt_temp_degc, 1000);

    new_ocv *=1000;

    if ((new_ocv - di->batt_ocv) > 0) {
        DBG_CNT_INC(dbg_ocv_cng_1);
        hwlog_info("full charged, and OCV change, "
                            "new_ocv = %d, old_ocv = %d \n",new_ocv,di->batt_ocv);
        di->batt_ocv = new_ocv;
        di->batt_ocv_temp = di->batt_temp;
        di->batt_ocv_valid_to_refresh_fcc = 1;
        save_ocv(new_ocv);
        /*clear cc register*/
        clear_cc_register();
        clear_coul_time();
    } else {
        DBG_CNT_INC(dbg_ocv_fc_failed);
        hwlog_err("full charged, but OCV don't change,\
                            new_ocv = %d, old_ocv = %d \n",new_ocv,di->batt_ocv);
    }
}


/*******************************************************
  Function:        calculate_eco_leak_uah
  Description:    calculate capacity leak from existing ECO MODE to calc soc first time
  Input:            NULL
  Output:          NULL
  Return:          leak capacity
  Remark:        ECO uah register keep the same value after exist from ECO
********************************************************/
static int calculate_eco_leak_uah( void )
{
    unsigned int in_val = 0;
    unsigned int out_val = 0;
    int eco_in_uah = 0;
    int eco_out_uah = 0;
    int present_in_uah = 0;
    int present_out_uah = 0;
    int rst_uah = 0;
    int eco_uah = 0;
    int cur_uah = 0;

    SMARTSTAR_REGS_READ(SMARTSTAR_ECO_OUT_CLIN_REG_BASE, &in_val, 4);
    SMARTSTAR_REGS_READ(SMARTSTAR_ECO_OUT_CLOUT_REG_BASE, &out_val, 4);
    /*if: first time to calc soc after exiting from ECO Mode */
    if( last_eco_in != in_val || last_eco_out != out_val)
    {
		eco_out_uah = convert_regval2uah(out_val);
		eco_in_uah = convert_regval2uah(in_val);
		eco_uah = eco_out_uah - eco_in_uah;

        /* current cc  */
		out_val = (unsigned int)smartstar_read_avg_reg(SMARTSTAR_CL_OUT_BASE, 4);
		in_val = (unsigned int)smartstar_read_avg_reg(SMARTSTAR_CL_IN_BASE, 4);
		present_in_uah = convert_regval2uah(in_val);
		present_out_uah = convert_regval2uah(out_val);
		cur_uah = present_out_uah - present_in_uah;

		/* leak cc from exisingt eco mode to first calc soc */
	    rst_uah = cur_uah - eco_uah;


	    hwlog_info("eco_in=%d,eco_out=%d,present_in=%d,present_out=%d,leak cc=%d .\n",
                                                 eco_in_uah, eco_out_uah,present_in_uah, present_out_uah, rst_uah);
    }
    else
    {
        rst_uah = 0;
	 hwlog_info("Not the FIRST time to calc soc after exiting from ECO Model, leak cc=0 .\n");
    }
    return rst_uah;
}
/*******************************************************
  Function:        smartstar_get_rm
  Description:     get remain capacity
  Input:            struct smartstar_coul_device *di                 ---- coul device
  Output:          NULL
  Return:          NULL
********************************************************/
static void smartstar_get_rm(struct smartstar_coul_device *di, int *rm)
{
    int fcc_uah = 0, unusable_charge_uah = 0, delta_rc_uah = 0;
    int remaining_charge_uah = 0;
    int cc_uah = 0;
    int rbatt = 0;
    int temp = 0;

    if( NULL == di )
    {
        hwlog_err("NULL point in [%s]\n", __func__);
        return;
    }

    /* check temperature */
    temp = smartstar_battery_temperature_tenth_degree();

    if (temp != di->batt_temp)
    {
        di->batt_temp = temp;
    }
    calculate_soc_params(di,
                                    &fcc_uah,
                                    &unusable_charge_uah,
                                    &remaining_charge_uah,
                                    &cc_uah,
                                    &delta_rc_uah,
                                    &rbatt);
    *rm = remaining_charge_uah - cc_uah;
}

 /*******************************************************
  Function:        get_ocv_by_vol
  Description:    calculate ocv by 10 history data when AP exist from deep sleep
  Input:            struct smartstar_coul_device *di      ---- coul device
  Output:          NULL
  Return:          OCV
********************************************************/
static void get_ocv_by_vol(struct smartstar_coul_device *di)
{
    int i = 0;
    int used;
    int current_ua = 0;
    int voltage_uv = 0;
    short regval = 0;
    short curreglimit = 0;
    int totalvol, totalcur;
    int rm = 0;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	    return;
    }
    curreglimit = (short)di->batt_ocv_curreglimit;

    totalvol = 0;
    totalcur = 0;
    used = 0;
#if 0
    for (i=0; i<FIFO_DEPTH; i++) {
        SMARTSTAR_REGS_READ(SMARTSTAR_CUR_FIFO_BASE + i*2,&regval,2);
        if ((regval >= curreglimit) || (regval <= 0)) {
            DBG_CNT_INC(dbg_invalid_vol);
            continue;
        }
        DBG_CNT_INC(dbg_valid_vol);
        SMARTSTAR_REGS_READ(SMARTSTAR_VOL_FIFO_BASE + i*2,&regval,2);
        totolvol += regval;
        used++;
    }
#else
	for (i = 0; i < FIFO_DEPTH; i++)
	{
		SMARTSTAR_REGS_READ(SMARTSTAR_CUR_FIFO_BASE + i * 2, &regval, 2);
		current_ua = convert_regval2ua(regval);
		if (current_ua >= CURRENT_LIMIT
			|| current_ua < CHARGING_CURRENT_OFFSET)
		{
			DBG_CNT_INC(dbg_invalid_vol);
			hwlog_info("invalid current = %d ua\n", current_ua);
			continue;
		}
		DBG_CNT_INC(dbg_valid_vol);
		hwlog_info("valid current = %d ua\n", current_ua);
		SMARTSTAR_REGS_READ(SMARTSTAR_VOL_FIFO_BASE + i * 2, &regval, 2);
		totalvol += convert_regval2uv(regval);
        totalcur += current_ua;
		used++;
	}
#endif
	hwlog_info("used = %d\n", used);
	if (used > 0)
	{
		voltage_uv = totalvol / used;
              current_ua = totalcur / used;
              voltage_uv += current_ua/1000*(di->r_pcb/1000 + DEFAULT_BATTERY_OHMIC_RESISTANCE);

	    if( voltage_uv<3200000
            || (voltage_uv>3670000 && voltage_uv<3690000)
            || (voltage_uv>3730000 && voltage_uv<3800000)){
    		hwlog_info("do not update OCV(%d)\n",
						   voltage_uv);
            return;
        }

		hwlog_info("awake from deep sleep, old OCV = %d \n",
						   di->batt_ocv);
		di->batt_ocv = voltage_uv;
        di->batt_ocv_temp = di->batt_temp;
        di->batt_ocv_valid_to_refresh_fcc = 1;
		clear_cc_register();
		clear_coul_time();
		save_ocv(voltage_uv);

		hwlog_info("awake from deep sleep, new OCV = %d \n",
						   di->batt_ocv);
		DBG_CNT_INC(dbg_ocv_cng_0);
        if(CHARGING_STATE_CHARGE_DONE == di->charging_state)
        {
            smartstar_get_rm(di, &rm);
            if(rm < di->batt_limit_fcc)
            {
                di->batt_limit_fcc = rm*100/101;
                di->is_nv_need_save = 1;
            }
        }
	}
}

 /*******************************************************
  Function:        get_calc_ocv
  Description:    get ocv by soft way when shutdown time less 20min
  Input:            struct smartstar_coul_device *di      ---- coul device
  Output:          NULL
  Return:          OCV
********************************************************/
static int get_calc_ocv(struct smartstar_coul_device *di)
{
    int ocv = 0;
    int batt_temp = 0;
    int chargecycles = 0;
    int soc_rbatt = 0;
    int rbatt = 0;
    int vbatt_uv = 0;
    int ibatt_ua = 0;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	    return -1;
    }
    batt_temp = di->batt_temp;
    chargecycles = di->batt_chargecycles/100;

    vbatt_uv = convert_regval2uv(di->nv_info.calc_ocv_reg_v);

    ibatt_ua = convert_regval2ua(di->nv_info.calc_ocv_reg_c);

    soc_rbatt = calculate_pc(di, vbatt_uv, batt_temp, chargecycles);

    rbatt = get_rbatt(di, soc_rbatt/10, batt_temp);
    ocv =  vbatt_uv + ibatt_ua*rbatt/1000;

    hwlog_info("calc ocv, v_uv=%d, i_ua=%d, soc_rbatt=%d, rbatt=%d, ocv=%d\n",
        vbatt_uv, ibatt_ua, soc_rbatt/10, rbatt, ocv);

    return ocv;
}



/*******************************************************
  Function:        get_initial_ocv
  Description:    get first ocv from register, hardware record it during system reset.
  Input:            struct smartstar_coul_device *di      ---- coul device
  Output:          NULL
  Return:          init OCV
********************************************************/
static void get_initial_ocv(struct smartstar_coul_device *di)
{
    unsigned short ocvreg = 0;
    unsigned short volreg_offset = 0;
    int ocv = 0;
	int ocvcur = 0;
    unsigned short ocvcurreg = 0;
    unsigned short curreg_offset = 0;
    //int rbatt;
    struct ss_coul_nv_info *pinfo=&di->nv_info;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return;
    }

    SMARTSTAR_REGS_READ(SMARTSTAR_SAVE_OCV_ADDR,&ocvreg,2);

    hwlog_info("ocvreg = 0x%x\n", ocvreg);
    di->batt_ocv_valid_to_refresh_fcc = 1;

    if (ocvreg == (unsigned short)FLAG_USE_CLAC_OCV){
	 	hwlog_info("using calc ocv.\n");
        ocv = get_calc_ocv(di);
        di->batt_ocv_valid_to_refresh_fcc = 0;
        ocvreg = convert_uv2regval(ocv);
        ocvreg = ocvreg | 0x8000;
        hwlog_info("regval to write [ 0x%x ]\n", ocvreg);
        SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR,&ocvreg,2);
        di->batt_ocv_temp = pinfo->hkadc_batt_temp*10;
        di->is_nv_need_save = 1;
    }
    else if (ocvreg==0 || ocvreg==FLAG_USE_PMU_OCV){
        hwlog_info("using pmu ocv.\n");

        SMARTSTAR_REGS_READ(SMARTSTAR_OCV_VOLTAGE_BASE,&ocvreg,2);
        SMARTSTAR_REGS_READ(SMARTSTAR_OFFSET_VOLTAGE,&volreg_offset,2);
        hwlog_info("OCV v=0x%x, v_offset=0x%x\n", ocvreg, volreg_offset);
        ocvreg -= volreg_offset;


        ocv = convert_regval2uv(ocvreg);
        hwlog_info("with offset v=0x%x, OCV BEFORE ADJUST %d \n", ocvreg, ocv);

        SMARTSTAR_REGS_READ(SMARTSTAR_OCV_CURRENT_BASE,&ocvcurreg,2);
        SMARTSTAR_REGS_READ(SMARTSTAR_OFFSET_CURRENT,&curreg_offset,2);
        hwlog_info("OCV c=0x%x, c_offset=0x%x\n", ocvcurreg, curreg_offset);

        ocvcurreg -= curreg_offset;
        hwlog_info("with offset cur=0x%x \n", ocvcurreg);

        ocvcur = convert_regval2ua(ocvcurreg);
        hwlog_info("ocvr=%d\n", ocvcur);

        //ocv = ocv + ocvcur * rbatt/1000;
        ocv = ocv + ocvcur/1000 *(di->r_pcb/1000 + DEFAULT_BATTERY_OHMIC_RESISTANCE);
        hwlog_info("ocv_cur = %d, OCV after adjust %d \n", ocvcur, ocv);
	    ocvreg = convert_uv2regval(ocv);

        hwlog_info("regval to write [ 0x%x ]\n", ocvreg);
        SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR,&ocvreg,2);
    }
    else{
        hwlog_info("using save ocv.\n");
        if (ocvreg & 0x8000){
            ocvreg = ocvreg & ~0x8000;
            di->batt_ocv_valid_to_refresh_fcc = 0;
        }
        ocv = convert_regval2uv(ocvreg);
    }

    if(fastboot_save_nv_info_error()){
        di->batt_ocv_valid_to_refresh_fcc = 0;
        di->batt_ocv_temp = 250;
    }
    else{
        di->batt_ocv_temp = pinfo->ocv_temp;
    }

    di->batt_limit_fcc = pinfo->limit_fcc;

    hwlog_info("ocv = %d uv\n", ocv);

    di->batt_ocv = ocv;
    //di->cc_start_value = 0;
    hwlog_info("initial OCV = %d\n", di->batt_ocv);
}

#define INT_OFFSET 10 /* mv */
/**
* calacute and set the low voltage interrupt value
*/
static void set_low_vol_int_reg(struct smartstar_coul_device *di, int state)
{
    int vol = 0;
    short regval = 0;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return;
    }
    di->v_low_int_value = state;

    if (state == LOW_INT_STATE_RUNNING){
        vol = di->v_cutoff;
    }
    else{
        vol = BATTERY_VOL_2_PERCENT;
    }

    regval = convert_uv2regval(vol*1000);
    udelay(110);
    SMARTSTAR_REGS_WRITE(SMARTSTAR_VOL_INT_BASE, &regval, 2);
}

/*******************************************************
  Function:        set_low_capacity_int_reg
  Description:    set capacity INT level
  Input:            NULL
  Output:          NULL
  Return:          low_level and  shutdown_level
  Remark:        计算的电量soc real与实际显示的电量soc new并不一致
                      考虑在soc new达到warning、shutdown level时使用内核通知链告知
                      上层，而不是靠中断触发
********************************************************/
static void set_low_capacity_int_reg(struct smartstar_coul_device *di)
{
    int capacity = 0;
    unsigned int regval_out = 0;
    int unusable_charge_uah, remaining_charge_uah, delta_rc_uah, rbatt;
    int fcc_uah, cc_uah;
    unsigned int cc_in = 0;
    int cc_uah_in=0;
    int cc_uah_out=0;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return;
    }
    capacity = BATTERY_CC_LOW_LEV;

    SMARTSTAR_REGS_READ(SMARTSTAR_CL_IN_BASE,&cc_in,4);
    cc_uah_in = convert_regval2uah(cc_in);

    calculate_soc_params(di, &fcc_uah, &unusable_charge_uah, &remaining_charge_uah, &cc_uah, &delta_rc_uah, &rbatt);
    cc_uah_out = remaining_charge_uah - fcc_uah * capacity / 100 + cc_uah_in;
    regval_out = convert_uah2regval(cc_uah_out);

    hwlog_info("set low power capacity :level=%d, reg=%d, and soc = %d\n", capacity, regval_out, di->batt_soc);
    SMARTSTAR_REGS_WRITE(SMARTSTAR_CL_INT_BASE, &regval_out, 4);
}

/*******************************************************
  Function:        clear_coul_interrupt
  Description:     whether battery is moved
  Input:           NULL
  Output:          NULL
  Remark:          clear low vol/capacity INT before coul
                   self_adjust when start up
********************************************************/
static void clear_coul_low_interrupt(void)
{
    /* clear low vol and low capacity INT bit */
    char val = 0x0F;
    udelay(110);
    SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_IRQ_REG, val);
}
/*******************************************************
  Function:        is_smartstar_battery_moved
  Description:    whether battery is moved
  Input:            NULL
  Output:          NULL
  Return:          0:not moved, 1: moved
********************************************************/
int is_smartstar_battery_moved(void)
{
    unsigned char val;
    val = SMARTSTAR_REG_READ(SMARTSTAR_BATTERY_MOVE_ADDR);

    if (val == BATTERY_MOVE_MAGIC_NUM){
	 hwlog_info("Battey not moved \n");
        return 0;
    }
    else {
	 hwlog_info("Battey  moved \n");
        udelay(110);
        SMARTSTAR_REG_WRITE(SMARTSTAR_BATTERY_MOVE_ADDR, BATTERY_MOVE_MAGIC_NUM);
        return 1;
    }
}


/**
 * get_battery_id_voltage - get voltage on ID pin by HKADC.
 * @di: stmartstar coul device
 * called in module initalization
 */
static void get_battery_id_voltage(struct smartstar_coul_device *di)
{
    /*change ID get from NTC resistance by HKADC path*/
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return;
    }
    if((int)di->nv_info.hkadc_batt_id_voltage == INVALID_BATT_ID_VOL)
        di->batt_id_vol= 0;
    else
    	di->batt_id_vol = (int)di->nv_info.hkadc_batt_id_voltage;
    hwlog_info("get battery id voltage is %d mv\n",di->batt_id_vol);
}

/**
 * get_battery_id_voltage - get voltage on ID pin by HKADC.
 * @di: stmartstar coul device
 * called in module initalization
 */
static void get_battery_id_voltage_real(struct smartstar_coul_device *di)
{
    int ret = 0;
    short temperature=0;
    unsigned short volt;

    /*change ID get from NTC resistance by HKADC path*/
    extern int DRV_HKADC_GET_TEMP(HKADC_TEMP_PROTECT_E log_chan,
            unsigned short* volt,
            short* temp, HKADC_CONV_MODE_E ulMode);
    temperature = temperature;
    ret = DRV_HKADC_GET_TEMP(HKADC_TEMP_BATT_ID,&volt,&temperature, 0);
    if(ret != 0){ // 0 == HKADC_OK
        hwlog_err("HKADC get battery id fail\n");
        volt = 0;
    }
	di->batt_id_vol = volt;
    hwlog_info("get battery id voltage is %d mv\n",di->batt_id_vol);
}

static int bound_soc(int soc)
{
	soc = max(0, soc);
	soc = min(100, soc);
	return soc;
}
static int hisi_battery_id_vol(void)
{
     struct smartstar_coul_device *di = g_smartstar_coul_dev;


     get_battery_id_voltage_real(di);

     return di->batt_id_vol;
}
/*******************************************************
  Function:        get_fifo_data
  Description:    calculate average of 10 history data
  Input:            NULL
  Output:          NULL
  Return:          avg , max and min cur, vol
********************************************************/
static void get_fifo_data(struct vcdata *vc)
{
    int i;
    short vol_reset_value = 0xffff;
    int abnormal_value_cnt = 0;
    static short vol_fifo[FIFO_DEPTH];
    static short cur_fifo[FIFO_DEPTH];
    int vol,cur;
    int max_cur, min_cur;
    int vols, curs;
    if( NULL == vc )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return;
    }
    for (i=0; i<FIFO_DEPTH; i++) {
        SMARTSTAR_REGS_READ(SMARTSTAR_VOL_FIFO_BASE+i*2, &vol_fifo[i], 2);
        SMARTSTAR_REGS_READ(SMARTSTAR_CUR_FIFO_BASE+i*2, &cur_fifo[i], 2);
    }


    if(vol_fifo[0] != vol_reset_value)
    {
        vol=convert_regval2uv(vol_fifo[0])/1000;
        cur=convert_regval2ua(cur_fifo[0])/1000;
    }
    else
    {
        vol = 0;
        cur = 0;
    }

    vols=vol;
    curs=cur;

    max_cur = min_cur = cur;

    for (i=1; i<FIFO_DEPTH; i++){
        if(vol_fifo[i] != vol_reset_value)
        {
            vol = convert_regval2uv(vol_fifo[i])/1000;
            cur = convert_regval2ua(cur_fifo[i])/1000;

            vols += vol;
            curs += cur;

            if (cur > max_cur){
                max_cur = cur;
            }
            else if (cur < min_cur){
                min_cur = cur;
            }
        }
        else
        {
            abnormal_value_cnt++;
        }
    }

    vol = vols/(FIFO_DEPTH - abnormal_value_cnt);
    cur = curs/(FIFO_DEPTH - abnormal_value_cnt);

    vc->avg_v = vol;
    vc->avg_c = cur;
    vc->max_c = max_cur;
    vc->min_c = min_cur;

    hwlog_info("avg_v = %d, avg_c = %d, max_c = %d, min_c = %d \n", vc->avg_v,
		                             vc->avg_c, vc->max_c, vc->min_c);
}

static int calculate_delta_rc(struct smartstar_coul_device *di, int soc,
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
    unsigned char use_delta_rc_flag = USE_DELTA_RC_SCENE;
    struct vcdata vc = {0};
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
    }
    get_simultaneous_battery_voltage_and_current(di,
                                                &ibat_ua, &vbat_uv);

    get_fifo_data(&vc);

    vc.avg_v += (di->r_pcb/1000)*(vc.avg_c)/1000;

    if (vc.avg_c < 10) {
    	goto out;
    }
    SMARTSTAR_REGS_READ(SMARTSTAR_DELTA_RC_SCENE,&use_delta_rc_flag,1);
    if(IGNORE_DELTA_RC_SCENE == (use_delta_rc_flag & DELTA_RC_SCENE_BITMASK))
    {
        use_delta_rc_flag &= ~DELTA_RC_SCENE_BITMASK;//clear the flag after read
        SMARTSTAR_REGS_WRITE(SMARTSTAR_DELTA_RC_SCENE,&use_delta_rc_flag,1);
        goto out;
    }
    di->last_fifo_iavg_ma = vc.avg_c;

    ocv = interpolate_ocv(di->batt_data->pc_temp_ocv_lut, batt_temp_degc, di->batt_soc_real);

    rbatt_calc = (ocv - vc.avg_v)*1000/vc.avg_c;

    ratio = rbatt_calc*100/rbatt_tbl;

    di->rbatt_ratio = ratio;

    delta_rbatt = rbatt_calc - rbatt_tbl;

    delta_ocv = delta_rbatt*vc.avg_c/1000;

    ocv_new = ocv - delta_ocv;

    pc_new = interpolate_pc(di->batt_data->pc_temp_ocv_lut, batt_temp, ocv_new);

    rc_new_uah = di->batt_fcc/1000 * pc_new;

    delta_pc = pc_new - di->batt_soc_real;

    delta_rc_uah = di->batt_fcc/1000 * delta_pc;

    if (ratio <= 0){
        delta_ocv_100 = -rbatt_tbl*vc.avg_c/1000;
        pc_new_100 = interpolate_pc(di->batt_data->pc_temp_ocv_lut, batt_temp, ocv-delta_ocv_100);
        delta_pc_100 = pc_new_100 - di->batt_soc_real;
        delta_rc_uah_100 = di->batt_fcc/1000 * delta_pc_100;

        delta_rc_final = delta_rc_uah - delta_rc_uah_100;
    }

    soc_new = (rc_new_uah)*100 / (fcc_uah);

    soc_new = bound_soc(soc_new);

out:
    hwlog_info("RBATT_ADJ: soc_new=%d rbat_calc=%d rbat_btl=%d ratio=%d "
                       "c=%d u=%d last_ocv=%d ocv_temp=%d "
                       "soc=%d.%d, ocv=%d "
                       "cmin=%d cmax=%d cavg=%d vavg=%d "
                       "delta_ocv=%d delta_pc=%d.%d delta_rc_uah=%d "
                       "delta_ocv_100=%d delta_pc_100=%d.%d delta_rc_uah_100=%d "
                       "delta_rc_final=%d \n",
                       soc_new, rbatt_calc, rbatt_tbl, ratio,
                       ibat_ua, vbat_uv, di->batt_ocv, di->batt_ocv_temp,
                       di->batt_soc_real/10, di->batt_soc_real%10, ocv,
                       vc.min_c, vc.max_c, vc.avg_c, vc.avg_v,
                       delta_ocv, delta_pc/10, (int)abs(delta_pc%10), delta_rc_uah,
                       delta_ocv_100, delta_pc_100/10, (int)abs(delta_pc_100%10), delta_rc_uah_100,
                       delta_rc_final
                       );

    di->batt_soc_est = soc_new;
    return delta_rc_final;

}

static int adjust_soc(struct smartstar_coul_device *di, int soc,
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
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
    }
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

    /*lint -e665*/
    soc_est_avg = DIV_ROUND_CLOSEST(soc_ests[0]+soc_ests[1]+soc_ests[2], 3);
    /*lint +e665*/

    if (soc_est_avg>2){
        goto out;
    }

    delta_soc = soc - soc_est_avg;

    if (delta_soc <= 0){
        goto out;
    }

    n = 3 - soc_est_avg;

    soc_new = soc - delta_soc*n/3;

    hwlog_info("delta_soc=%d, mark save ocv is invalid\n", delta_soc);
    udelay(110);
    SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR,&ocvreg,2);

out:
    hwlog_info("soc_est_avg=%d delta_soc=%d n=%d\n",
                       soc_est_avg, delta_soc, n);
    soc_new = bound_soc(soc_new);
    return soc_new;
}
/* 电量平滑修正*/
static int limit_soc(struct smartstar_coul_device *di,int input_soc)
{
    int output_soc = input_soc;
    static int power_on_cnt = 0;

    int last_soc = 0;
    int current_ua = 0;
    int voltage_uv = 0;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return -1;
    }
    last_soc = di->batt_soc;
    get_simultaneous_battery_voltage_and_current(di, &current_ua, &voltage_uv);
/*change <=1%*/
    if (di->soc_limit_flag == 1){
	/*soc can not increase during discharging*/
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
    /*exist from sleep*/
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
    /* charge_done, then soc 100% */
    if (di->charging_state == CHARGING_STATE_CHARGE_DONE){
        hwlog_info("pre_chargedone output_soc = %d\n", output_soc);
        output_soc = 100;
    }
    if (di->charging_state == CHARGING_STATE_CHARGE_START &&
        voltage_uv/1000>BATTERY_SWITCH_ON_VOLTAGE &&
        output_soc==0 &&
        (current_ua<-CHARGING_CURRENT_OFFSET || power_on_cnt < 3)
        ){
        output_soc = 1;
    }

    power_on_cnt ++;

    return output_soc;
}

static void calculate_iavg_ma(struct smartstar_coul_device *di, int iavg_ua)
{
       int iavg_ma = iavg_ua / 1000;
       int i;
	static int iavg_samples[IAVG_SAMPLES];
	static int iavg_index = 0;
	static int iavg_num_samples;
       if( NULL == di )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
      	    return;
       }
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
    return;
}

static void calculate_soc_params(struct smartstar_coul_device *di,
						int *fcc_uah,
						int *unusable_charge_uah,
						int *remaining_charge_uah,
						int *cc_uah,
						int *delta_rc_uah,
						int *rbatt)
{
    int soc_rbatt=0, iavg_ua=0;
    int batt_temp = 0;
    int chargecycles = 0;
    static int first_in = 1;
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
   	 return;
    }
    batt_temp = di->batt_temp;
    chargecycles = di->batt_chargecycles/100;
    *fcc_uah = calculate_fcc_uah(di, batt_temp, chargecycles); // calc fcc by cc and soc change

    di->batt_fcc = *fcc_uah;

    /* calculate remainging charge */
    *remaining_charge_uah = calculate_remaining_charge_uah(di,
    				*fcc_uah, chargecycles);

    di->batt_rm = *remaining_charge_uah;

    /* calculate cc micro_volt_hour */
    di->cc_end_value = calculate_cc_uah();
    *cc_uah = di->cc_end_value;

    di->batt_ruc = *remaining_charge_uah - *cc_uah;

    di->get_cc_end_time = get_coul_time();

    /*lint -e665*/
    di->batt_soc_real = DIV_ROUND_CLOSEST((*remaining_charge_uah - *cc_uah), *fcc_uah/1000);
    /*lint +e665*/

    hwlog_info("SOC real = %d\n", di->batt_soc_real);

    soc_rbatt = di->batt_soc_real/10;
    if (soc_rbatt < 0)
    	soc_rbatt = 0;

    *rbatt = get_rbatt(di, soc_rbatt, batt_temp);

#if RBATT_ADJ
    *delta_rc_uah = calculate_delta_rc(di, di->batt_soc_real, di->batt_temp, *rbatt, *fcc_uah);
    di->batt_delta_rc = *delta_rc_uah;
#endif

    if (first_in){
        di->last_cc = di->cc_end_value;
        di->last_time = di->get_cc_end_time;
        iavg_ua = smartstar_battery_current_ua();
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
            iavg_ua = smartstar_battery_current_ua();

        hwlog_info("delta_time=%ds, iavg_ua=%d\n", delta_time, iavg_ua);
    }

    calculate_iavg_ma(di,iavg_ua);

    *unusable_charge_uah = calculate_unusable_charge_uah(di, *rbatt,
    				*fcc_uah, *cc_uah,
    				batt_temp, chargecycles, iavg_ua);
}

static int calculate_state_of_charge(struct smartstar_coul_device *di)
{
    int remaining_usable_charge_uah, fcc_uah, unusable_charge_uah, delta_rc_uah;
    int remaining_charge_uah, soc;
    int cc_uah;
    int rbatt;
    int temp;
	int soc_no_uuc, soc_before_adjust, soc_before_limit;
    //unsigned int eco_leak_uah = 0;

    if( NULL == di )
    {
        hwlog_err("NULL point in [%s]\n", __func__);
   	 return -1;
    }
    if (!di->batt_exist){
        return 0;
    }

    /* check temperature */
    temp = smartstar_battery_temperature_tenth_degree();

    if (temp != di->batt_temp){
        di->batt_temp = temp;
    }

    calculate_soc_params(di,
                                    &fcc_uah,
                                    &unusable_charge_uah,
                                    &remaining_charge_uah,
                                    &cc_uah,
                                    &delta_rc_uah,
                                    &rbatt);
    hwlog_info("BATT_TEMP=%d FCC=%duAh, UUC=%duAh, RC=%duAh, CC=%duAh, delta_RC=%duAh, Rbatt=%dmOhm\n",
                       temp/10, fcc_uah, unusable_charge_uah, remaining_charge_uah, cc_uah, delta_rc_uah, rbatt);
    di->rbatt = rbatt;

    if (di->batt_limit_fcc && di->batt_limit_fcc<fcc_uah){
        fcc_uah = di->batt_limit_fcc;
        hwlog_info("use limit_FCC! %duAh\n", fcc_uah);
    }

    	/*lint -e665*/
	soc = DIV_ROUND_CLOSEST((remaining_charge_uah - cc_uah), fcc_uah/100);
	/*lint -e665*/

	soc_no_uuc = soc;
	di->batt_soc_with_uuc = soc;

    /* calculate remaining usable charge */
    //eco_leak_uah = calculate_eco_leak_uah();

	/* 退出ECO模式后 */
    //remaining_charge_uah = remaining_charge_uah - eco_leak_uah;

    remaining_usable_charge_uah = remaining_charge_uah
                                                - cc_uah - unusable_charge_uah + delta_rc_uah;

    if (fcc_uah - unusable_charge_uah <= 0) {
    	soc = 0;
    } else {
       /*lint -e665*/
    	soc = DIV_ROUND_CLOSEST((remaining_usable_charge_uah),
    				(fcc_uah - unusable_charge_uah)/100);
	/*lint +e665*/

    }
    if (soc > 100)
    	soc = 100;
	soc_before_adjust = soc;
    soc= adjust_soc(di, soc, di->batt_temp, di->batt_chargecycles/100, rbatt, fcc_uah, unusable_charge_uah, cc_uah);
	soc_before_limit = soc;
    hwlog_info("SOC without UUC = %d, SOC before adjust = %d, SOC before limit = %d\n",soc_no_uuc, soc_before_adjust, soc_before_limit);
    /*not exiting from ECO Mode capacity can not change more than 1%*/
    if( 0 == g_eco_leak_uah)
    {
        soc = limit_soc(di, soc);
	    hwlog_info("NOT EXIST FROM ECO, CHANGE <= 1, SOC_NEW = %d\n",soc);
    }
    else
    {
	 	hwlog_info("EXIST FROM ECO, CHANGE CAN BE BIGGER 1, SOC_NEW = %d\n",soc);
		g_eco_leak_uah = 0;
    }
	/*default is no battery in sft and udp, so here soc is fixed 20 to prevent low power reset*/
	if ( BAT_BOARD_ASIC != is_board_type) {
		if (soc < 20)
			soc = 20;
		hwlog_err("SFT and udp board: adjust Battery Capacity to %d Percents\n", soc);
	}
    di->batt_soc = soc;

    return soc;
}

static void cali_adc(void)
{
	unsigned char reg_val = 0;
    reg_val = SMARTSTAR_REG_READ(SMARTSTAR_COUL_STATE_REG);

    if (COUL_CALI_ING == reg_val){
    	hwlog_info("cali ing, don't do it again!\n");
        return;
    }
	hwlog_info("calibrate!\n");
	reg_val = SMARTSTAR_REG_READ(SMARTSTAR_COUL_CTRL_REG);
	/* Mode */
	reg_val = reg_val | COUL_CALI_ENABLE;
    udelay(110);
	SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_CTRL_REG, reg_val);
}

void print_offset(void)
{
	unsigned short volreg_offset = 0;
    unsigned short curreg_offset = 0;
	SMARTSTAR_REGS_READ(SMARTSTAR_OFFSET_VOLTAGE, &volreg_offset, 2);
	SMARTSTAR_REGS_READ(SMARTSTAR_OFFSET_CURRENT, &curreg_offset, 2);
	hwlog_info("calibrated, volreg_offset=%d, curreg_offset=%d\n",
					   volreg_offset, curreg_offset);
}

#define CHARGED_OCV_UPDATE_INTERVAL (10*60*1000)

#ifndef _DRV_LLT_
    extern int hi6521_set_charge_state(int state);
#else
    int hi6521_set_charge_state(int state)
	{
		return state;
	}
#endif

static void calc_initial_ocv(struct smartstar_coul_device *di)
{
    short reg;
    int old_charge_state;

    old_charge_state = hi6521_set_charge_state(0);

    cali_adc();

    mdelay(2500); // 2.2s for calibration, 0.11s for sampling, and 0.19s for pad

    di->batt_ocv = smartstar_battery_voltage()*1000;

    reg = convert_uv2regval(di->batt_ocv);

    reg &= 0x8000;

    udelay(110);
    SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR, &reg ,2);

    hi6521_set_charge_state(old_charge_state);

    clear_cc_register();
    clear_coul_time();

    hwlog_info("OCV = %d\n", di->batt_ocv);

}

static void battery_plug_in(struct smartstar_coul_device *di)
{
    char val;

    hwlog_info("%s: Enter\n",__FUNCTION__);

    if(NULL == di)
    {
        hwlog_info(KERN_ERR "[%s]di is null.\n",__FUNCTION__);
        return;
    }

    di->batt_exist = 1;

    /*set battery data*/
    get_battery_id_voltage_real(di);

    di->batt_data = get_battery_data(di->batt_id_vol, di->product_index);
    if(di->batt_data != NULL)
    {
        hwlog_info("%s: batt ID is %d, batt_brand is %s\n",__FUNCTION__,di->batt_id_vol, di->batt_data->batt_brand);
    }else{
        hwlog_err("%s: %d di->batt_data is NULL  \n", __func__, __LINE__);
        return;
    }
    di->batt_temp = smartstar_battery_temperature_tenth_degree();
    if(di->batt_temp != NULL)
    {
    	hwlog_info("battery temperature is %d.%d\n", di->batt_temp/10, di->batt_temp%10);
    }
    /*calculate first soc */
    calc_initial_ocv(di);

    di->charging_stop_time = get_coul_time();

    di->last_iavg_ma = IMPOSSIBLE_IAVG;
    di->prev_pc_unusable = -EINVAL;

    di->sr_resume_time = get_coul_time();
    sr_cur_state = SR_DEVICE_WAKEUP;

    di->batt_chargecycles = 0;
    di->batt_changed_flag = 1;
    di->batt_limit_fcc = 0;
    di->adjusted_fcc_temp_lut = NULL;
    di->is_nv_need_save = 1;
    val = SMARTSTAR_REG_READ(SMARTSTAR_NV_SAVE_SUCCESS);
    udelay(110);
    SMARTSTAR_REG_WRITE(SMARTSTAR_NV_SAVE_SUCCESS, (val & (~NV_SAVE_BITMASK)));
    hwlog_info("new battery plug in, reset chargecycles!\n");

    /*get the first soc value*/
    DI_LOCK();
    di->soc_limit_flag = 0;
    di->batt_soc = calculate_state_of_charge(di);
    di->soc_limit_flag = 1;
    DI_UNLOCK();

    set_low_vol_int_reg(di, LOW_INT_STATE_RUNNING);

    /*schedule calculate_soc_work*/
    schedule_delayed_work(&di->calculate_soc_delayed_work,
                        round_jiffies_relative(msecs_to_jiffies(di->soc_work_interval)));

    // save battery move magic number
    udelay(110);
    SMARTSTAR_REG_WRITE(SMARTSTAR_BATTERY_MOVE_ADDR, BATTERY_MOVE_MAGIC_NUM);

    blocking_notifier_call_chain(&notifier_list, BATTERY_MOVE, NULL);//hisi_coul_charger_event_rcv(evt);

    hwlog_info("%s: Exit\n",__FUNCTION__);

}

static void battery_plug_out(struct smartstar_coul_device *di)
{
    short reg;

    hwlog_info("%s: Enter\n",__FUNCTION__);

    di->batt_exist = 0;

    blocking_notifier_call_chain(&notifier_list, BATTERY_MOVE, NULL);//hisi_coul_charger_event_rcv(evt);

    cancel_delayed_work(&di->calculate_soc_delayed_work);

    // save battery move magic number
    udelay(110);
    SMARTSTAR_REG_WRITE(SMARTSTAR_BATTERY_MOVE_ADDR, 0x18);

    // clear saved ocv
    reg = 0;
    udelay(110);
    SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR, &reg ,2);

    hwlog_info("%s: Exit\n",__FUNCTION__);

}

static void battery_check_work(struct work_struct *work)
{
    struct smartstar_coul_device *di = container_of(work,
				struct smartstar_coul_device,
				battery_check_delayed_work.work);

    int batt_exist = is_smartstar_battery_exist();

    if (batt_exist != di->batt_exist){
        if (batt_exist){
            battery_plug_in(di);
        }
        else{
            battery_plug_out(di);
        }
    }

    schedule_delayed_work(&di->battery_check_delayed_work,
                round_jiffies_relative(msecs_to_jiffies(BATTERY_CHECK_TIME_MS)));
}
 /*******************************************************
  Function:        calculate_soc_work
  Description:    calculate soc every(schedule workqueue) CALCULATE_SOC_MS
  Input:            struct work_struct *work
  Output:          NULL
  Return:          NULL
********************************************************/
 static void calculate_soc_work(struct work_struct *work)
{
    struct smartstar_coul_device *di = container_of(work,
				struct smartstar_coul_device,
				calculate_soc_delayed_work.work);

    static int cali_cnt = 0;
    static int charged_cnt = 0;
    static int last_cc=0;
    static int last_time=0;
    int evt;
    int ret = -1;
    short offset_cur_modify_val = 0;

    if( NULL == di || NULL== work)
    {
        hwlog_err("NULL point in [%s]\n", __func__);
   	 	return;
    }
    if(di->is_nv_need_save){
        ret = save_nv_info(di);
        if(!ret)
        {
            di->is_nv_need_save = 0;
        }
    }

    if (!di->batt_exist){
    	hwlog_info("battery not exist, do not calc soc any more\n");
        return;
    }

    SMARTSTAR_REGS_READ(SMARTSTAR_COUL_OFFSET_CUR_MODIFY_BASE, &offset_cur_modify_val, 2);
    hwlog_info("curexception, offset_cur_modify_val:0x%x\n", offset_cur_modify_val);
    if(0 != offset_cur_modify_val)
    {
        hwlog_err("curexception, offset_cur_modify_val:0x%x\n", offset_cur_modify_val);
    }

    DI_LOCK();
    /* calc soc */
    di->batt_soc = calculate_state_of_charge(di);

   cali_cnt++;
   if (cali_cnt % (CALIBRATE_INTERVAL / di->soc_work_interval) == 0)
   {
           cali_adc();
   }
   else if (cali_cnt % (CALIBRATE_INTERVAL / di->soc_work_interval) == 1)
   {
           print_offset();
   }

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

        	hwlog_info("sleep_cc=%d, sleep_time=%d\n", sleep_cc, sleep_time);

            if (sleep_time <= 0){
                charged_cnt --;
            }
            else {
            	sleep_current = (sleep_cc * 18) / (sleep_time * 5);  /* 18 5 means what */

                if(sleep_current<0){
                    sleep_current = -sleep_current;
                }

            	hwlog_info("sleep_current=%d\n", sleep_current);

                if (sleep_current < 20){
                    get_ocv_by_vol(di);
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
/* work faster when capacity <3% */
    if (di->batt_soc>30){
        di->soc_work_interval = CALCULATE_SOC_MS;
    }
    else{
        di->soc_work_interval = CALCULATE_SOC_MS/2;
    }
	if ( BAT_BOARD_ASIC != is_board_type){

		di->soc_work_interval = CALCULATE_SOC_MS*10;
	}

    if(di->batt_soc <= BATTERY_CC_LOW_LEV)
    {
		evt = BATTERY_LOW_SHUTDOWN;
		hwlog_info("SMARTSTAR SHUTDOWN SOC LEVEL\n");
		blocking_notifier_call_chain(&notifier_list, evt, NULL);


	}
    schedule_delayed_work(&di->calculate_soc_delayed_work,
    		round_jiffies_relative(msecs_to_jiffies(di->soc_work_interval)) );

}

 /*******************************************************
  Function:        make_cc_no_overload
  Description:    update coulomb start value
  Input:            struct smartstar_coul_device *di    ----coul device
  Output:          NULL
  Return:          NULL
********************************************************/
static void make_cc_no_overload(struct smartstar_coul_device *di)
{
	int cc;
	if( NULL == di )
       {
           hwlog_info("NULL point in [%s]\n", __func__);
      	 return;
       }
	cc = calculate_cc_uah();
    save_cc_uah(cc);

}

/*******************************************************
  Function:         smartstar_irq_enable
  Description:     enable coul irq when charge stop or finish
  Input:             struct smartstar_coul_device *di   -------coul device
  Output:          NULL
  Return:          NULL
********************************************************/
 static void smartstar_irq_enable(void)
 {
	char irq_enable_flag = 0x0;
	char val;
	val = SMARTSTAR_REG_READ(SMARTSTAR_COUL_IRQ_REG);
    udelay(110);
	SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_IRQ_REG, val);
	SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_IRQ_MASK_REG, irq_enable_flag);
	hwlog_info("Enable smartstar coul irq\n");
 }
/*******************************************************
  Function:        smartstar_irq_enable
  Description:    disable coul irq when charge stop or finish
  Input:             struct smartstar_coul_device *di   -------coul device
  Output:          NULL
  Return:          NULL
********************************************************/
 static void smartstar_irq_disable(void)
 {
     char irq_disable_flag = 0x0f;
     udelay(110);
     SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_IRQ_MASK_REG, irq_disable_flag);
     hwlog_info("Disable smartstar coul irq\n");
 }
void smartstar_get_battery_vol_from_fifo(int *vbat_uv)
{
    short regval = 0;
    SMARTSTAR_REGS_READ(SMARTSTAR_VOL_FIFO_BASE,&regval,2);
	*vbat_uv = convert_regval2uv(regval);

}

/*******************************************************
  Function:        interrupt_notifier_work
  Description:     interrupt_notifier_work - send a notifier event to battery monitor.
  Input:           struct work_struct *work           ---- work queue
  Output:          NULL
  Return:          NULL
  Remark:          capacity INT : low level and shutdown level.
                   need to check by Test
********************************************************/
 static void interrupt_notifier_work(struct work_struct *work)
{
    struct smartstar_coul_device *di = container_of(work,
				struct smartstar_coul_device,
				notifier_work.work);
	unsigned char intstat = 0;
	int ibat_ua = 0, vbat_uv = 0;
    int delta_soc = 0;

    if( NULL == di || NULL== work)
    {
	  	hwlog_info("NULL point in [%s]\n", __func__);
     	return;
    }
    intstat = di->irq_mask;
	coul_reg_show();
	if (di->v_low_int_value == LOW_INT_STATE_SLEEP){
		get_simultaneous_battery_voltage_and_current(di, &ibat_ua, &vbat_uv);
		if ((vbat_uv/1000 -INT_OFFSET) < BATTERY_VOL_2_PERCENT){
			hwlog_info("IRQ: low_vol_int current_vol is [%d] < 3350,use fifo vol!\n",vbat_uv/1000);
			//smartstar_get_battery_vol_from_fifo(&vbat_uv);
		}
	}
	else{
		get_simultaneous_battery_voltage_and_current(di, &ibat_ua, &vbat_uv);
		if ((vbat_uv/1000 -INT_OFFSET) < di->v_cutoff){
			hwlog_info("IRQ: low_vol_int current_vol is [%d] < [%d],use fifo vol!\n",vbat_uv/1000,di->v_cutoff);
			//smartstar_get_battery_vol_from_fifo(&vbat_uv);
		}
	}

	if (intstat & COUL_VBAT_LOW_INT)
	{
		if (di->batt_exist){
			hwlog_info("IRQ: COUL_VBAT_LOW_INT, vbat=%duv, last vbat_int=%d, fifo0_out_eco =%d,fifo0_in_eco = %d\n",
										vbat_uv, di->v_low_int_value ,saved_vol_fifo0_out_eco,saved_vol_fifo0_in_eco);
		}
		else{
            hwlog_err("IRQ: COUL_VBAT_LOW_INT, no battery, error\n");
            return;
       }

        if(-1 != vbat_uv)
		{
           vbat_uv /= 1000;
		}
		/*if BATTERY vol it too low ,it's false ,return*/
		if ((vbat_uv < BATTERY_VOL_LOW_ERR) && (-1 !=vbat_uv)){
			di->irq_mask &= ~COUL_VBAT_LOW_INT;
			hwlog_err("Battery vol too low,low_vol_irq is't ture!\n");
            /*false low vol int ,next suspend don't cali*/
            is_cali_flag = 0;
			return;
		}

        if (di->v_low_int_value == LOW_INT_STATE_SLEEP){
			if (((vbat_uv -INT_OFFSET) > BATTERY_VOL_2_PERCENT)
				  || ((-1 == vbat_uv) && (saved_vol_fifo0_out_eco > BATTERY_VOL_2_PERCENT))){
				hwlog_err("false low_int,in sleep!\n");
				di->irq_mask &= ~COUL_VBAT_LOW_INT;
                /*false low vol int ,next suspend don't cali*/
                is_cali_flag = 0;
				return;
			} else {
			    mdelay(120);
                get_simultaneous_battery_voltage_and_current(di, &ibat_ua, &vbat_uv);
                hwlog_err("delay 120ms get vbat_uv is %duv!\n",vbat_uv);
                coul_reg_show();
                if ((vbat_uv/1000 -INT_OFFSET) > BATTERY_VOL_2_PERCENT){
                    di->irq_mask &= ~COUL_VBAT_LOW_INT;
                    hwlog_err("fifo0 is false,it's got in 32k clk period!\n");
                    /*false low vol int ,next suspend don't cali*/
                    is_cali_flag = 0;
    				return;
                }
				delta_soc = di->batt_soc - 2;
                di->batt_soc = 1;
			}
        }
        else if (di->v_low_int_value == LOW_INT_STATE_RUNNING){
			if (((vbat_uv -INT_OFFSET) > di->v_cutoff)
				  || ((-1 == vbat_uv) && (saved_vol_fifo0_in_eco > di->v_cutoff))){
				hwlog_err("false low_int,in running!\n");
				di->irq_mask &= ~COUL_VBAT_LOW_INT;
                /*false low vol int ,next suspend don't cali*/
                is_cali_flag = 0;
				return;

			} else {
			    mdelay(120);
                get_simultaneous_battery_voltage_and_current(di, &ibat_ua, &vbat_uv);
                hwlog_err("delay 120ms get vbat_uv is %duv!\n",vbat_uv);
                coul_reg_show();
                if ((vbat_uv/1000 -INT_OFFSET) > di->v_cutoff){
                    di->irq_mask &= ~COUL_VBAT_LOW_INT;
                    hwlog_err("fifo0 is false,it's got in 32k clk period!\n");
                    /*false low vol int ,next suspend don't cali*/
                    is_cali_flag = 0;
    				return;
                }
                delta_soc = di->batt_soc;
                di->batt_soc = 0;
			}
        }

        if (delta_soc > 1){
            short ocvreg = 0;
            hwlog_info("delta_soc=%d, mark save ocv is invalid\n", delta_soc);
            udelay(110);
            SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR,&ocvreg,2);
        }
		blocking_notifier_call_chain(&notifier_list, BATTERY_LOW_SHUTDOWN, NULL);

        di->irq_mask &= ~COUL_VBAT_LOW_INT;
	}

	if (intstat & COUL_CCOUT_BIG_INT)
	{
	      hwlog_info("IRQ: COUL_CCOUT_BIG_INT\n");
             di->irq_mask &= ~COUL_CCOUT_BIG_INT;
#if  0 // defined(CHIP_BB_HI6210)			can not work: soc real is not the same with soc new
		   DI_LOCK();
	       di->batt_soc = calculate_state_of_charge(di);
	       DI_UNLOCK();
	       if(di->batt_soc <= BATTERY_CC_LOW_LEV)         /* shutdown */
	       {
	           evt = BATTERY_LOW_SHUTDOWN;
	       }
		blocking_notifier_call_chain(&notifier_list, evt, NULL);//hisi_coul_charger_event_rcv(evt);
#endif
	}
	if (intstat & COUL_CCIN_CNT_INT)
	{
		hwlog_info("IRQ: COUL_CCIN_CNT_INT\n");
		make_cc_no_overload(di);
              di->irq_mask &= ~COUL_CCIN_CNT_INT;
	}

	if (intstat & COUL_CCOUT_CNT_INT)
	{
		hwlog_info("IRQ: COUL_CCOUT_CNT_INT\n");
		make_cc_no_overload(di);
              di->irq_mask &= ~COUL_CCOUT_CNT_INT;
	}

}

/*******************************************************
  Function:        coul_irq_cb
  Description:    coul irq handler
  Input:            int irq                 ---- irq number
                       void *_di            ---- coul device
  Output:          NULL
  Return:          IRQ_NONE: irq not exist,  IRQ_HANDLED: be handled
********************************************************/
static irqreturn_t coul_irq_cb(int irq,  void *_di)
{
    struct smartstar_coul_device *di = _di;
    int i;

    hwlog_info("coul_irq_cb irq=%d\n", irq);

    for (i=0; i<IRQ_MAX; i++){
        if (di->irqs[i] == irq){
            break;
        }
    }

    if (i == IRQ_MAX){
        hwlog_err("IRQ[%d], not know!\n", irq);
        return IRQ_NONE;
    }

    di->irq_mask |= 1<<i;

    schedule_delayed_work(&di->notifier_work, msecs_to_jiffies(0));

    return IRQ_HANDLED;
}

/*******************************************************
  Function:        smartstar_charging_begin
  Description:    be called when charge begin, update charge status,
                       calc soc, begin cc,  can't be called in atomic context
  Input:            struct smartstar_coul_device *di                 ---- coul device
  Output:          NULL
  Return:          NULL
********************************************************/
static void smartstar_charging_begin (struct smartstar_coul_device *di)
{
    hwlog_info("smartstar_charging_begin +\n");
    if( NULL == di )
    {
        hwlog_info("NULL point in [%s]\n", __func__);
     	 return;
     }
    hwlog_info("pre charging state is %d \n",di->charging_state);
    /* disable coul irq */
    //smartstar_irq_disable();
    if (di->charging_state == CHARGING_STATE_CHARGE_START)
        return;

    di->charging_state = CHARGING_STATE_CHARGE_START;

    /*calculate soc again*/
    di->batt_soc = calculate_state_of_charge(di);

    /*record soc of charging begin*/
    di->charging_begin_soc = di->batt_soc_real;

    /*record cc value*/
    di->charging_begin_cc = calculate_cc_uah();

    hwlog_info("smartstar_charging_begin -\n");
    hwlog_info("batt_soc=%d, charging_begin_soc=%d, charging_begin_cc=%d\n",
                       di->batt_soc, di->charging_begin_soc, di->charging_begin_cc);

}

/*******************************************************
  Function:        smartstar_charging_stop
  Description:     be called when charge stop, update charge status, update chargecycles
                   calc soc, cc, rm and set low vol reg. can't be called in atomic context
  Input:           struct smartstar_coul_device *di                 ---- coul device
  Output:          NULL
  Return:          NULL
********************************************************/
static void smartstar_charging_stop (struct smartstar_coul_device *di)
{
    int rm, cc;
    int fcc_101 = 0;
    if( NULL == di )
    {
        hwlog_err("NULL point in [%s]\n", __func__);
     	 return;
    }
    fcc_101 = di->batt_fcc*101/100;
    if (CHARGING_STATE_CHARGE_UNKNOW == di->charging_state){
        return;
    }
    /* enable coul irq */
    smartstar_irq_enable();
    di->batt_soc = calculate_state_of_charge(di);

    if (CHARGING_STATE_CHARGE_START == di->charging_state){
        update_chargecycles(di);
        di->is_nv_need_save = 1;
    }

    cc = calculate_cc_uah();


    rm = di->batt_rm - cc;
/* adjust rm */
    if (rm > fcc_101){
        cc = cc + (rm-fcc_101);
        save_cc_uah(cc);
        di->batt_limit_fcc = 0;
        di->is_nv_need_save = 1;
    }
    else if (di->batt_soc == 100 && di->batt_soc_real > 950){
        di->batt_limit_fcc = rm*100/101;
        di->is_nv_need_save = 1;
    }


    di->charging_state = CHARGING_STATE_CHARGE_STOP;
    di->charging_stop_time = get_coul_time();
    set_low_vol_int_reg(di, LOW_INT_STATE_RUNNING);
    /* set shutdown level */
    set_low_capacity_int_reg(di);
}

/*******************************************************
  Function:        calculate_real_fcc_uah
  Description:    calc fcc by cc_change /soc_change
  Input:            struct smartstar_coul_device *di                 ---- coul device
                       int *ret_fcc_uah   ----------real fcc
  Output:          ret_fcc_uah    ----------real fcc
  Return:          real_fcc
********************************************************/
static int calculate_real_fcc_uah(struct smartstar_coul_device *di,
								  int *ret_fcc_uah)
{
    int fcc_uah, unusable_charge_uah, delta_rc;
    int remaining_charge_uah;
    int cc_uah;
    int real_fcc_uah;
    int rbatt;
    if( NULL == di )
    {
        hwlog_err("NULL point in [%s]\n", __func__);
     	 return -1;
    }
    calculate_soc_params(di,
    					 &fcc_uah,
    					 &unusable_charge_uah,
    					 &remaining_charge_uah, &cc_uah, &delta_rc, &rbatt);
    real_fcc_uah = (-(cc_uah - di->charging_begin_cc))/(1000 - di->charging_begin_soc)*1000;
    //real_fcc_uah = remaining_charge_uah - cc_uah;
    //real_fcc_uah = real_fcc_uah*100/101;
    *ret_fcc_uah = fcc_uah;
    hwlog_info("real_fcc=%d, RC=%d CC=%d fcc=%d charging_begin_soc=%d.%d\n",
    				   real_fcc_uah, remaining_charge_uah, cc_uah, fcc_uah, di->charging_begin_soc/10, di->charging_begin_soc);
    return real_fcc_uah;
}

/*******************************************************
  Function:        readjust_fcc_table
  Description:    fcc self_study, establish a temp_fcc lookup table
  Input:            struct smartstar_coul_device *di                 ---- coul device
  Output:          a temp_fcc lookup table
  Return:          NULL
********************************************************/
static void readjust_fcc_table(struct smartstar_coul_device *di)
{
	struct single_row_lut *temp, *now;
	int i, fcc, ratio;
	int real_fcc_mah;
	if( NULL == di )
       {
           hwlog_err("NULL point in [%s]\n", __func__);
        	 return;
       }
	real_fcc_mah = di->fcc_real_mah;  /* calc by delt_cc/delt_soc */
	if (!di->batt_data->fcc_temp_lut)
	{
		hwlog_err("%s The static fcc lut table is NULL\n", __func__);
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
		temp->x[i] = now->x[i];
		ratio = div_u64(((u64)(now->y[i]) * 1000), fcc);
		temp->y[i] = (ratio * real_fcc_mah);
		temp->y[i] /= 1000;
		hwlog_info("temp=%d, staticfcc=%d, adjfcc=%d, ratio=%d\n",
						   temp->x[i], now->y[i],
						   temp->y[i], ratio);
	}
	di->adjusted_fcc_temp_lut = temp;
}

/*******************************************************
  Function:        refresh_fcc
  Description:    fcc self_study, check learning condition when charge done and
                       call readjust_fcc_table to establish a temp_fcc lookup table
  Input:            struct smartstar_coul_device *di                 ---- coul device
  Output:          a temp_fcc lookup table
  Return:          NULL
********************************************************/
void refresh_fcc(struct smartstar_coul_device *di)
{
       if( NULL == di )
       {
           hwlog_err("NULL point in [%s]\n", __func__);
        	 return;
       }
	if (di->charging_begin_soc/10 < MIN_BEGIN_PERCENT_FOR_LEARNING
        && di->batt_ocv_valid_to_refresh_fcc
	    && ((di->batt_ocv>3200000 && di->batt_ocv<3670000)
            || (di->batt_ocv>3690000 && di->batt_ocv <3730000)
            || (di->batt_ocv>3800000 && di->batt_ocv <3900000)
            )
        )
    {
        int fcc_uah, fcc_tbl_uah, new_fcc_uah, delta_fcc_uah, max_delta_fcc_uah;

        new_fcc_uah = calculate_real_fcc_uah(di, &fcc_uah);
        fcc_tbl_uah = interpolate_fcc(di, di->batt_temp)*1000;
        
        max_delta_fcc_uah = fcc_tbl_uah*DELTA_FCC_PERCENT/100;
        delta_fcc_uah = new_fcc_uah - fcc_tbl_uah;
        if (delta_fcc_uah < 0)
            delta_fcc_uah = -delta_fcc_uah;
        if (delta_fcc_uah > max_delta_fcc_uah)
        {
            /* new_fcc_uah is outside the scope limit it */
            if (new_fcc_uah > fcc_tbl_uah)
                new_fcc_uah = (fcc_tbl_uah + max_delta_fcc_uah);
            else
                new_fcc_uah = (fcc_tbl_uah - max_delta_fcc_uah);
            hwlog_info("delta_fcc=%d > %d percent of fcc=%d"
                "restring it to %d\n",
                delta_fcc_uah, DELTA_FCC_PERCENT,
                fcc_tbl_uah, new_fcc_uah);
        }
        di->fcc_real_mah = new_fcc_uah / 1000;
        hwlog_info("refresh_fcc, start soc=%d, new fcc=%d, fcc_tbl_uah=%d,  delta_fcc_uah=%d, max_delta_fcc_uah=%d\n",
            di->charging_begin_soc, di->fcc_real_mah, fcc_tbl_uah, delta_fcc_uah, max_delta_fcc_uah);
        /* update the temp_fcc lookup table */
        readjust_fcc_table(di);
    }
}

/*******************************************************
  Function:        smartstar_charging_done
  Description:    be called when charge finish, update charge status, chargecycles
                       calc soc(100%), OCV. can't be called in atomic context
                       refresh_fcc if can be
  Input:            struct smartstar_coul_device *di                 ---- coul device
  Output:          NULL
  Return:          NULL
********************************************************/
static void smartstar_charging_done (struct smartstar_coul_device *di)
{
    int rm = 0;
    if( NULL == di )
    {
        hwlog_err("NULL point in [%s]\n", __func__);
     	 return;
    }
    if (CHARGING_STATE_CHARGE_START != di->charging_state) {
        hwlog_err("smartstar_charging_done, but pre charge state is %d \n",
                            di->charging_state);
        return;
    }
    /* enable coul irq */
    smartstar_irq_enable();
    refresh_fcc(di);
    smartstar_get_rm(di, &rm);
    di->batt_limit_fcc = rm*100/101;
    hwlog_info("smartstar_charging_done, adjust soc from %d to 100\n",di->batt_soc);

    di->batt_soc = 100;

    if (di->batt_changed_flag)
    {
        /*recalculate charge cycles*/
        recalc_chargecycles(di);
    }

    get_ocv_by_fcc(di, di->batt_temp);

    di->batt_fcc  = calculate_fcc_uah(di, di->batt_temp, di->batt_chargecycles/100);
    //di->cc_start_value = -di->batt_fcc/100;

    update_chargecycles(di);

    di->is_nv_need_save = save_nv_info(di);

    di->charging_state = CHARGING_STATE_CHARGE_DONE;

	/* set shutdown level */
	set_low_capacity_int_reg(di);

    hwlog_info("new charging cycles = %d%%\n", di->batt_chargecycles);
}

/*******************************************************
  Function:        charger_event_process
  Description:    charge event distribution function
  Input:            struct smartstar_coul_device *di                 ---- coul device
                       unsigned int event                                  ---- charge event
  Output:          NULL
  Return:          NULL
********************************************************/
static void charger_event_process(struct smartstar_coul_device *di,unsigned int event)
{
    if( NULL == di )
    {
        hwlog_err("NULL point in [%s]\n", __func__);
     	 return;
    }
    switch (event) {
    	case VCHRG_START_USB_CHARGING_EVENT:
    	case VCHRG_START_AC_CHARGING_EVENT:
    	case VCHRG_START_CHARGING_EVENT:
            hwlog_info("receive charge start event = 0x%x\n",(int)event);
            /*record soc and cc value*/
            DI_LOCK();
            smartstar_charging_begin(di);
            DI_UNLOCK();
    		break;

    	case VCHRG_STOP_CHARGING_EVENT:
            hwlog_info("receive charge stop event = 0x%x\n",(int)event);
            DI_LOCK();
    	    smartstar_charging_stop(di);
    	    DI_UNLOCK();
    	    break;

    	case VCHRG_CHARGE_DONE_EVENT:
            hwlog_info("receive charge done event = 0x%x\n",(int)event);
            DI_LOCK();
    	    smartstar_charging_done(di);
    	    DI_UNLOCK();
    		break;

    	case VCHRG_NOT_CHARGING_EVENT:
    	    di->charging_state = CHARGING_STATE_CHARGE_NOT_CHARGE;
            hwlog_err("charging is stop by fault\n");
    	    break;

    	case VCHRG_POWER_SUPPLY_OVERVOLTAGE:
    	    di->charging_state = CHARGING_STATE_CHARGE_NOT_CHARGE;
            hwlog_err("charging is stop by overvoltage\n");
    		break;

    	case VCHRG_POWER_SUPPLY_WEAKSOURCE:
    	    di->charging_state = CHARGING_STATE_CHARGE_NOT_CHARGE;
            hwlog_err("charging is stop by weaksource\n");
    		break;

    	default:
            di->charging_state = CHARGING_STATE_CHARGE_NOT_CHARGE;
    	    hwlog_err("unknow event %d\n",(int)event);
    		break;
    }
}


/*******************************************************
  Function:        smartstar_battery_charger_event_rcv
  Description:    package charger_event_process, and be registered in scharger Model
                       to get charge event
  Input:            unsigned int event                                  ---- charge event
  Output:          NULL
  Return:          NULL
********************************************************/
int smartstar_battery_charger_event_rcv (unsigned int evt)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;
    if( NULL == di )
    {
        hwlog_err("NULL point in [%s]\n", __func__);
     	 return -1;
    }
    if (!di || !di->batt_exist){
        return 0;
    }
    if (!is_smartstar_battery_exist()){
        return 0;
    }

    charger_event_process(di,evt);

    return 0;
}

/*******************************************************
  Function:        smartstar_coul_config_power_on_reg
  Description:    config coul power on by write reg
  Input:            struct smartstar_coul_device *di                 ---- cou device
  Output:          NULL
  Return:          NULL
********************************************************/
static void smartstar_coul_config_power_on_reg(struct smartstar_coul_device *di)
{
    /* clear low INT before coul self_adjust */
    clear_coul_low_interrupt();
    smartstar_irq_enable();

    /* Mode */
    udelay(110);
    SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_CTRL_REG,DEFAULT_COUL_CTRL_VAL);

    /* coulometer interrupt mask */
    SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_IRQ_MASK_REG,DEFAULT_INT_MASK_VAL);
}


/*******************************************************
  Function:        smartstar_disable_coul
  Description:    config coul power down if register failed
  Input:            struct smartstar_coul_device *di                 ---- cou device
  Output:          NULL
  Return:          NULL
********************************************************/
static void smartstar_disable_coul(struct smartstar_coul_device *di)
{
    /* Mode */
    udelay(110);
    SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_CTRL_REG, (~COUL_CTRL_ENABLE) );
}


static ssize_t smartstar_show_gaugelog(struct device_driver *driver, char *buf)
{
    int uf_temp = 0, temp = 0, voltage = 0, ufcapacity = 0, capacity = 100, afcapacity = 0, rm = 0, fcc = 0, delta_rc = 0;
    int cur = 0,uuc = 0,cc = 0, ocv=0, rbatt;

    //struct bq27510_device_info* di = g_battery_measure_by_bq27510_device;
	struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if(NULL == buf){
        return -1;
    }

    if (di == NULL)
        return sprintf(buf, "%s", "Smartstar coulometer probe failed!");

    uf_temp = smartstar_battery_uf_temperature();
    temp =  smartstar_battery_temperature();
    voltage = smartstar_battery_voltage();
    cur = -(smartstar_battery_current());
    ufcapacity = smartstar_battery_unfiltered_capacity();
    capacity = smartstar_battery_capacity();
#ifndef _DRV_LLT_
	afcapacity = bq_bci_show_capacity();
#else
	afcapacity = smartstar_battery_capacity();
#endif
    rm =   smartstar_battery_rm();
    fcc =  smartstar_battery_fcc();
    uuc = smartstar_battery_uuc();
    cc = smartstar_battery_cc();
    delta_rc = smartstar_battery_delta_rc();
    ocv = smartstar_battery_ocv();
    rbatt = smartstar_battery_resistance();

    sprintf(buf, "%-6d  %-6d  %-8d  %-6d  %-3d  %-5d  %-6d  %-6d  %-5d  %-6d  %-5d  %-4d  %-7d  %-5d  %-5d  ",
                voltage,  (signed short)cur, ufcapacity, capacity, afcapacity, rm, fcc, uuc, cc, delta_rc, uf_temp, temp, ocv, rbatt, di->batt_limit_fcc/1000);

    return strlen(buf);
}

static ssize_t smartstar_set_hand_chg_capacity_flag(struct device_driver *driver, const char *buf, size_t count)
{
    long val;
    int status = count;

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
        return -EINVAL;
    hand_chg_capacity_flag = val;
    return status;
}

static ssize_t smartstar_show_hand_chg_capacity_flag(struct device_driver *driver, char *buf)
{
    unsigned int val;

    val = hand_chg_capacity_flag;
    return sprintf(buf, "%d\n", val);
}

static ssize_t smartstar_set_input_capacity(struct device_driver *driver, const char *buf, size_t count)
{
    long val;
    int status = count;

    if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 100))
        return -EINVAL;
    input_capacity = val;
    return status;
}

static ssize_t smartstar_show_input_capacity(struct device_driver *driver, char *buf)
{
    unsigned int val;

    val = input_capacity;
    return sprintf(buf, "%d\n", val);
}

static ssize_t smartstar_show_abs_cc(struct device_driver *driver, char *buf)
{
    int val = 0;

    val = hisi_saved_abs_cc_mah + (calculate_cc_uah() / 1000);
    return sprintf(buf, "%d\n", val);
}

static ssize_t smartstar_show_battery_brand_name(struct device_driver *driver, char *buf)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

     if((NULL == buf) || (NULL == di)){
        return -1;
    }

    sprintf(buf, "%s\n",di->batt_data->batt_brand);

    return strlen(buf);
}

static ssize_t smartstar_show_battery_id_voltage(struct device_driver *driver, char *buf)
{
    struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if((NULL == buf) || (NULL == di)){
        return -1;
    }

    sprintf(buf, "%d\n",di->batt_id_vol);

    return strlen(buf);
}

static DRIVER_ATTR(gaugelog, (S_IWUSR | S_IWGRP | S_IRUGO), smartstar_show_gaugelog,NULL);
static DRIVER_ATTR(hand_chg_capacity_flag, (S_IWUSR | S_IRUGO),
                  smartstar_show_hand_chg_capacity_flag,
                  smartstar_set_hand_chg_capacity_flag);
static DRIVER_ATTR(input_capacity, (S_IWUSR | S_IRUGO),
                  smartstar_show_input_capacity,
                  smartstar_set_input_capacity);
static DRIVER_ATTR(abs_cc, (S_IWUSR | S_IRUGO),
                  smartstar_show_abs_cc,
                  NULL);

static DRIVER_ATTR(battery_brand_name, (S_IRUSR | S_IRGRP | S_IRUGO), smartstar_show_battery_brand_name, NULL);
static DRIVER_ATTR(battery_id_voltage, (S_IRUSR | S_IRGRP | S_IRUGO), smartstar_show_battery_id_voltage, NULL);

/*******************************************************
  Function:        hisi_get_irqs
  Description:    register irq
  Input:            struct smartstar_coul_device *di                 ---- cou device
                       struct platform_device *pdev                     ---- platform device
  Output:          NULL
  Return:          NULL
********************************************************/

static int hisi_get_irqs(struct platform_device *pdev, struct smartstar_coul_device *di)
{
    int i;
    int retval = 0;
    if( NULL == di || NULL == pdev)
    {
        hwlog_err("NULL point in [%s]\n", __func__);
     	 return -1;
    }

    /*get irq number*/
	di->irqs[0]= platform_get_irq_byname(pdev, "cl_int_i");
	if (di->irqs[0] < 0){
		hwlog_err("Failed to get irq[cl_int_i]!\n");
		return -1;
	}

	di->irqs[1]= platform_get_irq_byname(pdev, "cl_out_i");
	if (di->irqs[1] < 0){
		hwlog_err("Failed to get irq[cl_out_i]!\n");
		return -1;
	}
	di->irqs[2]= platform_get_irq_byname(pdev, "cl_in_i");
	if (di->irqs[2] < 0){
		hwlog_err("Failed to get irq[cl_in_i]!\n");
		return -1;
	}
	di->irqs[3]= platform_get_irq_byname(pdev, "vbat_int_i");
	if (di->irqs[3] < 0){
		hwlog_err("Failed to get irq[vbat_int_i]!\n");
		return -1;
	}
	for (i=0; i<IRQ_MAX; i++){
		retval = request_irq(di->irqs[i], coul_irq_cb, IRQF_NO_SUSPEND, pdev->name, di);
     	if (retval){
          hwlog_err("Failed to request coul irq[%d],ret=%d\n", di->irqs[i], retval);
          return -1;
		}
		hwlog_info("request coul irq[%d]!\n", di->irqs[i]);
	}

	hwlog_err("coul irq request ok!\n");

    return 0;

}

static ssize_t hisi_coul_show_pl_v_offset_a(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", pl_v_offset_a);
}
static ssize_t hisi_coul_set_pl_v_offset_a(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if(strict_strtol(buf, 10, &val) < 0)
        return -EINVAL;

    pl_v_offset_a = val;
    v_offset_a = val;//clear it before calibration and make it effective realtime.

    return status;
}

static ssize_t hisi_coul_show_pl_v_offset_b(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", pl_v_offset_b);
}
static ssize_t hisi_coul_set_pl_v_offset_b(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if(strict_strtol(buf, 10, &val) < 0)
        return -EINVAL;

    pl_v_offset_b = val;
    v_offset_b = val;//clear it before calibration and make it effective realtime.

    return status;
}

static ssize_t hisi_coul_show_pl_c_offset_a(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", pl_c_offset_a);
}
static ssize_t hisi_coul_set_pl_c_offset_a(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if(strict_strtol(buf, 10, &val) < 0)
        return -EINVAL;

    pl_c_offset_a = val;
    c_offset_a = val;//clear it before calibration and make it effective realtime.

    return status;
}

static ssize_t hisi_coul_show_pl_c_offset_b(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", pl_c_offset_b);
}
static ssize_t hisi_coul_set_pl_c_offset_b(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long val = 0;
    int status = count;

    if(strict_strtol(buf, 10, &val) < 0)
        return -EINVAL;

    pl_c_offset_b = val;
    c_offset_b = val;//clear it before calibration and make it effective realtime.

    return status;
}

static ssize_t hisi_coul_show_ate_v_offset_a(struct device *dev, struct device_attribute *attr, char *buf)
{
    unsigned char v_offset_regval = 0;
    int ate_v_offset_a = 0;

    v_offset_regval = SMARTSTAR_REG_READ(SMARTSTAR_VOL_OFFSET_ADDR);
    ate_v_offset_a = get_vol_offset(v_offset_regval)==0? DEFAULT_V_OFF_A:get_vol_offset(v_offset_regval);
    return sprintf(buf, "%d\n", ate_v_offset_a);
}

static int do_save_offset_ret;
static ssize_t hisi_coul_show_do_save_offset_ret(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", do_save_offset_ret);
}
static ssize_t hisi_coul_do_save_offset(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct smartstar_coul_device *di = (struct smartstar_coul_device *) g_smartstar_coul_dev;
    if (!di){
        do_save_offset_ret = -EINVAL;
        hwlog_err("do_save_offset failed, di is NULL!!!\n");
        return count;
    }

    do_save_offset_ret = save_nv_info(di);
    read_nv_info_real(di);
    hwlog_info("do_save_offset_ret:%d, pl_v_offset_a:%d, pl_c_offset_a:%d\n", do_save_offset_ret, pl_v_offset_a, pl_c_offset_a);
    return count;
}
static ssize_t hi6521_show_gaugelog_head(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    return sprintf(buf,"ss_VOL  ss_CUR  ss_ufSOC  ss_SOC  SOC  ss_RM  ss_FCC  ss_UUC  ss_CC  ss_dRC  ufTemp Temp  ss_OCV   rbatt  fcc    ");
}
static ssize_t hi6521_show_gaugelog(struct device *dev,
                  struct device_attribute *attr,
                  char *buf)
{
    int uf_temp = 0, temp = 0, voltage = 0, ufcapacity = 0, capacity = 100, afcapacity = 0, rm = 0, fcc = 0, delta_rc = 0;
    int cur = 0,uuc = 0,cc = 0, ocv=0, rbatt;

	struct smartstar_coul_device *di = g_smartstar_coul_dev;

    if(NULL == buf){
        return -1;
    }

    if (di == NULL)
        return sprintf(buf, "%s", "Smartstar coulometer probe failed!");

    uf_temp = smartstar_battery_uf_temperature();
    temp =  smartstar_battery_temperature();
    voltage = smartstar_battery_voltage();
    cur = -(smartstar_battery_current());
    ufcapacity = smartstar_battery_unfiltered_capacity();
    capacity = smartstar_battery_capacity();
#ifndef _DRV_LLT_
	afcapacity = bq_bci_show_capacity();
#else
	afcapacity = smartstar_battery_capacity();
#endif
    rm =   smartstar_battery_rm();
    fcc =  smartstar_battery_fcc();
    uuc = smartstar_battery_uuc();
    cc = smartstar_battery_cc();
    delta_rc = smartstar_battery_delta_rc();
    ocv = smartstar_battery_ocv();
    rbatt = smartstar_battery_resistance();

    sprintf(buf, "%-6d  %-6d  %-8d  %-6d  %-3d  %-5d  %-6d  %-6d  %-5d  %-6d  %-5d  %-4d  %-7d  %-5d  %-5d  ",
                voltage,  (signed short)cur, ufcapacity, capacity, afcapacity, rm, fcc, uuc, cc, delta_rc, uf_temp, temp, ocv, rbatt, di->batt_limit_fcc/1000);

    return strlen(buf);
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
static DEVICE_ATTR(ate_v_offset_a, (S_IRUGO),
                hisi_coul_show_ate_v_offset_a,
                NULL);
static DEVICE_ATTR(do_save_offset, (S_IWUSR | S_IRUGO),
                hisi_coul_show_do_save_offset_ret,
                hisi_coul_do_save_offset);
static DEVICE_ATTR(gaugelog_head, (S_IWUSR | S_IRUGO),
                hi6521_show_gaugelog_head,
                NULL);
static DEVICE_ATTR(gaugelog, (S_IWUSR | S_IRUGO),
                hi6521_show_gaugelog,
                NULL);
static struct attribute *coul_attributes[] = {
    &dev_attr_pl_v_offset_a.attr,
    &dev_attr_pl_v_offset_b.attr,
    &dev_attr_pl_c_offset_a.attr,
    &dev_attr_pl_c_offset_b.attr,
    &dev_attr_ate_v_offset_a.attr,
    &dev_attr_do_save_offset.attr,
    &dev_attr_gaugelog_head.attr,
    &dev_attr_gaugelog.attr,
    NULL,
};

static const struct attribute_group coul_attr_group = {
    .attrs = coul_attributes,
};

/*******************************************************
  Function:        hisi_smartstar_coul_probe
  Description:    probe function
  Input:            struct platform_device *pdev                     ---- platform device
  Output:          NULL
  Return:          NULL
********************************************************/
static int  hisi_smartstar_coul_probe(struct platform_device *pdev)
{
    struct hisi_coul_ops *coul_ops = NULL;
    struct smartstar_coul_device *di = NULL;
    struct device_node* np;
	struct device_node *batt_node;
    unsigned int product_index = 0;
    unsigned int r_pcb = DEFAULT_RPCB;
    int retval = 0;
    int val;
    /*unsigned char v_offset_regval;*/
    int tryloop = 0;
    int i = 0;
    struct class *power_class = NULL;
    struct device *new_dev = NULL;

    if(NULL == dev_get_drvdata(pdev->dev.parent))
    {
        hwlog_err("failed to get parent resource!\n");
	 return -1;
     }
     np = pdev->dev.of_node;
     if(NULL == np)
     {
          return -1;
     }

/* check coulomb version */
#ifndef _DRV_LLT_
    do {
#endif
        retval = SMARTSTAR_REG_READ(SMARTSTAR_COUL_VERSION_ADDR);
        hwlog_info("do a dummy read, smartstar version is 0x%x\n",
                            retval);

        udelay(500);

        if ((tryloop++) > 5){
            hwlog_err("Smartstar version is not correct!\n");
            goto coul_failed;
        }
#ifndef _DRV_LLT_
    } while((retval < COUL_VERSION_MIN)||(retval > COUL_VERSION_MAX));
#endif

	s_smartstar_version = retval;
	/*HI6553 DTXO fre is different with hi6552,so cc unit isn't same*/
	if ((s_smartstar_version >= HI6553_COUL_VERSION_MIN) && (s_smartstar_version <= HI6553_COUL_VERSION_MAX))
	{
		retval = SMARTSTAR_REG_READ(SMARTSTAR_CLK19M2_600_586_EN);
		if (DCXO_586 == retval){
			s_dcxo_tpye = DCXO_586;
		}
		else{
			s_dcxo_tpye = DCXO_600;
		}
		hwlog_info("s_dcxo_tpye is %d!\n",s_dcxo_tpye);
	}

    val = SMARTSTAR_REG_READ(SMARTSTAR_DEBUG_REG);
    udelay(500);
    if(val != 0x0){
        short ocvreg = 0x0;
        hwlog_err("smartstar debug reg is not 0x0\n");
        SMARTSTAR_REG_WRITE(SMARTSTAR_DEBUG_REG, 0x0);
        udelay(500);
        SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR, &ocvreg, 2);
        udelay(500);
#ifndef _DRV_LLT_
       BUG();
#endif
    }

    /* set current offset_a */
    c_offset_b = 0;

    if (of_property_read_u32(np, "current_offset_a",&c_offset_a)){
	    c_offset_a = DEFAULT_C_OFF_A;
		hwlog_err("error:get r_pcb value failed!\n");
    }
    g_coul_work_mode = SMARTSTAR_REG_READ(COUL_WORK_MODE_ADDR);
	hwlog_info("v_a=%d,v_b=%d,c_a=%d,c_b=%d, pmu_work_mode=%d\n",
        				v_offset_a, v_offset_b, c_offset_a, c_offset_b, (int)g_coul_work_mode);

    retval = driver_create_file(&(hisi_smartstar_coul_driver.driver), &driver_attr_gaugelog);
        if (0 != retval) {
            hwlog_err("failed to create sysfs entry(gaugelog): %d\n", retval);
            return -1;
        }
    retval = driver_create_file(&(hisi_smartstar_coul_driver.driver), &driver_attr_hand_chg_capacity_flag);
        if (0 != retval) {
            hwlog_err("failed to create sysfs entry(hand_chg_capacity_flag): %d\n", retval);
            return -1;
        }
    retval = driver_create_file(&(hisi_smartstar_coul_driver.driver), &driver_attr_input_capacity);
        if (0 != retval) {
            hwlog_err("failed to create sysfs entry(input_capacity): %d\n", retval);
            return -1;
        }
    retval = driver_create_file(&(hisi_smartstar_coul_driver.driver), &driver_attr_abs_cc);
    if (0 != retval) {
        hwlog_err("failed to create sysfs entry(abs_cc): %d\n", retval);
        return -1;
    }

    retval = driver_create_file(&(hisi_smartstar_coul_driver.driver), &driver_attr_battery_brand_name);
    if (0 != retval) {
        hwlog_err("failed to create sysfs entry(battery_brand_name): %d\n", retval);
        return -1;
    }

    retval = driver_create_file(&(hisi_smartstar_coul_driver.driver), &driver_attr_battery_id_voltage);
    if (0 != retval) {
        hwlog_err("failed to create sysfs entry(battery_id_voltage): %d\n", retval);
        return -1;
    }

    retval = sysfs_create_group(&pdev->dev.kobj, &coul_attr_group);
    if (retval) {
        hwlog_err("%s failed to create sysfs group!!!\n", __FUNCTION__);
        return -1;
    }

#ifndef _DRV_LLT_
    power_class = hw_power_get_class();
    if(power_class)
    {
        new_dev = device_create(power_class, NULL, 0, "%s", "coul");
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
#endif

    //wake_lock_init(&hisi_smartstar_wakelock, WAKE_LOCK_SUSPEND, "hisi_smartstar_wakelock");
#ifdef _DRV_LLT_
    di = (struct smartstar_coul_device *)ut_malloc_device(sizeof(*di), GFP_KERNEL);
#else
    di = (struct smartstar_coul_device *)kzalloc(sizeof(*di), GFP_KERNEL);
#endif
    if (!di) {
		hwlog_err("%s failed to alloc di struct\n",__FUNCTION__);
		retval = -1;
		goto coul_failed_0;
    }
    g_smartstar_coul_dev = di;
    platform_set_drvdata(pdev, di);

    /*set di element with default data*/
    di->prev_pc_unusable = -EINVAL;
    di->v_cutoff = BATTERY_NORMAL_CUTOFF_VOL;//3200;
    di->batt_ocv_cur_limit = 25000; /*ua*/
    di->batt_ocv_curreglimit = (short)convert_ua2regval(di->batt_ocv_cur_limit);
    di->v_low_int_value = LOW_INT_NOT_SET;
    di->last_cali_temp = -990; /* invalid temperature */
    di->soc_work_interval = CALCULATE_SOC_MS;
    di->last_iavg_ma = IMPOSSIBLE_IAVG;
    /* read nv info */
    di->is_nv_read = !read_nv_info(di);

    val = SMARTSTAR_REG_READ(SMARTSTAR_NV_READ_SUCCESS);
    val = val & NV_READ_BITMASK;
    di->is_nv_read = !!NV_READ_BITMASK;

    di->is_nv_need_save = 0;

    mutex_init(&di->soc_mutex);

    di->sr_resume_time = get_coul_time();
    sr_cur_state = SR_DEVICE_WAKEUP;

    batt_node = of_find_compatible_node(NULL, NULL, "huawei,bq_bci_battery");
    if (batt_node) {
        of_property_read_u32(batt_node, "battery_board_type",&is_board_type);
		of_property_read_u32(batt_node, "battery_is_removable",&battery_is_removable);
    } else {
		is_board_type = BAT_BOARD_SFT;
		battery_is_removable = 0;
	}
	hwlog_err( "get board type is %d ,battery removable flag is %d !\n",is_board_type ,battery_is_removable);

	if (of_property_read_u32(np, "battery_product_index",&product_index)){
		hwlog_err("error:get battery product index fail,use defualt value !\n");
		product_index = 0;
	}
	di->product_index = product_index;

    if (of_property_read_u32(np, "r_pcb",&r_pcb)){
		hwlog_err("error:get r_pcb value failed!\n");
	}
	di->r_pcb = r_pcb;
	if (battery_is_removable) {
		    INIT_DELAYED_WORK(&di->battery_check_delayed_work,
        battery_check_work);
	}

    /*check battery is exist*/
    if (!is_smartstar_battery_exist()) {
        hwlog_err("%s: no battery, just register callback\n",__FUNCTION__);
        //retval = -EPERM;
        di->batt_data = get_battery_data(di->batt_id_vol, product_index);
        di->batt_exist = 0;
        goto coul_no_battery;
    }

    di->batt_exist = 1;

    /*set battery data*/
    get_battery_id_voltage(di);
    di->batt_data = get_battery_data(di->batt_id_vol, product_index);
    if (NULL == di->batt_data) {
        hwlog_err("%s: batt ID(0x%x) is invalid, product_index is %d\n",__FUNCTION__,di->batt_id_vol, product_index);
        retval = -1;
        goto coul_failed_1;
    }


    hwlog_info("%s: batt ID is %d, batt_brand is %s, product_index is %d\n",__FUNCTION__,di->batt_id_vol, di->batt_data->batt_brand, product_index);
    /* Init soc calc work */
    INIT_DELAYED_WORK(&di->calculate_soc_delayed_work,
        calculate_soc_work);


    di->batt_temp = smartstar_battery_temperature_tenth_degree();

    hwlog_info("battery temperature is %d.%d\n", di->batt_temp/10, di->batt_temp%10);

    /*calculate init soc */
    get_initial_ocv(di);

    di->charging_stop_time = get_coul_time();

    /*config coul power on */
    smartstar_coul_config_power_on_reg(di);
    /* battery moved, clear battery data  */
    if(is_smartstar_battery_moved()){
        char val;
        di->batt_chargecycles = 0;
        di->batt_changed_flag = 1;
        di->batt_limit_fcc = 0;
        di->adjusted_fcc_temp_lut = NULL; /* enable it when test ok */
        di->is_nv_need_save = 1;

        val = SMARTSTAR_REG_READ(SMARTSTAR_NV_SAVE_SUCCESS);
        udelay(110);
        SMARTSTAR_REG_WRITE(SMARTSTAR_NV_SAVE_SUCCESS, (val & (~NV_SAVE_BITMASK)));

        hwlog_info("battery changed, reset chargecycles!\n");
    } else {
        hwlog_info("battery not changed, chargecycles = %d%%\n", di->batt_chargecycles);
    }

    /*get the first soc value*/
    DI_LOCK();
    di->soc_limit_flag = 0;
    di->batt_soc = calculate_state_of_charge(di);
    di->soc_limit_flag = 1;
    DI_UNLOCK();

    /*schedule calculate_soc_work*/
    schedule_delayed_work(&di->calculate_soc_delayed_work,
                        round_jiffies_relative(msecs_to_jiffies(di->soc_work_interval)));

    /* Init interrupt notifier work */
    INIT_DELAYED_WORK(&di->notifier_work,
                      interrupt_notifier_work);

    set_low_vol_int_reg(di, LOW_INT_STATE_RUNNING);

    if (hisi_get_irqs(pdev, di)){
        goto coul_failed_2;
    }
	if (battery_is_removable){
	    schedule_delayed_work(&di->battery_check_delayed_work,
                        round_jiffies_relative(msecs_to_jiffies(BATTERY_CHECK_TIME_MS)));
	}

    /* set shutdown level */
    set_low_capacity_int_reg(di);
	smartstar_irq_enable();

coul_no_battery:
#ifdef _DRV_LLT_
    coul_ops = (struct hisi_coul_ops*) ut_malloc_ops(sizeof (*coul_ops), GFP_KERNEL);
#else
    coul_ops = (struct hisi_coul_ops*) kzalloc(sizeof (*coul_ops), GFP_KERNEL);
#endif
    if (!coul_ops) {
		hwlog_err("failed to alloc coul_ops struct\n");
		retval = -1;
        goto coul_failed_3;
    }
    /* config coul ops */
    coul_ops->battery_id_voltage = hisi_battery_id_vol;
    coul_ops->is_coul_ready = is_smartstar_coul_ready;
    coul_ops->is_battery_exist = is_battery_exist;//is_smartstar_battery_exist;
    coul_ops->is_battery_reach_threshold = is_smartstar_battery_reach_threshold;
    coul_ops->is_battery_full = is_smartstar_battery_full;
    coul_ops->battery_brand = smartstar_battery_brand;
    coul_ops->battery_voltage = smartstar_battery_voltage;
    coul_ops->battery_voltage_uv= smartstar_battery_voltage_uv;
    coul_ops->battery_current = smartstar_battery_current;
    coul_ops->battery_current_avg = smartstar_battery_current_avg;
    coul_ops->battery_unfiltered_capacity = smartstar_battery_unfiltered_capacity;
    coul_ops->battery_capacity = smartstar_battery_capacity;
    coul_ops->battery_uf_temperature = smartstar_battery_uf_temperature;
    coul_ops->battery_temperature = smartstar_battery_temperature;
    coul_ops->battery_rm = smartstar_battery_rm;
    coul_ops->battery_fcc = smartstar_battery_fcc;
    coul_ops->battery_tte = smartstar_battery_tte;
    coul_ops->battery_ttf = smartstar_battery_ttf;
    coul_ops->battery_health = smartstar_battery_health;
    coul_ops->battery_capacity_level = smartstar_battery_capacity_level;
    coul_ops->battery_technology = smartstar_battery_technology;
    coul_ops->battery_charge_param = smartstar_battery_charge_param;
    coul_ops->charger_event_rcv = smartstar_battery_charger_event_rcv;
#ifdef CONFIG_HUAWEI_HLTHERM_CHARGING
    coul_ops->is_hisi_coul_write = is_coul_reg_write;
    coul_ops->is_hisi_coul_read = is_coul_reg_read;
#endif
    di->ops = coul_ops;
 	retval = hisi_coul_ops_register(coul_ops,COUL_SMARTSTAR);
    if (retval) {
        hwlog_err("failed to register coul ops\n");
        goto coul_failed_4;
    }

    hwlog_info("coul probe ok!\n");
#ifdef _DRV_LLT_
#else
	g_pdev = pdev;				/* for debug */
#endif
    return retval;


coul_failed_4:
#ifndef _DRV_LLT_
    kfree(coul_ops);
#endif
    di->ops = NULL;
coul_failed_3:
    for (i=0; i<IRQ_MAX; i++){
    	free_irq(di->irqs[i], di);
    }
    cancel_delayed_work(&di->calculate_soc_delayed_work);
	if (battery_is_removable)
		cancel_delayed_work(&di->battery_check_delayed_work);
coul_failed_2:
	cancel_delayed_work(&di->notifier_work);
coul_failed_1:
    platform_set_drvdata(pdev, NULL);
#ifndef _DRV_LLT_
    kfree(di);
#endif
    g_smartstar_coul_dev = 0;
coul_failed_0:
    smartstar_disable_coul(di);
coul_failed:
    hwlog_err("Smartstar probe failed!\n");
    return retval;
}

/*******************************************************
  Function:        hisi_smartstar_coul_remove
  Description:    remove function
  Input:            struct platform_device *pdev        ---- platform device
  Output:          NULL
  Return:          NULL
********************************************************/
static int  hisi_smartstar_coul_remove(struct platform_device *pdev)
{
#ifdef _DRV_LLT_
    struct smartstar_coul_device *di = ut_platform_get_drvdata(pdev);
#else
    struct smartstar_coul_device *di = platform_get_drvdata(pdev);
#endif
    if(NULL == di)
    {
        hwlog_info("[%s]di is null\n",__FUNCTION__);
        return 0;
    }
#if RBATT_ADJ
    if (di->batt_soc <= 2){
        int ocv_invalid = 0;
        short ocvreg = 0;
        if (di->rbatt_ratio >= 400){
            ocv_invalid = 1;
            hwlog_info("rbatt_ratio(%d) > 400, mark save ocv is invalid\n", di->rbatt_ratio);
        }
        if (di->batt_delta_rc){
            ocv_invalid = 1;
            hwlog_info("batt_delta_rc=%d, mark save ocv is invalid\n", di->batt_delta_rc);
        }

        if (ocv_invalid){
            udelay(110);
            SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR,&ocvreg,2);
        }
    }
#endif

    hisi_coul_ops_unregister(di->ops);
    kfree(di->ops);
    di->ops = NULL;
    kfree(di);
    g_smartstar_coul_dev = NULL;

    return 0;
}

#ifdef CONFIG_PM

static void smartstar_clear_fifo(void)
{
	unsigned char reg_value = 0;
	reg_value = SMARTSTAR_REG_READ(SMARTSTAR_FIFO_CLEAR);
	SMARTSTAR_REG_WRITE(SMARTSTAR_FIFO_CLEAR, (reg_value |COUL_FIFO_CLEAR));

}

static void smartstar_enter_eco(void)
{
    int fifo0_vol = 0;
	int cur_vol = 0;
    unsigned char reg_val = 0;

    SMARTSTAR_REGS_READ(SMARTSTAR_ECO_OUT_CLIN_REG_BASE, &last_eco_in, 4);
    SMARTSTAR_REGS_READ(SMARTSTAR_ECO_OUT_CLOUT_REG_BASE, &last_eco_out, 4);

    reg_val = SMARTSTAR_REG_READ(SMARTSTAR_COUL_STATE_REG);
    if ((COUL_CALI_ING == reg_val) || (0 == is_cali_flag)){
    	hwlog_info("cali ing, don't do it again!\n");
        is_cali_flag = 1;
        reg_val= ECO_COUL_CTRL_VAL;
    }
    else {
        hwlog_info("calibrate!\n");
        reg_val= (ECO_COUL_CTRL_VAL | COUL_CALI_ENABLE);

    }
    udelay(110);
	SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_CTRL_REG,reg_val);
	/*avoid low interrput who occur in the process of wake and sleep < 110ms*/
	smartstar_get_battery_vol_from_fifo(&fifo0_vol);
	cur_vol = smartstar_battery_voltage_uv();
	if(-1 != fifo0_vol)
	{
		fifo0_vol /=1000;
		cur_vol /=1000;
		if (fifo0_vol > BATTERY_VOL_2_PERCENT)
			saved_vol_fifo0_out_eco = fifo0_vol;
		else
			saved_vol_fifo0_out_eco = cur_vol;
	}
	hwlog_info("fifo0_vol = %d, cur_vol =%d,out_eco=%d\n",fifo0_vol,cur_vol,saved_vol_fifo0_out_eco);
	smartstar_clear_fifo();
}


static void smartstar_exit_eco(void)
{
    int fifo0_vol = 0;
	int cur_vol = 0;
	/*avoid low interrput who occur in the process of sleep and wake < 110ms*/
	smartstar_get_battery_vol_from_fifo(&fifo0_vol);
	cur_vol = smartstar_battery_voltage_uv();
	if(-1 != fifo0_vol)
	{
		fifo0_vol /=1000;
		cur_vol /=1000;
		if (fifo0_vol > BATTERY_NORMAL_CUTOFF_VOL)
			saved_vol_fifo0_in_eco = fifo0_vol;
		else
			saved_vol_fifo0_in_eco = cur_vol;
	}
    else {
        /*cali in progress ,next suspend don't cali*/
        is_cali_flag = 0;
    }
	hwlog_info("fifo0_vol = %d, cur_vol =%d,in_eco=%d\n",fifo0_vol,cur_vol,saved_vol_fifo0_in_eco);
	smartstar_clear_fifo();
    udelay(110);
	SMARTSTAR_REG_WRITE(SMARTSTAR_COUL_CTRL_REG,DEFAULT_COUL_CTRL_VAL);
}


/*******************************************************
  Function:        hisi_smartstar_coul_suspend
  Description:    suspend function, called when coul enter sleep, v9 no sleep
  Input:            struct platform_device *pdev                     ---- platform device
  Output:          NULL
  Return:          NULL
********************************************************/
static int hisi_smartstar_coul_suspend(struct platform_device *pdev,
	pm_message_t state)
{
#ifdef _DRV_LLT_
    struct smartstar_coul_device *di = ut_platform_get_drvdata(pdev);
#else
    struct smartstar_coul_device *di = platform_get_drvdata(pdev);
#endif
    int current_sec = get_coul_time();
    int wakeup_time = 0;

    if(NULL == di)
    {
        hwlog_info("[%s]di is null\n",__FUNCTION__);
        return 0;
    }

    DI_LOCK();
    di->suspend_cc = calculate_cc_uah();
    di->suspend_time = current_sec;
    di->sr_suspend_time = current_sec;

    wakeup_time = current_sec - di->sr_resume_time;
    if (wakeup_time > SR_MAX_RESUME_TIME) {
        clear_sr_time_array();
        hwlog_info("[SR]%s(%d): wakeup_time(%d) > SR_MAX_RESUME_TIME(%d)\n", __func__, __LINE__, wakeup_time, SR_MAX_RESUME_TIME);
    }
    else if (wakeup_time >= 0) {
        sr_time_wakeup[sr_index_wakeup] = wakeup_time;
        sr_index_wakeup++;
        sr_index_wakeup = sr_index_wakeup % SR_ARR_LEN;
    }
    else {
        hwlog_err("[SR]%s(%d): wakeup_time=%d, di->sr_suspend_time=%d, di->sr_resume_time=%d\n",
            __func__, __LINE__, wakeup_time, di->sr_suspend_time, di->sr_resume_time);
    }

    sr_cur_state = SR_DEVICE_SLEEP;
    DI_UNLOCK();
	hwlog_info("SUSPEND! cc=%d, time=%d\n", di->suspend_cc,
					   di->suspend_time);
    if (di->batt_exist){
        cancel_delayed_work(&di->calculate_soc_delayed_work);
    }
    cali_adc();
    //udelay(35);
	if (battery_is_removable) {
    	cancel_delayed_work(&di->battery_check_delayed_work);
	}
    set_low_vol_int_reg(di, LOW_INT_STATE_SLEEP);
	//smartstar_enter_eco();
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

    do {
        total_sleep_time += sr_time_sleep[last_sleep_idx];
        total_wakeup_time += sr_time_wakeup[last_wakeup_idx];

        last_sleep_idx = (last_sleep_idx - 1 < 0) ? SR_ARR_LEN - 1 : last_sleep_idx - 1;
        last_wakeup_idx = (last_wakeup_idx - 1 < 0) ? SR_ARR_LEN - 1 : last_wakeup_idx - 1;

        cnt++;
        if (cnt >= SR_ARR_LEN) {
            break;
        }
    } while (total_sleep_time + total_wakeup_time < SR_TOTAL_TIME);

    /* calculate duty ratio */
    if (total_sleep_time + total_wakeup_time >= SR_TOTAL_TIME) {
        duty_ratio = total_sleep_time * 100 / (total_sleep_time + total_wakeup_time);
        hwlog_info("[SR]%s(%d): total_wakeup=%ds, total_sleep=%ds, duty_ratio=%d\n",
            __func__, __LINE__, total_wakeup_time, total_sleep_time, duty_ratio);
    }

    return duty_ratio;
}


static int sr_need_update_ocv(struct smartstar_coul_device *di) {
    int last_wakeup_time = 0;
    int last_sleep_time = 0;
    int duty_ratio = 0;

    /* get last wakeup time */
    if (sr_index_wakeup >= 0 && sr_index_wakeup < SR_ARR_LEN) {
        last_wakeup_time = (sr_index_wakeup - 1 < 0) ? sr_time_wakeup[SR_ARR_LEN - 1]: sr_time_wakeup[sr_index_wakeup - 1];
    }

    /* get last sleep time */
    if (sr_index_sleep >= 0 && sr_index_sleep < SR_ARR_LEN) {
        last_sleep_time = (sr_index_sleep - 1 < 0) ? sr_time_sleep[SR_ARR_LEN - 1]: sr_time_sleep[sr_index_sleep - 1];
    }

    /* get last SR_TOTAL_TIME seconds duty ratio */
    duty_ratio = sr_get_duty_ratio();

    /* judge if need update ocv */
    if (last_sleep_time > SR_DELTA_SLEEP_TIME &&
        last_wakeup_time < SR_DELTA_WAKEUP_TIME &&
        duty_ratio > SR_DUTY_RATIO ) {
        hwlog_info("[SR]%s(%d): need_update, last_sleep=%ds, last_wakeup=%ds, duty_ratio=%d\n",
            __func__, __LINE__, last_sleep_time, last_wakeup_time, duty_ratio);
        return 1;
    }
    else {
        hwlog_info("[SR]%s(%d): no_need_update, last_sleep=%ds, last_wakeup=%ds, duty_ratio=%d\n",
            __func__, __LINE__, last_sleep_time, last_wakeup_time, duty_ratio);
        return 0;
    }
}

/*******************************************************
  Function:        ocv_could_update
  Description:    check whether OCV can update
  Input:            struct platform_device *pdev    ---- platform device
  Output:          NULL
  Return:          1: can update, 0: can not
  Remark:         update condition----sleep_time > 10min && sleep_current < 50mA
********************************************************/
static int ocv_could_update(struct smartstar_coul_device *di)
{
    int sleep_cc, sleep_current = 0;
    int sleep_time, time_now;

    sleep_cc = calculate_cc_uah();
    sleep_cc = sleep_cc - di->suspend_cc;  /* sleep uah */
    time_now = get_coul_time();
    sleep_time = time_now - di->suspend_time; /* sleep time */

    if (sleep_time < delta_sleep_time && !sr_need_update_ocv(di))
    {
        hwlog_info("[SR]Can't update ocv, sleep_current=%d ma, sleep_time=%d s\n", sleep_current, sleep_time);
        return 0;
    }

    /* ma = ua/1000 = uas/s/1000 = uah*3600/s/1000 = uah*18/(s*5) */
    if (sleep_time > 0) {
        sleep_current = (sleep_cc * 18) / (sleep_time * 5);

        if (sleep_current > delta_sleep_current)
        {
            hwlog_info("[SR]Can't update ocv, sleep_current=%d ma, sleep_time=%d s\n", sleep_current, sleep_time);
            return 0;
        }
    }
    else {
        hwlog_err("[SR]%s(%d): sleep_time = %d\n",  __func__, __LINE__, sleep_time);
        return 0;
    }

    hwlog_info("[SR]going to update ocv, sleep_time=%ds, sleep_current=%d ma\n", sleep_time, sleep_current);
    return 1;
}

/*******************************************************
  Function:        get_ocv_resume
  Description:     get ocv after resuming
  Input:            struct platform_device *pdev    ---- platform device
  Output:          NULL
  Return:          1: can update, 0: can not
  Remark:         update condition----sleep_time > 10min && sleep_current < 50mA
********************************************************/
static void get_ocv_resume(struct smartstar_coul_device *di)
{
    int cc;

    //g_eco_leak_uah = calculate_eco_leak_uah();
    get_ocv_by_vol(di);
	//set_cc(eco_leak_uah);

	//di->cc_start_value = -g_eco_leak_uah;
    cc = calculate_cc_uah();
	cc = cc + g_eco_leak_uah;
	save_cc_uah(cc);
	return;
}
/*******************************************************
  Function:        hisi_smartstar_coul_resume
  Description:    suspend function, called when coul wakeup from deep sleep
  Input:            struct platform_device *pdev                     ---- platform device
  Output:          NULL
  Return:          NULL
********************************************************/
static int hisi_smartstar_coul_resume(struct platform_device *pdev)
{
#ifdef _DRV_LLT_
    struct smartstar_coul_device *di = ut_platform_get_drvdata(pdev);
#else
    struct smartstar_coul_device *di = platform_get_drvdata(pdev);
#endif
    int current_sec = get_coul_time();
    int sr_sleep_time;
	
    if(NULL == di)
    {
        hwlog_info("[%s]di is null\n",__FUNCTION__);
        return 0;
    }

    sr_sleep_time = current_sec - di->sr_suspend_time;
    hwlog_info("RESUME!\n");

    disable_temperature_debounce = 0;
    set_low_vol_int_reg(di, LOW_INT_STATE_RUNNING);
    DI_LOCK();
    sr_cur_state = SR_DEVICE_WAKEUP;
    di->sr_resume_time = current_sec;
    /* record sleep time */
    if (sr_sleep_time >= 0) {
        sr_time_sleep[sr_index_sleep] = sr_sleep_time;
        sr_index_sleep++;
        sr_index_sleep = sr_index_sleep % SR_ARR_LEN;
    }
    else {
        hwlog_err("[SR]%s(%d): sr_sleep_time = %d\n", __func__, __LINE__, sr_sleep_time);
    }

    if (ocv_could_update(di)
        && (current_sec - di->charging_stop_time > 30*60)){
        //get_ocv_by_vol(di);
        get_ocv_resume(di);
    }
    else if (di->batt_delta_rc > di->batt_data->fcc*5*10
        && di->charging_state != CHARGING_STATE_CHARGE_START
        && (current_sec - di->charging_stop_time > 30*60)){
        int old_ocv = di->batt_ocv;
    	hwlog_info("Update ocv for delta_rc(%d)!\n", di->batt_delta_rc);
        //get_ocv_by_vol(di);
        get_ocv_resume(di);
        if (old_ocv != di->batt_ocv){
            short ocvreg = convert_uv2regval(di->batt_ocv);
            ocvreg = ocvreg | 0x8000;
            udelay(110);
            SMARTSTAR_REGS_WRITE(SMARTSTAR_SAVE_OCV_ADDR,&ocvreg,2);
            di->batt_ocv_valid_to_refresh_fcc = 0;
        }
    }
	//smartstar_exit_eco();
    di->soc_limit_flag = 2;
    di->batt_soc = calculate_state_of_charge(di);
    di->soc_limit_flag = 1;

    DI_UNLOCK();

    if (di->batt_exist){
        schedule_delayed_work(&di->calculate_soc_delayed_work,
                    round_jiffies_relative(msecs_to_jiffies(CALCULATE_SOC_MS/2)));
    }
	if (battery_is_removable) {
	    schedule_delayed_work(&di->battery_check_delayed_work,
                round_jiffies_relative(msecs_to_jiffies(BATTERY_CHECK_TIME_MS)));
	}
        return 0;
}
#endif
static struct of_device_id hisi_smartstar_coul_match_table[] =
{
    {
          .compatible = "hisilicon,hi6xxx-coul",
    },
    { /*end*/},
};


void hisi_smartstar_coul_shutdown(struct platform_device *pdev)
{
    volatile int cycle = 0;
    unsigned char reg_val = 0;

    hwlog_err("hisi_smartstar_coul_shutdown start!\n");
	/*wait coul cali status valid*/
    mdelay(112);
    do {
        cycle++;
        reg_val = SMARTSTAR_REG_READ(SMARTSTAR_COUL_STATE_REG);
        if (COUL_CALI_ING == reg_val){
            mdelay(10);
            continue;
        } else {
            hwlog_info("hisi_smartstar_coul_shutdown cycle =%d,cali=%d!\n",cycle,reg_val);
            break;
        }
    } while(cycle < 150);
    hwlog_err("hisi_smartstar_coul_shutdown end!\n");
}


static struct platform_driver hisi_smartstar_coul_driver = {
	.probe		= hisi_smartstar_coul_probe,
	.remove		= hisi_smartstar_coul_remove,
	.shutdown   = hisi_smartstar_coul_shutdown,
#ifdef CONFIG_PM
	.suspend	       = hisi_smartstar_coul_suspend,
	.resume		= hisi_smartstar_coul_resume,
#endif
	.driver		= {
	.name		= "hisi_smartstar_coul",
       .owner          = THIS_MODULE,
       .of_match_table = hisi_smartstar_coul_match_table,
	},
};

int __init hisi_smartstar_coul_init(void)
{
    return platform_driver_register(&hisi_smartstar_coul_driver);
}

void __exit hisi_smartstar_coul_exit(void)
{
    platform_driver_unregister(&hisi_smartstar_coul_driver);
}

fs_initcall(hisi_smartstar_coul_init);
module_exit(hisi_smartstar_coul_exit);


#ifdef SMARTSTAR_DEBUG
void ss_cur_show(void)
{
    int i = 0;
    int ua = 0;
    short regval = 0;

    for (i=0; i<FIFO_DEPTH; i++) {
        SMARTSTAR_REGS_READ(SMARTSTAR_CUR_FIFO_BASE+i*2, &regval, 2);
        ua = convert_regval2ua(regval);
        hwlog_info( "[%02d]:regval=0x%04x,cur = %d ua \n",i,regval,ua);
    }

    SMARTSTAR_REGS_READ(SMARTSTAR_CURRENT, &regval, 2);
	ua = convert_regval2ua(regval);
	hwlog_err( "***CLJ ST*** CURRENT: regval = 0x%04x, cur = %d ua ***CLJ ST***\n", regval, ua);
}

void ss_vol_show(void)
{
    int i = 0;
    int uv = 0;
    short regval = 0;

    for (i=0; i<FIFO_DEPTH; i++) {
        SMARTSTAR_REGS_READ(SMARTSTAR_VOL_FIFO_BASE+i*2, &regval, 2);
        uv = convert_regval2uv(regval);
        hwlog_err("[%02d]:regval=0x%04x,vol = %d uv \n",i,regval,uv);
    }

    SMARTSTAR_REGS_READ(SMARTSTAR_V_OUT, &regval, 2);
	uv = convert_regval2uv(regval);
	hwlog_err( "***CLJ ST*** VOLTAGE: regval=0x%04x, vol= %d uv ***CLJ ST***\n",regval, uv);
}

void ss_ocv_show(void)
{
    short ocvreg = 0;
    short volreg_offset = 0;
    int ocv = 0;

    /*read ocv and offset */
    SMARTSTAR_REGS_READ(SMARTSTAR_OCV_VOLTAGE_BASE,&ocvreg,2);
    SMARTSTAR_REGS_READ(SMARTSTAR_OFFSET_VOLTAGE,&volreg_offset,2);

    ocv = convert_regval2uv(ocvreg - volreg_offset);

    hwlog_err("regval=0x%04x,offset = 0x%04x,ocv = %d uv \n",\
            ocvreg,volreg_offset,ocv);
    hwlog_err( "***CLJ ST*** OCV = %d uv ***CLJ ST***\n", ocv);
}

void ss_get_ocv(void)
{
    struct smartstar_coul_device *di = (struct smartstar_coul_device *)g_smartstar_coul_dev;
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

    SMARTSTAR_REGS_READ(SMARTSTAR_CL_IN_BASE,&cc_in,4);
    SMARTSTAR_REGS_READ(SMARTSTAR_CL_OUT_BASE,&cc_out,4);
	cc_uah_out = convert_regval2uah(cc_out);
	cc_uah_in = convert_regval2uah(cc_in);
	cc_uah = cc_uah - cc_uah_in;

	temp = (s64) c_offset_a *cc_uah_in;
	cc_uah_in = div_s64(temp, 1000000);
	temp = (s64) c_offset_a *cc_uah_out;
	cc_uah_out = div_s64(temp, 1000000);
	SMARTSTAR_REGS_READ(SMARTSTAR_CHG_TIMER_BASE, &cl_in_time, 4);
	SMARTSTAR_REGS_READ(SMARTSTAR_LOAD_TIMER_BASE, &cl_out_time, 4);
	/* uah = uas/3600 = ua*s/3600 */
	cc_uah_in = cc_uah_in - div_s64((s64) cl_in_time * c_offset_b, 3600);
	cc_uah_out =
		cc_uah_out + div_s64((s64) cl_out_time * c_offset_b, 3600);
	cc_uah_adjust = cc_uah_out - cc_uah_in;

	hwlog_err( "ccout_reg=0x%x,ccin_reg = 0x%x,cc = %d uah \n"
		   "cl_out_time=%d, cl_in_time=%d, cc_adjust=%d uah\n",
		   (unsigned int) cc_out, (unsigned int)cc_in, cc_uah,
		   cl_out_time, cl_in_time, cc_uah_adjust);
}

void ss_time_show(void)
{
    int cl_in_time = 0;
    int cl_out_time = 0;

    SMARTSTAR_REGS_READ(SMARTSTAR_CHG_TIMER_BASE,&cl_in_time,4);
    SMARTSTAR_REGS_READ(SMARTSTAR_LOAD_TIMER_BASE,&cl_out_time,4);

    hwlog_err("cl_in_time=%d,cl_out_time = %d,total = %d\n",\
            (unsigned int)cl_in_time,(unsigned int)cl_out_time,(unsigned int)(cl_in_time+cl_out_time));

}

void ss_soc_update(void)
{
    struct smartstar_coul_device *di = (struct smartstar_coul_device *)g_smartstar_coul_dev;
    unsigned int old_soc;
    old_soc = di->batt_soc;
    di->batt_soc = calculate_state_of_charge(di);
    hwlog_err("batt_soc %d ---> %d \n",old_soc,di->batt_soc);
}

void ss_soc_show(void)
{
	struct smartstar_coul_device *di = (struct smartstar_coul_device *)g_smartstar_coul_dev;
	hwlog_err( "***CLJ ST*** SOC = %d ***CLJ ST***\n", di->batt_soc);
}

void ss_di_show(void)
{
    struct smartstar_coul_device *di = (struct smartstar_coul_device *)g_smartstar_coul_dev;

    hwlog_err("batt_exist = 0x%x\n", di->batt_exist);
    hwlog_err("prev_pc_unusable = %d\n", di->prev_pc_unusable);
    //hwlog_err("irqs = %d\n", di->irqs);
    hwlog_err("irq_mask = 0x%x\n", di->irq_mask);
    hwlog_err("batt_ocv_cur_limit = %d\n", di->batt_ocv_cur_limit);
    hwlog_err("batt_ocv_curreglimit = %d\n", di->batt_ocv_curreglimit);
    hwlog_err("batt_ocv = %d\n", di->batt_ocv);
    hwlog_err("batt_changed_flag = %d\n", di->batt_changed_flag);
    hwlog_err("batt_vol_low_flag = %d\n", di->batt_vol_low_flag);
    hwlog_err("soc_limit_flag = %d\n", di->soc_limit_flag);
    hwlog_err("batt_temp = %d\n", di->batt_temp);
    hwlog_err("batt_fcc = %d\n", di->batt_fcc);
    hwlog_err("batt_limit_fcc = %d\n", di->batt_limit_fcc);
    hwlog_err("batt_rm = %d\n", di->batt_rm);
    hwlog_err("batt_ruc = %d\n", di->batt_ruc);
    hwlog_err("batt_uuc = %d\n", di->batt_uuc);
    hwlog_err("rbatt = %d\n", di->rbatt);
    hwlog_err("r_pcb = %d\n", di->r_pcb);
    hwlog_err("soc_work_interval = %d\n", di->soc_work_interval);
    hwlog_err("charging_begin_soc = %d\n", di->charging_begin_soc);
    hwlog_err("charging_state = %d\n", di->charging_state);
    hwlog_err("batt_soc = %d\n", di->batt_soc);
    hwlog_err("batt_soc_real = %d\n", di->batt_soc_real);
    hwlog_err("batt_soc_with_uuc = %d\n", di->batt_soc_with_uuc);
    hwlog_err("batt_soc_est = %d\n", di->batt_soc_est);
    hwlog_err("batt_id_vol = %d\n", di->batt_id_vol);
    hwlog_err("batt_chargecycles = %d\n", di->batt_chargecycles);
    hwlog_err("last_cali_temp = %d\n", di->last_cali_temp);
    hwlog_err("cc_end_value = %d\n", di->cc_end_value);
    //hwlog_err("cc_start_value = %d\n", di->cc_start_value);
    hwlog_err("v_cutoff = %d\n", di->v_cutoff);
    hwlog_err("v_low_int_value = %d\n", di->v_low_int_value);
    hwlog_err("get_cc_end_time = %d\n", di->get_cc_end_time);
    hwlog_err("suspend_time = %d\n", di->suspend_time);
    hwlog_err("charging_begin_cc = %d\n", di->charging_begin_cc);
    hwlog_err("suspend_cc = %d\n", di->suspend_cc);
    hwlog_err("last_time = %d\n", di->last_time);
    hwlog_err("last_cc = %d\n", di->last_cc);
    hwlog_err("last_iavg_ma = %d\n", di->last_iavg_ma);
    hwlog_err("fcc_real_mah = %d\n", di->fcc_real_mah);
    hwlog_err("is_nv_read = %d\n", di->is_nv_read);
    hwlog_err("is_nv_need_save = %d\n", di->is_nv_need_save);
    hwlog_err("dbg_ocv_cng_0 = %d\n",di->dbg_ocv_cng_0);
    hwlog_err("dbg_ocv_cng_1 = %d\n",di->dbg_ocv_cng_1);
    hwlog_err("dbg_valid_vol = %d\n",di->dbg_valid_vol);
    hwlog_err("dbg_invalid_vol = %d\n",di->dbg_invalid_vol);
    hwlog_err("dbg_ocv_fc_failed = %d\n",di->dbg_ocv_fc_failed);

}

int ss_offset_show(void)
{
    struct smartstar_coul_device *di =(struct smartstar_coul_device *) g_smartstar_coul_dev;

    hwlog_info("Present offset: C_offset: a=%d  b=%d uA; V_offset: a=%d, b=%d uV; r_pcb=%d\n",
		 c_offset_a, c_offset_b, v_offset_a, v_offset_b, di->r_pcb);

#if 0
    struct hisi_nve_info_user nve;
    int ret = 0;
    struct ss_coul_nv_info *pinfo = (struct ss_coul_nv_info *)&nve.nv_data[0];
    memset(&nve, 0, sizeof(nve));
    strcpy(nve.nv_name, SS_COUL_NV_NAME);
    nve.nv_number = SS_COUL_NV_NUM;
    nve.valid_size = sizeof(*pinfo);
    nve.nv_operation = NV_READ;
    ret = hisi_nve_direct_access(&nve);
    if (ret) {
        hwlog_err("read nv partion failed, ret=%d\n", ret);
	        return -1;
    }
    hwlog_info("NV offset: C_offset: a=%d, b=%d uA V_offset: a=%d, b=%d uV; r_pcb=%d\n",
	        pinfo->c_offset_a, pinfo->c_offset_b, pinfo->v_offset_a, pinfo->v_offset_b, pinfo->r_pcb);
#endif

    return 0;

}
int ss_offset_set(int c_a, int c_b, int v_a, int v_b, int r_pcb)
{
	struct smartstar_coul_device *di =(struct smartstar_coul_device *) g_smartstar_coul_dev;

	c_offset_a = c_a;
	c_offset_b = 0;
	v_offset_a = v_a;
	v_offset_b = 0;
	di->r_pcb = r_pcb;
       hwlog_info("C_offset: a=%d, b=%d uA; V_offset: a=%d, b=%d uV; r_pcb=%d\n",
		 c_offset_a, c_offset_b, v_offset_a, v_offset_b, di->r_pcb);

       return 0;
}

int ss_realfcc_show(void)
{
	struct smartstar_coul_device *di = (struct smartstar_coul_device *)g_smartstar_coul_dev;
    int i;
    struct single_row_lut *preal_fcc_lut = di->adjusted_fcc_temp_lut;

    if (preal_fcc_lut == NULL){
        hwlog_err("no real fcc data in nv\n");
        return 0;
    }

    for (i=0; i<preal_fcc_lut->cols; i++)
    {
        hwlog_info("temp=%d real_fcc=%d\n", preal_fcc_lut->x[i], preal_fcc_lut->y[i]);
    }

	return di->fcc_real_mah;
}

int ss_realfcc_set(int fcc_mah)
{
    struct smartstar_coul_device *di = (struct smartstar_coul_device *)g_smartstar_coul_dev;

    hwlog_info("ss_realfcc_set fcc_mah = %d\n", fcc_mah);

    if (fcc_mah == 0){
        di->adjusted_fcc_temp_lut = NULL;
        di->batt_limit_fcc = 0;
    }
    else{
    	di->fcc_real_mah = fcc_mah;
    	readjust_fcc_table(di);
    }

    save_nv_info(di);
    return 0;
}

void ss_pmu_mode_show(void)
{
    unsigned char val = SMARTSTAR_REG_READ(COUL_WORK_MODE_ADDR);
    hwlog_info("PMU work Mode Register Value=[0x%x]\n", val);

    if( (g_coul_work_mode & PMU_WORK_MODE_MSK) == TCXO_MODE  )
    {
        hwlog_info("PMU work Mode [==TCXO==]\n");
    }
    else
    {
        hwlog_info("PMU work Mode [**DCXO**]\n");
    }
}

void ss_eco_show(void)
{
    unsigned int in_val = 0;
    unsigned int out_val = 0;
    int eco_in_uah = 0;
    int eco_out_uah = 0;
    int present_in_uah = 0;
    int present_out_uah = 0;
    int rst_uah = 0;
    int eco_uah = 0;
    int cur_uah = 0;


    SMARTSTAR_REGS_READ(SMARTSTAR_ECO_OUT_CLIN_REG_BASE, &in_val, 4);
    SMARTSTAR_REGS_READ(SMARTSTAR_ECO_OUT_CLOUT_REG_BASE, &out_val, 4);

    /* calc cc at the point of existing from eco */
    eco_in_uah = convert_regval2uah(in_val);
    eco_out_uah = convert_regval2uah(out_val);
    eco_uah = eco_out_uah - eco_in_uah;
    /* calc current cc  */
    SMARTSTAR_REGS_READ(SMARTSTAR_CL_OUT_BASE, &in_val, 4);
    SMARTSTAR_REGS_READ(SMARTSTAR_CL_IN_BASE, &out_val, 4);
    present_in_uah = convert_regval2uah(in_val);
    present_out_uah = convert_regval2uah(out_val);
    cur_uah = present_out_uah - present_in_uah;

	/* leak cc between exist eco mode and calc soc */
    rst_uah = cur_uah - eco_uah;
    hwlog_info("eco_in=%d,eco_out=%d,present_in=%d,present_out=%d,leak=%d\n",
                                           eco_in_uah, eco_out_uah,present_in_uah, present_out_uah, rst_uah);

    return;
}

#endif



MODULE_AUTHOR("HUAWEI");
MODULE_DESCRIPTION("hisi coul module driver");
MODULE_LICENSE("GPL");
