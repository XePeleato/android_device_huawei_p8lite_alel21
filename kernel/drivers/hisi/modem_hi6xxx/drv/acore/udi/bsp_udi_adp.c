/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : bsp_udi_adp.c
  版 本 号   : 初稿
  作    者   : 夏青 00195127
  生成日期   : 2012年9月12日
  最近修改   :
  功能描述   : udi功能注册适配
  函数列表   :
              acm_async_write
              acm_open
              udi_acm_close
              udi_acm_init
              udi_acm_ioctl
              udi_acm_open
              udi_acm_read
              udi_acm_write
  修改历史   :
  1.日    期   : 2012年9月12日
    作    者   : 夏青 00195127
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include <linux/fd.h>
#include <linux/tty.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/of.h>
#ifndef _DRV_LLT_
#include <linux/dma-mapping.h>
#endif
#include <asm-generic/fcntl.h>

#include  "bsp_udi_drv.h"
#include  <bsp_udi_adp.h>
#include  "acm_ncopy_io.h"
#include  "acm_ldisc.h"
#include  "mdrv_udi.h"
#include  <drv_usb_if.h>     /*BSP_ACM_RegCmdHandle*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


unsigned int BSP_USB_RegUdiDisableCB(USB_UDI_DISABLE_CB_T pFunc);
int acm_handle_command(int cmd, int tty_idx, char *buf, int size);

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* ACM COM使用与UDI模块对应的FD列表 */
int g_acm_type_tbl[UDI_USB_ACM_MAX+1]=
{
    DEVICE_FD_CTRL,
    DEVICE_FD_ATCOM,
    DEVICE_FD_SHELL,
    DEVICE_FD_LTE_OM,
    DEVICE_FD_OMCOM,
    DEVICE_FD_MODEM,
    DEVICE_FD_GPS,
    DEVICE_FD_3G_GPS,
    DEVICE_FD_3G_PCVOICE,
    DEVICE_FD_PCVOICE,
    DEVICE_FD_NUM
};

struct dev_tty_map g_dev2tty[USB_ACM_COM_UDI_NUM] = {
    {DEVICE_FD_3G_GPS,   "/dev/ttyGS0"},
    {DEVICE_FD_SHELL,    "/dev/ttyGS4"},
    {DEVICE_FD_ATCOM,    "/dev/ttyGS1"},
    {DEVICE_FD_OMCOM,    "/dev/ttyGS3"},
    {DEVICE_FD_CTRL,     "/dev/ttyGS2"},
    {DEVICE_FD_LTE_OM,   "/dev/ttyGS5"},
    {DEVICE_FD_GPS,      "/dev/ttyGS6"},
};

char *acm_dev_name[USB_ACM_COM_UDI_NUM] = {
    ASHELL_TTY_NAME,
    CSHELL_TTY_NAME,
    ATCOM_TTY_NAME,
    OMCOM_TTY_NAME,
    ATCTRL_TTY_NAME,
    LTEOMCOM_TTY_NAME,
    GPS_TTY_NAME,
};

int g_acm_adp_dbg_lev = 3;  /* KERN_ERR */

#define ACM_ADP_ERR(args,...) do {\
    if (3 <= g_acm_adp_dbg_lev) {\
        printk("%s:%d:", __func__, __LINE__);\
        printk(args, ##__VA_ARGS__);\
    }\
}while(0)

#define ACM_ADP_DBG(args,...) do {\
    if (7 <= g_acm_adp_dbg_lev) {\
        printk("%s:%d:", __func__, __LINE__);\
        printk(args, ##__VA_ARGS__);\
    }\
}while(0)

#define ACM_NEED_NCOPY(tty_name) \
   ((!strncmp(tty_name, LTEOMCOM_TTY_NAME, strlen(LTEOMCOM_TTY_NAME))) ? 1 : \
    (!strncmp(tty_name, ASHELL_TTY_NAME,   strlen(ASHELL_TTY_NAME))) ? 1 : \
    (!strncmp(tty_name, CSHELL_TTY_NAME,   strlen(CSHELL_TTY_NAME))) ? 1 : \
    (!strncmp(tty_name, OMCOM_TTY_NAME,    strlen(OMCOM_TTY_NAME))) ? 1 : \
    (!strncmp(tty_name, ATCOM_TTY_NAME,    strlen(ATCOM_TTY_NAME))) ? 1 : \
    (!strncmp(tty_name, ATCTRL_TTY_NAME,   strlen(ATCTRL_TTY_NAME))) ? 1 : \
    (!strncmp(tty_name, GPS_TTY_NAME,      strlen(GPS_TTY_NAME))) ? 1 : 0 )

#define ADP_ERROR           (-1)
#define ADP_OK              0

static int udiAdpIccInit(VOID);
static int  acm_handle_status_notifier(int tty_idx, char *buf, int size);
static int  acm_suspend_notify(struct notifier_block *nb, unsigned long event, void *buf);
static int  acm_resume_notify(struct notifier_block *nb, unsigned long event, void *buf);

static struct notifier_block acm_suspend_notifier = {
    .notifier_call = acm_suspend_notify,
    .priority = INT_MIN,
};

static struct notifier_block acm_resume_notifier = {
    .notifier_call = acm_resume_notify,
    .priority = INT_MIN,
};

#ifdef _DRV_LLT_
ATOMIC_NOTIFIER_HEAD(android_usb_suspend_notifier_list);
ATOMIC_NOTIFIER_HEAD(android_usb_resume_notifier_list);
#else
extern struct atomic_notifier_head android_usb_suspend_notifier_list;
extern struct atomic_notifier_head android_usb_resume_notifier_list;
#endif

struct acm_ctx g_acm_priv[USB_ACM_COM_UDI_NUM];
extern struct acm_ldisc_priv g_acm_ldisc[USB_ACM_COM_UDI_NUM];
/* 各模块初始化函数定义 */
void* g_udiInitFuncTable[(unsigned int)UDI_DEV_MAX+1] =
{

    /* ACM Init */
    udi_acm_init,

    /* ICC Init */
    udiAdpIccInit,

    /* Must Be END */
    NULL
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/
static void acm_event_notify(struct work_struct *data)
{
    struct acm_ctx *acm = container_of(data, struct acm_ctx, notify_work);
    struct acm_ldisc_priv *acm_ldisc_ctx = acm->acm_ldisc_ctx;
    const int evt = acm->cur_evt;

    ACM_ADP_ERR("acm[%s] %s by PC\n", acm->tty_name, evt ? "openned" : "closed");
    /*端口关闭的时候 释放读链表内存 */
    if (0 == evt && acm_ldisc_ctx && acm_ldisc_ctx->tty) {
            ACM_ADP_ERR("acm[%s] need clean recv_list\n", acm_ldisc_ctx->tty_name);

            spin_lock_bh(&acm_ldisc_ctx->recv_lock);

            acm_free_list(&acm_ldisc_ctx->recv_list);
            INIT_LIST_HEAD(&acm_ldisc_ctx->recv_list);

            spin_unlock_bh(&acm_ldisc_ctx->recv_lock);
    }

    if (acm->eventCB != NULL) {
        acm->eventCB(evt);

    }

    ACM_ADP_DBG("acm[%s] evt[%d]\n", acm->tty_name, acm->cur_evt);
}
/**************************************************************************
  ACM 适配实现
**************************************************************************/


/*****************************************************************************
 函 数 名  : udi_acm_open
 功能描述  : acm设备注册到udi适配层open接口，用于AT、OM、AT Ctrl的open操作
 输入参数  : UDI_OPEN_PARAM *param  : 设备的打开配置参数
             UDI_HANDLE handle      : 设备节点handle
                UDI_USB_ACM_CTRL = 0,
                UDI_USB_ACM_AT,
                UDI_USB_ACM_SHELL,
                UDI_USB_ACM_LTE_DIAG,
                UDI_USB_ACM_OM,
                UDI_USB_ACM_MODEM,
                UDI_USB_ACM_GPS,
                UDI_USB_ACM_3G_GPS,
 输出参数  : 无
 返 回 值  : int 成功返回ADP_OK 失败返回ADP_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月12日
    作    者   : 夏青 00195127
    修改内容   : 新生成函数

*****************************************************************************/
int udi_acm_open(UDI_OPEN_PARAM_S *param, UDI_HANDLE handle)
{
    void *ret = 0;
    unsigned int dev_id;

    /* 根据param->dev_id获得设备id */
    dev_id = UDI_GET_DEV_TYPE(param->devid);
    ACM_ADP_DBG("open acm[%d]\n", dev_id);

    ret = acm_open(dev_id);
    if (NULL == ret) {
        ACM_ADP_ERR("open device[%d] failed\n", dev_id);
        return ADP_ERROR;
    }

    BSP_UDI_SetPrivate(param->devid, ret);

    return ADP_OK;
}

/*****************************************************************************
 函 数 名  : udi_acm_init
 功能描述  : acm端口注册到udi适配层初始化函数，用于将acm设备回调挂接到udi上
 输入参数  : void
 输出参数  : 无
 返 回 值  : int 成功返回ADP_OK 失败返回ADP_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月12日
    作    者   : 夏青 00195127
    修改内容   : 新生成函数

*****************************************************************************/
int udi_acm_init(void)
{
    int dev_num;
    int i;
    UDI_DRV_INTEFACE_TABLE *pDrvInterface = NULL;
    struct device_node *np = NULL;
    int phyaddr_from_app = 1;/*默认使用协议栈传下来的物理地址*/

    pDrvInterface = (UDI_DRV_INTEFACE_TABLE *)kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE),GFP_KERNEL);
    if (NULL == pDrvInterface) {
        ACM_ADP_ERR("kmalloc failed\n");
        return ADP_ERROR;
    }
    ACM_ADP_DBG("enter\n");

    /* 挂接UDI回调函数 */
    pDrvInterface->udi_open_cb  = (UDI_OPEN_CB_T)udi_acm_open;
    pDrvInterface->udi_close_cb = (UDI_CLOSE_CB_T)udi_acm_close;
    pDrvInterface->udi_write_cb = (UDI_WRITE_CB_T)udi_acm_write;
    pDrvInterface->udi_read_cb  = (UDI_READ_CB_T)udi_acm_read;
    pDrvInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)udi_acm_ioctl;

    /* 设置acm设备驱动回调函数，3个acm使用同一套驱动指针 */
    for (dev_num = UDI_USB_ACM_CTRL; dev_num < UDI_USB_ACM_MAX; dev_num++){
        (void)BSP_UDI_SetCapability((UDI_DEVICE_ID_E)UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, dev_num), UDI_USB_ACM_CAPA);
        (void)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID_E)UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, dev_num), pDrvInterface);
    }

    np = of_find_compatible_node(NULL, NULL, "hisilicon,hi6xxx-usb-otg");
    if (np) {
        if (of_property_read_u32(np, "huawei,phyaddr_from_app",&phyaddr_from_app)){
            printk(KERN_ERR "%s:huawei,phyaddr_from_app read error\n",__FUNCTION__);
        }
        printk(KERN_INFO "%s:huawei,phyaddr_from_app %d\n",__FUNCTION__,phyaddr_from_app);
    }

    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
        g_acm_priv[i].tty_name      = acm_dev_name[i];
        g_acm_priv[i].bopen         = ACM_DEV_CLOSE;
        g_acm_priv[i].readDoneCB    = NULL;
        g_acm_priv[i].writeDoneCB   = NULL;
        g_acm_priv[i].eventCB       = NULL;
        g_acm_priv[i].freeSendbuff  = NULL;
        g_acm_priv[i].acm_ncpy      = NULL;
        g_acm_priv[i].tty           = NULL;
        g_acm_priv[i].acm_ldisc_ctx = NULL;
        g_acm_priv[i].do_copy       = 0;
        g_acm_priv[i].cur_evt       = ACM_EVT_DEV_SUSPEND;
        g_acm_priv[i].ncopy         = 0;
        g_acm_priv[i].phyaddr_from_app = 0;
        INIT_WORK(&g_acm_priv[i].notify_work, acm_event_notify);

        if (ACM_NEED_NCOPY(g_acm_priv[i].tty_name)) {
            g_acm_priv[i].ncopy = 1;
            g_acm_priv[i].phyaddr_from_app = phyaddr_from_app;
        }

    }

    atomic_notifier_chain_register(&android_usb_suspend_notifier_list, &acm_suspend_notifier);
    atomic_notifier_chain_register(&android_usb_resume_notifier_list,  &acm_resume_notifier);

    BSP_ACM_RegCmdHandle((ACM_HANDLE_COMMAND_CB_T)acm_handle_command);

    return ADP_OK;
}

/*****************************************************************************
 函 数 名  : acm_open
 功能描述  : acm打开设备
 输入参数  : unsigned int dev_id  :需打开设备的ID
 输出参数  : 无
 返 回 值  : int 成功返回设备私有数据 失败返回ADP_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月12日
    作    者   : 夏青 00195127
    修改内容   : 新生成函数

*****************************************************************************/
void *acm_open(unsigned int dev_id)
{
    int  i;
    int  dev_type;
    int  ldisc;
    long dev_fd;
    mm_segment_t   fs         = 0;
    char           *dev_path  = NULL;
    struct acm_ctx *p_acm_ctx = NULL;
    void *tty    = 0;
    void *port   = NULL;
    long rc      = 0;

    /* 根据param->dev_id获得设备type */
    dev_type = UDI_GET_ACM_FD(dev_id);
    ACM_ADP_DBG("open acm[%d]\n", dev_id);

    /* 根据设备type查找对应TTY设备的路径 */
    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++){
        if (g_dev2tty[i].dev_type == dev_type){
            dev_path = g_dev2tty[i].path;
            break;
        }
    }

    if (NULL == dev_path) {
        ACM_ADP_ERR("device[%d] is invalid\n",dev_type);
        return NULL;
    }

    p_acm_ctx = &g_acm_priv[i];

    if (ACM_DEV_OPEN == p_acm_ctx->bopen) {
        ACM_ADP_ERR("device[%d] is already open\n", dev_type);
        return NULL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);

    /* sys_open打开tty设备获得fd */
    dev_fd = sys_open(dev_path, O_RDWR, 0);
    if (dev_fd < 0) {
        /* 打印打开tty设备失败 */
        ACM_ADP_ERR("open tty[%s] failed fd[0x%lx]\n",
                p_acm_ctx->tty_name, dev_fd);
        set_fs(fs);
        return NULL;
    }
    ACM_ADP_DBG("[%s] fd[%d]\n", p_acm_ctx->tty_name, (int)dev_fd);

    /* 设置ACM的l_disc线性规程 */
    ldisc = N_USB_COM;
    rc = sys_ioctl(dev_fd, TIOCSETD, (unsigned long)(&ldisc));
    if (ADP_OK != rc) {
        ACM_ADP_ERR("ioctl - [%s] TIOCSETD fail[0x%x]\n", p_acm_ctx->tty_name, (unsigned int)rc);
        sys_close((unsigned int)dev_fd);
        set_fs(fs);
        return NULL;
    }
    ACM_ADP_DBG("[%s] TIOCSETD rc[%ld]\n", p_acm_ctx->tty_name, rc);

    rc = sys_ioctl((unsigned int)dev_fd, (unsigned int)LDISC_IOCTL_GET_TTY,
                (unsigned long)(&tty));
    if (ADP_OK != rc) {
        ACM_ADP_ERR("ioctl - get tty[%s] fail[0x%x]\n", p_acm_ctx->tty_name, (unsigned int)rc);
        sys_close((unsigned int)dev_fd);
        set_fs(fs);
        return NULL;
    }
    ACM_ADP_DBG("[%s] GET_TTY rc[%ld]\n", p_acm_ctx->tty_name, rc);

    /* Whether tty instance is ready */
    port = acm_ncopy_port_ready(tty);
    if (!port) {
        /* acm<-->tty<---X--->port<-->gser */
        ACM_ADP_ERR("tty[%s] port not ready(enum)\n", p_acm_ctx->tty_name);
        sys_close((unsigned int)dev_fd);
        set_fs(fs);
        return NULL;
    }
    set_fs(fs);

    /* 将dev_id、fd填入ACM私有数据全局变量结构体中 */
    p_acm_ctx->bopen  = ACM_DEV_OPEN;
    p_acm_ctx->dev_id = dev_id;
    p_acm_ctx->fd     = dev_fd;
    p_acm_ctx->tty    = tty;

    if (p_acm_ctx->ncopy) {
        acm_ncopy_init((void *)p_acm_ctx);
    }
    ACM_ADP_DBG("leave\n");
    return (void *)p_acm_ctx;
}

/*****************************************************************************
 函 数 名  : udi_acm_close
 功能描述  : acm关闭设备
 输入参数  : unsigned int acm_dev  :需关闭设备
 输出参数  : 无
 返 回 值  : int 成功返回ADP_OK 失败返回ADP_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月12日
    作    者   : 夏青 00195127
    修改内容   : 新生成函数

*****************************************************************************/
int udi_acm_close(void *acm_dev)
{
    mm_segment_t   fs         = 0;
    struct acm_ctx *p_acm_ctx = (struct acm_ctx *)acm_dev;

    ACM_ADP_DBG("[%s] enter\n", p_acm_ctx->tty_name);

    if (ACM_DEV_CLOSE == p_acm_ctx->bopen) {
        ACM_ADP_ERR("acm[%s] is not open\n", p_acm_ctx->tty_name);
        return ADP_ERROR;
    }

    /* 将open标志位置0 */
    p_acm_ctx->bopen = ACM_DEV_CLOSE;

    fs = get_fs();
    set_fs(KERNEL_DS);

    /* 清空保存的对应tty设备fd */
    sys_close(p_acm_ctx->fd);

    set_fs(fs);

    if (p_acm_ctx->ncopy) {
        acm_ncopy_uninit((void *)p_acm_ctx);
    }

    p_acm_ctx->fd = INVALID_ACM_FD;

    /* 清空回调函数 */
    /*p_acm_ctx->eventCB      = NULL;
    p_acm_ctx->freeSendbuff = NULL;
    p_acm_ctx->readDoneCB   = NULL;
    p_acm_ctx->writeDoneCB  = NULL;*/
    ACM_ADP_DBG("[%s] leave\n", p_acm_ctx->tty_name);
    return ADP_OK;
}

/*****************************************************************************
 函 数 名  : udi_acm_read
 功能描述  : acm读函数，阻塞接口
 输入参数  : int acm_dev           :对象设备
             unsigned char *pbuff  :传入buffer的首地址
             unsigned int size     :读取的字节数,读取的字节数不能为0
 输出参数  : 无
 返 回 值  : int: 成功：读完成的字节数  错误：ADP_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月12日
    作    者   : 夏青 00195127
    修改内容   : 新生成函数

*****************************************************************************/
int udi_acm_read(void *acm_dev, unsigned char *pbuff, unsigned int size)
{
    long fd;
    int len;
    mm_segment_t   fs         = 0;
    struct acm_ctx *p_acm_ctx = (struct acm_ctx *)acm_dev;

    ACM_ADP_DBG(" acm[%s] size[%d]\n", p_acm_ctx->tty_name, size);
    if (!p_acm_ctx->bopen){
        ACM_ADP_ERR("acm[%s] is not opened\n", p_acm_ctx->tty_name);
        return ADP_ERROR;
    }

    if ((NULL == pbuff)||(size == 0)) {
        ACM_ADP_ERR("acm[%s] para is invalid\n", p_acm_ctx->tty_name);
        return ADP_ERROR;
    }

    /* 根据acm_dev_id获得设备对应tty设备fd */
    fd = p_acm_ctx->fd;

    fs = get_fs();
    set_fs(KERNEL_DS);

    /* 调用fd的read读取数据，并且得到返回值，返回值为读取数据的长度len */
    len = sys_read(fd, pbuff, size);

    set_fs(fs);

    ACM_ADP_DBG(" acm[%s] read len[0x%x]\n", p_acm_ctx->tty_name, len);
    return len;
}

/*****************************************************************************
 函 数 名  : udi_acm_write
 功能描述  : acm写函数，阻塞接口
 输入参数  : int acm_dev           : 对象设备
             unsigned char *pbuff  : 传入buffer的首地址
             unsigned int size     : 数据的字节数
 输出参数  : 无
 返 回 值  : int 写完成的字节数
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月12日
    作    者   : 夏青 00195127
    修改内容   : 新生成函数

*****************************************************************************/
int udi_acm_write(void *acm_dev, unsigned char *pbuff, unsigned int size)
{
    long fd;
    int  len;
    long flag = 0;
    mm_segment_t   fs         = 0;
    struct acm_ctx *p_acm_ctx = (struct acm_ctx *)acm_dev;

    ACM_ADP_DBG(" acm[%s] size[%d]\n", p_acm_ctx->tty_name, size);
    /* 检查open标志位 */
    if (!p_acm_ctx->bopen) {
        ACM_ADP_ERR("acm[%s] is not opened\n", p_acm_ctx->tty_name);
        return ADP_ERROR;
    }

    if ((NULL == pbuff)||(size == 0)) {
        ACM_ADP_ERR("acm[%s] para is invalid\n", p_acm_ctx->tty_name);
        return ADP_ERROR;
    }

    /* 根据acm_dev_id获得设备对应tty设备fd */
    fd = p_acm_ctx->fd;

    fs = get_fs();
    set_fs(KERNEL_DS);

    /* 将NONBLOCK标志位清除 */
    sys_fcntl(fd, F_GETFL, flag);
    flag &= ~O_NONBLOCK;
    sys_fcntl(fd, F_SETFL, flag);

    /* 调用fd的write写入数据，并且得到返回值，返回值为写数据的长度len */
    len = sys_write(fd, pbuff, size);

    set_fs(fs);
    /*udi_acm_write该接口已经不再使用回调临时打桩*/
    if (p_acm_ctx->writeDoneCB) {
        p_acm_ctx->writeDoneCB(pbuff,NULL,len);
    }
    ACM_ADP_DBG(" acm[%s] writeCB[%p] len[%d]\n",
        p_acm_ctx->tty_name, p_acm_ctx->writeDoneCB, size);

    return len;
}

/*****************************************************************************
 函 数 名  : udi_acm_ioctl
 功能描述  : USB ACM 业务相关 ioctl 设置
 输入参数  : int acm_dev  : 对象设备
             int cmd      : 命令码
             int arg      : 命令参数
 输出参数  : 无
 返 回 值  : int 成功返回ADP_OK 失败返回ADP_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月13日
    作    者   : 夏青 00195127
    修改内容   : 新生成函数

*****************************************************************************/
int udi_acm_ioctl(void *acm_dev, int cmd, void *arg)
{
    long fd;

    int ret = ADP_OK;
    ACM_WR_ASYNC_INFO *async_buf;
    struct acm_ctx *p_acm_ctx = (struct acm_ctx *)acm_dev;

    ACM_ADP_DBG(" acm[%s] cmd[0x%x]\n", p_acm_ctx->tty_name, cmd);

    /* 检查open标志位 */
    if (!p_acm_ctx->bopen) {
        ACM_ADP_ERR("acm[%s] is not opened, cmd[0x%x]\n",
                p_acm_ctx->tty_name, cmd);
        return ADP_ERROR;
    }
    fd = p_acm_ctx->fd;

    switch (cmd) {
    case ACM_IOCTL_SET_READ_CB:
    case UDI_IOCTL_SET_READ_CB:
        /* 将arg转定义填充到读回调中 */
        p_acm_ctx->readDoneCB = (ACM_READ_DONE_CB_T)arg;
        break;

    case ACM_IOCTL_SET_WRITE_CB:
    case UDI_IOCTL_SET_WRITE_CB:
        /* 将arg转定义填充到写回调中 */
        p_acm_ctx->writeDoneCB = (ACM_WRITE_DONE_CB_T)arg;
        break;

    case ACM_IOCTL_SET_EVT_CB:
        /* 将arg转定义填充到事件回调中 */
        p_acm_ctx->eventCB = (ACM_EVENT_CB_T)arg;
        break;

    case ACM_IOCTL_SET_FREE_CB:
        /* 将arg转定义填充到释放内存回调中 */
        p_acm_ctx->freeSendbuff = (ACM_FREE_CB_T)arg;
        break;

    case ACM_IOCTL_WRITE_ASYNC:
        if (0 == arg) {
            ACM_ADP_ERR("acm[%s] para is invalid\n", p_acm_ctx->tty_name);
            return ADP_ERROR;
        }

        async_buf = (ACM_WR_ASYNC_INFO *)arg;
        ret = acm_async_write(acm_dev,
                              async_buf->pVirAddr,
                              async_buf->pPhyAddr,
                              async_buf->u32Size);

        if (ret != async_buf->u32Size) {
            ACM_ADP_ERR("acm[%s] async write fail ret[0x%x] size[0x%x]\n",
                    p_acm_ctx->tty_name, ret, async_buf->u32Size);
            ret = ADP_ERROR;
        } else {
            ret = ADP_OK;
        }

        break;

    case ACM_IOCTL_GET_RD_BUFF:
        ret = sys_ioctl(fd, LDISC_IOCTL_GET_RD_BUF, (unsigned long)arg);
        if (ADP_OK != ret) {
            ACM_ADP_ERR("acm[%s] ioctl - get read buf fail[0x%x]\n",
                    p_acm_ctx->tty_name, ret);
            return ADP_ERROR;
        }
        break;

    case ACM_IOCTL_RETURN_BUFF:
        ret = sys_ioctl(fd, LDISC_IOCTL_RETURN_BUF, (unsigned long)arg);
        if (ADP_OK != ret) {
            ACM_ADP_ERR("acm[%s] ioctl - return buf fail[0x%x]\n",
                    p_acm_ctx->tty_name, ret);
            return ADP_ERROR;
        }
        break;

    case ACM_IOCTL_RELLOC_READ_BUFF:
        break;

    case ACM_IOCTL_IS_IMPORT_DONE:
        break;

    case ACM_IOCTL_SEND_BUFF_CAN_DMA:
        break;

    case ACM_IOCTL_WRITE_DO_COPY:
        p_acm_ctx->do_copy = (int)arg;
        break;

    default:
        ret = ADP_ERROR;
        break;
    }

    if(ADP_OK != ret)
    {
        ACM_ADP_ERR(" acm[%s] cmd:[0x%x] ret:[0x%x]\n",
                p_acm_ctx->tty_name, cmd, ret);
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : acm_async_write
 功能描述  : acm写函数，非阻塞接口
 输入参数  : int acm_dev           : 对象设备
             unsigned char *pbuff  : 传入buffer的首地址
             unsigned int size     : 源数据的字节数
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月13日
    作    者   : 夏青 00195127
    修改内容   : 新生成函数

*****************************************************************************/
int acm_async_write(void *acm_dev, char *pVirAddr, char *pPhyAddr, unsigned int size)
{
    long fd;
    int len = (int)ADP_ERROR;
    mm_segment_t   fs         = 0;
    struct acm_ctx *p_acm_ctx = (struct acm_ctx *)acm_dev;
    struct acm_ncopy_ctx * acm_ncpy = (struct acm_ncopy_ctx *)p_acm_ctx->acm_ncpy;
    int rc = ADP_OK;

    ACM_ADP_DBG(" acm[%s] pVirAddr[%p] pPhyAddr[%p]size[%d]\n",
                p_acm_ctx->tty_name, pVirAddr, pPhyAddr, size);

    if (!p_acm_ctx->bopen) {
        ACM_ADP_ERR("acm[%s] device is not opened\n", p_acm_ctx->tty_name);
        return ADP_ERROR;
    }

    if ((NULL == pVirAddr) || (size == 0)) {
        ACM_ADP_ERR("acm[%s] para is invalid\n", p_acm_ctx->tty_name);
        return ADP_ERROR;
    }

    if (p_acm_ctx->ncopy) {
        #ifndef _DRV_LLT_
        if((0 == p_acm_ctx->phyaddr_from_app)||(NULL == pPhyAddr)){
            pPhyAddr = (char *)dma_map_single(acm_ncpy->tty->dev,(void *)pVirAddr,size,DMA_TO_DEVICE);
        }
        #endif
        rc = acm_ncopy_start_tx((void *)p_acm_ctx, pVirAddr, pPhyAddr, size);
        if (ADP_OK == rc) {
            len = size;
        }
    } else {
        /* 根据acm_dev_id获得设备对应tty设备fd */
        fd = p_acm_ctx->fd;

        fs = get_fs();
        set_fs(KERNEL_DS);

        /* 设置NONBLOCK标志位 */
        sys_fcntl(fd, F_SETFL, O_NONBLOCK);

        /* 调用fd的write写入数据，并且得到返回值，返回值为写数据的长度len */
        len = sys_write(fd, pVirAddr, size);

        set_fs(fs);

        if (len == size) {
            if (p_acm_ctx->writeDoneCB) {
                ACM_ADP_DBG(" acm[%s] cb len[%d]\n", p_acm_ctx->tty_name, len);
                p_acm_ctx->writeDoneCB(pVirAddr, pPhyAddr, len);
            }
        }

    }
    ACM_ADP_DBG(" acm[%s] len[0x%x]\n", p_acm_ctx->tty_name, len);

    return len;
}

/*****************************************************************************
 函 数 名  : acm_suspend_notify
 功能描述  : acm suspend回调接口
 输入参数  : nb
             event
             buf
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年07月03日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
static int  acm_suspend_notify(struct notifier_block *nb, unsigned long event, void *buf)
{
    int i = 0;

    ACM_ADP_DBG("enter\n");
    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
        if (ACM_EVT_DEV_READY == g_acm_priv[i].cur_evt) {
            ACM_ADP_ERR("acm[%s] suspend\n", g_acm_priv[i].tty_name);
            g_acm_priv[i].cur_evt = ACM_EVT_DEV_SUSPEND;
            if (g_acm_priv[i].eventCB) {
                g_acm_priv[i].eventCB(ACM_EVT_DEV_SUSPEND);
            }
        }
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : acm_resume_notify
 功能描述  : acm resume回调接口
 输入参数  : nb
             event
             buf
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年07月03日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
static int  acm_resume_notify(struct notifier_block *nb, unsigned long event, void *buf)
{
    int i = 0;

    ACM_ADP_DBG("enter\n");
    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
        if (ACM_EVT_DEV_SUSPEND == g_acm_priv[i].cur_evt) {
            ACM_ADP_ERR("acm[%s] resume\n", g_acm_priv[i].tty_name);
            g_acm_priv[i].cur_evt = ACM_EVT_DEV_READY;
            if (g_acm_priv[i].eventCB) {
                g_acm_priv[i].eventCB(ACM_EVT_DEV_READY);
            }
        }
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : acm_handle_command
 功能描述  : acm控制通道命令处理接口
 输入参数  : cmd        控制通道命令
             interface  处理命令的接口号
             buf        待处理数据
             size       待处理数据大小
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年07月20日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
int acm_handle_command(int cmd, int tty_idx, char *buf, int size)
{
    int ret = ADP_ERROR;

    ACM_ADP_DBG(" ttyGS%d cmd[0x%x]\n", tty_idx, cmd);
    switch (cmd) {
    case USB_ACM_STATUS_NOTIFIER:
        ret = acm_handle_status_notifier(tty_idx, buf, size);
        break;
        /* any other requirement, add here */
    default:
        break;
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : acm_handle_status_notifier
 功能描述  : acm通道状态通知命令处理接口
 输入参数  : tty_idx    处理命令的接口号
             buf        待处理数据
             size       待处理数据大小
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年07月20日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
static int acm_handle_status_notifier(int tty_idx, char *buf, int size)
{
    struct acm_ctx *acm = NULL;
    struct acm_ldisc_priv *acm_ldisc_ctx = NULL;
    char tty_name[16];
    int evt = 0;
    int i = 0;

    if (!buf || (1 != size)) {
        ACM_ADP_ERR("ttyGS%d param fail buf[%p] size[%d]\n",
                tty_idx, buf, size);
        return ADP_ERROR;
    }

    if ((tty_idx < 0) || (tty_idx > USB_ACM_COM_UDI_NUM)) {
        ACM_ADP_ERR("param fail idx[%d] evt[%d]\n", tty_idx, *buf);
        return ADP_ERROR;
    }

    evt = (int)(*buf);
    snprintf(tty_name, 8, "ttyGS%d", tty_idx);

    ACM_ADP_DBG("acm[%s] evt[%d]\n", tty_name, evt);

    if((5 == tty_idx) || (6 == tty_idx)) {
            for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
                acm_ldisc_ctx = &g_acm_ldisc[i];
                if (0 == strncmp(tty_name, acm_ldisc_ctx->tty_name, strlen(acm_ldisc_ctx->tty_name))) {
                        break;
                    }
                }

            if(USB_ACM_COM_UDI_NUM <= i) {
                    acm_ldisc_ctx = NULL;
            }
    }

    for (i = 0; i < USB_ACM_COM_UDI_NUM; i++) {
        acm = &g_acm_priv[i];
        if (0 == strncmp(tty_name, acm->tty_name, strlen(acm->tty_name))) {
            if (evt != acm->cur_evt) {
                acm->cur_evt = evt;
                acm->acm_ldisc_ctx = acm_ldisc_ctx;
                schedule_work(&acm->notify_work);
                break;
            }
        }
    }

    return ADP_OK;
}

/*****************************************************************************
 函 数 名  : acm_adp_dbg_lev_set
 功能描述  : acmUDI适配层模块打印级别设置接口
 输入参数  : lev : 打印级别
 输出参数  : 无
 返 回 值  : 打印级别
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
int acm_adp_dbg_lev_set(int lev)
{
    g_acm_adp_dbg_lev = lev;
    return g_acm_adp_dbg_lev;
}

/*****************************************************************************
 函 数 名  : acm_adp_dump
 功能描述  : acm UDI适配层信息dump接口
 输入参数  : acm_idx : acm设备端口索引
 输出参数  : 无
 返 回 值  : ADP_OK 正常dump，ADP_ERROR dump失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : 周明发 00145324
    修改内容   : 新生成函数

*****************************************************************************/
int acm_adp_dump(int acm_idx)
{
    struct acm_ctx *acm = NULL;
    struct tty_struct *tty = NULL;
    struct acm_ldisc_priv *acm_ldisc_ctx = NULL;
    struct semaphore *sem = NULL;
    unsigned long flags;
    unsigned int count;

    if ((USB_ACM_COM_UDI_NUM <= acm_idx) || (acm_idx < 0)) {
        printk("acm_adp_dump invalid index[%d]\n", acm_idx);
        return ADP_ERROR;
    }

    acm = &g_acm_priv[acm_idx];
    if (acm) {
        printk("acm_ctx     :%p\n",    acm);
        printk("open        :%d\n",    acm->bopen);
        printk("dev_id      :%d\n",    acm->dev_id);
        printk("fd          :%ld\n",    acm->fd);
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
        printk("phyaddr_from_app:%d\n",acm->phyaddr_from_app);

        tty = (struct tty_struct *)acm->tty;
        if (tty && tty->disc_data) {
            acm_ldisc_ctx = (struct acm_ldisc_priv *)tty->disc_data;
            sem = &acm_ldisc_ctx->recv_sema;

            printk("acm_ldisc_ctx     :%p\n",    acm_ldisc_ctx);
            printk("ldisc write_task  :%p\n",    &acm_ldisc_ctx->write_tsk);
            printk("ldisc recv_list   :%p\n",    &acm_ldisc_ctx->recv_list);
            printk("ldisc write_list  :%p\n",    &acm_ldisc_ctx->write_list);
            printk("ldisc r_left_mem  :%p\n",    acm_ldisc_ctx->last_read_left_mem);
            printk("ldisc w_left_mem  :%p\n",    acm_ldisc_ctx->last_write_left_mem);
            printk("ldisc write_size  :%d\n",    acm_ldisc_ctx->write_size);
            printk("ldisc w_succ_size :%d\n",    acm_ldisc_ctx->write_success_size);
            printk("ldisc read_size   :%d\n",    acm_ldisc_ctx->read_size);
            printk("ldisc tty_recv_size:%d\n",    acm_ldisc_ctx->tty_recv_size);
            printk("ldisc read_sema   :%p\n",    &acm_ldisc_ctx->recv_sema);
            raw_spin_lock_irqsave((raw_spinlock_t *)(&sem->lock), flags);
            count = sem->count;
            raw_spin_unlock_irqrestore((raw_spinlock_t *)(&sem->lock), flags);
            printk("ldisc r_sema count:%d\n",    count);


        }
    }

    return ADP_OK;
}

/*******************************************************************************
                                for test
*******************************************************************************/
#define ACM_TR_TEST

#ifdef ACM_TR_TEST
#include <linux/kthread.h>
#include <linux/delay.h>

#define OP_READ             0
#define OP_WRITE            1
#define OP_LOOP             2
#define OP_SEIZE            3
#define OP_STOP             4


#define THREAD_CREATED      (0x12345678)
#define ACM_TEST_BUF_SIZE   (0x10000)   /* 64K */
#define ACM_TEST_BUF_NUM    (5)

typedef struct
{
    char *pVirAddr;
    char *pPhyAddr;
    unsigned int  size;
}acm_mem_t;

struct acm_udi_map
{
    int acm_type;
    int udi_type;
};

typedef struct
{
    int     op_mode;
    int     op_loop;
    int     op_dump;
    acm_mem_t   mem[ACM_TEST_BUF_NUM];
    char        thr_name[16];
    int         thr_state;
    struct task_struct  *thread;
    struct semaphore    thr_sema;
}acm_test_ctx_t;


struct acm_udi_map g_acm_udi_map[USB_ACM_COM_UDI_NUM] = {
    {DEVICE_FD_3G_GPS,UDI_ACM_3G_GPS_ID},
    {DEVICE_FD_SHELL,UDI_ACM_SHELL_ID},
    {DEVICE_FD_ATCOM,UDI_ACM_AT_ID},
    {DEVICE_FD_OMCOM,UDI_ACM_OM_ID},
    {DEVICE_FD_CTRL,UDI_ACM_CTRL_ID},
    {DEVICE_FD_LTE_OM, UDI_ACM_LTE_DIAG_ID},
    {DEVICE_FD_GPS,UDI_ACM_GPS_ID},
};

acm_mem_t g_acm_mem[USB_ACM_COM_UDI_NUM];

acm_test_ctx_t g_acm_test_ctx[USB_ACM_COM_UDI_NUM];

static int acm_loop_task(void *arg);
int acm_stop(int acm_idx);

void acm_disconnect_cb(void)
{
    int acm_idx = 0;

    ACM_ADP_DBG("......\n");
    for (acm_idx = 0; acm_idx < USB_ACM_COM_UDI_NUM; acm_idx++)
    {
        if (THREAD_CREATED != g_acm_test_ctx[acm_idx].thr_state)
        {
            continue;
        }

        acm_stop(acm_idx);
    }
}

static int g_acm_idx = 0;
static int acm_test_init(int *acm_idx)
{
    acm_test_ctx_t *acm_ctx = NULL;
    static int cb_reged = 0;
    int idx = *acm_idx;

    if (idx >= USB_ACM_COM_UDI_NUM)
    {
        ACM_ADP_ERR("invalid acm device ID[%d]\n", idx);
        return ADP_ERROR;
    }

    acm_ctx = &g_acm_test_ctx[idx];

    if (THREAD_CREATED == acm_ctx->thr_state)
    {
        ACM_ADP_ERR("thread already created ID[%d]\n", idx);
        return ADP_ERROR;
    }

    snprintf(acm_ctx->thr_name, 10, "acm_loop_%d", idx);
    ACM_ADP_DBG("create thread [%s]\n", acm_ctx->thr_name);

    sema_init(&acm_ctx->thr_sema, 0);

    g_acm_idx = idx;
    acm_ctx->thread = kthread_run(acm_loop_task, (void *)&g_acm_idx, acm_ctx->thr_name);
    if (acm_ctx->thread == ERR_PTR(-ENOMEM)) {
        ACM_ADP_ERR("thread create fail ID[%d]\n", idx);
        return ADP_ERROR;
    }
    acm_ctx->thr_state = THREAD_CREATED;

    if (!cb_reged)
    {
        BSP_USB_RegUdiDisableCB(acm_disconnect_cb);
        cb_reged = 1;
    }
    return ADP_OK;
}

int acm_send(int acm_idx, unsigned char *buf, unsigned int size, int loop)
{
    acm_test_ctx_t *acm_ctx = NULL;


    ACM_ADP_DBG("acm[%d] enter\n", acm_idx);

    if (acm_idx >= USB_ACM_COM_UDI_NUM)
    {
        ACM_ADP_ERR("invalid acm device ID[%d]\n", acm_idx);
        return ADP_ERROR;
    }

    acm_ctx = &g_acm_test_ctx[acm_idx];

    if (THREAD_CREATED != acm_ctx->thr_state)
    {
        acm_test_init(&acm_idx);
    }

    if (!buf && (0 == size))
    {
        ACM_ADP_ERR("invalid buf and size ID[%d]\n", acm_idx);
        return ADP_ERROR;
    }

    if (!acm_ctx->mem[OP_WRITE].pVirAddr)
    {
        acm_ctx->mem[OP_WRITE].pVirAddr = (char *)dma_alloc_coherent(NULL, ACM_TEST_BUF_SIZE,
                (dma_addr_t *)(&acm_ctx->mem[OP_WRITE].pPhyAddr), GFP_KERNEL);
        if (!acm_ctx->mem[OP_WRITE].pVirAddr)
        {
            ACM_ADP_ERR("buf malloc fail ID[%d]\n", acm_idx);
            return ADP_ERROR;
        }
    }

    size = (size > ACM_TEST_BUF_SIZE) ? ACM_TEST_BUF_SIZE : size;
    if (buf)
    {
        memcpy((void *)acm_ctx->mem[OP_WRITE].pVirAddr, (void *)buf, size);
    }
    else
    {
        memset((void *)acm_ctx->mem[OP_WRITE].pVirAddr, 'a', size);
    }

    acm_ctx->op_mode  = OP_WRITE;
    acm_ctx->op_loop  = loop;
    acm_ctx->mem[OP_WRITE].size = size;

    up(&acm_ctx->thr_sema);
    return ADP_OK;
}

int acm_recv(int acm_idx, int loop, int dump)
{
    acm_test_ctx_t *acm_ctx = NULL;

    ACM_ADP_DBG("acm[%d] enter\n", acm_idx);
    if (acm_idx >= USB_ACM_COM_UDI_NUM)
    {
        ACM_ADP_ERR("invalid acm device ID[%d]\n", acm_idx);
        return ADP_ERROR;
    }

    acm_ctx = &g_acm_test_ctx[acm_idx];

    if (THREAD_CREATED != acm_ctx->thr_state)
    {
        acm_test_init(&acm_idx);
    }

    if (!acm_ctx->mem[OP_READ].pVirAddr)
    {
        acm_ctx->mem[OP_READ].pVirAddr = (char *)dma_alloc_coherent(NULL, ACM_TEST_BUF_SIZE,
                (dma_addr_t *)(&acm_ctx->mem[OP_READ].pPhyAddr), GFP_KERNEL);
        if (!acm_ctx->mem[OP_READ].pVirAddr)
        {
            ACM_ADP_ERR("buf malloc fail [%d]\n", acm_idx);
            return ADP_ERROR;
        }
    }

    acm_ctx->mem[OP_READ].size = ACM_TEST_BUF_SIZE;
    acm_ctx->op_mode  = OP_READ;
    acm_ctx->op_loop  = loop;
    acm_ctx->op_dump  = dump;

    up(&acm_ctx->thr_sema);
    return ADP_OK;
}

int acm_loop(int acm_idx)
{
    acm_test_ctx_t *acm_ctx = NULL;

    ACM_ADP_DBG("acm[%d] enter\n", acm_idx);
    if (acm_idx >= USB_ACM_COM_UDI_NUM)
    {
        ACM_ADP_ERR("invalid acm device ID[%d]\n", acm_idx);
        return ADP_ERROR;
    }

    acm_ctx = &g_acm_test_ctx[acm_idx];

    if (THREAD_CREATED != acm_ctx->thr_state)
    {
        acm_test_init(&acm_idx);
    }

    acm_ctx->op_mode  = OP_LOOP;
    acm_ctx->op_loop  = 1;
    acm_ctx->op_dump  = 0;

    up(&acm_ctx->thr_sema);
    return ADP_OK;
}

int acm_stop(int acm_idx)
{
    acm_test_ctx_t *acm_ctx = NULL;

    ACM_ADP_DBG("acm[%d] enter\n", acm_idx);
    if (acm_idx >= USB_ACM_COM_UDI_NUM)
    {
        ACM_ADP_ERR("invalid acm device ID[%d]\n", acm_idx);
        return ADP_ERROR;
    }

    acm_ctx = &g_acm_test_ctx[acm_idx];

    if (THREAD_CREATED != acm_ctx->thr_state)
    {
        ACM_ADP_ERR("acm test not started ID[%d]\n", acm_idx);
        return ADP_ERROR;    }

    acm_ctx->op_mode  = OP_STOP;
    acm_ctx->op_loop  = 0;
    acm_ctx->op_dump  = 0;

    up(&acm_ctx->thr_sema);
    msleep(100);
    kthread_stop(acm_ctx->thread);
    acm_ctx->thr_state = 0;
    return ADP_OK;
}

int acm_seize(int acm_idx)
{
    acm_test_ctx_t *acm_ctx = NULL;

    ACM_ADP_DBG("acm[%d] enter\n", acm_idx);
    if (acm_idx >= USB_ACM_COM_UDI_NUM)
    {
        ACM_ADP_ERR("invalid acm device ID[%d]\n", acm_idx);
        return ADP_ERROR;
    }

    acm_ctx = &g_acm_test_ctx[acm_idx];

    if (THREAD_CREATED != acm_ctx->thr_state)
    {
        ACM_ADP_ERR("acm test not started ID[%d]\n", acm_idx);
        return ADP_ERROR;
    }

    acm_ctx->op_mode = OP_SEIZE;
    acm_ctx->op_loop = 0;
    acm_ctx->op_dump = 0;

    up(&acm_ctx->thr_sema);
    return ADP_OK;
}

static int acm_loop_task(void *arg)
{
    acm_test_ctx_t *acm_ctx = NULL;
    struct acm_ctx *p_acm_ctx = NULL;
    UDI_HANDLE handle = UDI_INVALID_HANDLE;
    UDI_OPEN_PARAM_S param;
    char *pVirAddr = NULL;
    char *pPhyAddr = NULL;
    unsigned int size = 0x2000;
    int acm_idx = *((int*)arg);
    mm_segment_t fs = 0;
    int  len = 0, length = 0, i = 0;
    long fd;
    struct tty_struct *p_tty;

    ACM_ADP_DBG("acm[%d] test enter\n", acm_idx);

    p_acm_ctx = &g_acm_priv[acm_idx];

    fs = get_fs();
    set_fs(KERNEL_DS);

    if (p_acm_ctx->bopen)
    {
        ACM_ADP_ERR("acm[%d] opened by others\n", acm_idx);
        p_acm_ctx->freeSendbuff = 0;
        p_acm_ctx->writeDoneCB  = 0;
        //return ADP_ERROR;
    }
    else
    {
        param.devid = g_acm_udi_map[acm_idx].udi_type;
        handle = mdrv_udi_open(&param);
        if (handle <= 0)
        {
            ACM_ADP_ERR("acm[%d] opened fail\n", acm_idx);
            return ADP_ERROR;
        }
        fd = p_acm_ctx->fd;
    }
    p_tty = (struct tty_struct *)p_acm_ctx->tty;


    acm_ctx = &g_acm_test_ctx[acm_idx];
    if (!acm_ctx->mem[OP_LOOP].pVirAddr)
    {
    #ifdef CONFIG_ARM64
        ACM_ADP_ERR("buf malloc, p_tty is [0x%p]\n", p_tty);
        acm_ctx->mem[OP_LOOP].pVirAddr = (char *)dma_alloc_coherent(p_tty->dev, ACM_TEST_BUF_SIZE,
                (dma_addr_t *)(&acm_ctx->mem[OP_LOOP].pPhyAddr), GFP_KERNEL);
    #else
        acm_ctx->mem[OP_LOOP].pVirAddr = (char *)dma_alloc_coherent(NULL, ACM_TEST_BUF_SIZE,
                (dma_addr_t *)(&acm_ctx->mem[OP_LOOP].pPhyAddr), GFP_KERNEL);
    #endif
        if (!acm_ctx->mem[OP_LOOP].pVirAddr)
        {
            ACM_ADP_ERR("buf malloc fail [%d]\n", acm_idx);
            return ADP_ERROR;
        }
    }

    acm_ctx->mem[OP_LOOP].size = ACM_TEST_BUF_SIZE;

    while (1)
    {
        down(&acm_ctx->thr_sema);

        if ((OP_READ == acm_ctx->op_mode) ||
            (OP_LOOP == acm_ctx->op_mode) ||
            (OP_WRITE == acm_ctx->op_mode))
        {
            pVirAddr  = acm_ctx->mem[acm_ctx->op_mode].pVirAddr;
            pPhyAddr  = acm_ctx->mem[acm_ctx->op_mode].pPhyAddr;
            size = acm_ctx->mem[acm_ctx->op_mode].size;
        }

        while (1)
        {
            if (OP_READ == acm_ctx->op_mode)
            {
                //memset((void *)buf, 0, size);
                //len = sys_read(fd, buf, size);
                //len = udi_read(handle, buf, size);
                len = udi_acm_read((void *)p_acm_ctx, pVirAddr, size);
                if (len <= 0)
                {
                    ACM_ADP_ERR("acm[%d] recv fail [0x%x] \n", acm_idx, len);
                }
                else
                {
                    if (acm_ctx->op_dump)
                    {
                        ACM_ADP_ERR("acm[%d] recv [%d] pVirAddr [%p]\n", acm_idx, len, pVirAddr);
                    }

                    if (acm_ctx->op_loop)
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else if (OP_WRITE == acm_ctx->op_mode)
            {
                //len = sys_write(fd, buf, size);
                //len = udi_write(handle, buf, size);
                len = acm_async_write((void *)p_acm_ctx, pVirAddr, pPhyAddr, size);
                if (len != size)
                {
                    ACM_ADP_ERR("acm[%d] send size mismatch [%d-%d]\n",
                            acm_idx, size, len);
                }

                if (acm_ctx->op_loop)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }

            else if (OP_LOOP == acm_ctx->op_mode)
            {
                /* delete to avoid ncopy bug */
                /*memset((void *)buf, 0, size);*/

                //length = sys_read(fd, buf, size);
                //len = udi_read(handle, buf, size);
                len = udi_acm_read((void *)p_acm_ctx, pVirAddr, size);
                if (len <= 0)
                {
                    ACM_ADP_ERR("acm[%d] loop recv fail [0x%x] \n", acm_idx, len);
                }
                else
                {
                    if (acm_ctx->op_dump)
                    {
                        ACM_ADP_ERR("acm[%d] loop recv [%d] bytes:%s\n",
                            acm_idx, len, pVirAddr);
                    }

                    /*len = sys_write(fd, buf, length);*/
                    length = acm_async_write((void *)p_acm_ctx, pVirAddr, pPhyAddr, len);

                    if (len != length)
                    {
                        ACM_ADP_ERR("acm[%d] loop send size mismatch [%d-%d]\n",
                            acm_idx, length, len);
                    }
                }

                if (acm_ctx->op_loop)
                {
                    continue;
                }
                else
                {
                   break;
                }
            }
            else if (OP_SEIZE == acm_ctx->op_mode)
            {
                ACM_ADP_ERR("acm[%d] test seize\n", acm_idx);
                break;
            }
            else if (OP_STOP == acm_ctx->op_mode)
            {
                ACM_ADP_ERR("acm[%d] test stop\n", acm_idx);
                goto exit;
            }
        }
    }

exit:

    mdrv_udi_close(handle);

    set_fs(fs);

    for (i = 0; i < ACM_TEST_BUF_NUM; i++)
    {
        if (acm_ctx->mem[i].pVirAddr)
        {
        #ifdef CONFIG_ARM64
            dma_free_coherent(p_tty->dev, ACM_TEST_BUF_SIZE, (void *)acm_ctx->mem[i].pVirAddr,
                    (dma_addr_t)(acm_ctx->mem[i].pPhyAddr));
        #else
            dma_free_coherent(NULL, ACM_TEST_BUF_SIZE, (void *)acm_ctx->mem[i].pVirAddr,
                    (dma_addr_t)(acm_ctx->mem[i].pPhyAddr));
        #endif
            acm_ctx->mem[i].pVirAddr = NULL;
            acm_ctx->mem[i].pPhyAddr = NULL;
            acm_ctx->mem[i].size = 0;
        }
    }
    ACM_ADP_DBG("acm[%d] test exit\n", acm_idx);

    return ADP_OK;
}

#endif

/**************************************************************************
  ICC 适配实现
**************************************************************************/
static unsigned int sg_chnHandleTbl[ICC_CHAN_NUM_MAX] = {0};

UDI_HANDLE BSP_UDI_ICC_ChnToHandle(unsigned int u32Chn)
{
    return sg_chnHandleTbl[u32Chn];
}

static int udiIccAdpOpen(UDI_OPEN_PARAM_S *param, UDI_HANDLE handle)
{
    unsigned int u32ChanId;
    ICC_CHAN_ATTR_S *pstOpenParam;

    u32ChanId = UDI_GET_DEV_TYPE(param->devid);
    (BSP_VOID)BSP_UDI_SetPrivate(param->devid, (VOID*)u32ChanId);

    /* 从param 中解析出各个参数 */
    pstOpenParam = (ICC_CHAN_ATTR_S *)param->pPrivate;
    sg_chnHandleTbl[u32ChanId] = handle;

    return BSP_ICC_Open(u32ChanId, pstOpenParam);
}

static int udiAdpIccInit(VOID)
{
    UDI_DRV_INTEFACE_TABLE* pDrvInterface = NULL;
    unsigned int u32Cnt;

    /* 构造回调函数指针列表 */
    pDrvInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
    if (NULL == pDrvInterface)
    {
        printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
        return ADP_ERROR;
    }
    memset((VOID*)pDrvInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

    /* 只设置需要实现的几个接口 */ /* for debug by x00195127*/
    pDrvInterface->udi_open_cb = (UDI_OPEN_CB_T)udiIccAdpOpen;
    pDrvInterface->udi_close_cb = (UDI_CLOSE_CB_T)BSP_ICC_Close;
    pDrvInterface->udi_write_cb = (UDI_WRITE_CB_T)BSP_ICC_Write;
    pDrvInterface->udi_read_cb = (UDI_READ_CB_T)BSP_ICC_Read;
    pDrvInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)BSP_ICC_Ioctl;

    /* 几个 ICC 都使用同一套驱动函数指针 */
    for (u32Cnt = 0; u32Cnt < ICC_CHAN_NUM_MAX; u32Cnt++)
    {
        (BSP_VOID)BSP_UDI_SetCapability((UDI_DEVICE_ID_E)UDI_BUILD_DEV_ID(UDI_DEV_ICC, u32Cnt), UDI_ICC_CAPA);
        (BSP_VOID)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID_E)UDI_BUILD_DEV_ID(UDI_DEV_ICC, u32Cnt), pDrvInterface);
    }

    return ADP_OK;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

