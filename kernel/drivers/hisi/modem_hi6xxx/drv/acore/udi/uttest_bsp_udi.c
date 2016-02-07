

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/fd.h>
#include <linux/tty.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <asm-generic/fcntl.h>

#include "bsp_udi_drv.h"
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

extern struct acm_ctx     g_acm_priv[];
extern struct dev_tty_map g_dev2tty[];
extern int                g_acm_type_tbl[];

/*****************************************************************************
  3 函数实现
*****************************************************************************/



void stub_write_cb(char *buf, int len)
{
    return;
}


int test_get_dev_ctx(unsigned int dev_id)
{
    struct acm_ctx *p_acm_ctx = NULL;
    char *dev_path  = NULL;
    int dev_type = 0;
    int i = 0;

    dev_type = UDI_GET_ACM_FD(dev_id);

    /*根据设备type查找对应TTY设备的路径*/
    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++){
        if (g_dev2tty[i].dev_type == (unsigned int)dev_type){
            dev_path = g_dev2tty[i].path;
            break;
        }
    }

    if (dev_path == NULL) {
        printf("dev_type 0x%x is invalid\n", dev_type);
        return ERROR;
    }

    g_acm_priv[i].bopen    = 0;
    g_acm_priv[i].readDoneCB   = NULL;
    g_acm_priv[i].writeDoneCB  = NULL;
    g_acm_priv[i].eventCB      = NULL;
    g_acm_priv[i].freeSendbuff = NULL;

    p_acm_ctx = &g_acm_priv[i];

    return (int)p_acm_ctx;
}
int test_udi_acm_init(void)
{
    int ret = 0;

    ret = udi_acm_init();

    return ret;
}


int test_udi_acm_open_invalid_devid(void)
{
    UDI_OPEN_PARAM_S acm_param;
    UDI_HANDLE handle = 0;
    int ret = 0;

    acm_param.devid = UDI_ACM_MODEM_ID;
    acm_param.pPrivate = NULL;

    ret = udi_acm_open(&acm_param, handle);

    return ret;
}


int test_udi_acm_open_success(void)
{
    UDI_OPEN_PARAM_S acm_param;
    UDI_HANDLE handle = 0;
    int acm_ctx = 0;
    int ret = 0;

    acm_param.devid = UDI_ACM_AT_ID;
    acm_param.pPrivate = NULL;

    acm_ctx = (int)test_get_dev_ctx(acm_param.devid);

    udi_acm_close(acm_ctx);

    ret = udi_acm_open(&acm_param, handle);

    return ret;
}


int test_acm_open_dev_path_null(void)
{
    unsigned int dev_id;
    int ret = 0;

    dev_id = UDI_ACM_MODEM_ID;
    ret = acm_open(dev_id);

    return ret;
}


int test_acm_open_reopen(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_AT_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    udi_acm_close(acm_ctx);

    ret = acm_open(dev_id);
    ret = acm_open(dev_id);

    return ret;
}


int test_acm_open_success(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_AT_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    udi_acm_close(acm_ctx);

    ret = acm_open(dev_id);

    return ret;
}


int test_udi_acm_close_success(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);
    ret = udi_acm_close(acm_ctx);

    return ret;
}


int test_udi_acm_read_not_openned(void)
{
    unsigned int dev_id = 0;
    unsigned char buf = '0';
    unsigned int size = 1;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    udi_acm_close((unsigned int)acm_ctx);

    ret = udi_acm_read(acm_ctx, &buf, size);

    return ret;
}


int test_udi_acm_read_buf_null(void)
{
    unsigned int dev_id = 0;
    unsigned char *buf = NULL;
    unsigned int size = 1;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    ret = udi_acm_read(acm_ctx, buf, size);

    return ret;
}


int test_udi_acm_read_size_zero(void)
{
    unsigned int dev_id = 0;
    unsigned char buf = '0';
    unsigned int size = 0;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    ret = udi_acm_read(acm_ctx, &buf, size);

    return ret;
}


int test_udi_acm_read_success(void)
{
    unsigned int dev_id = 0;
    unsigned char buf = '0';
    unsigned int size = 1;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    ret = udi_acm_read(acm_ctx, &buf, size);

    return ret;
}


int test_udi_acm_write_not_openned(void)
{
    unsigned int dev_id = 0;
    unsigned char buf = '0';
    unsigned int size = 1;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    udi_acm_close((unsigned int)acm_ctx);

    ret = udi_acm_write(acm_ctx, &buf, size);

    return ret;
}


int test_udi_acm_write_buf_null(void)
{
    unsigned int dev_id = 0;
    unsigned char *buf = NULL;
    unsigned int size = 1;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    ret = udi_acm_write(acm_ctx, buf, size);

    return ret;
}


int test_udi_acm_write_size_zero(void)
{
    unsigned int dev_id = 0;
    unsigned char buf = '0';
    unsigned int size = 0;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    ret = udi_acm_write(acm_ctx, &buf, size);

    return ret;
}


int test_udi_acm_write_success(void)
{
    unsigned int dev_id = 0;
    unsigned char buf = '0';
    unsigned int size = 1;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    ret = udi_acm_write(acm_ctx, &buf, size);
    return ret;
}


int test_udi_acm_ioctl_not_openned(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    udi_acm_close(acm_ctx);

    cmd = ACM_IOCTL_SET_READ_CB;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_acm_set_read_cb(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_SET_READ_CB;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_udi_set_read_cb(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = UDI_IOCTL_SET_READ_CB;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_acm_set_write_cb(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_SET_WRITE_CB;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_udi_set_write_cb(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = UDI_IOCTL_SET_WRITE_CB;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_acm_set_event_cb(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_SET_EVT_CB;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_acm_set_free_cb(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_SET_FREE_CB;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_write_async_no_buf(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    cmd = ACM_IOCTL_WRITE_ASYNC;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_write_async_zero_len(void)
{
    unsigned int dev_id = 0;
    ACM_WR_ASYNC_INFO async_mem = {0,0,0};
    int acm_ctx = 0;
    int cmd = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    cmd = ACM_IOCTL_WRITE_ASYNC;

    ret = udi_acm_ioctl(acm_ctx, cmd, (char *)(&async_mem));

    return ret;
}


int test_udi_acm_ioctl_write_async_success(void)
{
    unsigned int dev_id = 0;
    char buf[] = "abcdefg";
    int acm_ctx = 0;
    int cmd = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_WRITE_ASYNC;
    ACM_WR_ASYNC_INFO async_mem = {buf, 0, 0};
    async_mem.u32Size =  sizeof(buf);
    async_mem.pDrvPriv = NULL;

    ret = udi_acm_ioctl(acm_ctx, cmd, (char *)(&async_mem));

    return ret;
}


int test_udi_acm_ioctl_get_read_buf(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_GET_RD_BUFF;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_return_buf(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_RETURN_BUFF;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_relloc_read_buf(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_RELLOC_READ_BUFF;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_import_done(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_IS_IMPORT_DONE;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_send_buf_can_dma(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_SEND_BUFF_CAN_DMA;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_write_do_copy(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = ACM_IOCTL_WRITE_DO_COPY;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_udi_acm_ioctl_invalid_cmd(void)
{
    unsigned int dev_id = 0;
    int acm_ctx = 0;
    int cmd = 0;
    int arg = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_ctx = acm_open(dev_id);

    cmd = 0xFFFFFFFF;
    arg = 0x1;
    ret = udi_acm_ioctl(acm_ctx, cmd, arg);

    return ret;
}


int test_acm_async_write_not_openned(void)
{

    unsigned int dev_id = 0;
    unsigned char buf = '0';    
	unsigned char vbuf = '0';
    unsigned int size = 1;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    udi_acm_close((unsigned int)acm_ctx);

    ret = acm_async_write(acm_ctx, &vbuf, &buf, size);

    return ret;
}


int test_acm_async_write_buf_null(void)
{
    unsigned int dev_id = 0;
    unsigned char *buf = NULL;    
	unsigned char vbuf = '0';
    unsigned int size = 1;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    ret = acm_async_write(acm_ctx, &vbuf, buf, size);

    return ret;
}


int test_acm_async_write_size_zero(void)
{
    unsigned int dev_id = 0;
    unsigned char buf = '0';    
	unsigned char vbuf = '0';
    unsigned int size = 0;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    ret = acm_async_write(acm_ctx, &vbuf, &buf, size);

    return ret;
}


int test_acm_async_write_success(void)
{
    unsigned int dev_id = 0;
    unsigned char buf = '0';    
	unsigned char vbuf = '0';
    unsigned int size = 1;
    int acm_ctx = 0;
    int ret = 0;

    dev_id = UDI_ACM_CTRL_ID;

    acm_ctx = (int)test_get_dev_ctx(dev_id);
    acm_open(dev_id);

    ((struct acm_ctx *)acm_ctx)->writeDoneCB = stub_write_cb;

    ret = acm_async_write(acm_ctx, &vbuf, &buf, size);

    return ret;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


