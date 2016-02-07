

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/tty.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/fcntl.h>


#include "linux/hisi/bsp_udi_adp.h"
#include "acm_ldisc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int stub_write(struct tty_struct *tty, const unsigned char *buf, int count);

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
struct tty_struct tty;
struct acm_ldisc_priv acm_ldisc_ctx;

const struct tty_operations stub_tty_ops = {
    .write =    stub_write,
};

int g_count = 0;

extern struct acm_ctx g_acm_priv[];
extern char *acm_dev_name[];

/*****************************************************************************
  3 函数实现
*****************************************************************************/

void acm_ut_init()
{
    int i;
    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
            g_acm_priv[i].tty_name = acm_dev_name[i];
            g_acm_priv[i].bopen    = ACM_DEV_CLOSE;
            g_acm_priv[i].readDoneCB   = NULL;
            g_acm_priv[i].writeDoneCB  = NULL;
            g_acm_priv[i].eventCB      = NULL;
            g_acm_priv[i].freeSendbuff = NULL;
    }

    INIT_LIST_HEAD(&acm_ldisc_ctx.recv_list);
    INIT_LIST_HEAD(&acm_ldisc_ctx.write_list);
    acm_ldisc_ctx.last_read_left_mem  = NULL;
    acm_ldisc_ctx.last_write_left_mem = NULL;
    acm_ldisc_ctx.write_size          = 0;
    acm_ldisc_ctx.write_success_size  = 0;
    acm_ldisc_ctx.tty = &tty;

    tty.ops = &stub_tty_ops;
}

int stub_write(struct tty_struct *tty, const unsigned char *buf, int count)
{
    return 4;
}

void stub_read_down()
{
    struct acm_mem_info   *acm_mem_node = (struct acm_mem_info *)malloc(sizeof(struct acm_mem_info));
    acm_mem_node->valid_size = 8;
    acm_mem_node->mem = malloc(acm_mem_node->valid_size);
    memset(acm_mem_node->mem,'a',acm_mem_node->valid_size);
    acm_mem_node->current_pos = (unsigned int)acm_mem_node->mem;
    acm_mem_node->left_size   = acm_mem_node->valid_size;

    list_add_tail(&acm_mem_node->list, &acm_ldisc_ctx.recv_list);

}

int stub_count(void)
{
    g_count++;
	return g_count;
}


int test_acm_ldisc_open()
{

    int ret = 0;
    ret = acm_ldisc_open(&tty);
    return ret;
}



int test_acm_free_list()
{
    int ret = 0;
    struct list_head list;
    struct acm_mem_info   *acm_mem_node = (struct acm_mem_info *)malloc(sizeof(struct acm_mem_info));

    INIT_LIST_HEAD(&list);

    acm_mem_node->valid_size = 8;
    acm_mem_node->mem = malloc(acm_mem_node->valid_size);

    list_add_tail(&acm_mem_node->list, &list);

    acm_free_list(&list);

    if (list_empty(&list)) {
        ret = OK;
    } else {
        ret = ERROR;
    }

    if (NULL != acm_mem_node) {
        acm_mem_node = NULL;
    }

    return ret;
}


int test_acm_ldisc_close_ldisc_data_null()
{
    int ret = 0;
    tty.disc_data = NULL;

   acm_ldisc_close(&tty);

    return ret;
}


int test_acm_ldisc_close_normal()
{
    int ret = 0;
    struct acm_ldisc_priv *test_ctx;

    test_ctx = (struct acm_ldisc_priv*)malloc(sizeof(struct acm_ldisc_priv));
    INIT_LIST_HEAD(&test_ctx->recv_list);
    INIT_LIST_HEAD(&test_ctx->write_list);

    tty.disc_data = (void *)test_ctx;

    acm_ldisc_close(&tty);

	free(test_ctx);
    tty.disc_data = NULL;
    test_ctx      = NULL;

    return ret;
}


int test_acm_ldisc_read_ok()
{
    int ret = 0;
    char *buf = (char *)malloc(8);

    tty.disc_data = &acm_ldisc_ctx;

    ret = acm_ldisc_read(&tty, NULL, buf, 8);

    tty.disc_data = NULL;

    return ret;
}


int test_acm_ldisc_read_left_mem()
{
    int ret = 0;
    char *buf = (char *)malloc(8);
    char *stub_mem = (char *)malloc(8);
    struct acm_mem_info   *acm_mem_node = (struct acm_mem_info *)malloc(sizeof(struct acm_mem_info));

    acm_mem_node->mem         = (void *)stub_mem;
    acm_mem_node->left_size   = 8;
    acm_mem_node->current_pos = (unsigned int)stub_mem;
    acm_mem_node->valid_size  = 10;

    acm_ldisc_ctx.last_read_left_mem = acm_mem_node;
    tty.disc_data = &acm_ldisc_ctx;

    ret = acm_ldisc_read(&tty, NULL, buf, 4);

    tty.disc_data = NULL;
    free(buf);
    free(stub_mem);
    free(acm_mem_node);

    return ret;

}


int test_acm_ldisc_write_async()
{
    int ret = 0;
    struct file pfile;

    char *buf = (char *)malloc(8);
    memset(buf,'a',8);

    pfile.f_flags = 0;
    pfile.f_flags |= O_NONBLOCK;
    tty.disc_data = &acm_ldisc_ctx;
    tty.ops       = &stub_tty_ops;

    ret = acm_ldisc_write(&tty, &pfile, buf, 8);

    //list_del_init(acm_ldisc_ctx.write_list.next);
    tty.disc_data = NULL;
    //free(buf);
    buf = NULL;

    return ret;
}


int test_acm_ldisc_write_sync()
{
    int ret = 0;
    struct file pfile;

    char *buf = (char *)malloc(8);
    memset(buf,'a',8);

    struct acm_ldisc_priv *test_ldisc_ctx;
    test_ldisc_ctx = (struct acm_ldisc_priv*)malloc(sizeof(struct acm_ldisc_priv));

    spin_lock_init(&test_ldisc_ctx->recv_lock);
    spin_lock_init(&test_ldisc_ctx->write_lock);

    INIT_LIST_HEAD(&test_ldisc_ctx->recv_list);
    INIT_LIST_HEAD(&test_ldisc_ctx->write_list);

	
    tty.disc_data = &test_ldisc_ctx;
    pfile.f_flags = 0;

    ret = acm_ldisc_write(&tty, &pfile, buf, 8);

    list_del_init(&test_ldisc_ctx->write_list);
    test_ldisc_ctx->write_size          = 0;
    test_ldisc_ctx->write_success_size  = 0;
    tty.disc_data = NULL;
    free(buf);
    free(test_ldisc_ctx);
    buf = NULL;

    return ret;
}


int test_acm_write_task_list_empty()
{
    int ret = 0;

    acm_write_task((unsigned long)&acm_ldisc_ctx);

    if (0 == acm_ldisc_ctx.write_success_size) {
        ret = OK;
    } else {
        ret = ERROR;
    }

    g_count = 0;
    acm_ldisc_ctx.write_size          = 0;
    acm_ldisc_ctx.write_success_size  = 0;

    return ret;
}


int test_acm_write_task_list_node()
{
    int ret = 0;
    struct acm_mem_info   *acm_mem_node = (struct acm_mem_info *)malloc(sizeof(struct acm_mem_info));

    acm_mem_node->valid_size = 8;
    acm_mem_node->mem = malloc(acm_mem_node->valid_size);
    memset(acm_mem_node->mem, 'a', 8);
    acm_mem_node->current_pos = (unsigned int)acm_mem_node->mem;
    acm_mem_node->left_size   = acm_mem_node->valid_size;

    list_add_tail(&acm_mem_node->list, &acm_ldisc_ctx.write_list);

    acm_write_task((unsigned long)&acm_ldisc_ctx);

    if (4 == acm_ldisc_ctx.write_success_size) {
        ret = OK;
    } else {
        ret = ERROR;
    }

    acm_ldisc_ctx.last_write_left_mem = NULL;
    acm_ldisc_ctx.write_size          = 0;
    acm_ldisc_ctx.write_success_size  = 0;
    free(acm_mem_node->mem);
    acm_mem_node->mem = NULL;

    return ret;
}


int test_acm_write_task_left_node()
{
    int ret = 0;
    struct acm_mem_info   *acm_mem_node = (struct acm_mem_info *)malloc(sizeof(struct acm_mem_info));

    acm_mem_node->valid_size = 4;
    acm_mem_node->mem = malloc(acm_mem_node->valid_size);
    memset(acm_mem_node->mem, 'a', 4);
    acm_mem_node->current_pos = (unsigned int)acm_mem_node->mem;
    acm_mem_node->left_size   = acm_mem_node->valid_size;

    acm_ldisc_ctx.last_write_left_mem = acm_mem_node;

    acm_write_task((unsigned long)&acm_ldisc_ctx);

    if (4 == acm_ldisc_ctx.write_success_size) {
        ret = OK;
    } else {
        ret = ERROR;
    }

    acm_ldisc_ctx.last_write_left_mem = NULL;
    acm_ldisc_ctx.write_size          = 0;
    acm_ldisc_ctx.write_success_size  = 0;
    acm_mem_node->mem = NULL;

    return ret;
}


int test_acm_ldisc_receive_buf_cshell()
{
    int ret = 0;
    char *buf = (char *)malloc(8);

    memcpy(tty.name ,CSHELL_TTY_NAME ,strlen(CSHELL_TTY_NAME));
    g_acm_priv[0].readDoneCB = (ACM_READ_DONE_CB_T)stub_count;
    tty.disc_data = &acm_ldisc_ctx;

    acm_ldisc_receive_buf(&tty, buf, NULL, 8);

    if (1 == g_count) {
        ret = OK;
    } else {
        ret = ERROR;
    }

    tty.disc_data = NULL;
    g_count = 0;
    g_acm_priv[0].readDoneCB = NULL;

    return ret;
}


#if 0
int test_acm_ldisc_receive_buf_no_up()
{
    int ret = 0;
    char *buf = (char *)malloc(8);

    memcpy(tty.name ,ATCOM_TTY_NAME ,strlen(ATCOM_TTY_NAME));

  	struct acm_ldisc_priv *test_ldisc_ctx;
    test_ldisc_ctx = (struct acm_ldisc_priv*)malloc(sizeof(struct acm_ldisc_priv));

    spin_lock_init(&test_ldisc_ctx->recv_lock);
    spin_lock_init(&test_ldisc_ctx->write_lock);

    INIT_LIST_HEAD(&test_ldisc_ctx->recv_list);
    INIT_LIST_HEAD(&test_ldisc_ctx->write_list);

	tty.disc_data = &test_ldisc_ctx;
    test_ldisc_ctx->recv_sema.count = 1;

    acm_ldisc_receive_buf(&tty, buf, NULL, 8);

    if (0 == g_count) {
        ret = OK;
    } else {
        ret = ERROR;
    }

    tty.disc_data = NULL;
    g_count = 0;
    test_ldisc_ctx->recv_sema.count = 0;

	free(test_ldisc_ctx);

    return ret;
}
#endif


int test_acm_ldisc_write_wakeup_err()
{
    int ret = 0;
    tty.disc_data = NULL;

    acm_ldisc_write_wakeup(&tty);

    if (1 == g_count) {
        ret = OK;
    } else {
        ret = ERROR;
    }

    g_count = 0;

    return ret;
}


int test_acm_ldisc_write_wakeup_ok()
{
    int ret = 0;
    tty.disc_data = &acm_ldisc_ctx;

    acm_ldisc_write_wakeup(&tty);

    if (1 == g_count) {
        ret = OK;
    } else {
        ret = ERROR;
    }

    tty.disc_data = NULL;
    g_count = 0;

    return ret;
}


int test_acm_ldisc_init()
{
    int ret = 0;

    ret = acm_ldisc_init();

    return ret;
}



int test_acm_ldisc_exit()
{
    int ret = -1;

    acm_ldisc_exit();

    return ret;
}


int test_acm_ldisc_ioctl_check_arg()
{
    int ret = 0;

    struct tty_struct tty;
    struct file       file;

    unsigned int      cmd;
    unsigned long     arg = 0;

    cmd = LDISC_IOCTL_GET_RD_BUF;

    //arg = (unsigned long)malloc(sizeof(ACM_WR_ASYNC_INFO));

    ret = acm_ldisc_ioctl(&tty, &file, cmd, arg);

    return ret;
}


int test_acm_ldisc_ioctl_get_rd_buf()
{
    int ret = 0;

    struct tty_struct tty;
    struct file       file;

    unsigned int      cmd;
    unsigned long     arg;

    cmd = LDISC_IOCTL_GET_RD_BUF;

    arg = (unsigned long)malloc(sizeof(ACM_WR_ASYNC_INFO));

    ret = acm_ldisc_ioctl(&tty, &file, cmd, arg);

    return ret;
}


int test_acm_ldisc_ioctl_return_buf()
{
    int ret = 0;

    struct tty_struct tty;
    struct file       file;

    unsigned int      cmd;
    unsigned long     arg;

    cmd = LDISC_IOCTL_RETURN_BUF;

    arg = (unsigned long)malloc(sizeof(ACM_WR_ASYNC_INFO));

    ret = acm_ldisc_ioctl(&tty, &file, cmd, arg);

    return ret;
}


int test_acm_ldisc_ioctl_default()
{
    int ret = 0;

    struct tty_struct tty;
    struct file       file;

    unsigned int      cmd;
    unsigned long     arg;

    cmd = 0xffffffff;

    arg = (unsigned long)malloc(sizeof(ACM_WR_ASYNC_INFO));

    ret = acm_ldisc_ioctl(&tty, &file, cmd, arg);

    return ret;
}


int test_acm_ldisc_get_rd_buf_left_mem_null()
{
    int ret = 0;

    struct tty_struct tty;
    ACM_WR_ASYNC_INFO mem_info;
    struct acm_ldisc_priv *test_ldisc_ctx;
    struct acm_mem_info *acm_mem_node;

    tty.disc_data = malloc(sizeof(struct acm_ldisc_priv));
    test_ldisc_ctx = (struct acm_ldisc_priv*)tty.disc_data;
    test_ldisc_ctx->last_read_left_mem = NULL;

    spin_lock_init(&test_ldisc_ctx->recv_lock);
    spin_lock_init(&test_ldisc_ctx->write_lock);

    INIT_LIST_HEAD(&test_ldisc_ctx->recv_list);
    INIT_LIST_HEAD(&test_ldisc_ctx->write_list);

    acm_mem_node = (struct acm_mem_info *)malloc(sizeof(struct acm_mem_info));
    acm_mem_node->valid_size = 8;
    acm_mem_node->mem = malloc(acm_mem_node->valid_size);
    memset(acm_mem_node->mem, 'a', acm_mem_node->valid_size);
    acm_mem_node->current_pos = (unsigned int)acm_mem_node->mem;
    acm_mem_node->left_size   = acm_mem_node->valid_size;

    list_add_tail(&acm_mem_node->list, &test_ldisc_ctx->recv_list);

    ret = acm_ldisc_get_rd_buf(&tty, &mem_info);

    return ret;
}



int test_acm_ldisc_get_rd_buf_left_mem_data()
{
    int ret = 0;

    struct tty_struct tty;
    ACM_WR_ASYNC_INFO mem_info;
    struct acm_ldisc_priv *test_ldisc_ctx;
    struct acm_mem_info *acm_mem_node;

    tty.disc_data = malloc(sizeof(struct acm_ldisc_priv));
    test_ldisc_ctx = (struct acm_ldisc_priv*)tty.disc_data;

    acm_mem_node = (struct acm_mem_info *)malloc(sizeof(struct acm_mem_info));
    acm_mem_node->valid_size = 8;
    acm_mem_node->mem = malloc(acm_mem_node->valid_size);
    memset(acm_mem_node->mem, 'a', acm_mem_node->valid_size);
    acm_mem_node->current_pos = (unsigned int)acm_mem_node->mem;
    acm_mem_node->left_size   = acm_mem_node->valid_size;

    test_ldisc_ctx->last_read_left_mem = acm_mem_node;

    spin_lock_init(&test_ldisc_ctx->recv_lock);
    spin_lock_init(&test_ldisc_ctx->write_lock);

    INIT_LIST_HEAD(&test_ldisc_ctx->recv_list);
    INIT_LIST_HEAD(&test_ldisc_ctx->write_list);

    ret = acm_ldisc_get_rd_buf(&tty, &mem_info);

    return ret;
}


int test_acm_ldisc_get_rd_buf_list_empty()
{
    int ret = 0;

    struct tty_struct tty;
    ACM_WR_ASYNC_INFO mem_info;
    struct acm_ldisc_priv *test_ldisc_ctx;
    struct acm_mem_info *acm_mem_node;

    tty.disc_data = malloc(sizeof(struct acm_ldisc_priv));
    test_ldisc_ctx = (struct acm_ldisc_priv*)tty.disc_data;
    test_ldisc_ctx->last_read_left_mem = NULL;

    spin_lock_init(&test_ldisc_ctx->recv_lock);
    spin_lock_init(&test_ldisc_ctx->write_lock);

    INIT_LIST_HEAD(&test_ldisc_ctx->recv_list);
    INIT_LIST_HEAD(&test_ldisc_ctx->write_list);

    ret = acm_ldisc_get_rd_buf(&tty, &mem_info);

    return ret;
}


int test_acm_ldisc_return_buf()
{
    int ret = 0;

    struct tty_struct tty;
    ACM_WR_ASYNC_INFO mem_info = {0};

    mem_info.pDrvPriv = (unsigned long)malloc(sizeof(struct acm_mem_info));

    memset(mem_info.pDrvPriv, 0x0, sizeof(struct acm_mem_info));

    ret = acm_ldisc_return_buf(&tty, &mem_info);

    return ret;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

int test_acm_ldisc_return_buf_error()
{
    int ret = 0;

    struct tty_struct tty;
    ACM_WR_ASYNC_INFO mem_info = {0};

    mem_info.pDrvPriv = (unsigned long)malloc(sizeof(struct acm_mem_info));

    memset(mem_info.pDrvPriv, 0x0, sizeof(struct acm_mem_info));

    ret = acm_ldisc_return_buf(&tty, 0);

    return ret;
}



