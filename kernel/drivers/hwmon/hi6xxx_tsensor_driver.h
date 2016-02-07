/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : tsensor_driver.h
  版 本 号   : 初稿
  作    者   : 刘龙l00217270
  生成日期   : 2013年5月14日
  最近修改   :
  功能描述   : tsensor_driver.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年5月14日
    作    者   : 刘龙l00217270
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/


#ifndef __TSENSOR_DRVIER_H__
#define __TSENSOR_DRVIER_H__

#include <linux/err.h>
#include <linux/io.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/stddef.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/mutex.h>

#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/reboot.h>
#include <linux/slab.h>
//#include <linux/pm_qos_params.h>
#include <linux/pm_qos.h>
#include <linux/delay.h>
#include <linux/timer.h>
#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define TSENSOR_FEATURES_OFF        (0)
#define TSENSOR_FEATURES_ON         (1)

#define TSENSOR_DEBUG_OFF           (0)
#define TSENSOR_DEBUG_ON            (1)

#define TSENSOR_OK          (0)
#define TSENSOR_ERR         (-1)

#define FREQ_DOWN      (1)
#define FREQ_UP        (-1)

#define TSENSOR_INIT_NOT_OK   (0)
#define TSENSOR_INIT_OK       (1)

#define TSENSOR_INVALID_INDEX  (255)

#define TSENSOR_NORMAL_MONITORING_RATE  (1000)
#define TSENSOR_LATE_INIT_DELAY        (30000)

#define TSENSOR_READ_TEMP_COUNT         (3)

#define LOCK_FREQ_ON                    (1)
#define LOCK_FREQ_OFF                   (0)

/*tsensor修正值转换宏*/
#define REMOTE_GPU_TRIM_DATA_OFFSET                  26
#define REMOTE_GPU_TRIM_DATA_MASK                    0x7C000000
#define REMOTE_GPU_TRIM_DATA_PLUSMINUS               0x80000000

#define REMOTE_ACPU_C1_TRIM_DATA_OFFSET              20
#define REMOTE_ACPU_C1_TRIM_DATA_MASK                0x01F00000
#define REMOTE_ACPU_C1_TRIM_DATA_PLUSMINUS           0x02000000

#define REMOTE_ACPU_C0_TRIM_DATA_OFFSET              14
#define REMOTE_ACPU_C0_TRIM_DATA_MASK                0x0007C000
#define REMOTE_ACPU_C0_TRIM_DATA_PLUSMINUS           0x00080000

#define LOCAL_TRIM_DATA_OFFSET                       8
#define LOCAL_TRIM_DATA_MASK                         0x00001F00
#define LOCAL_TRIM_DATA_PLUSMINUS                    0x00002000

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
typedef enum {
    TSENSOR_ALARAM_OFF = 0,
    TSENSOR_ALARAM_ON = 1,
    TSENSOR_ALARAM_BUFF,
}ENUM_TSENSOR_ALARM;

/*sensor_config[ID]:Tsensor ID*/
/*Tsensor Consumer*/
typedef enum {
    TSENSOR_TYPE_ACPU_CLUSTER0 = 0,
    TSENSOR_TYPE_ACPU_CLUSTER1 = 1,
    TSENSOR_TYPE_GPU,
    TSENSOR_TYPE_LOCAL,
    TSENSOR_TYPE_BUFF,
}ENUM_TSENSOR_TYPE;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
typedef struct
{
    unsigned int sensor_type;   /*类型:ENUM_TSENSOR_TYPE*/
    unsigned int sel;           /*Tsensor0测试模式配置*/
    int          lag_value;     /*滞回温度*/
    int          thres_value;   /*告警温度*/
    int          reset_value;   /*复位温度*/

    int          lag_cfg_value;     /*滞回温度对应的寄存器配置值*/
    int          thres_cfg_value;   /*告警温度对应的寄存器配置值*/
    int          reset_cfg_value;   /*复位温度对应的寄存器配置值*/

    unsigned int alarm_cur_cnt;
    unsigned int alarm_cnt;    /*告警->限频计数*/

    unsigned int recover_cur_cnt;
    unsigned int recover_cnt;  /*取消告警计数*/

    unsigned int is_alarm;     /*是否处于告警状态*/
    int temp_t0;
}SENSOR_INFO;

typedef struct
{
    unsigned int  enable;      /*温度保护功能是否使能*/
    unsigned int  init_flag;
    void __iomem  *virt_base_addr;

    unsigned int  num;         /*个数*/
    SENSOR_INFO   *sensor_config;

    struct mutex  get_tmp_lock;    /*获取温度时的互斥锁*/
    unsigned int  cur_cfg_index;   /*当前检测的sensor*/
    unsigned int  cycle_cnt;       /*轮询count计数*/

    unsigned int  temp_prt_vote;   /*温度保护投票，有任意一个测点过温，加快轮询速度*/

    unsigned int  acpufreq_default_limit_value;
    int  acpu_freq_limit_num;
    unsigned int  *acpu_freq_limit_table;

    unsigned int  gpufreq_default_limit_value;
    unsigned int  gpu_freq_limit_num;
    unsigned int  *gpu_freq_limit_table;

    int  cur_acpu_freq_index;
    unsigned int  cur_gpu_freq_index;
    unsigned int  cur_ddr_freq_index;
    int  temp_offset_benchmark;

}TRMP_PRT_INFO;

struct tsensor_devinfo {
    struct platform_device *pdev;
    struct device *dev;
};

struct tsensor_governor {
    struct mutex lock;
    struct delayed_work tsensor_late_init_work;
    struct delayed_work tsensor_monitor_work;
    int average_period;
    int debug_switch;
    int warning_count;
};

struct efuse_trim
{
   unsigned int trimdata;
   int local;
   int remote_acpu_c0;
   int remote_acpu_c1;
   int remote_gpu;
};

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

int tsensor_temp_get(ENUM_TSENSOR_TYPE tsensor_type, int *temperature);
void tsensor_temp_check(void);
int tsensor_config_init(struct platform_device *pdev);
int tsensor_init(struct platform_device *pdev);
void tsensor_late_init(void);
int tsensor_suspend(void);
int tsensor_resume(void);
void efuse_trim_init(void);
void tsensor_monitor_work_fn(struct work_struct *work);
void tsensor_switch(int on_or_off);
int tsensor_status(void);


#endif /* end of tsensor_driver.h */
