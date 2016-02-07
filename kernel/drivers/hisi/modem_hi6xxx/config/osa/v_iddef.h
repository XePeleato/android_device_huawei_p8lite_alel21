/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: include/v_iddef.h                                               */
/*                                                                           */
/* Author:                                                                   */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date:                                                                     */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#ifndef  _V_IDDEF_H
#define  _V_IDDEF_H

#include "v_typdef.h"
#include "product_config.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/* VOS编程模式下PID与FID对应关系 */
typedef  struct ID_PID_to_FID
{
    unsigned long ulPID;    /* PID */
    unsigned long ulFID;    /* PID隶属的FID */
}PID_to_FID_Type;


/************************* MACRO & TYPE DEFINITION ***************************/
#define BEGIN_FID_DEFINITION() \
    enum VOS_Function_ID \
    { \
        VOS_FID_RESERVED = VOS_FID_DOPRAEND - 1,

#define END_FID_DEFINITION() \
        VOS_FID_BUTT \
    };

#define DEFINE_FID(fid) fid,

#define DEFINE_PID(pid) pid,

#define VOS_GET_CPU_ID(pid)      (((pid)>>0x08)&0xFF)

#define END_DECLARE_PID_BELONG_TO_FID() \
    }

#define ON_DECLARE(pid, fid, pri) \
    MOD_RegFidPidRSP(pid , fid, pri);

/* VOS编程模式下功能模块静态描述表 */
#define FID_NAME_LENGTH         (16)

typedef struct ID_Static_Table_Type
{
    VOS_FID       ulFID;                       /* FID           */
    VOS_UINT8     achFIDName[FID_NAME_LENGTH]; /* FID名称       */
    Init_Fun_Type       pfnInitFun;                  /* FID初始化函数 */
    VOS_UINT32    ulStartOrder;                /* FID启动顺序   */
    VOS_UINT32    ulStackSize ;
    /* which should be del when only one FID exists */
    VOS_UINT32    ulQueueSize;
} FID_Static_Table_Type;


/* 应用模块初始化顺序表 */
typedef struct APP_Start_Order_Type
{
    VOS_VOID (*pfnInit)(VOS_VOID);
    VOS_UINT32  ulStartOrder;    /*FID启动顺序*/
} APP_Start_Order_Type;


enum VOS_DOPRA_Function_ID
{

    DOPRA_FID_SYS=0,

    VOS_FID_DOPRAEND /* 系统FID定义结束标志 */
};


enum VOS_DOPRA_Process_ID
{

    DOPRA_PID_SYS=0,
    DOPRA_PID_TIMER=1
};

typedef struct
{
    VOS_INT size;
    VOS_INT number;
}VOS_MEM_BLOCK_INFO;

#if (RAT_MODE != RAT_GU)
#if (OSA_CPU_CCPU == VOS_OSA_CPU)

/* modify this to config memory*/
/* the number of message's control block */
#define VOS_MEM_CTRL_BLOCK_NUMBER                           13

/* the number of memory's control block */
#define VOS_SIMPLE_MEM_CTRL_BLOCK_NUMBER                    17

/* VOS_TIMER_MESSAGE_NUM must be equal 2*VOS_MAX_TIMER_NUMBER */
#define VOS_TIMER_MESSAGE_NUM                               400

#endif

#if (OSA_CPU_ACPU == VOS_OSA_CPU)

/* modify this to config memory*/
/* the number of message's control block */
#define VOS_MEM_CTRL_BLOCK_NUMBER                           12

/* the number of memory's control block */
#define VOS_SIMPLE_MEM_CTRL_BLOCK_NUMBER                    10

/* VOS_TIMER_MESSAGE_NUM must be equal 2*VOS_MAX_TIMER_NUMBER */
#define VOS_TIMER_MESSAGE_NUM                               300

#endif

/* config the number of pool which VOS support */
#define VOS_MEM_POOL_NUMBER                                 2

#endif

#if (RAT_MODE == RAT_GU)
#if (OSA_CPU_CCPU == VOS_OSA_CPU)

/* modify this to config memory*/
/* the number of message's control block */
#define VOS_MEM_CTRL_BLOCK_NUMBER                           13

/* the number of memory's control block */
#define VOS_SIMPLE_MEM_CTRL_BLOCK_NUMBER                    14

/* VOS_TIMER_MESSAGE_NUM must be equal 2*VOS_MAX_TIMER_NUMBER */
#define VOS_TIMER_MESSAGE_NUM                               400


#endif

#if (OSA_CPU_ACPU == VOS_OSA_CPU)

/* modify this to config memory*/
/* the number of message's control block */
#define VOS_MEM_CTRL_BLOCK_NUMBER                           9

/* the number of memory's control block */
#define VOS_SIMPLE_MEM_CTRL_BLOCK_NUMBER                    9

/* VOS_TIMER_MESSAGE_NUM must be equal 2*VOS_MAX_TIMER_NUMBER */
#define VOS_TIMER_MESSAGE_NUM                               300


#endif

/* config the number of pool which VOS support */
#define VOS_MEM_POOL_NUMBER                                 2

#endif

#define BEGIN_DECLARE_PID_BELONG_TO_FID()\
VOS_VOID REG_FID_PID_RSP(VOS_VOID)\
{\
        MOD_RegFidPidRSP( DOPRA_PID_SYS, DOPRA_FID_SYS, VOS_PRIORITY_BASE );\
        MOD_RegFidPidRSP( DOPRA_PID_TIMER, DOPRA_FID_SYS, VOS_PRIORITY_BASE );\

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_IDDEF_H */

