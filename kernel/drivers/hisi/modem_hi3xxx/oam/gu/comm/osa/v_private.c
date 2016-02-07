/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2015, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: v_private.c                                                     */
/*                                                                           */
/* Author: dongtinghuan                                                      */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2014-11                                                             */
/*                                                                           */
/* Description: 对底软接口的私有封装                                         */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "v_typdef.h"
#include "v_IO.h"
#include "v_int.h"
#include "vos_Id.h"
#include "v_sem.h"
#include "v_private.h"
#include "mdrv.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_V_PRIVATE_C

/*****************************************************************************
 Function   : VOS_GetDrvVerInfo
 Description: 获取单板版本信息
 Input      : enVerInfo -- 获取类型
 Return     : VOS_INT   -- 获取类型值
 Other      :
 *****************************************************************************/
VOS_INT VOS_GetDrvVerInfo(VOS_GET_DRV_VER_INFO_ENUM_UINT32 enVerInfo)
{
    const MODEM_VER_INFO_S             *pstModemVerInfo;

    pstModemVerInfo = mdrv_ver_get_info();

    if ((VOS_NULL_PTR == pstModemVerInfo)||(VOS_GET_DRV_VER_INFO_BUTT <= enVerInfo))
    {
        VOS_ProtectionReboot(VOS_GET_DRV_VER_INFO_ERROR, (VOS_INT)PS_FILE_ID_V_PRIVATE_C,
                             (VOS_INT)enVerInfo,(VOS_CHAR *)VOS_NULL_PTR, 0);
        return VOS_ERROR;
    }

    if (VOS_GET_DRV_BOARD_PRODUCT_ID == enVerInfo)
    {
        return pstModemVerInfo->sthwverinfo.hwindex;
    }
    else if (VOS_GET_DRV_PROTOCOL_TYPE == enVerInfo)
    {
        return pstModemVerInfo->stproductinfo.eprotype;
    }
    else
    {
    }

    return pstModemVerInfo->stproductinfo.eboardatype;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

