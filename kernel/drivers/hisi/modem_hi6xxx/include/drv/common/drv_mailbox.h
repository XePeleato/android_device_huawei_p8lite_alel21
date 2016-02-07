
#ifndef __DRV_MAILBOX_H__
#define __DRV_MAILBOX_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "mdrv_ipc_enum.h"
#include "drv_mailbox_ifc.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* 定义源CPU 往 目标CPU的邮箱通道, 每通道对应独立的邮箱缓存 */

/* 按规则生成枚举项的宏定义 */
#define MAILBOX_CHANNEL_BEGIN(src, dst) \
    enum MAILBOX_CHANNEL_##src##2##dst##_ENUM \
    { \
        MAILBOX_CHANNEL_##src##2##dst##_RSERVED = -1,

#define MAILBOX_CHANNEL_ITEM(src, dst, channel)  \
    MAILBOX_CHANNEL_##src##2##dst##_##channel

#define MAILBOX_CHANNEL_END(src, dst) \
        MAILBOX_CHANNEL_##src##2##dst##_BUTT \
    };

/* CCPU -> HIFI */
MAILBOX_CHANNEL_BEGIN(CCPU, HIFI)
    /* 以下各枚举项按规则生成, 形如: MAILBOX_CHANNEL_CCPU2HIFI_DEFAULT */
    MAILBOX_CHANNEL_ITEM(CCPU, HIFI, MSG),
MAILBOX_CHANNEL_END(CCPU, HIFI)

/* CCPU -> MCU */
MAILBOX_CHANNEL_BEGIN(CCPU, MCU)
    MAILBOX_CHANNEL_ITEM(CCPU, MCU, MSG),
    MAILBOX_CHANNEL_ITEM(CCPU, MCU, IFC),
MAILBOX_CHANNEL_END(CCPU, MCU)

/* CCPU -> ACPU */
MAILBOX_CHANNEL_BEGIN(CCPU, ACPU)
    MAILBOX_CHANNEL_ITEM(CCPU, ACPU, MSG),
    MAILBOX_CHANNEL_ITEM(CCPU, ACPU, IFC),
MAILBOX_CHANNEL_END(CCPU, ACPU)

/* ACPU -> CCPU */
MAILBOX_CHANNEL_BEGIN(ACPU, CCPU)
    MAILBOX_CHANNEL_ITEM(ACPU, CCPU, MSG),
    MAILBOX_CHANNEL_ITEM(ACPU, CCPU, IFC),
MAILBOX_CHANNEL_END(ACPU, CCPU)

/* ACPU -> MCU */
MAILBOX_CHANNEL_BEGIN(ACPU, MCU)
    MAILBOX_CHANNEL_ITEM(ACPU, MCU, MSG),
    MAILBOX_CHANNEL_ITEM(ACPU, MCU, IFC),
MAILBOX_CHANNEL_END(ACPU, MCU)

/* ACPU -> HIFI */
MAILBOX_CHANNEL_BEGIN(ACPU, HIFI)
    MAILBOX_CHANNEL_ITEM(ACPU, HIFI, MSG),
MAILBOX_CHANNEL_END(ACPU, HIFI)

/* HIFI -> ACPU */
MAILBOX_CHANNEL_BEGIN(HIFI, ACPU)
    MAILBOX_CHANNEL_ITEM(HIFI, ACPU, MSG),
MAILBOX_CHANNEL_END(HIFI, ACPU)

/* HIFI -> CCPU */
MAILBOX_CHANNEL_BEGIN(HIFI, CCPU)
    MAILBOX_CHANNEL_ITEM(HIFI, CCPU, MSG),
MAILBOX_CHANNEL_END(HIFI, CCPU)

/* HIFI -> BBE16 */
MAILBOX_CHANNEL_BEGIN(HIFI, BBE16)
    MAILBOX_CHANNEL_ITEM(HIFI, BBE16, MSG),
MAILBOX_CHANNEL_END(HIFI, BBE16)

/* MCU -> ACPU */
MAILBOX_CHANNEL_BEGIN(MCU, ACPU)
    MAILBOX_CHANNEL_ITEM(MCU, ACPU, MSG),
    MAILBOX_CHANNEL_ITEM(MCU, ACPU, IFC),
MAILBOX_CHANNEL_END(MCU, ACPU)

/* MCU -> CCPU */
MAILBOX_CHANNEL_BEGIN(MCU, CCPU)
    MAILBOX_CHANNEL_ITEM(MCU, CCPU, MSG),
    MAILBOX_CHANNEL_ITEM(MCU, CCPU, IFC),
MAILBOX_CHANNEL_END(MCU, CCPU)

/* BBE16 -> HIFI */
MAILBOX_CHANNEL_BEGIN(BBE16, HIFI)
    MAILBOX_CHANNEL_ITEM(BBE16, HIFI, MSG),
MAILBOX_CHANNEL_END(BBE16, HIFI)

/* MCU -> HIFI */
MAILBOX_CHANNEL_BEGIN(MCU, HIFI)
    MAILBOX_CHANNEL_ITEM(MCU, HIFI, IFC),
MAILBOX_CHANNEL_END(MCU, HIFI)

/* HIFI -> MCU */
MAILBOX_CHANNEL_BEGIN(HIFI, MCU)
    MAILBOX_CHANNEL_ITEM(HIFI, MCU, IFC),
MAILBOX_CHANNEL_END(HIFI, MCU)

/* 有助SI解析, 利于阅读代码 */
enum MAILBOX_GAP_FOR_SI_PARSE {MAILBOX_GAP_FOR_SI_BUTT};

#define MAILBOX_ID_SRC_CPU_OFFSET       (24)        /*邮件ID中的源CPU ID bit位偏移地址*/
#define MAILBOX_ID_DST_CPU_OFFSET       (16)        /*邮件ID中的源CPU ID bit位偏移地址*/
#define MAILBOX_ID_CHANNEL_OFFSET       (8)         /*邮件ID中邮箱内存通道 ID bit位偏移地址*/

/* 邮编起始编号宏定义 */
#define MAILBOX_MAILCODE_CHANNEL(src, dst, channel) \
    ( ((unsigned int)(src) << MAILBOX_ID_SRC_CPU_OFFSET) \
    | ((unsigned int)(dst) << MAILBOX_ID_DST_CPU_OFFSET) \
    | ((unsigned int)(channel) << MAILBOX_ID_CHANNEL_OFFSET) )

#define MAILBOX_CPUID(cpu)   MAILBOX_CPUID_##cpu

#define MAILBOX_MAILCODE_RESERVED(src, dst, channel) \
    MAILBOX_MAILCODE_CHANNEL(MAILBOX_CPUID(src), \
                             MAILBOX_CPUID(dst), \
                            MAILBOX_CHANNEL_ITEM(src, dst, channel))

#define MAILBOX_MAILCODE_ITEM_RESERVED(src, dst, channel) \
    MAILBOX_MAILCODE_##src##2##dst##_##channel##_RESERVED

#define MAILBOX_MAILCODE_ITEM_END(src, dst, channel)    \
    MAILBOX_MAILCODE_##src##2##dst##_##channel##_BUTT

#define MAILBOX_MAILCODE_ITEM_BEGIN(src, dst, channel)  \
    MAILBOX_MAILCODE_ITEM_RESERVED(src, dst, channel) = MAILBOX_MAILCODE_RESERVED(src, dst, channel)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实 体 名  : MAILBOX_CPUID_E
 功能描述  : 定义邮箱涉及CPU编号
*****************************************************************************/
enum MAILBOX_CPUID_ENUM
{
    MAILBOX_CPUID_RESERVED  =   -1,
    MAILBOX_CPUID_ACPU      =   IPC_CORE_ACPU,
    MAILBOX_CPUID_CCPU      =   IPC_CORE_CCPU,
    MAILBOX_CPUID_MCU       =   IPC_CORE_MCU,
    MAILBOX_CPUID_HIFI      =   IPC_CORE_HIFI,
    MAILBOX_CPUID_BBE16     =   IPC_CORE_BBE16,
    MAILBOX_CPUID_BUTT
};

/*****************************************************************************
 实体名称  : MAILBOX_MAILCODE_ENUM
 功能描述  : 邮编枚举定义, 邮编唯一标识邮件发送目标, 也即邮件数据接收处理者
*****************************************************************************/
enum MAILBOX_MAILCODE_ENUM
{
    /* CCPU->MCU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, MCU, MSG),
    MAILBOX_MAILCODE_CCPU_TO_MCU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_CCPU_TO_MCU_VOS_MSG_URGENT,
    BSP_MAILBOX_CHANNEL_CCPU_TO_MCU_MCA_CH,

    MAILBOX_MAILCODE_ITEM_END(CCPU, MCU, MSG),

    /* CCPU->MCU IFC调用号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, MCU, IFC),
    MAILBOX_IFC_CCPU_TO_MCU_TEST_CMP,
    MAILBOX_IFC_CCPU_TO_MCU_TEST,
    MAILBOX_IFC_CCPU_TO_MCU_MCA,
    MAILBOX_MAILCODE_ITEM_END(CCPU, MCU, IFC),

    /* CCPU->HIFI 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, HIFI, MSG),
    MAILBOX_MAILCODE_CCPU_TO_HIFI_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_CCPU_TO_HIFI_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ITEM_END(CCPU, HIFI, MSG),

    /* CCPU->ACPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, ACPU, MSG),
    MAILBOX_MAILCODE_ITEM_END(CCPU, ACPU, MSG),

    /* CCPU->ACPU IFC编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, ACPU, IFC),
    MAILBOX_IFC_CCPU_TO_ACPU_TEST_CMP,
    MAILBOX_IFC_CCPU_TO_ACPU_TEST,
    MAILBOX_IFC_CCPU_TO_ACPU_PRINT,
    MAILBOX_IFC_CCPU_TO_ACPU_FOPEN,
    MAILBOX_IFC_CCPU_TO_ACPU_FCLOSE,
    MAILBOX_IFC_CCPU_TO_ACPU_FREAD,
    MAILBOX_IFC_CCPU_TO_ACPU_FWRITE,
    MAILBOX_IFC_CCPU_TO_ACPU_FSEEK,
    MAILBOX_IFC_CCPU_TO_ACPU_REMOVE,
    MAILBOX_IFC_CCPU_TO_ACPU_FTELL,
    MAILBOX_IFC_CCPU_TO_ACPU_RENAME,
    MAILBOX_IFC_CCPU_TO_ACPU_ACCESS,
    MAILBOX_IFC_CCPU_TO_ACPU_MKDIR,
    MAILBOX_IFC_CCPU_TO_ACPU_RMDIR,
    MAILBOX_IFC_ACPU_TO_CCPU_PMIC_IRQEVENT_REPO,
    MAILBOX_MAILCODE_ITEM_END(CCPU, ACPU, IFC),

    /* ACPU->MCU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, MCU, MSG),
    MAILBOX_MAILCODE_ACPU_TO_MCU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_ACPU_TO_MCU_VOS_MSG_URGENT,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_IFC_CH,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_IFC_RESPONSE_CH,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_SENSOR_CH,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_TP_CH,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_MCA_CH,
    /*Modify_for_c_reset, l00212112,20130511, starts*/
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_RST_CH,
    /*Modify_for_c_reset, l00212112,20130511, ends*/
    MAILBOX_MAILCODE_ITEM_END(ACPU, MCU, MSG),

    /* ACPU->MCU IFC远程函数调用邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, MCU, IFC),
    MAILBOX_IFC_ACPU_TO_MCU_TEST_CMP,
    MAILBOX_IFC_ACPU_TO_MCU_TEST,
    MAILBOX_IFC_ACPU_TO_MCU_HUTAF_HLT,
	MAILBOX_IFC_ACPU_TO_MCU_MCA,
    MAILBOX_IFC_ACPU_TO_MCU_RUN_CMD,
    MAILBOX_MAILCODE_ITEM_END(ACPU, MCU, IFC),


    /* ACPU->HIFI 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, HIFI, MSG),
    MAILBOX_MAILCODE_ACPU_TO_HIFI_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_ACPU_TO_HIFI_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ACPU_TO_HIFI_AUDIO,
    MAILBOX_MAILCODE_ACPU_TO_HIFI_MISC,
    /*Modify_for_c_reset, l00212112,20130511, starts*/
    MAILBOX_MAILCODE_ACPU_TO_HIFI_CCORE_RESET_ID,
    /*Modify_for_c_reset, l00212112,20130511, ends*/
    MAILBOX_MAILCODE_ITEM_END(ACPU, HIFI, MSG),

    /* ACPU->CCPU MSG号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, CCPU, MSG),
    MAILBOX_IFC_ACPU_TO_CCPU_CSHELL_START,
    MAILBOX_MAILCODE_ITEM_END(ACPU, CCPU, MSG),

    /* ACPU->CCPU IFC号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, CCPU, IFC),
    MAILBOX_IFC_ACPU_TO_CCPU_TEST_CMP,
    MAILBOX_IFC_ACPU_TO_CCPU_TEST,
    MAILBOX_IFC_ACPU_TO_CCPU_PRINT,
    MAILBOX_IFC_ACPU_TO_CCPU_FOPEN,
    MAILBOX_IFC_ACPU_TO_CCPU_FCLOSE,
    MAILBOX_IFC_ACPU_TO_CCPU_FREAD,
    MAILBOX_IFC_ACPU_TO_CCPU_FWRITE,
    MAILBOX_IFC_ACPU_TO_CCPU_FSEEK,
    MAILBOX_IFC_ACPU_TO_CCPU_REMOVE,
    MAILBOX_IFC_ACPU_TO_CCPU_FTELL,
    MAILBOX_IFC_ACPU_TO_CCPU_RENAME,
    MAILBOX_IFC_ACPU_TO_CCPU_ACCESS,
    MAILBOX_IFC_ACPU_TO_CCPU_MKDIR,
    MAILBOX_IFC_ACPU_TO_CCPU_RMDIR,
    MAILBOX_IFC_ACPU_TO_CCPU_BASE_TEST2,
    MAILBOX_IFC_ACPU_TO_CCPU_BASE_TEST1,
	MAILBOX_IFC_ACPU_TO_CCPU_PMIC_IRQEVENT,
    MAILBOX_IFC_ACPU_TO_CCPU_SYSTEMERROR,
    MAILBOX_IFC_ACPU_TO_CCPU_RST_MODEM,
    MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_DIEID,
    MAILBOX_IFC_ACPU_TO_CCPU_SET_EFUSE_CHIPID,
    MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_CHIPID,
    MAILBOX_IFC_ACPU_TO_CCPU_SET_EFUSE_AUTHKEY,
    MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_AUTHKEY,
    MAILBOX_IFC_ACPU_TO_CCPU_SET_EFUSE_DEBUGMODE,
    MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_DEBUGMODE,
    MAILBOX_IFC_ACPU_TO_CCPU_GET_THERMAL,
    MAILBOX_IFC_ACPU_TO_CCPU_TEST_EFUSE_DISPLAY, 
	MAILBOX_IFC_ACPU_TO_CCPU_TEST_EFUSE_WR,
	MAILBOX_IFC_ACPU_TO_CCPU_GET_CARRIERID,
    MAILBOX_MAILCODE_ITEM_END(ACPU, CCPU, IFC),

    /* HIFI->CCPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(HIFI, CCPU, MSG),
    MAILBOX_MAILCODE_HIFI_TO_CCPU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_HIFI_TO_CCPU_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ITEM_END(HIFI, CCPU, MSG),

    /* HIFI->ACPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(HIFI, ACPU, MSG),
    MAILBOX_MAILCODE_HIFI_TO_ACPU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_HIFI_TO_ACPU_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_HIFI_TO_ACPU_AUDIO,
    MAILBOX_MAILCODE_HIFI_TO_ACPU_MISC,
    /*Modify_for_c_reset, l00212112,20130511, starts*/
    MAILBOX_MAILCODE_HIFI_TO_ACPU_CCORE_RESET_ID,
    /*Modify_for_c_reset, l00212112,20130511, ends*/
    MAILBOX_MAILCODE_ITEM_END(HIFI, ACPU, MSG),

    /* HIFI->BBE16 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(HIFI, BBE16, MSG),
    MAILBOX_MAILCODE_HIFI_TO_BBE16_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_HIFI_TO_BBE16_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ITEM_END(HIFI, BBE16, MSG),

    /* MCU->CCPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(MCU, CCPU, MSG),
    MAILBOX_MAILCODE_MCU_TO_CCPU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_MCU_TO_CCPU_VOS_MSG_URGENT,
    BSP_MAILBOX_CHANNEL_MCU_TO_CCPU_MCA_CH,
    MAILBOX_MAILCODE_ITEM_END(MCU, CCPU, MSG),

    /* MCU->CCPU ifc邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(MCU, CCPU, IFC),
    MAILBOX_IFC_MCU_TO_CCPU_TEST_CMP,
    MAILBOX_IFC_MCU_TO_CCPU_BASE_TEST2,
    MAILBOX_IFC_MCU_TO_CCPU_BASE_TEST1,
    MAILBOX_IFC_MCU_TO_CCPU_TEST,
    MAILBOX_MAILCODE_ITEM_END(MCU, CCPU, IFC),

    /* MCU->ACPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(MCU, ACPU, MSG),
    MAILBOX_MAILCODE_MCU_TO_ACPU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_MCU_TO_ACPU_VOS_MSG_URGENT,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_IFC_CH,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_IFC_RESPONSE_CH,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_SENSOR_CH,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_TP_CH,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_MCA_CH,
    MAILBOX_IFC_MCU_TO_ACPU_HUTAF_HLT,
    /*Modify_for_c_reset, l00212112,20130511, starts*/
    MAILBOX_MAILCODE_MCU_TO_ACPU_CCORE_RESET_ID,
    /*Modify_for_c_reset, l00212112,20130511, ends*/
    MAILBOX_MAILCODE_MCU_TO_ACPU_HIFI_RESET_ID,    /*Modify_for_hifi_reset, l00249396,2013-10-14*/
    MAILBOX_MAILCODE_ITEM_END(MCU, ACPU, MSG),

    /* MCU->ACPU 远程函数调用邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(MCU, ACPU, IFC),
    MAILBOX_IFC_MCU_TO_ACPU_TEST_CMP,
    MAILBOX_IFC_MCU_TO_ACPU_TEST,
    MAILBOX_IFC_MCU_TO_ACPU_PRINT,
    MAILBOX_MAILCODE_ITEM_END(MCU, ACPU, IFC),

    /* BBE16 -> HIFI 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(BBE16, HIFI, MSG),
    MAILBOX_MAILCODE_BBE16_TO_HIFI_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_BBE16_TO_HIFI_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ITEM_END(BBE16, HIFI, MSG),

    /* MCU->HIFI ifc邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(MCU, HIFI, IFC),
    MAILBOX_IFC_MCU_TO_HIFI_RETURN,
    MAILBOX_MAILCODE_ITEM_END(MCU, HIFI, IFC),

    /* HIFI->MCU ifc邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(HIFI, MCU, IFC),
    MAILBOX_IFC_HIFI_TO_MCU_VOTE,
    MAILBOX_MAILCODE_ITEM_END(HIFI, MCU, IFC),

	MAILBOX_MAILCODE_ENUM_BUTT
};

/*****************************************************************************
 实 体 名  : MAILBOX_RET_ENUM
 功能描述  : 邮箱接口返回值
*****************************************************************************/
#define    MAILBOX_OK                       0
#define    MAILBOX_ERRO                     0xF7654321
#define    MAILBOX_FULL                     0xF7654322
#define    MAILBOX_NOT_READY                0xF7654323   /*目标CPU邮箱未初始化*/
#define    MAILBOX_TARGET_NOT_READY         MAILBOX_NOT_READY   /*目标CPU邮箱未初始化*/
#define    MAILBOX_TIME_OUT                 0xF7654324     /*发送等待返回超时*/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/
/*远程调用发起函数生成器*/
#define DRV_IFC_GEN_CALL0(id, name)       _IFC_GEN_CALLx(0, id, name)
#define DRV_IFC_GEN_CALL1(id, name, ...)  _IFC_GEN_CALLx(1, id, name, __VA_ARGS__)
#define DRV_IFC_GEN_CALL2(id, name, ...)  _IFC_GEN_CALLx(2, id, name, __VA_ARGS__)
#define DRV_IFC_GEN_CALL3(id, name, ...)  _IFC_GEN_CALLx(3, id, name, __VA_ARGS__)
#define DRV_IFC_GEN_CALL4(id, name, ...)  _IFC_GEN_CALLx(4, id, name, __VA_ARGS__)
#define DRV_IFC_GEN_CALL5(id, name, ...)  _IFC_GEN_CALLx(5, id, name, __VA_ARGS__)
#define DRV_IFC_GEN_CALL6(id, name, ...)  _IFC_GEN_CALLx(6, id, name, __VA_ARGS__)

/*生成带各个参数的远程调用执行函数*/
#define DRV_IFC_GEN_EXEC0(name)      __IFC_GEN_EXECx(0, name)
#define DRV_IFC_GEN_EXEC1(name, ...) __IFC_GEN_EXECx(1, name, __VA_ARGS__)
#define DRV_IFC_GEN_EXEC2(name, ...) __IFC_GEN_EXECx(2, name, __VA_ARGS__)
#define DRV_IFC_GEN_EXEC3(name, ...) __IFC_GEN_EXECx(3, name, __VA_ARGS__)
#define DRV_IFC_GEN_EXEC4(name, ...) __IFC_GEN_EXECx(4, name, __VA_ARGS__)
#define DRV_IFC_GEN_EXEC5(name, ...) __IFC_GEN_EXECx(5, name, __VA_ARGS__)
#define DRV_IFC_GEN_EXEC6(name, ...) __IFC_GEN_EXECx(6, name, __VA_ARGS__)

/*注册远程调用执行函数*/
#define MAILBOX_IFC_REG(id, name) mailbox_ifc_register_exec(id ,IFC_GEN_EXEC_NAME(name));
typedef void (*mb_msg_cb)(
                void                   *user_handle,
                void                   *mail_handle,
                unsigned int           mail_len);

/*****************************************************************************
  9 全局变量声明
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/


unsigned int DRV_MAILBOX_SENDMAIL(
                unsigned int           MailCode,
                void                   *pData,
                unsigned int           Length);


unsigned int DRV_MAILBOX_REGISTERRECVFUNC(
                unsigned int           MailCode,
                mb_msg_cb               pFun,
                void                   *UserHandle);


unsigned int DRV_MAILBOX_READMAILDATA(
                void                   *MailHandle,
                unsigned char          *pData,
                unsigned int           *pSize);



/* TDS&LTE mailbox driver begin */
#if 0
typedef enum tagMAILBOX_SLEEP_PROC_E
{
    EN_MAILBOX_SLEEP_WAKEUP = 0,        /* 强制唤醒DSP后处理请求 */
    EN_MAILBOX_SLEEP_LOST,              /* DSP处于睡眠模式时丢弃该请求 */
    EN_MAILBOX_SLEEP_DIRECT,            /* 不判断DSP状态，直接写操作 慎用*/
    EN_MAILBOX_SLEEP_BUTT
}MAILBOX_SLEEP_PROC_E;

typedef enum tagMAILBOX_SERVICE_TYPE_E
{
    EN_MAILBOX_SERVICE_LTE_PS = 0,      /* LTE PS请求 */
    EN_MAILBOX_SERVICE_LTE_CT,          /* LTE CT请求 */
    EN_MAILBOX_SERVICE_LTE_BT,          /* LTE BT请求 */
    EN_MAILBOX_SERVICE_LTE_OM,          /* LTE OM邮箱请求 */
    EN_MAILBOX_SERVICE_LTE_COEX,        /* LTE WIFI coex */
    EN_MAILBOX_SERVICE_LTE_HS_DIAG,     /* LTE 可维可测低功耗数采请求 */
    EN_MAILBOX_SERVICE_LTE_RCM,         /* LTE RCM请求 */

    EN_MAILBOX_SERVICE_TDS_PS,          /* TDS PS请求 */
    EN_MAILBOX_SERVICE_TDS_CT,          /* TDS CT请求 */
    EN_MAILBOX_SERVICE_TDS_BT,          /* TDS BT请求 */
    EN_MAILBOX_SERVICE_TDS_OM,          /* TDS OM邮箱请求 */
    EN_MAILBOX_SERVICE_TDS_HS_DIAG,     /* TDS 可维可测低功耗数采请求 */
	EN_MAILBOX_SERVICE_TDS_RCM,         /* TDS RCM请求 */
#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    EN_MAILBOX_SERVICE_XSYS_RL,
    EN_MAILBOX_SERVICE_XSYS_OM,
    EN_MAILBOX_SERVICE_XSYS_BT,
    EN_MAILBOX_SERVICE_XSYS_CT,
    EN_MAILBOX_SERVICE_XSYS_HS_DIAG,
#endif
    EN_MAILBOX_SERVICE_RTT_SYS_CTRL,

    EN_MAILBOX_SERVICE_BUTT
}MAILBOX_SERVICE_TYPE_E;


typedef enum tagMAILBOX_INT_TYPE_E
{
    EN_MAILBOX_INT_SUBFRAME = 0,        /* 子帧中断 */
    EN_MAILBOX_INT_COM_IPC,             /* IPC中断 */
    EN_MAILBOX_INT_SP_IPC,              /* 专有邮箱IPC中断 */
    EN_MAILBOX_INT_BUTT
}MAILBOX_INT_TYPE_E;


typedef enum tagMAILBOX_ERRORCODE_E
{
    ERR_MAILBOX_NOT_INIT = 1,           /* 未初始化 */
    ERR_MAILBOX_PARAM_INCORRECT,        /* 参数错误 */
    ERR_MAILBOX_MEMORY_NOTENOUGH,       /* 空间不足 */
    ERR_MAILBOX_TIMEOUT,                /* 超时 */
    ERR_MAILBOX_COVER,                  /* 专有邮箱写操作时原语未读走，执行覆盖操作 */
    ERR_MAILBOX_WRITE_FAILED,           /* 邮箱写失败 */
    ERR_MAILBOX_READ_FAILED,            /* 邮箱读失败 */
    ERR_MAILBOX_READ_NULL,              /* 邮箱读到空信息 */
    ERR_MAILBOX_DSP_POWERDOWN,          /* DSP处于低功耗状态 */
    ERR_MAILBOX_OTHER,                  /* 其他错误 */
    ERR_MAILBOX_ERRORCODE_BUTT
}MAILBOX_ERRORCODE_E;
typedef enum tagMAILBOX_CALLBACK_TYPE_E
{
    EN_MAILBOX_CALLBACK_DSP_RUN = 0,        /*DSP RUN CALLBACK*/
    EN_MAILBOX_CALLBACK_DSP_RESUME,         /*DSP RESUME IRQ CALLBACK*/
    EN_MAILBOX_CALLBACK_DSP_BUTT
}MAILBOX_CALLBACK_TYPE;
typedef void (*BSP_MBX_RCM_CALLBACK_FUNC)(MAILBOX_CALLBACK_TYPE e);
void BSP_MailBox_Rcm_CallBackReg(BSP_MBX_RCM_CALLBACK_FUNC pFunc);


/*****************************************************************************
 Prototype      : BSP_MailBox_ComMsgInit
 Description    : 通用邮箱的初始化(提供给协议栈调用，
                  接口内部实现与DSP握手信息的初始化)
 Input          : BSP_U32 ulworkingmode
 Return Value   : None
*****************************************************************************/
void BSP_MailBox_ComMsgInit(unsigned int ulworkingmode);


/*****************************************************************************
 Prototype      : BSP_MailBox_IsDspOk
 Description    : 与DSP的握手函数，判断DSP是否初始化结束
                  (提供给协议栈调用，如果握手不成功，协议栈需要循环多次调用)
 Input          : None
 Return Value   : BSP_TRUE: DSP初始化结束，握手成功
                  BSP_FALSE:DSP没有初始化结束，握手不成功
*****************************************************************************/
int BSP_MailBox_IsDspOk(void);

/*****************************************************************************
 Prototype       : BSP_MailBox_ComMsgWrite
 Description     : 邮箱写接口(每次只支持写一条原语)
 Input           : enMbxType    邮箱的业务类型
                   pData        数据指针
                   ulLen        数据长度(以字节为单位)
                   enProcType   DSP睡眠时的处理类型
 Output          : None.
 Return Value    : BSP_OK: 成功
                   其他: 失败
*****************************************************************************/
unsigned int BSP_MailBox_ComMsgWrite(MAILBOX_SERVICE_TYPE_E enMbxType,
                               void* pData,
                               unsigned int ulLen,
                               MAILBOX_SLEEP_PROC_E enProcType);

typedef void (*BSP_MBX_NOTIFY_FUNC)(MAILBOX_INT_TYPE_E enIntType);

/*****************************************************************************
 Prototype       : BSP_MailBox_ComNotifyReg
 Description     : 邮箱数据到达通知注册接口（子帧中断即使没数据也要回调）
 param           : enMbxType        邮箱的业务类型
                   pFun             邮箱数据到达处理函数
 Return Value    : BSP_OK: 成功
                   ERR_MAILBOX_NOT_INIT
                   ERR_MAILBOX_PARAM_INCORRECT
*****************************************************************************/
unsigned int BSP_MailBox_ComNotifyReg(MAILBOX_SERVICE_TYPE_E enMbxType,
                                BSP_MBX_NOTIFY_FUNC pFun);

/*****************************************************************************
 Prototype       : BSP_MailBox_IntTypeSet
 Description     : 设置PS邮箱中断源接口(默认IPC中断方式)
                   只用于PS邮箱，OM邮箱无中断上报
 param           : None
 Return Value    : 0: 成功; 其他: 设置的type非法
*****************************************************************************/
unsigned int BSP_MailBox_IntTypeSet(MAILBOX_INT_TYPE_E enIntType);

/*****************************************************************************
 Prototype       : BSP_MailBox_ComMsgSize
 Description     : 获取通用邮箱中第一条原语的长度
                   (提供给上层调用，驱动不提供邮箱中原语条数接口，需要上层循环
                   读，以避免上层漏掉中断通知时造成邮箱中原语缓存)
 param           : enMbxType        邮箱的业务类型
 Return Value    : 0:  没有获取原语（邮箱为空，或有其他类型的原语未读走）
                   其他值: 邮箱第一条原语的长度
*****************************************************************************/
unsigned int BSP_MailBox_ComMsgSize(MAILBOX_SERVICE_TYPE_E enMbxType);

/*****************************************************************************
 Prototype       : BSP_MailBox_ComMsgRead
 Description     : 邮箱读接口
                   (此接口会唤醒DSP，需要BSP_MailBox_ComMsgSize返回非空时调用)
 param           : enMbxType    邮箱的业务类型
                   pData        接收的数据缓冲区指针(存储原语信息，不包含原语长度)
                   ulLen        pData缓冲区的size(以字节为单位)
                   enProcType   DSP睡眠时的处理类型
 Return Value    : 0 : 成功
                   其他 : 失败
*****************************************************************************/
unsigned int BSP_MailBox_ComMsgRead(MAILBOX_SERVICE_TYPE_E enMbxType,
                              void* pData,
                              unsigned int ulLen,
                               MAILBOX_SLEEP_PROC_E enProcType);


/*****************************************************************************
 Prototype      : BSP_MailBox_SpULMsgHeadInit
 Description    : 专有（Special Mailbox）上行邮箱的消息头初始化接口
                  (初始化上行邮箱每条原语的消息头，提供给协议栈，需要调用多次
                  由调用者保证调用接口时DSP处于非睡眠状态)
 Input          :
                ulAddr : 专有邮箱的地址
                ulSize : 初始值的大小(以字节为单位)
                ucData : 初始值
 Return Value   None
*****************************************************************************/
void BSP_MailBox_SpULMsgHeadInit(unsigned int ulAddr,
                                 unsigned int ulSize,
                                 unsigned char ucData);

/*****************************************************************************
 Prototype      : BSP_MailBox_SpDLMsgHeadInit
 Description    : 专有（Special Mailbox）下行邮箱的消息头初始化接口
                  (初始化上行邮箱每条原语的消息头，提供给协议栈，需要调用多次
                  由调用者保证调用接口时DSP处于非睡眠状态)
 Input          :
                ulAddr : 专有邮箱的地址
                ulSize : 初始值的大小(以字节为单位)
                ucData : 初始值
 Return Value   None
*****************************************************************************/
void BSP_MailBox_SpDLMsgHeadInit(unsigned int ulAddr,
                                 unsigned int ulSize,
                                 unsigned char ucData);

/*****************************************************************************
 Prototype      : BSP_MailBox_SpMsgWrite
 Description    : 专有邮箱（Special Mailbox）的写接口
                  (由调用者保证调用接口时DSP处于非睡眠状态)
 Input          :
                ulAddr : 专有邮箱的偏移地址（从消息头开始的地址）
                ulSize : 初始值的大小(以字节为单位)
                pData  : 数据信息(不包含消息头信息)
 Return Value   :
                BSP_OK: 表示操作成功
                ERR_MAILBOX_COVER: 表示前一条原语未被读走，覆盖
                ERR_MAILBOX_WRITE_FAILED: 写失败
                ERR_MAILBOX_NOT_INIT: 邮箱未初始化
                ERR_MAILBOX_PARAM_INCORRECT: 参数错误
*****************************************************************************/
unsigned int BSP_MailBox_SpMsgWrite(unsigned int ulAddr,
                               unsigned int ulSize,
                               void* pData);

/*****************************************************************************
 Prototype      : BSP_MailBox_SpMsgRead
 Description    : 专有邮箱（Special Mailbox）的读接口
                  (由调用者保证调用接口时DSP处于非睡眠状态)
 Input          :
                ulAddr : 专有邮箱的偏移地址（从消息头开始的地址）
                ulSize : 初始值的大小(以字节为单位)
                pData  : 数据信息(不包含消息头信息)
 Return Value   :
                BSP_OK: 读取成功
                ERR_MAILBOX_READ_NULL: 无数据
                ERR_MAILBOX_NOT_INIT: 邮箱未初始化
                ERR_MAILBOX_PARAM_INCORRECT: 参数错误
*****************************************************************************/
unsigned int BSP_MailBox_SpMsgRead(unsigned int ulAddr,
                              unsigned int ulSize,
                              void* pData);

/*****************************************************************************
 Prototype       : BSP_MailBox_SpNotifyReg
 Description     : 专有邮箱数据到达通知注册接口
 param           : pFun             邮箱数据到达处理函数
 Return Value    : BSP_OK: 成功
                   其他: 失败
*****************************************************************************/
unsigned int BSP_MailBox_SpNotifyReg(BSP_MBX_NOTIFY_FUNC pFun);

/*****************************************************************************
 Prototype       : BSP_MailBox_SpNotifyEnable
 Description     : 专有邮箱数据到达通知中断使能接口
 param           : None
 Return Value    : void
*****************************************************************************/
void BSP_MailBox_SpNotifyEnable(void);

/*****************************************************************************
 Prototype       : BSP_MailBox_SpNotifyDisable
 Description     : 专有邮箱数据到达通知中断去使能接口
 param           : None
 Return Value    : void
*****************************************************************************/
void BSP_MailBox_SpNotifyDisable(void);


/*****************************************************************************
 Prototype       : BSP_Mailbox_ForbidDspSleep
 Description     : 禁止DSP睡眠接口
 Input           : enProcType     DSP睡眠时的处理类型
                    EN_MAILBOX_SLEEP_WAKEUP : 强制唤醒时会有等待处理，
                                              不能在中断回调中输入此参数
                                              只能在任务中输入此参数
                    EN_MAILBOX_SLEEP_LOST   : 中断回调和任务中都可输入此参数

 Return Value    : BSP_OK: 成功,DSP当前没睡眠，且已设置禁止DSP进入睡眠
                   ERR_MAILBOX_DSP_POWERDOWN : 当输入为EN_MAILBOX_SLEEP_LOST,
                                               DSP当前处于低功耗状态,设置失败
                   ERR_MAILBOX_TIMEOUT       : 当输入为EN_MAILBOX_SLEEP_WAKEUP,
                                               唤醒DSP超时(10s)
*****************************************************************************/
unsigned int BSP_Mailbox_ForbidDspSleep(MAILBOX_SLEEP_PROC_E enProcType);


/*****************************************************************************
 Prototype       : BSP_Mailbox_AllowDspSleep
 Description     : 允许DSP睡眠接口
 Input           : None
 Return Value    : None
*****************************************************************************/
void BSP_Mailbox_AllowDspSleep(void);



typedef int (*BSP_MBX_GET_STATUS_FUNC)(void);

/*****************************************************************************
 Prototype       : BSP_MailBox_GetDspStatusReg
 Description     : 获取DSP睡眠状态回调函数注册接口，低功耗模块调用
 param           : pFun             获取DSP睡眠状态处理函数
 Return Value    : BSP_VOID
*****************************************************************************/
void BSP_MailBox_GetDspStatusReg(BSP_MBX_GET_STATUS_FUNC pFun);


typedef void (*BSP_MBX_DSP_AWAKE_FUNC)(void);

/*****************************************************************************
 Prototype       : BSP_MailBox_DspForceAwakeReg
 Description     : DSP强制唤醒回调函数注册接口，低功耗模块调用
 param           : pFun             DSP强制唤醒处理函数
 Return Value    : BSP_VOID
*****************************************************************************/
void BSP_MailBox_DspForceAwakeReg(BSP_MBX_DSP_AWAKE_FUNC pFun);


/*****************************************************************************
 Prototype       : BSP_MailBox_GetSpMsgStatusReg
 Description     : 获取专有邮箱允许DSP睡眠状态回调函数注册接口，PS调用
 param           : pFun             获取DSP睡眠状态处理函数
 Return Value    : BSP_VOID
*****************************************************************************/
void BSP_MailBox_GetSpMsgStatusReg(BSP_MBX_GET_STATUS_FUNC pFun);


/*****************************************************************************
 Prototype       : BSP_MailBox_DspAwakeProc
 Description     : DSP唤醒后的邮箱处理接口，低功耗模块调用
 param           : BSP_VOID
 Return Value    : BSP_VOID
*****************************************************************************/
void BSP_MailBox_DspAwakeProc(MAILBOX_CALLBACK_TYPE cbType);

/*****************************************************************************
 Prototype       : BSP_MailBox_IsAllowDspSleep
 Description     : 查询是否允许DSP睡眠的处理接口，低功耗模块调用
                    调用此接口时低功耗模块保证DSP未睡眠
                    此接口只检测PS邮箱和专有邮箱，不检查OM邮箱(OM原语无时延要求)
 param           : BSP_VOID
 Return Value    : BSP_BOOL: TRUE-allow;FALSE-forbid
*****************************************************************************/
int BSP_MailBox_IsAllowDspSleep(void);

/*****************************************************************************
 Prototype       : BSP_UniMailboxWrite
 Description     : 邮箱的通用写接口(由调用者保证传入的地址的合法性)
                   本接口实现类似memcpy函数的作用
                   接口内部保证DSP的唤醒操作，如果唤醒失败，返回ERR_MAILBOX_TIMEOUT
 Return Value    :
                   BSP_OK
                   ERR_MAILBOX_PARAM_INCORRECT
                   ERR_MAILBOX_TIMEOUT
*****************************************************************************/
unsigned int BSP_UniMailboxWrite(void *pDst, void *pSrc, unsigned int ulLen);

/*****************************************************************************
 Prototype       : BSP_MailboxAbnormalProc
 Description     : 邮箱异常处理
                   协议栈接收消息超时后调用此接口保存邮箱相关信息
                   与协议栈商定，前0.5K保存协议栈信息，后3.5K保存邮箱信息
 Input           : ucData       缓存首地址
 Return Value    : NONE
*****************************************************************************/
void BSP_MailboxAbnormalProc(unsigned char *ucData);
#endif
/* TDS&LTE mailbox driver end */



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of drv_mailbox.h */


