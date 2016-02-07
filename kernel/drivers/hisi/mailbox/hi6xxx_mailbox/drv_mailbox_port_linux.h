

#ifndef _DRV_MAILBOX_PORT_LINUX_H_
#define _DRV_MAILBOX_PORT_LINUX_H_

#include <linux/printk.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*虚实地址转换接口定义*/
#ifndef _DRV_LLT_
//#include <mach/hardware.h>

/*ACPU实地址向虚拟地址映射的转换接口*/
#define MEM_CORE_SHARE_PHY2VIRT(phy)         HISI_VA_ADDRESS(phy)
#define MEM_CORE_SHARE_VIRT2PHY(virt)        HISI_PA_ADDRESS(virt)

#else

/*ACPU实地址向虚拟地址映射的转换接口*/
#define MEM_CORE_SHARE_PHY2VIRT(phy)         (phy)
#define MEM_CORE_SHARE_VIRT2PHY(virt)        (virt)
#endif

/*邮箱通道，功能，核等相关配置接口定义*/
/*表明此CPU的ID*/
#define MAILBOX_LOCAL_CPUID                         MAILBOX_CPUID_ACPU

/*定义C核平台相关的 邮箱通道 总数，关心收发通道 */
#define MAILBOX_CHANNEL_NUM \
    ( MAILBOX_CHANNEL_BUTT(ACPU, HIFI)   \
    + MAILBOX_CHANNEL_BUTT(ACPU, MCU)    \
    + MAILBOX_CHANNEL_BUTT(ACPU, CCPU)   \
    + MAILBOX_CHANNEL_BUTT(HIFI, ACPU)   \
    + MAILBOX_CHANNEL_BUTT(MCU,  ACPU)   \
    + MAILBOX_CHANNEL_BUTT(CCPU,  ACPU)  \
    )

/*定义C核平台相关的 邮件 总数 , 只关心接收通道*/
#define MAILBOX_USER_NUM    \
    ( MAILBOX_USER_BUTT(HIFI, ACPU, MSG) \
    + MAILBOX_USER_BUTT(MCU,  ACPU, MSG) \
    + MAILBOX_USER_BUTT(CCPU,  ACPU, MSG) \
    + MAILBOX_USER_BUTT(CCPU,  ACPU, IFC) \
    + MAILBOX_USER_BUTT(MCU,  ACPU, IFC) \
    )

/*打印输出相关配置定义*/
/*控制台打印输出接口*/
#ifdef _DRV_LLT_
extern int printf (__const char *__restrict __format, ...);
#define mailbox_out(p)                              (printf p)
#else
#define mailbox_out(p)                              (printk p)
#endif

/*定义宏控制是否打印行数*/
#define _MAILBOX_LINE_                              __LINE__

/*定义宏控制是否打印文件名,(不打印文件名可节省代码空间)*/
#define _MAILBOX_FILE_                              (void*)(0) /*__FILE__*/

#define MAILBOX_LOG_LEVEL                           MAILBOX_LOG_INFO


/*邮箱调试功能相关配置定义*/
/*表明在此CPU上打开可维可测功能*/
#ifndef MAILBOX_OPEN_MNTN
#define MAILBOX_OPEN_MNTN
#endif

/*可维可测记录的最大传送ID数*/
#define MAILBOX_RECORD_USEID_NUM                    (64)
/*可维可测记录的最近记录个数*/
#define MAILBOX_RECORD_RECENT_NUM                   (2)

#define RT  "\n"   /*打印回车符号*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /*_DRV_MAILBOX_LINUX_H_*/

