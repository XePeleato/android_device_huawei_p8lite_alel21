/*
 * bsp_ipc.h
 * description: ipc module interface
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _BSP_IPC_H_
#define _BSP_IPC_H_
#ifdef __ASSEMBLY__
#include <bsp_memmap.h>
#define  IPC_CPU_RAW_INT_M3   0x420
/* c->m send ipc_int  addr & bit  */
#define  PM_C2M_IPC_ADDR       (HI_IPCM_REGBASE_ADDR+IPC_CPU_RAW_INT_M3)
#define  PM_C2M_IPC_BIT        (0x1<<17)
/* a->m send ipc_int  addr & bit  */
#define  PM_A2M_IPC_ADDR       (HI_IPCM_REGBASE_ADDR+IPC_CPU_RAW_INT_M3)
#define  PM_A2M_IPC_BIT        (0x1<<16)

#else

#include <product_config.h>
#include <osl_common.h>
#include <drv_ipc.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef void (*voidfun)(u32);
struct ipc_entry {
	voidfun routine;
	u32	arg;
};

#ifdef CONFIG_MODULE_IPC
/**
  \fn bsp_ipc_init
  \brief ipc module initial
  \return none
 */
void bsp_ipc_init(void);

/**
 \fn bsp_ipc_sem_create
 \brief create semaphore
 \param u32_signal_num the number of semaphore we want to create
 \return OK or error
 */
s32 bsp_ipc_sem_create(u32 u32_signal_num);

/**
 \fn bsp_ipc_sem_delete
 \brief remove semaphore
 \param u32_signal_num the number of semphore we want to delete
 \return ok or error
 */
s32 bsp_ipc_sem_delete(u32 u32_signal_num);

/**
 \fn bsp_ipc_int_enable
 \brief enable a interrupt
 \param ul_lvl the interrupt number we want to enable, from 0 to 31
 \return ok or error
 */
s32 bsp_ipc_int_enable(IPC_INT_LEV_E ul_lvl);

/**
 \fn bsp_ipc_int_disable
 \brief disable a interrupt
 \param ul_lvl the interrupt number we want to disable, from 0 to 31
 \return ok or error
 */
s32 bsp_ipc_int_disable(IPC_INT_LEV_E ul_lvl);

/**
 \fn bsp_ipc_int_connect
 \brief regist a interrupt to the system
 \param ul_lvl the interrupt number we want to regist, from 0 to 31
 \param routine the SR function
 \param param the paramter of the SR function
 \return ok or error
 */
s32 bsp_ipc_int_connect(IPC_INT_LEV_E ul_lvl, voidfun routine, u32 param);

/**
 \fn bsp_ipc_int_disconnect
 \brief unregist a interrupt from the system
 \param ul_lvl the interrupt number we want to unregist, from 0 to 31
 \param routine the SR function
 \param param the paramter of the SR function
 \return ok or error
 */
s32 bsp_ipc_int_disconnect(IPC_INT_LEV_E ul_lvl, voidfun routine, u32 param);

/**
 \fn bsp_ipc_int_send
 \param dst_core the core should receive and process the interrupt
 \param ul_lvl the interrupt number we want to send, from 0 to 31
 \return ok or error
 */
s32 bsp_ipc_int_send(IPC_INT_CORE_E dst_core, IPC_INT_LEV_E ul_lvl);

/*****************************************************************************
 * 函 数 名     : bsp_ipc_sem_take
 *
 * 功能描述  : 获取信号量
 *
 * 输入参数  : u32SignalNum 要获取的信号量ID
s32timeout  超时时间,tick为单位，一个tick为10ms
 * 输出参数  : 无
 *
 * 返 回 值      : OK&ERROR
 *
 * 修改记录  : 2013年1月9日 lixiaojie
 *****************************************************************************/

s32 bsp_ipc_sem_take(u32 u32_signal_num, s32 s32timeout);

/*****************************************************************************
 * 函 数 名     : bsp_ipc_sem_give
 *
 * 功能描述  : 释放信号量
 *
 * 输入参数  : u32_signal_num 要获取的信号量
s32timeout  以10ms为单位的超时时间
 * 输出参数  : 无
 *
 * 返 回 值      :OK&&ERROR
 *
 * 修改记录  : 2013年1月9日 lixiaojie
 *****************************************************************************/

s32 bsp_ipc_sem_give(u32 u32_signal_num);

/*****************************************************************************
* 函 数 名     : bsp_ipc_spin_lock
*
* 功能描述  : 查询等待获取信号量,在使用本函数前需要锁中断，
			建议使用带锁中断功能的bsp_ipc_spin_lock_irqsave函数
*
* 输入参数  : u32_signal_num 要获取的信号量
* 输出参数  : 无
*
* 返 回 值      :ERROR&OK
*
* 修改记录  :  2013年1月8日 lixiaojie
*****************************************************************************/

s32 bsp_ipc_spin_lock(u32 u32_signal_num);

/*****************************************************************************
* 函 数 名      : bsp_ipc_spin_unlock
*
* 功能描述  : 释放信号量，与bsp_ipc_spin_lock搭配使用，用后需要开中断，
			建议使用带开中断功能的bsp_ipc_spin_unlock_irqrestore函数
*
* 输入参数  : u32SignalNum  要释放的信号量
* 输出参数  : 无
*
* 返 回 值      : OK&ERROR
*
* 修改记录  :  2013年1月9日 lixiaojie
*****************************************************************************/

s32 bsp_ipc_spin_unlock(u32 u32_signal_num);

/*****************************************************************************
* 函 数 名     : bsp_ipc_spin_lock_irqsave
*
* 功能描述  : 锁中断并查询等待获取信号量
*
* 输入参数  : u32_signal_num 要获取的信号量
	flags:锁中断标志，与bsp_ipc_spin_unlock_irqrestore配对使用，
	这两个宏只能在同一个函数内部同时前后被调用，
	参考spin_lock_irqsave用法
* 输出参数  : 无
*
* 返 回 值      :ERROR&OK
*
* 修改记录  :  2013年5月8日 lixiaojie
*****************************************************************************/

#define bsp_ipc_spin_lock_irqsave(u32_signal_num, flags)  \
	do { \
		local_irq_save(flags); \
		(void)bsp_ipc_spin_lock(u32_signal_num); \
	} while (0)

/*****************************************************************************
* 函 数 名     : bsp_ipc_spin_unlock_irqrestore
*
* 功能描述  :开中断并释放获得的信号量
*
* 输入参数  : u32SignalNum 要获取的信号量
flags:锁中断标志，与bsp_ipc_spin_lock_irqsave配对使用，
这两个宏只能在同一个函数内部同时前后被调用，
参考spin_unlock_irqrestore用法
* 输出参数  : 无
*
* 返 回 值      :ERROR&OK
*
* 修改记录  :  2013年5月8日 lixiaojie
*****************************************************************************/

#define bsp_ipc_spin_unlock_irqrestore(u32_signal_num, flags) \
	do { \
		(void)bsp_ipc_spin_unlock(u32_signal_num); \
		local_irq_restore(flags); \
	} while (0)
#else
static inline s32 bsp_ipc_init(void) {return 0; }
static inline s32 bsp_ipc_sem_create(u32 u32_signal_num) {return 0; }
static inline s32 bsp_ipc_sem_delete(u32 u32_signal_num) {return 0; }
static inline s32 bsp_ipc_int_enable(IPC_INT_LEV_E ul_lvl) {return 0; }
static inline s32 bsp_ipc_int_disable(IPC_INT_LEV_E ul_lvl) {return 0; }
static inline s32 bsp_ipc_int_connect(IPC_INT_LEV_E ul_lvl,
			voidfun routine, u32 parameter) {return 0; }
static inline s32 bsp_ipc_int_disconnect(IPC_INT_LEV_E ul_lvl,
			voidfun routine, u32 parameter) {return 0; }
static inline s32 bsp_ipc_int_send(IPC_INT_CORE_E en_dst_core,
			IPC_INT_LEV_E ul_lvl) {return 0; }
static inline s32 bsp_ipc_sem_take(u32 u32_signal_num, s32 s32timeout)
{
	return 0;
}

static inline s32 bsp_ipc_sem_give(u32 u32_signal_num) {return 0; }
static inline s32 bsp_ipc_spin_lock(u32 u32_signal_num) {return 0; }
static inline s32 bsp_ipc_spin_unlock(u32 u32_signal_num) {return 0; }
#define bsp_ipc_spin_lock_irqsave(u32_signal_num, flags)  \
	do {} while (0)
#define bsp_ipc_spin_unlock_irqrestore(u32_signal_num, flags) \
	do {} while (0)

#endif
/*****************************************************************************
* 函 数 名     : bsp_int_send_info
*
* 功能描述  :查看往每个核发送每个中断次数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值      :无
*
* 修改记录  :  2013年5月10日 lixiaojie
*****************************************************************************/

void bsp_int_send_info(void);

#ifdef __CMSIS_RTOS
/*****************************************************************************
* 函 数 名	: bsp_ipc_suspend
*
* 功能描述	:IPC设备下电前保存寄存器
*
* 输入参数	: 无
* 输出参数	: 无
*
* 返 回 值		:
*
* 修改记录	:  2013年6月14日 lixiaojie
*****************************************************************************/
void bsp_ipc_suspend(void);

/**
 \fn bsp_ipc_resume
 \brief after ipc device power on, resume the register
 \return 0:success, -1:fail
 */
void bsp_ipc_resume(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /*#ifdef __ASSEMBLY__*/

#endif /* end #define _BSP_IPC_H_*/
