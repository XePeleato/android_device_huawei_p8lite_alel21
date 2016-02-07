/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_ipcm.c
  版 本 号   : 初稿
  作    者   : c61362
  生成日期   : 2012年3月2日
  最近修改   :
  功能描述   : 底软给上层软件封装的接口层
  修改历史   :
  1.日    期   : 2012年3月2日
    作    者   : c61362
    修改内容   : 新建Drvinterface.c

  2.日    期   : 2013年2月19日
    作    者   : 蔡喜 220237
    修改内容   : 由Drvinterface.c拆分所得

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "mdrv_ipc.h"
#include "bsp_drv_ipc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 * 函 数 名  : DRV_IPC_SEMCREATE
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
int mdrv_ipc_sem_create (unsigned int u32SignalNum)
{
    return BSP_IPC_SemCreate(u32SignalNum);
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMDELETE
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
int mdrv_ipc_sem_delete(unsigned int u32SignalNum)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_IPC_SemDelete(u32SignalNum);
#endif

#if (defined BSP_CORE_APP)
    return 0;
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTENABLE
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
int mdrv_ipc_int_enable (IPC_INT_LEV_E ulLvl)
{
    return BSP_IPC_IntEnable(ulLvl);
}

/*****************************************************************************
 * 函 数 名  : DRV_IPC_INTDISABLE
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
int mdrv_ipc_int_disable (IPC_INT_LEV_E ulLvl)
{
    return BSP_IPC_IntDisable(ulLvl);
}

/*****************************************************************************
 * 函 数 名  : DRV_IPC_INTCONNECT
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
int mdrv_ipc_int_connect (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, unsigned int parameter)
{
    return BSP_IPC_IntConnect(ulLvl, routine, parameter);
}

/*****************************************************************************
 * 函 数 名  : DRV_IPC_INTDISCONNECT
 *
 * 功能描述  : 去注册某个中断
 *
 * 输入参数  :
               BSP_U32 ulLvl 要去注册的中断号，取值范围0～31
               VOIDFUNCPTR routine 中断服务程序
 *             BSP_U32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
int mdrv_ipc_int_disconnect (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, unsigned int parameter)
{
    return BSP_IPC_IntDisonnect(ulLvl, routine, parameter);
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTSEND
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
int mdrv_ipc_int_send(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl)
{
    return BSP_IPC_IntSend(enDstCore, ulLvl);
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMTAKE
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
int mdrv_ipc_sem_take (unsigned int u32SignalNum,int s32timeout)
{
    return BSP_IPC_SemTake(u32SignalNum, s32timeout);
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMGIVE
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
int mdrv_ipc_sem_give(unsigned int u32SignalNum)
{
    BSP_IPC_SemGive(u32SignalNum);
}

/*****************************************************************************
* 函 数 名  : DRV_SPIN_LOCK
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
void mdrv_ipc_spin_lock(unsigned int u32SignalNum)
{
    BSP_IPC_SpinLock (u32SignalNum);
}

/*****************************************************************************
* 函 数 名  : DRV_SPIN_UNLOCK
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
void mdrv_ipc_spin_unlock(unsigned int u32SignalNum)
{
    BSP_IPC_SpinUnLock(u32SignalNum);
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

