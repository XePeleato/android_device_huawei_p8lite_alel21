/*
 * Device driver for PMU DRIVER	 in HI6xxx SOC
 *
 * Copyright (c) 2011 Hisilicon Co. Ltd
 *
 * information about chip-hi6552 which not in hixxx_pmic dts
 *
 */

#ifndef __PMIC_MNTN_H__
#define __PMIC_MNTN_H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/irqnr.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/rtc.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/hardirq.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/mfd/hisi_hi6xxx_pmic.h>
#include <linux/string.h>
#include <asm/uaccess.h>

#include "soc_smart_interface.h"

#include "../hisi/mntn/excDrv.h"
#include <linux/hisi/util.h>

#define SET_REG_BIT(reg_value, shift, mask, value)  (reg_value = (reg_value & (~((((unsigned int)1 << mask) - 1) << shift)))\
                                                        |((unsigned char)((value) & (((unsigned int)1 << mask) - 1)) << (shift)))
														
#define IRQ_MAX_SIZE (3)
#define RECORD_MAX_SIZE (6)
#define PMIC_REGISTER_DATA_WIDTH  (8)
#define PMIC_PRINT_BUF_SIZE             (4*1024)
#define PMIC_DUMP_FILE_PATH             ("/data/hisi_logs/ap_log/pmu.txt")
#define PMU_SHELL_SCRIPT                ("/system/etc/log/pmu_reboot.sh")
#define PMU_DUMP_FILE_MAX_SIZE           (2*1024*1024)
#define PMIC_DEV_NAME_LEN         (8)

#define PMIC_MNTN_OK     (0)
#define PMIC_MNTN_ERR    (-1)

typedef enum EM_PMIC_NP_REG_ID_E
{
    NP_IRQ1_RECORD_ID,
    NP_IRQ2_RECORD_ID,
    NP_IRQ3_RECORD_ID,
    NP_SCP_RECORD1_ID,
    NP_OCP_RECORD1_ID,
    NP_OCP_RECORD2_ID,
    NP_OCP_RECORD3_ID,
    NP_OCP_RECORD4_ID,
    NP_OCP_RECORD5_ID,
    NP_MAX_SIZE
}EM_PMIC_NP_REG_ID;

enum pmic_status_type
{
    PMIC_HEALTH_STATUS_NORMAL,
    PMIC_HEALTH_IRQ_EXCH_HAPPENED,
    PMIC_HEALTH_NP_EXCH_HAPPENED,
    PMIC_HEALTH_SMPL_EXCH_HAPPENED
};

struct pmic_dev_stat
{
    char dev_name[PMIC_DEV_NAME_LEN];
    int  dev_stat;
    struct pmic_dev_stat *next;
};

typedef struct
{
    struct pmic_dev_stat *head;
    struct pmic_dev_stat *tail;
}PMIC_DEV_INFO;

struct pmic_regs
{
    unsigned int start_addr;
    unsigned int size;/*int*/
};

struct record_2_disable_reg
{
    unsigned int record_reg;
    unsigned int dis_reg;
};

typedef struct pmic_exch_reg_desc
{
    unsigned int reg_addr;
    struct device_node* exch_reg_node;
    unsigned char inacceptable_event;
    char** event_bit_name;
    unsigned char dmd_not_report_event;
}PMIC_EXCH_REG;

typedef struct
{
    unsigned int reg_addr;
    unsigned int shift;
    unsigned int mask;
}PMIC_SMPL_REG_CTRL;

typedef struct
{
    unsigned int enable;
    unsigned int time_cfg;
    char last_record;
    PMIC_SMPL_REG_CTRL en_ctrl;
    PMIC_SMPL_REG_CTRL time_ctrl;
    PMIC_SMPL_REG_CTRL record_ctrl;
}PMIC_SMPL_DESC;

typedef struct pmic_mntn_desc
{
    struct pmic_regs np_regs;
    char np_data_buf[NP_MAX_SIZE];

    struct pmic_regs irq_regs;
    char irq_data_buf[IRQ_MAX_SIZE];

    struct pmic_regs record_regs;
    char record_data_buf[RECORD_MAX_SIZE];
	
    PMIC_SMPL_DESC *smpl_desc;

    unsigned int data_width;
    struct device_node *np;

    int ocp_scp_irq;
    struct workqueue_struct *ocp_scp_wq;
    struct work_struct ocp_scp_wk;
    int health;

    int otmp_set;
    int otmp_irq;
    struct workqueue_struct *otmp_wq;
    struct work_struct otmp_wk;

    unsigned int exch_reg_size;
    PMIC_EXCH_REG* exch_reg_desc;

    unsigned int mntn_later_time;
    struct workqueue_struct *mntn_later_init_wq;
    struct delayed_work mntn_later_init_wk;
    struct kobject *mntn_kobj;
    char* init_log_show;
    char* irq_log_show;

    unsigned char dmd_event;
}PMIC_MNTN_DESC;

#endif
