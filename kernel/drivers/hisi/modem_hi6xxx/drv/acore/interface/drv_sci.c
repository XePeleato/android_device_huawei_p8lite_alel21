/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_sci.c
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

#include "BSP.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_S32 BSP_SCI_SendData(BSP_U32 u32DataLength, BSP_U8 *pu8DataBuffer);
extern BSP_S32 BSP_SCI_ReceiveSync(BSP_U32 u32Length,BSP_U8 *pu8Data);
extern BSP_S32 BSP_SCI_GetATR (unsigned long *u32DataLength, BSP_U8 *pu8ATR,SCI_ATRInfo *sciATRInfo);
extern BSP_S32 BSP_SCI_ClassSwitch();
extern BSP_VOID BSP_SCI_TimeStop(BSP_U32 ulStopType);
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/

#if 0 /* 切换DSDA后，对外接口统一在DrvInterface.h中定义 */
/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_SND_DATA
*
* 功能描述  : 本接口用于发送一段数据到USIM卡
*
* 输入参数  : BSP_U32 u32DataLength 发送数据的有效长度。取值范围1～256，单位为字节
*             BSP_U8 *pu8DataBuffer 发送数据所在内存的首地址，如是动态分配，调用接口
*                                   后不能立即释放，依赖于硬件发完数据
* 输出参数  : 无
*
* 返 回 值  : OK
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_INVALIDPARA
*           BSP_ERR_SCI_DISABLED
*           BSP_ERR_SCI_NOCARD
*           BSP_ERR_SCI_NODATA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_SND_DATA(BSP_U32 u32DataLength, BSP_U8 *pu8DataBuffer)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_SCI_SendData(u32DataLength,pu8DataBuffer);
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_RCV
*
* 功能描述  : 本接口用于USIM Manager读取来自SCI Driver的卡返回数据
*             该接口为阻塞接口，只有SCI接收到足够的数据量后才会返回；
*             该接口的超时门限为1s
*
* 输入参数  : BSP_U32 u32DataLength USIM Manager欲从SCI Driver读取的数据长度。
* 输出参数  : BSP_U8 *pu8DataBuffer USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer。
* 返 回 值  : OK
*             BSP_ERR_SCI_NOTINIT
*             BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/

BSP_S32 DRV_USIMMSCI_RCV(BSP_U32 u32Length,BSP_U8 *pu8Data)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_SCI_ReceiveSync(u32Length,pu8Data);
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_GET_ATR
*
* 功能描述  : 本接口用于将Driver层缓存的ATR数据和数据个数返回给USIM Manager层
*
* 输入参数  : BSP_VOID
* 输出参数  : unsigned long *u8DataLength  Driver读取的ATR数据长度，返回给USIM Manager。
*                                   取值范围0～32，单位是字节
*           BSP_U8 *pu8ATR          USIM Manager指定的Buffer，SCI Driver将ATR
*                                   数据拷贝到本Buffer。一般为操作系统函数动态分配
*                                   或者静态分配的地址
*
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_GET_ATR(unsigned long *u32DataLength, BSP_U8 *pu8ATR)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_SCI_GetATR(u32DataLength, pu8ATR);
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_CLASS_SWITCH
*
* 功能描述  : 本接口用于支持PS对卡的电压类型进行切换，从1.8V切换到3V
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  :  OK    当前电压不是最高的，进行电压切换操作
*           BSP_ERR_SCI_CURRENT_STATE_ERR 切换失败 current SCI driver state is ready/rx/tx
*           BSP_ERR_SCI_VLTG_HIGHEST   当前电压已经是最高电压，没有进行电压切换
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_CURRENT_VLTG_ERR 当前电压值异常（非class B或者C）
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_CLASS_SWITCH(BSP_VOID)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_SCI_ClassSwitch();
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_TM_STOP
*
* 功能描述  : 本接口用于支持PS关闭SIM卡时钟
*
* 输入参数  :
*           SCI_CLK_STOP_TYPE_E enTimeStopCfg 时钟停止模式
*
* 输出参数  : 无
*
* 返 回 值  : OK - successful completion
*               ERROR - failed
*               BSP_ERR_SCI_NOTINIT
*               BSP_ERR_SCI_INVALIDPARA - invalid mode specified
*               BSP_ERR_SCI_UNSUPPORTED - not support such a operation
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_TM_STOP(BSP_U32 ulStopType)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_SCI_TimeStop(ulStopType);
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

