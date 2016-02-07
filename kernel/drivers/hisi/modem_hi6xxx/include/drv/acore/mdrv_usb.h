/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  mdrv_usb.h
*
*   作    者 :
*
*   描    述 :
*
*   修改记录 :
*************************************************************************/
#ifndef _MDRV_USB_H_
#define _MDRV_USB_H_

#ifdef _cplusplus
extern "C"
{
#endif

#ifdef __KERNEL__
#include <linux/notifier.h>
#endif


/*****************************************************************************
 *  函 数 名  : mdrv_usb_reg_enablecb
 *  功能描述  : 协议栈注册USB使能通知回调函数
 *  输入参数  : pFunc: USB使能回调函数指针
 *
 *  输出参数  : 无
 *  返 回 值  :  0	       操作成功。
 *                           其他	操作失败。
 *
 ******************************************************************************/
typedef void (*USB_UDI_ENABLE_CB_T)(void);
unsigned int mdrv_usb_reg_enablecb(USB_UDI_ENABLE_CB_T pFunc);

/*****************************************************************************
 *  函 数 名  : mdrv_usb_reg_disablecb
 *  功能描述  : 协议栈注册USB去使能通知回调函数
 *  输入参数  : pFunc: USB去使能回调函数指针
 *
 *  输出参数  : 无
 *  返 回 值  :  0	       操作成功。
 *                           其他	操作失败。
 *
 ******************************************************************************/
typedef void (*USB_UDI_DISABLE_CB_T)(void);
unsigned int mdrv_usb_reg_disablecb(USB_UDI_DISABLE_CB_T pFunc);


#ifdef _cplusplus
}
#endif
#endif

