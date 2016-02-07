

/*****************************************************************************
  1 其他头文件包含

  notice:  不要包含任何其他头文件!!!
*****************************************************************************/
#ifndef __IPCMAILBOXINTERFACE_H__
#define __IPCMAILBOXINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*lint -e413*/
#define GET_OFFSET(a,b)                 (unsigned long)((void *)(&(((a*)0)->b)))
/*lint +e413*/

/* 跨核通信邮箱长度(不含邮箱头信息，含保护字), bytes */
#define IPC_MAILBOX_SIZE_HIFI2CARM      (0x00000f98)
#define IPC_MAILBOX_SIZE_CARM2HIFI      (0x000007d8)
#define IPC_MAILBOX_SIZE_HIFI2ZSP       (0x000003c0)
#define IPC_MAILBOX_SIZE_ZSP2HIFI       (0x000003c0)
#define IPC_MAILBOX_SIZE_ZSP2CARM       (0x00004148)
#define IPC_MAILBOX_SIZE_CARM2ZSP       (0x00002510)

/*Build_Opt新增*/
#define IPC_MB_HEAD_OFFSET_CARM2HIFI    GET_OFFSET(DRV_CARM_HIFI_MB_STRU, stCarm2HifiHead)
#define IPC_MB_HEAD_OFFSET_HIFI2CARM    GET_OFFSET(DRV_CARM_HIFI_MB_STRU, stHifi2CarmHead)
#define IPC_MB_OFFSET_CARM2HIFI         GET_OFFSET(DRV_CARM_HIFI_MB_STRU, aCarm2Hifi[0])
#define IPC_MB_OFFSET_HIFI2CARM         GET_OFFSET(DRV_CARM_HIFI_MB_STRU, aHifi2Carm[0])
#define PC_VOICE_OM_MED_ADDR_OFFSET     GET_OFFSET(DRV_CARM_HIFI_MB_STRU, stTxRing)
#define PC_VOICE_MED_OM_ADDR_OFFSET     GET_OFFSET(DRV_CARM_HIFI_MB_STRU, stRxRing)

/*DSP状态标志，位于SRAM1非下电区，由OAM下发首地址，标志HIFI的运行状态*/
#define HIFI_STATUS_OFFSET              GET_OFFSET(DSP_DRX_STATUS_STRU, uwHifiStatus)

/* HIFI发给C-CPU的IPC中断复用标志地址，发送者置该标志指示该中断用途，默认为0(表
            示跨核邮箱中断)，后再发送IPC中断，接收者每次响应中断后负责恢复默认值 */
#define IPC_MB_STATUS_OFFSET_HIFI2CARM  GET_OFFSET(DSP_DRX_STATUS_STRU, uwIpcMailboxStatusHifi2Carm)

/* C-CPU发给HIFI的IPC中断复用标志地址，发送者置该标志指示该中断用途，默认为0(表
            示跨核邮箱中断)，后再发送IPC中断，接收者每次响应中断后负责恢复默认值 */
#define IPC_MB_STATUS_OFFSET_CARM2HIFI  GET_OFFSET(DSP_DRX_STATUS_STRU, uwIpcMailboxStatusCarm2Hifi)

/*DSP状态标志，位于SRAM1非下电区，由OAM下发首地址，标志ZSP的运行状态*/
#define ZSP_STATUS_OFFSET               GET_OFFSET(DSP_DRX_STATUS_STRU, uwZspStatus)

/*降SAR增加*/
#define PHY_ARM_SAR_SHARED_OFFSET       GET_OFFSET(CPHY_TEMP_INFO_STRU, usSarAntState)
#define PHY_ARM_SAR_REUCT_OFFSET        GET_OFFSET(CPHY_TEMP_INFO_STRU, usSarReduction)

/* GUL从模低功耗上电保护标志位 */
#define PHY_ARM_POWER_INIT_OFFSET           GET_OFFSET(CPHY_TEMP_INFO_STRU, ulSlaveLPPowerInit)
#define PHY_ARM_POWER_INIT_SLICE_OFFSET     GET_OFFSET(CPHY_TEMP_INFO_STRU, ulSlaveLPPowerInitSlice)
#define PHY_ARM_POWER_INIT_FLG              (0x12345678)

/* GUL从模低功耗加载保护标志位 */
#define PHY_ARM_CODE_LOADING_OFFSET         GET_OFFSET(CPHY_TEMP_INFO_STRU, ulSlaveLPCodeLoading)
#define PHY_ARM_CODE_LOADING_SLICE_OFFSET   GET_OFFSET(CPHY_TEMP_INFO_STRU, ulSlaveLPCodeLoadingSlice)
#define PHY_ARM_CODE_LOADING_FLG	        (0x55AA55AA)

/* 跨核通信邮箱头首地址 */
#define IPC_MAILBOX_HEAD_ADDR_CARM2HIFI     (HIFI_AXI_BASE_ADDR + IPC_MB_HEAD_OFFSET_CARM2HIFI)
#define IPC_MAILBOX_HEAD_ADDR_HIFI2CARM     (HIFI_AXI_BASE_ADDR + IPC_MB_HEAD_OFFSET_HIFI2CARM)
#define IPC_MAILBOX_HEAD_ADDR_ZSP2CARM      (AHB_BASE_ADDR + 0x3c00)
#define IPC_MAILBOX_HEAD_ADDR_CARM2ZSP      (AHB_BASE_ADDR + 0x01d8)

/* 跨核通信邮箱首地址 */
#define IPC_MAILBOX_ADDR_HIFI2ZSP_OFFSET    (0x1F620)
#define IPC_MAILBOX_ADDR_ZSP2HIFI_OFFSET    (0x1F820)
#define IPC_MAILBOX_ADDR_HIFI2CARM          (HIFI_AXI_BASE_ADDR + IPC_MB_OFFSET_HIFI2CARM)
#define IPC_MAILBOX_ADDR_CARM2HIFI          (HIFI_AXI_BASE_ADDR + IPC_MB_OFFSET_CARM2HIFI)
#define IPC_MAILBOX_ADDR_ZSP2CARM           (AHB_BASE_ADDR + 0x3c28)
#define IPC_MAILBOX_ADDR_CARM2ZSP           (AHB_BASE_ADDR + 0x0200)

#define IPC_MAILBOX_STATUS_DSP2ARM          (((unsigned long)AHB_BASE_ADDR)+0x01d0)/* 0x203001d0)
 */
#define IPC_MAILBOX_STATUS_ARM2DSP          (((unsigned long)AHB_BASE_ADDR)+0x01d4)/*(0x203001d4)
 */

/*自0x2ffe2060U开始共16byte作为Hifi非下电标志使用，位于SRAM1非下电区 */
#define HIFI_STATUS_FLAG_ADDR               (HIFI_STATUS_BASE_ADDR + HIFI_STATUS_OFFSET)/*(0x2ffe2060U)
 */

/* HIFI发给C-CPU的IPC中断复用标志地址，发送者置该标志指示该中断用途，默认为0(表
            示跨核邮箱中断)，后再发送IPC中断，接收者每次响应中断后负责恢复默认值 */
#define IPC_MAILBOX_STATUS_HIFI2CARM        (HIFI_STATUS_BASE_ADDR + IPC_MB_STATUS_OFFSET_HIFI2CARM)

/* C-CPU发给HIFI的IPC中断复用标志地址，发送者置该标志指示该中断用途，默认为0(表
            示跨核邮箱中断)，后再发送IPC中断，接收者每次响应中断后负责恢复默认值 */
#define IPC_MAILBOX_STATUS_CARM2HIFI        (HIFI_STATUS_BASE_ADDR + IPC_MB_STATUS_OFFSET_CARM2HIFI)

/*自0x2ffe2070U开始共16byte作为ZSP非下电标志使用，位于SRAM1非下电区 */
#define ZSP_STATUS_FLAG_ADDR                (HIFI_STATUS_BASE_ADDR + ZSP_STATUS_OFFSET)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实 体 名  : IPC_TARGET_CPU_ENUM
 功能描述  : 定义IPC中断输出目标CPU, 由SOC决定
*****************************************************************************/
enum IPC_TARGET_CPU_ENUM
{
    IPC_TARGET_CPU_APPARM = 0,          /* 中断APP ARM */
    IPC_TARGET_CPU_COMARM,              /* 中断COM ARM */
    IPC_TARGET_CPU_LTEDSP,              /* 中断LTE DSP, balongv3r2保留 */
    IPC_TARGET_CPU_MEDDSP,              /* 中断MED DSP */
    IPC_TARGET_CPU_GUDSP,               /* 中断GU DSP  */
    IPC_TARGET_CPU_BUTT
};
typedef unsigned short IPC_TARGET_CPU_ENUM_UINT16;

/*****************************************************************************
 实 体 名  : IPC_MEDDSP_INT_SRC_ENUM
 功能描述  : 定义HiFi IPC跨核消息中断源bit位置
*****************************************************************************/
enum IPC_MEDDSP_INT_SRC_ENUM
{
    IPC_MEDDSP_INT_SRC_CARM_MSG = 4,    /* bit4, CARM跨核消息通知 */
    IPC_MEDDSP_INT_SRC_GUDSP_MSG,       /* bit5, GUDSP跨核消息通知 */
    IPC_MEDDSP_INT_SRC_BUTT
};
typedef unsigned short IPC_MEDDSP_INT_SRC_ENUM_UINT16;

/*****************************************************************************
 实 体 名  : IPC_GUDSP_INT_SRC_ENUM
 功能描述  : 定义ZSP IPC跨核消息中断源bit位置
*****************************************************************************/
enum IPC_GUDSP_INT_SRC_ENUM
{
    IPC_GUDSP_INT_SRC_CARM_MSG = 4,     /* bit4, CARM跨核消息通知 */
    IPC_GUDSP_INT_SRC_MEDDSP_MSG,       /* bit5, MED-DSP跨核消息通知 */
    IPC_GUDSP_INT_SRC_BUTT
};
typedef unsigned short IPC_GUDSP_INT_SRC_ENUM_UINT16;

/*****************************************************************************
 实 体 名  : IPC_CARM_INT_SRC_ENUM
 功能描述  : 定义CCPU IPC跨核消息中断源bit位置
*****************************************************************************/
enum IPC_CARM_INT_SRC_ENUM
{
    IPC_CARM_INT_SRC_GUDSP_MSG = 4,     /* bit4, GUDSP跨核消息通知 */
    IPC_CARM_INT_SRC_MEDDSP_MSG,        /* bit5, MED-DSP跨核消息通知 */
    IPC_CARM_INT_SRC_BUTT
};
typedef unsigned short IPC_CARM_INT_SRC_ENUM_UINT16;

/*****************************************************************************
 枚举名    : HIFI_STATUS_FLAG_ENUM
 枚举说明  : HIFI状态标志类型
*****************************************************************************/
enum HIFI_STATUS_FLAG_ENUM
{
    HIFI_STATUS_SLEEP                   = 0,                /*HIFI状态为SLEEP态*/
    HIFI_STATUS_SLEEP2WAKE              = 1,                /*HIFI状态为SLEEP->WAKE中间态*/
    HIFI_STATUS_WAKE                    = 2,                /*HIFI状态为WAKE态*/
    HIFI_STATUS_WAKE2SLEEP              = 3,                /*HIFI状态为WAKE->SLEEP中间态*/
    HIFI_STATUS_BUTT
};
typedef unsigned long HIFI_STATUS_FLAG_ENUM_UINT32;


/*****************************************************************************
 枚举名    : ZSP_STATUS_FLAG_ENUM
 枚举说明  : HIFI对ZSP的访问ZSP是否可以睡眠标志类型
*****************************************************************************/
enum ZSP_STATUS_FLAG_ENUM
{
    ZSP_STATUS_WAKE                     = 0,                /* ZSP处于唤醒状态,HIFI可以访问 */
    ZSP_STATUS_FORBID_SLEEP             = 1,                /* HIFI正在访问,ZSP不可以睡眠 */
    ZSP_STATUS_SLEEP                    = 2,                /* ZSP处于睡眠状态,HIFI不可访问 */
    ZSP_STATUS_BUTT
};
typedef unsigned long ZSP_STATUS_FLAG_ENUM_UINT32;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

#if 0
/************************************************************************
 结构名    : IPC_MAILBOX_HEAD_STRU
 结构说明  : 核间消息缓存头
 ************************************************************************/
typedef struct
{
    unsigned long      ulProtectWord1;     /*保护字 0x55AA55AA*/
    unsigned long      ulProtectWord2;     /*保护字 0x5A5A5A5A*/
    unsigned long      ulFront;            /*队列待写单元距离队列(不含保护字)头的长度，单位32bit */
    unsigned long      ulRear;             /*队列待读单元距离队列(不含保护字)头的长度，单位32bit */
    unsigned long      ulFrontslice;       /*更新邮箱环形队列的头指针系统时间*/
    unsigned long      ulRearslice;        /*更新邮箱环形队列的尾指针系统时间*/
    unsigned short     ausReserve[4];      /*保留*/
    unsigned long      ulProtectWord3;     /*保护字 0x55AA55AA*/
    unsigned long      ulProtectWord4;     /*保护字 0x5A5A5A5A*/
} IPC_MAILBOX_HEAD_STRU;
#endif

/************************************************************************
 结构名    : IPC_MSG_HEADER_STRU
 结构说明  : 核间消息头
 ************************************************************************/
typedef struct
{
    unsigned long      ulPartition;        /*消息分隔字 0xA5A5A5A5*/
    unsigned long      ulWriteSlice;       /*消息写入系统时间信息 */
    unsigned long      ulReadSlice;        /*消息读出系统时间信息 */
    unsigned long      ulSeqNum;           /*消息序列号 */
    unsigned long      ulPriority;         /*0-紧急, 1-普通 */
    unsigned short     ausReserve[4];      /*保留*/
    unsigned long      ulMsgLength;        /*自ulMsgLength字段(不含本身)往后的消息长度*/
} IPC_MSG_HEADER_STRU;

#if 0
/************************************************************************
 结构名    : IPC_MSG_BLK_STRU
 结构说明  : 核间消息块
 ************************************************************************/
typedef struct
{
    IPC_MSG_HEADER_STRU stMsgHead;      /* 消息头 */
    unsigned long      ulSenderCpuId;      /* 消息发送CPU */
    unsigned long      ulSenderPid;        /* 消息发送PID */
    unsigned long      ulReceiverCpuId;    /* 消息接收CPU */
    unsigned long      ulReceiverPid;      /* 消息接收PID */
    unsigned long      ulLength;           /* 消息正文长度,bytes */
    unsigned char      aucValue[4];        /* 消息正文 */
} IPC_MSG_BLK_STRU;
#endif

/************************************************************************
 结构名    : IPC_MAILBOX_QUEUE_STRU
 结构说明  : 核间消息邮箱队列缓存
 ************************************************************************/
/* 邮箱结构示意:
typedef struct
{
    unsigned long      ulProtectWord1;     //保护字 0x55aa55aa
    unsigned long      ulProtectWord2;     //保护字 0a5a5a5a5a
    unsigned long      aulMsgQueue[队列长度-4];//环形队列存放若干个IPC_MSG_BLK_STRU
    unsigned long      ulProtectWord3;     //保护字 0x55aa55aa
    unsigned long      ulProtectWord4;     //保护字 0x5a5a5a5a
} IPC_MAILBOX_QUEUE_STRU;
*/


/*****************************************************************************
 结构名    : PS_PHY_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 物理层和PS的共享地址，OAM需要获取并传给TTF或者初始化后由PHY读取
*****************************************************************************/
typedef struct
{
    /*物理层初始化,PS只读*/
    unsigned long uwProtectWord1;                      /*0x5a5a5a5a*/
    unsigned long uwPhyGttfShareAddr;                  /* GDSP与GTTF共享内存接口地址,物理层初始化,使用者必须*2+基址*/

    /*PS负责初始化,PHY只读*/
    unsigned long uwProtectWord2;                      /*0x5a5a5a5a，PS填写*/
    unsigned long uwHsupaUpaccMacePduBaseAddr;         /*HSUPA_UPACC_MACE_PDU_BASE_ADDR*/
    unsigned long uwEcsArmDspBufferSize;               /*ECS_ARM_DSP_BUFFER_SIZE*/
    unsigned long uwSocTimeSliceAddr;                  /*时标读取地址*/

    /*AXI addr*/
    unsigned long uwStatusFlagAddr;                    /*HIFI/DSP STATUS_FLAG_ADDR*/
    unsigned long uwTempratureAddr;                    /*温保地址*/

    unsigned long uwHsdpaWttfBufAddr;                  /* HSDPA WTTF共享buf地址 */
    /*保留，扩展用。以后可以把平台相关的地址放到这里传递进来*/
    unsigned long uwReserved[15];
}PS_PHY_DYN_ADDR_SHARE_STRU;



/*****************************************************************************
 结构名    : PHY_HIFI_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 物理层和HIFI的共享地址，OAM需要拷贝到HIFI的BOOT区共享数据中的相应结构体
*****************************************************************************/
typedef struct
{
    unsigned long uwProtectWord;           /*0x5a5a5a5a*/
    unsigned long uwZspDtcmBaseAddr;       /*ZSP的基地址,32位格式*/
    unsigned long uwHifiPhyMboxHeadAddr;   /*HIFI和PHY的共享区偏移地址定义，PHY负责初始化（须静态初始化，,使用者必须*2+基址）*/
    unsigned long uwHifiPhyMboxBodyAddr;   /*HIFI和PHY的共享区偏移地址定义，PHY负责初始化（须静态初始化，,使用者必须*2+基址）*/
    unsigned long uwPhyHifiMboxHeadAddr;   /*PHY和HIFI的共享区偏移地址定义，PHY负责初始化（须静态初始化  ,使用者必须*2+基址）*/
    unsigned long uwPhyHifiMboxBodyAddr;   /*PHY和HIFI的共享区偏移地址定义，PHY负责初始化（须静态初始化  ,使用者必须*2+基址）*/
    unsigned long uwReserved[2];
}PHY_HIFI_DYN_ADDR_SHARE_STRU;
#if 0
/*****************************************************************************
 结构名    : CARM_HIFI_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :CARM和HIFI的共享地址，OAM需要赋值并拷贝至Hifi共享数据中的相应结构体
*****************************************************************************/
typedef struct
{
    unsigned long uwProtectWord;       /*0x5a5a5a5a*/
    unsigned long uwCarmHifiMBAddr;    /*CARM和HIFI的邮箱共享区定义，CARM负责初始化（须静态初始化）*/
    unsigned long uwNvBaseAddrPhy;     /*DDR上NV备份区首地址的物理地址*/
    unsigned long uwNvBaseAddrVirt;    /*DDR上NV备份区首地址的ARM虚拟地址*/
    unsigned long uwTempratureAddr;    /*温保空间首地址*/
    unsigned long uwStatusFlagAddr;    /*HIFI/DSP STATUS_FLAG_ADDR*/
    unsigned long uwReserved[6];
}CARM_HIFI_DYN_ADDR_SHARE_STRU;
#endif
/*****************************************************************************
 结构名    : PS_PHY_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DSP上电时OAM需要往DSP的".dsp_share_addr"段写如下结构的数据，起始地址由DRV解析ZSP bin件后传给OAM
*****************************************************************************/
typedef struct
{
    PS_PHY_DYN_ADDR_SHARE_STRU      stPsPhyDynAddr;     /*PS和PHY的共享区定义*/
    PHY_HIFI_DYN_ADDR_SHARE_STRU    stPhyHifiDynAddr;   /*PHY和HIFI的共享区定义*/
}DSP_SHARE_ADDR_STRU;
#if 0
/*****************************************************************************
 结构名    : HIFI_SHARE_ADDR_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : HIFI上电时OAM需要往Hifi的".sram1.shareaddr"段写如下结构的数据，起始
             地址为底软分配给Hifi的DDR首地址
*****************************************************************************/
typedef struct
{
    PHY_HIFI_DYN_ADDR_SHARE_STRU    stPhyHifiDynAddr;   /*PHY和HIFI的共享区定义*/
    CARM_HIFI_DYN_ADDR_SHARE_STRU   stCarmHifiDynAddr;  /*CARM与HIFI的共享区定义*/
}HIFI_SHARE_ADDR_STRU;
#endif
/*****************************************************************************
 结构名    : CPHY_TEMP_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 温度补偿相关数据结构
*****************************************************************************/
typedef struct
{
    /* ulTempType:
                   bit0 1:PA 温度变化 0:不变化
                   bit1 1:USIM温度变化
                   bit2 1:电压变化 */

    unsigned long                   ulTempType;
    unsigned short                  usPA1Adc;                   /* PA1 ADC值 */
    short                           sPA1Temperature;            /* PA1 温度 */
    unsigned short                  usPA2Adc;                   /* PA2 ADC值 */
    short                           sPA2Temperature;            /* PA2 温度 */
    short                           sTProtectTemperature;       /* DSP 温保温度，stick是USIM卡温度，E5是电池温度 */
    unsigned short                  usDCXOAdcResult;            /* DCXO温度ADC值 */
    unsigned short                  usBatVoltage;               /* 电池电压保留 */
    unsigned short                  usRsv1;
    unsigned short                  usSarAntState;              /*  */
    unsigned short                  usSarReduction;             /*  */
    unsigned long                   ulSlaveLPPowerInit;         /* 从模低功耗上电保护标志位 */
    unsigned long                   ulSlaveLPPowerInitSlice;    /*  */
    unsigned long                   ulSlaveLPCodeLoading;       /* 从模低功耗加载保护标志位 */
    unsigned long                   ulSlaveLPCodeLoadingSlice;  /*  */
    unsigned short                  usRev[30];
} CPHY_TEMP_INFO_STRU;

/*****************************************************************************
 结构名    : OMMED_RING_BUFFER_CONTROL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : ring buffer控制信息结构体
*****************************************************************************/
typedef struct
{
    unsigned long                          uwProtectWord1;                         /*保护字 0x55AA55AA*/
    unsigned long                          uwProtectWord2;                         /*保护字 0x5A5A5A5A*/
    unsigned long                          uwWriteAddr;                            /*下一个写入操作的相对地址,其指向单位为Byte*/
    unsigned long                          uwReadAddr;                             /*下一个读取操作的相对地址,其指向单位为Byte*/
    unsigned long                          uwBufSize;                              /*ring buffer的长度,单位byte */
    unsigned long                          uwBufAddr;                              /*ring buffer的基址*/
    unsigned long                          uwProtectWord3;                         /*保护字 0x55AA55AA*/
    unsigned long                          uwProtectWord4;                         /*保护字 0x5A5A5A5A*/
}RING_BUFFER_CONTROL_STRU;
/************************************************************************
 结构名    : IPC_MAILBOX_HEAD_STRU
 结构说明  : 核间邮箱缓存头
 ************************************************************************/
typedef struct
{
    unsigned long      ulProtectWord1;     /*保护字 0x55AA55AA*/
    unsigned long      ulProtectWord2;     /*保护字 0x5A5A5A5A*/
    unsigned long      ulFront;            /*队列待写单元距离队列(不含保护字)头的长度，单位32bit */
    unsigned long      ulRear;             /*队列待读单元距离队列(不含保护字)头的长度，单位32bit */
    unsigned long      ulFrontslice;       /*更新邮箱环形队列的头指针系统时间*/
    unsigned long      ulRearslice;        /*更新邮箱环形队列的尾指针系统时间*/
    unsigned short     ausReserve[4];      /*保留*/
    unsigned long      ulProtectWord3;     /*保护字 0x55AA55AA*/
    unsigned long      ulProtectWord4;     /*保护字 0x5A5A5A5A*/
} IPC_MAILBOX_HEAD_STRU;


/*****************************************************************************
 实体名称  : DRV_CARM_HIFI_MB_STRU
 功能描述  : CARM_HIFI跨核邮箱内存结构，由OAM指定邮箱空间的首地址
*****************************************************************************/
typedef struct
{
    IPC_MAILBOX_HEAD_STRU               stCarm2HifiHead;                        /* CARM给HIFI的邮箱邮箱头 */
    unsigned char                       aCarm2Hifi[IPC_MAILBOX_SIZE_CARM2HIFI]; /* CARM给HIFI的邮箱  */
    IPC_MAILBOX_HEAD_STRU               stHifi2CarmHead;                        /* HIFI给CARM的邮箱邮箱头 */
    unsigned char                       aHifi2Carm[IPC_MAILBOX_SIZE_HIFI2CARM]; /* HIFI给CARM的邮箱 */
    RING_BUFFER_CONTROL_STRU            stTxRing;
    RING_BUFFER_CONTROL_STRU            stRxRing;
}DRV_CARM_HIFI_MB_STRU;

/*****************************************************************************
 实体名称  : DRV_CARM_HIFI_MB_STRU
 功能描述  : CARM_HIFI跨核邮箱内存结构，由OAM指定邮箱空间的首地址
*****************************************************************************/
typedef struct
{
    unsigned long                  uwHifiStatus;
    unsigned long                  uwIpcMailboxStatusHifi2Carm;
    unsigned long                  uwIpcMailboxStatusCarm2Hifi;
    unsigned long                  uwZspStatus;
    unsigned long                  uwRsv[4];
}DSP_DRX_STATUS_STRU;

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

#endif /* end of IpcMailboxInterface.h */

