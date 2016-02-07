/*******************************************************************************

                  版权所有 (C), 2001-2012, 华为技术有限公司

 *******************************************************************************
  文 件 名   : usb_shell.h
  版 本 号   : 初稿
  作    者   : 张彪 kf67996
  生成日期   : 2012年9月13日
  最近修改   :
  功能描述   : USB ASHELL/CSHELL模块头文件信息
  函数列表   :
  修改历史   :
  1.日    期   : 2012年9月13日
    作    者   : 张彪 kf67996
    修改内容   : 创建文件

*******************************************************************************/

/*******************************************************************************
  1 其他头文件包含
*******************************************************************************/

#ifndef    _BSP_USB_SHELL_H_
#define    _BSP_USB_SHELL_H_
#include <linux/tty.h>
#include <linux/console.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>

#include "drv_usb_shell.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/**************************************************************************
  list 定义
**************************************************************************/

/**************************************************************************
  配置宏定义
**************************************************************************/
#define CSHELL_ICC_CHANNEL_FIFO_SIZE    8192
#define CSHELL_ICC_CHANNEL_ID           31
#define CSHELL_ICC_CHANNEL_INVALID_ID   0xFFFF
#define CSHELL_SEND_BUFFER_NUM          8
#define CSHELL_BUFFER_SIZE              (16*1024)
#define ASHELL_SEND_BUFFER_NUM          16
#define ASHELL_BUFFER_SIZE              2048

#ifdef _DRV_LLT_
#define SHELL_STATIC
#else
#define SHELL_STATIC                    static
#endif

/* it should be the same as MAX_ACM_INSTANCES in android.c */
#define USB_TTY_INSTANCES_NUM           7

#define SHELL_OK                        0
#define SHELL_ERROR                     (-1)

/*Modify_for_c_reset, l00212112,20130511, starts*/
#define CSHELL_VALID                   (1)
#define CSHELL_INVALID                 (0)
/*Modify_for_c_reset, l00212112,20130511, ends*/

/*move from drv_usb_if.h begin*/
#define USB_TTY_NAME0                 "ttyGS0"
#define USB_TTY_NAME1                 "ttyGS1"
#define USB_TTY_NAME2                 "ttyGS2"
#define USB_TTY_NAME3                 "ttyGS3"
#define USB_TTY_NAME4                 "ttyGS4"
#define USB_TTY_NAME5                 "ttyGS5"
#define USB_TTY_NAME6                 "ttyGS6"

#define USB_TTY_ASH_NAME              USB_TTY_NAME0
#define USB_TTY_AT_NAME               USB_TTY_NAME1
#define USB_TTY_CTRL_NAME             USB_TTY_NAME2
#define USB_TTY_OM_NAME               USB_TTY_NAME3
#define USB_TTY_CSH_NAME              USB_TTY_NAME4
#define USB_TTY_LTEOM_NAME            USB_TTY_NAME5
#define USB_TTY_GPS_NAME              USB_TTY_NAME6

/*move from drv_usb_if.h end*/

/**************************************************************************
  功能宏定义
**************************************************************************/


/**************************************************************************
  枚举定义
**************************************************************************/

/*move from drv_usb_if.h begin*/
enum usb_tty_type {
    USB_TTY_ASH_TYPE = 0,
    USB_TTY_AT_TYPE,
    USB_TTY_CTRL_TYPE,
    USB_TTY_OM_TYPE,
    USB_TTY_CSH_TYPE,
    USB_TTY_LTEOM_TYPE,
    USB_TTY_GPS_TYPE,
    USB_TTY_RESERVED_TYPE
};

enum usb_tty_index {
    USB_TTY_ASH_INDEX = 0,
    USB_TTY_AT_INDEX,
    USB_TTY_CTRL_INDEX,
    USB_TTY_OM_INDEX,
    USB_TTY_CSH_INDEX,
    USB_TTY_LTEOM_INDEX,
    USB_TTY_GPS_INDEX,
    USB_TTY_RESERVED_INDEX
};
/*move from drv_usb_if.h end*/

/**************************************************************************
  STRUCT定义
**************************************************************************/
struct shell_mem_info {
    char          *pbuf;    /* BUF实地址 */
    char          *vbuf;   /* BUF虚地址 */
    unsigned int  used;
    int           free;
    int           idx;
    struct shell_mem_info *next;
};

struct ashell_ctx_t {
    struct tty_struct   *tty;              /* tty结构体 */
    void                *acm_ctx;          /* acm结构体 */
    struct console      port_console;      /* console结构体 */
    spinlock_t          lock;              /* 自旋锁 */
    int                 enable;            /* USB端口使能 */
    int                 port_open;         /* USB端口打开 */
    int                 tx_idx;            /* 发送BUF索引值 */
    struct delayed_work tx_work;           /* 发送work */
    struct shell_mem_info tx_mem[ASHELL_SEND_BUFFER_NUM];
};

struct cshell_mem_handle_t {
    unsigned char   *buf;
    unsigned long   buf_size;
    bool            buf_valid;
    unsigned long   index;
    struct cshell_mem_handle_t *next;
};

struct cshell_ctx_t{
    unsigned int        icc_chan_id;        /* ICC通道ID */
    unsigned int        icc_opened;         /* ICC通道打开FLAG */
    int                 icc_fd;             /* ICC通道句柄 */
    int                 usb_fd;             /* ACM设备句柄 */
    int                 icc_valid;          /* icc 有效标志 */
    int                 usb_valid;	        /* usb 有效标志 */
    spinlock_t          icc_lock;           /* icc自旋锁 */
    spinlock_t          usb_lock;           /* usb自旋锁 */
    char                *icc_rx_buf;        /* icc接收buf */
    char                *usb_rx_buf;        /* usb接收buf */
    struct work_struct  icc_tx_work;        /* icc发送work */
    struct work_struct  usb_tx_work;        /* usb发送work */
    int                 icc_tx_idx;         /* icc发送buf索引 */
    int                 usb_tx_idx;         /* usb发送buf索引 */
    struct shell_mem_info   icc_tx_mem[CSHELL_SEND_BUFFER_NUM];  /* icc发送mem */
    struct shell_mem_info   usb_tx_mem[CSHELL_SEND_BUFFER_NUM];  /* usb发送mem */
};
/*move from drv_usb_if.h begin*/
struct usb_tty_t {
    enum usb_tty_index  index;
    enum usb_tty_type   type;
    char                *name;
};
/*move from drv_usb_if.h end*/
/**************************************************************************
  函数声明
**************************************************************************/
int  cshell_init(void);
void cshell_uninit(void);
int  cshell_icc_init(void);
void cshell_icc_uninit(void);
void cshell_udi_open_cb(void);
void cshell_udi_close_cb(void);
void cshell_shell_rx_cb(void);
unsigned int cshell_write_cb(unsigned int id);
unsigned int cshell_read_cb (unsigned int id, int size);
unsigned int cshell_event_cb(unsigned int id, unsigned int event, void *Param);

int  usb_tty_exist(char *name);
int  ashell_console_register(void);
struct tty_driver *ashell_console_device(struct console *cons, int *gidx);
int  ashell_console_setup(struct console *cons, char *options);
void ashell_console_write(struct console *cons, const char *buf, unsigned count);
int  ashell_start_io(void);
int  ashell_stop_io(void);
int  ashell_init(void);

/*Modify_for_c_reset, l00212112,20130511, starts*/
extern int bsp_ccpu_rest_cshell_handle(int status);
/*Modify_for_c_reset, l00212112,20130511, ends*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* _BSP_USB_SHELL_H_ */


