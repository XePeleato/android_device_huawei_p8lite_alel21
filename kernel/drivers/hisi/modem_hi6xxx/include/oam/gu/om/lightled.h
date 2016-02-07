/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: LightLED.h                                                      */
/*                                                                           */
/* Author: Xu cheng                                                          */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-06                                                             */
/*                                                                           */
/* Description: Turn on LED according to some Events which be supported by PS*/
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2008-06                                                          */
/*    Author: Xu cheng                                                       */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef  _LIGHT_LED_H
#define  _LIGHT_LED_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "vos.h"
#include "NasOmInterface.h"
#include "WasOmInterface.h"

enum
{
    OAM_LED_STATE_CNF = 0x0001,
    OAM_LED_STATE_BUTT
};

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                              ulMsgName;
    VOS_INT                                 lOldLedState;
    VOS_INT                                 lNewLedState;
    WAS_MNTN_OM_OUT_OF_SERVICE_ENUM_UINT8   enIsOutService;
    NAS_OM_REGISTER_STATE_ENUM_UINT8        enRegState;
    NAS_OM_SERVICE_TYPE_ENUM_UINT8          enServiceType;
    VOS_UINT8                               ucCardStatus;
    VOS_UINT32                              ulRatType;
}LED_STATE_STRUCT;

extern VOS_UINT32 OM_TraceMsgHook(VOS_VOID *pMsg);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _LIGHT_LED_H */

