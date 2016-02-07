/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : acm_ldisc.h
  版 本 号   : 初稿
  作    者   : 夏青 00195127
  生成日期   : 2012年9月13日
  最近修改   :
  功能描述   : acm_ldisc.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年9月13日
    作    者   : 夏青 00195127
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include <linux/interrupt.h>


#ifndef __ACM_LDISC_H__
#define __ACM_LDISC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#ifndef OK
#define OK 0
#endif

#ifndef ERROR
#define ERROR -1
#endif

#ifndef USB_NODATA
#define USB_NODATA -2
#endif

#ifndef NULL
#define NULL ((void)*)0
#endif

#define ACM_LD_ROOM 65536

#define ACM_LD_DBG(args,...) do {\
    printk("%d:%s:",__LINE__,__func__);\
    printk(args,##__VA_ARGS__);\
}while(0)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/* acm 读写mem结构体*/
struct acm_mem_info{
    struct list_head list;        /*链表头*/
    void             *mem;        /*buff地址*/
    int              valid_size;  /*buff总长度*/
    unsigned char *  current_pos; /*指示当前需要读写地址*/
    unsigned int     left_size;   /*上一次未读、写完的buff长度*/
    struct semaphore sema;      /*信号量，用于防止重入时信号量乱序*/
};

/* acm ldisc私有数据*/
struct acm_ldisc_priv{
    struct tty_struct     *tty;          /* ldisc对应tty节点 */
    char                   tty_name[64];
    spinlock_t             write_lock;    /* ldisc写自旋锁 */
    spinlock_t             recv_lock;     /* ldisc读互斥信号量 */
    struct semaphore       recv_sema;     /* ldisc读task_let */
    struct tasklet_struct  write_tsk;     /* ldisc写task_let*/
    struct list_head       recv_list;
    struct list_head       write_list;
    struct acm_mem_info    *last_read_left_mem;  /*记录未读完的数据*/
    struct acm_mem_info    *last_write_left_mem; /*记录未写完的数据*/
    unsigned int           write_size;           /*记录写数据的总长度*/
    unsigned int           write_success_size;   /*记录写成功的数据长度*/
    unsigned int           read_size;            /*读数据的总长度*/
    unsigned int           tty_recv_size;
    unsigned int           ps_recv_size;
    unsigned int           read_cb;
    unsigned int           get_read_buf;
    unsigned int           return_read_buf;
    unsigned int           get_read_buf_no_data;
    unsigned int           get_read_buf_invalid;
    unsigned int           acm_ldisc_read;
};


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
void acm_free_list(struct list_head *plist);
int acm_ldisc_open(struct tty_struct *tty);
void acm_ldisc_close(struct tty_struct *tty);
ssize_t acm_ldisc_read(struct tty_struct * tty, struct file * file,
                        unsigned char __user * buf, size_t nr);

ssize_t acm_ldisc_write(struct tty_struct *tty, struct file *file,
                       const unsigned char *buf, size_t nr);

void acm_write_task(unsigned long ldisc_priv);
void acm_ldisc_receive_buf(struct tty_struct *tty, const unsigned char *buf,
                                char *cflags, int count);

void acm_ldisc_write_wakeup(struct tty_struct *tty);
int  acm_ldisc_ioctl(struct tty_struct *tty, struct file *file,
                        unsigned int cmd, unsigned long arg);
long  acm_ldisc_compact_ioctl(struct tty_struct *tty, struct file *file,
                        unsigned int cmd, unsigned long arg);
int  acm_ldisc_get_rd_buf(struct tty_struct * tty, ACM_WR_ASYNC_INFO *mem_info);
int  acm_ldisc_return_buf(struct tty_struct * tty, ACM_WR_ASYNC_INFO *mem_info);
int acm_ldisc_hangup(struct tty_struct *tty);
int __init acm_ldisc_init(void);
void __exit acm_ldisc_exit(void);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of acm_ldisc.h */
