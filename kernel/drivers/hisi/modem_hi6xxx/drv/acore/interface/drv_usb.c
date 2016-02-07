/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_usb.c
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
#if (defined BSP_CORE_APP)
#include <linux/module.h>
#endif
#include "BSP.h"
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#include "stdio.h"
#include "sys_config.h"
#include "BSP_GLOBAL.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern void ErrlogRegFunc(MNTN_ERRLOGREGFUN ptr);
extern void * getSystemResetInfo(void);
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/



void MNTN_ERRLOG_REG_FUNC(MNTN_ERRLOGREGFUN pRegFunc)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    ErrlogRegFunc(pRegFunc);
#endif

#if (defined BSP_CORE_APP)
    return;   /* 打桩 */
#endif
}
void * GET_SYSTEM_RESET_INFO(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return getSystemResetInfo();
#endif

#if (defined BSP_CORE_APP)
    return (void *)0;   /* 打桩 */
#endif
}

/*****************************************************************************
* 函 数 名  : BSP_OM_DspAHBResetCancel
* 功能描述  : ZSP AHB总线解复位
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
void DRV_DSP_AHB_RESET_CANCEL(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    /* BSP_OM_DspAHBResetCancel(); */
	BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_USB,"stub:DRV_DSP_AHB_RESET_CANCEL\n");
    return;
#endif

#if (defined BSP_CORE_APP)
    return;   /* 打桩 */
#endif
}

/*****************************闪电卡相关START*******************************/

/*****************************************************************************
 函 数 名  : DRV_AT_SETAPPDAILMODE
 功能描述  : 闪电卡版本控制应用进行拨号或断开拨号连接
 输入参数  : unsigned int ulStatus - 进行拨号或断开拨号指示
 输出参数  : 无
 返 回 值  : VOID
*****************************************************************************/
VOID DRV_AT_SETAPPDAILMODE(unsigned int ulStatus)
{
    return;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if( FEATURE_HILINK == FEATURE_ON )
    rndis_app_event_dispatch(ulStatus);
#else
    DRV_PRINTF("Function DRV_AT_SETAPPDAILMODE unsupport!\n");
#endif
#endif
#endif
}

/*****************************闪电卡相关END**********************************/


/************************************************************************
 * FUNCTION
 *       PDP_ACT_DRV_CALLBACK
 * DESCRIPTION
 *       PDP激活后调用底软的回调函数，原本用作加载符号表信息，现在打桩
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
int PDP_ACT_DRV_CALLBACK(void)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : BSP_USB_UdiagValueCheck
 功能描述  : 本接口用于检查NV项中USB形态值的合法性.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  :
*****************************************************************************/
 int BSP_USB_UdiagValueCheck(unsigned int DiagValue)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;
#endif

#if (defined BSP_CORE_APP)
    return 1;
#endif
}

/********************************************************
函数说明：协议栈查询HSIC枚举状态
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  1：枚举完成
          0：枚举未完成
********************************************************/
unsigned int BSP_GetHsicEnumStatus(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return BSP_FALSE;
#endif
}

/********************************************************
函数说明：协议栈注册HSIC去使能通知回调函数
函数功能:
输入参数：pFunc: HSIC去使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
unsigned int BSP_HSIC_RegUdiDisableCB(HSIC_UDI_DISABLE_CB_T pFunc)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return BSP_OK;
#endif
}

/********************************************************
函数说明：协议栈注册HSIC使能通知回调函数
函数功能:
输入参数：pFunc: HSIC使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
unsigned int BSP_HSIC_RegUdiEnableCB(HSIC_UDI_ENABLE_CB_T pFunc)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return BSP_OK;
#endif
}

/*******************************************************************************
 函 数 名  : USB_otg_switch_get
 功能描述  : 用于BALONG和K3对接时MODEM侧USB PHY状态查询
 输入参数  : 无
 输出参数  : 返回USB PHY开启状态

 返 回 值  : 0:  操作成功；
             -1：操作失败。
******************************************************************************/
int USB_otg_switch_get(UINT8 *pvalue)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
	return BSP_ERROR;
#endif
}

/******************************************************************************
 函 数 名  : USB_otg_switch_set
 功能描述  : 用于BALONG和K3对接时MODEM侧开启或关闭USB PHY
 输入参数  : 开启或关闭
 输出参数  : 无

 返 回 值  : 0:  操作成功；
             -1：操作失败。
******************************************************************************/
int USB_otg_switch_set(UINT8 value)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
	return BSP_ERROR;
#endif
}
int USB_otg_switch_signal_set(UINT8 group,UINT8 pin, UINT8 value )
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
	return BSP_ERROR;
#endif
}

unsigned int pcsc_usim_ctrl_cmd(unsigned int CmdTpye, unsigned int Result,
                                        unsigned char *Buf, unsigned int Length)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return 10;  /* 参见drv_misc_if.c第371行 */
#endif
}

void pcsc_usim_int(pFunAPDUProcess pFun1, GetCardStatus pFun2)
{
#if (defined BSP_CORE_APP)
    return;
#endif
}

#if (defined BSP_CORE_APP)
EXPORT_SYMBOL(pcsc_usim_ctrl_cmd);
EXPORT_SYMBOL(pcsc_usim_int);
#endif
void BSP_NCM_Write_Ready(void)
{
	/* 打桩 */
    return;
}

unsigned int BSP_USB_RegIpsTraceCB(USB_IPS_MNTN_TRACE_CB_T pFunc)
{
    return 0;
}

/*move from drv_usb_if.c begin*/

int BSP_USB_GetDiagModeValue(unsigned char *dial_mode, unsigned char *cdc_spec)
{
    return 1;
}


int BSP_USB_GetLinuxSysType(void)
{
    return -1;
}


unsigned char BSP_USB_GetPortMode(char* PsBuffer, unsigned int* Length )
{
    return 1;
}


unsigned int BSP_USB_GetU2diagDefaultValue(void)
{
    return 0;
}


int BSP_USB_NASSwitchGatewayRegFunc(USB_NET_DEV_SWITCH_GATEWAY switchGwMode)
{
    return 0;
}


int BSP_USB_SetPid(unsigned char u2diagValue)
{
    return 1;
}

/*****************************************************************************
   与OAM的接口实现，包含打桩实现
*****************************************************************************/






/*****************************************************************************
   与TTF的接口实现，包含打桩实现
*****************************************************************************/


unsigned int USB_ETH_DrvSetRxFlowCtrl(unsigned int ulParam1, unsigned int ulParam2)
{
    return 0;
}


unsigned int USB_ETH_DrvClearRxFlowCtrl(unsigned int ulParam1, unsigned int ulParam2)
{
    return 0;
}


unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned int ulPortNum)
{
    return 0;
}


unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType, unsigned int *pulPortNum, unsigned int ulPortMax)
{
    return 0;
}


unsigned int BSP_USB_PortTypeQuery(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : l2_notify_register
 功能描述  : L2状态进入退出通知回调函数注册接口
 输入参数  : FUNC_USB_LP_NOTIFY *pUSBLPFunc回调函数指针
 输出参数  : 无
 返 回 值  : 0:    注册成功
           其他：注册失败
*****************************************************************************/
int l2_notify_register(FUNC_USB_LP_NOTIFY pUSBLPFunc)
{
    return ERROR;
}

extern unsigned int BSP_USB_RegUdiEnableCB(USB_UDI_ENABLE_CB_T pFunc);
unsigned int mdrv_usb_reg_enablecb(USB_UDI_ENABLE_CB_T pFunc)
{
    return BSP_USB_RegUdiEnableCB(pFunc);
}

extern unsigned int BSP_USB_RegUdiDisableCB(USB_UDI_DISABLE_CB_T pFunc);
unsigned int mdrv_usb_reg_disablecb(USB_UDI_DISABLE_CB_T pFunc)
{
    return BSP_USB_RegUdiDisableCB(pFunc);
}


EXPORT_SYMBOL(BSP_USB_GetDiagModeValue);
EXPORT_SYMBOL(BSP_USB_GetU2diagDefaultValue);
EXPORT_SYMBOL(BSP_USB_SetPid);
EXPORT_SYMBOL(BSP_USB_GetPortMode);
EXPORT_SYMBOL(BSP_USB_GetLinuxSysType);
EXPORT_SYMBOL(BSP_USB_NASSwitchGatewayRegFunc);
EXPORT_SYMBOL(BSP_USB_PortTypeValidCheck);
EXPORT_SYMBOL(BSP_USB_GetAvailabePortType);
EXPORT_SYMBOL(BSP_USB_PortTypeQuery);

/*提供给OAM的接口*/
EXPORT_SYMBOL(l2_notify_register);

/*提供给TTF的接口*/
EXPORT_SYMBOL(USB_ETH_DrvSetRxFlowCtrl);
EXPORT_SYMBOL(USB_ETH_DrvClearRxFlowCtrl);

/*move from drv_usb_if.c end*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

