/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: vos_main.c                                                      */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description: implement root function                                      */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#include "vos.h"
#include "mdrv.h"
#include <product_config.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 modify:x51137; review:h59254; cause:print log */
#define    THIS_FILE_ID        PS_FILE_ID_VOS_MAIN_C
/*lint +e767 modify:x51137; review:h59254; */

extern VOS_UINT32 OM_AcpuRegNvInit(VOS_VOID);
extern VOS_UINT32 OM_RegNvInit(VOS_VOID);
extern VOS_UINT32 OM_ComRx_ICC_Init(VOS_VOID);
extern VOS_UINT32 SCM_ChannelInit(VOS_VOID);
#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
extern VOS_UINT32 SocpInit(VOS_VOID);
#endif
extern VOS_UINT32 SCM_ChannelMemInit(VOS_VOID);
extern VOS_UINT32 VOS_ICC_Init(VOS_VOID);
extern VOS_UINT32 APM_InitSystemAddr(VOS_VOID);


VOS_UINT32 g_ulVosOutsideStep = 0;

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
extern signed int VOS_MsgLpmCb(int x);

/*****************************************************************************
 Function   : VOS_OutSideInit
 Description: finish the outside device init,eg. ICC
 Calls      :
 Called By  : VOS_Startup
 Input      : None
 Return     : VOS_OK or VOS_ERROR
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_OutsideInit(VOS_VOID)
{
    VOS_UINT32                          ulResult = 0;

    g_ulVosOutsideStep = 0x00200000;

    mdrv_icc_register_resume_cb(((UDI_ICC_GUOM4_ID & 0x00ff)<<16), VOS_MsgLpmCb, 0);

    /*DRV MB Init*/
    if(VOS_OK != VOS_DRVMB_Init())
    {
        ulResult |= 0x00200000;
    }

    g_ulVosOutsideStep = 0x00100000;

    if (VOS_OK != SCM_ChannelMemInit())/* 通道内存初始化 */
    {
        ulResult |= 0x00100000;
    }

    g_ulVosOutsideStep = 0x00400000;

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
    /*SOCP Init*/
    if(VOS_OK != SocpInit())
    {
        ulResult |= 0x00400000;
    }

    g_ulVosOutsideStep = 0x00800000;
#endif

    /*SCM  Init*/
    if(VOS_OK != SCM_ChannelInit())
    {
        ulResult |= 0x00800000;
    }

    g_ulVosOutsideStep = 0x1000;

    /*OSA ICC Channel Init*/
    if(VOS_OK != VOS_ICC_Init())
    {
        ulResult |= 0x1000;
    }

    g_ulVosOutsideStep = 0x2000;

    /*ICC Channel Init*/
    if(VOS_OK != OM_ComRx_ICC_Init())
    {
        ulResult |= 0x2000;
    }

    g_ulVosOutsideStep = 0x0002;

    if(VOS_OK != OM_AcpuRegNvInit())
    {
       ulResult |= 0x0002;
    }

    return ulResult;
}
#endif  /*(OSA_CPU_ACPU == VOS_OSA_CPU)*/

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
/*****************************************************************************
 Function   : VOS_OutSideInit
 Description: finish the outside device init,eg. ICC
 Calls      :
 Called By  : VOS_Startup
 Input      : None
 Return     : VOS_OK or VOS_ERROR
 Other      :
 *****************************************************************************/
VOS_UINT32 VOS_OutsideInit(VOS_VOID)
{
    VOS_UINT32                          ulResult = 0;

    g_ulVosOutsideStep = 0x0002;

    if(VOS_OK != OM_RegNvInit())
    {
       ulResult |= 0x0002;
    }

    g_ulVosOutsideStep = 0x1000;

    /*ICC channel Init*/
    if(VOS_OK != VOS_ICC_Init())
    {
        ulResult |= 0x1000;
    }

    g_ulVosOutsideStep = 0x2000;

    /*ICC channel Init*/
    if(VOS_OK != OM_ComRx_ICC_Init())
    {
        ulResult |= 0x2000;
    }

    g_ulVosOutsideStep = 0x00200000;

    /*DRV MB Init*/
    if(VOS_OK != VOS_DRVMB_Init())
    {
        ulResult |= 0x00200000;
    }

    g_ulVosOutsideStep = 0x00100000;

    if (VOS_OK != SCM_ChannelMemInit())/* 通道内存初始化 */
    {
        ulResult |= 0x00100000;
    }

    g_ulVosOutsideStep = 0x00400000;

#if (FEATURE_SOCP_SYNC_ENABLE == FEATURE_ON)
    /*SOCP Init*/
    if(VOS_OK != SocpInit())
    {
        ulResult |= 0x00400000;
    }

    g_ulVosOutsideStep = 0x00800000;
#endif

    /*SCM  Init*/
    if(VOS_OK != SCM_ChannelInit())
    {
        ulResult |= 0x00800000;
    }

    g_ulVosOutsideStep = 0x01000000;

    if (VOS_OK != APM_InitSystemAddr()) /*放在这里初始化HPA相关的基地址，HPA任务后面会删除*/
    {
        ulResult |= 0x01000000;
    }

    return ulResult;
}
#endif  /*(OSA_CPU_CCPU == VOS_OSA_CPU)*/

VOS_VOID OM_OutsideShow(VOS_VOID)
{
    vos_printf("OM_OutsideShow:g_ulVosOutsideStep = %d\n", g_ulVosOutsideStep);

    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

