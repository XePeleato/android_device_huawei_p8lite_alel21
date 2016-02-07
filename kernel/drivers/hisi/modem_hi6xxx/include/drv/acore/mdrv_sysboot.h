#ifndef __MDRV_ACORE_SYSBOOT_H__
#define __MDRV_ACORE_SYSBOOT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <mdrv_sysboot_commmon.h>

typedef enum DRV_SHUTDOWN_REASON_tag_s
{
	DRV_SHUTDOWN_POWER_KEY,             /* 长按 Power 键关机          */
	DRV_SHUTDOWN_BATTERY_ERROR,         /* 电池异常                   */
	DRV_SHUTDOWN_LOW_BATTERY,           /* 电池电量低                 */
	DRV_SHUTDOWN_TEMPERATURE_PROTECT,   /* 过温保护关机               */
	DRV_SHUTDOWN_CHARGE_REMOVE,         /* 关机充电模式下，拔除充电器 */
	DRV_SHUTDOWN_UPDATE,                /* 关机并进入升级模式         */
	DRV_SHUTDOWN_RESET,                 /* 系统软复位                 */
	DRV_SHUTDOWN_BUTT
}DRV_SHUTDOWN_REASON_E;

/*****************************************************************************
* 函 数 名  : mdrv_sysboot_shutdown
*
* 功能描述  : Modem子系统关机接口
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*            1 PS/TAF上层只需关注过温关机场景。
*            2 对于按键关机、低电关机、关机充电时拔出充电器关机等MBB解决方案的场景，使用底软内部接口，
*              不要调用mdrv_sysboot_shutdown接口。
*
*****************************************************************************/
void mdrv_sysboot_shutdown(void);

/*****************************************************************************
* 函 数 名  : mdrv_sysboot_restart
*
* 功能描述  : 复位Modem子系统
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*            1 在phone形态下，该函数实现对Modem子系统（C核）的单独复位；MBB形态下，则是对全系统复位；
*            2 ccore和acore都可以调用，最终结果一致，内部函数实现有差异；
*            3 异常情况下，如果需要重启modem时，请使用system_error接口；严禁直接调用该接口。
*
*****************************************************************************/
void mdrv_sysboot_restart(void);

/*****************************************************************************
* 函 数 名  : mdrv_set_modem_state
*
* 功能描述  : 设置Modem状态（Ready或者Off）
*
* 输入参数  : unsigned int state  Modem状态，MODEM_NOT_READY(0), MODEM_READY(1)
* 输出参数  :
*
* 返 回 值  : 0，设置成功；1，设置不成功
*
* 其它说明  :1 taf通过该接口设置Modem状态。
*            2 同时存在多个Modem时，上层必须在所有Modem状态均确认后再调用该接口设置Modem状态。上层调用该接口设置MODEM_READY
*            前，应用层获取到的状态为Modem Off。
*
*****************************************************************************/
int  mdrv_set_modem_state(unsigned int state);

/*****************************************************************************
* 函 数 名  : mdrv_sysboot_register_reset_notify
*
* 功能描述  : 向reset模块注册通知modem C核 reset发生的通知函数
*
* 输入参数  : const char *pname   上层组件注册的名字，最长9个字符（不包括结束符），底软负责存储
*             pdrv_reset_cbfun pcbfun  回调函数指针
*             int userdata      上层组件数据，在调用回调函数时，作为入参传给注册者
*             int priolevel  回调函数调用优先级，参考emum DRV_RESET_CALLCBFUN_PRIO定义，值越小优先级越高
* 输出参数  :
*
* 返 回 值  : 0，注册成功；1，注册不成功
*
* 其它说明  :
*
*****************************************************************************/
typedef int (*pdrv_reset_cbfun)(DRV_RESET_CB_MOMENT_E enparam, int userdata);
int mdrv_sysboot_register_reset_notify(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel);

/*****************************************************************************
 函 数 名  : hifireset_regcbfunc
 功能描述  : 提供上层应用程序注册HIFI单独复位的回调接口函数。
 输入参数  : pname：上层组件注册的名字，注意不包括结束符最长9个字符，底软负责存储。
             pcbfun：回调函数指针。
             puserdata:上层组件数据，在调用回调函数时，作为入参传给用户。
             priolevel: 回调函数调用优先级，0-49
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int hifireset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel);
#define DRV_HIFIRESET_REGCBFUNC(pname,pcbfun, userdata, priolevel)\
                hifireset_regcbfunc(pname,pcbfun, userdata, priolevel)

#ifdef __cplusplus
}
#endif
#endif
