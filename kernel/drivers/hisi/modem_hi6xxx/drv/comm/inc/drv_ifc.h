/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_ifc.h
  版 本 号   : 初稿
  作    者   : 陈易超 d00209607
  生成日期   : 2013年2月2日
  最近修改   :
  功能描述   : drv_ifc.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年2月2日
    作    者   : 陈易超 d00209607
    修改内容   : 创建文件

******************************************************************************/
#ifndef __DRV_IFC_H__
#define __DRV_IFC_H__

/* 模块ID枚举*/
typedef enum tagIFC_MODULE_E
{
    IFC_MODULE_BSP=  0,
    IFC_MODULE_LPS=  1,
    IFC_MODULE_GUPS= 2,
    IFC_MODULE_LMSP = 3,
    IFC_MODULE_GUOM = 4,
    IFC_MODULE_BUTT
} IFC_MODULE_E;


/* IFC回调函数结构*/
/* pMsgBody:该函数ID对应函数的参数,前四字节需保证为moduleId*/
/* u32Len:pMsgBody长度*/
typedef int (*BSP_IFC_REG_FUNC)(void *pMsgBody,unsigned int u32Len);

/*****************************************************************************
* 函 数 名  : BSP_IFC_RegFunc
*
* 功能描述  : 注册IFC回调函数接口
*
* 输入参数  : IFC_MODULE_E enModuleId    模块ID
*             BSP_IFC_REG_FUNC pFunc     回调函数指针
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*
* 其它说明  : 无
*
*****************************************************************************/
unsigned int BSP_IFC_RegFunc(IFC_MODULE_E enModuleId, BSP_IFC_REG_FUNC pFunc);


/*****************************************************************************
* 函 数 名  : BSP_IFC_Send
*
* 功能描述  : IFC发送消息接口
*
* 输入参数  : void * pMspBody           发送消息内容
*             unsigned int u32Len               pMspBody的长度
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*             BSP_ERR_IFC_SEND_FAIL
*
* 其它说明  : 无
*
*****************************************************************************/
unsigned int BSP_IFC_Send(void * pMspBody, unsigned int u32Len);


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define WAITFOREVER                     (0)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
 实 体 名  : tagIFCP_FUNC_E
 功能描述  : 定义五个核的核间函数调用
             前面16bit定位FUN所在CPUID,后面16bit定义函数FUN ID
*****************************************************************************/
typedef enum
{
    /* CCPU 0 上的FUN ID 定义 */
    IFCP_FUNC_CCPU_SAMPLE_ENABLE =  0x00000,
    IFCP_FUNC_CCPU_BUTT,

    /* ACPU 1 上的FUN ID 定义 */
    IFCP_FUNC_ACPU_SAMPLE_ENABLE =  0x10000,
    IFCP_FUNC_ACPU_BOARD_COMMCALL_TEST,        /* IFC 板测用例  commcall*/
    IFCP_FUNC_ACPU_BOARD_EXTENDCALL_TEST,       /* IFC 板测用例  extendcall*/
    IFCP_FUNC_ACPU_BUTT,

    /* GUDSP 2 上的FUN ID 定义 */
    IFCP_FUNC_GUDSP_SAMPLE_ENABLE =  0x20000,
    IFCP_FUNC_GUDSP_BUTT,

    /* HIFI 3 上的FUN ID 定义 */
    IFCP_FUNC_HIFI_SAMPLE_ENABLE  = 0x30000,
    IFCP_FUNC_HIFI_BUTT,

    /* MCU 4 上的FUN ID 定义 */
    IFCP_FUNC_MCU_TP_ENABLE       =  0x40000,
    IFCP_FUNC_MCU_HUTAF_HLT,                   /* HUTAF HLT 单板覆盖率 */
    IFCP_FUNC_MCU_MCA,                         /* mca interface*/
    IFCP_FUNC_MCU_BOARD_COMMCALL_TEST,        /* IFC 板测用例  commcall*/
    IFCP_FUNC_MCU_BOARD_EXTENDCALL_TEST,       /* IFC 板测用例  extendcall*/
    IFCP_FUNC_MCU_BUTT,

    /* BBE16 5 上的FUN ID 定义 */
    IFCP_FUNC_BBE16_SAMPLE_ENABLE =  0x50000,
    IFCP_FUNC_BBE16_BUTT

}SIFCP_FUNC_E;


/*****************************************************************************
 实 体 名  : FUNCID_RELATIVE_ADDR
 功能描述  : 定义五个核的核间函数调用函数ID相对地址
*****************************************************************************/
typedef enum
{
    /* CCPU 0 上的FUN ID 定义 */
    IFCP_FUNC_CCPU_INIT_ADDR   =  0x0,
    /* ACPU 1 上的FUN ID 定义 */
    IFCP_FUNC_ACPU_INIT_ADDR   =  0x10000,
    /* GUDSP 2 上的FUN ID 定义 */
    IFCP_FUNC_GUDSP_INIT_ADDR  =  0x20000,
    /* HIFI 3 上的FUN ID 定义 */
    IFCP_FUNC_HIFI_INIT_ADDR   =  0x30000,
    /* MCU 4 上的FUN ID 定义 */
    IFCP_FUNC_MCU_INIT_ADDR    =  0x40000,
    /* BBE16 5 上的FUN ID 定义 */
    IFCP_FUNC_BBE16_INIT_ADDR  =  0x50000
}FUNCID_RELATIVE_ADDR;


/*****************************************************************************
 枚举名    : BSP_MAILBOX_IFC_CALL_FALG_ENUM
 枚举说明  : IFC 函数调用的调用方式
*****************************************************************************/
 typedef enum
{
    BSP_MAILBOX_IFC_CALL_SYNC,        /*同步：0*/
    BSP_MAILBOX_IFC_CALL_ASYN,        /*异步：1*/
    BSP_MAILBOX_IFC_CALL_RESPONSE,    /*回复: 2*/
    BSP_MAILBOX_IFC_CALL_FLAG_BUTT
}BSP_MAILBOX_IFC_CALL_FALG_ENUM;

/*****************************************************************************
 枚举名    : BSP_MAILBOX_IFC_REG_FUN_FLAG_ENUM
 枚举说明  : IFC
*****************************************************************************/
typedef enum tagBSP_MAILBOX_IFC_REG_FUN_FLAG_ENUM
{
    BSP_MAILBOX_IFC_REG_FUN_FLAG_SELF            = 0,
    BSP_MAILBOX_IFC_REG_FUN_FLAG_IFC             = 1,
    BSP_MAILBOX_IFC_REG_FUN_FLAG_NONE            = 2,
    BSP_MAILBOX_IFC_REG_FUN_FLAG_BUTT
}BSP_MAILBOX_IFC_REG_FUN_FLAG_ENUM;

/*****************************************************************************
 实 体 名  : BSP_MAILBOX_ERR_ENUM
 功能描述  : 定义邮箱异常值
*****************************************************************************/
enum BSP_MAILBOX_ERR_ENUM
{
   /* BSP_MAILBOX_ERR_START                              = -2147483648, /\* 0x80000000,*\/ */
   BSP_MAILBOX_ERR_PARA                               = -2147483647, /* 0x80000001 */
   BSP_MAILBOX_ERR_MAILBOX_ID                         = -2147483646, /* 0x80000002 */
   BSP_MAILBOX_ERR_MAILBOX_WRITE                      = -2147483645, /* 0x80000003 */
   BSP_MAILBOX_ERR_MAILBOX_IPC_SEND_FAIL              = -2147483644, /* 0x80000004 */
   BSP_MAILBOX_ERR_MAILBOX_SEMB_CREATE                = -2147483643, /* 0x80000005 */
   BSP_MAILBOX_ERR_MAILBOX_MALLOC_FAIL                = -2147483642, /* 0x80000006 */
   BSP_MAILBOX_ERR_MAILBOX_TASK_CREATE                = -2147483641, /* 0x80000007 */
   BSP_MAILBOX_ERR_MAILBOX_LIST_INIT                  = -2147483640, /* 0x80000008 */
   BSP_MAILBOX_ERR_MAILBOX_SEND_MSG                   = -2147483639, /* 0x80000009 */
   BSP_MAILBOX_ERR_MAILBOX_CALLBACK_REGISTER          = -2147483638, /* 0x8000000a */
   BSP_MAILBOX_ERR_MAILBOX_FULL                       = -2147483637, /* 0x8000000b */
   BSP_MAILBOX_ERR_MAILBOX_FID_CREATE_FAIL            = -2147483636, /* 0x8000000c */
   BSP_MAILBOX_ERR_MAILBOX_IFC_FID_CREATE_FAIL        = -2147483635, /* 0x8000000d */
   BSP_MAILBOX_ERR_MAILBOX_MSG_ALLOC_FAIL             = -2147483634, /* 0x8000000e */
   BSP_MAILBOX_ERR_MAILBOX_MSG_SEND_FAIL              = -2147483633, /* 0x8000000f */
   BSP_MAILBOX_ERR_MAILBOX_MAIL_CHECK_FAIL            = -2147483632, /* 0x80000010 */
   BSP_MAILBOX_ERR_MAILBOX_INIT_NOT_OK                = -2147483631, /* 0x80000011 */
   BSP_MAILBOX_ERR_MAILBOX_CALLBACK_INIT              = -2147483630, /* 0x80000012 */
   BSP_MAILBOX_ERR_MAILBOX_CALLBACK_UNREGISTER        = -2147483629, /* 0x80000013 */
   BSP_MAILBOX_ERR_IFC_SEMID_FULL                     = -2147483628, /* 0x80000014 */
   BSP_MAILBOX_ERR_IFC_SEMID_EMPTY                    = -2147483627, /* 0x80000015 */
   BSP_MAILBOX_ERR_IFC_CALLBACK_FULL                  = -2147483626, /* 0x80000016 */
   BSP_MAILBOX_ERR_IFC_CALLBACK_EMPTY                 = -2147483625, /* 0x80000017 */
   BSP_MAILBOX_ERR_MAILBOX_PROTECTED_WORD_FAIL        = -2147483624, /* 0x80000018 */
   BSP_MAILBOX_ERR_IFC_OUT_OF_EXTENDCALL              = -2147483623, /* 0x80000019 */
   BSP_MAILBOX_ERR_IFC_OUT_OF_FIVE_PARA               = -2147483622, /* 0x8000001a */
   BSP_MAILBOX_ERR_MAILBOX_OUT_OF_BUFFER              = -2147483621, /* 0x8000001b */
   BSP_MAILBOX_ERR_MAILBOX_MSG_CHECK_FAIL             = -2147483620, /* 0x8000001c */
   BSP_MAILBOX_ERR_IFC                                = -2147483619  /* 0x8000001d */

};

/*****************************************************************************
 枚举名    : BSP_MAILBOX_IFC_CALL_TPYE_ENUM
 枚举说明  : IFC 函数调用的调用类型
*****************************************************************************/
 typedef enum
{
    BSP_MAILBOX_IFC_CALL_COMM     = 0x1,
    BSP_MAILBOX_IFC_CALL_EXTENED  = 0x2,
    BSP_MAILBOX_IFC_CALL_TYPE_BUTT
}BSP_MAILBOX_IFC_CALL_TPYE_ENUM;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

typedef void (*IFC_COMM_CALLBACK)(unsigned long fundi,long retvalue);
typedef void (*IFC_EXTEN_CALLBACK)(unsigned long fundi,unsigned char *buffer,unsigned long length);
typedef long (*IFC_REGISTER_PARA)(long Para1,long Para2,long Para3,long Para4,long Para5);
typedef long (*IFC_REGISTER_EXTEN)(unsigned char* Buffer,unsigned long Length);

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern unsigned long MB_IFC_RegFunc(unsigned long emFunid,void* pFunc, unsigned long emSelfFlag);

extern unsigned long  MB_IFC_CommCall(
             unsigned long                Funid,
             unsigned long                emSyncFlag,
             IFC_COMM_CALLBACK     CallBack,
             unsigned char*                InputPara,
             unsigned long                InputParaNumber);

extern unsigned long MB_IFC_ExtendCall(
              unsigned long                    Funid,
              unsigned long                    emSyncFlag,
              IFC_EXTEN_CALLBACK        CallBack,
              unsigned char*                    InputBuffer,
              unsigned long                    InputLen,
              unsigned char*                    OutPutBuffer,
              unsigned long*                   OutPutLen );

extern unsigned long MB_IFC_Response(
              unsigned long                SCPUID,
              unsigned long                Funid,
              unsigned long                emSyncFlag,
              void*              CallBack,
              unsigned long                CallType,
              unsigned char*                InputBuffer,
              unsigned long                InputLen,
              unsigned char*                OutputBuffer,
              unsigned long*               OutputLen );

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif

