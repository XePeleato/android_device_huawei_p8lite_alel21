/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       :   drv_nv_def.h
  Description     :   DRV组件的NV 结构体定义
  History         :
******************************************************************************/

#ifndef __DRV_NV_DEF_H__
#define __DRV_NV_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define LDO_GPIO_MAX 2

#ifndef s8
typedef signed char s8;
#endif
#ifndef u8
typedef unsigned char u8;
#endif
#ifndef s16
typedef signed short s16;
#endif
#ifndef u16
typedef unsigned short u16;
#endif
#ifndef s32
typedef signed int s32;
#endif
#ifndef u32
typedef unsigned int u32;
#endif
#ifndef s64
typedef signed long long s64;
#endif
#ifndef u64
typedef unsigned long long u64;
#endif
/*******************************************************************/

/*****************************************************************************
 结构名    : nv_protocol_base_type
 结构说明  : nv_protocol_base_type结构 ID= en_NV_Item_Modem_Log_Path 148
 			MBB形态 modem log路径名，用于区分新老形态的modem log路径。
*****************************************************************************/
/*NV ID = 50018*/
#pragma pack(push)
#pragma pack(1)
	typedef struct
	{
		s32 	   nvStatus;
		s8		   nv_version_info[30];
	}NV_SW_VER_STRU;
#pragma pack(pop)	
/*END NV ID = 50018*/

typedef struct
{
   char  ucModemLogPath[32];
   char  modemLogRsv[4];
}NV_MODEM_LOG_PATH;



/*NV ID = 0xd109*/
/*值为1则为打开，值为0则为关闭*/
typedef struct
{
	s32 buck2_switch;    /*[0, 1]*/
}NV_PASTAR_BUCK2_SWITCH_STRU;
/*end NV ID = 0xd109*/

/*NV ID  = 0xd10b*/

typedef struct ST_PWC_SWITCH_STRU_S {

	/*以下NV用于低功耗的整体控制，其中有些BIT暂时未用，做他用时，请更正为准确的名称*/
 	u32 deepsleep  :1; /*bit0*/
    u32 lightsleep :1; /*bit1*/
    u32 dfs        :1; /*bit2*/
    u32 hifi       :1; /*bit3*/
    u32 drxAbb     :1; /*bit4*/
    u32 drxZspCore :1; /*bit5*/
    u32 drxZspPll  :1; /*bit6*/
    u32 drxWLBbpPll  :1; /*bit7*/
    u32 drxGBbpPll   :1; /*bit8*/
    u32 drxRf      :1; /*bit9*/
    u32 drxPa      :1; /*bit10*/
    u32 drxGuBbpPd   :1; /*bit11*/
    u32 drxDspPd     :1; /*bit12*/
    u32 drxLBbpPd    :1; /*bit13*/
    u32 drxPmuEco    :1; /*bit14*/
    u32 drxPeriPd    :1; /*bit15*/
    u32 l2cache_mntn  :1; /*bit16*/
    u32 bugChk     :1; /*bit17*/
    u32 pmuSwitch     :1; /*bit18*/
    u32 drxLdsp      :1;  /*bit 19*/
    u32 matserTDSpd  :1; /*bit20*/
    u32 tdsClk    :1;  /*bit21*/
    u32 slaveTDSpd   :1; /*bit22*/
    u32 slow	     :1;/*bit23*/
    u32 reserved    :8; /*bit24-31*/

	/*以下NV用于DEBUG上下电和开关钟*/
	u32 drx_pa0_pd       :1; /*bit0 用于控制PA0的上下电*/
    u32 drx_pa1_pd       :1; /*bit1 用于控制PA1的上下电*/
    u32 drx_rfic0_pd     :1; /*bit2 用于控制RFIC0的上下电*/
    u32 drx_rfic1_pd     :1; /*bit3 用于控制RFIC1的上下电*/
    u32 drx_irm_pd       :1; /*bit4 用于控制BBP_IRM的上下电*/
    u32 drx_bbe16_pd     :1; /*bit5 用于控制BBE16的上下电*/
    u32 drx_abb_pd       :1; /*bit6 用于控制ABB的上下电*/
    u32 drx_g1bbp_pd     :1; /*bit7 用于控制G1_BBP的上下电*/
    u32 drx_g2bbp_pd     :1; /*bit8 用于控制G2_BBP的上下电*/
    u32 drx_wbbp_pd      :1; /*bit9 用于控制WBBP的上下电*/
    u32 drx_bbpcommon_pd :1; /*bit10 用于控制BBP_COMMON的上下电*/
    u32 drx_twbbp_pd     :1; /*bit11 用于控制TWBBP的上下电*/
    u32 drx_bbe16_pll    :1; /*bit12 用于控制BBE16_PLL的开关钟*/
    u32 drx_bbp_pll      :1; /*bit13 用于控制BBP_PLL的开关钟*/
    u32 drx_abb0_wpll    :1; /*bit14 用于控制ABB_CH0_WPLL的开关钟*/
    u32 drx_abb0_gpll    :1; /*bit15 用于控制ABB_CH0_GPLL的开关钟*/
    u32 drx_abb1_wpll    :1; /*bit16 用于控制ABB_CH1_WPLL的开关钟*/
    u32 drx_abb1_gpll    :1; /*bit17 用于控制ABB_CH1_GPLL的开关钟*/
    u32 reserved2        :14; /*bit18-31 未用*/
}ST_PWC_SWITCH_STRU;


/*NV ID = 0xd10c*/
typedef struct ST_PWC_DFS_STRU_S {
    u32 CcpuUpLimit;
	u32 CcpuDownLimit;
	u32 CcpuUpNum;
	u32 CcpuDownNum;
	u32 AcpuUpLimit;
	u32 AcpuDownLimit;
	u32 AcpuUpNum;
	u32 AcpuDownNum;
	u32 DFSTimerLen;
	u32 DFSHifiLoad;
 	u32 Strategy;/*使用何种策略bit0:1->200ms负载检测,bit0:0->4s负载检测;bit1:1/0打开/关闭辅助DDR调频*/
 	u32 DFSDdrUpLimit;
 	u32 DFSDdrDownLimit;
 	u32 DFSDdrprofile;
 	u32 reserved;
}ST_PWC_DFS_STRU;

/*NV ID = 0xd10f begin，配置PMU出现异常时的处理配置*/
typedef struct
{
    u8  VoltId;         /*需要特殊配置的电压源的id号*/
    u8  VoltOcpIsOff;   /*若该电压源过流的话是否关闭该路电压源*/
    u8  VoltOcpIsRst;   /*若该电压源过流的话是否重启系统*/
    u8  VoltOtpIsOff;   /*若PMU过热需要关闭非核心电源的话，是否能关闭该路电压源*/
} PMU_VOLT_PRO_STRU;
typedef struct
{
    u8    ulOcpIsOn;        /*过流的电源能否被重新打开:0:不能被重新打开；1:能被重新打开，默认为不能打开--0*/
    u8    ulOcpIsOff;       /*过流的电源是否关闭:0:不关闭；1:关闭，默认为关闭--1*/
    u8    ulOtpCurIsOff;    /*过温(超过温度预警值)时是否关闭非核心电源:0:不关闭非核心，1:关闭非核心电源。默认为关闭非核心电源--1*/
    u8    ulOtpIsRst;       /*过温(超过温度预警值)时是否重启系统:0:不重启，1:重启系统。默认为不重启系统--0*/

    u8    ulOtpIsOff;       /*PMU芯片结温超过150℃时PMU是否下电(测试时可配置):0:PMU不下电，1:PMU下电。默认为PMU下电--1*/
    u8    ulUvpIsRst;       /*欠压预警时是否重启系统，默认为0:不重启*/
    u16   reserved2;

    u16   ulOtpLimit;       /*温度预警阈值设置:105:105℃，115:115℃，125:125℃ ，135:135℃ (HI6559增加支持135)。默认为125℃--125 */
    u16   ulUvpLimit;       /*欠压预警阈值设置，单位mv:3000:3v; 2700:2.7v(2850:2.85v.HI6559支持2.85,HI6551支持2.7) ，默认为3v--3000*/

    PMU_VOLT_PRO_STRU VoltProConfig[50];/*每路电压源的异常保护策略配置，不同产品形态需要特殊配置的电压源配置*/
} PMU_EXC_PRO_NV_STRU;

/*NV ID = 0xd10f end*/

/*ID=0xd110*/
typedef struct
{
    u32    u32CalcTime;        /* 计算时间周期(10ms) */
    u32    u32PktNum;          /* 累计包个数 */
    u32    u32SwichFlag;       /* netif包上送适配开关 */
} NETIF_INIT_PARM_T;

/*ID=0xd111 begin */
typedef struct
{
    u32 dump_switch    : 2; /* 00: excdump, 01: usbdump, 1x: no dump */
    u32 ARMexc         : 1; /* 2 ARM异常检测开关*/
    u32 stackFlow      : 1; /* 3 堆栈溢出检测开关*/
    u32 taskSwitch     : 1; /* 4 任务切换记录开关*/
    u32 intSwitch      : 1; /* 5 中断记录开关*/
    u32 intLock        : 1; /* 6 锁中断记录开关*/
    u32 appRegSave1    : 1; /* 7 寄存器组1记录开关 */
    u32 appRegSave2    : 1; /* 8 寄存器组2记录开关*/
    u32 appRegSave3    : 1; /* 9 寄存器组3记录开关*/
    u32 commRegSave1   : 1; /* 10 寄存器组1记录开关 */
    u32 commRegSave2   : 1; /* 11 寄存器组2记录开关*/
    u32 commRegSave3   : 1; /* 12 寄存器组3记录开关*/
    u32 sysErrReboot   : 1; /* 13 systemError复位开关*/
    u32 reset_log      : 1; /* 14 强制记录开关，暂未使用*/
    u32 fetal_err      : 1; /* 15 强制记录开关，暂未使用*/
	u32 log_ctrl       : 2; /* bsp_trsce 输出控制*/
    u32 reserved1      : 14;
} DUMP_CFG_STRU;

typedef struct
{
    union
    {
        u32         uintValue;
        DUMP_CFG_STRU   cfg;
    } dumpCfg;

    u32 appRegAddr1;	/* ACORE保存寄存器组地址1*/
    u32 appRegSize1;	/* ACORE保存寄存器组长度1*/
    u32 appRegAddr2;	/* ACORE保存寄存器组地址2*/
    u32 appRegSize2;	/* ACORE保存寄存器组长度2*/
    u32 appRegAddr3;	/* ACORE保存寄存器组地址3*/
    u32 appRegSize3;	/* ACORE保存寄存器组长度3*/

    u32 commRegAddr1;	/* CCORE保存寄存器组地址1*/
    u32 commRegSize1;	/* CCORE保存寄存器组长度1*/
    u32 commRegAddr2;	/* CCORE保存寄存器组地址2*/
    u32 commRegSize2;	/* CCORE保存寄存器组长度2*/
    u32 commRegAddr3;	/* CCORE保存寄存器组地址3*/
    u32 commRegSize3;	/* CCORE保存寄存器组长度3*/

    u32 traceOnstartFlag;           /* 0:开机启动Trace, 非0:开机不启动Trace */
    u32 traceCoreSet;               /* 0:采集A核Trace, 1:采集C核Trace, 2:采集双核Trace */
    u32 BusErrorFlagSet;             /* 0:开机不启动防总线挂死功能, 非0:开机启动防总线挂死功能 */
} NV_DUMP_STRU;
/*ID=0xd111 end */

/*NV ID = 0xd114 begin,配置PMU初始化时应该由软件实现的基础配置*/
#define NUM_OF_PMU_NV  50
typedef struct
{
    u8  VoltId;     /*电源id号*/
    u8  IsNeedSet;  /*是否需要软件设置:0:不需要；1:需要*/
    u8  IsOnSet;    /*默认是否需要开启电压源:0:不需要；1:需要*/
    u8  IsOffSet;   /*默认是否需要关闭电压源:0:不需要；1:需要*/

    u8  IsVoltSet;  /*是否需要设置电压:0:不需要；1:需要*/
    u8  IsEcoSet;   /*是否需要设置ECO模式:0:不需要；1:需要*/
    u8  EcoMod;     /*需要设置的eco模式:0:normal;2:force_eco;3:follow_eco*/
    u8  reserved3;  /*默认*/

    u32 Voltage;    /*需要设置的默认电压*/
} PMU_INIT_CON_STRU;
typedef struct
{
    PMU_INIT_CON_STRU InitConfig[NUM_OF_PMU_NV];
} PMU_INIT_NV_STRU;
/*NV ID = 0xd114 end*/

/*NV ID = 0xd115 start*/

typedef struct {
    u32	index;           /*硬件版本号数值(大版本号1+大版本号2)，区分不同产品*/
    u32	hwIdSub;        /*硬件子版本号，区分产品的不同的版本*/
	char  name[32];           /*内部产品名*/
    char	namePlus[32];       /*内部产品名PLUS*/
    char	hwVer[32];          /*硬件版本名称*/
    char	dloadId[32];        /*升级中使用的名称*/
    char	productId[32];      /*外部产品名*/
}PRODUCT_INFO_NV_STRU;

/*NV ID =0xd115 end*/

/*NV ID =0xd116 start,mipi0_chn*/
typedef struct{
	u32 mipi_chn;
}MIPI0_CHN_STRU;
/*NV ID =0xd116 end,mipi0_chn*/

/*NV ID =0xd117 start,mipi1_chn*/
typedef struct{
	u32 mipi_chn;
}MIPI1_CHN_STRU;
/*NV ID =0xd117 end,mipi1_chn*/

/*NV ID =0xd12e start, rf power control, pastar config*/
typedef struct{
	u32 rfpower_m0;/*[0, 1,2]*//*modem0,物理通道0的供电单元,value为0表示不打开电源，为1表示为pastar供电，为2表示LDO供电*/
	u32 rfpower_m1;/*[0, 1,2]*//*modem1,物理通道1的供电单元,value为0表示不打开电源，为1表示为pastar供电，为2表示LDO供电*/
}NV_RFPOWER_UNIT_STRU;
/*NV ID =0xd12e start, pastar config*/

/* NV ID =0xd13A start, pa power control, pastar config */
typedef struct{
	u32 papower_m0;/*[0,1,2]*//*modem0,物理通道0的供电单元,value为0表示不打开电源，为1表示为pastar供电，为2表示电池供电*/
	u32 papower_m1;/*[0,1,2]*//*modem1,物理通道1的供电单元,value为0表示不打开电源，为1表示为pastar供电，为2表示电池供电*/
}NV_PAPOWER_UNIT_STRU;
/* NV ID =0xd13A start, pastar config */

typedef struct
{
    u32   nvSysTimeValue;   /* 获得单板系统运行时间 */
}SYS_TIME;

typedef struct
{
    u32  ulIsEnable;				/*温度保护使能*/
    u32    lCloseAdcThreshold;
    u32  ulTempOverCount;
}CHG_BATTERY_HIGH_TEMP_PROT_NV;

typedef struct
{
    u32  ulIsEnable;			/*温度保护使能*/
    u32    lCloseAdcThreshold;
    u32  ulTempLowCount;
}CHG_BATTERY_LOW_TEMP_PROTE_NV;

/* 工厂模式nv项,0 : 非工厂模式 1: 工厂模式**/
typedef struct
{
    u32 ulFactoryMode;
}FACTORY_MODE_TYPE;

/* 开机启动尝试次数 */
typedef struct
{
    u32 ulTryTimes;
}BOOT_TRY_TIMES_STRU;

/* 开机按键按下时间 */
typedef struct
{
    u32 ulPowKeyTime;
}POWER_KEY_TIME_STRU;

typedef struct
{
    u16 temperature;
    u16 voltage;
}CHG_TEMP_ADC_TYPE;

typedef struct
{
    CHG_TEMP_ADC_TYPE g_adc_batt_therm_map[31];
}NV_BATTERY_TEMP_ADC;


/*硬测版本nv项,1 : 硬测版本 0: 非硬测版本*/
typedef struct
{
    u32 ulHwVer;
}E5_HW_TEST_TYPE;

/*是否支持APT功能nv项,1 : 支持 0: 不支持*/
typedef struct
{
    u32 ulIsSupportApt;
}NV_SUPPORT_APT_TYPE;

/*PMU异常保护nv项*/
typedef struct
{
    u8 TemppmuLimit;  /*PMU温度阈值:0:105℃;1:115℃;2:125℃;3:135℃*/
	u8 ulCurIsOff;  /*过热时过流源是否下电：0：下电；1：不下电*/
	u8 ulOcpIsRst;  /*过热时是否进行软复位: 0:不软复位 1:软复位*/
	u8 PmuproIsOn;  /*PMU异常保护处理是否开启：0：不开启；1：开启*/
}PMU_PRO_NV;

typedef struct
{
    u32 u32SciGcfStubFlag;   /* 1: GCF测试使能；0：GCF测试不使能 */
}SCI_NV_GCF_STUB_FLAG;

/*快速开关机功能是否使能NV项*/
typedef struct
{
    u32 ulEnable;   /*快速开关机是否使能：0：不使能；1：使能*/
}NV_SHORT_ONOFF_ENABLE_TYPE;

/*快速开关机功能配置信息NV项*/
typedef struct NV_SHORT_ONOFF_TYPE_S
{
    u32 ulPowerOffMaxTimes;   /*支持的最大假关机次数*/
	u32 ulMaxTime;            /*假关机历史累加时间大于等于此时间自动真关机，单位小时*/
	u32 ulVoltLevel1;         /*过放保护第一档电压门限*/
	u32 ulVoltLevel2;         /*过放保护第二档电压门限*/
	u32 ulRTCLevel1;          /*小于第一档电压对应的RTC唤醒时间*/
	u32 ulRTCLevel2;          /*第一档和第二档之间电压对应的RTC唤醒时间*/
	u32 ulRTCLevel3;          /*大于等于第二档电压对应的RTC唤醒时间*/
}NV_SHORT_ONOFF_TYPE;

/*省电模式配置，标识各外设是否使能NV项*/
typedef struct
{
        u32 ulLEDEnable;   /*呼吸灯LED 是否使能：0：不使能；1：使能*/
        u32 ulReserved1;   /*此项预留  是否使能：0：不使能；1：使能*/
        u32 ulReserved2;   /*此项预留  是否使能：0：不使能；1：使能*/
}NV_POWER_SAVE_TYPE;

/*库仑计电压，电流校准参数*/
typedef struct
{
    u32 v_offset_a;         /* 电压校准线性参数 */
    s32 v_offset_b;         /* 电压校准线性偏移*/
    u32 c_offset_a;         /* 电流校准线性参数 */
    s32 c_offset_b;         /* 电流校准线性偏移 */
}COUL_CALI_NV_TYPE;

/*温度保护HKADC各个通道的配置 NV_ID_DRV_TEMP_HKADC_CONFIG            = 0xd120 */

typedef struct
{
    u32 out_config;         /* bit0-bit1 0:不输出 1:单次输出 2:循环输出 */
                                /* bit2 1:唤醒输出 0:非唤醒输出 */
                                /* bit3: 0:只输出电压 1:电压温度均输出 */
                                /* bit8: A核输出 */
                                /* bit9: C核输出 */
    u32 have_config;
    u16 out_period;         /* 循环输出时的循环周期，单位:秒 */
    u16 convert_list_len;   /* 温度转换表长度 */
    u16 convert_list_id;    /* 温度转换表NV标志，实际长度参见usTempDataLen */
    u16 reserved;
}TEMP_HKADC_CHAN_CONFIG;

typedef struct
{
    TEMP_HKADC_CHAN_CONFIG chan_config[16];

}TEMP_HKADC_CHAN_CONFIG_ARRAY;

/*温度保护HKADC各个通道的配置 NV_ID_DRV_TEMP_TSENS_CONFIG            = 0xd121 */
typedef struct
{
    u16 enable;         /* bit0:高温保护使能 bit1:低温保护使能 1 使能 0 关闭*/
    u16 high_thres;     /* 芯片高温保护的电压阀值  */
    u16 high_count;     /* 芯片高温保护次数上限，系统关机 */
    u16 reserved;
    /*u32 low_thres;*/  /* 芯片低温保护的电压阀值 */
    /*u32 low_count;*/  /* 芯片低温保护次数上限，系统关机 */

}TEMP_TSENS_REGION_CONFIG;

typedef struct
{
    TEMP_TSENS_REGION_CONFIG region_config[3];

}TEMP_TSENS_REGION_CONFIG_ARRAY;



/*温度保护电池高低温保护  NV_ID_DRV_TEMP_BATTERY_CONFIG          = 0xd122 */
typedef struct
{
    u16 enable;        /* bit0:高温保护使能 bit1:低温保护使能 1 使能 0 关闭*/
    u16 hkadc_id;      /* 系统放电温度保护的hkadc通道ID */
    u16 high_thres;    /* 系统放电高温保护的温度阀值 */
    u16 high_count;    /* 系统放电高温保护次数上限 */
    s16 low_thres;     /* 系统放电低温保护的温度阀值 */
    u16 low_count;     /* 系统放电低温保护次数 */

    u32 reserved[2];   /*保留*/
} DRV_SYS_TEMP_STRU;

/*温度保护物理逻辑转换表  NV_ID_DRV_TEMP_CHAN_MAP          = 0xd126 */
typedef struct
{
    u8 chan_map[32];

}TEMP_HKADC_PHY_LOGIC_ARRAY;

/* WIN7 Feature for usb id:50075 */
typedef struct
{
    u8 wwan_flag;
    u8 reserved1;
    u8 reserved2;
    u8 reserved3;
    u8 reserved4;
    u8 reserved5;
    u8 reserved6;
    u8 reserved7;
} NV_WIN7_FEATURE;

typedef struct
{
    u32 sci_dsda_select;        /* 0: sim0, 1:sim1*/
} DRV_SCI_DSDA_SELECT;
/* SIM gcf test flage*/
typedef struct
{
    u32 sci_gcf_sub_flag;        /* 0: open, 1:close */
} DRV_SCI_GCF_STUB_FLAG;

/*E5串口复用*/
typedef struct
{
	u32 wait_usr_sele_uart : 1;//[bit 0-0]1: await user's command for a moment; 0: do not wait
	u32 a_core_uart_num    : 2;//[bit 1-2]the number of uart used by a core
	u32 c_core_uart_num    : 2;//[bit 3-4]the number of uart used by c core
	u32 m_core_uart_num    : 2;//[bit 5-6]the number of uart used by m core
	u32 a_shell            : 1;//[bit 7-7]0:ashell is not used; 1:ashell is used
	u32 c_shell            : 1;//[bit 8-8]0:cshell is not used; 1:cshell is used
	u32 uart_at            : 1;//[bit 9-9]uart at control
	u32 extendedbits       : 22;//[b00]1:open cshell_auart; 0:close
}DRV_UART_SHELL_FLAG;

/* product support module nv define */
typedef struct
{
	u32 sdcard 		: 1;//1: support; 0: not support
	u32 charge 		: 1;
	u32 wifi    	: 1;
	u32 oled    	: 1;
	u32 hifi        : 1;
	u32 onoff       : 1;
	u32 hsic        : 1;
	u32 localflash  : 1;
	u32 reserved    : 24;
} DRV_MODULE_SUPPORT_STRU;

typedef struct
{
	u8 normalwfi_flag;
	u8 deepsleep_flag;
	u8 buck3off_flag;
	u8 peridown_flag;
	u32 deepsleep_Tth;
	u32 TLbbp_Tth;
}DRV_NV_PM_TYPE;

/* NV_ID_DRV_TSENS_TABLE = 0xd129*/
typedef struct
{
    s16 temp[256];

}DRV_TSENS_TEMP_TABLE;

typedef struct
{
    u8 ucABBSwitch; /*对应模式使用的ABB物理通道，0 通道0, 1: 通道1, 2: 同时使用两个通道*/
    u8 ucRFSwitch;  /*对应模式使用的RF物理通道，0 通道0, 1: 通道1, 2: 同时使用两个通道*/
    u8 ucTCXOSwitch; /*对应模式使用的TCXO ID 0 TCXO0, 1: TCXO1*/
    u8 reserve;
}NV_TLMODE_BASIC_PARA_STRU;

typedef struct
{
    NV_TLMODE_BASIC_PARA_STRU stModeBasicParam[2];/*下标为0:LTE, 1:TDS*/
}NV_TLMODE_CHAN_PARA_STRU;

typedef struct
{
    u32 clkdis1; /*初始关闭时钟寄存器，对应crg clk dis1*/
    u32 clkdis2; /*初始关闭时钟寄存器，对应crg clk dis2*/
    u32 clkdis3; /*初始关闭时钟寄存器，对应crg clk dis3*/
    u32 clkdis4;  /*初始关闭时钟寄存器，对应crg clk dis4*/
	u32 clkdis5;  /*初始关闭时钟寄存器，对应crg clk dis5*/
    u32 mtcmosdis; /*初始关闭MTCMOS寄存器，对应crg mtcmosdis*/
}DRV_NV_PM_CLKINIT_STRU;

typedef struct
{
    u32 ucBBPCh0TcxoSel:1; 	    /* bit 0,CH0 19.2M使用的TCXO源，0 TCXO0, 1: TCXO1; 对应寄存器 0x90000148 */
    u32 ucBBPCh1TcxoSel:1;  	/* bit 1,CH1 19.2M使用的TCXO源，0 TCXO0, 1: TCXO1; 对应寄存器 0x90000148 */
    u32 ucABBCh0TcxoSel:1; 		/* bit 2,ABB Ch0 使用的TCXO源，0 TCXO0, 1: TCXO1; 对应ABB寄存器 0x94 */
    u32 ucABBCh1TcxoSel:1; 		/* bit 3,ABB Ch1 使用的TCXO源，0 TCXO0, 1: TCXO1; 对应寄存器 0x94 */
	u32 ucBbpPllTcxoSel:1; 		/* bit 4,BBP PLL 使用的TCXO源，0 TCXO0, 1: TCXO1;*/
	u32 ucG1bp104mTcxoSel:1; 	/* bit 5,G1BBP 104M 使用的ABB通道，0：通道0, 1: 通道1;*/
	u32 ucG2bp104mTcxoSel:1; 	/* bit 6,G1BBP 104M 使用的ABB通道，0：通道0, 1: 通道1;*/
	u32 reserve:25;             /* bit 7-31*/
}DRV_TCXO_SEL_PARA_STRU;

typedef struct
{
    u32 u32UsbDbg;   /* usb模块调试信息级别 */
}DRV_USB_DBG_STRU;

typedef struct
{
	u32 wdt_enable;
	u32 wdt_timeout;
	u32 wdt_keepalive_ctime;
	u32 wdt_suspend_timerout;
	u32 wdt_reserve;
}DRV_WDT_INIT_PARA_STRU;


/*温度保护物理逻辑转换表  NV_ID_DRV_TSENSOR_TRIM          = 0xd12f */
typedef struct
{
    u16 tsensor_trim[16];

}DRV_TSENSOR_TRIM_STRU;

/* axi monitor监控ID配置 */
typedef struct
{
    u32 reset_flag;     /* 复位标志，匹配到监控条件是否复位 */
    u32 opt_type;       /* 监控类型，01:读，10:写，11:读写，其余值:不监控 */
    u32 port;           /* 监控端口 */
    u32 master_id;      /* 监控masterid */
    u32 start_addr;     /* 监控起始地址 */
    u32 end_addr;       /* 监控结束地址 */
} AMON_CONFIG_T;

/* axi monitor配置NV项 NV_ID_DRV_AMON = 0xd130 */
typedef struct
{
    u32         en_flag;            /* 使能标志，00:去使能，01:SOC，10:CPUFAST，11:SOC,CPUFAST */
    AMON_CONFIG_T   soc_config[8];      /* SOC配置，8个监控ID */
    AMON_CONFIG_T   cpufast_config[8];  /* CPUFAST配置，8个监控ID */
} DRV_AMON_CONFIG_STRU;

/*****************************************************************************
 结构名    : NV_RF_GPIO_CFG_STRU
 结构说明  : RF_GPIO中有若干可以复用为DRV_GPIO
             在一些射频功能场景需要改变这些RF_GPIO引脚的复用功能及输出状态
             通过该NV配置 ID=10047
*****************************************************************************/
typedef struct
{
    u16 is_used;    /*Range:[0,1]*/
	u16 gpio_level; /*Range:[0,1]*/
}RF_MODEM_CONTROL;

typedef struct
{
    u32 rf_gpio_num;
    RF_MODEM_CONTROL modem_inside;	/*内置modem*/
    RF_MODEM_CONTROL modem_outside; /*外置modem*/
}RF_GPIO_CFG;

typedef struct
{
    RF_GPIO_CFG rf_switch_cfg[16];
}NV_RF_SWITCH_CFG_STRU;

/* SOCP按需开关特性配置NV项 = 0xd132 */
typedef struct
{
    u32         en_flag;            /* 使能标志，00:去使能，01:使能 */
} DRV_SOCP_ON_DEMAND_STRU;

/* drx delay flag */
typedef struct
{
    u8         lpm3_flag;         /* 0x11 代表lpm3 */
    u8         lpm3_0;            /* 1打开delay,其他关闭delay */
    u8         lpm3_1;
    u8         lpm3_2;
    u8         lpm3_3;
    u8         lpm3_4;
    u8         drv_flag;          /* 0x22 代表drv */
    u8         drv_0;             /* 1打开delay,其他关闭delay */
    u8         drv_1;
    u8         drv_2;
    u8         drv_3;
    u8         drv_4;
    u8         msp_flag;          /* 0x33 代表msp */
    u8         msp_0;             /* 1打开delay,其他关闭delay */
    u8         msp_1;
    u8         msp_2;
    u8         msp_3;
    u8         msp_4;
}DRV_DRX_DELAY_STRU;

/* C核单独复位按需开关特性配置NV项 = 0xd134 */
typedef struct
{
    u32 is_feature_on:1;         /* bit0, 整个C核单独复位功能是否打开 */
    u32 is_connected_ril:1;      /* bit1, 是否和RIL对接 */
	u32 reserve:30;              /* bit 2-31*/
} DRV_CCORE_RESET_STRU;

/* NV ID = 0xd135 */
/* 用于表示每路LDO和BUCK是否打开 */
typedef struct
{
	u32 ldo1_switch     :1;         /* 第0bit，值为1则为打开，值为0则为关闭 */
	u32 ldo2_switch     :1;         /* 第1bit，值为1则为打开，值为0则为关闭 */
	u32 buckpa_switch   :1;         /* 第2bit，值为1则为打开，值为0则为关闭 */
	u32 buck1_switch    :1;         /* 第3bit，值为1则为打开，值为0则为关闭 */
	u32 buck2_switch    :1;         /* 第4bit，值为1则为打开，值为0则为关闭 */
	u32 reserved        :27;        /* 第5~31bit，预留。默认值为0 */
}NV_PASTAR_SWITCH_STRU_BITS;

typedef struct
{
    union
    {
        u32 u32;
        NV_PASTAR_SWITCH_STRU_BITS bits;
    }cont;
}NV_PASTAR_SWITCH_STRU;
/* end NV ID = 0xd135 */


/* log2.0 2014-03-19 Begin:*/
typedef struct
{
    u32 ulSocpDelayWriteFlg;/* SOCP延迟写入功能启动标记 */
    u32 ulGuSocpLevel;      /* GU SOCP目的BUFFER发送水线 */
    u32 ulLSocpLevel;       /* L SOCP目的BUFFER发送水线 */
    u32 ulTimeOutValue;     /* SOCP目的BUFFER的超时时间 */
}DRV_NV_SOCP_LOG_CFG_STRU;
/* log2.0 2014-03-19 End*/
typedef struct
{
    u32  enUartEnableCfg;
	u32  AwaitReplyTimer;
	u32  AwakeTmer;
	u32  DoSleepTimer;
}DRV_DUAL_MODEM_STR;

/* GPIO控制LDO开关 NV项 = 0xd137 */
typedef struct
{
    u32 gpio;      /* LDO对应GPIO编号 */
    u32 used;      /* GPIO是否使用 */
} DRV_DRV_LDO_GPIO_CFG;

typedef struct
{
    DRV_DRV_LDO_GPIO_CFG ldo_gpio[2];
}DRV_DRV_LDO_GPIO_STRU;


/* GPIO控制LDO开关 NV项 = 0xd138 */
typedef struct
{
    u32 modem_id;      /* modem id默认从0开始依次增大 */
    u32 gpio;          /* ANTEN对应GPIO编号 */
    u32 used;          /* GPIO是否使用 */
} DRV_DRV_ANTEN_GPIO_CFG;

typedef struct
{
    DRV_DRV_ANTEN_GPIO_CFG anten_gpio[LDO_GPIO_MAX];
}DRV_DRV_ANTEN_GPIO_STRU;

/* RSE POWER GPIO控制 NV项 = 0xd139 */
typedef struct
{
    u32 ulRsePowerOnIds;
    u32 ulFemCtrlInfo;
    u32 ulRfGpioBitMask;
    u32 ulRfGpioOutValue;
    u16 usFemMipiCmdAddr;
    u16 usFemMipiCmdData;
}RF_NV_RSE_CFG_STRU;


typedef struct
{
	u32 DialupEnableCFG;
	u32 DialupACShellCFG; 
}DRV_DIALUP_HSUART_STRU;


/*
*nvid = 0xd13d
*for mmc support or not
*/
typedef struct
{
	u32 support;
}DRV_MMC_SUPPORT_STRU;

/* PA/RF供电实体是否使用开关，必须与EM_MODEM_CONSUMER_ID枚举中的顺序保持一致 */
/*通信模块供电的 CONSUMER的id枚举
typedef enum EM_MODEM_CONSUMER_ID_E
{
    MODEM_PA0,
    MODEM_RFIC0_ANALOG0,
    MODEM_RFIC0_ANALOG1,
    MODEM_FEM0,
    MODEM_PA_VBIAS0,
    MODEM_PA1,
    MODEM_RFIC1_ANALOG0,
    MODEM_RFIC1_ANALOG1,
    MODEM_FEM1,
    MODEM_PA_VBIAS1,
    MODEM_CONSUMER_ID_BUTT
}EM_MODEM_CONSUMER_ID;
*/
typedef struct
{
	u32 pa0 				: 1;	//1: 硬件有实际供电连接; 0: 硬件无实际供电连接
	u32 rfic0_analog0 		: 1;
	u32 rfic0_analog1    	: 1;
	u32 fem0    			: 1;
	u32 pa_vbias0       	: 1;
	u32 pa1 				: 1;
	u32 rfic1_analog0 		: 1;
	u32 rfic1_analog1    	: 1;
	u32 fem1    			: 1;
	u32 pa_vbias1       	: 1;
	u32 reserved    : 22;
}PARF_SWITCH_BITS;

typedef struct
{
    union
    {
        u32 u32;
        PARF_SWITCH_BITS bits;
    }cont;
} NV_DRV_PARF_SWITCH_STRU;

/* NVID=0xd140 begin */
typedef struct
{
    u32 mdm_dump    :1;     /* modem_dump.bin */
    u32 mdm_sram    :1;     /* modem_sram.bin */
    u32 mdm_share   :1;     /* modem_share.bin */
    u32 mdm_ddr     :1;     /* modem_ddr.bin */
    u32 lphy_tcm    :1;     /* lphy_tcm.bin */
    u32 lpm3_tcm    :1;     /* lpm3_tcm.bin */
    u32 ap_etb      :1;     /* ap_etb.bin */
    u32 mdm_etb     :1;     /* modem_etb.bin */
    u32 reserved2   :24;
}DUMP_FILE_BITS;

typedef struct
{   
    union
    {
        u32         file_value;
        DUMP_FILE_BITS  file_bits;
    } file_list;
    u32             file_cnt; /* 记录异常文件份数 */
}DUMP_FILE_CFG_STRU;

/* pm om控制 NV项 = 0xd145 */
typedef struct
{
	u8  log_threshold[4];    /* acore/ccore/mcore触发写文件的阀值, 数值表示百分比 */
    u8  mod_sw[8];           /* 各个模块log功能控制开关 */
}DRV_PM_OM_CFG_STRU;
/* NVID=0xd13d end */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

