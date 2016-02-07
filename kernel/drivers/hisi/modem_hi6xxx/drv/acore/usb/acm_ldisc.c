

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/types.h>
#include <linux/module.h>
#include <linux/tty.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/list.h>

#include "BSP.h"
#include "acm_ldisc.h"
#include <bsp_udi_adp.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* acm ldisc ops定义结构体 */
static struct tty_ldisc_ops acm_ldisc = {
    .owner        = THIS_MODULE,
    .magic        = TTY_LDISC_MAGIC,
    .name         = "acm",
    .open         = acm_ldisc_open,
    .close        = acm_ldisc_close,
    .read         = acm_ldisc_read,
    .write        = acm_ldisc_write,
    .receive_buf  = acm_ldisc_receive_buf,
    .write_wakeup = acm_ldisc_write_wakeup,
    .ioctl        = acm_ldisc_ioctl,
    .compat_ioctl = acm_ldisc_compact_ioctl,
    .hangup       = acm_ldisc_hangup,
};

extern struct acm_ctx g_acm_priv[];
struct acm_ldisc_priv g_acm_ldisc[USB_ACM_COM_UDI_NUM];

/*****************************************************************************
  3 函数实现
*****************************************************************************/


void acm_free_list(struct list_head *plist)
{
    struct acm_mem_info   *acm_mem_node;

    while(!list_empty(plist)) {
        acm_mem_node = list_entry(plist->next,
                                  struct acm_mem_info,
                                  list);
        list_del(&acm_mem_node->list);

        if (NULL != acm_mem_node->mem) {
            kfree(acm_mem_node->mem);
        }

        kfree(acm_mem_node);
        acm_mem_node = NULL;
    }
}

/*  set ldisc for arm64
    the func just from tty_io.c
    tty_compact_ioctl do not support TIOCSETD so we need do it ourself
*/
#ifndef _DRV_LLT_UT_
static int acm_tiocsetd(struct tty_struct *tty, int __user *p)
{
	int ldisc;
	int ret;

	if (get_user(ldisc, p))
		return -EFAULT;

	ret = tty_set_ldisc(tty, ldisc);

	return ret;
}
#endif



int acm_ldisc_open(struct tty_struct *tty)
{
    struct acm_ldisc_priv *acm_ldisc_ctx = NULL;
    int i = 0;

    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
        if (!g_acm_ldisc[i].tty || (0 == strncmp(tty->name,g_acm_ldisc[i].tty_name,strlen(g_acm_ldisc[i].tty_name)))) {
            acm_ldisc_ctx = &g_acm_ldisc[i];
            strncpy(g_acm_ldisc[i].tty_name, tty->name, sizeof(tty->name));
            break;
        }
    }

    if (NULL == acm_ldisc_ctx) {
        ACM_LD_DBG("device is not opened\n");
        return ERROR;
    }

    /* 将tty节点填入acm ldisc私有数据中 */
    acm_ldisc_ctx->tty = tty;

    ACM_LD_DBG("[%s] enter\n", tty->name);

    spin_lock_init(&acm_ldisc_ctx->recv_lock);
    spin_lock_init(&acm_ldisc_ctx->write_lock);
    sema_init(&acm_ldisc_ctx->recv_sema, 0);

    tasklet_init(&acm_ldisc_ctx->write_tsk, acm_write_task, (unsigned long)acm_ldisc_ctx);

    /* 初始化读、写链表 */
    INIT_LIST_HEAD(&acm_ldisc_ctx->recv_list);
    INIT_LIST_HEAD(&acm_ldisc_ctx->write_list);

    acm_ldisc_ctx->read_size          = 0;
    acm_ldisc_ctx->tty_recv_size      = 0;
    acm_ldisc_ctx->write_size         = 0;
    acm_ldisc_ctx->write_success_size = 0;
    acm_ldisc_ctx->read_cb = 0;
    acm_ldisc_ctx->ps_recv_size = 0;
    acm_ldisc_ctx->get_read_buf = 0;
    acm_ldisc_ctx->return_read_buf = 0;
    acm_ldisc_ctx->get_read_buf_no_data = 0;
    acm_ldisc_ctx->get_read_buf_invalid = 0;
    acm_ldisc_ctx->acm_ldisc_read = 0;
    acm_ldisc_ctx->last_read_left_mem = NULL;
    acm_ldisc_ctx->last_write_left_mem = NULL;

    /* 初始化receive_room大小 */
    tty->receive_room = ACM_LD_ROOM;

    /* 将acm ldisc私有数据填入tty->disc_data中 */
    tty->disc_data = (void *)acm_ldisc_ctx;

    return OK;
}


void acm_ldisc_close(struct tty_struct *tty)
{
    struct acm_ldisc_priv *acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;
    struct list_head *recv_list;
    struct list_head *write_list;

    if (NULL == acm_ldisc_ctx) {
        ACM_LD_DBG("device ldisc data is null\n");
        return;
    }

    ACM_LD_DBG("[%s] enter\n", tty->name);

    /* 将tty节点记录的ldisc私有数据置空 */
    tty->disc_data = NULL;

    recv_list  = &(acm_ldisc_ctx->recv_list);
    write_list = &(acm_ldisc_ctx->write_list);

    /* 释放读写链表内存 */
    spin_lock_bh(&acm_ldisc_ctx->recv_lock);
    acm_free_list(recv_list);
    spin_unlock_bh(&acm_ldisc_ctx->recv_lock);

    spin_lock_bh(&acm_ldisc_ctx->write_lock);
    acm_free_list(write_list);
    spin_unlock_bh(&acm_ldisc_ctx->write_lock);

    if (acm_ldisc_ctx->acm_ldisc_read){
        up(&acm_ldisc_ctx->recv_sema);
    }

    /* kill写task_let */
    tasklet_kill(&acm_ldisc_ctx->write_tsk);

    /*kfree(acm_ldisc_ctx);*/

}


ssize_t acm_ldisc_read(struct tty_struct * tty, struct file * file,
                        unsigned char __user * buf, size_t nr)
{
    ssize_t ret       = 0;
    int read_size = 0;
    struct acm_ldisc_priv *acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;
    struct list_head *recv_list;
    struct acm_mem_info *acm_mem_node = NULL;
    unsigned char *copy_start_addr;

    if (NULL == acm_ldisc_ctx) {
        return USB_NODATA;
    }

    acm_ldisc_ctx->acm_ldisc_read++;
    recv_list = &acm_ldisc_ctx->recv_list;

    if (NULL == acm_ldisc_ctx->last_read_left_mem) {
        if (list_empty(recv_list)){
            /* 等待receive_buf的信号量 */
            down(&acm_ldisc_ctx->recv_sema);
        }

        if (NULL == acm_ldisc_ctx) {
            return USB_NODATA;
        }

        spin_lock_bh(&acm_ldisc_ctx->recv_lock);
        if (list_empty(recv_list)) {
            spin_unlock_bh(&acm_ldisc_ctx->recv_lock);
            return USB_NODATA;
        }

        acm_mem_node = list_entry(recv_list->next, struct acm_mem_info, list);
        list_del_init(recv_list->next);
        spin_unlock_bh(&acm_ldisc_ctx->recv_lock);

        if (NULL == acm_mem_node) {
            ACM_LD_DBG("read buff is invalid\n");
            return ERROR;
        }

    } else {
        acm_mem_node = acm_ldisc_ctx->last_read_left_mem;
    }

    /* 此次需要读取数据的首地址 */
    copy_start_addr = acm_mem_node->current_pos;

    if (acm_mem_node->left_size > nr){
        read_size = nr;
        acm_mem_node->left_size -= nr;
        acm_mem_node->current_pos +=nr;
        acm_ldisc_ctx->last_read_left_mem = acm_mem_node;
    } else {
        read_size = acm_mem_node->left_size;
        acm_ldisc_ctx->last_read_left_mem = NULL;
    }

    if (read_size > 0){
        /* copy数据到buf中 */
        if(copy_to_user((void *)buf, (void *)copy_start_addr, read_size))
        {
            return ERROR;
        }
        acm_ldisc_ctx->read_size += read_size;
        ret = read_size;
    }

    if (NULL == acm_ldisc_ctx->last_read_left_mem) {
        kfree(acm_mem_node->mem);
        acm_mem_node->mem = NULL;
        kfree(acm_mem_node);
        acm_mem_node      = NULL;
    }

    return ret;
}
ssize_t acm_ldisc_write(struct tty_struct *tty, struct file *file,
                       const unsigned char *buf, size_t nr)
{
    int ret  = 0;
    struct acm_ldisc_priv *acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;
    struct acm_mem_info *acm_mem_node;

    acm_ldisc_ctx->write_size += nr;

    /* 检查NONBLOCK标志位,异步写分支 */
    if (file->f_flags & O_NONBLOCK){
        ret = tty->ops->write(tty,buf,nr);
        acm_ldisc_ctx->write_success_size += ret;

        return (ssize_t)ret;
    }

    acm_mem_node = (struct acm_mem_info *)kmalloc(sizeof(struct acm_mem_info),GFP_KERNEL);

    /* 将buf封装成acm_mem_info结构然后加到写链表中 */
    acm_mem_node->mem         = (void *)buf;
    acm_mem_node->valid_size  = nr;
    acm_mem_node->left_size   = nr;
    acm_mem_node->current_pos = (unsigned char *)buf;
    sema_init(&acm_mem_node->sema,0);

    /* 写互斥锁 */
    spin_lock_bh(&acm_ldisc_ctx->write_lock);

    list_add_tail(&acm_mem_node->list, &acm_ldisc_ctx->write_list);

    /* 写互斥解锁 */
    spin_unlock_bh(&acm_ldisc_ctx->write_lock);

    /* 调度写task_let */
    tasklet_schedule(&acm_ldisc_ctx->write_tsk);

    /* 等待写完成信号量 */
    down(&acm_mem_node->sema);

    /*if (NULL != acm_mem_node->mem) {
        kfree(acm_mem_node->mem);
    }*/

    kfree(acm_mem_node);
    acm_mem_node = NULL;

    return (ssize_t)nr;
}


void acm_write_task(unsigned long ldisc_priv)
{
    int len       = 0;
    struct acm_ldisc_priv *acm_ldisc_ctx = (struct acm_ldisc_priv *)ldisc_priv;
    struct tty_struct *tty       = acm_ldisc_ctx->tty;
    struct list_head *write_list = &acm_ldisc_ctx->write_list;
    struct acm_mem_info *acm_mem_node;

    /* 写互斥锁 */
    spin_lock(&acm_ldisc_ctx->write_lock);

    while ((NULL != acm_ldisc_ctx->last_write_left_mem)||
            (!list_empty(write_list))) {
        /* 检查上次是否有未写完的数据 */
        if (NULL == acm_ldisc_ctx->last_write_left_mem) {
            /* 从写链表中取数据并且赋给用以操作的mem_info局部变量 */
            acm_mem_node = list_entry(write_list->next, struct acm_mem_info, list);
            list_del_init(write_list->next);

            /* 检查数据是否为空 */
            if (NULL == acm_mem_node) {
                /*ACM_LD_DBG("write buff is empty\n");*/
                spin_unlock(&acm_ldisc_ctx->write_lock);
                return;
            }

        } else {
            /* 未写完数据last_write_left_mem赋值给mem_info局部变量 */
            acm_mem_node= acm_ldisc_ctx->last_write_left_mem;
            clear_bit(TTY_DO_WRITE_WAKEUP, &tty->flags);
        }

        len = tty->ops->write(tty, acm_mem_node->current_pos, acm_mem_node->left_size);

        acm_ldisc_ctx->write_success_size += len;

        if (len < acm_mem_node->left_size) {
            /* 计算mem_info->left_size,得出剩余多少字节未写完  */
            acm_mem_node->left_size -= len;

            /* 计算mem_info->current_pos,得出当前写指针的位置  */
            acm_mem_node->current_pos += len;

            /* 刷新l_disc的last_write_left_mem，即未写完数据指针 */
            acm_ldisc_ctx->last_write_left_mem = acm_mem_node;

            /* 设置TTY_DO_WRITE_WAKEUP标志，让底层有空间时通知ldisc继续下发数据 */
            set_bit(TTY_DO_WRITE_WAKEUP, &tty->flags);
            break;
        } else {
            /* 清空l_disc的last_write_left_mem */
            acm_ldisc_ctx->last_write_left_mem = NULL;

            /* 释放该内存节点写信号量 */
            up(&acm_mem_node->sema);
        }
    }

    spin_unlock(&acm_ldisc_ctx->write_lock);

    return;
}


void acm_ldisc_receive_buf(struct tty_struct *tty, const unsigned char *buf,
                                char *cflags, int count)
{
    struct acm_ldisc_priv *acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;
    struct semaphore *sem = &acm_ldisc_ctx->recv_sema;
    struct acm_mem_info *acm_mem_node;
    unsigned char *buffer = NULL;
    unsigned long flags;
    int i;

    buffer = (unsigned char *)kmalloc(count, GFP_KERNEL);
    if (!buffer) {
        ACM_LD_DBG("kmalloc fail!\n");
        return;
    }

    memcpy((void *)buffer, (void *)buf, count);

    acm_mem_node = (struct acm_mem_info *)kmalloc(sizeof(struct acm_mem_info),GFP_ATOMIC);

    /* 将buf封装成acm_mem_info结构然后加到读链表中 */
    acm_mem_node->mem         = buffer;
    acm_mem_node->valid_size  = count;
    acm_mem_node->left_size   = count;
    acm_mem_node->current_pos = buffer;

    spin_lock(&acm_ldisc_ctx->recv_lock);
    list_add_tail(&acm_mem_node->list, &acm_ldisc_ctx->recv_list);
    acm_ldisc_ctx->tty_recv_size += count;
    spin_unlock(&acm_ldisc_ctx->recv_lock);

    /*如果是C SHELL，调用读回调将buff传回*/
    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
        if (0 == strncmp(tty->name, g_acm_priv[i].tty_name, strlen(g_acm_priv[i].tty_name))) {
            if (g_acm_priv[i].readDoneCB) {
                g_acm_priv[i].readDoneCB();
                acm_ldisc_ctx->read_cb++;
                break;
            }
        }
    }

    if (acm_ldisc_ctx->acm_ldisc_read){
	raw_spin_lock_irqsave((raw_spinlock_t *)(&sem->lock), flags);
	if (0 == sem->count) {
	    raw_spin_unlock_irqrestore((raw_spinlock_t *)(&sem->lock), flags);
	    up(sem);
	} else {
	    raw_spin_unlock_irqrestore((raw_spinlock_t *)(&sem->lock), flags);
	}
    }

    return;
}


void acm_ldisc_write_wakeup(struct tty_struct *tty)
{
    struct acm_ldisc_priv *acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;

    /* 清除TTY_DO_WRITE_WAKEUP标志位 */
    clear_bit(TTY_DO_WRITE_WAKEUP, &tty->flags);

    /* 检查ldisc私有数据是否为空 */
    if (NULL == acm_ldisc_ctx) {
       ACM_LD_DBG("acm_ldisc_priv is NULL!\n");
       return;
    }

    /* 调度写task_let */
    tasklet_schedule(&acm_ldisc_ctx->write_tsk);

    return;
}
int acm_ldisc_ioctl(struct tty_struct *tty, struct file *file, unsigned int cmd, unsigned long arg)
{
    ACM_WR_ASYNC_INFO *mem_info = NULL;
    unsigned long *buf = NULL;
    void __user *p = (void __user *)arg;
    int ret = OK;

    /* ACM_LD_DBG("enter\n"); */

    if (0 == arg){
        ACM_LD_DBG("param invalid, cmd is [0x%x]\n",cmd);
        return ERROR;
    }

    switch (cmd) {
        case LDISC_IOCTL_GET_RD_BUF:
            mem_info = (ACM_WR_ASYNC_INFO *)arg;
            ret = acm_ldisc_get_rd_buf(tty, mem_info);
            if (ERROR == ret){
                mem_info->pVirAddr = NULL;
                mem_info->u32Size = 0;
                mem_info->pDrvPriv = NULL;
                ACM_LD_DBG("get read buf fail\n");
                return ERROR;
            }
            break;

        case LDISC_IOCTL_RETURN_BUF:
            mem_info = (ACM_WR_ASYNC_INFO *)arg;
            ret = acm_ldisc_return_buf(tty, mem_info);
            if (ERROR == ret){
                ACM_LD_DBG("return buf fail\n");
                return ERROR;
            }
            break;

        case LDISC_IOCTL_GET_TTY:
            ACM_LD_DBG("tty is [0x%p]\n",tty);
            buf = (unsigned long *)arg;
            *buf = (unsigned long)tty;
            ret = OK;
            break;

#ifndef _DRV_LLT_UT_
        case TIOCSETD:
            ACM_LD_DBG("CMD TIOCSETD\n");
            return acm_tiocsetd(tty, p);
#endif

        default:
            ACM_LD_DBG("%s cmd[0x%x] unsupport\n", tty->name, cmd);
            break;
    }

    return ret;
}

long acm_ldisc_compact_ioctl(struct tty_struct *tty, struct file *file, unsigned int cmd, unsigned long arg)
{
    return acm_ldisc_ioctl(tty, file, cmd, arg);
}
int acm_ldisc_get_rd_buf(struct tty_struct * tty, ACM_WR_ASYNC_INFO *mem_info)
{
    struct acm_ldisc_priv *acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;
    struct acm_mem_info *acm_mem_node = NULL;
    struct list_head    *recv_list = NULL;


    /* ACM_LD_DBG("enter\n"); */
    acm_ldisc_ctx->get_read_buf++;
    recv_list = &acm_ldisc_ctx->recv_list;

    spin_lock_bh(&acm_ldisc_ctx->recv_lock);

    /*从recv_list取新节点*/
    if (list_empty(recv_list)){
        acm_ldisc_ctx->get_read_buf_no_data++;
        spin_unlock_bh(&acm_ldisc_ctx->recv_lock);
        ACM_LD_DBG("no data in tty\n");
        return ERROR;
    }

    acm_mem_node = list_entry(recv_list->next, struct acm_mem_info, list);
    list_del_init(recv_list->next);

    if (NULL == acm_mem_node) {
        acm_ldisc_ctx->get_read_buf_invalid++;
        spin_unlock_bh(&acm_ldisc_ctx->recv_lock);
        ACM_LD_DBG("read buf is invalid\n");
        return ERROR;
    }

    mem_info->pVirAddr = (char *)acm_mem_node->mem;
    mem_info->u32Size = acm_mem_node->valid_size;
    mem_info->pDrvPriv = (void *)acm_mem_node;
    acm_ldisc_ctx->ps_recv_size += mem_info->u32Size;
    spin_unlock_bh(&acm_ldisc_ctx->recv_lock);
    return OK;
}


int acm_ldisc_return_buf(struct tty_struct * tty, ACM_WR_ASYNC_INFO *mem_info)
{
    struct acm_ldisc_priv *acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;
    struct acm_mem_info *acm_mem_node = NULL;

    /* ACM_LD_DBG("enter\n"); */
    acm_ldisc_ctx->return_read_buf++;
    acm_mem_node = (struct acm_mem_info *)mem_info->pDrvPriv;
    /* mem_node全部发送完毕，则释放该内存 */
    kfree(acm_mem_node->mem);
    acm_mem_node->mem = NULL;
    kfree(acm_mem_node);
    acm_mem_node      = NULL;
    return OK;

}


int acm_ldisc_hangup(struct tty_struct *tty)
{
    struct acm_ldisc_priv *acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;
    struct semaphore *sem = &acm_ldisc_ctx->recv_sema;
    unsigned long flags;

    /* ACM_LD_DBG("tty[%p]\n", tty); */
    if (acm_ldisc_ctx->acm_ldisc_read){
        raw_spin_lock_irqsave((raw_spinlock_t *)(&sem->lock), flags);
        if (0 == sem->count) {
            raw_spin_unlock_irqrestore((raw_spinlock_t *)(&sem->lock), flags);
            up(sem);
        } else {
            raw_spin_unlock_irqrestore((raw_spinlock_t *)(&sem->lock), flags);
        }
    }
    return OK;
}
int __init acm_ldisc_init(void)
{
    int ret = 0;
    int i = 0;

    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
        g_acm_ldisc[i].tty = NULL;
        g_acm_ldisc[i].last_read_left_mem = NULL;
        g_acm_ldisc[i].last_write_left_mem = NULL;
    }

    ret = tty_register_ldisc(N_USB_COM, &acm_ldisc);
    if (ret != OK) {
       ACM_LD_DBG("acm_ldisc register failed!\n");
       ret = ERROR;
    }

    return ret;
}


void __exit acm_ldisc_exit(void)
{
    int ret = 0;

    /* 调用tty_unregister_ldisc去注册acm_ldisc，并且得到返回值ret */
    ret = tty_unregister_ldisc(N_USB_COM);
    if (ret != OK) {
       ACM_LD_DBG("acm_ldisc unregister failed! unregister's ret is %d\n",ret);
    }
}

module_init(acm_ldisc_init);
module_exit(acm_ldisc_exit);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

