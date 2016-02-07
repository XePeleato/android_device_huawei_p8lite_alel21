/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 *  reset.h    reset module interface declare
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */
#ifndef __RESET_SUB_CCORE_H__
#define __RESET_SUB_CCORE_H__

#include <linux/hisi/reset.h>

/*****************************************************************************
 宏定义
*****************************************************************************/


/*****************************************************************************
数据结构定义
*****************************************************************************/
#include <huawei_platform/log/log_exception.h>
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/rtc.h>

#define AP_LOG_PATH_APR             "/data/hisi_logs/ap_log/coredump/"
#define CP_LOG_PATH_APR             "/data/hisi_logs/cp_log/coredump/"
#define HIFI_LOG_PATH_APR           "/data/hisi_logs/hifi_log/coredump/"

#define MAX_APR_CMD_LEN		256
#define MAX_APR_FILE_LEN	64
#define MAX_APR_LOCAL_TIME_STR		32
typedef struct
{
    sreset_mailbox_msg_mcu              smailboxmsg_mcu;
    AP_HIFI_CCPU_RESET_REQ_STRU               smailboxmsg_hifi;
    int               iccore_reset;
    int               ivalue_reg1;      /*保存寄存器原始值，用于做恢复*/
    int               ivalue_reg2;      /*保存寄存器原始值，用于做恢复*/
    struct semaphore     sem_wait_ccorereset;
    struct semaphore     sem_wait_ccorereset_ok;
    struct semaphore     sem_wait_hifireset;
    struct semaphore     sem_wait_hifi_msg;
    struct semaphore     sem_wait_mcu_msg;
    struct semaphore     sem_wait_mcu_msg_hifireset;
} sreset_mgr_assistant_hifi;


/*****************************************************************************
函数引用
*****************************************************************************/

#endif

