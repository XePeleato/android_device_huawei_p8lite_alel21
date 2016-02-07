

#ifndef _BSP_SOCP_DRV_H
#define _BSP_SOCP_DRV_H

/**************************************************************************
  头文件包含
**************************************************************************/
#include "product_config.h"

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
#include "vos.h"
#include "SOCPInterface.h"

#if(VOS_OS_VER == VOS_LINUX)
#include <asm/dma-mapping.h>
#include <linux/dma-mapping.h>
#include <asm/io.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/io.h>
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/**************************************************************************
  宏定义
**************************************************************************/
/*编译宏开关，根据当前的Cpu进行确定*/
#if (VOS_OS_VER == VOS_WIN32)
#define SOCP_ACORE
#else

#if(OSA_CPU_ACPU == VOS_OSA_CPU)
#define SOCP_ACORE
#endif

#if(OSA_CPU_CCPU == VOS_OSA_CPU)
#define SOCP_CCORE
#endif
#endif    /*(VOS_OS_VER == VOS_WIN32)*/

#define SOCP_VIRT_PHY(virt)             (virt)
#define SOCP_PHY_VIRT(phy)              (phy)

#if(VOS_OS_VER == VOS_LINUX)
/*#define SOCP_VIRT_PHY(virt)             virt_to_phys((void*)virt)
 */
/*#define SOCP_PHY_VIRT(phy)              phys_to_virt((unsigned long)phy)
 */
#define SOCP_VIRT_BUS(virt)             virt_to_bus((void*)virt)
#define SOCP_BUS_VIRT(bus)              bus_to_virt((unsigned long)bus)
#endif

#if((VOS_OS_VER == VOS_VXWORKS)||(VOS_OS_VER == VOS_WIN32)||(VOS_OS_VER == VOS_RTOSCK) )
#define SOCP_VIRT_BUS(virt)             (virt)
#define SOCP_BUS_VIRT(bus)              (bus)
#endif

#define SOCP_REGISTER_SIZE                          (0x1000)

#define SOCP_ERROR_INFO_TAG                         (0x5A)

#define SOCP_ERRORNO_ENCDST_READ_DONE_REGERR        (0x20000050)

/* SOCP缓存数据立即输出最大延迟时间，ms为单位 */
#define SOCP_LOG_FLUSH_MAX_OVER_TIME                (100)

#define SOCP_OLD_VER                                (0x200)

/**************************************************************************
  寄存器定义,偏移地址
**************************************************************************/
/* BBP 通道寄存器 */
#define BBP_REG_LOG_ADDR(m)     (0x0200+0x10*m)
#define BBP_REG_LOG_WPTR(m)     (0x0204+0x10*m)
#define BBP_REG_LOG_RPTR(m)     (0x0208+0x10*m)
#define BBP_REG_LOG_CFG(m)      (0x020C+0x10*m)

#define BBP_REG_DS_ADDR         (0x0280)
#define BBP_REG_DS_WPTR         (0x0284)
#define BBP_REG_DS_RPTR         (0x0288)
#define BBP_REG_DS_CFG          (0x028C)
#define BBP_REG_PTR_ADDR        (0x0290)
#define BBP_REG_CH_EN           (0x0294)
#define BBP_REG_PKT_CNT         (0x0298)
#define BBP_REG_CH_STAT         (0x029C)

/* 基础寄存器 */
#define SOCP_REG_GBLRST         (0x0000)
#define SOCP_REG_ENCRST         (0x0004)
#define SOCP_REG_DECRST         (0x0008)
#define SOCP_REG_ENCSTAT        (0x000C)
#define SOCP_REG_DECSTAT        (0x0010)
#define SOCP_REG_CLKCTRL        (0x0014)
#define SOCP_REG_PRICFG         (0x0018)
#define SOCP_REG_DECINTTIMEOUT  (0x0020)
#define SOCP_REG_INTTIMEOUT     (0x0024)
#define SOCP_REG_BUFTIMEOUT     (0x0028)
#define SOCP_REG_DEC_PKTLEN     (0x002C)
#define SOCP_REG_GBL_INTSTAT    (0x0050)

/* 编码器 中断寄存器*/
#define SOCP_REG_ENC_MASK0          (0x0054)
#define SOCP_REG_ENC_RAWINT0        (0x0058)
#define SOCP_REG_ENC_INTSTAT0       (0x005C)    /*中断状态寄存器，在芯片手册中所写为ENC_MASKINT0，此处改为ENC_INTSTAT0*/
#define SOCP_REG_APP_MASK1          (0x0060)
#define SOCP_REG_MODEM_MASK1        (0x0064)
#define SOCP_REG_ENC_RAWINT1        (0x0068)
#define SOCP_REG_APP_INTSTAT1       (0x006C)
#define SOCP_REG_MODEM_INTSTAT1     (0x0070)
#define SOCP_REG_ENC_MASK2          (0x0074)
#define SOCP_REG_ENC_RAWINT2        (0x0078)
#define SOCP_REG_ENC_INTSTAT2       (0x007C)
#define SOCP_REG_APP_MASK3          (0x0080)
#define SOCP_REG_MODEM_MASK3        (0x0084)
#define SOCP_REG_ENC_RAWINT3        (0x0088)
#define SOCP_REG_APP_INTSTAT3       (0x008C)
#define SOCP_REG_MODEM_INTSTAT3     (0x0090)
#define SOCP_REG_MODEM_ENC_MASK0    (0x00DC)    /* CORE1编码传输中断屏蔽寄存器 */
#define SOCP_REG_MODEM_ENC_INT0     (0x00E0)    /* CORE1编码传输中断状态寄存器 */
#define SOCP_REG_MODEM_ENC_MASK2    (0x00E4)    /* CORE1编码目的buffer上溢中断屏蔽寄存器 */
#define SOCP_REG_MODEM_ENC_INT2     (0x00E8)    /* CORE1编码目的buffer上溢中断状态寄存器 */

/* 解码器中断寄存器*/

#define SOCP_REG_DEC_CORE0MASK0     (0x00A8)  /*传输中断*/
#define SOCP_REG_DEC_CORE1MASK0     (0x00AC)
#define SOCP_REG_DEC_RAWINT0        (0x00B0)
#define SOCP_REG_DEC_CORE0ISTAT0    (0x00B4)
#define SOCP_REG_DEC_CORE1ISTAT0    (0x00B8)
#define SOCP_REG_DEC_MASK1          (0x00BC)
#define SOCP_REG_DEC_RAWINT1        (0x00C0)
#define SOCP_REG_DEC_INTSTAT1       (0x00C4)
#define SOCP_REG_DEC_CORE0MASK2     (0x00C8) /* 目的buffer溢出*/
#define SOCP_REG_DEC_CORE1MASK2     (0x00CC)
#define SOCP_REG_DEC_RAWINT2        (0x00D0)
#define SOCP_REG_DEC_CORE0ISTAT2    (0x00D4)
#define SOCP_REG_DEC_CORE1ISTAT2    (0x00D8)
#define SOCP_REG_APP_DEC_MASK1      (0x00EC)    /* CORE1解码通道异常中断屏蔽寄存器 */
#define SOCP_REG_APP_DEC_INT1       (0x00F0)    /* CORE1解码通道异常中断状态寄存器 */


/*编码器通道buffer寄存器*/
#define SOCP_REG_ENCSRC_BUFWPTR(m)  (0x0100+m*0x40)
#define SOCP_REG_ENCSRC_BUFRPTR(m)  (0x0104+m*0x40)
#define SOCP_REG_ENCSRC_BUFADDR(m)  (0x0108+m*0x40)
#define SOCP_REG_ENCSRC_BUFCFG0(m)  (0x010C+m*0x40)
#define SOCP_REG_ENCSRC_BUFCFG1(m)  (0x0110+m*0x40)

#define SOCP_REG_ENCSRC_RDQWPTR(m)  (0x0114+m*0x40)
#define SOCP_REG_ENCSRC_RDQRPTR(m)  (0x0118+m*0x40)
#define SOCP_REG_ENCSRC_RDQADDR(m)  (0x011C+m*0x40)
#define SOCP_REG_ENCSRC_RDQCFG(m)   (0x0120+m*0x40)

/* 地址作修改0x10->0x20, 及增加阈值寄存器BUFTHRH */
#define SOCP_REG_ENCDEST_BUFWPTR(n) (0x0900+n*0x20)
#define SOCP_REG_ENCDEST_BUFRPTR(n) (0x0904+n*0x20)
#define SOCP_REG_ENCDEST_BUFADDR(n) (0x0908+n*0x20)
#define SOCP_REG_ENCDEST_BUFCFG(n)  (0x090C+n*0x20)
#define SOCP_REG_ENCDEST_BUFTHRH(n) (0x0910+n*0x20)
#define SOCP_REG_ENCDEST_INTTHRH(n) (0x0914+n*0x20)


/*解码器通道buffer寄存器*/
#define SOCP_REG_DECSRC_BUFWPTR(x)  (0x0A00+x*0x40)
#define SOCP_REG_DECSRC_BUFRPTR(x)  (0x0A04+x*0x40)
#define SOCP_REG_DECSRC_BUFADDR(x)  (0x0A08+x*0x40)
#define SOCP_REG_DECSRC_BUFCFG0(x)  (0x0A0C+x*0x40)

#define SOCP_REG_DECSRC_RDQWPTR(x)  (0x0A10+x*0x40)
#define SOCP_REG_DECSRC_RDQRPTR(x)  (0x0A14+x*0x40)
#define SOCP_REG_DECSRC_RDQADDR(x)  (0x0A18+x*0x40)
#define SOCP_REG_DECSRC_RDQCFG(x)   (0x0A1C+x*0x40)

#define SOCP_REG_DEC_BUFSTAT0(x)    (0x0A20+x*0x40)
#define SOCP_REG_DEC_BUFSTAT1(x)    (0x0A24+x*0x40)


#define SOCP_REG_DECDEST_BUFWPTR(y) (0x0C00+y*0x10)
#define SOCP_REG_DECDEST_BUFRPTR(y) (0x0C04+y*0x10)
#define SOCP_REG_DECDEST_BUFADDR(y) (0x0C08+y*0x10)
#define SOCP_REG_DECDEST_BUFCFG(y)  (0x0C0C+y*0x10)

/*DEBUG寄存器*/
#define SOCP_REG_ENCCD_DBG0         (0x0E00)
#define SOCP_REG_ENCCD_DBG1         (0x0E04)
#define SOCP_REG_ENCIBUF_DBG0       (0x0E08)
#define SOCP_REG_ENCIBUF_DBG1       (0x0E0C)
#define SOCP_REG_ENCIBUF_DBG2       (0x0E10)
#define SOCP_REG_ENCIBUF_DBG3       (0x0E14)
#define SOCP_REG_ENCOBUF_DBG0       (0x0E18)
#define SOCP_REG_ENCOBUF_DBG1       (0x0E1C)
#define SOCP_REG_ENCOBUF_DBG2       (0x0E20)
#define SOCP_REG_ENCOBUF_DBG3       (0x0E24)

#define SOCP_REG_DECIBUF_DBG0       (0x0E28)
#define SOCP_REG_DECIBUF_DBG1       (0x0E2C)
#define SOCP_REG_DECIBUF_DBG2       (0x0E30)
#define SOCP_REG_DECIBUF_DBG3       (0x0E34)
#define SOCP_REG_DECOBUF_DBG0       (0x0E38)
#define SOCP_REG_DECOBUF_DBG1       (0x0E3C)
#define SOCP_REG_DECOBUF_DBG2       (0x0E40)
#define SOCP_REG_DECOBUF_DBG3       (0x0E44)

/* 版本寄存器 */
#define SOCP_REG_VERSION            (0x0FFC)
/**************************************************************************
  结构定义
**************************************************************************/

/* 通道状态结构体，共四类 */
typedef struct
{
    VOS_UINT32                 u32Start;
    VOS_UINT32                 u32End;
    VOS_UINT32                 u32Write;
    VOS_UINT32                 u32Read;
    VOS_UINT32                 u32Length;
    VOS_UINT32                 u32IdleSize;
}SOCP_RING_BUF_STRU;

typedef struct
{
    VOS_UINT32                  u32ChanEn;      /*通道使能状态*/
    VOS_UINT32                  u32SetStat;     /*通道配置状态*/
    SOCP_ENCSRC_CHNMODE_ENUM_UIN32       eChnMode;      /* 数据结构类型 */
    VOS_UINT32                  u32LastRdSize;  /*最后一次获取的RD待释放的大小*/
    SOCP_RING_BUF_STRU          sEncSrcBuf;     /*源空间的信息*/
    SOCP_RING_BUF_STRU          sRdBuf;         /*RD空间信息*/
    socp_event_cb               event_cb;       /*通道事件回调函数*/
    socp_rd_cb                  rd_cb;          /*通道RD事件回调函数*/
}SOCP_ENCSRC_CHAN_STRU;

typedef struct
{
    VOS_UINT32                  u32ChanID;
    VOS_UINT32                  u32SetStat;    /* 通道已经或没有配置的标识*/
    VOS_UINT32                  u32Thrh;       /* 阈值*/
    SOCP_EVENT_ENUM_UIN32       eChnEvent;
    SOCP_RING_BUF_STRU          sEncDstBuf;
    socp_read_cb                read_cb;
}SOCP_ENCDST_CHAN_STRU;

typedef struct
{
    VOS_UINT32                  u32ChanID;
    VOS_UINT32                  u32ChanEn;
    VOS_UINT32                  u32SetStat;    /* 通道已经或没有配置的标识*/
    VOS_UINT32                  u32RdThreshold;
    SOCP_DATA_TYPE_EN_ENUM_UIN32         eDataTypeEn;
    SOCP_DECSRC_CHNMODE_ENUM_UIN32       eChnMode;      /* 数据结构类型 */
    SOCP_RING_BUF_STRU          sDecSrcBuf;
    SOCP_RING_BUF_STRU          sDecRdBuf;
    socp_event_cb               event_cb;
    socp_rd_cb                  rd_cb;
}SOCP_DECSRC_CHAN_STRU;

typedef struct
{
    VOS_UINT32                  u32ChanID;
    VOS_UINT32                  u32SetStat;
    SOCP_DATA_TYPE_ENUM_UIN32   eDataType;
    VOS_UINT32                  ulRsv;
    SOCP_RING_BUF_STRU          sDecDstBuf;
    socp_read_cb                read_cb;
}SOCP_DECDST_CHAN_STRU;


/* 全局状态结构体 */
typedef struct
{
    VOS_BOOL                    bInitFlag;                  /*标识当前SOCP初始化是否完成*/
    VOS_UINT32                  ulRfu;
    VOS_UINT_PTR                uBaseAddr;                  /*SOCP IP的基地址*/
#ifdef SOCP_ACORE
    VOS_SEM                     u32EncDstSemIDChannel[SOCP_REAL_CHAN_ID(SOCP_CODER_DST_BUTT)];     /*编码目的通道自处理任务的信号量*/

    SOCP_ENCDST_CHAN_STRU       sEncDstChan[SOCP_MAX_ENCDST_CHN];   /*编码目的通道参数*/
    SOCP_DECSRC_CHAN_STRU       sDecSrcChan[SOCP_MAX_DECSRC_CHN];   /*解码源通道参数*/
    SOCP_DECDST_CHAN_STRU       sDecDstChan[SOCP_MAX_DECDST_CHN];   /*解码目的通道参数*/
#endif
    SOCP_ENCSRC_CHAN_STRU       sEncSrcChan[SOCP_MAX_ENCSRC_CHN];   /*编码源通道参数*/
}SOCP_GBLSTATE_STRU;

typedef struct
{
#ifdef SOCP_ACORE
    VOS_UINT32                 u32SocpAppEtrIntCnt;        /* 进入APP中断的次数*/
    VOS_UINT32                 u32SocpAppSucIntCnt;        /* 进入APP中断处理成功的次数*/
#endif
#ifdef SOCP_CCORE
    VOS_UINT32                 u32SocpModemEtrIntCnt;      /* 进入Modem中断的次数*/
    VOS_UINT32                 u32SocpModemSucIntCnt;      /* 进入Modem中断处理成功的次数*/
#endif
} SOCP_DEBUG_GBL_STRU;

typedef struct
{
    VOS_UINT32 u32SocpStartEncSrcCnt[SOCP_MAX_ENCSRC_CHN];        /* SOCP启动编码源通道的次数*/
    VOS_UINT32 u32SocpStopEncSrcCnt[SOCP_MAX_ENCSRC_CHN];         /* SOCP关闭编码源通道的次数*/
    VOS_UINT32 u32SocpRegEventEncSrcCnt[SOCP_MAX_ENCSRC_CHN];     /* SOCP注册编码源通道事件的次数*/
    VOS_UINT32 u32SocpGetWBufEncSrcEtrCnt[SOCP_MAX_ENCSRC_CHN];      /* SOCP编码源通道尝试获得写buffer的次数*/
    VOS_UINT32 u32SocpGetWBufEncSrcSucCnt[SOCP_MAX_ENCSRC_CHN];      /* SOCP编码源通道成功获得写buffer的次数*/
    VOS_UINT32 u32SocpWriteDoneEncSrcEtrCnt[SOCP_MAX_ENCSRC_CHN];    /* SOCP编码源通道尝试写回buffer的次数*/
    VOS_UINT32 u32SocpWriteDoneEncSrcSucCnt[SOCP_MAX_ENCSRC_CHN];    /* SOCP编码源通道成功写回buffer的次数*/
    VOS_UINT32 u32SocpWriteDoneEncSrcFailCnt[SOCP_MAX_ENCSRC_CHN];    /* SOCP编码源通道写回buffer失败的次数*/
    VOS_UINT32 u32SocpGetRdBufEncSrcEtrCnt[SOCP_MAX_ENCSRC_CHN];     /* SOCP编码源通道尝试获得Rdbuffer的次数*/
    VOS_UINT32 u32SocpGetRdBufEncSrcSucCnt[SOCP_MAX_ENCSRC_CHN];     /* SOCP编码源通道成功获得Rdbuffer的次数*/
    VOS_UINT32 u32SocpReadRdDoneEncSrcEtrCnt[SOCP_MAX_ENCSRC_CHN];   /* SOCP编码源通道尝试写回Rdbuffer的次数*/
    VOS_UINT32 u32SocpReadRdDoneEncSrcSucCnt[SOCP_MAX_ENCSRC_CHN];   /* SOCP编码源通道成功写回Rdbuffer的次数*/
    VOS_UINT32 u32SocpReadRdDoneEncSrcFailCnt[SOCP_MAX_ENCSRC_CHN];   /* SOCP编码源通道写回Rdbuffer失败的次数*/
    VOS_UINT32 u32SocpEncSrcIsrHeadIntCnt[SOCP_MAX_ENCSRC_CHN];      /* ISR中进入编码源通道包头错误中断次数*/
    VOS_UINT32 u32SocpEncSrcIsrRdIntCnt[SOCP_MAX_ENCSRC_CHN];        /* ISR进入编码源通道Rd 完成中断次数*/
} SOCP_DEBUG_ENCSRC_STRU;

#ifdef SOCP_ACORE
typedef struct
{
    VOS_UINT32 u32SocpRegEventEncDstCnt[SOCP_MAX_ENCDST_CHN];     /* SOCP注册编码目的通道事件的次数*/
    VOS_UINT32 u32SocpRegReadCBEncDstCnt[SOCP_MAX_ENCDST_CHN];    /* SOCP注册编码目的通道读数据回调函数的次数*/
    VOS_UINT32 u32SocpGetReadBufEncDstEtrCnt[SOCP_MAX_ENCDST_CHN];   /* SOCP编码目的通道尝试读数据的次数*/
    VOS_UINT32 u32SocpGetReadBufEncDstSucCnt[SOCP_MAX_ENCDST_CHN];   /* SOCP编码目的通道成功读数据的次数*/
    VOS_UINT32 u32SocpReadDoneEncDstEtrCnt[SOCP_MAX_ENCDST_CHN];     /* SOCP编码目的通道尝试写回目的buffer的次数*/
    VOS_UINT32 u32SocpReadDoneEncDstSucCnt[SOCP_MAX_ENCDST_CHN];     /* SOCP编码目的通道写回目的buffer成功的次数*/
    VOS_UINT32 u32SocpReadDoneEncDstFailCnt[SOCP_MAX_ENCDST_CHN];     /* SOCP编码目的通道写回目的buffer失败的次数*/
    VOS_UINT32 u32SocpReadDoneValidEncDstCnt[SOCP_MAX_ENCDST_CHN];    /* SOCP编码目的通道写回目的buffer size 不等于0 的次数*/
    VOS_UINT32 u32SocpEncDstTskTrfCbCnt[SOCP_MAX_ENCDST_CHN];        /* 任务中回调编码目的通道传输完成中断处理函数完成的次数*/
    VOS_UINT32 u32SocpEncDstTskTrfCbOriCnt[SOCP_MAX_ENCDST_CHN];        /* 任务中回调编码目的通道传输完成中断处理函数的次数*/
    VOS_UINT32 u32SocpEncDstTskOvfCbCnt[SOCP_MAX_ENCDST_CHN];        /* 任务中回调编码目的通道buf 溢出中断处理函数完成的次数*/
    VOS_UINT32 u32SocpEncDstTskOvfCbOriCnt[SOCP_MAX_ENCDST_CHN];     /* 任务中回调编码目的通道buf 溢出中断处理函数次数*/
    VOS_UINT32 u32SocpEncDstIsrTrfIntCnt[SOCP_MAX_ENCDST_CHN];        /* ISR中进入编码目的通道传输完成中断次数*/
    VOS_UINT32 u32SocpEncDstIsrTrfIntINvalidChannelCnt[SOCP_MAX_ENCDST_CHN];  /* ISR中进入编码目的通道传输完成中断，但通道异常，统计次数*/
    VOS_UINT32 u32SocpEncDstTskThresholdOvfCbCnt[SOCP_MAX_ENCDST_CHN];        /* 任务中回调编码目的通道buf阈值溢出中断处理函数完成的次数*/
    VOS_UINT32 u32SocpEncDstTskThresholdOvfCbOriCnt[SOCP_MAX_ENCDST_CHN];     /* 任务中回调编码目的通道buf阈值溢出中断处理函数次数*/
    VOS_UINT32 u32SocpEncDstIsrThresholdOvfIntCnt[SOCP_MAX_ENCDST_CHN];       /* ISR中进入编码目的通道buf阈值溢出中断次数*/
    VOS_UINT32 u32SocpEncDstLogSaveOriCnt[SOCP_MAX_ENCDST_CHN];      /* SOCP编码目的通道响应AT^LOGSAVE命令主动保存log次数 */
    VOS_UINT32 u32SocpEncDstLogSaveCnt[SOCP_MAX_ENCDST_CHN];         /* SOCP编码目的通道响应AT^LOGSAVE命令主动保存log完成次数 */
} SOCP_DEBUG_ENCDST_STRU;

typedef struct
{
    VOS_UINT32 u32SocpStartDecSrcCnt[SOCP_MAX_DECSRC_CHN];        /* SOCP启动解码源通道的次数*/
    VOS_UINT32 u32SocpStopDecSrcCnt[SOCP_MAX_DECSRC_CHN];         /* SOCP关闭解码源通道的次数*/
    VOS_UINT32 u32SocpRegEventDecSrcCnt[SOCP_MAX_DECSRC_CHN];     /* SOCP注册解码源通道事件的次数*/
    VOS_UINT32 u32SocpGetWBufDecSrcEtrCnt[SOCP_MAX_DECSRC_CHN];      /* SOCP解码源通道尝试获得写buffer的次数*/
    VOS_UINT32 u32SocpGetWBufDecSrcSucCnt[SOCP_MAX_DECSRC_CHN];      /* SOCP解码源通道获得写buffer成功的次数*/
    VOS_UINT32 u32SocpWriteDoneDecSrcEtrCnt[SOCP_MAX_DECSRC_CHN];    /* SOCP解码源通道尝试写回buffer的次数*/
    VOS_UINT32 u32SocpWriteDoneDecSrcSucCnt[SOCP_MAX_DECSRC_CHN];    /* SOCP解码源通道写回buffer成功的次数*/
    VOS_UINT32 u32SocpWriteDoneDecSrcFailCnt[SOCP_MAX_DECSRC_CHN];    /* SOCP解码源通道写回buffer失败的次数*/
    VOS_UINT32 u32SocpDecSrcIsrErrIntCnt[SOCP_MAX_DECSRC_CHN];        /* ISR中进入解码源通道错误中断次数*/
} SOCP_DEBUG_DECSRC_STRU;
#endif

typedef struct
{
    VOS_UINT32 u32SocpRegEventDecDstCnt[SOCP_MAX_DECDST_CHN];     /* SOCP注册解码目的通道事件的次数*/
    VOS_UINT32 u32SocpRegReadCBDecDstCnt[SOCP_MAX_DECDST_CHN];    /* SOCP注册解码目的通道读数据回调函数的次数*/
    VOS_UINT32 u32SocpGetReadBufDecDstEtrCnt[SOCP_MAX_DECDST_CHN];   /* SOCP解码目的通道尝试读数据的次数*/
    VOS_UINT32 u32SocpGetReadBufDecDstSucCnt[SOCP_MAX_DECDST_CHN];   /* SOCP解码目的通道读数据成功的次数*/
    VOS_UINT32 u32SocpReadDoneDecDstEtrCnt[SOCP_MAX_DECDST_CHN];     /* SOCP解码目的通道尝试写回目的buffer的次数*/
    VOS_UINT32 u32SocpReadDoneDecDstSucCnt[SOCP_MAX_DECDST_CHN];     /* SOCP解码目的通道写回目的buffer成功的次数*/
    VOS_UINT32 u32SocpReadDoneDecDstFailCnt[SOCP_MAX_DECDST_CHN];     /* SOCP解码目的通道写回目的buffer失败的次数*/
    VOS_UINT32 u32SocpReadDoneZeroDecDstCnt[SOCP_MAX_DECDST_CHN];   /* SOCP解码目的通道写回目的buffer size 等于0 的次数*/
    VOS_UINT32 u32SocpReadDoneValidDecDstCnt[SOCP_MAX_DECDST_CHN];   /* SOCP解码目的通道写回目的buffer size 不等于0 的次数*/
    VOS_UINT32 u32SocpDecDstIsrTrfIntCnt[SOCP_MAX_DECDST_CHN];        /* ISR中进入解码目的通道传输完成中断次数*/
    VOS_UINT32 u32SocpDecDstIsrOvfIntCnt[SOCP_MAX_DECDST_CHN];        /* ISR中进入解码目的通道buf 溢出中断次数*/
} SOCP_DEBUG_DECDST_STRU;

typedef struct
{
    VOS_UINT32  ulStartTimeNum;
    VOS_UINT32  ulEncDstRunNum;
    VOS_UINT32  ulFlushBufferNum;
    VOS_UINT32  ulFlushTimeOutNum;
}SOCP_DEBUG_SDLOG_STRU;

#ifdef SOCP_ACORE
typedef struct
{
    VOS_UINT32  ulTimerNum;
    VOS_UINT32  ulTimerSlice;
    VOS_UINT32  ulFlushNum;
    VOS_UINT32  ulFlushSlice;
    VOS_UINT32  ulIntNum;
    VOS_UINT32  ulIntSlice;
    VOS_UINT32  ulTaskNum;
    VOS_UINT32  ulTaskSlice;
    VOS_UINT32  ulTaskErrNum;
    VOS_UINT32  ulTaskErrSlice;
}SOCP_ENCDST_SEM_STRU;
#endif

/* 调试信息*/
typedef struct
{
    SOCP_DEBUG_GBL_STRU         sSocpDebugGBl;
    SOCP_DEBUG_ENCSRC_STRU      sSocpDebugEncSrc;
#ifdef SOCP_ACORE
    SOCP_DEBUG_ENCDST_STRU      sSocpDebugEncDst;
    SOCP_DEBUG_DECSRC_STRU      sSocpDebugDecSrc;
    SOCP_DEBUG_DECDST_STRU      sSocpDebugDecDst;
    SOCP_DEBUG_SDLOG_STRU       stSocpSDLogInfo[SOCP_MAX_ENCDST_CHN];
    SOCP_ENCDST_SEM_STRU        stSocpEncDstSemInfo[SOCP_MAX_ENCDST_CHN];
#endif
}SOCP_DEBUGINFO_STRU;

typedef struct
{
    INNER_LOG_DATA_STRU         stSocpInfo;
    INNER_LOG_DATA_STRU         stSocpWarning;
    INNER_LOG_DATA_STRU         stSocpErr;
}SOCP_LOG_INFO_STRU;

/**************************************************************************
  寄存器位域定义
**************************************************************************/

/*
* Bit masks for registers: ENCSTAT,DECSTAT, channel state
*/
#define SOCP_CHN_BUSY                   ( (VOS_INT32) 1)   /*通道忙*/
#define SOCP_CHN_IDLE                   ( (VOS_INT32) 0)   /*通道闲*/

/*
* 通道使能宏定义，同时适用于编码源通道和解码源通道
*/
#define SOCP_CHN_ENABLE                 ( (VOS_INT32) 1)   /*通道打开*/
#define SOCP_CHN_DISABLE                ( (VOS_INT32) 0)   /*通道关闭*/

/*
* 通道是否已经设置
*/
#define SOCP_CHN_SET                    ( (VOS_INT32) 1)     /*通道已经配置*/
#define SOCP_CHN_UNSET                  ( (VOS_INT32) 0)     /*通道没有配置*/
/*
* 编码旁路设置使能位
*/
#define SOCP_ENCSRC_BYPASS_ENABLE       ( (VOS_INT32) 1)     /*通道旁路使能*/
#define SOCP_ENCSRC_BYPASS_DISABLE      ( (VOS_INT32) 0)     /*通道旁路没有使能*/


/*
* 编码旁路设置使能位
*/
#define SOCP_DECSRC_DEBUG_ENBALE        ( (VOS_INT32) 1)     /*通道DEBUG 使能*/
#define SOCP_DECSRC_DEBUG_DISBALE       ( (VOS_INT32) 0)     /*通道DEBUG 没有使能*/

#define SOCP_CORE0_DEC_OUTOVFINT_MASK   ( (VOS_INT32)(1))          /* 解码core0 目的BUFFER溢出中断 */
#define SOCP_DEC_INERRINT_MASK          ( (VOS_INT32)(1<<1))       /* 编码输入检测错误中断 */
#define SOCP_CORE0_DEC_TFRINT_MASK      ( (VOS_INT32)(1<<2))       /* 编码core0传输完成中断 */
#define SOCP_CORE1_DEC_TFRINT_MASK      ( (VOS_INT32)(1<<3))       /* 解码core1 传输完成中断 */
#define SOCP_CORE1_DEC_OUTOVFINT_MASK   ( (VOS_INT32)(1<<4))       /* 编码core1 目的BUFFER溢出中断*/
#define SOCP_MODEM_ENC_RDINT_MASK       ( (VOS_INT32)(1<<10))      /* 编码RDBUFFER完成中断 */
#define SOCP_APP_ENC_RDINT_MASK         ( (VOS_INT32)(1<<11))      /* 编码MODEMCPU包头检测错误中断 */
#define SOCP_APP_ENC_OUTOVFINT_MASK     ( (VOS_INT32)(1<<12))      /* 编码RDBUFFER完成中断 */
#define SOCP_MODEM_ENC_FLAGINT_MASK     ( (VOS_INT32)(1<<13))      /* 编码MODEMCPU包头检测错误中断 */
#define SOCP_APP_ENC_FLAGINT_MASK       ( (VOS_INT32)(1<<14))      /* 编码RDBUFFER完成中断 */
#define SOCP_APP_ENC_TFRINT_MASK        ( (VOS_INT32)(1<<15))      /* 编码MODEMCPU包头检测错误中断 */
#define SOCP_DEC_SRCINT_NUM             (6)              /* 编码MODEMCPU包头检测错误中断 */


#define SOCP_TRANS_TIMEOUT_DEFAULT      (0x17)      /*dec:23 */
#define SOCP_OVERFLOW_TIMEOUT_DEFAULT   (0x5969)    /*dec:22889 */
#define SOCP_DEC_PKTLGTH_MAX            (0x04)      /*dec:4090 */
#define SOCP_DEC_PKTLGTH_MIN            (0x06)      /*dec:6 */
#define SOCP_TIMEOUT_MAX                (0xffff)
#define SOCP_DEC_MAXPKT_MAX             (0xfff)
#define SOCP_DEC_MINPKT_MAX             (0x1f)
#define SOCP_ENC_SRC_BUFLGTH_MAX        (0x7ffffff)
#define SOCP_ENC_SRC_RDLGTH_MAX         (0xffff)
#define SOCP_ENC_DST_BUFLGTH_MAX        (0x1fffff)
#define SOCP_ENC_DST_TH_MAX             (0x7ff)
#define SOCP_ENC_DST_THRH_MIN           (256)
#define SOCP_DEC_SRC_BUFLGTH_MAX        (0xffff)
#define SOCP_DEC_SRC_RDLGTH_MAX         (0xffff)
#define SOCP_DEC_DST_BUFLGTH_MAX        (0xffff)
#define SOCP_DEC_DST_TH_MAX             (0xff)

/* 编码目的buffer上溢中断状态寄存器16-22位指示阈值溢出中断，0-6位指示上溢中断 */
#define SOCP_ENC_DST_BUFF_THRESHOLD_OVF_MASK    (0x007f0000)
#define SOCP_ENC_DST_BUFF_OVF_MASK              (0x0000007f)
/* 阈值中断寄存器起始位 */
#define SOCP_ENC_DST_BUFF_THRESHOLD_OVF_BEGIN   (16)


#define SOCP_RD_TIMEOUT                 ((VOS_INT32) 100)     /*待定*/
#define SOCP_RESET_TIME                 (1000)
#define SOCP_GBLRESET_TIME              (100)


/* 检测是否初始化的宏定义 */
#define SOCP_CHECK_INIT() \
        if (!g_strSocpStat.bInitFlag)\
        {\
            SOCP_LOG_ERR("The module has not been initialized!\n", 0, 0);\
            return BSP_ERR_SOCP_NOT_INIT;\
        }\

/* 检测参数是否有效 */
#define SOCP_CHECK_PARA(para) \
        if (VOS_NULL_PTR == para)\
        {\
            SOCP_LOG_ERR("the parameter is NULL!", 0, 0);\
            return BSP_ERR_SOCP_NULL;\
        }\

#define SOCP_CHECK_BUF_ADDR(start, end) \
        if (start >= end)\
        {\
            SOCP_LOG_ERR("the buff is valid!", 0, 0);\
            return BSP_ERR_SOCP_INVALID_PARA;\
        }\

#define SOCP_CHECK_CHAN_TYPE(para, type) \
        if (type != para)\
        {\
            SOCP_LOG_ERR("the Chan's Type is Valid!", 0, 0);\
            return BSP_ERR_SOCP_INVALID_CHAN;\
        }\

#define SOCP_CHECK_CHAN_ID(para, id) \
        if (para >= id)\
        {\
            SOCP_LOG_ERR("the Chan Id is Valid!", 0, 0);\
            return BSP_ERR_SOCP_INVALID_CHAN;\
        }\

/* 检测是否8字节对齐 */
#define SOCP_CHECK_8BYTESALIGN(para) \
        if (0 != (para%8))\
        {\
            SOCP_LOG_ERR("the parameter is not 8 bytes aligned!", 0, 0);\
            return BSP_ERR_SOCP_NOT_8BYTESALIGN;\
        }\

#define SOCP_CHECK_ENCSRC_CHNMODE(para) \
        if (para >= SOCP_ENCSRC_CHNMODE_BUTT)\
        {\
            SOCP_LOG_ERR("encoder src channele mode is, which is valid!", para, 0);\
            return BSP_ERR_SOCP_INVALID_PARA;\
        }\

#define SOCP_CHECK_DECSRC_CHNMODE(para) \
        if (para >= SOCP_DECSRC_CHNMODE_BUTT)\
        {\
            SOCP_LOG_ERR("decoder dst channele mode is, which is valid!", para, 0);\
            return BSP_ERR_SOCP_INVALID_PARA;\
        }\

#define SOCP_CHECK_CHAN_PRIORITY(para) \
        if (para >= SOCP_CHAN_PRIORITY_BUTT)\
        {\
            SOCP_LOG_ERR("encoder src channele priority is, which is valid!", para, 0);\
            return BSP_ERR_SOCP_INVALID_PARA;\
        }\

#define SOCP_CHECK_DATA_TYPE(para) \
        if (para >= SOCP_DATA_TYPE_BUTT)\
        {\
            SOCP_LOG_ERR("channel data type is, which is valid!", para, 0);\
            return BSP_ERR_SOCP_INVALID_PARA;\
        }\

#define SOCP_CHECK_ENCSRC_SET(id) \
        if(SOCP_CHN_SET != g_strSocpStat.sEncSrcChan[id].u32SetStat)\
        {\
            SOCP_LOG_ERR("encoder src not allocated!\n", id, 0);\
            return BSP_ERR_SOCP_INVALID_CHAN;\
        }\

#define SOCP_CHECK_ENCDST_SET(id) \
        if(SOCP_CHN_SET!= g_strSocpStat.sEncDstChan[id].u32SetStat)\
        {\
            SOCP_LOG_ERR("encoder dst set failed!\n", id, 0);\
            return BSP_ERR_SOCP_INVALID_CHAN;\
        }\

#define SOCP_CHECK_DECDST_SET(id) \
        if(SOCP_CHN_SET!= g_strSocpStat.sDecDstChan[id].u32SetStat)\
        {\
            SOCP_LOG_ERR("decoder dst[%d] alloc failed!\n", id, 0);\
            return BSP_ERR_SOCP_INVALID_CHAN;\
        }\

#define SOCP_CHECK_DECSRC_SET(id) \
        if(SOCP_CHN_SET!= g_strSocpStat.sDecSrcChan[id].u32SetStat)\
        {\
            SOCP_LOG_ERR("decoder src set failed!\n", id, 0);\
            return BSP_ERR_SOCP_INVALID_CHAN;\
        }\

#define SOCP_CHECK_DATA_TYPE_EN(param) \
        if (param >= SOCP_DATA_TYPE_EN_BUTT)\
        {\
            SOCP_LOG_ERR("the data type en is valid!\n", param, 0); \
            return BSP_ERR_SOCP_INVALID_PARA; \
        } \

#define SOCP_CHECK_ENC_DEBUG_EN(param) \
        if (param >= SOCP_ENC_DEBUG_EN_BUTT)\
        {\
            SOCP_LOG_ERR("the enc src debug en is valid", param, 0); \
            return BSP_ERR_SOCP_INVALID_PARA; \
        } \

/*
 * 寄存器操作宏定义
 */
#define SOCP_REG(base, reg)      (*(volatile VOS_UINT32 *)((base) + (VOS_UINT_PTR)(reg)))

/* 将var中[pos, pos + bits-1]比特清零,  pos从0开始编号
   e.g BDOM_CLR_BITS(var, 4, 2) 表示将Bit5~4清零 */
#define BSP_REG_CLRBITS(base, reg, pos, bits) (SOCP_REG(base, reg) &= ~((((VOS_UINT32)1 << (bits)) - 1) << (pos)))

/* 将var中[pos, pos + bits-1]比特设置为val,  pos从0开始编号
   e.g BDOM_SET_BITS(var, 4, 2, 2) 表示将Bit5~4设置为b'10 */
#define BSP_REG_SETBITS(base, reg, pos, bits, val) (SOCP_REG(base, reg) = (SOCP_REG(base, reg) & (~((((VOS_UINT32)1 << (bits)) - 1) << (pos)))) \
                                                                         | ((VOS_UINT32)((val) & (((VOS_UINT32)1 << (bits)) - 1)) << (pos)))

/* 获取var中[pos, pos + bits-1]比特值,  pos从0开始编号 */
#define BSP_REG_GETBITS(base, reg, pos, bits) ((SOCP_REG(base, reg) >> (pos)) & (((VOS_UINT32)1 << (bits)) - 1))

#define COMM_DRV_REG_READ(base, reg, result) \
    ((result) = SOCP_REG(base, reg))

#define COMM_DRV_REG_WRITE(base, reg, data) \
    (SOCP_REG(base, reg) = (data))

#define BSP_REG_WRITEBITS(base, reg, data) \
            (SOCP_REG(base, reg) |= (data))

#define BSP_REG_MODIFY(base, reg, clrmsk, setmsk) \
    (SOCP_REG(base, reg) = ((SOCP_REG(base, reg) & ~clrmsk) | setmsk))

#define SOCP_REG_WRITE(reg, data)               COMM_DRV_REG_WRITE(g_strSocpStat.uBaseAddr,   reg, data)
#define SOCP_REG_READ(reg, result)              COMM_DRV_REG_READ(g_strSocpStat.uBaseAddr,    reg, result)
#define SOCP_REG_SETBITS(reg, pos, bits, val)   BSP_REG_SETBITS(g_strSocpStat.uBaseAddr, reg, pos, bits, val)
#define SOCP_REG_GETBITS(reg, pos, bits)        BSP_REG_GETBITS(g_strSocpStat.uBaseAddr, reg, pos, bits)

#define SOCP_ENC_DEST_LOG_DELAY_ON              (0x5A5A5A5A)
typedef struct SOCP_ENC_DST_BUF_LOG_CFG
{
    VOS_VOID*      pVirBuffer;      /* SOCP编码目的通道数据虚拟BUFFER、在32位系统上是4字节，在64位系统上是8字节 */
    VOS_UINT32     ulPhyBufferAddr; /* SOCP编码目的通道数据物理BUFFER地址，在64位和32位系统上物理地址都是4字节 */
    VOS_UINT32     BufferSize;      /* SOCP编码目的通道数据BUFFER大小 */
    VOS_UINT32     overTime;        /* NVE中设置的超时时间 */
    VOS_UINT32     logOnFlag;       /* LOG2.0功能启动标志 */
    VOS_UINT32     ulCurTimeout;    /* SOCP编码目的通道数据传输超时时间 */
} SOCP_ENC_DST_BUF_LOG_CFG_STRU;


/**************************************************************************
  函数声明
**************************************************************************/

extern SOCP_GBLSTATE_STRU       g_strSocpStat;          /* 记录当前全局状态和通道配置信息 */

extern SOCP_DEBUGINFO_STRU      g_stSocpDebugInfo;      /* 记录在SOCP运行状态中的过程信息 */

extern SOCP_LOG_INFO_STRU       g_stSocpLog;            /* 记录在SOCP运行状态中的log信息 */

extern VOS_SPINLOCK             g_stSocpSpinLock;       /* 自旋锁，用来作SOCP临界资源保护 */


#ifdef SOCP_ACORE
extern VOS_VOID SocpHandlerEncDst(VOS_VOID);

extern VOS_UINT32 SocpAppInthandler(VOS_VOID);
#else
extern VOS_VOID SocpModemInthandler(VOS_VOID);
#endif

extern VOS_VOID SOCP_LFixChan_Init(VOS_VOID);

extern VOS_VOID SOCP_EnableLFixChan(VOS_VOID);


#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
#define SOCP_LOG_INFO(pucStr, ulP1, ulP2)\
    OM_Inner_Log(pucStr, ulP1, ulP2, &g_stSocpLog.stSocpInfo, VOS_FILE_ID, __LINE__)

#define SOCP_LOG_WARNING(pucStr, ulP1, ulP2)\
    OM_Inner_Log(pucStr, ulP1, ulP2, &g_stSocpLog.stSocpWarning, VOS_FILE_ID, __LINE__)

#define SOCP_LOG_ERR(pucStr, ulP1, ulP2)\
    OM_Inner_Log(pucStr, ulP1, ulP2, &g_stSocpLog.stSocpErr, VOS_FILE_ID, __LINE__)
#else
#define SOCP_LOG_INFO(pucStr, ulP1, ulP2)\
    vos_printf("SOCP_LOG_INFO %s, %d, %d", pucStr, ulP1, ulP2)

#define SOCP_LOG_WARNING(pucStr, ulP1, ulP2)\
    vos_printf("SOCP_LOG_WARNING %s, %d, %d", pucStr, ulP1, ulP2)

#define SOCP_LOG_ERR(pucStr, ulP1, ulP2)\
    vos_printf("SOCP_LOG_ERR %s, %d, %d", pucStr, ulP1, ulP2)

#endif


#if(VOS_OS_VER == VOS_LINUX)

#define SOCP_FLUSH_CACHE(ptr, size)     (0)/*__dma_single_cpu_to_dev(ptr, size, 1)*/

#define SOCP_INVALID_CACHE(ptr, size)   (0)/*__dma_single_dev_to_cpu(ptr, size, 2)*/

#elif(VOS_OS_VER == VOS_VXWORKS)
#define SOCP_FLUSH_CACHE(ptr, size) \
    VOS_FlushCpuCache((void  *)(((VOS_UINT32)(ptr)) & (~0x1F)), (unsigned int)((((size) + (((VOS_UINT32)(ptr)) & 0x1F) + 0x1F)>>0x5)<<0x5))
#else
#define SOCP_FLUSH_CACHE(ptr, size)     (0)

#endif

#if(VOS_OS_VER == VOS_WIN32)    /* for PC Stub */
extern VOS_VOID Stub_SendSocpData(VOS_UINT32 ulSrcChID,VOS_UINT8 *pucData,VOS_UINT32 ulDataLen);
#endif


#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#else
#include "mdrv_socp.h"
#endif

#endif


