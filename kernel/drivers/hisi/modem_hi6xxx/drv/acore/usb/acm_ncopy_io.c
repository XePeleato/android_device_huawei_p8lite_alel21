/*******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 *******************************************************************************
  文 件 名   : acm_ncopy_io.c
  版 本 号   : 初稿
  作    者   : 周明发 00145324
  生成日期   : 2013年4月15日
  最近修改   :
  功能描述   : 用于ACM零拷贝IO接口
  函数列表   :
  修改历史   :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 创建文件

*******************************************************************************/

/*******************************************************************************
  1 头文件包含
*******************************************************************************/
#include <linux/serial_core.h>
#include <linux/syscalls.h>
#include <linux/dma-mapping.h>
#include <u_serial.h>
#include <asm/cacheflush.h>
#include "acm_ncopy_io.h"
#include <bsp_udi_adp.h>

#define ACM_DUMP_DRV1_PRIVATE
#ifdef ACM_DUMP_DRV1_PRIVATE
#include <linux/tty.h>
#include <linux/semaphore.h>
#include  "acm_ldisc.h"
#endif /* ACM_DUMP_DRV1_PRIVATE */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*******************************************************************************
  2 全局变量定义
*******************************************************************************/
struct acm_ncopy_ctx g_acm_ncpy[USB_ACM_COM_UDI_NUM] = {{0}};
int g_acm_ncpy_dbg_lev = 3;  /* KERN_ERR */

/*******************************************************************************
  3 函数声明
*******************************************************************************/
#define ACM_NCPY_ERR(args,...) do {\
    if (3 <= g_acm_ncpy_dbg_lev) {\
        printk("%s:%d:", __func__, __LINE__);\
        printk(args, ##__VA_ARGS__);\
    }\
}while(0)

#define ACM_NCPY_DBG(args,...) do {\
    if (7 <= g_acm_ncpy_dbg_lev) {\
        printk("%s:%d:", __func__, __LINE__);\
        printk(args, ##__VA_ARGS__);\
    }\
}while(0)

#define ACM_NCPY_STATS_INCR(ctx, stats_info) do {\
    spin_lock_irqsave(&ctx->tx_lock,flags);\
    (ctx->stats_info)++;\
    spin_unlock_irqrestore(&ctx->tx_lock,flags);\
}while(0)

STATIC struct usb_request * acm_ncopy_alloc_one_request(gfp_t gfp_flags);
STATIC void acm_ncopy_free_one_request(struct usb_request *req);
STATIC int acm_ncopy_alloc_requests(struct list_head *head,
            void (*fn)(struct usb_ep *, struct usb_request *), int *allocated);
STATIC void acm_ncopy_free_requests(struct list_head *head, int *allocated);
STATIC int  acm_ncopy_queue(struct acm_ncopy_ctx * acm_ncopy, char *pVirAddr,
                            char *pPhyAddr, unsigned int size);
STATIC void acm_ncopy_tx_task(unsigned long arg);
STATIC void acm_ncopy_tx_complete(struct usb_ep *ep, struct usb_request *req);
/*******************************************************************************
  4 函数实现
*******************************************************************************/

/*****************************************************************************
 函 数 名  : acm_ncopy_init
 功能描述  : acm零拷贝初始化接口
 输入参数  : acm_dev : 对应acm对象设备
 输出参数  : 无
 返 回 值  : NCPY_OK 初始化成功，NCPY_ERROR 初始化失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
int acm_ncopy_init(void *acm_dev)
{
    struct acm_ctx *acm = (struct acm_ctx *)acm_dev;
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    long rc = 0;
    int i = 0;

    if (!acm) {
        ACM_NCPY_ERR("device is not invalid\n");
        return NCPY_ERROR;
    }

    ACM_NCPY_DBG("acm[%s] tty[%p] enter\n", acm->tty_name, acm->tty);

    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
        if (!g_acm_ncpy[i].acm || (acm == g_acm_ncpy[i].acm)) {
            acm_ncpy = &g_acm_ncpy[i];
            acm->acm_ncpy = (void *)acm_ncpy;
            break;
        }
    }

    if (!acm_ncpy) {
        ACM_NCPY_ERR("acm_ncpy[%s] not found\n", acm->tty_name);
        return NCPY_ERROR;
    }

    acm_ncpy->acm  = (void *)acm;
    acm_ncpy->tty  = (struct tty_struct *)acm->tty;
    acm_ncpy->port = acm_ncopy_port_ready(acm_ncpy->tty);
    if (!acm_ncpy->port) {
        ACM_NCPY_ERR("[%s] get port fail\n", acm->tty_name);
        return NCPY_ERROR;
    }

    INIT_LIST_HEAD(&acm_ncpy->tx_list);
    INIT_LIST_HEAD(&acm_ncpy->free_list);
    spin_lock_init(&acm_ncpy->tx_lock);

    rc = acm_ncopy_alloc_requests(&acm_ncpy->free_list,
                acm_ncopy_tx_complete, &acm_ncpy->allocated);
    if (rc) {
        ACM_NCPY_ERR("[%s] alloc requests fail\n", acm->tty_name);
        return NCPY_ERROR;
    }

    tasklet_init(&acm_ncpy->tx_task, acm_ncopy_tx_task, (unsigned long)acm_ncpy);
    acm_ncpy->available = 1;

    return NCPY_OK;
}

/*****************************************************************************
 函 数 名  : acm_ncopy_uninit
 功能描述  : acm零拷贝去初始化接口
 输入参数  : acm_dev : 对应acm对象设备
 输出参数  : 无
 返 回 值  : NCPY_OK 去初始化成功，NCPY_ERROR 去初始化失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
int acm_ncopy_uninit(void *acm_dev)
{
    struct acm_ctx *acm = (struct acm_ctx *)acm_dev;
    struct acm_ncopy_ctx *acm_ncpy = NULL;

    unsigned long flags = 0;
    struct list_head *tx_pool = NULL;
    struct usb_request *req = NULL;

    if (!acm) {
        ACM_NCPY_ERR("device is not invalid\n");
        return NCPY_ERROR;
    }

    ACM_NCPY_DBG("acm[%s] enter\n", acm->tty_name);

    acm_ncpy = (struct acm_ncopy_ctx *)acm->acm_ncpy;
    if (!acm_ncpy) {
        ACM_NCPY_ERR("acm_ncpy[%s] not found\n", acm->tty_name);
        return NCPY_ERROR;
    }

    tasklet_kill(&acm_ncpy->tx_task);

    spin_lock_irqsave(&acm_ncpy->tx_lock, flags);
    acm_ncpy->available = 0;

    tx_pool = &acm_ncpy->tx_list;
    while (!list_empty(tx_pool)) {
        req = list_entry(tx_pool->next, struct usb_request, list);
        list_del(&req->list);
        spin_unlock(&acm_ncpy->tx_lock);
        acm->writeDoneCB((char *)req->buf, (char *)req->dma,NCPY_ERROR);
        spin_lock(&acm_ncpy->tx_lock);
        acm_ncpy->tx_fail++;
        acm_ncpy->tx_cb++;
        list_add_tail(&req->list, &acm_ncpy->free_list);
        req->buf = NULL;
    }

    acm_ncopy_free_requests(&acm_ncpy->free_list, &acm_ncpy->allocated);
    spin_unlock_irqrestore(&acm_ncpy->tx_lock, flags);

    return NCPY_OK;
}

/*****************************************************************************
 函 数 名  : acm_ncopy_alloc_one_request
 功能描述  : 分配单个 USB请求
 输入参数  : gfp_t gfp_flags
 输出参数  : 无
 返 回 值  : struct usb_request * 分配成功，0  分配失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月14日
    作    者   : w00140341
    修改内容   : 新生成函数

*****************************************************************************/
STATIC struct usb_request * acm_ncopy_alloc_one_request(gfp_t gfp_flags)
{
    struct usb_request *usb_req;

    ACM_NCPY_DBG("acm_ncopy_alloc_one_request(%d)\n", gfp_flags);

    usb_req = kzalloc(sizeof(*usb_req), gfp_flags);
    if (0 == usb_req) {
        ACM_NCPY_ERR("acm_ncopy_alloc_one_request allocation failed\n");
        return 0;
    }

    usb_req->dma = (~(unsigned int)0);

    return usb_req;
}


/*****************************************************************************
 函 数 名  : acm_ncopy_alloc_one_request
 功能描述  : 分配单个 USB请求
 输入参数  : struct usb_request *req
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月14日
    作    者   : w00140341
    修改内容   : 新生成函数

*****************************************************************************/
STATIC void acm_ncopy_free_one_request(struct usb_request *req)
{
    ACM_NCPY_DBG("acm_ncopy_free_one_request(%p)\n",  req);

    if (0 == req) {
        ACM_NCPY_ERR("acm_ncopy_free_one_request Invalid req\n");
        return;
    }

    kfree(req);
}



/*****************************************************************************
 函 数 名  : acm_ncopy_alloc_requests
 功能描述  : 分配USB请求用于零拷贝tx操作
 输入参数  : head     : 存储USB请求的链表
             fn       : USB请求完成回调接口
             allocated: 待分USB请求个数
 输出参数  : 无
 返 回 值  : NCPY_OK 分配成功，-ENOMEM 分配失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
STATIC int acm_ncopy_alloc_requests(struct list_head *head,
        void (*fn)(struct usb_ep *, struct usb_request *), int *allocated)
{
    struct usb_request *req;
    int i = 0;
    int n = 0;

    n = allocated ? (ACM_NCOPY_TX_ALLOCATED - *allocated) : ACM_NCOPY_TX_ALLOCATED;
    ACM_NCPY_DBG("alloc [%2d] requests\n", n);
    /* Pre-allocate up to ACM_NCOPY_TX_ALLOCATED transfers, but if we can't
     * do quite that many this time, don't fail ... we just won't
     * be as speedy as we might otherwise be.
     */
    for (i = 0; i < n; i++) {
        req = acm_ncopy_alloc_one_request(GFP_ATOMIC);
        if (!req) {
            ACM_NCPY_ERR("alloc req[%d] fail\n", i);
            return list_empty(head) ? -ENOMEM : 0;
        }

        ACM_NCPY_DBG("alloc [%2d] requests[%p]\n", i, req);

        req->complete = fn;
        req->context  = NULL;
        req->buf = NULL;
        req->length = 0;
        req->zero = 0;

        list_add_tail(&req->list, head);
        if (allocated) {
            (*allocated)++;
        }
    }

    return NCPY_OK;
}

/*****************************************************************************
 函 数 名  : acm_ncopy_free_requests
 功能描述  : 释放USB请求及其对应内存
 输入参数  : head     : 存储USB请求的链表
             allocated: 待分USB请求个数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
STATIC void acm_ncopy_free_requests(struct list_head *head,
            int *allocated)
{
    struct usb_request	*req;

    while (!list_empty(head)) {
        req = list_entry(head->next, struct usb_request, list);

        list_del(&req->list);

        acm_ncopy_free_one_request(req);

        if (allocated) {
            (*allocated)--;
        }
    }
}

/*****************************************************************************
 函 数 名  : acm_ncopy_start_tx
 功能描述  : acm零拷贝tx接口
 输入参数  : acm_dev : acm对象设备
             buf     : 待发送数据buffer
             size    : 待发送数据字节数
 输出参数  : 无
 返 回 值  : NCPY_OK tx操作成功，NCPY_ERROR tx操作失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
int acm_ncopy_start_tx(void *acm_dev, char *pVirAddr, char *pPhyAddr, unsigned int size)
{
    struct acm_ctx *acm = (struct acm_ctx *)acm_dev;
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    int rc = NCPY_OK;
    unsigned long flags = 0;

    if (!acm) {
        ACM_NCPY_ERR("device is invalid\n");
        return NCPY_ERROR;
    }

    if ((NULL == pVirAddr) || (NULL == pPhyAddr) || (0 == size)) {
        ACM_NCPY_ERR("[%s] buf is invalid, size[%d]\n", acm->tty_name, size);
        return NCPY_ERROR;
    }

    if (size > ACM_NCOPY_TX_MAX_SIZE) {
        ACM_NCPY_ERR("[%s] size[%d] exceed the limit\n", acm->tty_name, size);
        return NCPY_ERROR;
    }

    ACM_NCPY_DBG("ACM[%s] send pVirAddr[%p] pPhyAddr[%p] size[%d]\n", acm->tty_name, pVirAddr, pPhyAddr, size);

    acm_ncpy = (struct acm_ncopy_ctx *)acm->acm_ncpy;
    if (NULL == acm_ncpy)  {
        ACM_NCPY_ERR("[%s] acm_ncpy is invalid\n", acm->tty_name);
        return NCPY_ERROR;
    }

    ACM_NCPY_STATS_INCR(acm_ncpy, tx_got);

    if (likely(acm_ncpy->available)) {
        /* Insert tx buffer into the list */
        rc = acm_ncopy_queue(acm_ncpy, pVirAddr, pPhyAddr, size);

        /* Schedule the task */
        tasklet_schedule(&acm_ncpy->tx_task);
        return rc;
    }

    ACM_NCPY_STATS_INCR(acm_ncpy, tx_discard);

    ACM_NCPY_ERR("ACM[%s] discard for invalid pVirAddr[%p] pPhyAddr[%p] size[%d]\n",
            acm->tty_name, pVirAddr, pPhyAddr, size);

    return NCPY_ERROR;
}

/*****************************************************************************
 函 数 名  : acm_ncopy_queue
 功能描述  : acm零拷贝tx请求插入链表，
 输入参数  : acm_ncopy_ctx : acm设备
             buf   : 待发送数据buffer
             size  : 待发送数据字节数
 输出参数  : 无
 返 回 值  : NCPY_OK 入队成功，NCPY_ERROR 入队失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
STATIC int acm_ncopy_queue(struct acm_ncopy_ctx * acm_ncopy, char *pVirAddr,
                           char *pPhyAddr, unsigned int size)
{
    struct acm_ncopy_ctx *acm_ncpy = acm_ncopy;
    struct list_head *tx_pool = &acm_ncpy->tx_list;
    struct list_head *free_pool  = &acm_ncpy->free_list;
    struct usb_request *req = NULL;
    unsigned long flags = 0;

    spin_lock_irqsave(&acm_ncpy->tx_lock, flags);
    if (!list_empty(free_pool)) {
        /* Get an item from free list */
        req = list_entry(free_pool->next, struct usb_request, list);

        req->buf = (void *)pVirAddr;
        req->dma = (dma_addr_t)(pPhyAddr);
        req->length = size;
        req->context = (void *)acm_ncpy;
        req->zero = size ? 0 : 1;
        req->no_unmap_flag = 1;

        /* Insert into tx list */
        list_del(&req->list);
        list_add_tail(&req->list, tx_pool);
        acm_ncpy->tx_queued++;
        spin_unlock_irqrestore(&acm_ncpy->tx_lock, flags);
        return NCPY_OK;
    }
    else {
        acm_ncpy->tx_discard++;
        spin_unlock_irqrestore(&acm_ncpy->tx_lock, flags);
        ACM_NCPY_ERR("no free item for buf[%p], size[%d]\n", pVirAddr, size);
        return NCPY_ERROR;
    }
}

/*****************************************************************************
 函 数 名  : acm_ncopy_tx_task
 功能描述  : acm零拷贝tx任务，调用USB底层接口执行发送动作
 输入参数  : arg : 任务入参，指向acm_ncopy设备
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
STATIC void acm_ncopy_tx_task(unsigned long arg)
{
    struct acm_ncopy_ctx *acm_ncpy = (struct acm_ncopy_ctx *)arg;
    struct acm_ctx *acm = (struct acm_ctx *)acm_ncpy->acm;
    struct list_head *tx_pool = &acm_ncpy->tx_list;
    struct list_head *free_pool  = &acm_ncpy->free_list;
    struct usb_request *req = NULL;
    struct gserial *port = NULL;
    struct usb_ep *ep_in = NULL;
    unsigned long flags = 0;
    int rc = 0;

    spin_lock_irqsave(&acm_ncpy->tx_lock, flags);
    port = (struct gserial *)acm_ncopy_port_ready(acm_ncpy->tty);
    if (!port) {
        spin_unlock_irqrestore(&acm_ncpy->tx_lock, flags);
        ACM_NCPY_ERR("%s port not ready\n", acm->tty_name);
        return;
    }
    ep_in = port->in;

    while (!list_empty(tx_pool))
    {
        req = list_entry(tx_pool->next, struct usb_request, list);

        list_del(&req->list);
        acm_ncpy->tx_sending++;
        spin_unlock(&acm_ncpy->tx_lock);
        ACM_NCPY_DBG("sending req[%p] ep[%s]\n", req, ep_in->name);
        rc = usb_ep_queue(ep_in, req, GFP_ATOMIC);
        if (rc) {
            ACM_NCPY_ERR("%s queue %s err %d\n", acm->tty_name,
                    ep_in->name, rc);
            if (acm->freeSendbuff) {
               acm->freeSendbuff((char *)req->buf);
            }
            else if (acm->writeDoneCB) {
                acm->writeDoneCB((char *)req->buf, (char *)req->dma, NCPY_ERROR);
                ACM_NCPY_STATS_INCR(acm_ncpy, tx_cb);
            }

            spin_lock(&acm_ncpy->tx_lock);
            acm_ncpy->tx_fail++;
            req->buf = NULL;
            list_add_tail(&req->list, free_pool);
            break;
        }
        spin_lock(&acm_ncpy->tx_lock);
    }
    spin_unlock_irqrestore(&acm_ncpy->tx_lock, flags);

    return;
}

/*****************************************************************************
 函 数 名  : acm_ncopy_port_ready
 功能描述  : 通过tty设备检查acm端口是否枚举
 输入参数  : tty: acm端口对应acm设备
 输出参数  : 无
 返 回 值  : tty对应的port信息
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月7日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
void *acm_ncopy_port_ready(void *tty)
{
    if (!tty) {
        return NULL;
    }
    return ((void *)usb_tty_port_get(tty));
}

/*****************************************************************************
 函 数 名  : acm_ncopy_tx_complete
 功能描述  : acm零拷贝tx完成回调
 输入参数  : ep  : acm设备对应端点
             req : USB请求
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
STATIC void acm_ncopy_tx_complete(struct usb_ep *ep, struct usb_request *req)
{
    struct acm_ncopy_ctx *acm_ncpy = (struct acm_ncopy_ctx *)req->context;
    struct acm_ctx *acm = (struct acm_ctx *)acm_ncpy->acm;
    unsigned long flags = 0;

    ACM_NCPY_DBG("req[%p] finish[%d]\n", req, req->status);

    if (!req->status) {
        /* normal completion */
        ACM_NCPY_STATS_INCR(acm_ncpy, tx_sent);
    } else {
        /* presumably a transient fault */
        ACM_NCPY_STATS_INCR(acm_ncpy, tx_fail);
        ACM_NCPY_ERR(" unexpected %s status 0x%x\n", ep->name, req->status);
    }

    if (acm->freeSendbuff) {
       acm->freeSendbuff((char *)req->buf);
    }
    else if (acm->writeDoneCB) {
        acm->writeDoneCB((char *)req->buf, (char *)req->dma, req->status ? -1 : (int)req->length);
        ACM_NCPY_STATS_INCR(acm_ncpy, tx_cb);
    }

    spin_lock(&acm_ncpy->tx_lock);
    list_add_tail(&req->list, &acm_ncpy->free_list);
    spin_unlock(&acm_ncpy->tx_lock);
}

/*****************************************************************************
 函 数 名  : acm_ncopy_dbg_lev_set
 功能描述  : acm零拷贝模块打印级别设置接口
 输入参数  : lev : 打印级别
 输出参数  : 无
 返 回 值  : 打印级别
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
int acm_ncopy_dbg_lev_set(int lev)
{
    g_acm_ncpy_dbg_lev = lev;
    return g_acm_ncpy_dbg_lev;
}

/*****************************************************************************
 函 数 名  : acm_ncopy_dump
 功能描述  : acm零拷贝模块信息dump接口
 输入参数  : acm_idx : acm设备端口索引
 输出参数  : 无
 返 回 值  : NCPY_OK 正常dump，NCPY_ERROR dump失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月15日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
int acm_ncopy_dump(int acm_idx)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct acm_ctx *acm = NULL;

    if (USB_ACM_COM_UDI_NUM <= acm_idx) {
        printk("acm_ncopy_dump invalid index[%d]\n", acm_idx);
        return NCPY_ERROR;
    }

    acm_ncpy = &g_acm_ncpy[acm_idx];
    acm = (struct acm_ctx *)acm_ncpy->acm;

    if (!acm_ncpy->available) {
        printk("acm_ncpy[%d] invalid\n", acm_idx);
        return NCPY_ERROR;
    }

    printk("acm_ncpy[%d] info:\n", acm_idx);
    printk("acm         :%p\n", acm_ncpy->acm);
    printk("tty         :%p\n", acm_ncpy->tty);
    printk("port        :%p\n", acm_ncpy->port);
    printk("tx_lock     :%p\n", &acm_ncpy->tx_lock);
    printk("tx_list     :%p\n", &acm_ncpy->tx_list);
    printk("free_list   :%p\n", &acm_ncpy->free_list);
    printk("tx_task     :%p\n", &acm_ncpy->tx_task);
    printk("allocated   :%d\n", acm_ncpy->allocated);
    printk("available   :%d\n", acm_ncpy->available);
    printk("tx_got      :%d\n", acm_ncpy->tx_got);
    printk("tx_discard  :%d\n", acm_ncpy->tx_discard);
    printk("tx_queued   :%d\n", acm_ncpy->tx_queued);
    printk("tx_sending  :%d\n", acm_ncpy->tx_sending);
    printk("tx_sent     :%d\n", acm_ncpy->tx_sent);
    printk("tx_fail     :%d\n", acm_ncpy->tx_fail);
    printk("tx_cb       :%d\n", acm_ncpy->tx_cb);

    printk("acm_ctx info:\n");
    printk("open        :%d\n",    acm->bopen);
    printk("dev_id      :%d\n",    acm->dev_id);
    printk("fd          :0x%lx\n", acm->fd);
    printk("tty_name    :%s\n",    acm->tty_name);
    printk("readDoneCB  :%p\n",    acm->readDoneCB);
    printk("writeDoneCB :%p\n",    acm->writeDoneCB);
    printk("freeSendbuff:%p\n",    acm->freeSendbuff);
    printk("eventCB     :%p\n",    acm->eventCB);
    printk("acm_ncpy    :%p\n",    acm->acm_ncpy);
    printk("tty         :%p\n",    acm->tty);
    printk("do_copy     :%d\n",    acm->do_copy);
    printk("cur_evt     :%d\n",    acm->cur_evt);
    printk("ncopy       :%d\n",    acm->ncopy);

    return NCPY_OK;
}

#ifdef ACM_DUMP_DRV1_PRIVATE
#define acm_dump_print(fmt, arg...) printk( KERN_ERR fmt, ##arg)
/*****************************************************************************
 函 数 名  : acm_ncopy_get_ctx
 功能描述  : 通过index得到acm_ncopy_ctx数据
 输入参数  : acm_idx : acm设备端口索引
 输出参数  : 无
 返 回 值  : NULL 失败
*****************************************************************************/
static struct acm_ncopy_ctx *acm_ncopy_get_ctx(int acm_idx)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;

    acm_ncpy = &g_acm_ncpy[acm_idx];
    if (!acm_ncpy->available) {
        acm_dump_print("acm_ncpy[%d] invalid\n", acm_idx);
        acm_ncpy = NULL;
    }

    return acm_ncpy;
}
/*****************************************************************************
 函 数 名  : do_acm_dump
 功能描述  : 打印acm 信息
 输入参数  : acm_ncpy : acm_ncopy_ctx数据指针
 输出参数  : 无
 返 回 值  : NCPY_OK 正常dump，NCPY_ERROR dump失败
*****************************************************************************/
static int do_acm_dump(struct acm_ncopy_ctx *acm_ncpy)
{
        struct acm_ctx *acm = acm_ncpy->acm;
        struct tty_struct *tty = NULL;
        struct acm_ldisc_priv *acm_ldisc_ctx = NULL;
        struct semaphore *sem = NULL;
        unsigned long flags;
        unsigned int count;

        acm_dump_print("acm_ncpy[%d] info:\n", (int)(acm_ncpy - g_acm_ncpy));
        acm_dump_print("acm         :%p\n", acm_ncpy->acm);
        acm_dump_print("tty         :%p\n", acm_ncpy->tty);
        acm_dump_print("port        :%p\n", acm_ncpy->port);
        acm_dump_print("tx_lock     :%p\n", &acm_ncpy->tx_lock);
        acm_dump_print("tx_list     :%p\n", &acm_ncpy->tx_list);
        acm_dump_print("free_list   :%p\n", &acm_ncpy->free_list);
        acm_dump_print("tx_task     :%p\n", &acm_ncpy->tx_task);
        acm_dump_print("allocated   :%d\n", acm_ncpy->allocated);
        acm_dump_print("available   :%d\n", acm_ncpy->available);
        acm_dump_print("tx_got      :%d\n", acm_ncpy->tx_got);
        acm_dump_print("tx_discard  :%d\n", acm_ncpy->tx_discard);
        acm_dump_print("tx_queued   :%d\n", acm_ncpy->tx_queued);
        acm_dump_print("tx_sending  :%d\n", acm_ncpy->tx_sending);
        acm_dump_print("tx_sent     :%d\n", acm_ncpy->tx_sent);
        acm_dump_print("tx_fail     :%d\n", acm_ncpy->tx_fail);
        acm_dump_print("tx_cb       :%d\n", acm_ncpy->tx_cb);

        acm_dump_print("acm_ctx info:\n");
        acm_dump_print("open        :%d\n",    acm->bopen);
        acm_dump_print("dev_id      :%d\n",    acm->dev_id);
        acm_dump_print("fd          :0x%lx\n", acm->fd);
        acm_dump_print("tty_name    :%s\n",    acm->tty_name);
        acm_dump_print("readDoneCB  :%p\n",    acm->readDoneCB);
        acm_dump_print("writeDoneCB :%p\n",    acm->writeDoneCB);
        acm_dump_print("freeSendbuff:%p\n",    acm->freeSendbuff);
        acm_dump_print("eventCB     :%p\n",    acm->eventCB);
        acm_dump_print("acm_ncpy    :%p\n",    acm->acm_ncpy);
        acm_dump_print("tty         :%p\n",    acm->tty);
        acm_dump_print("do_copy     :%d\n",    acm->do_copy);
        acm_dump_print("cur_evt     :%d\n",    acm->cur_evt);
        acm_dump_print("ncopy       :%d\n",    acm->ncopy);
        acm_dump_print("phyaddr_from_app:%d\n",acm->phyaddr_from_app);

        tty = (struct tty_struct *)acm->tty;
        if (tty && tty->disc_data) {
                acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;
                sem = &acm_ldisc_ctx->recv_sema;

                acm_dump_print("acm_ldisc_ctx     :%p\n",    acm_ldisc_ctx);
                acm_dump_print("ldisc write_task  :%p\n",    &acm_ldisc_ctx->write_tsk);
                acm_dump_print("ldisc recv_list   :%p\n",    &acm_ldisc_ctx->recv_list);
                acm_dump_print("ldisc write_list  :%p\n",    &acm_ldisc_ctx->write_list);
                acm_dump_print("ldisc r_left_mem  :%p\n",    acm_ldisc_ctx->last_read_left_mem);
                acm_dump_print("ldisc w_left_mem  :%p\n",    acm_ldisc_ctx->last_write_left_mem);
                acm_dump_print("ldisc write_size  :%d\n",    acm_ldisc_ctx->write_size);
                acm_dump_print("ldisc w_succ_size :%d\n",    acm_ldisc_ctx->write_success_size);
                acm_dump_print("ldisc read_size   :%d\n",    acm_ldisc_ctx->read_size);
                acm_dump_print("ldisc tty_recv_size:%d\n",    acm_ldisc_ctx->tty_recv_size);
                acm_dump_print("ldisc read_sema   :%p\n",    &acm_ldisc_ctx->recv_sema);
                acm_dump_print("ldisc ps_recv_size   :%u\n",    acm_ldisc_ctx->ps_recv_size);
                acm_dump_print("ldisc read_cb   :%u\n",    acm_ldisc_ctx->read_cb);
                acm_dump_print("ldisc get_read_buf   :%u\n",    acm_ldisc_ctx->get_read_buf);
                acm_dump_print("ldisc return_read_buf   :%u\n",    acm_ldisc_ctx->return_read_buf);
                acm_dump_print("ldisc get_read_buf_no_data   :%u\n",    acm_ldisc_ctx->get_read_buf_no_data);
                acm_dump_print("ldisc get_read_buf_invalid   :%u\n",    acm_ldisc_ctx->get_read_buf_invalid);
                acm_dump_print("ldisc acm_ldisc_read   :%u\n",    acm_ldisc_ctx->acm_ldisc_read);
                raw_spin_lock_irqsave((raw_spinlock_t *)(&sem->lock), flags);
                count = sem->count;
                raw_spin_unlock_irqrestore((raw_spinlock_t *)(&sem->lock), flags);
                acm_dump_print("ldisc r_sema count:%d\n",    count);


        }
        return NCPY_OK;
}
/*****************************************************************************
 函 数 名  : acm_dump_show
 功能描述  : 打印acm dump帮助菜单
 输入参数  : 无
 输出参数  : 无
 返 回 值  : NCPY_OK
*****************************************************************************/
int acm_dump_show(void)
{
        acm_dump_print("\n\n");
        acm_dump_print(" |=============================================================================================================|\n");
        acm_dump_print(" |udi index    ||        command               || tty          ||          port                || functions    |\n");
        acm_dump_print(" |_____________||______________________________||______________||______________________________||______________|\n");
        acm_dump_print(" | 0x0001      || acm_dump_pcui  | acm_dump 0  || ttyGS1       || 3G PC UI Interface           || AT           |\n");
        acm_dump_print(" | 0x0004      || acm_dump_3gapp | acm_dump 1  || ttyGS3       || 3G Application Interface     || OM           |\n");
        acm_dump_print(" | NA          || acm_dump_3ggps | acm_dump 2  || ttyGS0       || 3G GPS Interface             || ashell       |\n");
        acm_dump_print(" | 0x0003      || acm_dump_app   | acm_dump 3  || ttyGS5       || Application Interface        || LTE-OM       |\n");
        acm_dump_print(" | 0x0002      || acm_dump_bt    | acm_dump 4  || ttyGS4       || Bluetooth Interface          || cshell       |\n");
        acm_dump_print(" | 0x0000      || acm_dump_ctrl  | acm_dump 5  || ttyGS2       || Control Interface            || AT-CTRL      |\n");
        acm_dump_print(" | 0x0006      || acm_dump_gps   | acm_dump 6  || ttyGS6       || GPS interface                || LTE-GPS      |\n");
        acm_dump_print(" |=============================================================================================================|\n");
        return NCPY_OK;
}
/*****************************************************************************
 函 数 名  : find_acm_ctx_by_tty_name
 功能描述  : find acm_ncopy_ctx struct data by tty_name
 输入参数  : tty_name
 输出参数  : 无
 返 回 值  : struct acm_ncopy_ctx address, or NULL if not find.
*****************************************************************************/
struct acm_ncopy_ctx *find_acm_ctx_by_tty_name(const char *tty_name)
{
        int acm_index = 0;
        struct acm_ncopy_ctx *acm_ncpy = NULL;
        struct acm_ctx *acm = NULL;

        for(acm_index = 0; acm_index < USB_ACM_COM_UDI_NUM; acm_index++) {
                acm_ncpy = acm_ncopy_get_ctx(acm_index);
                if(NULL == acm_ncpy) {
                        continue;
                }
                acm = acm_ncpy->acm;
                if(!strncmp(acm->tty_name, tty_name, sizeof("ttyGSx") - 1)) {
                        break;
                }
        }

        return acm_ncpy;

}
/*****************************************************************************
 函 数 名  : acm_dump
 功能描述  : acm零拷贝模块信息dump接口
 输入参数  : acm_idx : acm设备端口索引
 输出参数  : 无
 返 回 值  : NCPY_OK 正常dump，NCPY_ERROR dump失败
*****************************************************************************/
int acm_dump(int acm_idx)
{
        struct acm_ncopy_ctx *acm_ncpy = NULL;
        const char *tty_name[USB_ACM_COM_UDI_NUM] = {
        /* this array can't change sequence */
                ATCOM_TTY_NAME,
                OMCOM_TTY_NAME,
                ASHELL_TTY_NAME,
                LTEOMCOM_TTY_NAME,
                CSHELL_TTY_NAME,
                ATCTRL_TTY_NAME,
                GPS_TTY_NAME
        };

        if(USB_ACM_COM_UDI_NUM <= acm_idx || 0 > acm_idx) {
                acm_dump_print("acm_ncopy_dump invalid index[%d]\n", acm_idx);
                return NCPY_ERROR;
        }
        acm_ncpy = find_acm_ctx_by_tty_name(tty_name[acm_idx]);
        if(NULL == acm_ncpy) {
                return NCPY_ERROR;
        }

        return do_acm_dump(acm_ncpy);
}

/*****************************************************************************
 宏    名  : DEF_ACM_DUMP_FUNCTION
 功能描述  : 定义acm dump函数，根据传入后缀定义不同的功能
 输入参数  : port定义acm dump函数后缀
 输出参数  : 无
 返 回 值  : 无
 注    意  ：请不要修改
*****************************************************************************/
#define DEF_ACM_DUMP_FUNCTION(port) \
int acm_dump_##port(void)\
{\
        return acm_dump(ACM_DUMP_##port); \
}

enum { 
        ACM_DUMP_pcui = 0,
        ACM_DUMP_3gapp,
        ACM_DUMP_3ggps,
        ACM_DUMP_app,
        ACM_DUMP_bt,
        ACM_DUMP_ctrl,
        ACM_DUMP_gps
};

DEF_ACM_DUMP_FUNCTION(pcui)
DEF_ACM_DUMP_FUNCTION(3gapp)
DEF_ACM_DUMP_FUNCTION(3ggps)
DEF_ACM_DUMP_FUNCTION(app)
DEF_ACM_DUMP_FUNCTION(bt)
DEF_ACM_DUMP_FUNCTION(ctrl)
DEF_ACM_DUMP_FUNCTION(gps)
#endif /* ACM_DUMP_DRV1_PRIVATE */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



