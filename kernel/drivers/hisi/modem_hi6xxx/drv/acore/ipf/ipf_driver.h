
#ifndef __IPF_DRIVER_H__
#define __IPF_DRIVER_H__

/**************************************************************************
  头文件包含
**************************************************************************/
#include "BSP.h"
#include "MemoryMap.h"
#include "soc_irqs.h"
#include "mdrv_ipf.h"
#include "soc_baseaddr_interface.h"
#include "soc_ipf_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**************************************************************************
  宏定义
**************************************************************************/
#define __BSP_IPF_DEBUG__

#define IPF_AXI_MEM_ADDR                (MEMORY_AXI_IPF_ADDR)
#define IPF_AXI_MEM_SIZE                (MEMORY_AXI_IPF_SIZE)

/* IPF地址配置信息 */

extern unsigned long ifp_ulbd_mem_addr;
#define IPF_ULBD_MEM_ADDR               (ifp_ulbd_mem_addr)
#define IPF_ULBD_MEM_SIZE               (IPF_ULBD_DESC_SIZE * sizeof(IPF_BD_DESC_S))

#define IPF_ULRD_MEM_ADDR               (IPF_ULBD_MEM_ADDR + IPF_ULBD_MEM_SIZE)
#define IPF_ULRD_MEM_SIZE               (IPF_ULRD_DESC_SIZE * sizeof(IPF_RD_DESC_S))

#define IPF_ULAD0_MEM_ADDR              (IPF_ULRD_MEM_ADDR + IPF_ULRD_MEM_SIZE)
#define IPF_ULAD0_MEM_SIZE              (IPF_ULAD0_DESC_SIZE * sizeof(IPF_AD_DESC_S))

#define IPF_ULAD1_MEM_ADDR              (IPF_ULAD0_MEM_ADDR + IPF_ULAD0_MEM_SIZE)
#define IPF_ULAD1_MEM_SIZE              (IPF_ULAD1_DESC_SIZE * sizeof(IPF_AD_DESC_S))

#define IPF_DLBD_MEM_ADDR               (IPF_ULAD1_MEM_ADDR + IPF_ULAD1_MEM_SIZE)
#define IPF_DLBD_MEM_SIZE               (IPF_DLBD_DESC_SIZE * sizeof(IPF_BD_DESC_S))

#define IPF_DLRD_MEM_ADDR               (IPF_DLBD_MEM_ADDR + IPF_DLBD_MEM_SIZE)
#define IPF_DLRD_MEM_SIZE               (IPF_DLRD_DESC_SIZE * sizeof(IPF_RD_DESC_S))

#define IPF_DLAD0_MEM_ADDR              (IPF_DLRD_MEM_ADDR + IPF_DLRD_MEM_SIZE)
#define IPF_DLAD0_MEM_SIZE              (IPF_DLAD0_DESC_SIZE * sizeof(IPF_AD_DESC_S))

#define IPF_DLAD1_MEM_ADDR              (IPF_DLAD0_MEM_ADDR + IPF_DLAD0_MEM_SIZE)
#define IPF_DLAD1_MEM_SIZE              (IPF_DLAD1_DESC_SIZE * sizeof(IPF_AD_DESC_S))

#define IPF_DLCD_MEM_ADDR               (IPF_DLAD1_MEM_ADDR  + IPF_DLAD1_MEM_SIZE)
#define IPF_DLCD_MEM_SIZE               (IPF_DLCD_DESC_SIZE * sizeof(IPF_CD_DESC_S))

#define IPF_ULBD_IDLENUM_ADDR           (IPF_DLCD_MEM_ADDR + IPF_DLCD_MEM_SIZE)
#define IPF_ULBD_IDLENUM_SIZE           (16)

#define IPF_DEBUG_INFO_ADDR             (IPF_ULBD_IDLENUM_ADDR + IPF_ULBD_IDLENUM_SIZE)
#define IPF_DEBUG_INFO_SIZE             (sizeof(IPF_DEBUG_INFO_S))

#define IPF_DEBUG_DLCD_ADDR             (IPF_DEBUG_INFO_ADDR + IPF_DEBUG_INFO_SIZE)
#define IPF_DEBUG_DLCD_SIZE             (IPF_DLCD_DESC_SIZE * sizeof(IPF_CD_DESC_S))

/* 保存下行CD读写指针 */
#define IPF_DLCD_PTR_ADDR               (IPF_DEBUG_DLCD_ADDR + IPF_DEBUG_DLCD_SIZE)
#define IPF_DLCD_PTR_SIZE               (8)

#define IPF_MEM_USED_SIZE               (IPF_DLCD_PTR_ADDR + IPF_DLCD_PTR_SIZE - IPF_ULBD_MEM_ADDR)

/* IPF属性配置信息 */
#define IPF_REG_NUM                         (172)         /* IPF寄存器个数 */
#define IPF_BF_NUM                          (FEATURE_IPF_BF_NUM)    /* 基本过滤器数目 */
#define IPF_TOTAL_FILTER_NUM                (256)         /* 过滤器总数目先使用一半 */
#define IPF_EXFLITER_NUM                    (IPF_TOTAL_FILTER_NUM-IPF_BF_NUM) /* 扩展过滤器数目 */
#define IPF_TAIL_INDEX                      (511)         /* 标识最后一个filter */
#define IPF_FILTERCHAIN_HEAD_INDEX          (0)           /* 过滤器链组头所在链的位置 */
#define IPF_FILTERTYPE_MASK                 (0xFFFF)      /* 区分过滤器是基本还是扩展滤波器 */
#define IPF_TIME_OUT_CFG                    (65)          /* RD超时时间0.1ms时钟频率166MHZ下 */
#define IPF_ULADQ_PLEN_TH                   (404)         /* 上行包长阈值 */
#define IPF_DLADQ_PLEN_TH                   (448)         /* 下行包长阈值 */
#define IPF_RESUME_COUNT                    (200)         /* 过滤规则恢复超时,单位10us,共2ms */
#define IPF_OBLIGATE_AD_NUM                 (3)           /* ADQ队列预留AD个数 */
#define IPF_OBLIGATE_BD_NUM                 (3)           /* ADQ队列预留BD个数 */
#define IPF_CH_IDLE                         (0x14)        /* 通道IDLE状态值 */
#define IPF_IDLE_TIMEOUT_NUM                (30)          /* 软件等待超时值,单位10ms,确保该时间内IPF处理完 */
#define IPF_ADQ_BUF_EPT_MASK                (0x00000004)  /* ADQ BUFF空预读状态mask */
#define IPF_ADQ_BUF_FULL_MASK               (0x00000008)  /* ADQ BUFF满预读状态mask */
#define IPF_SCHEDULE_STRATEGY               (0)           /* 上下行通道调度策略选择。
                                                            0：表示上下行通道根据BDQ是否为空进行调度；
                                                            1：表示上下行通道根据ADQ、BDQ是否为空，RDQ是否为满进行调度。*/

/* IPF中断上报、屏蔽、状态寄存器位定义,上报、屏蔽、状态三者一一对应 */
#define IPF_UL_RPT_INT                     (1 << 0)    /* 上行结果上报 */
#define IPF_UL_TIMEOUT_INT                 (1 << 1)    /* 上行结果上报超时 */
#define IPF_UL_RDQ_DOWN_OVERFLOW_INT       (1 << 6)    /* 上行RDQ下溢 */
#define IPF_UL_BDQ_UP_OVERFLOW_INT         (1 << 7)    /* 上行BDQ上溢 */
#define IPF_UL_RDQ_FULL_INT                (1 << 8)    /* 上行RDQ满 */
#define IPF_UL_BDQ_EMPTY_INT               (1 << 9)    /* 上行通道BDQ空 */
#define IPF_UL_ADQ0_EMPTY_INT              (1 << 10)   /* 上行通道ADQ0空 */
#define IPF_UL_ADQ1_EMPTY_INT              (1 << 11)   /* 上行通道ADQ1空 */

#define IPF_DL_RPT_INT                     (1 << 16)   /* 下行结果上报 */
#define IPF_DL_TIMEOUT_INT                 (1 << 17)   /* 下行结果上报超时 */
#define IPF_DL_RDQ_DOWN_OVERFLOW_INT       (1 << 22)   /* 下行RDQ下溢 */
#define IPF_DL_BDQ_UP_OVERFLOW_INT         (1 << 23)   /* 下行BDQ上溢 */
#define IPF_DL_RDQ_FULL_INT                (1 << 24)   /* 下行RDQ满 */
#define IPF_DL_BDQ_EMPTY_INT               (1 << 25)   /* 下行通道BDQ空 */
#define IPF_DL_ADQ0_EMPTY_INT              (1 << 26)   /* 下行通道ADQ0空 */
#define IPF_DL_ADQ1_EMPTY_INT              (1 << 27)   /* 下行通道ADQ1空 */
/* INTO中断上报情况: */
#define IPF_INT_OPEN0                       ( IPF_UL_RPT_INT \
                                            | IPF_UL_TIMEOUT_INT \
                                            | IPF_UL_ADQ0_EMPTY_INT \
                                            | IPF_UL_ADQ1_EMPTY_INT)
/* INT1中断上报情况: */
#define IPF_INT_OPEN1                       ( IPF_DL_RPT_INT \
                                            | IPF_DL_TIMEOUT_INT \
                                            | IPF_DL_ADQ0_EMPTY_INT \
                                            | IPF_DL_ADQ1_EMPTY_INT)
#define IPF_PRINT_INFO(fmt, arg...)     (printk(KERN_INFO "<%s> "fmt, __FUNCTION__, ##arg))
#define IPF_PRINT_ERROR(fmt, arg...)    (printk(KERN_ERR "<%s> "fmt, __FUNCTION__, ##arg))
extern unsigned long ipf_regs_base_addr;
#define IPF_REGBASE_ADR                 (ipf_regs_base_addr)
#define INT_LVL_IPF                     IRQ_IPF1
#define INT_VEC_IPF                     IVEC_TO_INUM(INT_LVL_IPF)

#if 0
#define ipf_isb()
#define ipf_dsb()

#define ipf_mb()
#define ipf_rmb()
#define ipf_wmb()

#define IPF_REG_WRITE(uwAddr,uwValue)   (*((volatile unsigned int *)(uwAddr)) = uwValue)
#define IPF_REG_READ(uwAddr,uwValue)    (uwValue = *((volatile unsigned int *)(uwAddr)))
#else
#ifndef CONFIG_ARM64
#define ipf_isb()		asm volatile("isb" : : : "memory")
#define ipf_dsb()		asm volatile("dsb" : : : "memory")

#define ipf_mb()		ipf_dsb()
#define ipf_rmb()		asm volatile("dsb" : : : "memory")
#define ipf_wmb()		asm volatile("dsb" : : : "memory")
#else
#define ipf_isb()		asm volatile("isb" : : : "memory")
#define ipf_dsb()		asm volatile("dsb sy" : : : "memory")

#define ipf_mb()		ipf_dsb()
#define ipf_rmb()		asm volatile("dsb ld" : : : "memory")
#define ipf_wmb()		asm volatile("dsb st" : : : "memory")
#endif

#define IPF_REG_WRITE(uwAddr,uwValue)  \
do{\
    ipf_wmb();\
    (*(volatile unsigned int *) (uwAddr)) = (uwValue);\
    ipf_wmb();\
}while(0)

#define IPF_REG_READ(uwAddr,uwValue)    \
do{\
    (uwValue) = (*((volatile unsigned int *)(uwAddr)));\
    ipf_rmb();\
}while(0)
#endif

/**************************************************************************
  枚举定义
**************************************************************************/

/**************************************************************************
  STRUCT定义
**************************************************************************/
typedef struct tagIPF_UL_S
{
    IPF_BD_DESC_S* pstIpfBDQ;
    IPF_RD_DESC_S* pstIpfRDQ;
    IPF_AD_DESC_S* pstIpfADQ0;
    IPF_AD_DESC_S* pstIpfADQ1;
    unsigned int* pu32IdleBd; /* 记录上一次获取的空闲BD 数 */
}IPF_UL_S;

typedef struct tagIPF_DL_S
{
    IPF_BD_DESC_S* pstIpfBDQ;
    IPF_RD_DESC_S* pstIpfRDQ;
    IPF_AD_DESC_S* pstIpfADQ0;
    IPF_AD_DESC_S* pstIpfADQ1;
    IPF_CD_DESC_S* pstIpfCDQ;
    IPF_CD_DESC_S* pstIpfDebugCDQ;
    BSP_IPF_WakeupDlCb pFnDlIntCb; /* 中断中唤醒的PS任务 */
    BSP_IPF_AdqEmptyDlCb pAdqEmptyDlCb;
    unsigned int* u32IpfCdRptr; /* 当前可以读出的位置 */
    unsigned int* u32IpfCdWptr;
}IPF_DL_S;

typedef struct tagIPF_ID_S
{
    unsigned int u32PsID;
    unsigned int u32FilterID;
    struct tagIPF_ID_S* pstNext;
}IPF_ID_S;

typedef struct tagIPF_FILTER_INFO_S
{
    IPF_ID_S* pstUseList;
    unsigned int u32FilterNum;
}IPF_FILTER_INFO_S;

typedef struct tagIPF_DEBUG_INFO_S
{
    unsigned int u32UlResultTime;        /* 上行最后一次上报结果中断时间 */
    unsigned int u32UlGetRdTime;         /* 上行最后一次获取处理结果时间 */
    unsigned int u32DlResultTime;        /* 下行最后一次上报结果中断时间 */
    unsigned int u32DlGetRdTime;         /* 下行最后一次获取处理结果时间 */
    unsigned int u32UlResultCnt;         /* 上行上报结果中断计数 */
    unsigned int u32UlResultTimeoutCnt;  /* 上行上报结果超时中断计数 */
    unsigned int u32DlResultCnt;         /* 下行上报结果中断计数 */
    unsigned int u32DlResultTimeoutCnt;  /* 下行上报结果超时中断计数 */

    unsigned int u32UlBdqEmptyCnt;       /* 上行BDQ空计数 */
    unsigned int u32UlRdqFullCnt;        /* 上行RDQ满计数 */
    unsigned int u32DlBdqEmptyCnt;       /* 下行BDQ空计数 */
    unsigned int u32DlRdqFullCnt;        /* 下行RDQ满计数 */
    unsigned int u32UlBdqOverflow;       /* 中断上报上行BD队列溢出次数 */
    unsigned int u32DlBdqOverflow;       /* 中断上报下行BD队列溢出次数 */
    unsigned int u32UlBdNotEnough;       /* 上行配置BD,BD不够用次数 */
    unsigned int u32DlBdNotEnough;       /* 下行配置BD,BD不够用次数 */
    unsigned int u32UlAdq0Empty;         /* 中断上报上行ADQ0空次数 */
    unsigned int u32UlAdq1Empty;         /* 中断上报上行ADQ1空次数 */
    unsigned int u32DlAdq0Empty;         /* 中断上报下行ADQ0空次数 */
    unsigned int u32DlAdq1Empty;         /* 中断上报下行ADQ1空次数 */
    unsigned int u32UlAd0NotEnough;      /* 上行ADQ0不够次数 */
    unsigned int u32UlAd1NotEnough;      /* 上行ADQ0不够次数 */
    unsigned int u32DlAd0NotEnough;      /* 下行ADQ0不够次数 */
    unsigned int u32DlAd1NotEnough;      /* 下行ADQ0不够次数 */
    unsigned int u32DlCdNotEnough;       /* 下行配置CD,CD不够用次数 */
    unsigned int u32UlIPFBusyNum;        /* 上行IPF忙次数 */
    unsigned int u32DlIPFBusyNum;        /* 下行IPF忙次数 */
    unsigned int u32UlResumeTimeoutCnt;  /* 上行过滤规则恢复超时次数 */
    unsigned int u32DlResumeTimeoutCnt;  /* 下行过滤规则恢复超时次数 */

    unsigned int u32UlBdNum;             /* 上行发送BD计数 */
    unsigned int u32UlAdq0Num;           /* 上行配置AD0计数 */
    unsigned int u32UlAdq1Num;           /* 上行配置AD1计数 */
    unsigned int u32UlRdNum;             /* 上行接收RD计数 */
    unsigned int u32DlBdNum;             /* 下行发送BD计数 */
    unsigned int u32DlAdq0Num;           /* 下行配置AD1计数 */
    unsigned int u32DlAdq1Num;           /* 下行配置AD1计数 */
    unsigned int u32DlRdNum;             /* 下行接收RD计数 */

    /* 参数有效性 */
    unsigned int u32UlBdAlignErr;        /* 上行Bd对齐错误,字节对齐 */
    unsigned int u32DlBdAlignErr;        /* 下行Bd对齐错误,8字节对齐 */
    unsigned int u32UlBdSizeErr;         /* 上行Bd大小错误,len为0 */
    unsigned int u32DlBdSizeErr;         /* 上行Bd大小错误,len为0 */
}IPF_DEBUG_INFO_S;

typedef struct tagIPF_ADQ_INFO_S
{
    unsigned int *pu32RptReg;   /* 读指针寄存器 */
}IPF_ADQ_INFO_S;

typedef struct tagIPF_CONTEXT_S
{
    unsigned int isInit:     1;  /* 1:已初始化 0:未初始化 */
    unsigned int resv:       31; /* reserved */
}IPF_CONTEXT_S;
/**************************************************************************
  UNION定义
**************************************************************************/

/**************************************************************************
  OTHERS定义
**************************************************************************/



/**************************************************************************
  函数声明
**************************************************************************/
extern int IPF_Init(void);
extern int BSP_IPF_RegisterWakeupDlCb(BSP_IPF_WakeupDlCb pFnWakeupDl);
extern void BSP_IPF_GetDlRd(unsigned int* pu32Num, IPF_RD_DESC_S *pstRd);
extern unsigned int BSP_IPF_GetUlRdNum(void);
extern unsigned int BSP_IPF_GetDlRdNum(void);
extern int BSP_IPF_GetDlAdNum(unsigned int* pu32AD0Num, unsigned int* pu32AD1Num);
extern int BSP_IPF_ConfigDlAd(IPF_AD_TYPE_E eAdType, unsigned int u32AdNum, IPF_AD_DESC_S *pstAdDesc);
extern int BSP_IPF_RegisterAdqEmptyDlCb(BSP_IPF_AdqEmptyDlCb pAdqEmptyDl);
extern int BSP_IPF_GetUsedDlAd(IPF_AD_TYPE_E eAdType, unsigned int *pu32AdNum, IPF_AD_DESC_S *pstAdDesc);
extern void BSP_IPF_DlRegReInit(void);
extern void BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_E flagvalue);
extern int BSP_IPF_ConfigTimeout(unsigned int u32Timeout);
extern int BSP_IPF_ConfigUpFilter(unsigned int u32Num, IPF_CONFIG_ULPARAM_S* pstUlPara);
extern unsigned int BSP_IPF_GetUlDescNum(void);
extern unsigned int BSP_IPF_GetUlBDNum(void);
extern int BSP_IPF_UlStateIdle(void);
extern int BSP_IPF_SetPktLen(unsigned int u32MaxLen, unsigned int u32MinLen);
extern int BSP_IPF_GetStat(IPF_FILTER_STAT_S *pstFilterStat);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
#endif/*_IPF_DRIVER_H_*/
