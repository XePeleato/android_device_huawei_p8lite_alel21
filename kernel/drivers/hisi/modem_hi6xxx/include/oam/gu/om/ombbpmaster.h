/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : omnosig.h
  版 本 号   : 初稿
  作    者   : 甘兰47350
  生成日期   : 2011年9月27日

  最近修改   :
  功能描述   : omnosig.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年9月27日
    作    者   : 甘兰47350
    修改内容   : 创建文件

******************************************************************************/

#ifndef __OMBBPMASTER_H__
#define __OMBBPMASTER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef _SDR_FEATURE_ON
#include "vos.h"
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define OM_BBPMASTER_IPC_LOCK_IDLE                      (0)
#define OM_BBPMASTER_IPC_LOCK_BUSY                      (9)

#define OM_BBPMASTER_WR_CHAN_CNT                        (5) /* 分配给GUX的写通道数目 */
#define OM_BBPMASTER_RD_CHAN_CNT                        (4) /* 分配给GUX的写通道数目 */

#define OM_BBPMASTER_WR_CHANNEL                         (0) /* BBP Master写通道 */
#define OM_BBPMASTER_RD_CHANNEL                         (1) /* BBP Master写通道 */

/*当前记录文件最大姓名，结构体中使用需要4字节对齐*/
#define OM_BBPMASTER_LOG_FILE_NAME_LEN                  (0x10)


#define OM_BBP_MASTER_LOG_FILE_MAX_SIZE                 (0x400)
#define OM_FILE_OPEN_MODE_RW                            "ab+"   /* 打开bin文件，在后部追加写入 */
#define OM_FILE_OPEN_MODE_NEW_RW                        "wb+"   /* 打开bin文件，从头开始覆盖追加写入 */

#define OM_IPC_SEM_INT_MASK(i)                          (0x604 + (i * 0x10))                /* 信号量释放中断掩码寄存器 */
#define OM_IPC_SEM_INT_CLR(i)                           (0x60C + (i * 0x10))                /* 信号量释放中断清除寄存器 */
#define OM_IPC_HS_CTRL(i,j)                             (0x800 + (i * 0x100) + (j * 0x8 ))  /* 信号量请求寄存器 */

#if ( VOS_WIN32 == VOS_OS_VER )
#define IPC_SEM_BBPMASTER_0                             (10)    /* IPC资源锁10，用于写通道0互斥保护 */
#define IPC_SEM_BBPMASTER_1                             (11)    /* IPC资源锁11，用于写通道1互斥保护 */
#define IPC_SEM_BBPMASTER_2                             (12)    /* IPC资源锁12，用于写通道2互斥保护 */
#define IPC_SEM_BBPMASTER_3                             (13)    /* IPC资源锁13，用于写通道3互斥保护 */
#define IPC_SEM_BBPMASTER_4                             (14)    /* IPC资源锁14，用于写通道4互斥保护 */
#define IPC_SEM_BBPMASTER_5                             (15)    /* IPC资源锁15，用于读通道0互斥保护 */
#define IPC_SEM_BBPMASTER_6                             (16)    /* IPC资源锁16，用于读通道1互斥保护 */
#define IPC_SEM_BBPMASTER_7                             (17)    /* IPC资源锁17，用于读通道2互斥保护 */
#define IPC_SEM_BBPMASTER_8                             (18)    /* IPC资源锁18，用于读通道3互斥保护 */
#endif

#if ( VOS_VXWORKS == VOS_OS_VER )
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
#define OM_BBPMASTER_LOG_PATH                           "/mlog:0/DrvLog/OmBbpMasterError"

#define OM_BBPMASTER_LOGCTRL_FILE                       "/mlog:0/DrvLog/OmBbpMasterError/OmBbpMasterlogctrl.bin"
#else
#define OM_BBPMASTER_LOG_PATH                           "/yaffs0/DrvLog/OmBbpMasterError"

#define OM_BBPMASTER_LOGCTRL_FILE                       "/yaffs0/DrvLog/OmBbpMasterError/OmBbpMasterlogctrl.bin"
#endif
#endif

#ifdef _SDR_FEATURE_ON
#define BBE16_PID_RESERVED                              (19)
#endif

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum OM_BBPMASTER_CFG_DECFG_TYPE_ENUM
{
    OM_BBPMASTER_CONFIG_NONE                = 0,    /* 配置通道接口返回值枚举: 未配置 */
    OM_BBPMASTER_CONFIG_SUCC                = 1,    /* 使能配置通道接口返回值枚举: 配置成功 */
    OM_BBPMASTER_PARA_CHANID_INVALID        = 2,    /* 使能配置通道接口返回值枚举: 配置号无效 */
    OM_BBPMASTER_PARA_CFG_NULL              = 3,    /* 使能配置通道接口返回值枚举: 配置参数为空指针 */
    OM_BBPMASTER_PARA_CFG_WORKMODE          = 4,    /* 使能配置通道接口返回值枚举: 配置工作模式参数错误 */
    OM_BBPMASTER_PARA_CFG_CLOCKSEL          = 5,    /* 使能配置通道接口返回值枚举: 配置时钟选择参数错误 */
    OM_BBPMASTER_PARA_CFG_AXISEL            = 6,    /* 使能配置通道接口返回值枚举: 配置AXI选择参数错误 */
    OM_BBPMASTER_PARA_CFG_BURSTLEN          = 7,    /* 使能配置通道接口返回值枚举: 配置BurstLen参数错误 */
    OM_BBPMASTER_CONFIG_CHAN_COLLIDE        = 8,    /* 使能配置通道接口返回值枚举: 发生配置冲突 */
    OM_BBPMASTER_LOCK_IPC_FAIL              = 9,    /* 使能配置通道接口返回值枚举: 获取IPC资源锁失败 */
    OM_BBPMASTER_DECONFIG_PARA_INVALID      = 10,   /* 去使能配置通道接口返回值枚举: 配置参数无效 */
    OM_BBPMASTER_DECFG_PARA_NULL            = 11,   /* 去使能配置通道接口返回值枚举: 配置参数为空指针 */
    OM_BBPMASTER_DECFG_CHAN_IDLE            = 12,   /* 去使能配置通道接口返回值枚举: 配置去使能通道状态为IDLE */
    OM_BBPMASTER_DECFG_PID_CHANGE           = 13,   /* 去使能配置通道接口返回值枚举: 通道的使能PID与去使能PID不同 */
    OM_BBPMASTER_DECFG_SUCC                 = 14,   /* 去使能配置通道接口返回值枚举: 配置成功 */
    OM_BBPMASTER_CFG_DECFG_TYPE_BUTT
};

#ifdef _SDR_FEATURE_ON
typedef unsigned long OM_BBPMASTER_CFG_DECFG_TYPE_ENUM_UINT32;
#else
typedef VOS_UINT32 OM_BBPMASTER_CFG_DECFG_TYPE_ENUM_UINT32;
#endif

enum OM_BBPMASTER_CHAN_STATE_ENUM
{
    OM_BBPMASTER_CHAN_STATE_IDLE            = 0,    /* 通道状态:未使能状态 */
    OM_BBPMASTER_CHAN_STATE_BUSY            = 1,    /* 通道状态:已使能状态 */
    OM_BBPMASTER_CHAN_STATE_BUTT
};

#ifdef _SDR_FEATURE_ON
typedef unsigned long OM_BBPMASTER_CHAN_STATE_ENUM_UINT32;
#else
typedef VOS_UINT32 OM_BBPMASTER_CHAN_STATE_ENUM_UINT32;
#endif

enum OM_BBPMASTER_WORKMODE_ENUM
{
    OM_BBPMASTER_WORKMODE_DISABLE           = 0,    /* 3'b000 ：通道非使能
                                                            此时该通道向前级的BBP输出valid/ready为0，
                                                            向后级的读写控制不再发出新的读/写请求，
                                                            如果所有的读写通道都没有新请求，则master口被释放，总线状态在cpu接口上报，
                                                            软件结合该状态可启动门控或掉电流程 */
    OM_BBPMASTER_WORKMODE_INTERFACE_1       = 1,    /* 3'b001：通道配置给第一组接口使用 */
    OM_BBPMASTER_WORKMODE_INTERFACE_2       = 2,    /* 3'b010：通道配置给第二组接口使用 */
    OM_BBPMASTER_WORKMODE_RESERVE           = 3,    /* 3'b100：通道配置给第三组接口使用 */
    OM_BBPMASTER_WORKMODE_INTERFACE_3       = 4,
    OM_BBPMASTER_WORKMODE_BUTT
};

#ifdef _SDR_FEATURE_ON
typedef unsigned long OM_BBPMASTER_WORKMODE_ENUM_UINT32;
#else
typedef VOS_UINT32 OM_BBPMASTER_WORKMODE_ENUM_UINT32;
#endif

enum OM_BBPMASTER_CLOCKSEL_ENUM
{
    OM_BBPMASTER_CLOCKSEL_RESERVE_0         = 0,    /* 保留时钟域枚举值，无效 */
    OM_BBPMASTER_CLOCKSEL_1                 = 1,    /* 3'b001：该通道的输入接口使用第一种时钟域,104MHz，为g模的工作时钟； */
    OM_BBPMASTER_CLOCKSEL_2                 = 2,    /* 3'b010：该通道的输入接口使用第二种时钟域,122.88MHz，
                                                             为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域； */
    OM_BBPMASTER_CLOCKSEL_RESERVE           = 3,    /* 保留时钟域枚举值，无效 */
    OM_BBPMASTER_CLOCKSEL_3                 = 4,    /* 3'b100：该通道的输入接口使用第三种时钟域,
                                                            245.76MHz，为U模中译码的工作时钟； */
    OM_BBPMASTER_CLOCKSEL_BUTT
};

#ifdef _SDR_FEATURE_ON
typedef unsigned long OM_BBPMASTER_CLOCKSEL_ENUM_UINT32;
#else
typedef VOS_UINT32 OM_BBPMASTER_CLOCKSEL_ENUM_UINT32;
#endif

enum OM_BBPMASTER_BURST_LEN_ENUM
{
    OM_BBPMASTER_BURST_LEN_1                = 0,    /* 4'b0000对应burst1 */
    OM_BBPMASTER_BURST_LEN_2                = 1,    /* 4'b0001对应burst2 */
    OM_BBPMASTER_BURST_LEN_3                = 2,    /* 4'b0010对应burst3 */
    OM_BBPMASTER_BURST_LEN_4                = 3,    /* 4'b0011对应burst4 */
    OM_BBPMASTER_BURST_LEN_5                = 4,    /* 4'b0100对应burst5 */
    OM_BBPMASTER_BURST_LEN_6                = 5,    /* 4'b0101对应burst6 */
    OM_BBPMASTER_BURST_LEN_7                = 6,    /* 4'b0110对应burst7 */
    OM_BBPMASTER_BURST_LEN_8                = 7,    /* 4'b0111对应burst8 */
    OM_BBPMASTER_BURST_LEN_9                = 8,    /* 4'b1000对应burst9 */
    OM_BBPMASTER_BURST_LEN_10               = 9,    /* 4'b1001对应burst10 */
    OM_BBPMASTER_BURST_LEN_11               = 10,   /* 4'b1010对应burst11 */
    OM_BBPMASTER_BURST_LEN_12               = 11,   /* 4'b1011对应burst12 */
    OM_BBPMASTER_BURST_LEN_13               = 12,   /* 4'b1100对应burst13 */
    OM_BBPMASTER_BURST_LEN_14               = 13,   /* 4'b1101对应burst14 */
    OM_BBPMASTER_BURST_LEN_15               = 14,   /* 4'b1110对应burst15 */
    OM_BBPMASTER_BURST_LEN_16               = 15,   /* 4'b1111对应burst16,（默认使用） */
    OM_BBPMASTER_BURST_LEN_BUTT
};

#ifdef _SDR_FEATURE_ON
typedef unsigned long OM_BBPMASTER_BURST_LEN_ENUM_UINT32;
#else
typedef VOS_UINT32 OM_BBPMASTER_BURST_LEN_ENUM_UINT32;
#endif

enum OM_BBPMASTER_AXI_SEL_ENUM
{
    OM_BBPMASTER_AXI_SEL_INTERFACE1         = 0,    /* 通道选择使用的master口1 */
    OM_BBPMASTER_AXI_SEL_INTERFACE2         = 1,    /* 通道选择使用的master口2 */
    OM_BBPMASTER_AXI_SEL_BUTT
};

#ifdef _SDR_FEATURE_ON
typedef unsigned long OM_BBPMASTER_AXI_SEL_ENUM_UINT32;
#else
typedef VOS_UINT32 OM_BBPMASTER_AXI_SEL_ENUM_UINT32;
#endif

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
#ifndef _SDR_FEATURE_ON
typedef struct
{
    OM_BBPMASTER_CHAN_STATE_ENUM_UINT32         enChanState;    /* 通道配置状态 */
    VOS_PID                                     ulConfigPID;    /* 通道配置完成的PID */
    VOS_UINT32                                  ulConfigSlice;  /* 通道配置完成时刻 */
}BBPMASTER_ENTITY_STRU;

typedef struct
{
    OM_BBPMASTER_CFG_DECFG_TYPE_ENUM_UINT32     enResult;       /* 接口配置结果 */
    VOS_PID                                     ulConfigPID;    /* 调用配置接口的PID */
    VOS_UINT32                                  ulConfigSlice;  /* 调用配置接口时刻 */
}BBPMASTER_ERROR_STRU;

typedef struct
{
    OM_BBPMASTER_WORKMODE_ENUM_UINT32           enWorkMode;     /* 使能写通道配置参数:工作模式 */
    OM_BBPMASTER_CLOCKSEL_ENUM_UINT32           enClockSel;     /* 使能写通道配置参数:时钟选择 */
    OM_BBPMASTER_AXI_SEL_ENUM_UINT32            enAxiSel;       /* 使能写通道配置参数:AXI通道选择 */
    VOS_UINT32                                  ulStartAddr;    /* 使能写通道配置参数:写通道起始地址 */
    VOS_UINT32                                  ulEndAddr;      /* 使能写通道配置参数:写通道终止地址 */
    OM_BBPMASTER_BURST_LEN_ENUM_UINT32          enBurstLen;     /* 使能写通道配置参数:Burst长度类型 */
}BBPMASTER_WR_CHANNEL_CONFIG_STRU;

typedef struct
{
    VOS_UINT32                                  ulWorkModeAddr; /* 写通道配置参数:工作模式配置寄存器，使能、去使能接口复用 */
    VOS_UINT32                                  ulClockSelAddr; /* 写通道配置参数:时钟选择配置寄存器，使能、去使能接口复用 */
    VOS_UINT32                                  ulAxiSelAddr;   /* 写通道配置参数:AXI通道选择配置寄存器，使能、去使能接口复用 */
    VOS_UINT32                                  ulStartAddr;    /* 写通道配置参数:写通道起始地址配置寄存器，使能、去使能接口复用 */
    VOS_UINT32                                  ulEndAddr;      /* 写通道配置参数:写通道终止地址配置寄存器，使能、去使能接口复用 */
    VOS_UINT32                                  ulBurstLenAddr; /* 写通道配置参数:Burst长度类型配置寄存器，使能、去使能接口复用 */
}BBPMASTER_WR_CHANNEL_CONFIG_ADDR_STRU;

typedef struct
{
    OM_BBPMASTER_WORKMODE_ENUM_UINT32           enWorkMode;     /* 去使能通道配置参数:工作模式配置寄存器 */
    OM_BBPMASTER_CLOCKSEL_ENUM_UINT32           enClockSel;     /* 去使能通道配置参数:时钟选择配置寄存器 */
}BBPMASTER_CHANNEL_DECONFIG_STRU;

typedef struct
{
    OM_BBPMASTER_WORKMODE_ENUM_UINT32           enWorkMode;     /* 使能读通道配置参数:工作模式 */
    OM_BBPMASTER_CLOCKSEL_ENUM_UINT32           enClockSel;     /* 使能读通道配置参数:时钟选择 */
    OM_BBPMASTER_AXI_SEL_ENUM_UINT32            enAxiSel;       /* 使能读通道配置参数:AXI通道选择 */
    VOS_UINT32                                  ulStartAddr;    /* 使能读通道配置参数:读通道起始地址 */
    OM_BBPMASTER_BURST_LEN_ENUM_UINT32          enBurstLen;     /* 通道配置参数:Burst长度类型 */
}BBPMASTER_RD_CHANNEL_CONFIG_STRU;

typedef struct
{
    VOS_UINT32                                  ulWorkModeAddr; /* 读通道配置参数:工作模式配置寄存器，使能、去使能接口复用 */
    VOS_UINT32                                  ulClockSelAddr; /* 读通道配置参数:时钟选择配置寄存器，使能、去使能接口复用 */
    VOS_UINT32                                  ulAxiSelAddr;   /* 读通道配置参数:AXI通道选择配置寄存器，使能、去使能接口复用 */
    VOS_UINT32                                  ulStartAddr;    /* 读通道配置参数:读通道起始地址配置寄存器，使能、去使能接口复用 */
    VOS_UINT32                                  ulBurstLenAddr; /* 读通道配置参数:Burst长度类型配置寄存器，使能、去使能接口复用 */
}BBPMASTER_RD_CHANNEL_CONFIG_ADDR_STRU;
#else
typedef struct
{
    OM_BBPMASTER_CHAN_STATE_ENUM_UINT32         enChanState;    /* 通道配置状态 */
    unsigned long                               ulConfigPID;    /* 通道配置完成的PID */
    unsigned long                               ulConfigSlice;  /* 通道配置完成时刻 */
}BBPMASTER_ENTITY_STRU;

typedef struct
{
    OM_BBPMASTER_CFG_DECFG_TYPE_ENUM_UINT32     enResult;       /* 接口配置结果 */
    unsigned long                               ulConfigPID;    /* 调用配置接口的PID */
    unsigned long                               ulConfigSlice;  /* 调用配置接口时刻 */
}BBPMASTER_ERROR_STRU;

typedef struct
{
    OM_BBPMASTER_WORKMODE_ENUM_UINT32           enWorkMode;     /* 使能写通道配置参数:工作模式 */
    OM_BBPMASTER_CLOCKSEL_ENUM_UINT32           enClockSel;     /* 使能写通道配置参数:时钟选择 */
    OM_BBPMASTER_AXI_SEL_ENUM_UINT32            enAxiSel;       /* 使能写通道配置参数:AXI通道选择 */
    unsigned long                               ulStartAddr;    /* 使能写通道配置参数:写通道起始地址 */
    unsigned long                               ulEndAddr;      /* 使能写通道配置参数:写通道终止地址 */
    OM_BBPMASTER_BURST_LEN_ENUM_UINT32          enBurstLen;     /* 使能写通道配置参数:Burst长度类型 */
}BBPMASTER_WR_CHANNEL_CONFIG_STRU;

typedef struct
{
    unsigned long                               ulWorkModeAddr; /* 写通道配置参数:工作模式配置寄存器，使能、去使能接口复用 */
    unsigned long                               ulClockSelAddr; /* 写通道配置参数:时钟选择配置寄存器，使能、去使能接口复用 */
    unsigned long                               ulAxiSelAddr;   /* 写通道配置参数:AXI通道选择配置寄存器，使能、去使能接口复用 */
    unsigned long                               ulStartAddr;    /* 写通道配置参数:写通道起始地址配置寄存器，使能、去使能接口复用 */
    unsigned long                               ulEndAddr;      /* 写通道配置参数:写通道终止地址配置寄存器，使能、去使能接口复用 */
    unsigned long                               ulBurstLenAddr; /* 写通道配置参数:Burst长度类型配置寄存器，使能、去使能接口复用 */
}BBPMASTER_WR_CHANNEL_CONFIG_ADDR_STRU;

typedef struct
{
    OM_BBPMASTER_WORKMODE_ENUM_UINT32           enWorkMode;     /* 去使能通道配置参数:工作模式配置寄存器 */
    OM_BBPMASTER_CLOCKSEL_ENUM_UINT32           enClockSel;     /* 去使能通道配置参数:时钟选择配置寄存器 */
}BBPMASTER_CHANNEL_DECONFIG_STRU;

typedef struct
{
    OM_BBPMASTER_WORKMODE_ENUM_UINT32           enWorkMode;     /* 使能读通道配置参数:工作模式 */
    OM_BBPMASTER_CLOCKSEL_ENUM_UINT32           enClockSel;     /* 使能读通道配置参数:时钟选择 */
    OM_BBPMASTER_AXI_SEL_ENUM_UINT32            enAxiSel;       /* 使能读通道配置参数:AXI通道选择 */
    unsigned long                               ulStartAddr;    /* 使能读通道配置参数:读通道起始地址 */
    OM_BBPMASTER_BURST_LEN_ENUM_UINT32          enBurstLen;     /* 通道配置参数:Burst长度类型 */
}BBPMASTER_RD_CHANNEL_CONFIG_STRU;

typedef struct
{
    unsigned long                               ulWorkModeAddr; /* 读通道配置参数:工作模式配置寄存器，使能、去使能接口复用 */
    unsigned long                               ulClockSelAddr; /* 读通道配置参数:时钟选择配置寄存器，使能、去使能接口复用 */
    unsigned long                               ulAxiSelAddr;   /* 读通道配置参数:AXI通道选择配置寄存器，使能、去使能接口复用 */
    unsigned long                               ulStartAddr;    /* 读通道配置参数:读通道起始地址配置寄存器，使能、去使能接口复用 */
    unsigned long                               ulBurstLenAddr; /* 读通道配置参数:Burst长度类型配置寄存器，使能、去使能接口复用 */
}BBPMASTER_RD_CHANNEL_CONFIG_ADDR_STRU;
#endif

/*****************************************************************************
  5 消息头定义
*****************************************************************************/
#ifndef _SDR_FEATURE_ON
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                                  ulMsgID;        /* BBP Master配置错误上报消息ID */
    VOS_UINT32                                  ulChanType;     /* BBP Master通道类型:WR/RD */
    VOS_UINT32                                  ulChanId;       /* BBP Master通道号 */
    BBPMASTER_ENTITY_STRU                       stEntityInfo;   /* BBP Master通道信息 */
    BBPMASTER_ERROR_STRU                        stErrorInfo;    /* BBP Master配置错误信息 */
}BBPMASTER_OM_ERROR_IND_STRU;
#endif


/*****************************************************************************
  6 消息定义
*****************************************************************************/
#define ID_OM_BBP_MASTER_CONFIG_ERROR_IND               (0xF086)

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern OM_BBPMASTER_CFG_DECFG_TYPE_ENUM_UINT32 OM_BbpMasterRdChannelConfig(VOS_PID ulPID,
                                                               VOS_UINT32 ulChanId,
                                                               BBPMASTER_RD_CHANNEL_CONFIG_STRU *pstRdChanCfg);
extern OM_BBPMASTER_CFG_DECFG_TYPE_ENUM_UINT32 OM_BbpMasterRdChannelDeConfig(VOS_PID ulPID,
                                                               VOS_UINT32 ulChanId,
                                                               BBPMASTER_CHANNEL_DECONFIG_STRU *pstChanDeCfg);

extern OM_BBPMASTER_CFG_DECFG_TYPE_ENUM_UINT32 OM_BbpMasterWrChannelConfig(VOS_PID ulPID,
                                                               VOS_UINT32 ulChanId,
                                                               BBPMASTER_WR_CHANNEL_CONFIG_STRU *pstWrChanCfg);
extern OM_BBPMASTER_CFG_DECFG_TYPE_ENUM_UINT32 OM_BbpMasterWrChannelDeConfig(VOS_PID ulPID,
                                                               VOS_UINT32 ulChanId,
                                                               BBPMASTER_CHANNEL_DECONFIG_STRU *pstChanDeCfg);

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of omnosig.h */

