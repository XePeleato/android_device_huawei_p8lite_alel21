/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  Drvinterface_inner.h
*
*   作    者 :  chenyingguo
*
*   描    述 :  
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_INTERFACE_INNER_H__
#define __DRV_INTERFACE_INNER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if ( defined(BSP_CORE_MODEM) && !defined(__BOOTLOADER__) && defined(CHIP_BB_HI6758) && defined(__OS_RTOSCK__))
#include "sre_base.h"
#include "sre_shell.h"
#endif

#include "drv_global.h"
#include "drv_ifc.h"
#include "drv_icc.h"
#include "mdrv.h"
#include "memMapGlobal.h"

/* 虚地址转换规则 */
#define VIRT_ADDRESS(x)          x##_VIRT

#define CT_LOG(fileno)      /*exchSaveCodeTrace(fileno, __LINE__)*/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#define DMA_SUCCESS 0
#else
#if (defined CHIP_BB_HI6758) ||(defined (BOARD_FPGA))/*address transfer */
#define IO_ADDRESS(x)   ((((x) & 0xF0000000) >> 4) | ((x) & 0x00FFFFFF) | 0xF0000000)
#define IO_TYPE(var) ((ICC_SUB_CHAN_S *)IO_ADDRESS((unsigned int)(var)))
#define PHY_TO_VIRT(var)  (((unsigned int)(var) - GLOBAL_MEM_CORE_SHARE_ADDR) + GLOBAL_MEM_CORE_SHARE_ADDR_VIRT)
#define DDR_VIRT_TO_PHY(var)  (((unsigned int)(var) - PBXA9_DRAM_SHARE_VIRT) + PBXA9_DRAM_SHARE_PHYS)
#define DRV_PHY_TO_VIRT(var)  (((unsigned int)(var) - PBXA9_DRAM_SHARE_PHYS) + PBXA9_DRAM_SHARE_VIRT)
#endif
#define bsp_assert(expr)                                            \
    if (!(expr)) {                                          \
        printk("Assertion failed! %s,%s,%s,line=%d\n",  \
        #expr, __FILE__, __func__, __LINE__);    \
    }
#endif

/* 通用错误码，从0x1001开始 */
#define BSP_ERR_COMMON (0x1000)
#define BSP_ERR_MODULE_NOT_INITED (BSP_ERR_COMMON + 1)
#define BSP_ERR_NULL_PTR (BSP_ERR_COMMON + 2)
#define BSP_ERR_INVALID_PARA (BSP_ERR_COMMON + 3)
#define BSP_ERR_RETRY_TIMEOUT (BSP_ERR_COMMON + 4)
#define BSP_ERR_BUF_ALLOC_FAILED (BSP_ERR_COMMON + 5)
#define BSP_ERR_BUF_FREE_FAILED (BSP_ERR_COMMON + 6)

#define  BSP_MNTN_CACHE_FLUSH   0x8765abcd

/* 各模块专用错误码，从0x1开始  */
#define BSP_ERR_SPECIAL (0x0)

/* 调试宏定义 */
#ifdef __BSP_DEBUG__
#undef __BSP_DEBUG__
#endif

#define MODE_GSM      0x01
#define MODE_CDMA     0x02
#define MODE_WCDMA    0x04

/* 寄存器操作宏定义 */
#define BSP_REG8(base, reg) (*(volatile BSP_U8 *)((BSP_U32)base + (reg)))
#define BSP_REG16(base, reg) (*(volatile BSP_U16 *)((BSP_U32)base + (reg)))

/* 将var中[pos, pos + bits-1]比特清零,  pos从0开始编号
   e.g BDOM_CLR_BITS(var, 4, 2) 表示将Bit5~4清零 */
#define BSP_REG_CLRBITS(base, reg, pos, bits) (BSP_REG(base, reg) &= ~((((BSP_U32)1 << (bits)) - 1) << (pos)))
   
/* 将var中[pos, pos + bits-1]比特设置为val,  pos从0开始编号
   e.g BDOM_SET_BITS(var, 4, 2, 2) 表示将Bit5~4设置为b'10 */
#define BSP_REG_SETBITS(base, reg, pos, bits, val) (BSP_REG(base, reg) = (BSP_REG(base, reg) & (~((((BSP_U32)1 << (bits)) - 1) << (pos)))) \
                                                                         | ((BSP_U32)((val) & (((BSP_U32)1 << (bits)) - 1)) << (pos)))

/* 获取var中[pos, pos + bits-1]比特值,  pos从0开始编号 */
#define BSP_REG_GETBITS(base, reg, pos, bits) ((BSP_REG(base, reg) >> (pos)) & (((BSP_U32)1 << (bits)) - 1))

#define BSP_REG_WRITEBITS(base, reg, data) \
            (BSP_REG(base, reg) |= (data))
#define BSP_REG8_READ(base, reg, result) \
    ((result) = BSP_REG8(base, reg))

#define BSP_REG16_READ(base, reg, result) \
    ((result) = BSP_REG16(base, reg))

#define BSP_REG_MODIFY(base, reg, clrmsk, setmsk) \
    (BSP_REG(base, reg) = ((BSP_REG(base, reg) & ~clrmsk) | setmsk))

/* 计算最大、最小值 */
#ifndef BSP_MIN
 #define BSP_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef BSP_MAX
 #define BSP_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/* 打印级别定义 */
typedef enum tagBSP_LOG_LEVEL_E
{
    BSP_LOG_LEVEL_DEBUG = 0,  /* 0x0:debug-level                                  */
    BSP_LOG_LEVEL_INFO,      /* 0x1:informational                                */
    BSP_LOG_LEVEL_NOTICE,     /* 0x2:normal but significant condition             */
    BSP_LOG_LEVEL_WARNING,    /* 0x3:warning conditions                           */
    BSP_LOG_LEVEL_ERROR,      /* 0x4:error conditions                             */
    BSP_LOG_LEVEL_CRIT,       /* 0x5:critical conditions                          */
    BSP_LOG_LEVEL_ALERT,      /* 0x6:action must be taken immediately             */
    BSP_LOG_LEVEL_FATAL,      /* 0x7:just for compatibility with previous version */
    BSP_LOG_LEVEL_MAX         /* 边界值 */
} BSP_LOG_LEVEL_E;

/* 底软模块枚举定义 */
typedef enum tagBSP_MODULE_E
{
    BSP_MODU_NANDF = 0x0, /* 0x0 */
    BSP_MODU_TIMER, /* 0x1 */
    BSP_MODU_VIC,   /* 0x2 */
    BSP_MODU_UART,  /* 0x3 */
    BSP_MODU_YAFFS, /* 0x4 */
    BSP_MODU_ETH,   /* 0x5 */
    BSP_MODU_DMAC,  /* 0x6 */
    BSP_MODU_SPI,   /* 0x7 */
    BSP_MODU_GPIO,  /* 0x8 */
    BSP_MODU_WDT,   /* 0x9 */
    BSP_MODU_SCI,   /* 0xA */
    BSP_MODU_USB,   /* 0xB */
    BSP_MODU_DRX,   /* 0xC */
    BSP_MODU_SDIO,  /* 0xD */
    BSP_MODU_FTP,   /* 0xE */
    BSP_MODU_GMAC,      /* 0xF */
    BSP_MODU_SEEKER,    /* 0x10 */
    BSP_MODU_UPGRADE,   /* 0x11 */
    BSP_MODU_AXI,       /* 0x12 */
    BSP_MODU_NET,       /* 0x13 */
    BSP_MODU_USB_ACM,   /* 0x14 */
    BSP_MODU_USB_NCM,   /* 0x15 */
    BSP_MODU_AXI_MONITOR,
    BSP_MODU_IFC,
    BSP_MODU_ICC,
    BSP_MODU_IPF,
    BSP_MODU_ACC,       /* 0x1A */
    BSP_MODU_BBP,
    BSP_MODU_MEM,
    BSP_MODU_UDI,
    BSP_MODU_DLOAD, 
    BSP_MODU_SOCP,
    BSP_MODU_IFCP,      /* 0x20 */
    BSP_MODU_PWRCTRL,
    BSP_MODU_SERIAL_UPGRADE,
    BSP_MODU_SYNC,
    BSP_MODU_IPC,
    BSP_MODU_MNTN,
    BSP_MODU_SSI,       /*增加SSI模块，added by z00187553*/
    BSP_MODU_HKADC,
    BSP_MODU_PMU,       /*增加PMU模块，added by z00187553*/
    BSP_MODU_HIFI,      /*0x29*//*增加HIFI模块，added by w00193956*/
    BSP_MODU_AUDIO,
    BSP_MODU_CHGC,
    BSP_MODU_MAILBOX,
    BSP_MODU_SLAVE,
    BSP_MODU_MIPI,
    BSP_MODU_SKU,
    BSP_MODU_THERMAL,   /*温保模块*/
    BSP_MODU_MBX,
    BSP_MODU_DORESET,   /*当处理C核或者hifi单独复位出现异常时，A核主动复位， added by l00212112*/
    BSP_MODU_EDMA,      /*0x33*/
	BSP_MODU_WM8990,	/*wm8990模块*/
    BSP_MODU_LED,		/*LED模块*/
    BSP_MODU_ABB,		/*ABB模块*/
	BSP_MODU_RFILE,     /*add rfile system，add by l00201187*/
	BSP_MODU_SSDMA,     /*安全模块*/
	BSP_MODU_OSAL,
	BSP_MODU_TUNER,
	BSP_MODU_BBE,
    BSP_MODU_CIPHER,
	BSP_MODU_RST,
	BSP_MODU_EMMC,
	/*新加模块请从尾端开始，同时请勿改变上边各模块的位置*/
    BSP_MODU_ALL,       /* 代表所有的模块 */
    BSP_MODU_MAX        /* 边界值 */
} BSP_MODULE_E;



BSP_VOID BSP_ClearLogModule( BSP_MODULE_E enModule );
BSP_VOID BSP_SetLogModule( BSP_MODULE_E enModule );
BSP_U32  BSP_GetLogModule(BSP_VOID);
BSP_VOID BSP_SetLogLevel(BSP_LOG_LEVEL_E enLogLevel);
BSP_U32  BSP_GetLogLevel(BSP_VOID);
BSP_VOID BSP_SetPrintPos( BSP_BOOL bPrintPos);
BSP_BOOL BSP_GetPrintPos(BSP_VOID);

BSP_S32  BSP_ErrnoGet (BSP_VOID);
BSP_S32  BSP_TaskErrnoGet (BSP_S32 taskId);
BSP_S32  BSP_ErrnoSet (BSP_S32 s32ErrorValue);
BSP_S32  BSP_TaskErrnoSet (BSP_S32 s32TaskId, BSP_S32 s32ErrorValue);
BSP_VOID BSPLogHelp(BSP_VOID);


BSP_S32 BSP_ICC_Init(void);
extern BSP_S32 BSP_UDI_Init(void);
BSP_S32 BSP_MEM_Init(VOID);
BSP_VOID USB_UnReset(BSP_VOID);

#if !defined(BSP_CORE_MODEM)
BSP_BOOL USB_ETH_LinkStatGet(BSP_VOID);
#if( FEATURE_WIFI == FEATURE_ON )
BSP_S32 wifi_get_sta_num(BSP_VOID);
#endif
#endif

BSP_VOID WIFI_UnReset(BSP_VOID);
BSP_VOID HSIC_UnReset(BSP_VOID);

BSP_S32 test_thread_init(void);




#ifdef __BSP_DEBUG__
#include <logLib.h>

#define BSP_ASSERT(expr) \
{\
    if (!(expr)) { \
        logMsg("ASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
               (BSP_S32)__FILE__, (BSP_S32)__FUNCTION__, (BSP_S32)__LINE__, (BSP_S32) # expr, (BSP_S32)5, (BSP_S32)6); \
    return BSP_ERROR;\
    } \
}

#define BSP_ASSERT_HALT (expr) \
{\
    if (!(expr)) { \
        logMsg("ASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
               (BSP_S32)__FILE__, (BSP_S32)__FUNCTION__, (BSP_S32)__LINE__, (BSP_S32) # expr, (BSP_S32)5, (BSP_S32)6); \
        for (;;) ;\
    } \
}


#else
#define BSP_ASSERT(expr)
#define BSP_ASSERT_HALT(expr)
#endif


/*************************IFC BEGIN************************/
/* 错误码*/
/*专用错误码*/
#define BSP_ERR_IFC_SEND_FAIL         BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 1)) /*0x10120001*/
#define BSP_ERR_IFC_SEND_TIMEOUT      BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 2)) /*0x10120002*/
#define BSP_ERR_IFC_RCV_FAIL          BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 3)) /*0x10120003*/

#define IFC_ICC_CHAN_IN_FIFOSIZE  1024
#define IFC_ICC_CHAN_OUT_FIFOSIZE 1024
/*IFC发送最大长度*/
#define BSP_IFC_SEND_MAX_LEN  IFC_ICC_CHAN_IN_FIFOSIZE

/* 底软核间调用封装回调函数*/
/* pMsgBody:该函数ID对应函数的参数*/
/* u32Len:pMsgBody长度*/
typedef BSP_S32 (*BSP_IFCP_REG_FUNC)(BSP_VOID *pMsgBody,BSP_U32 u32Len);

typedef enum tagIFCP_FUNC_E
{
    IFCP_FUNC_NANDFINIT =  0,   /* NANDflash初始化同步*/
    IFCP_FUNC_SD_POWER_CTRL,    /* SD卡上下电管理*/
    IFCP_FUNC_SYNC,             /* 同步模块*/
    IFCP_FUNC_DMACINIT,      /* DMAC初始化同步*/
    IFCP_FUNC_USB_PCSC,
    IFCP_FUNC_USB_DIAGMODE,
    IFCP_FUNC_USB_U2DIAG,
    IFCP_FUNC_DFS_ACPULOAD,  
    IFCP_FUNC_DFS_SETPROFILE,  
    IFCP_FUNC_DFS_RELEASEPROFILE,   
    IFCP_FUNC_ONOFF_NOTIFY_APP      = 10,     /* notify Application power off event */
    IFCP_FUNC_ONOFF_SHUTDOWN        = 11,     /* A-CPU to shutdown the system */
    IFCP_FUNC_ONOFF_DRV_POWER_OFF   = 12,
    IFCP_FUNC_CHGC_CallUsb=13,/*UT打桩*/
    IFCP_FUNC_CHGC_ReportStaToAPP=14,/*UT打桩*/
    IFCP_FUNC_CHGC_GetUsbReply=15,/*UT打桩*/
    IFCP_FUNC_CHGC_InterfacePro=16,/*UT打桩*/
    IFCP_FUNC_CHGC_GetInterface=17,/*UT打桩*/
    IFCP_FUNC_CHGC_SendOledIoctrl=18,/*UT打桩*/
    IFCP_FUNC_LED_CTRL_SET = 19,         /*LED_DRV added by j00179452*/
    IFCP_FUNC_SD_LDO_CTRL = 20,   /*SD卡电源开关及电压设置,y00186965*/
    IFCP_FUNC_PMU_ID = 21,
    IFCP_FUNC_PMU_CTRL = 22,
    IFCP_FUNC_RTC_RW_SET = 23,
    IFCP_FUNC_RTC_READ_GET = 24,
    IFCP_FUNC_NVIM_READ_SET = 25,
    IFCP_FUNC_NVIM_READ_GET = 26,
    IFCP_FUNC_PWRCTRL_PMU_ACCESS = 27,        /*ACPU PMU ACCESS  read/write Reg*/
    IFCP_FUNC_SD_HOOK = 28,   /*SD卡钩子函数*/
    IFCP_FUNC_SDUP_INFO_SET = 29, /*SD卡升级版本信息获取*/
    IFCP_FUNC_SDUP_INFO_GET = 30, /*SD卡升级版本信息获取*/  
	IFCP_FUNC_LCD_PMU_ACCESS = 31,        /*ACPU PMU ACCESS  read/write Reg*/
    IFCP_FUNC_CHGC_USB_INOUT = 33,
    IFCP_FUNC_ONLINE_UP_INFO_SET =34,/*在线升级双核通讯*/
    IFCP_FUNC_ONLINE_UP_INFO_GET =35,/*在线升级双核通讯*/
    IFCP_FUNC_ERROR_LOG_SET=36,
    IFCP_FUNC_BREATH_LED_CTRL = 37, /*呼吸灯双核通信*/
	IFCP_FUNC_SD_LDO_RESP = 38,   /*SD卡电源开关及电压设置响应,y00186965*/
    IFCP_FUNC_LCD_IOCTRL_ACCESS = 39,/*用来C核LCD模块向A核发送ioctl命令*/
    IFCP_FUNC_SD_GET_HWVER =40, /*SD初始化获取硬件版本号*/
    IFCP_FUNC_GPIO_ANT=41,
    IFCP_FUNC_GPIO_USIM=42,
#if 1/*(FEATURE_UPDATEONLINE == FEATURE_ON)*/
    IFCP_FUNC_ONLINE_UP_INFO_FILE =43, /*在线升级双核通讯*/
#endif
    IFCP_FUNC_RFILE_SEM = 44,
    IFCP_FUNC_RFILE_CP2AP_SEM = 45,
    IFCP_FUNC_RFILE_LOG_SAVE = 46,
    IFCP_FUNC_RFILE_AP_NOTIFY_CP_INIT = 47,
	IFCP_FUNC_SD_IO_PMU_CTRL = 48,
    IFCP_FUNC_HISC_PMU_CTRL = 49,
    IFCP_FUNC_LED_RED_PMU_CTRL = 50,
    IFCP_FUNC_LED_GREEN_PMU_CTRL = 51,
    IFCP_FUNC_LED_BLUE_PMU_CTRL = 52,
    IFCP_FUNC_BUTT
} IFCP_FUNC_E;

BSP_U32 BSP_IFCP_RegFunc(IFCP_FUNC_E enFuncId, BSP_IFCP_REG_FUNC pFuncCb);
BSP_S32 IFC_Process(BSP_VOID *pMsgBody,BSP_U32 u32Len);
BSP_S32 IFC_Init(void);
BSP_S32 IFCP_Init(void);

/*************************IFC END************************/



/*************************TIMER BEGIN*****************************/

#define BSP_ERR_CLK_NO_FREE_CLK         BSP_DEF_ERR(BSP_MODU_TIMER,(BSP_ERR_SPECIAL + 1)) 
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Alloc
*
* 功能描述  : 申请用户时钟ID
*
* 输入参数  : BSP_S32 * ps32UsrClkId  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_U32 BSP_USRCLK_Alloc(BSP_S32 * ps32UsrClkId);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Free
*
* 功能描述  : 释放用户时钟
*
* 输入参数  : BSP_S32 s32UsrClkId  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_U32 BSP_USRCLK_Free(BSP_S32 s32UsrClkId);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateGet
*
* 功能描述  : This routine returns the interrupt rate of the system aux clock.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : BSP_S32 * pu32ClkRate  clk rate
* 返 回 值  : OK&错误码
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32 BSP_USRCLK_RateGet (BSP_S32 s32UsrClkId, BSP_S32 * pu32ClkRate);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStart
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
*             u32Cycles   Timer寄存器的计数值
* 输出参数  : 无
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerStart(BSP_S32 s32UsrClkid,BSP_U32 u32Cycles);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStop
*
* 功能描述  : 关闭单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
* 输出参数  : 无
* 返 回 值  : 无.
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerStop(BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerValue
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
* 输出参数  : Value      当前Timer value寄存器的计数值
* 返 回 值  : OK&错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_U32  BSP_USRCLK_TimerValue(BSP_S32 s32UsrClkId, BSP_U32 *pu32Value);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerMilliSecStart
*
* 功能描述  : 以毫秒为单位开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid        Timer Id
*             u32MilliSecond   设置定时器超时的毫秒值
* 输出参数  : 无
* 返 回 值  : OK&其他错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerMilliSecStart(BSP_S32 s32UsrClkid ,BSP_U32 u32MilliSecond);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerMilliSecValue
*
* 功能描述  : 获得当前Timer的毫秒值.
*
* 输入参数  : s32UsrClkid        Timer Id
* 输出参数  : BSP_U32 * pu32Value 当前Timer value寄存器的毫秒值
* 返 回 值  : OK&其他错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_U32 BSP_USRCLK_TimerMilliSecValue(BSP_S32 s32UsrClkId ,BSP_U32 * pu32Value);

/*****************************************************************************
* 函 数 名  : sysTimestampRateSet
*
* 功能描述  : This routine sets the interrupt rate of the timestamp clock.  It does 
*             not enable system clock interrupts unilaterally, but if the timestamp is currently enabled, the clock is disabled and then 
*             re-enabled with the new rate.  Normally it is called by usrRoot() 
*             in usrConfig.c.
*
* 输入参数  : int ticksPerSecond   number of clock interrupts per second 
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateSet(BSP_S32 ticksPerSecond);

/*****************************************************************************
* 函 数 名  : sysTimestampRateGet
*
* 功能描述  : This routine returns the interrupt rate of the timestamp clock.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : The number of ticks per second of the system clock.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateGet (BSP_VOID);

/*************************TIMER END*****************************/

/*************************SCI BEGIN*****************************/
typedef void (*USIMMSCIInfo)(void);
typedef BSP_U32 (*Sci_Event_Func)(BSP_U32 u32Event, BSP_VOID* para);

/*错误码*/
#define BSP_ERR_SCI_NOTINIT              951
#define BSP_ERR_SCI_INVALIDFREQ          952
#define BSP_ERR_SCI_USEDEFAULT           953
#define BSP_ERR_SCI_INVALIDPARA          954
#define BSP_ERR_SCI_NOCARD               955
#define BSP_ERR_SCI_NODATA               956
#define BSP_ERR_SCI_NOTREADY             957
#define BSP_ERR_SCI_DISABLED             958
#define BSP_ERR_SCI_UNSUPPORTED          959
#define BSP_ERR_SCI_INSERROR             960 
#define BSP_ERR_SCI_GETAPDU_ERROR        961
#define BSP_ERR_SCI_VLTG_HIGHEST         962
#define BSP_ERR_SCI_CURRENT_STATE_ERR    963
#define BSP_ERR_SCI_CURRENT_VLTG_ERR     964
#define BSP_ERR_SCI_TIMESTOP_TIMEOUT     965
#define BSP_ERR_SCI_BUFF_OVERRUN         967

/*回调函数注册事件类型*/
#define SCI_EVENT_CARD_IN                0x1
#define SCI_EVENT_CARD_OUT               0x2
#define SCI_EVENT_CARD_DEACTIVE_SUCCESS  0x4
#define SCI_EVENT_CARD_RESET_SUCCESS     0x8
#define SCI_EVENT_CARD_READ_DATA         0x10
#define SCI_EVENT_CARD_TX_ERR            0x20

#define SCI_DEF_CARD_OK     0x0 /* 卡正常，可以使用*/
#define SCI_DEF_CARD_ERROR  0x1   /* 无卡或者卡异常*/


/*************************SCI END*******************************/

/*************************PMU BEGIN*****************************/

/* 错误码 */
#define BSP_ERR_PMU_PARA_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 0x50)
#define BSP_ERR_PMU_ERROR_INIT        BSP_DEF_ERR(BSP_MODU_SPI, 0x51)   
#define BSP_ERR_PMU_SPI_NOT_INIT      BSP_DEF_ERR(BSP_MODU_SPI, 0x52)
#define BSP_ERR_PMU_NOT_INIT          BSP_DEF_ERR(BSP_MODU_SPI, 0x53)

/* PMU核间通信功能 */
/* 控制源类型 */
typedef enum _tagePmuCtrlSrType
{
    E_PMU_CTRL_SR_TYPE_LDO = 0,
    E_PMU_CTRL_SR_TYPE_BUCK,
    E_PMU_CTRL_SR_TYPE_DR,
    E_PMU_CTRL_SR_TYPE_BOTTOM
}ePmuCtrlSrType;

/* 操作类型 */
typedef enum _tagePmuCtrlType
{
    E_PMU_CTRL_TYPE_VOL_CUR_SET= 0,
    E_PMU_CTRL_TYPE_ONOFF,
    E_PMU_CTRL_TYPE_BOTTOM
}ePmuCtrlType;

typedef struct _tagStPmuCtrl
{
    ePmuCtrlSrType  eSrType;
    ePmuCtrlType    eCtrlType;
    BSP_U32         id;         /* LDO/BUCK/DR号 */
    union 
    {
        BSP_U32 value;  /* 电压或电流值 */
        BSP_BOOL bOn;   /* 是否打开 */
    }param;
}stPmuCtrl;

/*****************************************************************************
* 函 数 名  : BSP_PMU_CtrlCb
*
* 功能描述  : 供A核调用的PMU控制函数
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_CtrlCb( BSP_VOID *pMsgBody,BSP_U32 u32Len);

/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOOff
*
* 功能描述  : 关闭PMU LDO输出
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOOff( BSP_U8 LdoID );

/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOOn
*
* 功能描述  : 打开PMU LDO输出
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOOn( BSP_U8 LdoID );

/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckOff
*
* 功能描述  : 关闭PMU的Buck
*
* 输入参数  : BuckID, Buck号，从1开始
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckOff( BSP_U8 BuckID );

/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckOn
*
* 功能描述  : 打开PMU的Buck
*
* 输入参数  : BuckID, Buck号，从1开始
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckOn( BSP_U8 BuckID );

/*****************************************************************************
* 函 数 名  : BSP_PMU_DROff
*
* 功能描述  : DR打开关闭控制
*
* 输入参数  : DR_ID,DR编号,从1开始
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DROff(BSP_U8 DR_ID);

/*****************************************************************************
* 函 数 名  : BSP_PMU_DROn
*
* 功能描述  : DR打开
*
* 输入参数  : DR_ID,DR编号,从1开始
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DROn(BSP_U8 DR_ID);

/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOVoltGet
*
* 功能描述  : 获取LDO通道的电压值
*
* 输入参数  : LdoID，LDO通道
*             pusVoltValue，电压值指针，等于实际电压乘以100
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOVoltGet(BSP_U8 ucLDO);

/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOVoltSet
*
* 功能描述  : 设置PMU LDO电压输出值
*
* 输入参数  : LdoID，LDO通道
*             vltg，电压值，等于实际电压乘以100
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOVoltSet(BSP_U8 LdoID, BSP_U16 vltg);

/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckVoltSet
*
* 功能描述  : 设置PMU BUCK电压输出值
*
* 输入参数  : buckID，BUCK通道
*             vltg，电压值，等于实际电压乘以100
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckVoltSet(BSP_U8 buckID, BSP_U16 vltg);
/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckVoltGet
*
* 功能描述  : 设置PMU BUCK电压输出值
*
* 输入参数  : buckID，BUCK通道
*
* 输出参数  : 
*
* 返 回 值  : 
* 返 回 值  : 电压值,等于实际电压乘以100
*               -1,错误
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckVoltGet(BSP_U8 buckID);
/*******************************************************************************
  Function:     BSP_PMU_DrCurrentSet
  Description:  用于设置各路电流源的输出电流

  Input:        Vs_ID: PMU电流源编号，取值范围由PMU_ISRC定义：
                          DR1~DR3；
                ImA:      目标电流值（单位:mA），取值范围：
                          3mA,6mA,9mA,12mA,15mA,18mA,21mA,24mA
  Output:
  Return:       BSP_OK:    操作成功
                BSP_ERROR: 操作失败
  Others:
*******************************************************************************/
int BSP_PMU_DrCurrentSet(int I_ID, BSP_U16 ImA);
/*******************************************************************************
  Function:     BSP_PMU_DrCurrentGet
  Description:  用于查询各路电流源的输出电流

  Input:        I_ID: PMU电流源编号，取值范围 DR1~DR3 (0~2)
  Output:
  Return:       查询电流源的输出电流值（单位:mA）
                为负值（ERROR）表示查询失败
  Others:
*******************************************************************************/
int BSP_PMU_DrCurrentGet(int I_ID);
/**************************************************************************
  全局变量声明
**************************************************************************/


/**************************************************************************
  枚举定义
**************************************************************************/
/*DVFS type*/
typedef enum tagPMU_DVFS_TYPE_E
{
    DVFS_ARMCORE = 0,
    DVFS_CEVACORE,
    DVFS_BUTTOM 
}PMU_DVFS_TYPE_E;

/*DVFS Voltage type*/
typedef enum tagPMU_DVFS_VOLTAGE_TYPE_E
{
    DVFS_VOLTAGE_1000 = 0,
    DVFS_VOLTAGE_1100,
    DVFS_VOLTAGE_1150,
    DVFS_VOLTAGE_1200,
    DVFS_VOLTAGE_1250,
    DVFS_VOLTAGE_1350,
    DVFS_VOLTAGE_1500,
    DVFS_VOLTAGE_1800,
    DVFS_VOLTAGE_BUTTOM 
}PMU_DVFS_VOLTAGE_TYPE_E;
    
 /*DEVICE type*/
typedef enum tagPMU_DEVS_TYPE_E
{
    DEVS_RF = 0,
    DEVS_ABB,
    DEVS_SD,
    DEVS_SIM,
    DEVS_LCD,
    DEVS_BUTTOM 
}PMU_DEVS_TYPE_E;

    
/*DEVICE CTRL type*/
typedef enum tagPMU_DEVS_CTRL_TYPE_E
{
    CTRL_ON = 0,
    CTRL_OFF,
    CTRL_BUTTOM 
}PMU_DEVS_CTRL_TYPE_E;

 /*OCP type*/
typedef enum tagPMU_OCP_TYPE_E
{
    OCP_ONLYINT_NOAUTOOFF = 0,
    OCP_INT_AND_AUTOOFF,
    OCP_AUTOOFF,
    OCP_BUTTOM 
}PMU_OCP_TYPE_E;

 /* VOLTAGE ADJUST REG */
typedef enum tagPMU_VOL_SCI_E
{
    PMU_VOL_UP = 0, /* 由1.8伏升级到3.0伏*/
    PMU_VOL_DOWN,   /* 由3.0伏降到1.8伏*/
    PMU_VOL_SCI_BUTTOM
}PMU_VOL_SCI_E;


/* LED MODE */
typedef enum tagPMU_LED_STATE_E
{
    PMU_LED_FLASH_POWER_OFF = 0,    /* 全部不闪 */
    PMU_LED_FLASH_BLUE_ON,          /* 蓝灯长亮 */
    PMU_LED_FLASH_BLUE_SLOW,        /* 蓝灯慢闪 */
    PMU_LED_FLASH_BLUE_FAST,        /* 蓝灯快闪 */
    PMU_LED_FLASH_BLUE_ONCE,        /* 蓝灯单闪 */
    PMU_LED_FLASH_BLUE_TWICE,       /* 蓝灯双闪 */
    PMU_LED_FLASH_BLUE_FOUR,        /* 蓝灯四闪 */
    
    PMU_LED_FLASH_RED_ON,           /* 红灯长亮 */
    PMU_LED_FLASH_RED_SLOW,         /* 红灯慢闪 */
    PMU_LED_FLASH_RED_FAST,         /* 红灯快闪 */
    PMU_LED_FLASH_RED_ONCE,         /* 红灯单闪 */
    PMU_LED_FLASH_RED_TWICE,        /* 红灯双闪 */
    PMU_LED_FLASH_RED_FOUR,         /* 红灯四闪 */
    
    PMU_LED_FLASH_GREEN_ON,         /* 绿灯长亮 */
    PMU_LED_FLASH_GREEN_SLOW,       /* 绿灯慢闪 */
    PMU_LED_FLASH_GREEN_FAST,       /* 绿灯快闪 */
    PMU_LED_FLASH_GREEN_ONCE,       /* 绿灯单闪 */
    PMU_LED_FLASH_GREEN_TWICE,      /* 绿灯双闪 */
    PMU_LED_FLASH_GREEN_FOUR,       /* 绿灯四闪 */
    
    PMU_LED_FLASH_CYANBLUE_ON,      /* 青灯长亮 */
    PMU_LED_FLASH_CYANBLUE_SLOW,    /* 青灯慢闪 */
    PMU_LED_FLASH_CYANBLUE_FAST,    /* 青灯快闪 */
    PMU_LED_FLASH_CYANBLUE_ONCE,    /* 青灯单闪 */
    PMU_LED_FLASH_CYANBLUE_TWICE,   /* 青灯双闪 */
    PMU_LED_FLASH_CYANBLUE_FOUR,    /* 青灯四闪 */

    PMU_LED_STATE_BUTT
}PMU_LED_STATE_E;


/**************************************************************************
  STRUCT定义
**************************************************************************/

/*SPI的ID，ASIC上使用3片SPI*/
typedef enum tagSPI_DEV_ID
{
    SPI_ID0,
#if defined(CHIP_BB_HI6620) || defined(CHIP_BB_HI6210) ||(defined(BOARD_FPGA))
    SPI_ID1,
#endif
    SPI_ID_MAX
}SPI_DEV_ID_E;


/*每个SPI上的片选号，当前每个SPI有4个片选*/
typedef enum tagSPI_DEV_CS
{
    SPI_DEV_CS0,
    SPI_DEV_CS1,
    SPI_DEV_CS2,
    SPI_DEV_CS3,
    SPI_DEV_CS_MAX,
    NO_OWNER = -1
}SPI_DEV_CS_E;


/*数据传输方式，包括DMA方式和轮循方式*/
typedef enum tagSPI_SENDMOD
{
    SPI_SENDMOD_POLLING,                                                          
    SPI_SENDMOD_DMA,
    SPI_SENDMOD_MAX
}RXTX_MODE_E;


/* 记录SPI的ID和片选号的结构体*/
typedef struct tagSPI_DEV_STRUCTION
{
    SPI_DEV_ID_E enSpiId;
    SPI_DEV_CS_E enSpiCs;
}SPI_DEV_S;

/* 控制命令字长度 */
typedef enum tagspi_CommandLen                                                       
{
    SPI_COMMANDSIZE_1BITS =  0x0,     /*  1 bit command */
    SPI_COMMANDSIZE_2BITS =  0x1,     /*  2 bit command */
    SPI_COMMANDSIZE_3BITS =  0x2,     /*  3 bit command */
    SPI_COMMANDSIZE_4BITS  = 0x3,     /*  4 bit command */                           
    SPI_COMMANDSIZE_5BITS  = 0x4,     /*  5 bit command */                           
    SPI_COMMANDSIZE_6BITS  = 0x5,     /*  6 bit command */                           
    SPI_COMMANDSIZE_7BITS  = 0x6,     /*  7 bit command */                           
    SPI_COMMANDSIZE_8BITS  = 0x7,     /*  8 bit command */                           
    SPI_COMMANDSIZE_9BITS  = 0x8,     /*  9 bit command */                           
    SPI_COMMANDSIZE_10BITS = 0x9,     /* 10 bit command */                           
    SPI_COMMANDSIZE_11BITS = 0xA,     /* 11 bit command */                           
    SPI_COMMANDSIZE_12BITS = 0xB,     /* 12 bit command */                           
    SPI_COMMANDSIZE_13BITS = 0xC,     /* 13 bit command */                           
    SPI_COMMANDSIZE_14BITS = 0xD,     /* 14 bit command */                           
    SPI_COMMANDSIZE_15BITS = 0xE,     /* 15 bit command */                           
    SPI_COMMANDSIZE_16BITS = 0xF,      /* 16 bit command */ 
    SPI_COMMANDSIZE_MAX
} SPI_COMMAND_LEN_E;

/*数据帧长度*/
typedef enum tagspi_DataLen                                                       
{                                                                              
    SPI_DATASIZE_4BITS  = 0x3,     /*  4 bit data */                           
    SPI_DATASIZE_5BITS  = 0x4,     /*  5 bit data */                           
    SPI_DATASIZE_6BITS  = 0x5,     /*  6 bit data */                           
    SPI_DATASIZE_7BITS  = 0x6,     /*  7 bit data */                           
    SPI_DATASIZE_8BITS  = 0x7,     /*  8 bit data */                           
    SPI_DATASIZE_9BITS  = 0x8,     /*  9 bit data */                           
    SPI_DATASIZE_10BITS = 0x9,     /* 10 bit data */                           
    SPI_DATASIZE_11BITS = 0xA,     /* 11 bit data */                           
    SPI_DATASIZE_12BITS = 0xB,     /* 12 bit data */                           
    SPI_DATASIZE_13BITS = 0xC,     /* 13 bit data */                           
    SPI_DATASIZE_14BITS = 0xD,     /* 14 bit data */                           
    SPI_DATASIZE_15BITS = 0xE,     /* 15 bit data */                           
    SPI_DATASIZE_16BITS = 0xF,     /* 16 bit data */
    SPI_DATASIZE_MAX
} SPI_DATA_LEN_E;    

/* SPI支持的协议*/
typedef enum tagSPI_PROT
{
    SPI_PROT_SPI,
    SPI_PROT_SSP,
    SPI_PROT_NSM,
    SPI_PROT_MAX
}SPI_PROT_E;

typedef enum tagSPI_SCPOL
{
    SPI_SCPOL_LOW, /*下降沿触发*/
    SPI_SCPOL_HIGH,/*上升沿触发*/
    SPI_SCPOL_MAX
}SPI_SCPOL_E;

/*SPI外接设备特性结构体，包括设备需要的命令字长度，数据帧长度，使用协议等 */
typedef struct tagSPI_DEV_ATTR
{
    SPI_COMMAND_LEN_E enCommandLen;
    SPI_DATA_LEN_E enDataLen;
    SPI_PROT_E enSpiProt;
    SPI_SCPOL_E enSpiScpol;
    BSP_U16 u16SpiBaud;
}SPI_DEV_ATTR_S;

/*记录与SPI数据传输相关信息的结构体，成员包括要读写的SPI号，片选号，传输模式，
  数据地址，数据长度等*/                                     
typedef struct tagSPI_DATA_HANDLE
{
    SPI_DEV_ID_E enSpiID;
    SPI_DEV_CS_E enCsID;
    RXTX_MODE_E enMode;
    void *pvCmdData;
    BSP_U32 u32length;
}SPI_DATA_HANDLE_S;

/*****************************************************************************
* 函 数 名  : BSP_PMU_ShutDown
*
* 功能描述  : 本接口用于关闭PMU
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_VOID BSP_PMU_ShutDown(void);
/*****************************************************************************
* 函 数 名  : BSP_PMU_DVFS_VolAdjust
*
* 功能描述  : DVFS 电压调整
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DVFS_VolAdjust(PMU_DVFS_TYPE_E enDvfsType,PMU_DVFS_VOLTAGE_TYPE_E enVoltage);
/*****************************************************************************
* 函 数 名  : BSP_PMU_OCPConfig
*
* 功能描述  : OCPC配置
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_OCPConfig(PMU_OCP_TYPE_E enOcpType);
/*****************************************************************************
* 函 数 名  : BSP_PMU_DEVS_Ctrl
*
* 功能描述  : DEVS　控制
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DEVS_Ctrl(PMU_DEVS_TYPE_E enDevsType, PMU_DEVS_CTRL_TYPE_E enCtrlType);
/*****************************************************************************
* 函 数 名  : BSP_PMU_VolAdjust_ForSci
*
* 功能描述  : SCI电压控制
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_VolAdjust_ForSci(PMU_VOL_SCI_E enSCIVolCfg);
/*****************************************************************************
* 函 数 名  : BSP_PMU_VolAdjust_ForSDIO
*
* 功能描述  : SDIO电压控制
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_VolAdjust_ForSDIO(PMU_VOL_SCI_E enSCIVolCfg);

/*****************************************************************************
* 函 数 名  : BSP_PMU_CutOff_ForDrx
*
* 功能描述  : 低功耗场景PMU下电
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  :
* 修改记录  : 正确/错误
*
*****************************************************************************/
BSP_S32 BSP_PMU_CutOff_ForDrx(void);

/*****************************************************************************
* 函 数 名  : BSP_PMU_SetLED
*
* 功能描述  : 设置LTD灯的开关
*
* 输入参数  : enLEDType         LED0
*                               LED1
*                               LED2
*             enPower           PMU_LED_POWER_ON
*                               PMU_LED_POWER_OFF
* 输出参数  : 无
*
* 返 回 值  : OK：配置成功
*             ERROR：配置失败
*****************************************************************************/
BSP_S32 BSP_PMU_SetLedState(PMU_LED_STATE_E eLedState);


/*****************************************************************************
 函 数 名  : BSP_PMU_GetPMUState
 功能描述  : 获取PMU模块开机方式、充电状态、电池电量、电池在位状态。
 输入参数  : 无。
 输出参数  : Pmu_State :开机方式、充电状态、电池电量、电池在位状态。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
extern unsigned int BSP_PMU_GetPMUState(void*  Pmu_State);

/*************************PMU END*****************************/

/*************************INT BEGIN*****************************/

/*****************************************************************************
 * 函 数 名  : BSP_INT_DisConnect
 *
 * 功能描述  : 取消注册某个中断
 *
 * 输入参数  : VOIDFUNCPTR * vector 中断向量号，取值范围0～40
 *           VOIDFUNCPTR routine  中断服务程序
 *           INT32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_DisConnect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter);

/*****************************************************************************
 * 函 数 名  : BSP_INT_Lock
 *
 * 功能描述  : 屏蔽所有中断
 *
 * 输入参数  : 无
 * 
 * 输出参数  : 无
 *
 * 返 回 值  : 中断锁号
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Lock (BSP_VOID);

/*****************************************************************************
 * 函 数 名  : BSP_INT_UnLock
 *
 * 功能描述  : 打开中断
 *
 * 输入参数  : INT32 ulLocKKey  中断锁号，前一次BSP_INT_Lock的返回值
 * 
 * 输出参数  : 无
 *
 * 返 回 值  : 中断锁号
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_VOID BSP_INT_UnLock ( BSP_S32 s32LocKKey);

/*************************INT END*****************************/

/*************************WDT BEGIN*****************************/

#if defined (BSP_CORE_MODEM)  || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/* 看门狗的喂狗模式 */
#define BSP_HW_WATCHDOG       0
#define BSP_SW_WATCHDOG       1

#if defined(CHIP_BB_HI6620) || defined(CHIP_BB_HI6210)
/* 看门狗逻辑ID */
typedef enum WDT_LOGIC_ID_E
{
    WDT_DEF_WATCHDOG0,
    WDT_DEF_WATCHDOG_MAX

}WDT_LOGIC_ID;

#else

/*看门狗硬件信息数据结构*/
typedef struct tagWDT_ID_INFO_S
{
    BSP_U32 u32PartNum;
    BSP_U32 u32DesignerID;
    BSP_U32 u32Version;
    BSP_U32 u32Config;
    BSP_U32 u32PrimeCellID;
}WDT_ID_INFO_S;

typedef BSP_VOID(*wdt_timeout_cb)(void);
#endif
#endif

/*************************SD BEGIN*****************************/

/*****************************************************************************
* 函 数 名  : SDIO_check_card
*
* 功能描述  :
*
* 输入参数  : BSP_S32 s32CardNo
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 SDIO_check_card(BSP_S32 s32CardNo);

/*****************************************************************************
* 函 数 名  : read_write_blkdata
*
* 功能描述  : 

* 输入参数  : 

* 输出参数  : 
* 返 回 值  : 
*
* 修改记录  : 

*****************************************************************************/
BSP_S32  read_write_blkdata(BSP_S32 s32CardNo, BSP_U32 u32Addr,
                            BSP_U32 u32DataSize,
                            BSP_U8  *pu8DataBuff,
                            BSP_U8  *pu8CmdRespBuff,
                            BSP_U8  *pu8ErrRespBuff,
                            BSP_VOID (*callback)(BSP_U32 u32ErrData),
                            BSP_S32 s32NoOfRetries,
                            BSP_S32 s32Flags);

/*****************************************************************************
* 函 数 名  : sd_get_status
*
* 功能描述  : 卡在位查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
extern int sd_get_status(void);

/*****************************************************************************
* 函 数 名  : sd_get_capacity
*
* 功能描述  : 卡容量查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 失败；>0: 卡容量
*
* 其它说明  : NA
*
*****************************************************************************/
extern int sd_get_capacity(void);

/*****************************************************************************
* 函 数 名     : sd_sg_init_table
*
* 功能描述  : SD多块数据传输sg list初始化
*
* 输入参数  :    const void *buf         待操作的buffer地址
                 unsigned int buflen    待操作的buffer大小，小于32K，大小为512B的整数倍；
                                        大于32K，大小为32KB的整数倍，最大buffer为128K。
                            
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int sd_sg_init_table(const void *buf,unsigned int buflen);
#endif

/*****************************************************************************
* 函 数 名  : sd_multi_transfer
*
* 功能描述  : SD多块数据传输。
*
* 输入参数  : unsigned dev_addr       待写入的SD block 地址
              unsigned blocks         待写入的block个数
              unsigned blksz          每个block的大小，单位字节
              int write               读写标志位，写:1 ; 读:0
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int sd_multi_transfer(unsigned dev_addr, unsigned blocks, unsigned blksz, int write);
#endif

/*************************SD END**************************************/
#ifdef BSP_CPE
typedef enum tagCPE_SIGNAL_INTENSITY_E
{ 
    SIGNALNULL,
    SIGNALLOW,
    SIGNALMIDDLE,
    SIGNALHIGH,
    SIGNALMAX
}CPE_SIGNAL_INTENSITY_E;

typedef enum tagCPE_SIGNAL_STATUS_E
{
     SIGNAL_STATUS_ABNORMAL,    //信号异常
     SIGNAL_STATUS_NORMAL,      //信号正常
     SIGNAL_STATUS_MAX
}CPE_SIGNAL_STATUS_E;

typedef enum tagGPIO_CTRL_E
{
    GPIO_ON,
    GPIO_OFF,
    GPIO_MAX
}GPIO_CTRL_E;

#endif
/*************************DRX BEGIN***********************************/

/*PS注册函数类型*/
typedef BSP_U8   (*BSPRegDRXFunc)(BSP_VOID);

typedef enum tagDRX_SLEEP_MODE_E
{
    DRX_WEEK_SLEEP_MODE_E = 0x0,
    DRX_DEEP_SLEEP_MODE_E = 0x1,
    DRX_WEEK2_SLEEP_MODE_E = 0x2,
    DRX_NO_SLEEP_MODE_E = 0x3
}DRX_SLEEP_MODE_E;


typedef enum tagDRX_TIMER_ID_E
{
    DRX_LTESYS_TIMER = 0x0,
    DRX_LTET3412_TIMER,    
    DRX_LTEVPLMN_TIMER,    
    DRX_LTETSRHC_TIMER,    
    DRX_LTETA_TIMER,    
    DRX_DEEP_SLEEP_TIMER,    
    DRX_LTE_PS32K_TIMER,
    DRX_LTE_PS32K_BASETIMER,
    DRX_LTE_NO_TIMER
}DRX_TIMER_ID_E;

typedef enum tagDRX_TIMER_MODE_E
{
    DRX_TIMER_FREE_MODE = 0x0,
    DRX_TIMER_USER_MODE
}DRX_TIMER_MODE_E;

typedef enum tagDRX_REG_CB_E
{
    DRX_PS_GET_SLEEP_INFO = 0x0,
    DRX_RTC_TIME_START = 0x1,
    DRX_RTC_TIME_END = 0x2
}DRX_REG_CB_E;

typedef enum tagPWRCTRL_MODULE_E
{
    PWRCTRL_MODU_BOOTROM,       /*0x0*/
    PWRCTRL_MODU_EFUSE,
    PWRCTRL_MODU_SSDMA,
    PWRCTRL_MODU_LBBP,
    PWRCTRL_MODU_LDSP,      
    PWRCTRL_MODU_HSUART,
    PWRCTRL_MODU_EMI,  
    PWRCTRL_MODU_SDCC,
    PWRCTRL_MODU_USBHSIC,
    PWRCTRL_MODU_SOCP,
    PWRCTRL_MODU_GMAC,          /* 0xa */
    PWRCTRL_MODU_SCI0,
    PWRCTRL_MODU_SCI1,
    PWRCTRL_MODU_UICC,
    PWRCTRL_MODU_USBOTG_BC,
    PWRCTRL_MODU_MMC0,              /*0x10*/
    PWRCTRL_MODU_MMC1,
    PWRCTRL_MODU_MMC2,
    PWRCTRL_MODU_USBOTG,
    PWRCTRL_MODU_WDT,
    PWRCTRL_MODU_TCSSI,
    PWRCTRL_MODU_IPF,
    PWRCTRL_MODU_CIPHER,
    PWRCTRL_MODU_BBPCOMM,
    PWRCTRL_MODU_TENSI,             /*0x1a*/
    PWRCTRL_MODU_CORESIGHT,
    PWRCTRL_MODU_ACP,
    PWRCTRL_MODU_I2C,
    PWRCTRL_MODU_SPI1,
    PWRCTRL_MODU_DMAC,
    PWRCTRL_MODU_LSIO,                  /*0x20*/
    PWRCTRL_MODU_HIFI,
    PWRCTRL_MODU_TSENSOR,
    PWRCTRL_MODU_CICOM,
    PWRCTRL_MODU_EDMA,
    PWRCTRL_MODU_HDLC,
    PWRCTRL_MODU_DM,
    PWRCTRL_MODU_DWSSI0,
    PWRCTRL_MODU_WGBBP,
    PWRCTRL_MODU_ZSP,
    PWRCTRL_MODU_ALL,   /* 代表所有的模块 */
    PWRCTRL_MODU_MAX    /* 边界值 */
}PWRCTRL_MODULE_E;

typedef enum tagPWC_SLEEP_TYPE_E
{
    PWC_ARM_SLEEP = 0,
    PWC_LIGHT_SLEEP,
    PWC_DEEP_SLEEP,
    PWC_TYPE_BUTTON
}PWC_SLEEP_TYPE_E;

BSP_VOID BSP_PWRCTRL_ClkGateEn(BSP_VOID);
BSP_VOID BSP_PWRCTRL_ClkGateDis(BSP_VOID);
BSP_VOID BSP_PWRCTRL_SetWakeSrc(PWC_SLEEP_TYPE_E enSleepType);

/*****************************************************************************
* 函 数 名  : BSP_PWRCTRL_TimerClose
*
* 功能描述  : SOC外设时钟关闭接口
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 BSP_PWRCTRL_TimerClose(PWRCTRL_MODULE_E enModu);

/*****************************************************************************
* 函 数 名  : BSP_PWRCTRL_TimerOpen
*
* 功能描述  : SOC外设时钟使能接口
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 BSP_PWRCTRL_TimerOpen(PWRCTRL_MODULE_E enModu);

/*****************************************************************************
* 函 数 名  : set32KHwTimerMode
*
* 功能描述  : 查询32K硬件时钟计数器值
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 set32KHwTimerMode(DRX_TIMER_MODE_E eMode);

/*****************************************************************************
* 函 数 名  : get32KHwTimerMode
*
* 功能描述  : 查询32K硬件时钟计数器的工作模式
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
DRX_TIMER_MODE_E get32KHwTimerMode(void);

/*****************************************************************************
* 函 数 名  : BSP_DRX_RegCb
*
* 功能描述  : PS注册函数
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_VOID BSP_DRX_RegCb (DRX_REG_CB_E enType, BSPRegDRXFunc pFunc);

/*****************************************************************************
* 函 数 名  : usrTimer4Start
*
* 功能描述  : 启动32K硬件时钟定时器
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 usrTimer4Start(DRX_TIMER_ID_E eTimerId, BSP_U32 u32Count);

/*****************************************************************************
* 函 数 名  : usrTimer4Stop
*
* 功能描述  : 停止32K硬件时钟定时器
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 usrTimer4Stop(DRX_TIMER_ID_E eTimerId);

/*****************************************************************************
* 函 数 名  : usrTimer4Value
*
* 功能描述  : 查询32K BBP 时钟定时器剩余值
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 usrTimer4Value(DRX_TIMER_ID_E eTimerId);

/*************************DRX END*************************************/

/*************************SPI START***********************************/
/*错误码定义*/
#define BSP_ERR_SPI_MODULE_NOT_INITED BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_SPI_INVALID_PARA      BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_INVALID_PARA)
#define BSP_ERR_SPI_RETRY_TIMEOUT     BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_RETRY_TIMEOUT)

#define BSP_ERR_SPI_SEM_CREATE        BSP_DEF_ERR(BSP_MODU_SPI, 1)
#define BSP_ERR_SPI_SEM_LOCK          BSP_DEF_ERR(BSP_MODU_SPI, 2)
#define BSP_ERR_SPI_SEM_UNLOCK        BSP_DEF_ERR(BSP_MODU_SPI, 3)
#define BSP_ERR_SPI_DATASIZE_INVALID  BSP_DEF_ERR(BSP_MODU_SPI, 4)
#define BSP_ERR_SPI_ATTR_NOTSET       BSP_DEF_ERR(BSP_MODU_SPI, 5)
#define BSP_ERR_SPI_PROT_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 6)
#define BSP_ERR_SPI_TMOD_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 7)
#define BSP_ERR_SPI_RECEIVE_POLL      BSP_DEF_ERR(BSP_MODU_SPI, 8)



/*****************************************************************************
* 函 数 名  : BSP_SPI_SetAttr
*
* 功能描述  : 配置SPI的寄存器，设置控制命令字长度、数据帧长度等。
*
* 输入参数  : enSpiID        需要设置的SPI号，以及根据哪片片选进行配置的片选号。
*             pstSpiDevAttr  记录SPI外接设备特性的结构体指针，结构体成员包括设备
                             需要的命令字长度，数据帧长度，使用协议，波特率等。
*
* 输出参数  : 无
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_SetAttr(SPI_DEV_S *enSpiID,SPI_DEV_ATTR_S *pstSpiDevAttr);

/*****************************************************************************
* 函 数 名  : BSP_SPI_Write
*
* 功能描述  : 通过SPI向设备写入数据
*
* 输入参数  : pstWriteData 记录与SPI数据传输有关信息的结构体指针，成员包括
                           要读写的SPI号，片选号，传输模式等
*             pSendData    存储接收的数据缓冲区指针
*             u32Length    待接收的数据长度
*
* 输出参数  : 无
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_Write(SPI_DATA_HANDLE_S *pstWriteData,BSP_VOID *pSendData, BSP_U32 u32Length);

/*****************************************************************************
* 函 数 名  : BSP_SPI_Read
*
* 功能描述  : 通过SPI读取设备数据
*
* 输入参数  : pstReadData  记录与SPI数据传输有关信息的结构体指针，成员包括
                           要读写的SPI号，片选号，传输模式等。 
*             u32Length    待接收的数据长度
*
* 输出参数  : pRecData     存储接收的数据缓冲区指针。
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_Read(SPI_DATA_HANDLE_S *pstReadData,BSP_VOID *pRecData, BSP_U32 u32Length);

/*****************************************************************************
* 函 数 名  : BSP_SPI_GetAttr
*
* 功能描述  : 获取当前SPI设置的属性
*
* 输入参数  : enSpiId    要查询的SPI号。                        
*            
* 输出参数  : pstDevAttr 存放SPI属性的结构体指针。
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_GetAttr(SPI_DEV_ID_E enSpiId, SPI_DEV_ATTR_S *pstDevAttr);

/*************************SPI END*************************************/

/*************************IPCM BEGIN**********************************/

 /*****************************************************************************
  * 函 数 名  : BSP_IPC_DebugShow
  *
  * 功能描述  : 可维可测接口
  *
  * 输入参数  : 无  
  * 输出参数  : 无
  *
  * 返 回 值  : 无
  *
  * 修改记录  : 2011年4月11日 wangjing creat
  *****************************************************************************/
 BSP_VOID BSP_IPC_DebugShow(BSP_VOID);

/*以下是仅在V7R1 FPGA平台上才提供的GU IPC驱动接口*/
 /*****************************************************************************
 * 函 数 名  : BSP_GUIPC_SemCreate
 *
 * 功能描述  : 信号量创建函数
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
BSP_S32 BSP_GUIPC_SemCreate(BSP_U32 u32SignalNum);

/*****************************************************************************
* 函 数 名  : BSP_GUIPC_SemDelete
*
* 功能描述  : 删除信号量
*
* 输入参数  :   BSP_U32 u32SignalNum 要删除的信号量编号

* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_GUIPC_SemDelete(BSP_U32 u32SignalNum);

/*****************************************************************************
* 函 数 名  : BSP_GUIPC_IntEnable
*
* 功能描述  : 使能某个中断
*
* 输入参数  :
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_GUIPC_IntEnable (IPC_INT_LEV_E ulLvl);

/*****************************************************************************
 * 函 数 名  : BSP_GUIPC_IntDisable
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  :
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
BSP_S32 BSP_GUIPC_IntDisable (IPC_INT_LEV_E ulLvl);

/*****************************************************************************
 * 函 数 名  : BSP_GUIPC_IntConnect
 *
 * 功能描述  : 注册某个中断
 *
 * 输入参数  :
               BSP_U32 ulLvl 要使能的中断号，取值范围0～31
               VOIDFUNCPTR routine 中断服务程序
 *             BSP_U32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
BSP_S32 BSP_GUIPC_IntConnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
* 函 数 名  : BSP_GUIPC_IntSend
*
* 功能描述  : 发送中断
*
* 输入参数  :
                IPC_INT_CORE_E enDstore 要接收中断的core
                BSP_U32 ulLvl 要发送的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_GUIPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* 函 数 名  : BSP_GUIPC_SemTake
*
* 功能描述  : 获取信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32  BSP_GUIPC_SemTake(BSP_U32 u32SignalNum, BSP_S32 s32timeout);

/*****************************************************************************
* 函 数 名  : BSP_GUIPC_SemGive
*
* 功能描述  : 释放信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID BSP_GUIPC_SemGive(BSP_U32 u32SignalNum);


/*****************************************************************************
* 函 数 名  : BSP_GUIPC_SpinLock
*
* 功能描述  : 获取信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID BSP_GUIPC_SpinLock (BSP_U32 u32SignalNum);

/*****************************************************************************
* 函 数 名  : BSP_GUIPC_SpinUnLock
*
* 功能描述  : 释放信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID BSP_GUIPC_SpinUnLock (BSP_U32 u32SignalNum);

/*************************IPCM END************************************/

/*************************IPM START***********************************/

/**************************************************************************
  宏定义
**************************************************************************/

#define MAC_HEADER_LEN      14
#define IPM_SUCCESS                       BSP_OK
#define IPM_ERROR                         BSP_ERROR

/**************************************************************************
  STRUCT定义
**************************************************************************/

/* 核间结点条目域结构体 */
typedef struct tagIPM_DESC_ITEM_S
{
    BSP_U32 Addr;
    BSP_U16 Len;
    BSP_U16 Invalid;  /* 0表示配置BD,1表示不配BD */
} IPM_DESC_ITEM_S;

/* 核间描述结点结构体 */
typedef struct tagIPM_DESC_NODE_S
{
    BSP_U32 StartAddr;
    BSP_U16 ItemNum;  /* 总的Item数目 */
    BSP_U16 TailIdx;  /* 最后一个有效的Item ID, 从0开始 */
    BSP_U16 ValidNum; /* 有效的Item数目 */
    IPM_DESC_ITEM_S *pstDescItem;
}IPM_DESC_NODE_S;

/*****************************************************************************
* 函 数 名  : BSP_IPM_GetDescNode
*
* 功能描述  : 获取信息结点指针接口
*
* 输入参数  : BSP_U32 u32ItemNum    IP包的数量
*
* 输出参数  : 无
* 返 回 值  : 空 --- 获取失败
*             非空 ---  获取的信息结点指针
*
* 修改记录  :2011年1月27日   鲁婷  创建
*****************************************************************************/
IPM_DESC_NODE_S *BSP_IPM_GetDescNode(BSP_U32 u32ItemNum);

/*****************************************************************************
* 函 数 名  : BSP_IPM_FreeDescNode
*
* 功能描述  : 释放信息结点指针接口
*
* 输入参数  : IPM_DESC_NODE_S *pstDescNode
*             BSP_BOOL bNodeOnly
*             true只释放结点      false 释放结点和组织内存
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月27日   鲁婷  创建
*****************************************************************************/
BSP_S32 BSP_IPM_FreeDescNode(IPM_DESC_NODE_S *pstDescNode, BSP_BOOL bNodeOnly);

/*****************************************************************************
* 函 数 名  : BSP_IPM_GetBspBuf
*
* 功能描述  : 申请内存接口
*
* 输入参数  : BSP_U32 u32len    需要申请的buf长度
*
* 输出参数  : 无
* 返 回 值  : 获取的buf指针
*
* 修改记录  :2011年1月27日   鲁婷  创建
*****************************************************************************/
BSP_U8 *BSP_IPM_GetBspBuf(BSP_U32 u32len);

/*************************IPM END*************************************/


/*************************SSI START***********************************/
/*错误码定义*/
#define BSP_ERR_SSI_MODULE_NOT_INITED  BSP_DEF_ERR(BSP_MODU_SSI, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_SSI_INVALID_PARA        BSP_DEF_ERR(BSP_MODU_SSI, BSP_ERR_INVALID_PARA)
#define BSP_ERR_SSI_RETRY_TIMEOUT       BSP_DEF_ERR(BSP_MODU_SSI, BSP_ERR_RETRY_TIMEOUT)

#define BSP_ERR_SSI_SEM_CREATE           BSP_DEF_ERR(BSP_MODU_SSI, 1)
#define BSP_ERR_SSI_SEM_LOCK             BSP_DEF_ERR(BSP_MODU_SSI, 2)
#define BSP_ERR_SSI_SEM_UNLOCK           BSP_DEF_ERR(BSP_MODU_SSI, 3)
#define BSP_ERR_SSI_ATTR_NOTSET          BSP_DEF_ERR(BSP_MODU_SSI, 4)
#define BSP_ERR_SSI_TXFIFO_NOTEMPTY      BSP_DEF_ERR(BSP_MODU_SSI, 5)
#define BSP_ERR_SSI_RXFIFO_EMPTY         BSP_DEF_ERR(BSP_MODU_SSI, 6)
#define BSP_ERR_SSI_RXFIFO_NOTEMPTY      BSP_DEF_ERR(BSP_MODU_SSI, 7)
#define BSP_ERR_SSI_RXNUM_WRONG          BSP_DEF_ERR(BSP_MODU_SSI, 8)

/*SSI的ID，ASIC上使用3片SSI*/
typedef enum tagSSI_DEV_ID
{
    SSI_ID0,
    SSI_ID1,
    SSI_ID2,
    SSI_ID3,
    SSI_ID_MAX
}SSI_ID_E;

/*数据长度 */
typedef enum tagssi_DATALEN                                                       
{
    SSI_DATALEN_2BITS =  0x1,     /*  2 bit address */
    SSI_DATALEN_3BITS =  0x2,     /*  3 bit address */
    SSI_DATALEN_4BITS  = 0x3,     /*  4 bit address */                           
    SSI_DATALEN_5BITS  = 0x4,     /*  5 bit address */                           
    SSI_DATALEN_6BITS  = 0x5,     /*  6 bit address */                           
    SSI_DATALEN_7BITS  = 0x6,     /*  7 bit address */                           
    SSI_DATALEN_8BITS  = 0x7,     /*  8 bit address */                           
    SSI_DATALEN_9BITS  = 0x8,     /*  9 bit address */                           
    SSI_DATALEN_10BITS = 0x9,     /* 10 bit address */                           
    SSI_DATALEN_11BITS = 0xA,     /* 11 bit address */                           
    SSI_DATALEN_12BITS = 0xB,     /* 12 bit address */                           
    SSI_DATALEN_13BITS = 0xC,     /* 13 bit address */                           
    SSI_DATALEN_14BITS = 0xD,     /* 14 bit address */                           
    SSI_DATALEN_15BITS = 0xE,     /* 15 bit address */                           
    SSI_DATALEN_16BITS = 0xF,     /* 16 bit address */
    SSI_DATALEN_17BITS = 0x10,    /* 17 bit address */
    SSI_DATALEN_18BITS = 0x11,    /* 18 bit address */
    SSI_DATALEN_19BITS = 0x12,    /* 19 bit address */
    SSI_DATALEN_20BITS = 0x13,    /* 20 bit address */
    SSI_DATALEN_21BITS = 0x14,    /* 21 bit address */
    SSI_DATALEN_22BITS = 0x15,    /* 22 bit address */
    SSI_DATALEN_23BITS = 0x16,    /* 23 bit address */
    SSI_DATALEN_24BITS = 0x17,    /* 24 bit address */
    SSI_DATALEN_MAX
} SSI_DATA_LEN_E;

/*地址长度*/
typedef enum tagssi_ADDRLEN                                                      
{                                                                              
    SSI_ADDRLEN_1BITS  = 0x0,     /*  4 bit data */                           
    SSI_ADDRLEN_2BITS  = 0x1,     /*  5 bit data */                           
    SSI_ADDRLEN_3BITS  = 0x2,     /*  6 bit data */                           
    SSI_ADDRLEN_4BITS  = 0x3,     /*  7 bit data */                           
    SSI_ADDRLEN_5BITS  = 0x4,     /*  8 bit data */                           
    SSI_ADDRLEN_6BITS  = 0x5,     /*  9 bit data */                           
    SSI_ADDRLEN_7BITS  = 0x6,     /* 10 bit data */                           
    SSI_ADDRLEN_8BITS  = 0x7,     /* 11 bit data */     
    SSI_ADDRLEN_MAX
} SSI_ADDR_LEN_E;    

/*超时等待*/
typedef enum tagssi_WAITTIME
{
    SSI_WAITTIME_1CYCLES = 0x0,
    SSI_WAITTIME_2CYCLES = 0x1,
    SSI_WAITTIME_3CYCLES = 0x2,
    SSI_WAITTIME_4CYCLES = 0x3,
    SSI_WAITTIME_5CYCLES = 0x4,
    SSI_WAITTIME_6CYCLES = 0x5,
    SSI_WAITTIME_7CYCLES = 0x6,
    SSI_WAITTIME_8CYCLES = 0x7,
    SSI_WAITTIME_9CYCLES = 0x8,
    SSI_WAITTIME_10CYCLES = 0x9,
    SSI_WAITTIME_11CYCLES = 0xA,
    SSI_WAITTIME_12CYCLES = 0xB,
    SSI_WAITTIME_13CYCLES = 0xC,
    SSI_WAITTIME_14CYCLES = 0xD,
    SSI_WAITTIME_15CYCLES = 0xE,
    SSI_WAITTIME_16CYCLES = 0xF,
    SSI_WAITTIME_MAX
}SSI_WAIT_TIME_E;

/*SSI外接设备特性结构体，包括设备的数据长度，地址长度，等待超时时间等 */
typedef struct tagSSI_DEV_ATTR
{
    SSI_ADDR_LEN_E enAddrLen;
    SSI_DATA_LEN_E enDataLen;
    SSI_WAIT_TIME_E enWaitTime;
}SSI_SLAVE_ATTR_S;

/*****************************************************************************
* 函 数 名  : BSP_SSI_SetAttr
*
* 功能描述  : 配置SSI的寄存器，设置控制命令字长度、数据帧长度等。
*
* 输入参数  : enSSIID        需要设置的SSI号，以及根据哪片片选进行配置的片选号。
*             pstSSIDevAttr  记录SSI外接设备特性的结构体指针，结构体成员包括设备
                             需要的命令字长度，数据帧长度，使用协议，波特率等。
*
* 输出参数  : 无
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SSI_SetAttr(SSI_ID_E enSsiId, SSI_SLAVE_ATTR_S *pstSsiDevAttr);

/*****************************************************************************
* 函 数 名  : BSP_SSI_Write
*
* 功能描述  : 通过SSI向设备写入数据
*
* 输入参数  : pstWriteData 记录与SSI数据传输有关信息的结构体指针，成员包括
                           要读写的SSI号，片选号，传输模式等
*             pSendData    存储接收的数据缓冲区指针
*             u32Length    待接收的数据长度
*
* 输出参数  : 无
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SSI_Write(SSI_ID_E enSsiId, BSP_VOID *pSrcAddr, BSP_U32 u32Length);

/*****************************************************************************
* 函 数 名  : BSP_SSI_Read
*
* 功能描述  : 通过SSI读取设备数据
*
* 输入参数  : pstReadData  记录与SSI数据传输有关信息的结构体指针，成员包括
                           要读写的SSI号，片选号，传输模式等。 
*             u32Length    待接收的数据长度
*
* 输出参数  : pRecData     存储接收的数据缓冲区指针。
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SSI_Read(SSI_ID_E enSsiId,BSP_VOID *pSrcAddr,BSP_VOID *pDescAddr,BSP_U32 u32Length);

/*****************************************************************************
* 函 数 名  : BSP_SSI_GetAttr
*
* 功能描述  : 获取当前SSI设置的属性
*
* 输入参数  : enSSIId    要查询的SSI号。                        
*            
* 输出参数  : pstDevAttr 存放SSI属性的结构体指针。
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SSI_GetAttr(SSI_ID_E enSsiId, SSI_SLAVE_ATTR_S *pstDevAttr);

/*************************SSI END*************************************/


/*************************NANDF BEGIN*********************************/

/**************************************************************************
  宏定义 
**************************************************************************/
/*错误码*/
#define NANDF_OK              BSP_OK
#define NANDF_ERROR_INIT      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_MODULE_NOT_INITED)  /*没有初始化*/
#define NANDF_ERROR_ARGS      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_INVALID_PARA)  /*参数错误*/
#define NANDF_ERROR_MALLOC    BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_BUF_ALLOC_FAILED)  /*内存空间分配失败*/
#define NANDF_ERROR_TIMEOUT   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_RETRY_TIMEOUT)  /*循环次数达到最大*/
#define NANDF_BAD_BLOCK       BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 1)  /*坏块*/
#define NANDF_GOOD_BLOCK      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 2)  /*好块*/
#define NANDF_ERROR_READ      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 3)  /*读操作失败*/
#define NANDF_ERROR_WRITE     BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 4)  /*写操作失败*/
#define NANDF_ERROR_ERASE     BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 5)  /*擦除操作失败*/
#define NANDF_ERROR_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 6)  /*标志坏块失败*/
#define NANDF_ERROR_ERASE_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 7)  /*擦除失败并且标志坏块失败*/
#define NANDF_ERROR_WRITE_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 8)  /*写入失败并且标志坏块失败*/
#define NANDF_ERROR_STATUS_CHECK   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 9)  /*读取状态寄存器，状态不正确*/
#define NANDF_ERROR_SEMTAKE   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 10)  /*申请信号量失败*/
#define NANDF_ERROR_SEMGIVE   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 11)  /*释放信号量失败*/
#define NANDF_ERROR_ADDR_OVERFLOW BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 12)  /* 地址越界 */
#define NANDF_ERROR_MODULE_LOCKED  BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 13)

/******************************************************************************
* Function     :   BSP_NANDF_Read
* 
* Description  :   读取Nand Flash中指定地址和长度内容到指定空间中
* 
* Input        :   u32FlashAddr  读取数据的源地址
*              :   u32NumByte    读取数据长度，单位为字节
* 
* Output       :   pRamAddr      读取的数据存放的地址
* 
* return       :   读操作成功与否
******************************************************************************/
extern BSP_U32 nandc4_FlashRead(BSP_U32 u32FlashAddr, BSP_VOID* pRamAddr, BSP_U32 u32NumByte);

/******************************************************************************
* Function     :   BSP_NANDF_Write
* 
* Description  :   将指定地址和长度内容到写到指定FLASH地址中
* 
* Input        :   pRamAddr     写操作源地址    
*              :   u32NumByte   数据长度，单位为字节
* 
* Output       :   u32FlashAddr 写操作目的地址
* 
* return       :   写操作成功与否
******************************************************************************/
extern BSP_U32 nandc4_FlashWrite(BSP_U32 u32FlashAddr, BSP_VOID * pRamAddr, BSP_U32 u32NumByte);

/******************************************************************************
* Function     :   BSP_NANDF_Erase
* 
* Description  :   擦除指定Flash地址所在块
* 
* Input        :   u32address   要擦除块的ID
* 
* Output       :   无
* 
* return       :   擦除操作成功与否
******************************************************************************/
extern  BSP_U32 nandc4_BlockEraseById(BSP_U32 u32BlockID);

/*************************NANDF END***********************************/

/*************************OM START************************************/

typedef enum{
     MEMCHKTYPE16BIT,
     MEMCHKTYPE32BIT
}ENMEMCHKTYPE;

/*****************************************************************************
* 函 数 名  : BSP_OM_GetFlashSpec
*
* 功能描述  : 获取FLASH规格
*
* 输入参数  : 无
*
* 输出参数  : BSP_U32 *pu32FlashTotalSize  : FLASH总大小
*             BSP_U32 *pu32FlashBlkSize     : FLASH块大小
*             BSP_U32 *pu32FlashPageSize    : FLASH页大小
*
* 返 回 值  : BSP_ERROR:获取失败
*             BSP_OK:获取成功
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_OM_GetFlashSpec(BSP_U32 *pu32FlashTotalSize, BSP_U32 *pu32FlashBlkSize, BSP_U32 *pu32FlashPageSize);
/*****************************************************************************
* 函 数 名  : BSP_OM_GetVerTime
*
* 功能描述  : 获得ver时间
*
* 输入参数  : 无
*
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_VOID BSP_OM_GetVerTime(BSP_S8* strVerTime);

/******************************************************************************
* Function     :   BSP_OM_MemNWrite
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemNWrite(BSP_U32* u32MemAddr, BSP_U32 u32Value, BSP_U32 u32Count);
#if 0
/******************************************************************************
* Function     :   BSP_OM_MemPrint
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemPrint(BSP_U32* u32MemAddr,ENADDRTYPE enAddrType);
#endif
/******************************************************************************
* Function     :   BSP_OM_MemChk
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemChk(BSP_U32* u32StartAddr, BSP_U32* u32EndAddr, ENMEMCHKTYPE enChkType);

/******************************************************************************
* Function     :   BSP_OM_GetFPGAVer
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_GetFPGAVer(void);
#if 0
/******************************************************************************
* Function     :   BSP_OM_NET
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_VOID BSP_OM_NET(BSP_OM_NET_S *pstNetOm);
#endif
/******************************************************************************
* Function     :   BSP_OM_ShellLock
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_ShellLock(BSP_BOOL bRequest);



/*****************************************************************************
* 函 数 名  : BSP_OM_GetTcmAdr
*
* 功能描述  : 获取DSP邮箱基地址
*
* 输入参数  : BSP_VOID  
* 输出参数  : 无
*
* 返 回 值  : DSP邮箱基地址
*
* 其它说明  : 返回值0为非法值
*
*****************************************************************************/
BSP_U32 BSP_OM_GetTcmAdr(BSP_VOID);

/*************************OM END**************************************/

/*************************WIFI START**********************************/

/*****************************************************************************
 函 数 名  : WIFI_TEST_CMD
 功能描述  : 测试命令
 输入参数  : cmdStr；命令字符串
 输出参数  : 无
 返回值    ：
    0: execute succes
    1: execute failed
*****************************************************************************/
extern int wifi_tcmd(char * cmdStr);

/*****************************************************************************
 函 数 名  : wifi_get_tcmd_mode
 功能描述  : 获取测试命令的模式
 输入参数  : 无
 输出参数  : 16：校准测试模式
             17：表示处于发射模式
             18：表示接收模式
 返回值    ：函数执行的状态结果值
*****************************************************************************/
extern int wifi_get_tcmd_mode(void);

/*****************************************************************************
 函 数 名  : wifi_power_on
 功能描述  : WIFI上电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok 
              1: execute failed
*****************************************************************************/
extern int wifi_power_on(void);

/*****************************************************************************
 函 数 名  : wifi_power_off
 功能描述  : WIFI下电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok 
              1: execute failed
*****************************************************************************/
extern int wifi_power_off(void);

/*****************************************************************************
 函 数 名  : wifi_get_status
 功能描述  : WIFI状态获取
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: wifi is off
              1: wifi is in normal mode
              2: wifi is in tcmd mode
*****************************************************************************/
extern int wifi_get_status(void);

/*****************************************************************************
 函 数 名  : wifi_get_rx_detail_report
 功能描述  : get result of rx report: totalPkt, GoodPkt, ErrorPkt
 输入参数  : 无
 输出参数  : totalPkt、goodPkt、badPkt
 返回值    ：无
*****************************************************************************/
extern void wifi_get_rx_detail_report(unsigned int *totalPkts, unsigned int *googPkts, unsigned int * badPkts);

/*****************************************************************************
 函 数 名  : wifi_get_rx_packet_report
 功能描述  : get result of rx ucast&mcast packets
 输入参数  : 无
 输出参数  : ucastPkts、mcastPkts
 返回值    ：无
*****************************************************************************/
extern void  wifi_get_rx_packet_report(unsigned int *ucastPkts, unsigned int *mcastPkts);

/*****************************************************************************
 函 数 名  : wifi_get_pa_curMode
 功能描述  : get the currrent PA mode of the wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI chip is in PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI chip is in no PA mode
             -1: wifi chip is in abnormal mode
*****************************************************************************/
extern int wifi_get_pa_curMode(void);

/*****************************************************************************
 函 数 名  : wifi_get_pa_mode
 功能描述  : get the support PA mode of wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode 
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode 
             -1: failed
*****************************************************************************/
extern int wifi_get_pa_mode(void);

/*****************************************************************************
 函 数 名  : wifi_set_pa_mode
 功能描述  : set the PA mode of wifi chip 
 输入参数  : 0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode 
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode 
 输出参数  : 无
 返回值    ：0: success
             -1: failed
*****************************************************************************/
extern int wifi_set_pa_mode(int wifiPaMode);

/*****************************************************************************
 函 数 名  : BSP_WifiDataReservedTail
 功能描述  : WIFI计算需要数据块内存
 输入参数  : usLen - 用户申请数据长度Len
 输出参数  : 无
 返 回 值  : 数据区的尾部预留的长度
*****************************************************************************/
extern unsigned int BSP_WifiDataReservedTail(unsigned int len);

/*****************************************************************************
 函 数 名  : WiFi_DrvSetRxFlowCtrl
 功能描述  : 设置WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : BSP_OK/BSP_ERROR
*****************************************************************************/
extern unsigned long WiFi_DrvSetRxFlowCtrl(unsigned long para1, unsigned long para2);

/*****************************************************************************
 函 数 名  : WiFi_DrvSetRxFlowCtrl
 功能描述  : 清除WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/    
extern unsigned long WiFi_DrvClearRxFlowCtrl(unsigned long para1, unsigned long para2);

/*****************************************************************************
 函 数 名  : drv_get_local_usr_connect_status
 功能描述  : 返回当前是否有USB连接或者WIFI用户连接(C核调用)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/
extern unsigned long  drv_get_local_usr_connect_status(void);

/*************************WIFI START**********************************/

/*************************NCM START***********************************/

/* 错误码*/
#define BSP_ERR_NCM_NOTXBUFF         BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 1)) /*0x10120001*/
#define BSP_ERR_NCM_NOIDLEDEV        BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 2)) /*0x10120002*/
#define BSP_ERR_NCM_AT_SEND_TIMEOUT  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 3))/*0x10121003*/
#define BSP_ERR_NCM_WRONG_STATE      BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 4))/*0x10121004*/
#define BSP_ERR_NCM_AT_SEND_FAIL     BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 5))/*0x10121005*/
#define BSP_ERR_NCM_SEND_EXCEED_MAXPKT  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 6))/*0x10121006*/
#define BSP_ERR_NCM_IPV6_DNS_NFT_FAIL  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 7))/*0x10121007*/


/*****************************************************************************
* 函 数 名  : BSP_NCM_Open
*
* 功能描述  : 用来获取可以使用NCM设备ID,数据通道类型，PS使用;控制通道类型，
*              MSP传输AT命令使用
*
* 输入参数  : NCM_DEV_TYPE_E enDevType  设备类型
* 输出参数  : pu32NcmDevId              NCM 设备ID
*
* 返 回 值  : BSP_ERR_NET_NOIDLEDEV
*             OK
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Open(NCM_DEV_TYPE_E enDevType, BSP_U32 * pu32NcmDevId);

/*****************************************************************************
* 函 数 名  : BSP_NCM_Write
*
* 功能描述  : 发送数据
*
* 输入参数  : BSP_U32 u32NcmDevId  NCM设备ID
* 输出参数  : BSP_VOID *pPktEncap       包封装首地址
*
* 返 回 值  : BSP_OK
*             BSP_ERR_NET_INVALID_PARA
*             BSP_ERR_NET_BUF_ALLOC_FAILED
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Write(BSP_U32 u32NcmDevId, BSP_VOID *pPktEncap, void *net_priv);

/*****************************************************************************
* 函 数 名  : BSP_NCM_Ioctl
*
* 功能描述  : 配置NCM设备属性
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Ioctl(BSP_U32 u32NcmDevId, NCM_IOCTL_CMD_TYPE_E enNcmCmd, BSP_VOID *param);

/*****************************************************************************
* 函 数 名  : BSP_NCM_Close
*
* 功能描述  : 关闭NCM设备
*
* 输入参数  : NCM_DEV_TYPE_E enDevType   设备ID类型
*             BSP_U32 u32NcmDevId        NCM设备ID
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Close(NCM_DEV_TYPE_E enDevType, BSP_U32 u32NcmDevId);

/*****************************************************************************
* 函 数 名  : NCM_debugPktGet
*
* 功能描述  : 提供给PS获取收发包统计
*
* 输入参数  : 
* 输出参数  : NCM_PKT_INFO_S *penPktNum :统计结构体
*
* 返 回 值  : OK/ERROR
*
* 修改记录  : 2011年8月5日   wangjing   creat
*****************************************************************************/
BSP_S32 NCM_debugPktGet(BSP_U32 u32DevId,NCM_PKT_INFO_S *penPktNum);

/*****************************************************************************
* 函 数 名  : NCM_debugPktClear
*
* 功能描述  : 提供给PS用于将统计值清0
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : OK/ERROR
*
* 修改记录  : 2011年8月5日   wangjing   creat
*****************************************************************************/
BSP_S32 NCM_debugPktClear(BSP_U32 u32DevId);

/*************************NCM END*************************************/

/*************************ACM START***********************************/

/* MODEM 管脚信号值定义 */
#define SIGNALNOCH 0
#define SIGNALCH 1
#define RECV_ENABLE 1
#define RECV_DISABLE 0
#define SEND_ENABLE 1
#define SEND_DISABLE 0
#define HIGHLEVEL 1
#define LOWLEVEL 0

/*************************ACM END*************************************/


/*************************HKADC START*********************************/

/*************************************************
 函 数 名   : BSP_HKADC_BatVoltGet
 功能描述   : 返回当前电池电压值
 输入参数   : pslData : 电池电压值
 输出参数   : pslData : 电池电压值
 返 回 值   :0:获取成功
            -1:获取失败
*************************************************/
extern BSP_S32 BSP_HKADC_BatVoltGet(BSP_S32 *ps32Data);

/*****************************************************************************
 函 数 名  : hkadcBatADCRead
 功能描述  : 获取电池电压采样值
 输入参数  : 无
 输出参数  : pTemp：        指向电池温度的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern BSP_S32 BSP_HKADC_BatADCRead(BSP_S32 * pslData);

/*************************************************
*  Function:  hkadcGsmTempRead
*  Description: Read gsm temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:对应hkadc通道的温度值
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcGsmPATempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcWcdmaTempRead
*  Description: Read wcdma temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:对应hkadc通道的温度值
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcWcdmaPATempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcSIMTempRead
*  Description: Read SIM temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:对应hkadc通道的温度值
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcSIMTempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcOLEDTempRead
*  Description: Read OLED temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:对应hkadc通道的温度值
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcOLEDTempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcBatTempRead
*  Description: Read battery temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:对应hkadc通道的温度值
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcBatTempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcBatRefTempRead
*  Description: 通过电池自身NTC电阻来读温度，用来判断电池是否在位*
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:*          
           hkAdcTable:  电压温度换算表；
*  Output:
*         pslData:     存储的电压转换为的温度值；
*  Return:
*         函数执行的状态结果值；
*************************************************/
int hkadcBatRefTempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcPaPowerRead
*  Description: Read gsm PA power *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         N/A
*  Return:
*         N/A
*************************************************/
int hkadcPAPowerRead(int *pslData);


/*************************************************
*  Function:  hkadcTemVoltTrans
*  Description:  *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         N/A
*  Return:
*         N/A
*************************************************/
signed short hkadcTemVoltTrans(unsigned char ucChan, unsigned short usVoltage, unsigned short *hkAdcTable);


/*************************************************
*  Function:  hkadcBatVoltRefSet
*  Description:  * 设置Battery最低和最高电压对应的adc值
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*        minVol:最低电压，minAdc：最低电压对应的adc值，
*	 maxVol:最高电压，maxAdc：最高电压对应的adc值
*  Output:
*         N/A
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcBatVoltRefSet(int minVol, int minAdc, int maxVol, int maxAdc);

/*************************************************
*  Function:  hkadcBatVoltGet
*  Description:  *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*        N/A  
*  Output:
*         pslData : 电池电压值
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcBatVoltGet(int *pslData );



/*************************************************
*  Function:  hkadcBatADCRead
*  Description: Read the battery adc value 
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         pslData:对应hkadc通道adc值
*  Return:
*         HKADC_OK/HKADC_ERROR；
*************************************************/
int hkadcBatADCRead(int* pslData);
/*************************HKADC END***********************************/

/*************************SEC START***********************************/

/*************************SEC START***********************************/

/*************************************************
 函 数 名       : BSP_SEC_Support
 功能描述   : 当前版本是否支持安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
extern BSP_S32 BSP_SEC_Support(BSP_U8 *pu8Data);

/*************************************************
 函 数 名       : BSP_SEC_AlreadyUse
 功能描述   : 查询当前版本是否已经启用安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
extern BSP_S32 BSP_SEC_AlreadyUse(BSP_U8 *pu8Data);

/*************************************************
 函 数 名       : BSP_SEC_Start
 功能描述   : 启用安全启动
 输入参数   :
 输出参数   :
 返 回 值      : OK/ERROR
*************************************************/
extern BSP_S32 BSP_SEC_Start(void);


extern BSP_U32 BSP_SEC_Check(void);


/*************************SEC END*************************************/

/************************Build优化 START******************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#ifndef __BOOTLOADER__
extern int	printf (const char *, ...); /*lint18,modify by z00212992*/
#ifndef __OS_RTOSCK__
#define   DRV_PRINTF     printf
#else
#define   DRV_PRINTF     SRE_Printf
#endif
#endif
#else
int printk(const char *fmt, ...);
#define   DRV_PRINTF     printk
#endif

/*****************************************************************************
 函 数 名  : BSP_DMR_ATAnalyze
 功能描述  : 供NAS查询动态内存占用信息，通过AT命令查询
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 成功0，失败-1
*****************************************************************************/
extern int BSP_DMR_ATAnalyze(void * pstMem,unsigned int uiSize, unsigned int * puiModIdSum);

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetDloadInfo
 功能描述  : Get dload infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/

extern BSP_S32 BSP_DLOAD_GetDloadInfo(unsigned char atCmdBuf[], unsigned int dloadType);

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetAuthorityVer
 功能描述  : Get Authority version
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetAuthorityVer(BSP_U8 *str, BSP_S32 len);

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetAuthorityId
 功能描述  : 获取下载鉴权协议Id
 输入参数  : unsigned char *buf
                          int len
 输出参数  : 无
 返 回 值  : 0:  操作成功；
                      -1：操作失败。
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetAuthorityId(unsigned char *buf, BSP_S32 len);

/*****************************************************************************
 函 数 名  : BSP_ONOFF_DrvPowerOff
 功能描述  : 单板直接下电
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void BSP_ONOFF_DrvPowerOff(void);

/************************************************************************
 * FUNCTION
 *       max_freeblock_size_get
 * DESCRIPTION
 *       get memory max free block size
 * INPUTS
 *       无
 * OUTPUTS
 *       max free block size
 *************************************************************************/
extern int max_freeblock_size_get(void);

#if defined(CHIP_BB_HI6620) || defined (CHIP_BB_HI6210) ||defined(BOARD_FPGA)
/*****************************************************************************
 函 数 名  : BSP_SOCP_SleepIn
 功能描述  : SOCP进入睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
extern BSP_U32 BSP_SOCP_SleepIn(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum);

/*****************************************************************************
 函 数 名  : BSP_SOCP_SleepOut
 功能描述  : SOCP退出睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
extern BSP_U32 BSP_SOCP_SleepOut(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum);
#endif

#if 0
/*****************************************************************************
* 函 数 名  : BSP_MspProcReg
*
* 功能描述  : DRV提供给OM的注册函数
*
* 输入参数  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
extern void BSP_MspProcReg(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc);
#endif
/*****************************************************************************
 函 数 名  : memReadApi
 功能描述  : 按32位宽度查询内存，输出4字节数据。可维可测，V7先打桩
 输入参数  : ulAddress：查询地址空间地址，地址需在内存范围内，否则返回-1。
 输出参数  : pulData：指向查询内容的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MEM_READ (unsigned int ulAddress, unsigned int* pulData);


extern BSP_S32  BSP_StartModeGet(BSP_VOID);

/*****************************************************************************
 函 数 名  : BSP_CHG_StateSet
 功能描述  :chg_state_set(UINT32 ulstate)在A核的映射函数,使能或者禁止充电
 输入参数  :ulState      0:禁止充电
 						  1:使能充电
 输出参数  :无
 返回值：    无
*****************************************************************************/
void BSP_CHG_StateSet(unsigned long ulState);


/************************Build优化 END********************************/

/*************************DPM START***********************************/
/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_DeepSleepForever
 功能描述  : AARM CARM 下电接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None

*****************************************************************************/
extern void BSP_PWRCTRL_DeepSleepForever(void );

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_AhbBakAddrAlloc
 功能描述  : arm上下电功能，需要备份ahb数据，该接口申请数据保存地址和长度
 输入参数  : length     保存申请空间大小的指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
/*V3R2 CS f00164371，这个接口暂时不隔离*/
extern void * BSP_PWRCTRL_AhbBakAddrAlloc(unsigned int * length);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_GetCpuLoad
 功能描述  : arm cpu占有率查询函数
 输入参数  : 无
 输出参数  : ulAcpuLoad:Acpu占有率地址.
             ulCcpuLoad:Ccpu占有率地址.
 返 回 值  : 0/1 。
*****************************************************************************/
extern int BSP_PWRCTRL_GetCpuLoad(unsigned int *ulAcpuLoad,unsigned int *ulCcpuLoad);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_UsbLowPowerEnter
 功能描述  : USB进入低功耗接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void BSP_PWRCTRL_UsbLowPowerEnter(void);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_UsbLowPowerEnter
 功能描述  : USB退出低功耗接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void BSP_PWRCTRL_UsbLowPowerExit(void);

/*************************DPM END*************************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/**********************************NEW PMU INTERFACE BEGIN*******************************************/
/*通信模块供电的 CONSUMER的id枚举*/
typedef enum
{
    CH0_PA_VCC,/* PA的VCC*/
    CH0_PA_VBIAS,/* PA的偏置电源*/
    CH0_SWITCH_VCC,/* 开关2.8V电源*/
    CH0_RFIC_TX_ANA,/* RFIC TX模拟电源*/
    CH0_RFIC_RX_ANA,/* RFIC RX模拟1.8V电源*/
    CH0_FEM_VIO,/*FEM的VIO电源1.8V*/
    CH0_TUNNER_VCC,/*前端Tunner VCC电源*/


    CH1_PA_VCC,/*PA的VCC*/
    CH1_PA_VBIAS,/* PA的偏置电源*/
    CH1_SWITCH_VCC,/* 开关2.8V电源*/
    CH1_RFIC_TX_ANA,/* RFIC TX模拟电源*/
    CH1_RFIC_RX_ANA,/* RFIC RX模拟1.8V电源*/
    CH1_FEM_VIO,/*FEM的VIO电源1.8V*/
    CH1_TUNNER_VCC,/*前端Tunner VCC电源*/
    CH0_RFIC_XO_2P85,
    CH1_RFIC_XO_2P85,
    /*增加RF 电源需修改 MODEM_RF_COMSUMER_ID_BUTT*/
    MODEM_SIM0,
    MODEM_SIM1,
    MODEM_CONSUMER_ID_MAX
}EM_MODEM_CONSUMER_ID;

/*****************************************************************************
 函 数 名  : bsp_pastar_exc_check
 功能描述  : 通信模块检查PASTAR是否有异常接口
 输入参数  : modem_id       卡号
 输出参数  : 无
 返 回 值  : BSP_OK          没有异常
             BSP_ERROR       存在异常
*****************************************************************************/
extern BSP_S32 bsp_pastar_exc_check(PWC_COMM_MODEM_E modem_id);
/*****************************************************************************
 函 数 名  : bsp_modem_voltage_set
 功能描述  : 通信模块设置电压接口
 输入参数  : consumer_id     用户id
             voltage_mv      设置的电压值毫伏
 输出参数  : 无
 返 回 值  : BSP_OK          设置成功
             BSP_ERROR       设置失败
*****************************************************************************/
extern BSP_S32 bsp_modem_voltage_set(EM_MODEM_CONSUMER_ID_E consumer_id, BSP_U32 voltage_mv);
/*****************************************************************************
 函 数 名  : bsp_modem_voltage_get
 功能描述  : 通信模块获取电压接口
 输入参数  : consumer_id     用户id
             voltage_mv      获得的电压值毫伏
 输出参数  : 无
 返 回 值  : BSP_OK          获取成功
             BSP_ERROR       获取失败
*****************************************************************************/
extern BSP_S32 bsp_modem_voltage_get(EM_MODEM_CONSUMER_ID_E consumer_id, BSP_U32 *voltage_mv);

/*****************************************************************************
 函 数 名  : bsp_modem_voltage_list
 功能描述  : 通信模块获取电压设置范围接口
 输入参数  : consumer_id     用户id
             list            电压范围数组
             size            数组大小
 输出参数  : 无
 返 回 值  : BSP_OK          获取成功
             BSP_ERROR       获取失败
*****************************************************************************/
extern BSP_S32 bsp_modem_voltage_list(EM_MODEM_CONSUMER_ID_E consumer_id,BSP_U16 **list, BSP_U32 *size);
/*****************************************************************************
 函 数 名  : bsp_modem_apt_enable
 功能描述  : 通信模块使能APT接口
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : BSP_OK         设置成功
             BSP_ERROR      设置失败
*****************************************************************************/
extern BSP_S32 bsp_modem_apt_enable(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id);

/*****************************************************************************
 函 数 名  : bsp_modem_apt_disable
 功能描述  : 通信模块去使能APT接口
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : BSP_OK         设置成功
             BSP_ERROR      设置失败
*****************************************************************************/
extern BSP_S32 bsp_modem_apt_disable(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id);

/*****************************************************************************
 函 数 名  : bsp_modem_apt_status_get
 功能描述  : 通信模块获取当前APT状态接口
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : PWRCTRL_COMM_ON     APT使能
             PWRCTRL_COMM_OFF    APT未使能
             BSP_ERROR           获取失败
*****************************************************************************/
extern BSP_S32 bsp_modem_apt_status_get(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id);


/*****************************************************************************
 函 数 名  : bsp_modem_mode_config
 功能描述  : 通信模块配置G模或W模接口
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : BSP_OK         配置成功
             BSP_ERROR      配置失败
*****************************************************************************/
extern BSP_S32 bsp_modem_mode_config(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id);

/**********************************NEW PMU INTERFACE END*******************************************/
#endif


/************************V3R2无实现的 START***************************/

/*****************************************************************************
* 函 数 名  : BSP_OM_DspAHBReset

* 功能描述  : ZSP AHB总线复位
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_VOID BSP_OM_DspAHBReset(BSP_VOID);
#else
#endif
/*****************************************************************************
* 函 数 名  : BSP_OM_DspAHBResetCancel
* 功能描述  : ZSP AHB总线解复位
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_VOID BSP_OM_DspAHBResetCancel(BSP_VOID);
#else
#endif

/************************V3R2无实现的 END*****************************/

#ifdef __cplusplus
    #if __cplusplus
        }    
    #endif   
#endif       
             
             
#endif /* end of __DRV_INTERFACE_INNER_H__ */
