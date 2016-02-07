

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "drv_mailbox.h"
#include "drv_mailbox_cfg.h"
#include "drv_mailbox_gut.h"

#if defined(_DRV_LLT_)
#ifndef CONFIG_UTTEST
#include "drv_mailbox_stub.h"
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
#undef  _MAILBOX_FILE_
#define _MAILBOX_FILE_   "table"
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 定义整个平台所有邮箱的物理通道内存及硬件资源配置，全局定义 */
/* MSG:消息通道，IFC:核间调用通道 */
MAILBOX_EXTERN struct mb_cfg g_mailbox_global_cfg_tbl[] =
{
    /* CCPU发送到其他核的通道 */
    MAILBOX_CHANNEL_COMPOSE(CCPU, MCU,  MSG),
    MAILBOX_CHANNEL_COMPOSE(CCPU, MCU,  IFC),
    MAILBOX_CHANNEL_COMPOSE(CCPU, HIFI, MSG),
    MAILBOX_CHANNEL_COMPOSE(CCPU, ACPU, IFC),
    MAILBOX_CHANNEL_COMPOSE(CCPU, ACPU, MSG),

    /* ACPU发送到其他核的通道 */
    MAILBOX_CHANNEL_COMPOSE(ACPU, MCU,  MSG),
    MAILBOX_CHANNEL_COMPOSE(ACPU, MCU,  IFC),
    MAILBOX_CHANNEL_COMPOSE(ACPU, HIFI, MSG),
    MAILBOX_CHANNEL_COMPOSE(ACPU, CCPU, IFC),
    MAILBOX_CHANNEL_COMPOSE(ACPU, CCPU, MSG),

    /*MCU发送到其他核的通道*/
    MAILBOX_CHANNEL_COMPOSE(MCU, CCPU,  MSG),
    MAILBOX_CHANNEL_COMPOSE(MCU, CCPU,  IFC),
    MAILBOX_CHANNEL_COMPOSE(MCU, ACPU,  MSG),
    MAILBOX_CHANNEL_COMPOSE(MCU, ACPU,  IFC),
    MAILBOX_CHANNEL_COMPOSE(MCU, HIFI,  IFC),

    /*HIFI发送到其他核的通道*/
    MAILBOX_CHANNEL_COMPOSE(HIFI, CCPU, MSG),
    MAILBOX_CHANNEL_COMPOSE(HIFI, ACPU, MSG),
    MAILBOX_CHANNEL_COMPOSE(HIFI, MCU, IFC),

    /*结束标志*/
    {MAILBOX_MAILCODE_INVALID,  0,  0, 0}

};

/*平台邮箱通道句柄的内存池空间*/
MAILBOX_EXTERN struct mb_buff     g_mailbox_channel_handle_pool[MAILBOX_CHANNEL_NUM];

/*平台邮箱用户回调句柄的内存池空间*/
MAILBOX_EXTERN struct mb_cb    g_mailbox_user_cb_pool[MAILBOX_USER_NUM];

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

