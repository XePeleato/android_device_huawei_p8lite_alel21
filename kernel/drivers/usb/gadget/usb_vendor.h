/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  usb_vendor.h
*
*   作    者 :  wangzhongshun
*
*   描    述 :  gadget usb驱动内接口头文件
*
*   修改记录 :  2013年2月2日  v1.00  wangzhongshun创建
*************************************************************************/

#ifndef __USB_VENDOR_H__
#define __USB_VENDOR_H__

#include <linux/notifier.h>

#define USB_NOTIF_PRIO_ADP      0           /* adp has lowest priority */
#define USB_NOTIF_PRIO_FD       100         /* function drvier */
#define USB_NOTIF_PRIO_CORE     200         /* usb core */
#define USB_NOTIF_PRIO_HAL      300         /* hardware has highest priority */

/*
 * usd to support usb event notify
 * connect pc:
 * insert(1) -> chargertype iden (2) -> enum_done(3) -> remove(0)/disable(-1) +
 *   ^___________________________________________________________|
 *
 * connect charger (must pmu support):
 * insert(1) -> remove(0) +
 *   ^____________________|
 */
#define USB_BALONG_DEVICE_INSERT    1
#define USB_BALONG_CHARGER_IDEN     2
#define USB_BALONG_ENUM_DONE        3
#define USB_BALONG_DEVICE_REMOVE    0
/* if the version is not support pmu detect
 * and all the device is disable, we assume that the usb is remove,
 */
#define USB_BALONG_DEVICE_DISABLE   (0xF1)

/*
 * charger type define
 */
#define USB_CHARGER_TYPE_HUAWEI         1
#define USB_CHARGER_TYPE_NOT_HUAWEI     2
#define USB_CHARGER_TYPE_INVALID        0

#define USB_IS_SUPPORT_NV               0
#define USB_IS_SUPPORT_OM_DUMP          0
#define USB_IS_SUPPORT_PMU              0
#define USB_IS_SUPPORT_VSHELL           0
#define USB_IS_SUPPORT_WAKELOCK         0
#define USB_IS_SUPPORT_DYN_INIT         0


#define USB_FD_DEVICE_MAX               32
#define USB_CDEV_NAME_MAX               64

#define GNET_MAX_NUM       (4)
#define GNET_USED_NUM      (1)


#if !defined(CONFIG_PC_USE_HUAWEI_DRIVER)
#define USB_IF_SUBCLASS_BALONG      0x02
#else /* PC_USE_HUAWEI_DRIVER */
#define USB_IF_SUBCLASS_BALONG      0x03
#endif


typedef enum tagUSB_PID_UNIFY_IF_PROT_T {
    USB_IF_PROTOCOL_VOID             = 0x00,
    USB_IF_PROTOCOL_3G_MODEM         = 0x01,
    USB_IF_PROTOCOL_3G_PCUI          = 0x02,
    USB_IF_PROTOCOL_3G_DIAG          = 0x03,
    USB_IF_PROTOCOL_PCSC             = 0x04,
    USB_IF_PROTOCOL_3G_GPS           = 0x05,
    USB_IF_PROTOCOL_CTRL             = 0x06,
    USB_IF_PROTOCOL_3G_NDIS          = 0x07,
    USB_IF_PROTOCOL_NDISDATA         = 0x08,
    USB_IF_PROTOCOL_NDISCTRL         = 0x09,
    USB_IF_PROTOCOL_BLUETOOTH        = 0x0A,
    USB_IF_PROTOCOL_FINGERPRINT      = 0x0B,
    USB_IF_PROTOCOL_ACMCTRL          = 0x0C,
    USB_IF_PROTOCOL_MMS              = 0x0D,
    USB_IF_PROTOCOL_3G_PCVOICE       = 0x0E,
    USB_IF_PROTOCOL_DVB              = 0x0F,
    USB_IF_PROTOCOL_MODEM            = 0x10,
    USB_IF_PROTOCOL_NDIS             = 0x11,
    USB_IF_PROTOCOL_PCUI             = 0x12,
    USB_IF_PROTOCOL_DIAG             = 0x13,
    USB_IF_PROTOCOL_GPS              = 0x14,
    USB_IF_PROTOCOL_PCVOICE          = 0x15,
    USB_IF_PROTOCOL_NCM              = 0x16,
    USB_IF_PROTOCOL_CDMA_LOG         = 0x17,
    USB_IF_PROTOCOL_CDROM            = 0xA1,
    USB_IF_PROTOCOL_SDRAM            = 0xA2,
    USB_IF_PROTOCOL_RNDIS            = 0xA3,

    USB_IF_PROTOCOL_NOPNP            = 0xFF
} USB_PID_UNIFY_IF_PROT_T;


typedef struct usb_enum_stat {
	char* fd_name;                  /* function drvier file name */
	unsigned usb_intf_id;           /* usb interface id */
	unsigned is_enum;               /* whether the dev is enum */
} usb_enum_stat_t;


/* charger type identify interface */
void bsp_usb_set_charger_type(int type);
int bsp_usb_get_charger_type(void);
int bsp_usb_is_support_charger(void);
int bsp_usb_is_support_pmu_detect(void);

/* notify interface */
void bsp_usb_register_notify(struct notifier_block *nb);
void bsp_usb_unregister_notify(struct notifier_block *nb);

/* usb status change interface*/
void bsp_usb_status_change(int status);
int usb_balong_init(void);
void usb_balong_exit(void);
int bsp_usb_console_init(void);


/* usb enum done interface */
void bsp_usb_init_enum_stat(void);
int bsp_usb_is_all_enum(void);
int bsp_usb_is_all_disable(void);
#define bsp_usb_add_setup_dev(intf_id) \
    bsp_usb_add_setup_dev_fdname(intf_id, __FILE__)
void bsp_usb_add_setup_dev_fdname(unsigned intf_id, char* fd_name);
void bsp_usb_set_enum_stat(unsigned intf_id, int enum_stat);
void bsp_usb_set_last_cdev_name(char* dev_name);


/* usb nv feature functions */
/* 1: support, 0:not support */
int bsp_usb_is_support_wwan(void);
int bsp_usb_is_support_shell(void);

#endif    /* __USB_VENDOR_H__ */
