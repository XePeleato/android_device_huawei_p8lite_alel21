/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  hisi_dmesg.h
*
*   作    者 :
*
*   描    述 :  打印输出处理头文件
*
*   修改记录 :
*************************************************************************/


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/spinlock.h>

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define DUMP_PRINT_MAGIC        (0x32324554)

/*****************************************************************************
  2 枚举定义
*****************************************************************************/


/*****************************************************************************
  2 结构体
*****************************************************************************/
typedef struct
{
    u32     magic;          /* magic number for print section */
    u32     log_read;       /* read pointer, update by acore */
    u32     log_write;      /* write pointer, update by dmesg_write */
    u32     logged_chars;   /* number of chars in log buffer */
    u32     log_buf_len;    /* buffer length */
    u32     w_mark;         /* water mark, if logged chars exceed w_mark, ccore send IPC to acore */
    u32     full_flag;      /* set this flag when ccore send IPC, acore clear this flag after data consumption */
    u32     reserved;       /* reserved for 32-byte align */
    u32     log_buf_l32;    /* data area low 32bit*/
    u32     log_buf_h32;    /* data area high 32bit: not used*/
}log_mem_stru;

typedef struct
{
    struct miscdevice	misc;       /* misc device */
    wait_queue_head_t	wq;         /* wait queue */
    u32                 in_use;     /* whether device is in use */
    u32                 sleep_flag; /* whether app is sleep */
    spinlock_t          lock;
    struct list_head	logs;
    log_mem_stru      * log_mem;
}logger_info_stru;

/* log缓冲区满阈值，buffer空闲空间小于200字符，认为buffer已满 */
#define LOG_BUFFER_FULL_THRESHOLD   200
/* log task调度间隔，2s */
#define LOG_TASK_SCHED_TIME         200
/*当log overlap时，丢弃的字节数*/
#define LOG_DROPPED_SIZE  1024
/* log buffer满，输出提示 */
#define LOG_OVERLAP_MESSAGE         "LOG OVERLAP, DATA DROPPED!\n"
/* log buffer满，输出提示 */
#define LOG_DROPPED_MESSAGE         "log buffer full, data dropped\n"
/* ccore log设备名 */
#define CCORE_LOG_DEV_NAME          "log_ccore"
/* mcore log设备名 */
#define MCORE_LOG_DEV_NAME          "log_mcore"

#ifndef BSP_TRUE
#define BSP_TRUE            (1)
#endif

#ifndef BSP_FALSE
#define BSP_FALSE           (0)
#endif

void log_buff_info(void);

/*****************************************************************************
  5 错误码定义
*****************************************************************************/


