

#ifndef __DRV_TELE_MNTN_PLATFORM_H__
#define __DRV_TELE_MNTN_PLATFORM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "drv_tele_mntn_common.h"
#include  "global_ddr_map.h"
#if defined(BSP_CORE_MODEM)
#include  <string.h>
#include  <osal.h>
#include  "mntnDrv.h"
#include  "BSP_GLOBAL.h"
#include  "mdrv_ipc_comm.h"
#include  "mdrv_rtc_common.h"
#elif defined (BSP_CORE_APP)
#include  <linux/string.h>
#include  <linux/kernel.h>
#include  <linux/slab.h>
#include  <asm/io.h>
#include  <linux/spinlock.h>
#include  <linux/hisi/hi6xxx-iomap.h>
#include  "bsp_drv_ipc.h"
#elif defined (BSP_CORE_CM3)
#include  "drv_mailbox_ipc.h"
#include  "mntn_if.h"
#include  "excDrv.h"
#include  "mcu_print.h"
#include  "v_int.h"
#include  "v_string.h"
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#if defined(BSP_CORE_MODEM) /*C核*/

#define TELE_MNTN_INTLOCK_INIT(lock)
#define TELE_MNTN_INT_LOCK(lock, flag)                  ((flag)=(u32_t)OSAL_IntLock())
#define TELE_MNTN_INT_UNLOCK(lock, flag)                OSAL_IntUnLock((s32_t)(flag))

#define TELE_MNTN_IPC_SEM_LOCK(ipc_sem_num)             mdrv_ipc_spin_lock(ipc_sem_num)
#define TELE_MNTN_IPC_SEM_UNLOCK(ipc_sem_num)           mdrv_ipc_spin_unlock(ipc_sem_num)

#define TELE_MNTN_IPC_INT_REGISTER(ipc_int_num, func, para)        mdrv_ipc_int_connect((ipc_int_num), (func), (para))
#define TELE_MNTN_IPC_INT_SEND(target, ipc_int_num)     (void_t)mdrv_ipc_int_send((target),(ipc_int_num))

#define TELE_MNTN_PHY_TO_VIRT(addr, size)               ((u8_t *)(addr))

#define TELE_MNTN_AXI_PHY_TO_VIRT(addr, size)           ((u8_t *)(addr))

#define TELE_MNTN_SLICE_TIME_GET()                      omTimerGet()

#define TELE_MNTN_RTC_TIME_GET()                        mdrv_rtc_get_value()

#define TELE_MNTN_TRACE_OUT(_ret, _para0)               BSP_TRACE((BSP_LOG_LEVEL_E)BSP_LOG_LEVEL_ERROR,BSP_MODU_MNTN,"ret=0x%08x, para0=0x%08x\r\n",(u32_t)(_ret),(u32_t)(_para0))

#define TELE_MNTN_MEMSET(m, c, size)                    (void_t)memset((void_t *)(m),(s32_t)(c),(u32_t)(size))
#define TELE_MNTN_MEMCPY(d, s, size)                    (void_t)memcpy((void_t *)(d),(void_t *)(s),(u32_t)(size))

#define TELE_MNTN_CUR_CPUID                             IPC_CORE_CCPU

#elif defined (BSP_CORE_APP) /*A核*/

#define TELE_MNTN_INTLOCK_INIT(lock)                    spin_lock_init((spinlock_t *)(lock))
#define TELE_MNTN_INT_LOCK(lock, flag)                  spin_lock_irqsave((spinlock_t *)(lock), flag)
#define TELE_MNTN_INT_UNLOCK(lock, flag)                spin_unlock_irqrestore((spinlock_t *)(lock),(udy_t)(flag))

#define TELE_MNTN_IPC_SEM_LOCK(ipc_sem_num)             BSP_IPC_SpinLock(ipc_sem_num)
#define TELE_MNTN_IPC_SEM_UNLOCK(ipc_sem_num)           BSP_IPC_SpinUnLock(ipc_sem_num)

#define TELE_MNTN_IPC_INT_REGISTER(ipc_int_num, func, para)        BSP_IPC_IntConnect((ipc_int_num), (func), (para))
#define TELE_MNTN_IPC_INT_SEND(target, ipc_int_num)     (void_t)BSP_IPC_IntSend((target),(ipc_int_num))

#define TELE_MNTN_PHY_TO_VIRT(addr, size)               ((u8_t*)ioremap((addr),(size)))

#define TELE_MNTN_AXI_PHY_TO_VIRT(addr, size)           ((u8_t *)HISI_VA_ADDRESS(addr))

#define TELE_MNTN_RTC_TIME_GET()                        get_rtc_timer_value()

#define TELE_MNTN_SLICE_TIME_GET()                      omTimerGet()

#define TELE_MNTN_TRACE_OUT(_ret, _para0)               printk(KERN_ERR"ret=%u, para0=%u\r\n",(u32_t)(_ret),(u32_t)(_para0))

#define TELE_MNTN_MEMSET(m, c, size)                    (void_t)hisi_io_memset((void_t *)(m),(s32_t)(c),(u32_t)(size))
#define TELE_MNTN_MEMCPY(d, s, size)                    (void_t)hisi_io_memcpy((void_t *)(d),(void_t *)(s),(u32_t)(size))

#define TELE_MNTN_CUR_CPUID                             IPC_CORE_ACPU

#elif defined (BSP_CORE_CM3)  /*M核*/

#define TELE_MNTN_INTLOCK_INIT(lock)
#define TELE_MNTN_INT_LOCK(lock, flag)                  ((flag)=(s32_t)VOS_SplIMP())
#define TELE_MNTN_INT_UNLOCK(lock, flag)                VOS_Splx(flag)

#define TELE_MNTN_IPC_SEM_LOCK(ipc_sem_num)             BSP_IPC_SpinLock(ipc_sem_num)
#define TELE_MNTN_IPC_SEM_UNLOCK(ipc_sem_num)           BSP_IPC_SpinUnLock(ipc_sem_num)

#define TELE_MNTN_IPC_INT_REGISTER(ipc_int_num, func, para)        BSP_IPC_RegIntSrc((ipc_int_num), (func), (para))
#define TELE_MNTN_IPC_INT_SEND(target, ipc_int_num)     (void_t)BSP_IPC_SendInt((target),(ipc_int_num))

#define TELE_MNTN_PHY_TO_VIRT(addr, size)               ((u8_t *)IO_REMAP(addr))

#define TELE_MNTN_AXI_PHY_TO_VIRT(addr, size)           ((u8_t *)(addr))

#define TELE_MNTN_SLICE_TIME_GET()                      mntn_get_slice_time()
#define TELE_MNTN_RTC_TIME_GET()                        mntn_get_rtc_time()

#define TELE_MNTN_TRACE_OUT(_ret, _para0)               mcu_err2(PWC_MODULE_PRINT_ID,tele_mntn_log_out,(_ret),(_para0))

#define TELE_MNTN_MEMSET(m, c, size)                    (void_t)memset((void_t *)(m),(s32_t)(c),(u32_t)(size))
#define TELE_MNTN_MEMCPY(d, s, size)                    (void_t)memcpy((void_t *)(d),(void_t *)(s),(u32_t)(size))

#define TELE_MNTN_CUR_CPUID                             IPC_CORE_MCU

#endif
/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


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
#if defined (BSP_CORE_APP)
extern unsigned int omTimerGet(void);
extern unsigned int get_rtc_timer_value(void);
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of drv_tele_mntn_platform.h */
