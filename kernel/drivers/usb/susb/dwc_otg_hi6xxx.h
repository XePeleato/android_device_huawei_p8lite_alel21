#ifndef _DWC_OTG_HI6xxx_
#define _DWC_OTG_HI6xxx_
#include <linux/wakelock.h>
#include <linux/notifier.h>
#include <linux/huawei/usb/hisi_susb.h>

#define usb_dbg(format, arg...)    \
    do {                 \
        printk(KERN_INFO "[USB][%s]"format, __func__, ##arg); \
    } while (0)
#define usb_err(format, arg...)    \
    do {                 \
        printk(KERN_ERR "[USB]"format, ##arg); \
    } while (0)

enum soc_sc_type {
    SOC_AO_CTRL = 0,
    SOC_SCTRL,
};

enum hisi_charger_type {
    CHARGER_TYPE_SDP = 0,   /* Standard Downstreame Port */
    CHARGER_TYPE_CDP,       /* Charging Downstreame Port */
    CHARGER_TYPE_DCP,       /* Dedicate Charging Port */
    CHARGER_TYPE_UNKNOWN,   /* non-standard */
    CHARGER_TYPE_NONE,      /* not connected */

    /* other messages */
    PLEASE_PROVIDE_POWER,   /* host mode, provide power */
};

enum otg_dev_event_type {
	CHARGER_CONNECT_EVENT = 0,
	CHARGER_DISCONNECT_EVENT,
	ID_RISE_EVENT,
	ID_FALL_EVENT,
	NONE_EVENT
};

#endif
