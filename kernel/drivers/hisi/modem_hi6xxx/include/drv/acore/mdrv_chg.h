/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __MDRV_ACORE_CHG_H__
#define __MDRV_ACORE_CHG_H__

#include "mdrv_public.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef enum CHARGING_STATE_E_tag
{
        CHARGING_INIT = -1,
        NO_CHARGING_UP = 0,  /*开机未充电*/
        CHARGING_UP ,              /*开机正充电*/
        NO_CHARGING_DOWN ,  /*关机未充电*/
        CHARGING_DOWN         /*关机未充电*/
}CHARGING_STATE_E;

typedef enum BATT_LEVEL_E_tag
{
        BATT_INIT = -2,
        BATT_LOW_POWER =-1,    /*电池低电*/
        BATT_LEVEL_0,          /*0格电池电量*/
        BATT_LEVEL_1,          /*1格电池电量*/
        BATT_LEVEL_2,          /*2格电池电量*/
        BATT_LEVEL_3,          /*3格电池电量*/
        BATT_LEVEL_4,          /*4格电池电量*/
        BATT_LEVEL_MAX
}BATT_LEVEL_E;

typedef struct BATT_STATE_tag
{
    CHARGING_STATE_E  charging_state;
    BATT_LEVEL_E      battery_level;
}BATT_STATE_S;

/*****************************************************************************
 函 数 名  : mdrv_misc_get_battery_state
 功能描述  :获取底层电池状态信息
 输入参数  :battery_state 电量信息
 输出参数  :battery_state 电量信息
 返回值：   0 操作成功
                         -1操作失败

*****************************************************************************/
int mdrv_misc_get_battery_state(BATT_STATE_S *battery_state);


/*****************************************************************************
 函 数 名  : mdrv_misc_get_charging_status
 功能描述  :查询目前是否正在充电中
 输入参数  :无
 输出参数  :无
 返回值：   0 未充电
                          1 充电中
*****************************************************************************/
int mdrv_misc_get_charging_status(void);


/*****************************************************************************
 函 数 名  : mdrv_misc_set_charge_state
 功能描述  :使能或者禁止充电
 输入参数  :ulState      0:禁止充电
 						1:使能充电
 输出参数  :无
 返回值：    无
*****************************************************************************/
void mdrv_misc_set_charge_state(unsigned long ulState);

/*****************************************************************************
 函 数 名  : mdrv_misc_get_charge_state
 功能描述  :查询
 输入参数  :
 输出参数  :无
 返回值：    无
*****************************************************************************/
int mdrv_misc_get_charge_state(void);


/*****************************************************************************
 函 数 名  : mdrv_misc_sply_battery
 功能描述  :查询
 输入参数  :
 输出参数  :无
 返回值：    无
*****************************************************************************/
int mdrv_misc_sply_battery(void);


/*****************************************************************************
 函 数 名  : mdrv_misc_get_cbc_state
 功能描述  : 返回电池状态和电量
 输入参数  :pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
                          pucBcl  0:电量不足或没有与电池连接
 输出参数  : pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
                          pucBcl  0:电量不足或没有与电池连接
 返回值：   0 操作成功
                         -1操作失败

*****************************************************************************/
int mdrv_misc_get_cbc_state(unsigned char *pusBcs,unsigned char *pucBcl);


#ifdef __cplusplus
}
#endif
#endif

