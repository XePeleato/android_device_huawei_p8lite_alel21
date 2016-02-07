/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_socp.c
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
#if (defined BSP_CORE_APP)
extern BSP_U32 BSP_SOCP_SLEEPIN(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum);
extern BSP_U32 BSP_SOCP_SLEEPOUT(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum);
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : DRV_SOCP_SLEEPIN
 功能描述  : SOCP进入睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
BSP_U32 DRV_SOCP_SLEEPIN(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum)
{
    
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#if 0
#ifdef BSP_COMPILE_ALLY
    return BSP_SOCP_SleepIn(pu32SrcChanID, uSrcChanNum, pu32DstChanID, uDstChanNum);
#else
    return 0;
#endif
#endif
#endif

#if (defined BSP_CORE_APP)
    BSP_SOCP_SLEEPIN(pu32SrcChanID, uSrcChanNum, pu32DstChanID, uDstChanNum);
    return 0;
#endif
}



/*****************************************************************************
 函 数 名  : DRV_SOCP_SLEEPOUT
 功能描述  : SOCP退出睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
BSP_U32 DRV_SOCP_SLEEPOUT(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#if 0
#ifdef BSP_COMPILE_ALLY
    return BSP_SOCP_SleepOut(pu32SrcChanID, uSrcChanNum, pu32DstChanID, uDstChanNum);
#else
    return 0;
#endif
#endif
#endif

#if (defined BSP_CORE_APP)
    BSP_SOCP_SLEEPOUT(pu32SrcChanID, uSrcChanNum, pu32DstChanID, uDstChanNum);
    return 0;
#endif
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

