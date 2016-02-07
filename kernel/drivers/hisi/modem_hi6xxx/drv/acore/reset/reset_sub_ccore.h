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

#include <mdrv_sysboot.h>
#include <mdrv_udi.h>
#include <linux/hisi/reset.h>
#include "excDrv.h"
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
/*****************************************************************************
 宏定义
*****************************************************************************/
#define RESET_LOG_PATH         	("/data/hisi_logs/cp_log/coredump/")
#define RESET_LOG_FILE               ("reset.log")
#define RESET_LOG_FILE_PATH_SIZE     (64)
#define RESET_LOG_MAX_SIZE (1024)

#define RESET_LOG_CPDIR_PROVALUE    0774
#define RESET_LOG_OWNER_UID    0		/*root*/
#define RESET_LOG_OWNER_GID    1000		/*system*/


/*------------------Portland----------------------*/
#define EXCH_STATE_TASK             (0x10)
#define EXCH_STATE_INT              (0x80)
#define EXCH_RTC_TIME_LEN           16

/*****************************************************************************
数据结构定义
*****************************************************************************/
typedef enum
{
    EXCH_CCORE_S_NORMAL,
    EXCH_CCORE_S_ARM,
    EXCH_CCORE_S_OS,
    EXCH_CCORE_S_WDT,
    EXCH_CCORE_S_SF,
    EXCH_CCORE_S_NOC,
    EXCH_CCORE_S_SD,
    EXCH_CCORE_S_SCI,
    EXCH_CCORE_S_USB,
    EXCH_CCORE_S_ONLINE,
    EXCH_CCORE_S_SSI_READ,
    EXCH_CCORE_S_SSI_WRITE,
    EXCH_CCORE_S_MODEM_REBOOT_BYRIL,
    EXCH_CCORE_S_MODEM_PANIC_BYRIL,
    EXCH_CCORE_S_IRQ_REQUIRE_ERROR,
    EXCH_CCORE_S_SC_ERR = 0x99,
    EXCH_CCORE_S_PMU = 0xAA,
    EXCH_CCORE_S_PMU_ERR = 0xDD,
} EXCH_SOURCE_CCORE;

typedef struct
{
    unsigned char   product;           /* 0x00  */
    unsigned char   rsv1;
    unsigned char   rsv2;
    unsigned char   rsv3;

    unsigned char   version[32];       /* 0x04  */

    unsigned int    whyReboot;         /* 0x24  */
    unsigned char   excSave;           /* 0x28  */
    unsigned char   omLogSave;         /* 0x29  */
    unsigned char   omMsgSave;         /* 0x2A  */
    unsigned char   cpuViewSave;       /* 0x2B  */
    unsigned char   wdtSave;           /* 0x2C  */
    unsigned char   sdExcSave;         /* 0x2D  */
    unsigned char   sciExcSave;        /* 0x2E  */
    unsigned char   usbExcSave;        /* 0x2F  */

    unsigned int    rebootTask;        /* 0x30  */  /*rebootTaskPid*/
    unsigned int    rebootTicks;       /* 0x34  */
    unsigned int    modId;             /* 0x38  */
    int             arg1;              /* 0x3C  */

    int             arg2;              /* 0x40  */
    unsigned int    omTimer;           /* 0x44  */
    unsigned int    vec;               /* 0x48  */
    unsigned int    cpsr;              /* 0x4C  */

    unsigned int    regSet[18];        /* 0x50  */

    unsigned char   taskName[16];      /* 0x98  */  /*lastTaskName*/

    unsigned int    taskId;            /* 0xA8 */  /*lastTaskId*/
    unsigned int    taskTicks;         /* 0xAC */  /*lastTaskTicks*/

    unsigned int    intId;             /* 0xB0  */ /*lastIntId*/
    unsigned int    intTicks;          /* 0xB4  */ /*lastIntTicks*/
    unsigned int    subIntId;          /* 0xB8  */
    unsigned int    subIntTicks;       /* 0xBC  */

    unsigned int    rebootstate;       /* 0xC0 */
    unsigned int    reboot_src;        /* 0xC4  */
    unsigned int    excSize;           /* 0xC8  */
    unsigned int    omLogSize;         /* 0xCC  */

    unsigned int    omMsgSize;         /* 0xD0 */
    unsigned int    freeHeapSize;      /* 0xD4 */
    unsigned int    maxFreeBlockSize;  /* 0xD8 */
    unsigned int    spinlock;          /* 0xDC */

    cores_share_info_t  share_addr;                 /* 0xE0  */
    cores_share_info_t  share_value;                /* 0xF0  */
    unsigned char       rtcTime[EXCH_RTC_TIME_LEN]; /* 0x100 */

    /*填充空白，使得 Addr 段能够从0x140这个固定地址开始定义*/
    unsigned int    fillReserved0[0xC];    /*0x110*/

    /*address段，固定在0x140偏移*/
    unsigned int    osMsgAddr;         /* 0x140  */
    unsigned int    taskTcbAddr;       /* 0x144  */   /*lastTaskTcbAddr*/
    unsigned int    customAddr;        /* 0x148  */   /*customArgAddr*/
    unsigned int    intAddr;           /* 0x14c  */

    unsigned int    subintAddr;        /* 0x150  */
    unsigned int    taskSwitchAddr;    /* 0x154  */
    unsigned int    intStackAddr;      /* 0x158  */   /*lastIntStackAddr*/
    unsigned int    taskStackAddr;     /* 0x15c  */   /*lastTaskStackAddr*/

    unsigned int    taskExcStackAddr;  /* 0x160 */
    unsigned int    cpuViewAddr;       /* 0x164 */
    unsigned int    taskAllTcbAddr;    /* 0x168 */
    unsigned int    omLogAddr;         /* 0x16c */

    unsigned int    omMsgAddr;         /* 0x170 */
    unsigned int    sdExcAddr;         /* 0x174 */
    unsigned int    sciExcAddr;        /* 0x178 */
    unsigned int    usbExcAddr;        /* 0x17c */

    unsigned int    customFuncQueueAddr; /* 0x180 */
    unsigned int    customTaskQueueAddr; /* 0x184 */
    unsigned int    customTaskInfoAddr;  /* 0x188 */
    unsigned int    customFuncInfoAddr;  /* 0x18C */

    unsigned int    allTaskNameAddr;     /* 0x190 */
    unsigned int    intSwitchAddr;       /* 0x194 */
    unsigned int    excBaseAddr;         /* 0x198 */
    unsigned int    taskAllStaskAddr;    /* 0x19c */
    unsigned int    memInfoAddr;         /* 0x1a0 */
    unsigned int    sysState;            /* 0x1a4 */
    unsigned int    cacheState;          /* 0x1a8 */
    unsigned int    freqInfo[0xf];       /* 0x1ac */
    unsigned int    armCP15[6];          /* 0x1e8 */

    /*尾部填充，   使struEndOffset为0x200*/
    /*unsigned int    fillReserved1[6];   *//* 0x1f8 */

    /*结束地址，固定为0x200*/
    unsigned int    struEndOffset[1] ;      /* 0x200 */
}EH_ROOT_CCORE;

typedef struct
{
    sreset_mailbox_msg_mcu              smailboxmsg_mcu;
    AP_HIFI_CCPU_RESET_REQ_STRU               smailboxmsg_hifi;
    int               iccore_reset;
    int               ivalue_reg1;      /*保存寄存器原始值，用于做恢复*/
    int               ivalue_reg2;      /*保存寄存器原始值，用于做恢复*/
	UDI_HANDLE        ccorereset_IccFd;
	ICC_CHAN_ATTR_S   attr;
	UDI_OPEN_PARAM_S    ccorereset_IccParam;
    struct semaphore     sem_wait_ccorereset;
    struct semaphore     sem_wait_ccorereset_ok;
    struct semaphore     sem_wait_hifireset;
    struct semaphore     sem_wait_hifi_msg;
    struct semaphore     sem_wait_mcu_msg;
    struct semaphore     sem_wait_mcu_msg_hifireset;
} sreset_mgr_assistant_ccore;


/*****************************************************************************
函数引用
*****************************************************************************/
extern void reset_ccore_up_semaphone(void);
extern void reset_ccore_up_byhimself(int is_normal);
extern int ccorereset_doreset(void);
extern int ccorereset_setCresetFlag(void);
extern int ccorereset_dostart(void);

#endif

