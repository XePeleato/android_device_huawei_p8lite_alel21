/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_temp_cfg.h
  版 本 号   : 初稿
  作    者   : 刘龙l00217270
  生成日期   : 2013年5月6日
  最近修改   :
  功能描述   : drv_temp_cfg.h 的头文件,定义DDR共享内存数据结构
  函数列表   :
  修改历史   :
  1.日    期   : 2013年5月6日
    作    者   : 刘龙l00217270
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef __DRV_TEMP_CFG_H__
#define __DRV_TEMP_CFG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define HKADC_LOGIC_CHAN_MAX 32 /* 最大逻辑通道号 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*定义hkadc物理通道号*/
#if defined(CHIP_BB_HI6210)
typedef enum hkadcChan
{
    HKADC_CHAN_INVALID = -1,
    HKADC_CHAN_0 = 0x00,
    HKADC_CHAN_1,
    HKADC_CHAN_2,
    HKADC_CHAN_3,
    HKADC_CHAN_4,
    HKADC_CHAN_5,
    HKADC_CHAN_6,
    HKADC_CHAN_7,
    HKADC_CHAN_8,
	HKADC_CHAN_9,
	HKADC_CHAN_10,
    HKADC_CHAN_11,
	HKADC_CHAN_12,

    HKADC_CHAN_MAX
}HKADCCHANNUM;
#else
#define HKADC_CHAN_MAX 16
#define TSENS_REGION_MAX 3
#endif
/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
struct drv_hkadc_map
{
    unsigned int logic_chan;
    unsigned int phy_chan;
    unsigned int nv_id;
    unsigned int nv_len;
    unsigned int modem_id;
    char * name;
};

/* 温度输出区结构 */
typedef struct hkadc_chan_out
{
    unsigned short volt_l;    /* 返回的HKADC电压值，单位:毫伏 */
    unsigned short volt_r;   /* 返回的HKADC电压值取反 */
    short          temp_l;     /* 返回的温度值，单位:摄氏度 */
    short          temp_r;    /* 返回的温度值取反 */
}DRV_CHANNEL_OUTPUT;

/* 温度电压对应表结构 */
typedef struct hkadc_tem_value
{
    short   temp;             /* 温度点，单位:摄氏度 */
    unsigned short volt;      /* 电压点，单位:毫伏 */
}TEM_VOLT_TABLE;

/* 输出配置bit结构 */
typedef struct hkadc_out_config
{
   unsigned int out_prop:2;
   unsigned int out_wake:1;
   unsigned int out_vol_temp:1;
   unsigned int out_reserv1:4;
   unsigned int out_acore:1;
   unsigned int out_ccore:1;
   unsigned int out_reserv2:22;
} OUT_PROP_CONFIG;

/* 温度配置区结构 */
typedef struct hkadc_chan_config
{
    union
    {
        unsigned int out_config;       /* bit0-bit1 0:不输出 1:单次输出 2:循环输出 */
                                       /* bit2 1:唤醒输出 0:非唤醒输出 */
                                       /* bit3: 0:只输出电压 1:电压温度均输出 */
                                       /* bit8: A核输出 */
                                       /* bit9: C核输出 */
        struct hkadc_out_config bit_out_config;
    } outcfg;

    unsigned int have_config;
    unsigned short out_peroid;     /* 循环输出时的循环周期，单位:秒 */
    unsigned short temp_data_len;  /* 温度转换表长度 */
    struct hkadc_tem_value temp_table[32]; /* 温度转换表，实际长度参见temp_data_len */
} DRV_CHANNEL_CONFIG;

#if defined(CHIP_BB_HI6210)
/* 系统放电温度保护配置结构 */
typedef struct sys_temp_cfg
{
    unsigned short enable;       /* bit0:高温保护使能 bit1:低温保护使能 1 使能 0 关闭*/
    unsigned short hkadc_id;      /* 系统放电温度保护的hkadc通道ID */
    unsigned short high_thres;     /* 系统放电高温保护的电压阀值 ，温度越高，电压越低*/
    unsigned short high_count;    /* 系统放电高温保护次数上限，到了后系统关机 */
    unsigned short low_thres;      /* 系统放电低温保护的电压阀值 ，温度越低，电压越高*/
    unsigned short low_count;     /* 系统放电低温保护次数上限，到了后系统关机 */
    unsigned int   reserved[2];   /*保留*/
} DRV_SYS_TEMP_PRO;

struct hkadc_nv_config
{
    unsigned int outconfig;         /* bit0-bit1 0:不输出 1:单次输出 2:循环输出 */
                                       /* bit2 1:唤醒输出 0:非唤醒输出 */
                                       /* bit3: 0:只输出电压 1:电压温度均输出 */
                                       /* bit8: A核输出 */
                                       /* bit9: C核输出 */
    unsigned short outperiod;          /* 循环输出时的循环周期，单位:秒 */
    unsigned short convertlistlen;     /* 温度转换表长度 */
    unsigned int   reserved[2];        /* 温度转换表，实际长度参见usTempDataLen */
};


typedef struct thermal_data_area
{
    unsigned int       magic_start;    /* 分区起始Magic Code 0x5A5A5A5A*/
    struct hkadc_chan_out chan_out[HKADC_CHAN_MAX];  /* 各通道输出区域,0x4开始*/ /*monan:16 是否改为HKADC_CHAN_MAX ?*/
    struct hkadc_chan_config chan_cfg[HKADC_CHAN_MAX];  /* 各通道配置区域 */
    struct sys_temp_cfg   sys_temp_cfg;    /* 系统放电温保配置 */
    unsigned int       hw_ver_id;       /* 版本ID */
    unsigned char      phy_tbl[HKADC_LOGIC_CHAN_MAX];  /*物理通道->逻辑通道的映射关系*/
    unsigned int       debug_flag;     /* 调测开关 */
    unsigned short     adc_value[HKADC_CHAN_MAX];
    unsigned short     acpu_temp;
    unsigned short     gpu_temp;
    unsigned int       last_slice;     /*A、M核同时读HKADC需要保证任意两次读HKADC的间隔大于1毫秒，通过这个记录保证A、M读取的间隔*/
    unsigned int       magic_end;      /* 分区结束Magic Code 0x5A5A5A5A*/
} DRV_HKADC_DATA_AREA;
#else
/* 系统放电温度保护配置结构 */
typedef struct sys_temp_cfg
{
    unsigned short enable;        /* bit0:高温保护使能 bit1:低温保护使能 1 使能 0 关闭*/
    unsigned short hkadc_id;      /* 系统放电温度保护的hkadc通道ID */
    short          high_thres;    /* 系统放电高温保护的电压阀值 ，温度越高，电压越低*/
    unsigned short high_count;    /* 系统放电高温保护次数上限，到了后系统关机 */
    short          low_thres;     /* 系统放电低温保护的电压阀值 ，温度越低，电压越高*/
    unsigned short low_count;     /* 系统放电低温保护次数上限，到了后系统关机 */

    unsigned int   reserved[2];   /*保留*/
} DRV_SYS_TEMP_PRO;

/* TSENEOR温度保护配置结构 */
typedef struct tsens_region_cfg
{
    unsigned short enable;         /* bit0:高温保护使能 bit1:低温保护使能 1 使能 0 关闭*/
    unsigned short high_thres;     /* 系统放电高温保护*/
    unsigned short high_count;     /* 系统放电高温保护次数上限，系统关机 */
    unsigned short reserved;
    /*unsigned short low_thres;*/     /* 系统放电低温保护*/
    /*unsigned short low_count;*/     /* 系统放电低温保护次数上限，系统关机 */
} TSENS_REGION_CFG;

typedef struct tsens_region_config
{
    short temp[256];

}TSENS_REGION_CONFIG;

/* 共享温度区数据结构 */
typedef struct thermal_data_area
{
    unsigned long               magic_start;                 /* 分区起始Magic Code 0x5A5A5A5A*/
    struct hkadc_chan_out       chan_out[HKADC_CHAN_MAX];    /* 各通道输出区域*/
    struct hkadc_chan_config    chan_cfg[HKADC_CHAN_MAX];    /* 各通道配置区域 */
    struct sys_temp_cfg         sys_temp_cfg;                /* 系统放电温保配置 */
    unsigned long               hw_ver_id;                   /* 版本ID */
    unsigned char               phy_tbl[HKADC_LOGIC_CHAN_MAX];     /* HKADC逻辑通道到物理通道对应表 */
    unsigned long               debug_flag;                  /* 调测开关 */
    struct tsens_region_cfg     tens_cfg[TSENS_REGION_MAX];  /* TSENSOR温保配置 */
    struct tsens_region_config  region_cfg;                  /* TSENSOR各区域的配置信息*/
    short                       tsens_out[TSENS_REGION_MAX]; /* TSENSOR各区域的输出信息*/
    short                       reserved;                    /* 保留信息*/
    unsigned long               magic_end;                   /* 分区结束Magic Code 0x5A5A5A5A*/
} DRV_HKADC_DATA_AREA;
#endif

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

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif

