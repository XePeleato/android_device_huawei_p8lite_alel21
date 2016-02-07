/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  Drvinterface.c
*
*   作    者 :  c61362
*
*   描    述 :  底软给上层软件封装的接口层
*
*   修改记录 :  2012年3月2日 创建
*************************************************************************/
#include "BSP.h"

/*****************************************************************************
 函 数 名  : BSP_SOCP_SLEEPIN
 功能描述  : SOCP进入睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
BSP_U32 BSP_SOCP_SLEEPIN(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum)
{
#if defined (BALONG_COMPILE_SEPATARE)
    return 0;
#else
    return 0;
#endif

}

/*****************************************************************************
 函 数 名  : BSP_SOCP_SLEEPOUT
 功能描述  : SOCP退出睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
BSP_U32 BSP_SOCP_SLEEPOUT(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum)
{
#if defined (BALONG_COMPILE_SEPATARE)
    return 0;
#else
    return 0;
#endif
}


//monan for v9r1bbit
#if (defined(BOARD_SFT) && defined(VERSION_V9R1))//monan for v9r1bbit

#define printk(a)



int pmREGSet(BSP_U8 addr, BSP_U8 value, BSP_U8 mask)
{
    printk("stub  monan :pmREGSet. \n");
    return 0;
}


void PWRCTRL_BspInitial(void)
{
    printk("stub  monan :PWRCTRL_BspInitial. \n");
    return ;
}


int pmBUCKVoltSet(int buck_ID, BSP_U16 value)
{
    printk("stub  monan :pmBUCKVoltSet. \n");
    return 0;
}




#endif

/************************V9R1SFT START***********************/
