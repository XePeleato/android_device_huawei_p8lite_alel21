/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : bsp_udi_adp.h
  版 本 号   : 初稿
  作    者   : 夏青 00195127
  生成日期   : 2012年9月12日
  最近修改   :
  功能描述   : bsp_udi_adp.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年9月12日
    作    者   : 夏青 00195127
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "BSP.h"


#ifndef __BSP_UDI_ADP_H__
#define __BSP_UDI_ADP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*ACM查找FD type宏*/
extern int sg_AcmTypeTbl[];
#define UDI_GET_ACM_FD(dev_type) g_acm_type_tbl[(unsigned int)(dev_type)]

#define USB_ACM_COM_UDI_NUM  7

#define ASHELL_TTY_NAME    "ttyGS0"
#define CSHELL_TTY_NAME    "ttyGS4"
#define ATCOM_TTY_NAME     "ttyGS1"
#define OMCOM_TTY_NAME     "ttyGS3"
#define ATCTRL_TTY_NAME    "ttyGS2"
#define LTEOMCOM_TTY_NAME  "ttyGS5"
#define GPS_TTY_NAME       "ttyGS6"

/* 各模块特性值定义 */
#define UDI_USB_ACM_CAPA  (UDI_CAPA_BLOCK_READ | UDI_CAPA_BLOCK_WRITE | UDI_CAPA_READ_CB | UDI_CAPA_WRITE_CB)
#define UDI_ICC_CAPA      (UDI_CAPA_BLOCK_READ | UDI_CAPA_BLOCK_WRITE | UDI_CAPA_READ_CB | UDI_CAPA_WRITE_CB)

#define ACM_DEV_OPEN    1
#define ACM_DEV_CLOSE   0

#define INVALID_ACM_FD  -1

/* IOCTL CMD 定义, 移植V3R2*/
#define ACM_IOCTL_SET_WRITE_CB      0x7F001000
#define ACM_IOCTL_SET_READ_CB       0x7F001001
#define ACM_IOCTL_SET_EVT_CB        0x7F001002
#define ACM_IOCTL_SET_FREE_CB       0x7F001003

#define ACM_IOCTL_WRITE_ASYNC       0x7F001010
#define ACM_IOCTL_GET_RD_BUFF       0x7F001011
#define ACM_IOCTL_RETURN_BUFF       0x7F001012
#define ACM_IOCTL_RELLOC_READ_BUFF  0x7F001013
#define ACM_IOCTL_SEND_BUFF_CAN_DMA 0x7F001014

#define ACM_IOCTL_IS_IMPORT_DONE    0x7F001020
#define ACM_IOCTL_WRITE_DO_COPY     0x7F001021

/*move from drv_usb_if.h begin*/
#define USB_ACM_STATUS_NOTIFIER       0xA3
/*move from drv_usb_if.h end*/


#define ACM_DBG(args,...) do {\
    printk("%d:%s:",__LINE__,__func__);\
    printk(args, ##__VA_ARGS__);\
}while(0)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*USB模块FD type枚举*/
typedef enum tagFD_ID_T
{
    DEVICE_FD_NONE = 0,
    DEVICE_FD_3G_MODEM,
    DEVICE_FD_3G_ATCOM,
    DEVICE_FD_3G_OMCOM,
    DEVICE_FD_PCSC,
    DEVICE_FD_3G_GPS,
    DEVICE_FD_CTRL,
    DEVICE_FD_3G_ECM,
    DEVICE_FD_NDISDATA,
    DEVICE_FD_NDISCTRL,
    DEVICE_FD_SHELL,
    DEVICE_FD_FINGERPRINT,
    DEVICE_FD_ACMCTRL,
    DEVICE_FD_MMS,
    DEVICE_FD_3G_PCVOICE,
    DEVICE_FD_DVB,
    DEVICE_FD_MODEM,
    DEVICE_FD_ECM,
    DEVICE_FD_ATCOM,
    DEVICE_FD_OMCOM,
    DEVICE_FD_GPS,
    DEVICE_FD_PCVOICE,
    DEVICE_FD_NCM,
    DEVICE_FD_CDROM,
    DEVICE_FD_SD,
    DEVICE_FD_RNDIS,
    DEVICE_FD_LTE_OM,
    /* todo: Add Information */
    DEVICE_FD_NUM
} FD_ID_T;

enum LIDSC_IOCTL_CMD {
    LDISC_IOCTL_GET_RD_BUF = 0xFFEE0001,
    LDISC_IOCTL_RETURN_BUF,
    LDISC_IOCTL_GET_TTY,
    LDISC_IOCTL_INVALID_CMD
};

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/* USB ACM COM 私有数据 */
struct acm_ctx{
    int                   bopen;         /*设备打开标志位*/
    unsigned int          dev_id;        /*设备对应ID*/
    long                  fd;            /*对应tty设备的fd*/
    char                  *tty_name;     /*对应tty设备名*/
    ACM_READ_DONE_CB_T    readDoneCB;    /*上层注册的read完毕回调函数*/
    ACM_WRITE_DONE_CB_T   writeDoneCB;   /*上层注册的write回调函数*/
    ACM_FREE_CB_T         freeSendbuff;  /*上层注册的释放传入的写buff的回调函数*/
    ACM_EVENT_CB_T        eventCB;       /*上层注册的事件回调函数*/
    void                  *acm_ncpy;     /*用于零拷贝方案*/
    void                  *tty;          /*ACM对应tty设备*/
    void                  *acm_ldisc_ctx;
    int                   do_copy;       /*是否需要拷贝*/
    int                   cur_evt;       /*当前已上报事件*/
    int                   ncopy;         /*是否支持零拷贝方案*/
    int                   phyaddr_from_app; /*phyaddr使用上层传下来的物理地址*/
    struct work_struct	notify_work;
};

/* dev_id与tty设备路径映射表 */
struct dev_tty_map{
    unsigned int  dev_type;     /*设备type*/
    char          *path;        /*设备对应tty的路径*/
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

int acm_async_write(void *acm_dev, char *pVirAddr, char *pPhyAddr, unsigned int size);
void *acm_open(unsigned int dev_id);
int udi_acm_close(void *acm_dev);
int udi_acm_init(void);
int udi_acm_ioctl(void *acm_dev, int cmd, void *arg);
int udi_acm_open(UDI_OPEN_PARAM_S *param, UDI_HANDLE handle);
int udi_acm_read(void *acm_dev, unsigned char *pbuff, unsigned int size);
int udi_acm_write(void *acm_dev, unsigned char *pbuff, unsigned int size);











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of bsp_udi_adp.h */
