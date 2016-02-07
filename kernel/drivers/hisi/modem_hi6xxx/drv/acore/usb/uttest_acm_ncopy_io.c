

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/tty.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <u_serial.h>

#include "linux/hisi/bsp_udi_adp.h"

#include "acm_ncopy_io.h"
#include "acm_ldisc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

extern struct acm_ctx g_acm_priv[];
extern struct acm_ncopy_ctx g_acm_ncpy[];
struct usb_request g_usb_req;

static int test_ep_queue_fail(struct usb_ep *usb_ep, struct usb_request *usb_req,
		    gfp_t gfp_flags)
{
    return -1;
}

static int test_ep_queue(struct usb_ep *usb_ep, struct usb_request *usb_req,
		    gfp_t gfp_flags)
{
    return 0;
}

struct usb_request * test_alloc_request_null(struct usb_ep *ep, gfp_t gfp_flags)
{
    return 0;
}

struct usb_request * test_alloc_request(struct usb_ep *ep, gfp_t gfp_flags)
{
    return &g_usb_req;
}

static void test_free_request(struct usb_ep *ep, struct usb_request *req)
{
    return;
}

static struct usb_ep_ops ep_ops = {
	.queue = test_ep_queue,
	.alloc_request = test_alloc_request_null,
	.free_request = test_free_request,
};

int g_free_called = 0;
int g_write_done_called = 0;

static void test_free_cb(char *buf)
{
    g_free_called++;
}

static void test_write_done_cb(char *buf, int size)
{
    g_write_done_called++;
}


/*****************************************************************************
  3 函数实现
*****************************************************************************/


int test_acm_ncopy_init_ncopy_invalid(void)
{
    int acm[USB_ACM_COM_UDI_NUM];
	int acm_dev = 0;
	int ret  = 0;
	int i = 0;

	for (i = 0; i < USB_ACM_COM_UDI_NUM; i++)
	{
	    acm[i] = (int)g_acm_ncpy[i].acm;
        g_acm_ncpy[i].acm = (void *)0x12345678;
	}

	acm_dev = (int)(&g_acm_priv[0]);
    ret = acm_ncopy_init(acm_dev);

	for (i = 0; i < USB_ACM_COM_UDI_NUM; i++)
	{
        g_acm_ncpy[i].acm = (void *)acm[i];
	}

    return ret;
}


int test_acm_ncopy_init_alloc_fail(void)
{
	int acm_dev = 0;
	int ret  = 0;

	acm_dev = (int)(&g_acm_priv[0]);

    ret = acm_ncopy_init(acm_dev);

    return ret;
}


int test_acm_ncopy_init_success(void)
{
	int acm_dev = 0;
	int ret  = 0;

	acm_dev = (int)(&g_acm_priv[0]);
    ret = acm_ncopy_init(acm_dev);

    return ret;
}


int test_acm_ncopy_uninit_ncopy_invalid(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
	int acm_dev = 0;
	int ret  = 0;

	acm_dev = (int)(&g_acm_priv[0]);

	acm_ncpy = (struct acm_ncopy_ctx *)g_acm_priv[0].acm_ncpy;
	g_acm_priv[0].acm_ncpy = 0;
    ret = acm_ncopy_uninit(acm_dev);
    g_acm_priv[0].acm_ncpy = acm_ncpy;

    return ret;
}


int test_acm_ncopy_uninit_success(void)
{
	int acm_dev = 0;
	int ret  = 0;

	struct acm_ncopy_ctx acm_ncpy ={0};
	 NIT_LIST_HEAD(&acm_ncpy.tx_list);

	acm_dev = (int)(&g_acm_priv[0]);
	g_acm_priv[0].acm_ncpy = &acm_ncpy;
    ret = acm_ncopy_uninit(acm_dev);

    return ret;
}


int test_acm_ncopy_alloc_requests_req_null(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct usb_ep ep;
    int allocated = 0;
	int ret  = 0;

    acm_ncpy = &g_acm_ncpy[0];
    ep.ops = &ep_ops;
    INIT_LIST_HEAD(&acm_ncpy->free_list);

    ret = acm_ncopy_alloc_requests(&acm_ncpy->free_list, 0, &allocated);

    return ret;
}


int test_acm_ncopy_alloc_requests_success(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct usb_request req;
    struct usb_ep ep;
    int allocated = 0;
	int ret  = 0;

    acm_ncpy = &g_acm_ncpy[0];
    ep_ops.alloc_request = test_alloc_request;
    ep.ops = &ep_ops;
    INIT_LIST_HEAD(&acm_ncpy->free_list);
    list_add(&req.list, &acm_ncpy->free_list);

    ret = acm_ncopy_alloc_requests(&acm_ncpy->free_list, 0, &allocated);

    return ret;
}


int test_acm_ncopy_free_requests_success(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct acm_ctx *acm = NULL;
    struct usb_ep ep;
    struct usb_request req;
    int allocated = 1;
	int ret  = 0;

    acm_ncpy = &g_acm_ncpy[0];
    acm = (struct acm_ctx *)acm_ncpy->acm;
    INIT_LIST_HEAD(&acm_ncpy->free_list);
    list_add(&req.list, &acm_ncpy->free_list);

    ret = allocated;
    acm_ncopy_free_requests(&acm_ncpy->free_list, &allocated);

    ret = ((ret - allocated) == 1) ? 0 : -1;

    return ret;
}



int test_acm_ncopy_start_tx_ctx_null(void)
{
    unsigned char *buf = "abcd";	
	unsigned char *vbuf = "abcd";
    unsigned int size = 4;
    int acm_dev = 0;
	int ret  = 0;

    ret = acm_ncopy_start_tx(acm_dev,vbuf, buf, size);

    return ret;
}


int test_acm_ncopy_start_tx_buf_null(void)
{
    unsigned char *buf = NULL;	
	unsigned char *vbuf = "abcd";
    unsigned int size = 4;
    int acm_dev = 0;
	int ret  = 0;

	acm_dev = (int)(&g_acm_priv[0]);

    ret = acm_ncopy_start_tx(acm_dev,vbuf, buf, size);

    return ret;
}


int test_acm_ncopy_start_tx_size_0(void)
{
    unsigned char *buf = "abcd";	
	unsigned char *vbuf = "abcd";
    unsigned int size = 0;
    int acm_dev = 0;
	int ret  = 0;

	acm_dev = (int)(&g_acm_priv[0]);

    ret = acm_ncopy_start_tx(acm_dev,vbuf, buf, size);

    return ret;
}


int test_acm_ncopy_start_tx_size_invalid(void)
{
    unsigned char *buf = "abcd";	
	unsigned char *vbuf = "abcd";
    unsigned int size = 100000;
    int acm_dev = 0;
	int ret  = 0;

	acm_dev = (int)(&g_acm_priv[0]);

    ret = acm_ncopy_start_tx(acm_dev,vbuf, buf, size);

    return ret;
}


int test_acm_ncopy_start_tx_available(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    unsigned char *buf = "abcd";
		unsigned char *vbuf = "abcd";
    unsigned int size = 4;
    int available = 0;
    int acm_dev = 0;
	int ret  = 0;

	acm_dev = (int)(&g_acm_priv[0]);
	((struct acm_ctx *)acm_dev)->acm_ncpy = (struct acm_ncopy_ctx *)malloc(sizeof(struct acm_ncopy_ctx));
	acm_ncpy = (struct acm_ncopy_ctx *)((struct acm_ctx *)acm_dev)->acm_ncpy;
    available = acm_ncpy->available;
	acm_ncpy->available = 1;
	INIT_LIST_HEAD(&acm_ncpy->free_list);

    ret = acm_ncopy_start_tx(acm_dev,vbuf, buf, size);

    acm_ncpy->available = available;
    return ret;
}


int test_acm_ncopy_start_tx_unavailable(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    unsigned char *buf = "abcd";
	unsigned char *vbuf = "abcd";
    unsigned int size = 4;
    int available = 0;
    int acm_dev = 0;
	int ret  = 0;

	acm_dev = (int)(&g_acm_priv[0]);
	acm_ncpy = (struct acm_ncopy_ctx *)((struct acm_ctx *)acm_dev)->acm_ncpy;
    available = acm_ncpy->available;
	acm_ncpy->available = 0;

    ret = acm_ncopy_start_tx(acm_dev,vbuf, buf, size);

    acm_ncpy->available = available;
    return ret;
}


int test_acm_ncopy_queue_list_empty(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    unsigned char *buf = "abcd";
    struct list_head *free_pool = NULL;
    struct list_head *next = NULL;
    unsigned int size = 4;
	int ret  = 0;

	acm_ncpy = (struct acm_ncopy_ctx *)(&g_acm_ncpy[0]);
	free_pool = &acm_ncpy->free_list;

    ret = acm_ncopy_queue(acm_ncpy, buf, size);

    return ret;
}


int test_acm_ncopy_queue_success(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    unsigned char *buf = "abcd";
    struct list_head *free_pool = NULL;
    struct list_head *next = NULL;
    struct usb_request req;
    unsigned int size = 4;
	int ret  = 0;

	acm_ncpy = (struct acm_ncopy_ctx *)(&g_acm_ncpy[0]);
	free_pool = &acm_ncpy->free_list;

    list_add(&req.list, free_pool);
    ret = acm_ncopy_queue(acm_ncpy, buf, size);

    return ret;
}


int test_acm_ncopy_tx_task_queue_fail(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct list_head *tx_pool = NULL;
    struct usb_request req;
    unsigned int tx_fail = 0;
    struct usb_ep ep_in;
	int ret  = 1;

	struct gserial port;

	g_acm_ncpy[0].port = &port;

	acm_ncpy = (struct acm_ncopy_ctx *)(&g_acm_ncpy[0]);
	tx_pool = &acm_ncpy->tx_list;
	tx_fail = acm_ncpy->tx_fail;

    ep_ops.queue = test_ep_queue_fail;
    ep_in.ops = &ep_ops;
	((struct gserial *)acm_ncpy->port)->in = &ep_in;

    INIT_LIST_HEAD(tx_pool);
    list_add(&req.list, tx_pool);

    acm_ncopy_tx_task(acm_ncpy);

    ret = acm_ncpy->tx_fail - tx_fail;

    return ret;
}


int test_acm_ncopy_tx_task_tx_success(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct acm_ctx *acm = NULL;
    struct list_head *tx_pool = NULL;
    struct usb_request req;
    unsigned int tx_sending = 0;
    struct usb_ep ep_in;
	int ret  = 1;
	struct gserial port;

	g_acm_ncpy[0].port = &port;

	acm_ncpy = (struct acm_ncopy_ctx *)(&g_acm_ncpy[0]);
	acm = (struct acm_ctx *)acm_ncpy->acm;
	tx_pool = &acm_ncpy->tx_list;
	tx_sending = acm_ncpy->tx_sending;

    ep_ops.queue = test_ep_queue;
    ep_in.ops = &ep_ops;
	((struct gserial *)acm_ncpy->port)->in = &ep_in;

    INIT_LIST_HEAD(tx_pool);
    list_add(&req.list, tx_pool);

    acm->freeSendbuff = test_free_cb;
    acm->writeDoneCB = test_write_done_cb;

    acm_ncopy_tx_task(acm_ncpy);

    ret = acm_ncpy->tx_sending - tx_sending;

    acm->freeSendbuff = 0;
    acm->writeDoneCB = 0;

    return ret;
}


int test_acm_ncopy_tx_complete_send_success(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct usb_ep ep;
    struct usb_request req;
    unsigned int tx_sent = 0;
	int ret  = 0;

	acm_ncpy = (struct acm_ncopy_ctx *)(&g_acm_ncpy[0]);

	tx_sent = acm_ncpy->tx_sent;
	req.context = (void *)acm_ncpy;
	req.status = 0;


    acm_ncopy_tx_complete(&ep, &req);

    ret = acm_ncpy->tx_sent - tx_sent;

    return ret;
}


int test_acm_ncopy_tx_complete_send_fail(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct usb_ep ep;
    struct usb_request req;
    unsigned int tx_fail = 0;
	int ret  = 0;

	acm_ncpy = (struct acm_ncopy_ctx *)(&g_acm_ncpy[0]);

	tx_fail = acm_ncpy->tx_fail;
	req.context = (void *)acm_ncpy;
	req.status = 1;


    acm_ncopy_tx_complete(&ep, &req);

    ret = acm_ncpy->tx_fail - tx_fail;

    return ret;
}


int test_acm_ncopy_tx_complete_free_called(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct acm_ctx *acm = NULL;
    struct usb_ep ep;
    struct usb_request req;
    int free = 0;
	int ret  = 0;

	acm_ncpy = (struct acm_ncopy_ctx *)(&g_acm_ncpy[0]);
    acm = (struct acm_ctx *)acm_ncpy->acm;

    acm->freeSendbuff = test_free_cb;
	free = g_free_called;
	req.context = (void *)acm_ncpy;
	req.status = 0;

    acm_ncopy_tx_complete(&ep, &req);

    ret = g_free_called - free;
    acm->freeSendbuff = 0;

    return ret;
}


int test_acm_ncopy_tx_complete_write_done_called(void)
{
    struct acm_ncopy_ctx *acm_ncpy = NULL;
    struct acm_ctx *acm = NULL;
    struct usb_request req;
    struct usb_ep ep;
    int write_done = 0;
	int ret  = 0;

	acm_ncpy = (struct acm_ncopy_ctx *)(&g_acm_ncpy[0]);
    acm = (struct acm_ctx *)acm_ncpy->acm;

    acm->writeDoneCB = test_write_done_cb;
	write_done = g_write_done_called;
	req.context = (void *)acm_ncpy;
	req.status = 0;

    acm_ncopy_tx_complete(&ep, &req);

    ret = g_write_done_called - write_done;
    acm->writeDoneCB = 0;

    return ret;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


